/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifndef PARSER_H
#define PARSER_H

#define MATH_TYPE 1
#define GRAPH_TYPE 2

#define MaxTypeNameLength 16
typedef CHAR_T typeName[MaxTypeNameLength];

typedef struct _ElemMapping
  {		/* mapping of a XML element */
     typeName	XMLname;	/* name of the XML element */
     char	XMLcontents;	/* info about the contents of the XML element:
				   'E'= empty,  space = some element(s),
				   'X'= an element from another DTD */
     int	ThotType;	/* type of the Thot element or attribute */
  }
ElemMapping;

typedef struct _AttributeMapping
  {		/* mapping of a XML attribute */
     CHAR_T   XMLattribute[30];	/* name of XML attribute */
     typeName XMLelement;		/* name of XML element type */
     char     AttrOrContent;		/* info about the corresponding Thot
					   thing: 'A'=Attribute, 'C'=Content
						  SPACE= Nothing */
     int	ThotAttribute;		/* Thot attribute */
  }
AttributeMapping;

typedef struct _AttrValueMapping
  {		/* mapping of a XML attribute value */
     int        ThotAttr;	/* corresponding Thot attribute */
     CHAR_T     XMLattrValue[20];	/* XML value */
     int        ThotAttrValue;		/* corresponding value of the Thot
						   attribute */
  }
AttrValueMapping;

#endif /* PARSER_H */











