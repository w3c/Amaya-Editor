#ifdef _WX

#include "wx/wx.h"
#include "wx/event.h"

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"
#include "libmsg.h"
#include "message.h"
#include "typeint.h"
#include "undo.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "boxes_tv.h"
#include "frame_tv.h"

#include "frame_f.h"
#include "presentationapi_f.h"
#include "structcreation_f.h"
#include "boxlocate_f.h"
#include "geom_f.h"
#include "font_f.h"
#include "content_f.h"
#include "windowdisplay_f.h"
#include "viewapi_f.h"

#ifdef _GL
#include "glwindowdisplay.h"
#endif /* _GL */

#include "logdebug.h"


#include "AmayaFrame.h"
#include "AmayaCanvas.h"
#include "AmayaEditPathEvtHandler.h"

/*----------------------------------------------------------------------
 *----------------------------------------------------------------------*/

extern void GetPositionAndSizeInParentSpace(Document doc, Element el, float *X,
					    float *Y, float *width,
					    float *height);

static  PtrPathSeg          pPa;
static  PtrTextBuffer       pBuffer;
static  int i_poly;
static  Element leaf;

IMPLEMENT_DYNAMIC_CLASS(AmayaEditPathEvtHandler, wxEvtHandler)

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaEditPathEvtHandler, wxEvtHandler)
EVT_KEY_DOWN( AmayaEditPathEvtHandler::OnChar )
EVT_LEFT_DOWN(AmayaEditPathEvtHandler::OnMouseDown) 
EVT_LEFT_UP(AmayaEditPathEvtHandler::OnMouseUp)
EVT_LEFT_DCLICK(AmayaEditPathEvtHandler::OnMouseDbClick)
EVT_MIDDLE_DOWN(AmayaEditPathEvtHandler::OnMouseDown)
EVT_MIDDLE_UP(AmayaEditPathEvtHandler::OnMouseUp)
EVT_MIDDLE_DCLICK(AmayaEditPathEvtHandler::OnMouseDbClick)
EVT_RIGHT_DOWN(AmayaEditPathEvtHandler::OnMouseDown)
EVT_RIGHT_UP(AmayaEditPathEvtHandler::OnMouseUp)
EVT_RIGHT_DCLICK(AmayaEditPathEvtHandler::OnMouseDbClick)
EVT_MOTION(AmayaEditPathEvtHandler::OnMouseMove)
EVT_MOUSEWHEEL(AmayaEditPathEvtHandler::OnMouseWheel)
END_EVENT_TABLE()

/*----------------------------------------------------------------------
 *----------------------------------------------------------------------*/
AmayaEditPathEvtHandler::AmayaEditPathEvtHandler() : wxEvtHandler()
{
}

/*----------------------------------------------------------------------
 *----------------------------------------------------------------------*/
AmayaEditPathEvtHandler::AmayaEditPathEvtHandler(AmayaFrame * p_frame,
						 Document doc,
						 void *inverse,
						 int ancestorX,
						 int ancestorY,
						 int canvasWidth,
						 int canvasHeight,
						 int edit_type,
						 Element element,
						 int point_number,
						 ThotBool *transformApplied)
  :wxEvtHandler()
  ,Finished(false)
  ,pFrame(p_frame)
  ,FrameId(p_frame->GetFrameId())
  ,document(doc)
  ,inverse(inverse)
  ,x0(ancestorX)
  ,y0(ancestorY)
  ,width(canvasWidth)
  ,height(canvasHeight)
  ,type(edit_type)
  ,el(element)
  ,box(NULL)
  ,ButtonDown(false)
  ,hasBeenTransformed(transformApplied)
{
  PtrAbstractBox pAb;
  int i,j;

  *hasBeenTransformed = FALSE;

  if (pFrame)
    {
      /* attach this handler to the canvas */
      AmayaCanvas * p_canvas = pFrame->GetCanvas();
      p_canvas->PushEventHandler(this);
      pFrame->GetCanvas()->CaptureMouse();

    }

  pAb = ((PtrElement)el) -> ElAbstractBox[0];
  if(!pAb && pAb -> AbBox)
    {
    Finished = true;
    return;
    }
  box = pAb -> AbBox;

  /* Get the GRAPHICS leaf */
  leaf = TtaGetFirstLeaf((Element)el);
  if(!leaf)
    {
    Finished = true;
    return;
    }

  if(((PtrElement)leaf)->ElLeafType == LtPolyLine)
    {
      /* It's a polyline or a polygon */

      pBuffer = ((PtrElement)leaf)->ElPolyLineBuffer;
      j = 1;
      for (i = 1; pBuffer; i++)
	{
	  if (j >= pBuffer->BuLength)
	    {
	      if (pBuffer->BuNext != NULL)
		{
		  /* Changement de buffer */
		  pBuffer = pBuffer->BuNext;
		  j = 0;
		}
	    }
	  
	  if(i == point_number)
	    {
	      /* Moving a point of a polyline/polygon  */
	      type = 5;
	      i_poly = j;
	      break;
	    }
	  
	  j++;
	}
    }
  else if(((PtrElement)leaf)->ElLeafType == LtPath)
    {
      /* It's a path: find the segment to edit */

      i = 1;
      
      pPa = ((PtrElement)leaf)->ElFirstPathSeg;
  
      while (pPa)
	{
	  if ((pPa->PaNewSubpath || !pPa->PaPrevious))
		{
		  if(i == point_number)
		    {
		      /* Moving the first point of subpath */
		      type = 0;
		      break;
		    }


		  i++;
                }

	      if(pPa->PaShape == PtCubicBezier ||
		 pPa->PaShape == PtQuadraticBezier)
		{
		  if(i == point_number)
		    {
		      /* Moving the start control point */
		      type = 1;
		      break;
		    }
	  
		  i++;
		}

	      if(pPa->PaShape == PtCubicBezier ||
		 pPa->PaShape == PtQuadraticBezier)
		{
		  if(i == point_number)
		    {
		      /* Moving the end control point */
		      type = 2;
		      break;
		    }
		  i++;
		}

	      if(i == point_number)
		{
		  if(pPa->PaNext && !(pPa->PaNext->PaNewSubpath))
		    {
		      /* Moving a point in the path */
		      type = 3;
		      break;
		    }
		  else
		    {
		      /* Moving the last point of a subpath */
		      type = 4;
		      break;
		    }
		}

	      i++;
              pPa = pPa->PaNext;
	      
            }
    }
  else 
    {
      Finished = true;
      return;
    }
}

/*----------------------------------------------------------------------
 *----------------------------------------------------------------------*/
AmayaEditPathEvtHandler::~AmayaEditPathEvtHandler()
{
  if (pFrame)
    {
      /* detach this handler from the canvas (restore default behaviour) */
      AmayaCanvas * p_canvas = pFrame->GetCanvas();
      p_canvas->PopEventHandler(false /* do not delete myself */);
      pFrame->GetCanvas()->ReleaseMouse();
    }

}

/*----------------------------------------------------------------------
 *----------------------------------------------------------------------*/
bool AmayaEditPathEvtHandler::IsFinish()
{
  return Finished;
}

/*----------------------------------------------------------------------
 *----------------------------------------------------------------------*/
void AmayaEditPathEvtHandler::OnChar( wxKeyEvent& event )
{
  if(event.GetKeyCode() !=  WXK_SHIFT)
    Finished = true;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaEditPathEvtHandler
 *      Method:  OnMouseDown
 * Description:  handle mouse button down events
 -----------------------------------------------------------------------*/
void AmayaEditPathEvtHandler::OnMouseDown( wxMouseEvent& event )
{
  if (IsFinish())return;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaEditPathEvtHandler
 *      Method:  OnMouseUp
 * Description:  handle mouse button up events
 -----------------------------------------------------------------------*/
void AmayaEditPathEvtHandler::OnMouseUp( wxMouseEvent& event )
{
  if (IsFinish())return;

  Finished = true;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaEditPathEvtHandler
 *      Method:  OnMouseDbClick
 * Description:  handle mouse dbclick events
 -----------------------------------------------------------------------*/
void AmayaEditPathEvtHandler::OnMouseDbClick( wxMouseEvent& event )
{
  Finished = true;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaEditPathEvtHandler
 *      Method:  OnMouseMove
 * Description:  handle mouse move events
 -----------------------------------------------------------------------*/
void AmayaEditPathEvtHandler::OnMouseMove( wxMouseEvent& event )
{
  int x1,y1,x2,y2;
  int dx, dy;
  ThotBool smooth = TRUE;

  if (IsFinish())return;

  if(event.ShiftDown())smooth = FALSE;

  /* DELTA is the sensitivity toward mouse moves. */
#define DELTA 20

  /* Update the current mouse coordinates */
  mouse_x = event.GetX();
  mouse_y = event.GetY();

  if(!ButtonDown)
    {
      lastX = mouse_x;
      lastY = mouse_y;
      ButtonDown = true;
    }

  if((abs(mouse_x -lastX) + abs(mouse_y - lastY) > DELTA))
    {
      /* The user is pressing the mouse button and moving */
      x1 = lastX;
      y1 = lastY;
      x2 = mouse_x;
      y2 = mouse_y;

      MouseCoordinatesToSVG(document, pFrame,
			    x0, y0,
			    width, height,
			    inverse,
			    TRUE, &x1, &y1);

      MouseCoordinatesToSVG(document, pFrame,
			    x0, y0,
			    width, height,
			    inverse,
			    TRUE, &x2, &y2);

      dx = x2 - x1;
      dy = y2 - y1;

      switch(type)
	{
	case 0:
	  /* Moving the start control point */
	  pPa->XStart += dx;
	  pPa->YStart += dy;
	  pPa->XCtrlStart += dx;
	  pPa->YCtrlStart += dy;
	  break;

	case 1:
	  /* Moving the start control point */
	  pPa->XCtrlStart += dx;
	  pPa->YCtrlStart += dy;

	  if(smooth && pPa->PaPrevious && !(pPa->PaNewSubpath) &&
	     (pPa->PaPrevious->PaShape == PtCubicBezier ||
	      pPa->PaPrevious->PaShape == PtQuadraticBezier))
	    {
	      /* Update the other control point */
	      pPa->PaPrevious->XCtrlEnd = 2*pPa->XStart - pPa->XCtrlStart;
	      pPa->PaPrevious->YCtrlEnd = 2*pPa->YStart - pPa->YCtrlStart;
	    }

	  break;

	case 2:
	  /* Moving the end control point */
	  pPa->XCtrlEnd += dx;
	  pPa->YCtrlEnd += dy;

	  if(smooth && pPa->PaNext && !(pPa->PaNext->PaNewSubpath) &&
	     (pPa->PaNext->PaShape == PtCubicBezier ||
	      pPa->PaNext->PaShape == PtQuadraticBezier))
	    {
	      /* Update the other control point */
	      pPa->PaNext->XCtrlStart = 2*pPa->XEnd - pPa->XCtrlEnd;
	      pPa->PaNext->YCtrlStart = 2*pPa->YEnd - pPa->YCtrlEnd;
	    }

	  break;

	case 3:
	  /* Moving a point in the path */
	  pPa->XCtrlEnd += dx;
	  pPa->YCtrlEnd += dy;
	  pPa->XEnd += dx;
	  pPa->YEnd += dy;
	  pPa->PaNext->XStart += dx;
	  pPa->PaNext->YStart += dy;
	  pPa->PaNext->XCtrlStart += dx;
	  pPa->PaNext->YCtrlStart += dy;

	  break;

	case 4:
	  /* Moving the last point of a subpath */
	  pPa->XCtrlEnd += dx;
	  pPa->YCtrlEnd += dy;
	  pPa->XEnd += dx;
	  pPa->YEnd += dy;
	  break;

	case 5:
	  /* Moving a point of a polyline/polygon  */
	  pBuffer->BuPoints[i_poly].XCoord += dx;
	  pBuffer->BuPoints[i_poly].YCoord += dy;
	  break;

	default:
	  break;
	}

#ifndef NODISPLAY
	  RedisplayLeaf ((PtrElement) leaf, document, 0);
#endif

      /* Redisplay the SVG canvas to see the transform applied to the object */
      DefBoxRegion (FrameId, box, -1, -1, -1, -1);
      RedrawFrameBottom (FrameId, 0, NULL);
      pFrame->GetCanvas()->Refresh();
      /* Update the previous mouse coordinates */
      lastX = mouse_x;
      lastY = mouse_y;

      *hasBeenTransformed = TRUE;
    }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaEditPathEvtHandler
 *      Method:  OnMouseWheel
 * Description:  handle mouse wheel events
 -----------------------------------------------------------------------*/
void AmayaEditPathEvtHandler::OnMouseWheel( wxMouseEvent& event )
{
}
#endif /* _WX */
