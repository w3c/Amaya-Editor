/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2002
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

#define ALLOC_POINTS    300

/*
 * Math Macros conversion from
 * degrees to radians and so on...
 * All for EllipticSplit and/or GL_DrawArc
 */
/*#include <math.h> */

#ifndef M_PI
#undef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifndef M_PI_DOUBLE
#undef M_PI_DOUBLE
#define M_PI_DOUBLE (6.2831853718027492)
#endif

/* ((A)*(M_PI/180.0)) */
#define DEG_TO_RAD(A)   ((double)A)/57.29577957795135
#define RAD_TO_DEG(A)   ((double)A)*57.29577957795135

/*If we should use a static table instead for
  performance bottleneck...*/
#define DCOS(A) ((double)cos (A))
#define DSIN(A) ((double)sin (A))
#define DACOS(A) ((double)acos (A))
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

#ifdef _GTK
#include <gtkgl/gtkglarea.h>
#ifdef _PCLDEBUGTIME
/* Unix timer */
#include <unistd.h>
#include <sys/timeb.h>
#endif /*_PCLDEBUG*/
/*#define GLU_CALLBACK_CAST (void (*)())*/
#else /*WINDOWS*/
#include <windows.h>
/*
#ifndef CALLBACK
#define CALLBACK
#endif
#define GLU_CALLBACK_CAST
*/
#endif /*_GTK*/

#include <GL/gl.h>
#include <GL/glu.h>

#ifndef CALLBACK
#define CALLBACK
#endif

#include "openglfont.h"
#include "glwindowdisplay.h"

static ThotBool Software_Mode;

/* Vertex list when tesselation is called 
   This list is filled with all new vertex 
   created by the tesselation 
   (all the list must stay in 
   memory until tesselation is finished)*/
typedef struct listmem {
  ThotPoint *data;
  struct listmem *next;
} ListMem;

static ListMem SAddedVertex;

ThotBool GL_Err() 
{
  GLenum errCode = GL_NO_ERROR;

  if((errCode = glGetError()) != GL_NO_ERROR)
    {
      /*g_print ("\n%s :", (char*) gluErrorString(errCode));*/
      return TRUE;
    }
  else 
    return FALSE;
}

/*-----------GLOBALS----------*/
/* Prevents double access */
ThotBool GL_Drawing = FALSE;

/*--------- STATICS ------*/

/*Current Thickness*/
static int S_thick;

/* background color*/
static int GL_Background[50];

#ifndef _WIN_PRINT
/*----------------------------------------------------------------------
  ClearAll clear the frame .
  ----------------------------------------------------------------------*/
void ClearAll (int frame)
{
  if (GL_prepare (frame))
    {
      glClear (GL_COLOR_BUFFER_BIT);
      GL_Swap (frame);
    }
}

/*----------------------------------------------------------------------
   SetMainWindowBackgroundColor :                          
  ----------------------------------------------------------------------*/
void SetMainWindowBackgroundColor (int frame, int color)
{
  unsigned short red, green, blue;
  
  if (GL_Background[frame] == color)
    return;
  else
    {
      TtaGiveThotRGB (color, &red, &green, &blue);
      glClearColor (red, green, blue, 255);
      GL_Background[frame] = color;
    }  
   return;
}

/*----------------------------------------------------------------------
  Clear clear the area of frame located at (x, y) and of size width x height.
  ----------------------------------------------------------------------*/
void Clear (int frame,
	    int width, int height,
	    int x, int y)
{
  int w,h;
  
  if (GL_prepare (frame))
    {
      /*(
      if (x == 0 && y == 0)
	{
	  GetSizesFrame (frame, &w, &h);
	  if (width == w && height == h)
	    ClearAll (frame);
	  return;	  
	  } 
      */     
      FrameTable[frame].DblBuffNeedSwap = TRUE; 
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

/* Win32 opengl context based on frame number*/
static HDC   GL_Windows[50];	
static HGLRC GL_Context[50];


int ChoosePixelFormatEx(HDC hdc)
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

void init_pfd ()
{
static PIXELFORMATDESCRIPTOR pfd = 
	{
        sizeof(PIXELFORMATDESCRIPTOR),  /* size */
        1,                              /* version */
        PFD_DRAW_TO_WINDOW |			/* Format Must Support Window*/
		PFD_SUPPORT_OPENGL |			/* Format Must Support OpenGL*/
		PFD_DOUBLEBUFFER,               /* support double-buffering */
        PFD_TYPE_RGBA,                  /* color type */
        16,                             /* prefered color depth */
        0, 0, 0, 0, 0, 0,               /* color bits (ignored) */
        0,                              /* no alpha buffer */
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
        0,                              /* no alpha buffer */
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
	
    /*int goodpixel;

     goodpixel = ChoosePixelFormatEx (hDC);
      DescribePixelFormat(hDC, goodpixel, sizeof(pfd), &pfd);*/

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
  else
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
#ifdef _SHARELIST
  if (GL_Context[1]) 
    wglShareLists (GL_Context[1], hGLRC);
#endif /*_SHARELIST*/
  if (wglMakeCurrent (hDC, hGLRC))
    {
      SetGlPipelineState ();
      if (!dialogfont_enabled)
	{
	  InitDialogueFonts ("");
	  dialogfont_enabled = TRUE;
	  for (frame_index = 0 ; frame_index <= MAX_FRAME; frame_index++)
	    {  
	      GL_Windows[frame_index] = 0;
	      GL_Context[frame_index] = 0;
	    }
	}
    }
  ActiveFrame = frame;
}

/*----------------------------------------------------------------------
 GL_Win32ContextInit : Free opengl contexts
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
}
#endif /*_WINDOWS*/

static int Opacity = 1000;

void GL_SetOpacity (int opacity)
{
  Opacity = (int) ((opacity * 255)/1000);
}


/*----------------------------------------------------------------------
 GL_SetForeground : set color before drawing a or many vertex
  ----------------------------------------------------------------------*/
void GL_SetForeground (int fg)
{
    unsigned short red, green, blue;

    TtaGiveThotRGB (fg, &red, &green, &blue);
    glColor4ub (red, green, blue, Opacity);
    
}
/*----------------------------------------------------------------------
  InitDrawing update the Graphic Context accordingly to parameters.
  The parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void InitDrawing (int style, int thick, int fg)
{
  char              dash[2];

  if (style >= 5)
    {
      /* solid */
       if (thick)
	{
	  S_thick = thick;
	  glLineWidth ((GLfloat) thick); 
	  glPointSize ((GLfloat) thick); 
	}
       else
	 {
	   glLineWidth ((GLfloat) 0.5); 
	   glPointSize ((GLfloat) 0.5); 
	 }
    }
  else
    {
      if (style == 3)
	/* dotted */
	dash[0] = 4;
      else
	/* dashed */
	dash[0] = 8;
      dash[1] = 4;
      if (thick)
	{
	  S_thick = thick;
	  glLineWidth ((GLfloat) thick); 
	  glPointSize ((GLfloat) thick); 
	}
      else
	{
	  glLineWidth ((GLfloat) 0.5); 
	  glPointSize ((GLfloat) 0.5); 
	}
     glEnable (GL_LINE_STIPPLE);
     glLineStipple (2, *((int *) (&dash[0]))); 
    }
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
  glVertex2f (x, y);
  glVertex2f (x + width, y);
  glVertex2f (x +  width, y + height);
  glVertex2f (x, y + height);
  glEnd (); 
}

/*----------------------------------------------------------------------
   GL_DrawEmptyRectangle Outlined rectangle
  ----------------------------------------------------------------------*/
void  GL_DrawEmptyRectangle (int fg, int x, int y, int width, int height)
{ 
  GL_SetForeground (fg);
  if (S_thick > 1)
    {
      glBegin (GL_POINTS);/*joining angles*/
      glVertex2f (  x, y );
      glVertex2f (  x, y + height);
      glVertex2f (  x +  width, y + height);
      glVertex2f (  x + width, y);
      glVertex2f (  x, y );
      glEnd ();
    }  
  glBegin (GL_LINE_LOOP);
  glVertex2f (  x, y );
  glVertex2f (  x, y + height);
  glVertex2f (  x +  width, y + height);
  glVertex2f (  x + width, y);
  glVertex2f (  x, y );
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
  glVertex2f (  x, y );
  glVertex2f (  x + width, y);
  glVertex2f (  x +  width, y + height);
  glVertex2f (  x, y + height);
  glEnd ();
}
/*----------------------------------------------------------------------
   GL_DrawLine
  ----------------------------------------------------------------------*/
void GL_DrawLine (int x1, int y1, int x2, int y2)
{
   if (S_thick > 1)
    {
      /* round line join*/ 
      glBegin (GL_POINTS);
      glVertex2f ((GLdouble) x1, 
		  (GLdouble) y1);
      glVertex2f ((GLdouble) x2, 
		  (GLdouble) y2);
      glEnd ();
    }
   glBegin (GL_LINES) ;
   glVertex2f ((GLdouble) x1, 
	       (GLdouble) y1);
   glVertex2f ((GLdouble) x2, 
		(GLdouble) y2);
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
  combineCallback :  used to create a new vertex when edges
  intersect.  coordinate location is trivial to calculate,
  but weight[4] may be used to average color, normal, or texture 
  coordinate data. 
  Here we add generated vertex to our list (so we can free it after tesselation)
  and give it to the tesselation engine. (dataout)
  ----------------------------------------------------------------------*/
static void CALLBACK myCombine (GLdouble coords[3], void *vertex_data[4], 
	  GLfloat weight[4], void **dataOut)
{
  ListMem *ptr = &SAddedVertex;
  
  while (ptr->next) 
    ptr = ptr->next;
  ptr->next = TtaGetMemory (sizeof (ListMem));
  ptr = ptr->next;
  ptr->next = 0;
  ptr->data = TtaGetMemory (sizeof (ThotPoint));
  ptr->data->x = (GLfloat) coords[0];
  ptr->data->y = (GLfloat) coords[1];
  *dataOut = ptr->data;
}
/*----------------------------------------------------------------------
 my_error : Displays GLU error 
 (VERY useful on 50000000000 vertex polygon... 
 see jasc webdraw butterfly sample )
  ----------------------------------------------------------------------*/
static void CALLBACK my_error (GLenum err)
{
#ifdef _GTK
	g_print ("%s \n", gluErrorString(err));
#else /*_GTK*/
	WinErrorBox (NULL, gluErrorString(err));;
#endif /*_GTK*/
}
/* To be malloc'ed !!!!!!!!!! 
   just don't when and how many... */
static int tab[1000];
/* Number of contour in the 
   polygon that must be tesselated
   also used in draw path 
   (ORDER and place of new countour 
   are the key of tesselation)*/ 
static int n_polygon;

/*----------------------------------------------------------------------
 tesse :  Tesselation that use GLU library tesselation 
   (triangulations based on Delaunay algorythms)
   Compute a polygon,
   handles 'holes' in the polygon,
   And create new point (Vertex) that 
   permits to display the polygon with simpler shape
   as it calls mycombine to create them
   (in this case triangles...)
  ----------------------------------------------------------------------*/
static void tesse(ThotPoint *contours, int contour_cnt, ThotBool only_countour)
{ 
  int i;
  GLdouble data[3];
  int n_poly_count = 0;
  GLUtesselator *tobj = NULL;

  tobj = gluNewTess();
  /* Winding possibilities are :
  GLU_TESS_WINDING_ODD = Classique
  GLU_TESS_WINDING_NONZERO
  GLU_TESS_WINDING_POSITIVE
  GLU_TESS_WINDING_NEGATIVE
  GLU_TESS_WINDING_ABS_GEQ_TWO */ 
  gluTessProperty (tobj,
		  GLU_TESS_WINDING_RULE, 
		  GLU_TESS_WINDING_ODD);
  gluTessProperty (tobj,
		  GLU_TESS_BOUNDARY_ONLY,
		  only_countour);
  gluTessProperty (tobj,
		  GLU_TESS_TOLERANCE, 
		   0);
  if (tobj != NULL) 
    {
      gluTessCallback (tobj, GLU_BEGIN, 
		     (void (CALLBACK*)()) glBegin);
      gluTessCallback (tobj, GLU_END, 
		      (void (CALLBACK*)()) glEnd);
      gluTessCallback (tobj, GLU_ERROR, 
		      (void (CALLBACK*)()) my_error); 
      gluTessCallback (tobj, GLU_VERTEX, 
		      (void (CALLBACK*)()) glVertex2fv);
      SAddedVertex.data = 0;
      SAddedVertex.next = 0;
      gluTessCallback (tobj, GLU_TESS_COMBINE, 
		      (void (CALLBACK*)()) myCombine);
      gluTessBeginPolygon( tobj, NULL );
      gluTessBeginContour (tobj);
      for (i = 0; i < contour_cnt; i++) 
	{ 
	  if (i == tab[n_poly_count] 
	      &&  n_poly_count < n_polygon)
	    {
	      n_poly_count++;
	      gluTessEndContour (tobj);
	      /* maybe calculation if not a new polygon...
	       if it's not inside this one,
	      but must calculate it for all the next polygon
	      */ 
	      glEdgeFlag(GL_TRUE);
	      gluTessBeginContour (tobj);
	    }
	  else
	    glEdgeFlag(GL_FALSE);
	  data[0] = (GLdouble) (contours[i].x);
	  data[1] = (GLdouble) (contours[i].y);
	  data[2] = 0.0;
	  gluTessVertex (tobj, data, (GLfloat *) &contours[i]);
	} 
      gluTessEndContour (tobj);
      gluTessEndPolygon (tobj);
      {
	ListMem *fptr = SAddedVertex.next;
	ListMem *tmp;
	while (fptr) 
	  {
	    tmp = fptr->next;
	    free(fptr->data);
	    free(fptr);
	    fptr = tmp;
	  }
      }
      gluDeleteTess(tobj);
    }
}
/*----------------------------------------------------------------------
 GL_DrawArc : receive angle at 64* their values...
 but
  ----------------------------------------------------------------------*/
void GL_DrawArc (int x, int y, 
		 int w, int h, 
		 int angle1, int angle2, 
		 ThotBool filled)
{  
  double angle, anglefinal, fastx, fasty;
  
  /*The formula is simple :
       y + (h/2)*(1 - sin (DEG_TO_RAD (Angle/64)))
       x + (w/2)*(1 + cos (DEG_TO_RAD (Angle/64)))
    But if we put all those calculation in the while
    Cpu will overheat with 5 *,  2 / and 2 +!!!
    That's why there is those preliminary steps */

  angle2 = angle1 + angle2;
  angle1 = angle1 / 64;
  angle2 = angle2 / 64;
  w =  w / 2;
  h =  h / 2;
  fastx = (double) (x + w);
  fasty = (double) (y + h);
  
  angle =  (double) DEG_TO_RAD (angle2);
  anglefinal = (double) DEG_TO_RAD (angle1);
  /* A good optimization is that
   cos(A)*cos(B)=(cos(A+B)+cos(A-B))/2 
   based on trigo decomposition
   sin(A+B)=sin A cos B + cos A sin B
   sin(A-B)=sin A cos B - cos A sin B
   cos(A+B)=cos A cos B - sin A sin B
   cos(A-B)=cos A cos B + sin A sin B
  it could eliminate MULs... 
  but need to calculate cos and sin more times
  perhaps precalculated tables...*/  
  if (!filled && 0)
    {
      angle =  (double) DEG_TO_RAD (angle2);
      glBegin (GL_POINTS);
      /* another one is to extend the use of Vertex array...
	 but not only for here... for all computations*/
      while (angle1 <= angle2)
	{
	  glVertex2d ( fastx + ((double)w) * DCOS(angle),
		       fasty - ((double)h) * DSIN(angle));
	  angle -= (double)A_DEGREE;
	  angle2--;
	}
      glVertex2d ( fastx + ((double)w) * DCOS (angle),
		   fasty - ((double)h) * DSIN (angle));
      glEnd();
    }
  angle = (double) DEG_TO_RAD (angle2);
  if (filled)
    {
      glBegin (GL_TRIANGLE_FAN);
      /* The center */
      glVertex2d (fastx, fasty);
    }
  else
    {
      glDisable (GL_BLEND);
      glBegin (GL_LINE_STRIP);
    }
  while (angle1 <= angle2)
    {
      glVertex2d (fastx + ((double)w) * DCOS(angle),
		  fasty - ((double)h) * DSIN(angle));
      angle -= (double)A_DEGREE;
      angle2--;
    }
  glVertex2d ( fastx + ((double)w) * DCOS (angle),
	       fasty - ((double)h) * DSIN (angle));
  glEnd();
  if (!filled)
    glEnable (GL_BLEND);
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
  for (i=0; i < npoints; i++)
    {
      if (i == tab[k] &&  k < n_polygon)
	{
	  glEnd ();
	  glBegin (GL_LINE_STRIP);
	  k++; 
	}
      
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
  if (0 && Software_Mode)
    {
      glEnable (GL_POLYGON_SMOOTH); 
      glHint (GL_POLYGON_SMOOTH_HINT, GL_NICEST);
      tesse (points, npoints, FALSE);
      glDisable (GL_POLYGON_SMOOTH); 
    }
  else
    tesse (points, npoints, FALSE);
}

void CountourCountReset ()
{
  n_polygon = 0;
}
void CountourCountAdd (int npoints)
{
  tab[n_polygon++] = npoints;
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
/*----------------------------------------------------------------------
   GL_Swap : swap frontbuffer with backbuffer (display changes)
  ----------------------------------------------------------------------*/
void GL_Swap (int frame)
{
  if (frame < MAX_FRAME)
#ifdef _WINDOWS
    if (GL_Windows[frame])
      SwapBuffers (GL_Windows[frame]);
#else
  if (FrameTable[frame].WdFrame)
    gtk_gl_area_swapbuffers (GTK_GL_AREA(FrameTable[frame].WdFrame));
#endif /*_WINDOWS*/
}

/*----------------------------------------------------------------------
   GL_prepare: If a modif has been done
  ----------------------------------------------------------------------*/
ThotBool GL_prepare (int frame)
{  

  if (frame < MAX_FRAME)
    {
      FrameTable[frame].DblBuffNeedSwap = TRUE;

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
void GL_realize ()
{
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
 GL_DrawAll : Only function that Really Draw opengl !!
  ----------------------------------------------------------------------*/
void GL_DrawAll (ThotWidget widget, int frame)
{  
#ifdef _PCLDEBUGTIME

  /* draw and calculate draw time 
     bench that helps finding bottlenecks...*/
  struct timeb	before;
  struct timeb	after;
  int	dsec, dms; 
#endif /*_PCLDEBUG*/
  
  /* if (GL_ANIM)
     RefreshAnimation (frame); */

  if (!GL_Drawing && !FrameUpdating )
    {
      for (frame = 1 ; frame < MAX_FRAME; frame++)
	{
	  if (FrRef[frame] != 0)
	    {
	      
	    if (FrameTable[frame].DblBuffNeedSwap)
	      {

	
		if (documentDisplayMode[FrameTable[frame].FrDoc - 1] 
		    != NoComputedDisplay)
		  {
		    widget = FrameTable[frame].WdFrame;
		    if (GL_prepare (frame))
		      	{
#ifdef _PCLDEBUGTIME
			  ftime(&before);

#endif /*_PCLDEBUG*/ 
			  GL_Drawing = TRUE;  	   
			  
			  RedrawFrameBottom (frame, 0, NULL);
			  glFinish ();
			  /*
			    saveBuffer (FrameTable[frame].FrWidth, FrameTable[frame].FrHeight);
			  */
			  GL_Swap (frame);
			  if (GL_Err())
#ifdef _GTK
			    g_print ("Bad drawing\n"); 
#else /*_GTK*/
			  WinErrorBox (NULL, "Bad drawing\n");
#endif /*_GTK*/
			  

#ifdef _PCLDEBUGTIME
			  ftime(&after);	
			  
			  dsec = after.time - before.time;	
			  dms = after.millitm - before.millitm;
			  if (dms > 0 )
			    {
			      g_print ("%d fps \t", (int) 1000/dms);
			      g_print ("=>\t %is %ims / frame\n", dsec, dms);
			    } 
			  
#endif /*_PCLDEBUG*/
			  GL_Drawing = FALSE;
			  FrameTable[frame].DblBuffNeedSwap = FALSE;
			}
		  }
	      }
	    }
	}
    }    
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
      glClearColor (1, 0, 0, 0);
      /* no fog*/
      glDisable (GL_FOG);
      /* No lights */
      glDisable (GL_LIGHTING);
      glDisable (GL_LIGHT0);
      glDisable (GL_AUTO_NORMAL);
      glDisable (GL_NORMALIZE);
      glDisable (GL_COLOR_MATERIAL);
      /* No z axis (SVG is 2d)  */
      glDisable (GL_DEPTH_TEST);
      glDepthMask (FALSE);
      /* No stencil buffer (one day perhaps, for background)*/
      glDisable (GL_STENCIL_TEST);
      /* At the beginning, 
	 there was no clipping*/
      glEnable (GL_SCISSOR_TEST);

      /* Modulated Transparency*/
      glDisable (GL_ALPHA_TEST); 	 
      /* Polygon are alway filled (until now)
	 Because Thot draws outlined polygons with lines
	 so...  if blending svg => GL_FRONT_AND_BACK*/
      glPolygonMode (GL_FRONT, GL_FILL);
      /*Hardware opengl may 
	support better rendering*/
      /*
      glEnable (GL_DITHER);
      */
      /*  Antialiasing 
	  Those Options give better 
	  quality image upon performance loss
	  Must be a user Option  */
      glEnable (GL_LINE_SMOOTH); 
      glEnable (GL_POINT_SMOOTH); 
      glHint (GL_POINT_SMOOTH_HINT, 
	      GL_NICEST);
      glHint (GL_LINE_SMOOTH_HINT, 
	      GL_NICEST); 
      /* For transparency and beautiful antialiasing*/
      glEnable (GL_BLEND); 
      glBlendFunc (GL_SRC_ALPHA, 
		   GL_ONE_MINUS_SRC_ALPHA); 
      /* Fastest Texture Mapping*/
      glHint (GL_PERSPECTIVE_CORRECTION_HINT, 
	      GL_FASTEST );    
      /* Bitmap font Text writing (even in texture font)*/
      glPixelStorei (GL_UNPACK_ALIGNMENT, 1); 
      /* Needs to clear buffer after allocating it before drawing*/
      glClear (GL_COLOR_BUFFER_BIT);
      /*(needed for gradients)*/
      /*glShadeModel (GL_SMOOTH);*/
      /* no gradients for now => */
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
      /*  If not in software mode,
	  glcopypixels is 1000x slower than a redraw	*/
      
      /*if (glMatroxBUG (frame, xf, yf, width, height))
	return;*/
      

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
	  
	  

	  /* Copy from backbuffer to backbuffer */
	  glFinish ();
	  glDisable (GL_BLEND);

	  glDisable (GL_DITHER);
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

	  glCopyPixels (x_source,   
			y_source,
			width,
			height,
			GL_COLOR); 
	  glEnable (GL_BLEND);
	  glEnable (GL_DITHER);
	  /*copy from back to front */
	  glFinish ();
	  FrameTable[frame].DblBuffNeedSwap = TRUE;
	}
}


/*-----------------------------------
 GLResize : 
 remake the current coordonate system 
 upon resize
------------------------------------*/
void GLResize (int width, int height, int x, int y)
{	
#ifdef _GTK
  /*
    gdk_gl_wait_gdk();
    gdk_gl_wait_gl();
  */
#endif /*_GTK*/

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
  glScissor (0, 0, 
	     width, height); 

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
      RedrawFrameBottom (frame, 0, NULL);
      return 1;      
    }
  return 0;  
}


/*-------------------------------
 SaveBuffer :
 Take a picture (png) of the backbuffer.
--------------------------------*/
void saveBuffer (int width, int height)
{
  static int z = 0;
  unsigned char *Data;

  z++;
  if (z != 20)
   return;
  width = 100;
  height = 200;
  Data = TtaGetMemory (sizeof (unsigned char) * width * height * 4);
  glReadPixels (0, 0, width, height, 
		GL_RGBA, 
		GL_UNSIGNED_BYTE, Data);
  SavePng ("screenshot.png", 
	   Data,
	   (unsigned int) width,
	   (unsigned int) height);
    /* savetga ("screenshot.tga", 
	   Data,
	   width,
	   height);*/
  free (Data);
}

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
#endif /*_PCLDEBUG*/

#endif /* _GL */
