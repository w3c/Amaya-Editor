/*
 *
 *  (c) COPYRIGHT INRIA, Grif, 1996.
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
 * Authors: V. Quint, I. Vatton (INRIA)
 *          R. Guetari (W3C/INRIA) for Windows 95/NT routines
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "fileaccess.h"
#include "application.h"
#include "document.h"
#include "view.h"
#include "tree.h"
#include "selection.h"
#include "typecorr.h"
#include "thotcolor.h"
#include "appdialogue.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
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
#include "attrpresent_f.h"
#include "attributes_f.h"
#include "context_f.h"
#include "structcreation_f.h"
#include "createabsbox_f.h"
#include "scroll_f.h"
#include "views_f.h"
#include "viewapi_f.h"

#include "absboxes_f.h"
#include "appdialogue_f.h"
#include "applicationapi_f.h"
#include "boxlocate_f.h"
#include "boxparams_f.h"
#include "boxselection_f.h"
#include "buildboxes_f.h"
#include "changeabsbox_f.h"
#include "changepresent_f.h"
#include "fileaccess_f.h"
#include "frame_f.h"
#include "inites_f.h"
#include "paginate_f.h"
#include "presrules_f.h"
#include "structschema_f.h"
#include "structselect_f.h"
#include "schemas_f.h"
#include "structmodif_f.h"
#include "thotmsg_f.h"

extern int          UserErrorCode;

/* descriptor of the selection to do after redosplaying */
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


/*----------------------------------------------------------------------
   GetViewInfo returns wiew number and assoc state of the          
   corresponding to the view of the document.             
   Parameters:                                                     
   document: the document.                                 
   view: the view.                                         
   Return value:                                                   
   corresponding view number.                              
   corresponding assoc state.                              
  ----------------------------------------------------------------------*/
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


/*----------------------------------------------------------------------
   GetWindowNumber returns the window corresponding to the view of 
   the document.                                           
   Parameters:                                                     
   document: the document.                                 
   view: the view.                                         
   Return value:                                                   
   corresponding window.                                   
  ----------------------------------------------------------------------*/
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
   int                 aView, win;

   win = 0;
   /* Checks parameters */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] != NULL)
      if (view < 1 || (view > MAX_VIEW_DOC && view < 100) || view > MAX_ASSOC_DOC + 100)
         TtaError (ERR_invalid_parameter);
      else
        {
	pDoc = LoadedDocument[document - 1];
	GetViewInfo (document, view, &aView, &assoc);
	if (assoc)
	   win = pDoc->DocAssocFrame[aView - 1];
	else
	   win = pDoc->DocViewFrame[aView - 1];
        }
   return win;
}				/*GetWindowNumber */


/*----------------------------------------------------------------------
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

  ----------------------------------------------------------------------*/

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
   int                 nView;
   View                view;

   UserErrorCode = 0;
   view = 0;
   /* Checks the parameter document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parameter document is ok */
     {
	pDoc = LoadedDocument[document - 1];
	if (pDoc->DocSSchema != NULL)
	   if (pDoc->DocSSchema->SsPSchema == NULL)
	      TtaError (ERR_no_presentation_schema);
	   else
	     {
#ifndef __COLPAGE__
		/* Add a pagebreak probably missed at the end of the document */
		if (pDoc->DocSSchema->SsPSchema->PsPaginatedView[0])
		   AddLastPageBreak (pDoc->DocRootElement, 1, pDoc, FALSE);
#endif /* __COLPAGE__ */
		nView = CreateAbstractImage (pDoc, 1, 0, pDoc->DocSSchema, 1, TRUE, NULL);
		OpenCreatedView (pDoc, nView, FALSE, x, y, w, h);
		view = nView;
	     }
     }
   return view;
}


/*----------------------------------------------------------------------
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
  ----------------------------------------------------------------------*/
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
   int                 nView;
   int                 nbViews;
   int                 i;
   int                 v;
   PtrDocument         pDoc;
   AvailableView       allViews;
   boolean             assoc;
   boolean             found;
   View                view;
   boolean             viewHasBeenOpen;

   UserErrorCode = 0;
   view = 0;
   v = 0;
   /* Checks the parameter document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parameter document is ok */
   if (LoadedDocument[document - 1]->DocSSchema->SsPSchema == NULL)
      TtaError (ERR_no_presentation_schema);
   else
     {
	pDoc = LoadedDocument[document - 1];
	assoc = FALSE;
	nView = 0;
	/* Enumerate the list of all pssible views for this document */
	nbViews = BuildDocumentViewList (pDoc, allViews);
	/* Looks in the list for the name of the view to be opened */
	found = FALSE;
	for (i = 0; i < nbViews && !found; i++)
	  {
	     found = strcmp (viewName, allViews[i].VdViewName) == 0;
	     if (found)
		v = i;
	  }
	if (found)
	  {
          viewHasBeenOpen = TRUE;

	     /* Open the view */
	     if (allViews[v].VdAssoc)
	       {
#ifndef __COLPAGE__
		  /* Add a page break probably missed at the end */
		  if (allViews[v].VdView > 0)
		     if (pDoc->DocSSchema->SsPSchema->PsPaginatedView[0])
			AddLastPageBreak (pDoc->DocAssocRoot[allViews[v].VdView - 1], 1, pDoc, FALSE);
#endif /* __COLPAGE__ */
		  nView = CreateAbstractImage (pDoc, 0, allViews[v].VdAssocNum,
		      allViews[v].VdSSchema, 1, TRUE, (PtrElement) subtree);
                  if (pDoc->DocAssocRoot[nView - 1] == NULL)
                    /*** Associated tree creation has been refused. ***/
                    viewHasBeenOpen = FALSE;
		  assoc = TRUE;
	       }
	     else
	       {
#ifndef __COLPAGE__
		  /* Add a page break probably missed at the end */
		  if (pDoc->DocSSchema->SsPSchema->PsPaginatedView[allViews[v].VdView])
		     AddLastPageBreak (pDoc->DocRootElement, allViews[v].VdView, pDoc, FALSE);
#endif /* __COLPAGE__ */
		  nView = CreateAbstractImage (pDoc, allViews[v].VdView, 0,
		     allViews[v].VdSSchema, 1, FALSE, (PtrElement) subtree);
		  assoc = FALSE;
	       }
	     if (nView == 0)
		TtaError (ERR_cannot_open_view);
	     else
	       {
                  if (viewHasBeenOpen)
                    {
		       OpenCreatedView (pDoc, nView, assoc, x, y, w, h);
		       if (assoc)
		          view = nView + 100;
		       else
		          view = nView;
                    }
	       }
	  }
     }
   return view;
}

/*----------------------------------------------------------------------
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

  ----------------------------------------------------------------------*/

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

/*----------------------------------------------------------------------
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

  ----------------------------------------------------------------------*/

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

/*----------------------------------------------------------------------
   TtaCloseView

   Closes a view.

   Parameters:
   document: the document for which a view must be closed.
   view: the view to be closed.

  ----------------------------------------------------------------------*/
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
   /* Checks the parameter document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parameter document is ok */
     {
	pDoc = LoadedDocument[document - 1];
	if (view < 100)
	   /* View of the main tree */
	   if (view < 1 || view > MAX_VIEW_DOC)
	      TtaError (ERR_invalid_parameter);
	   else
	     {
		DestroyFrame (pDoc->DocViewFrame[view - 1]);
		CloseDocumentView (pDoc, view, FALSE, FALSE);
	     }
	else
	   /* View of associated elements */
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

/*----------------------------------------------------------------------
   CleanImageView cleans the abstract image of View corresponding to pDoc. 
   View = view number or assoc. elem. number if assoc. view.      
   complete = TRUE if the window is completely cleaned.           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         CleanImageView (int View, boolean Assoc, PtrDocument pDoc, boolean complete)
#else  /* __STDC__ */
static void         CleanImageView (View, Assoc, pDoc, complete)
int                 View;
boolean             Assoc;
PtrDocument         pDoc;
boolean             complete;
#endif /* __STDC__ */
{
   PtrAbstractBox      pAb;
   int                 h;
   int                 frame;
   PtrAbstractBox      pAbbRoot;

   frame = 1;			/* initialization (for the compiler !) */
   pAbbRoot = NULL;		/* initialization (for the compiler !) */
   if (Assoc)
     {
	/* Associated element view */
	pAbbRoot = pDoc->DocAssocRoot[View - 1]->ElAbstractBox[0];
	frame = pDoc->DocAssocFrame[View - 1];
     }
   else
     {
	pAbbRoot = pDoc->DocViewRootAb[View - 1];
	frame = pDoc->DocViewFrame[View - 1];
     }

   /* All abstract boxes included into the root abs. box are marked dead */
   if (pAbbRoot == NULL)
     return;

   if (complete)
     {
	SetDeadAbsBox (pAbbRoot);
	ChangeConcreteImage (frame, &h, pAbbRoot);
	CloseDocumentView (pDoc, View, Assoc, TRUE);
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
	/* Flush the deferred rules on the root */
	ApplDelayedRule (pAbbRoot->AbFirstEnclosed->AbElement, pDoc);
	/* Realease all dead abstract boxes */
	h = -1;			/* Changing the meaning of h */
#else  /* __COLPAGE__ */
	h = 0;
#endif /* __COLPAGE__ */
	ChangeConcreteImage (frame, &h, pAbbRoot);
	/* Releases all dead abstract boxes of the view */
	FreeDeadAbstractBoxes (pAbbRoot);

	/* Shows that one must apply presentation rules of the root abstract box, for example
	   to rebuild presentaion boxes, created by the root and destroyed */
	pAbbRoot->AbSize = -1;
#ifdef __COLPAGE__
	pAbbRoot->AbTruncatedTail = TRUE;
#endif /* __COLPAGE__ */
	/* The complete root abstract box is marked. This allows  AbsBoxesCreate */
	/* to generate presentation abstract boxes created at the begenning */
	if (pAbbRoot->AbLeafType == LtCompound)
	   pAbbRoot->AbTruncatedHead = FALSE;
     }
}

/*----------------------------------------------------------------------
   TtaFreeView

   frees the view of the document. The window continues to exist but the document
   is no longer displayed in this window.

   Parameters:
   document: the document for which a view must be closed.
   view: the view to be closed.

  ----------------------------------------------------------------------*/
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
   /* Checks the parameter document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parameter document is ok */
     {
	pDoc = LoadedDocument[document - 1];
	if (view < 100)
	   /* View of the main tree */
	   if (view < 1 || view > MAX_VIEW_DOC)
	      TtaError (ERR_invalid_parameter);
	   else
	      CleanImageView (view, FALSE, pDoc, TRUE);
	else
	   /* View of associated elements */
	  {
	     numAssoc = view - 100;
	     if (numAssoc < 1 || numAssoc > MAX_ASSOC_DOC)
		TtaError (ERR_invalid_parameter);
	     else
		CleanImageView (numAssoc, TRUE, pDoc, TRUE);
	  }
     }
}

/*----------------------------------------------------------------------
   TtaChangeViewTitle

   Changes the title of a view.

   Parameters:
   document: the document.
   view: the view.
   title: the new title.

  ----------------------------------------------------------------------*/

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
   /* Checks the parameter document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parameter document is ok */
     {
	pDoc = LoadedDocument[document - 1];
	if (view < 100)
	   /* View of the main tree */
	   if (view < 1 || view > MAX_VIEW_DOC)
	      TtaError (ERR_invalid_parameter);
	   else
	      ChangeFrameTitle (pDoc->DocViewFrame[view - 1], title);
	else
	   /* View of associated elements */
	  {
	     numAssoc = view - 100;
	     if (numAssoc < 1 || numAssoc > MAX_ASSOC_DOC)
		TtaError (ERR_invalid_parameter);
	     else
		ChangeFrameTitle (pDoc->DocAssocFrame[numAssoc - 1], title);
	  }
     }
}


/*----------------------------------------------------------------------
   TtaSetSensibility

   Changes the current sensibility used to display a given view of a
   given document.

   Parameters:
   document: the document. Cannot be 0.
   view: the view.
   value: new value of the sensibility.

  ----------------------------------------------------------------------*/

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
       GetFrameParams (frame, &valvisib, &valzoom);
       if (valvisib < 0 || valvisib > 10)
	 TtaError (ERR_invalid_parameter);
       else
	 {
	   /* Translation of the sensibility into threshold */
	   valvisib = 10 - value;
	   SetFrameParams (frame, valvisib, valzoom);
	 }
     }
   else
     TtaError (ERR_invalid_parameter);
}


/*----------------------------------------------------------------------
   TtaSetZoom

   Changes the current zoom used to display a given
   view of a given document.

   Parameters:
   document: the document. Cannot be 0.
   view: the view.
   value: new value of the zoom.

  ----------------------------------------------------------------------*/
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
      GetFrameParams (frame, &valvisib, &valzoom);
      if (valzoom < 0 || valzoom > 10)
	TtaError (ERR_invalid_parameter);
      else
	{
	  /* Translation of the sensibility into threshold */
	  valzoom = value;
	  SetFrameParams (frame, valvisib, valzoom);
	}
    }
}


/*----------------------------------------------------------------------
   TtaGetFirstElementShown
   Returns the first element in a given view of a given document.

   Parameters:
   document: the document to which the element to be shown belongs.
   Cannot be 0.
   view: the view where the element must be shown.

   Returns:
   element: the first shown element.
   position: position of the top of the element in the window.
   Supposing that the y axis is oriented from the top
   of the window (coordinate 0) to the bottom (coordinate 100,
   whatever the actual height of the window), position is the
   desired y coordinate of the top of the element.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
Element             TtaGetFirstElementShown (Document document, View view, int *position)
#else  /* __STDC__ */
Element             TtaGetFirstElementShown (document, view, position)
Document            document;
View                view;
int                *position;
#endif /* __STDC__ */
{
  ViewFrame          *pFrame;
  PtrElement          pEl;
  PtrAbstractBox      pRootAb;
  PtrBox              pBox;
  int                 frame;
  int                 x, y, charsNumber;
  int                 width, height;

  UserErrorCode = 0;
  *position = 0;
  pEl = NULL;
  frame = GetWindowNumber (document, view);
  if (frame != 0)
    {
      pFrame = &ViewFrameTable[frame - 1];
      x = pFrame->FrXOrg;
      y = pFrame->FrYOrg;
      pRootAb = pFrame->FrAbstractBox;
      if (pRootAb != NULL && pRootAb->AbBox != NULL)
	{
	  charsNumber = 0;
	  /* recupere la boite selectionnee */
	  if (ThotLocalActions[T_selecbox] != NULL)
	    {
	      (*ThotLocalActions[T_selecbox]) (&pBox, pRootAb, frame, x, y, &charsNumber);
	      if (pBox != NULL && pBox->BxAbstractBox != NULL)
		{
		  if (pBox->BxType == BoPiece || pBox->BxType == BoSplit)
		    pBox = pBox->BxAbstractBox->AbBox->BxNexChild;
		  pEl = pBox->BxAbstractBox->AbElement;
		  /* width and height of the frame */
		  GetSizesFrame (frame, &width, &height);
		  /* position of the box top in the frame in % */
		  *position = (pBox->BxYOrg - pFrame->FrYOrg) * 100 / height;
		}
	    }
	}

    }
  return ((Element) pEl);
}


/*----------------------------------------------------------------------
   TtaShowElement
   Shows a given element in a given view of a given document.

   Parameters:
   document: the document to which the element to be shown belongs.
   Cannot be 0.
   view: the view where the element must be shown.
   element: the element to be shown.
   position: position of the top of the element in the window in %.
   Supposing that the y axis is oriented from the top
   of the window (coordinate 0) to the bottom (coordinate 100,
   whatever the actual height of the window), position is the
   desired y coordinate of the top of the element.
  ----------------------------------------------------------------------*/
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
  int                 aView;
  PtrElement          pEl;

  UserErrorCode = 0;
  frame = GetWindowNumber (document, view);
  if (frame != 0)
    if (element == NULL)
      TtaError (ERR_invalid_parameter);
    else
      {
	if (view < 100)
	  aView = view;
	else
	  aView = 1;
	pEl = (PtrElement) element;
	/* If the first abstract box of the element is incomplete, it is suppressed */
	if (pEl->ElAbstractBox[aView - 1] != NULL)
	  if (pEl->ElAbstractBox[aView - 1]->AbLeafType == LtCompound)
	    if (pEl->ElAbstractBox[aView - 1]->AbTruncatedHead)
	      /* Destroying the abstract box of the element in this view */
	      DestroyAbsBoxesView (pEl, LoadedDocument[document - 1], FALSE, aView);
	/* and CheckAbsBox will rebuild it at the beginning of the element */
	CheckAbsBox (pEl, aView, LoadedDocument[document - 1], FALSE, FALSE);
	if (pEl->ElAbstractBox[aView - 1] != NULL)
	  ShowBox (frame, pEl->ElAbstractBox[aView - 1]->AbBox, 0, position);
      }
}


/*----------------------------------------------------------------------
   TtaGetSensibility
   Reads the current sensibility used to display
   a given view of a given document.

   Parameters:
   document: the document. Cannot be 0.
   view: the view.

   Return value:
   current value of the sensibility.
  ----------------------------------------------------------------------*/
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
	GetFrameParams (frame, &valvisib, &valzoom);
	/* Translation of the sensibility into threshold */
	value = 10 - valvisib;
     }
   return value;
}				/*TtaGetSensibility */


/*----------------------------------------------------------------------
   TtaGetZoom

   Reads the current zoom used to display a given view
   of a given document.

   Parameters:
   document: the document. Cannot be 0.
   view: the view.

   Return value:
   current value of the zoom.

  ----------------------------------------------------------------------*/

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
	GetFrameParams (frame, &valvisib, &value);
     }
   return value;
}				/*TtaGetZoom */


/*----------------------------------------------------------------------
   TtaIsPSchemaValid

   Checks if a presentation schema can be applied to a document of a given
   class. No document is needed and the schemas are not loaded by this
   function.

   Parameters:
   structureName: Name of the document class.
   presentationName: Name of the presentation schema to be checked.

   Return value:
   1 if the presentation schema can be applied, 0 if it can not.

  ----------------------------------------------------------------------*/
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
   char                text[MAX_TXT_LEN];
   int                 i;
   Name                gotStructName;
   int                 result;

   UserErrorCode = 0;
   result = 0;
   /* Arrange the name of the file to be opened with the schema directory name */
   strncpy (DirBuffer, SchemaPath, MAX_PATH);
   MakeCompleteName (presentationName, "PRS", DirBuffer, text, &i);
   /* Checks if the file exists */
   file = TtaReadOpen (text);
   if (file == 0)
      /* presentation schema inaccessible */
      TtaError (ERR_cannot_load_pschema);
   else
     {
	/* Gets the corresponding structure schema name */
	TtaReadName (file, gotStructName);
	if (strcmp (structureName, gotStructName) == 0)
	   result = 1;
	TtaReadClose (file);
     }
   return result;
}


/*----------------------------------------------------------------------
   TtaGiveViewsToOpen

   Returns the names of the views that can be opened for a document.

   Parameters:
   document: the document.
   buffer: a buffer that will contain the result.

   Return parameters:
   buffer: list of view names. Each name is a character string with
   a final EOS. Names of views that are already open have a '*'
   at the       end.
   nbViews: number of names in the list, 0 if not any view can be open.

  ----------------------------------------------------------------------*/
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
   /* Checks the parameter document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parameter document is ok */
      BuildViewList (LoadedDocument[document - 1], buffer, nbViews);
}


/*----------------------------------------------------------------------
   TtaGetViewName

   Returns the name of an open view.

   Parameters:
   document: the document to which the view belongs.
   view: the view.

   Return value:
   Name of the view. The buffer must be provided by the caller.

  ----------------------------------------------------------------------*/
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
   DocViewDescr        dView;
   int                 numAssoc;

   UserErrorCode = 0;
   nameBuffer[0] = EOS;
   /* Checks the parameter document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parameter document is ok */
     {
	pDoc = LoadedDocument[document - 1];
	if (view < 100)
	   /* View of the main tree */
	   if (view < 1 || view > MAX_VIEW_DOC)
	      TtaError (ERR_invalid_parameter);
	   else
	     {
		dView = pDoc->DocView[view - 1];
		if (dView.DvSSchema != NULL || dView.DvPSchemaView != 0)
		   strncpy (nameBuffer, dView.DvSSchema->SsPSchema->PsView[dView.DvPSchemaView - 1], MAX_NAME_LENGTH);
	     }
	else
	   /* View of associated elements */
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


/*----------------------------------------------------------------------
   TtaIsViewOpened

   Returns TRUE for a open view.

   Parameters:
   document: the document to which the view belongs.
   view: the view.

   Return value:
   TRUE or FALSE.

  ----------------------------------------------------------------------*/
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
   DocViewDescr        dView;
   int                 numAssoc;
   boolean             opened;

   UserErrorCode = 0;
   opened = FALSE;
   /* Checks the parameter document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parameter document is ok */
     {
	pDoc = LoadedDocument[document - 1];
	if (view < 100)
	   /* View of the main tree */
	   if (view < 1 || view > MAX_VIEW_DOC)
	      TtaError (ERR_invalid_parameter);
	   else
	     {
		dView = pDoc->DocView[view - 1];
		if (dView.DvSSchema != NULL || dView.DvPSchemaView != 0)
		   opened = TRUE;
	     }
	else
	   /* View of associated elements */
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

/*----------------------------------------------------------------------
   TtaGetViewFromName

   Returns the identifier of a view of a given document from its name.

   Parameters:
   document: the document to which the view belongs.
   viewName: the name of the view.

   Return value:
   the view. 0 if no view of that name is currently open for the document.

  ----------------------------------------------------------------------*/
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
   DocViewDescr        dView;
   int                 aView;

   UserErrorCode = 0;
   view = 0;
   /* Checks the parameter document */
   if (document < 1 || document > MAX_DOCUMENTS)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else if (LoadedDocument[document - 1] == NULL)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else
      /* parameter document is ok */
     {
	pDoc = LoadedDocument[document - 1];
	/* search in the opened views of the main tree */
	for (aView = 1; aView <= MAX_VIEW_DOC && view == 0; aView++)
	  {
	     dView = pDoc->DocView[aView - 1];
	     if (dView.DvSSchema != NULL && dView.DvPSchemaView != 0)
		if (strcmp (viewName, dView.DvSSchema->SsPSchema->PsView[dView.DvPSchemaView - 1]) == 0)
		   view = aView;
	  }
	if (view == 0)
	   /* If not found, searching in the views of associated elements */
	   for (aView = 1; aView <= MAX_ASSOC_DOC && view == 0; aView++)
	     {
		pEl = pDoc->DocAssocRoot[aView - 1];
		if (pEl != NULL)
		   if (pDoc->DocAssocFrame[aView - 1] != 0)
		      if (strcmp (viewName, pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrName) == 0)
			 view = aView + 100;
	     }
     }
   return view;
}

/*----------------------------------------------------------------------
   TtaGiveActiveView

   Returns the active view and the document to which that view belongs.
   The active view is the one that receives the characters typed by
   the user.

   Parameter:
   No parameter.

   Return values:
   document: the active document.
   view: the active view.

  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                TtaGiveActiveView (Document * document, View * view)
#else  /* __STDC__ */
void                TtaGiveActiveView (document, view)
Document           *document;
View               *view;
#endif /* __STDC__ */
{
   PtrDocument         pDoc;
   int                 aView;
   boolean             assoc;

   UserErrorCode = 0;
   *document = 0;
   *view = 0;
   if (ActiveFrame != 0)
     {
	GetDocAndView (ActiveFrame, &pDoc, &aView, &assoc);
	if (pDoc != NULL)
	  {
	     *document = IdentDocument (pDoc);
	     if (assoc)
		*view = aView + 100;
	     else
		*view = aView;
	  }
     }
}

/*----------------------------------------------------------------------
   ExtinguishOrLightSelection bascule la selection courante dans      
   toutes les vues du document pDoc.                               
  ----------------------------------------------------------------------*/


#ifdef __STDC__
static void         ExtinguishOrLightSelection (PtrDocument pDoc, boolean lighted)
#else  /* __STDC__ */
static void         ExtinguishOrLightSelection (pDoc, lighted)
PtrDocument         pDoc;
boolean             lighted;
#endif /* __STDC__ */
{
   int                 view;
   int                 assoc;

   for (view = 1; view <= MAX_VIEW_DOC; view++)
      if (pDoc->DocView[view - 1].DvPSchemaView > 0)
	 SwitchSelection (pDoc->DocViewFrame[view - 1], lighted);
   for (assoc = 1; assoc <= MAX_ASSOC_DOC; assoc++)
      if (pDoc->DocAssocFrame[assoc - 1] > 0)
	 SwitchSelection (pDoc->DocAssocFrame[assoc - 1], lighted);
}


/*----------------------------------------------------------------------
   DestroyImage detruit l'image abstraite de toutes les vues          
   ouvertes dudocument pDoc                                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         DestroyImage (PtrDocument pDoc)
#else  /* __STDC__ */
static void         DestroyImage (pDoc)
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   int                 view;
   int                 assoc;

   for (view = 1; view <= MAX_VIEW_DOC; view++)
      if (pDoc->DocView[view - 1].DvPSchemaView > 0)
	 CleanImageView (view, FALSE, pDoc, FALSE);
   for (assoc = 1; assoc <= MAX_ASSOC_DOC; assoc++)
      if (pDoc->DocAssocFrame[assoc - 1] > 0)
	 CleanImageView (assoc, TRUE, pDoc, FALSE);
}				/* DestroyImage */


/*----------------------------------------------------------------------
   RebuildViewImage recree l'image abstraite de la vue Vue du
   document pDoc procedure partiellement reprise de               
   Aff_Select_Pages du module page.c                              
   Vue = numero d'elt assoc si vue associee sinon                 
   Vue = numero de vue si vue d'arbre principal                   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         RebuildViewImage (int view, boolean Assoc, PtrDocument pDoc)
#else  /* __STDC__ */
static void         RebuildViewImage (view, Assoc, pDoc)
int                 view;
boolean             Assoc;
PtrDocument         pDoc;
#endif /* __STDC__ */
{
   PtrElement          pElRoot;
   PtrAbstractBox      pAbbRoot;
   ViewFrame          *pFrame;
   int                 frame, h, w;
   boolean             complete;

   if (Assoc)
     {
#ifdef __COLPAGE__
	pDoc->DocAssocFreeVolume[view - 1] = THOT_MAXINT;
	pDoc->DocAssocNPages[view - 1] = 0;
#else  /* __COLPAGE__ */
	pDoc->DocAssocFreeVolume[view - 1] = pDoc->DocAssocVolume[view - 1];
#endif /* __COLPAGE__ */
	pElRoot = pDoc->DocAssocRoot[view - 1];
	pAbbRoot = pElRoot->ElAbstractBox[0];
	frame = pDoc->DocAssocFrame[view - 1];
	AbsBoxesCreate (pElRoot, pDoc, 1, TRUE, TRUE, &complete);
        if (pAbbRoot == NULL)
          pAbbRoot = pElRoot->ElAbstractBox[0];
	h = 0;
	ChangeConcreteImage (frame, &h, pAbbRoot);
     }
   else
     {
	pElRoot = pDoc->DocRootElement;
#ifdef __COLPAGE__
	pDoc->DocViewFreeVolume[view - 1] = THOT_MAXINT;
	pDoc->DocViewNPages[view - 1] = 0;
#else  /* __COLPAGE__ */
	pDoc->DocViewFreeVolume[view - 1] = pDoc->DocViewVolume[view - 1];
#endif /* __COLPAGE__ */
	pAbbRoot = pDoc->DocViewRootAb[view - 1];
	frame = pDoc->DocViewFrame[view - 1];
	AbsBoxesCreate (pElRoot, pDoc, view, TRUE, TRUE, &complete);
        if (pAbbRoot == NULL)
          pAbbRoot =
            pDoc->DocViewRootAb[view - 1] =
            pElRoot->ElAbstractBox[view - 1];
	h = 0;
	ChangeConcreteImage (frame, &h, pAbbRoot);
     }
   /* force to redraw all the frame */
   pFrame = &ViewFrameTable[frame - 1];
   GetSizesFrame (frame, &w, &h);
   DefClip (frame, pFrame->FrXOrg, pFrame->FrYOrg, w, h);
}                               /* RebuildViewImage */


/*----------------------------------------------------------------------
   RebuildImage recree l'image abstraite de toutes les vues            
   ouvertes du document pDoc                                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         RebuildImage (PtrDocument pDoc)
#else  /* __STDC__ */
static void         RebuildImage (pDoc)
PtrDocument         pDoc;
#endif /* __STDC__ */
{
   int                 view;
   int                 assoc;

   for (view = 1; view <= MAX_VIEW_DOC; view++)
      if (pDoc->DocView[view - 1].DvPSchemaView > 0)
	 RebuildViewImage (view, FALSE, pDoc);
   for (assoc = 1; assoc <= MAX_ASSOC_DOC; assoc++)
      if (pDoc->DocAssocFrame[assoc - 1] > 0)
	 RebuildViewImage (assoc, TRUE, pDoc);

}				/* RebuildImage */


static void         RedisplayCommand ( /* document */ );

/*----------------------------------------------------------------------
   RedisplayNewElement affiche un element qui vient d'etre ajoute'    
   dans un arbre abstrait.                                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                RedisplayNewElement (Document document, PtrElement newElement, PtrElement sibling, boolean first, boolean creation)
#else  /* __STDC__ */
void                RedisplayNewElement (document, newElement, sibling, first, creation)
Document            document;
PtrElement          newElement;
PtrElement          sibling;
boolean             first;
boolean             creation;
#endif /* __STDC__ */
{
   PtrDocument         pDoc;
   int                 view;

   pDoc = LoadedDocument[document - 1];
   if (pDoc == NULL)
      return;
   /* If the document has not a presentation schema, we do nothing */
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
      for (view = 1; view <= MAX_VIEW_DOC; view++)
	{
	   if (pDoc->DocView[view - 1].DvPSchemaView > 0)
	      /* la vue est ouverte */
	      if (ElemWithinImage (newElement, view, pDoc->DocViewRootAb[view - 1], pDoc))
		 /* l'element se trouve a l'interieur de l'image deja construite */
		{
		   /* indique qu'il faut creer les paves sans limite de volume */
		   pDoc->DocViewFreeVolume[view - 1] = THOT_MAXINT;
		   /* cree effectivement les paves du nouvel element dans la vue */
		   CreateNewAbsBoxes (newElement, pDoc, view);
		}
	}
   else
      /* View of associated elements */
   if (pDoc->DocAssocFrame[newElement->ElAssocNum - 1] != 0)
      /* la vue est ouverte */
      if (ElemWithinImage (newElement, 1, pDoc->DocAssocRoot[newElement->ElAssocNum - 1]->ElAbstractBox[0], pDoc))
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
	UpdateNumbers (newElement, newElement, pDoc, (documentDisplayMode[document - 1] == DisplayImmediately));
     }
}

/*----------------------------------------------------------------------
   ChangeAbsBoxModifAttrIntoView change les booleens AbCanBeModified et AbReadOnly   
   dans tous les paves de l'element pEl qui appartiennent a la vue 
   vue. newAbsModif donne la nouvelle valeur de AbCanBeModified,          
   reaffiche indique si on veut reafficher.                        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ChangeAbsBoxModifAttrIntoView (PtrElement pEl, int view, boolean newAbsModif, boolean redisplay)
#else  /* __STDC__ */
static void         ChangeAbsBoxModifAttrIntoView (pEl, view, newAbsModif, redisplay)
PtrElement          pEl;
int                 view;
boolean             newAbsModif;
boolean             redisplay;
#endif /* __STDC__ */
{
   PtrAbstractBox      pAb, pAbbChild;
   boolean             stop;

   pAb = pEl->ElAbstractBox[view - 1];
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
		pAb->AbReadOnly = !newAbsModif;
		if (redisplay)
		   pAb->AbAspectChange = TRUE;
		if (!pAb->AbPresentationBox)
		   /* c'est le pave principal de l'element */
		  {
		     /* les paves de presentation restent non modifiables */
		     pAb->AbCanBeModified = newAbsModif;
		     /* traite les paves de presentation crees par Create et */
		     /* CreateLast */
		     pAbbChild = pAb->AbFirstEnclosed;
		     while (pAbbChild != NULL)
		       {
			  if (pAbbChild->AbElement == pEl)
			     /* c'est un pave de l'element */
			    {
			       pAbbChild->AbReadOnly = !newAbsModif;
			       if (redisplay)
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


/*----------------------------------------------------------------------
   ChangeAbsBoxModif change les booleens AbCanBeModified et AbReadOnly dans 
   tous les paves existants de l'element pEl et de sa descendance. 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ChangeAbsBoxModif (PtrElement pEl, Document document, boolean newAbsModif)
#else  /* __STDC__ */
void                ChangeAbsBoxModif (pEl, document, newAbsModif)
PtrElement          pEl;
Document            document;
boolean             newAbsModif;
#endif /* __STDC__ */
{
   PtrDocument         pDoc;
   int                 view;
   PtrElement          pChild;
   boolean             redisplay;

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
   redisplay = ShowReadOnly ();
   if (!AssocView (pEl))
      /* on traite toutes les vues du document */
      for (view = 1; view <= MAX_VIEW_DOC; view++)
	{
	   /* on traite tous les paves de l'element dans cette vue */
	   ChangeAbsBoxModifAttrIntoView (pEl, view, newAbsModif, redisplay);
	   if (redisplay && pEl->ElAbstractBox[view - 1] != NULL)
	      RedispAbsBox (pEl->ElAbstractBox[view - 1], LoadedDocument[document - 1]);
	}
   else
      /* View of associated elements */
     {
	/* on traite tous les paves de l'element dans cette vue */
	ChangeAbsBoxModifAttrIntoView (pEl, 1, newAbsModif, redisplay);
	if (redisplay && pEl->ElAbstractBox[0] != NULL)
	   RedispAbsBox (pEl->ElAbstractBox[0], LoadedDocument[document - 1]);
     }
   if (redisplay)
      /* on fait reafficher pour visualiser le changement de couleur */
     {
	AbstractImageUpdated (LoadedDocument[document - 1]);
	RedisplayCommand (document);
     }
   /* meme traitement pour les fils qui heritent les droits d'acces */
   if (!pEl->ElTerminal)
     {
	pChild = pEl->ElFirstChild;
	while (pChild != NULL)
	  {
	     if (pChild->ElAccess == AccessInherited)
		ChangeAbsBoxModif (pChild, document, newAbsModif);
	     pChild = pChild->ElNext;
	  }
     }
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
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

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
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




/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             DemandeSelEnregistree (Document document, boolean * abort)
#else  /* __STDC__ */
boolean             DemandeSelEnregistree (document, abort)
Document            document;
boolean            *abort;
#endif /* __STDC__ */
{
   boolean             ret;

   ret = documentNewSelection[document - 1].SDSelActive;
   if (ret)
      *abort = (documentNewSelection[document - 1].SDElemSel == NULL);
   return ret;
}


/*----------------------------------------------------------------------
   CheckSelectedElement verifie si l'element pEl constitue    
   l'une des extremite's de la selection courante dans le document 
   "document" et si oui definit une nouvelle selection, sans cet   
   element.                                                        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         CheckSelectedElement (PtrElement pEl, Document document)
#else  /* __STDC__ */
static void         CheckSelectedElement (pEl, document)
PtrElement          pEl;
Document            document;
#endif /* __STDC__ */
{
   PtrDocument         pDoc;
   PtrDocument         selDoc;
   PtrElement          firstSelection, lastSelection, selEl, previousSelection;
   int                 firstChar, lastChar;
   boolean             ok, changeSelection;

   pDoc = LoadedDocument[document - 1];
   ok = GetCurrentSelection (&selDoc, &firstSelection, &lastSelection, &firstChar, &lastChar);
   if (ok && selDoc == pDoc)
      /* il y a une selection dans le document traite' */
      if (SelContinue)
	 /* la selection est continue */
	{
	   changeSelection = FALSE;
	   if (pEl == firstSelection)
	      /* l'element est en tete de la selection */
	      if (pEl == lastSelection)
		 /* la selection contient uniquement l'element, on l'annule */
		 ResetSelection (pDoc);
	      else
		 /* il y a d'autres elements selectionne's, on fait demarrer */
		 /* la nouvelle selection sur l'element suivant */
		{
		   firstSelection = NextInSelection (firstSelection, lastSelection);
		   firstChar = 0;
		   changeSelection = TRUE;
		}
	   else
	      /* l'element n'est pas en tete de la selection */
	   if (pEl == lastSelection)
	      /* l'element est en queue de selection */
	     {
		/* on cherche l'element precedent dans la selection */
		selEl = firstSelection;
		previousSelection = NULL;
		while (selEl != NULL && selEl != lastSelection)
		  {
		     previousSelection = selEl;
		     selEl = NextInSelection (selEl, lastSelection);
		  }
		if (previousSelection != NULL)
		   /* on fait terminer la nouvelle selection sur l'element */
		   /* precedent */
		  {
		     lastSelection = previousSelection;
		     lastChar = 0;
		     changeSelection = TRUE;
		  }
	     }
	   else
	     {
		if (ElemIsWithinSubtree (firstSelection, pEl) && ElemIsWithinSubtree (lastSelection, pEl))
		   /* la selection est entierement a l'interieur de l'element */
		   /* on annule la selection courante */
		   ResetSelection (pDoc);
	     }
	   if (changeSelection)
	     {
		if (firstChar > 1)
		   TtaSelectString (document, (Element) firstSelection, firstChar, 0);
		else
		   TtaSelectElement (document, (Element) firstSelection);
		if (lastSelection != firstSelection)
		   TtaExtendSelection (document, (Element) lastSelection, lastChar);
	     }
	}
      else
	 /* la selection est discontinue */
	{
	   selEl = firstSelection;
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
		 selEl = NextInSelection (selEl, lastSelection);
	}
}


/*----------------------------------------------------------------------
   UndisplayElement "desaffiche" un element qui va etre retire'    
   de son arbre abstrait.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                UndisplayElement (PtrElement pEl, Document document)
#else  /* __STDC__ */
void                UndisplayElement (pEl, document)
PtrElement          pEl;
Document            document;
#endif /* __STDC__ */
{
   PtrElement          pNext, pPrevious, pFather, pNeighbour, pE, pSS;
   boolean             stop;
   PtrDocument         pDoc;
   int                 savePageHeight;
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
   CheckSelectedElement (pEl, document);
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
   pFather = pEl->ElParent;
   RemoveElement (pEl);
   /* cherche l'element a partir duquel il faudra transmettre les */
   /* compteurs */
   if (pPrevious != NULL)
      pSS = pPrevious;
   else
      pSS = pFather;
   /* met a jour les numeros concerne's */
   pE = pEl;
   if (pNext != NULL)
     {
	/* il ne faut pas reafficher les numeros mis a jour si on est */
	/* en mode d'affichage differe'. Or, lorsque PageHeight != 0, */
	/* UpdateNumbers ne reaffiche pas les numeros qui changent. */
#ifdef __COLPAGE__
	savePageHeight = BreakPageHeight;
	if (documentDisplayMode[document - 1] == DeferredDisplay)
	   BreakPageHeight = 1;
#else  /* __COLPAGE__ */
	savePageHeight = PageHeight;
	if (documentDisplayMode[document - 1] == DeferredDisplay)
	   PageHeight = 1;
#endif /* __COLPAGE__ */
	while (pE != NULL)
	  {
	     UpdateNumbers (pNext, pE, pDoc, (documentDisplayMode[document - 1] == DisplayImmediately));
	     pE = pE->ElNext;
	  }
#ifdef __COLPAGE__
	BreakPageHeight = savePageHeight;
#else  /* __COLPAGE__ */
	PageHeight = savePageHeight;
#endif /* __COLPAGE__ */
     }
   if (pNext != NULL)
     {
	pNeighbour = pNext->ElPrevious;
	stop = FALSE;
	do
	   if (pNeighbour == NULL)
	      stop = TRUE;
	   else if (!pNeighbour->ElTerminal
		    || pNeighbour->ElLeafType != LtPageColBreak)
	      stop = TRUE;
	   else
	      pNeighbour = pNeighbour->ElPrevious;
	while (!(stop));
	if (pNeighbour == NULL)
	   /* l'element qui suit la partie detruite devient premier */
	   ChangeFirstLast (pNext, pDoc, TRUE, FALSE);
     }
   if (pPrevious != NULL)
     {
	pNeighbour = pPrevious->ElNext;
	stop = FALSE;
	do
	   if (pNeighbour == NULL)
	      stop = TRUE;
	   else if (!pNeighbour->ElTerminal
		    || pNeighbour->ElLeafType != LtPageColBreak)
	      stop = TRUE;
	   else
	      pNeighbour = pNeighbour->ElNext;
	while (!(stop));
	if (pNeighbour == NULL)
	   /* l'element qui precede la partie detruite devient dernier */
	   ChangeFirstLast (pPrevious, pDoc, FALSE, FALSE);
	/* traitement particulier aux tableaux */
	if (ThotLocalActions[T_createhairline] != NULL)
	   (*ThotLocalActions[T_createhairline]) (pPrevious, pEl, pDoc);
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


/*----------------------------------------------------------------------
   DisplayHolophrasted reaffiche un element sous forme holphrastee 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DisplayHolophrasted (PtrElement pEl, Document document)
#else  /* __STDC__ */
void                DisplayHolophrasted (pEl, document)
PtrElement          pEl;
Document            document;
#endif /* __STDC__ */
{
   PtrDocument         pDoc;
   int                 view;

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
      for (view = 1; view <= MAX_VIEW_DOC; view++)
	{
	   if (pDoc->DocView[view - 1].DvPSchemaView > 0)
	      /* la vue est ouverte */
	      if (ElemWithinImage (pEl, view, pDoc->DocViewRootAb[view - 1], pDoc))
		 /* l'element se trouve a l'interieur de l'image deja construite */
		{
		   /* indique qu'il faut creer les paves sans limite de volume */
		   pDoc->DocViewFreeVolume[view - 1] = THOT_MAXINT;
		   /* cree effectivement les paves du nouvel element dans la vue */
		   CreateNewAbsBoxes (pEl, pDoc, view);
		}
	}
   else
      /* View of associated elements */
   if (pDoc->DocAssocFrame[pEl->ElAssocNum - 1] != 0)
      /* la vue est ouverte */
      if (ElemWithinImage (pEl, 1, pDoc->DocAssocRoot[pEl->ElAssocNum - 1]->ElAbstractBox[0], pDoc))
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


/*----------------------------------------------------------------------
   HideElement "desaffiche" un element qui devient invisible       
   mais n'est pas detruit.                                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                HideElement (PtrElement pEl, Document document)
#else  /* __STDC__ */
void                HideElement (pEl, document)
PtrElement          pEl;
Document            document;
#endif /* __STDC__ */
{
   PtrDocument         pDoc;
   PtrElement          pChild;

   pDoc = LoadedDocument[document - 1];
   if (pDoc == NULL)
      return;
   /* si le document n'a pas de schema de presentation, on ne fait rien */
   if (pDoc->DocSSchema->SsPSchema == NULL)
      return;
   /* si le document est en mode de non calcul de l'image, on ne fait rien */
   if (documentDisplayMode[document - 1] == NoComputedDisplay)
      return;
   CheckSelectedElement (pEl, document);
   if (pEl->ElParent == NULL)
      /* c'est la racine d'un arbre, on detruit les paves des fils */
      /* pour garder au moins le pave racine */
     {
	if (!pEl->ElTerminal)
	  {
	     pChild = pEl->ElFirstChild;
	     while (pChild != NULL)
	       {
		  DestroyAbsBoxes (pChild, pDoc, TRUE);
		  pChild = pChild->ElNext;
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


/*----------------------------------------------------------------------
   RedisplayReference reaffiche une reference qui vient d'etre        
   modifiee.                                                       
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                RedisplayReference (PtrElement element, Document document)
#else  /* __STDC__ */
void                RedisplayReference (element, document)
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

/*----------------------------------------------------------------------
   RedisplayLeaf reaffiche le contenu d'une feuille.                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                RedisplayLeaf (PtrElement element, Document document, int delta)
#else  /* __STDC__ */
void                RedisplayLeaf (element, document, delta)
PtrElement          element;
Document            document;
int                 delta;
#endif /* __STDC__ */
{
   PtrDocument         pDoc;
   int                 v, frame, h;
   PtrAbstractBox      pAbb;
   boolean             modif;
   PtrAbstractBox      pAbbox1;

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
		    case LtPolyLine:
		       pAbbox1->AbLeafType = LtPolyLine;
		       pAbbox1->AbPolyLineBuffer = element->ElPolyLineBuffer;
		       pAbbox1->AbPolyLineShape = element->ElPolyLineType;
		       pAbbox1->AbVolume = element->ElNPoints;
		       break;
		    case LtSymbol:
		    case LtGraphics:
		       pAbbox1->AbLeafType = element->ElLeafType;
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


/*----------------------------------------------------------------------
   RedisplaySplittedText reaffiche le contenu d'un texte divise'.     
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                RedisplaySplittedText (PtrElement element, int position, PtrElement pNewEl, PtrElement  pNextEl, Document document)
#else  /* __STDC__ */
void                RedisplaySplittedText (element, position, pNewEl, pNextEl, document)
PtrElement          element;
int                 position;
PtrElement	    pNewEl;
PtrElement	    pNextEl;
Document            document;
#endif /* __STDC__ */
{
   PtrElement          pEl;
   PtrDocument         pDoc;

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
		LastSelectedElement = pNewEl;
		LastSelectedChar -= position;
	     }
	if ((PtrElement) element == FirstSelectedElement)
	   if (position < FirstSelectedChar)
	     {
		FirstSelectedElement = pNewEl;
		FirstSelectedChar -= position;
	     }
     }
   /* si le document est en mode de non calcul de l'image, on ne fait rien */
   if (documentDisplayMode[document - 1] == NoComputedDisplay)
      return;
   BuildAbsBoxSpliText ((PtrElement) element, pNewEl, pNextEl, pDoc);
   AbstractImageUpdated (pDoc);
   RedisplayCommand (document);
}

/*----------------------------------------------------------------------
   RedisplayMergedText reaffiche le contenu d'un texte fusionne'.     
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                RedisplayMergedText (PtrElement element, Document document)
#else  /* __STDC__ */
void                RedisplayMergedText (element, document)
PtrElement          element;
Document            document;
#endif /* __STDC__ */
{
   PtrElement          pEl, pENeighbour;
   boolean             stop;
   PtrDocument         pDoc;
   int                 view, dvol, h, frame;
   PtrAbstractBox      pAb;

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
   pENeighbour = pEl->ElNext;
   stop = FALSE;
   do
      if (pENeighbour == NULL)
	 /* pEl devient le dernier fils de son pere */
	{
	   ChangeFirstLast (pEl, pDoc, FALSE, FALSE);
	   stop = TRUE;
	}
      else if (!pENeighbour->ElTerminal || pENeighbour->ElLeafType != LtPageColBreak)
	 stop = TRUE;
      else
	 pENeighbour = pENeighbour->ElNext;
   while (!(stop));
   /* met a jour le volume des paves correspondants */
   for (view = 1; view <= MAX_VIEW_DOC; view++)
     {
	pAb = pEl->ElAbstractBox[view - 1];
	if (pAb != NULL)
	  {
	     dvol = pEl->ElTextLength - pAb->AbVolume;
	     pAb->AbVolume += dvol;
	     pAb->AbChange = TRUE;
	     if (!AssocView (pEl))
	       {
		  pDoc->DocViewModifiedAb[view - 1] =
		     Enclosing (pAb, pDoc->DocViewModifiedAb[view - 1]);
		  frame = pDoc->DocViewFrame[view - 1];
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

/*----------------------------------------------------------------------
   UndisplayInheritedAttributes      supprime sur l'element pEl et son       
   sous-arbre la presentation liee a l'attribut decrit par pAttr   
   ou a l'heritage d'un tel attribut.                              
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                UndisplayInheritedAttributes (PtrElement pEl, PtrAttribute pAttr, Document document, boolean suppression)
#else  /* __STDC__ */
void                UndisplayInheritedAttributes (pEl, pAttr, document, suppression)
PtrElement          pEl;
PtrAttribute        pAttr;
Document            document;
boolean             suppression;
#endif /* __STDC__ */
{
   boolean             inheritance, comparaison;
   PtrAttribute        pAttrAsc;
   PtrAttribute        pOldAttr;
   PtrElement          pElChild, pElAttr;

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
   inheritance = (pAttr->AeAttrSSchema->SsPSchema->
		  PsNHeirElems[pAttr->AeAttrNum - 1] > 0);
   comparaison = (pAttr->AeAttrSSchema->SsPSchema->
		  PsNComparAttrs[pAttr->AeAttrNum - 1] > 0);
   if (inheritance || comparaison)
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
	if (inheritance)
	   RemoveInheritedAttrPresent (pEl, LoadedDocument[document - 1], pOldAttr);
	/* puis on supprime sur les elements du sous-arbre pEl */
	/* les regles de presentation liees a la comparaison d'un attribut */
	/* du sous-arbre avec ce type d'attribut */
	if (!pEl->ElTerminal && comparaison)
	   for (pElChild = pEl->ElFirstChild; pElChild != NULL;
		pElChild = pElChild->ElNext)
	      RemoveComparAttrPresent (pElChild, LoadedDocument[document - 1], pOldAttr);
     }
   else if (pAttrAsc != NULL)
     {
	/* heritage et comparaison sont lies a un attribut porte' par un */
	/* ascendant de pEl */
	/* on supprime sur le sous arbre pEl les regles de presentation */
	/* liees a l'heritage de cet attribut par le sous-arbre s'il */
	/* existe des elements heritants de celui-ci */
	if (inheritance)
	   RemoveInheritedAttrPresent (pEl, LoadedDocument[document - 1], pAttrAsc);
	/* puis on supprime sur le sous-arbre pEl les regles de */
	/* presentation liees a la comparaison d'un attribut */
	/* du sous-arbre avec ce type d'attribut */
	if (comparaison)
	   RemoveComparAttrPresent (pEl, LoadedDocument[document - 1], pAttrAsc);
     }
}


/*----------------------------------------------------------------------
   DisplayAttribute                                                
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                DisplayAttribute (PtrElement pEl, PtrAttribute pAttr, Document document)
#else  /* __STDC__ */
void                DisplayAttribute (pEl, pAttr, document)
PtrElement          pEl;
PtrAttribute        pAttr;
Document            document;
#endif /* __STDC__ */
{
   boolean             inheritance, comparaison;
   PtrElement          pElChild;

   if (LoadedDocument[document - 1] == NULL)
      return;
   /* si le document n'a pas de schema de presentation, on ne fait rien */
   if (LoadedDocument[document - 1]->DocSSchema->SsPSchema == NULL)
      return;
   /* si le document est en mode de non calcul de l'image, on ne fait rien */
   if (documentDisplayMode[document - 1] == NoComputedDisplay)
      return;
   /* doit-on se preoccuper des heritages et comparaisons d'attributs? */
   inheritance = (pAttr->AeAttrSSchema->SsPSchema->
		  PsNHeirElems[pAttr->AeAttrNum - 1] > 0);
   comparaison = (pAttr->AeAttrSSchema->SsPSchema->
		  PsNComparAttrs[pAttr->AeAttrNum - 1] > 0);
   /* d'abord on applique les regles de presentation liees */
   /* a l'attribut sur l'element lui-meme */
   ApplyAttrPRulesToElem (pEl, LoadedDocument[document - 1], pAttr, FALSE);
   /* puis on applique sur pEl et les elements du sous-arbre pEl */
   /* les regles de presentation liees a l'heritage de cet attribut */
   /* par le sous arbre s'il existe des elements heritants de celui-ci */
   if (inheritance)
      ApplyAttrPRulesToSubtree (pEl, LoadedDocument[document - 1], pAttr);
   /* puis on applique sur les elements du sous arbre pEl */
   /* les regles de presentation liees a la comparaison d'un attribut */
   /* du sous-arbre avec cetype d'attribut */
   if (!pEl->ElTerminal && comparaison)
      for (pElChild = pEl->ElFirstChild; pElChild != NULL; pElChild = pElChild->ElNext)
	 ApplyAttrPRules (pElChild, LoadedDocument[document - 1], pAttr);
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


/*----------------------------------------------------------------------
   UndisplayAttribute                                              
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                UndisplayAttribute (PtrElement pEl, PtrAttribute pAttr, Document document)
#else  /* __STDC__ */
void                UndisplayAttribute (pEl, pAttr, document)
PtrElement          pEl;
PtrAttribute        pAttr;
Document            document;
#endif /* __STDC__ */
{
   boolean             inheritance, comparaison;
   PtrAttribute        pAttrAsc;
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
   inheritance = (pAttr->AeAttrSSchema->SsPSchema->
		  PsNHeirElems[pAttr->AeAttrNum - 1] > 0);
   comparaison = (pAttr->AeAttrSSchema->SsPSchema->
		  PsNComparAttrs[pAttr->AeAttrNum - 1] > 0);
   if (inheritance || comparaison)
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


/*----------------------------------------------------------------------
   RedisplayNewPRule                                               
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                RedisplayNewPRule (Document document, PtrElement pEl, PtrPRule pRule)
#else  /* __STDC__ */
void                RedisplayNewPRule (document, pEl, pRule)
Document            document;
PtrElement          pEl;
PtrPRule            pRule;
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
   ApplyNewRule (LoadedDocument[document - 1], pRule, pEl);
   AbstractImageUpdated (LoadedDocument[document - 1]);
   RedisplayCommand (document);
   /* la nouvelle regle de presentation doit etre prise en compte dans */
   /* les copies-inclusions de l'element */
   RedisplayCopies (pEl, LoadedDocument[document - 1], (documentDisplayMode[document - 1] == DisplayImmediately));
}

/*----------------------------------------------------------------------
   RedisplayDefaultPresentation                                              
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                RedisplayDefaultPresentation (Document document, PtrElement pEl, PRuleType typeRuleP, FunctionType funcType, int view)
#else  /* __STDC__ */
void                RedisplayDefaultPresentation (document, pEl, typeRuleP, funcType, view)
Document            document;
PtrElement          pEl;
PRuleType           typeRuleP;
FunctionType        funcType;
int                 view;
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
   /* if it's a function rule, nothing to do: there is no default presentation
      for functions */
   if (typeRuleP == PtFunction)
      return;
   ApplyStandardRule (pEl, LoadedDocument[document - 1], typeRuleP, funcType, view);
   AbstractImageUpdated (LoadedDocument[document - 1]);
   RedisplayCommand (document);
   /* le retrait de la regle de presentation doit etre pris en compte */
   /* dans les copies-inclusions de l'element */
   RedisplayCopies (pEl, LoadedDocument[document - 1], (documentDisplayMode[document - 1] == DisplayImmediately));
}

/*----------------------------------------------------------------------
   RedispNewGeometry                                               
  ----------------------------------------------------------------------*/

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

/*----------------------------------------------------------------------
   RedisplayCommand        Selon le mode d'affichage, execute ou   
   met en attente une commande de reaffichage secondaire.          
  ----------------------------------------------------------------------*/
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
	ExtinguishOrLightSelection (LoadedDocument[document - 1], FALSE);
	/* reaffiche ce qui a deja ete prepare' */
	RedisplayDocViews (LoadedDocument[document - 1]);
	/* rallume la selection */
	ExtinguishOrLightSelection (LoadedDocument[document - 1], TRUE);
     }
}


/*----------------------------------------------------------------------
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

  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaSetDisplayMode (Document document, DisplayMode newDisplayMode)
#else  /* __STDC__ */
void                TtaSetDisplayMode (document, newDisplayMode)
Document            document;
DisplayMode         newDisplayMode;
#endif /* __STDC__ */
{
  DisplayMode       oldDisplayMode;

  UserErrorCode = 0;
  /* Checks the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    /* parameter document is ok */
    {
      /* si le document n'a pas de schema de presentation, on ne fait rien */
      if (LoadedDocument[document - 1]->DocSSchema->SsPSchema == NULL)
	return;

      oldDisplayMode = documentDisplayMode[document - 1];
      if (oldDisplayMode != newDisplayMode)
	/* il y a effectivement changement de mode */
	{
	  if (oldDisplayMode == DisplayImmediately &&
	      (newDisplayMode == DeferredDisplay ||
	       newDisplayMode == NoComputedDisplay))
	    /* le document passe en mode affichage differe' ou sans calcul d'image */
	    {
	      /* eteint la selection */
	      ExtinguishOrLightSelection (LoadedDocument[document - 1], FALSE);
	      /* si on passe au mode sans calcul d'image il faut detruire l'image */
	      if (newDisplayMode == NoComputedDisplay)
		DestroyImage (LoadedDocument[document - 1]);
	      /* on met a jour le mode d'affichage */
	      documentDisplayMode[document - 1] = newDisplayMode;
	    }
	  else if ((oldDisplayMode == DeferredDisplay
		    || oldDisplayMode == NoComputedDisplay)
		   && newDisplayMode == DisplayImmediately)
	    /* le document passe du mode affichage differe' ou sans calcul  */
	    /* d'image au mode  d'affichage immediat */
	    {
	      /* on met a jour le mode d'affichage */
	      documentDisplayMode[document - 1] = newDisplayMode;

              if (oldDisplayMode == NoComputedDisplay)
                /* il faut recalculer l'image */
		RebuildImage (LoadedDocument[document - 1]);
	      
	      if (!documentNewSelection[document - 1].SDSelActive)
		/* la selection n'a pas change', on la rallume */
		ExtinguishOrLightSelection (LoadedDocument[document - 1], TRUE);
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
              /* reaffiche ce qui a deja ete prepare' */
              RedisplayDocViews (LoadedDocument[document - 1]);

	    }
	  else if (oldDisplayMode == DeferredDisplay
		   && newDisplayMode == NoComputedDisplay)
	    {
	      /* le document passe du mode affichage differe'  */
	      /* au mode d'affichage sans calcul d'image  */
	      DestroyImage (LoadedDocument[document - 1]);
	      /* on met a jour le mode d'affichage */
	      documentDisplayMode[document - 1] = newDisplayMode;
	    }
	  else if (oldDisplayMode == NoComputedDisplay
		   && newDisplayMode == DeferredDisplay)
	    {
	      /* on met a jour le mode d'affichage */
	      documentDisplayMode[document - 1] = newDisplayMode;
	      /* le document passe du mode affichage sans calcul d'image   */
	      /* au mode d'affichage differe'  */
	      RebuildImage (LoadedDocument[document - 1]);
	    }
	}
    }
}

/*----------------------------------------------------------------------
   TtaGetDisplayMode

   Returns the current display mode for a document.

   Parameter:
   document: the document.

   Return value:
   current display mode for that document.

  ----------------------------------------------------------------------*/
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
   /* Checks the parameter document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parameter document is ok */
      result = documentDisplayMode[document - 1];
   return result;
}

/* End of module */
