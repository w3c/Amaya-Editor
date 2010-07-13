/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
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

#ifdef _WX
#include "wx/wx.h"
#endif /* _WX */

#include "thot_gui.h"
#include "ustring.h"
#include "math.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"
#include "appdialogue.h"
#include "picture.h"
#ifdef _WINGUI
#include "wininclude.h"
#endif /* _WINGUI */
#include "logdebug.h"

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
#include "content_f.h"
#include "context_f.h"
#include "font_f.h"
#include "inites_f.h"
#include "memory_f.h"
#include "units_f.h"
#include "windowdisplay_f.h"
#include "frame_f.h"
#include "animbox_f.h"
#include "picture_f.h"
#include "tesse_f.h"
#include "applicationapi_f.h"
#include "registry_f.h"
#include "dialogapi_f.h"

#include "glprint.h"

#if defined(_GTK)
#include <gtkgl/gtkglarea.h>
#endif /* #if defined(_GTK) */

#if defined(_GTK) || defined(_WX) && !defined(_WINDOWS)
/* Unix timer */
#include <unistd.h>
#endif /* #if defined(_GTK) || defined(_WX) && !defined(_WINDOWS) */

#if defined(_GTK) || defined(_WX)
#include <sys/timeb.h>
#endif /* #if defined(_GTK) || defined(_WX)*/

#ifdef _WX
#include "AmayaFrame.h"
#endif /* _WX */

#if defined (_MACOS) && defined (_WX)
#include <gl.h>
#include <glu.h>
#else /* _MACOS */
#include <GL/gl.h>
#include <GL/glu.h>
#endif /* _MACOS */

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
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_PI_DOUBLE
#define M_PI_DOUBLE (6.2831853718027492)
#endif

/* ((A)*(M_PI/180.0)) */
#define DEG_TO_RAD(A)   ((GLfloat)A)/57.29577957795135
#define RAD_TO_DEG(A)   ((GLfloat)A)*57.29577957795135

#define GL_EPSILON                 5.e-3
#define IS_ZERO(arg)                    (fabs(arg)<1.e-20)

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
#define SLICES 180
#define SLICES_SIZE 181
#define INTERVAL 0.02 /*1/FPS*/ /* should be 1/25 ... 1/50 */
#define REALY(A) (A + FrameTable[frame].FrTopMargin)

/*--------- STATICS ------*/
/*Current Thickness*/
static GLubyte  FillOpacity = 255;
static GLubyte  StrokeOpacity = 255;
static GLfloat  S_thick = 0.;
static int      X_Clip = 0;
static int      Y_Clip = 0;
static int      Width_Clip = 0;
static int      Height_Clip = 0;
/*if no 3d card available*/
static ThotBool Software_Mode = TRUE;
static ThotBool TransText = FALSE;


/*----------------------------------------------------------------------
  SetSoftware_Mode : If OpenGL is accelerated or not
  ----------------------------------------------------------------------*/
void SetSoftware_Mode (ThotBool value)
{
  Software_Mode = value;
}

/*----------------------------------------------------------------------
  GL_TransText: If text must be texture or polygon
  ----------------------------------------------------------------------*/
ThotBool GL_TransText ()
{
  return TransText;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
ThotBool GL_Err() 
{
  GLenum errCode = GL_NO_ERROR;
  if((errCode = glGetError ()) != GL_NO_ERROR)
    {
#ifdef _GTK
      g_print ("\n%s :", (char*) gluErrorString (errCode));
#endif /*_GTK*/
#ifdef _WX
      printf ("GL_Err: %s\n",(char*) gluErrorString (errCode));
#endif /*_WX*/
#ifdef _WINGUI
      WinErrorBox (NULL, (char*) gluErrorString (errCode));
#endif /*_WINGUI*/
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
#ifdef _GL_COLOR_DEBUG
  {
    float tmp[4];
    glGetFloatv( GL_COLOR_CLEAR_VALUE, tmp );
    TTALOGDEBUG_5( TTA_LOG_DRAW, _T("glClear CLEAR_VALUE(%f,%f,%f,%f) - frame=%d"),tmp[0],tmp[1],tmp[2],tmp[3], frame );
  }
#endif /* _GL_COLOR_DEBUG */
  glClear( GL_COLOR_BUFFER_BIT );
}

#ifdef _GTK
/*----------------------------------------------------------------------
 * Make the window background the same as the OpenGL one.  This
 * is used to avoid flickers when the widget size changes.
 * This is fixed in GTK+-2.0 but doesn't hurt.
 ----------------------------------------------------------------------*/
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
#endif /*_WIN_PRINT*/

/*----------------------------------------------------------------------
  GL_SetOpacity :
  ----------------------------------------------------------------------*/
void GL_SetOpacity (int opacity)
{
  FillOpacity = (GLubyte) ((opacity * 255)/1000);
}

/*----------------------------------------------------------------------
  GL_SetStrokeOpacity :
  ----------------------------------------------------------------------*/
void GL_SetStrokeOpacity (int opacity)
{
  StrokeOpacity = (GLubyte) ((opacity * 255)/1000);
}

/*----------------------------------------------------------------------
  GL_SetFillOpacity  :
  ----------------------------------------------------------------------*/
void GL_SetFillOpacity (int opacity)
{
  FillOpacity = (GLubyte) ((opacity * 255)/1000);
}

/*----------------------------------------------------------------------
  GL_SetClipping : prevent drawing outside this rectangle
  ----------------------------------------------------------------------*/
void GL_SetClipping (int x, int y, int width, int height)
{
#ifdef _WX
  wxASSERT_MSG( x>=0 && y>=0 && width>=0 && height>=0,
                _T("GL_SetClipping : one clipping value is negative") );
#endif /* _WX */
  TTALOGDEBUG_4( TTA_LOG_DRAW, _T("GL_SetClipping : x=%d y=%d w=%d h=%d"),
                 x, y, width, height );
  glEnable (GL_SCISSOR_TEST);
  glScissor (x, y, width, height);
  if (Width_Clip == 0 && Height_Clip == 0)
    {
      X_Clip = x;
      Y_Clip = y;
      Width_Clip = width;
      Height_Clip = height;
    }
}
/*----------------------------------------------------------------------
  GL_UnsetClippingRestore : restore previous clipping or 
  free the drawing from it
  ----------------------------------------------------------------------*/
void GL_UnsetClipping ()
{  
  glDisable (GL_SCISSOR_TEST);
  X_Clip = 0;
  Y_Clip = 0;
  Width_Clip = 0;
  Height_Clip = 0;
}

/*----------------------------------------------------------------------
  GL_GetCurrentClipping : get  the clipping
  ----------------------------------------------------------------------*/
void GL_GetCurrentClipping (int *x, int *y, int *width, int *height)
{  
  *x = X_Clip;
  *y = Y_Clip;
  *width = Width_Clip;
  *height= Height_Clip;
}

/*----------------------------------------------------------------------
  GL_SetForeground : set color before drawing a or many vertex
  ----------------------------------------------------------------------*/
void GL_SetForeground (int fg, ThotBool fillstyle)
{
  unsigned short  red, green, blue;
  GLubyte         us_opac;

  if (fillstyle)
    us_opac = FillOpacity;
  else
    us_opac = StrokeOpacity;
  TtaGiveThotRGB (fg, &red, &green, &blue);
  glColor4ub ((GLubyte) red,  (GLubyte) green, (GLubyte) blue, us_opac);
}


/*----------------------------------------------------------------------
  GL_SetPicForeground : set opacity before drawing a or many vertex
  ----------------------------------------------------------------------*/
void GL_SetPicForeground ()
{
  GLubyte         us_opac;

  us_opac = (GLubyte) FillOpacity;
  glColor4ub ((GLubyte) 255, (GLubyte) 255, (GLubyte) 255, us_opac);
}


/*----------------------------------------------------------------------
  InitDrawing update the Graphic Context accordingly to parameters.
  The parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void InitDrawing (int style, int thick, int fg)
{
  S_thick = (double)thick;
  if (thick)
    {
      glLineWidth (S_thick); 
      glPointSize (S_thick / 2.); 
    }
  else
    {
      glLineWidth ((GLfloat) 0.5); 
      glPointSize ((GLfloat) 0.25); 
    }
  if (style >= 5)
    /* solid */
    glDisable (GL_LINE_STIPPLE);
  else
    {
      if (style == 3)
        /* dotted */
        glLineStipple (thick, 0x5555);
      else
        /* dashed */
        glLineStipple (thick, 0x1F1F);
      glEnable (GL_LINE_STIPPLE);
    }
  GL_SetForeground (fg, FALSE);
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
void  GL_DrawEmptyRectangle (int fg, float x, float y, float width,
                             float height, float thick)
{ 
  thick = thick / 2.;
  glBegin (GL_QUADS);
  
  glVertex2f (x - thick, y - thick);
  glVertex2f (x + thick + width, y - thick);
  glVertex2f (x + thick + width, y + thick);
  glVertex2f (x - thick, y + thick);
  
  glVertex2f (x - thick, y - thick + height);
  glVertex2f (x + thick + width, y - thick + height);
  glVertex2f (x + thick + width, y + thick + height);
  glVertex2f (x - thick, y + thick + height);
  
  glVertex2f (x - thick, y - thick);
  glVertex2f (x + thick, y - thick);
  glVertex2f (x + thick, y + thick + height);
  glVertex2f (x - thick, y + thick + height);
  
  glVertex2f (x - thick + width, y - thick);
  glVertex2f (x + thick + width, y - thick);
  glVertex2f (x + thick + width, y + thick + height);
  glVertex2f (x - thick + width, y + thick + height);
  glEnd ();
}

/*----------------------------------------------------------------------
  GL_DrawRectangle
  (don't use glrect because it's exactly the same but require opengl 1.2)
  ----------------------------------------------------------------------*/
void GL_DrawRectangle (int fg, float x, float y, float width, float height)
{
  GL_SetForeground (fg, TRUE);
  glBegin (GL_QUADS);
  glVertex2f (x, y);
  glVertex2f (x + width, y);
  glVertex2f (x +  width, y + height);
  glVertex2f (x, y + height);
  glEnd ();
}

/*----------------------------------------------------------------------
  GL_DrawSegments
  ----------------------------------------------------------------------*/
void GL_DrawSegments (ThotSegment *point, int npoints)
{
  int i;

  if (S_thick > 1.)
    {
      glBegin (GL_POINTS); 
      for (i = 0; i < npoints; i++)
        {
          glVertex2f ((point + i)->x1 , 
                      (point + i)->y1);
          glVertex2f ((point + i)->x2 , 
                      (point + i)->y2);
        }
      glEnd ();
    }
  glBegin (GL_LINES) ;
  for (i = 0; i < npoints; i++)
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
  mode = 0 (default mode), 1 (GLU_TESS_WINDING_ODD)
  ----------------------------------------------------------------------*/
void GL_DrawArc (float x, float y, float w, float h, int startAngle,
                 int sweepAngle, int mode, ThotBool filled)
{
  int       i, slices, npoints, j;
  GLfloat   angleOffset;
  GLfloat   sinCache[SLICES_SIZE];
  GLfloat   cosCache[SLICES_SIZE];
  GLfloat   cx, cy, angle, halfthick;
  GLfloat   wr, hr, rx, ry;
  ThotPoint points[SLICES_SIZE * 2 + 1];

  halfthick = S_thick / 2.;
  // set external ray
  rx  = ((GLfloat)w) / 2.;
  ry = ((GLfloat)h) / 2.;
  // set ellipse center
  cx  = ((GLfloat)x) + rx; 
  cy  = ((GLfloat)y) + ry;
  if (w < 10 && h < 10)
    slices = 36;
  else
    slices = SLICES;

  if (!filled && halfthick >= rx && halfthick >= ry)
    {
      // set external ray
      filled = TRUE;
      rx += halfthick;
      ry += halfthick;
    }

  startAngle = startAngle;
  sweepAngle = sweepAngle;
 
  /* Cache is the vertex locations cache */
  angleOffset = (GLfloat) (startAngle / 180.0 * M_PI);
  for (i = 0; i <= slices; i++) 
    {
      angle = angleOffset + (GLfloat) ((M_PI * sweepAngle) / 180.0) * i / ((GLfloat)slices);
      cosCache[i] = (GLfloat) DCOS(angle);
      sinCache[i] = (GLfloat) DSIN(angle);
    }

  if (fabs (sweepAngle - 360.0) < 0.0001) 
    {
      sinCache[slices] = sinCache[0];
      cosCache[slices] = cosCache[0];
    }

  if (filled)
    {
      for (i = 0; i <= slices; i++)
        {
          points[i].x = cx + (rx * cosCache[i]);
          points[i].y = cy - (ry * sinCache[i]);
        }
      points[i].x = points[0].x;
      points[i].y = points[0].y;

      glBegin (GL_TRIANGLE_FAN);
      /* The center */
      glVertex2d (cx, cy);
      for (i = 0; i <= slices; i++)
        glVertex2d (points[i].x, points[i].y);
      glEnd();
    }
  else
    {
      npoints = slices * 2 + 1;
      if (halfthick < 1.)
        halfthick = 1.;
      // set internal ray
      rx -= halfthick;
      ry -= halfthick;
      wr = rx + S_thick;
      hr = ry + S_thick;
      j = npoints;
      for (i = 0; i <= slices; i++)
        {
          // external arc
          points[i].x = cx + (rx * cosCache[i]);
          points[i].y = cy - (ry * sinCache[i]);
          // internal arc
          points[j].x = cx + (wr * cosCache[i]);
          points[j].y = cy - (hr * sinCache[i]);
          j--;
        }
      npoints++;
      points[npoints].x = points[slices+1].x;
      points[npoints].y = points[slices+1].y;
      // display a not convex polygon NONZERO mode
      MakefloatMesh (points, npoints, mode);
    }
}



/*----------------------------------------------------------------------
  GL_DrawLines
  npoints gives the number of points
  ----------------------------------------------------------------------*/
void GL_DrawLines (ThotPoint *points, int npoints)
{
  int i;

  if (S_thick > 1.)
    {
      /* Joinning if a certain thickness ?*/
      glBegin (GL_POINTS); 
      for (i = 0; i<npoints; i++)
        glVertex2dv ((double *) &points[i]);
      glEnd (); 
    }
  glBegin (GL_LINE_STRIP);
  for (i = 0; i < npoints; i++)
    glVertex2dv ((double *) &points[i]);
  glEnd ();
  
}

/*----------------------------------------------------------------------
  GL_DrawLine
  ----------------------------------------------------------------------*/
void GL_DrawLine (int x1, int y1, int x2, int y2, ThotBool round)
{
  if (S_thick > 1. && round)
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
  GL_DrawPolygon : tesselation handles 
  convex, concave and polygon with holes
  mode = 0 (default mode), 1 (GLU_TESS_WINDING_ODD)
  ----------------------------------------------------------------------*/
void GL_DrawPolygon (ThotPoint *points, int npoints, int mode)
{
  MakefloatMesh (points, npoints, mode);
}


/*----------------------------------------------------------------------
  GL_Point :
  Draw a point using GL primitives
  ----------------------------------------------------------------------*/
void GL_Point (int fg, float width, float x, float y)
{
  GL_SetForeground (fg, TRUE);
  glPointSize (width);
  glBegin (GL_POINTS);
  glVertex2f (x, y);
  glEnd ();
}


#ifdef _PIXELFONT
/*----------------------------------------------------------------------
  ResetPixelTransferBias
  ----------------------------------------------------------------------*/
static void ResetPixelTransferBias ()
{
  glPixelTransferf (GL_RED_BIAS, 0.0); 
  glPixelTransferf (GL_GREEN_BIAS, 0.0); 
  glPixelTransferf (GL_BLUE_BIAS, 0.0); 

}
#define BIT8DIVIDE(A) ((float)A /256)
/*----------------------------------------------------------------------
  SetPixelTransferBias
  ----------------------------------------------------------------------*/
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
  GL_DrawUnicodeChar : draw a character in a texture or a bitmap 
  ----------------------------------------------------------------------*/
void GL_DrawUnicodeChar (CHAR_T const c, float x, float y, 
                         void *GL_font, int fg)
{
  CHAR_T symbols[2];
  
  symbols[0] = c;
  symbols[1] = EOS;
  if (fg < 0 || GL_font == NULL)
    return;
  GL_SetForeground (fg, TRUE); 
  UnicodeFontRender (GL_font, symbols,  x, y, 1);
}

/*----------------------------------------------------------------------
  GL_DrawString: Draw a string in a texture or a bitmap 
  ----------------------------------------------------------------------*/
int GL_DrawString (int fg,  CHAR_T *str, float x, float y,  int hyphen,
                   void *GL_font, int end)
{
  int width;

  if (end <= 0 || fg < 0 || GL_font == NULL)
    return 0; 
  str[end] = EOS; 
  if (Printing)
    {      
      TransText = TRUE;
      GL_SetForeground (fg, TRUE); 
      width = UnicodeFontRender (GL_font, str, x, y, end);
      if (hyphen)
        /* draw the hyphen */
        GL_DrawUnicodeChar ((CHAR_T)'\255', x + width, y, GL_font, fg);
      width = 0;
      TransText = FALSE;
    }
  else
    {
      GL_SetForeground (fg, TRUE); 
      width = UnicodeFontRender (GL_font, str, x, y, end);
      if (hyphen)
        /* draw the hyphen */
        GL_DrawUnicodeChar ((CHAR_T)'\255', x + width, y, GL_font, fg);
    }
  return width;
}


/*----------------------------------------------------------------------
  GetBoxTransformed : only a translation
  ----------------------------------------------------------------------*/
ThotBool GetBoxTransformed (void *v_trans, int *x, int *y)
{
  PtrTransform Trans = (PtrTransform) v_trans;

  while (Trans)
    {
      switch (Trans->TransType)
        {
        case  PtElBoxTranslate:
          *x = (int)Trans->XScale;
          *y = (int)Trans->YScale;
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

/*----------------------------------------------------------------------
  IsDeformed : only a translation
  ----------------------------------------------------------------------*/
ThotBool IsDeformed (void *v_trans)
{
  PtrTransform Trans = (PtrTransform) v_trans;

  while (Trans)
    {
      switch (Trans->TransType)
        {
        case  PtElTranslate:
        case  PtElBoxTranslate:
        case  PtElAnimTranslate:	
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

/*----------------------------------------------------------------------
  IsBoxDeformed : only a translation
  ----------------------------------------------------------------------*/
ThotBool IsBoxDeformed (PtrBox box)
{
  PtrAbstractBox pAb;

  pAb = box->BxAbstractBox;  
  while (pAb)
    {
      if (pAb->AbElement)
        if (IsDeformed (pAb->AbElement->ElTransform))
          return TRUE;
      pAb = pAb->AbEnclosing;
    }
  return FALSE;
}

/*----------------------------------------------------------------------
  IsTransformed :
  ----------------------------------------------------------------------*/
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

/*----------------------------------------------------------------------
  IsBoxTransformed :
  ----------------------------------------------------------------------*/
ThotBool IsBoxTransformed (PtrBox box)
{
  PtrAbstractBox pAb;

  pAb = box->BxAbstractBox;
  while (pAb)
    {
      if (pAb->AbElement)
        if (IsTransformed (pAb->AbElement->ElTransform))
          return TRUE;
      pAb = pAb->AbEnclosing;
    }
  return FALSE;
}


#ifdef _GL
/*----------------------------------------------------------------------
  DisplayBoxTransformation :
  ----------------------------------------------------------------------*/
void DisplayBoxTransformation (void *v_trans, int x, int y)
{
  PtrTransform Trans = (PtrTransform) v_trans;

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
  if (x || y)
    glTranslatef (- ((float) x), - ((float) y), 0);
}

/*----------------------------------------------------------------------
  DisplayViewBoxTransformation
  ----------------------------------------------------------------------*/
static void DisplayViewBoxTransformation (PtrTransform Trans, int Width, int Height)
{
  float     x_trans, y_trans;
  double    x_scale, y_scale; 
  ThotBool  is_translated, is_scaled;

  /* How to clip ????
     if (is_x_clipped || is_y_clipped)
     @@@clip ((int) -x_trans, (int) -y_trans, Width, Height);
  */
  GetViewBoxTransformation (Trans, Width, Height, &x_trans, &y_trans,
                            &x_scale, &y_scale,
                            &is_translated, &is_scaled);
  if (is_translated)
    glTranslatef (x_trans, y_trans, (float) 0);
  if (is_scaled)
    glScaled (x_scale, y_scale, (double) 1);
}

/*----------------------------------------------------------------------
  ZoomedValue :
  ----------------------------------------------------------------------*/
static float ZoomedValue (float val, int zoom)
{
  float dist;

  if (zoom != 0)
    {
      dist = val + (val * zoom / 10);
      if (IS_ZERO(dist) && val > 0)
        dist = 1;
    }
  else
    dist = val;
  return dist;
}

#endif /* _GL */

/*----------------------------------------------------------------------
  DisplayTransformation :
  Modify the current transformation matrix
  this is a GL Matrix         this is SVG Matrix
  |a0  a4  a8   a12|        | a d f |
  |		   |        | b c d |
  |a1  a5  a9   a13|        | 0 0 1 |
  |		   |
  |a2  a6  a10  a14|
  |		   |
  |a3  a7  a11  a15|
  ----------------------------------------------------------------------*/
void DisplayTransformation (int frame, PtrTransform Trans, int Width, int Height)
{
#ifdef _GL
  double trans_matrix[16];
  double tx, ty;
    
  if (IsTransformed (Trans))
    {
      DisplayViewBoxTransformation (Trans, Width, Height);
      while (Trans)
        {
          switch (Trans->TransType)
            {
            case  PtElScale:
              glScalef (Trans->XScale, Trans->YScale, (GLfloat)1); 
              break;
            case PtElAnimTranslate:
            case PtElTranslate:
              tx = ZoomedValue (Trans->XScale,
                                ViewFrameTable[frame - 1].FrMagnification);
              ty = ZoomedValue (Trans->YScale,
                                ViewFrameTable[frame - 1].FrMagnification);
              glTranslatef (tx, ty, 0);
              break;
            case PtElAnimRotate:
            case PtElRotate:
              tx = ZoomedValue (Trans->XRotate,
                                ViewFrameTable[frame - 1].FrMagnification);
              ty = ZoomedValue (Trans->YRotate,
                                ViewFrameTable[frame - 1].FrMagnification);

              glTranslatef (tx, ty , 0);
              glRotatef (Trans->TrAngle, 0, 0, 1);
              glTranslatef (-tx, -ty, 0);
              break;
            case PtElMatrix:
              /* Matrix 
                 GlMatrix is 4*4
                 Svg is 3*3 but 
                 only 2*3 is specified */
              tx = ZoomedValue (Trans->EMatrix,
                                ViewFrameTable[frame - 1].FrMagnification);
              ty = ZoomedValue (Trans->FMatrix,
                                ViewFrameTable[frame - 1].FrMagnification);
              glTranslatef (tx, ty , 0);	      

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

              trans_matrix[12] = 0;
              trans_matrix[13] = 0;
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

              trans_matrix[4] = DTAN (DEG_TO_RAD(Trans->TrFactor));
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
              trans_matrix[1] = DTAN (DEG_TO_RAD(Trans->TrFactor));
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


/*----------------------------------------------------------------------
  print2DVertex: Write contents of one vertex to stdout
  ----------------------------------------------------------------------*/
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

/*----------------------------------------------------------------------
  printBuffer : Write contents of entire buffer.  (Parse tokens!)	
  Bounding box Debugging purpose (and print...)
  ----------------------------------------------------------------------*/
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
       
        case  GL_LINE_RESET_TOKEN:
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

/*----------------------------------------------------------------------
  computeisminmax : check if number is a new min or max
  ----------------------------------------------------------------------*/
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

/*----------------------------------------------------------------------
  getboundingbox : Get bound values of the box
  ----------------------------------------------------------------------*/
void getboundingbox (int size, float *buffer, int frame,
                     int *xorig, int *yorig, 
                     int *worig, int *horig)
{
  GLint  token, count, vertex_count;
  double x,y,w,h, TotalHeight;
  
  x = (double) *xorig;
  y = (double) *yorig;
  w = (double) *xorig + 1/**worig*/;
  h = (double) *yorig + 1/**horig*/;  
  TotalHeight = (double) FrameTable[frame].FrHeight;  
  count = size;
  while (count > 0) 
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
        case  GL_LINE_RESET_TOKEN:
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

          /* 	default: */
          /* 	  break; */
        }
    }
  *xorig = (int) x;
  *yorig = (int) y;
  *worig = (int) (w - x) + 1;
  *horig = (int) (h - y) + 1;
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




#define MAX_TIMEFUNC 50
static void (*TimeFunc[MAX_TIMEFUNC]) (Document doc, double current_time);
static int i_func_index = 0;

/*----------------------------------------------------------------------
  TtaRegisterTimeEven : Register function called on time change
  ----------------------------------------------------------------------*/
void TtaRegisterTimeEvent(void (*pfunc) (Document doc, double current_time))
{
  int i = 0;

  while (i < i_func_index)
    {
      if (TimeFunc[i_func_index] == pfunc)
        return;
      i++;
    }
  TimeFunc[i_func_index++] = pfunc;
}
/*----------------------------------------------------------------------
  GetCurrentTime : Get Current Time
  ----------------------------------------------------------------------*/
AnimTime ComputeThotCurrentTime (int frame)
{
#ifdef _WX
#ifdef _WINDOWS
  /* draw and calculate draw time bench that helps finding bottlenecks...*/
  struct timeb	after;
#else /* _WINDOWS */
  struct timeval tv;
  struct timezone tz;
#endif /* _WINDOWS */
#endif /* _WX */
  AnimTime current_time = 0; 
  int      i;

  if (FrameTable[frame].Anim_play) 
    {   
#ifdef _WX
#ifdef _WINDOWS
      ftime (&after);
      current_time = (double)after.time + (((double)after.millitm)/1000.);
#else /* _WINDOWS */
      gettimeofday (&tv, &tz);
      current_time = (double)tv.tv_sec + (((double)tv.tv_usec)/1000000.);
#endif /* _WINDOWS */
#endif /* _WX */
#ifdef _WINGUI
      current_time = ((double) GetTickCount ()) / 1000; 
#endif /*_WINGUI*/	

      if (FrameTable[frame].BeginTime < 0.0001)
        {
          FrameTable[frame].BeginTime = current_time;
          current_time += 0.001;
          FrameTable[frame].LastTime = 0.0;
        }
      current_time -= FrameTable[frame].BeginTime; 
      if (current_time - FrameTable[frame].LastTime < INTERVAL)
        current_time = -1;

      for (i = 0; i < i_func_index; i++)
        if (TimeFunc[i])
          (*TimeFunc[i]) (FrameTable[frame].FrDoc, current_time); 
    }
  else
    current_time = FrameTable[frame].LastTime; 
  return current_time;
}

/* flag used to display only once opengl status (version, renderer ...)*/
static ThotBool DisplayOpenGLStatus = FALSE;
/* Put back this boolean to TRUE to have the OpenGL Status at launching */
 
#define GLU_ERROR_MSG "\nSorry, Amaya requires GLU 1.2 or later.\n"
/*----------------------------------------------------------------------
  SetGlPipelineState : Detect Opengl, Software, Hardware, 
  Set The Openlg State machine to the fastest state possible for drawing 2d.
  ----------------------------------------------------------------------*/
void SetGlPipelineState ()
{
  const char *version = (const char *) gluGetString (GLU_VERSION);
  const char *renderer = (const char*) glGetString (GL_RENDERER);
  int auxnumBuffers, acred, acgreen, acblue, acalpha;
  ThotBool graph_aa = TRUE;
  ThotBool badbuffer = FALSE;
  
  SetSoftware_Mode( FALSE );
  if ( renderer &&
       ( strstr (renderer, "Mesa") ||
         strstr (renderer, "Microsoft") ||
         strstr (renderer, "Sgi")) )
    if ( !strstr (renderer, "Mesa DRI") )
      SetSoftware_Mode( TRUE );
  
  /* Test if GLU version is >= 1.2 
   * 1.2 is needed for tesselate functions : gluNewTess, gluTessCallback ...*/
  if ( version &&
       ( strstr (version, "1.0") ||
         strstr (version, "1.1")) ) 
    {
#ifdef _WINGUI
      WinErrorBox (NULL,  GLU_ERROR_MSG);
#endif /*  _WINGUI */
#ifdef _GTK
      fprintf( stderr, GLU_ERROR_MSG);
#endif /* #if defined(_GTK) */
#ifdef _WX
      DisplayConfirmMessage ( GLU_ERROR_MSG );
#endif /* _WX */
      exit (1);
    }
  
  if (!Software_Mode)
    {
      /* default value is true because I've seen performance
       * improvments on windows when badbuffer is true */
      TtaSetEnvBoolean("ENABLE_BAD_BUFFER", TRUE, FALSE);
      TtaGetEnvBoolean ("ENABLE_BAD_BUFFER", &badbuffer);
      SetBadCard (!badbuffer);
    }
  
  if ( DisplayOpenGLStatus )
    {
      /* Print opengl status : usefull for debug on multiple plateformes */
      printf ("\nOpenGL Status:");
      printf ("\n  Software Mode = %s", Software_Mode ? "Soft" : "Hard" );
      /* Display Opengl Vendor Name,  Opengl Version, Opengl Renderer*/
      printf ("\n  VENDOR : %s\n  VERSION : %s\n  RENDERER : %s", 
              (char *)glGetString(GL_VENDOR), 
              (char *)glGetString(GL_VERSION), 
              (char *)glGetString(GL_RENDERER));
      /* g_print( "%s\n", (char *)glGetString(GL_EXTENSIONS));  */
      printf ("\n  GLU Version : %s", 
              (char *)gluGetString (GLU_VERSION));
      glGetIntegerv (GL_AUX_BUFFERS,      (GLint*) &auxnumBuffers);
      glGetIntegerv (GL_ACCUM_RED_BITS,   (GLint*) &acred);
      glGetIntegerv (GL_ACCUM_GREEN_BITS, (GLint*) &acgreen); 
      glGetIntegerv (GL_ACCUM_BLUE_BITS,  (GLint*) &acblue); 
      glGetIntegerv (GL_ACCUM_ALPHA_BITS, (GLint*)&acalpha);
      printf ("\n  Aux buffers count %d", auxnumBuffers);
      printf ("\n  Acumm rgba : %d %d %d %d", acred, acgreen, acblue, acalpha);
      printf ("\n");
      DisplayOpenGLStatus = FALSE;
    }

  /*  glClearColor (1, 0, 0, 0); */
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
  TtaGetEnvBoolean ("ENABLE_GRAPH_ANTI_ALIASING", &graph_aa);
  if (graph_aa)
    {
      glEnable (GL_LINE_SMOOTH); 
      glHint (GL_LINE_SMOOTH_HINT,  
              GL_NICEST);  

      glEnable (GL_POINT_SMOOTH); 
      glHint (GL_POINT_SMOOTH_HINT, 
              GL_NICEST);
    }
  /* Fastest Texture Mapping*/
  glHint (GL_PERSPECTIVE_CORRECTION_HINT, 
          GL_NICEST );    

  /* Bitmap font Text writing (even in texture font) */
  glPixelStorei( GL_UNPACK_LSB_FIRST, GL_FALSE);
  glPixelStorei( GL_UNPACK_ROW_LENGTH, 0);
  glPixelStorei( GL_UNPACK_ALIGNMENT, 1);

  /* Needs to clear buffer after allocating it before drawing */
  glDisable (GL_SCISSOR_TEST);
  glClear (GL_COLOR_BUFFER_BIT);
  glEnable (GL_SCISSOR_TEST);  
  glShadeModel (GL_FLAT);

  /* Not recommended for hardware cards... 
     Global Antialiasing is done elsewhere...*/
  /* glEnable (GL_POLYGON_SMOOTH);    */
  /* glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST); */ 

  /* smooth polygon antialiasing */
  /* glBlendFunc (GL_SRC_ALPHA_SATURATE, GL_ONE); */
    
  /* Doesn't compute hidden drawing 
     Doesn't work for our tesselated
     polygons   not CGW oriented...
     50 % performance on geometry drawing...*/
  /* glEnable (GL_CULL_FACE); */
  /* glCullFace (GL_FRONT_AND_BACK.,GL_BACK, GL_FRONT); */
  
  /* For transparency and beautiful antialiasing*/
  glEnable (GL_BLEND); 
  glBlendFunc (GL_SRC_ALPHA, 
               GL_ONE_MINUS_SRC_ALPHA); 
  GL_SetOpacity (1000);
#ifdef _WX
  if (GL_Err())
    wxPrintf( _T("OpenGL: Bad INIT\n") ); 
#endif /* _WX */
}


/*----------------------------------------------------------------------
  GL_window_copy_area : 
  Soft : We copy region content of the back buffer on the exposed region 
  (=> opengl region buffer swapping )
  ----------------------------------------------------------------------*/
void GL_window_copy_area (int frame, int xf, int yf, int x_source, int y_source,
                          int width, int height)
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
      if (!Software_Mode)
	{
	  DefClip (frame, -1, -1, -1, -1);
          return;
        }

      /* Copy from backbuffer to backbuffer */
      glFlush ();
      /* glFinish (); */
      glDisable (GL_BLEND);

      GL_UnsetClipping  (/*0, 0, 0, 0*/);

      glRasterPos2i (xf, yf+height);	  
      /*IF Rasterpos is outside canvas...
        we must use a decaling 'feinte'*/
      if ((yf+height) == FrameTable[frame].FrHeight)
        {
          glRasterPos2i (xf, yf+height-1);
          glBitmap(0, 0, 0, 0, 0, -1, NULL);
        }
      glReadBuffer (GL_BACK);
      glCopyPixels (x_source, y_source, width, height, GL_COLOR); 
      glEnable (GL_BLEND);
      /*copy from back to front */
      GL_realize (frame);	  
    }
}


/*----------------------------------------------------------------------
  GLResize: 
  remake the current coordonate system upon resize
  ----------------------------------------------------------------------*/
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
#ifdef _GL_COLOR_DEBUG
  {
    float tmp[4];
    glGetFloatv( GL_COLOR_CLEAR_VALUE, tmp );
    TTALOGDEBUG_4( TTA_LOG_DRAW, _T("glClear CLEAR_VALUE(%f,%f,%f,%f)"),tmp[0],tmp[1],tmp[2],tmp[3] );
  }
#endif /* _GL_COLOR_DEBUG */  
  glDisable (GL_SCISSOR_TEST);
  glClear (GL_COLOR_BUFFER_BIT);
  glEnable (GL_SCISSOR_TEST);
}

/*----------------------------------------------------------------------
  glhard: if a 3d card is involved.
  ----------------------------------------------------------------------*/
ThotBool glhard()
{
#ifdef _MACOS
  return FALSE;
#else /* _MACOS */
  return (!Software_Mode);
#endif /* _MACOS */
}

/*----------------------------------------------------------------------
  SaveBuffer:
  Take a picture (png) of the backbuffer.
  ----------------------------------------------------------------------*/
void saveBuffer (char *filename, int width, int height)
{
  unsigned char *Data;

  glFlush ();
  /* glFinish (); */
  Data = (unsigned char *)TtaGetMemory (sizeof (unsigned char) * width * height * 4);
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
/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
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
    for (j = 0; j < names; j++)
      {
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

/*----------------------------------------------------------------------
  startPicking: Selection Mode Activation and then Object Clicked
  is return, and "normal" (projection mode) is setted back.
  ----------------------------------------------------------------------*/
void PickObject (int frame, int x, int y) 
{
  GLint viewport[4];
  GLuint selectBuf[8144];

  glSelectBuffer (BUFSIZE, selectBuf);
  glGetIntegerv (GL_VIEWPORT, viewport);
  glRenderMode (GL_SELECT);

  /*For name's stack (integers)*/
  glInitNames ();

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
    processHits2 (hits, selectBuf, 0);
  glMatrixMode (GL_MODELVIEW);
  mode = RENDER;
}
#endif/*  _PICK */



#ifdef _PCLDEBUG_GARBAGE

/*****************************************************/
/* TESTING NOT REALLY USED (FOR DEBUGGING)*/

/* Drawing grid (for canvas geometry precision tests)*/
/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void DrawGrid (int width, int height)
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
/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
int make_carre ()
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

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
int savetga (const char *filename, unsigned char *Data,
             unsigned int width, unsigned int height)
{
  FILE *screenFile;
  int length;

  unsigned char cGarbage = 0, type,mode,aux, pixelDepth;
  short int iGarbage = 0;
  int i;

  length = width * height * 4;
  screenFile = TtaWriteOpen ("screenshot.tga");
  pixelDepth = 16;
  /* compute image type: 2 for RGB(A), 3 for greyscale*/
  mode = pixelDepth / 8;
  if ((pixelDepth == 24) || (pixelDepth == 32))
    type = 2;
  else
    type = 3;
  /* convert the image data from RGB(a) to BGR(A)*/
  if (mode >= 3)
    for (i=0; i < width * height * mode ; i+= mode)
      {
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
  TtaWriteClose (screenFile);
  return 0;
}

#ifdef NOtENOUGHTIMEPRECIsION
/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
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

