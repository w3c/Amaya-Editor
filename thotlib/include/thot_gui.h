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
 
#ifndef THOT_GUI_H
#define THOT_GUI_H

#define MAX_LENGTH     512
#define MAX_EXT         10

#ifdef _WINDOWS
/****************************************************************
 *								*
 * MS-Windows specific definition, constants ...		*
 *								*
 ****************************************************************/
#include <windows.h>
#include <commdlg.h>
#include <commctrl.h>
#include <stdlib.h>
#include <stdio.h>

#include "typebase.h"
/* The current HDC : the current Device context in use */
extern HDC          TtDisplay;

/* Emulation of a X-Windows Graphic Context in MS-Windows */
#define THOT_GC_FOREGROUND  0x01
#define THOT_GC_BACKGROUND  0x02
#define THOT_GC_BRUSH       0x04
#define THOT_GC_FUNCTION    0x08
#define THOT_GC_FONT        0x10
#define THOT_GC_PEN         0x20

#define FR_LANG 1
#define EN_LANG 2
#define DE_LANG 3

int app_lang;

typedef struct struct_ThotGC {
        int capabilities;
        int foreground;
        int background;
        int thick;
        int style;
} WIN_GC_BLK, *WIN_GC, *ThotGC;

typedef struct _FontInfo {
  int   FiHeight;
  int   FiAscent;
  int   FiFirstChar;
  int   FiLastChar;
  int  *FiHeights;
  int  *FiWidths;
  int   FiHighlight; 
  int   FiSize;
  char  FiScript; 
  char  FiFamily; 
} FontInfo;
typedef FontInfo      *PtrFont;

typedef void          *ThotWidget;
typedef HWND           ThotWindow;
typedef TBBUTTON      *ThotButton;
typedef HMENU          ThotMenu;
typedef HBITMAP        ThotBitmap;
typedef COLORREF       ThotColor;
typedef COLORREF       Pixel;
typedef void          *Pixmap;
typedef HCURSOR        ThotCursor;
#ifndef _GL
typedef HWND           Drawable;
typedef POINT          ThotPoint;
#else /* _GL */
typedef unsigned char* Drawable;
typedef struct GL_point 
{
  double x;
  double y;
} ThotPoint;
#endif /*_GL*/

typedef MSG            ThotEvent;
typedef MSG            ThotKeyEvent;
typedef UINT           ThotComposeStatus;
typedef void          *ThotAppContext;
typedef void          *ThotTranslations;
typedef void          *KeySym;
typedef void          *XtPointer;
typedef void          *XtIntervalId;
typedef void          *XtInputId;
typedef int            ThotIcon;
#define TBBUTTONS_BASE 50000
#define ThotColorNone  ((COLORREF)~1) /* anything in high byte is bad COLORREF */
#define ThotBitmapNone ((ThotBitmap)NULL)

#define FOR_MSW			/* for XPM stuff ! */
#define PIXEL_ALREADY_TYPEDEFED /* for XPM stuff ! */

#include "simx.h"
typedef XColor    ThotColorStruct;

#endif /* _WINDOWS */

#if defined(_MOTIF) || defined(_GTK) || defined(_NOGUI)
/************************************************************************
 *									*
 * standard Unix interface : based on Motif + Intrinsics + X-Window	*
 *									*
 ************************************************************************/
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

#if defined(_MOTIF)  && !defined(NODISPLAY)
#include <Xm/MwmUtil.h>
#include <Xm/BulletinB.h>
#include <Xm/CascadeB.h>
#include <Xm/DialogS.h>
#include <Xm/DrawingA.h>
#include <Xm/DrawnB.h>
#include <Xm/Frame.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/List.h>
#include <Xm/MainW.h>
#include <Xm/MenuShell.h>
#include <Xm/PushB.h>
#include <Xm/RepType.h>
#include <Xm/RowColumn.h>
#include <Xm/Scale.h>
#include <Xm/ScrollBar.h>
#include <Xm/ScrolledW.h>
#include <Xm/SelectioB.h>
#include <Xm/Separator.h>
#include <Xm/Text.h>
#include <Xm/ToggleB.h>
#include <Xm/Xm.h>
#include <Xm/PushBG.h>
#endif /* #if defined(_MOTIF)  && !defined(NODISPLAY) */

#ifdef _GTK
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
typedef ThotWidget     ThotButton;
typedef ThotWidget     ThotMenu;
typedef GdkBitmap     *ThotBitmap;
typedef GdkGC         *ThotGC;
typedef unsigned long  ThotColor;
typedef XColor         ThotColorStruct;
typedef GdkFont       *PtrFont;
typedef GdkCursor     *ThotCursor;

#ifndef _GL
  typedef GdkPoint       ThotPoint;
#endif /* _GL */

typedef GdkEvent       ThotEvent;
typedef XKeyEvent      ThotKeyEvent;
typedef XComposeStatus ThotComposeStatus;
typedef XtAppContext   ThotAppContext;
typedef XtTranslations ThotTranslations;

typedef struct 
{
    GdkBitmap          *mask;
    GdkPixmap          *pixmap;
} _Thot_icon;

typedef _Thot_icon    *ThotIcon;
typedef GdkPixmap     *ThotPixmap;
#endif  /* _GTK */

#ifdef _MOTIF
typedef Widget         ThotWidget;
typedef Window         ThotWindow;
typedef ThotWidget     ThotButton;
typedef ThotWidget     ThotMenu;
typedef Drawable       ThotBitmap;
typedef GC             ThotGC;
typedef unsigned long  ThotColor;
typedef XColor         ThotColorStruct;
typedef int           *PtrFont;
typedef Cursor         ThotCursor;
typedef XPoint         ThotPoint;
typedef XEvent         ThotEvent;
typedef XKeyEvent      ThotKeyEvent;
typedef XComposeStatus ThotComposeStatus;
typedef XtAppContext   ThotAppContext;
typedef XtTranslations ThotTranslations;
typedef Pixmap         ThotIcon;
#endif /* _MOTIF */

#ifdef _NOGUI
typedef void *         ThotWidget;
typedef void *         ThotWindow;
typedef ThotWidget     ThotButton;
typedef ThotWidget     ThotMenu;
typedef int            ThotBitmap;
typedef int            ThotGC;
typedef unsigned long  ThotColor;
typedef XColor         ThotColorStruct;
typedef int *          PtrFont;
typedef void *         ThotCursor;
#ifndef _GL
typedef XPoint         ThotPoint;
#endif /* #ifndef _GL */
typedef void *         ThotEvent;
typedef void *         ThotKeyEvent;
typedef void *         ThotComposeStatus;
typedef void *         ThotAppContext;
typedef void *         ThotTranslations;
typedef int         ThotIcon;
#endif /* #ifdef _NOGUI */

/* button states */
#define TBSTYLE_BUTTON  0
#define TBSTYLE_CHECK   1
#define TBSTYLE_SEP     2
#define ThotColorNone ((Pixel)-1)
#define ThotBitmapNone ((ThotBitmap)-1)

#endif /* #if defined(_MOTIF) || defined(_GTK) || defined(_NOGUI) */


#endif /* THOT_GUI_H */





