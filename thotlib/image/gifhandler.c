/* 
 *
 *  (c) COPYRIGHT INRIA, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *
 * Authors: I. Vatton, N. Layaida (INRIA)
 *          R. Guetari (W3C/INRIA) - Previous Windows version
 */
#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "picture.h"
#include "frame.h"
#include "libmsg.h"
#include "message.h"

#ifdef _WINGUI
#include "winsys.h"
#endif /* _WINGUI */

//#include "xpm.h"
#include "thotcolor.h"

#define THOT_EXPORT extern
#include "picture_tv.h"
#include "frame_tv.h"
#include "edit_tv.h"
#include "thotcolor_tv.h"

#include "font_f.h"
#include "units_f.h"
#include "inites_f.h"
#include "gifhandler_f.h"
#include "picture_f.h"

#ifdef _WINGUI
#include "wininclude.h"
#endif /* _WINGUI */
#include "application.h"

#define	MAXCOLORMAPSIZE		256
#ifdef _WX
#define COLORMAPSCALE 65536 / MAXCOLORMAPSIZE;
#endif /* _WX */
#ifdef _GTK
#define COLORMAPSCALE 65536 / MAXCOLORMAPSIZE;
#endif /* _GTK */
#ifdef _WINGUI
#define COLORMAPSCALE 1;
int     PngTransparentColor;
#endif /* _WINGUI */

  
#define CM_RED		0
#define CM_GREEN	1
#define CM_BLUE		2
#define	MAX_LWZ_BITS		12
#define INTERLACE		0x40
#define LOCALCOLORMAP	0x80
#define BitSet(byte, bit)	(((byte) & (bit)) == (bit))
#define	ReadOK(file, buffer, len)	(fread(buffer, len, 1, file) > 0)
#define LM_to_uint(a,b)		(((b) << 8) | (a))

typedef struct {
  unsigned int        Width;
  unsigned int        Height;
  unsigned char       ColorMap[3][MAXCOLORMAPSIZE];
  unsigned int        BitPixel;
  unsigned int        ColorResolution;
  unsigned int        Background;
  unsigned int        AspectRatio;
} GifScreen_;


GifScreen_ GifScreen;

int                 ZeroDataBlock = FALSE;

static int          GifTransparent;
static int          code_size, set_code_size;
static int          max_code, max_code_size;
static int          firstcode, oldcode;
static int          clear_code, end_code;
static int          table[2][(1 << MAX_LWZ_BITS)];
static int          stack[(1 << (MAX_LWZ_BITS)) * 2], *sp = stack;

/*----------------------------------------------------------------------
  ReadGifImage
  ----------------------------------------------------------------------*/
static unsigned char *ReadGifImage (FILE *fd, int *w, int *h,
                                    unsigned char clmap[3][MAXCOLORMAPSIZE],
                                    int interlace, int ignore)
{
  unsigned char       c;
  int                 v, len, height, lines;
  int                 xpos = 0, ypos = 0, pass = 0;
  unsigned char      *data;
  unsigned char      *dptr;

  /* Initialize the Compression routines */
  if (!ReadOK (fd, &c, 1))
    return (NULL);
  if (LWZReadByte (fd, TRUE, c) < 0)
    return (NULL);
  /* If this is an "uninteresting picture" ignore it */
  if (ignore)
    return (NULL);
  len = *w;
  height = *h;
  lines = 1;
  data = (unsigned char*) TtaGetMemory (sizeof (unsigned char) * len * height);
  if (data == NULL)
    return (NULL);
  while ((v = LWZReadByte (fd, FALSE, c)) >= 0)
    {
      /* store the byte value */
      dptr = (unsigned char *) (data + (ypos * len) + xpos);
      *dptr = (unsigned char) v;
      
      xpos++;
      if (xpos == len)
        {
          xpos = 0;
          if (interlace)
            {
              switch (pass)
                {
                case 0:
                case 1:
                  ypos += 8;
                  break;
                case 2:
                  ypos += 4;
                  break;
                case 3:
                  ypos += 2;
                  break;
                }
	      
              if (ypos >= height)
                {
                  ++pass;
                  switch (pass)
                    {
                    case 1:
                      ypos = 4;
                      break;
                    case 2:
                      ypos = 2;
                      break;
                    case 3:
                      ypos = 1;
                      break;
                    default:
                      break;
                    }
                }
            }
          else
            ypos++;

          if (ypos >= height)
            break;
          else
            /* read a new line */
            lines++;
        }
    }

  if (LWZReadByte (fd, FALSE, c) >= 0)
    {
      fprintf (stderr, "gifhandler: too much input data, ignoring extra...\n");
      /* return the number of lines read */
      *h = lines;
    }
  return (data);
}

#ifdef _GL
/*----------------------------------------------------------------------
  ReadGIF
  ----------------------------------------------------------------------*/
unsigned char *ReadGIF (FILE *fd, int *w, int *h, int *ncolors, int *cpp,
                        ThotColorStruct *colrs)
{
  unsigned char       buf[16];
  unsigned char      *data;
  unsigned char       c;
  unsigned char       localColorMap[3][MAXCOLORMAPSIZE];
  int                 useGlobalColormap;
  int                 bitPixel;
  int                 imageCount = 0;
  char                version[4];
  int                 imageNumber = 1;
  int                 i;

  *w = 0;
  *h = 0;
  data = NULL;
  if (!ReadOK (fd, buf, 6)) 
    return (NULL);

  if (strncmp ((char *) buf, "GIF", 3) != 0)
    return (NULL);
  strncpy (version, (char *) buf + 3, 3);
  version[3] = EOS;
  if ((strcmp (version, "87a") != 0) && (strcmp (version, "89a") != 0))
    return (NULL);
   
  if (!ReadOK (fd, buf, 7))
    return (NULL);

  GifScreen.Width  = LM_to_uint (buf[0], buf[1]);
  GifScreen.Height = LM_to_uint (buf[2], buf[3]);
  GifScreen.BitPixel = 2 << (buf[4] & 0x07);
  *ncolors = GifScreen.BitPixel;
  GifScreen.ColorResolution = (((buf[4] & 0x70) >> 3) + 1);
  GifScreen.Background = buf[5];
  GifScreen.AspectRatio = buf[6];
  if (BitSet (buf[4], LOCALCOLORMAP))
    { /* Global Colormap */
      if (ReadColorMap (fd, GifScreen.BitPixel, GifScreen.ColorMap))
        return (NULL);
      for (i = 0; i < (int) GifScreen.BitPixel; i++)
        {
          colrs[i].red   = GifScreen.ColorMap[0][i];
          colrs[i].green = GifScreen.ColorMap[1][i];
          colrs[i].blue  = GifScreen.ColorMap[2][i];
        }

      for (i = GifScreen.BitPixel; i < MAXCOLORMAPSIZE; i++)
        {
          colrs[i].red = 0;
          colrs[i].green = 0;
          colrs[i].blue = 0;
        }
    }

  for (;;)
    {
      if (!ReadOK (fd, &c, 1))
        return (NULL);

      if (c == ';')
        { /* GIF terminator */
          if (imageCount < imageNumber)
            return (NULL);
          break;
        }

      if (c == '!')
        { /* Extension */
          if (!ReadOK (fd, &c, 1))
            return (NULL);
          DoExtension (fd, c);
          continue;
        }

      if (c != ',')
        continue;
      ++imageCount;
      if (!ReadOK (fd, buf, 9))
        return (NULL);

      useGlobalColormap = !BitSet (buf[8], LOCALCOLORMAP);
      bitPixel = 1 << ((buf[8] & 0x07) + 1);
      *w = LM_to_uint (buf[4], buf[5]);
      *h = LM_to_uint (buf[6], buf[7]);
      if (!useGlobalColormap)
        {
          if (ReadColorMap (fd, bitPixel, localColorMap))
            return (NULL);
          for (i = 0; i < bitPixel; i++)
            { 
              colrs[i].red   = localColorMap[0][i];
              colrs[i].green = localColorMap[1][i];
              colrs[i].blue  = localColorMap[2][i];
            }

          for (i = bitPixel; i < MAXCOLORMAPSIZE; i++)
            {
              colrs[i].red   = 0;
              colrs[i].green = 0;
              colrs[i].blue  = 0;
            }
          data = ReadGifImage (fd, w, h, localColorMap,
                               BitSet (buf[8], INTERLACE), imageCount != imageNumber);
          return (data);	/* anticipating the exit to prevent gif video */
        }
      else
        {
          data = ReadGifImage (fd, w, h, GifScreen.ColorMap,
                               BitSet (buf[8], INTERLACE), imageCount != imageNumber);
          return (data);	/* anticipating the exit to prevent gif video */
        }
    }
  return (data);
}
#else /*_GL*/
/*----------------------------------------------------------------------
  ReadGIF
  ----------------------------------------------------------------------*/
unsigned char *ReadGIF (FILE *fd, int *w, int *h, int *ncolors, int *cpp,
                        ThotColorStruct *colrs)
{
  unsigned char       buf[16];
  unsigned char      *data;
  unsigned char       c;
  unsigned char       localColorMap[3][MAXCOLORMAPSIZE];
  int                 useGlobalColormap;
  int                 bitPixel;
  int                 imageCount = 0;
  char                version[4];
  int                 imageNumber = 1;
  int                 i;

  *w = 0;
  *h = 0;
  data = NULL;
  if (!ReadOK (fd, buf, 6)) 
    return (NULL);

  if (strncmp ((char *) buf, "GIF", 3) != 0)
    return (NULL);
  strncpy (version, (char *) buf + 3, 3);
  version[3] = EOS;
  if ((strcmp (version, "87a") != 0) && (strcmp (version, "89a") != 0))
    return (NULL);
   
  if (!ReadOK (fd, buf, 7))
    return (NULL);

  GifScreen.Width  = LM_to_uint (buf[0], buf[1]);
  GifScreen.Height = LM_to_uint (buf[2], buf[3]);
  GifScreen.BitPixel = 2 << (buf[4] & 0x07);
  *ncolors = GifScreen.BitPixel;
  GifScreen.ColorResolution = (((buf[4] & 0x70) >> 3) + 1);
  GifScreen.Background = buf[5];
  GifScreen.AspectRatio = buf[6];
  if (BitSet (buf[4], LOCALCOLORMAP))
    { /* Global Colormap */
      if (ReadColorMap (fd, GifScreen.BitPixel, GifScreen.ColorMap))
        return (NULL);
      for (i = 0; i < (int) GifScreen.BitPixel; i++)
        {
          colrs[i].red   = GifScreen.ColorMap[0][i] * COLORMAPSCALE;
          colrs[i].green = GifScreen.ColorMap[1][i] * COLORMAPSCALE;
          colrs[i].blue  = GifScreen.ColorMap[2][i] * COLORMAPSCALE;
#if defined(_GTK) || defined(_WX)
          colrs[i].pixel = i;
#endif /* #if defined(_GTK) || defined(_WX) */
        }

      for (i = GifScreen.BitPixel; i < MAXCOLORMAPSIZE; i++)
        {
          colrs[i].red = 0;
          colrs[i].green = 0;
          colrs[i].blue = 0;
#if defined(_GTK) || defined(_WX)
          colrs[i].pixel = i;
#endif /* #if defined(_GTK) || defined(_WX) */
        }
    }

  for (;;)
    {
      if (!ReadOK (fd, &c, 1))
        return (NULL);

      if (c == ';')
        { /* GIF terminator */
          if (imageCount < imageNumber)
            return (NULL);
          break;
        }

      if (c == '!')
        { /* Extension */
          if (!ReadOK (fd, &c, 1))
            return (NULL);
          DoExtension (fd, c);
          continue;
        }

      if (c != ',')
        continue;
      ++imageCount;
      if (!ReadOK (fd, buf, 9))
        return (NULL);

      useGlobalColormap = !BitSet (buf[8], LOCALCOLORMAP);
      bitPixel = 1 << ((buf[8] & 0x07) + 1);
      *w = LM_to_uint (buf[4], buf[5]);
      *h = LM_to_uint (buf[6], buf[7]);
      if (!useGlobalColormap)
        {
          if (ReadColorMap (fd, bitPixel, localColorMap))
            return (NULL);
          for (i = 0; i < bitPixel; i++)
            {
              colrs[i].red   = localColorMap[0][i] * COLORMAPSCALE;
              colrs[i].green = localColorMap[1][i] * COLORMAPSCALE;
              colrs[i].blue  = localColorMap[2][i] * COLORMAPSCALE;
#if defined(_GTK) || defined(_WX)
              colrs[i].pixel = i;
#endif /* #if defined(_GTK) || defined(_WX) */
            }

          for (i = bitPixel; i < MAXCOLORMAPSIZE; i++)
            {
              colrs[i].red   = 0;
              colrs[i].green = 0;
              colrs[i].blue  = 0;
#if defined(_GTK) || defined(_WX)
              colrs[i].pixel = i;
#endif /* #if defined(_GTK) || defined(_WX) */
            }
          data = ReadGifImage (fd, w, h, localColorMap,
                               BitSet (buf[8], INTERLACE), imageCount != imageNumber);
          return (data);	/* anticipating the exit to prevent gif video */
        }
      else
        {
          data = ReadGifImage (fd, w, h, GifScreen.ColorMap,
                               BitSet (buf[8], INTERLACE), imageCount != imageNumber);
          return (data);	/* anticipating the exit to prevent gif video */
        }
    }
  return (data);
}
#endif /*_GL*/
/*----------------------------------------------------------------------
  ReadColorMap
  ----------------------------------------------------------------------*/
int ReadColorMap (FILE *fd, int number, unsigned char buffer[3][MAXCOLORMAPSIZE])
{
  int                 i;
  unsigned char       rgb[3];

  for (i = 0; i < number; ++i)
    {
      if (!ReadOK (fd, rgb, sizeof (rgb)))
        return (TRUE);
      buffer[CM_RED][i]   = rgb[0];
      buffer[CM_GREEN][i] = rgb[1];
      buffer[CM_BLUE][i]  = rgb[2];
    }
  return FALSE;
}

/*----------------------------------------------------------------------
  DoExtension
  ----------------------------------------------------------------------*/
int DoExtension (FILE *fd, int label)
{
  unsigned char     buf[256];

  switch (label)
    {
    case 0x01: /* Plain Text Extension */
    case 0xff: /* Application Extension */
      break;
    case 0xfe: /* Comment Extension */
      while (GetDataBlock (fd, (unsigned char *) buf) != 0) ;
      return FALSE;
    case 0xf9: /* Graphic Control Extension */
      (void) GetDataBlock (fd, (unsigned char *) buf);
      if ((buf[0] & 0x1) != 0)
        GifTransparent = (int) buf[3];
      while (GetDataBlock (fd, (unsigned char *) buf) != 0) ;
      return FALSE;
    default:
      sprintf ((char *)buf, "UNKNOWN (0x%02x)", label);
      break;
    }
  
  while (GetDataBlock (fd, (unsigned char *) buf) != 0);
  return FALSE;
}

/*----------------------------------------------------------------------
  GetDataBlock
  ----------------------------------------------------------------------*/
int GetDataBlock (FILE *fd, unsigned char *buf)
{
  unsigned char       count;

  if (!ReadOK (fd, &count, 1))
    return -1;

  ZeroDataBlock = (count == 0);
  if ((count != 0) && (!ReadOK (fd, buf, count)))
    return -1;
  else
    return count;
}

/*----------------------------------------------------------------------
  GetCode
  ----------------------------------------------------------------------*/
int GetCode (FILE *fd, int code_size, int flag)
{
  static unsigned char buf[280];
  static int           curbit, lastbit, done, last_byte;
  int                  i, j, ret;
  unsigned char        count;

  if (flag)
    {
      curbit = 0;
      lastbit = 0;
      done = FALSE;
      last_byte = 2;
      return 0;
    }

  if ((curbit + code_size) >= lastbit)
    {
      if (done)
        return -1;
      buf[0] = buf[last_byte - 2];
      buf[1] = buf[last_byte - 1];
      if ((count = GetDataBlock (fd, &buf[2])) == 0)
        done = TRUE;
      last_byte = 2 + count;
      curbit = (curbit - lastbit) + 16;
      lastbit = (2 + count) * 8;
    }

  ret = 0;
  for (i = curbit, j = 0; j < code_size; ++i, ++j)
    ret |= ((buf[i >> 3] & (1 << (i % 8))) != 0) << j;
  curbit += code_size;
  return ret;
}

/*----------------------------------------------------------------------
  LWZReadByte
  ----------------------------------------------------------------------*/
int LWZReadByte (FILE *fd, int flag, int input_code_size)
{
  static int          fresh = FALSE;
  int                 code, incode;
  register int        i;

  if (flag)
    {
      set_code_size = input_code_size;
      code_size = set_code_size + 1;
      clear_code = 1 << set_code_size;
      end_code = clear_code + 1;
      max_code_size = 2 * clear_code;
      max_code = clear_code + 2;
      GetCode (fd, 0, TRUE);
      fresh = TRUE;

      for (i = 0; i < clear_code; ++i)
        {
          table[0][i] = 0;
          table[1][i] = i;
        }
      for (; i < (1 << MAX_LWZ_BITS); ++i)
        table[0][i] = table[1][0] = 0;
      sp = stack;
      return 0;
    }
  else if (fresh)
    {
      fresh = FALSE;
      do
        {
          firstcode = oldcode = GetCode (fd, code_size, FALSE);
        }
      while (firstcode == clear_code);
      return firstcode;
    }

  if (sp > stack)
    return *--sp;

  while ((code = GetCode (fd, code_size, FALSE)) >= 0)
    {
      if (code == clear_code)
        {
          for (i = 0; i < clear_code; ++i)
            {
              table[0][i] = 0;
              table[1][i] = i;
            }
          for (; i < (1 << MAX_LWZ_BITS); ++i)
            table[0][i] = table[1][i] = 0;
          code_size = set_code_size + 1;
          max_code_size = 2 * clear_code;
          max_code = clear_code + 2;
          sp = stack;
          firstcode = oldcode = GetCode (fd, code_size, FALSE);
          return firstcode;
        }
      else if (code == end_code)
        {
          int                 count;
          unsigned char       buf[260];

          if (ZeroDataBlock)
            return -2;
          while ((count = GetDataBlock (fd, buf)) > 0)
            ;
          return -2;
        }

      incode = code;
      if (code >= max_code)
        {
          *sp++ = firstcode;
          code = oldcode;
        }

      while (code >= clear_code)
        {
          if ((sp - stack) >= ((1 << (MAX_LWZ_BITS)) * 2))
            return -2;	/* stop a code dump */
          *sp++ = table[1][code];
          if (code == table[0][code])
            return (code);
          code = table[0][code];
        }
      *sp++ = firstcode = table[1][code];
      if ((code = max_code) < (1 << MAX_LWZ_BITS))
        {
          table[0][code] = oldcode;
          table[1][code] = firstcode;
          ++max_code;
          if ((max_code >= max_code_size) &&
              (max_code_size < (1 << MAX_LWZ_BITS)))
            {
              max_code_size *= 2;
              ++code_size;
            }
        }
      oldcode = incode;
      if (sp > stack)
        return *--sp;
    }
  return code;
}


#ifdef _WINGUI
/*----------------------------------------------------------------------
  highbit returns position of highest set bit in 'ul' as an integer (0-31),
  or -1 if none.     
  ----------------------------------------------------------------------*/
static int highbit (unsigned long ul)
{
  int                 i;

  for (i = 31; ((ul & 0x80000000) == 0) && i >= 0; i--, ul <<= 1) ;
  return i;
}
#endif /* _WINGUI */

/*----------------------------------------------------------------------
  highbit16 returns position of highest set bit in 'ul' as an integer (0-31),
  or -1 if none.          
  ----------------------------------------------------------------------*/
int highbit16 (unsigned long ul)
{
  int                 i;

  for (i = 15; ((ul & 0x8000) == 0) && i >= 0; i--, ul <<= 1) ;
  return i;
}

#ifdef _WINGUI
/*----------------------------------------------------------------------
  nbbits returns the width of a bit PicMask.
  ----------------------------------------------------------------------*/
static int nbbits (unsigned long ul)
{
  while (!(ul & 1))
    ul >>= 1;
  switch (ul)
    {
    case 0x00:
      return (0);
    case 0x01:
      return (1);
    case 0x03:
      return (2);
    case 0x07:
      return (3);
    case 0x0F:
      return (4);
    case 0x1F:
      return (5);
    case 0x3F:
      return (6);
    case 0x7F:
      return (7);
    case 0xFF:
      return (8);
    case 0x01FF:
      return (1 + 8);
    case 0x03FF:
      return (2 + 8);
    case 0x07FF:
      return (3 + 8);
    case 0x0FFF:
      return (4 + 8);
    case 0x1FFF:
      return (5 + 8);
    case 0x3FFF:
      return (6 + 8);
    case 0x7FFF:
      return (7 + 8);
    case 0xFFFF:
      return (8 + 8);
    default:
      fprintf (stderr, "gifhandler: nbbits : invalid PicMask\n");
      return (8);
    }
}
#endif /* _WINGUI */

#if defined(_GTK) && !defined(_GL)
/*----------------------------------------------------------------------
  Make a shape  of depth 1 for display from image data.
  The parameter bperpix gives the number of bytes per pixel.
  ----------------------------------------------------------------------*/
ThotPixmap MakeMask (Display *dsp, unsigned char *pixels, int w, int h,
                     unsigned int bg, int bperpix)
{
  XImage             *newmask;
  ThotPixmap          pmask;
  unsigned short     *spixels;
  unsigned char      *data_ptr, *max_data;
  unsigned int        col;
  char                value;
  char               *data;
  int                 diff, count, width, height;
  int                 bpl, y, ind;

  width  = w;
  height = h;
  if (bperpix > 2)
    /* no mask generated for RGB descriptor */
    return (ThotPixmap) NULL;
  newmask = XCreateImage (TtDisplay, theVisual, 1, ZPixmap, 0, 0, width, height, 8, 0);
  bpl = newmask->bytes_per_line;
  newmask->data = (char *) TtaGetMemory (bpl * height);
  data = newmask->data;
  ind = 0; /* pixel index */
  spixels = (unsigned short *)pixels;
  diff = width & 7;
  width >>= 3;
  if (newmask->bitmap_bit_order == MSBFirst)
    for (y = 0; y < height; y++)
      {
        data_ptr = (unsigned char *)data;
        max_data = data_ptr + width;
        if (bperpix == 1)
          {
            /* use one byte per pixel */
            while (data_ptr < max_data)
              {
                value = 0;
                col = pixels[ind++];
                value = ((value << 1) | (col != bg));
                col = pixels[ind++];
                value = ((value << 1) | (col != bg));
                col = pixels[ind++];
                value = ((value << 1) | (col != bg));
                col = pixels[ind++];
                value = ((value << 1) | (col != bg));
                col = pixels[ind++];
                value = ((value << 1) | (col != bg));
                col = pixels[ind++];
                value = ((value << 1) | (col != bg));
                col = pixels[ind++];
                value = ((value << 1) | (col != bg));
                col = pixels[ind++];
                value = ((value << 1) | (col != bg));
                *(data_ptr++) = value;
              }
          }
        else
          {
            /* use two bytes per pixel */
            while (data_ptr < max_data)
              {
                value = 0;
                col = spixels[ind++];
                value = ((value << 1) | (col != bg));
                col = spixels[ind++];
                value = ((value << 1) | (col != bg));
                col = spixels[ind++];
                value = ((value << 1) | (col != bg));
                col = spixels[ind++];
                value = ((value << 1) | (col != bg));
                col = spixels[ind++];
                value = ((value << 1) | (col != bg));
                col = spixels[ind++];
                value = ((value << 1) | (col != bg));
                col = spixels[ind++];
                value = ((value << 1) | (col != bg));
                col = spixels[ind++];
                value = ((value << 1) | (col != bg));
                *(data_ptr++) = value;
              }
          }
        if (diff)
          {
            value = 0;
            for (count = 0; count < diff; count++)
              {
                if (bperpix == 1)
                  col = pixels[ind++];
                else
                  col = spixels[ind++];
                if (col != bg)
                  value |= (0x80 >> count);
              }
            *(data_ptr++) = value;
          }
        data += bpl;
      }
  else
    {
      for (y = 0; y < height; y++)
        {
          data_ptr = (unsigned char *)data;
          max_data = data_ptr + width;
          if (bperpix == 1)
            {
              /* use one byte per pixel */
              while (data_ptr < max_data)
                {
                  value = 0;
                  ind += 8;
                  col = pixels[--ind];
                  value = ((value << 1) | (col != bg));
                  col = pixels[--ind];
                  value = ((value << 1) | (col != bg));
                  col = pixels[--ind];
                  value = ((value << 1) | (col != bg));
                  col = pixels[--ind];
                  value = ((value << 1) | (col != bg));
                  col = pixels[--ind];
                  value = ((value << 1) | (col != bg));
                  col = pixels[--ind];
                  value = ((value << 1) | (col != bg));
                  col = pixels[--ind];
                  value = ((value << 1) | (col != bg));
                  col = pixels[--ind];
                  value = ((value << 1) | (col != bg));
                  ind += 8;
                  *(data_ptr++) = value;
                }
            }
          else
            {
              /* use two bytes per pixel */
              while (data_ptr < max_data)
                {
                  value = 0;
                  ind += 8;
                  col = spixels[--ind];
                  value = ((value << 1) | (col != bg));
                  col = spixels[--ind];
                  value = ((value << 1) | (col != bg));
                  col = spixels[--ind];
                  value = ((value << 1) | (col != bg));
                  col = spixels[--ind];
                  value = ((value << 1) | (col != bg));
                  col = spixels[--ind];
                  value = ((value << 1) | (col != bg));
                  col = spixels[--ind];
                  value = ((value << 1) | (col != bg));
                  col = spixels[--ind];
                  value = ((value << 1) | (col != bg));
                  col = spixels[--ind];
                  value = ((value << 1) | (col != bg));
                  ind += 8;
                  *(data_ptr++) = value;
                }
            }
          if (diff)
            {
              value = 0;
              for (count = 0; count < diff; count++)
                {
                  if (bperpix == 1)
                    col = pixels[ind++];
                  else
                    col = spixels[ind++];
                  if (col != bg)
                    value |= (1 << count);
                }
              *(data_ptr++) = value;
            }
          data += bpl;
        }
    }
  return (pmask);
}
#endif /* _GTK && !_GL*/

#ifdef _WINGUI
/*----------------------------------------------------------------------
  Make an image of appropriate depth for display from image data.
  The parameter ncolors gives the number of colors in the image.
  ----------------------------------------------------------------------*/
static HBITMAP WIN_MakeImage (HDC hDC, unsigned char *data, int width,
                              int height, int depth, ThotColorStruct *colrs,
                              int ncolors, ThotBool withAlpha, ThotBool grayScale)
{
  HBITMAP             newimage;
  unsigned char      *bit_data, *bitp;
  unsigned char       r, g, b, tr, tg, tb, alpha;
  unsigned int        col;
  int                 temp, w, h, ind;
  int                 linepad;
  int                 bytesperline;
  int                 rshift, gshift, bshift;
  unsigned short      rmask, gmask, bmask, tcolor;

  bit_data = NULL;
  /* this is an approximation for alpha channel support (please use OpenGL)
     by default there is no transparent color */
  PngTransparentColor = -1;
  tcolor = FALSE;
  switch (depth)
    {
    case 1:
    case 2:
    case 4:
    case 8:
      /* translate image palette to the system palette  */
      for (ind = 0; ind < ncolors; ind++)
        colrs[ind].pixel = GetSystemColorIndex (colrs[ind].red,
                                                colrs[ind].green,
                                                colrs[ind].blue);
      if (width % 2)
        linepad = 1;
      else 
        linepad = 0;
      bit_data = (unsigned char *) TtaGetMemory (((width + linepad) * height));
      bitp   = bit_data;
      ind = 0; /* pixel index */
      for (h = 0; h < height; h++)
        {
          for (w = 0; w < width; w++)
            {
              /* use one byte per pixel */
              col = data[ind++];
              *bitp++ = (unsigned char) colrs[col].pixel & 0xff;
            }
          if (linepad)
            *bitp++ = 0;	    
        }
      bytesperline = (width + linepad) * depth / 8;
      break;

    case 15:
    case 16:
      bit_data = (unsigned char *) TtaGetMemory (width * height * 2);
      bitp   = bit_data;
      ind = 0; /* pixel index */
      if (depth == 15)
        {
          rshift = 1;
          gshift = 6;
          bshift = 11;
          /* For 15 bit, Windows seems to be using 16 bit pixels */
          depth = 16;
        }
      else
        {
          rshift = 0;
          gshift = 5;
          bshift = 11;
        }
      for (w = (width * height); w > 0; w--)
        {
          if (ncolors == 0)
            {
              /* read the RGB from the data descriptor */
              r = data[ind++];
              if (grayScale)
                g = b = r;
              else
                {
                  g = data[ind++];
                  b = data[ind++];
                }
              if (withAlpha)
                {
                  /* skip the alpha channel */
                  alpha = data[ind++];
                  if (alpha < 127)
                    {
                      if (tcolor)
                        {
                          /* use the transparent color */
                          b = tb;
                          g = tg;
                          r = tr;
                        }
                      else
                        {
                          /* define the transparent color */
                          tb = b;
                          tg = g;
                          tr = r;
                          tcolor = TRUE;
                        }
                    }
                }
            }
          else
            {
              /* use one byte per pixel */
              col = data[ind++];
              r = colrs[col].red;
              g = colrs[col].green;
              b = colrs[col].blue;
            }
          rmask = ((r << 8) >> rshift) & 0xf800;
          gmask = ((g << 8) >> gshift) & 0x7e0;
          bmask = ((b << 8) >> bshift) & 0x1f;
          temp = (rmask | gmask | bmask);
          *bitp++ = temp & 0xff;
          *bitp++ = (temp >> 8) & 0xff;
        }
      break;
    case 24:
      bit_data = (unsigned char *) TtaGetMemory (width * height * 4);
      bitp   = bit_data;
      ind = 0; /* pixel index */
      for (h = height; h > 0; h--)
        {
          for (w = width; w > 0; w--)
            {
              if (ncolors == 0)
                {
                  /* read the RGB from the data descriptor */
                  r = data[ind++];
                  if (grayScale)
                    g = b = r;
                  else
                    {
                      g = data[ind++];
                      b = data[ind++];
                    }
                  if (withAlpha)
                    {
                      /* skip the alpha channel */
                      alpha = data[ind++];
                      if (alpha < 127)
                        {
                          if (tcolor)
                            {
                              /* use the transparent color */
                              b = tb;
                              g = tg;
                              r = tr;
                            }
                          else
                            {
                              /* define the transparent color */
                              tb = b;
                              tg = g;
                              tr = r;
                              tcolor = TRUE;
                            }
                        }
                    }
                  *bitp++ = b;
                  *bitp++ = g;
                  *bitp++ = r;
                }
              else
                {
                  /* use one byte per pixel */
                  col = data[ind++];
                  *bitp++ = colrs[col].blue;
                  *bitp++ = colrs[col].green;
                  *bitp++ = colrs[col].red;
                }
            }
          if (width % 2 != 0) 
            *bitp++ = 0;
        }
      break;
      
    case 32:
      bit_data = (unsigned char *) TtaGetMemory (width * height * 4);
      bitp   = bit_data;
      ind = 0; /* pixel index */
      for (h = height; h > 0; h--)
        {
          for (w = width; w > 0; w--)
            {
              if (ncolors == 0)
                {
                  /* read the RGB from the data descriptor */
                  r = data[ind++];
                  if (grayScale)
                    g = b = r;
                  else
                    {
                      g = data[ind++];
                      b = data[ind++];
                    }
                  if (withAlpha)
                    {
                      /* skip the alpha channel */
                      alpha = data[ind++];
                      if (alpha < 127)
                        {
                          if (tcolor)
                            {
                              /* use the transparent color */
                              b = tb;
                              g = tg;
                              r = tr;
                            }
                          else
                            {
                              /* define the transparent color */
                              tb = b;
                              tg = g;
                              tr = r;
                              tcolor = TRUE;
                            }
                        }
                    }
                  *bitp++ = b;
                  *bitp++ = g;
                  *bitp++ = r;
                }
              else
                {
                  /* use one byte per pixel */
                  col = data[ind++];
                  *bitp++ = colrs[col].blue;
                  *bitp++ = colrs[col].green;
                  *bitp++ = colrs[col].red;
                }
              *bitp++ = 0;
            }
        }
      break;
    }

  if (tcolor)
    /* get the pseudo transparent color */
    PngTransparentColor = TtaGetThotColor (tr, tg, tb);
  newimage = CreateCompatibleBitmap (hDC, width, height);
  if (depth < 8)
    SetBitmapBits (newimage, width * height, bit_data);
  else
    SetBitmapBits (newimage, width * height * (depth / 8), bit_data);
  TtaFreeMemory (bit_data);
  return (newimage);
}
#endif /* _WINGUI */

/*----------------------------------------------------------------------
  Allocate and return the thotColors table.
  The parameter bg gives the index of the transparent color in the
  image colormap.
  ----------------------------------------------------------------------*/
ThotPixmap DataToPixmap (unsigned char *image_data, int width, int height,
                         int ncolors, ThotColorStruct *colrs,
                         ThotBool withAlpha, ThotBool grayScale)
{ 
#ifdef _GTK
  ThotPixmap          img;
  unsigned long       FgPixel;
  unsigned long       BgPixel;
  ThotColorStruct     gdkFgPixel;
  ThotColorStruct     gdkBgPixel;
  FgPixel = ColorPixel (0);
  BgPixel = ColorPixel (1);
  gdkFgPixel.pixel = gdk_rgb_xpixel_from_rgb (FgPixel);
  gdkBgPixel.pixel = gdk_rgb_xpixel_from_rgb (BgPixel);
  /* TODO */
  img = (ThotPixmap)gdk_pixmap_create_from_data (DefaultDrawable,
                                                 (const gchar*)image_data,
                                                 width, height, TtWDepth,
                                                 (GdkColor *)&gdkFgPixel,
                                                 (GdkColor *)&gdkBgPixel);
  return (img);
#endif /* _GTK */
#ifdef _WINGUI
  return WIN_MakeImage (TtDisplay, image_data, width, height, TtWDepth,
                        colrs, ncolors, withAlpha, grayScale);
#endif /* _WINGUI */
#ifdef _WX
  return 0;
#endif /* _WX */
}


/*----------------------------------------------------------------------
  ReadGifToData decomresses the file and returns the picture data 
  ----------------------------------------------------------------------*/
unsigned char *ReadGifToData (char *datafile, int *w, int *h, int *ncolors,
                              int *cpp, ThotColorStruct *colrs)
{
  unsigned char      *bit_data;
  FILE               *fp;

  fp = TtaReadOpen (datafile);
  if (fp != NULL)
    {
      bit_data = ReadGIF (fp, w, h, ncolors, cpp, colrs);
      if (bit_data != NULL)
        {
          if (fp != stdin)
            TtaReadClose (fp);
          return (bit_data);
        }
      if (fp != stdin)
        TtaReadClose (fp);
    }
  return (NULL);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
ThotDrawable GifCreate (char *fn, ThotPictInfo *imageDesc, int *b_w, int *b_h,
                        int *wif, int *hif, int bgColor, int *width,
                        int *height, int zoom)
{
  ThotColorStruct     colrs[256];
#ifdef _GL
  unsigned char      *ptr, *cols;
  unsigned short      red, green, blue;
  int                 x, y;
#else /* _GL */
  ThotPixmap          pixmap = (ThotPixmap) NULL;
#ifdef _WINGUI
  unsigned short      red, green, blue;
#endif /* _WINGUI */
#endif /*_GL*/
  unsigned char      *buffer = NULL;
  unsigned char      *data = NULL;
  int                 w, h;
  int                 i;
  int                 ncolors, cpp;

  GifTransparent = -1;
  buffer = ReadGifToData (fn, &w, &h, &ncolors, &cpp, colrs);
  /* return image dimensions */
  *width = w;
  *height = h;
  if (buffer == NULL)
    return ((ThotDrawable) NULL);

  if (zoom && *b_w == 0 && *b_h == 0)
    {
      /* take zoom into account */
      *b_w = PixelValue (w, UnPixel, NULL, zoom);
      *b_h = PixelValue (h, UnPixel, NULL, zoom);
    }
  else
    {
      if (*b_w == 0 && *b_h != 0)
        *b_w = PixelValue (w, UnPixel, NULL, zoom);
      if (*b_w != 0 && *b_h == 0) 
        *b_h = PixelValue (h, UnPixel, NULL, zoom);
    }

#ifndef _WIN_PRINT
#ifndef _GL
  if (*b_w != 0 && *b_h != 0 && (w != *b_w || h != *b_h))
    {
      /* b_w and b_h contain width and height of the box */	  
      if ((*b_w * *b_h) > 4000000 )
        {
          i = 4000000 / (*b_w * *b_h);
          *b_w = i * *b_w;
          *b_h = i * *b_h;
        }
      data = ZoomPicture (buffer, w , h, *b_w, *b_h, 1);
      TtaFreeMemory (buffer);
      buffer = data;
      data = NULL;
      w = *b_w;
      h = *b_h;
    }
#endif /*_GL*/
#endif /* _WIN_PRINT */
  
  if (buffer == NULL)
    return ((ThotDrawable) NULL);	

#ifdef _GL
  cols = (unsigned char *)TtaGetMemory (w * h * 4);
  if (GifTransparent != -1 && Printing)
    {
      /* replace the transparent color by the background color */
      TtaGiveThotRGB (bgColor, &red, &green, &blue);
      colrs[GifTransparent].red   = red << 8;
      colrs[GifTransparent].green = green << 8;
      colrs[GifTransparent].blue  = blue << 8;
    }
  ptr = cols;
  y = h;
  data = buffer;
  while (y--)
    {
      data = buffer+ y * w;
      for (x = 0; x < w; x++)
        {		
          i = *data++;
          if (GifTransparent == i)
            {
              *ptr++ = (unsigned char)colrs[i].red;
              *ptr++ = (unsigned char)colrs[i].green;
              *ptr++ = (unsigned char)colrs[i].blue;	
              *ptr++ = (unsigned char)0;
            }
          else
            {
              *ptr++ = (unsigned char)colrs[i].red;
              *ptr++ = (unsigned char)colrs[i].green;
              *ptr++ = (unsigned char)colrs[i].blue;	
              *ptr++ = (unsigned char)255;
            }
        }
    }
  TtaFreeMemory (buffer);
  if (cols)
    {
      *wif = w;
      *hif = h;
      *b_w = 0;
      *b_h = 0;
    }
  return (ThotDrawable) cols;
#else /* _GL */
  pixmap = DataToPixmap (buffer, w, h, ncolors, colrs, FALSE, FALSE);
  if (GifTransparent >= 0)
    {
#ifdef _WINGUI
      if (Printing)
        {
          i = bgColor;
          TtaGiveThotRGB (bgColor, &red, &green, &blue);
          colrs[GifTransparent].red   = (unsigned char) red;
          colrs[GifTransparent].green = (unsigned char) green;
          colrs[GifTransparent].blue  = (unsigned char) blue;
          colrs[GifTransparent].pixel = ColorPixel (bgColor);
        }
      else
        /* register the Thot color index of the transparent color */
        i = TtaGetThotColor (colrs[GifTransparent].red, colrs[GifTransparent].green,
                             colrs[GifTransparent].blue);
      imageDesc->PicBgMask = i;
#endif /* _WINGUI */
#ifdef _GTK
      /* register the transparent mask */
      imageDesc->PicMask = MakeMask (TtDisplay, buffer, w, h, GifTransparent, 1);
#endif /* _GTK */
    }
  TtaFreeMemory (buffer);

  if (pixmap != None)
    {
      *wif = w;
      *hif = h;
      *b_w = 0;
      *b_h = 0;
    }
  return (ThotDrawable) pixmap;
#endif /*_GL*/
}


/*----------------------------------------------------------------------
  Print generates the PostScirpt of an image
  ----------------------------------------------------------------------*/
void DataToPrint (unsigned char *data, PictureScaling pres, int b_w, int b_h,
                  int wif, int hif, int picW, int picH, FILE *fd, int ncolors,
                  int transparent, int bgColor, ThotColorStruct *colrs,
                  ThotBool withAlpha, ThotBool grayScale)
{
  int              delta;
  int              xtmp, ytmp;
  int	           col, ind;
  int              x, y;
  unsigned short   red, green, blue;
  unsigned char    r, g, b;

  if (transparent != -1 && ncolors && transparent < ncolors)
    {
      TtaGiveThotRGB (bgColor, &red, &green, &blue);
      colrs[transparent].red   = red << 8;
      colrs[transparent].green = green << 8;
      colrs[transparent].blue  = blue << 8;
    }
  xtmp = 0;
  ytmp = 0;
  switch (pres)
    {
    case RealSize:
    case FillFrame:
    case XRepeat:
    case YRepeat:      
      delta = (wif - picW)/2;
      if (delta > 0)
        {
          b_w += delta;
          wif = picW;
        }
      else
        {
          xtmp = -delta;
          picW = wif;
        }     
      delta = (hif - picH)/2;
      if (delta > 0)
        {
          b_h += delta;
          hif = picH;
        }
      else
        {

          ytmp = - delta;
          picH = hif;
        } 

      fprintf(fd, "gsave %d -%d translate\n", b_w, b_h + hif);
      fprintf (fd, "%d %d %d %d DumpImage2\n", picW, picH, wif, hif);
      break;
    case ReScale:
      fprintf (fd, "gsave %d -%d translate\n", b_w, b_h + hif);
      fprintf (fd, "%d %d %d %d DumpImage2\n", picW, picH, wif, hif);
      wif = picW;
      hif = picH;
      break;
    default:
      break;
    }

  fprintf(fd, "\n");
  if (ncolors == 0)
    {
      if (withAlpha && grayScale)
        {
          /* using 2 bytes per pixel */
          xtmp = xtmp * 2;
          picW = picW * 2;
        }
      else if (grayScale)
        {
          /* using 1 bytes per pixel */
          xtmp = xtmp;
          picW = picW;
        }
      else if (withAlpha)
        {
          /* using 4 bytes per pixel */
          xtmp = xtmp * 4;
          picW = picW * 4;
        }
      else
        {
          /* using 3 bytes per pixel */
          xtmp = xtmp * 3;
          picW = picW * 3;
        }
    }
  for (y = 0 ; y < hif; y++)
    {
      ind = ((ytmp + y) * picW) + xtmp;
      for (x = 0 ; x < wif; x++)
        {
          if (ncolors == 0)
            {
              /* read the RGB from the data descriptor */
              r = data[ind++];
              if (grayScale)
                g = b = r;
              else
                {
                  g = data[ind++];
                  b = data[ind++];
                }
              if (withAlpha)
                ind++;
              fprintf (fd, "%02x%02x%02x", r, g, b);
            }
          else
            {
              if (grayScale)
                {
                  /* use one byte per pixel */
                  col = data[ind++];
                  fprintf (fd, "%02x%02x%02x",
                           colrs[col].red,
                           colrs[col].green,
                           colrs[col].blue);
                }
              else
                {
                  /* use one byte per pixel */
                  col = data[ind++];
                  fprintf (fd, "%02x%02x%02x",
                           colrs[col].red >> 8,
                           colrs[col].green >> 8,
                           colrs[col].blue >> 8);
                }
            }
        }
      fprintf(fd, "\n");
    }
  fprintf(fd, "\n");
  fprintf(fd, "grestore\n");
  fprintf(fd, "\n");   
}

/*----------------------------------------------------------------------
  GifPrint reads a gif file and produces PostScirpt      
  ----------------------------------------------------------------------*/
void GifPrint (char *fn, PictureScaling pres, int b_w, int b_h, int wif,
               int hif, FILE *fd, int bgColor)
{
#if defined(_GTK) || defined(_WX)
  ThotColorStruct     colrs[256];
  unsigned char      *data;
  int                 picW, picH;
  int                 ncolors, cpp;

  GifTransparent = -1;
  data = ReadGifToData (fn, &picW, &picH, &ncolors, &cpp, colrs);
  if (data)
    DataToPrint (data, pres, b_w, b_h, wif, hif, picW, picH, fd, ncolors,
                 GifTransparent, bgColor, colrs, FALSE, FALSE);
  TtaFreeMemory (data);
#endif /* defined(_GTK) || defined(_WX) */
}

/*----------------------------------------------------------------------
  IsGifFormat  checks the header file if it's of a gif file       
  ----------------------------------------------------------------------*/
ThotBool IsGifFormat (char *datafile)
{
  unsigned char       buf[16];
  FILE               *fp;
  char                version[4];

  fp = TtaReadOpen (datafile);
  if (fp == NULL)
    {
      if (fp != stdin)
        TtaReadClose (fp);
      return FALSE;
    }
  if (!ReadOK (fp, buf, 6))
    {
      if (fp != stdin)
        TtaReadClose (fp);
      return FALSE;
    }
  if (strncmp ((char *) buf, "GIF", 3) != 0)
    {
      if (fp != stdin)
        TtaReadClose (fp);
      return (FALSE);
    }

  strncpy (version, (char *) buf + 3, 3);
  version[3] = EOS;
  if ((strcmp (version, "87a") != 0) && (strcmp (version, "89a") != 0))
    {
      if (fp != stdin)
        TtaReadClose (fp);
      return (FALSE);
    }
  else
    {
      if (fp != stdin)
        TtaReadClose (fp);
      return (TRUE);
    }
}
