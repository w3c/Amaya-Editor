/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
   This module handles selection in abstract trees
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "appaction.h"
#include "app.h"
#include "libmsg.h"
#include "message.h"
#include "appdialogue.h"

#define EXPORT extern
#include "edit_tv.h"
#include "frame_tv.h"
#include "appdialogue_tv.h"
#undef EXPORT
#define EXPORT
#include "select_tv.h"

#include "appli_f.h"
#include "applicationapi_f.h"
#include "tree_f.h"
#include "search_f.h"
#include "config_f.h"
#include "createabsbox_f.h"
#include "scroll_f.h"
#include "views_f.h"
#include "draw_f.h"
#include "callback_f.h"
#include "exceptions_f.h"
#include "windowdisplay_f.h"
#include "exceptions_f.h"
#include "absboxes_f.h"
#include "keyboards_f.h"
#include "structmodif_f.h"
#include "boxselection_f.h"
#include "structselect_f.h"
#include "selectmenu_f.h"
#include "displayselect_f.h"
#include "searchref_f.h"

static PtrElement   FixedElement;	/* the element where the user clicked
					   first when setting the current
					   selection */
static int          FixedChar;		/* rank of the character where the
					   user clickeded first */

static int          NSelectedElements;	/* number of elements in table
					   SelectedElement */
static PtrElement   SelectedElement[MAX_SEL_ELEM]; /* the selected elements if
					   SelContinue is FALSE */
static int          LatestReturned;	/* rank in table SelectedElement of the
					   last element returned by function
					   NextInSelection */
static int          FrameWithNoUpdate;	/* the window where the selection does
					   not need to be highlighted */
static int          OldSelectedView;	/* old active view */
static PtrDocument  OldDocSelectedView;	/* the document to which the old active
					   view belongs */
static int          SelectedPointInPolyline;	/* if the current selection is
					   a PolyLine, rank of the selected
					   point, or 0 if the whole PolyLine is
					   selected */
static int          SelectedPictureEdge;/* if the current selection is a
					   picture, 1 means that the caret is
					   on the right side, 0 means that it
					   is on the left side or that the
					   whole picture is selected */

/* selection in the character string of an attribute value */
static PtrDocument  DocSelectedAttr;
static PtrAbstractBox AbsBoxSelectedAttr;
static int          FirstSelectedCharInAttr;
static int          LastSelectedCharInAttr;
static int          InitSelectedCharInAttr;


/*----------------------------------------------------------------------
   TtaSetCurrentKeyboard

   Sets a new current keyboard and displays it.

   Parameter:
       keyboard: the keyboard to be displayed.

  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                TtaSetCurrentKeyboard (int keyboard)

#else  /* __STDC__ */
void                TtaSetCurrentKeyboard (keyboard)
int                 keyboard;

#endif /* __STDC__ */

{
   if (ThotLocalActions[T_keyboard] != NULL)
      (*ThotLocalActions[T_keyboard]) (keyboard);
   /* remember current mode */
   KeyboardMode = keyboard;
}


/*----------------------------------------------------------------------
   InitSelection

   initialize variable describing the current selection.
  ----------------------------------------------------------------------*/

void                InitSelection ()
{
   SelectedDocument = NULL;
   FirstSelectedElement = NULL;
   FirstSelectedChar = 0;
   LastSelectedElement = NULL;
   LastSelectedChar = 0;
   SelectedView = 0;
   SynchronizeViews = TRUE;	/* all views of a document are synchronized */
   FixedElement = NULL;
   FixedChar = 0;
   FrameWithNoUpdate = 0;
   OldSelectedView = 0;
   OldDocSelectedView = NULL;
   SelectionUpdatesMenus = TRUE;
   LatestReturned = 0;
   SelMenuParentEl = NULL;
   SelMenuPreviousEl = NULL;
   SelMenuNextEl = NULL;
   SelMenuChildEl = NULL;
   SelectedPointInPolyline = 0;
   SelectedPictureEdge = 0;
   DocSelectedAttr = NULL;
   AbsBoxSelectedAttr = NULL;
   FirstSelectedCharInAttr = 0;
   LastSelectedCharInAttr = 0;
   InitSelectedCharInAttr = 0;
}


/*----------------------------------------------------------------------
   TtaSetFocus

   Choses the abstract box that should receive the characters typed by
   the user.
  ----------------------------------------------------------------------*/
void                TtaSetFocus ()
{
   PtrAbstractBox      pAb;

   if (SelectedDocument != NULL && FirstSelectedElement != NULL &&
       SelectedView != 0)
      /* there is a current selection */
     {
	/* abstract box of first selected element in active view */
	pAb = FirstSelectedElement->ElAbstractBox[SelectedView - 1];
	if (pAb == NULL)
	  {
	     /* the first selected element has no abstract box in the active
		active view. Try to create one */
	     CheckAbsBox (FirstSelectedElement, SelectedView, SelectedDocument,
			  FALSE, TRUE);
	     if (FirstSelectedElement->ElAssocNum != 0)
		/* it's an associated element */
		pAb = SelectedDocument->DocAssocRoot[FirstSelectedElement->
			ElAssocNum - 1]->ElAbstractBox[0];
	     else
		/* a view of the main tree */
		pAb = SelectedDocument->DocViewRootAb[SelectedView - 1];
	     ShowSelection (pAb, TRUE);
	  }
     }
}

/*----------------------------------------------------------------------
   VisibleDescendant

   returns the first element in the descendants of element pEl which has
   an abstract box in the view.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static PtrElement   VisibleDescendant (PtrElement pEl, int view)
#else  /* __STDC__ */
static PtrElement   VisibleDescendant (pEl, view)
PtrElement          pEl;
int                 view;

#endif /* __STDC__ */
{
   PtrElement          pChild, pVisible;

   if (pEl->ElAbstractBox[view - 1] != NULL)
      pVisible = pEl;
   else
     {
	pVisible = NULL;
	if (!pEl->ElTerminal)
	  {
	     pChild = pEl->ElFirstChild;
	     while (pVisible == NULL && pChild != NULL)
	       {
		  pVisible = VisibleDescendant (pChild, view);
		  pChild = pChild->ElNext;
	       }
	  }
     }
   return pVisible;
}


/*----------------------------------------------------------------------
  EnclosingAssocAbsBox

  Restore in all views the enclosing abstract boxes for associated element
  pEl, if this associated element is displayed in a page footer or header.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EnclosingAssocAbsBox (PtrElement pEl)
#else  /* __STDC__ */
static void         EnclosingAssocAbsBox (pEl)
PtrElement          pEl;

#endif /* __STDC__ */
{
   int                 view;
   PtrElement          pDesc;
   boolean             stop;

   if (!AssocView (pEl))
     {
	/* search the enclosing associated element */
	stop = FALSE;
	do
	   if (pEl == NULL)
	      stop = TRUE;
	   else if (pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrAssocElem)
	      stop = TRUE;
	   else
	      pEl = pEl->ElParent;
	while (!stop);
	if (pEl != NULL)
	   /* pEl is the enclosing associated element */
	   if (pEl->ElParent != NULL)
	      for (view = 0; view < MAX_VIEW_DOC; view++)
		 /* Restore the abstract box pointer for the root element of
		    the associated elements tree */
		 if (SelectedDocument->DocView[view].DvPSchemaView > 0)
		    /* search the first descendant having an abstract box in
		       the view */
		   {
		      pDesc = VisibleDescendant (pEl, view + 1);
		      if (pDesc != NULL)
			 pEl->ElParent->ElAbstractBox[view] =
			    pDesc->ElAbstractBox[view]->AbEnclosing;
		   }
     }
}


/*----------------------------------------------------------------------
   GetCurrentSelection

   returns the current selection.
   Returns FALSE if there is no selection.
   Returns TRUE if there is a selection.
   In the latter case, returns also
     pDoc: the document containing the current selection
     firstEl: first selected element
     lastEl: last selected element
     firstChar: rank of first selected char in firstEl, if firstEl is
	a text leaf, 0 if firstEl is entirely selected.
     lastChar: rank of the character following the last selected character,
	if lastEl is a text leaf.
     if firstEl is the same element as lastEl, if this element is a text leaf,
	and if *firstChar == *lastChar, no character is selected, only the
	position before firstChar is selected.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             GetCurrentSelection (PtrDocument * pDoc, PtrElement * firstEl, PtrElement * lastEl, int *firstChar, int *lastChar)
#else  /* __STDC__ */
boolean             GetCurrentSelection (pDoc, firstEl, lastEl, firstChar, lastChar)
PtrDocument        *pDoc;
PtrElement         *firstEl;
PtrElement         *lastEl;
int                *firstChar;
int                *lastChar;

#endif /* __STDC__ */

{
   boolean             ret;
   PtrElement          pEl;

   *pDoc = NULL;
   if (SelectedDocument != NULL)
     {
	/* there is a current selection */
	ret = TRUE;
	*pDoc = SelectedDocument;
	*firstEl = FirstSelectedElement;
	*lastEl = LastSelectedElement;
	pEl = FirstSelectedElement;
	if (pEl->ElTerminal && pEl->ElLeafType == LtText)
	  {
	     /* the first selected element is a text leaf */
	     if (LastSelectedElement == FirstSelectedElement)
		/* it's also the last selected element */
	       {
		  if (pEl->ElTextLength < LastSelectedChar &&
		      FirstSelectedChar <= 1)
		     /* the whole element is selected */
		    {
		       LastSelectedChar = 0;
		       FirstSelectedChar = 0;
		    }
	       }
	     else if (pEl->ElTextLength == 0)
		/* empty element. The whole element is selected */
		FirstSelectedChar = 0;
	  }
	if (pEl->ElTerminal && pEl->ElLeafType == LtPolyLine)
	  {
	     *firstChar = SelectedPointInPolyline;
	     *lastChar = SelectedPointInPolyline;
	  }
	else
	  {
	     *firstChar = FirstSelectedChar;
	     if (LastSelectedChar == 0)
		*lastChar = 0;
	     else if (SelPosition)
		*lastChar = LastSelectedChar;
	     else
		*lastChar = LastSelectedChar + 1;
	  }
     }
   else
      /* no current selection */
      ret = FALSE;
   return ret;
}


/*----------------------------------------------------------------------
   GetActiveView

   Returns the active view:
   pDoc: the document to which the active view belongs,
         NULL if there is no selection.
   view: number of this view in the document (if assoc is FALSE)
         number of the corresponding associated element (if assoc is TRUE)
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GetActiveView (PtrDocument * pDoc, int *view, boolean * assoc)

#else  /* __STDC__ */
void                GetActiveView (pDoc, view, assoc)
PtrDocument        *pDoc;
int                *view;
boolean            *assoc;

#endif /* __STDC__ */

{
   int                 firstChar, lastChar;
   PtrDocument         pSelDoc;
   PtrElement          firstSel, lastSel;

   if (!GetCurrentSelection (&pSelDoc, &firstSel, &lastSel, &firstChar, &lastChar))
      *pDoc = NULL;
   else
     {
	*pDoc = pSelDoc;
	if (firstSel->ElAssocNum != 0)
	  {
	     *assoc = TRUE;
	     *view = firstSel->ElAssocNum;
	  }
	else
	  {
	     *assoc = FALSE;
	     *view = SelectedView;
	  }
     }
}


/*----------------------------------------------------------------------
   CancelSelection

   Cancels the current selection.
  ----------------------------------------------------------------------*/
void                CancelSelection ()
{
   PtrDocument         pDoc;

   pDoc = SelectedDocument;
   TtaClearViewSelections ();
   SelectedDocument = NULL;
   FirstSelectedElement = NULL;
   FirstSelectedChar = 0;
   LastSelectedElement = NULL;
   LastSelectedChar = 0;
   FixedElement = NULL;
   FixedChar = 0;
   NSelectedElements = 0;
   /* update all menus related to the current selection */
   if (pDoc != NULL && SelectionUpdatesMenus)
     {
	PrepareSelectionMenu ();
	if (ThotLocalActions[T_chselect] != NULL)
	   (*ThotLocalActions[T_chselect]) (pDoc);
	if (ThotLocalActions[T_chattr] != NULL)
	   (*ThotLocalActions[T_chattr]) (pDoc);
     }
   SelMenuParentEl = NULL;
   SelMenuPreviousEl = NULL;
   SelMenuNextEl = NULL;
   SelMenuChildEl = NULL;
   SelectedPointInPolyline = 0;
   SelectedPictureEdge = 0;
   DocSelectedAttr = NULL;
   AbsBoxSelectedAttr = NULL;
   FirstSelectedCharInAttr = 0;
   LastSelectedCharInAttr = 0;
   InitSelectedCharInAttr = 0;
}

/*----------------------------------------------------------------------
   ResetSelection

   If the current selection is in document pDoc, cancel this selection.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                ResetSelection (PtrDocument pDoc)

#else  /* __STDC__ */
void                ResetSelection (pDoc)
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   if (pDoc == SelectedDocument || pDoc == DocSelectedAttr)
     {
	CancelSelection ();
	SelectedView = 0;
	OldSelectedView = 0;
	OldDocSelectedView = NULL;
     }
}

/*----------------------------------------------------------------------
   NextInSelection

   Returns the element in current selection that follows element pEl.
   Returns NULL if the last selected element (pLastEl) has previously been
   returned.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
PtrElement          NextInSelection (PtrElement pEl, PtrElement pLastEl)

#else  /* __STDC__ */
PtrElement          NextInSelection (pEl, pLastEl)
PtrElement          pEl;
PtrElement          pLastEl;

#endif /* __STDC__ */

{
   int                 i;
   boolean             found;

   if (pEl != NULL)
      if (SelContinue)
	 if (pEl == pLastEl)
	    /* the last selected element has been returned previously */
	    pEl = NULL;
	 else if (ElemIsWithinSubtree (pLastEl, pEl))
	    /* the current element is an ancestor of the last selected
	       element. It's finished */
	    pEl = NULL;
	 else
	    /* search the following element in the abstract tree, or the
	       following element at an upper level */
	   {
	      found = FALSE;
	      while (pEl != NULL && !found)
		 if (pEl->ElNext == NULL)
		    pEl = pEl->ElParent;
		 else
		    found = TRUE;
	      if (found)
		{
		   pEl = pEl->ElNext;
		   while (ElemIsWithinSubtree (pLastEl, pEl) && pEl != pLastEl)
		      pEl = pEl->ElFirstChild;
		}
	   }
      else
	 /* the current selection is not contiguous. Search next element in
	    the SelectedElement table */
	{
	   /* first, search pEl in the table */
	   i = 1;
	   found = FALSE;
	   while (i <= NSelectedElements && !found)
	      if (SelectedElement[i - 1] == pEl)
		 found = TRUE;
	      else
		 i++;
	   if (!found)
	      /* pEl is not in the table */
	      if (LatestReturned == 0)
		{
		   pEl = SelectedElement[0];
		   LatestReturned = 1;
		}
	      else
		{
		   pEl = SelectedElement[LatestReturned];
		   LatestReturned++;
		}
	   else
	      /* pEl is in the table */
	   if (i < NSelectedElements)
	      /* it's not the last element in the table, returns the next one*/
	     {
		pEl = SelectedElement[i];
		LatestReturned = i + 1;
	     }
	   else
	      /* it's the last element in the table, return NULL */
	     {
		pEl = NULL;
		LatestReturned = 0;
	     }
	}
   return pEl;
}


/*----------------------------------------------------------------------
   ReverseSelection

   Reverse (highlight or not) the current selection in all views of the
   selected document, except in the active view.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                ReverseSelection ()

#else  /* __STDC__ */
void                ReverseSelection ()
#endif				/* __STDC__ */

{
   int                 view;

   if (SelectedDocument != NULL && FirstSelectedElement != NULL)
      /* there is a current selection */
      /* if the current selection is in an associated element, there is nothing
	 to do: associated elements have only one view, which is obviously the
	 active view */
      if (!AssocView (FirstSelectedElement))
	{
	   /* process all views */
	   for (view = 0; view < MAX_VIEW_DOC; view++)
	      if (SelectedDocument->DocView[view].DvPSchemaView > 0)
		 /* this view is open */
		 if (SelectedDocument->DocView[view].DvSSchema !=
		     SelectedDocument->DocView[SelectedView - 1].DvSSchema ||
		     SelectedDocument->DocView[view].DvPSchemaView !=
		     SelectedDocument->DocView[SelectedView - 1].DvPSchemaView)
		    /* it's not the active view */
		    SwitchSelection (SelectedDocument->DocViewFrame[view], FALSE);
	}
}


/*----------------------------------------------------------------------
   HiddenType

   Returns TRUE if, according to its type, element pEl must be hidden to
   the user.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
boolean             HiddenType (PtrElement pEl)

#else  /* __STDC__ */
boolean             HiddenType (pEl)
PtrElement          pEl;

#endif /* __STDC__ */

{
   SRule              *pSRule;
   boolean             ret;

   ret = FALSE;
   if (TypeHasException (ExcHidden, pEl->ElTypeNumber, pEl->ElStructSchema))
      /* the element type has exception Hidden */
     {
	/* if element is not empty or is a leaf, it is hidden
	   otherwise, exception Hidden is ignored: the user could not
	   select that element */
	if (pEl->ElTerminal || pEl->ElFirstChild != NULL)
	   ret = TRUE;
     }
   else
     {
	pSRule = &pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1];
	if (pSRule->SrConstruct == CsChoice)
	   if (pEl->ElFirstChild != NULL)
	      /* it's a choice element with a child */
	      if (pEl->ElSource == NULL)
		 /* it's not an inclusion -> it is hidden */
		 ret = TRUE;
     }
   return ret;
}


/*----------------------------------------------------------------------
   HighlightSelection

   Highlight all selected elements in all views.
   If showBegin is TRUE, scroll the document to show the beginning of
   the first selected element.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                HighlightSelection (boolean showBegin)

#else  /* __STDC__ */
void                HighlightSelection (showBegin)
boolean             showBegin;

#endif /* __STDC__ */

{
   PtrAbstractBox      pAb, pNextAb;

#ifdef __COLPAGE__
   PtrAbstractBox      pSavedAb;

#endif /* __COLPAGE__ */
   PtrElement          pEl, pNextEl;
   int                 view, lastView, frame, firstChar, lastChar;
   boolean             first, last, partialSel, active, unique, stop, assoc;

   if (SelectedDocument != NULL)
     {
	/* first, choose the views to be processed */
	if (AssocView (FirstSelectedElement))
	  {
	     /* associated elements have only 1 view */
	     assoc = TRUE;
	     lastView = 1;
	  }
	else
	  {
	     /* all views should be considered */
	     assoc = FALSE;
	     lastView = MAX_VIEW_DOC;
	  }
	unique = FirstSelectedElement == LastSelectedElement;
	/* process all chosen views */
	for (view = 0; view < lastView; view++)
	  {
	     /* get the number of the window (frame) where the view is
		displayed */
	     if (AssocView (FirstSelectedElement))
		frame = SelectedDocument->DocAssocFrame[FirstSelectedElement->ElAssocNum - 1];
	     else if (SelectedDocument->DocView[view].DvPSchemaView > 0)
		frame = SelectedDocument->DocViewFrame[view];
	     else
		/* this view is not open */
		frame = 0;
	     /* if the view is open, process all abstract boxes of the
		selected element in this view */
	     if (frame > 0 && frame != FrameWithNoUpdate)
	       {
		  /* switch selection off in this view */
		  ClearViewSelection (frame);
		  first = TRUE;
		  /* search the first abstract box of the current selection
		     that is visible in this view */
		  pEl = FirstSelectedElement;
		  pAb = pEl->ElAbstractBox[view];
		  partialSel = FALSE;
		  if (pEl->ElTerminal && pEl->ElLeafType == LtText)
		     /* the first element of the current selection is a
			text leaf */
		     if (FirstSelectedChar > 1 && pEl->ElTextLength > 0)
			/* that text leaf is only partly selected */
			partialSel = TRUE;
		  if (partialSel && pAb != NULL)
		     /* skip presentation abstract box created before the
			element box */
		    {
		       while (pAb->AbPresentationBox && pAb->AbNext != NULL)
			  pAb = pAb->AbNext;
		       if (pAb != NULL)
			  if (pAb->AbElement != pEl)
			     pAb = NULL;
		    }
		  pNextEl = pEl;
		  while (pAb == NULL && pNextEl != NULL)
		    {
		       stop = FALSE;
		       do
			  /* search next element in current selection */
			 {
			    pNextEl = NextInSelection (pNextEl, LastSelectedElement);
			    if (pNextEl == NULL)
			       /* end of current selection */
			       stop = TRUE;
			    else if (pNextEl->ElAbstractBox[view] != NULL)
			       /* the next element in the current selection
				  has an abstract box in this view */
			       stop = TRUE;
			 }
		       while (!stop);
		       if (pNextEl != NULL)
			  /* first element of current selection having an
			     abstract box in the view */
			 {
			    pEl = pNextEl;
			    pAb = pEl->ElAbstractBox[view];
			 }
		    }
		  /* handles all abstract boxes of the selected elements in
		     the view */
		  while (pAb != NULL)
		     /* search first the abstract box to be handled next */
		    {
#ifdef __COLPAGE__
		       /* handles all duplicated boxes of the element */
		       if (!pAb->AbPresentationBox)
			  /* only the main abstract box can be duplicated */
			  pSavedAb = pAb;
		       pNextAb = pAb->AbNext;
		       if (pNextAb != NULL)
			  /* there is a following abstract box */
			 {
			    if (pNextAb->AbElement != pEl)
			       /* the next abstract box does not belong to
				  that element */
			       pNextAb = pSavedAb->AbNextRepeated;
			 }
		       else
		          /* if pNextAb is NULL, the abstract box may be a
			     duplicate */
			  pNextAb = pSavedAb->AbNextRepeated;
#else  /* __COLPAGE__ */
		       pNextAb = pAb->AbNext;
		       if (pNextAb != NULL)
			  if (pNextAb->AbElement != pEl)
			     /* next abstract box does not belong to the
				element */
			     pNextAb = NULL;
#endif /* __COLPAGE__ */
		       if (pNextAb != NULL)
			 {
			    partialSel = FALSE;
			    if (pEl == LastSelectedElement)
			       /* last element in current selection */
			       if (pEl->ElTerminal && pEl->ElLeafType == LtText)
				  /* it's a text leaf */
				  if (LastSelectedChar < pEl->ElTextLength
				      && pEl->ElTextLength > 0
				      && LastSelectedChar > 0)
				     /* text is only partially selected */
				     partialSel = TRUE;
			    if (partialSel && !pAb->AbPresentationBox)
			       pNextAb = NULL;
			 }
		       if (pNextAb == NULL)
			  /* search next element in selection */
			 {
			    pNextEl = pEl;
			    stop = FALSE;
			    do
			      {
				 pNextEl = NextInSelection (pNextEl,
							  LastSelectedElement);
				 if (pNextEl == NULL)
				    stop = TRUE;
				 else if (pNextEl->ElAbstractBox[view] != NULL)
				    stop = TRUE;
			      }
			    while (!stop);
			    if (pNextEl != NULL)
			       /* first element in selection having an
				  abstract box in the view */
			      {
				 pEl = pNextEl;
				 /* first element to be handled */
				 pNextAb = pNextEl->ElAbstractBox[view];
			      }
			 }
		       last = pNextAb == NULL;
		       /* select the abstract box to be highlighted */
		       if (first || last)
			 {
			    if (pAb->AbElement == FirstSelectedElement)
			       firstChar = FirstSelectedChar;
			    else
			       firstChar = 0;
			    if (pAb->AbElement == LastSelectedElement)
			       lastChar = LastSelectedChar;
			    else
			       lastChar = 0;
			    InsertViewSelMarks (frame, pAb, firstChar,lastChar,
						first, last, unique);
			    active = (view + 1 == SelectedView);
			    if (showBegin && first &&
				(assoc || SelectedDocument->DocView[view].DvSync || active))
			       ShowSelectedBox (frame, active);

			    first = FALSE;
			    if (last)
			       /* highlight selection */
			       SwitchSelection (frame, TRUE);
			 }
		       pAb->AbSelected = TRUE;
		       pAb = pNextAb;
		    }
	       }
	  }
     }
}


/*----------------------------------------------------------------------
   SetActiveView

   cherche une vue du document selectionne' ou`    
   le debut de la selection est visible. Si exceptView
   n'est pas nul, evite de choisir cette vue.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         SetActiveView (int exceptView)
#else  /* __STDC__ */
static void         SetActiveView (exceptView)
int                 exceptView;

#endif /* __STDC__ */
{
   int                 view;
   boolean             stop;

   SelectedView = 0;
   /* plus de vue active */
   /* l'ancienne vue active convient-elle ? */
   if (OldDocSelectedView == SelectedDocument)
      /* l'ancienne vue active concerne bien le document selectionne' */
      if (OldSelectedView != 0 && OldSelectedView != exceptView)
	 if (FirstSelectedElement->ElAbstractBox[OldSelectedView - 1] != NULL)
	    SelectedView = OldSelectedView;
   if (SelectedView == 0)
      /* l'ancienne vue active ne convient pas, on en cherche une autre */
     {
	stop = FALSE;
	view = 0;
	do
	  {
	     view++;
	     if (view != exceptView)
		if (FirstSelectedElement->ElAbstractBox[view - 1] != NULL)
		  {
		     /* c'est la nouvelle vue active */
		     SelectedView = view;
		     OldDocSelectedView = SelectedDocument;
		     OldSelectedView = view;
		     stop = TRUE;
		  }
	     if (view >= MAX_VIEW_DOC)
		stop = TRUE;
	  }
	while (!stop);
     }
}


/*----------------------------------------------------------------------
   DeactivateView la vue de numero view du document pDoc est detruite,
   on change de vue active si c'est possible.              
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DeactivateView (PtrDocument pDoc, int view, boolean assoc)
#else  /* __STDC__ */
void                DeactivateView (pDoc, view, assoc)
PtrDocument         pDoc;
int                 view;
boolean             assoc;

#endif /* __STDC__ */
{

   if (pDoc == SelectedDocument)
      /* la selection se trouve dans le document dont on detruit une vue */
      if (assoc)
	 /* on detruit une vue d'elements associes */
	{
	   if (FirstSelectedElement->ElAssocNum == view)
	      /* ce sont les elements associes ou se trouve la selection */
	     {
		/* pour eviter de mettre a jour les menus */
		SelectedDocument = NULL;
		CancelSelection ();
		/* on annule donc la selection */
		SelectedView = 0;
		/* plus de vue active */
		OldSelectedView = 0;
		OldDocSelectedView = pDoc;
	     }
	}
      else
	 /* on detruit une vue de l'arbre principal */
      if (view == SelectedView)
	 /* c'est la vue active */
	{
	   SetActiveView (view);
	   /* cherche une nouvelle vue active */
	   if (SelectedView != 0)
	      /* signale la nouvelle vue active au Mediateur */
	      HighlightSelection (TRUE);
	}
}


/*----------------------------------------------------------------------
   WithinAbsBox   renvoie Vrai si le pave pAb est dans le          
   sous-arbre du pave' pRootAb.                                    
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static boolean      WithinAbsBox (PtrAbstractBox pAb, PtrAbstractBox pRootAb)

#else  /* __STDC__ */
static boolean      WithinAbsBox (pAb, pRootAb)
PtrAbstractBox      pAb;
PtrAbstractBox      pRootAb;

#endif /* __STDC__ */

{
   boolean             ret;

   ret = FALSE;
   do
      if (pAb == pRootAb)
	 ret = TRUE;
      else if (pAb != NULL)
	 pAb = pAb->AbEnclosing;
   while (!ret && pAb != NULL) ;
   return ret;
}


/*----------------------------------------------------------------------
   GetAbsBoxSelectedAttr retourne le pave' qui, dans la vue view
   affiche le meme attribut que AbsBoxSelectedAttr.        
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static PtrAbstractBox GetAbsBoxSelectedAttr (int view)

#else  /* __STDC__ */
static PtrAbstractBox GetAbsBoxSelectedAttr (view)
int                 view;

#endif /* __STDC__ */

{
   PtrAbstractBox      pAbView, pAb;

   if (view == AbsBoxSelectedAttr->AbDocView)
      pAbView = AbsBoxSelectedAttr;
   else
     {
	/* cherche le pave correspondant dans la vue : pAbView */
	pAbView = NULL;
	if (AbsBoxSelectedAttr->AbElement == NULL)
	   pAb = NULL;
	else
	   pAb = AbsBoxSelectedAttr->AbElement->ElAbstractBox[view - 1];
	while (pAbView == NULL && pAb != NULL &&
	       pAb->AbElement == AbsBoxSelectedAttr->AbElement)
	  {
	     if (pAb->AbPresentationBox)
		/* un pave de presentation de l'element auquel est attache' */
		/* l'attribut */
	       {
		  if (pAb->AbCanBeModified &&
		    pAb->AbCreatorAttr == AbsBoxSelectedAttr->AbCreatorAttr)
		     pAbView = pAb;
	       }
	     else
		/* le pave principal de l'element auquel est attache' l'attribut */
	       {
		  pAb = pAb->AbFirstEnclosed;
		  while (pAbView == NULL && pAb != NULL &&
			 pAb->AbElement == AbsBoxSelectedAttr->AbElement)
		    {
		       if (pAb->AbPresentationBox && pAb->AbCanBeModified)
			  if (pAb->AbCreatorAttr == AbsBoxSelectedAttr->AbCreatorAttr)
			     pAbView = pAb;
		       pAb = pAb->AbNext;
		    }
		  if (pAb != NULL)
		     pAb = pAb->AbEnclosing;
	       }
	     if (pAb != NULL)
		pAb = pAb->AbNext;
	  }
     }
   return pAbView;
}


/*----------------------------------------------------------------------
   ShowSelection allume, dans le sous-arbre de paves de racine     
   pRootAb, toutes les chaines et les paves qui font partie
   de la selection courante. Si visible est vrai, on       
   demande au Mediateur de rendre visible a l'utilisateur  
   le debut de la selection.                               
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                ShowSelection (PtrAbstractBox pRootAb, boolean visible)

#else  /* __STDC__ */
void                ShowSelection (pRootAb, visible)
PtrAbstractBox      pRootAb;
boolean             visible;

#endif /* __STDC__ */

{
   PtrElement          pEl, pNextEl;
   PtrAbstractBox      pAb, pNextAb;

#ifdef __COLPAGE__
   PtrAbstractBox      pSavedAb;

#endif /* __COLPAGE__ */
   int                 view, frame, firstChar, lastChar;
   boolean             selBegin, selEnd, active, unique, assoc, stop;

   view = pRootAb->AbDocView;
   if (AssocView (pRootAb->AbElement))
      /* vue d'elements associes */
      assoc = TRUE;
   else
      assoc = FALSE;
   if (SelectedDocument != NULL && FirstSelectedElement != NULL &&
       pRootAb != NULL)
     {
	/* il y a bien une selection */
	if (assoc)
	   frame = SelectedDocument->DocAssocFrame[pRootAb->AbElement->ElAssocNum - 1];
	else
	   frame = SelectedDocument->DocViewFrame[view - 1];
	selBegin = TRUE;
	selEnd = FALSE;
	unique = FirstSelectedElement == LastSelectedElement;
	/* cherche le 1er pave de la selection visible dans la vue et */
	/* appartenant au sous-arbre */
	pEl = FirstSelectedElement;
	if (WithinAbsBox (pEl->ElAbstractBox[view - 1], pRootAb))
	   /* 1er pave de cet element dans la vue */
	   pAb = pEl->ElAbstractBox[view - 1];
	else
	   pAb = NULL;
	pNextEl = pEl;
	while (pAb == NULL && pNextEl != NULL)
	  {
	     stop = FALSE;
	     do
		/* cherche l'element suivant de la selection */
	       {
		  pNextEl = NextInSelection (pNextEl, LastSelectedElement);
		  if (pNextEl == NULL)
		     stop = TRUE;
		  else if (pNextEl->ElAbstractBox[view - 1] != NULL)
		     stop = TRUE;
	       }
	     while (!stop);
	     if (pNextEl != NULL)
		/* 1er element de la selection qui a un pave dans la vue */
		if (WithinAbsBox (pNextEl->ElAbstractBox[view - 1], pRootAb))
		   /* le pave est dans le sous-arbre, on a trouve' */
		  {
		     pEl = pNextEl;
		     /* 1er element a traiter */
		     pAb = pNextEl->ElAbstractBox[view - 1];
		     /* 1er pave a traiter */
		  }
	  }
	if (pAb != NULL)
	   /* eteint la selection dans la vue */
	   ClearViewSelection (frame);
	/* traite successivement tous les paves de tous les elements de la */
	/* selection, a partir du premier pave visible dans la vue */
	while (pAb != NULL)
	  {
	     if (pEl == FirstSelectedElement)
		/* c'est le premier element de la selection */
		firstChar = FirstSelectedChar;
	     else
		firstChar = 0;
	     if (pEl == LastSelectedElement)
		/* c'est le dernier element de la selection */
		lastChar = LastSelectedChar;
	     else
		lastChar = 0;
	     /* cherche si c'est le dernier pave visible de la selection */
	     pNextAb = pAb->AbNext;
#ifdef __COLPAGE__
	     if (!pAb->AbPresentationBox)
		/* pour parcours des dup */
		pSavedAb = pAb;
	     pNextAb = pAb->AbNext;
	     if (pNextAb != NULL)
	       {
		  /* il y a un pave suivant */
		  if (pNextAb->AbElement != pEl)
		     pNextAb = pSavedAb->AbNextRepeated;
		  /* on parcourt aussi les dupliques */
	       }
	     else
		pNextAb = pSavedAb->AbNextRepeated;
#else  /* __COLPAGE__ */
	     if (pNextAb != NULL)
		/* il y a un pave suivant */
		if (pNextAb->AbElement != pEl)
		   pNextAb = NULL;
#endif /* __COLPAGE__ */
	     /* il n'appartient pas a l'element */
	     if (pNextAb == NULL)
		/* cherche l'element suivant de la selection ayant un pave dans */
		/* le sous-arbre */
	       {
		  pNextEl = pEl;
		  stop = FALSE;
		  do
		    {
		       pNextEl = NextInSelection (pNextEl, LastSelectedElement);
		       if (pNextEl == NULL)
			  stop = TRUE;
		       else if (pNextEl->ElAbstractBox[view - 1] != NULL)
			  stop = TRUE;
		    }
		  while (!stop);
		  if (pNextEl != NULL)
		     /* il y a un element suivant de la selection qui a un pave */
		     /* dans la vue */
		     if (WithinAbsBox (pNextEl->ElAbstractBox[view - 1], pRootAb))
			/* ce pave est dans le sous-arbre, ce sera le prochain */
			/* pave traite' */
		       {
			  pNextAb = pNextEl->ElAbstractBox[view - 1];
			  pEl = pNextEl;
		       }
	       }
	     selEnd = pNextAb == NULL;
	     /* signale la selection de l'element a l'afficheur */
	     if (selBegin || selEnd)
	       {
		  InsertViewSelMarks (frame, pAb, firstChar, lastChar,
				      selBegin, selEnd, unique);
		  /* vue active */
		  active = view == SelectedView;
		  if (visible)
		     if (assoc || SelectedDocument->DocView[view - 1].DvSync || active)
			ShowSelectedBox (frame, active);

		  selBegin = FALSE;
		  visible = FALSE;
		  if (selEnd)
		     /* allume la selection */
		     SwitchSelection (frame, TRUE);
	       }
	     pAb->AbSelected = TRUE;
	     /* passe au pave suivant a allumer */
	     pAb = pNextAb;
	  }
     }
   else if (DocSelectedAttr != NULL && AbsBoxSelectedAttr != NULL)
      /* il y a une selection dans une valeur d'attribut */
     {
	if (assoc)
	   frame = DocSelectedAttr->DocAssocFrame[pRootAb->AbElement->ElAssocNum - 1];
	else
	   frame = DocSelectedAttr->DocViewFrame[view - 1];
	pAb = GetAbsBoxSelectedAttr (view);
	if (pAb != NULL)
	  {
	     pAb->AbSelected = TRUE;
	     InsertViewSelMarks (frame, pAb, FirstSelectedCharInAttr,
				 LastSelectedCharInAttr, TRUE, TRUE, TRUE);
	     ShowSelectedBox (frame, TRUE);
	     SwitchSelection (frame, TRUE);
	  }
     }
}

/*----------------------------------------------------------------------
   DisplaySel		
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         DisplaySel (PtrElement pEl, int view, int frame, boolean assoc, boolean * abExist)

#else  /* __STDC__ */
static void         DisplaySel (pEl, view, frame, assoc, abExist)
PtrElement          pEl;
int                 view;
int                 frame;
boolean             assoc;
boolean            *abExist;

#endif /* __STDC__ */

{
   PtrAbstractBox      pAb, pNextAb;

#ifdef __COLPAGE__
   PtrAbstractBox      pSavedAb;

#endif /* __COLPAGE__ */
   int                 firstChar, lastChar;
   boolean             first, last, partialSel, unique, active;

   first = TRUE;
   pAb = pEl->ElAbstractBox[view - 1];
   /* premier pave de l'element dans la vue */
   if (pAb != NULL)
     {
	partialSel = FALSE;
	if (pEl == FirstSelectedElement)
	   /* c'est le premier element de la selection */
	   if (pEl->ElTerminal)
	      if (pEl->ElLeafType == LtText)
		 /* le premier element de la selection est un texte */
		{
		   if (FirstSelectedChar > 1 && pEl->ElTextLength > 0)
		      /* le texte n'est selectionne' que partiellement */
		      partialSel = TRUE;
		}
	      else if (pEl->ElLeafType == LtPolyLine)
		 if (SelectedPointInPolyline > 0)
		    partialSel = TRUE;
	if (partialSel)
	   /* on saute les paves de presentation crees avant */
	  {
	     while (pAb->AbPresentationBox && pAb->AbNext != NULL)
		pAb = pAb->AbNext;
	     if (pAb != NULL)
		if (pAb->AbElement != pEl)
		   pAb = NULL;
	  }
	/* l'element a au moins un pave */
	*abExist = pAb != NULL;
     }
   /* boucle sur les paves de l'element dans la vue */
   while (pAb != NULL)
     {
	if (!pAb->AbSelected)
	   if (!pAb->AbPresentationBox && pEl->ElAssocNum != 0)
	      /* retablit le pave englobant des elements associes */
	      EnclosingAssocAbsBox (pEl);
	/* cherche le pave suivant a selectionner */
	partialSel = FALSE;
	if (pEl == LastSelectedElement)
	   /* c'est le dernier element de la selection */
	   if (pEl->ElTerminal)
	      if (pEl->ElLeafType == LtText)
		 /* et c'est un texte */
		{
		   if (LastSelectedChar < pEl->ElTextLength
		       && pEl->ElTextLength > 0 && LastSelectedChar > 0)
		      /* le texte n'est selectionne' que partiellement */
		      partialSel = TRUE;
		}
	      else if (pEl->ElLeafType == LtPolyLine)
		 if (SelectedPointInPolyline > 0)
		    partialSel = TRUE;
	if (partialSel && !pAb->AbPresentationBox)
	   pNextAb = NULL;
	else
	   pNextAb = pAb->AbNext;
#ifdef __COLPAGE__
	if (!pAb->AbPresentationBox)
	   pSavedAb = pAb;
	if (pNextAb != NULL)
	  {
	     if (pNextAb->AbElement != pEl)
		/* le pave suivant n'appartient pas a l'element */
		pNextAb = pSavedAb->AbNextRepeated;
	  }
	/* si pNextAb = NULL, le pave a peut-etre un dup */
	else
	   pNextAb = pSavedAb->AbNextRepeated;
#else  /* __COLPAGE__ */
	if (pNextAb != NULL)
	   if (pNextAb->AbElement != pEl)
	      /* le pave suivant n'appartient pas a l'element */
	      pNextAb = NULL;
#endif /* __COLPAGE__ */
	last = pNextAb == NULL;
	/* indique la selection du pave au Mediateur */
	if (first || last)
	  {
	     if (pEl == FirstSelectedElement)
	       {
		  if (pEl->ElLeafType == LtText)
		     firstChar = FirstSelectedChar;
		  else if (pEl->ElLeafType == LtPolyLine)
		     firstChar = SelectedPointInPolyline;
		  else if (pEl->ElLeafType == LtPicture)
		     firstChar = SelectedPictureEdge;
		  else
		     firstChar = 0;
	       }
	     else
		firstChar = 0;
	     if (pAb->AbElement == LastSelectedElement)
	       {
		  if (pEl->ElLeafType == LtText)
		     lastChar = LastSelectedChar;
		  else if (pEl->ElLeafType == LtPolyLine)
		     lastChar = SelectedPointInPolyline;
		  else if (pEl->ElLeafType == LtPicture)
		     lastChar = SelectedPictureEdge;
		  else
		     lastChar = 0;
	       }
	     else
		lastChar = 0;
	     unique = FirstSelectedElement == LastSelectedElement;
	     InsertViewSelMarks (frame, pAb, firstChar, lastChar, first, last,
				 unique);
	     /* faut-il que le pave selectionne' soit amene' */
	     /* dans la partie visible de l'image (scroll) ? */
	     active = view == SelectedView;
	     if (first)
		if (assoc || active || SelectedDocument->DocView[view - 1].DvSync)
		   ShowSelectedBox (frame, active);
	     first = FALSE;
	     if (last)
		/* allume la selection */
		SwitchSelection (frame, TRUE);
	  }
	pAb->AbSelected = TRUE;
	/* marque le pave selectionne' */
	/* passe au pave suivant de l'element s'il y en a un */
	pAb = pNextAb;
     }
}


/*----------------------------------------------------------------------
   SelectAbsBoxes selectionne, dans toutes les vues, tous les
   paves de l'element pEl. Si aucun pave n'existe pour cet
   element, on essaie de creer une vue ou` cet element ait un
   pave' a` condition que createView soit vrai.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static boolean      SelectAbsBoxes (PtrElement pEl, boolean createView)

#else  /* __STDC__ */
static boolean      SelectAbsBoxes (pEl, createView)
PtrElement          pEl;
boolean             createView;

#endif /* __STDC__ */

{
   PtrElement          pRoot;
   NotifyDialog        notifyDoc;
   AvailableView       viewTable;
   int                 X, Y, width, height, view, lastView, frame, run,
                       nViews, i, createdView;
   DocViewNumber       docView, freeView;
   boolean             abExist, done, assoc, deleteView;

   /* on n'a pas (encore) trouve' de pave' */
   abExist = FALSE;
   if (pEl != NULL)
     {
	if (AssocView (pEl))
	   /* les elements associes n'ont qu'une vue, la vue 1 */
	  {
	     assoc = TRUE;
	     lastView = 1;
	  }
	else
	   /* pour les autres elements on traite toutes les vues */
	  {
	     assoc = FALSE;
	     lastView = MAX_VIEW_DOC;
	  }
	/* on fait deux fois le tour des vues. Au premier tour on selectionne */
	/* les paves de l'element qui existent, au deuxieme tour on cree ceux */
	/* qui n'existent pas. */
	for (run = 1; run <= 2; run++)
	   /* deux fois le tour des vues */
	   for (view = 0; view < lastView; view++)
	      /* numero de la fenetre du Mediateur ou la selection sera */
	      /* visualisee */
	     {
		if (assoc)
		   /* vue d'elements associes */
		   frame = SelectedDocument->DocAssocFrame[pEl->ElAssocNum - 1];
		else if (SelectedDocument->DocView[view].DvPSchemaView > 0)
		   frame = SelectedDocument->DocViewFrame[view];
		else
		   frame = 0;
		if (frame != 0 && frame != FrameWithNoUpdate)
		  {
		     if (run == 1)
			ClearViewSelection (frame);
		     done = FALSE;
		     /* au deuxieme tour, on cree les paves absents */
		     if (run == 2)
			if (pEl->ElAbstractBox[view] != NULL)
			  {
			     done = TRUE;
			     /* l'element a au moins un pave */
			     abExist = TRUE;
			  }
			else
			   /* cree les paves de l'element pointe' par pEl pour */
			   /* la vue view s'il faut synchroniser cette vue */
			if (assoc || SelectedDocument->DocView[view].DvSync)
			   /* si pEl est une marque page, inutile d'appeler */
			   /* CheckAbsBox si ElViewPSchema n'est pas egal a la */
			   /* VueSch correspondant a view */
			   if (pEl->ElTypeNumber != PageBreak + 1 ||
			       pEl->ElViewPSchema == SelectedDocument->DocView[view].DvPSchemaView)
			     {
				CheckAbsBox (pEl, view + 1, SelectedDocument, FALSE, TRUE);
				if (SelectedView == 0)
				   if (pEl->ElAbstractBox[view] != NULL)
				      SetActiveView (0);
			     }
		     if (!done)
		       {
			  DisplaySel (pEl, view + 1, frame, assoc, &abExist);
			  if (run == 1)
			     XFlushOutput (frame);
		       }
		  }
	     }

	if (!abExist)
	   /* on n'a pas trouve' de pave' pour l'element */
	   if (createView)
	      /* on essaie de creer une vue ou l'element ait un pave' */
	      if (AssocView (pEl))
		 /* l'element s'affiche dans une vue d'elements associes */
		{
		   /* la vue d'elements associes est-elle deja creee ? */
		   if (SelectedDocument->DocAssocFrame[pEl->ElAssocNum - 1] == 0)
		      /* la vue n'est pas encore creee, on tente de la creer */
		      /* cherche la racine de l'arbre d'elements associes */
		     {
			pRoot = pEl;
			while (pRoot->ElParent != NULL)
			   pRoot = pRoot->ElParent;
			/* cherche dans le fichier de configuration de ce type */
			/* de document la position et la dimension de la fenetre */
			/* ou s'affiche cette vue d'elements associes */
			ConfigGetViewGeometry (SelectedDocument,
					       pRoot->ElStructSchema->SsRule[pRoot->ElTypeNumber - 1].SrName,
					       &X, &Y, &width, &height);
			notifyDoc.event = TteViewOpen;
			notifyDoc.document = (Document) IdentDocument (SelectedDocument);
			notifyDoc.view = 0;
			if (!CallEventType ((NotifyEvent *) & notifyDoc, TRUE))
			  {
			     createdView = CreateAbstractImage (SelectedDocument, 0, pRoot->ElTypeNumber,
				      pRoot->ElStructSchema, 0, TRUE, NULL);
			     OpenCreatedView (SelectedDocument, createdView,
					      TRUE, X, Y, width, height);
			     notifyDoc.event = TteViewOpen;
			     notifyDoc.document = (Document) IdentDocument (SelectedDocument);
			     notifyDoc.view = createdView + 100;
			     CallEventType ((NotifyEvent *) & notifyDoc, FALSE);
			     /* et on essaie a nouveau */
			     abExist = SelectAbsBoxes (pEl, FALSE);
			  }
		     }
		}
	      else
		 /* l'element s'affiche dans une vue de l'arbre principal */
		{
		   /* on essaie d'ouvrir successivement toutes les vues */
		   /* definies dans le schema de presentation et qui ne sont */
		   /* pas encore ouvertes, jusqu'a en trouver une ou` */
		   /* l'element ait un pave' */
		   /* cherche d'abord s'il existe une vue libre dans le */
		   /* descripteur du document */
		   docView = 1;
		   freeView = 0;
		   while (freeView == 0 && docView <= MAX_VIEW_DOC)
		      if (SelectedDocument->DocView[docView - 1].DvPSchemaView == 0)
			 freeView = docView;
		      else
			 docView++;
		   if (freeView != 0)
		      /* il y a de la place au moins pour une vue de l'arbre */
		      /* principal */
		     {
			/* construit la liste de toutes les vues possibles pour */
			/* le document */
			nViews = BuildDocumentViewList (SelectedDocument, viewTable);
			/* essaie toutes ces vues successivement */
			for (i = 0; i < nViews && !abExist; i++)
			  {
			     /* on n'essaie que les vues de l'arbre principal */
			     /* qui ne sont pas deja ouvertes */
			     if (!viewTable[i].VdAssoc)
				if (!viewTable[i].VdOpen)
				  {
				     /* cree la vue */
				     createdView = CreateAbstractImage (SelectedDocument,
						     viewTable[i].VdView, 0,
						  viewTable[i].VdSSchema, 1,
							       FALSE, NULL);
				     /* on essaie a nouveau de selectionner l'element */
				     abExist = SelectAbsBoxes (pEl, FALSE);
				     deleteView = TRUE;
				     if (pEl->ElAbstractBox[createdView - 1] != NULL)
					/* il y a un pave visible pour l'element */
				       {
					  deleteView = FALSE;
					  /* on a reussi, on s'arrete */
					  abExist = TRUE;
					  /* cherche la geometrie de la vue dans le */
					  /* fichier de configuration du type de doc */
					  ConfigGetViewGeometry (SelectedDocument,
								 viewTable[i].VdViewName, &X, &Y, &width, &height);
					  notifyDoc.event = TteViewOpen;
					  notifyDoc.document =
					     (Document) IdentDocument (SelectedDocument);
					  notifyDoc.view = createdView;
					  if (CallEventType ((NotifyEvent *) & notifyDoc,
							      TRUE))
					    /* application does not want Thot
					       to create a view */
					    deleteView = TRUE;
					  else
					    {
					       OpenCreatedView (SelectedDocument, createdView, assoc, X, Y,
							     width, height);
					       notifyDoc.event = TteViewOpen;
					       notifyDoc.document =
						  (Document) IdentDocument (SelectedDocument);
					       notifyDoc.view = createdView;
					       CallEventType ((NotifyEvent *) & notifyDoc,
							      FALSE);
					    }
				       }
				     if (deleteView)
					/* l'element n'est pas visible dans cette vue */
					/* or the application has created the view */
					/* on detruit l'image creee et on continue... */
				     if (assoc)
				       {
					  LibAbbView (SelectedDocument->DocAssocRoot[createdView - 1]
						      ->ElAbstractBox[0]);
					  SelectedDocument->DocAssocFrame[createdView - 1] = 0;
				       }
				     else
					FreeView (SelectedDocument, createdView);
				  }
			  }
		     }
		}
     }
   return abExist;
}


/*----------------------------------------------------------------------
   HighlightVisibleAncestor
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                HighlightVisibleAncestor (PtrElement pEl)

#else  /* __STDC__ */
void                HighlightVisibleAncestor (pEl)
PtrElement          pEl;

#endif /* __STDC__ */

{
   int                 view, lastView, frame;
   boolean             assoc, found, abExist;
   PtrElement          pAncest;

   view = 0;
   if (pEl != NULL)
     {
	if (AssocView (pEl))
	   /* les elements associes n'ont qu'une vue, la vue 1 */
	  {
	     assoc = TRUE;
	     lastView = 1;
	  }
	else
	   /* pour les autres elements on traite toutes les vues */
	  {
	     assoc = FALSE;
	     lastView = MAX_VIEW_DOC;
	  }
	found = FALSE;
	pAncest = pEl->ElParent;
	/* cherche le premier element englobant pEl qui ait un pave dans au */
	/* moins une vue */
	while (!found && pAncest != NULL)
	  {
	     view = 1;
	     /* parcourt toutes les vues */
	     while (view <= lastView && !found)
		if (pAncest->ElAbstractBox[view - 1] != NULL)
		   found = TRUE;
		else
		   view++;
	     if (!found)
		pAncest = pAncest->ElParent;
	  }
	if (pAncest != NULL)
	   /* on a trouve un englobant qui ait un pave' */
	  {
	     if (assoc)
		/* vue d'elements associes */
		frame = SelectedDocument->DocAssocFrame[pEl->ElAssocNum - 1];
	     else if (SelectedDocument->DocView[view - 1].DvPSchemaView > 0)
		frame = SelectedDocument->DocViewFrame[view - 1];
	     else
		frame = 0;
	     /* allume l'element englobant trouve' */
	     DisplaySel (pAncest, view, frame, assoc, &abExist);
	  }
     }
}


/*----------------------------------------------------------------------
   SelectStringInAttr la selection courante devient la chaine de
   caracteres allant du caractere de rang Prem au caractere
   de rang lastChar dans le buffer de texte du pave pAb.
   pAb est un pave de presentation contenant la valeur     
   d'un attribut numerique ou textuel.                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         SelectStringInAttr (PtrDocument pDoc, PtrAbstractBox pAb, int firstChar, int lastChar, boolean string)
#else  /* __STDC__ */
static void         SelectStringInAttr (pDoc, pAb, firstChar, lastChar, string)
PtrDocument         pDoc;
PtrAbstractBox      pAb;
int                 firstChar;
int                 lastChar;
boolean             string;

#endif /* __STDC__ */
{
   PtrElement          pEl;
   PtrAbstractBox      pAbView;
   int                 frame, lastView, view;
   boolean             assoc;

   if (pAb == NULL || pDoc == NULL)
      return;
   pEl = pAb->AbElement;
   if (pEl->ElIsCopy)
      /* la chaine a selectionner est dans un element copie' */
      /* on selectionne tout cet element. SelectElement selectionnera le */
      /* premier englobant qui n'est pas une copie. */
      SelectElement (pDoc, pEl, TRUE, TRUE);
   else
     {
	DocSelectedAttr = pDoc;
	AbsBoxSelectedAttr = pAb;
	FirstSelectedCharInAttr = firstChar;
	LastSelectedCharInAttr = lastChar;
	/* determine les vues a traiter */
	assoc = AssocView (AbsBoxSelectedAttr->AbElement);
	if (assoc)
	   /* vue d'elements associes, on ne traite que la vue 1 */
	   lastView = 1;
	else
	   /* on traite toutes les vues du document */
	   lastView = MAX_VIEW_DOC;
	SelPosition = !string;
	/* pour chaque vue a traiter, allume la nouvelle selection */
	for (view = 1; view <= lastView; view++)
	  {
	     /* calcule le numero de la fenetre ou est affichee la vue */
	     if (assoc)
		frame = pDoc->DocAssocFrame[pAb->AbElement->ElAssocNum - 1];
	     else if (pDoc->DocView[view - 1].DvPSchemaView > 0)
		frame = pDoc->DocViewFrame[view - 1];
	     else
		frame = 0;	/* vue non creee */
	     /* si la vue est creee, traite la selection dans cette vue */
	     if (frame > 0)
	       {
		  /* cherche dans la vue le pave de presentation d'attribut */
		  /* correspondant a AbsBoxSelectedAttr */
		  pAbView = GetAbsBoxSelectedAttr (view);
		  /* eteint toute la selection dans cette vue */
		  ClearViewSelection (frame);
		  if (pAbView != NULL)
		    {
		       InsertViewSelMarks (frame, pAbView, firstChar,
					   lastChar, TRUE, TRUE, TRUE);
		       ShowSelectedBox (frame, TRUE);
		       SwitchSelection (frame, TRUE);
		    }
	       }
	  }
	PrepareSelectionMenu ();
	BuildSelectionMessage ();
	if (ThotLocalActions[T_chselect] != NULL)
	   (*ThotLocalActions[T_chselect]) (pDoc);
	if (ThotLocalActions[T_chattr] != NULL)
	   (*ThotLocalActions[T_chattr]) (pDoc);
     }
}

/*----------------------------------------------------------------------
   SelectString la selection courante devient la chaine allant du  
   caractere de rang firstChar au caractere de rang
   		lastChar dans l'element texte pointe par pEl dans le
   		document pDoc.	
   S'il s'agit d'une feuille Polyline, c'est le point de   
   rang firstChar qui est selectionne'.                    
   string indique si on selectionne une chaine ou une
   position entre deux caracteres.                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         SelectStringOrPosition (PtrDocument pDoc, PtrElement pEl, int firstChar, int lastChar, boolean string)
#else  /* __STDC__ */
static void         SelectStringOrPosition (pDoc, pEl, firstChar, lastChar, string)
PtrDocument         pDoc;
PtrElement          pEl;
int                 firstChar;
int                 lastChar;
boolean             string;

#endif /* __STDC__ */
{
   int                 i;
   boolean             elVisible;
   PtrElement          oldFirstSelEl;
   int                 oldFirstSelChar;
   PtrElement          pAncest;
   boolean             holophrast;

   /* Si l'element est dans un arbre holophraste', on prend */
   /* l'ascendant qui est holophraste' */
   pAncest = pEl;
   holophrast = FALSE;
   while (pAncest != NULL && !holophrast)
      if (pAncest->ElHolophrast)
	 holophrast = TRUE;
      else
	 pAncest = pAncest->ElParent;

   if (pEl != NULL && pDoc != NULL)
      /* ignorer l'exception NoSelect */
      if (pEl->ElIsCopy || holophrast)
	 /* la chaine a selectionner est dans un element copie' ou holophraste' */
	 /* on selectionne tout cet element. SelectElement selectionnera le */
	 /* premier englobant qui n'est pas une copie. */
	 SelectElement (pDoc, pEl, TRUE, TRUE);
      else
	{
	   oldFirstSelEl = FirstSelectedElement;
	   oldFirstSelChar = FirstSelectedChar;
	   if (pEl->ElLeafType == LtText)
	      if (lastChar == 0)
		 lastChar = pEl->ElTextLength;
	      else if (lastChar < firstChar)
		{
		   i = lastChar;
		   lastChar = firstChar;
		   firstChar = i;
		}
	   /* la nouvelle selection est-elle dans un arbre different de */
	   /* l'ancienne ? */
	   FirstSelectedElement = pEl;
	   if (pDoc != SelectedDocument)
	     {
		CancelSelection ();
		SelectedDocument = pDoc;
		FirstSelectedElement = pEl;
		SetActiveView (0);
	     }
	   else if (LastSelectedElement != NULL)
	      if ((AssocView (pEl) || AssocView (LastSelectedElement))
		  && pEl->ElAssocNum != LastSelectedElement->ElAssocNum)
		{
		   TtaClearViewSelections ();
		   SetActiveView (0);
		}
	   LastSelectedElement = FirstSelectedElement;
	   if (pEl->ElLeafType == LtPolyLine)
	     {
		SelectedPointInPolyline = firstChar;
		FirstSelectedChar = 0;
		LastSelectedChar = 0;
	     }
	   else if (pEl->ElLeafType == Picture)
	     {
		SelectedPictureEdge = firstChar;
		FirstSelectedChar = 0;
		LastSelectedChar = 0;
	     }
	   else
	     {
		FirstSelectedChar = firstChar;
		LastSelectedChar = lastChar;
	     }
	   /* element fixe de la selection */
	   FixedElement = pEl;
	   /* caractere fixe de la selection */
	   FixedChar = FirstSelectedChar;
	   SelContinue = TRUE;
	   NSelectedElements = 0;
	   SelPosition = !string;
	   /* allume les paves de la selection courante */
	   elVisible = SelectAbsBoxes (pEl, TRUE);
	   if (!elVisible)
	      /* la selection n'est pas visible, on allume le premier element */
	      /* englobant visible */
	      HighlightVisibleAncestor (pEl);

	   /* met a jour les menus qui doivent changer dans toutes les vues */
	   /* ouvertes */
	   if (SelectionUpdatesMenus && oldFirstSelEl != FirstSelectedElement)
	     {
		PrepareSelectionMenu ();
		BuildSelectionMessage ();
		if (ThotLocalActions[T_chselect] != NULL)
		   (*ThotLocalActions[T_chselect]) (pDoc);
		if (ThotLocalActions[T_chattr] != NULL)
		   (*ThotLocalActions[T_chattr]) (pDoc);
	     }
	   else
	      /* la selection demarre dans le meme element que precedemment */
	      if ((oldFirstSelChar <= 1 && FirstSelectedChar > 1) ||
		  (oldFirstSelChar > 1 && FirstSelectedChar <= 1))
	      /* la selection etait au debut de l'element et n'y est plus */
	      /* ou elle etait a l'interieur de l'element et elle est */
	      /* maintenant au debut */
	      /* l'entree Split du menu Edit doit etre mise a jour */
	      if (SelectionUpdatesMenus)
		 if (ThotLocalActions[T_chsplit] != NULL)
		    (*ThotLocalActions[T_chsplit]) (pDoc);
	}
}


/*----------------------------------------------------------------------
   MoveCaret		
   		la selection courante devient la position du caractere
   		de rang firstChar dans l'element texte pointe par pEl
   dans le document pDoc.
   S'il s'agit d'une feuille Polyline, c'est le point de   
   rang firstChar qui est selectionne'.                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                MoveCaret (PtrDocument pDoc, PtrElement pEl, int firstChar)
#else  /* __STDC__ */
void                MoveCaret (pDoc, pEl, firstChar)
PtrDocument         pDoc;
PtrElement          pEl;
int                 firstChar;

#endif /* __STDC__ */
{
   SelectStringOrPosition (pDoc, pEl, firstChar, firstChar, FALSE);
}

/*----------------------------------------------------------------------
   SelectString la selection courante devient la chaine allant du  
   caractere de rang firstChar au caractere de rang lastChar dans  
   l'element texte pointe par pEl dans le document pDoc.   
   S'il s'agit d'une feuille Polyline, c'est le point de   
   rang firstChar qui est selectionne'.                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SelectString (PtrDocument pDoc, PtrElement pEl, int firstChar, int lastChar)
#else  /* __STDC__ */
void                SelectString (pDoc, pEl, firstChar, lastChar)
PtrDocument         pDoc;
PtrElement          pEl;
int                 firstChar;
int                 lastChar;

#endif /* __STDC__ */
{
   boolean             string;

   /* it's a string, not a position within a string */
   string = TRUE;
   if (firstChar == lastChar)
      /* it's a position */
      string = FALSE;
   SelectStringOrPosition (pDoc, pEl, firstChar, lastChar, string);
}


/*----------------------------------------------------------------------
   SelectElement l'element pointe par pEl dans le document pDoc devient
   l'element selectionne. Dans le cas ou cet element ne    
   doit pas etre montre' a l'utilisateur, c'est son premier
   ou dernier fils qui est selectionne' (selon le booleen  
   begin), sauf si check est faux.                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SelectElement (PtrDocument pDoc, PtrElement pEl, boolean begin, boolean check)
#else  /* __STDC__ */
void                SelectElement (pDoc, pEl, begin, check)
PtrDocument         pDoc;
PtrElement          pEl;
boolean             begin;
boolean             check;

#endif /* __STDC__ */
{
   PtrElement          pAncest, pE;
   boolean             bool, stop, elVisible;

   if (pEl != NULL && pDoc != NULL)
     {
	if (check)
	  {
	     /* on ne selectionne que ce qui est presentable a l'utilisateur */
	     stop = FALSE;
	     while (!stop)
		if (!TypeHasException (ExcHidden, pEl->ElTypeNumber, pEl->ElStructSchema))
		  {
		     /* ce type d'element ne porte pas l'exception Hidden */
		     stop = TRUE;
		     if (pEl->ElFirstChild != NULL)
			/* accede a la regle qui definit le type de l'element */
			if (pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrConstruct ==
			    CsChoice)
			  {
			     /* c'est un choix avec un fils, on prend le fils */
			     pEl = pEl->ElFirstChild;
			     stop = FALSE;
			  }
		  }
		else
		   /* ce type d'element porte l'exception Hidden */
		if (pEl->ElTerminal || pEl->ElFirstChild == NULL)
		   /* l'element n'a pas de fils */
		   stop = TRUE;
		else
		  {
		     /* on prend le premier ou le dernier fils de */
		     /* l'element selon le booleen begin */
		     pEl = pEl->ElFirstChild;
		     if (!begin)
			while (pEl->ElNext != NULL)
			   pEl = pEl->ElNext;
		  }

	  }
	/* Si l'element a selectionner est une copie protegee, change pour */
	/* l'element englobant protege' de plus haut niveau. */
	if (pEl->ElIsCopy)
	  {
	     pAncest = pEl->ElParent;
	     stop = FALSE;
	     while (!stop)
		if (pAncest == NULL)
		   stop = TRUE;
		else if (!pAncest->ElIsCopy)
		   stop = TRUE;
		else
		  {
		     pEl = pAncest;
		     pAncest = pAncest->ElParent;
		  }
	  }
	/* Si l'element est dans un arbre holophraste', on prend */
	/* l'ascendant qui est holophraste' */
	pAncest = pEl;
	do
	  {
	     pAncest = pAncest->ElParent;
	     if (pAncest != NULL)
		if (pAncest->ElHolophrast)
		  {
		     pEl = pAncest;
		     pAncest = NULL;
		  }
	  }
	while (pAncest != NULL);

	/* la nouvelle selection est-elle dans un arbre different de */
	/* l'ancienne ? */
	FirstSelectedElement = pEl;
	if (pDoc != SelectedDocument)
	  {
	     CancelSelection ();
	     SelectedDocument = pDoc;
	     FirstSelectedElement = pEl;
	     SetActiveView (0);
	  }
	else if (LastSelectedElement != NULL)
	   if ((AssocView (pEl) || AssocView (LastSelectedElement))
	       && pEl->ElAssocNum != LastSelectedElement->ElAssocNum)
	     {
		TtaClearViewSelections ();
		SetActiveView (0);
	     }
	/* si la selection reste dans le meme arbre, SelectAbsBoxes eteindra */
	/* l'ancienne selection */
	/* ignorer l'exception NoSelect */
	SelContinue = TRUE;
	NSelectedElements = 0;
	LastSelectedElement = FirstSelectedElement;
	SelectedPointInPolyline = 0;
	SelectedPictureEdge = 0;
	FirstSelectedChar = 0;
	LastSelectedChar = 0;
	FixedElement = pEl;	/* element fixe de la selection */
	FixedChar = 0;		/* caractere fixe de la selection */
	/* si l'element est vide on considere que c'est un point d'insertion */
	/* dans l'element et non pas une selection de l'element */
	/* de meme si c'est une image */
	if ((pEl->ElTerminal &&
	     (pEl->ElVolume == 0 || pEl->ElLeafType == LtPicture)) ||
	    (!pEl->ElTerminal && pEl->ElFirstChild == NULL))
	   SelPosition = TRUE;
	else
	   SelPosition = FALSE;
	elVisible = SelectAbsBoxes (FirstSelectedElement, TRUE);
	if (!elVisible)
	   /* la selection n'est pas visible, on allume le premier element */
	   /* englobant visible */
	   HighlightVisibleAncestor (FirstSelectedElement);
	/* cherche une nouvelle vue active */
	/* appelle la procedure traitant les exceptions pour la selection */
	/* dans les tableaux */
	if (ThotLocalActions[T_selecttable] != NULL)
	   (*ThotLocalActions[T_selecttable]) (FirstSelectedElement,
					    SelectedDocument, FALSE, &bool);
	/* si c'est une marque qui est selectionnee, on selectionne aussi */
	/* la marque qui fait la paire */
	if (FirstSelectedElement->ElStructSchema->SsRule[FirstSelectedElement->ElTypeNumber - 1].SrConstruct ==
	    CsPairedElement)
	  {
	     AddInSelection (GetOtherPairedElement (FirstSelectedElement), TRUE);
	     if (!FirstSelectedElement->ElStructSchema->SsRule[FirstSelectedElement->ElTypeNumber - 1].
		 SrFirstOfPair)
		/* le premier selectionne' est une marque de fin, on permute */
	       {
		  pE = FirstSelectedElement;
		  FirstSelectedElement = LastSelectedElement;
		  LastSelectedElement = pE;
		  SelectedElement[0] = FirstSelectedElement;
		  SelectedElement[1] = LastSelectedElement;
	       }
	  }

	if (FirstSelectedElement != NULL)
	   if (FirstSelectedElement->ElTerminal)
	     {
		if (FirstSelectedElement->ElLeafType == LtSymbol)
		   /* affiche le clavier des symboles math. */
		   TtaSetCurrentKeyboard (0);
		else if (FirstSelectedElement->ElLeafType == LtGraphics)
		   /* affiche le clavier des graphiques */
		   TtaSetCurrentKeyboard (1);
		else if (FirstSelectedElement->ElLeafType == LtPolyLine)
		   /* affiche le clavier des graphiques */
		   TtaSetCurrentKeyboard (1);
	     }
	/* met a jour les menus qui dependent de la selection dans */
	/* toutes les vues ouvertes */
	if (SelectionUpdatesMenus)
	  {
	     PrepareSelectionMenu ();
	     BuildSelectionMessage ();
	     if (ThotLocalActions[T_chselect] != NULL)
		(*ThotLocalActions[T_chselect]) (pDoc);
	     if (ThotLocalActions[T_chattr] != NULL)
		(*ThotLocalActions[T_chattr]) (pDoc);
	  }
     }
}


/*----------------------------------------------------------------------
   ExtendSelection etend la selection courante jusqu'a l'element
   pEl, ce dernier etant entierement selectionne' si
   rank=0, ou selectionne jusqu'au caractere de rang       
   rank sinon. Si fixed est vrai, la nouvelle selection    
   est la partie entre le point fixe courant et le point   
   d'extension passe' en parametre. Le parametre begin n'a 
   pas de signification dans ce cas. Si fixed est faux,    
   le debut de la selection est etendu jusqu'au point      
   d'extension dans le cas ou begin est vrai, ou la fin de 
   la selection est etendue jusqu'au point d'extension si  
   begin est faux.                                         
   Si drag est vrai, l'extension de selection se fait par  
   dragging: on fait un traitement minimum.                
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                ExtendSelection (PtrElement pEl, int rank, boolean fixed, boolean begin, boolean drag)

#else  /* __STDC__ */
void                ExtendSelection (pEl, rank, fixed, begin, drag)
PtrElement          pEl;
int                 rank;
boolean             fixed;
boolean             begin;
boolean             drag;

#endif /* __STDC__ */

{
   PtrElement          oldFirstEl, oldLastEl, pElP;
   int                 oldFirstChar, oldLastChar;
   boolean             change, done, sel;

   sel = TRUE;
   if (pEl != NULL)
      /* appelle la procedure traitant les exceptions pour la selection */
      /* dans les tableaux */
      if (ThotLocalActions[T_selecttable] != NULL)
	 (*ThotLocalActions[T_selecttable]) (pEl, SelectedDocument, TRUE, &sel);
   if (sel)
      /* appelle la procedure traitant les exceptions pour la selection */
      /* dans les dessins */
     {
	done = FALSE;
	if (!SelContinue || pEl != FirstSelectedElement)
	   if (DrawEtendSelection (pEl, SelectedDocument))
	      done = TRUE;
	if (!done)
	  {
	     oldFirstEl = FirstSelectedElement;
	     oldLastEl = LastSelectedElement;
	     oldFirstChar = FirstSelectedChar;
	     oldLastChar = LastSelectedChar;
	     SelectedPointInPolyline = 0;
	     SelectedPictureEdge = 0;
	     if (pEl->ElHolophrast)
		/* on est dans un element holophraste',on le selectionne en entier */
		rank = 0;
	     if (!fixed && begin)
		/* on change le point fixe, en le mettant d'abord a la fin */
	       {
		  FixedElement = LastSelectedElement;
		  if (LastSelectedChar < rank)
		     LastSelectedChar = rank;
		  FixedChar = LastSelectedChar;
	       }
	     if (pEl == FixedElement)
		/* extension dans l'element fixe */
	       {
		  FirstSelectedElement = pEl;
		  FirstSelectedChar = 0;
		  LastSelectedElement = pEl;
		  LastSelectedChar = 0;
		  if (FixedChar > 0)
		     if (rank < FixedChar)
		       {
			  FirstSelectedChar = rank;
			  LastSelectedChar = FixedChar;
		       }
		     else
		       {
			  FirstSelectedChar = FixedChar;
			  LastSelectedChar = rank;
		       }
	       }
	     else
		/* extension en dehors de l'element fixe */
	     if (ElemIsAnAncestor (pEl, FixedElement))
		/* on a designe' un englobant de l'element fixe, on selectionne */
		/* cet element englobant */
	       {
		  FirstSelectedElement = pEl;
		  FirstSelectedChar = 0;
		  LastSelectedElement = pEl;
		  LastSelectedChar = 0;
		  FixedElement = pEl;
		  FixedChar = 0;
	       }
	     else
		/* cherche s'il est avant l'element fixe */
	     if (ElemIsBefore (pEl, FixedElement))
	       {
		  FirstSelectedElement = pEl;
		  FirstSelectedChar = rank;
		  LastSelectedElement = FixedElement;
		  LastSelectedChar = FixedChar;
	       }
	     else
		/* il est apres l'element fixe */
	       {
		  LastSelectedElement = pEl;
		  LastSelectedChar = rank;
		  FirstSelectedElement = FixedElement;
		  FirstSelectedChar = FixedChar;
	       }
	     /* ajuste la selection */
	     if (FirstSelectedElement->ElTerminal &&
		 FirstSelectedElement->ElLeafType == LtText &&
		 FirstSelectedElement->ElTextLength > 0 &&
		 FirstSelectedElement->ElTextLength < FirstSelectedChar &&
		 FirstSelectedElement != LastSelectedElement)
		/* Le debut de selection est a la fin de l'element de texte */
		/* FirstSelectedElement et ce n'est pas la fin de la selection */
		/* On met le debut de selection au debut de l'element suivant */
	       {
		  FirstSelectedElement = FirstLeaf (NextElement (FirstSelectedElement));
		  if (FirstSelectedElement->ElTerminal && FirstSelectedElement->ElLeafType == LtText)
		     FirstSelectedChar = 1;
		  else
		     FirstSelectedChar = 0;
	       }
	     if (StructSelectionMode)
		/* on normalise la selection: le premier et le dernier element */
		/* selectionne's doivent etre des freres dans l'arbre abstrait */
		if (FirstSelectedElement->ElParent != LastSelectedElement->ElParent)
		  {
		     /* remonte les ascendants du premier element de la selection */
		     pElP = FirstSelectedElement->ElParent;
		     while (pElP != NULL)
			if (ElemIsAnAncestor (pElP, LastSelectedElement))
			   /* cet ascendant (pElP) du premier selectionne' est aussi */
			   /* un ascendant du dernier selectionne' */
			  {
			     if (LastSelectedElement->ElParent != pElP)
				LastSelectedChar = 0;
			     /* on retient comme dernier l'ascendant du dernier qui a */
			     /* pElP pour pere */
			     while (LastSelectedElement->ElParent != pElP)
				LastSelectedElement = LastSelectedElement->ElParent;
			     /* on a fini */
			     pElP = NULL;
			  }
			else
			   /* cet ascendant (pElP) du premier selectionne' n'est pas */
			   /* un ascendant du dernier selectionne' */
			  {
			     /* on retient pour l'instant pElP et on va regarder si */
			     /* son pere est un ascendant du dernier selectionne' */
			     FirstSelectedElement = pElP;
			     /* on retient l'element entier */
			     FirstSelectedChar = 0;
			     pElP = pElP->ElParent;
			  }
		  }
	     change = FALSE;
	     if (oldFirstEl != FirstSelectedElement
		 || oldLastEl != LastSelectedElement
		 || oldFirstChar != FirstSelectedChar
		 || oldLastChar != LastSelectedChar)
	       {
		  change = TRUE;
		  if (FirstSelectedChar == 0)
		     while (FirstSelectedElement->ElNext == NULL
			    && FirstSelectedElement->ElPrevious == NULL
			    && FirstSelectedElement->ElParent != NULL)
			FirstSelectedElement = FirstSelectedElement->ElParent;
		  if (ElemIsAnAncestor (FirstSelectedElement, LastSelectedElement))
		    {
		       LastSelectedElement = FirstSelectedElement;
		       LastSelectedChar = 0;
		    }
		  if (LastSelectedChar == 0)
		     while (LastSelectedElement->ElNext == NULL
			    && LastSelectedElement->ElPrevious == NULL
			    && LastSelectedElement->ElParent != NULL)
			LastSelectedElement = LastSelectedElement->ElParent;
		  if (ElemIsAnAncestor (LastSelectedElement, FirstSelectedElement))
		    {
		       FirstSelectedElement = LastSelectedElement;
		       FirstSelectedChar = 0;
		    }
	       }

	     SelPosition = FALSE;
	     if (!change)
		if (drag)
		   if (FirstSelectedElement == LastSelectedElement &&
		       FirstSelectedElement->ElVolume == 0)
		      SelPosition = TRUE;

	     if (change || !drag)
		/* la nouvelle selection est differente de l'ancienne */
		/* on la visualise */
		HighlightSelection (FirstSelectedElement != oldFirstEl);
	     if (!drag)
		/* met a jour les menus qui dependent de la selection dans */
		/* toutes les vues ouvertes */
		if (SelectionUpdatesMenus)
		   if (oldFirstEl != FirstSelectedElement ||
		       oldLastEl != LastSelectedElement)
		     {
			PrepareSelectionMenu ();
			BuildSelectionMessage ();
			if (ThotLocalActions[T_chselect] != NULL)
			   (*ThotLocalActions[T_chselect]) (SelectedDocument);
			if (ThotLocalActions[T_chattr] != NULL)
			   (*ThotLocalActions[T_chattr]) (SelectedDocument);
		     }
	     if (!fixed && begin)
		/* on change le point fixe, en le mettant au debut */
	       {
		  FixedElement = FirstSelectedElement;
		  FixedChar = FirstSelectedChar;
	       }
	  }
     }
}


/*----------------------------------------------------------------------
   ReverseSelect   allume ou eteint (selon le booleen highlight) tous
   les paves de l'element pEl du document pDoc.    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ReverseSelect (PtrElement pEl, PtrDocument pDoc, boolean highlight)

#else  /* __STDC__ */
static void         ReverseSelect (pEl, pDoc, highlight)
PtrElement          pEl;
PtrDocument         pDoc;
boolean             highlight;

#endif /* __STDC__ */

{
   PtrAbstractBox      pAb;

#ifdef __COLPAGE__
   PtrAbstractBox      pNextAb, pSavedAb;

#endif /* __COLPAGE__ */
   int                 lastView, view, frame;

   if (AssocView (pEl))
      /* les elements associes n'ont qu'une vue, la vue 1 */
      lastView = 1;
   else
      /* pour les autres elements on traite toutes les vues */
      lastView = MAX_VIEW_DOC;
   for (view = 0; view < lastView; view++)
     {
	/* cherche le numero de la fenetre correspondant a la vue view */
	if (AssocView (pEl))
	   /* vue d'elements associes */
	   frame = pDoc->DocAssocFrame[pEl->ElAssocNum - 1];
	else if (pDoc->DocView[view].DvPSchemaView > 0)
	   frame = pDoc->DocViewFrame[view];
	else
	   frame = 0;
	/* boucle sur les paves de l'element dans la vue */
	pAb = pEl->ElAbstractBox[view];
	while (pAb != NULL)
	  {
	     /* marque le pave */
	     pAb->AbSelected = highlight;
	     SetNewSelectionStatus (frame, pAb, highlight);
	     /* passe au pave suivant de l'element s'il y en a un */
#ifdef __COLPAGE__
	     if (!pAb->AbPresentationBox)
		pSavedAb = pAb;	/* pour parcours des dups */
	     pNextAb = pAb->AbNext;
	     if (pNextAb != NULL)
		if (pNextAb->AbElement != pEl)
		   /* le pave suivant n'appartient pas a l'element */
		   pAb = pSavedAb->AbNextRepeated;
		else
		   pAb = pNextAb;
	     else
		/* pNextAb est NULL, le pave a peut-etre un dup */
		pAb = pSavedAb->AbNextRepeated;
#else  /* __COLPAGE__ */
	     pAb = pAb->AbNext;
	     if (pAb != NULL)
		if (pAb->AbElement != pEl)
		   /* le pave suivant n'appartient pas a l'element */
		   pAb = NULL;
#endif /* __COLPAGE__ */
	  }
     }
}

/*----------------------------------------------------------------------
   AddInSelection ajoute a la selection courante l'element pEl.    
   last indique s'il s'agit du dernier element ajoute' a la
   selection courante.                                             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                AddInSelection (PtrElement pEl, boolean last)
#else  /* __STDC__ */
void                AddInSelection (pEl, last)
PtrElement          pEl;
boolean             last;

#endif /* __STDC__ */
{
   int                 i;
   boolean             ok;

   if (pEl != NULL)
     {
	if (SelContinue)
	  {
	     /* la selection etait continue, on passe a une selection */
	     /* discontinue */
	     NSelectedElements = 0;
	     if (FirstSelectedElement != NULL)
	       {
		  if (FirstSelectedElement->ElTerminal)
		     if ((FirstSelectedElement->ElLeafType == LtText &&
			  FirstSelectedChar > 1) ||
			 (FirstSelectedElement->ElLeafType == LtPolyLine &&
			  SelectedPointInPolyline > 0))
			SelectElement (SelectedDocument, FirstSelectedElement, TRUE, TRUE);
		  SelectedElement[0] = FirstSelectedElement;
		  NSelectedElements = 1;
	       }
	     SelContinue = FALSE;
	  }
	SelectedPointInPolyline = 0;
	SelectedPictureEdge = 0;
	/* verifie que l'element a ajouter n'est pas deja dans la selection */
	i = 1;
	ok = TRUE;
	do
	   if (SelectedElement[i - 1] == pEl)
	      ok = FALSE;
	   else
	      i++;
	while (ok && i <= NSelectedElements);
	if (NSelectedElements < MAX_SEL_ELEM && ok)
	  {
	     NSelectedElements++;
	     SelectedElement[NSelectedElements - 1] = pEl;
	     LastSelectedElement = pEl;
	     /* allume l'element dans toutes les vues ou il est visible */
	     ReverseSelect (pEl, SelectedDocument, TRUE);
	     if (last && SelectionUpdatesMenus)
	       {
		  /* met a jour les menus qui dependent de la selection dans */
		  /* toutes les vues ouvertes */
		  if (ThotLocalActions[T_chselect] != NULL)
		     (*ThotLocalActions[T_chselect]) (SelectedDocument);
		  if (ThotLocalActions[T_chattr] != NULL)
		     (*ThotLocalActions[T_chattr]) (SelectedDocument);
	       }
	  }
     }
}

/*----------------------------------------------------------------------
   RemoveFromSelection retire l'element pEl de la selection courante, 
   mais seulement si la selection est discontinue. 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                RemoveFromSelection (PtrElement pEl, PtrDocument pDoc)

#else  /* __STDC__ */
void                RemoveFromSelection (pEl, pDoc)
PtrElement          pEl;
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   int                 i, j;

   /* on ne traite que le cas de la selection discontinue */
   if (SelContinue)
      return;
   /* cherche l'element parmi ceux de la selection */
   for (i = 0; i < NSelectedElements && SelectedElement[i] != pEl; i++) ;
   if (SelectedElement[i] == pEl)
     {
	/* supprime l'element de la selection */
	for (j = i + 1; j < NSelectedElements; j++)
	   SelectedElement[j - 1] = SelectedElement[j];
	NSelectedElements--;
	/* eteint l'element dans toutes les vues ou` il est visible */
	ReverseSelect (pEl, pDoc, FALSE);
	if (LatestReturned == i + 1)
	   LatestReturned = i;
     }
}

/*----------------------------------------------------------------------
   	SelectElementWithEvent	
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SelectElementWithEvent (PtrDocument pDoc, PtrElement pEl, boolean begin, boolean check)

#else  /* __STDC__ */
void                SelectElementWithEvent (pDoc, pEl, begin, check)
PtrDocument         pDoc;
PtrElement          pEl;
boolean             begin;
boolean             check;

#endif /* __STDC__ */

{
   NotifyElement       notifyEl;

   if (pDoc != NULL && pEl != NULL)
     {
	notifyEl.event = TteElemSelect;
	notifyEl.document = (Document) IdentDocument (pDoc);
	notifyEl.element = (Element) pEl;
	notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
	notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
	notifyEl.position = 0;
	if (!CallEventType ((NotifyEvent *) & notifyEl, TRUE))
	  {
	     SelectElement (pDoc, pEl, begin, check);
	     notifyEl.event = TteElemSelect;
	     notifyEl.document = (Document) IdentDocument (pDoc);
	     notifyEl.element = (Element) pEl;
	     notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
	     notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
	     notifyEl.position = 0;
	     CallEventType ((NotifyEvent *) & notifyEl, FALSE);
	  }
     }
}

/*----------------------------------------------------------------------
   SelectPositionWithEvent   agit comme MoveCaret, mais envoie
   en plus les evenements TteElemSelect.Pre et             
   TteElemSelect.Post                                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SelectPositionWithEvent (PtrDocument pDoc, PtrElement pEl, int first)

#else  /* __STDC__ */
void                SelectStringWithEvent (pDoc, pEl, first)
PtrDocument         pDoc;
PtrElement          pEl;
int                 first;

#endif /* __STDC__ */

{
   NotifyElement       notifyEl;

   if (pDoc != NULL && pEl != NULL)
     {
	notifyEl.event = TteElemSelect;
	notifyEl.document = (Document) IdentDocument (pDoc);
	notifyEl.element = (Element) pEl;
	notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
	notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
	notifyEl.position = 0;
	if (!CallEventType ((NotifyEvent *) & notifyEl, TRUE))
	  {
	     SelectStringOrPosition (pDoc, pEl, first, first, FALSE);
	     notifyEl.event = TteElemSelect;
	     notifyEl.document = (Document) IdentDocument (pDoc);
	     notifyEl.element = (Element) pEl;
	     notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
	     notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
	     notifyEl.position = 0;
	     CallEventType ((NotifyEvent *) & notifyEl, FALSE);
	  }
     }
}

/*----------------------------------------------------------------------
   SelectStringWithEvent     agit comme SelectString, mais envoie
   en plus les evenements TteElemSelect.Pre et             
   TteElemSelect.Post                                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SelectStringWithEvent (PtrDocument pDoc, PtrElement pEl, int firstChar, int lastChar)

#else  /* __STDC__ */
void                SelectStringWithEvent (pDoc, pEl, firstChar, lastChar)
PtrDocument         pDoc;
PtrElement          pEl;
int                 firstChar;
int                 lastChar;

#endif /* __STDC__ */

{
   NotifyElement       notifyEl;

   if (pDoc != NULL && pEl != NULL)
     {
	notifyEl.event = TteElemSelect;
	notifyEl.document = (Document) IdentDocument (pDoc);
	notifyEl.element = (Element) pEl;
	notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
	notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
	notifyEl.position = 0;
	if (!CallEventType ((NotifyEvent *) & notifyEl, TRUE))
	  {
	     SelectStringOrPosition (pDoc, pEl, firstChar, lastChar, TRUE);
	     notifyEl.event = TteElemSelect;
	     notifyEl.document = (Document) IdentDocument (pDoc);
	     notifyEl.element = (Element) pEl;
	     notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
	     notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
	     notifyEl.position = 0;
	     CallEventType ((NotifyEvent *) & notifyEl, FALSE);
	  }
     }
}


/*----------------------------------------------------------------------
   ChangeSelection		
   l'utilisateur veut faire une nouvelle selection ou une  
   extension de la selection courante (selon le parametre  
   extension).                                             
   frame: numero de la fenetre ou l'utilisateur a clique'
   pAb: pointeur sur le pave ou l'utilisateur a clique'.
   rank: rang du caractere sur lequel l'utilisateur a      
   clique', ou 0 si tout le pave a ete designe'.           
   update: le Mediateur demande qu'on lui mette a jour sa  
   selection. Si update est faux, il s'agit                
   necessairement d'une extension et le parametre extension
   est interprete comme 'debut' : s'il est Vrai il s'agit  
   d'une extension du debut, sinon d'une extension de la   
   fin de la selection.                                    
   Si doubleClick est vrai, il s'agit d'un double clic sans
   mouvement de la souris.                                 
   Si drag est vrai, il s'agit d'une extension de selection
   par dragging.                                           
  ----------------------------------------------------------------------*/


#ifdef __STDC__
void                ChangeSelection (int frame, PtrAbstractBox pAb, int rank, boolean extension, boolean update, boolean doubleClick, boolean drag)

#else  /* __STDC__ */
void                ChangeSelection (frame, pAb, rank, extension, update, doubleClick, drag)
int                 frame;
PtrAbstractBox      pAb;
int                 rank;
boolean             extension;
boolean             update;
boolean             doubleClick;
boolean             drag;

#endif /* __STDC__ */

{
   PtrDocument         pDoc;
   PtrElement          pEl, pEl1;
   PtrAttribute        pAttr;
   NotifyElement       notifyEl;
   int                 vue, numassoc;
   boolean             assoc, error, fixed, begin, stop, doubleClickRef;

   numassoc = 0;
   pEl1 = NULL;
   /* cherche le document et la vue auxquels correspond la fenetre */
   GetDocAndView (frame, &pDoc, &vue, &assoc);
   if (doubleClick)
      if (pAb != NULL)
	{
	   pEl1 = pAb->AbElement;
	   if (pEl1 != NULL)
	     {
		notifyEl.event = TteElemActivate;
		notifyEl.document = (Document) IdentDocument (pDoc);
		notifyEl.element = (Element) pEl1;
		notifyEl.elementType.ElTypeNum = pEl1->ElTypeNumber;
		notifyEl.elementType.ElSSchema = (SSchema) (pEl1->ElStructSchema);
		notifyEl.position = 0;
		if (CallEventType ((NotifyEvent *) & notifyEl, TRUE))
		   return;
		if (pEl1->ElHolophrast)
		  {
		     /* pour eviter de recalculer les menus: */
		     /* ce sera fait par SelectElement */
		     SelectedDocument = NULL;
		     /* eteint la selection precedente */
		     CancelSelection ();
		     DeHolophrast (pEl1, pDoc);
		     SelectElementWithEvent (pDoc, pEl1, TRUE, FALSE);
		     return;
		  }
	     }
	}

   error = FALSE;
   doubleClickRef = FALSE;
   /* verifie les doubles clics et extensions sur les points de */
   /* controle des polylines */
   if (pAb != NULL)
      if (pAb->AbElement->ElTerminal &&
	  pAb->AbElement->ElLeafType == LtPolyLine)
	 /* il s'agit bien d'une polyline */
	{
	   if (extension)
	      /* c'est bien une extension */
	      if (FirstSelectedElement == pAb->AbElement && rank == SelectedPointInPolyline)
		 /* c'est la meme polyline et le meme point que precedemment, */
		 /* ce n'est pas vraiment une extension */
		 extension = FALSE;
	      else
		 /* on etend jusqu'a la PolyLine entiere */
		 rank = 0;
	   if (doubleClick)
	      /* un double-clic concerne une PolyLine entiere */
	      rank = 0;
	}
   if (extension && SelectedDocument == NULL && DocSelectedAttr == NULL)
      /* on veut etendre une selection qui n'existe pas ! On fait comme si */
      /* il s'agissait d'une nouvelle selection */
      extension = FALSE;
   /* si c'est un double clic, on verife qu'il s'agit bien d'une reference */
   /* ou d'une inclusion */
   if (doubleClick)
      if (pAb != NULL)
	 if (pAb->AbElement != NULL)
	   {
	      pEl1 = pAb->AbElement;
	      if (pEl1->ElStructSchema->SsRule[pEl1->ElTypeNumber - 1].SrConstruct != CsReference)
		{
		   /* cherche si un element englobant est une inclusion */
		   pEl = pEl1;
		   while (pEl->ElParent != NULL && pEl->ElSource == NULL)
		      pEl = pEl->ElParent;
		   if (pEl->ElSource != NULL)
		      pEl1 = pEl;
		}
	      if (pEl1->ElStructSchema->SsRule[pEl1->ElTypeNumber - 1].SrConstruct ==
		  CsReference || pEl1->ElSource != NULL)
		 /* cet element est une reference ou une */
		 /* inclusion : double clic sur une reference */
		{
		   doubleClickRef = TRUE;
		   FirstSelectedElement = pEl1;
		   LastSelectedElement = pEl1;
		   SelectedPointInPolyline = 0;
		   SelectedPictureEdge = 0;
		}
	      else
		{
		   /* ce n'est ni une inclusion ni un element reference */
		   /* l'element ou l'un de ses ascendants a-t-il un */
		   /* attribut reference ayant l'exception ActiveRef ? */
		   pEl = pEl1;
		   do
		     {
			pAttr = pEl->ElFirstAttr;
			/* parcourt les attributs de l'element */
			while (pAttr != NULL && !doubleClickRef)
			   if (pAttr->AeAttrType == AtReferenceAttr &&
			       AttrHasException (ExcActiveRef, pAttr->AeAttrNum, pAttr->AeAttrSSchema))
			      /* trouve' */
			     {
				doubleClickRef = TRUE;
				FirstSelectedElement = pEl;
				LastSelectedElement = pEl;
				SelectedPointInPolyline = 0;
				SelectedPictureEdge = 0;
			     }
			   else
			      /* passe a l'attribut suivant de l'element */
			      pAttr = pAttr->AeNext;
			if (!doubleClickRef)
			   /* passe a l'element pere */
			   pEl = pEl->ElParent;
		     }
		   while (pEl != NULL && !doubleClickRef);
		}
	   }
   if (pAb != NULL)
      if (pAb->AbElement->ElTerminal)
	 if (pAb->AbElement->ElLeafType == LtPairedElem ||
	     pAb->AbElement->ElLeafType == LtReference)
	    /* c'est une feuille reference ou marque de paire */
	    if (!pAb->AbPresentationBox || !pAb->AbCanBeModified)
	       /* ce n'est pas la boite de presentation d'une valeur d'attribut */
	       /* on selectionne tout le contenu */
	       rank = 0;
   if (assoc)
      /* frame d'elements associes */
     {
	/* numero d'element associe' */
	numassoc = vue;
	vue = 1;
     }
   if (!update)
      FrameWithNoUpdate = frame;
   if (extension || !update)
      /* extension de la selection courante */
      if (DocSelectedAttr != NULL)
	 /* extension de la selection dans une boite de presentation */
	 /* affichant une valeur d'attribut */
	{
	   if (DocSelectedAttr == pDoc && AbsBoxSelectedAttr == pAb)
	      /* l'extension n'est prise en compte que si elle a lieu dans le meme */
	      /* pave de presentation */
	     {
		if (rank == 0)
		  {
		     FirstSelectedCharInAttr = 1;
		     LastSelectedCharInAttr = pAb->AbVolume;
		  }
		else if (rank <= InitSelectedCharInAttr)
		  {
		     FirstSelectedCharInAttr = rank;
		     LastSelectedCharInAttr = InitSelectedCharInAttr;
		  }
		else
		  {
		     FirstSelectedCharInAttr = InitSelectedCharInAttr;
		     LastSelectedCharInAttr = rank;
		  }
		SelectStringInAttr (pDoc, pAb, FirstSelectedCharInAttr, LastSelectedCharInAttr, TRUE);
	     }
	}
      else
	 /* la selection n'est pas dans une boite de presentation affichant */
	 /* une valeur d'attribut */
	{
	   if (pDoc != SelectedDocument || (assoc && numassoc != FirstSelectedElement->ElAssocNum))
	     {
		TtaDisplaySimpleMessage (INFO, LIB, TMSG_CHANGING_DOC_IMP);
		/* message 'Ne changez pas de document' */
		error = TRUE;
	     }
	   else
	     {
		/* meme document: mais est-ce le meme arbre ? */
		if (FirstSelectedElement != NULL)
		   /* on cherche un pave englobant qui appartienne au meme type */
		   /* d'element associe' que le premier element selectionne' */
		  {
		     stop = FALSE;
		     do
			if (pAb == NULL)
			   stop = TRUE;		/* echec */
			else if (FirstSelectedElement->ElAssocNum == pAb->AbElement->ElAssocNum)
			   stop = TRUE;		/* meme type d'element associe */
			else
			   pAb = pAb->AbEnclosing;	/* passe a l'englobant */
		     while (!stop);
		  }
		if (pAb == NULL)
		   error = TRUE;
		else
		  {
		     fixed = update;
		     begin = extension;
		     pEl = pAb->AbElement;
		     if (pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrConstruct == CsConstant)
			/* l'element a selectionner est une constante, on le */
			/* selectionne en entier */
			rank = 0;
		     /* Si l'element a selectionner est cache' ou non selectionnable, */
		     /* on change pour le premier element englobant qui ne l'est pas */
		     if (TypeHasException (ExcNoSelect, pEl->ElTypeNumber, pEl->ElStructSchema)
			 || HiddenType (pEl))
		       {
			  stop = FALSE;
			  rank = 0;	/* selectionne tout l'element */
			  while (!stop)
			     if (pEl->ElParent == NULL)
				/* c'est la racine de l'arbre, on la selectionne */
				stop = TRUE;
			     else
			       {
				  pEl = pEl->ElParent;
				  if (!TypeHasException (ExcNoSelect, pEl->ElTypeNumber,
							 pEl->ElStructSchema)
				      && !HiddenType (pEl))
				     stop = TRUE;
			       }
		       }
		     notifyEl.event = TteElemExtendSelect;
		     notifyEl.document = (Document) IdentDocument (pDoc);
		     notifyEl.element = (Element) pEl;
		     notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
		     notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
		     notifyEl.position = 0;
		     if (!CallEventType ((NotifyEvent *) & notifyEl, TRUE))
		       {
			  ExtendSelection (pEl, rank, fixed, begin, drag);
			  notifyEl.event = TteElemExtendSelect;
			  notifyEl.document = (Document) IdentDocument (pDoc);
			  notifyEl.element = (Element) pEl;
			  notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
			  notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
			  notifyEl.position = 0;
			  CallEventType ((NotifyEvent *) & notifyEl, FALSE);
		       }
		  }
	     }
	}
   else
     {
	/* nouvelle selection */
	if (pDoc != SelectedDocument)
	   /* on change de document */
	   TtaClearViewSelections ();	/* eteint la selection precedente */
	else if (!doubleClickRef)
	   /* selection dans le meme document que precedemment */
	   if (doubleClick)	/* l'utilisateur a clique' double */
	      if (SelectedView == vue)	/* dans la meme vue */
		 /* un seul element selectionne' */
		 if (FirstSelectedElement == LastSelectedElement)
		    /* c'est le meme */
		    if (pAb->AbElement == FirstSelectedElement)
		      {
			 pEl1 = FirstSelectedElement;
			 if (pEl1->ElStructSchema->SsRule[pEl1->ElTypeNumber - 1].SrConstruct ==
			     CsReference || pEl1->ElSource != NULL)
			    /* cet element est une reference ou une copie par */
			    /* inclusion : double clic sur une   reference */
			    doubleClickRef = TRUE;
		      }
	if (doubleClick)
	  {
	     /* cherche l'element reference' */
	     FindReferredEl ();
	     notifyEl.event = TteElemActivate;
	     notifyEl.document = (Document) IdentDocument (pDoc);
	     notifyEl.element = (Element) pEl1;
	     notifyEl.elementType.ElTypeNum = pEl1->ElTypeNumber;
	     notifyEl.elementType.ElSSchema = (SSchema) (pEl1->ElStructSchema);
	     notifyEl.position = 0;
	     CallEventType ((NotifyEvent *) & notifyEl, FALSE);
	  }
	else
	  {
	     SelectedView = vue;
	     /* la vue active est celle ou l'utilisateur */
	     /* a choisi le debut de la selection */
	     OldSelectedView = vue;
	     /* on essaiera de reselectionner dans cette vue */
	     OldDocSelectedView = pDoc;
	     pEl = pAb->AbElement;
	     if (pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrConstruct == CsConstant)
		/* l'element a selectionner est une constante, on le */
		/* selectionne en entier */
		rank = 0;
	     /* Si l'element a selectionner possede l'exception NoSelect ou */
	     /* est cache', on change pour le premier element englobant qui ne */
	     /* l'est pas */
	     if (TypeHasException (ExcNoSelect, pEl->ElTypeNumber,
				   pEl->ElStructSchema)
		 || HiddenType (pEl))
	       {
		  stop = FALSE;
		  rank = 0;	/* selectionne tout l'element */
		  while (!stop)
		     if (pEl->ElParent == NULL)
			/* c'est la racine de l'arbre, on la selectionne */
			stop = TRUE;
		     else
		       {
			  pEl = pEl->ElParent;
			  if (!TypeHasException (ExcNoSelect, pEl->ElTypeNumber,
						 pEl->ElStructSchema)
			      && !HiddenType (pEl))
			     stop = TRUE;
		       }
	       }
	     if (rank > 0 && pAb->AbPresentationBox && pAb->AbCanBeModified)
		/* l'utilisateur a clique' dans une boite de presentation d'une
		   valeur d'attribut */
	       {
		  CancelSelection ();
		  SelectStringInAttr (pDoc, pAb, rank, rank, FALSE);
		  InitSelectedCharInAttr = rank;
	       }
	     else if (rank > 0 && pEl->ElTerminal &&
		  (pEl->ElLeafType == LtText || pEl->ElLeafType == LtPolyLine
		   || pEl->ElLeafType == LtPicture))
		SelectPositionWithEvent (pDoc, pEl, rank);
	     else
		SelectElementWithEvent (pDoc, pEl, TRUE, FALSE);
	  }
     }
   if (!doubleClick)
     {
	FrameWithNoUpdate = 0;
	if (!error)
	   /* Si tout le contenu d'une feuille de texte est selectionne', */
	   /* c'est la feuille elle-meme qui est selectionnee */
	   if (LastSelectedElement != NULL)
	      if (LastSelectedElement == FirstSelectedElement)
		{
		   pEl1 = FirstSelectedElement;
		   if (pEl1->ElTerminal && pEl1->ElLeafType == LtText)
		      if (pEl1->ElTextLength < LastSelectedChar &&
			  FirstSelectedChar <= 1)
			{
			   LastSelectedChar = 0;
			   FirstSelectedChar = 0;
			}
		}
	if (FirstSelectedElement != NULL)
	   if (FirstSelectedElement->ElAssocNum != 0)
	      /* retablit le pave englobant des elements associes */
	      EnclosingAssocAbsBox (FirstSelectedElement);
     }
}

#ifdef __COLPAGE__
/*----------------------------------------------------------------------
   ColPageType rend un pointeur sur la chaine donnant le type de
   la page pPage (utilise pour la construction de la
   ligne de selection et le menu selection.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
char               *TypePageCol (PtrElement pPage)

#else  /* __STDC__ */
char               *TypePageCol (pPage)
PtrElement          pPage;

#endif /* __STDC__ */

{

   char               *typepage;

   if (pPage->ElTypeNumber == PageBreak + 1)
      switch (pPage->ElPageType)
	    {
	       case PgBegin:
		  typepage = "PgBegin";
		  break;
	       case PgComputed:
		  typepage = "PgComputed";
		  break;
	       case PgUser:
		  typepage = "PgUser";
		  break;
	       case ColBegin:
		  typepage = "ColonneDebut";
		  break;
	       case ColComputed:
		  typepage = "ColonneCalculee";
		  break;
	       case ColUser:
		  typepage = "ColonneUtilisateur";
		  break;
	       case ColGroup:
		  typepage = "ColonnesGroupees";
		  break;
	       default:
		  typepage = "PageBreak";
		  break;
	    }
   else
      typepage = "";
   return typepage;
}
#endif /* __COLPAGE__ */

/*----------------------------------------------------------------------
   PrepareSelectionMenu recherche les elements qui vont composer     
   le menu selection.                                            
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                PrepareSelectionMenu ()

#else  /* __STDC__ */
void                PrepareSelectionMenu ()
#endif				/* __STDC__ */

{
   PtrElement          pEl1;
   PtrElement          pEl2;
   boolean             stop;
   boolean             stop1;

   /* on ignore l'exception NoSelect */
   /* cherche le premier element englobant non cache' */
   SelMenuParentEl = NULL;
   if (FirstSelectedElement != NULL)
      if (FirstSelectedElement->ElParent != NULL)
	{
	   pEl1 = FirstSelectedElement->ElParent;
	   stop = FALSE;
	   while (!stop)
	      if (pEl1 == NULL)
		 stop = TRUE;
	      else if (!ElementIsHidden (pEl1) && !HiddenType (pEl1) && ElemIsAnAncestor (pEl1, LastSelectedElement))
		 stop = TRUE;
	      else
		 pEl1 = pEl1->ElParent;
	   if (pEl1 != NULL)
	      SelMenuParentEl = pEl1;
	}

   /* cherche le type de l'element selectionable precedent */
   SelMenuPreviousEl = NULL;
   stop = FALSE;
   pEl1 = FirstSelectedElement;
   while (!stop && pEl1 != NULL)
      if (pEl1->ElPrevious != NULL)
	{
	   pEl1 = pEl1->ElPrevious;
	   if (!ElementIsHidden (pEl1))
	      if (!HiddenType (pEl1))
		 stop = TRUE;
	      else
		{
		   pEl2 = pEl1;
		   stop1 = FALSE;
		   while (!stop1)
		      if (pEl2->ElTerminal)
			 stop1 = TRUE;
		      else if (pEl2->ElFirstChild == NULL)
			 stop1 = TRUE;
		      else
			{
			   pEl2 = pEl2->ElFirstChild;
			   while (pEl2->ElNext != NULL)
			      pEl2 = pEl2->ElNext;
			   while (ElementIsHidden (pEl2) && pEl2->ElPrevious != NULL)
			      pEl2 = pEl2->ElPrevious;
			   if (!ElementIsHidden (pEl2))
			     {
				pEl1 = pEl2;
				if (!HiddenType (pEl2))
				  {
				     stop1 = TRUE;
				     stop = TRUE;
				  }
			     }
			}
		}
	}
      else
	 pEl1 = pEl1->ElParent;
   if (pEl1 != NULL)
      SelMenuPreviousEl = pEl1;

   /* cherche le type de l'element selectionable suivant */
   SelMenuNextEl = NULL;
   stop = FALSE;
   pEl1 = LastSelectedElement;
   while (!stop && pEl1 != NULL)
      if (pEl1->ElNext != NULL)
	{
	   pEl1 = pEl1->ElNext;
	   if (!ElementIsHidden (pEl1))
	      if (!HiddenType (pEl1))
		 stop = TRUE;
	      else
		{
		   pEl2 = pEl1;
		   stop1 = FALSE;
		   while (!stop1)
		      if (pEl2->ElTerminal)
			 stop1 = TRUE;
		      else if (pEl2->ElFirstChild == NULL)
			 stop1 = TRUE;
		      else
			{
			   pEl2 = pEl2->ElFirstChild;
			   while (ElementIsHidden (pEl2) && pEl2->ElNext != NULL)
			      pEl2 = pEl2->ElNext;
			   if (!ElementIsHidden (pEl2))
			     {
				pEl1 = pEl2;
				if (!HiddenType (pEl2))
				  {
				     stop1 = TRUE;
				     stop = TRUE;
				  }
			     }
			}
		}
	}
      else
	 pEl1 = pEl1->ElParent;
   if (pEl1 != NULL)
      SelMenuNextEl = pEl1;

   /* cherche le type du 1er element englobe' selectionable */
   SelMenuChildEl = NULL;
   stop = FALSE;
   pEl1 = FirstSelectedElement;
   while (!stop && pEl1 != NULL)
      /* interdit la selection du contenu d'une copie ou d'un */
      /* element protege' */
      if (pEl1->ElIsCopy || ElementIsHidden (pEl1))
	 pEl1 = NULL;
      else if (pEl1->ElTerminal)
	 /* pas de descendant */
	 pEl1 = NULL;
      else
	{
	   /* on passe au descendant */
	   pEl1 = pEl1->ElFirstChild;
	   stop1 = FALSE;
	   /* on cherche le premier frere non protege' */
	   while (!stop1 && pEl1 != NULL)
	      if (!ElementIsHidden (pEl1))
		 stop1 = TRUE;
	      else
		 pEl1 = pEl1->ElNext;
	   if (pEl1 != NULL)
	      if (!HiddenType (pEl1))
		 /* cet element est bien presentable a l'utilisateur */
		 stop = TRUE;
	}
   if (pEl1 != NULL)
      SelMenuChildEl = pEl1;
}

static char         OldMsgSelect[MAX_TXT_LEN];
static PtrDocument  OldDocMsgSelect = NULL;

/*----------------------------------------------------------------------
   BuildSelectionMessage construit et affiche le texte du          
   message de selection correspondant a la selection courante.     
  ----------------------------------------------------------------------*/
void                BuildSelectionMessage ()
{
   PtrElement          pEl;
   PtrDocument         pDoc;
   char                msgBuf[MAX_TXT_LEN];
   int                 nbasc;

   if (DocSelectedAttr != NULL && AbsBoxSelectedAttr != NULL)
      /* il y a une selection dans une valeur d'attribut */
      /* on selectionne l'element sur lequel porte l'attribut */
     {
	pDoc = DocSelectedAttr;
	pEl = AbsBoxSelectedAttr->AbElement;
     }
   else
     {
	pDoc = SelectedDocument;
	pEl = FirstSelectedElement;
     }
   /* on met d'abord le nom du type du premier element selectionne' */
#ifdef __COLPAGE__
   /* dans le cas d'un element de type SAUT PAGE, on affiche */
   /* le type de saut de page (PgBegin, PgComputed, */
   /* PgUser, ColBegin, ColComputed, ColUser, */
   /* ou ColGroup */
   if (pEl->ElTypeNumber == PageBreak + 1)
      strncpy (msgBuf, TypePageCol (pEl), MAX_NAME_LENGTH);
   else
#endif /* __COLPAGE__ */
      strncpy (msgBuf, pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrName,
	       MAX_NAME_LENGTH);
   /* ajoute les types des elements englobants */
   pEl = pEl->ElParent;
   nbasc = 1;
   while (pEl != NULL)
     {
	/* on saute l'element s'il est cache' */
	if (!HiddenType (pEl))
	  {
	     /* un caractere pour separer les niveaux */
	     strcat (&msgBuf[strlen (msgBuf)], " \\ ");
	     /* ajoute le type de l'element */
	     strcat (&msgBuf[strlen (msgBuf)], pEl->ElStructSchema->
		     SsRule[pEl->ElTypeNumber - 1].SrName);
	     nbasc++;		/* nombre d'elements traites */
	  }
	if (nbasc >= MAX_ITEM_MSG_SEL)
	   pEl = NULL;		/* on arrete */
	else
	   pEl = pEl->ElParent;	/* passe au pere */
     }
   /* si le message Selection ou le document selectionne' ont change'
      depuis la derniere fois, on affiche le message de Selection */
   if (pDoc != OldDocMsgSelect || strcmp (OldMsgSelect, msgBuf) != 0)
     {
	/* affiche le message Selection en haut de la fenetre */
	DisplaySelMessage (msgBuf);
	OldDocMsgSelect = pDoc;
	strncpy (OldMsgSelect, msgBuf, MAX_TXT_LEN);
     }
}


/*----------------------------------------------------------------------
   SelectPairInterval                               
   Si la selection courante est une paire de marques, on           
   selectionne tous les elements compris entre les deux marques,   
   les marques comprises, et on retourne Vrai.                     
   Sinon, on ne fait rien et on retourne Faux.                     
  ----------------------------------------------------------------------*/

boolean             SelectPairInterval ()
{
   boolean             ret;

   ret = FALSE;
   if (!SelContinue)
      /* la selection courante est une selection discrete */
      if (NSelectedElements == 2)
	 /* seulement deux elements sont selectionne's */
	 if (SelectedElement[0] == GetOtherPairedElement (SelectedElement[1]))
	    /* les 2 elements sont des elements de paire et ils */
	    /* appartiennent a` la meme paire */
	   {
	      SelContinue = TRUE;
	      LastSelectedElement = FirstSelectedElement;
	      ExtendSelection (SelectedElement[1], 0, FALSE, TRUE, FALSE);
	      ret = TRUE;
	   }
   return ret;
}


/*----------------------------------------------------------------------
   SelectAround se'lectionne autour de l'e'le'ment courant.
   - le pe`re si val=1                                             
   - le pre'ce'dent si val=2                                       
   - le suivant si val=3                                           
   - le premier fils si val=4                                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SelectAround (int val)
#else  /* __STDC__ */
void                SelectAround (val)
int                 val;

#endif /* __STDC__ */
{
   PtrElement          pEl, pParent, pFirst, pLast;

   /*boolean     stop; */

   pEl = NULL;
   /*stop = FALSE; */
   switch (val)
	 {
	    case 1:
	       /* Enclosing */
	       if (SelectPairInterval ())
		  pEl = NULL;
	       else
		 {
		    if (DocSelectedAttr != NULL && AbsBoxSelectedAttr != NULL)
		       /* il y a une selection dans une valeur d'attribut */
		       /* on selectionne l'element sur lequel porte l'attribut */
		       SelectElementWithEvent (DocSelectedAttr,
				AbsBoxSelectedAttr->AbElement, TRUE, FALSE);
		    else
		       /* si le premier et le dernier selectionne's ne sont 
		          pas freres on normalise la selection: le premier et
		          le dernier element selectionne's doivent etre des
		          freres dans l'arbre abstrait */
		    if (!StructSelectionMode)
		      {
			 if (FirstSelectedElement != NULL &&
			     LastSelectedElement != NULL)
			    if (FirstSelectedElement->ElParent !=
				LastSelectedElement->ElParent)
			      {
				 pFirst = FirstSelectedElement;
				 pLast = LastSelectedElement;
				 /* remonte les ascendants du premier element
				    de la selection */
				 pParent = pFirst->ElParent;
				 while (pParent != NULL)
				    if (ElemIsAnAncestor (pParent, pLast))
				       /* cet ascendant (pParent) du premier
				          selectionne' est aussi un ascendant
				          du dernier selectionne' */
				      {
					 /* on retient comme dernier
					    l'ascendant du dernier qui a
					    pParent pour pere */
					 while (pLast->ElParent != pParent)
					    pLast = pLast->ElParent;
					 /* on a fini */
					 pParent = NULL;
				      }
				    else
				       /* cet ascendant (pParent) du premier
				          selectionne' n'est pas un ascendant
				          du dernier selectionne' */
				      {
					 /* on retient pour l'instant pParent
					    et on va regarder si son pere est
					    un ascendant du dernier selectionne' */
					 pFirst = pParent;
					 pParent = pParent->ElParent;
				      }
				 SelectElementWithEvent (SelectedDocument,
						       pFirst, TRUE, FALSE);
				 if (pFirst != pLast)
				    ExtendSelection (pLast, 0, FALSE, TRUE, FALSE);
			      }
			    else
			       pEl = SelMenuParentEl;
		      }
		    else
		       pEl = SelMenuParentEl;
		 }
	       break;
	    case 2:
	       /* RlPrevious */
	       pEl = SelMenuPreviousEl;
	       break;
	    case 3:
	       /* RlNext */
	       pEl = SelMenuNextEl;
	       break;
	    case 4 /* ElemIsAnAncestor */ :
	       pEl = SelMenuChildEl;
	       break;
#ifdef __COLPAGE__
	    case 6:		/* structure physique englobante */
	       pEl = SelMenuPageColParent;
	       break;
	    case 7:		/* structure physique precedente */
	       pEl = SelMenuPageColPrev;
	       break;
	    case 8:		/* structure physique suivante */
	       pEl = SelMenuPageColNext;
	       break;
	    case 9:		/* structure physique englobee */
	       pEl = SelMenuPageColChild;
	       break;
#endif /* __COLPAGE__ */
	    default:
	       break;
	 }
   if (pEl != NULL)
      if (!ElementIsHidden (pEl))
	 SelectElementWithEvent (SelectedDocument, pEl, TRUE, FALSE);
}


/*----------------------------------------------------------------------
   TtcParentElement                                                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcParentElement (Document document, View view)
#else  /* __STDC__ */
void                TtcParentElement (document, view)
Document            document;
View                view;
char                c;

#endif /* __STDC__ */
{
   SelectAround (1);
}


/*----------------------------------------------------------------------
   TtcPreviousElement                                              
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcPreviousElement (Document document, View view)
#else  /* __STDC__ */
void                TtcPreviousElement (document, view)
Document            document;
View                view;
char                c;

#endif /* __STDC__ */
{
   SelectAround (2);
}


/*----------------------------------------------------------------------
   TtcNextElement                                                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcNextElement (Document document, View view)
#else  /* __STDC__ */
void                TtcNextElement (document, view)
Document            document;
View                view;
char                c;

#endif /* __STDC__ */
{
   SelectAround (3);
}

/*----------------------------------------------------------------------
   TtcChildElement                                                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcChildElement (Document document, View view)
#else  /* __STDC__ */
void                TtcChildElement (document, view)
Document            document;
View                view;
char                c;

#endif /* __STDC__ */
{
   SelectAround (4);
}
