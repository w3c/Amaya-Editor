
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*=======================================================================*/
/*|                                                                     | */
/*|                            Projet THOT                              | */
/*|                                                                     | */
/*|     Module de gestion des contextes de documents.                   | */
/*|                                                                     | */
/*|                                                                     | */
/*|                     I. Vatton       Mai     91                      | */
/*|                                                                     | */
/*|     France Logiciel numero de depot 88-39-001-00                    | */
/*|                                                                     | */
/*=======================================================================*/

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "thotconfig.h"
#include "libmsg.h"
#include "message.h"
#include "imagedrvr.h"
#include "appdialogue.h"

#undef EXPORT
#define EXPORT extern
#include "frame.var"
#include "img.var"
#include "thotcolor.var"
#include "appdialogue.var"

ThotColorStruct     cblack;
static ThotColorStruct cwhite;

#include "appli.f"
#include "curs.f"
#include "cmd.f"
#include "cmdedit.f"
#include "corrmenu.f"
#include "environ.f"
#include "font.f"
#include "imagedrvr.f"
#include "indmenu.f"
#include "initcatal.f"
#include "inites.f"
#include "memory.f"
#include "sysexec.f"

#ifdef __STDC__
extern void         ColorRGB (int, unsigned short *, unsigned short *, unsigned short *);
extern char        *TtaGetEnvString (char *);

#else
extern void         ColorRGB ();
extern char        *TtaGetEnvString ();

#endif /* __STDC__ */

#ifdef NEW_WILLOWS
/* ---------------------------------------------------------------------- */
/* |    WinCreateGC is an emulation of the XWindows XCreateGC under     | */
/* |       MS-Windows.                                                  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
ThotGC              WinCreateGC (void)
#else  /* __STDC__ */
ThotGC              WinCreateGC (void)
#endif				/* __STDC__ */
{
   ThotGC              gc = (ThotGC) TtaGetMemory (sizeof (WIN_GC_BLK));

   return (gc);
}

/* ---------------------------------------------------------------------- */
/* |    WinLoadGC has to be called before using an GC X-Windows         | */
/* |       emulation under MS-Windows.                                  | */
/* | Full description of Device Context Attributes : Petzolt p 102      | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                WinLoadGC (HDC hdc, ThotGC gc)
#else  /* __STDC__ */
void                WinLoadGC (hdc, gc)
HDC                 hdc;
ThotGC              gc;

#endif /* __STDC__ */
{
   if (gc->capabilities & THOT_GC_PEN)
     {
	SelectObject (hdc, gc->pen);
     }
    /*******************
    if (gc->capabilities & THOT_GC_BRUSH) {
        SelectObject(hdc, gc->brush);
    }
     *******************/
   if (gc->capabilities & THOT_GC_FOREGROUND)
     {
	SetTextColor (hdc, gc->foreground);
     }
   if (gc->capabilities & THOT_GC_BACKGROUND)
     {
	SetBkMode (hdc, OPAQUE);
	SetBkColor (hdc, gc->background);
     }
   else
     {
	SetBkMode (hdc, TRANSPARENT);
     }
   if (gc->capabilities & THOT_GC_FUNCTION)
     {
	SetROP2 (hdc, gc->mode);
     }
    /*******************
    if (gc->capabilities & THOT_GC_FONT) {
        SelectObject(hdc, gc->font);
    }
     *******************/
}

/* ---------------------------------------------------------------------- */
/* |    WinUnloadGC has to be called after using an GC X-Windows        | */
/* |       emulation under MS-Windows.                                  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                WinUnloadGC (HDC hdc, ThotGC gc)
#else  /* __STDC__ */
void                WinUnloadGC (gc)
HDC                 hdc;
ThotGC              gc;

#endif /* __STDC__ */
{
}

/* ---------------------------------------------------------------------- */
/* |    WinInitColors initialize the color table depending on the       | */
/* |       device capabilities under MS-Windows.                        | */
/* ---------------------------------------------------------------------- */
void                WinInitColors (void)
{
   int                 i;
   LOGPALETTE         *ptrLogPal;
   static int          initialized = 0;

   if (initialized)
      return;

   fprintf (stderr, "WinInitColors\n");

   WIN_GetDeviceContext (-1);

   /*
    * Create initialize and install a color palette for
    * the Thot set of colors.
    */
   ptrLogPal = (LOGPALETTE *) TtaGetMemory (sizeof (LOGPALETTE) +
					 MAX_COLOR * sizeof (PALETTEENTRY));
   ptrLogPal->palVersion = 0x300;
   ptrLogPal->palNumEntries = MAX_COLOR;
   ptrLogPal->palPalEntry[0].peRed = 255;
   ptrLogPal->palPalEntry[0].peGreen = 255;
   ptrLogPal->palPalEntry[0].peBlue = 255;
   ptrLogPal->palPalEntry[0].peFlags = 0;
   ptrLogPal->palPalEntry[1].peRed = 0;
   ptrLogPal->palPalEntry[1].peGreen = 0;
   ptrLogPal->palPalEntry[1].peBlue = 0;
   ptrLogPal->palPalEntry[1].peFlags = 0;

   for (i = 2; i < MAX_COLOR; i++)
     {
	ptrLogPal->palPalEntry[1].peRed = RGB_Table[i].red;
	ptrLogPal->palPalEntry[1].peGreen = RGB_Table[i].green;
	ptrLogPal->palPalEntry[1].peBlue = RGB_Table[i].blue;
	ptrLogPal->palPalEntry[1].peFlags = 0;
     }
   cmap (0) = CreatePalette (ptrLogPal);
   if (cmap (0) == NULL)
     {
	fprintf (stderr, "couldn't CreatePalette\n");
	WinErrorBox ();
     }
   else
      SelectPalette (WIN_curHdc, cmap (0), TRUE);
   TtaFreeMemory (ptrLogPal);

   /*
    * fill-in the Pix_Color table
    */
   for (i = 0;
	i < (sizeof (Pix_Color) / sizeof (Pix_Color[0]));
	i++)
      Pix_Color[i] = PALETTERGB (RGB_Table[i].red,
				 RGB_Table[i].green,
				 RGB_Table[i].blue);
    /***********
	Pix_Color[i] = GetNearestColor(WIN_curHdc,
	   RGB(RGB_Table[i].red, RGB_Table[i].green, RGB_Table[i].blue));
     ***********/

   /*
    * initialize some standard colors.
    */
   Black_Color = GetNearestColor (WIN_curHdc, PALETTERGB (0, 0, 0));
   White_Color = GetNearestColor (WIN_curHdc, PALETTERGB (255, 255, 255));
   Scroll_Color = GetNearestColor (WIN_curHdc, PALETTERGB (190, 190, 190));
   Button_Color = GetNearestColor (WIN_curHdc, PALETTERGB (190, 190, 190));
   Select_Color = GetNearestColor (WIN_curHdc, PALETTERGB (70, 130, 180));
   BgMenu_Color = GetNearestColor (WIN_curHdc, PALETTERGB (190, 190, 190));
   Box_Color = GetNearestColor (WIN_curHdc, PALETTERGB (255, 0, 0));
   RO_Color = GetNearestColor (WIN_curHdc, PALETTERGB (0, 0, 205));
   InactiveB_Color = GetNearestColor (WIN_curHdc, PALETTERGB (255, 0, 0));

   /*
    * set up the default background colors for all views.
    */
   for (i = 0;
	i < (sizeof (BackgroundColor) / sizeof (BackgroundColor[0]));
	i++)
     {
	BackgroundColor[i] = White_Color;
     }

   initialized = 1;
}

#endif /* NEW_WILLOWS */

#ifdef WWW_XWINDOWS
/* ---------------------------------------------------------------------- */
/* |    X_Erreur est le handler d'erreur X11 (non-fatale).              | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                X_Erreur (Display * dpy, XErrorEvent * err)
#else  /* __STDC__ */
void                X_Erreur (dpy, err)
Display            *dpy;
XErrorEvent        *err;

#endif /* __STDC__ */
{
   char                msg[200];

   XGetErrorText (dpy, err->error_code, msg, 200);
}

/* ---------------------------------------------------------------------- */
/* |    X_ErreurFatale est le handler d'erreur X11.                     | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                X_ErreurFatale (Display * dpy)
#else  /* __STDC__ */
void                X_ErreurFatale (dpy)
Display            *dpy;

#endif /* __STDC__ */
{
   extern int          errno;

   perror ("*** Fatal Error");
   if (errno != EPIPE)
      TtaDisplayMessage (FATAL, TtaGetMessage(LIB, LIB_X11_ERR), DisplayString (dpy));
   else
      TtaDisplayMessage (FATAL, TtaGetMessage(LIB, LIB_X11_ERR), DisplayString (dpy));
}
#endif /* WWW_XWINDOWS */

/* ---------------------------------------------------------------------- */
/* | TtaGetThotColor returns the Thot Color.                            | */
/* |            red, green, blue express the color RGB in 8 bits values | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
int                 TtaGetThotColor (unsigned short red, unsigned short green, unsigned short blue)
#else  /* __STDC__ */
int                 TtaGetThotColor (red, green, blue)
unsigned short      red;
unsigned short      green;
unsigned short      blue;

#endif /* __STDC__ */
{
   short               delred, delgreen, delblue;
   int                 i;
   unsigned int        dsquare;
   unsigned int        best_dsquare = (unsigned int) -1;
   int                 best;
   int                 maxcolor;

   /*
    * lookup for the color number among the color set allocated
    * by the application.
    * The lookup is based on a closest in cube algorithm hence
    * we try to get the closest color available for the display.
    */
   maxcolor = NumberOfColors ();
   best = 0;			/* best color in list not found */
   for (i = 0; i < maxcolor; i++)
     {
	ColorRGB (i, &delred, &delgreen, &delblue);
	/* delred <<= 8; delgreen <<= 8; delblue <<= 8; */
	delred -= red;
	delgreen -= green;
	delblue -= blue;
	dsquare = delred * delred + delgreen * delgreen + delblue * delblue;
	if (dsquare < best_dsquare)
	  {
	     best = i;
	     best_dsquare = dsquare;
	  }
     }
   return (best);
}


/* ---------------------------------------------------------------------- */
/* | TtaGiveRGB returns the RGB of the color.                           | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtaGiveRGB (char *colname, unsigned short *red, unsigned short *green, unsigned short *blue)
#else  /* __STDC__ */
void                TtaGiveRGB (colname, red, green, blue)
char               *colname;
unsigned short     *red;
unsigned short     *green;
unsigned short     *blue;

#endif /* __STDC__ */
{
   int                 i, maxcolor;
   ThotColorStruct     color;

   /*
    * Lookup the color name in the application color name database
    */
   maxcolor = NumberOfColors ();
   for (i = 0; i < maxcolor; i++)
      if (!strcasecmp (ColorName (i), colname))
	 ColorRGB (i, red, green, blue);

#ifndef NEW_WILLOWS
   /*
    * Lookup the color name in the X color name database
    */
   if (XParseColor (GDp (0), cmap (0), colname, &color))
     {
	/* normalize RGB color values to 8 bits values */
	color.red >>= 8;
	color.green >>= 8;
	color.blue >>= 8;
	*red = color.red;
	*green = color.green;
	*blue = color.blue;
     }
#endif /* NEW_WILLOWS */
}


/* ---------------------------------------------------------------------- */
/* |    FindColor looks for the named color ressource.                  | */
/* |       The result is the closest color found the Thot color table.  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static boolean      FindColor (int disp, char *name, char *colorplace, char *defaultcolor, unsigned long *colorpixel)
#else  /* __STDC__ */
static boolean      FindColor (disp, name, colorplace, defaultcolor, colorpixel)
int                 disp;
char               *name;
char               *colorplace;
char               *defaultcolor;
unsigned long      *colorpixel;

#endif /* __STDC__ */
{
   int                 col;
   char               *value;
   unsigned short      red;
   unsigned short      green;
   unsigned short      blue;

   value = (char *) TtaGetEnvString (colorplace);
   /* faut-il prendre la valeur par defaut ? */
   if (value == NULL && defaultcolor != NULL)
      value = defaultcolor;

   if (value != NULL)
     {
	TtaGiveRGB (value, &red, &green, &blue);
	col = TtaGetThotColor (red, green, blue);
	/* register the default background color */
	if (strcmp (colorplace, "BackgroundColor") == 0)
	   DefaultBColor = col;
	*colorpixel = ColorPixel (col);
	return (True);
     }
   else
      return (False);
}


/* ---------------------------------------------------------------------- */
/* |    InitColors initialise les couleurs (X11)                        | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         InitColors (char *name)
#else  /* __STDC__ */
static void         InitColors (name)
char               *name;

#endif /* __STDC__ */
{
#ifndef NEW_WILLOWS
   ThotColorStruct     col;
   boolean             found;

   /* L'ordre d'allocation du blanc et du  noir depend de l'ecran */
   if (XWhitePixel (GDp (0), ThotScreen (0)) == 0)
     {
	if (!XAllocNamedColor (GDp (0), cmap (0), "White", &cwhite, &col))
	   TtaDisplaySimpleMessage (LIB, FATAL, LIB_ERR_NOT_ENOUGH_MEM);
	if (!XAllocNamedColor (GDp (0), cmap (0), "Black", &cblack, &col))
	   TtaDisplaySimpleMessage (LIB, FATAL, LIB_ERR_NOT_ENOUGH_MEM);
     }
   else
     {
	if (!XAllocNamedColor (GDp (0), cmap (0), "Black", &cblack, &col))
	   TtaDisplaySimpleMessage (LIB, FATAL, LIB_ERR_NOT_ENOUGH_MEM);
	if (!XAllocNamedColor (GDp (0), cmap (0), "White", &cwhite, &col))
	   TtaDisplaySimpleMessage (LIB, FATAL, LIB_ERR_NOT_ENOUGH_MEM);
     }

   /* Initialise les couleurs de l'application */
   Black_Color = cblack.pixel;
   Button_Color = Select_Color = cblack.pixel;
   White_Color = cwhite.pixel;
   Scroll_Color = BgMenu_Color = cwhite.pixel;


   if (Gdepth (0) > 1)
     {
	/* La couleur de fond */
	found = FindColor (0, name, "BackgroundColor", "gainsboro", &White_Color);
	/* La couleur de trace */
	found = FindColor (0, name, "ForegroundColor", "black", &Black_Color);
	/* La couleur bandeaux */
	found = FindColor (0, name, "ScrollColor", "grey", &Scroll_Color);
	/* Lecture de la couleur select-menu */
	found = FindColor (0, name, "MenuBgColor", "grey", &BgMenu_Color);
	/* La couleur Selection */
	found = FindColor (0, name, "DocSelectColor", "SteelBlue", &Select_Color);
	/* La couleur des bords */
	found = FindColor (0, name, "ButtonColor", "grey", &Button_Color);
	/* La couleur des boutons inactifs */
	found = FindColor (0, name, "InactiveItemColor", "LightGrey", &InactiveB_Color);
     }
   else
      /* La couleur Selection */
      found = FindColor (0, name, "DocSelectColor", "Black", &Select_Color);

   /* La couleur des references */
   found = FindColor (0, name, "ActiveBoxColor", "Red", &(Box_Color));
   if (!found)
      /* La couleur n'est pas definie -> Non utilisee */
      Box_Color = cwhite.pixel;
   else if (Gdepth (0) == 1)
      Box_Color = cblack.pixel;

   /* La couleur du Read Only */
   found = FindColor (0, name, "ReadOnlyColor", "MediumBlue", &(RO_Color));
   if (!found)
      /* La couleur n'est pas definie -> Non utilisee */
      RO_Color = cwhite.pixel;
   else if (Gdepth (0) == 1)
      RO_Color = cblack.pixel;
#endif /* NEW_WILLOWS */
}


/* ---------------------------------------------------------------------- */
/* |     ShowReference rend la valeur Vrai s'il existe une couleur de   | */
/* |            boi^te active.                                          | */
/* ---------------------------------------------------------------------- */
boolean             ShowReference ()
{
#ifndef NEW_WILLOWS
   if (Box_Color == cwhite.pixel)
      return (False);
   else
      return (True);
#endif /* NEW_WILLOWS */
}


/* ---------------------------------------------------------------------- */
/* |     ShowReadOnly rend la valeur Vrai s'il existe une couleur RO.   | */
/* ---------------------------------------------------------------------- */
boolean             ShowReadOnly ()
{
#ifndef NEW_WILLOWS
   if (RO_Color == cwhite.pixel)
      return (False);
   else
      return (True);
#endif /* NEW_WILLOWS */
}
/*fin */

/* ---------------------------------------------------------------------- */
/* |    InitGC initialise les contextes graphiques (X11).               | */
/* ---------------------------------------------------------------------- */
void                InitGC ()
{
#ifndef NEW_WILLOWS
   unsigned long       valuemask;
   unsigned long       white;
   unsigned long       black;
   XGCValues           GCmodel;
   Pixmap              pix;

#endif /* NEW_WILLOWS */
#ifdef NEW_WILLOWS
   ThotGC              gcModel;

#endif

   /*
    * Create a Graphic Context to write white on black.
    */
#ifndef NEW_WILLOWS
   valuemask = GCForeground | GCBackground | GCFunction;
   white = ColorNumber ("White");
   black = ColorNumber ("Black");

   GCmodel.function = GXcopy;
   pix = CreatePattern (0, 0, 0, black, white, 6);	/* !!!! */
   GCmodel.foreground = White_Color;
   GCmodel.background = Black_Color;
   GCwhite (0) = XCreateGC (GDp (0), GRootW (0), valuemask, &GCmodel);
#endif /* NEW_WILLOWS */
#ifdef NEW_WILLOWS
   gcModel = GCwhite (0);
   gcModel->capabilities = THOT_GC_PEN | THOT_GC_FOREGROUND | THOT_GC_BACKGROUND;
   gcModel->pen = GetStockObject (WHITE_PEN);
   gcModel->background = Black_Color;
   gcModel->foreground = White_Color;
#endif /* NEW_WILLOWS */

   /*
    * Create a Graphic Context to write black on white.
    */
#ifndef NEW_WILLOWS
   GCmodel.foreground = Button_Color;
   GCmodel.background = White_Color;
   GCblack (0) = XCreateGC (GDp (0), GRootW (0), valuemask, &GCmodel);
#endif /* NEW_WILLOWS */
#ifdef NEW_WILLOWS
   gcModel = GCblack (0);
   gcModel->capabilities = THOT_GC_PEN | THOT_GC_FOREGROUND | THOT_GC_BACKGROUND;
   gcModel->pen = GetStockObject (BLACK_PEN);
   gcModel->background = White_Color;
   gcModel->foreground = Black_Color;
#endif /* NEW_WILLOWS */

   /*
    * Create a Graphic Context to write black on white,
    * but with a specific 10101010 pattern.
    */
#ifndef NEW_WILLOWS
   GCmodel.foreground = Black_Color;
   GCmodel.background = White_Color;
   GCtrait (0) = XCreateGC (GDp (0), GRootW (0), valuemask, &GCmodel);
   XSetTile (GDp (0), GCtrait (0), pix);
#endif /* NEW_WILLOWS */
#ifdef NEW_WILLOWS
   gcModel = GCtrait (0);
   gcModel->capabilities = THOT_GC_FOREGROUND |		/* THOT_GC_BACKGROUND | */
   /* THOT_GC_BRUSH | */ THOT_GC_PEN;
   gcModel->pen = GetStockObject (BLACK_PEN);
   /* gcModel->background = White_Color; */
   gcModel->foreground = Black_Color;
   /* !!!! WIN_LastBitmap created by pix = CreatePattern(...); */
   /* gcModel->brush = CreatePatternBrush(WIN_LastBitmap); */
#endif /* NEW_WILLOWS */

   /*
    * Another Graphic Context to write black on white, for dialogs.
    */
#ifndef NEW_WILLOWS
   GCdialogue (0) = XCreateGC (GDp (0), GRootW (0), valuemask, &GCmodel);
#endif /* NEW_WILLOWS */
#ifdef NEW_WILLOWS
   gcModel = GCdialogue (0);
   gcModel->capabilities = THOT_GC_FOREGROUND | THOT_GC_BACKGROUND | THOT_GC_PEN;
   gcModel->pen = GetStockObject (BLACK_PEN);
   gcModel->background = White_Color;
   gcModel->foreground = Black_Color;
#endif /* NEW_WILLOWS */

   /*
    * A Graphic Context to show selected objects. On X-Windows,
    * the colormap indexes are XORed to show the object without
    * destroying the colors : XOR.XOR = I ...
    */
#ifndef NEW_WILLOWS
   GCmodel.function = GXinvert;
   GCmodel.plane_mask = Select_Color;
   if (Gdepth (0) > 1)
      GCmodel.foreground = Black_Color;
   else
      GCmodel.foreground = Select_Color;
   GCmodel.background = White_Color;
   GCinvert (0) = XCreateGC (GDp (0), GRootW (0), valuemask | GCPlaneMask, &GCmodel);
#endif /* NEW_WILLOWS */
#ifdef NEW_WILLOWS
   gcModel = GCdialogue (0);
   gcModel->capabilities = THOT_GC_FOREGROUND | THOT_GC_BACKGROUND |
      THOT_GC_FUNCTION;
   gcModel->mode = R2_XORPEN;
   if (Gdepth (0) > 1)
      gcModel->foreground = Black_Color;
   else
      gcModel->foreground = Select_Color;
   gcModel->background = White_Color;
#endif /* NEW_WILLOWS */

   /*
    * A Graphic Context to for trame objects.
    */
#ifndef NEW_WILLOWS
   GCmodel.function = GXcopy;
   GCmodel.foreground = Black_Color;
   GCgrey (0) = XCreateGC (GDp (0), GRootW (0), valuemask, &GCmodel);
   XSetFillStyle (GDp (0), GCgrey (0), FillTiled);
   XFreePixmap (GDp (0), pix);
#endif /* NEW_WILLOWS */
#ifdef NEW_WILLOWS
   gcModel = GCgrey (0);
   gcModel->capabilities = THOT_GC_FOREGROUND | THOT_GC_BACKGROUND |
   /* THOT_GC_BRUSH | */ THOT_GC_PEN |
      THOT_GC_FUNCTION;
   gcModel = GCblack (0);
   gcModel->background = White_Color;
   gcModel->foreground = Black_Color;
   gcModel->mode = R2_XORPEN;
   /* !!!! WIN_LastBitmap created by pix = CreatePattern(...); */
   /* gcModel->brush = CreatePatternBrush(WIN_LastBitmap); */
   DeleteObject (WIN_LastBitmap);
   WIN_LastBitmap = 0;
#endif
}


/* ---------------------------------------------------------------------- */
/* |    InitEcrans fait les initialisations des ecrans.                 | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                InitEcrans (char *name, int dx, int dy)
#else  /* __STDC__ */
void                InitEcrans (name, dx, dy)
char               *name;
int                 dx;
int                 dy;

#endif /* __STDC__ */
{
#ifdef NEW_WILLOWS
   HDC                 hdc;

   hdc = GetDC (WIN_Main_Wd);
   Larg_Ecran (0) = GetSystemMetrics (SM_CXSCREEN);
   Haut_Ecran (0) = GetSystemMetrics (SM_CYSCREEN);
   Gdepth (0) = GetDeviceCaps (hdc, PLANES);
   if (Gdepth (0) == 1)
      Gdepth (0) = GetDeviceCaps (hdc, BITSPIXEL);

   /* Position par defaut du 1er menu */
   XCurs (0) = 100;
   YCurs (0) = 100;
   ReleaseDC (WIN_Main_Wd, hdc);
#endif /* NEW_WILLOWS */

   /* En pixel */
#ifndef NEW_WILLOWS
   Larg_Ecran (0) = DisplayWidth (GDp (0), ThotScreen (0));
   Haut_Ecran (0) = DisplayHeight (GDp (0), ThotScreen (0));
   XSetErrorHandler (X_Erreur);
   XSetIOErrorHandler (X_ErreurFatale);
   ThotScreen (0) = DefaultScreen (GDp (0));
   Gdepth (0) = DefaultDepth (GDp (0), ThotScreen (0));
   GRootW (0) = RootWindow (GDp (0), ThotScreen (0));
   cmap (0) = XDefaultColormap (GDp (0), ThotScreen (0));
#endif

   /* Position par defaut du 1er menu */
   XCurs (0) = 100;
   YCurs (0) = 100;

   /* Largeur des bandes de scroll */
   InitDocColors (name);
   InitColors (name);
   InitGC ();
   InitCurs ();			/* curseurs                           */
   InitFont (name);		/* polices de caracteres              */

   /* Initialisation des Picture Drivers */
   InitImageDrivers (XWindowSystem);
   OpenAllImageDrivers (XWindowSystem);
}

/* ---------------------------------------------------------------------- */
/* |    InitContexts initialise les contextes de frames.                | */
/* ---------------------------------------------------------------------- */
void                InitDocContexts ()
{
   int                 i;

   MemInit ();			/* Initialisation de la gestion memoire */

   /* Initialisation de la table des frames */
   for (i = 0; i <= MAX_FRAME; i++)
      FrRef[i] = 0;

   MIN_BLANC = 3;		/* Largeur minmale d'un espace */
   MAX_BLANC = 6;		/* Largeur maximale d'un espace */
   DELTA = 4;			/* Erreur de position autorisee */
   Placement = NULL;
   /* Pas de placement absolu de boites en cours */
   Englobement = NULL;
   /* Aucun traitement de l'englobement n'est enregistre */
   RetardeEngl = NULL;
   /* Aucune boite dont l'englobement est differe */
   EnCreation = False;		/* Pas de creation interactive en cours */
   InitAutreContexts ();

}				/*InitContexts */

#ifndef NEW_WILLOWS
/* ---------------------------------------------------------------------- */
/* |    SelectionEvents traite les evenements de selection.             | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                SelectionEvents (XSelectionEvent * event)
#else  /* __STDC__ */
void                SelectionEvents (event)
XSelectionEvent    *event;

#endif /* __STDC__ */
{
   XSelectionRequestEvent *request;
   XSelectionEvent     notify;
   ThotWindow          w, wind;
   Atom                type;
   int                 format, r, frame;
   unsigned long       nbitems, bytes_after;
   unsigned char      *buffer;

   switch (event->type)
	 {
	    case SelectionClear:
	       /* Perte de la selection courante -> il faut liberer le Xbuffer */
	       w = ((XSelectionClearEvent *) event)->window;
	       wind = 0;
	       frame = 0;
	       while (wind == 0 && frame <= MAX_FRAME)
		 {
		    if (w == FrRef[frame])
		       wind = w;
		    frame++;
		 }
	       if (w == wind && event->display == GDp (0))
		 {
		    if (Xbuffer != NULL)
		      {
			 /* libere le buffer des donnees */
			 free (Xbuffer);
			 Xbuffer = NULL;
			 LgXbuffer = 0;
		      }
		 }
	       break;

	    case SelectionNotify:
	       /* Reception du Xbuffer -> copier dans le document concerne */
	       /* recherche si une frame thot est concernee */
	       w = event->requestor;
	       wind = 0;
	       frame = 0;
	       while (wind == 0 && frame <= MAX_FRAME)
		 {
		    if (w == FrRef[frame])
		       wind = w;
		    frame++;
		 }
	       if (w == wind && event->display == GDp (0))
		 {
		    if (event->property == None)
		      {
			 /* Pas de selection courante -> on regarde s'il y a un cutbuffer */
			 buffer = (unsigned char *) XFetchBytes (GDp (0), &r);
			 if (buffer != NULL)
			   {
			      /* Retourne le cutbuffer */
			      if (ThotLocalActions[T_pasteClipboard] != NULL)
				 (*ThotLocalActions[T_pasteClipboard]) (buffer, r);
			   }
		      }
		    else
		      {
			 /* Recupere les donnees */
			 r = XGetWindowProperty (event->display, event->requestor,
			       event->property, (long) 0, (long) 256, False,
				  AnyPropertyType, &type, &format, &nbitems,
						 &bytes_after, &buffer);
			 if (r == Success && type != None && format == 8)
			   {
			      /* format traite */
			      /* Colle le contenu de la selection rendue dans le document */
			      if (ThotLocalActions[T_pasteClipboard] != NULL)
				 (*ThotLocalActions[T_pasteClipboard]) (buffer, (int) nbitems);
			   }
		      }
		 }
	       break;

	    case SelectionRequest:
	       /* Demande du Xbuffer -> copier le Xbuffer dans le buffer X */
	       w = ((XSelectionRequestEvent *) event)->owner;
	       wind = 0;
	       frame = 0;
	       while (wind == 0 && frame <= MAX_FRAME)
		 {
		    if (w == FrRef[frame])
		       wind = w;
		    frame++;
		 }
	       if (w == wind && event->display == GDp (0))
		 {
		    request = (XSelectionRequestEvent *) event;
		    /* Construit l'evenement notify */
		    notify.type = SelectionNotify;
		    notify.display = request->display;
		    notify.requestor = request->requestor;
		    notify.selection = request->selection;
		    notify.target = request->target;
		    notify.time = request->time;

		    if (Xbuffer == NULL)
		      {
			 /* La selection est vide -> vide le cutbuffer */
			 XStoreBuffer (request->display, NULL, 0, 0);
			 notify.property = None;
			 XSendEvent (request->display, request->requestor, True, NoEventMask, (XEvent *) & notify);
		      }
		    else if (request->property == None)
		      {
			 /* Il n'y a pas de propriete specifiee */
			 XStoreBuffer (request->display, Xbuffer, LgXbuffer, 0);
		      }
		    else
		      {
			 /* On range la valeur dans la propriete specifiee */
			 XChangeProperty (request->display, request->requestor, request->property,
			 XA_STRING, 8, PropModeReplace, Xbuffer, LgXbuffer);
			 /* Signale que la selection est transmise */
			 notify.property = request->property;
			 XSendEvent (request->display, request->requestor, True, NoEventMask, (XEvent *) & notify);
		      }
		 }
	       break;
	 }
}
#endif /* NEW_WILLOWS */
