/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifndef _ATTRIBUTE_H_
#define _ATTRIBUTE_H_
#include "tree.h"

#ifndef __CEXTRACT__

/* ----------------------------------------------------------------------
   TtaNewAttribute

   Creates an attribute that will be attached to an element.
   Parameter:
   attributeType: type of the attribute to be created.
   Return value:
   the attribute that has been created.
   ---------------------------------------------------------------------- */
extern Attribute TtaNewAttribute (AttributeType attributeType);

/* ----------------------------------------------------------------------
   TtaAttachAttribute

   Attaches an attribute to an element.
   Parameters:
   element: the element to which the attribute has to be attached.
   attribute: the attribute to be attached.
   document: the document to which the element belongs.
   ---------------------------------------------------------------------- */
extern void TtaAttachAttribute (Element element, Attribute attribute, Document document);

/* ----------------------------------------------------------------------
   TtaRemoveAttribute

   Removes an attribute from an element and release that attribute.
   Parameters:
   element: the element with which the attribute is associated.
   attribute: the attribute to be removed.
   document: the document to which the element belongs.
   ---------------------------------------------------------------------- */
extern void TtaRemoveAttribute (Element element, Attribute attribute, Document document);


/* ----------------------------------------------------------------------
   TtaCopyAttributes

   Copy all attributes of the src element to the target element
   Parameters:
   src: the element which provides attributes.
   target: the element which receives attributes.
   ---------------------------------------------------------------------- */
extern void TtaCopyAttributes (Element src, Element target,
                               Document doc_src, Document doc_target);

/* ----------------------------------------------------------------------
   TtaSetAttributeValue

   Changes the value of an attribute of type integer or enumerate.
   Parameters:
   attribute: the attribute to be modified.
   value: new value of the attribute.
   element: the element with which the attribute is associated,
   NULL if the attribute is not yet associated with an element.
   document: the document to which the element belongs.
   Must be 0 if element is NULL.
   ---------------------------------------------------------------------- */
extern void TtaSetAttributeValue (Attribute attribute, int value, Element element, Document document);

/* ----------------------------------------------------------------------
   TtaSetAttributeText

   Changes the value of an attribute of type text.
   Parameters:
   attribute: the attribute to be modified.
   buffer: character string representing the new value of the attribute.
   element: the element with which the attribute is associated,
   NULL if the attribute is not yet associated with an element.
   document: the document to which the element belongs.
   Must be 0 if element is NULL.
   ---------------------------------------------------------------------- */
extern void TtaSetAttributeText (Attribute attribute, const char *buffer,
				 Element element, Document document);

/* ----------------------------------------------------------------------
   TtaNextAttribute

   Returns the first attribute associated with a given element (if attribute
   is NULL) or the attribute that follows a given attribute of a given element.
   Parameters:
   element: the element of interest.
   attribute: an attribute of that element, or NULL if the
   first attribute is asked.
   Return parameter:
   attribute: the next attribute, or NULL if attribute
   is the last attribute of the element.
   ---------------------------------------------------------------------- */
extern void TtaNextAttribute (Element element, /*INOUT*/ Attribute *attribute);

/* ----------------------------------------------------------------------
   TtaGetAttribute

   Returns an attribute of a given type associated with a given element.
   Parameters:
   element: the element of interest.
   attributeType: type of the desired attribute. If the attribute "Language"
   is searched, attributeType.AttrTypeNum must be 1. If the attribute
   "Language" is searched whatever its structure schema,
   attributeType.AttrSSchema must be NULL. A NULL
   attributeType.AttrSSchema is accepted only when an attribute
   "Language" is searched.
   Return value:
   the attribute found, or NULL if the element  does not have this
   type of attribute.
   ---------------------------------------------------------------------- */
extern Attribute TtaGetAttribute (Element element, AttributeType attributeType);

/* ----------------------------------------------------------------------
   TtaGetAttributeKind

   Returns the kind of the attribute: 0 = Enumerate, 1 = Integer, 2 = Text,
   3 = CsReference.
   Parameter:
   attType: type of the attribute.
   ---------------------------------------------------------------------- */
extern int TtaGetAttributeKind (AttributeType attType);

/* ----------------------------------------------------------------------
   TtaGiveAttributeType

   Returns the type of a given attribute.

   Parameter:
   attribute: the attribute of interest.
   Return parameters:
   attributeType: type of the attribute.
   attrKind: kind of the attribute: 0 = Enumerate, 1 = Integer, 2 = Text,
   3 = CsReference
   ---------------------------------------------------------------------- */
extern void TtaGiveAttributeType (Attribute attribute, AttributeType *attributeType,
				  int *attrKind);

/* ----------------------------------------------------------------------
   TtaGiveAttributeTypeFromName

   Retrieves the type of an attribute from its name.
   Parameter:
   name: name of the attribute.
   element: the element with which the attribute is associated.
   Return parameters:
   attributeType: type of the attribute.
   attrKind: kind of the attribute: 0 = Enumerate, 1 = Integer, 2 = Text,
   3 = CsReference
   ---------------------------------------------------------------------- */
extern void TtaGiveAttributeTypeFromName (const char *name, Element element, AttributeType *attributeType, int *attrKind);

/* ----------------------------------------------------------------------
   TtaGiveAttributeTypeFromOriginalName

   Retrieves the type of an attribute from its original name.
   Parameter:
   name: name of the attribute (in the language of the structure schema).
   element: the element with which the attribute is associated.
   Return parameters:
   attributeType: type of the attribute.
   attrKind: kind of the attribute: 0 = Enumerate, 1 = Integer, 2 = Text,
   3 = CsReference
   ---------------------------------------------------------------------- */
extern void TtaGiveAttributeTypeFromOriginalName (char *name, Element element, AttributeType *attributeType, int *attrKind);

/* ----------------------------------------------------------------------
   TtaGetAttributeName

   Returns the name of an attribute type.

   Parameter:
   attributeType: type of the attribute.

   Return value:
   name of that type.

   ---------------------------------------------------------------------- */
extern char *TtaGetAttributeName (AttributeType attributeType);

/* ----------------------------------------------------------------------
   TtaGetAttributeOriginalName

   Returns the name of an attribute type in the schema language.
   Parameter:
   attributeType: type of the attribute.
   Return value:
   name of that type.
   ---------------------------------------------------------------------- */
extern char *TtaGetAttributeOriginalName (AttributeType attributeType);

/* ----------------------------------------------------------------------
   TtaSameAttributeTypes

   Compares two attribute types.
   Parameter:
   type1: first attribute type.
   type2: second attribute type.
   Return value:
   0 if both types are different, 1 if they are identical.
   ---------------------------------------------------------------------- */
extern int TtaSameAttributeTypes (AttributeType type1, AttributeType type2);

/* ----------------------------------------------------------------------
   TtaGetAttributeValue

   Returns the value of a given attribute of type integer or enumerate.
   Parameter:
   attribute: the attribute of interest.
   Return value:
   Value of that attribute.
   ---------------------------------------------------------------------- */
extern int TtaGetAttributeValue (Attribute attribute);


/* ----------------------------------------------------------------------
   TtaGetAttributeValueName
 
   Returns the name of a value of an attribute of type enumerate.
   Parameter:
   attributeType: type of the attribute.
   value: the value
   Return value:
   name of that value or empty string if error.
   ---------------------------------------------------------------------- */
extern char *TtaGetAttributeValueName (AttributeType attributeType, int value);

/*----------------------------------------------------------------------
   TtaIsValidID
   Returns TRUE if the attribute value is valid for an ID
   else if the parameter update is TRUE updates the string name.
  ----------------------------------------------------------------------*/
extern ThotBool TtaIsValidID (Attribute attr, ThotBool update);

/* ----------------------------------------------------------------------
   TtaGetAttributeValueFromName
 
   Retrieves the int value of an attribute of type enumerate from its name.
   Parameter:
   name: name of the value.
   attrType: type of the attribute
   Return value:
   the corresponding int value, or 0 if error.
   ---------------------------------------------------------------------- */
extern int TtaGetAttributeValueFromName (char *name, AttributeType attributeType);

/* ----------------------------------------------------------------------
   TtaGetAttributeValueFromOriginalName

   Retrieves the int value of an attribute of type enumerate from its 
   original name (as it is defined in the S schema).
   Parameter:
   name: original name of the value.
   attrType: type of the attribute
   Return value:
   the corresponding int value, or 0 if error.
   ---------------------------------------------------------------------- */
extern int TtaGetAttributeValueFromOriginalName (char *name, AttributeType attributeType);


/* ----------------------------------------------------------------------
   TtaGetTextAttributeLength

   Returns the length of a given attribute of type text.
   Parameter:
   attribute: the attribute of interest.
   Return values:
   length of the character string contained in the attribute.
   ---------------------------------------------------------------------- */
extern int TtaGetTextAttributeLength (Attribute attribute);

/* ----------------------------------------------------------------------
   TtaGiveTextAttributeValue

   Returns the value of a given attribute of type text.
   Parameters:
   attribute: the attribute of interest.
   buffer: address of the buffer that will contain the value of the attribute.
   length: size of the buffer (in bytes).
   Return values:
   buffer: character string representing the value of the attribute.
   length: actual length of the character string.
   ---------------------------------------------------------------------- */
extern void TtaGiveTextAttributeValue (Attribute attribute, char *buffer, int *length);

/* ----------------------------------------------------------------------
   TtaSearchAttribute

   Searches the next element that has a given attribute.
   Searching can be done in a subtree or starting from a given element towards
   the beginning or the end of the abstract tree.
   Parameters:
   searchedAttribute: attribute to be searched.
   If searchedAttribute.AttrSSchema is NULL, the next element
   that has an attribute is searched, whatever the attribute.
   scope: SearchForward, SearchBackward or SearchInTree.
   element: the element that is the root of the tree
   (if scope = SearchInTree) or the starting element
   (if scope = SearchForward or SearchBackward).
   Return parameters:
   elementFound: the element found, or NULL if not found.
   attributeFound: the searched attribute, or NULL if not found.
   ---------------------------------------------------------------------- */
extern void TtaSearchAttribute (AttributeType searchedAttribute, SearchDomain scope, Element element, Element *elementFound, Attribute *attributeFound);

/* ----------------------------------------------------------------------
   TtaSearchAttributes

   Searches the next element that has one of given attributes.
   Searching can be done in a subtree or starting from a given element towards
   the beginning or the end of the abstract tree.
   Parameters:
   searchedAtt1, searchedAtt2: attributes to be searched.
   If searchedAtt.AttrSSchema is NULL, the next element
   that has an attribute is searched, whatever the attribute.
   scope: SearchForward, SearchBackward or SearchInTree.
   element: the element that is the root of the tree
   (if scope = SearchInTree) or the starting element
   (if scope = SearchForward or SearchBackward).
   Return parameters:
   elementFound: the element found, or NULL if not found.
   attributeFound: the searched attribute, or NULL if not 
   ---------------------------------------------------------------------- */
extern void TtaSearchAttributes (AttributeType searchedAtt1,
				 AttributeType searchedAtt2,
				 SearchDomain scope, Element element,
				 Element *elementFound, Attribute *attributeFound);

/*----------------------------------------------------------------------
   TtaGetTypedAttrAncestor

   returns a pointer to the attribute of the first
   element which encloses pEl and that has an attribute
   of type attNum. The function returns pElAttr if
   the search is succesful, NULL otherwise.
  ----------------------------------------------------------------------*/
Attribute  TtaGetTypedAttrAncestor (Element pEl, int attNum,
				    SSchema pSSattr, Element *pElAttr);

#endif /* __CEXTRACT__ */

#endif
