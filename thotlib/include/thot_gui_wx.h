/*
 * wxWindow GUI types
 */
#ifdef _WX

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
    #include "wx/wx.h"
    #include "wx/toolbar.h"
    #include "wx/colour.h"
    #include "wx/event.h"
    #include "wx/font.h"
#endif

class AmayaFrame;

typedef wxObject *	ThotWidget;
typedef wxScrollBar * 	ThotScrollBar;
typedef AmayaFrame *	ThotFrame;
typedef wxStatusBar *	ThotStatusBar;

typedef wxWindow *	ThotWindow;
typedef wxButton * 	ThotButton;
typedef wxMenuBar *	ThotMenuBar;
typedef wxMenu *	ThotMenu;
typedef wxToolBar *	ThotToolBar;
typedef wxControl *	ThotControl;
typedef wxColour *      ThotColor;
#ifdef _GL
typedef void *          ThotFont; // these fonts are used for low level display
#else /* _GL */
typedef wxFont *        ThotFont;
#endif /* _GL */
//typedef int           * PtrFont;
typedef void          * ThotCursor;

#ifndef _GL
//  typedef XPoint       ThotPoint;
#endif /* _GL */

typedef wxEvent *      ThotEvent;
typedef void *         ThotKeyEvent;
typedef void *         ThotComposeStatus;
typedef void *         ThotAppContext;
typedef void *         ThotTranslations;

typedef int            ThotGC;
typedef wxBitmap *     ThotIcon;
typedef wxBitmap *     ThotPixmap;
typedef wxBitmap *     ThotBitmap;

  // TODO : remove this X dependancy used in AHTBridge (ask jose)
  typedef unsigned long        XtInputId;
  typedef unsigned long        XtIntervalId;
  typedef void*                XtPointer;

typedef struct {
	unsigned long pixel;
	unsigned short red, green, blue;
	char flags;  /* do_red, do_green, do_blue */
	char pad;
} ThotColorStruct;

#ifndef _GL
typedef struct {
    short x, y;
} ThotPoint; /* replace XPoint */
#endif /* #ifndef _GL */

typedef struct {
    short x1, y1, x2, y2;
} ThotSegment; /* replace XSegment */

typedef struct {
    short x, y;
    unsigned short width, height;
    short angle1, angle2;
} ThotArc; /* replace XArc */

typedef unsigned long  ThotPixel;    /* replace Pixel */
typedef unsigned long  ThotKeySym;   /* replace KeySym */
typedef unsigned long  ThotDisplay;  /* replace Display */
typedef unsigned long  ThotDrawable; /* replace Drawable */
typedef unsigned long  ThotVisual;   /* replace Visual */

#endif /* _WX */

