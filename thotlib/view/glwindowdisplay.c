/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2003
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * GLwindowdisplay.c : handling of low level drawing routines, 
 *                      for Opengl 
 *
 * Author:  P. Cheyrou-lagreze (INRIA)
 *
 */
#ifdef _GL

#include "ustring.h"
#include "math.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"
#include "appdialogue.h"
#include "picture.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "boxes_tv.h"
#include "font_tv.h"
#include "frame_tv.h"
#include "units_tv.h"
#include "edit_tv.h"
#include "appdialogue_tv.h"
#include "thotcolor_tv.h"

#include "appli_f.h"
#include "buildboxes_f.h"
#include "displaybox_f.h"
#include "displayselect_f.h"
#include "boxlocate_f.h"
#include "buildlines_f.h"
#include "context_f.h"
#include "font_f.h"
#include "inites_f.h"
#include "memory_f.h"
#include "units_f.h"
#include "xwindowdisplay_f.h"
#include "frame_f.h"
#include "animbox_f.h"
#include "picture_f.h"
#include "tesse_f.h"
#include "applicationapi_f.h"


#ifdef _GTK
#include <gtkgl/gtkglarea.h>
/* Unix timer */
#include <unistd.h>
#include <sys/timeb.h>
#else /*WINDOWS*/
#include <windows.h>
/* Win32 opengl context based on frame number*/
static HDC   GL_Windows[50];	
static HGLRC GL_Context[50];
#endif /*_GTK*/

#include <GL/gl.h>
#include <GL/glu.h>

#ifndef CALLBACK
#define CALLBACK
#endif

#include "openglfont.h"
#include "glwindowdisplay.h"

#include <math.h> 
#include <stdio.h> 


#define ALLOC_POINTS    300

/*
 * Math Macros conversion from
 * degrees to radians and so on...
 * All for EllipticSplit and/or GL_DrawArc
 */

#define PRECISION float

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_PI_DOUBLE
#define M_PI_DOUBLE (6.2831853718027492)
#endif

/* ((A)*(M_PI/180.0)) */
#define DEG_TO_RAD(A)   ((PRECISION)A)/57.29577957795135
#define RAD_TO_DEG(A)   ((PRECISION)A)*57.29577957795135


/*If we should use a static table instead for
  performance bottleneck...*/
#define DCOS(A) (cos (A))
#define DSIN(A) (sin (A))
#define DTAN(A) (tan (A))
#define DACOS(A) (acos (A))
#define DACOS(A) (acos (A))

/*#define A_DEGREE 0.017453293*/
#define A_DEGREE ((M_PI / 24))

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

/* Arc Precision Drawings */
#define SLICES 360
#define SLICES_SIZE 361

/* Animation Smoothness*/
#define FPS 25 /*Frame Per Second*/
#define INTERVAL 0.0005 /*1/FPS*/ /* 1/25 */
#define FRAME_TIME 5 /*(INTERVAL*1000) */
/* milliseconds */


/*--------- STATICS ------*/

/*Current Thickness*/
static int      S_thick;

/* background color*/
static int      GL_Background[50];

/*if no 3d card available*/
static ThotBool Software_Mode = TRUE;

/*if just computing bounding box*/
static ThotBool NotFeedBackMode = TRUE;

/*One Timer to rule them all */
static int AnimTimer = 0; 


ThotBool GL_Err() 
{
  GLenum errCode = GL_NO_ERROR;

  if((errCode = glGetError ()) != GL_NO_ERROR)
    {
#ifdef _PCLDEBUG
#ifdef _GTK
      g_print ("\n%s :", (char*) gluErrorString (errCode));
#else /*_GTK*/
      WinErrorBox (NULL, (char*) gluErrorString (errCode));;
#endif /*_GTK*/
#endif /*_PCLDEBUG*/
      return TRUE;
    }
  else 
    return FALSE;
}



#ifndef _WIN_PRINT
/*----------------------------------------------------------------------
  ClearAll clear the frame .
  ----------------------------------------------------------------------*/
void ClearAll (int frame)
{  
  if (GL_prepare (frame))
  {
    /* SetMainWindowBackgroundColor (frame, GL_Background[frame]); */
    glClear (GL_COLOR_BUFFER_BIT); 
    
  }
}

#ifndef _NOSHARELIST

static int Shared_Context=-1;
/*----------------------------------------------------------------------
  GetSharedContext : get the name of the frame used as shared context
  ----------------------------------------------------------------------*/
int GetSharedContext ()
{
  if (Shared_Context != -1)
    return Shared_Context;
  return -1;
}
/*----------------------------------------------------------------------
  SetSharedContext : set the name of the frame used as shared context
  ----------------------------------------------------------------------*/
void SetSharedContext (int frame)
{
  Shared_Context = frame;
}
/*----------------------------------------------------------------------
  GL_KillFrame : if realeasing a source sharing context, name a new one 
as the source sharing context
  ----------------------------------------------------------------------*/
static void GL_KillFrame (int frame)
{
  int i;

  if (frame != Shared_Context)
    return;
  for (i = 0 ; i <= MAX_FRAME; i++)
    {  
#ifndef _WINDOWS 
      if (i != Shared_Context && FrameTable[i].WdFrame)
#else /* _WINDOWS */
	if (i != Shared_Context && GL_Context[i])
#endif /* _WINDOWS */
	{
	Shared_Context = i;
	return;
	}
    }
}
#endif /*_NOSHARELIST*/

#ifdef _GTK
/*--------------------------------------------------------------
 * Make the window background the same as the OpenGL one.  This
 * is used to avoid flickers when the widget size changes.
 * This is fixed in GTK+-2.0 but doesn't hurt.
 --------------------------------------------------------------*/
void update_bg_colorGTK (int frame, int color)
{
  GtkWidget *gl_widget;
  GdkColormap *cmap;
  GdkColor col;
  unsigned short red, green, blue;

  gl_widget = FrameTable[frame].WdFrame;  

  cmap = gtk_widget_get_colormap (gl_widget);
  TtaGiveThotRGB (color, &red, &green, &blue);
  col.red = (guint16) red*257;
  col.green = (guint16) green*257;
  col.blue = (guint16) blue*257;
  
  gdk_colormap_alloc_color (cmap, &col, FALSE, TRUE);
  gdk_window_set_background (gl_widget->window, &col);
}

#endif /*_GTK*/
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
  Clear clear the area of frame located at (x, y) and of size width x height.
  ----------------------------------------------------------------------*/
void Clear (int frame, int width, int height, int x, int y)
{
  if (GL_prepare (frame))
    { 
      /* FrameTable[frame].DblBuffNeedSwap = TRUE;  */
      y = y + FrameTable[frame].FrTopMargin;
      GL_SetForeground (GL_Background[frame]); 
      glBegin (GL_QUADS);
      glVertex2i (x, y); 
      glVertex2i (x, y + height);
      glVertex2i (x +  width, y + height);
      glVertex2i (x + width, y);
      glEnd ();
    } 
}
#endif /*_WIN_PRINT*/

#ifdef _WINDOWS 

/*----------------------------------------------------------------------
  ChoosePixelFormatEx : Get Pixel format descriptor in order to request it
to windows
  ----------------------------------------------------------------------*/
int ChoosePixelFormatEx (HDC hdc)
{ 
  int wbpp = 32; 
  int wdepth = 16; 
  int wdbl = 1; 
  int wacc = 1; 
  PIXELFORMATDESCRIPTOR pfd; 
  int num;
  unsigned int maxqual=0; 
  int maxindex=0;
  int max_bpp, max_depth, max_dbl, max_acc;
  int i;
  int bpp;
  int depth;
  int pal, mcd, soft, icd,opengl, window, bitmap, dbuff;
  unsigned int q=0;

  ZeroMemory(&pfd,sizeof(pfd)); 
  pfd.nSize=sizeof(pfd); 
  pfd.nVersion=1;
  num=DescribePixelFormat(hdc,1,sizeof(pfd),&pfd);
  if (num==0) 
    return 0;
  for (i=1; i<=num; i++)
    { 
      ZeroMemory(&pfd,sizeof(pfd)); 
      pfd.nSize=sizeof(pfd); 
      pfd.nVersion=1;
      DescribePixelFormat(hdc,i,sizeof(pfd),&pfd);
      bpp=pfd.cColorBits;
      depth=pfd.cDepthBits;
      pal=(pfd.iPixelType==PFD_TYPE_COLORINDEX);
      mcd=((pfd.dwFlags & PFD_GENERIC_FORMAT) && (pfd.dwFlags & PFD_GENERIC_ACCELERATED));
      soft=((pfd.dwFlags & PFD_GENERIC_FORMAT) && !(pfd.dwFlags & PFD_GENERIC_ACCELERATED));
      icd=(!(pfd.dwFlags & PFD_GENERIC_FORMAT) && !(pfd.dwFlags & PFD_GENERIC_ACCELERATED));
      opengl=(pfd.dwFlags & PFD_SUPPORT_OPENGL);
      window=(pfd.dwFlags & PFD_DRAW_TO_WINDOW);
      bitmap=(pfd.dwFlags & PFD_DRAW_TO_BITMAP);
      dbuff=(pfd.dwFlags & PFD_DOUBLEBUFFER);
      if (opengl && window) 
	q=q+0x8000;
      if (wdepth==-1 || (wdepth>0 && depth>0)) 
	q=q+0x4000;
      if (wdbl==-1 || (wdbl==0 && !dbuff) || (wdbl==1 && dbuff)) 
	q=q+0x2000;
      if (wacc==-1 || (wacc==0 && soft) || (wacc==1 && (mcd || icd))) 
	q=q+0x1000;
      if (mcd || icd) 
	q=q+0x0040; 
      if (icd) 
	q=q+0x0002;
      if (wbpp==-1 || (wbpp==bpp)) 
	q=q+0x0800;
      if (bpp>=16) 
	q=q+0x0020; 
      if (bpp==16) 
	q=q+0x0008;
      if (wdepth==-1 || (wdepth==depth)) 
	q=q+0x0400;
      if (depth>=16) 
	q=q+0x0010; 
      if (depth==16) 
	q=q+0x0004;
      if (!pal) 
	q=q+0x0080;
      if (bitmap) 
	q=q+0x0001;
      if (q>maxqual) 
	{
	  maxqual=q; 
	  maxindex=i;
	  max_bpp=bpp; 
	  max_depth=depth; 
	  max_dbl=dbuff?1:0; 
	  max_acc=soft?0:1;
	}
    }
  if (maxindex==0) 
    return maxindex;
  return maxindex;
}
/*----------------------------------------------------------------------
  init_pfd : init the struct describing the screen we want
 ----------------------------------------------------------------------*/
void init_pfd ()
{
  static PIXELFORMATDESCRIPTOR myPFD;

  ZeroMemory(&myPFD, sizeof(myPFD));
  myPFD.nSize    = sizeof(myPFD);
  myPFD.nVersion = 1;
  myPFD.dwFlags  = PFD_DRAW_TO_WINDOW |
    PFD_SUPPORT_OPENGL |
    PFD_DOUBLEBUFFER;
  myPFD.cColorBits = 16;
  myPFD.cDepthBits = 16;
  myPFD.iPixelType = PFD_TYPE_RGBA;
  myPFD.iLayerType = PFD_MAIN_PLANE;

}

/*----------------------------------------------------------------------
  GL_SetupPixelFormat : Sets up opengl buffers pixel format.
  Double Buffer, RGBA (32 bits), 
  no depth (z-buffer), no stencil (boolean buffer), no alpha (transparency), 
  no accum (special effect like multisampling, antialiasing), no aux (all purpose buffers),
  no pbuffers (?) buffers...
  ----------------------------------------------------------------------*/
static void GL_SetupPixelFormat (HDC hDC)
{
  static PIXELFORMATDESCRIPTOR pfd = 
    {
      sizeof(PIXELFORMATDESCRIPTOR),  /* size */
      1,                              /* version */
      PFD_DRAW_TO_WINDOW |			/* Format Must Support Window*/
      PFD_SUPPORT_OPENGL |			/* Format Must Support OpenGL*/
      PFD_DOUBLEBUFFER   |            /* support double-buffering */
      PFD_DEPTH_DONTCARE |            /* If Depth is obligated by hardware*/
      PFD_GENERIC_ACCELERATED,        /* We try to get hardware here */       
      PFD_TYPE_RGBA,                  /* color type */
      24,                             /* prefered color depth */
      0, 0, 0, 0, 0, 0,               /* color bits (ignored) */
      1,                              /* no alpha buffer */
      0,                              /* alpha bits (ignored) */
      0,                              /* no accumulation buffer */
      0, 0, 0, 0,                     /* accum bits (ignored) */
      0,                              /* depth buffer */
      0,                              /* no stencil buffer */
      0,                              /* no auxiliary buffers */
      PFD_MAIN_PLANE,                 /* main layer */
      0,                              /* reserved */
      0, 0, 0,                        /* no layer, visible, damage masks */
    };
  int pixelFormat;

  /*
    ----Switch to software opengl----
    bool bSupportsgeneric_format = (pfd->dwFlags & PFD_GENERIC_FORMAT) ==
    PFD_GENERIC_FORMAT;
    bool bSupportsgeneric_accelerated = (pfd->dwFlags &
    PFD_GENERIC_ACCELERATED) == PFD_GENERIC_ACCELERATED;
    
    bool bAccelerated = false;
    
    if(( bSupportsgeneric_format && bSupportsgeneric_accelerated ) ||
    (!bSupportsgeneric_format && !bSupportsgeneric_accelerated)  )
    {
    bAccelerated = true;
    }
  */
  pixelFormat = ChoosePixelFormat (hDC, &pfd);
  if (pixelFormat == 0) 
    {
      MessageBox(WindowFromDC(hDC), "ChoosePixelFormat failed.", "Error",
		 MB_ICONERROR | MB_OK);
      exit(1);
    }

  if (SetPixelFormat(hDC, pixelFormat, &pfd) != TRUE) 
    {
      MessageBox(WindowFromDC(hDC), "SetPixelFormat failed.", "Error",
		 MB_ICONERROR | MB_OK);
      exit(1);
    }

}

/*----------------------------------------------------------------------
  GL_Win32ContextInit : Turn a win32 windows into an opengl drawing canvas, 
  setting up pxel format,
  Creating the frame number if needed.
  ----------------------------------------------------------------------*/
void GL_Win32ContextInit (HWND hwndClient, int frame)
{
  static ThotBool dialogfont_enabled = FALSE;
  int frame_index;
  HGLRC hGLRC;
  HDC hDC;
  ThotBool found;

  hDC = 0;
  hDC = GetDC (hwndClient);	
  if (!hDC) 
    {      
      MessageBox(NULL, "ERROR!", "No device context", MB_OK); 
      return;    
    }
  else if (dialogfont_enabled)
    {
      for (frame_index = 0 ; frame_index <= MAX_FRAME; frame_index++)
	{  
	  if (GL_Windows[frame_index] == hDC) 
	    return;
	}
    }
  found = FALSE;       
  if (frame <= 0)
    {
      frame = 1;
      while (frame <= MAX_FRAME && !found)
	{
	  /* Seeks a free frame slot */
	  found = (FrameTable[frame].FrDoc == 0 && FrameTable[frame].WdFrame != 0);
	  if (!found)
	    frame++;
	}	
      if (!found)
	{
	  frame = 1;
	  while (frame <= MAX_FRAME && !found)
	    {
	      /* Seeks a free frame slot */
	      found = (FrameTable[frame].WdFrame == 0);
	      if (!found)
		frame++;
	    }
	}
    }
  GL_SetupPixelFormat (hDC);
  hGLRC = wglCreateContext (hDC);
  GL_Windows[frame] = hDC;
  GL_Context[frame] = hGLRC;
  if (wglMakeCurrent (hDC, hGLRC))
    {
      SetGlPipelineState ();
      if (!dialogfont_enabled)
	{
	  InitDialogueFonts ("");
	  dialogfont_enabled = TRUE;
	  for (frame_index = 0 ; frame_index <= MAX_FRAME; frame_index++)
	    {  
	      if (frame_index != frame)
		{
		  GL_Windows[frame_index] = 0;
		  GL_Context[frame_index] = 0;
		}
	    }
	}
    }
#ifdef _SHARELIST
  if (GetSharedContext () != 1) 
    wglShareLists (GL_Context[GetSharedContext ()], hGLRC);
  else
    SetSharedContext (frame);
#endif /*_SHARELIST*/
  ActiveFrame = frame;
}

/*----------------------------------------------------------------------
  GL_Win32ContextClose : Free opengl contexts
  ----------------------------------------------------------------------*/
void GL_Win32ContextClose (int frame, HWND hwndClient)
{
  /* make our context 'un-'current */
  wglMakeCurrent (NULL, NULL);
  /* delete the rendering context */
  if (GL_Context[frame])
    wglDeleteContext (GL_Context[frame]);
  if (GL_Windows[frame])
    ReleaseDC (hwndClient, GL_Windows[frame]);
  GL_Windows[frame] = 0;
  GL_Context[frame] = 0;
  GL_KillFrame (frame);
}

/*----------------------------------------------------------------------
  MyTimerProc : Call drawing upon timer calls
  ----------------------------------------------------------------------*/
VOID CALLBACK MyTimerProc (HWND hwnd, UINT message, UINT idTimer, DWORD dwTime)     
{ 
 GL_DrawAll ();   
} 

#endif /*_WINDOWS*/

static int Opacity = 1000;
static int FillOpacity = 1000;
static int StrokeOpacity = 1000;
static ThotBool Fill_style = TRUE;

/*------------------------------------------
  GL_SetOpacity :
  ---------------------------------------------*/
void GL_SetOpacity (int opacity)
{
  Opacity = (int) ((opacity * 255)/1000);
}
/*------------------------------------------
  GL_SetStrokeOpacity :
  ---------------------------------------------*/
void GL_SetStrokeOpacity (int opacity)
{
  StrokeOpacity = (int) ((opacity * 255)/1000);
}
/*------------------------------------------
  GL_SetFillOpacity  :
  ---------------------------------------------*/
void GL_SetFillOpacity (int opacity)
{
  FillOpacity = (int) ((opacity * 255)/1000);
}

static int x_previous_clip = 0;
static int y_previous_clip = 0;
static int width_previous_clip = 0;
static int height_previous_clip = 0;
/*----------------------------------------------------------------------
  GL_DestroyFrame :
  Close Opengl pipeline
  ----------------------------------------------------------------------*/
void  GL_DestroyFrame (int frame)
{
#ifndef _NOSHARELIST
  GL_KillFrame (frame);
#endif /*_NOSHARELIST*/
  FreeAllPicCacheFromFrame (frame);
}
/*----------------------------------------------------------------------
  GL_SetClipping : prevent drawing outside this rectangle
  ----------------------------------------------------------------------*/
void GL_SetClipping (int x, int y, int width, int height)
{
  glEnable (GL_SCISSOR_TEST);
  glScissor (x, y, width, height);
  if (width_previous_clip == 0 && height_previous_clip == 0)
    {
      x_previous_clip = x;
      y_previous_clip = y;
      width_previous_clip = width;
      height_previous_clip = height;
    }
}
/*----------------------------------------------------------------------
  GL_UnsetClippingRestore : restore previous clipping or 
free the drawing from it
  ----------------------------------------------------------------------*/
void GL_UnsetClippingRestore (ThotBool Restore)
{  
  glDisable (GL_SCISSOR_TEST);
  if (Restore)
    {
      if (width_previous_clip != 0 && height_previous_clip != 0)
	{
	  GL_SetClipping (x_previous_clip, y_previous_clip, 
			  width_previous_clip, height_previous_clip);
	  width_previous_clip = 0;
	  height_previous_clip = 0;
	}
    }
  else
    {
      width_previous_clip = 0;
      height_previous_clip = 0;
    }
}
/*----------------------------------------------------------------------
  GL_UnsetClipping : free the drawing from clipping
  ----------------------------------------------------------------------*/
void GL_UnsetClipping  (int x, int y, int width, int height)
{  
  glDisable (GL_SCISSOR_TEST);
  if (width && height)
    {       
      GL_SetClipping (x, y, width, height);
    }
  else
    { 
      x_previous_clip = 0;
      y_previous_clip = 0;
      width_previous_clip = 0;
      height_previous_clip = 0;
    }
}
/*----------------------------------------------------------------------
  GL_GetCurrentClipping : get  the clipping
  ----------------------------------------------------------------------*/
void GL_GetCurrentClipping (int *x, int *y, int *width, int *height)
{  
  *x = x_previous_clip;
  *y = y_previous_clip;
  *width = width_previous_clip;
  *height= height_previous_clip;
}
/*----------------------------------------------------------------------
  GL_SetForeground : set color before drawing a or many vertex
  ----------------------------------------------------------------------*/
void GL_SetForeground (int fg)
{
  unsigned short red, green, blue, us_opac;

  if (Fill_style)
    us_opac= (GLubyte) FillOpacity;
  else
    {
      us_opac= (GLubyte) StrokeOpacity;
      Fill_style = TRUE;	
    }
    
  TtaGiveThotRGB (fg, &red, &green, &blue);
  glColor4ub ((GLubyte) red,  (GLubyte) green, (GLubyte) blue, (GLubyte) us_opac);    
}


/*----------------------------------------------------------------------
  GL_SetPicForeground : set opacity before drawing a or many vertex
  ----------------------------------------------------------------------*/
void GL_SetPicForeground ()
{
  unsigned short us_opac;

  us_opac= (GLubyte) FillOpacity;
  glColor4ub ((GLubyte) 255,  (GLubyte) 255, (GLubyte) 255, (GLubyte) us_opac);    
}
/*----------------------------------------------------------------------
  InitDrawing update the Graphic Context accordingly to parameters.
  The parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void InitDrawing (int style, int thick, int fg)
{
  float float_thick;
  
  float_thick = (GLfloat) thick;
  
  if (style >= 5)
    {
      /* solid */
      if (thick)
	{
	  S_thick = thick;
	  glLineWidth (float_thick); 
	  glPointSize (float_thick); 
	}
      else
	{
	  glLineWidth ((GLfloat) 0.5); 
	  glPointSize ((GLfloat) 0.5); 
	}
      glDisable (GL_LINE_STIPPLE);
    }
  else
    {
      if (style == 3)
	/* dotted */
	{
	  glEnable (GL_LINE_STIPPLE);
	  glLineStipple (1, 0x1111); 
	}
      else
	/* dashed */
	{
	  glEnable (GL_LINE_STIPPLE);
	  glLineStipple (1, 0x0F0F); 
	}
      if (thick)
	{
	  S_thick = thick;
	  glLineWidth (float_thick); 
	  glPointSize (float_thick); 
	}
      else
	{
	  glLineWidth ((GLfloat) 0.5); 
	  glPointSize ((GLfloat) 0.5); 
	}
     
    }
  Fill_style = FALSE;  
  GL_SetForeground (fg);
}

/*----------------------------------------------------------------------
  GL_VideoInvert : 
  using a transparent yellow instead of inverting... much simpler !   
  ----------------------------------------------------------------------*/
void GL_VideoInvert (int width, int height, int x, int y)
{ 
  /*a blend func like that could be coool ? 
    (GL_ONE_MINUS_DST_COLOR,GL_ZERO) */
  glColor4ub (127, 127, 127, 80);
  glBegin (GL_QUADS);
  glVertex2i (x, y);
  glVertex2i (x + width, y);
  glVertex2i (x +  width, y + height);
  glVertex2i (x, y + height);
  glEnd (); 
}

/*----------------------------------------------------------------------
  GL_DrawEmptyRectangle Outlined rectangle
  ----------------------------------------------------------------------*/
void  GL_DrawEmptyRectangle (int fg, int x, int y, int width, int height)
{ 
  Fill_style = FALSE;	
  GL_SetForeground (fg);
  if (S_thick > 1 && 0)
    {
      glBegin (GL_POINTS);/*joining angles*/
      glVertex2i (x, y );
      glVertex2i (x, y + height);
      glVertex2i (x + width, y + height);
      glVertex2i (x + width, y);
      glVertex2i (x, y );
      glEnd ();
    }  
  glBegin (GL_LINE_LOOP);
  glVertex2i (x, y );
  glVertex2i (x + width, y);
  glVertex2i (x +  width, y + height);
  glVertex2i (x, y + height);
  /* glVertex2i (x, y ); */
  glEnd (); 
  
}
/*----------------------------------------------------------------------
  GL_DrawEmptyRectangle Outlined rectangle
  ----------------------------------------------------------------------*/
void  GL_DrawEmptyRectanglef (int fg, float x, float y, float width, float height)
{ 
  Fill_style = FALSE;	
  GL_SetForeground (fg);
  if (S_thick > 1 && 0)
    {
      glBegin (GL_POINTS);/*joining angles*/
      glVertex2f (x, y );
      glVertex2f (x, y + height);
      glVertex2f (x + width, y + height);
      glVertex2f (x + width, y);
      glVertex2f (x, y );
      glEnd ();
    }  
  glBegin (GL_LINE_LOOP);
  glVertex2f (x, y );
  glVertex2f (x + width, y);
  glVertex2f (x +  width, y + height);
  glVertex2f (x, y + height);
  /* glVertex2i (x, y ); */
  glEnd ();   
}
/*----------------------------------------------------------------------
  GL_DrawRectangle
  (don't use glrect because it's exactly the same but require opengl 1.2)
  ----------------------------------------------------------------------*/
void GL_DrawRectangle (int fg, int x, int y, int width, int height)
{
  GL_SetForeground (fg);
  glBegin (GL_QUADS);
  glVertex2i (x, y );
  glVertex2i (x + width, y);
  glVertex2i (x +  width, y + height);
  glVertex2i (x, y + height);
  glEnd ();
}
/*----------------------------------------------------------------------
  GL_DrawRectangle
  (don't use glrect because it's exactly the same but require opengl 1.2)
  ----------------------------------------------------------------------*/
void GL_DrawRectanglef (int fg, float x, float y, float width, float height)
{
  GL_SetForeground (fg);
  glBegin (GL_QUADS);
  glVertex2f (x, y );
  glVertex2f (x + width, y);
  glVertex2f (x +  width, y + height);
  glVertex2f (x, y + height);
  glEnd ();
}
/*----------------------------------------------------------------------
  GL_DrawLine
  ----------------------------------------------------------------------*/
void GL_DrawLine (int x1, int y1, int x2, int y2, ThotBool round)
{
  if (S_thick > 1 && round)
    {
      /* round line join*/ 
      glBegin (GL_POINTS);
      glVertex2i (x1, y1);
      glVertex2i (x2, y2);
      glEnd ();
    }
  glBegin (GL_LINES) ;
  glVertex2i (x1, y1);
  glVertex2i (x2, y2);
  glEnd ();
    
}
/*----------------------------------------------------------------------
  GL_DrawSegments
  ----------------------------------------------------------------------*/
void GL_DrawSegments (XSegment *point, int npoints)
{
  int i;
  
  if (S_thick > 1)
    {
      glBegin (GL_POINTS); 
      for(i=0; i < npoints; i++) 
	{
	  glVertex2f ((point + i)->x1 , 
		      (point + i)->y1);
	  glVertex2f ((point + i)->x2 , 
		      (point + i)->y2);
	}
      glEnd ();
    }
  glBegin (GL_LINES) ;
  for(i=0; i < npoints; i++) 
    {
      glVertex2f ((point + i)->x1 , 
		  (point + i)->y1);
      glVertex2f ((point + i)->x2 , 
		  (point + i)->y2);   	
    }    
  glEnd ();
}

/*----------------------------------------------------------------------
  GL_DrawArc : Draw an arc
  ----------------------------------------------------------------------*/
void GL_DrawArc (int x, int y, 
		 int w, int h, 
		 int startAngle, int sweepAngle, 
		 ThotBool filled)
{
  GLint     i, slices;

  PRECISION angleOffset;
  PRECISION sinCache[SLICES_SIZE];
  PRECISION cosCache[SLICES_SIZE];
  PRECISION y_cache[SLICES_SIZE];
  PRECISION x_cache[SLICES_SIZE];
  PRECISION angle;
  PRECISION fastx, fasty, width, height;


  width  = ((PRECISION)w) / 2;
  height = ((PRECISION)h) / 2;
  fastx  = ((PRECISION)x) + width; 
  fasty  = ((PRECISION)y) + height;


  if (w < 10 && h < 10)
    {
      glPointSize ((float)0.1);
      slices = 36;
    }
  else
    slices = SLICES;

  startAngle = startAngle;
  sweepAngle = sweepAngle;
 
  /* Cache is the vertex locations cache */
  angleOffset = (PRECISION) (startAngle / 180.0 * M_PI);
  for (i = 0; i <= slices; i++) 
    {
      angle = angleOffset + (PRECISION) ((M_PI * sweepAngle) / 180.0) * i / slices;
      cosCache[i] = (PRECISION) DCOS(angle);
      sinCache[i] = (PRECISION) DSIN(angle);
    }

  if (sweepAngle == 360.0) 
    {
      sinCache[slices] = sinCache[0];
      cosCache[slices] = cosCache[0];
    }

  
  for (i = 0; i <= slices; i++)
    {	
      x_cache[i] = fastx + (width * cosCache[i]);
      y_cache[i] = fasty - (height * sinCache[i]);
    }

  if (filled)
    {
      glBegin (GL_TRIANGLE_FAN);
      /* The center */
      glVertex2d (fastx, fasty);
      for (i = 0; i <= slices; i++)
	{
	  glVertex2d (x_cache[i], 
		      y_cache[i]);
	}
      glEnd();
    }

  if (!filled)
    {
      if (w < 20 && h < 20)
	glBegin(GL_POINTS);
      else
	glBegin(GL_LINE_STRIP);

      slices--;
      for (i = 0; i <= slices; i++)
	{
	  glVertex2d (x_cache[i], 
		      y_cache[i]);
	}
      glEnd();
    }
  
}



/*----------------------------------------------------------------------
  GL_DrawLines
  (Not static because used in geom.c)
  ----------------------------------------------------------------------*/
void GL_DrawLines (ThotPoint *point, int npoints)
{
  int i, k;
  
  if (S_thick > 1)
    {
      /* Joinning if a certain thickness ?*/
      glBegin (GL_POINTS); 
      for (i=0; i<npoints; i++)
	{	  
	  glVertex2fv ((GLfloat *) (point+i));
	}
      glEnd (); 
    }
  k = 0;
  glBegin (GL_LINE_STRIP);
  for (i = 0; i < npoints; i++)
    { 
      glVertex2fv ((GLfloat *) (point+i));
    }
  glEnd ();
  
}

/*----------------------------------------------------------------------
  GL_DrawPolygon : tesselation handles 
  convex, concave and polygon with holes
  ----------------------------------------------------------------------*/
void GL_DrawPolygon (ThotPoint *points, int npoints)
{
  MakefloatMesh (points, npoints);
}


/*------------------------------------------
  GL_Point :
  Draw a point using GL primitives
  -------------------------------------------*/
void GL_Point (int fg, float width, float x, float y)
{
  GL_SetForeground (fg);
  glPointSize (width);
  glBegin (GL_POINTS);
  glVertex2f (x, y);
  glEnd ();
}


#ifdef _PIXELFONT
/*----------------------
  ResetPixelTransferBias
  ------------------------*/
static void ResetPixelTransferBias ()
{
  /*
    static GLuint Precompiled = 0;

    if (!Precompiled)
    {
    Precompiled = glGenLists (1);
    glNewList (Precompiled,  GL_COMPILE);
  */    
  glPixelTransferf (GL_RED_BIAS, 0.0); 
  glPixelTransferf (GL_GREEN_BIAS, 0.0); 
  glPixelTransferf (GL_BLUE_BIAS, 0.0); 

  /*
    glEndList ();
    }
    glCallList (Precompiled); 
  */
}
#define BIT8DIVIDE(A) ((float)A /256)
/*----------------------
  SetPixelTransferBias
  ------------------------*/
static void SetPixelTransferBias (int fg)
{
  unsigned short red, green, blue;

  TtaGiveThotRGB (fg, &red, &green, &blue);

  glPixelTransferf (GL_RED_BIAS, BIT8DIVIDE(red));
  glPixelTransferf (GL_GREEN_BIAS, BIT8DIVIDE(green));
  glPixelTransferf (GL_BLUE_BIAS,  BIT8DIVIDE(blue));
}
#endif /*_PIXELFONT*/


/*----------------------------------------------------------------------
  TranslateChars replaces in the text space chars to their visual
  equivalents and the character 128 by '&'.
  ----------------------------------------------------------------------*/
void TranslateChars (CHAR_T *text)
{
  int                 i;

  if (text == NULL)
    return;
  i = 0;
  while (text[i] != 0)
    {
      switch (text[i])
	{
	case BREAK_LINE:
	  if (!ShowSpace)
	    text[i] = SHOWN_BREAK_LINE;
	  break;
	case THIN_SPACE:
	  if (!ShowSpace)
	    text[i] = SHOWN_THIN_SPACE;
	  break;
	case FOUR_PER_EM:
	  if (!ShowSpace)
	    text[i] = SHOWN_HALF_EM;
	  break;
	case UNBREAKABLE_SPACE:
	  if (!ShowSpace)
	    text[i] = SHOWN_UNBREAKABLE_SPACE;
	  break;
	case SPACE:
	  if (!ShowSpace)
	    text[i] = SHOWN_SPACE;
	  break;
	case START_ENTITY:
	  text[i] = '&';
	}
      i++;
    }
}


/*----------------------------------------------------------------------
  GL_DrawUnicodeChar : draw a character in a texture or a bitmap 
  ----------------------------------------------------------------------*/
void GL_DrawUnicodeChar (CHAR_T const c, float x, float y, 
			 void *GL_font, int fg)
{
  CHAR_T symbols[2];
  
  symbols[0] = c;
  symbols[1] = '\0';
  
  if (fg < 0 || GL_font == NULL)
    return;
  GL_SetForeground (fg); 
  
  UnicodeFontRender (GL_font,
		     symbols, 
		     x,
		     y, 
		     1);
}

#ifndef _WIN_PRINT
/*----------------------------------------------------------------------
  WDrawString draw a char string of lg chars beginning in buff.
  Drawing starts at (x, y) in frame and using font.
  boxWidth gives the width of the final box or zero,
  this is used only by the thot formmating engine.
  bl indicates that there are one or more spaces before the string
  hyphen indicates whether an hyphen char has to be added.
  startABlock is 1 if the text is at a paragraph beginning
  (no justification of first spaces).
  parameter fg indicates the drawing color
  Returns the lenght of the string drawn.
  ----------------------------------------------------------------------*/
int WDrawString (wchar_t *buff, int lg, int frame, int x, int y,
		 PtrFont font, int boxWidth, int bl, int hyphen,
		 int startABlock, int fg, int shadow)
{
  int j;

  if (lg < 0)
    return 0;
  
  y += FrameTable[frame].FrTopMargin;
  if (shadow)
    {
      /* replace each character by a star */
      j = 0;
      while (j < lg)
	{
	  buff[j++] = '*';
	}
    }
  return (GL_UnicodeDrawString (fg, 
				buff, 
				(float) x,
				(float) y, 
				hyphen,
				(void *)font, 
				lg));
}
#endif /*_WIN_PRINT*/
/*----------------------------------------------------------------------
  GL_DrawString : Draw a string in a texture or a bitmap 
  ----------------------------------------------------------------------*/
int GL_UnicodeDrawString (int fg, 
			  CHAR_T *str, 
			  float x, float y, 
			  int hyphen,
			  void *GL_font, 
			  int end)
{
  int width;

  if (end <= 0 || fg < 0 || GL_font == NULL)
    return 0;
  str[end] = EOS;
  TranslateChars (str);
  
  GL_SetForeground (fg); 
 	
  width = UnicodeFontRender (GL_font, str, 
			     x, y, end);
  if (hyphen)
    /* draw the hyphen */
    GL_DrawUnicodeChar ('\255', 
			x + width, y, 
			GL_font, fg);
  return width;
}


#define REALY(A) (A + FrameTable[frame].FrTopMargin)

/*----------------------------------------------------------------------
  CharacterWidth returns the width of a char in a given font.
  ----------------------------------------------------------------------*/
int CharacterWidth (int c, PtrFont font)
{
  int                 l;
  
  
  if (c == INVISIBLE_CHAR)
    return 1;

  if (c == START_ENTITY)
    c = '&';
  else if (c == TAB || c == UNBREAKABLE_SPACE)
    /* we use the SPACE width for the character TAB */
    c = SPACE;

  if (c == NEW_LINE || c == BREAK_LINE)
    /* characters NEW_LINE and BREAK_LINE are equivalent */
    l = 1;
  else
    {
      if (font == NULL)
	return 1;
      /* Thin space and Half em in gtk and win =>
	 w(32)/4 and w(32)/2 in Motif w(32)
      */
      if (c == THIN_SPACE)
	l = gl_font_char_width ((void *) font, 32) / 4;
      else if (c == FOUR_PER_EM)
	l = gl_font_char_width ((void *) font, 32) / 2;
      else
	l = gl_font_char_width ((void *) font, (CHAR_T) c);

      /* the Max*/
      if (l == 0)
	l = gl_font_char_width ((void *) font, 32);
    }
  return l;
}

ThotBool GetBoxTransformed (void *v_trans, int *x, int *y)
{
  PtrTransform Trans = (PtrTransform) v_trans;

  while (Trans)
    {
      switch (Trans->TransType)
	{
	case  PtElBoxTranslate:
	  *x = Trans->XScale;
	  *y = Trans->YScale;
          return TRUE;
	default:
	  break;	  
	}
      Trans = Trans->Next;
    }
  *x = 0;
  *y = 0;
  return FALSE;
}
ThotBool IsTransformed (void *v_trans)
{
  PtrTransform Trans = (PtrTransform) v_trans;

  while (Trans)
    {
      switch (Trans->TransType)
	{
	case  PtElBoxTranslate:
          /*result = FALSE;*/
	  break;
	default:
	  return TRUE;
	  break;	  
	}
      Trans = Trans->Next;
    }
  return FALSE;
}

#ifdef _GL
/*---------------------------------------------------
  DisplayBoxTransformation :
  ----------------------------------------------------*/
void DisplayBoxTransformation (void *v_trans, int x, int y)
{
  PtrTransform Trans = (PtrTransform) v_trans;

  glPushMatrix ();
  while (Trans)
    {
      switch (Trans->TransType)
	{
	case  PtElBoxTranslate:
	  glTranslatef (Trans->XScale - ((float) x), 
			Trans->YScale - ((float) y), 
			0);
	  return;
	default:
	  break;	  
	}
      Trans = Trans->Next;
    }
  glTranslatef (- ((float) x), - ((float) y), 0);
}
/*---------------------------------------------------
  DisplayViewBoxTransformation
  ----------------------------------------------------*/
static void DisplayViewBoxTransformation (PtrTransform Trans, int Width, int Height)
{
  float x_trans, y_trans;
  double  w_scale, h_scale; 
  ThotBool is_translated, is_scaled;

  x_trans = y_trans = w_scale = h_scale = 0;
  is_translated = is_scaled = FALSE;
  while (Trans)
    {
      switch (Trans->TransType)
	{
	case  PtElviewboxScale:
	  w_scale = (double) (Width / Trans->XScale); 
	  h_scale = (double) (Height / Trans->YScale);
	  is_scaled = TRUE;
	  break;
	case PtElviewboxTranslate:
	  x_trans = Trans->XScale; 
	  y_trans = Trans->YScale;
	  is_translated = TRUE;
	  break;
	default:
	  break;	  
	}
      Trans = Trans->Next;
    }
  if (is_scaled)
    glScaled (w_scale, h_scale, (double) 1.0f);
  if (is_translated)
    glTranslatef (-x_trans, -y_trans, (float) 0.0f);
}
#endif /* _GL */
/*---------------------------------------------------
  DisplayTransformation :
  Modify the current transformation matrix
  this is a GL Matrix         this is SVG Matrix
  |a0  a4  a8   a12|        | a d f |
  |		    |        | b c d |
  |a1  a5  a9   a13|        | 0 0 1 |
  |		    |
  |a2  a6  a10  a14|
  |		    |
  |a3  a7  a11  a15|
  ----------------------------------------------------*/
void DisplayTransformation (PtrTransform Trans, int Width, int Height)
{
#ifdef _GL
  double trans_matrix[16];
    
  if (IsTransformed (Trans))
    {
      glPushMatrix ();
      DisplayViewBoxTransformation (Trans, Width, Height);
      while (Trans)
	{
	  switch (Trans->TransType)
	    {
	    case  PtElScale:
	      glScalef (Trans->XScale, 
			Trans->YScale, 
			1.0);
	      break;
	    case PtElTranslate:
	      glTranslatef (Trans->XScale, 
			    Trans->YScale, 
			    0);
	      break;
	    case PtElRotate:
	      glTranslatef (Trans->XRotate, 
			    Trans->YRotate, 
			    0);
	      glRotatef (Trans->Angle, 0, 0, 1);
	      glTranslatef (-Trans->XRotate, 
			    -Trans->YRotate, 
			    0);
	      break;
	    case PtElMatrix:
	      /* Matrix 
		 GlMatrix is 4*4
		 Svg is 3*3 but 
		 only 2*3 is specified */
	      trans_matrix[0] = Trans->AMatrix;
	      trans_matrix[1] = Trans->BMatrix;
	      trans_matrix[2] = 0;
	      trans_matrix[3] = 0;

	      trans_matrix[4] = Trans->CMatrix;
	      trans_matrix[5] = Trans->DMatrix;
	      trans_matrix[6] = 0;
	      trans_matrix[7] = 0;

	      trans_matrix[8] = 0;
	      trans_matrix[9] = 0;
	      trans_matrix[10] = 1;
	      trans_matrix[11] = 0;

	      trans_matrix[12] = Trans->EMatrix;
	      trans_matrix[13] = Trans->FMatrix;	  
	      trans_matrix[14] = 0;
	      trans_matrix[15] = 1;

	      glMultMatrixd (trans_matrix);
	      break;
	    case PtElSkewX:
	      /* SkewX */
	      trans_matrix[0] = 1;
	      trans_matrix[1] = 0;
	      trans_matrix[2] = 0;
	      trans_matrix[3] = 0;

	      trans_matrix[4] = DTAN (DEG_TO_RAD(Trans->Factor));
	      trans_matrix[5] = 1;
	      trans_matrix[6] = 0;
	      trans_matrix[7] = 0;

	      trans_matrix[8] = 0;
	      trans_matrix[9] = 0;
	      trans_matrix[10] = 1;
	      trans_matrix[11] = 0;

	      trans_matrix[12] = 0;
	      trans_matrix[13] = 0;
	      trans_matrix[14] = 0;
	      trans_matrix[15] = 1;

	      glMultMatrixd (trans_matrix);
	      break;
	    case PtElSkewY:
	      /* SkewY */
	      trans_matrix[0] = 1;
	      trans_matrix[1] = DTAN (DEG_TO_RAD(Trans->Factor));
	      trans_matrix[2] = 0;
	      trans_matrix[3] = 0;

	      trans_matrix[4] = 0;
	      trans_matrix[5] = 1;
	      trans_matrix[6] = 0;
	      trans_matrix[7] = 0;

	      trans_matrix[8] = 0;
	      trans_matrix[9] = 0;
	      trans_matrix[10] = 1;
	      trans_matrix[11] = 0;

	      trans_matrix[12] = 0;
	      trans_matrix[13] = 0;
	      trans_matrix[14] = 0;
	      trans_matrix[15] = 1;

	      glMultMatrixd (trans_matrix);
	      break;	  
	    default:
	      break;
	    }
	  Trans = Trans->Next;
	}
    }
#endif /* _GL */
}
/*---------------------------------------------------
  DisplayTransformationExit :
  ----------------------------------------------------*/
void DisplayTransformationExit ()
{
#ifdef _GL
  glPopMatrix ();
#endif /* _GL */
}
/*-------------------------------
 print2DVertex: Write contents of one vertex to stdout
-------------------------------*/
void print2DVertex (GLint size,
		    GLint *count, 
		    GLfloat *buffer)
{
  int i;
  
  printf ("  ");
  for (i = 0; i < 2; i++) 
    {
      printf ("%4.2f ", buffer[size-(*count)]);
      *count = *count - 1;
    }
  printf ("\t");
}
/*---------------------------------------
  printBuffer :
Write contents of entire buffer.  (Parse tokens!)	
Bounding box Debugging purpose
(and print...)
---------------------------------------------*/
void printBuffer (GLint size, GLfloat *buffer)
{
  GLint  token, count, vertex_count;

  count = size;
  printf ("\n------------------------------------------\n");
   
  while (count) 
    {
      token = (GLint) buffer[size-count]; 
      count--;
      /*
	GL_POLYGON_TOKEN
	GL_POINT_TOKEN
	GL_LINE_TOKEN
	*_RESET => stipple change
	*/
      switch (token)
	{
	   
	case GL_POINT_TOKEN:
	  {
	     
	    /*pour les points*/
	    printf ("GL_POINT_TOKEN\n");
	    print2DVertex (size, &count, buffer);
	    printf ("\n");
	  }
	  break;
       
	case  GL_LINE_TOKEN:
	  {
	    /*pour les lignes*/
	    printf ("GL_LINE_TOKEN\n");
	    print2DVertex (size, &count, buffer);
	    print2DVertex (size, &count, buffer);
	    printf ("\n");
	  }
	  break;	   
	   
	case GL_POLYGON_TOKEN:
	  {
	    /*pour les lignes*/
	    printf ("GL_POLYGON_TOKEN\n");
	    vertex_count = (GLint) buffer[size - count];
	    count--;	     
	    while (vertex_count--)
	      print2DVertex (size, &count, buffer);
	    printf ("\n");
	  }
	  break;

	default:
	  break;
	}
    }
}
/*------------------------------------------------------------
 computeisminmax : check if number is a new min or max
  ------------------------------------------------------------*/
static void computeisminmax (double number, double *min, double *max)
{
  if (*min < 0)
    *min = number;
  if (*max < 0)
    *min = number;
  if (number < *min)
    *min = number;
  else if (number > *max)
    *max = number;
}
/*------------------------------------------------------------
 getboundingbox : Get bound values of the box
  ------------------------------------------------------------*/
static void getboundingbox (GLint size, GLfloat *buffer, int frame,
			    int *xorig, int *yorig, 
			    int *worig, int *horig)
{
  GLint  token, count, vertex_count;
  double x,y,w,h, TotalHeight;
  
  x = (double) *xorig;
  y = (double) *yorig;
  w = (double) *xorig + *worig;
  h = (double) *yorig + *horig;  
  TotalHeight = (double) FrameTable[frame].FrHeight;  
  count = size;
  while (count) 
    {
      token = (GLint) buffer[size-count]; 
      count--;
      switch (token)
	{
	case GL_POINT_TOKEN:
	  {
	    computeisminmax (buffer[size-count], &x, &w);count--;
	    computeisminmax (TotalHeight - buffer[size-count], &y, &h);count--;
	  }
	  break;
	case  GL_LINE_TOKEN:
	  {
	    computeisminmax (buffer[size-count], &x, &w);count--;
	    computeisminmax (TotalHeight - buffer[size-count], &y, &h);count--;

	    computeisminmax (buffer[size-count], &x, &w);count--;
	    computeisminmax (TotalHeight - buffer[size-count], &y, &h);count--;
	  }
	  break;
	case GL_POLYGON_TOKEN:
	  {
	    vertex_count = (GLint) buffer[size - count];
	    count--;	     
	    while (vertex_count--)
	      {
		computeisminmax (buffer[size-count], &x, &w);count--;
		computeisminmax (TotalHeight - buffer[size-count], &y, &h);count--;
	      }
	  }
	  break;
	default:
	  break;
	}
    }
  *xorig = (int) x;
  *yorig = (int) y;
  *worig = (int) (w - x) + 1;
  *horig = (int) (h - y) + 1;

  /* if (*xorig > 0) */
/*     *xorig += 1; */
/*   if (*yorig > 0) */
/*     *yorig += 1; */
}
/*---------------------------------------------------
  GL_NotInFeedbackMode : if all openGL operation are
  permitted or not.		    
  ----------------------------------------------------*/
ThotBool GL_NotInFeedbackMode ()
{
  return NotFeedBackMode;
}
/*---------------------------------------------------
  ComputeBoundingBox :
  Modify Bounding Box according to opengl feedback mechanism
  (after transformation, coordinates may have changed)			    
  ----------------------------------------------------*/
void ComputeBoundingBox (PtrBox box, int frame, int xmin, int xmax, int ymin, int ymax)
{
  GLfloat feedBuffer[4096];
  GLint   size;
  
  glFeedbackBuffer (4096, GL_2D, feedBuffer);
  NotFeedBackMode = FALSE;  
  glRenderMode (GL_FEEDBACK);
  DisplayBox (box, frame, xmin, xmax, ymin, ymax);
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
      /* printBuffer (size, feedBuffer); */
      box->BxBoundinBoxComputed = TRUE; 
    }
}

/*---------------------------------------------------
  ComputeBoundingBox :
  Modify Bounding Box according to opengl feedback mechanism
  (after transformation, coordinates may have changed)			    
  ----------------------------------------------------*/
void ComputeFilledBox (PtrBox box, int frame, int xmin, int xmax, int ymin, int ymax)
{
  GLfloat feedBuffer[4096];
  GLint size;
  
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
/*----------------------------------------------------------------------
  GL_Swap : swap frontbuffer with backbuffer (display changes)
  ----------------------------------------------------------------------*/
void GL_Swap (int frame)
{
  if (frame < MAX_FRAME)
    {
      /* gl_synchronize ();  */
      glFinish ();
      /* glFlush (); */      
      glDisable (GL_SCISSOR_TEST);
#ifdef _WINDOWS
      if (GL_Windows[frame])
	SwapBuffers (GL_Windows[frame]);
#else
      if (FrameTable[frame].WdFrame)
	{
	  gtk_gl_area_swapbuffers (GTK_GL_AREA(FrameTable[frame].WdFrame));
	}
#endif /*_WINDOWS*/
      glEnable (GL_SCISSOR_TEST); 
    }
}

/*----------------------------------------------------------------------
  GL_prepare: If a modif has been done
  ----------------------------------------------------------------------*/
ThotBool GL_prepare (int frame)
{  

  if (frame < MAX_FRAME && NotFeedBackMode)
    {
#ifdef _TESTSWAP
      FrameTable[frame].DblBuffNeedSwap = TRUE;
#endif /*_TESTSWAP*/
      if (FrRef[frame])
#ifdef _WINDOWS
	if (GL_Windows[frame])
	  if (wglMakeCurrent (GL_Windows[frame], 
			      GL_Context[frame]))
	    return TRUE;		
#else /*_WINDOWS*/
      if (FrameTable[frame].WdFrame)
	if (gtk_gl_area_make_current (GTK_GL_AREA(FrameTable[frame].WdFrame)))
	  return TRUE;
#endif /*_WINDOWS*/
    }
  return FALSE;
}

/*----------------------------------------------------------------------
  GL_realize : can we cancel if no modifs ?
  ----------------------------------------------------------------------*/
void GL_realize (int frame)
{
#ifdef _TESTSWAP
  GL_Swap (frame);
  FrameTable[frame].DblBuffNeedSwap = FALSE;
#else /*_TESTSWAP*/
  FrameTable[frame].DblBuffNeedSwap = TRUE;
#endif /*_TESTSWAP*/
  return;
}
/*----------------------------------------------------------------------
  GL_ActivateDrawing : Force Recalculation of the frame and redisplay
  ----------------------------------------------------------------------*/
void GL_ActivateDrawing(int frame)
{
  if (frame < MAX_FRAME)
    FrameTable[frame].DblBuffNeedSwap = TRUE;
}


/*----------------------------------------------------------------------
  TtaChangePlay : Activate Animation
  ----------------------------------------------------------------------*/
static void TtaChangePlay (int frame)
{
  ThotBool remove;

  if (frame && frame <= MAX_FRAME)
    if (FrameTable[frame].Animated_Boxes)
      {
	FrameTable[frame].Anim_play = (FrameTable[frame].Anim_play ? FALSE : TRUE);
	if (FrameTable[frame].Anim_play)
	  {
	    if (AnimTimer == 0)
#ifdef _GTK
	      AnimTimer = gtk_timeout_add (FRAME_TIME,
				       (gpointer) GL_DrawAll, 
				       (gpointer)   NULL); 
	   	      
#else /*_GTK*/
#ifdef _WINDOWS
	    {
	     /* SetTimer(FrMainRef[frame],                
		       frame,               
		       FRAME_TIME,                     
		       (TIMERPROC) MyTimerProc); 

	      AnimTimer = frame;*/
          AnimTimer = SetTimer(NULL,                
		       frame,               
		       FRAME_TIME,                     
		       (TIMERPROC) MyTimerProc);
	    }    
#endif /*_WINDOWS*/
#endif /*_GTK*/
	    FrameTable[frame].BeginTime = 0;
	    FrameTable[frame].LastTime = 0;
	  }
	else
	  if (AnimTimer)
	    {
	      remove = FALSE;
	      for (frame = 0; frame < MAX_FRAME; frame++)
		  {
		     if (FrameTable[frame].Anim_play)
		        remove = TRUE;
		  }
	      if (remove)
		{

#ifdef _GTK
		  gtk_timeout_remove (AnimTimer); 	
#else /*_GTK*/
#ifdef _WINDOWS
		  /*KillTimer (FrMainRef[AnimTimer], AnimTimer);*/
		  KillTimer (NULL, AnimTimer);
#endif /*_WINDOWS*/
#endif /*_GTK*/
		  AnimTimer = 0; 
		}
	    }
      }  
}
/*----------------------------------------------------------------------
  TtaPlay : Activate/Deactivate Animation (restart)
  ----------------------------------------------------------------------*/
void TtaPlay (Document doc, View view)
{
  int frame;

  frame = GetWindowNumber (doc, view);
  TtaChangePlay (frame);
  FrameTable[frame].BeginTime = 0;
  FrameTable[frame].LastTime = 0;
}

/*----------------------------------------------------------------------
  TtaNoPlay : Make sure we deactivate/reset all Animation of this frame
  And reset Time (Stop)
  ----------------------------------------------------------------------*/
void TtaNoPlay (int frame)
{
  if (frame && frame < MAX_FRAME)
    if (FrameTable[frame].Anim_play)
      {
	TtaChangePlay (frame);
	FrameTable[frame].BeginTime = 0;
	FrameTable[frame].LastTime = 0;
      }
}

/*----------------------------------------------------------------------
  TtaPause : Make sure we pause all Animation of this frame
  ----------------------------------------------------------------------*/
void TtaPause (int frame)
{

  if (frame && frame < MAX_FRAME)
    if (FrameTable[frame].Anim_play)
      {
	TtaChangePlay (frame);
	FrameTable[frame].BeginTime = 0;
	FrameTable[frame].LastTime = 0;
      }
}
/*----------------------------------------------------------------------
 SetCurrentTime : Position current time
  ----------------------------------------------------------------------*/
void SetAmayaCurrentTime (AnimTime current_time, int frame)
{
  FrameTable[frame].LastTime = current_time;
}
/*----------------------------------------------------------------------
  GetCurrentTime : Get Current Time
  ----------------------------------------------------------------------*/
static AnimTime ComputeAmayaCurrentTime (int frame)
{
#ifdef _GTK
  /* draw and calculate draw time 
     bench that helps finding bottlenecks...*/
  struct timeb	after;
#endif /*_GTK*/
  AnimTime current_time; 

  if (FrameTable[frame].Anim_play) 
    {   
#ifdef _GTK
      /* while (gtk_events_pending ()) */
      /*   gtk_main_iteration (); */
      ftime (&after);
      current_time = after.time + (((double)after.millitm)/1000);      
#else /* _GTK */
#ifdef _WINDOWS
      current_time = ((double) GetTickCount ()) / 1000; 
#endif /*_WINDOWS*/	
#endif /*_GTK*/
      if (FrameTable[frame].BeginTime == 0)
	{
	  FrameTable[frame].BeginTime = current_time;
	  current_time += 0.001;
	  FrameTable[frame].LastTime = 0.0;
	}
      current_time -= FrameTable[frame].BeginTime; 
      if (current_time - FrameTable[frame].LastTime < INTERVAL)
	 current_time = -1;
    }
  else
    current_time = FrameTable[frame].LastTime; 
  return current_time;
}
/*----------------------------------------------------------------------
  GL_DrawAll : Really Draws opengl !!
  ----------------------------------------------------------------------*/
ThotBool GL_DrawAll ()
{  
  int             frame;
  AnimTime        current_time; 
  static ThotBool frame_animating = FALSE;  
#ifdef _FPS_DEBUG
  static double   lastime;
  char    out[2048];
  CHAR_T  outw[2048];
  int i;
#endif /* _FPS_DEBUG */

  if (!FrameUpdating )
    {
      FrameUpdating = TRUE;     
      if (!frame_animating)
	{	
	 while (gtk_events_pending ())
	   gtk_main_iteration ();
	  
	  frame_animating = TRUE; 
	  for (frame = 0 ; frame < MAX_FRAME; frame++)
	    {
	      if (FrRef[frame] != 0)
		{
#ifdef _GL
		  if (FrameTable[frame].Animated_Boxes &&
		      FrameTable[frame].Anim_play)
		    {	
		      current_time = ComputeAmayaCurrentTime (frame);  
		      if (current_time != -1)
			{
			  if (Animate_boxes (frame, current_time))
			    TtaPause (frame);
			  FrameTable[frame].LastTime = current_time;
			}
		      else
			current_time = FrameTable[frame].LastTime;
		    }
#endif /* _GL */		    
		  if (FrameTable[frame].DblBuffNeedSwap)
		    {
		      if (documentDisplayMode[FrameTable[frame].FrDoc - 1] 
			  != NoComputedDisplay)
			{
			  if (GL_prepare (frame))
			    {	
#ifdef _FPS_DEBUG
			      lastime = current_time - lastime;
			      if (lastime != 0)
				{
				  sprintf (out, " t: %2.3f <=> %2.0f fps", 
					   current_time, 
					   (double) 1 / lastime);

				  i = 0;
				  while (i < strlen(out))
				    {
				      outw[i] = TtaGetWCFromChar (out[i], 
								  ISO_8859_1);
				      i++;
				    }
				}
			      DefRegion (frame, 0, 0, 10, 10);		      			      			      
#endif /* _FPS_DEBUG */
			      
			      RedrawFrameBottom (frame, 0, NULL); 
#ifdef _FPS_DEBUG
			      if (lastime != 0)
				{
				  GL_SetFillOpacity (500);
				  GL_SetPicForeground ();
				  UnicodeFontRender (GetFirstFont (12), 
						     outw, 
						     10.0f, 10.0f,
						     strlen (out));
				  GL_SetFillOpacity (1000);
				  lastime = current_time;
				}
#endif /* _FPS_DEBUG */

			      GL_Swap (frame);  
			      /* All transformation are resetted */   
			      glLoadIdentity (); 
			      FrameTable[frame].DblBuffNeedSwap = FALSE;
			    }
			  GL_Err ();
			}
		    }
		}
	    }
	  frame_animating  = FALSE;      
	}  
      FrameUpdating = FALSE;     
    }
  return TRUE;  
}

#define GLU_ERROR_MSG "\nSorry, Amaya requires GLU 1.2 or later.\n"
/*----------------------------------------------------------------------
  SetGlPipelineState : Detect Opengl, Software, Hardware, 
  Set The Openlg State machine to the fastest state possible for drawing 2d.
  ----------------------------------------------------------------------*/
void SetGlPipelineState ()
{  
  const char *version = (const char *) gluGetString (GLU_VERSION);
  const char *renderer = glGetString (GL_RENDERER);
  
  Software_Mode = FALSE;
  if (strstr (renderer, "Mesa")
      || strstr (renderer, "Microsoft")
      || strstr (renderer, "Sgi"))
    if (!strstr (renderer, "Mesa DRI"))
      Software_Mode = TRUE;  
  if (strstr (version, "1.0") || strstr (version, "1.1")) 
    {
#ifdef _WINDOWS
      WinErrorBox (NULL,  GLU_ERROR_MSG);
#else /*  _WINDOWS */
      fprintf( stderr, GLU_ERROR_MSG);
#endif /* _WINDOWS */
      exit (1);
    }
#ifdef _PCLDEBUG
  g_print ("\n%s", (Software_Mode)?
	   "Soft":"Hard");
  /* Display Opengl Vendor Name,  Opengl Version, Opengl Renderer*/
  g_print ("\nVENDOR : %s\nVERSION : %s\nRENDERER : %s", 
	   (char *)glGetString(GL_VENDOR), 
	   (char *)glGetString(GL_VERSION), 
	   (char *)glGetString(GL_RENDERER));
  /* g_print( "%s\n", (char *)glGetString(GL_EXTENSIONS));  */
  g_print ("\nGLU Version : %s", 
	   (char *)gluGetString (GLU_VERSION));
#endif /*_PCLDEBUG*/
  /*glClearColor (1, 0, 0, 0);*/
  /* no fog*/
  glDisable (GL_FOG);
  /*No Dithering*/
  glDisable (GL_DITHER);
  /* No lights */
  glDisable (GL_LIGHTING);
  glDisable (GL_LIGHT0);
  glDisable (GL_AUTO_NORMAL);
  glDisable (GL_NORMALIZE);
  glDisable (GL_COLOR_MATERIAL);
  /* No z axis (SVG is 2d)  */
  glDisable (GL_DEPTH_TEST);
  glDepthMask (FALSE);
  glDisable (GL_STENCIL_TEST);
  /* At the beginning, 
     there was no clipping*/
  glDisable (GL_SCISSOR_TEST);
  /* Modulated Transparency*/
  glDisable (GL_ALPHA_TEST); 	 
  /* Polygon are alway filled (until now)
     Because Thot draws outlined polygons with lines
     so...  if blending svg => GL_FRONT_AND_BACK*/
  glPolygonMode (GL_FRONT, GL_FILL);
  
  /*  Antialiasing 
      Those Options give better 
      quality image upon performance loss
      Must be a user Option  */
  glEnable (GL_LINE_SMOOTH); 
  glHint (GL_LINE_SMOOTH_HINT,  
 	  GL_NICEST);  

  glEnable (GL_POINT_SMOOTH); 
  glHint (GL_POINT_SMOOTH_HINT, 
	  GL_NICEST);
     
  /* For transparency and beautiful antialiasing*/
  glEnable (GL_BLEND); 

  glBlendFunc (GL_SRC_ALPHA, 
	       GL_ONE_MINUS_SRC_ALPHA); 

  /* Fastest Texture Mapping*/
  glHint (GL_PERSPECTIVE_CORRECTION_HINT, 
	  GL_NICEST );    
  /* Bitmap font Text writing (even in texture font)*/
  glPixelStorei (GL_UNPACK_ALIGNMENT, 1); 
  /* Needs to clear buffer after allocating it before drawing*/

  glDisable (GL_SCISSOR_TEST);
  glClear (GL_COLOR_BUFFER_BIT);
  glEnable (GL_SCISSOR_TEST);  
  glShadeModel (GL_FLAT);

  /* Not recommended for hardware cards... 
     Global Antialiasing is done elsewhere...*/
  /*  glEnable (GL_POLYGON_SMOOTH);   */
  /* smooth polygon antialiasing */
  /* glBlendFunc (GL_SRC_ALPHA_SATURATE, GL_ONE); */
  /* glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST); */
  
  
  /* Doesn't compute hidden drawing 
     Doesn't work for our tesselated
     polygons   not CGW oriented...
     50 % performance on geometry drawing...*/
  /* glEnable (GL_CULL_FACE); */
  /* glCullFace (GL_FRONT_AND_BACK.,GL_BACK, GL_FRONT); */
  
  GL_SetOpacity (1000);
  if (GL_Err())
#ifdef _GTK
    g_print ("Bad INIT\n"); 
#else  /*_GTK*/
  WinErrorBox (NULL, "Bad INIT\n");
#endif  /*_GTK*/
}


/*---------------------------------------
  BackBufferRegionSwapping
  We copy region content of the back buffer 
  on the exposed region 
  => opengl region buffer swapping 
  --------------------------------------------*/
void GL_BackBufferRegionSwapping (int x, int y,
				  int width, int height, 
				  int Totalheight)
{  
#ifndef _WINDOWS
  /* copy form bottom to top
     so we must add height and 
     invert y */
  y = y + height;
  glRasterPos2i (x, y);
  glDrawBuffer (GL_FRONT); 
  y =  Totalheight - y;
  glCopyPixels (x, y, width, height, GL_COLOR);  
  glDrawBuffer (GL_BACK);
  glFlush ();
#else /* _WINDOWS*/
  static PFNGLADDSWAPHINTRECTWINPROC p = 0;
	  
  if (p == 0)
    p = (PFNGLADDSWAPHINTRECTWINPROC) wglGetProcAddress("glAddSwapHintRectWIN");

  (*p) (x, y, x+width, y+height);
  SwapBuffers (GL_Windows[ActiveFrame]);
#endif /*_WINDOWS*/
}



/*---------------------------------------
  GL_window_copy_area : 
  Soft : We copy region content of the back buffer 
  on the exposed region 
  (=> opengl region buffer swapping )
  --------------------------------------------*/
void GL_window_copy_area (int frame, 
			  int xf, 
			  int yf, 
			  int x_source, /*source x*/
			  int y_source, /*source y*/
			  int width, 
			  int height)
{
  if (GL_prepare (frame) == FALSE)
    return;  
  /* Horizontal Scroll problems...*/
  if (xf < 0)
    {
      width -= xf;
      xf = 0;
    }
  if (x_source < 0)
    {
      width -= x_source;
      x_source = 0;	
    }
  if (x_source + width > 
      FrameTable[frame].FrWidth) 
    width -= (x_source + width) 
      - FrameTable[frame].FrWidth;

  if (width >= FrameTable[frame].FrWidth)
    width = FrameTable[frame].FrWidth;

  /*if (xf >= FrameTable[frame].FrWidth)
    xf = FrameTable[frame].FrWidth - 1;*/


  /* Vertical Scroll problems...*/
  if (yf < 0)
    {
      height -= yf;
      yf = 0;
    }
  if (y_source < 0)
    {
      height -= y_source;
      y_source = 0;	
    }
  if (y_source + height > 
      FrameTable[frame].FrHeight) 
    height -= (y_source + height) 
      - FrameTable[frame].FrHeight;
 
      
  if (height > FrameTable[frame].FrHeight)
    height = FrameTable[frame].FrHeight;

  /*if ((yf + height) >= FrameTable[frame].FrHeight)
    height = FrameTable[frame].FrHeight - yf - 1;*/

  if (width > 0 && height  > 0)
    {	  
      y_source = (FrameTable[frame].FrHeight) -
	(y_source + height + 
	 FrameTable[frame].FrTopMargin);

      /*Hardware rendering faster than Reading pixel from buffer
	(here glcopypixels)*/
      if (glMatroxBUG (frame, xf, yf, width, height))
	{
	  /* FrameTable[frame].DblBuffNeedSwap = TRUE; */
	  return;
	}

      /* Copy from backbuffer to backbuffer */
      glFinish ();
      glDisable (GL_BLEND);

      GL_UnsetClipping  (0, 0, 0, 0);

      glRasterPos2i (xf, yf+height);	  
      /*IF Rasterpos is outside canvas...
	we must use a decaling 'feinte'*/
      if ((yf+height) == FrameTable[frame].FrHeight)
	{
	  glRasterPos2i (xf, yf+height-1);

	  glBitmap(0,
		   0,
		   0,
		   0,
		   0,
		   -1,
		   NULL);
	}
      glReadBuffer (GL_BACK);
      glCopyPixels (x_source,   
		    y_source,
		    width,
		    height,
		    GL_COLOR); 
      glEnable (GL_BLEND);
      /*copy from back to front */
      GL_realize (frame);	  
    }
}

/*-----------------------------------
  GLSynchronize : Make sure all 
    opengl calls are done
  ------------------------------------*/
void gl_synchronize ()
{
#ifdef _GTK
/* gtk_main_iteration_do (FALSE); */
/* 	while (gtk_events_pending ())  */
/* 	  gtk_main_iteration (); */

    gdk_gl_wait_gdk ();
    gdk_gl_wait_gl ();
#endif /* _GTK */
}

/*-----------------------------------
  GLResize : 
  remake the current coordonate system 
  upon resize
  ------------------------------------*/
void GLResize (int width, int height, int x, int y)
{
  /* gl_synchronize(); */
  glViewport (0, 0, width, height);
  glMatrixMode (GL_PROJECTION);      
  glLoadIdentity (); 
  /* Invert the opengl coordinate system
     to get the same as Thot	  
     (opengl Y origin  is the left up corner
     and the left bottom is negative !!)
  */
  glOrtho (0, width, height, 0, -1, 1); 
  /* Needed for 3d only...*/
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity (); 
  glDisable (GL_SCISSOR_TEST);
  glClear (GL_COLOR_BUFFER_BIT);
  glEnable (GL_SCISSOR_TEST);  
}

/*-----------------------------------
  gl_window_resize : Some video cards or software 
  implementations  mechanisms clears when resizing 
  viewport so we redraw all
  ------------------------------------*/
void gl_window_resize (int frame, int width, int height)
{
#ifdef _GTK
#ifdef _GTK
  GtkWidget *widget;

  widget = FrameTable[frame].WdFrame;

  gtk_widget_queue_resize  (widget->parent->parent);

  FrameTable[frame].DblBuffNeedSwap = TRUE;

/*   gdk_gl_wait_gdk ();  */
/*   gdk_gl_wait_gl ( ); */

  return;
#endif /*_GTK*/

  if (GL_prepare (frame))
    {
#ifdef _GTK
      DefClip (frame, -1, -1, -1, -1);
      return;
#endif /*_GTK*/
	DefRegion (frame, 
 		   0, 0,
 		   width, height);
	FrameRedraw (frame, width, height);
	GL_realize (frame);	 
      }
#endif /*_GTK*/
}
/*-----------------------------------
  glhard : if a 3d card is involved.
------------------------------------*/
ThotBool glhard()
{
  return (!Software_Mode);
}
/*-----------------------------------
  glMatroxBUG : expose without a drawing 
  make black swapbuffer...
------------------------------------*/
int glMatroxBUG (int frame, int x, int y, 
		 int width, int height)
{
  if (!Software_Mode)
    {
      DefRegion (frame, x, y, 
		 width+x, y+height);
      return 1;      
    }
  return 0;  
}
/*-------------------------------
 SaveBuffer :
 Take a picture (png) of the backbuffer.
--------------------------------*/
void saveBuffer (char *filename, int width, int height)
{
  unsigned char *Data;

  glFlush ();
  glFinish ();
  Data = TtaGetMemory (sizeof (unsigned char) * width * height * 4);
  glReadPixels (0, 0, width, height, 
		GL_RGBA, 
		GL_UNSIGNED_BYTE, Data);
  SavePng (filename, 
	   Data,
	   (unsigned int) width,
	   (unsigned int) height);
  free (Data);
}

#ifdef PICK
void processHits2 (GLint hits, GLuint buffer[], int sw)
{
   GLint i, j, numberOfNames;
   GLuint names, *ptr, minZ,*ptrNames;

   ptr = (GLuint *) buffer;
   printf ("\n\n");
   for (i = 0; i < hits; i++) {	/*  for each hit  */
      names = *ptr;
      printf (" Nombre d'objet selectionnés :  %d\n", names); 
      ptr++;// premier z
      ptr++;// deuxieme z
      ptr++;// positionné sur l'identifiant (enfin !)
      for (j = 0; j < names; j++) {
	switch (*ptr){
	 case SPLINE:
	  printf ("SPLINE Est dans la zone ! ");
	  break;
	 case CARRE:
	  printf ("CARRE Est dans la zone ! ");
	  break; 
	 case CERCLE:
	  printf ("CERCLE Est dans la zone ! ");
	  break;
	 case POLYGONE:
	  printf ("POLYGONE Est dans la zone ! ");
	  break;
	}
	ptr++;
      }
      printf ("\n");
   }
}

/*-----------------------------------------
  startPicking : Selection Mode Activation 
and then Object Clicked is return, and "normal" 
(projection mode) is setted back.
 -----------------------------------------*/
void PickObject (int frame, int x, int y) 
{
  GLint viewport[4];
  GLuint selectBuf[8144];

  glSelectBuffer (BUFSIZE, selectBuf);
  glGetIntegerv (GL_VIEWPORT, viewport);
  glRenderMode (GL_SELECT);

  /*For name's stack (integers)*/
  glInitNames ();

  /* glPushName(0); */

  /*create a clip around the mouse cursor
    and gets objects in it*/
  glMatrixMode (GL_PROJECTION);  
  glPushMatrix ();
  glLoadIdentity (); 
  gluPickMatrix ((GLdouble) x, 
		 (GLdouble) (viewport[3] - y), 
		 5, 5, 
		 viewport);
  glOrtho (0, FrameTable[frame].FrWidth, FrameTable[frame].FrHeight, 0, -1, 1); 
  /* glOrtho (0,  WIN_WIDTH, 0, WIN_HEIGHT, 0,  WIN_WIDTH); */
  glMatrixMode (GL_MODELVIEW); 
  glLoadIdentity ();


  DrawSelect ();


  glMatrixMode (GL_PROJECTION);
  glPopMatrix ();
  glFlush ();
  hits = glRenderMode (GL_RENDER);
  printf ("%i hits\n", hits);
  if (hits != 0)
    {
      processHits2 (hits, selectBuf, 0);
    }
  glMatrixMode (GL_MODELVIEW);
  mode = RENDER;
}
#endif/*  _PICK */



#ifdef _PCLDEBUG

/*****************************************************/
/* TESTING NOT REALLY USED (FOR DEBUGGING)*/

/* Drawing grid (for canvas geometry precision tests)*/
void DrawGrid(int width, int height)
{  
  GLfloat grid2x2[2][2][3];


  grid2x2[0][0][0] = 0.0;
  grid2x2[0][0][1] = 0.0;
  grid2x2[0][0][2] = 0.0;

  grid2x2[0][1][0] = width;
  grid2x2[0][1][1] = 0.0;
  grid2x2[0][1][2] = 0.0;


  grid2x2[1][0][0] = 0.0;
  grid2x2[1][0][1] = height;
  grid2x2[1][0][2] = 0.0;

  grid2x2[1][1][0] = width;
  grid2x2[1][1][1] = height;
  grid2x2[1][1][2] = 0.0;
  
  glColor3f(1.0, 0.0, 0.0);
  glEnable(GL_MAP2_VERTEX_3);  
  glMap2f(GL_MAP2_VERTEX_3,    
	  0.0, 1.0,  /* U ranges 0..1 */    
	  3,         /* U stride, 3 floats per coord */    
	  2,         /* U is 2nd order, ie. linear */    
	  0.0, 1.0,  /* V ranges 0..1 */    
	  2 * 3,     /* V stride, row is 2 coords, 3 floats per coord */    
	  2,         /* V is 2nd order, ie linear */    
	  (GLfloat *) grid2x2);  /* control points */ 
  glMapGrid2f(    5, 0.0, 1.0,    6, 0.0, 1.0);
  glEvalMesh2(GL_LINE,    0, 5,   
	      /* Starting at 0 mesh 5 steps (rows). */    
	      0, 6);  /* Starting at 0 mesh 6 steps (columns). */
}

/* stupid animation render testing
   in order to vizualise renderings 
   times */
int make_carre()
{
  static float k = 0.0;
  static float l = 0.0;

  /*(glScalef (1-l, 1-l, 1.0);*/
  glPushMatrix();
         k += 15.0;
         k = k > 500.0 ? 0 : k;
	 l += 0.05;
	 l = l > 1.0 ? 0 : l;
	 glTranslatef( 0.0, k, 0.0);
	 glBegin(GL_QUADS);
	 glColor4f(1.0-l, 0.0, 1.0, 1-l);
	 glVertex2i(  0, 20 );/* haut gauche*/
	 glColor4f(1.0, 0.0, 1.0-l, 0.75-l);
	 glVertex2i( 20, 20);/* haut droit*/
	 glColor4f(0.5, 0.5, 1.0-l, 0.75-l);
	 glVertex2i( 20, 0);/* bas droit*/
	 glColor4f(0.5, 0.5, 1.0-l, 0.25-l);
	 glVertex2i(  0, 0);/* bas gauche */
	 glEnd();
   glPopMatrix(); 
   return 0;
}

int  savetga (const char *filename, 
		 unsigned char *Data,
		 unsigned int width,
		 unsigned int height)
{
  FILE *screenFile;
  int length;

  unsigned char cGarbage = 0, type,mode,aux, pixelDepth;
  short int iGarbage = 0;
  int i;

  length = width * height * 4;
  screenFile = fopen("screenshot.tga", "w");
  pixelDepth = 16;
  /* compute image type: 2 for RGB(A), 3 for greyscale*/
  mode = pixelDepth / 8;
  if ((pixelDepth == 24) || (pixelDepth == 32))
    type = 2;
  else
    type = 3;
  /* convert the image data from RGB(a) to BGR(A)*/
  if (mode >= 3)
    for (i=0; i < width * height * mode ; i+= mode) {
      aux = Data[i];
      Data[i] = Data[i+2];
      Data[i+2] = aux;
      Data[i+3] = 255;
    }
  /* write the header*/
  fwrite(&cGarbage, sizeof(unsigned char), 1,screenFile);
  fwrite(&cGarbage, sizeof(unsigned char), 1, screenFile);  
  fwrite(&type, sizeof(unsigned char), 1, screenFile);  
  fwrite(&iGarbage, sizeof(short int), 1, screenFile);
  fwrite(&iGarbage, sizeof(short int), 1, screenFile);
  fwrite(&cGarbage, sizeof(unsigned char), 1, screenFile);
  fwrite(&iGarbage, sizeof(short int), 1, screenFile);
  fwrite(&iGarbage, sizeof(short int), 1, screenFile);  
  fwrite(&width, sizeof(short int), 1, screenFile);
  fwrite(&height, sizeof(short int), 1, screenFile);
  fwrite(&pixelDepth, sizeof(unsigned char), 1, screenFile);  
  fwrite(&cGarbage, sizeof(unsigned char), 1, screenFile);  
  fwrite(Data, sizeof(unsigned char), length, screenFile);
  fclose(screenFile);
  return 0;
}
#ifdef NOtENOUGHTIMEPRECIsION
static wintime()
{
  /*if get tick count gets is more than 50 ms precise...*/
  {
    LARGE_INTEGER ticksPerSecond;
     				LARGE_INTEGER tick;   // A point in time
     				LARGE_INTEGER time;   // For converting tick into real time

      // get the high resolution counter's accuracy
      QueryPerformanceFrequency(&ticksPerSecond);
      // what time is it?
      QueryPerformanceCounter(&tick);
      // convert the tick number into the number of seconds
      // since the system was started...
      time.QuadPart = tick.QuadPart/ticksPerSecond.QuadPart;

      //get the number of hours
      int hours = time.QuadPart/3600;

      //get the number of minutes
      time.QuadPart = time.QuadPart - (hours * 3600);

      int minutes = time.QuadPart/60;

      //get the number of seconds
      int seconds = time.QuadPart - (minutes * 60);
  }
}
#endif /*NOtENOUGHTIMEPRECIsION*/
#endif /*_PCLDEBUG*/

#endif /* _GL */

