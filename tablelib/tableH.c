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
static ThotBool          Lock = FALSE;

#include "attributes_f.h"
#include "boxmoves_f.h"
#include "boxrelations_f.h"
#include "buildboxes_f.h"
#include "buildlines_f.h"
#include "createabsbox_f.h"
#include "exceptions_f.h"
#include "frame_f.h"
#include "memory_f.h"
#include "tree_f.h"
#define MAX_COLROW 50

#ifdef __STDC__
static ThotBool SetCellWidths (PtrAbstractBox cell, PtrAbstractBox table, int frame);
#else
static ThotBool SetCellWidths (/*PtrAbstractBox cell, PtrAbstractBox table, int frame*/);
#endif

/*----------------------------------------------------------------------
  DifferFormatting registers differed table formatting in the right
  order:
  The ancestor before its child.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void       DifferFormatting (PtrAbstractBox table, PtrAbstractBox cell, int frame)
#else  /* __STDC__ */
static void       DifferFormatting (table, cell, frame)
PtrAbstractBox    table;
PtrAbstractBox    cell;
int               frame;
#endif /* __STDC__ */
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
	      j = pLockRel->LockRFrame[i];
	      pAb = pLockRel->LockRTable[i];
	      pLockRel->LockRTable[i] = table;
	      table = pAb;
	      pLockRel->LockRFrame[i] = frame;
	      frame = j;
	      pAb = pLockRel->LockRCell[i];
	      pLockRel->LockRCell[i] = cell;
	      cell = pAb;
	    }
	  else
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
      pLockRel = TtaGetMemory (sizeof (LockRelations));
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
#ifdef __STDC__
static ThotBool   IsDifferredTable(PtrAbstractBox table, PtrAbstractBox cell)
#else  /* __STDC__ */
static ThotBool   IsDifferredTable (table, cell)
PtrAbstractBox    table;
PtrAbstractBox    cell;
#endif /* __STDC__ */
{
  PtrLockRelations    pLockRel;
  int                 i;

  if (table == NULL)
    return FALSE;

  /* Look for an empty entry */
  pLockRel = DifferedChecks;
  i = 0;
  while (pLockRel != NULL)
    {
      i = 0;
      while (i < MAX_RELAT_DIM)
	{
	  if (pLockRel->LockRTable[i] == table &&
	      (pLockRel->LockRCell[i] == NULL || pLockRel->LockRCell[i] == cell))
	    /* The table is already registered */
	    return TRUE;
	  else
	    i++;
	}
	/* next block */
	pLockRel = pLockRel->LockRNext;
    }
  return FALSE;
}

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

#ifdef IV
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
	  }
	else
	  pAttr = pAttr->AeNext;
    }
}
#endif /* IV */

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
	  }
	else if (pAttr->AeAttrNum == attrPercent &&
	    pAttr->AeAttrSSchema->SsCode == pSS->SsCode &&
	    (pAttr->AeAttrType == AtNumAttr || pAttr->AeAttrType == AtEnumAttr))
	  {
	    found = TRUE;
	    *percent = pAttr->AeAttrValue;
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
static void     CheckRowHeights (PtrAbstractBox table, int frame)
#else
static void     CheckRowHeights (table, frame)
PtrAbstractBox  table;
int             frame;
#endif
{
  PtrAttribute        pAttr;
  PtrSSchema          pSS = NULL;
  PtrDocument         pDoc;
  PtrTabSpan          pTabSpan;
  PtrAbstractBox      cell, row , firstRow, pAb;
  PtrAbstractBox      rowList[MAX_COLROW];
  PtrBox              box;
  PtrTabRelations     pTabRel;
  Document            doc;
  int                 i, j, k, org;
  int                 sum, height;
  int                 attrHeight = 0;
  int                 remainder;
  ThotBool             found, modified;

  if (table->AbBox->BxCycles != 0 || table->AbBox->BxSpans == NULL)
    /* the table formatting is currently in process */
    return;

  j = 0;
  /* manage spanned columns */
  doc = FrameTable[frame].FrDoc;
  modified = TtaIsDocumentModified (doc);
  pDoc = LoadedDocument[doc - 1];
  pSS = table->AbElement->ElStructSchema;
  attrHeight = GetAttrWithException (ExcNewHeight, pSS);
  /* first manage diferred enclosing rules */
  ComputeEnclosing (frame);

  pTabSpan = table->AbBox->BxSpans;
  while (pTabSpan != NULL)
    {
      for (i = 0; i < MAX_RELAT_DIM; i++)
	{
	  cell = pTabSpan->TaSpanCell[i];
	  if (cell != NULL && cell->AbBox != NULL && pTabSpan->TaSpanNumber[i] > 1)
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
		  for (k = 0; k <pTabSpan->TaSpanNumber [i] && k < MAX_COLROW; k++)
		    {
		      rowList[k] = row;
		      if (row != NULL && row->AbBox != NULL)
			{
			  /* add spacing */
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
		  /* update the span value if necessary */
		  if (k < pTabSpan->TaSpanNumber[i])
		    pTabSpan->TaSpanNumber[i] = k;
		  box = cell->AbBox;
		  org = box->BxYOrg;
		  height = box->BxHeight;

		  /* add space between the the cell and its parent row */
		  if (firstRow != NULL)
		    height += org - firstRow->AbBox->BxYOrg;

                 /* update rows' height if necessary */
	         height = height - sum;
		 if (height > 0)
		   {
		     height = height / pTabSpan->TaSpanNumber[i];
		     for (k = 0; k < pTabSpan->TaSpanNumber[i]; k++)
		       if (rowList[k] != NULL &&
			   rowList[k]->AbBox->BxH + height > 0)
			 {
			   /* create the attribute for this element */
			   GetAttribute (&pAttr);
			   pAttr->AeAttrSSchema = pSS;
			   pAttr->AeAttrNum = attrHeight;
			   pAttr->AeAttrType = AtNumAttr;
			   pAttr->AeAttrValue = rowList[k]->AbBox->BxH + height;
			   AttachAttrWithValue (rowList[k]->AbElement, pDoc, pAttr);
			   DeleteAttribute (NULL, pAttr);
			   /* update the row box */
			   ComputeUpdates (rowList[k], frame);
			 }
		     if (firstRow != NULL)
		       {
			 HeightPack (firstRow->AbEnclosing, firstRow->AbBox, frame);
			 HeightPack (table, firstRow->AbBox, frame);
		       }
		     /* Redisplay views */
		     if (ThotLocalActions[T_redisplay] != NULL)
		       (*ThotLocalActions[T_redisplay]) (pDoc);
		   }
		}
	    }
	}
      pTabSpan = pTabSpan->TaSpanNext;
    }
  if (!modified)
    TtaSetDocumentUnmodified (doc);
}


/*----------------------------------------------------------------------
  CheckTableWidths
  Test the coherence between 
  t.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void     CheckTableWidths (PtrAbstractBox table, int frame)
#else
static void     CheckTableWidths (table, frame)
PtrAbstractBox  table;
int             cNumber;
PtrAbstractBox *colBox;
int            *colWidth;
int            *colPercent;
int             frame;
ThotBool         force;
#endif
{
  PtrAbstractBox      pCell;
  PtrAbstractBox     *colBox;
  PtrBox              pBox, box;
  PtrTabRelations     pTabRel;
  int                *colWidth, *colPercent;
  int                 cNumber, cRef;
  int                 i, delta, px, n;
  int                 width, nPercent;
  int                 min, max, sum;
  int                 percent, sumPercent;
  int                 minOfPercent, maxOfPercent;
  int                 minOfWidth, maxOfWidth;
  int                 mbp, var;
  ThotBool            constraint, useMax;
  ThotBool            addIt, addExtra;

  /* Now check the table size */
  if (table->AbBox->BxCycles != 0)
    /* the table formatting is currently in process */
    return;

  /* get the number of columns */
  pBox = table->AbBox;
  pTabRel = pBox->BxColumns;
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

  mbp = pBox->BxLMargin + pBox->BxRMargin + pBox->BxLPadding + pBox->BxRPadding + pBox->BxLBorder + pBox->BxRBorder;
  pCell = GetParentCell (pBox);

  /* get the inside table width */
  constraint = GiveAttrWidth (table, &width, &percent);
  if (!constraint)
    /* limit given by available space */
    width = table->AbEnclosing->AbBox->BxW - mbp;
  else if (percent != 0)
    /* limit given by precent of available space */
    width = (table->AbEnclosing->AbBox->BxW * percent / 100) - mbp;

  if (constraint && width == 0)
    {
      /* limit given by available space */
      width = table->AbEnclosing->AbBox->BxW - mbp;
      constraint = FALSE;
    }
  else if (constraint && pCell && width > table->AbEnclosing->AbBox->BxW - mbp)
    {
      /* limit given by available space */
      width = table->AbEnclosing->AbBox->BxW - mbp;
      constraint = FALSE;
    }

  /* get columns information */
  pTabRel = pBox->BxColumns;
  cRef = 0;
  colBox = TtaGetMemory (sizeof (PtrAbstractBox) * cNumber);
  colWidth = TtaGetMemory (sizeof (int) * cNumber);
  colPercent = TtaGetMemory (sizeof (int) * cNumber);
  sumPercent = 0;
  sum = 0;
  minOfPercent = 0;
  maxOfPercent = 0;
  minOfWidth = 0;
  maxOfWidth = 0;
  min = 0;
  max = 0;
  mbp = 0;
  /* number of unconstrained columns */
  n = 0;
  /* number of constrained columns by percent */
  nPercent = 0;
  while (pTabRel != NULL)
    {
      for (i = 0; i < MAX_RELAT_DIM &&
	     pTabRel->TaRTable[i] != NULL &&
	     pTabRel->TaRTable[i]->AbBox != NULL;  i++)
	{
	  colBox[cRef] = pTabRel->TaRTable[i];
	  colWidth[cRef] = pTabRel->TaRTWidths[i];
	  colPercent[cRef] = pTabRel->TaRTPercents[i];
	  box = colBox[cRef]->AbBox;
	  /* take valid column percent attributes into account */
	  if (colPercent[cRef] != 0 &&
	      colPercent[cRef] * width / 100 < box->BxMinWidth)
	    colPercent[cRef] = 0;
	  min += box->BxMinWidth;
	  max += box->BxMaxWidth;
	  if (colPercent[cRef] != 0)
	    {
	      sumPercent += colPercent[cRef];
	      /* min and max replaced by a percent value */
	      minOfPercent += box->BxMinWidth;
	      maxOfPercent += box->BxMaxWidth;
	      nPercent++;
	    }
	  else if (colWidth[cRef] != 0)
	    {
	      sum += colWidth[cRef];
	      /* min and max replaced by a width value */
	      minOfWidth += box->BxMinWidth;
	      maxOfWidth += box->BxMaxWidth;
	    }
	  else
	    n++;
	  cRef++;
	}
      pTabRel = pTabRel->TaRNext;
    }
  /* get the extra width of the table */
  mbp =  pBox->BxMinWidth - min;
  min = min + mbp - minOfWidth - minOfPercent;
  max = max + mbp - maxOfWidth - maxOfPercent;
  if (sumPercent > 0)
    {
      /*
	get the percent value necessary for other columns if we choose:
	- the conrtained width
	- or the minimum width
      */
      if (width > 0)
	delta = (min + sum) * 100 / width;
      else
	delta = 0;
      if (sumPercent + delta > 100)
	{
	  /* we cannot apply the percent constraint: change the constraint */
	  i = sumPercent + delta - 100;
	  i = i / nPercent;
	  sumPercent = (100 - delta) * width / 100;
	  for (cRef = 0; cRef < cNumber; cRef++)
	    if (colPercent[cRef])
	      if (colPercent[cRef] <= i)
		{
		  colPercent[cRef] = 0;
		  min += box->BxMinWidth;
		  max += box->BxMaxWidth;
		}
	      else if (colPercent[cRef] > i)
		colPercent[cRef] -= i;
	}
      else
	sumPercent = sumPercent * width / 100;
    }
#ifdef TAB_DEBUG
printf ("CheckTableWidths (table=%s)\n", table->AbElement->ElLabel);
#endif

  /* now update real widths */
  pBox->BxCycles = 1;
  if (max + sum + sumPercent <= width && !constraint)
    {
      /* assign the maximum width, or the percent, or the width */
      width = max + sum + sumPercent;
      if (width - pBox->BxW)
	/* we will have to recheck scrollbars */
	AnyWidthUpdate = TRUE;
      ResizeWidth (pBox, pBox, NULL, width - pBox->BxW, 0, 0, 0, frame);
      for (cRef = 0; cRef < cNumber; cRef++)
	{
	  box = colBox[cRef]->AbBox;
	  if (colPercent[cRef] != 0)
	    delta = ((width - mbp) * colPercent[cRef] / 100);
	  else if (colWidth[cRef] != 0)
	    delta = colWidth[cRef];
	  else
	    delta = box->BxMaxWidth;
	  /* update the new inside width */
	  delta = delta - box->BxWidth;
	  ResizeWidth (box, box, NULL, delta, 0, 0, 0, frame);
#ifdef TAB_DEBUG
printf ("Width[%d]=%d\n", cRef, box->BxWidth);
#endif
	}
    }
  else if (min + sum + sumPercent >= width && pCell == NULL)
    {
      /* assign the minimum width, or the percent, or the width */
      width = min + sum + sumPercent;
      if (width - pBox->BxW)
	/* we will have to recheck scrollbars */
	AnyWidthUpdate = TRUE;
      ResizeWidth (pBox, pBox, NULL, width - pBox->BxW, 0, 0, 0, frame);
      for (cRef = 0; cRef < cNumber; cRef++)
	{
	  box = colBox[cRef]->AbBox;
	  if (colPercent[cRef] != 0)
	    delta = ((width - mbp) * colPercent[cRef] / 100);
	  else if (colWidth[cRef] != 0)
	    delta = colWidth[cRef];
	  else
	    delta = box->BxMinWidth;
	  /* update the new inside width */
	  delta = delta - box->BxWidth;
	  ResizeWidth (box, box, NULL, delta, 0, 0, 0, frame);
#ifdef TAB_DEBUG
printf ("Width[%d]=%d\n", cRef, box->BxWidth);
#endif
	}
    }
  else
    {
      /* assign the specific width to the table */
      if (width - pBox->BxW)
	/* we will have to recheck scrollbars */
	AnyWidthUpdate = TRUE;
      ResizeWidth (pBox, pBox, NULL, width - pBox->BxW, 0, 0, 0, frame);
      /* get the space available for stretchable columns */      
      delta = width - sum - sumPercent;
      /* get the sapce needed for displaying with the maximum or the minimum widths */
      if (n == 0)
	{
	  /* no stretchable columns */
	  n = cNumber;
	  addExtra = TRUE;
	}
      else
	addExtra = FALSE;
      if (max > 0 && max < delta)
	{
	  /* extend the maximum of each stretchable column */
	  useMax = TRUE;
	  delta = delta - max;
	}
      else
	{
	  useMax = FALSE;
	  delta = delta - min;
	  if (delta)
	    {
	      /* don't increase the min more than the current max */
	      var = delta / n;
	      for (cRef = 0; cRef < cNumber; cRef++)
		{
		  box = colBox[cRef]->AbBox;
		  if (colPercent[cRef] == 0 && colWidth[cRef] == 0 &&
		      box->BxMinWidth + var > box->BxMaxWidth)
		    {
		      /* use the max instead of the min + delta */
		      delta = delta + box->BxMinWidth - box->BxMaxWidth;
			n--;
		    }
		}
	    }
	}
      if (n)
	{
	  i = delta / n;
	  var = delta - (i * n);
	  delta = i;
	}
      else
	var = 0;

      /* assign the width to each column */
      for (cRef = 0; cRef < cNumber; cRef++)
	{
	  box = colBox[cRef]->AbBox;
	  addIt = TRUE;
	  if (colPercent[cRef])
	    i = ((width - mbp) * colPercent[cRef] / 100);
	  else if (colWidth[cRef])
	    i = colWidth[cRef];
	  else if (useMax)
	    i = box->BxMaxWidth;
	  else if (box->BxMinWidth + delta > box->BxMaxWidth)
	    {
	      addIt = FALSE;
	      i = box->BxMaxWidth;
	    }
	  else
	    i = box->BxMinWidth;
	   if ((addExtra && (colPercent[cRef] || colWidth[cRef])) ||
	       (addIt && colPercent[cRef] == 0 && colWidth[cRef] == 0))
	     {
	      /* add extra pixels */
	      if (var > 0)
		{
		  px = 1;
		  var--;
		}
	      else
		px = 0;
	      i = i + delta + px;
	    }
	  /* update the new inside width */
	  i = i - box->BxWidth;
	  ResizeWidth (box, box, NULL, i, 0, 0, 0, frame);
#ifdef TAB_DEBUG
printf ("Width[%d]=%d\n", cRef, box->BxWidth);
#endif
	}
    }

  table->AbBox->BxCycles = 0;
#ifdef TAB_DEBUG
printf(">>>>>>>>>>>>>>>>>%d\n", table->AbBox->BxWidth);
#endif
  if (table->AbBox->BxRows != NULL && table->AbBox->BxRows->TaRTable[0]->AbEnclosing != NULL)
    RecordEnclosing (table->AbBox->BxRows->TaRTable[0]->AbEnclosing->AbBox, FALSE);
  else
    RecordEnclosing (table->AbBox, FALSE);

  TtaFreeMemory (colBox);
  TtaFreeMemory (colWidth);
  TtaFreeMemory (colPercent);
}


/*----------------------------------------------------------------------
  ChangeTableWidth

  The table width changes, we need to propagate the change.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void     ChangeTableWidth (PtrAbstractBox table, int frame)
#else
static void     ChangeTableWidth (table, frame)
PtrAbstractBox  table;
int             frame;
#endif
{
  if (Lock)
    /* the table formatting is locked */
    DifferFormatting (table, NULL, frame);
  else if (IsDifferredTable (table, NULL))
    /* the table will be managed later */
    return;
  else
    {
      /* Now check the table size */
      CheckTableWidths (table, frame);
      CheckRowHeights (table, frame);
    }
}

/*----------------------------------------------------------------------
  GiveCellWidths returns the minimum width, the maximum width, the
  constrained width and the percent width of a specific cell.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void     GiveCellWidths (PtrAbstractBox cell, int *min, int *max, int *width, int *percent)
#else
static void     GiveCellWidths (cell, min, max)
PtrAbstractBox  cell;
int            *min;
int            *max;
int            *width;
int            *percent;
#endif
{
  PtrAbstractBox      pAb;
  PtrBox              box;
  PtrSSchema          pSS;
  int                 mbp, delta;
  ThotBool            skip;

  box = cell->AbBox;
  /* take into account the left margin, border and padding */
  mbp = box->BxLMargin + box->BxLBorder + box->BxLPadding;

  /* process elements in this cell */
  pSS = cell->AbElement->ElStructSchema;
  *min = 1;
  *max = 1;
  skip = FALSE;
  pAb = cell;
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
	      delta = pAb->AbBox->BxXOrg - box->BxXOrg - mbp;
	      if (delta < 0)
		delta = 0;
	    }
	  else
	    delta = 0;
	  if (pAb->AbBox->BxType == BoBlock ||
	      pAb->AbBox->BxType == BoTable)
	    {
	      if (*min < pAb->AbBox->BxMinWidth + delta)
		*min = pAb->AbBox->BxMinWidth + delta;
	      if (*max < pAb->AbBox->BxMaxWidth + delta)
		*max = pAb->AbBox->BxMaxWidth + delta;
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
		  if (*min < pAb->AbBox->BxWidth + delta)
		    *min = pAb->AbBox->BxWidth + delta;
		  if (*max < pAb->AbBox->BxWidth + delta)
		    *max = pAb->AbBox->BxWidth + delta;
		}
	      skip = TRUE;
	    }
	}
    }
  /* take into account the right margin, border, padding */
  mbp += box->BxRMargin + box->BxRBorder + box->BxRPadding;
  *min = *min + mbp;
  *max = *max + mbp;
  GiveAttrWidth (cell, width, percent);
  if (*width)
    *width = *width + mbp;
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
#ifdef __STDC__
static ThotBool SetTableWidths (PtrAbstractBox table, int frame)
#else
static ThtoBool SetTableWidths (table, frame)
PtrAbstractBox  table;
int             frame;
#endif
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
  ThotBool            skip, change, reformat;
  ThotBool            foundH, foundV;

  pDoc = LoadedDocument[FrameTable[frame].FrDoc - 1];
  pSS = table->AbElement->ElStructSchema;
  /* how many columns */
  pBox = table->AbBox;
  pTabRel = pBox->BxColumns;
  cNumber = 0;
  change = FALSE;
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

  /* remove the list of vertical spanned cells */
  pTabSpan = pBox->BxSpans;
  while (pTabSpan != NULL)
    {
      pBox->BxSpans = pTabSpan->TaSpanNext;
      TtaFreeMemory (pTabSpan);
      pTabSpan = pBox->BxSpans;
    }

  /* register widths of each columns */
  pTabRel = pBox->BxColumns;
  cRef = 0;
  colBox = TtaGetMemory (sizeof (PtrAbstractBox) * cNumber);
  colWidth = TtaGetMemory (sizeof (int) * cNumber);
  colPercent = TtaGetMemory (sizeof (int) * cNumber);
  colVSpan = TtaGetMemory (sizeof (int) * cNumber);
  while (pTabRel != NULL)
    {
      for (i = 0; i < MAX_RELAT_DIM &&
	     pTabRel->TaRTable[i] != NULL &&
	     pTabRel->TaRTable[i]->AbBox != NULL;  i++)
	{
	  colBox[cRef] = pTabRel->TaRTable[i];
	  colBox[cRef]->AbBox->BxMinWidth = 0;
	  colBox[cRef]->AbBox->BxMaxWidth = 0;
	  colWidth[cRef] = 0;
	  colPercent[cRef] = 0;
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
				  if (pAttr->AeAttrType == AtEnumAttr || pAttr->AeAttrType == AtNumAttr)
				    {
				      /* rowspan on this cell */
				      colVSpan[cRef] = pAttr->AeAttrValue - 1;
				      if (colVSpan[cRef] > 0 && rspanNumber < MAX_COLROW)
					{
					  /* register current cell and span value */
					  if (pTabSpan == NULL)
					    {
					      rspanNumber = 0;
					      pTabSpan = TtaGetMemory (sizeof (TabSpan));
					      memset (pTabSpan, 0, sizeof (TabSpan));
					      pBox->BxSpans = pTabSpan;
					    }
					  else if (rspanNumber< MAX_RELAT_DIM)
					    {
					      rspanNumber = 0;
					      pTabSpan->TaSpanNext = TtaGetMemory (sizeof (TabSpan));
					      pTabSpan = pTabSpan->TaSpanNext;
					      memset (pTabSpan, 0, sizeof (TabSpan));
					    }
					  pTabSpan->TaSpanNumber[rspanNumber] = pAttr->AeAttrValue;
					  pTabSpan->TaSpanCell[rspanNumber++] = pAb;
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
		      box = cell->AbBox;
		      /* get the min and max and constrained widths */
		      GiveCellWidths (cell, &min, &max, &cellWidth, &percent);
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
			  delta = colBox[cRef]->AbBox->BxWidth - box->BxWidth;
			  if (delta < 0)
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
	  realMin += colBox[cRef]->AbBox->BxMinWidth;
	  realMax += colBox[cRef]->AbBox->BxMaxWidth;
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
	    width = (width + span - 1) / span;
	  else
	    {
	      delta = colSpan_Last[i] - colSpan_First[i] + 1;
	      width = (width + delta - 1) / delta;
	    }
	  for (cRef = colSpan_First[i]; cRef <= colSpan_Last[i]; cRef++)
	    if ((colPercent[cRef] == 0 && colWidth[cRef] == 0) || span == 0)
	      colBox[cRef]->AbBox->BxMaxWidth += width;
	}
    }

  GiveAttrWidth (table, &width, &percent);
  /* now update column boxes */
  pTabRel = pBox->BxColumns;
  cRef = 0;
  min = 0;
  max = 0;
  delta = 0;
  /* no previous column */
  prevBox = NULL;
  while (pTabRel != NULL)
    {
      for (i = 0; i < MAX_RELAT_DIM && pTabRel->TaRTable[i] != NULL;  i++)
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
	      delta += box1->BxRMargin + box1->BxRPadding + box1->BxRBorder;
	    }
	  else if (box->BxXOrg - prevBox->BxXOrg - prevBox->BxWidth > 0)
	    delta += box->BxXOrg - prevBox->BxXOrg - prevBox->BxWidth;
	  prevBox = box;
	  cRef++;
	}
      pTabRel = pTabRel->TaRNext;
    }

  /* get constraints on the table itself */
  mbp = pBox->BxLMargin + pBox->BxRMargin + pBox->BxLPadding + pBox->BxRPadding + pBox->BxLBorder + pBox->BxRBorder;
  min = min + delta + mbp;
  max = max + delta + mbp;
  if (width)
    {
      /* the table width is constrained */
      width += mbp;
      max = width;
    }
  /* do we need to reformat the table */
  change = (pBox->BxRuleWidth != width ||  pBox->BxMinWidth != min ||
            pBox->BxMaxWidth != max);
  reformat = ((pBox->BxRuleWidth != width &&
	       pBox->BxWidth == pBox->BxRuleWidth) ||
	    (pBox->BxMinWidth != min &&
	     (pBox->BxWidth < min || pBox->BxWidth == pBox->BxMinWidth)) ||
	    (pBox->BxMaxWidth != max && pBox->BxWidth  == pBox->BxMaxWidth));
  pBox->BxRuleWidth = width;
  pBox->BxMinWidth = min;
  pBox->BxMaxWidth = max;
  TtaFreeMemory (colVSpan);
  TtaFreeMemory (colBox);
  TtaFreeMemory (colWidth);
  TtaFreeMemory (colPercent);
  if (change)
    {
      /* trasmit the min and max widths of the enclosing paragraph */
      pAb = SearchEnclosingType (table, BoBlock);
      if (pAb && pAb->AbBox)
	{
	  pBox = pAb->AbBox;
	  /* adding the block margins, border, paddins */
	  mbp = pBox->BxLMargin + pBox->BxRMargin + pBox->BxLPadding + pBox->BxRPadding + pBox->BxLBorder + pBox->BxRBorder;
	  min += mbp;
	  max += mbp;
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
	      row = SearchEnclosingType (cell, BoRow);
	      if (row  && row->AbBox)
		table = (PtrAbstractBox) row->AbBox->BxTable;
	      if (table && table->AbBox && !IsDifferredTable (table, cell))
		SetCellWidths (cell, table, frame);
	    }
	}
    }
  return (reformat);
}

/*----------------------------------------------------------------------
  SetCellWidths checks if any cell width is modified.
  If TRUE updates table widths.
  Return TRUE if any table width is modified
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool SetCellWidths (PtrAbstractBox cell, PtrAbstractBox table, int frame)
#else
static ThtoBool SetCellWidths (cell, table, frame)
PtrAbstractBox  cell;
PtrAbstractBox  table;
int             frame;
#endif
{
  PtrBox              box;
  int                 min, max;
  int                 width, percent;
  ThotBool            reformat;

  box = cell->AbBox;
  GiveCellWidths (cell, &min, &max, &width, &percent);
  if (width && width < min)
    /* a constrained width it must be greater than the minimum */
    width = min;
  if (width)
    /* when there is a constrained width the maximum is forced */
    max = width;
  reformat = (box->BxRuleWidth != width ||
	    (box->BxMinWidth != min &&
	     (box->BxWidth < min || box->BxWidth == box->BxMinWidth)) ||
	    (box->BxMaxWidth != max && box->BxWidth  == box->BxMaxWidth));
  box->BxMinWidth = min;
  box->BxMaxWidth = max;
  box->BxRuleWidth = width;

  if (reformat && table)
    /* something changed in the cell, check any table change */
    SetTableWidths (table, frame);
  return (reformat);
}


/*----------------------------------------------------------------------
  UpdateCellHeight

  The cell height changes, we need to propagate the change.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void      UpdateCellHeight (PtrAbstractBox cell, int frame)
#else
static void      UpdateCellHeight (cell, frame)
PtrAbstractBox   cell;
int              frame;
#endif
{
  PtrAbstractBox      table;
  PtrAbstractBox      row;

  /* get row and table elements */
  row = SearchEnclosingType (cell, BoRow);
  if (row  && row->AbBox)
    {
      table = (PtrAbstractBox) row->AbBox->BxTable;
      if (table)
	CheckRowHeights (table, frame);
    }
}


/*----------------------------------------------------------------------
  UpdateColumnWidth

  The column width changes, we need to propagate the change.
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

  /* look for the table */
  table = NULL;
  if (col && col->AbBox)
    /* get the table element */
    table = (PtrAbstractBox) col->AbBox->BxTable;
  else if (cell && cell->AbBox)
    {
      /* get row and table elements */
      row = SearchEnclosingType (cell, BoRow);
      if (row  && row->AbBox)
	table = (PtrAbstractBox) row->AbBox->BxTable;
    }
  if (table && !table->AbNew && !table->AbDead &&
      table->AbBox && table->AbBox->BxCycles == 0)
    {
      if (Lock)
	/* the table formatting is locked */
	DifferFormatting (table, cell, frame);
      else if (IsDifferredTable (table, NULL))
	/* the table will be managed later */
	return;
      else
	{
	  if (cell && cell->AbBox)
	    {
	      /* there a change within a specific cell */
	      if (SetCellWidths (cell, table, frame))
		{
		  /* Now check the table size */
		  CheckTableWidths (table, frame);
		  CheckRowHeights (table, frame);
		}
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
      if (col && col->AbBox)
	{
	  pAb = (PtrAbstractBox) col->AbBox->BxTable;
	  if (pAb == NULL)
	    {
	      pAb = SearchEnclosingType (col, BoTable);
	      /* during table building each column needs a minimum width */
	      if (col->AbBox->BxWidth < 20)
		ResizeWidth (col->AbBox, col->AbBox, NULL, 20 - col->AbBox->BxWidth, 0, 0, 0, frame);
	    }
	}
      else if (row && row->AbBox)
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

  if (pAb && pAb->AbBox && !pAb->AbNew && !pAb->AbDead)
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
      else if (IsDifferredTable (pAb, NULL))
	/* the table will be managed later */
	return;
      else if (table)
	{
	  /* compute widths of each column within the table */
	  if (SetTableWidths (table, frame))
	    {
	      /* Now check the table size */
	      CheckTableWidths (table, frame);
	      CheckRowHeights (table, frame);
	    }
	}
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
  PtrTabSpan          pTabSpan;
  PtrBox              pBox;

  /* free specific blocks */
  pBox = table->AbBox;
  FreeTaRBlock (pBox->BxColumns);
  pBox->BxColumns = NULL;
  FreeTaRBlock (pBox->BxRows);
  pBox->BxRows = NULL;
  /* remove the list of vertical spanned cells */
  pTabSpan = pBox->BxSpans;
  while (pTabSpan != NULL)
    {
      pBox->BxSpans = pTabSpan->TaSpanNext;
      TtaFreeMemory (pTabSpan);
      pTabSpan = pBox->BxSpans;
    }
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
  TtaLockTableFormatting suspends all tables formatting
  ----------------------------------------------------------------------*/
void    TtaLockTableFormatting ()
{
  Lock = TRUE;
}

/*----------------------------------------------------------------------
  UnlockTableFormatting reformats all locked tables
  ----------------------------------------------------------------------*/
static void    UnlockTableFormatting ()
{
  PtrLockRelations    pLockRel;
  PtrAbstractBox      table, cell;
  Propagation         savpropage;
  int                 i;

  if (Lock)
    {
      Lock = FALSE;
      if (DifferedChecks == NULL)
	/* nothing to do */
	return;

      savpropage = Propagate;
      Propagate = ToAll;
      /*
	First, compute the minimum width, the maximum width and
	the contrained width of each column and table starting
	form the most embedded to the enclosing table
      */
      pLockRel = DifferedChecks;
      while (pLockRel->LockRNext != NULL)
	pLockRel = pLockRel->LockRNext;
      while (pLockRel != NULL)
	{
	  /* Manage all locked tables */
	  i = MAX_RELAT_DIM - 1;
	  while (i >= 0)
	    {
	      table = pLockRel->LockRTable[i];
	      if (table && table->AbElement)
		{
		  cell = pLockRel->LockRCell[i];
		  if (cell && cell->AbBox)
		    {
		      /* there is a change within a specific cell */
		      if (!SetCellWidths (cell, table, pLockRel->LockRFrame[i]))
			/* nothing to do more on this table */
			pLockRel->LockRTable[i] = NULL;
		    }
		  else
		    {
		      /* there is a change within a specific cell */
		      SetTableWidths (table, pLockRel->LockRFrame[i]);
		    }
		}
	      /* next entry */
	      i--;
	    }
	  /* next block */
	  pLockRel = pLockRel->LockRPrev;
	}

      /*
	Second, reformat all tables starting
	form the enclosing table to the most embedded
      */
      pLockRel = DifferedChecks;
      while (pLockRel != NULL)
	{
	  /* Manage all locked tables */
	  i = 0;
	  while (i < MAX_RELAT_DIM)
	    {
	      table = pLockRel->LockRTable[i];
	      if (table && table->AbElement)
		{
		  /*pLockRel->LockRTable[i] = NULL;*/
		  CheckTableWidths (table, pLockRel->LockRFrame[i]);
		  /* need to propagate to enclosing boxes */
		  ComputeEnclosing (pLockRel->LockRFrame[i]);
		}
	      /* next entry */
	      i++;
	    }
	  /* next block */
	  pLockRel = pLockRel->LockRNext;
	}

      /*
	Then, check all table heighs form the most embedded to the enclosing table
      */
      pLockRel = DifferedChecks;
      while (pLockRel->LockRNext != NULL)
	pLockRel = pLockRel->LockRNext;
      while (pLockRel != NULL)
	{
	  /* Manage all locked tables */
	  i = MAX_RELAT_DIM - 1;
	  while (i >= 0)
	    {
	      table = pLockRel->LockRTable[i];
	      if (table && table->AbElement)
		{
		  CheckRowHeights (table, pLockRel->LockRFrame[i]);
		  /* need to propagate to enclosing boxes */
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
      while (DifferedChecks != NULL)
	{
	  pLockRel = DifferedChecks;
	  DifferedChecks = DifferedChecks->LockRNext;
	  TtaFreeMemory (pLockRel);
	}
    }
}

/*----------------------------------------------------------------------
  TtaUnlockTableFormatting reformats all locked tables
  Redisplay the selection and update scrolling bars
  ----------------------------------------------------------------------*/
void    TtaUnlockTableFormatting ()
{
  UnlockTableFormatting ();
}


/*----------------------------------------------------------------------
  TtaGiveTableFormattingLock gives the status of the table formatting lock.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void      TtaGiveTableFormattingLock (ThotBool *lock)
#else  /* __STDC__ */
void      TtaGiveTableFormattingLock (lock)
ThotBool *lock;
#endif /* __STDC__ */
{
  /* check if we're not processing a TtaUnlockTableFormatting */
  if (DifferedChecks != NULL)
    *lock = TRUE;
  else
    *lock = Lock;
}


/*----------------------------------------------------------------------
   TableHLoadResources : connect resources for managing HTML tables
  ----------------------------------------------------------------------*/
void                TableHLoadResources ()
{

   if (ThotLocalActions[T_checktable] == NULL)
     {
	/* connecting resources */
	TteConnectAction (T_lock, (Proc) TtaLockTableFormatting);
	TteConnectAction (T_unlock, (Proc) UnlockTableFormatting);
	TteConnectAction (T_islock, (Proc) TtaGiveTableFormattingLock);
	TteConnectAction (T_checktable, (Proc) UpdateTable);
	TteConnectAction (T_checkcolumn, (Proc) UpdateColumnWidth);
	TteConnectAction (T_checktableheight, (Proc) UpdateCellHeight);
	TteConnectAction (T_resizetable, (Proc) ChangeTableWidth);
	TteConnectAction (T_cleartable, (Proc) ClearTable);
	TteConnectAction (T_firstcolumn, (Proc) IsFirstColumn);
     }
}

/* End Of Module Table2 */
