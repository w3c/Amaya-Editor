/* 
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Warning:
 * This module is part of the Thot library, which was originally
 * developed in French. That's why some comments are still in
 * French, but their translation is in progress and the full module
 * will be available in English in the next release.
 * 
 */

/*
 *
 * Authors: I. Vatton, N. Layaida (INRIA)
 *          R. Guetari (W3C/INRIA) - Unicode and Windows version
 */
 
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "picture.h"
#include "frame.h"
#include "libmsg.h"
#include "message.h"
#include "application.h"
#include "xpm.h"
#include "thotcolor.h"
#ifdef _WINDOWS
#include "winsys.h"
#endif /* _WINDOWS */

#define THOT_EXPORT extern
#include "picture_tv.h"
#include "frame_tv.h"
#include "thotcolor_tv.h"

#include "font_f.h"
#include "units_f.h"
#include "inites_f.h"
#include "gifhandler_f.h"
#include "picture_f.h"
#ifdef _WINDOWS
#include "wininclude.h"
#endif /* _WINDOWS */

#define	MAXCOLORMAPSIZE		256
#define scale 65536 / MAXCOLORMAPSIZE;
#define CM_RED		0
#define CM_GREEN	1
#define CM_BLUE		2
#define	MAX_LWZ_BITS		12
#define INTERLACE		0x40
#define LOCALCOLORMAP	0x80
#define BitSet(byte, bit)	(((byte) & (bit)) == (bit))
#define	ReadOK(file,buffer,len)	(fread(buffer, len, 1, file) > 0)
#define LM_to_uint(a,b)		(((b)<<8)|(a))

struct {
     unsigned int        Width;
     unsigned int        Height;
     unsigned char       ColorMap[3][MAXCOLORMAPSIZE];
     unsigned int        BitPixel;
     unsigned int        ColorResolution;
     unsigned int        Background;
     unsigned int        AspectRatio;
} GifScreen;

struct {
     int                 transparent;
     int                 delayTime;
     int                 inputFlag;
     int                 disposal;
} Gif89 = {
  -1, -1, -1, 0
};

int                 verbose;
int                 showComment;
unsigned char       nibMask[8] = {1, 2, 4, 8, 16, 32, 64, 128};
int                 ZeroDataBlock = FALSE;

static int          code_size, set_code_size;
static int          max_code, max_code_size;
static int          firstcode, oldcode;
static int          clear_code, end_code;
static int          table[2][(1 << MAX_LWZ_BITS)];
static int          stack[(1 << (MAX_LWZ_BITS)) * 2], *sp = stack;

#ifdef _WINDOWS
#define MAXNUMBER 256
#define ALIGNLONG(i) ((i+3)/4*4)

static signed int   tabCorres [256];
static PALETTEENTRY sysPalEntries[256];
ThotBool            peInitialized = FALSE;
static int          nbSysColors;
static int          best_dsquare = INT_MAX;

/* ----------------------------------------------------------------------
  WIN_InitSysColors						
  ---------------------------------------------------------------------- */
int WIN_InitSystemColors (HDC hDC)
{
    if (peInitialized)
       return 1;

    if (!(GetDeviceCaps (hDC, RASTERCAPS) & RC_PALETTE))
       return 1;

    nbSysColors = GetSystemPaletteEntries (hDC, 0, GetDeviceCaps (hDC, SIZEPALETTE), sysPalEntries);

    peInitialized = TRUE;
    return 0;
}

/* ---------------------------------------------------------------------- 
   WIN_GetColorIndex
   ---------------------------------------------------------------------- */
BYTE WIN_GetColorIndex (int r, int g, int b)
{
    int                 best;
    int                 i;
    unsigned int      delred, delgreen, delblue;
    unsigned int        dsquare;
    unsigned int        best_dsquare = (unsigned int) -1;

    for (i = 0; i < nbSysColors; i++) {
        delred   = sysPalEntries[i].peRed;
        delgreen = sysPalEntries[i].peGreen;
		delblue  = sysPalEntries[i].peBlue;
        delred   -= r;
        delgreen -= g;
        delblue  -= b;
        dsquare  = delred * delred + delgreen * delgreen + delblue * delblue;
        if (dsquare < best_dsquare) {
           best = i;
           best_dsquare = dsquare;
        }
    }
    return ((BYTE)best);
}
#endif /* _WINDOWS */

/*----------------------------------------------------------------------
  ReadGifImage
  ----------------------------------------------------------------------*/
static unsigned char* ReadGifImage (FILE* fd, int len, int height,
									unsigned char clmap[3][MAXCOLORMAPSIZE],
									int interlace, int ignore)
{
  unsigned char       c;
  int                 v;
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
    {
      while (LWZReadByte (fd, FALSE, c) >= 0)
	;
      return (NULL);
    }

  data = (unsigned char*) TtaGetMemory (sizeof (unsigned char) * len * height);

  if (data == NULL)
    return (NULL);

  while ((v = LWZReadByte (fd, FALSE, c)) >= 0)
    {
      dptr = (unsigned char *) (data + (ypos * len) + xpos);
      *dptr = (unsigned char) v;
      
      ++xpos;
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
		      goto fini;
		    }
		}
	    }
	  else
	    ++ypos;
	}
      if (ypos >= height)
	break;
    }

 fini:
  if (LWZReadByte (fd, FALSE, c) >= 0)
    fprintf (stderr, "gifhandler: too much input data, ignoring extra...\n");
  return (data);
}

/*----------------------------------------------------------------------
  ReadGIF
  ----------------------------------------------------------------------*/
unsigned char *ReadGIF (FILE* fd, int* w, int* h, int* ncolors, int* cpp,
			ThotColorStruct colrs[256])
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
   verbose = FALSE;
   showComment = FALSE;
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

   if (BitSet (buf[4], LOCALCOLORMAP)) { /* Global Colormap */
      if (ReadColorMap (fd, GifScreen.BitPixel, GifScreen.ColorMap))
         return (NULL);
      for (i = 0; i < (int) GifScreen.BitPixel; i++) {
#         ifndef _WINDOWS
          colrs[i].red   = GifScreen.ColorMap[0][i] * scale;
          colrs[i].green = GifScreen.ColorMap[1][i] * scale;
          colrs[i].blue  = GifScreen.ColorMap[2][i] * scale;
          colrs[i].pixel = i;
#ifndef _GTK
          colrs[i].flags = DoRed | DoGreen | DoBlue;
#endif /* !_GTK */
#         else  /* _WINDOWS */
          colrs[i].red   = GifScreen.ColorMap[0][i];
          colrs[i].green = GifScreen.ColorMap[1][i];
          colrs[i].blue  = GifScreen.ColorMap[2][i];
#         endif /* !_WINDOWS */
	  }

      for (i = GifScreen.BitPixel; i < MAXCOLORMAPSIZE; i++) {
          colrs[i].red = 0;
          colrs[i].green = 0;
          colrs[i].blue = 0;
#         ifndef _WINDOWS
          colrs[i].pixel = i;
#ifndef _GTK
          colrs[i].flags = DoRed | DoGreen | DoBlue;
#endif /* !_GTK */
#         endif /* !_WINDOWS */
	  }

   }

   for (;;) {
       if (!ReadOK (fd, &c, 1))
          return (NULL);

       if (c == ';') { /* GIF terminator */
          if (imageCount < imageNumber)
             return (NULL);
          break;
	   }

       if (c == '!') { /* Extension */
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
       if (!useGlobalColormap) {
          if (ReadColorMap (fd, bitPixel, localColorMap))
             return (NULL);
          for (i = 0; i < bitPixel; i++) {
#             ifndef _WINDOWS
              colrs[i].red   = localColorMap[0][i] * scale;
              colrs[i].green = localColorMap[1][i] * scale;
              colrs[i].blue  = localColorMap[2][i] * scale;
              colrs[i].pixel = i;
#ifndef _GTK
              colrs[i].flags = DoRed | DoGreen | DoBlue;
#endif /* !_GTK */
#             else  /* _WINDOWS */
              colrs[i].red   = GifScreen.ColorMap[0][i];
              colrs[i].green = GifScreen.ColorMap[1][i];
              colrs[i].blue  = GifScreen.ColorMap[2][i];
#             endif /* _WINDOWS */
		  }
          for (i = bitPixel; i < MAXCOLORMAPSIZE; i++) {
              colrs[i].red   = 0;
              colrs[i].green = 0;
              colrs[i].blue  = 0;
#             ifndef _WINDOWS
              colrs[i].pixel = i;
#ifndef _GTK
              colrs[i].flags = DoRed | DoGreen | DoBlue;
#endif /* !_GTK */
#             endif /* _WINDOWS */
		  }
          data = ReadGifImage (fd, LM_to_uint (buf[4], buf[5]),
                               LM_to_uint (buf[6], buf[7]), localColorMap,
                               BitSet (buf[8], INTERLACE), imageCount != imageNumber);
          return (data);	/* anticipating the exit to prevent gif video */
	   } else {
             data = ReadGifImage (fd, LM_to_uint (buf[4], buf[5]),
                                  LM_to_uint (buf[6], buf[7]), GifScreen.ColorMap,
                                  BitSet (buf[8], INTERLACE), imageCount != imageNumber);
             return (data);	/* anticipating the exit to prevent gif video */
	   }
	
   }
   return (data);
}

/*----------------------------------------------------------------------
  ReadColorMap
  ----------------------------------------------------------------------*/
int ReadColorMap (FILE* fd, int number, unsigned char buffer[3][MAXCOLORMAPSIZE])
{
   int                 i;
   unsigned char       rgb[3];

   for (i = 0; i < number; ++i) {
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
int DoExtension (FILE* fd, int label)
{
   unsigned char                buf[256];

   switch (label) {
           case 0x01: /* Plain Text Extension */
           case 0xff: /* Application Extension */
                      break;

           case 0xfe: /* Comment Extension */
                while (GetDataBlock (fd, (unsigned char *) buf) != 0) ;
                return FALSE;

           case 0xf9: /* Graphic Control Extension */
                      (void) GetDataBlock (fd, (unsigned char *) buf);
                      Gif89.disposal = (buf[0] >> 2) & 0x7;
                      Gif89.inputFlag = (buf[0] >> 1) & 0x1;
                      Gif89.delayTime = LM_to_uint (buf[1], buf[2]);
                      if ((buf[0] & 0x1) != 0)
                         Gif89.transparent = (int) buf[3];

                      while (GetDataBlock (fd, (unsigned char *) buf) != 0) ;
                      return FALSE;

           default: sprintf (buf, "UNKNOWN (0x%02x)", label);
                    break;
   }

   while (GetDataBlock (fd, (unsigned char *) buf) != 0);
   return FALSE;
}

/*----------------------------------------------------------------------
  GetDataBlock
  ----------------------------------------------------------------------*/
int GetDataBlock (FILE* fd, unsigned char *buf)
{
   unsigned char       count;

   if (!ReadOK (fd, &count, 1)) {
      fprintf (stderr, "gifhandler: error in getting DataBlock size\n");
      return -1;
   }

   ZeroDataBlock = (count == 0);
   if ((count != 0) && (!ReadOK (fd, buf, count)))
      return -1;
   else
      return count;
}

/*----------------------------------------------------------------------
  GetCode
  ----------------------------------------------------------------------*/
int GetCode (FILE* fd, int code_size, int flag)
{
   static unsigned char buf[280];
   static int           curbit, lastbit, done, last_byte;
   int                  i, j, ret;
   unsigned char        count;

   if (flag) {
      curbit = 0;
      lastbit = 0;
      done = FALSE;
      last_byte = 2;
      return 0;
   }

   if ((curbit + code_size) >= lastbit) {
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
int LWZReadByte (FILE * fd, int flag, int input_code_size)
{
   static int          fresh = FALSE;
   int                 code, incode;
   register int        i;

   if (flag) {
      set_code_size = input_code_size;
      code_size = set_code_size + 1;
      clear_code = 1 << set_code_size;
      end_code = clear_code + 1;
      max_code_size = 2 * clear_code;
      max_code = clear_code + 2;

      GetCode (fd, 0, TRUE);
      
      fresh = TRUE;

      for (i = 0; i < clear_code; ++i) {
	  table[0][i] = 0;
	  table[1][i] = i;
      }
      for (; i < (1 << MAX_LWZ_BITS); ++i)
	  table[0][i] = table[1][0] = 0;

      sp = stack;
      
      return 0;
   }
   else if (fresh) {
	fresh = FALSE;
	do {
	   firstcode = oldcode = GetCode (fd, code_size, FALSE);
	} while (firstcode == clear_code);
	return firstcode;
   }

   if (sp > stack)
      return *--sp;

   while ((code = GetCode (fd, code_size, FALSE)) >= 0) {
	 if (code == clear_code) {
	    for (i = 0; i < clear_code; ++i) {
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
	 } else if (code == end_code) {
	        int                 count;
		unsigned char       buf[260];

		if (ZeroDataBlock)
		   return -2;
		while ((count = GetDataBlock (fd, buf)) > 0)
		      ;
		return -2;
	 }

	incode = code;
	if (code >= max_code) {
	   *sp++ = firstcode;
	   code = oldcode;
	}

	while (code >= clear_code) {
	      if ((sp - stack) >= ((1 << (MAX_LWZ_BITS)) * 2))
		 return -2;	/* stop a code dump */
	      *sp++ = table[1][code];
	      if (code == table[0][code])
		 return (code);
	      code = table[0][code];
	}
	*sp++ = firstcode = table[1][code];
	if ((code = max_code) < (1 << MAX_LWZ_BITS)) {
	   table[0][code] = oldcode;
	   table[1][code] = firstcode;
	   ++max_code;
	   if ((max_code >= max_code_size) && (max_code_size < (1 << MAX_LWZ_BITS))) {
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


/*----------------------------------------------------------------------
  highbit returns position of highest set bit in 'ul' as an integer (0-31),
  or -1 if none.     
  ----------------------------------------------------------------------*/
static int          highbit (unsigned long ul)
{
  int                 i;

  for (i = 31; ((ul & 0x80000000) == 0) && i >= 0; i--, ul <<= 1) ;
  return i;
}

/*----------------------------------------------------------------------
  highbit16 returns position of highest set bit in 'ul' as an integer (0-31),
  or -1 if none.          
  ----------------------------------------------------------------------*/
int                 highbit16 (unsigned long ul)
{
  int                 i;

  for (i = 15; ((ul & 0x8000) == 0) && i >= 0; i--, ul <<= 1) ;
  return i;
}

/*----------------------------------------------------------------------
  nbbits returns the width of a bit PicMask.
  ----------------------------------------------------------------------*/
static int          nbbits (unsigned long ul)
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

#ifndef _WINDOWS
/*----------------------------------------------------------------------
  Make a shape  of depth 1 for display from image data.
  ----------------------------------------------------------------------*/
Pixmap MakeMask (Display* dsp, char* pixelindex, int w, int h, int bg)
{
  Pixmap              PicMask;
  unsigned char      *iptr;
  char                value;
  char               *data;
  int                 bpl, y;
  unsigned char      *data_ptr, *max_data;
  int                 diff, count, width, height;
  XImage             *newmask;
  ThotGC              tmp_gc;

  width  = w;
  height = h;

  newmask = XCreateImage (TtDisplay, theVisual, 1, ZPixmap, 0, 0, width, height, 8, 0);
  bpl = newmask->bytes_per_line;
  newmask->data = (char *) TtaGetMemory (bpl * height);
  data = newmask->data;
  iptr = pixelindex; 
  diff = width & 7;
  width >>= 3;
  if (newmask->bitmap_bit_order == MSBFirst)
    for (y = 0; y < height; y++)
      {
	data_ptr = data;
	max_data = data_ptr + width;
	while (data_ptr < max_data)
	  {
	    value = 0;
	    value = (value << 1) | (*(iptr++) != bg);
	    value = (value << 1) | (*(iptr++) != bg);
	    value = (value << 1) | (*(iptr++) != bg);
	    value = (value << 1) | (*(iptr++) != bg);
	    value = (value << 1) | (*(iptr++) != bg);
	    value = (value << 1) | (*(iptr++) != bg);
	    value = (value << 1) | (*(iptr++) != bg);
	    value = (value << 1) | (*(iptr++) != bg);
	    *(data_ptr++) = value;
	  }
	if (diff)
	  {
	    value = 0;
	    for (count = 0; count < diff; count++)
	      if (*(iptr++) != bg)
		value |= (0x80 >> count);
	    *(data_ptr++) = value;
	  }
	data += bpl;
      }
  else
    {
      for (y = 0; y < height; y++)
	{
	  data_ptr = data;
	  max_data = data_ptr + width;
	  while (data_ptr < max_data)
	    {
	      value = 0;
	      iptr += 8;
	      value = (value << 1) | (*(--iptr) != bg);
	      value = (value << 1) | (*(--iptr) != bg);
	      value = (value << 1) | (*(--iptr) != bg);
	      value = (value << 1) | (*(--iptr) != bg);
	      value = (value << 1) | (*(--iptr) != bg);
	      value = (value << 1) | (*(--iptr) != bg);
	      value = (value << 1) | (*(--iptr) != bg);
	      value = (value << 1) | (*(--iptr) != bg);
	      iptr += 8;
	      *(data_ptr++) = value;
	    }
	  if (diff)
	    {
	      value = 0;
	      for (count = 0; count < diff; count++)
		if (*(iptr++) != bg)
		  value |= (1 << count);	      
	      *(data_ptr++) = value;
	    }
	  data += bpl;
	}
    }

#ifndef _GTK
  PicMask = XCreatePixmap (TtDisplay, TtRootWindow, w, h, 1);
  if ((PicMask == (Pixmap) None) || (newmask == NULL))
    {
      if (newmask != NULL)
	XDestroyImage (newmask);
      if (PicMask != (Pixmap) None)
	XFreePixmap (TtDisplay, PicMask);
      PicMask = None;
    }
  else
    {
      tmp_gc = XCreateGC (TtDisplay, PicMask, 0, NULL);
      XPutImage (TtDisplay, PicMask, tmp_gc, newmask, 0, 0, 0, 0, w, h);
      XDestroyImage (newmask);
      XFreeGC (TtDisplay, tmp_gc);
    }
#endif /* _GTK */
  return (PicMask);
}


/*----------------------------------------------------------------------
  Make an image of appropriate depth for display from image data.
  ----------------------------------------------------------------------*/
XImage *MakeImage (Display* dsp, unsigned char *data, int width, int height,
				   int depth, ThotColorStruct *colrs)
{
  int                 linepad, shiftnum;
  int                 shiftstart, shiftstop, shiftinc;
  int                 bytesperline;
  int                 temp;
  int                 w, h;
  XImage             *newimage = NULL;
  unsigned char      *bit_data, *bitp, *datap;
  int                 bmap_order;
  unsigned long       c;
  int                 rshift, gshift, bshift;
  int                 useMSB;

  switch (depth)
    {
    case 6:
    case 8:
      bit_data = (unsigned char *) TtaGetMemory (width * height);
      bcopy (data, bit_data, (width * height));
      bytesperline = width;
      newimage = XCreateImage (dsp,
			       theVisual,
			       depth, ZPixmap, 0, (char *) bit_data,
			       width, height, 8, bytesperline);
      break;
    case 1:
    case 2:
    case 4:
      if (BitmapBitOrder (dsp) == LSBFirst)
	{
	  shiftstart = 0;
	  shiftstop = 8;
	  shiftinc = depth;
	}
      else
	{
	  shiftstart = 8 - depth;
	  shiftstop = -depth;
	  shiftinc = -depth;
	}
      linepad = 8 - (width % 8);
      bit_data = (unsigned char *) TtaGetMemory (((width + linepad) * height) + 1);
      bitp = bit_data;
      datap = data;
      *bitp = 0;
      shiftnum = shiftstart;
      for (h = 0; h < height; h++)
	{
	  for (w = 0; w < width; w++)
	    {
	      temp = *datap++ << shiftnum;
	      *bitp = *bitp | temp;
	      shiftnum = shiftnum + shiftinc;
	      if (shiftnum == shiftstop)
		{
		  shiftnum = shiftstart;
		  bitp++;
		  *bitp = 0;
		}
	    }
	  for (w = 0; w < linepad; w++)
	    {
	      shiftnum = shiftnum + shiftinc;
	      if (shiftnum == shiftstop)
		{
		  shiftnum = shiftstart;
		  bitp++;
		  *bitp = 0;
		}
	    }
	}
      bytesperline = (width + linepad) * depth / 8;
      newimage = XCreateImage (dsp,
			       theVisual,
			       depth, ZPixmap, 0, (char *) bit_data,
			       (width + linepad), height, 8, bytesperline);
      break;
      
    case 15:
    case 16:
      bit_data = (unsigned char *) TtaGetMemory (width * height * 2);
      bitp = bit_data;
      datap = data;
      rshift = 0;
      gshift = nbbits (theVisual->red_mask);
      bshift = gshift + nbbits (theVisual->green_mask);
      for (w = (width * height); w > 0; w--)
	{
	  temp = ((colrs[(int) *datap].red & theVisual->red_mask) | 
		  ((colrs[(int) *datap].green >> gshift) & theVisual->green_mask) |
		  (((colrs[(int) *datap].blue >> bshift) & theVisual->blue_mask)));
	  
	  if (BitmapBitOrder (dsp) == MSBFirst)
	    {
	      *bitp++ = (temp >> 8) & 0xff;
	      *bitp++ = temp & 0xff;
	    }
	  else
	    {
	      *bitp++ = temp & 0xff;
	      *bitp++ = (temp >> 8) & 0xff;
	    }
	  datap++;
	}
      
      newimage = XCreateImage (dsp,
			       theVisual,
			       depth, ZPixmap, 0, (char *) bit_data,
			       width, height, 16, 0);
      break;

    case 24:
    case 32:
      bit_data = (unsigned char *) TtaGetMemory (width * height * 4);
      newimage = XCreateImage (dsp,
			       theVisual,
			       depth, ZPixmap, 0, (char *) bit_data,
			       width, height, 8, 0);

      rshift = highbit (theVisual->red_mask) - 7;
      gshift = highbit (theVisual->green_mask) - 7;
      bshift = highbit (theVisual->blue_mask) - 7;
      bmap_order = BitmapBitOrder (dsp);

      bitp = bit_data;
      datap = data;
      useMSB = (newimage->bits_per_pixel > 24);
      for (w = (width * height); w > 0; w--)
	{
	  c =
	    (((colrs[(int) *datap].red >> 8) & 0xff) << rshift) |
	    (((colrs[(int) *datap].green >> 8) & 0xff) << gshift) |
	    (((colrs[(int) *datap].blue >> 8) & 0xff) << bshift);
	  
	  datap++;
	  
	  if (bmap_order == MSBFirst)
	    {
	      if (useMSB)
		*bitp++ = (unsigned char) ((c >> 24) & 0xff);
	      *bitp++ = (unsigned char) ((c >> 16) & 0xff);
	      *bitp++ = (unsigned char) ((c >> 8) & 0xff);
	      *bitp++ = (unsigned char) (c & 0xff);
	    }
	  else
	    {
	      *bitp++ = (unsigned char) (c & 0xff);
	      *bitp++ = (unsigned char) ((c >> 8) & 0xff);
	      *bitp++ = (unsigned char) ((c >> 16) & 0xff);
	      if (useMSB)
		*bitp++ = (unsigned char) ((c >> 24) & 0xff);
	    }
	}
      break;
    default:
      fprintf (stderr, "gifhandler: Don't know how to format image for display of depth %d\n", depth);
      return (None);
    }
   return (newimage);
}
#else /* _WINDOWS */
/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
HBITMAP WIN_MakeImage (HDC hDC, unsigned char *data, int width, int height,
		       int depth, ThotColorStruct * colrs)
{
  HBITMAP             newimage;
  unsigned char      *bit_data, *bitp, *datap;
  int                 temp;
  int                 w, h;
  int                 shiftstart, shiftstop, shiftinc;
  int                 linepad, shiftnum;
  int                 bytesperline;
  int                 rshift, gshift, bshift;

  switch (depth)
    {
    case 1:
    case 2:
    case 4: 
      shiftstart = 0;
      shiftstop = 8;
      shiftinc = depth;
      linepad = 8 - (width % 8);
      bit_data = (unsigned char *) TtaGetMemory (((width + linepad) * height) + 1);
      bitp = bit_data;
      datap = data;
      *bitp = 0;
      shiftnum = shiftstart;
      for (h = 0; h < height; h++)
	{
	  for (w = 0; w < width; w++)
	    {
	      temp = *datap++ << shiftnum;
	      *bitp = *bitp | temp;
	      shiftnum = shiftnum + shiftinc;
	      if (shiftnum == shiftstop)
		{
		  shiftnum = shiftstart;
		  bitp++;
		  *bitp = 0;
		}
	    }
	  for (w = 0; w < linepad; w++)
	    {
	      shiftnum = shiftnum + shiftinc;
	      if (shiftnum == shiftstop)
		{
		  shiftnum = shiftstart;
		  bitp++;
		  *bitp = 0;
		}
	    }
	}
      bytesperline = (width + linepad) * depth / 8;
      break;

    case 16:
      bit_data = (unsigned char *) TtaGetMemory (width * height * 2);
      bitp   = bit_data;
      datap  = data;
      rshift = 0;
      gshift = 5;
      bshift = 11;
      for (w = (width * height); w > 0; w--)
	{
	  if (IS_WIN95)
	    temp = (((colrs[(int) *datap].red * 255) & 63488) |
		    (((colrs[(int) *datap].green * 255) >> gshift) & 2016) |
		    ((((colrs[(int) *datap].blue * 255) >> bshift) & 31)));
	  else
	    temp = (((colrs[(int) *datap].red << 8) & 63488) |
		    (((colrs[(int) *datap].green << 8) >> gshift) & 2016) |
		    ((((colrs[(int) *datap].blue << 8) >> bshift) & 31)));
	  *bitp++ = temp & 0xff;
	  *bitp++ = (temp >> 8) & 0xff;	  
	  datap++;
	}
      break;
    case 24:
      bit_data = (unsigned char *) TtaGetMemory (width * height * 4);
      bitp   = bit_data;
      datap  = data;
      for (h = height; h > 0; h--)
	{
	  for (w = width; w > 0; w--)
	    {
	      *bitp++ = colrs[(int) *datap].blue;
	      *bitp++ = colrs[(int) *datap].green;
	      *bitp++ = colrs[(int) *datap].red;
	      datap++;
	    }
	  if (width % 2 != 0) 
	    *bitp++=0;
	}
      break;
      
    case 32:
      bit_data = (unsigned char *) TtaGetMemory (width * height * 4);
      bitp   = bit_data;
      datap  = data;
      for (h = height; h > 0; h--)
	{
	  for (w = width; w > 0; w--)
	    {
	      *bitp++ = colrs[(int) *datap].blue;
	      *bitp++ = colrs[(int) *datap].green;
	      *bitp++ = colrs[(int) *datap].red;
	      *bitp++ = 0;
	      datap++;
	    }
	}
      break;
    }

  newimage = CreateCompatibleBitmap (hDC, width, height);
  SetBitmapBits (newimage, width * height * (depth/8), bit_data);
  TtaFreeMemory (bit_data);
  return newimage;
}
#endif /* _WINDOWS */

/*----------------------------------------------------------------------
  Allocate and return the thotColors table.
  ----------------------------------------------------------------------*/
Pixmap DataToPixmap (unsigned char *image_data, int width, int height,
		     int ncolors, ThotColorStruct colrs[256], int **thotColors)
{
#ifndef _WINDOWS
  Pixmap              Img;
  XImage             *tmpimage;
  ThotColorStruct     tmpcolr;
  unsigned char      *tmpdata;
  unsigned char      *ptr;
  unsigned char      *ptr2;
  int                 i, size;
  int                 delta, not_right_col, not_last_row;
  int                 cx, cy;
  int                *cmap = NULL, *tcolors;

  /* find the visual class. */
  if (ncolors > 0)
    {
      tcolors = *thotColors;
      cmap = (int*) TtaGetMemory (ncolors * sizeof (int));
      for (i = 0; i < ncolors; i++)
	{
	  tmpcolr.red   = colrs[i].red;
	  tmpcolr.green = colrs[i].green;
	  tmpcolr.blue  = colrs[i].blue;
	  tmpcolr.pixel = 0;
#ifndef _GTK
	  tmpcolr.flags = DoRed | DoGreen | DoBlue;
#endif /* !_GTK */
	  if (TtIsTrueColor)
	    cmap[i] = i;
	  else if (TtWDepth == 1)
	    {
	      cmap[i] = ((tmpcolr.red >> 5) * 11 +
			 (tmpcolr.green >> 5) * 16 +
			 (tmpcolr.blue >> 5) * 5) / (65504 / 64);
	    }
	  else
	    {
	      if (tcolors == NULL)
		{
		  tcolors = (int*) TtaGetMemory (ncolors * sizeof (int));
		  memset (tcolors, 0, ncolors * sizeof (int));
		}
	      tcolors[i] = TtaGetThotColor (tmpcolr.red /256,
					    tmpcolr.green / 256,
					    tmpcolr.blue / 256);
	      tmpcolr.pixel = ColorPixel (tcolors[i]);
	      cmap[i] = tmpcolr.pixel;
	    }
	}
      *thotColors = tcolors;

      /*
       * Special case:  For 2 color non-black&white images, instead
       * of 2 dither patterns, we will always drop them to be
       * black on white.
       */
      if (TtWDepth == 1 && ncolors == 2)
	{
	  if (cmap[0] < cmap[1])
	    {
	      cmap[0] = 0;
	      cmap[1] = 64;
	    }
	  else
	    {
	      cmap[0] = 64;
	      cmap[1] = 0;
	    }
	}
    }

  size = width * height;
  if (size == 0)
    tmpdata = NULL;
  else
    tmpdata = (unsigned char *) TtaGetMemory (size);
  if (tmpdata == NULL)
    {
      /* no image to display */
      tmpimage = None;
      Img = (Pixmap) None;
    }
  else
    {
      ptr = image_data;
      ptr2 = tmpdata;
      /* use the color table */
      while (ptr2 < tmpdata + size - 1)
	{
	  if (*ptr > ncolors)
	    *ptr2 = (unsigned char) cmap[ncolors];
	  else
	    *ptr2 = cmap[(int) *ptr];
	  ptr2++;
	  ptr++;
	}
      if (TtWDepth == 1)
	{
	  ptr2 = tmpdata;
	  for (cy = 0; cy < height; cy++)
	    {
	      for (cx = 0; cx < width; cx++)
		{
		  /* Assume high numbers are really negative. */
		  if (*ptr2 > 128)
		    *ptr2 = 0;
		  else if (*ptr2 > 64)
		    *ptr2 = 64;
		  
		  /* Traditional Floyd-Steinberg */
		  if (*ptr2 < 32)
		    {
		      delta = *ptr2;
		      *ptr2 = Black_Color;
		    }
		  else
		    {
		      delta = *ptr2 - 64;
		      *ptr2 = White_Color;
		    }
		  if ((not_right_col = (cx < (width - 1))))
		    *(ptr2 + 1) += delta * 7 >> 4;
		  
		  if ((not_last_row = (cy < (height - 1))))
		    (*(ptr2 + width)) += delta * 5 >> 4;
		  
		  if (not_right_col && not_last_row)
		    (*(ptr2 + width + 1)) += delta >> 4;
		  
		  if (cx && not_last_row)
		    (*(ptr2 + width - 1)) += delta * 3 >> 4;
		  ptr2++;
		}
	    }
	}
      tmpimage = MakeImage (TtDisplay, tmpdata, width, height, TtWDepth, colrs);
      TtaFreeMemory (tmpdata);
      Img = XCreatePixmap (TtDisplay, TtRootWindow, width, height, TtWDepth);
    }

  if ((tmpimage == None) || (Img == (Pixmap) None))
    {
#ifndef _GTK 
      if (tmpimage != None)
	XDestroyImage (tmpimage);
      if (Img != (Pixmap) None)
	XFreePixmap (TtDisplay, Img);
#endif /* _GTK */
      Img = None;
    }
  else
    {
#ifndef _GTK 
      XPutImage (TtDisplay, Img, GCimage, tmpimage, 0, 0, 0, 0, width, height);
      XDestroyImage (tmpimage);
#endif /* _GTK */
    }
  TtaFreeMemory ( cmap);
  return (Img);

#else /* _WINDOWS */
  static int        cbBits, cbPlanes; 
  BYTE               mapIndex;
  int               padding, i, j, ret = 0;
  int               cmap [MAXNUMBER];
  BYTE*             bmBits;
  HDC               destMemDC;  
  HBITMAP           bmp = 0;
  unsigned int      colorIndex;

  if (TtIsTrueColor)
    return WIN_MakeImage (TtDisplay, image_data, width, height, TtWDepth, colrs);
  else
    {
      destMemDC = CreateCompatibleDC (TtDisplay);
      WIN_InitSystemColors (TtDisplay);
      if (width % 2)
	padding = 1;
      else 
	padding = 0;

      bmBits = (BYTE*) TtaGetMemory ((width + padding) * height * sizeof (BYTE));
      if (bmBits == NULL)
	{
	  DeleteDC (destMemDC);
	  return NULL;
	}
    
      for (i = 0; i < MAXNUMBER; i++)
	cmap [i] = -1;
      bmp = CreateCompatibleBitmap (TtDisplay, width, height);
      if ((bmp == NULL))
	{
	  TtaFreeMemory (bmBits);
	  DeleteDC (destMemDC);
	  return (Pixmap) bmp;
	}

      SelectObject (destMemDC, bmp);
      for (j = 0; j < height; j++)
	{
	  for (i = 0; i < width; i++)
	    {
	      colorIndex = (unsigned int) image_data [i + j * width];
	      if (cmap [colorIndex] != -1)
		mapIndex = (BYTE) cmap[colorIndex];
	      else
		{
		  mapIndex = WIN_GetColorIndex (colrs [colorIndex].red, colrs [colorIndex].green, colrs [colorIndex].blue);
		  cmap[colorIndex] = (int) mapIndex ;  
		}    
	      bmBits[i + j * (padding + width)] = mapIndex;
	    }
	}
      ret = SetBitmapBits (bmp, width * height, bmBits);
      
      /* Cleanup */
      DeleteDC(destMemDC);
      TtaFreeMemory (bmBits);
      peInitialized = 0;	 
      return (Pixmap) bmp;
    }
#  endif /* _WINDOWS */
}


/*----------------------------------------------------------------------
   ReadGifToData decomresses the file and returns the picture data 
  ----------------------------------------------------------------------*/
unsigned char *ReadGifToData (char *datafile, int *w, int *h, int *ncolors,
			      int *cpp, ThotColorStruct colrs[256])
{
   unsigned char      *bit_data;
   FILE               *fp;

#  ifndef _WINDOWS
   fp = ufopen (datafile, "r");
#  else  /* _WINDOWS */
   fp = ufopen (datafile, "rb");
#  endif /* _WINDOWS */

   if (fp != NULL)
     {
	bit_data = ReadGIF (fp, w, h, ncolors, cpp, colrs);
	if (bit_data != NULL)
	  {
	     if (fp != stdin)
		fclose (fp);
	     return (bit_data);
	  }
	if (fp != stdin)
	   fclose (fp);
     }
   return ((unsigned char *) NULL);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
Drawable GifCreate (char *fn, PictInfo *imageDesc, int *xif, int *yif,
		    int *wif, int *hif, unsigned long BackGroundPixel,
		    ThotBitmap * mask1, int *width, int *height, int zoom)
{
  Pixmap              pixmap = (Pixmap) 0;
  ThotColorStruct     colrs[256];
  unsigned char*      buffer = (unsigned char*)0;
  unsigned char*      buffer2 = (unsigned char*)0;
  int                 w, h;
  int                 i;
  int                 ncolors, cpp;

  Gif89.transparent = -1;
  Gif89.delayTime = -1;
  Gif89.inputFlag = -1;
  Gif89.disposal = 0;

#ifdef _WINDOWS
  bgRed   = -1;
  bgGreen = -1;
  bgBlue  = -1;
#endif /* _WINDOWS */

  buffer = ReadGifToData (fn, &w, &h, &ncolors, &cpp, colrs);
  /* return image dimensions */
  *width = w;
  *height = h;
  if (buffer == NULL)
    {
#ifdef _WINDOWS
      WinErrorBox (NULL, "GifCreate(1)");
#endif /* _WINDOWS */
      return ((Drawable) NULL);
    }

  if (zoom != 0 && *xif == 0 && *yif == 0)
    {
      /* take zoom into account */
      *xif = PixelValue (w, UnPixel, NULL, zoom);
      *yif = PixelValue (h, UnPixel, NULL, zoom);
    }
  else
    {
      if (*xif == 0 && *yif != 0)
	*xif = PixelValue (w, UnPixel, NULL, zoom);
      if (*xif != 0 && *yif == 0)
	*yif = PixelValue (h, UnPixel, NULL, zoom);
    }

#ifndef _WIN_PRINT
  if ((*xif != 0 && *yif != 0) && (w != *xif || h != *yif))
    {
      /* xif and yif contain width and height of the box */	  
      if ((*xif * *yif) > 4000000 )
	{
	  i = 4000000 / (*xif * *yif);
	  *xif = i * *xif;
	  *yif = i * *yif;
	}
      buffer2 = ZoomPicture (buffer, w , h, *xif, *yif, 1);
      TtaFreeMemory (buffer);
      buffer = buffer2;
      buffer2 = NULL;
      w = *xif;
      h = *yif;
    }
#endif /* _WIN_PRINT */
  
  if (buffer == NULL)
    return ((Drawable) NULL);	

  if (Gif89.transparent != -1)
    {
      if (Gif89.transparent < 0)
	i = 256 + Gif89.transparent;
      else
	i = Gif89.transparent;
#ifndef _WINDOWS
      *mask1 = MakeMask (TtDisplay, buffer, w, h, i);
#else  /* _WINDOWS */
      bgRed   = colrs[i].red;
      bgGreen = colrs[i].green;
      bgBlue  = colrs[i].blue;
#endif /* _WINDOWS */
    }
  
  pixmap = DataToPixmap (buffer, w, h, ncolors, colrs,
			 &(imageDesc->PicColors));
  if (imageDesc->PicColors != NULL)
    imageDesc->PicNbColors = ncolors;
  TtaFreeMemory (buffer);
  if (pixmap == None)
    {
#ifdef _WINDOWS
      WinErrorBox (NULL, "GifCreate(2)");
#endif /* _WINDOWS */
      return ((Drawable) NULL);
    }
  else
    {
      *wif = w;
      *hif = h;
      *xif = 0;
      *yif = 0;
      return (Drawable) pixmap;
    }
}


/*----------------------------------------------------------------------
   GifPrint  : reads a gif file and produces PostScirpt      
  ----------------------------------------------------------------------*/
void GifPrint (char *fn, PictureScaling pres, int xif, int yif, int wif,
	       int hif, int PicXArea, int PicYArea, int PicWArea,
	       int PicHArea, FILE *fd, unsigned long BackGroundPixel)
{
   int                 delta;
   int                 xtmp, ytmp;
   unsigned char      *pt;
   int                 x, y, w, h;
   int                 wim;
   unsigned int        NbCharPerLine;
   ThotColorStruct     colrs[256];
   unsigned char      *buffer;
   int                 ncolors, cpp, i;

   Gif89.transparent = -1;
   Gif89.delayTime = -1;
   Gif89.inputFlag = -1;
   Gif89.disposal = 0;

   buffer = ReadGifToData (fn, &w, &h, &ncolors, &cpp, colrs);
   if (Gif89.transparent != -1)
     {
	if (Gif89.transparent < 0)
	   i = 256 + Gif89.transparent;
	else
	   i = Gif89.transparent;
#       ifndef _WINDOWS
	colrs[i].red   = 65535;
	colrs[i].green = 65535;
	colrs[i].blue  = 65535;
#       else _WINDOWS
	colrs[i].red   = 255;
	colrs[i].green = 255;
	colrs[i].blue  = 255;
#       endif /* _WINDOWS */
     }

   if (!buffer)
      return;
   PicWArea = w;
   PicHArea = h;
   xtmp = 0;
   ytmp = 0;
   switch (pres)
     {
     case FillFrame:
     case XRepeat:
     case YRepeat:
     case RealSize:
       delta = (wif - PicWArea) / 2;
       if (delta > 0)
	 {
	   xif += delta;
	   wif = PicWArea;
	 }
       else
	 {
	   xtmp = -delta;
	   PicWArea = wif;
	 }
       delta = (hif - PicHArea) / 2;
       if (delta > 0)
	 {
	   yif += delta;
	   hif = PicHArea;
	 }
       else
	 {
	   
	   ytmp = -delta;
	   PicHArea = hif;
	 }
       fprintf (fd, "gsave %d -%d translate\n", PixelToPoint (xif),
		   PixelToPoint (yif + hif));
       fprintf (fd, "%d %d %d %d DumpImage2\n", PicWArea, PicHArea,
		   PixelToPoint (wif), PixelToPoint (hif));
       break;
     case ReScale:
       fprintf (fd, "gsave %d -%d translate\n", PixelToPoint (xif),
		   PixelToPoint (yif + hif));
       fprintf (fd, "%d %d %d %d DumpImage2\n", PicWArea, PicHArea,
		   PixelToPoint (wif), PixelToPoint (hif));
       wif = PicWArea;
       hif = PicHArea;
       break;
     default:
       break;
     }

   wim = w;
   fprintf (fd, "\n");
   NbCharPerLine = wim;

   for (y = 0; y < hif; y++)
     {
	pt = (unsigned char *) (buffer + ((ytmp + y) * NbCharPerLine) + xtmp);

	for (x = 0; x < wif; x++)
	  {
#       ifdef _WINDOWS
		fprintf (fd, "%02x%02x%02x",
		      (colrs[*pt].red),
		      (colrs[*pt].green),
		      (colrs[*pt].blue));
#       else  /* !_WINDOWS */
		fprintf (fd, "%02x%02x%02x",
		      (colrs[*pt].red) >> 8,
		      (colrs[*pt].green) >> 8,
		      (colrs[*pt].blue) >> 8);
#       endif /* !_WINDOWS */

	     pt++;
	  }
	fprintf (fd, "\n");
     }

   fprintf (fd, "\n");
   fprintf (fd, "grestore\n");
   fprintf (fd, "\n");
   TtaFreeMemory (buffer);

}

/*----------------------------------------------------------------------
   IsGifFormat  checks the header file if it's of a gif file       
  ----------------------------------------------------------------------*/
ThotBool            IsGifFormat (CHAR_T* datafile)
{
   unsigned char       buf[16];
   FILE               *fp;
   char                version[4];


   verbose = FALSE;
   showComment = FALSE;


   fp = ufopen (datafile, "r");

   if (fp == NULL)
     {
	if (fp != stdin)
	   fclose (fp);
	return FALSE;
     }

   if (!ReadOK (fp, buf, 6))
     {
	if (fp != stdin)
	   fclose (fp);
	return FALSE;
     }

   if (strncmp ((char *) buf, "GIF", 3) != 0)
     {
	if (fp != stdin)
	   fclose (fp);
	return (FALSE);
     }

   strncpy (version, (char *) buf + 3, 3);
   version[3] = EOS;

   if ((strcmp (version, "87a") != 0) && (strcmp (version, "89a") != 0))
     {
	if (fp != stdin)
	   fclose (fp);
	return (FALSE);
     }
   else
     {
	if (fp != stdin)
	   fclose (fp);
	return (TRUE);
     }
}
