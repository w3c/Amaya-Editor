/*
 * wxWindow GUI types
 */
#ifdef _WX

/* don't include wx.h to optimize compilation */
/* only declare that these classes are used */
class wxObject;
class AmayaFrame;
class AmayaWindow;
class AmayaToolBar;
class AmayaScrollBar;
class AmayaStatusBar;
class wxStatusBar;
class wxWindow;
class wxButton;
class wxBitmapButton;
class wxMenuBar;
class wxMenu;
class wxToolBar;
class wxControl;
class wxColour;
class wxFont;
class wxEvent;
class wxBitmap;
class wxEvent;
class wxKeyEvent;

typedef wxWindow *	ThotWidget;
typedef AmayaScrollBar * ThotScrollBar;
typedef AmayaFrame *	ThotFrame;
typedef AmayaStatusBar *	ThotStatusBar;

typedef wxWindow *	ThotWindow;
typedef wxButton * 	ThotButton;
typedef wxBitmapButton * 	ThotBitmapButton;
typedef wxMenuBar *	ThotMenuBar;
typedef wxMenu *	ThotMenu;
typedef AmayaToolBar *	ThotToolBar;
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

typedef struct {
    float x1, y1, x2, y2;
} ThotSegment; /* replace XSegment */

typedef struct {
    float x, y;
    float width, height;
    int angle1, angle2;
} ThotArc; /* replace XArc */

typedef unsigned long  ThotPixel;    /* replace Pixel */
typedef unsigned long  ThotKeySym;   /* replace KeySym */
typedef unsigned long  ThotDisplay;  /* replace Display */
typedef void * ThotDrawable; /* replace Drawable */
typedef unsigned long  ThotVisual;   /* replace Visual */

#endif /* _WX */

