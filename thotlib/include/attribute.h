/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Warning:
 * This module is part of the Thot library, which was originally
 * developed in French. That's why some comments are still in
 * French, but their translation is in progress and the full module
 * will be available in English in the next release.
 * 
 */
 
#ifndef _ATTRIBUTE_H_
#define _ATTRIBUTE_H_
#include "tree.h"

/* attribute */
typedef int        *Attribute;

/* attribute type */
typedef struct _AttributeType
  {
     SSchema             AttrSSchema;
     int                 AttrTypeNum;
  }
AttributeType;

#ifndef __CEXTRACT__
#ifdef __STDC__

extern Attribute    TtaNewAttribute (AttributeType attributeType);
extern void         TtaAttachAttribute (Element element, Attribute attribute, Document document);
extern void         TtaRemoveAttribute (Element element, Attribute attribute, Document document);
extern void         TtaSetAttributeValue (Attribute attribute, int value, Element element, Document document);
extern void         TtaSetAttributeText (Attribute attribute, char *buffer, Element element, Document document);
extern void         TtaNextAttribute (Element element, Attribute * attribute);
extern Attribute    TtaGetAttribute (Element element, AttributeType attributeType);
extern void         TtaGiveAttributeType (Attribute attribute, AttributeType * attributeType, int *attrKind);
extern void         TtaGiveAttributeTypeFromName (char *name, Element element, AttributeType * attributeType, int *attrKind);
extern char        *TtaGetAttributeName (AttributeType attributeType);
extern int          TtaSameAttributeTypes (AttributeType type1, AttributeType type2);
extern int          TtaGetAttributeValue (Attribute attribute);
extern int          TtaGetTextAttributeLength (Attribute attribute);
extern void         TtaGiveTextAttributeValue (Attribute attribute, char *buffer, int *length);
extern void         TtaSearchAttribute (AttributeType searchedAttribute, SearchDomain scope, Element element, Element * elementFound, Attribute * attributeFound);

#else  /* __STDC__ */

extern Attribute    TtaNewAttribute ( /* AttributeType attributeType */ );
extern void         TtaAttachAttribute ( /* Element element, Attribute attribute, Document document */ );
extern void         TtaRemoveAttribute ( /* Element element, Attribute attribute, Document document */ );
extern void         TtaSetAttributeValue ( /* Attribute attribute, int value, Element element, Document document */ );
extern void         TtaSetAttributeText ( /* Attribute attribute, char *buffer, Element element, Document document */ );
extern void         TtaNextAttribute ( /* Element element, Attribute *attribute */ );
extern Attribute    TtaGetAttribute ( /* Element element, AttributeType attributeType */ );
extern void         TtaGiveAttributeType ( /* Attribute attribute, AttributeType *attributeType, int *attrKind */ );
extern void         TtaGiveAttributeTypeFromName ( /* char *name, Element element, AttributeType *attributeType, int *attrKind */ );
extern char        *TtaGetAttributeName ( /* AttributeType attributeType */ );
extern int          TtaSameAttributeTypes ( /* AttributeType type1, AttributeType type2 */ );
extern int          TtaGetAttributeValue ( /* Attribute attribute */ );
extern int          TtaGetTextAttributeLength ( /* Attribute attribute */ );
extern void         TtaGiveTextAttributeValue ( /* Attribute attribute, char *buffer, int *length */ );
extern void         TtaSearchAttribute ( /* AttributeType searchedAttribute, SearchDomain scope, Element element, Element *elementFound, Attribute *attributeFound */ );

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */

#endif
