/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *
 * Author: V. Quint (INRIA)
 *
 */
#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "application.h"
#include "document.h"
#include "tree.h"
#include "attribute.h"
#include "reference.h"
#include "typecorr.h"
#include "fileaccess.h"
#include "message.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern /* to avoid redefinition */
#include "edit_tv.h"

#include "applicationapi_f.h"
#include "displayview_f.h"
#include "memory_f.h"
#include "readpivot_f.h"
#include "references_f.h"
#include "schemas_f.h"
#include "search_f.h"
#include "structmodif_f.h"
#include "structschema_f.h"
#include "thotmsg_f.h"
#include "tree_f.h"
#include "viewapi_f.h"


/*----------------------------------------------------------------------
  TtaSetReference

  Changes (or sets) the target of a reference element. The reference element
  must be part of an abstract tree.
  Parameters:
  element: the reference element to be set.
  document: the document containing the reference element.
  target : the target element (NULL for resetting the reference).
  ----------------------------------------------------------------------*/
void TtaSetReference (Element element, Document document, Element target)
{
  PtrDocument         pRefDoc;
  PtrReference       *pRef;
  ReferenceType       refType;
  ThotBool            ok;
  
  pRef = NULL;
  refType = RefFollow;
  ok = FALSE;
  UserErrorCode = 0;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else if (((PtrElement) element)->ElIsCopy)
    {
      ok = TRUE;
      refType = RefInclusion;
      pRef = &((PtrElement) element)->ElSource;
    } 
  else if (((PtrElement) element)->ElTerminal &&
           ((PtrElement) element)->ElLeafType == LtReference)
    {
      ok = TRUE;
      refType = RefFollow;
      pRef = &((PtrElement) element)->ElReference;
    } 
  else
    TtaError (ERR_invalid_element_type);
    
  if (ok)
    {
      /* checks the parameter document */
      if (document < 1 || document > MAX_DOCUMENTS)
        TtaError (ERR_invalid_document_parameter);
      else if (LoadedDocument[document - 1] == NULL)
        TtaError (ERR_invalid_document_parameter);
      else
        /* parameter document is ok */
        {
          ok = FALSE;
          if (target == NULL)
            /* Abort the reference */
            {
              if (*pRef != NULL)
                DeleteReference (*pRef);
              ok = TRUE;
            }
          else
            /* parameter targetDocument is correct */
            {
              pRefDoc = LoadedDocument[document - 1];
              /* The element target will be referenced */
              if (((PtrElement) target)->ElReferredDescr == NULL)
                /* This element has not yet a referenced element descriptor.
                   A descriptor is given to this element */
                {
                  ((PtrElement) target)->ElReferredDescr = NewReferredElDescr (pRefDoc);
                  ((PtrElement) target)->ElReferredDescr->ReReferredElem =
                    (PtrElement) target;
                }
              if (*pRef == NULL)
                GetReference (pRef);
              (*pRef)->RdElement = (PtrElement) element;
              (*pRef)->RdTypeRef = refType;
              /* saveNbCar = LoadedDocument[document - 1]->DocNTypedChars; */
              ok = SetReference ((PtrElement) element, NULL,
                                 (PtrElement) target,
                                 LoadedDocument[document - 1], pRefDoc,
                                 TRUE, FALSE);
              if (ok && refType == RefInclusion)
                CopyIncludedElem ((PtrElement)element, LoadedDocument[document - 1]);
              /* an API function is not supposed to change */
              /* the number of characters typed by the user */
              /* LoadedDocument[document - 1]->DocNTypedChars = saveNbCar; */
              if (!ok)
                TtaError (ERR_cannot_set_link);
            }
#ifndef NODISPLAY
          if (ok)
            RedisplayReference ((PtrElement) element, document);
#endif
        }
    }
}


/*----------------------------------------------------------------------
  TtaNewInclusion

  Creates an inclusion of a given element.
  Parameters:
  document: the document for which the inclusion is created.
  target: the element to be included.
  Return value:
  the created inclusion.
  ----------------------------------------------------------------------*/
Element TtaNewInclusion (Document document, Element target)
{
  PtrElement          inclusion;
  PtrDocument         pRefDoc;

  UserErrorCode = 0;
  inclusion = NULL;
  /* checks the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  /* parameter document is ok */
  else if (target == NULL)
    TtaError (ERR_invalid_parameter);
  else
    /* parameter targetDocument is ok */
    {
      inclusion = NewSubtree (((PtrElement) target)->ElTypeNumber,
                              ((PtrElement) target)->ElStructSchema,
                              LoadedDocument[document - 1], FALSE,
                              TRUE, TRUE, TRUE);
      GetReference (&inclusion->ElSource);
      inclusion->ElSource->RdElement = inclusion;
      inclusion->ElSource->RdTypeRef = RefInclusion;
      pRefDoc = LoadedDocument[document - 1];
      /* The element target will be referenced */
      if (((PtrElement) target)->ElReferredDescr == NULL)
        /* This element has not yet a referenced element descriptor.
           A descriptor is given to this element */
        {
          ((PtrElement) target)->ElReferredDescr = NewReferredElDescr (pRefDoc);
          ((PtrElement) target)->ElReferredDescr->ReReferredElem = (PtrElement) target;
        }
      /* saveNbCar = LoadedDocument[document - 1]->DocNTypedChars; */
      if (SetReference (inclusion, NULL, (PtrElement) target,
                        LoadedDocument[document - 1], pRefDoc, TRUE, FALSE))
        CopyIncludedElem (inclusion, LoadedDocument[document - 1]);
      else
        TtaError (ERR_cannot_set_link);
      /* an API function is not supposed to change */
      /* the number of characters typed by the user */
      /* LoadedDocument[document - 1]->DocNTypedChars = saveNbCar; */
    }
  return ((Element) inclusion);
}


/*----------------------------------------------------------------------
  TtaCopyReference

  Copies a reference element into another reference element.
  Both reference elements must be in an abstract tree.
  Parameters:
  element: the reference element to be set.
  source : the element to be copied.
  ----------------------------------------------------------------------*/
void TtaCopyReference (Element element, Element source, Document document)
{

  UserErrorCode = 0;
  if (element == NULL || source == NULL)
    TtaError (ERR_invalid_parameter);
  else if (!((PtrElement) element)->ElTerminal ||
           ((PtrElement) element)->ElLeafType != LtReference)
    TtaError (ERR_invalid_element_type);
  else if (!((PtrElement) source)->ElTerminal ||
           ((PtrElement) source)->ElLeafType != LtReference)
    TtaError (ERR_invalid_element_type);
  else
    {
      CancelReference ((PtrElement) element, LoadedDocument[document -1]);
      if (((PtrElement) source)->ElReference != NULL)
        {
          if (((PtrElement) element)->ElReference == NULL)
            GetReference (&((PtrElement) element)->ElReference);
          CopyReference (((PtrElement) element)->ElReference,
                         ((PtrElement) source)->ElReference, (PtrElement *) & element);
        }
    }
}


/*----------------------------------------------------------------------
  TtaSetAttributeReference

  Changes the value of an attribute of type reference
  Parameters:
  attribute: the attribute to be changed.
  element: the element with which the attribute is associated.
  document: the document containing the attribute.
  target: the target element (NULL for resetting the reference).
  ----------------------------------------------------------------------*/
void TtaSetAttributeReference (Attribute attribute, Element element,
                               Document document, Element target)
{
  PtrDocument         pDoc, pRefDoc;
  PtrReference        ref;
  PtrAttribute        pAttr;
  /* int              saveNbCar; */
  ThotBool            ok;

  UserErrorCode = 0;
  if (attribute == NULL || element == NULL)
    TtaError (ERR_invalid_parameter);
  else if (((PtrAttribute) attribute)->AeAttrType != AtReferenceAttr)
    TtaError (ERR_invalid_attribute_type);
  else
    {
      /* checks the parameter document */
      if (document < 1 || document > MAX_DOCUMENTS)
        TtaError (ERR_invalid_document_parameter);
      else if (LoadedDocument[document - 1] == NULL)
        TtaError (ERR_invalid_document_parameter);
      else
        /* parameter document is ok */
        {
          pDoc = LoadedDocument[document - 1];
          /* verifies that the attribut belongs to the element */
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
            /* The reference is aborted */
            {
              if (((PtrAttribute) attribute)->AeAttrReference != NULL)
                DeleteReference (((PtrAttribute) attribute)->AeAttrReference);
              ok = TRUE;
            }
          else
            {
              ok = FALSE;
#ifndef NODISPLAY
              UndisplayInheritedAttributes ((PtrElement) element,
                                            (PtrAttribute) attribute,
                                            document, FALSE);
#endif
              pRefDoc = LoadedDocument[document - 1];
              /* The element target will be referenced */
              if (((PtrElement) target)->ElReferredDescr == NULL)
                /* This element has not yet a referenced element descriptor.
                   A descriptor is given to this element */
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
              /* saveNbCar = LoadedDocument[document - 1]->DocNTypedChars; */
              ok = SetReference (NULL, (PtrAttribute) attribute,
                                 (PtrElement) target,
                                 pDoc, pRefDoc, TRUE, FALSE);
              /* an API function is not supposed to change */
              /* the number of characters typed by the user */
              /* LoadedDocument[document - 1]->DocNTypedChars = saveNbCar; */
              if (!ok)
                TtaError (ERR_cannot_set_link);
            }
#ifndef NODISPLAY
          if (ok)
            DisplayAttribute ((PtrElement) element,
                              (PtrAttribute) attribute, document);
#endif
        }
    }
}


/*----------------------------------------------------------------------
  TtaCopyAttributeReference

  Copies the reference attribute source into the reference attribute attribute.
  Both attributes must be attached to an element in an abstract tree.
  Parameters:
  attribute: the reference attribute to be set.
  element: the element to which attribute is attached.
  source : the source attribute.
  ----------------------------------------------------------------------*/
void TtaCopyAttributeReference (Attribute attribute, Element element,
                                Attribute source)
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
   TtaCopyIncludedElem

   Up to date the value of inclusion element with the value of its source.
   Parameters:
   element: the element to be up to dated.
   document: the document that contains the element.
   ---------------------------------------------------------------------- */
void TtaCopyIncludedElem (Element element, Document document)
{
  UserErrorCode = 0;

  /* Checks the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    {
      TtaError (ERR_invalid_document_parameter);
      return;
    }
  if (LoadedDocument[document - 1] == NULL)
    {
      TtaError (ERR_invalid_document_parameter);
      return;
    }

  if (element == NULL)
    {
      TtaError (ERR_invalid_parameter);
      return;
    }

  CopyIncludedElem ((PtrElement)element, LoadedDocument[document - 1]);
}


/* ----------------------------------------------------------------------
   TtaUpdateInclusionElements

   Up to date the value of inclusions that belong to the document.
   If loadExternalDoc is TRUE, the inclusions whose the sources belong to 
   another document, are up to date too. In this case, the others documents
   are opened temporarely. If removeExclusions is TRUE, the exclusions
   are removed from the documents opened temporarely.
   Parameters:
   document: the document in question.
   ---------------------------------------------------------------------- */
void TtaUpdateInclusionElements (Document document)
{
  PtrDocument           pDoc;

  UserErrorCode = 0;
  /* Checks the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    {
      TtaError (ERR_invalid_document_parameter);
      return;
    }
  if (LoadedDocument[document - 1] == NULL)
    {
      TtaError (ERR_invalid_document_parameter);
      return;
    }

  pDoc = LoadedDocument[document - 1];
  UpdateInclusionElements (pDoc);
}


/*----------------------------------------------------------------------
  TtaGiveReferredElement

  Returns the element referred by a given reference element.
  Parameter:
  element: the reference element.
  Return parameters:
  target: the referred element, or NULL if that element is not
  accessible (empty reference or referred document not open).
  ----------------------------------------------------------------------*/
void TtaGiveReferredElement (Element element, Element *target)
{
  UserErrorCode = 0;
  *target = NULL;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else if (!((PtrElement) element)->ElTerminal ||
           ( !((PtrElement) element)->ElIsCopy &&
             ((PtrElement) element)->ElLeafType != LtReference ))
    /* It is not a reference */
    TtaError (ERR_invalid_element_type);
  else
    {
      if (((PtrElement) element)->ElIsCopy)
        *target = (Element) ReferredElement (((PtrElement) element)->ElSource);
      else if (((PtrElement) element)->ElReference != NULL)
        *target = (Element) ReferredElement (((PtrElement) element)->ElReference);
    }
}


/*----------------------------------------------------------------------
  TtaIsElementTypeReference

  Indicates whether an element type is a reference.
  Parameter:
  elementType: type to be tested.
  Return value:
  1 = the type is a reference, 0 = the type is not a reference.
  ----------------------------------------------------------------------*/
int TtaIsElementTypeReference (ElementType elementType)
{
  int                 result;

  UserErrorCode = 0;
  result = 0;
  if (elementType.ElSSchema == NULL)
    TtaError (ERR_invalid_parameter);
  else if (elementType.ElTypeNum > ((PtrSSchema) (elementType.ElSSchema))->SsNRules ||
           elementType.ElTypeNum < 1)
    TtaError (ERR_invalid_element_type);
  else
    {
      if (((PtrSSchema) (elementType.ElSSchema))->SsRule->SrElem[elementType.ElTypeNum - 1]->SrConstruct == CsReference)
        result = 1;
    }
  return result;
}

/*----------------------------------------------------------------------
  TtaSameReferences

  Compares two reference elements.
  Parameters:
  element1: first reference element.
  element2: second reference element.
  Return value:
  0 if both references are different, 1 if they are identical.
  ----------------------------------------------------------------------*/
int TtaSameReferences (Element element1, Element element2)
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
        {
          pRef1 = ((PtrElement) element1)->ElReference->RdReferred;
          pRef2 = ((PtrElement) element2)->ElReference->RdReferred;
          if (pRef1 == NULL || pRef2 == NULL)
            {
              if (pRef1 == pRef2)
                result = 1;
            }
          else
            /* Internal references */
            {
              if (pRef1->ReReferredElem != NULL && pRef2->ReReferredElem != NULL)
                if (strcmp (pRef1->ReReferredElem->ElLabel, pRef2->ReReferredElem->ElLabel) == 0)
                  result = 1;
            }
        }
    }
  return result;
}

/*----------------------------------------------------------------------
  TtaGiveReferenceAttributeValue

  Returns the value of a given attribute of type reference
  Parameter:
  attribute: the attribute of interest.
  Return parameters:
  target: the element referred by the attribute, or NULL
  if that element is not accessible (empty reference or referred
  document not loaded)
  ----------------------------------------------------------------------*/
void TtaGiveReferenceAttributeValue (Attribute attribute, Element *target)
{
  UserErrorCode = 0;
  *target = NULL;
  if (attribute == NULL)
    TtaError (ERR_invalid_parameter);
  else if (((PtrAttribute) attribute)->AeAttrType != AtReferenceAttr)
    TtaError (ERR_invalid_attribute_type);
  else if (((PtrAttribute) attribute)->AeAttrReference != NULL)
    *target = (Element) ReferredElement (((PtrAttribute) attribute)->AeAttrReference);
}

/*----------------------------------------------------------------------
  TtaIsElementReferred

  Tells whether a given element is the target of a reference or not.
  Parameter:
  element: the element.
  Return value:
  1 if the element is referred by another element or an
  attribute, 0 if not.
  ----------------------------------------------------------------------*/
int TtaIsElementReferred (Element element)
{
  int                 result;

  UserErrorCode = 0;
  result = 0;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else if (((PtrElement) element)->ElReferredDescr != NULL)
    /* The element has a referenced element descriptor */
    if (((PtrElement) element)->ElReferredDescr->ReFirstReference)
      /* The element is referenced */
      result = 1;
  return result;
}

/*----------------------------------------------------------------------
  TtaGetFirstReferringAttribute

  Return the first element that refers to the given element through
  an attribute of type attrType.
  Parameter:
  element: the element of interest
  attrType: type of the reference attribute considered
  Return value:
  the referring element if it exists, NULL if element is not referred
  by this type of attribute.
  ----------------------------------------------------------------------*/
Element TtaGetFirstReferringAttribute (Element element, AttributeType attrType)
{
  PtrElement      referringEl;
  PtrReference    ref;

  UserErrorCode = 0;
  referringEl = NULL;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else if (attrType.AttrSSchema == NULL)
    TtaError (ERR_invalid_attribute_type);
  else if (attrType.AttrTypeNum < 1 ||
           attrType.AttrTypeNum > ((PtrSSchema)(attrType.AttrSSchema))->SsNAttributes)
    TtaError (ERR_invalid_attribute_type);
  else
    /* parameters are OK */
    {
      if (((PtrElement) element)->ElReferredDescr)
        /* The element has a referenced element descriptor */
        {
          ref = ((PtrElement) element)->ElReferredDescr->ReFirstReference;
          /* check all references to this element */
          while (ref && !referringEl)
            {
              if (ref->RdAttribute)
                /* this is a reference from an attribute. Check the attribute*/
                {
                  if (ref->RdAttribute->AeAttrSSchema == (PtrSSchema)(attrType.AttrSSchema) &&
                      ref->RdAttribute->AeAttrNum == attrType.AttrTypeNum)
                    /* that's the attribute we are looking for */
                    referringEl = ref->RdElement;
                }
              if (!referringEl)
                ref = ref->RdNext;
            }
        }
    }
  return ((Element) referringEl);
}

/*----------------------------------------------------------------------
  TtaSameReferenceAttributes

  Compares two reference attributes.
  Parameters:
  attribute1: first reference attribute.
  attribute2: second reference attribute.
  Return value:
  0 if both references are different, 1 if they are identical.
  ----------------------------------------------------------------------*/
int TtaSameReferenceAttributes (Attribute attribute1, Attribute attribute2)
{
  int                 result;
  PtrReferredDescr    pRef1, pRef2;

  UserErrorCode = 0;
  result = 0;
  if (attribute1 == NULL || attribute2 == NULL)
    TtaError (ERR_invalid_parameter);
  else if (((PtrAttribute) attribute1)->AeAttrType != AtReferenceAttr ||
           ((PtrAttribute) attribute2)->AeAttrType != AtReferenceAttr)
    TtaError (ERR_invalid_attribute_type);
  else
    {
      if (((PtrAttribute) attribute1)->AeAttrReference != NULL &&
          ((PtrAttribute) attribute2)->AeAttrReference != NULL)
        {
          pRef1 = ((PtrAttribute) attribute1)->AeAttrReference->RdReferred;
          pRef2 = ((PtrAttribute) attribute2)->AeAttrReference->RdReferred;
          if (pRef1->ReReferredElem != NULL && pRef2->ReReferredElem != NULL)
            if (strcmp (pRef1->ReReferredElem->ElLabel, pRef1->ReReferredElem->ElLabel) == 0)
              result = 1;
        }
    }
  return result;
}

/*----------------------------------------------------------------------
  TtaNextLoadedReference

  Searches a reference that has a given element as a target and that
  belongs to a document currently loaded.
  Parameters:
  target: the target element.
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
  ----------------------------------------------------------------------*/
void TtaNextLoadedReference (Element target, Element *referenceElement,
                             Attribute *referenceAttribute)
{
  PtrReference        pRef;

  UserErrorCode = 0;
  pRef = NULL;
  if (target == NULL)
    TtaError (ERR_invalid_parameter);
  else
    /* parameter targetDocument is ok */
    {
      if (*referenceElement == NULL && *referenceAttribute == NULL)
        /* Looks for the first reference */
        {
          if (((PtrElement) target)->ElReferredDescr)
            {
              /*  The element has a referenced element descriptor */
              if (((PtrElement) target)->ElReferredDescr->ReFirstReference)
                /* The element has an internal reference */
                pRef = ((PtrElement) target)->ElReferredDescr->ReFirstReference;
            }
        }
      else
        /* parameter referenceDocument is correct */
        {
          if (*referenceAttribute != NULL)
            /* The previous reference must be a reference attribut */
            if (((PtrAttribute) (*referenceAttribute))->AeAttrType != AtReferenceAttr)
              /* Error: it is not a reference attribut */
              TtaError (ERR_invalid_parameter);
            else
              pRef = ((PtrAttribute) (*referenceAttribute))->AeAttrReference;
          else if (((PtrElement) (*referenceElement))->ElIsCopy)
            /* The previous reference must be a reference element */
            /* or an inclusion */
            pRef = ((PtrElement) (*referenceElement))->ElSource;
          else if (!((PtrElement) (*referenceElement))->ElTerminal)
            /* Error: not a leaf */
            TtaError (ERR_invalid_parameter);
          else if (((PtrElement) (*referenceElement))->ElLeafType != LtReference)
            /* Not a reference */
            TtaError (ERR_invalid_parameter);
          else
            pRef = ((PtrElement) (*referenceElement))->ElReference;
          if (pRef)
            /* Go to the next reference */
            pRef = pRef->RdNext;
        }
    }
  if (pRef == NULL)
    /* There is no a reference to this element */
    {
      *referenceElement = NULL;
      *referenceAttribute = NULL;
    }
  else
    /* There is a reference to the element */
    {
      *referenceElement = (Element) (pRef->RdElement);
      *referenceAttribute = (Attribute) (pRef->RdAttribute);
      /* If the reference is into the Copy/paste buffer, one search the following one */
      if (IsASavedElement ((PtrElement) * referenceElement))
        TtaNextLoadedReference (target, referenceElement, referenceAttribute);
    }
}


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
Element TtaSearchReferenceElement (SearchDomain scope, Element element)
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

/* End of module */
