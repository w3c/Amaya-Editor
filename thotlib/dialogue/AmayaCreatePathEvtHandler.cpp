#ifdef _WX

#include "wx/wx.h"
#include "wx/event.h"

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"
#include "libmsg.h"
#include "message.h"
#include "typeint.h"
#include "undo.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "boxes_tv.h"
#include "frame_tv.h"

#include "frame_f.h"
#include "presentationapi_f.h"
#include "structcreation_f.h"
#include "boxlocate_f.h"
#include "geom_f.h"
#include "font_f.h"
//#include "content_f.h"
#include "content.h"

#ifdef _GL
#include "glwindowdisplay.h"
#include "tesse_f.h"
#include "spline_f.h"
#endif /* _GL */

#include "logdebug.h"


#include "AmayaFrame.h"
#include "AmayaCanvas.h"
#include "AmayaCreatePathEvtHandler.h"

IMPLEMENT_DYNAMIC_CLASS(AmayaCreatePathEvtHandler, wxEvtHandler)

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaCreatePathEvtHandler, wxEvtHandler)
EVT_KEY_DOWN( AmayaCreatePathEvtHandler::OnChar )
EVT_LEFT_DOWN(	AmayaCreatePathEvtHandler::OnMouseDown)
EVT_LEFT_DCLICK(	AmayaCreatePathEvtHandler::OnMouseDbClick)
EVT_MIDDLE_DOWN(	AmayaCreatePathEvtHandler::OnMouseDown)
EVT_MIDDLE_DCLICK(	AmayaCreatePathEvtHandler::OnMouseDbClick)
EVT_RIGHT_DOWN(	AmayaCreatePathEvtHandler::OnMouseRightDown)
EVT_RIGHT_DCLICK(	AmayaCreatePathEvtHandler::OnMouseDbClick)
EVT_MOTION(		AmayaCreatePathEvtHandler::OnMouseMove)
EVT_MOUSEWHEEL(	AmayaCreatePathEvtHandler::OnMouseWheel)
END_EVENT_TABLE()

/*----------------------------------------------------------------------
 *----------------------------------------------------------------------*/
AmayaCreatePathEvtHandler::AmayaCreatePathEvtHandler() : wxEvtHandler()
{
}

/*----------------------------------------------------------------------
 *----------------------------------------------------------------------*/
AmayaCreatePathEvtHandler::AmayaCreatePathEvtHandler(AmayaFrame * p_frame,
						     Document doc,
						     void *inverse,
						     int ancestorX,
						     int ancestorY,
						     int canvasWidth,
						     int canvasHeight,
						     int shape,
						     Element el,
						     ThotBool *created)
  : wxEvtHandler()
  ,finished(false)
  ,pFrame(p_frame)
  ,frameId(p_frame->GetFrameId())
  ,document(doc)    
  ,inverse(inverse)
  ,x0(ancestorX)
  ,y0(ancestorY)
  ,width(canvasWidth)
  ,height(canvasHeight)
  ,shape(shape)
  ,el(el)
  ,box(NULL)
  ,created(created)
  ,nb_points(1)
{
  PtrAbstractBox pAb;

  if (pFrame)
    {
      /* attach this handler to the canvas */
      AmayaCanvas * p_canvas = pFrame->GetCanvas();
      p_canvas->PushEventHandler(this);

      /* assign a cross mouse cursor */
      pFrame->GetCanvas()->SetCursor( wxCursor(wxCURSOR_CROSS) );
      pFrame->GetCanvas()->CaptureMouse();
    }

  InitDrawing (5, 1, 0);
  state = 0;
  clear = false;

  /* Get the box of the SVG element */
  pAb = ((PtrElement)el) -> ElAbstractBox[0];
  if(!pAb || !(pAb->AbBox))
    {
    finished = true;
    return;
    }
  box = pAb -> AbBox;

  /* Get the GRAPHICS leaf */
  leaf = TtaGetFirstLeaf((Element)el);
  if(!leaf)
    {
    finished = true;
    return;
    }

    if(((PtrElement)leaf)->ElLeafType == LtPolyLine)
      {
	/* It's a polyline or a polygon */
	pBuffer = ((PtrElement)leaf)->ElPolyLineBuffer;
      }
    else if(((PtrElement)leaf)->ElLeafType == LtPath)
      {
	/* It's a path */
	pPa = ((PtrElement)leaf)->ElFirstPathSeg;
      }
}

/*----------------------------------------------------------------------
 *----------------------------------------------------------------------*/
AmayaCreatePathEvtHandler::~AmayaCreatePathEvtHandler()
{
  int symX2, symY2;

  if(shape == 8 && nb_points > 2)
    {
      /* close the curve */
      symX2=2*lastX1-lastX2;
      symY2=2*lastY1-lastY2;
      //AddPointInPolyline (pBuffer, *nb_points, symX2, symY2);
      nb_points++;

      //symX2=2*pBuffer->BuPoints[1].XCoord-pBuffer->BuPoints[2].XCoord;
      //symY2=2*pBuffer->BuPoints[1].YCoord-pBuffer->BuPoints[2].YCoord;
      //AddPointInPolyline (pBuffer, *nb_points, symX2, symY2);
      nb_points++;
      /*AddPointInPolyline (pBuffer, *nb_points,
			  pBuffer->BuPoints[1].XCoord,
			  pBuffer->BuPoints[1].YCoord
			  );*/
      nb_points++;
    }
  
  if (pFrame)
    {
      /* detach this handler from the canvas (restore default behaviour) */
      AmayaCanvas * p_canvas = pFrame->GetCanvas();
      p_canvas->PopEventHandler(false /* do not delete myself */);

      /* restore the default cursor */
      pFrame->GetCanvas()->SetCursor( wxNullCursor );

      pFrame->GetCanvas()->ReleaseMouse();
    }

}

/*----------------------------------------------------------------------
 *----------------------------------------------------------------------*/
bool AmayaCreatePathEvtHandler::IsFinish()
{
  return finished;
}

/*----------------------------------------------------------------------
 *----------------------------------------------------------------------*/
void AmayaCreatePathEvtHandler::OnMouseRightDown( wxMouseEvent& event )
{
  *created = TRUE;
  finished = true;
}

/*----------------------------------------------------------------------
 *----------------------------------------------------------------------*/
void AmayaCreatePathEvtHandler::OnChar( wxKeyEvent& event )
{
  if(event.GetKeyCode() !=  WXK_SHIFT)
    {
      *created = FALSE;
      finished = true;
    }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaCreatePathEvtHandler
 *      Method:  OnMouseDown
 * Description:  handle mouse button down events
 -----------------------------------------------------------------------*/
void AmayaCreatePathEvtHandler::OnMouseDown( wxMouseEvent& event )
{
  int x1,y1,x2,y2,x3,y3,x4,y4;

  if (IsFinish())
    return;

  /* Are we in the SVG ? */
  if(!MouseCoordinatesToSVG(document, pFrame,
			    x0,y0,
			    width,height,
			    NULL,
			    FALSE, &currentX, &currentY))return;

  if(shape == 7 || shape == 8)
    {
      clear = false;

      if(state == 0 || state == 1 || state == 3)
	{
	  /* Clear the active fragment */
	  DrawPathFragment(shape, TRUE);
	}
      else
	{
	  /* Clear the active fragment */
	  DrawPathFragment(shape, TRUE);

	  /* Draw the new curve fragment */
	  DrawPathFragment(shape, FALSE);
	}
    }
  else
    {
      /* Clear the active fragment */
      DrawPathFragment(shape, TRUE);
      
      /* Draw the new curve fragment */
      DrawPathFragment(shape, FALSE);
    }

  if(shape == 5 || shape == 6)
    {
      /* Add a new point in the polyline/polygon */
      state = 1;
      x1 = currentX;
      y1 = currentY;
      MouseCoordinatesToSVG(document, pFrame,
			    x0,y0,
			    width,height,
			    NULL,
			    TRUE, &x1, &y1);

      TtaAddPointInPolyline (leaf, nb_points, UnPixel, x1, y1, document);
      nb_points++;
    }
  else if(shape == 7 || shape == 8)
    {
      /* Bezier Curve */
      
      if(state == 2)
	{
	  /* Add a quadratic Bezier curve */
	  x1 = lastX2;
	  y1 = lastY2;
	  MouseCoordinatesToSVG(document, pFrame,
				x0,y0,
				width,height,
				NULL,
				TRUE, &x1, &y1);

	  x2 = symX;
	  y2 = symY;
	  MouseCoordinatesToSVG(document, pFrame,
				x0,y0,
				width,height,
				NULL,
				TRUE, &x2, &y2);

	  x4 = lastX1;
	  y4 = lastY1;
	  MouseCoordinatesToSVG(document, pFrame,
				x0,y0,
				width,height,
				NULL,
				TRUE, &x4, &y4);

	  
	  TtaAppendPathSeg (leaf,
			    TtaNewPathSegQuadratic (x1, y1, x4 ,y4,
						    x2, y2, FALSE),
			    document);

	  // AddPointInPolyline (pBuffer, *nb_points, lastX2, lastY2);
	  //	  nb_points++;
	  // AddPointInPolyline (pBuffer, *nb_points, symX, symY);
	  //nb_points++;
	  // AddPointInPolyline (pBuffer, *nb_points, symX, symY);
	  //nb_points++;
	  // AddPointInPolyline (pBuffer, *nb_points, lastX1, lastY1);
	  //nb_points++;
	}
      else if(state == 4)
	{
	  x1 = lastX3;
	  y1 = lastY3;
	  MouseCoordinatesToSVG(document, pFrame,
				x0,y0,
				width,height,
				NULL,
				TRUE, &x1, &y1);

	  x2 = lastX2;
	  y2 = lastY2;
	  MouseCoordinatesToSVG(document, pFrame,
				x0,y0,
				width,height,
				NULL,
				TRUE, &x2, &y2);

	  x3 = symX;
	  y3 = symY;
	  MouseCoordinatesToSVG(document, pFrame,
				x0,y0,
				width,height,
				NULL,
				TRUE, &x3, &y3);

	  x4 = lastX1;
	  y4 = lastY1;
	  MouseCoordinatesToSVG(document, pFrame,
				x0,y0,
				width,height,
				NULL,
				TRUE, &x4, &y4);

	  
	  TtaAppendPathSeg (leaf,
			    TtaNewPathSegCubic (x1, y1, x4 ,y4,
						x2, y2, x3, y3, FALSE),
			    document);
	  

	  /* Add a cubic Bezier Curve */
	  //AddPointInPolyline (pBuffer, *nb_points, lastX2, lastY2);
	  //nb_points++;
	  //AddPointInPolyline (pBuffer, *nb_points, symX, symY);
	  //nb_points++;
	  //AddPointInPolyline (pBuffer, *nb_points, lastX1, lastY1);
	  //nb_points++;
	}

      if(state < 4)state++;
      else state=3;
    }

  lastX3 = lastX2;
  lastY3 = lastY2;
  lastX2 = lastX1;
  lastY2 = lastY1;
  lastX1 = currentX;
  lastY1 = currentY;
  UpdateSymetricPoint();

}


/*----------------------------------------------------------------------
 *       Class:  AmayaCreatePathEvtHandler
 *      Method:  OnMouseDbClick
 * Description:  handle mouse dbclick events
 -----------------------------------------------------------------------*/
void AmayaCreatePathEvtHandler::OnMouseDbClick( wxMouseEvent& event )
{
  *created = nb_points > 2;
  finished = true;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaCreatePathEvtHandler
 *      Method:  OnMouseMove
 * Description:  handle mouse move events
 -----------------------------------------------------------------------*/
void AmayaCreatePathEvtHandler::OnMouseMove( wxMouseEvent& event )
{
  if(clear)
    DrawPathFragment(shape, TRUE);

  currentX = event.GetX();
  currentY = event.GetY();

  if(event.ShiftDown())
    {
      if(shape == 5 || shape == 6)
	ApproximateAngleOfLine(15, lastX1, lastY1, &currentX, &currentY);
    }

  UpdateSymetricPoint();

  DrawPathFragment(shape, TRUE);
  clear = true;

  MouseCoordinatesToSVG(document, pFrame,
			x0,y0,
			width,height,
			inverse,
			FALSE, &currentX, &currentY);
  
#ifndef _WINDOWS
  pFrame->GetCanvas()->Refresh();
#endif /* _WINDOWS */

}

/*----------------------------------------------------------------------
 *       Class:  AmayaCreatePathEvtHandler
 *      Method:  OnMouseWheel
 * Description:  handle mouse wheel events
 -----------------------------------------------------------------------*/
void AmayaCreatePathEvtHandler::OnMouseWheel( wxMouseEvent& event )
{
}


/*----------------------------------------------------------------------
  DrawLine
  draw a line from (x2,y2) to (x3,y3)

  (x3,y3)
  .
  .
  .
  (x2,y2)

  *----------------------------------------------------------------------*/
void AmayaCreatePathEvtHandler::DrawLine (int x2, int y2, int x3,
					  int y3, ThotBool specialColor)
{
  glEnable(GL_COLOR_LOGIC_OP);
#ifdef _WINDOWS
  glLogicOp(GL_COPY_INVERTED);
  glColor4ub (127, 127, 127, 0);
#else /* _WINDOWS */
  glLogicOp(GL_XOR);
  if(specialColor)
    glColor4ub (127, 0, 127, 0);
  else
    glColor4ub (127, 127, 127, 0);
#endif /* _WINDOWS */

  glBegin(GL_LINE_STRIP);
  glVertex2i(x2, y2);
  glVertex2i(x3, y3);
  glEnd ();
  glDisable(GL_COLOR_LOGIC_OP);
}

/*----------------------------------------------------------------------
  DrawQuadraticBezier
  draw a quadratic Bezier Curve from (x1,y1) to (x3,y3),
  using (x2,y2) as the control points.

  *----------------------------------------------------------------------*/
void AmayaCreatePathEvtHandler::DrawQuadraticBezier (int x1, int y1,
						     int x2, int y2,
						     int x3, int y3,
						     ThotBool specialColor)
	      
{
#define N_INTERP 50
  int i;

  GLfloat ctrlpoints[3][3] =
    {{ (float) x1, (float) y1, 0.0}, { (float) x2, (float) y2, 0.0},
     { (float) x3, (float) y3, 0.0}};

  glEnable(GL_COLOR_LOGIC_OP);
#ifdef _WINDOWS
  glLogicOp(GL_COPY_INVERTED);
  glColor4ub (127, 127, 127, 0);
#else /* _WINDOWS */
  glLogicOp(GL_XOR);
  if(specialColor)
    glColor4ub (127, 0, 127, 0);
  else
    glColor4ub (127, 127, 127, 0);
#endif /* _WINDOWS */

  glEnable(GL_MAP1_VERTEX_3);
  glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 3, ctrlpoints[0]);
  glBegin(GL_LINE_STRIP);

  for (i = 0; i <= N_INTERP; i++)
    glEvalCoord1f((GLfloat) i/((float)N_INTERP));
  glEnd();

  glDisable(GL_MAP1_VERTEX_3);
  glDisable(GL_COLOR_LOGIC_OP);

  /*
    void        *mesh;
    mesh = GetNewMesh ();
    QuadraticSplit2 (x1, y1, x2, y2, x3, y3, mesh);
    MeshNewPoint (x3, y3, mesh);
    CountourCountAdd (mesh);
    MakeMeshLines (mesh);
    FreeMesh (mesh);*/

}

/*----------------------------------------------------------------------
  DrawCubicBezier
  draw a cubic Bezier Curve from (x1,y1) to (x4,y4),
  using (x2,y2) and (x3,y3) as the control points.
  *----------------------------------------------------------------------*/
void AmayaCreatePathEvtHandler::DrawCubicBezier (int x1, int y1,
						 int x2, int y2,
						 int x3, int y3,
						 int x4, int y4,
						 ThotBool specialColor)
{
  int i;

  GLfloat ctrlpoints[4][3] =
    {{ (float) x1, (float) y1, 0.0}, { (float) x2, (float) y2, 0.0},
     { (float) x3, (float) y3, 0.0}, { (float) x4, (float) y4, 0.0}};


  glEnable(GL_COLOR_LOGIC_OP);
#ifdef _WINDOWS
  glLogicOp(GL_COPY_INVERTED);
  glColor4ub (127, 127, 127, 0);
#else /* _WINDOWS */
  glLogicOp(GL_XOR);
  if(specialColor)
    glColor4ub (127, 0, 127, 0);
  else
    glColor4ub (127, 127, 127, 0);
#endif /* _WINDOWS */

  glEnable(GL_MAP1_VERTEX_3);
  glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, ctrlpoints[0]);
  glBegin(GL_LINE_STRIP);

  for (i = 0; i <= N_INTERP; i++)
    glEvalCoord1f((GLfloat) i/((float)N_INTERP));
  glEnd();

  glDisable(GL_MAP1_VERTEX_3);
  glDisable(GL_COLOR_LOGIC_OP);

  /*
    void        *mesh;
    mesh = GetNewMesh ();
    PolySplit2 (x1, y1, x2, y2, x3, y3, x4, y4, mesh);
    MeshNewPoint (x4, y4, mesh);
    CountourCountAdd (mesh);
    MakeMeshLines (mesh);
    FreeMesh (mesh);
  */
}

/*----------------------------------------------------------------------
  UpdateSymetricPoint

  * Update the value of the current symetric point.
  *
  * (current)
  *      ---- 
  *          -----(last)-----
  *                          ----
  *                              (sym)

  ----------------------------------------------------------------------*/
void AmayaCreatePathEvtHandler::UpdateSymetricPoint()
{
  symX = 2*lastX1 - currentX;
  symY = 2*lastY1 - currentY;
}


/*----------------------------------------------------------------------
  DrawControlPoints

  Draw the control points of the Bezier Curve.
  *----------------------------------------------------------------------*/
void AmayaCreatePathEvtHandler::DrawControlPoints ()
{
  glEnable(GL_COLOR_LOGIC_OP);
#ifdef _WINDOWS
  glLogicOp(GL_COPY_INVERTED);
  glColor4ub (127, 127, 127, 0);
#else /* _WINDOWS */
  glLogicOp(GL_XOR);
  glColor4ub (0, 127, 127, 100);
#endif /* _WINDOWS */

  glBegin(GL_LINE_STRIP);
  glVertex2i(symX, symY);
  glVertex2i(currentX, currentY);
  glEnd ();
  glDisable(GL_COLOR_LOGIC_OP);
}


/*----------------------------------------------------------------------
  DrawPathFragment

  Draw the last path fragment. If specialColor is TRUE, then this fragment
  is drawn with a special color to show that it is active.
  *----------------------------------------------------------------------*/
void AmayaCreatePathEvtHandler::DrawPathFragment(int shape,
						 ThotBool specialColor)
{

  switch(shape)
    {
    case 5:
    case 6:
      /*      if(state == 1)
	      DrawLine (lastX1, lastY1, currentX, currentY, specialColor);*/
      break;

    case 7:
    case 8:
      switch(state)
	{
	case 0:
	  /*                                                  */
	  /*                                                  */
	  /*         (current)+                               */
	  break;
	case 1:
	  /*                          .....+(current)         */
	  /*                    ......                        */
	  /*         (last1)O...                              */
	  DrawLine (lastX1, lastY1, currentX, currentY, specialColor);
	  break;
	
	case 2:
	  /*                           +(current)             */
	  /*  (last2)                 /                       */
	  /*        .                /                        */
	  /*         ..          .(last1)                     */
	  /*           ..     ..  /                           */
	  /*             .....   /                            */
	  /*                  (sym)                           */
	  DrawQuadraticBezier (lastX2, lastY2, symX, symY,
			       lastX1, lastY1, specialColor);
	  if(specialColor)DrawControlPoints();
	  break;

	case 3:
	  /*                    +(current)                    */
	  /*                    ...                           */
	  /*                       .                          */
	  /*                      .                           */
	  /*                  ...   (last1)                   */
	  /*             .....                                */
	  /*   (last2)...                                     */
	  /*                                                  */
	  DrawQuadraticBezier (lastX2, lastY2, lastX1, lastY1,
			       currentX, currentY, specialColor);
	  break;

	case 4:
	  /*      (current)+---(last1)---(sym)                */
	  /*                      ...                         */
	  /*                         ....                     */
	  /*                            .                     */
	  /*                            .                     */
	  /*                          ...                     */
	  /*               ...........     (last2)            */
	  /*    (last3)....                                   */
	  DrawCubicBezier(lastX3, lastY3, lastX2, lastY2, symX, symY,
			  lastX1, lastY1, specialColor);
	  if(specialColor)DrawControlPoints();
	  break;
	}
      break;

    default:
      break;
    }

#ifdef _WINDOWS
  GL_Swap (frameId);
#endif /* WINDOWS */
}


#endif /* _WX */
