/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2002.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Handle Opengl Fonts with Freetype 2 
 * (http://www.freetype.org)
 * ( Here we make glyph to bitmap font || antialiased texture font)
 * Based on FTGLIB a very good C++ lib that handles fonts in opengl
 * ( http://homepages.paradise.net.nz/henryj/code/index.html )
 * 
 * Author: P. Cheyrou-lagreze
 *
 */

#ifdef _GL

#include "openglfonts.h"



/* Memory state Var needed often*/
static FT_Library FTlib = NULL;
static int init_done = 0;
static int maxTextSize = 0;
static GLuint activeTextureID = 0;

/*
 Freetype library Handling
*/
static int FTLibraryInit ()
{
  int err;
  
  if (!maxTextSize)
    glGetIntegerv (GL_MAX_TEXTURE_SIZE, (GLint*) &maxTextSize);
  if (!maxTextSize)
    maxTextSize = 256;	
  if( FTlib != 0 )
    return TRUE;
  err = FT_Init_FreeType( &FTlib);
  if( err)
    {
      FTlib = 0;
      return FALSE;
    }
  return TRUE;
}

static void FTLibraryFree ()
{
  if (init_done)
    {
      FT_Done_FreeType (FTlib);
      FTlib= 0;
    }
}

static void FaceKernAdvance (FT_Face face, 
		      unsigned int index1, unsigned int index2, 
		      FT_Vector *kernAdvance)
{  
  int err;

  kernAdvance->x = 0;  
  kernAdvance->y = 0; 
  if (FT_HAS_KERNING(face) && index1 && index2)
    {
      err = FT_Get_Kerning (face, index1, index2, ft_kerning_unfitted, kernAdvance);
      if (!err)
	{	
	  kernAdvance->x /= 64; 
	  kernAdvance->y /= 64;
	}
    }
}

/*-------------------------------------------
  MakeAlphaBitmap : Make antialiased bitmap
--------------------------------------------*/
static unsigned char *MakeAlphaBitmap (unsigned char *source,
				int destWidth, int destHeight, int Pitch)
{
  unsigned char *data, *ptr;
  int x, y, current;;

  current = 0;
  data = (unsigned char *) malloc (destWidth * destHeight * sizeof (unsigned char));
  for (y = destHeight; y > 0; y--)
    {
      ptr = source + (y * destWidth - Pitch);
      for (x = 0; x < destWidth; x++)
	{
	  *( data + current) = *(ptr++);
	  current++;
	}
    }
  return data;
}

/*-----------------------------
  Bitmap specific handling
-------------------------------*/
static GL_glyph  *MakeBitmapGlyph(GL_font *font, unsigned int g)
{
  FT_BitmapGlyph  bitmap;
  FT_Bitmap       *source;
  GL_glyph        *BitmapGlyph;
  FT_Glyph        Glyph;
  int             destWidth, destHeight, 
                  srcWidth, srcHeight, 
                  srcPitch;
  char *data;
  int err;

  data = NULL;

  FT_Load_Glyph (*font->face, g,  FT_LOAD_NO_HINTING);
  FT_Get_Glyph( (*(font->face))->glyph, &Glyph);

  if(Glyph)
    {
      err = FT_Glyph_To_Bitmap( &Glyph, ft_render_mode_normal, 0, 1);
      if( err || ft_glyph_format_bitmap != Glyph->format)
	{
	  FT_Done_Glyph (Glyph);
	  return NULL; 	
	}
      else
	{
	  BitmapGlyph = (GL_glyph *) malloc ( sizeof (GL_glyph) );
	  bitmap = (FT_BitmapGlyph) Glyph;
	  source = &bitmap->bitmap;
	  srcWidth = source->width;
	  srcHeight = source->rows;
	  srcPitch = source->pitch;
	  destWidth = srcWidth;
	  destHeight = srcHeight;  
	  if (destWidth && destHeight)
	      data = MakeAlphaBitmap (source->buffer, 
				      destWidth, destHeight, 
				      srcPitch);  
	  FT_Glyph_Get_CBox (Glyph, ft_glyph_bbox_subpixels, &(BitmapGlyph->bbox));
	  BitmapGlyph->data = data;
	  BitmapGlyph->advance = (float) (Glyph->advance.x >> 16);
	  BitmapGlyph->pos.x = bitmap->left;
	  BitmapGlyph->pos.y = destHeight - bitmap->top;   
	  BitmapGlyph->dimension.x = destWidth;
	  BitmapGlyph->dimension.y = destHeight;  	  
	  FT_Done_Glyph (Glyph);
	  return BitmapGlyph;
	}
    }
  else
    return NULL;
}

static int BitmapGlyphRender (GL_glyph *glyph, FT_Vector pen)
{				    
  if(glyph->data)
    {
      glBitmap( 0, 0, 0.0, 0.0,
		pen.x, 
		pen.y - glyph->pos.y, 
		(const GLubyte *) 0 );
      glDrawPixels(glyph->dimension.x,
		   glyph->dimension.y,
		   GL_ALPHA,
		   GL_UNSIGNED_BYTE,
		   (const GLubyte *) glyph->data);
      glBitmap( 0, 0, 0.0, 0.0, 
		- pen.x, 
		- pen.y + glyph->pos.y, 
		(const GLubyte *) 0 );
    }
  return glyph->advance;
}

static void BitmapFontMakeGlyphList (GL_font *font)
{
  int c;

  font->glyphList = (GL_glyph **) malloc ( sizeof(GL_glyph *) * (*(font->face))->num_glyphs );
  for (c = 0; c < (*(font->face))->num_glyphs; c++)
    {
      if (TRUE)
	font->glyphList[c] = MakeBitmapGlyph (font, c);
      else
	font->glyphList[c] = NULL;
    }
}


/*-------------------------------
Texture Specific handling

--------------------------------*/
static GLuint NextPowerOf2 (unsigned int in)
{
     in -= 1;

     in |= in >> 16;
     in |= in >> 8;
     in |= in >> 4;
     in |= in >> 2;
     in |= in >> 1;

     return in + 1;
}


static float SizeHeight(GL_font *font)
{
  float height;
      
  if( FT_IS_SCALABLE ((*(font->face))) )
    {
      if( FT_IS_SFNT ((*(font->face))) ) 
	height = ((*(font->face))->bbox.yMax - ((*(font->face))->bbox.yMin)); 
      else
	height = ((*(font->face))->bbox.yMax - ((*(font->face))->bbox.yMin)) >> 16; 
      height =  height * ( (float) (*(font->face))->size->metrics.y_ppem / (float) (*(font->face))->units_per_EM);
      return height;
    }
  else
      return (*(font->face))->size->metrics.height >> 6;
}

static float SizeWidth(GL_font *font)
{
  float width;

  if( FT_IS_SCALABLE ((*(font->face))) )
    {
      if( FT_IS_SFNT ((*(font->face))) )
	width = ((*(font->face))->bbox.xMax - (*(font->face))->bbox.xMin); 
      else
	width = (((*(font->face))->bbox.xMax - (*(font->face))->bbox.xMin)) >> 16; 
      width = width * ( (float) (*(font->face))->size->metrics.x_ppem / (float) (*(font->face))->units_per_EM);
      return (width);
    }
  else
      return (*(font->face))->size->metrics.max_advance >> 6;
}

/*------------------------------------------------------------
  MakeTextureGlyph : 
  Turn Glyph into Alpha bitmap then store it in texture objects 
  (Video card memory Resident.)
  Texture are in the card max size texture and 
  several Glyph are stored in one texture.
 -----------------------------------------------------------*/
static GL_glyph  *MakeTextureGlyph(GL_font *font, unsigned int g, 
				   int *remGlyphs,int  *numTextures,
				   GLuint glTextureID[], 
				   int *xOffset, int *yOffset)
{
  unsigned char   *data = NULL;
  float           glyphHeight,glyphWidth; 
  int             srcPitch,destWidth, destHeight,
                  err, textureWidth, textureHeight;
  FT_BitmapGlyph  bitmap;
  FT_Bitmap*      source;
  FT_Glyph        Glyph;
  GL_glyph        *BitmapGlyph;
  GL_textureinfo *info;
  
  FT_Load_Glyph (*font->face, g,  FT_LOAD_NO_HINTING);
  FT_Get_Glyph ((*(font->face))->glyph, &Glyph);
  if (Glyph)
    {
      glyphHeight = SizeHeight (font);
      glyphWidth = SizeWidth (font);   
      if (*numTextures == 0)
	{
	  glTextureID[0] = TextureFontCreateTexture (&textureWidth, &textureHeight, 
						     glyphWidth, glyphHeight,
						     (*(font->face))->num_glyphs, 
						     *remGlyphs);
	  *xOffset = *yOffset = PADDING;
	  (*numTextures)++;
	}      
      if (*xOffset > ( textureWidth - glyphWidth))
	{
	  *xOffset = PADDING;
	  *yOffset += glyphHeight;	  
	  if( *yOffset > ( textureHeight - glyphHeight))
	    {
	      glTextureID[*numTextures] = TextureFontCreateTexture (&textureWidth, &textureHeight, 
								   glyphWidth, glyphHeight,
								   (*(font->face))->num_glyphs, 
								   *remGlyphs);
	      *yOffset = PADDING;
	      (*numTextures)++;
	    }
	}      
      err = FT_Glyph_To_Bitmap( &Glyph, ft_render_mode_normal, 0, 1);
      if (err || Glyph->format != ft_glyph_format_bitmap)
	{ 
	  FT_Done_Glyph (Glyph);
	  return NULL;
	}
      BitmapGlyph = (GL_glyph *) malloc ( sizeof (GL_glyph) );
      BitmapGlyph->data = (GL_textureinfo *) malloc (sizeof (GL_textureinfo));
      info = (GL_textureinfo *) BitmapGlyph->data; 
      bitmap = (FT_BitmapGlyph) Glyph;
      source = &bitmap->bitmap;
      srcPitch = source->pitch;
      destWidth = source->width;
      destHeight = source->rows;
      if (destWidth && destHeight)
	{
	  data = MakeAlphaBitmap (source->buffer, destWidth, destHeight, srcPitch);
	   if(activeTextureID != glTextureID[(*numTextures)-1])
	     {
	       glBindTexture( GL_TEXTURE_2D, glTextureID[(*numTextures)-1]);
	       activeTextureID = glTextureID[(*numTextures)-1];
	     }
	   info->textureid = glTextureID[(*numTextures)-1];
	   glTexSubImage2D( GL_TEXTURE_2D, 0, 
			   *xOffset, *yOffset, 
			   destWidth, destHeight, 
			   GL_ALPHA, GL_UNSIGNED_BYTE, data);
	}
      FT_Glyph_Get_CBox (Glyph, ft_glyph_bbox_subpixels, &(BitmapGlyph->bbox));
      BitmapGlyph->dimension.x = destWidth;
      BitmapGlyph->dimension.y = destHeight; 
      BitmapGlyph->pos.x = bitmap->left;
      BitmapGlyph->pos.y = destHeight - bitmap->top;   
      BitmapGlyph->advance = Glyph->advance.x >> 16;
      info->uv[0].x = (float) (*xOffset) / (float) (textureWidth);
      info->uv[0].y = (float) (*yOffset) / (float) (textureHeight);
      info->uv[1].x = (float) (*xOffset + destWidth) / (float) (textureWidth);
      info->uv[1].y = (float) (*yOffset + destHeight) / (float) (textureHeight);
      if (data)
	free (data);
      FT_Done_Glyph (Glyph);
      *xOffset += BitmapGlyph->bbox.xMax - BitmapGlyph->bbox.xMin + PADDING;
      (*remGlyphs)--;       
      return BitmapGlyph;
    }
  return NULL;
}

static int TextureFontCreateTexture (int *textureWidth, int *textureHeight, 
				     float glyphWidth, float glyphHeight,
				     int numGlyphs, int remGlyphs)
{
  GLuint textID;
  int totalMem, h;
  unsigned char *textMem;
	
  *textureWidth = NextPowerOf2 ((remGlyphs * glyphWidth) + PADDING * 2);
  if( *textureWidth > maxTextSize)
      *textureWidth = maxTextSize;
  h = (int) ( (*textureWidth - PADDING * 2) / glyphWidth);
  *textureHeight = NextPowerOf2 ((( numGlyphs / h) + 1) * glyphHeight);
  *textureHeight = *textureHeight > maxTextSize ? maxTextSize : *textureHeight;
  totalMem = *textureWidth * *textureHeight;
  textMem = (unsigned char *) malloc (totalMem * sizeof (unsigned char)); 
  memset (textMem, 0, totalMem);
  glGenTextures (1, (GLuint*)&textID);
  glBindTexture (GL_TEXTURE_2D, textID);
  activeTextureID = textID;

  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

  glTexImage2D( GL_TEXTURE_2D, 0, GL_ALPHA, 
		*textureWidth, 
		*textureHeight, 
		0, GL_ALPHA, GL_UNSIGNED_BYTE, 
		textMem);  
  free (textMem);
  if (GL_Err()) 
      g_print ("Texture binding problem");
  return textID;
}

static void TextureFontMakeGlyphList(GL_font *font)
{
  int c, numTextures, remGlyphs;
  GLuint glTextureID[2048];
  int xOffset, yOffset;

  numTextures = 0;
  remGlyphs = (*(font->face))->num_glyphs;
  font->glyphList = (GL_glyph **) malloc ( sizeof (GL_glyph) * (*(font->face))->num_glyphs );
  for (c = 0; c < (*(font->face))->num_glyphs; c++)
    font->glyphList[c] = MakeTextureGlyph (font, c, &remGlyphs, 
					   &numTextures, glTextureID,
					   &xOffset, &yOffset);
}


static int TextureGlyphRender(GL_glyph *glyph, FT_Vector pen)
{
  GL_textureinfo *info;

  info = (GL_textureinfo *) glyph->data; 
  if (glyph->dimension.y && glyph->dimension.x)
    {
      if(activeTextureID != info->textureid)
	{
	  glBindTexture (GL_TEXTURE_2D, (GLuint) info->textureid);
	  activeTextureID = info->textureid;
	}
      glBegin (GL_QUADS);
      glTexCoord2f (info->uv[0].x, info->uv[0].y);
      glVertex2f (pen.x + glyph->pos.x,
		  pen.y + glyph->pos.y );  

      glTexCoord2f (info->uv[0].x, info->uv[1].y);
      glVertex2f (pen.x + glyph->pos.x,
		  pen.y + glyph->pos.y - glyph->dimension.y);

      glTexCoord2f (info->uv[1].x, info->uv[1].y);
      glVertex2f (pen.x + glyph->dimension.x + glyph->pos.x,
		  pen.y + glyph->pos.y - glyph->dimension.y);

      glTexCoord2f (info->uv[1].x, info->uv[0].y);
      glVertex2f (pen.x + glyph->dimension.x + glyph->pos.x,	
		  pen.y + glyph->pos.y );

      glEnd ();
    }
  return glyph->advance;
}

/*---------------------------------- 
   Switch over texture or bitmap font
   if hardware or software...
---------------------------------*/
static GL_glyph  *MakeGlyph(GL_font *font, unsigned int g)
{
  return MakeBitmapGlyph (font, g);
}

static void MakeGlyphList(GL_font *font)
{
#ifdef MESA
  return BitmapFontMakeGlyphList (font);
#else /* MESA */
  return TextureFontMakeGlyphList (font);
#endif/* MESA */
}

static int RenderGlyph (GL_glyph *glyph, FT_Vector pen)
{
#ifdef MESA
  return BitmapGlyphRender (glyph, pen);
#else /* MESA */
  return TextureGlyphRender (glyph, pen);
#endif/* MESA */
}

static void FontClose (GL_font *font)
{
  GL_glyph **glyphListPtr;
  int numglyphs, c;
#ifndef MESA
  GL_textureinfo *info;
#endif/* MESA */

  if (font)
    {
      numglyphs = (*(font->face))->num_glyphs;
      FT_Done_Face (*(font->face));
      glyphListPtr = font->glyphList;
      for (c = 0; c < numglyphs; c++)
	{
#ifndef MESA
	  info = (GL_textureinfo *) font->glyphList[c]->data; 
	  /* free textures */
	  if (glIsTexture (info->textureid))
	    glDeleteTextures (1, &(info->textureid));	    
#endif/* MESA */
	  if (font->glyphList[c] != NULL)
	    {
	      if (font->glyphList[c]->data != NULL)
		free (font->glyphList[c]->data);
	      free (font->glyphList[c]);
	    }
	}
      free (font->glyphList); 
      free (font);
      font = NULL;
    }
  
}
/*---------------------------- 
   Common Font handling 
------------------------------*/
static GL_font *FontOpen (const char* fontname)
{
  GL_font *font;
  int err;

  if (!init_done)
    FTLibraryInit ();
  font = (GL_font *) malloc (sizeof (GL_font));
  font->face = (FT_Face *) malloc (sizeof (FT_Face *));
  font->glyphList = NULL;
  err = FT_New_Face( FTlib, fontname, 0, font->face);
  if(err)
    {
      font->face = 0;
      return NULL;
    }
  return font; 
}


static void FontBBox (GL_font *font,
	  char* string,
	  float *llx, float *lly, 
	  float *llz, float *urx, 
	  float *ury, float *urz)
{
  unsigned int left, right;
  char *c; 
  FT_Vector kernAdvance;
  FT_BBox bbox;

  *llx = *lly = *llz = *urx = *ury = *urz = 0;
  c = string;
  left = FT_Get_Char_Index (*(font->face), *c);
  right = 0;
  while (*c)
    {
      if( !font->glyphList[left])
	  font->glyphList[left] = MakeGlyph (font, left);
      bbox = font->glyphList[left]->bbox;
      bbox.yMin = bbox.yMin >> 6;
      bbox.yMax = bbox.yMax >> 6;
      /* Lower extent */
      *lly = (*lly < bbox.yMin) ? *lly: bbox.yMin;
      /* Upper extent */
      *ury = (*ury > bbox.yMax) ? *ury: bbox.yMax;
      /* Width */
      right = FT_Get_Char_Index (*(font->face), *(c + 1));
      FaceKernAdvance (*(font->face), 
		       left, right, 
		       &kernAdvance);
      *urx += kernAdvance.x;
      ++c;
      left = right;
    }
  /* Final adjustments */
  left = FT_Get_Char_Index (*(font->face), *c);
  if( !font->glyphList[left])
      font->glyphList[left] = MakeGlyph (font, left);
  bbox = font->glyphList[left]->bbox;
  *llx = bbox.xMin >> 6;
  FaceKernAdvance (*(font->face), left, right, &kernAdvance);
  *urx -= kernAdvance.x - font->glyphList[left]->advance;
  *urx += bbox.xMax >> 6;
}

static int FontFaceSize (GL_font *font, const unsigned int size, const unsigned int res )
{
  unsigned int err;
  
  err = FT_Set_Char_Size (*(font->face), 0L, size * 64, res, res);
  /*res x_resolution, res, y_resolution*/
  if (err)
    return err;
  if (font->glyphList)
    free (font->glyphList);
  MakeGlyphList (font);
  return err;
}


/*------------------------------------------------------------------------
   FontCharMap : select Charpmap that can handle charactere code 
   to glyph index conversion 
   The Aim is mainly to get Unicode Charmaps
-------------------------------------------------------------------------*/
static int FontCharMap (GL_font *font, FT_Encoding encoding, char alphabet)
{  
  FT_CharMap  found = 0;
  FT_CharMap  charmap;
  int         n, err;
  int         my_platform_id, my_encoding_id;

  err = FT_Select_Charmap (*(font->face), encoding);
  if (!err)
    {
      /* Get a Unicode mapping for this font */      
      if (encoding == ft_encoding_unicode)
	{
	  /* Microsoft unicode*/
	  for ( n = 0; n < (*(font->face))->num_charmaps; n++ )
	    {
	      charmap = (*(font->face))->charmaps[n];
	      my_platform_id = charmap->platform_id;
	      my_encoding_id = charmap->encoding_id;
	      if ((my_platform_id == TT_PLATFORM_MICROSOFT) 
		  && (my_encoding_id == TT_MS_ID_UNICODE_CS)) 
		{
		  found = charmap;
		  break;
		}
	    }
	  if ( !found )
	    /* Apple unicode */
	    for ( n = 0; n < (*(font->face))->num_charmaps; n++ )
	      {
		charmap = (*(font->face))->charmaps[n];
		my_platform_id = charmap->platform_id;
		my_encoding_id = charmap->encoding_id;
	      if ( (my_platform_id == TT_PLATFORM_APPLE_UNICODE) 
		    && (my_encoding_id != TT_APPLE_ID_ISO_10646)) 
		{
		  found = charmap;
		  break;
		}
	      }     
	  if ( !found )
	    err = 1;
	  else
	    (*(font->face))->charmap = found;
	}
    }
  return err;
}

static int FontAscender (GL_font *font) 
{
  return ((*(font->face))->size->metrics.ascender >> 6);
}

static int FontDescender (GL_font *font)
{
  return ((*(font->face))->size->metrics.descender >> 6);
}

static float FontAdvance (GL_font *font, const char* string)
{
  const unsigned char* c = (unsigned char*)string;
  float width = 0;
  unsigned int left, right;
  FT_Vector kernAdvance;
 
  left = FT_Get_Char_Index (*(font->face), *c);
  while (*c)
    {
      if( !font->glyphList[left] )
	{
	  font->glyphList[left] = MakeGlyph (font, left);
	  /*here we can make a charmap index*/ 
	}
      right = FT_Get_Char_Index (*(font->face), *(c + 1));
      FaceKernAdvance (*(font->face), left, right, &kernAdvance);
      width += kernAdvance.x + font->glyphList[left]->advance;
      c++;
      left = right;
    }
  return width;
}


static int FontRender (GL_font *font, const char* string )
{
  FT_Vector pen;
  FT_Vector kernAdvance;
  int advance;
  unsigned int left, right;
  unsigned char* c = (unsigned char*) string;

  pen.x = 0; 
  pen.y = 0;  
  left = FT_Get_Char_Index (*(font->face), *c);
  if( !font->glyphList[left])
      font->glyphList[left] = MakeGlyph (font, left);
  while (*c)
    {
      right = FT_Get_Char_Index (*(font->face), *(c + 1));
      if( !font->glyphList[right] )
	font->glyphList[right] = MakeGlyph (font, right);
      FaceKernAdvance( *(font->face), left, right, &kernAdvance);
      advance = RenderGlyph (font->glyphList[left], pen); 
      kernAdvance.x = advance + kernAdvance.x;
      /*	kernAdvance.y = advance.y + kernAdvance.y; */
      pen.x += kernAdvance.x;
      pen.y += kernAdvance.y;
      left = right;
      c++;
    }
  return pen.x;
}


/* Generic Calls */ 
int gl_draw_text (void* gl_void_font, const char *str)
{    
  return FontRender ((GL_font *) gl_void_font, str);
}

void gl_font_delete (void *gl_void_font)
{ 
  FontClose ((GL_font *) gl_void_font);
}

int gl_font_height (void *gl_void_font)
{
  GL_font* gl_font = (GL_font *) gl_void_font;

  return (FontAscender(gl_font) - FontDescender(gl_font));
}

int gl_font_ascent (void *gl_void_font)
{
  return (FontAscender((GL_font *) gl_void_font));
}

int gl_font_char_width (void *gl_void_font, char c)
{
  char character_only[2];
   
  character_only[0] = c;
  character_only[1] = '\0';

  return (FontAdvance ((GL_font *) gl_void_font, character_only)); 
}

int gl_font_char_height (void *gl_void_font, char *c)
{
  float llx;            /*  The bottom left near most ?? in the x axis */
  float lly;            /*  The bottom left near most ?? in the y axis */
  float llz;            /* The bottom left near most ?? in the z axis */
  float urx;            /* The top right far most ?? in the x axis */
  float ury;            /*  The top right far most ?? in the y axis */
  float urz;            /* The top right far most ?? in the z axis */

   FontBBox ((GL_font *) gl_void_font, c, 
	     &llx, &lly, &llz, 
	     &urx, &ury, &urz);
   return ((int) (ury - lly));
}

int gl_font_char_ascent (void *gl_void_font, char *c)
{
  float llx, lly, llz, urx, ury, urz;          

  FontBBox ((GL_font *) gl_void_font, c, 
	    &llx, &lly, &llz, 
	    &urx, &ury, &urz);
  return ((int) ury);
}

/* query replace pixmap bitmap texture*/
void *gl_font_init (const char *font_filename, char alphabet, int size)
{
  GL_font* gl_font = NULL;
  int err;
  
  err= 0;
  gl_font = FontOpen (font_filename);
  if (gl_font != NULL)
    {        
      /* From the freetype docs... http://www.freetype.org/
	 "By default, when a new face object is created, 
	 (freetype) lists all the charmaps contained in the font face 
	 and selects the one that supports Unicode character codes 
	 if it finds one. 
	 Otherwise, it tries to find support for Latin-1, then ASCII."
	 Here are encoding possible value as a parameter :
	 ft_encoding_none	ft_encoding_symbol
	 ft_encoding_unicode	ft_encoding_latin_2
	 ft_encoding_sjis	ft_encoding_gb2312
	 ft_encoding_big5	ft_encoding_wansung
	 ft_encoding_johab	ft_encoding_adobe_standard
	 ft_encoding_adobe_expert ft_encoding_adobe_custom
	 ft_encoding_apple_roman 
      */	
      if (alphabet == 'G')
	{
	  err = FontCharMap (gl_font, ft_encoding_symbol, alphabet);
	}
      else 
	{
	  if (alphabet != 'L')
	    err = FontCharMap (gl_font, ft_encoding_unicode, alphabet);
	}
      if (err)
	{
	  FT_Done_Face (*(gl_font->face));
	  free (gl_font);
	  return NULL;
	}
      err = FontFaceSize (gl_font, size, 72);	
      if (err)
	{
	  FT_Done_Face (*(gl_font->face));
	  free (gl_font);
	  return NULL;
	}
      return (void *) gl_font; 
    }
  return NULL;
}
  

int UnicodeFontRender (void *gl_font, wchar_t *string )
{
  FT_Vector pen;
  FT_Vector kernAdvance;
  int advance;
  unsigned int left, right;
  GL_font* font;

  font = (GL_font *) gl_font;
  pen.x = 0; 
  pen.y = 0;
  left = FT_Get_Char_Index (*(font->face), (FT_ULong) *string);
  if (!font->glyphList[left])
      font->glyphList[left] = MakeGlyph (font, left);
  while (*string)
    {
      right = FT_Get_Char_Index (*(font->face), *(string + 1));
      if(!font->glyphList[right])
	font->glyphList[right] = MakeGlyph (font, right);
      FaceKernAdvance( *(font->face), left, right, &kernAdvance);
      advance = RenderGlyph (font->glyphList[left], pen); 
      kernAdvance.x = advance + kernAdvance.x;
      /*	kernAdvance.y = advance.y + kernAdvance.y; */
      pen.x += kernAdvance.x;
      pen.y += kernAdvance.y;
      left = right;
      string++;
    }
  return pen.x;
}

#endif /* _GL */
