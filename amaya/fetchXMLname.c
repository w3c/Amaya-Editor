/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996-2000
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 *
 * fetchXMLname
 *
 * Author: I. Vatton
 *
 */

#define THOT_EXPORT extern
#include "amaya.h"
#include "parser.h"

/* mapping table of XHTML elements */
static ElemMapping    XHTMLElemMappingTable[] =
{
   /* This table MUST be in alphabetical order */
   {TEXT("XMLcomment"), SPACE, HTML_EL_Comment_, L_Undefined},
   {TEXT("XMLcomment_line"), SPACE, HTML_EL_Comment_line, L_Undefined},
   {TEXT("a"), SPACE, HTML_EL_Anchor, L_Basic},
   {TEXT("abbr"), SPACE, HTML_EL_ABBR, L_Basic},
   {TEXT("acronym"), SPACE, HTML_EL_ACRONYM, L_Basic},
   {TEXT("address"), SPACE, HTML_EL_Address, L_Basic},
   {TEXT("applet"), SPACE, HTML_EL_Applet, L_Transitional},
   {TEXT("area"), 'E', HTML_EL_AREA, L_Transitional},
   {TEXT("b"), SPACE, HTML_EL_Bold_text, L_Transitional},
   {TEXT("base"), 'E', HTML_EL_BASE, L_Basic},
   {TEXT("basefont"), 'E', HTML_EL_BaseFont, L_Transitional},
   {TEXT("bdo"), SPACE, HTML_EL_BDO, L_Transitional},
   {TEXT("big"), SPACE, HTML_EL_Big_text, L_Transitional},
   {TEXT("blockquote"), SPACE, HTML_EL_Block_Quote, L_Basic},
   {TEXT("body"), SPACE, HTML_EL_BODY, L_Basic},
   {TEXT("br"), 'E', HTML_EL_BR, L_Basic},
   {TEXT("button"), SPACE, HTML_EL_BUTTON, L_Transitional},
   {TEXT("c"), SPACE, HTML_EL_TEXT_UNIT, L_Transitional},
   {TEXT("caption"), SPACE, HTML_EL_CAPTION, L_Transitional},
   {TEXT("center"), SPACE, HTML_EL_Center, L_Transitional},
   {TEXT("cite"), SPACE, HTML_EL_Cite, L_Basic},
   {TEXT("code"), SPACE, HTML_EL_Code, L_Basic},
   {TEXT("colgroup"), SPACE, HTML_EL_COLGROUP, L_Transitional},
   {TEXT("col"), SPACE, HTML_EL_COL, L_Transitional},
   {TEXT("dd"), SPACE, HTML_EL_Definition, L_Basic},
   {TEXT("del"), SPACE, HTML_EL_DEL, L_Transitional},
   {TEXT("dfn"), SPACE, HTML_EL_Def, L_Basic},
   {TEXT("dir"), SPACE, HTML_EL_Directory, L_Transitional},
   {TEXT("div"), SPACE, HTML_EL_Division, L_Basic},
   {TEXT("dl"), SPACE, HTML_EL_Definition_List, L_Basic},
   {TEXT("dt"), SPACE, HTML_EL_Term, L_Basic},
   {TEXT("em"), SPACE, HTML_EL_Emphasis, L_Basic},
   {TEXT("fieldset"), SPACE, HTML_EL_FIELDSET, L_Transitional},
   {TEXT("font"), SPACE, HTML_EL_Font_, L_Transitional},
   {TEXT("form"), SPACE, HTML_EL_Form, L_Basic},
   {TEXT("frame"), 'E', HTML_EL_FRAME, L_Transitional},
   {TEXT("frameset"), SPACE, HTML_EL_FRAMESET, L_Transitional},
   {TEXT("h1"), SPACE, HTML_EL_H1, L_Basic},
   {TEXT("h2"), SPACE, HTML_EL_H2, L_Basic},
   {TEXT("h3"), SPACE, HTML_EL_H3, L_Basic},
   {TEXT("h4"), SPACE, HTML_EL_H4, L_Basic},
   {TEXT("h5"), SPACE, HTML_EL_H5, L_Basic},
   {TEXT("h6"), SPACE, HTML_EL_H6, L_Basic},
   {TEXT("head"), SPACE, HTML_EL_HEAD, L_Basic},
   {TEXT("hr"), 'E', HTML_EL_Horizontal_Rule, L_Transitional},
   {TEXT("html"), SPACE, HTML_EL_HTML, L_Basic},
   {TEXT("i"), SPACE, HTML_EL_Italic_text, L_Transitional},
   {TEXT("iframe"), SPACE, HTML_EL_IFRAME, L_Transitional},
   {TEXT("image"), 'E', HTML_EL_PICTURE_UNIT, L_Transitional},
   {TEXT("img"), 'E', HTML_EL_PICTURE_UNIT, L_Basic},
   {TEXT("input"), 'E', HTML_EL_Input, L_Basic},
   {TEXT("ins"), SPACE, HTML_EL_INS, L_Transitional},
   {TEXT("isindex"), 'E', HTML_EL_ISINDEX, L_Transitional},
   {TEXT("kbd"), SPACE, HTML_EL_Keyboard, L_Basic},
   {TEXT("label"), SPACE, HTML_EL_LABEL, L_Basic},
   {TEXT("legend"), SPACE, HTML_EL_LEGEND, L_Transitional},
   {TEXT("li"), SPACE, HTML_EL_List_Item, L_Basic},
   {TEXT("link"), 'E', HTML_EL_LINK, L_Basic},
   {TEXT("listing"), SPACE, HTML_EL_Preformatted, L_Transitional}, /*converted to PRE */
   {TEXT("map"), SPACE, HTML_EL_MAP, L_Transitional},
   {TEXT("math"), SPACE, HTML_EL_Math, L_Transitional},
   {TEXT("menu"), SPACE, HTML_EL_Menu, L_Transitional},
   {TEXT("meta"), 'E', HTML_EL_META, L_Basic},
   {TEXT("noframes"), SPACE, HTML_EL_NOFRAMES, L_Transitional},
   {TEXT("noscript"), SPACE, HTML_EL_NOSCRIPT, L_Transitional},
   {TEXT("object"), SPACE, HTML_EL_Object, L_Basic},
   {TEXT("ol"), SPACE, HTML_EL_Numbered_List, L_Basic},
   {TEXT("optgroup"), SPACE, HTML_EL_OptGroup, L_Transitional},
   {TEXT("option"), SPACE, HTML_EL_Option, L_Basic},
   {TEXT("p"), SPACE, HTML_EL_Paragraph, L_Basic},
   {TEXT("p*"), SPACE, HTML_EL_Pseudo_paragraph, L_Basic},
   {TEXT("param"), 'E', HTML_EL_Parameter, L_Basic},
   {TEXT("plaintext"), SPACE, HTML_EL_Preformatted, L_Transitional},/* converted to PRE */
   {TEXT("pre"), SPACE, HTML_EL_Preformatted, L_Basic},
   {TEXT("q"), SPACE, HTML_EL_Quotation, L_Basic},
   {TEXT("s"), SPACE, HTML_EL_Struck_text, L_Transitional},
   {TEXT("samp"), SPACE, HTML_EL_Sample, L_Basic},
   {TEXT("script"), SPACE, HTML_EL_SCRIPT, L_Transitional},
   {TEXT("select"), SPACE, HTML_EL_Option_Menu, L_Basic},
   {TEXT("small"), SPACE, HTML_EL_Small_text, L_Transitional},
   {TEXT("span"), SPACE, HTML_EL_Span, L_Basic},
   {TEXT("strike"), SPACE, HTML_EL_Struck_text, L_Transitional},
   {TEXT("strong"), SPACE, HTML_EL_Strong, L_Basic},
   {TEXT("style"), SPACE, HTML_EL_STYLE_, L_Transitional},
   {TEXT("sub"), SPACE, HTML_EL_Subscript, L_Transitional},
   {TEXT("sup"), SPACE, HTML_EL_Superscript, L_Transitional},
#ifdef GRAPHML
   {TEXT("svg"), SPACE, HTML_EL_XMLGraphics, L_Transitional},
#endif
   {TEXT("table"), SPACE, HTML_EL_Table, L_Basic},
   {TEXT("tbody"), SPACE, HTML_EL_tbody, L_Basic},
   {TEXT("td"), SPACE, HTML_EL_Data_cell, L_Basic},
   {TEXT("textarea"), SPACE, HTML_EL_Text_Area, L_Basic},
   {TEXT("tfoot"), SPACE, HTML_EL_tfoot, L_Transitional},
   {TEXT("th"), SPACE, HTML_EL_Heading_cell, L_Basic},
   {TEXT("thead"), SPACE, HTML_EL_thead, L_Transitional},
   {TEXT("title"), SPACE, HTML_EL_TITLE, L_Basic},
   {TEXT("tr"), SPACE, HTML_EL_Table_row, L_Basic},
   {TEXT("tt"), SPACE, HTML_EL_Teletype_text, L_Transitional},
   {TEXT("u"), SPACE, HTML_EL_Underlined_text, L_Transitional},
   {TEXT("ul"), SPACE, HTML_EL_Unnumbered_List, L_Basic},
   {TEXT("var"), SPACE, HTML_EL_Variable, L_Basic},
#ifdef GRAPHML
   {TEXT("xmlgraphics"), SPACE, HTML_EL_XMLGraphics, L_Transitional},
#endif
   {TEXT("xmp"), SPACE, HTML_EL_Preformatted, L_Transitional},  /* converted to PRE */
   {TEXT(""), SPACE, 0, L_Basic}	/* Last entry. Mandatory */
};
int           HTML_ENTRIES = (sizeof(XHTMLElemMappingTable) / sizeof(ElemMapping));
ElemMapping  *pHTMLGIMapping = XHTMLElemMappingTable;

/* mapping table of MathML elements */
#include "MathML.h"
static ElemMapping    MathMLElemMappingTable[] =
{
   /* This table MUST be in alphabetical order */
   {TEXT("XMLcomment"), SPACE, MathML_EL_XMLcomment, L_Undefined},
   {TEXT("XMLcomment_line"), SPACE, MathML_EL_XMLcomment_line, L_Undefined},
   {TEXT("maligngroup"), 'E', MathML_EL_MALIGNGROUP, L_Undefined},
   {TEXT("malignmark"), 'E', MathML_EL_MALIGNMARK, L_Undefined},
   {TEXT("mchar"), 'E', MathML_EL_MCHAR, L_Undefined},
   {TEXT("menclose"), SPACE, MathML_EL_MENCLOSE, L_Undefined},
   {TEXT("merror"), SPACE, MathML_EL_MERROR, L_Undefined},
   {TEXT("mf"), SPACE, MathML_EL_MF, L_Undefined},  /* for compatibility with an old version
					  of MathML: WD-math-970704 */
   {TEXT("mfenced"), SPACE, MathML_EL_MFENCED, L_Undefined},
   {TEXT("mfrac"), SPACE, MathML_EL_MFRAC, L_Undefined},
   {TEXT("mglyph"), 'E', MathML_EL_MGLYPH, L_Undefined},
   {TEXT("mi"), SPACE, MathML_EL_MI, L_Undefined},
   {TEXT("mlabeledtr"), SPACE, MathML_EL_MLABELEDTR, L_Undefined},
   {TEXT("mmultiscripts"), SPACE, MathML_EL_MMULTISCRIPTS, L_Undefined},
   {TEXT("mn"), SPACE, MathML_EL_MN, L_Undefined},
   {TEXT("mo"), SPACE, MathML_EL_MO, L_Undefined},
   {TEXT("mover"), SPACE, MathML_EL_MOVER, L_Undefined},
   {TEXT("mpadded"), SPACE, MathML_EL_MPADDED, L_Undefined},
   {TEXT("mphantom"), SPACE, MathML_EL_MPHANTOM, L_Undefined},
   {TEXT("mprescripts"), SPACE, MathML_EL_PrescriptPairs, L_Undefined},
   {TEXT("mroot"), SPACE, MathML_EL_MROOT, L_Undefined},
   {TEXT("mrow"), SPACE, MathML_EL_MROW, L_Undefined},
   {TEXT("ms"), SPACE, MathML_EL_MS, L_Undefined},
   {TEXT("mspace"), 'E', MathML_EL_MSPACE, L_Undefined},
   {TEXT("msqrt"), SPACE, MathML_EL_MSQRT, L_Undefined},
   {TEXT("mstyle"), SPACE, MathML_EL_MSTYLE, L_Undefined},
   {TEXT("msub"), SPACE, MathML_EL_MSUB, L_Undefined},
   {TEXT("msubsup"), SPACE, MathML_EL_MSUBSUP, L_Undefined},
   {TEXT("msup"), SPACE, MathML_EL_MSUP, L_Undefined},
   {TEXT("mtable"), SPACE, MathML_EL_MTABLE, L_Undefined},
   {TEXT("mtd"), SPACE, MathML_EL_MTD, L_Undefined},
   {TEXT("mtext"), SPACE, MathML_EL_MTEXT, L_Undefined},
   {TEXT("mtr"), SPACE, MathML_EL_MTR, L_Undefined},
   {TEXT("munder"), SPACE, MathML_EL_MUNDER, L_Undefined},
   {TEXT("munderover"), SPACE, MathML_EL_MUNDEROVER, L_Undefined},
   {TEXT("none"), SPACE, MathML_EL_Construct, L_Undefined},
   {TEXT("sep"), 'E', MathML_EL_SEP, L_Undefined},
   {TEXT(""), SPACE, 0, L_Undefined}	/* Last entry. Mandatory */
};

#ifdef GRAPHML
/* mapping table of GraphML elements */
#include "GraphML.h"
static ElemMapping    GraphMLElemMappingTable[] =
{
   /* This table MUST be in alphabetical order */
   {TEXT("XMLcomment"), SPACE, GraphML_EL_XMLcomment, L_Undefined},
   {TEXT("XMLcomment_line"), SPACE, GraphML_EL_XMLcomment_line, L_Undefined},
   {TEXT("circle"), SPACE, GraphML_EL_Circle, L_Undefined},
   {TEXT("closedspline"), SPACE, GraphML_EL_ClosedSpline, L_Undefined},
   {TEXT("desc"), SPACE, GraphML_EL_desc, L_Undefined},
   {TEXT("group"), SPACE, GraphML_EL_Group, L_Undefined},
   {TEXT("label"), 'X', GraphML_EL_Label, L_Undefined}, /* see function GraphMLGetDTDName */
   {TEXT("line"), 'E', GraphML_EL_Line_, L_Undefined},
   {TEXT("math"), 'X', GraphML_EL_Math, L_Undefined},   /* see function GraphMLGetDTDName */
   {TEXT("metadata"), SPACE, GraphML_EL_metadata, L_Undefined},
   {TEXT("oval"), SPACE, GraphML_EL_Oval, L_Undefined},
   {TEXT("polygon"), SPACE, GraphML_EL_Polygon, L_Undefined},
   {TEXT("polyline"), 'E', GraphML_EL_Polyline, L_Undefined},
   {TEXT("rect"), SPACE, GraphML_EL_Rectangle, L_Undefined},
   {TEXT("roundrect"), SPACE, GraphML_EL_RoundRect, L_Undefined},
   {TEXT("spline"), 'E', GraphML_EL_Spline, L_Undefined},
   {TEXT("text"), 'X', GraphML_EL_Text_, L_Undefined},  /* see function GraphMLGetDTDName */
   {TEXT("title"), SPACE, GraphML_EL_title, L_Undefined},
   {TEXT(""), SPACE, 0, L_Undefined}	/* Last entry. Mandatory */
};
#else /* GRAPHML */
/* there is no mapping table of GraphML elements */

static ElemMapping *GraphMLElemMappingTable = NULL;
#endif /* GRAPHML */

#include "fetchXMLname_f.h"

/*----------------------------------------------------------------------
   GetXHTMLSSchema returns the XHTML Thot schema for document doc.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
SSchema            GetXHTMLSSchema (Document doc)
#else
SSchema            GetXHTMLSSchema (doc)
Document	   doc;

#endif
{
  SSchema	XHTMLSSchema;

   XHTMLSSchema = TtaGetSSchema (TEXT("HTML"), doc);
   if (XHTMLSSchema == NULL)
       XHTMLSSchema = TtaNewNature(doc, TtaGetDocumentSSchema(doc),
				    TEXT("HTML"), TEXT("HTMLP"));
   return (XHTMLSSchema);
}

/*----------------------------------------------------------------------
   GetMathMLSSchema returns the MathML Thot schema for document doc.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
SSchema            GetMathMLSSchema (Document doc)
#else
SSchema            GetMathMLSSchema (doc)
Document	   doc;

#endif
{
  SSchema	MathMLSSchema;

  MathMLSSchema = TtaGetSSchema (TEXT("MathML"), doc);
  if (MathMLSSchema == NULL)
     MathMLSSchema = TtaNewNature(doc, 
				  TtaGetDocumentSSchema(doc), TEXT("MathML"),
				  TEXT("MathMLP"));
  return (MathMLSSchema);
}

/*----------------------------------------------------------------------
   GetGraphMLSSchema returns the GraphML Thot schema for document doc.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
SSchema            GetGraphMLSSchema (Document doc)
#else
SSchema            GetGraphMLSSchema (doc)
Document	   doc;

#endif
{
  SSchema	GraphMLSSchema;

  GraphMLSSchema = TtaGetSSchema (TEXT("GraphML"), doc);
  if (GraphMLSSchema == NULL)
    GraphMLSSchema = TtaNewNature(doc,
				  TtaGetDocumentSSchema(doc), TEXT("GraphML"),
				  TEXT("GraphMLP"));
  return (GraphMLSSchema);
}

/*----------------------------------------------------------------------
   GetXLinkSSchema returns the XLink Thot schema for document doc.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
SSchema            GetXLinkSSchema (Document doc)
#else
SSchema            GetXLinkSSchema (doc)
Document	   doc;

#endif
{
  SSchema	XLinkSSchema;

  XLinkSSchema = TtaGetSSchema (TEXT("XLink"), doc);
  if (XLinkSSchema == NULL)
    XLinkSSchema = TtaNewNature(doc, TtaGetDocumentSSchema(doc), TEXT("XLink"),
				TEXT("XLinkP"));
  return (XLinkSSchema);
}

/*----------------------------------------------------------------------
   GetXMLSSchema returns the XML Thot schema for document doc.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
SSchema            GetXMLSSchema (int XMLtype, Document doc)
#else
SSchema            GetXMLSSchema (XMLtype, doc)
Document	   doc;
int                XMLtype;
#endif
{
  if (XMLtype == XHTML_TYPE)
    return GetXHTMLSSchema (doc);
  else if (XMLtype == MATH_TYPE)
    return GetMathMLSSchema (doc);
  else if (XMLtype == GRAPH_TYPE)
    return GetGraphMLSSchema (doc);
  else if (XMLtype == XLINK_TYPE)
    return GetXLinkSSchema (doc);
  else
    return NULL;
}


/*----------------------------------------------------------------------
  MapXMLElementType
  Generic function which searchs in the Element Mapping table, selected
  by the parameter XMLtype, the entry XMLname and returns the corresponding
  Thot element type.
   Returns:
    - ElTypeNum and ElSSchema into elType  ElTypeNum = 0 if not found.
    - content 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void               MapXMLElementType (int XMLtype,
				      STRING XMLname,
				      ElementType *elType,
				      STRING *mappedName,
				      CHAR_T *content,
				      Document doc)
#else
void               MapXMLElementType (XMLtype,
				      XMLname,
				      elType,
				      mappedName,
				      content,
				      doc)
int                XMLtype;
STRING             XMLname;
ElementType       *elType;
STRING            *mappedName;
CHAR_T  	  *content;
Document           doc;
#endif
{
   int                 i;
   ElemMapping        *ptr;

   /* Select the right table */
   if (XMLtype == XHTML_TYPE)
     ptr = XHTMLElemMappingTable;
   else if (XMLtype == MATH_TYPE)
     ptr = MathMLElemMappingTable;
   else if (XMLtype == GRAPH_TYPE)
     ptr = GraphMLElemMappingTable;
   else
     ptr = NULL;
   *mappedName = NULL;
   elType->ElTypeNum = 0;
   if (ptr != NULL)
     {
       /* search in the ElemMappingTable */
       i = 0;
       /* look for the first concerned entry in the table */
       while (ptr[i].XMLname[0] < XMLname[0] && ptr[i].XMLname[0] != WC_EOS)
	 i++;

       /* look at all entries starting with the right character */
       do
	 if (ustrcmp (ptr[i].XMLname, XMLname) || ptr[i].Level > ParsingLevel[doc])
	   /* it's not the tag or this tag is not valid for the current parsing level */
	   i++;
	 else
	   {
	     elType->ElTypeNum = ptr[i].ThotType;
	     if (elType->ElSSchema == NULL)
	       elType->ElSSchema = GetXMLSSchema (XMLtype, doc);
	     *mappedName = ptr[i].XMLname;
	     *content = ptr[i].XMLcontents;
	   }
       while (elType->ElTypeNum <= 0 && ptr[i].XMLname[0] == XMLname[0]);
     }
}


/*----------------------------------------------------------------------
   GetXMLElementName
   Generic function which searchs in the mapping table the XML name for
   a given Thot type.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void              GetXMLElementName (ElementType elType, CHAR_T** buffer)
#else
void              GetXMLElementName (elType, buffer)
ElementType       elType;
CHAR_T**          buffer;
#endif
{
   int                 i;
   ElemMapping        *ptr;
   STRING              name;

   if (elType.ElTypeNum > 0)
     {
	i = 0;
	/* Select the table which matches with the element schema */
	name = TtaGetSSchemaName (elType.ElSSchema);
	if (ustrcmp (TEXT("MathML"), name) == 0)
	  ptr = MathMLElemMappingTable;
	else if (ustrcmp (TEXT("GraphML"), name) == 0)
	  ptr = GraphMLElemMappingTable;
        else
	  ptr = NULL;

	if (ptr)
	  do
	    {
	    if (ptr[i].ThotType == elType.ElTypeNum)
	      {
		*buffer = ptr[i].XMLname;
		return;
	      }
	    i++;
	    }
	  while (ptr[i].XMLname[0] != WC_EOS);	  
     }
   *buffer = TEXT("???");
   return;
}
/*----------------------------------------------------------------------
   XmlMapAttribute
   Generic function which searchs in the Attribute Mapping Table (table)
   the entry attrName associated to the element elementName.
   Returns the corresponding entry.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int               XmlMapAttribute (CHAR_T *attrName,
				   CHAR_T *elementName,
				   Document doc,
				   AttributeMapping table[])
#else
int               XmlMapAttribute (attrName,
				   elementName,
				   doc,
				   table)
CHAR_T           *attrName;
CHAR_T           *elementName;
Document          doc;
AttributeMapping  table[];
#endif
{
  int             i;

  i = 0;
  /* look for the first concerned entry in the table */
  while (table[i].XMLattribute[0] < attrName[0] && table[i].XMLattribute[0] != WC_EOS)
    i++;
  while (table[i].XMLattribute[0] == attrName[0])
    {
      if (table[i].Level > ParsingLevel[doc] ||
	  ustrcasecmp (table[i].XMLattribute, attrName) ||
	  (table[i].XMLelement[0] != WC_EOS && ustrcasecmp (table[i].XMLelement, elementName)))
	i++;
      else
	return (i);
    }
  return (i);
}
