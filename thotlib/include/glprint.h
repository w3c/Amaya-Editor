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

typedef GLfloat GLrgba[4];
typedef GLfloat GLxyz[3];

typedef struct {
  GLxyz xyz;
  GLrgba rgba;
} GLvertex;

typedef struct {
  GLshort fontsize;
  char *str, *fontname;
} GLstring;

typedef struct {
  GLsizei width, height;
  GLenum format, type;
  GLfloat *pixels;
} GLimage;

typedef struct {
  GLshort type, numverts;
  char boundary, dash, culled;
  GLfloat width, depth;
  GLvertex *verts;
  GLstring *text;
  GLimage *image;
} GLprimitive;

typedef struct {
  GLint buffersize;
  const char *title, *producer, *filename;
  GLfloat *feedback;
  GLint viewport[4];
  GLrgba lastrgba, threshold;
  float lastlinewidth;
  FILE *stream;
} GLcontext;

/* public functions */

#ifdef __cplusplus
extern "C" {
#endif

GLint GLBeginPage (const char *title, const char *producer, 
				  GLint viewport[4],
				  FILE *stream, const char *filename);

GLint GLEndPage (void);

GLint GLBeginViewport (GLint viewport[4]);
GLint GLEndViewport (void);

GLint GLEnable (GLint mode);
GLint GLDisable (GLint mode);
GLint GLPointSize (GLfloat value);
GLint GLLineWidth (GLfloat value);

/*Graphic stuff*/
GLint GLParseFeedbackBuffer (GLfloat *current);

/*Text*/
GLint GLText (const char *str, const char *fontname, GLshort fontsize, 
	      GLfloat x, GLfloat y);

/*Pictures*/
GLint GLDrawPixels (GLsizei width, GLsizei height,
				   GLint xorig, GLint yorig,
				   GLenum format, GLenum type, const void *pixels);
#ifdef __cplusplus
};
#endif

#endif /* __GLPRINT_H__ */
