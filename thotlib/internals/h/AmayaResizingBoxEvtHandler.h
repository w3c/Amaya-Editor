#ifdef _WX

#ifndef __AMAYARESIZINGBOXEVTHANDLER_H__
#define __AMAYARESIZINGBOXEVTHANDLER_H__

#include "wx/wx.h"
#include "wx/event.h"

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typeint.h"

class AmayaFrame;

class AmayaResizingBoxEvtHandler : public wxEvtHandler
{
 public:
  DECLARE_DYNAMIC_CLASS(AmayaResizingBoxEvtHandler)

  AmayaResizingBoxEvtHandler();
  AmayaResizingBoxEvtHandler( AmayaFrame * p_frame,
                              int x, int y, int *width, int *height,
                              PtrBox box, int xmin, int xmax, int ymin, int ymax,
                              int xm, int ym, int percentW, int percentH );

  virtual ~AmayaResizingBoxEvtHandler();

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
  bool m_IsFinish;

  AmayaFrame * m_pFrame;
  int m_FrameId;

  int m_pX;
  int m_pY;
  int *m_pWidth;
  int *m_pHeight;
  PtrBox m_Box;
  int m_Xmin;
  int m_Xmax;
  int m_Ymin;
  int m_Ymax;
  int m_Xm;
  int m_Ym;
  int m_Xref;
  int m_Yref;
  int m_HDirection;
  int m_VDirection;
  bool m_IsEllipse;
};

#endif // __AMAYARESIZINGBOXEVTHANDLER_H__

#endif /* #ifdef _WX */
