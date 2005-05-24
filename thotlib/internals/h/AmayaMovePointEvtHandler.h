#ifdef _WX

#ifndef __AMAYAMOVEPOINTEVTHANDLER_H__
#define __AMAYAMOVEPOINTEVTHANDLER_H__

#include "wx/wx.h"
#include "wx/event.h"

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typeint.h"

class AmayaFrame;

class AmayaMovePointEvtHandler : public wxEvtHandler
{
 public:
  DECLARE_DYNAMIC_CLASS(AmayaMovePointEvtHandler)

  AmayaMovePointEvtHandler();
  AmayaMovePointEvtHandler(AmayaFrame * p_frame,
                           PtrBox box,
                           int firstx, int firsty,
                           int x1, int y1, int x3, int y3,
                           int lastx, int lasty, int point,
                           int x, int y, int width, int height,
                           PtrTextBuffer Pbuffer, PtrTextBuffer Bbuffer,
                           int pointselect);

  virtual ~AmayaMovePointEvtHandler();

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
  int m_Width;
  int m_Height;

  int m_X1;
  int m_Y1;
  int m_X3;
  int m_Y3;

  int m_FirstX;
  int m_FirstY;
  int m_LastX;
  int m_LastY;

  int m_Point;
  int m_PointSelect;
  PtrTextBuffer m_Pbuffer;
  PtrTextBuffer m_Bbuffer;
  PtrBox        m_Box;
  
  int * m_pXCoord;
  int * m_pYCoord;
};

#endif // __AMAYAMOVEPOINTEVTHANDLER_H__

#endif /* #ifdef _WX */
