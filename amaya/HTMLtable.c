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
static Element      CurrentColumn = NULL;
static Element      CurrentCell;
static int          PreviousColSpan;
static int          PreviousRowSpan;
static ThotBool     NewTable = FALSE;

/*----------------------------------------------------------------------
  GetSiblingRow
  returns the sibling row before or after the cuttent row.
  ----------------------------------------------------------------------*/
static Element GetSiblingRow (Element row, ThotBool before, ThotBool inMath)
{
  ElementType         elType;
  SSchema             rowSS;

  if (row)
    {
      rowSS = TtaGetElementType(row).ElSSchema;
      do
	{
	  if (before)
	    TtaPreviousSibling (&row);
	  else
	    TtaNextSibling (&row);
	  elType = TtaGetElementType (row);
	  if (elType.ElSSchema == rowSS &&
	      (inMath && (elType.ElTypeNum == MathML_EL_MTR ||
			  elType.ElTypeNum == MathML_EL_MLABELEDTR)) ||
	      (!inMath && elType.ElTypeNum == HTML_EL_Table_row))
	    return row;
	} 
      while (row);
    }
  return row;
}

/*----------------------------------------------------------------------
  GetSiblingCell
  returns the sibling cell before or after the cuttent cell.
  ----------------------------------------------------------------------*/
static Element GetSiblingCell (Element cell, ThotBool before, ThotBool inMath)
{
  ElementType         elType;
  SSchema             cellSS;

  if (cell)
    {
      cellSS = TtaGetElementType(cell).ElSSchema;
      do
	{
	  if (before)
	    TtaPreviousSibling (&cell);
	  else
	    TtaNextSibling (&cell);
	  elType = TtaGetElementType (cell);
	  if (elType.ElSSchema == cellSS &&
	      ((inMath && elType.ElTypeNum == MathML_EL_MTD) ||
	       (!inMath && (elType.ElTypeNum == HTML_EL_Data_cell ||
			    elType.ElTypeNum == HTML_EL_Heading_cell))))
	    return cell;
	} 
      while (cell);
    }
  return cell;
}

/*----------------------------------------------------------------------
   SetRowExt
   Set the attribute RowExt of cell "cell" in row "row" according to span.
  ----------------------------------------------------------------------*/
void SetRowExt (Element cell, int span, Document doc, ThotBool inMath)
{
   Element             row, spannedrow, nextspannedrow;
   ElementType         elType;
   AttributeType       attrType;
   Attribute           attr;

   row = TtaGetParent (cell);
   elType = TtaGetElementType (row);
   attrType.AttrSSchema = elType.ElSSchema;
   if (inMath)
     attrType.AttrTypeNum = MathML_ATTR_MRowExt;
   else
     attrType.AttrTypeNum = HTML_ATTR_RowExt;

   if (span == 1 || span < 0)
     /* remove attribute RowExt if it is attached to the cell */
     {
       attr = TtaGetAttribute (cell, attrType);
       if (attr)
	 TtaRemoveAttribute (cell, attr, doc);
     }
   else
     {
       if (span == 0)
	 span = THOT_MAXINT;
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
	       if ((span == 1 || nextspannedrow == NULL) &&
		   spannedrow != row)
		 {
		   attr = TtaGetAttribute (cell, attrType);
		   if (attr == NULL)
		     {
		       attr = TtaNewAttribute (attrType);
		       if (attr != NULL)
			 TtaAttachAttribute (cell, attr, doc);
		     }
		   if (attr)
		     TtaSetAttributeReference (attr, cell, doc, spannedrow,
					       doc);
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
void SetColExt (Element cell, int span, Document doc, ThotBool inMath)
{
  ElementType         elType;
  Element             colHead, nextColHead;
  AttributeType       attrType, refColType;
  Attribute           attr, refColAttr;
  int                 i;
  char                name[50];
  Document            refdoc;

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
  if (span == 1 || span < 0)
    /* remove attribute ColExt */
    {
      attr = TtaGetAttribute (cell, attrType);
      if (attr)
	TtaRemoveAttribute (cell, attr, doc);
    }
  else
    {
      /* set attribute ColExt */
      attr = TtaGetAttribute (cell, attrType);
      if (!attr)
	{
	  attr = TtaNewAttribute (attrType);
	  if (attr)
	    TtaAttachAttribute (cell, attr, doc);
	}
      if (attr)
	{
	  refColAttr = TtaGetAttribute (cell, refColType);
	  if (refColAttr)
	    {
	      TtaGiveReferenceAttributeValue (refColAttr, &colHead, name,
					      &refdoc);
	      if (colHead)
		{
		  if (span == 0)
		    span = THOT_MAXINT;
		  nextColHead = colHead;
		  for (i = 0; i < span && nextColHead; i++)
		    {
		      colHead = nextColHead;
		      TtaNextSibling (&nextColHead);
		    }
		  TtaSetAttributeReference (attr, cell, doc, colHead, doc);
		}
	    }
	}
    }
}

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
   ElementType         elType, cellType;
   AttributeType       attrType;
   Attribute           attr;
   char                name[50];
   Document            refdoc;

   cell = NULL;
   /* if it's not a row, return immediately with NULL */
   elType = TtaGetElementType (row);
   if ((inMath && (elType.ElTypeNum == MathML_EL_MTR ||
		   elType.ElTypeNum == MathML_EL_MLABELEDTR)) ||
       (!inMath && elType.ElTypeNum == HTML_EL_Table_row))
     {
       attrType.AttrSSchema = elType.ElSSchema;
       if (inMath)
	 attrType.AttrTypeNum = MathML_ATTR_MRef_column;
       else
	 attrType.AttrTypeNum = HTML_ATTR_Ref_column;

       cell = TtaGetFirstChild (row);
       found = FALSE;
       while (cell != NULL && !found)
	 {
	   cellType = TtaGetElementType (cell);
	   if (cellType.ElSSchema == elType.ElSSchema &&
	       ((inMath && cellType.ElTypeNum == MathML_EL_MTD) ||
		(!inMath && (cellType.ElTypeNum == HTML_EL_Data_cell ||
			     cellType.ElTypeNum == HTML_EL_Heading_cell))))
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
     }
   return cell;
}

/*----------------------------------------------------------------------
   GetCloseCell
   returns the cell that corresponds to the Column_head element colhead
   or a previous or next colhead in a given row.
   - When the parameter before is TRUE, a previous cell is returned.
     The parameter spanupdate returns TRUE if the previous cell is extended
     (add = TRUE) or reduced (add = FALSE) and the parameter rowspan
     returns its rowspan value (1 by default).
   - When the parameter before is FALSE, the next cell is returned.
  ----------------------------------------------------------------------*/
static Element GetCloseCell (Element row, Element colhead,
			     Document doc, ThotBool before, ThotBool add,
			     ThotBool inMath, ThotBool *spanupdate,
			     int *rowspan)
{
  Element             col, cell;
  ElementType         elType;
  Attribute           attr;
  AttributeType       attrType, attrTypeRef;
  int                 colspan, pos;

  *spanupdate = FALSE;
  *rowspan = 1;
  col = colhead;
  elType = TtaGetElementType (col);
  cell = GetCellFromColumnHead (row, colhead, inMath);
  pos = 0;
  while (!cell && col)
    {
      /* no cell related to this column in this row */
      if (before)
	TtaPreviousSibling (&col);
      else
	TtaNextSibling (&col);
      cell = GetCellFromColumnHead (row, col, inMath);
      pos++;
    }

  if (cell && before)
    {
      /* get the colspan value of the element */
      attrType.AttrSSchema = elType.ElSSchema;
      attrTypeRef.AttrSSchema = elType.ElSSchema;
      if (inMath)
	{
	  attrTypeRef.AttrTypeNum = MathML_ATTR_MColExt;
	  attrType.AttrTypeNum = MathML_ATTR_columnspan;
	}
      else
	{
	  attrTypeRef.AttrTypeNum = HTML_ATTR_ColExt;
	  attrType.AttrTypeNum = HTML_ATTR_colspan_;
	}
      attr = TtaGetAttribute (cell, attrType);
      if (attr)
	{
	  colspan = TtaGetAttributeValue (attr);
	  if (colspan < 0)
	    colspan = 1;
	  if (colspan == 0 ||  /* 0 means infinite span */
	      (add && colspan - pos > 1) ||
	      (!add && colspan - pos >= 1))
	    {
	      if (colspan > 0)
		if (!add && colspan == 2)
		  {
		    colspan--;
		    TtaRegisterAttributeDelete (attr, cell, doc);
		    TtaRemoveAttribute (cell, attr, doc);
		    attr = TtaGetAttribute (cell, attrTypeRef);
		    if (attr)
		      TtaRemoveAttribute (cell, attr, doc);
		  }
		else
		  {
		    if (add)
		      colspan++;
		    else
		      colspan--;
		    if (!add)
		      TtaRegisterAttributeReplace (attr, cell, doc);
		    TtaSetAttributeValue (attr, colspan, cell, doc);
		  }
	      if (!add && colspan > 0 && colspan - pos == 0)
		SetColExt (cell, colspan, doc, inMath);
	      *spanupdate = TRUE;
	      /* check its rowspan attribute */
	      if (inMath)
		attrType.AttrTypeNum = MathML_ATTR_rowspan_;
	      else
		attrType.AttrTypeNum = HTML_ATTR_rowspan_;
	      attr = TtaGetAttribute (cell, attrType);
	      if (attr)
		/* this cell has an attribute rowspan */
		*rowspan = TtaGetAttributeValue (attr);
	      if (*rowspan < 0)
		*rowspan = 1;
	    }
	}
    }
  return (cell);
}

/*----------------------------------------------------------------------
   LinkCellToColumnHead link a cell to the Column_head representing the
   column it belongs to.
  ----------------------------------------------------------------------*/
static void LinkCellToColumnHead (Element cell, Element colhead,
				      Document doc, ThotBool inMath)
{
  ElementType         elType;
  AttributeType       attrType;
  Attribute           attr;

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
      attr = TtaNewAttribute (attrType);
      if (attr)
	TtaAttachAttribute (cell, attr, doc);
    }
  if (attr)
    TtaSetAttributeReference (attr, cell, doc, colhead, doc);
}

/*----------------------------------------------------------------------
   AddEmptyCellInRow add an empty cell element in the given row and relates
   it to the given Column_head colhead.
   The parameter sibling gives the cell which will precede or follow the new
   cell according to the before value.
   withUndo indicate whether the new empty cell must be registered in the
   undo queue or not.
   Return the created empty cell.
  ----------------------------------------------------------------------*/
static Element AddEmptyCellInRow (Element row, Element colhead,
				  Element sibling, ThotBool before,
				  Document doc, ThotBool inMath,
				  ThotBool placeholder, ThotBool withUndo)
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
      LinkCellToColumnHead (lastcell, colhead, doc, inMath);
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
      if (withUndo)
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
	  /* process the row group except the given row */
	  currentrow = GetSiblingRow (row, TRUE, inMath);
	  if (!last && currentrow == NULL)
	    {
	      /* when last is TRUE, only cells of previous rows should be
		 created  */
	      currentrow = GetSiblingRow (row, FALSE, inMath);
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
		child = GetCloseCell (currentrow, prevCol, doc, TRUE,
				      TRUE, inMath, &span, &rowspan);
	      if (child)
		{
		  if (!span)
		    {
		      if (select && row == NULL)
			/* first row where a cell is created */
			row = currentrow;
		      /* add a new cell after */
		      AddEmptyCellInRow (currentrow, colhead, child, FALSE,
					 doc, inMath, FALSE, TRUE);
		    }
		}
	      else
		{
		  /* look for the next cell */
		  child = GetCloseCell (currentrow, nextCol, doc, FALSE,
					TRUE, inMath, &span, &rowspan);
		  if (select && row == NULL)
		    /* first row where a cell is created */
		    row = currentrow;
		  /* add a cell before */
		  AddEmptyCellInRow (currentrow, colhead, child, TRUE, doc,
				     inMath, FALSE, TRUE);
		}
	      if (rowspan == 0)
		rowspan = THOT_MAXINT;
	      while (rowspan >= 1 && currentrow)
		{
		  if (backward)
		    {
		      TtaPreviousSibling (&currentrow);
		      if (currentrow == NULL && !last)
			{
			  /* we have to manage following rows too */
			  currentrow = GetSiblingRow (row, FALSE, inMath);
			  backward = FALSE;
			  rowspan = 1;
			}
		    }
		  else
		    currentrow = GetSiblingRow (currentrow, FALSE, inMath);
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
				child = GetCloseCell (currentrow,
						      prevCol, doc, TRUE,
						      TRUE, inMath,
						      &span, &rowspan);
			      if (child)
				{
				  if (!span)
				    /* add a new cell after */
				    AddEmptyCellInRow (currentrow, colhead,
				       child, FALSE, doc, inMath, FALSE, TRUE);
				}
			      else
				{
				  /* look for the next cell */
				  child = GetCloseCell (currentrow, nextCol,
						     doc, FALSE, TRUE, inMath,
						     &span, &rowspan);
				  /* add before */
				  AddEmptyCellInRow (currentrow, colhead,
					child, TRUE, doc, inMath, FALSE, TRUE);
				}
			      if (rowspan == 0)
				rowspan = THOT_MAXINT;
			      while (rowspan >= 1 && currentrow)
				{
				  currentrow = GetSiblingRow (currentrow,
							      FALSE, inMath);
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
				    child = GetCloseCell (currentrow, prevCol,
						       doc, TRUE, TRUE, inMath,
						       &span, &rowspan);
				  if (child)
				    {
				      if (!span)
					/* add a new cell after */
					AddEmptyCellInRow (currentrow, colhead, child,
							   FALSE, doc, inMath, FALSE, TRUE);
				    }
				  else
				    {
				      /* look for the next cell */
				      child = GetCloseCell (currentrow, 
						    nextCol, doc, FALSE, TRUE,
						    inMath, &span, &rowspan);
				      /* add before */
				      AddEmptyCellInRow (currentrow, colhead, child,
							 TRUE, doc, inMath, FALSE, TRUE);
				    }
				  currentrow = GetSiblingRow (currentrow, FALSE, inMath);
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
  int                 rowType, colspan;
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
		row = GetSiblingRow (row, FALSE, inMath);
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
	  /* the rows group could be thead, tbody, or tfoot */
	  group = TtaGetParent (row);
	  while (row != NULL)
	    {
	      cell = GetCellFromColumnHead (row, colhead, inMath);
	      if (cell != NULL)
		{
		  if (!inMath)
		    {
		      /* look at whether it's a spanning column */
		      attr = TtaGetAttribute (cell, attrType);
		      if (attr)
			{
			  colspan = TtaGetAttributeValue (attr);
			  if (colspan == 0 || colspan > 1)
			    /* colspan = 0 means infinite span */
			    span = TRUE;
			}
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
	      row = GetSiblingRow (row, FALSE, inMath);
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
  int                 span, cRef, cNumber;
  int                 i, rowType;
  ThotBool            inMath;

  if (table == NULL)
    return;
  colElement = (Element*)TtaGetMemory (sizeof (Element) * MAX_COLS);
  colVSpan = (int *)TtaGetMemory (sizeof (int) * MAX_COLS);
  /* store the list of colheads */
  elType = TtaGetElementType (table);
  inMath = TtaSameSSchemas (elType.ElSSchema, TtaGetSSchema ("MathML", doc));
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
  attrType.AttrSSchema = elType.ElSSchema;
  attrTypeHSpan.AttrSSchema = elType.ElSSchema;
  attrTypeVSpan.AttrSSchema = elType.ElSSchema;
  if (inMath)
    {
      attrTypeHSpan.AttrTypeNum = MathML_ATTR_columnspan;
      attrTypeVSpan.AttrTypeNum = MathML_ATTR_rowspan_;
    }
  else
    {
      attrTypeHSpan.AttrTypeNum = HTML_ATTR_colspan_;
      attrTypeVSpan.AttrTypeNum = HTML_ATTR_rowspan_;
    }
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
		if (colVSpan[cRef] < THOT_MAXINT)
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
	      /* link the cell to its corresponding Column_head */
	      LinkCellToColumnHead (cell, colElement[cRef], doc, inMath);

	      /* is there an attribute rowspan for that cell ? */
	      attr = TtaGetAttribute (cell, attrTypeVSpan);
	      if (attr == NULL)
		span = 1;
	      else
		{
		  span = TtaGetAttributeValue (attr);
		  /* Set the attribute RowExt if row span > 1 */
		  if (span > 1)
		    colVSpan[cRef] = span - 1;
		  else if (span == 0)
		    colVSpan[cRef] = THOT_MAXINT;
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
		if (span == 0)
		  /* infinite span */
		  span = THOT_MAXINT;
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
		    /* There are more cells in this row, after the spanning
		       cell. Create additional Column_heads for the spanning
		       cell, except if spanning is "infinite". */
		    {
		    if (span < THOT_MAXINT)
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
	    {
	      if (colVSpan[cRef] < THOT_MAXINT)
		colVSpan[cRef]--;
	    }
	  else
	    cell = AddEmptyCellInRow (row, colElement[cRef], cell, FALSE, doc,
				      inMath, placeholder, TRUE);
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
  Element             newcell, row, colhead, lastColhead;
  ElementType         elType;
  AttributeType       attrType;
  Attribute           attr;
  DisplayMode         dispMode;
  char                ptr[100];
  int                 span, i;
  ThotBool            before, inMath;

  dispMode = TtaGetDisplayMode (doc);
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, DeferredDisplay);

  elType = TtaGetElementType (cell);
  attrType.AttrSSchema = elType.ElSSchema;
  inMath = TtaSameSSchemas (elType.ElSSchema, TtaGetSSchema ("MathML", doc));

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

  /* insert a new column here */
  if (inMath)
    {
      elType.ElTypeNum = MathML_EL_TableRow;
      attrType.AttrTypeNum = MathML_ATTR_MRef_column;
    }
  else
    {
      elType.ElTypeNum = HTML_EL_Table_row;
      attrType.AttrTypeNum = HTML_ATTR_Ref_column;
    }
  /* get the enclosing row element */
  row = TtaGetTypedAncestor (cell, elType);
  /* locate the previous or the next column head */
  colhead = NULL;
  cell = GetSiblingCell (cell, TRUE, inMath);
  if (cell == NULL)
    {
      cell = newcell;
      cell = GetSiblingCell (cell, FALSE, inMath);
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
	      if (span > 1 || span == 0)
		for (i = 1; (i < span || span == 0) && colhead != NULL; i++)
		  {
		    lastColhead = colhead;
		    TtaNextSibling (&colhead);
		  }
	      if (span == 0)
		/* creating a new column after a cell that spans to the
		   end of the table */
		{
		  colhead = lastColhead;
		  before = FALSE;
		}
	    }
	}
    }

  if (colhead)
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
      /* next pasted cells with info = 3 should be linked to this column */
      CurrentColumn = colhead;
      /* link the new cell to the new colhead */
      LinkCellToColumnHead (newcell, colhead, doc, inMath);
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
	CurrentRow = NULL;
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
   Element             cell, row;
   ElementType         elType;
   Document            doc;
   ThotBool            inMath;

  if (event->info != 4 && event->info != 3)
    return;
   cell = event->element;
   doc = event->document;
   row = TtaGetParent (cell);
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
     {
     /* undoing the deletion of any other cell in a "delete column"
	command. Link the restored cell with the corresponding ColumnHead*/
       elType = TtaGetElementType (cell);
       inMath = TtaSameSSchemas (elType.ElSSchema, TtaGetSSchema ("MathML", doc));
       LinkCellToColumnHead (cell, CurrentColumn, doc, inMath);
     }
   HandleColAndRowAlignAttributes (row, doc);
   /* Check attribute NAME or ID in order to make sure that its value */
   /* is unique in the document */
   MakeUniqueName (cell, doc);
}

/*----------------------------------------------------------------------
   SpanningCellForRow
   returns the cell in the column represented by colhead that spans the row
   element. Return NULL if there is no spanning cell in this column for
   that row.
   If a spanning cell is found, parameter colspan returns its colspan value.
  ----------------------------------------------------------------------*/
static Element SpanningCellForRow (Element row, Element colhead,
				   Document doc, ThotBool inMath,
				   ThotBool addRow, int *colspan)
{
  Element             cell;
  ElementType         elType;
  Attribute           attr;
  AttributeType       attrType;
  int                 rowspan, pos;

  *colspan = 1;
  if (row == NULL)
    return NULL;
  elType = TtaGetElementType (row);
  cell = GetCellFromColumnHead (row, colhead, inMath);
  pos = 0;
  while (!cell && row)
    {
      /* no cell related to this column in this row */
      row = GetSiblingRow (row, TRUE, inMath);
      if (row)
	{
	  cell = GetCellFromColumnHead (row, colhead, inMath);
	  pos++;
	}
    }

  if (cell)
    {
      /* check its rowspan attribute */
      attrType.AttrSSchema = elType.ElSSchema;
      if (inMath)
	attrType.AttrTypeNum = MathML_ATTR_rowspan_;
      else
	attrType.AttrTypeNum = HTML_ATTR_rowspan_;
      attr = TtaGetAttribute (cell, attrType);
      if (attr)
	{
	  rowspan = TtaGetAttributeValue (attr);
	  if (rowspan == 0 || rowspan - pos > 1)
	    {
	      if (rowspan != 0)
		/* rowspan = 0 means that the cell spans to the end of the
		   current rows group */
		if (addRow)
		  rowspan++;
		else
		  rowspan--;
	      if (rowspan == 1)
		{
		  /* do not register this deletion of attribute rowspan in the
		     undo queue: undo, when undoing a row creation, would send
		     the event for the attribute before the row is actually
		     removed. RowDeleted called by undo will do the job */
		  if (!addRow)
		    TtaRegisterAttributeDelete (attr, cell, doc);
		  TtaRemoveAttribute (cell, attr, doc);
		  if (inMath)
		    attrType.AttrTypeNum = MathML_ATTR_MRowExt;
		  else
		    attrType.AttrTypeNum = HTML_ATTR_RowExt;
		  attr = TtaGetAttribute (cell, attrType);
		  if (attr)
		    TtaRemoveAttribute (cell, attr, doc);
		}
	      else if (rowspan != 0)
		{
		  /* do not register this change of attribute rowspan in the
		     undo queue: undo, when undoing a row creation, would send
		     the event for the attribute before the row is actually
		     removed. RowDeleted called by undo will do the job */
		  if (!addRow)
		    TtaRegisterAttributeReplace (attr, cell, doc);
		  TtaSetAttributeValue (attr, rowspan, cell, doc);
		}
	      /* get the colspan value of the element */
	      if (inMath)
		attrType.AttrTypeNum = MathML_ATTR_columnspan;
	      else
		attrType.AttrTypeNum = HTML_ATTR_colspan_;
	      attr = TtaGetAttribute (cell, attrType);
	      if (attr)
		/* this cell has an attribute rowspan */
		*colspan = TtaGetAttributeValue (attr);
	      if (*colspan < 0)
		*colspan = 1;
	    }
	  else
	    cell = NULL;
	}
      else
	cell = NULL;
    }
  return (cell);
}

/*----------------------------------------------------------------------
   UpdateRowspanForRow
   A new row has been created by the user (if addRow) or will be deleted
   (if not addRow). Take care of the rowspan attribute of the previous rows
   for creating cells in the row if it's a new row,
   or create empty cells in the next rows according of the rowspan attribute
   of the deleted row.
  ----------------------------------------------------------------------*/
static void UpdateRowspanForRow (Element row, Document doc, ThotBool inMath,
				 ThotBool addRow)
{
  Element             colhead, cell, prev, prevRow, table;
  ElementType         elType;
  int                 colspan, i;

  elType = TtaGetElementType (row);
  if (inMath)
    elType.ElTypeNum = MathML_EL_MTABLE;
  else
    elType.ElTypeNum = HTML_EL_Table;
  table = TtaGetTypedAncestor (row, elType);
  /* get the first column */
  if (inMath)
    elType.ElTypeNum = MathML_EL_MColumn_head;
  else
    elType.ElTypeNum = HTML_EL_Column_head;
  colhead = TtaSearchTypedElement (elType, SearchForward, table);
  /* get the previous row, if any */
  prevRow = GetSiblingRow (row, TRUE, inMath);
  /* check the cells for all columns */
  cell = NULL;
  while (colhead)
    {
      /* is there a cell in a row above that spans the current row? */
      prev = SpanningCellForRow (prevRow, colhead, doc, inMath, addRow,
				 &colspan);
      if (addRow && !prev)
	/* we are creating a new row and no cell from a row above is
           covering the current cell position. Create an empty cell at
	   that position */
	cell = AddEmptyCellInRow (row, colhead, cell, FALSE, doc, inMath,
				  FALSE, FALSE);
      else if (!addRow)
	/* we are deleting a row */
	{
	  if (prev)
	    /* a cell from a row above is covering the current cell position.
	       Create an empty cell to allow this row to be pasted correctly
	       later */
	    {
	      if (colspan > 0)
		for (i = 1; i <= colspan; i++)
		  cell = AddEmptyCellInRow (row, colhead, cell, FALSE, doc,
					    inMath, FALSE, TRUE);
	    }
	  else
	    cell = GetCellFromColumnHead (row, colhead, inMath);
	}
      if (colspan == 0)
	colhead = NULL;
      else
	while (colspan >= 1 && colhead)
	  {
	    TtaNextSibling (&colhead);
	    colspan--;
	  }
    }
}

/*----------------------------------------------------------------------
   DeleteRow
   A table row will be deleted by the user
  ----------------------------------------------------------------------*/
ThotBool DeleteRow (NotifyElement *event)
{
  Element             row, cell;
  ElementType         elType, cellType;
  Attribute           attr;
  AttributeType       rowspanType;
  Document            doc;
  int                 rowspan;
  ThotBool            inMath;

  row = event->element;
  doc = event->document;
  elType = TtaGetElementType (row);
  inMath = TtaSameSSchemas (elType.ElSSchema, TtaGetSSchema ("MathML", doc));
  /* decrease the value of the rowspan attribute of cells from rows above
     that span the deleted row */
  UpdateRowspanForRow (row, doc, inMath, FALSE);
  /* create empty cells in the following rows, where cells of the deleted
     row span the following rows */
  rowspanType.AttrSSchema = elType.ElSSchema;
  if (inMath)
    rowspanType.AttrTypeNum = MathML_ATTR_rowspan_;
  else
    rowspanType.AttrTypeNum = HTML_ATTR_rowspan_;
  cell = TtaGetFirstChild (row);
  while (cell)
    {
      cellType = TtaGetElementType (cell);
      if (cellType.ElSSchema == elType.ElSSchema &&
	  ((inMath && cellType.ElTypeNum == MathML_EL_MTD) ||
	   (!inMath && (cellType.ElTypeNum == HTML_EL_Data_cell ||
			cellType.ElTypeNum == HTML_EL_Heading_cell))))
	/* it's really a cell */
	{
	  /* check its rowspan attribute */
	  attr = TtaGetAttribute (cell, rowspanType);
	  if (attr)
	    /* this cell has an attribute rowspan */
	    {
	      rowspan = TtaGetAttributeValue (attr);
	      if (rowspan < 0)
		rowspan = 1;
	    }
	  else
	    rowspan = 1;
	  if (rowspan > 1 || rowspan == 0)
	    {
	      ChangeRowspan (cell, rowspan, 1, doc);
	      TtaRegisterAttributeDelete (attr, cell, doc);
	      TtaRemoveAttribute (cell, attr, doc);
	    }
	}
      TtaNextSibling (&cell);
    }
  return FALSE;		/* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
   RowDeleted
   A table row has been deleted by the user
  ----------------------------------------------------------------------*/
void RowDeleted (NotifyElement *event)
{
  Element             rowgroup;
  Document            doc;

  doc = event->document;
  rowgroup = event->element;
  if (rowgroup == NULL)
    /* the rowgroup doesn't exist */
    return;
  HandleColAndRowAlignAttributes (rowgroup, doc);
}

/*----------------------------------------------------------------------
  DeleteColumn
  A column will be deleted by the user.
  ----------------------------------------------------------------------*/
ThotBool DeleteColumn (NotifyElement * event)
{
  Element             cell, prevCell, row, colhead, prev;
  Document            doc;
  ElementType         elType;
  Attribute           attr;
  AttributeType       attrTypeC, attrTypeR;
  int                 rowspan, colspan, rs;
  ThotBool            span, inMath;

  /* if called by undo/redo, don't do anything */
  if (event->info != 1)
    {
      colhead = event->element;
      doc = event->document;
      elType = TtaGetElementType (colhead);
      attrTypeC.AttrSSchema = elType.ElSSchema;
      attrTypeR.AttrSSchema = elType.ElSSchema;
      inMath = TtaSameSSchemas (elType.ElSSchema, TtaGetSSchema("MathML",doc));
      if (inMath)
	{
	  elType.ElTypeNum = MathML_EL_TableRow;
	  attrTypeC.AttrTypeNum = MathML_ATTR_columnspan;
	  attrTypeR.AttrTypeNum = MathML_ATTR_rowspan_;
	}
      else
	{
	  elType.ElTypeNum = HTML_EL_Table_row;
	  attrTypeC.AttrTypeNum = HTML_ATTR_colspan_;
	  attrTypeR.AttrTypeNum = HTML_ATTR_rowspan_;
	}
      /* get the first row in the table */
      row = TtaSearchTypedElement (elType, SearchForward, colhead);
      while (row)
	{
	  /* check if the cell has span values */
	  rowspan = 1;
	  colspan = 1;
	  cell = GetCellFromColumnHead (row, colhead, inMath);
	  if (cell)
	    {
	      attr = TtaGetAttribute (cell, attrTypeC);
	      if (attr)
		/* there is a colspan attribute */
		colspan = TtaGetAttributeValue (attr);
	      if (colspan > 1 || colspan == 0)
		{
		  ChangeColspan (cell, colspan, 1, doc);
		  TtaRegisterAttributeDelete (attr, cell, doc);
		  TtaRemoveAttribute (cell, attr, doc);
		}
	      attr = TtaGetAttribute (cell, attrTypeR);
	      if (attr)
		/* there is a rowspan */
		{
		  rowspan = TtaGetAttributeValue (attr);
		  if (rowspan < 0)
		    rowspan = 1;
		}
	    }
	  else
	    /* there is no cell for the column in that row. Create an empty
	       cell to allow the column to be pasted correctly later */
	    {
	      prevCell = GetCloseCell (row, colhead, doc, TRUE, FALSE, inMath,
				       &span, &rowspan);
	    }
	  prev = row;
	  if (rowspan == 0)
	    rowspan = THOT_MAXINT;
	  while (rowspan >= 1 && row)
	    {
	      if (!cell)
		/* there is no cell for the column in that row. Create an empty
		   cell to allow the column to be pasted correctly later */
		AddEmptyCellInRow (row, colhead, prevCell, FALSE, doc,
				   inMath, FALSE, TRUE);
	      row = GetSiblingRow (row, FALSE, inMath);
	      if (row)
		{
		  if (rowspan < THOT_MAXINT)
		    rowspan--;
		  if (!cell && rowspan >= 1)
		    prevCell = GetCloseCell (row, colhead, doc, TRUE, FALSE,
					     inMath, &span, &rs);
		}
	    }
	  if (row == NULL)
	    {
	      prev = TtaGetParent (prev);
	      while (prev && row == NULL)
		{
		  TtaNextSibling (&prev);
		  row = TtaSearchTypedElementInTree (elType, SearchForward,
						     prev, prev);
		}
	    }
	}
    }
  return FALSE;		/* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
  ColumnDeleted                                             
  ----------------------------------------------------------------------*/
void ColumnDeleted (NotifyElement * event)
{
}

/*----------------------------------------------------------------------
  ColumnPasted                                             
  ----------------------------------------------------------------------*/
void ColumnPasted (NotifyElement * event)
{
  CurrentColumn = event->element;
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
void RowCreated (NotifyElement *event)
{
  Element             row, table;
  Element             cell;
  ElementType         elType;
  Document            doc;
  ThotBool            inMath;

  row = event->element;
  doc = event->document;
  elType = TtaGetElementType (row);
  inMath = TtaSameSSchemas (elType.ElSSchema, TtaGetSSchema ("MathML", doc));
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
      /* avoid processing the pasted cells in the created row */
      CurrentRow = row;
      /* remove the cell created by the editor */
      cell = TtaGetFirstChild (row);
      if (cell)
	TtaDeleteTree (cell, doc);
      UpdateRowspanForRow (row, doc, inMath, TRUE);
    }
  HandleColAndRowAlignAttributes (row, doc);
  CurrentCell = TtaGetLastChild (row);
}

/*----------------------------------------------------------------------
   RowPasted                                               
  ----------------------------------------------------------------------*/
void RowPasted (NotifyElement * event)
{
  Element             row, prevRow, table, colhead, prevCell, cell, nextCell,
                      prev;
  ElementType         elType, cellType;
  Attribute           attr;
  AttributeType       rowspanType, rowextType, colspanType;
  Document            doc;
  int                 colspan, span;
  ThotBool            inMath;

  row = event->element;
  doc = event->document;
  elType = TtaGetElementType (row);
  inMath = TtaSameSSchemas (elType.ElSSchema, TtaGetSSchema ("MathML", doc));
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
      /* link each cell to the appropriate column head and add empty cells
         if the pasted row is too short */
      rowspanType.AttrSSchema = elType.ElSSchema;
      rowextType.AttrSSchema = elType.ElSSchema;
      colspanType.AttrSSchema = elType.ElSSchema;
      if (inMath)
	{
	  elType.ElTypeNum = MathML_EL_MColumn_head;
	  rowspanType.AttrTypeNum = MathML_ATTR_rowspan_;
	  rowextType.AttrTypeNum = MathML_ATTR_MRowExt;
	  colspanType.AttrTypeNum = MathML_ATTR_columnspan;
	}
      else
	{
	  elType.ElTypeNum = HTML_EL_Column_head;
	  rowspanType.AttrTypeNum = HTML_ATTR_rowspan_;
	  rowextType.AttrTypeNum = HTML_ATTR_RowExt;
	  colspanType.AttrTypeNum = HTML_ATTR_colspan_;
	}
      /* get the first column */
      colhead = TtaSearchTypedElement (elType, SearchForward, table);
      /* get the previous row, if any */
      prevRow = GetSiblingRow (row, TRUE, inMath);
      prevCell = NULL;
      /* get the first cell in the pasted row */
      cell = TtaGetFirstChild (row);
      cellType = TtaGetElementType (cell);
      if (cellType.ElSSchema != elType.ElSSchema ||
	  (inMath && elType.ElTypeNum != MathML_EL_MTD) ||
	  (!inMath && (cellType.ElTypeNum != HTML_EL_Data_cell &&
	               cellType.ElTypeNum != HTML_EL_Heading_cell)))
	cell = GetSiblingCell (cell, FALSE, inMath);
      while (colhead)
	{
	  nextCell = GetSiblingCell (cell, FALSE, inMath);
	  /* handle the colspan attribute */
	  span = 1;
	  if (cell)
	    {
	      attr = TtaGetAttribute (cell, colspanType);
	      if (attr)
		{
		  span = TtaGetAttributeValue (attr);
		  if (span < 0)
		    span = 1;
		}
	    }
	  /* is there a cell in a row above that spans this row? */
	  prev = SpanningCellForRow (prevRow, colhead, doc, inMath, TRUE,
				     &colspan);
	  if (!prev)
	    /* no cell from a row above is covering the current cell position*/
	    {
	      if (cell)
		{
		  LinkCellToColumnHead (cell, colhead, doc, inMath);
		  /* remove the rowspan attribute, except if we are undoing
		     an operation */
		  if (event->info != 1)
		    /******** do not remove attr rowspan if we are pasting
		    this row as part of the pasting of a whole table *********/
		    {
		      attr = TtaGetAttribute (cell, rowspanType);
		      if (attr)
			TtaRemoveAttribute (cell, attr, doc);
		      attr = TtaGetAttribute (cell, rowextType);
		      if (attr)
			TtaRemoveAttribute (cell, attr, doc);
		    }
		  if (span > 1 || span == 0)
		    SetColExt (cell, span, doc, inMath);
		}
	      else
		/* the pasted row has no cell for this column.
		   Add an empty cell */
		cell = AddEmptyCellInRow (row, colhead, prevCell, FALSE, doc,
					  inMath, FALSE, FALSE);
	    }
	  else
	    /* this position is taken by a cell from a row above */
	    if (cell)
	      /* the pasted row has a cell at this position. Remove it */
	      {
		TtaDeleteTree (cell, doc);
	        cell = NULL;
	      }
	  /* get the next column where there is a free slot for the pasted
	     row */
          if (colspan == 0)
	    colspan = THOT_MAXINT;
          if (span == 0)
	    span = THOT_MAXINT;
	  while ((colspan >= 1 || span >= 1) && colhead)
	    {
	      TtaNextSibling (&colhead);
	      span--;
	      if (span == 0)
		{
		  if (cell)
		    prevCell = cell;
		  cell = nextCell;
		}
	      colspan--;
	      if (colspan > 0)
		{
		  nextCell = GetSiblingCell (cell, FALSE, inMath);
		  if (cell)
		    TtaDeleteTree (cell, doc);
		}
	    }
	}
      if (nextCell)
	/* we have checked all columns of the table and there are extra cells.
	   Remove them */
	while (nextCell)
	  {
	    cell = nextCell;
	    nextCell = GetSiblingCell (cell, FALSE, inMath);
	    TtaDeleteTree (cell, doc);
	  }
    }
  HandleColAndRowAlignAttributes (row, doc);
  /* avoid processing the cells of the created row */
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
  /* move the contents of this cell to the cell whose attribute colspan
     has changed */
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
  ElementType         tableType;
  AttributeType       attrType, rowspanType, colspanType;
  Attribute           attr;
  Document            refDoc;
  char                name[50];
  int                 ncol, nrow, i, j, rowspan, curColspan, curRowspan;
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
  inMath = TtaSameSSchemas (tableType.ElSSchema, TtaGetSSchema ("MathML",doc));
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
      if (rowspan < 0)
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

  if ((newspan > oldspan && oldspan > 0) || newspan == 0)
    /* merge with cells in following columns */
    {
      previous = TtaGetLastChild (cell);
      if (inMath)
	/* get the last element contained in the CellWrapper */
	previous = TtaGetLastChild (previous);
      curColHead = colHead;
      ncol = 0;
      while (curColHead && (ncol < newspan || newspan == 0))
	{
	  ncol++; /* count columns */
	  if (ncol > oldspan)
	    {
	      curRow = row; nrow = 0;
	      while (curRow && (nrow < rowspan || rowspan == 0))
		{
		  nrow++;
		  nextCell = GetCellFromColumnHead (curRow, curColHead,inMath);
		  if (nextCell)
		    {
		      /* get the colspan value of the current cell */
		      attr = TtaGetAttribute (nextCell, colspanType);
		      if (attr)
			{
			  curColspan = TtaGetAttributeValue (attr);
			  if (curColspan < 0)
			    curColspan = 1;
			}
		      else
			curColspan = 1;
		      if (newspan > 0 &&
			  (curColspan > newspan - ncol + 1 || curColspan == 0))
			{
			  prevCell = nextCell;
			  cHead = colHead;
			  for (j = 0; j < newspan && cHead; j++)
			    TtaNextSibling (&cHead);
			  for (j = 0; j < curColspan-newspan+ncol-1 && cHead;
			       j++)
			    {
			      prevCell = AddEmptyCellInRow (curRow, cHead,
				    prevCell, FALSE, doc, inMath, FALSE, TRUE);
			      TtaNextSibling (&cHead);
			    }
			}
		      attr = TtaGetAttribute (nextCell, rowspanType);
		      if (attr)
			/* this cell has a rowspan attribute */
			{
			  curRowspan = TtaGetAttributeValue (attr);
			  if (curRowspan < 0)
			    curRowspan = 1;
			}
		      else
			curRowspan = 1;
		      if ((curRowspan == 0 || nrow + curRowspan -1 > rowspan)&&
			  rowspan > 0)
			ChangeRowspan (nextCell, curRowspan,
				       rowspan - nrow + 1, doc);
		      MoveCellContents (nextCell, cell, &previous, doc,inMath);
		    }
		  curRow = GetSiblingRow (curRow, FALSE, inMath);
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
      while (curRow && (i < rowspan || rowspan == 0))
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
	  for (j = 0; j < newspan && cHead; j++)
	    TtaNextSibling (&cHead);
	  /* add new empty cells to fill the space left by reducing the
	     colspan value */
	  for (ncol = 0; (oldspan == 0 || ncol < oldspan - newspan) && cHead;
	       ncol++)
	    {
	      prevCell = AddEmptyCellInRow (curRow, cHead, prevCell, FALSE,
					    doc, inMath, FALSE, TRUE);
	      TtaNextSibling (&cHead);
	    }
	  /* next sibling of current row */
	  curRow = GetSiblingRow (curRow, FALSE, inMath);
	}
    }
}

/*----------------------------------------------------------------------
   ColspanCreated                                          
  ----------------------------------------------------------------------*/
void ColspanCreated (NotifyAttribute * event)
{
  ElementType         elType;
  int                 span;
  ThotBool            inMath;

  span = TtaGetAttributeValue (event->attribute);
  if (span < 0 || span == 1)
    /* invalid value */
    TtaRemoveAttribute (event->element, event->attribute, event->document);
  else
    {
      /* for undo operations just restore the col extension */
      if (event->info != 1)
	ChangeColspan (event->element, 1, span, event->document);
      elType = TtaGetElementType (event->element);
      inMath = TtaSameSSchemas (elType.ElSSchema,
				TtaGetSSchema ("MathML", event->document));
      SetColExt (event->element, span, event->document, inMath);
    }
}

/*----------------------------------------------------------------------
   RegisterColspan                                         
  ----------------------------------------------------------------------*/
ThotBool RegisterColspan (NotifyAttribute * event)
{
  PreviousColSpan = TtaGetAttributeValue (event->attribute);
  if (PreviousColSpan < 0)
    PreviousColSpan = 1;
  return FALSE;		/* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
   ColspanModified                                         
  ----------------------------------------------------------------------*/
void ColspanModified (NotifyAttribute * event)
{
  Element             cell;
  ElementType         elType;
  Attribute           attr;
  Document            doc;
  int                 span;
  ThotBool            inMath;

  doc = event->document;
  cell = event->element;
  attr = event->attribute;
  span = TtaGetAttributeValue (attr);
  if (span < 0)
    /* invalid value */
    span = 1;
  if (span != PreviousColSpan)
    {
      /* for undo operations just restore the col extension */
      if (event->info != 1)
	ChangeColspan (cell, PreviousColSpan, span, doc);
      elType = TtaGetElementType (cell);
      inMath = TtaSameSSchemas (elType.ElSSchema,
				TtaGetSSchema ("MathML", doc));
      SetColExt (cell, span, doc, inMath);
    }
  if (span == 1)
    /* invalid value */
    TtaRemoveAttribute (cell, attr, doc);
}

/*----------------------------------------------------------------------
   ColspanDeleted
  ----------------------------------------------------------------------*/
void ColspanDeleted (NotifyAttribute * event)
{
  ElementType         elType;
  ThotBool            inMath;

  if (PreviousColSpan != 1)
    {
      /* for undo operations just restore the col extension */
      if (event->info != 1)
	ChangeColspan (event->element, PreviousColSpan, 1, event->document);
      elType = TtaGetElementType (event->element);
      inMath = TtaSameSSchemas (elType.ElSSchema,
				TtaGetSSchema ("MathML", event->document));
      SetColExt (event->element, 1, event->document, inMath);
    }
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
  ElementType         tableType;
  AttributeType       attrType, colspanType, rowspanType;
  Attribute           attr;
  Document            refDoc;
  char                name[50];
  int                 i, nrows, colspan, curColspan, curRowspan;
  ThotBool            inMath, before;

  if (oldspan == newspan)
    /* no change */
    return;
  if (cell == NULL)
    return;

  tableType = TtaGetElementType (cell);
  attrType.AttrSSchema = tableType.ElSSchema;
  colspanType.AttrSSchema = tableType.ElSSchema;
  rowspanType.AttrSSchema = tableType.ElSSchema;
  inMath = TtaSameSSchemas (tableType.ElSSchema, TtaGetSSchema ("MathML",doc));
  if (inMath)
    {
      tableType.ElTypeNum = MathML_EL_MTABLE;
      attrType.AttrTypeNum = MathML_ATTR_MRef_column;
      colspanType.AttrTypeNum = MathML_ATTR_columnspan;
      rowspanType.AttrTypeNum = MathML_ATTR_rowspan_;
    }
  else
    {
      tableType.ElTypeNum = HTML_EL_Table;
      attrType.AttrTypeNum = HTML_ATTR_Ref_column;
      colspanType.AttrTypeNum = HTML_ATTR_colspan_;
      rowspanType.AttrTypeNum = HTML_ATTR_rowspan_;
    }
  table = TtaGetTypedAncestor (cell, tableType);
  row = TtaGetParent (cell);

  /* get the colspan value of the cell */
  attr = TtaGetAttribute (cell, colspanType);
  if (attr)
    {
      colspan = TtaGetAttributeValue (attr);
      if (colspan < 0)
	colspan = 1;
    }
  else
    colspan = 1;
  /* get the column of the cell */
  attr = TtaGetAttribute (cell, attrType);
  if (attr)
    TtaGiveReferenceAttributeValue (attr, &colHead, name, &refDoc);
  else
    return;

  nrows = 0;
  if ((newspan > oldspan && oldspan > 0) || newspan == 0)
    /* merge with following cells */
    {
      previous = TtaGetLastChild (cell);
      if (inMath)
	/* get the last element contained in the CellWrapper */
	previous = TtaGetLastChild (previous);
      while (row && (newspan == 0 || nrows < newspan))
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
		      if (curColspan < 0)
			curColspan = 1;
		      else if (curColspan == 0)
			curColspan = THOT_MAXINT;
		    }
		  else
		    curColspan = 1;
		  curColHead = colHead;
		  if (curColspan > colspan && colspan > 0)
		    /* create empty cells */
		    {
		      prevCell = nextCell;
		      for (i = 1; i < curColspan && curColHead; i++)
			{
			  TtaNextSibling (&curColHead);
			  if (i >= colspan && curColHead)
			    prevCell = AddEmptyCellInRow (row, curColHead,
							  prevCell, FALSE, doc,
							  inMath, FALSE, TRUE);
			}
		    }
		  else if (colspan > 0)
		    /* merge cells from that row */
		    for (i = curColspan; i <= colspan && curColHead; i++)
		      {
			if (nextCell)
			  {
			    attr = TtaGetAttribute (nextCell, rowspanType);
			    if (attr)
			      /* this cell has a rowspan attribute */
			      {
				curRowspan = TtaGetAttributeValue (attr);
				if (curRowspan < 0)
				  curRowspan = 1;
			      }
			    else
			      curRowspan = 1;
			    if ((curRowspan == 0 ||
				 nrows + curRowspan - 1 > newspan) &&
				newspan>0)
			      /* this cell extends beyound the limit of the
				 extension of the cell of interest. Fill the
			         holes with empty cells */
			      ChangeRowspan (nextCell, curRowspan,
					     newspan - nrows + 1, doc);
			    MoveCellContents (nextCell, cell, &previous, doc,
					      inMath);
			  }
			TtaNextSibling (&curColHead);
			nextCell = GetCellFromColumnHead (row, curColHead,
							  inMath);
		      }
		}
	    }
	  row = GetSiblingRow (row, FALSE, inMath);
	}
    }
  else if ((newspan < oldspan && newspan > 0) || oldspan == 0)
    /* generate empty cells */
    {
      while (row && (nrows < oldspan || oldspan == 0))
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
	      for (i = 0; (i < colspan || colspan == 0) && curColHead; i++)
		{
		  prevCell = AddEmptyCellInRow (row, curColHead, prevCell,
					     before, doc, inMath, FALSE, TRUE);
		  before = FALSE;
		  TtaNextSibling (&curColHead);
		}
	    }
	  row = GetSiblingRow (row, FALSE, inMath);
	}
    }
}

/*----------------------------------------------------------------------
   RowspanCreated                                          
  ----------------------------------------------------------------------*/
void RowspanCreated (NotifyAttribute * event)
{
  ElementType         elType;
  int                 span;
  ThotBool            inMath;

  span = TtaGetAttributeValue (event->attribute);
  if (span < 0 || span == 1)
    /* invalid value */
    TtaRemoveAttribute (event->element, event->attribute, event->document);
  else 
    {
      /* for undo operations just restore the row extension */
      if (event->info != 1)
	ChangeRowspan (event->element, 1, span, event->document);
      elType = TtaGetElementType (event->element);
      inMath = TtaSameSSchemas (elType.ElSSchema,
				TtaGetSSchema ("MathML", event->document));
      SetRowExt (event->element, span, event->document, inMath);
    }
}

/*----------------------------------------------------------------------
   RegisterRowspan                                         
  ----------------------------------------------------------------------*/
ThotBool RegisterRowspan (NotifyAttribute * event)
{
  PreviousRowSpan = TtaGetAttributeValue (event->attribute);
  if (PreviousRowSpan < 0)
    PreviousRowSpan = 1;
  return FALSE;		/* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
   RowspanModified                                         
  ----------------------------------------------------------------------*/
void RowspanModified (NotifyAttribute * event)
{
  Element             cell;
  ElementType         elType;
  Attribute           attr;
  Document            doc;
  int                 span;
  ThotBool            inMath;

  doc = event->document;
  cell = event->element;
  attr = event->attribute;
  span = TtaGetAttributeValue (attr);
  if (span < 0)
    /* invalid value */
    span = 1;
  if (span != PreviousRowSpan)
    {
      /* for undo operations just restore the row extension */
      if (event->info != 1)
	ChangeRowspan (cell, PreviousRowSpan, span, doc);
      elType = TtaGetElementType (cell);
      inMath = TtaSameSSchemas (elType.ElSSchema,
				TtaGetSSchema ("MathML", doc));
      SetRowExt (cell, span, doc, inMath);
    }
  if (span < 0 || span == 1)
    /* invalid value */
    TtaRemoveAttribute (cell, attr, doc);
}

/*----------------------------------------------------------------------
   RowspanDeleted
  ----------------------------------------------------------------------*/
void RowspanDeleted (NotifyAttribute * event)
{
  ElementType         elType;
  ThotBool            inMath;

  if (PreviousRowSpan > 1 || PreviousRowSpan == 0)
    {
      /* for undo operations just restore the col extension */
      if (event->info != 1)
	ChangeRowspan (event->element, PreviousRowSpan, 1, event->document);
      elType = TtaGetElementType (event->element);
      inMath = TtaSameSSchemas (elType.ElSSchema,
				TtaGetSSchema ("MathML", event->document));
      SetRowExt (event->element, 1, event->document, inMath);
    }
}
