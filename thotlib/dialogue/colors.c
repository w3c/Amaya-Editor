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
#include "edit_tv.h"
#include "frame_tv.h"
#include "font_tv.h"
#include "select_tv.h"
#include "appdialogue_tv.h"

#define COLORS_COL 8
#define MAX_ARGS 20

static ThotWindow   Color_Window = 0;
static ThotWidget   Color_Palette;
static int          LastBg;
static int          LastFg;
static ThotGC       GCkey;

#include "config_f.h"
#include "appdialogue_f.h"
#include "windowdisplay_f.h"
#include "font_f.h"
#include "changepresent_f.h"
#include "structselect_f.h"

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

   if (TtWDepth == 1)
      /* Affiche le nom des couleurs sur un ecran N&B */
      wcase = CarWidth ('m', FontDialogue) * 12;
   else
      /* Affiche les couleurs sur un ecran couleur */
      wcase = CarWidth ('m', FontDialogue) * 4;

   hcase = FontHeight (FontDialogue);
   if (LastBg != -1 && LastBg != bground)
     {
	/* eteint le background precedent */
	x = (LastBg % COLORS_COL) * wcase;
	y = ((LastBg / COLORS_COL) + 1) * hcase;
#ifndef NEW_WILLOWS
	x -= 2;
	y -= 2;
	w = wcase + 2;
	h = hcase + 2;
	XFillRectangle (TtDisplay, Color_Window, TtInvertGC, x + 2, y, w, 2);
	XFillRectangle (TtDisplay, Color_Window, TtInvertGC, x, y, 2, h);
	XFillRectangle (TtDisplay, Color_Window, TtInvertGC, x, y + h, w, 2);
	XFillRectangle (TtDisplay, Color_Window, TtInvertGC, x + w, y + 2, 2, h);
#endif /* NEW_WILLOWS */
     }

   if (LastFg != -1 && LastFg != fground)
     {
	/* eteint le foreground precedent */
	x = (LastFg % COLORS_COL) * wcase;
	y = ((LastFg / COLORS_COL) + 1) * hcase;
#ifndef NEW_WILLOWS
	w = wcase - 2;
	h = hcase - 2;
	XFillRectangle (TtDisplay, Color_Window, TtInvertGC, x + 2, y, w, 2);
	XFillRectangle (TtDisplay, Color_Window, TtInvertGC, x, y, 2, h);
	XFillRectangle (TtDisplay, Color_Window, TtInvertGC, x, y + h, w, 2);
	XFillRectangle (TtDisplay, Color_Window, TtInvertGC, x + w, y + 2, 2, h);
#endif /* NEW_WILLOWS */
     }

   /* nouveau background */
   if (LastBg != bground)
     {
	LastBg = bground;
	if (LastBg != -1)
	  {
	     x = (LastBg % COLORS_COL) * wcase;
	     y = ((LastBg / COLORS_COL) + 1) * hcase;
#ifndef NEW_WILLOWS
	     x -= 2;
	     y -= 2;
	     w = wcase + 2;
	     h = hcase + 2;
	     XFillRectangle (TtDisplay, Color_Window, TtInvertGC, x + 2, y, w, 2);
	     XFillRectangle (TtDisplay, Color_Window, TtInvertGC, x, y, 2, h);
	     XFillRectangle (TtDisplay, Color_Window, TtInvertGC, x, y + h, w, 2);
	     XFillRectangle (TtDisplay, Color_Window, TtInvertGC, x + w, y + 2, 2, h);
#endif /* NEW_WILLOWS */
	  }
     }

   /* nouveau foreground */
   if (LastFg != fground)
     {
	LastFg = fground;
	if (LastFg != -1)
	  {
	     x = (LastFg % COLORS_COL) * wcase;
	     y = ((LastFg / COLORS_COL) + 1) * hcase;
#ifndef NEW_WILLOWS
	     w = wcase - 2;
	     h = hcase - 2;
	     XFillRectangle (TtDisplay, Color_Window, TtInvertGC, x + 2, y, w, 2);
	     XFillRectangle (TtDisplay, Color_Window, TtInvertGC, x, y, 2, h);
	     XFillRectangle (TtDisplay, Color_Window, TtInvertGC, x, y + h, w, 2);
	     XFillRectangle (TtDisplay, Color_Window, TtInvertGC, x + w, y + 2, 2, h);
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

   if (TtWDepth == 1)
      /* Affiche le nom des couleurs sur un ecran N&B */
      wcase = CarWidth ('m', FontDialogue) * 12;
   else
      /* Affiche les couleurs sur un ecran couleur */
      wcase = CarWidth ('m', FontDialogue) * 4;

   w = wcase * COLORS_COL;
   hcase = FontHeight (FontDialogue);
   max = NumberOfColors ();
   y = hcase;
   h = (max + COLORS_COL - 1) / COLORS_COL * hcase + y;
#ifndef NEW_WILLOWS
   XClearWindow (TtDisplay, Color_Window);

   /* entree couleur standard */
   if (SmallFontDialogue == NULL)
      KeyboardsLoadResources ();
   XSetForeground (TtDisplay, TtLineGC, ColorPixel (0));
   XFillRectangle (TtDisplay, Color_Window, TtLineGC, 0, 0, w, hcase - 2);
   XSetForeground (TtDisplay, TtLineGC, ColorPixel (1));
   ptr = TtaGetMessage (LIB, STD_COLORS);
   WChaine (Color_Window, ptr,
   (w / 2) - (XTextWidth ((XFontStruct *) SmallFontDialogue, ptr, strlen (ptr)) / 2),
	    0, FontDialogue, TtLineGC);

   /* grille */
   XSetLineAttributes (TtDisplay, TtLineGC, 1, LineSolid, CapButt, JoinMiter);
   for (x = wcase; x < w; x += wcase)
      XDrawLine (TtDisplay, Color_Window, TtLineGC, x, y, x, h);
   for (y = hcase; y < h; y += hcase)
      XDrawLine (TtDisplay, Color_Window, TtLineGC, 0, y, w, y);

   /* items */
   if (TtWDepth == 1)
      for (i = 0; i < max; i++)
	{
	   x = (i % COLORS_COL) * wcase;
	   y = ((i / COLORS_COL) + 1) * hcase;
	   WChaine (Color_Window, ColorName (i),
		    x + (wcase / 2) - (XTextWidth ((XFontStruct *) SmallFontDialogue, ColorName (i), strlen (ColorName (i))) / 2),
		    y, SmallFontDialogue, TtLineGC);
	}
   else
      for (i = 0; i < max; i++)
	{
	   x = (i % COLORS_COL) * wcase;
	   y = ((i / COLORS_COL) + 1) * hcase;
	   XSetForeground (TtDisplay, TtLineGC, ColorPixel (i));
	   XFillRectangle (TtDisplay, Color_Window, TtLineGC, x + 1, y + 1, wcase - 2, hcase - 2);
	}

   /* show the current selection */
   fground = LastFg;
   bground = LastBg;
   LastFg = -1;
   LastBg = -1;
   ThotSelectPalette (bground, fground);
#endif /* NEW_WILLOWS */
}


/* ---------------------------------------------------------------------- */
/* |    ColorsPress traite un clic dans la palette des couleurs.        | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         ColorsPress (int button, int x, int y)
#else  /* __STDC__ */
static void         ColorsPress (button, x, y)
int                 button;
int                 x;
int                 y;

#endif /* __STDC__ */
{
   int                 color, li, co;
   int                 wcase, hcase;

   if (TtWDepth == 1)
      /* Affiche le nom des couleurs sur un ecran N&B */
      wcase = CarWidth ('m', FontDialogue) * 12;
   else
      /* Affiche les couleurs sur un ecran couleur */
      wcase = CarWidth ('m', FontDialogue) * 4;
   hcase = FontHeight (FontDialogue);

   /* Regarde si on n'a pas clique dans le titre */
   if (y < hcase)
     {
#ifdef NEW_WILLOWS
#define Button1 1		/* MSWindows will probably use same model */
#endif
	if (button == Button1)
	  {
	     /* couleur de trace' standard */
	     ModifyColor (-1, FALSE);
	     ThotSelectPalette (LastBg, -1);
	  }
	else
	  {
	     /* couleur de fond standard */
	     ModifyColor (-1, TRUE);
	     ThotSelectPalette (-1, LastFg);
	  }
	return;
     }

   li = x / wcase;
   co = (y - hcase) / hcase;
   color = co * COLORS_COL + li;
   if (button == Button1)
     {
	/* selectionne la couleur de trace' */
	ModifyColor (color, FALSE);
	ThotSelectPalette (LastBg, color);
     }
   else
     {
	/* selectionne la couleur de fond */
	ModifyColor (color, TRUE);
	ThotSelectPalette (color, LastFg);
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
   char                string[10];

   xfont = XmFontListCreate ((XFontStruct *) FontDialogue, XmSTRING_DEFAULT_CHARSET);
   if (SmallFontDialogue == NULL)
      SmallFontDialogue = ReadFont ('L', 'H', 0, 9, UnPoint);

   n = 0;
   sprintf (string, "+%d+%d", x, y);
   XtSetArg (args[n], XmNx, (Position) x);
   n++;
   XtSetArg (args[n], XmNy, (Position) y);
   n++;
   XtSetArg (args[n], XmNallowShellResize, TRUE);
   n++;
   XtSetArg (args[n], XmNuseAsyncGeometry, TRUE);
   n++;
   Color_Palette = XtCreatePopupShell (TtaGetMessage (LIB, COLORS),
			   applicationShellWidgetClass, RootShell, args, n);
/*** Cree la palette dans sa frame ***/
   n = 0;
   XtSetArg (args[n], XmNbackground, BgMenu_Color);
   n++;
   XtSetArg (args[n], XmNborderColor, Button_Color);
   n++;
   XtSetArg (args[n], XmNfontList, xfont);
   n++;
   title_string = XmStringCreateSimple (TtaGetMessage (LIB, COLORS));
   XtSetArg (args[n], XmNdialogTitle, title_string);
   n++;
   XtSetArg (args[n], XmNautoUnmanage, FALSE);
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
   XtSetArg (args[n], XmNadjustLast, FALSE);
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
   XtSetArg (args[n], XmNresizeHeight, TRUE);
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
   title_string = XmStringCreateSimple (TtaGetMessage (LIB, BUTTON_1));
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
   title_string = XmStringCreateSimple (TtaGetMessage (LIB, BUTTON_2));
   XtSetArg (args[n], XmNlabelString, title_string);
   n++;
   w = XmCreateLabel (row, "Thot_MSG", args, n);
   XtManageChild (w);
   XmStringFree (title_string);

   /* Evalue la largeur et la hauteur de la palette */
   n = 0;
   if (TtWDepth == 1)
      /* Affiche le nom des couleurs sur un ecran N&B */
      width = CarWidth ('m', FontDialogue) * 12 * COLORS_COL;
   else
      /* Affiche les couleurs sur un ecran couleur */
      width = CarWidth ('m', FontDialogue) * 4 * COLORS_COL;
   height = ((NumberOfColors () + COLORS_COL - 1) / COLORS_COL + 1) * FontHeight (FontDialogue);

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
   GCkey = XCreateGC (TtDisplay, TtRootWindow, GCForeground | GCBackground | GCFunction, &GCmodel);

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
   w = XmCreatePushButton (row, TtaGetMessage (LIB, DONE), args, n);
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
   LastBg = -1;
   LastFg = -1;
#endif /* NEW_WILLOWS */
}


/* ---------------------------------------------------------------------- */
/* |    TtcChangeColors L'utilisateur demande a modifier les            | */
/* |            couleurs (presentation specifique) pour la vue          | */
/* |            Vue du document pDoc (si Assoc = FALSE) ou les elements | */
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
   PtrDocument         pSelDoc;
   PtrElement          pFirstSel, pLastSel;
   int                 firstChar, lastChar;
   boolean             selectionOK;
   PtrAbstractBox             pAb;
   PtrDocument         pDoc;

#ifndef NEW_WILLOWS
   if (ThotLocalActions[T_colors] == NULL)
     {
	/* Connecte le traitement des evenements */
	TteConnectAction (T_colors, (Proc) ColorsEvent);
     }
#endif /* NEW_WILLOWS */

   pDoc = LoadedDocument[document - 1];
   /* demande quelle est la selection courante */
   selectionOK = GetCurrentSelection (&pSelDoc, &pFirstSel, &pLastSel, &firstChar, &lastChar);
   if (!selectionOK)
     {
	/* par defaut la racine du document */
	pSelDoc = pDoc;
	pFirstSel = pDoc->DocRootElement;
	selectionOK = TRUE;
     }

   if (selectionOK && pSelDoc == pDoc && NumberOfColors () > 0)
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
	   pAb = AbsBoxOfEl (pFirstSel, 1);
	else
	   pAb = AbsBoxOfEl (pFirstSel, view);

	if (pAb != NULL)
	   ThotSelectPalette (pAb->AbBackground, pAb->AbForeground);
     }
}
