/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifndef PARSER_H
#define PARSER_H

#define MATH_TYPE  1
#define GRAPH_TYPE 2
#define XLINK_TYPE 3
#define XHTML_TYPE 4

/* current HTML parsing level */
#define L_Other        0
#define L_Basic        1
#define L_Strict       2
#define L_Transitional 3

#define MaxTypeNameLength 30
#define DummyAttribute    500
typedef CHAR_T typeName[MaxTypeNameLength];

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
  CHAR_T     XMLattribute[30]; /* name of XML attribute */
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
  CHAR_T     XMLattrValue[20];	/* XML value */
  int        ThotAttrValue;	/* corresponding value of the Thot attribute */
}
AttrValueMapping;

/* information about an entity being read maximum size entity */
#define MaxEntityLength 80

typedef struct _ParserData
{	     /* global variables used by XML and HTML parsers */
  Document   doc;                   /* the Thot document */
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

typedef CHAR_T XhtmlEntityName[10];
typedef struct _XhtmlEntity
{
  XhtmlEntityName  charName;  /* entity name */
  int              charCode;  /* decimal code of ISO-Latin1 char */
}
XhtmlEntity;

#endif /* PARSER_H */











