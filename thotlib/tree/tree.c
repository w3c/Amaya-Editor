/*
 * Copyright (c) 1996 INRIA, All rights reserved
 */

/*
   This module implements the operations on the abstract trees used
   by the Thot editor.
*/

#include "libmsg.h"
#include "thot_sys.h"
#include "message.h"
#include "constmedia.h"
#include "typemedia.h"
#include "storage.h"
#include "res.h"

#define EXPORT extern
#include "select_tv.h"
#include "edit_tv.h"

#include "memory_f.h"
#include "schemas_f.h"
#include "absboxes_f.h"
#include "draw_f.h"
#include "content_f.h"
#include "references_f.h"
#include "fileaccess_f.h"
#include "structschema_f.h"
#include "ouvre_f.h"
#include "tree_f.h"
#include "exceptions_f.h"
#include "abspictures_f.h"
#include "externalref_f.h"

/*----------------------------------------------------------------------
   NewLabel retourne la valeur entiere du prochain label a         
   affecter a un element du document pDoc.                 
  ----------------------------------------------------------------------*/

#ifdef __STDC__
int                 NewLabel (PtrDocument pDoc)

#else  /* __STDC__ */
int                 NewLabel (pDoc)
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   pDoc->DocLabelExpMax++;
   return pDoc->DocLabelExpMax;
}

/*----------------------------------------------------------------------
   GetCurrentLabel retourne la valeur entiere du plus grand label  
   affecte' a un element du document pDoc.                 
  ----------------------------------------------------------------------*/

#ifdef __STDC__
int                 GetCurrentLabel (PtrDocument pDoc)

#else  /* __STDC__ */
int                 GetCurrentLabel (pDoc)
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   return pDoc->DocLabelExpMax;
}

/*----------------------------------------------------------------------
   SetCurrentLabel change la valeur entiere du plus grand label    
   affecte' a un element du document pDoc.                 
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                SetCurrentLabel (PtrDocument pDoc, int label)

#else  /* __STDC__ */
void                SetCurrentLabel (pDoc, label)
PtrDocument         pDoc;
int                 label;

#endif /* __STDC__ */

{
   pDoc->DocLabelExpMax = label;
}

/*----------------------------------------------------------------------
   DocumentOfElement
   returns the document to which a pEl element belongs.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
PtrDocument         DocumentOfElement (PtrElement pEl)

#else  /* __STDC__ */
PtrDocument         DocumentOfElement (pEl)
PtrElement          pEl;

#endif /* __STDC__ */

{
   PtrElement          pE;
   int                 i, j;
   PtrDocument         pDoc;

   if (pEl == NULL)
      return NULL;

   pE = pEl;
   /* finds the root of the tree to which the element belongs */
   while (pE->ElParent != NULL)
      pE = pE->ElParent;

   /* searchs among the roots of existing documents as well as among
      the associated elements and parameters of those documents */
   for (i = 0; i < MAX_DOCUMENTS; i++)
     {
	pDoc = LoadedDocument[i];
	if (pDoc != NULL)
	  {
	     if (pDoc->DocRootElement == pE)
              /* it's the root of the principal tree */
		return pDoc;
             /* searchs among the associate elements */
	     for (j = 0; j < MAX_ASSOC_DOC; j++)
		if (pDoc->DocAssocRoot[j] == pE)
		   return pDoc;
             /* searchs among the parameters */
	     for (j = 0; j < MAX_PARAM_DOC; j++)
		if (pDoc->DocParameters[j] == pE)
		   return pDoc;
	  }
     }

   return NULL;
}

/*----------------------------------------------------------------------
   ProtectElement
   sets the ElIsCopy flag in the elements of the subtree of pEl.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                ProtectElement (PtrElement pEl)

#else  /* __STDC__ */
void                ProtectElement (pEl)
PtrElement          pEl;

#endif /* __STDC__ */

{
   pEl->ElIsCopy = TRUE;
   if (!pEl->ElTerminal)
     {
	pEl = pEl->ElFirstChild;
	while (pEl != NULL)
	  {
	     ProtectElement (pEl);
	     pEl = pEl->ElNext;
	  }
     }
}


/*----------------------------------------------------------------------
   GetOtherPairedElement
   returns a pointer on the mark which is pair to the one pointed to by pEl.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
PtrElement          GetOtherPairedElement (PtrElement pEl)

#else  /* __STDC__ */
PtrElement          GetOtherPairedElement (pEl)
PtrElement          pEl;

#endif /* __STDC__ */

{
   PtrElement          pOther;
   PtrSSchema        pSS;
   int                 typeNum;
   boolean             found, begin;

   pOther = NULL;
   if (pEl != NULL)
      if (pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrConstruct == CsPairedElement)
	/* check if it's a pair element */
	 if (pEl->ElOtherPairedEl != NULL)
	    /* the element already has a pointer to the other element of the
	       pair */
	    pOther = pEl->ElOtherPairedEl;
	 else
	   {
	      /* searchs the mark having the same type and number */
	      pSS = pEl->ElStructSchema;
	      begin = pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrFirstOfPair;
	      if (begin)
		 /* pEl has a begin mark, so we search the end mark */
		 typeNum = pEl->ElTypeNumber + 1;
	      else
		 /* pEl has an end mark, so we search the begin mark */
		 typeNum = pEl->ElTypeNumber - 1;
	      found = FALSE;
	      pOther = pEl;
	      /* search loop */
	      while ((pOther != NULL) && (!found))
		{
		   if (begin)
		     /* forward search */
		      pOther = FwdSearchTypedElem (pOther, typeNum, pSS);
		   else
		     /* backward search */
		      pOther = BackSearchTypedElem (pOther, typeNum, pSS);
		   if (pOther != NULL)
		     /* we found an element having the same type as that of
			 the of the element we are searching */
		     /* it's a hit, if it has the same identifier */
		      found = (pOther->ElPairIdent == pEl->ElPairIdent ||
			       (pEl->ElPairIdent == 0 && pOther->ElOtherPairedEl == NULL));
		}
	      if (found)
		 /* found it! Now link the two elements */
		{
		   pEl->ElOtherPairedEl = pOther;
		   pOther->ElOtherPairedEl = pEl;
		   if (pEl->ElPairIdent == 0)
		      pEl->ElPairIdent = pOther->ElPairIdent;
		}
	   }
   return pOther;
}

/*----------------------------------------------------------------------
   ElementIsReadOnly
   returns TRUE if the element pointed by pEl is protected against user
   modifications, or if it belongs to a protected tree.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
boolean             ElementIsReadOnly (PtrElement pEl)

#else  /* __STDC__ */
boolean             ElementIsReadOnly (pEl)
PtrElement          pEl;

#endif /* __STDC__ */

{
   PtrElement          pAsc;
   boolean             ret;
   boolean             testRO;

   ret = FALSE;
   testRO = TRUE;
   pAsc = pEl;
   while (pAsc != NULL)
      if (pAsc->ElAccess == AccessHidden ||
	  (testRO && pAsc->ElAccess == AccessReadOnly))
	{
	   ret = TRUE;
	   pAsc = NULL;
	}
      else
	{
	   if (pAsc->ElAccess == AccessReadWrite)
	      testRO = FALSE;
	   pAsc = pAsc->ElParent;
	}
   return ret;
}

/*----------------------------------------------------------------------
   ElementIsHidden
   returns TRUE if the element pointed by pEl is hidden to the user, or 
   if it belongs to a hidden tree.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
boolean             ElementIsHidden (PtrElement pEl)

#else  /* __STDC__ */
boolean             ElementIsHidden (pEl)
PtrElement          pEl;

#endif /* __STDC__ */

{
   PtrElement          pAsc;
   boolean             ret;

   ret = FALSE;
   pAsc = pEl;
   while (pAsc != NULL && !ret)
      if (pAsc->ElAccess == AccessHidden)
	 ret = TRUE;
      else
	 pAsc = pAsc->ElParent;
   return ret;
}

/*----------------------------------------------------------------------
   FwdSearchTypeNameInSubtree					
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static PtrElement   FwdSearchTypeNameInSubtree (PtrElement pEl, boolean test, char *typeName)

#else  /* __STDC__ */
static PtrElement   FwdSearchTypeNameInSubtree (pEl, test, typeName)
PtrElement          pEl;
boolean             test;
char               *typeName;

#endif /* __STDC__ */

{
   PtrElement          pRet, pChild;

   pRet = NULL;
   if (test)
      if (strcmp (typeName, pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrName) == 0)
	/* got a hit on the element */
	 pRet = pEl;		
   if (pRet == NULL && !pEl->ElTerminal)
      /* a recursive search among the sons of the element */
     {
	pChild = pEl->ElFirstChild;
	while (pChild != NULL && pRet == NULL)
	  {
	     pRet = FwdSearchTypeNameInSubtree (pChild, TRUE, typeName);
	     pChild = pChild->ElNext;
	  }
     }
   return pRet;
}

/*----------------------------------------------------------------------
   FwdSearchElemByTypeName
   makes forward search on a tree, starting from the element pointed by
   Pel,  of an element having the name typeName. The function returns a
   pointer to the element if there's a hit, NULL otherwise.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
PtrElement          FwdSearchElemByTypeName (PtrElement pEl, char *typeName)

#else  /* __STDC__ */
PtrElement          FwdSearchElemByTypeName (pEl, typeName)
PtrElement          pEl;
char               *typeName;

#endif /* __STDC__ */

{
   PtrElement          pRet, pCur, pAsc;
   boolean             stop;

   pRet = NULL;
   if (pEl != NULL && typeName != NULL)
     {
	/* searches the subtree of the element */
	pRet = FwdSearchTypeNameInSubtree (pEl, FALSE, typeName);
	if (pRet == NULL)
	   /* if not found, searches on the subtrees of the next siblings
	      of the element */
	  {
	     pCur = pEl->ElNext;
	     while (pCur != NULL && pRet == NULL)
	       {
		  pRet = FwdSearchTypeNameInSubtree (pCur, TRUE, typeName);
		  pCur = pCur->ElNext;
	       }
	     /* if not found, climbs up a level, then continues the search
		with the next brother */
	     if (pRet == NULL)
	       {
		  stop = FALSE;
		  pAsc = pEl;
		  do
		    {
		       pAsc = pAsc->ElParent;
		       if (pAsc == NULL)
			  stop = TRUE;
		       else if (pAsc->ElNext != NULL)
			  stop = TRUE;
		    }
		  while (!stop);
		  if (pAsc != NULL)
		     /* verifies if this element is the one we're looking
			for */
		    {
		       pAsc = pAsc->ElNext;
		       if (pAsc != NULL)
			  if (strcmp (typeName, pAsc->ElStructSchema->SsRule[pAsc->ElTypeNumber - 1].SrName) == 0)
			    /* found */
			     pRet = pAsc; 
			  else
			     pRet = FwdSearchElemByTypeName (pAsc, typeName);
		    }
	       }
	  }
     }
   return pRet;
}

/*----------------------------------------------------------------------
   BackSearchTypeNameInSubtree				       
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static PtrElement   BackSearchTypeNameInSubtree (PtrElement pEl, char *typeName)

#else  /* __STDC__ */
static PtrElement   BackSearchTypeNameInSubtree (pEl, typeName)
PtrElement          pEl;
char               *typeName;

#endif /* __STDC__ */

{
   PtrElement          pRet, pChild;

   pRet = NULL;
   /* Cherche le dernier fils */
   if (!pEl->ElTerminal)
     {
	pChild = pEl->ElFirstChild;
	if (pChild != NULL)
	   while (pChild->ElNext != NULL)
	      pChild = pChild->ElNext;
	/* on cherche a partir des freres precedents */
	while (pChild != NULL && pRet == NULL)
	  {
	     pRet = BackSearchTypeNameInSubtree (pChild, typeName);
	     pChild = pChild->ElPrevious;
	  }
     }
   if (pRet == NULL)
     {
	if (strcmp (typeName, pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrName) == 0)
	   pRet = pEl;		/* found ! C'est l'element lui-meme */
     }
   return pRet;
}
/*----------------------------------------------------------------------
   BackSearchElemByTypeName
   starting from the element pointed by elt, it makes a backward tree search
   for an element having type typeName.
   If it finds the  element, it returns a pointer to it. Otherwise, it
   NULL.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
PtrElement          BackSearchElemByTypeName (PtrElement pEl, char *typeName)

#else  /* __STDC__ */
PtrElement          BackSearchElemByTypeName (pEl, typeName)
PtrElement          pEl;
char               *typeName;

#endif /* __STDC__ */

{
   PtrElement          pRet, pCur;

   pRet = NULL;			/* pRet: futur resultat de la recherche */
   if (pEl != NULL)
      /* cherche dans les sous-arbres des freres precedents de l'element */
     {
	pCur = pEl->ElPrevious;
	while (pCur != NULL && pRet == NULL)
	  {
	     pRet = BackSearchTypeNameInSubtree (pCur, typeName);
	     pCur = pCur->ElPrevious;
	  }
	/* si echec, cherche si c'est le pere, puis cherche dans les */
	/* sous-arbres des oncles de l'element */
	if (pRet == NULL)
	   if (pEl->ElParent != NULL)
	     {
		pEl = pEl->ElParent;
		if (strcmp (typeName, pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrName) == 0)
		   pRet = pEl;	/* found, c'est le pere */
		else
		   pRet = BackSearchElemByTypeName (pEl, typeName);
	     }
     }
   return pRet;
}

/*----------------------------------------------------------------------
   BackSearchVisibleSubtree                                        
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static PtrElement   BackSearchVisibleSubtree (PtrElement pEl, int *view)

#else  /* __STDC__ */
static PtrElement   BackSearchVisibleSubtree (pEl, view)
PtrElement          pEl;
int                *view;

#endif /* __STDC__ */

{
   PtrElement          pRet, pChild;

   pRet = NULL;
   if (pEl->ElAbstractBox[*view - 1] != NULL)
      pRet = pEl;
   else
      /* Cherche le dernier fils */
   if (!pEl->ElTerminal)
     {
	pChild = pEl->ElFirstChild;
	if (pChild != NULL)
	   while (pChild->ElNext != NULL)
	      pChild = pChild->ElNext;
	/* on cherche a partir des freres precedents */
	while (pChild != NULL && pRet == NULL)
	  {
	     pRet = BackSearchVisibleSubtree (pChild, view);
	     pChild = pChild->ElPrevious;
	  }

     }
   return pRet;
}

/*----------------------------------------------------------------------
   AttrFound
   searches for a specific attribute in an element.
   Returns TRUE if the element pointed by pEL is the one we are looking
   for, FALSE otherwise.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static boolean      AttrFound (PtrElement pEl, char *textVal, int val, int attrNum, PtrSSchema pSS)

#else  /* __STDC__ */
static boolean      AttrFound (pEl, textVal, val, attrNum, pSS)
PtrElement          pEl;
char               *textVal;
int      val;
int     attrNum;
PtrSSchema       pSS;

#endif /* __STDC__ */

{
   PtrAttribute         pAttr;
   boolean             ret;

   ret = FALSE;
   if (pEl->ElFirstAttr != NULL)	/* l'element a un attribut */
      if (pSS == NULL && attrNum == 0)
	 /* on cherche un attribut quelconque */
	 ret = TRUE;		/* found ! */
      else
	 /* parcourt les attributs de l'element */
	{
	   pAttr = pEl->ElFirstAttr;
	   while (pAttr != NULL && !ret)
	     {
		if ((pSS == NULL || pAttr->AeAttrSSchema->SsCode == pSS->SsCode)
		    && pAttr->AeAttrNum == attrNum)
		   /* c'est l'attribut cherche */
		   if (val == 0)
		      /* on cherche une valeur quelconque */
		      ret = TRUE;	/* found ! */
		   else
		      switch (pAttr->AeAttrType)
			    {
			       case AtNumAttr:
			       case AtEnumAttr:
				  if (pAttr->AeAttrValue == val)
				     /* valeur cherchee */

				     ret = TRUE;	/* found ! */
				  break;
			       case AtTextAttr:
				  if (StringAndTextEqual (textVal, pAttr->AeAttrText))
				     ret = TRUE;
				  break;
			       default:
				  break;
			    }

		pAttr = pAttr->AeNext;
	     }
	}
   return ret;
}

/*----------------------------------------------------------------------
   FwdSearchAttrInSubtree                                          
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static PtrElement   FwdSearchAttrInSubtree (PtrElement pEl, boolean test, PtrSSchema pSS, int attrNum, int val, char *textVal)

#else  /* __STDC__ */
static PtrElement   FwdSearchAttrInSubtree (pEl, test, pSS, attrNum, val, textVal)
PtrElement          pEl;
boolean             test;
PtrSSchema       pSS;
int     attrNum;
int      val;
char               *textVal;

#endif /* __STDC__ */

{
   PtrElement          pRet, pChild;

   pRet = NULL;
   if (test)
      if (AttrFound (pEl, textVal, val, attrNum, pSS))
	 pRet = pEl;
   if (pRet == NULL)
      if (!pEl->ElTerminal)
	 /* on cherche parmi les fils de l'element */
	{
	   pChild = pEl->ElFirstChild;
	   while (pChild != NULL && pRet == NULL)
	     {
		pRet = FwdSearchAttrInSubtree (pChild, TRUE, pSS, attrNum, val, textVal);
		pChild = pChild->ElNext;
	     }
	}
   return pRet;
}

/*----------------------------------------------------------------------
   BackSearchAttrInSubtree                                         
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static PtrElement   BackSearchAttrInSubtree (PtrElement pEl, char *textVal, int val, int attrNum, PtrSSchema pSS)

#else  /* __STDC__ */
static PtrElement   BackSearchAttrInSubtree (pEl, textVal, val, attrNum, pSS)
PtrElement          pEl;
char               *textVal;
int       val;
int      attrNum;
PtrSSchema        pSS;

#endif /* __STDC__ */

{
   PtrElement          pRet, pChild;
   PtrAttribute         pAttr;

   pRet = NULL;
   if (pEl->ElFirstAttr != NULL)
      /* l'element a un attribut */
      if (pSS == NULL && attrNum == 0)
	 /* on cherche un attribut quelconque */
	 pRet = pEl;		/* found ! */
      else
	 /* parcourt les attributs de l'element */
	{
	   pAttr = pEl->ElFirstAttr;
	   while (pAttr != NULL && pRet == NULL)
	     {
		if ((pSS == NULL ||
		     pAttr->AeAttrSSchema->SsCode == pSS->SsCode)
		    && pAttr->AeAttrNum == attrNum)
		   /* c'est l'attribut cherche */
		   if (val == 0)
		      /* on cherche une valeur quelconque */
		      pRet = pEl;	/* found ! */
		   else
		      switch (pAttr->AeAttrType)
			    {
			       case AtNumAttr:
			       case AtEnumAttr:
				  if (pAttr->AeAttrValue == val)
				     /* valeur cherchee */
				     pRet = pEl;	/* found ! */
				  break;
			       case AtTextAttr:
				  if (StringAndTextEqual (textVal, pAttr->AeAttrText))
				     pRet = pEl;	/* found ! */
				  break;
			       default:
				  break;
			    }

		pAttr = pAttr->AeNext;
	     }
	}
   if (pRet == NULL)		/* Cherche le dernier fils */
      if (!pEl->ElTerminal)
	{
	   pChild = pEl->ElFirstChild;
	   if (pChild != NULL)
	      while (pChild->ElNext != NULL)
		 pChild = pChild->ElNext;
	   /* on cherche a partir des freres precedents */
	   while (pChild != NULL && pRet == NULL)
	     {
		pRet = BackSearchAttrInSubtree (pChild, textVal, val, attrNum, pSS);
		pChild = pChild->ElPrevious;
	     }
	}
   return pRet;
}

/*----------------------------------------------------------------------
   ElemIsEmptyOrRefOrPair
   tests if the element pointed by pEL is empty (Kind = 1), if it's a 
   reference element (Kind = 2), or if its a pair element (Kind = 3).
   Returns TRUE or FALSE, according to the result of the test.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static boolean      ElemIsEmptyOrRefOrPair (PtrElement pEl, int Kind)

#else  /* __STDC__ */
static boolean      ElemIsEmptyOrRefOrPair (pEl, Kind)
PtrElement          pEl;
int                 Kind;

#endif /* __STDC__ */

{
   boolean             ret;

   ret = FALSE;
   switch (Kind)
	 {
	    case 1:		/* element vide ? */
	       if (!pEl->ElTerminal)
		 {
		    if (pEl->ElFirstChild == NULL)
		       ret = TRUE;
		 }
	       else
		  switch (pEl->ElLeafType)
			{
			   case LtText:
			   case LtPicture:
			      if (pEl->ElTextLength == 0)
				 ret = TRUE;
			      break;
			   case LtPlyLine:
			      if (pEl->ElNPoints == 0)
				 ret = TRUE;
			      break;
			   case LtSymbol:
			   case LtGraphics:
			      if (pEl->ElGraph == '\0')
				 ret = TRUE;
			      break;
			   case LtReference:
			      if (pEl->ElReference == NULL)
				 ret = TRUE;
			      break;
			   default:
			      break;
			}
	       break;
	    case 2:		/* element reference ? */
	       if (pEl->ElTerminal)
		  if (pEl->ElLeafType == LtReference)
		     ret = TRUE;
	       break;
	    case 3:		/* element de paire ? */
	       if (pEl->ElTerminal)
		  if (pEl->ElLeafType == LtPairedElem)
		     ret = TRUE;
	       break;
	 }
   return ret;
}

/*----------------------------------------------------------------------
   FwdSearchEmptyInSubtree						
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static PtrElement   FwdSearchEmptyInSubtree (PtrElement pEl, boolean test, int Kind)

#else  /* __STDC__ */
static PtrElement   FwdSearchEmptyInSubtree (pEl, test, Kind)
PtrElement          pEl;
boolean             test;
int                 Kind;

#endif /* __STDC__ */

{
   PtrElement          pRet, pChild;

   pRet = NULL;
   if (test)
      if (ElemIsEmptyOrRefOrPair (pEl, Kind))
	 pRet = pEl;
   if (pRet == NULL && !pEl->ElTerminal)
      /* on cherche parmi les fils de l'element */
     {
	pChild = pEl->ElFirstChild;
	while (pChild != NULL && pRet == NULL)
	  {
	     pRet = FwdSearchEmptyInSubtree (pChild, TRUE, Kind);
	     pChild = pChild->ElNext;
	  }
     }
   return pRet;
}

/*----------------------------------------------------------------------
   FwdSearchRefOrEmptyElem
   starting from the element pointed by pEl, makes a forward search
   for the first empty element (Kind = 1), for the first 
   CsReference (Kind = 2), or for the first element of the pair
   pair (Kind = 3).
   If it finds an element, it returns a pointer to it. Otherwise, it
   returns NULL.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
PtrElement          FwdSearchRefOrEmptyElem (PtrElement pEl, int Kind)

#else  /* __STDC__ */
PtrElement          FwdSearchRefOrEmptyElem (pEl, Kind)
PtrElement          pEl;
int                 Kind;

#endif /* __STDC__ */

{
   PtrElement          pRet, pCur, pAsc;
   boolean             stop;

   pRet = NULL;
   if (pEl != NULL)
      /* cherche dans le sous-arbre de l'element */
     {
	pRet = FwdSearchEmptyInSubtree (pEl, FALSE, Kind);
	if (pRet == NULL)
	   /* si echec, cherche dans les sous-arbres des freres suivants */
	   /* de l'element */
	  {
	     pCur = pEl->ElNext;
	     while (pCur != NULL && pRet == NULL)
	       {
		  pRet = FwdSearchEmptyInSubtree (pCur, TRUE, Kind);
		  pCur = pCur->ElNext;
	       }
	     /* si echec, cherche le premier ascendant avec un frere suivant */
	     if (pRet == NULL)
	       {
		  stop = FALSE;
		  pAsc = pEl;
		  do
		    {
		       pAsc = pAsc->ElParent;
		       if (pAsc == NULL)
			  stop = TRUE;
		       else if (pAsc->ElNext != NULL)
			  stop = TRUE;
		    }
		  while (!stop);
		  if (pAsc != NULL)
		     /* cherche si cet element est celui cherche */
		    {
		       pAsc = pAsc->ElNext;
		       if (pAsc != NULL)
			 {
			    if (ElemIsEmptyOrRefOrPair (pAsc, Kind))
			       pRet = pAsc;		/* found */
			    if (pRet == NULL)
			       pRet = FwdSearchRefOrEmptyElem (pAsc, Kind);
			 }
		    }
	       }
	  }
     }
   return pRet;
}

/*----------------------------------------------------------------------
   BackSearchEmptyInSubtree					
  ----------------------------------------------------------------------*/


#ifdef __STDC__
static PtrElement   BackSearchEmptyInSubtree (PtrElement pEl, int Kind)

#else  /* __STDC__ */
static PtrElement   BackSearchEmptyInSubtree (pEl, Kind)
PtrElement          pEl;
int                 Kind;

#endif /* __STDC__ */

{
   PtrElement          pRet, pChild;

   pRet = NULL;
   /* Cherche le dernier fils */
   if (!pEl->ElTerminal)
     {
	pChild = pEl->ElFirstChild;
	if (pChild != NULL)
	   while (pChild->ElNext != NULL)
	      pChild = pChild->ElNext;
	/* on cherche a partir des freres precedents */
	while (pChild != NULL && pRet == NULL)
	  {
	     pRet = BackSearchEmptyInSubtree (pChild, Kind);
	     pChild = pChild->ElPrevious;
	  }
     }
   if (pRet == NULL)
      if (ElemIsEmptyOrRefOrPair (pEl, Kind))
	 pRet = pEl;		/* found */
   return pRet;
}

/*----------------------------------------------------------------------
   BackSearchRefOrEmptyElem
   starting from the element pointed by pEl, makes a forward search
   for the first empty element (Kind = 1), for the first 
   CsReference (Kind = 2), or for the first element of the pair
   pair (Kind = 3).
   If it finds an element, it returns a pointer to it. Otherwise, it
   returns NULL.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
PtrElement          BackSearchRefOrEmptyElem (PtrElement pEl, boolean Kind)

#else  /* __STDC__ */
PtrElement          BackSearchRefOrEmptyElem (pEl, Kind)
PtrElement          pEl;
boolean             Kind;

#endif /* __STDC__ */

{
   PtrElement          pRet, pCur;

   pRet = NULL;			/* pRet: futur resultat de la recherche */
   if (pEl != NULL)
      /* cherche dans les sous-arbres des freres precedents de l'element */
     {
	pCur = pEl->ElPrevious;
	while (pCur != NULL && pRet == NULL)
	  {
	     pRet = BackSearchEmptyInSubtree (pCur, Kind);
	     pCur = pCur->ElPrevious;
	  }
	/* si echec, cherche si c'est le pere, puis cherche dans les */
	/* sous-arbres des oncles de l'element */
	if (pRet == NULL)
	   if (pEl->ElParent != NULL)
	     {
		pEl = pEl->ElParent;
		if (ElemIsEmptyOrRefOrPair (pEl, Kind))
		   pRet = pEl;	/* found */
		else
		   pRet = BackSearchRefOrEmptyElem (pEl, Kind);
	     }
     }
   return pRet;
}

/*----------------------------------------------------------------------
   ChangeLnaguageLeaves
   changes the language attribute for all the text leaves in the
   subtree of the element pointed by pEl.
  ----------------------------------------------------------------------*/


#ifdef __STDC__
static void         ChangeLanguageLeaves (PtrElement pEl, Language lang)

#else  /* __STDC__ */
static void         ChangeLanguageLeaves (pEl, lang)
PtrElement          pEl;
Language            lang;

#endif /* __STDC__ */

{
   PtrElement          pChild;

   if (!pEl->ElTerminal)
      /* l'element n'est pas une feuille, on traite recursivement tout */
      /* son sous-arbre */
     {
	pChild = pEl->ElFirstChild;
	while (pChild != NULL)
	  {
	     /* si un descendant a un attribut Langue, on ne le change pas */
	     if (GetTypedAttrForElem (pChild, 1, NULL) == NULL)
		ChangeLanguageLeaves (pChild, lang);
	     pChild = pChild->ElNext;
	  }
     }
   else
      /* l'element est une feuille */ if (pEl->ElLeafType == LtText && pEl->ElLanguage != lang)
      /* c'est du texte dans une autre langue */
      if (TtaGetAlphabet (pEl->ElLanguage) == TtaGetAlphabet (lang))
	 /* cette langue s'ecrit dans le meme alphabet */
	 /* change la langue de l'element */
	 pEl->ElLanguage = lang;
}


/*----------------------------------------------------------------------
   LeavesInheritLanguage    fait heriter aux feuilles de texte du  
   sous-arbre de pEl la langue definie par le premier element      
   englobant.                                                      
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         LeavesInheritLanguage (PtrElement pEl)

#else  /* __STDC__ */
static void         LeavesInheritLanguage (pEl)
PtrElement          pEl;

#endif /* __STDC__ */

{
   PtrAttribute         pAttr;
   PtrElement          pElAttr;
   Language            lang;

   if (GetTypedAttrForElem (pEl, 1, NULL) == NULL)
      /* l'element lui-meme ne porte pas d'attribut Langue */
     {
	/* cherche l'attribut Langue porte' par un ascendant de pEl */
	pAttr = GetTypedAttrAncestor (pEl, 1, NULL, &pElAttr);
	if (pAttr != NULL)
	   /* on a trouve' un attribut langue sur un ascendant */
	   if (pAttr->AeAttrText != NULL)
	      if (pAttr->AeAttrText->BuContent[0] != '\0')
		 /* cet attribut Langue a une valeur */
		{
		   lang = TtaGetLanguageIdFromName (pAttr->AeAttrText->BuContent);
		   /* change la langue des feuilles de texte du sous-arbre */
		   ChangeLanguageLeaves (pEl, lang);
		}
     }
}


/*----------------------------------------------------------------------
   InsertElemAfterLastSibling insere l'element pointe par pNew (et ses freres	
   suivants), apres le dernier element suivant celui pointe par	
   pOld dans l'arbre abstrait.					
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                InsertElemAfterLastSibling (PtrElement pOld, PtrElement pNew)

#else  /* __STDC__ */
void                InsertElemAfterLastSibling (pOld, pNew)
PtrElement          pOld;
PtrElement          pNew;

#endif /* __STDC__ */

{
   PtrElement          pEl;
   PtrElement          pAsc;

   if (pNew != NULL && pOld != NULL)
     {
	pEl = pOld;
	while (pEl->ElNext != NULL)
	   pEl = pEl->ElNext;
	/* pEl pointe sur le dernier element suivant pOld */
	pEl->ElNext = pNew;
	pNew->ElPrevious = pEl;
	pEl = pNew;
	do
	  {
	     pEl->ElParent = pOld->ElParent;
	     /* fait heriter aux feuilles de texte de l'element la langue */
	     /* definie par le premier element englobant */
	     LeavesInheritLanguage (pEl);
	     /* met a jour le volume des elements englobants */
	     pAsc = pEl->ElParent;
	     while (pAsc != NULL)
	       {
		  pAsc->ElVolume = pAsc->ElVolume + pEl->ElVolume;
		  pAsc = pAsc->ElParent;
	       }
	     pEl = pEl->ElNext;
	  }
	while (pEl != NULL);
     }
}


/*----------------------------------------------------------------------
   CreateParameter   cree un parametre pour le document dont le    
   descripteur est pointe par pDoc.                                
   Le parametre a creer est du type elemType dans le schema de     
   structure pointe par pSS et appartient a la liste d'elements    
   associes assocNum.						
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static PtrElement   CreateParameter (int elemType, PtrSSchema pSS, PtrDocument pDoc, int assocNum)

#else  /* __STDC__ */
static PtrElement   CreateParameter (elemType, pSS, pDoc, assocNum)
int         elemType;
PtrSSchema        pSS;
PtrDocument         pDoc;
int                 assocNum;

#endif /* __STDC__ */

{

   int                 par, i;
   boolean             found;
   PtrElement          pEl;

   PtrElement          pPar;
   SRule              *pR;

   /* Cherche si ce parametre a une valeur pour le document */
   par = 0;
   found = FALSE;
   do
     {
	if (pDoc->DocParameters[par] != NULL)
	  {
	     pPar = pDoc->DocParameters[par];
	     if (pPar->ElTypeNumber == elemType && pPar->ElStructSchema == pSS)
		found = TRUE;
	  }
	par++;
     }
   while (!found && par < MAX_PARAM_DOC);
   if (found)
      /* le parametre a une valeur: l'arbre pointe par DocParameters[par]. */
      /* on copie cet arbre. */
     {
	pEl = CopyTree (pDoc->DocParameters[par - 1], pDoc, assocNum, pSS, pDoc,
			  NULL, TRUE, FALSE);

	/* on rend la copie non modifiable */
	if (pEl != NULL)
	   ProtectElement (pEl);
     }
   else
      /* le parametre n'a pas de valeur, on cree un element texte contenant */
      /* son nom entre deux caracteres '$' */
     {
	pEl = NewSubtree (CharString + 1, pSS, pDoc, assocNum, TRUE, TRUE,
			  TRUE, FALSE);
	if (pEl != NULL)
	  {
	     pEl->ElTypeNumber = elemType;	/* change le type: ce n'est pas un texte */
	     pEl->ElIsCopy = TRUE;	/* l'element est non modifiable */

	     i = 1;
	     pR = &pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1];
	     pEl->ElText->BuContent[0] = '$';
	     do
	       {
		  i++;
		  pEl->ElText->BuContent[i - 1] = pR->SrName[i - 2];
	       }
	     while (pEl->ElText->BuContent[i - 1] != '\0' && i != MAX_CHAR);
	     if (i < MAX_CHAR)
	       {
		  pEl->ElText->BuContent[i - 1] = '$';
		  i++;
	       }

	     else
		pEl->ElText->BuContent[i - 2] = '$';
	     pEl->ElText->BuContent[i - 1] = '\0';
	     pEl->ElTextLength = i - 1;
	     pEl->ElVolume = pEl->ElTextLength;
	     pEl->ElText->BuLength = i - 1;
	     pEl->ElLanguage = TtaGetDefaultLanguage ();
	  }

     }
   return pEl;
}


/*----------------------------------------------------------------------
   CopyAttributes copie les attributs de l'element pointe par pEl1 
   dans l'element pointe par pEl2.                                 
   Si Check est 'vrai', les attributs ne sont copies que si le     
   schema de structure qui les definit est utilise' par pEl2 ou    
   un de ses elements englobants.                                  
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         CopyAttributes (PtrElement pEl1, PtrElement pEl2, boolean Check)

#else  /* __STDC__ */
static void         CopyAttributes (pEl1, pEl2, Check)
PtrElement          pEl1;
PtrElement          pEl2;
boolean             Check;

#endif /* __STDC__ */

{
   PtrAttribute         pAttr1, pAttr2, pPrevAttr;
   PtrElement          pAsc;
   boolean             found;
   PtrReference        rf;
   PtrReference        pPr;
   boolean             bool;
   int                 len;

   pEl2->ElFirstAttr = NULL;	/* pas (encore) d'attributs dans la copie */
   if (pEl1->ElFirstAttr != NULL)
      /* il y a des attributs dans l'original, on les copie */
     {
	pAttr1 = pEl1->ElFirstAttr;	/* attribut a copier */
	pPrevAttr = NULL;	/* dernier attribut de la copie */
	pAttr2 = NULL;
	/* remplit et chaine tous les attributs copie */
	do
	  {
	     if (pAttr2 == NULL)
		GetAttr (&pAttr2);	/* acquiert un bloc attribut pour la copie */
	     *pAttr2 = *pAttr1;	/* copie l'attribut */
	     if (pAttr2->AeAttrType == AtTextAttr)
		/* c'est un attribut texte; il n'a pas encore de buffer */
		pAttr2->AeAttrText = NULL;
	     pAttr2->AeNext = NULL;
	     found = !Check;
	     if (!found)
		/* verifie que le schema de structure definissant l'attribut */
		/* est bien utilise' par un element ascendant de la copie */
	       {
		  pAsc = pEl2;
		  do

		     if (pAsc->ElStructSchema == pAttr2->AeAttrSSchema)
			found = TRUE;
		     else
			pAsc = pAsc->ElParent;
		  while (pAsc != NULL && !found);
		  if (!found)
		     /* cherche un schema de structure de meme nom ou une */
		     /* extension de schema de structure dans les */
		     /* ascendants de la copie */
		    {
		       pAsc = pEl2;
		       do
			  if (strcmp (pAsc->ElStructSchema->SsName, pAttr2->
				      AeAttrSSchema->SsName) == 0)
			    {
			       pAttr2->AeAttrSSchema = pAsc->ElStructSchema;
			       found = TRUE;
			    }
			  else
			    {
			       if (pAttr2->AeAttrSSchema->SsExtension)
				  /* l'attribut a copier est defini dans une extension */
				  if (ValidExtension (pAsc, &pAttr2->AeAttrSSchema))
				     /* cette extension s'applique au schema de l'ascendant */
				     found = TRUE;
			       if (!found)
				  pAsc = pAsc->ElParent;
			    }
		       while (pAsc != NULL && !found);
		    }
		  if (found)
		     found = CanAssociateAttr (pEl2, NULL, pAttr1, &bool);
	       }
	     if (found)
	       {
		  if (pAttr1->AeAttrType == AtReferenceAttr)
		     /* c'est un attribut reference, on copie la reference */
		    {
		       if (pAttr1->AeAttrReference == NULL)
			  pAttr2->AeAttrReference = NULL;
		       else
			 {
			    GetReference (&rf);		/* acquiert une reference */
			    pAttr2->AeAttrReference = rf;
			    /* remplit la nouvelle reference */
			    pPr = pAttr2->AeAttrReference;
			    pPr->RdElement = pEl2;
			    pPr->RdAttribute = pAttr2;
			    pPr->RdReferred = pAttr1->AeAttrReference->RdReferred;
			    pPr->RdTypeRef = pAttr1->AeAttrReference->RdTypeRef;
			    pPr->RdInternalRef = pAttr1->AeAttrReference->RdInternalRef;
			    if (pPr->RdReferred != NULL)
			       /* met la nouvelle reference en tete de chaine */
			      {
				 pPr->RdNext = pPr->RdReferred->ReFirstReference;
				 if (pPr->RdNext != NULL)
				    pPr->RdNext->RdPrevious = rf;
				 pPr->RdReferred->ReFirstReference = rf;
			      }
			 }
		    }
		  else if (pAttr2->AeAttrType == AtTextAttr)
		     /* c'est un attribut texte, on lui attache un buffer de texte */
		     /* dans lequel on copie le contenu */
		     if (pAttr1->AeAttrText != NULL)
		       {
			  GetTextBuffer (&pAttr2->AeAttrText);
			  CopyTextToText (pAttr1->AeAttrText, pAttr2->AeAttrText, &len);
		       }
		  /* chaine la copie */
		  if (pPrevAttr == NULL)
		     /* premier attribut de la copie */

		     pEl2->ElFirstAttr = pAttr2;
		  else
		     pPrevAttr->AeNext = pAttr2;
		  pPrevAttr = pAttr2;
		  pAttr2 = NULL;
	       }
	     /* passe a l'attribut suivant de l'original */
	     pAttr1 = pAttr1->AeNext;
	  }
	while (pAttr1 != NULL);
	if (pAttr2 != NULL)
	   FreeAttr (pAttr2);
     }
}


/*----------------------------------------------------------------------
   CopyPresRules  copie les regles de presentation specifique de   
   l'element pointe' par pEl dans l'element pointe' par pEl2.      
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         CopyPresRules (PtrElement pEl, PtrElement pEl2)

#else  /* __STDC__ */
static void         CopyPresRules (pEl, pEl2)
PtrElement          pEl;
PtrElement          pEl2;

#endif /* __STDC__ */

{
   PtrPRule        pRC1, pRC2, pRS;


   if (pEl->ElFirstPRule == NULL)
      pEl2->ElFirstPRule = NULL;	/* pas de presentation specifique */
   else
     {
	GetPresentRule (&pRC1);	/* acquiert une premiere regle */
	pEl2->ElFirstPRule = pRC1;
	*pRC1 = *pEl->ElFirstPRule;	/* copie la premiere regle */
	pRS = pEl->ElFirstPRule->PrNextPRule;
	while (pRS != NULL)
	  {
	     GetPresentRule (&pRC2);	/* acquiert une regle pour la copie */
	     *pRC2 = *pRS;	/* copie le contenu */
	     pRC1->PrNextPRule = pRC2;	/* chaine cette regle a la precedente */
	     pRC1 = pRC2;
	     pRS = pRS->PrNextPRule;
	  }
     }
}


/*----------------------------------------------------------------------
   ElemIsWithinSubtree retourne vrai si l'element pointe par pEl appartient 
   au sous-arbre dont la racine est pointee par pRoot.     
  ----------------------------------------------------------------------*/

#ifdef __STDC__
boolean             ElemIsWithinSubtree (PtrElement pEl, PtrElement pRoot)

#else  /* __STDC__ */
boolean             ElemIsWithinSubtree (pEl, pRoot)
PtrElement          pEl;
PtrElement          pRoot;

#endif /* __STDC__ */

{
   boolean             within;

   within = FALSE;
   while (!within && pEl != NULL)
      if (pEl == pRoot)
	 within = TRUE;
      else
	 pEl = pEl->ElParent;
   return within;
}


/*----------------------------------------------------------------------
   EquivalentType                                                          
   Teste si le type de pEl est coherent avec typeNum               
  ----------------------------------------------------------------------*/

#ifdef __STDC__
boolean             EquivalentType (PtrElement pEl, int typeNum, PtrSSchema pSS)

#else  /* __STDC__ */
boolean             EquivalentType (pEl, typeNum, pSS)
PtrElement          pEl;
int         typeNum;
PtrSSchema        pSS;

#endif /* __STDC__ */

{
   boolean             ok;
   SRule              *pRe1;
   SRule              *pRe2;
   int                 i;
   boolean             SSok;

   ok = FALSE;			/* a priori, reponse negative */
   if (pSS == NULL)
      SSok = TRUE;		/* n'importe quel schema de struct. convient */
   else
      /* compare l'identificateur du schema de structure */
      SSok = pEl->ElStructSchema->SsCode == pSS->SsCode;
   if (SSok && pEl->ElTypeNumber == typeNum)
      ok = TRUE;
   else if (pSS != NULL)
     {
	pRe1 = &pSS->SsRule[typeNum - 1];
	/* regle definissant le type d'element cherche' */
	if (pRe1->SrConstruct == CsChoice)
	  {
	     if (pRe1->SrNChoices > 0)
		/* le type cherche' est un choix explicite (alias) */
		/* l'element courant est defini dans le meme schema que */
		/* le type cherche' */
	       {
		  i = 0;
		  do
		     /* compare le type de l'element aux options du choix */
		    {
		       if (pEl->ElTypeNumber == pRe1->SrChoice[i])
			  ok = pEl->ElStructSchema->SsCode == pSS->SsCode;
		       else
			 {
			    pRe2 = &pSS->SsRule[pRe1->SrChoice[i] - 1];
			    if (pRe2->SrConstruct == CsNatureSchema)
			       /* l'option du choix est un changement de nature */
			       /* retient les elements racines de cette nature */
			       if (pEl->ElStructSchema == pRe2->SrSSchemaNat)
				  ok = pEl->ElTypeNumber == pEl->ElStructSchema->SsRootElem;
			 }
		       i++;
		    }
		  while ((!ok) && (i < pRe1->SrNChoices));

	       }
	  }
	else if (pRe1->SrConstruct == CsNatureSchema)
	  {
	     /* le type cherche' est un changement de nature, on */
	     /* retient les elements racines de cette nature */
	     if (pEl->ElStructSchema == pRe1->SrSSchemaNat)
		if (pEl->ElTypeNumber == pRe1->SrSSchemaNat->SsRootElem)
		   ok = TRUE;
	  }
     }
   return ok;
}


/*----------------------------------------------------------------------
   ElemIsBefore retourne vrai si l'element pointe' par pEl1 se trouve dans
   l'arbre avant l'element pointe par pEl2. L'element      
   lui-meme et ses ascendants ne sont pas consideres       
   comme des predecesseurs.                                
  ----------------------------------------------------------------------*/

#ifdef __STDC__
boolean             ElemIsBefore (PtrElement pEl1, PtrElement pEl2)

#else  /* __STDC__ */
boolean             ElemIsBefore (pEl1, pEl2)
PtrElement          pEl1;
PtrElement          pEl2;

#endif /* __STDC__ */

{
   PtrElement          pEl;
   boolean             found;
   boolean             avant;

   if (pEl1 == pEl2)
      avant = FALSE;
   else
     {
	pEl = pEl2;
	found = FALSE;
	while (pEl != NULL && !found)
	  {
	     while (pEl->ElPrevious != NULL)
	       {
		  pEl = pEl->ElPrevious;
		  if (ElemIsWithinSubtree (pEl1, pEl))
		     found = TRUE;
	       }
	     pEl = pEl->ElParent;
	  }
	avant = found;
     }
   return avant;
}

/*----------------------------------------------------------------------
   ElemIsAnAncestor retourne Vrai si l'element pointe par pEl1 est un	
   ascendant de l'element pointe par pEl2.			
  ----------------------------------------------------------------------*/

#ifdef __STDC__
boolean             ElemIsAnAncestor (PtrElement pEl1, PtrElement pEl2)

#else  /* __STDC__ */
boolean             ElemIsAnAncestor (pEl1, pEl2)
PtrElement          pEl1;
PtrElement          pEl2;

#endif /* __STDC__ */

{
   PtrElement          p;
   boolean             found;
   boolean             englobe;

   if (pEl1 == NULL)
      englobe = FALSE;
   else if (pEl2 == NULL)
      englobe = TRUE;
   else
     {
	found = FALSE;
	p = pEl2->ElParent;
	while (!found && p != NULL)
	   if (p == pEl1)
	      found = TRUE;
	   else
	      p = p->ElParent;
	englobe = found;
     }
   return englobe;
}

/*----------------------------------------------------------------------
   CommonAncestor trouve le plus petit sous arbre qui contient pEl1	
   pEl2.                                                   
   Retourne un pointeur sur la racine de ce sous-arbre ou  
   NULL si pEl1 et pEl2 ne sont pas dans le meme arbre.    
  ----------------------------------------------------------------------*/

#ifdef __STDC__
PtrElement          CommonAncestor (PtrElement pEl1, PtrElement pEl2)

#else  /* __STDC__ */
PtrElement          CommonAncestor (pEl1, pEl2)
PtrElement          pEl1;
PtrElement          pEl2;

#endif /* __STDC__ */

{
   PtrElement          pAsc;
   boolean             stop;

   if (pEl1 == pEl2)
      pAsc = pEl1;
   else if (ElemIsAnAncestor (pEl1, pEl2))
      pAsc = pEl1;
   else if (ElemIsAnAncestor (pEl2, pEl1))
      pAsc = pEl2;
   else
     {
	pAsc = pEl1->ElParent;
	stop = FALSE;
	while (!stop && pAsc != NULL)
	   if (ElemIsAnAncestor (pAsc, pEl2))
	      stop = TRUE;
	   else
	      pAsc = pAsc->ElParent;
     }
   return pAsc;
}

/*----------------------------------------------------------------------
   FirstLeaf retourne un pointeur sur le premier element sans    
   descendance dans le sous-arbre de l'element pointe par  
   pEl.                                                    
  ----------------------------------------------------------------------*/

#ifdef __STDC__
PtrElement          FirstLeaf (PtrElement pEl)

#else  /* __STDC__ */
PtrElement          FirstLeaf (pEl)
PtrElement          pEl;

#endif /* __STDC__ */

{
   PtrElement          pE;
   boolean             stop;
   PtrElement          pEl1;

   pE = pEl;
   if (pE != NULL)
     {
	stop = FALSE;
	do
	  {
	     pEl1 = pE;
	     if (pEl1->ElTerminal)
		if (pEl1->ElLeafType == LtPageColBreak)
		   /* on ignore les marques de page */
		   if (pEl1->ElNext != NULL)
		      pE = pEl1->ElNext;
		   else
		     {
			pE = pEl1->ElParent;
			stop = TRUE;
		     }
		else
		   stop = TRUE;
	     else if (pEl1->ElFirstChild == NULL)
		stop = TRUE;
	     else
		pE = pEl1->ElFirstChild;
	  }
	while (!stop);
     }
   return pE;
}

/*----------------------------------------------------------------------
   LastLeaf retourne un pointeur sur le dernier element sans     
   descendance dans le sous-arbre de l'element pointe par  
   pEl.                                                    
  ----------------------------------------------------------------------*/

#ifdef __STDC__
PtrElement          LastLeaf (PtrElement pRoot)

#else  /* __STDC__ */
PtrElement          LastLeaf (pRoot)
PtrElement          pRoot;

#endif /* __STDC__ */

{
   PtrElement          pEl;

   pEl = pRoot;
   if (pRoot != NULL)
      if (!pRoot->ElTerminal)
	 if (pRoot->ElFirstChild != NULL)
	   {
	      pEl = pRoot->ElFirstChild;
	      while (pEl->ElNext != NULL)
		 pEl = pEl->ElNext;
	      pEl = LastLeaf (pEl);
	   }
   return pEl;
}


/*----------------------------------------------------------------------
   GetTypedAncestor retourne un pointeur sur l'element ascendant de       
   l'element pointe par pEl et qui est du type typeNum	
   dans le schema de structure pointe par pSS. Retourne    
   NULL si un tel element n'existe pas.                    
  ----------------------------------------------------------------------*/

#ifdef __STDC__
PtrElement          GetTypedAncestor (PtrElement pEl, int typeNum, PtrSSchema pSS)

#else  /* __STDC__ */
PtrElement          GetTypedAncestor (pEl, typeNum, pSS)
PtrElement          pEl;
int         typeNum;
PtrSSchema        pSS;

#endif /* __STDC__ */

{
   boolean             found;
   PtrElement          pEl1;
   PtrElement          pAsc;

   found = FALSE;
   if (pEl != NULL && typeNum > 0 && pSS != NULL)
      do
	{
	   pEl1 = pEl;
	   if (pSS->SsRule[typeNum - 1].SrConstruct == CsNatureSchema)
	     {
		/* le type cherche' est une nature donnee */
		if (pEl1->ElTypeNumber == pEl1->ElStructSchema->SsRootElem)
		   /* l'element courant est la racine d'une nature, l'element    */
		   /* courant convient si ce sont les memes schemas de structure */
		   found = (strcmp (pEl1->ElStructSchema->SsName,
				 pSS->SsRule[typeNum - 1].SrOrigNat) == 0);
	     }
	   else
	      found = EquivalentSRules (typeNum, pSS, pEl1->ElTypeNumber, pEl1->ElStructSchema, pEl);
	   if (!found)
	      pEl = pEl->ElParent;	/* passe au niveau superieur */
	}
      while (pEl != NULL && !found);

   if (found)
     {
	/* si l'element trouve' a seulement un type equivalent au type */
	/* cherche', regarde si l'ascendant n'a pas le type cherche' */
	if (typeNum != pEl->ElTypeNumber)
	   if (pEl->ElParent != NULL)
	      if (pEl->ElParent->ElTypeNumber == typeNum)
		 if (pEl->ElParent->ElStructSchema->SsCode == pSS->SsCode)
		    pEl = pEl->ElParent;
	pAsc = pEl;
     }
   else
      pAsc = NULL;
   return pAsc;
}


/*----------------------------------------------------------------------
   FwdSearch2TypesInSubtree                                        
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static PtrElement   FwdSearch2TypesInSubtree (PtrElement pEl, boolean test, int typeNum2, int typeNum1, PtrSSchema pSS2, PtrSSchema pSS1)

#else  /* __STDC__ */
static PtrElement   FwdSearch2TypesInSubtree (pEl, test, typeNum2, typeNum1, pSS2, pSS1)
PtrElement          pEl;
boolean             test;
int         typeNum2;
int         typeNum1;
PtrSSchema        pSS2;
PtrSSchema        pSS1;

#endif /* __STDC__ */

{
   PtrElement          pRet, pChild;

   pRet = NULL;
   if (test)
     {
	if (EquivalentType (pEl, typeNum1, pSS1) || EquivalentType (pEl, typeNum2, pSS2) || typeNum1 == 0)
	   /* element found */
	   pRet = pEl;
     }
   if (pRet == NULL && !pEl->ElTerminal)
      /* search in the chidren */
     {
	pChild = pEl->ElFirstChild;
	while (pChild != NULL && pRet == NULL)
	  {
	     pRet = FwdSearch2TypesInSubtree (pChild, TRUE, typeNum2, typeNum1, pSS2, pSS1);
	     pChild = pChild->ElNext;
	  }
     }
   return pRet;
}


/*----------------------------------------------------------------------
   FwdSearchElem2Types  cherche en avant dans l'arbre, a partir de l'element
   pointe par pEl, un element du type typeNum1 (defini dans le schema  
   de structure pointe' par pSS1) ou typeNum2 (defini par le schema	
   de structure defini par pSS2). Si pSS1 ou pSS2 est NIL,		
   la recherche s'arretera sur tout element dont le numero de      
   type est typeNum1 ou typeNum2, quelque soit son schema de structure     
   (Utile pour chercher les elements de base : chaine de           
   caracteres, symboles, elements graphiques..).                   
   La fonction rend un pointeur sur l'element trouve               
   ou NULL si echec.                                               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PtrElement          FwdSearchElem2Types (PtrElement pEl, int typeNum1, int typeNum2, PtrSSchema pSS1, PtrSSchema pSS2)
#else  /* __STDC__ */
PtrElement          FwdSearchElem2Types (pEl, typeNum1, typeNum2, pSS1, pSS2)
PtrElement          pEl;
int         typeNum1;
int         typeNum2;
PtrSSchema        pSS1;
PtrSSchema        pSS2;

#endif /* __STDC__ */
{
   PtrElement          pRet, pCur, pAsc;
   boolean             stop;

   pRet = NULL;
   if (pEl != NULL)
      /* cherche dans le sous-arbre de l'element */
     {
	pRet = FwdSearch2TypesInSubtree (pEl, FALSE, typeNum2, typeNum1, pSS2, pSS1);
	if (pRet == NULL)
	   /* si echec, cherche dans les sous-arbres des freres suivants */
	   /* de l'element */
	  {
	     pCur = pEl->ElNext;
	     while (pCur != NULL && pRet == NULL)
	       {
		  pRet = FwdSearch2TypesInSubtree (pCur, TRUE, typeNum2, typeNum1, pSS2, pSS1);
		  pCur = pCur->ElNext;
	       }
	     /* si echec, cherche le premier ascendant avec un frere suivant */
	     if (pRet == NULL)
	       {
		  stop = FALSE;
		  pAsc = pEl;
		  do
		    {
		       pAsc = pAsc->ElParent;
		       if (pAsc == NULL)
			  stop = TRUE;
		       else if (pAsc->ElNext != NULL)
			  stop = TRUE;
		    }
		  while (!stop);
		  if (pAsc != NULL)
		     /* cherche si cet element est celui cherche */
		    {
		       pAsc = pAsc->ElNext;
		       if (pAsc != NULL)
			 {
			    if (EquivalentType (pAsc, typeNum1, pSS1) || EquivalentType (pAsc, typeNum2, pSS2) || typeNum1 == 0)
			       pRet = pAsc;		/* found */
			    else
			       pRet = FwdSearchElem2Types (pAsc, typeNum1, typeNum2, pSS1, pSS2);
			 }
		    }
	       }
	  }
     }
   return pRet;
}


/*----------------------------------------------------------------------
   FwdSearchTypedElem  cherche en avant dans l'arbre, a partir de l'element    
   pointe par pEl un element du type typeNum defini dans le	
   schema de structure pointe par pSS.				
   Si pSS = NULL, recherche quelque soit le schema de structure	
   schema de structure (utile pour chercher les elements de base  
   comme chaine de caracteres, symboles, elements graphiques..).  
   La fonction rend un pointeur sur l'element trouve ou NULL.     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PtrElement          FwdSearchTypedElem (PtrElement pEl, int typeNum, PtrSSchema pSS)
#else  /* __STDC__ */
PtrElement          FwdSearchTypedElem (pEl, typeNum, pSS)
PtrElement          pEl;
int         typeNum;
PtrSSchema        pSS;

#endif /* __STDC__ */
{
   return FwdSearchElem2Types (pEl, typeNum, 0, pSS, NULL);
}


/*----------------------------------------------------------------------
   BackSearch2TypesInSubtree                                      
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static PtrElement   BackSearch2TypesInSubtree (PtrElement pEl, int typeNum2, int typeNum1, PtrSSchema pSS2, PtrSSchema pSS1)

#else  /* __STDC__ */
static PtrElement   BackSearch2TypesInSubtree (pEl, typeNum2, typeNum1, pSS2, pSS1)
PtrElement          pEl;
int         typeNum2;
int         typeNum1;
PtrSSchema        pSS2;
PtrSSchema        pSS1;

#endif /* __STDC__ */

{
   PtrElement          pRet, pChild;

   pRet = NULL;
   /* Cherche le dernier fils */
   if (!pEl->ElTerminal)
     {
	pChild = pEl->ElFirstChild;
	if (pChild != NULL)

	   while (pChild->ElNext != NULL)
	      pChild = pChild->ElNext;
	/* on cherche a partir des freres precedents */
	while (pChild != NULL && pRet == NULL)
	  {
	     pRet = BackSearch2TypesInSubtree (pChild, typeNum2, typeNum1, pSS2, pSS1);
	     pChild = pChild->ElPrevious;
	  }
     }
   if (pRet == NULL)
     {
	if (EquivalentType (pEl, typeNum1, pSS1) || EquivalentType (pEl, typeNum2, pSS2) || typeNum1 == 0)
	   pRet = pEl;		/* found */
     }
   return pRet;
}

/*----------------------------------------------------------------------
   BackSearchElem2Types  cherche en arriere dans l'arbre, a partir de	
   l'element pEl, un element du type typeNum1 (defini dans le	
   schema de structure pSS1) ou typeNum2 (defini dans le schema	
   	de structure pSS2). Si pSS1 (ou pSS2) est NULL, la recherche	
   	s'arrete sur tout element dont le numero de type est typeNum1	
   	(ou typeNum2), quel que soit son schema de structure		
   Utile pour chercher les elements de base : chaine de caracteres,
   symboles, elements graphiques..).                               
   La fonction rend un pointeur sur l'element trouve ou NULL       
   si echec.                                                       
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PtrElement          BackSearchElem2Types (PtrElement pEl, int typeNum1, int typeNum2, PtrSSchema pSS1, PtrSSchema pSS2)
#else  /* __STDC__ */
PtrElement          BackSearchElem2Types (pEl, typeNum1, typeNum2, pSS1, pSS2)
PtrElement          pEl;
int         typeNum1;
int         typeNum2;
PtrSSchema        pSS1;
PtrSSchema        pSS2;

#endif /* __STDC__ */
{
   PtrElement          pRet, pCur;

   pRet = NULL;			/* pRet: futur resultat de la recherche */
   if (pEl != NULL)
      /* cherche dans les sous-arbres des freres precedents de l'element */
     {
	pCur = pEl->ElPrevious;
	while (pCur != NULL && pRet == NULL)
	  {
	     pRet = BackSearch2TypesInSubtree (pCur, typeNum2, typeNum1, pSS2, pSS1);
	     pCur = pCur->ElPrevious;
	  }
	/* si echec, cherche si c'est le pere, puis cherche dans les */
	/* sous-arbres des oncles de l'element */
	if (pRet == NULL)
	   if (pEl->ElParent != NULL)
	     {
		pEl = pEl->ElParent;
		if (EquivalentType (pEl, typeNum1, pSS1) || EquivalentType (pEl, typeNum2, pSS2))
		   pRet = pEl;	/* found, c'est le pere */
		else
		   pRet = BackSearchElem2Types (pEl, typeNum1, typeNum2, pSS1, pSS2);
	     }
     }
   return pRet;
}


/*----------------------------------------------------------------------
   BackSearchTypedElem  cherche en arriere dans l'arbre, a partir de		
   l'element pEl, un element du type typeNum, defini dans le	
   schema de structure pSS. Si pSS est NULL la recherche		
   s'arrete sur tout element dont le numero de type est typeNum,	
   quel que soit son schema de structure				
   La fonction rend un pointeur sur l'element trouve               
   ou NULL si echec.                                               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PtrElement          BackSearchTypedElem (PtrElement pEl, int typeNum, PtrSSchema pSS)
#else  /* __STDC__ */
PtrElement          BackSearchTypedElem (pEl, typeNum, pSS)
PtrElement          pEl;
int         typeNum;
PtrSSchema        pSS;

#endif /* __STDC__ */
{
   return BackSearchElem2Types (pEl, typeNum, 0, pSS, NULL);
}


/*----------------------------------------------------------------------
   BackSearchVisibleElem      cherche dans le sous-arbre dont la racine est
   pointee par pRoot un element qui soit avant l'element pointe    
   par pEl et qui ait un pave dans la vue view.                    
   Retourne un pointeur sur un tel elt ou NULL s'il n'y en a pas.  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PtrElement          BackSearchVisibleElem (PtrElement pRoot, PtrElement pEl, int view)
#else  /* __STDC__ */
PtrElement          BackSearchVisibleElem (pRoot, pEl, view)
PtrElement          pRoot;
PtrElement          pEl;
int                 view;

#endif /* __STDC__ */
{
   PtrElement          pRet, pCur;

   pRet = NULL;
   if (pEl != NULL)
      /* cherche dans les sous-arbres des freres precedents de l'element */
     {
	pCur = pEl->ElPrevious;
	while (pCur != NULL && pRet == NULL)
	  {
	     pRet = BackSearchVisibleSubtree (pCur, &view);
	     pCur = pCur->ElPrevious;
	  }
	/* si echec, cherche dans les sous-arbres des oncles de l'element */
	if (pRet == NULL)
	   if (pEl->ElParent != NULL && pEl->ElParent != pRoot)
	      pRet = BackSearchVisibleElem (pRoot, pEl->ElParent, view);
     }
   return pRet;
}


/*----------------------------------------------------------------------
   FwdSearchAttribute  cherche en avant dans l'arbre, a partir de	
   l'element pEl, un element ayant l'attribut attrNum defini dans	
   le schema de structure pSS et ayant la valeur val.		
   Si pSS est NIL,la recherche s'arrete sur le 1er element ayant	
   un attribut, quel que soit cet attribut.                        
   Si val=0, la recherche s'arrete sur le premier element ayant	
   l'attribut attrNum, quelle que soit sa valeur.			
   La fonction rend un pointeur sur l'element trouve' ou NULL	
   si echec.							
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PtrElement          FwdSearchAttribute (PtrElement pEl, int attrNum, int val, char *textVal, PtrSSchema pSS)
#else  /* __STDC__ */
PtrElement          FwdSearchAttribute (pEl, attrNum, val, textVal, pSS)
PtrElement          pEl;
int      attrNum;
int       val;
char               *textVal;
PtrSSchema        pSS;

#endif /* __STDC__ */
{
   PtrElement          pRet, pCur, pAsc;
   boolean             stop;

   pRet = NULL;
   if (pEl != NULL)
      /* cherche dans le sous-arbre de l'element */
     {
	pRet = FwdSearchAttrInSubtree (pEl, FALSE, pSS, attrNum, val, textVal);
	if (pRet == NULL)
	   /* si echec, cherche dans les sous-arbres des freres suivants */
	   /* de l'element */
	  {
	     pCur = pEl->ElNext;
	     while (pCur != NULL && pRet == NULL)
	       {
		  pRet = FwdSearchAttrInSubtree (pCur, TRUE, pSS, attrNum, val, textVal);
		  pCur = pCur->ElNext;
	       }
	     /* si echec, cherche le premier ascendant avec un frere suivant */
	     if (pRet == NULL)
	       {
		  stop = FALSE;
		  pAsc = pEl;
		  do
		    {
		       pAsc = pAsc->ElParent;
		       if (pAsc == NULL)
			  stop = TRUE;
		       else if (pAsc->ElNext != NULL)
			  stop = TRUE;
		    }
		  while (!stop);
		  if (pAsc != NULL)
		     /* cherche si cet element est celui cherche */
		    {
		       pAsc = pAsc->ElNext;
		       if (pAsc != NULL)
			  if (AttrFound (pAsc, textVal, val, attrNum, pSS))
			     pRet = pAsc;	/* found */
			  else
			     pRet = FwdSearchAttribute (pAsc, attrNum, val, textVal, pSS);
		    }
	       }
	  }
     }
   return pRet;
}


/*----------------------------------------------------------------------
   BackSearchAttribute cherche en arriere dans l'arbre, a partir de	
   l'element pEl, un element ayant l'attribut attNum defini dans	
   le schema de structure pSS et ayant la valeur val.		
   Si pSS est NIL, la recherche s'arretera sur le 1er elt ayant	
   un attribut, quel que soit cet attribut.                        
   Si val=0,  la recherche s'arretera sur le  1er elt ayant        
   l'attribut attNum, quelle que soit sa valeur.			
   La fonction rend un pointeur sur l'element trouve ou NULL	
   si echec.							
  ----------------------------------------------------------------------*/

#ifdef __STDC__
PtrElement          BackSearchAttribute (PtrElement pEl, int attNum, int val, char *textVal, PtrSSchema pSS)

#else  /* __STDC__ */
PtrElement          BackSearchAttribute (pEl, attNum, val, textVal, pSS)
PtrElement          pEl;
int      attNum;
int       val;
char               *textVal;
PtrSSchema        pSS;

#endif /* __STDC__ */

{
   PtrElement          pRet, pCur;

   pRet = NULL;
   if (pEl != NULL)
      /* cherche dans les sous-arbres des freres precedents de l'element */
     {
	pCur = pEl->ElPrevious;
	while (pCur != NULL && pRet == NULL)
	  {
	     pRet = BackSearchAttrInSubtree (pCur, textVal, val, attNum, pSS);
	     pCur = pCur->ElPrevious;
	  }
	/* si echec, cherche dans les sous-arbres des oncles de l'element */
	if (pRet == NULL)
	   if (pEl->ElParent != NULL)
	      pRet = BackSearchAttribute (pEl->ElParent, attNum, val, textVal, pSS);
     }
   return pRet;
}

/*----------------------------------------------------------------------
   FwdSkipPageBreak si pEl pointe sur une marque de page, retourne    
   dans pEl un pointeur sur le premier element qui suit et qui n'est 
   pas une marque de page. Retourne NULL si pEl pointe sur une marque
   de page qui n'est suivie d'aucun element ou seulement par des     
   marques de page.                                                  
   Ne fait rien si pEl ne pointe pas une marque de page a l'appel.   
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                FwdSkipPageBreak (PtrElement * pEl)

#else  /* __STDC__ */
void                FwdSkipPageBreak (pEl)
PtrElement         *pEl;

#endif /* __STDC__ */

{
   boolean             stop;

   stop = FALSE;
   do
     {
	if (*pEl == NULL)
	   stop = TRUE;
	else if (!(*pEl)->ElTerminal)
	   stop = TRUE;
	else if ((*pEl)->ElLeafType != LtPageColBreak)
	   stop = TRUE;
	if (!stop)
	   *pEl = (*pEl)->ElNext;
     }
   while (!stop);
}


/*----------------------------------------------------------------------
   SkipPageBreakBegin  si pEl pointe sur une marque de page de debut,    
   retourne dans pEl un pointeur sur le dernier element qui precede  
   un element qui n'est pas une marque de page de debut.           
   Ne fait rien si pEl ne pointe pas une marque de page de debut     
   a l'appel.                                                      
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                SkipPageBreakBegin (PtrElement * pEl)

#else  /* __STDC__ */
void                SkipPageBreakBegin (pEl)
PtrElement         *pEl;

#endif /* __STDC__ */

{
   boolean             stop;
   PtrElement          pPrev;

   stop = FALSE;
   pPrev = *pEl;
   do
     {
	if (*pEl == NULL)
	   stop = TRUE;
	else if (!(*pEl)->ElTerminal)
	   stop = TRUE;
	else if ((*pEl)->ElLeafType != LtPageColBreak
		 || (*pEl)->ElPageType != PgBegin)
	   stop = TRUE;
	if (!stop)
	  {
	     pPrev = *pEl;
	     *pEl = (*pEl)->ElNext;
	  }
     }
   while (!stop);
   *pEl = pPrev;
}



/*----------------------------------------------------------------------
   BackSkipPageBreak       si pEl pointe sur une marque de page,     
   retourne dans pEl un pointeur sur le premier element qui precede  
   et qui n'est pas une marque de page. Retourne NULL si pEl pointe  
   sur une marque de page qui n'est precedee que de marques        
   de page.                                                        
   Ne fait rien si pEl ne pointe pas une marque de page a l'appel.   
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                BackSkipPageBreak (PtrElement * pEl)

#else  /* __STDC__ */
void                BackSkipPageBreak (pEl)
PtrElement         *pEl;

#endif /* __STDC__ */

{
   boolean             stop;

   stop = FALSE;
   do
     {
	if (*pEl == NULL)
	   stop = TRUE;
	else if (!(*pEl)->ElTerminal)
	   stop = TRUE;
	else if ((*pEl)->ElLeafType != LtPageColBreak)
	   stop = TRUE;
	if (!stop)
	   *pEl = (*pEl)->ElPrevious;
     }
   while (!stop);
}


/*----------------------------------------------------------------------
   NextElement      retourne un pointeur sur l'element qui se trouve
   apres l'element pointe par pEl.                                 
  ----------------------------------------------------------------------*/

#ifdef __STDC__
PtrElement          NextElement (PtrElement pEl)

#else  /* __STDC__ */
PtrElement          NextElement (pEl)
PtrElement          pEl;

#endif /* __STDC__ */

{
   if (pEl == NULL)
      return NULL;
   while (pEl->ElNext == NULL && pEl->ElParent != NULL)
      pEl = pEl->ElParent;
   return pEl->ElNext;
}

/*----------------------------------------------------------------------
   PreviousLeaf       retourne un pointeur sur la premiere    
   feuille qui precede l'element pEl.                              
  ----------------------------------------------------------------------*/

#ifdef __STDC__
PtrElement          PreviousLeaf (PtrElement pEl)

#else  /* __STDC__ */
PtrElement          PreviousLeaf (pEl)
PtrElement          pEl;

#endif /* __STDC__ */

{
   PtrElement          pEl1;

   pEl1 = pEl;
   if (pEl1 != NULL)
     {
	while (pEl1->ElPrevious == NULL && pEl1->ElParent != NULL)
	   pEl1 = pEl1->ElParent;
	pEl1 = pEl1->ElPrevious;
	if (pEl1 != NULL)
	   while (!pEl1->ElTerminal && pEl1->ElFirstChild != NULL)
	     {
		pEl1 = pEl1->ElFirstChild;
		while (pEl1->ElNext != NULL)
		   pEl1 = pEl1->ElNext;
	     }
     }
   return pEl1;
}


/*----------------------------------------------------------------------
   NextLeaf         retourne un pointeur sur la premiere    
   feuille qui suit l'element pEl.                                 
  ----------------------------------------------------------------------*/

#ifdef __STDC__
PtrElement          NextLeaf (PtrElement pEl)

#else  /* __STDC__ */
PtrElement          NextLeaf (pEl)
PtrElement          pEl;

#endif /* __STDC__ */

{
   PtrElement          pEl1;

   pEl1 = pEl;
   if (pEl1 != NULL)
     {
	while (pEl1->ElNext == NULL && pEl1->ElParent != NULL)
	   pEl1 = pEl1->ElParent;
	pEl1 = pEl1->ElNext;
	if (pEl1 != NULL)
	   while (!pEl1->ElTerminal && pEl1->ElFirstChild != NULL)
	      pEl1 = pEl1->ElFirstChild;
     }
   return pEl1;
}


/*----------------------------------------------------------------------
   InsertElementBefore insere l'element pointe par pNew (et ses freres	
   suivants), avant l'element pointe par pOld dans l'arbre abstrait
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                InsertElementBefore (PtrElement pOld, PtrElement pNew)

#else  /* __STDC__ */
void                InsertElementBefore (pOld, pNew)
PtrElement          pOld;
PtrElement          pNew;

#endif /* __STDC__ */

{
   PtrElement          pE;
   PtrElement          pAsc;

   if (pNew != NULL && pOld != NULL)
     {
	pNew->ElParent = pOld->ElParent;
	pNew->ElPrevious = pOld->ElPrevious;
	if (pNew->ElPrevious != NULL)
	   pNew->ElPrevious->ElNext = pNew;
	if (pNew->ElParent != NULL)
	   if (pNew->ElParent->ElFirstChild == pOld)
	      pNew->ElParent->ElFirstChild = pNew;
	/* met a jour le volume des elements englobants */
	pAsc = pNew->ElParent;
	while (pAsc != NULL)
	  {
	     pAsc->ElVolume = pAsc->ElVolume + pNew->ElVolume;
	     pAsc = pAsc->ElParent;
	  }
	pE = pNew;
	/* fait heriter aux feuilles de texte de l'element la langue */
	/* definie par le premier element englobant */
	LeavesInheritLanguage (pE);
	while (pE->ElNext != NULL)
	  {
	     pE = pE->ElNext;
	     pE->ElParent = pOld->ElParent;
	     /* fait heriter aux feuilles de texte de l'element la langue */
	     /* definie par le premier element englobant */
	     LeavesInheritLanguage (pE);
	     /* met a jour le volume des elements englobants */
	     pAsc = pE->ElParent;
	     while (pAsc != NULL)
	       {
		  pAsc->ElVolume = pAsc->ElVolume + pE->ElVolume;
		  pAsc = pAsc->ElParent;
	       }
	  }
	pE->ElNext = pOld;
	pOld->ElPrevious = pE;
     }
}


/*----------------------------------------------------------------------
   InsertElementAfter insere l'element pointe par pNew (et ses freres     
   suivants), apres l'element pointe par pOld dans l'arbre       
   abstrait.                                                       
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                InsertElementAfter (PtrElement pOld, PtrElement pNew)

#else  /* __STDC__ */
void                InsertElementAfter (pOld, pNew)
PtrElement          pOld;
PtrElement          pNew;

#endif /* __STDC__ */

{
   PtrElement          pE;
   PtrElement          pAsc;

   if (pNew != NULL && pOld != NULL)
     {
	pNew->ElParent = pOld->ElParent;
	pNew->ElPrevious = pOld;
	/* met a jour le volume des elements englobants */
	pAsc = pNew->ElParent;
	while (pAsc != NULL)
	  {
	     pAsc->ElVolume = pAsc->ElVolume + pNew->ElVolume;
	     pAsc = pAsc->ElParent;
	  }
	pE = pNew;
	/* fait heriter aux feuilles de texte de l'element la langue */
	/* definie par le premier element englobant */
	LeavesInheritLanguage (pE);
	while (pE->ElNext != NULL)
	  {
	     pE = pE->ElNext;
	     pE->ElParent = pOld->ElParent;
	     /* fait heriter aux feuilles de texte de l'element la langue */
	     /* definie par le premier element englobant */
	     LeavesInheritLanguage (pE);
	     /* met a jour le volume des elements englobants */
	     pAsc = pE->ElParent;
	     while (pAsc != NULL)
	       {
		  pAsc->ElVolume = pAsc->ElVolume + pE->ElVolume;
		  pAsc = pAsc->ElParent;
	       }
	  }
	pE->ElNext = pOld->ElNext;
	if (pOld->ElNext != NULL)
	   pOld->ElNext->ElPrevious = pE;
	pOld->ElNext = pNew;
     }
}



/*----------------------------------------------------------------------
   InsertFirstChild ajoute l'element pointe par pNew (et ses freres   
   suivants), comme premier fils de l'element pointe par pOld    
   dans l'arbre abstrait.                                          
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                InsertFirstChild (PtrElement pOld, PtrElement pNew)

#else  /* __STDC__ */
void                InsertFirstChild (pOld, pNew)
PtrElement          pOld;
PtrElement          pNew;

#endif /* __STDC__ */

{
   PtrElement          pE;
   PtrElement          pAsc;

   if (pNew != NULL && pOld != NULL)
     {
	if (pOld->ElFirstChild != NULL)
	   InsertElementBefore (pOld->ElFirstChild, pNew);
	else
	  {
	     pOld->ElFirstChild = pNew;
	     pE = pNew;
	     while (pE != NULL)
	       {
		  pE->ElParent = pOld;
		  /* fait heriter aux feuilles de texte de l'element la langue */
		  /* definie par le premier element englobant */
		  LeavesInheritLanguage (pE);
		  /* met a jour le volume des elements englobants */
		  pAsc = pE->ElParent;
		  while (pAsc != NULL)
		    {
		       pAsc->ElVolume = pAsc->ElVolume + pE->ElVolume;
		       pAsc = pAsc->ElParent;
		    }
		  pE = pE->ElNext;
	       }
	  }
     }
}

/*----------------------------------------------------------------------
   InsertElemInChoice   met le nouvel element pNew a la place de l'element   
   de type CHOIX pointe par pEl, sauf si celui-ci est un element	
   d'agregat ou la racine du schema de structure ou un element	
   associe.                                                        
   Dans ce cas, le nouvel element est chaine' comme 1er fils de    
   l'element pEl.							
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                InsertElemInChoice (PtrElement pEl, PtrElement * pNew, boolean del)

#else  /* __STDC__ */
void                InsertElemInChoice (pEl, pNew, del)
PtrElement          pEl;
PtrElement         *pNew;
boolean             del;

#endif /* __STDC__ */

{
   boolean             replace;
   PtrTextBuffer      pTB;
   PtrAttribute         pAttr;
   PtrSSchema        pSS;
   int                 T;
   PtrElement          pE;
   SRule              *pSRule;

   /* le nouvel element remplacera l'element CHOIX */
   replace = TRUE;
   if (pEl->ElParent != NULL)
      /* ...sauf si le choix est un element d'agregat */
	if (pEl->ElParent->ElStructSchema->SsRule[pEl->ElParent->ElTypeNumber - 1].SrConstruct == CsAggregate ||
	    pEl->ElParent->ElStructSchema->SsRule[pEl->ElParent->ElTypeNumber - 1].SrConstruct == CsUnorderedAggregate)
	   replace = FALSE;
   /* sauf si le choix porte des exceptions */
   pSRule = &pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1];
   if (pSRule->SrNInclusions > 0 || pSRule->SrNExclusions > 0)
      replace = FALSE;
   /* sauf si c'est la racine du schema de structure */
   if (pEl->ElTypeNumber == pEl->ElStructSchema->SsRootElem)
      replace = FALSE;
   /* sauf si c'est la racine d'un element associe */
   if (pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrAssocElem)
      replace = FALSE;
   if (!replace)
      if (pEl->ElTypeNumber == (*pNew)->ElTypeNumber)
	 if (pEl->ElStructSchema->SsCode == (*pNew)->ElStructSchema->SsCode)
	    /* les deux elements sont de meme type; l'un remplacera l'autre */
	    replace = TRUE;
   if (del)
      replace = TRUE;
   if (!replace)
      /* ajoute l'element cree comme fils de l'element choix */
      InsertFirstChild (pEl, *pNew);
   else
      /* met le nouvel element a la place de l'element CsChoice */
     {
	/* Echange les types des deux elements */
	pSS = pEl->ElStructSchema;
	T = pEl->ElTypeNumber;
	pEl->ElStructSchema = (*pNew)->ElStructSchema;
	pEl->ElTypeNumber = (*pNew)->ElTypeNumber;
	(*pNew)->ElStructSchema = pSS;
	(*pNew)->ElTypeNumber = T;
	if ((*pNew)->ElReferredDescr != NULL)
	   /* supprime toutes les references a l'element CHOIX et le */
	   /* descripteur d'element reference' de l'element CHOIX */
	  {
	     DeleteAllReferences (pEl);
	     DeleteReferredElDescr (pEl->ElReferredDescr);
	     /* recupere le descripteur d'element reference' du nouvel elem. */
	     pEl->ElReferredDescr = (*pNew)->ElReferredDescr;
	     (*pNew)->ElReferredDescr = NULL;
	     if (!pEl->ElReferredDescr->ReExternalRef)
		if (pEl->ElReferredDescr->ReReferredElem == *pNew)
		   pEl->ElReferredDescr->ReReferredElem = pEl;
	  }
	/* ajoute les attributs de pNew a ceux de pEl */
	if (pEl->ElFirstAttr == NULL)
	   /* pEl n'a pas d'attributs */
	   pEl->ElFirstAttr = (*pNew)->ElFirstAttr;
	else
	   /* cherche le dernier attribut de pEl */
	  {
	     pAttr = pEl->ElFirstAttr;
	     while (pAttr->AeNext != NULL)
		pAttr = pAttr->AeNext;
	     /* chaine les attributs de pNew^ apres ceux de pEl^ */
	     pAttr->AeNext = (*pNew)->ElFirstAttr;
	  }
	/* pNew^ n'a plus d'attributs */
	(*pNew)->ElFirstAttr = NULL;
	if ((*pNew)->ElSource != NULL)
	  {
	     pEl->ElSource = (*pNew)->ElSource;
	     (*pNew)->ElSource = NULL;
	     if (pEl->ElSource != NULL)
		pEl->ElSource->RdElement = pEl;
	  }
	pEl->ElTerminal = (*pNew)->ElTerminal;
	if (pEl->ElTerminal)
	  {
	     pEl->ElLeafType = (*pNew)->ElLeafType;
	     switch (pEl->ElLeafType)
		   {
		      case LtPicture:
			 pEl->ElPictInfo = (*pNew)->ElPictInfo;
		      case LtText:
			 pEl->ElText = (*pNew)->ElText;
			 pEl->ElTextLength = (*pNew)->ElTextLength;
			 if (pEl->ElLeafType == LtText)
			   {
			      pEl->ElVolume = pEl->ElTextLength;
			      pEl->ElLanguage = (*pNew)->ElLanguage;
			   }
			 /* ajoute le volume de l'element a celui de ses ascendants */

			 if (pEl->ElVolume != 0)
			   {
			      pE = pEl->ElParent;
			      while (pE != NULL)
				{
				   pE->ElVolume = pE->ElVolume + pEl->ElVolume;
				   pE = pE->ElParent;
				}
			   }
			 LeavesInheritLanguage (pEl);
			 /* attache les buffers de texte de pNew a pEl */
			 pTB = pEl->ElText;
			 while (pTB != NULL)
			    pTB = pTB->BuNext;
			 (*pNew)->ElText = NULL;
			 (*pNew)->ElTextLength = 0;
			 break;
		      case LtPlyLine:
			 pEl->ElPolyLineBuffer = (*pNew)->ElPolyLineBuffer;
			 pEl->ElNPoints = (*pNew)->ElNPoints;
			 pEl->ElVolume = pEl->ElNPoints;
			 /* ajoute le volume de l'element a celui de ses ascendants */
			 if (pEl->ElVolume != 0)
			   {
			      pE = pEl->ElParent;
			      while (pE != NULL)
				{
				   pE->ElVolume = pE->ElVolume + pEl->ElVolume;
				   pE = pE->ElParent;
				}
			   }
			 /* attache les buffers de texte de pNew a pEl */
			 pTB = pEl->ElPolyLineBuffer;
			 while (pTB != NULL)
			    pTB = pTB->BuNext;
			 (*pNew)->ElPolyLineBuffer = NULL;
			 (*pNew)->ElNPoints = 0;
			 break;
		      case LtSymbol:
		      case LtGraphics:
		      case LtCompound:
			 pEl->ElGraph = (*pNew)->ElGraph;
			 break;
		      case LtPageColBreak:
			 pEl->ElPageModified = (*pNew)->ElPageModified;
			 pEl->ElPageType = (*pNew)->ElPageType;
			 pEl->ElPageNumber = (*pNew)->ElPageNumber;
			 pEl->ElViewPSchema = (*pNew)->ElViewPSchema;
			 break;
		      case LtReference:
			 pEl->ElReference = (*pNew)->ElReference;
			 pEl->ElReference->RdElement = pEl;
			 (*pNew)->ElReference = NULL;
			 break;
		      case LtPairedElem:
			 pEl->ElPairIdent = (*pNew)->ElPairIdent;
			 pEl->ElOtherPairedEl = (*pNew)->ElOtherPairedEl;
			 if (pEl->ElOtherPairedEl != NULL)
			    pEl->ElOtherPairedEl->ElOtherPairedEl = pEl;
			 break;
		   }

	  }
	else if ((*pNew)->ElFirstChild != NULL)
	  {
	     InsertFirstChild (pEl, (*pNew)->ElFirstChild);
	     (*pNew)->ElFirstChild = NULL;
	  }
	if (!del)
	   DeleteElement (pNew);
	*pNew = pEl;
     }
}

/*----------------------------------------------------------------------
   AttachRequiredAttributes     met sur l'element pointe' par pEl les   
   attributs impose's indique's dans la regle pRe1 du schema de    
   structure pSS.                                                  
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                AttachRequiredAttributes (PtrElement pEl, SRule * pSRule, PtrSSchema pSS, boolean withAttr, PtrDocument pDoc)

#else  /* __STDC__ */
void                AttachRequiredAttributes (pEl, pSRule, pSS, withAttr, pDoc)
PtrElement          pEl;
SRule              *pSRule;
PtrSSchema        pSS;
boolean             withAttr;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   int                 i;
   int      att;
   PtrAttribute         pAttr;
   PtrReference        ref;
   int                 l;

   for (i = 0; i < pSRule->SrNDefAttrs; i++)
     {
	if (pSRule->SrDefAttrModif[i] && !withAttr)
	   /* on est en lecture de pivot, on ne cree pas les attributs */
	   /* avec valeur par defaut */
	   continue;

	att = pSRule->SrDefAttr[i];
	GetAttr (&pAttr);	/* acquiert un bloc attribut */
	/* chaine ce bloc en tete de la liste des attributs de */
	/* l'element */
	pAttr->AeNext = pEl->ElFirstAttr;
	pEl->ElFirstAttr = pAttr;
	/* remplit le bloc attribut */
	pAttr->AeAttrSSchema = pSS;
	pAttr->AeAttrNum = att;
	if (!pSRule->SrDefAttrModif[i])	/* attribut a valeur fixe */
	   pAttr->AeDefAttr = TRUE;
	pAttr->AeAttrType = pSS->SsAttribute[att - 1].AttrType;
	switch (pAttr->AeAttrType)
	      {
		 case AtEnumAttr:
		 case AtNumAttr:
		    pAttr->AeAttrValue = pSRule->SrDefAttrValue[i];
		    break;
		 case AtTextAttr:
		    GetTextBuffer (&pAttr->AeAttrText);
		    CopyStringToText (pSS->SsConstBuffer + pSRule->SrDefAttrValue[i] - 1,
					  pAttr->AeAttrText, &l);
		    break;
		 case AtReferenceAttr:
		    /* acquiert un descripteur de reference */
		    GetReference (&ref);
		    pAttr->AeAttrReference = ref;
		    pAttr->AeAttrReference->RdElement = pEl;
		    pAttr->AeAttrReference->RdAttribute = pAttr;
		    break;
	      }
     }
}

/*----------------------------------------------------------------------
   NewSubtree cree un sous-arbre et rend un pointeur sur le        
   sous-arbre cree. Si le booleen Desc vaut vrai, tout le          
   sous-arbre est creee d'apres le schema de structure, sinon      
   seule la racine du sous-arbre est creee.                        
   Si Root est vrai, on cree l'element racine du sous-arbre,	
   sinon on cree sa descendance et la fonction retourne un pointeur
   sur le premier descendant.                                      
   typeNum: Numero dans le schema de structure de la regle		
   definissant le type du sous-arbre a creer.                      
   pSS: Pointeur sur le schema de structure correspondant au       
   sous-arbre a creer.                                             
   pDoc: Pointeur sur le descripteur de document auquel            
   appartiendra le sous-arbre a creer.                             
   assocNum: Numero de la liste d'elements associes a laquelle	
   appartient le sous-arbre a creer. Zero si pas element asocie.   
   withAttr : si vrai, on cree les elements du sous-arbre		
   avec leurs attributs a valeur par defaut, (cas de la creation)  
   sinon on ne les cree pas (cas de la lecture d'un pivot)         
   withLabel indique s'il faut affecter a l'element un nouveau     
   label.                                                          
  ----------------------------------------------------------------------*/

#ifdef __STDC__
PtrElement          NewSubtree (int typeNum, PtrSSchema pSS, PtrDocument pDoc, int assocNum, boolean Desc, boolean Root, boolean withAttr, boolean withLabel)

#else  /* __STDC__ */
PtrElement          NewSubtree (typeNum, pSS, pDoc, assocNum, Desc, Root, withAttr, withLabel)
int         typeNum;
PtrSSchema        pSS;
PtrDocument         pDoc;
int                 assocNum;
boolean             Desc;
boolean             Root;
boolean             withAttr;
boolean             withLabel;

#endif /* __STDC__ */

{
   PtrElement          pEl, t1, t2;
   int                 i;
   boolean             gener, create, error;
   PtrReference        ref;
   Name                 PSchName;
   SRule              *pSRule;
   PtrTextBuffer      pBu1;
   SRule              *pSRule2;
   PtrSSchema        pExtSSch;
   SRule              *pExtRule;

   pEl = NULL;			/* on n'a pas (encore) cree d'element */
   if (Root)
      /* regle definissant le type de l'element */
     {
	error = FALSE;
	pSRule = &pSS->SsRule[typeNum - 1];
	if (pSRule->SrConstruct == CsNatureSchema)
	   /* C'est une regle de changement de nature, on va creer un */
	   /* element du type de la racine de la nature introduite */
	  {
	     if (pSRule->SrSSchemaNat == NULL)
		/* Schema de nature non charge' */
		/* charge le schema de nature */
	       {
		  PSchName[0] = '\0';
		  LoadNatureSchema (pSS, PSchName, typeNum);
	       }
	     if (pSRule->SrSSchemaNat == NULL)
		/* echec chargement schema */
		error = TRUE;
	     else
		/* le schema de nature est charge' */
		/* il y a un objet de plus de cette nature */
	       {
		  /* change de schema de structure */
		  pSS = pSRule->SrSSchemaNat;
		  /* type de la racine du nouveau schema de structure */
		  typeNum = pSS->SsRootElem;
	       }
	  }
	pSRule = &pSS->SsRule[typeNum - 1];
	if (!error)
	   if (pSRule->SrParamElem)
	      pEl = CreateParameter (typeNum, pSS, pDoc, assocNum);
	   else
	      GetElement (&pEl);	/* acquiert un element */
	if (pEl != NULL)
	  {
	     if (typeNum == pSS->SsRootElem)
		/* on cree un element construit selon la regle racine de */
		/* son schema de structure, on incremente le compteur */
		pSS->SsNObjects++;
	     pEl->ElStructSchema = pSS;
	     pEl->ElTypeNumber = typeNum;
	     pEl->ElAssocNum = assocNum;
	     if (withLabel)
		/* calcule la valeur du label */
		LabelIntToString (NewLabel (pDoc), pEl->ElLabel);
	     /* met les valeurs des attributs imposes definis dans le */
	     /* schema de structure definissant l'element */
	     AttachRequiredAttributes (pEl, pSRule, pSS, withAttr, pDoc);
	     /* met les valeurs des attributs imposes definis dans les */
	     /* extensions du schema de structure definissant l'element */
	     pExtSSch = pSS->SsNextExtens;
	     while (pExtSSch != NULL)
	       {
		  pExtRule = ExtensionRule (pSS, typeNum, pExtSSch);
		  if (pExtRule != NULL)
		     AttachRequiredAttributes (pEl, pExtRule, pExtSSch, withAttr, pDoc);
		  pExtSSch = pExtSSch->SsNextExtens;
	       }

	     /* chaine les blocs specifiques du constructeur */
	     switch (pSRule->SrConstruct)
		   {
		      case CsReference:
			 pEl->ElTerminal = TRUE;
			 pEl->ElLeafType = LtReference;
			 /* acquiert un descripteur de reference */
			 GetReference (&ref);
			 pEl->ElReference = ref;
			 pEl->ElReference->RdElement = pEl;
			 break;
		      case CsPairedElement:
			 pEl->ElTerminal = TRUE;
			 pEl->ElLeafType = LtPairedElem;
			 if (pSRule->SrFirstOfPair)
			    pDoc->DocMaxPairIdent++;
			 pEl->ElPairIdent = pDoc->DocMaxPairIdent;
			 pEl->ElOtherPairedEl = NULL;
			 break;
		      case CsBasicElement:
			 pEl->ElTerminal = TRUE;
			 switch (pSRule->SrBasicType)
			       {
				  case CharString:
				     CreateTextBuffer (pEl);
				     pEl->ElLeafType = LtText;
				     pEl->ElLanguage = TtaGetDefaultLanguage ();
				     pEl->ElVolume = 0;
				     pEl->ElTextLength = 0;
				     break;
				  case Picture:
				     CreateTextBuffer (pEl);
				     pEl->ElLeafType = LtPicture;
				     pEl->ElVolume = 0;
				     pEl->ElPictInfo = NULL;
				     break;
				  case GraphicElem:
				     pEl->ElLeafType = LtGraphics;
				     pEl->ElGraph = '\0';
				     pEl->ElVolume = 0;
				     break;
				  case Symbol:
				     pEl->ElLeafType = LtSymbol;
				     pEl->ElGraph = '\0';
				     pEl->ElVolume = 0;
				     break;
				  case PageBreak:
				     pEl->ElLeafType = LtPageColBreak;
				     pEl->ElPageType = PgComputed;
				     pEl->ElPageNumber = 1;
				     pEl->ElPageNumber = 0;
				     pEl->ElViewPSchema = 0;
				     pEl->ElPageModified = FALSE;
				     pEl->ElAssocHeader = TRUE;
				     pEl->ElVolume = 10;
				     break;
				  case Refer:
				     break;
				  default:
				     break;
			       }
			 break;
		      case CsConstant:
			 pEl->ElTerminal = TRUE;
			 CreateTextBuffer (pEl);
			 /* copie la valeur de la constante */
			 i = 0;
			 pBu1 = pEl->ElText;
			 do
			   {
			      pBu1->BuContent[i] = pSS->SsConstBuffer[i + pSRule->SrIndexConst - 1];
			      i++;
			   }
			 while (pBu1->BuContent[i - 1] != '\0' && i < MAX_CHAR);
			 pBu1->BuContent[i - 1] = '\0';
			 pEl->ElTextLength = i - 1;
			 pEl->ElVolume = pEl->ElTextLength;
			 pEl->ElText->BuLength = i - 1;
			 pEl->ElLanguage = TtaGetDefaultLanguage ();
			 break;
		      default:
			 break;
		   }
	  }
     }
   /* evalue s'il faut engendrer la descendance */
   pSRule = &pSS->SsRule[typeNum - 1];
   if (!Desc || pSRule->SrParamElem)
      gener = FALSE;
   else
     {
	gener = TRUE;
	if (pSRule->SrRecursive)
	   /* regle recursive */
	   if (pSRule->SrRecursDone)
	      /* elle a deja ete appliquee */
	      gener = FALSE;	/* on n'engendre pas sa descendance */
	   else
	      /* elle n'a pas encore ete appliquee */
	      pSRule->SrRecursDone = TRUE;	/* on l'applique et on s'en souvient */
     }
   if (gener)
      /* on engendre la descendance selon le constructeur de la regle */
      switch (pSRule->SrConstruct)
	    {
	       case CsNatureSchema:
		  /* ce cas ne se produit pas, on a */
		  /* change' de type d'element (voir plus haut). */
		  break;
	       case CsIdentity:
		  /* structure identique a celle definie par une autre regle */
		  /* du meme schema */
		  create = FALSE;
		  pSRule2 = &pSS->SsRule[pSRule->SrIdentRule - 1];
		  if (pSRule2->SrParamElem || pSRule2->SrAssocElem || pSRule2->SrConstruct == CsBasicElement ||
		      pSRule2->SrNInclusions > 0 || pSRule2->SrNExclusions > 0 ||
		      pSRule2->SrConstruct == CsConstant || pSRule2->SrConstruct == CsChoice ||
		      pSRule2->SrConstruct == CsPairedElement ||
		      pSRule2->SrConstruct == CsReference || pSRule2->SrConstruct == CsNatureSchema)
		     create = TRUE;
		  t1 = NewSubtree (pSRule->SrIdentRule, pSS, pDoc, assocNum, Desc,
				 create, withAttr, withLabel);
		  if (pEl == NULL)
		     pEl = t1;
		  else
		     InsertFirstChild (pEl, t1);
		  break;
	       case CsBasicElement:
	       case CsConstant:
	       case CsReference:
	       case CsPairedElement:
	       case CsChoice:
		  break;
	       case CsList:
		  t1 = NewSubtree (pSRule->SrListItem, pSS, pDoc, assocNum, Desc,
				 TRUE, withAttr, withLabel);
		  if (pEl == NULL)
		     pEl = t1;
		  else
		     InsertFirstChild (pEl, t1);
		  if (t1 != NULL)
		     for (i = 2; i <= pSRule->SrMinItems; i++)
		       {
			  t2 = NewSubtree (pSRule->SrListItem, pSS, pDoc, assocNum, Desc,
				 TRUE, withAttr, withLabel);
			  if (t2 != NULL)
			    {
			       InsertElemAfterLastSibling (t1, t2);
			       t1 = t2;
			    }
		       }
		  break;
	       case CsAggregate:
	       case CsUnorderedAggregate:
		  t1 = NULL;
		  for (i = 1; i <= pSRule->SrNComponents; i++)
		     if (!pSRule->SrOptComponent[i - 1])
			/* on ne cree pas les composants optionnels */
		       {
			  t2 = NewSubtree (pSRule->SrComponent[i - 1], pSS, pDoc, assocNum, Desc, TRUE,
				       withAttr, withLabel);
			  if (t2 != NULL)
			    {
			       if (t1 != NULL)
				  InsertElemAfterLastSibling (t1, t2);
			       else if (pEl == NULL)
				  pEl = t2;
			       else
				  InsertFirstChild (pEl, t2);
			       t1 = t2;
			    }
		       }
		  break;
	       default:
		  break;
	    }

   if (pSRule->SrRecursive && pSRule->SrRecursDone && gener)
      pSRule->SrRecursDone = FALSE;	/* pour une autre fois... */
   return pEl;
}


/*----------------------------------------------------------------------
   RemoveExcludedElem retire du sous-arbre dont la racine est pointee    
   par pEl tous les elements exclus.			
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                RemoveExcludedElem (PtrElement * pEl)

#else  /* __STDC__ */
void                RemoveExcludedElem (pEl)
PtrElement         *pEl;

#endif /* __STDC__ */

{
   SRule              *pRule;
   SRule              *pRuleExcl;
   PtrElement          pAsc, pChild, pNextChild;
   PtrSSchema        pSS, pExtSSch;
   int                 i;
   boolean             exclus;

   if ((*pEl) != NULL)
     {
	/* teste tous les ascendants de l'element */
	pAsc = (*pEl)->ElParent;
	exclus = FALSE;
	while (pAsc != NULL && !exclus)
	  {
	     /* examine toutes les exclusions definies pour ce type d'element */
	     /* accede d'abord a la regle de structuration */
	     pSS = pAsc->ElStructSchema;
	     pRule = &pSS->SsRule[pAsc->ElTypeNumber - 1];
	     pExtSSch = NULL;
	     do
	       {
		  if (pRule != NULL)
		     /* examine toutes les exclusions definies dans la regle */
		     for (i = 1; i <= pRule->SrNExclusions; i++)
		       {
			  if (pRule->SrExclusion[i - 1] == (*pEl)->ElTypeNumber)
			     /* l'element traite' a le meme numero de type qu'un */
			     /* type d'element exclus par l'ascendant */
			     if ((*pEl)->ElTypeNumber <= MAX_BASIC_TYPE)
				/* c'est un type de base, il est exclus */
				exclus = TRUE;
			     else
				/* compare les identificateurs des schemas de structure */
			     if (pSS->SsCode == (*pEl)->ElStructSchema->SsCode)
				/* memes schemas de structure, type exclus */
				exclus = TRUE;
			  if (!exclus)
			     if ((*pEl)->ElTypeNumber == (*pEl)->ElStructSchema->SsRootElem)
				/* l'element traite' est l'element racine de son */
				/* schema de structure : c'est une nature. */
			       {
				  /* regle de structure de l'element exclus traite' */
				  pRuleExcl = &pSS->SsRule[pRule->SrExclusion[i - 1] - 1];
				  if (pRuleExcl->SrConstruct == CsNatureSchema)
				     /* l'exclusion traitee est une nature, l'exclusion */
				     /* s'applique si les noms de nature sont les memes */
				     exclus = (strcmp ((*pEl)->ElStructSchema->SsName,
						pRuleExcl->SrName) == 0);
			       }
			  /* le 2eme element d'une paire est exclus si le 1er est exclus */
			  if (!exclus)
			     /* on n'a pas encore exclus notre element */
			     if (pRule->SrExclusion[i - 1] + 1 == (*pEl)->ElTypeNumber)
				/* le type precedent est exclus */
				if (pSS->SsCode == (*pEl)->ElStructSchema->SsCode)
				   /* on est bien dans le bon schema de structure */
				   if ((*pEl)->ElStructSchema->SsRule[(*pEl)->ElTypeNumber - 1].SrConstruct == CsPairedElement)
				      /* l'element est un membre de paire */
				      if (!(*pEl)->ElStructSchema->SsRule[(*pEl)->ElTypeNumber - 1].SrFirstOfPair)
					 /* c'est le 2eme membre de la paire */
					 exclus = TRUE;
		       }
		  /* passe au schema d'extension suivant */
		  if (pExtSSch == NULL)
		     pExtSSch = pSS->SsNextExtens;
		  else
		     pExtSSch = pExtSSch->SsNextExtens;
		  if (pExtSSch != NULL)
		     /* il y a encore un schema d'extension */
		    {
		       pSS = pExtSSch;
		       /* cherche dans ce schema la regle d'extension pour l'ascendant */
		       pRule = ExtensionRule (pAsc->ElStructSchema, pAsc->ElTypeNumber, pExtSSch);
		    }
	       }
	     while (pExtSSch != NULL);
	     /* passe a l'element ascendant au-dessus */
	     pAsc = pAsc->ElParent;
	  }
	if (exclus)
	  {
	     /* retire le sous arbre de son arbre et le libere */
	     DeleteElement (pEl);
	     *pEl = NULL;
	  }
	/* traite le sous arbre de pEl, s'il n'a pas ete supprime' */
	if ((*pEl) != NULL)
	   if (!(*pEl)->ElTerminal)
	     {
		pChild = (*pEl)->ElFirstChild;
		while (pChild != NULL)
		  {
		     pNextChild = pChild->ElNext;
		     RemoveExcludedElem (&pChild);
		     pChild = pNextChild;
		  }
	     }
     }
}

/*----------------------------------------------------------------------
   RemoveElement retire le sous-arbre dont la racine est pointee par pEl  
   de l'arbre ou il se trouve.                             
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                RemoveElement (PtrElement pEl)

#else  /* __STDC__ */
void                RemoveElement (pEl)
PtrElement          pEl;

#endif /* __STDC__ */

{
   PtrElement          pAsc;

   if (pEl != NULL)
     {
	/* retire le volume de l'element du volume de ses ascendants */
	pAsc = pEl->ElParent;
	while (pAsc != NULL)
	  {
	     pAsc->ElVolume = pAsc->ElVolume - pEl->ElVolume;
	     pAsc = pAsc->ElParent;
	  }
	/* retire l'element de la liste de ses freres */
	if (pEl->ElPrevious != NULL)
	   pEl->ElPrevious->ElNext = pEl->ElNext;
	if (pEl->ElNext != NULL)
	   pEl->ElNext->ElPrevious = pEl->ElPrevious;
	/* supprime le chainage du pere si c'est le premier fils */
	if (pEl->ElParent != NULL)
	   if (pEl->ElParent->ElFirstChild == pEl)
	      pEl->ElParent->ElFirstChild = pEl->ElNext;
	pEl->ElPrevious = NULL;
	pEl->ElNext = NULL;
	pEl->ElParent = NULL;
     }
}

/*----------------------------------------------------------------------
   RemoveAttribute retire de l'element pointe' par pEl l'attribut       
   pointe' par pAttr, sans liberer cet attribut            
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                RemoveAttribute (PtrElement pEl, PtrAttribute pAttr)

#else  /* __STDC__ */
void                RemoveAttribute (pEl, pAttr)
PtrElement          pEl;
PtrAttribute         pAttr;

#endif /* __STDC__ */

{
   PtrAttribute         pPrevAttr;
   boolean             stop;

   if (pEl != NULL && pAttr != NULL)
     {
	/* retire l'attribut de la chaine des attributs de l'element */
	if (pEl->ElFirstAttr == pAttr)
	   /* c'est le premier attribut de l'element */
	   pEl->ElFirstAttr = pAttr->AeNext;
	else
	   /* cherche l'attribut a supprimer dans la chaine des attributs de */
	   /* l'element */
	  {
	     pPrevAttr = pEl->ElFirstAttr;
	     stop = FALSE;
	     do
		if (pPrevAttr == NULL)
		   /* fin de chaine, on n'a pas trouve' l'attribut. Il a du etre */
		   /* deja retire' */
		   stop = TRUE;
		else if (pPrevAttr->AeNext == pAttr)
		   /* found', on le dechaine */
		  {
		     pPrevAttr->AeNext = pAttr->AeNext;
		     stop = TRUE;
		  }
		else
		   /* passe au suivant */
		   pPrevAttr = pPrevAttr->AeNext;
	     while (!stop);
	  }
     }
}

/*----------------------------------------------------------------------
   DeleteAttribute supprime de l'element pointe' par pEl l'attribut   
   pointe' par pAttr. DeleteElement egalement de l'element      
   toutes les regles de presentation specifique relatives  
   a` l'attribut.                                          
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                DeleteAttribute (PtrElement pEl, PtrAttribute pAttr)

#else  /* __STDC__ */
void                DeleteAttribute (pEl, pAttr)
PtrElement          pEl;
PtrAttribute         pAttr;

#endif /* __STDC__ */

{
   PtrPRule        pPR, pPRprev, pPRnext;
   PtrTextBuffer      buf, nextbuf;

   if (pEl != NULL && pAttr != NULL)
      /* supprime les regles de presentation specifique attachees a */
      /* l'element et qui dependent de cet attribut */
     {
	pPRprev = NULL;
	pPR = pEl->ElFirstPRule;
	while (pPR != NULL)
	  {
	     pPRnext = pPR->PrNextPRule;
	     if (pPR->PrSpecifAttr == pAttr->AeAttrNum
		 && pPR->PrSpecifAttrSSchema == pAttr->AeAttrSSchema)
		/* la regle pPR depend de l'attribut a supprimer, on */
		/* dechaine et libere la regle */
	       {
		  if (pPRprev == NULL)
		     pEl->ElFirstPRule = pPRnext;
		  else
		     pPRprev->PrNextPRule = pPRnext;
		  FreePresentRule (pPR);
		  pPR = pPRnext;
	       }
	     else
		/* passe a la regle suivante */
	       {
		  pPRprev = pPR;
		  pPR = pPRnext;
	       }
	  }
	/* retire l'attribut de la chaine des attributs de l'element, s'il */
	/* n'a pas encore ete libere' */
	RemoveAttribute (pEl, pAttr);
	/* libere la memoire attachee a l'attribut */
	if (pAttr->AeAttrType == AtReferenceAttr)
	   /* libere la reference */
	   if (pAttr->AeAttrReference != NULL)
	     {
		DeleteReference (pAttr->AeAttrReference);
		FreeReference (pAttr->AeAttrReference);
	     }
	if (pAttr->AeAttrType == AtTextAttr)
	   /* libere les buffers de texte */
	  {
	     buf = pAttr->AeAttrText;
	     while (buf != NULL)
	       {
		  nextbuf = buf->BuNext;
		  FreeTextBuffer (buf);
		  buf = nextbuf;
	       }
	  }
	/* traitement particulier a la suppression des attributs dans un Draw*/
	DrawSupprAttr (pAttr, pEl);
	/* libere le bloc attribut */
	FreeAttr (pAttr);
     }
}


/*----------------------------------------------------------------------
   DeleteElement supprime l'element pointe par pEl, ainsi que toute sa   
   descendance dans l'arbre abstrait et annule toutes les  
   references qui le pointent. Au retour le pointeur passe'
   en parametre est remis a NIL.                           
   Pour chaque element libere', tous les paves             
   correspondants, dans toutes les vues, sont liberes.     
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                DeleteElement (PtrElement * pEl)

#else  /* __STDC__ */
void                DeleteElement (pEl)
PtrElement         *pEl;

#endif /* __STDC__ */

{
   PtrElement          pChild, pNextChild;
   PtrTextBuffer      pBuf, pNextBuf;
   int                 c, n, d;
   PtrAttribute         pAttr, pNextAttr;
   PtrPRule        pRule, pNextRule;
   PtrElement          pEl1;
   PtrCopyDescr        pCD, pNextCD;
   PtrElement          pAsc;
   PtrSSchema        pSS;
   boolean             ok, stop;
   PtrDocument         pDoc;

   if (*pEl != NULL)
     {
	pEl1 = *pEl;
	/* supprime le texte, si c'est une feuille de texte */
	if (pEl1->ElTerminal)
	  {
	     if ((pEl1->ElLeafType == LtText || pEl1->ElLeafType == LtPicture)
		 && pEl1->ElText != NULL)
		/* supprime tous les buffers de texte associes a l'element */
	       {
		  pBuf = pEl1->ElText;
		  c = 0;
		  do
		    {
		       pNextBuf = pBuf->BuNext;
		       c += pBuf->BuLength;
		       DeleteTextBuffer (&pBuf);
		       pBuf = pNextBuf;
		    }
		  while (c < pEl1->ElTextLength && pBuf != NULL);
		  pEl1->ElText = NULL;
		  pEl1->ElTextLength = 0;

		  /* Liberation du descripteur d'image */
		  if (pEl1->ElLeafType == LtPicture)
		    {
		       FreeImageDescriptor (pEl1->ElPictInfo);
		       pEl1->ElPictInfo = NULL;
		    }
	       }
	     if (pEl1->ElLeafType == LtPlyLine)
		/* supprime tous les buffers de coordonnees associes a l'element */
	       {
		  pBuf = pEl1->ElPolyLineBuffer;
		  while (pBuf != NULL)
		    {
		       pNextBuf = pBuf->BuNext;
		       DeleteTextBuffer (&pBuf);
		       pBuf = pNextBuf;
		    }
		  pEl1->ElPolyLineBuffer = NULL;
		  pEl1->ElNPoints = 0;
	       }
	     if (pEl1->ElLeafType == LtReference)
		/* supprime et dechaine la reference */
	       {
		  CancelReference (*pEl);
		  if (pEl1->ElReference != NULL)
		     FreeReference (pEl1->ElReference);
	       }
	     if (pEl1->ElLeafType == LtPairedElem)
		if (pEl1->ElOtherPairedEl != NULL)
		   pEl1->ElOtherPairedEl->ElOtherPairedEl = NULL;
	  }
	else
	   /* ce n'est pas une feuille, supprime tous les fils */
	  {
	     pChild = pEl1->ElFirstChild;
	     while (pChild != NULL)
	       {
		  pNextChild = pChild->ElNext;
		  DeleteElement (&pChild);
		  pChild = pNextChild;
	       }
	  }
	/* supprime tous les descripteurs d'elements copies */
	pCD = pEl1->ElCopyDescr;
	while (pCD != NULL)
	  {
	     if (pCD->CdCopiedAb != NULL)
		pCD->CdCopiedAb->AbCopyDescr = NULL;
	     pNextCD = pCD->CdNext;
	     FreeDescCopy (pCD);
	     pCD = pNextCD;
	  }

	/* supprime la reference d'inclusion si elle existe */
	if (pEl1->ElSource != NULL)
	  {
	     CancelReference (*pEl);
	     FreeReference (pEl1->ElSource);
	  }
	/* supprime tous les attributs */
	pAttr = pEl1->ElFirstAttr;
	while (pAttr != NULL)
	  {
	     pNextAttr = pAttr->AeNext;
	     DeleteAttribute (*pEl, pAttr);
	     pAttr = pNextAttr;
	  }
	/* supprime toutes les regles de presentation de l'element */
	pRule = pEl1->ElFirstPRule;
	pEl1->ElFirstPRule = NULL;
	while (pRule != NULL)
	  {
	     pNextRule = pRule->PrNextPRule;
	     FreePresentRule (pRule);
	     pRule = pNextRule;
	  }
	/* supprime le commentaire associe a l'element */
	pBuf = pEl1->ElComment;
	while (pBuf != NULL)
	  {
	     pNextBuf = pBuf->BuNext;
	     DeleteTextBuffer (&pBuf);
	     pBuf = pNextBuf;
	  }
	/* supprime les references sur cet element */
	DeleteAllReferences (*pEl);
	/* supprime le descripteur d'element referenc'e */
	DeleteReferredElDescr (pEl1->ElReferredDescr);
	pEl1->ElReferredDescr = NULL;
	/* decremente le nombre d'objets si c'est un element construit selon */
	/* la regle racine de son schema de structure. */
	if (pEl1->ElTypeNumber == pEl1->ElStructSchema->SsRootElem)
	   /* l'element est construit selon la regle racine de son schema */
	  {
	     pEl1->ElStructSchema->SsNObjects--;
	     if (pEl1->ElStructSchema->SsNObjects == 0)	/* c'etait le dernier objet */
		/* libere la nature */
	       {
		  pAsc = pEl1->ElParent;
		  ok = FALSE;
		  while ((!ok) && (pAsc != NULL))
		    {
		       pSS = pEl1->ElStructSchema;
		       ok = FreeNature (pAsc->ElStructSchema, pSS);
		       if (ok)
			  /* le schema de structure a ete libere'. Il faut le */
			  /* supprimer de la table des natures du document */
			 {
			    stop = FALSE;
			    for (d = 0; d < MAX_DOCUMENTS && !stop; d++)
			      {
				 pDoc = LoadedDocument[d];
				 if (pDoc != NULL)
				   {
				      for (n = 1; n <= pDoc->DocNNatures && !stop; n++)
					 if (pDoc->DocNatureSSchema[n - 1] == pSS)
					   {
					      stop = TRUE;
					      while (n < pDoc->DocNNatures)
						{
						   strcpy (pDoc->DocNatureName[n - 1], pDoc->DocNatureName[n]);
						   strcpy (pDoc->DocNaturePresName[n - 1], pDoc->DocNaturePresName[n]);
						   pDoc->DocNatureSSchema[n - 1] = pDoc->DocNatureSSchema[n];
						   n++;
						}
					      pDoc->DocNNatures--;
					   }
				   }
			      }
			 }
		       else
			  /* cette nature n'etait pas definie au niveau */
			  /* immediatement superieur, cherche un autre schema de */
			  /* structure englobant, ou` cette nature est une unite' */
			 {
			    pSS = pAsc->ElStructSchema;
			    stop = FALSE;
			    do
			      {
				 pAsc = pAsc->ElParent;
				 if (pAsc == NULL)
				    stop = TRUE;
				 else if (pAsc->ElStructSchema != pSS)
				    stop = TRUE;
			      }
			    while (!stop);
			 }
		    }
	       }
	  }
	/* retire l'element de l'arbre */
	RemoveElement (*pEl);
	/* libere tous les paves */
	LibAbbEl (*pEl);
	/* rend la memoire */
	FreeElement (*pEl);
	*pEl = NULL;
     }
}


/*----------------------------------------------------------------------
   CopyTree cree un arbre copie de l'arbre (ou sous-arbre)       
   pointe' par pSource et rend un pointeur sur la racine	
   de l'arbre cree, ou NULL si la creation de la copie a   
   echoue'. On ne copie pas les marques de page.           
   - pDocSource: pointeur sur le descripteur du document   
   auquel appartient l'arbre a` copier.                    
   - assocNum est le numero de la liste d'elements associes
   auquel doit appartenir la copie. assocNum=0 si ce n'est 
   pas un elt associe'.                                    
   - pSSchema: pointeur sur le schema de structure dont	
   doivent dependre les elements de la copie.              
   - pDocCopy: pointeur sur le descripteur du document     
   auquel doit appartenir la copie.                        
   - pPere: pointeur sur l'element qui sera le pere de     
   l'arbre cree'.                                          
   Si checkAttr est vrai, on ne copie les attributs de la  
   source que s'ils peuvent effectivement s'appliquer a`   
   la copie. Si shareRef est vrai, les elements            
   reference's de la copie partagent leur descripteur      
   d'element reference' avec l'original.                   
  ----------------------------------------------------------------------*/

#ifdef __STDC__
PtrElement          CopyTree (PtrElement pSource, PtrDocument pDocSource, int assocNum, PtrSSchema pSSchema, PtrDocument pDocCopy, PtrElement pParent, boolean checkAttr, boolean shareRef)

#else  /* __STDC__ */
PtrElement          CopyTree (pSource, pDocSource, assocNum, pSSchema, pDocCopy, pParent, checkAttr, shareRef)
PtrElement          pSource;
PtrDocument         pDocSource;
int                 assocNum;
PtrSSchema        pSSchema;
PtrDocument         pDocCopy;
PtrElement          pParent;
boolean             checkAttr;
boolean             shareRef;

#endif /* __STDC__ */

{
   PtrElement          pEl, pS2, pC1, pC2, pSP;
   PtrReference        rf;
   int         copyType, nR;
   Name                 PSchName;
   SRule              *pSRule;
   PtrElement          pAsc;
   PtrSSchema        pSS;
   boolean             sameSSchema;
   boolean             doCopy;

   pEl = NULL;
   /* pointeur sur l'element qui sera cree' */
   if (pSource != NULL)
     {
	doCopy = TRUE;
	/* on ne copie pas les marques de page */
	if (pSource->ElTerminal && pSource->ElLeafType == LtPageColBreak)
	   doCopy = FALSE;
	else if (pSource->ElSource != NULL)
	   /* cet element est une copie-inclusion */
	   /* on ne le copie pas s'il a ete engendre' avant ou apres une */
	   /* marque de page (comme les tetieres de tableau par exemple) */
	   if (TypeHasException (ExcPageBreakRepBefore, pSource->ElTypeNumber, pSource->ElStructSchema))
	      doCopy = FALSE;
	   else if (TypeHasException (ExcPageBreakRepetition, pSource->ElTypeNumber, pSource->ElStructSchema))
	      doCopy = FALSE;
	if (doCopy)
	  {
	     copyType = pSource->ElTypeNumber;
	     if (strcmp (pSource->ElStructSchema->SsName, pSSchema->SsName) != 0)
		/* changement de structure generique */
		if (pSource->ElStructSchema->SsRule[pSource->ElTypeNumber - 1].SrUnitElem ||
		    pSource->ElStructSchema->SsExtension || pSource->ElTypeNumber <= MAX_BASIC_TYPE)
		   /* l'element source est une unite' ou un element defini dans une */
		   /* extension de schema */
		   if (!checkAttr)
		      /* on ne verifie pas les unites. La copie aura le meme */
		      /* schema de structure que la source */
		      pSSchema = pSource->ElStructSchema;
		   else
		     {
			/* cherche si la future ascendance de la copie comporte */
			/* un element appartenant au schema ou` est defini */
			/* l'element source */
			if (pParent == NULL)
			   pAsc = pDocCopy->DocRootElement;
			else
			   pAsc = pParent;
			sameSSchema = pSource->ElTypeNumber <= MAX_BASIC_TYPE;
			if (pAsc != NULL && !sameSSchema)
			   do
			     {
				if (pSource->ElStructSchema->SsCode ==
				    pAsc->ElStructSchema->SsCode)
				  {
				     /* la copie aura le schema de structure de son doc. */
				     pSSchema = pAsc->ElStructSchema;
				     sameSSchema = TRUE;
				  }
				else if (pSource->ElStructSchema->SsExtension)
				   /* verifie si l'ascendant a cette extension de schema */
				  {
				     pSSchema = pSource->ElStructSchema;
				     if (ValidExtension (pAsc, &pSSchema))
					sameSSchema = TRUE;
				  }
				else if (pAsc->ElStructSchema->SsExtension)
				   /* l'ascendant est une extension */
				  {
				     pSS = pAsc->ElStructSchema;
				     if (ValidExtension (pSource, &pSS))
				       {
					  pSSchema = pSource->ElStructSchema;
					  sameSSchema = TRUE;
				       }
				  }
				if (!sameSSchema)
				   /* passe a l'element ascendant au-dessus */
				   pAsc = pAsc->ElParent;
			     }
			   while (pAsc != NULL && !sameSSchema);
			if (!sameSSchema)
			   /* il n'y a pas d'ascendant qui ait ce schema de */
			   /* structure, l'unite' est invalide. */
			  {
#ifdef IV
			     if (ResdynCt.ElSour != NULL && !pSource->ElStructSchema->SsExtension)
				/* Si on est en cours de restructuration on recherche */
				/* une unite compatible */
				GDRCompatibleUnit ((Element) pSource, (Element) pParent,
					       &copyType, (int **) &pSSchema);
			     else
#endif
				copyType = 0;
			  }
		     }
		else
		   /* la source est un autre document ou une autre nature, il */
		   /* faut charger les schemas pour la copie */
		  {
		     /* charge les schemas de structure et present. pour la copie */
		     PSchName[0] = '\0';
		     /* pas de preference pour le schema de presentation */
		     nR = CreateNature (pSource->ElStructSchema->SsName, PSchName, pSSchema);
		     if (nR == 0)
			/* echec de chargement des schemas */
			copyType = 0;
		     else
			/* chargement reussi, change de schema de structure pour la copie. */
		       {
			  pSRule = &pSSchema->SsRule[nR - 1];
			  pSSchema = pSRule->SrSSchemaNat;
		       }
		  }
	     if (copyType != 0)
	       {
		  /* acquiert un element pour la copie */
		  GetElement (&pEl);
		  /* remplit la copie */
		  pEl->ElStructSchema = pSSchema;
		  pEl->ElTypeNumber = copyType;
		  if (pEl->ElTypeNumber == pEl->ElStructSchema->SsRootElem)
		     /* on cree un element construit selon la regle racine de */
		     /* son schema de structure, on incremente le compteur */
		     pSSchema->SsNObjects++;
		  pEl->ElAssocNum = assocNum;
		  pEl->ElParent = pParent;	/* pour CopyAttributes */
		  CopyAttributes (pSource, pEl, checkAttr);	/* copie les attributs */
		  /* copie les regles de presentation specifique */
		  CopyPresRules (pSource, pEl);	/* copie les regles de presentation */
		  /* copie le commentaire associe a l'element */
		  if (pSource->ElComment != NULL)
		     pEl->ElComment = CopyText (pSource->ElComment, pEl);
		  if (shareRef)
		    {
		       strncpy (pEl->ElLabel, pSource->ElLabel, MAX_LABEL_LEN);
		       pEl->ElReferredDescr = pSource->ElReferredDescr;	/* partage (temporairement) */
		       /* le descripteur d'element reference' entre l'element source */
		       /*et l'element copie,pour que CopyCommand ou les procedure Coller */
		       /* puissent relier les references copiees aux elements copies */
		    }
		  else
		    {
		       pEl->ElReferredDescr = NULL;	/* la copie n'est pas reference'e */
		       /* calcule la valeur du label */
		       LabelIntToString (NewLabel (pDocCopy), pEl->ElLabel);
		    }
		  pEl->ElIsCopy = pSource->ElIsCopy;
		  pEl->ElAccess = AccessInherited;
		  pEl->ElHolophrast = pSource->ElHolophrast;
		  pEl->ElTerminal = pSource->ElTerminal;
		  if (!pEl->ElTerminal)
		     pEl->ElFirstChild = NULL;
		  else
		    {
		       pEl->ElLeafType = pSource->ElLeafType;
		       switch (pEl->ElLeafType)
			     {
				case LtPicture:
				   pEl->ElPictInfo = NULL;
				   /* copie le contenu d'un texte ou d'une image */
				   pEl->ElText = CopyText (pSource->ElText, pEl);
				   pEl->ElTextLength = pSource->ElTextLength;
				   pEl->ElVolume = pEl->ElTextLength;
				   break;
				case LtText:
				   pEl->ElLanguage = pSource->ElLanguage;
				   /* copie le contenu d'un texte ou d'une image */
				   pEl->ElText = CopyText (pSource->ElText, pEl);
				   pEl->ElTextLength = pSource->ElTextLength;
				   pEl->ElVolume = pEl->ElTextLength;
				   break;
				case LtPlyLine:
				   pEl->ElPolyLineBuffer = CopyText (pSource->ElPolyLineBuffer, pEl);
				   pEl->ElNPoints = pSource->ElNPoints;
				   pEl->ElPolyLineType = pSource->ElPolyLineType;
				   pEl->ElVolume = pEl->ElNPoints;
				   break;
				case LtSymbol:
				case LtGraphics:
				   pEl->ElGraph = pSource->ElGraph;
				   break;
				case LtPageColBreak:

				   break;
				case LtReference:
				   if (pSource->ElReference != NULL)
				     {
					GetReference (&rf);	/* acquiert une reference */
					pEl->ElReference = rf;
					/* remplit la nouvelle reference */
					CopyReference (pEl->ElReference, pSource->ElReference, &pEl);
				     }
				   break;
				case LtPairedElem:
				   pEl->ElPairIdent = pSource->ElPairIdent;
				   pEl->ElOtherPairedEl = NULL;
				   break;
				default:
				   break;
			     }
		    }
		  if (pSource->ElSource != NULL)
		    {
		       GetReference (&rf);	/* acquiert une reference */
		       pEl->ElSource = rf;
		       /* remplit la nouvelle reference */
		       CopyReference (pEl->ElSource, pSource->ElSource, &pEl);
		    }
		  /* cree les copies des elements de la descendance, s'il y en a */
		  if (!pSource->ElTerminal)
		     if (pSource->ElFirstChild != NULL)
		       {
			  pS2 = pSource->ElFirstChild;
			  pC1 = NULL;
			  do
			    {
			       pC2 = CopyTree (pS2, pDocSource, assocNum, pSSchema,
				      pDocCopy, pEl, checkAttr, shareRef);
			       if (pC2 != NULL)
				 {
				    /* rompt le lien avec le futur pere pour que */
				    /* le calcul du volume se passe bien */
				    pSP = pEl->ElParent;
				    pEl->ElParent = NULL;
				    if (pC1 == NULL)
				       InsertFirstChild (pEl, pC2);
				    else
				       InsertElementAfter (pC1, pC2);
				    pC1 = pC2;
				    /* retablit le lien avec le futur pere */
				    pEl->ElParent = pSP;
				 }
			       pS2 = pS2->ElNext;
			    }
			  while (pS2 != NULL);
		       }
		  pEl->ElParent = NULL;	/* ElParent avait ete mis pour CopyAttributes */
	       }
	  }
     }
   return pEl;
}

/*----------------------------------------------------------------------
   Rend le premier element associe de type typeNum (defini dans le  
   schema de structure pSS) appartenant au document pDoc ou NULL si 
   aucun element associe' de ce type n'existe			 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PtrElement          FirstAssocElem (PtrDocument pDoc, int typeNum, PtrSSchema pSS)

#else  /* __STDC__ */
PtrElement          FirstAssocElem (pDoc, typeNum, pSS)
PtrDocument         pDoc;
int         typeNum;
PtrSSchema        pSS;

#endif /* __STDC__ */

{
   int                 a;
   PtrElement          pEl, pEl2;
   boolean             stop;

   pEl = NULL;
   stop = FALSE;
   a = 0;
   do
     {
	if (pDoc->DocAssocRoot[a] != NULL)
	   if (pDoc->DocAssocRoot[a]->ElFirstChild != NULL)
	     {
		pEl2 = pDoc->DocAssocRoot[a]->ElFirstChild;
		FwdSkipPageBreak (&pEl2);
		if (pEl2 != NULL)
		   if (pEl2->ElTypeNumber == typeNum &&
		       pEl2->ElStructSchema->SsCode == pSS->SsCode)
		     {
			pEl = pEl2;
			stop = TRUE;
		     }
	     }
	a++;
     }
   while (!stop && a < MAX_ASSOC_DOC);
   return pEl;
}


/*----------------------------------------------------------------------
   ChangeLabels affecte de nouveaux labels a tous les elements du  
   sous-arbre pEl, lui-meme exclus.                        
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         ChangeLabels (PtrElement pEl, PtrDocument pDoc)

#else  /* __STDC__ */
static void         ChangeLabels (pEl, pDoc)
PtrElement          pEl;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrElement          pE;

   if (!pEl->ElTerminal)
     {
	pE = pEl->ElFirstChild;
	while (pE != NULL)
	  {
	     LabelIntToString (NewLabel (pDoc), pE->ElLabel);
	     ChangeLabels (pE, pDoc);
	     pE = pE->ElNext;
	  }
     }
}


/*----------------------------------------------------------------------
   CopyIncludedElem copie l'arbre abstrait de l'element a` inclure comme
   sous-arbre de l'element pointe' par pEl. pEl pointe sur 
   un element representant une inclusion. pDoc designe le  
   document auquel appartient l'element pointe' par pEl.   
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                CopyIncludedElem (PtrElement pEl, PtrDocument pDoc)

#else  /* __STDC__ */
void                CopyIncludedElem (pEl, pDoc)
PtrElement          pEl;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrReference        pRef;
   PtrElement          pSource, pS2, pC1, pC2, pE;
   DocumentIdentifier     docIdent;
   PtrDocument         pDocSource;
   boolean             done;

   /* on n'a pas encore fait la copie */
   done = FALSE;
   if (pEl->ElTerminal)
      switch (pEl->ElLeafType)
	    {
	       case LtPicture:
	       case LtText:
		  done = pEl->ElTextLength > 0;
		  break;
	       case LtPlyLine:
		  done = pEl->ElNPoints > 0;
	       case LtSymbol:
	       case LtGraphics:
		  done = pEl->ElGraph != '\0';
		  break;
	       case LtPageColBreak:
	       case LtReference:
		  break;
	       default:
		  break;
	    }
   else
      done = pEl->ElFirstChild != NULL;
   if (!done)
      /* La copie n'est pas encore faite, on la fait */
     {
	pRef = pEl->ElSource;	/* reference a l'element source */
	/* cherche l'element dont il faut faire la copie : pSource */
	pSource = ReferredElement (pRef, &docIdent, &pDocSource);
	/* l'element a copier est pointe' par pSource, on le copie */
	if (pSource != NULL)
	  {
	     if (DocIdentIsNull (docIdent))
		/* l'element a copier est dans le meme document */
		pDocSource = pDoc;
	     if (pDocSource != NULL)
		/* le document contenant l'element a copier est charge', */
		/* on copie son contenu */
	       {
		  /* copie les attributs */
		  CopyAttributes (pSource, pEl, TRUE);
		  /* copie les regles de presentation specifique */
		  CopyPresRules (pSource, pEl);
		  /* copie le commentaire associe a l'element */
		  ClearText (pEl->ElComment);
		  FreeTextBuffer (pEl->ElComment);
		  if (pSource->ElComment != NULL)
		     pEl->ElComment = CopyText (pSource->ElComment, pEl);
		  if (pEl->ElTerminal)
		     switch (pSource->ElLeafType)
			   {
			      case LtText:
			      case LtPicture:
			      case LtPlyLine:
				 if (pSource->ElLeafType == LtText)
				    pEl->ElLanguage = pSource->ElLanguage;
				 /* copie le contenu d'un texte ou d'une image */
				 if (pSource->ElLeafType == LtPlyLine)
				   {
				      if (pEl->ElLeafType == LtPlyLine &&
					  pEl->ElPolyLineBuffer != NULL)
					{
					   ClearText (pEl->ElPolyLineBuffer);
					   FreeTextBuffer (pEl->ElPolyLineBuffer);
					}
				      pEl->ElLeafType = LtPlyLine;
				      pEl->ElPolyLineBuffer = CopyText (pSource->ElPolyLineBuffer, pEl);
				      pEl->ElNPoints = pSource->ElNPoints;
				      pEl->ElPolyLineType = pSource->ElPolyLineType;
				      pEl->ElVolume = pEl->ElNPoints;
				   }
				 else
				   {
				      pEl->ElText = CopyText (pSource->ElText, pEl);
				      pEl->ElTextLength = pSource->ElTextLength;
				      pEl->ElVolume = pEl->ElTextLength;
				   }
				 /* ajoute le volume de l'element a celui de ses ascendants */
				 if (pEl->ElVolume != 0)
				   {
				      pE = pEl->ElParent;
				      while (pE != NULL)
					{
					   pE->ElVolume = pE->ElVolume + pEl->ElVolume;
					   pE = pE->ElParent;
					}
				   }

				 break;
			      case LtSymbol:
			      case LtGraphics:
				 pEl->ElGraph = pSource->ElGraph;
				 break;
			      case LtPageColBreak:
				 break;
			      case LtReference:
				 if (pSource->ElReference != NULL)
				   {
				      if (pEl->ElReference == NULL)
					 GetReference (&pEl->ElReference);
				      CopyReference (pEl->ElReference, pSource->ElReference, &pEl);
				   }
				 break;
			      default:
				 break;
			   }
		  else if (pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrConstruct == CsChoice
			   && (pEl->ElTypeNumber != pSource->ElTypeNumber
			       || pEl->ElStructSchema->SsCode !=
			       pSource->ElStructSchema->SsCode))
		    {
		       pC1 = CopyTree (pSource, pDocSource, pEl->ElAssocNum, pEl
				      ->ElStructSchema, pDoc, pEl, TRUE, TRUE);
		       if (pC1 != NULL)
			 {
			    pC1->ElReferredDescr = NULL;
			    InsertElemInChoice (pEl, &pC1, FALSE);
			 }
		    }
		  else if (pSource->ElFirstChild == NULL)
		     pEl->ElFirstChild = NULL;
		  else
		    {
		       pS2 = pSource->ElFirstChild;
		       pC1 = NULL;
		       do
			 {
			    pC2 = CopyTree (pS2, pDocSource, pEl->ElAssocNum,
				   pEl->ElStructSchema, pDoc, pEl, TRUE, TRUE);
			    if (pC2 != NULL)
			      {
				 if (pC1 == NULL)
				    InsertFirstChild (pEl, pC2);
				 else
				    InsertElementAfter (pC1, pC2);
				 pC1 = pC2;
			      }
			    pS2 = pS2->ElNext;
			 }
		       while (pS2 != NULL);
		    }
	       }
	  }
	/* Met a jour les references internes a la partie copiee */
	TransferReferences (pEl, pDoc, pEl, pDocSource);

	/* affecte des nouveaux labels aux elements de la copie */
	ChangeLabels (pEl, pDoc);

	/* protege le sous-arbre inclus contre toute modification de */
	/* l'utilisateur */
	ProtectElement (pEl);
     }
}

/*----------------------------------------------------------------------
   ReplicateElement duplique un noeud sans copier ses fils.        
  ----------------------------------------------------------------------*/

#ifdef __STDC__
PtrElement          ReplicateElement (PtrElement pEl, PtrDocument pDoc)

#else  /* __STDC__ */
PtrElement          ReplicateElement (pEl, pDoc)
PtrElement          pEl;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrElement          pNew;
   int                 view;

   GetElement (&pNew);
   *pNew = *pEl;
   if (pNew->ElTypeNumber == pNew->ElStructSchema->SsRootElem)
      /* on cree un element construit selon la regle racine de */
      /* son schema de structure, on incremente le compteur */
      pNew->ElStructSchema->SsNObjects++;
   /* calcule la valeur du label */
   LabelIntToString (NewLabel (pDoc), pNew->ElLabel);
   /* copie les attributs sans verification puisqu'on reste dans le meme */
   /* schema de structure. */
   CopyAttributes (pEl, pNew, FALSE);
   /* copie les regles de presentation specifique */
   CopyPresRules (pEl, pNew);
   pNew->ElPrevious = NULL;
   pNew->ElParent = NULL;
   pNew->ElReferredDescr = NULL;
   pNew->ElPrevious = NULL;
   pNew->ElNext = NULL;
   pNew->ElText = NULL;
   pNew->ElFirstChild = NULL;
   pNew->ElLanguage = pEl->ElLanguage;
   pNew->ElComment = NULL;
   pNew->ElCopyDescr = NULL;
   pNew->ElVolume = 0;
   for (view = 0; view < MAX_VIEW_DOC; view++)
      pNew->ElAbstractBox[view] = NULL;
   return pNew;
}


/*----------------------------------------------------------------------
   GetTypedAttrForElem cherche parmi les attributs attaches a l'element pEl
   s'il en existe un du type attrNum. Retourne un pointeur 
   sur cet attribut ou NULL s'il n'existe pas.             
  ----------------------------------------------------------------------*/

#ifdef __STDC__
PtrAttribute         GetTypedAttrForElem (PtrElement pEl, int attrNum, PtrSSchema pSSattr)

#else  /* __STDC__ */
PtrAttribute         GetTypedAttrForElem (pEl, attrNum, pSSattr)
PtrElement          pEl;
int                 attrNum;
PtrSSchema        pSSattr;

#endif /* __STDC__ */

{
   boolean             found;
   PtrAttribute         pAttr;

   found = FALSE;
   pAttr = pEl->ElFirstAttr;
   /* premier attribut de l'element */
   while (pAttr != NULL && !found)
     {
	if (pAttr->AeAttrNum == attrNum)
	   /* memes numeros d'attribut */
	   if (attrNum == 1)
	      /* c'est l'attribut Langue, inutile de comparer les schemas */
	      found = TRUE;
	   else if (pAttr->AeAttrSSchema->SsCode == pSSattr->SsCode)
	      /* memes schemas : c'est bien l'attribut cherche' */
	      found = TRUE;
	if (!found)
	   /* passe a l'attribut suivant du meme element */
	   pAttr = pAttr->AeNext;
     }
   return pAttr;
}


/*----------------------------------------------------------------------
   GetTypedAttrAncestor retourne un pointeur vers l'attribut du	
   premier element qui englobe pEl et qui porte un		
   attribut de type attrNum. La fonction retourne NULL si	
   pas trouve'. Si on trouve, pElAttr pointe sur l'element	
   qui porte cet attribut.			                
  ----------------------------------------------------------------------*/

#ifdef __STDC__
PtrAttribute         GetTypedAttrAncestor (PtrElement pEl, int attrNum,
			       PtrSSchema pSSattr, PtrElement * pElAttr)

#else  /* __STDC__ */
PtrAttribute         GetTypedAttrAncestor (pEl, attrNum, pSSattr, pElAttr)
PtrElement          pEl;
int                 attrNum;
PtrSSchema        pSSattr;
PtrElement         *pElAttr;

#endif /* __STDC__ */

{
   PtrElement          pElAtt;
   PtrAttribute         pAttr;

   pAttr = NULL;
   *pElAttr = NULL;
   /* on commence par l'element lui-meme */
   pElAtt = pEl;
   /* cherche dans ses ascendants */
   while (pElAtt != NULL && pAttr == NULL)
      /* cherche parmi les attributs de l'element */
      if ((pAttr = GetTypedAttrForElem (pElAtt, attrNum, pSSattr)) == NULL)
	 /* l'element n'a pas cet attribut, passe a l'element ascendant */
	 pElAtt = pElAtt->ElParent;
      else
	 *pElAttr = pElAtt;
   return pAttr;
}


/*----------------------------------------------------------------------
   CheckLanguageAttr     verifie que la racine pEl porte un      
   attribut Langue et si non, on en met un.                        
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                CheckLanguageAttr (PtrDocument pDoc, PtrElement pEl)

#else  /* __STDC__ */
void                CheckLanguageAttr (pDoc, pEl)
PtrDocument         pDoc;
PtrElement          pEl;

#endif /* __STDC__ */

{
   PtrAttribute         pAttr, pA;
   int                 len;
   Language            lang;

   if (pEl != NULL)
      if (GetTypedAttrForElem (pEl, 1, NULL) == NULL)
	 /* cet element n'a pas d'attribut langue */
	{
	   /* a priori, on prendra la langue par defaut */
	   lang = TtaGetDefaultLanguage ();
	   if (pEl != pDoc->DocRootElement)
	      /* ce n'est pas la racine de l'arbre principal, on cherche si */
	      /* l'arbre principal a un attribut langue et si oui, on prend */
	      /* cette langue */
	     {
		pAttr = GetTypedAttrForElem (pDoc->DocRootElement, 1, NULL);
		if (pAttr != NULL)
		   if (pAttr->AeAttrText != NULL)
		      lang = TtaGetLanguageIdFromName (pAttr->AeAttrText->BuContent);
	     }
	   /* change la langue des feuilles de texte */
	   ChangeLanguageLeaves (pEl, lang);
	   /* met l'attribut langue sur l'element */
	   GetAttr (&pAttr);
	   pAttr->AeAttrSSchema = pDoc->DocSSchema;
	   pAttr->AeAttrNum = 1;
	   pAttr->AeDefAttr = FALSE;
	   pAttr->AeAttrType = AtTextAttr;
	   GetTextBuffer (&pAttr->AeAttrText);
	   CopyStringToText (TtaGetLanguageName (lang), pAttr->AeAttrText, &len);
	   if (pEl->ElFirstAttr == NULL)
	      /* c'est le 1er attribut de l'element */
	      pEl->ElFirstAttr = pAttr;
	   else
	     {
		pA = pEl->ElFirstAttr;	/* 1er attribut de l'element */
		while (pA->AeNext != NULL)	/* cherche le dernier attr */
		   pA = pA->AeNext;
		pA->AeNext = pAttr;	/* chaine le nouvel attribut */
	     }
	   pAttr->AeNext = NULL;		/* c'est le dernier attribut */
	}
}
