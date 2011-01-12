/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2011
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * This module handles selection in abstract trees
 *
 * Authors: V. Quint, I. Vatton (INRIA)
 *
 */
#ifdef _WX
#include "wx/wx.h"
#include "wx/colordlg.h"
#endif /* _WX */

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "appaction.h"
#include "appstruct.h"
#include "libmsg.h"
#include "message.h"
#include "appdialogue.h"
#ifdef _WX
#include "paneltypes_wx.h"
#include "dialog.h"
#include "appdialogue_wx.h"
#endif /* _WX */

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "edit_tv.h"
#include "frame_tv.h"
#include "appdialogue_tv.h"
#include "select_tv.h"

#include "absboxes_f.h"
#include "appli_f.h"
#include "applicationapi_f.h"
#include "boxselection_f.h"
#include "buildboxes_f.h"
#include "callback_f.h"
#include "changepresent_f.h"
#include "checkermenu_f.h"
#include "config_f.h"
#include "createabsbox_f.h"
#include "displayselect_f.h"
#include "displayview_f.h"
#include "exceptions_f.h"
#include "font_f.h"
#include "frame_f.h"
#include "scroll_f.h"
#include "search_f.h"
#include "searchmenu_f.h"
#include "searchref_f.h"
#include "selectionapi_f.h"
#include "structmodif_f.h"
#include "structselect_f.h"
#include "tree_f.h"
#include "unstructchange_f.h"
#include "views_f.h"
#include "windowdisplay_f.h"
#ifdef _GL
#include "glwindowdisplay.h"
#endif /* _GL */
#ifdef _GTK
#include "gtk-functions.h"
#endif /* _GTK */

static int          NSelectedElements;	/* number of elements in table
                                           SelectedElement */
static PtrElement   SelectedElement[MAX_SEL_ELEM]; /* the selected elements if
                                                      SelContinue is FALSE */
static int          LatestReturned;	/* rank in table SelectedElement of the
                                       last element returned by function
                                       NextInSelection */
static int          OldSelectedView;	/* old active view */
static PtrDocument  OldDocSelectedView;	/* the document to which the old active
                                           view belongs */
#define MAX_TRANSMIT 10


/*----------------------------------------------------------------------
  InitSelection

  initialize variable describing the current selection.
  ----------------------------------------------------------------------*/
void InitSelection ()
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
  FirstSelectedColumn = NULL;
  LastSelectedColumn = NULL;
  WholeColumnSelected = FALSE;
  SelPosition = FALSE;
}


/*----------------------------------------------------------------------
  CheckSelectedElement verifie si l'element pEl constitue    
  l'une des extremite's de la selection courante dans le document 
  "document" et si oui definit une nouvelle selection, sans cet   
  element.                                                        
  ----------------------------------------------------------------------*/
void CheckSelectedElement (PtrElement pEl, Document document)
{
  PtrDocument         pDoc;
  PtrDocument         selDoc;
  PtrElement          firstSelection, lastSelection, selEl, previousSelection;
  int                 firstChar, lastChar;
  ThotBool            ok, changeSelection;

  pDoc = LoadedDocument[document - 1];
  ok = GetCurrentSelection (&selDoc, &firstSelection, &lastSelection,
                            &firstChar, &lastChar);
  if (ok && selDoc == pDoc)
    /* il y a une selection dans le document traite' */
    {
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
                firstSelection = NextInSelection (firstSelection,
                                                  lastSelection);
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
                if (ElemIsWithinSubtree (firstSelection, pEl) &&
                    ElemIsWithinSubtree (lastSelection, pEl))
                  /* la selection est entierement a l'interieur de l'element*/
                  /* on annule la selection courante */
                  ResetSelection (pDoc);
              }
          if (changeSelection)
            {
              if (firstChar > 1)
                TtaSelectString (document, (Element) firstSelection,
                                 firstChar, 0);
              else
                TtaSelectElement (document, (Element) firstSelection);
              if (lastSelection != firstSelection)
                TtaExtendSelection (document, (Element) lastSelection,
                                    lastChar);
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
                RemoveFromSelection (selEl);
                selEl = NULL;
              }
            else
              selEl = NextInSelection (selEl, lastSelection);
        }
    }
}

/*----------------------------------------------------------------------
  TtaSetFocus

  Choses the abstract box that should receive the characters typed by
  the user.
  ----------------------------------------------------------------------*/
void TtaSetFocus ()
{
  PtrAbstractBox      pAb;

  if (SelectedDocument && FirstSelectedElement && SelectedView != 0)
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
          /* a view of the main tree */
          pAb = SelectedDocument->DocViewRootAb[SelectedView - 1];
          ShowSelection (pAb, TRUE);
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
ThotBool GetCurrentSelection (PtrDocument *pDoc, PtrElement *firstEl,
                              PtrElement *lastEl, int *firstChar, int *lastChar)
{
  ThotBool            ret;
  PtrElement          pEl;

  *pDoc = NULL;
  if (SelectedDocument && FirstSelectedElement->ElStructSchema)
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
      if (pEl->ElTerminal &&
          (pEl->ElLeafType == LtPolyLine || pEl->ElLeafType == LtPath))
        {
          if (LastSelectedElement == FirstSelectedElement)
            {
              *firstChar = SelectedPointInPolyline;
              *lastChar = SelectedPointInPolyline;
            }
          else
            {
              *firstChar = 0;
              *lastChar = LastSelectedChar;
            }
        }
      else if (pEl->ElTerminal && pEl->ElLeafType == LtPicture)
        {
          *firstChar = SelectedPictureEdge;
          if (LastSelectedElement == FirstSelectedElement)
           *lastChar = 0;
          else
            *lastChar = LastSelectedChar;
        }
      else
        {
          *firstChar = FirstSelectedChar;
          if (LastSelectedChar == 0)
            *lastChar = 0;
          else if (SelPosition) 
            *lastChar = FirstSelectedChar - 1;
          else
            *lastChar = LastSelectedChar;
        }
    }
  else if (DocSelectedAttr)
    {
      /* there is a current selection */
      ret = TRUE;
      *pDoc = DocSelectedAttr;
      *firstEl = AbsBoxSelectedAttr->AbElement;
      *lastEl = *firstEl;
      *firstChar = 0;
      *lastChar = 0;
    }
  else
    {
      /* no current selection */
      ret = FALSE;
      *pDoc = NULL;
      *firstEl = NULL;
      *lastEl = NULL;
      *firstChar = 0;
      *lastChar = 0;
    }
  return ret;
}

/*----------------------------------------------------------------------
  GetActiveView

  Returns the active view:
  pDoc: the document to which the active view belongs,
  NULL if there is no selection.
  view: number of this view in the document
  ----------------------------------------------------------------------*/
void GetActiveView (PtrDocument *pDoc, int *view)
{
  PtrDocument         pSelDoc;
  PtrElement          firstSel, lastSel;
  int                 firstChar, lastChar;

  if (!GetCurrentSelection (&pSelDoc, &firstSel, &lastSel, &firstChar,
                            &lastChar))
    *pDoc = NULL;
  else
    {
      *pDoc = pSelDoc;
      *view = SelectedView;
    }
}

/*----------------------------------------------------------------------
  CancelSelection

  Cancels the current selection.
  ----------------------------------------------------------------------*/
void CancelSelection ()
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
      if (ThotLocalActions[T_chattr] != NULL)
        (*(Proc1)ThotLocalActions[T_chattr]) ((void *)pDoc);
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
}

/*----------------------------------------------------------------------
  ResetSelection

  If the current selection is in document pDoc, cancel this selection.
  ----------------------------------------------------------------------*/
void ResetSelection (PtrDocument pDoc)
{
  if (pDoc == SelectedDocument || pDoc == DocSelectedAttr)
    {
      CancelSelection ();
      SelectedView = 0;
      OldSelectedView = 0;
      OldDocSelectedView = NULL;
    }
  /* clean up the search and the checkspeller domains */
  ResetSearchInDocument (pDoc);
  ResetCheckInDocument (pDoc);
}

/*----------------------------------------------------------------------
  GetColHeadOfCell
  Return the column head element to which cell pCell is related.
  ----------------------------------------------------------------------*/
PtrElement GetColHeadOfCell (PtrElement pCell)
{
  int           attr;
  PtrElement    pColHead;
  PtrAttribute  pAttr;

  pColHead = NULL;
  /* get the type of the attribute that relate a cell with its column head */
  attr =  GetAttrWithException (ExcColRef, pCell->ElStructSchema);
  pAttr = pCell->ElFirstAttr;
  while (pAttr && !pColHead)
    if (pAttr->AeAttrSSchema == pCell->ElStructSchema &&
        pAttr->AeAttrNum == attr)
      /* That's the attribute referring to the column head  */
      {
        if (pAttr->AeAttrReference)
          if (pAttr->AeAttrReference->RdReferred)
            pColHead = pAttr->AeAttrReference->RdReferred->ReReferredElem;
      }
    else
      /* next attribute of the cell */
      pAttr = pAttr->AeNext;
  return pColHead;
}

/*----------------------------------------------------------------------
  GetCellInRow
  Return the cell in row pRow that is related to column head pColHead or
  a previous column if orPrevious is TRUE.
  If the cell comes from a previous column, *back gives the distance of
  that related column.
  ----------------------------------------------------------------------*/
PtrElement GetCellInRow (PtrElement pRow, PtrElement pColHead,
                         ThotBool orPrevious, int *back)
{
  PtrElement   pCell;
  ThotBool     found;

  *back = 0;
  if (pRow == NULL || pColHead == NULL)
    return NULL;
  pCell = NULL;
  if (!pRow->ElTerminal)
    pCell = pRow->ElFirstChild;
  found = FALSE;
  while (pCell && !found)
    {
      if (pColHead == GetColHeadOfCell (pCell))
        found = TRUE;
      else
        pCell = pCell->ElNext;
    }
  if (orPrevious && pColHead && pCell == NULL)
    {
      /* look for a cell in a previous column */ 
      pColHead = pColHead->ElPrevious;
      while (pColHead &&
             !TypeHasException (ExcIsColHead,
                                pColHead->ElTypeNumber,
                                pColHead->ElStructSchema))
        /* skip comments */
        pColHead = pColHead->ElPrevious;
      pCell = GetCellInRow (pRow, pColHead, orPrevious, back);
      (*back)++;
    }
  return pCell;
}

/*----------------------------------------------------------------------
  NextRowInTable
  Return the row that follows the current row pRow in the table pTable.
  ----------------------------------------------------------------------*/
PtrElement NextRowInTable (PtrElement pRow, PtrElement pTable)
{
  PtrElement pNextRow, pAsc;
  int        rowType;

  if (pRow)
    {
      /* next row */
      pNextRow = pRow->ElNext;
      while (pNextRow && !TypeHasException (ExcIsRow,
                                            pNextRow->ElTypeNumber,
                                            pNextRow->ElStructSchema))
        /* skip comments, PIs, etc. */
        pNextRow = pNextRow->ElNext;
      if (!pNextRow)
        {
          rowType = GetElemWithException (ExcIsRow, pRow->ElStructSchema);
          pAsc = pRow->ElParent;
          while (pAsc && pAsc->ElParent != pTable && !pAsc->ElNext)
            pAsc = pAsc->ElParent;
          while (!pNextRow && pAsc && pAsc->ElNext)
            {
              pAsc = pAsc->ElNext;
              if (pAsc)
                /* look for a row in another tbody or in tfoot */
                pNextRow = SearchTypedElementInSubtree (pAsc, rowType,
                                                        pRow->ElStructSchema);
              if (!pNextRow && !pAsc->ElNext)
                {
                  if (pAsc->ElParent != pTable)
                    pAsc = pAsc->ElParent;
                }
            }
        }
    }
  else
    pNextRow = NULL;
  return (pNextRow);
}

/*----------------------------------------------------------------------
  PreviousRowInTable
  Return the row before the current row pRow in the table pTable.
  ----------------------------------------------------------------------*/
static PtrElement PreviousRowInTable (PtrElement pRow, PtrElement pTable)
{
  PtrElement pPrevRow, pAsc, pRow1;

  if (pRow)
    {
      /* previous row */
      pPrevRow = pRow->ElPrevious;
      while (pPrevRow && !TypeHasException (ExcIsRow,
                                            pPrevRow->ElTypeNumber,
                                            pPrevRow->ElStructSchema))
        /* skip comments */
        pPrevRow = pPrevRow->ElPrevious;
      if (!pPrevRow && pRow->ElParent &&
          pRow->ElParent != pTable && pRow->ElParent->ElPrevious)
        {
          pAsc = pRow->ElParent->ElPrevious;
          while (!pPrevRow && pAsc)
            {
              /* look for a row in another tbody */
              if (pAsc->ElTerminal)
                pRow1 = NULL;
              else
                {
                  pRow1 = pAsc->ElFirstChild;
                  while (pRow1->ElNext)
                    pRow1 = pRow1->ElNext;
                }
              while (pRow1 && !TypeHasException (ExcIsRow,
                                                 pRow1->ElTypeNumber,
                                                 pRow1->ElStructSchema))
                /* skip comments */
                pRow1 = pRow1->ElPrevious;
              if (pRow1)
                pPrevRow = pRow1;
              else
                pAsc = pAsc->ElPrevious;
            }
        }
    }
  else
    pPrevRow = NULL;
  return (pPrevRow);
}

/*----------------------------------------------------------------------
  NextColumnInTable
  Return the column that follows the current column pCol in the table pTable.
  ----------------------------------------------------------------------*/
PtrElement NextColumnInTable (PtrElement pCol, PtrElement pTable)
{
  PtrElement pNextCol;

  if (pCol)
    {
      /* next column */
      pNextCol = pCol->ElNext;
      while (pNextCol &&
             !TypeHasException (ExcIsColHead, pNextCol->ElTypeNumber,
                                pNextCol->ElStructSchema))
        /* skip comments */
        pNextCol = pNextCol->ElNext;
    }
  else
    pNextCol = NULL;
  return (pNextCol);
}

/*----------------------------------------------------------------------
  PreviousColumnInTable
  Return the column that precedes the current column pCol in the same table.
  ----------------------------------------------------------------------*/
static PtrElement PreviousColumnInTable (PtrElement pCol)
{
  PtrElement pPrevCol;

  if (pCol)
    {
      /* next column */
      pPrevCol = pCol->ElPrevious;
      while (pPrevCol &&
             !TypeHasException (ExcIsColHead, pPrevCol->ElTypeNumber,
                                pPrevCol->ElStructSchema))
        /* skip comments */
        pPrevCol = pPrevCol->ElPrevious;
    }
  else
    pPrevCol = NULL;
  return (pPrevCol);
}

/*----------------------------------------------------------------------
  PreviousLeafInSelection
  Returns the element that is before element pEl in the selection order.
  ----------------------------------------------------------------------*/
static PtrElement PreviousLeafInSelection (PtrElement pEl)
{
  PtrElement          pCell, pRow, pTable, pEl1;
  int                 i, back;
  ThotBool            found;

  if (pEl != NULL)
    {
      if (SelContinue)
        {
          pEl1 = pEl;
          /* search the previous sibling in the abstract tree, or the
             previous element at an upper level */
          found = FALSE;
          while (pEl && !found)
            if (pEl->ElPrevious)
              /* this element has a previous sibling */
              {
                if (FirstSelectedColumn &&
                    TypeHasException (ExcIsCell, pEl->ElTypeNumber,
                                      pEl->ElStructSchema))
                  /* We are in column selection mode and it's a cell */
                  /* Don't take the sibling. stop */
                  pEl = NULL;
                else
                  found = TRUE;
              }
            else
              /* no previous sibling */
              {
                if (FirstSelectedColumn &&
                    TypeHasException (ExcIsCell, pEl->ElTypeNumber,
                                      pEl->ElStructSchema))
                  /* We are in column selection mode and it's a cell */
                  /* Don't take the sibling */
                  pEl = NULL;
                else
                  {
                    /* check the parent element */
                    pEl = pEl->ElParent;
                    if (pEl && FirstSelectedColumn &&
                        TypeHasException (ExcIsCell, pEl->ElTypeNumber,
                                          pEl->ElStructSchema))
                      /* We are in column selection mode and the parent */
                      /* is a cell. Don't take it */
                      pEl = NULL;
                  }
              }
          if (found)
            /* pEl has a previous sibling. Take it */
            pEl = pEl->ElPrevious;
          if (FirstSelectedColumn && !pEl)
            /* We are in column selection mode and we have not found yet */
            /* get the cell belonging to the column in the previous row */
            {
              /* get the table that contains the selection */
	      pTable = FirstSelectedColumn;
	      while (pTable &&
		     !TypeHasException (ExcIsTable,
					pTable->ElTypeNumber,
					pTable->ElStructSchema))
		pTable = pTable->ElParent;
              /* get the row that contains pEl */
              pRow = pEl1->ElParent;
              while (pRow && !TypeHasException (ExcIsRow,
                                                pRow->ElTypeNumber,
                                                pRow->ElStructSchema))
                pRow = pRow->ElParent;
              /* get the previous row that contains a cell in this
                 column (a colspanned cell from a previous column may
                 have "eaten" the cell in this column */
              while (pRow && !pEl)
                {
                  pRow = PreviousRowInTable (pRow, pTable);
                  if (pRow)
                    {
                      pCell = GetCellInRow (pRow, FirstSelectedColumn, FALSE,
                                            &back);
                      if (pCell)
                        /* there is a cell for that column in the row */
                        /* take that cell */
                        pEl = pCell;
                    }
                }
            }
        }
      else
        /* the current selection is not contiguous. Get the previous element
           in the SelectedElement table */
        {
          /* first, search pEl in the table */
          i = 0;
          found = FALSE;
          while (i < NSelectedElements && !found)
            if (SelectedElement[i] == pEl)
              found = TRUE;
            else
              i++;
          if (!found)
            pEl = NULL;
          else
            /* pEl is in the table */
            if (i > 0)
              /* it's not the first element in the table, return the
                 previous one*/
              pEl = SelectedElement[i-1];
            else
              /* it's the first element in the table, return NULL */
              pEl = NULL;
        }
    }
  if (pEl)
    pEl = LastLeaf (pEl);
  return pEl;
}

/*----------------------------------------------------------------------
  NextInSelection

  Returns the element in current selection that follows element pEl.
  Returns NULL if the last selected element (pLastEl) has previously been
  returned.
  ----------------------------------------------------------------------*/
PtrElement NextInSelection (PtrElement pEl, PtrElement pLastEl)
{
  PtrElement          pCell, pRow, pEl1, pTable, pCol;
  int                 i, back;
  ThotBool            found;

  if (pEl != NULL)
    {
      if (SelContinue)
        {
          if (pEl == pLastEl)
            /* the last selected element has been returned previously */
            pEl = NULL;
          else if (ElemIsWithinSubtree (pLastEl, pEl))
            /* the current element is an ancestor of the last selected
               element. It's finished */
            pEl = NULL;
          else
            {
              pEl1 = pEl;
              /* search the following element in the abstract tree, or the
                 following element at an upper level */
              found = FALSE;
              while (pEl && !found)
                if (pEl->ElNext)
                  /* this element has a following sibling */
                  {
                    if (FirstSelectedColumn &&
                        TypeHasException (ExcIsCell, pEl->ElTypeNumber,
                                          pEl->ElStructSchema))
                      /* We are in column selection mode and it's a cell */
                      /* Take the next sibling if it is before (or in) the
                         last selected column */
                      {
                        if (LastSelectedColumn == FirstSelectedColumn)
                          /* the whole selection is in a single column */
                          pEl = NULL;
                        else
                          if (!TypeHasException (ExcIsCell,
                                                 pEl->ElNext->ElTypeNumber,
                                                 pEl->ElNext->ElStructSchema))
                            /* the next sibling is not a cell */
                            pEl = NULL;
                          else
                            {
                              pCol = GetColHeadOfCell (pEl->ElNext);
                              if (pCol == LastSelectedColumn)
                                /* the next cell is in the last column selected */
                                found = TRUE;
                              else
                                if (ElemIsBefore (pCol, LastSelectedColumn))
                                  /* the next cell is before the last column
                                     selected */
                                  found = TRUE;
                                else
                                  pEl = NULL;
                            }
                      }
                    else
                      found = TRUE;
                  }
                else
                  /* no following sibling */
                  {
                    if (FirstSelectedColumn &&
                        TypeHasException (ExcIsCell, pEl->ElTypeNumber,
                                          pEl->ElStructSchema))
                      /* We are in column selection mode and it's a cell */
                      /* Don't take the next sibling */
                      pEl = NULL;
                    else
                      /* check the parent element */
                      pEl = pEl->ElParent;
                  }
              if (found)
                /* pEl has a following sibling. Take it */
                {
                  pEl = pEl->ElNext;
                  /* if this sibling contains the end of the selection,
                     take the lowest descendant that contains the last
                     selected element */
                  while (pEl != pLastEl && ElemIsWithinSubtree (pLastEl, pEl))
                    pEl = pEl->ElFirstChild;
                }
              if (FirstSelectedColumn && !pEl)
                /* We are in column selection mode and we have not found yet */
                /* get the cell belonging to the first selected column in the
                   next row */
                {
                  /* get the table that contains our column */
                  pTable = FirstSelectedColumn;
                  while (pTable &&
                         !TypeHasException (ExcIsTable,
                                            pTable->ElTypeNumber,
                                            pTable->ElStructSchema))
                    pTable = pTable->ElParent;
                  /* get the row that contains pEl */
                  pRow = pEl1->ElParent;
                  while (pRow && !TypeHasException (ExcIsRow,
                                                    pRow->ElTypeNumber,
                                                    pRow->ElStructSchema))
                    pRow = pRow->ElParent;
                  /* get the following row that contains a cell in this
                     column (a colspanned cell from a previous column may
                     have "eaten" the cell in this column */
                  while (pRow && !pEl)
                    {
                      pRow = NextRowInTable (pRow, pTable);
                      if (pRow)
                        {
                          pCell = GetCellInRow (pRow, FirstSelectedColumn,
                                                TRUE, &back);
                          if (!pCell)
                            /* this row has no cell in the first selected col.
                               nor before. */
                            {
                              if (LastSelectedColumn == FirstSelectedColumn)
                                /* the whole selection is in a single column */
                                pCell = NULL;
                              else
                                {
                                  /* take the first cell in the row */
                                  if (!pRow->ElTerminal)
                                    pCell = pRow->ElFirstChild;
                                  while (pCell && !TypeHasException (ExcIsCell,
                                                        pCell->ElTypeNumber,
                                                        pCell->ElStructSchema))
                                    pCell = pCell->ElNext;
                                  if (pCell)
                                    {
                                    pCol = GetColHeadOfCell (pCell);
                                    if (pCol != LastSelectedColumn)
                                      /* this cell is not in the last
                                         selected column */
                                      if (!ElemIsBefore (pCol, LastSelectedColumn))
                                        /* this cell is after the last
                                           selected column */
                                        pCell = NULL;
                                    }                                  
                                }
                            }
                          else
                            {
                              if (back > 0)
				{
				  /* pCell is an extended cell from a previous
				     column. Take next cell if it is not after
				     the last selected column */
				  if (LastSelectedColumn == FirstSelectedColumn)
				    /*the whole selection is in a single column*/
				    pCell = NULL;
				  else
				    {
				      pCell = pCell->ElNext;
				      while (pCell && !TypeHasException (ExcIsCell,
									 pCell->ElTypeNumber,
									 pCell->ElStructSchema))
					pCell = pCell->ElNext;
				      if (pCell)
					{
					  pCol = GetColHeadOfCell (pCell);
					  if (pCol != LastSelectedColumn)
					    /* the next cell is not in the last
					       selected column */
					    if (!ElemIsBefore (pCol, LastSelectedColumn))
					      /* the next cell is after the last
						 selected column */
					      pCell = NULL;
					}
				    }
				}
                            }
                          if (pCell)
			    {
			      /* the row contains a cell that is between the
				 selected columns */
			      if (ElemIsWithinSubtree (pLastEl, pCell))
				/* this cell contains the end of the selection */
				{
				  pEl = pCell;
				  while (pEl != pLastEl &&
					 ElemIsWithinSubtree (pLastEl, pEl))
				    pEl = pEl->ElFirstChild;
				}
			      else
				/* take that cell */
				pEl = pCell;
			    }
                        }
                    }
                }
            }
        }
      else
        /* the current selection is not contiguous. Get the next element from
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
              /* it's not the last element in the table, return the next one*/
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
    }
  return pEl;
}

/*----------------------------------------------------------------------
  HiddenType
  Returns TRUE if, according to its type, element pEl must be hidden to
  the user.
  ----------------------------------------------------------------------*/
ThotBool HiddenType (PtrElement pEl)
{
  PtrSRule            pSRule;
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
  else if (pEl->ElStructSchema)
    {
      pSRule = pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber - 1];
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
  GetAbsBoxSelectedAttr

  returns the abstract box that displays the attribute in the view view.
  ----------------------------------------------------------------------*/
static PtrAbstractBox GetAbsBoxSelectedAttr (PtrAttribute pAttr,
                                             PtrElement pEl, int view)
{
  PtrAbstractBox      pAbView, pAb, pAbMain;

  if (AbsBoxSelectedAttr && view == AbsBoxSelectedAttr->AbDocView)
    pAbView = AbsBoxSelectedAttr;
  else
    {
      /* search the corresponding abstract box in the view: pAbView */
      pAbView = NULL;
      if (pEl == NULL)
        pAb = NULL;
      else
        pAb = pEl->ElAbstractBox[view - 1];
      while (pAbView == NULL && pAb != NULL &&
             pAb->AbElement == pEl)
        {
          if (pAb->AbPresentationBox)
            /* pAb is a presentation abstract box for the element */
            /* to which the attribute is attached */
            {
              if (pAb->AbCanBeModified &&
                  pAb->AbCreatorAttr == pAttr)
                pAbView = pAb;
            }
          else
            /* pAb is the main abstract box for the element to which */
            /* the attribute is attached */
            {
              pAbMain = pAb;
              pAb = pAb->AbFirstEnclosed;
              while (pAbView == NULL && pAb != NULL)
                {
                  if (pAb->AbElement == pEl)
                    if (pAb->AbPresentationBox && pAb->AbCanBeModified)
                      if (pAb->AbCreatorAttr == pAttr)
                        pAbView = pAb;
                  pAb = pAb->AbNext;
                }
              if (pAbView == NULL)
                pAb = pAbMain;
            }
          if (pAbView == NULL && pAb != NULL)
            pAb = pAb->AbNext;
        }
    }
  return pAbView;
}


/*----------------------------------------------------------------------
  HighlightAttrSelection
  
  Highlight the selected string of an attribute in all views.
  ----------------------------------------------------------------------*/
void HighlightAttrSelection (PtrDocument pDoc, PtrElement pEl,
                             PtrAttribute pAttr, int firstChar, int lastChar)
{
  PtrAbstractBox      pAb;
  int                 view, lastView, frame;

  if (pDoc)
    {
      /* first, choose the views to be processed */
      /* all views should be considered */
      lastView = MAX_VIEW_DOC;
      CancelSelection ();
      DocSelectedAttr = pDoc;
      AbsBoxSelectedAttr = NULL;
      FirstSelectedCharInAttr = firstChar;
      LastSelectedCharInAttr = lastChar;
      FixedChar = 0;

      /* process all chosen views */
      for (view = 0; view < lastView; view++)
        {
          /* get the number of the window (frame) where the view is displayed */
          if (pDoc->DocView[view].DvPSchemaView > 0)
            frame = pDoc->DocViewFrame[view];
          else
            /* this view is not open */
            frame = 0;
          /* if the view is open, process all abstract boxes of the
             selected element in this view */
          if (frame > 0)
            {
              /* switch selection off in this view */
              pAb = GetAbsBoxSelectedAttr (pAttr, pEl, view + 1);
              if (pAb)
                {
                  if (AbsBoxSelectedAttr == NULL)
                    AbsBoxSelectedAttr = pAb;
                  pAb->AbSelected = TRUE;
                  SelPosition = (firstChar >= lastChar);
                  InsertViewSelMarks (frame, pAb, firstChar,
                                      lastChar, TRUE, TRUE, TRUE);
                  ShowSelectedBox (frame, TRUE);
                  /* display the new selection */
                  DisplayFrame (frame);
                }
            }
        }
      if (AbsBoxSelectedAttr == NULL)
        {
          /* attribute not found */
          DocSelectedAttr = NULL;
          FirstSelectedCharInAttr = 0;
          LastSelectedCharInAttr = 0;
          SelectElement (pDoc, pEl, TRUE, FALSE, TRUE);
        }
    }
}

/*----------------------------------------------------------------------
  HighlightSelection
  
  Highlight all selected elements in all views.
  If showBegin is TRUE, scroll the document to show the beginning of
  the first selected element.
  ----------------------------------------------------------------------*/
void HighlightSelection (ThotBool showBegin, ThotBool clearOldSel)
{
  int                 view, lastView, frame;

  if (SelectedDocument)
    {
      /* first, choose the views to be processed */
      /* all views should be considered */
      lastView = MAX_VIEW_DOC;
      /* process all chosen views */
      for (view = 0; view < lastView; view++)
        {
          /* get the number of the window (frame) where the view is displayed */
          if (SelectedDocument->DocView[view].DvPSchemaView > 0)
            frame = SelectedDocument->DocViewFrame[view];
          else
            /* this view is not open */
            frame = 0;
          /* if the view is open, process all abstract boxes of the
             selected element in this view */
          if (frame > 0)
            {
              /* switch selection off in this view */
              if (clearOldSel)
                ClearViewSelection (frame);
              ShowSelection (SelectedDocument->DocViewRootAb[view], showBegin);
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
static void         SetActiveView (int exceptView)
{
  int                 view;
  ThotBool            stop;

  /* there is no longer any active view */
  SelectedView = 0;
  /* is the former active view OK? */
  if (OldDocSelectedView == SelectedDocument &&
      /* the former active view displays the selected document */
      OldSelectedView != 0 && OldSelectedView != exceptView &&
      FirstSelectedElement->ElAbstractBox[OldSelectedView - 1] != NULL)
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
          if (view != exceptView &&
              FirstSelectedElement->ElAbstractBox[view - 1] != NULL)
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
  view is the number of the closed view
  ----------------------------------------------------------------------*/
void    DeactivateView (PtrDocument pDoc, int view)
{

  if (pDoc == SelectedDocument)
    /* the current selection is in that document */
    {
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
}


/*----------------------------------------------------------------------
  WithinAbsBox

  return TRUE if abstract box pAB is in the subtree of abstract box pRootAb.
  ----------------------------------------------------------------------*/
static ThotBool WithinAbsBox (PtrAbstractBox pAb, PtrAbstractBox pRootAb)
{
  ThotBool            ret;

  ret = FALSE;
  do
    if (pAb == pRootAb)
      ret = TRUE;
    else if (pAb != NULL)
      pAb = pAb->AbEnclosing;
  while (!ret && pAb != NULL);
  return ret;
}


/*----------------------------------------------------------------------
  ShowSelection

  Highlight all character strings and boxes that are part of the
  current selection and belong to the subtree of pRootAb.
  If showBegin is TRUE, the beginning of the selection should be made
  visible to the user.
  ----------------------------------------------------------------------*/
void ShowSelection (PtrAbstractBox pRootAb, ThotBool showBegin)
{
  PtrElement          pEl, pNextEl;
  PtrAbstractBox      pAb, pNextAb, pSelAb[MAX_TRANSMIT];
  int                 view, frame = 0, firstChar, lastChar;
  int                 depth;
  ThotBool            selBegin, selEnd, active, unique, stop;

  if (pRootAb == NULL)
    return;
  view = pRootAb->AbDocView;
  if (SelectedDocument && FirstSelectedElement && pRootAb)
    {
      /* there is a current selection */
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
      while (pAb == NULL && pNextEl)
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
      /* switch current selection off in the view */
      /* if (pAb)
         ClearViewSelection (frame); */
      /* by default the selection applies to selectected abstract box */
      depth = 0;
      /* process all abstract boxes of all elements belonging to the */
      /* current selection, starting with the first visible in the view */
      while (pAb)
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

          /*
            For ghost elements or elements with the exception
            HighlightChildren, the selection is transmitted to children
          */
          while (pAb && pAb->AbFirstEnclosed &&
                 FrameTable[frame].FrView == 1 &&
                 TypeHasException (ExcHighlightChildren,
                                   pAb->AbElement->ElTypeNumber,
                                   pAb->AbElement->ElStructSchema))
            {
              /* select children intead of the current abstract box */
              if (depth < MAX_TRANSMIT)
                pSelAb[depth++] = pAb;
              pAb = pAb->AbFirstEnclosed;
              pEl = pAb->AbElement;
            }

          /* is that the last visible abstract box of the selection? */
          pNextAb = pAb->AbNext;
          while (pNextAb == NULL && depth > 0)
            {
              /* all children of the ghost element are now highlighted */ 
              pNextAb = pSelAb[--depth]->AbNext;
              pEl = pSelAb[depth]->AbElement;
            }
          if (depth == 0 && pNextAb && pNextAb->AbElement != pEl)
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
                  pNextEl = NextInSelection (pNextEl, LastSelectedElement);
                  if (pNextEl == NULL)
                    stop = TRUE;
                  else if (pNextEl->ElAbstractBox[view - 1] != NULL)
                    stop = TRUE;
                }
              while (!stop);
              if (pNextEl != NULL &&
                  /* there is a next element in the selection having an */
                  /* abstract box in the view */
                  WithinAbsBox (pNextEl->ElAbstractBox[view - 1], pRootAb))
                /* this abstract box is in the subtree */
                /* It's the next to be processed */
                {
                  pNextAb = pNextEl->ElAbstractBox[view - 1];
                  pEl = pNextEl;
                }
              else
                selEnd = TRUE;
            }
          else
            selEnd = TRUE;
          /* indicate that selected element to the display module */
          pAb->AbSelected = TRUE;
          if (selBegin || selEnd)
            {
              InsertViewSelMarks (frame, pAb, firstChar, lastChar,
                                  selBegin, selEnd, unique);
              /* active view */
              active = view == SelectedView;
              if (showBegin &&
                  (SelectedDocument->DocView[view - 1].DvSync || active))
                ShowSelectedBox (frame, active);

              selBegin = FALSE;
              showBegin = FALSE;
            }
          else if (pAb->AbBox)
            DrawBoxSelection (frame, pAb->AbBox);
	    
          /* next abstract box to be highlighted */
          pAb = pNextAb;
          selEnd = (pNextEl == LastSelectedElement &&
                    (pAb == NULL || pAb->AbNext == NULL));
        }
      /* display the new selection */
      DisplayFrame (frame);
    }
  else if (DocSelectedAttr && AbsBoxSelectedAttr)
    /* the current selection is within an attribute value */
    {
      frame = DocSelectedAttr->DocViewFrame[view - 1];
      pAb = GetAbsBoxSelectedAttr (AbsBoxSelectedAttr->AbCreatorAttr,
                                   AbsBoxSelectedAttr->AbElement, view);
      if (pAb)
        {
          pAb->AbSelected = TRUE;
          InsertViewSelMarks (frame, pAb, FirstSelectedCharInAttr,
                              LastSelectedCharInAttr, TRUE, TRUE, TRUE);
          ShowSelectedBox (frame, TRUE);
          /* display the new selection */
          DisplayFrame (frame);
        }
    }
}

/*----------------------------------------------------------------------
  DisplaySel
  Highlight the selected element pEl in view view.
  ----------------------------------------------------------------------*/
static void DisplaySel (PtrElement pEl, int view, int frame, ThotBool *abExist)
{
  PtrAbstractBox      pAb, pNextAb, pSelAb[MAX_TRANSMIT];
  PtrElement          parent, first, last;
  int                 firstChar, lastChar;
  int                 depth;
  ThotBool            selBegin, selEnd, partialSel;
  ThotBool            unique, active, show, saveSelPosition;

  if (TtaGetDisplayMode (FrameTable[frame].FrDoc) != DisplayImmediately)
    show = FALSE;
  else
    show = TRUE;
  unique = FirstSelectedElement == LastSelectedElement;
  first = FirstSelectedElement;
  last = LastSelectedElement;
  firstChar = FirstSelectedChar;
  lastChar = LastSelectedChar;
  saveSelPosition = SelPosition; // could be altered
  if (FrameTable[view].FrView != 1 && // not formatted view
      first == last && unique &&
      pEl->ElTerminal && pEl->ElLeafType == LtPicture)
    {
      parent = pEl->ElParent;
      if (parent &&
          TypeHasException (ExcIsImg, parent->ElTypeNumber,
                            parent->ElStructSchema))
        {
          // higlight the IMG element instead of the PICTURE
          pEl = parent;
          first = last = pEl;
          SelPosition = FALSE;
        }
      
    }
  pAb = pEl->ElAbstractBox[view - 1];
  /* first abstract box of elemenebt in the view */
  if (pAb)
    {
      partialSel = FALSE;
      if (pEl == first && pEl->ElTerminal)
        /* it's the first element in the current selection */
        {
          if (pEl->ElLeafType == LtText &&
              firstChar > 1 && pEl->ElTextLength > 0)
            /* the text leaf is partly selected */
            partialSel = TRUE;
          else if (pEl->ElLeafType == LtSymbol &&
                   firstChar >= 1 && pEl->ElVolume > 0)
            /* the text leaf is partly selected */
            partialSel = TRUE;
          else if ((pEl->ElLeafType == LtPolyLine ||
                    pEl->ElLeafType == LtPath ||
                    pEl->ElLeafType == LtGraphics) &&
                   SelectedPointInPolyline > 0)
            partialSel = TRUE;
        }

      if (partialSel)
        /* skip presentation abstract boxes created before the main box */
        {
          while (pAb->AbPresentationBox && pAb->AbNext != NULL)
            pAb = pAb->AbNext;
          if (pAb != NULL && pAb->AbElement != pEl)
            pAb = NULL;
        }
      /* the element has at least one abstract box in the view */
      *abExist = pAb != NULL;
    }

  /* handles all abstract box of the element in the view */
  active = view == SelectedView;
  selBegin =  TRUE;
  depth = 0;
  while (pAb != NULL)
    {
      /* search the next selected element */
      partialSel = FALSE;
      if (pEl == last && pEl->ElTerminal)
        /* that's the last element in the current selection */
        {
          if (pEl->ElLeafType == LtText &&
              LastSelectedChar < pEl->ElTextLength &&
              pEl->ElTextLength > 0 && LastSelectedChar > 0)
            /* that text leaf is partly selected */
            partialSel = TRUE;
          else if (pEl->ElLeafType == LtSymbol &&
                   firstChar >= 1 && pEl->ElVolume > 0)
            /* the text leaf is partly selected */
            partialSel = TRUE;
          else if ((pEl->ElLeafType == LtPolyLine ||
                    pEl->ElLeafType == LtPath ||
                    pEl->ElLeafType == LtGraphics) &&
                   SelectedPointInPolyline > 0)
            partialSel = TRUE;
        }
      /*
        For ghost elements or elements with the exception
        HighlightChildren, the selection is transmitted to children
      */
      while (pAb && pAb->AbFirstEnclosed &&
              FrameTable[frame].FrView == 1 &&
             TypeHasException (ExcHighlightChildren,
                               pAb->AbElement->ElTypeNumber,
                               pAb->AbElement->ElStructSchema))
        {
          /* select children instead of the current abstract box */
          if (depth < MAX_TRANSMIT)
            pSelAb[depth++] = pAb;
          pAb = pAb->AbFirstEnclosed;
          pEl = pAb->AbElement;
        }
     
      if (partialSel && !pAb->AbPresentationBox)
        pNextAb = NULL;
      else
        pNextAb = pAb->AbNext;
      while (pNextAb == NULL && depth > 0)
        {
          /* all children of the ghost element are now highlighted */ 
          pNextAb = pSelAb[--depth]->AbNext;
          pEl = pSelAb[depth]->AbElement;
        }
      if (depth == 0 && pNextAb != NULL && pNextAb->AbElement != pEl)
        /* the next abstract box does not belong to the element */
        pNextAb = NULL;
      selEnd = pNextAb == NULL;

      /* indicate that this abstract box is selected to the display module */
     if (pEl == first)
        {
          if (pEl->ElLeafType == LtText || pEl->ElLeafType == LtSymbol)
            firstChar = FirstSelectedChar;
          else if (pEl->ElLeafType == LtPolyLine ||
                   pEl->ElLeafType == LtPath ||
                   pEl->ElLeafType == LtGraphics)
            firstChar = SelectedPointInPolyline;
          else if (pEl->ElLeafType == LtPicture)
            firstChar = SelectedPictureEdge;
          else
            firstChar = 0;
        }
      else
        firstChar = 0;
      
      if (pAb->AbElement == last)
        {
          if (pEl->ElLeafType == LtText || pEl->ElLeafType == LtSymbol)
            lastChar = LastSelectedChar;
          else if (pEl->ElLeafType == LtPolyLine ||
                   pEl->ElLeafType == LtPath ||
                   pEl->ElLeafType == LtGraphics)
            lastChar = SelectedPointInPolyline;
          else if (pEl->ElLeafType == LtPicture)
            lastChar = SelectedPictureEdge;
          else
            lastChar = 0;
        }
      else
        lastChar = 0;

      /* this abstract box is selected */
      pAb->AbSelected = TRUE;
      if (selBegin || selEnd)
        {
          InsertViewSelMarks (frame, pAb, firstChar, lastChar,
                              selBegin, selEnd, unique);
          /* should this abstract box be made visible to the user? (scroll) */
          if (show &&
              (SelectedDocument->DocView[view - 1].DvSync || active))
            ShowSelectedBox (frame, active);
          selBegin = FALSE;
        }
      else if (pAb->AbBox)
        DrawBoxSelection (frame, pAb->AbBox);

      /* next abstract box of the element */
      pAb = pNextAb;
    }
  /* display the new selection */
  DisplayFrame (frame);
#ifdef _GL
  /* not sure it's the right place */
  //GL_Swap (frame);
#endif /* _GL */
  SelPosition = saveSelPosition; // restore original value
}


/*----------------------------------------------------------------------
  SelectAbsBoxes
  Select in all views all abstract boxes of element pEl.
  If no abstract exists currently for this element in any view, try
  to to open a view where this element would have an abstract box, but
  only if createView is TRUE.
  ----------------------------------------------------------------------*/
static ThotBool SelectAbsBoxes (PtrElement pEl, ThotBool createView)
{
  Document            doc;
  int                 view, lastView, frame, run;
  ThotBool            abExist, done;
  NotifyDialog        notifyDoc;

  /* there is not any abstract box yet */
  abExist = FALSE;
  if (pEl && pEl->ElStructSchema)
    {
      lastView = MAX_VIEW_DOC;
      /* views are scanned twice. In the first run, existing abstract */
      /* boxes are selected. In the second run, new abstract boxes are */
      /* created if necessary */
      doc = IdentDocument (SelectedDocument);
      if (documentDisplayMode[doc - 1] != NoComputedDisplay)
        {
          for (run = 1; run <= 2; run++)
            for (view = 0; view < lastView; view++)
              {
                /* frame: window where the selection will be shown */
                if (SelectedDocument->DocView[view].DvPSchemaView > 0)
                  frame = SelectedDocument->DocViewFrame[view];
                else
                  frame = 0;
                if (frame != 0)
                  {
                    if (run == 1)
                      ClearViewSelection (frame);
                    done = FALSE;
                    if (run == 2)
                      /* second run. Create missing abstract boxes */
                      {
                        if (pEl->ElAbstractBox[view] != NULL)
                          {
                            done = TRUE;
                            /* the element has at least one abstract box */
                            abExist = TRUE;
                          }
                        else
                          /* create the abstract boxes for view */
                          /* if this view is synchronized */
                          if (SelectedDocument->DocView[view].DvSync &&
                              /* if pEl is a page break, don't call CheckAbsBox*/
                              /* if this break is not for the right view */
                              (pEl->ElTypeNumber != PageBreak + 1 ||
                               pEl->ElViewPSchema == SelectedDocument->DocView[view].DvPSchemaView))
                            {
                              CheckAbsBox (pEl, view + 1, SelectedDocument,
                                           FALSE, TRUE);
                              if (SelectedView == 0 && pEl->ElAbstractBox[view])
                                SetActiveView (0);
                            }
                      }
                    if (!done && pEl)
                      {
                        while (pEl && pEl->ElParent &&
                               (pEl->ElIsCopy ||
                                TypeHasException (ExcSelectParent, pEl->ElTypeNumber,
                                                  pEl->ElStructSchema)))
                          pEl = pEl->ElParent;
                        // when the selection comes from a click on children
                        if (TypeHasException (ExcIsBreak, pEl->ElTypeNumber,
                                              pEl->ElStructSchema))
                          SelPosition = TRUE;
                        DisplaySel (pEl, view + 1, frame, &abExist);
                      }
                  }
              }

          if (!abExist && createView && pEl->ElTerminal &&
              pEl->ElStructSchema && pEl->ElStructSchema->SsName &&
              !strcmp (pEl->ElStructSchema->SsName, "HTML"))
            {
              /* send an event to the application to open another view*/
              notifyDoc.event = TteViewOpen;
              notifyDoc.document = doc;
              notifyDoc.view = createView;
              if (CallEventType ((NotifyEvent *) & notifyDoc, TRUE))
                {
                  /* application has created another view */
                  abExist = SelectAbsBoxes (pEl, FALSE);
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
void HighlightVisibleAncestor (PtrElement pEl)
{
  PtrElement          pAncest;
  int                 view, lastView, frame;
  ThotBool            found, abExist;

  view = 0;
  if (pEl != NULL)
    {
      lastView = MAX_VIEW_DOC;
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
      if (pAncest != NULL && SelectedDocument->DocView[view - 1].DvPSchemaView > 0)
        /* there is an ancestor with an abstract box */
        {
          frame = SelectedDocument->DocViewFrame[view - 1];
          /* highlight the ancestor found */
          DisplaySel (pAncest, view, frame, &abExist);
        }
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void DecorationAfterSeletion (ThotBool withPath)
{
#ifdef _WX
  AmayaParams         p;
  PtrAbstractBox      pAb = NULL;
  PtrElement          pEl;
  int                 doc;
  int                 size, family, val;
  TypeUnit            unit;

  if (SelectedDocument == NULL && DocSelectedAttr == NULL)
    return;

  if (SelectedDocument)
    {
      doc = IdentDocument (SelectedDocument);
      pEl = FirstSelectedElement;
    }
  else
    {
      doc = IdentDocument (DocSelectedAttr);
      pEl = AbsBoxSelectedAttr->AbElement;
    }

  if (pEl && pEl->ElStructSchema &&
      strcmp (pEl->ElStructSchema->SsName, "TextFile"))
    {
      // update the current font
      TtaGiveBoxFontInfo ((Element)pEl, doc, 1,
                          &size, &unit, &family);
      pAb = AbsBoxOfEl (pEl, 1);
     if (size > 0 && pAb)
        {
          if (unit == UnPoint)
            Current_FontSize = size;
          else
            {
              if (unit != UnPixel)
                val = PixelValue (size, unit, pAb, 0);
              else
                val = size;
              Current_FontSize = LogicalValue (size, UnPoint, pAb, 0);
            }
        }
      Current_FontFamily = family;
      p.param1 = doc;
      TtaSendDataToPanel (WXAMAYA_PANEL_STYLE, p );
    }
  if (withPath)
    // update the status bar
    TtaSetStatusSelectedElement (doc, 1, (Element) pEl);
#endif /* _WX */
}

/*----------------------------------------------------------------------
  SelectStringInAttr
  The new current selection is now the character string contained
  in the text buffer of abstract box pAb, starting at rank firstChar
  and ending at rank last Char.
  pAb is a presentation abstract box that contains the value of a
  numerical or textual attribute.
  The parameter string is TRUE when one or more characters are selected.
  ----------------------------------------------------------------------*/
void SelectStringInAttr (PtrDocument pDoc, PtrAbstractBox pAb, int firstChar,
                         int lastChar, ThotBool string)
{
  PtrElement          pEl;
  PtrAbstractBox      pAbView;
  int                 frame, view;

  if (pAb == NULL || pDoc == NULL)
    return;
  pEl = pAb->AbElement;
  if (pEl && pEl->ElIsCopy)
    /* the string to be selected is in a copy element */
    /* Select the whole element. SelectElement will select the first */
    /* ancestor that is not a copy */
    SelectElement (pDoc, pEl, TRUE, TRUE, TRUE);
  else
    {
      DocSelectedAttr = pDoc;
      AbsBoxSelectedAttr = pAb;
      FirstSelectedCharInAttr = firstChar;
      LastSelectedCharInAttr = lastChar;
      SelPosition = !string;
      if (SelPosition)
        FixedChar = firstChar;
      /* highlight the new selection in all views */
      for (view = 0; view < MAX_VIEW_DOC; view++)
        {
          /* frame: window where the view is displayed */
          if (pDoc->DocView[view].DvPSchemaView > 0)
            frame = pDoc->DocViewFrame[view];
          else
            frame = 0;	/* vue non creee */
          /* if the view exists, highlight selection in that view */
          if (frame > 0)
            {
              /* search in the view the presentation abstract box that */
              /* contains an attribute value */
              pAbView = GetAbsBoxSelectedAttr (AbsBoxSelectedAttr->AbCreatorAttr,
                                               AbsBoxSelectedAttr->AbElement, view + 1);
              /* switch the former selection off in that view */
              ClearViewSelection (frame);
              if (pAbView)
                {
                  /* highlight the new selection */
                  pAb->AbSelected = TRUE;
                  InsertViewSelMarks (frame, pAbView, firstChar,
                                      lastChar, TRUE, TRUE, TRUE);
                  ShowSelectedBox (frame, TRUE);
                  DisplayFrame (frame); 
                }
            }
        }
      PrepareSelectionMenu ();
      if (ThotLocalActions[T_chattr] != NULL)
        (*(Proc1)ThotLocalActions[T_chattr]) ((void *)pDoc);
#ifdef _WX
          // update the status bar and style panel
          DecorationAfterSeletion (TRUE);
#endif /* _WX */
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
static void SelectStringOrPosition (PtrDocument pDoc, PtrElement pEl,
                                    int firstChar, int lastChar, ThotBool string,
                                    ThotBool withDecoration)
{
  PtrElement          pAncest;
  ThotBool            elVisible;
  PtrElement          oldFirstSelEl;
  int                 oldFirstSelChar;
  int                 i;
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

  if (pEl && pDoc)
    /* ignore exception NoSelect */
    {
      if (pEl->ElIsCopy || holophrast)
        /* the string to be selected is in a copy or holophrasted element */
        /* select that element: SelectElement will select the first */
        /* that can be selected */
        SelectElement (pDoc, pEl, TRUE, TRUE, TRUE);
      else
        {
          oldFirstSelEl = FirstSelectedElement;
          oldFirstSelChar = FirstSelectedChar;
          if (string && pEl->ElLeafType == LtText)
            {
              if (lastChar == 0)
                lastChar = pEl->ElTextLength;
              else if (lastChar < firstChar)
                {
                  i = lastChar;
                  lastChar = firstChar;
                  firstChar = i;
                }
            }
          /* Is the new selection in the same tree as the former selection */
          FirstSelectedElement = pEl;
          SelPosition = !string;
          if (pDoc != SelectedDocument)
            {
              CancelSelection ();
              SelectedDocument = pDoc;
              FirstSelectedElement = pEl;
              SetActiveView (0);
            }

          LastSelectedElement = FirstSelectedElement;
          if (pEl->ElLeafType == LtPolyLine ||
              pEl->ElLeafType == LtPath ||
              pEl->ElLeafType == LtGraphics)
            {
	      SelectedPointInPolyline = firstChar;
              FirstSelectedChar = 0;
              LastSelectedChar = 0;
            }
          else if (pEl->ElLeafType == LtPicture)
            {
              if (pEl->ElStructSchema && pEl->ElStructSchema->SsName &&
                  !strcmp (pEl->ElStructSchema->SsName, "SVG"))
                {
                  // force the selection of the whole picture
                  SelectedPictureEdge = 0;
                  SelPosition = FALSE;
                }
              else
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
          FirstSelectedColumn = NULL;
          LastSelectedColumn = NULL;
          WholeColumnSelected = FALSE;
          NSelectedElements = 0;
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
              if (ThotLocalActions[T_chattr] != NULL)
                (*(Proc1)ThotLocalActions[T_chattr]) ((void *)pDoc);
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
                  (*(Proc1)ThotLocalActions[T_chsplit]) ((void *)pDoc);
#ifdef _WX
           if ( !TypeHasException (ExcHidden, pEl->ElTypeNumber, pEl->ElStructSchema))
             // update the status bar and style panel
             if (withDecoration)
               DecorationAfterSeletion (TRUE);
#endif /* _WX */
        }
    }
}


/*----------------------------------------------------------------------
  MoveCaret
  Set the current selection to the position before the character of rank
  firstChar in the text element pEl.
  If pEl is a polyline element, the vertex of rank firstChar is selected.
  ----------------------------------------------------------------------*/
void MoveCaret (PtrDocument pDoc, PtrElement pEl, int firstChar)
{
  SelectStringOrPosition (pDoc, pEl, firstChar, firstChar, FALSE, FALSE);
}

/*----------------------------------------------------------------------
  SelectString
  Set the current selection to the string beginning at position firstChar
  and ending at position lastChar in the text element pEl.
  If pEl is a polyline, the vertex of rank firstChar is selected.
  ----------------------------------------------------------------------*/
void SelectString (PtrDocument pDoc, PtrElement pEl, int firstChar, int lastChar)
{
  ThotBool            string;

  if (pDoc == NULL || pEl == NULL)
    return;
  if (pEl->ElStructSchema == NULL)
    return;
  /* by default it's a string, not a position within a string */
  string = TRUE;
  if (firstChar > lastChar)
    /* it's a position */
    string = FALSE;
  SelectStringOrPosition (pDoc, pEl, firstChar, lastChar, string, TRUE);
}

/*----------------------------------------------------------------------
  SelectElement
  Set the current selection to element pEl in document pDoc.
  If this element is not supposed to be shown to the user, its first or
  last child is selected instead, depending on parameter begin, except
  when check is FALSE.
  When withPath is TRUE, the status bar is updated.
  ----------------------------------------------------------------------*/
void SelectElement (PtrDocument pDoc, PtrElement pEl, ThotBool begin,
                    ThotBool check, ThotBool withPath)
{
  PtrElement          pAncest, pE;
  ThotBool            stop, elVisible;

  if (pEl && pDoc && pEl->ElStructSchema &&
      pEl != pDoc->DocDocElement) /* do not select the Document element */
    {
      if (check)
        {
          stop = FALSE;
          while (!stop)
            if (!TypeHasException (ExcHidden, pEl->ElTypeNumber, pEl->ElStructSchema))
              /* exception Hidden is not associated with this element type */
              {
                stop = TRUE;
                if (!pEl->ElTerminal && pEl->ElFirstChild &&
                    /* get the structure rule defining the element type */
                    pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber - 1]->SrConstruct == CsChoice)
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
      /* highest level protected ancestor. */
      /* If it has exception SelectParent, select the first ancestor that */
      /* do not have that exception */
      while (pEl && pEl->ElParent &&
             (pEl->ElIsCopy ||
              TypeHasException (ExcSelectParent, pEl->ElTypeNumber,
                                pEl->ElStructSchema)))
        pEl = pEl->ElParent;
 
      /* If the element is in a holophrasted tree, selected the */
      /* holphrasted ancestor */
      pAncest = pEl;
      do
        {
          pAncest = pAncest->ElParent;
          if (pAncest != NULL && pAncest->ElHolophrast)
            {
              pEl = pAncest;
              pAncest = NULL;
            }
        }
      while (pAncest);

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
      /* If the new selection is in the same tree, SelectAbsBoxes will */
      /* switch the previous selection off */
      /* ignore exception NoSelect */
      SelContinue = TRUE;
      FirstSelectedColumn = NULL;
      LastSelectedColumn = NULL;
      WholeColumnSelected = FALSE;
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
      if (pEl->ElTerminal && pEl->ElLeafType == LtPicture &&
          pEl->ElStructSchema && pEl->ElStructSchema->SsName &&
          !strcmp (pEl->ElStructSchema->SsName, "SVG"))
        // force the selection of the whole picture
        SelPosition = FALSE;
      else if ((pEl->ElTerminal &&
                (pEl->ElVolume == 0 || pEl->ElLeafType == LtPicture)) ||
               (!pEl->ElTerminal && pEl->ElFirstChild == NULL))
        {
          SelPosition = TRUE;
          if (pEl->ElTerminal && pEl->ElLeafType == LtPicture)
            {
              if (begin)
                SelectedPictureEdge = 0;
              else
                SelectedPictureEdge = 1;
            }
        }
      else
        SelPosition = FALSE;
      elVisible = SelectAbsBoxes (FirstSelectedElement, TRUE);
      if (!elVisible)
        /* the selection is not visible, highlight the first visible */
        /* ancestor */
        HighlightVisibleAncestor (FirstSelectedElement);
      /* if the selected element is a paired element, select the other */
      /* element of the pair too */
      if (FirstSelectedElement->ElStructSchema->SsRule->SrElem[FirstSelectedElement->ElTypeNumber - 1]->SrConstruct ==
          CsPairedElement)
        {
          AddInSelection (GetOtherPairedElement (FirstSelectedElement), TRUE);
          if (!FirstSelectedElement->ElStructSchema->
              SsRule->SrElem[FirstSelectedElement->ElTypeNumber - 1]->SrFirstOfPair)
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
      
      /* update all the menus that depend on the current selection */
      if (SelectionUpdatesMenus)
        {
          PrepareSelectionMenu ();
          if (ThotLocalActions[T_chattr] != NULL)
            (*(Proc1)ThotLocalActions[T_chattr]) ((void *)pDoc);
        }
#ifdef _WX
      DecorationAfterSeletion (withPath);
#endif /* _WX */
    }
}

/*----------------------------------------------------------------------
  MakeSelectionRectangle
  When selection is in a table, make sure that all selected cells
  (with their horizontal and vertical extension) take the shape
  of a rectangle.
  ----------------------------------------------------------------------*/
static void MakeSelectionRectangle ()
{
  PtrElement          pTable, pCell, pRow, pCurRow, pLastRow, pFirstRow, pR,
                      rowOfCell, pCol, pCurCol, pC;
  PtrAttribute        attrSpan;
  int                 back, span, i;
  ThotBool            change, checkCell;

  if (!FirstSelectedColumn || !LastSelectedColumn ||
      !FirstSelectedElement || !LastSelectedElement)
    return;
  /* get the row that contains the last selected cell */
  pLastRow = LastSelectedElement->ElParent;
  while (pLastRow && !TypeHasException (ExcIsRow, pLastRow->ElTypeNumber,
                                        pLastRow->ElStructSchema))
    pLastRow = pLastRow->ElParent;
  if (!pLastRow)
    return;
  /* get the table of interest */
  pTable = FirstSelectedColumn;
  while (pTable && !TypeHasException (ExcIsTable, pTable->ElTypeNumber,
                                      pTable->ElStructSchema))
    pTable = pTable->ElParent;
  if (!pTable)
    return;
  /* get the row that contains the first selected cell */
  pCurRow = FirstSelectedElement->ElParent;
  while (pCurRow && !TypeHasException (ExcIsRow, pCurRow->ElTypeNumber,
                                    pCurRow->ElStructSchema))
    pCurRow = pCurRow->ElParent;
  pFirstRow = pCurRow;
  /* scan all rows containing the selected cells */
  pCurCol = FirstSelectedColumn;
  while (pCurRow)
    {
      pCell = GetCellInRow (pCurRow, FirstSelectedColumn, TRUE, &back);
      rowOfCell = pCurRow;
      /* check the cells that are part of the current selection in this row */
      do
        {
          checkCell = TRUE;
          if (!pCell)
            /* this row does not contain any cell up to the first selected
               column */
            {
              if (pCurRow != pFirstRow)
                /* this is not the first row of the selection */
                {
                  if (LastSelectedColumn == FirstSelectedColumn)
                    /* the whole selection is in a single column */
                    pCell = NULL;
                  else
                    {
                      /* get the first cell in the row */
                      if (!pCurRow->ElTerminal)
                        pCell = pCurRow->ElFirstChild;
                      while (pCell && !TypeHasException (ExcIsCell,
                                                        pCell->ElTypeNumber,
                                                        pCell->ElStructSchema))
                        pCell = pCell->ElNext;
                      if (pCell)
                        {
                          pCol = GetColHeadOfCell (pCell);
                          if (pCol != LastSelectedColumn)
                            /* this cell is not in the last selected column */
                            if (!ElemIsBefore (pCol, LastSelectedColumn))
                              /* this cell is after the last selected column,
                               ignore it */
                              pCell = NULL;
                        }                                  
                    }
                }
              else
                /* This is the first row of the selection */
                /* There is a cell above in the first column of the table
                   that is extended vertically. */
                {
                  /* selection must start in the first column of the table */
                  /* get the first column of the table */
                  do
                    {
                      pCol = PreviousColumnInTable (FirstSelectedColumn);
                      if (pCol)
                        FirstSelectedColumn = pCol;
                    }
                  while (pCol);
                  /* find the previous row that has a cell in the first col */
                  pRow = PreviousRowInTable (pCurRow, pTable);
                  while (pRow && !pCell)
                    {
                      pCell = GetCellInRow (pRow, FirstSelectedColumn, FALSE,
                                            &back);
                      if (pCell)
                        /* there is a cell in the first column of this row */
                        {
                          rowOfCell = pRow;
                          if (ElemIsBefore (pRow, pFirstRow))
                            /* this cell is above the first selected cell.
                               Make it the new first selected cell */
                            {
                              FirstSelectedElement = pCell;
                              pFirstRow = pRow;
                              /* check the table again starting from the new
                                 first selected cell */
                              MakeSelectionRectangle ();
                              pRow = NULL;  pCell = NULL;
                            }
                        }
                      else
                        pRow = PreviousRowInTable (pRow, pTable);
                    }
                }
            }
          else if (back > 0)
            /* pCell is in a previous column */
            {
              /* get the horizontal spanning of the cell */
              attrSpan = GetAttrElementWithException (ExcColSpan, pCell);
              if (!attrSpan)
                span = 1;
              else
                span = attrSpan->AeAttrValue;
              if (span >= back+1 || span == 0)
                /* the previous cell in that row is extended up to (and
                   including) the first selected column.
                   It must be part of the selection */
                {
                  change = FALSE;
                  pCol = GetColHeadOfCell (pCell);
                  if (pCol != FirstSelectedColumn &&
                      ElemIsBefore (pCol, FirstSelectedColumn))
                    {
                      FirstSelectedColumn = pCol;
                      change = TRUE;
                    }
                  if (ElemIsBefore (pCell, FirstSelectedElement))
                    {
                      FirstSelectedElement = pCell;
                      change = TRUE;
                    }
                  if (change)
                    {
                      /* the beginning of the selection has changed.
                         Check again from the new beginning */
                      MakeSelectionRectangle ();
                      pCurRow = NULL; pCell = NULL;
                    }
                }
              else
                /* the space is occupied by a cell above that spans vertically.
                   If it is not the first selected row, this cell has already
                   been checked */
                {
                  if (pCurRow != pFirstRow)
                    checkCell = FALSE;
                  else
                    {
                      pCell = NULL;
                      /* get the row where this cell starts */
                      pRow = pCurRow;
                      do
                        {
                          pRow = PreviousRowInTable (pRow, pTable);
                          if (pRow)
                            pCell = GetCellInRow (pRow, pCurCol, FALSE, &back);
                        }
                      while (pRow && !pCell);
                      if (pRow && pCell)
                        /* this cell becomes the new first selected cell */
                        {
                          FirstSelectedElement = pCell;
                          pFirstRow = pRow;
                          /* Check again from the new beginning */
                          MakeSelectionRectangle ();
                          pCurRow = NULL; pCell = NULL;
                        }
                    }
                }
            }
          else
            /* there is a cell in the first selected column in this row */
            {
              if (pCurRow == pFirstRow)
                if (pCell != FirstSelectedElement &&
                    ElemIsBefore (pCell, FirstSelectedElement))
                  FirstSelectedElement = pCell;
            }

          /* now, check the right side of the current cell */
          if (pCell && checkCell)
            {
              /* get the horizontal spanning of the cell */
              attrSpan = GetAttrElementWithException (ExcColSpan, pCell);
              if (attrSpan)
                span = attrSpan->AeAttrValue;
              else
                span = 1;
              if (span != 1)
                {
                  /* does this cell extend beyound the last selected column? */
                  pCol = GetColHeadOfCell (pCell);
                  pC = pCol;
                  /* span = 0 means "to the last column of the table" */
                  for (i = 1; pC && (i < span || span == 0); i++)
                    {
                      pC = NextColumnInTable (pC, pTable);
                      if (pC)
                        pCol = pC;
                    }
                  if (pCol != LastSelectedColumn &&
                      ElemIsBefore (LastSelectedColumn, pCol))
                    /* this cell spans outside of the current selection */
                    /* the new last selected column is the end of the cell */
                    {
                      LastSelectedColumn = pCol;
                      /* Check again from the new beginning */
                      MakeSelectionRectangle ();
                      pCurRow = NULL; pCell = NULL;
                    }
                }
              if (pCell && pCell != LastSelectedElement &&
                  ElemIsBefore (LastSelectedElement, pCell))
                LastSelectedElement = pCell;
            }

          /* now, check the bottom of the current cell */
          if (pCell && checkCell)
            {
              /* get the vertical spanning of the cell */
              attrSpan = GetAttrElementWithException (ExcRowSpan, pCell);
              if (attrSpan)
                span = attrSpan->AeAttrValue;
              else
                span = 1;
              if (span != 1)
                {
                  /* does this cell extend beyound the last selected row? */
                  pRow = rowOfCell;
                  pR = pRow;
                  /* span = 0 means "to the bottom of the current tbody, thead
                     or tfoot" */
                  for (i = 1; pR && (i < span || span == 0); i++)
                    {
                      /* next sibling */
                      pR = pR->ElNext;
                      /* skip comments, PIs, etc. */
                      while (pR &&
                             !TypeHasException (ExcIsRow, pR->ElTypeNumber,
                                                pR->ElStructSchema))
                        pR = pR->ElNext;
                      if (pR)
                        pRow = pR;
                    }
                  if (pRow != pLastRow &&
                      ElemIsBefore (pLastRow, pRow))
                    /* this cell spans outside of the current selection */
                    /* the new last selected row is the end of the cell */
                    pLastRow = pRow;
                }
            }

          /* get the next cell to be checked in this row */
          if (pCell)
            {
              if (pCurRow == pFirstRow)
                /* for the first row, check every column in the current
                   selection */
                {
                  if (pCurCol == LastSelectedColumn)
                    pCell = NULL;
                  else
                    {
                      pCurCol = NextColumnInTable (pCurCol, pTable);
                      pCell = GetCellInRow (pCurRow, pCurCol, TRUE, &back);
                      if (pCell)
                        rowOfCell = pCurRow;
                    }
                }
              else
                {
                  /* get the next cell in the current row */
                  pCell = pCell->ElNext;
                  while (pCell &&
                         !TypeHasException (ExcIsCell, pCell->ElTypeNumber,
                                            pCell->ElStructSchema))
                    pCell = pCell->ElNext;
                  if (pCell)
                    /* is that cell in the selected range ? */
                    {
                      back = 0;
                      pCol = GetColHeadOfCell (pCell);
                      if (pCol && ElemIsBefore (LastSelectedColumn, pCol))
                        pCell = NULL;
                    }
                }
            }
        }
      while (pCell);

      /* get the next row to be checked */
      if (pCurRow)
	{
	  if (pCurRow == pLastRow)
	    pCurRow = NULL;
	  else
	    pCurRow = NextRowInTable (pCurRow, pTable);
	}
    }
}

/*----------------------------------------------------------------------
  DoExtendSelection
  Extend current selection to element pEl.
  If rank = 0, element pEl is entirely selected.
  If rank > 0, extend selection to the character having that rank in the
  text element pEl.
  If fixed is TRUE keeps the current fixed point and element.
  If begin is TRUE, the beginning of the current selection is moved to
  element pEl
  If begin is FALSE, the end of the current selection is extended to
  element pEl.
  If drag is TRUE, only the minimum processing is done.
  If checkSelection is TRUE the coherence of the selection is checked.
  ----------------------------------------------------------------------*/
static void DoExtendSelection (PtrElement pEl, int rank, ThotBool fixed,
                               ThotBool begin, ThotBool drag,
                               ThotBool checkSelection,
                               ThotBool checkTableSelection)
{
  PtrElement          oldFirstEl, oldLastEl, pElP, pAsc, pCell1, pCell2,
                      pColHead1, pColHead2, pTable1, pTable2,
                      pNext, parent;
  int                 oldFirstChar, oldLastChar;
  ThotBool            change, done, sel;
  ThotBool            updateFixed;

  sel = TRUE;
#ifdef _GTK
  gtk_claim_selection();
#endif /*_GTK*/
  if (!pEl)
    return;

  if (sel)
    {
      done = FALSE;
      if (!SelContinue || pEl != FirstSelectedElement)
        /* call the procedure handling selection in drawings */
        {
          if (pEl->ElParent != NULL)
            if (TypeHasException (ExcExtendedSelection,
                                  pEl->ElParent->ElTypeNumber,
                                  pEl->ElParent->ElStructSchema))
              /* l'element est le fils d'un element qui a l'exception
                 ExtendedSelection */
              {
                AddInSelection (pEl, TRUE);
                done = TRUE;
              }
        }
      if (!done)
        {
          /* keep the old selection */
          oldFirstEl = FirstSelectedElement;
          oldLastEl = LastSelectedElement;
          oldFirstChar = FirstSelectedChar;
          oldLastChar = LastSelectedChar;
          if (oldFirstEl &&
              oldFirstEl->ElTerminal && oldFirstEl->ElLeafType != LtPicture &&
              oldLastChar == 0 && oldFirstEl == oldLastEl &&
              oldFirstChar == oldLastChar)
            {
              /* the whole element was selected */
              oldFirstChar = 1;
              oldLastChar = oldFirstEl->ElVolume + 1;
            }
          SelectedPointInPolyline = 0;
          SelectedPictureEdge = 0;
          if (pEl->ElHolophrast)
            /* element pEl is holophrasted, select it entirely */
            rank = 0;
          if (!fixed)
            /* change fixed point by moving it first to the end */
            {
              FixedElement = LastSelectedElement;
              if (LastSelectedChar < rank)
                LastSelectedChar = rank;
              FixedChar = LastSelectedChar;
            }

          if (pEl == FixedElement ||
              (pEl->ElLeafType == LtPicture && FixedElement == pEl->ElParent))
            /* extension within fixed element */
            {
              FirstSelectedElement = pEl;
              FirstSelectedChar = 0;
              LastSelectedElement = pEl;
              LastSelectedChar = 0;
              if (FixedChar > 0 ||
                  pEl->ElLeafType == LtPicture ||
                  pEl->ElLeafType == LtSymbol)
                {
                  parent = pEl->ElParent;
                  if (pEl->ElLeafType == LtPicture && parent &&
                      TypeHasException (ExcIsImg, parent->ElTypeNumber,
                                        parent->ElStructSchema))
                    {
                      // select the whole IMG element
                      pEl = parent;
                      FirstSelectedElement = pEl;
                      FirstSelectedChar = 0;
                      LastSelectedElement = pEl;
                      LastSelectedChar = 0;
                      FixedElement = pEl;
                    }
                  else if (rank > 0 && rank < FixedChar)
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
              /* a single element is selected. If we were in column selection
                 mode, it's no longer the case */
              FirstSelectedColumn = NULL;
              LastSelectedColumn = NULL;
              WholeColumnSelected = FALSE;
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
              /* a single element is selected. If we were in column selection
                 mode, it's no longer the case */
              FirstSelectedColumn = NULL;
              LastSelectedColumn = NULL;
              WholeColumnSelected = FALSE;
            }
          else
            {
              /* is the fixed element (within) a table cell ? */
              pCell1 = FixedElement;
              while (pCell1 &&
                     !TypeHasException (ExcIsCell, pCell1->ElTypeNumber,
                                        pCell1->ElStructSchema))
                pCell1 = pCell1->ElParent;
              /* is element pEl (within) a table cell */
              if (pCell1)
                {
                  /* get the table that contains pCell1 */
                  pTable1 = pCell1;
                  while (pTable1 && !TypeHasException (ExcIsTable,
                                                      pTable1->ElTypeNumber,
                                                      pTable1->ElStructSchema))
                    pTable1 = pTable1->ElParent;
                }
              else
                pTable1 = NULL;
              pCell2 = pEl;
              while (pCell2 &&
                     !TypeHasException (ExcIsCell, pCell2->ElTypeNumber,
                                        pCell2->ElStructSchema))
                pCell2 = pCell2->ElParent;
              if (pCell2)
                {
                  /* get the table that contains pCell2 */
                  pTable2 = pCell2;
                  while (pTable2 && !TypeHasException (ExcIsTable,
                                                      pTable2->ElTypeNumber,
                                                      pTable2->ElStructSchema))
                    pTable2 = pTable2->ElParent;
                }
              else
                pTable2 = NULL;

              // check if they are nested tables
              if (pTable1 && pTable2 && pTable1 != pTable2)
                {
                  while (ElemIsAnAncestor (pTable1, pTable2))
                    {
                      pCell2 = pTable2->ElParent;
                      while (pCell2 &&
                             !TypeHasException (ExcIsCell, pCell2->ElTypeNumber,
                                                pCell2->ElStructSchema))
                        pCell2 = pCell2->ElParent;
                      if (pCell2)
                        {
                          /* get the table that contains pCell2 */
                          pTable2 = pCell2;
                          while (pTable2 && !TypeHasException (ExcIsTable,
                                                               pTable2->ElTypeNumber,
                                                               pTable2->ElStructSchema))
                            pTable2 = pTable2->ElParent;
                        }
                      else
                        pTable2 = NULL;
                      pEl = pCell2;
                    }
                  while (ElemIsAnAncestor (pTable2, pTable1))
                    {
                      pCell1 = pTable1->ElParent;
                      while (pCell1 &&
                             !TypeHasException (ExcIsCell, pCell1->ElTypeNumber,
                                                pCell1->ElStructSchema))
                        pCell1 = pCell1->ElParent;
                      if (pCell1)
                        {
                          /* get the table that contains pCell1 */
                          pTable1 = pCell1;
                          while (pTable1 && !TypeHasException (ExcIsTable,
                                                               pTable1->ElTypeNumber,
                                                               pTable1->ElStructSchema))
                            pTable1 = pTable1->ElParent;
                        }
                      else
                        pTable1 = NULL;
                      FixedElement = pCell1;
                    }
                  FixedChar = 0;
                  rank = 0;
                }

              if (pTable1 != pTable2)
                {
                  /* selection has started within a table and is being extended
                     outside, or it has started outside of a table and the user
                     tries to extend it in a table. Select the whole table */
                  if (pTable1 && pTable2)
                    {
                      if (ElemIsBefore (pTable1, pTable2))
                        {
                          FixedElement = pTable1;
                          FixedChar = 0;
                          pEl = pTable2;
                          rank = 0;
                        }
                      else
                        {
                          FixedElement = pTable2;
                          FixedChar = 0;
                          pEl = pTable1;
                          rank = 0;
                        }
                    }
                  else if (pTable1)
                    {
                      FixedElement = pTable1;
                      FixedChar = 0;
                    }
                  else
                    {
                      pEl = pTable2;
                      rank = 0;
                    }
                }
              else if (pCell1 && pCell2)
                {
                  if (pCell2 == pCell1)
                    /* extending selection within the same cell. We are no
                       longer in table selection mode */
                    {
                      FirstSelectedColumn = NULL;
                      LastSelectedColumn = NULL;
                      WholeColumnSelected = FALSE;
                    }
                  else
                    /* pEl is in a cell (pCell2), but not in the same cell as
                       the initial selection (pCell1). */
                    {
                      pColHead1 = GetColHeadOfCell (pCell1);
                      pColHead2 = GetColHeadOfCell (pCell2);
                      if (pColHead1 == pColHead2)
                        {
                          FirstSelectedColumn = pColHead1;
                          LastSelectedColumn = pColHead2;
                        }
                      else
                        {
                          if (ElemIsBefore (pColHead1, pColHead2))
                            {
                              FirstSelectedColumn = pColHead1;
                              LastSelectedColumn = pColHead2;
                            }
                          else
                            {
                              FirstSelectedColumn = pColHead2;
                              LastSelectedColumn = pColHead1;
                            }
                        }
                      WholeColumnSelected = FALSE;
                      pEl = pCell2;
                      rank = 0;
                    }
                }

              if (ElemIsBefore (pEl, FixedElement))
                /* pEl is before the fixed point in the tree structure */
                {
                  FirstSelectedElement = pEl;
                  FirstSelectedChar = rank;
                  while (FixedChar == 1 && ElemIsBefore (pEl, FixedElement))
                    {
                      /* move the end of the selection to the end of the
                         previous element */
                      FixedElement = PreviousLeafInSelection (FixedElement);
                      if (FixedElement && FixedElement->ElTerminal &&
                          FixedElement->ElLeafType == LtText)
                        FixedChar =  FixedElement->ElVolume + 1;
                    }
                  LastSelectedElement = FixedElement;
                  LastSelectedChar = FixedChar;
                  if (FirstSelectedColumn)
                    /* we are in table selection mode. Select the enclosing
                       cell of the fixed element */
                    {
                      LastSelectedChar = 0;
                      pAsc = LastSelectedElement;
                      while (pAsc && !TypeHasException (ExcIsCell,
                                                        pAsc->ElTypeNumber,
                                                        pAsc->ElStructSchema))
                        pAsc = pAsc->ElParent;
                      if (pAsc)
                        LastSelectedElement = pAsc;
                    }
                }
              else
                /* pEl is after the fixed point */
                {
                  FirstSelectedElement = FixedElement;
                  FirstSelectedChar = FixedChar;
                  if (FirstSelectedColumn)
                    /* we are in table selection mode. Select the enclosing
                       cell of the fixed element */
                    {
                      FirstSelectedChar = 0;
                      pAsc = FirstSelectedElement;
                      while (pAsc && !TypeHasException (ExcIsCell,
                                                        pAsc->ElTypeNumber,
                                                        pAsc->ElStructSchema))
                        pAsc = pAsc->ElParent;
                      if (pAsc)
                        FirstSelectedElement = pAsc;
                    }
                  if (rank == 1 && drag)
                    {
                      pElP = pEl;
                      /* move the end of the selection to the end of the 
                         previous element */
                      pEl = PreviousLeafInSelection (pEl);
                      if (pEl && pEl->ElTerminal &&
                          pEl->ElLeafType == LtText)
                        rank =  pEl->ElVolume + 1;
                    }
                  LastSelectedElement = pEl;
                  LastSelectedChar = rank;
                }
            }

          if (FirstSelectedColumn && checkTableSelection)
            /* we are in table selection mode. Make sure that the selection
               has the shape of a rectangle */
            MakeSelectionRectangle ();

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
              updateFixed = (FirstSelectedElement == FixedElement);
              pElP = FirstSelectedElement;
              pNext = NextInSelection (FirstSelectedElement,
                                       LastSelectedElement);
              if (pNext)
                {
                  FirstSelectedElement = FirstLeaf (pNext);
                  if (updateFixed)
                    FixedElement = FirstSelectedElement;
                  if (FirstSelectedElement->ElTerminal &&
                      FirstSelectedElement->ElLeafType == LtText)
                    {
                      if (updateFixed && FirstSelectedChar == FixedChar) 
                        FixedChar = 1;
                      if (FirstSelectedElement == oldFirstEl &&
                          oldFirstChar == 1)
                        /* probably the user wanted to move to the previous
                           element */
                        FirstSelectedElement = pElP;
                      else
                        FirstSelectedChar = 1;
                    }
                  else
                    {
                      if (updateFixed && FirstSelectedChar == FixedChar) 
                        FixedChar = 0;
                      FirstSelectedChar = 0;
                    }
                }
            }
          if (StructSelectionMode && checkSelection)
            /* selection is structured mode */
            /* normalize selection: the first and last selected elements */
            /* must be siblings in the abstract tree */
            if (FirstSelectedElement->ElParent != LastSelectedElement->ElParent)
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
                        LastSelectedElement = LastSelectedElement->ElParent;
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
          if (checkSelection &&
              (oldFirstEl != FirstSelectedElement ||
               oldLastEl != LastSelectedElement ||
               oldFirstChar != FirstSelectedChar ||
               oldLastChar != LastSelectedChar))
            {
              change = TRUE;
              if (oldFirstEl)
                if ((!oldFirstEl->ElTerminal || oldFirstEl->ElLeafType != LtPicture) &&
                  !TypeHasException (ExcIsImg, FirstSelectedElement->ElTypeNumber,
                                     FirstSelectedElement->ElStructSchema))
                  {
                  // select the parent element if the whole content is selected
                  if (FirstSelectedChar == 0 && !FirstSelectedColumn)
                    while (FirstSelectedElement->ElNext == NULL
                           && FirstSelectedElement->ElPrevious == NULL
                           && FirstSelectedElement->ElParent != NULL)
                      FirstSelectedElement = FirstSelectedElement->ElParent;
                  if (ElemIsAnAncestor (FirstSelectedElement, LastSelectedElement))
                    {
                      LastSelectedElement = FirstSelectedElement;
                      LastSelectedChar = 0;
                    }
                  }
              if (LastSelectedChar == 0 &&
                  LastSelectedElement != FirstSelectedElement &&
                  !LastSelectedColumn)
                while (LastSelectedElement->ElNext == NULL
                       && LastSelectedElement->ElPrevious == NULL
                       && LastSelectedElement->ElParent != NULL)
                  LastSelectedElement = LastSelectedElement->ElParent;
              if (LastSelectedElement->ElLeafType == LtPicture &&
                  LastSelectedElement != FirstSelectedElement)
                {
                  pElP = LastSelectedElement->ElParent;
                  if (pElP && TypeHasException (ExcIsImg, pElP->ElTypeNumber,
                                                pElP->ElStructSchema))
                    // select the image element instead of the PICTURE_UNIT
                LastSelectedElement = pElP;
                }
              if (ElemIsAnAncestor (LastSelectedElement, FirstSelectedElement))
                {
                 FirstSelectedElement = LastSelectedElement;
                  FirstSelectedChar = 0;
                }
            }
	  
          SelPosition = FALSE;
          if (!change && drag && FirstSelectedElement == LastSelectedElement &&
              FirstSelectedElement->ElVolume == 0)
            SelPosition = TRUE;
          else if (FirstSelectedElement == LastSelectedElement &&
                   LastSelectedChar <= FirstSelectedChar &&
                   FirstSelectedElement->ElTerminal &&
                   FirstSelectedElement->ElLeafType != LtPicture)
            /* In this case there is no longer an extended selection */
            SelPosition = TRUE;
	  
          if (change || !drag)
            /* the new selection is not the same as the previous one */
            /* highlight it */
            HighlightSelection ((ThotBool)(FirstSelectedElement != oldFirstEl), TRUE);
          if (!drag && SelectionUpdatesMenus &&
              /* update all menus that change with the selection */
              (oldFirstEl != FirstSelectedElement ||
               oldLastEl != LastSelectedElement))
            {
              PrepareSelectionMenu ();
              if (ThotLocalActions[T_chattr] != NULL)
                (*(Proc1)ThotLocalActions[T_chattr]) (
                                                      (void *)SelectedDocument);
            }
          if (!fixed)
            /* change the fixed point: move it to the begining */
            {
              FixedElement = FirstSelectedElement;
              FixedChar = FirstSelectedChar;
            }
        }
    }
}

/*----------------------------------------------------------------------
  ExtendSelection
  Extend current selection to element pEl.
  If rank = 0, element pEl is entirely selected.
  If rank > 0, extend selection to the character having that rank in the
  text element pEl.
  If fixed is TRUE keeps the current fixed point and element.
  If begin is TRUE, the beginning of the current selection is moved to
  element pEl
  If begin is FALSE, the end of the current selection is extended to
  element pEl.
  If drag is TRUE, only the minimum processing is done.
  ----------------------------------------------------------------------*/
void ExtendSelection (PtrElement pEl, int rank, ThotBool fixed, ThotBool begin,
                      ThotBool drag)
{
  DoExtendSelection (pEl, rank, fixed, begin, drag, TRUE, TRUE);
}

/*----------------------------------------------------------------------
  ReverseSelect
  Highlight or switch off (according to parameter highlight) all
  boxes of element pEl in document pDoc.
  ----------------------------------------------------------------------*/
static void ReverseSelect (PtrElement pEl, PtrDocument pDoc, ThotBool highlight)
{
  PtrAbstractBox      pAb, pSelAb[MAX_TRANSMIT];
  int                 lastView, view, frame;
  int                 depth;

  /* consider all views */
  lastView = MAX_VIEW_DOC;
  for (view = 0; view < lastView; view++)
    {
      /* get the window corresponding to the view */
      if (pDoc->DocView[view].DvPSchemaView > 0)
        frame = pDoc->DocViewFrame[view];
      else
        frame = 0;
      /* scan all abstract boxes of the element in the view */
      pAb = pEl->ElAbstractBox[view];
      depth = 0;
      while (pAb != NULL)
        {
          /*
            For ghost elements or elements with the exception
            HighlightChildren, the selection is transmitted to children
          */
          while (pAb && pAb->AbFirstEnclosed &&
                 FrameTable[frame].FrView == 1 &&
                 TypeHasException (ExcHighlightChildren,
                                   pAb->AbElement->ElTypeNumber,
                                   pAb->AbElement->ElStructSchema))
            {
              /* select children intead of the current abstract box */
              if (depth < MAX_TRANSMIT)
                pSelAb[depth++] = pAb;
              pAb = pAb->AbFirstEnclosed;
            }
          pAb->AbSelected = highlight;
          SetNewSelectionStatus (frame, pAb, highlight);
          /* get the next abstract box for the element */
          pAb = pAb->AbNext;
          while (pAb == NULL && depth > 0)
            /* all children of the ghost element are now handled */ 
            pAb = pSelAb[--depth]->AbNext;
          if (depth == 0 && pAb && pAb->AbElement != pEl)
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
void AddInSelection (PtrElement pEl, ThotBool last)
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
              if (FirstSelectedElement->ElTerminal &&
                  ((FirstSelectedElement->ElLeafType == LtText &&
                    FirstSelectedChar > 1) ||
                   ((FirstSelectedElement->ElLeafType == LtPolyLine ||
                     FirstSelectedElement->ElLeafType == LtPath) &&
                    SelectedPointInPolyline > 0)))
                SelectElement (SelectedDocument, FirstSelectedElement, TRUE,
                               TRUE, TRUE);
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
        {
          if (SelectedElement[i - 1] == pEl)
            ok = FALSE;
          else
            i++;
        }
      while (ok && i <= NSelectedElements);
      if (NSelectedElements < MAX_SEL_ELEM && ok)
        {
          SelectedElement[NSelectedElements++] = pEl;
          LastSelectedElement = pEl;
          /* highlight the new selected element in all views where is */
          /* is visible */
          ReverseSelect (pEl, SelectedDocument, TRUE);
          if (last && SelectionUpdatesMenus)
            {
              /* update all the menus that depend on the current */
              /* selection */
              if (ThotLocalActions[T_chattr] != NULL)
                (*(Proc1)ThotLocalActions[T_chattr]) (
                                                      (void *)SelectedDocument);
            }
        }
    }
}

/*----------------------------------------------------------------------
  RemoveFromSelection
  Remove element pEl from the current selection, but only if the
  current selection is discrete.
  ----------------------------------------------------------------------*/
void RemoveFromSelection (PtrElement pEl)
{
  int                 i, j;

  if (SelContinue || pEl == NULL)
    return;
  /* search that element in the selected elements */
  for (i = 0; i < NSelectedElements && SelectedElement[i] != pEl; i++) ;
  if (SelectedElement[i] == pEl)
    {
      /* remove that element from the list of selected elements */
      for (j = i + 1; j < NSelectedElements; j++)
        SelectedElement[j - 1] = SelectedElement[j];
      NSelectedElements--;
      if (i == NSelectedElements)
        LastSelectedElement = SelectedElement[i - 1];
      /* switch selection off */
      ReverseSelect (pEl, SelectedDocument, FALSE);
      if (LatestReturned == i + 1)
        LatestReturned = i;
    }
}

/*----------------------------------------------------------------------
  SelectableAncestor
  If element pEl has exception NoSelect or Hidden, return the first ancestor
  that can be selected.
  However, if the element is hidden, but has specified a callback for event
  Select, it is accepted.
  ----------------------------------------------------------------------*/
static void SelectableAncestor (PtrElement *pEl, int *position)
{
  ThotBool    graph, stop;

  graph = ((*pEl)->ElLeafType == LtPolyLine ||
           (*pEl)->ElLeafType == LtPath ||
           ((*pEl)->ElLeafType == LtGraphics && (*pEl)->ElGraph == 'g'));
  if (TypeHasException (ExcNoSelect, (*pEl)->ElTypeNumber,
                        (*pEl)->ElStructSchema) ||
      (HiddenType (*pEl) && !ElementHasAction(*pEl, TteElemSelect, TRUE)) ||
      (TypeHasException (ExcSelectParent, (*pEl)->ElTypeNumber,
                         (*pEl)->ElStructSchema) &&
       (*position == 0 || !graph)))
    {
      /* select element entirely */
      *position = 0;
      stop = FALSE;
      while (!stop)
        if ((*pEl)->ElParent == NULL)
          /* that's a root. Select it */
          stop = TRUE;
        else
          {
            *pEl = (*pEl)->ElParent;
            if (!TypeHasException (ExcNoSelect, (*pEl)->ElTypeNumber,
                                   (*pEl)->ElStructSchema)  &&
                (!HiddenType (*pEl) ||
                 ElementHasAction (*pEl, TteElemSelect, TRUE)) &&
                !TypeHasException (ExcSelectParent, (*pEl)->ElTypeNumber,
                                   (*pEl)->ElStructSchema))
              stop = TRUE;
          }
    }
}

/*----------------------------------------------------------------------
  SelectElementWithEvent
  Same function as SelectElement, but send  events TteElemSelect.Pre and
  TteElemSelect.Post to the application
  ----------------------------------------------------------------------*/
void SelectElementWithEvent (PtrDocument pDoc, PtrElement pEl,
                             ThotBool begin, ThotBool check)
{
  NotifyElement       notifyEl;
  Document            doc;
  int                 pos;

  if (IsEmptyDocument (pDoc))
    return; // don't select within an empty document
  if (pDoc && pEl)
    {
      pos = 0;
      SelectableAncestor (&pEl, &pos);
      doc = IdentDocument (pDoc);
      notifyEl.event = TteElemSelect;
      notifyEl.document = doc;
      notifyEl.element = (Element) pEl;
      notifyEl.info = 0; /* not sent by undo */
      notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
      notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
      notifyEl.position = 0;
      if (!CallEventType ((NotifyEvent *) & notifyEl, TRUE))
        {
          SelectElement (pDoc, pEl, begin, check, TRUE);
          notifyEl.event = TteElemSelect;
          notifyEl.document = doc;
          notifyEl.element = (Element) pEl;
          notifyEl.info = 0; /* not sent by undo */
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
void SelectPositionWithEvent (PtrDocument pDoc, PtrElement pEl, int first,
                              ThotBool withDecoration)
{
  NotifyElement       notifyEl;
  Document            doc;

  if (IsEmptyDocument (pDoc))
    return; // don't select within an empty document
  if (pDoc && pEl)
    {
      SelectableAncestor (&pEl, &first);
      doc = IdentDocument (pDoc);
      notifyEl.event = TteElemSelect;
      notifyEl.document = doc;
      notifyEl.element = (Element) pEl;
      notifyEl.info = 0; /* not sent by undo */
      notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
      notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
      notifyEl.position = 0;
      if (!CallEventType ((NotifyEvent *) & notifyEl, TRUE))
        {
          SelectStringOrPosition (pDoc, pEl, first, first, FALSE, withDecoration);
          notifyEl.event = TteElemSelect;
          notifyEl.document = doc;
          notifyEl.element = (Element) pEl;
          notifyEl.info = 0; /* not sent by undo */
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
void SelectStringWithEvent (PtrDocument pDoc, PtrElement pEl, int firstChar,
                            int lastChar)
{
  NotifyElement       notifyEl;
  Document            doc;
  int                 pos;

  if (pDoc && pEl)
    {
      pos = 0;
      SelectableAncestor (&pEl, &pos);
      doc = IdentDocument (pDoc);
      notifyEl.event = TteElemSelect;
      notifyEl.document = doc;
      notifyEl.element = (Element) pEl;
      notifyEl.info = 0; /* not sent by undo */
      notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
      notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
      notifyEl.position = 0;
      if (!CallEventType ((NotifyEvent *) & notifyEl, TRUE))
        {
          SelectStringOrPosition (pDoc, pEl, firstChar, lastChar, TRUE, TRUE);
          notifyEl.event = TteElemSelect;
          notifyEl.document = doc;
          notifyEl.element = (Element) pEl;
          notifyEl.info = 0; /* not sent by undo */
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
  extension: TRUE if the user wants to extend the current selection.
  begin: TRUE when the beginning of the current selection is extended,
  FALSE when the end of the current selection is extended.
  doubleClick: if TRUE, the user has double-clicked without moving the mouse.
  drag: the user extends the selection by dragging.
  Return TRUE when the application asks Thot to do nothing.
  ----------------------------------------------------------------------*/
ThotBool ChangeSelection (int frame, PtrAbstractBox pAb, int rank,
                          ThotBool extension, ThotBool begin,
                          ThotBool doubleClick, ThotBool drag)
{
  PtrDocument         pDoc;
  PtrSSchema          pSS;
  PtrElement          pEl, pParent;
  PtrAttribute        pAttr;
  PtrAbstractBox      pGroup;
  NotifyElement       notifyEl;
  Document            doc;
  int                 view;
  ThotBool            error, stop, doubleClickRef;
  ThotBool            graphSel, result;

  pEl = NULL;
  result = FALSE;
  if (pAb == NULL || pAb->AbElement == NULL)
    return result;
  /* search the document and the view corresponding to the window */
  GetDocAndView (frame, &pDoc, &view);
  doc = IdentDocument (pDoc);
  /* by default Thot applies its editing changes */
  if (doubleClick)
    {
      /* send event TteElemActivate.Pre to the application */
      pEl = pAb->AbElement;
      notifyEl.event = TteElemActivate;
      notifyEl.document = doc;
      notifyEl.element = (Element) pEl;
      notifyEl.info = 0; /* not sent by undo */
      notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
      notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
      notifyEl.position = 0;
      if (CallEventType ((NotifyEvent *) & notifyEl, TRUE))
        /* the application asks Thot to do nothing */
        return TRUE;
      if (pEl->ElHolophrast)
        {
          /* avoid to rebuild menus. It will be done by */
          /* SelectElement */
          SelectedDocument = NULL;
          /* switch off the previous selection */
          CancelSelection ();
          DeHolophrast (pEl, pDoc);
          SelectElementWithEvent (pDoc, pEl, TRUE, FALSE);
          return result;
        }
    }
  else if (view == 1)
    {
      // do we have to move the selection to an emclosing SVG group
      pGroup = GetParentGroup (pAb->AbBox);
      if (pGroup)
        pAb = pGroup;
    }

  pEl = pAb->AbElement;
  error = FALSE;
  doubleClickRef = FALSE;
  graphSel = (pAb->AbElement->ElLeafType == LtPolyLine ||
              pAb->AbElement->ElLeafType == LtPath ||
              (pAb->AbElement->ElLeafType == LtGraphics &&
               pAb->AbElement->ElGraph == 'g'));
  /* process double clicks and extensions for polyline vertices */
  if (pAb && pAb->AbElement->ElTerminal && graphSel)
    /* it's a polyline or a line */
    {
      if (extension)
        /* it's a selection extension */
        {
          if (FirstSelectedElement == pAb->AbElement &&
              rank == SelectedPointInPolyline)
            /* same polyline and same vertex as before. Then, it's not */
            /* really an extension */
            extension = FALSE;
          else
            /* select the entire polyline */
            rank = 0;
        }
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
  if (doubleClick && pAb && pAb->AbElement)
    {
      pEl = pAb->AbElement;
      pSS = pEl->ElStructSchema;
      if (pSS->SsRule->SrElem[pEl->ElTypeNumber - 1]->SrConstruct != CsReference)
        {
          /* search for an inclusion among the ancestors */
          pParent = pEl;
          while (pParent->ElParent != NULL && pParent->ElSource == NULL)
            pParent = pParent->ElParent;
          if (pParent->ElSource != NULL)
            /* it's an inclusion */
            pEl = pParent;
        }
      if (pSS->SsRule->SrElem[pEl->ElTypeNumber - 1]->SrConstruct == CsReference ||
          pEl->ElSource != NULL)
        /* this element is a reference or an inclusion */
        {
          doubleClickRef = TRUE;
          FirstSelectedElement = pEl;
          LastSelectedElement = pEl;
          SelectedPointInPolyline = 0;
          SelectedPictureEdge = 0;
        }
      else
        {
          /* it's neither an inclusion nor a reference element */
          /* search a reference attribute with exception ActiveRef */
          /* among the ancestors */
          pParent = pEl;
          do
            {
              pAttr = pParent->ElFirstAttr;
              /* scan all attributes of current element */
              while (pAttr != NULL && !doubleClickRef)
                {
                  if (pAttr->AeAttrType == AtReferenceAttr &&
                      AttrHasException (ExcActiveRef, pAttr->AeAttrNum, pAttr->AeAttrSSchema))
                    /* a reference attribute has been found */
                    {
                      doubleClickRef = TRUE;
                      FirstSelectedElement = pParent;
                      LastSelectedElement = pParent;
                      SelectedPointInPolyline = 0;
                      SelectedPictureEdge = 0;
                    }
                  else
                    /* next attribute of same element */
                    pAttr = pAttr->AeNext;
                }
              if (!doubleClickRef)
                /* higher level ancestor */
                pParent = pParent->ElParent;
            }
          while (pParent != NULL && !doubleClickRef);
        }
    }

  if (pAb && pAb->AbElement->ElTerminal &&
      (pAb->AbElement->ElLeafType == LtPairedElem ||
       pAb->AbElement->ElLeafType == LtReference) &&
      /* it's a reference element or a paired element */
      (!pAb->AbPresentationBox || !pAb->AbCanBeModified))
    /* it's not the presentation box of an attribute value */
    /* select all the contents */
    rank = 0;
  
  if (extension)
    {
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
                  LastSelectedCharInAttr = pAb->AbVolume + 1;
                }
              else if (rank <= FixedChar)
                {
                  FirstSelectedCharInAttr = rank;
                  LastSelectedCharInAttr = FixedChar;
                }
              else
                {
                  FirstSelectedCharInAttr = FixedChar;
                  LastSelectedCharInAttr = rank;
                }
              if (TtaGetDisplayMode (FrameTable[frame].FrDoc) == DisplayImmediately)
                SelectStringInAttr (pDoc, pAb, FirstSelectedCharInAttr,
                                    LastSelectedCharInAttr, TRUE);
            }
        }
      else
        {
          if (pDoc != SelectedDocument)
            /* extension to a different document is not allowed */
            error = TRUE;
          else
            {
              if (pAb == NULL)
                error = TRUE;
              else
                {
                  pEl = pAb->AbElement;
                  pSS = pEl->ElStructSchema;
                  if (pSS->SsRule->SrElem[pEl->ElTypeNumber - 1]->SrConstruct == CsConstant)
                    /* the element to be selected is a constant */
                    /* select it entirely */
                    rank = 0;
                  /* If the element to be selected is hidden or cannot be */
                  /* selected, get the first ancestor that can be selected*/
                  if (TypeHasException (ExcNoSelect, pEl->ElTypeNumber, pSS) ||
                      (HiddenType (pEl) &&
                       !ElementHasAction(pEl, TteElemExtendSelect, TRUE)) ||
                      TypeHasException (ExcSelectParent, pEl->ElTypeNumber, pSS))
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
                            if (!TypeHasException (ExcNoSelect, pEl->ElTypeNumber, pEl->ElStructSchema) &&
                                (!HiddenType (pEl) ||
                                 ElementHasAction(pEl, TteElemExtendSelect, TRUE)))
                              stop = TRUE;
                          }
                    }
                  /* send event TteElemExtendSelect.Pre to the application*/
                  notifyEl.event = TteElemExtendSelect;
                  notifyEl.document = doc;
                  notifyEl.element = (Element) pEl;
                  notifyEl.info = 0; /* not sent by undo */
                  notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
                  notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
                  notifyEl.position = 0;
                  result = CallEventType ((NotifyEvent *) &notifyEl, TRUE);
                  if (!result)
                    /* application accepts selection */
                    {
                      /* do select */
                      ExtendSelection (pEl, rank, extension, begin, drag);
                      /* send event TteElemExtendSelect.Pre to the */
                      /* application */
                      notifyEl.event = TteElemExtendSelect;
                      notifyEl.document = doc;
                      notifyEl.element = (Element) pEl;
                      notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
                      notifyEl.info = 0; /* not sent by undo */
                      notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
                      notifyEl.position = 0;
                      CallEventType ((NotifyEvent *) & notifyEl, FALSE);
                    }
                }
            }
        }
    }
  else
    {
      /* new selection */
      if (pDoc != SelectedDocument && pDoc != DocSelectedAttr)
        /* in another document */
        TtaClearViewSelections ();
      else if (!doubleClickRef && doubleClick && SelectedView == view &&
               FirstSelectedElement == LastSelectedElement &&
               pAb->AbElement == FirstSelectedElement)
        /* user has double-clicked on the same element */
        {
          pEl = FirstSelectedElement;
          if (pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber - 1]->SrConstruct == CsReference ||
              pEl->ElSource != NULL)
            /* this element is a reference or an inclusion */
            doubleClickRef = TRUE;
        }
      if (doubleClick)
        {
          FindReferredEl ();
          /* send an event TteElemActivate.Pre to the application */
          notifyEl.event = TteElemActivate;
          notifyEl.document = doc;
          notifyEl.element = (Element) pEl;
          notifyEl.info = 0; /* not sent by undo */
          notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
          notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
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
          if (pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber - 1]->SrConstruct == CsConstant)
            /* the element to be selected is a constant */
            /* Select it entirely */
            rank = 0;
          if (rank > 0 && pAb->AbPresentationBox && pAb->AbCanBeModified)
            /* user has clicked in the presentation box displaying an */
            /* attribute value */
            {
              CancelSelection ();
              SelectStringInAttr (pDoc, pAb, rank, rank, FALSE);
              FixedChar = rank;
            }
          else if (rank > 0 && pEl->ElTerminal &&
                   (pEl->ElLeafType == LtText ||
                    pEl->ElLeafType == LtSymbol ||
                    pEl->ElLeafType == LtPolyLine ||
                    pEl->ElLeafType == LtPath ||
                    pEl->ElLeafType == LtGraphics ||
                    pEl->ElLeafType == LtPicture))
            SelectPositionWithEvent (pDoc, pEl, rank, FALSE);
          else
            SelectElementWithEvent (pDoc, pEl, TRUE, FALSE);
        }
    }

  if (!doubleClick)
    {
      if (!error)
        /* If all the contents of a text leaf is selected, then the leaf */
        /* itself is considered as selected */
        if (LastSelectedElement && LastSelectedElement == FirstSelectedElement)
          {
            pEl = FirstSelectedElement;
            if (pEl->ElTerminal && pEl->ElLeafType == LtText &&
                pEl->ElTextLength < LastSelectedChar && FirstSelectedChar <= 1)
              {
                LastSelectedChar = 0;
                FirstSelectedChar = 0;
              }
          }
    }

#ifdef _WINGUI
  /* SG: bug fix, when a dialog is opened, the windows does not generate a
   * paint event, in order to show the selection, we must force a paint event */
  SendMessage (FrRef[frame], WM_PAINT, (WPARAM) 0, (LPARAM) 0);
#endif /* _WINGUI */

  return result;
}

/*----------------------------------------------------------------------
  PrepareSelectionMenu
  Search elements to be put in the Select menu.
  ----------------------------------------------------------------------*/
void PrepareSelectionMenu ()
{
  PtrElement          pEl1;
  PtrElement          pEl2;
  ThotBool            stop;
  ThotBool            stop1;

  /* ignore exception NoSelect */
  /* search the first ancestor of the first selected element that is */
  /* not hidden */
  SelMenuParentEl = NULL;
  if (FirstSelectedElement && FirstSelectedElement->ElParent)
    {
      pEl1 = FirstSelectedElement->ElParent;
      stop = FALSE;
      while (!stop)
        {
          if (pEl1 == NULL)
            stop = TRUE;
          else if (!ElementIsHidden (pEl1) && !HiddenType (pEl1) &&
                   ElemIsAnAncestor (pEl1, LastSelectedElement))
            stop = TRUE;
          else
            pEl1 = pEl1->ElParent;
        }
      if (pEl1)
        SelMenuParentEl = pEl1;
    }
  
  /* search the previous element that can be selected */
  SelMenuPreviousEl = NULL;
  stop = FALSE;
  pEl1 = FirstSelectedElement;
  while (!stop && pEl1)
    if (pEl1->ElPrevious)
      {
        pEl1 = pEl1->ElPrevious;
        if (!ElementIsHidden (pEl1))
          {
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
                      while (ElementIsHidden (pEl2) &&
                             pEl2->ElPrevious != NULL)
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
      }
    else
      pEl1 = pEl1->ElParent;
  if (pEl1 != NULL)
    SelMenuPreviousEl = pEl1;

  /* search the next element that can be selected */
  SelMenuNextEl = NULL;
  stop = FALSE;
  pEl1 = LastSelectedElement;
  while (!stop && pEl1)
    if (pEl1->ElNext)
      {
        pEl1 = pEl1->ElNext;
        if (!ElementIsHidden (pEl1))
          {
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
                      while (ElementIsHidden (pEl2) &&
                             pEl2->ElNext != NULL)
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
      }
    else
      pEl1 = pEl1->ElParent;
  if (pEl1 != NULL)
    SelMenuNextEl = pEl1;

  /* search the first descendent that can be selected */
  SelMenuChildEl = NULL;
  stop = FALSE;
  pEl1 = FirstSelectedElement;
  while (!stop && pEl1)
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
void BuildSelectionMessage ()
{
  PtrElement          pEl, pTable, parent;
  PtrDocument         pDoc;
  char                msgBuf[MAX_TXT_LEN];
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
  if (pEl == NULL)
    return;
  /* add the types of the ancestors */
  nbasc = 0;
  msgBuf[0] = EOS;
  pTable = NULL;
  while (pEl)
    {
      parent = pEl->ElParent;
      /* skip that ancestor if it is hidden */
      if (!HiddenType (pEl))
        {
          if (!parent ||
              !TypeHasException (ExcIsImg, parent->ElTypeNumber, parent->ElStructSchema))
            {
              if (nbasc == 0 && WholeColumnSelected)
                {
                  strcat (msgBuf, "column");
                  pTable = pEl;
                  while (pTable &&
                         !TypeHasException (ExcIsTable,
                                            pTable->ElTypeNumber,
                                            pTable->ElStructSchema))
                    pTable = pTable->ElParent;
                }
              else if (pTable == NULL || pEl == pTable)
                {
                  /* put a separator if it's not the first element name */
                  if (nbasc > 0)
                    strcat (msgBuf, " \\ ");
                  /* put the name of the element */
                  if (pEl->ElStructSchema)
                    strcat (msgBuf,
                            pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber - 1]->SrName);
                  /* display parent elements of the table */
                  pTable = NULL;
                }
              if (msgBuf[0] != EOS)
                nbasc++;
            }
        }
      if (nbasc >= MAX_ITEM_MSG_SEL)
        pEl = NULL;
      else
        {
          pEl = parent;
          if (pEl && pEl == pDoc->DocDocElement)
            /* do not display the Document element name */
            pEl = NULL;
        }
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
ThotBool SelectPairInterval ()
{
  ThotBool            ret;

  ret = FALSE;
  if (!SelContinue && NSelectedElements == 2)
    /* only two elements are selected */
    if (SelectedElement[0] == GetOtherPairedElement (SelectedElement[1]))
      /* they are paired elements */
      {
        SelContinue = TRUE;
        FirstSelectedColumn = NULL;
        LastSelectedColumn = NULL;
        WholeColumnSelected = FALSE;
        LastSelectedElement = FirstSelectedElement;
        ExtendSelection (SelectedElement[1], 0, TRUE, TRUE, FALSE);
        ret = TRUE;
      }
  return ret;
}

/*----------------------------------------------------------------------
  MoveSelectionToCol returns pointer to <col> elements if the selection
  concerns a <colhead>
  Return TRUE if a change is done
  ----------------------------------------------------------------------*/
ThotBool MoveSelectionToCol (PtrElement *firstSel, PtrElement *lastSel)
{
  PtrElement          pTable, pRow;
  PtrAttribute        pAttr;
  ThotBool            first, last;
  int                 back;

  if (FirstSelectedColumn)
    // we are in column selection mode
    {
      /* get the table that contains the selection */
      pTable = FirstSelectedColumn;
      while (pTable &&
	     !TypeHasException (ExcIsTable, pTable->ElTypeNumber,
				pTable->ElStructSchema))
	pTable = pTable->ElParent;
      /* get the row that contains the first cell selected */
      pRow = (*firstSel)->ElParent;
      while (pRow && !TypeHasException (ExcIsRow,
					pRow->ElTypeNumber,
					pRow->ElStructSchema))
	pRow = pRow->ElParent;
      /* get the previous row that contains a cell in this column (a
         colspanned cell from a previous column may have "eaten" the cell
         in this column */
      first = true;
      while (pRow && first)
	{
	  pRow = PreviousRowInTable (pRow, pTable);
	  if (pRow)
	    {
	      if (GetCellInRow (pRow, FirstSelectedColumn, FALSE, &back))
		/* there is a cell for that column in a previous row */
		first = false;
	    }
	}
      if (first)
        {
	  /* get the row that contains the last cell selected */
	  pRow = (*lastSel)->ElParent;
	  while (pRow && !TypeHasException (ExcIsRow,
					    pRow->ElTypeNumber,
					    pRow->ElStructSchema))
	    pRow = pRow->ElParent;
	  /* get the next row that contains a cell in this column (a colspanned
             cell from a previous column may have "eaten" the cell in this
             column */
	  last = true;
	  while (pRow && last)
	    {
	      pRow = NextRowInTable (pRow, pTable);
	      if (pRow)
		{
		  if (GetCellInRow (pRow, LastSelectedColumn, FALSE, &back))
		    /* there is a cell for that column in a following row */
		    last = false;
		}
	    }
	  if (last)
            // all cells in the column are selected
	    {
	      // point to the corresponding <col> elements instead of cells
	      pAttr = GetAttrElementWithException (ExcColColRef,
						   FirstSelectedColumn);
	      if (pAttr && pAttr->AeAttrReference &&
		  pAttr->AeAttrReference->RdReferred)
		*firstSel = pAttr->AeAttrReference->RdReferred->ReReferredElem;
	      if (LastSelectedColumn != FirstSelectedColumn)
		{
		  pAttr = GetAttrElementWithException (ExcColColRef,
						       LastSelectedColumn);
		  if (pAttr && pAttr->AeAttrReference &&
		      pAttr->AeAttrReference->RdReferred)
		    *lastSel = pAttr->AeAttrReference->RdReferred->ReReferredElem;
		}
	      else
		*lastSel = *firstSel;
	      return TRUE;
	    }
	}
    }
  return FALSE;
}

/*----------------------------------------------------------------------
  SelColumns
  Select the whole columns between col1 and col2.
  ----------------------------------------------------------------------*/
void SelColumns (PtrElement col1, PtrElement col2)
{
  PtrElement          pNextRow, pCell;
  PtrElement          pFirst, pLast, pRow, pTable;
  int                 rowType, back;
  NotifyElement       notifyEl;
  Document            doc;
  ThotBool            result;
  
  /* get the table ancestor first */
  pTable = col1;
  while (pTable && !TypeHasException (ExcIsTable, pTable->ElTypeNumber,
                                      pTable->ElStructSchema))
    pTable = pTable->ElParent;
  if (!pTable)
    /* we are not in a table... */
    return;
  /* get the first row of the table */
  rowType = GetElemWithException (ExcIsRow, col1->ElStructSchema);
  pRow = FwdSearchTypedElem (col1, rowType, col1->ElStructSchema, NULL);
  /* get the relevant cell in the first row */
  do
    {
      pCell = GetCellInRow (pRow, col1, TRUE, &back);
      if (pCell && back > 0)
        /* in this row the cell is a horizontally extended cell from a previous
           column. skip it */
        pCell = NULL;
      if (!pCell)
        pRow = NextRowInTable (pRow, pTable);
    }
  while (pRow && !pCell);
  pFirst = pCell;
  /* get the last row of the table */
  pNextRow = pRow;
  do
    {
      pRow = pNextRow;
      pNextRow = NextRowInTable (pRow, pTable);
    }
  while (pNextRow);
  /* get the relevant cell in the last row */
  do
    {
      pCell = GetCellInRow (pRow, col2, TRUE, &back);
      if (pCell && back > 0)
        /* in this row the cell is a horizontally extended cell from a previous
           column. skip it */
        pCell = NULL;
      if (!pCell)
        pRow = PreviousRowInTable (pRow, pTable);
    }
  while (pRow && !pCell);
  pLast = pCell;
  
  doc = IdentDocument (SelectedDocument);
  if (pFirst && pFirst == pLast)
    SelectElementWithEvent (SelectedDocument, pFirst, TRUE, FALSE);
  else
    {
      SelectElementWithEvent (SelectedDocument, pFirst, TRUE, FALSE);
      /* send event TteElemExtendSelect.Pre to the application*/
      notifyEl.event = TteElemExtendSelect;
      notifyEl.document = doc;
      notifyEl.element = (Element) pLast;
      notifyEl.info = 0; /* not sent by undo */
      notifyEl.elementType.ElTypeNum = pLast->ElTypeNumber;
      notifyEl.elementType.ElSSchema = (SSchema) (pLast->ElStructSchema);
      notifyEl.position = 0;
      result = CallEventType ((NotifyEvent *) &notifyEl, TRUE);
      if (!result)
        /* application accepts selection */
        {
          /* do select */
          DoExtendSelection (pLast, 0, FALSE, TRUE, FALSE, TRUE, FALSE); /* the
          last parameter should be TRUE, but we do not want multiple columns
          to be selected as long as commands Cut, Copy and Paste can not
          handle multiple columns at a time */
          
          /* send event TteElemExtendSelect.Post to the application */
          notifyEl.event = TteElemExtendSelect;
          notifyEl.document = doc;
          notifyEl.element = (Element) pLast;
          notifyEl.elementType.ElTypeNum = pLast->ElTypeNumber;
          notifyEl.info = 0; /* not sent by undo */
          notifyEl.elementType.ElSSchema = (SSchema) (pLast->ElStructSchema);
          notifyEl.position = 0;
          CallEventType ((NotifyEvent *) & notifyEl, FALSE);
        }
    }

  WholeColumnSelected = TRUE;
  FirstSelectedColumn = col1;
  LastSelectedColumn = col2;
  // force the update of the attributes
  TtaUpdateAttrMenu (doc);
  TtaSetStatusSelectedElement (doc, 1, (Element) FirstSelectedElement);  
}

/*----------------------------------------------------------------------
  TtaGetColumn
  Returns the column element which includes the element el.
  ----------------------------------------------------------------------*/
Element TtaGetColumn (Element el)
{
  PtrElement          pEl;
  
  /* get the cell ancestor first */
  if (el == NULL)
    return NULL;
  else
    pEl = (PtrElement) el;
  while (pEl &&
         !TypeHasException (ExcIsCell,
                            pEl->ElTypeNumber,
                            pEl->ElStructSchema))
    pEl = pEl->ElParent;
  if (pEl)
    pEl = GetColHeadOfCell (pEl);
  return ((Element) pEl);
}

/*----------------------------------------------------------------------
  TtaSelectEnclosingColumn
  Select the enclosing column of the element.
  ----------------------------------------------------------------------*/
void TtaSelectEnclosingColumn (Element el)
{
  el = TtaGetColumn (el);
  if (el)
    SelColumns ((PtrElement) el, (PtrElement) el);
}

/*----------------------------------------------------------------------
  SelectAround
  Select an element relatively to current selection:
  - parent of first selected element if val = 1
  - previous element if val = 2
  - next element if val = 3
  - child of first selected element if val = 4
  ----------------------------------------------------------------------*/
void SelectAround (int val)
{
  PtrElement          pEl, pParent, pFirst, pLast, firstParent, lastParent,
                      pRow, pRow1, pRow2, pCol, pTable;
  int                 nRows, nCols;
  ThotBool            done;

  pEl = NULL;
  pLast = NULL;
  pFirst = NULL;
  firstParent = NULL;
  lastParent = NULL;
  switch (val)
    {
    case 1:
      /* select parent */
      if (SelectPairInterval ())
        pEl = NULL;
      else
        {
          if (DocSelectedAttr && AbsBoxSelectedAttr)
            {
              /* current selection is within an attribute value */
              /* select the element to which the attribute is */
              /* attached */
              if (FirstSelectedCharInAttr != 1 &&
                  LastSelectedCharInAttr <= AbsBoxSelectedAttr->AbVolume)
                SelectStringInAttr (DocSelectedAttr, AbsBoxSelectedAttr,
                                    1, AbsBoxSelectedAttr->AbVolume + 1, TRUE);
              else
                SelectElementWithEvent (DocSelectedAttr,
                                        AbsBoxSelectedAttr->AbElement, TRUE, FALSE);
            }
          else if (StructSelectionMode)
            pEl = SelMenuParentEl;
          else
            {
              if (FirstSelectedColumn)
                /* we are in column selection mode */
                {
                  if (WholeColumnSelected)
                    /* the whole columns are already selected. Select the
                       enclosing table */
                    {
                      pParent = FirstSelectedColumn->ElParent;
                      while (pParent && !TypeHasException (ExcIsTable,
                                                           pParent->ElTypeNumber,
                                                           pParent->ElStructSchema))
                        pParent = pParent->ElParent;
                      if (pParent)
                        pEl = pParent;
                      /* we are no longer in column selection mode */
                      FirstSelectedColumn = NULL;
                      LastSelectedColumn = NULL;
                      WholeColumnSelected = FALSE;
                    }
                  else
                    /* select complete cells or the whole column */
                    {
                      done = FALSE;
                      pFirst = FirstSelectedElement;
                      pLast = LastSelectedElement;
                      if (!TypeHasException (ExcIsCell,
                                             FirstSelectedElement->ElTypeNumber,
                                             FirstSelectedElement->ElStructSchema))
                        /* the first selected element is not a cell. Take the
                           enclosing cell instead */
                        {
                          pParent = FirstSelectedElement->ElParent;
                          while (pParent && !TypeHasException (ExcIsCell,
                                                               pParent->ElTypeNumber,
                                                               pParent->ElStructSchema))
                            pParent = pParent->ElParent;
                          if (pParent)
                            pFirst = pParent;
                          done = TRUE;
                        }
                      if (!TypeHasException (ExcIsCell,
                                             LastSelectedElement->ElTypeNumber,
                                             LastSelectedElement->ElStructSchema))
                        /* the last selected element is not a cell. Take the
                           enclosing cell instead */
                        {
                          pParent = LastSelectedElement->ElParent;
                          while (pParent && !TypeHasException (ExcIsCell,
                                                               pParent->ElTypeNumber,
                                                               pParent->ElStructSchema))
                            pParent = pParent->ElParent;
                          if (pParent)
                            pLast = pParent;
                          done = TRUE;
                        }
                      if (!done)
                        {
                          /* the current selection contains only full cells */
                          if (FirstSelectedColumn == LastSelectedColumn)
                            /* all selected cells are in the same column.
                               Select that column */ 
                            SelColumns (FirstSelectedColumn,
                                        FirstSelectedColumn);
                          else
                            {
                              /* if all selected cells are in the same row,
                                 select that row */
                              /* get the row that contains the first selected
                                 element */
                              pRow1 = FirstSelectedElement->ElParent;
                              while (pRow1 && !TypeHasException (ExcIsRow,
                                                     pRow1->ElTypeNumber,
                                                     pRow1->ElStructSchema))
                                pRow1 = pRow1->ElParent;
                              /* get the row that contains the last
                                 selected element */
                              pRow2 = LastSelectedElement->ElParent;
                              while (pRow2 && !TypeHasException (ExcIsRow,
                                                       pRow2->ElTypeNumber,
                                                       pRow2->ElStructSchema))
                                pRow2 = pRow2->ElParent;
                              if (pRow1 && pRow2 == pRow1)
                                /* all selected cells are in the same row.
                                   Select that row */
                                SelectElementWithEvent (SelectedDocument,
                                                        pRow1, TRUE, FALSE);
                              else
                                {
                                  /* the selected cells are in several rows and
                                  several columns. Check the number of rows and
                                  columns */
                                  pTable = pRow1;
                                  while (pTable &&
                                         !TypeHasException (ExcIsTable,
                                                     pTable->ElTypeNumber,
                                                     pTable->ElStructSchema))
                                    pTable = pTable->ElParent;
                                  if (!pTable)
                                    return;
                                  nRows = 1;
                                  pRow = pRow1;
                                  while (pRow && pRow != pRow2)
                                    {
                                      pRow = NextRowInTable (pRow, pTable);
                                      nRows++;
                                    }
                                  nCols = 1;
                                  pCol = FirstSelectedColumn;
                                  while (pCol && pCol != LastSelectedColumn)
                                    {
                                      pCol = NextColumnInTable (pCol, pTable);
                                      nCols++;
                                    }
                                  if (nCols <= nRows)
                                    /* we should select all columns containing
                                       the current selection:
                                    SelColumns (FirstSelectedColumn,
                                                LastSelectedColumn); */
                                    {
                                    /* but, as commands Copy, Delete and Paste
                                     can not yet handle several columns at a
                                     time, we select multiple rows instead */
                                      SelectElementWithEvent (SelectedDocument,
                                                           pRow1, TRUE, FALSE);
                                      ExtendSelection (pRow2, 0, FALSE, TRUE,
                                                       FALSE);
                                    } 
                                  else
                                    /* select all rows containing the
                                       current selection */
                                    {
                                      SelectElementWithEvent (SelectedDocument,
                                                           pRow1, TRUE, FALSE);
                                      ExtendSelection (pRow2, 0, FALSE, TRUE,
                                                       FALSE);
                                    } 
                                }
                            } 
                          return;
                        }
                    }
                }
              else
                /* we are not in column selection mode: selection is not always
                   "normalized". Select the parent only if the selection is
                   "normalized", normalize it otherwise. */
                {
                  if (FirstSelectedElement && LastSelectedElement)
                    {
                      if (FirstSelectedElement->ElParent ==
                          LastSelectedElement->ElParent)
                        /* selection is normalized. Select the parent */
                        {
                          if (FirstSelectedElement->ElTerminal &&
                              FirstSelectedElement->ElLeafType == LtPicture &&
                              SelPosition)
                            {
                              // select the whole element
                               DoExtendSelection (FirstSelectedElement, 0,
                                                  TRUE,TRUE, TRUE, TRUE, TRUE);
                              return;
                            }
                          else if (FirstSelectedElement->ElParent &&
                                   /* do not go up to the Document element */
                                   FirstSelectedElement->ElParent->ElParent)
                            pEl = FirstSelectedElement->ElParent;
                        }
                      else
                        /* The first and last selected elements are not siblings.
                           Change the selection so that the first and last selected
                           elements be siblings */
                        {
                          pFirst = FirstSelectedElement;
                          pLast = LastSelectedElement;
                          /* get the ancestors of the first selected */
                          /* element */
                          pParent = pFirst->ElParent;
                          while (pParent)
                            {
                              if (ElemIsAnAncestor (pParent, pLast))
                                /* this ancestor (pParent) of the first selected
                                   element is also an ancestor of the last selected
                                   element */
                                {
                                  /* replace the last selected element by its
                                     ancestor whose parent is pParent */
                                  while (pLast->ElParent != pParent)
                                    {
                                      pLast = pLast->ElParent;
                                      if (!TypeHasException (ExcHidden, pLast->ElTypeNumber,
                                                             pLast->ElStructSchema))
                                        lastParent = pLast;
                                    }
                                  /* finished */
                                  if (firstParent && firstParent != pFirst)
                                    pFirst = pParent;
                                  if (lastParent && lastParent != pLast)
                                    pLast = pParent;
                                  pParent = NULL;
                                }
                              else
                                /* this ancestor (pParent) of the first selected
                                   element is not an ancestor of the last selected
                                   element */
                                {
                                  /* replace provisionally the first selected
                                     element by pParent and check whether its
                                     parent is an ancestor of the last selected
                                     element */
                                  pFirst = pParent;
                                  if (!TypeHasException (ExcHidden, pFirst->ElTypeNumber,
                                                         pFirst->ElStructSchema))
                                    firstParent = pFirst;
                                  pParent = pFirst->ElParent;
                                }
                            }
                        }
                    }
                }
              if (!pEl)
                /* the new selection is not a single element */
                {
                  if (pFirst && pFirst == pLast)
                    {
                      if (firstParent && lastParent &&
                          firstParent != lastParent &&
                          (TypeHasException (ExcHidden, pFirst->ElTypeNumber,
                                             pFirst->ElStructSchema) ||
                           firstParent->ElParent == lastParent->ElParent))
                        {
                          SelectElementWithEvent (SelectedDocument, firstParent,
                                                  TRUE, FALSE);
                          /* keep the selection as it is */
                          DoExtendSelection (lastParent, 0, FALSE, TRUE, FALSE,
                                             FALSE, TRUE);
                        }
                      else
                        SelectElementWithEvent (SelectedDocument, pFirst, TRUE,
                                                FALSE);
                    }
                  else
                    {
                      SelectElementWithEvent (SelectedDocument, pFirst, TRUE,
                                              FALSE);
                      ExtendSelection (pLast, 0, FALSE, TRUE, FALSE);
                    }
                }
            }
        }
      break;
    case 2:
      /* Previous */
      pEl = SelMenuPreviousEl;
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
  if (pEl && !ElementIsHidden (pEl))
    SelectElementWithEvent (SelectedDocument, pEl, TRUE, FALSE);
}

/*----------------------------------------------------------------------
  TtcParentElement
  Select the parent of the first selected element
  ----------------------------------------------------------------------*/
void TtcParentElement (Document document, View view)
{
  SelectAround (1);
}

/*----------------------------------------------------------------------
  TtcPreviousElement
  Select the element preceding the first selected element
  ----------------------------------------------------------------------*/
void TtcPreviousElement (Document document, View view)
{
  SelectAround (2);
}

/*----------------------------------------------------------------------
  TtcNextElement
  Select the element following the last selected element
  ----------------------------------------------------------------------*/
void TtcNextElement (Document document, View view)
{
  SelectAround (3);
}

/*----------------------------------------------------------------------
  TtcChildElement
  Select the first child of the first selected element
  ----------------------------------------------------------------------*/
void TtcChildElement (Document document, View view)
{
  SelectAround (4);
}

