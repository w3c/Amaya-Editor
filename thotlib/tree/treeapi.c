/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *
 * Author: V. Quint (INRIA)
 *
 */ 

#include "ustring.h"
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
#include "displayview_f.h"
#include "documentapi_f.h"
#include "externalref_f.h"
#include "labelalloc_f.h"
#include "references_f.h"
#include "structcommands_f.h"
#include "structcreation_f.h"
#include "structmodif_f.h"
#include "structschema_f.h"
#include "translation_f.h"
#include "thotmsg_f.h"
#include "tree_f.h"
#include "viewapi_f.h"

static Name         nameBuffer;

/* ----------------------------------------------------------------- 
   SetAssocNumber: assign the number nAssoc of the associated element 
   to all sub-tree which root is pEl.  
   ------------------------------------------------------------------ */

#ifdef __STDC__
static void         SetAssocNumber (PtrElement pEl, int nAssoc)

#else  /* __STDC__ */
static void         SetAssocNumber (pEl, nAssoc)
PtrElement          pEl;
int                 nAssoc;

#endif /* __STDC__ */

{
   if (pEl != NULL)
     {
	pEl->ElAssocNum = nAssoc;
	if (!pEl->ElTerminal)
	  {
	     pEl = pEl->ElFirstChild;
	     while (pEl != NULL)
	       {
		  SetAssocNumber (pEl, nAssoc);
		  pEl = pEl->ElNext;
	       }
	  }
     }
}


/* ----------------------------------------------------------------------
   ChangeElementType

   Change the type of a given element.
   CAUTION: THIS FUNCTION SHOULD BE USED VERY CARFULLY!

   Parameters:
   element: the concerned element
   typeNum: new type for the element

   ---------------------------------------------------------------------- */

#ifdef __STDC__
void             ChangeElementType (Element element, int typeNum)

#else  /* __STDC__ */
void             ChangeElementType (element, typeNum)
Element             element;
int                 typeNum;

#endif /* __STDC__ */

{
   UserErrorCode = 0;
   if (element == NULL)
      TtaError (ERR_invalid_parameter);
   else
      if (typeNum < 1 || typeNum > ((PtrElement)element)->ElStructSchema->SsNRules)
	 TtaError (ERR_invalid_element_type);
      else
	 ((PtrElement)element)->ElTypeNumber = typeNum;
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

#ifdef __STDC__
Element             TtaNewElement (Document document, ElementType elementType)

#else  /* __STDC__ */
Element             TtaNewElement (document, elementType)
Document            document;
ElementType         elementType;

#endif /* __STDC__ */

{
   PtrElement          element;

   UserErrorCode = 0;
   element = NULL;
   if (elementType.ElSSchema == NULL)
     {
	TtaError (ERR_invalid_parameter);
     }
   else
      /* Checks the parameter document */
   if (document < 1 || document > MAX_DOCUMENTS)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else if (LoadedDocument[document - 1] == NULL)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else
      /* Parameter document is ok */
      if (elementType.ElTypeNum < 1 ||
   elementType.ElTypeNum > ((PtrSSchema) (elementType.ElSSchema))->SsNRules)
     {
	TtaError (ERR_invalid_element_type);
     }
   else
     {
	element = NewSubtree (elementType.ElTypeNum, (PtrSSchema) (elementType.ElSSchema),
		  LoadedDocument[document - 1], 0, FALSE, TRUE, TRUE, TRUE);
	if (element != NULL)
	   if (element->ElStructSchema->SsRule[element->ElTypeNumber - 1].SrConstruct == CsPairedElement)
	      if (!element->ElStructSchema->SsRule[element->ElTypeNumber - 1].SrFirstOfPair)
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

#ifdef __STDC__
Element             TtaNewTree (Document document, ElementType elementType, char* label)

#else  /* __STDC__ */
Element             TtaNewTree (document, elementType, label)
Document            document;
ElementType         elementType;
char*               label;

#endif /* __STDC__ */

{
   PtrElement          element;

   UserErrorCode = 0;
   element = NULL;
   if (elementType.ElSSchema == NULL)
     {
	TtaError (ERR_invalid_parameter);
     }
   else
      /* Checks the parameter document */
   if (document < 1 || document > MAX_DOCUMENTS)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else if (LoadedDocument[document - 1] == NULL)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else
      /* Parameter document is ok */
      if (elementType.ElTypeNum < 1 ||
   elementType.ElTypeNum > ((PtrSSchema) (elementType.ElSSchema))->SsNRules)
     {
	TtaError (ERR_invalid_element_type);
     }
   else
     {
	element = NewSubtree (elementType.ElTypeNum, (PtrSSchema) (elementType.ElSSchema),
			  LoadedDocument[document - 1], 0, TRUE, TRUE, TRUE,
			      (ThotBool)(*label == EOS));
	if (element->ElStructSchema->SsRule[element->ElTypeNumber - 1].SrConstruct == CsPairedElement)
	   if (!element->ElStructSchema->SsRule[element->ElTypeNumber - 1].SrFirstOfPair)
	      element->ElPairIdent = 0;
	if (*label != EOS)
	   strncpy (element->ElLabel, label, MAX_LABEL_LEN);
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
#ifdef __STDC__
static void         TransRef (PtrElement pElem, PtrElement pRoot, PtrDocument pDoc)

#else  /* __STDC__ */
static void         TransRef (pElem, pRoot, pDoc)
PtrElement          pElem;
PtrElement          pRoot;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrElement          pSon;
   PtrReferredDescr    pDescRef;
   PtrReference        pRef, pNextRef;
   PtrReference        pPR1;

   /* Sets a new label to the element */
   ConvertIntToLabel (NewLabel (pDoc), pElem->ElLabel);
   if (pElem->ElReferredDescr != NULL)
      /* This element is referenced. CopyTree did not copy its referenced element descriptor
         which is shared by the source element.
         Deals with source element references which are into the sub-tree which root is pRoot */
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
		       if (!pElem->ElReferredDescr->ReExternalRef)
			  pElem->ElReferredDescr->ReReferredElem = pElem;
		    }
		  strncpy (pElem->ElReferredDescr->ReReferredLabel, pElem->ElLabel, MAX_LABEL_LEN);
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

   Creates a copy of a tree.

   Parameters:
   sourceElement: element to be copied.
   sourceDocument: the document containing the element to be copied.
   destinationDocument: the document for which the copy must be created.
   parent: element that will become the parent of the created tree.

   Return value:
   the root element of the created tree.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
Element             TtaCopyTree (Element sourceElement, Document sourceDocument, Document destinationDocument, Element parent)

#else  /* __STDC__ */
Element             TtaCopyTree (sourceElement, sourceDocument, destinationDocument, parent)
Element             sourceElement;
Document            sourceDocument;
Document            destinationDocument;
Element             parent;

#endif /* __STDC__ */

{
   PtrElement          element;
   PtrElement          ancestor;
   PtrSSchema          pSS, nextExtension;

   UserErrorCode = 0;
   element = NULL;
   /* verifies the parameters Document */
   if (sourceDocument < 1 || sourceDocument > MAX_DOCUMENTS ||
       destinationDocument < 1 || destinationDocument > MAX_DOCUMENTS)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else if (LoadedDocument[sourceDocument - 1] == NULL ||
	    LoadedDocument[destinationDocument - 1] == NULL)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else
      /* parameters Document are ok */
   if (sourceElement == NULL)
     {
	TtaError (ERR_invalid_parameter);
     }
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
		if (ancestor->ElStructSchema->SsCode ==
		    ((PtrElement) sourceElement)->ElStructSchema->SsCode)
		   pSS = ancestor->ElStructSchema;
		else
		   ancestor = ancestor->ElParent;
	     if (pSS == NULL)
                if (((PtrElement) sourceElement)->ElStructSchema->SsExtension)
                  {
                     nextExtension = LoadedDocument[destinationDocument - 1]->DocSSchema->SsNextExtens;
                     while (nextExtension != NULL)
                       {
                          if (nextExtension->SsCode ==
                              ((PtrElement) sourceElement)->ElStructSchema->SsCode)
                             break;
                          nextExtension = nextExtension->SsNextExtens;
                       }
                     if (nextExtension == NULL)
                       {
                          TtaError (ERR_invalid_parameter);
                       }
                     pSS = nextExtension;
                  }
                else
                  {
		     if (LoadedDocument[destinationDocument - 1]->DocSSchema->SsCode ==
		         ((PtrElement) sourceElement)->ElStructSchema->SsCode)
		        pSS = LoadedDocument[destinationDocument - 1]->DocSSchema;
		     else if (((PtrElement) sourceElement)->ElTerminal)
		        pSS = ((PtrElement) parent)->ElStructSchema;
		     else
		        pSS = ((PtrElement) sourceElement)->ElStructSchema;
                  }
	  }
	/* Copying */
	element = CopyTree (((PtrElement) sourceElement),
			    LoadedDocument[sourceDocument - 1], 0, pSS,
			    LoadedDocument[destinationDocument - 1],
			    (PtrElement) parent,
			    TRUE, TRUE);
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
#ifdef __STDC__
static Element      CreateDescent (Document document, Element element, ElementType elementType, ThotBool withContent)
#else  /* __STDC__ */
static Element      CreateDescent (document, element, elementType, withContent)
Document            document;
Element             element;
ElementType         elementType;
ThotBool            withContent;
#endif /* __STDC__ */
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
					   LoadedDocument[document - 1], &lastCreated, pEl->ElAssocNum,
					   elementType.ElTypeNum,
					   (PtrSSchema) (elementType.ElSSchema));
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
	  if (pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrConstruct == CsChoice)
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
		    if (pSon->ElTerminal &&
			pSon->ElLeafType == LtPageColBreak)
		      SkipPageBreakBegin (&pSon);
		    else
		      pSon = NULL;
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
	      pNeighbour = firstCreated->ElNext;
	      /* ignore the following page marks */
	      FwdSkipPageBreak (&pNeighbour);
#endif
	      if (withContent)
		if (!lastCreated->ElTerminal)
		  if (lastCreated->ElStructSchema->SsRule[lastCreated->ElTypeNumber - 1].SrConstruct != CsChoice)
		    {
		      pSon = NewSubtree (lastCreated->ElTypeNumber, lastCreated->ElStructSchema, LoadedDocument[document - 1], lastCreated->ElAssocNum, TRUE, FALSE, TRUE, TRUE);
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
		      /* Dealing with exceptions */
		      CreateWithException (pE, LoadedDocument[document - 1]);
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
#ifdef __STDC__
Element             TtaCreateDescent (Document document, Element element, ElementType elementType)
#else  /* __STDC__ */
Element             TtaCreateDescent (document, element, elementType)
Document            document;
Element             element;
ElementType         elementType;
#endif /* __STDC__ */
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
#ifdef __STDC__
Element             TtaCreateDescentWithContent (Document document, Element element, ElementType elementType)
#else  /* __STDC__ */
Element             TtaCreateDescentWithContent (document, element, elementType
Document            document;
Element             element;
ElementType         elementType;
#endif /* __STDC__ */
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
#ifdef __STDC__
void                TtaDeleteTree (Element element, Document document)
#else  /* __STDC__ */
void                TtaDeleteTree (element, document)
Element             element;
Document            document;
#endif /* __STDC__ */
{
   PtrDocument         pDoc;
   ThotBool            root;
   PtrElement          pEl;

   UserErrorCode = 0;
   if (element == NULL)
     {
	TtaError (ERR_invalid_parameter);
     }
   else
     {
	/* Checks the parameter document */
	if (document < 1 || document > MAX_DOCUMENTS)
	  {
	     TtaError (ERR_invalid_document_parameter);
	  }
	else if (LoadedDocument[document - 1] == NULL)
	  {
	     TtaError (ERR_invalid_document_parameter);
	  }
	else
	   /* Parameter document is ok */
	  {
	     pDoc = LoadedDocument[document - 1];
	     pEl = (PtrElement) element;
	     RegisterExternalRef (pEl, pDoc, FALSE);
	     RegisterDeletedReferredElem (pEl, pDoc);
	     root = (pEl->ElParent == NULL);
#ifndef NODISPLAY
	     UndisplayElement (pEl, document);
#endif
	     if (root)
		if (pDoc->DocRootElement == pEl)
		   /* The whole main tree is destroyed */
		   pDoc->DocRootElement = NULL;
		else if (pEl->ElAssocNum > 0 &&
			 pDoc->DocAssocRoot[pEl->ElAssocNum - 1] == pEl)
		   pDoc->DocAssocRoot[pEl->ElAssocNum - 1] = NULL;
	     DeleteElement (&pEl, pDoc);
	  }
     }
}


/* ----------------------------------------------------------------------
   TtaAttachNewTree

   Attaches an entire tree (main tree or associated tree) to a document.

   Parameter:
   tree: root of the tree to be attached. This tree
   must be a valid main tree or associated tree according to the
   document structure schema.
   document: the document to which the tree is to be attached.
   ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtaAttachNewTree (Element tree, Document document)
#else  /* __STDC__ */
void                TtaAttachNewTree (tree, document)
Element             tree;
Document            document;
#endif /* __STDC__ */
{
   PtrDocument         pDoc;
   SRule              *pRule;
   PtrElement          pRoot;
   PtrSSchema          curExtension;
   int                 numAssoc, numAssocLibre;
   ThotBool            found;
   ThotBool            ok;

   UserErrorCode = 0;
   numAssocLibre = 0;
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
       if (pDoc->DocSSchema->SsCode == pRoot->ElStructSchema->SsCode)
	 /* document schema */
	 found = TRUE;
       else if (pRoot->ElStructSchema->SsExtension)
	 /* The tree is defined by an extension schema */
	 /* Is it an extension of this document ? */
	 {
	   curExtension = pDoc->DocSSchema->SsNextExtens;
	   while (!found && curExtension != NULL)
	     if (pRoot->ElStructSchema->SsCode == curExtension->SsCode)
	       found = TRUE;
	     else
	       curExtension = curExtension->SsNextExtens;
	 }

       if (!found)
	 /* The tree is not defined into a structure schema of the document */
	 TtaError (ERR_element_does_not_match_DTD);
       else
	 {
	   if (pRoot->ElTypeNumber == pRoot->ElStructSchema->SsRootElem)
	     /* The main tree */
	     {
#ifndef NODISPLAY
	       if (pDoc->DocRootElement != NULL)
		 UndisplayElement (pDoc->DocRootElement, document);
#endif
	       /* Don't remove the root element */
	       DeleteElement (&pDoc->DocRootElement, pDoc);
	       pDoc->DocRootElement = pRoot;
	       numAssocLibre = 0;
	       ok = TRUE;
	     }
	   else if (pRoot->ElTerminal)
	     /* Is it a right associated tree ? */
	     TtaError (ERR_element_does_not_match_DTD);
	   else
	     {
	       /* One access to the rule which defines the root of the tree */
	       pRule = &pRoot->ElStructSchema->SsRule[pRoot->ElTypeNumber - 1];
	       if (pRule->SrConstruct != CsList)
		 /* Error: not a list */
		 TtaError (ERR_element_does_not_match_DTD);
	       else
		 /* It is a list */
		 {
		   /* one access to rules defining the elements of the list */
		   pRule = &pRoot->ElStructSchema->SsRule[pRule->SrListItem - 1];
		   if (!pRule->SrAssocElem)
		     /* Error: elements of the list are not associated elements */
		     TtaError (ERR_element_does_not_match_DTD);
		   else
		     ok = TRUE;
		 }
	       if (ok)
		 {
		   /* Verifies if some associated elements of this type already exist into the document */
		   numAssocLibre = 0;
		   numAssoc = 1;
		   found = FALSE;
		   while (!found && numAssoc <= MAX_ASSOC_DOC)
		     {
		       if (pDoc->DocAssocRoot[numAssoc - 1] == NULL)
			 {
			   if (numAssocLibre == 0)
			     numAssocLibre = numAssoc;
			 }
		       else if (pRoot->ElTypeNumber == pDoc->DocAssocRoot[numAssoc - 1]->ElTypeNumber)
			 found = TRUE;
		       if (!found)
			 numAssoc++;
		     }
		   if (found)
		     /* Associated elements of this type already exist */
		     {
#ifndef NODISPLAY
		       if (pDoc->DocAssocRoot[numAssoc - 1] != NULL)
			 UndisplayElement (pDoc->DocAssocRoot[numAssoc - 1],
					   document);
#endif
		       DeleteElement (&pDoc->DocAssocRoot[numAssoc - 1], pDoc);
		       if (numAssocLibre == 0)
			 numAssocLibre = numAssoc;
		     }
		   if (numAssocLibre == 0)
		     /* All associated elements of the document are busy */
		     {
		       TtaError (ERR_invalid_parameter);
		       ok = FALSE;
		     }
		   else
		     pDoc->DocAssocRoot[numAssocLibre - 1] = pRoot;
		 }
	     }
	   /* change the associated element number of the whole tree */
	   if (ok)
	     {
	       pRoot->ElAccess = AccessReadWrite;
	       SetAssocNumber (pRoot, numAssocLibre);
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
#ifdef __STDC__
void                TtaExportTree (Element element, Document document, STRING fileName, STRING TSchemaName)
#else  /* __STDC__ */
void                TtaExportTree (element, document, fileName, TSchemaName)
Element             element;
Document            document;
STRING              fileName;
STRING              TSchemaName;
#endif /* __STDC__ */
{
  UserErrorCode = 0;
  /* verifies the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else if (element == NULL)
    TtaError (ERR_invalid_parameter);
    /* parameter document is correct */
    ExportTree ((PtrElement)element, LoadedDocument[document - 1], fileName, TSchemaName);
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
#ifdef __STDC__
void                TtaInsertSibling (Element newElement, Element sibling, ThotBool before, Document document)
#else  /* __STDC__ */
void                TtaInsertSibling (newElement, sibling, before, document)
Element             newElement;
Element             sibling;
ThotBool            before;
Document            document;
#endif /* __STDC__ */
{
#ifndef NODISPLAY
   PtrElement          pNeighbour;

#endif
   PtrElement          pEl;

   UserErrorCode = 0;
   if (newElement == NULL || sibling == NULL)
     {
	TtaError (ERR_invalid_parameter);
     }
   else if (((PtrElement) newElement)->ElParent != NULL)
      TtaError (ERR_element_already_inserted);
   else if (((PtrElement) sibling)->ElParent == NULL)
      /* cannot insert an element as a sibling of a root */
      TtaError (ERR_element_already_inserted);
   else
      /* Checks the parameter document */
   if (document < 1 || document > MAX_DOCUMENTS)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else if (LoadedDocument[document - 1] == NULL)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else
      /* Parameter document is ok */
      if (((LoadedDocument[document - 1])->DocCheckingMode & STR_CHECK_MASK)
	  && !AllowedSibling ((PtrElement) sibling,
			      LoadedDocument[document - 1],
			      ((PtrElement) newElement)->ElTypeNumber,
			      ((PtrElement) newElement)->ElStructSchema,
			      before, FALSE, FALSE))
     {
	TtaError (ERR_element_does_not_match_DTD);
     }
   else
     {
	if (before)
	  {
#ifndef NODISPLAY
	     pNeighbour = ((PtrElement) sibling)->ElPrevious;
	     BackSkipPageBreak (&pNeighbour);
#endif
	     InsertElementBefore ((PtrElement) sibling, (PtrElement) newElement);
	  }
	else
	  {
#ifndef NODISPLAY
	     pNeighbour = ((PtrElement) sibling)->ElNext;
	     FwdSkipPageBreak (&pNeighbour);
#endif
	     InsertElementAfter ((PtrElement) sibling, (PtrElement) newElement);
	  }
	/* updates the associated element number */
	SetAssocNumber ((PtrElement) newElement,
			((PtrElement) sibling)->ElAssocNum);
	/* treats the exclusions of the created element */
	pEl = (PtrElement) newElement;
	if ((LoadedDocument[document - 1])->DocCheckingMode & STR_CHECK_MASK)
	   RemoveExcludedElem (&pEl, LoadedDocument[document - 1]);
	if (pEl != NULL)
	  {
	     /* Dealing with exceptions */
	     CreateWithException (pEl, LoadedDocument[document - 1]);
	     /* If element pair, chain it with its homologue */
	     if (((PtrElement) newElement)->ElStructSchema->SsRule[((PtrElement) newElement)->ElTypeNumber - 1].SrConstruct == CsPairedElement)
		GetOtherPairedElement ((PtrElement) newElement);
#ifndef NODISPLAY
	     /* treats the required attributs of created elements */
	     if ((LoadedDocument[document - 1])->DocCheckingMode & ATTR_MANDATORY_MASK)
		AttachMandatoryAttributes (pEl, LoadedDocument[document - 1]);
	     if (pNeighbour != NULL)
		/* The inserted element is not the first nor the last between its brothers */
		sibling = NULL;
	     RedisplayNewElement (document, pEl,
			       (PtrElement) sibling, before, TRUE);
#endif
	  }
     }
}

/* ----------------------------------------------------------------------
   TtaAskFirstCreation

   Asks interactive creation for "UserSpecified" elements
   ---------------------------------------------------------------------- */
void                TtaAskFirstCreation ()
{
   FirstCreation = TRUE;
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
#ifdef __STDC__
void                TtaInsertFirstChild (Element * newElement, Element parent, Document document)
#else  /* __STDC__ */
void                TtaInsertFirstChild (newElement, parent, document)
Element            *newElement;
Element             parent;
Document            document;
#endif /* __STDC__ */
{
#ifndef NODISPLAY
   PtrElement          pNeighbour;

#endif
   ThotBool            ok;
   PtrElement          pSon;

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
	if (((PtrElement) parent)->ElStructSchema->SsRule[((PtrElement) parent)->ElTypeNumber - 1].SrConstruct == CsChoice)
	  {
	     ((PtrElement) (*newElement))->ElAssocNum = ((PtrElement) parent)->ElAssocNum;
#ifndef NODISPLAY
	     InsertOption ((PtrElement) parent, (PtrElement *) newElement,
			   LoadedDocument[document - 1]);
#else
	     InsertElemInChoice ((PtrElement) parent, (PtrElement *) newElement, LoadedDocument[document - 1], FALSE);
#endif
	     ok = TRUE;
	  }
	else if (((LoadedDocument[document - 1])->DocCheckingMode & STR_CHECK_MASK)
		 && !AllowedFirstChild ((PtrElement) parent, 
					LoadedDocument[document - 1],
					((PtrElement) (*newElement))->ElTypeNumber, 
					((PtrElement) (*newElement))->ElStructSchema, 
					FALSE, FALSE))
	   TtaError (ERR_element_does_not_match_DTD);
	else
	  {
	     pSon = ((PtrElement) parent)->ElFirstChild;
	     if (pSon != NULL)
		if (pSon->ElTerminal && pSon->ElLeafType == LtPageColBreak)
		   /* Ignore the page marks */
		   SkipPageBreakBegin (&pSon);
		else
		   pSon = NULL;
	     if (pSon != NULL)
		/* There is page marks, the element is inserted after these marks */
		InsertElementAfter (pSon, (PtrElement) (*newElement));
	     else
		InsertFirstChild ((PtrElement) parent, (PtrElement) (*newElement));
	     ok = TRUE;
	  }

	if (ok)
	  {
#ifndef NODISPLAY
	     pNeighbour = ((PtrElement) (*newElement))->ElNext;
	     /* ignore the following page marks */
	     FwdSkipPageBreak (&pNeighbour);
#endif
	     /* updates the associated element number */
	     SetAssocNumber ((PtrElement) (*newElement),
			     ((PtrElement) parent)->ElAssocNum);
	     /* Treats the exclusions in the created element */
	     if ((LoadedDocument [document - 1])->DocCheckingMode & STR_CHECK_MASK)
		RemoveExcludedElem ((PtrElement *) newElement, LoadedDocument[document - 1]);
	     if ((PtrElement) (*newElement) != NULL)
	       {
		  /* Dealing with exceptions */
		  CreateWithException ((PtrElement) (*newElement),
				       LoadedDocument[document - 1]);
#ifndef NODISPLAY
		  /* treats the required attibutes of the created elements */
		  if ((LoadedDocument [document - 1])->DocCheckingMode & ATTR_MANDATORY_MASK)
		     AttachMandatoryAttributes ((PtrElement) (*newElement), LoadedDocument[document - 1]);
		  RedisplayNewElement (document, (PtrElement) (*newElement),
				    pNeighbour, TRUE, TRUE);
#endif
	       }
	  }
     }
}

/* ----------------------------------------------------------------------
   TtaInsertElement

   Create an element of a given type and insert it at the current position within
   a given document. The current position is defined by the current selection.
   If the current position is a single position (insertion point) the new element
   is simply inserted at that position. If one or several characters and/or
   elements are selected, the new element is created before the first selected
   character/element and the selected characters/elements are not changed.

   Parameters:
   elementType: type of the element to be created.
   document: the document for which the element is created.

   ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtaInsertElement (ElementType elementType, Document document)
#else  /* __STDC__ */
void                TtaInsertElement (elementType, document)
ElementType         elementType;
Document            document;

#endif /* __STDC__ */
{
   UserErrorCode = 0;
   if (elementType.ElSSchema == NULL)
      TtaError (ERR_invalid_parameter);
   else if (document < 1 || document > MAX_DOCUMENTS)
      /* Checks the parameter document */
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else if (elementType.ElTypeNum < 1 ||
   elementType.ElTypeNum > ((PtrSSchema) (elementType.ElSSchema))->SsNRules)
      /* Parameter document is ok */
      TtaError (ERR_invalid_element_type);
   else
      CreateNewElement (elementType.ElTypeNum,
			(PtrSSchema) (elementType.ElSSchema),
			LoadedDocument[document - 1], TRUE);
}

/* ----------------------------------------------------------------------
   TtaRemoveTree

   Removes a tree (or a single element) from its tree, without freeing it.

   Parameters:
   element: the element (or root of the tree) to be removed.
   document: the document containing the element to be removed.

   ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtaRemoveTree (Element element, Document document)
#else  /* __STDC__ */
void                TtaRemoveTree (element, document)
Element             element;
Document            document;

#endif /* __STDC__ */
{
   PtrDocument         pDoc;
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
	RegisterExternalRef ((PtrElement) element, pDoc, FALSE);
	RegisterDeletedReferredElem ((PtrElement) element, pDoc);
	root = (((PtrElement) element)->ElParent == NULL);
#ifndef NODISPLAY
	UndisplayElement ((PtrElement) element, document);
#else
	RemoveElement ((PtrElement) element);
#endif
	if (root)
	   if (pDoc->DocRootElement == (PtrElement) element)
	      /* Tha main tree is cleared */
	      pDoc->DocRootElement = NULL;
	   else if (pDoc->DocAssocRoot[((PtrElement) element)->ElAssocNum - 1] == (PtrElement) element)
	      /* Verifies if it is the root of an associated tree */
	      pDoc->DocAssocRoot[((PtrElement) element)->ElAssocNum - 1] = NULL;
     }
}


/* ----------------------------------------------------------------------
   TtaSetElementLineNumber

   Set the line number of a given element.

   Parameter:
   element: the element.
   nb: line number of the element.
   ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtaSetElementLineNumber (Element element, int nb)
#else  /* __STDC__ */
void                TtaSetElementLineNumber (element, nb)
Element             element;
int		    int;
#endif /* __STDC__ */
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
#ifdef __STDC__
void                TtaSetAccessRight (Element element, AccessRight right, Document document)
#else  /* __STDC__ */
void                TtaSetAccessRight (element, right, document)
Element             element;
AccessRight         right;
Document            document;
#endif /* __STDC__ */
{
#ifndef NODISPLAY
  AccessRight         oldAccessRight;
  AccessRight         newAccessRight = 0;
  DisplayMode         SaveDisplayMode;
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
	    oldAccessRight = (AccessRight) AccessHidden;
	  else if (ElementIsReadOnly ((PtrElement) element))
	    oldAccessRight = (AccessRight) AccessReadOnly;
	  else
	    oldAccessRight = (AccessRight) AccessReadWrite;
#endif
	  switch (right)
	    {
	    case ReadOnly:
	      ((PtrElement) element)->ElAccess = AccessReadOnly;
#ifndef NODISPLAY
	      newAccessRight = (AccessRight) AccessReadOnly;
#endif
	      break;
	    case ReadWrite:
	      ((PtrElement) element)->ElAccess = AccessReadWrite;
#ifndef NODISPLAY
	      newAccessRight = (AccessRight) AccessReadWrite;
#endif
	      break;
	    case Hidden:
	      ((PtrElement) element)->ElAccess = AccessHidden;
#ifndef NODISPLAY
	      newAccessRight = (AccessRight) AccessHidden;
#endif
	      break;
	    case Inherited:
	      ((PtrElement) element)->ElAccess = AccessInherited;
#ifndef NODISPLAY
	      if (ElementIsHidden ((PtrElement) element))
		newAccessRight = (AccessRight) AccessHidden;
	      else if (ElementIsReadOnly ((PtrElement) element))
		newAccessRight = (AccessRight) AccessReadOnly;
	      else
		newAccessRight = (AccessRight) AccessReadWrite;
#endif
	      break;
	    default:
	      TtaError (ERR_invalid_parameter);
	      break;
	    }
#ifndef NODISPLAY
	  if (newAccessRight != oldAccessRight)
	    /* Rights of the element are modified */
	    if (newAccessRight == AccessHidden)
	      /* The element is hidden, clear its abstract boxes */
	      HideElement ((PtrElement) element, document);
	    else if (oldAccessRight == AccessHidden)
	      /* The element is not hiddden, Creating its abstract boxes */
	      RedisplayNewElement (document, (PtrElement) element, NULL, TRUE, FALSE);
	    else
	      {
		SaveDisplayMode = TtaGetDisplayMode (document);
		if (SaveDisplayMode != NoComputedDisplay
		    && (newAccessRight == AccessReadOnly ||
			newAccessRight == AccessReadWrite))
		  /* change AbCanBeModified in all abstract boxes except if it's
		     without image calculation mode */
		  {
		    /* We set the deferred displaying mode */
		    if (SaveDisplayMode != DeferredDisplay)
		      TtaSetDisplayMode (document, DeferredDisplay);
		    /* change AbCanBeModified for all abstract boxes */
		    ChangeAbsBoxModif ((PtrElement) element, document, (ThotBool)(newAccessRight == AccessReadWrite));
		    /* Restore the display mode of the document */
		    /* Redisplay if the mode is immediat display */
		    if (SaveDisplayMode != DeferredDisplay)
		      TtaSetDisplayMode (document, SaveDisplayMode);
		  }
	      }
#endif
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
#ifdef __STDC__
void                TtaHolophrastElement (Element element, ThotBool holophrast, Document document)
#else  /* __STDC__ */
void                TtaHolophrastElement (element, holophrast, document)
Element             element;
ThotBool            holophrast;
Document            document;
#endif /* __STDC__ */
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
	if (ThotLocalActions[T_holotable] != NULL)
	   (*ThotLocalActions[T_holotable]) (((PtrElement) element), &CanHolo);
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
#ifdef __STDC__
void                TtaSetMandatoryInsertion (ThotBool on, Document document)
#else  /* __STDC__ */
void                TtaSetMandatoryInsertion (on, document)
ThotBool            on;
Document            document;

#endif /* __STDC__ */
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
#ifdef __STDC__
void                TtaSetStructureChecking (ThotBool on, Document document)
#else  /* __STDC__ */
void                TtaSetStructureChecking (on, document)
ThotBool            on;
Document            document;
#endif /* __STDC__ */
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
#ifdef __STDC__
int                 TtaGetStructureChecking (Document document)
#else  /* __STDC__ */
int                 TtaGetStructureChecking (document)
Document            document;
#endif /* __STDC__ */
{
  int	ret;

  ret = 0;
  if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
  else
    ret = !(ThotBool)(~COMPLETE_CHECK_MASK | (LoadedDocument[document - 1]->DocCheckingMode));
  return ret;
}


/* ----------------------------------------------------------------------
   TtaSetCheckingMode

   Changes checking mode.

   Parameter:
   strict: if TRUE, the presence of all mandatory elements is checked.
   ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtaSetCheckingMode (ThotBool strict)
#else  /* __STDC__ */
void                TtaSetCheckingMode (strict)
ThotBool            strict;
#endif /* __STDC__ */
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
#ifdef __STDC__
void                TtaNextAssociatedRoot (Document document, Element * root)
#else  /* __STDC__ */
void                TtaNextAssociatedRoot (document, root)
Document            document;
Element            *root;
#endif /* __STDC__ */
{
   int                 assoc;
   PtrDocument         pDoc;
   PtrElement          nextRoot;
   PtrElement          pEl;
   ThotBool            found;

   UserErrorCode = 0;
   nextRoot = NULL;
   /* Checks the parameter document */
   if (document < 1 || document > MAX_DOCUMENTS)
	TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
	TtaError (ERR_invalid_document_parameter);
   else
      /* Parameter document is ok */
     {
	if (*root == NULL)
	     nextRoot = LoadedDocument[document - 1]->DocAssocRoot[0];
	else
	  {
	     pDoc = LoadedDocument[document - 1];
	     pEl = (PtrElement) (*root);
	     /* Go to the root of the tree */
	     while (pEl->ElParent != NULL)
		pEl = pEl->ElParent;
	     if (pEl == pDoc->DocRootElement)
		/* It's the main tree, the associated tree is returned */
		nextRoot = pDoc->DocAssocRoot[0];
	     else
	       {
		  /* Looking for associated elements which root is pEl */
		  found = FALSE;
		  for (assoc = 0; assoc < MAX_ASSOC_DOC && !found; assoc++)
		     if (pDoc->DocAssocRoot[assoc] == pEl)
		       {
			  if (assoc < MAX_ASSOC_DOC - 1)
			     nextRoot = pDoc->DocAssocRoot[assoc + 1];
			  else
			     nextRoot = NULL;
			  found = TRUE;
		       }
		  if (!found)
		     TtaError (ERR_invalid_associated_root);
	       }
	  }
     }
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
#ifdef __STDC__
Element             TtaGetFirstChild (Element parent)
#else  /* __STDC__ */
Element             TtaGetFirstChild (parent)
Element             parent;
#endif /* __STDC__ */
{
   PtrElement          child;

   UserErrorCode = 0;
   child = NULL;
   if (parent == NULL)
     {
	TtaError (ERR_invalid_parameter);
     }
   else if (!((PtrElement) parent)->ElTerminal)
      if (((PtrElement) parent)->ElFirstChild != NULL)
	 if (((PtrElement) parent)->ElFirstChild->ElParent != (PtrElement) parent)
	   {
	      TtaError (ERR_incorrect_tree);
	   }
	 else
	    child = ((PtrElement) parent)->ElFirstChild;
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
#ifdef __STDC__
Element             TtaGetLastChild (Element parent)
#else  /* __STDC__ */
Element             TtaGetLastChild (parent)
Element             parent;
#endif /* __STDC__ */
{
   PtrElement          child;

   UserErrorCode = 0;
   child = NULL;
   if (parent == NULL)
     {
	TtaError (ERR_invalid_parameter);
     }
   else if (!((PtrElement) parent)->ElTerminal)
      if (((PtrElement) parent)->ElFirstChild != NULL)
	 if (((PtrElement) parent)->ElFirstChild->ElParent != (PtrElement) parent)
	   {
	      TtaError (ERR_incorrect_tree);
	   }
	 else
	   {
	      child = ((PtrElement) parent)->ElFirstChild;
	      while (child->ElNext != NULL)
		 child = child->ElNext;
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
#ifdef __STDC__
Element             TtaGetFirstLeaf (Element parent)
#else  /* __STDC__ */
Element             TtaGetFirstLeaf (parent)
Element             parent;
#endif /* __STDC__ */
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
#ifdef __STDC__
Element             TtaGetLastLeaf (Element parent)
#else  /* __STDC__ */
Element             TtaGetLastLeaf (parent)
Element             parent;
#endif /* __STDC__ */
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
#ifdef __STDC__
void                TtaPreviousSibling (Element * element)
#else  /* __STDC__ */
void                TtaPreviousSibling (element)
Element            *element;
#endif /* __STDC__ */
{
   PtrElement          sibling;

   UserErrorCode = 0;
   sibling = NULL;
   if ((PtrElement) (*element) == NULL)
     {
	TtaError (ERR_invalid_parameter);
     }
   else if (((PtrElement) (*element))->ElPrevious != NULL)
      if (((PtrElement) (*element))->ElPrevious->ElNext != (PtrElement) (*element))
	{
	   TtaError (ERR_incorrect_tree);
	}
      else
	 sibling = ((PtrElement) (*element))->ElPrevious;
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
#ifdef __STDC__
void                TtaNextSibling (Element * element)
#else  /* __STDC__ */
void                TtaNextSibling (element)
Element            *element;
#endif /* __STDC__ */
{
   PtrElement          sibling;

   UserErrorCode = 0;
   sibling = 0;
   if (*element == NULL)
     {
	TtaError (ERR_invalid_parameter);
     }
   else if (((PtrElement) (*element))->ElNext != NULL)
      if (((PtrElement) (*element))->ElNext->ElPrevious != (PtrElement) (*element))
	{
	   TtaError (ERR_incorrect_tree);
	}
      else
	 sibling = ((PtrElement) (*element))->ElNext;
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
#ifdef __STDC__
Element             TtaGetSuccessor (Element element)
#else  /* __STDC__ */
Element             TtaGetSuccessor (element)
Element             element;
#endif /* __STDC__ */
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
#ifdef __STDC__
Element             TtaGetPredecessor (Element element)
#else  /* __STDC__ */
Element             TtaGetPredecessor (element)
Element             element;
#endif /* __STDC__ */
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
#ifdef __STDC__
Element             TtaGetCommonAncestor (Element element1, Element element2)
#else  /* __STDC__ */
Element             TtaGetCommonAncestor (element1, element2)
Element             element1;
Element             element2;
#endif /* __STDC__ */
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
#ifdef __STDC__
Element             TtaGetTypedAncestor (Element element, ElementType ancestorType)
#else  /* __STDC__ */
Element             TtaGetTypedAncestor (element, ancestorType)
Element             element;
ElementType         ancestorType;
#endif /* __STDC__ */
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
   TtaIsExtensionElement

   Returns true is the element is from an extension schema

   Parameter:
   element: the element.

   Return value:
   true or false.
   ---------------------------------------------------------------------- */
#ifdef __STDC__
ThotBool            TtaIsExtensionElement (Element element)
#else  /* __STDC__ */
ThotBool            TtaIsExtensionElement (element)
Element             element;
#endif /* __STDC__ */
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
   TtaGetElementTypeName

   Returns the name of an element type.

   Parameter:
   elementType: element type.

   Return value:
   name of that type.
   ---------------------------------------------------------------------- */
#ifdef __STDC__
STRING              TtaGetElementTypeName (ElementType elementType)
#else  /* __STDC__ */
STRING              TtaGetElementTypeName (elementType)
ElementType         elementType;
#endif /* __STDC__ */
{

   UserErrorCode = 0;
   nameBuffer[0] = WC_EOS;
   if (elementType.ElSSchema == NULL)
	TtaError (ERR_invalid_parameter);
   else if (elementType.ElTypeNum > ((PtrSSchema) (elementType.ElSSchema))->SsNRules ||
	    elementType.ElTypeNum < 1)
	TtaError (ERR_invalid_element_type);
   else
	ustrncpy (nameBuffer, ((PtrSSchema) (elementType.ElSSchema))->SsRule[elementType.ElTypeNum - 1].SrName, MAX_NAME_LENGTH);
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
#ifdef __STDC__
STRING              TtaGetElementTypeOriginalName (ElementType elementType)
#else  /* __STDC__ */
STRING              TtaGetElementTypeOriginalName (elementType)
ElementType         elementType;
#endif /* __STDC__ */
{

   UserErrorCode = 0;
   nameBuffer[0] = WC_EOS;
   if (elementType.ElSSchema == NULL)
	TtaError (ERR_invalid_parameter);
   else if (elementType.ElTypeNum > ((PtrSSchema) (elementType.ElSSchema))->SsNRules ||
	    elementType.ElTypeNum < 1)
	TtaError (ERR_invalid_element_type);
   else
	ustrncpy (nameBuffer, ((PtrSSchema) (elementType.ElSSchema))->SsRule[elementType.ElTypeNum - 1].SrOrigName, MAX_NAME_LENGTH);
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
#ifdef __STDC__
void                TtaGiveTypeFromName (ElementType * elementType, STRING name)
#else  /* __STDC__ */
void                TtaGiveTypeFromName (elementType, name)
ElementType        *elementType;
STRING              name;
#endif /* __STDC__ */
{
   UserErrorCode = 0;
   (*elementType).ElTypeNum = 0;
   if (name == NULL || name[0] == EOS || (*elementType).ElSSchema == NULL)
     {
	(*elementType).ElSSchema = NULL;
	TtaError (ERR_invalid_parameter);
     }
   else
	GetSRuleFromName (&((*elementType).ElTypeNum), (PtrSSchema *) (&((*elementType).ElSSchema)), name, USER_NAME);
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
#ifdef __STDC__
void                TtaGiveTypeFromOriginalName (ElementType * elementType, STRING name)

#else  /* __STDC__ */
void                TtaGiveTypeFromOriginalName (elementType, name)
ElementType        *elementType;
STRING              name;

#endif /* __STDC__ */

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
#ifdef __STDC__
int                 TtaSameTypes (ElementType type1, ElementType type2)
#else  /* __STDC__ */
int                 TtaSameTypes (type1, type2)
ElementType         type1;
ElementType         type2;
#endif /* __STDC__ */
{
   int                 result;

   UserErrorCode = 0;
   result = 0;
   if (type1.ElTypeNum < 1 || type2.ElTypeNum < 1)
      TtaError (ERR_invalid_element_type);
   else if (type1.ElSSchema == NULL)
      if (type1.ElTypeNum > MAX_BASIC_TYPE)
	 TtaError (ERR_invalid_element_type);
      else if (type1.ElTypeNum == type2.ElTypeNum)
	 result = 1;
      else
	 result = 0;
   else if (type1.ElTypeNum > ((PtrSSchema) (type1.ElSSchema))->SsNRules)
      TtaError (ERR_invalid_element_type);
   else if (type1.ElTypeNum != type2.ElTypeNum)
      result = 0;
   else if (type2.ElSSchema == NULL)
      if (type2.ElTypeNum > MAX_BASIC_TYPE)
	 TtaError (ERR_invalid_element_type);
      else
	 result = 1;
   else if (type2.ElTypeNum > ((PtrSSchema) (type2.ElSSchema))->SsNRules)
      TtaError (ERR_invalid_element_type);
   else if (((PtrSSchema) (type1.ElSSchema))->SsCode ==
	    ((PtrSSchema) (type2.ElSSchema))->SsCode)
      result = 1;
   else
      result = 0;
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
#ifdef __STDC__
STRING              TtaGetElementLabel (Element element)
#else  /* __STDC__ */
STRING              TtaGetElementLabel (element)
Element             element;
#endif /* __STDC__ */
{

   UserErrorCode = 0;
   nameBuffer[0] = WC_EOS;
   if (element == NULL)
	TtaError (ERR_invalid_parameter);
   else 
#   ifdef _I18N_
   {
    CHAR_T wcsTmpStr [MAX_NAME_LENGTH];
    mbstowcs (wcsTmpStr, ((PtrElement) element)->ElLabel, MAX_NAME_LENGTH);
	ustrncpy (nameBuffer, wcsTmpStr, MAX_NAME_LENGTH);
   }
#   else  /* !_I18N_ */
	ustrncpy (nameBuffer, ((PtrElement) element)->ElLabel, MAX_NAME_LENGTH);
#   endif /* !_I18N_ */
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
#ifdef __STDC__
int                 TtaGetElementLineNumber (Element element)
#else  /* __STDC__ */
int                 TtaGetElementLineNumber (element)
Element             element;
#endif /* __STDC__ */
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
   TtaIsConstant

   Indicates whether an element type is a constant.
   Parameter:
   elementType: type to be tested.
   Return value:
   1 = the type is a constant, 0 = the type is not a constant.
   ---------------------------------------------------------------------- */
#ifdef __STDC__
int                 TtaIsConstant (ElementType elementType)
#else  /* __STDC__ */
int                 TtaIsConstant (elementType)
ElementType         elementType;
#endif /* __STDC__ */

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
	if (((PtrSSchema) (elementType.ElSSchema))->SsRule[elementType.ElTypeNum - 1].SrConstruct == CsConstant)
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
#ifdef __STDC__
int                 TtaIsLeaf (ElementType elementType)
#else  /* __STDC__ */
int                 TtaIsLeaf (elementType)
ElementType         elementType;
#endif /* __STDC__ */
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
	if ((((PtrSSchema) (elementType.ElSSchema))->SsRule[elementType.ElTypeNum - 1].SrConstruct == CsConstant) ||
	    (((PtrSSchema) (elementType.ElSSchema))->SsRule[elementType.ElTypeNum - 1].SrConstruct == CsReference) ||
	    (((PtrSSchema) (elementType.ElSSchema))->SsRule[elementType.ElTypeNum - 1].SrConstruct == CsPairedElement) ||
	    (((PtrSSchema) (elementType.ElSSchema))->SsRule[elementType.ElTypeNum - 1].SrConstruct == CsBasicElement))
	   result = 1;
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
#ifdef __STDC__
Construct           TtaGetConstructOfType (ElementType elementType)
#else  /* __STDC__ */
Construct           TtaGetConstructOfType (elementType)
ElementType         elementType;
#endif /* __STDC__ */
{
   Construct           result;

   UserErrorCode = 0;
   result = ConstructError;
   if (elementType.ElSSchema == NULL)
	TtaError (ERR_invalid_parameter);
   else if (elementType.ElTypeNum > ((PtrSSchema) (elementType.ElSSchema))->SsNRules ||
	    elementType.ElTypeNum < 1)
	TtaError (ERR_invalid_element_type);
   else
      switch (((PtrSSchema) (elementType.ElSSchema))->SsRule[elementType.ElTypeNum - 1].SrConstruct)
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

#ifdef __STDC__
int           TtaGetCardinalOfType (ElementType elementType)
#else  /* __STDC__ */
int           TtaGetCardinalOfType (elementType)
ElementType         elementType;

#endif /* __STDC__ */

{
   int           result;
   SRule        *pRule;

   UserErrorCode = 0;
   result = 0;
   if (elementType.ElSSchema == NULL)
	TtaError (ERR_invalid_parameter);
   else if (elementType.ElTypeNum > ((PtrSSchema) (elementType.ElSSchema))->SsNRules ||
	    elementType.ElTypeNum < 1)
	TtaError (ERR_invalid_element_type);
   else
     {
        pRule = &(((PtrSSchema) (elementType.ElSSchema))->SsRule[elementType.ElTypeNum - 1]);
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
		  result = pRule->SrNChoices;
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
#ifdef __STDC__
void           TtaGiveConstructorsOfType (ElementType **typesArray,int *size,ElementType elementType)
#else  /* __STDC__ */
void           TtaGiveConstructorsOfType (typesArray,size,elementType)
ElementType         elementType;
ElementType       **typesArray;
int                *size;
#endif /* __STDC__ */

{ 
   SRule        *pRule;
   int i;

   UserErrorCode = 0;
   if (elementType.ElSSchema == NULL)
	TtaError (ERR_invalid_parameter);
   else if (elementType.ElTypeNum > ((PtrSSchema) (elementType.ElSSchema))->SsNRules ||
	    elementType.ElTypeNum < 1)
	TtaError (ERR_invalid_element_type);
   else
     {
        pRule = &(((PtrSSchema) (elementType.ElSSchema))->SsRule[elementType.ElTypeNum - 1]);
        switch (pRule->SrConstruct)
          {
	   case CsNatureSchema:
		if(*size<1)
		    TtaError(ERR_buffer_too_small);
                else
                   {
                     ((*typesArray)[0]).ElSSchema = (SSchema)pRule->SrSSchemaNat;
                     ((*typesArray)[0]).ElTypeNum = 0;
                     *size = 1;
                   }
                break;
	   case CsBasicElement:
		if(*size<1)
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
		if(*size<1)
		    TtaError(ERR_buffer_too_small);
                else
                   {
                    ((*typesArray)[0]).ElSSchema = elementType.ElSSchema;
                    ((*typesArray)[0]).ElTypeNum = pRule->SrIdentRule;
                    *size = 1;
                   }
                 break;
	   case CsList:
		if(*size<1)
		    TtaError(ERR_buffer_too_small);
                else
                   {
                    ((*typesArray)[0]).ElSSchema = elementType.ElSSchema;
                    ((*typesArray)[0]).ElTypeNum = pRule->SrListItem;
                    *size = 1;
                   }
                 break;
	   case CsChoice:
		if(*size<pRule->SrNChoices)
                   {
		    TtaError(ERR_buffer_too_small);
                    *size = 0;
                   }
                else
                   {
                    for (i=0; i< pRule->SrNChoices;i++)
                      {
                        ((*typesArray)[i]).ElSSchema = elementType.ElSSchema;
                        ((*typesArray)[i]).ElTypeNum = pRule->SrChoice[i];
                      }
                    *size = i;
                   }
                break;
           case CsAggregate:
           case CsUnorderedAggregate:
                if(*size<pRule->SrNComponents)
                   {
		    TtaError(ERR_buffer_too_small);
                    *size = 0;
                   }
                else
                   {
                    for (i=0; i< pRule->SrNComponents;i++)
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
#ifdef __STDC__
int           TtaGetRankInAggregate (ElementType componentType, ElementType aggregateType)
#else  /* __STDC__ */
int           TtaGetRankInAggregate (componentType, aggregateType)
ElementType 	    componentType;
ElementType         aggregateType;
#endif /* __STDC__ */
{
   int		rank, i;
   SRule        *pRule;

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
        pRule = &(((PtrSSchema) (aggregateType.ElSSchema))->SsRule[aggregateType.ElTypeNum - 1]);
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
#ifdef __STDC__
ThotBool          TtaIsOptionalInAggregate (int rank, ElementType elementType)
#else  /* __STDC__ */
ThotBool          TtaIsOptionalInAggregate (rank, elementType)
int 		    rank;
ElementType         elementType;
#endif /* __STDC__ */
{
   ThotBool result;
   SRule        *pRule;

   UserErrorCode = 0;
   result = FALSE;
   if (elementType.ElSSchema == NULL)
	TtaError (ERR_invalid_parameter);
   else if (elementType.ElTypeNum > ((PtrSSchema) (elementType.ElSSchema))->SsNRules ||
	    elementType.ElTypeNum < 1)
	TtaError (ERR_invalid_element_type);
   else
     {
        pRule = &(((PtrSSchema) (elementType.ElSSchema))->SsRule[elementType.ElTypeNum - 1]);
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
#ifdef __STDC__
Construct           TtaGetConstruct (Element element)
#else  /* __STDC__ */
Construct           TtaGetConstruct (element)
Element             element;
#endif /* __STDC__ */

{
   Construct           result;

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

#ifdef __STDC__
AccessRight         TtaGetAccessRight (Element element)

#else  /* __STDC__ */
AccessRight         TtaGetAccessRight (element)
Element             element;

#endif /* __STDC__ */

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
		 case AccessReadOnly:
		    right = ReadOnly;
		    break;
		 case AccessReadWrite:
		    right = ReadWrite;
		    break;
		 case AccessHidden:
		    right = Hidden;
		    break;
		 case AccessInherited:
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

#ifdef __STDC__
int                 TtaIsHolophrasted (Element element)

#else  /* __STDC__ */
int                 TtaIsHolophrasted (element)
Element             element;

#endif /* __STDC__ */

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

#ifdef __STDC__
int                 TtaIsReadOnly (Element element)

#else  /* __STDC__ */
int                 TtaIsReadOnly (element)
Element             element;

#endif /* __STDC__ */

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
   TtaIsHidden

   Tests whether a given element is hidden to the user.

   Parameter:
   element: the element to be tested.

   Return Value:
   1 if the element is hidden, 0 if not.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
int                 TtaIsHidden (Element element)

#else  /* __STDC__ */
int                 TtaIsHidden (element)
Element             element;

#endif /* __STDC__ */

{
   int                 result;

   UserErrorCode = 0;
   result = 0;
   if (element == NULL)
	TtaError (ERR_invalid_parameter);
   else
     {
	if (ElementIsHidden ((PtrElement) element))
	   result = 1;
     }
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

#ifdef __STDC__
int                 TtaIsInAnInclusion (Element element)

#else  /* __STDC__ */
int                 TtaIsInAnInclusion (element)
Element             element;

#endif /* __STDC__ */

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

#ifdef __STDC__
int                 TtaIsAncestor (Element element, Element ancestor)

#else  /* __STDC__ */
int                 TtaIsAncestor (element, ancestor)
Element             element;
Element             ancestor;

#endif /* __STDC__ */

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

#ifdef __STDC__
int                 TtaIsBefore (Element element1, Element element2)

#else  /* __STDC__ */
int                 TtaIsBefore (element1, element2)
Element             element1;
Element             element2;

#endif /* __STDC__ */

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

#ifdef __STDC__
int                 TtaIsFirstPairedElement (Element element)

#else  /* __STDC__ */
int                 TtaIsFirstPairedElement (element)
Element             element;

#endif /* __STDC__ */

{
   int                 result;

   UserErrorCode = 0;
   result = 0;
   if (element == NULL)
	TtaError (ERR_invalid_parameter);
   else if (((PtrElement) element)->ElStructSchema->SsRule[((PtrElement) element)->ElTypeNumber - 1].SrConstruct != CsPairedElement)
     {
	TtaError (ERR_invalid_element_type);
     }
   else if (((PtrElement) element)->ElStructSchema->SsRule[((PtrElement) element)->ElTypeNumber - 1].SrFirstOfPair)
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

#ifdef __STDC__
ThotBool            TtaCanInsertSibling (ElementType elementType, Element sibling, ThotBool before, Document document)

#else  /* __STDC__ */
ThotBool            TtaCanInsertSibling (elementType, sibling, before, document)
ElementType         elementType;
Element             sibling;
ThotBool            before;
Document            document;

#endif /* __STDC__ */

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
#ifdef __STDC__
ThotBool            TtaCanInsertFirstChild (ElementType elementType, Element parent, Document document)
#else  /* __STDC__ */
ThotBool            TtaCanInsertFirstChild (elementType, parent, document)
ElementType         elementType;
Element             parent;
Document            document;
#endif /* __STDC__ */
{
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
      ret = AllowedFirstChild ((PtrElement) parent, LoadedDocument[document - 1],
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
#ifdef __STDC__
void                TtaNextCopiedElement (Element * element)
#else  /* __STDC__ */
void                TtaNextCopiedElement (element)
Element            *element;
#endif /* __STDC__ */
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
Document            TtaGetCopiedDocument ()
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
#ifdef __STDC__
static PtrElement   SearchLabel (STRING label, PtrElement pEl)
#else  /* __STDC__ */
static PtrElement   SearchLabel (label, pEl)
STRING              label;
PtrElement          pEl;
#endif /* __STDC__ */
{
   PtrElement          pE, pFound;
#  ifdef _I18N_
   char                mbsLabel[MAX_LENGTH];
#  else  /* !_I18N_ */
   char*               mbsLabel = label;
#  endif /* !_I18N_ */
   
#  ifdef _I18N_
   wcstombs (mbsLabel, label, MAX_LENGTH);
#  endif /* _I18N_ */

   pFound = NULL;
   if (strcmp (mbsLabel, pEl->ElLabel) == 0)
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
#ifdef __STDC__
Element             TtaSearchElementByLabel (STRING searchedLabel, Element element)
#else  /* __STDC__ */
Element             TtaSearchElementByLabel (searchedLabel, element)
STRING              searchedLabel;
Element             element;
#endif /* __STDC__ */
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
#ifdef __STDC__
Element             TtaSearchEmptyElement (SearchDomain scope, Element element)
#else  /* __STDC__ */
Element             TtaSearchEmptyElement (scope, element)
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
#ifdef __STDC__
Element             TtaSearchOtherPairedElement (Element element)
#else  /* __STDC__ */
Element             TtaSearchOtherPairedElement (element)
Element             element;
#endif /* __STDC__ */
{
   PtrElement          pairedElement;

   UserErrorCode = 0;
   pairedElement = NULL;
   if (element == NULL)
	TtaError (ERR_invalid_parameter);
   else if (((PtrElement) element)->ElStructSchema->SsRule[((PtrElement) element)->ElTypeNumber - 1].SrConstruct != CsPairedElement)
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
#ifdef __STDC__
Element             TtaSearchNoPageBreak (Element element, ThotBool forward)
#else  /* __STDC__ */
Element             TtaSearchNoPageBreak (element, forward)
Element             element;
ThotBool            forward;
#endif /* __STDC__ */
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
