#ifdef _WX

#ifndef __AMAYACREATEPATHEVTHANDLER_H__
#define __AMAYACREATEPATHEVTHANDLER_H__

#include "wx/wx.h"
#include "wx/event.h"

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typeint.h"

class AmayaFrame;

class AmayaCreatePathEvtHandler : public wxEvtHandler
{
 public:
  DECLARE_DYNAMIC_CLASS(AmayaCreatePathEvtHandler)

  AmayaCreatePathEvtHandler();
  AmayaCreatePathEvtHandler(AmayaFrame * p_frame, int x_min, int y_min, int x_max, int y_max, PtrTextBuffer Pbuffer, Document doc, int shape_number);

  virtual ~AmayaCreatePathEvtHandler();

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
  int m_xmin, m_ymin, m_xmax, m_ymax;

  int m_ShapeNumber;

  PtrTextBuffer m_Pbuffer;
  int m_document;

};

#endif /* __AMAYACREATEPATHEVTHANDLER_H__ */

#endif /* #ifdef _WX */
