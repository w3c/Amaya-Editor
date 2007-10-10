
#ifndef _OPENGLFONT_H_
#define _OPENGLFONT_H_

#include "glglyph.h"
int   gl_font_char_width  (void *gl_void_font, wchar_t c);
int   gl_font_char_height (void *gl_void_font, wchar_t *c);
int   gl_font_char_ascent (void *gl_void_font, wchar_t *c);

void  *GetFirstFont (int size);

void GL_Font_Change_Height (void *font, int size);
unsigned int GL_Font_Get_Size (void *font);

int UnicodeFontRender (void *gl_font, wchar_t *string, 
		       float x, float y, int size);

int UnicodeFontRenderPoly (void *gl_font, wchar_t *string, 
			   float x, float y, int size);

int   gl_font_ascent      (void *gl_void_font);
int   gl_font_height      (void *gl_void_font);
void *gl_font_init        (const char *font_filename, 
			   char alphabet, int size);
void  gl_font_delete      (void *gl_font);
int   GetFontFilename     (char script, int family, 
			   int highlight, int size,  char *filename);
void  FTLibraryFree ();
int SetTextureScale (ThotBool Scaled);
void StopTextureScale (int texture_id);
unsigned char *GetCharacterGlyph (GL_font *font, unsigned int idx, int w, int h);
void MakePolygonGlyph (GL_font *font, unsigned int g, GL_glyph *BitmapGlyph);

int   UnicodeFontRenderPoly (void *gl_font, wchar_t *string, 
			     float x, float y, int size);

Char_Cache_index *Char_index_lookup_cache (GL_font *font, unsigned int idx,
					   ThotBool isPoly);
#endif/*_OPENGLFONT_H_*/
