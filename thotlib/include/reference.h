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
 
#ifndef _REFERENCE_H_
#define _REFERENCE_H_
#include "tree.h"
#include "attribute.h"

#ifndef __CEXTRACT__
#ifdef __STDC__


/*----------------------------------------------------------------------
   TtaSetReference

   Changes (or sets) the target of a reference element. The reference element
   must be part of an abstract tree.

   Parameters:
   element: the reference element to be set.
   document: the document containing the reference element.
   target : the target element (NULL for resetting the reference).
   targetDocument: the document containing the target element.

  ----------------------------------------------------------------------*/
extern void         TtaSetReference (Element element, Document document, Element target, Document targetDocument);

/*----------------------------------------------------------------------
   TtaNewInclusion

   Creates an inclusion of a given element.

   Parameters:
   document: the document for which the inclusion is created.
   target: the element to be included.
   targetDocument: the document containing the element to be included.

   Return value:
   the created inclusion.

  ----------------------------------------------------------------------*/
extern Element      TtaNewInclusion (Document document, Element target, Document targetDocument);

/*----------------------------------------------------------------------
   TtaCopyReference

   Copies a reference element into another reference element.
   Both reference elements must be in an abstract tree.

   Parameters:
   element: the reference element to be set.
   source: the element to be copied.
   document: the concerned document
  ----------------------------------------------------------------------*/
extern void         TtaCopyReference (Element element, Element source, Document document);

/*----------------------------------------------------------------------
   TtaSetAttributeReference

   Changes the value of an attribute of type reference

   Parameters:
   attribute: the attribute to be changed.
   element: the element with which the attribute is associated.
   document: the document containing the attribute.
   target: the target element (NULL for resetting the reference).
   targetDocument: the document containing the target element.

  ----------------------------------------------------------------------*/
extern void         TtaSetAttributeReference (Attribute attribute, Element element, Document document, Element target, Document targetDocument);

/*----------------------------------------------------------------------
   TtaCopyAttributeReference

   Copies the reference attribute source into the reference attribute attribute.
   Both attributes must be attached to an element in an abstract tree.

   Parameters:
   attribute: the reference attribute to be set.
   element: the element to which attribute is attached.
   source : the source attribute.

  ----------------------------------------------------------------------*/
extern void         TtaCopyAttributeReference (Attribute attribute, Element element, Attribute source);

/* ----------------------------------------------------------------------
   TtaCopyIncludedElem

   Up to date the value of inclusion element with the value of its source.

   Parameters:
   element: the element to be up to dated.
   document: the document that contains the element.

   ---------------------------------------------------------------------- */
extern void	    TtaCopyIncludedElem (Element element, Document document);

/* ----------------------------------------------------------------------
   TtaUpdateInclusionElements

   Up to date the value of inclusions that belong to the document.
   If loadExternalDoc is TRUE, the inclusions whose the sources belong to
   another document, are up to date too. In this case, the others documents
   are opened temporarely. If removeExclusions is TRUE, the exclusions
   are removed from the documents opened temporarely.

   Parameters:
   document: the document in question.
   loadExternalDoc: TRUE if it is necessary to up to date the inclusions
   whose the sources belong to another (external) document.
   removeExclusions : TRUE if exclusions of external documents have to be
   removed when these ones are temporarely opened.

   ---------------------------------------------------------------------- */
extern void         TtaUpdateInclusionElements (Document document, ThotBool loadExternalDoc, ThotBool removeExclusions);

/*----------------------------------------------------------------------
   TtaGiveReferredElement

   Returns the element referred by a given reference element.

   Parameter:
   element: the reference element.

   Return parameters:
   target: the referred element, or NULL if that element is not
   accessible (empty reference or referred document not open).
   targetDocumentName: name of the document containing the referred element;
   empty string if the referred element is in the same document as the
   reference element.
   targetDocument: the document containing the referred element;
   0 if the document containing the referred element is not loaded or
   if the referred element is in the same document as the reference
   element.

  ----------------------------------------------------------------------*/
extern void         TtaGiveReferredElement (Element element, /*OUT*/ Element * target, /*OUT*/ STRING targetDocumentName, /*OUT*/ Document * targetDocument);

/*----------------------------------------------------------------------
   TtaIsElementTypeReference

   Indicates whether an element type is a reference.

   Parameter:
   elementType: type to be tested.

   Return value:
   1 = the type is a reference, 0 = the type is not a reference.

  ----------------------------------------------------------------------*/
extern int          TtaIsElementTypeReference (ElementType elementType);

/*----------------------------------------------------------------------
   TtaSameReferences

   Compares two reference elements.

   Parameters:
   element1: first reference element.
   element2: second reference element.

   Return value:
   0 if both references are different, 1 if they are identical.

  ----------------------------------------------------------------------*/
extern int          TtaSameReferences (Element element1, Element element2);

/*----------------------------------------------------------------------
   TtaGiveReferenceAttributeValue

   Returns the value of a given attribute of type reference

   Parameter:
   attribute: the attribute of interest.

   Return parameters:
   target: the element referred by the attribute, or NULL
   if that element is not accessible (empty reference or referred
   document not loaded)
   targetDocumentName: name of the document containing the referred element;
   empty string if the referred element is in the same document as the
   attribute.
   targetDocument: the document containing the referred element;
   0 if the document containing the referred element is not loaded or
   if the referred element is in the same document as the attribute.

  ----------------------------------------------------------------------*/
extern void         TtaGiveReferenceAttributeValue (Attribute attribute, /*OUT*/ Element * target, /*OUT*/ STRING targetDocumentName, /*OUT*/ Document * targetDocument);

/*----------------------------------------------------------------------
   TtaIsElementReferred

   Tells whether a given element is the target of a reference or not.

   Parameter:
   element: the element.

   Return value:
   1 if the element is referred by another element or an
   attribute, 0 if not.

  ----------------------------------------------------------------------*/
extern int          TtaIsElementReferred (Element element);

/*----------------------------------------------------------------------
   TtaSameReferenceAttributes

   Compares two reference attributes.

   Parameters:
   attribute1: first reference attribute.
   attribute2: second reference attribute.

   Return value:
   0 if both references are different, 1 if they are identical.

  ----------------------------------------------------------------------*/
extern int          TtaSameReferenceAttributes (Attribute attribute1, Attribute attribute2);

/*----------------------------------------------------------------------
   TtaNextLoadedReference

   Searches a reference that has a given element as a target and that
   belongs to a document currently loaded.

   Parameters:
   target: the target element.
   targetDocument: the document to which the target element belongs.
   referenceElement: previous element found by the function; NULL if the first
   reference is searched. (Both referenceElement and referenceAttribute
   must be NULL if the first reference is searched.)
   referenceAttribute: previous reference attribute found; NULL if the first
   reference is searched or if the previous reference found by the
   function was an element.
   referenceDocument: document to which the previous reference found belongs;
   Zero if the first reference is searched.

   Return parameters:
   referenceAttribute: the reference attribute found. NULL if the reference
   found is an element or if no reference is found.
   referenceElement: the element found. If referenceAttribute is NULL, this
   element is a reference element, else it is the element with which the
   reference attribute found is associated. If both referenceAttribute
   and referenceElement are NULL, then no reference has been found.
   referenceDocument: the document to which the reference found belongs.
   Zero if no reference has been found.

  ----------------------------------------------------------------------*/
extern void         TtaNextLoadedReference (Element target, Document targetDocument, /*OUT*/ Element * referenceElement, /*OUT*/ Attribute * referenceAttribute, /*OUT*/ Document * referenceDocument);

/*----------------------------------------------------------------------
   TtaNextUnloadedReferringDocument

   Returns the name of a document that is not currently loaded and that contains
   references to a given target element.

   Parameters:
   target: the target element.
   targetDocument: the document to which the target element belongs.
   referringDocumentName: name of the previous document found. PcEmpty string
   if the first referring document is searched.

   Return parameter:
   referringDocumentName: name of the document found. PcEmpty string if no
   referring document has been found.

  ----------------------------------------------------------------------*/
extern void         TtaNextUnloadedReferringDocument (Element target, Document targetDocument, /*OUT*/ STRING referringDocumentName);

/*----------------------------------------------------------------------
   TtaSearchReferenceElement

   Searches the next reference element.
   Searching can be done in a subtree or starting from a given element towards
   the beginning or the end of the abstract tree.

   Parameters:
   scope: SearchForward, SearchBackward or SearchInTree.
   element: the element that is the root of the subtree
   (if scope = SearchInTree) or the starting element
   (if scope = SearchForward or SearchBackward).

   Return value:
   the element found, or NULL if not found.

  ----------------------------------------------------------------------*/
extern Element      TtaSearchReferenceElement (SearchDomain scope, Element element);

#else  /* __STDC__ */

extern void         TtaSetReference ( /* Element element, Document document, Element target, Document targetDocument */ );
extern Element      TtaNewInclusion ( /* Document document, Element target, Document targetDocument */ );
extern void         TtaCopyReference ( /* Element element, Element source, Document document */ );
extern void         TtaSetAttributeReference ( /* Attribute attribute, Element element, Document document, Element target, Document targetDocument */ );
extern void         TtaCopyAttributeReference ( /* Attribute attribute, Element element, Attribute source */ );
extern void         TtaGiveReferredElement ( /* Element element, Element *target, char *targetDocumentName, Document *targetDocument */ );
extern int          TtaIsElementTypeReference ( /* ElementType elementType */ );
extern int          TtaSameReferences ( /* Element element1, Element element2 */ );
extern void         TtaGiveReferenceAttributeValue ( /* Attribute attribute, Element *target, char *targetDocumentName, Document *targetDocument */ );
extern int          TtaIsElementReferred ( /* Element element */ );
extern int          TtaSameReferenceAttributes ( /* Attribute attribute1, Attribute attribute2 */ );
extern void         TtaNextLoadedReference ( /* Element target, Document targetDocument, Element * referenceElement, Attribute * referenceAttribute, Document * referenceDocument */ );
extern void         TtaNextUnloadedReferringDocument ( /* Element target, Document targetDocument, char * referringDocumentName */ );
extern Element      TtaSearchReferenceElement ( /* SearchDomain scope, Element element */ );

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */

#endif
