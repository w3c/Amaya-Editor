/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2003
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
#include "typebase.h"
/*For ttagetmemory and free*/
#include "memory_f.h"
/*for ttafileexists*/
#include "fileaccess.h"
#include "openglfonts.h"
#include "font_f.h"
#include "glwindowdisplay.h"


#ifdef _SUPERS
/* 
   Supposed to give 
   better antialiasing
   but give more aliased chars..
*/
#define SUPERSAMPLING(X)  (X / 2)
#define SUPERSAMPLINGMUL(X) (X * 2)

#else

#define SUPERSAMPLING(X)  (X)
#define SUPERSAMPLINGMUL(X) (X)

#endif /*_SUPERS*/

#define ANTIALIASINGDEPTH 0



/*ft_kerning_unscaled*/
/*ft_kerning_default,*/
/*ft_kerning_unfitted*/
#define KERNING_CHOICE  ft_kerning_default

/* Memory state Var needed often*/
static FT_Library FTlib = NULL;
static int init_done = 0;


/*--------------Font Caching---------------------------*/

/*Font Cache */
static Font_Slot FontTab[1024];

/*---------------------------- 
  FontAlreadyLoaded : Checks if font not already loaded
------------------------------*/
static GL_font* FontAlreadyLoaded (const char *font_filename,
				   int size)
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
/*----------------------------------------------------------
  GetFirstFont  : Get anyway a font, 
  best is according to size
------------------------------------------------------------*/
void  *GetFirstFont (int size)
{
  int i;

  i = 0;
  while (i < 1023)
    {
      if (FontTab[i].ref)
	if (size == FontTab[i].size)
	  {
	    return FontTab[i].font;
	  }
      i++;
    }
  i = 0;
  while (i < 1023)
    {
      if (FontTab[i].ref)
	return FontTab[i].font;
      i++;
    }
  return NULL;
}
/*---------------------------- 
  FontCache : Add font in cache
------------------------------*/
static void FontCache (GL_font *font,
		       const char *font_filename,
		       int size)
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
 Char index Cache freeing recursive function 
 ---------------------------------------------------*/
static void FreeACharCache (Char_Cache_index *Cache)
{
  if (Cache->next)
    FreeACharCache (Cache->next);
  if (Cache->glyph.data)
    TtaFreeMemory (Cache->glyph.data);
  TtaFreeMemory (Cache);  
}

/*--------------------------------------------------
 Char index lookup 
 ---------------------------------------------------*/
static GL_glyph *Char_index_lookup_cache (GL_font *font,
					  unsigned int idx,
					  unsigned int *glyph_index)
{
  Char_Cache_index *Cache;

  Cache = font->Cache;
  if (Cache)
    while (1)
      {
	if (Cache->index == idx)
	  {
	    *glyph_index = Cache->character;
	    return (&Cache->glyph);  
	  }
	
	if (Cache->next)
	  Cache = Cache->next;      
	else break;      
      }

  if (Cache)
    {
      Cache->next = TtaGetMemory (sizeof (Char_Cache_index));
      Cache = Cache->next;  
    }
  else 
    {
      font->Cache = TtaGetMemory (sizeof (Char_Cache_index));
      Cache = font->Cache;      
    }
  Cache->index = idx;
  Cache->character = FT_Get_Char_Index (font->face, 
				       idx);
  Cache->next = NULL;  
  MakeBitmapGlyph (font,
		   Cache->character,
		   &Cache->glyph);
  
  *glyph_index = Cache->character;
  return (&Cache->glyph);  
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



/*---------------------------- 
  FontClose :
------------------------------*/
static void FontClose (GL_font *font)
{
  if (font->Cache)
    FreeACharCache (font->Cache);
  if (font->face)
      FT_Done_Face (font->face);
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
  err = FT_New_Face (FTlib, fontname, 0, &(font->face));
  if(err)
    {
      TtaFreeMemory (font);
      return NULL;
    }
  return font; 
}




/*--------------------------------------------------- 
   FontFaceSize : set char Size
------------------------------------------------------*/
static int FontFaceSize (GL_font *font,
			 unsigned int size,
			 unsigned int res)
{
  unsigned int err;

  if (size == font->size)
    return 0;
  size = SUPERSAMPLINGMUL (size);
#ifdef _WINDOWS
  err = FT_Set_Char_Size (font->face, 0, size * 64, 96, 96);
#else 
  err = FT_Set_Char_Size (font->face, 0, size * 64, res, res);
#endif /*_WINDOWS*/
  /*res x_resolution, res, y_resolution*/
  if (err)
    return err;
  font->size = size;
  return err;
}


/*------------------------------------------------------------------------
   FontCharMap : select Charpmap that can handle charactere code 
   to glyph index conversion 
   The Aim is mainly to get Unicode Charmaps
-------------------------------------------------------------------------*/
static int FontCharMap (GL_font *font, FT_Encoding encoding)
{  
  FT_CharMap  found = 0;
  FT_CharMap  charmap;
  int         n, err;
  int         my_platform_id, my_encoding_id;

  err = 1;
  /* Get a Unicode mapping for this font */      
  if (encoding == ft_encoding_unicode)
    {
      err = FT_Select_Charmap (font->face, ft_encoding_unicode);
      if (err)
	{
	  /* Microsoft unicode*/
	  for (n = 0; n < (font->face)->num_charmaps; n++ )
	    {
	      charmap = (font->face)->charmaps[n];
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
	    for ( n = 0; n < (font->face)->num_charmaps; n++ )
	      {
		charmap = (font->face)->charmaps[n];
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
	    (font->face)->charmap = found;
	}
    }
   else
      {
	   if (encoding == ft_encoding_symbol)
	   {
	     err = FT_Select_Charmap (font->face, 
				      ft_encoding_symbol);
	     if (err)
	       {	 
		 if ((font->face)->num_charmaps > 0) 
		   { 
		     FT_Set_Charmap( (font->face), 
				     (font->face)->charmaps[0]);
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
		   if ((font->face)->num_charmaps > 0) 
			{ 
				FT_Set_Charmap( (font->face), 
						(font->face)->charmaps[0]);
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
void *gl_font_init (const char *font_filename,
		    char alphabet, int size)
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
	FontCharMap (gl_font, ft_encoding_symbol);
      else        
	if (alphabet == 'E')
	  FontCharMap (gl_font, ft_encoding_none);/*err = 0;*/
	else
	  err = FontCharMap (gl_font, ft_encoding_unicode);
      if (err) 
 	{ 
 	  FT_Done_Face (gl_font->face); 
 	  TtaFreeMemory (gl_font); 
 	  return NULL; 
 	}
      gl_font->size = 0;
      err = FontFaceSize (gl_font, (unsigned int)size, 0);
      if (err)
	{
	  FT_Done_Face (gl_font->face);
	  TtaFreeMemory (gl_font);
	  return NULL;
	}

      /********Antialias**************
      if (FT_HAS_KERNING(gl_font->face))
	gl_font->kerning = 1;
      else
	gl_font->kerning = 0;
      TODO*/
      gl_font->kerning = 0;

      gl_font->ascent = (gl_font->face->size->metrics.ascender >> 6);
      gl_font->height = gl_font->ascent - FontDescender (gl_font);
      
      /*Cache font And description*/
      FontCache (gl_font, font_filename, size);
      /*init characters cache*/
      gl_font->Cache = NULL;
      return (void *) gl_font;
    }
  return NULL;
}

/************** Generic Calls *****************/ 

void GL_Font_Change_Height (void *font, int size)
{
  FontFaceSize ((GL_font *) font,
		(unsigned int) size,
		0);
}

void gl_font_delete (void *gl_void_font)
{ 
  FreeFontEntry ((GL_font *) gl_void_font);
}

int gl_font_height (void *gl_void_font)
{

  return  SUPERSAMPLING (((GL_font *) gl_void_font)->height);
  
}

int gl_font_ascent (void *gl_void_font)
{
  return SUPERSAMPLING (((GL_font *) gl_void_font)->ascent);
}

int gl_font_char_width (void *gl_void_font, wchar_t c)
{
  GL_font* font = (GL_font *) gl_void_font;
  unsigned int glyph_index;
    
  return SUPERSAMPLING ((Char_index_lookup_cache (font, c, &glyph_index))->advance);
  
}
int gl_font_char_height (void *gl_void_font, wchar_t *c)
{
  float llx, lly, llz, urx, ury, urz;          
  
  FontBBox ((GL_font *) gl_void_font, c, 1,
	      &llx, &lly, &llz, 
	    &urx, &ury, &urz);
  return SUPERSAMPLING ((int) (ury - lly));
 
}

int gl_font_char_ascent (void *gl_void_font, wchar_t *c)
{
  float llx, lly, llz, urx, ury, urz;          
  
  FontBBox ((GL_font *) gl_void_font, c, 1,
	    &llx, &lly, &llz, 
	    &urx, &ury, &urz);
  return SUPERSAMPLING ((int) ury);
}
/**********FONT INFO ***********/


/*--------------------------------------------------- 
   FontDescender :  return descent
------------------------------------------------------*/
static int FontDescender (GL_font *font)
{
  return ((font->face)->size->metrics.descender >> 6);
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
  glyph = Char_index_lookup_cache (font, c[0], &left);
  right = 0;
  i = 0;
  while (i < length)
    {
      if (left)
	{
	  
	  bbox = glyph->bbox;
	  bbox.yMin = bbox.yMin >> 6;
	  bbox.yMax = bbox.yMax >> 6;
	  *lly = (*lly < bbox.yMin) ? *lly: bbox.yMin;
	  *ury = (*ury > bbox.yMax) ? *ury: bbox.yMax;
	}
      i++;
      glyph = Char_index_lookup_cache (font, c[i], &right);
      if (font->kerning)
	*urx += FaceKernAdvance (font->face, 
				 left, right);
      left = right;
    }
  glyph = Char_index_lookup_cache (font, c[i], &left);
  
  if (left)
    {
      bbox = glyph->bbox;
      *llx = (float) (bbox.xMin >> 6);
      if (font->kerning)
	*urx -= FaceKernAdvance (font->face, left, right);
      *urx +=  (float) glyph->advance;
      *urx += bbox.xMax >> 6;
    }
}


/********OPENGL BITMAP CONSTRUCTION SET********/

/*-----------------------------------------------------------
  MakeBitmapGlyph : Make bitmap and struct to handle the glyph
  
-------------------------------------------------------------*/
static void MakeBitmapGlyph (GL_font *font,
			     unsigned int g,
			     GL_glyph *BitmapGlyph)
{
  FT_BitmapGlyph  bitmap;
  FT_Bitmap       *source;
  FT_Glyph        Glyph;
  unsigned        char *data, *ptr;
  int             err;
  register short unsigned int y,w,h,p;

  data = NULL;
  err = 0;
  if (g != 0)
    /*use of FT_LOAD_DEFAULT when quality will be ok*/
    if (!FT_Load_Glyph (font->face, 
		       g, 
			FT_LOAD_NO_HINTING)) 
      if (!FT_Get_Glyph (font->face->glyph, 
			 &Glyph))
	{
	  /*Last parameter tells that we destroy font's bitmap
	    So we MUST cache it    */
	  if (ft_glyph_format_bitmap != Glyph->format)
	    err = FT_Glyph_To_Bitmap (&Glyph, 
				      ft_render_mode_normal,
				      0, 
				      1);
	  if (err)
	    FT_Done_Glyph (Glyph);
	  else
	    {
	      bitmap = (FT_BitmapGlyph) Glyph;
	      source = &bitmap->bitmap;	 
	      if (source->width && source->rows)
		{
		  w = (short unsigned int) source->width;     
		  h = (short unsigned int) source->rows;
		  p = w*h;
		  /*if (source->pitch*(h-1) > p)
		    source->pitch = w;
		  */
		  data = TtaGetMemory ((int) p * sizeof (unsigned char));
		  if (data)
		    {
		      memset (data, 0, (int)p);
		      p = y = 0;
		      ptr = data;	      
		      while (y++ < h)
			{
			  memcpy (ptr + p, source->buffer + p, w);
			  p += source->pitch;
			}	    
		    }	      
		}
	      
	      FT_Glyph_Get_CBox (Glyph, 
				 ft_glyph_bbox_subpixels, 
				 &(BitmapGlyph->bbox));
	      
	      BitmapGlyph->data = data;
	      BitmapGlyph->advance = (int) (Glyph->advance.x >> 16);
	      BitmapGlyph->pos.x = bitmap->left;
	      BitmapGlyph->pos.y = source->rows - bitmap->top;   
	      BitmapGlyph->dimension.x = source->width;
	      BitmapGlyph->dimension.y = source->rows;  	  
	      FT_Done_Glyph (Glyph);
	      return;	      
	    }
	}
  BitmapGlyph->data = NULL;
  BitmapGlyph->advance = 0;
  BitmapGlyph->pos.x = 0;
  BitmapGlyph->pos.y = 0;   
  BitmapGlyph->dimension.x = 0;
  BitmapGlyph->dimension.y = 0; 
}
/*--------------------------------------------------
  BitmapAppend : Add the right portion of a 
  bitmap at end of another one.
---------------------------------------------------*/
static void BitmapAppend (unsigned char *data, 
			  unsigned char *append_data,
			  int x,
			  int y,
			  unsigned int width, 
			  register unsigned int height,
			  unsigned int Width)
{   
  register unsigned int i = 0;

  /* We position origin in the big bitmap*/
  data += y*Width + x;
  append_data += height*width;
  if (data &&
      append_data)
    while (height)
      {      
	append_data -= width;

	while (i < width)
	  {
	    if (*(append_data + i) > ANTIALIASINGDEPTH)
	      *(data + i) = *(append_data + i);
#ifdef AALIASTEST
	    /*if (*(append_data + i) < 12)*/
	    if (*(append_data + i))
	      {		
		if ((*(append_data + i) + 15) > 255)
		  *(data + i) = 255;
		else
		  *(data + i) = (*(append_data + i) + 15);
	      }	  
#endif /* AALIASTEST*/
	    i++;
	  }
	i = 0;
	data += Width;
	height--;
      }
}


/***************** FONT RENDERING *************/


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


      
/*----------------------------------------------------------------------
 GL_TextureInit : map texture on a Quad (sort of a rectangle)
 Drawpixel Method for software implementation, as it's much faster for those
 Texture Method for hardware implementation as it's faster and better.
  ----------------------------------------------------------------------*/
static void GL_TextureInit (unsigned char *Image,  
			    int width,
			    int height)
{  

  glTexParameteri (GL_TEXTURE_2D,
		   GL_TEXTURE_MIN_FILTER,
		   GL_NEAREST);
  glTexParameteri (GL_TEXTURE_2D,
		   GL_TEXTURE_MAG_FILTER,
		   GL_NEAREST);	    
  glTexParameteri (GL_TEXTURE_2D,
		   GL_TEXTURE_WRAP_S,
		   GL_CLAMP);
  glTexParameteri (GL_TEXTURE_2D,
		   GL_TEXTURE_WRAP_T,
		   GL_CLAMP); 
  /* does current Color modify texture no = GL_REPLACE, 
     else => GL_MODULATE, GL_DECAL, ou GL_BLEND */
  glTexEnvi (GL_TEXTURE_ENV,
	     GL_TEXTURE_ENV_MODE,
	     GL_MODULATE);
  /* We give te texture to opengl Pipeline system */	    
  
  glTexImage2D (GL_TEXTURE_2D, 
		0, 
		GL_ALPHA,
		width, 
		height, 
		0,
		GL_ALPHA, 
		GL_UNSIGNED_BYTE, 
		(GLvoid *) Image);
  
}

 /*----------------------------------------------------------------------
 GL_TextMap : map texture on a Quad (sort of a rectangle)
 Drawpixel Method for software implementation, as it's much faster for those
 Texture Method for hardware implementation as it's faster and better.
  ----------------------------------------------------------------------*/
static void GL_TextMap (float x, float y, int width, int height,
			int Texture_w, int Texture_h)
{
  float GL_w, GL_h;
  int xFrame, yFrame;

  GL_w = SUPERSAMPLING ((float)width / (float)Texture_w);  
  GL_h = SUPERSAMPLING ((float)height / (float)Texture_h);
  xFrame = (int) x;
  yFrame = (int) y;
  glBegin (GL_QUADS); 
  glTexCoord2f (   0, 0);   glVertex2i (xFrame,         yFrame + height);
  glTexCoord2f (GL_w, 0);   glVertex2i (xFrame + width, yFrame + height);
  glTexCoord2f (GL_w, GL_h);glVertex2i (xFrame + width, yFrame); 
  glTexCoord2f (   0, GL_h);glVertex2i (xFrame,         yFrame);
  glEnd ();	
}

#define MAX_STRING 128

/*--------------------------------------------------------
  UnicodeFontRender : Render an unicode string 
  (no more than a word)
  in a Bitmap.
  Using Two step computation :
     1) computes each glyphs position in the word (advance and kerning).
     2) place them in a bitmap.
  
  
---------------------------------------------------------*/
int UnicodeFontRender (void *gl_font,
                       wchar_t *text, 
                       float x,
                       float y, 
                       int size)
{
  FLOAT_VECTOR       bitmap_pos[MAX_STRING];   
  FT_Vector          delta;   
  FT_Bool            use_kerning;
  FT_UInt            previous, glyph_index;
  GL_glyph           *bitmaps[MAX_STRING];
  GL_font            *font;
  GL_glyph           *glyph;
  unsigned char      *data;
  float		     maxy, miny, shift;
  int                Width, Height, width;
  register int       pen_x, n;
  int FontBind;

  if (text == NULL) 
    return 0;
  
  if (size > MAX_STRING)
    {
      while (size > MAX_STRING)
	{
	  x += (float) UnicodeFontRender (gl_font,
					  text, 
					  x, 
					  y, 
					  MAX_STRING);
	  size -= MAX_STRING;
	  text += MAX_STRING;
	}
      if (size)
	x += (float) UnicodeFontRender (gl_font,
					text, 
					x,
					y, 
					size);
      return ((int) (x));
    }
  
  font = (GL_font *) gl_font;    
  use_kerning = font->kerning;
  previous = 0;
  miny = 10000;
  maxy = 0;
  pen_x = 0;

  for (n = 0; n < size; n++)
    {
        /* convert character code to glyph index */
        glyph = Char_index_lookup_cache (font, text[n], &glyph_index);
        /* retrieve kerning distance 
	   and move pen position */
	if (use_kerning &&
	    previous &&
	    glyph_index)
        {
          FT_Get_Kerning (font->face, 
			  previous, glyph_index,
			  KERNING_CHOICE,
			  &delta);
          pen_x += delta.x >> 6;
	  /* record current glyph index */
	  previous = glyph_index;
        }
	/* Position of the glyph in the texture*/
	bitmap_pos[n].x = (float) pen_x + (float) glyph->pos.x;
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
  
  maxy = maxy - miny;  
  Height = (p2 ((int) maxy));
  Width = 0;  
  
  /*shift if the first pos is neg */
  if (bitmap_pos[0].x < 0)
    shift = 0 - bitmap_pos[0].x;
  else
    shift = 0;
  /* final width of the text */
  n = size - 1;
  width = 0;
  while (n >= 0)
    if (bitmaps[n])
      {
	width = (int) (bitmap_pos[n].x +
				   shift +
		       bitmaps[n]->dimension.x);
	Width = (p2 (width));
	break;
      }
    else 
      n--;  
  if (Height <= 0 ||
      Width <= 0 ||
      miny == 10000)
    return 0;
  
  n = sizeof (unsigned char)*Height*Width;
  data = TtaGetMemory (n); 
  if (data == NULL)
    return 0;
  memset (data, 0, n);
  /* Load glyph image into the texture */
  for (n = 0; n < size; n++)
    {
      if (bitmaps[n] && 
	  bitmaps[n]->data)
	{
	  BitmapAppend (data, 
			bitmaps[n]->data,
			(int) (bitmap_pos[n].x + shift), 
			(int) (bitmap_pos[n].y - miny),
			(int) bitmaps[n]->dimension.x, 
			(int) bitmaps[n]->dimension.y, 
			Width);
	}
    }
  glEnable (GL_TEXTURE_2D);
  
  if (GL_NotInFeedbackMode ())
    {
      glGenTextures (1, &(FontBind));
      glBindTexture (GL_TEXTURE_2D, 
		     FontBind);
    
      GL_TextureInit (data,
		      Width,
		      Height);
    }
  if (data)
    TtaFreeMemory (data);
  
  y -= SUPERSAMPLING (maxy + miny);
  GL_TextMap ((x - SUPERSAMPLING(shift)), 
		 y, 
		 width, 
		 (int) maxy, 
		 Width, 
		 Height);
  
  if (GL_NotInFeedbackMode ())
   glDeleteTextures (1, &(FontBind));

  /* If there is no cache we must free
     allocated glyphs   */
  glDisable (GL_TEXTURE_2D);
  return (SUPERSAMPLING(pen_x));  
}
#endif /* _GL */
