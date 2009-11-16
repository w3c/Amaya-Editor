/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Printing OpenGL Handling
 * Authors:  P. Cheyrou-lagreze (INRIA) - Opengl Version
 */
#ifdef _GL

#ifdef _WX
  #include "wx/wx.h"
  #include "wx/glcanvas.h"
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
#include "windowdisplay_f.h"
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
#endif /* _GTK */

#ifdef _WX
  #include "AmayaApp.h"
  #include "AmayaPrintNotify.h"
  static wxGLCanvas * wx_canvas = NULL;
  extern AmayaPrintNotify * g_p_print_dialog;

#endif /* _WX */

#if defined (_MACOS) && defined (_WX)
#include <gl.h>
#else /* _MACOS */
#include <GL/gl.h>
#endif /* _MACOS */

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
#ifdef _GTK
#include "initpses_f.h"
#endif /* _GTK */
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
static int            X, Y;

#ifdef _WX
/* TODO : rendre le code plus propre car FrRef n'a rien a voir avec un file descriptor !!! */
/* TODO : FrRef est definit dans print.c */
extern ThotWindow  FrRef[MAX_FRAME + 2];
#endif /* _WX */

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
  int           i;
#ifdef _GTK
  GdkVisual    *visual = NULL;
  /*       Parameters of the opengl Buffers
	   More tweaks we have the less memory we use !!
	   => ie depth, stencil, shadow...
	   double buffering or not...
  */  
  int attrlist[] =
    {
      GDK_GL_RGBA,
      GDK_GL_RED_SIZE, 1,
      GDK_GL_GREEN_SIZE, 1,
      GDK_GL_BLUE_SIZE, 1,
      GDK_GL_ALPHA_SIZE, 1, /* don't change the position of the entry (8) */
      GDK_GL_STENCIL_SIZE, 1,
      GDK_GL_NONE
    };

  /* Is opengl working ? */
  if (gdk_gl_query() == FALSE) 
    {
      g_print("OpenGL not supported!\n");
      exit(0);
    }
  /* select and use visual as default so all widgets are OpenGL renderable */
  visual = gdk_gl_choose_visual (attrlist);
  if (visual == NULL)
    {
      /* remove the alpha channel: entry  (8)*/
      attrlist[8] = 0;
      visual = gdk_gl_choose_visual (attrlist);
    }  
  if (visual == NULL)
    {
      g_print("Error creating GtkGLArea!\n");
      exit(0);
    }
  gtk_widget_set_default_colormap (gdk_colormap_new (visual, TRUE));
  gtk_widget_set_default_visual (visual);
  /* FrameTable[1].WdFrame = visual;  */
  context  = gdk_gl_context_new (visual);
  pixmap = gdk_pixmap_new (NULL, GL_HEIGHT, GL_WIDTH, visual->depth);
  glpixmap = gdk_gl_pixmap_new (visual, pixmap);
  gdk_gl_pixmap_make_current (glpixmap, context);
#endif /* _GTK */

#ifdef _WX
  wx_canvas = new wxGLCanvas( g_p_print_dialog, -1,
			      wxDefaultPosition, wxDefaultSize,
			      0, _T("AmayaCanvas"),
			      AmayaApp::GetGL_AttrList() );
  wx_canvas->SetCurrent();
#endif /* _WX */

  SetGlPipelineState ();
  GLResize (GL_HEIGHT, GL_WIDTH, 0, 0);
  glScissor (0, 0, GL_HEIGHT, GL_WIDTH);
  GL_Err();

  for (i = 0; i < 4; i++)
    LastRgb[i] = -1.;
  LastLineWidth = -1.;
}

#ifdef _WINGUI
/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void initwgl (HDC hDC, int frame)
{
  HGLRC hGLRC = 0;
  hGLRC = wglCreateContext (hDC);
  GL_Windows[frame] = hDC;
  GL_Context[frame] = hGLRC;
  if (wglMakeCurrent (hDC, hGLRC))
    {
      SetGlPipelineState ();
      GLResize (GL_HEIGHT, GL_WIDTH,0,0);
	  glEnable (GL_SCISSOR_TEST);
      glScissor (0, 0, GL_HEIGHT, GL_WIDTH);
      GL_Err();
#ifdef _TEST
      /*permet de verifier la validite du contexte opengl. 
	size doit etre >0, sinon cela signifie qu'opengl n'arrive pas a dessiner !!*/
      {
	int size = 0;
	/*char feedbuffer[16384];*/
	
	glFeedbackBuffer (FEEDBUFFERSIZE, GL_3D_COLOR, feedBuffer);
	glRenderMode (GL_FEEDBACK);
	glBegin (GL_QUADS);
	glVertex2i (1, 1);
	glVertex2i (4, 400);
	glVertex2i (400, 400);
	glVertex2i (400, 1);
	glEnd ();
	size = glRenderMode (GL_RENDER);
	if (size > 0)
	  {
	    if (size > 1)
	      size = 1;
	  }
      }
#endif /*_TEST*/
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void closewgl (HDC hDC, int frame)
{
  wglDeleteContext (GL_Context[frame]);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void SetupPixelFormatPrintGL (HDC hDC, int frame)
{
  PIXELFORMATDESCRIPTOR pfd = 
    {
      sizeof(PIXELFORMATDESCRIPTOR),  /* size */
      1,                              /* version */
      PFD_DRAW_TO_BITMAP |			  /* Format Must Support Bitmap*/
      PFD_SUPPORT_OPENGL |
      PFD_SUPPORT_GDI, 		  /* Format Must Support OpenGL*/         
      PFD_TYPE_RGBA,                  /* color type */
      32,                             /* prefered color depth */
      0, 0, 0, 0, 0, 0,               /* color bits (ignored) */
      1,                              /* alpha buffer */
      0,                              /* alpha bits (ignored) */
      0,                              /* no accumulation buffer */
      0, 0, 0, 0,                     /* accum bits (ignored) */
      0,                              /* depth buffer */
      1,                              /* stencil buffer */
      0,                              /* no auxiliary buffers */
      PFD_MAIN_PLANE,                 /* main layer */
      0,                              /* reserved */
      0, 0, 0,                        /* no layer, visible, damage masks */
    };
  int pixelFormat;	
  HDC hdcurrent;

  hdcurrent = hDC;
  //hdcurrent = GetDC (NULL);//CreateDC ("DISPLAY", NULL, NULL, NULL);
  pixelFormat = ChoosePixelFormat (hdcurrent, &pfd);
  if (pixelFormat == 0) 
    {
      MessageBox (WindowFromDC(hDC), "ChoosePixelFormat failed.", "Error",
		 MB_ICONERROR | MB_OK);
      exit(1);
    }

  if (SetPixelFormat (hdcurrent, pixelFormat, &pfd) != TRUE) 
    {
	LPVOID lpMsgBuf;

	FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | 
		       FORMAT_MESSAGE_IGNORE_INSERTS,
		       NULL, GetLastError(),
		       MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
		       (LPTSTR) &lpMsgBuf,  0,  NULL );
	MessageBox (NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );
	LocalFree (lpMsgBuf );
	MessageBox (WindowFromDC(hDC), "SetPixelFormat failed.", "Error",
		    MB_ICONERROR | MB_OK);
	exit(1);
    }
  initwgl (hdcurrent, frame);
}
#endif /*_WINGUI*/

extern  int ColorPs;
/*----------------------------------------------------------------------
  GL_SetPrintForeground: set the foreground color.
  ----------------------------------------------------------------------*/
void GL_SetPrintForeground (int fg)
{
  unsigned short      red, green, blue;
  float               fact;

  if (fg != ColorPs)
    {
      TtaGiveThotRGB (fg, &red, &green, &blue);
      if (red == 0 && green == 0 && blue == 0)
	fprintf (FILE_STREAM, "0 setgray\n");
      else
	{
	  /* write the Postscript command */
	  fact = 255;
	  fprintf (FILE_STREAM, "%f %f %f setrgbcolor\n",
		   ((float) red) / fact, ((float) green) / fact, ((float) blue) / fact);
	}
      ColorPs = fg;

      /* SG : bug fix, when a new textcolor is set, the lastrgb must be reinitialized */
	  {
		int i;
		for (i = 0; i < 4; i++)
	      LastRgb[i] = -1.;
	  }
    }
  /*******
  float rgb[3];
  rgb[0] = (float) red;
  rgb[1] = (float) green;
  rgb[2] = (float) blue;
 
  GLPrintPostScriptColor (&rgb);
  ******/  
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
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

/*----------------------------------------------------------------------
  GL_NotInFeedbackMode : if all openGL operation are permitted or not.		    
  ----------------------------------------------------------------------*/
ThotBool GL_NotInFeedbackMode ()
{
  return NotFeedBackMode;
}

/*----------------------------------------------------------------------
  InitPrintBox :  	     
  ----------------------------------------------------------------------*/
void InitPrintBox ()
{
  if (NotFeedBackMode && !CompBoundingBox)
    {
      NotFeedBackMode = FALSE;  
      glFeedbackBuffer (FEEDBUFFERSIZE, GL_3D_COLOR, feedBuffer);
      glRenderMode (GL_FEEDBACK);
    }
}

/*----------------------------------------------------------------------
  FinishPrintBox :  	     
  ----------------------------------------------------------------------*/
void FinishPrintBox ()
{  
  if (NotFeedBackMode == FALSE && !CompBoundingBox)
    {  
      GLParseFeedbackBuffer (feedBuffer); 
      NotFeedBackMode = TRUE;
    }
}
#ifdef IV
/*----------------------------------------------------------------------
  PrintBox :  	     
  ----------------------------------------------------------------------*/
void PrintBox (PtrBox box, int frame, int xmin, int xmax, 
	       int ymin, int ymax)
{
  CompBoundingBox = TRUE;
  glFeedbackBuffer (FEEDBUFFERSIZE, GL_3D_COLOR, feedBuffer);
  NotFeedBackMode = FALSE;  
  glRenderMode (GL_FEEDBACK);
  DisplayBox (box, frame, xmin, xmax, ymin, ymax, NULL, FALSE);
  NotFeedBackMode = TRUE;
  GLParseFeedbackBuffer (feedBuffer);
  NotFeedBackMode = TRUE;
  CompBoundingBox = FALSE;
}
#endif
/*----------------------------------------------------------------------
  ComputeBoundingBox :
  Modify Bounding Box according to opengl feedback mechanism
  (after transformation, coordinates may have changed)			    
  ----------------------------------------------------------------------*/
void ComputeBoundingBox (PtrBox box, int frame, int xmin, int xmax, 
			 int ymin, int ymax)
{
  return;
}

/*----------------------------------------------------------------------
  ComputeFilledBox :
  Modify Bounding Box according to opengl feedback mechanism
  (after transformation, coordinates may have changed)			    
  ----------------------------------------------------------------------*/
void ComputeFilledBox (PtrBox box, int frame, int xmin, int xmax,
                       int ymin, int ymax, ThotBool show_bgimage)
{
  return;
}

/*----------------------------------------------------------------------
  GL_prepare: If a modif has been done
  ----------------------------------------------------------------------*/
ThotBool GL_prepare (int frame)
{ 
#ifdef _GTK
  return gdk_gl_pixmap_make_current (glpixmap, context);
#endif 

#ifdef _WX
  wx_canvas->SetCurrent();
#endif /* _WX */
  
#ifdef _WINGUI
  return wglMakeCurrent (GL_Windows[frame], GL_Context[frame]);
#endif /* _WINGUI */
  return TRUE;
}

/* system */
#define GLSameColor(rgb1, rgb2) (!(rgb1[0] != rgb2[0] || \
                                                    rgb1[1] != rgb2[1] || \
						    rgb1[2] != rgb2[2]))


/* PostScript writings. */
/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void GLWriteByte (FILE *stream, unsigned char byte)
{
  unsigned char h = byte / 16;
  unsigned char l = byte % 16;

  fprintf (stream, "%x%x", h, l);
}



/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
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

      /* SG : bug fix, when a new RGB is set, the textcolor must be reinitialized */
      ColorPs = -1;
    }
}




/****************
End POSTCRIPT
*****************/

/*----------------------------------------------------------------------
  The feedback buffer parser
  ----------------------------------------------------------------------*/
static int GLGetVertex(GLvertex *v, GLfloat *p)
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

/*-----------------------------------------------------------------
GLParseFeedbackBuffer : Reads the buffer containing the information 
about each graphic rendered on screen by openGL
-------------------------------------------------------------------*/
int GLParseFeedbackBuffer (GLfloat *current)
{
  char flag, dash = 0;
  int i, used, count, v, vtot;
  GLfloat lwidth = 1., psize = 1.;
  GLvertex vertices[3];

  used = glRenderMode (GL_RENDER);
  /*
	used doit etre > 0, 
	sinon cela signifie qu'opengl n'arrive pas a dessiner !!
	(ou qu'il dessine des objets transparents...
  */
  if (used > 0)
    {
      while (used > 0)
	{
	  switch ((int)*current)
	    {
	    case GL_POINT_TOKEN :
	      current ++;
	      used --;
	      i = GLGetVertex (&vertices[0], current);
	      current += i;
	      used    -= i;
#if defined(_GTK) || defined(_WX)
	      GLPrintPostScriptColor (vertices[0].rgb);
	      fprintf (FILE_STREAM, "%g -%g %g P\n", 
		       vertices[0].xyz[0], GL_HEIGHT - vertices[0].xyz[1], 0.5*psize);
#endif /*#if defined(_GTK) || defined(_WX)*/
	      /* Call Win32 function */
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

#if defined(_GTK) || defined(_WX)
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
		fprintf (FILE_STREAM, "[] 0 setdash\n");
#endif /* #if defined(_GTK) || defined(_WX) */
	      break;

	    case GL_POLYGON_TOKEN :
	      count = (int)current[1];
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
#if defined(_GTK) || defined(_WX)
		      GLPrintPostScriptColor (vertices[0].rgb);
		      fprintf (FILE_STREAM, "%g -%g %g -%g %g -%g T\n",
			       vertices[2].xyz[0], GL_HEIGHT - vertices[2].xyz[1],
			       vertices[1].xyz[0], GL_HEIGHT - vertices[1].xyz[1],
			       vertices[0].xyz[0], GL_HEIGHT - vertices[0].xyz[1]);
		      vertices[1] = vertices[2];
#endif /*#if defined(_GTK) || defined(_WX) */
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
	      switch ((int) current[1])
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


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
int GLText (const char *str, const int fg, const void *font,
	      const unsigned int fontsize, const int x, const int y,
	      const int length)
{
  char fontname[35];
  int i, width;

  width = 0;
  if (length > 0)
    {
      GL_SetPrintForeground (fg); 
      GetPostscriptNameFromFont ((ThotFont) font, fontname);
      fprintf (FILE_STREAM, fontname);
      fprintf (FILE_STREAM, "(");
      for (i = 0; i < length; i++)
	{
	  fprintf (FILE_STREAM, "%c", str[i]); 
	  width += CharacterWidth (42, (ThotFont) font);     
	}
      fprintf (FILE_STREAM, ") %d %d %d s\n", width, x, -y);
    }
  return width;
}


/*----------------------------------------------------------------------
   Transcode emit the PostScript code for the given char.
  ----------------------------------------------------------------------*/
static void Transcode (FILE *fout, int encoding, unsigned char car)
{
  if (!fout)
	return;
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

static  int         SameBox = 0;
static  int         NbWhiteSp = 0;
/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
int GLString (unsigned char *buff, int lg, int frame, int x, int y,
		ThotFont font, int boxWidth, int bl, int hyphen,
		int startABlock, int fg)
{
  FILE               *fout;
  int                 j, i, encoding, width;
  int                 noJustifiedWhiteSp;
  char                fontname[35];

  fout = (FILE *) FILE_STREAM;
  if (!fout)
	return 0;
  encoding = 0;
  if (y < 0)
    return 0;

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
      
  buff[lg] = EOS;
  /* Add the justified white space */
  if (bl > 0)
    {
      NbWhiteSp += bl;
      if (fg >= 0)
	{
	  for (i = 1; i <= bl; i++)
	    fprintf (fout, "%c", ' ');
	}
    }

  width = 0;
  if (lg > 0)
    {
      /* noJustifiedWhiteSp is > 0 if writing a fixed lenght is needed */
      /* and equal to 0 if a justified space is to be printed */  
      noJustifiedWhiteSp = startABlock;
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

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
int GLDrawPixelsPoscript (int width, int height,
			  int xorig, int yorig,
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
  if (!stream)
	  return 0;
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

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
int GLEnable(int mode)
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

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
int GLDisable(int mode)
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

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
int GLPointSize(GLfloat value)
{
  if (!GL) 
    return 0;
  glPassThrough(GL_SET_POINT_SIZE);
  glPassThrough(value);
  return 1;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
int GLLineWidth(GLfloat value)
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

#ifdef _WIN_PRINT
/*----------------------------------------------------------------------
  WDrawString draw a char string of lg chars beginning in buff.
   ----------------------------------------------------------------------*/
int WDrawString (wchar_t *buff, int lg, int frame, int x, int y,
		 ThotFont font, int boxWidth, int bl, int hyphen,
		 int startABlock, int fg)
{
  if (lg < 0)
    return 0;
  
  y += FrameTable[frame].FrTopMargin;
  /* Appeler un fonction win32 de rendu de texte  !!! */
  return 1;
}
#endif /*_WIN_PRINT*/
#endif /* _WINDOWS */

/*----------------------------------------------------------------------
  SetBadCard :  handle video cards that flush backbuffer after each
  buffer swap
  ----------------------------------------------------------------------*/
void SetBadCard (ThotBool badbuffer)
{
}
#endif /* _GL */
