/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 *
 * fetchXMLname
 *
 * Author: I. Vatton
 *         L. Carcone
 *
 */

#define THOT_EXPORT extern
#include "amaya.h"
#include "parser.h"
 
/* mapping table of XHTML elements */
static ElemMapping    XHTMLElemMappingTable[] =
{
   /* This table MUST be in alphabetical order */
   {TEXT("XMLcomment"), SPACE, HTML_EL_Comment_},
   {TEXT("XMLcomment_line"), SPACE, HTML_EL_Comment_line},
   {TEXT("a"), SPACE, HTML_EL_Anchor},
   {TEXT("abbr"), SPACE, HTML_EL_ABBR},
   {TEXT("acronym"), SPACE, HTML_EL_ACRONYM},
   {TEXT("address"), SPACE, HTML_EL_Address},
   {TEXT("applet"), SPACE, HTML_EL_Applet},
   {TEXT("area"), 'E', HTML_EL_AREA},
   {TEXT("b"), SPACE, HTML_EL_Bold_text},
   {TEXT("base"), 'E', HTML_EL_BASE},
   {TEXT("basefont"), 'E', HTML_EL_BaseFont},
   {TEXT("bdo"), SPACE, HTML_EL_BDO},
   {TEXT("big"), SPACE, HTML_EL_Big_text},
   {TEXT("blockquote"), SPACE, HTML_EL_Block_Quote},
   {TEXT("body"), SPACE, HTML_EL_BODY},
   {TEXT("br"), 'E', HTML_EL_BR},
   {TEXT("button"), SPACE, HTML_EL_BUTTON},
   {TEXT("c"), SPACE, HTML_EL_TEXT_UNIT},
   {TEXT("caption"), SPACE, HTML_EL_CAPTION},
   {TEXT("center"), SPACE, HTML_EL_Center},
   {TEXT("cite"), SPACE, HTML_EL_Cite},
   {TEXT("code"), SPACE, HTML_EL_Code},
   {TEXT("colgroup"), SPACE, HTML_EL_COLGROUP},
   {TEXT("col"), SPACE, HTML_EL_COL},
   {TEXT("dd"), SPACE, HTML_EL_Definition},
   {TEXT("del"), SPACE, HTML_EL_DEL},
   {TEXT("dfn"), SPACE, HTML_EL_Def},
   {TEXT("dir"), SPACE, HTML_EL_Directory},
   {TEXT("div"), SPACE, HTML_EL_Division},
   {TEXT("dl"), SPACE, HTML_EL_Definition_List},
   {TEXT("dt"), SPACE, HTML_EL_Term},
   {TEXT("em"), SPACE, HTML_EL_Emphasis},
   {TEXT("fieldset"), SPACE, HTML_EL_FIELDSET},
   {TEXT("font"), SPACE, HTML_EL_Font},
   {TEXT("form"), SPACE, HTML_EL_Form},
   {TEXT("frame"), 'E', HTML_EL_FRAME},
   {TEXT("frameset"), SPACE, HTML_EL_FRAMESET},
   {TEXT("h1"), SPACE, HTML_EL_H1},
   {TEXT("h2"), SPACE, HTML_EL_H2},
   {TEXT("h3"), SPACE, HTML_EL_H3},
   {TEXT("h4"), SPACE, HTML_EL_H4},
   {TEXT("h5"), SPACE, HTML_EL_H5},
   {TEXT("h6"), SPACE, HTML_EL_H6},
   {TEXT("head"), SPACE, HTML_EL_HEAD},
   {TEXT("hr"), 'E', HTML_EL_Horizontal_Rule},
   {TEXT("html"), SPACE, HTML_EL_HTML},
   {TEXT("i"), SPACE, HTML_EL_Italic_text},
   {TEXT("iframe"), SPACE, HTML_EL_IFRAME},
   {TEXT("image"), 'E', HTML_EL_PICTURE_UNIT},
   {TEXT("img"), 'E', HTML_EL_PICTURE_UNIT},
   {TEXT("input"), 'E', HTML_EL_Input},
   {TEXT("ins"), SPACE, HTML_EL_INS},
   {TEXT("isindex"), 'E', HTML_EL_ISINDEX},
   {TEXT("kbd"), SPACE, HTML_EL_Keyboard},
   {TEXT("label"), SPACE, HTML_EL_LABEL},
   {TEXT("legend"), SPACE, HTML_EL_LEGEND},
   {TEXT("li"), SPACE, HTML_EL_List_Item},
   {TEXT("link"), 'E', HTML_EL_LINK},
   {TEXT("listing"), SPACE, HTML_EL_Preformatted}, /*converted to PRE */
   {TEXT("map"), SPACE, HTML_EL_MAP},
   {TEXT("menu"), SPACE, HTML_EL_Menu},
   {TEXT("meta"), 'E', HTML_EL_META},
   {TEXT("noframes"), SPACE, HTML_EL_NOFRAMES},
   {TEXT("noscript"), SPACE, HTML_EL_NOSCRIPT},
   {TEXT("object"), SPACE, HTML_EL_Object},
   {TEXT("ol"), SPACE, HTML_EL_Numbered_List},
   {TEXT("optgroup"), SPACE, HTML_EL_OptGroup},
   {TEXT("option"), SPACE, HTML_EL_Option},
   {TEXT("p"), SPACE, HTML_EL_Paragraph},
   {TEXT("p*"), SPACE, HTML_EL_Pseudo_paragraph},
   {TEXT("param"), 'E', HTML_EL_Parameter},
   {TEXT("plaintext"), SPACE, HTML_EL_Preformatted},/* converted to PRE */
   {TEXT("pre"), SPACE, HTML_EL_Preformatted},
   {TEXT("q"), SPACE, HTML_EL_Quotation},
   {TEXT("s"), SPACE, HTML_EL_Struck_text},
   {TEXT("samp"), SPACE, HTML_EL_Sample},
   {TEXT("script"), SPACE, HTML_EL_SCRIPT},
   {TEXT("select"), SPACE, HTML_EL_Option_Menu},
   {TEXT("small"), SPACE, HTML_EL_Small_text},
   {TEXT("span"), SPACE, HTML_EL_Span},
   {TEXT("strike"), SPACE, HTML_EL_Struck_text},
   {TEXT("strong"), SPACE, HTML_EL_Strong},
   {TEXT("style"), SPACE, HTML_EL_STYLE_},
   {TEXT("sub"), SPACE, HTML_EL_Subscript},
   {TEXT("sup"), SPACE, HTML_EL_Superscript},
   {TEXT("table"), SPACE, HTML_EL_Table},
   {TEXT("tbody"), SPACE, HTML_EL_tbody},
   {TEXT("td"), SPACE, HTML_EL_Data_cell},
   {TEXT("textarea"), SPACE, HTML_EL_Text_Area},
   {TEXT("tfoot"), SPACE, HTML_EL_tfoot},
   {TEXT("th"), SPACE, HTML_EL_Heading_cell},
   {TEXT("thead"), SPACE, HTML_EL_thead},
   {TEXT("title"), SPACE, HTML_EL_TITLE},
   {TEXT("tr"), SPACE, HTML_EL_Table_row},
   {TEXT("tt"), SPACE, HTML_EL_Teletype_text},
   {TEXT("u"), SPACE, HTML_EL_Underlined_text},
   {TEXT("ul"), SPACE, HTML_EL_Unnumbered_List},
   {TEXT("var"), SPACE, HTML_EL_Variable},
   {TEXT("xmp"), SPACE, HTML_EL_Preformatted},  /* converted to PRE */
   {TEXT(""), SPACE, 0}	/* Last entry. Mandatory */
};

/* mapping table of MathML elements */
#include "MathML.h"
static ElemMapping    MathMLElemMappingTable[] =
{
   /* This table MUST be in alphabetical order */
   {TEXT("XMLcomment"), SPACE, MathML_EL_XMLcomment},
   {TEXT("XMLcomment_line"), SPACE, MathML_EL_XMLcomment_line},
   {TEXT("maligngroup"), 'E', MathML_EL_MALIGNGROUP},
   {TEXT("malignmark"), 'E', MathML_EL_MALIGNMARK},
   {TEXT("mchar"), 'E', MathML_EL_MCHAR},
   {TEXT("menclose"), SPACE, MathML_EL_MENCLOSE},
   {TEXT("merror"), SPACE, MathML_EL_MERROR},
   {TEXT("mf"), SPACE, MathML_EL_MF},  /* for compatibility with an old version of
				    MathML: WD-math-970704 */
   {TEXT("mfenced"), SPACE, MathML_EL_MFENCED},
   {TEXT("mfrac"), SPACE, MathML_EL_MFRAC},
   {TEXT("mglyph"), 'E', MathML_EL_MGLYPH},
   {TEXT("mi"), SPACE, MathML_EL_MI},
   {TEXT("mlabeledtr"), SPACE, MathML_EL_MLABELEDTR},
   {TEXT("mmultiscripts"), SPACE, MathML_EL_MMULTISCRIPTS},
   {TEXT("mn"), SPACE, MathML_EL_MN},
   {TEXT("mo"), SPACE, MathML_EL_MO},
   {TEXT("mover"), SPACE, MathML_EL_MOVER},
   {TEXT("mpadded"), SPACE, MathML_EL_MPADDED},
   {TEXT("mphantom"), SPACE, MathML_EL_MPHANTOM},
   {TEXT("mprescripts"), SPACE, MathML_EL_PrescriptPairs},
   {TEXT("mroot"), SPACE, MathML_EL_MROOT},
   {TEXT("mrow"), SPACE, MathML_EL_MROW},
   {TEXT("ms"), SPACE, MathML_EL_MS},
   {TEXT("mspace"), 'E', MathML_EL_MSPACE},
   {TEXT("msqrt"), SPACE, MathML_EL_MSQRT},
   {TEXT("mstyle"), SPACE, MathML_EL_MSTYLE},
   {TEXT("msub"), SPACE, MathML_EL_MSUB},
   {TEXT("msubsup"), SPACE, MathML_EL_MSUBSUP},
   {TEXT("msup"), SPACE, MathML_EL_MSUP},
   {TEXT("mtable"), SPACE, MathML_EL_MTABLE},
   {TEXT("mtd"), SPACE, MathML_EL_MTD},
   {TEXT("mtext"), SPACE, MathML_EL_MTEXT},
   {TEXT("mtr"), SPACE, MathML_EL_MTR},
   {TEXT("munder"), SPACE, MathML_EL_MUNDER},
   {TEXT("munderover"), SPACE, MathML_EL_MUNDEROVER},
   {TEXT("none"), SPACE, MathML_EL_Construct},
   {TEXT("sep"), 'E', MathML_EL_SEP},
   {TEXT(""), SPACE, 0}	/* Last entry. Mandatory */
};

#ifdef GRAPHML
/* mapping table of GraphML elements */
#include "GraphML.h"
static ElemMapping    GraphMLElemMappingTable[] =
{
   /* This table MUST be in alphabetical order */
   {TEXT("XMLcomment"), SPACE, GraphML_EL_XMLcomment},
   {TEXT("XMLcomment_line"), SPACE, GraphML_EL_XMLcomment_line},
   {TEXT("circle"), SPACE, GraphML_EL_Circle},
   {TEXT("closedspline"), SPACE, GraphML_EL_ClosedSpline},
   {TEXT("group"), SPACE, GraphML_EL_Group},
   {TEXT("label"), 'X', GraphML_EL_Label},	/* see function GraphMLGetDTDName */
   {TEXT("line"), 'E', GraphML_EL_Line_},
   {TEXT("math"), 'X', GraphML_EL_Math},	/* see function GraphMLGetDTDName */
   {TEXT("oval"), SPACE, GraphML_EL_Oval},
   {TEXT("polygon"), SPACE, GraphML_EL_Polygon},
   {TEXT("polyline"), 'E', GraphML_EL_Polyline},
   {TEXT("rect"), SPACE, GraphML_EL_Rectangle},
   {TEXT("roundrect"), SPACE, GraphML_EL_RoundRect},
   {TEXT("spline"), 'E', GraphML_EL_Spline},
   {TEXT("text"), 'X', GraphML_EL_Text_},      /* see function GraphMLGetDTDName */
   {TEXT(""), SPACE, 0}	/* Last entry. Mandatory */
};
#else /* GRAPHML */
/* there is no mapping table of GraphML elements */

static ElemMapping *GraphMLElemMappingTable = NULL;
#endif /* GRAPHML */

#include "XmlNameSpaces_f.h"


/*----------------------------------------------------------------------
   GetXHTML_DTD returns the MathML Thot schema for document doc.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
SSchema            GetXHTML_DTD (Document doc)
#else
SSchema            GetXHTML_DTD (doc)
Document	   doc;

#endif
{
  SSchema	XHTMLSSchema;

   XHTMLSSchema = TtaGetSSchema (TEXT("HTML"), doc);
   if (XHTMLSSchema == NULL)
       XHTMLSSchema = TtaNewNature(TtaGetDocumentSSchema(doc),
				    TEXT("HTML"), TEXT("HTMLP"));
   return (XHTMLSSchema);
}

/*----------------------------------------------------------------------
   GetMathML_DTD returns the MathML Thot schema for document doc.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
SSchema            GetMathML_DTD (Document doc)
#else
SSchema            GetMathML_DTD (doc)
Document	   doc;

#endif
{
  SSchema	MathMLSSchema;

   MathMLSSchema = TtaGetSSchema (TEXT("MathML"), doc);
   if (MathMLSSchema == NULL)
       MathMLSSchema = TtaNewNature(TtaGetDocumentSSchema(doc),
				    TEXT("MathML"), TEXT("MathMLP"));
   return (MathMLSSchema);
}

/*----------------------------------------------------------------------
   GetGraphML_DTD returns the GraphML Thot schema for document doc.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
SSchema            GetGraphML_DTD (Document doc)
#else
SSchema            GetGraphML_DTD (doc)
Document	   doc;

#endif
{
  SSchema	GraphMLSSchema;

  GraphMLSSchema = TtaGetSSchema (TEXT("GraphML"), doc);
  if (GraphMLSSchema == NULL)
      GraphMLSSchema = TtaNewNature(TtaGetDocumentSSchema(doc),
				    TEXT("GraphML"), TEXT("GraphMLP"));
  return (GraphMLSSchema);
}

/*----------------------------------------------------------------------
   GetXmlDTD returns the Thot structure schema for document doc.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
SSchema            GetXmlDTD (int XMLtype, Document doc)
#else
SSchema            GetXmlDTD (XMLtype, doc)
Document	   doc;
int                XMLtype;
#endif
{
   if (XMLtype == XHTML_TYPE)
     return GetXHTML_DTD (doc);
   else
     if (XMLtype == MATH_TYPE)
       return GetMathML_DTD (doc);
     else
       if (XMLtype == GRAPH_TYPE)
	 return GetGraphML_DTD (doc);
       else
	 return NULL;
}

/*----------------------------------------------------------------------
   MapXmlElType
   search in the mapping tables the entry for the element type of name
   XMLname and returns the corresponding Thot element type.
   Returns:
    - ElTypeNum and ElSSchema inelType  ElTypeNum = 0 if not found.
    - content 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void               MapXmlElType (int XMLtype,
				 STRING XMLname,
				 ElementType *elType,
				 STRING *mappedName,
				 CHAR_T *content,
				 Document doc)
#else
void               MapXmlElType (XMLtype,
				 XMLname,
				 elType,
				 mappedName,
				 content, doc)
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
   else
     if (XMLtype == MATH_TYPE)
       ptr = MathMLElemMappingTable;
     else
       if (XMLtype == GRAPH_TYPE)
	 ptr = GraphMLElemMappingTable;
       else
	 ptr = NULL;

   elType->ElTypeNum = 0;
   if (ptr != NULL)
     {
       /* search in the ElemMappingTable */
       i = 0;
       /* look for the first concerned entry in the table */
       while (ptr[i].XMLname[0] < XMLname[0] && ptr[i].XMLname[0] != EOS)
	 i++;
       /* look at all entries starting with the right character */
       do
	 if (ustrcmp (ptr[i].XMLname, XMLname))
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
   GetXmlElName
   search in the mapping tables the XML name for a given Thot type
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void              GetXmlElName (ElementType elType,
				CHAR_T** buffer)
#else
void              GetXmlElName (elType, buffer)
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
	  while (ptr[i].XMLname[0] != EOS);	  
     }
   *buffer = TEXT("???");
   return;
}
