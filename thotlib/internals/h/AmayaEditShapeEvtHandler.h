#ifdef _WX

#ifndef __AMAYAEDITSHAPEEVTHANDLER_H__
#define __AMAYAEDITSHAPEEVTHANDLER_H__

#include "wx/wx.h"
#include "wx/event.h"

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typeint.h"

class AmayaFrame;

class AmayaEditShapeEvtHandler : public wxEvtHandler
{
 public:
  DECLARE_DYNAMIC_CLASS(AmayaEditShapeEvtHandler)

  AmayaEditShapeEvtHandler();
  AmayaEditShapeEvtHandler(
			     AmayaFrame * p_frame,
			     Document doc,
			     void *inverse,
			     int ancestorX,
			     int ancestorY,
			     int canvasWidth,
			     int canvasHeight,
			     Element el,
			     int point_number,
			     ThotBool *hasBeenEdited);
  virtual ~AmayaEditShapeEvtHandler();

  bool IsFinish();

 protected:
  DECLARE_EVENT_TABLE()
  void OnKeyDown( wxKeyEvent& event );
  void OnMouseMove( wxMouseEvent& event );
  void OnMouseWheel( wxMouseEvent& event );
  void OnMouseDbClick( wxMouseEvent& event );
  void OnMouseUp( wxMouseEvent& event );
  void OnMouseDown( wxMouseEvent& event );

 protected:
  bool finished;

  AmayaFrame * pFrame;
  int frameId;
  int doc;
  void *inverse;
  int x0, y0, width, height;
  int point;
  ThotBool *hasBeenEdited;

};

#endif /* __AMAYAEDITSHAPEEVTHANDLER_H__ */

#endif /* #ifdef _WX */
