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
typedef ThotWidget 	ThotScrollBar;
typedef ThotWidget	ThotFrame;
typedef ThotWidget	ThotStatusBar;
typedef TBBUTTON      *ThotButton;
typedef ThotWidget 	ThotMenuBar;
typedef HMENU          ThotMenu;
typedef ThotWidget	ThotToolBar;
typedef HBITMAP        ThotBitmap;
typedef COLORREF       ThotColor;
typedef COLORREF       Pixel;
typedef void          *ThotPixmap;
typedef HCURSOR        ThotCursor;
#ifndef _GL
typedef HWND           Drawable;
typedef POINT          ThotPoint;
#else /* _GL */
typedef unsigned char* Drawable;
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

