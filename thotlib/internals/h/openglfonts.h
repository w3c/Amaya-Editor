
#ifndef _OPENGLFONTS_H_
#define _OPENGLFONTS_H_
static GL_font *FontOpen (const char* fontname);
static void     FontClose (GL_font *font);
static int      FontDescender (GL_font *font);

static int      FontFaceSize (GL_font *font,
			      unsigned int size,
			      unsigned int res);
static int      FontCharMap (GL_font *font,
			     FT_Encoding encoding);
static void     FontBBox (GL_font *font,
			  wchar_t* string,
			  int length,
			  float *llx, float *lly, 
			  float *llz, float *urx, 
			  float  *ury, float *urz);
/* Font handling internals */
static int      FTLibraryInit ();
void            FTLibraryFree ();

static float    FaceKernAdvance (FT_Face face, 
				 unsigned int index1, 
				 unsigned int index2);

static void     MakeBitmapGlyph (GL_font *font,
				 unsigned int g,
				 GL_glyph *BitmapGlyph);
#endif



