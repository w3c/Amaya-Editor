/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2003
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * glbox.c : Handling of Double buffering 
 *           and Bounding box calculation, 
 *               with Opengl library both for
 *                   MS-Windows and
 *                   Unix
 *
 * Author: P. Cheyrou-lagreze (INRIA)
 *
 */
#ifdef _GL

#ifdef _WINDOWS
#include <windows.h>
#else /* _WINDOWS*/
#include <gtkgl/gtkglarea.h>
#endif /* _WINDOWS*/

#include <GL/gl.h>

#include "ustring.h"
#include "math.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"

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
#include "units_f.h"
#include "xwindowdisplay_f.h"
#include "displaybox_f.h"
#include "frame_f.h"
#include "tesse_f.h"

#include "glwindowdisplay.h"
#include "stix.h"

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


#define	MAX_STACK	50
#define	MIDDLE_OF(v1, v2) (((v1)+(v2))/2.0)
#define ALLOC_POINTS    300


#define MESA


#define FEEDBUFFERSIZE 32768


/*if just computing bounding box*/
static ThotBool NotFeedBackMode = TRUE;
/* background color*/
static int      GL_Background[MAX_FRAME];

/*Control When swapping applies*/
static ThotBool SwapOK[MAX_FRAME];

/*----------------------------------------------------------------------
  SetMainWindowBackgroundColor :                          
  ----------------------------------------------------------------------*/
void SetMainWindowBackgroundColor (int frame, int color)
{
  unsigned short red, green, blue;

#ifdef _GTK
  update_bg_colorGTK (frame, color);
#endif /*_GTK*/
  GL_Background[frame] = color;
  TtaGiveThotRGB (color, &red, &green, &blue);
  /* the 0.0 for alpha is needed for group opacity */
  glClearColor ((float)red/255, (float)green/255, (float)blue/255, 0.0);
}

/*----------------------------------------------------------------------
  ResetMainWindowBackgroundColor :                          
  ----------------------------------------------------------------------*/
void ResetMainWindowBackgroundColor (int frame)
{
  unsigned short red, green, blue;
  int color = GL_Background[frame];
  
#ifdef _GTK
  update_bg_colorGTK (frame, color);
#endif /*_GTK*/
  GL_Background[frame] = color;
  TtaGiveThotRGB (color, &red, &green, &blue);
  /* the 0.0 for alpha is needed for group opacity */
  glClearColor ((float)red/255, (float)green/255, (float)blue/255, 0.0);
} 
/*----------------------------------------------------------------------
  Clear clear the area of frame located at (x, y) and of size width x height.
  ----------------------------------------------------------------------*/
void Clear (int frame, int width, int height, int x, int y)
{
  if (GL_prepare (frame))
    { 
      y = y + FrameTable[frame].FrTopMargin;

      GL_SetClipping (x, FrameTable[frame].FrHeight - (y+height), width, height);
      glClear (GL_COLOR_BUFFER_BIT); 
      GL_UnsetClippingRestore (TRUE);
    }
}


static ThotBool NeedRedraw (int frame)
{
  ViewFrame          *pFrame;

  pFrame = &ViewFrameTable[frame - 1];
  if (pFrame->FrReady &&
      pFrame->FrAbstractBox && 
      pFrame->FrAbstractBox->AbElement)
    return TRUE;
  return FALSE;
}
/*---------------------------------------------------
  GL_NotInFeedbackMode : if all openGL operation are
  permitted or not.		    
  ----------------------------------------------------*/
ThotBool GL_NotInFeedbackMode ()
{
  return NotFeedBackMode;
}
/*----------------------------------------------------------------------
  GL_prepare: If a modif has been done
  ----------------------------------------------------------------------*/
ThotBool GL_prepare (int frame)
{  
  if (frame >= 0 && frame < MAX_FRAME && NotFeedBackMode)
    {
#ifdef _TESTSWAP
      FrameTable[frame].DblBuffNeedSwap = TRUE;
#endif /*_TESTSWAP*/
      if (FrRef[frame])
#ifdef _WINDOWS
	if (GL_Windows[frame])
	  {
	    GL_Windows[frame] = GetDC (FrRef[frame]);
	    wglMakeCurrent (GL_Windows[frame], GL_Context[frame]);	 
	    /*	ReleaseDC (FrRef[frame], GL_Windows[frame] ); */
	    return TRUE;
	  }
#else /*_WINDOWS*/
      if (FrameTable[frame].WdFrame)
	if (gtk_gl_area_make_current (GTK_GL_AREA(FrameTable[frame].WdFrame)))
	  return TRUE;
#endif /*_WINDOWS*/
    }
  return FALSE;
}
/*----------------------------------------------------------------------
  GL_Swap : swap frontbuffer with backbuffer (display changes)
  ----------------------------------------------------------------------*/
void GL_Swap (int frame)
{
  if (frame >= 0 && frame < MAX_FRAME && 
      SwapOK[frame] && 
      NeedRedraw (frame))
    {
      /* gl_synchronize ();  */
      /* glFinish (); */
      /* glFlush (); */      
      glDisable (GL_SCISSOR_TEST);

      /* glReadBuffer(GL_BACK); */
      /* glAccum (GL_LOAD, 1); */

#ifdef _WINDOWS

      if (FrRef[frame])
	if (GL_Windows[frame])
	  {
	    /*GL_Windows[frame] = GetDC (FrRef[frame]);*/
	    /*wglMakeCurrent (GL_Windows[frame], GL_Context[frame]);*/
	    SwapBuffers (GL_Windows[frame]);
	    /*or*/
	    /* glSwapBuffers(GL_Windows[frame]); */

	    ReleaseDC (FrRef[frame], GL_Windows[frame] );
	  }
#else
      if (FrameTable[frame].WdFrame)
	{
	  gtk_gl_area_swapbuffers (GTK_GL_AREA(FrameTable[frame].WdFrame));
	}
#endif /*_WINDOWS*/


      /* glDrawBuffer(GL_BACK); */
      /* glAccum (GL_RETURN, 1); */

      /* glReadBuffer (GL_FRONT); */
      /*       glRasterPos2i (0, 0); */
      /*       glCopyPixels (0, 0,  */
      /* 		    FrameTable[frame].FrWidth, FrameTable[frame].FrHeight,  */
      /* 		    GL_COLOR); */
      /*       glDrawBuffer (GL_BACK); */
      /*       glReadBuffer (GL_BACK); */

      glEnable (GL_SCISSOR_TEST); 
      FrameTable[frame].DblBuffNeedSwap = FALSE;
    }
}

/*----------------------------------------------------------------------
  GL_SwapStop : Prevent savage swapping (causes flickering)
  ----------------------------------------------------------------------*/
void GL_SwapStop (int frame)
{
  SwapOK[frame] = FALSE;
}
/*----------------------------------------------------------------------
  GL_SwapGet : 
  ----------------------------------------------------------------------*/
ThotBool GL_SwapGet (int frame)
{
  return SwapOK[frame];
}
/*----------------------------------------------------------------------
  GL_SwapEnable : 
  ----------------------------------------------------------------------*/
void GL_SwapEnable (int frame)
{
  SwapOK[frame] = TRUE;
}


#ifdef _WINDOWS
/*----------------------------------------------------------------------
  WinGL_Swap : specific to windows
  ----------------------------------------------------------------------*/
void WinGL_Swap (HDC hDC)
{
  /* glSwapBuffers (hDC); */
  SwapBuffers (hDC);
}
#endif /*_WINDOWS*/




/* /\*--------------------------------------------------- */
/*   PrintBox :  	     */
/*   ----------------------------------------------------*\/ */
/* void PrintBox (PtrBox box, int frame,  */
/* 	       int xmin, int xmax,  */
/* 	       int ymin, int ymax) */
/* { */
/*   GLfloat feedBuffer[FEEDBUFFERSIZE]; */

/*   glFeedbackBuffer (FEEDBUFFERSIZE, GL_3D_COLOR, feedBuffer); */
/*   NotFeedBackMode = FALSE;   */
/*   glRenderMode (GL_FEEDBACK); */
/*   DisplayBox (box, frame, xmin, xmax, ymin, ymax); */
/*   NotFeedBackMode = TRUE; */
/*   GLParseFeedbackBuffer (feedBuffer); */
/*   NotFeedBackMode = TRUE; */
/* } */


/*---------------------------------------------------
  ComputeBoundingBox :
  Modify Bounding Box according to opengl feedback mechanism
  (after transformation, coordinates may have changed)			    
  ----------------------------------------------------*/
void ComputeBoundingBox (PtrBox box, int frame, 
			 int xmin, int xmax, 
			 int ymin, int ymax)
{
  GLfloat feedBuffer[FEEDBUFFERSIZE];
  GLint   size;
  
  if (NotFeedBackMode)
    {
      glFeedbackBuffer (FEEDBUFFERSIZE, GL_2D, feedBuffer);
      NotFeedBackMode = FALSE;  
      glRenderMode (GL_FEEDBACK);
      DisplayBox (box, frame, xmin, xmax, ymin, ymax);
      size = glRenderMode (GL_RENDER);
      NotFeedBackMode = TRUE;
      if (size > 0)
	{
	  if (size > FEEDBUFFERSIZE)
	    size = FEEDBUFFERSIZE;

	  box->BxClipX = -1;
	  box->BxClipY = -1;
	  getboundingbox (size, feedBuffer, frame,
			  &box->BxClipX,
			  &box->BxClipY,
			  &box->BxClipW,
			  &box->BxClipH);    
  
	  /* printBuffer (size, feedBuffer); */ 
	  
	  box->BxBoundinBoxComputed = TRUE; 
	}
      else
	{
	  box->BxClipX = box->BxXOrg;
	  box->BxClipY = box->BxYOrg;
	  box->BxClipW = box->BxW;
	  box->BxClipH = box->BxH;
	  box->BxBoundinBoxComputed = FALSE; 
	}   
    }
}

/*---------------------------------------------------
  ComputeFilledBox :
  Modify Bounding Box according to opengl feedback mechanism
  (after transformation, coordinates may have changed)			    
  ----------------------------------------------------*/
void ComputeFilledBox (PtrBox box, int frame, int xmin, int xmax, int ymin, int ymax)
{
  GLfloat feedBuffer[4096];
  GLint size;
  
  if (NotFeedBackMode)
    {
      box->BxBoundinBoxComputed = TRUE; 
      glFeedbackBuffer (2048, GL_2D, feedBuffer);
      NotFeedBackMode = FALSE;
      glRenderMode (GL_FEEDBACK);
      DrawFilledBox (box->BxAbstractBox, frame, xmin, xmax, ymin, ymax);
      size = glRenderMode (GL_RENDER);
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


 

/*---------------------------------------------------
  InitPrintBox :  	     
 ----------------------------------------------------*/
void InitPrintBox ()
{
}
/*---------------------------------------------------
  ClosePrintBox :  	     
 ----------------------------------------------------*/
void FinishPrintBox ()
{
 
}   

GLint GLDrawPixelsPoscript (GLsizei width, GLsizei height,
			    GLint xorig, GLint yorig,
			    GLenum format, GLenum type, 
			    unsigned char *pixels, 
			    GLfloat x, GLfloat y)
{
return 0;
}

int GLString (unsigned char *buff, int lg, int frame, int x, int y,
		PtrFont font, int boxWidth, int bl, int hyphen,
	      int startABlock, int fg, int shadow)
{
return 0;
}

GLint GLText (const char *str,
	      const int fg,
	      const void *font,
	      const unsigned int fontsize, 
	      const int x, 
	      const int y,
	      const int length)
{
return 0;
}
void GLPrintPostScriptColor(void *rgb)
{
}
#endif /* _GL */
