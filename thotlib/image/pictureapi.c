/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2005
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
  #include "message_wx.h"
#endif /* _WX */

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
   pixmap = new wxBitmap( bits, (int)wxBITMAP_TYPE_XPM, width, height );
   return pixmap;
#endif /* _WX */
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
}

