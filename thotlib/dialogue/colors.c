
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */
/* I. Vatton    Septembre 1994 */
/* Module de visualisation des claviers. */


#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "constmenu.h"
#include "appdialogue.h"
#include "message.h"
#include "dialog.h"

#undef EXPORT
#define EXPORT extern
#include "edit.var"
#include "frame.var"
#include "font.var"
#include "select.var"
#include "appdialogue.var"

#define COLORS_COL 8
#define MAX_ARGS 20

static ThotWindow   Color_Window = 0;
static ThotWidget   Color_Palette;
static int          lastBg;
static int          lastFg;
static ThotGC       GCkey;

#include "config.f"
#include "appdialogue.f"
#include "es.f"
#include "font.f"
#include "inites.f"
#include "modpres.f"
#include "select.f"

#ifdef __STDC__
extern ThotWidget   XmCreateBulletinBoard (ThotWidget, char *, Arg[], int);

#else  /* __STDC__ */
extern ThotWidget   XmCreateBulletinBoard ();

#endif /* __STDC__ */

/* ---------------------------------------------------------------------- */
/* |    FinPalette termine l'affichage de la palette.                   | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         ThotSelectPalette (int bground, int fground)
#else  /* __STDC__ */
static void         ThotSelectPalette (bground, fground)
int                 bground;
int                 fground;

#endif /* __STDC__ */
{
   int                 x, y;
   int                 wcase, hcase;
   int                 w, h;

   if (Gdepth (0) == 1)
      /* Affiche le nom des couleurs sur un ecran N&B */
      wcase = CarWidth ('m', FontMenu) * 12;
   else
      /* Affiche les couleurs sur un ecran couleur */
      wcase = CarWidth ('m', FontMenu) * 4;

   hcase = FontHeight (FontMenu);
   if (lastBg != -1 && lastBg != bground)
     {
	/* eteint le background precedent */
	x = (lastBg % COLORS_COL) * wcase;
	y = ((lastBg / COLORS_COL) + 1) * hcase;
#ifndef NEW_WILLOWS
	x -= 2;
	y -= 2;
	w = wcase + 2;
	h = hcase + 2;
	XFillRectangle (GDp (0), Color_Window, GCinvert (0), x + 2, y, w, 2);
	XFillRectangle (GDp (0), Color_Window, GCinvert (0), x, y, 2, h);
	XFillRectangle (GDp (0), Color_Window, GCinvert (0), x, y + h, w, 2);
	XFillRectangle (GDp (0), Color_Window, GCinvert (0), x + w, y + 2, 2, h);
#endif /* NEW_WILLOWS */
     }

   if (lastFg != -1 && lastFg != fground)
     {
	/* eteint le foreground precedent */
	x = (lastFg % COLORS_COL) * wcase;
	y = ((lastFg / COLORS_COL) + 1) * hcase;
#ifndef NEW_WILLOWS
	w = wcase - 2;
	h = hcase - 2;
	XFillRectangle (GDp (0), Color_Window, GCinvert (0), x + 2, y, w, 2);
	XFillRectangle (GDp (0), Color_Window, GCinvert (0), x, y, 2, h);
	XFillRectangle (GDp (0), Color_Window, GCinvert (0), x, y + h, w, 2);
	XFillRectangle (GDp (0), Color_Window, GCinvert (0), x + w, y + 2, 2, h);
#endif /* NEW_WILLOWS */
     }

   /* nouveau background */
   if (lastBg != bground)
     {
	lastBg = bground;
	if (lastBg != -1)
	  {
	     x = (lastBg % COLORS_COL) * wcase;
	     y = ((lastBg / COLORS_COL) + 1) * hcase;
#ifndef NEW_WILLOWS
	     x -= 2;
	     y -= 2;
	     w = wcase + 2;
	     h = hcase + 2;
	     XFillRectangle (GDp (0), Color_Window, GCinvert (0), x + 2, y, w, 2);
	     XFillRectangle (GDp (0), Color_Window, GCinvert (0), x, y, 2, h);
	     XFillRectangle (GDp (0), Color_Window, GCinvert (0), x, y + h, w, 2);
	     XFillRectangle (GDp (0), Color_Window, GCinvert (0), x + w, y + 2, 2, h);
#endif /* NEW_WILLOWS */
	  }
     }

   /* nouveau foreground */
   if (lastFg != fground)
     {
	lastFg = fground;
	if (lastFg != -1)
	  {
	     x = (lastFg % COLORS_COL) * wcase;
	     y = ((lastFg / COLORS_COL) + 1) * hcase;
#ifndef NEW_WILLOWS
	     w = wcase - 2;
	     h = hcase - 2;
	     XFillRectangle (GDp (0), Color_Window, GCinvert (0), x + 2, y, w, 2);
	     XFillRectangle (GDp (0), Color_Window, GCinvert (0), x, y, 2, h);
	     XFillRectangle (GDp (0), Color_Window, GCinvert (0), x, y + h, w, 2);
	     XFillRectangle (GDp (0), Color_Window, GCinvert (0), x + w, y + 2, 2, h);
#endif /* NEW_WILLOWS */
	  }
     }
}


/* ---------------------------------------------------------------------- */
/* |    FinPalette termine l'affichage de la palette.                   | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         FinPalette (ThotWidget w, int index, caddr_t call_d)

#else  /* __STDC__ */
static void         FinPalette (w, index, call_d)
ThotWidget          w;
int                 index;
caddr_t             call_d;

#endif /* __STDC__ */
{
#ifndef NEW_WILLOWS
   XtPopdown (Color_Palette);
#endif
}


/* ---------------------------------------------------------------------- */
/* |    KillPalette detruit la palette.                                 | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         KillPalette (ThotWidget w, int index, caddr_t call_d)

#else  /* __STDC__ */
static void         KillPalette (w, index, call_d)
ThotWidget          w;
int                 index;
caddr_t             call_d;

#endif /* __STDC__ */
{
   Color_Palette = 0;
   Color_Window = 0;
}


/* ---------------------------------------------------------------------- */
/* |    ColorsExpose reaffiche un clavier des couleurs.                 | */
/* ---------------------------------------------------------------------- */
static void         ColorsExpose ()
{
   register int        i;
   int                 max, x, y, w, h;
   int                 wcase, hcase;
   char               *ptr;
   int                 fground, bground;

   if (Gdepth (0) == 1)
      /* Affiche le nom des couleurs sur un ecran N&B */
      wcase = CarWidth ('m', FontMenu) * 12;
   else
      /* Affiche les couleurs sur un ecran couleur */
      wcase = CarWidth ('m', FontMenu) * 4;

   w = wcase * COLORS_COL;
   hcase = FontHeight (FontMenu);
   max = NumberOfColors ();
   y = hcase;
   h = (max + COLORS_COL - 1) / COLORS_COL * hcase + y;
#ifndef NEW_WILLOWS
   XClearWindow (GDp (0), Color_Window);

   /* entree couleur standard */
   if (FonteLeg == NULL)
      KeyboardsLoadResources ();
   XSetForeground (GDp (0), GCtrait (0), ColorPixel (0));
   XFillRectangle (GDp (0), Color_Window, GCtrait (0), 0, 0, w, hcase - 2);
   XSetForeground (GDp (0), GCtrait (0), ColorPixel (1));
   ptr = TtaGetMessage (LIB, LIB_STANDARD_COLORS);
   WChaine (Color_Window, ptr,
   (w / 2) - (XTextWidth ((XFontStruct *) FonteLeg, ptr, strlen (ptr)) / 2),
	    0, FontMenu, GCtrait (0));

   /* grille */
   XSetLineAttributes (GDp (0), GCtrait (0), 1, LineSolid, CapButt, JoinMiter);
   for (x = wcase; x < w; x += wcase)
      XDrawLine (GDp (0), Color_Window, GCtrait (0), x, y, x, h);
   for (y = hcase; y < h; y += hcase)
      XDrawLine (GDp (0), Color_Window, GCtrait (0), 0, y, w, y);

   /* items */
   if (Gdepth (0) == 1)
      for (i = 0; i < max; i++)
	{
	   x = (i % COLORS_COL) * wcase;
	   y = ((i / COLORS_COL) + 1) * hcase;
	   WChaine (Color_Window, ColorName (i),
		    x + (wcase / 2) - (XTextWidth ((XFontStruct *) FonteLeg, ColorName (i), strlen (ColorName (i))) / 2),
		    y, FonteLeg, GCtrait (0));
	}
   else
      for (i = 0; i < max; i++)
	{
	   x = (i % COLORS_COL) * wcase;
	   y = ((i / COLORS_COL) + 1) * hcase;
	   XSetForeground (GDp (0), GCtrait (0), ColorPixel (i));
	   XFillRectangle (GDp (0), Color_Window, GCtrait (0), x + 1, y + 1, wcase - 2, hcase - 2);
	}

   /* show the current selection */
   fground = lastFg;
   bground = lastBg;
   lastFg = -1;
   lastBg = -1;
   ThotSelectPalette (bground, fground);
#endif /* NEW_WILLOWS */
}


/* ---------------------------------------------------------------------- */
/* |    ColorsPress traite un clic dans la palette des couleurs.        | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         ColorsPress (int bouton, int x, int y)
#else  /* __STDC__ */
static void         ColorsPress (bouton, x, y)
int                 bouton;
int                 x;
int                 y;

#endif /* __STDC__ */
{
   int                 couleur, li, co;
   int                 wcase, hcase;

   if (Gdepth (0) == 1)
      /* Affiche le nom des couleurs sur un ecran N&B */
      wcase = CarWidth ('m', FontMenu) * 12;
   else
      /* Affiche les couleurs sur un ecran couleur */
      wcase = CarWidth ('m', FontMenu) * 4;
   hcase = FontHeight (FontMenu);

   /* Regarde si on n'a pas clique dans le titre */
   if (y < hcase)
     {
#ifdef NEW_WILLOWS
#define Button1 1		/* MSWindows will probably use same model */
#endif
	if (bouton == Button1)
	  {
	     /* couleur de trace' standard */
	     ChangeCouleur (-1, False);
	     ThotSelectPalette (lastBg, -1);
	  }
	else
	  {
	     /* couleur de fond standard */
	     ChangeCouleur (-1, True);
	     ThotSelectPalette (-1, lastFg);
	  }
	return;
     }

   li = x / wcase;
   co = (y - hcase) / hcase;
   couleur = co * COLORS_COL + li;
   if (bouton == Button1)
     {
	/* selectionne la couleur de trace' */
	ChangeCouleur (couleur, False);
	ThotSelectPalette (lastBg, couleur);
     }
   else
     {
	/* selectionne la couleur de fond */
	ChangeCouleur (couleur, True);
	ThotSelectPalette (couleur, lastFg);
     }

}


#ifdef WWW_XWINDOWS
/* ---------------------------------------------------------------------- */
/* |    ColorsEvent traite les Evenements X sur la palette.             | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                ColorsEvent (XEvent * event)
#else  /* __STDC__ */
void                ColorsEvent (event)
XEvent             *event;

#endif /* __STDC__ */
{
   if (event->xbutton.window == Color_Window && Color_Window != 0)
     {
	if (event->type == Expose)
	   ColorsExpose ();
	else if (event->type == ButtonPress)
	   ColorsPress (event->xbutton.button, event->xbutton.x, event->xbutton.y);
     }
}
#endif /* WWW_XWINDOWS */


/* ---------------------------------------------------------------------- */
/* |    ThotCreatePalette cree la palette de couleurs.                      | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         ThotCreatePalette (int x, int y)
#else  /* __STDC__ */
static void         ThotCreatePalette (x, y)
int                 x;
int                 y;

#endif /* __STDC__ */
{
#ifndef NEW_WILLOWS
   int                 n;
   int                 width, height;
   ThotWidget          w;
   ThotWidget          row;
   ThotWidget          frame;
   Arg                 args[MAX_ARGS];
   XmString            title_string;
   XmFontList          xfont;
   XGCValues           GCmodel;
   char                chaine[10];

   xfont = XmFontListCreate ((XFontStruct *) FontMenu, XmSTRING_DEFAULT_CHARSET);
   if (FonteLeg == NULL)
      FonteLeg = LireFonte ('L', 'H', 0, 9, UnPoint);

   n = 0;
   sprintf (chaine, "+%d+%d", x, y);
   XtSetArg (args[n], XmNx, (Position) x);
   n++;
   XtSetArg (args[n], XmNy, (Position) y);
   n++;
   XtSetArg (args[n], XmNallowShellResize, True);
   n++;
   XtSetArg (args[n], XmNuseAsyncGeometry, True);
   n++;
   Color_Palette = XtCreatePopupShell (TtaGetMessage (LIB, LIB_COLORS),
			   applicationShellWidgetClass, RootShell, args, n);
/*** Cree la palette dans sa frame ***/
   n = 0;
   XtSetArg (args[n], XmNbackground, BgMenu_Color);
   n++;
   XtSetArg (args[n], XmNborderColor, Button_Color);
   n++;
   XtSetArg (args[n], XmNfontList, xfont);
   n++;
   title_string = XmStringCreateSimple (TtaGetMessage (LIB, LIB_COLORS));
   XtSetArg (args[n], XmNdialogTitle, title_string);
   n++;
   XtSetArg (args[n], XmNautoUnmanage, False);
   n++;
   XtSetArg (args[n], XmNmarginWidth, 0);
   n++;
   XtSetArg (args[n], XmNmarginHeight, 0);
   n++;
   XtSetArg (args[n], XmNspacing, 0);
   n++;
   XtSetArg (args[n], XmNresizePolicy, XmRESIZE_GROW);
   n++;
   w = XmCreateBulletinBoard (Color_Palette, "Dialogue", args, n);
   XtManageChild (w);
   XtAddCallback (w, XmNdestroyCallback, (XtCallbackProc) KillPalette, NULL);
   XmStringFree (title_string);

/*** Cree un Row-Column pour ajouter les labels et le bouton Quit ***/
/*** au dessus et en dessous des touches du clavier.    ***/
   n = 0;
   XtSetArg (args[n], XmNbackground, BgMenu_Color);
   n++;
   XtSetArg (args[n], XmNborderColor, Button_Color);
   n++;
   XtSetArg (args[n], XmNadjustLast, False);
   n++;
   XtSetArg (args[n], XmNmarginWidth, 0);
   n++;
   XtSetArg (args[n], XmNmarginHeight, 0);
   n++;
   XtSetArg (args[n], XmNspacing, 0);
   n++;
   XtSetArg (args[n], XmNpacking, XmPACK_TIGHT);
   n++;
   XtSetArg (args[n], XmNorientation, XmVERTICAL);
   n++;
   XtSetArg (args[n], XmNresizeHeight, True);
   n++;
   row = XmCreateRowColumn (w, "Dialogue", args, n);
   XtManageChild (row);

   /* Les labels */
   n = 0;
   XtSetArg (args[n], XmNborderColor, Button_Color);
   n++;
   XtSetArg (args[n], XmNforeground, Black_Color);
   n++;
   XtSetArg (args[n], XmNfontList, xfont);
   n++;
   XtSetArg (args[n], XmNbackground, BgMenu_Color);
   n++;
   title_string = XmStringCreateSimple (TtaGetMessage (LIB, LIB_BUTTON_1));
   XtSetArg (args[n], XmNlabelString, title_string);
   n++;
   w = XmCreateLabel (row, "Thot_MSG", args, n);
   XtManageChild (w);
   XmStringFree (title_string);
   n = 0;
   XtSetArg (args[n], XmNborderColor, Button_Color);
   n++;
   XtSetArg (args[n], XmNforeground, Black_Color);
   n++;
   XtSetArg (args[n], XmNfontList, xfont);
   n++;
   XtSetArg (args[n], XmNbackground, BgMenu_Color);
   n++;
   title_string = XmStringCreateSimple (TtaGetMessage (LIB, LIB_BUTTON_2));
   XtSetArg (args[n], XmNlabelString, title_string);
   n++;
   w = XmCreateLabel (row, "Thot_MSG", args, n);
   XtManageChild (w);
   XmStringFree (title_string);

   /* Evalue la largeur et la hauteur de la palette */
   n = 0;
   if (Gdepth (0) == 1)
      /* Affiche le nom des couleurs sur un ecran N&B */
      width = CarWidth ('m', FontMenu) * 12 * COLORS_COL;
   else
      /* Affiche les couleurs sur un ecran couleur */
      width = CarWidth ('m', FontMenu) * 4 * COLORS_COL;
   height = ((NumberOfColors () + COLORS_COL - 1) / COLORS_COL + 1) * FontHeight (FontMenu);

/*** Cree un DrawingArea pour contenir les touches de la palette ***/
   n = 0;
   XtSetArg (args[n], XmNborderColor, Button_Color);
   n++;
   XtSetArg (args[n], XmNfontList, xfont);
   n++;
   XtSetArg (args[n], XmNbackground, BgMenu_Color);
   n++;
   XtSetArg (args[n], XmNmarginWidth, 0);
   n++;
   XtSetArg (args[n], XmNmarginHeight, 0);
   n++;
   XtSetArg (args[n], XmNwidth, (Dimension) width);
   n++;
   XtSetArg (args[n], XmNheight, (Dimension) height);
   n++;
   XtSetArg (args[n], XmNkeyboardFocusPolicy, XmPOINTER);
   n++;
   frame = XmCreateFrame (row, "Frame", args, n);
   XtManageChild (frame);
   n = 0;
   XtSetArg (args[n], XmNborderColor, Button_Color);
   n++;
   XtSetArg (args[n], XmNfontList, xfont);
   n++;
   XtSetArg (args[n], XmNbackground, BgMenu_Color);
   n++;
   XtSetArg (args[n], XmNmarginWidth, 0);
   n++;
   XtSetArg (args[n], XmNmarginHeight, 0);
   n++;
   XtSetArg (args[n], XmNwidth, (Dimension) width);
   n++;
   XtSetArg (args[n], XmNheight, (Dimension) height);
   n++;
   XtSetArg (args[n], XmNkeyboardFocusPolicy, XmPOINTER);
   n++;
   frame = XmCreateDrawingArea (frame, "", args, n);
   XtManageChild (frame);

/*** Cree un Row-Column pour contenir le bouton Quit ***/
   n = 0;
   XtSetArg (args[n], XmNborderColor, Button_Color);
   n++;
   XtSetArg (args[n], XmNfontList, xfont);
   n++;
   XtSetArg (args[n], XmNbackground, BgMenu_Color);
   n++;
   XtSetArg (args[n], XmNorientation, XmHORIZONTAL);
   n++;
   XtSetArg (args[n], XmNmarginWidth, 100);
   n++;
   XtSetArg (args[n], XmNmarginHeight, 0);
   n++;
   row = XmCreateRowColumn (row, "Dialogue", args, n);
   XtManageChild (row);

   /* Cree si necessaire le contexte graphique des cles */
   GCmodel.function = GXcopy;
   GCmodel.foreground = Black_Color;
   GCmodel.background = BgMenu_Color;
   GCkey = XCreateGC (GDp (0), GRootW (0), GCForeground | GCBackground | GCFunction, &GCmodel);

/*** Cree le bouton Quit ***/
   n = 0;
   XtSetArg (args[n], XmNbackground, Button_Color);
   n++;
   XtSetArg (args[n], XmNbottomShadowColor, BgMenu_Color);
   n++;
   XtSetArg (args[n], XmNforeground, Black_Color);
   n++;
   XtSetArg (args[n], XmNfontList, xfont);
   n++;
   w = XmCreatePushButton (row, TtaGetMessage (LIB, LIB_DONE), args, n);
   XtManageChild (w);
   XtAddCallback (w, XmNactivateCallback, (XtCallbackProc) FinPalette, NULL);

   /* Definit le bouton d'annulation comme bouton par defaut */
   n = 0;
   XtSetArg (args[n], XmNdefaultButton, w);
   n++;
   XtSetValues (Color_Palette, args, n);


   /* Force la largeur et la hauteur de la palette */
   XtSetArg (args[n], XmNwidth, (Dimension) width);
   n++;
   XtSetArg (args[n], XmNheight, (Dimension) height);
   n++;
   XtSetValues (frame, args, n);

   XmFontListFree (xfont);
   /* affiche la palette */
   XtPopup (Color_Palette, XtGrabNonexclusive);
   Color_Window = XtWindowOfObject (frame);
   /* pas de selection precedente */
   lastBg = -1;
   lastFg = -1;
#endif /* NEW_WILLOWS */
}


/* ---------------------------------------------------------------------- */
/* |    TtcChangeColors L'utilisateur demande a modifier les            | */
/* |            couleurs (presentation specifique) pour la vue          | */
/* |            Vue du document pDoc (si Assoc = False) ou les elements | */
/* |            associes de numero Vue (si Assoc = Vrai)                | */
/* |            Initialise et active le formulaire correspondant.       | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtcChangeColors (Document document, View view)
#else  /* __STDC__ */
void                TtcChangeColors (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   PtrDocument         SelDoc;
   PtrElement          PremSel, DerSel;
   int                 premcar, dercar;
   boolean             selok;
   PtrAbstractBox             pPav;
   PtrDocument         pDoc;

#ifndef NEW_WILLOWS
   if (ThotLocalActions[T_colors] == NULL)
     {
	/* Connecte le traitement des evenements */
	TteConnectAction (T_colors, (Proc) ColorsEvent);
     }
#endif /* NEW_WILLOWS */

   pDoc = TabDocuments[document - 1];
   /* demande quelle est la selection courante */
   selok = SelEditeur (&SelDoc, &PremSel, &DerSel, &premcar, &dercar);
   if (!selok)
     {
	/* par defaut la racine du document */
	SelDoc = pDoc;
	PremSel = pDoc->DocRootElement;
	selok = True;
     }

   if (selok && SelDoc == pDoc && NumberOfColors () > 0)
      /* il y a effectivement des couleurs disponibles */
     {
	/* Cree la palette si elle n'existe pas */
	if (Color_Window == 0)
	   ThotCreatePalette (800, 100);
#ifndef NEW_WILLOWS
	else
	   XtPopup (Color_Palette, XtGrabNonexclusive);
#endif /* NEW_WILLOWS */

	/* recherche le pave concerne */
	if (view > 100)
	   pPav = PaveDeElem (PremSel, 1);
	else
	   pPav = PaveDeElem (PremSel, view);

	if (pPav != NULL)
	   ThotSelectPalette (pPav->AbBackground, pPav->AbForeground);
     }
}
