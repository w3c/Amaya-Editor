/*
 * NOGUI types
 */
#ifdef _NOGUI
typedef void *         ThotWidget;
typedef void *         ThotWindow;
typedef ThotWidget     ThotScrollBar;
typedef ThotWidget     ThotFrame;
typedef ThotWidget     ThotStatusBar;
typedef ThotWidget     ThotButton;
typedef ThotWidget     ThotMenuBar;
typedef ThotWidget     ThotMenu;
typedef ThotWidget     ThotToolBar;
typedef int            ThotBitmap;
typedef int            ThotGC;
typedef unsigned long  ThotColor;
typedef int *          ThotFont;
typedef void *         ThotCursor;
#ifndef _GL
//typedef XPoint         ThotPoint;
#endif /* #ifndef _GL */
typedef void *         ThotEvent;
typedef void *         ThotKeyEvent;
typedef void *         ThotComposeStatus;
typedef void *         ThotAppContext;
typedef void *         ThotTranslations;
typedef int            ThotIcon;
typedef void *         ThotPixmap;

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

#endif /* #ifdef _NOGUI */
