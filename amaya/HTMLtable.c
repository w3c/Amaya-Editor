/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *
 * Author: V. Quint
 */

/* Amaya includes  */
#define THOT_EXPORT extern
#include "amaya.h"

static Element      CurrentPastedRow = NULL;
static Element      CurrentCreatedRow = NULL;


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
	    elType.ElTypeNum == HTML_EL_Heading_cell ||
	    elType.ElTypeNum == HTML_EL_Table_cell_ghost)
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
   UpdateColHeadWidth

   update attribute Col_width_pxl (width of column in pixels)  or
   Col_width_percent (width of column in percentage) of element
   colhead according to the width of a given cell.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         UpdateColHeadWidth (Element colhead, Element cell,
					Document doc)
#else
static void         UpdateColHeadWidth (colhead, cell, doc)
Element             colhead;
Element             cell;
Document            doc;

#endif
{
   ElementType	       elType;
   Element	       child, leaf;
   AttributeType       attrType;
   Attribute           attrCellWidth, attrColWidth, attrColspan, attrPictWidth;
   int                 max, length, cellWidth;
   char		       *attrVal;

   cellWidth = 0;
   elType = TtaGetElementType (cell);
   attrType.AttrSSchema = elType.ElSSchema;
   attrType.AttrTypeNum = HTML_ATTR_colspan;
   attrColspan = TtaGetAttribute (cell, attrType);
   if (attrColspan != NULL)
      /* the cell has an attribute colspan, ignore its width if colspan > 1 */
      if (TtaGetAttributeValue (attrColspan) > 1)
         return;
   /* look for an attribute Cell_width (width, in HTML) */
   attrType.AttrTypeNum = HTML_ATTR_Cell_width;
   attrCellWidth = TtaGetAttribute (cell, attrType);
   if (attrCellWidth != NULL)
      /* this cell has an attribute Cell_width */
      {
      /* get the value of that attribute */
      length = TtaGetTextAttributeLength (attrCellWidth);
      attrVal = TtaGetMemory (length + 1);
      TtaGiveTextAttributeValue (attrCellWidth, attrVal, &length);
      /* is the last character a '%' ? */
      while (length > 0 && attrVal[length] <= ' ')
         length --;
      if (attrVal[length] == '%')
	 attrType.AttrTypeNum = HTML_ATTR_Col_width_percent;
      else
	 attrType.AttrTypeNum = HTML_ATTR_Col_width_pxl;
      sscanf (attrVal, "%d", &cellWidth);
      TtaFreeMemory (attrVal);
      }
   else
      /* no attribute Cell_width on the cell */
      /* if the cell contains only an <IMG> with an attribute WIDTH, the */
      /* cell takes that width */
      {
      child = cell;
      while (child != NULL)
	{
	leaf = child;
        child = TtaGetFirstChild (child);
	}
      elType = TtaGetElementType (leaf);
      if (elType.ElTypeNum == HTML_EL_PICTURE_UNIT)
	/* the first leaf in the cell is a picture */
        if (TtaGetElementVolume (leaf) == TtaGetElementVolume (cell))
	  /* the cell has the same volume as the picture */
	  {
	  attrType.AttrTypeNum = HTML_ATTR_Width_;
	  attrPictWidth = TtaGetAttribute (leaf, attrType);
	  if (attrPictWidth != NULL)
	     /* the picture has an attribute WIDTH */
	     {
	     cellWidth = TtaGetAttributeValue (attrPictWidth);
	     cellWidth += 6;
	     attrType.AttrTypeNum = HTML_ATTR_Col_width_pxl;
	     }
	  }
      }
   if (cellWidth > 0)
      {
      attrColWidth = TtaGetAttribute (colhead, attrType);
      if (attrColWidth != NULL)
	/* the Column_head has this attribute, get its value */
	max = TtaGetAttributeValue (attrColWidth);
      else
	/* create an attribute for the Column_head */
	{
	attrColWidth = TtaNewAttribute (attrType);
	TtaAttachAttribute (colhead, attrColWidth, doc);
	max = 0;
	}
      /* set value of the attribute */
      if (cellWidth > max)
         TtaSetAttributeValue (attrColWidth, cellWidth, colhead, doc);
      }
}

/*----------------------------------------------------------------------
   UpdateColHeadVol

   updates attributes Col_width_pxl (width of column in pixels) and/or
   IntMaxVol (volume of biggest cell in the column) of element colhead
   according to the width and/or volume of a given cell.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         UpdateColHeadVol (Element colhead, Element cell,
				      Document doc)
#else
static void         UpdateColHeadVol (colhead, cell, doc)
Element             colhead;
Element             cell;
Document            doc;

#endif
{
   ElementType	       elType;
   AttributeType       attrType;
   Attribute           attr, attrIntMaxVol;
   int                 vol, max, span, i;

   /* update attribute IntMaxVol of Column_head if the volume of the cell is
      greater than the current value of IntMaxVol */
   elType = TtaGetElementType (cell);
   attrType.AttrSSchema = elType.ElSSchema;
   attrType.AttrTypeNum = HTML_ATTR_IntMaxVol;
   attrIntMaxVol = TtaGetAttribute (colhead, attrType);
   if (attrIntMaxVol != NULL)
	{
	   max = TtaGetAttributeValue (attrIntMaxVol);
	   vol = TtaGetElementVolume (cell);
	   /* when loading a document, images are not loaded when */
	   /* this procedure is executed: their volume is 0 */
	   if (vol == 0)
	     {
		/* does this cell only contain an image element? */
		if (TtaGetFirstChild (cell) != NULL)
		   /* assume vol = 30 */
		   vol = 30;
	     }
	   else
	     {
		attrType.AttrTypeNum = HTML_ATTR_rowspan;
		attr = TtaGetAttribute (cell, attrType);
		if (attr != NULL)
		  {
		     /* the cell has an attribute rowspan */
		     span = TtaGetAttributeValue (attr);
		  }
		attrType.AttrTypeNum = HTML_ATTR_colspan;
		attr = TtaGetAttribute (cell, attrType);
		if (attr != NULL)
		  {
		     /* the cell has an attribute colspan */
		     span = TtaGetAttributeValue (attr);
		     vol = vol / span;
		     for (i = 1; i < span; i++)
		       {
			  if (vol > max)
			     TtaSetAttributeValue (attrIntMaxVol, vol, colhead,
						   doc);
			  TtaNextSibling (&colhead);
			  attrType.AttrTypeNum = HTML_ATTR_IntMaxVol;
			  attrIntMaxVol = TtaGetAttribute (colhead, attrType);
		       }
		  }
	     }
	   if (vol > max)
	      TtaSetAttributeValue (attrIntMaxVol, vol, colhead, doc);
	}
}


/*----------------------------------------------------------------------
   SetColumnWidth

   computes the value of attributes Col_width_percent, Col_width_pxl
   or Col_width_delta for each column of the table whose first Column_head
   is firstcolhead.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         SetColumnWidth (Element firstcolhead, Element newcolhead,
				    Document doc)
#else
static void         SetColumnWidth (firstcolhead, newcolhead, doc)
Element             firstcolhead;
Element             newcolhead;
Document            doc;

#endif
{
   Element             el, group, row, tbody, colhead, cell, freecolhead;
   ElementType         elType, groupType;
   AttributeType       attrType;
   Attribute           attrMaxVol, attrRefCol, attrColWidthPcent,
		       attrColWidthPxl, attrColWidthDelta;
   int                 vol, volnew, total, nEmptyCols, nColumns, nFreeCols,
                       volemptycol, colwidthpercent, totalpercent,
		       totalfixedwidth;
   char                name[50];
   Document            refdoc;

#ifndef STANDALONE
   DisplayMode         dispMode;

   dispMode = TtaGetDisplayMode (doc);
   if (dispMode == DisplayImmediately)
     TtaSetDisplayMode (doc, DeferredDisplay);
#endif
   /* Create an attribute IntVolMax for each Column_head if it does not exist*/
   /* This attribute will contain the volume (number of characters) of the */
   /* biggest cell in the corresponding column */
   /* Initialize all IntVolMax attributes to 0 */
   elType = TtaGetElementType (firstcolhead);
   attrType.AttrSSchema = elType.ElSSchema;
   colhead = firstcolhead;
   while (colhead != NULL)
     {
	/* remove attribute Col_width_percent if it exists */
	attrType.AttrTypeNum = HTML_ATTR_Col_width_percent;
	attrColWidthPcent = TtaGetAttribute (colhead, attrType);
	if (attrColWidthPcent != NULL)
	   TtaRemoveAttribute (colhead, attrColWidthPcent, doc);
	/* remove attribute Col_width_delta if it exists */
	attrType.AttrTypeNum = HTML_ATTR_Col_width_delta;
	attrColWidthDelta = TtaGetAttribute (colhead, attrType);
	if (attrColWidthDelta != NULL)
	   TtaRemoveAttribute (colhead, attrColWidthDelta, doc);
	/* create attribute IntVolMax if it does not exist */
	attrType.AttrTypeNum = HTML_ATTR_IntMaxVol;
	attrMaxVol = TtaGetAttribute (colhead, attrType);
	if (attrMaxVol == NULL)
	  {
	     /* There is no IntVolMax attribute on that Column_head.
		Create one */
	     attrMaxVol = TtaNewAttribute (attrType);
	     TtaAttachAttribute (colhead, attrMaxVol, doc);
	  }
	/* set initial value of IntVolMax to 0 */
	TtaSetAttributeValue (attrMaxVol, 0, colhead, doc);
	/* next Column_head */
	TtaNextSibling (&colhead);
     }

   /* process all cells in the table and looks for the cell having the */
   /* largest volume and/or largest width in each column. */
   /* For each column, store the maximum volume in attribute IntMaxVol and */
   /* the maximum width in attribute Col_width_pxl */
   el = TtaGetParent (firstcolhead);
   if (el != NULL)
     {
	TtaNextSibling (&el);
	group = el;
	/* process all row groups: thead, tbody, tfoot */
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
	     /* process all rows in a group */
	     while (row != NULL)
	       {
		  elType = TtaGetElementType (row);
		  if (elType.ElTypeNum == HTML_EL_Table_row)
		    {
		       cell = TtaGetFirstChild (row);
		       /* process all cells in a row */
		       while (cell != NULL)
			 {
			    elType = TtaGetElementType (cell);
			    if (elType.ElTypeNum == HTML_EL_Data_cell ||
				elType.ElTypeNum == HTML_EL_Heading_cell)
			      {
				 /* get the Column_head corresponding to that
				    cell, using the Ref_column attribute */
				 colhead = NULL;
				 attrType.AttrTypeNum = HTML_ATTR_Ref_column;
				 attrRefCol = TtaGetAttribute (cell, attrType);
				 if (attrRefCol != NULL)
				    TtaGiveReferenceAttributeValue (attrRefCol,
						      &colhead, name, &refdoc);
				 /* if this cell is bigger than the current
				 biggest cell in that column, update the
				 attributes of the corresponding Column_head */
				 if (colhead != NULL)
				   {
				   UpdateColHeadWidth (colhead, cell, doc);
				   UpdateColHeadVol (colhead, cell, doc);
				   }
			      }
			    /* get the next cell in the current row */
			    TtaNextSibling (&cell);
			 }
		    }
		  /* get the next row in the current group */
		  TtaNextSibling (&row);
		  if (row == NULL)
		     if (groupType.ElTypeNum == HTML_EL_Table_body)
		       {
			  /* get the next tbody element */
			  TtaNextSibling (&tbody);
			  if (tbody != NULL)
			     row = TtaGetFirstChild (tbody);
		       }
	       }
	     /* get the next row group in the table */
	     TtaNextSibling (&group);
	  }
	/* the size of the largest cell in each column is known (attributes */
	/* IntMaxVol and Col_width_pxl). Compute the relative width */
	/* (attribute Col_width_percent) of each column */
	/* compute first the number of columns (nColumns) and the sum of */
	/* maximum volumes (total) */
	total = 0;
	nColumns = 0;
	nEmptyCols = 0;
	totalpercent = 0;
	totalfixedwidth = 0;
	nFreeCols = 0;
	freecolhead = NULL;
	colhead = firstcolhead;
	while (colhead != NULL)
	  {
	     attrType.AttrTypeNum = HTML_ATTR_Col_width_percent;
	     attrColWidthPcent = TtaGetAttribute (colhead, attrType);
	     attrType.AttrTypeNum = HTML_ATTR_Col_width_pxl;
	     attrColWidthPxl = TtaGetAttribute (colhead, attrType);
	     if (attrColWidthPxl == NULL)
	        /* the column has no attribute Col_width_pxl */
		{
		if (nFreeCols == 0)
		   freecolhead = colhead;
		/* count the number of column heads without fixed width */
		nFreeCols++;
		}
	     else
	        /* the column has an attribute Col_width_pxl, remove attribute
		   Col_width_percent */
		{
		totalfixedwidth += TtaGetAttributeValue (attrColWidthPxl);
		if (attrColWidthPcent != NULL)
		   {
		   TtaRemoveAttribute (colhead, attrColWidthPcent, doc);
		   attrColWidthPcent = NULL;
		   }
		}
	     if (attrColWidthPcent != NULL)
		/* this column already has a %age width */
		totalpercent += TtaGetAttributeValue (attrColWidthPcent);
	     else
		{
	        nColumns++;
	        attrType.AttrTypeNum = HTML_ATTR_IntMaxVol;
	        attrMaxVol = TtaGetAttribute (colhead, attrType);
	        if (attrMaxVol != NULL)
	          {
		  vol = TtaGetAttributeValue (attrMaxVol);
		  if (vol == 0)
		     nEmptyCols++;
		  else
		     total += vol;
	          }
		}
	     /* next Column_head */
	     TtaNextSibling (&colhead);
	  }
	if (nFreeCols == 1)
	  /* all columns except one have a fixed width */
	  /* the only free column will take the remaining space */
	  {
	    attrType.AttrTypeNum = HTML_ATTR_Col_width_delta;
	    attrColWidthDelta = TtaNewAttribute (attrType);
	    TtaAttachAttribute (freecolhead, attrColWidthDelta, doc);
	    TtaSetAttributeValue (attrColWidthDelta, totalfixedwidth,
				  freecolhead, doc);
	  }
	else
	  {
	  if (nEmptyCols > 0)
	     /* there are some empty columns */
	     /* assign them 1/40 of the Table width */
	    {
	     volemptycol = total / 40;
	     total += volemptycol * nEmptyCols;
	    }
	  if (newcolhead != NULL)
	    {
	     /* a new column has been created by the user. It's empty but */
	     /* its width should not be 0.  An average width is assigned to */
	     /* that new column. (The new column is counted in nColumns). */
	     if (nColumns > 2)
		volnew = total / (nColumns - 1);
	     else
		volnew = total;
	     total += volnew;
	    }
	  /* set the relative width (% of table width) of each Column_head */
	  colhead = firstcolhead;
	  colwidthpercent = 100;
	  if (total == 0 && nColumns > 0)
	     /* empty table. All columns will get the same width. */
	     colwidthpercent = 100 / nColumns;
	  while (colhead != NULL)
	    {
	     /* get attributes Col_width_percent and Col_width_pxl for that
		Column_head */
	     attrType.AttrTypeNum = HTML_ATTR_Col_width_pxl;
	     attrColWidthPxl = TtaGetAttribute (colhead, attrType);
	     if (attrColWidthPxl == NULL)
		{
	          attrType.AttrTypeNum = HTML_ATTR_Col_width_percent;
	          attrColWidthPcent = TtaGetAttribute (colhead, attrType);
	          if (attrColWidthPcent == NULL)
		    /* there is no attribute Col_width_pxl
		       nor Col_width_percent.
		       Compute the value of attribute Col_width_percent */
		    {
	            attrType.AttrTypeNum = HTML_ATTR_IntMaxVol;
	            attrMaxVol = TtaGetAttribute (colhead, attrType);
	            if (attrMaxVol != NULL)
	              {
		      if (total != 0)
		        {
		         if (colhead == newcolhead)
			    /* it's the new (empty) column */
			    vol = volnew;
		         else
			   {
			    vol = TtaGetAttributeValue (attrMaxVol);
			    if (vol == 0)
			       vol = volemptycol;
			   }
		         colwidthpercent = ((100 - totalpercent) * vol) /total;
		        }
		      attrType.AttrTypeNum = HTML_ATTR_Col_width_percent;
		      attrColWidthPcent = TtaNewAttribute (attrType);
		      TtaAttachAttribute (colhead, attrColWidthPcent, doc);
		      TtaSetAttributeValue (attrColWidthPcent, colwidthpercent,
					    colhead, doc);
	              }
		    }
		}
	     /* next Column_head */
	     TtaNextSibling (&colhead);
	    }
	  }
     }
#ifndef STANDALONE
   TtaSetDisplayMode (doc, dispMode);
#endif
}

/*----------------------------------------------------------------------
   RelateCellWithColumnHead        relates a cell with a           
   Column_head.                                            
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
   AddCellGhostInRow       add a Table_cell_ghost element at the   
   end of the given row and relates it to the given Column_head.   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         AddCellGhostInRow (Element row, Element colhead, Document doc)
#else
static void         AddCellGhostInRow (row, colhead, doc)
Element             row;
Element             colhead;
Document            doc;

#endif
{
   Element             lastcell, ghost;
   ElementType         elType;
   int                 PreviousStuctureChecking;

   if (row == NULL)
      return;
   elType = TtaGetElementType (row);
   if (elType.ElTypeNum != HTML_EL_Table_row)
      return;
   lastcell = TtaGetLastChild (row);
   if (lastcell != NULL)
     {
	elType.ElTypeNum = HTML_EL_Table_cell_ghost;
	ghost = TtaNewTree (doc, elType, "");
	if (ghost != NULL)
	  {
	     /* don't check the abstract tree against the structure schema */
	     PreviousStuctureChecking = TtaGetStructureChecking (doc);
	     TtaSetStructureChecking (0, doc);
	     TtaInsertSibling (ghost, lastcell, FALSE, doc);
	     RelateCellWithColumnHead (ghost, colhead, doc);
	     /* resume structure checking */
	     TtaSetStructureChecking (PreviousStuctureChecking, doc);
	  }
     }
}

/*----------------------------------------------------------------------
   NewColumnHead   create a new last Column_head and returns it.   
   If all == TRUE, create an additional Cell_ghost in all  
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
   Element             colhead, firstcolhead, currentrow, group, groupdone,
                       table, child;
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
	     /* add Table_cell_ghosts to all other rows */
	     /* process the row group that contains the row which doesn't */
	     /* need to be processed */
	     currentrow = row;
	     while (currentrow != NULL)
	       {
		  TtaPreviousSibling (&currentrow);
		  AddCellGhostInRow (currentrow, colhead, doc);
	       }
	     currentrow = row;
	     while (currentrow != NULL)
	       {
		  TtaNextSibling (&currentrow);
		  AddCellGhostInRow (currentrow, colhead, doc);
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
				 AddCellGhostInRow (currentrow, colhead, doc);
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
				      AddCellGhostInRow (currentrow, colhead, doc);
				      TtaNextSibling (&currentrow);
				   }
			      }
			    TtaNextSibling (&group);
			 }
		    }
		  TtaNextSibling (&child);
	       }
	     /* update column width */
	     firstcolhead = TtaGetFirstChild (TtaGetParent (lastcolhead));
	     SetColumnWidth (firstcolhead, colhead, doc);
	  }
     }
   return colhead;
}

/*----------------------------------------------------------------------
   ShiftByRowSpan  returns the width of the cell that spans
   a given cell of a given row.                    
   Return 0 if the cell is not spanned by a cell   
   above.                                          
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
      return 0;
   if (row == NULL)
      return 0;
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
		       attrType.AttrTypeNum = HTML_ATTR_rowspan;
		       attr = TtaGetAttribute (curcell, attrType);
		       if (attr != NULL)
			 {
			    /* this cell has an attribute rowspan */
			    span = TtaGetAttributeValue (attr);
			    if (span >= rownb)
			      {
				 /* this cells spans the row of interest */
				 shift = 1;
				 attrType.AttrTypeNum = HTML_ATTR_colspan;
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
   return shift;
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
   int                 PreviousStuctureChecking;

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
		 elType.ElTypeNum == HTML_EL_Heading_cell ||
		 elType.ElTypeNum == HTML_EL_Table_cell_ghost)
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
		       attrType.AttrTypeNum = HTML_ATTR_colspan;
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
		       /* Table_cell_ghost elements after the last cell in the row. */
		       while (colhead != NULL)
			 {
			    prevcell = lastcell;
			    elType.ElTypeNum = HTML_EL_Table_cell_ghost;
			    lastcell = TtaNewTree (doc, elType, "");
			    if (lastcell != NULL)
			      {
				 /* don't check the abstract tree against the structure schema */
				 PreviousStuctureChecking = TtaGetStructureChecking (doc);
				 TtaSetStructureChecking (0, doc);
				 TtaInsertSibling (lastcell, prevcell, FALSE, doc);
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
				      if (colhead == NULL)
					{
					   TtaDeleteTree (lastcell, doc);
					   lastcell = NULL;
					}
				   }
				 /* relate the cell_ghost with its corresponding */
				 /* Column_head */
				 if (lastcell != NULL)
				    RelateCellWithColumnHead (lastcell, colhead, doc);
				 /* resume structure checking */
				 TtaSetStructureChecking (PreviousStuctureChecking, doc);
			      }
			    if (colhead != NULL)
			       TtaNextSibling (&colhead);
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
   attrType.AttrTypeNum = HTML_ATTR_colspan;
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
   FormatRow       reformat an entire row.                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         FormatRow (Element row, Element firstcolhead, Document doc, boolean rowspan)
#else
static void         FormatRow (row, firstcolhead, doc, rowspan)
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
   boolean             computeColWidth;

   if (row == NULL)
      return;
   if (firstcolhead == NULL)
      firstcolhead = GetFirstColumnHead (row);
   if (firstcolhead == NULL)
      return;
   computeColWidth = FALSE;
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
				   {
				      TtaInsertSibling (colhead, prevcolhead, FALSE, doc);
				      if (rowspan)
					 computeColWidth = TRUE;
				   }
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
		  attrType.AttrTypeNum = HTML_ATTR_rowspan;
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
	if (computeColWidth)
	   SetColumnWidth (firstcolhead, NULL, doc);
     }
}


/*----------------------------------------------------------------------
   RemoveCellGhost remove all Table_cell_ghost elements in a row   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         RemoveCellGhost (Element row, Document doc)
#else
static void         RemoveCellGhost (row, doc)
Element             row;
Document            doc;

#endif
{
   Element             cell, nextcell;
   ElementType         elType;

   if (row == NULL)
      return;
   elType = TtaGetElementType (row);
   if (elType.ElTypeNum != HTML_EL_Table_row)
      return;
   cell = TtaGetFirstChild (row);
   while (cell != NULL)
     {
	nextcell = cell;
	TtaNextSibling (&nextcell);
	elType = TtaGetElementType (cell);
	if (elType.ElTypeNum == HTML_EL_Table_cell_ghost)
	   TtaDeleteTree (cell, doc);
	cell = nextcell;
     }
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
		  attrType.AttrTypeNum = HTML_ATTR_rowspan;
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
   Element             colhead, prevcolhead, cell, prevcell;
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
	    elType.ElTypeNum == HTML_EL_Heading_cell ||
	    elType.ElTypeNum == HTML_EL_Table_cell_ghost)
	  {
	     attrType.AttrSSchema = elType.ElSSchema;
	     if (all)
	       {
		  /* does this cell have an attribute rowspan ? */
		  attrType.AttrTypeNum = HTML_ATTR_rowspan;
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
			       /* there is no Column_head for that cell */
			    if (elType.ElTypeNum == HTML_EL_Table_cell_ghost)
			       /* the exceeding cell is a ghost */
			       /* remove this ghost and all its following siblings */
			       while (cell != NULL)
				 {
				    prevcell = cell;
				    TtaNextSibling (&cell);
				    TtaDeleteTree (prevcell, doc);
				 }
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
					      /* there is no Column_head for that cell */
					   if (elType.ElTypeNum == HTML_EL_Table_cell_ghost)
					      /* the exceeding cell is a ghost */
					      /* remove this ghost and all the following ghosts */
					      while (cell != NULL)
						{
						   prevcell = cell;
						   TtaNextSibling (&cell);
						   TtaDeleteTree (prevcell, doc);
						}
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
	     RemoveCellGhost (row, doc);
	     FormatRow (row, firstcolhead, doc, TRUE);
	     CompleteRow (row, doc, TRUE);
	     TtaNextSibling (&row);
	     maxrowspanning--;
	  }
     }
}


/*----------------------------------------------------------------------
   MinimizeGhosts  Remove all Table_Cell_ghost element which are   
   not necessary in a given table.                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                MinimizeGhosts (Element table, Document doc)
#else
void                MinimizeGhosts (table, doc)
Element             table;
Document            doc;

#endif
{
   Element             head, firstgroup, group, colhead, firstcolhead,
                       lastcolhead, row, cell, tbody;
   ElementType         elType, groupType;
   boolean             emptycol;

   if (table == NULL)
      return;
   /* search the Table_head element */
   head = TtaGetFirstChild (table);
   if (head == NULL)
      return;
   elType = TtaGetElementType (head);
   while (elType.ElTypeNum != HTML_EL_Table_head && head != NULL)
     {
	TtaNextSibling (&head);
	if (head != NULL)
	   elType = TtaGetElementType (head);
     }
   /* search the first row group */
   firstgroup = head;
   do
     {
	TtaNextSibling (&firstgroup);
	if (firstgroup != NULL)
	   elType = TtaGetElementType (firstgroup);
     }
   while (firstgroup != NULL &&
	  elType.ElTypeNum != HTML_EL_thead &&
	  elType.ElTypeNum != HTML_EL_Table_body &&
	  elType.ElTypeNum != HTML_EL_tfoot);
   if (firstgroup == NULL)
      return;
   /* search the last Column_head */
   lastcolhead = TtaGetLastChild (head);
   if (lastcolhead == NULL)
      return;
   /* treat all column, starting from the last one */
   /* stop when a column is not empty */
   emptycol = TRUE;
   while (emptycol && lastcolhead != NULL)
     {
	colhead = lastcolhead;
	TtaPreviousSibling (&colhead);
	elType = TtaGetElementType (lastcolhead);
	if (elType.ElTypeNum == HTML_EL_Column_head)
	  {
	     /* check all cells in that column, one row after the other */
	     group = firstgroup;
	     while (group != NULL && emptycol)
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
		  /* check all rows in the group */
		  while (row != NULL && emptycol)
		    {
		       elType = TtaGetElementType (row);
		       /* ignore Comments and Invalid_elements */
		       if (elType.ElTypeNum == HTML_EL_Table_row)
			 {
			    cell = GetCellFromColumnHead (row, lastcolhead);
			    if (cell != NULL)
			      {
				 elType = TtaGetElementType (cell);
				 if (elType.ElTypeNum == HTML_EL_Data_cell ||
				   elType.ElTypeNum == HTML_EL_Heading_cell)
				    /* there is a cell. Stop */
				    emptycol = FALSE;
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
	  }
	if (emptycol)
	   /* this column is empty */
	  {
	     /* delete all Table_cell_ghost elements in that column, one row */
	     /* after the other */
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
		       if (elType.ElTypeNum == HTML_EL_Table_row)
			 {
			    cell = GetCellFromColumnHead (row, lastcolhead);
			    if (cell != NULL)
			      {
				 elType = TtaGetElementType (cell);
				 if (elType.ElTypeNum == HTML_EL_Table_cell_ghost)
				    TtaDeleteTree (cell, doc);
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
	     /* delete the Column_head element for that column */
	     TtaDeleteTree (lastcolhead, doc);
	     lastcolhead = colhead;
	     /* recompute the width of remaining columns */
	     firstcolhead = TtaGetFirstChild (head);
	     SetColumnWidth (firstcolhead, NULL, doc);
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
   boolean             before;
   int                 span, hspan;
   Document            refdoc;
   char                name[20];
   int                 PreviousStuctureChecking;

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

	     /* Add Table_cell_ghost elements at the end of the rows that are */
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
				      attrType.AttrTypeNum = HTML_ATTR_rowspan;
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
								       spannedcell = GetCellFromColumnHead (spannedrow,
													    colhead);
								    if (spannedcell != NULL)
								      {
									 /* shift all following cells in the spanned row */
									 hspan = 1;
									 attrType.AttrTypeNum = HTML_ATTR_colspan;
									 attr = TtaGetAttribute (cell, attrType);
									 if (attr != NULL)
									   {
									      hspan = TtaGetAttributeValue (attr);
									      if (hspan < 1)
										 hspan = 1;
									   }
									 ShiftCellsRight (spannedcell, spannedrow, hspan,
											  FALSE, doc, firstcolhead);
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
	  }
	/* compute the width of each column */
	SetColumnWidth (firstcolhead, NULL, doc);
     }
   /* resume document structure checking */
   TtaSetStructureChecking (PreviousStuctureChecking, doc);
}

/*----------------------------------------------------------------------
   NewCell         a new cell has been created in a        
   HTML table.                             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         NewCell (Element cell, Document doc)
#else
static void         NewCell (cell, doc)
Element             cell;
Document            doc;

#endif
{
   Element             newcell, prevcell, nextcell, row, colhead, firstcolhead,
                       prevcolhead;
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
	 {
	   colhead = firstcolhead;
	   /********
	     nextcell = cell;
	     TtaNextSibling(&nextcell);
	     if (nextcell == NULL)
	     colhead = firstcolhead;
	     else
	     {
	     attr = TtaGetAttribute(nextcell, attrType);
	     if (attr != NULL)
	     TtaGiveReferenceAttributeValue(attr, &colhead, name, &refdoc);
	     else
	     colhead = firstcolhead;
	     }
	     ********/
	 }
       else
	 {
	   attr = TtaGetAttribute (prevcell, attrType);
	   if (attr != NULL)
	     {
	       TtaGiveReferenceAttributeValue (attr, &colhead, name, &refdoc);
	       if (colhead != NULL)
		 {
		   /* is there an attribute colspan for the previous cell ? */
		   attrType.AttrTypeNum = HTML_ATTR_colspan;
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
	   attrType.AttrTypeNum = HTML_ATTR_colspan;
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
	attrType.AttrTypeNum = HTML_ATTR_rowspan;
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
				 RemoveCellGhost (row, doc);
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
		       attrType.AttrTypeNum = HTML_ATTR_rowspan;
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
						RemoveCellGhost (oldrow, doc);
						FormatRow (oldrow, NULL, doc, TRUE);
						CompleteRow (oldrow, doc, TRUE);
					     }
					}
				   }
				 spannedrow = SetRowExt (cell, currentrow, span, doc);
				 if (!NewRow)
				   {
				      RemoveCellGhost (spannedrow, doc);
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
   attrType.AttrTypeNum = HTML_ATTR_rowspan;
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
   /* Remove exceeding Cell_ghost in the whole table */
   if (table != NULL)
      MinimizeGhosts (table, doc);
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
   attrType.AttrTypeNum = HTML_ATTR_rowspan;
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

static int          maxcellspan;

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
   /* seach the maximum value of attribute rowspan for the deleted cell and */
   /* all its following cells */
   maxcellspan = MaxRowSpanNextCells (event->element);
   return FALSE;		/* let Thot perform normal operation */
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
   Element             row, currentrow, nextrow, prevrow, tbody, cell, table;
   ElementType         elType;
   Document            doc;
   int                 span;
   boolean             empty;

   doc = event->document;
   row = event->element;
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
	     RemoveCellGhost (currentrow, doc);
	     FormatRow (currentrow, NULL, doc, TRUE);
	     CompleteRow (currentrow, doc, TRUE);
	     span--;
	  }
	TtaNextSibling (&currentrow);
     }
   /* Remove exceeding Cell_ghost in the whole table */
   elType = TtaGetElementType (row);
   elType.ElTypeNum = HTML_EL_Table;
   table = TtaGetTypedAncestor (row, elType);
   if (table != NULL)
      MinimizeGhosts (table, doc);
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
   CheckGhosts     in the table to which row belongs, add  
   Table_cell_ghost elements where needed and      
   delete unnecessary Table_cell_ghost elements.   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         CheckGhosts (Element row, Document doc)
#else
static void         CheckGhosts (row, doc)
Element             row;
Document            doc;

#endif
{
   ElementType         elType, groupType;
   Element             table, group, currentrow, tbody;

   elType = TtaGetElementType (row);
   elType.ElTypeNum = HTML_EL_Table;
   table = TtaGetTypedAncestor (row, elType);
   if (table != NULL)
     {
	/* add Ghost_cells at the end of the other rows, if necessary */
	group = TtaGetFirstChild (table);
	while (group != NULL)
	  {
	     groupType = TtaGetElementType (group);
	     if (groupType.ElTypeNum == HTML_EL_thead ||
		 groupType.ElTypeNum == HTML_EL_tfoot)
		currentrow = TtaGetFirstChild (group);
	     else if (groupType.ElTypeNum == HTML_EL_Table_body)
	       {
		  tbody = TtaGetFirstChild (group);
		  if (tbody == NULL)
		     currentrow = NULL;
		  else
		     currentrow = TtaGetFirstChild (tbody);
	       }
	     else
		currentrow = NULL;
	     while (currentrow != NULL)
	       {
		  CompleteRow (currentrow, doc, TRUE);
		  TtaNextSibling (&currentrow);
		  if (currentrow == NULL)
		     if (groupType.ElTypeNum == HTML_EL_Table_body)
			/* get the next tbody element */
		       {
			  TtaNextSibling (&tbody);
			  if (tbody != NULL)
			     currentrow = TtaGetFirstChild (tbody);
		       }
	       }
	     TtaNextSibling (&group);
	  }
	/* Remove exceeding Cell_ghost in the whole table */
	MinimizeGhosts (table, doc);
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

   row = event->element;
   doc = event->document;
   elType = TtaGetElementType (row);
   attrType.AttrSSchema = elType.ElSSchema;
   attrType.AttrTypeNum = HTML_ATTR_rowspan;
   /* remove all Cell_ghost elements in the pasted row */
   RemoveCellGhost (row, doc);
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
   FormatRow (row, NULL, doc, TRUE);
   /* add Ghost_cells at the end */
   CompleteRow (row, doc, TRUE);
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
		       RemoveCellGhost (currentrow, doc);
		       FormatRow (currentrow, NULL, doc, TRUE);
		    }
		  maxspan--;
	       }
	  }
     }
   while (currentrow != NULL);

   CheckGhosts (row, doc);
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
   maxrowspan = MaximumRowSpan (row, maxrowspan);
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
	     RemoveCellGhost (currentrow, doc);
	     FormatRow (currentrow, NULL, doc, TRUE);
	     CompleteRow (currentrow, doc, TRUE);
	     maxrowspan--;
	  }
	TtaNextSibling (&currentrow);
     }
   CheckGhosts (row, doc);
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
   if (span <= 1)
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
	     RemoveCellGhost (currentrow, doc);
	     FormatRow (currentrow, NULL, doc, TRUE);
	     span--;
	  }
	TtaNextSibling (&currentrow);
     }
   CheckGhosts (row, doc);
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
   if (span <= 1)
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

/*----------------------------------------------------------------------
  CellWidthCreated
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void CellWidthCreated (NotifyAttribute *event)
#else /* __STDC__*/
void CellWidthCreated(event)
     NotifyAttribute *event;
#endif /* __STDC__*/
{
   ElementType         elType;
   AttributeType       attrType;
   Element             cell, colhead, firstcolhead;
   Attribute           attr, attrColWidthPcent, attrColWidthDelta;
   Document            doc, refdoc;
   char                name[50];

   doc = event->document;
   cell = event->element;
   /* search the Column_head that corresponds to the cell */
   elType = TtaGetElementType (cell);
   attrType.AttrSSchema = elType.ElSSchema;
   attrType.AttrTypeNum = HTML_ATTR_Ref_column;
   attr = TtaGetAttribute (cell, attrType);
   if (attr != NULL)
     {
     TtaGiveReferenceAttributeValue (attr, &colhead, name, &refdoc);
     if (colhead != NULL)
	{
        /* remove attribute Col_width_percent from column head if it is present */
        attrType.AttrTypeNum = HTML_ATTR_Col_width_percent;
        attrColWidthPcent = TtaGetAttribute (colhead, attrType);
        if (attrColWidthPcent != NULL)
           TtaRemoveAttribute (colhead, attrColWidthPcent, doc);
        /* remove existing attribute Col_width_delta from column head */
        attrType.AttrTypeNum = HTML_ATTR_Col_width_delta;
        attrColWidthDelta = TtaGetAttribute (colhead, attrType);
        if (attrColWidthDelta != NULL)
           TtaRemoveAttribute (colhead, attrColWidthDelta, doc);
	/* compute the width of all columns */
	firstcolhead = GetFirstColumnHead (cell);
	SetColumnWidth (firstcolhead, NULL, doc);
	}
     }
}
 
 
/*----------------------------------------------------------------------
  CellWidthModified
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void CellWidthModified (NotifyAttribute *event)
#else /* __STDC__*/
void CellWidthModified(event)
     NotifyAttribute *event;
#endif /* __STDC__*/
{
   ElementType         elType;
   AttributeType       attrType;
   Element             cell, colhead, firstcolhead;
   Attribute           attr, attrColWidthPcent;
   Document            doc, refdoc;
   char                name[50];

   doc = event->document;
   cell = event->element;
   /* search the Column_head that corresponds to the cell */
   elType = TtaGetElementType (cell);
   attrType.AttrSSchema = elType.ElSSchema;
   attrType.AttrTypeNum = HTML_ATTR_Ref_column;
   attr = TtaGetAttribute (cell, attrType);
   if (attr != NULL)
     {
     TtaGiveReferenceAttributeValue (attr, &colhead, name, &refdoc);
     if (colhead != NULL)
	{
        /* remove attribute Col_width_percent from column head if it is present */
        attrType.AttrTypeNum = HTML_ATTR_Col_width_percent;
        attrColWidthPcent = TtaGetAttribute (colhead, attrType);
        if (attrColWidthPcent != NULL)
           TtaRemoveAttribute (colhead, attrColWidthPcent, doc);
	/* compute the width of all columns */
	firstcolhead = GetFirstColumnHead (cell);
	SetColumnWidth (firstcolhead, NULL, doc);
	}
     }
}
 
 
/*----------------------------------------------------------------------
  CellWidthDeleted
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void CellWidthDeleted (NotifyAttribute *event)
#else /* __STDC__*/
void CellWidthDeleted(event)
     NotifyAttribute *event;
#endif /* __STDC__*/
{
   ElementType         elType;
   AttributeType       attrType;
   Element             cell, colhead, firstcolhead;
   Attribute           attr, attrColWidthPxl;
   Document            doc, refdoc;
   char                name[50];

   doc = event->document;
   cell = event->element;
   /* search the Column_head that corresponds to the cell */
   elType = TtaGetElementType (cell);
   attrType.AttrSSchema = elType.ElSSchema;
   attrType.AttrTypeNum = HTML_ATTR_Ref_column;
   attr = TtaGetAttribute (cell, attrType);
   if (attr != NULL)
     {
     TtaGiveReferenceAttributeValue (attr, &colhead, name, &refdoc);
     if (colhead != NULL)
	{
        /* remove attribute Col_width_pxl from column head if it is present */
        attrType.AttrTypeNum = HTML_ATTR_Col_width_pxl;
        attrColWidthPxl = TtaGetAttribute (colhead, attrType);
        if (attrColWidthPxl != NULL)
           TtaRemoveAttribute (colhead, attrColWidthPxl, doc);
	/* compute the width of all columns */
	firstcolhead = GetFirstColumnHead (cell);
	SetColumnWidth (firstcolhead, NULL, doc);
	}
     }
}
