
#ifndef _GLGLYPH_H_
#define _GLGLYPH_H_

#ifdef _WINGUI
#include <windows.h>
#endif /*_WINGUI*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef _MACOS
#include <gl.h>
#else /* _MACOS */
#include <GL/gl.h>
#endif /* _MACOS */

#ifdef GL_MESA_window_pos
#define MESA
#endif

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_TRUETYPE_IDS_H


#ifndef FT_CURVE_TAG_CONIC
#define FT_CURVE_TAG_CONIC FT_Curve_Tag_Conic
#endif

#ifndef FT_CURVE_TAG_ON
#define FT_CURVE_TAG_ON FT_Curve_Tag_On
#endif

#ifndef FT_CURVE_TAG_CUBIC
#define FT_CURVE_TAG_CUBIC FT_Curve_Tag_Cubic
#endif

#ifndef FT_OUTLINE_EVEN_ODD_FILL
#define FT_OUTLINE_EVEN_ODD_FILL ft_outline_even_odd_fill
#endif

#ifndef FT_OUTLINE_REVERSE_FILL
#define FT_OUTLINE_REVERSE_FILL ft_outline_reverse_fill
#endif

/*
 * A GL_glyph can be of tow types
 * a glList or a bitmap
 * it's necessary to indicate the type of the data to be able to free it.
 */
typedef enum
  {
    GL_GLYPH_DATATYPE_NONE,
    GL_GLYPH_DATATYPE_GLLIST, /* a glList */
    GL_GLYPH_DATATYPE_FTBITMAP /* a freetype bitmap */
} GL_glyph_DataType;

typedef struct _GL_glyph
{
  FT_BBox           bbox;
  FT_Vector         pos;
  FT_Vector         dimension;  
  int               advance;
  void             *data;
  GL_glyph_DataType data_type;
} GL_glyph;

typedef struct _Cache_index {
  unsigned int         index;
  unsigned int         character;
  GL_glyph             glyph;  
  struct _Cache_index *next;
} Char_Cache_index;

typedef struct _GL_font
{
  FT_Face   face;
  int               kerning;
  int               Cache_index;
  unsigned int      size;
  int               height;
  int               ascent;
  Char_Cache_index *Cache;  
} GL_font;


typedef struct F_VECTOR {
  float x;
  float y;
} FLOAT_VECTOR;

typedef struct F_Slot {
  char     *name;
  GL_font  *font;
  int       size;
  int       ref;
} Font_Slot;

#ifndef PADDING
#define PADDING 1
#endif 

#ifndef TRUE
#define TRUE 1
#endif 

#ifndef FALSE
#define FALSE 0
#endif 

#endif /* GLGLYPH */
