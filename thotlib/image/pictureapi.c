/*
 * Copyright (c) 1996 INRIA, All rights reserved
 */

/*----------------------------------------------------------------------
   
   Thot Toolkit: Application Program Interface                     
   --->logo and image managment                                    
   
   I. Vatton       October 92                      
   
  ----------------------------------------------------------------------*/
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"
#ifndef WWW_XWINDOWS
#define FOR_MSW
#endif /* WWW_XWINDOWS */
#include "xpmP.h"
#include "xpm.h"

#undef EXPORT
#define EXPORT extern
#include "frame_tv.h"
#include "platform_tv.h"
static char         NoneTxt[] = "None";

#include "memory_f.h"

/*----------------------------------------------------------------------
   TtaCreateBitmapLogo

   Creates a logo pixmap from a bitmap description: width, height and bit array.

   Parameters:
   width: the width value of the bitmap.
   height: the height value of the bitmap.
   bits: the bit array.

   Return value:
   The created pixmap for the logo.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
Pixmap              TtaCreateBitmapLogo (int width, int height, char *bits)
#else  /* __STDC__ */
Pixmap              TtaCreateBitmapLogo (width, height, bits)
int                 width;
int                 height;
char               *bits;

#endif /* __STDC__ */

{
#ifndef NEW_WILLOWS
   if (bits != NULL)
      return (XCreateBitmapFromData (TtDisplay, TtRootWindow, bits, width, height));
   else
      return (0);
#endif /* NEW_WILLOWS */
}

/*----------------------------------------------------------------------
   TtaCreatePixmapLogo create a pixmap from an XPM file.           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
Pixmap              TtaCreatePixmapLogo (char **d)
#else  /* __STDC__ */
Pixmap              TtaCreatePixmapLogo (d)
char              **d;

#endif /* __STDC__ */

{
#ifndef NEW_WILLOWS
   Pixmap              pixmap;
   Pixmap              PicMask;
   XpmAttributes       att;
   XpmColorSymbol      cs;

   pixmap = 0;
   memset (&att, 0, sizeof (XpmAttributes));
   memset (&cs, 0, sizeof (XpmColorSymbol));
   if (d != NULL)
     {
	att.valuemask = 0;
	att.valuemask |= XpmReturnPixels;
	att.valuemask |= XpmRGBCloseness;
	att.valuemask |= XpmColorSymbols;
	att.red_closeness = 40000;
	att.green_closeness = 40000;
	att.blue_closeness = 40000;
	/* None  for the background color */
	att.numsymbols = 1;
	att.colorsymbols = &cs;
	cs.name = NoneTxt;
	cs.value = NULL;
	cs.pixel = (Pixel) BgMenu_Color;
#ifdef WWW_MSWINDOWS
	/* whatever the windows version is - @@@ */
#else
	XpmCreatePixmapFromData (TtDisplay, TtRootWindow, d, &pixmap, &PicMask, &att);
#endif
	if (PicMask)
	   XFreePixmap (TtDisplay, PicMask);
     }
   return (pixmap);
#endif /* NEW_WILLOWS */
}
