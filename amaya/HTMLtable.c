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
	if (all)
	  {
	     /* add empty cells to all other rows */
	     /* process the row group that contains the row which doesn't */
	     /* need to be processed */
	     currentrow = row;
	     while (currentrow != NULL)
	       {
		  TtaPreviousSibling (&currentrow);
		  AddEmptyCellInRow (currentrow, colhead, doc);
	       }
	     currentrow = row;
	     while (currentrow != NULL)
	       {
		  TtaNextSibling (&currentrow);
		  AddEmptyCellInRow (currentrow, colhead, doc);
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
			    TtaNextSibling (&group);
			 }
		    }
		  TtaNextSibling (&child);
	       }
	  }
     }
   return colhead;
}

/*----------------------------------------------------------------------
   ShiftByRowSpan  returns the width of the cell that spans
   a given cell of a given row.
   Return 0 if the cell is not spanned by a cell above.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          ShiftByRowSpan (Element cell, Element row, Element colhead)
#else
static int          ShiftByRowSpan (cell, row, colhead)
Element             cell;
Element             row;
Element             colhead;

#endif
{
  int                 shift, rownb, span;
  Element             currow, curcell;
  AttributeType       attrType;
  ElementType         elType;
  Attribute           attr;
  char                name[50];
  Document            refdoc;

  shift = 0;
  if (cell == NULL)
    return (0);
  if (row == NULL)
    return (0);

  elType = TtaGetElementType (cell);
  attrType.AttrSSchema = elType.ElSSchema;
  if (colhead == NULL)
    {
      /* get the Column_head corresponding to the cell */
      attrType.AttrTypeNum = HTML_ATTR_Ref_column;
      attr = TtaGetAttribute (cell, attrType);
      if (attr != NULL)
	TtaGiveReferenceAttributeValue (attr, &colhead, name, &refdoc);
    }

  if (colhead != NULL)
    {
      currow = row;
      TtaPreviousSibling (&currow);
      rownb = 1;
      /* checks all rows above the row of interest */
      while (currow != NULL)
	{
	  elType = TtaGetElementType (currow);
	  /* treat only Table_row elements */
	  if (elType.ElTypeNum == HTML_EL_Table_row)
	    {
	      rownb++;
	      /* get the corresponding cell in that row */
	      curcell = GetCellFromColumnHead (currow, colhead);
	      if (curcell != NULL)
		{
		  attrType.AttrTypeNum = HTML_ATTR_rowspan_;
		  attr = TtaGetAttribute (curcell, attrType);
		  if (attr != NULL)
		    {
		      /* this cell has an attribute rowspan */
		      span = TtaGetAttributeValue (attr);
		      if (span >= rownb)
			{
			  /* this cells spans the row of interest */
			  shift = 1;
			  attrType.AttrTypeNum = HTML_ATTR_colspan_;
			  attr = TtaGetAttribute (curcell, attrType);
			  if (attr != NULL)
			    {
			      /* this cell also has an attribute colspan */
			      shift = TtaGetAttributeValue (attr);
			      if (shift < 1)
				shift = 1;
			    }
			}
		    }
		}
	    }
	  TtaPreviousSibling (&currow);
	}
    }
  return (shift);
}


/*----------------------------------------------------------------------
   CompleteRow                                             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         CompleteRow (Element row, Document doc, boolean rowspan)
#else
static void         CompleteRow (row, doc, rowspan)
Element             row;
Document            doc;
boolean             rowspan;

#endif
{
   Element             cell, lastcell, prevcell, colhead;
   ElementType         elType;
   AttributeType       attrType;
   Attribute           attr;
   int                 span, shift;
   char                name[50];
   Document            refdoc;

   elType = TtaGetElementType (row);
   attrType.AttrSSchema = elType.ElSSchema;
   /* process only Table_row elements */
   if (elType.ElTypeNum == HTML_EL_Table_row)
     {
	/* search the last cell in the Table_row */
	lastcell = NULL;
	cell = TtaGetLastChild (row);
	while (cell != NULL && lastcell == NULL)
	  {
	     elType = TtaGetElementType (cell);
	     /* process only cell elements */
	     if (elType.ElTypeNum == HTML_EL_Data_cell ||
		 elType.ElTypeNum == HTML_EL_Heading_cell)
		lastcell = cell;
	     else
		TtaPreviousSibling (&cell);
	  }
	if (lastcell != NULL)
	  {
	     /* search the Column_head that corresponds to the last cell */
	     attrType.AttrTypeNum = HTML_ATTR_Ref_column;
	     attr = TtaGetAttribute (lastcell, attrType);
	     if (attr != NULL)
	       {
		  TtaGiveReferenceAttributeValue (attr, &colhead, name, &refdoc);
		  if (colhead != NULL)
		    {
		       attrType.AttrTypeNum = HTML_ATTR_colspan_;
		       attr = TtaGetAttribute (lastcell, attrType);
		       span = 1;
		       if (attr != NULL)
			 {
			    /* the last cell has an attribute colspan */
			    span = TtaGetAttributeValue (attr);
			    if (span < 1)
			       span = 1;
			 }
		       while (span > 0 && colhead != NULL)
			 {
			    elType = TtaGetElementType (colhead);
			    if (elType.ElTypeNum == HTML_EL_Column_head)
			       span--;
			    TtaNextSibling (&colhead);
			 }
		       /* if it's not the last colhead, insert additional */
		       /* empty cell elements after the last cell in the row. */
		       while (colhead != NULL)
			 {
			    prevcell = lastcell;
			    if (rowspan)
			      {
				/* if some cell above the new cell has an attribute rowspan, */
				/* the new cell must be shifted on the right */
				shift = ShiftByRowSpan (lastcell, row, colhead);
				while (shift > 0 && colhead != NULL)
				  {
				    elType = TtaGetElementType (colhead);
				    if (elType.ElTypeNum == HTML_EL_Column_head)
				      shift--;
				    TtaNextSibling (&colhead);
				  }
			      }
			    if (colhead != NULL)
			      {
				lastcell = AddEmptyCellInRow (row, colhead, doc);
				TtaNextSibling (&colhead);
			      }
			    else
			      lastcell = NULL;
			 }
		    }
	       }
	  }
     }
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
   SetColExt       update attribute ColExt of cell                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         SetColExt (Element cell, Element * colhead, Document doc, int *span,
			       boolean all)
#else
static void         SetColExt (cell, colhead, doc, span, all)
Element             cell;
Element            *colhead;
Document            doc;
int                *span;
boolean             all;

#endif
{
   ElementType         elType;
   Element             prevcolhead, row;
   AttributeType       attrType;
   Attribute           attr;
   int                 ncol;

   elType = TtaGetElementType (cell);
   attrType.AttrSSchema = elType.ElSSchema;
   attrType.AttrTypeNum = HTML_ATTR_colspan_;
   attr = TtaGetAttribute (cell, attrType);
   *span = 1;
   if (attr != NULL)
      /* this cell has an attribute colspan */
     {
	*span = TtaGetAttributeValue (attr);
	if (*span < 1)
	   *span = 1;
	/* search the Column_head corresponding to the end of this */
	/* spannig cell */
	ncol = *span;
	row = TtaGetParent (cell);
	prevcolhead = *colhead;
	while (ncol > 1)
	  {
	     TtaNextSibling (colhead);
	     if (*colhead != NULL)
		prevcolhead = *colhead;
	     else
	       {
		  /* there is not enough Column_head for that cell */
		  /* Create an additional Column_head */
		  *colhead = NewColumnHead (prevcolhead, doc, all, row);
		  prevcolhead = *colhead;
	       }
	     elType = TtaGetElementType (*colhead);
	     if (elType.ElTypeNum == HTML_EL_Column_head)
		ncol--;
	  }
	/* set the attribute ColExt */
	attrType.AttrTypeNum = HTML_ATTR_ColExt;
	attr = TtaGetAttribute (cell, attrType);
	if (attr == NULL)
	  {
	     attr = TtaNewAttribute (attrType);
	     if (attr != NULL)
		TtaAttachAttribute (cell, attr, doc);
	  }
	if (attr != NULL)
	   TtaSetAttributeReference (attr, cell, doc, *colhead, doc);
     }
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
   FormatRow reformat an entire row.
   Return TRUE if new column element has been added.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean      FormatRow (Element row, Element firstcolhead, Document doc, boolean rowspan)
#else
static boolean      FormatRow (row, firstcolhead, doc, rowspan)
Element             row;
Element             firstcolhead;
Document            doc;
boolean             rowspan;
#endif
{
  Element             cell, colhead, prevcolhead;
  ElementType         elType;
  AttributeType       attrType;
  Attribute           attr;
  int                 colnumber, i, span, shift, rowspanshift;
  boolean             addCol;

  addCol = FALSE;
  if (row == NULL)
    return (addCol);
  if (firstcolhead == NULL)
    firstcolhead = GetFirstColumnHead (row);
  if (firstcolhead == NULL)
    return (addCol);

  elType = TtaGetElementType (row);
  attrType.AttrSSchema = elType.ElSSchema;
  /* process only Table_row elements */
  if (elType.ElTypeNum == HTML_EL_Table_row)
    {
      /* treat all cells in the row */
      cell = TtaGetFirstChild (row);
      colnumber = 1;
      prevcolhead = NULL;
      while (cell != NULL)
	{
	  elType = TtaGetElementType (cell);
	  /* process only cell elements */
	  if (elType.ElTypeNum == HTML_EL_Data_cell ||
	      elType.ElTypeNum == HTML_EL_Heading_cell)
	    {
	      /* searches the Column_head corresponding to that cell */
	      /* Create additional Column_heads if necessary */
	      i = colnumber;
	      prevcolhead = NULL;
	      while (i > 0)
		{
		  if (prevcolhead == NULL)
		    colhead = firstcolhead;
		  else
		    {
		      TtaNextSibling (&colhead);
		      if (colhead == NULL)
			{
			  /* there is no Column_head for that cell. Create one */
			  elType.ElTypeNum = HTML_EL_Column_head;
			  colhead = TtaNewTree (doc, elType, "");
			  if (colhead != NULL)
			    TtaInsertSibling (colhead, prevcolhead, FALSE, doc);
			  addCol = TRUE;
			}
		    }
		  prevcolhead = colhead;
		  elType = TtaGetElementType (colhead);
		  if (elType.ElTypeNum == HTML_EL_Column_head)
		    i--;
		}

	      if (rowspan)
		{
		  /* if some cell above the current cell has an attribute rowspan, */
		  /* the current cell must be shifted right */
		  do
		    {
		      rowspanshift = ShiftByRowSpan (cell, row, colhead);
		      colnumber += rowspanshift;
		      shift = rowspanshift;
		      while (shift > 0 && colhead != NULL)
			{
			  TtaNextSibling (&colhead);
			  if (colhead != NULL)
			    prevcolhead = colhead;
			  else
			    {
			      /* there is no Column_head for that cell */
			      /* Create an additional Column_head */
			      colhead = NewColumnHead (prevcolhead, doc, TRUE, row);
			      prevcolhead = colhead;
			    }
			  elType = TtaGetElementType (colhead);
			  if (elType.ElTypeNum == HTML_EL_Column_head)
			    shift--;
			}
		    }
		  while (rowspanshift > 0);
		}
	      /* relate the cell with its corresponding Column_head */
	      RelateCellWithColumnHead (cell, colhead, doc);
	      /* if there an attribute colspan for that cell, update
		 attribute ColExt */
	      SetColExt (cell, &colhead, doc, &span, rowspan);
	      colnumber += span;

	      /* is there an attribute rowspan for that cell ? */
	      attrType.AttrTypeNum = HTML_ATTR_rowspan_;
	      attr = TtaGetAttribute (cell, attrType);
	      if (attr == NULL)
		span = 1;
	      else
		{
		  span = TtaGetAttributeValue (attr);
		  if (span < 1)
		    span = 1;
		}
	      /* Set the attribute RowExt if row span > 1 */
	      if (span > 1)
		SetRowExt (cell, row, span, doc);
	    }
	  TtaNextSibling (&cell);
	}
    }
  return  (addCol);
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

/*----------------------------------------------------------------------
   ShiftCellsRight shift a given cell and all following cells      
   in the row by npos positions to the right.              
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ShiftCellsRight (Element theCell, Element row, int npos, boolean all, Document doc, Element firstcolhead)
#else
static void         ShiftCellsRight (theCell, row, npos, all, doc, firstcolhead)
Element             theCell;
Element             row;
int                 npos;
boolean             all;
Document            doc;
Element             firstcolhead;

#endif
{
   Element             colhead, prevcolhead, cell;
   ElementType         elType;
   AttributeType       attrType;
   Attribute           attr;
   char                name[50];
   Document            refdoc;
   int                 shift, rowspanshift, span, maxrowspanning;

   maxrowspanning = 0;
   cell = theCell;
   /* process the cell and all its following siblings */
   while (cell != NULL)
     {
	elType = TtaGetElementType (cell);
	/* Only cells have to be treated */
	if (elType.ElTypeNum == HTML_EL_Data_cell ||
	    elType.ElTypeNum == HTML_EL_Heading_cell)
	  {
	     attrType.AttrSSchema = elType.ElSSchema;
	     if (all)
	       {
		  /* does this cell have an attribute rowspan ? */
		  attrType.AttrTypeNum = HTML_ATTR_rowspan_;
		  attr = TtaGetAttribute (cell, attrType);
		  if (attr != NULL)
		    {
		       /* Yes. Remember that the following rows have to be reformatted */
		       span = TtaGetAttributeValue (attr);
		       if (span > maxrowspanning)
			  maxrowspanning = span;
		    }
	       }
	     maxrowspanning = MaximumRowSpan (row, maxrowspanning);
	     /* get the Column_head corresponding to the cell */
	     attrType.AttrTypeNum = HTML_ATTR_Ref_column;
	     attr = TtaGetAttribute (cell, attrType);
	     if (attr != NULL)
	       {
		  TtaGiveReferenceAttributeValue (attr, &colhead, name, &refdoc);
		  if (colhead != NULL)
		    {
		       prevcolhead = colhead;
		       shift = npos;
		       /* search the right Column_head for that cell */
		       while (shift > 0)
			 {
			    TtaNextSibling (&colhead);
			    if (colhead != NULL)
			       prevcolhead = colhead;
			    else
			      {
				 /* Create an additional Column_head */
				 colhead = NewColumnHead (prevcolhead, doc, all, row);
				 prevcolhead = colhead;
			      }
			    shift--;
			 }
		       if (all)
			 {
			    /* does an upper cell span the cell ? */
			    do
			      {
				 rowspanshift = ShiftByRowSpan (cell, row, colhead);
				 if (rowspanshift > 0)
				   {
				      shift = rowspanshift;
				      while (shift > 0)
					{
					   TtaNextSibling (&colhead);
					   if (colhead != NULL)
					      prevcolhead = colhead;
					   else
					      /* Create an additional Column_head */
					     {
						colhead = NewColumnHead (prevcolhead, doc, all, row);
						prevcolhead = colhead;
					     }
					   shift--;
					}
				   }
			      }
			    while (rowspanshift > 0);
			 }
		       if (cell != NULL)
			 {
			    RelateCellWithColumnHead (cell, colhead, doc);
			    /* if the cell has an attribute colspan, update its */
			    /* attribute ColExt */
			    SetColExt (cell, &colhead, doc, &shift, all);
			 }
		    }
	       }
	  }
	if (cell != NULL)
	   TtaNextSibling (&cell);
     }
   if (all && maxrowspanning > 1)
      /* reformat the next rows */
     {
	TtaNextSibling (&row);
	while (row != NULL && maxrowspanning > 1)
	  {
	     /* reformat one row */
	     CompleteRow (row, doc, TRUE);
	     TtaNextSibling (&row);
	     maxrowspanning--;
	  }
     }
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
  ElementType         elType, groupType;
  Element             el, columnHeads, colhead, thead, tfoot, firstcolhead,
                      tbody, Tablebody, firstgroup, group, row, prevrow,
                      spannedrow, foot, cell, spannedcell, prevEl, nextEl,
		      enclosingTable;
  AttributeType       attrType;
  Attribute           attr;
  Document            refdoc;
  char                name[20];
  int                 span, hspan;
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
	  
	  /* associate each cell with a column. Ignore attribute rowspan */
	  /* for the moment. Consider only attributes colspan */
	  group = firstgroup;
	  while (group != NULL)
	    {
	      groupType = TtaGetElementType (group);
	      if (groupType.ElTypeNum == HTML_EL_thead ||
		  groupType.ElTypeNum == HTML_EL_tfoot)
		row = TtaGetFirstChild (group);
	      else if (groupType.ElTypeNum == HTML_EL_Table_body)
		{
		  tbody = TtaGetFirstChild (group);
		  if (tbody == NULL)
		    row = NULL;
		  else
		    row = TtaGetFirstChild (tbody);
		}
	      else
		row = NULL;
	      while (row != NULL)
		{
		  FormatRow (row, firstcolhead, doc, FALSE);
		  TtaNextSibling (&row);
		  if (row == NULL)
		    if (groupType.ElTypeNum == HTML_EL_Table_body)
		      /* get the next tbody element */
		      {
			TtaNextSibling (&tbody);
			if (tbody != NULL)
			  row = TtaGetFirstChild (tbody);
		      }
		}
	      TtaNextSibling (&group);
	    }
	  
	  /* Process attribute rowspan of all cells in all rows */
	  group = firstgroup;
	  while (group != NULL)
	    {
	      groupType = TtaGetElementType (group);
	      if (groupType.ElTypeNum == HTML_EL_thead ||
		  groupType.ElTypeNum == HTML_EL_tfoot)
		row = TtaGetFirstChild (group);
	      else if (groupType.ElTypeNum == HTML_EL_Table_body)
		{
		  tbody = TtaGetFirstChild (group);
		  if (tbody == NULL)
		    row = NULL;
		  else
		    row = TtaGetFirstChild (tbody);
		}
	      else
		row = NULL;
	      
	      while (row != NULL)
		{
		  elType = TtaGetElementType (row);
		  /* process only Table_row elements */
		  if (elType.ElTypeNum == HTML_EL_Table_row)
		    {
		      /* treat all cells in the row */
		      cell = TtaGetFirstChild (row);
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
				/* this cell has an attribute rowspan */
				{
				  span = TtaGetAttributeValue (attr);
				  if (span > 1)
				    {
				      /* Assume that a spanning cell does not cross the */
				      /* CHKR_LIMIT of its row group */
				      /* search the Column_head that corresponds to that cell */
				      attrType.AttrTypeNum = HTML_ATTR_Ref_column;
				      attr = TtaGetAttribute (cell, attrType);
				      if (attr != NULL)
					{
					  TtaGiveReferenceAttributeValue (attr, &colhead, name,
									  &refdoc);
					  if (colhead != NULL)
					    {
					      /* process all spanned rows */
					      spannedrow = row;
					      while (span > 1 && spannedrow != NULL)
						{
						  TtaNextSibling (&spannedrow);
						  if (spannedrow == NULL)
						    elType.ElTypeNum = 0;
						  else
						    elType = TtaGetElementType (spannedrow);
						  /* process only Table_row elements */
						  if (elType.ElTypeNum == HTML_EL_Table_row)
						    {
						      /* get the cell of that spanned row that */
						      /* corresponds to the same Column_head */
						      if (spannedrow == NULL)
							spannedcell = NULL;
						      else
							spannedcell = GetCellFromColumnHead (spannedrow, colhead);
						      if (spannedcell != NULL)
							{
							  /* shift all following cells in the spanned row */
							  hspan = 1;
							  attrType.AttrTypeNum = HTML_ATTR_colspan_;
							  attr = TtaGetAttribute (cell, attrType);
							  if (attr != NULL)
							    {
							      hspan = TtaGetAttributeValue (attr);
							      if (hspan < 1)
								hspan = 1;
							    }
							  ShiftCellsRight (spannedcell, spannedrow, hspan, FALSE, doc, firstcolhead);
							}
						      span--;
						    }
						}
					    }
					}
				    }
				}
			    }
			  TtaNextSibling (&cell);
			}
		    }
		  TtaNextSibling (&row);
		  if (row == NULL)
		    if (groupType.ElTypeNum == HTML_EL_Table_body)
		      /* get the next tbody element */
		      {
			TtaNextSibling (&tbody);
			if (tbody != NULL)
			  row = TtaGetFirstChild (tbody);
		      }
		}
	      TtaNextSibling (&group);
	    }
	  
	  /* Add empty cell elements at the end of the rows that are */
	  /* not full */
	  group = firstgroup;
	  while (group != NULL)
	    {
	      groupType = TtaGetElementType (group);
	      if (groupType.ElTypeNum == HTML_EL_thead ||
		  groupType.ElTypeNum == HTML_EL_tfoot)
		row = TtaGetFirstChild (group);
	      else if (groupType.ElTypeNum == HTML_EL_Table_body)
		{
		  tbody = TtaGetFirstChild (group);
		  if (tbody == NULL)
		    row = NULL;
		  else
		    row = TtaGetFirstChild (tbody);
		}
	      else
		row = NULL;
	      while (row != NULL)
		{
		  CompleteRow (row, doc, FALSE);
		  TtaNextSibling (&row);
		  if (row == NULL)
		    if (groupType.ElTypeNum == HTML_EL_Table_body)
		      /* get the next tbody element */
		      {
			TtaNextSibling (&tbody);
			if (tbody != NULL)
			  row = TtaGetFirstChild (tbody);
		      }
		}
	      TtaNextSibling (&group);
	    }
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
   Element             newcell, prevcell, nextcell, row, colhead;
   Element             firstcolhead, prevcolhead;
   AttributeType       attrType;
   ElementType         elType;
   Attribute           attr;
   int                 colspan, shift, rowspanshift;
   char                name[50];
   Document            refdoc;
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

   firstcolhead = GetFirstColumnHead (cell);
   if (firstcolhead != NULL)
     {
       attrType.AttrSSchema = elType.ElSSchema;
       attrType.AttrTypeNum = HTML_ATTR_Ref_column;
       row = TtaGetParent (cell);
       prevcell = cell;
       prevcolhead = NULL;
       colhead = NULL;
       TtaPreviousSibling (&prevcell);
       if (prevcell == NULL)
	 colhead = firstcolhead;
       else
	 {
	   attr = TtaGetAttribute (prevcell, attrType);
	   if (attr != NULL)
	     {
	       TtaGiveReferenceAttributeValue (attr, &colhead, name, &refdoc);
	       if (colhead != NULL)
		 {
		   /* is there an attribute colspan for the previous cell ? */
		   attrType.AttrTypeNum = HTML_ATTR_colspan_;
		   attr = TtaGetAttribute (prevcell, attrType);
		   if (attr == NULL)
		     colspan = 1;
		   else
		     {
		       colspan = TtaGetAttributeValue (attr);
		       if (colspan < 1)
			 colspan = 1;
		     }
		   while (colspan > 0)
		     {
		       prevcolhead = colhead;
		       TtaNextSibling (&colhead);
		       if (colhead == NULL)
			 /* there is not enough Column_heads. Create one */
			 colhead = NewColumnHead (prevcolhead, doc, TRUE, row);
		       colspan--;
		     }
		 }
	     }
	 }

       if (colhead != NULL)
	 {
	   /* if some cell above the new cell has an attribute rowspan, the new cell */
	   /* must be shifted on the right */
	   rowspanshift = ShiftByRowSpan (cell, row, colhead);
	   shift = rowspanshift;
	   while (shift > 0)
	     {
	       prevcolhead = colhead;
	       TtaNextSibling (&colhead);
	       if (colhead == NULL)
		 /* there is not enough Column_heads. Create one */
		 colhead = NewColumnHead (prevcolhead, doc, TRUE, row);
	       shift--;
	     }
	   /* relate the new cell with its Column_head */
	   RelateCellWithColumnHead (cell, colhead, doc);
	   /* is there an attribute colspan for that new cell ? */
	   attrType.AttrTypeNum = HTML_ATTR_colspan_;
	   attr = TtaGetAttribute (cell, attrType);
	   if (attr == NULL)
	     colspan = 1;
	   else
	     {
	       colspan = TtaGetAttributeValue (attr);
	       if (colspan < 1)
		 colspan = 1;
	     }
	   /* shift all following cells to the right */
	   nextcell = cell;
	   TtaNextSibling (&nextcell);
	   if (nextcell != NULL)
	     ShiftCellsRight (nextcell, row, colspan, TRUE, doc, firstcolhead);
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
   ElementType         elType;
   AttributeType       attrType;
   Element             cell, nextcell, row, colhead;
   Attribute           attr;
   Document            doc, refdoc;
   char                name[50];
   int                 span;

   cell = event->element;
   doc = event->document;
   row = TtaGetParent (cell);
   if (row != CurrentPastedRow)
      /* a single cell has been pasted */
     {
	NewCell (cell, doc);
	/* search the Column_head that corresponds to the pasted cell */
	elType = TtaGetElementType (row);
	attrType.AttrSSchema = elType.ElSSchema;
	attrType.AttrTypeNum = HTML_ATTR_Ref_column;
	attr = TtaGetAttribute (cell, attrType);
	if (attr != NULL)
	  {
	     TtaGiveReferenceAttributeValue (attr, &colhead, name, &refdoc);
	     if (colhead != NULL)
		/* set the attribute ColExt if the cell has an attribute colspan */
		SetColExt (cell, &colhead, doc, &span, TRUE);
	  }
	attrType.AttrTypeNum = HTML_ATTR_rowspan_;
	attr = TtaGetAttribute (cell, attrType);
	if (attr != NULL)
	   /* this cell has an attribute rowspan */
	  {
	     span = TtaGetAttributeValue (attr);
	     if (span > 1)
	       {
		  span = MaximumRowSpan (row, span);
		  /* Set the attribute RowExt of the pasted cell */
		  SetRowExt (cell, row, span, doc);
		  /* format the rows spanned by the pasted cell */
		  do
		    {
		       TtaNextSibling (&row);
		       if (row != NULL)
			 {
			    elType = TtaGetElementType (row);
			    if (elType.ElTypeNum == HTML_EL_Table_row)
			      {
				 FormatRow (row, NULL, doc, TRUE);
				 CompleteRow (row, doc, TRUE);
				 span--;
			      }
			 }
		    }
		  while (row != NULL && span > 1);
	       }
	  }
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


/*----------------------------------------------------------------------
   UpdateRowExt                                            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                UpdateRowExt (Element row, Document doc, boolean NewRow)
#else
void                UpdateRowExt (row, doc, NewRow)
Element             row;
Document            doc;
boolean             NewRow;

#endif
{
   Element             currentrow, cell, oldrow, spannedrow;
   ElementType         elType;
   AttributeType       attrType;
   Attribute           attr;
   int                 rownb, span;
   Document            refdoc;
   char                name[50];

   if (row == NULL)
      return;
   /* search in previous rows the cells that span the new row */
   currentrow = row;
   TtaPreviousSibling (&currentrow);
   rownb = 1;
   /* check all rows above the row of interest */
   while (currentrow != NULL)
     {
	elType = TtaGetElementType (currentrow);
	/* treat only Table_row elements */
	if (elType.ElTypeNum == HTML_EL_Table_row)
	  {
	     rownb++;
	     /* get the first cell in that row */
	     cell = TtaGetFirstChild (currentrow);
	     /* treat all cells in the row */
	     while (cell != NULL)
	       {
		  elType = TtaGetElementType (cell);
		  /* process only cell elements */
		  if (elType.ElTypeNum == HTML_EL_Data_cell ||
		      elType.ElTypeNum == HTML_EL_Heading_cell)
		    {
		       attrType.AttrSSchema = elType.ElSSchema;
		       attrType.AttrTypeNum = HTML_ATTR_rowspan_;
		       attr = TtaGetAttribute (cell, attrType);
		       if (attr != NULL)
			  /* this cell has an attribute rowspan */
			 {
			    span = TtaGetAttributeValue (attr);
			    if (span >= rownb)
			       /* this cell spans the new row */
			      {
				 if (NewRow)
				   {
				      attrType.AttrTypeNum = HTML_ATTR_RowExt;
				      attr = TtaGetAttribute (cell, attrType);
				      if (attr != NULL)
					{
					   TtaGiveReferenceAttributeValue (attr, &oldrow, name, &refdoc);
					   if (oldrow != NULL)
					     {
						FormatRow (oldrow, NULL, doc, TRUE);
						CompleteRow (oldrow, doc, TRUE);
					     }
					}
				   }
				 spannedrow = SetRowExt (cell, currentrow, span, doc);
				 if (!NewRow)
				   {
				      FormatRow (spannedrow, NULL, doc, TRUE);
				      CompleteRow (spannedrow, doc, TRUE);
				   }
			      }
			 }
		    }
		  TtaNextSibling (&cell);
	       }
	  }
	TtaPreviousSibling (&currentrow);
     }
}

static int          maxrowspan;

/*----------------------------------------------------------------------
   PreDeleteRow                                            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                PreDeleteRow (Element row)
#else
void                PreDeleteRow (row)
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
   PostDeleteRow                                           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                PostDeleteRow (Element row, Element table, Document doc)
#else
void                PostDeleteRow (row, table, doc)
Element             row;
Element             table;
Document            doc;

#endif
{
   ElementType         elType;

   /* update the RowExt attribute of revious rows */
   UpdateRowExt (row, doc, FALSE);
   /* reformat and complete the following rows that were spanned by the */
   /* deleted row */
   while (row != NULL && maxrowspan > 1)
     {
	elType = TtaGetElementType (row);
	if (elType.ElTypeNum == HTML_EL_Table_row)
	  {
	     FormatRow (row, NULL, doc, TRUE);
	     CompleteRow (row, doc, TRUE);
	     maxrowspan--;
	  }
	TtaNextSibling (&row);
     }
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
   Element             row, rowgroup, table;
   ElementType         elType;
   Document            doc;
   int                 rank;

   rowgroup = event->element;
   if (rowgroup == NULL)
      return;
   elType = TtaGetElementType (rowgroup);
   elType.ElTypeNum = HTML_EL_Table;
   table = TtaGetTypedAncestor (rowgroup, elType);
   doc = event->document;
   row = TtaGetFirstChild (rowgroup);
   rank = event->position;
   /* get the first row following the deleted row */
   while (rank > 0 && row != NULL)
     {
	TtaNextSibling (&row);
	rank--;
     }
   PostDeleteRow (row, table, doc);
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
   CheckRow                                             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         CheckRow (Element row, Document doc)
#else
static void         CheckRow (row, doc)
Element             row;
Document            doc;
#endif
{
  Element             currentrow, nextrow, prevrow;
  Element             tbody, cell, table;
  ElementType         elType;
  int                 span;
  boolean             empty;

  if (row == NULL)
    return;
  /* reformat all rows that were spanned by the deleted cell */
  currentrow = row;
  span = MaximumRowSpan (row, maxcellspan);
  while (currentrow != NULL && span > 0)
    {
      elType = TtaGetElementType (currentrow);
      if (elType.ElTypeNum == HTML_EL_Table_row)
	{
	  FormatRow (currentrow, NULL, doc, TRUE);
	  CompleteRow (currentrow, doc, TRUE);
	  span--;
	}
      TtaNextSibling (&currentrow);
    }

  elType = TtaGetElementType (row);
  elType.ElTypeNum = HTML_EL_Table;
  table = TtaGetTypedAncestor (row, elType);
  /* if the row does not contain any cell, delete the row */
  empty = TRUE;
  cell = TtaGetFirstChild (row);
  while (empty && cell != NULL)
    {
      elType = TtaGetElementType (cell);
      if (elType.ElTypeNum == HTML_EL_Data_cell ||
	  elType.ElTypeNum == HTML_EL_Heading_cell)
	empty = FALSE;
      else
	TtaNextSibling (&cell);
    }
  if (empty)
    {
      nextrow = row;
      TtaNextSibling (&nextrow);
      prevrow = row;
      TtaPreviousSibling (&prevrow);
      if (prevrow == NULL && nextrow == NULL)
	/* it's the only row in the tbody element. Delete the tbody */
	{
	  tbody = TtaGetParent(row);
	  TtaDeleteTree (tbody, doc);
	}
      else
	{
	  PreDeleteRow (row);
	  TtaDeleteTree (row, doc);
	  row = NULL;
	  PostDeleteRow (nextrow, table, doc);
	}
    }

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
static boolean      RemoveEmptyColumn (colhead, table, doc)
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
  Element           colhead, next;
  boolean           removed;

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
	  span--;
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
  Document          doc;
  boolean           removed;

  doc = event->document;
  CheckRow (event->element, doc);
  removed = RemoveEmptyColumn (CurrentColumn, doc);
  if (removed)
    CurrentSpan--;

  /* remove extra columns if needed */
  RemoveExtraColumns (event->element, CurrentSpan, doc);
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
   CompleteRow (row, doc, TRUE);
   UpdateRowExt (row, doc, TRUE);
   /* avoid processing the cells of the created row */
   CurrentCreatedRow = row;
}


/*----------------------------------------------------------------------
   CompleteOtherRows in the table to which row belongs, add  
   empty cell elements where needed.   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         CompleteOtherRows (Element currentrow, Document doc)
#else
static void         CompleteOtherRows (currentrow, doc)
Element             currentrow;
Document            doc;
#endif
{
  ElementType         elType;
  Element             table, group, row;

  elType = TtaGetElementType (currentrow);
  elType.ElTypeNum = HTML_EL_Table;
  table = TtaGetTypedAncestor (currentrow, elType);
  if (table != NULL)
    {
      elType.ElTypeNum = HTML_EL_Table_row;
      row = TtaSearchTypedElement (elType, SearchForward, table);
      /* add empty cells at the end of the other rows, if necessary */
      group = TtaGetParent (row);
      while (row != NULL)
	{
	  if (row != currentrow)
	    CompleteRow (row, doc, TRUE);
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
   Element             row, currentrow, cell;
   ElementType         elType;
   AttributeType       attrType;
   Attribute           attr;
   Document            doc;
   int                 span, maxspan;
   boolean             addCol;

   row = event->element;
   doc = event->document;
   elType = TtaGetElementType (row);
   attrType.AttrSSchema = elType.ElSSchema;
   attrType.AttrTypeNum = HTML_ATTR_rowspan_;
   /* looks for the cell having the greatest rowspan */
   cell = TtaGetFirstChild (row);
   maxspan = 0;
   while (cell != NULL)
     {
	attr = TtaGetAttribute (cell, attrType);
	if (attr != NULL)
	   /* this cell has an attribute rowspan */
	  {
	     span = TtaGetAttributeValue (attr);
	     if (span > maxspan)
		maxspan = span;
	  }
	TtaNextSibling (&cell);
     }
   maxspan = MaximumRowSpan (row, maxspan);
   addCol = FormatRow (row, NULL, doc, TRUE);
   /* update RowExt attributes in the rows above */
   UpdateRowExt (row, doc, TRUE);
   /* format the following rows that are spanned */
   currentrow = row;
   do
     {
	TtaNextSibling (&currentrow);
	if (currentrow != NULL)
	  {
	     elType = TtaGetElementType (currentrow);
	     if (elType.ElTypeNum == HTML_EL_Data_cell ||
		 elType.ElTypeNum == HTML_EL_Heading_cell)
	       {
		  if (maxspan > 1)
		    {
		      FormatRow (currentrow, NULL, doc, TRUE);
		      CompleteRow (currentrow, doc, TRUE);
		    }
		  maxspan--;
	       }
	  }
     }
   while (currentrow != NULL);

   /* add empty cells at the end */
   if (addCol)
     CompleteOtherRows (row, doc);
   else
     CompleteRow (row, doc, TRUE);

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
   Element             nextcell, row, currentrow, colhead;
   ElementType         elType;
   AttributeType       attrType;
   Attribute           attr;
   Document            refdoc;
   char                name[50];
   int                 span, maxrowspan, shift;

   if (cell == NULL)
      return;
   elType = TtaGetElementType (cell);
   attrType.AttrSSchema = elType.ElSSchema;
   if (newspan <= 1)
      /* remove the attribute ColExt if it is present */
     {
	attrType.AttrTypeNum = HTML_ATTR_ColExt;
	attr = TtaGetAttribute (cell, attrType);
	if (attr != NULL)
	   TtaRemoveAttribute (cell, attr, doc);
     }
   else
     {
	/* Search the Column_head corresponding to the cell */
	attrType.AttrTypeNum = HTML_ATTR_Ref_column;
	attr = TtaGetAttribute (cell, attrType);
	if (attr != NULL)
	  {
	     TtaGiveReferenceAttributeValue (attr, &colhead, name, &refdoc);
	     if (colhead != NULL)
		/* set the attribute ColExt */
	       {
		  span = newspan;
		  SetColExt (cell, &colhead, doc, &span, TRUE);
	       }
	  }
     }
   row = TtaGetParent (cell);
   maxrowspan = MaxRowSpanNextCells (cell);
   /*maxrowspan = MaximumRowSpan (row, maxrowspan);*/
   if (newspan > oldspan)
     {
	/* shift all following cells to the right */
	shift = newspan - oldspan;
	nextcell = cell;
	TtaNextSibling (&nextcell);
	if (nextcell != NULL)
	   if (row != NULL)
	      ShiftCellsRight (nextcell, row, shift, TRUE, doc, NULL);
	/* the current row does not need to be reformatted */
	currentrow = row;
	TtaNextSibling (&currentrow);
	maxrowspan--;
     }
   else
      currentrow = row;
   /* reformat all rows that are spanned by the cell and its following siblings */
   while (currentrow != NULL && maxrowspan > 0)
     {
	elType = TtaGetElementType (currentrow);
	if (elType.ElTypeNum == HTML_EL_Table_row)
	  {
	     FormatRow (currentrow, NULL, doc, TRUE);
	     CompleteRow (currentrow, doc, TRUE);
	     maxrowspan--;
	  }
	TtaNextSibling (&currentrow);
     }
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
   Element             row, currentrow;
   ElementType         elType;
   AttributeType       attrType;
   Attribute           attr;
   int                 span, minspan, maxspan;

   if (oldspan == newspan)
      return;
   if (cell == NULL)
      return;
   elType = TtaGetElementType (cell);
   attrType.AttrSSchema = elType.ElSSchema;
   row = TtaGetParent (cell);
   if (row == NULL)
      return;
   if (newspan <= 1)
      /* remove the attribute RowExt if it is present */
     {
	attrType.AttrTypeNum = HTML_ATTR_RowExt;
	attr = TtaGetAttribute (cell, attrType);
	if (attr != NULL)
	   TtaRemoveAttribute (cell, attr, doc);
     }
   else
     {
	/* Set the attribute RowExt of the cell */
	SetRowExt (cell, row, newspan, doc);
     }
   if (newspan > oldspan)
     {
	maxspan = newspan;
	minspan = oldspan;
     }
   else
     {
	maxspan = oldspan;
	minspan = newspan;
     }
   span = minspan;
   while (span > 0 && row != NULL)
     {
	elType = TtaGetElementType (row);
	if (elType.ElTypeNum == HTML_EL_Table_row)
	   span--;
	TtaNextSibling (&row);
     }
   /* format the rows spanned by the cell */
   span = maxspan - minspan;
   span = MaximumRowSpan (row, span);
   currentrow = row;
   while (currentrow != NULL && span > 0)
     {
	elType = TtaGetElementType (currentrow);
	if (elType.ElTypeNum == HTML_EL_Table_row)
	  {
	     FormatRow (currentrow, NULL, doc, TRUE);
	     CompleteRow (currentrow, doc, TRUE);
	     span--;
	  }
	TtaNextSibling (&currentrow);
     }
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
