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
 
/*
 * Global variables for frame management
 */

#ifndef THOT_FRAME_VAR
#define THOT_FRAME_VAR

#ifndef NODISPLAY

/* Frames table */
THOT_EXPORT Frame_Ctl   FrameTable[MAX_FRAME+1];
#ifdef _GTK
THOT_EXPORT GtkWidget  *DefaultWindow;
THOT_EXPORT GtkWidget  *DefaultDrawingarea;
THOT_EXPORT GdkDrawable *DefaultDrawable;
THOT_EXPORT GdkDrawable *FrRef[MAX_FRAME + 2];    /* window references */
#else /* _GTK */
THOT_EXPORT ThotWindow  FrRef[MAX_FRAME + 2];     /* window references */
#endif /* _GTK */

#ifdef _WINDOWS
THOT_EXPORT ThotWindow  FrMainRef[MAX_FRAME + 2]; /* window references */
THOT_EXPORT CHAR_T      wTitle [256];
#endif /* _WINDOWS */
THOT_EXPORT ThotColor BackgroundColor[MAX_FRAME + 2]; /* Background color */
THOT_EXPORT int       DefaultBColor;    /* default background color index */
THOT_EXPORT int       DefaultFColor;    /* default foreground color index */
THOT_EXPORT ThotColor Black_Color;      /* writing color                  */
THOT_EXPORT ThotColor White_Color;      /* background color               */
THOT_EXPORT ThotColor Scroll_Color;     /* scroll color                   */
THOT_EXPORT ThotColor Select_Color;     /* selection color                */
THOT_EXPORT ThotColor BgMenu_Color;     /* menu background color          */
THOT_EXPORT ThotColor FgMenu_Color;     /* menu foreground color          */
THOT_EXPORT ThotColor Box_Color;        /* active box color               */
THOT_EXPORT ThotColor RO_Color;         /* Read Only color                */
THOT_EXPORT ThotColor InactiveB_Color;  /* Disabled button color          */
THOT_EXPORT ThotBool  TtIsTrueColor;

#ifdef _WINDOWS
THOT_EXPORT HACCEL       hAccel[MAX_FRAME + 2];
THOT_EXPORT WNDCLASSEX   RootShell;
THOT_EXPORT HFONT        DefaultFont;  /* default Font                 */
THOT_EXPORT HDC          TtDisplay;
THOT_EXPORT HDC          TtPrinterDC;
THOT_EXPORT HPALETTE     TtCmap;       /* Color palette                */
THOT_EXPORT LPLOGPALETTE ptrLogPal;
THOT_EXPORT HRGN         clipRgn;
THOT_EXPORT WIN_GC_BLK   TtLineGC;
THOT_EXPORT ThotBool     TtIsPrinterTrueColor;
#else /* _WINDOWS */
#ifdef _GTK
THOT_EXPORT ThotWidget   RootShell;
THOT_EXPORT XmFontList   DefaultFont;  /* default Font                  */
THOT_EXPORT Time         t1;
THOT_EXPORT Display     *TtDisplay;    /* Server context                */
THOT_EXPORT int          TtScreen;     /* Screen                        */
THOT_EXPORT GdkColormap *TtCmap;       /* Color palette                 */
THOT_EXPORT GdkGC       *TtWhiteGC;
THOT_EXPORT GdkGC       *TtBlackGC;
THOT_EXPORT GdkGC       *TtInvertGC;
THOT_EXPORT GdkGC       *TtGreyGC;
THOT_EXPORT GdkGC       *TtLineGC;
THOT_EXPORT GdkGC       *TtDialogueGC;
THOT_EXPORT GdkGC       *TtGraphicGC;   /* Graphic context for images    */
#else /* _GTK */
THOT_EXPORT ThotWidget   RootShell;
THOT_EXPORT XmFontList   DefaultFont;   /* default Font                  */
/* The fonts symbol include wrong values for the character 244 */
THOT_EXPORT int          TtPatchedFont[MAX_FONT];/* size of the patched font */
THOT_EXPORT Time         t1;
THOT_EXPORT Display     *TtDisplay;	/* Server context                */
THOT_EXPORT int	         TtScreen; 	/* Screen                        */
THOT_EXPORT Colormap     TtCmap;	/* Color palette                 */
THOT_EXPORT ThotGC       TtWhiteGC;
THOT_EXPORT ThotGC       TtBlackGC;
THOT_EXPORT ThotGC       TtInvertGC;
THOT_EXPORT ThotGC       TtGreyGC;
THOT_EXPORT ThotGC       TtLineGC;
THOT_EXPORT ThotGC       TtDialogueGC;
THOT_EXPORT ThotGC       TtGraphicGC;	/* Graphic context for images    */
#endif /* _GTK */

#endif /* _WINDOWS */
THOT_EXPORT ThotWindow TtRootWindow;    /* Thot root window              */
THOT_EXPORT int        TtWDepth;	/* Screen color depth            */
THOT_EXPORT int        TtWPrinterDepth;	/* Printer color depth           */
THOT_EXPORT char       TtFontName[MAX_FONT * MAX_FONTNAME];/* font names */
THOT_EXPORT char       TtPsFontName[MAX_FONT * 8];/* PS font names       */
THOT_EXPORT int        TtFontFrames[MAX_FONT];
THOT_EXPORT ptrfont    TtFonts[MAX_FONT];/* loaded fonts                 */
THOT_EXPORT ThotCursor WindowCurs;	/* The window Cursor             */
THOT_EXPORT ThotCursor WaitCurs;	/* Waiting cursor                */
THOT_EXPORT ThotCursor VCurs;	/* vertical resizing cursor              */
THOT_EXPORT ThotCursor HCurs;	/* horizontal resizing cursor            */
THOT_EXPORT ThotCursor HVCurs;	/* vertical+horizontal resizing cursor   */

THOT_EXPORT int            ActiveFrame;	      /* Active frame            */
THOT_EXPORT int            ClickFrame;	      /* Clicked frame           */
THOT_EXPORT int            ClickX;	      /* Clicked X position      */
THOT_EXPORT int            ClickY;	      /* Clicked Y position      */
THOT_EXPORT int            ClickIsDone;	      /* Click done              */
THOT_EXPORT USTRING        Xbuffer;	      /* Clipboard buffer        */
THOT_EXPORT int		   ClipboardLength;
THOT_EXPORT Proc	   CurrentCallbackAPI;

#ifdef _WINDOWS
#include <windows.h>
#ifndef __GNUC__
#include <commctrl.h>
#endif
THOT_EXPORT HWND       WIN_Main_Wd;
THOT_EXPORT ThotWindow WinToolBar[MAX_FRAME + 1 + 1];
THOT_EXPORT HMENU      WinMenus[MAX_FRAME + 1 + 1];

extern HBITMAP	  WIN_LastBitmap;
extern HINSTANCE  hInstance;

#endif /* _WINDOWS */
#endif /* !NODISPLAY */
#endif
