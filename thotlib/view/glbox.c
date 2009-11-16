/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * glbox.c : Handling of Double buffering and Bounding box calculation, 
 *           with Opengl library both for MS-Windows and Unix
 *
 * Author: P. Cheyrou-lagreze (INRIA)
 *
 */
#ifdef _GL
#ifdef _WX
  #include "wx/wx.h"
  #include "wx/glcanvas.h"
  #include "AmayaFrame.h"
#else /* _WX */
  #include <GL/gl.h>
#endif /* _WX */
#endif /* _GL */
#include "ustring.h"
#include "math.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"
#include "logdebug.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "boxes_tv.h"
#include "font_tv.h"
#include "frame_tv.h"
#include "units_tv.h"
#include "edit_tv.h"
#include "thotcolor_tv.h"

#define ALLOC_POINTS    300

#include "boxlocate_f.h"
#include "buildlines_f.h"
#include "context_f.h"
#include "font_f.h"
#include "inites_f.h"
#include "memory_f.h"
#include "stix_f.h"
#include "units_f.h"
#include "windowdisplay_f.h"
#include "displaybox_f.h"
#include "frame_f.h"
#include "tesse_f.h"
#include "glwindowdisplay.h"

#ifdef _GL
/*
 * Math Macros conversion from
 * degrees to radians and so on...
 * All for EllipticSplit and/or GL_DrawArc
 */
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#define M_PI_DOUBLE (6.2831853718027492)

/* ((A)*(M_PI/180.0)) */
#define DEG_TO_RAD(A)   ((float)A)/57.29577957795135
#define RAD_TO_DEG(A)   ((float)A)*57.29577957795135

#define IS_ZERO(arg)                    (fabs(arg)<1.e-20)

/*If we should use a static table instead for
  performance bottleneck...*/
#define DCOS(A) ((float)cos (A))
#define DSIN(A) ((float)sin (A))
#define DACOS(A) ((float)acos (A))
#define A_DEGREE 0.017453293

/* Precision of a degree/1 
   If we need more precision 
   dision is our friend 
   1/2 degree = 0.0087266465
   1/4 degree = 0.0043633233
   or the inverse 
   24 degree = 0.41887903
   5 degree = 0.087266465
   2 degree = 0.034906586
   but best is a degree...
*/
#define TRIGO_PRECISION 1;
#define A_DEGREE_PART A_DEGREE/TRIGO_RECISION

/* Must find better one... 
   Bits methods...*/
/*for float => ( pow (N, 2))*/
/*for int  => (((int)N)<<1)*/
#define P2(N) (N*N)


#define	MIDDLE_OF(v1, v2) (((v1)+(v2))/2.0)
#define ALLOC_POINTS    300
#define MESA

#define FEEDBUFFERSIZE 32768
static ThotBool NotFeedBackMode = TRUE;
/* background color*/
static int      GL_Background[MAX_FRAME];

/*----------------------------------------------------------------------
  SetMainWindowBackgroundColor :                          
  ----------------------------------------------------------------------*/
void SetMainWindowBackgroundColor (int frame, int color)
{
  unsigned short red, green, blue;

#ifdef _GL
  /* to be sure that the frame is the current one when drawing its background */
  glRenderMode (GL_RENDER);
  GL_prepare(frame);
  GL_Background[frame] = color;
#endif /* _GL */
  if (color != -1)
    TtaGiveThotRGB (color, &red, &green, &blue);
  else
    TtaGiveThotRGB (WindowBColor, &red, &green, &blue);
  /* the 0.0 for alpha is needed for group opacity */
  glClearColor ((float)red/255., (float)green/255., (float)blue/255., 0.0);
#ifdef _GL_COLOR_DEBUG
  {
    float tmp[4];
    glGetFloatv( GL_COLOR_CLEAR_VALUE, tmp );
    TTALOGDEBUG_5( TTA_LOG_DRAW, _T("glClearColor CLEAR_VALUE(%f,%f,%f,%f) - frame=%d"),tmp[0],tmp[1],tmp[2],tmp[3],frame );
  }
#endif /* _GL_COLOR_DEBUG */
}

/*----------------------------------------------------------------------
  ResetMainWindowBackgroundColor :                          
  ----------------------------------------------------------------------*/
void ResetMainWindowBackgroundColor (int frame)
{
  unsigned short red, green, blue;
  int color = GL_Background[frame];

#ifdef _GL
  /* to be sure that the frame is the current one when drawing its background */
  GL_prepare(frame);
#endif /* _GL */
  if (color != -1)
      TtaGiveThotRGB (color, &red, &green, &blue);
  else
    TtaGiveThotRGB (WindowBColor, &red, &green, &blue);
  /* the 0.0 for alpha is needed for group opacity */
  glClearColor ((float)red/255., (float)green/255., (float)blue/255., 0.0);

#ifdef _GL_COLOR_DEBUG
  float tmp[4];
  glGetFloatv( GL_COLOR_CLEAR_VALUE, tmp );
  TTALOGDEBUG_5( TTA_LOG_DRAW, _T("glClearColor CLEAR_VALUE(%f,%f,%f,%f) - frame=%d"),tmp[0],tmp[1],tmp[2],tmp[3],frame);
#endif /* _GL_COLOR_DEBUG */
}

/*----------------------------------------------------------------------
  Clear clear the area of frame located at (x, y) and of size width x height.
  ----------------------------------------------------------------------*/
void Clear (int frame, int width, int height, int x, int y)
{
  int        bottom;

  if (GL_prepare (frame))
    {
      bottom = FrameTable[frame].FrHeight + FrameTable[frame].FrTopMargin;
      if (bottom > y+height)
	{
	  GL_SetClipping (x, bottom - (y + height), width, height);
#ifdef _GL_COLOR_DEBUG
	  {
	    float tmp[4];
	    glGetFloatv( GL_COLOR_CLEAR_VALUE, tmp );
	    TTALOGDEBUG_5( TTA_LOG_DRAW, _T("glClear CLEAR_VALUE(%f,%f,%f,%f) - frame"),tmp[0],tmp[1],tmp[2],tmp[3],frame );
	  }
#endif /* _GL_COLOR_DEBUG */
	  glClear( GL_COLOR_BUFFER_BIT );
	  /*GL_UnsetClippingRestore (TRUE);*/
	}
    }
}

/*----------------------------------------------------------------------
  GL_NotInFeedbackMode: if all openGL operations are permitted or not.
  ----------------------------------------------------------------------*/
ThotBool GL_NotInFeedbackMode ()
{
  return (NotFeedBackMode);
}

/*----------------------------------------------------------------------
  GL_prepare: If a modif has been done
  ----------------------------------------------------------------------*/
ThotBool GL_prepare (int frame)
{  
  if (frame >= 0 && frame < MAX_FRAME && NotFeedBackMode)
    {
      //#ifdef _TESTSWAP
      //FrameTable[frame].DblBuffNeedSwap = TRUE;
      //#endif /*_TESTSWAP*/

    if (FrameTable[frame].WdFrame)
      return FrameTable[frame].WdFrame->SetCurrent();
    }
  return FALSE;
}

/*----------------------------------------------------------------------
  GL_Swap : swap frontbuffer with backbuffer (display changes)
  ----------------------------------------------------------------------*/
void GL_Swap (int frame)
{
  if (frame >= 0 && frame < MAX_FRAME &&
      ViewFrameTable[frame - 1].FrReady &&
      ViewFrameTable[frame - 1].FrAbstractBox &&
      FrameTable[frame].SwapOK &&
      FrameTable[frame].WdFrame && FrameTable[frame].FrDoc &&
      documentDisplayMode[FrameTable[frame].FrDoc - 1] == DisplayImmediately)
    {
      glFlush ();
      glDisable (GL_SCISSOR_TEST);
#ifdef _GL_DEBUG
      TTALOGDEBUG_1( TTA_LOG_DRAW, _T("GL_Swap: frame=%d"), frame );
#endif /* _GL_DEBUG */
      FrameTable[frame].WdFrame->SwapBuffers();
      glEnable (GL_SCISSOR_TEST); 
      FrameTable[frame].DblBuffNeedSwap = FALSE;
    }
}

/*----------------------------------------------------------------------
  GL_SwapStop : Prevent savage swapping (causes flickering)
  ----------------------------------------------------------------------*/
void GL_SwapStop (int frame)
{
  FrameTable[frame].SwapOK = FALSE;
}
/*----------------------------------------------------------------------
  GL_SwapGet : 
  ----------------------------------------------------------------------*/
ThotBool GL_SwapGet (int frame)
{
  return FrameTable[frame].SwapOK;
}

/*----------------------------------------------------------------------
  GL_SwapEnable : 
  ----------------------------------------------------------------------*/
void GL_SwapEnable (int frame)
{
  FrameTable[frame].SwapOK = TRUE;
}
#endif /* _GL */

/*----------------------------------------------------------------------
  ComputeBoundingBox :
  Modify Bounding Box according to opengl feedback mechanism
  (after transformation, coordinates may have changed)			    
  ----------------------------------------------------------------------*/
void ComputeBoundingBox (PtrBox box, int frame, int xmin, int xmax, 
			 int ymin, int ymax)
{
#ifdef _GL
  GLfloat    feedBuffer[FEEDBUFFERSIZE];
  GLint      mode;
  int        size;
  ViewFrame  *pFrame;
 
  if (NotFeedBackMode)
    {
      glGetIntegerv (GL_RENDER_MODE, &mode);
       /* display into a temporary buffer */
      glFeedbackBuffer (FEEDBUFFERSIZE, GL_2D, feedBuffer);
      glRenderMode (GL_FEEDBACK);
      NotFeedBackMode = FALSE;
      /* display the box with transformation and clipping */
      DisplayBox (box, frame, xmin, xmax, ymin, ymax, NULL, FALSE);
      size = glRenderMode (mode);
      NotFeedBackMode = TRUE;
      if (size > 0)
        {
          /* the box is displayed */
          if (size > FEEDBUFFERSIZE)
            size = FEEDBUFFERSIZE;
          
          box->BxClipX = -1;
          box->BxClipY = -1;
          getboundingbox (size, feedBuffer, frame,
                          &box->BxClipX,
                          &box->BxClipY,
                          &box->BxClipW,
                          &box->BxClipH);    
          box->BxBoundinBoxComputed = TRUE; 
        }
      else
        {
          /* the box is not displayed */
          pFrame = &ViewFrameTable[frame - 1];
          /* */
          box->BxClipX = box->BxXOrg - (pFrame->FrXOrg?pFrame->FrXOrg:pFrame->OldFrXOrg);
          box->BxClipY = box->BxYOrg - (pFrame->FrYOrg?pFrame->FrYOrg:pFrame->OldFrYOrg);
          box->BxClipW = box->BxW;
          box->BxClipH = box->BxH;
          box->BxBoundinBoxComputed = FALSE; 
        }   
    }
#endif /* _GL */
}

#ifdef _GL
/*----------------------------------------------------------------------
  ComputeFilledBox :
  Modify Bounding Box according to opengl feedback mechanism
  (after transformation, coordinates may have changed)
  
  ----------------------------------------------------------------------*/
void ComputeFilledBox (PtrBox box, int frame, int xmin, int xmax,
                       int ymin, int ymax, ThotBool show_bgimage)
{
  GLfloat feedBuffer[4096];
  GLint   mode;
  int     size;
  
  if (NotFeedBackMode)
    {
      glGetIntegerv (GL_RENDER_MODE, &mode);
      box->BxBoundinBoxComputed = TRUE; 
      glFeedbackBuffer (4096, GL_2D, feedBuffer);
      glRenderMode (GL_FEEDBACK);
      NotFeedBackMode = FALSE;
      DrawFilledBox (box, box->BxAbstractBox, frame, NULL,
		     xmin, xmax, ymin, ymax, FALSE, TRUE, TRUE, show_bgimage);
      size = glRenderMode (mode);
      NotFeedBackMode = TRUE;
      if (size > 0)
        {
          box->BxClipX = -1;
          box->BxClipY = -1;
          getboundingbox (size, feedBuffer, frame,
                          &box->BxClipX,
                          &box->BxClipY,
                          &box->BxClipW,
                          &box->BxClipH);     
          box->BxBoundinBoxComputed = TRUE; 
          /* printBuffer (size, feedBuffer); */
        }
    }
}

/*----------------------------------------------------------------------
  InitPrintBox :  	     
  ----------------------------------------------------------------------*/
void InitPrintBox ()
{
}

/*----------------------------------------------------------------------
  ClosePrintBox :  	     
  ----------------------------------------------------------------------*/
void FinishPrintBox ()
{
 
}   


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
int GLDrawPixelsPoscript (int width, int height,
			    int xorig, int yorig,
			    GLenum format, GLenum type, 
			    unsigned char *pixels, 
			    GLfloat x, GLfloat y)
{
return 0;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
int GLString (unsigned char *buff, int lg, int frame, int x, int y,
		ThotFont font, int boxWidth, int bl, int hyphen,
	      int startABlock, int fg)
{
return 0;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
int GLText (const char *str,
	    const int fg,
	    const void *font,
	    const unsigned int fontsize, 
	    const int x, 
	    const int y,
	    const int length)
{
return 0;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void GLPrintPostScriptColor (void *rgb)
{
}
#endif /* _GL */
