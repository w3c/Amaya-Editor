#ifdef _WX

#ifndef __AMAYAADDPOINTEVTHANDLER_H__
#define __AMAYAADDPOINTEVTHANDLER_H__

#include "wx/wx.h"
#include "wx/event.h"

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typeint.h"

class AmayaFrame;

class AmayaAddPointEvtHandler : public wxEvtHandler
{
 public:
  DECLARE_DYNAMIC_CLASS(AmayaAddPointEvtHandler)

  AmayaAddPointEvtHandler();
  AmayaAddPointEvtHandler(AmayaFrame * p_frame,
			  int x, int y, int x1, int y1, int x3,
			  int y3, int lastx, int lasty, int point,
			  int *nbpoints, int maxPoints, int width, int height,
			  PtrTextBuffer Pbuffer, PtrTextBuffer Bbuffer
			  );

  virtual ~AmayaAddPointEvtHandler();

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

  float m_RatioX;
  float m_RatioY;

  int m_X;
  int m_Y;
  int m_X1;
  int m_Y1;
  int m_X3;
  int m_Y3;
  int m_LastX;
  int m_LastY;
  int m_Point;
  int *m_NbPoints;
  int m_MaxPoints;
  int m_Width;
  int m_Height;
  PtrTextBuffer m_Pbuffer;
  PtrTextBuffer m_Bbuffer;
};

#endif // __AMAYAADDPOINTEVTHANDLER_H__

#endif /* #ifdef _WX */
