#ifdef _WX

#ifndef __AMAYATRANSFORMEVTHANDLER_H__
#define __AMAYATRANSFORMEVTHANDLER_H__

#include "wx/wx.h"
#include "wx/event.h"

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typeint.h"
#include "content.h"

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
			   Element element,
			   ThotBool *transformApplied
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
  void DrawRotationCenter();
  void DrawSkewingArrows();
  void DrawScalingArrows();
  void UpdatePositions();
  bool IsNear(int x, int y);

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

  /* Coordinates of the center of rotation */
  int cx2, cy2;
  float cx, cy;

  /* Coordinates of the box */
  int top2, left2, bottom2, right2, size;
  float top, left, bottom, right;

  /* Mouse Button Down */
  bool ButtonDown;
  bool hasBeenRotated;
  ThotBool *hasBeenTransformed; 

};
#endif /* __AMAYATRANSFORMEVTHANDLER_H__ */

#endif /* #ifdef _WX */
