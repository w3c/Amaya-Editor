
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


/* In case of old opengl version where
 Texture Objects were an extension */
/* #if defined(GL_VERSION_1_1) */
/* #define TEXTURE_OBJECT 1 */
/* #elif defined(GL_EXT_texture_object) */
/* #define TEXTURE_OBJECT 1 */
/* #define glBindTexture(A,B)     glBindTextureEXT(A,B) */
/* #define glGenTextures(A,B)     glGenTexturesEXT(A,B) */
/* #define glDeleteTextures(A,B)  glDeleteTexturesEXT(A,B) */
/* #endif */


typedef struct _GL_texturemap
{
  float x;
  float y;
} GL_texturemap;

typedef struct _GL_textureinfo
{
  GL_texturemap uv[2];
  GLuint textureid;
} GL_textureinfo;

typedef struct _GL_glyph
{
  FT_BBox bbox;
  FT_Vector pos;
  FT_Vector dimension;  
  float advance;
  void *data;
} GL_glyph;

typedef struct _GL_font
{
  FT_Face *face;  
  GL_glyph **glyphList;
} GL_font;

#define PADDING 1

static GL_font       *FontOpen (const char* fontname);
static void          FontClose (GL_font *font);
static int           FontAscender (GL_font *font);
static int           FontDescender (GL_font *font);
static int           FontRender (GL_font *font, const char* string );
static int          FontFaceSize (GL_font *font, const unsigned int size, const unsigned int res );
static int           FontCharMap (GL_font *font, FT_Encoding encoding, char alphabet);

static void          FontBBox (GL_font *font,
			       char* string,
			       float *llx, float *lly, 
			       float *llz, float *urx, 
			       float  *ury, float *urz);
static float         FontAdvance (GL_font *font, const char* string);

/* Font handling internals */
static int           FTLibraryInit ();
static void          FTLibraryFree ();


static int           RenderGlyph (GL_glyph *glyph, FT_Vector pen);
static void          MakeGlyphList(GL_font *font);
static void          FaceKernAdvance (FT_Face face, unsigned int index1, 
				      unsigned int index2, FT_Vector *kernAdvance);


static GL_glyph      *MakeBitmapGlyph(GL_font *font, unsigned int g);
static int           BitmapGlyphRender (GL_glyph *glyph, FT_Vector pen);
static void          BitmapFontMakeGlyphList (GL_font *font);

/* static GLuint        NextPowerOf2 (GLuint in); */
static float         SizeWidth (GL_font *font);
static float         SizeWidth (GL_font *font);
static GL_glyph      *MakeTextureGlyph (GL_font *font, unsigned int g, 
				   int *remGlyphs, int  *numTextures,
				   GLuint glTextureID[],
				   int *xOffset, int *yOffset);
static int           TextureFontCreateTexture (int *textureWidth, int *textureHeight, 
				     float glyphWidth, float glyphHeight,
				     int numgGlyphs, int remGlyphs);
static int           TextureGlyphRender (GL_glyph *glyph, FT_Vector pen);
static void          TextureFontMakeGlyphList (GL_font *font);

#define TRUE 1
#define FALSE 0




