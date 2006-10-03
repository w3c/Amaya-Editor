#ifndef __GLPRINT_H__
#define __GLPRINT_H__

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef WIN32
#  include <windows.h>
#endif

#ifdef __APPLE__
#  include <OpenGL/gl.h>
#else
#  include <GL/gl.h>
#endif

#define GL_PS 1
#define GL_EPS 1

#define GL_SUCCESS 1

#define GLPRINT_LINE_STIPPLE               3

/* Arguments for gl2psEnable/gl2psDisable */

/* Magic numbers */

#define GL_EPSILON                    5.e-3
#define GL_DEPTH_FACT                 1000.0
#define GL_SIMPLE_OFFSET              0.05
#define GL_SIMPLE_OFFSET_LARGE        1.0
#define GLPRINT_ZERO(arg)                  (fabs(arg)<1.e-20)

/* Primitive types */

#define GLPRINT_TEXT                       1
#define GLPRINT_POINT                      2
#define GLPRINT_LINE                       3
#define GLPRINT_QUADRANGLE                 4
#define GLPRINT_TRIANGLE                   5
#define GLPRINT_PIXMAP                     6

/* Pass through options */

#define GL_BEGIN_LINE_STIPPLE         5
#define GL_END_LINE_STIPPLE           6
#define GL_SET_POINT_SIZE             7
#define GL_SET_LINE_WIDTH             8
 
#define FORMAT GL_PS

#undef GL_LANDSCAPE
#define GL_LANDSCAPE 0

#undef GL_DRAW_BACKGROUND
#define GL_DRAW_BACKGROUND 0

typedef GLfloat GLrgb[4];
typedef GLfloat GLxyz[3];

typedef struct 
{
  GLxyz   xyz;
  GLrgb   rgb;
} GLvertex;

typedef struct 
{
  GLshort     fontsize;
  char        *str, *fontname;
} GLstring;

typedef struct 
{
  GLsizei  width, height;
  GLenum   format, type;
  GLfloat  *pixels;
} GLimage;


typedef struct 
{
  GLshort   type, numverts;
  char      boundary, dash, culled;
  GLfloat   width, depth;
  GLvertex  *verts;
  GLstring  *text;
  GLimage   *image;
} GLprimitive;

typedef struct 
{
  int         buffersize;
  float       lastlinewidth;
  GLfloat     *feedback;
  GLrgb       lastrgb, threshold;
  int         viewport[4];
  char        *title, *producer, *filename;
  FILE        *stream;
} GLcontext;

/* public functions */
/*
#ifdef __cplusplus
extern "C" {
#endif
*/
  int GLBeginPage (char *title, char *producer, 
		   int viewport[4],
		   FILE *stream, const char *filename);

  int GLEndPage (void);

  int GLBeginViewport (int viewport[4]);
  int GLEndViewport (void);

  int GLEnable (int mode);
  int GLDisable (int mode);
  int GLPointSize (GLfloat value);
  int GLLineWidth (GLfloat value);

  /*Graphic stuff*/
  int GLParseFeedbackBuffer (GLfloat *current);

  void GLPrintPostScriptColor(void *rgb);
 
  /*Text*/
  int GLText (const char *str, 
	      const int fg, 
	      const void *font, 
	      const unsigned int fontsize, 
	      const int x, 
	      const int y, 
	      const int length);
  int GLString (unsigned char *buff, int lg, int frame, int x, int y,
		ThotFont font, int boxWidth, int bl, int hyphen,
		int startABlock, int fg);

  /*Pictures*/
  int GLDrawPixelsPoscript(int width, int height,
			   int xorig, int yorig,
			   GLenum format, GLenum type, 
			   unsigned char *pixels, 
			   GLfloat x, GLfloat y);

  void FinishPrintBox();
  void InitPrintBox ();
/*
#ifdef __cplusplus
};
#endif
*/
#endif /* __GLPRINT_H__ */
