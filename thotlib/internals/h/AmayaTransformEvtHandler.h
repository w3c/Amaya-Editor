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
			   void *CTM,
			   void *inverse,
			   int ancestorX, int ancestorY,
			   int canvasWidth, int canvasHeight,
			   int transform_type,
			   PtrBox box
			   );

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
  PtrBox m_box;
  int m_document;
  void *m_CTM;
  void *m_inverse;
  int m_x0,m_y0;
  int m_width,m_height;
  int m_type;
  Element m_el;
};

#endif /* __AMAYATRANSFORMEVTHANDLER_H__ */

#endif /* #ifdef _WX */
