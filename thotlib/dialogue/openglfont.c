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
 
#define THOT_EXPORT extern
#include "constmedia.h"
#include "typemedia.h"
/*For ttagetmemory and free*/
#include "memory_f.h"
/*for ttafileexists*/
#include "fileaccess.h"
#include "openglfonts.h"
#include "font_f.h"



/* Memory state Var needed often*/
static FT_Library FTlib = NULL;
static int init_done = 0;


/*--------------Font Caching---------------------------*/

/*Font Cache */
static Font_Slot FontTab[1024];

/*---------------------------- 
  FontAlreadyLoaded : Checks if font not already loaded
------------------------------*/
static GL_font* FontAlreadyLoaded (const char *font_filename, int size)
{
	int i;
	
	i = 0;
	while (i < 1023)
	{
		if (FontTab[i].ref)
		  if (size == FontTab[i].size)
			if (strcasecmp (font_filename, FontTab[i].name) == 0)			
			{
				FontTab[i].ref++;
				return FontTab[i].font;
			}
		i++;
	}
	return NULL;
}
/*---------------------------- 
  FontCache : Add font in cache
------------------------------*/
static void FontCache (GL_font *font, const char *font_filename, int size)
{
	int i;
	
	i = 0;
	while (FontTab[i].ref != 0 && i < 1023)
		i++;
	FontTab[i].font = font;
	FontTab[i].font->Cache_index = i;
	FontTab[i].size = size;
	FontTab[i].name = TtaGetMemory (strlen(font_filename) + 1);
	strcpy (FontTab[i].name, font_filename);
	FontTab[i].ref = 1;
}
/*---------------------------- 
 FreeFontCache :
------------------------------*/
static void FreeFontCache ()
{
	int i;
	
	i = 0;
	while (i < 1024)
	{
		if (FontTab[i].ref)
		{

			FontClose (FontTab[i].font);
			TtaFreeMemory (FontTab[i].name);
			FontTab[i].ref = 0;
			FontTab[i].size = 0;
		}
		i++;
	}
}
/*---------------------------- 
 FreeFontEntry :
------------------------------*/
static void FreeFontEntry (GL_font* font)
{
	int i;
	
	if (font == NULL) 
		return;
	i = font->Cache_index;
	if (FontTab[i].ref == 1)
	{			
	   FontClose (FontTab[i].font);
	   TtaFreeMemory (FontTab[i].name);
	   FontTab[i].name = NULL;
	   FontTab[i].font = NULL;
	   FontTab[i].ref = 0;
	   FontTab[i].size = 0;
	}
	else
	   FontTab[i].ref--;
	font = NULL;
}

/*--------------------------------------------------
 Freetype library Handling
---------------------------------------------------*/
static int FTLibraryInit ()
{
  int err;
  int i;

  if (FTlib != 0 )
    return TRUE;
  err = FT_Init_FreeType( &FTlib);
  if (err)
    {
      FTlib = 0;
      return FALSE;
    }
  i = 0;
  while (i < 1024)
  {
	FontTab[i].ref = 0;
	FontTab[i].name = NULL;
	FontTab[i].font = NULL;
	FontTab[i].size = 0;
	i++;
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
	  FreeFontCache();
	  FT_Done_FreeType (FTlib);
      FTlib= 0;
    }
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
			TtaFreeMemory (font->glyphList[c]->data);
	      TtaFreeMemory (font->glyphList[c]);
	    }		
	}
      TtaFreeMemory (font->glyphList); 
      font->glyphList = NULL;
    }
}


/*---------------------------- 
  FontClose :
------------------------------*/
static void FontClose (GL_font *font)
{
  FreeGlyphList (font);
  if (font->face)
  {
	if (*(font->face))
	   FT_Done_Face (*(font->face));
	TtaFreeMemory (font->face);
  }
  TtaFreeMemory (font);
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
  {
	  FTLibraryInit ();
	  init_done = TRUE;
  }

  if (TtaFileExist (fontname) == 0)
    return NULL;
  font = (GL_font *) TtaGetMemory (sizeof (GL_font));
  font->face = (FT_Face *) TtaGetMemory (sizeof (FT_Face *));
  font->glyphList = NULL;
  err = FT_New_Face( FTlib, fontname, 0, font->face);
  if(err)
    {
	  TtaFreeMemory (font->face);
	  TtaFreeMemory (font);
      font->face = 0;
      return NULL;
    }
  return font; 
}

/*--------------------------------------------------- 
   FontFaceSize : set char Size
------------------------------------------------------*/
static int FontFaceSize (GL_font *font, const unsigned int size, const unsigned int res )
{
  unsigned int err, c, num;

#ifdef _WINDOWS
  err = FT_Set_Char_Size (*(font->face), 0, size * 64, 96, 96);
#else 
  err = FT_Set_Char_Size (*(font->face), 0, size * 64, res, res);
#endif /*_WINDOWS*/
  /*res x_resolution, res, y_resolution*/
  if (err)
    return err;
  if (font->glyphList)
    FreeGlyphList (font);
  num = (*(font->face))->num_glyphs;
  font->glyphList = (GL_glyph **) TtaGetMemory ( sizeof(GL_glyph *) * num );
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

  err = 1;
  /* Get a Unicode mapping for this font */      
  if (encoding == ft_encoding_unicode)
    {
      err = FT_Select_Charmap (*(font->face), ft_encoding_unicode);
      if (err)
	{
	  /* Microsoft unicode*/
	  for (n = 0; n < (*(font->face))->num_charmaps; n++ )
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
	  if (!found )
	    /* Apple unicode */
	    for ( n = 0; n < (*(font->face))->num_charmaps; n++ )
	      {
		charmap = (*(font->face))->charmaps[n];
		my_platform_id = charmap->platform_id;
		my_encoding_id = charmap->encoding_id;
		if ((my_platform_id == TT_PLATFORM_APPLE_UNICODE) 
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
   else
      {
	   if (encoding == ft_encoding_symbol)
	   {
	     /*err = 0;*/
	     /*FT_Select_Charmap (*(font->face), ft_encoding_symbol);*/
	     err = 1;
		if (err)
		  {	 
		    if ((*(font->face))->num_charmaps > 0) 
		      { 
			FT_Set_Charmap( (*(font->face)), 
					(*(font->face))->charmaps[0]);
			return 0; 
		      } 
		    else 
		      return err;
		  }
	   }
	   else
		{
		   /* Esstix Charmap is unknown so we use the first charmap 
		   we found in the font itself */
		   if ((*(font->face))->num_charmaps > 0) 
			{ 
				FT_Set_Charmap( (*(font->face)), 
						(*(font->face))->charmaps[0]);
				return 0; 
			} 
		   else 
			 return err;
		}
      } 
   return err;
}

/*--------------------------------------------------
  gl_font_init : Load a font
  From the freetype docs... http://www.freetype.org/
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
---------------------------------------------------*/
void *gl_font_init (const char *font_filename, char alphabet, int size)
{
  GL_font* gl_font = NULL;
  int err;
  
  err= 0;
  gl_font = NULL;

  /* Checks if font not already loaded */
  gl_font = FontAlreadyLoaded (font_filename, size);
  if (gl_font)
    return (void *) (gl_font);
  gl_font = FontOpen (font_filename);
  if (gl_font != NULL)
    {   
      if (alphabet == 'G')
	FontCharMap (gl_font, ft_encoding_symbol, alphabet);
      else        
	if (alphabet == 'E')
	  FontCharMap (gl_font, ft_encoding_none, alphabet);/*err = 0;*/
	else
	  err = FontCharMap (gl_font, ft_encoding_unicode, alphabet);
      if (err) 
 	{ 
 	  FT_Done_Face (*(gl_font->face)); 
 	  TtaFreeMemory (gl_font); 
 	  return NULL; 
 	} 
      if (alphabet != 'E')
	err = FontFaceSize (gl_font, size, 0);	
      if (err)
	{
	  FT_Done_Face (*(gl_font->face));
	  TtaFreeMemory (gl_font);
	  return NULL;
	}
      if (FT_HAS_KERNING((*(gl_font->face))))
	gl_font->kerning = 1;
      else
	gl_font->kerning = 0;
      /*Cache font*/
      FontCache (gl_font, font_filename, size);
      return (void *) gl_font;
    }
  return NULL;
}


/************** Generic Calls *****************/ 

void gl_font_delete (void *gl_void_font)
{ 
   FreeFontEntry ((GL_font *) gl_void_font);
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
  GL_glyph *glyph;
  unsigned int glyph_index;
  int advance;

  glyph_index = FT_Get_Char_Index (*(font->face), c);

  if (font->glyphList)
    {
      if( !font->glyphList[glyph_index] )
	font->glyphList[glyph_index] = MakeBitmapGlyph (font, glyph_index);
      if (font->glyphList[glyph_index])
	return ((int) (font->glyphList[glyph_index]->advance));
      else 
	return 0;
    }
  else
    {
      glyph = MakeBitmapGlyph (font, glyph_index);
      if (glyph)
	{
	  advance = (int) glyph->advance;
	  if (glyph->data != NULL)
	    TtaFreeMemory (glyph->data);
	  TtaFreeMemory (glyph);
	  return advance;
	} 
      else 
	return 0;
    }
}

int gl_font_char_height (void *gl_void_font, wchar_t *c)
{
  float llx, lly, llz, urx, ury, urz;          
  
  FontBBox ((GL_font *) gl_void_font, c, 1,
	      &llx, &lly, &llz, 
	    &urx, &ury, &urz);
  return ((int) (ury - lly));
 
}

int gl_font_char_ascent (void *gl_void_font, wchar_t *c)
{
  float llx, lly, llz, urx, ury, urz;          
  
  FontBBox ((GL_font *) gl_void_font, c, 1,
	    &llx, &lly, &llz, 
	    &urx, &ury, &urz);
  return ((int) ury);
}

/**********FONT INFO ***********/
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
      FT_Get_Kerning (face, 
		      index1, index2, 
		      ft_kerning_unscaled,
		      &kernAdvance);
	  if (kernAdvance.x)
		return (float) (kernAdvance.x >> 16); 
    }
  return (0x0);
}

/*---------------------------- 
   FontBBox : String bounding box
------------------------------*/
static void FontBBox (GL_font *font,
		      wchar_t *string,
		      int length,
		      float *llx, float *lly, 
		      float *llz, float *urx, 
		      float *ury, float *urz)
{
  int left, right, i;
  wchar_t *c; 
  FT_BBox bbox;
  GL_glyph *glyph;

  glyph = NULL;
  *llx = *lly = *llz = *urx = *ury = *urz = 0;
  c = string;
  left = FT_Get_Char_Index (*(font->face), c[0]);
  right = 0;
  i = 0;
  while (i < length)
    {
      if (font->glyphList)
	{
	  if (!font->glyphList[left])
	    font->glyphList[left] = MakeBitmapGlyph (font, left);
	  bbox = font->glyphList[left]->bbox;
	}
      else
	{
	  glyph = MakeBitmapGlyph (font, left);
	  if (glyph)
	    {
	      bbox = glyph->bbox;
	      if (glyph->data != NULL)
		TtaFreeMemory (glyph->data);
	      TtaFreeMemory (glyph); 
	    }  
	}
      bbox.yMin = bbox.yMin >> 6;
      bbox.yMax = bbox.yMax >> 6;
      *lly = (*lly < bbox.yMin) ? *lly: bbox.yMin;
      *ury = (*ury > bbox.yMax) ? *ury: bbox.yMax;
      i++;
      right = FT_Get_Char_Index (*(font->face), c[i]);
      if (font->kerning)
	*urx += FaceKernAdvance (*(font->face), 
				 left, right);
      left = right;
    }
  left = FT_Get_Char_Index (*(font->face), c[i]);
  if (font->glyphList)
    {
      if( !font->glyphList[left])
	font->glyphList[left] = MakeBitmapGlyph (font, left);
      bbox = font->glyphList[left]->bbox;
    }
  else
    {
      glyph = MakeBitmapGlyph (font, left);
      if (glyph)
	bbox = glyph->bbox;
    }
  *llx = (float) (bbox.xMin >> 6);
  if (font->kerning)
    *urx -= FaceKernAdvance (*(font->face), left, right);
  if (font->glyphList)
    {
      *urx +=  font->glyphList[left]->advance;
    }
  else
    {
      *urx +=  glyph->advance;
      if (glyph)
	{
	  if (glyph->data != NULL)
	    TtaFreeMemory (glyph->data);
	  TtaFreeMemory (glyph);
	}
    }
  *urx += bbox.xMax >> 6;
}


/********OPENGL BITMAP CONSTRUCTION SET********/

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
  data = (unsigned char *) TtaGetMemory (destWidth * destHeight * sizeof (unsigned char));
  for (y = destHeight; y > 0; y--)
    {
      ptr = source + (y * destWidth - Pitch);
      for (x = 0; x < destWidth; x++)
	{
	  *(data + current) = *(ptr++);
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
  if (Glyph)
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
	  BitmapGlyph = (GL_glyph *) TtaGetMemory (sizeof (GL_glyph) );
	  FT_Glyph_Get_CBox (Glyph, ft_glyph_bbox_subpixels, &(BitmapGlyph->bbox));
	  BitmapGlyph->data = data;
	  BitmapGlyph->advance = (float) (Glyph->advance.x >> 16);
	  BitmapGlyph->pos.x = bitmap->left;
	  BitmapGlyph->pos.y = source->rows - bitmap->top;   
	  BitmapGlyph->dimension.x = source->width;
	  BitmapGlyph->dimension.y = source->rows;  	  
	  FT_Done_Glyph (Glyph);
	  return BitmapGlyph;
	}
    }
  BitmapGlyph = (GL_glyph *) TtaGetMemory (sizeof (GL_glyph) );
  BitmapGlyph->data = NULL;
  BitmapGlyph->advance = 0;
  BitmapGlyph->pos.x = 0;
  BitmapGlyph->pos.y = 0;   
  BitmapGlyph->dimension.x = 0;
  BitmapGlyph->dimension.y = 0;  
  return BitmapGlyph;	   
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
  register unsigned int i = 0;
 
  if (data && append_data)
    while (height)
      {      
	while (i < width)
	  {
	    if (*(append_data + i) > 15)
	      *(data + i) = *(append_data + i);
	    i++;
	  }
	i = 0;
	data += Width;
	append_data += width;
	height--;
      }
}


/***************** FONT RENDERING *************/
/*-------------------------------------
 UnicodeFontRenderCharSize :
--------------------------------------*/
int UnicodeFontRenderCharSize (void *gl_font, wchar_t c, 
			       float x, float y, int size,
			       int TotalHeight)
{
  GL_font* font;
  GL_glyph *glyph;
  int left, right, i;   
  unsigned char test = (unsigned char) c;

  i = 0;
  font = (GL_font *) gl_font;
  
  i = FT_Get_Char_Index (*(font->face), test); 
  if (i == 0)
    i = FT_Get_Char_Index (*(font->face), c); 
  if (i != 0)
    {
      if (size > 0)
	{
	  FontFaceSize (font, size, 0);
	  glyph = MakeBitmapGlyph (font, i);
	}
      else
	{
	  if (!font->glyphList[i])
	    font->glyphList[i] = MakeBitmapGlyph (font, i);
	  glyph = font->glyphList[i];
	}
  /* If y > height or x < 0 
     Opengl doesn't draw bitmap 
     with his clipping mechanism
     so we must translate them*/

  y += glyph->pos.y;
  x += glyph->pos.x;

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

  
  /* Position inside the canvas*/
  glRasterPos2f (x, y);
  /*Translation for outside clipping strings*/
  glBitmap (0, 0,
	    0, 0,
	    (float) -right, 
	    (float) -left,
	    NULL);
  glDrawPixels ((int) glyph->dimension.x,
		        (int) glyph->dimension.y,
		        GL_ALPHA,
		        GL_UNSIGNED_BYTE,
		        (const GLubyte *) glyph->data);
 

  /*We restore originale translation state*/
  glBitmap (0, 0,
	        0, 0,
	        (float) right, 
	        (float) left,
	        NULL);
  i = (int) glyph->advance;

  if (size > 0)
    {
      if (glyph->data != NULL)
	TtaFreeMemory (glyph->data);
      TtaFreeMemory (glyph);
    }

  return (i);
  }
  else
	return 0;
}
  
/*----------------------------------------------------------------------
  DrawMonoSymb draw a one glyph symbol.
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void StixFontRenderCharSize (PtrFont font, CHAR_T symb, 
			     int x, int y, 
			     int size, int l, int h,  
			     int TotalHeight)
{
  GL_font* glfont;
  CHAR_T symbols[2];
  
  symbols[0] = symb;
  symbols[1] = '\0';

  glfont = (GL_font*) font;
  if (size > 0)
    FontFaceSize (glfont, size, 0);

  
  x  = x + ((l - gl_font_char_width (font, symb)) / 2);
        
  y  = y + (((h - gl_font_char_height (font, symbols)) / 2) 
	    + gl_font_char_ascent (font, symbols));
  
   UnicodeFontRenderCharSize (font, symb, (float) x, (float) y, size,
		       TotalHeight);
}


/*--------------------------------------------------------
  UnicodeFontRender : Render an unicode string 
  (no more than a word)
  in a Bitmap.
  Using Two step computation :
     1) computes each glyphs position in the word (advance and kerning).
     2) place them in a bitmap.
  
  That should/can be use as a texture ?
  
  TODO : Clean the mess between XWidth (bitmap size) and 
         Xpos (string size) but somme times 
         ("FULL" in italic for example), 
         bitmap size is bigger than string size...
---------------------------------------------------------*/
#ifndef NEWKERN
int UnicodeFontRender (void *gl_font, wchar_t *string, float x, float y, int size,
		       int TotalHeight)
{

  GL_font*			font;
  GL_glyph			*glyph;
  float				Xpos, Xpostest, XWidth,	maxy, miny;
  int               left, right, i, Height, Width;

  unsigned char     *data;
  GL_glyph          *bitmaps[MAX_LENGTH];
  FLOAT_VECTOR       bitmap_pos[MAX_LENGTH];        
  wchar_t            *c;

  c = string;
  font = (GL_font *) gl_font;
  miny = 10000;
  maxy = 0;
  i = 0;
  Xpos = 0x0;
  left = FT_Get_Char_Index (*(font->face), string[0]); 
  Xpos = 0;
  XWidth = 0;
  glyph = NULL;

 
  /* First We conmpute all character size (width and height)
   and their respective placement in the string */
  while (i < size)
    {
      if (left != 0)
	{
	  if (!font->glyphList[left])
	    font->glyphList[left] = MakeBitmapGlyph (font, left);
	  glyph = font->glyphList[left];
	  bitmaps[i] = glyph;
	  bitmap_pos[i].x = Xpos;
	  bitmap_pos[i].y = (float) ( - glyph->pos.y );
	  if (miny > bitmap_pos[i].y)
	    miny = bitmap_pos[i].y;
	  if ((glyph->dimension.y - glyph->pos.y) > maxy)
	    maxy =  (float) (glyph->dimension.y - glyph->pos.y);
	  Xpostest = glyph->advance;
	}
      else
	Xpostest = 0;
      /* distance between chars
	 (aka kerning) */
      i++;
      if (string[i])
	{
	  right = FT_Get_Char_Index (*(font->face), string[i]); 	  
	  if (font->kerning)
	    Xpostest += FaceKernAdvance (*(font->face), left, right);
	  left = right;
	}
      else
	left = 0;
      /* I cannot decide on which width 
	 is the more precise, normally,
	 Xpos can do the job, but some string are
	 wider so */
      if (Xpostest)
	{
	  Xpos += Xpostest;
	  /* First char can have negative X 
	   so we store it for use after string creation
	  (in order to avoid segfault...)*/

	  if ((i-1) == 0)
	    {
	      if (bitmaps[0])
		if (bitmaps[0]->pos.x > 0)
		  Xpostest += bitmaps[0]->pos.x;
	    }
	  else
	    Xpostest += bitmaps[i-1]->pos.x;

	  if (glyph->dimension.x > glyph->advance)
	    XWidth += glyph->dimension.x - glyph->advance + Xpostest;
	  else
	    XWidth += Xpostest;
	}
    } 
  Height = (int) (maxy - miny);
  Width = (int) XWidth;
  if (Height <= 0 || Width <= 0 || miny == 10000)
    return 0;
  data = (unsigned char *) TtaGetMemory (sizeof (unsigned char)
					  *Height*Width); 
  memset (data, 0, sizeof (unsigned char)*Height*Width);
  i = 0; 
  while (i < size)
    {
      if (i == 0)
	if (bitmaps[0])
	  if (bitmaps[0]->pos.x > 0)
	    left = bitmaps[0]->pos.x;
	  else
	    left = 0;
	else
	  left = 0;
      else
	left = bitmaps[i]->pos.x;
      left = (int) ((bitmap_pos[i].y - miny)*Width + bitmap_pos[i].x + left);
      BitmapAppend (data + left,
		    bitmaps[i]->data,
		    bitmaps[i]->dimension.x,
		    bitmaps[i]->dimension.y,
		    Width);
      i++;
    }
  if (bitmaps[0])
    x = x  + (float) (bitmaps[0]->pos.x < 0 ? bitmaps[0]->pos.x : 0); 

  y -= (float) miny;

  /* If y > height or x < 0 
     Opengl doesn't draw bitmap 
     due to his clipping mechanism
     so we must translate them*/
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
  
  /* Position inside the canvas*/
  glRasterPos2f (x, y);
  /*Translation for outside clipping strings*/
  glBitmap (0, 0,
	    0, 0,
	    (float) -right, 
	    (float) -left,
	    NULL);
  glDrawPixels (Width,
		Height,
		GL_ALPHA,
		GL_UNSIGNED_BYTE,
		(const GLubyte *) data);
  /*We restore originale translation state*/
  glBitmap (0, 0,
	    0, 0,
	    (float) right, 
	    (float) left,
	    NULL);
  if (data)
    TtaFreeMemory (data);

  return (((int) Xpos) + (bitmaps[0]->pos.x < 0 ? bitmaps[0]->pos.x : 0));  
}

#else /*NEWKERN*/

/*----------------------------------------------------------------------
  p2 :  Lowest power of two bigger than the argument.
  ----------------------------------------------------------------------*/
/*All these work on UNSIGNED BITS !!! 
 if little-big endianess is involved,
 all those atre wrong !!!*/

#define lowest_bit(x) (x & -x)
#define is_pow2(x) (x != 0 && x == lowest_bit(x))

static int ceil_pow2_minus_1(unsigned int x)
{
  unsigned int i;
  
  for (i=1; i; i <<= 1)
    x |= x >> i;
  return x;
}
#define p2(p) (is_pow2(p)?p:ceil_pow2_minus_1((unsigned int) p) + 1)
  
/* Don't know exactly wich is faster...
 this on is pretty good too...*/

/* 
int p2(p){
p -= 1; 
p |= p >> 16; 
p |= p >> 8; 
p |= p >> 4; 
p |= p >> 2; 
p |= p >> 1; 
return p + 1;

otherwise, identical, formulical,
but VERY VERY VERY slower 
(int to float, log, ceil, and finally float to int...)
return 1 << (int) ceilf(logf((float) p) / M_LN2);
} 
*/

/*----------------------------------------------------------------------
  GL_MakeTextureSize : Texture sizes must be power of two
  ----------------------------------------------------------------------*/
static void GL_MakeTextureSize(unsigned char *PicPixmap, 
			       int PicWidth, int PicHeight,
			       int GL_w, int GL_h)
{
  unsigned char      *data, *ptr1, *ptr2;
  int                 xdiff, x, y, nbpixel;

  if (PicPixmap != None &&
      !(PicWidth == GL_w &&
       PicHeight == GL_h))
	{
	data = TtaGetMemory (sizeof (unsigned char) * GL_w * GL_h);
	/* Black transparent filling */
	memset (data, 0, sizeof (unsigned char) * GL_w * GL_h);
	ptr1 = PicPixmap;
	ptr2 = data;
	xdiff = (GL_w - PicWidth) * nbpixel;
	x = nbpixel * PicWidth;
	for (y = 0; y < PicHeight; y++)
	    {
		/* copy R,G,B,A */
		memcpy (ptr2, ptr1, x); 
		/* jump over the black transparent zone*/
		ptr1 += x;
		ptr2 += x + xdiff;
		}	
	TtaFreeMemory (PicPixmap);
	PicPixmap = data;
  }
}


/*----------------------------------------------------------------------
 GL_TextureMap : map texture on a Quad (sort of a rectangle)
 Drawpixel Method for software implementation, as it's much faster for those
 Texture Method for hardware implementation as it's faster and better.
  ----------------------------------------------------------------------*/
static void GL_TextureMap (unsigned char *Image, 
			   int xFrame, int yFrame, 
			   int width, int height)
{  
  GLfloat   GL_w, GL_h;   
  GLint		Mode;
  
  GL_w = p2(width);
  GL_h = p2(height);

  glEnable (GL_TEXTURE_2D); 
  
/* Put texture in 3d card memory */
  /*
    if (!glIsTexture (Image->TextureBind))
    {
*/      

  /*
    glGenTextures (1, &(Image->TextureBind));
    glBindTexture (GL_TEXTURE_2D, Image->TextureBind);
  */
  
      /*TEXTURE ZOOM : GL_NEAREST is fastest and GL_LINEAR is second fastest*/
      glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      /* How texture is mapped... initially GL_REPEAT
	 GL_REPEAT, GL_CLAMP, GL_CLAMP_TO_EDGE are another option.. Bench !!*/	    
      glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP); 
      /* does current Color modify texture no = GL_REPLACE, 
	 else => GL_MODULATE, GL_DECAL, ou GL_BLEND */
      glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
      /* We give te texture to opengl Pipeline system */	    
     
      GL_MakeTextureSize (Image, 
			  width, height,
			  GL_w, GL_h);	

      glTexImage2D (GL_TEXTURE_2D, 
		    0, 
		    GL_ALPHA,
		    GL_w, 
		    GL_h, 
		    0,
		    Mode, 
		    GL_UNSIGNED_BYTE, 
		    (GLvoid *) Image);
      /*
	}
  else
    {
      glBindTexture(GL_TEXTURE_2D, Image->TextureBind);
    }
      */

  /* We have resized the picture to match a power of 2
     We don't want to see all the picture, just the w and h 
     portion*/
  GL_w = (GLfloat) width/GL_w;
  GL_h = (GLfloat) height/GL_h;   	
  /* Not sure of the vertex order 
     (not the faster one, I think) */
  glBegin (GL_QUADS);
  /* Texture coordinates are unrelative 
     to the size of the square */      
  /* lower left */
  glTexCoord2i (0,    0); 
  glVertex2i (xFrame,     yFrame + height);
  /* upper right*/
  glTexCoord2f (GL_w, 0.0); 
  glVertex2i (xFrame + width, yFrame + height);
  /* lower right */
  glTexCoord2f (GL_w, GL_h); 
  glVertex2i (xFrame + width, yFrame); 
  /* upper left */
  glTexCoord2f (0.0,  GL_h); 
  glVertex2i (xFrame, yFrame);      
  glEnd ();	
  /* State disabling */
  glDisable (GL_TEXTURE_2D); 
}


int UnicodeFontRender (void *gl_font, wchar_t *text, 
		       float x, float y, 
		       int size,
		       int TotalHeight)
{
  FLOAT_VECTOR       bitmap_pos[MAX_LENGTH];   
  FT_Vector          delta;   
  FT_Face            face;
  FT_Bool            use_kerning;
  FT_UInt            previous, glyph_index;

  GL_glyph           *bitmaps[MAX_LENGTH];
  GL_font            *font;
  GL_glyph	     *glyph;

  unsigned char     *data;
  float		    maxy, miny;
  int               left, right, Width, Height;
  int               pen_x, n;


                          /*ft_kerning_unscaled*/
                          /*ft_kerning_default,*/
			  /*ft_kerning_unfitted*/
#define KERNING_CHOICE  ft_kerning_default

  font = (GL_font *) gl_font;
  face = *(font->face);
  use_kerning = font->kerning;
  previous = 0;
  miny = 10000;
  maxy = 0;
  pen_x = 0;

  for ( n = 0; n < size; n++ )
      {
        /* convert character code to glyph index*/
        glyph_index = FT_Get_Char_Index ( face, text[n] );

        /* retrieve kerning distance and move pen position*/
        if ( use_kerning && previous && glyph_index )
        {
          FT_Get_Kerning (face, 
			  previous, glyph_index,
			  KERNING_CHOICE,
			  &delta);

          pen_x += delta.x >> 6;
        }

	/* Load glyph image into the cache */
	if (!font->glyphList[glyph_index])
	    font->glyphList[glyph_index] = MakeBitmapGlyph (font, glyph_index);
	glyph = font->glyphList[glyph_index];
	if (glyph)
	  {
	    /* Position of the glyph in the texture*/
	    bitmap_pos[n].x = pen_x + glyph->pos.x;
	    bitmap_pos[n].y = (float) ( - glyph->pos.y );
	    bitmaps[n] = glyph;

	    /* Compute The height of the Texture*/
	    if (miny > bitmap_pos[n].y)
	      miny = bitmap_pos[n].y;
	    if ((glyph->dimension.y - glyph->pos.y) > maxy)
	      maxy =  (float) (glyph->dimension.y - glyph->pos.y);
	    
	    /* increment pen position*/
	    pen_x += glyph->advance;
	  }
	else
	  {
	    bitmap_pos[n].x = pen_x;
	    bitmap_pos[n].y = 0;
	    bitmaps[n] = 0;
	  }
	/* record current glyph index*/
        previous = glyph_index;
      }
  /* retrieve kerning distance and move pen position*/
  if ( use_kerning && previous && glyph_index )
    {
      FT_Get_Kerning (face, 
		      previous, 0,
		      KERNING_CHOICE,
		      &delta);
          pen_x += delta.x >> 6;
    }
  /*  
  for ( n = 0; n < size; n++ )
    {
      glRasterPos2f (x + bitmap_pos[n].x, 
		     y - bitmap_pos[n].y);
      glDrawPixels (bitmaps[n]->dimension.x,
		    bitmaps[n]->dimension.y,
		    GL_ALPHA,
		    GL_UNSIGNED_BYTE,
		    (const GLubyte *) bitmaps[n]->data);
    }
  return (pen_x);
  */

  //Height = (int)  (p2 (((maxy - miny)*10)));
  Height = (int)  (maxy - miny)*10;
  Width = (int)  (p2 ((pen_x*10)));

  if (Height <= 0 || Width <= 0 || miny == 10000)
    return 0;

  data = (unsigned char *) TtaGetMemory (sizeof (unsigned char)
					  *Height*Width); 
  left = 0;

  memset (data, 25, sizeof (unsigned char)*Height*Width);

   /* Load glyph image into the texture */
   for ( n = 0; n < size; n++ )
      {

        left = (int) ((bitmap_pos[n].y - miny)*Width 
		    + bitmap_pos[n].x);
	if (bitmaps[n])
	  {
	  
	g_print ("\n%i H:%i W : %i, Pixels : %i \n\t w: %i h: %i", 
		 left, Height, Width, Height*Width,
		 (int) bitmaps[n]->dimension.x,
		 (int) bitmaps[n]->dimension.y);

	  BitmapAppend (data + left,
			bitmaps[n]->data,
			(int) bitmaps[n]->dimension.x,
			(int) bitmaps[n]->dimension.y,
			Width);
	  }
      }

   y -= (float) miny;

   GL_TextureMap (data, x, y, Width, Height);

#ifdef NOWAY
  /* If y > height or x < 0 
     OpenGL doesn't draw bitmap 
     due to his clipping mechanism
     so we must translate them*/
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
  


  /* Position inside the canvas*/
  glRasterPos2f (x, y);
  /*Translation for outside clipping strings*/
  glBitmap (0, 0,
	    0, 0,
	    (float) -right, 
	    (float) -left,
	    NULL);
  glDrawPixels (Width,
		Height,
		GL_ALPHA,
		GL_UNSIGNED_BYTE,
		(const GLubyte *) data);
  /*We restore originale translation state*/
  glBitmap (0, 0,
	    0, 0,
	    (float) right, 
	    (float) left,
	    NULL);

#endif /*NOWAY*/

  if (data)
    TtaFreeMemory (data);

  return (pen_x);  
}
#endif /*NEWKERN*/
#endif /* _GL */
