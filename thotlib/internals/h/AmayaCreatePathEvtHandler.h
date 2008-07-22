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
  AmayaCreatePathEvtHandler(AmayaFrame * p_frame, int x_min, int y_min,
			    int x_max, int y_max, PtrTextBuffer buffer,
			    Document doc, int shape_number, int *nbPoints,
			    ThotBool *created);

  virtual ~AmayaCreatePathEvtHandler();

  bool IsFinish();

 protected:
  DECLARE_EVENT_TABLE()
  void OnChar( wxKeyEvent& event );
  void OnMouseMove( wxMouseEvent& event );
  void OnMouseWheel( wxMouseEvent& event );
  void OnMouseDbClick( wxMouseEvent& event );
  void OnMouseDown( wxMouseEvent& event );
  void OnMouseRightDown( wxMouseEvent& event );

  void DrawLine (int x2, int y2, int x3, int y3, ThotBool specialColor);
  void DrawQuadraticBezier (int x1, int y1, int x2, int y2, int x3, int y3,
			    ThotBool specialColor);
  void DrawCubicBezier (int x1, int y1, int x2, int y2,
			int x3, int y3, int x4, int y4,
			ThotBool specialColor);
  void UpdateSymetricPoint();
  void DrawControlPoints ();
  void DrawPathFragment(int shape, ThotBool specialColor);

 protected:
  bool finished;

  AmayaFrame * pFrame;
  int frameId;
  int x0, y0, width,height;
  int shape;

  PtrTextBuffer pBuffer;
  int document;
  ThotBool *created;

  int *nb_points, state;
  int currentX,currentY;
  int lastX1, lastY1;
  int symX, symY;
  int lastX2, lastY2;
  int lastX3, lastY3;
  ThotBool clear;
};

#endif /* __AMAYACREATEPATHEVTHANDLER_H__ */

#endif /* #ifdef _WX */
