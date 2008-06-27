#ifdef _WX

#ifndef __AMAYATRANSFORMEVTHANDLER_H__
#define __AMAYATRANSFORMEVTHANDLER_H__

#include "wx/wx.h"
#include "wx/event.h"

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typeint.h"

class AmayaFrame;

class AmayaTransformEvtHandler : public wxEvtHandler
{
 public:
  DECLARE_DYNAMIC_CLASS(AmayaTransformEvtHandler)

  AmayaTransformEvtHandler();
  AmayaTransformEvtHandler(AmayaFrame * p_frame,
			   Document doc,
			   void *transform,
			   int ancestorX, int ancestorY,
			   int canvasWidth, int canvasHeight,
			   Element el,
			   int transform_type,
			   int x1, int y1,
			   int x2, int y2,
			   int x3, int y3,
			   int x4, int y4);

  virtual ~AmayaTransformEvtHandler();

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
  int m_document;
  void *m_transform;
  int m_x0,m_y0;
  int m_width,m_height;
  Element m_el;
  int m_type;
  int m_x1, m_y1;
  int m_x2, m_y2;
  int m_x3, m_y3;
  int m_x4, m_y4;
};

#endif /* __AMAYATRANSFORMEVTHANDLER_H__ */

#endif /* #ifdef _WX */
