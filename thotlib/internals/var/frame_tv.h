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
THOT_EXPORT ThotWindow  FrRef[MAX_FRAME + 2];		/* window references */
#ifdef _WINDOWS
THOT_EXPORT ThotWindow  FrMainRef[MAX_FRAME + 2];	/* window references */
#endif /* _WINDOWS */
THOT_EXPORT ThotColor   BackgroundColor[MAX_FRAME + 2];	/* Background color */

THOT_EXPORT ThotColor Black_Color;      /* Couleur d'ecriture          */
THOT_EXPORT ThotColor White_Color;      /* Couleur de fond             */
THOT_EXPORT ThotColor Scroll_Color;     /* Couleur des bandeaux        */
THOT_EXPORT ThotColor Button_Color;     /* Couleur des boutons         */
THOT_EXPORT ThotColor Select_Color;     /* Couleur de la selection     */
THOT_EXPORT ThotColor BgMenu_Color;     /* Couleur des menus           */
THOT_EXPORT ThotColor Box_Color;        /* Couleur des boites actives  */
THOT_EXPORT ThotColor RO_Color;         /* Couleur du Read Only        */
THOT_EXPORT ThotColor InactiveB_Color;  /* Couleur des boutons inactifs*/

#ifdef _WINDOWS
THOT_EXPORT HACCEL       hAccel[MAX_FRAME + 2];
THOT_EXPORT WNDCLASSEX   RootShell;
THOT_EXPORT HFONT        DefaultFont; /* default Font                */
THOT_EXPORT HDC          TtDisplay;
THOT_EXPORT HPALETTE     TtCmap;      /* Table des couleurs pour Thot  */
THOT_EXPORT LPLOGPALETTE ptrLogPal;
THOT_EXPORT HRGN         clipRgn;
THOT_EXPORT WIN_GC_BLK   TtLineGC;
THOT_EXPORT boolean      TtIsTrueColor;
#else /* _WINDOWS */
THOT_EXPORT ThotWidget   RootShell;
THOT_EXPORT XmFontList   DefaultFont;  /* default Font                */
THOT_EXPORT Time         t1;
THOT_EXPORT Display*     TtDisplay;	/* Identification du serveur X 	       	*/
THOT_EXPORT int	         TtScreen; 	/* Identification de l'ecran   		*/
THOT_EXPORT Colormap     TtCmap;	/* Table des couleurs pour Thot		*/
THOT_EXPORT ThotGC       TtWhiteGC;
THOT_EXPORT ThotGC       TtBlackGC;
THOT_EXPORT ThotGC       TtInvertGC;
THOT_EXPORT ThotGC       TtGreyGC;
THOT_EXPORT ThotGC       TtLineGC;
THOT_EXPORT ThotGC       TtDialogueGC;
THOT_EXPORT ThotGC       TtGraphicGC;	/* Graphic context pour les images      */
#endif /* _WINDOWS */

THOT_EXPORT ThotWindow TtRootWindow; /* Identification de la fenetre racine 	*/
THOT_EXPORT int        TtWDepth;	/* Nombre de plans de l'ecran  		*/
THOT_EXPORT char       TtFontName[MAX_FONT * MAX_FONTNAME];/* Name X-Window 	*/
THOT_EXPORT char       TtPsFontName[MAX_FONT * 8];	/* Name PostScript 	*/
THOT_EXPORT int        TtFontFrames[MAX_FONT];
THOT_EXPORT ptrfont    TtFonts[MAX_FONT];/* fontes chargees dans le serveur 	*/
THOT_EXPORT ThotCursor WindowCurs;	/* Curseur de designation de fenetres	*/
THOT_EXPORT ThotCursor WaitCurs;	/* Curseur pour attendre	*/
THOT_EXPORT ThotCursor VCurs;	/* Curseur modification presentation 	*/
THOT_EXPORT ThotCursor HCurs;	/* Curseur modification presentation 	*/
THOT_EXPORT ThotCursor HVCurs;	/* Curseur modification presentation 	*/

THOT_EXPORT int            DefaultBColor;     /* default background color index      */
THOT_EXPORT int            ActiveFrame;	      /* Numero de fenetre document active   */
THOT_EXPORT int            ClickFrame;	      /* ViewFrame designee par la selection */
THOT_EXPORT int            ClickX;	      /* Position X de la selection          */
THOT_EXPORT int            ClickY;	      /* Position X de la selection          */
THOT_EXPORT int            ClickIsDone;	      /* Indicateur de selection             */
THOT_EXPORT unsigned char* Xbuffer;	      /* Buffer pour echanges avec X         */
THOT_EXPORT int		   ClipboardLength;
THOT_EXPORT Proc	   CurrentCallbackAPI;

#ifdef _WINDOWS
#include <windows.h>
#ifndef __GNUC__
#include <commctrl.h>
#endif

extern HINSTANCE  hInstance;
extern char*      tszAppName;
extern HWND       hwndAmaya;
extern int        nAmayaShow;
extern HFONT      WIN_formFONT;
extern HMENU      WinMenus[MAX_FRAME + 1 + 1];
extern HFONT      WIN_DefaultFont;
extern ThotWindow WinToolBar[MAX_FRAME + 1 + 1];
extern ThotWindow WinStatusBar[MAX_FRAME + 1 + 1];
extern HWND       WIN_Main_Wd;
extern TBBUTTON   WIN_buttons[];

extern int      WIN_DECAL_X, WIN_DECAL_Y; /* Decalages des fenetres document      */
extern HFONT	WIN_DefaultFont;
extern HBITMAP	WIN_LastBitmap;
extern HFONT    WIN_LastFont;

extern struct WIN_DisplayDesc WIN_OpenDisplays[1];

extern int 	      WIN_ActifFen;		/* Numero de fenetre document active */
extern int            WIN_DesFen;		/* ViewFrame designee par la selection */
extern int            WIN_DesX;		/* Position X de la selection  */
extern int            WIN_DesY;		/* Position Y de la selection  */
extern int            WIN_DesReturn;		/* Indicateur de selection     */
extern unsigned char* WIN_buffer;	/* Buffer pour echanges avec Window */
extern int	      WIN_Lgbuffer;
#endif /* _WINDOWS */
#endif /* !NODISPLAY */
#endif
