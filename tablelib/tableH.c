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

#define THOT_EXPORT extern
#include "appdialogue_tv.h"
#include "boxes_tv.h"

#include "boxmoves_f.h"
#include "boxrelations_f.h"
#include "buildboxes_f.h"
#include "buildlines_f.h"
#include "createabsbox_f.h"
#include "exceptions_f.h"
#include "memory_f.h"
#define MAX_COLS 50
/*#define TAB_DEBUG */

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
  boolean           skip;

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
  boolean             loop;
  boolean             empty;

  /* select the rigth list */
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
  boolean             found;

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
  boolean             found;

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
static boolean  GiveAttrWidth (PtrAbstractBox pAb, int *width, int *percent)
#else
static boolean  GiveAttrWidth (pAb, width, percent)
PtrAbstractBox  pAb;
int            *width;
int            *percent;
#endif
{
  PtrSSchema          pSS;
  PtrAttribute        pAttr;
  int                 attrWidth, attrPercent;
  boolean             found;

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
  CheckTableSize computes the minimum width and the maximum width of a
  table.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void     CheckTableSize (PtrAbstractBox table, int cNumber, PtrAbstractBox *colBox, int *colWidth, int *colPercent,  int frame, boolean force)
#else
static void     CheckTableSize (table, cNumber, colBox, colWidth, colpercent, frame, force)
PtrAbstractBox  table;
int             cNumber;
PtrAbstractBox *colBox;
int            *colWidth;
int            *colPercent;
int             frame;
boolean         force;
#endif
{
  PtrAbstractBox      pBlock, pCell;
  PtrBox              pBox;
  Propagation         savePropagate;
  int                 j, var, delta, px, val;
  int                 width, i, cRef, minsize;
  int                 min, max, sum, remainder;
  int                 percent, sumPercent, n;
  int                 realMin, realMax;
  boolean             constraint, still;
  boolean             useMax, checkEnclosing;

  percent = 0;
  /* Now check the table size */
  if (cNumber == 0)
    return;
  else if (table->AbBox->BxCycles != 0)
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
#ifdef TAB_DEBUG
printf("<<<<<<<<<<<<<<<%d\n", table->AbBox->BxWidth);
#endif
  minsize = (int)((float) width /(float) cNumber) /2.;
  for (cRef = 0; cRef < cNumber; cRef++)
    {
      pBox = colBox[cRef]->AbBox;
      if (pBox->BxWidth > 0)
	delta += pBox->BxWidth;
      /* how many columns can be modified and what is current values */
      realMin += pBox->BxMinWidth;
      realMax += pBox->BxMaxWidth;
      if (colPercent[cRef] != 0)
	sumPercent += colPercent[cRef];
      else if (colWidth[cRef] != 0)
	{
	  sum += colWidth[cRef];
	  var += colWidth[cRef];
	}
      else
	{
	  min += pBox->BxMinWidth;
	  max += pBox->BxMaxWidth;
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
#ifdef TAB_DEBUG
printf ("cref=%d: Min =%d, Max=%d, colWidth=%d, colPercent=%d\n", cRef, pBox->BxMinWidth, pBox->BxMaxWidth, colWidth[cRef], colPercent[cRef]);
#endif
    }

  /* additional space for rows */ 
  if (table->AbBox->BxRows != NULL && table->AbBox->BxRows->TaRTable[0] != NULL &&
      table->AbBox->BxRows->TaRTable[0]->AbFirstEnclosed != NULL &&
      table->AbBox->BxRows->TaRTable[0]->AbFirstEnclosed->AbPresentationBox)
    remainder = table->AbBox->BxRows->TaRTable[0]->AbFirstEnclosed->AbWidth.DimValue;
  else
    remainder = 0;
  if (min + sum > 0)
    {
      /* there is almost one column width not given by a % value */
      sum += remainder;
      realMin += remainder;
      realMax += remainder;
    }

  /* check if percent values are ok */
  if (width > 0 && percent == 0)
    delta = (min + sum) * 100 / width;
  else
    delta = 0;
  if (sumPercent + delta > 100 && sumPercent > 0)
    {
      /* reduce each colPercent */
      val = sumPercent + delta - 100;
      n = 0;
      for (cRef = 0; cRef < cNumber; cRef++)
	if (colPercent[cRef] != 0)
	  n++;
      delta = (val + n - 1) / n;
      sumPercent = 0;
      for (cRef = 0; cRef < cNumber; cRef++)
	if (colPercent[cRef] != 0)
	  {
	    colPercent[cRef] -= delta;
	    sumPercent += colPercent[cRef];
	  }
    }
  sumPercent = sumPercent * width / 100;
  min = min + sum + sumPercent;
  max = max + sum + sumPercent;

  /* check if the contraint is valid */
  if (constraint && min > width)
    {
      constraint = FALSE;
      if (percent != 0)
	{
	  /* limit given by precent of available space */
	  if (table->AbEnclosing->AbBox->BxWidth > 0 && pCell == NULL)
	    {
	      percent = min * 100 / table->AbEnclosing->AbBox->BxWidth;
	      SetAttrWidthPercent (table, percent);
	      table->AbWidth.DimValue = percent;
	    }
	}
      else
	{
	  SetAttrWidthPxl (table, min);
	  /* update the width rule of table */
	  table->AbWidth.DimValue = min;
	}
      checkEnclosing = TRUE;
    }

  if (constraint && percent == 0)
    {
      table->AbBox->BxMinWidth = width;
      table->AbBox->BxMaxWidth = width;
    }
  else
    {
      table->AbBox->BxMinWidth = realMin;
      table->AbBox->BxMaxWidth = realMax;
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
	      j = width - var - sumPercent;
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
			  /* now this column takes its maximum width */
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
	  j = width - var - sumPercent;
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
  PtrAbstractBox     *colBox;
  PtrTabRelations     pTabRel;
  int                *colWidth, *colPercent;
  int                 cRef, i;
  int                 cNumber;

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
  PtrTabRelations     pTabRel;
  PtrAbstractBox     *colBox;
  PtrAbstractBox      pAb, row, cell;
  int                *colSpan_First, *colSpan_Last;
  int                *colVSpan;
  int                *colSpan_MinWidth, *colSpan_MaxWidth;
  int                *colSpan_Width, *colSpan_Percent;
  int                *colMinWidth, *colMaxWidth;
  int                *colWidth, *colPercent;
  int                 cNumber, spanNumber;
  int                 span, delta, j;
  int                 width, i, cRef;
  int                 min, max, percent;
  int                 attrVSpan, attrHSpan, cellWidth;
  int                 tabWidth, tabPercent, minsize;
  boolean             skip;
  boolean             foundH, foundV;

  if (col != NULL)
    if (col->AbBox == NULL)
      return;

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

  colSpan_MinWidth = TtaGetMemory (sizeof (int) * MAX_COLS);
  colSpan_MaxWidth = TtaGetMemory (sizeof (int) * MAX_COLS);
  colSpan_Width = TtaGetMemory (sizeof (int) * MAX_COLS);
  colSpan_Percent = TtaGetMemory (sizeof (int) * MAX_COLS);
  colSpan_First = TtaGetMemory (sizeof (int) * MAX_COLS);
  colSpan_Last = TtaGetMemory (sizeof (int) * MAX_COLS);
  attrVSpan = GetAttrWithException (ExcRowSpan, pSS);
  attrHSpan = GetAttrWithException (ExcColSpan, pSS);
  spanNumber = 0;
  /* process all rows */
  pTabRel = table->AbBox->BxRows;
  while (pTabRel != NULL)
    {
      for (i = 0; i < MAX_RELAT_DIM && pTabRel->TaRTable[i] != NULL; i++)
	{
	  /* process all cells in the row */
	  row = pTabRel->TaRTable[i];
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
				  foundV = TRUE;
				  if (pAttr->AeAttrType == AtEnumAttr || pAttr->AeAttrType == AtNumAttr)
				    colVSpan[cRef] = pAttr->AeAttrValue - 1;
				}
			      else if (pAttr->AeAttrNum == attrHSpan &&
				       pAttr->AeAttrSSchema->SsCode == pSS->SsCode)
				{
				  foundH = TRUE;
				  if (pAttr->AeAttrType == AtEnumAttr || pAttr->AeAttrType == AtNumAttr)
				    {
				      span = pAttr->AeAttrValue;
				      /* it colud be an invalid span */
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
			      /* there is a column width attribute */
			      if (cell->AbPrevious != NULL &&
				  cell->AbPrevious->AbPresentationBox)
				/* this is a glitch to find out the border width */
				delta = cell->AbPrevious->AbWidth.DimValue;
			      else
				delta = 0;
			      if (cellWidth > 0)
				{
				  cellWidth += delta;
				  if (cellWidth > colWidth[cRef] && span == 1)
				    colWidth[cRef] = cellWidth;
				}
			      else
				{
				  /*percent += delta / table->AbBox.BxWidth;*/
				  if (percent > colPercent[cRef] && span == 1)
				    colPercent[cRef] = percent;
				}

			      /* change the cell width rule if needed */
			      if (span == 1 && cell->AbWidth.DimAbRef != colBox[cRef])
				{
				  cell->AbWidth.DimAbRef = colBox[cRef];
				  cell->AbWidth.DimValue = -delta;
				  cell->AbWidth.DimUnit = UnPixel;
				  cell->AbWidth.DimSameDimension = TRUE;
				  ComputeDimRelation (cell, frame, TRUE);
				}
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
				  /* diff between cell and box widths */
				  delta = width - pAb->AbBox->BxWidth;
				  if (delta < 0)
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
				      if (min < pAb->AbBox->BxWidth + delta)
					min = pAb->AbBox->BxWidth + delta;
				      if (max < pAb->AbBox->BxWidth + delta)
					max = pAb->AbBox->BxWidth + delta;
				      skip = TRUE;
				    }
				}
			    }

			  /* update the min and max of the column */
			  if (span > 1 && spanNumber < MAX_COLS)
			    {
			      colSpan_MinWidth[spanNumber] = min;
			      colSpan_MaxWidth[spanNumber] = max;
			      colSpan_Width[spanNumber] = cellWidth;
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
      /* limit given by percent of available space */
      tabWidth = table->AbEnclosing->AbBox->BxWidth * tabPercent / 100;
    }
  minsize = (int)((float) tabWidth /(float) cNumber) /2.;
  if (col == NULL)
    {
      /* manage spanned columns */
      for (i = 0; i < spanNumber; i++)
	{
	  min = 0;
	  max = 0;
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
	  percent = percent * tabWidth / 100;
	  min = min + width + percent;
	  max = max + width + percent;
	  if (colSpan_MinWidth[i] > min)
	    {
	      /* change width of included columns */
	      width = colSpan_MinWidth[i] - min;
	      if (span > 0)
		width = (width + span - 1) / span;
	      else
		{
		  delta = colSpan_Last[i] - colSpan_First[i] + 1;
		  width = (width + delta - 1) / delta;
		}
	      for (cRef = colSpan_First[i]; cRef <= colSpan_Last[i]; cRef++)
		if (colPercent[cRef] == 0 && colWidth[cRef] == 0 &&
		    (colMaxWidth[cRef] > minsize || span == 0))
		  colMinWidth[cRef] += width;
	    }
	  if (colSpan_MaxWidth[i] > max)
	    {
	      /* change width of included columns */
	      width = colSpan_MaxWidth[i] - max;
	      if (span > 0)
		width = (width + span - 1) / span;
	      else
		{
		  delta = colSpan_Last[i] - colSpan_First[i] + 1;
		  width = (width + delta - 1) / delta;
		}
	      for (cRef = colSpan_First[i]; cRef <= colSpan_Last[i]; cRef++)
		if (colPercent[cRef] == 0 && colWidth[cRef] == 0 &&
		    (colMaxWidth[cRef] > minsize || span == 0))
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

  TtaFreeMemory (colMinWidth);
  TtaFreeMemory (colMaxWidth);
  TtaFreeMemory (colVSpan);
  TtaFreeMemory (colSpan_MinWidth);
  TtaFreeMemory (colSpan_MaxWidth);
  TtaFreeMemory (colSpan_Width);
  TtaFreeMemory (colSpan_Percent);
  TtaFreeMemory (colSpan_First);
  TtaFreeMemory (colSpan_Last);
   /* Now check the table size */
  CheckTableSize (table, cNumber, colBox, colWidth, colPercent, frame, FALSE);
  TtaFreeMemory (colBox);
  TtaFreeMemory (colWidth);
  TtaFreeMemory (colPercent);
}

/*----------------------------------------------------------------------
   CheckColumnWidth

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
  boolean             found;

  /* look for the table */
  table = NULL;
  if (col != NULL && col->AbBox != NULL)
    /* get the table element */
    table = (PtrAbstractBox) col->AbBox->BxTable;
  else if (cell != NULL && cell->AbBox != NULL)
    {
      /* get row and table elements */
      row = SearchEnclosingType (cell, BoRow);
      if (row != NULL  && row->AbBox != NULL)
	table = (PtrAbstractBox) row->AbBox->BxTable;

      if (table != NULL && !table->AbNew && !table->AbDead)
	{
	  pEl = cell->AbElement;
	  pSS = pEl->ElStructSchema;
	  col = NULL;
	  span = 1;
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
 
  if (table != NULL && !table->AbNew && !table->AbDead && col != NULL)
    {
      /* the table exists, compute the column width */
      ComputeColWidth (col, table, frame);
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
      ComputeColWidth (col, pAb, frame);
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
static void      IsFirstColumn (PtrAbstractBox cell, PtrAbstractBox table, boolean *result)
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
  boolean             found;

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

	/* connexion des ressources */
	TteConnectAction (T_checktable, (Proc) UpdateTable);
	TteConnectAction (T_checkcolumn, (Proc) UpdateColumnWidth);
	TteConnectAction (T_resizetable, (Proc) UpdateTableWidth);
	TteConnectAction (T_cleartable, (Proc) ClearTable);
	TteConnectAction (T_firstcolumn, (Proc) IsFirstColumn);
     }
}

/* End Of Module Table2 */
