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

#if defined(_WIN32) || defined(WIN32)
#ifndef _WINDOWS
#define _WINDOWS
#endif
#endif

#if defined(_WINDOWS) || defined(_CONSOLE)

#define WWW_MSWINDOWS

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
/*
 * The current HDC : the current Device context in use.
 */
extern HDC          TtDisplay;

/*
 * Emulation of a X-Windows Graphic Context in MS-Windows
 */
#define THOT_GC_FOREGROUND  0x01
#define THOT_GC_BACKGROUND  0x02
#define THOT_GC_BRUSH       0x04
#define THOT_GC_FUNCTION    0x08
#define THOT_GC_FONT        0x10
#define THOT_GC_PEN         0x20

typedef struct struct_ThotGC {
     int                 capabilities;
     int                 foreground;
     int                 background;
} WIN_GC_BLK, *WIN_GC, *ThotGC;

typedef struct _FontInfo
{
  int            FiHeight;
  int            FiAscent;
  int            FiHeights[256];
  int            FiWidths[256];
  HFONT          FiFont;
}FontInfo;
typedef FontInfo *ptrfont;

typedef HWND        ThotWidget;
typedef HWND        ThotWindow;
typedef HBITMAP     ThotBitmap;
typedef COLORREF    ThotColor;
typedef COLORREF    Pixel;
typedef char       *Pixmap;
typedef HCURSOR     ThotCursor;
typedef HBITMAP     Drawable;
typedef POINT       ThotPoint;
typedef MSG         ThotEvent;
typedef MSG         ThotKeyEvent;
typedef UINT        ThotComposeStatus;
typedef void       *ThotAppContext;
typedef void       *ThotTranslations;

#define TBBUTTONS_BASE 50000
#define ThotColorNone ((COLORREF)~1)	/* anything in high byte is bad COLORREF */
#define ThotBitmapNone ((ThotBitmap)NULL)

#define FOR_MSW			/* for XPM stuff ! */
#define PIXEL_ALREADY_TYPEDEFED /* for XPM stuff ! */

#include "simx.h"
typedef XColor    ThotColorStruct;

#else  /* defined(_WINDOWS) || defined(_CONSOLE) */

#ifdef _WINDOWS

/************************************************************************
 *									*
 * Special case : building on Unix but for a WILLOWS			*
 * (i.e. WINDOWS) environment.						*
 *									*
 ************************************************************************/
#else  /* _WINDOWS */

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

/*
 * OWN_XLOOKUPSTRING means we redefine XLookupString function
 * (in api/interface.c) and we need internal informations on
 * some X11 specific structures.
 */

#ifdef OWN_XLOOKUPSTRING
#define X_USEBFUNCS
#include <X11/Xlibint.h>
#endif /* OWN_XLOOKUPSTRING */

#define THOT_StaticGray		StaticGray
#define THOT_GrayScale		GrayScale
#define THOT_StaticColor       	StaticColor
#define THOT_PseudoColor       	PseudoColor
#define THOT_TrueColor		TrueColor
#define THOT_DirectColor	DirectColor

typedef Widget      ThotWidget;
typedef Window      ThotWindow;
typedef Drawable    ThotBitmap;
typedef GC          ThotGC;
typedef unsigned long ThotColor;
typedef XColor      ThotColorStruct;
typedef int        *ptrfont;
typedef Cursor      ThotCursor;
typedef XPoint      ThotPoint;
typedef XEvent      ThotEvent;
typedef XKeyEvent   ThotKeyEvent;
typedef XComposeStatus ThotComposeStatus;
typedef XtAppContext ThotAppContext;
typedef XtTranslations ThotTranslations;

#define ThotColorNone ((Pixel)-1)
#define ThotBitmapNone ((ThotBitmap)-1)

#endif /* ! _WINDOWS */
#endif /* !(defined(_WINDOWS) || defined(_CONSOLE)) */

/************************************************************************
 *									*
 * Common definition for all the GUIs					*
 *									*
 ************************************************************************/

typedef struct
{
#ifdef __cplusplus
  int                 v_depth;
  int                 v_class;
#else
  int                 depth;
  int                 class;
#endif
}
THOT_VInfo;

#endif /* THOT_GUI_H */
