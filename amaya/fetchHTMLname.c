/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *
 * fetchHTMLname
 *
 * Author: I. Vatton
 *
 */

/* This module is used by the html2thot parser and the css parser. */

#define THOT_EXPORT extern
#include "amaya.h"
#include "fetchHTMLname.h"
#include "parser.h"

/* mapping table of HTML elements */
static GIMapping    HTMLGIMappingTable[] =
{
   /* This table MUST be in alphabetical order */
   {TEXT ("A"), SPACE, HTML_EL_Anchor, NULL},
   {TEXT ("ABBR"), SPACE, HTML_EL_ABBR, NULL},
   {TEXT ("ACRONYM"), SPACE, HTML_EL_ACRONYM, NULL},
   {TEXT ("ADDRESS"), SPACE, HTML_EL_Address, NULL},
   {TEXT ("APPLET"), SPACE, HTML_EL_Applet, NULL},
   {TEXT ("AREA"), TEXT ('E'), HTML_EL_AREA, NULL},
   {TEXT ("B"), SPACE, HTML_EL_Bold_text, NULL},
   {TEXT ("BASE"), TEXT ('E'), HTML_EL_BASE, NULL},
   {TEXT ("BASEFONT"), TEXT ('E'), HTML_EL_BaseFont, NULL},
   {TEXT ("BDO"), SPACE, HTML_EL_BDO, NULL},
   {TEXT ("BIG"), SPACE, HTML_EL_Big_text, NULL},
   {TEXT ("BLOCKQUOTE"), SPACE, HTML_EL_Block_Quote, NULL},
   {TEXT ("BODY"), SPACE, HTML_EL_BODY, NULL},
   {TEXT ("BR"), TEXT ('E'), HTML_EL_BR, NULL},
   {TEXT ("BUTTON"), SPACE, HTML_EL_BUTTON, NULL},
   {TEXT ("C"), SPACE, HTML_EL_TEXT_UNIT, NULL},
   {TEXT ("CAPTION"), SPACE, HTML_EL_CAPTION, NULL},
   {TEXT ("CENTER"), SPACE, HTML_EL_Center, NULL},
   {TEXT ("CITE"), SPACE, HTML_EL_Cite, NULL},
   {TEXT ("CODE"), SPACE, HTML_EL_Code, NULL},
   {TEXT ("COLGROUP"), SPACE, HTML_EL_COLGROUP, NULL},
   {TEXT ("COL"), SPACE, HTML_EL_COL, NULL},
   {TEXT ("DD"), SPACE, HTML_EL_Definition, NULL},
   {TEXT ("DEL"), SPACE, HTML_EL_DEL, NULL},
   {TEXT ("DFN"), SPACE, HTML_EL_Def, NULL},
   {TEXT ("DIR"), SPACE, HTML_EL_Directory, NULL},
   {TEXT ("DIV"), SPACE, HTML_EL_Division, NULL},
   {TEXT ("DL"), SPACE, HTML_EL_Definition_List, NULL},
   {TEXT ("DT"), SPACE, HTML_EL_Term, NULL},
   {TEXT ("EM"), SPACE, HTML_EL_Emphasis, NULL},
   {TEXT ("FIELDSET"), SPACE, HTML_EL_FIELDSET, NULL},
   {TEXT ("FONT"), SPACE, HTML_EL_Font_, NULL},
   {TEXT ("FORM"), SPACE, HTML_EL_Form, NULL},
   {TEXT ("FRAME"), TEXT ('E'), HTML_EL_FRAME, NULL},
   {TEXT ("FRAMESET"), SPACE, HTML_EL_FRAMESET, NULL},
   {TEXT ("H1"), SPACE, HTML_EL_H1, NULL},
   {TEXT ("H2"), SPACE, HTML_EL_H2, NULL},
   {TEXT ("H3"), SPACE, HTML_EL_H3, NULL},
   {TEXT ("H4"), SPACE, HTML_EL_H4, NULL},
   {TEXT ("H5"), SPACE, HTML_EL_H5, NULL},
   {TEXT ("H6"), SPACE, HTML_EL_H6, NULL},
   {TEXT ("HEAD"), SPACE, HTML_EL_HEAD, NULL},
   {TEXT ("HR"), TEXT ('E'), HTML_EL_Horizontal_Rule, NULL},
   {TEXT ("HTML"), SPACE, HTML_EL_HTML, NULL},
   {TEXT ("I"), SPACE, HTML_EL_Italic_text, NULL},
   {TEXT ("IFRAME"), SPACE, HTML_EL_IFRAME, NULL},
   {TEXT ("IMAGE"), TEXT ('E'), HTML_EL_PICTURE_UNIT, NULL},
   {TEXT ("IMG"), TEXT ('E'), HTML_EL_PICTURE_UNIT, NULL},
   {TEXT ("INPUT"), TEXT ('E'), HTML_EL_Input, NULL},
   {TEXT ("INS"), SPACE, HTML_EL_INS, NULL},
   {TEXT ("ISINDEX"), TEXT ('E'), HTML_EL_ISINDEX, NULL},
   {TEXT ("KBD"), SPACE, HTML_EL_Keyboard, NULL},
   {TEXT ("LABEL"), SPACE, HTML_EL_LABEL, NULL},
   {TEXT ("LEGEND"), SPACE, HTML_EL_LEGEND, NULL},
   {TEXT ("LI"), SPACE, HTML_EL_List_Item, NULL},
   {TEXT ("LINK"), TEXT ('E'), HTML_EL_LINK, NULL},
   {TEXT ("LISTING"), SPACE, HTML_EL_Preformatted, NULL},	/*converted to PRE */
   {TEXT ("MAP"), SPACE, HTML_EL_MAP, NULL},
   {TEXT ("MATH"), SPACE, HTML_EL_Math, NULL},
   {TEXT ("MATHDISP"), SPACE, HTML_EL_Math, NULL},  /* for compatibility with an
				     old version of MathML: WD-math-970704 */
   {TEXT ("MENU"), SPACE, HTML_EL_Menu, NULL},
   {TEXT ("META"), TEXT ('E'), HTML_EL_META, NULL},
   {TEXT ("NOFRAMES"), SPACE, HTML_EL_NOFRAMES, NULL},
   {TEXT ("NOSCRIPT"), SPACE, HTML_EL_NOSCRIPT, NULL},
   {TEXT ("OBJECT"), SPACE, HTML_EL_Object, NULL},
   {TEXT ("OL"), SPACE, HTML_EL_Numbered_List, NULL},
   {TEXT ("OPTGROUP"), SPACE, HTML_EL_OptGroup, NULL},
   {TEXT ("OPTION"), SPACE, HTML_EL_Option, NULL},
   {TEXT ("P"), SPACE, HTML_EL_Paragraph, NULL},
   {TEXT ("P*"), SPACE, HTML_EL_Pseudo_paragraph, NULL},
   {TEXT ("PARAM"), TEXT ('E'), HTML_EL_Parameter, NULL},
   {TEXT ("PLAINTEXT"), SPACE, HTML_EL_Preformatted, NULL},	/* converted to PRE */
   {TEXT ("PRE"), SPACE, HTML_EL_Preformatted, NULL},
   {TEXT ("Q"), SPACE, HTML_EL_Quotation, NULL},
   {TEXT ("S"), SPACE, HTML_EL_Struck_text, NULL},
   {TEXT ("SAMP"), SPACE, HTML_EL_Sample, NULL},
   {TEXT ("SCRIPT"), SPACE, HTML_EL_SCRIPT, NULL},
   {TEXT ("SELECT"), SPACE, HTML_EL_Option_Menu, NULL},
   {TEXT ("SMALL"), SPACE, HTML_EL_Small_text, NULL},
   {TEXT ("SPAN"), SPACE, HTML_EL_Span, NULL},
   {TEXT ("STRIKE"), SPACE, HTML_EL_Struck_text, NULL},
   {TEXT ("STRONG"), SPACE, HTML_EL_Strong, NULL},
   {TEXT ("STYLE"), SPACE, HTML_EL_STYLE_, NULL},
   {TEXT ("SUB"), SPACE, HTML_EL_Subscript, NULL},
   {TEXT ("SUP"), SPACE, HTML_EL_Superscript, NULL},
   {TEXT ("TABLE"), SPACE, HTML_EL_Table, NULL},
   {TEXT ("TBODY"), SPACE, HTML_EL_tbody, NULL},
   {TEXT ("TD"), SPACE, HTML_EL_Data_cell, NULL},
   {TEXT ("TEXTAREA"), SPACE, HTML_EL_Text_Area, NULL},
   {TEXT ("TFOOT"), SPACE, HTML_EL_tfoot, NULL},
   {TEXT ("TH"), SPACE, HTML_EL_Heading_cell, NULL},
   {TEXT ("THEAD"), SPACE, HTML_EL_thead, NULL},
   {TEXT ("TITLE"), SPACE, HTML_EL_TITLE, NULL},
   {TEXT ("TR"), SPACE, HTML_EL_Table_row, NULL},
   {TEXT ("TT"), SPACE, HTML_EL_Teletype_text, NULL},
   {TEXT ("U"), SPACE, HTML_EL_Underlined_text, NULL},
   {TEXT ("UL"), SPACE, HTML_EL_Unnumbered_List, NULL},
   {TEXT ("VAR"), SPACE, HTML_EL_Variable, NULL},
   {TEXT ("XMLGRAPHICS"), SPACE, HTML_EL_XMLGraphics, NULL},
   {TEXT ("XMP"), SPACE, HTML_EL_Preformatted, NULL},		/* converted to PRE */
   {TEXT (""), SPACE, 0, NULL}	/* Last entry. Mandatory */
};

/* define a pointer to let other parser functions access the local table */
GIMapping    *pHTMLGIMapping = HTMLGIMappingTable;

/*----------------------------------------------------------------------
   MapGI
   search in the mapping tables the entry for the element of
   name GI and returns the rank of that entry.
   When returning, schema contains the Thot SSchema that defines that element,
   Returns -1 and schema = NULL if not found.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 MapGI (STRING gi, SSchema *schema, Document doc)
#else
int                 MapGI (gi, schema, doc)
STRING              gi;
SSchema		   *schema;
Document            doc;
#endif
{
  ElementType	      elType;
  STRING              ptr; 
  CHAR_T              c;
  int                 i;
  int                 entry;
  ThotBool	      isHTML;

  /* TODO: use NameSpaces to search in the right table */
  entry = -1;
  if (*schema == NULL)
    {
      isHTML = FALSE;
      ptr = NULL;
    }
  else
    {
      ptr = TtaGetSSchemaName (*schema);
      isHTML = !ustrcmp (ptr, TEXT("HTML"));
    }

  i = 0;
  if (*schema == NULL || isHTML)
    {
      /*
	First convert the first char into lower case to locate
	registered tags in the HTML mapping table.
	Entries are registered in upper case and in alphabetic order.
      */

      /* TODO: define a function which works on unicode */
      c = toupper (gi[0]);
      /* look for the first concerned entry in the table */
      while (HTMLGIMappingTable[i].htmlGI[0] < c
	     && HTMLGIMappingTable[i].htmlGI[0] != EOS)
	i++;

      /* look at all entries starting with the right character */
      do
	{
	  if (ustrcasecmp (HTMLGIMappingTable[i].htmlGI, gi))
	    i++;
	  else
	    entry = i;
	}
      while (entry < 0 && HTMLGIMappingTable[i].htmlGI[0] == c);
    }

  if (entry < 0)
    if (*schema != NULL && isHTML)
      *schema = NULL;
    else
      /* not found. Look at the XML mapping tables */
      {
	elType.ElTypeNum = 0;
	elType.ElSSchema = *schema;

#ifndef STANDALONE
	if (!ptr || !ustrcmp (ptr, TEXT("MATHML")))
	  MapXMLElementType (MATH_TYPE, gi, &elType, &ptr, &c, doc);
	if (elType.ElTypeNum == 0 && (!ptr || !ustrcmp (ptr, TEXT("GRAPHML"))))
	  MapXMLElementType (GRAPH_TYPE, gi, &elType, &ptr, &c, doc);
#endif
	if (elType.ElTypeNum == 0)
	  {
            entry = -1;
	    elType.ElSSchema = NULL;
	    *schema = NULL;
	  }
	else
	  {
            entry = elType.ElTypeNum;
            *schema = elType.ElSSchema;
	  }
      }
  return entry;
}

/*----------------------------------------------------------------------
   GIType  search in mapping tables the Element type associated with
   a given GI Name. If not found returns zero.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GIType (STRING gi, ElementType *elType, Document doc)
#else
void                GIType (gi, elType, doc)
STRING              gi;
ElementType        *elType;
Document	    doc;
#endif
{
  CHAR_T              c;
  STRING              ptr;
  int                 i;

  /* TODO: use NameSpaces to search in the right table */
  elType->ElSSchema = NULL;
  elType->ElTypeNum = 0;

  /*
    First convert the first char into lower case to locate
    registered tags in the HTML mapping table.
    Entries are registered in upper case and in alphabetic order.
  */

  /* TODO: define a function which works on unicode */
  c = toupper (gi[0]);

  i = 0;
  /* look for the first concerned entry in the table */
  while (HTMLGIMappingTable[i].htmlGI[0] < c
	 && HTMLGIMappingTable[i].htmlGI[0] != EOS)
    i++;
  /* look at all entries starting with the right character */
  do
    {
      if (!ustrcasecmp (HTMLGIMappingTable[i].htmlGI, gi))
	{
	  
	  if (doc != 0)
	    elType->ElSSchema = TtaGetSSchema (TEXT("HTML"), doc);
	  elType->ElTypeNum = HTMLGIMappingTable[i].ThotType;
	  return;
	}
      i++;
    }
  while (HTMLGIMappingTable[i].htmlGI[0] == c);

#ifndef STANDALONE
  /* if not found, look at the XML mapping tables */
  MapXMLElementType (MATH_TYPE, gi, elType, &ptr, &c, doc);
  if (elType->ElTypeNum == 0)
    MapXMLElementType (GRAPH_TYPE, gi, elType, &ptr, &c, doc);
#endif
}
