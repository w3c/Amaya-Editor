#ifdef _GL

#include <string.h>
#include <sys/types.h>
#include <stdarg.h>
#include <time.h>
#include "gl2ps.h"


#define FORMAT GL_PS

#undef GL_LANDSCAPE
#define GL_LANDSCAPE 0

#undef GL_DRAW_BACKGROUND
#define GL_DRAW_BACKGROUND 0

#undef GL_NO_PS3_SHADING
#define GL_NO_PS3_SHADING 0


/* The GL context. GL is not thread safe (we should create a
   local GLcontext during GLBeginPage) */

GLcontext *GL = NULL;


/* system */

static void *GLMalloc (size_t size)
{
  void *ptr;

  if (!size) 
    return(NULL);
  ptr = malloc(size);  
  return(ptr);
}

static void GLFree (void *ptr)
{
  if (!ptr) 
    return;
  free(ptr);
}

#define GLSameColor(rgba1, rgba2) (!(rgba1[0] != rgba2[0] || \
                                                    rgba1[1] != rgba2[1] || \
						    rgba1[2] != rgba2[2]))


/* PostScript writings. */

static void GLWriteByte (FILE *stream, unsigned char byte)
{
  unsigned char h = byte / 16;
  unsigned char l = byte % 16;

  fprintf (stream, "%x%x", h, l);
}


static void GLPrintPostScriptHeader(void)
{
  GLfloat rgba[4];
  time_t now;

  time(&now);

  if(FORMAT == GL_PS)
    fprintf (GL->stream, "%%!PS-Adobe-3.0\n");
  else
    fprintf (GL->stream, "%%!PS-Adobe-3.0 EPSF-3.0\n");

  fprintf (GL->stream, 
	   "%%%%Title: %s\n"
	   "%%%%Creator: Amaya\n"
	   "%%%%For: %s\n"
	   "%%%%CreationDate: %s"
	   "%%%%LanguageLevel: 3\n"
	   "%%%%DocumentData: Clean7Bit\n"
	   "%%%%Pages: 1\n",
	   GL->title, 
	   GL->producer, 
	   ctime(&now));

  if(FORMAT == GL_PS)
    {
      fprintf (GL->stream, 
	       "%%%%Orientation: %s\n"
	       "%%%%DocumentMedia: Default %d %d 0 () ()\n",
	       (GL_LANDSCAPE) ? "Landscape" : "Portrait",
	       (GL_LANDSCAPE) ? GL->viewport[3] : GL->viewport[2],
	       (GL_LANDSCAPE) ? GL->viewport[2] : GL->viewport[3]);
    }

  fprintf (GL->stream,
	   "%%%%BoundingBox: %d %d %d %d\n"
	   "%%%%EndComments\n",
	   (GL_LANDSCAPE) ? GL->viewport[1] : GL->viewport[0],
	   (GL_LANDSCAPE) ? GL->viewport[0] : GL->viewport[1],
	   (GL_LANDSCAPE) ? GL->viewport[3] : GL->viewport[2],
	   (GL_LANDSCAPE) ? GL->viewport[2] : GL->viewport[3]);

  /* RGB color: r g b C (replace C by G in output to change from rgb to gray)
     Grayscale: r g b G
     Font choose: size fontname FC
     String primitive: (string) x y size fontname S
     Point primitive: x y size P
     Line width: width W
     Flat-shaded line: x2 y2 x1 y1 L
     Smooth-shaded line: x2 y2 r2 g2 b2 x1 y1 r1 g1 b1 SL
     Flat-shaded triangle: x3 y3 x2 y2 x1 y1 T
     Smooth-shaded triangle: x3 y3 r3 g3 b3 x2 y2 r2 g2 b2 x1 y1 r1 g1 b1 ST */

  fprintf (GL->stream,
	   "%%%%BeginProlog\n"
	   "/GLdict 64 dict def GLdict begin\n"
	   "1 setlinecap 1 setlinejoin\n"
	   "/tryPS3shading %s def %% set to false to force subdivision\n"
	   "/rThreshold %g def %% red component subdivision threshold\n"
	   "/gThreshold %g def %% green component subdivision threshold\n"
	   "/bThreshold %g def %% blue component subdivision threshold\n"
	   "/BD { bind def } bind def\n"
	   "/C  { setrgbcolor } BD\n"
	   "/G  { 0.082 mul exch 0.6094 mul add exch 0.3086 mul add neg 1.0 add setgray } BD\n"
	   "/W  { setlinewidth } BD\n"
	   "/FC { findfont exch scalefont setfont } BD\n"
	   "/S  { FC moveto show } BD\n"
	   "/P  { newpath 0.0 360.0 arc closepath fill } BD\n"
	   "/L  { newpath moveto lineto stroke } BD\n"
	   "/SL { C moveto C lineto stroke } BD\n"
	   "/T  { newpath moveto lineto lineto closepath fill } BD\n",
	   (GL_NO_PS3_SHADING) ? "false" : "true",
	   GL->threshold[0], GL->threshold[1], GL->threshold[2]);

  /* Flat-shaded triangle with middle color:
     x3 y3 r3 g3 b3 x2 y2 r2 g2 b2 x1 y1 r1 g1 b1 Tm */
  fprintf (GL->stream,
	   /* stack : x3 y3 r3 g3 b3 x2 y2 r2 g2 b2 x1 y1 r1 g1 b1 */
	   "/Tm { 3 -1 roll 8 -1 roll 13 -1 roll add add 3 div\n" /* r = (r1+r2+r3)/3 */
	   /* stack : x3 y3 g3 b3 x2 y2 g2 b2 x1 y1 g1 b1 r */
	   "      3 -1 roll 7 -1 roll 11 -1 roll add add 3 div\n" /* g = (g1+g2+g3)/3 */
	   /* stack : x3 y3 b3 x2 y2 b2 x1 y1 b1 r g b */
	   "      3 -1 roll 6 -1 roll 9 -1 roll add add 3 div" /* b = (b1+b2+b3)/3 */
	   /* stack : x3 y3 x2 y2 x1 y1 r g b */
	   " C T } BD\n");

  fprintf (GL->stream,
	   "tryPS3shading\n"
	   "{ /shfill where\n"
	   "  { /ST { STshfill } BD }\n"
	   "  { /ST { STnoshfill } BD }\n"
	   "  ifelse }\n"
	   "{ /ST { STnoshfill } BD }\n"
	   "ifelse\n");

  fprintf (GL->stream,
	   "end\n"
	   "%%%%EndProlog\n"
	   "%%%%BeginSetup\n"
	   "/DeviceRGB setcolorspace\n"
	   "GLdict begin\n"
	   "%%%%EndSetup\n"
	   "%%%%Page: 1 1\n"
	   "%%%%BeginPageSetup\n");

  fprintf (GL->stream, 
	   "%%%%EndPageSetup\n"
	   "mark\n"
	   "gsave\n"
	   "1.0 1.0 scale\n");
	  
  if(GL_DRAW_BACKGROUND)
    {
      glGetFloatv(GL_COLOR_CLEAR_VALUE, rgba);

      fprintf (GL->stream,
	       "%g %g %g C\n"
	       "newpath %d %d moveto %d %d lineto %d %d lineto %d %d lineto\n"
	       "closepath fill\n",
	       rgba[0], rgba[1], rgba[2], 
	       GL->viewport[0], GL->viewport[1], GL->viewport[2], 
	       GL->viewport[1], GL->viewport[2], GL->viewport[3], 
	       GL->viewport[0], GL->viewport[3]);
    }
}

static void GLPrintPostScriptColor(GLrgba rgba)
{
  if (!GLSameColor(GL->lastrgba, rgba))
    {
      GL->lastrgba[0] = rgba[0];
      GL->lastrgba[1] = rgba[1];
      GL->lastrgba[2] = rgba[2];
      fprintf (GL->stream, "%g %g %g C\n", rgba[0], rgba[1], rgba[2]);
    }
}



static void GLPrintPostScriptFooter(void)
{
  fprintf (GL->stream,
	   "grestore\n"
	   "showpage\n"
	   "cleartomark\n"
	   "%%%%PageTrailer\n"
	   "%%%%Trailer\n"
	   "end\n"
	   "%%%%EOF\n");
}

static void GLPrintPostScriptBeginViewport(GLint viewport[4])
{
  GLfloat rgba[4];
  int x = viewport[0], y = viewport[1], w = viewport[2], h = viewport[3];

  glRenderMode (GL_FEEDBACK);

  fprintf (GL->stream, 
	   "gsave\n"
	   "1.0 1.0 scale\n");
	  
  if (GL_DRAW_BACKGROUND)
    {
      glGetFloatv(GL_COLOR_CLEAR_VALUE, rgba);
     
      fprintf (GL->stream,
	       "%g %g %g C\n"
	       "newpath %d %d moveto %d %d lineto %d %d lineto %d %d lineto\n"
	       "closepath fill\n",
	       rgba[0], rgba[1], rgba[2], 
	       x, y, x+w, y, x+w, y+h, x, y+h);
      fprintf (GL->stream,
	       "newpath %d %d moveto %d %d lineto %d %d lineto %d %d lineto\n"
	       "closepath clip\n",
	       x, y, x+w, y, x+w, y+h, x, y+h);
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
  v->rgba[0] = p[3];
  v->rgba[1] = p[4];
  v->rgba[2] = p[5];
  v->rgba[3] = p[6]; 
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

	      GLPrintPostScriptColor (vertices[0].rgba);
	      fprintf (GL->stream, "%g %g %g P\n", 
		       vertices[0].xyz[0], vertices[0].xyz[1], 0.5*psize);

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

	      if (GL->lastlinewidth != lwidth)
		{
		  GL->lastlinewidth = lwidth;
		  fprintf (GL->stream, "%g W\n", GL->lastlinewidth);
		}
	      if (dash)
		{
		  fprintf (GL->stream, "[%d] 0 setdash\n", dash);
		}
	      GLPrintPostScriptColor (vertices[0].rgba);
	      fprintf (GL->stream, "%g %g %g %g L\n",
		       vertices[1].xyz[0], vertices[1].xyz[1],
		       vertices[0].xyz[0], vertices[0].xyz[1]);   
	      if (dash)
		{
		  fprintf (GL->stream, "[] 0 setdash\n");
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
		      GLPrintPostScriptColor (vertices[0].rgba);
		      fprintf (GL->stream, "%g %g %g %g %g %g T\n",
			       vertices[2].xyz[0], vertices[2].xyz[1],
			       vertices[1].xyz[0], vertices[1].xyz[1],
			       vertices[0].xyz[0], vertices[0].xyz[1]);
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


static GLint GLPrintPostScriptEndViewport (void)
{
  GLint res;

  fprintf (GL->stream, "grestore\n");
  return res;
}


/* The public routines */
GLint GLBeginPage (const char *title, const char *producer, 
		   GLint viewport[4], 
		   FILE *stream, const char *filename)
{
  int i;

  GL = (GLcontext*) GLMalloc (sizeof (GLcontext));  
  GL->title = title;
  GL->producer = producer;
  GL->filename = filename;
  GL->threshold[0] = 0.032;
  GL->threshold[1] = 0.017;
  GL->threshold[2] = 0.05;
  for (i = 0; i < 4; i++)
      GL->lastrgba[i] = -1.;
  for (i = 0; i < 4; i++)
      GL->viewport[i] = viewport[i];
  GL->lastlinewidth = -1.;
  GL->stream = stream;

  GLPrintPostScriptHeader();
  return 1;
}

GLint GLEndPage (void)
{
  GLint res;
  
  GLPrintPostScriptFooter ();

  fflush (GL->stream);

  
  GLFree (GL);
  GL = NULL;

  return res;
}

GLint GLBeginViewport (GLint viewport[4])
{
  if (FORMAT == GL_EPS)
    GLPrintPostScriptBeginViewport (viewport);
       
  return GL_SUCCESS;
}

GLint GLEndViewport (void)
{
  GLint res;

  if (FORMAT == GL_EPS)
    res = GLPrintPostScriptEndViewport ();

  return res;
}

GLint GLText (const char *str, 
	      const char *fontname, 
	      GLshort fontsize, 
	      GLfloat x, GLfloat y)
{
  GLfloat rgba[4];

  glGetFloatv (GL_CURRENT_RASTER_COLOR, rgba);
  GLPrintPostScriptColor (rgba);

  fprintf (GL->stream, "(%s) %g %g %d /%s S\n",
	   str, x, y,
	   fontsize, fontname);

  return GL_SUCCESS;
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

  stream = GL->stream;
  x = x + xorig;
  y = y + yorig;

  if((width <= 0) || (height <= 0))
    return 0;

  fprintf (stream, "gsave\n");
  fprintf (stream, "%.2f %.2f translate\n", x, y); 
  fprintf (stream, "%d %d scale\n", width, height); 
  
  Mode = (format == GL_RGBA)?1:0;

  /* 8 bit for r and g and b */
  fprintf (stream, "/rgbstr %d string def\n", width * 3);
  fprintf (stream, "%d %d %d\n", width, height, 8);
  fprintf (stream, "[ %d 0 0 -%d 0 %d ]\n", width, height, height); 
  fprintf (stream, "{ currentfile rgbstr readhexstring pop }\n");
  fprintf (stream, "false 3\n");
  fprintf (stream, "colorimage\n");

  colwidth = width*((Mode)?4:3);
  pixels += (height-1)*colwidth;
  current = pixels;
  /* inverts pixels upside-down*/
  for (row = 0; row < height; row++)
    {
      col = 0;
      while (col < width)
	{
	  GLWriteByte (stream, *current++);/*R*/
	  GLWriteByte (stream, *current++);/*G*/
	  GLWriteByte (stream, *current++);/*B*/
	  
	  col ++;
	  if (Mode)
	    current++;
	}
      current -= colwidth + colwidth;
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

#endif /* _GL
 */
