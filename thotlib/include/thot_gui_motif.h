/*
 * Motif GUI types
 */
#if defined(_MOTIF)
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

#ifndef NODISPLAY
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
#endif /* #ifndef NODISPLAY */

typedef Widget         ThotWidget;
typedef Window         ThotWindow;
typedef ThotWidget     ThotScrollBar;
typedef ThotWidget     ThotFrame;
typedef ThotWidget     ThotStatusBar;
typedef ThotWidget     ThotButton;
typedef ThotWidget     ThotMenuBar;
typedef ThotWidget     ThotMenu;
typedef ThotWidget     ThotToolBar;
typedef Drawable       ThotBitmap;
typedef GC             ThotGC;
typedef unsigned long  ThotColor;
typedef int           *ThotFont;
typedef Cursor         ThotCursor;
typedef XEvent         ThotEvent;
typedef XKeyEvent      ThotKeyEvent;
typedef XComposeStatus ThotComposeStatus;
typedef XtAppContext   ThotAppContext;
typedef XtTranslations ThotTranslations;
typedef Pixmap         ThotIcon;
typedef Pixmap         ThotPixmap;
typedef Visual         ThotVisual;
typedef XColor         ThotColorStruct;
typedef XPoint         ThotPoint;
typedef XSegment       ThotSegment;
typedef XArc           ThotArc;
typedef Pixel          ThotPixel;
typedef KeySym         ThotKeySym;
typedef Display        ThotDisplay;
typedef Drawable       ThotDrawable;
#endif /* #if defined(_MOTIF) */

