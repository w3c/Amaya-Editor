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
 * Authors: N. Layaida (INRIA)
 *          R. Guetari (W3C/INRIA) Windows 95/NT routines
 */
 
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "picture.h"
#include "frame.h"
#include "libmsg.h"
#include "message.h"
#include "application.h"
#include "xpmP.h"
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
#include "win_f.h"
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

extern int bgRed ;
extern int bgGreen;
extern int bgBlue ;

/* static PALETTEENTRY sysPalEntries[MAX_COLOR]; */
static PALETTEENTRY sysPalEntries[256];
boolean             peInitialized = FALSE;
static int          nbSysColors ;
/* static int          mapIndex; */
static int          best_dsquare = INT_MAX;

/* ----------------------------------------------------------------------
  WIN_InitSysColors						
  ---------------------------------------------------------------------- */
#ifdef __STDC__
int WIN_InitSystemColors (void)
#else /* __STDC__ */ 
int WIN_InitSystemColors () 
#endif /* __STDC __ */
{
    int i;
	HDC hDC;

    if (peInitialized)
       return 1;

	hDC = GetDC (NULL);
    
    if (!(GetDeviceCaps (TtDisplay, RASTERCAPS) & RC_PALETTE)) {
       ReleaseDC (NULL, hDC);
       return 1;
    }

    nbSysColors = GetSystemPaletteEntries (hDC, 0, GetDeviceCaps (hDC, SIZEPALETTE), sysPalEntries);
	ReleaseDC (NULL, hDC);


    /*********************************************************************************************************
    ptrLogPal = (PLOGPALETTE) LocalAlloc (LMEM_FIXED, sizeof(LOGPALETTE) + MAX_COLOR * sizeof(PALETTEENTRY));
    ptrLogPal->palVersion    = 0x300;
    ptrLogPal->palNumEntries = MAX_COLOR;
       
    for (i = 0; i < MAX_COLOR; i++) {
        ptrLogPal->palPalEntry[i].peRed   = (BYTE) RGB_Table[i].red;
        ptrLogPal->palPalEntry[i].peGreen = (BYTE) RGB_Table[i].green;
        ptrLogPal->palPalEntry[i].peBlue  = (BYTE) RGB_Table[i].blue;
        ptrLogPal->palPalEntry[i].peFlags = (BYTE) PC_RESERVED;
    }

    TtCmap = CreatePalette (ptrLogPal);
    LocalFree (ptrLogPal);
      
    if (TtCmap == NULL) {
       WinErrorBox (WIN_Main_Wd);
    }
    **********************************************************************************************************/
    peInitialized = TRUE;
    return 0;
}

/* ---------------------------------------------------------------------- 
   WIN_GetColorIndex
   ---------------------------------------------------------------------- */
#ifdef __STDC__
BYTE WIN_GetColorIndex (int r, int g, int b)
#else /* __STDC__ */
BYTE WIN_GetColorIndex (r, g, b)
int r; 
int g; 
int b;
#endif /* __STDC__ */
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
#ifdef __STDC__
static unsigned char* ReadGifImage (FILE* fd, int len, int height, unsigned char clmap[3][MAXCOLORMAPSIZE], int interlace, int ignore)
#else  /* __STDC__ */
static unsigned char* ReadGifImage (fd, len, height, clmap, interlace, ignore)
FILE               *fd;
int                 len;
int                 height;
unsigned char       clmap[3][MAXCOLORMAPSIZE];
int                 interlace;
int                 ignore;

#endif /* __STDC__ */
{
   unsigned char       c;
   int                 v;
   int                 xpos = 0, ypos = 0, pass = 0;
   unsigned char      *data;
   unsigned char      *dptr;

   /*
    **  Initialize the Compression routines
    */
   if (!ReadOK (fd, &c, 1))
      return (NULL);

   if (LWZReadByte (fd, TRUE, c) < 0)
      return (NULL);

   /*
    **  If this is an "uninteresting picture" ignore it.
    */
   if (ignore) {
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
      fprintf (stderr, "too much input data, ignoring extra...\n");
   return (data);
}

/*----------------------------------------------------------------------
  ReadGIF
  ----------------------------------------------------------------------*/
#ifdef __STDC__
unsigned char* ReadGIF (FILE* fd, int* w, int* h, int* ncolors, int* cpp, ThotColorStruct colrs[256])
#else  /* __STDC__ */
unsigned char* ReadGIF (fd, w, h, ncolors, cpp, colrs)
FILE           *fd;
int            *w;
int            *h;
int            *ncolors;
int            *cpp;
ThotColorStruct colrs[256];

#endif /* __STDC__ */
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
          colrs[i].flags = DoRed | DoGreen | DoBlue;
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
          colrs[i].flags = DoRed | DoGreen | DoBlue;
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
              colrs[i].flags = DoRed | DoGreen | DoBlue;
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
              colrs[i].flags = DoRed | DoGreen | DoBlue;
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
#ifdef __STDC__
int                 ReadColorMap (FILE* fd, int number, unsigned char buffer[3][MAXCOLORMAPSIZE])
#else  /* __STDC__ */
int                 ReadColorMap (fd, number, buffer)
FILE               *fd;
int                 number;
unsigned char       buffer[3][MAXCOLORMAPSIZE];
#endif /* __STDC__ */
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
#ifdef __STDC__
int                 DoExtension (FILE* fd, int label)
#else  /* __STDC__ */
int                 DoExtension (fd, label)
FILE               *fd;
int                 label;

#endif /* __STDC__ */
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
#ifdef __STDC__
int                 GetDataBlock (FILE* fd, unsigned char *buf)
#else  /* __STDC__ */
int                 GetDataBlock (fd, buf)
FILE               *fd;
unsigned char      *buf;
#endif /* __STDC__ */
{
   unsigned char       count;

   if (!ReadOK (fd, &count, 1)) {
      fprintf (stderr, "error in getting DataBlock size\n");
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
#ifdef __STDC__
int                 GetCode (FILE* fd, int code_size, int flag)
#else  /* __STDC__ */
int                 GetCode (fd, code_size, flag)
FILE               *fd;
int                 code_size;
int                 flag;
#endif /* __STDC__ */
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
#ifdef __STDC__
int                 LWZReadByte (FILE * fd, int flag, int input_code_size)
#else  /* __STDC__ */
int                 LWZReadByte (fd, flag, input_code_size)
FILE               *fd;
int                 flag;
int                 input_code_size;

#endif /* __STDC__ */
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
   returns position of highest set bit in 'ul' as an integer (0-31),      	
   or -1 if none.                                                       
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          highbit (unsigned long ul)
#else  /* __STDC__ */
static int          highbit (ul)
unsigned long       ul;

#endif /* __STDC__ */
{
   /*
    * returns position of highest set bit in 'ul' as an integer (0-31),
    * or -1 if none.
    */

   int                 i;

   for (i = 31; ((ul & 0x80000000) == 0) && i >= 0; i--, ul <<= 1) ;
   return i;
}

/*----------------------------------------------------------------------
   returns position of highest set bit in 'ul' as an integer (0-31),      
   or -1 if none.                                                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 highbit16 (unsigned long ul)
#else  /* __STDC__ */
int                 highbit16 (ul)
unsigned long       ul;

#endif /* __STDC__ */
{
   int                 i;

   for (i = 15; ((ul & 0x8000) == 0) && i >= 0; i--, ul <<= 1) ;
   return i;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          nbbits (unsigned long ul)
#else  /* __STDC__ */
static int          nbbits (ul)
unsigned long       ul;

#endif /* __STDC__ */
{
   /*
    * returns the width of a bit PicMask.
    */
   while (!(ul & 1))
         ul >>= 1;
   switch (ul) {
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
	       fprintf (stderr, "nbbits : invalid PicMask\n");
	       return (8);
   }
}

#  ifndef _WINDOWS
/*
 * Make a shape  of depth 1 for display from image data.
 */
#ifdef __STDC__
Pixmap              MakeMask (Display* dsp, char* pixelindex, int w, int h, int bg)
#else  /* __STDC__ */
Pixmap              MakeMask (dsp, pixelindex, w, h, bg)
Display            *dsp;
unsigned char      *pixelindex;
int                 w, h;
int                 bg;

#endif /* __STDC__ */
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
      for (y = 0; y < height; y++) {
	  data_ptr = data;
	  max_data = data_ptr + width;
	  while (data_ptr < max_data) {
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
	  if (diff) {
	     value = 0;
	     for (count = 0; count < diff; count++)
		 if (*(iptr++) != bg)
		    value |= (0x80 >> count);
	     *(data_ptr++) = value;
	  }
	  data += bpl;
      }
   else {
	for (y = 0; y < height; y++) {
	    data_ptr = data;
	    max_data = data_ptr + width;
	    while (data_ptr < max_data) {
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
	    if (diff) {
	       value = 0;
	       for (count = 0; count < diff; count++)
		   if (*(iptr++) != bg)
		      value |= (1 << count);

	       *(data_ptr++) = value;
	    }
	    data += bpl;
	}
   }

   PicMask = XCreatePixmap (TtDisplay, TtRootWindow, w, h, 1);

   if ((PicMask == (Pixmap) None) || (newmask == NULL)) {
      if (newmask != NULL)
	 XDestroyImage (newmask);

      if (PicMask != (Pixmap) None)
	 XFreePixmap (TtDisplay, PicMask);

	PicMask = None;
   } else {
	  tmp_gc = XCreateGC (TtDisplay, PicMask, 0, NULL);
	  XPutImage (TtDisplay, PicMask, tmp_gc, newmask, 0, 0, 0, 0, w, h);
	  XDestroyImage (newmask);
	  XFreeGC (TtDisplay, tmp_gc);
   }

   return (PicMask);
}
#  endif /* _WINDOWS */

#ifdef _WINDOWS
#ifdef __STDC__
HBITMAP WIN_MakeImage (HDC hDC, unsigned char *data, int width, int height, int depth, ThotColorStruct * colrs)
#else  /* __STDC__ */
HBITMAP WIN_MakeImage (hDC, data, width, height, depth, colrs)
HDC              dsp;
unsigned char*   data;
int              width, height;
int              depth;
ThotColorStruct* colrs;

#endif /* __STDC__ */
{
   int                 temp;
   int                 w, h;
   HBITMAP             newimage;
   unsigned char      *bit_data, *bitp, *datap;
   int                 rshift, gshift, bshift;
   switch (depth) {
          case 16:
               bit_data = (unsigned char *) TtaGetMemory (width * height * 2);
               bitp   = bit_data;
               datap  = data;
               rshift = 0;
               gshift = 5;
               bshift = 11;
               for (w = (width * height); w > 0; w--) {
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
              for (h = height; h > 0; h--) {
				  for (w = width; w > 0; w--) {
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
              for (w = (width * height); w > 0; w--) {
                   *bitp++ = colrs[(int) *datap].blue;
                   *bitp++ = colrs[(int) *datap].green;
                   *bitp++ = colrs[(int) *datap].red;
                   *bitp++ = 0;
       
                   datap++;
			  }
			  break;
   }
   
   newimage = CreateCompatibleBitmap (hDC, width, height);
   SetBitmapBits (newimage, width * height * (depth/8), bit_data);
   TtaFreeMemory (bit_data);
   return newimage;
}
#endif /* _WINDOWS */


#  ifndef _WINDOWS
/*----------------------------------------------------------------------
  Make an image of appropriate depth for display from image data.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
XImage             *MakeImage (Display* dsp, unsigned char *data, int width, int height, int depth,
			       ThotColorStruct * colrs)
#else  /* __STDC__ */
XImage             *MakeImage (dsp, data, width, height, depth, colrs)
Display            *dsp;
unsigned char      *data;
int                 width, height;
int                 depth;
ThotColorStruct    *colrs;

#endif /* __STDC__ */
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
	       bit_data = (unsigned char *) TtaGetMemory (((width + linepad) * height)
							  + 1);
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


	       rshift = highbit (theVisual->red_mask) - 7;
	       gshift = highbit (theVisual->green_mask) - 7;
	       bshift = highbit (theVisual->blue_mask) - 7;
	       bmap_order = BitmapBitOrder (dsp);

	       bitp = bit_data;
	       datap = data;
	       for (w = (width * height); w > 0; w--)
		 {
		    c =
		       (((colrs[(int) *datap].red >> 8) & 0xff) << rshift) |
		       (((colrs[(int) *datap].green >> 8) & 0xff) << gshift) |
		       (((colrs[(int) *datap].blue >> 8) & 0xff) << bshift);

		    datap++;

		    if (bmap_order == MSBFirst)
		      {
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
			 *bitp++ = (unsigned char) ((c >> 24) & 0xff);
		      }
		 }

	       newimage = XCreateImage (dsp,
					theVisual,
					depth, ZPixmap, 0, (char *) bit_data,
					width, height, 32, 0);
	       break;
	    default:
	       fprintf (stderr, "Don't know how to format image for display of depth %d\n", depth);
	       return (None);
	 }

   return (newimage);
}
#  endif /* _WINDOWS */

#ifdef _WINDOWS
static signed int tabCorres [256];
#endif /* _WINDOWS */

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
Pixmap DataToPixmap (unsigned char *image_data, int width, int height, int num_colors, ThotColorStruct colrs[256])
#else  /* __STDC__ */
Pixmap DataToPixmap (image_data, width, height, num_colors, colrs)
unsigned char*      image_data;
int                 width;
int                 height;
int                 num_colors;
ThotColorStruct     colrs[256];

#endif /* __STDC__ */
{

#  ifndef _WINDOWS

   int                 i, size;
   int                 delta, not_right_col, not_last_row;
   Pixmap              Img;
   XImage             *tmpimage;
   ThotColorStruct     tmpcolr;
   int                *Mapping;
   unsigned char      *tmpdata;
   unsigned char      *ptr;
   unsigned char      *ptr2;
   boolean             need_to_dither;

   /* find the visual class. */
   if (THOT_vInfo.depth == 1)
      need_to_dither = TRUE;
   else
      need_to_dither = FALSE;

   Mapping = (int*) TtaGetMemory (num_colors * sizeof (int));

   for (i = 0; i < num_colors; i++) {
	tmpcolr.red   = colrs[i].red;
	tmpcolr.green = colrs[i].green;
	tmpcolr.blue  = colrs[i].blue;
	tmpcolr.pixel = 0;
	tmpcolr.flags = DoRed | DoGreen | DoBlue;

	if ((THOT_vInfo.class == THOT_TrueColor) || (THOT_vInfo.class == THOT_DirectColor))
	   Mapping[i] = i;
	else if (need_to_dither == TRUE) {
	     Mapping[i] = ((tmpcolr.red >> 5) * 11 +
			   (tmpcolr.green >> 5) * 16 +
			   (tmpcolr.blue >> 5) * 5) / (65504 / 64);
	} else {
	       FindOutColor (TtDisplay, TtCmap, &tmpcolr);
	       Mapping[i] = tmpcolr.pixel;
	}
   }

   /*
    * Special case:  For 2 color non-black&white images, instead
    * of 2 dither patterns, we will always drop them to be
    * black on white.
    */
   if ((need_to_dither == TRUE) && (num_colors == 2)) {
      if (Mapping[0] < Mapping[1]) {
	 Mapping[0] = 0;
	 Mapping[1] = 64;
      } else {
	     Mapping[0] = 64;
	     Mapping[1] = 0;
      }
   }

   size = width * height;
   if (size == 0)
      tmpdata = NULL;
   else
       tmpdata = (unsigned char *) TtaGetMemory (size);

   if (tmpdata == NULL) {
      tmpimage = None;
      Img = (Pixmap) None;
   } else {
	  ptr = image_data;
	  ptr2 = tmpdata;

	  if (need_to_dither == TRUE) {
	     int                 cx, cy;

	     for (ptr2 = tmpdata, ptr = image_data; ptr2 < tmpdata + (size - 1); ptr2++, ptr++)
		 *ptr2 = Mapping[(int) *ptr];

	     ptr2 = tmpdata;
	     for (cy = 0; cy < height; cy++) {
		 for (cx = 0; cx < width; cx++) {
		     /* Assume high numbers are really negative. */
		     if (*ptr2 > 128)
			*ptr2 = 0;
		     else if (*ptr2 > 64)
			  *ptr2 = 64;

		     /* Traditional Floyd-Steinberg */
		     if (*ptr2 < 32) {
			delta = *ptr2;
			*ptr2 = Black_Color;
		     } else {
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
	  } else {
	         for (i = 0; i < size; i++) {
		   if (*ptr < 0)
		     *ptr2++ = (unsigned char) Mapping[0];
		   else if (*ptr > num_colors)
		     *ptr2++ = (unsigned char) Mapping[num_colors];
		   else
		     *ptr2++ = (unsigned char) Mapping[(int) *ptr];
		   ptr++;
		 }
	  }
	  tmpimage = MakeImage (TtDisplay, tmpdata, width, height, TtWDepth, colrs);
	  TtaFreeMemory (tmpdata);
	  Img = XCreatePixmap (TtDisplay, TtRootWindow, width, height, TtWDepth);
   }

   if ((tmpimage == None) || (Img == (Pixmap) None))
     {
       if (tmpimage != None)
	 XDestroyImage (tmpimage);
       if (Img != (Pixmap) None)
	 XFreePixmap (TtDisplay, Img);
       Img = None;
     }
   else
     {
       XPutImage (TtDisplay, Img, GCimage, tmpimage, 0, 0, 0, 0, width, height);
       XDestroyImage (tmpimage);
     }
   TtaFreeMemory ( Mapping);

   return (Img);

#  else /* _WINDOWS */
   boolean           need_to_dither;
   if (THOT_vInfo.depth == 1)
      need_to_dither = TRUE;
   else
       need_to_dither = FALSE;

   if (TtIsTrueColor) 
      return WIN_MakeImage (TtDisplay, image_data, width, height, TtWDepth, colrs);
   else {
         static int        cbBits, cbPlanes; 
         BYTE               mapIndex ;
         int               padding, i, j, ret = 0, nbPalColors;
         int               Mapping [MAXNUMBER];
         BYTE*             bmBits;
         HDC               destMemDC;  
         HBITMAP           bmp = 0;
         unsigned int      colorIndex;

         destMemDC = CreateCompatibleDC (TtDisplay);
         WIN_InitSystemColors ();

         if (width % 2)
			padding = 1;
		 else 
			 padding = 0;

         bmBits = (BYTE*) TtaGetMemory ((width + padding) * height * sizeof (BYTE));
         if (bmBits == NULL) {
            DeleteDC (destMemDC);
            return NULL;
         }
    
         for (i = 0; i < MAXNUMBER; i++)
             Mapping [i] = -1;

         bmp = CreateCompatibleBitmap (TtDisplay, width, height);

         if ((bmp == NULL)) {
            TtaFreeMemory (bmBits);
            DeleteDC (destMemDC);
            return (Pixmap) bmp;
         }
    /********************************
		 SelectPalette (destMemDC, TtCmap, FALSE);
		 nbPalColors = RealizePalette (destMemDC);
         nbSysColors = GetSystemPaletteEntries (TtDisplay, 0, GetDeviceCaps (TtDisplay, SIZEPALETTE), sysPalEntries);
		 *********************************/

         SelectObject (destMemDC, bmp);

         for (j = 0; j < height; j++) {
             for (i = 0; i < width; i++) {
                 colorIndex = (unsigned int) image_data [i + j * width];
                 if (Mapping [colorIndex] != -1)
                    mapIndex = (BYTE) Mapping [colorIndex];
                 else {
                      mapIndex = WIN_GetColorIndex (colrs [colorIndex].red, colrs [colorIndex].green, colrs [colorIndex].blue);
                      Mapping [colorIndex] = (int) mapIndex ;  
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
#ifdef __STDC__
unsigned char      *ReadGifToData (char *datafile, int *w, int *h, int *ncolors, int *cpp, ThotColorStruct colrs[256])

#else  /* __STDC__ */
unsigned char      *ReadGifToData (datafile, w, h, ncolors, cpp, colrs)
char               *datafile;
int                *w;
int                *h;
int                *ncolors;
int                *cpp;
ThotColorStruct     colrs[256];

#endif /* __STDC__ */
{
   unsigned char      *bit_data;
   FILE               *fp;

#  ifndef _WINDOWS
   fp = fopen (datafile, "r");
#  else  /* _WINDOWS */
   fp = fopen (datafile, "rb");
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
#ifdef __STDC__
ThotBitmap          GifCreate (char *fn, PictureScaling pres, int *xif, int *yif, int *wif, int *hif, unsigned long BackGroundPixel, ThotBitmap * mask1, int *width, int *height)
#else  /* __STDC__ */
ThotBitmap          GifCreate (fn, pres, xif, yif, wif, hif, BackGroundPixel, mask1, width, height)
char               *fn;
PictureScaling      pres;
int                *xif;
int                *yif;
int                *wif;
int                *hif;
unsigned long       BackGroundPixel;
ThotBitmap         *mask1;
int                *width;
int                *height;
#endif /* __STDC__ */
{
  int                 w, h;
  Pixmap              pixmap;
  int                 i, ratio;
  ThotColorStruct     colrs[256];
  unsigned char      *buffer, *buffer2;
  int                 ncolors, cpp;

  Gif89.transparent = -1;
  Gif89.delayTime = -1;
  Gif89.inputFlag = -1;
  Gif89.disposal = 0;

# ifdef _WINDOWS
  bgRed   = -1;
  bgGreen = -1;
  bgBlue  = -1;
# endif /* _WINDOWS */

  buffer = ReadGifToData (fn, &w, &h, &ncolors, &cpp, colrs);
  /* return image dimensions */
  *width = w;
  *height = h;
  if (buffer == NULL)
     return (ThotBitmapNone);

  if (*xif == 0 && *yif != 0)
    *xif = w;
  if (*xif != 0 && *yif == 0)
    *yif = h;
  if ((*xif != 0 && *yif != 0) && (w != *xif || h != *yif))
    {
      /* xif and yif contain width and height of the box */	  
      if ((*xif * *yif) > 4000000 )
	{
	  ratio = 4000000 / (*xif * *yif);
	  *xif = ratio * *xif;
	  *yif = ratio * *yif;
	}
      buffer2 = ZoomPicture (buffer, w , h, *xif, *yif, 1);
      TtaFreeMemory (buffer);
      buffer = buffer2;
      buffer2 = NULL;
      w = *xif;
      h = *yif;
    }
  
  if (buffer == NULL)
    return (ThotBitmapNone);	
  if (Gif89.transparent != -1)
    {
      if (Gif89.transparent < 0)
	i = 256 + Gif89.transparent;
      else
	i = Gif89.transparent;
#     ifndef _WINDOWS
      *mask1 = MakeMask (TtDisplay, buffer, w, h, i);
#     else  /* _WINDOWS */
      bgRed   = colrs[i].red;
      bgGreen = colrs[i].green;
      bgBlue  = colrs[i].blue;
#     endif /* _WINDOWS */
    }
  
   pixmap = DataToPixmap (buffer, w, h, ncolors, colrs);
   TtaFreeMemory (buffer);
   if (pixmap == None)
     return (ThotBitmapNone);
   else
     {
       *wif = w;
       *hif = h;
       *xif = 0;
       *yif = 0;
       return (ThotBitmap) pixmap;
     }
}


/*----------------------------------------------------------------------
   GifPrint  : reads a gif file and produces PostScirpt            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GifPrint (char *fn, PictureScaling pres, int xif, int yif, int wif, int hif, int PicXArea, int PicYArea, int PicWArea, int PicHArea, int filedesc, unsigned long BackGroundPixel)
#else  /* __STDC__ */
void                GifPrint (fn, pres, xif, yif, wif, hif, PicXArea, PicYArea, PicWArea, PicHArea, filedesc, BackGroundPixel)
char               *fn;
PictureScaling      pres;
int                 xif;
int                 yif;
int                 wif;
int                 hif;
int                 PicXArea;
int                 PicYArea;
int                 PicWArea;
int                 PicHArea;
int                 filedesc;
unsigned long       BackGroundPixel;

#endif /* __STDC__ */
{
   int                 delta;
   int                 xtmp, ytmp;
   unsigned char      *pt;
   int                 x, y, w, h;
   int                 wim;
   unsigned int        NbCharPerLine;
   FILE               *fd;
   ThotColorStruct     colrs[256];
   unsigned char      *buffer;
   int                 ncolors, cpp, i;

   Gif89.transparent = -1;
   Gif89.delayTime = -1;
   Gif89.inputFlag = -1;
   Gif89.disposal = 0;

   buffer = ReadGifToData (fn, &w, &h, &ncolors, &cpp, colrs);
   fd = (FILE *)filedesc;
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
       fprintf (fd, "gsave %d -%d translate\n", PixelToPoint (xif), PixelToPoint (yif + hif));
       fprintf (fd, "%d %d %d %d DumpImage2\n", PicWArea, PicHArea, PixelToPoint (wif), PixelToPoint (hif));
       break;
     case ReScale:
       fprintf (fd, "gsave %d -%d translate\n", PixelToPoint (xif), PixelToPoint (yif + hif));
       fprintf (fd, "%d %d %d %d DumpImage2\n", PicWArea, PicHArea, PixelToPoint (wif), PixelToPoint (hif));
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
#ifdef __STDC__
boolean             IsGifFormat (char *datafile)
#else  /* __STDC__ */
boolean             IsGifFormat (datafile)
char               *datafile;

#endif /* __STDC__ */
{
   unsigned char       buf[16];
   FILE               *fp;
   char                version[4];


   verbose = FALSE;
   showComment = FALSE;


   fp = fopen (datafile, "r");

   if (fp == NULL)
     {
	if (fp != stdin)
	   fclose (fp);
	return FALSE;
     }

   if (!ReadOK (fp, buf, 6))
     {
#if 0
	fprintf (stderr, "error reading magic number\n");
#endif
	if (fp != stdin)
	   fclose (fp);
	return FALSE;
     }

   if (strncmp ((char *) buf, "GIF", 3) != 0)
     {
#if 0
	if (verbose)
	   fprintf (stderr, "not a GIF file\n");
#endif
	if (fp != stdin)
	   fclose (fp);
	return (FALSE);
     }

   strncpy (version, (char *) buf + 3, 3);
   version[3] = EOS;

   if ((strcmp (version, "87a") != 0) && (strcmp (version, "89a") != 0))
     {
#if 0
	fprintf (stderr, "bad version number, not '87a' or '89a'\n");
#endif
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
