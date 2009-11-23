/*
 *
 *  (c) COPYRIGHT INRIA 1997-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * HTML tables
 *
 * Author: I. Vatton (INRIA)
 *
 */
#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "appdialogue.h"
#include "application.h"
#include "frame.h"

#ifdef _GL
#include "glwindowdisplay.h"
#endif /*_GL*/

#define THOT_EXPORT extern
#include "appdialogue_tv.h"
#include "boxes_tv.h"
#include "edit_tv.h"
#include "frame_tv.h"
#include "thotmsg_f.h"

typedef struct _LockRelations *PtrLockRelations;
typedef struct _LockRelations
{
  PtrLockRelations LockRPrev;	/* Previous block */
  PtrLockRelations LockRNext;	/* Next block */
  PtrAbstractBox  LockRTable[MAX_RELAT_DIM];
  PtrAbstractBox  LockRCell[MAX_RELAT_DIM];
  int             LockRFrame[MAX_RELAT_DIM];
} LockRelations;

static PtrLockRelations  DifferedChecks = NULL;
static PtrLockRelations  ActiveChecks = NULL;
static PtrAbstractBox    CheckedTable = NULL;
static PtrAbstractBox    UpdatedParentTable = NULL;
static ThotBool          Lock = FALSE;
static ThotBool          DoUnlock1 = FALSE; // first unlock pass
static ThotBool          DoUnlock2 = FALSE; // second unlock pass

#include "appli_f.h"
#include "attributes_f.h"
#include "boxmoves_f.h"
#include "boxrelations_f.h"
#include "buildboxes_f.h"
#include "buildlines_f.h"
#include "createabsbox_f.h"
#include "displayview_f.h"
#include "editcommands_f.h"
#include "exceptions_f.h"
#include "font_f.h"
#include "frame_f.h"
#include "memory_f.h"
#include "tree_f.h"
#define MAX_COLROW 50

static ThotBool SetCellWidths (PtrAbstractBox cell, PtrAbstractBox table,
                               int frame);

/*----------------------------------------------------------------------
  DifferFormatting registers a differed table formatting in the right
  order: the ancestor before its child.
  ----------------------------------------------------------------------*/
static void DifferFormatting (PtrAbstractBox table, PtrAbstractBox cell,
                              int frame)
{
  PtrLockRelations    pLockRel;
  PtrLockRelations    pPreviousLockRel;
  PtrAbstractBox      pAb;
  int                 i, j;
  ThotBool            toCreate;

  if (table == NULL)
    return;

  /* Look for an empty entry */
  pPreviousLockRel = NULL;
  if (DoUnlock1)
    pLockRel = ActiveChecks;
  else
    pLockRel = DifferedChecks;
  toCreate = TRUE;
  i = 0;
  while (toCreate && pLockRel != NULL)
    {
      i = 0;
      pPreviousLockRel = pLockRel;
      while (i < MAX_RELAT_DIM && pLockRel->LockRTable[i] != NULL)
        {
          if (pLockRel->LockRTable[i] == table)
            {
              /* The table is already registered */
              if (pLockRel->LockRCell[i] != cell)
                pLockRel->LockRCell[i] = NULL;
              return;
            }
          else if (IsParentBox (table->AbBox, pLockRel->LockRTable[i]->AbBox))
            {
              /* exchange information for managing enclosing tables first */
              pAb = pLockRel->LockRTable[i];
              pLockRel->LockRTable[i] = table;
              table = pAb;
              j = pLockRel->LockRFrame[i];
              pLockRel->LockRFrame[i] = frame;
              frame = j;
              pAb = pLockRel->LockRCell[i];
              pLockRel->LockRCell[i] = cell;
              cell = pAb;
            }
          i++;
        }
      
      if (i == MAX_RELAT_DIM)
        /* next block */
        pLockRel = pLockRel->LockRNext;
      else
        toCreate = FALSE;
    }

  if (toCreate)
    {
      /* Create a new block */
      i = 0;
      pLockRel = (PtrLockRelations)TtaGetMemory (sizeof (LockRelations));
      memset (pLockRel, 0, sizeof (LockRelations));
      if (pPreviousLockRel == NULL)
        DifferedChecks = pLockRel;
      else
        {
          pPreviousLockRel->LockRNext = pLockRel;
          pLockRel->LockRPrev = pPreviousLockRel;
        }
    }

  pLockRel->LockRTable[i] = table;
  pLockRel->LockRFrame[i] = frame;
  pLockRel->LockRCell[i] = cell;
}


/*----------------------------------------------------------------------
  IsDifferredTable
  Return TRUE if this table is already registered.        
  ----------------------------------------------------------------------*/
static ThotBool IsDifferredTable (PtrAbstractBox table, PtrAbstractBox cell,
                                  int frame)
{
  PtrLockRelations    pLockRel;
  int                 i, ref;

  if (table == NULL)
    return FALSE;

  /* Check if the table is already registered */
  if (DoUnlock1 || DoUnlock2)
    pLockRel = ActiveChecks;
  else
    pLockRel = DifferedChecks;
  i = 0;
  ref = -1;
  while (pLockRel != NULL)
    {
      i = 0;
      while (i < MAX_RELAT_DIM)
        {
          if (CheckedTable && pLockRel->LockRTable[i] == CheckedTable)
            {
              /* position of the current checked table in the list */
              ref = i;
              i++;
            }
          else if (pLockRel->LockRTable[i] == table &&
                   (pLockRel->LockRCell[i] == NULL || pLockRel->LockRCell[i] == cell))
            {
              /* the table is already registered */
              if (DoUnlock2 && CheckedTable && ref == -1 &&
                  UpdatedParentTable == NULL)
                // the table is already checked but must be updated
                UpdatedParentTable = table;
              return TRUE;
            }
          else if (DoUnlock1 && pLockRel->LockRTable[i] &&
                   !IsParentBox (table->AbBox, pLockRel->LockRTable[i]->AbBox))
            /* another differed table is enclosed by the same table */
            {
              /* register that table before the current position */
              DifferFormatting (table, NULL, frame);
              return TRUE;
            }
          else
            i++;
        }
      /* next block */
      pLockRel = pLockRel->LockRNext;
    }
  if (DoUnlock1 && ref != -1)
    {
      /* register that table before the current position */
      DifferFormatting (table, cell, frame);
      return TRUE;
    }
  else
    return FALSE;
}

/*----------------------------------------------------------------------
  NextSiblingAbsBox returns the next sibling or the next sibling of a parent.
  When pRoot is not Null, the returned abstract box has to be included
  within the pRoot.
  ----------------------------------------------------------------------*/
static PtrAbstractBox NextSiblingAbsBox (PtrAbstractBox pAb, PtrAbstractBox pRoot)
{
  ThotBool           skip;

  if (pAb->AbNext != NULL)
    /* next sibling */
    pAb = pAb->AbNext;
  else
    {
      /* next sibling of a parent */
      skip = TRUE;
      while (skip)
        {
          if (pAb->AbNext != NULL)
            {
              pAb = pAb->AbNext;
              skip = FALSE;
            }
          else if (pAb->AbEnclosing == pRoot)
            {
              pAb = NULL;
              skip = FALSE;
            }
          else
            pAb = pAb->AbEnclosing;
        }	      
    }
  return (pAb);
}


/*----------------------------------------------------------------------
  GetTaRBlock allocates table relations
  ----------------------------------------------------------------------*/
static void GetTaRBlock (PtrTabRelations *pBlock)
{
  PtrTabRelations   pNewBlock;
  int               i;

  pNewBlock = (PtrTabRelations) TtaGetMemory (sizeof (TabRelations));
  /* Initialisation du bloc */
  *pBlock = pNewBlock;
  if (pNewBlock)
    {
      memset (pNewBlock, 0, sizeof (TabRelations));
      pNewBlock->TaRNext = NULL;
      
      for (i = 0; i < MAX_RELAT_DIM; i++)
        pNewBlock->TaRTable[i] = NULL;
    }
}

/*----------------------------------------------------------------------
  FreeTaRBlock frees table relations
  ----------------------------------------------------------------------*/
static void FreeTaRBlock (PtrTabRelations pBlock)
{
  if (pBlock != NULL)
    {
      FreeTaRBlock (pBlock->TaRNext);
      TtaFreeMemory (pBlock);
    }
}

/*----------------------------------------------------------------------
  BuildColOrRowList builds the list of columns or rows included within
  a table
  ----------------------------------------------------------------------*/
static void BuildColOrRowList (PtrAbstractBox table, BoxType colrow)
{
  PtrTabRelations     pTabRel, pOldTabRel;
  PtrTabRelations     pPreviousTabRel;
  PtrAbstractBox      pAb;
  int                 i, j;
  ThotBool            loop;
  ThotBool            empty;

  /* select the rigth list */
  i = 0;
  pPreviousTabRel = NULL;
  if (colrow == BoColumn && table->AbBox->BxColumns != NULL)
    {
      pOldTabRel = table->AbBox->BxColumns;
      table->AbBox->BxColumns = NULL;
    }
  else if (colrow == BoRow && table->AbBox->BxRows != NULL)
    {
      pOldTabRel = table->AbBox->BxRows;
      table->AbBox->BxRows = NULL;
    }
  else
    pOldTabRel = NULL;

  /* search each column or row box within the table */
  pAb = table;
  while (pAb)
    {
      if (pAb->AbBox && !pAb->AbDead &&
          (pAb->AbBox->BxType == BoRow || pAb->AbBox->BxType == BoColumn))
        /* skip over the element contents */
        pAb = NextSiblingAbsBox (pAb, table);
      else
        pAb = SearchNextAbsBox (pAb, table);

      if (pAb && pAb->AbBox &&
          pAb->AbBox->BxType == BoTable && !pAb->AbPresentationBox)
        /* it's an included table, skip over this table */
        pAb = NextSiblingAbsBox (pAb, table);

      if (pAb && pAb->AbBox)
        {
          if (pAb->AbBox->BxType == BoRow && colrow == BoColumn)
            /* stop the process */
            pAb = NULL;	    
          else if (table->AbBox->BxColumns != NULL && colrow == BoColumn &&
                   pAb->AbElement->ElTypeNumber == PageBreak + 1)
            /* stop the process */
            pAb = NULL;
          else if (!pAb->AbDead && pAb->AbBox->BxType == colrow)
            {
              /* link the column or row to the table element */
              pAb->AbBox->BxTable = (PtrTabRelations) table;
              /* add a new item into the list */
              loop = TRUE;
              if (colrow == BoColumn)
                pTabRel = table->AbBox->BxColumns;
              else
                pTabRel = table->AbBox->BxRows;
              while (loop && pTabRel != NULL)
                {
                  i = 0;
                  pPreviousTabRel = pTabRel;
                  do
                    {
                      empty = (pTabRel->TaRTable[i] == NULL);
                      i++;
                    }
                  while (i != MAX_RELAT_DIM && !empty);
		  
                  if (empty)
                    {
                      loop = FALSE;
                      i--;
                    }
                  else
                    pTabRel = pTabRel->TaRNext;
                }
	  
              /* do we need to create a block ? */
              if (loop)
                {
                  GetTaRBlock (&pTabRel);
                  if (pPreviousTabRel == NULL)
                    if (colrow == BoColumn)
                      table->AbBox->BxColumns = pTabRel;
                    else
                      table->AbBox->BxRows = pTabRel;
                  else
                    pPreviousTabRel->TaRNext = pTabRel;
                  i = 0;
                }
              pTabRel->TaRTable[i] = pAb;
            }
        }
    }

  if (pOldTabRel != NULL)
    {
      if (colrow == BoColumn)
        {
          /* transmit known width and percent of previous columns */
          pTabRel = table->AbBox->BxColumns;
          while (pTabRel != NULL)
            {
              for (i = 0; i < MAX_RELAT_DIM && pTabRel->TaRTable[i] != NULL;  i++)
                {
                  pPreviousTabRel = pOldTabRel;
                  loop = TRUE;
                  while (pPreviousTabRel != NULL && loop)
                    {
                      j = 0;
                      while (loop && j < MAX_RELAT_DIM &&
                             pPreviousTabRel->TaRTable[j] != NULL)
                        {
                          if (pPreviousTabRel->TaRTable[j] == pTabRel->TaRTable[i])
                            {
                              loop = FALSE;
                              pTabRel->TaRTWidths[i] = pPreviousTabRel->TaRTWidths[j];
                              pTabRel->TaRTPercents[i] = pPreviousTabRel->TaRTPercents[j];
                            }
                          j++;
                        }
                      pPreviousTabRel = pPreviousTabRel->TaRNext;
                    }
                }
              pTabRel = pTabRel->TaRNext;
            }
        }
      FreeTaRBlock (pOldTabRel);
    }
}


/*----------------------------------------------------------------------
  GiveAttrWidth returns TRUE if the element width is given
  by an attribute.
  The parameter width returns the value of the attribute which has
  exception ExcNewWidth or 0.
  The parameter percent returns the value of the attribute which has
  exception ExcNewPercentWidth or 0
  ----------------------------------------------------------------------*/
ThotBool GiveAttrWidth (PtrAbstractBox pAb, int zoom, int *width, int *percent)
{
  PtrAbstractBox      pRef;
  PtrAttribute        pAttr;
  ThotBool            found;

  *width = 0;
  *percent = 0;
  found = FALSE;
  pAttr = GetAttrElementWithException (ExcNewWidth, pAb->AbElement);
  if (pAttr)
    {
      *width = PixelValue (pAttr->AeAttrValue, UnPixel, NULL, zoom);
      found = TRUE;
    }
  else
    {
      pAttr = GetAttrElementWithException (ExcNewPercentWidth, pAb->AbElement);
      if (pAttr)
        {
          *percent = pAttr->AeAttrValue;
          found = TRUE;
        }
    }

  /* these values can be overwritten by CSS rules on table and cells */
  if (pAb->AbBox && pAb->AbBox->BxType != BoColumn)
    {
      if (!pAb->AbWidth.DimIsPosition && pAb->AbWidth.DimUnit == UnPercent)
        {
          found = TRUE;
          *width = 0;
          *percent = pAb->AbWidth.DimValue;
        }
      else if (!pAb->AbWidth.DimIsPosition && pAb->AbWidth.DimValue > 0)
        {
          found = TRUE;
          pRef = pAb->AbEnclosing;
          *width = PixelValue (pAb->AbWidth.DimValue, pAb->AbWidth.DimUnit, pRef, zoom);
          *percent = 0;
        }
    }
  return (found);
}

/*----------------------------------------------------------------------
  CheckRowHeights checks row-spanned cells with related rows.
  ----------------------------------------------------------------------*/
static void CheckRowHeights (PtrAbstractBox table, int frame)
{
  PtrAttribute        pAttr;
  PtrSSchema          pSS = NULL;
  PtrDocument         pDoc;
  PtrTabSpan          pTabSpan;
  PtrAbstractBox      cell, row;
  PtrAbstractBox      rowList[MAX_COLROW];
  PtrBox              box;
  PtrTabRelations     pTabRel;
  Document            doc;
  int                 rowHeight[MAX_COLROW], rowSpan[MAX_COLROW];
  int                 i, j, irow;
  int                 sum, delta;
  int                 attrHeight = 0;
  ThotBool            modified, computeBBoxes = FALSE;

  if (table->AbBox == NULL ||
      table->AbBox->BxCycles != 0 || table->AbBox->BxSpans == NULL)
    /* the table formatting is currently in process */
    return;
  doc = FrameTable[frame].FrDoc;
  modified = TtaIsDocumentModified (doc);
  pDoc = LoadedDocument[doc - 1];
  pSS = table->AbElement->ElStructSchema;
  attrHeight = GetAttrWithException (ExcNewHeight, pSS);
  if (attrHeight == 0)
    /* there is no attribute to force the height */
    return;

  /* give the complete list of rows */
  pTabRel = table->AbBox->BxRows;
  irow = 0;
  while (pTabRel && irow < MAX_COLROW)
    {
      j = 0;
      while ( j < MAX_RELAT_DIM && pTabRel->TaRTable[j] && irow < MAX_COLROW)
        {
          rowList[irow] = pTabRel->TaRTable[j];
          rowHeight[irow] = 0;
          row = rowList[irow];
          if (row && row->AbElement)
            pAttr = row->AbElement->ElFirstAttr;
          else
            pAttr = NULL;
          while (pAttr)
            {
              if (pAttr->AeAttrNum == attrHeight &&
                  pAttr->AeAttrSSchema == pSS &&
                  (pAttr->AeAttrType == AtNumAttr || pAttr->AeAttrType == AtEnumAttr))
                {
                  RemoveAttribute (row->AbElement, pAttr);
                  DeleteAttribute (row->AbElement, pAttr);
                  /* update the row box */
                  row->AbHeight.DimAbRef = NULL;
                  row->AbHeight.DimValue = -1;
                  row->AbHeightChange = TRUE;
                  ComputeUpdates (row, frame, &computeBBoxes);
                  pAttr = NULL;
                }
              else
                pAttr = pAttr->AeNext;
            }
          rowSpan[irow] = 1;
          irow++;
          j++;
        }    
      pTabRel = pTabRel->TaRNext;
    }

  /* manage spanned columns */
  ComputeEnclosing (frame);
  pTabSpan = table->AbBox->BxSpans;
  while (pTabSpan)
    {
      for (i = 0; i < MAX_RELAT_DIM; i++)
        {
          cell = pTabSpan->TaSpanCell[i];
          if (cell && cell->AbBox && pTabSpan->TaSpanNumber[i] > 1)
            {
              box = cell->AbBox;
              /* get the first row */
              row = SearchEnclosingType (cell, BoRow, BoRow, BoRow);
              j = 0;
              while (j < irow && row != rowList[j])
                j++;
              sum = box->BxHeight;
              //if (row->AbBox)
              /* add the extra space on top of the cell */
              //sum = sum + box->BxYOrg - row->AbBox->BxYOrg;
              if (j + pTabSpan->TaSpanNumber[i] > irow)
                /* update the span value if necessary */
                pTabSpan->TaSpanNumber[i] = irow - j;
              if (j < irow && rowHeight[j] < box->BxHeight)
                {
                  /* row found: store information */
                  rowHeight[j] = sum;
                  rowSpan[j] = pTabSpan->TaSpanNumber[i];
                }
            }
        }
      pTabSpan = pTabSpan->TaSpanNext;
    }

  /* first manage differed enclosing rules */
  for (j = 0; j < irow; j++)
    {
      if (rowHeight[j])
        {
          /* compare the cell height with rows heights */
          sum = rowHeight[j];
          for (i = 0; i < rowSpan[j]; i++)
            if (rowList[j + i]->AbBox)
              {
                row = rowList[j + i];
                sum -= row->AbBox->BxHeight;
                if (i > 0)
                  {
                    /* add extra space between two boxes */
                    box = rowList[j + i - 1]->AbBox;
                    delta = box->BxYOrg + box->BxHeight - row->AbBox->BxYOrg;
                    sum += delta;
                  }
              }
          if (sum > 0)
            {
              sum = (sum + rowSpan[j] - 1) / rowSpan[j];
              /* create the attribute for these rows */
              for (i = 0; i < rowSpan[j]; i++)
                {
                  row = rowList[j + i];
                  if (row->AbBox)
                    {
                      GetAttribute (&pAttr);
                      pAttr->AeAttrSSchema = pSS;
                      pAttr->AeAttrNum = attrHeight;
                      pAttr->AeAttrType = AtNumAttr;
                      pAttr->AeAttrValue = LogicalValue (row->AbBox->BxHeight + sum,
                                                         UnPixel, NULL, ViewFrameTable[frame - 1].FrMagnification);
                      AttachAttrWithValue (row->AbElement, pDoc, pAttr, FALSE);
                      DeleteAttribute (NULL, pAttr);
                      /* update the row box */
                      ComputeUpdates (row, frame, &computeBBoxes);
                    }
                }
            }
        }
    }
      
  if (rowList[0])
    {
      HeightPack (rowList[0]->AbEnclosing, rowList[0]->AbBox, frame);
      HeightPack (table, rowList[0]->AbBox, frame);
    }
  /* Redisplay views */
  RedisplayDocViews (pDoc);

  if (!modified)
    TtaSetDocumentUnmodified (doc);
}


/*----------------------------------------------------------------------
  CheckTableWidths
  Test the coherence between min, max, width constraint and the current
  width.
  The parameter freely lets know that the width can be increased.
  ----------------------------------------------------------------------*/
static void CheckTableWidths (PtrAbstractBox table, int frame, ThotBool freely)
{
  PtrAbstractBox      pCell, pParent;
  PtrAbstractBox     *colBox;
  PtrBox              pBox, box = NULL;
  PtrTabRelations     pTabRel;
  PtrLine             pLine;
  int                *colWidth, *colPercent;
  int                 cNumber, cRef;
  int                 i, delta, n, extra, pixels;
  int                 width, nPercent;
  int                 min, max, sum, parentWidth;
  int                 percent, sumPercent;
  int                 minOfPercent, minOfWidth;
  int                 mbp, var, cellspacing;
  ThotBool            constraint, useMax = FALSE;
  ThotBool            addPixels;

  pBox = table->AbBox;
  /* check if we're really handling a table */
  if (!pBox || pBox->BxType != BoTable)
    return;
  /* Now check the table size */
  if (table->AbBox->BxCycles != 0)
    /* the table formatting is currently in process */
    return;
  if ((DoUnlock1 || DoUnlock2) &&
      CheckedTable && table != CheckedTable &&
      /* accept to reformat enclosed tables */
      !IsParentBox (CheckedTable->AbBox, table->AbBox))
    return;

  /* get the number of columns */
  pTabRel = pBox->BxColumns;
  cNumber = 0;
  while (pTabRel)
    {
      for (i = 0; i < MAX_RELAT_DIM &&
             pTabRel->TaRTable[i] != NULL &&
             pTabRel->TaRTable[i]->AbBox != NULL;  i++)
        cNumber++;
      pTabRel = pTabRel->TaRNext;
    }
  if (cNumber == 0)
    return;
  mbp = pBox->BxLBorder + pBox->BxRBorder;
  if (table->AbLeftMarginUnit != UnAuto && pBox->BxLMargin > 0)
    mbp += pBox->BxLMargin;
  if (table->AbRightMarginUnit != UnAuto && pBox->BxRMargin > 0)
    mbp += pBox->BxRMargin;
  pCell = GetParentCell (pBox);

  /* get the inside table width */
  constraint = GiveAttrWidth (table, ViewFrameTable[frame - 1].FrMagnification,
                              &width, &percent);
  pParent = table->AbEnclosing;
  parentWidth = pParent->AbBox->BxW;
  if (pParent->AbBox->BxType == BoGhost || pParent->AbBox->BxType == BoStructGhost)
    {
      pLine = SearchLine (pBox, frame);
      if (pLine)
        parentWidth = pLine->LiXMax;
    }
  else if (pParent->AbBox->BxType == BoFloatGhost)
    parentWidth = table->AbBox->BxW;

  if (!constraint)
    /* limit given by available space */
    width = parentWidth - mbp;
  else if (percent != 0)
    {
      /* limit given by precent of available space */
      if (percent == 100)
        width = parentWidth - mbp;
      else
        width = (parentWidth * percent / 100);
    }

  if (constraint && width < 20)
    {
      /* limit given by available space */
      width = parentWidth - mbp;
      constraint = pCell == NULL;
    }
  else if (constraint && pCell && width > parentWidth - mbp)
    {
      /* limit given by available space */
      width = parentWidth - mbp;
      constraint = FALSE;
    }

  /* get columns information */
  pTabRel = pBox->BxColumns;
  cRef = 0;
  colBox = (PtrAbstractBox *)TtaGetMemory (sizeof (PtrAbstractBox) * cNumber);
  colWidth = (int *)TtaGetMemory (sizeof (int) * cNumber);
  colPercent = (int *)TtaGetMemory (sizeof (int) * cNumber);
  sumPercent = 0;
  sum = 0;
  minOfPercent = 0;
  minOfWidth = 0;
  min = 0;
  max = 0;
  /* number of unconstrained columns */
  n = 0;
  nPercent = 0;
  /* number of constrained columns by percent */
  while (pTabRel)
    {
      for (i = 0; i < MAX_RELAT_DIM &&
             pTabRel->TaRTable[i] &&
             pTabRel->TaRTable[i]->AbBox;  i++)
        {
          colBox[cRef] = pTabRel->TaRTable[i];
          colWidth[cRef] = pTabRel->TaRTWidths[i];
          colPercent[cRef] = pTabRel->TaRTPercents[i];
          box = colBox[cRef]->AbBox;
          if (colPercent[cRef] && colPercent[cRef] * width / 100 < box->BxMinWidth)
            /* cannot apply that rule */
            colPercent[cRef] = 0;
          if (colPercent[cRef])
            {
              sumPercent += colPercent[cRef];
              /* min and max replaced by a percent value */
              minOfPercent += box->BxMinWidth;
              nPercent++;
            }
          else if (colWidth[cRef])
            {
              sum += colWidth[cRef];
              /* min and max replaced by a width value */
              minOfWidth += box->BxMinWidth;
            }
          else
            {
              min += box->BxMinWidth;
              max += box->BxMaxWidth;
              n++;
            }
          cRef++;
        }
      pTabRel = pTabRel->TaRNext;
    }

  /* take into account the cell spacing */
  if (cNumber > 1 && colBox[1] &&  colBox[1]->AbEnclosing && colBox[1]->AbHorizPos.PosDistance)
    // the distance between 2 columns gives the cellspacing
    cellspacing = colBox[1]->AbHorizPos.PosDistance * (cNumber + 1);
  else
    cellspacing = 0;
  
  width -= cellspacing;
  if (sumPercent > 0)
    {
      minOfPercent = sumPercent * width / 100;
      if (min + sum + minOfPercent > width /*|| minOfPercent < width - sum - max*/)
        {
          delta = width - sum - min;
          /* table contents too narrow */
          for (cRef = 0; cRef < cNumber; cRef++)
            if (colPercent[cRef])
              {
                /* colPercent[cRef] = - new min */
                colPercent[cRef] = - delta * colPercent[cRef] / 100;
                min -= colPercent[cRef];
                if (- colPercent[cRef] > colBox[cRef]->AbBox->BxMaxWidth)
                  colBox[cRef]->AbBox->BxMaxWidth = - colPercent[cRef];
                max += colBox[cRef]->AbBox->BxMaxWidth;
                n++;
              }
          sumPercent = 0;
        }
      else
        sumPercent = minOfPercent;
    }
#ifdef TAB_DEBUG
  printf ("\nCheckTableWidths (%s) %d cols\n", table->AbElement->ElLabel, cNumber);
#endif
  /* now update real widths */
  pBox->BxCycles = 1;
  if (max + sum + sumPercent <= width && !constraint)
    {
#ifdef TAB_DEBUG
      printf ("Maximum Widths ...\n");
#endif
      /* assign the maximum width, or the percent, or the width */
      width = max + sum + sumPercent;
      /* the table width is not constrained by the enclosing box */
      //table->AbWidth.DimAbRef = NULL;
      if (width + cellspacing - pBox->BxW != 0)
        /* we will have to recheck scrollbars */
        AnyWidthUpdate = TRUE;
      /*if (width != pBox->BxW)*/
      /* don't pack rows with each cell */
      PackRows = FALSE;
      ResizeWidth (pBox, pBox, NULL, width + cellspacing - pBox->BxW,
                   0, 0, 0, frame, FALSE);
      for (cRef = 0; cRef < cNumber; cRef++)
        {
          box = colBox[cRef]->AbBox;
          if (colPercent[cRef] > 0)
            delta = (width * colPercent[cRef] / 100);
          else if (colPercent[cRef] < 0)
            delta = - colPercent[cRef];
          else if (colWidth[cRef] > 0)
            delta = colWidth[cRef];
          else
            delta = box->BxMaxWidth;
          /* update the new inside width */
          delta = delta - box->BxW;
          ResizeWidth (box, box, NULL, delta, 0, 0, 0, frame, FALSE);
#ifdef TAB_DEBUG
          printf ("Width[%d]=%d\n", cRef, box->BxWidth);
#endif
        }
    }
  else if (min + sum + sumPercent >= width)
    {
#ifdef TAB_DEBUG
      printf ("Minimum Widths ...\n");
#endif
      /* assign the minimum width, or the percent, or the width */
      width = min + sum + sumPercent;
      /* the table width is constrained by the enclosing box */
      //table->AbWidth.DimAbRef = table->AbEnclosing;
      //table->AbWidth.DimValue = 0;
      if (width + cellspacing - pBox->BxW != 0 && pCell == NULL)
        /* we will have to recheck scrollbars */
        AnyWidthUpdate = TRUE;
      /*if (width != pBox->BxW)*/
      /* don't pack rows with each cell */
      PackRows = FALSE;
      ResizeWidth (pBox, pBox, NULL, width + cellspacing - pBox->BxW,
                   0, 0, 0, frame, FALSE);
      for (cRef = 0; cRef < cNumber; cRef++)
        {
          box = colBox[cRef]->AbBox;
          if (colPercent[cRef] > 0)
            delta = (width * colPercent[cRef] / 100);
          else if (colPercent[cRef] < 0)
            delta = - colPercent[cRef];
          else if (colWidth[cRef] > 0)
            delta = colWidth[cRef];
          else
            delta = box->BxMinWidth;
          /* update the new inside width */
          delta = delta - box->BxW;
          ResizeWidth (box, box, NULL, delta, 0, 0, 0, frame, FALSE);
#ifdef TAB_DEBUG
          printf ("Width[%d]=%d\n", cRef, box->BxWidth);
#endif
        }
    }
  else
    {
#ifdef TAB_DEBUG
      printf ("Specific Widths ...\n");
#endif
      /* assign the specific width to the table */
      if (width + cellspacing - pBox->BxW != 0 && pCell == NULL)
        /* we will have to recheck scrollbars */
        AnyWidthUpdate = TRUE;
      /* the table width is constrained by the enclosing box */
      //table->AbWidth.DimAbRef = table->AbEnclosing;
      /* don't pack rows with each cell */
      PackRows = FALSE;
      ResizeWidth (pBox, pBox, NULL, width + cellspacing - pBox->BxW,
                   0, 0, 0, frame, FALSE);
      /* get the space available for stretchable columns */      
      delta = width - sum - sumPercent;
      /* display with the maximum or the minimum widths */
      pixels = 0;
      if (n == 0)
        /* no stretchable columns */
        n = 1;
      else if (max == 0 || max <= delta)
        {
          /* extend the maximum of each stretchable column */
          useMax = TRUE;
          // check if a column is too narrow
          var = (delta - max) / n;
#ifndef IV
          delta = delta - max;
          pixels = delta - (var * n);
          delta -= pixels;
#else
          i = 0;
          for (cRef = 0; cRef < cNumber; cRef++)
            {
              if (colBox[cRef]->AbBox->BxMaxWidth > var)
                i++;
                }*/
          if (i == 0)
            // all cells have the same width
            max = 0;

          delta = delta - max;
          /* get extra pixels */
          pixels = delta;
          for (cRef = 0; cRef < cNumber; cRef++)
            {
              box = colBox[cRef]->AbBox;
              if (max)
                var = delta * box->BxMaxWidth / max;
              else
                var = delta / n;
              if (colPercent[cRef] == 0 && colWidth[cRef] == 0)
                /* decrease extra pixels */
                pixels -= var;
            }
#endif
        }
      else
        {
          useMax = FALSE;
          delta = delta - min; /* delta to be distributed */
          if (delta)
            {
              /* get extra pixels */
              pixels = delta;
              /* don't increase the min more than the current max */
              /* check if extra space can be found */
              for (cRef = 0; cRef < cNumber; cRef++)
                {
                  box = colBox[cRef]->AbBox;
                  var = delta * box->BxMaxWidth / max;
                  /* check if the delta could be increased */
                  if (colPercent[cRef] < 0 &&
                      var - colPercent[cRef] > box->BxMaxWidth)
                    {
                      /* we'll use the max instead of the min + var */
                      extra = box->BxMaxWidth + colPercent[cRef];
                      pixels -= extra;
                      n--;
                    }
                  else if (colPercent[cRef] == 0 && colWidth[cRef] == 0 &&
                           box->BxMinWidth + var > box->BxMaxWidth)
                    {
                      /* we'll use the max instead of the min + var */
                      extra = box->BxMaxWidth - box->BxMinWidth;
                      pixels -= extra;
                      n--;
                    }
                  else if (colPercent[cRef] < 0 ||
                           (colPercent[cRef] == 0 && colWidth[cRef] == 0))
                    pixels -= var;
                }
            }
        }

      if (n)
        extra = (pixels + n / 2) / n;
      else
        extra = pixels;
      for (cRef = 0; cRef < cNumber; cRef++)
        {
          box = colBox[cRef]->AbBox;
          addPixels = FALSE;
          if (colPercent[cRef] > 0)
            i = ((width - mbp) * colPercent[cRef] / 100);
          else if (colWidth[cRef] > 0)
            i = colWidth[cRef];
          else if (useMax)
            {
              //if (max)
              //  i = delta * box->BxMaxWidth / max + box->BxMaxWidth;
              //else
                i = delta / n;
              i = box->BxMaxWidth + var;
              addPixels = TRUE;
            }
          else
            {
              var = delta * box->BxMaxWidth / max;
              if (colPercent[cRef] < 0 &&
                  var - colPercent[cRef] > box->BxMaxWidth)
                /* we'll use the max instead of the min + var */
                i = box->BxMaxWidth;
              else if (colPercent[cRef] < 0)
                /* colPercent[cRef] = - new min */
                i = var - colPercent[cRef];
              else if (colPercent[cRef] == 0 && colWidth[cRef] == 0 &&
                       box->BxMinWidth + var > box->BxMaxWidth)
                /* use the max instead of the min + delta */
                i = box->BxMaxWidth;
              else
                {
                  i = box->BxMinWidth + var;
                  addPixels = TRUE;
                }
            }
          /* update the new inside width */
          if (addPixels && pixels > 0)
            {
              if (pixels > extra && extra > 0 && cRef < cNumber - 1)
                {
                  i += extra;
                  pixels -= extra;
                }
              else
                {
                  // get all extra pixels
                  i += pixels;
                  pixels = 0;
                }
            }
          i = i - box->BxW;
          ResizeWidth (box, box, NULL, i, 0, 0, 0, frame, FALSE);
#ifdef TAB_DEBUG
          printf ("Width[%d]=%d\n", cRef, box->BxWidth);
#endif
        }
    }

  pTabRel = pBox->BxRows;
  /* update rows */
  if (!PackRows && pTabRel &&
      pTabRel->TaRTable[0]->AbEnclosing &&
      pTabRel->TaRTable[0]->AbEnclosing->AbBox)
    {
      /* pack all rows */
      PackRows = TRUE;
      width = pBox->BxW;
      delta = 0;
      // remove the padding border and margin of included boxes
      pParent = pBox->BxRows->TaRTable[0]->AbEnclosing;
      while (pParent && pParent->AbEnclosing &&
             pParent->AbEnclosing->AbBox && pParent->AbEnclosing->AbBox->BxType != BoTable)
        {
          box = pParent->AbBox;
          delta += box->BxLPadding + box->BxRPadding + box->BxLBorder + box->BxRBorder;
          pParent = pParent->AbEnclosing;
        }
      width -= delta;
      while (pTabRel)
        {
          for (i = 0; i < MAX_RELAT_DIM &&
                 pTabRel->TaRTable[i] != NULL &&
                 pTabRel->TaRTable[i]->AbBox != NULL;  i++)
            {
              box = pTabRel->TaRTable[i]->AbBox;
              mbp = box->BxLPadding + box->BxRPadding + box->BxLBorder + box->BxRBorder;
              ChangeDefaultWidth (box, box, width - mbp, 0, frame);
            }
          pTabRel = pTabRel->TaRNext;
        }
      box = pBox->BxRows->TaRTable[0]->AbEnclosing->AbBox;
      ChangeDefaultWidth (box, box, width, 0, frame);
    }

  /* recheck auto and % margins */
  CheckMBP (table, table->AbBox, frame, TRUE);
  table->AbBox->BxCycles = 0;
#ifdef TAB_DEBUG
  printf("End CheckTableWidths (%s) = %d [%d]\n", table->AbElement->ElLabel,
         table->AbBox->BxWidth, table->AbBox->BxW);
#endif
  TtaFreeMemory (colBox);
  TtaFreeMemory (colWidth);
  TtaFreeMemory (colPercent);
}


/*----------------------------------------------------------------------
  ChangeTableWidth
  The table width changes, we need to propagate the change.
  ----------------------------------------------------------------------*/
void ChangeTableWidth (PtrAbstractBox table, int frame)
{
  /* table formatting in the main view only */
  if (table == NULL || FrameTable[frame].FrDoc == 0)
    return;
  if (FrameTable[frame].FrView != 1)
    return;

  if (Lock)
    /* the table formatting is locked */
    DifferFormatting (table, NULL, frame);
  else if (IsDifferredTable (table, NULL, frame))
    /* the table will be managed later */
    return;
  else
    {
      /* Now check the table size */
      CheckTableWidths (table, frame, FALSE);
      CheckRowHeights (table, frame);
    }
}

/*----------------------------------------------------------------------
  GiveCellWidths returns the minimum width, the maximum width, the
  constrained width and the percent width of a specific cell.
  ----------------------------------------------------------------------*/
static void GiveCellWidths (PtrAbstractBox cell, int frame, int *min, int *max,
                            int *width, int *percent)
{
  PtrAbstractBox      pAb, pParent;
  PtrBox              box, parent;
  PtrSSchema          pSS;
  int                 mbp, delta;
  ThotBool            skip;

  GiveAttrWidth (cell, ViewFrameTable[frame - 1].FrMagnification, width, percent);
  box = cell->AbBox;
  /* take into account the left margin, border and padding */
  mbp = box->BxLBorder + box->BxLPadding;
  if (box->BxType == BoCellBlock)
    {
      // min and max are already computed
      *min = box->BxMinWidth;
      *max = box->BxMaxWidth;      
      if (*width)
        {
          *width = *width + mbp;
          if (*width > *min)
            *min = *width;
          else if (*width < *min)
            *width = *min;
          if (*width > *max)
            *max = *width;
        }
      return;
    }
  if (cell->AbLeftMarginUnit != UnAuto && box->BxLMargin > 0)
    mbp += box->BxLMargin;
  /* process elements in this cell */
  pSS = cell->AbElement->ElStructSchema;
  *min = 1;
  *max = 1;
  skip = FALSE;
  pAb = cell;
  while (pAb)
    {
      if (skip)
        {
          pAb = NextSiblingAbsBox (pAb, cell);
          skip = FALSE;
        }
      else
        pAb = SearchNextAbsBox (pAb, cell);
      if (pAb && !pAb->AbDead &&
          pAb->AbBox && !pAb->AbPresentationBox)
        {
          /* diff between cell's and box's position */
          if (pAb->AbBox->BxHorizEdge == Left ||
              pAb->AbBox->BxHorizEdge == VertRef)
            {
              delta = pAb->AbBox->BxXOrg - box->BxXOrg - mbp;
              if (delta < 0)
                delta = 0;
            }
          else
            delta = 0;

          if (pAb->AbBox->BxType == BoBlock ||
              pAb->AbBox->BxType == BoFloatBlock ||
              pAb->AbBox->BxType == BoTable)
            {
              /* take into account enclosing margins */
              pParent = pAb->AbEnclosing;
              while (pParent && pParent != cell && pParent->AbBox)
                {
                  parent = pParent->AbBox;
                  if (parent->BxType != BoGhost &&
                      parent->BxType != BoStructGhost &&
                      parent->BxType != BoFloatGhost)
                    {
                      delta += parent->BxLBorder + parent->BxLPadding
                        + parent->BxRBorder + parent->BxRPadding;
                      if (pParent->AbLeftMarginUnit != UnAuto || pAb->AbBox->BxLMargin < 0)
                        delta += parent->BxLMargin;
                      if (pParent->AbRightMarginUnit != UnAuto || pAb->AbBox->BxRMargin < 0)
                        delta += parent->BxRMargin;
                    }
                  pParent = pParent->AbEnclosing;
                }
              if (pAb->AbBox->BxMaxWidth < pAb->AbBox->BxMinWidth)
                pAb->AbBox->BxMaxWidth = pAb->AbBox->BxMinWidth;
              if (*min < pAb->AbBox->BxMinWidth + delta)
                *min = pAb->AbBox->BxMinWidth + delta;
              if (*max < pAb->AbBox->BxMaxWidth + delta)
                *max = pAb->AbBox->BxMaxWidth + delta;
              skip = TRUE;
            }
          else if (!pAb->AbWidth.DimIsPosition &&
                   pAb->AbHorizEnclosing &&
                   pAb->AbWidth.DimUnit != UnPercent &&
                   pAb->AbWidth.DimUnit != UnAuto &&
                   (pAb->AbWidth.DimAbRef == NULL ||
                    !IsParentBox (pAb->AbWidth.DimAbRef->AbBox, pAb->AbBox)))
            {
              /* the box width doesn't depend on cell width */
              if (!TypeHasException (ExcPageBreakRepBefore, pAb->AbElement->ElTypeNumber, pSS) &&
                  !TypeHasException (ExcPageBreakRepetition, pAb->AbElement->ElTypeNumber, pSS) &&
                  pAb->AbElement->ElTypeNumber != PageBreak + 1)
                {
                  /* the box is not generated by a page break */
                  if (pAb->AbLeftMarginUnit == UnAuto || pAb->AbBox->BxLMargin < 0)
                    /* ignore auto margins */
                    delta -= pAb->AbBox->BxLMargin;
                  if (pAb->AbRightMarginUnit == UnAuto || pAb->AbBox->BxRMargin < 0)
                    /* ignore auto margins */
                    delta -= pAb->AbBox->BxRMargin;
                  if (*min < pAb->AbBox->BxWidth + delta)
                    *min = pAb->AbBox->BxWidth + delta;
                  if (*max < pAb->AbBox->BxWidth + delta)
                    *max = pAb->AbBox->BxWidth + delta;
                }
              skip = TRUE;
            }
        }
    }
  /* take into account margins, borders, paddings */
  mbp = box->BxLBorder + box->BxLPadding + box->BxRBorder + box->BxRPadding;
  if (box->BxLMargin > 0)
    mbp += box->BxLMargin;
  if (box->BxRMargin > 0)
    mbp += box->BxRMargin;
  *min = *min + mbp;
  *max = *max + mbp;
  if (*width)
    {
      *width = *width + mbp;
      if (*width > *min)
        *min = *width;
      else if (*width < *min)
        *width = *min;
      if (*width > *max)
        *max = *width;
    }
}

/*----------------------------------------------------------------------
  GetCellSpans
  Return the colspan (1 if no span) and rowspan (1 if no span) of a cell.
  If colspanAttr is not NULL, it returns the colspan Attrinute of the cell.
  ----------------------------------------------------------------------*/
void GetCellSpans (PtrElement cell, int *colspan, int *rowspan,
                   PtrAttribute *colspanAttr)
{
  PtrSSchema          pSS;
  PtrAttribute        pAttr;
  int                 attrVSpan, attrHSpan;

  *colspan = 1; /* no col-spanned cell */
  *rowspan = 1; /* no row-spanned cell */
  if (cell)
    {
      pSS = cell->ElStructSchema;
      attrVSpan = GetAttrWithException (ExcRowSpan, pSS);
      attrHSpan = GetAttrWithException (ExcColSpan, pSS);
      if (attrVSpan != 0 || attrHSpan != 0)
        {
          /* is this attribute attached to the cell */
          pAttr = cell->ElFirstAttr;
          while (pAttr)
            {
              if (pAttr->AeAttrNum == attrVSpan &&
                  pAttr->AeAttrSSchema == pSS)
                {
                  /* rowspan on this cell */
                  if (pAttr->AeAttrValue != 1)
                    *rowspan = pAttr->AeAttrValue;
                }
              else if (pAttr->AeAttrNum == attrHSpan &&
                       pAttr->AeAttrSSchema == pSS)
                {
                  if (colspanAttr)
                    *colspanAttr = pAttr;
                  if (pAttr->AeAttrValue != 1)
                    *colspan = pAttr->AeAttrValue;
                }
              pAttr = pAttr->AeNext;
            }
        }
    }
}


/*----------------------------------------------------------------------
  SetTableWidths computes the minimum width and the maximum width of
  all cells, columns and the table itself.
  Store in cell, column, table boxes:
  - BxMinWidth = the minimum width
  - BxMaxWidth = the maximum width
  - TaRTWidth = the contrained width in pixels
  - TaRTPercent = the contrained width in percent
  Return TRUE if any table width is modified
  ----------------------------------------------------------------------*/
ThotBool SetTableWidths (PtrAbstractBox table, int frame)
{
  PtrAttribute        pAttr;
  PtrSSchema          pSS;
  PtrDocument         pDoc;
  PtrTabRelations     pTabRel;
  PtrTabSpan          pTabSpan;
  PtrAbstractBox     *colBox;
  PtrAbstractBox      pAb, row, cell;
  PtrBox              pBox, box, box1, prevBox;
  int                *colWidth, *colPercent, *colVSpan;
  int                 colSpan_MinWidth[MAX_COLROW], colSpan_Percent[MAX_COLROW];
  int                 colSpan_Width[MAX_COLROW], colSpan_MaxWidth[MAX_COLROW];
  int                 colSpan_First[MAX_COLROW], colSpan_Last[MAX_COLROW];
  int                 cNumber, spanNumber, rspanNumber;
  int                 span, delta, j;
  int                 width, i, cRef, mbp;
  int                 min, max, percent, realMin, realMax;
  int                 attrVSpan, attrHSpan;
  int                 cellWidth;
  ThotBool           *colWidthForced;
  ThotBool            skip, change, reformat;
  ThotBool            foundH, foundV;

  pDoc = LoadedDocument[FrameTable[frame].FrDoc - 1];
  pSS = table->AbElement->ElStructSchema;
  /* how many columns */
  pBox = table->AbBox;
  /* check if we're really handling a table */
  if (!pBox || pBox->BxType != BoTable)
    return FALSE;
  pTabRel = pBox->BxColumns;
  cNumber = 0;
  change = FALSE;
  span = 1;
  width = 0;
  while (pTabRel != NULL)
    {
      for (i = 0; i < MAX_RELAT_DIM &&
             pTabRel->TaRTable[i] != NULL &&
             pTabRel->TaRTable[i]->AbBox != NULL;  i++)
        cNumber++;
      pTabRel = pTabRel->TaRNext;
    }
  if (cNumber == 0)
    return (change);

#ifdef TAB_DEBUG
  printf ("\nSetTableWidths (%s) %d cols\n", table->AbElement->ElLabel, cNumber);
#endif
  /* remove the list of vertical spanned cells */
  pTabSpan = pBox->BxSpans;
  while (pTabSpan)
    {
      pBox->BxSpans = pTabSpan->TaSpanNext;
      TtaFreeMemory (pTabSpan);
      pTabSpan = pBox->BxSpans;
    }

  /* register widths of each columns */
  pTabRel = pBox->BxColumns;
  cRef = 0;
  colBox = (PtrAbstractBox*)TtaGetMemory (sizeof (PtrAbstractBox) * cNumber);
  colWidth = (int*)TtaGetMemory (sizeof (int) * cNumber);
  colPercent = (int*)TtaGetMemory (sizeof (int) * cNumber);
  colVSpan = (int*)TtaGetMemory (sizeof (int) * cNumber);
  colWidthForced = (ThotBool*)TtaGetMemory (sizeof (ThotBool) * cNumber);
  while (pTabRel)
    {
      for (i = 0; i < MAX_RELAT_DIM &&
             pTabRel->TaRTable[i] != NULL &&
             pTabRel->TaRTable[i]->AbBox != NULL;  i++)
        {
          colBox[cRef] = pTabRel->TaRTable[i];
          colBox[cRef]->AbBox->BxMinWidth = 0;
          colBox[cRef]->AbBox->BxMaxWidth = 0;
          // check if col or colgroup elements are defined
          colWidth[cRef] = 0;
          colPercent[cRef] = 0;
          colWidthForced[cRef] = GiveAttrWidth (colBox[cRef],
                                                ViewFrameTable[frame - 1].FrMagnification,
                                                &colWidth[cRef], &colPercent[cRef]);
          colVSpan[cRef] = 0;
          cRef++;
        }
      pTabRel = pTabRel->TaRNext;
    }

  attrVSpan = GetAttrWithException (ExcRowSpan, pSS);
  attrHSpan = GetAttrWithException (ExcColSpan, pSS);
  spanNumber = 0; /* no col-spanned cell */
  rspanNumber = 0; /* no row-spanned cell */
  /* process all rows */
  pTabRel = pBox->BxRows;
  while (pTabRel != NULL)
    {
      for (i = 0; i < MAX_RELAT_DIM && pTabRel->TaRTable[i]; i++)
        {
          /* process all cells in the row */
          row = pTabRel->TaRTable[i];
          cRef = 0;
          pAb = row;
          skip = FALSE;
          while (pAb && cRef < cNumber)
            {
              if (skip)
                {
                  pAb = NextSiblingAbsBox (pAb, row);
                  skip = FALSE;
                }
              else
                pAb = SearchNextAbsBox (pAb, row);
	      
              if (pAb && !pAb->AbDead && pAb->AbBox &&
                  TypeHasException (ExcIsCell, pAb->AbElement->ElTypeNumber, pSS) &&
                  !pAb->AbPresentationBox)
                {
                  /* it is a cell element */
                  /* is there any rowspan attribute in previous row */
                  while (colVSpan[cRef] > 0 && cRef < cNumber)
                    {
                      colVSpan[cRef]--;
                      cRef++;
                    }
                  if (cRef >= cNumber)
                    pAb = NULL;
                  //manage the cell only if the column width is not forced
                  if (pAb && !colWidthForced[cRef])
                    {
                      /* is it vertically or horizontally spanned ? */
                      span = 1;
                      if (attrVSpan != 0 || attrHSpan != 0)
                        {
                          /* is this attribute attached to the cell */
                          pAttr = pAb->AbElement->ElFirstAttr;
                          foundH = FALSE;
                          foundV = FALSE;
                          while ((!foundH || !foundV) && pAttr)
                            {
                              if (pAttr->AeAttrNum == attrVSpan &&
                                  pAttr->AeAttrSSchema == pSS)
                                {
                                  if (pAttr->AeAttrType == AtEnumAttr || pAttr->AeAttrType == AtNumAttr)
                                    {
                                      /* rowspan on this cell */
                                      if (pAttr->AeAttrValue == 0)
                                        colVSpan[cRef] = 9999; /*****/
                                      else
                                        colVSpan[cRef] = pAttr->AeAttrValue - 1;
                                      if (colVSpan[cRef] > 0 && rspanNumber < MAX_COLROW)
                                        {
                                          /* register current cell and span value */
                                          if (pTabSpan == NULL)
                                            {
                                              rspanNumber = 0;
                                              pTabSpan = (TabSpan*)TtaGetMemory (sizeof (TabSpan));
                                              memset (pTabSpan, 0, sizeof (TabSpan));
                                              pBox->BxSpans = pTabSpan;
                                            }
                                          else if (rspanNumber< MAX_RELAT_DIM)
                                            {
                                              rspanNumber = 0;
                                              pTabSpan->TaSpanNext = (TabSpan*)TtaGetMemory (sizeof (TabSpan));
                                              pTabSpan = pTabSpan->TaSpanNext;
                                              memset (pTabSpan, 0, sizeof (TabSpan));
                                            }
                                          if (pAttr->AeAttrValue == 0)
                                            pTabSpan->TaSpanNumber[rspanNumber] = 9999;
                                          else
                                            pTabSpan->TaSpanNumber[rspanNumber] = pAttr->AeAttrValue;
                                          pTabSpan->TaSpanCell[rspanNumber++] = pAb;
                                          foundV = TRUE;
                                        }
                                    }
                                }
                              if (pAttr->AeAttrNum == attrHSpan &&
                                  pAttr->AeAttrSSchema == pSS)
                                {
                                  foundH = TRUE;
                                  if (pAttr->AeAttrType == AtEnumAttr ||
                                      pAttr->AeAttrType == AtNumAttr)
                                    {
                                      if (pAttr->AeAttrValue == 0)
                                        span = cNumber - cRef;
                                      else if (pAttr->AeAttrValue > 1)
                                        /* ignore values equal to 1 */
                                        span = pAttr->AeAttrValue;
                                      /* it could be an invalid span */
                                      if (cRef + span > cNumber)
                                        span = cNumber - cRef;
                                    }
                                }
                              pAttr = pAttr->AeNext;
                            }
                        }

                      cell = pAb;
                      box = cell->AbBox;
                      /* get the min and max and constrained widths */
                      GiveCellWidths (cell, frame, &min, &max, &cellWidth, &percent);
                      if (percent == 100 && cNumber > 1)
                        // this rule cannot apply: ignore
                        percent = 0;
                      if (box->BxMinWidth != min)
                        box->BxMinWidth = min;
                      if (box->BxMaxWidth != max)
                        box->BxMaxWidth = max;
                      box->BxRuleWidth = cellWidth;
                      /* update the min and max of the column */
                      if (span > 1 && spanNumber < MAX_COLROW)
                        {
                          colSpan_MinWidth[spanNumber] = min;
                          colSpan_MaxWidth[spanNumber] = max;
                          if (cellWidth >= min)
                            colSpan_Width[spanNumber] = cellWidth;
                          else
                            colSpan_Width[spanNumber] = 0;
                          colSpan_Percent[spanNumber] = percent;
                          colSpan_First[spanNumber] = cRef;
                          colSpan_Last[spanNumber] = cRef + span - 1;
                          spanNumber ++;
                          if (colVSpan[cRef] > 0)
                            {
                              /* propagate vertical span */
                              for (delta = 1; delta < span; delta++)
                                if (cRef + delta < cNumber)
                                  colVSpan[cRef + delta] = colVSpan[cRef];
                            }
                        }
                      else
                        {
                          /* diff between the column and cell widths */
                          /*delta = colBox[cRef]->AbBox->BxWidth - box->BxWidth;
                            if (delta < 0)*/
                          delta = 0;
                          if (colBox[cRef]->AbBox->BxMinWidth < min + delta)
                            colBox[cRef]->AbBox->BxMinWidth = min + delta;
                          if (colBox[cRef]->AbBox->BxMaxWidth < max + delta)
                            colBox[cRef]->AbBox->BxMaxWidth = max + delta;
                          if (colWidth[cRef] < cellWidth + delta)
                            colWidth[cRef] = cellWidth + delta;
                          if (colPercent[cRef] < percent)
                            colPercent[cRef] = percent;
                        }
                    }
                  /* next column */
                  skip = TRUE;
                  cRef += span;
                }
            }
          /* check missing cells */
          while (cRef < cNumber)
            {
              if (colVSpan[cRef] > 0)
                colVSpan[cRef]--;
              cRef++;
            }
        }
      pTabRel = pTabRel->TaRNext;
    }

  /* take spanned cells into account */
  for (i = 0; i < spanNumber; i++)
    {
      min = 0;
      max = 0;
      realMin = 0;
      realMax = 0;
      percent = 0;
      span = 0;
      width = 0;
      mbp = 0;
      j = 0;
      for (cRef = colSpan_First[i]; cRef <= colSpan_Last[i]; cRef++)
        {
          if (colPercent[cRef] == 0 && colWidth[cRef] == 0)
            {
              realMin += colBox[cRef]->AbBox->BxMinWidth;
              realMax += colBox[cRef]->AbBox->BxMaxWidth;
              span++;
            }
          else if (colWidth[cRef])
            {
              realMin += colWidth[cRef];
              realMax += colWidth[cRef];
            }
          else
            {
              realMin += colBox[cRef]->AbBox->BxMinWidth;
              realMax += colBox[cRef]->AbBox->BxMaxWidth;
              percent += colPercent[cRef];
            }
        }
	  
      /* compare min and max values */
      if (colSpan_MinWidth[i] > realMin)
        {
          /* change width of included columns */
          width = colSpan_MinWidth[i] - realMin;
          if (span > 0)
            width = (width + span - 1) / span;
          else
            {
              delta = colSpan_Last[i] - colSpan_First[i] + 1;
              width = (width + delta - 1) / delta;
            }
          for (cRef = colSpan_First[i]; cRef <= colSpan_Last[i]; cRef++)
            if ((colPercent[cRef] == 0 && colWidth[cRef] == 0) || span == 0)
              colBox[cRef]->AbBox->BxMinWidth += width;
        }
      if (colSpan_MaxWidth[i] > realMax)
        {
          /* change width of included columns */
          width = colSpan_MaxWidth[i] - realMax;
          if (span > 0)
            {
              width = (width + span - 1) / span;
              for (cRef = colSpan_First[i]; cRef <= colSpan_Last[i]; cRef++)
                if (colPercent[cRef] == 0 && colWidth[cRef] == 0)
                  colBox[cRef]->AbBox->BxMaxWidth += width;
            }
        }
    }

  GiveAttrWidth (table, ViewFrameTable[frame - 1].FrMagnification, &width, &percent);
  /* now update column boxes */
  pTabRel = pBox->BxColumns;
  cRef = 0;
  min = 0;
  max = 0;
  delta = 0;
  /* no previous column */
  prevBox = NULL;
  while (pTabRel)
    {
      for (i = 0; i < MAX_RELAT_DIM && pTabRel->TaRTable[i];  i++)
        {
          box = colBox[cRef]->AbBox;
          if (colWidth[cRef] && colWidth[cRef] < box->BxMinWidth)
            /* a constrained width it must be greater than the minimum */
            colWidth[cRef] = box->BxMinWidth;
          pTabRel->TaRTWidths[i] = colWidth[cRef];
          pTabRel->TaRTPercents[i] = colPercent[cRef];
          min += box->BxMinWidth;
          if (colWidth[cRef])
            /* when there is a constrained width the maximum is forced */
            max += colWidth[cRef];
          else
            max += box->BxMaxWidth;
          /* take the spacing into account */
          if (prevBox == NULL)
            {
              /* add the left cellspacing */
              if (box->BxXOrg - table->AbBox->BxXOrg > 0)
                delta += box->BxXOrg - pBox->BxXOrg;
              /* add the right cellspacing */
              box1 = colBox[cRef]->AbEnclosing->AbBox;
              delta += box1->BxRPadding + box1->BxRBorder;
              if (colBox[cRef]->AbEnclosing->AbRightMarginUnit != UnAuto)
                delta += box1->BxRMargin;
            }
          else if (box->BxXOrg - prevBox->BxXOrg - prevBox->BxWidth > 0)
            delta += box->BxXOrg - prevBox->BxXOrg - prevBox->BxWidth;
          prevBox = box;
          cRef++;
        }
      pTabRel = pTabRel->TaRNext;
    }

  /* get constraints on the table itself */
  mbp = pBox->BxLPadding + pBox->BxRPadding + pBox->BxLBorder + pBox->BxRBorder;
  if (table->AbLeftMarginUnit != UnAuto && pBox->BxLMargin > 0)
    mbp += pBox->BxLMargin;
  if (table->AbRightMarginUnit != UnAuto && pBox->BxRMargin > 0)
    mbp += pBox->BxRMargin;
  min = min + mbp + delta;
  max = max + mbp + delta;
  if (width)
    {
      /* the table width is constrained */
      width += mbp;
      if (max < width)
        max = width;
    }
  /* do we need to reformat the table */
  change = (pBox->BxRuleWidth != width ||  pBox->BxMinWidth != min ||
            pBox->BxMaxWidth != max);
  reformat = (pBox->BxWidth < min ||
              (pBox->BxRuleWidth != width && pBox->BxWidth == pBox->BxRuleWidth) ||
              (pBox->BxMinWidth != min && pBox->BxWidth == pBox->BxMinWidth) ||
              (max != pBox->BxMaxWidth &&
               (pBox->BxWidth  == pBox->BxMaxWidth ||
                pBox->BxRuleWidth != width && max < pBox->BxWidth)));
  pBox->BxMinWidth = min;
  pBox->BxMaxWidth = max;
  pBox->BxRuleWidth = width;
  TtaFreeMemory (colWidthForced);
  TtaFreeMemory (colVSpan);
  TtaFreeMemory (colBox);
  TtaFreeMemory (colWidth);
  TtaFreeMemory (colPercent);
  if (change)
    {
      /* trasmit the min and max widths to the enclosing paragraph */
      pAb = SearchEnclosingType (table, BoBlock, BoFloatBlock, BoFloatBlock);
      if (pAb && pAb->AbBox)
        {
          pBox = pAb->AbBox;
          if (pBox->BxMinWidth < min)
            pBox->BxMinWidth = min;
          if (pBox->BxMaxWidth < max)
            pBox->BxMaxWidth = max;
        }
      if (reformat)
        {
          cell = GetParentCell (pBox);
          if (cell)
            {
              /* propagate changes to the enclosing table */
              row = SearchEnclosingType (cell, BoRow, BoRow, BoRow);
              if (row  && row->AbBox)
                pAb = (PtrAbstractBox) row->AbBox->BxTable;
              if (pAb && pAb->AbBox)
                {
                  if (DoUnlock1 && table == CheckedTable)
                    {
                      /* we are managing a differed table
                         register the enclosing table */
                      DifferFormatting (pAb, cell, frame);
                      /* and update its widths */
                      CheckedTable = pAb;
                      SetCellWidths (cell, pAb, frame);
                    }
                  else if (!IsDifferredTable (pAb, cell, frame))
                    SetCellWidths (cell, pAb, frame);
                }
            }
          if (!DoUnlock1)
            /* recompute table widths */
            CheckTableWidths (table, frame, TRUE);
        }
    }
  return (reformat);
}

/*----------------------------------------------------------------------
  TtaUpdateTableWidths
  Force the rebuild of a table after presentation changes
  ----------------------------------------------------------------------*/
void TtaUpdateTableWidths (Element table, Document doc)
{
  PtrAbstractBox pAb;

  UserErrorCode = 0;
  if (table == NULL)
    TtaError (ERR_invalid_parameter);
  else if (doc < 1 || doc > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[doc - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    {
      pAb = ((PtrElement) table)->ElAbstractBox[0];
      if (pAb)
        SetTableWidths (pAb, LoadedDocument[doc - 1]->DocViewFrame[0]);
    }
}

/*----------------------------------------------------------------------
  SetCellWidths checks if any cell width is modified.
  If TRUE updates table widths.
  Return TRUE if any table width is modified
  ----------------------------------------------------------------------*/
static ThotBool SetCellWidths (PtrAbstractBox cell, PtrAbstractBox table,
                               int frame)
{
  PtrBox              box;
  int                 min, max, dmax;
  int                 width, percent;
  ThotBool            reformat;

  if (FrameTable[frame].FrView != 1)
    return FALSE;
  box = cell->AbBox;
  GiveCellWidths (cell, frame, &min, &max, &width, &percent);
  if (width && width < min)
    /* a constrained width it must be greater than the minimum */
    width = min;
  if (width)
    /* when there is a constrained width the maximum is forced */
    max = width;
  if (max > box->BxMaxWidth)
    dmax = max - box->BxMaxWidth;
  else
    dmax = box->BxMaxWidth - max;
  reformat = (box->BxWidth < min ||
              (box->BxRuleWidth != width && box->BxWidth == box->BxRuleWidth) ||
              (box->BxMinWidth != min && box->BxWidth == box->BxMinWidth) ||
              (dmax &&
               (box->BxWidth == box->BxMaxWidth ||
                (box->BxWidth != box->BxRuleWidth && 
                 (max < box->BxWidth || dmax > box->BxWidth)))));
  box->BxMinWidth = min;
  box->BxMaxWidth = max;
  box->BxRuleWidth = width;

  if (reformat && table)
    {
      if (DoUnlock1)
        {
          if (table == CheckedTable)
            /* something changed in the cell, check any table change */
            SetTableWidths (table, frame);
          else
            /* register a new differed table */
            DifferFormatting (table, cell, frame);
        }
      else //if (!IsDifferredTable (table, cell, frame))
        /* something changed in the cell, check any table change */
        SetTableWidths (table, frame);
    }
  return (reformat);
}


/*----------------------------------------------------------------------
  UpdateCellHeight
  The cell height changes, we need to propagate the change.
  ----------------------------------------------------------------------*/
void UpdateCellHeight (PtrAbstractBox cell, int frame)
{
  PtrAbstractBox      table;
  PtrAbstractBox      row;

  /* table formatting in the main view only */
  if (cell == NULL || FrameTable[frame].FrDoc == 0)
    return;
  if (FrameTable[frame].FrView != 1)
    return;

  /* get row and table elements */
  row = SearchEnclosingType (cell, BoRow, BoRow, BoRow);
  if (row  && row->AbBox)
    {
      table = (PtrAbstractBox) row->AbBox->BxTable;
      if (table)
        {
          if (Lock)
            /* the table formatting is locked */
            DifferFormatting (table, cell, frame);
          else if (IsDifferredTable (table, NULL, frame))
            /* the table will be managed later */
            return;
          else
            CheckRowHeights (table, frame);
        }
    }
}



/*----------------------------------------------------------------------
  UpdateCellRowSpan
  The column width changes, we need to propagate the change.
  ----------------------------------------------------------------------*/
static void UpdateCellRowSpan (PtrAbstractBox cell, PtrAbstractBox table)
{
  PtrTabSpan          pTabSpan;
  PtrAttribute        pAttr;
  PtrSSchema          pSS;
  int                 i, attrVSpan;

  if (table && table->AbBox)
    {
      /* check if the cell is into the list of vertical spanned cells */
      pTabSpan = table->AbBox->BxSpans;
      pSS = table->AbElement->ElStructSchema;
      while (pTabSpan)
        {
          for (i = 0; i < MAX_RELAT_DIM; i++)
            {
              if (cell == pTabSpan->TaSpanCell[i])
                {
                  pAttr = cell->AbElement->ElFirstAttr;
                  attrVSpan = GetAttrWithException (ExcRowSpan, pSS);
                  while (pAttr)
                    {
                      if (pAttr->AeAttrNum == attrVSpan &&
                          pAttr->AeAttrSSchema == pSS)
                        {
                          if (pAttr->AeAttrValue != pTabSpan->TaSpanNumber[i])
                            /* update the registered row span value */
                            pTabSpan->TaSpanNumber[i] = pAttr->AeAttrValue;
                          pAttr = NULL;
                        }
                      else
                        pAttr = pAttr->AeNext;
                    }
                  return;
                }
            }
          pTabSpan = pTabSpan->TaSpanNext;
        }
    }
}

/*----------------------------------------------------------------------
  UpdateColumnWidth
  The column width changes, we need to propagate the change.
  ----------------------------------------------------------------------*/
void UpdateColumnWidth (PtrAbstractBox cell, PtrAbstractBox col, int frame)
{
  PtrAbstractBox      table;
  PtrAbstractBox      row;

  /* table formatting in the main view only */
  if (FrameTable[frame].FrDoc == 0)
    return;
  if (FrameTable[frame].FrView != 1)
    return;

  /* look for the table */
  table = NULL;
  if (col && col->AbBox)
    /* get the table element */
    table = (PtrAbstractBox) col->AbBox->BxTable;
  else if (cell && cell->AbBox)
    {
      /* get row and table elements */
      row = SearchEnclosingType (cell, BoRow, BoRow, BoRow);
      if (row  && row->AbBox)
        table = (PtrAbstractBox) row->AbBox->BxTable;
      /* if the cell is row spanned update its rowspan value */
      UpdateCellRowSpan (cell, table);
    }
  if (table && !table->AbNew && !table->AbDead &&
      table->AbBox && table->AbBox->BxCycles == 0)
    {
      if (Lock)
        /* the table formatting is locked */
        DifferFormatting (table, cell, frame);
      else if (cell && cell->AbBox)
        {
          /* there a change within a specific cell */
          if (SetCellWidths (cell, table, frame))
            {
#ifdef TAB_DEBUG
              if (table->AbBox->BxCycles > 0)
                printf ("table in progress\n");
#endif
              if (IsDifferredTable (table, NULL, frame))
                /* the table will be managed later */
                return;
              /* Now check the table size */
              CheckTableWidths (table, frame, TRUE);
              CheckRowHeights (table, frame);
            }
        }
    }
}


/*----------------------------------------------------------------------
  UpdateTable checks information about the table element after creation of:
  - a table (table != NULL)
  - a column (table == NULL && col != NULL)
  - a row (table == NULL && row != NULL)
  ----------------------------------------------------------------------*/
void UpdateTable (PtrAbstractBox table, PtrAbstractBox col,
                  PtrAbstractBox row, int frame)
{
  PtrAbstractBox      pAb;

  /* table formatting in the main view only */
  if (FrameTable[frame].FrDoc == 0)
    return;
  if (FrameTable[frame].FrView != 1)
    return;

  if (table == NULL)
    {
      /* look for the table */
      if (col && col->AbBox)
        {
          pAb = (PtrAbstractBox) col->AbBox->BxTable;
          if (pAb == NULL)
            {
              pAb = SearchEnclosingType (col, BoTable, BoTable, BoTable);
              /* during table building each column needs a minimum width */
              if (col->AbBox->BxWidth < 20)
                ResizeWidth (col->AbBox, col->AbBox, NULL, 20 - col->AbBox->BxWidth,
                             0, 0, 0, frame, FALSE);
            }
        }
      else if (row && row->AbBox)
        {
          pAb = (PtrAbstractBox) row->AbBox->BxTable;
          if (pAb == NULL)
            pAb = SearchEnclosingType (row, BoTable, BoTable, BoTable);
        }
      else
        pAb = NULL;
    }
  else
    pAb = table;

  if (pAb && pAb->AbBox && !pAb->AbNew && !IsDead (pAb))
    {
      /* the table box has been created */
      if (table || col)
        /* build or rebuild columns list */
        BuildColOrRowList (pAb, BoColumn);
      if (table || row)
        /* build or rebuild rows list */
        BuildColOrRowList (pAb, BoRow);

      if (Lock)
        /* the table formatting is locked */
        DifferFormatting (pAb, NULL, frame);
      else if (IsDifferredTable (pAb, NULL, frame))
        /* the table will be managed later */
        return;
      else if (table)
        {
          /* compute widths of each column within the table */
          if (SetTableWidths (table, frame))
            {
              /* Now check the table size */
              CheckTableWidths (table, frame, TRUE);
              CheckRowHeights (table, frame);
            }
        }
    }
}


/*----------------------------------------------------------------------
  ClearTable removes table information
  ----------------------------------------------------------------------*/
void ClearTable (PtrAbstractBox table)
{
  PtrTabSpan          pTabSpan;
  PtrBox              pBox;

  /* free specific blocks */
  pBox = table->AbBox;
  if (pBox)
    {
      FreeTaRBlock (pBox->BxColumns);
      pBox->BxColumns = NULL;
      FreeTaRBlock (pBox->BxRows);
      pBox->BxRows = NULL;
      /* remove the list of vertical spanned cells */
      pTabSpan = pBox->BxSpans;
      while (pTabSpan)
        {
          pBox->BxSpans = pTabSpan->TaSpanNext;
          TtaFreeMemory (pTabSpan);
          pTabSpan = pBox->BxSpans;
        }
    }
}


/*----------------------------------------------------------------------
  IsFirstColumn returns result = TRUE if cel is within the first
  column of the table.
  ----------------------------------------------------------------------*/
void IsFirstColumn (PtrAbstractBox cell, PtrAbstractBox table,
                    ThotBool *result)
{
  PtrAbstractBox      col, firstcol;
  PtrAttribute        pAttr;
  PtrSSchema          pSS;
  PtrElement          pRefEl;
  int                 attrNum;
  ThotBool             found;

  *result = FALSE;
  if (cell != NULL &&  table != NULL && table->AbBox->BxColumns != NULL)
    {
      /* get first column element */
      firstcol = table->AbBox->BxColumns->TaRTable[0];
      /* look at the current column */
      pSS = cell->AbElement->ElStructSchema;
      attrNum = GetAttrWithException (ExcColRef, pSS);
      if (attrNum != 0)
        {
          /* search this attribute attached to the cell element */
          pAttr = cell->AbElement->ElFirstAttr;
          found = FALSE;
          col = NULL;
          while (!found && pAttr != NULL)
            if (pAttr->AeAttrType == AtReferenceAttr &&
                pAttr->AeAttrReference != NULL &&
                pAttr->AeAttrReference->RdReferred != NULL)
              {
                pRefEl = pAttr->AeAttrReference->RdReferred->ReReferredElem;
                col = pRefEl->ElAbstractBox[cell->AbDocView - 1];
                found = TRUE;
              }
            else
              pAttr = pAttr->AeNext;

          *result = (col == firstcol);
        }
    }
}

/*----------------------------------------------------------------------
  TtaLockTableFormatting suspends all tables formatting
  ----------------------------------------------------------------------*/
void TtaLockTableFormatting ()
{
  Lock = TRUE;
}

/*----------------------------------------------------------------------
  UnlockTableFormatting reformats all locked tables
  ----------------------------------------------------------------------*/
void TtaUnlockTableFormatting ()
{
  PtrLockRelations    pLockRel, first;
  PtrAbstractBox      table, cell;
  PtrLine             pLine;
  Propagation         savpropage;
  int                 i;

  if (Lock)
    {
      Lock = FALSE;
      if (DifferedChecks == NULL)
        /* nothing to do */
        return;

      DoUnlock1 = TRUE;
      savpropage = Propagate;
      Propagate = ToAll;
      /*
        First, compute the minimum width, the maximum width and
        the contrained width of each column and table starting
        form the most embedded to the enclosing table
      */
      first = DifferedChecks;
      ActiveChecks = DifferedChecks;
      DifferedChecks = NULL;
      pLockRel = first;
      while (pLockRel->LockRNext != NULL)
        pLockRel = pLockRel->LockRNext;
      while (pLockRel != NULL)
        {
          /* Manage all locked tables */
          i = MAX_RELAT_DIM - 1;
          while (i >= 0)
            {
              table = pLockRel->LockRTable[i];
              if (table && table->AbElement &&
                  table->AbLeafType == LtCompound &&
                  table->AbBox && table->AbBox->BxType == BoTable)
                {
                  cell = pLockRel->LockRCell[i];
                  if (IsDead (table))
                    /* nothing to do more on this table */
                    pLockRel->LockRTable[i] = NULL;
                  else
                    {
                      /* there is a change within a specific cell */
                      CheckedTable = table;
                      if (cell && cell->AbBox)
                        /* there is a change within a specific cell */
                        SetCellWidths (cell, table, pLockRel->LockRFrame[i]);
                      else
                        SetTableWidths (table, pLockRel->LockRFrame[i]);
                      CheckedTable = NULL;
                    }
                }
              /* next entry */
              i--;
            }
          /* next block */
          pLockRel = pLockRel->LockRPrev;
        }
      DoUnlock1 = FALSE;

      /*
        Second, reformat all tables starting
        form the enclosing table to the most embedded
      */
      DoUnlock2 = TRUE;
      pLockRel = first;
      while (pLockRel != NULL)
        {
          /* Manage all locked tables */
          i = 0;
          while (i < MAX_RELAT_DIM)
            {
              table = pLockRel->LockRTable[i];
              if (table && table->AbElement &&
                  table->AbLeafType == LtCompound &&
                  table->AbBox && table->AbBox->BxType == BoTable)
                {
                  CheckedTable = table;
                  CheckTableWidths (table, pLockRel->LockRFrame[i], FALSE);
                  /* need to propagate to enclosing boxes */
                  ComputeEnclosing (pLockRel->LockRFrame[i]);
                  while (UpdatedParentTable)
                    {
                      CheckedTable = UpdatedParentTable;
                      UpdatedParentTable = NULL;
                      CheckTableWidths (CheckedTable, pLockRel->LockRFrame[i], FALSE);
                    }
                }
              /* next entry */
              i++;
            }
          /* next block */
          pLockRel = pLockRel->LockRNext;
        }
      CheckedTable = NULL;
      ActiveChecks = NULL;
      DoUnlock2 = FALSE;

      /*
        Then, check all table heighs form the most embedded to
        the enclosing table
      */
      pLockRel = first;
      while (pLockRel->LockRNext != NULL)
        pLockRel = pLockRel->LockRNext;
      while (pLockRel != NULL)
        {
          /* Manage all locked tables */
          i = MAX_RELAT_DIM - 1;
          while (i >= 0)
            {
              table = pLockRel->LockRTable[i];
              if (table && table->AbElement &&
                  table->AbLeafType == LtCompound &&
                  table->AbBox && table->AbBox->BxType == BoTable)
                {
                  /* it's still a table */
                  cell = pLockRel->LockRCell[i];
                  if (cell)
                    HeightPack (cell, cell->AbBox,
                                pLockRel->LockRFrame[i]);
                  CheckRowHeights (table, pLockRel->LockRFrame[i]);
                  /* need to propagate to enclosing boxes */
                  if (table && table->AbEnclosing->AbBox &&
                      table->AbEnclosing->AbBox->BxType == BoBlock ||
                      table->AbEnclosing->AbBox->BxType == BoFloatBlock ||
                      table->AbEnclosing->AbBox->BxType == BoStructGhost ||
                      table->AbEnclosing->AbBox->BxType == BoGhost)
                    {
                      if (table->AbBox)
                        pLine = SearchLine (table->AbBox, pLockRel->LockRFrame[i]);
                      else
                        pLine = NULL;
                      RecomputeLines (table->AbEnclosing, pLine, NULL, pLockRel->LockRFrame[i]);
                    }
                  else if (table && table->AbEnclosing->AbBox &&
                           table->AbEnclosing->AbBox->BxType != BoCell &&
                           table->AbEnclosing->AbBox->BxType != BoCellBlock &&
                           table->AbEnclosing->AbWidth.DimAbRef == NULL &&
                           table->AbEnclosing->AbWidth.DimValue == -1)
                    WidthPack (table->AbEnclosing, table->AbBox, pLockRel->LockRFrame[i]);
                  ComputeEnclosing (pLockRel->LockRFrame[i]);
                  DisplayFrame (pLockRel->LockRFrame[i]);
                }
              /* next entry */
              i--;
            }
          /* next block */
          pLockRel = pLockRel->LockRPrev;
        }

      /* Now, free allocated blocks */
      Propagate = savpropage;
      while (first != NULL)
        {
          pLockRel = first;
          first = pLockRel->LockRNext;
          TtaFreeMemory (pLockRel);
        }
    }
}


/*----------------------------------------------------------------------
  TtaGiveTableFormattingLock gives the status of the table formatting lock.
  ----------------------------------------------------------------------*/
void TtaGiveTableFormattingLock (ThotBool *lock)
{
  /* check if we're not processing a TtaUnlockTableFormatting */
  if (DifferedChecks != NULL)
    *lock = TRUE;
  else
    *lock = Lock;
}


