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
#include "viewapi_f.h"

#ifdef _GL
#include "glwindowdisplay.h"
#endif /* _GL */

#include "logdebug.h"


#include "AmayaFrame.h"
#include "AmayaCanvas.h"
#include "AmayaEditShapeEvtHandler.h"

static void apply_deltas(int *x, int *y, int *lx, int *ly,
		      int dx, int dy,
		      int sx, int sy, float ratio)
{
  if(ratio)
    {
      /* The ratio = ly/lx must be preserved */

      if(sx == 0)
	{
	  /* Vertical resize */
	  (*ly)+=sy*dy;
	  dx = (*lx - (*ly)/ratio)/2;
	  (*lx)=(*ly)/ratio;
	  sx = -1; // Force the change of coordinates
	}
      else if(sy == 0)
	{
	  /* Horizontal resize */
	  (*lx)+=sx*dx;
	  dy = (*ly - (*lx)*ratio)/2;
	  (*ly)=(*lx)*ratio;
	  sy = -1; // Force the change of coordinates
	}
      else if(dx*dy*sx*sy <= 0)
	{
	  /* A diagonal resize with contradictory directions */
	  return;
	}
      else
	{
	  /* Diagonal resize */
	  if(ratio*abs(dx) < abs(dy))
	    {
	      /* Choose the horizontal delta */
	      (*lx)+=sx*dx;
	      dy = sy*((*lx)*ratio - *ly);
	      (*ly)=(*lx)*ratio;
	    }
	  else
	    {
	      /* Choose the vertical delta */
	      (*ly)+=sy*dy;
	      dx = sx*((*ly)/ratio - *lx);
	      (*lx)=(*ly)/ratio;
	    }

	}
    }
  else
    {
      /* Resize without ratio */
      (*lx)+=sx*dx;
      (*ly)+=sy*dy;
    }

  /* Change the coordinates (*x,*y) of the box */
  if(sx<0)(*x)+=dx;
  if(sy<0)(*y)+=dy;
}

IMPLEMENT_DYNAMIC_CLASS(AmayaEditShapeEvtHandler, wxEvtHandler)

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaEditShapeEvtHandler, wxEvtHandler)
EVT_KEY_DOWN ( AmayaEditShapeEvtHandler::OnKeyDown )
EVT_LEFT_DOWN(	AmayaEditShapeEvtHandler::OnMouseDown)
EVT_LEFT_UP(		AmayaEditShapeEvtHandler::OnMouseUp)
EVT_LEFT_DCLICK(	AmayaEditShapeEvtHandler::OnMouseDbClick)
EVT_MIDDLE_DOWN(	AmayaEditShapeEvtHandler::OnMouseDown)
EVT_MIDDLE_UP(	AmayaEditShapeEvtHandler::OnMouseUp)
EVT_MIDDLE_DCLICK(	AmayaEditShapeEvtHandler::OnMouseDbClick)
EVT_RIGHT_DOWN(	AmayaEditShapeEvtHandler::OnMouseDown)
EVT_RIGHT_UP(		AmayaEditShapeEvtHandler::OnMouseUp)
EVT_RIGHT_DCLICK(	AmayaEditShapeEvtHandler::OnMouseDbClick)
EVT_MOTION(		AmayaEditShapeEvtHandler::OnMouseMove)
EVT_MOUSEWHEEL(	AmayaEditShapeEvtHandler::OnMouseWheel)
END_EVENT_TABLE()

/*----------------------------------------------------------------------
 *----------------------------------------------------------------------*/
AmayaEditShapeEvtHandler::AmayaEditShapeEvtHandler() : wxEvtHandler()
{
}

/*----------------------------------------------------------------------
 *----------------------------------------------------------------------*/
AmayaEditShapeEvtHandler::AmayaEditShapeEvtHandler
(AmayaFrame * p_frame,
 Document document,
 void *inverse,
 int ancestorX,
 int ancestorY,
 int canvasWidth,
 int canvasHeight,
 Element element,
 int point,
 ThotBool *hasBeenEdited)
  : wxEvtHandler()
  ,finished(false)
  ,pFrame(p_frame)
  ,frameId(p_frame->GetFrameId())
  ,doc(document)
  ,inverse(inverse)
  ,x0(ancestorX)
  ,y0(ancestorY)
  ,width(canvasWidth)
  ,height(canvasHeight)
  ,el(element)
  ,point(point)
  ,hasBeenEdited(hasBeenEdited)
  ,leaf(NULL)
  ,e_ab(NULL)
  ,e_box(NULL)
  ,ab(NULL)
  ,box(NULL)
{
  *hasBeenEdited = FALSE;
  buttonDown = false;

  if (pFrame)
    {
      /* attach this handler to the canvas */
      AmayaCanvas * p_canvas = pFrame->GetCanvas();
      p_canvas->PushEventHandler(this);

      /* assign a cross mouse cursor */
      pFrame->GetCanvas()->SetCursor( wxCursor(wxCURSOR_CROSS) );
      pFrame->GetCanvas()->CaptureMouse();
    }

  /* Get the box of the SVG element */
  e_ab = ((PtrElement)el)->ElAbstractBox[0];
  if(!e_ab || !(e_ab->AbBox))
    {
    finished = true;
    return;
    }
  e_box = e_ab->AbBox;

  /* Get the GRAPHICS leaf */
  leaf = TtaGetLastChild((Element)el);
  if(!leaf)
    {
    finished = true;
    return;
    }

  /* Get the box of the SVG element */
  ab = ((PtrElement)leaf)->ElAbstractBox[0];
  if(!ab || !(ab->AbBox))
    {
    finished = true;
    return;
    }

  box = ab->AbBox;
  shape = ab->AbShape;
}

/*----------------------------------------------------------------------
 *----------------------------------------------------------------------*/
AmayaEditShapeEvtHandler::~AmayaEditShapeEvtHandler()
{
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
  IsFinish
 *----------------------------------------------------------------------*/
bool AmayaEditShapeEvtHandler::IsFinish()
{
  return finished;
}

/*----------------------------------------------------------------------
  OnKeyDown
 *----------------------------------------------------------------------*/
void AmayaEditShapeEvtHandler::OnKeyDown( wxKeyEvent& event )
{
  if(event.GetKeyCode() !=  WXK_SHIFT)
    {
      *hasBeenEdited = FALSE;
      finished = true;
    }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaEditShapeEvtHandler
 *      Method:  OnMouseDown
 * Description:  handle mouse button down events
 -----------------------------------------------------------------------*/
void AmayaEditShapeEvtHandler::OnMouseDown( wxMouseEvent& event )
{
}

/*----------------------------------------------------------------------
 *       Class:  AmayaEditShapeEvtHandler
 *      Method:  OnMouseUp
 * Description:  handle mouse button up events
 -----------------------------------------------------------------------*/
void AmayaEditShapeEvtHandler::OnMouseUp( wxMouseEvent& event )
{
  finished = true;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaEditShapeEvtHandler
 *      Method:  OnMouseDbClick
 * Description:  handle mouse dbclick events
 -----------------------------------------------------------------------*/
void AmayaEditShapeEvtHandler::OnMouseDbClick( wxMouseEvent& event )
{
  finished = true;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaEditShapeEvtHandler
 *      Method:  OnMouseMove
 * Description:  handle mouse move events
 -----------------------------------------------------------------------*/
void AmayaEditShapeEvtHandler::OnMouseMove( wxMouseEvent& event )
{
#define RATIO_EQUILATERAL sqrt(3)/2

  ThotBool same_size;
  int rx,ry,lx,ly, x, y;
  int x1,y1,x2,y2,  x3=0,y3=0,x4=0,y4=0;
  int dx, dy;
  float ratio = 0.;

  if (IsFinish())return;

  /* DELTA is the sensitivity toward mouse moves. */
#define DELTA 0

  /* Update the current mouse coordinates */
  mouse_x = event.GetX();
  mouse_y = event.GetY();

  if(!buttonDown)
    {
      lastX = mouse_x;
      lastY = mouse_y;
      buttonDown = true;
    }

  if((abs(mouse_x -lastX) + abs(mouse_y - lastY) > DELTA))
    {
      x1 = lastX;
      y1 = lastY;
      x2 = mouse_x;
      y2 = mouse_y;

      MouseCoordinatesToSVG(doc, pFrame,
			    x0, y0,
			    width, height,
			    inverse,
			    TRUE, &x1, &y1);

      MouseCoordinatesToSVG(doc, pFrame,
			    x0, y0,
			    width, height,
			    inverse,
			    TRUE, &x2, &y2);

      dx = x2 - x1;
      dy = y2 - y1;

      /*               lx
       *         <------------->
       *                       
       *       (x,y)------O-rx-.    ^
       *         |             |    |
       *         |             ry   |
       *         |             |    |
       *         |             O    |    
       *         |             |    |    
       *         |             |    |ly    
       *         |             |    |    
       *         |             |    |    
       *         |             |    |    
       *         |             |    |    
       *         .-------------.    v
       */


      lx = box->BxW;
      ly = box->BxH;
      x = box->BxXOrg;
      y = box->BxYOrg;

      /*                                   9 = Rx handle       */
      /*                                  /                    */
      /*            1-------------2------O------3              */
      /*            |                           O              */
      /*            |                           |\             */
      /*            8                           4 \            */
      /*            |                           |  10 = Ry     */
      /*            |                           |       Handle */
      /*            7-------------6-------------5              */

      switch(shape)
	{
	case 'g': /* line */
	  switch(point)
	    {
	    case 1:
	      x3 = x+lx;
	      y3 = y+ly;
	      x4 = x;
	      y4 = y;
	      break;

	    case 3:
	      x3 = x;
	      y3 = y+ly;
	      x4 = x+lx;
	      y4 = y;
	      break;

	    case 5:
	      x3 = x;
	      y3 = y;
	      x4 = x+lx;
	      y4 = y+ly;
	      break;

	    case 7:
	      x3 = x+lx;
	      y3 = y;
	      x4 = x;
	      y4 = y+ly;
	      break;
	    }

	  x4+=dx;
	  y4+=dy;

	  lx = abs(x4 - x3);
	  ly = abs(y4 - y3);
	  if(x3 < x4)x = x3; else x = x4;
	  if(y3 < y4)y = y3; else y = y4;

	  e_ab->AbHorizPos.PosEdge = (x == x4 ? Left : Right);
	  e_ab->AbVertPos.PosEdge = (y == y4 ? Top : Bottom);

	  if(e_ab->AbHorizPos.PosEdge == Left)
	    {
	      if(e_ab->AbVertPos.PosEdge == Top)
		point = 1;
	      else
		point = 7;
	    }
	  else
	    {
	      if(e_ab->AbVertPos.PosEdge == Top)
		point = 3;
	      else
		point = 5;
	    }

	  /* Change the selected point in the line. */
	  ViewFrameTable[frameId - 1].FrSelectionBegin.VsIndBox = point;
	  break;

	case '\1': /* square */
	case 'C': /* rectangle */
	case 'a': /* circle */
	case 'c': /* ellipse */
	case 'L': /* diamond */
	case '\4': /* Equilateral triangle (ly = lx*R) */
	case '\5': /* Isosceles triangle */
	case '\6': /* Rectangle triangle */

	  if(shape == '\1' || shape == 'C')
	    {
	      rx = box->BxRx;
	      ry = box->BxRy;
	      if(ry == -1)ry = rx;
	      else if(rx == -1)rx = ry;
	    }

	  if(1 <= point && point <= 8)
	    {
	    same_size = (shape == '\1' || shape == 'a');
	    if(same_size)ratio = 1.;
	    }
	  else
	    same_size = (box->BxRx == -1 || box->BxRy == -1);

	  if(shape == '\4')
	    ratio = RATIO_EQUILATERAL;

	  if(shape == '\6' && point == 5)
	    {
	      /* The point is actually the middle of the hypot */
	      dx*=2;
	      dy*=2;
	    }

	  switch(point)
	    {
	    case 1:
	      apply_deltas(&x, &y, &lx, &ly, dx, dy, -1, -1, ratio);
	      break;
	      
	    case 2:
	      apply_deltas(&x, &y, &lx, &ly, dx, dy, 0, -1, ratio);
   	      break;
	      
	    case 3:
	      apply_deltas(&x, &y, &lx, &ly, dx, dy, +1, -1, ratio);
	      break;

	    case 4:
	      apply_deltas(&x, &y, &lx, &ly, dx, dy, +1, 0, ratio);
	      break;
	      
	    case 5:
	      apply_deltas(&x, &y, &lx, &ly, dx, dy, +1, +1, ratio);
	      break;
	      
	    case 6:
	      apply_deltas(&x, &y, &lx, &ly, dx, dy, 0, +1, ratio);
	      break;
	      
	    case 7:
	      apply_deltas(&x, &y, &lx, &ly, dx, dy, -1, +1, ratio);
	      break;
	      
	    case 8:
	      apply_deltas(&x, &y, &lx, &ly, dx, dy, -1, 0, ratio);
	      break;
	      
	    case 9:
	      rx -= dx;
	      if(same_size)ry=rx;
	      break;
	      
	    case 10:
	      ry += dy;
	      if(same_size)rx=ry;
	      break;
	    }
	  
	  if(lx < 0){lx = 0; x = box->BxXOrg;}
	  if(ly < 0){ly = 0; y = box->BxYOrg;}
	  
	  if(shape == '\1' || shape == 'C')
	    {
	      if(rx < 0)rx = 0;
	      if(ry < 0)ry = 0;
	      if(rx > lx/2)rx = lx/2;
	      if(ry > ly/2)ry = ly/2;
	      
	      if(box->BxRx != -1)box->BxRx = rx;
	      if(box->BxRy != -1)box->BxRy = ry;
	    }

	  break;

	default:
	  break;
	}
      
      /*NewDimension (ab, lx, ly, frameId, TRUE);
	NewPosition (ab, x, 0, y, 0, frameId, TRUE);*/
      
      box->BxXOrg = x;
      box->BxYOrg = y;
      box->BxW = lx;
      box->BxH = ly;
      box->BxWidth = lx;
      box->BxHeight = ly;
      
      /*ab->AbWidthChange = TRUE;
      ab->AbHeightChange = TRUE;
      ab->AbHorizPosChange = TRUE;
      ab->AbVertPosChange = TRUE;*/

      //e_box->BxXOrg = x;
      //e_box->BxYOrg = y;
      e_box->BxW = lx;
      e_box->BxH = ly;
      e_box->BxWidth = lx;
      e_box->BxHeight = ly;

      /*e_ab->AbWidthChange = TRUE;
      e_ab->AbHeightChange = TRUE;
      e_ab->AbHorizPosChange = TRUE;
      e_ab->AbVertRefChange = TRUE;*/

#ifndef NODISPLAY
      /* Redisplay the GRAPHICS leaf */
      RedisplayLeaf ((PtrElement) leaf, doc, 0);
#endif

      /* Redisplay the SVG canvas to see the transform applied to the object */
       DefBoxRegion (frameId, box, -1, -1, -1, -1);
       RedrawFrameBottom (frameId, 0, NULL);
       pFrame->GetCanvas()->Refresh();

      /* Update the previous mouse coordinates */
      lastX = mouse_x;
      lastY = mouse_y;

      *hasBeenEdited = TRUE;
    }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaEditShapeEvtHandler
 *      Method:  OnMouseWheel
 * Description:  handle mouse wheel events
 -----------------------------------------------------------------------*/
void AmayaEditShapeEvtHandler::OnMouseWheel( wxMouseEvent& event )
{
}
#endif /* _WX */
