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
#define DEG_TO_RAD(A)   ((float)A)/57.29577957795135
#define RAD_TO_DEG(A)   ((float)A)*57.29577957795135

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

#ifdef _GTK

#include <gtkgl/gtkglarea.h>
/* Unix timer */
#include <unistd.h>
#include <sys/timeb.h>


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

#ifdef GL_MESA_window_pos
#define MESA
#endif

/* Texture Font */
/* ~/Amaya/thotlib/internals/h */
#include "openglfont.h"
#include "glwindowdisplay.h"

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

/* background color*/
static int BG_Frame;
/*Current Thickness*/
static int S_thick;
/* if a refresh is needed, it is TRUE*/
static ThotBool GL_Modif = FALSE;

#ifdef _WINDOWS 

/* Win32 opengl context based on frame number*/
static HDC   GL_Windows[50];	
static HGLRC GL_Context[50];

/*----------------------------------------------------------------------
 GL_SetupPixelFormat : Sets up opengl buffers pixel format.
 Double Buffer, RGBA (32 bits), 
 no depth (z-buffer), no stencil (boolean buffer), no alpha (transparency), 
 no accum (special effect like multisampling, antialiasing), no aux (all purpose buffers),
 no pbuffers (?) buffers...
  ----------------------------------------------------------------------*/
static void GL_SetupPixelFormat (HDC hDC)
{
    PIXELFORMATDESCRIPTOR pfd = 
	{
        sizeof(PIXELFORMATDESCRIPTOR),  /* size */
        1,                              /* version */
        PFD_SUPPORT_OPENGL
         | PFD_DRAW_TO_WINDOW 
         | PFD_DOUBLEBUFFER,               /* support double-buffering */
        PFD_TYPE_RGBA,                  /* color type */
        32,                             /* prefered color depth */
        0, 0, 0, 0, 0, 0,               /* color bits (ignored) */
        0,                              /* no alpha buffer */
        0,                              /* alpha bits (ignored) */
        0,                              /* no accumulation buffer */
        0, 0, 0, 0,                     /* accum bits (ignored) */
        0,                             /* depth buffer */
        0,                              /* no stencil buffer */
        0,                              /* no auxiliary buffers */
        PFD_MAIN_PLANE,                 /* main layer */
        0,                              /* reserved */
        0, 0, 0,                        /* no layer, visible, damage masks */
    };
    int pixelFormat;

    pixelFormat = ChoosePixelFormat(hDC, &pfd);
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
  HGLRC hGLRC;
  HDC hDC;
  ThotBool found;

  hDC = GetDC (hwndClient);
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
      ActiveFrame = frame;
    }
  GL_SetupPixelFormat (hDC);
  hGLRC = wglCreateContext (hDC);
  if (GL_Context[1]) 
    wglShareLists (GL_Context[1], hGLRC);
  if (wglMakeCurrent (hDC, hGLRC))
    {
      SetGlPipelineState ();
      if (!dialogfont_enabled)
	{
	  InitDialogueFonts ("");
	  dialogfont_enabled = TRUE;
	}
    }
  GL_Windows[frame] = hDC;
  GL_Context[frame] = hGLRC;
  ActiveFrame = frame;
}

/*----------------------------------------------------------------------
 GL_Win32ContextInit : Free opengl contexts
  ----------------------------------------------------------------------*/
void GL_Win32ContextClose (int frame)
{
  /* make our context 'un-'current */
  wglMakeCurrent (NULL, NULL);
  /* delete the rendering context */
  wglDeleteContext (GL_Context[frame]);
  GL_Windows[frame] = 0;
  GL_Context[frame] = 0;
}
#endif /*_WINDOWS*/



/*----------------------------------------------------------------------
 GL_SetForeground : set color before drawing a or many vertex
  ----------------------------------------------------------------------*/
void GL_SetForeground (int fg)
{
    unsigned short red, green, blue;

    TtaGiveThotRGB (fg, &red, &green, &blue);
    glColor4ub (red, green, blue, 255);
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
	   glLineWidth (0.1); 
	   glPointSize (0.1); 
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
	  glLineWidth (0.1); 
	  glPointSize (0.1); 
	}
     glEnable (GL_LINE_STIPPLE);
     glLineStipple (2, *((int *) (&dash[0]))); 
    }
  GL_SetForeground (fg);
}

/*----------------------------------------------------------------------
   GL_DrawEmptyRectangle Outlined rectangle
  ----------------------------------------------------------------------*/
void  GL_ClearBackground (int width, int height)
{
  GL_SetForeground (BG_Frame);
  glBegin (GL_QUADS);
  glVertex2f (  0, 0 );
  glVertex2f (  0, 0 + height);
  glVertex2f (  0 +  width, 0 + height);
  glVertex2f (  0 + width, 0);
  glEnd ();
}
/*----------------------------------------------------------------------
  GL_SetBackground : Save background color ('cause also used in GL_Clear)
  ----------------------------------------------------------------------*/
void GL_SetBackground (int bg, int frame)
{   
  BG_Frame = bg;
  return;
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
   GL_ClearArea
    Create a plane filled with background color
    at the size of the clear area
    (sort of an erase tool, corresponding to a gdk_clear...)
  ----------------------------------------------------------------------*/
void GL_ClearArea (int x, int y, int width, int height)
{
   if (width == 0 || height == 0)
    return;
  GL_SetForeground (BG_Frame); 
  glBegin (GL_QUADS); 
  glVertex2f (x, y); 
  glVertex2f (x, y + height);
  glVertex2f (x +  width, y + height);
  glVertex2f (x + width, y);
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
void GL_DrawArc (int x, int y, int w, int h, int angle1, int angle2, ThotBool filled)
{  
  float angle, anglefinal, fastx, fasty;

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
  fastx = x + w;
  fasty = y + h;
  
  angle =  DEG_TO_RAD (angle2);
  anglefinal = DEG_TO_RAD (angle1);
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
      angle =  DEG_TO_RAD (angle2);
      glBegin (GL_POINTS);
      /* another one is to extend the use of Vertex array...
	 but not only for here... for all computations*/
      while (angle1 <= angle2)
	{
	  glVertex2f ( fastx + w * DCOS(angle),
		       fasty - h * DSIN(angle));
	  angle -= A_DEGREE;
	  angle2--;
	}
      glVertex2f ( fastx + w * DCOS (angle),
		   fasty - h * DSIN (angle));
      glEnd();
    }
  angle = DEG_TO_RAD (angle2);
  if (filled)
    {
      glBegin (GL_TRIANGLE_FAN);
      /* The center */
      glVertex2f (fastx, fasty);
    }
  else
    {
      glDisable (GL_BLEND);
      glBegin (GL_LINE_STRIP);
    }
  while (angle1 <= angle2)
    {
      glVertex2f ( fastx + w * DCOS(angle),
		   fasty - h * DSIN(angle));
      angle -= A_DEGREE;
      angle2--;
    }
  glVertex2f ( fastx + w * DCOS (angle),
	       fasty - h * DSIN (angle));
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
#ifdef MESA
/*----------------------
  ResetPixelTransferBias
------------------------*/
static void ResetPixelTransferBias ()
{
  static GLuint Precompiled = 0;
  
  if (!Precompiled)
    {
      Precompiled = glGenLists (1);
      glNewList (Precompiled,  GL_COMPILE);
      
      glPixelTransferf (GL_RED_BIAS, 0.0); 
      glPixelTransferf (GL_GREEN_BIAS, 0.0); 
      glPixelTransferf (GL_BLUE_BIAS, 0.0); 
      
      glEndList ();
    }
  glCallList (Precompiled); 
}
#define BIT8DIVIDE(A) ((float)A / 255)
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
#endif /* MESA */
/*----------------------------------------------------------------------
 GL_DrawString : Draw a string in a texture or a bitmap 
  ----------------------------------------------------------------------*/
int GL_DrawString (char const *str, float x, float y, 
			  void *GL_font, int fg)
{ 
  int width;

#ifdef MESA
  SetPixelTransferBias (fg); 
  glRasterPos2f (x, y);
  width = gl_draw_text (GL_font, str);
  ResetPixelTransferBias();
#else /* MESA */
  GL_SetForeground (fg);
  glPushMatrix ();
  glTranslatef (x, y, 0.0);
  glEnable (GL_TEXTURE_2D);
  width = gl_draw_text (GL_font, str);
  glDisable (GL_TEXTURE_2D);  
  glPopMatrix ();
#endif /* MESA */
  return width;
}
/*----------------------------------------------------------------------
 GL_DrawString : Draw a string in a texture or a bitmap 
  ----------------------------------------------------------------------*/
int GL_UnicodeDrawString (int fg, 
			  CHAR_T *str, 
			  float x, float y, 
			  int hyphen,
			  void *GL_font, int end)
{
  int width;

  if (end <= 0 || fg < 0 || GL_font == NULL)
    return 0;
  if (hyphen)
    {
      str[end] = '\255';
      str[end+1] = EOS;
    }
  else
    str[end] = EOS;
  /*TranslateUnicodeChars (str); */
#ifdef MESA
  SetPixelTransferBias (fg);
  glRasterPos2f (x, y);
  width = UnicodeFontRender (GL_font, str, x, y, end);
  ResetPixelTransferBias();
#else /* MESA */
  GL_SetForeground (fg);
  glPushMatrix ();
  glTranslatef (x, y, 0.0);
  glEnable (GL_TEXTURE_2D);
  width = UnicodeFontRender (GL_font, str, x, y, end);
  glDisable (GL_TEXTURE_2D);  
  glPopMatrix ();
#endif /* MESA */ 
  return width;
}
/*----------------------------------------------------------------------
  GL_DrawUnicodeChar : draw a character in a texture or a bitmap 
  ----------------------------------------------------------------------*/
void GL_DrawUnicodeChar (CHAR_T const c, float x, float y, void *GL_font, int fg)
{
  CHAR_T str[2];

  str[0] = c;
  str[1] = '\0';
  GL_UnicodeDrawString (fg, str, x, y, 0, GL_font, 1);
}
/*----------------------------------------------------------------------
  GL_DrawChar : draw a character in a texture or a bitmap 
  ----------------------------------------------------------------------*/
void GL_DrawChar (char const c, float x, float y, void *GL_font, int fg)
{
  char str[2];

  str[0] = c;
  str[1] = '\0';
  GL_DrawString (str, x, y, GL_font, fg);
}

#define REALY(A) (A + FrameTable[frame].FrTopMargin)
/*----------------------------------------------------------------------
  CharacterWidth returns the width of a char in a given font.
  ----------------------------------------------------------------------*/
int UnicodeCharacterWidth (CHAR_T c, PtrFont font)
{
  int                 i, l;

  if (font == NULL)
    return 0;
  else if (c == INVISIBLE_CHAR)
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
      if (c == THIN_SPACE)
	l = gl_font_char_width ((void *) font, 32) / 4;
      else if (c == HALF_EM)
	l = gl_font_char_width ((void *) font, 32) / 2;
      else
	l = gl_font_char_width ((void *) font, c);
#ifndef _WINDOWS
      if (c == 244)
	{
	  /* a patch due to errors in standard symbol fonts */
	  i = 0;
	  while (i < MAX_FONT && font != TtFonts[i])
	    i++;
	  if (TtPatchedFont[i] == 8 || TtPatchedFont[i] == 10)
	    l = 1;
	  else if (TtPatchedFont[i] == 12 || TtPatchedFont[i] == 14)
	    l = 2;
	  else if (TtPatchedFont[i] == 24)
	    l = 4;
	}
#endif /*_WINDOWS*/
    }
  return l;
}
#ifdef _I18N_
/*----------------------------------------------------------------------
  DisplayJustifiedText display the content of a Text box tweaking
  the space sizes to ajust line length to the size of the frame.
  Remaining pixel space (BxNPixels) is equally dispatched 
  on all spaces in the line.
  ----------------------------------------------------------------------*/
void DisplayJustifiedText (PtrBox pBox, PtrBox mbox, int frame,
				  ThotBool selected)
{
  PtrTextBuffer       adbuff;
  ViewFrame          *pFrame;
  PtrBox              nbox;
  PtrAbstractBox      pAb;
  SpecFont            font;
  PtrFont             prevfont = NULL;
  PtrFont             nextfont = NULL;
  CHAR_T              bchar;
  CHAR_T              *bbuffer;
  int                 restbl;
  int                 newbl, lg;
  int                 charleft;
  int                 buffleft;
  int                 indbuff, bl;
  int                 indmax;
  int                 nbcar, x, y, y1;  /******/
  int                 lgspace, whitespace;
  int                 fg, bg;
  int                 shadow;
  int                 width;
  int                 left, right;
  ThotBool            blockbegin;
  ThotBool            withbackground;
  ThotBool            hyphen, rtl;

  indmax = 0;
  buffleft = 0;
  adbuff = NULL;
  indbuff = 0;
  restbl = 0;
  pAb = pBox->BxAbstractBox;
  /* is it a box with a right-to-left writing? */
  bchar = pBox->BxScript;
  if (pAb->AbUnicodeBidi == 'O')
    rtl = (pAb->AbDirection == 'R');
  else
    rtl = (bchar == 'A' || bchar == 'H');
  font = pBox->BxFont;
  /* do we have to display stars instead of characters? */
  if (pAb->AbBox->BxShadow)
    shadow = 1;
  else
    shadow = 0;
  
  /* Is this box the first of a block of text? */
  if (mbox == pBox)
    blockbegin = TRUE;
  else if (mbox->BxType != BoBlock || mbox->BxFirstLine == NULL)
    blockbegin = TRUE;
  else if (pBox->BxType == BoComplete && mbox->BxFirstLine->LiFirstBox == pBox)
    blockbegin = TRUE;
  else if ((pBox->BxType == BoPiece ||
	    pBox->BxType == BoScript ||
	    pBox->BxType == BoDotted) &&
	   mbox->BxFirstLine->LiFirstPiece == pBox)
    blockbegin = TRUE;
  else
    blockbegin = FALSE;
  
  /* Is an hyphenation mark needed at the end of the box? */
  if (pBox->BxType == BoDotted)
    hyphen = TRUE;
  else
    hyphen = FALSE;
  /* in SVG foreground and background are inverted in the main view */
  if (!strcmp(pAb->AbElement->ElStructSchema->SsName, "SVG") &&
      FrameTable[frame].FrView == 1)
    {
      bg = pAb->AbForeground;
      fg = pAb->AbBackground;
      withbackground = FALSE;
    }
  else
    {
      fg = pAb->AbForeground;
      bg = pAb->AbBackground;
      withbackground = (pAb->AbBox->BxFill && pAb->AbBox->BxDisplay);
    }
  pFrame = &ViewFrameTable[frame - 1];
  left = 0;
  right = 0;
  if (pAb->AbVisibility >= pFrame->FrVisibility)
    {
      /* Initialization */
      x = pBox->BxXOrg + pBox->BxLMargin + pBox->BxLBorder +
	  pBox->BxLPadding - pFrame->FrXOrg;
      y = pBox->BxYOrg + pBox->BxTMargin + pBox->BxTBorder +
	  pBox->BxTPadding - pFrame->FrYOrg;
      /* no previous spaces */
      bl = 0;
      charleft = pBox->BxNChars;
      newbl = pBox->BxNPixels;
      lg = 0;
	   
      /* box sizes have to be positive */
      width = pBox->BxW;
      if (width < 0)
	width = 0;
      whitespace = BoxCharacterWidth (SPACE, font);
      lgspace = pBox->BxSpaceWidth;
      if (lgspace == 0)
	lgspace = whitespace;
      
      /* locate the first character */
      LocateFirstChar (pBox, rtl, &adbuff, &indbuff);
      /* Search the first displayable char */
      if (charleft > 0 && adbuff)
	{
	  /* there is almost one character to display */
	  do
	    {
	      /* skip invisible characters */
	      restbl = newbl;
	      x += lg;
	      bchar = adbuff->BuContent[indbuff];
	      if (bchar == SPACE)
		{
		  lg = lgspace;
		  if (newbl > 0)
		    {
		      newbl--;
		      lg++;
		    } 
		}
	      else
		lg = CharacterWidth (bchar, nextfont);
	      /* Skip to the next char */
	      if (x + lg <= 0)
		{
		  if (LocateNextChar (&adbuff, &indbuff, rtl))
		    charleft--;
		  else
		    charleft = 0;
		}
	    }
	  while (x + lg <= 0 && charleft > 0);
	   
	  /* Display the list of text buffers pointed by adbuff */
	  /* beginning at indbuff and of lenght charleft.       */
	  /* -------------------------------------------------- */
	  if (adbuff == NULL)
	    charleft = 0;
	  else
	    {
	      /* number of characters to be displayed in the current buffer */
	      if (rtl)
		{
		  buffleft = indbuff + 1;
		  if (charleft < buffleft)
		    {
		      indmax = indbuff - charleft;
		      buffleft = charleft;		      
		    }
		  else
		    indmax = 0;
		}
	      else
		{
		  buffleft = adbuff->BuLength - indbuff;
		  if (charleft < buffleft)
		    {
		      indmax = indbuff + charleft - 1;
		      buffleft = charleft;
		    }
		  else
		    indmax = adbuff->BuLength - 1;
		}
	    } 
	  
	  /* Do we need to draw a background */
	  if (withbackground)
	    DrawRectangle (frame, 0, 0,
			   x - pBox->BxLPadding, y - pBox->BxTPadding,
			   width + pBox->BxLPadding + pBox->BxRPadding,
			   BoxFontHeight (font) + pBox->BxTPadding + pBox->BxBPadding,
			   0, bg, 2);
	}

      /* check if the box is selected */
      if (selected)
	{
	  if (pBox == pFrame->FrSelectionBegin.VsBox ||
	      pBox == pFrame->FrSelectionEnd.VsBox)
	    {
	      if (pFrame->FrSelectOnePosition)
		{
		  left = pFrame->FrSelectionBegin.VsXPos;
		  right = left + 2;
		}
	      else
		{
		  /* almost one character is selected */
		  if (pBox == pFrame->FrSelectionBegin.VsBox)
		    left = pFrame->FrSelectionBegin.VsXPos;
		  if (pBox == pFrame->FrSelectionEnd.VsBox &&
		      pFrame->FrSelectionEnd.VsXPos != 0)
		    right = pFrame->FrSelectionEnd.VsXPos;
		  else
		    right = pBox->BxWidth;
		  DisplayStringSelection (frame, left, right, pBox);
		  /* the selection is done now */
		  left = 0;
		  right = 0;
		}
	    }
	  else if (pBox->BxType == BoPiece ||
		   pBox->BxType == BoScript ||
		   pBox->BxType == BoDotted)
	    {
	      /* check if the box in within the selection */
	      if (pFrame->FrSelectionBegin.VsBox &&
		  pAb == pFrame->FrSelectionBegin.VsBox->BxAbstractBox)
		{
		  nbox = pFrame->FrSelectionBegin.VsBox;
		  while (nbox && nbox != pFrame->FrSelectionEnd.VsBox &&
			 nbox != pBox)
		    nbox = nbox->BxNexChild;
		  if (nbox == pBox)
		    /* it's within the current selection */
		    DisplayBgBoxSelection (frame, pBox);
		}
	      else if (pFrame->FrSelectionEnd.VsBox &&
		       pAb == pFrame->FrSelectionEnd.VsBox->BxAbstractBox)
		{
		  nbox = pBox->BxNexChild;
		  while (nbox && nbox != pFrame->FrSelectionEnd.VsBox)
		    nbox = nbox->BxNexChild;
		  if (nbox == pFrame->FrSelectionEnd.VsBox)
		    /* it's within the current selection */
		    DisplayBgBoxSelection (frame, pBox);
		}
	      else
		DisplayBgBoxSelection (frame, pBox);
	    }
	  else
	    DisplayBgBoxSelection (frame, pBox);
	}

      /* allocate a buffer to store converted characters */
      bbuffer = TtaGetMemory (sizeof (CHAR_T) * (pBox->BxNChars + 2));  
      nbcar = 0;
      while (charleft > 0)
	{
	  /* handle each char in the buffer */
	  while ((rtl && indbuff >= indmax) ||
		 (!rtl && indbuff <= indmax))
	    {
	      bchar = adbuff->BuContent[indbuff];
	      GetFontAndIndexFromSpec (bchar, font, &nextfont);
	      if (bchar == SPACE || bchar == THIN_SPACE ||
		  bchar == HALF_EM || bchar == UNBREAKABLE_SPACE || bchar == TAB ||
		  bchar == EOL)
		{
		  /* display previous chars handled */
		  if (nbcar > 0)
		    {
		      y1 = y + BoxFontBase (pBox->BxFont);
		      x += GL_UnicodeDrawString (fg, bbuffer, 
						 x, REALY(y1),  
						 0, prevfont, nbcar); 

		      /* all previous spaces are declared */
		      bl = 0;
		    }		  
		  if (shadow)
		    {
		      y1 = y + BoxFontBase (pBox->BxFont);
		      GL_DrawUnicodeChar ('*', x, y1, nextfont, fg);
		    }
		  else if (!ShowSpace)
		    {
		      /* Show the space chars */
		      if (bchar == SPACE || bchar == TAB) 
			GL_DrawUnicodeChar (SHOWN_SPACE, x, y,
					    nextfont, fg);
		      else if (bchar == THIN_SPACE)
			GL_DrawUnicodeChar (SHOWN_THIN_SPACE, x, y, 
					    nextfont, fg);
		      else if (bchar == HALF_EM)
			GL_DrawUnicodeChar (SHOWN_HALF_EM, x, y, 
					    nextfont, fg);
		      else if (bchar == UNBREAKABLE_SPACE)
			GL_DrawUnicodeChar (SHOWN_UNBREAKABLE_SPACE, x, y,
					    nextfont, fg);
		    }
		 
		  nbcar = 0;
#ifdef _WINDOWS
		  if (bchar != EOS)
#else /* _WINDOWS */
		  if (!Printing)
#endif /* _WINDOWS */
		    {
		  if (bchar == SPACE)
		    {
		      if (restbl > 0)
			{
			  /* Pixel space splitting */
			  x = x + lgspace + 1;
			  restbl--;
			}
		      else
			x += lgspace;
		    }
		  else
		    x += UnicodeCharacterWidth (bchar, nextfont);
		    }
		  /* a new space is handled */
		  bl++;
		}
	      else if (bchar == INVISIBLE_CHAR)
		/* do nothing */;
	      else if (nextfont == NULL && bchar == UNDISPLAYED_UNICODE)
		{
		  /* display previous chars handled */
		  if (nbcar > 0)
		    {
		      y1 = y + BoxFontBase (pBox->BxFont);
		      x += GL_UnicodeDrawString (fg, bbuffer, 
						  x, REALY(y1),  
						  0, prevfont, nbcar); 
		    }
		  nbcar = 0;
		  /* all previous spaces are declared */
		  bl = 0;
		  prevfont = nextfont;
		  DrawRectangle (frame, 1, 5, x, y, 6, pBox->BxH - 1, fg, 0, 0);
		  x += 6;
		}
	      else
		{
		  if (prevfont != nextfont)
		    {
		      /* display previous chars handled */
		      if (nbcar > 0)
			{
		          y1 = y + BoxFontBase (pBox->BxFont);
			  x += GL_UnicodeDrawString (fg, bbuffer, 
						      x, REALY(y1),  
						      0, prevfont, nbcar); 
			  /* all previous spaces are declared */
			  bl = 0;
			}
		      nbcar = 0;
		      prevfont = nextfont;
		    }
		  /* add the new char */
		  bbuffer[nbcar++] = bchar; 
		}
	      /* Skip to next char */
	      if (rtl)
		indbuff--;
	      else
		indbuff++;
	    }

	  /* Draw previous chars in the buffer */
	  charleft -= buffleft;
	  if (charleft > 0)
	    {
	      /* number of characters to be displayed in the next buffer */
	      if (rtl)
		{
		  if (adbuff->BuPrevious == NULL)
		    charleft = 0;
		  else
		    {
		      adbuff = adbuff->BuPrevious;
		      indbuff = adbuff->BuLength - 1;
		      buffleft = adbuff->BuLength;
		      if (charleft < buffleft)
			{
			  indmax = indbuff - charleft;
			  buffleft = charleft;		      
			}
		      else
			indmax = 0;
		    }
		}
	      else
		{
		  if (adbuff->BuNext == NULL)
		    charleft = 0;
		  else
		    {
		      adbuff = adbuff->BuNext;
		      indbuff = 0;
		      buffleft = adbuff->BuLength;
		      if (charleft < buffleft)
			{
			  indmax = charleft - 1;
			  buffleft = charleft;		      
			}
		      else
			indmax = adbuff->BuLength - 1;
		    }
		}
	    }
	  if (charleft <= 0)
	    {
	      /*
		Draw the content of the buffer.
		Call the function in any case to let Postscript justify the
		text of the box.
	      */
	      if (nbcar == 0)
		bl = 0;
	      y1 = y + BoxFontBase (pBox->BxFont);
	      x += GL_UnicodeDrawString (fg, bbuffer,
					  x, REALY(y1), 
					  hyphen, prevfont, nbcar);
	      if (pBox->BxUnderline != 0)
		DisplayUnderline (frame, x, y, nextfont,
				  pBox->BxUnderline, width, fg);
	      nbcar = 0;
	    }
	} 
      
      /* Should the end of the line be filled with dots */
      if (pBox->BxEndOfBloc > 0)
	{
	  /* fill the end of the line with dots */
	  x = pBox->BxXOrg + pBox->BxLMargin + pBox->BxLBorder +
	    pBox->BxLPadding;
	  y = pBox->BxYOrg + pBox->BxHorizRef - pFrame->FrYOrg;
	  DrawPoints (frame, pBox->BxXOrg + width - pFrame->FrXOrg, y,
		      pBox->BxEndOfBloc, fg);
	}
      /* display a caret if needed */
      if (left != right)
	DisplayStringSelection (frame, left, right, pBox);
      TtaFreeMemory (bbuffer);  
    }
}
#endif /* I18N */

/*----------------------------------------------------------------------
   GL_prepare: If a modif has been done
  ----------------------------------------------------------------------*/
ThotBool GL_prepare (ThotWidget *widget)
{  
  GL_Modif = TRUE; 
  return TRUE;
}

/*----------------------------------------------------------------------
   GL_realize : can we cancel if no modifs ?
  ----------------------------------------------------------------------*/
void GL_realize (ThotWidget *widget)
{
  return;
}

/*----------------------------------------------------------------------
   GL_ActivateDrawing : Force Recalculation of the frame and redisplay
  ----------------------------------------------------------------------*/
void GL_ActivateDrawing()
{
  GL_Modif = TRUE;
}



/*----------------------------------------------------------------------
 GL_DrawAll : Only function that Really Draw opengl !!
  ----------------------------------------------------------------------*/
void GL_DrawAll (ThotWidget widget, int frame)
{  
#ifdef _GTK
  struct timeb	before;
  struct timeb	after;
  int	dsec, dms; 
 
  /* draw and calculate draw time 
     bench that helps finding bottlenecks...*/

  /*      for animation GL_modif always true */
  /* GL_Modif=TRUE;  */
   if (GL_Modif && !GL_Drawing && !FrameUpdating)	
     { 
       ftime(&before);
       
       ActiveFrame = frame;
       if (gtk_gl_area_make_current (GTK_GL_AREA(widget)))
	 { 	  
	   /* prevent other computation at 
	      the same time*/
	   GL_Drawing = TRUE;  
	   
	   /* Redraw ALL THE CANVAS (Animation testing)
	      usually only modified buffer will be copied 
	      into the frame buffer */      
	   
	   /*  DrawGrid (FrameTable[frame].WdFrame->allocation.width,  */
	   /* 		FrameTable[frame].WdFrame->allocation.height); */
	   
	   DefClip (frame, -1, -1, -1, -1); 
	   RedrawFrameBottom (frame, 0, NULL);      
	   /*a resfresh indicator*/
	   /*  make_carre();	  */  
	   
	   /* Double Buffering */
	   gtk_gl_area_swapbuffers (GTK_GL_AREA(widget)); 
	   glFlush ();
	   /* Paints a background color 
	      Have to discard it if 
	      background image exist in document
	      Clear is after buffer swapping as it take 
	      times and is asynchronous with Amaya computation*/     
	   /* glClear(GL_COLOR_BUFFER_BIT); */          
	   
	   
		if (GL_Err())
	     g_print ("Bad drawing\n"); 
	   GL_Drawing = FALSE;
	 }
       
       GL_Modif = FALSE;
       
       ftime(&after);	
       
       dsec = after.time - before.time;	
       dms = after.millitm - before.millitm;
       if (dms > 0 )
	 {
	   g_print (" %d fps \t", (int) 1000/dms);
	   g_print ("=>\t %is %ims / frame\n", dsec, dms);
	 }
     }
#else /*_GTK*/
 if (GL_Modif && !GL_Drawing && !FrameUpdating)	
     { 
	   frame = ActiveFrame;
       if (wglMakeCurrent (GL_Windows[frame], GL_Context[frame]))
	   { 	  
	       /* prevent other computation at 
	      the same time*/
	      GL_Drawing = TRUE;  
	   
	      /* Redraw ALL THE CANVAS (Animation testing)
	      usually only modified buffer will be copied 
	      into the frame buffer */      
	   
	      /*  DrawGrid (FrameTable[frame].WdFrame->allocation.width,  */
	      /* 		FrameTable[frame].WdFrame->allocation.height); */

	      DefClip (frame, -1, -1, -1, -1); 
	      RedrawFrameBottom (frame, 0, NULL);      
	      /*a resfresh indicator*/
	      /*  make_carre();	  */  
	    
	      /* Double Buffering */
make_carre();
		  glFlush ();
	      SwapBuffers (GL_Windows[frame]);
	      /* Paints a background color 
	      Have to discard it if 
	      background image exist in document
	      Clear is after buffer swapping as it take 
	      times and is asynchronous with Amaya computation*/     
	      /* glClear(GL_COLOR_BUFFER_BIT); */	    
	   
		if (GL_Err())
			WinErrorBox (NULL, "Bad drawing\n");
	      GL_Drawing = FALSE;
	   }        
	   GL_Modif = FALSE;
	}
#endif /*_GTK*/
 
}

void SetGlPipelineState ()
{
      /* Display Opengl Vendor Name,  Opengl Version, Opengl Renderer*/
      /*g_print("%s, %s, %s", (char *)glGetString(GL_VENDOR), 
	      (char *)glGetString(GL_VERSION), 
	      (char *)glGetString(GL_RENDERER));   */
      /* g_print("%s\n", (char *)glGetString(GL_EXTENSIONS));  */   


      glClearColor (1, 1, 1, 0);
      /* only enable it when needed 
	 (color mix and better antialiasing)*/
      glDisable (GL_BLEND);
      /* Fast Transparency*/
      //glAlphaFunc (GL_LESS, 0.950);
      glDisable (GL_ALPHA_TEST); 
      /* no fog*/
      glDisable (GL_FOG);
      /* No lights */
      glDisable (GL_LIGHTING);
      glDisable (GL_LIGHT0);
      glDisable (GL_AUTO_NORMAL);
      glDisable (GL_NORMALIZE);
      glDisable (GL_COLOR_MATERIAL);
      /* No z axis (SVG is 2d) until X3D */
      glDisable (GL_DEPTH_TEST);
      /* No stencil buffer (one day perhaps, for background)*/
      glDisable (GL_STENCIL_TEST);

      /* svg viewports will use it, one day*/
      glDisable (GL_SCISSOR_TEST);	 

      /* Polygon are alway filled (until now)
	 Because Thot draws outlined polygons with lines
	 so...  if blending svg => GL_FRONT_AND_BACK*/
      glPolygonMode( GL_FRONT, GL_FILL );
     
      /* Doesn't compute hidden drawing 
	 Doesn't work for our tesselated
	 polygons   not CGW oriented...*/
      /* glEnable (GL_CULL_FACE); */
      /* glCullFace (GL_FRONT_AND_BACK.,GL_BACK, GL_FRONT); */

      /*(needed for gradients)*/
      glShadeModel (GL_SMOOTH);
#ifdef MESA
      /* No Texture and pixel drawing enhancement*/
      glDisable (GL_DITHER); 
      if (1)
	{
	  glEnable (GL_POINT_SMOOTH); 
	  glEnable (GL_LINE_SMOOTH);    
	  /* glEnable (GL_POLYGON_SMOOTH); */  
	  
	  /* glHint(GL_POINT_SMOOTH_HINT, GL_NICEST); */
	  /* 	  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST); */
	  /* 	  glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);  */
	  
	  glEnable (GL_BLEND);  
	  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
	}
      else
	{
	  /* No AntiAliasing*/ 
	  glDisable (GL_LINE_SMOOTH); 
	  glDisable (GL_POINT_SMOOTH);
	  glDisable (GL_POLYGON_SMOOTH); 
	}
      glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST ); 
#else /*!MESA*/
      /*Hardware opengl may support better rendering*/
      glEnable (GL_DITHER);glDisable (GL_DITHER); 
      /*  Antialiasing 
	  Those Options give better 
	  quality image upon performance loss
	  Must be a user Option  */
      glEnable (GL_LINE_SMOOTH); 
      glEnable (GL_POINT_SMOOTH); 
      glHint (GL_POINT_SMOOTH_HINT, GL_NICEST);
      glHint (GL_LINE_SMOOTH_HINT, GL_NICEST);

      /* Not recommended for hardware cards... 
	 Global Antialiasing is done elsewhere...*/
      /*  glEnable (GL_POLYGON_SMOOTH);   */
      /* smooth polygon antialiasing */
      /* glBlendFunc (GL_SRC_ALPHA_SATURATE, GL_ONE); */
      /* glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST); */

      /* For transparency and beautiful antialiasing*/
       glEnable (GL_BLEND); 
       glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
#endif /*MESA*/    
      /* Bitmap font Text writing (even in texture font)*/
      glPixelStorei( GL_UNPACK_ALIGNMENT, 1); 
	  /* Needs to clear buffer after allocating it before drawing*/
	  glClear(GL_COLOR_BUFFER_BIT);
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
void GL_BackBufferRegionSwapping (int x, int y, int width, int height, int Totalheight)
{  
#ifndef _WINDOWS
  /* copy form bottom to top
   so we must add height and 
  invert y */
  y = y + height;
  glRasterPos2i (x, y);
  glDrawBuffer (GL_FRONT);       
  glReadBuffer (GL_BACK);
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
  GL_window_copy_area
       We copy region content of the back buffer 
	    on the exposed region 
	    => opengl region buffer swapping 
--------------------------------------------*/
void GL_window_copy_area (int xf, int yf, int xd, int yd,
			  int width, int height)
{
#if defined (_WINDOWS) || !defined (MESA)
	GL_DrawAll (0, NULL);
#else /*_WINDOWS*/
	glRasterPos2i (xf, yf);
	glCopyPixels(xd, yd, width, height, GL_COLOR);
	glFlush ();
#endif /*_WINDOWS*/
}



/*-----------------------------------
 GLResize : 
 remake the current coordonate system 
 upon resize
------------------------------------*/
void GLResize (int width, int height, int x, int y)
{
  glViewport (0, 0, width, height);
  glMatrixMode (GL_PROJECTION);      
  glLoadIdentity (); 
  /* Invert the opengl coordinate system
     to get the same as Thot	  
     (opengl Y origin  is the left up corner
     and the left bottom is negative !!)	*/
  glOrtho (0, width, height, 0, -1, 1); 
  /* Needed for 3d only...*/
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity (); 
  GL_ClearBackground (width, height);
}


/*****************************************************/
/* TESTING */




/* Testing purpose function 
   Drawing grid (for canvas geometry tests)*/
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
}

/*-------------------------------
 saveBuffer :
 Take a picture (tga) of the backbuffer.
 mainly for debug purpose, but could be used for a
 C remplacment of Batik 
--------------------------------*/
int saveBuffer (int width, int height)
{
  static int z = 0;
  FILE *screenFile;
  unsigned char *Data;
  int length;

  unsigned char cGarbage = 0, type,mode,aux, pixelDepth;
  short int iGarbage = 0;
  int i;

  z++;
  /*if (z != 500)
    return;
  */
  length = width * height * 4;
  Data = TtaGetMemory (sizeof (unsigned char) * length);
  glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, Data);
  screenFile = fopen("screenshot.tga", "w");
  pixelDepth = 32;
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
  free(Data);
}


#endif /* _GL */
