
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */


#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "application.h"
#include "document.h"
#include "tree.h"
#include "attribute.h"
#include "reference.h"
#include "typecorr.h"
#include "storage.h"

#undef EXPORT
#define EXPORT
#include "edit.var"

#include "refelem.f"
#include "arbabs.f"
#include "cherche.f"
#include "commun.f"
#include "ouvre.f"
#include "storage.f"
#include "thotmsg.f"
#include "modif.f"
#include "memory.f"

extern int          UserErrorCode;


/* ----------------------------------------------------------------------
   TtaSetReference

   Changes (or sets) the target of a reference element. The reference element
   must be part of an abstract tree.

   Parameters:
   element: the reference element to be set.
   document: the document containing the reference element.
   target : the target element (NULL for resetting the reference).
   targetDocument: the document containing the target element.

   ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtaSetReference (Element element, Document document, Element target, Document targetDocument)

#else  /* __STDC__ */
void                TtaSetReference (element, document, target, targetDocument)
Element             element;
Document            document;
Element             target;
Document            targetDocument;

#endif /* __STDC__ */

{
   PtrDocument         pRefDoc;
   int                 saveNbCar;
   boolean             ok;

   UserErrorCode = 0;
   if (element == NULL)
     {
	TtaError (ERR_invalid_parameter);
     }
   else if (!((PtrElement) element)->ElTerminal ||
	    ((PtrElement) element)->ElLeafType != LtReference)
     {
	TtaError (ERR_invalid_element_type);
     }
   else
     {
	/* verifie le parametre document */
	if (document < 1 || document > MAX_DOCUMENTS)
	  {
	     TtaError (ERR_invalid_document_parameter);
	  }
	else if (TabDocuments[document - 1] == NULL)
	  {
	     TtaError (ERR_invalid_document_parameter);
	  }
	else
	   /* parametre document correct */
	  {
	     ok = FALSE;
	     if (target == NULL)
		/* annulation de la reference */
	       {
		  if (((PtrElement) element)->ElReference != NULL)
		     DeleteReference (((PtrElement) element)->ElReference);
		  ok = TRUE;
	       }
	     else
		/* verifie le parametre targetDocument */
	     if (targetDocument < 1 || targetDocument > MAX_DOCUMENTS)
	       {
		  TtaError (ERR_invalid_document_parameter);
	       }
	     else if (TabDocuments[targetDocument - 1] == NULL)
	       {
		  TtaError (ERR_invalid_document_parameter);
	       }
	     else
		/* parametre targetDocument correct */
	       {
		  pRefDoc = TabDocuments[targetDocument - 1];
		  /* l'element pointe' par target va etre reference' */
		  if (((PtrElement) target)->ElReferredDescr == NULL)
		     /* cet element n'a pas encore de descripteur */
		     /* d'element reference', on lui en associe un */
		    {
		       ((PtrElement) target)->ElReferredDescr = NewReferredElDescr (pRefDoc);
		       ((PtrElement) target)->ElReferredDescr->ReReferredElem =
			  (PtrElement) target;
		    }
		  if (((PtrElement) element)->ElReference == NULL)
		     GetReference (&((PtrElement) element)->ElReference);
		  ((PtrElement) element)->ElReference->RdElement =
		     (PtrElement) element;
		  ((PtrElement) element)->ElReference->RdTypeRef = RefFollow;
		  saveNbCar = TabDocuments[document - 1]->DocNTypedChars;
		  ok = SetReference ((PtrElement) element, NULL,
				     (PtrElement) target,
				     TabDocuments[document - 1], pRefDoc,
				     TRUE, FALSE);
		  /* an API function is not supposed to change */
		  /* the number of characters typed by the user */
		  TabDocuments[document - 1]->DocNTypedChars = saveNbCar;
		  if (!ok)
		    {
		       TtaError (ERR_cannot_set_link);
		    }
	       }
#ifndef NODISPLAY
	     if (ok)
		RedispReference ((PtrElement) element, document);
#endif
	  }
     }
}


/* ----------------------------------------------------------------------
   TtaNewInclusion

   Creates an inclusion of a given element.

   Parameters:
   document: the document for which the inclusion is created.
   target: the element to be included.
   targetDocument: the document containing the element to be included.

   Return value:
   the created inclusion.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
Element             TtaNewInclusion (Document document, Element target, Document targetDocument)

#else  /* __STDC__ */
Element             TtaNewInclusion (document, target, targetDocument)
Document            document;
Element             target;
Document            targetDocument;

#endif /* __STDC__ */

{
   PtrElement          inclusion;
   PtrDocument         pRefDoc;
   int                 saveNbCar;

   UserErrorCode = 0;
   inclusion = NULL;
   /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else if (TabDocuments[document - 1] == NULL)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else
      /* parametre document correct */
   if (target == NULL)
     {
	TtaError (ERR_invalid_parameter);
     }
   else
      /* verifie le parametre targetDocument */
   if (targetDocument < 1 || targetDocument > MAX_DOCUMENTS)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else if (TabDocuments[targetDocument - 1] == NULL)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else
      /* parametre targetDocument correct */
     {
	inclusion = NewSubtree (((PtrElement) target)->ElTypeNumber,
				((PtrElement) target)->ElSructSchema,
				TabDocuments[document - 1], 0, FALSE,
				TRUE, TRUE, TRUE);
	GetReference (&inclusion->ElSource);
	inclusion->ElSource->RdElement = inclusion;
	inclusion->ElSource->RdTypeRef = RefInclusion;
	pRefDoc = TabDocuments[targetDocument - 1];
	/* l'element pointe' par target va etre reference' */
	if (((PtrElement) target)->ElReferredDescr == NULL)
	   /* cet element n'a pas encore de descripteur */
	   /* d'element reference', on lui en associe un */
	  {
	     ((PtrElement) target)->ElReferredDescr = NewReferredElDescr (pRefDoc);
	     ((PtrElement) target)->ElReferredDescr->ReReferredElem = (PtrElement) target;
	  }
	saveNbCar = TabDocuments[document - 1]->DocNTypedChars;
	if (SetReference (inclusion, NULL, (PtrElement) target,
			  TabDocuments[document - 1], pRefDoc, TRUE, FALSE))
	   CopyIncludedElem (inclusion, TabDocuments[document - 1]);
	else
	  {
	     TtaError (ERR_cannot_set_link);
	  }
	/* an API function is not supposed to change */
	/* the number of characters typed by the user */
	TabDocuments[document - 1]->DocNTypedChars = saveNbCar;
     }
   return ((Element) inclusion);
}


/* ----------------------------------------------------------------------
   TtaCopyReference

   Copies a reference element into another reference element.
   Both reference elements must be in an abstract tree.

   Parameters:
   element: the reference element to be set.
   source : the element to be copied.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TtaCopyReference (Element element, Element source)

#else  /* __STDC__ */
void                TtaCopyReference (element, source)
Element             element;
Element             source;

#endif /* __STDC__ */

{

   UserErrorCode = 0;
   if (element == NULL || source == NULL)
     {
	TtaError (ERR_invalid_parameter);
     }
   else if (!((PtrElement) element)->ElTerminal ||
	    ((PtrElement) element)->ElLeafType != LtReference)
     {
	TtaError (ERR_invalid_element_type);
     }
   else if (!((PtrElement) source)->ElTerminal ||
	    ((PtrElement) source)->ElLeafType != LtReference)
     {
	TtaError (ERR_invalid_element_type);
     }
   else
     {
	CancelReference ((PtrElement) element);
	if (((PtrElement) source)->ElReference != NULL)
	  {
	     if (((PtrElement) element)->ElReference == NULL)
		GetReference (&((PtrElement) element)->ElReference);
	     CopyReference (((PtrElement) element)->ElReference,
		 ((PtrElement) source)->ElReference, (PtrElement *) & element);
	  }
     }
}


/* ----------------------------------------------------------------------
   TtaSetAttributeReference

   Changes the value of an attribute of type reference

   Parameters:
   attribute: the attribute to be changed.
   element: the element with which the attribute is associated.
   document: the document containing the attribute.
   target: the target element (NULL for resetting the reference).
   targetDocument: the document containing the target element.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TtaSetAttributeReference (Attribute attribute, Element element, Document document, Element target, Document targetDocument)

#else  /* __STDC__ */
void                TtaSetAttributeReference (attribute, element, document, target, targetDocument)
Attribute           attribute;
Element             element;
Document            document;
Element             target;
Document            targetDocument;

#endif /* __STDC__ */

{
   PtrDocument         pDoc, pRefDoc;
   PtrReference        ref;
   PtrAttribute         pAttr;
   int                 saveNbCar;
   boolean             ok;

   UserErrorCode = 0;
   if (attribute == NULL || element == NULL)
     {
	TtaError (ERR_invalid_parameter);
     }
   else if (((PtrAttribute) attribute)->AeAttrType != AtReferenceAttr)
     {
	TtaError (ERR_invalid_attribute_type);
     }
   else
     {
	/* verifie le parametre document */
	if (document < 1 || document > MAX_DOCUMENTS)
	  {
	     TtaError (ERR_invalid_document_parameter);
	  }
	else if (TabDocuments[document - 1] == NULL)
	  {
	     TtaError (ERR_invalid_document_parameter);
	  }
	else
	   /* parametre document correct */
	  {
	     pDoc = TabDocuments[document - 1];
	     /* verifie que l'attribut appartient bien a l'element */
	     pAttr = ((PtrElement) element)->ElFirstAttr;
	     ok = FALSE;
	     while (pAttr != NULL)
		if (pAttr == (PtrAttribute) attribute)
		  {
		     ok = TRUE;
		     pAttr = NULL;
		  }
		else
		   pAttr = pAttr->AeNext;
	     if (!ok)
		TtaError (ERR_attribute_element_mismatch);
	     else if (target == NULL)
		/* annulation de la reference */
	       {
		  if (((PtrAttribute) attribute)->AeAttrReference != NULL)
		     DeleteReference (((PtrAttribute) attribute)->AeAttrReference);
		  ok = TRUE;
	       }
	     else
	       {
		  /* verifie le parametre targetDocument */
		  ok = FALSE;
		  if (targetDocument < 1 || targetDocument > MAX_DOCUMENTS)
		    {
		       TtaError (ERR_invalid_document_parameter);
		    }
		  else if (TabDocuments[targetDocument - 1] == NULL)
		    {
		       TtaError (ERR_invalid_document_parameter);
		    }
		  else
		     /* parametre targetDocument correct */
		    {
#ifndef NODISPLAY
		       UndisplayHeritAttr ((PtrElement) element,
					   (PtrAttribute) attribute,
					   document, FALSE);
#endif
		       pRefDoc = TabDocuments[targetDocument - 1];
		       /* l'element pointe' par target va etre reference' */
		       if (((PtrElement) target)->ElReferredDescr == NULL)
			  /* cet element n'a pas encore de descripteur */
			  /* d'element reference', on lui en associe un */
			 {
			    ((PtrElement) target)->ElReferredDescr = NewReferredElDescr (pRefDoc);
			    ((PtrElement) target)->ElReferredDescr->ReReferredElem =
			       (PtrElement) target;
			 }
		       if (((PtrAttribute) attribute)->AeAttrReference == NULL)
			 {
			    GetReference (&ref);
			    ((PtrAttribute) attribute)->AeAttrReference = ref;
			 }
		       else
			  ref = ((PtrAttribute) attribute)->AeAttrReference;
		       ref->RdElement = (PtrElement) element;
		       ref->RdAttribute = (PtrAttribute) attribute;
		       ref->RdTypeRef = RefFollow;
		       saveNbCar = TabDocuments[document - 1]->DocNTypedChars;
		       ok = SetReference (NULL, (PtrAttribute) attribute,
					  (PtrElement) target,
					  pDoc, pRefDoc, TRUE, FALSE);
		       /* an API function is not supposed to change */
		       /* the number of characters typed by the user */
		       TabDocuments[document - 1]->DocNTypedChars = saveNbCar;
		       if (!ok)
			  TtaError (ERR_cannot_set_link);
		    }
	       }
#ifndef NODISPLAY
	     if (ok)
		DisplayAttribute ((PtrElement) element,
				  (PtrAttribute) attribute, document);
#endif
	  }
     }
}


/* ----------------------------------------------------------------------
   TtaCopyAttributeReference

   Copies the reference attribute source into the reference attribute attribute.
   Both attributes must be attached to an element in an abstract tree.

   Parameters:
   attribute: the reference attribute to be set.
   element: the element to which attribute is attached.
   source : the source attribute.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TtaCopyAttributeReference (Attribute attribute, Element element, Attribute source)

#else  /* __STDC__ */
void                TtaCopyAttributeReference (attribute, element, source)
Attribute           attribute;
Element             element;
Attribute           source;

#endif /* __STDC__ */

{
   PtrElement          pEl;

   UserErrorCode = 0;
   if (attribute == NULL || source == NULL || element == NULL)
     {
	TtaError (ERR_invalid_parameter);
     }
   else if (((PtrAttribute) attribute)->AeAttrType != AtReferenceAttr ||
	    ((PtrAttribute) source)->AeAttrType != AtReferenceAttr)
     {
	TtaError (ERR_invalid_attribute_type);
     }
   else if (((PtrAttribute) source)->AeAttrReference != NULL)
     {
	if (((PtrAttribute) attribute)->AeAttrReference == NULL)
	   GetReference (&((PtrAttribute) attribute)->AeAttrReference);
	pEl = (PtrElement) element;
	CopyReference (((PtrAttribute) attribute)->AeAttrReference,
		  ((PtrAttribute) source)->AeAttrReference, &pEl);
     }
}


/* ----------------------------------------------------------------------
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

   ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TtaGiveReferredElement (Element element, Element * target, char *targetDocumentName, Document * targetDocument)

#else  /* __STDC__ */
void                TtaGiveReferredElement (element, target, targetDocumentName, targetDocument)
Element             element;
Element            *target;
char               *targetDocumentName;
Document           *targetDocument;

#endif /* __STDC__ */

{
   DocumentIdentifier     iDocExt;
   PtrDocument         pDocExt;

   UserErrorCode = 0;
   *target = NULL;
   targetDocumentName[0] = '\0';
   *targetDocument = 0;
   if (element == NULL)
      TtaError (ERR_invalid_parameter);
   else if (!((PtrElement) element)->ElTerminal ||
	    ((PtrElement) element)->ElLeafType != LtReference)
      /* ce n'est pas une reference */
      TtaError (ERR_invalid_element_type);
   else
     {
	if (((PtrElement) element)->ElReference != NULL)
	   *target = (Element) ReferredElement (((PtrElement) element)->ElReference,
					  &iDocExt, &pDocExt);
	if (!IdentDocNul (iDocExt))
	  {
	     strncpy (targetDocumentName, iDocExt, MAX_DOC_IDENT_LEN);
	     targetDocumentName[MAX_DOC_IDENT_LEN - 1] = '\0';
	  }
	if (pDocExt != NULL)
	   *targetDocument = IdentDocument (pDocExt);
     }
}


/* ----------------------------------------------------------------------
   TtaIsElementTypeReference

   Indicates whether an element type is a reference.

   Parameter:
   elementType: type to be tested.

   Return value:
   1 = the type is a reference, 0 = the type is not a reference.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
int                 TtaIsElementTypeReference (ElementType elementType)

#else  /* __STDC__ */
int                 TtaIsElementTypeReference (elementType)
ElementType         elementType;

#endif /* __STDC__ */

{
   int                 result;

   UserErrorCode = 0;
   result = 0;
   if (elementType.ElSSchema == NULL)
     {
	TtaError (ERR_invalid_parameter);
     }
   else if (elementType.ElTypeNum > ((PtrSSchema) (elementType.ElSSchema))->SsNRules ||
	    elementType.ElTypeNum < 1)
     {
	TtaError (ERR_invalid_element_type);
     }
   else
     {
	if (((PtrSSchema) (elementType.ElSSchema))->SsRule[elementType.ElTypeNum - 1].SrConstruct == CsReference)
	   result = 1;
     }
   return result;
}

/* ----------------------------------------------------------------------
   TtaSameReferences

   Compares two reference elements.

   Parameters:
   element1: first reference element.
   element2: second reference element.

   Return value:
   0 if both references are different, 1 if they are identical.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
int                 TtaSameReferences (Element element1, Element element2)

#else  /* __STDC__ */
int                 TtaSameReferences (element1, element2)
Element             element1;
Element             element2;

#endif /* __STDC__ */

{
   int                 result;
   PtrReferredDescr    pRef1, pRef2;

   UserErrorCode = 0;
   result = 0;
   if (element1 == NULL || element2 == NULL)
     {
	TtaError (ERR_invalid_parameter);
     }
   else if (!((PtrElement) element1)->ElTerminal ||
	    ((PtrElement) element1)->ElLeafType != LtReference ||
	    !((PtrElement) element2)->ElTerminal ||
	    ((PtrElement) element2)->ElLeafType != LtReference)
     {
	TtaError (ERR_invalid_element_type);
     }
   else
     {
	if (((PtrElement) element1)->ElReference != NULL &&
	    ((PtrElement) element2)->ElReference != NULL)
	   if (((PtrElement) element1)->ElReference->RdInternalRef ==
	       ((PtrElement) element2)->ElReference->RdInternalRef)
	     {
		pRef1 = ((PtrElement) element1)->ElReference->RdReferred;
		pRef2 = ((PtrElement) element2)->ElReference->RdReferred;
		if (pRef1 == NULL || pRef2 == NULL)
		  {
		     if (pRef1 == pRef2)
			result = 1;
		  }
		else if (!pRef1->ReExternalRef)
		   /* references internes */
		  {
		     if (pRef1->ReReferredElem != NULL && pRef2->ReReferredElem != NULL)
			if (strcmp (pRef1->ReReferredElem->ElLabel, pRef2->ReReferredElem->ElLabel) == 0)
			   result = 1;
		  }
		else
		   /* references externes */
		if (strcmp (pRef1->ReReferredLabel, pRef2->ReReferredLabel) == 0)
		   if (strcmp (pRef1->ReExtDocument, pRef2->ReExtDocument) == 0)
		      result = 1;
	     }
     }
   return result;
}

/* ----------------------------------------------------------------------
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

   ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TtaGiveReferenceAttributeValue (Attribute attribute, Element * target, char *targetDocumentName, Document * targetDocument)

#else  /* __STDC__ */
void                TtaGiveReferenceAttributeValue (attribute, target, targetDocumentName, targetDocument)
Attribute           attribute;
Element            *target;
char               *targetDocumentName;
Document           *targetDocument;

#endif /* __STDC__ */

{
   DocumentIdentifier     iDocExt;
   PtrDocument         pDocExt;

   UserErrorCode = 0;
   *target = NULL;
   targetDocumentName[0] = '\0';
   *targetDocument = 0;
   if (attribute == NULL)
      TtaError (ERR_invalid_parameter);
   else if (((PtrAttribute) attribute)->AeAttrType != AtReferenceAttr)
      TtaError (ERR_invalid_attribute_type);
   else
     {
	if (((PtrAttribute) attribute)->AeAttrReference != NULL)
	   *target = (Element) ReferredElement (((PtrAttribute) attribute)->AeAttrReference,
					  &iDocExt, &pDocExt);
	if (!IdentDocNul (iDocExt))
	  {
	     strncpy (targetDocumentName, iDocExt, MAX_DOC_IDENT_LEN);
	     targetDocumentName[MAX_DOC_IDENT_LEN - 1] = '\0';
	  }
	if (pDocExt != NULL)
	   *targetDocument = IdentDocument (pDocExt);
     }
}

/* ----------------------------------------------------------------------
   TtaIsElementReferred

   Tells whether a given element is the target of a reference or not.

   Parameter:
   element: the element.

   Return value:
   1 if the element is referred by another element or an
   attribute, 0 if not.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
int                 TtaIsElementReferred (Element element)

#else  /* __STDC__ */
int                 TtaIsElementReferred (element)
Element             element;

#endif /* __STDC__ */

{
   int                 result;

   UserErrorCode = 0;
   result = 0;
   if (element == NULL)
     {
	TtaError (ERR_invalid_parameter);
     }
   else
     {
	if (((PtrElement) element)->ElReferredDescr != NULL)
	   /* l'element a un descripteur d'element reference' */
	   if (((PtrElement) element)->ElReferredDescr->ReFirstReference != NULL ||
	       ((PtrElement) element)->ElReferredDescr->ReExtDocRef != NULL)
	      /* l'element est effectivement reference' */
	      result = 1;
     }
   return result;
}

/* ----------------------------------------------------------------------
   TtaSameReferenceAttributes

   Compares two reference attributes.

   Parameters:
   attribute1: first reference attribute.
   attribute2: second reference attribute.

   Return value:
   0 if both references are different, 1 if they are identical.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
int                 TtaSameReferenceAttributes (Attribute attribute1, Attribute attribute2)

#else  /* __STDC__ */
int                 TtaSameReferenceAttributes (attribute1, attribute2)
Attribute           attribute1;
Attribute           attribute2;

#endif /* __STDC__ */

{
   int                 result;
   PtrReferredDescr    pRef1, pRef2;

   UserErrorCode = 0;
   result = 0;
   if (attribute1 == NULL || attribute2 == NULL)
     {
	TtaError (ERR_invalid_parameter);
     }
   else if (((PtrAttribute) attribute1)->AeAttrType != AtReferenceAttr ||
	    ((PtrAttribute) attribute2)->AeAttrType != AtReferenceAttr)
     {
	TtaError (ERR_invalid_attribute_type);
     }
   else
     {
	if (((PtrAttribute) attribute1)->AeAttrReference != NULL &&
	    ((PtrAttribute) attribute2)->AeAttrReference != NULL)
	   if (((PtrAttribute) attribute1)->AeAttrReference->RdInternalRef ==
	       ((PtrAttribute) attribute2)->AeAttrReference->RdInternalRef)
	     {
		pRef1 = ((PtrAttribute) attribute1)->AeAttrReference->RdReferred;
		pRef2 = ((PtrAttribute) attribute2)->AeAttrReference->RdReferred;
		if (!pRef1->ReExternalRef)
		   /* references internes */
		  {
		     if (pRef1->ReReferredElem != NULL && pRef2->ReReferredElem != NULL)
			if (strcmp (pRef1->ReReferredElem->ElLabel, pRef1->ReReferredElem->ElLabel) == 0)
			   result = 1;
		  }
		else
		   /* references externes */
		if (strcmp (pRef1->ReReferredLabel, pRef2->ReReferredLabel) == 0)
		   if (strcmp (pRef1->ReExtDocument, pRef2->ReExtDocument) == 0)
		      result = 1;
	     }
     }
   return result;
}

/* ----------------------------------------------------------------------
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

   ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TtaNextLoadedReference (Element target, Document targetDocument, Element * referenceElement, Attribute * referenceAttribute, Document * referenceDocument)

#else  /* __STDC__ */
void                TtaNextLoadedReference (target, targetDocument, referenceElement, referenceAttribute, referenceDocument)
Element             target;
Document            targetDocument;
Element            *referenceElement;
Attribute          *referenceAttribute;
Document           *referenceDocument;

#endif /* __STDC__ */

{
   PtrReference        pRef;
   PtrExternalDoc       pDE;
   PtrDocument         pDocRef;
   boolean             trouve;

   UserErrorCode = 0;
   pRef = NULL;
   if (target == NULL)
      TtaError (ERR_invalid_parameter);
   else
      /* verifie le parametre targetDocument */
   if (targetDocument < 1 || targetDocument > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (TabDocuments[targetDocument - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parametre targetDocument correct */
     {
	if (*referenceElement == NULL && *referenceAttribute == NULL)
	   /* recherche de la premiere reference */
	  {
	     *referenceDocument = 0;
	     if (((PtrElement) target)->ElReferredDescr != NULL)
		/*  l'element a un descripteur d'element reference' */
		if (((PtrElement) target)->ElReferredDescr->ReFirstReference != NULL)
		   /* l'element est reference' en interne */
		  {
		     pRef = ((PtrElement) target)->ElReferredDescr->ReFirstReference;
		     *referenceDocument = targetDocument;
		  }
		else if (((PtrElement) target)->ElReferredDescr->ReExtDocRef != NULL)
		   /* l'element est reference' en externe */
		  {
		     pDE = NULL;
		     pRef = SearchExternalReferenceToElem ((PtrElement) target,
				    TabDocuments[targetDocument - 1], FALSE,
					 &pDocRef, &pDE, TRUE);
		     if (pRef != NULL)
			/*il y a une reference externe dans un document charge' */
			*referenceDocument = IdentDocument (pDocRef);
		  }
	  }
	else
	   /* on a deja cherche' d'autres references a cet element */
	   /* verifie le parametre referenceDocument */
	if (*referenceDocument < 1 || *referenceDocument > MAX_DOCUMENTS)
	   TtaError (ERR_invalid_document_parameter);
	else if (TabDocuments[*referenceDocument - 1] == NULL)
	   TtaError (ERR_invalid_document_parameter);
	else
	   /* parametre referenceDocument correct */
	  {
	     if (*referenceAttribute != NULL)
		/* la reference precedente doit etre un attribut reference */
		if (((PtrAttribute) (*referenceAttribute))->AeAttrType !=
		    AtReferenceAttr)
		   /* ce n'est pas un attribut reference, erreur */
		   TtaError (ERR_invalid_parameter);
		else
		   pRef = ((PtrAttribute) (*referenceAttribute))->AeAttrReference;
	     else
		/* la reference precedente doit etre un element reference */
	     if (!((PtrElement) (*referenceElement))->ElTerminal)
		/* ce n'est pas une feuille, erreur */
		TtaError (ERR_invalid_parameter);
	     else if (((PtrElement) (*referenceElement))->ElLeafType !=
		      LtReference)
		/* ce n'est pas une reference */
		TtaError (ERR_invalid_parameter);
	     else
		pRef = ((PtrElement) (*referenceElement))->ElReference;
	     if (pRef != NULL)
	       {
		  /* passe a la reference suivante */
		  pRef = pRef->RdNext;
		  if (pRef == NULL)
		     /* c'etait la derniere reference dans ce document, */
		     /* cherche dans un autre document la 1ere reference */
		     /* au meme element */
		    {
		       pDE = NULL;
		       if (((PtrElement) target)->ElReferredDescr != NULL)
			 {
			    pDE = ((PtrElement) target)->ElReferredDescr->ReExtDocRef;
			    /* si on etait sur les references internes,on prend */
			    /* le premier descripteur de document referencant */
			    if (targetDocument != *referenceDocument)
			      {
				 /* sinon, on cherche le descripteur de document */
				 /* referencant qui correspond a referenceDocument */
				 /* et on prend le suivant */
				 trouve = FALSE;
				 while (pDE != NULL && !trouve)
				   {
				      if (MemeIdentDoc (pDE->EdDocIdent,
							TabDocuments[*referenceDocument - 1]->DocIdent))
					 trouve = TRUE;
				      pDE = pDE->EdNext;
				   }
			      }
			 }
		       if (pDE != NULL)
			 {
			    /* on cherche la 1ere reference dans le document */
			    /* referencant retenu */
			    pRef = SearchExternalReferenceToElem ((PtrElement) target,
					   TabDocuments[targetDocument - 1],
					      FALSE, &pDocRef, &pDE, FALSE);
			    if (pRef != NULL)
			       /* on a trouve' une reference externe dans un */
			       /* document charge' */
			       *referenceDocument = IdentDocument (pDocRef);
			 }
		    }
	       }
	  }
     }
   if (pRef == NULL)
      /* on n'a pas trouve' de reference a l'element */
     {
	*referenceElement = NULL;
	*referenceAttribute = NULL;
	*referenceDocument = 0;
     }
   else
      /* on a trouve' une reference a l'element */
     {
	*referenceElement = (Element) (pRef->RdElement);
	*referenceAttribute = (Attribute) (pRef->RdAttribute);
	/* si la reference trouve'e est dans le tampon copier-coller, on */
	/* cherche la suivante */
	if (DansTampon ((PtrElement) * referenceElement))
	   TtaNextLoadedReference (target, targetDocument, referenceElement,
				   referenceAttribute, referenceDocument);
     }
}


/* ----------------------------------------------------------------------
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

   ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TtaNextUnloadedReferringDocument (Element target, Document targetDocument, char *referringDocumentName)

#else  /* __STDC__ */
void                TtaNextUnloadedReferringDocument (target, targetDocument, referringDocumentName)
Element             target;
Document            targetDocument;
char               *referringDocumentName;

#endif /* __STDC__ */

{
   PtrExternalDoc       pDE;
   boolean             trouve;
   boolean             charge;

   UserErrorCode = 0;
   if (target == NULL)
      TtaError (ERR_invalid_parameter);
   else
      /* verifie le parametre targetDocument */
   if (targetDocument < 1 || targetDocument > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (TabDocuments[targetDocument - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parametre targetDocument correct */
   if (((PtrElement) target)->ElReferredDescr == NULL)
      /* element non reference' */
      referringDocumentName[0] = '\0';
   else
     {
	pDE = ((PtrElement) target)->ElReferredDescr->ReExtDocRef;
	if (referringDocumentName[0] == '\0')
	   /* premier appel, on traite le premier document referencant */
	   trouve = TRUE;
	else
	  {
	     /* on cherche le descripteur de document referencant qui */
	     /* correspond a referringDocumentName et on prend le suivant */
	     trouve = FALSE;
	     while (pDE != NULL && !trouve)
	       {
		  if (strcmp (pDE->EdDocIdent, referringDocumentName) == 0)
		     trouve = TRUE;
		  /* passe au document suivant */
		  pDE = pDE->EdNext;
	       }
	  }
	if (trouve)
	   /* on a trouve' un document referencant, on saute les */
	   /* documents charge's */
	  {
	     charge = TRUE;
	     while (charge && pDE != NULL)
	       {
		  /* verifie si le document est charge' */
		  charge = (pDocument (pDE->EdDocIdent) != NULL);
		  if (charge)
		     /* document charge', on le saute */
		     pDE = pDE->EdNext;
	       }
	  }
	if (pDE == NULL)
	   referringDocumentName[0] = '\0';
	else
	   strcpy (referringDocumentName, pDE->EdDocIdent);
     }
}


/* ----------------------------------------------------------------------
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

   ---------------------------------------------------------------------- */

#ifdef __STDC__
Element             TtaSearchReferenceElement (SearchDomain scope, Element element)

#else  /* __STDC__ */
Element             TtaSearchReferenceElement (scope, element)
SearchDomain        scope;
Element             element;

#endif /* __STDC__ */

{
   PtrElement          pEl;
   PtrElement          elementFound;

   UserErrorCode = 0;
   elementFound = NULL;
   if (element == NULL)
     {
	TtaError (ERR_invalid_parameter);
     }
   else
     {
	if (scope == SearchBackward)
	   pEl = BackSearchRefOrEmptyElem (((PtrElement) element), 2);
	else
	   pEl = FwdSearchRefOrEmptyElem (((PtrElement) element), 2);
	if (pEl != NULL)
	   if (scope == SearchInTree)
	     {
		if (!ElemIsWithinSubtree (pEl, ((PtrElement) element)))
		   pEl = NULL;
	     }
	if (pEl != NULL)
	   elementFound = pEl;
     }
   return ((Element) elementFound);
}

/* fin du module */
