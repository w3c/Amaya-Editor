/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2003
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Printing OpenGL Handling
 * Authors:  P. Cheyrou-lagreze (INRIA) - Opengl Version
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

#include "glwindowdisplay.h"


#include <string.h>
#include <sys/types.h>
#include <stdarg.h>
#include <time.h>


#ifdef _GTK

#include <gtk/gtk.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <gdk/gdk.h>
#include <gtkgl/gdkgl.h>

static GdkGLContext *context = NULL;
static GdkPixmap    *pixmap = NULL;
static GdkGLPixmap  *glpixmap = NULL;
static GdkVisual    *visual = NULL;

#endif /* _GTK */

#include "GL/gl.h"

#include "ustring.h"
#include "math.h"
#include "thot_sys.h"
#include "message.h"
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"
#include "picture.h"


#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "boxes_tv.h"
#include "font_tv.h"
#include "frame_tv.h"
#include "units_tv.h"
#include "edit_tv.h"
#include "thotcolor_tv.h"

#include "glwindowdisplay.h"
#include "glprint.h"


#include "buildlines_f.h"
#include "font_f.h"
#ifndef _WINDOWS
#include "initpses_f.h"
#endif /*_WINDOWS*/
#include "memory_f.h"
#include "units_f.h"


#define FEEDBUFFERSIZE 32768
static  GLfloat      feedBuffer[FEEDBUFFERSIZE];

/*if not in feedback mode*/
static ThotBool      NotFeedBackMode = TRUE;
/*if just computing bounding box*/
static ThotBool      CompBoundingBox = FALSE;

/*Control When swapping applies*/
static ThotBool      SwapOK[MAX_FRAME];

static GLrgb         LastRgb;
static GLfloat       LastLineWidth;

/* The GL context. GL is not thread safe (we should create a
   local GLcontext during GLBeginPage) */
GLcontext *GL = NULL;
 
/*#define FILE_STREAM GL->stream;*/
#define FILE_STREAM ((FILE*) FrRef[1])


#define GL_HEIGHT 2048
#define GL_WIDTH 2048

/*----------------------------------------------------------------------
  GetGLContext : Main program : init an offscreen rendering context 
  in order to use OpenGL drawing results  as a base for printing                                      
  ----------------------------------------------------------------------*/
void GetGLContext ()
{
  int i;
#ifdef _GTK
  /*       Parameters of the opengl Buffers
	   More tweaks we have the less memory we use !!
	   => ie depth, stencil, shadow...
	   double buffering or not...
  */  
  int attrlist[] =
    {
      GDK_GL_RGBA,
      GDK_GL_RED_SIZE,1,
      GDK_GL_GREEN_SIZE,1,
      GDK_GL_BLUE_SIZE,1,
      GDK_GL_ALPHA_SIZE,1,
      GDK_GL_STENCIL_SIZE, 1,
      GDK_GL_NONE
    }; 

  /* Is opengl working ? */
  if(gdk_gl_query() == FALSE) 
    {
      g_print("OpenGL not supported!\n");
      exit(0);
    }

  /* select and use visual as default so all widgets are OpenGL renderable */
  visual = gdk_gl_choose_visual (attrlist);
  if (visual == NULL)
    {
      g_print("Error creating GtkGLArea!\n");
      exit(0);
    }
  gtk_widget_set_default_colormap(gdk_colormap_new(visual, TRUE));
  gtk_widget_set_default_visual(visual);

  /* FrameTable[1].WdFrame = visual;  */

  context  = gdk_gl_context_new (visual);
  pixmap = gdk_pixmap_new (NULL, GL_HEIGHT, GL_WIDTH, visual->depth);
  glpixmap = gdk_gl_pixmap_new (visual, pixmap);
  gdk_gl_pixmap_make_current (glpixmap, context);


#endif /* _GTK */

  SetGlPipelineState ();
  GLResize (GL_HEIGHT, GL_WIDTH,0,0);
  glScissor (0, 0, GL_HEIGHT, GL_WIDTH);
  GL_Err();

  for (i = 0; i < 4; i++)
    LastRgb[i] = -1.;

  LastLineWidth = -1.;

 /*  GL_SetTransText (TRUE); */
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

/*---------------------------------------------------
  InitPrintBox :  	     
 ----------------------------------------------------*/
void InitPrintBox ()
{
  if (NotFeedBackMode && !CompBoundingBox)
    {
      NotFeedBackMode = FALSE;  
      glFeedbackBuffer (FEEDBUFFERSIZE, GL_3D_COLOR, feedBuffer);
      glRenderMode (GL_FEEDBACK);
    }
}
/*---------------------------------------------------
  ClosePrintBox :  	     
  ----------------------------------------------------*/
void FinishPrintBox ()
{  
  if (NotFeedBackMode == FALSE && !CompBoundingBox)
    {  
      GLParseFeedbackBuffer (feedBuffer); 
      NotFeedBackMode = TRUE;
    }
}
/*---------------------------------------------------
  PrintBox :  	     
  ----------------------------------------------------*/
void PrintBox (PtrBox box, int frame, 
	       int xmin, int xmax, 
	       int ymin, int ymax)
{
  CompBoundingBox = TRUE;
  glFeedbackBuffer (FEEDBUFFERSIZE, GL_3D_COLOR, feedBuffer);
  NotFeedBackMode = FALSE;  
  glRenderMode (GL_FEEDBACK);
  DisplayBox (box, frame, xmin, xmax, ymin, ymax);
  NotFeedBackMode = TRUE;
  GLParseFeedbackBuffer (feedBuffer);
  NotFeedBackMode = TRUE;
  CompBoundingBox = FALSE;
}

/*---------------------------------------------------
  ComputeBoundingBox :
  Modify Bounding Box according to opengl feedback mechanism
  (after transformation, coordinates may have changed)			    
  ----------------------------------------------------*/
void ComputeBoundingBox (PtrBox box, int frame, 
			 int xmin, int xmax, 
			 int ymin, int ymax)
{
  
  return;

}

/*---------------------------------------------------
  ComputeFilledBox :
  Modify Bounding Box according to opengl feedback mechanism
  (after transformation, coordinates may have changed)			    
  ----------------------------------------------------*/
void ComputeFilledBox (PtrBox box, int frame, int xmin, int xmax, int ymin, int ymax)
{
  
  return;

}

/*----------------------------------------------------------------------
  GL_prepare: If a modif has been done
  ----------------------------------------------------------------------*/
ThotBool GL_prepare (int frame)
{  
#ifdef _GTK
  if (gdk_gl_pixmap_make_current (glpixmap, context) != TRUE)
    return FALSE;
#endif /* _GTK */
  return TRUE;
}

/* system */


#define GLSameColor(rgb1, rgb2) (!(rgb1[0] != rgb2[0] || \
                                                    rgb1[1] != rgb2[1] || \
						    rgb1[2] != rgb2[2]))


/* PostScript writings. */

static void GLWriteByte (FILE *stream, unsigned char byte)
{
  unsigned char h = byte / 16;
  unsigned char l = byte % 16;

  fprintf (stream, "%x%x", h, l);
}



void GLPrintPostScriptColor(void *v_rgb)
{
  GLrgb rgb;

 rgb[0] = *((float *) v_rgb);
 rgb[1] = *((float *) v_rgb + 1);
 rgb[2] = *((float *) v_rgb + 2);

  if (!GLSameColor(LastRgb, rgb))
    {
      LastRgb[0] = rgb[0];
      LastRgb[1] = rgb[1];
      LastRgb[2] = rgb[2];

      if (rgb[0] == 0 && rgb[1] == 0 && rgb[2] == 0)
	fprintf (FILE_STREAM, "0 G\n");
      else
	fprintf (FILE_STREAM, "%g %g %g C\n", rgb[0], rgb[1], rgb[2]);
    }
}




/****************
End POSTCRIPT
*****************/

/* The feedback buffer parser */

static GLint GLGetVertex(GLvertex *v, GLfloat *p)
{
  v->xyz[0] = p[0];
  v->xyz[1] = p[1];
  v->xyz[2] = 0;
  v->rgb[0] = p[3];
  v->rgb[1] = p[4];
  v->rgb[2] = p[5];
  v->rgb[3] = p[6]; 
  return 7;
}

GLint GLParseFeedbackBuffer (GLfloat *current)
{
  char flag, dash = 0;
  GLint i, used, count, v, vtot;
  GLfloat lwidth = 1., psize = 1.;
  GLvertex vertices[3];

  used = glRenderMode (GL_RENDER);
  if (used > 0)
    {
      while (used > 0)
	{
	  switch ((GLint)*current)
	    {
	    case GL_POINT_TOKEN :
	      current ++;
	      used --;
	      i = GLGetVertex (&vertices[0], current);
	      current += i;
	      used    -= i;

	      GLPrintPostScriptColor (vertices[0].rgb);
	      fprintf (FILE_STREAM, "%g -%g %g P\n", 
		       vertices[0].xyz[0], GL_HEIGHT - vertices[0].xyz[1], 0.5*psize);

	      break;

	    case GL_LINE_TOKEN :
	    case GL_LINE_RESET_TOKEN :
	      current ++;
	      used --;
	      i = GLGetVertex (&vertices[0], current);
	      current += i;
	      used    -= i;
	      i = GLGetVertex (&vertices[1], current);
	      current += i;
	      used    -= i;

	      if (LastLineWidth != lwidth)
		{
		  LastLineWidth = lwidth;
		  fprintf (FILE_STREAM, "%g W\n", LastLineWidth);
		}
	      if (dash)
		{
		  fprintf (FILE_STREAM, "[%d] 0 setdash\n", dash);
		}
	      GLPrintPostScriptColor (vertices[0].rgb);
	      fprintf (FILE_STREAM, "%g -%g %g -%g L\n",
		       vertices[1].xyz[0], GL_HEIGHT - vertices[1].xyz[1],
		       vertices[0].xyz[0], GL_HEIGHT - vertices[0].xyz[1]);   
	      if (dash)
		{
		  fprintf (FILE_STREAM, "[] 0 setdash\n");
		}
	      break;

	    case GL_POLYGON_TOKEN :
	      count = (GLint)current[1];
	      current += 2;
	      used -= 2;
	      v = vtot = 0;
	      while (count > 0 && used > 0)
		{
		  i = GLGetVertex (&vertices[v], current);
		  current += i;
		  used    -= i;
		  count --;
		  vtot++;
		  if (v == 2)
		    {
                      flag = 0;
		      GLPrintPostScriptColor (vertices[0].rgb);
		      fprintf (FILE_STREAM, "%g -%g %g -%g %g -%g T\n",
			       vertices[2].xyz[0], GL_HEIGHT - vertices[2].xyz[1],
			       vertices[1].xyz[0], GL_HEIGHT - vertices[1].xyz[1],
			       vertices[0].xyz[0], GL_HEIGHT - vertices[0].xyz[1]);
		      vertices[1] = vertices[2];
		    }
		  else
		    v ++;
		}
	      break;      

	    case GL_BITMAP_TOKEN :
	    case GL_DRAW_PIXEL_TOKEN :
	    case GL_COPY_PIXEL_TOKEN :
	     /*  current ++; */
/* 	      used --; */
/* 	      i = GLGetVertex (&vertices[0], current); */
/* 	      current += i; */
/* 	      used    -= i; */
	      break;    
  
	    case GL_PASS_THROUGH_TOKEN :
	      switch ((GLint) current[1])
		{
		case GL_BEGIN_LINE_STIPPLE : 
		  dash = 4; break;
		case GL_END_LINE_STIPPLE : 
		  dash = 0; break;
		case GL_SET_POINT_SIZE : 
		  current += 2; 
		  used -= 2; 
		  psize = current[1];
		  break;

		case GL_SET_LINE_WIDTH : 
		  current += 2; 
		  used -= 2; 
		  lwidth = current[1];
		  break;
		}
	      current += 2; 
	      used -= 2; 
	      break;      

	    default :
	      /*unknown*/
	      current ++;
	      used --;
	      break;
	    }
	}
    }
  return 1;
}


GLint GLText (const char *str,
	      const int fg,
	      const void *font,
	      const unsigned int fontsize, 
	      const int x, 
	      const int y,
	      const int length)
{
  char fontname[35];
  int i, width;

  width = 0;
  GL_SetPrintForeground (fg); 
  GetPostscriptNameFromFont ((PtrFont) font, fontname);
  fprintf (FILE_STREAM, fontname);
  fprintf (FILE_STREAM, "(");
  for (i = 0; i < length; i++)
    {
      fprintf (FILE_STREAM, "%c", str[i]); 
      width += CharacterWidth (42, (PtrFont) font);     
    }
  fprintf (FILE_STREAM, ") %d %d %d s\n", 
	   width, x, -y);

  /* fprintf (FILE_STREAM, "(%s) %g %g %d /%s S\n", */
  /* 	   str,  */
  /* 	   x, y, */
  /* 	   fontsize,  */
  /* 	   fontname); */

  return width;
}


/*----------------------------------------------------------------------
   Transcode emit the PostScript code for the given char.
  ----------------------------------------------------------------------*/
static void Transcode (FILE *fout, int encoding, unsigned char car)
{
  switch (car)
    {
    case '(':
      fprintf (fout, "\\(");
      break;
    case ')':
      fprintf (fout, "\\)");
      break;
    case '*':
      if (encoding == 0)
	fprintf (fout, "*");
      else
	fprintf (fout, "\\267");
      break;
    case '\\':
      fprintf (fout, "\\\\");
      break;
    case START_ENTITY:
      fprintf (fout, "&");
      break;
    default:
      if (car >= ' ' && car <= '~')
	fprintf (fout, "%c", car);
      else
	fprintf (fout, "\\%o", car);
    }
}

int GLString (unsigned char *buff, int lg, int frame, int x, int y,
		PtrFont font, int boxWidth, int bl, int hyphen,
		int startABlock, int fg, int shadow)
{
  FILE               *fout;
  int                 j, i, encoding, width;
  int                 noJustifiedWhiteSp;
  int                 X = 0,Y = 0;
  int                 NbWhiteSp = 0;
  char                fontname[35];
  static  int         SameBox = 0;

  fout = (FILE *) FILE_STREAM;
  encoding = 0;
  if (y < 0)
    return 0;
  width = 0;
  if (lg > 0)
    {
      /* noJustifiedWhiteSp is > 0 if writing a fixed lenght is needed */
      /* and equal to 0 if a justified space is to be printed */  
      noJustifiedWhiteSp = startABlock;
      /* Is this a new box ? */
      if (SameBox == 0)
      {
	/* store the start position for the justified box */
	SameBox = 1;
	X = x;
	Y = y;
	NbWhiteSp = 0;
	if (fg >= 0)
	  {
	    /* Do we need to change the current color ? */
	    GL_SetPrintForeground (fg); 
	    /* Do we need to change the current font ? */
	    GetPostscriptNameFromFont (font, fontname);
	    fprintf (fout, fontname);
	    fprintf (fout, "(");
	  }
      }
      
      if (shadow)
	{
	  /* replace each character by a star */
	  j = 0;
	  while (j < lg)
	    {
	      buff[j++] = '*';
	      width += CharacterWidth (42, font);
	    }
	  buff[lg] = EOS;
	  bl = 0;
	}
      else
	{
	  buff[lg] = EOS;
	  /* Add the justified white space */
	  if (bl > 0)
	    {
	      NbWhiteSp += bl;
	      if (fg >= 0)
		{
		  for (i = 1; i <= bl; i++)
		    fprintf (fout, "%c", ' ');
		  /* Transcode (fout, encoding, ' '); */
		}
	    }
	  /* Emit the chars */
	  for (j = 0; j < lg; j++)
	    {
	      /* compute the width of the string */
	      width += CharacterWidth (buff[j], font);
	      /* enumerate the white spaces */
	      if (buff[j] == ' ')
		{
		  if (noJustifiedWhiteSp == 0)
		    {
		      /* write a justified white space */
		      NbWhiteSp++;
		      if (fg >= 0)
			fputs (" ", fout);
		    }
		  else if (fg >= 0)
		    /* write a fixed lenght white space */
		    fputs ("\\240", fout);
		}
	      else
		{
		  noJustifiedWhiteSp = 0;
		  if (fg >= 0)
		    Transcode (fout, encoding, buff[j]);
		}
	    }
	}
    }
  else if (bl > 0)
    {
      /* store previous spaces */
      NbWhiteSp += bl;
      if (fg >= 0)
	{
	  for (i = 1; i <= bl; i++)
	    fprintf (fout, "%c", ' ');
	}
    }
   
  /* Is an hyphen needed ? */
  if (hyphen && fg >= 0)
    fprintf (fout, "%c", '\255');
  /* is this the end of the box */
  if (boxWidth != 0 && SameBox == 1 )
    {
      /* now let Postscript justify the text with the right width */
      if (boxWidth < 0)
	boxWidth = - boxWidth + width;
      else if (boxWidth < width)
	/* not enough space to display the last piece of text */
	boxWidth = width;
      /* Is justification needed ? */
      if (fg >= 0)
	{
	  if (NbWhiteSp == 0)
	    fprintf (fout, ") %d %d %d s\n", boxWidth, X, -Y);
	  else
	    fprintf (fout, ") %d %d %d %d j\n", NbWhiteSp, boxWidth, X, -Y);
	}
      SameBox = 0; 
      NbWhiteSp = 0;
    }
  return (width);
}





GLint GLDrawPixelsPoscript (GLsizei width, GLsizei height,
			    GLint xorig, GLint yorig,
			    GLenum format, GLenum type, 
			    unsigned char *pixels, 
			    GLfloat x, GLfloat y)
{
  int row, col, colwidth;
  FILE *stream;
  int Mode;
  unsigned char *current;

  if((width <= 0) || (height <= 0))
    return 0;
  stream = FILE_STREAM;
  x = x + xorig;
  y = y + yorig; 
  Mode = (format == GL_RGBA)?1:0;
  fprintf (stream, "gsave\n");
  fprintf (stream, "%.2f %.2f translate\n", x, -(y+height)); 
  fprintf (stream, "%d %d scale\n", width, height);
  /* 8 bit for r and g and b */
  fprintf (stream, "/rgbstr %d string def\n", width * 3); 
  fprintf (stream, "%d %d %d\n", width, height, 8);
  fprintf (stream, "[ %d 0 0 -%d 0 %d ]\n", width, height, height); 
  fprintf (stream, "{ currentfile rgbstr readhexstring pop }\n");
  fprintf (stream, "false 3\n");
  fprintf (stream, "colorimage\n");

  colwidth = width*((Mode)?4:3);
  current = pixels + (height-1)*colwidth;
  colwidth += colwidth;
  /* inverts pixels upside-down*/
  for (row = 0; row < height; row++)
    {
      col = 0;
      while (col < width)
	{
	  GLWriteByte (stream, *current);/*R*/
	  current ++;
	  GLWriteByte (stream, *current);/*G*/
	  current ++;
	  GLWriteByte (stream, *current);/*B*/
	  current ++;
	  if (Mode)
	    current ++;
	  col ++;
	}
      current -= colwidth;      
      fprintf (stream, "\n");
    }

  fprintf (stream, "grestore\n");
  return 1;
}

GLint GLEnable(GLint mode)
{
  switch(mode)
    {
    case GLPRINT_LINE_STIPPLE :
      glPassThrough(GL_BEGIN_LINE_STIPPLE);
      break;
    default :
      break;
    }

  return 1;
}

GLint GLDisable(GLint mode)
{ 
  switch(mode)
    {
    case GLPRINT_LINE_STIPPLE :
      glPassThrough(GL_END_LINE_STIPPLE);
      break;
    default :
      break;
    }

  return 1;
}

GLint GLPointSize(GLfloat value)
{
  if (!GL) 
    return 0;

  glPassThrough(GL_SET_POINT_SIZE);
  glPassThrough(value);
  
  return 1;
}

GLint GLLineWidth(GLfloat value)
{
  if (!GL) 
    return 0;

  glPassThrough(GL_SET_LINE_WIDTH);
  glPassThrough(value);

  return 1;
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
      glFinish ();
      glFlush ();   
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
 
}
#endif /*_WINDOWS*/
/*----------------------------------------------------------------------
  SetBadCard :  handle video cards that flush backbuffer after each
  buffer swap
  ----------------------------------------------------------------------*/
void SetBadCard (ThotBool badbuffer)
{
}
#endif /* _GL */
