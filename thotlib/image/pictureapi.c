/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2004
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * logo and image managment                                    
 *
 * Authors: I. Vatton (INRIA)
 *          R. Guetari (W3C/INRIA) - Unicode and Windows version
 */

#ifdef _WX
  #include "wx/wx.h"
#endif /* _WX */

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"
#include "picture.h"
#include "content.h"

#include "xpm.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "frame_tv.h"
#include "platform_tv.h"

#include "memory_f.h"

#ifdef _WX
  #include "AmayaWindow.h"
  #include "appdialogue_wx.h"
#endif /* _WX */

/*----------------------------------------------------------------------
   TtaCreateBitmap
   create a bitmap from a file
   const char * filename : the picture filename
   int type   : the picture type (content.h)
   xbm_type, eps_type, xpm_type, gif_type, png_type, jpeg_type
  ----------------------------------------------------------------------*/
ThotPixmap TtaCreateBitmap( const char * filename, int type )
{
#ifdef _WX
  ThotPixmap pixmap = NULL;
  
  /* convert thot picture type to wxwindows picture type */
  wxBitmapType wx_type = wxBITMAP_TYPE_INVALID;
  switch ( type )
    {
    case xbm_type:	/* X11 BitmapFile format */
      wx_type = wxBITMAP_TYPE_XBM;
    case xpm_type:	/* Xpm XReadFileToPixmap format */
      wx_type = wxBITMAP_TYPE_XPM;
    case gif_type:	/* gif */
      wx_type = wxBITMAP_TYPE_GIF;
    case png_type:	/* Png */
      wx_type = wxBITMAP_TYPE_PNG;
    case jpeg_type:	/* Jpeg */
      wx_type = wxBITMAP_TYPE_JPEG;
    default :
      return NULL;
    }
  
  /* create the picture form file */
  pixmap = new wxBitmap( TtaConvMessageToWX( filename ), wx_type );
  return pixmap;
#endif /* _WX */

  return NULL;
}

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
ThotPixmap TtaCreateBitmapLogo (int width, int height, char *bits)
{
#ifdef _WX
   ThotPixmap pixmap;
   pixmap = new wxBitmap( bits, wxBITMAP_TYPE_XPM, width, height );
   return pixmap;
#endif /* _WX */
  
#ifdef _MOTIF
   if (bits != NULL)
      return (XCreateBitmapFromData (TtDisplay, TtRootWindow, bits, width, height));
   else
      return (0);
#endif /* _MOTIF */

#ifdef _GTK
   GdkColor           black;
   GdkColor           white;
   gdk_color_black (TtCmap, &black);
   gdk_color_white (TtCmap, &white);
   if (bits != NULL)
     return (ThotPixmap) gdk_pixmap_create_from_data (DefaultWindow->window,
						  bits,
						  width,
						  height,					   
						  1,
						  (GdkColor *)&white,
						  (GdkColor *)&black);
   else
     return 0;
#endif /* _GTK */

#ifdef _WINGUI
   return CreateBitmap (width, height, 16, 4, bits);
#endif /* _WINGUI */   

#ifdef _NOGUI
  return 0;
#endif /* #ifdef _NOGUI */   
}

/*----------------------------------------------------------------------
   TtaCreatePixmapLogo create a pixmap from an XPM file.           
  ----------------------------------------------------------------------*/
ThotIcon TtaCreatePixmapLogo(char **d)
{
#ifdef _WX
   ThotIcon pixmap = NULL;
   pixmap = new wxBitmap( d );
   return pixmap;
#endif /* _WX */
  
#ifdef _WINGUI
   return (ThotIcon) NULL;
#endif /* _WINGUI */
   
#ifdef _GTK
  ThotIcon	icon;

  icon = (ThotIcon)TtaGetMemory (sizeof (_Thot_icon));
  if (d != NULL)
    icon->pixmap = gdk_pixmap_create_from_xpm_d (DefaultWindow->window,
						 &(icon->mask),
						 &DefaultWindow->style->bg[GTK_STATE_NORMAL] ,
						 (gchar **) d); 
  return icon;
#endif /* _GTK */

#ifdef _MOTIF  
   ThotIcon              pixmap;
   ThotIcon              pmask;
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
	XpmCreatePixmapFromData (TtDisplay, TtRootWindow, d, &pixmap, &pmask, &att);
	if (att.pixels != NULL)
	  XpmFree (att.pixels);

	if (pmask)
	   XFreePixmap (TtDisplay, pmask);
     }
   return (pixmap);
#endif /* _MOTIF */

#if defined(_NOGUI)
  return 0;
#endif /* #if defined(_NOGUI) */   
}

