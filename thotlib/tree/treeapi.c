
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */


#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "application.h"
#include "document.h"
#include "tree.h"
#include "view.h"
#include "storage.h"
#include "typecorr.h"
#include "appdialogue.h"

#undef EXPORT
#define EXPORT extern
#include "edit.var"
#include "modif.var"
#include "environ.var"
#include "appdialogue.var"

#include "tree_f.h"
#include "attributes_f.h"
#include "attributeapi_f.h"
#include "structcreation_f.h"
#include "structmodif_f.h"
#include "references_f.h"
#include "externalref_f.h"
#include "structschema_f.h"
#include "thotmsg_f.h"

extern int          UserErrorCode;
static Name          nameBuffer;
int                 AvecControleStruct = 1;

#ifdef __STDC__
extern void         CreateNewElement (int, PtrSSchema, PtrDocument, boolean);

#else  /* __STDC__ */
extern void         CreateNewElement ();

#endif /* __STDC__ */


/* SetAssocNumber       assigne le numero d'element associe' nAssoc
   a tout le sous-arbre de racine pEl.  */

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
      if (elementType.ElTypeNum < 1 ||
	  elementType.ElTypeNum > ((PtrSSchema) (elementType.ElSSchema))->SsNRules)
     {
	TtaError (ERR_invalid_element_type);
     }
   else
     {
	element = NewSubtree (elementType.ElTypeNum, (PtrSSchema) (elementType.ElSSchema),
	     TabDocuments[document - 1], 0, FALSE, TRUE, TRUE, TRUE);
	if (element->ElSructSchema->SsRule[element->ElTypeNumber - 1].SrConstruct == CsPairedElement)
	   if (!element->ElSructSchema->SsRule[element->ElTypeNumber - 1].SrFirstOfPair)
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
   label: label of the root element to be created. PcEmpty string if the value
   of the label is undefined.

   Return value:
   the root element of the created tree.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
Element             TtaNewTree (Document document, ElementType elementType, char *label)

#else  /* __STDC__ */
Element             TtaNewTree (document, elementType, label)
Document            document;
ElementType         elementType;
char               *label;

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
      if (elementType.ElTypeNum < 1 ||
	  elementType.ElTypeNum > ((PtrSSchema) (elementType.ElSSchema))->SsNRules)
     {
	TtaError (ERR_invalid_element_type);
     }
   else
     {
	element = NewSubtree (elementType.ElTypeNum, (PtrSSchema) (elementType.ElSSchema),
		     TabDocuments[document - 1], 0, TRUE, TRUE, TRUE,
			      (*label) == '\0');
	if (element->ElSructSchema->SsRule[element->ElTypeNumber - 1].SrConstruct == CsPairedElement)
	   if (!element->ElSructSchema->SsRule[element->ElTypeNumber - 1].SrFirstOfPair)
	      element->ElPairIdent = 0;
	if (*label != '\0')
	   strncpy (element->ElLabel, label, MAX_LABEL_LEN);
     }
   return ((Element) element);
}

/* ---------------------------------------------------------------------- */
/* |    TransRef cherche dans le sous-arbre de racine pElem tous les    | */
/* |    elements reference's et transfert sur eux les references qui    | */
/* |    sont dans l'arbre de racine pRac.                               | */
/* |    affecte aussi un nouveau label a tous les elements du           | */
/* |    sous-arbre de racine pElem.                                     | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         TransRef (PtrElement pElem, PtrElement pRac, PtrDocument pDoc)

#else  /* __STDC__ */
static void         TransRef (pElem, pRac, pDoc)
PtrElement          pElem;
PtrElement          pRac;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrElement          pFils;
   PtrReferredDescr    pDescRef;
   PtrReference        pRef, pRefSuiv;
   PtrReference        pPR1;

   /* affecte un nouveau label a l'element */
   LabelIntToString (NewLabel (pDoc), pElem->ElLabel);
   if (pElem->ElReferredDescr != NULL)
      /* cet element est reference'. CopyTree n'a pas copie' son */
      /* descripteur d'element reference', qui est encore partage' avec */
      /* celui de l'element source */
      /* traite les references a l'element source qui sont dans le */
      /* sous-arbre de racine pRac */
     {
	pDescRef = pElem->ElReferredDescr;
	/* descripteur d'element reference' */
	pElem->ElReferredDescr = NULL;
	/* reste attache' seulement a l'element source */
	pRef = pDescRef->ReFirstReference;
	/* 1ere reference a l'element source */
	while (pRef != NULL)
	   /* parcourt les references a l'element source */
	  {
	     pRefSuiv = pRef->RdNext;	/* prepare la reference suivante */
	     if (ElemIsWithinSubtree (pRef->RdElement, pRac))
		/* la reference est dans le sous-arbre de racine pRac, on la */
		/* fait pointer vers l'element traite' (pElem) */
	       {
		  if (pElem->ElReferredDescr == NULL)
		     /* l'element n'a pas de descripteur d'element reference', */
		     /* on lui en affecte un */
		    {
		       pElem->ElReferredDescr = NewReferredElDescr (pDoc);
		       if (!pElem->ElReferredDescr->ReExternalRef)
			  pElem->ElReferredDescr->ReReferredElem = pElem;
		    }
		  strncpy (pElem->ElReferredDescr->ReReferredLabel, pElem->ElLabel, MAX_LABEL_LEN);
		  /* lie le descripteur de reference et le descripteur */
		  /* d'element reference' de l'element traite' */
		  pPR1 = pRef;
		  /* dechaine le descripteur de la chaine des references a */
		  /* l'element source */
		  if (pPR1->RdNext != NULL)
		     pPR1->RdNext->RdPrevious = pPR1->RdPrevious;
		  if (pPR1->RdPrevious == NULL)
		     pDescRef->ReFirstReference = pPR1->RdNext;
		  else
		     pPR1->RdPrevious->RdNext = pPR1->RdNext;
		  /* le chaine en tete de la liste des references a */
		  /* l'element traite' */
		  pPR1->RdReferred = pElem->ElReferredDescr;
		  pPR1->RdNext = pPR1->RdReferred->ReFirstReference;
		  if (pPR1->RdNext != NULL)
		     pPR1->RdNext->RdPrevious = pRef;
		  pPR1->RdReferred->ReFirstReference = pRef;
		  pPR1->RdPrevious = NULL;
	       }
	     pRef = pRefSuiv;
	     /* passe a la reference suivante */
	  }
     }
   if (!pElem->ElTerminal)
      /* element non terminal, on traite tous ses fils */
     {
	pFils = pElem->ElFirstChild;
	while (pFils != NULL)
	  {
	     TransRef (pFils, pRac, pDoc);
	     pFils = pFils->ElNext;
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
   PtrSSchema        pSS;

   UserErrorCode = 0;
   element = NULL;
   /* verifie les parametres Document */
   if (sourceDocument < 1 || sourceDocument > MAX_DOCUMENTS ||
       destinationDocument < 1 || destinationDocument > MAX_DOCUMENTS)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else if (TabDocuments[sourceDocument - 1] == NULL ||
	    TabDocuments[destinationDocument - 1] == NULL)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else
      /* parametres Document corrects */
   if (sourceElement == NULL)
     {
	TtaError (ERR_invalid_parameter);
     }
   else
     {
	/* cherche le schema de structure a utiliser pour la copie */
	if (sourceDocument == destinationDocument)
	   pSS = ((PtrElement) sourceElement)->ElSructSchema;
	else
	  {
	     pSS = NULL;
	     ancestor = (PtrElement) parent;
	     while (pSS == NULL && ancestor != NULL)
		if (ancestor->ElSructSchema->SsCode ==
		    ((PtrElement) sourceElement)->ElSructSchema->SsCode)
		   pSS = ancestor->ElSructSchema;
		else
		   ancestor = ancestor->ElParent;
	     if (pSS == NULL)
		if (TabDocuments[destinationDocument - 1]->DocSSchema->SsCode ==
		    ((PtrElement) sourceElement)->ElSructSchema->SsCode)
		   pSS = TabDocuments[destinationDocument - 1]->DocSSchema;
		else if (((PtrElement) sourceElement)->ElTerminal)
		   pSS = ((PtrElement) parent)->ElSructSchema;
		else
		   pSS = ((PtrElement) sourceElement)->ElSructSchema;
	  }
	/* effectue la copie */
	element = CopyTree (((PtrElement) sourceElement),
			      TabDocuments[sourceDocument - 1], 0, pSS,
			      TabDocuments[destinationDocument - 1],
			      (PtrElement) parent,
			      TRUE, TRUE);
	TransRef (element, element, TabDocuments[destinationDocument - 1]);
     }
   return ((Element) element);
}


/** ---------------------------------------------------------------------- *
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
*** ---------------------------------------------------------------------- */

#ifdef __STDC__
static Element      CreateDescent (Document document, Element element, ElementType elementType, boolean withContent)

#else  /* __STDC__ */
static Element      CreateDescent (document, element, elementType, withContent)
Document            document;
Element             element;
ElementType         elementType;
boolean             withContent;

#endif /* __STDC__ */

{
   PtrElement          lastCreated;
   PtrElement          firstCreated;
   PtrElement          pEl;
   PtrElement          pFils;
   PtrElement          pFrere;
   PtrElement          pE;
   PtrElement          pSuiv;
   PtrElement          lastNew;

#ifndef NODISPLAY
   PtrElement          pVoisin;

#endif
   boolean             ident;
   boolean             ok;

   UserErrorCode = 0;
   lastCreated = NULL;
   if (element == NULL)
     {
	TtaError (ERR_invalid_parameter);
     }
   else if (elementType.ElSSchema == NULL)
     {
	TtaError (ERR_invalid_parameter);
     }
   else
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
      if (elementType.ElTypeNum < 1 ||
	  elementType.ElTypeNum > ((PtrSSchema) (elementType.ElSSchema))->SsNRules)
     {
	TtaError (ERR_invalid_element_type);
     }
   else
     {
	pEl = (PtrElement) element;
	firstCreated = NULL;
	/* on ne cree pas de descendance pour un element terminal */
	if (!pEl->ElTerminal)
	   /* on ne cree pas de descendance dans une copie */
	   if (!pEl->ElIsCopy)
	      firstCreated = CreateDescendant (pEl->ElTypeNumber, pEl->ElSructSchema,
		  TabDocuments[document - 1], &lastCreated, pEl->ElAssocNum,
					  elementType.ElTypeNum,
				    (PtrSSchema) (elementType.ElSSchema));
	if (firstCreated != NULL)
	  {
	     if (firstCreated->ElNext != NULL &&
		 pEl->ElFirstChild != NULL)
		/* on a cree' plusieurs arbres, on ne garde que celui */
		/* qui contient l'element qu'on voulait creer */
	       {
		  /* cherche la racine de l'arbre a garder */
		  pE = lastCreated;
		  while (pE->ElParent != NULL)
		     pE = pE->ElParent;
		  pFrere = firstCreated;
		  firstCreated = pE;
		  /* detruit les autres arbres */
		  while (pFrere != NULL)
		    {
		       pSuiv = pFrere->ElNext;
		       if (pFrere != firstCreated)
			  DeleteElement (&pFrere);
		       pFrere = pSuiv;
		    }
	       }
	     lastNew = firstCreated;
	     while (lastNew->ElNext != NULL)
		lastNew = lastNew->ElNext;
	     if (pEl->ElSructSchema->SsRule[pEl->ElTypeNumber - 1].SrConstruct == CsChoice)
	       {
		  if (lastCreated == firstCreated)
		     ident = TRUE;
		  else
		     ident = FALSE;
#ifndef NODISPLAY
		  InsertOption (pEl, &firstCreated, TabDocuments[document - 1]);
#else
		  InsertElemInChoice (pEl, &firstCreated, FALSE);
#endif
		  /* chaine l'element cree */
		  if (pEl == firstCreated && ident)
		     lastCreated = pEl;
	       }
	     else
	       {
		  if (AllowedFirstChild (pEl, TabDocuments[document - 1],
			    firstCreated->ElTypeNumber, firstCreated->ElSructSchema,
					   FALSE, FALSE))
		    {
		       pFils = pEl->ElFirstChild;
		       if (pFils != NULL)
			  if (pFils->ElTerminal &&
			      pFils->ElLeafType == LtPageColBreak)
			     /* saute les marques de page qui sont en tete */
			     SkipPageBreakBegin (&pFils);
			  else
			     pFils = NULL;
		       if (pFils != NULL)
			  /* il y a des marques de page, on insere l'element */
			  /* apres ces marques */
			  InsertElementAfter (pFils, firstCreated);
		       else
			  InsertFirstChild (pEl, firstCreated);
		    }
		  else
		    {
		       pFils = pEl->ElFirstChild;
		       ok = FALSE;
		       while (pFils != NULL && !ok)
			 {
			    if (AllowedSibling (pFils, TabDocuments[document - 1],
			    firstCreated->ElTypeNumber, firstCreated->ElSructSchema,
						FALSE, FALSE, FALSE))
			       /* notre arbre peut se placer apres pFils */
			       if (pFils->ElNext == NULL)
				  /* pFils n'a pas de frere suivant, OK */
				  ok = TRUE;
			       else
				  /* on verifie que notre arbre peut se placer */
				  /* devant le frere suivant de pFils */
				  ok = AllowedSibling (pFils->ElNext,
						 TabDocuments[document - 1],
						       firstCreated->ElTypeNumber,
						  firstCreated->ElSructSchema,
						       TRUE, FALSE, FALSE);
			    if (!ok)
			       pFils = pFils->ElNext;
			 }
		       if (pFils == NULL)
			 {
			    lastCreated = NULL;
			    DeleteElement (&firstCreated);
			 }
		       else
			  InsertElementAfter (pFils, firstCreated);
		    }
	       }
	     if (firstCreated != NULL)
	       {
#ifndef NODISPLAY
		  pVoisin = firstCreated->ElNext;
		  /* saute les marques de page qui suivent */
		  FwdSkipPageBreak (&pVoisin);
#endif
		  if (withContent)
		     if (!lastCreated->ElTerminal)
			if (lastCreated->ElSructSchema->SsRule[lastCreated->ElTypeNumber - 1].SrConstruct != CsChoice)
			  {
			     pFils = NewSubtree (lastCreated->ElTypeNumber, lastCreated->ElSructSchema, TabDocuments[document - 1], lastCreated->ElAssocNum, TRUE, FALSE, TRUE, TRUE);
			     if (pFils != NULL)
				InsertFirstChild (lastCreated, pFils);
			  }
		  pE = firstCreated;
		  while (pE != NULL)
		    {
		       if (pE == lastNew)
			  pSuiv = NULL;
		       else
			  pSuiv = pE->ElNext;
		       RemoveExcludedElem (&pE);
		       if (pE != NULL)
			 {
			    /* traitement des exceptions */
			    TraiteExceptionCreation (pE, TabDocuments[document - 1]);
#ifndef NODISPLAY
			    /* traite les attributs requis des elements crees */
			    AttachMandatoryAttributes (pE, TabDocuments[document - 1]);
			    RedispNewElement (document, pE, pVoisin,
					      TRUE, TRUE);
#endif
			 }
		       pE = pSuiv;
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
Element             TtaCreateDescentWithContent (document, element, elementType)
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
   boolean             racine;
   PtrElement          pEl;

   UserErrorCode = 0;
   if (element == NULL)
     {
	TtaError (ERR_invalid_parameter);
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
	     pEl = (PtrElement) element;
	     RegisterExternalRef (pEl, pDoc, FALSE);
	     RegisterDeletedReferredElem (pEl, pDoc);
	     racine = (pEl->ElParent == NULL);
#ifndef NODISPLAY
	     UndisplayElement (pEl, document);
#endif
	     if (racine)
		if (pDoc->DocRootElement == pEl)
		   /* c'est tout l'arbre principal qu'on detruit */
		   pDoc->DocRootElement = NULL;
		else if (pDoc->DocAssocRoot[pEl->ElAssocNum - 1] == pEl)
		   /* c'est la racine d'un arbre associe' */
		   pDoc->DocAssocRoot[pEl->ElAssocNum - 1] = NULL;
	     DeleteElement (&pEl);
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
   int                 numAssoc, numAssocLibre;
   SRule              *pRegle;
   boolean             trouve;
   boolean             ok;
   PtrElement          pRoot;
   PtrSSchema        curExtension;

   UserErrorCode = 0;
   numAssocLibre = 0;
   pRoot = (PtrElement) tree;
   ok = FALSE;
   if (tree == NULL)
     {
	TtaError (ERR_invalid_parameter);
     }
   else
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
	/* verifie que le type de l'arbre est defini dans le schema */
	/* du document ou dans l'une de ses extensions */
	trouve = FALSE;
	if (pDoc->DocSSchema->SsCode == pRoot->ElSructSchema->SsCode)
	   /* schema du document */
	   trouve = TRUE;
	else if (pRoot->ElSructSchema->SsExtension)
	   /* l'arbre est defini par une extension de schema */
	   /* Est-ce une extension de ce document ? */
	  {
	     curExtension = pDoc->DocSSchema->SsNextExtens;
	     while (!trouve && curExtension != NULL)
		if (pRoot->ElSructSchema->SsCode == curExtension->SsCode)
		   trouve = TRUE;
		else
		   curExtension = curExtension->SsNextExtens;
	  }
	if (!trouve)
	   /* l'arbre n'est pas defini dans un schema de structure */
	   /* du document */
	  {
	     TtaError (ERR_element_does_not_match_DTD);
	  }
	else
	  {
	     if (pRoot->ElTypeNumber == pRoot->ElSructSchema->SsRootElem)
		/* c'est l'arbre principal */
	       {
#ifndef NODISPLAY
		  if (pDoc->DocRootElement != NULL)
		     UndisplayElement (pDoc->DocRootElement, document);
#endif
		  DeleteElement (&pDoc->DocRootElement);
		  pDoc->DocRootElement = pRoot;
		  numAssocLibre = 0;
		  ok = TRUE;
	       }
	     else
		/* est-ce un arbre associe' correct ? */
	     if (pRoot->ElTerminal)
	       {
		  TtaError (ERR_element_does_not_match_DTD);
	       }
	     else
	       {
		  /* on accede a la regle qui definit la racine de l'arbre */
		  pRegle = &pRoot->ElSructSchema->SsRule[pRoot->ElTypeNumber - 1];
		  if (pRegle->SrConstruct != CsList)
		     /* ce n'est pas une liste, erreur */
		    {
		       TtaError (ERR_element_does_not_match_DTD);
		    }
		  else
		     /* c'est bien une liste */
		    {
		       /* on accede a la regle qui definit les elements de */
		       /* la liste */
		       pRegle = &pRoot->ElSructSchema->SsRule[pRegle->SrListItem - 1];
		       if (!pRegle->SrAssocElem)
			  /* les elements de la liste ne sont pas des */
			  /* elements associes, erreur */
			 {
			    TtaError (ERR_element_does_not_match_DTD);
			 }
		       else
			  ok = TRUE;
		    }
		  if (ok)
		    {
		       /* cherche s'il existe deja dans le document des */
		       /* elements asssocies de ce type */
		       numAssocLibre = 0;
		       numAssoc = 1;
		       trouve = FALSE;
		       while (!trouve && numAssoc <= MAX_ASSOC_DOC)
			 {
			    if (pDoc->DocAssocRoot[numAssoc - 1] == NULL)
			      {
				 if (numAssocLibre == 0)
				    numAssocLibre = numAssoc;
			      }
			    else if (pRoot->ElTypeNumber == pDoc->DocAssocRoot[numAssoc - 1]->ElTypeNumber)
			       trouve = TRUE;
			    if (!trouve)
			       numAssoc++;
			 }
		       if (trouve)
			  /* il existe deja des elements associes de ce type */
			 {
#ifndef NODISPLAY
			    if (pDoc->DocAssocRoot[numAssoc - 1] != NULL)
			       UndisplayElement (pDoc->DocAssocRoot[numAssoc - 1],
						 document);
#endif
			    DeleteElement (&pDoc->DocAssocRoot[numAssoc - 1]);
			    if (numAssocLibre == 0)
			       numAssocLibre = numAssoc;
			 }
		       if (numAssocLibre == 0)
			  /* tous les elements associes du document sont pris */
			 {
			    TtaError (ERR_invalid_parameter);
			    ok = FALSE;
			 }
		       else
			  pDoc->DocAssocRoot[numAssocLibre - 1] = pRoot;
		    }
	       }
	     /* change le numero d'element associe' de tout l'arbre */
	     if (ok)
	       {
		  pRoot->ElAccess = AccessReadWrite;
		  SetAssocNumber (pRoot, numAssocLibre);
		  /* verifie que la nouvelle racine porte un attribut Langue */
		  CheckLanguageAttr (pDoc, pRoot);
#ifndef NODISPLAY
		  RedispNewElement (document, pRoot, NULL, TRUE, TRUE);
#endif
	       }
	  }
     }
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
void                TtaInsertSibling (Element newElement, Element sibling, boolean before, Document document)

#else  /* __STDC__ */
void                TtaInsertSibling (newElement, sibling, before, document)
Element             newElement;
Element             sibling;
boolean             before;
Document            document;

#endif /* __STDC__ */

{
#ifndef NODISPLAY
   PtrElement          pVoisin;

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
      if (AvecControleStruct && !AllowedSibling ((PtrElement) sibling,
						 TabDocuments[document - 1],
					  ((PtrElement) newElement)->ElTypeNumber,
				     ((PtrElement) newElement)->ElSructSchema,
						 before, FALSE, FALSE))
     {
	TtaError (ERR_element_does_not_match_DTD);
     }
   else
     {
	if (before)
	  {
#ifndef NODISPLAY
	     pVoisin = ((PtrElement) sibling)->ElPrevious;
	     BackSkipPageBreak (&pVoisin);
#endif
	     InsertElementBefore ((PtrElement) sibling, (PtrElement) newElement);
	  }
	else
	  {
#ifndef NODISPLAY
	     pVoisin = ((PtrElement) sibling)->ElNext;
	     FwdSkipPageBreak (&pVoisin);
#endif
	     InsertElementAfter ((PtrElement) sibling, (PtrElement) newElement);
	  }
	/* met a jour le numero d'element associe' */
	SetAssocNumber ((PtrElement) newElement,
			((PtrElement) sibling)->ElAssocNum);
	/* Traite les exclusions dans l'element cree */
	pEl = (PtrElement) newElement;
	if (AvecControleStruct)
	   RemoveExcludedElem (&pEl);
	if (pEl != NULL)
	  {
	     /* traitement des exceptions */
	     TraiteExceptionCreation (pEl,
				      TabDocuments[document - 1]);
	     /* s'il s'agit d'un element de paire, etablit le */
	     /* chainage avec son homologue */
	     if (((PtrElement) newElement)->ElSructSchema->SsRule[((PtrElement) newElement)->ElTypeNumber - 1].SrConstruct == CsPairedElement)
		GetOtherPairedElement ((PtrElement) newElement);
#ifndef NODISPLAY
	     /* traite les attributs requis des elements crees */
	     if (AvecControleStruct)
		AttachMandatoryAttributes (pEl, TabDocuments[document - 1]);
	     if (pVoisin != NULL)
		/* l'element qui vient d'etre insere' n'est ni le */
		/* premier ni le dernier parmi ses freres */
		sibling = NULL;
	     RedispNewElement (document, pEl,
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
   PtrElement          pVoisin;

#endif
   boolean             ok;
   PtrElement          pFils;

   UserErrorCode = 0;
   ok = FALSE;
   if (*newElement == NULL || parent == NULL)
      TtaError (ERR_invalid_parameter);
   else if (((PtrElement) (*newElement))->ElParent != NULL)
      TtaError (ERR_element_already_inserted);
   /* verifie le parametre document */
   else if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   /* parametre document correct ? */
   else if (TabDocuments[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else if (((PtrElement) parent)->ElTerminal)
      TtaError (ERR_element_does_not_match_DTD);
   else
     {

	if (((PtrElement) parent)->ElSructSchema->SsRule[((PtrElement) parent)->ElTypeNumber - 1].SrConstruct == CsChoice)
	  {
	     ((PtrElement) (*newElement))->ElAssocNum = ((PtrElement) parent)->ElAssocNum;
#ifndef NODISPLAY
	     InsertOption ((PtrElement) parent, (PtrElement *) newElement,
			  TabDocuments[document - 1]);
#else
	     InsertElemInChoice ((PtrElement) parent, (PtrElement *) newElement, FALSE);
#endif
	     ok = TRUE;
	  }
	else if (AvecControleStruct
		 && !AllowedFirstChild ((PtrElement) parent, TabDocuments[document - 1],
					  ((PtrElement) (*newElement))->ElTypeNumber, ((PtrElement) (*newElement))->ElSructSchema, FALSE, FALSE))
	   TtaError (ERR_element_does_not_match_DTD);
	else
	  {
	     pFils = ((PtrElement) parent)->ElFirstChild;
	     if (pFils != NULL)
		if (pFils->ElTerminal && pFils->ElLeafType == LtPageColBreak)
		   /* saute les marques de page qui sont en tete */
		   SkipPageBreakBegin (&pFils);
		else
		   pFils = NULL;
	     if (pFils != NULL)
		/* il y a des marques de page, on insere l'element */
		/* apres ces marques */
		InsertElementAfter (pFils, (PtrElement) (*newElement));
	     else
		InsertFirstChild ((PtrElement) parent, (PtrElement) (*newElement));
	     ok = TRUE;
	  }

	if (ok)
	  {
#ifndef NODISPLAY
	     pVoisin = ((PtrElement) (*newElement))->ElNext;
	     /* saute les marques de page qui suivent */
	     FwdSkipPageBreak (&pVoisin);
#endif
	     /* met a jour le numero d'element associe' */
	     SetAssocNumber ((PtrElement) (*newElement),
			     ((PtrElement) parent)->ElAssocNum);
	     /* Traite les exclusions dans l'element cree */
	     if (AvecControleStruct)
		RemoveExcludedElem ((PtrElement *) newElement);
	     if ((PtrElement) (*newElement) != NULL)
	       {
		  /* traitement des exceptions */
		  TraiteExceptionCreation ((PtrElement) (*newElement),
					   TabDocuments[document - 1]);
#ifndef NODISPLAY
		  /* traite les attributs requis des elements crees */
		  if (AvecControleStruct)
		     AttachMandatoryAttributes ((PtrElement) (*newElement), TabDocuments[document - 1]);
		  RedispNewElement (document, (PtrElement) (*newElement),
				    pVoisin, TRUE, TRUE);
#endif
	       }
	  }
     }
}

#ifndef NODISPLAY

/* ----------------------------------------------------------------------
   TtaCreateElement

   Create an element of a given type and insert it at the current position within
   a given document. The current position is defined by the current selection.
   If the current position is a single position (insertion point) the new element
   is simply inserted at that position. If one or several characters and/or
   elements are selected, the new element is created at that position and the
   selected characters/elements become the content of the new element, provided
   the  structure schema allows it.

   Parameters:
   elementType: type of the element to be created.
   document: the document for which the element is created.

   ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtaCreateElement (ElementType elementType, Document document)
#else  /* __STDC__ */
void                TtaCreateElement (elementType, document)
ElementType         elementType;
Document            document;

#endif /* __STDC__ */
{
   UserErrorCode = 0;
   if (elementType.ElSSchema == NULL)
      TtaError (ERR_invalid_parameter);
   else if (document < 1 || document > MAX_DOCUMENTS)
      /* verifie le parametre document */
      TtaError (ERR_invalid_document_parameter);
   else if (TabDocuments[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else if (elementType.ElTypeNum < 1 ||
	    elementType.ElTypeNum > ((PtrSSchema) (elementType.ElSSchema))->SsNRules)
      /* parametre document correct */
      TtaError (ERR_invalid_element_type);
   else
      CreateNewElement (elementType.ElTypeNum,
			(PtrSSchema) (elementType.ElSSchema),
			TabDocuments[document - 1], FALSE);
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
      /* verifie le parametre document */
      TtaError (ERR_invalid_document_parameter);
   else if (TabDocuments[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else if (elementType.ElTypeNum < 1 ||
	    elementType.ElTypeNum > ((PtrSSchema) (elementType.ElSSchema))->SsNRules)
      /* parametre document correct */
      TtaError (ERR_invalid_element_type);
   else
      CreateNewElement (elementType.ElTypeNum,
			(PtrSSchema) (elementType.ElSSchema),
			TabDocuments[document - 1], TRUE);
}

#endif	/* NODISPLAY */

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
   boolean             racine;

   UserErrorCode = 0;
   if (element == NULL)
      TtaError (ERR_invalid_parameter);
   /* verifie le parametre document */
   else if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (TabDocuments[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parametre document correct */
     {
	pDoc = TabDocuments[document - 1];
	RegisterExternalRef ((PtrElement) element, pDoc, FALSE);
	RegisterDeletedReferredElem ((PtrElement) element, pDoc);
	racine = (((PtrElement) element)->ElParent == NULL);
#ifndef NODISPLAY
	UndisplayElement ((PtrElement) element, document);
#else
	RemoveElement ((PtrElement) element);
#endif
	if (racine)
	   if (pDoc->DocRootElement == (PtrElement) element)
	      /* c'est tout l'arbre principal qu'on retire */
	      pDoc->DocRootElement = NULL;
	   else if (pDoc->DocAssocRoot[((PtrElement) element)->ElAssocNum - 1] == (PtrElement) element)
	      /* cherche si c'est la racine d'un arbre associe' */
	      pDoc->DocAssocRoot[((PtrElement) element)->ElAssocNum - 1] = NULL;
     }
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
   DisplayMode         SauveDisplayMode;

#endif

   UserErrorCode = 0;
   if (element == NULL)
      TtaError (ERR_invalid_parameter);
   else
     {
	/* verifie le parametre document */
	if (document < 1 || document > MAX_DOCUMENTS)
	   TtaError (ERR_invalid_document_parameter);
	else if (TabDocuments[document - 1] == NULL)
	   TtaError (ERR_invalid_document_parameter);
	else
	   /* parametre document correct */
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
		/* on a effectivement change' les droits de l'element */
		if (newAccessRight == AccessHidden)
		   /* l'element devient cache', detruire ses paves */
		   HideElement ((PtrElement) element, document);
		else if (oldAccessRight == AccessHidden)
		   /* l'element n'est plus cache' : creer ses paves */
		   RedispNewElement (document, (PtrElement) element, NULL, TRUE, FALSE);
		else
		  {
		     SauveDisplayMode = TtaGetDisplayMode (document);
		     if (SauveDisplayMode != NoComputedDisplay
			 && (newAccessRight == AccessReadOnly ||
			     newAccessRight == AccessReadWrite))
			/* changer AbCanBeModified dans tous les paves */
			/* sauf si on est en mode sans calcul de l'image */
		       {
			  /* on passe d'abord en mode d'affichage differe' si */
			  /* on n'y est pas deja */
			  if (SauveDisplayMode != DeferredDisplay)
			     TtaSetDisplayMode (document, DeferredDisplay);
			  /* on change AbCanBeModified sur tous les paves */
			  ChangePavModif ((PtrElement) element, document,
				       (newAccessRight == AccessReadWrite));
			  /* on retablit le mode d'affichage du document */
			  /* on fait reafficher si on etait en mode immediat */
			  if (SauveDisplayMode != DeferredDisplay)
			     TtaSetDisplayMode (document, SauveDisplayMode);
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
void                TtaHolophrastElement (Element element, boolean holophrast, Document document)
#else  /* __STDC__ */
void                TtaHolophrastElement (element, holophrast, document)
Element             element;
boolean             holophrast;
Document            document;

#endif /* __STDC__ */
{
boolean             CanHolo;

   UserErrorCode = 0;
   if (element == NULL)
      TtaError (ERR_invalid_parameter);
   else if (document < 1 || document > MAX_DOCUMENTS)
      /* verifie le parametre document */
      TtaError (ERR_invalid_document_parameter);
   else if (TabDocuments[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else if (((PtrElement) element)->ElParent == NULL)
      /* parametre document correct */
      TtaError (ERR_cannot_holophrast_a_root);
   else 
    {
      CanHolo = TRUE;
      if (ThotLocalActions[T_holotable]!=NULL)
          (*ThotLocalActions[T_holotable])(((PtrElement)element), &CanHolo);
      if ((((PtrElement)element)->ElTerminal &&
	    ((PtrElement)element)->ElLeafType == LtPageColBreak) ||
	   !CanHolo)
      /* on n'holophraste ni les sauts de page ni certains */
      /* elements de tableau */
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
void                TtaSetStructureChecking (boolean on, Document document)
#else  /* __STDC__ */
void                TtaSetStructureChecking (on, document)
boolean             on;
Document            document;

#endif /* __STDC__ */
{
   if (on == 0)
      AvecControleStruct = 0;
   else
      AvecControleStruct = 1;
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
   return AvecControleStruct;
}


/* ----------------------------------------------------------------------
   TtaSetCheckingMode

   Changes checking mode.

   Parameter:
   strict: if TRUE, the presence of all mandatory elements is checked.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TtaSetCheckingMode (boolean strict)

#else  /* __STDC__ */
void                TtaSetCheckingMode (strict)
boolean             strict;

#endif /* __STDC__ */

{
   FullStructureChecking = strict;
}


/* ----------------------------------------------------------------------
   TtaGetMainRoot

   Returns the root element of the main abstract tree representing a document.

   Parameter:
   document: the document.

   Return value:
   the root element of the main abstract tree.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
Element             TtaGetMainRoot (Document document)

#else  /* __STDC__ */
Element             TtaGetMainRoot (document)
Document            document;

#endif /* __STDC__ */

{
   PtrElement          element;

   UserErrorCode = 0;
   /* verifie le parametre document */
   element = NULL;
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
	element = TabDocuments[document - 1]->DocRootElement;
     }
   return ((Element) element);
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
   boolean             found;

   UserErrorCode = 0;
   nextRoot = NULL;
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
	if (*root == NULL)
	  {
	     nextRoot = TabDocuments[document - 1]->DocAssocRoot[0];
	  }
	else
	  {
	     pDoc = TabDocuments[document - 1];
	     pEl = (PtrElement) (*root);
	     /* si l'element n'est pas une racine, on remonte jusqu'a la */
	     /* racine de son arbre */
	     while (pEl->ElParent != NULL)
		pEl = pEl->ElParent;
	     if (pEl == pDoc->DocRootElement)
		/* c'est l'arbre principal, on retourne le 1er arbre associe' */
		nextRoot = pDoc->DocAssocRoot[0];
	     else
	       {
		  /* on cherche les elements associes de racine pEl */
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
   TtaGetParent

   Returns the parent element (i.e. first ancestor) of a given element.

   Parameter:
   element: the element whose the parent is asked.

   Return value:
   the parent element, or NULL if there is no parent (root).

   ---------------------------------------------------------------------- */

#ifdef __STDC__
Element             TtaGetParent (Element element)

#else  /* __STDC__ */
Element             TtaGetParent (element)
Element             element;

#endif /* __STDC__ */

{
   PtrElement          parent;

   UserErrorCode = 0;
   parent = NULL;
   if (element == NULL)
     {
	TtaError (ERR_invalid_parameter);
     }
   else
      parent = ((PtrElement) element)->ElParent;
   return ((Element) parent);
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
     {
	TtaError (ERR_invalid_parameter);
     }
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
     {
	TtaError (ERR_invalid_parameter);
     }
   else if (ancestorType.ElTypeNum > ((PtrSSchema) (ancestorType.ElSSchema))->SsNRules
	    || ancestorType.ElTypeNum < 1)
     {
	TtaError (ERR_invalid_element_type);
     }
   else
     {
	ancestor = GetTypedAncestor (((PtrElement) element)->ElParent,
			      ancestorType.ElTypeNum,
			      (PtrSSchema) (ancestorType.ElSSchema));
     }
   return ((Element) ancestor);
}

/* ----------------------------------------------------------------------
   TtaGetElementType

   Returns the type of a given element.

   Parameter:
   element: the element.

   Return value:
   type of the element.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
ElementType         TtaGetElementType (Element element)

#else  /* __STDC__ */
ElementType         TtaGetElementType (element)
Element             element;

#endif /* __STDC__ */

{
   ElementType         elementType;

   UserErrorCode = 0;
   elementType.ElSSchema = NULL;
   elementType.ElTypeNum = 0;
   if (element == NULL)
     {
	TtaError (ERR_invalid_parameter);
     }
   else
     {
	elementType.ElSSchema = (SSchema) ((PtrElement) element)->ElSructSchema;
	elementType.ElTypeNum = ((PtrElement) element)->ElTypeNumber;
     }
   return elementType;
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
char               *TtaGetElementTypeName (ElementType elementType)

#else  /* __STDC__ */
char               *TtaGetElementTypeName (elementType)
ElementType         elementType;

#endif /* __STDC__ */

{

   UserErrorCode = 0;
   nameBuffer[0] = '\0';
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
	strncpy (nameBuffer, ((PtrSSchema) (elementType.ElSSchema))->SsRule[elementType.ElTypeNum - 1].SrName, MAX_NAME_LENGTH);
     }
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
void                TtaGiveTypeFromName (ElementType * elementType, char *name)

#else  /* __STDC__ */
void                TtaGiveTypeFromName (elementType, name)
ElementType        *elementType;
char               *name;

#endif /* __STDC__ */

{
   UserErrorCode = 0;
   (*elementType).ElTypeNum = 0;
   if (name == NULL || name[0] == '\0' || (*elementType).ElSSchema == NULL)
     {
	(*elementType).ElSSchema = NULL;
	TtaError (ERR_invalid_parameter);
     }
   else
     {
	GetSRuleFromName (&((*elementType).ElTypeNum), (PtrSSchema *) (&((*elementType).ElSSchema)), name);
     }
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
char               *TtaGetElementLabel (Element element)

#else  /* __STDC__ */
char               *TtaGetElementLabel (element)
Element             element;

#endif /* __STDC__ */

{

   UserErrorCode = 0;
   nameBuffer[0] = '\0';
   if (element == NULL)
     {
	TtaError (ERR_invalid_parameter);
     }
   else
     {
	strncpy (nameBuffer, ((PtrElement) element)->ElLabel, MAX_NAME_LENGTH);
     }
   return nameBuffer;
}

/* ----------------------------------------------------------------------
   TtaGetElementVolume

   Returns the volume of a given element, i.e. the number of characters
   contained in that element.

   Parameter:
   element: the element.

   Return value:
   element volume.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
int                 TtaGetElementVolume (Element element)

#else  /* __STDC__ */
int                 TtaGetElementVolume (element)
Element             element;

#endif /* __STDC__ */

{
   int                 vol = 0;

   UserErrorCode = 0;
   if (element == NULL)
     {
	TtaError (ERR_invalid_parameter);
     }
   else
     {
	vol = ((PtrElement) element)->ElVolume;
     }
   return vol;
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
     {
	TtaError (ERR_invalid_parameter);
     }
   else if (elementType.ElTypeNum > ((PtrSSchema) (elementType.ElSSchema))->SsNRules ||
	    elementType.ElTypeNum < 1)
     {
	TtaError (ERR_invalid_element_type);
     }
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
     {
	TtaError (ERR_invalid_parameter);
     }
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
     {
	TtaError (ERR_invalid_parameter);
     }
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

/** debut ajout */
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
     {
	TtaError (ERR_invalid_parameter);
     }
   else
     {
	if (((PtrElement) element)->ElHolophrast)
	   result = 1;
	else
	   result = 0;
     }
   return result;
}
/** fin ajout */


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
     {
	TtaError (ERR_invalid_parameter);
     }
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
     {
	TtaError (ERR_invalid_parameter);
     }
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
     {
	TtaError (ERR_invalid_parameter);
     }
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
     {
	TtaError (ERR_invalid_parameter);
     }
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
     {
	TtaError (ERR_invalid_parameter);
     }
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
     {
	TtaError (ERR_invalid_parameter);
     }
   else if (((PtrElement) element)->ElSructSchema->SsRule[((PtrElement) element)->ElTypeNumber - 1].SrConstruct != CsPairedElement)
     {
	TtaError (ERR_invalid_element_type);
     }
   else if (((PtrElement) element)->ElSructSchema->SsRule[((PtrElement) element)->ElTypeNumber - 1].SrFirstOfPair)
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
boolean             TtaCanInsertSibling (ElementType elementType, Element sibling, boolean before, Document document)

#else  /* __STDC__ */
boolean             TtaCanInsertSibling (elementType, sibling, before, document)
ElementType         elementType;
Element             sibling;
boolean             before;
Document            document;

#endif /* __STDC__ */

{
   boolean             ret;

   UserErrorCode = 0;
   ret = FALSE;
   if (sibling == NULL)
      TtaError (ERR_invalid_parameter);
   else if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (TabDocuments[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parametre document correct */
   if (elementType.ElSSchema == NULL)
      TtaError (ERR_invalid_parameter);
   else if (elementType.ElTypeNum < 1 ||
	    elementType.ElTypeNum > ((PtrSSchema) (elementType.ElSSchema))->SsNRules)
      TtaError (ERR_invalid_element_type);
   else
      ret = AllowedSibling ((PtrElement) sibling, TabDocuments[document - 1],
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
boolean             TtaCanInsertFirstChild (ElementType elementType, Element parent, Document document)

#else  /* __STDC__ */
boolean             TtaCanInsertFirstChild (elementType, parent, document)
ElementType         elementType;
Element             parent;
Document            document;

#endif /* __STDC__ */

{
   boolean             ret;

   UserErrorCode = 0;
   ret = FALSE;
   if (parent == NULL)
      TtaError (ERR_invalid_parameter);
   else if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (TabDocuments[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parametre document correct */
   if (elementType.ElSSchema == NULL)
      TtaError (ERR_invalid_parameter);
   else if (elementType.ElTypeNum < 1 ||
	    elementType.ElTypeNum > ((PtrSSchema) (elementType.ElSSchema))->SsNRules)
      TtaError (ERR_invalid_element_type);
   else
      ret = AllowedFirstChild ((PtrElement) parent, TabDocuments[document - 1],
				 elementType.ElTypeNum,
				 (PtrSSchema) (elementType.ElSSchema),
				 FALSE, FALSE);
   return ret;
}

/* ----------------------------------------------------------------------
   TtaGetDocument

   Returns the document containing a given element

   Parameters:
   element: the element for which document is asked.

   Return value:
   the document containing that element or 0 if the element does not
   belong to any document.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
Document            TtaGetDocument (Element element)

#else  /* __STDC__ */
Document            TtaGetDocument (element)
Element             element;

#endif /* __STDC__ */

{

   PtrDocument         pDoc;
   Document            ret;

   UserErrorCode = 0;
   ret = 0;
   if (element == NULL)
     {
	TtaError (ERR_invalid_parameter);
     }
   else
     {
	pDoc = DocumentOfElement ((PtrElement) element);
	if (pDoc != NULL)
	   ret = IdentDocument (pDoc);
     }
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
   PtrPasteElem      next;

   UserErrorCode = 0;
   next = NULL;
   if (*element == NULL)
      next = ElemSauve;
   else if (!DansTampon ((PtrElement) (*element)))
      /* element is not in the clipboard */
      TtaError (ERR_invalid_parameter);
   else if (((PtrElement) (*element))->ElParent != NULL)
      TtaError (ERR_invalid_parameter);
   else
     {
	next = ElemSauve;
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
   if (DocDeSauve == NULL)
      return 0;
   else
      return IdentDocument (DocDeSauve);
}
#endif


/* ----------------------------------------------------------------------
   TtaSearchTypedElement

   Returns the first element of a given type. Searching can be done in
   a tree or starting from a given element towards the beginning or the
   end of the abstract tree.

   Parameters:
   searchedType: type of element to be searched. If searchedType.ElSSchema
   is NULL, searchedType must be a basic type ; then the next basic
   element of that type will be returned, whatever its structure
   schema.
   scope: SearchForward, SearchBackward or SearchInTree.
   element: the element that is the root of the tree
   (if scope = SearchInTree) or the starting element
   (if scope = SearchForward or SearchBackward).

   Return value:
   the element found, or NULL if no element has been found.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
Element             TtaSearchTypedElement (ElementType searchedType, SearchDomain scope, Element element)

#else  /* __STDC__ */
Element             TtaSearchTypedElement (searchedType, scope, element)
ElementType         searchedType;
SearchDomain        scope;
Element             element;

#endif /* __STDC__ */

{
   PtrElement          pEl;
   PtrElement          elementFound;
   boolean             ok;

   UserErrorCode = 0;
   elementFound = NULL;
   ok = TRUE;
   if (element == NULL)
     {
	TtaError (ERR_invalid_parameter);
	ok = FALSE;
     }
   else if (searchedType.ElSSchema == NULL)
     {
	if (searchedType.ElTypeNum > MAX_BASIC_TYPE)
	  {
	     TtaError (ERR_invalid_element_type);
	     ok = FALSE;
	  }
     }
   else if (searchedType.ElTypeNum < 1 ||
	    searchedType.ElTypeNum > ((PtrSSchema) (searchedType.ElSSchema))->SsNRules)
     {
	TtaError (ERR_invalid_element_type);
	ok = FALSE;
     }

   if (ok)
     {
	if (scope == SearchBackward)
	   pEl = BackSearchTypedElem ((PtrElement) element, searchedType.ElTypeNum, (PtrSSchema) (searchedType.ElSSchema));
	else
	   pEl = FwdSearchTypedElem ((PtrElement) element, searchedType.ElTypeNum, (PtrSSchema) (searchedType.ElSSchema));

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

#ifdef __STDC__
static PtrElement   SearchLabel (char *label, PtrElement pEl)

#else  /* __STDC__ */
static PtrElement   SearchLabel (label, pEl)
char               *label;
PtrElement          pEl;

#endif /* __STDC__ */

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

#ifdef __STDC__
Element             TtaSearchElementByLabel (char *searchedLabel, Element element)

#else  /* __STDC__ */
Element             TtaSearchElementByLabel (searchedLabel, element)
char               *searchedLabel;
Element             element;

#endif /* __STDC__ */

{
   PtrElement          elementFound;

   UserErrorCode = 0;
   elementFound = NULL;
   if (element == NULL)
     {
	TtaError (ERR_invalid_parameter);
     }
   else if (searchedLabel[0] == '\0')
     {
	TtaError (ERR_invalid_element_type);
     }
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
     {
	TtaError (ERR_invalid_parameter);
     }
   else if (((PtrElement) element)->ElSructSchema->SsRule[((PtrElement) element)->ElTypeNumber - 1].SrConstruct != CsPairedElement)
     {
	TtaError (ERR_invalid_element_type);
     }
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
Element             TtaSearchNoPageBreak (Element element, boolean forward)

#else  /* __STDC__ */
Element             TtaSearchNoPageBreak (element, forward)
Element             element;
boolean             forward;

#endif /* __STDC__ */

{
   PtrElement          noPage;

   UserErrorCode = 0;
   noPage = NULL;
   if (element == NULL)
     {
	TtaError (ERR_invalid_parameter);
     }
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
/* fin du module */
