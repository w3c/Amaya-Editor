/*
 *
 *  (c) COPYRIGHT INRIA 1997.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
 /*
 * HTML tables
 */

 /*
 *
 * Author: I. Vatton (INRIA)
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "appdialogue.h"
#include "frame.h"

#define THOT_EXPORT extern
#include "appdialogue_tv.h"
#include "boxes_tv.h"
#include "edit_tv.h"
#include "frame_tv.h"

static PtrTabUpdate FirstColUpdate;
static ThotBool ComputeColInWork = FALSE;
#include "attributes_f.h"
#include "boxmoves_f.h"
#include "boxrelations_f.h"
#include "buildboxes_f.h"
#include "buildlines_f.h"
#include "createabsbox_f.h"
#include "exceptions_f.h"
#include "memory_f.h"
#include "tree_f.h"
#define MAX_COLROW 50
/*#define TAB_DEBUG*/

#ifdef __STDC__
static void UpdateColumnWidth (PtrAbstractBox cell, PtrAbstractBox col, int frame);
#else
static void UpdateColumnWidth (/*cell, col, frame*/);
#endif

/*----------------------------------------------------------------------
  NextSiblingAbsBox returns the next sibling or the next sibling of a parent.
  When pRoot is not Null, the returned abstract box has to be included
  within the pRoot.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static PtrAbstractBox NextSiblingAbsBox (PtrAbstractBox pAb, PtrAbstractBox pRoot)
#else  /* __STDC__ */
static PtrAbstractBox NextSiblingAbsBox (pAb, pRoot)
PtrAbstractBox      pAb;
PtrAbstractBox      pRoot;
#endif /* __STDC__ */
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
#ifdef __STDC__
static void       GetTaRBlock (PtrTabRelations *pBlock)
#else  /* __STDC__ */
static void       GetTaRBlock (pBlock)
PtrTabRelations  *pBlock;
#endif /* __STDC__ */

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
#ifdef __STDC__
static void       FreeTaRBlock (PtrTabRelations pBlock)
#else  /* __STDC__ */
static void       FreeTaRBlock (pBlock)
PtrTabRelations   pBlock;
#endif /* __STDC__ */

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
#ifdef __STDC__
static void      BuildColOrRowList (PtrAbstractBox table, BoxType colrow)
#else
static void      BuildOrColRowList (table, colrow)
PtrAbstractBox   table;
BoxType          colrow;
#endif
{
  PtrTabRelations     pTabRel, pOldTabRel;
  PtrTabRelations     pPreviousTabRel;
  PtrAbstractBox      pAb;
  int                 i, j;
  ThotBool             loop;
  ThotBool             empty;

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
  while (pAb != NULL)
    {
      if (pAb->AbBox != NULL &&
	  (pAb->AbBox->BxType == BoRow || pAb->AbBox->BxType == BoColumn))
	/* skip over the element contents */
	pAb = NextSiblingAbsBox (pAb, table);
      else
	pAb = SearchNextAbsBox (pAb, table);

      if (pAb != NULL && !pAb->AbDead && pAb->AbBox != NULL &&
	  pAb->AbBox->BxType == BoTable && !pAb->AbPresentationBox)
	/* it's an included table, skip over this table */
	pAb = NextSiblingAbsBox (pAb, table);

      if (pAb != NULL && pAb->AbBox != NULL)
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
  SetAttrWidthPxl changes the AttrWidthPxl value.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void     SetAttrWidthPxl (PtrAbstractBox pAb, int width)
#else
static void     SetAttrWidthPxl (pAb, width)
PtrAbstractBox  pAb;
int             width;
#endif
{
  PtrSSchema          pSS;
  PtrAttribute        pAttr;
  int                 attr;
  ThotBool             found;

  found = FALSE;
  /* look for ExcNewWidth attribute */
  pSS = pAb->AbElement->ElStructSchema;
  attr = GetAttrWithException (ExcNewWidth, pSS);
  if (attr != 0)
    {
      pAttr = pAb->AbElement->ElFirstAttr;
      found = FALSE;
      while (!found && pAttr != NULL)
	if (pAttr->AeAttrNum == attr &&
	    pAttr->AeAttrSSchema->SsCode == pSS->SsCode &&
	    (pAttr->AeAttrType == AtNumAttr || pAttr->AeAttrType == AtEnumAttr))
	  {
	    found = TRUE;
            pAttr->AeAttrValue = width;
#ifdef TAB_DEBUG
if (pAb->AbBox->BxType == BoTable)
printf("set table ");
else if (pAb->AbBox->BxType == BoColumn)
printf("set column ");
else
printf("set cell ");
printf("width=%d\n", width);
#endif 
	  }
	else
	  pAttr = pAttr->AeNext;
    }
}


/*----------------------------------------------------------------------
  SetAttrWidthPercent changes the AttrWidthPercent value.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void     SetAttrWidthPercent (PtrAbstractBox pAb, int width)
#else
static void     SetAttrWidthPercent (pAb, width)
PtrAbstractBox  pAb;
int             width;
#endif
{
  PtrSSchema          pSS;
  PtrAttribute        pAttr;
  int                 attr;
  ThotBool             found;

  found = FALSE;
  /* look for ExcNewWidth attribute */
  pSS = pAb->AbElement->ElStructSchema;
  attr = GetAttrWithException (ExcNewPercentWidth, pSS);
  if (attr != 0)
    {
      pAttr = pAb->AbElement->ElFirstAttr;
      found = FALSE;
      while (!found && pAttr != NULL)
	if (pAttr->AeAttrNum == attr &&
	    pAttr->AeAttrSSchema->SsCode == pSS->SsCode &&
	    (pAttr->AeAttrType == AtNumAttr || pAttr->AeAttrType == AtEnumAttr))
	  {
	    found = TRUE;
            pAttr->AeAttrValue = width;
#ifdef TAB_DEBUG
if (pAb->AbBox->BxType == BoTable)
printf("set table ");
else if (pAb->AbBox->BxType == BoColumn)
printf("set column ");
else
printf("set cell ");
printf("width=%d\n", width);
#endif
	  }
	else
	  pAttr = pAttr->AeNext;
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
#ifdef __STDC__
static ThotBool  GiveAttrWidth (PtrAbstractBox pAb, int *width, int *percent)
#else
static ThotBool  GiveAttrWidth (pAb, width, percent)
PtrAbstractBox  pAb;
int            *width;
int            *percent;
#endif
{
  PtrSSchema          pSS;
  PtrAttribute        pAttr;
  int                 attrWidth, attrPercent;
  ThotBool             found;

  *width = 0;
  *percent = 0;
  found = FALSE;
  /* look for ExcNewWidth attribute */
  pSS = pAb->AbElement->ElStructSchema;
  attrWidth = GetAttrWithException (ExcNewWidth, pSS);
  attrPercent = GetAttrWithException (ExcNewPercentWidth, pSS);
  if (attrWidth != 0 || attrPercent != 0)
    {
      pAttr = pAb->AbElement->ElFirstAttr;
      found = FALSE;
      while (!found && pAttr != NULL)
	if (pAttr->AeAttrNum == attrWidth &&
	    pAttr->AeAttrSSchema->SsCode == pSS->SsCode &&
	    (pAttr->AeAttrType == AtNumAttr || pAttr->AeAttrType == AtEnumAttr))
	  {
	    found = TRUE;
            *width = pAttr->AeAttrValue;
#ifdef TAB_DEBUG
if (pAb->AbBox->BxType == BoTable)
printf("table ");
else if (pAb->AbBox->BxType == BoColumn)
printf("column ");
else
printf("cell ");
printf("width=%d\n", *width);
#endif
	  }
	else if (pAttr->AeAttrNum == attrPercent &&
	    pAttr->AeAttrSSchema->SsCode == pSS->SsCode &&
	    (pAttr->AeAttrType == AtNumAttr || pAttr->AeAttrType == AtEnumAttr))
	  {
	    found = TRUE;
	    *percent = pAttr->AeAttrValue;
#ifdef TAB_DEBUG
if (pAb->AbBox->BxType == BoTable)
printf("table ");
else if (pAb->AbBox->BxType == BoColumn)
printf("column ");
else
printf("cell ");
printf("width=%d%%\n", *percent);
#endif
	  }
	else
	  pAttr = pAttr->AeNext;
    }
  return (found);
}


/*----------------------------------------------------------------------
  CheckRowHeights checks row-spanned cells with related rows.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void     CheckRowHeights (PtrAbstractBox table, int number, PtrAbstractBox *rowSpanCell, int *rowSpans, int frame)
#else
static void     CheckRowHeights (table, number, rowSpanCell, rowSpans, frame)
PtrAbstractBox  table;
int             number;
PtrAbstractBox *crowSpanCell;
int            *rowSpans;
int             frame;
#endif
{
  PtrAttribute        pAttr;
  PtrSSchema          pSS = NULL;
  PtrDocument         pDoc;
  PtrAbstractBox      cell, row , firstRow, pAb;
  PtrAbstractBox      rowList[MAX_COLROW];
  PtrTabRelations     pTabRel;
  int                 i, j, k, org, val;
  int                 sum, height;
  int                 attrHeight = 0;
  int                 remainder;
  ThotBool             found;

  if (table->AbBox->BxCycles != 0)
    /* the table formatting is currently in process */
    return;
  j = 0;
  /* manage spanned columns */
  pDoc = LoadedDocument[FrameTable[frame].FrDoc - 1];
  if (number > 0)
    {
      pSS = table->AbElement->ElStructSchema;
      attrHeight = GetAttrWithException (ExcNewHeight, pSS);
    }
  for (i = 0; i < number; i++)
    {
      cell = rowSpanCell[i];
      if (cell != NULL && cell->AbBox != NULL && rowSpans[i] > 1)
	{
	  firstRow = SearchEnclosingType (cell, BoRow);
	  row = firstRow;
	  /* search the current row in the rows list of the table */
	  found = FALSE;
	  pTabRel = table->AbBox->BxRows;
	  while (pTabRel != NULL && !found)
	    {
	      j = 0;
	      while ( j < MAX_RELAT_DIM && pTabRel->TaRTable[j] != NULL &&
		      row != pTabRel->TaRTable[j])
		j++;
	      
	      found = (j < MAX_RELAT_DIM && row == pTabRel->TaRTable[j]);
	      if (!found)
		pTabRel = pTabRel->TaRNext;
	    }

	  if (found)
	    {
	      /* compare the cell height with rows heights */
	      sum = 0;
	      pAb = NULL;
	      for (k = 0; k < rowSpans[i] && k < MAX_COLROW; k++)
		{
		  rowList[k] = row;
		  if (row != NULL && row->AbBox != NULL)
		    {
		      HeightPack (row, NULL, frame);
		      /* add padding and borders space */
		      if (pAb != NULL)
			remainder = row->AbBox->BxYOrg - pAb->AbBox->BxYOrg - pAb->AbBox->BxHeight;
		      else
			remainder = 0;
		      sum += row->AbBox->BxHeight + remainder;
		    }
		  /* select the next row in the list */
		  j++;
		  if (pTabRel != NULL &&
		      (j >= MAX_RELAT_DIM || pTabRel->TaRTable[j] == NULL))
		    {
		      pTabRel = pTabRel->TaRNext;
		      j = 0;
		    }
		  /* keep in mindf the previous row */
		  pAb = row;
		  if (pTabRel != NULL)
		    row = pTabRel->TaRTable[j];
		  else
		    row = NULL;
		}
	      /* update rowSpans[i] if necessary */
	      if (k < rowSpans[i])
		rowSpans[i] = k;

	      /* get the real cell height */
	      pAb = cell->AbFirstEnclosed;
	      org = cell->AbBox->BxYOrg;
	      height = 0;
	      while (pAb != NULL)
		{
		  if (!pAb->AbDead && pAb->AbBox != NULL )
		    {
		      if (pAb->AbHeight.DimAbRef == NULL ||
			  !IsParentBox (pAb->AbHeight.DimAbRef->AbBox, pAb->AbBox))
			{
			  val = pAb->AbBox->BxYOrg + pAb->AbBox->BxHeight - org;
			  if (height < val)
			    height = val;
			  pAb = NextSiblingAbsBox (pAb, cell);
			}
		      else
			pAb = pAb->AbFirstEnclosed;
		    }
		  else
		    pAb = NextSiblingAbsBox (pAb, cell);
		}
	      /* add space between the the cell and its parent row */
	      if (firstRow != NULL)
		height += org - firstRow->AbBox->BxYOrg;

#ifdef TAB_DEBUG
printf("<<<check cell_height=%d over %d rows_height=%d\n", height, rowSpans[i], sum);
#endif
	   
	      /* update rows' height if necessary */
	      height = height - sum;
	      if (height > 0)
		{
		  height = height / rowSpans[i];
		  for (k = 0; k < rowSpans[i]; k++)
		    if (rowList[k] != NULL &&
			rowList[k]->AbBox->BxHeight + height > 0)
		      {
			/* create the attribute for this element */
			GetAttribute (&pAttr);
			pAttr->AeAttrSSchema = pSS;
			pAttr->AeAttrNum = attrHeight;
			pAttr->AeAttrType = AtNumAttr;
			pAttr->AeAttrValue = rowList[k]->AbBox->BxHeight + height;
			AttachAttrWithValue (rowList[k]->AbElement, pDoc, pAttr);
			DeleteAttribute (NULL, pAttr);
			/* update the row box */
			ComputeUpdates (rowList[k], frame);
		      }
		  /* Redisplay views */
		  if (ThotLocalActions[T_redisplay] != NULL)
		    (*ThotLocalActions[T_redisplay]) (pDoc);
		}
	    }
	}
    }
}


/*----------------------------------------------------------------------
  CheckTableSize computes the minimum width and the maximum width of a
  table.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void     CheckTableSize (PtrAbstractBox table, int cNumber, PtrAbstractBox *colBox, int *colWidth, int *colPercent,  int frame, ThotBool force)
#else
static void     CheckTableSize (table, cNumber, colBox, colWidth, colPercent, frame, force)
PtrAbstractBox  table;
int             cNumber;
PtrAbstractBox *colBox;
int            *colWidth;
int            *colPercent;
int             frame;
ThotBool         force;
#endif
{
  PtrAbstractBox      pBlock, pCell;
  PtrBox              pBox, pOldBox;
  Propagation         savePropagate;
  int                 j, var, delta, px;
  int                 width, i, cRef, minsize;
  int                 min, max, sum, remainder;
  int                 percent, sumPercent, n;
  int                 realMin, realMax;
  int                 minWithPercent, maxWithPercent;
  ThotBool             constraint, still;
  ThotBool             useMax, checkEnclosing;

  percent = 0;
  /* Now check the table size */
  if (cNumber == 0)
    return;
  else if (table->AbBox->BxCycles != 0)
    /* the table formatting is currently in process */
    return;

  pCell = GetParentCell (table->AbBox);
  checkEnclosing = FALSE;
  constraint = GiveAttrWidth (table, &width, &percent);
  if (!constraint)
    {
      if (force && pCell == NULL)
	force = FALSE;
      /* limit given by available space */
      width = table->AbEnclosing->AbBox->BxWidth;
    }
  else if (percent != 0)
    {
      /* limit given by precent of available space */
      width = table->AbEnclosing->AbBox->BxWidth * percent / 100;
      if (width == 0)
	width = 1;      
    }
  sumPercent = 0;
  min = 0;
  max = 0;
  delta = 0;
  sum = 0;
  i = 0;
  j = 0;
  var = 0;
  realMin = 0;
  realMax = 0;
  minWithPercent = 0;
  maxWithPercent = 0;
  /* additional space */ 
    remainder = 0;
  /* no previous column */
  pOldBox = NULL;
#ifdef TAB_DEBUG
printf("<<<<<<<<<<<<<<<%d\n", table->AbBox->BxWidth);
#endif
  minsize = (int)(((float) width /(float) cNumber) /2.);
  for (cRef = 0; cRef < cNumber; cRef++)
    {
      pBox = colBox[cRef]->AbBox;
      /* add padding and border space */
      if (pOldBox == NULL)
	{
	  if (pBox->BxXOrg - table->AbBox->BxXOrg > 0)
	    remainder += pBox->BxXOrg - table->AbBox->BxXOrg;
	}
      else if (pBox->BxXOrg - pOldBox->BxXOrg - pOldBox->BxWidth > 0)
	remainder += pBox->BxXOrg - pOldBox->BxXOrg - pOldBox->BxWidth;

      if (pBox->BxWidth > 0)
	delta += pBox->BxWidth;
      /* how many columns can be modified and what is current values */
      realMin += pBox->BxMinWidth;
      realMax += pBox->BxMaxWidth;
      if (colPercent[cRef] != 0 &&
	  (colPercent[cRef] * 100 / width) < pBox->BxMinWidth)
	colPercent[cRef] = 0;
      if (colPercent[cRef] != 0)
	{
	  sumPercent += colPercent[cRef];
	  minWithPercent += pBox->BxMinWidth;
	  maxWithPercent += pBox->BxMaxWidth;
	}
      else if (colWidth[cRef] != 0)
	{
	  sum += colWidth[cRef];
	  var += colWidth[cRef];
	}
      else
	{
	  min += pBox->BxMinWidth;
	  max += pBox->BxMaxWidth;
	  minWithPercent += pBox->BxMinWidth;
	  maxWithPercent += pBox->BxMaxWidth;
	  if (pBox->BxMaxWidth > minsize)
	    {
	      i++;
	      j++;
	      var += pBox->BxMinWidth;
	    }
	  else
	    {
	      /* take the maximum value */
	      j++;
	      /*colWidth[cRef] = pBox->BxMaxWidth;*/
	      var += pBox->BxMaxWidth;
	    }
	}

      /* keep in mind the previous column box */
      pOldBox = pBox;
#ifdef TAB_DEBUG
printf ("cref=%d: Min =%d, Max=%d, colWidth=%d, colPercent=%d\n", cRef, pBox->BxMinWidth, pBox->BxMaxWidth, colWidth[cRef], colPercent[cRef]);
#endif
    }

  /* add space after the last column */
  if (pOldBox != NULL)
    {
      /* locate the enclosing box of columns */
      pBox = pOldBox;
      while (pBox->BxAbstractBox->AbEnclosing != NULL &&
	     pBox->BxAbstractBox->AbEnclosing->AbBox->BxType != BoTable)
	pBox = pBox->BxAbstractBox->AbEnclosing->AbBox;
      WidthPack (pBox->BxAbstractBox, NULL, frame);
      remainder += pBox->BxXOrg + pBox->BxWidth - pOldBox->BxXOrg - pOldBox->BxWidth;
    }

  if (min + sum > 0)
    {
      /* there is almost one column width not given by a % value */
      sum += remainder;
      realMin += remainder;
      realMax += remainder;
    }

  /* check if percent values are ok */
  if (width > 0)
    delta = (min + sum) * 100 / width;
  else
    delta = 0;
  if (sumPercent + delta > 100 && sumPercent > 0)
    {
      /* remove each colPercent */
      sumPercent = 0;
      min = maxWithPercent;
      max = maxWithPercent;
      for (cRef = 0; cRef < cNumber; cRef++)
	colPercent[cRef] = 0;
    }
  else
    sumPercent = sumPercent * width / 100;
  min = min + sum + sumPercent;
  max = max + sum + sumPercent;
  var = var + sumPercent;
  /* check if the contraint is valid */
  if (constraint && min > width)
    {
      constraint = FALSE;
      if (percent != 0)
	{
	  /* the limit is given by a precent rule */
	  if (table->AbEnclosing->AbBox->BxWidth > 0 && pCell == NULL && percent != 100)
	    {
	      percent = 100;
	      SetAttrWidthPercent (table, percent);
	      table->AbWidth.DimValue = percent;
	    }
	}
      else
	{
	  SetAttrWidthPxl (table, min);
	  /* update the width rule of the table element */
	  table->AbWidth.DimValue = min;
	}
      checkEnclosing = TRUE;
    }

  if (constraint)
    {
      table->AbBox->BxMinWidth = width;
      table->AbBox->BxMaxWidth = width;
    }
  else
    {
      table->AbBox->BxMinWidth = realMin + sumPercent;
      table->AbBox->BxMaxWidth = realMax + sumPercent;
    }

  /* remind that the table height has to be recomputed */
  pBox = table->AbBox;
  pBox->BxCycles = 1;
  if (force && min <= width && pCell != NULL)
    /* it's not possible to change the table width */
    constraint = TRUE;

  if (max <= width && !constraint)
    {
      /* assign the maximum width to each column */
      ResizeWidth (pBox, pBox, NULL, max - pBox->BxWidth, 0, frame);
      for (cRef = 0; cRef < cNumber; cRef++)
	{
	  pBox = colBox[cRef]->AbBox;
	  if (colPercent[cRef] != 0)
	    delta = ((max - remainder) * colPercent[cRef] / 100) - pBox->BxWidth;
	  else if (colWidth[cRef] != 0)
	    delta = colWidth[cRef] - pBox->BxWidth;
	  else
	    delta = pBox->BxMaxWidth - pBox->BxWidth;
	  ResizeWidth (pBox, pBox, NULL, delta, 0, frame);
#ifdef TAB_DEBUG
printf ("Width[%d]=%d\n", cRef, pBox->BxWidth);
#endif
	}
    }
  else if (min >= width || (!constraint && i == 0))
    {
      /* assign the minimum width to each column */
      ResizeWidth (pBox, pBox, NULL, min - pBox->BxWidth, 0, frame);
      for (cRef = 0; cRef < cNumber; cRef++)
	{
	  pBox = colBox[cRef]->AbBox;
	  if (colPercent[cRef] != 0)
	    delta = ((min - remainder) * colPercent[cRef] / 100) - pBox->BxWidth;
	  else if (colWidth[cRef] != 0)
	    delta = colWidth[cRef] - pBox->BxWidth;
	  else if (pBox->BxMaxWidth <= minsize)
	    delta = pBox->BxMaxWidth - pBox->BxWidth;
	  else
	    delta = pBox->BxMinWidth - pBox->BxWidth;
	  ResizeWidth (pBox, pBox, NULL, delta, 0, frame);
#ifdef TAB_DEBUG
printf ("Width[%d]=%d\n", cRef, pBox->BxWidth);
#endif
	}
    }
  else
    {
      /* assign a specific width to each column */
      ResizeWidth (pBox, pBox, NULL, width - pBox->BxWidth, 0, frame);
      useMax = FALSE;
      if (max < width)
	{
	  var = max;
	  i = j;
	  useMax = TRUE;
	}
      else if (min > width)
	{
	  var = min;
	  i = j;
	}
      else if (i > 0)
	{
	  /* process usually */
	  constraint = FALSE;
	  do
	    {
	      still = FALSE;
	      j = width - var;
	      delta = j / i;
	      n = j - (delta * i);
	      /* compare maximum width and computed width of each column  */
	      for (cRef = 0; cRef < cNumber && i > 0; cRef++)
		{
		  pBox = colBox[cRef]->AbBox;
		  if (colPercent[cRef] == 0 && colWidth[cRef] == 0)
		    {
		      j = pBox->BxMaxWidth - pBox->BxMinWidth;
		      /* add extra pixels */
		      if (n > 0)
			{
			  px = 1;
			  n--;
			}
		      else
			px = 0;
		      if (pBox->BxMaxWidth > minsize && j < delta)
			{
			  /* now this column gets the maximum width */
			  colWidth[cRef] = -1;
			  still = TRUE;
			  var = var + j;
			  i--;
			  n++;
			}
		    }
		}
	    }
	  while (still && i > 0);
	}
      if (i > 0)
	{
	  j = width - var;
	  delta = j / i;
	  n =  j - (delta * i);
	}
      else
	{
	  delta = 0;
	  n = 0;
	}

      /* assign column widths */
      for (cRef = 0; cRef < cNumber; cRef++)
	{
	  pBox = colBox[cRef]->AbBox;
	  if (colPercent[cRef] != 0)
	    i = ((width - remainder) * colPercent[cRef] / 100) - pBox->BxWidth;
	  else if (colWidth[cRef] > 0)
	    i = colWidth[cRef] - pBox->BxWidth;
	  else if ((colWidth[cRef] < 0 || pBox->BxMaxWidth < minsize) &&
		   !constraint)
	    i = pBox->BxMaxWidth - pBox->BxWidth;
	  else
	    {
	      /* add extra pixels */
	      if (n > 0)
		{
		  px = 1;
		  n--;
		}
	      else
		px = 0;
	      if (useMax)
		i = pBox->BxMaxWidth + delta + px - pBox->BxWidth;
	      else
		i = pBox->BxMinWidth + delta + px - pBox->BxWidth;
	    }
	  ResizeWidth (pBox, pBox, NULL, i, 0, frame);
#ifdef TAB_DEBUG
printf ("Width[%d]=%d\n", cRef, pBox->BxWidth);
#endif
	}
    }

  table->AbBox->BxCycles = 0;
#ifdef TAB_DEBUG
printf(">>>>>>>>>>>>>>>>>%d\n", table->AbBox->BxWidth);
#endif
  if (pCell != NULL && (!force || checkEnclosing))
  {
    pBlock = SearchEnclosingType (table, BoBlock);
    if (pBlock != NULL)
      RecomputeLines (pBlock, NULL, NULL, frame);
    /* the current cell width have to be equal to the table width */
    /* we have to propage position to cell children */
    savePropagate = Propagate;
    Propagate = ToAll;
    UpdateColumnWidth (pCell, NULL, frame);
    /* restore propagate mode */
    Propagate = savePropagate;
  }

  if (table->AbBox->BxRows != NULL && table->AbBox->BxRows->TaRTable[0]->AbEnclosing != NULL)
    RecordEnclosing (table->AbBox->BxRows->TaRTable[0]->AbEnclosing->AbBox, FALSE);
  else
    RecordEnclosing (table->AbBox, FALSE);
}


/*----------------------------------------------------------------------
  UpdateTableWidth

   checks the table width and changes columns or table width if needed.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void     UpdateTableWidth (PtrAbstractBox table, int frame)
#else
static void     UpdateTableWidth (table, frame)
PtrAbstractBox  table;
int             frame;
#endif
{
  PtrAttribute        pAttr;
  PtrSSchema          pSS;
  PtrAbstractBox     *colBox, rowSpanCell[MAX_COLROW];
  PtrAbstractBox      pAb, row;
  PtrTabRelations     pTabRel;
  PtrDocument         pDoc;
  int                 rowSpans[MAX_COLROW];
  int                *colWidth, *colPercent;
  int                 cRef, i;
  int                 cNumber, rspanNumber;
  int                 attrHeight, attrVSpan;
  ThotBool             skip;
  ThotBool             modified;

  /* how many columns */
  pTabRel = table->AbBox->BxColumns;
  cNumber = 0;
  while (pTabRel != NULL)
    {
      for (i = 0; i < MAX_RELAT_DIM && pTabRel->TaRTable[i] != NULL;  i++)
	cNumber++;
      pTabRel = pTabRel->TaRNext;
    }
  if (cNumber == 0)
    return;

  colBox = TtaGetMemory (sizeof (PtrAbstractBox) * cNumber);
  colWidth = TtaGetMemory (sizeof (int) * cNumber);
  colPercent = TtaGetMemory (sizeof (int) * cNumber);

  pTabRel = table->AbBox->BxColumns;
  cRef = 0;
  while (pTabRel != NULL)
    {
      for (i = 0; i < MAX_RELAT_DIM && pTabRel->TaRTable[i] != NULL;  i++)
	{
	  colBox[cRef] = pTabRel->TaRTable[i];
	  colWidth[cRef] = pTabRel->TaRTWidths[i];
	  colPercent[cRef] = pTabRel->TaRTPercents[i];
	  cRef++;
	}
      pTabRel = pTabRel->TaRNext;
    }

  /* Now check the table size */
  CheckTableSize (table, cNumber, colBox, colWidth, colPercent, frame, TRUE);
  TtaFreeMemory (colBox);
  TtaFreeMemory (colWidth);
  TtaFreeMemory (colPercent);

  /* check vertically spanned cells */
  pSS = table->AbElement->ElStructSchema;
  rspanNumber = 0; /* no row-spanned cell */
  attrVSpan = GetAttrWithException (ExcRowSpan, pSS);
  attrHeight = GetAttrWithException (ExcNewHeight, pSS);
  if (attrVSpan != 0)
    {
      /* Enter a critical section */
      ComputeColInWork = TRUE;
      pDoc = LoadedDocument[FrameTable[frame].FrDoc - 1];
      modified = pDoc->DocModified;
      /* process all rows */
      pTabRel = table->AbBox->BxRows;
      while (pTabRel != NULL)
	{
	  for (i = 0; i < MAX_RELAT_DIM && pTabRel->TaRTable[i] != NULL; i++)
	    {
	      /* process all cells in the row */
	      row = pTabRel->TaRTable[i];
	      /* remove existing Height attribute */
	      GetAttribute (&pAttr);
	      pAttr->AeAttrSSchema = pSS;
	      pAttr->AeAttrNum = attrHeight;
	      pAttr->AeAttrType = AtNumAttr;
	      pAttr->AeAttrValue = MAX_INT_ATTR_VAL + 1;
	      AttachAttrWithValue (row->AbElement, pDoc, pAttr);
	      DeleteAttribute (NULL, pAttr);
	      cRef = 0;
	      pAb = row;
	      skip = FALSE;
	      while (pAb != NULL && cRef < cNumber)
		{
		  if (skip)
		    {
		      pAb = NextSiblingAbsBox (pAb, row);
		      skip = FALSE;
		    }
		  else
		    pAb = SearchNextAbsBox (pAb, row);

		  if (pAb != NULL && !pAb->AbDead && pAb->AbBox != NULL &&
		      TypeHasException (ExcIsCell, pAb->AbElement->ElTypeNumber, pSS) &&
		      !pAb->AbPresentationBox)
		    {
		      pAttr = pAb->AbElement->ElFirstAttr;
		      while (pAttr != NULL)
			{
			  if (pAttr->AeAttrNum == attrVSpan &&
			      pAttr->AeAttrSSchema->SsCode == pSS->SsCode)
			    {
			      /* rowspan on this cell */
			      if (pAttr->AeAttrValue > 1 && rspanNumber < MAX_COLROW)
				{
				  /* register current cell and span value */
				  rowSpans[rspanNumber] = pAttr->AeAttrValue;
				  rowSpanCell[rspanNumber++] = pAb;
				}
			      pAttr = NULL;
			    }
			  else
			    pAttr = pAttr->AeNext;
			}
		    }
		}
	    }
	  pTabRel = pTabRel->TaRNext;
	}
      /* Now check row heights */
      CheckRowHeights (table, rspanNumber, rowSpanCell, rowSpans, frame);
      pDoc->DocModified = modified;
      /* Exit a critical section */
      ComputeColInWork = FALSE;
    }
}

/*----------------------------------------------------------------------
  ComputeColWidth computes the minimum width and the maximum width of a
  specific column or all columns if col equal NULL.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void     ComputeColWidth (PtrAbstractBox col, PtrAbstractBox table, int frame)
#else
static void     ComputeColWidth (col, table, frame)
PtrAbstractBox  col;
PtrAbstractBox  table;
int             frame;
#endif
{
  PtrAttribute        pAttr;
  PtrSSchema          pSS;
  PtrDocument         pDoc;
  PtrTabRelations     pTabRel;
  PtrAbstractBox     *colBox, rowSpanCell[MAX_COLROW];
  PtrAbstractBox      pAb, row, cell;
  int                *colMinWidth, *colMaxWidth;
  int                *colWidth, *colPercent, *colVSpan;
  int                 colSpan_MinWidth[MAX_COLROW], colSpan_Percent[MAX_COLROW];
  int                 colSpan_Width[MAX_COLROW], colSpan_MaxWidth[MAX_COLROW];
  int                 colSpan_First[MAX_COLROW], colSpan_Last[MAX_COLROW];
  int                 rowSpans[MAX_COLROW];
  int                 cNumber, spanNumber, rspanNumber;
  int                 span, delta, j;
  int                 width, i, cRef;
  int                 min, max, percent, realMin, realMax;
  int                 attrVSpan, attrHSpan;
  int                 attrHeight, cellWidth;
  int                 tabWidth, tabPercent, minsize;
  ThotBool             skip, statusColInWork;
  ThotBool             foundH, foundV;
  ThotBool             modified;

  if (col != NULL)
    if (col->AbBox == NULL)
      return;

  pDoc = LoadedDocument[FrameTable[frame].FrDoc - 1];
  modified = pDoc->DocModified;

  pSS = table->AbElement->ElStructSchema;
  /* how many columns */
  pTabRel = table->AbBox->BxColumns;
  cNumber = 0;
  while (pTabRel != NULL)
    {
      for (i = 0; i < MAX_RELAT_DIM &&
	     pTabRel->TaRTable[i] != NULL &&
	     pTabRel->TaRTable[i]->AbBox != NULL;  i++)
	cNumber++;
      pTabRel = pTabRel->TaRNext;
    }
  if (cNumber == 0)
    return;

  /* Enter a critical section */
  statusColInWork = ComputeColInWork;
  ComputeColInWork = TRUE;
  /* register widths of each columns */
  pTabRel = table->AbBox->BxColumns;
  cRef = 0;
  colBox = TtaGetMemory (sizeof (PtrAbstractBox) * cNumber);
  colMinWidth = TtaGetMemory (sizeof (int) * cNumber);
  colWidth = TtaGetMemory (sizeof (int) * cNumber);
  colPercent = TtaGetMemory (sizeof (int) * cNumber);
  colMaxWidth = TtaGetMemory (sizeof (int) * cNumber);
  colVSpan = TtaGetMemory (sizeof (int) * cNumber);
  while (pTabRel != NULL)
    {
      for (i = 0; i < MAX_RELAT_DIM &&
	     pTabRel->TaRTable[i] != NULL &&
	     pTabRel->TaRTable[i]->AbBox != NULL;  i++)
	{
	  colBox[cRef] = pTabRel->TaRTable[i];
	  if (colBox[cRef] == col || col == NULL)
	    {
	      /* to recompute */
	      colMinWidth[cRef] = 0;
	      colMaxWidth[cRef] = 0;
	      GiveAttrWidth (colBox[cRef], &width, &percent);
	      colWidth[cRef] = width;
	      colPercent[cRef] = percent;
	    }
	  else
	    {
	      colWidth[cRef] = pTabRel->TaRTWidths[i];
	      colPercent[cRef] = pTabRel->TaRTPercents[i];
	      colMinWidth[cRef] = colBox[cRef]->AbBox->BxMinWidth;
	      colMaxWidth[cRef] = colBox[cRef]->AbBox->BxMaxWidth;
	    }
	  colVSpan[cRef] = 0;
	  cRef++;
	}
      pTabRel = pTabRel->TaRNext;
    }

  attrVSpan = GetAttrWithException (ExcRowSpan, pSS);
  attrHSpan = GetAttrWithException (ExcColSpan, pSS);
  attrHeight = GetAttrWithException (ExcNewHeight, pSS);
  spanNumber = 0; /* no col-spanned cell */
  rspanNumber = 0; /* no row-spanned cell */
  /* process all rows */
  pTabRel = table->AbBox->BxRows;
  while (pTabRel != NULL)
    {
      for (i = 0; i < MAX_RELAT_DIM && pTabRel->TaRTable[i] != NULL; i++)
	{
	  /* process all cells in the row */
	  row = pTabRel->TaRTable[i];
      if (attrHeight != 0) {
	  /* remove existing Height attribute */
	  GetAttribute (&pAttr);
	  pAttr->AeAttrSSchema = pSS;
	  pAttr->AeAttrNum = attrHeight;
	  pAttr->AeAttrType = AtNumAttr;
	  pAttr->AeAttrValue = MAX_INT_ATTR_VAL + 1;
	  AttachAttrWithValue (row->AbElement, pDoc, pAttr);
	  DeleteAttribute (NULL, pAttr);
	  }

	  cRef = 0;
	  pAb = row;
	  skip = FALSE;
	  while (pAb != NULL && cRef < cNumber)
	    {
	      if (skip)
		{
		  pAb = NextSiblingAbsBox (pAb, row);
		  skip = FALSE;
		}
	      else
		pAb = SearchNextAbsBox (pAb, row);
	      
	      if (pAb != NULL && !pAb->AbDead && pAb->AbBox != NULL &&
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

		  if (pAb != NULL)
		    {
		      /* is it vertically or horizontally spanned ? */
		      span = 1;
		      if (attrVSpan != 0 || attrHSpan != 0)
			{
			  /* is this attribute attached to the cell */
			  pAttr = pAb->AbElement->ElFirstAttr;
			  foundH = FALSE;
			  foundV = FALSE;
			  while ((!foundH || !foundV) && pAttr != NULL)
			    {
			      if (pAttr->AeAttrNum == attrVSpan &&
				  pAttr->AeAttrSSchema->SsCode == pSS->SsCode)
				{
				  if (pAttr->AeAttrType == AtEnumAttr || pAttr->AeAttrType == AtNumAttr)
				    {
				      /* rowspan on this cell */
				      colVSpan[cRef] = pAttr->AeAttrValue - 1;
				      if (colVSpan[cRef] > 0 && rspanNumber < MAX_COLROW)
					{
					  /* register current cell and span value */
					  rowSpans[rspanNumber] = pAttr->AeAttrValue;
					  rowSpanCell[rspanNumber++] = pAb;
					  foundV = TRUE;
					}
				    }
				}
			      if (pAttr->AeAttrNum == attrHSpan &&
				       pAttr->AeAttrSSchema->SsCode == pSS->SsCode)
				{
				  foundH = TRUE;
				  if (pAttr->AeAttrType == AtEnumAttr || pAttr->AeAttrType == AtNumAttr)
				    {
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
		      if (span == 1 && col != 0 && col != colBox[cRef])
			{
			  /* the column width is already known */
			  pAb = NULL;
			  skip = TRUE;
			}
		      else
			{
			  if (GiveAttrWidth (cell, &cellWidth, &percent))
			    {
			      /*percent += delta / table->AbBox.BxWidth;*/
			      if (percent > colPercent[cRef] && span == 1)
				colPercent[cRef] = percent;
			    }

			  /* change the cell width rule if needed */
			  if (span == 1 && cell->AbWidth.DimAbRef != colBox[cRef])
			    {
			      /* there is a column width attribute */
			      if (cell->AbPrevious != NULL &&
				  cell->AbPrevious->AbPresentationBox)
				/* this is a glitch to find out the border width */
				delta = cell->AbPrevious->AbWidth.DimValue;
			      else
				delta = 0;
			      /*cell->AbWidth.DimAbRef = colBox[cRef];
			      cell->AbWidth.DimValue = -delta;
			      cell->AbWidth.DimUnit = UnPixel;
			      cell->AbWidth.DimSameDimension = TRUE;
			      ComputeDimRelation (cell, frame, TRUE);*/
			    }

			  /* process elements in this cell */
			  min = 8;
			  max = 8;
			  width = cell->AbBox->BxWidth;
			  while (pAb != NULL)
			    {
			      if (skip)
				{
				  pAb = NextSiblingAbsBox (pAb, cell);
				  skip = FALSE;
				}
			      else
				pAb = SearchNextAbsBox (pAb, cell);
			      if (pAb != NULL && !pAb->AbDead &&
				  pAb->AbBox != NULL && !pAb->AbPresentationBox)
				{
				  /* diff between cell's and box's position */
				  if (pAb->AbBox->BxHorizEdge == Left ||
				      pAb->AbBox->BxHorizEdge == VertRef)
				    {
				      delta = pAb->AbBox->BxXOrg - cell->AbBox->BxXOrg;
				      if (delta < 0)
					delta = 0;
				    }
				  else
				    delta = 0;
				  if (pAb->AbBox->BxType == BoBlock ||
				      pAb->AbBox->BxType == BoTable)
				    {
				      if (min < pAb->AbBox->BxMinWidth + delta)
					min = pAb->AbBox->BxMinWidth + delta;
				      if (max < pAb->AbBox->BxMaxWidth + delta)
					max = pAb->AbBox->BxMaxWidth + delta;
				      skip = TRUE;
				    }
				  else if (!pAb->AbWidth.DimIsPosition &&
					   pAb->AbHorizEnclosing &&
					   (pAb->AbWidth.DimAbRef == NULL ||
					    !IsParentBox (pAb->AbWidth.DimAbRef->AbBox, pAb->AbBox)))
				    {
				      /* the box width doesn't depend on cell width */
				      if (!TypeHasException (ExcPageBreakRepBefore, pAb->AbElement->ElTypeNumber, pSS) &&
					  !TypeHasException (ExcPageBreakRepetition, pAb->AbElement->ElTypeNumber, pSS) &&
					  pAb->AbElement->ElTypeNumber != PageBreak + 1)
					{
					  /* the box is not generated by a page break */
					  if (min < pAb->AbBox->BxWidth + delta)
					    min = pAb->AbBox->BxWidth + delta;
					  if (max < pAb->AbBox->BxWidth + delta)
					    max = pAb->AbBox->BxWidth + delta;
					}
				      skip = TRUE;
				    }
				}
			    }

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
			      delta = colBox[cRef]->AbBox->BxWidth - width;
			      if (delta < 0)
				delta = 0;
			      if (colMinWidth[cRef] < min + delta)
				colMinWidth[cRef] = min + delta;
			      if (colMaxWidth[cRef] < max + delta)
				colMaxWidth[cRef] = max + delta;
			      if (cellWidth >= colMinWidth[cRef])
				{
				  if (cellWidth > colWidth[cRef])
				    colWidth[cRef] = cellWidth;
				}
			      else
				/* we need to apply the min rule */
				colWidth[cRef] = 0;
			    }
			}
		      /* next column */
		      pAb = cell;
		      skip = TRUE;
		      cRef += span;
		    }
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
  if (!GiveAttrWidth (table, &tabWidth, &tabPercent))
    /* limit given by available space */
    tabWidth = table->AbEnclosing->AbBox->BxWidth;
  else if (tabPercent != 0)
    {
      /* the table width is given by a percent value */
      tabWidth = table->AbEnclosing->AbBox->BxWidth * tabPercent / 100;
    }
  minsize = (int)(((float) tabWidth /(float) cNumber) /2.);
  if (col == NULL)
    {
      /* manage spanned columns */
      for (i = 0; i < spanNumber; i++)
	{
	  min = 0;
	  max = 0;
	  realMin = 0;
	  realMax = 0;
	  percent = 0;
	  span = 0;
	  width = 0;
	  j = 0;
	  for (cRef = colSpan_First[i]; cRef <= colSpan_Last[i]; cRef++)
	    {
	      if (colPercent[cRef] != 0)
		percent += colPercent[cRef];
	      else if (colWidth[cRef] != 0)
		width += colWidth[cRef];
	      else if (colMaxWidth[cRef] < minsize)
		{
		  width += colMaxWidth[cRef];
		  j++;
		}
	      else
		{
		  min += colMinWidth[cRef];
		  max += colMaxWidth[cRef];
		  span++;
		  j++;
		}
	      realMin += colMinWidth[cRef];
	      realMax += colMaxWidth[cRef];
	    }
	  
	  /* compare percent values */
	  if (colSpan_Percent[i] > percent)
	    {
	      delta = colSpan_Percent[i] - percent;
	      if (j > 0)
		{
		  delta = delta / j;
		  for (cRef = colSpan_First[i]; cRef <= colSpan_Last[i]; cRef++)
		    if (colPercent[cRef] == 0 && colWidth[cRef] == 0)
		      colPercent[cRef] = delta;
		}
	      percent = colSpan_Percent[i];
	    }
	  /* compare width values */
	  if (colSpan_Width[i] > width + min)
	    {
	      delta = colSpan_Width[i] - width - min;
	      if (j > 0)
		{
		  delta = delta / j;
		  for (cRef = colSpan_First[i]; cRef <= colSpan_Last[i]; cRef++)
		    if (colPercent[cRef] == 0 && colWidth[cRef] == 0)
		      if (delta > colMinWidth[cRef])
			colWidth[cRef] = delta;
		      else
			colWidth[cRef] = colMinWidth[cRef];
		}
	      width = colSpan_Width[i] - min;
	    }
	  
	  /* compare min and max values */
	  /*percent = percent * tabWidth / 100;
	  min = min + width + percent;
	  max = max + width + percent;*/
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
		if ((colPercent[cRef] == 0 && colWidth[cRef] == 0 &&
		     colMaxWidth[cRef] > minsize) || span == 0)
		  colMinWidth[cRef] += width;
	    }
	  if (colSpan_MaxWidth[i] > realMax)
	    {
	      /* change width of included columns */
	      width = colSpan_MaxWidth[i] - realMax;
	      if (span > 0)
		width = (width + span - 1) / span;
	      else
		{
		  delta = colSpan_Last[i] - colSpan_First[i] + 1;
		  width = (width + delta - 1) / delta;
		}
	      for (cRef = colSpan_First[i]; cRef <= colSpan_Last[i]; cRef++)
		if ((colPercent[cRef] == 0 && colWidth[cRef] == 0 &&
		     colMaxWidth[cRef] > minsize) || span == 0)
		  colMaxWidth[cRef] += width;
	    }
	}
    }

  /* now update column boxes */
  pTabRel = table->AbBox->BxColumns;
  cRef = 0;
  while (pTabRel != NULL)
    {
      for (i = 0; i < MAX_RELAT_DIM && pTabRel->TaRTable[i] != NULL;  i++)
	{
	  if (colWidth[cRef] != 0 && colWidth[cRef] < colMinWidth[cRef])
	    colWidth[cRef] = colMinWidth[cRef];
	  pTabRel->TaRTWidths[i] = colWidth[cRef];
	  pTabRel->TaRTPercents[i] = colPercent[cRef];
	  colBox[cRef]->AbBox->BxMinWidth = colMinWidth[cRef];
	  colBox[cRef]->AbBox->BxMaxWidth = colMaxWidth[cRef];
	  cRef++;
	}
      pTabRel = pTabRel->TaRNext;
    }

  TtaFreeMemory (colVSpan);
  TtaFreeMemory (colMinWidth);
  TtaFreeMemory (colMaxWidth);
   /* Now check the table size */
  CheckTableSize (table, cNumber, colBox, colWidth, colPercent, frame, FALSE);
  TtaFreeMemory (colBox);
  TtaFreeMemory (colWidth);
  TtaFreeMemory (colPercent);
  /* Now check row heights */
  CheckRowHeights (table, rspanNumber, rowSpanCell, rowSpans, frame);
  pDoc->DocModified = modified;
  /* Perhaps exit the critical section */
  ComputeColInWork = statusColInWork;
}


/*----------------------------------------------------------------------
  SaveColUpdate stores a new call to ComputeColWidth
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void     SaveColUpdate (PtrAbstractBox col, PtrAbstractBox table, int frame)
#else
static void     SaveColUpdate (col, table, frame)
PtrAbstractBox  col;
PtrAbstractBox  table;
int             frame;
#endif
{
  PtrTabUpdate pTabUpdate, pPrevTabUpdate;
  ThotBool      found;

  if (table == NULL)
    return;

  pPrevTabUpdate = NULL;
  pTabUpdate = FirstColUpdate;
  /* look at if a similar update is already registered */
  found = FALSE;
  while (!found && pTabUpdate != NULL)
    {
      pPrevTabUpdate = pTabUpdate;
      found = (pTabUpdate->TaUTable == table);
      if (found && col != pTabUpdate->TaUColumn && pTabUpdate->TaUColumn != NULL)
	/* update the entry */
	pTabUpdate->TaUColumn = NULL;
      else if (!found)
	pTabUpdate = pTabUpdate->TaUNext;
    }

  if (!found)
    {
      /* create a new entry */
      pTabUpdate = (PtrTabUpdate) TtaGetMemory (sizeof (TabUpdate));
      pTabUpdate->TaUNext = NULL;
      pTabUpdate->TaUTable = table;
      pTabUpdate->TaUColumn = col;
      pTabUpdate->TaUFrame = frame;
      if (pPrevTabUpdate != NULL)
	pPrevTabUpdate->TaUNext = pTabUpdate;
      else
	FirstColUpdate = pTabUpdate;
    }
}


/*----------------------------------------------------------------------
  ComputeColUpdates computes calls to ComputeColWidth
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void     ComputeColUpdates (Document document)
#else
static void     ComputeColUpdates (document)
Document        document;
#endif
{
  PtrTabUpdate pTabUpdate, pPrevTabUpdate, pNextTabUpdate;

  /* check if we are executing a ComputeColWidth */
  if (ComputeColInWork)
    return;
  else
    /* avoid to re-execute two times the same work */
    ComputeColInWork = TRUE;

  pPrevTabUpdate = NULL;
  pTabUpdate = FirstColUpdate;
  while (pTabUpdate != NULL)
    {
      if (FrameTable[pTabUpdate->TaUFrame].FrDoc == document)
	{
	  pNextTabUpdate = pTabUpdate->TaUNext;
	  /* update the list of column updates */
	  if (pPrevTabUpdate == NULL)
	    FirstColUpdate = pNextTabUpdate;
	  else
	    pPrevTabUpdate->TaUNext = pNextTabUpdate;
	  /* execute the update */
	  ComputeColWidth (pTabUpdate->TaUColumn, pTabUpdate->TaUTable, pTabUpdate->TaUFrame);
	  /* free the context and move to the next one */
	  TtaFreeMemory (pTabUpdate);
	  pTabUpdate = pNextTabUpdate;
	}
      else
	{
	  pPrevTabUpdate = pTabUpdate;
	  pTabUpdate = pTabUpdate->TaUNext;
	}
    }
  ComputeColInWork = FALSE;
}

/*----------------------------------------------------------------------
   UpdateColumnWidth
   checks the column width and changes columns or table width if needed.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void      UpdateColumnWidth (PtrAbstractBox cell, PtrAbstractBox col, int frame)
#else
static void      UpdateColumnWidth (cell, col, frame)
PtrAbstractBox   cell;
PtrAbstractBox   col;
int              frame;
#endif
{
  PtrAbstractBox      table;
  PtrAbstractBox      row;
  PtrAttribute        pAttr;
  PtrElement          pEl, pRefEl;
  PtrSSchema          pSS;
  int                 attrNum, span;
  ThotBool             found;

  /* look for the table */
  table = NULL;
  span = 1;
  if (col != NULL && col->AbBox != NULL)
    /* get the table element */
    table = (PtrAbstractBox) col->AbBox->BxTable;
  else if (cell != NULL && cell->AbBox != NULL)
    {
      /* get row and table elements */
      row = SearchEnclosingType (cell, BoRow);
      if (row != NULL  && row->AbBox != NULL)
	table = (PtrAbstractBox) row->AbBox->BxTable;

      if (table != NULL && !table->AbNew && !table->AbDead &&
	  table->AbBox != NULL && table->AbBox->BxCycles == 0)
	{
	  pEl = cell->AbElement;
	  pSS = pEl->ElStructSchema;
	  col = NULL;
	  /* check if there is a colspan attribute */
	  attrNum = GetAttrWithException (ExcColSpan, pSS);
	  if (attrNum != 0)
	    {
	      /* search this attribute attached to the cell element */
	      pAttr = pEl->ElFirstAttr;
	      found = FALSE;
	      while (!found && pAttr != NULL)
		if (pAttr->AeAttrNum == attrNum &&
		    pAttr->AeAttrSSchema->SsCode == pSS->SsCode)
		  {
		    found = TRUE;
		    if (pAttr->AeAttrType == AtEnumAttr || pAttr->AeAttrType == AtNumAttr)
		      span = pAttr->AeAttrValue;
		  }
		else
		  pAttr = pAttr->AeNext;
	    }
	  if (span == 1)
	    {
	      /* search the refered column */
	      attrNum = GetAttrWithException (ExcColRef, pSS);
	      if (attrNum != 0)
		{
		  /* search this attribute attached to the cell element */
		  pAttr = pEl->ElFirstAttr;
		  found = FALSE;
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
		}
	    }
	}
    }
 
  if (table != NULL && !table->AbNew && !table->AbDead &&
      table->AbBox != NULL && table->AbBox->BxCycles == 0 &&
      (col != NULL || span > 1))
    {
      /* the table exists, compute the column width */
      if (documentDisplayMode[FrameTable[frame].FrDoc - 1] == DisplayImmediately ||
	  documentDisplayMode[FrameTable[frame].FrDoc - 1] == DeferredDisplay)
	ComputeColWidth (col, table, frame);
      else
	SaveColUpdate (col, table, frame);
    }
}


/*----------------------------------------------------------------------
   UpdateTable checks information about the table element after creation of:
   - a table (table != NULL)
   - a column (table == NULL && col != NULL)
   - a row (table == NULL && row != NULL)
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void      UpdateTable (PtrAbstractBox table, PtrAbstractBox col, PtrAbstractBox row, int frame)
#else
static void      UpdateTable (table, col, row, frame)
PtrAbstractBox   table;
PtrAbstractBox   col;
PtrAbstractBox   row;
int              frame;
#endif
{
  PtrAbstractBox      pAb;

  if (table == NULL)
    {
      /* look for the table */
      if (col != NULL && col->AbBox != NULL)
	{
	  pAb = (PtrAbstractBox) col->AbBox->BxTable;
	  if (pAb == NULL)
	    {
	      pAb = SearchEnclosingType (col, BoTable);
	      /* during table building each column needs a minimum width */
	      if (col->AbBox->BxWidth < 20)
		ResizeWidth (col->AbBox, col->AbBox, NULL, 20 - col->AbBox->BxWidth, 0, frame);
	    }
	}
      else if (row != NULL && row->AbBox != NULL)
	{
	  pAb = (PtrAbstractBox) row->AbBox->BxTable;
	  if (pAb == NULL)
	    pAb = SearchEnclosingType (row, BoTable);
	}
      else
	pAb = NULL;
    }
  else
    pAb = table;

  if (pAb != NULL && pAb->AbBox != NULL && !pAb->AbNew && !pAb->AbDead)
    {
      /* the table box has been created */
      if (table != NULL || col != NULL)
	/* build or rebuild columns list */
	BuildColOrRowList (pAb, BoColumn);
      if (table != NULL || row != NULL)
	/* build or rebuild rows list */
	BuildColOrRowList (pAb, BoRow);
      
      /* compute widths of each column within the table */
      if (documentDisplayMode[FrameTable[frame].FrDoc - 1] == DisplayImmediately || table != NULL)
	ComputeColWidth (col, pAb, frame);
      else
	SaveColUpdate (col, pAb, frame);
    }
}


/*----------------------------------------------------------------------
   ClearTable removes table information
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void      ClearTable (PtrAbstractBox table)
#else
static void      ClearTable (table)
PtrAbstractBox   table;
#endif
{
  /* free specific blocks */
  FreeTaRBlock (table->AbBox->BxColumns);
  table->AbBox->BxColumns = NULL;
  FreeTaRBlock (table->AbBox->BxRows);
  table->AbBox->BxRows = NULL;
}


/*----------------------------------------------------------------------
   ClearTable removes table information
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void      IsFirstColumn (PtrAbstractBox cell, PtrAbstractBox table, ThotBool *result)
#else
static void      IsFirstColumn (cell, table, result)
PtrAbstractBox   cell;
PtrAbstractBox   table;
oolean          *result;
#endif
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
   TableHLoadResources : connect resources for managing HTML tables
  ----------------------------------------------------------------------*/
void                TableHLoadResources ()
{

   if (ThotLocalActions[T_checktable] == NULL)
     {
	/* initialisations */
        FirstColUpdate = NULL;
	/* connexion des ressources */
	TteConnectAction (T_checktable, (Proc) UpdateTable);
	TteConnectAction (T_checkcolumn, (Proc) UpdateColumnWidth);
	TteConnectAction (T_resizetable, (Proc) UpdateTableWidth);
	TteConnectAction (T_cleartable, (Proc) ClearTable);
	TteConnectAction (T_firstcolumn, (Proc) IsFirstColumn);
	TteConnectAction (T_colupdates, (Proc) ComputeColUpdates);
     }
}

/* End Of Module Table2 */
