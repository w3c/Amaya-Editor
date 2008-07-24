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

#ifdef _GL
#include "glwindowdisplay.h"
#endif /* _GL */

#include "logdebug.h"


#include "AmayaFrame.h"
#include "AmayaCanvas.h"
#include "AmayaEditShapeEvtHandler.h"

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
 Document doc,
 void *inverse,
 int ancestorX,
 int ancestorY,
 int canvasWidth,
 int canvasHeight,
 Element el,
 int point,
 ThotBool *hasBeenEdited)
  : wxEvtHandler()
  ,finished(false)
  ,pFrame(p_frame)
  ,frameId(p_frame->GetFrameId())
  ,doc(doc)
  ,inverse(inverse)
  ,x0(ancestorX)
  ,y0(ancestorY)
  ,width(canvasWidth)
  ,height(canvasHeight)
  ,point(point)
  ,hasBeenEdited(hasBeenEdited)
{
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
