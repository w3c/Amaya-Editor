/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2001.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * logo and image managment                                    
 *
 * Authors: I. Vatton (INRIA)
 *          R. Guetari (W3C/INRIA) - Unicode and Windows version
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"

#include "xpm.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "frame_tv.h"
#include "platform_tv.h"

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
Pixmap TtaCreateBitmapLogo (int width, int height, char *bits)
{
#ifndef _WINDOWS
#ifndef _GTK
   if (bits != NULL)
      return (XCreateBitmapFromData (TtDisplay, TtRootWindow, bits, width, height));
   else
      return (0);
#else /* _GTK */
   /*   if (bits !=NULL)
     return gdk_pixmap_create_from_data (DefaultWindow->window,
					 bits,
					 width,
					 height,
					 1,
					 ColorPixel(TtaGetThotColor(0,0,0)),
					 ColorPixel(TtaGetThotColor(255,255,255))); 
					 else*/

     return 0;
#endif /* !_GTK */
#else  /* _WINDOWS */
   return CreateBitmap (width, height, 16, 4, bits);
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
   TtaCreatePixmapLogo create a pixmap from an XPM file.           
  ----------------------------------------------------------------------*/
Pixmap TtaCreatePixmapLogo (char **d)
{
#ifdef _WINDOWS
   return (Pixmap) NULL;
#else  /* _WINDOWS */
   Pixmap              pixmap;
#ifdef _GTK
   ThotIcon            icon;
   ThotBitmap          mask;

   if (d != NULL)
     {
       pixmap = gdk_pixmap_create_from_xpm_d (DefaultWindow->window, &mask ,
					      &DefaultWindow->style->bg[GTK_STATE_NORMAL] ,(gchar **) d); 
       icon = gtk_pixmap_new (pixmap, mask);
       gdk_pixmap_unref (pixmap);
       gdk_bitmap_unref (mask);	
     }	    
   return (pixmap);
#else /* _GTK */
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
	att.pixels = NULL;
	att.red_closeness = 40000;
	att.green_closeness = 40000;
	att.blue_closeness = 40000;
	/* None  for the background color */
	att.numsymbols = 1;
	att.colorsymbols = &cs;
	cs.name = "None";
	cs.value = NULL;
	cs.pixel = (Pixel) BgMenu_Color;
	XpmCreatePixmapFromData (TtDisplay, TtRootWindow, d, &pixmap, &PicMask, &att);
	if (att.pixels != NULL)
	  XpmFree (att.pixels);

	if (PicMask)
	   XFreePixmap (TtDisplay, PicMask);
     }
   return (pixmap);
#endif /* _GTK */
#endif /* _WINDOWS */
}
