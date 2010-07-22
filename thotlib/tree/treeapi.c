/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2010
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
#include "view.h"
#include "fileaccess.h"
#include "typecorr.h"
#include "appdialogue.h"
#include "labelAllocator.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "edit_tv.h"
#include "modif_tv.h"
#include "platform_tv.h"
#include "appdialogue_tv.h"

#include "applicationapi_f.h"
#include "attributes_f.h"
#include "attributeapi_f.h"
#include "changeabsbox_f.h"
#include "createabsbox_f.h"
#include "displayview_f.h"
#include "documentapi_f.h"
#include "exceptions_f.h"
#include "externalref_f.h"
#include "labelalloc_f.h"
#include "memory_f.h"
#include "references_f.h"
#include "schemas_f.h"
#include "structcommands_f.h"
#include "structcreation_f.h"
#include "structmodif_f.h"
#include "structschema_f.h"
#include "structselect_f.h"
#include "translation_f.h"
#include "thotmsg_f.h"
#include "tree_f.h"
#include "viewapi_f.h"

#define ELEM_NAME_LENGTH 100
static char        nameBuffer[ELEM_NAME_LENGTH];

/* ----------------------------------------------------------------------
   TtaChangeElementType
   Change the type of a given element.
   CAUTION: THIS FUNCTION SHOULD BE USED VERY CARFULLY!
   Parameters:
   element: the concerned element
   typeNum: new type for the element
   ---------------------------------------------------------------------- */
void TtaChangeElementType (Element element, int typeNum)
{
  UserErrorCode = 0;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else if (typeNum < 1 ||
           typeNum > ((PtrElement)element)->ElStructSchema->SsNRules)
    TtaError (ERR_invalid_element_type);
  else
    {
      ((PtrElement)element)->ElTypeNumber = typeNum;
      if (typeNum < AnyType)
        ((PtrElement)element)->ElTerminal = TRUE;
    }
}

/* ----------------------------------------------------------------------
   TtaUpdateRootElementType
   Change the document and the root element schema if different to the
   requested schema.
   CAUTION: THIS FUNCTION SHOULD BE USED VERY CARFULLY!
   Parameters:
   root:   the concerned element must be the root of the document
   schemaName: the requested schema name.
   ---------------------------------------------------------------------- */
void TtaUpdateRootElementType (Element root, const char *schemaName, Document doc)
{
  PtrDocument pDoc;
  PtrSSchema  pSS, pSNew;
  PtrElement  pEl = (PtrElement)root;

  UserErrorCode = 0;
  if (root == NULL)
    TtaError (ERR_invalid_parameter);
  else if (pEl->ElParent || pEl->ElTerminal)
    TtaError (ERR_invalid_parameter);
  else if (LoadedDocument[doc - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    {
      pDoc = LoadedDocument[doc - 1];
      pSS = pEl->ElStructSchema;
      if (pSS == NULL || pSS->SsName == NULL ||
          strcmp (pSS->SsName, schemaName))
        {
          pSNew = LoadStructureSchema (NULL, schemaName, pDoc);
          pEl->ElStructSchema = pSNew;
          pDoc->DocSSchema = pSNew;
          //ReleaseStructureSchema (pSS, pDoc);
        }
    }
}

/*----------------------------------------------------------------------
  TtaChangeTypeOfElement
  Change the type of element elem into newTypeNum
  Parameters:
  element: the changed element
  document: the document for which the element is changed.
  typeNum: new type for the element
  Return value:
  TRUE if the operation is done.
  -----------------------------------------------------------------------*/
ThotBool TtaChangeTypeOfElement (Element elem, Document doc, int newTypeNum)
{
  Element    prev, next, parent;

  UserErrorCode = 0;
  if (elem == NULL)
    TtaError (ERR_invalid_parameter);
  else if (newTypeNum < 1 ||
           newTypeNum > ((PtrElement) elem)->ElStructSchema->SsNRules)
    TtaError (ERR_invalid_element_type);
  else
    {
      parent = NULL;
      prev = elem;
      TtaPreviousSibling (&prev);
      if (prev == NULL)
        {
          next = elem;
          TtaNextSibling (&next);
          if (next == NULL)
            parent = TtaGetParent (elem);
        } 
      TtaRemoveTree (elem, doc);
      TtaChangeElementType (elem, newTypeNum);
      if (prev != NULL)
        TtaInsertSibling (elem, prev, FALSE, doc);
      else if (next != NULL)
        TtaInsertSibling (elem, next, TRUE, doc);
      else
        TtaInsertFirstChild (&elem, parent, doc);
#ifndef NODISPLAY
#ifndef _WX
      /* redisplay the selection message */
      BuildSelectionMessage ();
#endif /* _WX */
#endif
      SetDocumentModified (LoadedDocument[doc - 1], TRUE, 0);
      return TRUE;
    }
  return FALSE;
}

/* ----------------------------------------------------------------------
   TtaNewElement
   Creates a new element of a given type.
   Parameters:
   document: the document for which the element is created.
   elementType: type of the element to be created.
   Return value:
   the created element.
   ---------------------------------------------------------------------- */
Element TtaNewElement (Document document, ElementType elementType)
{
  PtrElement          element;

  UserErrorCode = 0;
  element = NULL;
  if (elementType.ElSSchema == NULL)
    TtaError (ERR_invalid_parameter);
  else if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else if (elementType.ElTypeNum < 1 ||
           elementType.ElTypeNum > ((PtrSSchema) (elementType.ElSSchema))->SsNRules)
    TtaError (ERR_invalid_element_type);
  else
    {
      element = NewSubtree (elementType.ElTypeNum,
                            (PtrSSchema) (elementType.ElSSchema),
                            LoadedDocument[document - 1], FALSE, TRUE,
                            TRUE, TRUE);
      if (element &&
          element->ElStructSchema->SsRule->SrElem[element->ElTypeNumber - 1]->SrConstruct == CsPairedElement &&
          !element->ElStructSchema->SsRule->SrElem[element->ElTypeNumber - 1]->SrFirstOfPair)
        element->ElPairIdent = 0;
    }
  return ((Element) element);
}

/* ----------------------------------------------------------------------
   TtaNewTree
   Creates a new element of a given type and all its descendants, according
   to the structure schema.
   Parameters:
   document: the document for which the tree is created.
   elementType: type of the root element of the tree to be created.
   label: label of the root element to be created. Empty string if the value
   of the label is undefined.
   Return value:
   the root element of the created tree.
   ---------------------------------------------------------------------- */
Element TtaNewTree (Document document, ElementType elementType, const char* label)
{
  PtrElement          element;
  
  UserErrorCode = 0;
  element = NULL;
  if (elementType.ElSSchema == NULL)
    TtaError (ERR_invalid_parameter);
  else if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else if (elementType.ElTypeNum < 1 ||
           elementType.ElTypeNum > ((PtrSSchema) (elementType.ElSSchema))->SsNRules)
    TtaError (ERR_invalid_element_type);
  else
    {
      element = NewSubtree (elementType.ElTypeNum, (PtrSSchema) (elementType.ElSSchema),
                            LoadedDocument[document - 1], TRUE, TRUE, TRUE,
                            (ThotBool)(label == NULL || *label == EOS));
      if (element->ElStructSchema->SsRule->SrElem[element->ElTypeNumber - 1]->SrConstruct == CsPairedElement)
        if (!element->ElStructSchema->SsRule->SrElem[element->ElTypeNumber - 1]->SrFirstOfPair)
          element->ElPairIdent = 0;
      if (*label != EOS)
        strncpy (element->ElLabel, label, MAX_LABEL_LEN);
    }
  return ((Element) element);
}

/* ----------------------------------------------------------------------
   TtaNewTranscludedElement

   Creates a new element that is a dynamic copy of another element.
   Parameters:
   document: the document for which the element is created.
   orig:     the element that is copied.
   Return value:
   the created element.
   ---------------------------------------------------------------------- */
Element TtaNewTranscludedElement (Document document, Element orig)
{
  PtrElement          element;
  PtrReference        pRef;

  UserErrorCode = 0;
  element = NULL;
  if (orig == NULL)
    TtaError (ERR_invalid_parameter);
  else if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    /* Parameter document is ok */
    {
      element = NewSubtree (((PtrElement)orig)->ElTypeNumber,
                            ((PtrElement)orig)->ElStructSchema,
                            LoadedDocument[document - 1], FALSE, TRUE, TRUE, TRUE);
      if (element != NULL)
        {
          GetReference (&pRef);
          if (pRef)
            {
              element->ElSource = pRef;
              pRef->RdElement = element;
              pRef->RdTypeRef = RefInclusion;
              if (SetReference (element, NULL, (PtrElement) orig,
                                LoadedDocument[document - 1],
                                LoadedDocument[document - 1], TRUE, FALSE))
                CopyIncludedElem (element, LoadedDocument[document - 1]);
              else
                {
                  element->ElSource = NULL;
                  pRef->RdElement = NULL;
                  FreeReference (pRef);
                }
            }
        }
    }
  return ((Element) element);
}

/* ------------------------------------------------------------
   TransRef cherche dans le sous-arbre de racine pElem tous les
   elements reference's et transfert sur eux les references qui    
   sont dans l'arbre de racine pRoot.                               
   affecte aussi un nouveau label a tous les elements du           
   sous-arbre de racine pElem.                                     
   ------------------------------------------------------------ */
static void TransRef (PtrElement pElem, PtrElement pRoot, PtrDocument pDoc)
{
  PtrElement          pSon;
  PtrReferredDescr    pDescRef;
  PtrReference        pRef, pNextRef;
  PtrReference        pPR1;

  if (!pElem || !pDoc || !pRoot)
    return;
  /* Sets a new label to the element */
  ConvertIntToLabel (NewLabel (pDoc), pElem->ElLabel);
  if (pElem->ElReferredDescr != NULL)
    /* This element is referenced. CopyTree did not copy its referenced
       element descriptor which is shared by the source element.
       Deals with source element references which are into the sub-tree
       whose root is pRoot */
    {
      pDescRef = pElem->ElReferredDescr;
      /* references element descriptor */
      pElem->ElReferredDescr = NULL;
      /* remains attached only to the source element */
      pRef = pDescRef->ReFirstReference;
      /* First reference to the source element */
      while (pRef != NULL)
        /* Examine the source element references */
        {
          pNextRef = pRef->RdNext;	/* prepare the next reference */
          if (ElemIsWithinSubtree (pRef->RdElement, pRoot))
            /* The reference is into the sub-tree which root is pRoot. 
               It will refer the treated element (pElem) */
            {
              if (pElem->ElReferredDescr == NULL)
                /* The element has not a referenced element descriptor */
                /* A descriptor is affected to it */
                {
                  pElem->ElReferredDescr = NewReferredElDescr (pDoc);
                  pElem->ElReferredDescr->ReReferredElem = pElem;
                }
              strncpy (pElem->ElReferredDescr->ReReferredLabel,
                       pElem->ElLabel, MAX_LABEL_LEN);
              /* bind the reference descriptor and the referenced element 
                 descriptor of the treated element */
              pPR1 = pRef;
              /* unchain the descriptor from chain of references to the source element */
              if (pPR1->RdNext != NULL)
                pPR1->RdNext->RdPrevious = pPR1->RdPrevious;
              if (pPR1->RdPrevious == NULL)
                pDescRef->ReFirstReference = pPR1->RdNext;
              else
                pPR1->RdPrevious->RdNext = pPR1->RdNext;
              /* The chain at the begenning of the references list of the treated element */
              pPR1->RdReferred = pElem->ElReferredDescr;
              pPR1->RdNext = pPR1->RdReferred->ReFirstReference;
              if (pPR1->RdNext != NULL)
                pPR1->RdNext->RdPrevious = pRef;
              pPR1->RdReferred->ReFirstReference = pRef;
              pPR1->RdPrevious = NULL;
            }
          pRef = pNextRef;
          /* Go to the next reference */
        }
    }
  if (!pElem->ElTerminal)
    /* non terminal element, we deal with all its sons */
    {
      pSon = pElem->ElFirstChild;
      while (pSon != NULL)
        {
          TransRef (pSon, pRoot, pDoc);
          pSon = pSon->ElNext;
        }
    }
}

/* ----------------------------------------------------------------------
   TtaCopyTree
   Creates a deep copy of a tree.
   Parameters:
   sourceElement: root of the tree to be copied.
   sourceDocument: the document containing the tree to be copied.
   destinationDocument: the document for which the copy must be created.
   parent: element that will become the parent of the created tree.
   Return value:
   the root element of the created tree.
   ---------------------------------------------------------------------- */
Element TtaCopyTree (Element sourceElement, Document sourceDocument,
                     Document destinationDocument, Element parent)
{
  PtrElement          element, ancestor;
  PtrSSchema          pSS, nextExtension;

  UserErrorCode = 0;
  element = NULL;
  /* verifies the parameters Document */
  if (sourceDocument < 1 || sourceDocument > MAX_DOCUMENTS ||
      destinationDocument < 1 || destinationDocument > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[sourceDocument - 1] == NULL ||
           LoadedDocument[destinationDocument - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else if (sourceElement == NULL)
    TtaError (ERR_invalid_parameter);
  else
    {
      /* Looks for the structure schema to use for the copy */
      if (sourceDocument == destinationDocument)
        pSS = ((PtrElement) sourceElement)->ElStructSchema;
      else
        {
          pSS = NULL;
          ancestor = (PtrElement) parent;
          while (pSS == NULL && ancestor != NULL)
            if (!strcmp (ancestor->ElStructSchema->SsName,
                         ((PtrElement) sourceElement)->ElStructSchema->SsName))
              pSS = ancestor->ElStructSchema;
            else
              ancestor = ancestor->ElParent;
          if (pSS == NULL)
            {
              if (((PtrElement) sourceElement)->ElStructSchema->SsExtension)
                {
                  nextExtension = LoadedDocument[destinationDocument - 1]->DocSSchema->SsNextExtens;
                  while (nextExtension != NULL)
                    {
                      if (!strcmp (nextExtension->SsName,
                                   ((PtrElement) sourceElement)->ElStructSchema->SsName))
                        break;
                      nextExtension = nextExtension->SsNextExtens;
                    }
                  if (nextExtension == NULL)
                    TtaError (ERR_invalid_parameter);
                  pSS = nextExtension;
                }
              else if (!strcmp (LoadedDocument[destinationDocument - 1]->DocSSchema->SsName,
                                ((PtrElement) sourceElement)->ElStructSchema->SsName))
                pSS = LoadedDocument[destinationDocument - 1]->DocSSchema;
              else if (((PtrElement) sourceElement)->ElTerminal)
                pSS = ((PtrElement) parent)->ElStructSchema;
              else
                pSS = ((PtrElement) sourceElement)->ElStructSchema;
            }
        }
      /* Copying */
      element = CopyTree (((PtrElement) sourceElement),
                          LoadedDocument[sourceDocument - 1], pSS,
                          LoadedDocument[destinationDocument - 1],
                          (PtrElement) parent,
                          TRUE, TRUE, FALSE, TRUE, TRUE);
      if (element)
	TransRef (element, element, LoadedDocument[destinationDocument - 1]);
    }
  return ((Element) element);
}

/* ----------------------------------------------------------------------
   TtaCopyElement
   Creates a copy of an element (does not copy the descendants).
   Parameters:
   sourceElement: element to be copied.
   sourceDocument: the document containing the element to be copied.
   destinationDocument: the document for which the copy must be created.
   parent: element that will become the parent of the created element.
   Return value:
   An element whic is a copy of the sourceElement.
   ---------------------------------------------------------------------- */
Element TtaCopyElement (Element sourceElement, Document sourceDocument,
                        Document destinationDocument, Element parent)
{
  PtrElement          element;
  PtrElement          ancestor;
  PtrSSchema          pSS, nextExtension;

  UserErrorCode = 0;
  element = NULL;
  /* check parameters */
  if (sourceDocument < 1 || sourceDocument > MAX_DOCUMENTS ||
      destinationDocument < 1 || destinationDocument > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[sourceDocument - 1] == NULL ||
           LoadedDocument[destinationDocument - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else if (sourceElement == NULL)
    TtaError (ERR_invalid_parameter);
  else
    {
      /* Looks for the structure schema to use for the copy */
      if (sourceDocument == destinationDocument)
        pSS = ((PtrElement) sourceElement)->ElStructSchema;
      else
        {
          pSS = NULL;
          ancestor = (PtrElement) parent;
          while (pSS == NULL && ancestor != NULL)
            if (!strcmp (ancestor->ElStructSchema->SsName,
                         ((PtrElement) sourceElement)->ElStructSchema->SsName))
              pSS = ancestor->ElStructSchema;
            else
              ancestor = ancestor->ElParent;
          if (pSS == NULL)
            {
              if (((PtrElement) sourceElement)->ElStructSchema->SsExtension)
                {
                  nextExtension = LoadedDocument[destinationDocument - 1]->DocSSchema->SsNextExtens;
                  while (nextExtension != NULL)
                    {
                      if (!strcmp (nextExtension->SsName,
                                   ((PtrElement) sourceElement)->ElStructSchema->SsName))
                        break;
                      nextExtension = nextExtension->SsNextExtens;
                    }
                  if (nextExtension == NULL)
                    TtaError (ERR_invalid_parameter);
                  pSS = nextExtension;
                }
              else if (!strcmp (LoadedDocument[destinationDocument - 1]->DocSSchema->SsName,
                                ((PtrElement) sourceElement)->ElStructSchema->SsName))
                pSS = LoadedDocument[destinationDocument - 1]->DocSSchema;
              else if (((PtrElement) sourceElement)->ElTerminal)
                pSS = ((PtrElement) parent)->ElStructSchema;
              else
                pSS = ((PtrElement) sourceElement)->ElStructSchema;
            }
        }
      /* Copying */
      element = CopyTree (((PtrElement) sourceElement),
                          LoadedDocument[sourceDocument - 1], pSS,
                          LoadedDocument[destinationDocument - 1],
                          (PtrElement) parent,
                          TRUE, TRUE, FALSE, FALSE, TRUE);
      if (element)
	TransRef (element, element, LoadedDocument[destinationDocument - 1]);
    }
  return ((Element) element);
}

/* ---------------------------------------------------------------------- *
   CreateDescent
   CreateDescent creates a new element of a given type and inserts it in the
   tree as a descendant of a given element. All elements of the descent required
   by the structure schema are also created, as well as the content of the
   required element if parameter withContent is TRUE.
   Parameters:
   document: the document for which the tree is created.
   element: the element for which a descent will be created.
   elementType: type of the element to be created as the last descendant.
   withContent: if TRUE, the minimum content if that element must also
   be created.
   Return value:
   the last descendant created or NULL if the element cannot be created.
   ---------------------------------------------------------------------- */
static Element CreateDescent (Document document, Element element,
                              ElementType elementType, ThotBool withContent)
{
  PtrElement          lastCreated;
  PtrElement          firstCreated;
  PtrElement          pEl;
  PtrElement          pSon;
  PtrElement          pBrother;
  PtrElement          pE;
  PtrElement          pNext;
  PtrElement          lastNew;
#ifndef NODISPLAY
  PtrElement          pNeighbour;
#endif
  ThotBool            ident;
  ThotBool            ok;

  UserErrorCode = 0;
  lastCreated = NULL;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else if (elementType.ElSSchema == NULL)
    TtaError (ERR_invalid_parameter);
  else if (document < 1 || document > MAX_DOCUMENTS)
    /* Checks the parameter document */
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else if (elementType.ElTypeNum < 1 ||
           elementType.ElTypeNum > ((PtrSSchema) (elementType.ElSSchema))->SsNRules)
    TtaError (ERR_invalid_element_type);
  else
    {
      pEl = (PtrElement) element;
      firstCreated = NULL;
      /* No sons for a terminal element */
      if (!pEl->ElTerminal)
        /* No sons for a copy */
        if (!pEl->ElIsCopy)
          firstCreated = CreateDescendant (pEl->ElTypeNumber, pEl->ElStructSchema,
                                           LoadedDocument[document - 1], &lastCreated,
                                           elementType.ElTypeNum,
                                           (PtrSSchema) (elementType.ElSSchema), pEl);
      if (firstCreated != NULL)
        {
          if (firstCreated->ElNext != NULL &&
              pEl->ElFirstChild != NULL)
            /* Too many trees created. One keep only the tree which contain the
               element to create */
            {
              /* Looking for the root of the tree to keep */
              pE = lastCreated;
              while (pE->ElParent != NULL)
                pE = pE->ElParent;
              pBrother = firstCreated;
              firstCreated = pE;
              /* Destroy other trees */
              while (pBrother != NULL)
                {
                  pNext = pBrother->ElNext;
                  if (pBrother != firstCreated)
                    DeleteElement (&pBrother, LoadedDocument[document - 1]);
                  pBrother = pNext;
                }
            }
          lastNew = firstCreated;
          while (lastNew->ElNext != NULL)
            lastNew = lastNew->ElNext;
          if (pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber - 1]->SrConstruct == CsChoice)
            {
              if (lastCreated == firstCreated)
                ident = TRUE;
              else
                ident = FALSE;
#ifndef NODISPLAY
              InsertOption (pEl, &firstCreated, LoadedDocument[document - 1]);
#else
              InsertElemInChoice (pEl, &firstCreated, LoadedDocument[document - 1], FALSE);
#endif
              /* chains the new element */
              if (pEl == firstCreated && ident)
                lastCreated = pEl;
            }
          else
            {
              if (AllowedFirstChild (pEl, LoadedDocument[document - 1],
                                     firstCreated->ElTypeNumber, firstCreated->ElStructSchema,
                                     FALSE, FALSE))
                {
                  pSon = pEl->ElFirstChild;
                  if (pSon != NULL)
                    {
                      if (pSon->ElTerminal &&
                          pSon->ElLeafType == LtPageColBreak)
                        SkipPageBreakBegin (&pSon);
                      else
                        pSon = NULL;
                    }
                  if (pSon != NULL)
                    /* Insert the element after the page mark */
                    InsertElementAfter (pSon, firstCreated);
                  else
                    InsertFirstChild (pEl, firstCreated);
                }
              else
                {
                  pSon = pEl->ElFirstChild;
                  ok = FALSE;
                  while (pSon != NULL && !ok)
                    {
                      if (AllowedSibling (pSon, LoadedDocument[document - 1],
                                          firstCreated->ElTypeNumber, firstCreated->ElStructSchema,
                                          FALSE, FALSE, FALSE))
                        {
                          /* One put the tree after pSon */
                          if (pSon->ElNext == NULL)
                            ok = TRUE;
                          else
                            /* one verifies the tree can be placed before pSon's brother */
                            ok = AllowedSibling (pSon->ElNext,
                                                 LoadedDocument[document - 1],
                                                 firstCreated->ElTypeNumber,
                                                 firstCreated->ElStructSchema,
                                                 TRUE, FALSE, FALSE);
                        }
                      if (!ok)
                        pSon = pSon->ElNext;
                    }
                  if (pSon == NULL)
                    {
                      lastCreated = NULL;
                      DeleteElement (&firstCreated, LoadedDocument[document - 1]);
                    }
                  else
                    InsertElementAfter (pSon, firstCreated);
                }
            }
          if (firstCreated != NULL)
            {
#ifndef NODISPLAY
              pNeighbour = SiblingElement (firstCreated, FALSE);
#endif
              if (withContent)
                if (!lastCreated->ElTerminal)
                  if (lastCreated->ElStructSchema->SsRule->SrElem[lastCreated->ElTypeNumber - 1]->SrConstruct != CsChoice)
                    {
                      pSon = NewSubtree (lastCreated->ElTypeNumber, lastCreated->ElStructSchema, LoadedDocument[document - 1], TRUE, FALSE, TRUE, TRUE);
                      if (pSon != NULL)
                        InsertFirstChild (lastCreated, pSon);
                    }
              pE = firstCreated;
              while (pE != NULL)
                {
                  if (pE == lastNew)
                    pNext = NULL;
                  else
                    pNext = pE->ElNext;
                  RemoveExcludedElem (&pE, LoadedDocument[document - 1]);
                  if (pE != NULL)
                    {
#ifndef NODISPLAY
                      /* Treats the required attributs of the created elements */
                      AttachMandatoryAttributes (pE, LoadedDocument[document - 1]);
                      RedisplayNewElement (document, pE, pNeighbour, TRUE, TRUE);
#endif
                    }
                  pE = pNext;
                }
            }
        }
    }
  return ((Element) lastCreated);
}

/* ----------------------------------------------------------------------
   TtaCreateDescent
   Creates a new element of a given type and inserts it in the tree as a
   descendant of a given element. All elements of the descent required by the
   structure schema are also created.
   Parameters:
   document: the document for which the tree is created.
   element: the element for which a descent will be created.
   elementType: type of the element to be created as the last descendant.
   Return value:
   the last descendant created or NULL if the element cannot be created.
   This element is empty.
   ---------------------------------------------------------------------- */
Element TtaCreateDescent (Document document, Element element,
                          ElementType elementType)
{
  return CreateDescent (document, element, elementType, FALSE);
}

/* ----------------------------------------------------------------------
   TtaCreateDescentWithContent
   Creates a new element of a given type and inserts it in the tree as a
   descendant of a given element. All elements of the descent required by the
   structure schema are created, as well as the content of the requested element.
   Parameters:
   document: the document for which the tree is created.
   element: the element for which a descent will be created.
   elementType: type of the element to be created as the last descendant.
   Return value:
   the last descendant created or NULL if the element cannot be created.
   If not NULL, the minimum content of that element has been created.
   ---------------------------------------------------------------------- */
Element TtaCreateDescentWithContent (Document document, Element element,
                                     ElementType elementType)
{
  return CreateDescent (document, element, elementType, TRUE);
}

/* ----------------------------------------------------------------------
   TtaDeleteTree
   Deletes a tree (or a single element) and frees it.
   All references that points at any element of that tree are
   cancelled.
   The deleted element must not be used later.
   Parameters:
   element: the element (or root of the tree) to be deleted.
   document: the document containing the element to be deleted.
   ---------------------------------------------------------------------- */
void TtaDeleteTree (Element element, Document document)
{
  PtrDocument         pDoc;
  ThotBool            root;
  PtrElement          pEl;

  UserErrorCode = 0;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else
    {
      /* Checks the parameter document */
      if (document == 0 && DocumentOfElement ((PtrElement) element) != NULL)
        TtaError (ERR_invalid_document_parameter);
      else if (document < 0 || document > MAX_DOCUMENTS)
        TtaError (ERR_invalid_document_parameter);
      else if (document > 0 && LoadedDocument[document - 1] == NULL)
        TtaError (ERR_invalid_document_parameter);
      else
        /* Parameter document is ok */
        {
          pEl = (PtrElement) element;
          if (document == 0)
            pDoc = NULL;
          else
            {
              pDoc = LoadedDocument[document - 1];
              root = (pEl->ElParent == NULL);
#ifndef NODISPLAY
              UndisplayElement (pEl, document);
#endif
              if (root && pDoc->DocDocElement == pEl)
                /* The whole main tree is destroyed */
                pDoc->DocDocElement = NULL;
            }
          DeleteElement (&pEl, pDoc);
        }
    }
}

/* ----------------------------------------------------------------------
   TtaAttachNewTree
   Attaches an entire tree to a document.
   Parameter:
   tree: root of the tree to be attached. This tree
   must be a valid main tree according to the document structure schema.
   document: the document to which the tree is to be attached.
   ---------------------------------------------------------------------- */
void TtaAttachNewTree (Element tree, Document document)
{
  PtrDocument         pDoc;
  PtrElement          pRoot;
  PtrSSchema          curExtension;
  ThotBool            found;
  ThotBool            ok;

  UserErrorCode = 0;
  pRoot = (PtrElement) tree;
  ok = FALSE;
  if (tree == NULL)
    TtaError (ERR_invalid_parameter);
  else
    /* Checks the parameter document */
    if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
    else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
    else
      /* Parameter document is ok */
      {
        pDoc = LoadedDocument[document - 1];
        /* verifies that the tree type is defined into the document schema 
           or into one of its extensions */
        found = FALSE;
        if (!strcmp (pDoc->DocSSchema->SsName, pRoot->ElStructSchema->SsName))
          /* document schema */
          found = TRUE;
        else if (pRoot->ElStructSchema->SsExtension)
          /* The tree is defined by an extension schema */
          /* Is it an extension of this document ? */
          {
            curExtension = pDoc->DocSSchema->SsNextExtens;
            while (!found && curExtension != NULL)
              if (!strcmp (pRoot->ElStructSchema->SsName, curExtension->SsName))
                found = TRUE;
              else
                curExtension = curExtension->SsNextExtens;
          }

        if (!found)
          /* The tree is not defined into a structure schema of the document */
          TtaError (ERR_element_does_not_match_DTD);
        else
          {
            if (pRoot->ElTypeNumber == pRoot->ElStructSchema->SsDocument &&
                pRoot->ElStructSchema == pDoc->DocSSchema)
              /* The main tree (starting at the document element) */
              {
#ifndef NODISPLAY
                if (pDoc->DocDocElement != NULL)
                  UndisplayElement (pDoc->DocDocElement, document);
#endif
                DeleteElement (&pDoc->DocDocElement, pDoc);
                pDoc->DocDocElement = pRoot;
                ok = TRUE;
              }
            else if (pRoot->ElTypeNumber == pRoot->ElStructSchema->SsRootElem &&
                     pRoot->ElStructSchema == pDoc->DocSSchema)
              /* The main tree (starting at the root) */
              {
#ifndef NODISPLAY
                if (pDoc->DocDocElement != NULL)
                  UndisplayElement (pRoot, document);
#endif
                DeleteElement (&pRoot, pDoc);
                InsertFirstChild (pDoc->DocDocElement, pRoot);
                ok = TRUE;
              }
            else
              TtaError (ERR_element_does_not_match_DTD);
            if (ok)
              {
                pRoot->ElAccess = ReadWrite;
                /* verifies if the new root has an attribut language */
                CheckLanguageAttr (pDoc, pRoot);
#ifndef NODISPLAY
                RedisplayNewElement (document, pRoot, NULL, TRUE, TRUE);
#endif
              }
          }
      }
}

/*----------------------------------------------------------------------
  TtaExportTree
  Saves an abstract tree into a file in a particular format. The output
  format is specified by a translation schema.
  Parameters:
  element: the root of the tree to be exported.
  document: the document containing the tree to be exported.
  fileName: name of the file in which the tree must be saved,
  including the directory name.
  TSchemaName: name of the translation schema to be used. The directory
  name must not be specified in parameter TSchemaName. See
  function TtaSetSchemaPath.
  ----------------------------------------------------------------------*/
void TtaExportTree (Element element, Document document,
                    const char *fileName, const char *TSchemaName)
{
#ifndef NODISPLAY
  UserErrorCode = 0;
  /* verifies the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else if (element == NULL)
    TtaError (ERR_invalid_parameter);
  /* parameter document is correct */
  ExportTree ((PtrElement)element, document, fileName, TSchemaName);
#endif
}

/* ----------------------------------------------------------------------
   TtaRedisplayElement
   Remove, recompute and redisplay the whole image of element elem.
   ---------------------------------------------------------------------- */
void TtaRedisplayElement (Element element, Document document)
{
#ifndef NODISPLAY
  PtrDocument         pDoc;

  UserErrorCode = 0;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    {
      pDoc = LoadedDocument[document - 1];
      DestroyAbsBoxes ((PtrElement) element, pDoc, TRUE);
      AbstractImageUpdated (pDoc);
      RedisplayNewElement (document, (PtrElement) element, NULL, TRUE, TRUE);
    }
#endif /* NODISPLAY */
}

/* ----------------------------------------------------------------------
   TtaInsertSibling
   Inserts an element in a tree, as an immediate sibling of a given element.
   The element to be inserted must not yet be part of a document.
   Parameters:
   newElement: the element (or root of the tree) to be inserted.
   sibling: an element belonging to a tree. This element
   must not be the root of a tree.
   before: if TRUE, inserts newElement as previous sibling of sibling,
   if FALSE, inserts newElement as next sibling of sibling.
   document: the document to which both elements belong.
   ---------------------------------------------------------------------- */
void TtaInsertSibling (Element newElement, Element sibling,
                       ThotBool before, Document document)
{
#ifndef NODISPLAY
  PtrElement          pNeighbour, pOtherNeighbour;
  ThotBool            first;
#endif
  PtrElement          pEl, pSibling;
  PtrDocument         pDoc;

  UserErrorCode = 0;
  pEl = (PtrElement) newElement;
  pSibling = (PtrElement) sibling;
  if (pEl == NULL || pSibling == NULL)
    TtaError (ERR_invalid_parameter);
  else if (pEl->ElParent != NULL)
    TtaError (ERR_element_already_inserted);
  else if (pSibling->ElParent == NULL)
    /* cannot insert an element as a sibling of a root */
    TtaError (ERR_element_already_inserted);
  else if (document < 1 || document > MAX_DOCUMENTS)
    /* Checks the parameter document */
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else if (((LoadedDocument[document - 1])->DocCheckingMode & STR_CHECK_MASK)
           && !AllowedSibling (pSibling, LoadedDocument[document - 1],
                               pEl->ElTypeNumber, pEl->ElStructSchema, before,
                               FALSE, FALSE))
    TtaError (ERR_element_does_not_match_DTD);
  else
    {
      pDoc = LoadedDocument[document - 1];
#ifndef NODISPLAY
      first = before;
      if (ElemDoesNotCount (pEl, before))
        /* the new element is not taken into account to decide whether its
           sibling is a first (resp. last) child or not. Its status won't
           change */
        pNeighbour = NULL;
      else
        {
          pNeighbour = SiblingElement (pSibling, before);
          if (!pNeighbour)
            /* element pSibling was a first (resp. last) child. After insertion
               its status will change */
            pNeighbour = pSibling;
          else
            /* a significant element prevented element pSibling to be a first
               (resp. last) child */
            {
              /* this significant element was perhaps a first (resp. last)
                 child (e.g. if pSibling was it self not significant) */
              pOtherNeighbour = SiblingElement (pNeighbour, !before);
              if (!pOtherNeighbour)
                /* it was first (resp. last). Change its status */
                first = !first;
              else
                pNeighbour = NULL;
            }
        }
#endif
      if (before)
        InsertElementBefore (pSibling, pEl);
      else
        InsertElementAfter (pSibling, pEl);
      /* treats the exclusions of the created element */
      if (pDoc->DocCheckingMode & STR_CHECK_MASK)
        RemoveExcludedElem (&pEl, pDoc);
      if (pEl)
        {
          if (pEl->ElReferredDescr)
            /* the descriptor should be linked to the current document */
            LinkReferredElDescr (pEl->ElReferredDescr, pDoc);
          /* If element pair, chain it with its homologue */
          if (pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber - 1]->SrConstruct == CsPairedElement)
            GetOtherPairedElement (pEl);
#ifndef NODISPLAY
          /* treats the required attributs of created elements */
          if (pDoc->DocCheckingMode & ATTR_MANDATORY_MASK)
            AttachMandatoryAttributes (pEl, pDoc);
          RedisplayNewElement (document, pEl, pNeighbour, first, TRUE);
#endif /* NODISPLAY */
        }
    }
}

/* ----------------------------------------------------------------------
   TtaInsertFirstChild
   Inserts an element in a tree, as the first child of a given element.
   The element to be inserted must not yet be part of a document.
   This function can also be used for attaching an option to a choice.
   Parameters:
   newElement: the element (or root of the tree) to be inserted.
   parent: an element belonging to a tree.
   document: the document to which both elements belong.
   Return parameter:
   If newElement is an option that replaces the choice, newElement takes
   the value of parent.
   ---------------------------------------------------------------------- */
void TtaInsertFirstChild (Element *newElement, Element parent, Document document)
{
#ifndef NODISPLAY
  PtrElement          pNeighbour;
#endif
  PtrElement          pSon, pParent;
  PtrDocument         pDoc;
  ThotBool            ok;

  UserErrorCode = 0;
  ok = FALSE;
  if (*newElement == NULL || parent == NULL)
    TtaError (ERR_invalid_parameter);
  else if (((PtrElement) (*newElement))->ElParent != NULL)
    TtaError (ERR_element_already_inserted);
  else if (*newElement == parent)
    TtaError (ERR_invalid_parameter);
  /* Checks the parameter document */
  else if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  /* Parameter document is ok ? */
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else if (((PtrElement) parent)->ElTerminal)
    TtaError (ERR_element_does_not_match_DTD);
  else
    {
      pDoc = LoadedDocument[document - 1];
      pParent = (PtrElement) parent;
      if (pParent->ElStructSchema == NULL)
        return;
      if (pParent->ElStructSchema->SsRule->SrElem[pParent->ElTypeNumber - 1]->SrConstruct == CsChoice)
        {
#ifndef NODISPLAY
          InsertOption (pParent, (PtrElement *) newElement, pDoc);
#else
          InsertElemInChoice (pParent, (PtrElement *) newElement, pDoc, FALSE);
#endif
          ok = TRUE;
        }
      else if (((pDoc)->DocCheckingMode & STR_CHECK_MASK)
               && !AllowedFirstChild (pParent, pDoc,
                                      ((PtrElement) (*newElement))->ElTypeNumber, 
                                      ((PtrElement) (*newElement))->ElStructSchema, 
                                      FALSE, FALSE))
        TtaError (ERR_element_does_not_match_DTD);
      else
        {
          pSon = pParent->ElFirstChild;
          if (pSon)
            {
              if (pSon->ElTerminal && pSon->ElLeafType == LtPageColBreak)
                /* Ignore the page marks */
                SkipPageBreakBegin (&pSon);
              else
                pSon = NULL;
            }
          if (pSon)
            /* There is page marks, the element is inserted after these marks */
            InsertElementAfter (pSon, (PtrElement) (*newElement));
          else
            InsertFirstChild (pParent, (PtrElement) (*newElement));
          ok = TRUE;
        }
      
      if (ok)
        {
          pSon = (PtrElement) (*newElement);
#ifndef NODISPLAY
          pNeighbour = SiblingElement (pSon, FALSE);
#endif
          /* Treats the exclusions in the created element */
          if (pDoc->DocCheckingMode & STR_CHECK_MASK)
            RemoveExcludedElem ((PtrElement *) newElement, pDoc);
          pSon = (PtrElement) (*newElement);
          if (pSon)
            {
              if (pSon->ElReferredDescr)
                /* the descriptor should be linked to the current document */
                LinkReferredElDescr (pSon->ElReferredDescr, pDoc);
#ifndef NODISPLAY
              /* treats the required attibutes of the created elements */
              if (pDoc->DocCheckingMode & ATTR_MANDATORY_MASK)
                AttachMandatoryAttributes (pSon, pDoc);
              RedisplayNewElement (document, pSon, pNeighbour, TRUE, TRUE);
#endif
            }
        }
    }
}

/* ----------------------------------------------------------------------
   TtaRemoveTree
   Removes a tree (or a single element) from its tree, without freeing it.
   Parameters:
   element: the element (or root of the tree) to be removed.
   document: the document containing the element to be removed.
   ---------------------------------------------------------------------- */
void TtaRemoveTree (Element element, Document document)
{
  PtrDocument         pDoc;
  PtrElement          pEl = (PtrElement) element;
  ThotBool            root;

  UserErrorCode = 0;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  /* Checks the parameter document */
  else if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    /* Parameter document is ok */
    {
      pDoc = LoadedDocument[document - 1];
      root = (pEl->ElParent == NULL);
#ifndef NODISPLAY
      UndisplayElement (pEl, document);
#else
      RemoveElement (pEl);
#endif
      if (pEl->ElReferredDescr)
        /* the descriptor if no longer linked to the current document */
        UnlinkReferredElDescr (pEl->ElReferredDescr);
      if (root && pDoc->DocDocElement == (PtrElement) element)
        /* The main tree is cleared */
        pDoc->DocDocElement = NULL;
    }
}

/* ----------------------------------------------------------------------
   TtaSetElementLineNumber
   Set the line number of a given element.
   Parameter:
   element: the element.
   nb: line number of the element.
   ---------------------------------------------------------------------- */
void TtaSetElementLineNumber (Element element, int nb)
{
  UserErrorCode = 0;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else
    ((PtrElement) element)->ElLineNb = nb;
}

/* ----------------------------------------------------------------------
   TtaSetAccessRight
   Sets the access rights for a given element.  Access rights apply only during
   the current editing session; they are not saved with the document. They must
   be set each time the document is loaded.
   Parameters:
   element: the element.
   right: access right for that element (ReadOnly, ReadWrite, Hidden).
   document: the document to which the element belongs.
   ---------------------------------------------------------------------- */
void TtaSetAccessRight (Element element, AccessRight right, Document document)
{
#ifndef NODISPLAY
  AccessRight         oldAccessRight;
  AccessRight         newAccessRight = ReadWrite;
#endif

  UserErrorCode = 0;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else
    {
      /* Checks the parameter document */
      if (document < 1 || document > MAX_DOCUMENTS)
        TtaError (ERR_invalid_document_parameter);
      else if (LoadedDocument[document - 1] == NULL)
        TtaError (ERR_invalid_document_parameter);
      else
        /* Parameter document is ok */
        {
#ifndef NODISPLAY
          if (ElementIsHidden ((PtrElement) element))
            oldAccessRight = Hidden;
          else if (ElementIsReadOnly ((PtrElement) element))
            oldAccessRight = ReadOnly;
          else
            oldAccessRight = ReadWrite;
#endif
          switch (right)
            {
            case ReadOnly:
              ((PtrElement) element)->ElAccess = ReadOnly;
#ifndef NODISPLAY
              newAccessRight = ReadOnly;
#endif
              break;
            case ReadWrite:
              ((PtrElement) element)->ElAccess = ReadWrite;
#ifndef NODISPLAY
              newAccessRight = ReadWrite;
#endif
              break;
            case Hidden:
              ((PtrElement) element)->ElAccess = Hidden;
#ifndef NODISPLAY
              newAccessRight = Hidden;
#endif
              break;
            case Inherited:
              ((PtrElement) element)->ElAccess = Inherited;
#ifndef NODISPLAY
              if (ElementIsHidden ((PtrElement) element))
                newAccessRight = Hidden;
              else if (ElementIsReadOnly ((PtrElement) element))
                newAccessRight = ReadOnly;
              else
                newAccessRight = ReadWrite;
#endif
              break;
            default:
              TtaError (ERR_invalid_parameter);
              break;
            }
#ifndef NODISPLAY
          if (newAccessRight != oldAccessRight)
            {
              /* Rights of the element are modified */
              if (newAccessRight == Hidden)
                /* The element is hidden, clear its abstract boxes */
                HideElement ((PtrElement) element, document);
              else if (oldAccessRight == Hidden)
                /* The element is not hiddden, Creating its abstract boxes */
                RedisplayNewElement (document, (PtrElement) element, NULL, TRUE, FALSE);
            }
#endif /* NODISPLAY */
        }
    }
}

/* ----------------------------------------------------------------------
   TtaHolophrastElement
   Changes the holophrasting status of a given element.
   Parameters:
   element: the element.
   holophrast: TRUE: the element gets holophrasted if it is not,
   FALSE: if the element is holphrasted, it gets expanded.
   document: the document to which the element belongs.
   ---------------------------------------------------------------------- */
void TtaHolophrastElement (Element element, ThotBool holophrast,
                           Document document)
{
  ThotBool            CanHolo;

  UserErrorCode = 0;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else if (document < 1 || document > MAX_DOCUMENTS)
    /* Checks the parameter document */
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else if (((PtrElement) element)->ElParent == NULL)
    /* Parameter document is ok */
    TtaError (ERR_cannot_holophrast_a_root);
  else
    {
      CanHolo = TRUE;
      if ((((PtrElement) element)->ElTerminal &&
           ((PtrElement) element)->ElLeafType == LtPageColBreak) ||
          !CanHolo)
        /* No holophraste for page breaks or some arrays elements */
        TtaError (ERR_cannot_holophrast_that_type);
      else if (((PtrElement) element)->ElHolophrast != holophrast)
        {
          ((PtrElement) element)->ElHolophrast = holophrast;
#ifndef NODISPLAY
          DisplayHolophrasted ((PtrElement) element, document);
#endif
        }
    }
}

/* ----------------------------------------------------------------------
   TtaSetMandatoryInsertion
   Activate or disable element and attributes insertion. When a
   modification of the abstract tree is performed, mandatory elements
   and attributes are not inserted. 
   By default, insertion is activated.
   Parameter:
   on: 0 disables the insertion. All other values activates it.
   document: the document for which insertion is changed.

   ---------------------------------------------------------------------- */
void TtaSetMandatoryInsertion (ThotBool on, Document document)
{
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else if (on == 0)
    (LoadedDocument [document - 1])->DocCheckingMode &= ~(ATTR_MANDATORY_MASK | EL_MANDATORY_MASK);
  else
    (LoadedDocument [document - 1])->DocCheckingMode |= (ATTR_MANDATORY_MASK | EL_MANDATORY_MASK);
}

/* ----------------------------------------------------------------------
   TtaSetStructureChecking
   Activate or disable structure checking. When structure checking is
   activated, modifications of the abstract tree are refused if they
   lead to an invalid structure with respect to the structure schema.
   By default, checking is activated.
   Parameter:
   on: 0 disables structure checking. All other values activates
   structure checking.
   document: the document for which structure checking is changed.
   ---------------------------------------------------------------------- */
void TtaSetStructureChecking (ThotBool on, Document document)
{
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else if (on == 0)
    (LoadedDocument [document - 1])->DocCheckingMode &= ~COMPLETE_CHECK_MASK;
  else
    (LoadedDocument [document - 1])->DocCheckingMode |= COMPLETE_CHECK_MASK;
}

/* ----------------------------------------------------------------------
   TtaGetStructureChecking
   Return the current structure checking mode for a given document.
   When structure checking is activated, modifications of the abstract tree
   are refused if they lead to an invalid structure with respect to the
   structure schema. By default, checking is activated.
   Parameter:
   document: the document for which structure checking is asked.
   ---------------------------------------------------------------------- */
ThotBool TtaGetStructureChecking (Document document)
{
  ThotBool	ret;

  ret = FALSE;
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1]->DocCheckingMode & COMPLETE_CHECK_MASK)
    ret = TRUE;
  return ret;
}

/* ----------------------------------------------------------------------
   TtaSetCheckingMode
   Changes checking mode.
   Parameter:
   strict: if TRUE, the presence of all mandatory elements is checked.
   ---------------------------------------------------------------------- */
void TtaSetCheckingMode (ThotBool strict)
{
  FullStructureChecking = strict;
}

/* ----------------------------------------------------------------------
   TtaNextAssociatedRoot
   Returns the root element of the associated tree that follows the
   tree to which a given element belongs.
   Parameters:
   document: the document.
   root: the element for which the next associated tree is searched.
   That element does not need to be the root of a tree.
   If root is NULL or if root is an element in the main tree, the
   root of the first associated tree is returned.
   Return parameter:
   root: the root element of the next associated tree.
   NULL if there is no next associated tree for the document.
   ---------------------------------------------------------------------- */
void TtaNextAssociatedRoot (Document document, Element * root)
{
  PtrElement          nextRoot;

  UserErrorCode = 0;
  nextRoot = NULL;
  *root = (Element) nextRoot;
}

/* ----------------------------------------------------------------------
   TtaGetFirstChild
   Returns the first child element of a given element.
   Parameter:
   parent: the element for which the first child element is asked.
   Return value:
   the first child element of parent; NULL if parent has no child.
   ---------------------------------------------------------------------- */
Element TtaGetFirstChild (Element parent)
{
  PtrElement          child;
  PtrElement          pParent = (PtrElement) parent;

  UserErrorCode = 0;
  child = NULL;
  if (parent == NULL)
    TtaError (ERR_invalid_parameter);
  else if (!pParent->ElTerminal && pParent->ElFirstChild)
    {
      if (pParent->ElFirstChild->ElParent != pParent)
        TtaError (ERR_incorrect_tree);
      else
        child = pParent->ElFirstChild;
    }
  return ((Element) child);
}

/* ----------------------------------------------------------------------
   TtaGetLastChild
   Returns the last child element of a given element.
   Parameter:
   parent: the element for which the last child element is asked.
   Return value:
   the last child element of parent; NULL if parent has no child.
   ---------------------------------------------------------------------- */
Element TtaGetLastChild (Element parent)
{
  PtrElement          child;
  PtrElement          pParent = (PtrElement) parent;

  UserErrorCode = 0;
  child = NULL;
  if (parent == NULL)
    TtaError (ERR_invalid_parameter);
  else if (!pParent->ElTerminal && pParent->ElFirstChild)
    {
      if (pParent->ElFirstChild->ElParent != pParent)
        TtaError (ERR_incorrect_tree);
      else
        {
          child = pParent->ElFirstChild;
          while (child->ElNext != NULL)
            child = child->ElNext;
        }
    }
  return ((Element) child);
}

/* ----------------------------------------------------------------------
   TtaGetFirstLeaf
   Returns the first leaf element of a given element.
   Parameter:
   parent: the element for which the first leaf element is asked.
   Return value:
   the first leaf element of parent; parent itself if it has no leaf
   ---------------------------------------------------------------------- */
Element TtaGetFirstLeaf (Element parent)
{
  Element          leaf;

  UserErrorCode = 0;
  leaf = parent;
  if (parent == NULL)
    TtaError (ERR_invalid_parameter);
  else
    leaf = (Element)FirstLeaf ((PtrElement)parent);
  return (leaf);
}

/* ----------------------------------------------------------------------
   TtaGetLastLeaf
   Returns the last leaf element of a given element.
   Parameter:
   parent: the element for which the last leaf element is asked.
   Return value:
   the last leaf element of parent; parent itself if it has no leaf.
   ---------------------------------------------------------------------- */
Element             TtaGetLastLeaf (Element parent)
{
  Element          leaf;

  UserErrorCode = 0;
  leaf = NULL;
  if (parent == NULL)
    TtaError (ERR_invalid_parameter);
  else
    leaf = (Element)LastLeaf ((PtrElement)parent);
  return (leaf);
}

/* ----------------------------------------------------------------------
   TtaPreviousSibling
   Returns the previous sibling element of a given element.
   Parameter:
   element: the element whose previous sibling is asked.
   Return parameter:
   element: the previous sibling element, or NULL if there is no
   previous sibling.
   ---------------------------------------------------------------------- */
void                TtaPreviousSibling (Element * element)
{
  PtrElement          sibling;

  UserErrorCode = 0;
  sibling = NULL;
  if ((PtrElement) (*element) == NULL)
    TtaError (ERR_invalid_parameter);
  else if (((PtrElement) (*element))->ElPrevious != NULL)
    {
      if (((PtrElement) (*element))->ElPrevious->ElNext != (PtrElement) (*element))
        TtaError (ERR_incorrect_tree);
      else
        sibling = ((PtrElement) (*element))->ElPrevious;
    }
  *element = (Element) sibling;
}

/* ----------------------------------------------------------------------
   TtaNextSibling
   Returns the next sibling element of a given element.
   Parameter:
   element: the element whose next sibling is asked.
   Return parameter:
   element: the next sibling element, or NULL if there is no next sibling.
   ---------------------------------------------------------------------- */
void                TtaNextSibling (Element * element)
{
  PtrElement          sibling;

  UserErrorCode = 0;
  sibling = 0;
  if (*element == NULL)
    TtaError (ERR_invalid_parameter);
  else if (((PtrElement) (*element))->ElNext != NULL)
    {
      if (((PtrElement) (*element))->ElNext->ElPrevious != (PtrElement) (*element))
        TtaError (ERR_incorrect_tree);
      else
        sibling = ((PtrElement) (*element))->ElNext;
    }
  *element = (Element) sibling;
}

/* ----------------------------------------------------------------------
   TtaGetSuccessor
   Returns the element that follows a given element at the same level or
   at the first upper level where there is a following element.
   Parameter:
   element: the element whose successor is asked.
   Return value:
   the successor, or NULL if there is no successor.
   ---------------------------------------------------------------------- */
Element             TtaGetSuccessor (Element element)
{
  PtrElement          successor;

  UserErrorCode = 0;
  successor = NULL;
  if (element == NULL)
    {
      TtaError (ERR_invalid_parameter);
    }
  else
    successor = NextElement ((PtrElement) element);
  return ((Element) successor);
}

/* ----------------------------------------------------------------------
   TtaGetPredecessor
   Returns the element that precedes a given element at the same level or
   at the first upper level where there is a preceding element.
   Parameter:
   element: the element whose predecessor is asked.
   Return value:
   the predecessor, or NULL if there is no predecessor.
   ---------------------------------------------------------------------- */
Element             TtaGetPredecessor (Element element)
{
  PtrElement          predecessor, pEl;

  UserErrorCode = 0;
  predecessor = NULL;
  if (element == NULL)
    {
      TtaError (ERR_invalid_parameter);
    }
  else
    {
      pEl = (PtrElement) element;
      while (pEl->ElPrevious == NULL && pEl->ElParent != NULL)
        pEl = pEl->ElParent;
      predecessor = pEl->ElPrevious;
    }
  return ((Element) predecessor);
}

/* ----------------------------------------------------------------------
   TtaGetCommonAncestor
   Returns the common ancestor element of two given elements.
   Parameters:
   element1: the first element whose ancestor is asked.
   element2: the second element whose ancestor is asked.
   Return value:
   the common ancestor, or NULL if there is no
   common ancestor.
   ---------------------------------------------------------------------- */
Element             TtaGetCommonAncestor (Element element1, Element element2)
{
  PtrElement          ancestor;

  UserErrorCode = 0;
  ancestor = NULL;
  if (element1 == NULL || element2 == NULL)
    TtaError (ERR_invalid_parameter);
  else
    ancestor = CommonAncestor ((PtrElement) element1, (PtrElement) element2);
  return ((Element) ancestor);
}

/* ----------------------------------------------------------------------
   TtaGetTypedAncestor
   Returns the first ancestor of a given type for a given element.
   Parameters:
   element: the element whose ancestor is asked.
   ancestorType: type of the asked ancestor.
   Return value:
   the ancestor, or NULL if there is no ancestor of that type.
   ---------------------------------------------------------------------- */
Element TtaGetTypedAncestor (Element element, ElementType ancestorType)
{
  PtrElement          ancestor;

  UserErrorCode = 0;
  ancestor = NULL;
  if (element == NULL || ancestorType.ElSSchema == NULL)
    TtaError (ERR_invalid_parameter);
  else if (ancestorType.ElTypeNum > ((PtrSSchema) (ancestorType.ElSSchema))->SsNRules
           || ancestorType.ElTypeNum < 1)
    TtaError (ERR_invalid_element_type);
  else
    ancestor = GetTypedAncestor (((PtrElement) element)->ElParent,
                                 ancestorType.ElTypeNum,
                                 (PtrSSchema) (ancestorType.ElSSchema));
  return ((Element) ancestor);
}

/* ----------------------------------------------------------------------
   TtaGetExactTypedAncestor
   Returns the first ancestor of the exact given type for a given element.
   Parameters:
   element: the element whose ancestor is asked.
   ancestorType: type of the asked ancestor.
   Return value:
   the ancestor, or NULL if there is no ancestor of that type.
   ---------------------------------------------------------------------- */
Element TtaGetExactTypedAncestor (Element element, ElementType ancestorType)
{
  PtrElement          ancestor;

  UserErrorCode = 0;
  ancestor = NULL;
  if (element == NULL || ancestorType.ElSSchema == NULL)
    TtaError (ERR_invalid_parameter);
  else if (ancestorType.ElTypeNum > ((PtrSSchema) (ancestorType.ElSSchema))->SsNRules
           || ancestorType.ElTypeNum < 1)
    TtaError (ERR_invalid_element_type);
  else
    {
      ancestor = (PtrElement)element;
      while (ancestor && ancestorType.ElTypeNum != ancestor->ElTypeNumber)
        {
          ancestor = GetTypedAncestor (ancestor->ElParent,
                                       ancestorType.ElTypeNum,
                                       (PtrSSchema) (ancestorType.ElSSchema));
        }
    }
  return ((Element) ancestor);
}

/* ----------------------------------------------------------------------
   TtaIsExtensionElement
   Returns true if the element is from an extension schema
   Parameter:
   element: the element.
   Return value:
   true or false.
   ---------------------------------------------------------------------- */
ThotBool TtaIsExtensionElement (Element element)
{

  UserErrorCode = 0;
  if (element == NULL)
    {
      TtaError (ERR_invalid_parameter);
      return FALSE;
    }
  else
    {
      return (((PtrElement) element)->ElStructSchema->SsExtension);
    }
}

/* ----------------------------------------------------------------------
   TtaIsTranscludedElement
   Returns true if the element is a transcluded element
   Parameter:
   element: the element.
   Return value:
   true or false.
   ---------------------------------------------------------------------- */
ThotBool TtaIsTranscludedElement (Element element)
{
  UserErrorCode = 0;
  if (element == NULL)
    {
      TtaError (ERR_invalid_parameter);
      return FALSE;
    }
  else
    if (!((PtrElement) element)->ElSource)
      return FALSE;
    else
      return (((PtrElement) element)->ElSource->RdTypeRef == RefInclusion);
}

/* ----------------------------------------------------------------------
   TtaGetElementTypeName
   Returns the name of an element type.
   Parameter:
   elementType: element type.
   Return value:
   name of that type.
   ---------------------------------------------------------------------- */
char *TtaGetElementTypeName (ElementType elementType)
{
  PtrSSchema schema = (PtrSSchema) elementType.ElSSchema;

  UserErrorCode = 0;
  nameBuffer[0] = EOS;
  if (schema == NULL || schema->SsName == NULL)
    TtaError (ERR_invalid_parameter);
  else if (elementType.ElTypeNum > schema->SsNRules || elementType.ElTypeNum < 1)
    TtaError (ERR_invalid_element_type);
  else
    strncpy (nameBuffer, schema->SsRule->SrElem[elementType.ElTypeNum - 1]->SrName,
             ELEM_NAME_LENGTH);
  return nameBuffer;
}

/* ----------------------------------------------------------------------
   TtaGetElementTypeOriginalName
   Returns the name of an element type in the language it is defined in
   the structure schema.
   Parameter:
   elementType: element type.
   Return value:
   original name of that type.
   ---------------------------------------------------------------------- */
char *TtaGetElementTypeOriginalName (ElementType elementType)
{
  UserErrorCode = 0;
  nameBuffer[0] = EOS;
  if (elementType.ElSSchema == NULL)
    TtaError (ERR_invalid_parameter);
  else if (elementType.ElTypeNum > ((PtrSSchema) (elementType.ElSSchema))->SsNRules ||
           elementType.ElTypeNum < 1)
    TtaError (ERR_invalid_element_type);
  else
    strncpy (nameBuffer, ((PtrSSchema) (elementType.ElSSchema))->SsRule->SrElem[elementType.ElTypeNum - 1]->SrOrigName, ELEM_NAME_LENGTH);
  return nameBuffer;
}

/* ----------------------------------------------------------------------
   TtaGiveTypeFromName
   Gives an element type whose name is known (the structure schema that
   defines that type must be loaded). That type is searched in a given
   structure schema (elementType.ElSSchema) and in all structure schemas
   that are extensions of that structure schema or natures used in that
   structure schema.
   Parameters:
   elementType.ElSSchema: the structure schema of interest.
   name: the name of the type of interest.
   Return parameter:
   elementType: the type having this name, or elementType.ElTypeNum = 0
   if the type is not found.
   ---------------------------------------------------------------------- */
void TtaGiveTypeFromName (ElementType * elementType, char *name)
{
  UserErrorCode = 0;
  (*elementType).ElTypeNum = 0;
  if (name == NULL || name[0] == EOS || (*elementType).ElSSchema == NULL)
    {
      (*elementType).ElSSchema = NULL;
      TtaError (ERR_invalid_parameter);
    }
  else
    GetSRuleFromName (&((*elementType).ElTypeNum),
                      (PtrSSchema *) (&((*elementType).ElSSchema)), name, USER_NAME);
}

/* ----------------------------------------------------------------------
   TtaGiveTypeFromOriginalName
   Gives an element type whose name is known (the structure schema that
   defines that type must be loaded). That type is searched in a given
   structure schema (elementType.ElSSchema) and in all structure schemas
   that are extensions of that structure schema or natures used in that
   structure schema.
   Parameters:
   elementType.ElSSchema: the structure schema of interest.
   name: the name of the type of interest in the language it is defined
   in the structure schema.
   Return parameter:
   elementType: the type having this name, or elementType.ElTypeNum = 0
   if the type is not found.
   ---------------------------------------------------------------------- */
void TtaGiveTypeFromOriginalName (ElementType * elementType, char *name)
{
  UserErrorCode = 0;
  (*elementType).ElTypeNum = 0;
  if (name == NULL || name[0] == EOS || (*elementType).ElSSchema == NULL)
    {
      (*elementType).ElSSchema = NULL;
      TtaError (ERR_invalid_parameter);
    }
  else
    GetSRuleFromName (&((*elementType).ElTypeNum), (PtrSSchema *) (&((*elementType).ElSSchema)), name, SCHEMA_NAME);
}

/* ----------------------------------------------------------------------
   TtaSameTypes
   Compares two element types.
   Parameters:
   type1: first element type.
   type2: second element type.
   Return value:
   0 if both types are different, 1 if they are identical.
   ---------------------------------------------------------------------- */
ThotBool TtaSameTypes (ElementType type1, ElementType type2)
{
  ThotBool                 result;

  UserErrorCode = 0;
  result = FALSE;
  if (type1.ElTypeNum < 1 || type2.ElTypeNum < 1)
    TtaError (ERR_invalid_element_type);
  else if (type1.ElSSchema == NULL)
    if (type1.ElTypeNum > MAX_BASIC_TYPE)
      TtaError (ERR_invalid_element_type);
    else if (type1.ElTypeNum == type2.ElTypeNum)
      result = TRUE;
    else
      result = FALSE;
  else if (type1.ElTypeNum > ((PtrSSchema) (type1.ElSSchema))->SsNRules)
    TtaError (ERR_invalid_element_type);
  else if (type1.ElTypeNum != type2.ElTypeNum)
    result = FALSE;
  else if (type2.ElSSchema == NULL)
    if (type2.ElTypeNum > MAX_BASIC_TYPE)
      TtaError (ERR_invalid_element_type);
    else
      result = TRUE;
  else if (type2.ElTypeNum > ((PtrSSchema) (type2.ElSSchema))->SsNRules)
    TtaError (ERR_invalid_element_type);
  else if (!strcmp (((PtrSSchema) (type1.ElSSchema))->SsName,
                    ((PtrSSchema) (type2.ElSSchema))->SsName))
    result = TRUE;
  else
    result = FALSE;
  return result;
}

/* ----------------------------------------------------------------------
   TtaGetElementLabel
   Returns the label of a given element.
   Parameter:
   element: the element.
   Return value:
   label of the element.
   ---------------------------------------------------------------------- */
char *TtaGetElementLabel (Element element)
{
  UserErrorCode = 0;
  nameBuffer[0] = EOS;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else 
    strncpy (nameBuffer, ((PtrElement) element)->ElLabel, ELEM_NAME_LENGTH);
  return nameBuffer;
}

/* ----------------------------------------------------------------------
   TtaGetElementLineNumber
   Returns the line number of a given element.
   Parameter:
   element: the element.
   Return value:
   line number of the element.
   ---------------------------------------------------------------------- */
int TtaGetElementLineNumber (Element element)
{
  int	lineNb;

  UserErrorCode = 0;
  lineNb = 0;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else
    lineNb = ((PtrElement) element)->ElLineNb;
  return lineNb;
}

/* ----------------------------------------------------------------------
   TtaGetElementLevel
   Returns the level of an element in the abstract tree. The root has
   level 0, its children have level 1, its grandchildren have level 2,
   and so on
   Parameter:
   element: the element.
   Return value:
   level of the element.
   ---------------------------------------------------------------------- */
int TtaGetElementLevel (Element element)
{
  int	      level;
  PtrElement  ancestor;

  UserErrorCode = 0;
  level = 0;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else
    {
      ancestor = ((PtrElement) element)->ElParent;
      while (ancestor)
        {
          level++;
          ancestor = ancestor->ElParent;
        }
    }
  return level;
}

/* ----------------------------------------------------------------------
   TtaIsConstant
   Indicates whether an element type is a constant.
   Parameter:
   elementType: type to be tested.
   Return value:
   1 = the type is a constant, 0 = the type is not a constant.
   ---------------------------------------------------------------------- */
int TtaIsConstant (ElementType elementType)
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
      if (((PtrSSchema) (elementType.ElSSchema))->SsRule->SrElem[elementType.ElTypeNum - 1]->SrConstruct == CsConstant)
        result = 1;
    }
  return result;
}

/* ----------------------------------------------------------------------
   TtaIsLeaf
   Indicates whether an element type is a leaf.
   Parameter:
   elementType: type to be tested.
   Return value:
   1 if the type is a leaf, 0 if the type is not a leaf.
   ---------------------------------------------------------------------- */
ThotBool TtaIsLeaf (ElementType elementType)
{
  ThotBool result;

  UserErrorCode = 0;
  result = FALSE;
  if (elementType.ElSSchema == NULL)
    TtaError (ERR_invalid_parameter);
  else if (elementType.ElTypeNum > ((PtrSSchema) (elementType.ElSSchema))->SsNRules ||
           elementType.ElTypeNum < 1)
    TtaError (ERR_invalid_element_type);
  else
    {
      if ((((PtrSSchema) (elementType.ElSSchema))->SsRule->SrElem[elementType.ElTypeNum - 1]->SrConstruct == CsConstant) ||
          (((PtrSSchema) (elementType.ElSSchema))->SsRule->SrElem[elementType.ElTypeNum - 1]->SrConstruct == CsReference) ||
          (((PtrSSchema) (elementType.ElSSchema))->SsRule->SrElem[elementType.ElTypeNum - 1]->SrConstruct == CsPairedElement) ||
          (((PtrSSchema) (elementType.ElSSchema))->SsRule->SrElem[elementType.ElTypeNum - 1]->SrConstruct == CsBasicElement))
        result = TRUE;
    }
  return result;
}

/* ----------------------------------------------------------------------
   TtaGetConstructOfType

   Returns the construct of an element type
   Parameter:
   elementType: the element type of interest.
   Return value:
   the construct that defines the structure of that element type.
   ---------------------------------------------------------------------- */
ConstructType TtaGetConstructOfType (ElementType elementType)
{
  ConstructType          result;

  UserErrorCode = 0;
  result = ConstructError;
  if (elementType.ElSSchema == NULL)
    TtaError (ERR_invalid_parameter);
  else if (elementType.ElTypeNum > ((PtrSSchema) (elementType.ElSSchema))->SsNRules ||
           elementType.ElTypeNum < 1)
    TtaError (ERR_invalid_element_type);
  else
    switch (((PtrSSchema) (elementType.ElSSchema))->SsRule->SrElem[elementType.ElTypeNum - 1]->SrConstruct)
      {
      case CsIdentity:
        result = ConstructIdentity;
        break;
      case CsList:
        result = ConstructList;
        break;
      case CsChoice:
        result = ConstructChoice;
        break;
      case CsAggregate:
        result = ConstructOrderedAggregate;
        break;
      case CsUnorderedAggregate:
        result = ConstructUnorderedAggregate;
        break;
      case CsConstant:
        result = ConstructConstant;
        break;
      case CsReference:
        result = ConstructReference;
        break;
      case CsBasicElement:
        result = ConstructBasicType;
        break;
      case CsNatureSchema:
        result = ConstructNature;
        break;
      case CsPairedElement:
        result = ConstructPair;
        break;
      case CsExtensionRule:
        result = ConstructError;
        break;
      case CsAny:
        result = ConstructAny;
        break;
      case CsDocument:
        result = ConstructDocument;
      case CsEmpty:
        result = ConstructEmpty;
      default:
        result = ConstructError;
        break;
      }
  return result;
}

/* ----------------------------------------------------------------------
   TtaGetCardinalOfType
   Returns the cardinal of an element type, e.g. the number of types
   that participates in its definition in the structure schema.
   Parameter:
   elementType: the element type of interest.
   Return value:
   the cardinal of that element type (integer value).
   ---------------------------------------------------------------------- */
int TtaGetCardinalOfType (ElementType elementType)
{
  int           result;
  PtrSRule      pRule;

  UserErrorCode = 0;
  result = 0;
  if (elementType.ElSSchema == NULL)
    TtaError (ERR_invalid_parameter);
  else if (elementType.ElTypeNum > ((PtrSSchema) (elementType.ElSSchema))->SsNRules ||
           elementType.ElTypeNum < 1)
    TtaError (ERR_invalid_element_type);
  else
    {
      pRule = ((PtrSSchema) (elementType.ElSSchema))->SsRule->SrElem[elementType.ElTypeNum - 1];
      switch (pRule->SrConstruct)
        {
        case CsIdentity:
        case CsConstant:
        case CsReference:
        case CsNatureSchema:
        case CsBasicElement:
        case CsList:
        case CsPairedElement:
        case CsExtensionRule:
          result = 1;
          break;
        case CsChoice:
          if (pRule->SrNChoices > 0)
            result = pRule->SrNChoices;
          else
            result = 0;
          break;
        case CsAggregate:
        case CsUnorderedAggregate:
          result = pRule->SrNComponents;
          break;
        default:
          result = 0;
          break;
        }
    }
  return result;
}

/* ----------------------------------------------------------------------
   TtaGiveConstructorsOfType
   Fills a array with the element types defining the given element type
   in the structure schema.
   Parameter:
   typesArray: a pointer to an initialized ElementType array.
   size: size of the array
   elementType: the element type of interest.
   Return value:
   typesArray: a array of element types.
   size: the number of types actually inserted in the array
   ---------------------------------------------------------------------- */
void TtaGiveConstructorsOfType (ElementType **typesArray,
                                int *size, ElementType elementType)
{ 
  PtrSRule      pRule;
  int i;

  UserErrorCode = 0;
  if (elementType.ElSSchema == NULL)
    TtaError (ERR_invalid_parameter);
  else if (elementType.ElTypeNum > ((PtrSSchema) (elementType.ElSSchema))->SsNRules ||
           elementType.ElTypeNum < 1)
    TtaError (ERR_invalid_element_type);
  else
    {
      pRule = ((PtrSSchema) (elementType.ElSSchema))->SsRule->SrElem[elementType.ElTypeNum - 1];
      switch (pRule->SrConstruct)
        {
        case CsNatureSchema:
          if (*size < 1)
            TtaError(ERR_buffer_too_small);
          else
            {
              ((*typesArray)[0]).ElSSchema = (SSchema)pRule->SrSSchemaNat;
              ((*typesArray)[0]).ElTypeNum = 0;
              *size = 1;
            }
          break;
        case CsBasicElement:
          if (*size < 1)
            TtaError(ERR_buffer_too_small);
          else
            {
              ((*typesArray)[0]).ElSSchema = elementType.ElSSchema;
              ((*typesArray)[0]).ElTypeNum = pRule->SrBasicType;
              *size = 1;
            }
          break;
        case CsReference:
          if(*size<1)
            TtaError(ERR_buffer_too_small);
          else
            {
              if(pRule->SrRefTypeNat[0] == 0)
                {
                  ((*typesArray)[0]).ElTypeNum = pRule->SrReferredType;
                  ((*typesArray)[0]).ElSSchema = elementType.ElSSchema;
                }
              else
                {
                  ((*typesArray)[0]).ElTypeNum = 0;
                  ((*typesArray)[0]).ElSSchema = NULL;
                }
              *size =1; 
            }
          break;
        case CsIdentity:
          if (*size < 1)
            TtaError(ERR_buffer_too_small);
          else
            {
              ((*typesArray)[0]).ElSSchema = elementType.ElSSchema;
              ((*typesArray)[0]).ElTypeNum = pRule->SrIdentRule;
              *size = 1;
            }
          break;
        case CsList:
          if (*size < 1)
            TtaError(ERR_buffer_too_small);
          else
            {
              ((*typesArray)[0]).ElSSchema = elementType.ElSSchema;
              ((*typesArray)[0]).ElTypeNum = pRule->SrListItem;
              *size = 1;
            }
          break;
        case CsChoice:
          if (*size < pRule->SrNChoices)
            {
              TtaError (ERR_buffer_too_small);
              *size = 0;
            }
          else
            {
              for (i = 0; i < pRule->SrNChoices; i++)
                {
                  ((*typesArray)[i]).ElSSchema = elementType.ElSSchema;
                  ((*typesArray)[i]).ElTypeNum = pRule->SrChoice[i];
                }
              *size = i;
            }
          break;
        case CsAggregate:
        case CsUnorderedAggregate:
          if (*size < pRule->SrNComponents)
            {
              TtaError(ERR_buffer_too_small);
              *size = 0;
            }
          else
            {
              for (i = 0; i< pRule->SrNComponents; i++)
                {
                  ((*typesArray)[i]).ElSSchema = elementType.ElSSchema;
                  ((*typesArray)[i]).ElTypeNum = pRule->SrComponent[i];
                }
              *size = i;
            }
          break;
        case CsConstant:
        case CsPairedElement:
        case CsExtensionRule:
          ((*typesArray)[0]).ElSSchema = elementType.ElSSchema;
          ((*typesArray)[0]).ElTypeNum = 0;
          break;
        default:
	        TtaError (ERR_invalid_element_type);
          break;
        }
    }
}

/* ----------------------------------------------------------------------
   TtaGetRankInAggregate
   Returns the rank that an element of type componentType should have in an
   aggregate of type aggregateType, according to the structure schema.
   Parameter:
   componentType: type of the element whose rank is asked.
   elementType: type of the aggregate.
   Return value:
   rank of the component (first component = 1), or 0 if no element of type
   componentType is allowed in the aggregate or if aggregateType is not
   an aggregate.
   ---------------------------------------------------------------------- */
int TtaGetRankInAggregate (ElementType componentType, ElementType aggregateType)
{
  int		rank, i;
  PtrSRule     pRule;

  UserErrorCode = 0;
  rank = 0;
  if (componentType.ElSSchema == NULL || aggregateType.ElSSchema == NULL)
    TtaError (ERR_invalid_parameter);
  else if (componentType.ElTypeNum > ((PtrSSchema) (componentType.ElSSchema))->SsNRules ||
           componentType.ElTypeNum < 1 ||
           aggregateType.ElTypeNum > ((PtrSSchema) (aggregateType.ElSSchema))->SsNRules ||
           aggregateType.ElTypeNum < 1)
    TtaError (ERR_invalid_element_type);
  else if (componentType.ElSSchema == aggregateType.ElSSchema)
    {
      pRule = ((PtrSSchema) (aggregateType.ElSSchema))->SsRule->SrElem[aggregateType.ElTypeNum - 1];
      if (pRule->SrConstruct == CsAggregate ||
          pRule->SrConstruct == CsUnorderedAggregate)
        for (i = 0; i < pRule->SrNComponents && rank == 0; i++)
          if (pRule->SrComponent[i] == componentType.ElTypeNum)
            rank = i+1;
    }
  return rank;
}

/* ----------------------------------------------------------------------
   TtaIsOptionalInAggregate
   Returns TRUE if component of rank rank is declared optionnal in 
   the aggregate of type elementType, according to the structure schema.
   Parameter:
   rank: the rank in the agreggate declaration of the component to be tested.
   elementType: type of the aggregate.
   Return value:
   TRUE if this component is optional.
   ---------------------------------------------------------------------- */
ThotBool TtaIsOptionalInAggregate (int rank, ElementType elementType)
{
  ThotBool       result;
  PtrSRule       pRule;

  UserErrorCode = 0;
  result = FALSE;
  if (elementType.ElSSchema == NULL)
    TtaError (ERR_invalid_parameter);
  else if (elementType.ElTypeNum > ((PtrSSchema) (elementType.ElSSchema))->SsNRules ||
           elementType.ElTypeNum < 1)
    TtaError (ERR_invalid_element_type);
  else
    {
      pRule = ((PtrSSchema) (elementType.ElSSchema))->SsRule->SrElem[elementType.ElTypeNum - 1];
      if (pRule->SrConstruct != CsAggregate && pRule->SrConstruct !=CsUnorderedAggregate)
        TtaError (ERR_invalid_element_type);
      else if(rank > pRule->SrNComponents)
        TtaError (ERR_invalid_parameter);
      else
        result = pRule->SrOptComponent[rank];
    }
  return result;
}

/* ----------------------------------------------------------------------
   TtaGetConstruct
   Returns the construct of an element.
   Parameter:
   element: the element of interest.
   Return value:
   the construct that defines the structure of that element.
   ---------------------------------------------------------------------- */
ConstructType TtaGetConstruct (Element element)
{
  ConstructType          result;

  UserErrorCode = 0;
  result = ConstructError;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else
    result = TtaGetConstructOfType (TtaGetElementType (element));
  return result;
}

/* ----------------------------------------------------------------------
   TtaGetAccessRight
   Returns the access rights for a given element.
   Parameter:
   element: the element.
   Return Value:
   access right for that element (ReadOnly, ReadWrite, Hidden, Inherited).
   ---------------------------------------------------------------------- */
AccessRight TtaGetAccessRight (Element element)
{
  AccessRight         right;

  UserErrorCode = 0;
  right = ReadWrite;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else
    {
      switch (((PtrElement) element)->ElAccess)
	      {
        case ReadOnly:
          right = ReadOnly;
          break;
        case ReadWrite:
          right = ReadWrite;
          break;
        case Hidden:
          right = Hidden;
          break;
        case Inherited:
          right = Inherited;
          break;
	      }
    }
  return right;
}

/* ----------------------------------------------------------------------
   TtaIsHolophrasted
   Tests whether a given element is holphrasted or not.
   Parameter:
   element: the element to be tested.
   Return Value:
   1 if the element is holphrasted, 0 if not.
   ---------------------------------------------------------------------- */
int TtaIsHolophrasted (Element element)
{
  int                 result;

  UserErrorCode = 0;
  result = 0;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else
    {
      if (((PtrElement) element)->ElHolophrast)
        result = 1;
      else
        result = 0;
    }
  return result;
}

/* ----------------------------------------------------------------------
   TtaIsReadOnly
   Tests whether a given element is protected against user modifications (ReadOnly).
   Parameter:
   element: the element to be tested.
   Return Value:
   1 if the element is protected, 0 if not.
   ---------------------------------------------------------------------- */
int TtaIsReadOnly (Element element)
{
  int                 result;

  UserErrorCode = 0;
  result = 0;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else
    {
      if (ElementIsReadOnly ((PtrElement) element))
        result = 1;
    }
  return result;
}

/* ----------------------------------------------------------------------
   TtaIsSetReadOnly
   Tests whether the given element itself is ReadOnly.
   Parameter:
   element: the element to be tested.
   Return Value:
   1 if the element is protected, 0 if not.
   ---------------------------------------------------------------------- */
int TtaIsSetReadOnly (Element element)
{
  int                 result;

  UserErrorCode = 0;
  result = 0;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else
    {
      if (((PtrElement) element)->ElAccess == ReadOnly)
        result = 1;
    }
  return result;
}

/* ----------------------------------------------------------------------
   TtaIsHidden
   Tests whether a given element is hidden to the user.
   Parameter:
   element: the element to be tested.
   Return Value:
   1 if the element is hidden, 0 if not.

   ---------------------------------------------------------------------- */
int TtaIsHidden (Element element)
{
  int                 result;

  UserErrorCode = 0;
  result = 0;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else if (ElementIsHidden ((PtrElement) element))
    result = 1;
  return result;
}

/* ----------------------------------------------------------------------
   TtaIsCopy
   Tests whether a given element is a copy.
   Parameter:
   element: the element to be tested.
   Return Value:
   1 if the element is a copy, 0 if not.

   ---------------------------------------------------------------------- */
int TtaIsCopy (Element element)
{
  int                 result;

  UserErrorCode = 0;
  result = 0;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else if (((PtrElement)element)->ElIsCopy)
    result = 1;
  return result;
}

/* ----------------------------------------------------------------------
   TtaIsGaphics
   Tests whether a given element is a graphics.
   Parameter:
   element: the element to be tested.
   Return Value:
   1 if the element is a graphics, 0 if not.

   ---------------------------------------------------------------------- */
int TtaIsGraphics (Element element)
{
  PtrElement  pEl = (PtrElement)element;
  int         result;

  UserErrorCode = 0;
  result = 0;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else if (pEl->ElTerminal &&
           (pEl->ElLeafType == LtPolyLine ||
            pEl->ElLeafType == LtGraphics ||
            pEl->ElLeafType == LtPath))
    result = 1;
  return result;
}

/* ----------------------------------------------------------------------
   TtaIsInAnInclusion
   Tests whether a given element is (in) an included element. An included element
   is a "live" copy of another element.
   Parameter:
   element: the element to be tested.
   Return Value:
   1 if the element is included, 0 if not.
   ---------------------------------------------------------------------- */
int TtaIsInAnInclusion (Element element)
{
  int                 result;
  PtrElement          pAsc;

  UserErrorCode = 0;
  result = 0;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else
    {
      pAsc = (PtrElement) element;
      while (pAsc->ElParent != NULL && pAsc->ElSource == NULL)
        pAsc = pAsc->ElParent;
      if (pAsc->ElSource != NULL)
        result = 1;
    }
  return result;
}

/* ----------------------------------------------------------------------
   TtaIsAncestor
   Tests if an element is an ancestor of another element.
   Parameters:
   element: an element.
   ancestor: the supposed ancestor of element.
   Return value:
   1 if ancestor in an ancestor of element, 0 if not.
   ---------------------------------------------------------------------- */
int TtaIsAncestor (Element element, Element ancestor)
{
  int                 result;

  UserErrorCode = 0;
  result = 0;
  if (element == NULL || ancestor == NULL)
    TtaError (ERR_invalid_parameter);
  else
    {
      if (ElemIsAnAncestor ((PtrElement) ancestor, (PtrElement) element))
        result = 1;
    }
  return result;
}

/* ----------------------------------------------------------------------
   TtaIsBefore
   Tests if an element precedes another element in the preorder traversal
   of the tree.
   Parameters:
   element1: the first element.
   element2: the second element.
   Return value:
   1 if the first element precedes the second element, 0 if not.
   ---------------------------------------------------------------------- */
int TtaIsBefore (Element element1, Element element2)
{
  int                 result;

  UserErrorCode = 0;
  result = 0;
  if (element1 == NULL || element2 == NULL)
    TtaError (ERR_invalid_parameter);
  else
    {
      if (ElemIsBefore ((PtrElement) element1, (PtrElement) element2))
        result = 1;
    }
  return result;
}

/* ----------------------------------------------------------------------
   TtaIsFirstPairedElement
   Indicates if a given paired element is the first or the second of the pair.
   Parameter:
   element: the paired element.
   Return value:
   1 if it is the first element of the pair, 0 if it is the second.
   ---------------------------------------------------------------------- */
int TtaIsFirstPairedElement (Element element)
{
  int                 result;

  UserErrorCode = 0;
  result = 0;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else if (((PtrElement) element)->ElStructSchema->SsRule->SrElem[((PtrElement) element)->ElTypeNumber - 1]->SrConstruct != CsPairedElement)
    {
      TtaError (ERR_invalid_element_type);
    }
  else if (((PtrElement) element)->ElStructSchema->SsRule->SrElem[((PtrElement) element)->ElTypeNumber - 1]->SrFirstOfPair)
    result = 1;
  return result;
}

/* ----------------------------------------------------------------------
   TtaCanInsertSibling
   Checks whether an element of a given type can be inserted in an
   abstract tree as an immediate sibling of an existing element.
   Parameters:
   elementType: element type to be checked.
   sibling: an existing element which is part of an abstract tree.
   before: if TRUE, checks if insertion is allowed before element "sibling".
   If FALSE, checks if insertion is allowed after element "sibling".
   document: the document to which element "sibling" belongs.
   Return value:
   TRUE if that element type can be inserted, FALSE if the structure
   schema does not allow that insertion.
   ---------------------------------------------------------------------- */
ThotBool TtaCanInsertSibling (ElementType elementType, Element sibling,
                              ThotBool before, Document document)
{
  ThotBool            ret;

  UserErrorCode = 0;
  ret = FALSE;
  if (sibling == NULL)
    TtaError (ERR_invalid_parameter);
  else if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    /* Parameter document is ok */
    if (elementType.ElSSchema == NULL)
      TtaError (ERR_invalid_parameter);
    else if (elementType.ElTypeNum < 1 ||
             elementType.ElTypeNum > ((PtrSSchema) (elementType.ElSSchema))->SsNRules)
      TtaError (ERR_invalid_element_type);
    else
      ret = AllowedSibling ((PtrElement) sibling, LoadedDocument[document - 1],
                            elementType.ElTypeNum,
                            (PtrSSchema) (elementType.ElSSchema),
                            before, FALSE, FALSE);
  return ret;
}

/* ----------------------------------------------------------------------
   TtaCanInsertFirstChild
   Checks whether an element of a given type can be inserted in an
   abstract tree as the first child of an existing element (parent).
   Parameters:
   elementType: element type to be checked.
   parent: an existing element which is part of an abstract tree.
   document: the document to which element parent belongs.
   Return value:
   TRUE if that element type can be inserted, FALSE if the structure
   schema does not allow that insertion.
   ---------------------------------------------------------------------- */
ThotBool TtaCanInsertFirstChild (ElementType elementType, Element parent,
                                 Document document)
{
  PtrElement          pParent = (PtrElement) parent;
  ThotBool            ret;

  UserErrorCode = 0;
  ret = FALSE;
  if (parent == NULL)
    TtaError (ERR_invalid_parameter);
  else if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    /* Parameter document is ok */
    if (elementType.ElSSchema == NULL)
      TtaError (ERR_invalid_parameter);
    else if (elementType.ElTypeNum < 1 ||
             elementType.ElTypeNum > ((PtrSSchema) (elementType.ElSSchema))->SsNRules)
      TtaError (ERR_invalid_element_type);
    else
      ret = AllowedFirstChild (pParent, LoadedDocument[document - 1],
                               elementType.ElTypeNum,
                               (PtrSSchema) (elementType.ElSSchema),
                               FALSE, FALSE);
  return ret;
}

#ifndef NODISPLAY
/* ----------------------------------------------------------------------
   TtaNextCopiedElement
   Returns one of the elements that have been copied into the ``clipboard''
   by the last Copy or Cut command. (This function is available only in the
   ThotEditor library).
   Parameter:
   element: NULL if the first element of the clipboard is asked;
   an element of the clipboard if the next one is asked.
   Return parameter:
   element: the asked element if it exists, or NULL if there is no next
   element in the clipboard or if the clipboard is empty.
   ---------------------------------------------------------------------- */
void TtaNextCopiedElement (Element * element)
{
  PtrPasteElem        next;

  UserErrorCode = 0;
  next = NULL;
  if (*element == NULL)
    next = FirstSavedElement;
  else if (!IsASavedElement ((PtrElement) (*element)))
    /* element is not in the clipboard */
    TtaError (ERR_invalid_parameter);
  else if (((PtrElement) (*element))->ElParent != NULL)
    TtaError (ERR_invalid_parameter);
  else
    {
      next = FirstSavedElement;
      while (next != NULL &&
             next->PeElement != (PtrElement) (*element))
        next = next->PeNext;
      if (next == NULL)
        TtaError (ERR_invalid_parameter);
      else
        next = next->PeNext;
    }
  if (next == NULL)
    *element = NULL;
  else
    *element = (Element) (next->PeElement);
}

/* ----------------------------------------------------------------------
   TtaGetCopiedDocument
   Returns the document from which the current content of the clipboard
   has been copied or cut. (This function is available only in the ThotEditor
   library).
   Parameters:
   No parameter.
   Return value:
   the document from which the current content of the clipboard has been
   copied or cut; 0 if the clipboard is empty.
   ---------------------------------------------------------------------- */
Document TtaGetCopiedDocument ()
{
  UserErrorCode = 0;
  if (DocOfSavedElements == NULL)
    return 0;
  else
    return IdentDocument (DocOfSavedElements);
}
#endif

/* ----------------------------------------------------------------------
   ---------------------------------------------------------------------- */
static PtrElement SearchLabel (char *label, PtrElement pEl)
{
  PtrElement          pE, pFound;

  pFound = NULL;
  if (strcmp (label, pEl->ElLabel) == 0)
    pFound = pEl;
  else if (!pEl->ElTerminal && pEl->ElFirstChild != NULL)
    {
      pE = pEl->ElFirstChild;
      do
        {
          pFound = SearchLabel (label, pE);
          if (pFound != NULL)
            pE = NULL;
          else
            pE = pE->ElNext;
        }
      while (pE != NULL);
    }
  return pFound;
}

/* ----------------------------------------------------------------------
   TtaSearchElementByLabel
   Searches the element that has a given label. The search is done in
   a given tree.
   Parameters:
   searchedLabel: label of element to be searched.
   element: the element that is the root of the tree in which the search
   is done.
   Return value:
   the element found, or NULL if no element has been found.
   ---------------------------------------------------------------------- */
Element TtaSearchElementByLabel (char *searchedLabel, Element element)
{
  PtrElement          elementFound;

  UserErrorCode = 0;
  elementFound = NULL;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else if (searchedLabel[0] == EOS)
    TtaError (ERR_invalid_element_type);
  else
    elementFound = SearchLabel (searchedLabel, (PtrElement) element);
  return ((Element) elementFound);
}

/* ----------------------------------------------------------------------
   ---------------------------------------------------------------------- */
static PtrElement SearchSSchema (SSchema sschema, PtrElement pEl)
{
  PtrElement          pE, pFound;

  pFound = NULL;
  if ((PtrSSchema)sschema == pEl->ElStructSchema)
    pFound = pEl;
  else if (!pEl->ElTerminal && pEl->ElFirstChild != NULL)
    {
      pE = pEl->ElFirstChild;
      do
        {
          pFound = SearchSSchema (sschema, pE);
          if (pFound != NULL)
            pE = NULL;
          else
            pE = pE->ElNext;
        }
      while (pE != NULL);
    }
  return pFound;
}

/* ----------------------------------------------------------------------
   TtaSearchElementBySchema
   Searches the first element that has a given schema.
   The search is done in a given tree.
   Parameters:
   searchedSschema: schema of element to be searched.
   element: the element that is the root of the tree in which the search
   is done.
   Return value:
   the element found, or NULL if no element has been found.
   ---------------------------------------------------------------------- */
Element TtaSearchElementBySchema (SSchema sschema, Element element)
{
  PtrElement          elementFound;

  UserErrorCode = 0;
  elementFound = NULL;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else if (sschema == NULL)
    TtaError (ERR_invalid_parameter);
  else
    elementFound = SearchSSchema (sschema, (PtrElement) element);
  return ((Element) elementFound);
}

/* ----------------------------------------------------------------------
   TtaSearchEmptyElement
   Searches the next empty element. An empty element is either a compound
   element without child or a leaf without content.
   Searching can be done in a tree or starting from a given element towards
   the beginning or the end of the abstract tree.
   Parameters:
   scope: SearchForward, SearchBackward or SearchInTree.
   element: the element that is the root of the tree
   (if scope = SearchInTree) or the starting element
   (if scope = SearchForward or SearchBackward).
   Return values:
   the element found, or NULL if not found.
   ---------------------------------------------------------------------- */
Element TtaSearchEmptyElement (SearchDomain scope, Element element)
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
        pEl = BackSearchRefOrEmptyElem ((PtrElement) element, 1);
      else
        pEl = FwdSearchRefOrEmptyElem ((PtrElement) element, 1);
      if (pEl != NULL)
        if (scope == SearchInTree)
          {
            if (!ElemIsWithinSubtree (pEl, (PtrElement) element))
              pEl = NULL;
          }
      if (pEl != NULL)
        elementFound = pEl;
    }
  return ((Element) elementFound);
}

/* ----------------------------------------------------------------------
   TtaSearchOtherPairedElement
   Returns the element that is part of the same pair as a given element.
   Parameter:
   element: the element whose paired element is searched.
   Return value:
   the paired element.
   ---------------------------------------------------------------------- */
Element TtaSearchOtherPairedElement (Element element)
{
  PtrElement          pairedElement;

  UserErrorCode = 0;
  pairedElement = NULL;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else if (((PtrElement) element)->ElStructSchema->SsRule->SrElem[((PtrElement) element)->ElTypeNumber - 1]->SrConstruct != CsPairedElement)
    TtaError (ERR_invalid_element_type);
  else
    pairedElement = GetOtherPairedElement ((PtrElement) element);
  return ((Element) pairedElement);
}

/* ----------------------------------------------------------------------
   TtaSearchNoPageBreak
   Returns the first sibling element that is not a page break.
   Parameter:
   element: the element.
   forward: TRUE for skipping the next page breaks, FALSE for skipping
   the previous ones.
   Return value:
   the first sibling element, or NULL if there are
   only page breaks.
   ---------------------------------------------------------------------- */
Element TtaSearchNoPageBreak (Element element, ThotBool forward)
{
  PtrElement          noPage;

  UserErrorCode = 0;
  noPage = NULL;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else
    {
      if (forward)
        {
          noPage = ((PtrElement) element)->ElNext;
          FwdSkipPageBreak (&noPage);
        }
      else
        {
          noPage = ((PtrElement) element)->ElPrevious;
          BackSkipPageBreak (&noPage);
        }
    }
  return ((Element) noPage);
}

/* ----------------------------------------------------------------------
   TtaHasHiddenException
   Returns TRUE if the elType has the ExcHidden exception
   (Not DTD elements)
   ---------------------------------------------------------------------- */
ThotBool TtaHasHiddenException (ElementType elType)
{
  return TypeHasException (ExcHidden, elType.ElTypeNum, 
                           (PtrSSchema) elType.ElSSchema);
}

/* ----------------------------------------------------------------------
   TtaHasNotElementException
   Returns TRUE if the elType has the ExcNotAnElementNode exception
   (Comments, PIs, etc)
   ---------------------------------------------------------------------- */
ThotBool TtaHasNotElementException (ElementType elType)
{
  return TypeHasException (ExcNotAnElementNode, elType.ElTypeNum, 
                           (PtrSSchema) elType.ElSSchema);
}

/* ----------------------------------------------------------------------
   TtaHasInvisibleException
   Returns TRUE if the elType is defined by the document schema's
   DTD. For example, elements with the hidden and exception
   attributes are not included in the DTD.
   ---------------------------------------------------------------------- */
ThotBool TtaHasInvisibleException (AttributeType attrType)
{
  return TypeHasException (ExcInvisible, attrType.AttrTypeNum, 
                           (PtrSSchema) attrType.AttrSSchema);
}

/* ----------------------------------------------------------------------
   TtaHasReturnCreateNLException
   Returns TRUE if the elType has the CreateNL exception
   (Preformatted, STYLE_, SCRIPT_ andText_Area HTML elements
   ---------------------------------------------------------------------- */
ThotBool TtaHasReturnCreateNLException (ElementType elType)
{
  return TypeHasException (ExcReturnCreateNL, elType.ElTypeNum, 
                           (PtrSSchema) elType.ElSSchema);
}
