#include "application.h"
#include "app.h"

/*-------------------------------------------------------------------------
  Namespace prefix: strucutres used for storing namespace 
                    prefixs contained in documents
-------------------------------------------------------------------------*/
typedef struct _PrefixType {
  SSchema              Schema;        /* The corresponding SSchema */
  char                *SchemaName;    /* The corresponding SSchema name */
  char                *Name;          /* Prefix's id */
  struct _PrefixType  *Next;          /* Next prefix */
}PrefixType;

/*-------------------------------------------------------------------------
  Special XML/THOT attribute: structure used for the treatment of special 
                              Thot and XML attribute. used for parsing
			      It associates an attr name with an action.
			      Used in thotmodule.c and xmlmodule.c
--------------------------------------------------------------------------*/
typedef struct _XmlAttrEntry {
  unsigned char AttrName[32];          /* Attr name */
  Proc          AttrAction;            /* Action    */
}XmlAttrEntry;


#define TYPETABLE unsigned char
/*-------------------------------------------------------------------------
  structures for the translation of Xml <-> S names:
              used in translatexml.c
  --------------------------------------------------------------------------*/
/* reference to an XML attribute name and its values */
typedef struct _AttrTable 
{
  TYPETABLE  AttrNameRank;    /* rank in the XmlNameTable of the */
                              /* attribute type */
  TYPETABLE  AttrFirstValue;  /* rank in the XmlNameTable of the */
		              /* first attribute value */
                              /* 0 if the attribute is NOT enum */
  TYPETABLE  AttrNbValue;     /* number of attribute values */
                              /* 0 if the attribute is NOT enum */
}AttrTable;

/* translation schema */
typedef struct _TranslationSchema *PtrTranslationSchema;
typedef struct _TranslationSchema
{
  char       *SchemaName;      /* the SSchema which was given to construct */
                               /* the tables */
  TYPETABLE   FirstAttr;       /* the rank of the first attribute name */
  TYPETABLE   FirstAttrValue;  /* the rank of the first attribute value */
  TYPETABLE   XmlElemNameTable[MAXNTYPE]; /* entry in the XmlNameTable for */
 		                          /* each thot element type */
  AttrTable   XmlAttrNameTable[MAXNTYPE]; /* entry in the XmlNameTable for */
		                          /* thot attr type and values */
  char       *XmlNameTable[MAXNTYPE];     /* the alphabetical ordered */
                                          /* Xml name table */
  TYPETABLE   XmlReverseTable[MAXNTYPE];  /* The reverse of XmlElemNameTable */
                                          /* and XmlAttrNameTable */
  PtrTranslationSchema Next;
}TranslationSchema;

