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
 *
 */

#define THOT_EXPORT extern
#include "amaya.h"
#include "parser.h"
 
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

#include "fetchXMLname_f.h"

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
      MathMLSSchema = TtaNewNature(TtaGetDocumentSSchema(doc), TEXT("MathML"), TEXT("MathMLP"));
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
    GraphMLSSchema = TtaNewNature(TtaGetDocumentSSchema(doc), TEXT("GraphML"), TEXT("GraphMLP"));
  return (GraphMLSSchema);
}

/*----------------------------------------------------------------------
   GetXMLSSchema returns the XMLML Thot schema for document doc.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
SSchema            GetXMLSSchema (int XMLtype, Document doc)
#else
SSchema            GetXMLSSchema (XMLtype, doc)
Document	   doc;
int                XMLtype;
#endif
{
   if (XMLtype == MATH_TYPE)
     return GetMathMLSSchema (doc);
   else if (XMLtype == GRAPH_TYPE)
     return GetGraphMLSSchema (doc);
   else
     return NULL;
}


/*----------------------------------------------------------------------
   MapXMLElementType
   search in the mapping tables the entry for the element type of name
   XMLname and returns the corresponding Thot element type.
   Returns:
    - ElTypeNum and ElSSchema inelType  ElTypeNum = 0 if not found.
    - content 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void               MapXMLElementType (int XMLtype, STRING XMLname, ElementType *elType, STRING *mappedName, CHAR_T *content, Document doc)
#else
void               MapXMLElementType (XMLtype, XMLname, elType, mappedName, content, doc)
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
   if (XMLtype == MATH_TYPE)
     ptr = MathMLElemMappingTable;
   else if (XMLtype == GRAPH_TYPE)
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
   GetXMLElementName
   search in the mapping tables the XML name for a given Thot type
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
	  while (ptr[i].XMLname[0] != EOS);	  
     }
   *buffer = TEXT("???");
   return;
}
