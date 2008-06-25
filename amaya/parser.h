/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2007
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifndef PARSER_H
#define PARSER_H

#include "profiles.h"
#define XHTML_TYPE   1
#define MATH_TYPE    2
#define SVG_TYPE     3
#define XLINK_TYPE   4
#define ANNOT_TYPE   5
#define XML_TYPE     6
#define Template_TYPE 7


/* Supported namespace declarations */
#define MAX_URI_NAME_LENGTH  60
#define XHTML_URI       "http://www.w3.org/1999/xhtml"
#define MathML_URI      "http://www.w3.org/1998/Math/MathML"
#define SVG_URI         "http://www.w3.org/2000/svg"
#define XLink_URI       "http://www.w3.org/1999/xlink"
#define XLink_PREFIX    "xlink"
#define Template_PREFIX "t"
#define NAMESPACE_URI   "http://www.w3.org/XML/1998/namespace"

/* RDFa namespace declarations */
#define RDF_URI        "http://www.w3.org/1999/02/22-rdf-syntax-ns#"
#define RDF_PREFIX     "rdf"
#define RDFS_URI       "http://www.w3.org/2000/01/rdf-schema#"
#define RDFS_PREFIX    "rdfs"
#define FOAF_URI       "http://xmlns.com/foaf/0.1/"
#define FOAF_PREFIX    "foaf"
#define DC_URI         "http://purl.org/dc/elements/1.1/"
#define DC_PREFIX      "dc"
#define OWL_URI        "http://www.w3.org/2002/07/owl#"
#define OWL_PREFIX     "owl"
#define XSD_URI        "http://www.w3.org/2001/XMLSchema#"
#define XSD_PREFIX     "xsd"
#define VCARD_URI      "http://www.w3.org/2001/vcard-rdf/3.0#"
#define VCARD_PREFIX   "vcard"
#define GEO_URI        "http://www.w3.org/2003/01/geo/wgs84_pos#"
#define GEO_PREFIX     "geo"
#define CONTACT_URI    "http://www.w3.org/2000/10/swap/pim/contact#"
#define CONTACT_PREFIX "contact"
#define AIR_URI        "http://www.daml.org/2001/10/html/airport-ont#"
#define AIR_PREFIX     "air"
#define SRW_URI        "http://purl.org/net/inkel/rdf/schemas/lang/1.1#"
#define SRW_PREFIX     "srw"
#define BIO_URI        "http://purl.org/vocab/bio/0.1/"
#define BIO_PREFIX     "bio"
#define WN_URI         "http://xmlns.com/wordnet/1.6/"
#define WN_PREFIX      "wn"

#define MaxTypeNameLength 30
#define DummyAttribute    500
typedef char typeName[MaxTypeNameLength];

typedef struct _ElemMapping
{	     /* mapping of a XML element */
  typeName   XMLname;	        /* name of the XML element */
  char       XMLcontents;	/* info about the contents of the XML element:
				   'E'= empty,  space = some element(s),
				   'X'= an element from another DTD */
  int	     ThotType;          /* type of the Thot element or attribute */
  int        Level;             /* XHTML level basic, strict, transitional */
  ThotBool   InlineElem;            /* TRUE for character level elements */
}
ElemMapping;

typedef struct _AttributeMapping
{	     /* mapping of a XML attribute */
  char       XMLattribute[30]; /* name of XML attribute */
  typeName   XMLelement;       /* name of XML element type */
  char       AttrOrContent;    /* info about the corresponding Thot
				  thing: 'A'=Attribute, 'C'=Content
				  SPACE= Nothing */
  int        ThotAttribute;    /* Thot attribute */
  int        Level;            /* XHTML level basic, strict, transitional */
}
AttributeMapping;

typedef struct _AttrValueMapping
{	     /* mapping of a XML attribute value */
  int        ThotAttr;	        /* corresponding Thot attribute */
  char       XMLattrValue[24];	/* XML value */
  int        ThotAttrValue;	/* corresponding value of the Thot attribute */
}
AttrValueMapping;

/* information about an entity being read maximum size entity */
#define MaxEntityLength 80

typedef struct _ParserData
{	     /* global variables used by XML and HTML parsers */
  Document   doc;                   /* the Thot document */
  CHARSET    encoding;              /* encoding of the document */
  Language   language;              /* language used in the document */
  Element    lastElement;	    /* last element created */
  int        withinTable;           /* <TABLE> has been read */
  ThotBool   lastElementClosed;     /* last element is complete */
  ThotBool   mergeText;	            /* character data should be catenated */
  ThotBool   parsingCSS;            /* reading the content of a STYLE element */
  ThotBool   parsingTextArea;       /* reading the content of a textarea element */
  ThotBool   parsingScript;         /* reading the content of a script element */
  ThotBool   readingAnAttrValue;
}
ParserData;

typedef struct _XmlEntity
{
  const char   *charName;      /* entity name */
  int           charCode;      /* decimal code of ISO-Latin1 char */
}
XmlEntity;

/* Error types for the XML parser*/
typedef enum
{
  errorEncoding,
  errorNotWellFormed,
  errorCharacterNotSupported,
  errorParsing,
  errorParsingProfile,
  undefinedEncoding,
  warningMessage
} ErrorType;

/* Allowed values for the attributes REL and REV in XHTML+RDFa */
#define REL_REV_Attr_Values  "alternate appendix bookmark cite chapter contants copyright glossary help icon index last license meta next p3pv1 prev role section stylesheet subsection start up"

#endif /* PARSER_H */
