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
				       int destWidth, 
				       int destHeight, 
				       int Pitch)
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
  FT_Glyph        Glyph;
  GL_glyph        *BitmapGlyph;
  unsigned   char *data;
  int             err;

  data = NULL;

  FT_Load_Glyph (*font->face, 
		 g, 
		 FT_LOAD_NO_HINTING
		 /*FT_LOAD_MONOCHROME*/
		 /*FT_LOAD_RENDER*/
		 );
  FT_Get_Glyph( (*(font->face))->glyph, &Glyph);
  if(Glyph)
    {
      /*Last parameter tells that we destroy font's bitmap
	So we MUST cache it    */
      err = FT_Glyph_To_Bitmap( &Glyph, 
				ft_render_mode_normal, 
				0, 
				1);
      if( err || ft_glyph_format_bitmap != Glyph->format)
	{
	  FT_Done_Glyph (Glyph);
	  return NULL; 	
	}
      else
	{
	  bitmap = (FT_BitmapGlyph) Glyph;
	  source = &bitmap->bitmap;
	  if (source->width && source->rows)
	      data = MakeAlphaBitmap (source->buffer, 
				      source->width, 
				      source->rows, 
				      source->pitch);  
	  BitmapGlyph = (GL_glyph *) malloc (sizeof (GL_glyph) );
	  FT_Glyph_Get_CBox (Glyph, ft_glyph_bbox_subpixels, &(BitmapGlyph->bbox));
	  BitmapGlyph->data = data;
	  BitmapGlyph->advance = Glyph->advance.x >> 16;
	  BitmapGlyph->pos.x = bitmap->left;
	  BitmapGlyph->pos.y = source->rows - bitmap->top;   
	  BitmapGlyph->dimension.x = source->width;
	  BitmapGlyph->dimension.y = source->rows;  	  
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
		      wchar_t *string,
		      float *llx, float *lly, 
		      float *llz, float *urx, 
		      float *ury, float *urz)
{
  unsigned int left, right;
  wchar_t *c; 
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
      *lly = (*lly < bbox.yMin) ? *lly: bbox.yMin;
      *ury = (*ury > bbox.yMax) ? *ury: bbox.yMax;
      right = FT_Get_Char_Index (*(font->face), *(c + 1));
      *urx += FaceKernAdvance (*(font->face), 
		       left, right);
      ++c;
      left = right;
    }
  left = FT_Get_Char_Index (*(font->face), *c);
  if( !font->glyphList[left])
      font->glyphList[left] = MakeBitmapGlyph (font, left);
  bbox = font->glyphList[left]->bbox;
  *llx = bbox.xMin >> 6;
  FaceKernAdvance (*(font->face), left, right);
  *urx -= FaceKernAdvance (*(font->face), left, right)
          - font->glyphList[left]->advance;
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

  
  /* Get a Unicode mapping for this font */      
  if (encoding == ft_encoding_unicode)
    {
      err = FT_Select_Charmap (*(font->face), encoding);
      if (!err)
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
	  if (!found )
	    err = 1;
	  else
	    (*(font->face))->charmap = found;
	}
    }
  /*   else */
  /*     { */
  /*       err = FT_Select_Charmap (*(font->face), ft_encoding_symbol);  */
  /*       if (err) */
  /* 	{ */
  /* 	  if (!(*(font->face))->charmap */
  /* 	      && FT_Select_Charmap ((*(font->face)), ft_encoding_apple_roman) */
  /* 	      && FT_Select_Charmap ((*(font->face)), ft_encoding_symbol)) */
  /* 	    FT_Set_Charmap ((*(font->face)), (*(font->face))->charmaps[0]); */
  /* 	} */  
  /*     } */
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
static float FaceKernAdvance (FT_Face face, 
		      unsigned int index1, 
		      unsigned int index2)
{  
  FT_Vector kernAdvance;

  kernAdvance.x = 0;  
  if (index1 && index2)
    {
      /*  kernAdvance->y = 0;  */
      FT_Get_Kerning (face, 
		      index1, index2, 
		      /*ft_kerning_default, */
		      /*ft_kerning_unfitted,*/ 
		      ft_kerning_unscaled,
		      &kernAdvance);
      return (kernAdvance.x >> 16); 
      /*kernAdvance->y = kernAdvance->y >> 6;*/
    }
  return (0x0);
}

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

int gl_font_char_width (void *gl_void_font, wchar_t c)
{
  GL_font* font = (GL_font *) gl_void_font;
  unsigned int glyph_index;
 
  glyph_index = FT_Get_Char_Index (*(font->face), c);
  if( !font->glyphList[glyph_index] )
    font->glyphList[glyph_index] = MakeBitmapGlyph (font, glyph_index);
  return (font->glyphList[glyph_index]->advance); 
  /* return ( (font->glyphList[glyph_index]->dimension.x > font->glyphList[glyph_index]->advance)? */
  /* 	  font->glyphList[glyph_index]->dimension.x */
  /* 	  : */
  /* 	  font->glyphList[glyph_index]->advance */
  /* 	  );  */
}

int gl_font_char_height (void *gl_void_font, wchar_t *c)
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

int gl_font_char_ascent (void *gl_void_font, wchar_t *c)
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
      if (alphabet == 'G')
	err = FontCharMap (gl_font, ft_encoding_symbol, alphabet);
      else
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
      if (FT_HAS_KERNING((*(gl_font->face))))
	gl_font->kerning = 1;
      else
	gl_font->kerning = 0;
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
	  if (*(append_data + i) > 30)
	    *(data + i) = *(append_data + i);
	  i++;
	}
      i = 0;
      data += Width;
      append_data += width;
    }
}

/*--------------------------------------------------------
  UnicodeFontRender : Render an unicode string 
  (no more than a word)
  in a Bitmap.
  By :
     1) computes glyphs position (advance and kerning).
     2) place them in a bitmap.
  
  That should/can be use as a texture ?
  
  TODO : Clean the mess between XWidth (bitmap size) and 
         Xpos (string size) but somme times 
         ("FULL" in italic for example), 
         bitmap size is bigger than string size...
---------------------------------------------------------*/
#define MAX_LENGTH 1024
int UnicodeFontRender (void *gl_font, wchar_t *string, float x, float y, int size,
		       int TotalHeight)
{
  GL_font* font;
  GL_glyph *glyph;
  float Xpos, Xpostest, XWidth;
  int left, right, i;
  int Height, Width, maxy, miny;
  unsigned char *data;
  GL_glyph      *bitmaps[MAX_LENGTH];
  FT_Vector     bitmap_pos[MAX_LENGTH];

  font = (GL_font *) gl_font;
  miny = 10000;
  maxy = 0;
  i = 0;
  Xpos = 0x0;
  left = FT_Get_Char_Index (*(font->face), *string); 
  /* left =  FT_Get_Char_Index (*(font->face), */
  /* 			    ((*(font->face))->charmap->encoding == ft_encoding_symbol) ?  */
  /* 			     *string | 0xf000: *string); */

  Xpos = 0;
  XWidth = 0;
  while (i < size)
    {
      if (!font->glyphList[left])
	font->glyphList[left] = MakeBitmapGlyph (font, left);
      glyph = font->glyphList[left];
      bitmaps[i] = glyph;
      bitmap_pos[i].x = ((int) Xpos);
      bitmap_pos[i].y = - glyph->pos.y;
      if (miny > bitmap_pos[i].y)
	miny = bitmap_pos[i].y;
      if ((glyph->dimension.y - glyph->pos.y) > maxy)
	maxy =  glyph->dimension.y - glyph->pos.y;
      Xpostest = glyph->advance;
      string++;
      if (*string)
	{
	  right = FT_Get_Char_Index (*(font->face), *string); 	
	  /* right = FT_Get_Char_Index (*(font->face), */
	  /* 			    ((*(font->face))->charmap->encoding ==  ft_encoding_symbol) ?  */
	  /* 			     *string | 0xf000: *string); */
  
	  if (font->kerning)
	    Xpostest += FaceKernAdvance (*(font->face), left, right);
	  left = right;
	}
      else
	left = 0;
      if (Xpostest)
	{
	  Xpos += Xpostest;
	  if (i == 0)
	    {
	      if (bitmaps[0]->pos.x > 0)
		Xpostest += bitmaps[0]->pos.x;
	    }
	  else
	    Xpostest += bitmaps[i]->pos.x;
	  if (glyph->dimension.x > glyph->advance)
	    XWidth += glyph->dimension.x - glyph->advance + Xpostest;
	  else
	    XWidth += Xpostest;
	}
      i++;
    } 
  Height = maxy - miny;
  
  Width = ((int) XWidth);
  data = (unsigned char *) TtaGetMemory (sizeof (unsigned char)
					  *Height*Width); 
  memset (data, 0, sizeof (unsigned char)*Height*Width);
  i = 0; 
  while (i < size)
    {
      if (i == 0)
	if (bitmaps[0]->pos.x > 0)
	  left = bitmaps[0]->pos.x;
	else
	  left = 0;
      else
	left = bitmaps[i]->pos.x;
      left = (bitmap_pos[i].y - miny)*Width + bitmap_pos[i].x + left;
      BitmapAppend (data + left,
		    bitmaps[i]->data,
		    bitmaps[i]->dimension.x,
		    bitmaps[i]->dimension.y,
		    Width);
      i++;
    }

  x = x  + (float) (bitmaps[0]->pos.x < 0 ? bitmaps[0]->pos.x : 0); 
  y -= (float) miny;

  if ((int)y > TotalHeight)
    {
      left = (int) y - TotalHeight;
      y = (float) TotalHeight;
    }
  else 
    left = 0;
  
  if ((int)x < 0)
    {
      right = -(int) x;
      x = (float) 0;
    }
  else 
    right = 0;
  
  glRasterPos2f (x, y);
  glBitmap (0, 0,
	    0, 0,
	    (float) -right, (float) -left,
	    NULL);
  glDrawPixels (Width,
		Height,
		GL_ALPHA,
		GL_UNSIGNED_BYTE,
		(const GLubyte *) data);
  glBitmap (0, 0,
	    0, 0,
	    (float) right, (float) left,
	    NULL);
  free (data);
  return (((int) Xpos) + (bitmaps[0]->pos.x < 0 ? bitmaps[0]->pos.x : 0) );
}

#endif /* _GL */
