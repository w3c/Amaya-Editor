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

static Element      CurrentPastedRow = NULL;
static Element      CurrentCreatedRow = NULL;
static Element      CurrentColumn = NULL;
static int          CurrentSpan;
static int          maxcellspan;


/*----------------------------------------------------------------------
   GetCellFromColumnHead

   returns the cell that corresponds to the Column_head element colhead
   in a given row.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static Element      GetCellFromColumnHead (Element row, Element colhead)
#else
static Element      GetCellFromColumnHead (row, colhead)
Element             row;
Element             colhead;
#endif
{
   Element             cell, currentcolhead;
   boolean             found;
   ElementType         elType;
   AttributeType       attrType;
   Attribute           attr;
   char                name[50];
   Document            refdoc;

   cell = TtaGetFirstChild (row);
   found = FALSE;
   while (cell != NULL && !found)
     {
	elType = TtaGetElementType (cell);
	if (elType.ElTypeNum == HTML_EL_Data_cell ||
	    elType.ElTypeNum == HTML_EL_Heading_cell)
	  {
	     attrType.AttrSSchema = elType.ElSSchema;
	     attrType.AttrTypeNum = HTML_ATTR_Ref_column;
	     attr = TtaGetAttribute (cell, attrType);
	     if (attr != NULL)
	       {
		  TtaGiveReferenceAttributeValue (attr, &currentcolhead, name, &refdoc);
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
   RelateCellWithColumnHead relates a cell with a Column_head.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         RelateCellWithColumnHead (Element cell, Element colhead, Document doc)
#else
static void         RelateCellWithColumnHead (cell, colhead, doc)
Element             cell;
Element             colhead;
Document            doc;

#endif
{
   ElementType         elType;
   AttributeType       attrType;
   Attribute           attr;

   if (cell == NULL || colhead == NULL)
      return;
   elType = TtaGetElementType (cell);
   attrType.AttrSSchema = elType.ElSSchema;
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
   AddEmptyCellInRow add empty cell element at the   
   end of the given row and relates it to the given Column_head.
   Return the last created empty cell.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static Element      AddEmptyCellInRow (Element row, Element colhead, Document doc)
#else
static Element      AddEmptyCellInRow (row, colhead, doc)
Element             row;
Element             colhead;
Document            doc;

#endif
{
   Element             lastcell, ghost;
   ElementType         elType;

   if (row == NULL)
      return (NULL);
   elType = TtaGetElementType (row);
   if (elType.ElTypeNum != HTML_EL_Table_row)
      return (NULL);

   lastcell = TtaGetLastChild (row);
   if (lastcell != NULL)
     {
	elType.ElTypeNum = HTML_EL_Data_cell;
	ghost = TtaNewTree (doc, elType, "");
	if (ghost != NULL)
	  {
	    TtaInsertSibling (ghost, lastcell, FALSE, doc);
	    RelateCellWithColumnHead (ghost, colhead, doc);
	  }
     }
   return (lastcell);
}

/*----------------------------------------------------------------------
   NewColumnHead   create a new last Column_head and returns it.   
   If all == TRUE, create an additional empty cell in all  
   rows, except the row indicated.                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static Element      NewColumnHead (Element lastcolhead, Document doc, boolean all, Element row)
#else
static Element      NewColumnHead (lastcolhead, doc, all, row)
Element             lastcolhead;
Document            doc;
boolean             all;
Element             row;

#endif
{
   Element             colhead, currentrow, group, groupdone;
   Element             table, child;
   ElementType         elType;

   if (lastcolhead == NULL)
      return NULL;
   elType = TtaGetElementType (lastcolhead);
   elType.ElTypeNum = HTML_EL_Column_head;
   colhead = TtaNewTree (doc, elType, "");
   if (colhead != NULL)
     {
	TtaInsertSibling (colhead, lastcolhead, FALSE, doc);
	/* add empty cells to all other rows */
	/* process the row group that contains the row which doesn't */
	/* need to be processed */
	currentrow = row;
	while (currentrow != NULL)
	  {
	    TtaPreviousSibling (&currentrow);
	    AddEmptyCellInRow (currentrow, colhead, doc);
	  }

	if (all)
	  {
	    currentrow = row;
	    while (currentrow != NULL)
	      {
		TtaNextSibling (&currentrow);
		AddEmptyCellInRow (currentrow, colhead, doc);
	      }
	  }
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
			AddEmptyCellInRow (currentrow, colhead, doc);
			TtaNextSibling (&currentrow);
		      }
		  }
		else if (!all)
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
			    AddEmptyCellInRow (currentrow, colhead, doc);
			    TtaNextSibling (&currentrow);
			  }
		      }

		    if (!all)
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
   GetFirstColumnHead      search the first Column_head element    
   in the table to which el belongs.                       
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static Element      GetFirstColumnHead (Element el)
#else
static Element      GetFirstColumnHead (el)
Element             el;

#endif

{
   Element             head, table, child, firstcolhead;
   ElementType         elType;

   elType = TtaGetElementType (el);
   elType.ElTypeNum = HTML_EL_Table;
   table = TtaGetTypedAncestor (el, elType);
   child = TtaGetFirstChild (table);
   head = NULL;
   while (head == NULL && child != NULL)
     {
	elType = TtaGetElementType (child);
	if (elType.ElTypeNum == HTML_EL_Table_head)
	   head = child;
	else
	   TtaNextSibling (&child);
     }
   if (head == NULL)
      firstcolhead = NULL;
   else
      firstcolhead = TtaGetFirstChild (head);
   return firstcolhead;
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
   CheckTable      Check a table and create the missing elements.  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         CheckAllRows (Element table, Document doc)
#else
static void         CheckAllRows (table, doc)
Element             table;
Document            doc;
#endif
{
  Element            *colElement;
  Element             row, firstrow, colhead;
  Element             cell, group;
  ElementType         elType;
  AttributeType       attrTypeHSpan, attrTypeVSpan, attrType;
  Attribute           attr;
  int                *colVSpan;
  int                 span, cRef, cNumber;
  int                 i;

  if (table == NULL)
    return;

  colElement = TtaGetMemory (sizeof (Element) * MAX_COLS);
  colVSpan = TtaGetMemory (sizeof (int) * MAX_COLS);

  /* store list of colheads */
  elType = TtaGetElementType (table);
  elType.ElTypeNum = HTML_EL_Column_head;
  colhead = TtaSearchTypedElement (elType, SearchForward, table);
  cNumber = 0;
  while (colhead != 0 && cNumber < MAX_COLS)
    {
      colElement[cNumber] = colhead;
      colVSpan[cNumber] = 0;
      TtaNextSibling (&colhead);
      cNumber++;
    }
  
  attrType.AttrSSchema = elType.ElSSchema;
  attrTypeHSpan.AttrSSchema = elType.ElSSchema;
  attrTypeHSpan.AttrTypeNum = HTML_ATTR_colspan_;
  attrTypeVSpan.AttrSSchema = elType.ElSSchema;
  attrTypeVSpan.AttrTypeNum = HTML_ATTR_rowspan_;
  elType.ElTypeNum = HTML_EL_Table_row;
  firstrow = TtaSearchTypedElement (elType, SearchForward, table);
  if (cNumber != 0 && firstrow != NULL)
    {
      row = firstrow;
      /* the rows group could be thead, tbody, tfoot */
      group = TtaGetParent (row);
      while (row != NULL)
	{
	  /* treat all cells in the row */
	  cRef = 0;
	  cell = TtaGetFirstChild (row);
	  while (cell != NULL)
	    {
	      elType = TtaGetElementType (cell);
	      /* process only cell elements */
	      if (elType.ElTypeNum == HTML_EL_Data_cell ||
		  elType.ElTypeNum == HTML_EL_Heading_cell)
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
		      colElement[cRef] = NewColumnHead (colElement[cRef - 1], doc, FALSE, row);
		      colVSpan[cRef] = 0;
		      cNumber++;
		    }

		  if (cRef < cNumber)
		    {
		      /* relate the cell with its corresponding Column_head */
		      RelateCellWithColumnHead (cell, colElement[cRef], doc);
		      
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
		      /* if there an attribute colspan for that cell, update
			 attribute ColExt */
		      attr = TtaGetAttribute (cell, attrTypeHSpan);
		      if (attr != NULL)
			{
			  span = TtaGetAttributeValue (attr);
			  if (span > 1)
			    {
			      for (i = 1; i < span; i++)
				{
				  cRef++;
				  if (cRef == cNumber && cRef < MAX_COLS)
				    {
				      /* there is no Column_head for that cell */
				      /* Create an additional Column_head */
				      colElement[cRef] = NewColumnHead (colElement[cRef-1], doc, FALSE, row);
				      cNumber++;
				    }
				  colVSpan[cRef] = colVSpan[cRef-1];
				}

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
			  cRef++;
			}
		      else
			cRef++;
		    }
		}
	      TtaNextSibling (&cell);
	    }

	  /* check missing cells */
	  while (cRef < cNumber)
	    {
	      if (colVSpan[cRef] > 0)
		colVSpan[cRef]--;
	      else
		
		AddEmptyCellInRow (row, colElement[cRef], doc);
	      cRef++;
	    }

	  TtaNextSibling (&row);
	  /* do we have to get a new group of rows */
	  if (row == NULL)
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
  boolean             before;

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
	}
    }
  /* resume document structure checking */
  TtaSetStructureChecking (PreviousStuctureChecking, doc);
}

/*----------------------------------------------------------------------
  NewCell  a new cell has been created in a HTML table.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                NewCell (Element cell, Document doc)
#else
void                NewCell (cell, doc)
Element             cell;
Document            doc;
#endif
{
   Element             newcell;
   Element             table;
   ElementType         elType;
#ifndef STANDALONE
   DisplayMode         dispMode;

   dispMode = TtaGetDisplayMode (doc);
   if (dispMode == DisplayImmediately)
     TtaSetDisplayMode (doc, DeferredDisplay);
#endif

   elType = TtaGetElementType (cell);
   if (elType.ElTypeNum == HTML_EL_Table_cell)
      /* change the cell into a Data_cell */
     {
	elType.ElTypeNum = HTML_EL_Data_cell;
	newcell = TtaNewTree (doc, elType, "");
	TtaInsertFirstChild (&newcell, cell, doc);
	cell = newcell;
     }

   elType.ElTypeNum = HTML_EL_Table;
   table = TtaGetTypedAncestor (cell, elType);
   CheckAllRows (table, doc);

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
   Element             cell;
   Document            doc;

   cell = event->element;
   doc = event->document;
   if (TtaGetParent (cell) == CurrentCreatedRow)
      /* the new cell belongs to the newly created row. Already processed */
      /* by RowCreated. */
      CurrentCreatedRow = NULL;
   else
      /* a new cell in an existing row */
      NewCell (cell, doc);
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
   Element             table;
   ElementType         elType;
   Document            doc;

   cell = event->element;
   doc = event->document;
   row = TtaGetParent (cell);
   if (row != CurrentPastedRow)
      /* a single cell has been pasted */
     {
	NewCell (cell, doc);
	elType = TtaGetElementType (cell);
	elType.ElTypeNum = HTML_EL_Table;
	table = TtaGetTypedAncestor (cell, elType);
	CheckAllRows (table, doc);
	CurrentPastedRow = NULL;
     }
   else
      /* this cell is part of the pasted row */
     {
	nextcell = cell;
	TtaNextSibling (&nextcell);
	if (nextcell == NULL)
	   /* this is the last cell in the pasted row */
	   CurrentPastedRow = NULL;
     }
}


static int          maxrowspan;

/*----------------------------------------------------------------------
   PreDeleteRow                                            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         PreDeleteRow (Element row)
#else
static void         PreDeleteRow (row)
Element             row;

#endif
{
   Element             cell;
   ElementType         elType;
   AttributeType       attrType;
   Attribute           attr;
   int                 span;

   if (row == NULL)
      return;
   cell = TtaGetFirstChild (row);
   if (cell == NULL)
      return;
   /* seach the maximum value of attribute rowspan for the deleted row */
   elType = TtaGetElementType (row);
   attrType.AttrSSchema = elType.ElSSchema;
   attrType.AttrTypeNum = HTML_ATTR_rowspan_;
   maxrowspan = 1;
   while (cell != NULL)
     {
	attr = TtaGetAttribute (cell, attrType);
	if (attr != NULL)
	   /* this cell has an attribute rowspan */
	  {
	     span = TtaGetAttributeValue (attr);
	     if (span > maxrowspan)
		maxrowspan = span;
	  }
	TtaNextSibling (&cell);
     }
   maxrowspan = MaximumRowSpan (row, maxrowspan);
}

/*----------------------------------------------------------------------
   DeleteRow                                               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             DeleteRow (NotifyElement * event)
#else
boolean             DeleteRow (event)
NotifyElement      *event;

#endif
{
   PreDeleteRow (event->element);
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

   rowgroup = event->element;
   if (rowgroup == NULL)
      return;
   elType = TtaGetElementType (rowgroup);
   elType.ElTypeNum = HTML_EL_Table;
   table = TtaGetTypedAncestor (rowgroup, elType);
   doc = event->document;
   CheckAllRows (table, doc);
}

/*----------------------------------------------------------------------
   MaxRowSpanNextCells                                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          MaxRowSpanNextCells (Element cell)
#else
static int          MaxRowSpanNextCells (cell)
Element             cell;

#endif
{
   Element             curcell;
   ElementType         elType;
   AttributeType       attrType;
   Attribute           attr;
   int                 span, max;

   elType = TtaGetElementType (cell);
   attrType.AttrSSchema = elType.ElSSchema;
   attrType.AttrTypeNum = HTML_ATTR_rowspan_;
   curcell = cell;
   max = 1;
   while (curcell != NULL)
     {
	attr = TtaGetAttribute (curcell, attrType);
	if (attr != NULL)
	   /* this cell has an attribute rowspan */
	  {
	     span = TtaGetAttributeValue (attr);
	     if (span > max)
		max = span;
	  }
	TtaNextSibling (&curcell);
     }
   return max;
}


/*----------------------------------------------------------------------
   DeleteCell                                              
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             DeleteCell (NotifyElement * event)
#else
boolean             DeleteCell (event)
NotifyElement      *event;
#endif
{
  Element             cell;
  ElementType         elType;
  AttributeType       attrType;
  Attribute           attr;
  Document            refDoc;
  char                name[50];

  cell = event->element;
  /* seach the maximum value of attribute rowspan for the deleted cell and */
  /* all its following cells */
  maxcellspan = MaxRowSpanNextCells (cell);
  elType = TtaGetElementType (cell);
  attrType.AttrSSchema = elType.ElSSchema;
  /* get current column */
  attrType.AttrTypeNum = HTML_ATTR_Ref_column;
  attr = TtaGetAttribute (cell, attrType);
  if (attr != NULL)
    TtaGiveReferenceAttributeValue (attr, &CurrentColumn, name, &refDoc);
  else
    CurrentColumn = NULL;
  /* get current column span */
  attrType.AttrTypeNum = HTML_ATTR_colspan_;
  attr = TtaGetAttribute (cell, attrType);
  if (attr != NULL)
    CurrentSpan = TtaGetAttributeValue (attr);
  else
    CurrentSpan = 1;
  /* let Thot perform normal operation */
   return FALSE;
}

/*----------------------------------------------------------------------
  RemoveEmptyColumn remove the current colhead if it's empty.
  Returns TRUE if the column has been removed.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean      RemoveEmptyColumn (Element colhead, Document doc)
#else
static boolean      RemoveEmptyColumn (colhead, doc)
Element colhead;
Document doc;
#endif
{
  Element             row, firstrow;
  Element             cell, group, table;
  ElementType         elType;
  boolean             empty;

  empty = FALSE;
  if (colhead == NULL)
    return (empty);
  /* check if the column becomes empty */
  elType = TtaGetElementType (colhead);
  elType.ElTypeNum = HTML_EL_Table;
  table = TtaGetTypedAncestor (colhead, elType);
  elType.ElTypeNum = HTML_EL_Table_row;
  firstrow = TtaSearchTypedElement (elType, SearchForward, table);
  if (colhead != NULL && firstrow != NULL)
    {
      empty = TRUE;
      row = firstrow;
      /* the rows group could be thead, tbody, tfoot */
      group = TtaGetParent (row);
      while (row != NULL)
	{
	  cell = GetCellFromColumnHead (row, colhead);
	  if (cell != NULL && TtaGetVolume (cell) != 0)
	    empty = FALSE;
	  TtaNextSibling (&row);
	  /* do we have to get a new group of rows */
	  if (row == NULL)
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

      if (empty)
	{
	  /* remove the current column */
	  row = firstrow;
	  /* the rows group could be thead, tbody, tfoot */
	  group = TtaGetParent (row);
	  while (row != NULL)
	    {
	      cell = GetCellFromColumnHead (row, colhead);
	      if (cell != NULL)
		TtaDeleteTree (cell, doc);
	      TtaNextSibling (&row);
	      /* do we have to get a new group of rows */
	      if (row == NULL)
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
	}
    }
  return (empty);
}

/*----------------------------------------------------------------------
   RemoveExtraColumns removes extra cells corresponding to the span value.
   Return remaining span value..
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         RemoveExtraColumns (Element cell, int span, Document doc)
#else
static void         RemoveExtraColumns (cell, span, doc)
Element             cell;
int                 span;
Document            doc;
#endif
{
  Element             colhead, next, table;
  ElementType         elType;
  boolean             removed;

  /* remove last columns if needed */
  if (span > 0)
    {
      removed = TRUE;
      next = GetFirstColumnHead (cell);
      do
	{
	  colhead = next;
	  TtaNextSibling (&next);
	}
      while (next != NULL);
    
      while (span > 0 && removed && colhead != NULL)
	{
	  next = colhead;
	  TtaPreviousSibling (&next);
	  removed = RemoveEmptyColumn (colhead, doc);
	  colhead = next;
	  if (removed)
	    span--;
	}
      /* all columns corresponding to the removed span are not removed */
      if (span > 0)
	{
	  elType = TtaGetElementType (cell);
	  elType.ElTypeNum = HTML_EL_Table;
	  table = TtaGetTypedAncestor (cell, elType);
	  CheckAllRows (table, doc);
	}
    }
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
  Element             colhead;
  Element             table;
  ElementType         elType;
  Document            doc;
  int                 span;
  boolean             removed;

  doc = event->document;
  span = CurrentSpan;
  CurrentSpan = 0;
  colhead = GetFirstColumnHead (event->element);
  removed = RemoveEmptyColumn (colhead, doc);
  if (removed)
    span--;
  else
    {
      /* restore the current cell */
      elType = TtaGetElementType (event->element);
      elType.ElTypeNum = HTML_EL_Table;
      table = TtaGetTypedAncestor (event->element, elType);
      CheckAllRows (table, doc);      
    }
  if (span > 0)
    {
      /* remove extra columns if needed */
      RemoveExtraColumns (event->element, span, doc);
    }
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
  boolean	      empty;

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
     TtaDeleteTree (table, doc);
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
   Element             firstcell, row;
   Document            doc;

   row = event->element;
   doc = event->document;
   firstcell = TtaGetFirstChild (row);
   if (firstcell != NULL)
      NewCell (firstcell, doc);
   /* avoid processing the cells of the created row */
   CurrentCreatedRow = row;
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

   row = event->element;
   doc = event->document;
   elType = TtaGetElementType (row);
   elType.ElTypeNum = HTML_EL_Table;
   table = TtaGetTypedAncestor (row, elType);
   CheckAllRows (table, doc);

   /* avoid processing the cells of the pasted row */
   CurrentPastedRow = row;
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
}

static int          PreviousColSpan;

/*----------------------------------------------------------------------
   RegisterColspan                                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             RegisterColspan (NotifyAttribute * event)
#else
boolean             RegisterColspan (event)
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

  /* remove extra columns if needed */
  span -= PreviousColSpan;
  RemoveExtraColumns (cell, span, doc);
}

/*----------------------------------------------------------------------
   ColspanDelete                                           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             ColspanDelete (NotifyAttribute * event)
#else
boolean             ColspanDelete (event)
NotifyAttribute    *event;

#endif
{
  int                 span;
  
  span = TtaGetAttributeValue (event->attribute);
  if (span > 1)
    {
      TtaRemoveAttribute (event->element, event->attribute, event->document);
      ChangeColSpan (event->element, span, 1, event->document);
      /* remove extra columns if needed */
      RemoveExtraColumns (event->element, span, event->document);
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
   if (oldspan > newspan)
     /* remove extra columns if needed */
     RemoveExtraColumns (cell, 1, doc);

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

static int          PreviousRowSpan;

/*----------------------------------------------------------------------
   RegisterRowspan                                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             RegisterRowspan (NotifyAttribute * event)
#else
boolean             RegisterRowspan (event)
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
boolean             RowspanDelete (NotifyAttribute * event)
#else
boolean             RowspanDelete (event)
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
