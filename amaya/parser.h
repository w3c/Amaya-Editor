/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifndef PARSER_H
#define PARSER_H

#define MATH_TYPE    1
#define GRAPH_TYPE   2
#define XLINK_TYPE   3
#define XHTML_TYPE   4
#define XML_TYPE     5

/* Masks for the XHTML profiles */
#define L_Other            0x00
#define L_Basic            0x02
#define L_Strict           0x06
#define L_Xhtml11          0x1E
#define L_Transitional     0x2E

/* Value for the XHTML profiles */
#define L_BasicValue        0x02
#define L_StrictValue       0x0C
#define L_Xhmli11Value      0x0C
#define L_RubyValue         0x10
#define L_TransitionalValue 0x20
#define L_OtherValue        0xFF

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
  ThotBool   Inline;            /* TRUE for character level elements */
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
  ThotBool   parsingTextArea;       /* reading the content of a text area element */
  ThotBool   readingAnAttrValue;
}
ParserData;

typedef struct _XmlEntity
{
  char         *charName;      /* entity name */
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

#endif /* PARSER_H */











