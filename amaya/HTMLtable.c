/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2003
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *
 * Authors: V. Quint
 *          I. Vatton - new version of table management
 */

/* Amaya includes  */
#define THOT_EXPORT extern
#include "amaya.h"
#include "fetchXMLname_f.h"
#include "undo.h"
#include "MathML.h"
#include "HTMLedit_f.h"
#include "HTMLtable_f.h"
#include "MathMLbuilder_f.h"
#include "Mathedit_f.h"

static Element      CurrentRow = NULL;
static Element      CurrentDeletedRow = NULL;
static Element      CurrentPastedRow = NULL;
static Element      CurrentColumn = NULL;
static Element      CurrentCell;
static int          CurrentSpan;
static int          MaxRowSpan;
static int          PreviousColSpan;
static int          PreviousRowSpan;
static ThotBool     CheckTableAfterCellUpdate = TRUE;
static ThotBool     NewTable = FALSE;

/*----------------------------------------------------------------------
   GetCellFromColumnHead

   returns the cell that corresponds to the Column_head element colhead
   in a given row.
  ----------------------------------------------------------------------*/
static Element GetCellFromColumnHead (Element row, Element colhead,
				      ThotBool inMath)
{
   Element             cell, currentcolhead;
   ThotBool            found;
   ElementType         elType;
   AttributeType       attrType;
   Attribute           attr;
   char                name[50];
   Document            refdoc;

   cell = TtaGetFirstChild (row);
   elType = TtaGetElementType (row);
   attrType.AttrSSchema = elType.ElSSchema;
   if (inMath)
     attrType.AttrTypeNum = MathML_ATTR_MRef_column;
   else
     attrType.AttrTypeNum = HTML_ATTR_Ref_column;

   found = FALSE;
   while (cell != NULL && !found)
     {
	elType = TtaGetElementType (cell);
	if ((inMath && elType.ElTypeNum == MathML_EL_MTD) ||
	    (!inMath && (elType.ElTypeNum == HTML_EL_Data_cell ||
			 elType.ElTypeNum == HTML_EL_Heading_cell)))
	  {
	     attr = TtaGetAttribute (cell, attrType);
	     if (attr != NULL)
	       {
		  TtaGiveReferenceAttributeValue (attr, &currentcolhead, name,
						  &refdoc);
		  if (currentcolhead == colhead)
		     found = TRUE;
	       }
	  }
	if (!found)
	   TtaNextSibling (&cell);
     }
   return cell;
}

/*----------------------------------------------------------------------
   CloseCellForNewColumn

   returns the cell that corresponds to the Column_head element colhead
   or a previous or next colhead in a given row.
   - When the parameter before is TRUE, a previous cell is returned.
     The parameter spanned returns TRUE if the previous cell was extended
     and the parameter rowspan returns its rowspan value.
   - When the parameter before is FALSE, the next cell is returned.
  ----------------------------------------------------------------------*/
static Element CloseCellForNewColumn (Element row, Element colhead,
				      Document doc, ThotBool before,
				      ThotBool inMath, ThotBool *spanned,
				      int *rowspan)
{
  Element             col, child;
  ElementType         elType;
  Attribute           attr;
  AttributeType       attrType;
  int                 colspan, pos;

  *spanned = FALSE;
  *rowspan = 1;
  col = colhead;
  elType = TtaGetElementType (col);
  child = GetCellFromColumnHead (row, colhead, inMath);
  pos = 0;
  while (!child && col)
    {
      /* no cell related to this column in this row */
      if (before)
	col = TtaSearchTypedElement (elType, SearchBackward, col);
      else
	col = TtaSearchTypedElement (elType, SearchForward, col);
      child = GetCellFromColumnHead (row, col, inMath);
      pos++;
    }
  if (child && before)
    {
      /* get the colspan value of the element */
      attrType.AttrSSchema = elType.ElSSchema;
      if (inMath)
	attrType.AttrTypeNum = MathML_ATTR_columnspan;
      else
	attrType.AttrTypeNum = HTML_ATTR_colspan_;
      attr = TtaGetAttribute (child, attrType);
      if (attr)
	{
	  colspan = TtaGetAttributeValue (attr);
	  if (colspan - pos > 1)
	    {
	      colspan++;
	      TtaRegisterAttributeReplace (attr, child, doc);
	      TtaSetAttributeValue (attr, colspan, child, doc);
	      *spanned = TRUE;
	      /* check its rowspan attribute */
	      if (inMath)
		attrType.AttrTypeNum = MathML_ATTR_rowspan_;
	      else
		attrType.AttrTypeNum = HTML_ATTR_rowspan_;
	      attr = TtaGetAttribute (child, attrType);
	      if (attr)
		/* this cell has an attribute rowspan */
		*rowspan = TtaGetAttributeValue (attr);
	      if (*rowspan == 0)
		*rowspan = 1;
	    }
	}
    }
  return (child);
}


/*----------------------------------------------------------------------
   RelateCellWithColumnHead relates a cell with a Column_head.
  ----------------------------------------------------------------------*/
static void RelateCellWithColumnHead (Element cell, Element colhead,
				      Document doc, ThotBool inMath)
{
  ElementType         elType;
  AttributeType       attrType;
  Attribute           attr;
  ThotBool            newAttr;

  if (cell == NULL || colhead == NULL)
    return;
  elType = TtaGetElementType (cell);
  attrType.AttrSSchema = elType.ElSSchema;
  if (inMath)
    attrType.AttrTypeNum = MathML_ATTR_MRef_column;
  else
    attrType.AttrTypeNum = HTML_ATTR_Ref_column;
  attr = TtaGetAttribute (cell, attrType);
  if (attr == NULL)
    {
      newAttr = TRUE;
      attr = TtaNewAttribute (attrType);
      if (attr)
	TtaAttachAttribute (cell, attr, doc);
    }
  else
    newAttr = FALSE;

  if (attr)
    {
      if (!newAttr)
	TtaRegisterAttributeReplace (attr, cell, doc);
      TtaSetAttributeReference (attr, cell, doc, colhead, doc);
      if (newAttr)
	TtaRegisterAttributeCreate (attr, cell, doc);
    }
}

/*----------------------------------------------------------------------
   AddEmptyCellInRow add an empty cell element in the given row and relates
   it to the given Column_head colhead.
   The parameter sibling gives the cell which will precede or follow the new
   cell according to the before value.
   Return the created empty cell.
  ----------------------------------------------------------------------*/
static Element AddEmptyCellInRow (Element row, Element colhead,
				  Element sibling, ThotBool before,
				  Document doc, ThotBool inMath,
				  ThotBool placeholder)
{
  Element             lastcell, constr;
  ElementType         elType;
  Attribute           attr;
  AttributeType       attrType;

  lastcell = NULL;
  if (row == NULL)
    return (NULL);

  elType = TtaGetElementType (row);
  if (inMath)
    elType.ElTypeNum = MathML_EL_MTD;
  else
    elType.ElTypeNum = HTML_EL_Data_cell;
  lastcell = TtaNewTree (doc, elType, "");
  if (lastcell)
    {
      if (sibling)
	TtaInsertSibling (lastcell, sibling, before, doc);
      else
	TtaInsertFirstChild (&lastcell, row, doc);
      RelateCellWithColumnHead (lastcell, colhead, doc, inMath);
      if (inMath && placeholder)
	/* put attribute IntPlaceholder on the empty Construct that has
	   been created in the new cell */
	{
	  elType.ElTypeNum = MathML_EL_Construct;
	  constr = TtaSearchTypedElement (elType, SearchInTree, lastcell);
	  if (constr)
	    {
	      attrType.AttrSSchema = elType.ElSSchema;
	      attrType.AttrTypeNum = MathML_ATTR_IntPlaceholder;
	      attr = TtaNewAttribute (attrType);
	      if (attr)
		{
		  TtaAttachAttribute (constr, attr, doc);
		  TtaSetAttributeValue (attr,
			   MathML_ATTR_IntPlaceholder_VAL_yes_, constr, doc);
		}
	    }
	}
      TtaRegisterElementCreate (lastcell, doc);
    }
  return (lastcell);
}

/*----------------------------------------------------------------------
  NewColumnHead creates a new Column_head and returns it.   
  If generateEmptyCells == TRUE, create an additional empty cell in all rows,
  except the row indicated.
  If last == TRUE when lastcolhead is the last current column.
  The parameter before indicates if the lastcolhead precedes or follows
  the new created Column_head. It should be FALSE when last is TRUE.
  ----------------------------------------------------------------------*/
Element NewColumnHead (Element lastcolhead, ThotBool before,
		       ThotBool last, Element row, Document doc,
		       ThotBool inMath, ThotBool generateEmptyCells)
{
  Element             colhead, currentrow;
  Element             group, groupdone;
  Element             prevCol, nextCol;
  Element             table, child;
  ElementType         elType;
  int                 rowspan;
  ThotBool            select, backward, span;

  if (lastcolhead == NULL)
    return NULL;
  select = (row == NULL);
  span = FALSE;
  elType = TtaGetElementType (lastcolhead);
  colhead = TtaNewTree (doc, elType, "");
  if (colhead != NULL)
    {
      if (row)
	{
	  /* process the row group exept the given row */
	  currentrow = row;
	  TtaPreviousSibling (&currentrow);
	  if (!last && currentrow == NULL)
	    {
	      /* when last is TRUE, only cells of previous rows should be created  */
	      currentrow = row;
	      TtaNextSibling (&currentrow);
	      backward = FALSE;
	    }
	  else
	    backward = TRUE;
	}
      else
	{
	  elType = TtaGetElementType (lastcolhead);
	  /* get the first row */
	  if (inMath)
	    elType.ElTypeNum = MathML_EL_TableRow;
	  else
	    elType.ElTypeNum = HTML_EL_Table_row;
	  currentrow = TtaSearchTypedElement (elType, SearchForward, lastcolhead);
	  backward = FALSE;
	}
      prevCol = nextCol = lastcolhead;
      if (before)
	TtaPreviousSibling (&prevCol);
      else
	TtaNextSibling (&nextCol);	

      TtaInsertSibling (colhead, lastcolhead, before, doc);
      TtaRegisterElementCreate (colhead, doc);
      if (generateEmptyCells)
	/* add empty cells to all other rows */
	{
	  while (currentrow)
	    {
	      rowspan = 1;
	      /* get the sibling cell */
	      if (last)
		child = TtaGetLastChild (currentrow);
	      else
		/* look for the previous cell */
		child = CloseCellForNewColumn (currentrow, prevCol, doc,
					       TRUE, inMath, &span, &rowspan);
	      if (child)
		{
		  if (!span)
		    {
		      if (select && row == NULL)
			/* first row where a cell is created */
			row = currentrow;
		      /* add a new cell after */
		      AddEmptyCellInRow (currentrow, colhead, child, FALSE, doc,
					 inMath, FALSE);
		    }
		}
	      else
		{
		  /* look for the next cell */
		  child = CloseCellForNewColumn (currentrow, nextCol, doc,
						 FALSE, inMath, &span, &rowspan);
		  if (select && row == NULL)
		    /* first row where a cell is created */
		    row = currentrow;
		  /* add a cell before */
		  AddEmptyCellInRow (currentrow, colhead, child, TRUE, doc,
				     inMath, FALSE);
		}
	      while (rowspan >= 1 && currentrow)
		{
		  if (backward)
		    {
		      TtaPreviousSibling (&currentrow);
		      if (currentrow == NULL && !last)
			{
			  /* we have to manage following rows too */
			  currentrow = row;
			  TtaNextSibling (&currentrow);
			  backward = FALSE;
			  rowspan = 1;
			}
		    }
		  else
		    TtaNextSibling (&currentrow);
		  rowspan--;
		}
	    }

	  if (!inMath)
	    {
	      groupdone = TtaGetParent (row);	/* done with this group */
	      /* process the other row groups */
	      if (inMath)
		elType.ElTypeNum = MathML_EL_MTABLE;
	      else
		elType.ElTypeNum = HTML_EL_Table;
	      table = TtaGetTypedAncestor (groupdone, elType);
	      /* visit all children of the Table element */
	      child = TtaGetFirstChild (table);
	      while (child != NULL)
		{
		  elType = TtaGetElementType (child);
		  if ((!inMath && (elType.ElTypeNum == HTML_EL_thead ||
				   elType.ElTypeNum == HTML_EL_tfoot)) ||
		      (inMath && elType.ElTypeNum == MathML_EL_MTable_body))
		    {
		      /* this child is a thead or tfoot element */
		      group = child;
		      if (group != groupdone)
			{
			  currentrow = TtaGetFirstChild (group);
			  while (currentrow)
			    {
			      rowspan = 1;
				/* get the sibling cell */
			      if (last)
				child = TtaGetLastChild (currentrow);
			      else
				/* look for the previous cell */
				child = CloseCellForNewColumn (currentrow,
							       prevCol, doc,
							       TRUE, inMath,
							       &span, &rowspan);
			      if (child)
				{
				  if (!span)
				    /* add a new cell after */
				    AddEmptyCellInRow (currentrow, colhead, child,
						       FALSE, doc, inMath, FALSE);
				}
			      else
				{
				  /* look for the next cell */
				  child = CloseCellForNewColumn (currentrow,
								 nextCol, doc,
								 FALSE, inMath,
								 &span, &rowspan);
				  /* add before */
				  AddEmptyCellInRow (currentrow, colhead, child,
						     TRUE, doc, inMath, FALSE);
				}
			      while (rowspan >= 1 && currentrow)
				{
				  TtaNextSibling (&currentrow);
				  rowspan--;
				}
			    }
			}
		      else if (last)
			child = NULL;
		    }
		  else if (elType.ElTypeNum == HTML_EL_Table_body)
		    {
		      /* this child is the Table_body element */
		      /* get the first tbody element */
		      group = TtaGetFirstChild (child);
		      /* process all tbody elements */
		      while (group != NULL)
			{
			  if (group != groupdone)
			    {
			      currentrow = TtaGetFirstChild (group);
			      while (currentrow)
				{
				  rowspan = 1;
				  /* get the sibling cell */
				  if (last)
				    child = TtaGetLastChild (currentrow);
				  else
				    /* look for the previous cell */
				    child = CloseCellForNewColumn (currentrow,
								   prevCol, doc,
								   TRUE, inMath,
								   &span, &rowspan);
				  if (child)
				    {
				      if (!span)
					/* add a new cell after */
					AddEmptyCellInRow (currentrow, colhead, child,
							   FALSE, doc, inMath, FALSE);
				    }
				  else
				    {
				      /* look for the next cell */
				      child = CloseCellForNewColumn (currentrow,
								     nextCol, doc,
								     FALSE, inMath,
								     &span, &rowspan);
				      /* add before */
				      AddEmptyCellInRow (currentrow, colhead, child,
							 TRUE, doc, inMath, FALSE);
				    }
				  TtaNextSibling (&currentrow);
				}
			    }
			    
			  if (last)
			    {
			      group = NULL;
			      child = NULL;
			    }
			  else
			    TtaNextSibling (&group);
			}
		    }
		  if (child != NULL)
		    TtaNextSibling (&child);
		}
	    }
	}
    }
  if (select)
    {
      child = GetCellFromColumnHead (row, colhead, inMath);
      child = TtaGetFirstChild (child);
      if (child)
	TtaSelectElement (doc, child);
    }
  return colhead;
}


/*----------------------------------------------------------------------
   SetRowExt       Sets the attribute RowExt of cell "cell" in row 
   "row", according to span.
  ----------------------------------------------------------------------*/
static void SetRowExt (Element cell, int span, Document doc, ThotBool inMath)
{
   Element             row, spannedrow, nextspannedrow;
   ElementType         elType;
   AttributeType       attrType;
   Attribute           attr;
   ThotBool            newAttr;

   row = TtaGetParent (cell);
   elType = TtaGetElementType (row);
   attrType.AttrSSchema = elType.ElSSchema;
   if (inMath)
     attrType.AttrTypeNum = MathML_ATTR_MRowExt;
   else
     attrType.AttrTypeNum = HTML_ATTR_RowExt;

   if (span <= 1)
     /* remove attribute RowlExt if it is attached to the cell */
     {
       attr = TtaGetAttribute (cell, attrType);
       if (attr)
	 {
	   TtaRegisterAttributeDelete (attr, cell, doc);
	   TtaRemoveAttribute (cell, attr, doc);
	 }
     }
   else
     {
       spannedrow = row;
       while (span > 0 && spannedrow != NULL)
	 {
	   nextspannedrow = spannedrow;
	   TtaNextSibling (&nextspannedrow);
	   elType = TtaGetElementType (spannedrow);
	   /* process only Table_row elements */
	   if ((!inMath && elType.ElTypeNum == HTML_EL_Table_row) ||
	       (inMath && (elType.ElTypeNum == MathML_EL_MTR ||
			   elType.ElTypeNum == MathML_EL_MLABELEDTR)))
	     {
	       if (span == 1 || nextspannedrow == NULL)
		 if (spannedrow != row)
		   {
		     attr = TtaGetAttribute (cell, attrType);
		     if (attr == NULL)
		       {
			 newAttr = TRUE;
			 attr = TtaNewAttribute (attrType);
			 if (attr != NULL)
			   TtaAttachAttribute (cell, attr, doc);
		       }
		     else
		       newAttr = FALSE;
		     if (attr)
		       {
			 if (!newAttr)
			   TtaRegisterAttributeReplace (attr, cell, doc);
			 TtaSetAttributeReference (attr, cell, doc, spannedrow,
						   doc);
			 if (newAttr)
			   TtaRegisterAttributeCreate (attr, cell, doc);
		       }
		   }
	       span--;
	     }
	   spannedrow = nextspannedrow;
	 }
     }
}

/*----------------------------------------------------------------------
   SetColExt
   Sets the attribute ColExt of cell "cell" according to span.
  ----------------------------------------------------------------------*/
static void SetColExt (Element cell, int span, Document doc, ThotBool inMath)
{
  ElementType         elType;
  Element             colHead, nextColHead;
  AttributeType       attrType, refColType;
  Attribute           attr, refColAttr;
  int                 i;
  char                name[50];
  Document            refdoc;
  ThotBool            newAttr;

  elType = TtaGetElementType (cell);
  attrType.AttrSSchema = elType.ElSSchema;
  refColType.AttrSSchema = elType.ElSSchema;
  if (inMath)
    {
      attrType.AttrTypeNum = MathML_ATTR_MColExt;
      refColType.AttrTypeNum = MathML_ATTR_MRef_column;
    }
  else
    {
      attrType.AttrTypeNum = HTML_ATTR_ColExt;
      refColType.AttrTypeNum = HTML_ATTR_Ref_column;
    }
  if (span <= 1)
    /* remove attribute ColExt */
    {
      attr = TtaGetAttribute (cell, attrType);
      if (attr)
	{
	  TtaRegisterAttributeDelete (attr, cell, doc);
	  TtaRemoveAttribute (cell, attr, doc);
	}
    }
  else
    {
      /* set attribute ColExt */
      attr = TtaGetAttribute (cell, attrType);
      if (!attr)
	{
	  newAttr = TRUE;
	  attr = TtaNewAttribute (attrType);
	  if (attr)
	    TtaAttachAttribute (cell, attr, doc);
	}
      else
	newAttr = FALSE;
      if (attr)
	{
	  refColAttr = TtaGetAttribute (cell, refColType);
	  if (refColAttr)
	    {
	      TtaGiveReferenceAttributeValue (refColAttr, &colHead, name,
					      &refdoc);
	      if (colHead)
		{
		  nextColHead = colHead;
		  for (i = 0; i < span && nextColHead; i++)
		    {
		      colHead = nextColHead;
		      TtaNextSibling (&nextColHead);
		    }
		  if (!newAttr)
		    TtaRegisterAttributeReplace (attr, cell, doc);
		  TtaSetAttributeReference (attr, cell, doc, colHead, doc);
		  if (newAttr)
		    TtaRegisterAttributeCreate (attr, cell, doc);
		}
	    }
	}
    }
}

/*----------------------------------------------------------------------
   MaximumRowSpan                                                  
  ----------------------------------------------------------------------*/
static int MaximumRowSpan (Element row, int span, ThotBool inMath)
{
   ElementType         elType;
   AttributeType       attrType;
   Attribute           attr;
   Element             cell, currentrow;
   int                 max, rownb;

   if (span < 2 || row == NULL)
      return span;
   max = span;
   currentrow = row;
   rownb = 1;
   elType = TtaGetElementType (row);
   attrType.AttrSSchema = elType.ElSSchema;
   while (currentrow != NULL && rownb <= max)
     {
       cell = TtaGetFirstChild (currentrow);
       while (cell != NULL)
	 {
	   elType = TtaGetElementType (cell);
	   /* process only cell elements */
	   if ((!inMath && (elType.ElTypeNum == HTML_EL_Data_cell ||
			    elType.ElTypeNum == HTML_EL_Heading_cell)) ||
	       (inMath && elType.ElTypeNum == MathML_EL_MTD))
	     {
	       if (inMath)
		 attrType.AttrTypeNum = MathML_ATTR_rowspan_;
	       else
		 attrType.AttrTypeNum = HTML_ATTR_rowspan_;
	       attr = TtaGetAttribute (cell, attrType);
	       if (attr != NULL)
		 {
		   /* this cell has an attribute rowspan */
		   span = TtaGetAttributeValue (attr);
		   if (span > 1)
		     if (max < rownb + span - 1)
		       max = rownb + span - 1;
		 }
	     }
	   TtaNextSibling (&cell);
	 }
       rownb++;
       TtaNextSibling (&currentrow);
     }
   return max;
}

/*----------------------------------------------------------------------
  RemoveColumn remove the current colhead if it's empty.
  The parameter ifEmpty makes removing optional.
  Returns TRUE if the column has been removed.
  ----------------------------------------------------------------------*/
ThotBool RemoveColumn (Element colhead, Document doc, ThotBool ifEmpty,
		       ThotBool inMath)
{
  Element             row, firstrow;
  Element             cell, group, table;
  ElementType         elType;
  AttributeType       attrType;
  Attribute           attr;
  int                 rowType;
  ThotBool            empty, span;

  empty = FALSE; /* return TRUE if the column is deleted */
  if (colhead == NULL)
    return (empty);

  /* check if the column becomes empty */
  elType = TtaGetElementType (colhead);
  attrType.AttrSSchema = elType.ElSSchema;
  if (inMath)
    {
      elType.ElTypeNum = MathML_EL_MTABLE;
      rowType = MathML_EL_TableRow;
      attrType.AttrTypeNum = MathML_ATTR_columnspan;
    }
  else
    {
      elType.ElTypeNum = HTML_EL_Table;
      rowType = HTML_EL_Table_row;
      attrType.AttrTypeNum = HTML_ATTR_colspan_;
    }

  table = TtaGetTypedAncestor (colhead, elType);
  elType.ElTypeNum = rowType;
  firstrow = TtaSearchTypedElement (elType, SearchForward, table);
  if (colhead != NULL && firstrow != NULL)
    {
      empty = TRUE;  /* TRUE when all cells are empty */
      if (ifEmpty)
	{
	  row = firstrow;
	  /* the rows group could be thead, tbody, tfoot */
	  group = TtaGetParent (row);
	  while (row && empty)
	    {
	      cell = GetCellFromColumnHead (row, colhead, inMath);
	      if (cell != NULL && TtaGetElementVolume (cell) != 0)
		empty = FALSE;
	      else
		{
	        TtaNextSibling (&row);
	        /* do we have to get a new group of rows */
	        if (row == NULL && !inMath)
		  {
		  elType = TtaGetElementType (group);
		  if (elType.ElTypeNum == HTML_EL_tbody)
		    {
		      row = group;
		      group = TtaGetParent (group);
		      TtaNextSibling (&row);
		      if (row != NULL)
			group = row;
		      else
			TtaNextSibling (&group);
		    }
		  else
		    {
		      TtaNextSibling (&group);
		      if (group != NULL)
			{
			  elType = TtaGetElementType (group);
			  if (elType.ElTypeNum == HTML_EL_Table_body)
			    group = TtaGetFirstChild (group);
			}
		    }
		  if (group != NULL)
		    row = TtaGetFirstChild (group);
		  else
		    row = NULL;
		  }
		}
	    }
	}

      if (empty)
	{
	  span = FALSE; /* TRUE when a cell with a span value is deleted */
	  /* remove the current column */
	  row = firstrow;
	  /* the rows group could be thead, tbody, tfoot */
	  group = TtaGetParent (row);
	  while (row != NULL)
	    {
	      cell = GetCellFromColumnHead (row, colhead, inMath);
	      if (cell != NULL)
		{
		  if (!inMath)
		    {
		      /* look at whether it's a spanned column */
		      attr = TtaGetAttribute (cell, attrType);
		      if (attr != NULL && TtaGetAttributeValue (attr) > 1)
			span = TRUE;
		    }
		  /* register the deleted cells only if the column is
		     deleted explicitely by the user */
		  if (!ifEmpty)
		    if (TtaPrepareUndo (doc))
		      {
		        TtaRegisterElementDelete (cell, doc);
                        /* change the value of "info" in the latest cell
			   deletion recorded in the Undo queue. The goal is to
			   allow procedure CellPasted to regenerate only one
			   column head when undoing the operation */
			TtaChangeInfoLastRegisteredElem (doc, 3);
		      }
		  TtaDeleteTree (cell, doc);
		}
	      TtaNextSibling (&row);
	      /* do we have to get a new group of rows */
	      if (row == NULL && !inMath)
		{
		  elType = TtaGetElementType (group);
		  if (elType.ElTypeNum == HTML_EL_tbody)
		    {
		      row = group;
		      group = TtaGetParent (group);
		      TtaNextSibling (&row);
		      if (row != NULL)
			group = row;
		      else
			TtaNextSibling (&group);
		    }
		  else
		    {
		      TtaNextSibling (&group);
		      if (group != NULL)
			{
			  elType = TtaGetElementType (group);
			  if (elType.ElTypeNum == HTML_EL_Table_body)
			    group = TtaGetFirstChild (group);
			}
		    }
		  if (group != NULL)
		    row = TtaGetFirstChild (group);
		  else
		    row = NULL;
		}
	    }
	  TtaDeleteTree (colhead, doc);
	  if (!ifEmpty)
	    if (TtaPrepareUndo (doc))
	      /* The value of "info" in the latest cell deletion recorded in
		 the Undo queue should be 4 to allow procedure CellPasted to
		 regenerate a column head for the last cell when undoing
		 the operation */
		TtaChangeInfoLastRegisteredElem (doc, 4);
	  if (span)
	     CheckAllRows (table, doc, FALSE, FALSE);
	}
    }
  return (empty);
}

#define MAX_COLS 100
/*----------------------------------------------------------------------
  CheckAllRows
  ----------------------------------------------------------------------*/
void CheckAllRows (Element table, Document doc, ThotBool placeholder,
		   ThotBool deleteLastEmptyColumns)
{
  Element            *colElement;
  Element             row, nextRow, firstrow, colhead, prevColhead;
  Element             cell, nextCell, group, prevGroup, new_;
  ElementType         elType;
  AttributeType       attrTypeHSpan, attrTypeVSpan, attrType;
  Attribute           attr;
  int                *colVSpan;
  int                 span, cRef, cNumber, extracol;
  int                 i, rowType;
  ThotBool            inMath, newAttr;

  if (table == NULL)
    return;

  colElement = (Element*)TtaGetMemory (sizeof (Element) * MAX_COLS);
  colVSpan = (int *)TtaGetMemory (sizeof (int) * MAX_COLS);
  /* store the list of colheads */
  elType = TtaGetElementType (table);
  inMath = !TtaSameSSchemas (elType.ElSSchema,
			     TtaGetSSchema ("HTML", doc));
  if (inMath)
    {
      elType.ElTypeNum = MathML_EL_MColumn_head;
      rowType = MathML_EL_TableRow;
    }
  else
    {
      elType.ElTypeNum = HTML_EL_Column_head;
      rowType = HTML_EL_Table_row;
    }
  
  colhead = TtaSearchTypedElement (elType, SearchForward, table);
  cNumber = 0;
  while (colhead != 0 && cNumber < MAX_COLS)
    {
      colElement[cNumber] = colhead;
      colVSpan[cNumber] = 0;
      TtaNextSibling (&colhead);
      cNumber++;
    }
  cell = NULL;
  /* number of extra columns */
  extracol = 0;
  attrType.AttrSSchema = elType.ElSSchema;
  attrTypeHSpan.AttrSSchema = elType.ElSSchema;
  if (inMath)
    attrTypeHSpan.AttrTypeNum = MathML_ATTR_columnspan;
  else
    attrTypeHSpan.AttrTypeNum = HTML_ATTR_colspan_;
  attrTypeVSpan.AttrSSchema = elType.ElSSchema;
  if (inMath)
    attrTypeVSpan.AttrTypeNum = MathML_ATTR_rowspan_;
  else
    attrTypeVSpan.AttrTypeNum = HTML_ATTR_rowspan_;
  elType.ElTypeNum = rowType;
  firstrow = TtaSearchTypedElement (elType, SearchForward, table);
  if (cNumber != 0 && firstrow != NULL)
    {
    row = firstrow;
    /* the rows group could be thead, tbody, tfoot */
    group = TtaGetParent (row);
    while (row != NULL)
      {
      nextRow = row;
      TtaNextSibling (&nextRow);
      elType = TtaGetElementType (row);
      if ((!inMath && elType.ElTypeNum == rowType) ||
	  (inMath && (elType.ElTypeNum == MathML_EL_MTR ||
		      elType.ElTypeNum == MathML_EL_MLABELEDTR)))
	{
	/* treat all cells in the row */
	cRef = 0;
	cell = NULL;
	nextCell = TtaGetFirstChild (row);
	if (inMath && elType.ElTypeNum == MathML_EL_MLABELEDTR)
	  /* skip the first significant child of the row: it's a label */
	  {
	  /* skip comments and PIs first */
	  do
	    {
	    elType = TtaGetElementType (nextCell);
	    if (elType.ElTypeNum == MathML_EL_XMLcomment ||
		elType.ElTypeNum == MathML_EL_XMLPI)
	      TtaNextSibling (&nextCell);
	    }
	  while (nextCell &&
		 (elType.ElTypeNum == MathML_EL_XMLcomment ||
		  elType.ElTypeNum == MathML_EL_XMLPI));
	  /* skip the following element */
	  TtaNextSibling (&nextCell);
	  }
	while (nextCell != NULL)
	  {
	  cell = nextCell;
	  TtaNextSibling (&nextCell);
	  elType = TtaGetElementType (cell);
	  if (!inMath && elType.ElTypeNum == HTML_EL_Table_cell)
	    {
	    /* replace the Table_cell by a Data_cell */
	    elType.ElTypeNum = HTML_EL_Data_cell;
	    new_ = TtaNewTree (doc, elType, "");
	    TtaInsertFirstChild (&new_, cell, doc);
	    cell = new_;
	    }
	  
	  /* process only cell elements */
	  if ((inMath && elType.ElTypeNum == MathML_EL_MTD) ||
	      (!inMath && (elType.ElTypeNum == HTML_EL_Data_cell ||
			   elType.ElTypeNum == HTML_EL_Heading_cell)))
	    {
	    /* is there any rowspan attribute in previous row */
	    while (cRef < cNumber && colVSpan[cRef] > 0)
	      {
	      colVSpan[cRef]--;
	      cRef++;
	      }
	    
	    if (cRef == cNumber && cRef < MAX_COLS)
	      {
	      /* there is no Column_head for that cell */
	      /* Create an additional Column_head */
	      colElement[cRef] = NewColumnHead (colElement[cRef - 1], FALSE,
						TRUE, row, doc, inMath, TRUE);
	      colVSpan[cRef] = 0;
	      cNumber++;
	      }
	    
	    if (cRef < cNumber)
	      {
	      /* relate the cell with its corresponding Column_head */
	      RelateCellWithColumnHead (cell, colElement[cRef], doc, inMath);

	      /* is there an attribute rowspan for that cell ? */
	      attr = TtaGetAttribute (cell, attrTypeVSpan);
	      if (attr == NULL)
		span = 1;
	      else
		{
		span = TtaGetAttributeValue (attr);
		if (span > 1)
		  {
		  /* Set the attribute RowExt if row span > 1 */
		  colVSpan[cRef] = span - 1;
		  }
		}
	      SetRowExt (cell, span, doc, inMath);

	      /* if there an attribute colspan for that cell,
		 update attribute ColExt */
	      if (inMath)
		attrType.AttrTypeNum = MathML_ATTR_MColExt;
	      else
		attrType.AttrTypeNum = HTML_ATTR_ColExt;
	      attr = TtaGetAttribute (cell, attrTypeHSpan);
	      if (attr == NULL)
		/* no colspan attribute */
		span = 1;
	      else
		{
		span = TtaGetAttributeValue (attr);
		if (span > 1)
		  {
		  i = 1;
		  while (i < span && cRef + 1 < cNumber)
		    {
		    cRef++;
		    colVSpan[cRef] = colVSpan[cRef-1];
		    i++;
		    }
		  if (nextCell)
		    extracol++;
		    /* Create additional Column_heads */
		    while (i < span)
		      {
		      if (cRef + 1 < MAX_COLS)
			{
			cRef++;
			colElement[cRef] = NewColumnHead (colElement[cRef-1],
					  FALSE, TRUE, row, doc, inMath, TRUE);
			cNumber++;
			}
		      colVSpan[cRef] = colVSpan[cRef-1];
		      i++;
		      }
		  }
		}
	      SetColExt (cell, span, doc, inMath);

	      cRef++;
	      }
	    }

	  /* accept comments, PIs and invalid elements */
	  else if ((!inMath &&
		    elType.ElTypeNum != HTML_EL_Invalid_element &&
		    elType.ElTypeNum != HTML_EL_Comment_ &&
		    elType.ElTypeNum != HTML_EL_XMLPI) ||
		   (inMath &&
		    elType.ElTypeNum != MathML_EL_XMLcomment &&
		    elType.ElTypeNum != MathML_EL_XMLPI))
	    /* Delete any other type of element */
	    TtaDeleteTree (cell, doc);
	  }
	
	/* check missing cells */
	while (cRef < cNumber)
	  {
	  if (colVSpan[cRef] > 0)
	    colVSpan[cRef]--;
	  else
	    cell = AddEmptyCellInRow (row, colElement[cRef], cell, FALSE, doc,
				      inMath, placeholder);
	  cRef++;
	  }
	}
      /* accept comments, PIs and invalid elements */
      else if ((!inMath &&
		elType.ElTypeNum != HTML_EL_Invalid_element &&
		elType.ElTypeNum != HTML_EL_Comment_ &&
		elType.ElTypeNum != HTML_EL_XMLPI) ||
	       (inMath &&
		elType.ElTypeNum != MathML_EL_XMLcomment &&
		elType.ElTypeNum != MathML_EL_XMLPI))
	/* Delete any other type of element */
	TtaDeleteTree (row, doc);
      row = nextRow;
      
      /* do we have to get a new group of rows */
      if (row == NULL && !inMath)
	{
	elType = TtaGetElementType (group);
	if (elType.ElTypeNum == HTML_EL_tbody)
	  {
	  row = group;
	  group = TtaGetParent (group);
	  TtaNextSibling (&row);
	  if (row != NULL)
	    group = row;
	  else
	    TtaNextSibling (&group);
	  }
	else
	  {
	  TtaNextSibling (&group);
	  if (group != NULL)
	    {
	    elType = TtaGetElementType (group);
	    if (elType.ElTypeNum == HTML_EL_Table_body)
	      group = TtaGetFirstChild (group);
	    }
	  }
	if (group != NULL)
	  {
	  elType = TtaGetElementType (group);
	  /* skip comments, PIs and invalid elements */
	  while (group &&
		 (elType.ElTypeNum == HTML_EL_TEXT_UNIT ||
		  elType.ElTypeNum == HTML_EL_Invalid_element ||
		  elType.ElTypeNum == HTML_EL_Comment_ ||
		  elType.ElTypeNum == HTML_EL_XMLPI))
	    {
	      prevGroup = group;
	      TtaNextSibling (&group);
	      if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
		TtaDeleteTree (prevGroup, doc);
	      if (group)
		elType = TtaGetElementType (group);
	    }
	  if (group)
	    {
	      elType = TtaGetElementType (group);
	      if (elType.ElTypeNum == HTML_EL_Table_foot)
		/* don't look for rows in the Table_foot! */
		row = NULL;
	      else
		row = TtaGetFirstChild (group);
	    }
	  else
	    row = NULL;
	  }
	else
	  row = NULL;
	}
      }
    }

  /* if there are some empty columns at the end, remove them */
  if (deleteLastEmptyColumns && cNumber > 0)
    {
    /* start with the last column of the table */
    colhead = colElement[cNumber - 1];
    while (colhead)
      {
      /* get the previous column head before it is deleted */
      prevColhead = colhead;
      TtaPreviousSibling (&prevColhead);
      /* remove this column if it is empty */
      if (!RemoveColumn (colhead, doc, TRUE, inMath))
	/* this column is not empty and has not been removed. Stop */
	colhead = NULL;
      else
	/* the column was empty and has been removed */
	colhead = prevColhead;     
      }
    }

  TtaFreeMemory (colElement);
  TtaFreeMemory (colVSpan);
}

/*----------------------------------------------------------------------
   CheckTable      Check a table and create the missing elements.  
  ----------------------------------------------------------------------*/
void CheckTable (Element table, Document doc)
{
  ElementType         elType;
  Element             el, columnHeads, thead, tfoot, firstcolhead,
                      tbody, Tablebody, firstgroup, prevrow,
                      foot, prevEl, nextEl,
		      enclosingTable;
  AttributeType       attrType;
  Attribute           attr;
  int                 PreviousStuctureChecking;
  ThotBool            before;

  firstcolhead = NULL;
  PreviousStuctureChecking = 0;
  if (table != NULL)
    {
      /* what are the children of element table? */
      columnHeads = NULL;
      thead = NULL;
      tfoot = NULL;
      Tablebody = NULL;
      el = TtaGetFirstChild (table);
      while (el != NULL)
	{
	  elType = TtaGetElementType (el);
	  if (elType.ElTypeNum == HTML_EL_Table_head)
	    columnHeads = el;
	  else if (elType.ElTypeNum == HTML_EL_Table_body)
	    {
	      if (Tablebody == NULL)
		Tablebody = el;
	    }
	  else if (elType.ElTypeNum == HTML_EL_thead)
	    thead = el;
	  else if (elType.ElTypeNum == HTML_EL_tfoot)
	    tfoot = el;
	  TtaNextSibling (&el);
	}
      if (columnHeads != NULL)
	/* this table has already been checked */
	return;

      /* if this table is within another table and if it has no border
	 attribute, create an attribute border=0 to avoid inheritance
	 of the the border attribute from the enclosing table */
      elType = TtaGetElementType (table);
      enclosingTable = TtaGetTypedAncestor (table, elType);
      if (enclosingTable != NULL)
	/* there is an enclosing table */
	{
	  attrType.AttrSSchema = elType.ElSSchema;
	  attrType.AttrTypeNum = HTML_ATTR_Border;
	  if (TtaGetAttribute (enclosingTable, attrType) != NULL)
	    /* the enclosing table has a Border attribute */
	    if (TtaGetAttribute (table, attrType) == NULL)
	      /* the current table has no Border attribute */
	      {
		attr = TtaNewAttribute (attrType);
		TtaAttachAttribute (table, attr, doc);
		TtaSetAttributeValue (attr, 0, table, doc);
	      }
	}

      /* disable document structure checking */
      PreviousStuctureChecking = TtaGetStructureChecking (doc);
      TtaSetStructureChecking (0, doc);

      /* create a Table_head element with a first Column_head */
      elType = TtaGetElementType (table);
      elType.ElTypeNum = HTML_EL_Table_head;
      columnHeads = TtaNewTree (doc, elType, "");
      if (columnHeads != NULL)
	{
	  firstcolhead = TtaGetFirstChild (columnHeads);
	  el = TtaGetFirstChild (table);
	  if (el == NULL)
	    TtaInsertFirstChild (&columnHeads, table, doc);
	  else
	    {
	      /* skip elements Comment and Invalid_element and insert the new
	         element Table_head, after the element caption if it is
		 present or as the first child if there is no caption */
	      while (el != NULL)
		{
		  elType = TtaGetElementType (el);
		  if (elType.ElTypeNum == HTML_EL_Invalid_element ||
		      elType.ElTypeNum == HTML_EL_Comment_ ||
		      elType.ElTypeNum == HTML_EL_XMLPI)
		    TtaNextSibling (&el);
		  else
		    {
		      before = elType.ElTypeNum != HTML_EL_CAPTION;
		      TtaInsertSibling (columnHeads, el, before, doc);
		      el = NULL;
		    }
		}
	    }

	  if (Tablebody == NULL)
	    {
	      /* There is no Table_body element. Create a Table_body element */
	      elType.ElTypeNum = HTML_EL_Table_body;
	      Tablebody = TtaNewElement (doc, elType);
	      if (Tablebody != NULL)
		{
		  if (thead != NULL)
		    TtaInsertSibling (Tablebody, thead, FALSE, doc);
		  else
		    TtaInsertSibling (Tablebody, columnHeads, FALSE, doc);
		  /* collect all Table_row elements and put them in the new
		     Table_body */
		  tbody = NULL;
		  el = Tablebody;
		  TtaNextSibling (&el);
		  prevrow = NULL;
		  prevEl = NULL;
		  attrType.AttrSSchema = elType.ElSSchema;
		  while (el != NULL)
		    {
		      elType = TtaGetElementType (el);
		      nextEl = el;
		      TtaNextSibling (&nextEl);
		      if (elType.ElTypeNum == HTML_EL_Table_row)
			{
			  TtaRemoveTree (el, doc);
			  if (prevrow == NULL)
			    {
			      /* create a tbody element */
			      elType.ElTypeNum = HTML_EL_tbody;
			      tbody = TtaNewElement (doc, elType);
			      if (prevEl == NULL)
				TtaInsertFirstChild (&tbody, Tablebody, doc);
			      else
				TtaInsertSibling (tbody, prevEl, FALSE, doc);
			      TtaInsertFirstChild (&el, tbody, doc);
			    }
			  else
			    TtaInsertSibling (el, prevEl, FALSE, doc);
			  prevrow = el;
			  prevEl = el;
			}
		      else
			{
			  if (elType.ElTypeNum == HTML_EL_tbody)
			    {
			      if (prevrow != NULL)
				prevEl = TtaGetParent (prevrow);
			      prevrow = NULL;
			    }
			  TtaRemoveTree (el, doc);
			  if (prevEl == NULL)
			    TtaInsertFirstChild (&el, Tablebody, doc);
			  else
			    TtaInsertSibling (el, prevEl, FALSE, doc);
			  prevEl = el;
			}
		      el = nextEl;
		    }
		}
	    }

	  /* create a Table_foot element at the end */
	  elType.ElTypeNum = HTML_EL_Table_foot;
	  foot = TtaNewTree (doc, elType, "");
	  if (foot != NULL)
	    {
	    if (tfoot != NULL)
	      {
		/* move element tfoot at the end */
		TtaRemoveTree (tfoot, doc);
		TtaInsertSibling (tfoot, Tablebody, FALSE, doc);
		TtaInsertSibling (foot, tfoot, FALSE, doc);
	      }
	    else
	      TtaInsertSibling (foot, Tablebody, FALSE, doc);
	    }
	  
	  if (thead != NULL)
	    firstgroup = thead;
	  else if (Tablebody != NULL)
	    firstgroup = Tablebody;
	  else
	    firstgroup = tfoot;

	  /* associate each cell with a column */
	  CheckAllRows (table, doc, FALSE, FALSE);
	  CheckTableAfterCellUpdate = TRUE;
	}
    }
  /* resume document structure checking */
  TtaSetStructureChecking ((ThotBool)PreviousStuctureChecking, doc);
}

/*----------------------------------------------------------------------
  NewCell  a new cell has been created in a HTML table.
  If generateColumn is TRUE, the new cell generates a new column head.
  If generateEmptyCells is TRUE, generate empty cells for the same column
  in other rows.
  ----------------------------------------------------------------------*/
void NewCell (Element cell, Document doc, ThotBool generateColumn,
	      ThotBool generateEmptyCells)
{
  Element             newcell, row;
  Element             colhead;
  ElementType         elType;
  AttributeType       attrType;
  Attribute           attr;
  DisplayMode         dispMode;
  char              ptr[100];
  int                 span, i;
  ThotBool            before, inMath;

  dispMode = TtaGetDisplayMode (doc);
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, DeferredDisplay);

  elType = TtaGetElementType (cell);
  inMath = !TtaSameSSchemas (elType.ElSSchema, TtaGetSSchema ("HTML", doc));

  if (!inMath && elType.ElTypeNum == HTML_EL_Table_cell)
    /* change the cell into a Data_cell */
    {
      elType.ElTypeNum = HTML_EL_Data_cell;
      newcell = TtaNewTree (doc, elType, "");
      TtaInsertFirstChild (&newcell, cell, doc);
      cell = newcell;
    }
  else
    newcell = cell;

  if (CheckTableAfterCellUpdate)
    {
      /* insert a new column here */
      if (inMath)
	{
	  elType.ElTypeNum = MathML_EL_TableRow;
	  attrType.AttrSSchema = elType.ElSSchema;
	  attrType.AttrTypeNum = MathML_ATTR_MRef_column;
	}
      else
	{
	  elType.ElTypeNum = HTML_EL_Table_row;
	  attrType.AttrSSchema = elType.ElSSchema;
	  attrType.AttrTypeNum = HTML_ATTR_Ref_column;
	}
      /* get the enclosing row element */
      row = TtaGetTypedAncestor (cell, elType);
      /* locate the previous or the next column head */
      colhead = NULL;
      TtaPreviousSibling (&cell);
      if (cell == NULL)
	{
	  cell = newcell;
	  TtaNextSibling (&cell);
	  before = TRUE;
	  if (cell != NULL)
	    {
	      attr = TtaGetAttribute (cell, attrType);
	      if (attr != NULL)
		TtaGiveReferenceAttributeValue (attr, &colhead, ptr, &i);
	    }
	}
      else
	{
	  before = FALSE;
	  attr = TtaGetAttribute (cell, attrType);
	  if (attr != NULL)
	    {
	      TtaGiveReferenceAttributeValue (attr, &colhead, ptr, &i);
	      /* if there an attribute colspan for that cell, select the
		 right colhead */
	      if (inMath)
		attrType.AttrTypeNum = MathML_ATTR_columnspan;
	      else
		attrType.AttrTypeNum = HTML_ATTR_colspan_;
	      attr = TtaGetAttribute (cell, attrType);
	      if (attr != NULL)
		{
		  span = TtaGetAttributeValue (attr);
		  if (span > 1)
		    for (i = 1; i < span && colhead != NULL; i++)
		      TtaNextSibling (&colhead);
		}
	    }
	}
      if (colhead != NULL)
	{
	  if (generateColumn)
	    /* generate the new column */
	    colhead = NewColumnHead (colhead, before, FALSE, row, doc, inMath,
				     generateEmptyCells);
	  else if (before)
	    /* select the previous column */
	    TtaPreviousSibling (&colhead);
	  else
	    /* select the next column */
	    TtaNextSibling (&colhead);
	  /* relate the new cell to the new colhead */
	  RelateCellWithColumnHead (newcell, colhead, doc, inMath);
	}
    }

   TtaSetDisplayMode (doc, dispMode);
}


/*----------------------------------------------------------------------
   CellCreated
   a new cell has been created in a table
  ----------------------------------------------------------------------*/
void CellCreated (NotifyElement * event)
{
  Element             cell, row;
  Document            doc;

  if (event->info == 1)
    /* the delete is already done by undo */
    return;
  cell = event->element;
  doc = event->document;
  row = TtaGetParent (cell);
  if (row == CurrentRow)
    {
      /* the new cell belongs to the newly created row. Already processed */
      /* by RowCreated. */
      if (cell == CurrentCell)
	{
	  CurrentRow = NULL;
	  CheckTableAfterCellUpdate = TRUE;
	}
    }
  else
    /* a new cell in an existing row */
    {
      NewCell (cell, doc, TRUE, TRUE);
      HandleColAndRowAlignAttributes (row, doc);
    }
}

/*----------------------------------------------------------------------
   CellPasted                                              
  ----------------------------------------------------------------------*/
void CellPasted (NotifyElement * event)
{
   Element             cell, nextcell, row;
   Document            doc;

  if (event->info == 1)
    return;
   cell = event->element;
   doc = event->document;
   if (!ElementOKforProfile (cell, doc))
     return;
   row = TtaGetParent (cell);
   if (row == CurrentPastedRow)
     /* this cell is part of the pasted row */
     {
       nextcell = cell;
       TtaNextSibling (&nextcell);
       if (nextcell == NULL)
	 /* this is the last cell in the pasted row */
	 CurrentPastedRow = NULL;
     }
   else
     {
       /* regenerate the corresponding ColumnHead except if it's called by
	  undo for reinserting the cells deleted by a "Delete Column"
	  command (only the first reinserted cell has to create a ColumnHead)
          See function RemoveColumn above */
       if (event->info == 4)
	 /* undoing the deletion of the last cell in a "delete column"
	    command. Regenerate the corresponding ColumnHead and link the
	    restored cell with that ColumnHead, but do not generate empty
	    cells in other rows */
	 NewCell (cell, doc, TRUE, FALSE);
       else if (event->info == 3)
	 /* undoing the deletion of any other cell in a "delete column"
	    command. Link the restored cell with the corresponding ColumnHead*/
	 NewCell (cell, doc, FALSE, FALSE);
       else
	 /* usual case : regenerate the corresponding ColumnHead as well as
	    empty cells in other rows */
	 NewCell (cell, doc, TRUE, TRUE);
       HandleColAndRowAlignAttributes (row, doc);
       CurrentPastedRow = NULL;
     }
   /* Check attribute NAME or ID in order to make sure that its value */
   /* is unique in the document */
   MakeUniqueName (cell, doc);
}


/*----------------------------------------------------------------------
   PreDeleteRow                                            
  ----------------------------------------------------------------------*/
static void PreDeleteRow (Element row, Document doc)
{
   Element             cell;
   ElementType         elType;
   AttributeType       attrType;
   Attribute           attr;
   int                 span;
   ThotBool            inMath;

   if (row == NULL)
      return;
   cell = TtaGetFirstChild (row);
   if (cell == NULL)
      return;
   /* seach the maximum value of attribute rowspan for the deleted row */
   elType = TtaGetElementType (row);
   inMath = !TtaSameSSchemas (elType.ElSSchema, TtaGetSSchema ("HTML",
							       doc));
   attrType.AttrSSchema = elType.ElSSchema;
   if (inMath)
     attrType.AttrTypeNum = MathML_ATTR_rowspan_;
   else
     attrType.AttrTypeNum = HTML_ATTR_rowspan_;
   MaxRowSpan = 1;
   while (cell != NULL)
     {
       attr = TtaGetAttribute (cell, attrType);
       if (attr != NULL)
	 /* this cell has an attribute rowspan */
	 {
	   span = TtaGetAttributeValue (attr);
	   if (span > MaxRowSpan)
	     MaxRowSpan = span;
	 }
       TtaNextSibling (&cell);
     }
   MaxRowSpan = MaximumRowSpan (row, MaxRowSpan, inMath);
}

/*----------------------------------------------------------------------
   DeleteRow                                               
  ----------------------------------------------------------------------*/
ThotBool DeleteRow (NotifyElement * event)
{
  if (CurrentDeletedRow == NULL)
    {
      CurrentDeletedRow = event->element;
      PreDeleteRow (event->element, event->document);
      CheckTableAfterCellUpdate = FALSE;
    }
   return FALSE;		/* let Thot perform normal operation */
}


/*----------------------------------------------------------------------
   RowDeleted                                              
  ----------------------------------------------------------------------*/
void RowDeleted (NotifyElement * event)
{
  Element             rowgroup, table;
  ElementType         elType;
  Document            doc;
  ThotBool            inMath;

   CurrentDeletedRow = NULL;
   doc = event->document;
   rowgroup = event->element;
   if (rowgroup == NULL)
     /* the row doesn't exist */
     return;
   elType = TtaGetElementType (rowgroup);
   inMath = TtaSameSSchemas (elType.ElSSchema, TtaGetSSchema ("MathML",
							      doc));
   if (inMath)
     elType.ElTypeNum = MathML_EL_MTABLE;
   else
     elType.ElTypeNum = HTML_EL_Table;
   table = TtaGetTypedAncestor (rowgroup, elType);
   CheckAllRows (table, doc, FALSE, FALSE);
   CheckTableAfterCellUpdate = TRUE;
   HandleColAndRowAlignAttributes (rowgroup, doc);
}

/*----------------------------------------------------------------------
   DeleteCell                                              
  ----------------------------------------------------------------------*/
ThotBool DeleteCell (NotifyElement * event)
{
printf ("Delete cell\n");
#ifdef IV
  Element             cell;
  ElementType         elType;
  AttributeType       attrType;
  Attribute           attr;
  Document            refDoc;
  char                name[50];
  ThotBool            inMath;

  if (CurrentRow)
    /* manage only one row at a time */
    return FALSE;
  cell = event->element;
  CurrentRow = TtaGetParent (cell);
  if (CurrentRow == CurrentDeletedRow)
    {
      CurrentRow = NULL;
      return FALSE;
    }
  /* seach the maximum value of attribute rowspan for the deleted cell and */
  /* all its following cells */
  elType = TtaGetElementType (cell);
  inMath = !TtaSameSSchemas (elType.ElSSchema, TtaGetSSchema ("HTML", event->document));

  /* get previous or next cell */
  /* get current column */
  attrType.AttrSSchema = elType.ElSSchema;
  if (inMath)
    attrType.AttrTypeNum = MathML_ATTR_MRef_column;
  else
    attrType.AttrTypeNum = HTML_ATTR_Ref_column;

  attr = TtaGetAttribute (cell, attrType);
  if (attr != NULL)
    TtaGiveReferenceAttributeValue (attr, &CurrentColumn, name, &refDoc);
  else
    CurrentColumn = NULL;

  /* get current column span */
  if (inMath)
    attrType.AttrTypeNum = MathML_ATTR_columnspan;
  else
    attrType.AttrTypeNum = HTML_ATTR_colspan_;
  attr = TtaGetAttribute (cell, attrType);
  if (attr != NULL)
    CurrentSpan = TtaGetAttributeValue (attr);
  else
    CurrentSpan = 1;
#endif
  /* let Thot perform normal operation */
  return FALSE;
}


/*----------------------------------------------------------------------
   CellDeleted                                             
  ----------------------------------------------------------------------*/
void CellDeleted (NotifyElement * event)
{
printf ("Cell deleted\n");
#ifdef IV
  Element             cell, col, child;
  ElementType         elType;
  Document            doc;
  int                 span;
  ThotBool            removed;
  ThotBool            inMath;
  ThotBool            before;

  if (event->info == 1)
    /* the delete is already done by undo */
    return;
  doc = event->document;
  span = CurrentSpan;
  CurrentSpan = 0;
  elType = TtaGetElementType (event->element);
  inMath = !TtaSameSSchemas (elType.ElSSchema,
			     TtaGetSSchema ("HTML",event->document));
  removed = RemoveColumn (CurrentColumn, doc, TRUE, inMath);
  if (removed)
    span--;
  else if (CurrentColumn != NULL)
    {
      /* get the previous or the next one in the current row */
      before = FALSE;
      col = CurrentColumn;
      TtaPreviousSibling (&col);
      if (col == NULL)
	{
	  before = TRUE;
	  col = CurrentColumn;
	  TtaNextSibling (&col);
	}
      cell = CloseCellForNewColumn (CurrentRow, col, doc, before, inMath,
				    &span, &rowspan);
      /* regenerate an empty cell */
      cell = AddEmptyCellInRow (CurrentRow, CurrentColumn, cell, before, doc,
				inMath, FALSE);
      child = TtaGetFirstChild (cell);
      TtaSelectElement (doc, child);
    }
  if (CurrentRow)
    HandleColAndRowAlignAttributes (CurrentRow, doc);
  CurrentColumn = NULL;
  CurrentRow = NULL;
#endif
}

/*----------------------------------------------------------------------
   TableCreated                                            
  ----------------------------------------------------------------------*/
void TableCreated (NotifyElement * event)
{
   Element             table;
   Document            doc;
   ElementType         elType;
   AttributeType       attrType;
   Attribute           attr;

   table = event->element;
   doc = event->document;
   elType = TtaGetElementType (table);
   attrType.AttrSSchema = elType.ElSSchema;
   attrType.AttrTypeNum = HTML_ATTR_Border;
   attr = TtaGetAttribute (table, attrType);
   if (attr == NULL)
     {
	attr = TtaNewAttribute (attrType);
	if (attr != NULL)
	   TtaAttachAttribute (table, attr, doc);
     }
   TtaSetAttributeValue (attr, 1, table, doc);
   CheckTableAfterCellUpdate = TRUE;
   NewTable = TRUE;
}


/*----------------------------------------------------------------------
   TablebodyDeleted                                             
  ----------------------------------------------------------------------*/
void TablebodyDeleted (NotifyElement * event)
{
  Element             sibling, table;
  ElementType	      elType;
  Document            doc;
  ThotBool	      empty;

  if (event->info == 2)
     /* call from the Return key. Don't do anything */
     return;
  doc = event->document;
  table = event->element;
  sibling = TtaGetFirstChild (table);
  empty = TRUE;
  while (sibling != NULL && empty)
    {
      elType = TtaGetElementType (sibling);
      if (elType.ElTypeNum == HTML_EL_CAPTION ||
	  elType.ElTypeNum == HTML_EL_thead ||
	  elType.ElTypeNum == HTML_EL_Table_body ||
	  elType.ElTypeNum == HTML_EL_tfoot)
	if (TtaGetElementVolume (sibling) > 0)
	  empty = FALSE;
      TtaNextSibling (&sibling);
    }
  if (empty)
    {
      if (TtaPrepareUndo (doc))
	  /* register that the table is deleted */
	  TtaRegisterElementDelete (table, doc);
      TtaDeleteTree (table, doc);
    }
}


/*----------------------------------------------------------------------
   RowCreated                                              
  ----------------------------------------------------------------------*/
void RowCreated (NotifyElement * event)
{
  Element             row, table;
  ElementType         elType;
  Document            doc;
  ThotBool            inMath;

  row = event->element;
  doc = event->document;
  elType = TtaGetElementType (row);
  inMath = TtaSameSSchemas (elType.ElSSchema,
			    TtaGetSSchema ("MathML", doc));
  if (inMath)
    elType.ElTypeNum = MathML_EL_MTABLE;
  else
    elType.ElTypeNum = HTML_EL_Table;
  table = TtaGetTypedAncestor (row, elType);
  if (NewTable)
    {
      /* the table element is just created now
       We need to create the table_head element */
      if (inMath)
	CheckMTable (table, doc, FALSE);
      else
	CheckTable (table, doc);
      NewTable = FALSE;
    }
  else
    {
      CheckAllRows (table, doc, FALSE, FALSE);
      CheckTableAfterCellUpdate = FALSE;
      /* avoid processing the cells of the created row */
      CurrentRow = row;
    }
  HandleColAndRowAlignAttributes (row, doc);
  CurrentCell = TtaGetLastChild (row);
}

/*----------------------------------------------------------------------
   RowPasted                                               
  ----------------------------------------------------------------------*/
void RowPasted (NotifyElement * event)
{
  Element             row, table;
  ElementType         elType;
  Document            doc;
  ThotBool            inMath;

  if (event->info == 1)
    return;
  row = event->element;
  doc = event->document;
  if (!ElementOKforProfile (row, doc))
    return;
  elType = TtaGetElementType (row);
  inMath = TtaSameSSchemas (elType.ElSSchema,
			    TtaGetSSchema ("MathML", doc));
  if (inMath)
    elType.ElTypeNum = MathML_EL_MTABLE;
  else
    elType.ElTypeNum = HTML_EL_Table;
  table = TtaGetTypedAncestor (row, elType);
  if (NewTable)
    {
      /* the table element is just created now
       We need to create the table_head element */
      if (inMath)
	CheckMTable (table, doc, FALSE);
      else
	CheckTable (table, doc);
      NewTable = FALSE;
    }
  else
    {
      CheckAllRows (table, doc, FALSE, FALSE);
      CheckTableAfterCellUpdate = FALSE;
    }
  HandleColAndRowAlignAttributes (row, doc);
  /* avoid processing the cells of the created row */
  CurrentPastedRow = row;
  CurrentCell = TtaGetLastChild (row);
  /* Check attribute NAME or ID in order to make sure that its value */
  /* is unique in the document */
  MakeUniqueName (row, doc);
}

/*----------------------------------------------------------------------
   MoveCellContents
   Move all children of element nextCell as siblings of element previous
   (if it is not NULL) or as children of element cell (if previous is NULL),
   and delete element nextCell.
   When returning, previous is the latest element moved.
  ----------------------------------------------------------------------*/
static void MoveCellContents (Element nextCell, Element cell,
			      Element* previous, Document doc, ThotBool inMath)
{
  Element             child, nextChild;
  ElementType         elType;

  TtaRegisterElementDelete (nextCell, doc);
  if (TtaGetElementVolume (nextCell) == 0)
    /* empty cell */
    child = NULL;
  else
    {
      child = TtaGetFirstChild (nextCell);
      if (inMath)
	/* get the first element contained in the CellWrapper */
	child = TtaGetFirstChild (child);
    }
  /* move the contents of this cell to the cell whose
     attribute colspan has changed */
  while (child)
    {
      nextChild = child;  TtaNextSibling (&nextChild);
      TtaRemoveTree (child, doc);
      if (!inMath && *previous)
	/* if it's a pseudo-paragraph, change it into a
	   paragraph */
	{
	  elType = TtaGetElementType (child);
	  if (elType.ElTypeNum == HTML_EL_Pseudo_paragraph)
	    TtaChangeElementType (child, HTML_EL_Paragraph);
	}
      if (*previous)
	TtaInsertSibling (child, *previous, FALSE, doc);
      else
	TtaInsertFirstChild (&child, cell, doc);
      TtaRegisterElementCreate (child, doc);
      *previous = child;
      child = nextChild;
    }
  /* delete the cell */
  TtaDeleteTree (nextCell, doc);
}

/*----------------------------------------------------------------------
   ChangeColspan
   The value of the colspan attribute has changed from oldspan to newspan
   for the given cell. Add new empty cells (if newspan < oldspan) after
   element cell or merge the following cells (if oldspan > newspan).
  ----------------------------------------------------------------------*/
void ChangeColspan (Element cell, int oldspan, int newspan, Document doc)
{
  Element             table, nextCell, previous, colHead, curColHead, cHead,
                      row, curRow, prevCell;
  ElementType         elType, tableType;
  AttributeType       attrType, rowspanType, colspanType;
  Attribute           attr;
  Document            refDoc;
  char                name[50];
  int                 ncol, i, j, rowspan, curColspan;
  ThotBool            inMath;

  if (oldspan == newspan)
    /* no change */
    return;
  if (cell == NULL)
    return;

  tableType = TtaGetElementType (cell);
  attrType.AttrSSchema = tableType.ElSSchema;
  rowspanType.AttrSSchema = tableType.ElSSchema;
  colspanType.AttrSSchema = tableType.ElSSchema;
  inMath = !TtaSameSSchemas (tableType.ElSSchema,
			     TtaGetSSchema ("HTML", doc));
  if (inMath)
    {
      tableType.ElTypeNum = MathML_EL_MTABLE;
      attrType.AttrTypeNum = MathML_ATTR_MRef_column;
      rowspanType.AttrTypeNum = MathML_ATTR_rowspan_;
      colspanType.AttrTypeNum = MathML_ATTR_columnspan;
    }
  else
    {
      tableType.ElTypeNum = HTML_EL_Table;
      attrType.AttrTypeNum = HTML_ATTR_Ref_column;
      rowspanType.AttrTypeNum = HTML_ATTR_rowspan_;
      colspanType.AttrTypeNum = HTML_ATTR_colspan_;
    }
  table = TtaGetTypedAncestor (cell, tableType);
  row = TtaGetParent (cell);

  /* get the rowspan value of the cell */
  attr = TtaGetAttribute (cell, rowspanType);
  if (attr)
    {
      rowspan = TtaGetAttributeValue (attr);
      if (rowspan < 1)
	rowspan = 1;
    }
  else
    rowspan = 1;
  /* get the column of the cell */
  attr = TtaGetAttribute (cell, attrType);
  if (attr)
    TtaGiveReferenceAttributeValue (attr, &colHead, name, &refDoc);
  else
    return;

  if (newspan > oldspan)
    /* merge with cells in following columns */
    {
      previous = TtaGetLastChild (cell);
      if (inMath)
	/* get the last element contained in the CellWrapper */
	previous = TtaGetLastChild (previous);
      curColHead = colHead;
      ncol = 0;
      while (curColHead && ncol < newspan)
	{
	  ncol++; /* count columns */
	  if (ncol > oldspan)
	    {
	      curRow = row; i = 0;
	      while (curRow && i < rowspan)
		{
		  elType = TtaGetElementType (curRow);
		  if ((!inMath && elType.ElTypeNum == HTML_EL_Table_row) ||
		      (inMath && (elType.ElTypeNum == MathML_EL_MTR ||
				  elType.ElTypeNum == MathML_EL_MLABELEDTR)))
		    {
		      i++;
		      nextCell = GetCellFromColumnHead (curRow, curColHead,
							inMath);
		      if (nextCell)
			{
			  /* get the colspan value of the current cell */
			  attr = TtaGetAttribute (nextCell, colspanType);
			  if (attr)
			    {
			      curColspan = TtaGetAttributeValue (attr);
			      if (curColspan < 1)
				curColspan = 1;
			    }
			  else
			    curColspan = 1;
			  if (curColspan > newspan - ncol + 1)
			    {
			    prevCell = nextCell;
			    cHead = colHead;
			    for (j = 0; j < newspan; j++)
			      TtaNextSibling (&cHead);
			    for (j = 0; j < curColspan-newspan+ncol-1; j++)
			      {
			      prevCell = AddEmptyCellInRow (curRow, cHead,
					  prevCell, FALSE, doc, inMath, FALSE);
			      TtaNextSibling (&cHead);
			      }
			    }
			  MoveCellContents (nextCell, cell, &previous, doc,
					    inMath);
			}
		    }
		  TtaNextSibling (&curRow);
		}
	    }
	  TtaNextSibling (&curColHead);
	}
    }
  else
    /* generate empty cells */
    {
      /* process all rows covered by the cell (due to attribute rowspan) */
      curRow = row; i = 0;
      while (curRow && i < rowspan)
	{
	  elType = TtaGetElementType (curRow);
	  if ((!inMath && elType.ElTypeNum == HTML_EL_Table_row) ||
	      (inMath && (elType.ElTypeNum == MathML_EL_MTR ||
			  elType.ElTypeNum == MathML_EL_MLABELEDTR)))
	    /* this is really a row */
	    {
	      i++;
	      if (curRow == row)
		prevCell = cell;
	      else
		{
		  cHead = colHead;
		  prevCell = NULL;
		  do
		    {
		      TtaPreviousSibling (&cHead);
		      if (cHead)
			prevCell = GetCellFromColumnHead (curRow, cHead,
							  inMath);
		    }
		  while (cHead && !prevCell);
		}
	      cHead = colHead;
	      /* skip the columns covered by the cell with its new colspan */
	      for (j = 0; j < newspan; j++)
		TtaNextSibling (&cHead);
	      /* add new empty cells to fill the space left by reducing the
		 colspan value */
	      for (ncol = 0; ncol < oldspan - newspan; ncol++)
		{
		  prevCell = AddEmptyCellInRow (curRow, cHead, prevCell, FALSE,
						doc, inMath, FALSE);
		  TtaNextSibling (&cHead);
		}
	    }
	  /* next sibling of current row */
	  TtaNextSibling (&curRow);
	}
    }
  SetColExt (cell, newspan, doc, inMath);
  CheckTableAfterCellUpdate = TRUE;
}

/*----------------------------------------------------------------------
   ColspanCreated                                          
  ----------------------------------------------------------------------*/
void ColspanCreated (NotifyAttribute * event)
{
   int                 span;

   span = TtaGetAttributeValue (event->attribute);
   if (span <= 1)
      /* invalid value */
      TtaRemoveAttribute (event->element, event->attribute, event->document);
   else if (event->info != 1)
     ChangeColspan (event->element, 1, span, event->document);
   CheckTableAfterCellUpdate = FALSE;
}


/*----------------------------------------------------------------------
   RegisterColspan                                         
  ----------------------------------------------------------------------*/
ThotBool RegisterColspan (NotifyAttribute * event)
{
   PreviousColSpan = TtaGetAttributeValue (event->attribute);
   return FALSE;		/* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
   ColspanModified                                         
  ----------------------------------------------------------------------*/
void ColspanModified (NotifyAttribute * event)
{
  Element             cell;
  Attribute           attr;
  Document            doc;
  int                 span;

  doc = event->document;
  cell = event->element;
  attr = event->attribute;
  span = TtaGetAttributeValue (attr);
  if (span < 1)
    /* invalid value */
    span = 1;
  if (span != PreviousColSpan && event->info != 1)
    ChangeColspan (cell, PreviousColSpan, span, doc);
  if (span <= 1)
    /* invalid value */
    TtaRemoveAttribute (cell, attr, doc);
}

/*----------------------------------------------------------------------
   ColspanDeleted
  ----------------------------------------------------------------------*/
void ColspanDeleted (NotifyAttribute * event)
{
  if (PreviousColSpan > 1 && event->info != 1)
    ChangeColspan (event->element, PreviousColSpan, 1, event->document);
}

/*----------------------------------------------------------------------
   ChangeRowspan
   The value of the rowspan attribute has changed from oldspan to newspan
   for the given cell. Add new empty cells (if newspan < oldspan) below
   the cell or merge the cells below (if oldspan > newspan).
  ----------------------------------------------------------------------*/
void ChangeRowspan (Element cell, int oldspan, int newspan, Document doc)
{
  Element             table, row, previous, nextCell, colHead, curColHead,
                      prevCell, siblingColhead;
  ElementType         elType, tableType;
  AttributeType       attrType, colspanType;
  Attribute           attr;
  Document            refDoc;
  char                name[50];
  int                 i, nrows, colspan, curColspan;
  ThotBool            inMath, before;

  if (oldspan == newspan)
    /* no change */
    return;
  if (cell == NULL)
    return;

  tableType = TtaGetElementType (cell);
  attrType.AttrSSchema = tableType.ElSSchema;
  colspanType.AttrSSchema = tableType.ElSSchema;
  inMath = !TtaSameSSchemas (tableType.ElSSchema,
			     TtaGetSSchema ("HTML", doc));
  if (inMath)
    {
      tableType.ElTypeNum = MathML_EL_MTABLE;
      attrType.AttrTypeNum = MathML_ATTR_MRef_column;
      colspanType.AttrTypeNum = MathML_ATTR_columnspan;
    }
  else
    {
      tableType.ElTypeNum = HTML_EL_Table;
      attrType.AttrTypeNum = HTML_ATTR_Ref_column;
      colspanType.AttrTypeNum = HTML_ATTR_colspan_;
    }
  table = TtaGetTypedAncestor (cell, tableType);
  row = TtaGetParent (cell);

  /* get the colspan value of the cell */
  attr = TtaGetAttribute (cell, colspanType);
  if (attr)
    {
      colspan = TtaGetAttributeValue (attr);
      if (colspan < 1)
	colspan = 1;
    }
  else
    colspan = 1;
  /* get current column */
  attr = TtaGetAttribute (cell, attrType);
  if (attr)
    TtaGiveReferenceAttributeValue (attr, &colHead, name, &refDoc);
  else
    return;

  nrows = 0;
  if (newspan > oldspan)
    /* merge with following cells */
    {
      previous = TtaGetLastChild (cell);
      if (inMath)
	/* get the last element contained in the CellWrapper */
	previous = TtaGetLastChild (previous);
      while (row && nrows < newspan)
	{
	  elType = TtaGetElementType (row);
	  if ((!inMath && elType.ElTypeNum == HTML_EL_Table_row) ||
	      (inMath && (elType.ElTypeNum == MathML_EL_MTR ||
			  elType.ElTypeNum == MathML_EL_MLABELEDTR)))
	    /* it's a row (skip comments) */
	    {
	      nrows++; /* count rows */
	      if (nrows > oldspan)
		{
		  nextCell = GetCellFromColumnHead (row, colHead, inMath);
		  if (nextCell)
		    {
		      attr = TtaGetAttribute (nextCell, colspanType);
		      if (attr)
			/* this cell has a colspan attribute */
			{
			  curColspan = TtaGetAttributeValue (attr);
			  if (curColspan < 1)
			    curColspan = 1;
			}
		      else
			curColspan = 1;
		      if (curColspan > colspan)
			/* create empty cells */
			{
			  curColHead = colHead;
			  prevCell = nextCell;
			  for (i = 1; i < curColspan; i++)
			    {
			      TtaNextSibling (&curColHead);
			      if (i >= colspan)
				prevCell = AddEmptyCellInRow (row, curColHead,
					  prevCell, FALSE, doc, inMath, FALSE);
			    }
			}
		      MoveCellContents (nextCell, cell, &previous, doc,inMath);
		      if (colspan > curColspan)
			{
			  /* merge more cells from that row */
			  curColHead = colHead;
			  for (i = curColspan; i < colspan; i++)
			    {
			      TtaNextSibling (&curColHead);
			      nextCell = GetCellFromColumnHead (row,
							   curColHead, inMath);
			      if (nextCell)
				MoveCellContents (nextCell, cell, &previous,
						  doc, inMath);
			    }
			}
		    }
		}
	    }
	  TtaNextSibling (&row);
	}
    }
  else if (newspan < oldspan)
    /* generate empty cells */
    {
      while (row && nrows < oldspan)
	{
	  elType = TtaGetElementType (row);
	  if ((!inMath && elType.ElTypeNum == HTML_EL_Table_row) ||
	      (inMath && (elType.ElTypeNum == MathML_EL_MTR ||
			  elType.ElTypeNum == MathML_EL_MLABELEDTR)))
	    /* it's a row (skip comments) */
	    {
	      nrows++;
	      if (nrows > newspan)
		{
		  prevCell = NULL;
		  siblingColhead = colHead;
		  TtaPreviousSibling (&siblingColhead);
		  if (siblingColhead)
		    {
		      prevCell = GetCellFromColumnHead (row, siblingColhead,
							inMath);
		      if (prevCell)
		        before = FALSE;
		    }
		  if (!prevCell)
		    {
		      before = TRUE;
		      siblingColhead = colHead;
		      while (siblingColhead && !prevCell)
			{
			  TtaNextSibling (&siblingColhead);
			  prevCell = GetCellFromColumnHead (row,siblingColhead,
							    inMath);
			}
		    }
		  curColHead = colHead;
		  for (i = 0; i < colspan; i++)
		    {
		      prevCell = AddEmptyCellInRow (row, curColHead, prevCell,
						before, doc, inMath, FALSE);
		      before = FALSE;
		      TtaNextSibling (&curColHead);
		    }
		}
	    }
	  TtaNextSibling (&row);
	}
    }
  SetRowExt (cell, newspan, doc, inMath);
  CheckTableAfterCellUpdate = TRUE;
}

/*----------------------------------------------------------------------
   RowspanCreated                                          
  ----------------------------------------------------------------------*/
void RowspanCreated (NotifyAttribute * event)
{
   int                 span;

   span = TtaGetAttributeValue (event->attribute);
   if (span <= 1)
      /* invalid value */
      TtaRemoveAttribute (event->element, event->attribute, event->document);
   else if (event->info != 1)
      ChangeRowspan (event->element, 1, span, event->document);
}

/*----------------------------------------------------------------------
   RegisterRowspan                                         
  ----------------------------------------------------------------------*/
ThotBool RegisterRowspan (NotifyAttribute * event)
{
   PreviousRowSpan = TtaGetAttributeValue (event->attribute);
   return FALSE;		/* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
   RowspanModified                                         
  ----------------------------------------------------------------------*/
void RowspanModified (NotifyAttribute * event)
{
   Element             cell;
   Attribute           attr;
   Document            doc;
   int                 span;

   doc = event->document;
   cell = event->element;
   attr = event->attribute;
   span = TtaGetAttributeValue (attr);
   if (span < 1)
      /* invalid value */
      span = 1;
   if (span != PreviousRowSpan && event->info != 1)
      ChangeRowspan (cell, PreviousRowSpan, span, doc);
   if (span <= 1)
      /* invalid value */
      TtaRemoveAttribute (cell, attr, doc);
}

/*----------------------------------------------------------------------
   RowspanDeleted
  ----------------------------------------------------------------------*/
void RowspanDeleted (NotifyAttribute * event)
{
   if (PreviousRowSpan > 1 && event->info != 1)
      ChangeRowspan (event->element, PreviousRowSpan, 1, event->document);
}
