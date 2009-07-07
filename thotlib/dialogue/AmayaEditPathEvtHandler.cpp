/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

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
#include "content_f.h"
#include "windowdisplay_f.h"
#include "viewapi_f.h"
#include "content.h"
#include "svgedit.h"

#ifdef _GL
#include "glwindowdisplay.h"
#endif /* _GL */

#include "logdebug.h"


#include "AmayaFrame.h"
#include "AmayaCanvas.h"
#include "AmayaEditPathEvtHandler.h"

IMPLEMENT_DYNAMIC_CLASS(AmayaEditPathEvtHandler, wxEvtHandler)

  /*----------------------------------------------------------------------
   *  this is where the event table is declared
   *  the callbacks are assigned to an event type
   *----------------------------------------------------------------------*/
  BEGIN_EVENT_TABLE(AmayaEditPathEvtHandler, wxEvtHandler)
  EVT_KEY_DOWN( AmayaEditPathEvtHandler::OnChar )
  EVT_LEFT_DOWN(AmayaEditPathEvtHandler::OnMouseDown) 
  EVT_LEFT_UP(AmayaEditPathEvtHandler::OnMouseUp)
  EVT_LEFT_DCLICK(AmayaEditPathEvtHandler::OnMouseDbClick)
  EVT_MIDDLE_DOWN(AmayaEditPathEvtHandler::OnMouseDown)
  EVT_MIDDLE_UP(AmayaEditPathEvtHandler::OnMouseUp)
  EVT_MIDDLE_DCLICK(AmayaEditPathEvtHandler::OnMouseDbClick)
  EVT_RIGHT_DOWN(AmayaEditPathEvtHandler::OnMouseDown)
  EVT_RIGHT_UP(AmayaEditPathEvtHandler::OnMouseUp)
  EVT_RIGHT_DCLICK(AmayaEditPathEvtHandler::OnMouseDbClick)
  EVT_MOTION(AmayaEditPathEvtHandler::OnMouseMove)
  EVT_MOUSEWHEEL(AmayaEditPathEvtHandler::OnMouseWheel)
  END_EVENT_TABLE()

/*----------------------------------------------------------------------
*----------------------------------------------------------------------*/
  AmayaEditPathEvtHandler::AmayaEditPathEvtHandler() : wxEvtHandler()
{
}

/*----------------------------------------------------------------------
 *----------------------------------------------------------------------*/
AmayaEditPathEvtHandler::AmayaEditPathEvtHandler(AmayaFrame *p_frame,
                                                 Document doc,
                                                 void *inverse,
                                                 int ancestorX,
                                                 int ancestorY,
                                                 int canvasWidth,
                                                 int canvasHeight,
                                                 Element element,
                                                 int point_number,
                                                 ThotBool *transformApplied)
  :wxEvtHandler()
  ,finished(false)
  ,pFrame(p_frame)
  ,frameId(p_frame->GetFrameId())
  ,document(doc)
  ,inverse(inverse)
  ,x0(ancestorX)
  ,y0(ancestorY)
  ,width(canvasWidth)
  ,height(canvasHeight)
  ,type(-1)
  ,el(element)
  ,box(NULL)
  ,buttonDown(false)
  ,hasBeenTransformed(transformApplied)
{
  PtrAbstractBox pAb;
  int i,j;
  PtrPathSeg          pPa, pPaStart = NULL;

  *hasBeenTransformed = FALSE;
  if (pFrame)
    {
      /* attach this handler to the canvas */
      AmayaCanvas * p_canvas = pFrame->GetCanvas();
      p_canvas->PushEventHandler(this);
      pFrame->GetCanvas()->CaptureMouse();
    }

  /* Get the GRAPHICS leaf */
  leaf = GetGraphicsUnit(el);

  if (!leaf)
    {
      finished = true;
      return;
    }

  /* Get the box of the SVG element */
  pAb = ((PtrElement)leaf) -> ElAbstractBox[0];
  if (!pAb || !(pAb->AbBox))
    {
      finished = true;
      return;
    }
  box = pAb -> AbBox;
  if (((PtrElement)leaf)->ElLeafType == LtPolyLine)
    {
      /* It's a polyline or a polygon */
      pBuffer = ((PtrElement)leaf)->ElPolyLineBuffer;
      j = 1;
      for (i = 1; pBuffer; i++)
        {
          if (i == point_number)
            {
              /* Moving a point of a polyline/polygon  */
              type = 6;
              i_poly = j;
              break;
            }
	  
          j++;
          if (j == pBuffer->BuLength)
            {
              pBuffer = pBuffer->BuNext;
              j = 0;
            }
        }
    }
  else if (((PtrElement)leaf)->ElLeafType == LtPath)
    {
      /* It's a path: find the segment to edit */
      pPaPrevious = NULL;
      pPaNext = NULL;

      i = 1;

      pPa = ((PtrElement)leaf)->ElFirstPathSeg;
      if (pPa == NULL)
        {
          finished = true;
          return;
        }
  
      while (pPa)
        {
          if ((pPa->PaNewSubpath || !pPa->PaPrevious))
            {
              /* this path segment starts a new subpath */
              pPaStart = pPa;
	      
              if (i == point_number)
                {
                  /* Moving the first point of subpath */
                  type = 0;
                  break;
                }
	      
              i++;
            }

          if (pPa->PaShape == PtCubicBezier ||
              pPa->PaShape == PtQuadraticBezier)
            {
	      
              if (i == point_number)
                {
                  if (pPa->PaNewSubpath || !pPa->PaPrevious)
                    {
                      /* Moving the start control point of a subpath */
                      type = 1;
                      break;
                    }
                  else
                    {
                      /* Moving a start control point */
                      type = 2;
                      pPaPrevious = pPa->PaPrevious;
                      break;
                    }
                }
              i++;

              if (i == point_number)
                {
                  if (pPa->PaNext && !(pPa->PaNext->PaNewSubpath))
                    /* Moving an end control point */
                    {
                      type = 3;
                      pPaNext = pPa->PaNext;
                      break;
                    }
                  else
                    /* Moving the end control point of the last point
                       of a subpath*/
                    {
                      type = 7;
                      break;
                    }
                }
              i++;
            }

          if (i == point_number)
            {
              if (pPa->PaNext && !(pPa->PaNext->PaNewSubpath))
                {
                  /* Moving a point in the path */
                  type = 4;
                  pPaNext = pPa->PaNext;
                  break;
                }
              else
                {
                  /* Moving the last point of a subpath */
                  type = 5;
                  break;
                }
            }
	  
          i++;
          pPa = pPa->PaNext;
        }

      pPaCurrent = pPa;

      if (pPaCurrent)
        {
          if (type == 0 || type == 1)
            {
              /* We want to edit the first point of a subpath:
                 is the last point at the same position? */
              while(pPa)
                {
                  if (!pPa->PaNext || pPa->PaNext->PaNewSubpath)
                    {
                      if (pPa->XEnd == pPaCurrent->XStart &&
                          pPa->YEnd == pPaCurrent->YStart)
                        pPaPrevious = pPa;

                      break;
                    }
		  
                  pPa = pPa->PaNext;
                }
            }
          else if (type == 7 || type == 5)
            {
              /* We want to edit the last point of a subpath:
                 is the first point at the same position? */
              if (pPaStart && 
                  pPaStart->XStart == pPaCurrent->XEnd &&
                  pPaStart->YStart == pPaCurrent->YEnd)
                pPaNext = pPaStart;
            }
        }
     
    }
  else 
    {
      finished = true;
      return;
    }
}

/*----------------------------------------------------------------------
 *----------------------------------------------------------------------*/
AmayaEditPathEvtHandler::~AmayaEditPathEvtHandler()
{
  if (pFrame)
    {
      /* detach this handler from the canvas (restore default behaviour) */
      AmayaCanvas * p_canvas = pFrame->GetCanvas();
      p_canvas->PopEventHandler(false /* do not delete myself */);
      pFrame->GetCanvas()->ReleaseMouse();
    }

#ifdef _GL
  if (glIsList (box->DisplayList))
    {
      glDeleteLists (box->DisplayList, 1);
      box->DisplayList = 0;
    }
#endif /* _GL */

}

/*----------------------------------------------------------------------
 *----------------------------------------------------------------------*/
bool AmayaEditPathEvtHandler::IsFinish()
{
  return finished;
}

/*----------------------------------------------------------------------
 *----------------------------------------------------------------------*/
void AmayaEditPathEvtHandler::OnChar( wxKeyEvent& event )
{
  if (event.GetKeyCode() !=  WXK_SHIFT)
    {
      TtaSetStatus (document, 1, "", NULL);
      finished = true;
    }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaEditPathEvtHandler
 *      Method:  OnMouseDown
 * Description:  handle mouse button down events
 -----------------------------------------------------------------------*/
void AmayaEditPathEvtHandler::OnMouseDown( wxMouseEvent& event )
{
  if (IsFinish())
    return;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaEditPathEvtHandler
 *      Method:  OnMouseUp
 * Description:  handle mouse button up events
 -----------------------------------------------------------------------*/
void AmayaEditPathEvtHandler::OnMouseUp( wxMouseEvent& event )
{
  if (IsFinish())
    return;

  TtaSetStatus (document, 1, "", NULL);
  finished = true;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaEditPathEvtHandler
 *      Method:  OnMouseDbClick
 * Description:  handle mouse dbclick events
 -----------------------------------------------------------------------*/
void AmayaEditPathEvtHandler::OnMouseDbClick( wxMouseEvent& event )
{
  TtaSetStatus (document, 1, "", NULL);
  finished = true;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaEditPathEvtHandler
 *      Method:  OnMouseMove
 * Description:  handle mouse move events
 -----------------------------------------------------------------------*/
void AmayaEditPathEvtHandler::OnMouseMove( wxMouseEvent& event )
{
  int x1,y1,x2,y2;
  int dx, dy;
  ThotBool smooth = TRUE;

  if (IsFinish())
    return;
  if (event.ShiftDown())
    smooth = FALSE;

  /* DELTA is the sensitivity toward mouse moves. */
#define DELTA 0
  /* Update the current mouse coordinates */
  mouse_x = event.GetX();
  mouse_y = event.GetY();

  if (!buttonDown)
    {
      lastX = mouse_x;
      lastY = mouse_y;
      buttonDown = true;
    }

  if ((abs(mouse_x -lastX) + abs(mouse_y - lastY) > DELTA))
    {

      if (!(*hasBeenTransformed) && type != 6)
        {
          /* Convert Quadratic Bezier to Cubic */
          if (pPaCurrent && pPaCurrent->PaShape == PtQuadraticBezier)
            TtaQuadraticToCubicPathSeg ((void *)pPaCurrent);
	  
          if (pPaNext && pPaNext->PaShape == PtQuadraticBezier)
            TtaQuadraticToCubicPathSeg ((void *)pPaNext);
	  
          if (pPaPrevious && pPaPrevious->PaShape == PtQuadraticBezier)
            TtaQuadraticToCubicPathSeg ((void *)pPaPrevious);
        }

      /* The user is pressing the mouse button and moving */
      x1 = lastX;
      y1 = lastY;
      x2 = mouse_x;
      y2 = mouse_y;

      MouseCoordinatesToSVG(document, pFrame, x0, y0, width, height,
                            inverse, TRUE, NULL,
                            &x1, &y1, FALSE);

      MouseCoordinatesToSVG(document, pFrame, x0, y0, width, height,
                            inverse, TRUE, NULL,
                            &x2, &y2, FALSE);

      dx = x2 - x1;
      dy = y2 - y1;

      switch(type)
        {
        case 0:
          /* Moving a start point */
          pPaCurrent->XStart += dx;
          pPaCurrent->YStart += dy;

          if (pPaCurrent->PaShape == PtCubicBezier)
            {
              pPaCurrent->XCtrlStart += dx;
              pPaCurrent->YCtrlStart += dy;
            }

          if (pPaPrevious)
            {
              pPaPrevious->XEnd += dx;
              pPaPrevious->YEnd += dy;

              if (pPaPrevious->PaShape == PtCubicBezier)
                {
                  pPaPrevious->XCtrlEnd += dx;
                  pPaPrevious->YCtrlEnd += dy;
                }
            }
          break;

        case 1:
          /* Moving the start control point of a new subpath */
        case 2:
          /* Moving the start control point */
          pPaCurrent->XCtrlStart += dx;
          pPaCurrent->YCtrlStart += dy;

          if (smooth && pPaPrevious &&
              pPaPrevious->PaShape == PtCubicBezier)
            {
              /* Update the other control point */
              GetSymetricPoint(pPaCurrent->XCtrlStart,
                               pPaCurrent->YCtrlStart,
                               pPaCurrent->XStart,
                               pPaCurrent->YStart,

                               &(pPaPrevious->XCtrlEnd),
                               &(pPaPrevious->YCtrlEnd));
            }

          break;

        case 3:
          /* Moving the end control point */

        case 7:
          /* Moving the end control point of the last point
             of a subpath*/
          pPaCurrent->XCtrlEnd += dx;
          pPaCurrent->YCtrlEnd += dy;

          if (smooth && pPaNext && 
              pPaNext->PaShape == PtCubicBezier)
            {
              /* Update the other control point */
              GetSymetricPoint(pPaCurrent->XCtrlEnd,
                               pPaCurrent->YCtrlEnd,
                               pPaCurrent->XEnd,
                               pPaCurrent->YEnd,

                               &(pPaNext->XCtrlStart),
                               &(pPaNext->YCtrlStart));
            }

          break;

        case 4:
          /* Moving a point in the path */
        case 5:
          /* Moving the last point of a subpath */

          /*if (pPaCurrent->PaShape == PtEllipticalArc)
            UpdateArc(pPaCurrent->XStart,
            pPaCurrent->XStart,
            pPaCurrent->XEnd,
            pPaCurrent->YEnd,
            (int)(pPaCurrent->XEnd+dx),
            (int)(pPaCurrent->YEnd+dy),
            &(pPaCurrent->XAxisRotation),
            &(pPaCurrent->XRadius),
            &(pPaCurrent->YRadius));*/

          if (pPaCurrent->PaShape == PtCubicBezier)
            {
              pPaCurrent->XCtrlEnd += dx;
              pPaCurrent->YCtrlEnd += dy;
            }

          pPaCurrent->XEnd += dx;
          pPaCurrent->YEnd += dy;

          if (pPaNext)
            {
              pPaNext->XStart += dx;
              pPaNext->YStart += dy;

              if (pPaNext->PaShape == PtCubicBezier)
                {
                  pPaNext->XCtrlStart += dx;
                  pPaNext->YCtrlStart += dy;
                }
            }
          break;

        case 6:
          /* Moving a point of a polyline/polygon  */
          pBuffer->BuPoints[i_poly].XCoord += dx;
          pBuffer->BuPoints[i_poly].YCoord += dy;
          break;

        default:
          break;
        }

#ifndef NODISPLAY
      /* Redisplay the GRAPHICS leaf */
      RedisplayLeaf ((PtrElement) leaf, document, 0);
#endif

      /* Update the previous mouse coordinates */
      lastX = mouse_x;
      lastY = mouse_y;
      *hasBeenTransformed = TRUE;
    }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaEditPathEvtHandler
 *      Method:  OnMouseWheel
 * Description:  handle mouse wheel events
 -----------------------------------------------------------------------*/
void AmayaEditPathEvtHandler::OnMouseWheel( wxMouseEvent& event )
{
}

/*----------------------------------------------------------------------
  GetSymetricPoint
  *----------------------------------------------------------------------*/
void AmayaEditPathEvtHandler::GetSymetricPoint(int x, int y,
                                               int x0, int y0,
                                               int *symx, int *symy)
{
  *symx = 2*x0 - x;
  *symy = 2*y0 - y;
}

/*----------------------------------------------------------------------
 *----------------------------------------------------------------------*/
//  void AmayaEditPathEvtHandler::UpdateArc(int x1, int y1,
// 					 int x2, int y2,
// 					 int x3, int y3,
// 					 int *phi, int *rx, int *ry)
// {
//   /*
//     We want to transform move the point (x2,y2) to (x3,y3).
//     The arc is scaled (ratio=k) and the x-axis is rotated.


//          (x2,y2)                              (x3,y3)
//           /                                     | 
//          /                                      |
//         /                                       |
//        /                                        |
//     (x1,y1)                                  (x1,y1)

//     {rx:  x radius                      {rx' = k*rx
//     {ry:  y radius                      {ry' = k*ry
//     {phi: x-axis-rotation               {phi' ?
//     {largearc                           {largearc
//     {sweep                              {sweep
   
//           || (x3 - x1) ||
//           || (y3 - y1) ||

//      k =  ---------------

//           || (x2 - x1) ||
//           || (y2 - y1) ||

//   */

//   float k, a, b, c, d;
//   float dx1, dx2, dy1, dy2;
//   float d1, d2;
//   float f, cosf, sinf;
//   float x,y, det;

//   dx1 = (float)(x2 - x1);
//   dx2 = (float)(x3 - x1);
//   dy1 = (float)(y2 - y1);
//   dy2 = (float)(y3 - y1);

//   d1 = dx1*dx1 + dy1*dy1;
//   d2 = dx2*dx2 + dy2*dy2;

//   if (d1 == 0 || d2 == 0)
//     return;

//   k = sqrt(d2/d1);
//   *rx = (int)(k * *rx);
//   *ry = (int)(k * *ry);

//   f = *phi * M_PI / 180;
//   cosf = cos(f);
//   sinf = sin(f);
 
//   /*  One can check that if M(p) = (cos(p)  -sin(p))
//                                    (sin(p)   cos(p))

//       then
//              (k*dx1)         (dx2)
//       M(phi')(k*dy1) = M(phi)(dy2)

//              (a)   (c)
//       M(phi')(b) = (d)

//    */

//   a = k*dx1;
//   b = k*dy1;
//   c = cosf*dx2 - sinf*dy2;
//   d = sinf*dx2 + cosf*dy2;

//   /*
//       x = cos(phi')
//       y = sin(phi')
//       phi' = acos(x)*sgn(y)


//       (a -b)(x)   (c)
//       (b  a)(y) = (d)

//       det = a^2 + b^2;

//       (x)    1  (a  b)(c)
//       (y) = --- (-b a)(d)
//             det   
//    */
  
//   det = a*a + b*b;
//   x = (a*c + b*d)/det;
//   y = (-b*c + a*d)/det;

//   *phi = (int)(acos(x) * 180 / M_PI);
//   if (y < 0)*phi = 360-*phi;
// }

#endif /* _WX */
