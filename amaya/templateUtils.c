#include "templates.h"

/*----------------------------------------------------------------------
GetSchemaFromDocType: Returns the name of the schema corresponding to 
a doc type.
----------------------------------------------------------------------*/
char *GetSchemaFromDocType (DocumentType docType)
{
#ifdef TEMPLATES
	switch (docType)
    {
    case docAnnot :
		return "Annot";
    case docBookmark :
		return "Topics";
    case docSVG :
		return "SVG";
    case docMath :
		return "MathML";
    case docXml :
		return "XML";
    default :
		return "HTML";
    }
#endif // TEMPLATES
	return "HTML";
}

/*----------------------------------------------------------------------
Set the value of a string attribute 
----------------------------------------------------------------------*/
void SetAttributeStringValue (Element el, int att, char* value)
{
#ifdef TEMPLATES
  Document doc = TtaGetDocument(el);

  AttributeType attType;
  attType.AttrSSchema = TtaGetElementType(el).ElSSchema;
  attType.AttrTypeNum = att;
  
  Attribute attribute = TtaGetAttribute(el, attType);

  TtaSetAttributeText(attribute, value, el, doc);
#endif /* TEMPLATES */
}


/*----------------------------------------------------------------------
Returns the value of a string attribute 
----------------------------------------------------------------------*/
char *GetAttributeStringValue (Element el, int att, int* sz)
{
#ifdef TEMPLATES
	AttributeType attType;
	attType.AttrSSchema = TtaGetElementType(el).ElSSchema;
	attType.AttrTypeNum = att;
	
	Attribute attribute = TtaGetAttribute(el, attType);
	
	int size = TtaGetTextAttributeLength(attribute);
	char *aux = (char*) TtaGetMemory(size+1);
	TtaGiveTextAttributeValue (attribute, aux, &size);
  if(sz)
    *sz = size;
	return aux;
#else
	return '\0';
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
Returns the value of a string attribute 
----------------------------------------------------------------------*/
char *GetAttributeStringValue (Element el, Attribute attribute, int* sz)
{
#ifdef TEMPLATES
	int size = TtaGetTextAttributeLength(attribute);
	char *aux = (char*) TtaGetMemory(size+1);
	TtaGiveTextAttributeValue (attribute, aux, &size);
  if(sz)
    *sz = size;
	return aux;
#else
	return '\0';
#endif /* TEMPLATES */
}
