/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "storage.h"
#include "application.h"
#include "document.h"
#include "view.h"
#include "tree.h"
#include "selection.h"
#include "typecorr.h"
#include "thotcolor.h"
#include "appdialogue.h"

#undef EXPORT
#define EXPORT extern
#include "platform_tv.h"
#include "page_tv.h"
#include "select_tv.h"
#include "frame_tv.h"
#include "edit_tv.h"
#include "boxes_tv.h"
#include "platform_tv.h"
#include "thotcolor_tv.h"
#include "appdialogue_tv.h"

#include "appli_f.h"
#include "tree_f.h"
#include "attrherit_f.h"
#include "attributes_f.h"
#include "context_f.h"
#include "structcreation_f.h"
#include "createabsbox_f.h"
#include "scroll_f.h"
#include "views_f.h"
#include "viewapi_f.h"
#include "dofile_f.h"
#include "absboxes_f.h"
#include "buildboxes_f.h"
#include "inites_f.h"
#include "structmodif_f.h"
#include "changeabsbox_f.h"
#include "changepresent_f.h"
#include "boxparams_f.h"
#include "paginate_f.h"
#include "presrules_f.h"
#include "schemas_f.h"
#include "boxselection_f.h"
#include "structselect_f.h"
#include "fileaccess_f.h"
#include "structschema_f.h"
#include "thotmsg_f.h"

extern int          UserErrorCode;

/* descripteur de la selection a effectuer apres reaffichage */
typedef struct _SelectionDescriptor
  {
     boolean             SDSelActive;
     Element             SDElemSel;
     int                 SDPremCar;
     int                 SDDerCar;
     Element             SDElemExt;
     int                 SDCarExt;
  }
SelectionDescriptor;

static SelectionDescriptor documentNewSelection[MAX_DOCUMENTS];
static char         nameBuffer[MAX_NAME_LENGTH];


/* ---------------------------------------------------------------------- */
/* |    GetViewInfo returns wiew number and assoc state of the          | */
/* |             corresponding to the view of the document.             | */
/* |    Parameters:                                                     | */
/* |            document: the document.                                 | */
/* |            view: the view.                                         | */
/* |    Return value:                                                   | */
/* |            corresponding view number.                              | */
/* |            corresponding assoc state.                              | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                GetViewInfo (Document document, View view, int *viewnumber, boolean * assoc)
#else  /* __STDC__ */
void                GetViewInfo (document, view, viewnumber, assoc)
Document            document;
View                view;
int                *viewnumber;
boolean            *assoc;

#endif /* __STDC__ */
{

   *assoc = FALSE;
   *viewnumber = 0;

   if (view < 100)
      *viewnumber = (int) view;
   else
     {
	*assoc = TRUE;
	*viewnumber = (int) view - 100;
     }
}


/* ---------------------------------------------------------------------- */
/* |    GetWindowNumber returns the window corresponding to the view of | */
/* |            the document.                                           | */
/* |    Parameters:                                                     | */
/* |            document: the document.                                 | */
/* |            view: the view.                                         | */
/* |    Return value:                                                   | */
/* |            corresponding window.                                   | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
int                 GetWindowNumber (Document document, View view)

#else  /* __STDC__ */
int                 GetWindowNumber (document, view)
Document            document;
View                view;

#endif /* __STDC__ */

{
   PtrDocument         pDoc;
   boolean             assoc;
   int                 vue, win;

   win = 0;
   /* verifie les parametres */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else if (view < 1 || (view > MAX_VIEW_DOC && view < 100) || view > MAX_ASSOC_DOC + 100)
      TtaError (ERR_invalid_parameter);
   else
     {
	pDoc = LoadedDocument[document - 1];
	GetViewInfo (document, view, &vue, &assoc);
	if (assoc)
	   win = pDoc->DocAssocFrame[vue - 1];
	else
	   win = pDoc->DocViewFrame[vue - 1];
     }
   return win;
}				/*GetWindowNumber */


/* ----------------------------------------------------------------------
   TtaOpenMainView

   Opens the main view of a document. This document must have a PSchema
   (see TtaSetPSchema).

   Parameters:
   document: the document for which a window must be open.
   x, y: coordinate (in millimeters) of the upper left corner of the
   window that will display the view.
   w, h: width and height (in millimeters) of the upper left corner of the
   window that will display the view.

   Return value:
   the view opened or 0 if the view cannot be opened.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
View                TtaOpenMainView (Document document, int x, int y, int w, int h)

#else  /* __STDC__ */
View                TtaOpenMainView (document, x, y, w, h)
Document            document;
int                 x;
int                 y;
int                 w;
int                 h;

#endif /* __STDC__ */

{
   PtrDocument         pDoc;
   int                 nVue;
   View                view;

   UserErrorCode = 0;
   view = 0;
   /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parametre document correct */
     {
	pDoc = LoadedDocument[document - 1];
	if (pDoc->DocSSchema != NULL)
	   if (pDoc->DocSSchema->SsPSchema == NULL)
	      TtaError (ERR_no_presentation_schema);
	   else
	     {
#ifndef __COLPAGE__
		/* Ajoute le saut de page qui manque eventuellement a la */
		/* fin du document */
		if (pDoc->DocSSchema->SsPSchema->PsPaginatedView[0])
		   AddLastPageBreak (pDoc->DocRootElement, 1, pDoc, FALSE);
#endif /* __COLPAGE__ */
		nVue = CreateAbstractImage (pDoc, 1, 0, pDoc->DocSSchema, 1, TRUE, NULL);
		OpenCreatedView (pDoc, nVue, FALSE, x, y, w, h);
		view = nVue;
	     }
     }
   return view;
}


/* ----------------------------------------------------------------------
   OpenView

   Opens a view for a document.

   Parameters:
   document: the document.
   viewName: name of the view to be opened.
   x, y: coordinate (in millimeters) of the upper left corner of the
   window that will display the view.
   w, h: width and height (in millimeters) of the upper left corner of the
   window that will display the view.

   Return value:
   the view opened or 0 if the view cannot be opened.
   ------------------------------------------------------------------------ */
#ifdef __STDC__
static View         OpenView (Document document, char *viewName, int x, int y, int w, int h, Element subtree)
#else  /* __STDC__ */
static View         OpenView (document, viewName, x, y, w, h, subtree)
Document            document;
char               *viewName;
int                 x;
int                 y;
int                 w;
int                 h;
Element             subtree;

#endif /* __STDC__ */
{
   int                 nVue;
   int                 nbvues;
   int                 i;
   int                 v;
   PtrDocument         pDoc;
   AvailableView           toutesLesVues;
   boolean             assoc;
   boolean             trouve;
   View                view;

   UserErrorCode = 0;
   view = 0;
   v = 0;
   /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parametre document correct */
   if (LoadedDocument[document - 1]->DocSSchema->SsPSchema == NULL)
      TtaError (ERR_no_presentation_schema);
   else
     {
	pDoc = LoadedDocument[document - 1];
	assoc = FALSE;
	nVue = 0;
	/* etablit la liste de toutes les vues possibles pour ce document */
	nbvues = BuildDocumentViewList (pDoc, toutesLesVues);
	/* cherche dans cette liste le nom de la vue a ouvrir */
	trouve = FALSE;
	for (i = 0; i < nbvues && !trouve; i++)
	  {
	     trouve = strcmp (viewName, toutesLesVues[i].VdViewName) == 0;
	     if (trouve)
		v = i;
	  }
	if (!trouve)
	   TtaError (ERR_invalid_parameter);
	else
	   /* on a trouve' cette vue */
	  {
	     /* on ouvre la vue */
	     if (toutesLesVues[v].VdAssoc)
	       {
#ifndef __COLPAGE__
		  /* Ajoute le saut de page qui manque eventuellement a la fin */
		  if (toutesLesVues[v].VdView > 0)
		     if (pDoc->DocSSchema->SsPSchema->PsPaginatedView[0])
			AddLastPageBreak (pDoc->DocAssocRoot[toutesLesVues[v].VdView - 1], 1, pDoc, FALSE);
#endif /* __COLPAGE__ */
		  nVue = CreateAbstractImage (pDoc, 0, toutesLesVues[v].VdAssocNum,
		    toutesLesVues[v].VdSSchema, 1, TRUE, (PtrElement) subtree);
		  assoc = TRUE;
	       }
	     else
	       {
#ifndef __COLPAGE__
		  /* Ajoute le saut de page qui manque eventuellement a la fin */
		  if (pDoc->DocSSchema->SsPSchema->PsPaginatedView[toutesLesVues[v].VdView])
		     AddLastPageBreak (pDoc->DocRootElement, toutesLesVues[v].VdView, pDoc, FALSE);
#endif /* __COLPAGE__ */
		  nVue = CreateAbstractImage (pDoc, toutesLesVues[v].VdView, 0,
		   toutesLesVues[v].VdSSchema, 1, FALSE, (PtrElement) subtree);
		  assoc = FALSE;
	       }
	     if (nVue == 0)
		TtaError (ERR_cannot_open_view);
	     else
	       {
		  OpenCreatedView (pDoc, nVue, assoc, x, y, w, h);
		  if (assoc)
		     view = nVue + 100;
		  else
		     view = nVue;
	       }
	  }
     }
   return view;
}


/* ----------------------------------------------------------------------
   TtaOpenView

   Opens a view for a document. This document must have a PSchema
   (see TtaSetPSchema).

   Parameters:
   document: the document.
   viewName: name of the view to be opened.
   x, y: coordinate (in millimeters) of the upper left corner of the
   window that will display the view.
   w, h: width and height (in millimeters) of the upper left corner of the
   window that will display the view.

   Return value:
   the view opened or 0 if the view cannot be opened.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
View                TtaOpenView (Document document, char *viewName, int x, int y, int w, int h)

#else  /* __STDC__ */
View                TtaOpenView (document, viewName, x, y, w, h)
Document            document;
char               *viewName;
int                 x;
int                 y;
int                 w;
int                 h;

#endif /* __STDC__ */

{
   return OpenView (document, viewName, x, y, w, h, NULL);
}

/* ----------------------------------------------------------------------
   TtaOpenSubView

   Opens a view that shows only a subtree. This document must have a PSchema
   (see TtaSetPSchema).

   Parameters:
   document: the document.
   viewName: name of the view to be opened.
   x, y: coordinate (in millimeters) of the upper left corner of the
   window that will display the view.
   w, h: width and height (in millimeters) of the upper left corner of the
   window that will display the view.
   subtree: root element of the subtree to be shown in the view.

   Return value:
   the view opened or 0 if the view cannot be opened.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
View                TtaOpenSubView (Document document, char *viewName, int x, int y, int w, int h, Element subtree)

#else  /* __STDC__ */
View                TtaOpenSubView (document, viewName, x, y, w, h, subtree)
Document            document;
char               *viewName;
int                 x;
int                 y;
int                 w;
int                 h;
Element             subtree;

#endif /* __STDC__ */

{
   return OpenView (document, viewName, x, y, w, h, subtree);
}

/* ----------------------------------------------------------------------
   TtaCloseView

   Closes a view.

   Parameters:
   document: the document for which a view must be closed.
   view: the view to be closed.

   ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtaCloseView (Document document, View view)
#else  /* __STDC__ */
void                TtaCloseView (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   PtrDocument         pDoc;
   int                 numAssoc;

   UserErrorCode = 0;
   /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parametre document correct */
     {
	pDoc = LoadedDocument[document - 1];
	if (view < 100)
	   /* vue de l'arbre principal */
	   if (view < 1 || view > MAX_VIEW_DOC)
	      TtaError (ERR_invalid_parameter);
	   else
	     {
		DestroyFrame (pDoc->DocViewFrame[view - 1]);
		CloseDocumentView (pDoc, view, FALSE, FALSE);
	     }
	else
	   /* vue d'elements associes */
	  {
	     numAssoc = view - 100;
	     if (numAssoc < 1 || numAssoc > MAX_ASSOC_DOC)
		TtaError (ERR_invalid_parameter);
	     else
	       {
		  DestroyFrame (pDoc->DocAssocFrame[numAssoc - 1]);
		  CloseDocumentView (pDoc, numAssoc, TRUE, FALSE);
	       }
	  }
     }
}

/* ---------------------------------------------------------------------- */
/* |  DetruitImageVue detruit l'image abstraite de la vue Vue du        | */
/* |     document pDoc. Procedure partiellement reprise de              | */
/* |     DetrImAbs_Pages du module page.c                               | */
/* |     Vue = numero de vue ou d'elt assoc si vue associee.            | */
/* |     complete = TRUE si on libere complement la fenetre             | */
/* ----------------------------------------------------------------------- */
#ifdef __STDC__
static void         DetruitImageVue (int Vue, boolean Assoc, PtrDocument pDoc, boolean complete)
#else  /* __STDC__ */
static void         DetruitImageVue (Vue, Assoc, pDoc, complete)
int                 Vue;
boolean             Assoc;
PtrDocument         pDoc;
boolean             complete;

#endif /* __STDC__ */
{
   PtrAbstractBox             pAb;
   int                 h;
   int                 frame;
   PtrAbstractBox             pAbbRoot;

   frame = 1;			/* initialisation (pour le compilateur !) */
   pAbbRoot = NULL;		/* initialisation (pour le compilateur !) */
   if (Assoc)
     {
	/* c'est une vue d'elements associes */
	pAbbRoot = pDoc->DocAssocRoot[Vue - 1]->ElAbstractBox[0];
	frame = pDoc->DocAssocFrame[Vue - 1];
     }
   else
     {
	pAbbRoot = pDoc->DocViewRootAb[Vue - 1];
	frame = pDoc->DocViewFrame[Vue - 1];
     }
   /* tous les paves englobes par le pave racine de la vue sont marques */
   /* morts */
   if (complete)
     {
	SetDeadAbsBox (pAbbRoot);
	ChangeConcreteImage (frame, &h, pAbbRoot);
	CloseDocumentView (pDoc, Vue, Assoc, TRUE);
	FrameTable[frame].FrDoc = 0;
	/*ViewFrameTable[frame - 1].FrAbstractBox = NULL; */
     }
   else
     {
	pAb = pAbbRoot->AbFirstEnclosed;
	while (pAb != NULL)
	  {
	     SetDeadAbsBox (pAb);
	     pAb = pAb->AbNext;
	  }
#ifdef __COLPAGE__
	/* vide la chaine des regles en retard sur la racine */
	/* normalement doit etre deja vide ! */
	ApplDelayedRule (pAbbRoot->AbFirstEnclosed->AbElement, pDoc);
	/* libere tous les paves morts de la vue */
	/* ceci est signale au Mediateur */
	/* ceci est signale au Mediateur */
	h = -1;			/* changement de signification de h */
#else  /* __COLPAGE__ */
	h = 0;
#endif /* __COLPAGE__ */
	ChangeConcreteImage (frame, &h, pAbbRoot);
	/* libere tous les paves morts de la vue */
	FreeDeadAbstractBoxes (pAbbRoot);

	/* indique qu'il faudra reappliquer les regles de presentation du */
	/* pave racine, par exemple pour recreer les boites de presentation */
	/* creees par lui et qui viennent d'etre detruites. */
	pAbbRoot->AbSize = -1;
#ifdef __COLPAGE__
	pAbbRoot->AbTruncatedTail = TRUE;
#endif /* __COLPAGE__ */
	/* on marque le pave racine complet en tete pour que AbsBoxesCreate */
	/* engendre effectivement les paves de presentation cree's en tete */
	if (pAbbRoot->AbLeafType == LtCompound)
	   pAbbRoot->AbTruncatedHead = FALSE;
     }
}				/* DetruitImageVue */

/* ----------------------------------------------------------------------
   TtaFreeView

   frees the view of the document. The window continues to exist but the document
   is no longer displayed in this window.

   Parameters:
   document: the document for which a view must be closed.
   view: the view to be closed.

   ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtaFreeView (Document document, View view)
#else  /* __STDC__ */
void                TtaFreeView (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   PtrDocument         pDoc;
   int                 numAssoc;

   UserErrorCode = 0;
   /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parametre document correct */
     {
	pDoc = LoadedDocument[document - 1];
	if (view < 100)
	   /* vue de l'arbre principal */
	   if (view < 1 || view > MAX_VIEW_DOC)
	      TtaError (ERR_invalid_parameter);
	   else
	      DetruitImageVue (view, FALSE, pDoc, TRUE);
	else
	   /* vue d'elements associes */
	  {
	     numAssoc = view - 100;
	     if (numAssoc < 1 || numAssoc > MAX_ASSOC_DOC)
		TtaError (ERR_invalid_parameter);
	     else
		DetruitImageVue (numAssoc, TRUE, pDoc, TRUE);
	  }
     }
}

/* ----------------------------------------------------------------------
   TtaChangeViewTitle

   Changes the title of a view.

   Parameters:
   document: the document.
   view: the view.
   title: the new title.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TtaChangeViewTitle (Document document, View view, char *title)

#else  /* __STDC__ */
void                TtaChangeViewTitle (document, view, title)
Document            document;
View                view;
char               *title;

#endif /* __STDC__ */

{
   PtrDocument         pDoc;
   int                 numAssoc;

   UserErrorCode = 0;
   /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parametre document correct */
     {
	pDoc = LoadedDocument[document - 1];
	if (view < 100)
	   /* vue de l'arbre principal */
	   if (view < 1 || view > MAX_VIEW_DOC)
	      TtaError (ERR_invalid_parameter);
	   else
	      ChangeTitre (pDoc->DocViewFrame[view - 1], title);
	else
	   /* vue d'elements associes */
	  {
	     numAssoc = view - 100;
	     if (numAssoc < 1 || numAssoc > MAX_ASSOC_DOC)
		TtaError (ERR_invalid_parameter);
	     else
		ChangeTitre (pDoc->DocAssocFrame[numAssoc - 1], title);
	  }
     }
}


/* ---------------------------------------------------------------------
   TtaSetSensibility

   Changes the current sensibility used to display a given view of a
   given document.

   Parameters:
   document: the document. Cannot be 0.
   view: the view.
   value: new value of the sensibility.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TtaSetSensibility (Document document, View view, int value)

#else  /* __STDC__ */
void                TtaSetSensibility (document, view, value)
Document            document;
View                view;
int                 value;

#endif /* __STDC__ */

{
   int                 frame;
   int                 valvisib, valzoom;

   UserErrorCode = 0;
   frame = GetWindowNumber (document, view);
   if (frame != 0)
     {
	GetVisu (frame, &valvisib, &valzoom);
	/* Traduction de la sensibilite en seuil */
	valvisib = 10 - value;
	if (valvisib < 1 || valvisib > 10)
	   TtaError (ERR_invalid_parameter);
	else
	   ModVisu (frame, valvisib, valzoom);
     }
}				/*TtaSetSensibility */


/* ----------------------------------------------------------------------
   TtaSetZoom

   Changes the current zoom used to display a given
   view of a given document.

   Parameters:
   document: the document. Cannot be 0.
   view: the view.
   value: new value of the zoom.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TtaSetZoom (Document document, View view, int value)

#else  /* __STDC__ */
void                TtaSetZoom (document, view, value)
Document            document;
View                view;
int                 value;

#endif /* __STDC__ */

{
   int                 frame;
   int                 valvisib, valzoom;

   UserErrorCode = 0;
   frame = GetWindowNumber (document, view);
   if (frame != 0)
     {
	GetVisu (frame, &valvisib, &valzoom);
	/* Traduction de la sensibilite en seuil */
	valzoom += value;
	if (valzoom < 1 || valzoom > 10)
	   TtaError (ERR_invalid_parameter);
	else
	   ModVisu (frame, valvisib, valzoom);
     }
}				/*TtaSetZoom */


/* ----------------------------------------------------------------------
   TtaShowElement

   Shows a given element in a given view of a given document.

   Parameters:
   document: the document to which the element to be shown belongs.
   Cannot be 0.
   view: the view where the element must be shown.
   element: the element to be shown.
   position: position of the top of the element in the window.
   Supposing that the y axis is oriented from the top
   of the window (coordinate 0) to the bottom (coordinate 100,
   whatever the actual height of the window), position is the
   desired y coordinate of the top of the element.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TtaShowElement (Document document, View view, Element element, int position)

#else  /* __STDC__ */
void                TtaShowElement (document, view, element, position)
Document            document;
View                view;
Element             element;
int                 position;

#endif /* __STDC__ */

{
   int                 frame;
   int                 vue;
   PtrElement          pEl;

   UserErrorCode = 0;
   frame = GetWindowNumber (document, view);
   if (frame != 0)
      if (position < 0 || position > 100 || element == NULL)
	 TtaError (ERR_invalid_parameter);
      else
	{
	   if (view < 100)
	      vue = view;
	   else
	      vue = 1;
	   pEl = (PtrElement) element;
	   /* si l'element a un pave' incomplet en tete, on supprime ce pave' */
	   /* et VerifAbsBoxe le recreera depuis la tete de l'element */
	   if (pEl->ElAbstractBox[vue - 1] != NULL)
	      if (pEl->ElAbstractBox[vue - 1]->AbLeafType == LtCompound)
		 if (pEl->ElAbstractBox[vue - 1]->AbTruncatedHead)
		    /* on detruit les paves de l'element dans cette vue */
		    DestroyAbsBoxesView (pEl, LoadedDocument[document - 1], FALSE, vue);
	   VerifAbsBoxe (pEl, vue, LoadedDocument[document - 1], FALSE, FALSE);
	   if (pEl->ElAbstractBox[vue - 1] != NULL)
	      MontrerBoite (frame, pEl->ElAbstractBox[vue - 1]->AbBox, 0, position);
	}
}				/*TtaShowElement */


/* ---------------------------------------------------------------------- 
   TtaGetSensibility

   Reads the current sensibility used to display
   a given view of a given document.

   Parameters:
   document: the document. Cannot be 0.
   view: the view.

   Return value:
   current value of the sensibility.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
int                 TtaGetSensibility (Document document, View view)

#else  /* __STDC__ */
int                 TtaGetSensibility (document, view)
Document            document;
View                view;

#endif /* __STDC__ */

{
   int                 frame;
   int                 valvisib, valzoom;
   int                 value;

   UserErrorCode = 0;
   value = 0;
   frame = GetWindowNumber (document, view);
   if (frame != 0)
     {
	GetVisu (frame, &valvisib, &valzoom);
	/* Traduction du seuil en sensibilite */
	value = 10 - valvisib;
     }
   return value;
}				/*TtaGetSensibility */


/* ---------------------------------------------------------------------- 
   TtaGetZoom

   Reads the current zoom used to display a given view
   of a given document.

   Parameters:
   document: the document. Cannot be 0.
   view: the view.

   Return value:
   current value of the zoom.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
int                 TtaGetZoom (Document document, View view)

#else  /* __STDC__ */
int                 TtaGetZoom (document, view)
Document            document;
View                view;

#endif /* __STDC__ */

{
   int                 frame;
   int                 valvisib;
   int                 value;

   UserErrorCode = 0;
   frame = GetWindowNumber (document, view);
   if (frame != 0)
     {
	GetVisu (frame, &valvisib, &value);
     }
   return value;
}				/*TtaGetZoom */


/* ----------------------------------------------------------------------
   TtaIsPSchemaValid

   Checks if a presentation schema can be applied to a document of a given
   class. No document is needed and the schemas are not loaded by this
   function.

   Parameters:
   structureName: Name of the document class.
   presentationName: Name of the presentation schema to be checked.

   Return value:
   1 if the presentation schema can be applied, 0 if it can not.

   ---------------------------------------------------------------------- */
#ifdef __STDC__
int                 TtaIsPSchemaValid (char *structureName, char *presentationName)
#else  /* __STDC__ */
int                 TtaIsPSchemaValid (structureName, presentationName)
char               *structureName;
char               *presentationName;

#endif /* __STDC__ */
{
   PathBuffer          DirBuffer;
   BinFile             file;
   char                texte[MAX_TXT_LEN];
   int                 i;
   Name                 NomStructLu;
   int                 result;

   UserErrorCode = 0;
   result = 0;
   /* compose le nom du fichier a ouvrir avec le nom du directory */
   /* des schemas... */
   strncpy (DirBuffer, SchemaPath, MAX_PATH);
   BuildFileName (presentationName, "PRS", DirBuffer, texte, &i);
   /* teste si le fichier existe */
   file = BIOreadOpen (texte);
   if (file == 0)
      /* schema de presentation inaccessible */
      TtaError (ERR_cannot_load_pschema);
   else
     {
	/* lit le nom du schema de structure correspondant */
	BIOreadName (file, NomStructLu);
	if (strcmp (structureName, NomStructLu) == 0)
	   result = 1;
	BIOreadClose (file);
     }
   return result;
}


/* ----------------------------------------------------------------------
   TtaGiveViewsToOpen

   Returns the names of the views that can be opened for a document.

   Parameters:
   document: the document.
   buffer: a buffer that will contain the result.

   Return parameters:
   buffer: list of view names. Each name is a character string with
   a final '\0'. Names of views that are already open have a '*'
   at the       end.
   nbViews: number of names in the list, 0 if not any view can be open.

   ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtaGiveViewsToOpen (Document document, char *buffer, int *nbViews)
#else  /* __STDC__ */
void                TtaGiveViewsToOpen (document, buffer, nbViews)
Document            document;
char               *buffer;
int                *nbViews;

#endif /* __STDC__ */
{

   UserErrorCode = 0;
   /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parametre document correct */
      BuildViewList (LoadedDocument[document - 1], buffer, nbViews);
}


/* ----------------------------------------------------------------------
   TtaGetViewName

   Returns the name of an open view.

   Parameters:
   document: the document to which the view belongs.
   view: the view.

   Return value:
   Name of the view. The buffer must be provided by the caller.

   ---------------------------------------------------------------------- */
#ifdef __STDC__
char               *TtaGetViewName (Document document, View view)
#else  /* __STDC__ */
char               *TtaGetViewName (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   PtrDocument         pDoc;
   PtrElement          pEl;
   DocViewDescr          dVue;
   int                 numAssoc;

   UserErrorCode = 0;
   nameBuffer[0] = '\0';
   /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parametre document correct */
     {
	pDoc = LoadedDocument[document - 1];
	if (view < 100)
	   /* vue de l'arbre principal */
	   if (view < 1 || view > MAX_VIEW_DOC)
	      TtaError (ERR_invalid_parameter);
	   else
	     {
		dVue = pDoc->DocView[view - 1];
		if (dVue.DvSSchema != NULL || dVue.DvPSchemaView != 0)
		   strncpy (nameBuffer, dVue.DvSSchema->SsPSchema->PsView[dVue.DvPSchemaView - 1], MAX_NAME_LENGTH);
	     }
	else
	   /* vue d'elements associes */
	  {
	     numAssoc = view - 100;
	     if (numAssoc < 1 || numAssoc > MAX_ASSOC_DOC)
		TtaError (ERR_invalid_parameter);
	     else
	       {
		  pEl = pDoc->DocAssocRoot[numAssoc - 1];
		  if (pEl != NULL)
		     strncpy (nameBuffer, pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrName, MAX_NAME_LENGTH);
	       }
	  }
     }
   return nameBuffer;
}


/* ----------------------------------------------------------------------
   TtaIsViewOpened

   Returns TRUE for a open view.

   Parameters:
   document: the document to which the view belongs.
   view: the view.

   Return value:
   TRUE or FALSE.

   ---------------------------------------------------------------------- */
#ifdef __STDC__
boolean             TtaIsViewOpened (Document document, View view)
#else  /* __STDC__ */
boolean             TtaIsViewOpened (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   PtrDocument         pDoc;
   PtrElement          pEl;
   DocViewDescr          dVue;
   int                 numAssoc;
   boolean             opened;

   UserErrorCode = 0;
   opened = FALSE;
   /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parametre document correct */
     {
	pDoc = LoadedDocument[document - 1];
	if (view < 100)
	   /* vue de l'arbre principal */
	   if (view < 1 || view > MAX_VIEW_DOC)
	      TtaError (ERR_invalid_parameter);
	   else
	     {
		dVue = pDoc->DocView[view - 1];
		if (dVue.DvSSchema != NULL || dVue.DvPSchemaView != 0)
		   opened = TRUE;
	     }
	else
	   /* vue d'elements associes */
	  {
	     numAssoc = view - 100;
	     if (numAssoc < 1 || numAssoc > MAX_ASSOC_DOC)
		TtaError (ERR_invalid_parameter);
	     else
	       {
		  pEl = pDoc->DocAssocRoot[numAssoc - 1];
		  if (pEl != NULL)
		     opened = TRUE;
	       }
	  }
     }
   return opened;
}

/* ----------------------------------------------------------------------
   TtaGetViewFromName

   Returns the identifier of a view of a given document from its name.

   Parameters:
   document: the document to which the view belongs.
   viewName: the name of the view.

   Return value:
   the view. 0 if no view of that name is currently open for the document.

   ---------------------------------------------------------------------- */
#ifdef __STDC__
View                TtaGetViewFromName (Document document, char *viewName)
#else  /* __STDC__ */
View                TtaGetViewFromName (document, viewName)
Document            document;
char               *viewName;

#endif /* __STDC__ */
{
   View                view;
   PtrDocument         pDoc;
   PtrElement          pEl;
   DocViewDescr          dVue;
   int                 vue;

   UserErrorCode = 0;
   view = 0;
   /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else if (LoadedDocument[document - 1] == NULL)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else
      /* parametre document correct */
     {
	pDoc = LoadedDocument[document - 1];
	/* on cherche parmi les vues ouvertes de l'arbre principal */
	for (vue = 1; vue <= MAX_VIEW_DOC && view == 0; vue++)
	  {
	     dVue = pDoc->DocView[vue - 1];
	     if (dVue.DvSSchema != NULL && dVue.DvPSchemaView != 0)
		if (strcmp (viewName, dVue.DvSSchema->SsPSchema->PsView[dVue.DvPSchemaView - 1]) == 0)
		   view = vue;
	  }
	if (view == 0)
	   /* on n'a pas encore trouve', on cherche parmi les vues */
	   /* d'elements associes */
	   for (vue = 1; vue <= MAX_ASSOC_DOC && view == 0; vue++)
	     {
		pEl = pDoc->DocAssocRoot[vue - 1];
		if (pEl != NULL)
		   if (pDoc->DocAssocFrame[vue - 1] != 0)
		      if (strcmp (viewName, pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrName) == 0)
			 view = vue + 100;
	     }
     }
   return view;
}

/* ----------------------------------------------------------------------
   TtaGiveActiveView

   Returns the active view and the document to which that view belongs.
   The active view is the one that receives the characters typed by
   the user.

   Parameter:
   No parameter.

   Return values:
   document: the active document.
   view: the active view.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TtaGiveActiveView (Document * document, View * view)

#else  /* __STDC__ */
void                TtaGiveActiveView (document, view)
Document           *document;
View               *view;

#endif /* __STDC__ */

{
   PtrDocument         pDoc;
   int                 vue;
   boolean             assoc;

   UserErrorCode = 0;
   *document = 0;
   *view = 0;
   if (ActiveFrame != 0)
     {
	GetDocAndView (ActiveFrame, &pDoc, &vue, &assoc);
	if (pDoc != NULL)
	  {
	     *document = IdentDocument (pDoc);
	     if (assoc)
		*view = vue + 100;
	     else
		*view = vue;
	  }
     }
}


/* les fonctions suivantes servent au reaffichage */

/* ---------------------------------------------------------------------- */
/* |    ElemDansImage   evalue si les paves de l'element pEl pour la    | */
/* |    vue vue se placeraient a l'interieur (limites comprises) de la  | */
/* |    partie d'image abstraite deja construite.                       | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static boolean      ElemDansImage (PtrElement pEl, int vue, PtrAbstractBox pAbbRoot, PtrDocument pDoc)

#else  /* __STDC__ */
static boolean      ElemDansImage (pEl, vue, pAbbRoot, pDoc)
PtrElement          pEl;
int                 vue;
PtrAbstractBox             pAbbRoot;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   boolean             result, fini, trouve;
   PtrElement          pAsc;
   PtrAbstractBox             pAb;

   result = TRUE;
   fini = FALSE;
   /* on commence par un test simple : l'element voisin a-t-il un pave */
   /* dans la vue? */
   if (pEl->ElPrevious != NULL)
      /* il y a un voisin precedent */
     {
	pAb = pEl->ElPrevious->ElAbstractBox[vue - 1];
	if (pAb != NULL)
	   /* l'element precedent a un pave dans la vue */
	  {
	     fini = TRUE;
	     /* si le pave de l'element precedent est complet en queue, */
	     /* l'element aura son pave dans l'image existante */
	     if (pAb->AbInLine || pAb->AbLeafType != LtCompound)
		/* les paves mis en lignes ou feuille sont toujours entiers */
		result = TRUE;
	     else
		result = !pAb->AbTruncatedTail;
	  }
     }
   else if (pEl->ElNext != NULL)
      /* il y a un voisin suivant */
     {
	pAb = pEl->ElNext->ElAbstractBox[vue - 1];
	if (pAb != NULL)
	   /* l'element suivant a un pave dans la vue */
	  {
	     fini = TRUE;
	     /* si le pave de l'element suivant est complet en tete, */
	     /* l'element aura son pave dans l'image existante */
	     if (pAb->AbInLine || pAb->AbLeafType != LtCompound)
		/* les paves mis en lignes sont toujours entiers */
		result = TRUE;
	     else
		result = !pAb->AbTruncatedHead;
	  }
     }
   else
      /* l'element n'a aucun voisin. On cherche le premier ascendant qui */
      /* ait un pave dans la vue */
     {
	pAsc = pEl->ElParent;
	trouve = FALSE;
	while (pAsc != NULL && !trouve)
	   if (pAsc->ElAbstractBox[vue - 1] == NULL)
	      pAsc = pAsc->ElParent;
	   else
	      trouve = TRUE;
	if (trouve)
	   if (pAsc->ElAbstractBox[vue - 1]->AbInLine ||
	       ((!pAsc->ElAbstractBox[vue - 1]->AbTruncatedHead) &&
		(!pAsc->ElAbstractBox[vue - 1]->AbTruncatedTail)))
	      /* le premier pave englobant est complet */
	     {
		result = TRUE;
		fini = TRUE;
	     }
     }
   if (!fini && pEl->ElParent == NULL)
      /* c'est un element racine. Il s'agit donc d'une vue qu'on cree */
      /* entierement */
     {
	result = TRUE;
	fini = TRUE;
     }
   if (!fini && pAbbRoot != NULL)
      /* on regarde si notre element est entre l'element qui possede le */
      /* premier pave feuille de la vue et celui qui possede le dernier */
     {
	/* cherche le premier pave feuille */
	pAb = pAbbRoot;
	while (pAb->AbFirstEnclosed != NULL)
	   pAb = pAb->AbFirstEnclosed;
	if (ElemIsBefore (pEl, pAb->AbElement))
	   /* notre element se trouve avant l'element qui a la premiere */
	   /* feuille */
	   if (!pAbbRoot->AbTruncatedHead)
	      /* l'image est complete en tete,on creera l'image de l'element */
	      result = TRUE;
	   else
	      /* il est hors de l'image */
	      result = FALSE;
	else
	   /* notre element se trouve apres l'element qui a la premiere */
	   /* feuille, comment se situe-t-il par rapport a l'element qui */
	   /* a la derniere feuille de l'image ? */
	  {
	     /* cherche d'abord le dernier pave feuille */
	     pAb = pAbbRoot;
	     while (pAb->AbFirstEnclosed != NULL)
	       {
		  pAb = pAb->AbFirstEnclosed;
		  while (pAb->AbNext != NULL)
		     pAb = pAb->AbNext;
	       }
	     if (ElemIsBefore (pEl, pAb->AbElement))
		/* notre element se trouve avant l'element qui a la derniere */
		/* feuille, il est dans l'image */
		result = TRUE;
	     else
		/* notre element se trouve apres l'element qui a la derniere */
		/* feuille */
	     if (!pAbbRoot->AbTruncatedTail)
		/* l'image est complete en queue, on creera l'image de */
		/* l'element */
		result = TRUE;
	     else
		/* il est hors de l'image */
		result = FALSE;
	  }
     }
   return result;
}

/* ---------------------------------------------------------------------- */
/* |    EteintOuAllumeSelection bascule la selection courante dans      | */
/* |    toutes les vues du document pDoc.                               | */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
static void         EteintOuAllumeSelection (PtrDocument pDoc, boolean Allume)

#else  /* __STDC__ */
static void         EteintOuAllumeSelection (pDoc, Allume)
PtrDocument         pDoc;
boolean             Allume;

#endif /* __STDC__ */

{
   int                 vue;
   int                 assoc;

   for (vue = 1; vue <= MAX_VIEW_DOC; vue++)
      if (pDoc->DocView[vue - 1].DvPSchemaView > 0)
	 SwitchSelection (pDoc->DocViewFrame[vue - 1], Allume);
   for (assoc = 1; assoc <= MAX_ASSOC_DOC; assoc++)
      if (pDoc->DocAssocFrame[assoc - 1] > 0)
	 SwitchSelection (pDoc->DocAssocFrame[assoc - 1], Allume);
}


/* ---------------------------------------------------------------------- */
/* | DetruitImage detruit l'image abstraite de toutes les vues          | */
/* |       ouvertes dudocument pDoc                                     | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         DetruitImage (PtrDocument pDoc)
#else  /* __STDC__ */
static void         DetruitImage (pDoc)
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   int                 vue;
   int                 assoc;

   for (vue = 1; vue <= MAX_VIEW_DOC; vue++)
      if (pDoc->DocView[vue - 1].DvPSchemaView > 0)
	 DetruitImageVue (vue, FALSE, pDoc, FALSE);
   for (assoc = 1; assoc <= MAX_ASSOC_DOC; assoc++)
      if (pDoc->DocAssocFrame[assoc - 1] > 0)
	 DetruitImageVue (assoc, TRUE, pDoc, FALSE);
}				/* DetruitImage */


/* ---------------------------------------------------------------------- */
/* | RecreeImageVue recree l'image abstraite de la vue Vue du|          | */
/* |     document pDoc procedure partiellement reprise de               | */
/* |     Aff_Select_Pages du module page.c                              | */
/* |     Vue = numero d'elt assoc si vue associee sinon                 | */
/* |     Vue = numero de vue si vue d'arbre principal                   | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         RecreeImageVue (int Vue, boolean Assoc, PtrDocument pDoc)
#else  /* __STDC__ */
static void         RecreeImageVue (Vue, Assoc, pDoc)
int                 Vue;
boolean             Assoc;
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   PtrElement          pElRacine;
   PtrAbstractBox             pAbbRoot;
   int                 frame, h;
   boolean             complet;

   if (Assoc)
     {
#ifdef __COLPAGE__
	pDoc->DocAssocFreeVolume[Vue - 1] = THOT_MAXINT;
	pDoc->DocAssocNPages[Vue - 1] = 0;
#else  /* __COLPAGE__ */
	pDoc->DocAssocFreeVolume[Vue - 1] = pDoc->DocAssocVolume[Vue - 1];
#endif /* __COLPAGE__ */
	pElRacine = pDoc->DocAssocRoot[Vue - 1];
	pAbbRoot = pElRacine->ElAbstractBox[0];
	frame = pDoc->DocAssocFrame[Vue - 1];
	AbsBoxesCreate (pElRacine, pDoc, 1, TRUE, TRUE, &complet);
	h = 0;
	ChangeConcreteImage (frame, &h, pAbbRoot);
     }
   else
     {
	pElRacine = pDoc->DocRootElement;
#ifdef __COLPAGE__
	pDoc->DocViewFreeVolume[Vue - 1] = THOT_MAXINT;
	pDoc->DocViewNPages[Vue - 1] = 0;
#else  /* __COLPAGE__ */
	pDoc->DocViewFreeVolume[Vue - 1] = pDoc->DocViewVolume[Vue - 1];
#endif /* __COLPAGE__ */
	pAbbRoot = pDoc->DocViewRootAb[Vue - 1];
	frame = pDoc->DocViewFrame[Vue - 1];
	AbsBoxesCreate (pElRacine, pDoc, Vue, TRUE, TRUE, &complet);
	h = 0;
	ChangeConcreteImage (frame, &h, pAbbRoot);
     }
}				/* RecreeImageVue */

/* ---------------------------------------------------------------------- */
/* | RecreeImage recree l'image abstraite de toutes les vues            | */
/* |     ouvertes du document pDoc                                      | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         RecreeImage (PtrDocument pDoc)
#else  /* __STDC__ */
static void         RecreeImage (pDoc)
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   int                 vue;
   int                 assoc;

   for (vue = 1; vue <= MAX_VIEW_DOC; vue++)
      if (pDoc->DocView[vue - 1].DvPSchemaView > 0)
	 RecreeImageVue (vue, FALSE, pDoc);
   for (assoc = 1; assoc <= MAX_ASSOC_DOC; assoc++)
      if (pDoc->DocAssocFrame[assoc - 1] > 0)
	 RecreeImageVue (assoc, TRUE, pDoc);

}				/* RecreeImage */


static void         RedisplayCommand ( /* document */ );

/* ---------------------------------------------------------------------- */
/* |    RedispNewElement affiche un element qui vient d'etre ajoute'    | */
/* |    dans un arbre abstrait.                                         | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                RedispNewElement (Document document, PtrElement newElement, PtrElement sibling, boolean first, boolean creation)

#else  /* __STDC__ */
void                RedispNewElement (document, newElement, sibling, first, creation)
Document            document;
PtrElement          newElement;
PtrElement          sibling;
boolean             first;
boolean             creation;

#endif /* __STDC__ */

{
   PtrDocument         pDoc;
   int                 vue;

   pDoc = LoadedDocument[document - 1];
   if (pDoc == NULL)
      return;
   /* si le document n'a pas de schema de presentation, on ne fait rien */
   if (pDoc->DocSSchema->SsPSchema == NULL)
      return;
   /* si le document est en mode de non calcul de l'image, on ne fait rien */
   if (documentDisplayMode[document - 1] == NoComputedDisplay)
      return;
   if (sibling != NULL)
      /* l'element sibling n'est plus le dernier (ou premier) fils de son pere */
      ChangeFirstLast (sibling, pDoc, first, TRUE);
   /* on ne cree les paves que s'ils tombent dans la partie de l'image */
   /* du document deja construite */
   if (!AssocView (newElement))
      /* nombre de vues du document */
      for (vue = 1; vue <= MAX_VIEW_DOC; vue++)
	{
	   if (pDoc->DocView[vue - 1].DvPSchemaView > 0)
	      /* la vue est ouverte */
	      if (ElemDansImage (newElement, vue, pDoc->DocViewRootAb[vue - 1], pDoc))
		 /* l'element se trouve a l'interieur de l'image deja construite */
		{
		   /* indique qu'il faut creer les paves sans limite de volume */
		   pDoc->DocViewFreeVolume[vue - 1] = THOT_MAXINT;
		   /* cree effectivement les paves du nouvel element dans la vue */
		   CreateNewAbsBoxes (newElement, pDoc, vue);
		}
	}
   else
      /* vue d'elements associes */
   if (pDoc->DocAssocFrame[newElement->ElAssocNum - 1] != 0)
      /* la vue est ouverte */
      if (ElemDansImage (newElement, 1, pDoc->DocAssocRoot[newElement->ElAssocNum - 1]->ElAbstractBox[0], pDoc))
	 /* l'element se trouve a l'interieur de l'image deja construite */
	{
	   /* indique qu'il faut creer les paves sans limite de volume */
	   pDoc->DocAssocFreeVolume[newElement->ElAssocNum - 1] = THOT_MAXINT;
	   /* cree effectivement les paves du nouvel element dans la vue */
	   CreateNewAbsBoxes (newElement, pDoc, 0);
	}
   /* applique les regles retardees concernant les paves cree's */
   ApplDelayedRule (newElement, pDoc);
   AbstractImageUpdated (pDoc);
   RedisplayCommand (document);
   if (creation)
     {
	/* reaffiche les paves qui copient le nouvel element */
	RedisplayCopies (newElement, pDoc, (documentDisplayMode[document - 1] == DisplayImmediately));
	/* reaffiche les numeros suivants qui changent */
	MajNumeros (newElement, newElement, pDoc, (documentDisplayMode[document - 1] == DisplayImmediately));
     }
}

/* ---------------------------------------------------------------------- */
/* |    ChPavModifDansVue change les booleens AbCanBeModified et AbReadOnly   | */
/* |    dans tous les paves de l'element pEl qui appartiennent a la vue | */
/* |    vue. newPavModif donne la nouvelle valeur de AbCanBeModified,          | */
/* |    reaffiche indique si on veut reafficher.                        | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         ChPavModifDansVue (PtrElement pEl, int vue, boolean newPavModif, boolean reaffiche)
#else  /* __STDC__ */
static void         ChPavModifDansVue (pEl, vue, newPavModif, reaffiche)
PtrElement          pEl;
int                 vue;
boolean             newPavModif;
boolean             reaffiche;

#endif /* __STDC__ */
{
   PtrAbstractBox             pAb, pAbbChild;
   boolean             stop;

   pAb = pEl->ElAbstractBox[vue - 1];
   if (pAb != NULL)
     {
	stop = FALSE;
	while (!stop)
	   if (pAb->AbElement != pEl)
	      /* ce n'est pas un pave de l'element, on arrete */
	      stop = TRUE;
	   else
	      /* c'est un pave de l'element, on le traite */
	     {
		pAb->AbReadOnly = !newPavModif;
		if (reaffiche)
		   pAb->AbAspectChange = TRUE;
		if (!pAb->AbPresentationBox)
		   /* c'est le pave principal de l'element */
		  {
		     /* les paves de presentation restent non modifiables */
		     pAb->AbCanBeModified = newPavModif;
		     /* traite les paves de presentation crees par Create et */
		     /* CreateLast */
		     pAbbChild = pAb->AbFirstEnclosed;
		     while (pAbbChild != NULL)
		       {
			  if (pAbbChild->AbElement == pEl)
			     /* c'est un pave de l'element */
			    {
			       pAbbChild->AbReadOnly = !newPavModif;
			       if (reaffiche)
				  pAbbChild->AbAspectChange = TRUE;
			    }
			  pAbbChild = pAbbChild->AbNext;
		       }
		  }
		if (pAb->AbNext != NULL)
		   /* passe au pave suivant */
		   pAb = pAb->AbNext;
		else
		   stop = TRUE;
	     }
     }
}


/* ---------------------------------------------------------------------- */
/* |    ChangePavModif change les booleens AbCanBeModified et AbReadOnly dans | */
/* |    tous les paves existants de l'element pEl et de sa descendance. | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                ChangePavModif (PtrElement pEl, Document document, boolean newPavModif)
#else  /* __STDC__ */
void                ChangePavModif (pEl, document, newPavModif)
PtrElement          pEl;
Document            document;
boolean             newPavModif;

#endif /* __STDC__ */
{
   PtrDocument         pDoc;
   int                 vue;
   PtrElement          pFils;
   boolean             reaffiche;

   pDoc = LoadedDocument[document - 1];
   if (pDoc == NULL)
      return;
   /* si le document n'a pas de schema de presentation, on ne fait rien */
   if (pDoc->DocSSchema->SsPSchema == NULL)
      return;
   /* si le document est en mode de non calcul de l'image, on ne fait rien */
   if (documentDisplayMode[document - 1] == NoComputedDisplay)
      return;
   /* demande au mediateur si une couleur est associee a ReadOnly */
   /* si oui, il faut reafficher les paves modifie's */
   reaffiche = ShowReadOnly ();
   if (!AssocView (pEl))
      /* on traite toutes les vues du document */
      for (vue = 1; vue <= MAX_VIEW_DOC; vue++)
	{
	   /* on traite tous les paves de l'element dans cette vue */
	   ChPavModifDansVue (pEl, vue, newPavModif, reaffiche);
	   if (reaffiche && pEl->ElAbstractBox[vue - 1] != NULL)
	      RedispAbsBox (pEl->ElAbstractBox[vue - 1], LoadedDocument[document - 1]);
	}
   else
      /* vue d'elements associes */
     {
	/* on traite tous les paves de l'element dans cette vue */
	ChPavModifDansVue (pEl, 1, newPavModif, reaffiche);
	if (reaffiche && pEl->ElAbstractBox[0] != NULL)
	   RedispAbsBox (pEl->ElAbstractBox[0], LoadedDocument[document - 1]);
     }
   if (reaffiche)
      /* on fait reafficher pour visualiser le changement de couleur */
     {
	AbstractImageUpdated (LoadedDocument[document - 1]);
	RedisplayCommand (document);
     }
   /* meme traitement pour les fils qui heritent les droits d'acces */
   if (!pEl->ElTerminal)
     {
	pFils = pEl->ElFirstChild;
	while (pFils != NULL)
	  {
	     if (pFils->ElAccess == AccessInherited)
		ChangePavModif (pFils, document, newPavModif);
	     pFils = pFils->ElNext;
	  }
     }
}


/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                NewSelection (Document document, Element element, int firstCharacter, int lastCharacter)
#else  /* __STDC__ */
void                NewSelection (document, element, firstCharacter, lastCharacter)
Document            document;
Element             element;
int                 firstCharacter;
int                 lastCharacter;

#endif /* __STDC__ */
{

   /* annule l'extension precedente */
   documentNewSelection[document - 1].SDElemExt = NULL;
   documentNewSelection[document - 1].SDCarExt = 0;
   /* enregistre cette nouvelle selection */
   documentNewSelection[document - 1].SDSelActive = TRUE;
   documentNewSelection[document - 1].SDElemSel = element;
   documentNewSelection[document - 1].SDPremCar = firstCharacter;
   documentNewSelection[document - 1].SDDerCar = lastCharacter;
}

/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                NewSelectionExtension (Document document, Element element, int lastCharacter)
#else  /* __STDC__ */
void                NewSelectionExtension (document, element, lastCharacter)
Document            document;
Element             element;
int                 lastCharacter;

#endif /* __STDC__ */
{
   /* enregistre cette nouvelle extension de selection */
   documentNewSelection[document - 1].SDElemExt = element;
   documentNewSelection[document - 1].SDCarExt = lastCharacter;
}




/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
boolean             DemandeSelEnregistree (Document document, boolean * annulation)
#else  /* __STDC__ */
boolean             DemandeSelEnregistree (document, annulation)
Document            document;
boolean            *annulation;

#endif /* __STDC__ */
{
   boolean             ret;

   ret = documentNewSelection[document - 1].SDSelActive;
   if (ret)
      *annulation = (documentNewSelection[document - 1].SDElemSel == NULL);
   return ret;
}


/* ---------------------------------------------------------------------- */
/* |    VerifieElementSelectionne verifie si l'element pEl constitue    | */
/* |    l'une des extremite's de la selection courante dans le document | */
/* |    "document" et si oui definit une nouvelle selection, sans cet   | */
/* |    element.                                                        | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         VerifieElementSelectionne (PtrElement pEl, Document document)

#else  /* __STDC__ */
static void         VerifieElementSelectionne (pEl, document)
PtrElement          pEl;
Document            document;

#endif /* __STDC__ */
{
   PtrDocument         pDoc;
   PtrDocument         selDoc;
   PtrElement          premSel, derSel, selEl, selPreced;
   int                 premCar, derCar;
   boolean             ok, changeSelection;

   pDoc = LoadedDocument[document - 1];
   ok = GetCurrentSelection (&selDoc, &premSel, &derSel, &premCar, &derCar);
   if (ok && selDoc == pDoc)
      /* il y a une selection dans le document traite' */
      if (SelContinue)
	 /* la selection est continue */
	{
	   changeSelection = FALSE;
	   if (pEl == premSel)
	      /* l'element est en tete de la selection */
	      if (pEl == derSel)
		 /* la selection contient uniquement l'element, on l'annule */
		 ResetSelection (pDoc);
	      else
		 /* il y a d'autres elements selectionne's, on fait demarrer */
		 /* la nouvelle selection sur l'element suivant */
		{
		   premSel = NextInSelection (premSel, derSel);
		   premCar = 0;
		   changeSelection = TRUE;
		}
	   else
	      /* l'element n'est pas en tete de la selection */
	   if (pEl == derSel)
	      /* l'element est en queue de selection */
	     {
		/* on cherche l'element precedent dans la selection */
		selEl = premSel;
		selPreced = NULL;
		while (selEl != NULL && selEl != derSel)
		  {
		     selPreced = selEl;
		     selEl = NextInSelection (selEl, derSel);
		  }
		if (selPreced != NULL)
		   /* on fait terminer la nouvelle selection sur l'element */
		   /* precedent */
		  {
		     derSel = selPreced;
		     derCar = 0;
		     changeSelection = TRUE;
		  }
	     }
	   else
	     {
		if (ElemIsWithinSubtree (premSel, pEl) && ElemIsWithinSubtree (derSel, pEl))
		   /* la selection est entierement a l'interieur de l'element */
		   /* on annule la selection courante */
		   ResetSelection (pDoc);
	     }
	   if (changeSelection)
	     {
		if (premCar > 1)
		   TtaSelectString (document, (Element) premSel, premCar, 0);
		else
		   TtaSelectElement (document, (Element) premSel);
		if (derSel != premSel)
		   TtaExtendSelection (document, (Element) derSel, derCar);
	     }
	}
      else
	 /* la selection est discontinue */
	{
	   selEl = premSel;
	   /* parcourt tous les elements selectionne' */
	   while (selEl != NULL)
	      if (ElemIsWithinSubtree (pEl, selEl))
		 /* l'element selEl est selectionne' et se trouve dans le */
		 /* sous-arbre de l'element detruit */
		{
		   /* on le retire de la selection */
		   RemoveFromSelection (selEl, pDoc);
		   selEl = NULL;
		}
	      else
		 selEl = NextInSelection (selEl, derSel);
	}
}


/* ---------------------------------------------------------------------- */
/* |    UndisplayElement "desaffiche" un element qui va etre retire'    | */
/* |    de son arbre abstrait.                                          | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                UndisplayElement (PtrElement pEl, Document document)
#else  /* __STDC__ */
void                UndisplayElement (pEl, document)
PtrElement          pEl;
Document            document;

#endif /* __STDC__ */
{
   PtrElement          pNext, pPrevious, pPere, pVoisin, pE, pSS;
   boolean             stop;
   PtrDocument         pDoc;
   int                 SauveHauteurPage;
   int                 assoc;

   pDoc = LoadedDocument[document - 1];
   if (pDoc == NULL)
      return;
   /* si le document n'a pas de schema de presentation, */
   /* on retire simplement l'element de l'arbre abstrait */
   if (pDoc->DocSSchema->SsPSchema == NULL)
     {
	RemoveElement (pEl);
	return;
     }
   /* si le document est en mode de non calcul de l'image, */
   /* on retire simplement l'element de l'arbre abstrait */
   /* sauf si c'est la racine car son pave n'avait pas ete detruit */
   /* il faut donc executer la suite de la procedure */
   if (documentDisplayMode[document - 1] == NoComputedDisplay)
      if (pEl->ElParent == NULL)
	{
	   DestroyAbsBoxes (pEl, pDoc, TRUE);
	   RemoveElement (pEl);
	   if (pEl != pDoc->DocRootElement)
	     {
		for (assoc = 0; assoc < MAX_ASSOC_DOC; assoc++)
		   if (pDoc->DocAssocRoot[assoc] == pEl)
		      break;	/* C'est une racine associee */
		if (assoc == MAX_ASSOC_DOC)
		   /* Ce n'est pas une racine ! */
		   return;
	     }
	}
      else
	{
	   RemoveElement (pEl);
	   return;
	}
   VerifieElementSelectionne (pEl, document);
   /* cherche l'element qui precede l'element a detruire : pPrevious */
   pPrevious = pEl->ElPrevious;
   /* saute les marques de page */
   stop = FALSE;
   do
      if (pPrevious == NULL)
	 stop = TRUE;
      else if (!pPrevious->ElTerminal || pPrevious->ElLeafType != LtPageColBreak)
	 stop = TRUE;
      else
	 pPrevious = pPrevious->ElPrevious;
   while (!(stop));
   /* cherche le premier element apres l'element a detruire : pNext */
   pNext = NextElement (pEl);
   stop = FALSE;
   do
      if (pNext == NULL)
	 stop = TRUE;
      else if (pNext->ElTerminal && pNext->ElLeafType == LtPageColBreak)
	 pNext = NextElement (pNext);
      else
	 stop = TRUE;
   while (!(stop));
   if (pNext == NULL)
      pNext = NextElement (pPrevious);
   DestroyAbsBoxes (pEl, pDoc, TRUE);
   pPere = pEl->ElParent;
   RemoveElement (pEl);
   /* cherche l'element a partir duquel il faudra transmettre les */
   /* compteurs */
   if (pPrevious != NULL)
      pSS = pPrevious;
   else
      pSS = pPere;
   /* met a jour les numeros concerne's */
   pE = pEl;
   if (pNext != NULL)
     {
	/* il ne faut pas reafficher les numeros mis a jour si on est */
	/* en mode d'affichage differe'. Or, lorsque PageHeight != 0, */
	/* MajNumeros ne reaffiche pas les numeros qui changent. */
#ifdef __COLPAGE__
	SauveHauteurPage = BreakPageHeight;
	if (documentDisplayMode[document - 1] == DeferredDisplay)
	   BreakPageHeight = 1;
#else  /* __COLPAGE__ */
	SauveHauteurPage = PageHeight;
	if (documentDisplayMode[document - 1] == DeferredDisplay)
	   PageHeight = 1;
#endif /* __COLPAGE__ */
	while (pE != NULL)
	  {
	     MajNumeros (pNext, pE, pDoc, (documentDisplayMode[document - 1] == DisplayImmediately));
	     pE = pE->ElNext;
	  }
#ifdef __COLPAGE__
	BreakPageHeight = SauveHauteurPage;
#else  /* __COLPAGE__ */
	PageHeight = SauveHauteurPage;
#endif /* __COLPAGE__ */
     }
   if (pNext != NULL)
     {
	pVoisin = pNext->ElPrevious;
	stop = FALSE;
	do
	   if (pVoisin == NULL)
	      stop = TRUE;
	   else if (!pVoisin->ElTerminal
		    || pVoisin->ElLeafType != LtPageColBreak)
	      stop = TRUE;
	   else
	      pVoisin = pVoisin->ElPrevious;
	while (!(stop));
	if (pVoisin == NULL)
	   /* l'element qui suit la partie detruite devient premier */
	   ChangeFirstLast (pNext, pDoc, TRUE, FALSE);
     }
   if (pPrevious != NULL)
     {
	pVoisin = pPrevious->ElNext;
	stop = FALSE;
	do
	   if (pVoisin == NULL)
	      stop = TRUE;
	   else if (!pVoisin->ElTerminal
		    || pVoisin->ElLeafType != LtPageColBreak)
	      stop = TRUE;
	   else
	      pVoisin = pVoisin->ElNext;
	while (!(stop));
	if (pVoisin == NULL)
	   /* l'element qui precede la partie detruite devient dernier */
	   ChangeFirstLast (pPrevious, pDoc, FALSE, FALSE);
	/* traitement particulier aux tableaux */
      if (ThotLocalActions[T_createhairline]!=NULL)
        (*ThotLocalActions[T_createhairline])(pPrevious, pEl, pDoc);
     }
   /* reevalue l'image de toutes les vues */
   AbstractImageUpdated (pDoc);
   RedisplayCommand (document);
   if (pEl != NULL)
     {
	/* reaffiche les paves qui copient les elements detruits */
	RedisplayCopies (pEl, pDoc, (documentDisplayMode[document - 1] == DisplayImmediately));
	/* la renumerotation est faite plus haut */
	/* reaffiche les references aux elements detruits */
	/* et enregistre les references sortantes coupees */
	/* ainsi que les elements coupe's qui sont reference's par */
	/* d'autres documents */
	RedisplayEmptyReferences (pEl, &pDoc, (documentDisplayMode[document - 1] == DisplayImmediately));
	/* Retransmet les valeurs des compteurs et attributs TRANSMIT */
	/* s'il y a des elements apres */
	if (pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrRefImportedDoc)
	   RepApplyTransmitRules (pEl, pSS, pDoc);
     }
}


/* ---------------------------------------------------------------------- */
/* |    DisplayHolophrasted reaffiche un element sous forme holphrastee | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                DisplayHolophrasted (PtrElement pEl, Document document)
#else  /* __STDC__ */
void                DisplayHolophrasted (pEl, document)
PtrElement          pEl;
Document            document;

#endif /* __STDC__ */
{
   PtrDocument         pDoc;
   int                 vue;

   pDoc = LoadedDocument[document - 1];
   if (pDoc == NULL)
      return;
   /* si le document n'a pas de schema de presentation, on ne fait rien */
   if (pDoc->DocSSchema->SsPSchema == NULL)
      return;
   /* si le document est en mode de non calcul de l'image, on ne fait rien */
   if (documentDisplayMode[document - 1] == NoComputedDisplay)
      return;
   /* Detruit les paves de l'element */
   DestroyAbsBoxes (pEl, pDoc, TRUE);
   /* reevalue l'image de toutes les vues */
   AbstractImageUpdated (pDoc);
   /* on ne cree les paves que s'ils tombent dans la partie de l'image */
   /* du document deja construite */
   if (!AssocView (pEl))
      /* nombre de vues du document */
      for (vue = 1; vue <= MAX_VIEW_DOC; vue++)
	{
	   if (pDoc->DocView[vue - 1].DvPSchemaView > 0)
	      /* la vue est ouverte */
	      if (ElemDansImage (pEl, vue, pDoc->DocViewRootAb[vue - 1], pDoc))
		 /* l'element se trouve a l'interieur de l'image deja construite */
		{
		   /* indique qu'il faut creer les paves sans limite de volume */
		   pDoc->DocViewFreeVolume[vue - 1] = THOT_MAXINT;
		   /* cree effectivement les paves du nouvel element dans la vue */
		   CreateNewAbsBoxes (pEl, pDoc, vue);
		}
	}
   else
      /* vue d'elements associes */
   if (pDoc->DocAssocFrame[pEl->ElAssocNum - 1] != 0)
      /* la vue est ouverte */
      if (ElemDansImage (pEl, 1, pDoc->DocAssocRoot[pEl->ElAssocNum - 1]->ElAbstractBox[0], pDoc))
	 /* l'element se trouve a l'interieur de l'image deja construite */
	{
	   /* indique qu'il faut creer les paves sans limite de volume */
	   pDoc->DocAssocFreeVolume[pEl->ElAssocNum - 1] = THOT_MAXINT;
	   /* cree effectivement les paves du nouvel element dans la vue */
	   CreateNewAbsBoxes (pEl, pDoc, 0);
	}
   AbstractImageUpdated (pDoc);
   /* pas d'operation de reaffichage secondaires */
   RedisplayCommand (document);
}


/* ---------------------------------------------------------------------- */
/* |    HideElement "desaffiche" un element qui devient invisible       | */
/* |    mais n'est pas detruit.                                         | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                HideElement (PtrElement pEl, Document document)
#else  /* __STDC__ */
void                HideElement (pEl, document)
PtrElement          pEl;
Document            document;

#endif /* __STDC__ */
{
   PtrDocument         pDoc;
   PtrElement          pFils;

   pDoc = LoadedDocument[document - 1];
   if (pDoc == NULL)
      return;
   /* si le document n'a pas de schema de presentation, on ne fait rien */
   if (pDoc->DocSSchema->SsPSchema == NULL)
      return;
   /* si le document est en mode de non calcul de l'image, on ne fait rien */
   if (documentDisplayMode[document - 1] == NoComputedDisplay)
      return;
   VerifieElementSelectionne (pEl, document);
   if (pEl->ElParent == NULL)
      /* c'est la racine d'un arbre, on detruit les paves des fils */
      /* pour garder au moins le pave racine */
     {
	if (!pEl->ElTerminal)
	  {
	     pFils = pEl->ElFirstChild;
	     while (pFils != NULL)
	       {
		  DestroyAbsBoxes (pFils, pDoc, TRUE);
		  pFils = pFils->ElNext;
	       }
	  }
     }
   else
      /* ce n'est pas une racine, on detruit les paves de l'element */
      DestroyAbsBoxes (pEl, pDoc, TRUE);
   /* reevalue l'image de toutes les vues */
   AbstractImageUpdated (pDoc);
   /* pas d'operation de reaffichage secondaires */
   RedisplayCommand (document);
}


/* ---------------------------------------------------------------------- */
/* |    RedispReference reaffiche une reference qui vient d'etre        | */
/* |    modifiee.                                                       | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                RedispReference (PtrElement element, Document document)
#else  /* __STDC__ */
void                RedispReference (element, document)
PtrElement          element;
Document            document;

#endif /* __STDC__ */
{
   PtrDocument         pDoc;

   pDoc = LoadedDocument[document - 1];
   if (pDoc == NULL)
      return;
   /* si le document n'a pas de schema de presentation, on ne fait rien */
   if (pDoc->DocSSchema->SsPSchema == NULL)
      return;
   /* si le document est en mode de non calcul de l'image, on ne fait rien */
   if (documentDisplayMode[document - 1] == NoComputedDisplay)
      return;
   /* supprime les anciens paves de la reference */
   DestroyAbsBoxes (element, pDoc, FALSE);
   AbstractImageUpdated (pDoc);
   ApplyTransmitRules (element, pDoc);
   RepApplyTransmitRules (element, element, pDoc);
   CreateAllAbsBoxesOfEl (element, pDoc);
   AbstractImageUpdated (pDoc);
   RedisplayCommand (document);
   /* reaffiche les paves qui copient l'element */
   RedisplayCopies (element, pDoc, (documentDisplayMode[document - 1] == DisplayImmediately));
}

/* ---------------------------------------------------------------------- */
/* |    RedispLeaf reaffiche le contenu d'une feuille.                  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                RedispLeaf (PtrElement element, Document document, int delta)
#else  /* __STDC__ */
void                RedispLeaf (element, document, delta)
PtrElement          element;
Document            document;
int                 delta;

#endif /* __STDC__ */
{
   PtrDocument         pDoc;
   int                 v, frame, h;
   PtrAbstractBox             pAbb;
   boolean             modif;
   PtrAbstractBox             pAbbox1;

   pDoc = LoadedDocument[document - 1];
   if (pDoc == NULL)
      return;
   /* si le document n'a pas de schema de presentation, on ne fait rien */
   if (pDoc->DocSSchema->SsPSchema == NULL)
      return;
   /* si le document est en mode de non calcul de l'image, on ne fait rien */
   if (documentDisplayMode[document - 1] == NoComputedDisplay)
      return;
   modif = FALSE;
   for (v = 1; v <= MAX_VIEW_DOC; v++)
      if (element->ElAbstractBox[v - 1] != NULL
	  && !element->ElAbstractBox[v - 1]->AbNew
	  && !element->ElAbstractBox[v - 1]->AbDead)
	 /* un pave correspondant existe dans la vue v */
	 /* met a jour le volume dans les paves englobants */
	{
	   if (delta != 0)
	     {
		pAbb = element->ElAbstractBox[v - 1]->AbEnclosing;
		while (pAbb != NULL)
		  {
		     pAbb->AbVolume += delta;
		     pAbb = pAbb->AbEnclosing;
		  }
	     }
	   if (AssocView (element))
	      pDoc->DocAssocModifiedAb[element->ElAssocNum - 1] = NULL;
	   else
	      pDoc->DocViewModifiedAb[v - 1] = NULL;
	   /* met a jour le contenu et le volume et demande le */
	   /* reaffichage du pave */
	   pAbbox1 = element->ElAbstractBox[v - 1];
	   /* saute les paves de presentation */
	   while (pAbbox1->AbElement == element && pAbbox1->AbPresentationBox &&
		  pAbbox1->AbNext != NULL)
	      pAbbox1 = pAbbox1->AbNext;
	   pAbbox1->AbVolume += delta;
	   pAbbox1->AbChange = TRUE;
	   switch (element->ElLeafType)
		 {
		    case LtPicture:
		       /* Rien a faire */
		       break;
		    case LtText:
		       pAbbox1->AbText = element->ElText;
		       pAbbox1->AbLanguage = element->ElLanguage;
		       break;
		    case LtPlyLine:
		       pAbbox1->AbLeafType = LtPlyLine;
		       pAbbox1->AbPolyLineBuffer = element->ElPolyLineBuffer;
		       pAbbox1->AbPolyLineShape = element->ElPolyLineType;
		       pAbbox1->AbVolume = element->ElNPoints;
		       break;
		    case LtSymbol:
		    case LtGraphics:
		       pAbbox1->AbLeafType = LtGraphics;
		       pAbbox1->AbShape = element->ElGraph;
		       pAbbox1->AbGraphAlphabet = 'G';
		       break;
		    default:
		       break;
		 }
	   /* memorise le pave a reafficher */
	   if (AssocView (element))
	      pDoc->DocAssocModifiedAb[element->ElAssocNum - 1] = element->ElAbstractBox[v - 1];
	   else
	      pDoc->DocViewModifiedAb[v - 1] = element->ElAbstractBox[v - 1];
	   modif = TRUE;
	}
   if (modif)
      /* ajuste le volume dans toutes les vues, ce qui peut modifier */
      /* le sous-arbre a reafficher */
     {
	/* reevalue l'image des vues modifiees */
	for (v = 1; v <= MAX_VIEW_DOC; v++)
	   if (element->ElAbstractBox[v - 1] != NULL)
	     {
		/* un pave correspondant existe dans la vue v */
		if (AssocView (element))
		  {
		     /* vue d'element associe */
		     frame = pDoc->DocAssocFrame[element->ElAssocNum - 1];
		     pAbbox1 = pDoc->DocAssocModifiedAb[element->ElAssocNum - 1];
		  }
		else
		  {
		     frame = pDoc->DocViewFrame[v - 1];
		     pAbbox1 = pDoc->DocViewModifiedAb[v - 1];
		  }

		if (pAbbox1 != NULL)
		  {
		     h = 0;
		     /* on ne s'occupe pas de la hauteur de page */
		     ChangeConcreteImage (frame, &h, pAbbox1);
		  }
	     }
     }
   RedisplayCommand (document);
   /* si l'element modifie' appartient soit a un element copie' */
   /* dans des paves par une regle Copy, soit a un element inclus */
   /* dans d'autres, il faut reafficher ses copies */
   RedisplayCopies (element, pDoc, (documentDisplayMode[document - 1] == DisplayImmediately));
}


/* ---------------------------------------------------------------------- */
/* |    RedispSplittedText reaffiche le contenu d'un texte divise'.     | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                RedispSplittedText (PtrElement element, int position, Document document)

#else  /* __STDC__ */
void                RedispSplittedText (element, position, document)
PtrElement          element;
int                 position;
Document            document;

#endif /* __STDC__ */

{
   PtrElement          pEl;
   PtrDocument         pDoc;
   int                 vue, dvol;
   PtrAbstractBox             pAb;

   pDoc = LoadedDocument[document - 1];
   if (pDoc == NULL)
      return;
   /* si le document n'a pas de schema de presentation, on ne fait rien */
   if (pDoc->DocSSchema->SsPSchema == NULL)
      return;
   pEl = (PtrElement) element;
   /* check current selection */
   if (SelectedDocument == pDoc)
     {
	/* current selection is in that document */
	if (pEl == LastSelectedElement)
	   if (position < LastSelectedChar)
	     {
		LastSelectedElement = ((PtrElement) element)->ElNext;
		LastSelectedChar -= position;
	     }
	if ((PtrElement) element == FirstSelectedElement)
	   if (position < FirstSelectedChar)
	     {
		FirstSelectedElement = ((PtrElement) element)->ElNext;
		FirstSelectedChar -= position;
	     }
     }
   /* si le document est en mode de non calcul de l'image, on ne fait rien */
   if (documentDisplayMode[document - 1] == NoComputedDisplay)
      return;
   if (pEl->ElNext != NULL)
      /* cree les paves du nouvel element de texte */
      CreateAllAbsBoxesOfEl (pEl->ElNext, pDoc);
   /* met a jour le volume des paves du premier element de texte */
   for (vue = 1; vue <= MAX_VIEW_DOC; vue++)
     {
	pAb = pEl->ElAbstractBox[vue - 1];
	if (pAb != NULL)
	  {
	     dvol = pEl->ElTextLength - pAb->AbVolume;
	     pAb->AbVolume += dvol;
	     pAb->AbChange = TRUE;
	     if (!AssocView (pEl))
		pDoc->DocViewModifiedAb[vue - 1] =
		   Enclosing (pAb, pDoc->DocViewModifiedAb[vue - 1]);
	     else
		pDoc->DocAssocModifiedAb[pEl->ElAssocNum - 1] =
		   Enclosing (pAb, pDoc->DocAssocModifiedAb[pEl->ElAssocNum - 1]);
	  }
     }
   AbstractImageUpdated (pDoc);
   RedisplayCommand (document);
}

/* ---------------------------------------------------------------------- */
/* |    RedispMergedText reaffiche le contenu d'un texte fusionne'.     | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                RedispMergedText (PtrElement element, Document document)

#else  /* __STDC__ */
void                RedispMergedText (element, document)
PtrElement          element;
Document            document;

#endif /* __STDC__ */

{
   PtrElement          pEl, pEVoisin;
   boolean             stop;
   PtrDocument         pDoc;
   int                 vue, dvol, h, frame;
   PtrAbstractBox             pAb;

   pDoc = LoadedDocument[document - 1];
   if (pDoc == NULL)
      return;
   /* si le document n'a pas de schema de presentation, on ne fait rien */
   if (pDoc->DocSSchema->SsPSchema == NULL)
      return;
   pEl = element;
   /* si le document est en mode de non calcul de l'image, on ne fait rien */
   if (documentDisplayMode[document - 1] == NoComputedDisplay)
      return;
   /* teste si pEl est le dernier fils de son pere, */
   /* abstraction faite des marques de page */
   pEVoisin = pEl->ElNext;
   stop = FALSE;
   do
      if (pEVoisin == NULL)
	 /* pEl devient le dernier fils de son pere */
	{
	   ChangeFirstLast (pEl, pDoc, FALSE, FALSE);
	   stop = TRUE;
	}
      else if (!pEVoisin->ElTerminal || pEVoisin->ElLeafType != LtPageColBreak)
	 stop = TRUE;
      else
	 pEVoisin = pEVoisin->ElNext;
   while (!(stop));
   /* met a jour le volume des paves correspondants */
   for (vue = 1; vue <= MAX_VIEW_DOC; vue++)
     {
	pAb = pEl->ElAbstractBox[vue - 1];
	if (pAb != NULL)
	  {
	     dvol = pEl->ElTextLength - pAb->AbVolume;
	     pAb->AbVolume += dvol;
	     pAb->AbChange = TRUE;
	     if (!AssocView (pEl))
	       {
		  pDoc->DocViewModifiedAb[vue - 1] =
		     Enclosing (pAb, pDoc->DocViewModifiedAb[vue - 1]);
		  frame = pDoc->DocViewFrame[vue - 1];
	       }
	     else
	       {
		  pDoc->DocAssocModifiedAb[pEl->ElAssocNum - 1] =
		     Enclosing (pAb, pDoc->DocAssocModifiedAb[pEl->ElAssocNum - 1]);
		  frame = pDoc->DocAssocFrame[pEl->ElAssocNum - 1];
	       }
	     h = 0;		/* on ne s'occupe pas de la hauteur de page */
	     ChangeConcreteImage (frame, &h, pAb->AbEnclosing);
	     if (pAb->AbDead && pAb->AbNext != NULL)
		pAb->AbNext->AbVolume += dvol;
	     do
	       {
		  pAb->AbVolume += dvol;
		  pAb = pAb->AbEnclosing;
	       }
	     while (!(pAb == NULL));
	  }
     }
   RedisplayCommand (document);
}

/* ---------------------------------------------------------------------- */
/* |    UndisplayHeritAttr      supprime sur l'element pEl et son       | */
/* |    sous-arbre la presentation liee a l'attribut decrit par pAttr   | */
/* |    ou a l'heritage d'un tel attribut.                              | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                UndisplayHeritAttr (PtrElement pEl, PtrAttribute pAttr, Document document, boolean suppression)

#else  /* __STDC__ */
void                UndisplayHeritAttr (pEl, pAttr, document, suppression)
PtrElement          pEl;
PtrAttribute         pAttr;
Document            document;
boolean             suppression;

#endif /* __STDC__ */

{
   boolean             heritage, comparaison;
   PtrAttribute         pAttrAsc;
   PtrAttribute         pOldAttr;
   PtrElement          pElFils, pElAttr;

   if (LoadedDocument[document - 1] == NULL)
      return;
   /* si le document n'a pas de schema de presentation, on ne fait rien */
   if (LoadedDocument[document - 1]->DocSSchema->SsPSchema == NULL)
      return;
   /* si le document est en mode de non calcul de l'image, on ne fait rien */
   if (documentDisplayMode[document - 1] == NoComputedDisplay)
      return;
   if (suppression)
      pOldAttr = pAttr;
   else
      /* l'element porte-t-il deja un attribut du meme type ? */
      pOldAttr = AttributeValue (pEl, pAttr);
   /* doit-on se preoccuper des heritages et comparaisons d'attributs? */
   heritage = (pAttr->AeAttrSSchema->SsPSchema->
	       PsNHeirElems[pAttr->AeAttrNum - 1] > 0);
   comparaison = (pAttr->AeAttrSSchema->SsPSchema->
		  PsNComparAttrs[pAttr->AeAttrNum - 1] > 0);
   if (heritage || comparaison)
      /* cherche le premier attribut de meme type pose' sur un ascendant */
      /* de pEl */
      pAttrAsc = GetTypedAttrAncestor (pEl, pAttr->AeAttrNum,
				     pAttr->AeAttrSSchema, &pElAttr);
   else
      pAttrAsc = NULL;
   if (pOldAttr != NULL)
      /* oui, on s'en occupe */
     {
	/* on supprime d'abordles regles de presentation liees */
	/* a l'attribut sur l'element lui-meme */
	RemoveAttrPresentation (pEl, LoadedDocument[document - 1], pOldAttr, FALSE, NULL);
	/* puis on supprime sur pEl et sur les elements du sous arbre pEl */
	/* les regles de presentation liees a l'heritage de cet attribut */
	/* par le sous-arbre s'il existe des elements heritants de celui-ci */
	if (heritage)
	   RemoveInheritedAttrPresent (pEl, LoadedDocument[document - 1], pOldAttr);
	/* puis on supprime sur les elements du sous-arbre pEl */
	/* les regles de presentation liees a la comparaison d'un attribut */
	/* du sous-arbre avec ce type d'attribut */
	if (!pEl->ElTerminal && comparaison)
	   for (pElFils = pEl->ElFirstChild; pElFils != NULL;
		pElFils = pElFils->ElNext)
	      RemoveComparAttrPresent (pElFils, LoadedDocument[document - 1], pOldAttr);
     }
   else if (pAttrAsc != NULL)
     {
	/* heritage et comparaison sont lies a un attribut porte' par un */
	/* ascendant de pEl */
	/* on supprime sur le sous arbre pEl les regles de presentation */
	/* liees a l'heritage de cet attribut par le sous-arbre s'il */
	/* existe des elements heritants de celui-ci */
	if (heritage)
	   RemoveInheritedAttrPresent (pEl, LoadedDocument[document - 1], pAttrAsc);
	/* puis on supprime sur le sous-arbre pEl les regles de */
	/* presentation liees a la comparaison d'un attribut */
	/* du sous-arbre avec ce type d'attribut */
	if (comparaison)
	   RemoveComparAttrPresent (pEl, LoadedDocument[document - 1], pAttrAsc);
     }
}


/* ---------------------------------------------------------------------- */
/* |    DisplayAttribute                                                | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                DisplayAttribute (PtrElement pEl, PtrAttribute pAttr, Document document)

#else  /* __STDC__ */
void                DisplayAttribute (pEl, pAttr, document)
PtrElement          pEl;
PtrAttribute         pAttr;
Document            document;

#endif /* __STDC__ */

{
   boolean             heritage, comparaison;
   PtrElement          pElFils;

   if (LoadedDocument[document - 1] == NULL)
      return;
   /* si le document n'a pas de schema de presentation, on ne fait rien */
   if (LoadedDocument[document - 1]->DocSSchema->SsPSchema == NULL)
      return;
   /* si le document est en mode de non calcul de l'image, on ne fait rien */
   if (documentDisplayMode[document - 1] == NoComputedDisplay)
      return;
   /* doit-on se preoccuper des heritages et comparaisons d'attributs? */
   heritage = (pAttr->AeAttrSSchema->SsPSchema->
	       PsNHeirElems[pAttr->AeAttrNum - 1] > 0);
   comparaison = (pAttr->AeAttrSSchema->SsPSchema->
		  PsNComparAttrs[pAttr->AeAttrNum - 1] > 0);
   /* d'abord on applique les regles de presentation liees */
   /* a l'attribut sur l'element lui-meme */
   ApplyAttrPRulesToElem (pEl, LoadedDocument[document - 1], pAttr, FALSE);
   /* puis on applique sur pEl et les elements du sous-arbre pEl */
   /* les regles de presentation liees a l'heritage de cet attribut */
   /* par le sous arbre s'il existe des elements heritants de celui-ci */
   if (heritage)
      ApplyAttrPRulesToSubtree (pEl, LoadedDocument[document - 1], pAttr);
   /* puis on applique sur les elements du sous arbre pEl */
   /* les regles de presentation liees a la comparaison d'un attribut */
   /* du sous-arbre avec cetype d'attribut */
   if (!pEl->ElTerminal && comparaison)
      for (pElFils = pEl->ElFirstChild; pElFils != NULL; pElFils = pElFils->ElNext)
	 ApplyAttrPRules (pElFils, LoadedDocument[document - 1], pAttr);
   if (pAttr->AeAttrType == AtNumAttr)
      /* s'il s'agit d'un attribut initialisant un compteur, il */
      /* faut mettre a jour les boites utilisant ce compteur */
      UpdateCountersByAttr (pEl, pAttr, LoadedDocument[document - 1]);
   /* on applique les regles retardee */
   ApplDelayedRule (pEl, LoadedDocument[document - 1]);
   AbstractImageUpdated (LoadedDocument[document - 1]);
   RedisplayCommand (document);
   /* le nouvel attribut doit etre pris en compte dans */
   /* les copies-inclusions de l'element */
   RedisplayCopies (pEl, LoadedDocument[document - 1], (documentDisplayMode[document - 1] == DisplayImmediately));
}


/* ---------------------------------------------------------------------- */
/* |    UndisplayAttribute                                              | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                UndisplayAttribute (PtrElement pEl, PtrAttribute pAttr, Document document)

#else  /* __STDC__ */
void                UndisplayAttribute (pEl, pAttr, document)
PtrElement          pEl;
PtrAttribute         pAttr;
Document            document;

#endif /* __STDC__ */

{
   boolean             heritage, comparaison;
   PtrAttribute         pAttrAsc;
   PtrElement          pElAttr;

   if (LoadedDocument[document - 1] == NULL)
      return;
   /* si le document n'a pas de schema de presentation, on ne fait rien */
   if (LoadedDocument[document - 1]->DocSSchema->SsPSchema == NULL)
      return;
   /* si le document est en mode de non calcul de l'image, on ne fait rien */
   if (documentDisplayMode[document - 1] == NoComputedDisplay)
      return;
   /* doit-on se preoccuper des heritages et comparaisons d'attributs? */
   heritage = (pAttr->AeAttrSSchema->SsPSchema->
	       PsNHeirElems[pAttr->AeAttrNum - 1] > 0);
   comparaison = (pAttr->AeAttrSSchema->SsPSchema->
		  PsNComparAttrs[pAttr->AeAttrNum - 1] > 0);
   if (heritage || comparaison)
      /* cherche le premier attribut de meme type pose' sur un ascendant */
      /* de pEl */
      pAttrAsc = GetTypedAttrAncestor (pEl, pAttr->AeAttrNum,
				     pAttr->AeAttrSSchema, &pElAttr);
   else
      pAttrAsc = NULL;
   if (pAttrAsc != NULL)
     {
	/* les regles viennent maintenant de l'heritage ou */
	/* de la comparaison a un ascendant */
	/* on applique sur les elements du sous arbre pEl  */
	/* les regles de presentation liees a l'heritage de cet attribut */
	/* par le sous-arbre s'il existe des elements heritants de celui-ci */
	ApplyAttrPRulesToSubtree (pEl, LoadedDocument[document - 1], pAttrAsc);

	/* puis on applique sur les elements du sous-arbre pEl */
	/* les regles de presentation liees a la comparaison d'un attribut */
	/* du sous-arbre avec ce type d'attribut */
	ApplyAttrPRules (pEl, LoadedDocument[document - 1], pAttrAsc);
     }
   AbstractImageUpdated (LoadedDocument[document - 1]);
   RedisplayCommand (document);
   /* le nouvel attribut doit etre pris en compte dans */
   /* les copies-inclusions de l'element */
   RedisplayCopies (pEl, LoadedDocument[document - 1], (documentDisplayMode[document - 1] == DisplayImmediately));
}


/* ---------------------------------------------------------------------- */
/* |    RedispNewPresSpec                                               | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                RedispNewPresSpec (Document document, PtrElement pEl, PtrPRule pRegle)

#else  /* __STDC__ */
void                RedispNewPresSpec (document, pEl, pRegle)
Document            document;
PtrElement          pEl;
PtrPRule        pRegle;

#endif /* __STDC__ */

{
   if (LoadedDocument[document - 1] == NULL)
      return;
   /* si le document n'a pas de schema de presentation, on ne fait rien */
   if (LoadedDocument[document - 1]->DocSSchema->SsPSchema == NULL)
      return;
   /* si le document est en mode de non calcul de l'image, on ne fait rien */
   if (documentDisplayMode[document - 1] == NoComputedDisplay)
      return;
   ApplNouvRegle (LoadedDocument[document - 1], pRegle, pEl);
   AbstractImageUpdated (LoadedDocument[document - 1]);
   RedisplayCommand (document);
   /* la nouvelle regle de presentation doit etre prise en compte dans */
   /* les copies-inclusions de l'element */
   RedisplayCopies (pEl, LoadedDocument[document - 1], (documentDisplayMode[document - 1] == DisplayImmediately));
}

/* ---------------------------------------------------------------------- */
/* |    RedispPresStandard                                              | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                RedispPresStandard (Document document, PtrElement pEl, PRuleType typeRegleP, int vue)

#else  /* __STDC__ */
void                RedispPresStandard (document, pEl, typeRegleP, vue)
Document            document;
PtrElement          pEl;
PRuleType           typeRegleP;
int                 vue;

#endif /* __STDC__ */

{

   if (LoadedDocument[document - 1] == NULL)
      return;
   /* si le document n'a pas de schema de presentation, on ne fait rien */
   if (LoadedDocument[document - 1]->DocSSchema->SsPSchema == NULL)
      return;
   /* si le document est en mode de non calcul de l'image, on ne fait rien */
   if (documentDisplayMode[document - 1] == NoComputedDisplay)
      return;
   AppliqueRegleStandard (pEl, LoadedDocument[document - 1], typeRegleP, vue);
   AbstractImageUpdated (LoadedDocument[document - 1]);
   RedisplayCommand (document);
   /* le retrait de la regle de presentation doit etre pris en compte */
   /* dans les copies-inclusions de l'element */
   RedisplayCopies (pEl, LoadedDocument[document - 1], (documentDisplayMode[document - 1] == DisplayImmediately));
}

/* ---------------------------------------------------------------------- */
/* |    RedispNewGeometry                                               | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                RedispNewGeometry (Document document, PtrElement pEl)

#else  /* __STDC__ */
void                RedispNewGeometry (document, pEl)
Document            document;
PtrElement          pEl;

#endif /* __STDC__ */

{
   if (LoadedDocument[document - 1] == NULL)
      return;
   /* si le document n'a pas de schema de presentation, on ne fait rien */
   if (LoadedDocument[document - 1]->DocSSchema->SsPSchema == NULL)
      return;
   /* si le document est en mode de non calcul de l'image, on ne fait rien */
   if (documentDisplayMode[document - 1] == NoComputedDisplay)
      return;
   AbstractImageUpdated (LoadedDocument[document - 1]);
   RedisplayCommand (document);
   /* la nouvelle regle de presentation doit etre prise en compte dans */
   /* les copies-inclusions de l'element */
   RedisplayCopies (pEl, LoadedDocument[document - 1], (documentDisplayMode[document - 1] == DisplayImmediately));
}

/* ---------------------------------------------------------------------- */
/* |    RedisplayCommand        Selon le mode d'affichage, execute ou   | */
/* |    met en attente une commande de reaffichage secondaire.          | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         RedisplayCommand (Document document)
#else  /* __STDC__ */
static void         RedisplayCommand (document)
Document            document;

#endif /* __STDC__ */
{
   if (documentDisplayMode[document - 1] == DisplayImmediately)
     {
	/* eteint la selection */
	EteintOuAllumeSelection (LoadedDocument[document - 1], FALSE);
	/* reaffiche ce qui a deja ete prepare' */
	RedisplayDocViews (LoadedDocument[document - 1]);
	/* rallume la selection */
	EteintOuAllumeSelection (LoadedDocument[document - 1], TRUE);
     }
}


/* ----------------------------------------------------------------------
   TtaSetDisplayMode

   Changes display mode for a document. Three display modes are available.
   In the immediate mode, each modification made in the abstract tree of a
   document is immediately reflected in all opened views where the modification
   can be seen.
   In the deferred mode, the programmer can decide when the modifications are
   made visible to the user; this avoids the image of the document to blink when
   several elementary changes are made successively. Modifications are displayed
   when mode is changed to DisplayImmediately.
   In the NoComputedDisplay mode, the modifications are not displayed  and they 
   not computed inside the editor; the execution is more rapid but the current image is 
   lost. When mode is changed to DisplayImmediately or DeferredMode, the image 
   is completely redrawn by the editor.
   An application that handles several documents at the same time can choose
   different modes for different documents. When a document is open or created,
   it is initially in the immediate mode.

   Parameters:
   document: the document.
   NewDisplayMode: new display mode for that document.

   ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtaSetDisplayMode (Document document, DisplayMode newDisplayMode)
#else  /* __STDC__ */
void                TtaSetDisplayMode (document, newDisplayMode)
Document            document;
DisplayMode         newDisplayMode;

#endif /* __STDC__ */
{
   UserErrorCode = 0;
   /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parametre document correct */
     {
	/* si le document n'a pas de schema de presentation, on ne fait rien */
	if (LoadedDocument[document - 1]->DocSSchema->SsPSchema == NULL)
	   return;
	if (documentDisplayMode[document - 1] != newDisplayMode)
	   /* il y a effectivement changement de mode */
	  {
	     if (documentDisplayMode[document - 1] == DisplayImmediately &&
		 (newDisplayMode == DeferredDisplay ||
		  newDisplayMode == NoComputedDisplay))
		/* le document passe en mode affichage differe' ou sans calcul d'image */
	       {
		  /* eteint la selection */
		  EteintOuAllumeSelection (LoadedDocument[document - 1], FALSE);
		  /* si on passe au mode sans calcul d'image il faut detruire l'image */
		  if (newDisplayMode == NoComputedDisplay)
		     DetruitImage (LoadedDocument[document - 1]);
	       }
	     else if ((documentDisplayMode[document - 1] == DeferredDisplay
		  || documentDisplayMode[document - 1] == NoComputedDisplay)
		      && newDisplayMode == DisplayImmediately)
		/* le document passe du mode affichage differe' ou sans calcul  */
		/* d'image au mode  d'affichage immediat */
	       {
		  if (documentDisplayMode[document - 1] == NoComputedDisplay
		      && (!documentNewSelection[document - 1].SDSelActive ||
		      documentNewSelection[document - 1].SDElemSel == NULL))
		     /* il faut recalculer l'image , la suite du code est pareil */
		     RecreeImage (LoadedDocument[document - 1]);
		  /* reaffiche ce qui a deja ete prepare' */
		  if (documentDisplayMode[document - 1] == DeferredDisplay
		      || (!documentNewSelection[document - 1].SDSelActive ||
		      documentNewSelection[document - 1].SDElemSel == NULL))
		     RedisplayDocViews (LoadedDocument[document - 1]);

		  if (!documentNewSelection[document - 1].SDSelActive)
		     /* la selection n'a pas change', on la rallume */
		     EteintOuAllumeSelection (LoadedDocument[document - 1], TRUE);
		  else
		     /* la selection a change', on etablit la selection */
		     /* enregistree */
		    {
		       if (documentNewSelection[document - 1].SDElemSel == NULL)
			  /* c'est une annulation de selection */
			  ResetSelection (LoadedDocument[document - 1]);
		       else
			 {
			    /* il y a effectivement une selection a etablir */
			    if (documentNewSelection[document - 1].SDPremCar == 0 &&
			    documentNewSelection[document - 1].SDDerCar == 0)
			       /* selection d'un element complet */
			       SelectElement (LoadedDocument[document - 1],
					 (PtrElement) (documentNewSelection[document - 1].SDElemSel), TRUE, TRUE);
			    else
			       /* selection d'une chaine */
			       SelectString (LoadedDocument[document - 1],
					     (PtrElement) (documentNewSelection[document - 1].SDElemSel),
			       documentNewSelection[document - 1].SDPremCar,
			       documentNewSelection[document - 1].SDDerCar);
			    /* il n'y a plus de selection a etablir */
			    documentNewSelection[document - 1].SDElemSel = NULL;
			 }
		       /* etablit l'extension de selection enregistree */
		       if (documentNewSelection[document - 1].SDElemExt != NULL)
			  /* il y a une extension de selection a etablir */
			 {
			    ExtendSelection ((PtrElement) (documentNewSelection[document - 1].SDElemExt),
				documentNewSelection[document - 1].SDCarExt,
				      FALSE, FALSE, FALSE);
			    /* il n'y a plus d'extension de selection a etablir */
			    documentNewSelection[document - 1].SDElemExt = NULL;
			 }
		       /* plus de selection a faire pour ce document */
		       documentNewSelection[document - 1].SDSelActive = FALSE;
		    }
	       }
	     else if (documentDisplayMode[document - 1] == DeferredDisplay
		      && newDisplayMode == NoComputedDisplay)
		/* le document passe du mode affichage differe'  */
		/* au mode d'affichage sans calcul d'image  */
		DetruitImage (LoadedDocument[document - 1]);
	     else if (documentDisplayMode[document - 1] == NoComputedDisplay
		      && newDisplayMode == DeferredDisplay)
		/* le document passe du mode affichage sans calcul d'image   */
		/* au mode d'affichage differe'  */
		RecreeImage (LoadedDocument[document - 1]);
	     /* on met a jour le mode d'affichage */
	     documentDisplayMode[document - 1] = newDisplayMode;
	  }
     }
}

/* ----------------------------------------------------------------------
   TtaGetDisplayMode

   Returns the current display mode for a document.

   Parameter:
   document: the document.

   Return value:
   current display mode for that document.

   ---------------------------------------------------------------------- */
#ifdef __STDC__
DisplayMode         TtaGetDisplayMode (Document document)
#else  /* __STDC__ */
DisplayMode         TtaGetDisplayMode (document)
Document            document;

#endif /* __STDC__ */
{
   DisplayMode         result;

   UserErrorCode = 0;
   result = DisplayImmediately;
   /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parametre document correct */
      result = documentDisplayMode[document - 1];
   return result;
}

/* ---------------------------------------------------------------------- */
/* |    SetMainWindowBackgroundColor :                          | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         SetMainWindowBackgroundColor (int frame, int color)
#else  /* __STDC__ */
static void         SetMainWindowBackgroundColor (frame, color)
int                 frame;
int                 color;

#endif /* __STDC__ */
{
   unsigned long       Bgcolor;

#ifndef NEW_WILLOWS
   Bgcolor = ColorPixel (color);
   XSetWindowBackground (TtDisplay, FrRef[frame], Bgcolor);
#endif /* NEW_WILLOWS */
}				/*SetMainWindowBackgroundColor */


/* ---------------------------------------------------------------------- */
/* | TtaResetViewBackgroundColor : reset the background color for a view        | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtaResetViewBackgroundColor (Document doc, View view)
#else  /* __STDC__ */
void                TtaResetViewBackgroundColor (doc, view)
Document            doc;
View                view;

#endif /* __STDC__ */
{
   int                 frame;

   if ((doc <= 0) || (doc > MAX_DOCUMENTS))
      return;
   if ((view <= 0) || (view > MAX_VIEW_DOC))
      return;

   frame = GetWindowNumber (doc, view);
   if (BackgroundColor[frame] != DefaultBColor)
     {
	BackgroundColor[frame] = DefaultBColor;
	SetMainWindowBackgroundColor (frame, DefaultBColor);
     }
}

/* ---------------------------------------------------------------------- */
/* |    TtaSetViewBackgroundColor : set the background color for a view | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtaSetViewBackgroundColor (Document doc, View view, int color)
#else  /* __STDC__ */
void                TtaSetViewBackgroundColor (doc, view, color)
Document            doc;
View                view;
int                 color;

#endif /* __STDC__ */
{
   int                 frame;

   if ((doc <= 0) || (doc > MAX_DOCUMENTS))
      return;
   if ((view <= 0) || (view > MAX_VIEW_DOC))
      return;
   if ((color < 0) || (color >= MAX_COLOR))
      return;

   frame = GetWindowNumber (doc, view);
   if (BackgroundColor[frame] != color)
     {
	BackgroundColor[frame] = color;
	SetMainWindowBackgroundColor (frame, color);
     }
}

/* ---------------------------------------------------------------------- */
/* | TtaGetViewBackgroundColor : returns the background color for a view        | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
int                 TtaGetViewBackgroundColor (Document doc, View view)
#else  /* __STDC__ */
int                 TtaSetViewBackgroundColor (doc, view)
Document            doc;
View                view;

#endif /* __STDC__ */
{
   int                 frame;

   if ((doc <= 0) || (doc > MAX_DOCUMENTS))
      return (-1);
   if ((view <= 0) || (view > MAX_VIEW_DOC))
      return (-1);

   frame = GetWindowNumber (doc, view);
   return (BackgroundColor[frame]);
}

/* Fin du module */
