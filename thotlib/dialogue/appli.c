/*=======================================================================*/
/*|                                                                     | */
/*|     Module de gestion des frames d'application.                     | */
/*|                                                                     | */
/*=======================================================================*/

#include "thot_gui.h"
#include "thot_sys.h"
#include "thot_key.h"
#include "constmedia.h"

#include "constmenu.h"

#include "typemedia.h"
#include "message.h"
#include "dialog.h"
#include "application.h"
#include "interface.h"
#include "document.h"
#include "view.h"
#include "appdialogue.h"

#define MAX_ARGS 20

/* Declarations des variables */
#undef EXPORT
#define EXPORT extern
#include "boxes_tv.h"
#include "font_tv.h"
#include "edit_tv.h"
#include "frame_tv.h"
#include "appdialogue_tv.h"

#ifndef NEW_WILLOWS
static XmString     null_string;

#endif

#include "appli_f.h"
#include "textcommands_f.h"
#include "editcommands_f.h"
#include "context_f.h"
#include "structcreation_f.h"
#include "scroll_f.h"
#include "boxlocate_f.h"
#include "views_f.h"
#include "callback_f.h"
#include "viewcommands_f.h"
#include "windowdisplay_f.h"
#include "font_f.h"
#include "absboxes_f.h"
#include "buildboxes_f.h"
#include "input_f.h"
#include "keyboards_f.h"
#include "memory_f.h"
#include "message_f.h"
#include "structmodif_f.h"
#include "structcommands_f.h"
#include "boxparams_f.h"
#include "boxselection_f.h"
#include "structselect_f.h"
#include "selectmenu_f.h"
#include "thotmsg_f.h"

#ifdef __STDC__
extern void         EndInsert (void);
extern void         DefRegion (int, int, int, int, int);
extern boolean      RedrawFrameBottom (int, int);
extern PtrAbstractBox      GetClickedAbsBox (int, int, int);

#else
extern void         EndInsert ();
extern void         DefRegion ();
extern boolean      RedrawFrameBottom ();
extern PtrAbstractBox      GetClickedAbsBox ();

#endif

/* ---------------------------------------------------------------------- */
/* |    VueDeFenetre retourne, sous la forme qui convient a l'API Thot, | */
/* |            les parametres identifiant le document et la vue        | */
/* |            qui correspondent a une frame donnee.                   | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                VueDeFenetre (int frame, int *doc, int *view)
#else  /* __STDC__ */
void                VueDeFenetre (frame, doc, view)
int                 frame;
int                *doc;
int                *view;

#endif /* __STDC__ */
{
   int                 i;
   PtrDocument         pDoc;
   boolean             assoc;

   *doc = FrameTable[frame].FrDoc;
   *view = 0;
   if (doc == 0)
      return;
   else
     {
	pDoc = LoadedDocument[*doc - 1];
	*view = 0;
	if (pDoc != NULL)
	  {
	     GetViewFromFrame (frame, pDoc, &i, &assoc);
	     if (assoc)
		*view = i + 100;
	     else
		*view = i;
	  }
     }
}


/* -------------------------------------------------------------------- */
/* | Evenement sur une frame document.                              | */
/* -------------------------------------------------------------------- */
#ifdef __STDC__
void                RetourKill (int *w, int frame, int *info)

#else  /* __STDC__ */
void                RetourKill (w, frame, info)
int                *w;
int                 frame;
int                *info;

#endif /* __STDC__ */

{
   /* Enleve la procedure de Callback */
   /* Detruit la fenetre si elle existe encore */
   if (FrRef[frame] != 0 && frame > 0)
      ViewClosed (frame);
}


#ifdef NEW_WILLOWS
/* ---------------------------------------------------------------------- */
/* |  WIN_HandleExpose deal with the redrawing of the Client Area when  | */
/* |            a WM_PAINT has been received in MS-Windows.             | */
/* ---------------------------------------------------------------------- */
void                WIN_HandleExpose (ThotWindow w, int frame, WPARAM wParam, LPARAM lParam)
{
   PAINTSTRUCT         ps;

   if (frame > 0 && frame <= MAX_FRAME)
     {
	/*
	 * Do not redraw if the document is in NoComputedDisplay mode.
	 */
	if (documentDisplayMode[FrameTable[frame].FrDoc - 1] != NoComputedDisplay)
	  {
	     WIN_curHdc = BeginPaint (w, &ps);
	     DefRegion (frame, ps.rcPaint.left, ps.rcPaint.top,
			ps.rcPaint.right, ps.rcPaint.bottom);
	     SwitchSelection (frame, FALSE);
	     RedrawFrameBottom (frame, 0);
	     SwitchSelection (frame, TRUE);
	     EndPaint (w, &ps);
	  }
     }
}
#endif /* NEW_WILLOWS */

#ifndef NEW_WILLOWS
/* ---------------------------------------------------------------------- */
/* |    TraiteExpose effectue le traitement des expositions X11 des     | */
/* |            frames de dialogue et de documents.                   | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TraiteExpose (ThotWindow w, int frame, XExposeEvent * event)
#else  /* __STDC__ */
void                TraiteExpose (w, frame, event)
ThotWindow          w;
int                 frame;
XExposeEvent       *event;

#endif /* __STDC__ */
{
   int                 x;
   int                 y;
   int                 l;
   int                 h;

   x = event->x;
   y = event->y;
   l = event->width;
   h = event->height;

   if (frame > 0 && frame <= MAX_FRAME)
     {
	/* ne pas traiter si le document est en mode NoComputedDisplay */
	if (documentDisplayMode[FrameTable[frame].FrDoc - 1] != NoComputedDisplay)
	  {
	     DefRegion (frame, x, y, x + l, y + h);
	     SwitchSelection (frame, FALSE);
	     RedrawFrameBottom (frame, 0);
	     SwitchSelection (frame, TRUE);
	  }
     }
}
#endif /* !NEW_WILLOWS */

#ifdef NEW_WILLOWS
/* -------------------------------------------------------------------- */
/* | MSChangeTaille : function called when a view is resized under    | */
/* |    MS-Windows.                                                   | */
/* -------------------------------------------------------------------- */
void                MSChangeTaille (int frame, int width, int height, int top_delta, int bottom_delta)
{
   int                 n, dx, dy, view;
   Document            doc;

   if ((width <= 0) || (height <= 0))
      return;

   if (documentDisplayMode[FrameTable[frame].FrDoc - 1] == NoComputedDisplay)
      return;

   VueDeFenetre (frame, &doc, &vview);
   FrameTable[frame].TopMargin = top_delta;
   FrameTable[frame].LeftMargin = 0;
   FrameTable[frame].FrWidth = (int) width - bottom_delta;
   FrameTable[frame].FrHeight = (int) height;

   /* need to recompute the content of the window */
   RebuildConcreteImage (frame);

   /* recompute the scroll bars
      MajScrolls(frame); */
}
#endif /* NEW_WILLOWS */


#ifndef NEW_WILLOWS
/* -------------------------------------------------------------------- */
/* | Evenement sur une frame document.                              | */
/* -------------------------------------------------------------------- */
#ifdef __STDC__
void                XChangeTaille (int *w, int frame, int *info)
#else  /* __STDC__ */
void                XChangeTaille (w, frame, info)
int                *w;
int                 frame;
int                *info;

#endif /* __STDC__ */
{
   int                 n, dx, dy, view;
   Dimension           width, height;
   Arg                 args[MAX_ARGS];
   NotifyWindow        notifyDoc;
   Document            doc;

   n = 0;
   XtSetArg (args[n], XmNwidth, &width);
   n++;
   XtSetArg (args[n], XmNheight, &height);
   n++;
   XtGetValues ((ThotWidget) w, args, n);

   if ((width > 0) && (height > 0)
   /* ne pas traiter si le document est en mode NoComputedDisplay */
   && documentDisplayMode[FrameTable[frame].FrDoc - 1] != NoComputedDisplay)
     {
	notifyDoc.event = TteViewResize;
	VueDeFenetre (frame, &doc, &view);
	notifyDoc.document = doc;
	notifyDoc.view = view;
	dx = width - FrameTable[frame].FrWidth;
	dy = height - FrameTable[frame].FrHeight;
	notifyDoc.verticalValue = dy;
	notifyDoc.horizontalValue = dx;
	if (!CallEventType ((NotifyEvent *) & notifyDoc, TRUE))
	  {
	     FrameTable[frame].FrWidth = (int) width;
	     FrameTable[frame].FrHeight = (int) height;

	     /* Il faut reevaluer le contenu de la fenetre */
	     RebuildConcreteImage (frame);

	     /* Reevalue les ascenseurs */
	     MajScrolls (frame);
	     notifyDoc.event = TteViewResize;
	     notifyDoc.document = doc;
	     notifyDoc.view = view;
	     notifyDoc.verticalValue = dy;
	     notifyDoc.horizontalValue = dx;
	     CallEventType ((NotifyEvent *) & notifyDoc, FALSE);
#ifdef IV
	     /* Met a jour les widgets de texte */
	     i = 1;
	     n = 0;
	     /*XtSetArg(args[n], XmNeditable, TRUE); n++; */
	     XtSetArg (args[n], XmNwidth, (Dimension) (width - 100));
	     n++;
	     while (i < MAX_TEXTZONE && FrameTable[frame].Text_Zone[i] != 0)
	       {
		  /* recupere l'indicateur editable
		     XtSetArg(argument[0], XmNeditable, &editable);
		     XtGetValues(FrameTable[frame].Text_Zone[i], argument, 1); */
		  /* retire l'indicateur non-editable pour mettre a jour la taille */
		  XtSetValues (FrameTable[frame].Text_Zone[i], args, n);
	    /***
	    if (!editable)
	      {
		XtSetArg(argument[0], XmNeditable, editable);
		XtSetValues(FrameTable[frame].Text_Zone[i], argument, 1);
	      }
	    ***/
		  i++;
	       }
#endif
	  }
     }
}				/*XChangeTaille */

#endif /* !NEW_WILLOWS */

#ifdef NEW_WILLOWS
/* -------------------------------------------------------------------- */
/* | Demande de scroll vertical.                                      | */
/* -------------------------------------------------------------------- */
#ifdef __STDC__
void                MSChangeVScroll (int frame, int reason, int value)
#else  /* __STDC__ */
void                MSChangeVScroll (frame, reason, value)
int                 frame;
int                 reason;
int                 value;

#endif /* __STDC__ */

{
   int                 delta;
   int                 n;
   int                 h, y;
   int                 start, end, total;
   float               charperpix;
   Document            doc;

   /* do not redraw it if in NoComputedDisplay mode */
   if (documentDisplayMode[FrameTable[frame].FrDoc - 1] == NoComputedDisplay)
      return;

   WIN_GetDeviceContext (frame);

   switch (reason)
	 {
	    case SB_TOP:
	       JumpIntoView (frame, 0);
	       break;
	    case SB_BOTTOM:
	       JumpIntoView (frame, 100);
	       break;
	    case SB_LINEUP:
	       delta = -13;
	       DefFenV (frame, delta, TRUE);
	       break;
	    case SB_LINEDOWN:
	       delta = 13;
	       DefFenV (frame, delta, TRUE);
	       break;
	    case SB_PAGEUP:
	       delta = -FrameTable[frame].FrHeight;
	       DefFenV (frame, delta, TRUE);
	       break;
	    case SB_PAGEDOWN:
	       delta = FrameTable[frame].FrHeight;
	       DefFenV (frame, delta, TRUE);
	       break;
	/*************************
	case SB_THUMBPOSITION:
	    fprintf(stderr,"SB_THUMBPOSITION\n");
	    JumpIntoView(frame, value);
	    break;
	 *************************/
	    case SB_THUMBTRACK:
	       JumpIntoView (frame, value);
	       break;
	 }

   /*
    * get some information on the position of the displayed part
    * for this document.
    */

   n = ZoneImageAbs (frame, &start, &end, &total);
   switch (n)
	 {
	    case -1:
	       /* No abstract Picture, strange */
	       SetScrollPos (WIN_curHdc, SB_VERT, 0, TRUE);
	       break;
	    case 0:
	       /* Abstract Picture fully shown */
	    case 1:
	       /* Abstract Picture at the top */
	       SetScrollPos (WIN_curHdc, SB_VERT, 0, TRUE);
	       break;
	    case 2:
	       /* Abstract Picture at the end */
	       SetScrollPos (WIN_curHdc, SB_VERT, 100, TRUE);
	       break;
	    case 3:
	       /* Abstract Picture at the end */
	       SetScrollPos (WIN_curHdc, SB_VERT, (100 * total) / start, TRUE);
	       break;
	 }
}
#endif /* NEW_WILLOWS */

#ifndef NEW_WILLOWS
/* -------------------------------------------------------------------- */
/* | Demande de scroll horizontal.                                    | */
/* -------------------------------------------------------------------- */
#ifdef __STDC__
void                XChangeHScroll (int *w, int frame, int *param)
#else  /* __STDC__ */
void                XChangeHScroll (w, frame, param)
int                *w;
int                 frame;
int                *param;

#endif /* __STDC__ */
{
   int                 delta, l;
   int                 n, view;
   Arg                 args[MAX_ARGS];
   NotifyWindow        notifyDoc;
   Document            doc;
   XmScrollBarCallbackStruct *info;

   info = (XmScrollBarCallbackStruct *) param;
   /* ne pas traiter si le document est en mode NoComputedDisplay */
   if (documentDisplayMode[FrameTable[frame].FrDoc - 1] == NoComputedDisplay)
      return;

   if (info->reason == XmCR_DECREMENT)
      /* Deplacement en arriere d'un caractere de la fenetre */
      delta = -13;
   else if (info->reason == XmCR_INCREMENT)
      /* Deplacement en avant d'un caractere de la fenetre */
      delta = 13;
   else if (info->reason == XmCR_PAGE_DECREMENT)
      /* Deplacement en arriere du volume de la fenetre */
      delta = -FrameTable[frame].FrWidth;
   else if (info->reason == XmCR_PAGE_INCREMENT)
      /* Deplacement en avant du volume de la fenetre */
      delta = FrameTable[frame].FrWidth;
   else
      delta = MAX_SIZE;		/* indeterminee */

   notifyDoc.event = TteViewScroll;
   VueDeFenetre (frame, &doc, &view);
   notifyDoc.document = doc;
   notifyDoc.view = view;
   notifyDoc.verticalValue = 0;
   notifyDoc.horizontalValue = delta;
   if (!CallEventType ((NotifyEvent *) & notifyDoc, TRUE))
     {
	if (info->reason == XmCR_VALUE_CHANGED || info->reason == XmCR_DRAG)
	  {
	     /* On recupere la largeur de l'ascenseur */
	     n = 0;
	     XtSetArg (args[n], XmNsliderSize, &l);
	     n++;
	     XtGetValues (FrameTable[frame].WdScrollH, args, n);
	     /* On regarde si le deplacement bute sur le bord droit */
	     if (info->value + l >= FrameTable[frame].FrWidth)
		delta = FrameTable[frame].FrWidth;
	     else
		delta = info->value;
	     /* Cadre a la position demandee */
	     CadrerVueEnX (frame, delta, FrameTable[frame].FrWidth);
	  }
	else if (info->reason == XmCR_TO_TOP)
	   /* Cadre a gauche */
	   CadrerVueEnX (frame, 0, FrameTable[frame].FrWidth);
	else if (info->reason == XmCR_TO_BOTTOM)
	   /* Cadre a droite */
	   CadrerVueEnX (frame, FrameTable[frame].FrWidth, FrameTable[frame].FrWidth);
	else
	   DefFenH (frame, delta, 1);

	notifyDoc.document = doc;
	notifyDoc.view = view;
	notifyDoc.verticalValue = 0;
	notifyDoc.horizontalValue = delta;
	CallEventType ((NotifyEvent *) & notifyDoc, FALSE);
     }
}				/*XChangeHScroll */

/* -------------------------------------------------------------------- */
/* | Demande de scroll vertical.                                      | */
/* -------------------------------------------------------------------- */
#ifdef __STDC__
void                XChangeVScroll (int *w, int frame, int *param)
#else  /* __STDC__ */
void                XChangeVScroll (w, frame, param)
int                *w;
int                 frame;
int                *param;

#endif /* __STDC__ */

{
   int                 delta;
   int                 n, view;
   int                 h, y;
   int                 start, end, total;
   Arg                 args[MAX_ARGS];
   float               carparpix;
   NotifyWindow        notifyDoc;
   Document            doc;
   XmScrollBarCallbackStruct *infos;

   infos = (XmScrollBarCallbackStruct *) param;

   /* ne pas traiter si le document est en mode NoComputedDisplay */
   if (documentDisplayMode[FrameTable[frame].FrDoc - 1] == NoComputedDisplay)
      return;

   if (infos->reason == XmCR_DECREMENT)
      /* Deplacement en arriere d'un caractere de la fenetre */
      delta = -13;
   else if (infos->reason == XmCR_INCREMENT)
      /* Deplacement en avant d'un caractere de la fenetre */
      delta = 13;
   else if (infos->reason == XmCR_PAGE_DECREMENT)
      /* Deplacement en arriere du volume de la fenetre */
      delta = -FrameTable[frame].FrHeight;
   else if (infos->reason == XmCR_PAGE_INCREMENT)
      /* Deplacement en avant du volume de la fenetre */
      delta = FrameTable[frame].FrHeight;
   else
      delta = MAX_SIZE;		/* indeterminee */

   notifyDoc.event = TteViewScroll;
   VueDeFenetre (frame, &doc, &view);
   notifyDoc.document = doc;
   notifyDoc.view = view;
   notifyDoc.verticalValue = delta;
   notifyDoc.horizontalValue = 0;
   if (!CallEventType ((NotifyEvent *) & notifyDoc, TRUE))
     {
	if (infos->reason == XmCR_VALUE_CHANGED || infos->reason == XmCR_DRAG)
	  {
	     /* Deplacement absolu dans la vue du document */
	     delta = infos->value;
	     /* Recupere la hauteur de l'ascenseur */
	     n = 0;
	     XtSetArg (args[n], XmNsliderSize, &h);
	     n++;
	     XtGetValues (FrameTable[frame].WdScrollV, args, n);

	     /* Regarde ou se situe l'image abstraite dans le document */
	     n = ZoneImageAbs (frame, &start, &end, &total);
	     /* au retour n = 0 si l'Picture est complete */
	     /* Calcule le nombre de caracteres represente par un pixel */
	     carparpix = (float) total / (float) FrameTable[frame].FrHeight;
	     y = (int) ((float) infos->value * carparpix);

	     if (n == 0 || (y >= start && y <= total - end))
	       {
		  /* On se deplace a l'interieur de l'Picture Concrete */
		  /* Calcule la portion de scroll qui represente l'Picture Concrete */
		  start = (int) ((float) start / carparpix);
		  end = (int) ((float) end / carparpix);
		  delta = FrameTable[frame].FrHeight - start - end;
		  /* Calcule la position demandee dans cette portion de scroll */
		  /* On detecte quand le deplacement bute en bas du document */
		  if (infos->value + h >= FrameTable[frame].FrHeight)
		     y = delta;
		  else
		     y = infos->value - start;
		  CadrerVueEnY (frame, y, delta);
	       }
	     else
	       {
		  /* On regarde si le deplacement bute en bas du document */
		  if (delta + h >= FrameTable[frame].FrHeight - 4)
		     delta = FrameTable[frame].FrHeight;
		  else if (delta >= 4)
		     /* Ou plutot vers le milieu */
		     delta += h / 2;
		  else
		     delta = 0;

		  delta = (delta * 100) / FrameTable[frame].FrHeight;
		  JumpIntoView (frame, delta);
		  /* Mise a jour des bandes de scroll pour ajustement */
		  MajScrolls (frame);
	       }
	  }
	else if (infos->reason == XmCR_TO_TOP)
	  {
	     /* Sauter au debut du document */
	     JumpIntoView (frame, 0);
	     /* Mise a jour des bandes de scroll pour ajustement */
	     MajScrolls (frame);
	  }
	else if (infos->reason == XmCR_TO_BOTTOM)
	  {
	     /* Sauter a la fin du document */
	     JumpIntoView (frame, 100);
	     /* Mise a jour des bandes de scroll pour ajustement */
	     MajScrolls (frame);
	  }
	else
	   DefFenV (frame, delta, 1);

	notifyDoc.document = doc;
	notifyDoc.view = view;
	notifyDoc.verticalValue = delta;
	notifyDoc.horizontalValue = 0;
	CallEventType ((NotifyEvent *) & notifyDoc, FALSE);
     }
}				/*XChangeVScroll */

#endif /* !NEW_WILLOWS */

/* -------------------------------------------------------------------- */
/* | PageUp scrolls one screen up.                                    | */
/* -------------------------------------------------------------------- */
#ifdef __STDC__
void                TtcPageUp (Document document, View view)
#else  /* __STDC__ */
void                TtcPageUp (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
#ifndef NEW_WILLOWS
   XmScrollBarCallbackStruct infos;
   int                 frame;

   if (document != 0)
      frame = GetWindowNumber (document, view);

   infos.reason = XmCR_PAGE_DECREMENT;
   XChangeVScroll (0, frame, (int *) &infos);
#endif /* NEW_WILLOWS */
}

/* -------------------------------------------------------------------- */
/* | PageDown scrolls one screen down.                                | */
/* -------------------------------------------------------------------- */
#ifdef __STDC__
void                TtcPageDown (Document document, View view)
#else  /* __STDC__ */
void                TtcPageDown (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
#ifndef NEW_WILLOWS
   XmScrollBarCallbackStruct infos;
   int                 frame;

   if (document != 0)
      frame = GetWindowNumber (document, view);

   infos.reason = XmCR_PAGE_INCREMENT;
   XChangeVScroll (0, frame, (int *) &infos);
#endif /* NEW_WILLOWS */
}


/* -------------------------------------------------------------------- */
/* | PageTop goes to the document top.                                | */
/* -------------------------------------------------------------------- */
#ifdef __STDC__
void                TtcPageTop (Document document, View view)
#else  /* __STDC__ */
void                TtcPageTop (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
#ifndef NEW_WILLOWS
   XmScrollBarCallbackStruct infos;
   int                 frame;

   if (document != 0)
      frame = GetWindowNumber (document, view);

   infos.reason = XmCR_TO_TOP;
   XChangeVScroll (0, frame, (int *) &infos);
#endif /* NEW_WILLOWS */
}

/* -------------------------------------------------------------------- */
/* | PageEnd goes to the document end.                                | */
/* -------------------------------------------------------------------- */
#ifdef __STDC__
void                TtcPageEnd (Document document, View view)
#else  /* __STDC__ */
void                TtcPageEnd (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
#ifndef NEW_WILLOWS
   XmScrollBarCallbackStruct infos;
   int                 frame;

   if (document != 0)
      frame = GetWindowNumber (document, view);

   infos.reason = XmCR_TO_BOTTOM;
   XChangeVScroll (0, frame, (int *) &infos);
#endif /* NEW_WILLOWS */
}




/* ---------------------------------------------------------------------- */
/* |    InitAutreContexts initialise les contextes complementaires.     | */
/* ---------------------------------------------------------------------- */
void                InitAutreContexts ()
{
   int                 i;

   /* Initialisation de la table des widgets de frames */
   for (i = 0; i <= MAX_FRAME; i++)
     {
	FrameTable[i].WdFrame = 0;
	FrameTable[i].FrDoc = 0;
     }

   ClickIsDone = 0;
   ClickFrame = 0;
   ClickX = 0;
   ClickY = 0;
   /* message de selection vide */
#ifndef NEW_WILLOWS
   null_string = XmStringCreateSimple ("");
#endif
}				/*InitAutreContexts */


/* -------------------------------------------------------------------- */
/* | Map and raise the corresponding window.                          | */
/* -------------------------------------------------------------------- */
#ifdef __STDC__
void                TtaRaiseView (Document document, View view)
#else  /* __STDC__ */
void                TtaRaiseView (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   int                 idwindow;
   ThotWidget          w;

   UserErrorCode = 0;
   idwindow = GetWindowNumber (document, view);
   if (idwindow != 0)
     {
	w = FrameTable[idwindow].WdFrame;
#ifndef NEW_WILLOWS
	if (w != 0)
	   XMapRaised (TtDisplay, XtWindowOfObject (XtParent (XtParent (XtParent (w)))));
#endif /* NEW_WILLOWS */
     }
}


/* ---------------------------------------------------------------------- */
/* | TtaGetViewFrame retourne le widget du frame de la vue document.    | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
ThotWidget          TtaGetViewFrame (Document document, View view)
#else  /* __STDC__ */
ThotWidget          TtaGetViewFrame (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   int                 frame;

   if (document == 0 && view == 0)
      frame = 0;
   else
      frame = GetWindowNumber (document, view);

   /* Si les parametres sont invalides */
   if (frame > MAX_FRAME)
     {
	TtaError (ERR_invalid_parameter);
	return (0);
     }
   else
      return (FrameTable[frame].WdFrame);
}

/* ---------------------------------------------------------------------- */
/* | MsgSelect affiche la se'lection donne'e en parame`tre (texte) dans | */
/* | la fenetree^tre active.                                            | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                MsgSelect (char *text)

#else  /* __STDC__ */
void                MsgSelect (text)
char               *text;

#endif /* __STDC__ */

{
   int                 doc;
   int                 view;

   if (ActiveFrame != 0)
     {
	doc = FrameTable[ActiveFrame].FrDoc;	/* recupere le document concerne */
	for (view = 1; view <= MAX_VIEW_DOC; view++)
	  {
	  /****frame = LoadedDocument[doc-1]->DocView[view - 1].DvPSchemaView;
	  if (frame != 0)****/
	     TtaSetStatus ((Document) doc, view, text, NULL);
	  }
     }
}				/*MsgSelect */


/* ---------------------------------------------------------------------- */
/* | TtaSetStatus affiche le status de la vue du document.                      | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtaSetStatus (Document document, View view, char *text, char *name)
#else  /* __STDC__ */
void                TtaSetStatus (document, view, text, name)
Document            document;
View                view;
char               *text;
char               *name;

#endif /* __STDC__ */
{
   int                 frame;
   char                s[200];

#ifndef NEW_WILLOWS
   Arg                 args[MAX_ARGS];
   XmString            title_string;

#endif /* NEW_WILLOWS */
   if (document == 0)
      return;
   else
     {
	frame = GetWindowNumber (document, view);

	if (FrameTable[frame].WdStatus != 0)
	  {
	     if (name != NULL)
	       {
		  /* text est un format */
		  sprintf (s, text, name);
#ifndef NEW_WILLOWS
		  title_string = XmStringCreateSimple (s);
#endif /* !NEW_WILLOWS */
	       }
	     else
#ifdef NEW_WILLOWS
		strncpy (&s[0], text, sizeof (s));
#else  /* !NEW_WILLOWS */
		title_string = XmStringCreateSimple (text);
#endif /* !NEW_WILLOWS */

#ifdef NEW_WILLOWS
	     SendMessage (FrameTable[frame].WdStatus, SB_SETTEXT,
			  (WPARAM) 0, (LPARAM) & s[0]);
#else  /* !NEW_WILLOWS */
	     XtSetArg (args[0], XmNlabelString, title_string);
	     XtSetValues (FrameTable[frame].WdStatus, args, 1);
	     XtManageChild (FrameTable[frame].WdStatus);
	     XmStringFree (title_string);
#endif /* NEW_WILLOWS */
	  }
     }
}				/*TtaSetStatus */



#ifdef NEW_WILLOWS
/* ---------------------------------------------------------------------- */
/* |  WndProc :  The main MS-Windows event handler for the Thot         | */
/* |        Library.                                                    | */
/* ---------------------------------------------------------------------- */
LRESULT CALLBACK    WndProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   int                 comm;
   HDC                 saveHdc;	/* Used to save WIN_curHdc during current event processing */
   int                 frame;

   if (msg == WM_CREATE)
     {
	/* cannot get WIN_GetFen at this point */
	fprintf (stderr, "Thot window created ...\n");

	return DefWindowProc (hWnd, msg, wParam, lParam);
     }

   frame = WIN_GetFen (hWnd);

   /* 
    * do not handle events if the Document is in NoComputedDisplay mode.
    */
   if (documentDisplayMode[FrameTable[frame].FrDoc - 1] == NoComputedDisplay)
      return (DefWindowProc (hWnd, msg, wParam, lParam));

   /*
    * If are waiting for the user to explicitely point to a document,
    * store the location and return.
    */
   if (ClickIsDone == 1 &&
       ((msg == WM_LBUTTONDOWN) || (msg == WM_RBUTTONDOWN)))
     {
	ClickIsDone = 0;
	ClickFrame = frame;
	ClickX = LOWORD (lParam);
	ClickY = HIWORD (lParam);
	return (DefWindowProc (hWnd, msg, wParam, lParam));
     }

   /*
    * If there is a TtaWaitShowDialogue, it's not possible to change
    * the current selection, type-in a char etc....
    if (TtaTestWaitShowDialogue() && 
    (((msg == WM_LBUTTONDOWN) || (msg == WM_RBUTTONDOWN)) &&
    !(GetKeyState(VK_CONTROL))))
    return(DefWindowProc(hWnd,msg,wParam,lParam));
    */

   /* fprintf(stderr,"WndProc\n"); */
   switch (msg)
	 {

	    case WM_PAINT:
	       WinInitColors ();	/* has to go to some better place !!!! */
	       /*
	        * Some part of the Client Area has to be repaint.
	        */
	       saveHdc = WIN_curHdc;
	       WIN_curHdc = NULL;
	       WIN_HandleExpose (hWnd, frame, wParam, lParam);
	       WIN_ReleaseDeviceContext ();
	       WIN_curHdc = saveHdc;
	       return 0;

	    case WM_KEYDOWN:
	    case WM_CHAR:
	       TtaAbortShowDialogue ();
	       MSCharTranslation (hWnd, frame, msg, wParam, lParam);
	       return 0;

	    case WM_LBUTTONDOWN:
	       /* stop any current insertion of text */
	       EndInsert ();

	       /* if the CTRL key is pressed this is a geometry change */
	       if (GetKeyState (VK_CONTROL))
		 {
		    /* changes the box position */
		    ApplyDirectTranslate (frame, LOWORD (lParam), HIWORD (lParam));

		    /* This is the beginning of a selection */
		 }
	       else
		 {
		    ClickFrame = frame;
		    ClickX = LOWORD (lParam);
		    ClickY = HIWORD (lParam);
		    LocateSelectionInView (frame, ClickX, ClickY, 2);
		 }
	       return (0);

	    case WM_MOUSEMOVE:
	       {
		  WPARAM              mMask = wParam & (MK_LBUTTON | MK_MBUTTON | MK_RBUTTON |
						     MK_SHIFT | MK_CONTROL);

		  if (mMask == MK_LBUTTON)
		    {
		       LocateSelectionInView (frame, LOWORD (lParam), HIWORD (lParam), 0);
		       return (0);
		    }
		  break;
	       }

	    case WM_LBUTTONDBLCLK:
	       /* left double click handling */
	       TtaAbortShowDialogue ();

	       /* memorise la position de la souris */
	       ClickFrame = frame;
	       ClickX = LOWORD (lParam);
	       ClickY = HIWORD (lParam);
	       LocateSelectionInView (frame, ClickX, ClickY, 3);
	       return (0);


	    case WM_MBUTTONDOWN:
	       /* stop any current insertion of text */
	       EndInsert ();

	       /* if the CTRL key is pressed this is a size change */
	       if (GetKeyState (VK_CONTROL))
		 {
		    /* changes the box size */
		    ApplyDirectResize (frame, LOWORD (lParam), HIWORD (lParam));

		    /* memorize the click position */
		 }
	       else
		 {
		    TtaAbortShowDialogue ();
		    LocateSelectionInView (frame, LOWORD (lParam), HIWORD (lParam), 0);
		 }
	       return (0);

	    case WM_SIZE:
	       {
		  RECT                rWindow;
		  int                 width = LOWORD (lParam);
		  int                 height = HIWORD (lParam);
		  int                 cyStatus;
		  int                 cyToolBar;

		  if (IsWindowVisible (WinToolBar[frame]))
		    {
		       SendMessage (WinToolBar[frame], TB_AUTOSIZE, 0, 0L);
		       InvalidateRect (WinToolBar[frame], NULL, TRUE);
		       GetWindowRect (WinToolBar[frame], &rWindow);
		       cyToolBar = rWindow.bottom - rWindow.top;
		    }
		  if (IsWindowVisible (FrameTable[frame].WdStatus))
		    {
		       GetWindowRect (FrameTable[frame].WdStatus, &rWindow);
		       cyStatus = rWindow.bottom - rWindow.top;
		       MoveWindow (FrameTable[frame].WdStatus, 0, height - cyStatus,
				   width, cyStatus, TRUE);
		    }
		  MSChangeTaille (frame, width, height, cyToolBar, cyStatus);
		  WIN_ReleaseDeviceContext ();
		  return (0);
	       }

	    case WM_VSCROLL:
	       MSChangeVScroll (frame, LOWORD (wParam), HIWORD (wParam));
	       WIN_ReleaseDeviceContext ();
	       return (0);

	    case WM_COMMAND:
	       WinThotCallBack (hWnd, wParam, lParam);
	       return (0);
	 }
   return DefWindowProc (hWnd, msg, wParam, lParam);
}
#endif /* NEW_WILLOWS */

#ifndef NEW_WILLOWS
/* -------------------------------------------------------------------- */
/* | Evenement sur une frame document.                              | */
/* |   D.V. equivalent de la fontion MS-Windows ci dessus !           | */
/* -------------------------------------------------------------------- */
#ifdef __STDC__
void                RetourFntr (int frame, XEvent * ev)

#else  /* __STDC__ */
void                RetourFntr (frame, ev)
int                 frame;
XEvent             *ev;

#endif /* __STDC__ */

{
   int                 comm, dx, dy;
   PtrDocument         docsel;
   XEvent              event;
   PtrElement          firstSel, lastSel;
   int                 firstCar, lastCar;
   boolean             ok;

   /* ne pas traiter si le document est en mode NoComputedDisplay */
   if (documentDisplayMode[FrameTable[frame].FrDoc - 1] == NoComputedDisplay)
      return;
/*_______> S'il n'y a pas d'evenement associe */
   else if (ev == NULL)
      return;
/*_______> Si une designation de pave est attendue*/
   else if (ClickIsDone == 1 && ev->type == ButtonPress)
     {
	ClickIsDone = 0;
	ClickFrame = frame;
	ClickX = ev->xbutton.x;
	ClickY = ev->xbutton.y;
	return;
     }				/*else if */

   /* S'il y a un TtaWaitShowDialogue en cours on n'autorise pas de changer */
   /* la selection courante. */
   if (TtaTestWaitShowDialogue ()
       && (ev->type != ButtonPress || (ev->xbutton.state & THOT_KEY_ControlMask) == 0))
      return;

   switch (ev->type)
	 {
	    case ButtonPress:
/*_____________________________________________________*/
	       switch (ev->xbutton.button)
		     {
			   /* ==========BOUTON GAUCHE========== */
			case Button1:
			   /* Termine l'insertion courante s'il y en a une */
			   EndInsert ();

			   /* Est-ce que la touche modifieur de geometrie est active ? */
			   if ((ev->xbutton.state & THOT_KEY_ControlMask) != 0)
			     {
				/* On change la position d'une boite */
				ApplyDirectTranslate (frame, ev->xbutton.x, ev->xbutton.y);
			     }
			   /* Est-ce un double clic */
			   else if (t1 + (Time) 500 > ev->xbutton.time)
			     {
				TtaAbortShowDialogue ();
				XtAppNextEvent (app_cont, &event);
				while (event.type != ButtonRelease)
				  {
				     TtaHandleOneEvent (&event);
				     XtAppNextEvent (app_cont, &event);
				  }	/*while */

				/* memorise la position de la souris */
				ClickFrame = frame;
				ClickX = ev->xbutton.x;
				ClickY = ev->xbutton.y;
				LocateSelectionInView (frame, ClickX, ClickY, 3);
			     }
			   /* Sinon c'est une selection normale */
			   else
			     {
				t1 = ev->xbutton.time;
				ClickFrame = frame;
				ClickX = ev->xbutton.x;
				ClickY = ev->xbutton.y;
				LocateSelectionInView (frame, ClickX, ClickY, 2);

				/* Regarde s'il s'agit d'un drag ou d'une simple marque d'insertion */
				comm = 0;	/* il n'y a pas de drag */
				XtAppNextEvent (app_cont, &event);
				while (event.type != ButtonRelease)
				  {
				     dx = event.xbutton.x - ClickX;
				     dy = event.xbutton.y - ClickY;
				     if (event.type == MotionNotify
					 && (dx > 2 || dx < -2 || dy > 2 || dy < -2))
				       {
					  LocateSelectionInView (frame, event.xbutton.x, event.xbutton.y, 1);
					  comm = 1;	/* il y a un drag */
				       }
				     TtaHandleOneEvent (&event);
				     XtAppNextEvent (app_cont, &event);
				  }	/*while */
				TtaHandleOneEvent (&event);

				/* S'il y a un drag on termine la selection */
				if (comm == 1)
				   LocateSelectionInView (frame, event.xbutton.x, event.xbutton.y, 0);
			     }	/*else */
			   break;

			   /* ==========BOUTON MILIEU========== */
			case Button2:
			   /* Termine l'insertion courante s'il y en a une */
			   EndInsert ();

			   /* Est-ce que la touche modifieur de geometrie est active ? */
			   if ((ev->xbutton.state & THOT_KEY_ControlMask) != 0)
			     {
				/* On modifie les dimensions d'une boite */
				ApplyDirectResize (frame, ev->xbutton.x, ev->xbutton.y);
			     }
			   else
			     {
				TtaAbortShowDialogue ();
				LocateSelectionInView (frame, ev->xbutton.x, ev->xbutton.y, 0);
			     }	/*else */
			   break;

			   /* ==========BOUTON DROIT========== */
			case Button3:
			   /* Termine l'insertion courante s'il y en a une */
			   EndInsert ();
			   TtaSetDialoguePosition ();
			   if (!GetCurrentSelection (&docsel, &firstSel, &lastSel, &firstCar, &lastCar))
			      TtaDisplaySimpleMessage (INFO, LIB, SEL_EL);
			   /* non, message 'Selectionnez' */
			   else if (docsel->DocReadOnly)
			      /* on ne peut inserer ou coller dans un document en lecture seule */
			      TtaDisplaySimpleMessage (INFO, LIB, RO_DOC_FORBIDDEN);
			   /* Message 'Document en lecture seule' */
			   else if (firstCar != 0 && firstSel->ElTerminal && firstSel->ElLeafType == LtPlyLine)
			     {
				/* selection a l'interieur d'une polyline */
				if (ThotLocalActions[T_editfunc] != NULL)
				   (*ThotLocalActions[T_editfunc]) (TEXT_INSERT, FALSE);
			     }
			   else
			     {
				if (ThotLocalActions[T_insertpaste] != NULL)
				   (*ThotLocalActions[T_insertpaste]) (TRUE, FALSE, 'R', &ok);
			     }

			default:
			   break;
		     }		/*switch */
	       break;

	    case KeyPress:
/*________________________________________________________*/
	       TtaAbortShowDialogue ();
	       XCharTranslation (ev);
	       break;

	    default:
	       break;
	 }			/*switch */
}				/*RetourFntr */
#endif /* NEW_WILLOWS */


/* ---------------------------------------------------------------------- */
/* |    ThotGrab fait un XGrabPointer.                                  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                ThotGrab (ThotWindow win, ThotCursor cursor, long events, int disp)
#else  /* __STDC__ */
void                ThotGrab (win, cursor, events, disp)
ThotWindow          win;
ThotCursor          cursor;
long                events;
int                 disp;

#endif /* __STDC__ */
{
#ifndef NEW_WILLOWS
   XGrabPointer (TtDisplay, win, FALSE, events, GrabModeAsync, GrabModeAsync,
		 win, cursor, CurrentTime);
#endif /* NEW_WILLOWS */
}


/* ---------------------------------------------------------------------- */
/* |    ThotGrabRoot fait un XGrabPointer dans la root window.          | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                ThotGrabRoot (ThotCursor cursor, int disp)
#else  /* __STDC__ */
void                ThotGrabRoot (cursor, disp)
ThotCursor          cursor;
int                 disp;

#endif /* __STDC__ */
{
#ifndef NEW_WILLOWS
   XGrabPointer (TtDisplay, TtRootWindow, TRUE, ButtonReleaseMask, GrabModeAsync,
		 GrabModeAsync, TtRootWindow, cursor, CurrentTime);
#endif /* NEW_WILLOWS */
}


/* ---------------------------------------------------------------------- */
/* |    ThotUngrab est une fonction d'interface pour UnGrab.            | */
/* ---------------------------------------------------------------------- */
void                ThotUngrab ()
{
#ifndef NEW_WILLOWS
   XUngrabPointer (TtDisplay, CurrentTime);
#endif /* NEW_WILLOWS */
}


/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                ManageCSS (Document document, View view)
#else  /* __STDC__ */
void                ManageCSS (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   /* This function has to be written */
}

/* ------------------------------------------------------------------- */
/* |    TtaGetThotWindow recupere le numero de la fenetre.           | */
/* ------------------------------------------------------------------- */
#ifdef __STDC__
ThotWindow          TtaGetThotWindow (int frame)
#else  /* __STDC__ */
ThotWindow          TtaGetThotWindow (frame)
int                 frame;

#endif /* __STDC__ */
{
   return FrRef[frame];
}


/* ------------------------------------------------------------------- */
/* |    SetCursorWatch affiche le curseur "montre".                  | */
/* ------------------------------------------------------------------- */
#ifdef __STDC__
void                SetCursorWatch (int thotThotWindowid)
#else  /* __STDC__ */
void                SetCursorWatch (thotThotWindowid)
int                 thotThotWindowid;

#endif /* __STDC__ */
{
#ifndef NEW_WILLOWS
   Drawable            drawable;

   drawable = TtaGetThotWindow (thotThotWindowid);
   XDefineCursor (TtDisplay, drawable, WaitCurs);
#endif /* NEW_WILLOWS */
}


/* ------------------------------------------------------------------- */
/* |    ResetCursorWatch enleve le curseur "montre".                 | */
/* ------------------------------------------------------------------- */
#ifdef __STDC__
void                ResetCursorWatch (int thotThotWindowid)
#else  /* __STDC__ */
void                ResetCursorWatch (thotThotWindowid)
int                 thotThotWindowid;

#endif /* __STDC__ */
{
#ifndef NEW_WILLOWS
   Drawable            drawable;

   drawable = TtaGetThotWindow (thotThotWindowid);
   XUndefineCursor (TtDisplay, drawable);
#endif /* NEW_WILLOWS */
}

/* ----------------------------------------------------------------------
   ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtaSetCursorWatch (Document document, View view)

#else  /* __STDC__ */
void                TtaSetCursorWatch (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   int                 frame;
   Drawable            drawable;

   UserErrorCode = 0;
   /* verifie le parametre document */
   if (document == 0 && view == 0)
     {
	for (frame = 1; frame <= MAX_FRAME; frame++)
	  {
#ifndef NEW_WILLOWS
	     drawable = TtaGetThotWindow (frame);
	     if (drawable != 0)
		XDefineCursor (TtDisplay, drawable, WaitCurs);
#endif /* NEW_WILLOWS */
	  }
     }
   else
     {
	frame = GetWindowNumber (document, view);
#ifndef NEW_WILLOWS
	if (frame != 0)
	   XDefineCursor (TtDisplay, TtaGetThotWindow (frame), WaitCurs);
#endif /* NEW_WILLOWS */
     }
}

/* ----------------------------------------------------------------------
   ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtaResetCursor (Document document, View view)

#else  /* __STDC__ */
void                TtaResetCursor (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   int                 frame;
   Drawable            drawable;

   UserErrorCode = 0;
   /* verifie le parametre document */
   if (document == 0 && view == 0)
     {
	for (frame = 1; frame <= MAX_FRAME; frame++)
	  {
#ifndef NEW_WILLOWS
	     drawable = TtaGetThotWindow (frame);
	     if (drawable != 0)
		XUndefineCursor (TtDisplay, drawable);
#endif /* NEW_WILLOWS */
	  }
     }
   else
     {
	frame = GetWindowNumber (document, view);
#ifndef NEW_WILLOWS
	if (frame != 0)
	   XUndefineCursor (TtDisplay, TtaGetThotWindow (frame));
#endif /* NEW_WILLOWS */
     }
}

/* ---------------------------------------------------------------------- */
/* | DesignationPave retourne l'identification de la fenetre et du pave | */
/* |            designe.                                                | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                DesignationPave (int *frame, int *pave)

#else  /* __STDC__ */
void                DesignationPave (frame, pave)
int                *frame;
int                *pave;

#endif /* __STDC__ */

{
#ifndef NEW_WILLOWS
   XEvent              event;

#endif /* !NEW_WILLOWS */
   int                 i;
   Drawable            drawable;

   /* Changement du curseur */
   for (i = 1; i <= MAX_FRAME; i++)
     {
#ifndef NEW_WILLOWS
	drawable = TtaGetThotWindow (i);
	if (drawable != 0)
	   XDefineCursor (TtDisplay, drawable, WindowCurs);
#endif /* !NEW_WILLOWS */
     }

   /* Boucle d'attente de designation */
   ClickIsDone = 1;
   ClickFrame = 0;
   ClickX = 0;
   ClickY = 0;
   while (ClickIsDone == 1)
     {
#ifndef NEW_WILLOWS
	XtAppNextEvent (app_cont, &event);
	TtaHandleOneEvent (&event);
#endif /* NEW_WILLOWS */
     }				/*while */

   /* Restauration du curseur */
   for (i = 1; i <= MAX_FRAME; i++)
     {
#ifndef NEW_WILLOWS
	drawable = TtaGetThotWindow (i);
	if (drawable != 0)
	   XUndefineCursor (TtDisplay, drawable);
#endif /* NEW_WILLOWS */
     }

   *frame = ClickFrame;
   if (ClickFrame > 0 && ClickFrame <= MAX_FRAME)
      *pave = (int) GetClickedAbsBox (ClickFrame, ClickX, ClickY);
   else
      *pave = 0;
}				/*DesignationPave */


/* -------------------------------------------------------------------- */
/* | Modifie le titre de la fenetre d'indice frame.                     | */
/* -------------------------------------------------------------------- */
#ifdef __STDC__
void                ChangeTitre (int frame, char *text)

#else  /* __STDC__ */
void                ChangeTitre (frame, text)
int                 frame;
char               *text;

#endif /* __STDC__ */
{
#ifndef NEW_WILLOWS
   int                 n;
   Arg                 args[MAX_ARGS];
   ThotWidget          w;

   w = FrameTable[frame].WdFrame;
   if (w != 0)
     {
	w = XtParent (XtParent (XtParent (w)));
	n = 0;
	XtSetArg (args[n], XmNtitle, text);
	n++;
	XtSetArg (args[n], XmNiconName, text);
	n++;
	XtSetValues (w, args, n);
     }
#endif /* NEW_WILLOWS */
}				/*ChangeTitre */


/* -------------------------------------------------------------------- */
/* | La frame d'indice frame devient la fenetre active.               | */
/* -------------------------------------------------------------------- */
#ifdef __STDC__
void                ChangeSelFntr (int frame)
#else  /* __STDC__ */
void                ChangeSelFntr (frame)
int                 frame;

#endif /* __STDC__ */
{
   ThotWidget          w;

   if (ActiveFrame != frame)
     {
	ActiveFrame = frame;
	if (frame != 0)
	  {
	     w = FrameTable[frame].WdFrame;
#ifndef NEW_WILLOWS
	     if (w != 0)
		XMapRaised (TtDisplay, XtWindowOfObject (XtParent (XtParent (XtParent (w)))));
#endif /* NEW_WILLOWS */
	  }
     }
}				/*ChangeSelFntr */


/* ---------------------------------------------------------------------- */
/* |    GetFenetre retourne l'indice de la table des Cadres associe'    | */
/* |            a` la fenetre w.                                        | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
int                 GetFenetre (ThotWindow w)
#else  /* __STDC__ */
int                 GetFenetre (w)
ThotWindow          w;

#endif /* __STDC__ */
{
   int                 f;

   /* On recherche l'indice de la fenetre */
   for (f = 0; f <= MAX_FRAME; f++)
     {
	if (FrRef[f] != 0 && FrRef[f] == w)
	   break;
     }
   return (f);
}


/* ---------------------------------------------------------------------- */
/* |    DimFenetre retourne les dimensions de la fenetre d'indice frame.        | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                DimFenetre (int frame, int *width, int *height)

#else  /* __STDC__ */
void                DimFenetre (frame, width, height)
int                 frame;
int                *width;
int                *height;

#endif /* __STDC__ */

{
   *width = FrameTable[frame].FrWidth;
   *height = FrameTable[frame].FrHeight;
}


/* ---------------------------------------------------------------------- */
/* |    SetClip limite la zone de reaffichage sur la fenetre frame et   | */
/* |            recalcule ses limites sur l'image concrete.             | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                SetClip (int frame, int orgx, int orgy, int *xd, int *yd, int *xf, int *yf, int raz)

#else  /* __STDC__ */
void                SetClip (frame, orgx, orgy, xd, yd, xf, yf, raz)
int                 frame;
int                 orgx;
int                 orgy;
int                *xd;
int                *yd;
int                *xf;
int                *yf;
int                 raz;

#endif /* __STDC__ */
{
   int                 clipx, clipy, clipwidth, clipheight;

#ifndef NEW_WILLOWS
   XRectangle          rect;

#endif /* NEW_WILLOWS */

   if (*xd < *xf && *yd < *yf && orgx < *xf && orgy < *yf)
     {
	/* On calcule le rectangle de clipping su la fenetre */
	clipx = *xd - orgx;
	if (clipx < 0)
	  {
	     *xd -= clipx;
	     clipx = 0;
	  }

	clipy = *yd - orgy;
	if (clipy < 0)
	  {
	     *yd -= clipy;
	     clipy = 0;
	  }

	clipwidth = FrameTable[frame].FrWidth + orgx;
	if (*xf > clipwidth)
	   *xf = clipwidth;
	clipheight = FrameTable[frame].FrHeight + orgy;
	if (*yf > clipheight)
	   *yf = clipheight;
	clipwidth = *xf - *xd;
	clipheight = *yf - *yd;
#ifndef NEW_WILLOWS
	rect.x = 0;
	rect.y = 0;
	rect.width = clipwidth;
	rect.height = clipheight;
	XSetClipRectangles (TtDisplay, TtLineGC, clipx + FrameTable[frame].FrLeftMargin,
		 clipy + FrameTable[frame].FrTopMargin, &rect, 1, Unsorted);
	XSetClipRectangles (TtDisplay, TtGreyGC, clipx + FrameTable[frame].FrLeftMargin,
		 clipy + FrameTable[frame].FrTopMargin, &rect, 1, Unsorted);
	XSetClipRectangles (TtDisplay, TtGraphicGC, clipx + FrameTable[frame].FrLeftMargin,
		 clipy + FrameTable[frame].FrTopMargin, &rect, 1, Unsorted);
#endif /* NEW_WILLOWS */
	if (raz > 0)
	   Clear (frame, clipwidth, clipheight, clipx, clipy);
     }
}


/* ---------------------------------------------------------------------- */
/* |    ResetClip annule le rectangle de clipping de la fenetre frame.  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                ResetClip (int frame)
#else  /* __STDC__ */
void                ResetClip (frame)
int                 frame;

#endif /* __STDC__ */
{
#ifndef NEW_WILLOWS
   XRectangle          rect;

   rect.x = 0;
   rect.y = 0;
   rect.width = MAX_SIZE;
   rect.height = MAX_SIZE;
   XSetClipRectangles (TtDisplay, TtLineGC, 0, 0, &rect, 1, Unsorted);
   XSetClipRectangles (TtDisplay, TtGraphicGC, 0, 0, &rect, 1, Unsorted);
   XSetClipRectangles (TtDisplay, TtGreyGC, 0, 0, &rect, 1, Unsorted);
   XFlushOutput (frame);
#endif /* NEW_WILLOWS */
}


/* -------------------------------------------------------------------- */
/* | MajScrolls met a jour les bandes de defilement de la fenetree^tre    | */
/* -------------------------------------------------------------------- */
#ifdef __STDC__
void                MajScrolls (int frame)

#else  /* __STDC__ */
void                MajScrolls (frame)
int                 frame;

#endif /* __STDC__ */

{
   int                 Xpos, Ypos;
   int                 width, height;
   int                 l, h;
   ThotWidget          hscroll, vscroll;
   int                 n;

#ifndef NEW_WILLOWS
   Arg                 args[MAX_ARGS];

#endif /* NEW_WILLOWS */

   /* Demande le volume affiche dans la fenetre */
   VolumeAffiche (frame, &Xpos, &Ypos, &width, &height);
   hscroll = FrameTable[frame].WdScrollH;
   vscroll = FrameTable[frame].WdScrollV;
   l = FrameTable[frame].FrWidth;
   h = FrameTable[frame].FrHeight;

#ifndef NEW_WILLOWS
   n = 0;
   if (width + Xpos <= l)
     {
	XtSetArg (args[n], XmNminimum, 0);
	n++;
	XtSetArg (args[n], XmNmaximum, l);
	n++;
	XtSetArg (args[n], XmNvalue, Xpos);
	n++;
	XtSetArg (args[n], XmNsliderSize, width);
	n++;
	XtSetValues (hscroll, args, n);
     }

   n = 0;
   if (height + Ypos <= h)
     {
	XtSetArg (args[n], XmNminimum, 0);
	n++;
	XtSetArg (args[n], XmNmaximum, h);
	n++;
	XtSetArg (args[n], XmNvalue, Ypos);
	n++;
	XtSetArg (args[n], XmNsliderSize, height);
	n++;
	XtSetValues (vscroll, args, n);
     }
#ifdef NEW_WILLOWS
#endif /* NEW_WILLOWS */
#endif /* NEW_WILLOWS */
}				/*MajScrolls */


/* End Of Module Thot */

