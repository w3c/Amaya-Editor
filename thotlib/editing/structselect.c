/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * This module handles selection in abstract trees
 *
 * Authors: V. Quint, I. Vatton (INRIA)
 *
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "appaction.h"
#include "appstruct.h"
#include "libmsg.h"
#include "message.h"
#include "appdialogue.h"

#define THOT_EXPORT extern
#include "edit_tv.h"
#include "frame_tv.h"
#include "appdialogue_tv.h"
#undef THOT_EXPORT
#define THOT_EXPORT
#include "select_tv.h"

#include "absboxes_f.h"
#include "appli_f.h"
#include "applicationapi_f.h"
#include "boxselection_f.h"
#include "callback_f.h"
#include "config_f.h"
#include "createabsbox_f.h"
#include "displayselect_f.h"
#include "displayview_f.h"
#include "draw_f.h"
#include "exceptions_f.h"
#include "keyboards_f.h"
#include "scroll_f.h"
#include "search_f.h"
#include "searchref_f.h"
#include "selectionapi_f.h"
#include "selectmenu_f.h"
#include "structmodif_f.h"
#include "structselect_f.h"
#include "tree_f.h"
#include "views_f.h"
#include "windowdisplay_f.h"

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
					   vertex, or 0 if the whole PolyLine
					   is selected */
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
   CheckSelectedElement verifie si l'element pEl constitue    
   l'une des extremite's de la selection courante dans le document 
   "document" et si oui definit une nouvelle selection, sans cet   
   element.                                                        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void         CheckSelectedElement (PtrElement pEl, Document document)
#else  /* __STDC__ */
void         CheckSelectedElement (pEl, document)
PtrElement          pEl;
Document            document;
#endif /* __STDC__ */
{
   PtrDocument         pDoc;
   PtrDocument         selDoc;
   PtrElement          firstSelection, lastSelection, selEl, previousSelection;
   int                 firstChar, lastChar;
   ThotBool            ok, changeSelection;

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
   ThotBool            stop;

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
ThotBool            GetCurrentSelection (PtrDocument * pDoc, PtrElement * firstEl, PtrElement * lastEl, int *firstChar, int *lastChar)
#else  /* __STDC__ */
ThotBool            GetCurrentSelection (pDoc, firstEl, lastEl, firstChar, lastChar)
PtrDocument        *pDoc;
PtrElement         *firstEl;
PtrElement         *lastEl;
int                *firstChar;
int                *lastChar;

#endif /* __STDC__ */

{
   ThotBool            ret;
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
	else if (pEl->ElTerminal && pEl->ElLeafType == LtPicture)
	  {
	     *firstChar = SelectedPictureEdge;
	     *lastChar = 0;
	  }
	else
	  {
	     *firstChar = FirstSelectedChar;
	     if (LastSelectedChar == 0)
		*lastChar = 0;
	     else if (SelPosition) 
		*lastChar = FirstSelectedChar;
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
void                GetActiveView (PtrDocument * pDoc, int *view, ThotBool * assoc)

#else  /* __STDC__ */
void                GetActiveView (pDoc, view, assoc)
PtrDocument        *pDoc;
int                *view;
ThotBool           *assoc;

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
   if (SelectionUpdatesMenus && pDoc != NULL)
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
   ThotBool            found;

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
	      if (ThotLocalActions[T_switchsel])
		(*ThotLocalActions[T_switchsel]) (SelectedDocument->DocViewFrame[view], FALSE);
	}
}


/*----------------------------------------------------------------------
   HiddenType

   Returns TRUE if, according to its type, element pEl must be hidden to
   the user.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            HiddenType (PtrElement pEl)
#else  /* __STDC__ */
ThotBool            HiddenType (pEl)
PtrElement          pEl;
#endif /* __STDC__ */

{
   SRule              *pSRule;
   ThotBool            ret;

   ret = FALSE;
   if (TypeHasException (ExcHidden, pEl->ElTypeNumber, pEl->ElStructSchema))
      /* the element type has exception Hidden */
     {
	/* if element is not empty or is a leaf, it is hidden
	   otherwise, exception Hidden is ignored: the user could not
	   select that element otherwise */
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
   the first selected element. If 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                HighlightSelection (ThotBool showBegin, ThotBool clearOldSel)
#else  /* __STDC__ */
void                HighlightSelection (showBegin, clearOldSel)
ThotBool            showBegin;
ThotBool            clearOldSel;
#endif /* __STDC__ */
{
   PtrAbstractBox      pAb, pNextAb;
   PtrElement          pEl, pNextEl;
   int                 view, lastView, frame, firstChar, lastChar;
   ThotBool            first, last, partialSel, active, unique, stop, assoc;

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
		  if (clearOldSel)
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
		       pNextAb = pAb->AbNext;
		       if (pNextAb != NULL)
			  if (pNextAb->AbElement != pEl)
			     /* next abstract box does not belong to the
				element */
			     pNextAb = NULL;
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
			    if (last && ThotLocalActions[T_switchsel])
			      /* highlight selection */
			      (*ThotLocalActions[T_switchsel]) (frame, TRUE);
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

   Search for the selected document a view where the beginning of
   current selection os visible.
   If exceptView is not null, avoid to choose that view.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         SetActiveView (int exceptView)
#else  /* __STDC__ */
static void         SetActiveView (exceptView)
int                 exceptView;

#endif /* __STDC__ */
{
   int                 view;
   ThotBool            stop;

   /* there is no longer any active view */
   SelectedView = 0;
   /* is the former active view OK? */
   if (OldDocSelectedView == SelectedDocument)
      /* the former active view displays the selected document */
      if (OldSelectedView != 0 && OldSelectedView != exceptView)
	 if (FirstSelectedElement->ElAbstractBox[OldSelectedView - 1] != NULL)
	   /* the first selected element has an abstract box in this view */
	   /* keep this view */
	    SelectedView = OldSelectedView;
   if (SelectedView == 0)
      /* the former active view is not OK. Search another one */
     {
	stop = FALSE;
	view = 0;
	do
	  {
	     view++;
	     if (view != exceptView)
		if (FirstSelectedElement->ElAbstractBox[view - 1] != NULL)
		  {
		     /* the first selected element has an abstract box in */
		     /* view. Take it as the active view */
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
   DeactivateView

   A view has been closed for document pDoc. Change active view if the
   closed view was the active one.
   view is the number of the view, or, if assoc is TRUE, the number of the
   associated tree whose view has been closed.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DeactivateView (PtrDocument pDoc, int view, ThotBool assoc)
#else  /* __STDC__ */
void                DeactivateView (pDoc, view, assoc)
PtrDocument         pDoc;
int                 view;
ThotBool            assoc;

#endif /* __STDC__ */
{

   if (pDoc == SelectedDocument)
      /* the current selection is in that document */
      if (assoc)
	 /* it's a view for an associated tree */
	{
	   if (FirstSelectedElement->ElAssocNum == view)
	      /* the current selection is in that associated tree */
	     {
		/* there is no other view to show the current selection, as */
	        /* each associated tree has only one view */
		SelectedDocument = NULL;
		CancelSelection ();
		SelectedView = 0;
		OldSelectedView = 0;
		OldDocSelectedView = pDoc;
	     }
	}
      else
	 /* it's a view of the main tree */
         if (view == SelectedView)
	    /* it's the active view */
	   {
	     /* search another active view */
	     SetActiveView (view);
	     if (SelectedView != 0)
	       /* highlight the current selection in the new active view */
	       HighlightSelection (TRUE, TRUE);
	   }
}


/*----------------------------------------------------------------------
   WithinAbsBox

   return TRUE if abstract box pAB is in the subtree of abstract box pRootAb.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static ThotBool     WithinAbsBox (PtrAbstractBox pAb, PtrAbstractBox pRootAb)

#else  /* __STDC__ */
static ThotBool     WithinAbsBox (pAb, pRootAb)
PtrAbstractBox      pAb;
PtrAbstractBox      pRootAb;

#endif /* __STDC__ */

{
   ThotBool            ret;

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
   GetAbsBoxSelectedAttr

   returns the abstract box that displays the same attribute as
   AbsBoxSelectedAttr, but in view view.
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
	/* search the corresponding abstract box in the view: pAbView */
	pAbView = NULL;
	if (AbsBoxSelectedAttr->AbElement == NULL)
	   pAb = NULL;
	else
	   pAb = AbsBoxSelectedAttr->AbElement->ElAbstractBox[view - 1];
	while (pAbView == NULL && pAb != NULL &&
	       pAb->AbElement == AbsBoxSelectedAttr->AbElement)
	  {
	     if (pAb->AbPresentationBox)
		/* pAb is a presentation abstract box for the element */
	        /* to which the attibute is attached */
	       {
		  if (pAb->AbCanBeModified &&
		    pAb->AbCreatorAttr == AbsBoxSelectedAttr->AbCreatorAttr)
		     pAbView = pAb;
	       }
	     else
		/* pAb is the main abstract box for the element to which */
	        /* the attribute is attached */
	       {
		  pAb = pAb->AbFirstEnclosed;
		  while (pAbView == NULL && pAb != NULL &&
			 pAb->AbElement == AbsBoxSelectedAttr->AbElement)
		    {
		       if (pAb->AbPresentationBox && pAb->AbCanBeModified)
			  if (pAb->AbCreatorAttr ==
			      		AbsBoxSelectedAttr->AbCreatorAttr)
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
   ShowSelection

   Highlight all character strings and boxes that are part of the
   current selection and belong to the subtree of pRootAb.
   If visible is TRUE, the beginning of the selection should be made
   visible to the user.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                ShowSelection (PtrAbstractBox pRootAb, ThotBool visible)

#else  /* __STDC__ */
void                ShowSelection (pRootAb, visible)
PtrAbstractBox      pRootAb;
ThotBool            visible;

#endif /* __STDC__ */

{
   PtrElement          pEl, pNextEl;
   PtrAbstractBox      pAb, pNextAb;
   int                 view, frame, firstChar, lastChar;
   ThotBool            selBegin, selEnd, active, unique, assoc, stop;

   view = pRootAb->AbDocView;
   if (AssocView (pRootAb->AbElement))
      /* it's an associated tree that is displayed in its own view */
      assoc = TRUE;
   else
      assoc = FALSE;
   if (SelectedDocument != NULL && FirstSelectedElement != NULL &&
       pRootAb != NULL)
     {
	/* there is a current selection */
	if (assoc)
	   frame = SelectedDocument->DocAssocFrame[pRootAb->AbElement->ElAssocNum - 1];
	else
	   frame = SelectedDocument->DocViewFrame[view - 1];
	selBegin = TRUE;
	selEnd = FALSE;
	unique = FirstSelectedElement == LastSelectedElement;
	/* search the first abstract box of the current selection that is */
	/* visible in the view and that belongs to the subtree */
	pEl = FirstSelectedElement;
	if (WithinAbsBox (pEl->ElAbstractBox[view - 1], pRootAb))
	   /* first abstract box of this element in the view */
	   pAb = pEl->ElAbstractBox[view - 1];
	else
	   pAb = NULL;
	pNextEl = pEl;
	while (pAb == NULL && pNextEl != NULL)
	  {
	     /* get the next element in the current selection */
	     stop = FALSE;
	     do
	       {
		  pNextEl = NextInSelection (pNextEl, LastSelectedElement);
		  if (pNextEl == NULL)
		     stop = TRUE;
		  else if (pNextEl->ElAbstractBox[view - 1] != NULL)
		     /* this element has an abstract box in the view */
		     stop = TRUE;
	       }
	     while (!stop);
	     if (pNextEl != NULL)
		if (WithinAbsBox (pNextEl->ElAbstractBox[view - 1], pRootAb))
		   /* the abstract box is part of the subtree */
		  {
		     /* first element to be processed */
		     pEl = pNextEl;
		     /* first abstract box to be processed */
		     pAb = pNextEl->ElAbstractBox[view - 1];
		  }
	  }
	if (pAb != NULL)
	   /* switch current selection off in the view */
	   ClearViewSelection (frame);
	/* process all abstract boxes of all elements belonging to the */
	/* current selection, starting with the first visible in the view */
	while (pAb != NULL)
	  {
	     if (pEl == FirstSelectedElement)
		/* first element in current selection */
		firstChar = FirstSelectedChar;
	     else
		firstChar = 0;
	     if (pEl == LastSelectedElement)
		/* last element in current selection */
		lastChar = LastSelectedChar;
	     else
		lastChar = 0;
	     /* is that the last visible abstract box of the selection? */
	     pNextAb = pAb->AbNext;
	     if (pNextAb != NULL)
		if (pNextAb->AbElement != pEl)
	           /* the next abstract box does not belong to the element */
		   pNextAb = NULL;
	     if (pNextAb == NULL)
		/* search the next element in the selection having an */
	        /* abstract box in the subtree */
	       {
		  pNextEl = pEl;
		  stop = FALSE;
		  do
		    {
		       pNextEl = NextInSelection (pNextEl,LastSelectedElement);
		       if (pNextEl == NULL)
			  stop = TRUE;
		       else if (pNextEl->ElAbstractBox[view - 1] != NULL)
			  stop = TRUE;
		    }
		  while (!stop);
		  if (pNextEl != NULL)
		     /* there is a next element in the selection having an */
		     /* abstract box in the view */
		     if (WithinAbsBox (pNextEl->ElAbstractBox[view - 1], pRootAb))
			/* this abstract box is in the subtree */
		        /* It's the next to be processed */
		       {
			  pNextAb = pNextEl->ElAbstractBox[view - 1];
			  pEl = pNextEl;
		       }
	       }
	     selEnd = pNextAb == NULL;
	     /* indicate that selected element to the display module */
	     if (selBegin || selEnd)
	       {
		  InsertViewSelMarks (frame, pAb, firstChar, lastChar,
				      selBegin, selEnd, unique);
		  /* active view */
		  active = view == SelectedView;
		  if (visible)
		     if (assoc || SelectedDocument->DocView[view - 1].DvSync || active)
			ShowSelectedBox (frame, active);

		  selBegin = FALSE;
		  visible = FALSE;
		  if (selEnd && ThotLocalActions[T_switchsel])
		    /* highlight selection */
		    (*ThotLocalActions[T_switchsel]) (frame, TRUE);
	       }
	     pAb->AbSelected = TRUE;
	     /* next abstract box to be highlighted */
	     pAb = pNextAb;
	  }
     }
   else if (DocSelectedAttr != NULL && AbsBoxSelectedAttr != NULL)
      /* the current selection is within an attribute value */
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
	     if (ThotLocalActions[T_switchsel])
	       (*ThotLocalActions[T_switchsel]) (frame, TRUE);
	  }
     }
}

/*----------------------------------------------------------------------
   DisplaySel

   Highlight the selected element pEl in view view.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         DisplaySel (PtrElement pEl, int view, int frame, ThotBool assoc, ThotBool * abExist)

#else  /* __STDC__ */
static void         DisplaySel (pEl, view, frame, assoc, abExist)
PtrElement          pEl;
int                 view;
int                 frame;
ThotBool            assoc;
ThotBool           *abExist;
#endif /* __STDC__ */
{
   PtrAbstractBox      pAb, pNextAb;
   int                 firstChar, lastChar;
   ThotBool            first, last, partialSel, unique, active, show;

   if (TtaGetDisplayMode (FrameTable[frame].FrDoc) != DisplayImmediately)
     show = FALSE;
   else
     show = TRUE;
   first = TRUE;
   pAb = pEl->ElAbstractBox[view - 1];
   /* first abstract box of elemenebt in the view */
   if (pAb != NULL)
     {
	partialSel = FALSE;
	if (pEl == FirstSelectedElement)
	   /* it's the firqt elemenebt in the current selection */
	   if (pEl->ElTerminal)
	      if (pEl->ElLeafType == LtText)
		 /* that first element is a text leaf */
		{
		   if (FirstSelectedChar > 1 && pEl->ElTextLength > 0)
		      /* the text leaf is partly selected */
		      partialSel = TRUE;
		}
	      else if (pEl->ElLeafType == LtPolyLine)
		 if (SelectedPointInPolyline > 0)
		    partialSel = TRUE;
	if (partialSel)
	   /* skip presentation abtract boxes created before the main box */
	  {
	     while (pAb->AbPresentationBox && pAb->AbNext != NULL)
		pAb = pAb->AbNext;
	     if (pAb != NULL)
		if (pAb->AbElement != pEl)
		   pAb = NULL;
	  }
	/* the element has at least one abstract box in the view */
	*abExist = pAb != NULL;
     }
   /* handles all abstract box of the element in the view */
   while (pAb != NULL)
     {
	if (!pAb->AbSelected)
	   if (!pAb->AbPresentationBox && pEl->ElAssocNum != 0)
	      /* it's an assiciated element. Reset its enclosing abst. boxes */
	      EnclosingAssocAbsBox (pEl);
	/* search the next selected element */
	partialSel = FALSE;
	if (pEl == LastSelectedElement)
	   /* that's the last element in the current selection */
	   if (pEl->ElTerminal)
	      if (pEl->ElLeafType == LtText)
		 /* that's a text leaf */
		{
		   if (LastSelectedChar < pEl->ElTextLength
		       && pEl->ElTextLength > 0 && LastSelectedChar > 0)
		      /* that text leaf is partly selected */
		      partialSel = TRUE;
		}
	      else if (pEl->ElLeafType == LtPolyLine)
		 if (SelectedPointInPolyline > 0)
		    partialSel = TRUE;
	if (partialSel && !pAb->AbPresentationBox)
	   pNextAb = NULL;
	else
	   pNextAb = pAb->AbNext;
	if (pNextAb != NULL)
	   if (pNextAb->AbElement != pEl)
	      /* the next abstract box does not belong to the element */
	      pNextAb = NULL;
	last = pNextAb == NULL;
	/* indicate that this abstract box is selected to the display module */
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

	     if (show)
	       {
		 /* should this abstract box be made visible to the user? (scroll) */
		 active = view == SelectedView;
		 if (first)
		   if (assoc || active || SelectedDocument->DocView[view - 1].DvSync)
		     ShowSelectedBox (frame, active);
		 first = FALSE;
		 if (last && ThotLocalActions[T_switchsel])
		   /* highlight selection */
		   (*ThotLocalActions[T_switchsel]) (frame, TRUE);
	       }
	  }
	/* this abstract box is selected */
	pAb->AbSelected = TRUE;
	/* next abstract box of the element */
	pAb = pNextAb;
     }
}


/*----------------------------------------------------------------------
   SelectAbsBoxes

   Select in all views all abstract boxes of element pEl.
   If no abstract exists currently for this element in any view, try
   to to open a view where this element would have an abstract box, but
   only if createView is TRUE.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     SelectAbsBoxes (PtrElement pEl, ThotBool createView)
#else  /* __STDC__ */
static ThotBool     SelectAbsBoxes (pEl, createView)
PtrElement          pEl;
ThotBool            createView;
#endif /* __STDC__ */

{
  PtrElement          pRoot;
  NotifyDialog        notifyDoc;
  AvailableView       viewTable;
  DocViewNumber       docView, freeView;
  Document            doc;
  int                 X, Y, width, height, view, lastView, frame, run;
  int                 nViews, i, createdView;
  ThotBool            abExist, done, assoc, deleteView;

  /* there is not any abstract box yet */
  abExist = FALSE;
  if (pEl != NULL && pEl->ElStructSchema != NULL)
    {
      if (AssocView (pEl))
	/* associated elements have only one view, view 1 */
	{
	  assoc = TRUE;
	  lastView = 1;
	}
      else
	/* for other elements, all views are considered */
	{
	  assoc = FALSE;
	  lastView = MAX_VIEW_DOC;
	}
      /* views are scanned twice. In the first run, existing abstract */
      /* boxes are selected. In the second run, new abstract boxes are */
      /* created if necessary */
      doc = IdentDocument (SelectedDocument);
      for (run = 1; run <= 2; run++)
	for (view = 0; view < lastView; view++)
	  {
	    /* frame: window where the selection will be shown */
	    if (assoc)
	      /* view for associated elements */
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
		if (run == 2)
		  /* second run. Create missing abstract boxes */
		  if (pEl->ElAbstractBox[view] != NULL)
		    {
		      done = TRUE;
		      /* the element has at least one abstract box */
		      abExist = TRUE;
		    }
		  else
		    /* create the abstract boxes for view view if */
		    /* this view is synchronized */
		    if (assoc || SelectedDocument->DocView[view].DvSync)
		      /* if pEl is a page break, don't call CheckAbsBox*/
		      /* if this break is not for the right view */
		      if (pEl->ElTypeNumber != PageBreak + 1 ||
			  pEl->ElViewPSchema == SelectedDocument->DocView[view].DvPSchemaView)
			{
			  CheckAbsBox (pEl, view + 1, SelectedDocument, FALSE, TRUE);
			  if (SelectedView == 0 && pEl->ElAbstractBox[view] != NULL)
			    SetActiveView (0);
			}
		if (!done)
		  DisplaySel (pEl, view + 1, frame, assoc, &abExist);
	      }
	  }

      if (!abExist)
	/* there is no existing abstract box for this element */
	if (createView)
	  /* try to create a view where this element has an abstract box */
	  if (assoc)
	    /* the element is displayed in an view for associated
	       elements */
	    {
	      /* does this view for associated element already exist? */
	      if (SelectedDocument->DocAssocFrame[pEl->ElAssocNum - 1] == 0)
		/* The view does not exist. try to create it */
		{
		  /* first, search the root of the associated tree */
		  pRoot = pEl;
		  while (pRoot->ElParent != NULL)
		    pRoot = pRoot->ElParent;
		  /* search in the config file the position and the */
		  /* size of the corresponding window */
		  ConfigGetViewGeometry (SelectedDocument, pRoot->ElStructSchema->SsRule[pRoot->ElTypeNumber - 1].SrName, &X, &Y, &width, &height);
		  /* send qan event to the application before opening */
		  /* the view */
		  notifyDoc.event = TteViewOpen;
		  notifyDoc.document = doc;
		  notifyDoc.view = 0;
		  if (!CallEventType ((NotifyEvent *) & notifyDoc, TRUE))
		    /* the application accepts */
		    {
		      /* open the view */
		      createdView = CreateAbstractImage (SelectedDocument, 0, pRoot->ElTypeNumber,
							 pRoot->ElStructSchema, 0, TRUE, NULL);
		      OpenCreatedView (SelectedDocument, createdView, TRUE, X, Y, width, height);
		      /* tell the application that the view has */
		      /* been opened */
		      notifyDoc.event = TteViewOpen;
		      notifyDoc.document = doc;
		      notifyDoc.view = createdView + 100;
		      CallEventType ((NotifyEvent *) & notifyDoc, FALSE);
		      /* and try to select the element in this view */
		      abExist = SelectAbsBoxes (pEl, FALSE);
		    }
		}
	    }
	  else
	    /* the element is part of the main tree */
	    {
	      /* try to open all views defined in the presentation */
	      /* schema that are not open yet, until the element may */
	      /* have an abstract box in a view */
	      /* first, search a free view in the document descriptor */
	      docView = 1;
	      freeView = 0;
	      while (freeView == 0 && docView <= MAX_VIEW_DOC)
		if (SelectedDocument->DocView[docView - 1].DvPSchemaView == 0)
		  freeView = docView;
		else
		  docView++;

	      if (freeView != 0)
		/* there is room for a new view */
		{
		  /* build the list of all possible views for the */
		  /* document */
		  nViews = BuildDocumentViewList (SelectedDocument, viewTable);
		  for (i = 0; i < nViews && !abExist; i++)
		    {
		      if (!viewTable[i].VdAssoc)
			/* it's a view of the main tree */
			if (!viewTable[i].VdOpen)
			  /* it's not open yet */
			  {
			    /* create that view */
			    createdView = CreateAbstractImage (SelectedDocument,
							       viewTable[i].VdView, 0,
							       viewTable[i].VdSSchema, 1,
							       FALSE, NULL);
			    /* now, try to select the elment */
			    abExist = SelectAbsBoxes (pEl, FALSE);
			    deleteView = TRUE;
			    if (pEl->ElAbstractBox[createdView - 1] != NULL)
			      /* there is an abstract box for */
			      /* the element! */
			      {
				deleteView = FALSE;
				abExist = TRUE;
				/* search in the config file the */
				/* position and size of the view */
				/* to be open */
				ConfigGetViewGeometry (SelectedDocument, viewTable[i].VdViewName, &X, &Y, &width, &height); 
				/* send an event to the application*/
				notifyDoc.event = TteViewOpen;
				notifyDoc.document = doc;
				notifyDoc.view = createdView;
				if (CallEventType ((NotifyEvent *) & notifyDoc, TRUE))
				  /* application does not want Thot
				     to create a view */
				  deleteView = TRUE;
				else
				  {
				    /* open the new view */
				    OpenCreatedView (SelectedDocument, createdView, assoc, X, Y,
						     width, height);
				    /* tell the application that */
				    /* the view has been opened */
				    notifyDoc.event = TteViewOpen;
				    notifyDoc.document = doc;
				    notifyDoc.view = createdView;
				    CallEventType ((NotifyEvent *) & notifyDoc, FALSE);
				  }
			      }
			    if (deleteView)
			      /* the element is not visible in this view */
			      /* or the application does not want */
			      /* the view to be opened */
			      /* delete the created abstract image */
			      if (assoc)
				{
				  createdView--;
				  FreeAbView (SelectedDocument->DocAssocRoot[createdView]->ElAbstractBox[0], SelectedDocument->DocAssocFrame[createdView]);
				  SelectedDocument->DocAssocRoot[createdView]->ElAbstractBox[0] = NULL;
				  SelectedDocument->DocAssocFrame[createdView] = 0;
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

   Highlight the first ancestor of element pEl that has an abstract box.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                HighlightVisibleAncestor (PtrElement pEl)
#else  /* __STDC__ */
void                HighlightVisibleAncestor (pEl)
PtrElement          pEl;
#endif /* __STDC__ */

{
   int                 view, lastView, frame;
   ThotBool            assoc, found, abExist;
   PtrElement          pAncest;

   view = 0;
   if (pEl != NULL)
     {
	if (AssocView (pEl))
	   /* associated elements have only one view */
	  {
	     assoc = TRUE;
	     lastView = 1;
	  }
	else
	   /* consider all views for other elements */
	  {
	     assoc = FALSE;
	     lastView = MAX_VIEW_DOC;
	  }
	found = FALSE;
	pAncest = pEl->ElParent;
	/* search the first ancestor that has an abstract box in a view */
	while (!found && pAncest != NULL)
	  {
	     view = 1;
	     /* scan all views */
	     while (view <= lastView && !found)
		if (pAncest->ElAbstractBox[view - 1] != NULL)
		   found = TRUE;
		else
		   view++;
	     if (!found)
		pAncest = pAncest->ElParent;
	  }
	if (pAncest != NULL)
	   /* there is an ancestor with an abstract box */
	  {
	     if (assoc)
		/* associated element view */
		frame = SelectedDocument->DocAssocFrame[pEl->ElAssocNum - 1];
	     else if (SelectedDocument->DocView[view - 1].DvPSchemaView > 0)
		frame = SelectedDocument->DocViewFrame[view - 1];
	     else
		frame = 0;
	     /* highlight the ancestor found */
	     DisplaySel (pAncest, view, frame, assoc, &abExist);
	  }
     }
}


/*----------------------------------------------------------------------
   SelectStringInAttr

   The new current selection is now the character string contained
   in the text buffer of abstract box pAb, starting at rank firstChar
   and ending at rank last Char.
   pAb is a presentation abstract box that contains the value of a
   numerical or textual attribute.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         SelectStringInAttr (PtrDocument pDoc, PtrAbstractBox pAb, int firstChar, int lastChar, ThotBool string)
#else  /* __STDC__ */
static void         SelectStringInAttr (pDoc, pAb, firstChar, lastChar, string)
PtrDocument         pDoc;
PtrAbstractBox      pAb;
int                 firstChar;
int                 lastChar;
ThotBool            string;

#endif /* __STDC__ */
{
   PtrElement          pEl;
   PtrAbstractBox      pAbView;
   int                 frame, lastView, view;
   ThotBool            assoc;

   if (pAb == NULL || pDoc == NULL)
      return;
   pEl = pAb->AbElement;
   if (pEl->ElIsCopy)
      /* the string to be selected is in a copy element */
      /* Select the whole element. SelectElement will select the first */
      /* ancestor that is not a copy */
      SelectElement (pDoc, pEl, TRUE, TRUE);
   else
     {
	DocSelectedAttr = pDoc;
	AbsBoxSelectedAttr = pAb;
	FirstSelectedCharInAttr = firstChar;
	LastSelectedCharInAttr = lastChar;
	/* determine the views to be processed */
	assoc = AssocView (AbsBoxSelectedAttr->AbElement);
	if (assoc)
	   /* associated elements have only one view */
	   lastView = 1;
	else
	   /* for other elements, process all views */
	   lastView = MAX_VIEW_DOC;
	SelPosition = !string;
	/* highlight the new selection in all views */
	for (view = 0; view < lastView; view++)
	  {
	     /* frame: window where the view is displayed */
	     if (assoc)
		frame = pDoc->DocAssocFrame[pAb->AbElement->ElAssocNum - 1];
	     else if (pDoc->DocView[view].DvPSchemaView > 0)
		frame = pDoc->DocViewFrame[view];
	     else
		frame = 0;	/* vue non creee */
	     /* if the view exists, highlight selection in that view */
	     if (frame > 0)
	       {
		  /* search in the view the presentation abstract box that */
		  /* contains an attribute value */
		  pAbView = GetAbsBoxSelectedAttr (view+1);
		  /* switch the former selection off in that view */
		  ClearViewSelection (frame);
		  if (pAbView != NULL)
		    {
		      /* highlight the new selection */
		       InsertViewSelMarks (frame, pAbView, firstChar,
					   lastChar, TRUE, TRUE, TRUE);
		       ShowSelectedBox (frame, TRUE);
		       if (ThotLocalActions[T_switchsel])
			 (*ThotLocalActions[T_switchsel]) (frame, TRUE);
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
   SelectStringOrPosition

   Set the current selection to the string beginning at position firstChar
   and ending at position lastChar in the text element pEl.
   If pEl is a polyline, the vertex having rank firstChar is selected.
   string indicates if a string is selected (TRUE) or a position between
   two characters.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         SelectStringOrPosition (PtrDocument pDoc, PtrElement pEl, int firstChar, int lastChar, ThotBool string)
#else  /* __STDC__ */
static void         SelectStringOrPosition (pDoc, pEl, firstChar, lastChar, string)
PtrDocument         pDoc;
PtrElement          pEl;
int                 firstChar;
int                 lastChar;
ThotBool            string;

#endif /* __STDC__ */
{
   int                 i;
   ThotBool            elVisible;
   PtrElement          oldFirstSelEl;
   int                 oldFirstSelChar;
   PtrElement          pAncest;
   ThotBool            holophrast;

   /* If the selected element is in a holophrasted subtree, the root of that */
   /* subtree is selected */
   pAncest = pEl;
   holophrast = FALSE;
   while (pAncest != NULL && !holophrast)
      if (pAncest->ElHolophrast)
	 holophrast = TRUE;
      else
	 pAncest = pAncest->ElParent;

   if (pEl != NULL && pDoc != NULL)
      /* ignore exception NoSelect */
      if (pEl->ElIsCopy || holophrast)
	 /* the string to be selected is in a copy or holophrasted element */
	 /* select that element: SelectElement will select the first */
	 /* that can be selected */
	 SelectElement (pDoc, pEl, TRUE, TRUE);
      else
	{
	   oldFirstSelEl = FirstSelectedElement;
	   oldFirstSelChar = FirstSelectedChar;
	   if (string && pEl->ElLeafType == LtText)
	      if (lastChar == 0)
		 lastChar = pEl->ElTextLength;
	      else if (lastChar < firstChar)
		{
		   i = lastChar;
		   lastChar = firstChar;
		   firstChar = i;
		}
	   /* Is the new selection in the same tree as the former seelction */
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
	   /* pEl becomes the fixed element of the selection */
	   FixedElement = pEl;
	   FixedChar = FirstSelectedChar;
	   SelContinue = TRUE;
	   NSelectedElements = 0;
	   SelPosition = !string;
	   /* highlight boxes of current selection */
	   elVisible = SelectAbsBoxes (pEl, TRUE);
	   if (!elVisible)
	      /* the current selection is not visible. Highlight the first */
	      /* ancestor that is visible */
	      HighlightVisibleAncestor (pEl);

	   /* update all menus that must change in all open views */
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
	      /* the new selection starts in the same element as before */
	      if ((oldFirstSelChar <= 1 && FirstSelectedChar > 1) ||
		  (oldFirstSelChar > 1 && FirstSelectedChar <= 1))
	         /* the selection was at the beginning of the element and */
		 /* it is no longer at the beginnig or it was within the  */
		 /* element and it is now at the beginning */
	         /* Item Split in  the Edit menu must be updated */
	         if (SelectionUpdatesMenus)
		    if (ThotLocalActions[T_chsplit] != NULL)
		       (*ThotLocalActions[T_chsplit]) (pDoc);
	}
}


/*----------------------------------------------------------------------
   MoveCaret

   Set the current selection to the position before the character of rank
   firstChar in the text element pEl.
   If pEl is a polyline element, the vertex of rank firstChar is selected.
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
   SelectString

   Set the current selection to the string beginning at position firstChar
   and ending at position lastChar in the text element pEl.
   If pEl is a polyline, the vertex of rank firstChar is selected.
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
   ThotBool            string;

   /* it's a string, not a position within a string */
   string = TRUE;
   if (firstChar == lastChar+1)
      /* it's a position */
      string = FALSE;
   SelectStringOrPosition (pDoc, pEl, firstChar, lastChar, string);
}

/*----------------------------------------------------------------------
   SelectElement

   Set the current selection to element pEl in document pDoc.
   If this element is not supposed to be shown to the user, its first or
   last child is selected instead, depending on parameter begin, except
   when check is FALSE.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SelectElement (PtrDocument pDoc, PtrElement pEl, ThotBool begin, ThotBool check)
#else  /* __STDC__ */
void                SelectElement (pDoc, pEl, begin, check)
PtrDocument         pDoc;
PtrElement          pEl;
ThotBool            begin;
ThotBool            check;

#endif /* __STDC__ */
{
   PtrElement          pAncest, pE;
   ThotBool            bool, stop, elVisible;

   if (pEl != NULL && pDoc != NULL && pEl->ElStructSchema != NULL)
     {
	if (check)
	  {
	     stop = FALSE;
	     while (!stop)
		if (!TypeHasException (ExcHidden, pEl->ElTypeNumber, pEl->ElStructSchema))
		  /* exception Hidden is not associated with this elment type*/
		  {
		     stop = TRUE;
		     if (!pEl->ElTerminal && pEl->ElFirstChild != NULL)
			/* get the structure rule defining the element type */
			if (pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrConstruct ==
			    CsChoice)
			  {
			     /* it's a Choice with a child. Select the child */
			     pEl = pEl->ElFirstChild;
			     stop = FALSE;
			  }
		  }
		else if (pEl->ElTerminal || pEl->ElFirstChild == NULL)
		   /* this element type has exception Hidden */
		   /* the element has no child */
		   stop = TRUE;
		else
		  {
		     /* choose the first or last child, according to */
		     /* parameter begin */
		     pEl = pEl->ElFirstChild;
		     if (!begin)
			while (pEl->ElNext != NULL)
			   pEl = pEl->ElNext;
		  }
	  }
	/* If the element to be selected is a protected copy, select the */
	/* highest level protected ancestor */
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
	/* If the element is in a holophrasted tree, selected the */
	/* holphrasted ancestor */
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

	/* Is the new selected element in the same tree as the previous one? */
	FirstSelectedElement = pEl;
	if (pDoc != SelectedDocument)
	  /* the new selection is in a different document */
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
	/* If the new selection is in the same tree, SelectAbsBoxes will */
	/* switch the previous selection off */
	/* ignore exception NoSelect */
	SelContinue = TRUE;
	NSelectedElements = 0;
	LastSelectedElement = FirstSelectedElement;
	SelectedPointInPolyline = 0;
	SelectedPictureEdge = 0;
	FirstSelectedChar = 0;
	LastSelectedChar = 0;
	FixedElement = pEl;
	FixedChar = 0;
	/* If the selected element is empty or is a picture, the new */
	/* selection is simply considered as an insertion position */
	if ((pEl->ElTerminal &&
	     (pEl->ElVolume == 0 || pEl->ElLeafType == LtPicture)) ||
	    (!pEl->ElTerminal && pEl->ElFirstChild == NULL))
	   SelPosition = TRUE;
	else
	   SelPosition = FALSE;
	elVisible = SelectAbsBoxes (FirstSelectedElement, TRUE);
	if (!elVisible)
	   /* the selection is not visible, highlight the first visible */
	   /* ancestor */
	   HighlightVisibleAncestor (FirstSelectedElement);
	/* call the procedure handling selection in tables */
	if (ThotLocalActions[T_selecttable] != NULL)
	   (*ThotLocalActions[T_selecttable]) (FirstSelectedElement,
					    SelectedDocument, FALSE, &bool);
	/* if the selected element is a paired element, select the other */
	/* element of the pair too */
	if (FirstSelectedElement->ElStructSchema->SsRule[FirstSelectedElement->ElTypeNumber - 1].SrConstruct ==
	    CsPairedElement)
	  {
	     AddInSelection (GetOtherPairedElement (FirstSelectedElement), TRUE);
	     if (!FirstSelectedElement->ElStructSchema->
		  SsRule[FirstSelectedElement->ElTypeNumber - 1].SrFirstOfPair)
		/* the first selected element is the second element of the */
	        /* pair. Exchange first and last selected elements */
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
		/* if a symbol is selected, display the symbol palette */
/******		if (FirstSelectedElement->ElLeafType == LtSymbol)
		   TtaSetCurrentKeyboard (0);
******/
		/* if a graphic shape is selected, display the graphic palette */
		if (FirstSelectedElement->ElLeafType == LtGraphics)
		   TtaSetCurrentKeyboard (1);
		/* if a polyline is selected, display the graphic palette */
		else if (FirstSelectedElement->ElLeafType == LtPolyLine)
		   TtaSetCurrentKeyboard (1);
	     }
	/* update all the menus that depend on the current selection */
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
   ExtendSelection

   Extend current selection to element pEl.
   If rank = 0, element pEl is entirely selected.
   If rank > 0, extend selection to the character having that rank in the
   text element pEl.
   If fixed is TRUE, the new selection is between the current fixed point and
   element pEl; parameter begin is meaningless.
   If fixed is FALSE,
      if begin is TRUE, the beginning of the curent selection is moved to
              element pEl
      if begin is FALSE, the end of the current selection is extended to
              element pEl.
   If drag is TRUE, only the minimum processing is done.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                ExtendSelection (PtrElement pEl, int rank, ThotBool fixed, ThotBool begin, ThotBool drag)

#else  /* __STDC__ */
void                ExtendSelection (pEl, rank, fixed, begin, drag)
PtrElement          pEl;
int                 rank;
ThotBool            fixed;
ThotBool            begin;
ThotBool            drag;

#endif /* __STDC__ */

{
   PtrElement          oldFirstEl, oldLastEl, pElP;
   int                 oldFirstChar, oldLastChar;
   ThotBool            change, done, sel;

   sel = TRUE;
   if (pEl != NULL)
      /* call procedure handling selection in tables, if it is present */
      if (ThotLocalActions[T_selecttable] != NULL)
	 (*ThotLocalActions[T_selecttable]) (pEl, SelectedDocument, TRUE, &sel);
   if (sel)
     {
	done = FALSE;
	if (!SelContinue || pEl != FirstSelectedElement)
	  /* call the procedure handling selection in drawing */
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
		/* element pEl is holophrasted, select it entirely */
		rank = 0;
	     if (!fixed && begin)
		/* change fixed point by moving it first to the end */
	       {
		  FixedElement = LastSelectedElement;
		  if (LastSelectedChar < rank)
		     LastSelectedChar = rank;
		  FixedChar = LastSelectedChar;
	       }
	     if (pEl == FixedElement)
		/* extension within fixed element */
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
	     else if (ElemIsAnAncestor (pEl, FixedElement))
		/* extension to an ancestor of the fixed point. Select that */
	        /* ancestor */
	       {
		  FirstSelectedElement = pEl;
		  FirstSelectedChar = 0;
		  LastSelectedElement = pEl;
		  LastSelectedChar = 0;
		  FixedElement = pEl;
		  FixedChar = 0;
	       }
	     else  if (ElemIsBefore (pEl, FixedElement))
	       /* pEl is before the fixed point */
	       {
		  FirstSelectedElement = pEl;
		  FirstSelectedChar = rank;
		  LastSelectedElement = FixedElement;
		  LastSelectedChar = FixedChar;
	       }
	     else
	       /* pEl is after the fixed point */
	       {
		  LastSelectedElement = pEl;
		  LastSelectedChar = rank;
		  FirstSelectedElement = FixedElement;
		  FirstSelectedChar = FixedChar;
	       }
	     /* adjust selection */
	     if (FirstSelectedElement->ElTerminal &&
		 FirstSelectedElement->ElLeafType == LtText &&
		 FirstSelectedElement->ElTextLength > 0 &&
		 FirstSelectedElement->ElTextLength < FirstSelectedChar &&
		 FirstSelectedElement != LastSelectedElement)
		/* the beginning of the selection is at the end of text */
		/* element FirstSelectedElement and it's not the selection */
		/* end. Set the beginning of selection to the beginning of */
	        /* next element */
	        {
		  FirstSelectedElement =
		             FirstLeaf (NextElement (FirstSelectedElement));
		  if (FirstSelectedElement->ElTerminal &&
		      FirstSelectedElement->ElLeafType == LtText)
		     FirstSelectedChar = 1;
		  else
		     FirstSelectedChar = 0;
	        }
	     if (StructSelectionMode)
	        /* selection is structured mode */
		/* normalize selection: the first and last selected elements */
		/* must be siblings in the abstract tree */
		if (FirstSelectedElement->ElParent !=
		                                LastSelectedElement->ElParent)
		  {
		     /* consider the ancestors of the first selected element */
		     pElP = FirstSelectedElement->ElParent;
		     while (pElP != NULL)
			if (ElemIsAnAncestor (pElP, LastSelectedElement))
			   /* this ancestor (pElP) is an ancestor of the */
			   /* selected element */
			  {
			     if (LastSelectedElement->ElParent != pElP)
				LastSelectedChar = 0;
			     /* the last selected is now the ancestor of the */
			     /* last selected element whose parent is pElP */
			     while (LastSelectedElement->ElParent != pElP)
				LastSelectedElement =
				                LastSelectedElement->ElParent;
			     /* finished */
			     pElP = NULL;
			  }
			else
			   /* this ancestor (pElP) is not an ancestor of the */
			   /* last selected element */
			  {
			     /* keep pElP and check whether is parent is an */
			     /* ancestor of the last selected element */
			     FirstSelectedElement = pElP;
			     /* keep the entire element */
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
		/* the new selection is not the same as the previous one */
		/* highlight it */
		HighlightSelection ((ThotBool)(FirstSelectedElement != oldFirstEl), TRUE);
	     if (!drag)
		/* update all menus that change with the selection */
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
		/* change the fixed point: move it to the begining */
	       {
		  FixedElement = FirstSelectedElement;
		  FixedChar = FirstSelectedChar;
	       }
	  }
     }
}


/*----------------------------------------------------------------------
   ReverseSelect

   Highlight or switch off (according to parameter highlight) all
   boxes of element pEl in document pDoc.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ReverseSelect (PtrElement pEl, PtrDocument pDoc, ThotBool highlight)

#else  /* __STDC__ */
static void         ReverseSelect (pEl, pDoc, highlight)
PtrElement          pEl;
PtrDocument         pDoc;
ThotBool            highlight;

#endif /* __STDC__ */

{
   PtrAbstractBox      pAb;
   int                 lastView, view, frame;

   if (AssocView (pEl))
      /* associated elements have only one view */
      lastView = 1;
   else
      /* for other elements, consider all views */
      lastView = MAX_VIEW_DOC;
   for (view = 0; view < lastView; view++)
     {
	/* get the window corresponding to the view */
	if (AssocView (pEl))
	   frame = pDoc->DocAssocFrame[pEl->ElAssocNum - 1];
	else if (pDoc->DocView[view].DvPSchemaView > 0)
	   frame = pDoc->DocViewFrame[view];
	else
	   frame = 0;
	/* scan all abstract boxes of the element in the view */
	pAb = pEl->ElAbstractBox[view];
	while (pAb != NULL)
	  {
	     pAb->AbSelected = highlight;
	     SetNewSelectionStatus (frame, pAb, highlight);
	     /* get the next abstract box for the element */
	     pAb = pAb->AbNext;
	     if (pAb != NULL)
		if (pAb->AbElement != pEl)
		   /* next abstract box does not belong to the element */
		   pAb = NULL;
	  }
     }
}

/*----------------------------------------------------------------------
   AddInSelection

   Add element pEl to the current selection.
   Parameter last indicates if it's the last element added to the current
   selection.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                AddInSelection (PtrElement pEl, ThotBool last)
#else  /* __STDC__ */
void                AddInSelection (pEl, last)
PtrElement          pEl;
ThotBool            last;

#endif /* __STDC__ */
{
   int                 i;
   ThotBool            ok;

   if (pEl != NULL)
     {
	if (SelContinue)
	  {
	     /* the selection was a sequence of elements. Change to a */
	     /* discrete selection */
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
	/* check that the element to be added is not yet in the selection */
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
	     /* highlight the new selected element in all views where is */
	     /* is visible */
	     ReverseSelect (pEl, SelectedDocument, TRUE);
	     if (last && SelectionUpdatesMenus)
	       {
		  /* update all the menus that depend on the current */
		  /* selection */
		  if (ThotLocalActions[T_chselect] != NULL)
		     (*ThotLocalActions[T_chselect]) (SelectedDocument);
		  if (ThotLocalActions[T_chattr] != NULL)
		     (*ThotLocalActions[T_chattr]) (SelectedDocument);
	       }
	  }
     }
}

/*----------------------------------------------------------------------
   RemoveFromSelection

   Remove element pEl from the current selection, but only if the
   current selection is discrete.
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

   if (SelContinue)
      return;
   /* search that element in the selected elements */
   for (i = 0; i < NSelectedElements && SelectedElement[i] != pEl; i++) ;
   if (SelectedElement[i] == pEl)
     {
	/* remove that element from the list of selected elements */
	for (j = i + 1; j < NSelectedElements; j++)
	   SelectedElement[j - 1] = SelectedElement[j];
	NSelectedElements--;
	/* switch selection off */
	ReverseSelect (pEl, pDoc, FALSE);
	if (LatestReturned == i + 1)
	   LatestReturned = i;
     }
}

/*----------------------------------------------------------------------
  SelectElementWithEvent

  Same function as SelectElement, but send  events TteElemSelect.Pre and
   TteElemSelect.Post to the application
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SelectElementWithEvent (PtrDocument pDoc, PtrElement pEl, ThotBool begin, ThotBool check)
#else  /* __STDC__ */
void                SelectElementWithEvent (pDoc, pEl, begin, check)
PtrDocument         pDoc;
PtrElement          pEl;
ThotBool            begin;
ThotBool            check;
#endif /* __STDC__ */

{
   NotifyElement       notifyEl;
   Document            doc;

   if (pDoc != NULL && pEl != NULL)
     {
        doc = IdentDocument (pDoc);
	notifyEl.event = TteElemSelect;
	notifyEl.document = doc;
	notifyEl.element = (Element) pEl;
	notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
	notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
	notifyEl.position = 0;
	if (!CallEventType ((NotifyEvent *) & notifyEl, TRUE))
	  {
	     SelectElement (pDoc, pEl, begin, check);
	     notifyEl.event = TteElemSelect;
	     notifyEl.document = doc;
	     notifyEl.element = (Element) pEl;
	     notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
	     notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
	     notifyEl.position = 0;
	     CallEventType ((NotifyEvent *) & notifyEl, FALSE);
	  }
     }
}

/*----------------------------------------------------------------------
   SelectPositionWithEvent

   Same function as MoveCaret, but send  events TteElemSelect.Pre and
   TteElemSelect.Post to the application.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SelectPositionWithEvent (PtrDocument pDoc, PtrElement pEl, int first)
#else  /* __STDC__ */
void                SelectPositionWithEvent (pDoc, pEl, first)
PtrDocument         pDoc;
PtrElement          pEl;
int                 first;
#endif /* __STDC__ */

{
   NotifyElement       notifyEl;
   Document            doc;

   if (pDoc != NULL && pEl != NULL)
     {
        doc = IdentDocument (pDoc);
	notifyEl.event = TteElemSelect;
	notifyEl.document = doc;
	notifyEl.element = (Element) pEl;
	notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
	notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
	notifyEl.position = 0;
	if (!CallEventType ((NotifyEvent *) & notifyEl, TRUE))
	  {
	     SelectStringOrPosition (pDoc, pEl, first, first, FALSE);
	     notifyEl.event = TteElemSelect;
	     notifyEl.document = doc;
	     notifyEl.element = (Element) pEl;
	     notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
	     notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
	     notifyEl.position = 0;
	     CallEventType ((NotifyEvent *) & notifyEl, FALSE);
	  }
     }
}

/*----------------------------------------------------------------------
   SelectStringWithEvent

   Same function as SelectString, but send events TteElemSelect.Pre and
   TteElemSelect.Post to the application
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
   Document            doc;

   if (pDoc != NULL && pEl != NULL)
     {
        doc = IdentDocument (pDoc);
	notifyEl.event = TteElemSelect;
	notifyEl.document = doc;
	notifyEl.element = (Element) pEl;
	notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
	notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
	notifyEl.position = 0;
	if (!CallEventType ((NotifyEvent *) & notifyEl, TRUE))
	  {
	     SelectStringOrPosition (pDoc, pEl, firstChar, lastChar, TRUE);
	     notifyEl.event = TteElemSelect;
	     notifyEl.document = doc;
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

   The user wants to make a new selection or an extension to the current
   selection, according to parameter extension.
   frame: the window where the user has clicked.
   pAb: the abstract box where the user has clicked.
   rank: rank of the character on which the user has clicked, or 0 if the
         whole abstract box has been designated.
   update: the display module asks for the selection to be updated.
         if update is FALSE, the user wants to extend the current selection
         and parameter extension means:
               TRUE: the beginning of the current selection is extended,
               FALSE: the end of the current selection is extended.
   doubleClick: if TRUE, the user has double-clicked without moving the mouse.
   drag: the user extends the selection by dragging.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ChangeSelection (int frame, PtrAbstractBox pAb, int rank, ThotBool extension, ThotBool update, ThotBool doubleClick, ThotBool drag)
#else  /* __STDC__ */
void                ChangeSelection (frame, pAb, rank, extension, update, doubleClick, drag)
int                 frame;
PtrAbstractBox      pAb;
int                 rank;
ThotBool            extension;
ThotBool            update;
ThotBool            doubleClick;
ThotBool            drag;
#endif /* __STDC__ */
{
   PtrDocument         pDoc;
   PtrElement          pEl, pEl1;
   PtrAttribute        pAttr;
   NotifyElement       notifyEl;
   Document            doc;
   int                 view, numassoc;
   ThotBool            assoc, error, fixed, begin, stop, doubleClickRef;

   numassoc = 0;
   pEl1 = NULL;
   /* search the document and the view corresponding to the window */
   GetDocAndView (frame, &pDoc, &view, &assoc);
   doc = IdentDocument (pDoc);
   if (doubleClick && pAb != NULL)
     {
       pEl1 = pAb->AbElement;
       if (pEl1 != NULL)
	 {
	   /* send event TteElemActivate.Pre to the application */
	   notifyEl.event = TteElemActivate;
	   notifyEl.document = doc;
	   notifyEl.element = (Element) pEl1;
	   notifyEl.elementType.ElTypeNum = pEl1->ElTypeNumber;
	   notifyEl.elementType.ElSSchema = (SSchema) (pEl1->ElStructSchema);
	   notifyEl.position = 0;
	   if (CallEventType ((NotifyEvent *) & notifyEl, TRUE))
	     /* the application asks Thot to do nothing */
	     return;
	   if (pEl1->ElHolophrast)
	     {
	       /* avoid to rebuild menus. It will be done by */
	       /* SelectElement */
	       SelectedDocument = NULL;
	       /* switch off the previous selection */
	       CancelSelection ();
	       DeHolophrast (pEl1, pDoc);
	       SelectElementWithEvent (pDoc, pEl1, TRUE, FALSE);
	       return;
	     }
	 }
     }

   error = FALSE;
   doubleClickRef = FALSE;
   /* process double clicks and extensions for polyline vertices */
   if (pAb != NULL &&
       pAb->AbElement->ElTerminal &&
       pAb->AbElement->ElLeafType == LtPolyLine)
     /* it's a polyline */
     {
       if (extension)
	 /* it's a selection extension */
	 if (FirstSelectedElement == pAb->AbElement &&
	     rank == SelectedPointInPolyline)
	   /* same polyline and same vertex as before. Then, it's not */
	   /* really an extension */
	   extension = FALSE;
	 else
	   /* select the entire polyline */
	   rank = 0;
       if (doubleClick)
	 /* a double-click applies to the polyline as a whole */
	 rank = 0;
     }

   if (extension && SelectedDocument == NULL && DocSelectedAttr == NULL)
      /* it's an extension, but there is no selection. Consider it as a new */
      /* selection */
      extension = FALSE;

   /* if it's a double-click, check that the element is a reference or an */
   /* inclusion */
   if (doubleClick &&
       pAb != NULL &&
       pAb->AbElement != NULL)
     {
       pEl1 = pAb->AbElement;
       if (pEl1->ElStructSchema->SsRule[pEl1->ElTypeNumber - 1].SrConstruct != CsReference)
	 {
	   /* search for an inclusion among the ancestors */
	   pEl = pEl1;
	   while (pEl->ElParent != NULL && pEl->ElSource == NULL)
	     pEl = pEl->ElParent;
	   if (pEl->ElSource != NULL)
	     /* it's an inclusion */
	     pEl1 = pEl;
	 }
       if (pEl1->ElStructSchema->SsRule[pEl1->ElTypeNumber - 1].SrConstruct ==
	   CsReference || pEl1->ElSource != NULL)
	 /* this element is a reference or an inclusion */
	 {
	   doubleClickRef = TRUE;
	   FirstSelectedElement = pEl1;
	   LastSelectedElement = pEl1;
	   SelectedPointInPolyline = 0;
	   SelectedPictureEdge = 0;
	 }
       else
	 {
	   /* it's neither an inclusion nor a reference element */
	   /* search a reference attribute with exception ActiveRef */
	   /* among the ancestors */
	   pEl = pEl1;
	   do
	     {
	       pAttr = pEl->ElFirstAttr;
	       /* scan all attributes of current element */
	       while (pAttr != NULL && !doubleClickRef)
		 if (pAttr->AeAttrType == AtReferenceAttr &&
		     AttrHasException (ExcActiveRef, pAttr->AeAttrNum, pAttr->AeAttrSSchema))
		   /* a reference attribute has been found */
		   {
		     doubleClickRef = TRUE;
		     FirstSelectedElement = pEl;
		     LastSelectedElement = pEl;
		     SelectedPointInPolyline = 0;
		     SelectedPictureEdge = 0;
		   }
		 else
		   /* next attribute of same element */
		   pAttr = pAttr->AeNext;
	       if (!doubleClickRef)
		 /* higher level ancestor */
		 pEl = pEl->ElParent;
	     }
	   while (pEl != NULL && !doubleClickRef);
	 }
     }

   if (pAb != NULL &&
       pAb->AbElement->ElTerminal &&
       (pAb->AbElement->ElLeafType == LtPairedElem || pAb->AbElement->ElLeafType == LtReference) &&
       /* it's a reference element or a paired element */
       (!pAb->AbPresentationBox || !pAb->AbCanBeModified))
     /* it's not the presentation box of an attribute value */
     /* select all the contents */
     rank = 0;

   if (assoc)
     {
	numassoc = view;
	view = 1;
     }
   if (!update)
      FrameWithNoUpdate = frame;
   if (extension || !update)
      /* extension of current selection */
      if (DocSelectedAttr != NULL)
	 /* the current selection is within a presentation box that displays */
	 /* an attribute value */
	{
	   if (DocSelectedAttr == pDoc && AbsBoxSelectedAttr == pAb)
	      /* extension is allowed only if it's within the same box */
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
		if (TtaGetDisplayMode (FrameTable[frame].FrDoc) == DisplayImmediately)
		  SelectStringInAttr (pDoc, pAb, FirstSelectedCharInAttr,
				      LastSelectedCharInAttr, TRUE);
	     }
	}
      else
	{
	   if (pDoc != SelectedDocument ||
	       (assoc && numassoc != FirstSelectedElement->ElAssocNum))
	     /* extension to a different tree is not allowed */
	     {
		TtaDisplaySimpleMessage (INFO, LIB, TMSG_CHANGING_DOC_IMP);
		error = TRUE;
	     }
	   else
	     {
		/* same document, but is it the same tree? */
		if (FirstSelectedElement != NULL)
		   /* search an enclosing abstract box that belongs to the */
		   /* same associated tree as the first selected element */
		  {
		     stop = FALSE;
		     do
			if (pAb == NULL)
			   stop = TRUE;
			else if (FirstSelectedElement->ElAssocNum == pAb->AbElement->ElAssocNum)
			   stop = TRUE;
			else
			   pAb = pAb->AbEnclosing;
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
			/* the element to be selected is a constant */
			/* select it entirely */
			rank = 0;
		     /* If the element to be selected is hidden or cannot be */
		     /* selected, get the first ancestor that can be selected*/
		     if (TypeHasException (ExcNoSelect, pEl->ElTypeNumber,
					   pEl->ElStructSchema) ||
			 (HiddenType (pEl) &&
			  !ElementHasAction(pEl, TteElemExtendSelect, TRUE)))
		       {
			  stop = FALSE;
			  /* select the entire element */
			  rank = 0;
			  while (!stop)
			     if (pEl->ElParent == NULL)
				/* root of a tree. Select it */
				stop = TRUE;
			     else
			       {
			       pEl = pEl->ElParent;
			       if (!TypeHasException (ExcNoSelect,
						      pEl->ElTypeNumber,
						      pEl->ElStructSchema) &&
				   (!HiddenType (pEl) ||
				    ElementHasAction(pEl, TteElemExtendSelect, TRUE)))
				   stop = TRUE;
			       }
		       }
		     /* send event TteElemExtendSelect.Pre to the application*/
		     notifyEl.event = TteElemExtendSelect;
		     notifyEl.document = doc;
		     notifyEl.element = (Element) pEl;
		     notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
		     notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
		     notifyEl.position = 0;
		     if (!CallEventType ((NotifyEvent *) & notifyEl, TRUE))
		       /* application accepts selection */
		       {
			  /* do select */
			  ExtendSelection (pEl, rank, fixed, begin, drag);
			  /* send event TteElemExtendSelect.Pre to the */
			  /* application */
			  notifyEl.event = TteElemExtendSelect;
			  notifyEl.document = doc;
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
	/* new selection */
	if (pDoc != SelectedDocument)
	   /* in another document */
	   TtaClearViewSelections ();
	else if (!doubleClickRef)
	   /* selection in the same document as before */
	   if (doubleClick)
	      /* user has double-clicked */
	      if (SelectedView == view)
		 /* in same view */
		 if (FirstSelectedElement == LastSelectedElement)
		   /* only one element was selected */
		    if (pAb->AbElement == FirstSelectedElement)
		      /* the new selected element is the same as before */
		      {
			 pEl1 = FirstSelectedElement;
			 if (pEl1->ElStructSchema->SsRule[pEl1->ElTypeNumber - 1].SrConstruct ==
			     CsReference || pEl1->ElSource != NULL)
			    /* this element is a reference or an inclusion */
			    doubleClickRef = TRUE;
		      }
	if (doubleClick)
	  {
	     FindReferredEl ();
	     /* send an event TteElemActivate.Pre to the application */
	     notifyEl.event = TteElemActivate;
	     notifyEl.document = doc;
	     notifyEl.element = (Element) pEl1;
	     notifyEl.elementType.ElTypeNum = pEl1->ElTypeNumber;
	     notifyEl.elementType.ElSSchema = (SSchema) (pEl1->ElStructSchema);
	     notifyEl.position = 0;
	     CallEventType ((NotifyEvent *) & notifyEl, FALSE);
	  }
	else
	  {
	     /* the view where the user has clicked becomes the active view */
	     SelectedView = view;
	     OldSelectedView = view;
	     OldDocSelectedView = pDoc;
	     pEl = pAb->AbElement;
	     if (pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrConstruct == CsConstant)
		/* the element to be selected is a constant */
	        /* Select it entirely */
		rank = 0;
	     /* if the element to be selected has exception NoSelect or */
	     /* Hidden, select the first ancestor that can be selected */
	     /* However, if the element is hidden, but has specified a */
             /* callback for event Select, it is selected */
	     if (TypeHasException (ExcNoSelect, pEl->ElTypeNumber,
				   pEl->ElStructSchema) ||
		 (HiddenType (pEl) && !ElementHasAction(pEl, TteElemSelect, TRUE)))
	       {
		  /* select element entirely */
		  rank = 0;
		  stop = FALSE;
		  while (!stop)
		     if (pEl->ElParent == NULL)
			/* that's a root. Select it */
			stop = TRUE;
		     else
		       {
			  pEl = pEl->ElParent;
			  if (!TypeHasException (ExcNoSelect,pEl->ElTypeNumber,
						 pEl->ElStructSchema)  &&
			      (!HiddenType (pEl) ||
			       ElementHasAction(pEl, TteElemSelect, TRUE)))
			     stop = TRUE;
		       }
	       }
	     if (rank > 0 && pAb->AbPresentationBox && pAb->AbCanBeModified)
		/* user has clicked in the presentation box displaying an */
	        /* attribute value */
	       {
		  CancelSelection ();
		  if (TtaGetDisplayMode (FrameTable[frame].FrDoc) == DisplayImmediately)
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
	   /* If all the contents of a text leaf is selected, then the leaf */
	   /* itself is considered as selected */
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
	      /* reset enclosing abstract boxes for associated elements */
	      EnclosingAssocAbsBox (FirstSelectedElement);
     }
}

/*----------------------------------------------------------------------
   PrepareSelectionMenu

   Search elements to be put in the Select menu.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                PrepareSelectionMenu ()

#else  /* __STDC__ */
void                PrepareSelectionMenu ()
#endif				/* __STDC__ */

{
   PtrElement          pEl1;
   PtrElement          pEl2;
   ThotBool            stop;
   ThotBool            stop1;

   /* ignore exception NoSelect */
   /* search the first ancestor of the first selected element that is */
   /* not hidden */
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

   /* search the previous element that can be selected */
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

   /* search the next element that can be selected */
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

   /* search the first descendent that can be selected */
   SelMenuChildEl = NULL;
   stop = FALSE;
   pEl1 = FirstSelectedElement;
   while (!stop && pEl1 != NULL)
      /* cannot select the contents of a copy or hidden element */
      if (pEl1->ElIsCopy || ElementIsHidden (pEl1))
	 pEl1 = NULL;
      else if (pEl1->ElTerminal)
	 pEl1 = NULL;
      else
	{
	  /* search the first child that is not hidden */
	   pEl1 = pEl1->ElFirstChild;
	   stop1 = FALSE;
	   while (!stop1 && pEl1 != NULL)
	      if (!ElementIsHidden (pEl1))
		 stop1 = TRUE;
	      else
		 pEl1 = pEl1->ElNext;
	   if (pEl1 != NULL)
	      if (!HiddenType (pEl1))
		 /* this element can be selected */
		 stop = TRUE;
	}
   if (pEl1 != NULL)
      SelMenuChildEl = pEl1;
}

/*----------------------------------------------------------------------
   BuildSelectionMessage

   build the selection message according to the current selection
   and display that message
  ----------------------------------------------------------------------*/
void                BuildSelectionMessage ()
{
   PtrElement          pEl;
   PtrDocument         pDoc;
   CHAR_T              msgBuf[MAX_TXT_LEN];
   int                 nbasc;

   if (DocSelectedAttr != NULL && AbsBoxSelectedAttr != NULL)
      /* current selection is within an attribute value */
      /* present the element to which the attribute is attached */
     {
	pDoc = DocSelectedAttr;
	pEl = AbsBoxSelectedAttr->AbElement;
     }
   else
     {
	pDoc = SelectedDocument;
	pEl = FirstSelectedElement;
     }
   /* put the type name of the first selected element */
      ustrncpy (msgBuf, pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrName,
	       MAX_NAME_LENGTH);
   /* add the types of the ancestors */
   pEl = pEl->ElParent;
   nbasc = 1;
   while (pEl != NULL)
     {
	/* skip that ancestor if it is hidden */
	if (!HiddenType (pEl))
	  {
	     /* put a separator */
	     ustrcat (&msgBuf[ustrlen (msgBuf)], TEXT(" \\ "));
	     /* put the element type */
	     ustrcat (&msgBuf[ustrlen (msgBuf)], pEl->ElStructSchema->
		     SsRule[pEl->ElTypeNumber - 1].SrName);
	     nbasc++;
	  }
	if (nbasc >= MAX_ITEM_MSG_SEL)
	   pEl = NULL;
	else
	   pEl = pEl->ElParent;
     }
   /* if the Selection message or the selected document have changed, */
   /* display this new message */
   DisplaySelMessage (msgBuf, pDoc);
}


/*----------------------------------------------------------------------
   SelectPairInterval                               

   If the current selection is a paired element, select all elements
      comprised between the two elements of the pair, and the paired
      elements themselves, and return TRUE
   else
      just return FALSE

  ----------------------------------------------------------------------*/

ThotBool            SelectPairInterval ()
{
   ThotBool            ret;

   ret = FALSE;
   if (!SelContinue)
      if (NSelectedElements == 2)
	 /* only two elements are selected */
	 if (SelectedElement[0] == GetOtherPairedElement (SelectedElement[1]))
	    /* they are paired elements */
	   {
	      SelContinue = TRUE;
	      LastSelectedElement = FirstSelectedElement;
	      ExtendSelection (SelectedElement[1], 0, FALSE, TRUE, FALSE);
	      ret = TRUE;
	   }
   return ret;
}


/*----------------------------------------------------------------------
   SelectAround

   Select an element relatively to current selection:

   - parent of first selected element if val = 1
   - previous element if val = 2
   - next element if val = 3
   - child of first selected element if val = 4
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SelectAround (int val)
#else  /* __STDC__ */
void                SelectAround (val)
int                 val;

#endif /* __STDC__ */
{
   PtrElement          pEl, pParent, pFirst, pLast;
   int                 lg;

   pEl = NULL;
   lg = 0;
   switch (val)
	 {
	    case 1:
	       /* select parent */
	       if (SelectPairInterval ())
		  pEl = NULL;
	       else
		 {
		    if (DocSelectedAttr != NULL && AbsBoxSelectedAttr != NULL)
		       /* current selection is within an attribute value */
		       /* select the element to which the attribute is */
		       /* attached */
		       SelectElementWithEvent (DocSelectedAttr,
				AbsBoxSelectedAttr->AbElement, TRUE, FALSE);
		    else
		       if (StructSelectionMode)
		         pEl = SelMenuParentEl;
		       else
			 /* selection is not always "normalized". Select the */
			 /* parent only if the selection is "normalized", */
			 /* normalize it otherwise. */
		         {
			 if (FirstSelectedElement != NULL &&
			     LastSelectedElement != NULL)
			    if (FirstSelectedElement->ElParent ==
				LastSelectedElement->ElParent)
			      /* selection is normalized */
			       pEl = SelMenuParentEl;
			    else
		              /* The first and last selected elements are */
		              /* not siblings. Change the selection so that */
		              /* first and last selected elements be siblings*/
			      {
				 pFirst = FirstSelectedElement;
				 pLast = LastSelectedElement;
				 /* get the ancestors of the first selected */
				 /* element */
				 pParent = pFirst->ElParent;
				 while (pParent != NULL)
				    if (ElemIsAnAncestor (pParent, pLast))
				      /* this ancestor (pParent) of first */
				      /* selected element is also an ancestor*/
				      /* of the last selected element */
				      {
					/* replace the last selected element*/
					/* by its ancestor whose parent is */
					/* pParent */
					 while (pLast->ElParent != pParent)
					    pLast = pLast->ElParent;
					 /* finished */
					 pParent = NULL;
				      }
				    else
				      /* this ancestor (pParent) of the first*/
				      /* selected element is not an ancestor */
				      /* of the last selected element */
				      {
					/* replace priovisionally the first */
					/* selected element by pParent and */
					/* whether its parent is an ancestor */
					/* of the last selected element */
					 pFirst = pParent;
					 pParent = pParent->ElParent;
				      }
				 SelectElementWithEvent (SelectedDocument,
						         pFirst, TRUE, FALSE);
				 if (pFirst != pLast)
				    ExtendSelection (pLast, 0, FALSE, TRUE, FALSE);
			      }
		         }
		 }
	       break;
	    case 2:
	       /* Previous */
	       pEl = SelMenuPreviousEl;
	       if (pEl)
	          lg = pEl->ElVolume;
	       break;
	    case 3:
	       /* Next */
	       pEl = SelMenuNextEl;
	       break;
	    case 4 /* child */ :
	       pEl = SelMenuChildEl;
	       break;
	    default:
	       break;
	 }
   if (pEl != NULL)
      if (!ElementIsHidden (pEl))
	{
	  /****if (SelPosition && pEl->ElTypeNumber == 1)
	      SelectString (SelectedDocument, pEl, lg, lg);
	  else****/
	    SelectElementWithEvent (SelectedDocument, pEl, TRUE, FALSE);
	}
}


/*----------------------------------------------------------------------
   TtcParentElement

   Select the parent of the first selected element
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcParentElement (Document document, View view)
#else  /* __STDC__ */
void                TtcParentElement (document, view)
Document            document;
View                view;
CHAR_T                c;

#endif /* __STDC__ */
{
   SelectAround (1);
}


/*----------------------------------------------------------------------
   TtcPreviousElement

   Select the element preceding the first selected element
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcPreviousElement (Document document, View view)
#else  /* __STDC__ */
void                TtcPreviousElement (document, view)
Document            document;
View                view;
CHAR_T                c;

#endif /* __STDC__ */
{
   SelectAround (2);
}


/*----------------------------------------------------------------------
   TtcNextElement

   Select the element following the last selected element
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcNextElement (Document document, View view)
#else  /* __STDC__ */
void                TtcNextElement (document, view)
Document            document;
View                view;
CHAR_T                c;

#endif /* __STDC__ */
{
   SelectAround (3);
}

/*----------------------------------------------------------------------
   TtcChildElement

   Select the first child of the first selected element
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcChildElement (Document document, View view)
#else  /* __STDC__ */
void                TtcChildElement (document, view)
Document            document;
View                view;
CHAR_T                c;

#endif /* __STDC__ */
{
   SelectAround (4);
}

