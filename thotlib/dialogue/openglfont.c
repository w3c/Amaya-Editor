/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2007
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

#ifdef _WX
#include "wx/wx.h"
#endif /* _WX */

#define THOT_EXPORT extern
#include "thot_gui.h"
#include "constmedia.h"
#include "typemedia.h"
#include "typebase.h"
/*For ttagetmemory and free*/
#include "memory_f.h"
/*for ttafileexists*/
#include "fileaccess.h"
#include "font_f.h"
#include "glwindowdisplay.h"
#include "openglfont.h"
#include "openglfonts.h"
#include "registry_f.h"
#include "logdebug.h"
#include "message_wx.h"
#include "units_tv.h"

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

/* How to render glyphs ? : 
 * 
 FT_LOAD flags : (cf. http://www.freetype.org/freetype2/docs/reference/ft2-base_interface.html#FT_LOAD_XXX)
 #define FT_LOAD_DEFAULT                      0x0
 #define FT_LOAD_NO_SCALE                     0x1
 #define FT_LOAD_NO_HINTING                   0x2
 #define FT_LOAD_RENDER                       0x4
 #define FT_LOAD_NO_BITMAP                    0x8
 #define FT_LOAD_VERTICAL_LAYOUT              0x10
 #define FT_LOAD_FORCE_AUTOHINT               0x20
 #define FT_LOAD_CROP_BITMAP                  0x40
 #define FT_LOAD_PEDANTIC                     0x80
 #define FT_LOAD_IGNORE_GLOBAL_ADVANCE_WIDTH  0x200
 #define FT_LOAD_NO_RECURSE                   0x400
 #define FT_LOAD_IGNORE_TRANSFORM             0x800
 #define FT_LOAD_MONOCHROME                   0x1000
 #define FT_LOAD_LINEAR_DESIGN                0x2000

 FT_RENDER_MODE flags : (cf. http://www.freetype.org/freetype2/docs/reference/ft2-base_interface.html#FT_Render_Mode)
 FT_RENDER_MODE_NORMAL = 0,
 FT_RENDER_MODE_LIGHT,
 FT_RENDER_MODE_MONO,
 FT_RENDER_MODE_LCD,v 
 FT_RENDER_MODE_LCD_V,
 *
 * By default:
 * -> FT_RENDER_MODE_NORMAL : it corresponds to 8-bit anti-aliased bitmaps, using 256 levels of opacity.
 * -> FT_LOAD_FORCE_AUTOHINT : force the use of the FreeType auto-hinter when a glyph outline is loaded.
 * */
static FT_Render_Mode thot_ft_render_mode = ft_render_mode_normal;
static int            thot_ft_load_mode   = FT_LOAD_NO_HINTING;  /*** FT_LOAD_FORCE_AUTOHINT ***/

/*--------------Font Caching---------------------------*/

/*Font Cache */
static Font_Slot FontTab[1024];

/*----------------------------------------------------------------------
  FontAlreadyLoaded: Checks if font not already loaded
  ----------------------------------------------------------------------*/
static GL_font* FontAlreadyLoaded (const char *font_filename, int size)
{
  int i;
  
  i = 0;
  while (i < 1023)
    {
      if (FontTab[i].ref > 0 && size == FontTab[i].size &&
          strcasecmp (font_filename, FontTab[i].name) == 0)			
        {
#ifdef _WX
          TTALOGDEBUG_1( TTA_LOG_FONT, _T("FontAlreadyLoaded ")+TtaConvMessageToWX(font_filename)+_T(" %d"), size );
#endif /* _WX */
          FontTab[i].ref++;
          return FontTab[i].font;
        }
      i++;
    }
  return NULL;
}

/*----------------------------------------------------------------------
  GetFirstFont: Get anyway a font, best is according to size first
  ----------------------------------------------------------------------*/
void  *GetFirstFont (int size)
{
  int i;

  i = 0;
  while (i < 1023)
    {
      if (FontTab[i].ref > 0 && size == FontTab[i].size)
        return FontTab[i].font;
      i++;
    }
  i = 0;
  while (i < 1023)
    {
      if (FontTab[i].ref > 0)
        return FontTab[i].font;
      i++;
    }
  return NULL;
}

/*----------------------------------------------------------------------
  FontCache: Add font in cache
  ----------------------------------------------------------------------*/
static void FontCache (GL_font *font, const char *font_filename, int size)
{
  int i;
	
  i = 0;
  while (FontTab[i].ref > 0 && i < 1023)
    i++;
  FontTab[i].font = font;
  FontTab[i].font->Cache_index = i;
  FontTab[i].size = size;
  FontTab[i].name = (char *)TtaGetMemory (strlen(font_filename) + 1);
  strcpy (FontTab[i].name, font_filename);
  FontTab[i].ref = 1;

#ifdef _WX
  TTALOGDEBUG_2( TTA_LOG_FONT, _T("FontCache ")+TtaConvMessageToWX(font_filename)+_T(" %d : Cache_index=%d"), size, i );
#endif /* _WX */
}

/*----------------------------------------------------------------------
  FreeFontCache:
  ----------------------------------------------------------------------*/
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

/*----------------------------------------------------------------------
  FreeFontEntry :
  ----------------------------------------------------------------------*/
static void FreeFontEntry (GL_font* font)
{
  int i;
  
  if (font == NULL) 
    return;
  i = font->Cache_index;
  if (FontTab[i].ref == 1)
    {
#ifdef _WX
      TTALOGDEBUG_2( TTA_LOG_FONT, _T("FreeFontEntry ")+TtaConvMessageToWX(FontTab[i].name)+_T("%d : Cache_index=%d"), FontTab[i].size, i );
#endif /* _WX */
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

/*----------------------------------------------------------------------
  Char index Cache freeing recursive function 
  ----------------------------------------------------------------------*/
static void FreeACharCache (Char_Cache_index *cache)
{
  if (cache->next)
    FreeACharCache (cache->next);
  if (cache->glyph.data)
    {
      /* the cache data can be of tow different type */
      if (cache->glyph.data_type == GL_GLYPH_DATATYPE_GLLIST)
        glDeleteLists(*((GLuint*)cache->glyph.data), 1); /* a glList */
      else if (cache->glyph.data_type == GL_GLYPH_DATATYPE_FTBITMAP)
        TtaFreeMemory (cache->glyph.data); /* a freetype bitmap */
    }
  TtaFreeMemory (cache);  
}

/*----------------------------------------------------------------------
  Char_index_lookup_cache returns the character cache entry.
  This entry is created if it doesn't already exist.
  ----------------------------------------------------------------------*/
Char_Cache_index *Char_index_lookup_cache (GL_font *font, unsigned int idx,
                                           ThotBool isPoly)
{
  Char_Cache_index   *cache, *newcache;
  GL_glyph_DataType   data_type;

  if (isPoly)
    data_type = GL_GLYPH_DATATYPE_GLLIST;
  else
    data_type = GL_GLYPH_DATATYPE_FTBITMAP;
  
  /* look for an existing font in the cache */
  cache = font->Cache;
  if (cache)
    while (1)
      {
        if (cache->index == idx &&
            (cache->glyph.data_type == data_type ||
             cache->glyph.data_type == GL_GLYPH_DATATYPE_NONE))
          return (cache);
        if (cache->next)
          cache = cache->next;      
        else break;      
      }

#ifdef _WX
  TTALOGDEBUG_3( TTA_LOG_FONT, _T("new cache entry font=%x idx=%d datatype=%d"), font, idx, data_type );
#endif /* _WX */

  /* nothing has been found : now create a new cache entry */
  newcache = (Char_Cache_index*)TtaGetMemory (sizeof (Char_Cache_index));
  memset (newcache, 0, sizeof (Char_Cache_index));
  if (cache)
    {
      cache->next = newcache;
      cache = cache->next;  
    }
  else 
    {
      font->Cache = newcache;
      cache = font->Cache;      
    }
  cache->index = idx;
  cache->character = FT_Get_Char_Index (font->face, idx);
  cache->next = NULL;
  if (isPoly)
    /* here the cache->glyph.data is filled with a glList */
    /* must be freed with glDeleteLists */
    MakePolygonGlyph (font, cache->character, &cache->glyph);
  else
    /* here the cache->glyph.data is filled with a FT bitmap */
    /* must be freed with TtaFreeMemory */
    MakeBitmapGlyph (font, cache->character, &cache->glyph);
  /* return the new cache entry */

  return (cache);
}

/*----------------------------------------------------------------------
  Load for thotrc the wanted fonts render mode
  ----------------------------------------------------------------------*/
void InitFreetype_Modes()
{
  ThotBool noaliasing;

  TtaSetEnvBoolean ("FONT_ALIASING", TRUE, FALSE);
  TtaGetEnvBoolean ("FONT_ALIASING", &noaliasing);
  if (noaliasing)
    {
      thot_ft_render_mode = ft_render_mode_normal;
      thot_ft_load_mode   = FT_LOAD_NO_HINTING;  /**** FT_LOAD_FORCE_AUTOHINT ***/
    }
  else
    {
      thot_ft_render_mode = ft_render_mode_mono;
      thot_ft_load_mode   = FT_LOAD_MONOCHROME;
    }
}

/*----------------------------------------------------------------------
  Freetype library Handling
  ----------------------------------------------------------------------*/
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
  memset(FontTab, 0, sizeof(Font_Slot)*1024);
  InitFreetype_Modes();
  return TRUE;
}

/*----------------------------------------------------------------------
  FTLibraryFree : Free the Freetype hamdle
  ----------------------------------------------------------------------*/
void FTLibraryFree ()
{
  if (init_done)
    {      
      FreeFontCache ();
      FT_Done_FreeType (FTlib);
      FTlib= 0;
    }
}


/*----------------------------------------------------------------------
  FontClose :
  ----------------------------------------------------------------------*/
static void FontClose (GL_font *font)
{
  if (!font)
    return;
  if (font->Cache)
    FreeACharCache (font->Cache);
  if (font->face)
    FT_Done_Face (font->face);
  TtaFreeMemory (font);
  font = NULL;
}

/*----------------------------------------------------------------------
  Common Font handling 
  ----------------------------------------------------------------------*/
static GL_font *FontOpen (const char* fontname)
{
  GL_font *font;
  int err; 

  if (!init_done)
    {
      FTLibraryInit ();
      init_done = TRUE;
    }

  if (!TtaFileExist (fontname))
    return NULL;
  font = (GL_font *) TtaGetMemory (sizeof (GL_font));
  err = FT_New_Face (FTlib, fontname, 0, &(font->face));
  if(err)
    {
      TtaFreeMemory (font);
      return NULL;
    }
#ifdef _WX
  TTALOGDEBUG_0( TTA_LOG_FONT, _T("FontOpen ")+TtaConvMessageToWX(fontname) );
#endif /* _WX */

  return font; 
}


/*----------------------------------------------------------------------
  FontFaceSize : set char Size
  ----------------------------------------------------------------------*/
static int FontFaceSize (GL_font *font, unsigned int size, unsigned int res)
{
  unsigned int err;

  if (size == font->size)
    return 0;
  size = SUPERSAMPLINGMUL (size);
  err = FT_Set_Char_Size (font->face, 0, size * 64, res, res);
  if (err)
    err = FT_Set_Char_Size (font->face, 0, size * 64, 75, 75);
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
          err = FT_Select_Charmap (font->face, ft_encoding_symbol);
          if (err)
            {	 
              if ((font->face)->num_charmaps > 0) 
                { 
                  FT_Set_Charmap( (font->face), (font->face)->charmaps[0]);
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

/*----------------------------------------------------------------------
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
  ----------------------------------------------------------------------*/
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
      err = FontFaceSize (gl_font, (unsigned int)size, DOT_PER_INCH);
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

/*----------------------------------------------------------------------
  FontBBox : String bounding box
  ----------------------------------------------------------------------*/
static void FontBBox (GL_font *font, wchar_t *string, int length,
                      float *llx, float *lly, float *llz, float *urx, 
                      float *ury, float *urz)
{
  Char_Cache_index   *cache;
  FT_BBox             bbox;
  GL_glyph           *glyph;
  wchar_t            *c; 
  int                 left, right, i;

  glyph = NULL;
  *llx = *lly = *llz = *urx = *ury = *urz = 0;
  c = string;
  /* get info about the first character */
  cache = Char_index_lookup_cache (font, c[0], GL_TransText());
  glyph = &(cache->glyph);
  left =  cache->character;
  if (left)
    {
      bbox = glyph->bbox;
      bbox.yMin = bbox.yMin >> 6;
      bbox.yMax = bbox.yMax >> 6;
      *lly = (*lly < bbox.yMin) ? *lly: bbox.yMin;
      *ury = (*ury > bbox.yMax) ? *ury: bbox.yMax;
    }
  right = 0;
  i = 1;
  while (i < length)
    {
      /* get info about the next character */
      cache = Char_index_lookup_cache (font, c[i], GL_TransText());
      glyph = &(cache->glyph);
      right = left;
      left =  cache->character;
      if (font->kerning)
        *urx += FaceKernAdvance (font->face, right, left);
      if (left)
        {
          bbox = glyph->bbox;
          *llx = (float) (bbox.xMin >> 6);
          if (font->kerning)
            *urx -= FaceKernAdvance (font->face, left, right);
          *urx +=  (float) glyph->advance;
          *urx += bbox.xMax >> 6;
        }
      i++;
    }
  
}

/************** Generic Calls *****************/ 
/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
unsigned int GL_Font_Get_Size (void *font)
{
  return ((GL_font *)font)->size;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void GL_Font_Change_Height (void *font, int size)
{
  FontFaceSize ((GL_font *) font, (unsigned int) size, DOT_PER_INCH);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void gl_font_delete (void *gl_void_font)
{ 
  FreeFontEntry ((GL_font *) gl_void_font);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
int gl_font_height (void *gl_void_font)
{
  return  SUPERSAMPLING (((GL_font *) gl_void_font)->height);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
int gl_font_ascent (void *gl_void_font)
{
  return SUPERSAMPLING (((GL_font *) gl_void_font)->ascent);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
int gl_font_char_width (void *gl_void_font, wchar_t c)
{
  GL_font            *font = (GL_font *) gl_void_font;
  Char_Cache_index   *cache;
  
  cache = Char_index_lookup_cache (font, c, GL_TransText());
  return SUPERSAMPLING (cache->glyph.advance);
  
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
int gl_font_char_height (void *gl_void_font, wchar_t *c)
{
  float llx, lly, llz, urx, ury, urz;          
  
  FontBBox ((GL_font *) gl_void_font, c, 1, &llx, &lly, &llz, 
            &urx, &ury, &urz);
  return SUPERSAMPLING ((int) (ury - lly));
 
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
int gl_font_char_ascent (void *gl_void_font, wchar_t *c)
{
  float llx, lly, llz, urx, ury, urz;          
  
  FontBBox ((GL_font *) gl_void_font, c, 1, &llx, &lly, &llz, 
            &urx, &ury, &urz);
  return SUPERSAMPLING ((int) ury);
}


/**********FONT INFO ***********/
/*----------------------------------------------------------------------
  FontDescender :  return descent
  ----------------------------------------------------------------------*/
static int FontDescender (GL_font *font)
{
  return ((font->face)->size->metrics.descender >> 6);
}


/*----------------------------------------------------------------------
  FaceKernAdvance : Return Distance between 2 glyphs
  ----------------------------------------------------------------------*/
static float FaceKernAdvance (FT_Face face, unsigned int index1,  unsigned int index2)
{  
  FT_Vector kernAdvance;

  kernAdvance.x = 0;  
  if (index1 && index2)
    {
      FT_Get_Kerning (face, index1, index2, ft_kerning_unscaled,
                      &kernAdvance);
      if (kernAdvance.x)
        return (float) (kernAdvance.x >> 16); 
    }
  return (0x0);
}

/********OPENGL BITMAP CONSTRUCTION SET********/
/*----------------------------------------------------------------------
  GetCharacterGlyph
  ----------------------------------------------------------------------*/
unsigned char *GetCharacterGlyph (GL_font *font, unsigned int idx, int w, int h)
{
  FT_BitmapGlyph  bitmap;
  FT_Bitmap       *source = NULL;
  FT_Glyph        Glyph;
  unsigned        char *data = NULL, *dest = NULL, *src = NULL;
  unsigned char  *bsrc, b;
  unsigned int    index;
  int             err = 0, p, i, y, width, height, offx, offy;

  p = w * h * 3; // 3 bytes by pixel
  data = (unsigned char *)TtaGetMemory (p);
  memset (data, 0xFF, p);
  index = FT_Get_Char_Index (font->face, idx);
  if (index != 0 &&
      !FT_Load_Glyph (font->face, index, thot_ft_load_mode) &&
      !FT_Get_Glyph (font->face->glyph, &Glyph))
    {
      if (Glyph->format != ft_glyph_format_bitmap)
        /* Last parameter tells that we destroy font's bitmap
         * So we MUST cache it */
        err = FT_Glyph_To_Bitmap (&Glyph, thot_ft_render_mode, 0, 1);
      if (err)
        FT_Done_Glyph (Glyph);
      else
        {
          /* when the glyph is found into the font, we must come here */
          bitmap = (FT_BitmapGlyph) Glyph;
          source = &bitmap->bitmap;
          width = source->width;
          height = source->rows;
          offx = (w-width)/2;
          offy = (h-height)/2;
          if(offx<0)
            offx = 0;
          if(offy<0)
            offy = 0;
          if (data)
            {
              dest = data;
              src = source->buffer;
              switch (source->pixel_mode)
                {
                  /* 1 bit per pixel, expand the bitmap */
                case ft_pixel_mode_mono:
                  for (y = offy; y < h; y++)
                    {
                      if ( y - offy < height)
                        {
                          dest = data + y * w * 3 + offx * 3;
                          bsrc = src;
                          b = 0;
                          for (i = 0; i < w; i++)
                            {
                              if (i < width)
                                {
                                  if (i%8 == 0)
                                    b = *bsrc++; // get the current byte
                                  if (b&0x80)
                                    memset (dest, 0, 3);
                                  b <<= 1;
                                }
                              dest += 3; // next 3 bytes
                            }
                          src += source->pitch;
                        }
                    }
                  break;
                  /* one byte per pixel, just copy the bitmap */
                case ft_pixel_mode_grays:
                  for (y = offy; y < h; y++)
                    {
                      dest = data + y * w * 3 + offx * 3;
                      if ( y - offy < height)
                        {
                          bsrc = src;
                          b = 0;
                          for (i = 0; i < w; i++)
                            {
                              if (i < width)
                                {
                                  b = *bsrc++; // get the current byte
                                  memset (dest, ~b, 3);
                                }
                              dest += 3; // next 3 bytes
                            }
                          src += source->pitch;
                        }
                    }
                  break;
                default:
                  ; /* currently unused by freetype */
                }
            }
        }
    }
  
  return data;
}

/*----------------------------------------------------------------------
  MakeBitmapGlyph : Make bitmap and struct to handle the glyph
  ----------------------------------------------------------------------*/
static void MakeBitmapGlyph (GL_font *font, unsigned int g, GL_glyph *BitmapGlyph)
{
  FT_BitmapGlyph  bitmap;
  FT_Bitmap       *source = NULL;
  FT_Glyph        Glyph;
  unsigned        char *data = NULL, *ptr = NULL, *src = NULL;
  int             err = 0;
  register unsigned int y = 0, w = 0, h = 0, p = 0, i = 0;
  int             advance = 9; /* default value (used when no glyph has been found)*/

  /* just to be sure bitmap is empty */
  memset( &bitmap, 0, sizeof(BitmapGlyph) );

  err = 0; /* by default suppose the process find a bitmap glyph */

  if ( g != 0 &&
       !FT_Load_Glyph (font->face, g, thot_ft_load_mode) &&
       !FT_Get_Glyph (font->face->glyph, &Glyph) )
    {
      if (Glyph->format != ft_glyph_format_bitmap)
        /* Last parameter tells that we destroy font's bitmap
         * So we MUST cache it */
        err = FT_Glyph_To_Bitmap (&Glyph, thot_ft_render_mode, 0, 1);
      if (err)
        FT_Done_Glyph (Glyph);
      else
        {
          /* when the glyph is found into the font, we must come here */
          bitmap = (FT_BitmapGlyph) Glyph;
          source = &bitmap->bitmap;
          w = (unsigned int) source->width;
          h = (unsigned int) source->rows;
          advance = (int)(Glyph->advance.x >> 16);
          if (w && h)
            {
              p = w * h;
              data = (unsigned char *)TtaGetMemory (p);
              if (data)
                {
                  memset (data, 0, p);
                  ptr = data;
                  src = source->buffer;
                  switch (source->pixel_mode)
                    {
                      /* 1 bit per pixel, expand the bitmap */
                    case ft_pixel_mode_mono:
                      for (y=0; y<h; y++)
                        {
                          unsigned char *bptr = src;
                          unsigned char b = 0;
                          for (i=0; i<w; i++)
                            {
                              if (i%8==0) b = *bptr++;
                              *ptr++ = b&0x80 ? 0xFF : 0;
                              b <<= 1;
                            }
                          src += source->pitch;
                        }
                      break;
                      /* one byte per pixel, just copy the bitmap */
                    case ft_pixel_mode_grays:
                      memcpy (ptr, src, p);
                      break;
                    default:
                      ; /* currently unused by freetype */
                    }
                }
            }
          else
            {
              err = 1;
              FT_Done_Glyph (Glyph);
            }
        }
    }
  else
    err = 1;
  
  if (err == 0)
    {
      // a glyph has been found, store it
      FT_Glyph_Get_CBox (Glyph, ft_glyph_bbox_subpixels, &(BitmapGlyph->bbox));
      BitmapGlyph->data = data;
      BitmapGlyph->data_type = GL_GLYPH_DATATYPE_FTBITMAP; /* must be freed with TtaFreeMemory */
      BitmapGlyph->advance = advance;
      BitmapGlyph->pos.x = bitmap->left;
      BitmapGlyph->pos.y = source->rows - bitmap->top;   
      BitmapGlyph->dimension.x = w;
      BitmapGlyph->dimension.y = h;  	  
      FT_Done_Glyph (Glyph);
    }
  else
    {
      // no glyph has been found just draw a small black rectangle
      // to indicate the caractere is not available in amaya's fonts
      w = 7;     
      h = font->height;
      if (h < 2)
        h = 2;
      p = w * h;		  
      data = (unsigned char *)TtaGetMemory (p);
      if (data)
        {
          ptr = data;
          memset (ptr, 0xFF, w);
          ptr += w;
          while (ptr < data + p - 2*w)
            {
              memset (ptr++, 0xFF, 1);
              memset (ptr, 0, w - 2);
              ptr = ptr + w - 2;
              memset (ptr++, 0xFF, 1);
            }
          memset (ptr, 0xFF, w);
	        ptr = ptr + w;
	        memset (ptr, 0, w);
        }
      
      BitmapGlyph->data = data;
      BitmapGlyph->data_type = GL_GLYPH_DATATYPE_FTBITMAP; /* must be freed with TtaFreeMemory */
      BitmapGlyph->advance = advance;
      BitmapGlyph->pos.x = 1;
      BitmapGlyph->pos.y = 4;
      BitmapGlyph->dimension.x = w;
      BitmapGlyph->dimension.y = h;  	  
    }
}

/*----------------------------------------------------------------------
  BitmapAppend : Add the right portion of a bitmap at end of another one.
  ----------------------------------------------------------------------*/
static void BitmapAppend (unsigned char *data, unsigned char *append_data,
                          int x, int y, unsigned int width, 
                          unsigned int height, unsigned int Width, ThotBool justcopy)
{
  register unsigned int i = 0;

  /* Origin position within the big bitmap */
  data += y*Width + x;
  append_data += height*width;
  if (data && append_data)
    /* copy each line one by one */
    if (justcopy)
      {
        // copy bitmap line by line (FASTEST than pixel by pixel bellow)
        while (height)
          {      
            append_data -= width;
            memcpy( data, append_data, width * sizeof(unsigned char));
            data += Width;
            height--;
          }
      }
    else
      {
        while (height)
          {      
            append_data -= width;
            while (i < width)
              {
                /* copy each pixels one by one
                 * it's not possible to copy directly the whole line because
                 * sometime characteres overlap on other characteres
                 * it's the case when an italic word is displayed,
                 * for example : "lp"
                 *  **********                                                       
                 *  *      ..*                                                       
                 *  *     .. *                                                       
                 *  *    ..  ******                                                  
                 *  *   ..  .!... *                                                  
                 *  *  ..   .!....*                                                  
                 *  * ..   ..!  ..*                                                  
                 *  *....  ..!... *                                                  
                 *  *.... ...!.   *                                                  
                 *  ****-----     *                                                  
                 *     * ..       *                                                  
                 *     *..        *                                                  
                 *     *..        *                                                  
                 *     ************                                                  
                 * */
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

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static int ceil_pow2_minus_1(unsigned int x)
{
  unsigned int i;
  
  for (i=1; i; i <<= 1)
    x |= x >> i;
  return x;
}
#define p2(p) (is_pow2(p)?p:ceil_pow2_minus_1((unsigned int) p) + 1)

/* a unique identifier is used to bind font texture into opengl memory
 * it's not possible to reuse the same fontbind for other textures because 
 * a texture is a word and there is not a lot of word repetition */

/* do not use anymore this global variable because it causes gl memory leak
 * when 2 SetTextureScale was called with 2 StopTextureScale after :
 * when the second SetTextureScale the first FontBind was lost so never deleted !
 */

/*----------------------------------------------------------------------
  returns a texture identifier (0 if no texture to generate)
  ----------------------------------------------------------------------*/
int SetTextureScale (ThotBool Scaled)
{
  int texture_id = 0;

  if (GL_NotInFeedbackMode () && !GL_TransText ())
    {
      glEnable (GL_TEXTURE_2D);
      /* get a new identifier for the following font texture */
      glGenTextures (1, (GLuint*)&(texture_id));
	  if (texture_id)
	  {
      /* set the allocated texture id to the current used texture
         (setup opengl state machine to use this texture) */
      glBindTexture (GL_TEXTURE_2D, texture_id);
      glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                       ((Scaled)?GL_LINEAR:GL_NEAREST));
      glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP); 
      /* does current Color modify texture no = GL_REPLACE, 
         else => GL_MODULATE, GL_DECAL, ou GL_BLEND */
      glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	  }
    }
  return texture_id; /* 0 is a invalid texture identifier */
}

/*----------------------------------------------------------------------
  param: texture_id the texture identifier given by SetTextureScale
  ----------------------------------------------------------------------*/
void StopTextureScale ( int texture_id )
{   
  if (GL_NotInFeedbackMode () && !GL_TransText () && texture_id)
    {
      if (glIsTexture(texture_id))
       glDeleteTextures (1, (GLuint*)&(texture_id));
      glDisable (GL_TEXTURE_2D);
    }
}

/*----------------------------------------------------------------------
  GL_TextureInit : map texture on a Quad (sort of a rectangle)
  Drawpixel Method for software implementation, as it's much faster for those
  Texture Method for hardware implementation as it's faster and better.
  ----------------------------------------------------------------------*/
static void GL_TextureInit (unsigned char *image,  int width, int height)
{
  /* We give te texture to opengl Pipeline system */
  glTexImage2D (GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0,
                GL_ALPHA, GL_UNSIGNED_BYTE,(GLvoid *) image);
}

/*----------------------------------------------------------------------
  GL_TextMap : map texture on a Quad (sort of a rectangle)
  Drawpixel Method for software implementation, as it's much faster for those
  Texture Method for hardware implementation as it's faster and better.
  ----------------------------------------------------------------------*/
static void GL_TextMap (float x, float y, int width, int height,
                        int texture_w, int texture_h)
{
  float GL_w, GL_h;
  int xFrame, yFrame;

  GL_w =/* SUPERSAMPLING*/ ((float)width / (float)texture_w);  
  GL_h = /*SUPERSAMPLING*/ ((float)height / (float)texture_h);
  xFrame = (int) x;
  yFrame = (int) y;
  glBegin (GL_QUADS); 
  glTexCoord2f (   0, 0);   glVertex2i (xFrame,         yFrame + height);
  glTexCoord2f (GL_w, 0);   glVertex2i (xFrame + width, yFrame + height);
  glTexCoord2f (GL_w, GL_h);glVertex2i (xFrame + width, yFrame); 
  glTexCoord2f (   0, GL_h);glVertex2i (xFrame,         yFrame);
  glEnd ();	
}

#define MAX_STRING 64
#define MAX_BITMAP_ALLOC 4096
static unsigned char g_gl_bitmap_buffer[MAX_BITMAP_ALLOC];
/*----------------------------------------------------------------------
  UnicodeFontRender : Render an unicode string (no more than a word)
  in a Bitmap.
  Using Two step computation :
  1) computes each glyphs position in the word (advance and kerning).
  2) place them in a bitmap.
  ----------------------------------------------------------------------*/
int UnicodeFontRender (void *gl_font, wchar_t *text, float x, float y, int size)
{
  FLOAT_VECTOR        bitmap_pos[MAX_STRING];   
  FT_Vector           delta;   
  FT_Bool             use_kerning;
  FT_UInt             previous, glyph_index;
  GL_glyph           *bitmaps[MAX_STRING];
  GL_font            *font;
  GL_glyph           *glyph;
  Char_Cache_index   *cache;
  unsigned char      *data = NULL;
  float		            maxy, miny, shift, xorg;
  int                 w, h, width, bitmap_alloc;
  register int        pen_x, n;
 
  if (text == NULL) 
    return 0;
  if (GL_TransText ())
    return UnicodeFontRenderPoly (gl_font, text, x, y, size);

  if (size > MAX_STRING)
    {
      xorg = x;
      while (size > MAX_STRING)
        {
          x += (float) UnicodeFontRender (gl_font, text, x, y, MAX_STRING);
          size -= MAX_STRING;
          text += MAX_STRING;
        }
      if (size)
        x += (float) UnicodeFontRender (gl_font, text, x, y, size);
      return ((int) (x - xorg));
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
      cache = Char_index_lookup_cache (font, text[n], GL_TransText());
      glyph = &(cache->glyph);
      glyph_index = cache->character;
      /* retrieve kerning distance 
         and move pen position */
      if (use_kerning && previous && glyph_index)
        {
          FT_Get_Kerning (font->face, previous, glyph_index,
                          KERNING_CHOICE, &delta);
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
        maxy = (float) (glyph->dimension.y - glyph->pos.y);
      
      /* increment pen position*/
      pen_x += glyph->advance;
    }
  
  maxy = maxy - miny;
  h = (p2 ((int) maxy));
  w = 0;
  
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
        width = (int) (bitmap_pos[n].x + shift + bitmaps[n]->dimension.x);
        w = (p2 (width));
        break;
      }
    else 
      n--;  
  if (h <= 0 || w <= 0 || fabs (miny - 10000) < 0.0001)
    return 0;
  
  bitmap_alloc = (sizeof (unsigned char) * h * w) + 4;
  if (bitmap_alloc >= MAX_BITMAP_ALLOC)
    {
      data = (unsigned char *)TtaGetMemory (bitmap_alloc); 
      if (data == NULL)
        return 0;
    }
  else
    data = g_gl_bitmap_buffer;
  memset (data, 0, bitmap_alloc);
  /* Load glyph image into the texture */
  for (n = 0; n < size; n++)
    {
      if (bitmaps[n] && bitmaps[n]->data)
        {
          BitmapAppend( data, (unsigned char *)bitmaps[n]->data,
                        (int) (bitmap_pos[n].x + shift), 
                        (int) (bitmap_pos[n].y - miny),
                        (int) bitmaps[n]->dimension.x, 
                        (int) bitmaps[n]->dimension.y, 
                        w,
                        // if it's the first lettre just copy the bitmap without testing the pixels
                        (n == 0) ||
                        // OR
                        // if the variation between the two letter is bigger than the
                        // last character width, just copy the bitmap without testing each pixels.
                        (bitmap_pos[n].x-bitmap_pos[n-1].x)>(bitmaps[n-1]->advance)
                        );
        }
    }
  
  /* SG: I think there is an optimisation to to here because 
   * for each characteres a new OpenGL texture is created with GL_TextureInit
   * (this function is very slow)
   * What it is possible to do is to save into the glyph cache, each OpenGL textures id
   * (given by glGenTextures). Then when a charactere wants to be drawn, if there is an OpenGL texture id,
   * it uses it and map the allready generated texture on a new quad (with GL_TextMap).
   */

  /* if notinfeedbackmode (we draw draw something) then */
  /* throw bitmap data to OpenGL */
  if (GL_NotInFeedbackMode ())
    GL_TextureInit (data, w, h);
  if (data && bitmap_alloc >= MAX_BITMAP_ALLOC)
    TtaFreeMemory (data);

  /* now map the created texture on a quad */
  y -= SUPERSAMPLING (maxy + miny);
  GL_TextMap ((x - SUPERSAMPLING(shift)), y, width, (int) maxy, w, h);
  
  /* If there is no cache we must free
     allocated glyphs   */
  return (SUPERSAMPLING(pen_x));  
}

#endif /* _GL */
