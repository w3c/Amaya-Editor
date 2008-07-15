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
static void GetSymetricPoint(int x, int y, int x0, int y0,
			     int *symx, int *symy)
{
  *symx = 2*x0 - x;
  *symy = 2*y0 - y;
}

extern void GetPositionAndSizeInParentSpace(Document doc, Element el, float *X,
					    float *Y, float *width,
					    float *height);

static  PtrPathSeg          pPaCurrent, pPaPrevious, pPaNext;
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
						 Element element,
						 int point_number,
						 int *n_path_segments,
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
  ,type(-1)
  ,el(element)
  ,box(NULL)
  ,ButtonDown(false)
  ,Nseg(n_path_segments)
  ,hasBeenTransformed(transformApplied)
{
  PtrAbstractBox pAb;
  int i,j;
  PtrPathSeg          pPa, pPaStart;

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
	      type = 6;
	      i_poly = j;
	      break;
	    }
	  
	  j++;
	}
    }
  else if(((PtrElement)leaf)->ElLeafType == LtPath)
    {
      /* It's a path: find the segment to edit */
      pPaPrevious = NULL;
      pPaNext = NULL;

      i = 1;
      *Nseg = 0;

      pPa = ((PtrElement)leaf)->ElFirstPathSeg;
      if(pPa == NULL)
	{
	  Finished = true;
	  return;
	}
  
      while (pPa)
	{
	  if ((pPa->PaNewSubpath || !pPa->PaPrevious))
	    {
	      /* this path segment starts a new subpath */
	      pPaStart = pPa;
	      
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
		  if(pPa->PaNewSubpath || !pPa->PaPrevious)
		    {
		      /* Moving the start control point of a subpath */
		      type = 1;
		      break;
		    }
		  else
		    {
		      /* Moving a start control point */
		      type = 2;
		      pPaPrevious = pPa->PaPrevious;
		      break;
		    }
		}
	      i++;

	      if(i == point_number)
		{
		  if(pPa->PaNext && !(pPa->PaNext->PaNewSubpath))
		      /* Moving an end control point */
		    {
		      type = 3;
		      pPaNext = pPa->PaNext;
		      break;
		    }
		  else
		    /* Moving the end control point of the last point
		       of a subpath*/
		    {
		      type = 7;
		      break;
		    }
		}
	      i++;
	    }

	  if(i == point_number)
	    {
	      if(pPa->PaNext && !(pPa->PaNext->PaNewSubpath))
		{
		  /* Moving a point in the path */
		  type = 4;
		  pPaNext = pPa->PaNext;
		  break;
		}
	      else
	      {
		  /* Moving the last point of a subpath */
		  type = 5;
		  break;
		}
	    }
	  
	  i++;
	  pPa = pPa->PaNext;
	  (*Nseg)++;
	}

      pPaCurrent = pPa;

      if(pPaCurrent)
	{
	  if(type == 0 || type == 1)
	    {
	      /* We want to edit the first point of a subpath:
		 is the last point at the same position? */
	      while(pPa)
		{
		  if(!pPa->PaNext || pPa->PaNext->PaNewSubpath)
		    {
		      if(pPa->XEnd == pPaCurrent->XStart &&
			 pPa->YEnd == pPaCurrent->YStart)
			  pPaPrevious = pPa;

		      break;
		    }
		  
		  pPa = pPa->PaNext;
		  (*Nseg)++;
		}
	    }
	  else if(type == 7 || type == 5)
	    {
	      /* We want to edit the last point of a subpath:
		 is the first point at the same position? */
	      if(pPaStart && 
		 pPaStart->XStart == pPaCurrent->XEnd &&
		 pPaStart->YStart == pPaCurrent->YEnd)
		pPaNext = pPaStart;
	    }
	}
      
      /* Go to the end of the path to see how many segment there are */
      while (pPa)
	{
	  pPa = pPa->PaNext;
	  (*Nseg)++;
	}

      /* Convert Quadratic Bezier to Cubic */
      if(pPaCurrent->PaShape == PtQuadraticBezier)
	pPaCurrent->PaShape = PtCubicBezier;
	  
      if(pPaNext && pPaNext->PaShape == PtQuadraticBezier)
	pPaNext->PaShape = PtCubicBezier;

      if(pPaPrevious && pPaPrevious->PaShape == PtQuadraticBezier)
	pPaPrevious->PaShape = PtCubicBezier;
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
	  /* Moving a start point */
	  pPaCurrent->XStart += dx;
	  pPaCurrent->YStart += dy;
	  pPaCurrent->XCtrlStart += dx;
	  pPaCurrent->YCtrlStart += dy;

	  if(pPaPrevious)
	    {
	      pPaPrevious->XEnd += dx;
	      pPaPrevious->YEnd += dy;
	      pPaPrevious->XCtrlEnd += dx;
	      pPaPrevious->YCtrlEnd += dy;
	      }
	  break;

	case 1:
	  /* Moving the start control point of a new subpath */
	case 2:
	  /* Moving the start control point */
	  pPaCurrent->XCtrlStart += dx;
	  pPaCurrent->YCtrlStart += dy;

	  if(smooth && pPaPrevious &&
	     (pPaPrevious->PaShape == PtCubicBezier ||
	      pPaPrevious->PaShape == PtQuadraticBezier))
	    {
	      /* Update the other control point */
	      GetSymetricPoint(pPaCurrent->XCtrlStart,
			       pPaCurrent->YCtrlStart,
			       pPaCurrent->XStart,
			       pPaCurrent->YStart,

			       &(pPaPrevious->XCtrlEnd),
			       &(pPaPrevious->YCtrlEnd));
	    }

	  break;

	case 3:
	  /* Moving the end control point */

	case 7:
	  /* Moving the end control point of the last point
	     of a subpath*/


	  pPaCurrent->XCtrlEnd += dx;
	  pPaCurrent->YCtrlEnd += dy;

	  if(smooth && pPaNext && 
	     (pPaNext->PaShape == PtCubicBezier ||
	      pPaNext->PaShape == PtQuadraticBezier))
	    {
	      /* Update the other control point */
	      GetSymetricPoint(pPaCurrent->XCtrlEnd,
			       pPaCurrent->YCtrlEnd,
			       pPaCurrent->XEnd,
			       pPaCurrent->YEnd,

			       &(pPaNext->XCtrlStart),
			       &(pPaNext->YCtrlStart));
	    }

	  break;

	case 4:
	  /* Moving a point in the path */
	case 5:
	  /* Moving the last point of a subpath */
	  pPaCurrent->XCtrlEnd += dx;
	  pPaCurrent->YCtrlEnd += dy;
	  pPaCurrent->XEnd += dx;
	  pPaCurrent->YEnd += dy;

	  if(pPaNext)
	    {
	      pPaNext->XStart += dx;
	      pPaNext->YStart += dy;
	      pPaNext->XCtrlStart += dx;
	      pPaNext->YCtrlStart += dy;
	    }
	  break;

	case 6:
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
