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
   {"XMLcomment", SPACE, MathML_EL_XMLcomment},
   {"XMLcomment_line", SPACE, MathML_EL_XMLcomment_line},
   {"maligngroup", 'E', MathML_EL_MALIGNGROUP},
   {"malignmark", 'E', MathML_EL_MALIGNMARK},
   {"merror", SPACE, MathML_EL_MERROR},
   {"mf", SPACE, MathML_EL_MF},  /* for compatibility with an old version of
				    MathML: WD-math-970704 */
   {"mfenced", SPACE, MathML_EL_MFENCED},
   {"mfrac", SPACE, MathML_EL_MFRAC},
   {"mi", SPACE, MathML_EL_MI},
   {"mmultiscripts", SPACE, MathML_EL_MMULTISCRIPTS},
   {"mn", SPACE, MathML_EL_MN},
   {"mo", SPACE, MathML_EL_MO},
   {"mover", SPACE, MathML_EL_MOVER},
   {"mpadded", SPACE, MathML_EL_MPADDED},
   {"mphantom", SPACE, MathML_EL_MPHANTOM},
   {"mprescripts", SPACE, MathML_EL_PrescriptPairs},
   {"mroot", SPACE, MathML_EL_MROOT},
   {"mrow", SPACE, MathML_EL_MROW},
   {"ms", SPACE, MathML_EL_MS},
   {"mspace", 'E', MathML_EL_MSPACE},
   {"msqrt", SPACE, MathML_EL_MSQRT},
   {"mstyle", SPACE, MathML_EL_MSTYLE},
   {"msub", SPACE, MathML_EL_MSUB},
   {"msubsup", SPACE, MathML_EL_MSUBSUP},
   {"msup", SPACE, MathML_EL_MSUP},
   {"mtable", SPACE, MathML_EL_MTABLE},
   {"mtd", SPACE, MathML_EL_MTD},
   {"mtext", SPACE, MathML_EL_MTEXT},
   {"mtr", SPACE, MathML_EL_MTR},
   {"munder", SPACE, MathML_EL_MUNDER},
   {"munderover", SPACE, MathML_EL_MUNDEROVER},
   {"none", SPACE, MathML_EL_Construct},
   {"sep", 'E', MathML_EL_SEP},
   {"", SPACE, 0}	/* Last entry. Mandatory */
};

#ifdef GRAPHML
/* mapping table of GraphML elements */
#include "GraphML.h"
static ElemMapping    GraphMLElemMappingTable[] =
{
   /* This table MUST be in alphabetical order */
   {"XMLcomment", SPACE, GraphML_EL_XMLcomment},
   {"XMLcomment_line", SPACE, GraphML_EL_XMLcomment_line},
   {"circle", SPACE, GraphML_EL_Circle},
   {"closedspline", SPACE, GraphML_EL_ClosedSpline},
   {"group", SPACE, GraphML_EL_Group},
   {"label", 'X', GraphML_EL_Label},	/* see function GraphMLGetDTDName */
   {"line", 'E', GraphML_EL_Line_},
   {"math", 'X', GraphML_EL_Math},	/* see function GraphMLGetDTDName */
   {"oval", SPACE, GraphML_EL_Oval},
   {"polygon", SPACE, GraphML_EL_Polygon},
   {"polyline", 'E', GraphML_EL_Polyline},
   {"rect", SPACE, GraphML_EL_Rectangle},
   {"roundrect", SPACE, GraphML_EL_RoundRect},
   {"spline", 'E', GraphML_EL_Spline},
   {"text", 'X', GraphML_EL_Text_},      /* see function GraphMLGetDTDName */
   {"", SPACE, 0}	/* Last entry. Mandatory */
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

   MathMLSSchema = TtaGetSSchema ("MathML", doc);
   if (MathMLSSchema == NULL)
      MathMLSSchema = TtaNewNature(TtaGetDocumentSSchema(doc), "MathML", "MathMLP");
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

  GraphMLSSchema = TtaGetSSchema ("GraphML", doc);
  if (GraphMLSSchema == NULL)
    GraphMLSSchema = TtaNewNature(TtaGetDocumentSSchema(doc), "GraphML", "GraphMLP");
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
void               MapXMLElementType (int XMLtype, char* XMLname, ElementType *elType, char** mappedName, char* content, Document doc)
#else
void               MapXMLElementType (XMLtype, XMLname, elType, mappedName, content, doc)
int                XMLtype;
char*              XMLname;
ElementType*       elType;
char**             mappedName;
char* 	           content;
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
	 if (strcmp (ptr[i].XMLname, XMLname))
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
void              GetXMLElementName (ElementType elType, char** buffer)
#else
void              GetXMLElementName (elType, buffer)
ElementType       elType;
char**            buffer;
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
