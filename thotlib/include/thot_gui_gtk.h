/*
 * GTK GUI types
 */
#ifdef _GTK
/* GTK needs x11 includes for font/display
 * maybe it's possible to remove theses dependancies
 * if gtk provides font server query ... and display managment
 * like with motif gui
 *  */
#define WWW_XWINDOWS
#include <X11/keysym.h>
#include <X11/IntrinsicP.h>
#include <X11/CoreP.h>
#include <X11/Shell.h>
#include <X11/StringDefs.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/keysym.h>
#include <X11/Intrinsic.h>

  #include <gtk/gtk.h>
  #include <gdk/gdk.h>
  #include <gdk/gdkkeysyms.h> /* for keycode */

#ifndef _GL
  #ifndef _GTK2
    #include <gdk_imlib.h>      /* for load image (jpg, gif, tiff...) */
  #else /* _GTK2 */
    #include <gdk/gdkpixbuf.h>  /* for load image (jpg, gif, tiff...) */
  #endif /* !_GTK2 */
#endif /* _GL */

typedef GtkWidget     *ThotWidget;
typedef GdkWindow     *ThotWindow;
typedef ThotWidget     ThotScrollBar;
typedef ThotWidget     ThotFrame;
typedef ThotWidget     ThotStatusBar;
typedef ThotWidget     ThotButton;
typedef ThotWidget     ThotMenuBar;
typedef ThotWidget     ThotMenu;
typedef ThotWidget     ThotToolBar;
typedef GdkBitmap     *ThotBitmap;
typedef GdkGC         *ThotGC;
typedef unsigned long  ThotColor;
typedef XColor         ThotColorStruct;
typedef GdkFont       *ThotFont;
typedef GdkCursor     *ThotCursor;
typedef Display        ThotDisplay; /* X11 dependancy :( */
typedef Visual         ThotVisual;  /* X11 dependancy :( */
#ifndef _GL
typedef GdkPoint       ThotPoint;
#endif /* _GL */
typedef GdkEvent       ThotEvent;
typedef unsigned long  ThotKeyEvent; /* NOT USED ! */
typedef unsigned long  ThotComposeStatus; /* NOT USED ! */
typedef unsigned long  ThotAppContext; /* NOT USED ! */
/* typedef XtTranslations ThotTranslations; */ /* NOT USED ! */

typedef unsigned long  ThotPixel;    /* replace Pixel */
typedef unsigned long  ThotKeySym;   /* replace KeySym */
typedef unsigned long  ThotDrawable; /* replace Drawable */

typedef struct {
    short x1, y1, x2, y2;
} ThotSegment; /* replace XSegment */

typedef struct {
    short x, y;
    unsigned short width, height;
    short angle1, angle2;
} ThotArc; /* replace XArc */

typedef struct 
{
    GdkBitmap          *mask;
    GdkPixmap          *pixmap;
} _Thot_icon;

typedef _Thot_icon    *ThotIcon;
typedef GdkPixmap     *ThotPixmap;
#endif  /* _GTK */


