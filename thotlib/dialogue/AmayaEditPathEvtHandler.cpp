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
						 void *CTM,
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
  ,CTM(CTM)
  ,inverse(inverse)
  ,x0(ancestorX)
  ,y0(ancestorY)
  ,width(canvasWidth)
  ,height(canvasHeight)
  ,type(edit_type)
  ,el(element)
  ,box(NULL)
  ,hasBeenTransformed(transformApplied)
{
  PtrAbstractBox pAb;

  *hasBeenTransformed = FALSE;

  pAb = ((PtrElement)el) -> ElAbstractBox[0];
  if(!pAb && pAb -> AbBox)
    {
    Finished = true;
    return;
    }

  box = pAb -> AbBox;

  if (pFrame)
    {
      /* attach this handler to the canvas */
      AmayaCanvas * p_canvas = pFrame->GetCanvas();
      p_canvas->PushEventHandler(this);

      /* assign a cross mouse cursor */
      pFrame->GetCanvas()->SetCursor( wxCursor(wxCURSOR_CROSS) );
      pFrame->GetCanvas()->CaptureMouse();

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
      
      /* restore the default cursor */
      pFrame->GetCanvas()->SetCursor( wxNullCursor );
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

  /* DELTA is the sensitivity toward mouse moves. */
#define DELTA 20

  /* Update the current mouse coordinates */
  mouse_x = event.GetX();
  mouse_y = event.GetY();

  if((abs(mouse_x -lastX) + abs(mouse_y - lastY) > DELTA))
    {
      /* The user is pressing the mouse button and moving */

      /* Translate the previous and current coordinates of the mouse
	 into the correspoint of the SVG canvas:
	 - previous position: (x1,y1)
	 - new position: (x2,y2)
      */
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

      switch(type)
	{
	default:
	  *hasBeenTransformed = TRUE;
	  break;
	}

      /* Redisplay the SVG canvas to see the transform applied to the object */
      DefBoxRegion (FrameId, box, -1, -1, -1, -1);
      RedrawFrameBottom (FrameId, 0, NULL);
      pFrame->GetCanvas()->Refresh();

      /* Update the previous mouse coordinates */
      lastX = mouse_x;
      lastY = mouse_y;
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
