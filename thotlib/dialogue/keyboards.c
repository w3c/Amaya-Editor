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
 * Latin, Greek, Math keyboards
 *
 * Authors: I. Vatton (INRIA)
 *          R. Guetari (W3C/INRIA) - Unicode and Windows version
 *
 */

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "constmenu.h"
#include "appdialogue.h"
#include "dialog.h"
#include "message.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "edit_tv.h"
#include "frame_tv.h"
#include "font_tv.h"
#include "select_tv.h"
#include "appdialogue_tv.h"

#define MAX_ARGS 20
#define MAX_KEYBOARD 4


typedef struct _item
  {
     char          name;
     unsigned char value;
     char*         legend;
  }
ITEM;


static ThotWidget   Keyboards[MAX_KEYBOARD];
static ptrfont      KbFonts[MAX_KEYBOARD];
static int          KbX = 800;
static int          KbY = 100;
static ThotGC       GCkey;

/* data for the keyboards */
/*iso */
static ITEM         Items_Grec[] =
{
   {'\40', '\40', NULL},
   {'\41', '\41', NULL},
   {'\42', '\42', NULL},
   {'\43', '\43', NULL},
   {'\44', '\44', NULL},
   {'\45', '\45', NULL},
   {'\46', '\46', NULL},
   {'\47', '\47', NULL},
   {'\50', '\50', NULL},
   {'\51', '\51', NULL},
   {'\52', '\52', NULL},
   {'\53', '\53', NULL},
   {'\54', '\54', NULL},
   {'\55', '\55', NULL},
   {'\56', '\56', NULL},
   {'\57', '\57', NULL},
   {'\60', '\60', NULL},
   {'\61', '\61', NULL},
   {'\62', '\62', NULL},
   {'\63', '\63', NULL},
   {'\64', '\64', NULL},
   {'\65', '\65', NULL},
   {'\66', '\66', NULL},
   {'\67', '\67', NULL},
   {'\70', '\70', NULL},
   {'\71', '\71', NULL},
   {'\72', '\72', NULL},
   {'\73', '\73', NULL},
   {'\74', '\74', NULL},
   {'\75', '\75', NULL},
   {'\76', '\76', NULL},
   {'\77', '\77', NULL},
   {'\100', '\100', NULL},
   {'A', 'A', NULL},
   {'B', 'B', NULL},
   {'C', 'C', NULL},
   {'D', 'D', NULL},
   {'E', 'E', NULL},
   {'F', 'F', NULL},
   {'G', 'G', NULL},
   {'H', 'H', NULL},
   {'I', 'I', NULL},
   {'J', 'J', NULL},
   {'K', 'K', NULL},
   {'L', 'L', NULL},
   {'M', 'M', NULL},
   {'N', 'N', NULL},
   {'O', 'O', NULL},
   {'P', 'P', NULL},
   {'Q', 'Q', NULL},
   {'R', 'R', NULL},
   {'S', 'S', NULL},
   {'T', 'T', NULL},
   {'U', 'U', NULL},
   {'V', 'V', NULL},
   {'W', 'W', NULL},
   {'X', 'X', NULL},
   {'Y', 'Y', NULL},
   {'Z', 'Z', NULL},
   {'\133', '\133', NULL},
   {'\134', '\134', NULL},
   {'\135', '\135', NULL},
   {'\136', '\136', NULL},
   {'\137', '\137', NULL},
   {'a', 'a', NULL},
   {'b', 'b', NULL},
   {'c', 'c', NULL},
   {'d', 'd', NULL},
   {'e', 'e', NULL},
   {'f', 'f', NULL},
   {'g', 'g', NULL},
   {'h', 'h', NULL},
   {'i', 'i', NULL},
   {'j', 'j', NULL},
   {'k', 'k', NULL},
   {'l', 'l', NULL},
   {'m', 'm', NULL},
   {'n', 'n', NULL},
   {'o', 'o', NULL},
   {'p', 'p', NULL},
   {'q', 'q', NULL},
   {'r', 'r', NULL},
   {'s', 's', NULL},
   {'t', 't', NULL},
   {'u', 'u', NULL},
   {'v', 'v', NULL},
   {'w', 'w', NULL},
   {'x', 'x', NULL},
   {'y', 'y', NULL},
   {'z', 'z', NULL},
   {'\173', '\173', NULL},
   {'\174', '\174', NULL},
   {'\175', '\175', NULL},
   {'\176', '\176', NULL},
   {'\241', '\241', NULL},
   {'\242', '\242', NULL},
   {'\243', '\243', NULL},
   {'\244', '\244', NULL},
   {'\245', '\245', NULL},
   {'\246', '\246', NULL},
   {'\247', '\247', NULL},
   {'\250', '\250', NULL},
   {'\251', '\251', NULL},
   {'\252', '\252', NULL},
   {'\253', '\253', NULL},
   {'\254', '\254', NULL},
   {'\255', '\255', NULL},
   {'\256', '\256', NULL},
   {'\257', '\257', NULL},
   {'\260', '\260', NULL},
   {'\261', '\261', NULL},
   {'\262', '\262', NULL},
   {'\263', '\263', NULL},
   {'\264', '\264', NULL},
   {'\265', '\265', NULL},
   {'\266', '\266', NULL},
   {'\267', '\267', NULL},
   {'\270', '\270', NULL},
   {'\271', '\271', NULL},
   {'\272', '\272', NULL},
   {'\273', '\273', NULL},
   {'\274', '\274', NULL},
   {'\275', '\275', NULL},
   {'\276', '\276', NULL},
   {'\277', '\277', NULL},
   {'\300', '\300', NULL},
   {'\301', '\301', NULL},
   {'\302', '\302', NULL},
   {'\303', '\303', NULL},
   {'\304', '\304', NULL},
   {'\305', '\305', NULL},
   {'\306', '\306', NULL},
   {'\307', '\307', NULL},
   {'\310', '\310', NULL},
   {'\311', '\311', NULL},
   {'\312', '\312', NULL},
   {'\313', '\313', NULL},
   {'\314', '\314', NULL},
   {'\315', '\315', NULL},
   {'\316', '\316', NULL},
   {'\317', '\317', NULL},
   {'\320', '\320', NULL},
   {'\321', '\321', NULL},
   {'\324', '\324', NULL},
   {'\325', '\325', NULL},
   {'\326', '\326', NULL},
   {'\327', '\327', NULL},
   {'\330', '\330', NULL},
   {'\331', '\331', NULL},
   {'\332', '\332', NULL},
   {'\333', '\333', NULL},
   {'\334', '\334', NULL},
   {'\335', '\335', NULL},
   {'\336', '\336', NULL},
   {'\337', '\337', NULL},
   {'\340', '\340', NULL},
   {'\341', '\341', NULL},
   {'\345', '\345', NULL},
   {'\361', '\361', NULL},
   {'\362', '\362', NULL}
};

/*iso */
static ITEM         Items_Isol[] =
{
   {'\230', '\230', ""},
   {'\231', '\231', ""},
   {'\240', '\240', "^SP"},
   {'\241', '\241', ""},
   {'\242', '\242', ""},
   {'\243', '\243', ""},
   {'\244', '\244', ""},
   {'\245', '\245', ""},
   {'\246', '\246', ""},
   {'\247', '\247', ""},
   {'\251', '\251', ""},
   {'\253', '\253', ""},
   {'\254', '\254', ""},
   {'\255', '\255', ""},
   {'\256', '\256', ""},
   {'\257', '\257', ""},
   {'\260', '\260', ""},
   {'\261', '\261', ""},
   {'\265', '\265', ""},
   {'\266', '\266', ""},
   {'\267', '\267', ""},
   {'\273', '\273', ""},
   {'\274', '\274', ""},
   {'\275', '\275', ""},
   {'\276', '\276', ""},
   {'\277', '\277', ""},
   {'\300', '\300', ""},
   {'\301', '\301', ""},
   {'\302', '\302', ""},
   {'\303', '\303', ""},
   {'\304', '\304', ""},
   {'\305', '\305', ""},
   {'\306', '\306', ""},
   {'\307', '\307', ""},
   {'\310', '\310', ""},
   {'\311', '\311', ""},
   {'\312', '\312', ""},
   {'\313', '\313', ""},
   {'\314', '\314', ""},
   {'\315', '\315', ""},
   {'\316', '\316', ""},
   {'\317', '\317', ""},
   {'\320', '\320', ""},
   {'\321', '\321', ""},
   {'\322', '\322', ""},
   {'\323', '\323', ""},
   {'\324', '\324', ""},
   {'\325', '\325', ""},
   {'\326', '\326', ""},
   {'\327', '\327', ""},
   {'\330', '\330', ""},
   {'\331', '\331', ""},
   {'\332', '\332', ""},
   {'\333', '\333', ""},
   {'\334', '\334', ""},
   {'\335', '\335', ""},
   {'\336', '\336', ""},
   {'\337', '\337', ""},
   {'\340', '\340', "[a]`"},
   {'\341', '\341', "[a]'"},
   {'\342', '\342', "[a]6"},
   {'\343', '\343', "[a]="},
   {'\344', '\344', "[a];"},
   {'\345', '\345', ""},
   {'\346', '\346', "[a]e"},
   {'\347', '\347', "[c],"},
   {'\350', '\350', "[e]`"},
   {'\351', '\351', "[e]'"},
   {'\352', '\352', "[e]6"},
   {'\353', '\353', "[e];"},
   {'\354', '\354', "[i]`"},
   {'\355', '\355', "[i]'"},
   {'\356', '\356', "[i]6"},
   {'\357', '\357', "[i];"},
   {'\360', '\360', ""},
   {'\361', '\361', "[n]="},
   {'\362', '\362', "[o]`"},
   {'\363', '\363', "[o]'"},
   {'\364', '\364', "[o]6"},
   {'\365', '\365', "[o]="},
   {'\366', '\366', "[o];"},
   {'\367', '\367', "[o]e"},
   {'\370', '\370', ""},
   {'\371', '\371', "[u]`"},
   {'\372', '\372', "[u]'"},
   {'\373', '\373', "[u]6"},
   {'\374', '\374', "[u];"},
   {'\375', '\375', ""},
   {'\376', '\376', ""},
   {'\377', '\377', ""},
   {'\212', '\212', "^CR"}
};


static ITEM         Items_Symb[] =
{
   {83, 'S', NULL},		/* sum */
   {80, 'P', NULL},		/* product */
   {85, 'U', NULL},		/* union */
   {73, 'I', NULL},		/* intersection */
   {105, 'i', NULL},		/* integral */
   {99, 'c', NULL},		/* circle integral */
   {100, 'd', NULL},		/* double integral */
   {114, 'r', NULL},		/* root */
   {60, '<', NULL},		/* < arrow */
   {62, '>', NULL},		/* > arrow */
   {118, 'V', NULL},		/* V arrow */
   {94, '^', NULL},		/* ^ arrow */
   {91, '[', NULL},		/* */
   {93, ']', NULL},		/* */
   {40, '(', NULL},		/* */
   {41, ')', NULL},		/* */
   {123, '{', NULL},		/* */
   {125, '}', NULL},		/* */
};


static ITEM         Items_Graph[] =
{
   {'C', 'C', NULL},		/* oval circle */
   {'L', 'L', NULL},		/* diamond */
   {'P', 'P', NULL},		/* crossed ellipse */
   {'Q', 'Q', NULL},		/* crossed oval circle */
   {'R', 'R', NULL},		/* rectangle */
   {'W', 'W', NULL},		/* up + right */   
   {'X', 'X', NULL},		/* down + right */
   {'Y', 'Y', NULL},		/* up + left */
   {'Z', 'Z', NULL},		/* down + left*/
   {'c', 'c', NULL},		/* ellipse */
   {'b', 'b', NULL},		/* - top  */
   {'h', 'h', NULL},		/* - middle */
   {'t', 't', NULL},		/* - bottom */
   {'l', 'l', NULL},		/* left  | */
   {'v', 'v', NULL},		/* centered | */
   {'r', 'r', NULL},		/* right | */
   {'/', '/', NULL},		/* / */
   {'\\', '\\', NULL},		/* \  */
   {'<', '<', NULL},		/* < arrow */
   {'>', '>', NULL},		/* > arrow */
   {'V', 'V', NULL},		/* V arrow */
   {'^', '^', NULL},		/* ^ arrow */
   {'E', 'E', NULL},		/* NE arrow */
   {'e', 'e', NULL},		/* SE arrow */
   {'O', 'O', NULL},		/* NW arrow */
   {'o', 'o', NULL},		/* SW arrow */
   {'S', 'S', NULL},		/* segments */
   {'U', 'U', NULL},		/* > segments */
   {'N', 'N', NULL},		/* < segments */
   {'M', 'M', NULL},		/* <> segments */
   {'p', 'p', NULL},		/* polygon */
   {'B', 'B', NULL},		/* beziers */
   {'F', 'F', NULL},		/* > beziers */
   {'A', 'A', NULL},		/* < beziers */
   {'D', 'D', NULL},		/* <> beziers */
   {'s', 's', NULL}		/* closed bezier */
};

#include "actions_f.h"
#include "appdialogue_f.h"
#include "applicationapi_f.h"
#include "config_f.h"
#include "editcommands_f.h"
#include "font_f.h"
#include "structselect_f.h"
#include "textcommands_f.h"
#include "windowdisplay_f.h"

#ifdef _WINDOWS
#ifdef __STDC__
extern void CreateGreekKeyboardDlgWindow (HWND);
#else /* __STDC__ */
extern void CreateGreekKeyboardDlgWindow ();
#endif /* __STDC__ */
#endif /* _WINDOWS */

/*----------------------------------------------------------------------
   WChar
   displays character ch at position <x,y> of window w using the character
   policy font.
   Function func indicates if it's an active box (1) or not (0).
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         WChar (ThotWindow w, CHAR_T ch, int x, int y, int func, ptrfont font, int disp, ThotGC GClocal)
#else  /* __STDC__ */
static void         WChar (w, ch, x, y, func, font, disp, GClocal)
ThotWindow          w;
CHAR_T                ch;
int                 x;
int                 y;
int                 func;
ptrfont             font;
int                 disp;
ThotGC              GClocal;

#endif /* __STDC__ */
{
#ifndef _GTK
   int                 length;

   length = 1;
#ifdef _WINDOWS
   /* DrawTextEx or some such thing - @@@ */
#else
   XSetFont (TtDisplay, GClocal, ((XFontStruct *) font)->fid);
   FontOrig (font, ch, &x, &y);
   XDrawString (TtDisplay, w, GClocal, x, y, &ch, length);
#endif
#endif /* _GTK */
}

/*----------------------------------------------------------------------
   KbdEndDisplay
   Ends the display of a keyboard.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         KbdEndDisplay (ThotWidget w, int index, caddr_t call_d)

#else  /* __STDC__ */
static void         KbdEndDisplay (w, index, call_d)
ThotWidget          w;
int                 index;
caddr_t             call_d;

#endif /* __STDC__ */
{
#ifndef _GTK
#ifndef _WINDOWS
   XtPopdown (Keyboards[index]);
#endif /* _WINDOWS */
#endif /* _GTK */
}

/*----------------------------------------------------------------------
   KbdCallbackHandler
   handles the keyboard keys.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void         KbdCallbackHandler (ThotWidget w, int param, caddr_t call_d)
#else  /* __STDC__ */
void         KbdCallbackHandler (w, param, call_d)
ThotWidget          w;
int                 param;
caddr_t             call_d;

#endif /* __STDC__ */
{
#ifndef _GTK
   UCHAR_T       car;
   ThotWidget          wp;
   int                 i;

   /* Recupere la table des items */
   car = (UCHAR_T) param % 256;
   /* Recupere le widget de la palette */
#  ifndef _WINDOWS
   wp=XtParent(XtParent(XtParent(XtParent(w))));
#  else  /* _WINDOWS */
   wp = GetParent (GetParent (GetParent (GetParent (w))));
#  endif /* _WINDOWS */
   /* met a jour l'indicateur de palette */
   if(Keyboards[KeyboardMode] != wp)
     {
     CloseTextInsertion();
     for(i = 0; i < (MAX_KEYBOARD + 1); i++)
       if(Keyboards[i] == wp)
         KeyboardMode = i;
     }
   /* Insere le caractere selectionne */
   if (ThotLocalActions[T_insertchar] != NULL)
      (*ThotLocalActions[T_insertchar]) (ActiveFrame, car, KeyboardMode);
#endif /* _GTK */
}


#ifndef _WINDOWS
/*----------------------------------------------------------------------
   ExposeKbd
   displays the keyboard keys
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ExposeKbd (ThotWidget w, int param, XmDrawnButtonCallbackStruct * infos)
#else  /* __STDC__ */
static void         ExposeKbd (w, param, infos)
ThotWidget          w;
int                 param;
XmDrawnButtonCallbackStruct *infos;

#endif /* __STDC__ */
{
#ifndef _GTK
   int                 y;
   int                 i, kb;
   ITEM               *it;

   /* Recupere la table des items */
   kb = param / 256;
   switch (kb)
	 {
	    case 0:
	       it = Items_Symb;
	       break;
	    case 1:
	       it = Items_Graph;
	       break;
	    case 2:
	       it = Items_Isol;
	       break;
	    case 4:
	       it = Items_Grec;
	       break;
	    default:
	       return;
	 }
   y = 4;
   i = param % 256;		/* indice dans la table des items */
   it = (ITEM *) ((int) it + (sizeof (ITEM) * i));
   WChar (infos->window, it->name, CharacterWidth (87, FontDialogue), y, GXcopy, KbFonts[kb], 0, GCkey);
   if (it->legend)
     {
	y = FontHeight (KbFonts[kb]);
	WChaine (infos->window, it->legend, 4, y, FontDialogue, GCkey);
     }
#endif /* _GTK */
}


/*----------------------------------------------------------------------
   CreateKeyboard
   creates a keyboard.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         CreateKeyboard (int number, STRING title, ptrfont pFont, int col, int x, int y, ITEM * items, int nbitem)
#else  /* __STDC__ */
static void         CreateKeyboard (number, title, font, col, x, y, items, nbitem)
int                 number;
STRING              title;
ptrfont             pFont;
int                 col;
int                 x;
int                 y;
ITEM               *items;
int                 nbitem;

#endif /* __STDC__ */
{
#ifndef _GTK
   int                 n;
   int                 i;
   register ITEM      *it;
   ThotWidget          w;
   ThotWidget          row;
   Arg                 args[MAX_ARGS];
   XmString            title_string;
   XmFontList          xfont;
   CHAR_T                string[10];
   int                 param;
   XGCValues           GCmodel;

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
#ifdef OLD
   w = XtCreateWidget (title, topLevelShellWidgetClass, XtParent (FrameTable[0].WdFrame), args, n);
#else
   w = XtCreatePopupShell (title, applicationShellWidgetClass, RootShell, args, n);
#endif
/*** Cree le clavier dans sa frame ***/
   n = 0;
   XtSetArg (args[n], XmNbackground, BgMenu_Color);
   n++;
   XtSetArg (args[n], XmNborderColor, BgMenu_Color);
   n++;
   xfont = XmFontListCreate ((XFontStruct *) FontDialogue, XmSTRING_DEFAULT_CHARSET);
   XtSetArg (args[n], XmNfontList, xfont);
   n++;
   title_string = XmStringCreateSimple (title);
   XtSetArg (args[n], XmNdialogTitle, title_string);
   n++;
   XtSetArg (args[n], XmNautoUnmanage, FALSE);
   n++;
/**XtSetArg(args[n], XmNdefaultPosition, FALSE); n++;**/
   XtSetArg (args[n], XmNmarginWidth, 0);
   n++;
   XtSetArg (args[n], XmNmarginHeight, 0);
   n++;
   XtSetArg (args[n], XmNspacing, 0);
   n++;
   Keyboards[number] = w;
   w = XmCreateBulletinBoard (w, "Dialogue", args, n);
   XtManageChild (w);
   XmStringFree (title_string);

/*** Cree un Row-Column pour mettre le bouton Quit ***/
/*** en dessous des touches du clavier.    ***/
   n = 3;
   XtSetArg (args[n], XmNadjustLast, FALSE);
   n++;
   XtSetArg (args[n], XmNmarginWidth, 0);
   n++;
   XtSetArg (args[n], XmNmarginHeight, 0);
   n++;
/*SN */ XtSetArg (args[n], XmNpacking, XmPACK_TIGHT);
   n++;
   XtSetArg (args[n], XmNspacing, 0);
   n++;
   w = XmCreateRowColumn (w, "Dialogue", args, n);
   XtManageChild (w);

/*** Cree un Row-Column pour contenir les touches du clavier ***/
   n = 3;
   XtSetArg (args[n], XmNadjustLast, FALSE);
   n++;
   XtSetArg (args[n], XmNmarginWidth, 0);
   n++;
   XtSetArg (args[n], XmNmarginHeight, 0);
   n++;
   XtSetArg (args[n], XmNspacing, 0);
   n++;
   XtSetArg (args[n], XmNpacking, XmPACK_COLUMN);
   n++;
   XtSetArg (args[n], XmNnumColumns, col);
   n++;
   XtSetArg (args[n], XmNorientation, XmHORIZONTAL);
   n++;
   row = XmCreateRowColumn (w, "Dialogue", args, n);
   XtManageChild (row);

/*** Cree un Row-Column pour contenir le bouton Quit ***/
   n = 3;
   XtSetArg (args[n], XmNorientation, XmHORIZONTAL);
   n++;
   XtSetArg (args[n], XmNmarginWidth, 60);
   n++;
   XtSetArg (args[n], XmNmarginHeight, 0);
   n++;
   w = XmCreateRowColumn (w, "Dialogue", args, n);
   XtManageChild (w);

   GCmodel.function = GXcopy;
   GCmodel.foreground = FgMenu_Color;
   GCmodel.background = BgMenu_Color;
   GCkey = XCreateGC (TtDisplay, TtRootWindow, GCForeground | GCBackground | GCFunction, &GCmodel);

/*** Cree le bouton Quit ***/
   n = 0;
   XtSetArg (args[n], XmNbackground, BgMenu_Color);
   n++;
   XtSetArg (args[n], XmNbottomShadowColor, BgMenu_Color);
   n++;
   XtSetArg (args[n], XmNforeground, FgMenu_Color);
   n++;
   XtSetArg (args[n], XmNfontList, xfont);
   n++;
   w = XmCreatePushButton (w, TtaGetMessage (LIB, TMSG_CANCEL), args, n);
   XtManageChild (w);
   XtAddCallback (w, XmNactivateCallback, (XtCallbackProc) KbdEndDisplay, (XtPointer) number);
   XmFontListFree (xfont);

   /* Definit le bouton d'annulation comme bouton par defaut */
   n = 0;
   XtSetArg (args[n], XmNdefaultButton, w);
   n++;
   XtSetValues (Keyboards[number], args, n);

   n = 0;
   XtSetArg (args[n], XmNbackground, BgMenu_Color);
   n++;
   XtSetArg (args[n], XmNforeground, FgMenu_Color);
   n++;
   XtSetArg (args[n], XmNborderColor, BgMenu_Color);
   n++;

   /* Affiche les differents boutons du clavier */
   it = items;
   /* Prepare les parametres des procedures KbdCallbackHandler et ExposeKbd */
   param = number * 256;	/* indice du clavier */

   if (it->legend == 0)
     {
	/* Un clavier sans legende */
	XtSetArg (args[n], XmNmarginWidth, 4);
	n++;
	XtSetArg (args[n], XmNmarginHeight, 4);
	n++;
	xfont = XmFontListCreate ((XFontStruct *) pFont, XmSTRING_DEFAULT_CHARSET);
	XtSetArg (args[n], XmNfontList, xfont);
	n++;
	for (i = 0; i < nbitem; i++, it++)
	  {
	     string[0] = it->name;
	     string[1] = EOS;
	     w = XmCreatePushButton (row, string, args, n);
	     XtManageChild (w);
	     XtAddCallback (w, XmNactivateCallback, (XtCallbackProc) KbdCallbackHandler, (XtPointer) (param + (int) (it->value)));
	  }			/*for */
	XmFontListFree (xfont);
     }
   else
     {
	/* Un clavier avec legende */
	XtSetArg (args[n], XmNmarginWidth, 0);
	n++;
	XtSetArg (args[n], XmNmarginHeight, 0);
	n++;
	XtSetArg (args[n], XmNwidth, (Dimension) CharacterWidth (87, FontDialogue) * 3);
	n++;
	XtSetArg (args[n], XmNheight, (Dimension) FontHeight (pFont) + FontHeight (FontDialogue) + 4);
	n++;

	for (i = 0; i < nbitem; i++, it++)
	  {
	     string[0] = it->name;
	     string[1] = '\n';
	     w = XmCreateDrawnButton (row, "", args, n);
	     XtManageChild (w);
	     XtAddCallback (w, XmNactivateCallback, (XtCallbackProc) KbdCallbackHandler, (XtPointer) (param + (int) (it->value)));
	     XtAddCallback (w, XmNexposeCallback, (XtCallbackProc) ExposeKbd, (XtPointer) (param + i));
	  }
     }
#endif /* _GTK */
}


/*----------------------------------------------------------------------
   LoadKbd
   loads a keyboard.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         LoadKbd (int number)
#else  /* __STDC__ */
static void         LoadKbd (number)
int                 number;
#endif /* __STDC__ */
{
#ifndef _GTK
  ptrfont             pFontAc;
  ptrfont             pFontIg;

  ConfigKeyboard (&KbX, &KbY);
  switch (number)
    {
    case 0:		/* Symboles */
      if (SymbolIcons != NULL)
	{
	  KbFonts[number] = SymbolIcons;
	  CreateKeyboard (number, TtaGetMessage (LIB, TMSG_MATH_SYMBOLS),
			  SymbolIcons, 3, KbX, KbY, Items_Symb,
			  sizeof (Items_Symb) / sizeof (ITEM));
	}
      break;
    case 1:		/* Graphiques */
      if (GraphicsIcons != NULL)
	{
	  KbFonts[number] = GraphicsIcons;
	  CreateKeyboard (number, TtaGetMessage (LIB, TMSG_GRAPHICS),
			  GraphicsIcons, 6,
			  KbX, KbY, Items_Graph,
			  sizeof (Items_Graph) / sizeof (ITEM));
	}
      break;
    case 2:		/* ISO latin 1 */
      pFontAc = ReadFont ('L', 'T', 0, 14, UnPoint);
      if (!pFontAc)
	pFontAc = FontDialogue;
      KbFonts[number] = pFontAc;
      if (pFontAc != NULL)
	CreateKeyboard (number, TtaGetMessage (LIB, TMSG_LATIN_ALPHABET), pFontAc, 13,
			KbX, KbY, Items_Isol, sizeof (Items_Isol) / sizeof (ITEM));
      break;
    case 3:		/* Grec */
      pFontIg = ReadFont ('G', 'T', 0, 14, UnPoint);
      if (!pFontIg)
	pFontIg = FontDialogue;
      if (pFontIg != NULL)
	KbFonts[number] = pFontIg;	/* Grec */
      CreateKeyboard (number, TtaGetMessage (LIB, TMSG_GREEK_ALPHABET), pFontIg, 16,
		      KbX, KbY, Items_Grec, sizeof (Items_Grec) / sizeof (ITEM));
      break;
    }
#endif /* _GTK */
}
#endif /* _WINDOWS */


/*----------------------------------------------------------------------
   KeyboardMap
   maps a keyboard.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                KeyboardMap (int kb)
#else  /* __STDC__ */
void                KeyboardMap (kb)
int                 kb;

#endif /* __STDC__ */
{
#ifndef _GTK
#ifndef _WINDOWS
   if (kb >= 0 && kb < MAX_KEYBOARD)
     {
	/* Faut-il charger le clavier avant de l'afficher ? */
	if (Keyboards[kb] == 0)
	   LoadKbd (kb);
	if (Keyboards[kb] != 0)
	  {
	    XtPopup (Keyboards[kb], XtGrabNonexclusive);
	    XMapRaised (TtDisplay, XtWindowOfObject (Keyboards[kb]));
	  }
     }
#endif /* _WINDOWS */
#endif /* _GTK */
}

/*----------------------------------------------------------------------
  GraphicsLoadResource
  Initializes the keyboards.
  ----------------------------------------------------------------------*/
void                GraphicsLoadResources ()
{
#ifndef _GTK
   int                 i;

   if (ThotLocalActions[T_keyboard] == NULL)
     {
	TteConnectAction (T_keyboard, (Proc) KeyboardMap);

	/* Initialise la table des claviers */
	for (i = 0; i < MAX_KEYBOARD; i++)
	   Keyboards[i] = 0;
	SymbolIcons = NULL;

	if (SmallFontDialogue == NULL)
	   SmallFontDialogue = ReadFont ('L', 'H', 0, 9, UnPoint);
	if (SmallFontDialogue == NULL)
	   SmallFontDialogue = FontDialogue;
     }

#   ifndef _WINDOWS
   GraphicsIcons = LoadFont ("ivgraf", 0);
#   endif /* _WINDOWS */
   if (GraphicsIcons == NULL)
     {
       /*Fonte 'ivgraf' inaccessible */
       TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_LIB_MISSING_FILE), "ivgraf");
       GraphicsIcons = FontDialogue;
     }
#endif /* _GTK */
}

/*----------------------------------------------------------------------
  KeyboardsLoadResource
  Initializes the keyboards.
  ----------------------------------------------------------------------*/
void                KeyboardsLoadResources ()
{
#ifndef _GTK
   int                 i;

   if (ThotLocalActions[T_keyboard] == NULL)
     {
	TteConnectAction (T_keyboard, (Proc) KeyboardMap);

	/* Initialise la table des claviers */
	for (i = 0; i < MAX_KEYBOARD; i++)
	   Keyboards[i] = 0;
	GraphicsIcons = NULL;

	if (SmallFontDialogue == NULL)
	   SmallFontDialogue = ReadFont ('L', 'H', 0, 9, UnPoint);
	if (SmallFontDialogue == NULL)
	   SmallFontDialogue = FontDialogue;
     }

#   ifndef _WINDOWS
    SymbolIcons = LoadFont ("ivsymb", 0);
    if (SymbolIcons == NULL) {
       /*Fonte 'ivsymb' inaccessible */
       TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_LIB_MISSING_FILE), "ivsymb");
       SymbolIcons = FontDialogue;
	} 
#   endif /* _WINDOWS */

#endif /* _GTK */
}


/*----------------------------------------------------------------------
   TtcDisplayMathKeyboard
   displays the math keyboard
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcDisplayMathKeyboard (Document document, View view)

#else  /* __STDC__ */
void                TtcDisplayMathKeyboard (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
#ifndef _GTK
   KeyboardsLoadResources ();
   /* Enregistre la position pour le dialogue */
#  ifndef _WINDOWS
   TtaSetDialoguePosition ();
#  endif /* !_WINDOWS */
   TtaSetCurrentKeyboard (0);
#endif /* _GTK */
}


/*----------------------------------------------------------------------
   TtcDisplayGraphicsKeyboard
   displays the graphics keyboard
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcDisplayGraphicsKeyboard (Document document, View view)

#else  /* __STDC__ */
void                TtcDisplayGraphicsKeyboard (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
#ifndef _GTK
   KeyboardsLoadResources ();
   /* Enregistre la position pour le dialogue */
#  ifndef _WINDOWS
   TtaSetDialoguePosition ();
#  endif /* !_WINDOWS */
   TtaSetCurrentKeyboard (1);
#endif /* _GTK */
}


/*----------------------------------------------------------------------
   TtcDisplayLatinKeyboard
   displays the latin keyboard
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcDisplayLatinKeyboard (Document document, View view)

#else  /* __STDC__ */
void                TtcDisplayLatinKeyboard (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
#ifndef _GTK
   KeyboardsLoadResources ();
   /* Enregistre la position pour le dialogue */
#  ifndef _WINDOWS 
   TtaSetDialoguePosition ();
#  endif /* !_WINDOWS */
   TtaSetCurrentKeyboard (2);
#endif /* _GTK */
}

/*----------------------------------------------------------------------
   TtcDisplayGreekKeyboard
   displays the greek keyboard 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcDisplayGreekKeyboard (Document document, View view)

#else  /* __STDC__ */
void                TtcDisplayGreekKeyboard (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
#ifndef _GTK
   KeyboardsLoadResources ();
   /* Enregistre la position pour le dialogue */
#  ifndef _WINDOWS 
   TtaSetDialoguePosition ();
#  endif /* !_WINDOWS */
   TtaSetCurrentKeyboard (3);
#  ifdef _WINDOWS
   CreateGreekKeyboardDlgWindow (NULL);
#  endif /* _WINDOWS */
#endif /* _GTK */
}
