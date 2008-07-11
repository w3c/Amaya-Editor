#ifdef _WX

#ifndef __AMAYAEDITPATHEVTHANDLER_H__
#define __AMAYAEDITPATHEVTHANDLER_H__

#include "wx/wx.h"
#include "wx/event.h"

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typeint.h"

class AmayaFrame;

class AmayaEditPathEvtHandler : public wxEvtHandler
{
 public:
  DECLARE_DYNAMIC_CLASS(AmayaEditPathEvtHandler)

  AmayaEditPathEvtHandler();
  AmayaEditPathEvtHandler(AmayaFrame * p_frame,
			  Document doc,
			  void *inverse,
			  int ancestorX, int ancestorY,
			  int canvasWidth, int canvasHeight,
			  int edit_type,
			  Element element,
			  int point_number,
			  ThotBool *transformApplied
			  );

  virtual ~AmayaEditPathEvtHandler();

  bool IsFinish();

 protected:
  DECLARE_EVENT_TABLE()
  void OnChar( wxKeyEvent& event );
  void OnMouseMove( wxMouseEvent& event );
  void OnMouseWheel( wxMouseEvent& event );
  void OnMouseDbClick( wxMouseEvent& event );
  void OnMouseUp( wxMouseEvent& event );
  void OnMouseDown( wxMouseEvent& event );

 protected:
  bool Finished;

  AmayaFrame * pFrame;
  int FrameId;
  int document;
  void *CTM;
  void *inverse;
  int x0,y0;
  int width,height;
  int type;
  Element el;
  PtrBox box;

  /* Coordinates of the mouse */
  int lastX, lastY, mouse_x,mouse_y;

  bool ButtonDown;
  ThotBool *hasBeenTransformed; 

};

#endif /* __AMAYAEDITPATHEVTHANDLER_H__ */

#endif /* #ifdef _WX */
