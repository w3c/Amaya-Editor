#ifdef _WX

#ifndef __AMAYACREATESHAPEEVTHANDLER_H__
#define __AMAYACREATESHAPEEVTHANDLER_H__

#include "wx/wx.h"
#include "wx/event.h"

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typeint.h"

class AmayaFrame;

class AmayaCreateShapeEvtHandler : public wxEvtHandler
{
 public:
  DECLARE_DYNAMIC_CLASS(AmayaCreateShapeEvtHandler)

  AmayaCreateShapeEvtHandler();
  AmayaCreateShapeEvtHandler(AmayaFrame * p_frame,
 Document doc, void *transform,
 int ancestorX,
 int ancestorY,
 int canvasWidth,
 int canvasHeight,
 int shape_number,
 int *x1, int *y1, int *x2, int *y2,
 int *nb_points);
  virtual ~AmayaCreateShapeEvtHandler();

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
  int m_x0, m_y0, m_width, m_height;
  int m_ShapeNumber;
  int *m_x1,*m_y1,*m_x2,*m_y2;
  int *m_NbPoints;

};

#endif /* __AMAYACREATESHAPEEVTHANDLER_H__ */

#endif /* #ifdef _WX */
