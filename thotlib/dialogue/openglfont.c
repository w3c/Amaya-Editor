/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2002.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
/*
 * Handles Opengl Fonts with Freetype 2 
 * (http://www.freetype.org)
 * ( Here we make glyph to antialiased bitmap font )
 * Based on FTGLIB a very good C++ lib that handles fonts in opengl
 * ( http://homepages.paradise.net.nz/henryj/code/index.html )
 * 
 * Author: P. Cheyrou-lagreze (INRIA)
 *
 */
 
#ifdef _GL
 
#include "openglfonts.h"

/* Memory state Var needed often*/
static FT_Library FTlib = NULL;
static int init_done = 0;

/*--------------------------------------------------
 Freetype library Handling
---------------------------------------------------*/
static int FTLibraryInit ()
{
  int err;
  
  if (FTlib != 0 )
    return TRUE;
  err = FT_Init_FreeType( &FTlib);
  if (err)
    {
      FTlib = 0;
      return FALSE;
    }
  return TRUE;
}

/*--------------------------------------------------
  FTLibraryFree : Free the Freetype hamdle
---------------------------------------------------*/
void FTLibraryFree ()
{
  if (init_done)
    {
      FT_Done_FreeType (FTlib);
      FTlib= 0;
    }
}


/*--------------------------------------------------
  MakeAlphaBitmap : Make an Alpha bitmap of a glyph
  (Alpha is the glyph's transparency level that will 
  permits antialiasing)
--------------------------------------------------*/
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

/*-----------------------------------------------------------
  MakeBitmapGlyph : Make bitmap and struct to handle the glyph
-------------------------------------------------------------*/
static GL_glyph  *MakeBitmapGlyph (GL_font *font, unsigned int g)
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

  FT_Load_Glyph (*font->face, g, 
		 FT_LOAD_NO_HINTING
		 /*FT_LOAD_MONOCHROME*/
		 /*FT_LOAD_RENDER*/
		 );
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
	  BitmapGlyph = (GL_glyph *) malloc (sizeof (GL_glyph) );
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

  
/*---------------------------- 
  FontClose :
------------------------------*/
static void FontClose (GL_font *font)
{
  FreeGlyphList (font);
  FT_Done_Face (*(font->face));
  free (font->face);
  free (font);
  font = NULL;
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
/*---------------------------- 
   FontBBox : String bounding box
------------------------------*/
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
	  font->glyphList[left] = MakeBitmapGlyph (font, left);
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
      font->glyphList[left] = MakeBitmapGlyph (font, left);
  bbox = font->glyphList[left]->bbox;
  *llx = bbox.xMin >> 6;
  FaceKernAdvance (*(font->face), left, right, &kernAdvance);
  *urx -= kernAdvance.x - font->glyphList[left]->advance;
  *urx += bbox.xMax >> 6;
}
/*--------------------------------------------------- 
   FreeGlyphList : Free all glyph previously cached 
   and the array
------------------------------------------------------*/
static void FreeGlyphList (GL_font *font)
{
  GL_glyph **glyphListPtr;
  int numglyphs, c;

  if (font)
    {
      numglyphs = (*(font->face))->num_glyphs;      
      glyphListPtr = font->glyphList;
      for (c = 0; c < numglyphs; c++)
	{
	  if (font->glyphList[c] != NULL)
	    {
	      if (font->glyphList[c]->data != NULL)
		free (font->glyphList[c]->data);
	      free (font->glyphList[c]);
	    }
	}
      free (font->glyphList); 
      font->glyphList = NULL;
    }
}
/*--------------------------------------------------- 
   FontFaceSize : set char Size
------------------------------------------------------*/
static int FontFaceSize (GL_font *font, const unsigned int size, const unsigned int res )
{
  unsigned int err, c, num;
  
  err = FT_Set_Char_Size (*(font->face), 0, size * 64, res, res);
  /*res x_resolution, res, y_resolution*/
  if (err)
    return err;
  if (font->glyphList)
    FreeGlyphList (font);
  num = (*(font->face))->num_glyphs;
  font->glyphList = (GL_glyph **) malloc ( sizeof(GL_glyph *) * num );
  for (c=0 ; c < num; c++)
     font->glyphList[c] = NULL;
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

/*--------------------------------------------------- 
   FontAscender : return ascent
------------------------------------------------------*/
static int FontAscender (GL_font *font) 
{
  return ((*(font->face))->size->metrics.ascender >> 6);
}

/*--------------------------------------------------- 
   FontDescender :  return descent
------------------------------------------------------*/
static int FontDescender (GL_font *font)
{
  return ((*(font->face))->size->metrics.descender >> 6);
}


/*--------------------------------------------------
  FaceKernAdvance : Return Distance between 2 glyphs
---------------------------------------------------*/
static void FaceKernAdvance (FT_Face face, 
		      unsigned int index1, 
		      unsigned int index2, 
		      FT_Vector *kernAdvance)
{  
  int err;

  kernAdvance->x = 0;  
  kernAdvance->y = 0; 
  if (FT_HAS_KERNING(face) && index1 && index2)
    {
      err = FT_Get_Kerning (face, index1, index2, 
			    ft_kerning_default, 
			    /*ft_kerning_unfitted,*/ 
			    /*ft_kerning_unscaled,*/
			    kernAdvance);
      if (!err)
	{	
	  kernAdvance->x = kernAdvance->x >> 6; 
	  /*kernAdvance->y = kernAdvance->y >> 6;*/
	}
  }
}

/*--------------------------------------------------
  FontAdvance : Return advance of a string
  without displaying it (With Kerning)
Not used... for now

static float FontAdvance (GL_font *font, const char* string)
{
  const unsigned char* c = (unsigned char*)string;
  float width = 0;
  unsigned int left, right;
  FT_Vector kernAdvance;
 
  left = FT_Get_Char_Index (*(font->face), *c);
  while (*c)
    {
      if( !font->glyphList[left] ){
	  font->glyphList[left] = MakeBitmapGlyph (font, left);}
      right = FT_Get_Char_Index (*(font->face), *(c + 1));
      FaceKernAdvance (*(font->face), left, right, &kernAdvance);
      width += kernAdvance.x + font->glyphList[left]->advance;
      c++;
      left = right;
    }
  return width;
}
---------------------------------------------------*/


/* Generic Calls */ 

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
  GL_font* font = (GL_font *) gl_void_font;
  unsigned int glyph_index;
 
  glyph_index = FT_Get_Char_Index (*(font->face), c);
  if( !font->glyphList[glyph_index] )
    font->glyphList[glyph_index] = MakeBitmapGlyph (font, glyph_index);
  return (font->glyphList[glyph_index]->advance);
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


/*--------------------------------------------------
  gl_font_init : Load a font
---------------------------------------------------*/
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
      err = FontCharMap (gl_font, ft_encoding_unicode, alphabet);
      if (err)
	{
	  FT_Done_Face (*(gl_font->face));
	  free (gl_font);
	  return NULL;
	}
      err = FontFaceSize (gl_font, size, 0);	
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
/*--------------------------------------------------
  BitmapAppend : Add the right portion of a 
  bitmap at end of another one.
---------------------------------------------------*/
static void BitmapAppend (unsigned char *data, 
			  unsigned char *append_data,
			  unsigned int width, 
			  register int height,
			  unsigned int Width)
{  
  register int i = 0;
 
  while (height--)
    {      
      while (i < width)
	{
	  if (*(append_data + i))
	    *(data + i) = *(append_data + i);
	  i++;
	}
      i = 0;
      data += Width;
      append_data += width;
    }
}
/*--------------------------------------------------
  BitmapPrepend : Add a bitmap at the beginning of 
  another one
---------------------------------------------------*/
static void BitmapPrepend (unsigned char *data, 
			   unsigned char *prepend_data,
			   unsigned int width, 
			   register int height,
			   unsigned int Width)
{  
  while (height--)
    {      
      memcpy (data, prepend_data, Width); 
      data += Width;
      prepend_data += width;
    }
}

/*--------------------------------------------------------
  UnicodeFontRender : Render an unicode string in a Bitmap
  (That can be use as a texture)
---------------------------------------------------------*/
int UnicodeFontRender (void *gl_font, wchar_t *string, float x, float y, int size,
		       int TotalHeight)
{
  GL_font* font;
  GL_glyph *glyph;
  FT_Vector pen;
  FT_Vector kernAdvance;
  register int left, right;
  int Height, Width, maxy, miny, currenty;
  unsigned char *data, *data2;

  font = (GL_font *) gl_font;
  Height = ((*(font->face))->size->metrics.height >> 6);
  Width = ((*(font->face))->size->metrics.max_advance >> 6) * size;
  data = (unsigned char *) TtaGetMemory (sizeof (unsigned char)*(Height * 2) * Width);
  memset (data, 0, sizeof (unsigned char)*(Height * 2) * Width);
  miny = Height;
  maxy = 0;
  pen.x = 0;
  pen.y = 0;
  left = FT_Get_Char_Index (*(font->face), *string);
  if (!font->glyphList[left])
      font->glyphList[left] = MakeBitmapGlyph (font, left);
  while (*string)
    {
      glyph = font->glyphList[left];
      currenty = Height - glyph->pos.y;
      BitmapAppend (data + currenty*Width + pen.x + glyph->pos.x,
		   glyph->data,
		   glyph->dimension.x, 
		   glyph->dimension.y,
		   Width);
      miny = (miny < currenty) ? miny : currenty;
      currenty = currenty + glyph->dimension.y;
      maxy = (maxy > currenty) ? maxy : currenty; 
      right = FT_Get_Char_Index (*(font->face), *(string + 1));
      if (!font->glyphList[right])
	font->glyphList[right] = MakeBitmapGlyph (font, right);
      FaceKernAdvance (*(font->face), left, right, &kernAdvance);
      pen.x +=  glyph->advance + kernAdvance.x;
      left = right;
      string++;
    }
  if (glyph->advance < glyph->dimension.x)
    pen.x += glyph->dimension.x - glyph->advance;
  /*the string real's Conputed Heigth*/
  currenty = maxy - miny;
  data2 = (unsigned char *) TtaGetMemory (sizeof (unsigned char)
					  * currenty * pen.x); 
  y = y + (float) (Height - miny);
  Height = ((int) y) - TotalHeight;
  /* If rasterpos is out of the canvas 
   We must create a smaller bitmap that fits in*/
  if (Height > 0)
    {
      y = (float) TotalHeight;
      miny = miny + Height;
      currenty = maxy - miny;
      if (currenty <= 0)
	return pen.x;
    }      
  BitmapPrepend (data2, 
		 data + miny*Width,
		 Width, 
		 currenty,
		 pen.x);
  free (data);
  glRasterPos2f (x, y);   
  glDrawPixels (pen.x,
		currenty,
		GL_ALPHA,
		GL_UNSIGNED_BYTE,
		(const GLubyte *) data2);
  free (data2);
  return pen.x;
}
#endif /* _GL */
