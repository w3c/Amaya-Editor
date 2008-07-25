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
#include "viewapi_f.h"

#ifdef _GL
#include "glwindowdisplay.h"
#endif /* _GL */

#include "logdebug.h"


#include "AmayaFrame.h"
#include "AmayaCanvas.h"
#include "AmayaEditShapeEvtHandler.h"

static char shape;

IMPLEMENT_DYNAMIC_CLASS(AmayaEditShapeEvtHandler, wxEvtHandler)

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaEditShapeEvtHandler, wxEvtHandler)
EVT_KEY_DOWN ( AmayaEditShapeEvtHandler::OnKeyDown )
EVT_LEFT_DOWN(	AmayaEditShapeEvtHandler::OnMouseDown)
EVT_LEFT_UP(		AmayaEditShapeEvtHandler::OnMouseUp)
EVT_LEFT_DCLICK(	AmayaEditShapeEvtHandler::OnMouseDbClick)
EVT_MIDDLE_DOWN(	AmayaEditShapeEvtHandler::OnMouseDown)
EVT_MIDDLE_UP(	AmayaEditShapeEvtHandler::OnMouseUp)
EVT_MIDDLE_DCLICK(	AmayaEditShapeEvtHandler::OnMouseDbClick)
EVT_RIGHT_DOWN(	AmayaEditShapeEvtHandler::OnMouseDown)
EVT_RIGHT_UP(		AmayaEditShapeEvtHandler::OnMouseUp)
EVT_RIGHT_DCLICK(	AmayaEditShapeEvtHandler::OnMouseDbClick)
EVT_MOTION(		AmayaEditShapeEvtHandler::OnMouseMove)
EVT_MOUSEWHEEL(	AmayaEditShapeEvtHandler::OnMouseWheel)
END_EVENT_TABLE()

/*----------------------------------------------------------------------
 *----------------------------------------------------------------------*/
AmayaEditShapeEvtHandler::AmayaEditShapeEvtHandler() : wxEvtHandler()
{
}

/*----------------------------------------------------------------------
 *----------------------------------------------------------------------*/
AmayaEditShapeEvtHandler::AmayaEditShapeEvtHandler
(AmayaFrame * p_frame,
 Document document,
 void *inverse,
 int ancestorX,
 int ancestorY,
 int canvasWidth,
 int canvasHeight,
 Element element,
 int point,
 ThotBool *hasBeenEdited)
  : wxEvtHandler()
  ,finished(false)
  ,pFrame(p_frame)
  ,frameId(p_frame->GetFrameId())
  ,doc(document)
  ,inverse(inverse)
  ,x0(ancestorX)
  ,y0(ancestorY)
  ,width(canvasWidth)
  ,height(canvasHeight)
  ,el(element)
  ,point(point)
  ,hasBeenEdited(hasBeenEdited)
  ,leaf(NULL)
  ,box(NULL)
{
  PtrAbstractBox pAb;

  *hasBeenEdited = FALSE;
  buttonDown = false;

  if (pFrame)
    {
      /* attach this handler to the canvas */
      AmayaCanvas * p_canvas = pFrame->GetCanvas();
      p_canvas->PushEventHandler(this);

      /* assign a cross mouse cursor */
      pFrame->GetCanvas()->SetCursor( wxCursor(wxCURSOR_CROSS) );
      pFrame->GetCanvas()->CaptureMouse();
    }

  /* Get the GRAPHICS leaf */
  leaf = TtaGetFirstLeaf((Element)el);
  if(!leaf)
    {
    finished = true;
    return;
    }

  /* Get the box of the SVG element */
  pAb = ((PtrElement)leaf) -> ElAbstractBox[0];
  if(!pAb || !(pAb->AbBox))
    {
    finished = true;
    return;
    }

  box = pAb->AbBox;
  shape = pAb->AbShape;
}

/*----------------------------------------------------------------------
 *----------------------------------------------------------------------*/
AmayaEditShapeEvtHandler::~AmayaEditShapeEvtHandler()
{
  if (pFrame)
    {
      /* detach this handler from the canvas (restore default behaviour) */
      AmayaCanvas * p_canvas = pFrame->GetCanvas();
      p_canvas->PopEventHandler(false /* do not delete myself */);
      
      /* restore the default cursor */
      pFrame->GetCanvas()->SetCursor( wxNullCursor );
      pFrame->GetCanvas()->ReleaseMouse();
    }


}

/*----------------------------------------------------------------------
  IsFinish
 *----------------------------------------------------------------------*/
bool AmayaEditShapeEvtHandler::IsFinish()
{
  return finished;
}

/*----------------------------------------------------------------------
  OnKeyDown
 *----------------------------------------------------------------------*/
void AmayaEditShapeEvtHandler::OnKeyDown( wxKeyEvent& event )
{
  if(event.GetKeyCode() !=  WXK_SHIFT)
    {
      *hasBeenEdited = FALSE;
      finished = true;
    }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaEditShapeEvtHandler
 *      Method:  OnMouseDown
 * Description:  handle mouse button down events
 -----------------------------------------------------------------------*/
void AmayaEditShapeEvtHandler::OnMouseDown( wxMouseEvent& event )
{
}

/*----------------------------------------------------------------------
 *       Class:  AmayaEditShapeEvtHandler
 *      Method:  OnMouseUp
 * Description:  handle mouse button up events
 -----------------------------------------------------------------------*/
void AmayaEditShapeEvtHandler::OnMouseUp( wxMouseEvent& event )
{
  finished = true;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaEditShapeEvtHandler
 *      Method:  OnMouseDbClick
 * Description:  handle mouse dbclick events
 -----------------------------------------------------------------------*/
void AmayaEditShapeEvtHandler::OnMouseDbClick( wxMouseEvent& event )
{
  finished = true;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaEditShapeEvtHandler
 *      Method:  OnMouseMove
 * Description:  handle mouse move events
 -----------------------------------------------------------------------*/
void AmayaEditShapeEvtHandler::OnMouseMove( wxMouseEvent& event )
{
  int rx,ry,lx,ly;
  int x1,y1,x2,y2;
  int dx, dy;
  if (IsFinish())return;

  /* DELTA is the sensitivity toward mouse moves. */
#define DELTA 0

  /* Update the current mouse coordinates */
  mouse_x = event.GetX();
  mouse_y = event.GetY();

  if(!buttonDown)
    {
      lastX = mouse_x;
      lastY = mouse_y;
      buttonDown = true;
    }

  if((abs(mouse_x -lastX) + abs(mouse_y - lastY) > DELTA))
    {
      x1 = lastX;
      y1 = lastY;
      x2 = mouse_x;
      y2 = mouse_y;

      MouseCoordinatesToSVG(doc, pFrame,
			    x0, y0,
			    width, height,
			    inverse,
			    TRUE, &x1, &y1);

      MouseCoordinatesToSVG(doc, pFrame,
			    x0, y0,
			    width, height,
			    inverse,
			    TRUE, &x2, &y2);

      dx = x2 - x1;
      dy = y2 - y1;

      lx = box->BxW;
      ly = box->BxH;

      if(shape == 'C')
	{
	  rx = box->BxRx;
	  ry = box->BxRy;
	  if(ry == -1)ry = rx;
	}

      switch(point)
	{
	case 4:
	  lx+=dx;
	  if(shape == 'a')ly=lx;
	  break;
	  
	case 5:
	  lx+=dx;
	  ly+=dy;

	  if(shape == 'a')
	    {
	      if(lx < ly)ly=lx;
	      else lx = ly;
	    }

	  break;

	case 6:
	  ly+=dy;
	  if(shape == 'a')lx=ly;
	  break;

	case 9:
	  rx -= dx;
	  break;

	case 10:
	  ry += dy;
	  break;
	}

      if(lx < 0)lx = 0;
      if(ly < 0)ly = 0;

      if(shape == 'C')
	{
	  if(rx < 0)rx = 0;
	  if(ry < 0)ry = 0;
	  if(rx > lx/2)rx = lx/2;
	  if(ry > ly/2)ry = ly/2;

	  box->BxRx = rx;
	  if(box->BxRy != -1)box->BxRy = ry;
	}

      box->BxW = lx;
      box->BxH = ly;
      box->BxWidth = lx;
      box->BxHeight = ly;
      NewDimension (box->BxAbstractBox, lx, ly, frameId, FALSE);
      /*      NewDimension (((PtrElement)el)->ElAbstractBox[0],
	      lx, ly, frameId, FALSE);*/

#ifndef NODISPLAY
      /* Redisplay the GRAPHICS leaf */
            RedisplayLeaf ((PtrElement) leaf, doc, 0);
#endif

      /* Redisplay the SVG canvas to see the transform applied to the object */
       DefBoxRegion (frameId, box, -1, -1, -1, -1);
       RedrawFrameBottom (frameId, 0, NULL);
       pFrame->GetCanvas()->Refresh();

      /* Update the previous mouse coordinates */
      lastX = mouse_x;
      lastY = mouse_y;

      *hasBeenEdited = TRUE;
    }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaEditShapeEvtHandler
 *      Method:  OnMouseWheel
 * Description:  handle mouse wheel events
 -----------------------------------------------------------------------*/
void AmayaEditShapeEvtHandler::OnMouseWheel( wxMouseEvent& event )
{
}
#endif /* _WX */
