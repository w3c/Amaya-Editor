/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
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
#include "undo.h"
#include "MathML.h"
#include "MathMLbuilder_f.h"

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
#ifdef __STDC__
static Element      GetCellFromColumnHead (Element row, Element colhead, ThotBool inMath)
#else
static Element      GetCellFromColumnHead (row, colhead, inMath)
Element             row;
Element             colhead;
ThotBool            inMath;
#endif
{
   Element             cell, currentcolhead;
   ThotBool            found;
   ElementType         elType;
   AttributeType       attrType;
   Attribute           attr;
   CHAR_T                name[50];
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
   GetCloseCellFromColumnHead

   returns the cell that corresponds to the Column_head element colhead
   or a previous or next colhead in a given row.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static Element      GetCloseCellFromColumnHead (Element row, Element colhead, ThotBool before, ThotBool inMath)
#else
static Element      GetCloseCellFromColumnHead (row, colhead, before, inMath)
Element             row;
Element             colhead;
ThotBool            before;
ThotBool            inMath;
#endif
{
  Element             col, child;
  ElementType         elType;

  col = colhead;
  elType = TtaGetElementType (col);
  child = GetCellFromColumnHead (row, colhead, inMath);
  while (!child && col)
    {
      /* no cell related to this column in this row */
      if (before)
	col = TtaSearchTypedElement (elType, SearchForward, col);
      else
	col = TtaSearchTypedElement (elType, SearchBackward, col);
      child = GetCellFromColumnHead (row, col, inMath);
    }
  return (child);
}


/*----------------------------------------------------------------------
   RelateCellWithColumnHead relates a cell with a Column_head.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         RelateCellWithColumnHead (Element cell, Element colhead, Document doc, ThotBool inMath)
#else
static void         RelateCellWithColumnHead (cell, colhead, doc, inMath)
Element             cell;
Element             colhead;
Document            doc;
ThotBool            inMath;
#endif
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
      if (attr != NULL)
	TtaAttachAttribute (cell, attr, doc);
    }
  if (attr != NULL)
    TtaSetAttributeReference (attr, cell, doc, colhead, doc);
}

/*----------------------------------------------------------------------
   AddEmptyCellInRow add empty cell element in the given row and relates
   it to the given Column_head colhead.
   The parameter sibling gives the cell which will precede or follow the newl
   cell according to the before value.
   Return the created empty cell.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static Element      AddEmptyCellInRow (Element row, Element colhead, Element sibling, ThotBool before, Document doc, ThotBool inMath)
#else
static Element      AddEmptyCellInRow (row, colhead, sibling, before, doc, inMath)
Element             row;
Element             colhead;
Element             sibling;
ThotBool            before;
Document            doc;
ThotBool            inMath;
#endif
{
  Element             lastcell;
  ElementType         elType;

  lastcell = NULL;
  if (row == NULL)
    return (NULL);

  elType = TtaGetElementType (row);
  if (sibling != NULL)
    {
      if (inMath)
	elType.ElTypeNum = MathML_EL_MTD;
      else
	elType.ElTypeNum = HTML_EL_Data_cell;
      lastcell = TtaNewTree (doc, elType, "");
      if (lastcell != NULL)
	{
	  TtaInsertSibling (lastcell, sibling, before, doc);
	  RelateCellWithColumnHead (lastcell, colhead, doc, inMath);
	}
    }
  return (lastcell);
}

/*----------------------------------------------------------------------
  NewColumnHead creates a new Column_head and returns it.   
  Creates an additional empty cell in all rows, except the row indicated.
  If last == TRUE when lastcolhead is the last current column.
  The parameter before indicates if the lastcolhead precedes or follows
  the new created Column_head. It should be FALSE when last is TRUE.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static Element      NewColumnHead (Element lastcolhead, ThotBool before, ThotBool last, Element row, Document doc, ThotBool inMath)
#else
static Element      NewColumnHead (lastcolhead, before, last, row, doc, inMath)
Element             lastcolhead;
ThotBool            before;
ThotBool            last;
Element             row;
Document            doc;
ThotBool            inMath;
#endif
{
   Element             colhead, currentrow;
   Element             group, groupdone;
   Element             table, child;
   ElementType         elType;

   if (lastcolhead == NULL)
      return NULL;
   elType = TtaGetElementType (lastcolhead);
   colhead = TtaNewTree (doc, elType, "");
   if (colhead != NULL)
     {
	TtaInsertSibling (colhead, lastcolhead, before, doc);
	/* add empty cells to all other rows */
	/* process the row group that contains the row which doesn't */
	/* need to be processed */
	currentrow = row;
	TtaPreviousSibling (&currentrow);
	while (currentrow != NULL)
	  {
	    /* get the sibling cell */
	    if (last)
	      child = TtaGetLastChild (currentrow);
	    else
	      child = GetCloseCellFromColumnHead (currentrow, lastcolhead, before, inMath);
	    AddEmptyCellInRow (currentrow, colhead, child, before, doc, inMath);
	    TtaPreviousSibling (&currentrow);
	  }

	if (!last)
	  {
	    /* we have to manage following rows too */
	    currentrow = row;
	    TtaNextSibling (&currentrow);
	    while (currentrow != NULL)
	      {
		/* get the sibling cell */
		if (last)
		  child = TtaGetLastChild (currentrow);
		else
		  child = GetCloseCellFromColumnHead (currentrow, lastcolhead, before, inMath);
		AddEmptyCellInRow (currentrow, colhead, child, before, doc, inMath);
		TtaNextSibling (&currentrow);
	      }
	  }
	if (!inMath)
	  {
	    groupdone = TtaGetParent (row);	/* done with this group */
	    /* process the other row groups */
	    elType.ElTypeNum = HTML_EL_Table;
	    table = TtaGetTypedAncestor (groupdone, elType);
	    /* visit all children of the Table element */
	    child = TtaGetFirstChild (table);
	    while (child != NULL)
	      {
		elType = TtaGetElementType (child);
		if (elType.ElTypeNum == HTML_EL_thead ||
		    elType.ElTypeNum == HTML_EL_tfoot)
		  {
		    /* this child is a thead or tfoot element */
		    group = child;
		    if (group != groupdone)
		      {
			currentrow = TtaGetFirstChild (group);
			while (currentrow != NULL)
			  {
			    /* get the sibling cell */
			    if (last)
			      child = TtaGetLastChild (currentrow);
			    else
			      child = GetCloseCellFromColumnHead (currentrow, lastcolhead, before, inMath);
			    AddEmptyCellInRow (currentrow, colhead, child, before, doc, inMath);
			    TtaNextSibling (&currentrow);
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
			    while (currentrow != NULL)
			      {
				/* get the sibling cell */
				if (last)
				  child = TtaGetLastChild (currentrow);
				else
				  child = GetCloseCellFromColumnHead (currentrow, lastcolhead, before, inMath);
				AddEmptyCellInRow (currentrow, colhead, child, before, doc, inMath);
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
   return colhead;
}


/*----------------------------------------------------------------------
   SetRowExt       Sets the attribute RowExt of cell "cell" in row 
   "row", according to span. Return the row which is the   
   bottom of the spanning cell.                            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static Element      SetRowExt (Element cell, Element row, int span, Document doc)
#else
static Element      SetRowExt (cell, row, span, doc)
Element             cell;
Element             row;
int                 span;
Document            doc;

#endif
{
   Element             spannedrow, nextspannedrow, ret;
   ElementType         elType;
   AttributeType       attrType;
   Attribute           attr;

   spannedrow = row;
   ret = NULL;
   while (span > 0 && spannedrow != NULL)
     {
	nextspannedrow = spannedrow;
	TtaNextSibling (&nextspannedrow);
	elType = TtaGetElementType (spannedrow);
	/* process only Table_row elements */
	if (elType.ElTypeNum == HTML_EL_Table_row)
	  {
	     if (span == 1 || nextspannedrow == NULL)
		if (spannedrow != row)
		  {
		     attrType.AttrSSchema = elType.ElSSchema;
		     attrType.AttrTypeNum = HTML_ATTR_RowExt;
		     attr = TtaGetAttribute (cell, attrType);
		     if (attr == NULL)
		       {
			  attr = TtaNewAttribute (attrType);
			  if (attr != NULL)
			     TtaAttachAttribute (cell, attr, doc);
		       }
		     if (attr != NULL)
			TtaSetAttributeReference (attr, cell, doc, spannedrow, doc);
		     ret = spannedrow;
		  }
	     span--;
	  }
	spannedrow = nextspannedrow;
     }
   return ret;
}


/*----------------------------------------------------------------------
   MaximumRowSpan                                                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          MaximumRowSpan (Element row, int span)
#else
static int          MaximumRowSpan (row, span)
Element             row;
int                 span;

#endif
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
	     if (elType.ElTypeNum == HTML_EL_Data_cell ||
		 elType.ElTypeNum == HTML_EL_Heading_cell)
	       {
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


#define MAX_COLS 100
/*----------------------------------------------------------------------
  CheckAllRows
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CheckAllRows (Element table, Document doc)
#else
void                CheckAllRows (table, doc)
Element             table;
Document            doc;
#endif
{
  Element            *colElement;
  Element             row, nextRow, firstrow, colhead;
  Element             cell, nextCell, group, new;
  Element             differredHSpan[MAX_COLS];
  ElementType         elType;
  AttributeType       attrTypeHSpan, attrTypeVSpan, attrType;
  Attribute           attr;
  int                *colVSpan, hspan, differredRef[MAX_COLS];
  int                 span, cRef, cNumber;
  int                 i, rowType;
  ThotBool            inMath;

  if (table == NULL)
    return;

  colElement = TtaGetMemory (sizeof (Element) * MAX_COLS);
  colVSpan = TtaGetMemory (sizeof (int) * MAX_COLS);
  
  /* store the list of colheads */
  elType = TtaGetElementType (table);
  inMath = !TtaSameSSchemas (elType.ElSSchema, TtaGetSSchema (TEXT("HTML"), doc));
  if (inMath)
    {
      elType.ElTypeNum = MathML_EL_MColumn_head;
      rowType = MathML_EL_MTR;
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
  /* number of differed colspan rules */
  hspan = 0;
  attrType.AttrSSchema = elType.ElSSchema;
  attrTypeHSpan.AttrSSchema = elType.ElSSchema;
  attrTypeHSpan.AttrTypeNum = HTML_ATTR_colspan_;
  attrTypeVSpan.AttrSSchema = elType.ElSSchema;
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
	if (elType.ElTypeNum == rowType)
	  {
	  /* treat all cells in the row */
	  cRef = 0;
	  cell = NULL;
	  nextCell = TtaGetFirstChild (row);
	  while (nextCell != NULL)
	    {
	      cell = nextCell;
	      TtaNextSibling (&nextCell);
	      elType = TtaGetElementType (cell);
	      if (!inMath && elType.ElTypeNum == HTML_EL_Table_cell)
		{
		  /* replace the Table_cell by a Data_cell */
		  elType.ElTypeNum = HTML_EL_Data_cell;
		  new = TtaNewTree (doc, elType, "");
		  TtaInsertFirstChild (&new, cell, doc);
		  cell = new;
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
		      colElement[cRef] = NewColumnHead (colElement[cRef - 1], FALSE, TRUE, row, doc, inMath);
		      colVSpan[cRef] = 0;
		      cNumber++;
		    }

		  if (cRef < cNumber)
		    {
		      /* relate the cell with its corresponding Column_head */
		      RelateCellWithColumnHead (cell, colElement[cRef], doc, inMath);

		      if (!inMath)
			{
			  /* is there an attribute rowspan for that cell ? */
			  attr = TtaGetAttribute (cell, attrTypeVSpan);
			  if (attr != NULL)
			    {
			      span = TtaGetAttributeValue (attr);
			      if (span > 1)
				{
				  /* Set the attribute RowExt if row span > 1 */
				  colVSpan[cRef] = span - 1;
				  SetRowExt (cell, row, span, doc);
				}
			    }
			  /* if there an attribute colspan for that cell,
			     update attribute ColExt */
			  attr = TtaGetAttribute (cell, attrTypeHSpan);
			  if (attr != NULL)
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
				    /* Create additional Column_heads */
				    while (i < span)
				      {
					if (cRef + 1 < MAX_COLS)
					  {
					    cRef++;
					    colElement[cRef] = NewColumnHead (colElement[cRef-1], FALSE, TRUE, row, doc, inMath);
					    cNumber++;
					  }
					colVSpan[cRef] = colVSpan[cRef-1];
					i++;
				      }
				  else if (i < span && hspan < MAX_COLS)
				    {
				      /* differs the management of the colspan */
				      differredHSpan[hspan] = cell;
				      differredRef[hspan] = cRef - i + 1;
				      hspan++;
				      span = 1;
				    }
				  if (span > 1)
				    {
				      /* set the attribute ColExt */
				      attrType.AttrTypeNum = HTML_ATTR_ColExt;
				      attr = TtaGetAttribute (cell, attrType);
				      if (attr == NULL)
					{
					  attr = TtaNewAttribute (attrType);
					  TtaAttachAttribute (cell, attr, doc);
					}
				      if (attr != NULL)
					TtaSetAttributeReference (attr, cell, doc, colElement[cRef], doc);
				    }
				}
			    }
			}
		      cRef++;
		    }
		}
	      /* accept comments and invalid elements */
	      else if (!inMath &&
		       elType.ElTypeNum != HTML_EL_Invalid_element &&
		       elType.ElTypeNum != HTML_EL_Comment_)
		/* Delete any other type of element */
		TtaDeleteTree (cell, doc);
	      /* accept XML comments */
	      else if (inMath &&
		       elType.ElTypeNum != MathML_EL_XMLcomment)
		/* Delete any other type of element */
		TtaDeleteTree (cell, doc);
	    }

	  /* check missing cells */
	  while (cRef < cNumber)
	    {
	      if (colVSpan[cRef] > 0)
		colVSpan[cRef]--;
	      else
		cell = AddEmptyCellInRow (row, colElement[cRef], cell, FALSE, doc, inMath);
	      cRef++;
	    }
	  }
	/* accept comments and invalid elements instead of rows */
	else if (!inMath &&
		 elType.ElTypeNum != HTML_EL_Invalid_element &&
		 elType.ElTypeNum != HTML_EL_Comment_)
	  /* Delete any other type of element */
	  TtaDeleteTree (row, doc);
	/* accept XML comments */
	else if (inMath && cell &&
		 elType.ElTypeNum != MathML_EL_XMLcomment)
	  /* Delete any other type of element */
	  TtaDeleteTree (cell, doc);
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
		if (elType.ElTypeNum == HTML_EL_Table_foot)
		  /* don't look for rows in the Table_foot! */
		  row = NULL;
		else
		  row = TtaGetFirstChild (group);
	      }
	    else
	      row = NULL;
	  }
	}
    }
  /* now manage differed colspan rules */
  for (i = 0; i < hspan; i++)
    {
      /* if there an attribute colspan for that cell,
	 update attribute ColExt */
      cell = differredHSpan[i];
      attr = TtaGetAttribute (cell, attrTypeHSpan);
      span = TtaGetAttributeValue (attr);
      cRef = differredRef[i];
      if (span + cRef >= cNumber)
	{
	  span = cNumber - cRef;
	  /* the span value is not correct, set the right value */
	  TtaSetAttributeValue (attr, span, cell, doc);
	}
      if (span > 1)
	{
	  /* set the attribute ColExt */
	  attrType.AttrTypeNum = HTML_ATTR_ColExt;
	  attr = TtaGetAttribute (cell, attrType);
	  if (attr == NULL)
	    {
	      attr = TtaNewAttribute (attrType);
	      TtaAttachAttribute (cell, attr, doc);
	    }
	  if (attr != NULL)
	    TtaSetAttributeReference (attr, cell, doc, colElement[cRef + span - 1], doc);
	  /* remove extra cells */
	  nextCell = cell;
	  TtaNextSibling (&nextCell);
	  while (span > 1 && nextCell != NULL)
	    {
	      cell = nextCell;
	      TtaNextSibling (&nextCell);
	      TtaDeleteTree (cell, doc);
	      span--;
	    }
	}
    }
  TtaFreeMemory (colElement);
  TtaFreeMemory (colVSpan);

}

/*----------------------------------------------------------------------
   CheckTable      Check a table and create the missing elements.  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CheckTable (Element table, Document doc)
#else
void                CheckTable (table, doc)
Element             table;
Document            doc;

#endif
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
	      /* skip elements Comment and Invalid_element and insert the new */
	      /* element Table_head, after the element caption if it is present */
	      /* or as the first child if there is no caption */
	      while (el != NULL)
		{
		  elType = TtaGetElementType (el);
		  if (elType.ElTypeNum == HTML_EL_Invalid_element ||
		      elType.ElTypeNum == HTML_EL_Comment_)
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
		  /* collect all Table_row elements and put them in the new Table_body */
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
	    if (tfoot != NULL)
	      {
		/* move element tfoot at the end */
		TtaRemoveTree (tfoot, doc);
		TtaInsertSibling (tfoot, Tablebody, FALSE, doc);
		TtaInsertSibling (foot, tfoot, FALSE, doc);
	      }
	    else
	      TtaInsertSibling (foot, Tablebody, FALSE, doc);
	  
	  if (thead != NULL)
	    firstgroup = thead;
	  else if (Tablebody != NULL)
	    firstgroup = Tablebody;
	  else
	    firstgroup = tfoot;

	  /* associate each cell with a column */
	  CheckAllRows (table, doc);
	  CheckTableAfterCellUpdate = TRUE;
	}
    }
  /* resume document structure checking */
  TtaSetStructureChecking ((ThotBool)PreviousStuctureChecking, doc);
}

/*----------------------------------------------------------------------
  NewCell  a new cell has been created in a HTML table.
  If genrateColumn is TRUE, the new cell generates a new column.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                NewCell (Element cell, Document doc, ThotBool genrateColumn)
#else
void                NewCell (cell, doc, genrateColumn)
Element             cell;
Document            doc;
ThotBool            genrateColumn;
#endif
{
  Element             newcell, row;
  Element             colhead;
  ElementType         elType;
  AttributeType       attrType;
  Attribute           attr;
  CHAR_T                ptr[100];
  int                 span, i;
  ThotBool            before, inMath;
#ifndef STANDALONE
  DisplayMode         dispMode;
  
  dispMode = TtaGetDisplayMode (doc);
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, DeferredDisplay);
#endif

  elType = TtaGetElementType (cell);
  inMath = !TtaSameSSchemas (elType.ElSSchema, TtaGetSSchema (TEXT("HTML"), doc));

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
	  elType.ElTypeNum = MathML_EL_MTR;
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
	      if (!inMath)
		{
		  /* if there an attribute colspan for that cell, 
		     select the right colhead */
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
	}
      if (colhead != NULL)
	{
	  if (genrateColumn)
	    /* generate the new column */
	    colhead = NewColumnHead (colhead, before, FALSE, row, doc, inMath);
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

#ifndef STANDALONE
   TtaSetDisplayMode (doc, dispMode);
#endif
}


/*----------------------------------------------------------------------
   CellCreated

   a new cell has been created in a table
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CellCreated (NotifyElement * event)
#else
void                CellCreated (event)
NotifyElement      *event;

#endif
{
   Element             cell, row;
   Document            doc;

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
     NewCell (cell, doc, TRUE);
}

/*----------------------------------------------------------------------
   CellPasted                                              
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CellPasted (NotifyElement * event)
#else
void                CellPasted (event)
NotifyElement      *event;

#endif
{
   Element             cell, nextcell, row;
   Document            doc;

   cell = event->element;
   doc = event->document;
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
       /* a single cell has been pasted */
       NewCell (cell, doc, TRUE);
       CurrentPastedRow = NULL;
     }
}


/*----------------------------------------------------------------------
   PreDeleteRow                                            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         PreDeleteRow (Element row, Document doc)
#else
static void         PreDeleteRow (row, doc)
Element             row;
Document            doc;
#endif
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
   inMath = !TtaSameSSchemas (elType.ElSSchema, TtaGetSSchema (TEXT("HTML"), doc));
   if (!inMath)
     {
       attrType.AttrSSchema = elType.ElSSchema;
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
       MaxRowSpan = MaximumRowSpan (row, MaxRowSpan);
     }
   else
     MaxRowSpan = 1;
}

/*----------------------------------------------------------------------
   DeleteRow                                               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            DeleteRow (NotifyElement * event)
#else
ThotBool            DeleteRow (event)
NotifyElement      *event;

#endif
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
#ifdef __STDC__
void                RowDeleted (NotifyElement * event)
#else
void                RowDeleted (event)
NotifyElement      *event;

#endif
{
  Element             rowgroup, table;
  ElementType         elType;
  Document            doc;
  ThotBool            inMath;

   CurrentDeletedRow = NULL;
   rowgroup = event->element;
   if (rowgroup == NULL)
     /* the row doesn't exist */
     return;
   elType = TtaGetElementType (rowgroup);
   inMath = !TtaSameSSchemas (elType.ElSSchema, TtaGetSSchema (TEXT("HTML"), event->document));
   if (inMath)
     elType.ElTypeNum = MathML_EL_MTABLE;
   else
     elType.ElTypeNum = HTML_EL_Table;
   table = TtaGetTypedAncestor (rowgroup, elType);
   doc = event->document;
   CheckAllRows (table, doc);
   CheckTableAfterCellUpdate = TRUE;
}


/*----------------------------------------------------------------------
   DeleteCell                                              
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            DeleteCell (NotifyElement * event)
#else
ThotBool            DeleteCell (event)
NotifyElement      *event;
#endif
{
  Element             cell;
  ElementType         elType;
  AttributeType       attrType;
  Attribute           attr;
  Document            refDoc;
  CHAR_T                name[50];
  ThotBool            inMath;

  if (CurrentRow)
    /* manage only one row at the same time */
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
  inMath = !TtaSameSSchemas (elType.ElSSchema, TtaGetSSchema (TEXT("HTML"), event->document));

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

  if (!inMath)
    {
      /* get current column span */
      attrType.AttrTypeNum = HTML_ATTR_colspan_;
      attr = TtaGetAttribute (cell, attrType);
      if (attr != NULL)
	CurrentSpan = TtaGetAttributeValue (attr);
      else
	CurrentSpan = 1;
      /* let Thot perform normal operation */
    }
  else
    CurrentSpan = 1;
  return FALSE;
}


/*----------------------------------------------------------------------
  RemoveColumn remove the current colhead if it's empty.
  The parameter ifEmpty makes the removing optional.
  Returns TRUE if the column has been removed.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool     RemoveColumn (Element colhead, Document doc, ThotBool ifEmpty, ThotBool inMath)
#else
ThotBool     RemoveColumn (colhead, doc, ifEmpty, inMath)
Element      colhead;
Document     doc;
ThotBool     ifEmpty;
ThotBool     inMath;
#endif
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
  if (inMath)
    {
      elType.ElTypeNum = MathML_EL_MTABLE;
      rowType = MathML_EL_MTR;
    }
  else
    {
      elType.ElTypeNum = HTML_EL_Table;
      rowType = HTML_EL_Table_row;
      attrType.AttrSSchema = elType.ElSSchema;
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
	      if (cell != NULL && TtaGetVolume (cell) != 0)
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
	  span = FALSE; /* TRUE when a cell with aspan value is deleted */
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
		  if (TtaPrepareUndo (doc))
		     TtaRegisterElementDelete (cell, doc);
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
	  if (TtaPrepareUndo (doc))
	      TtaDeleteTree (colhead, doc);
	  if (span)
	     CheckAllRows (table, doc);
	}
    }
  return (empty);
}


/*----------------------------------------------------------------------
   CellDeleted                                             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CellDeleted (NotifyElement * event)
#else
void                CellDeleted (event)
NotifyElement      *event;
#endif
{
  Element             cell, col, child;
  ElementType         elType;
  Document            doc;
  int                 span;
  ThotBool            removed;
  ThotBool            inMath;
  ThotBool            before;

  doc = event->document;
  span = CurrentSpan;
  CurrentSpan = 0;
  elType = TtaGetElementType (event->element);
  inMath = !TtaSameSSchemas (elType.ElSSchema, TtaGetSSchema (TEXT("HTML"), event->document));
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
      cell = GetCloseCellFromColumnHead (CurrentRow, col, before, inMath);
      /* regenerate an empty cell */
      cell = AddEmptyCellInRow (CurrentRow, CurrentColumn, cell, before, doc, inMath);
      child = TtaGetFirstChild (cell);
      TtaSelectElement (doc, child);
    }
  CurrentColumn = NULL;
  CurrentRow = NULL;
}

/*----------------------------------------------------------------------
   TableCreated                                            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TableCreated (NotifyElement * event)
#else
void                TableCreated (event)
NotifyElement      *event;

#endif
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
#ifdef __STDC__
void                TablebodyDeleted (NotifyElement * event)
#else
void                TablebodyDeleted (event)
NotifyElement      *event;
#endif
{
  Element             sibling, table;
  ElementType	      elType;
  Document            doc;
  ThotBool	      empty;

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
#ifdef __STDC__
void                RowCreated (NotifyElement * event)
#else
void                RowCreated (event)
NotifyElement      *event;

#endif
{
  Element             row;
  Element             table;
  ElementType         elType;
  Document            doc;
  ThotBool            inMath;

  row = event->element;
  doc = event->document;
  elType = TtaGetElementType (row);
  inMath = !TtaSameSSchemas (elType.ElSSchema, TtaGetSSchema (TEXT("HTML"), event->document));
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
	CheckMTable (table, doc);
      else
	CheckTable (table, doc);
      NewTable = FALSE;
    }
  else
    {
      CheckAllRows (table, doc);
      CheckTableAfterCellUpdate = FALSE;
      /* avoid processing the cells of the created row */
      CurrentRow = row;
    }
  CurrentCell = TtaGetLastChild (row);
}


/*----------------------------------------------------------------------
   RowPasted                                               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                RowPasted (NotifyElement * event)
#else
void               <RowPasted (event)
NotifyElement      *event;
#endif
{
  Element             row;
  Element             table;
  ElementType         elType;
  Document            doc;
  ThotBool            inMath;

  row = event->element;
  doc = event->document;
  elType = TtaGetElementType (row);
  inMath = !TtaSameSSchemas (elType.ElSSchema, TtaGetSSchema (TEXT("HTML"), event->document));
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
	CheckMTable (table, doc);
      else
	CheckTable (table, doc);
      NewTable = FALSE;
    }
  else
    {
      CheckAllRows (table, doc);
      CheckTableAfterCellUpdate = FALSE;
    }
  /* avoid processing the cells of the created row */
  CurrentPastedRow = row;
  CurrentCell = TtaGetLastChild (row);
}

/*----------------------------------------------------------------------
   ChangeColSpan                                           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ChangeColSpan (Element cell, int oldspan, int newspan, Document doc)
#else
static void         ChangeColSpan (cell, oldspan, newspan, doc)
Element             cell;
int                 oldspan;
int                 newspan;
Document            doc;

#endif
{
   Element             table;
   ElementType         elType;

   if (cell == NULL)
      return;

   elType = TtaGetElementType (cell);
   elType.ElTypeNum = HTML_EL_Table;
   table = TtaGetTypedAncestor (cell, elType);
   CheckAllRows (table, doc);
   CheckTableAfterCellUpdate = TRUE;
}

/*----------------------------------------------------------------------
   ColspanCreated                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ColspanCreated (NotifyAttribute * event)
#else
void                ColspanCreated (event)
NotifyAttribute    *event;

#endif
{
   int                 span;

   span = TtaGetAttributeValue (event->attribute);
   if (span <= 1)
      /* invalid value */
      TtaRemoveAttribute (event->element, event->attribute, event->document);
   else
      ChangeColSpan (event->element, 1, span, event->document);
   CheckTableAfterCellUpdate = FALSE;
}


/*----------------------------------------------------------------------
   RegisterColspan                                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            RegisterColspan (NotifyAttribute * event)
#else
ThotBool            RegisterColspan (event)
NotifyAttribute    *event;

#endif
{
   Attribute           attr;

   attr = event->attribute;
   PreviousColSpan = TtaGetAttributeValue (attr);
   return FALSE;		/* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
   ColspanModified                                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ColspanModified (NotifyAttribute * event)
#else
void                ColspanModified (event)
NotifyAttribute    *event;

#endif
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
  if (span != PreviousColSpan)
    ChangeColSpan (cell, PreviousColSpan, span, doc);
  if (span <= 1)
    /* invalid value */
    TtaRemoveAttribute (cell, attr, doc);
}

/*----------------------------------------------------------------------
   ColspanDelete                                           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            ColspanDelete (NotifyAttribute * event)
#else
ThotBool            ColspanDelete (event)
NotifyAttribute    *event;

#endif
{
  int                 span;
  
  span = TtaGetAttributeValue (event->attribute);
  if (span > 1)
    {
      TtaRemoveAttribute (event->element, event->attribute, event->document);
      ChangeColSpan (event->element, span, 1, event->document);
      return TRUE;
    }
  else
    return FALSE;		/* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
   ChangeRowSpan                                           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ChangeRowSpan (Element cell, int oldspan, int newspan, Document doc)
#else
static void         ChangeRowSpan (cell, oldspan, newspan, doc)
Element             cell;
int                 oldspan;
int                 newspan;
Document            doc;

#endif
{
   Element             table;
   ElementType         elType;
 
   if (oldspan == newspan)
      return;
   if (cell == NULL)
      return;

   elType = TtaGetElementType (cell);
   elType.ElTypeNum = HTML_EL_Table;
   table = TtaGetTypedAncestor (cell, elType);
   CheckAllRows (table, doc);
   CheckTableAfterCellUpdate = TRUE;
}


/*----------------------------------------------------------------------
   RowspanCreated                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                RowspanCreated (NotifyAttribute * event)
#else
void                RowspanCreated (event)
NotifyAttribute    *event;

#endif
{
   int                 span;

   span = TtaGetAttributeValue (event->attribute);
   if (span <= 1)
      /* invalid value */
      TtaRemoveAttribute (event->element, event->attribute, event->document);
   else
      ChangeRowSpan (event->element, 1, span, event->document);
}


/*----------------------------------------------------------------------
   RegisterRowspan                                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            RegisterRowspan (NotifyAttribute * event)
#else
ThotBool            RegisterRowspan (event)
NotifyAttribute    *event;

#endif
{
   Attribute           attr;

   attr = event->attribute;
   PreviousRowSpan = TtaGetAttributeValue (attr);
   return FALSE;		/* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
   RowspanModified                                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                RowspanModified (NotifyAttribute * event)
#else
void                RowspanModified (event)
NotifyAttribute    *event;

#endif
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
   if (span != PreviousRowSpan)
      ChangeRowSpan (cell, PreviousRowSpan, span, doc);
   if (span <= 1)
      /* invalid value */
      TtaRemoveAttribute (cell, attr, doc);
}

/*----------------------------------------------------------------------
   RowspanDelete
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            RowspanDelete (NotifyAttribute * event)
#else
ThotBool            RowspanDelete (event)
NotifyAttribute    *event;

#endif
{
   int                 span;

   span = TtaGetAttributeValue (event->attribute);
   if (span > 1)
     {
	TtaRemoveAttribute (event->element, event->attribute, event->document);
	ChangeRowSpan (event->element, span, 1, event->document);
	return TRUE;
     }
   else
      return FALSE;		/* let Thot perform normal operation */
}
