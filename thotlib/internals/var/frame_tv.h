/*
 * Global variables for frame management
 */


#ifndef THOT_FRAME_VAR
#define THOT_FRAME_VAR

#ifndef NODISPLAY
/* Frames table */
EXPORT Frame_Ctl        FrameTable[MAX_FRAME+1];
EXPORT ThotWindow  FrRef[MAX_FRAME + 1 + 1]; /* ref des fenetres de travail */
EXPORT ThotColor BackgroundColor[MAX_FRAME + 1 + 1]; /* Background colors   */

EXPORT int  DOT_PER_INCHE;         /* Precision en points par pouce */
EXPORT ThotColor Black_Color;      /* Couleur d'ecriture          */
EXPORT ThotColor White_Color;      /* Couleur de fond             */
EXPORT ThotColor Scroll_Color;     /* Couleur des bandeaux        */
EXPORT ThotColor Button_Color;     /* Couleur des boutons         */
EXPORT ThotColor Select_Color;     /* Couleur de la selection     */
EXPORT ThotColor BgMenu_Color;     /* Couleur des menus           */
EXPORT ThotColor Box_Color;        /* Couleur des boites actives  */
EXPORT ThotColor RO_Color;         /* Couleur du Read Only        */
EXPORT ThotColor InactiveB_Color;  /* Couleur des boutons inactifs*/

#ifdef NEW_WILLOWS
EXPORT WNDCLASSEX	RootShell;
EXPORT HFONT      DefaultFont;/* default Font                */
EXPORT HPALETTE   TtCmap;	/* Table des couleurs pour Thot		*/
EXPORT WIN_GC_BLK TtWhiteGC;
EXPORT WIN_GC_BLK TtBlackGC;
EXPORT WIN_GC_BLK TtInvertGC;
EXPORT WIN_GC_BLK TtGreyGC;
EXPORT WIN_GC_BLK TtLineGC;
EXPORT WIN_GC_BLK TtDialogueGC;
EXPORT WIN_GC_BLK TtGraphicGC;	/* Graphic context pour les images      */

#else /* NEW_WILLOWS */
EXPORT ThotWidget	RootShell;
EXPORT XmFontList DefaultFont;  /* default Font                */
EXPORT Time	  t1;
EXPORT Display   *TtDisplay;	/* Identification du serveur X 	       	*/
EXPORT int	  TtScreen; 	/* Identification de l'ecran   		*/
EXPORT Colormap   TtCmap;	/* Table des couleurs pour Thot		*/
EXPORT ThotGC     TtWhiteGC;
EXPORT ThotGC     TtBlackGC;
EXPORT ThotGC     TtInvertGC;
EXPORT ThotGC     TtGreyGC;
EXPORT ThotGC     TtLineGC;
EXPORT ThotGC     TtDialogueGC;
EXPORT ThotGC     TtGraphicGC;	/* Graphic context pour les images      */
#endif /* !NEW_WILLOWS */

EXPORT ThotWindow TtRootWindow; /* Identification de la fenetre racine 	*/
EXPORT int        TtWDepth;	/* Nombre de plans de l'ecran  		*/
EXPORT char       TtFontName[MAX_FONT * MAX_FONTNAME];/* Name X-Window 	*/
EXPORT char       TtPsFontName[MAX_FONT * 8];	/* Name PostScript 	*/
EXPORT ptrfont    TtFonts[MAX_FONT];/* fontes chargees dans le serveur 	*/
EXPORT int 	  TtFontFrames[MAX_FONT];
EXPORT ThotCursor WindowCurs;	/* Curseur de designation de fenetres	*/
EXPORT ThotCursor WaitCurs;	/* Curseur pour attendre	*/
EXPORT ThotCursor VCurs;	/* Curseur modification presentation 	*/
EXPORT ThotCursor HCurs;	/* Curseur modification presentation 	*/
EXPORT ThotCursor HVCurs;	/* Curseur modification presentation 	*/

EXPORT int     DefaultBColor;		/* default background color index   */
EXPORT int     ActiveFrame;		/* Numero de fenetre document active */
EXPORT int     ClickFrame;			/* ViewFrame designee par la selection */
EXPORT int     ClickX;			/* Position X de la selection  */
EXPORT int     ClickY;			/* Position X de la selection  */
EXPORT int     ClickIsDone;		/* Indicateur de selection     */
EXPORT unsigned char	*Xbuffer;	/* Buffer pour echanges avec X */
EXPORT int		ClipboardLength;
EXPORT Proc		CurrentCallbackAPI;

#ifdef NEW_WILLOWS
#include <windows.h>
#include <commctrl.h>

extern HINSTANCE hInstance;
extern char   *tszAppName;
extern HWND    hwndAmaya;
extern int     nAmayaShow;
extern HFONT   WIN_formFONT;
extern HMENU       WinMenus[MAX_FRAME + 1 + 1];
extern HFONT       WIN_DefaultFont;
extern ThotWindow  WinToolBar[MAX_FRAME + 1 + 1];
extern ThotWindow  WinStatusBar[MAX_FRAME + 1 + 1];
extern HWND        WIN_Main_Wd;
extern TBBUTTON    WIN_buttons[];

extern int  WIN_DECAL_X, WIN_DECAL_Y; /* Decalages des fenetres document      */
extern HFONT	WIN_DefaultFont;
extern HBITMAP	WIN_LastBitmap;
extern HFONT    WIN_LastFont;

extern struct 	WIN_DisplayDesc WIN_OpenDisplays[MAX_DISPLAY];
extern int 	WIN_ActifFen;		/* Numero de fenetre document active */
extern int      WIN_DesFen;		/* ViewFrame designee par la selection */
extern int      WIN_DesX;		/* Position X de la selection  */
extern int      WIN_DesY;		/* Position Y de la selection  */
extern int      WIN_DesReturn;		/* Indicateur de selection     */
extern unsigned char	*WIN_buffer;	/* Buffer pour echanges avec Window */
extern int		WIN_Lgbuffer;

#endif /* NEW_WILLOWS */

#endif /* !NODISPLAY */

#endif
