
#ifndef _OPENGLFONTS_H_
#define _OPENGLFONTS_H_

#ifdef _WINDOWS
#include <windows.h>
#endif /*_WINDOWS*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <GL/gl.h>

#ifdef GL_MESA_window_pos
#define MESA
#endif

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_TRUETYPE_IDS_H

typedef struct _GL_glyph
{
  FT_BBox   bbox;
  FT_Vector pos;
  FT_Vector dimension;  
  int       advance;
  void      *data;
} GL_glyph;

typedef struct _Cache_index {
  unsigned int  index;
  unsigned int  character;
  GL_glyph      glyph;  
  struct _Cache_index *next;
} Char_Cache_index;

typedef struct _GL_font
{
  FT_Face   face;
  int       kerning;
  int       Cache_index;
  unsigned int size;
  int       height;
  int       ascent;
  Char_Cache_index *Cache;  
} GL_font;



typedef struct F_VECTOR {
  float x;
  float y;
} FLOAT_VECTOR;

typedef struct F_Slot {
  char     *name;
  GL_font  *font;
  int      size;
  int      ref;
} Font_Slot;


static GL_font       *FontOpen (const char* fontname);
static void          FontClose (GL_font *font);
static int           FontDescender (GL_font *font);

static int           FontFaceSize (GL_font *font,
			   unsigned int size,
			   unsigned int res);

static int           FontCharMap (GL_font *font,
				  FT_Encoding encoding);
static void          FontBBox (GL_font *font,
			       wchar_t* string,
			       int length,
			       float *llx, float *lly, 
			       float *llz, float *urx, 
			       float  *ury, float *urz);



/* Font handling internals */
static int           FTLibraryInit ();
void                 FTLibraryFree ();


static float          FaceKernAdvance (FT_Face face, 
				       unsigned int index1, 
				       unsigned int index2);

static void           MakeBitmapGlyph (GL_font *font,
				       unsigned int g,
				       GL_glyph *BitmapGlyph);

extern void MakePolygonGlyph (GL_font *font,
		      unsigned int g,
			      GL_glyph *BitmapGlyph);



extern int   UnicodeFontRenderPoly (void *gl_font, wchar_t *string, 
			 float x, float y, int size);
#ifndef PADDING
#define PADDING 1
#endif 

#ifndef TRUE
#define TRUE 1
#endif 

#ifndef FALSE
#define FALSE 0
#endif 

#endif



