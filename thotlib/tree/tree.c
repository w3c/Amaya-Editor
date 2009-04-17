/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * This module implements the operations on the abstract trees used by Thot.
 *
 * Author: V. Quint (INRIA)
 *
 */
#include "thot_gui.h"
#include "libmsg.h"
#include "thot_sys.h"
#include "message.h"
#include "constmedia.h"
#include "typemedia.h"
#include "fileaccess.h"
#include "res.h"
#include "labelAllocator.h"
#include "appdialogue.h"
#include "picture.h"
#include "application.h"

#define THOT_EXPORT extern
#include "select_tv.h"
#include "edit_tv.h"
#include "appdialogue_tv.h"

#include "absboxes_f.h"
#include "abspictures_f.h"
#include "applicationapi_f.h"
#include "content_f.h"
#include "exceptions_f.h"
#include "externalref_f.h"
#include "labelalloc_f.h"
#include "memory_f.h"
#include "references_f.h"
#include "schemas_f.h"
#include "structschema_f.h"
#include "thotmsg_f.h"
#include "tree_f.h"
#include "content.h"

/*----------------------------------------------------------------------
  DocumentOfElement
  returns the document to which a pEl element belongs.
  ----------------------------------------------------------------------*/
PtrDocument DocumentOfElement (PtrElement pEl)
{
  PtrElement          pE;
  int                 i;
  PtrDocument         pDoc;

  if (pEl == NULL)
    return NULL;

  pE = pEl;
  /* finds the root of the tree to which the element belongs */
  while (pE->ElParent != NULL)
    pE = pE->ElParent;

  /* searches among the roots of existing documents */
  for (i = 0; i < MAX_DOCUMENTS; i++)
    {
      pDoc = LoadedDocument[i];
      if (pDoc != NULL)
        if (pDoc->DocDocElement == pE)
          /* it's the root of the principal tree */
          return pDoc;
    }
  return NULL;
}

/*----------------------------------------------------------------------
  ProtectElement
  sets the ElIsCopy flag in the elements of the subtree of pEl.
  ----------------------------------------------------------------------*/
void ProtectElement (PtrElement pEl)
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
PtrElement GetOtherPairedElement (PtrElement pEl)
{
  PtrElement          pOther;
  PtrSSchema          pSS;
  int                 typeNum;
  ThotBool            found, begin;

  pOther = NULL;
  if (pEl != NULL)
    {
      if (pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber - 1]->SrConstruct ==
          CsPairedElement)
        {
          /* check if it's a pair element */
          if (pEl->ElOtherPairedEl != NULL)
            /* the element already has a pointer to the other element of the
               pair */
            pOther = pEl->ElOtherPairedEl;
          else
            {
              /* searches the mark having the same type and number */
              pSS = pEl->ElStructSchema;
              begin = pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber - 1]->
                SrFirstOfPair;
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
                    pOther = FwdSearchTypedElem (pOther, typeNum, pSS, NULL);
                  else
                    /* backward search */
                    pOther = BackSearchTypedElem (pOther, typeNum, pSS,NULL);
                  if (pOther != NULL)
                    /* we found an element having the same type as that of
                       the of the element we are searching */
                    /* it's a hit, if it has the same identifier */
                    found = (pOther->ElPairIdent == pEl->ElPairIdent ||
                             (pEl->ElPairIdent == 0 &&
                              pOther->ElOtherPairedEl == NULL));
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
        }
    }
  return pOther;
}

/*----------------------------------------------------------------------
  ElementIsHidden
  returns TRUE if the element pointed by pEl is hidden to the user, or 
  if it belongs to a hidden tree.
  ----------------------------------------------------------------------*/
ThotBool ElementIsHidden (PtrElement pEl)
{
  ThotBool isHI;

  if (ThotLocalActions[T_checkHiddenElement] != NULL)
    {
      (*(Proc2)ThotLocalActions[T_checkHiddenElement]) (pEl, &isHI);
      return isHI;
    }
  else
    return FALSE; /* No function => Element not hidden! */
}

/*----------------------------------------------------------------------
  ElementIsReadOnly
  returns TRUE if the element pointed by pEl is protected against user
  modifications, or if it belongs to a protected tree.
  ----------------------------------------------------------------------*/
ThotBool ElementIsReadOnly (PtrElement pEl)
{
  Proc2              Rofunction;
  ThotBool          isRO;

  if (!pEl)
    return FALSE;
  else if ((Rofunction = (Proc2)ThotLocalActions[T_checkReadOnlyElement]) == NULL)
    return FALSE; /* No function => Element not protected! */
  else
    {
      (*Rofunction) (pEl, &isRO);
      return isRO;
    }
}

/*----------------------------------------------------------------------
  CannotInsertNearElement
  returns TRUE if a new element cannot be inserted near the element
  pointed by pEl.
  returns FALSE if insertion is authorized.
  If beforeElement = TRUE, it is an insertion before the element.
  If FALSE,  it is an insertion after the element.
  if it belongs to a hidden tree.
  Rules to authorize or not such insertion depends of the
  application criteria.
  ----------------------------------------------------------------------*/
ThotBool CannotInsertNearElement (PtrElement pEl, ThotBool beforeElement)
{
  register Proc3 InsertNearFunction;
  ThotBool      isForbidden;

  if ((InsertNearFunction = (Proc3)ThotLocalActions[T_checkInsertNearElement]) == NULL)
    return FALSE; /* No function => Insertion is authorized! */
  else
    {
      (*InsertNearFunction) ((void *)pEl, (void *)(beforeElement ? 0xFFFFFF : 0x000000), (void *)&isForbidden);
      return isForbidden;
    }

}

/*----------------------------------------------------------------------
  SearchTypedElementInSubtree					
  ----------------------------------------------------------------------*/
PtrElement SearchTypedElementInSubtree (PtrElement pEl, int typeNum,
                                        PtrSSchema pSS)
{
  PtrElement          pRet, pChild;

  pRet = NULL;
  if (pEl->ElStructSchema == pSS && pEl->ElTypeNumber == typeNum)
    /* got a hit on the element */
    pRet = pEl;
  else if (!pEl->ElTerminal)
    /* recursive search among the children of the element */
    {
      pChild = pEl->ElFirstChild;
      while (pChild && !pRet)
        {
          pRet = SearchTypedElementInSubtree (pChild, typeNum, pSS);
          pChild = pChild->ElNext;
        }
    }
  return pRet;
}

/*----------------------------------------------------------------------
  FwdSearchTypeNameInSubtree					
  ----------------------------------------------------------------------*/
static PtrElement FwdSearchTypeNameInSubtree (PtrElement pEl, ThotBool test,
                                              char *typeName)
{
  PtrElement          pRet, pChild;

  pRet = NULL;
  if (test &&
      strcmp (typeName, pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber - 1]->SrName) == 0)
    /* got a hit on the element */
    pRet = pEl;
  if (pRet == NULL && !pEl->ElTerminal)
    /* a recursive search among the children of the element */
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
PtrElement FwdSearchElemByTypeName (PtrElement pEl, char *typeName)
{
  PtrElement          pRet, pCur, pAsc;
  ThotBool            stop;

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
             with the next sibling */
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
                    {
                      if (strcmp (typeName, pAsc->ElStructSchema->SsRule->SrElem[pAsc->ElTypeNumber - 1]->SrName) == 0)
                        /* found */
                        pRet = pAsc;
                      else
                        pRet = FwdSearchElemByTypeName (pAsc, typeName);
                    }
                }
            }
        }
    }
  return pRet;
}

/*----------------------------------------------------------------------
  BackSearchTypeNameInSubtree				       
  ----------------------------------------------------------------------*/
static PtrElement BackSearchTypeNameInSubtree (PtrElement pEl, char *typeName)
{
  PtrElement          pRet, pChild;

  pRet = NULL;
  /* Searches the last child */
  if (!pEl->ElTerminal)
    {
      pChild = pEl->ElFirstChild;
      if (pChild != NULL)
        while (pChild->ElNext != NULL)
          pChild = pChild->ElNext;
      /* search starting from previous siblings */
      while (pChild != NULL && pRet == NULL)
        {
          pRet = BackSearchTypeNameInSubtree (pChild, typeName);
          pChild = pChild->ElPrevious;
        }
    }
  if (pRet == NULL)
    {
      if (strcmp (typeName, pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber - 1]->SrName) == 0)
        pRet = pEl;		/* found ! it's the element itself */
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
PtrElement BackSearchElemByTypeName (PtrElement pEl, char *typeName)
{
  PtrElement          pRet, pCur;

  pRet = NULL;			/* pRet: search result */
  if (pEl != NULL)
    /* searches in the subtrees of the siblings preceding the element */
    {
      pCur = pEl->ElPrevious;
      while (pCur != NULL && pRet == NULL)
        {
          pRet = BackSearchTypeNameInSubtree (pCur, typeName);
          pCur = pCur->ElPrevious;
        }
      /* if it fails, verifies if it's the parent, then it searches in
         the subtrees of the uncles of the element */
      if (pRet == NULL)
        if (pEl->ElParent != NULL)
          {
            pEl = pEl->ElParent;
            if (strcmp (typeName, pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber - 1]->SrName) == 0)
              pRet = pEl;	/* found, it's the parent */
            else
              pRet = BackSearchElemByTypeName (pEl, typeName);
          }
    }
  return pRet;
}

/*----------------------------------------------------------------------
  BackSearchVisibleSubtree                                        
  ----------------------------------------------------------------------*/
static PtrElement BackSearchVisibleSubtree (PtrElement pEl, int *view)
{
  PtrElement          pRet, pChild;

  pRet = NULL;
  if (pEl->ElAbstractBox[*view - 1] != NULL)
    pRet = pEl;
  else
    /* Searches the last child */
    if (!pEl->ElTerminal)
      {
        pChild = pEl->ElFirstChild;
        if (pChild != NULL)
          while (pChild->ElNext != NULL)
            pChild = pChild->ElNext;
        /* search starting from the previous siblings */
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
static ThotBool AttrFound (PtrElement pEl, int val, const char *textVal,
                           int attNum, PtrSSchema pSS)
{
  PtrAttribute        pAttr;
  ThotBool            ret;

  ret = FALSE;
  if (pEl->ElFirstAttr != NULL)
    {
      /* element has an attribute */
      if (pSS == NULL && attNum == 0)
        /* we search any attribute */
        ret = TRUE;		/* found ! */
      else
        /* searches the element's attributes */
        {
          pAttr = pEl->ElFirstAttr;
          while (pAttr != NULL && pAttr->AeAttrSSchema != NULL && !ret)
            {
              if ((pSS == NULL ||
                   !strcmp (pAttr->AeAttrSSchema->SsName, pSS->SsName))
                  && pAttr->AeAttrNum == attNum)
                {
                  /* it's the attribute we are searching */
                  if (val == 0)
                    /* we are looking for any value */
                    ret = TRUE;	/* found ! */
                  else
                    switch (pAttr->AeAttrType)
                      {
                      case AtNumAttr:
                      case AtEnumAttr:
                        if (pAttr->AeAttrValue == val)
                          /* searched value */
                          ret = TRUE;	/* found ! */
                        break;
                      case AtTextAttr:
                        if (StringAndTextEqual (textVal, pAttr->AeAttrText))
                          ret = TRUE;
                        break;
                      default:
                        break;
                      }
                }
              pAttr = pAttr->AeNext;
            }
        }
    }
  return ret;
}

/*----------------------------------------------------------------------
  FwdSearchAttrInSubtree
  ----------------------------------------------------------------------*/
static PtrElement FwdSearch2AttrInSubtree (PtrElement pEl, ThotBool test,
                                           int val, const char *textVal,
                                           int attNum, int attNum2,
                                           PtrSSchema pSS, PtrSSchema pSS2)
{
  PtrElement          pRet, pChild;

  pRet = NULL;
  if (test &&
      (AttrFound (pEl, val, textVal, attNum, pSS) ||
       (attNum2 && AttrFound (pEl, 0, "", attNum2, pSS2))))
    pRet = pEl;
  if (pRet == NULL && !pEl->ElTerminal)
    /* search among the children of the element */
    {
      pChild = pEl->ElFirstChild;
      while (pChild != NULL && pRet == NULL)
        {
          pRet = FwdSearch2AttrInSubtree (pChild, TRUE, val, textVal,
                                          attNum, attNum2, pSS, pSS2);
          pChild = pChild->ElNext;
        }
    }
  return pRet;
}

/*----------------------------------------------------------------------
  BackSearchAttrInSubtree
  ----------------------------------------------------------------------*/
static PtrElement BackSearch2AttrInSubtree (PtrElement pEl, int val,
                                            const char *textVal,
                                            int attNum, int attNum2,
                                            PtrSSchema pSS, PtrSSchema pSS2)
{
  PtrElement          pRet, pChild;

  pRet = NULL;
  if (!pEl->ElTerminal)
    {
      /* searches the last child */
      pChild = pEl->ElFirstChild;
      if (pChild != NULL)
        while (pChild->ElNext != NULL)
          pChild = pChild->ElNext;
      /* search the previous siblings */
      while (pChild != NULL && pRet == NULL)
        {
          pRet = BackSearch2AttrInSubtree (pChild, val, textVal,
                                           attNum, attNum2, pSS, pSS2);
          pChild = pChild->ElPrevious;
        }
    }
  if (!pRet)
    {
      if (AttrFound (pEl, val, textVal, attNum, pSS) ||
          (attNum2 && AttrFound (pEl, 0, "", attNum2, pSS2)))
        pRet = pEl;
    }
  return pRet;
}

/*----------------------------------------------------------------------
  ElemIsEmptyOrRefOrPair
  tests if the element pointed by pEL is empty (Kind = 1), if it's a 
  reference element (Kind = 2), or if its a pair element (Kind = 3).
  Returns TRUE or FALSE, according to the result of the test.
  ----------------------------------------------------------------------*/
static ThotBool ElemIsEmptyOrRefOrPair (PtrElement pEl, int Kind)
{
  ThotBool            ret;

  ret = FALSE;
  switch (Kind)
    {
    case 1:		/* empty element ? */
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
          case LtPolyLine:
			      if (pEl->ElNPoints == 0)
              ret = TRUE;
			      break;
          case LtPath:
			      if (pEl->ElFirstPathSeg == NULL)
              ret = TRUE;
			      break;
          case LtSymbol:
          case LtGraphics:
			      if (pEl->ElGraph == EOS)
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
    case 2:		/* reference element ? */
      if (pEl->ElTerminal)
        if (pEl->ElLeafType == LtReference)
          ret = TRUE;
      break;
    case 3:		/* paired element ? */
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
static PtrElement FwdSearchEmptyInSubtree (PtrElement pEl, ThotBool test, int Kind)
{
  PtrElement          pRet, pChild;

  pRet = NULL;
  if (test)
    if (ElemIsEmptyOrRefOrPair (pEl, Kind))
      pRet = pEl;
  if (pRet == NULL && !pEl->ElTerminal)
    /* searches among the children of the element */
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
PtrElement          FwdSearchRefOrEmptyElem (PtrElement pEl, int Kind)
{
  PtrElement          pRet, pCur, pAsc;
  ThotBool            stop;

  pRet = NULL;
  if (pEl != NULL)
    /* searches the subtree of the element */
    {
      pRet = FwdSearchEmptyInSubtree (pEl, FALSE, Kind);
      if (pRet == NULL)
        /* if failure, search the subtrees of the element's next siblings */
        {
          pCur = pEl->ElNext;
          while (pCur != NULL && pRet == NULL)
            {
              pRet = FwdSearchEmptyInSubtree (pCur, TRUE, Kind);
              pCur = pCur->ElNext;
            }
          /* if failure, searches the first ancestor with a next sibling */
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
                /* verifies if its the element we are looking for */
                {
                  pAsc = pAsc->ElNext;
                  if (pAsc != NULL)
                    {
                      if (ElemIsEmptyOrRefOrPair (pAsc, Kind))
                        pRet = pAsc;	/* found */
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
static PtrElement   BackSearchEmptyInSubtree (PtrElement pEl, int Kind)
{
  PtrElement          pRet, pChild;

  pRet = NULL;
  /* Searches the last children */
  if (!pEl->ElTerminal)
    {
      pChild = pEl->ElFirstChild;
      if (pChild != NULL)
        while (pChild->ElNext != NULL)
          pChild = pChild->ElNext;
      /* searches the previous siblings */
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
PtrElement BackSearchRefOrEmptyElem (PtrElement pEl, int Kind)
{
  PtrElement          pRet, pCur;

  pRet = NULL;			/* pRet: search result */
  if (pEl != NULL)
    /* searches the subtrees of the siblings preceding the element */
    {
      pCur = pEl->ElPrevious;
      while (pCur != NULL && pRet == NULL)
        {
          pRet = BackSearchEmptyInSubtree (pCur, Kind);
          pCur = pCur->ElPrevious;
        }
      /* if failure, verifies if it's not the parent. If not, it verifies
         all the subtrees of the element's uncles */

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
  ChangeLanguageLeaves
  changes the language for all the text leaves in the
  subtree of the element pointed by pEl.
  ----------------------------------------------------------------------*/
void ChangeLanguageLeaves (PtrElement pEl, Language lang)
{
  PtrElement          pChild;

  if (pEl == NULL)
    return;
  if (!pEl->ElTerminal)
    /* The element is not a leaf. We recursively process all of the */
    /* element's subtree */
    {
      pChild = pEl->ElFirstChild;
      while (pChild)
        {
          /* if the child has a Language attribute, don't change it */
          if (GetTypedAttrForElem (pChild, 1, NULL) == NULL)
            ChangeLanguageLeaves (pChild, lang);
          pChild = pChild->ElNext;
        }
    }
  else if (pEl->ElLeafType == LtText &&
           pEl->ElLanguage != lang && pEl->ElLanguage  >= 4)
    /*
      Changes the language of the element:
      lang < TtaGetFirstUserLanguage () when a specific script like symbol
    */
    pEl->ElLanguage = lang;
}


/*----------------------------------------------------------------------
  LeavesInheritLanguage
  makes the text leaves of the subtree of pEl inherit the language
  defined by Language attribute of pEl or, if it does not have one,
  by that of one of his ancestors.
  ----------------------------------------------------------------------*/
static void LeavesInheritLanguage (PtrElement pEl)
{
  PtrAttribute        pAttr;
  PtrElement          pElAttr;
  unsigned char       text[400];
  Language            lang;

  if (GetTypedAttrForElem (pEl, 1, NULL) == NULL)
    /* pEl does not have a Language attribute */
    {
      /* searches the ancestors of pEl for a Language attribute */
      pAttr = GetTypedAttrAncestor (pEl, 1, NULL, &pElAttr);
      if (pAttr && pAttr->AeAttrText &&
          pAttr->AeAttrText->BuContent[0] != EOS)
        /* this Language attribute has a value */
        {
          CopyBuffer2MBs (pAttr->AeAttrText, 0, text, 399);
          lang = TtaGetLanguageIdFromName ((char *)text);
          /* changes the language of the text leaves of the subtree */
          ChangeLanguageLeaves (pEl, lang);
        }
    }
}


/*----------------------------------------------------------------------
  InsertElemAfterLastSibling
  Inserts the element pointed by pNew (and his next siblings), after
  the last element following the one pointed by pOld.
  ----------------------------------------------------------------------*/
void InsertElemAfterLastSibling (PtrElement pOld, PtrElement pNew)
{
  PtrElement          pEl;
  PtrElement          pAsc;

  if (pNew != NULL && pOld != NULL)
    {
      pEl = pOld;
      while (pEl->ElNext != NULL)
        pEl = pEl->ElNext;
      /* pEl points to the last element following pOld */
      pEl->ElNext = pNew;
      pNew->ElPrevious = pEl;
      pEl = pNew;
      do
        {
          pEl->ElParent = pOld->ElParent;
          /* makes all text leaves of the element inherit the
             language defined by the language attribute of pEl or,
             if it doesn't have one, by the first one of his ancestors
             having it
          */
          LeavesInheritLanguage (pEl);
          /* updates the volume of the enclosing elements */
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
  CopyAttributes
  copies the attributs of the source element pEl1 belonging to document
  pSourceDoc into the target element pEl2 belonging to document pTargetDoc.
  If Check is TRUE, check that the target document uses the structure
  schemas that define the attributes to be copied.
  If copyRef is FALSE reference attributes are not copied.
  ----------------------------------------------------------------------*/
void CopyAttributes (PtrElement pEl1, PtrElement pEl2,
                     PtrDocument pSourceDoc, PtrDocument pTargetDoc,
                     ThotBool Check, ThotBool copyRef)
{
  PtrAttribute        pAttr1, pAttr2, pPrevAttr;
  PtrReference        rf;
  PtrReference        pPr;
  int                 len, nR;
  ThotBool            ok;

  /* no attributes (yet) on the copy */
  pEl2->ElFirstAttr = NULL;	
  if (pEl1->ElFirstAttr != NULL)
    /* there are some attributes on the source element, so let's copy them */
    {
      pAttr1 = pEl1->ElFirstAttr;	/* attribute to copy */
      pPrevAttr = NULL;	/* last attribute of the target */
      pAttr2 = NULL;
      /* fill and link all copied elements */
      do
        {
          if (copyRef || pAttr1->AeAttrType != AtReferenceAttr)
            {
              if (pAttr2 == NULL)
                /* gets an attribute block for the target */
                GetAttribute (&pAttr2);	
              /* copies the attribute */
              *pAttr2 = *pAttr1;	
              if (pAttr2->AeAttrType == AtTextAttr)
                /* it's a text attribute and it does not yet have a buffer */
                pAttr2->AeAttrText = NULL;
              pAttr2->AeNext = NULL;
              ok = TRUE;
              if (Check && pSourceDoc != pTargetDoc)
                /* check that the structure schema defining the attribute is
                   known in the target document */
                if (!GetSSchemaForDoc (pAttr1->AeAttrSSchema->SsName, pTargetDoc))
                  {
                    nR = CreateNature (NULL, pAttr1->AeAttrSSchema->SsName, NULL,
                                       pTargetDoc->DocSSchema, pTargetDoc);
                    if (nR == 0)
                      /* can't create the schema for the target document. Don't
                         copy this attribute */
                      ok = FALSE;
                    else
                      {
                        /* schema loaded. Change the structure schema of the copy */
                        pAttr2->AeAttrSSchema = pTargetDoc->DocSSchema->
                          SsRule->SrElem[nR - 1]->SrSSchemaNat;
                        AddSchemaGuestViews (pTargetDoc, pAttr2->AeAttrSSchema);
                      }
                  }
              if (ok)
                {
                  if (pAttr1->AeAttrType == AtReferenceAttr)
                    /* it's a reference attribute, we copy the reference */
                    {
                      if (pAttr1->AeAttrReference == NULL)
                        pAttr2->AeAttrReference = NULL;
                      else
                        {
                          /* gets a reference */
                          GetReference (&rf);		
                          pAttr2->AeAttrReference = rf;
                          /* fills the new reference */
                          pPr = pAttr2->AeAttrReference;
                          pPr->RdElement = pEl2;
                          pPr->RdAttribute = pAttr2;
                          pPr->RdReferred = pAttr1->AeAttrReference->RdReferred;
                          pPr->RdTypeRef = pAttr1->AeAttrReference->RdTypeRef;
                          if (pPr->RdReferred != NULL)
                            /* puts the new reference at the head of the link */
                            {
                              pPr->RdNext = pPr->RdReferred->ReFirstReference;
                              if (pPr->RdNext != NULL)
                                pPr->RdNext->RdPrevious = rf;
                              pPr->RdReferred->ReFirstReference = rf;
                            }
                        }
                    }
                  else if (pAttr2->AeAttrType == AtTextAttr)
                    /* it's a text attribute, we attach a text buffer to it */
                    /* where we'll copy the context */
                    if (pAttr1->AeAttrText != NULL)
                      {
                        GetTextBuffer (&pAttr2->AeAttrText);
                        CopyTextToText (pAttr1->AeAttrText, pAttr2->AeAttrText,
                                        &len);
                      }
                  /* links the attribute to the target */
                  if (pPrevAttr == NULL)
                    /* first attribute of the target */
                    pEl2->ElFirstAttr = pAttr2;
                  else
                    pPrevAttr->AeNext = pAttr2;
                  pPrevAttr = pAttr2;
                  pAttr2 = NULL;
                }
            }
          /* continues with the next attribute of the source */
          pAttr1 = pAttr1->AeNext;
        }
      while (pAttr1);
      if (pAttr2)
        FreeAttribute (pAttr2);
    }
}

/*----------------------------------------------------------------------
  CopyPresRules
  copies the specific presentation rules of the source element pointed
  by pEl into the target element pointed by pEl2.
  ----------------------------------------------------------------------*/
static void CopyPresRules (PtrElement pEl, PtrElement pEl2)
{
  PtrPRule       pRC1, pRC2, pRS;


  if (pEl->ElFirstPRule == NULL)
    pEl2->ElFirstPRule = NULL;	/* no specific presentation */
  else
    {
      GetPresentRule (&pRC1);	/* gets a first rule */
      pEl2->ElFirstPRule = pRC1;
      *pRC1 = *pEl->ElFirstPRule;	/* copies the first rule */
      pRS = pEl->ElFirstPRule->PrNextPRule;
      while (pRS != NULL)
        {
          /* gets a rule for the target */
          GetPresentRule (&pRC2);
          /* copies the content */
          *pRC2 = *pRS;
          /* links this rule to the precedent one */
          pRC1->PrNextPRule = pRC2;
          pRC1 = pRC2;
          pRS = pRS->PrNextPRule;
        }
    }
}


/*----------------------------------------------------------------------
  ElemIsWithinSubtree
  returns TRUE if the element pointed by pEl belongs to the subtree
  whose root is given by pRoot.
  ----------------------------------------------------------------------*/
ThotBool ElemIsWithinSubtree (PtrElement pEl, PtrElement pRoot)
{
  ThotBool            within;

  if (pEl == NULL || pEl->ElStructSchema == NULL || pRoot == NULL)
    return FALSE;

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
  Tests if the type of pEl is equivalent to typeNum.
  ----------------------------------------------------------------------*/
ThotBool EquivalentType (PtrElement pEl, int typeNum, PtrSSchema pSS)
{
  ThotBool            ok;
  PtrSRule            pRe1, pRe2;
  int                 i;
  ThotBool            SSok;

  if (!pEl)
    return FALSE;
  ok = FALSE;			/* assume a FALSE reply */
  if (pSS == NULL)
    SSok = TRUE;		/* use any struct. scheme */
  else
    /* compares the identifier of the structure scheme */
    SSok = !strcmp (pEl->ElStructSchema->SsName, pSS->SsName);
  if (SSok && pEl->ElTypeNumber == typeNum)
    ok = TRUE;
  else if (SSok && typeNum == 1 && pEl->ElTerminal && pEl->ElLeafType == LtText &&
           TypeHasException (ExcSelectParent, pEl->ElTypeNumber, pEl->ElStructSchema))
    // this is a patch to manage <br> like a string for copy/paste
    ok = TRUE;
  else if (pSS && (SSok || pEl->ElTerminal))
    {
      pRe1 = pSS->SsRule->SrElem[typeNum - 1];
      /* rule defining the type of element we are searching */
      if (pRe1->SrConstruct == CsChoice)
        {
          if (pRe1->SrNChoices > 0)
            /* the searched type is an explicit choice
               the current element is defined in the same scheme
               as that of the searched element */
            {
              i = 0;
              do
                /* compares the element type with the options of the choice */
                {
                  if (pEl->ElTypeNumber == pRe1->SrChoice[i])
                    ok = !strcmp (pEl->ElStructSchema->SsName, pSS->SsName);
                  else
                    {
                      pRe2 = pSS->SsRule->SrElem[pRe1->SrChoice[i] - 1];
                      if (pRe2->SrConstruct == CsNatureSchema)
                        /* the choice option is a change of nature */
                        /* keeps the root elements of this nature */
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
          /* the searched type is a change of nature, we keep the
             root elements of this nature */
          if (pEl->ElStructSchema == pRe1->SrSSchemaNat)
            if (pEl->ElTypeNumber == pRe1->SrSSchemaNat->SsRootElem)
              ok = TRUE;
        }
    }
  return ok;
}


/*----------------------------------------------------------------------
  ElemIsBefore
  returns TRUE if the element pointed by pEl1 is begfore the element pointed
  by pEl2 in the document order. The element itself and its ancestors are not
  taken into account during this test.
  ----------------------------------------------------------------------*/
ThotBool            ElemIsBefore (PtrElement pEl1, PtrElement pEl2)
{
  PtrElement          pEl;
  ThotBool            found;
  ThotBool            before;

  if (pEl1 == pEl2)
    before = FALSE;
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
      before = found;
    }
  return before;
}

/*----------------------------------------------------------------------
  ElemIsBeforeWithin
  returns TRUE if the element pointed by pEl1 is before the element pointed
  by pEl2. Both elements belong to the same subtree.
  ----------------------------------------------------------------------*/
ThotBool            ElemIsBeforeWithin (PtrElement pEl1, PtrElement pEl2,
                                        PtrElement subTree)
{
  PtrElement          pEl;
  ThotBool            found;
  ThotBool            before;

  if (pEl1 == pEl2)
    before = FALSE;
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
          if (pEl == subTree)
            pEl = NULL;
        }
      before = found;
    }
  return before;
}

/*----------------------------------------------------------------------
  ElemIsAnAncestor
  returns TRUE if the element pointed by pEl1 is an ancestor of the
  element pointed by pEl2.
  ----------------------------------------------------------------------*/
ThotBool            ElemIsAnAncestor (PtrElement pEl1, PtrElement pEl2)
{
  PtrElement          p;
  ThotBool            found;
  ThotBool            ancestor;

  if (pEl1 == NULL)
    ancestor = FALSE;
  else if (pEl2 == NULL)
    ancestor = TRUE;
  else
    {
      found = FALSE;
      p = pEl2->ElParent;
      while (!found && p != NULL)
        if (p == pEl1)
          found = TRUE;
        else
          p = p->ElParent;
      ancestor = found;
    }
  return ancestor;
}

/*----------------------------------------------------------------------
  CommonAncestor
  finds the least sized tree containing both pEl1 and pEl2.
  Returns a pointed to the root of this subtree or NULL if pEl1 and
  pEl2 are not in the same tree.
  ----------------------------------------------------------------------*/
PtrElement          CommonAncestor (PtrElement pEl1, PtrElement pEl2)
{
  PtrElement          pAsc;
  ThotBool            stop;

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
  FirstLeaf
  returns a pointer to the first element without descendants in the
  subtree of the element  pointed by pEl.
  ----------------------------------------------------------------------*/
PtrElement          FirstLeaf (PtrElement pEl)
{
  PtrElement          pE;
  ThotBool            stop;
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
              /* ignore page marks */
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
  LastLeaf
  returns a pointer to the last element without descendants in the
  subtree of the element pointed by pEl.
  ----------------------------------------------------------------------*/
PtrElement          LastLeaf (PtrElement pRoot)
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
  GetTypedAncestor
  given an element pointed by pEl, it returns a pointer to the ancestor
  element of pEl having type typeNum of the structure scheme pSS.
  Returns NULL if no such element exists.
  ----------------------------------------------------------------------*/
PtrElement  GetTypedAncestor (PtrElement pEl, int typeNum, PtrSSchema pSS)
{
  ThotBool            found;
  PtrElement          pEl1;
  PtrElement          pAsc;

  found = FALSE;
  if (pEl != NULL && typeNum > 0 && pSS != NULL)
    do
      {
        pEl1 = pEl;
        if (pSS->SsRule->SrElem[typeNum - 1]->SrConstruct == CsNatureSchema)
          {
            /* the searched type has a given nature */
            if (pEl1->ElTypeNumber == pEl1->ElStructSchema->SsRootElem)
              /* the current element is the root of a nature, the current element
                 is appropriate if we have the same structure schemes */
              found = (strcmp (pEl1->ElStructSchema->SsName, pSS->SsRule->SrElem[typeNum - 1]->SrOrigNat) == 0);
          }
        else
          found = EquivalentSRules (typeNum, pSS, pEl1->ElTypeNumber, pEl1->ElStructSchema, pEl);
        if (!found)
          pEl = pEl->ElParent;	/* moves up to the parent */
      }
    while (pEl != NULL && !found);

  if (found)
    {
      /* if the found element has only one type equivalent to the type we are looking for,
         we search if an ancestor does not have this type */
      if (typeNum != pEl->ElTypeNumber &&
          pEl->ElParent != NULL &&
          pEl->ElParent->ElTypeNumber == typeNum &&
          !strcmp (pEl->ElParent->ElStructSchema->SsName, pSS->SsName))
        pEl = pEl->ElParent;
      pAsc = pEl;
    }
  else
    pAsc = NULL;
  return pAsc;
}


/*----------------------------------------------------------------------
  FwdSearch2TypesInSubtree
  The parameter test is TRUE if pEl should be tested too.
  ----------------------------------------------------------------------*/
PtrElement FwdSearch2TypesInSubtree (PtrElement pEl, ThotBool test,
                                     int typeNum2, int typeNum1,
                                     PtrSSchema pSS2, PtrSSchema pSS1)
{
  PtrElement          pRet, pChild;

  pRet = NULL;
  if (test)
    {
      if (EquivalentType (pEl, typeNum1, pSS1) ||
          EquivalentType (pEl, typeNum2, pSS2) || typeNum1 == 0)
        /* element found */
        pRet = pEl;
    }
  if (pRet == NULL && !pEl->ElTerminal)
    /* search the children */
    {
      pChild = pEl->ElFirstChild;
      while (pChild != NULL && pRet == NULL)
        {
          pRet = FwdSearch2TypesInSubtree (pChild, TRUE, typeNum2, typeNum1,
                                           pSS2, pSS1);
          pChild = pChild->ElNext;
        }
    }
  return pRet;
}


/*----------------------------------------------------------------------
  FwdSearchElem2Types
  starting from the element pointed by pEl search forward in the tree an
  element of type typeNum1 (defined in the structure schema pointed by pSS1)
  or of typeNum2 (defined in the structure schema pointed by pSS2).
  If pSS1 (resp. pSS2) is NULL, the search will stop on any element whose
  type number is typeNum1 (resp. peNum2), independently of its structure
  schema. This is useful for searching basic elements such as character
  strings, symbols, graphic elements, etc.
  If pAncestor is NULL, the search may proceed to the end of the whole
  abstract tree.
  If pAncestor is not NULL, the search is performed only in the subtree
  rooted at pAncestor.
  The function returns a pointer to the element found or NULL if not found.
  ----------------------------------------------------------------------*/
PtrElement FwdSearchElem2Types (PtrElement pEl, int typeNum1, int typeNum2,
                                PtrSSchema pSS1, PtrSSchema pSS2,
                                PtrElement pAncestor)
{
  PtrElement          pRet, pCur, pAsc;
  ThotBool            stop;

  pRet = NULL;
  if (pEl != NULL)
    /* searches the subtree of the element */
    {
      pRet = FwdSearch2TypesInSubtree (pEl, FALSE, typeNum2, typeNum1, pSS2,
                                       pSS1);
      if (pRet == NULL && pEl != pAncestor)
        /* failure. Search the subtrees of the next siblings of the element */
        {
          pCur = pEl->ElNext;
          while (pCur && pCur->ElStructSchema && pRet == NULL)
            {
              pRet = FwdSearch2TypesInSubtree (pCur, TRUE, typeNum2,
                                               typeNum1, pSS2, pSS1);
              pCur = pCur->ElNext;
            }
          /* if failure, search the first ancestor with a following sibling */
          if (pRet == NULL)
            {
              stop = FALSE;
              pAsc = pEl;
              do
                {
                  pAsc = pAsc->ElParent;
                  if (pAsc == NULL)
                    stop = TRUE;
                  else if (pAsc == pAncestor)
                    {
                      pAsc = NULL;
                      stop = TRUE;
                    }
                  else if (pAsc->ElNext)
                    stop = TRUE;
                }
              while (!stop);
              if (pAsc != NULL)
                /* verifies if this element is the one we're looking for */
                {
                  pAsc = pAsc->ElNext;
                  if (pAsc != NULL)
                    {
                      if (EquivalentType (pAsc, typeNum1, pSS1) ||
                          EquivalentType (pAsc, typeNum2, pSS2) ||
                          typeNum1 == 0)
                        pRet = pAsc;	/* found */
                      else
                        pRet = FwdSearchElem2Types (pAsc, typeNum1, typeNum2,
                                                    pSS1, pSS2, pAncestor);
                    }
                }
            }
        }
    }
  return pRet;
}

/*----------------------------------------------------------------------
  FwdSearchTypedElem
  starting from the element pointed by pEl, makes a forward search
  for an element of type typeNum defined in the structure schema
  pointed by pSS. If pSS is NULL, it ignores the structure schema.
  This is useful for searching basic elements such as character
  strings, symbols, graphic elements, ...
  If pAncestor is NULL, the search may proceed to the end of the whole
  abstract tree.
  If pAncestor is not NULL, the search is performed only in the subtree
  rooted at pAncestor.
  The function returns a pointer to the element found, or NULL if not found
  ----------------------------------------------------------------------*/
PtrElement FwdSearchTypedElem (PtrElement pEl, int typeNum, PtrSSchema pSS,
                               PtrElement pAncestor)
{
  return FwdSearchElem2Types (pEl, typeNum, 0, pSS, NULL, pAncestor);
}

/*----------------------------------------------------------------------
  BackSearch2TypesInSubtree                                      
  ----------------------------------------------------------------------*/
static PtrElement BackSearch2TypesInSubtree (PtrElement pEl, int typeNum2,
                                             int typeNum1, PtrSSchema pSS2,
                                             PtrSSchema pSS1)
{
  PtrElement          pRet, pChild;

  pRet = NULL;
  /* searches the last child */
  if (!pEl->ElTerminal)
    {
      pChild = pEl->ElFirstChild;
      if (pChild)
        while (pChild->ElNext)
          pChild = pChild->ElNext;
      /* searches from the precedent siblings */
      while (pChild && pRet == NULL)
        {
          pRet = BackSearch2TypesInSubtree (pChild, typeNum2, typeNum1, pSS2, pSS1);
          pChild = pChild->ElPrevious;
        }
    }
  if (pRet == NULL)
    {
      if (EquivalentType (pEl, typeNum1, pSS1) ||
          EquivalentType (pEl, typeNum2, pSS2) || typeNum1 == 0)
        pRet = pEl;		/* found */
    }
  return pRet;
}

/*----------------------------------------------------------------------
  BackSearchElem2Types
  starting from the element pointed by pEl search backwards in the tree
  an element of type typeNum1 (defined in the structure schema pointed
  by pSS1) or of typeNum2 (defined in the structure schema pointed by pSS2).
  If pSS1 (resp. pSS2) is NULL, the search will stop on any element whose
  type number is typeNum1 (resp. typeNum2), independently of its structure
  schema.  This is useful for searching basic elements such as character
  strings symbols, graphic elements, ...
  If pAncestor is NULL, the search may proceed to the end of the whole
  abstract tree.
  If pAncestor is not NULL, the search is performed only in the subtree
  rooted at pAncestor.
  The function returns a pointer to the element found or NULL.
  ----------------------------------------------------------------------*/
PtrElement BackSearchElem2Types (PtrElement pEl, int typeNum1, int typeNum2,
                                 PtrSSchema pSS1, PtrSSchema pSS2,
                                 PtrElement pAncestor)
{
  PtrElement          pRet, pCur;

  pRet = NULL;	 /* pRet: result of the search */
  if (pEl)
    /* search in the subtrees of the siblings preceding the element */
    {
      pCur = pEl->ElPrevious;
      while (pCur != NULL && pRet == NULL)
        {
          pRet = BackSearch2TypesInSubtree (pCur, typeNum2, typeNum1, pSS2,
                                            pSS1);
          pCur = pCur->ElPrevious;
        }
      /* if failure, verify if it's the parent, then search in the subtrees of
         the uncles of the element */
      if (pRet == NULL)
        if (pEl->ElParent && pEl->ElParent != pAncestor)
          {
            pEl = pEl->ElParent;
            if (EquivalentType (pEl, typeNum1, pSS1) ||
                EquivalentType (pEl, typeNum2, pSS2))
              pRet = pEl;	/* found, it's the parent */
            else
              pRet = BackSearchElem2Types (pEl, typeNum1, typeNum2, pSS1, pSS2,
                                           pAncestor);
          }
    }
  return pRet;
}


/*----------------------------------------------------------------------
  BackSearchTypedElem
  starting from the element pointed by pEl, do a backward search
  for an element of type typeNum defined in the structure schema
  pointed by pSS. If pSS is NULL, ignore the structure schema.
  This is useful for searching basic elements such as character
  strings, symbols, graphic elements, ...
  If pAncestor is NULL, the search may proceed to the beginning of
  the whole abstract tree.
  If pAncestor is not NULL, the search is performed only in the
  subtree rooted at pAncestor.
  The function returns a pointer to the element found or NULL.
  ----------------------------------------------------------------------*/
PtrElement BackSearchTypedElem (PtrElement pEl, int typeNum, PtrSSchema pSS,
                                PtrElement pAncestor)
{
  return BackSearchElem2Types (pEl, typeNum, 0, pSS, NULL, pAncestor);
}


/*----------------------------------------------------------------------
  BackSearchVisibleElem
  searches in the subtree whose root is pointed by pRoot an element
  preceding the one pointed by pEl and having an abstract box in the
  view "view".
  Returns a pointer to that element or NULL.
  ----------------------------------------------------------------------*/
PtrElement BackSearchVisibleElem (PtrElement pRoot, PtrElement pEl, int view)
{
  PtrElement          pRet, pCur;

  pRet = NULL;
  if (pEl != NULL)
    /* searches in the subtrees of the siblings preceding the element */
    {
      pCur = pEl->ElPrevious;
      while (pCur != NULL && pRet == NULL)
        {
          pRet = BackSearchVisibleSubtree (pCur, &view);
          pCur = pCur->ElPrevious;
        }
      /* if failure, searches in the subtrees of the uncles of the element */
      if (pRet == NULL)
        if (pEl->ElParent != NULL && pEl->ElParent != pRoot)
          pRet = BackSearchVisibleElem (pRoot, pEl->ElParent, view);
    }
  return pRet;
}


/*----------------------------------------------------------------------
  FwdSearchAttribute 
  starting from the pEl element, forward searches in the tree an element
  having the attribute attNum defined in the structure scheme pSS and having
  a val value. If pSS is NULL, the search stops on the first element
  having an attribute, regardless of the type of this attribute. If val = 0,
  the search stops on the first element having un attNum attribute, 
  regardless of its value. 
  If attNum2 != 0 the search stops on the first attNum or attNum2 found.
  The function returns a pointer to the found element or NULL.
  ----------------------------------------------------------------------*/
PtrElement FwdSearch2Attributes (PtrElement pEl, int val, const char *textVal,
                                 int attNum, int attNum2, PtrSSchema pSS,
                                 PtrSSchema pSS2)
{
  PtrElement          pRet, pCur, pAsc;
  ThotBool            stop;

  pRet = NULL;
  if (pEl)
    /* searches in the subtree of the element */
    {
      pRet = FwdSearch2AttrInSubtree (pEl, FALSE, val, textVal,
                                      attNum, attNum2, pSS, pSS2);
      if (pRet == NULL)
        /* if failure, searches in the subtrees of the next siblings of the element */
        {
          pCur = pEl->ElNext;
          while (pCur && pRet == NULL)
            {
              pRet = FwdSearch2AttrInSubtree (pCur, TRUE, val, textVal,
                                              attNum, attNum2, pSS, pSS2);
              pCur = pCur->ElNext;
            }
          /* if failure, searches the first ancestor with a next sibling */
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
                /* verifies if this element is the one we're looking for */
                {
                  pAsc = pAsc->ElNext;
                  if (pAsc != NULL)
                    {
                      if (AttrFound (pAsc, val, textVal, attNum, pSS))
                        pRet = pAsc;	/* found */
                      else
                        pRet = FwdSearch2Attributes (pAsc, val, textVal,
                                                     attNum, attNum2, pSS, pSS2);
                    }
                }
            }
        }
    }
  return pRet;
}


/*----------------------------------------------------------------------
  BackSearch2Attributes
  starting from the pEl element, backward searches in the tree an element
  having the attribute attNum defined in the structure scheme pSS and having
  a val value. If pSS is NULL, the search stops on the first element
  having an attribute, regardless of the type of this attribute. If val = 0,
  the search stops on the first element having un attNum attribute, 
  regardless of its value.
  If attNum2 != 0 the search stops on the first attNum or attNum2 found.
  The function returns a pointer to the found element or NULL.   
  ----------------------------------------------------------------------*/
PtrElement BackSearch2Attributes (PtrElement pEl, int val, const char *textVal,
                                  int attNum, int attNum2, PtrSSchema pSS,
                                  PtrSSchema pSS2)
{
  PtrElement          pRet, pCur;

  pRet = NULL;
  if (pEl)
    /* searches in the subtree of the siblings preceding the element */ 
    {
      pCur = pEl->ElPrevious;
      while (pCur && pRet == NULL)
        {
          pRet = BackSearch2AttrInSubtree (pCur, val, textVal,
                                           attNum, attNum2, pSS, pSS2);
          pCur = pCur->ElPrevious;
        }
      /* if failure, searches in the subtrees of the uncles of the element */
      if (pRet == NULL && pEl->ElParent != NULL)
        {
          if (AttrFound (pEl->ElParent, val, textVal, attNum, pSS) ||
              (attNum2 && AttrFound (pEl->ElParent, 0, "", attNum2, pSS2)))
            pRet = pEl->ElParent;
          else
            pRet = BackSearch2Attributes (pEl->ElParent, val, textVal,
                                          attNum, attNum2, pSS, pSS2);
        }
    }
  return pRet;
}

/*----------------------------------------------------------------------
  FwdSkipPageBreak 
  if pEl points to a page break, it returns inside pEl a pointer to
  the first element following the break and which is not a page
  break. It returns NULL if pEl points to a page break which is not
  followed by any element or not followed by any element different
  from a page break.
  Does not do anything if pEl does not point to a page break.
  ----------------------------------------------------------------------*/
void FwdSkipPageBreak (PtrElement * pEl)
{
  ThotBool            stop;

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
  FwdSkipPageBreakAndExtension 
  if pEl points to a page break or to an extension element, it returns
  inside pEl a pointer to the first element which is not a page break
  nor an extension element.
  It returns NULL if pEl points to a page break or an extension item
  which is not followed by any element or not followed by any element
  different from a page break and which is not an extension item.
  Does not do anything if pEl does not point to a page break nor an
  extension element.
  ----------------------------------------------------------------------*/
void                FwdSkipPageBreakAndExtension (PtrElement * pEl)
{
  ThotBool            stop;

  stop = FALSE;
  do
    {
      if (*pEl == NULL)
        stop = TRUE;
      else if (!strcmp ((*pEl)->ElStructSchema->SsName,
                        (*pEl)->ElParent->ElStructSchema->SsName))
        {
          if (!(*pEl)->ElTerminal)
            stop = TRUE;
          else if ((*pEl)->ElLeafType != LtPageColBreak)
            stop = TRUE;
        }

      if (!stop)
        *pEl = (*pEl)->ElNext;
    }
  while (!stop);
}


/*----------------------------------------------------------------------
  SkipPageBreakBegin
  If pEl points to a begin page break, it returns inside pEl a
  pointer to an element different from a page break and preceding
  that break.It does not do anything if pEl does not point
  to a begin page break.
  ----------------------------------------------------------------------*/
void                SkipPageBreakBegin (PtrElement * pEl)
{
  ThotBool            stop;
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
  BackSkipPageBreak     
  if pEl points to a page break, it returns inside pEl a pointer to
  the first element preceding the break and which is not a page
  break. It returns NULL if pEl points to a page break which is not
  preceded by any element or not preceded by any element different
  from a page break.
  Does not do anything if pEl does not point to a page break.
  ----------------------------------------------------------------------*/
void BackSkipPageBreak (PtrElement * pEl)
{
  ThotBool            stop;

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
  NextElement 
  returns a pointer to the element following the one pointed by
  pEl.
  ----------------------------------------------------------------------*/
PtrElement NextElement (PtrElement pEl)
{
  if (pEl == NULL)
    return NULL;
  while (pEl->ElNext == NULL && pEl->ElParent)
    pEl = pEl->ElParent;
  return pEl->ElNext;
}


/*----------------------------------------------------------------------
  PreviousElement 
  returns a pointer to the element preceding the one pointed by
  pEl.
  ----------------------------------------------------------------------*/
PtrElement PreviousElement (PtrElement pEl)
{
  if (pEl == NULL)
    return NULL;
  while (pEl->ElPrevious == NULL && pEl->ElParent)
    pEl = pEl->ElParent;
  return pEl->ElPrevious;
}

/*----------------------------------------------------------------------
  PreviousLeaf
  returns a pointer to the first leaf preceding the pEl element.
  ----------------------------------------------------------------------*/
PtrElement PreviousLeaf (PtrElement pEl)
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
  NextLeaf   
  returns a pointer to the first leaf following the element pEl.
  ----------------------------------------------------------------------*/
PtrElement          NextLeaf (PtrElement pEl)
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
  InsertElementBefore
  inserts the pNew element (and its next siblings), before the pOld
  element.
  ----------------------------------------------------------------------*/
void      InsertElementBefore (PtrElement pOld, PtrElement pNew)
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
      /* updates the volume of the ancestor elements */

      if (pNew->ElVolume > 0)
        {
          pAsc = pNew->ElParent;
          while (pAsc != NULL)
            {
              pAsc->ElVolume = pAsc->ElVolume + pNew->ElVolume;
              pAsc = pAsc->ElParent;
            }
        }
      pE = pNew;
      /* makes the text leaves of the element inherit the language defined
         by the first ancestor element */
      LeavesInheritLanguage (pE);
      while (pE->ElNext != NULL)
        {
          pE = pE->ElNext;
          pE->ElParent = pOld->ElParent;
          /* makes the texte leaves of the element inherit the language
             defined by the first ancestor element */
          LeavesInheritLanguage (pE);
          /* updates the volume of the ancestor elements */
          if (pE->ElVolume > 0)
            {
              pAsc = pE->ElParent;
              while (pAsc != NULL)
                {
                  pAsc->ElVolume = pAsc->ElVolume + pE->ElVolume;
                  pAsc = pAsc->ElParent;
                }
            }
        }
      pE->ElNext = pOld;
      pOld->ElPrevious = pE;
    }
}


/*----------------------------------------------------------------------
  InsertElementAfter
  inserts the pNew element (and its next siblings) after the pOld element.
  ----------------------------------------------------------------------*/
void                InsertElementAfter (PtrElement pOld, PtrElement pNew)
{
  PtrElement          pE;
  PtrElement          pAsc;

  if (pNew != NULL && pOld != NULL)
    {
      pNew->ElParent = pOld->ElParent;
      pNew->ElPrevious = pOld;
      /* updates the volume of the ancestor elements */
      pAsc = pNew->ElParent;
      while (pAsc != NULL)
        {
          pAsc->ElVolume = pAsc->ElVolume + pNew->ElVolume;
          pAsc = pAsc->ElParent;
        }
      pE = pNew;
      /* makes texte leaves of the element inherit the language defined by the
         first ancestor element */
      LeavesInheritLanguage (pE);
      while (pE->ElNext != NULL)
        {
          pE = pE->ElNext;
          pE->ElParent = pOld->ElParent;
          /* makes texte leaves of the element inherit the language defined by the 
             first ancestor element */
          LeavesInheritLanguage (pE);
          /* updates the volume of the ancestor elements */
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
  InsertFirstChild
  Inserts the pNew element (and its next siblings), as the first
  child of the pOld element.
  ----------------------------------------------------------------------*/
void                InsertFirstChild (PtrElement pOld, PtrElement pNew)
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
              /* makes texte leaves of the element inherit the language defined by the 
                 first ancestor element */
              LeavesInheritLanguage (pE);
              /* updates the volume of the ancestor elements */
              if (pE->ElVolume != 0)
                {
                  pAsc = pE->ElParent;
                  while (pAsc != NULL)
                    {
                      pAsc->ElVolume = pAsc->ElVolume + pE->ElVolume;
                      pAsc = pAsc->ElParent;
                    }
                }
              pE = pE->ElNext;
            }
        }
    }
}

/*----------------------------------------------------------------------
  InsertElemInChoice
  Replaces the element if type CHOICE pointed by pEl with  the new
  element pNew, except if pEl is a composite element or the root of
  the structure scheme.
  In this case, the new element is inserted as pEl's first child.
  ----------------------------------------------------------------------*/
void InsertElemInChoice (PtrElement pEl, PtrElement *pNew, PtrDocument pDoc,
                         ThotBool del)
{
  PtrAttribute        pAttr, pA, pPrevA;
  PtrSSchema          pSS;
  PtrElement          pE, pParent;
  PtrSRule            pSRule;
  PtrTextBuffer       buf, nextbuf;
  int                 typenum;
  ThotBool            replace;
  
  /* the new element will replace the CHOICE element */
  replace = TRUE;
  pParent = pEl->ElParent;
  if (!TypeHasException (ExcIsPlaceholder, pEl->ElTypeNumber, pEl->ElStructSchema) &&
      pParent != NULL)
    /* ...except if the choice is a composite element */
    if (pParent->ElStructSchema->SsRule->SrElem[pParent->ElTypeNumber - 1]->SrConstruct == CsAggregate ||
        pParent->ElStructSchema->SsRule->SrElem[pParent->ElTypeNumber - 1]->SrConstruct == CsUnorderedAggregate)
      replace = FALSE;
  /* except if the choice has exceptions */
  pSRule = pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber - 1];
  if (pSRule->SrNInclusions > 0 || pSRule->SrNExclusions > 0)
    replace = FALSE;
  /* except if it's the root of the structure scheme */
  if (pEl->ElTypeNumber == pEl->ElStructSchema->SsRootElem)
    replace = FALSE;
  if (!replace &&
      pEl->ElTypeNumber == (*pNew)->ElTypeNumber &&
      !strcmp (pEl->ElStructSchema->SsName, (*pNew)->ElStructSchema->SsName))
    /* the two elements are of the same type; one will replace the other */
    replace = TRUE;
  if (del)
    replace = TRUE;
  if (!replace)
    /* adds the new element as the child of the chosen element */
    InsertFirstChild (pEl, *pNew);
  else
    /* replaces element CsChoice with the new element */
    {
      // replace the refred element in the namespaces list
      ReplaceNamespaceDeclaration (pDoc, *pNew, pEl);

      /* exchanges the types of the two elements */
      pSS = pEl->ElStructSchema;
      typenum = pEl->ElTypeNumber;
      pEl->ElStructSchema = (*pNew)->ElStructSchema;
      pEl->ElTypeNumber = (*pNew)->ElTypeNumber;
      (*pNew)->ElStructSchema = pSS;
      (*pNew)->ElTypeNumber = typenum;
      
      if ((*pNew)->ElReferredDescr != NULL)
        /* suppreses all references to the CHOICE element and to the */
        /* descriptor of the reference element  of the CHOIX element */
        {
          DeleteAllReferences (pEl);
          DeleteReferredElDescr (pEl->ElReferredDescr);
          /* gets the description of the referenced element of the new
             element */
          pEl->ElReferredDescr = (*pNew)->ElReferredDescr;
          (*pNew)->ElReferredDescr = NULL;
          if (pEl->ElReferredDescr->ReReferredElem == *pNew)
            pEl->ElReferredDescr->ReReferredElem = pEl;
        }
      /* adds the attributes of pNew to those of pEl */
      if (pEl->ElFirstAttr == NULL)
        /* pEl does not have any attributes */
        pEl->ElFirstAttr = (*pNew)->ElFirstAttr;
      else
        /* searches for the last attribute of pEl */
        {
          pAttr = pEl->ElFirstAttr;
          while (pAttr->AeNext != NULL)
            pAttr = pAttr->AeNext;
          /* links the attributs of pNew after those of pEl */
          pAttr->AeNext = (*pNew)->ElFirstAttr;
          /* look for duplicate attributes */
          pAttr = pEl->ElFirstAttr;
          while (pAttr)
            {
              /* compare the current attribute (pAttr) to the following
                 attributes (pA) */
              pA = pAttr->AeNext;
              pPrevA = pAttr;
              while (pA)
                {
                  if (pA->AeAttrSSchema == pAttr->AeAttrSSchema &&
                      pA->AeAttrNum == pAttr->AeAttrNum)
                    /* duplicate attribute. Delete pA */
                    {
                      /* Remove it from the chain */
                      pPrevA->AeNext = pA->AeNext;
                      /* frees the memory allocated to the attribute */
                      if (pA->AeAttrType == AtReferenceAttr)
                        /* frees the reference */
                        if (pA->AeAttrReference != NULL)
                          {
                            DeleteReference (pA->AeAttrReference);
                            FreeReference (pA->AeAttrReference);
                            pA->AeAttrReference = NULL;
                          }
                      if (pA->AeAttrType == AtTextAttr)
                        /* frees the text buffers */
                        {
                          buf = pA->AeAttrText;
                          while (buf != NULL)
                            {
                              nextbuf = buf->BuNext;
                              FreeTextBuffer (buf);
                              buf = nextbuf;
                            }
                          pA->AeAttrText = NULL;
                        }
                      FreeAttribute (pA);
                      pA = pPrevA;
                    }
                  pPrevA = pA;
                  pA = pA->AeNext;
                }
              pAttr = pAttr->AeNext;
            }
        }
      /* pNew does not have any attributes */
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
              /* adds the volume of the element to that of its ancestors */
              if (pEl->ElVolume != 0)
                {
                  pE = pEl->ElParent;
                  while (pE)
                    {
                      pE->ElVolume = pE->ElVolume + pEl->ElVolume;
                      pE = pE->ElParent;
                    }
                }
              LeavesInheritLanguage (pEl);
              (*pNew)->ElText = NULL;
              (*pNew)->ElTextLength = 0;
              break;
            case LtPolyLine:
              pEl->ElPolyLineBuffer = (*pNew)->ElPolyLineBuffer;
              pEl->ElNPoints = (*pNew)->ElNPoints;
              pEl->ElVolume = pEl->ElNPoints;
              /* adds the volume of the element to that of its ancestors */
              if (pEl->ElVolume != 0)
                {
                  pE = pEl->ElParent;
                  while (pE != NULL)
                    {
                      pE->ElVolume = pE->ElVolume + pEl->ElVolume;
                      pE = pE->ElParent;
                    }
                }
              (*pNew)->ElPolyLineBuffer = NULL;
              (*pNew)->ElNPoints = 0;
              break;
            case LtPath:
              pEl->ElFirstPathSeg = (*pNew)->ElFirstPathSeg;
              pEl->ElVolume = (*pNew)->ElVolume;
              /* adds the volume of the element to that of its ancestors */
              if (pEl->ElVolume != 0)
                {
                  pE = pEl->ElParent;
                  while (pE != NULL)
                    {
                      pE->ElVolume = pE->ElVolume + pEl->ElVolume;
                      pE = pE->ElParent;
                    }
                }
              (*pNew)->ElFirstPathSeg = NULL;
              (*pNew)->ElVolume = 0;
              break;
            case LtSymbol:
            case LtGraphics:
            case LtCompound:
              pEl->ElGraph = (*pNew)->ElGraph;
              pEl->ElWideChar = (*pNew)->ElWideChar;
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
        DeleteElement (pNew, pDoc);
      *pNew = pEl;
    }
}

/*----------------------------------------------------------------------
  AttachRequiredAttributes
  Attaches to the pEl Element the required attributes specified by the
  pRe1 rule of the structure scheme pSS.
  ----------------------------------------------------------------------*/
void AttachRequiredAttributes (PtrElement pEl, SRule *pSRule, PtrSSchema pSS,
                               ThotBool withAttr, PtrDocument pDoc)
{
  PtrAttribute        pAttr;
  PtrReference        ref;
  int                 i;
  int                 att;
  int                 l;

  for (i = 0; i < pSRule->SrNDefAttrs; i++)
    {
      if (pSRule->SrDefAttrModif[i] && !withAttr)
        /* we are reading a pivot, so we don't create any attributes with a
           default value */
        continue;

      att = pSRule->SrDefAttr[i];
      /* gets an attribute block */
      GetAttribute (&pAttr);
      /* links this block to the head of the attributes list of the element */
      pAttr->AeNext = pEl->ElFirstAttr;
      pEl->ElFirstAttr = pAttr;
      /* fills the attribute block */
      pAttr->AeAttrSSchema = pSS;
      pAttr->AeAttrNum = att;
      if (!pSRule->SrDefAttrModif[i])
        /* fixed value attribute */
        pAttr->AeDefAttr = TRUE;
      pAttr->AeAttrType = pSS->SsAttribute->TtAttr[att - 1]->AttrType;
      switch (pAttr->AeAttrType)
        {
        case AtEnumAttr:
        case AtNumAttr:
          pAttr->AeAttrValue = pSRule->SrDefAttrValue[i];
          break;
        case AtTextAttr:
          GetTextBuffer (&pAttr->AeAttrText);
          CopyStringToBuffer ((unsigned char *)(pSS->SsConstBuffer + pSRule->SrDefAttrValue[i] - 1), pAttr->AeAttrText, &l);
          break;
        case AtReferenceAttr:
          /* gets a reference descriptor */
          GetReference (&ref);
          pAttr->AeAttrReference = ref;
          pAttr->AeAttrReference->RdElement = pEl;
          pAttr->AeAttrReference->RdAttribute = pAttr;
          break;
        }
    }
}

/*----------------------------------------------------------------------
  IsConstantConstructor returns TRUE is the element uses the constructor
  CsConstant.
  ----------------------------------------------------------------------*/
ThotBool          IsConstantConstructor (PtrElement pEl)
{
  PtrSRule        pSRule;

  if (pEl == NULL || pEl->ElStructSchema == NULL)
    return FALSE;
  else
    {
      pSRule = pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber - 1];
      if (pSRule->SrConstruct != CsConstant)
        return TRUE;
      else if (pSRule->SrConstruct != CsIdentity)
        {
          pSRule = pEl->ElStructSchema->SsRule->SrElem[pSRule->SrIdentRule - 1];
          if (pSRule->SrConstruct != CsConstant)
            return TRUE;
          else
            return FALSE;
        }
      else
        return FALSE;
    }
}


/*----------------------------------------------------------------------
  NewSubtree
  Creates a subtreee and returns a pointer to it.
  If the Desc ThotBool is TRUE, all of the subtree is created according
  to the structure scheme, otherwise, only the root of the subtree
  is created.
  If Root is TRUE, the function creates the root element of the subtree, otherwise
  it creates its children and the function returns a pointer to the first child.
  typeNum: number of the structure scheme rule which defines the type
  of subtree to create.
  pSS: pointer to the structure scheme of the subtree to create.
  pDoc: pointer to the document descriptor to which the subtree will belong.
  withAttr : if TRUE, the function creates the elements of the subtree with
  and gives their attributes the default value (creation case), otherwise,
  the function does not create them (case of a pivot read).
  withLabel tells if one must give the element a new label.
  ----------------------------------------------------------------------*/
PtrElement NewSubtree (int typeNum, PtrSSchema pSS, PtrDocument pDoc,
                       ThotBool Desc, ThotBool Root, ThotBool withAttr,
                       ThotBool withLabel)
{  
  PtrElement          pEl, t1, t2;
  int                 i, r;
  ThotBool            gener, create, error;
  PtrReference        ref;
  Name                PSchName;
  PtrSRule            pSRule, pSRule2, pExtRule;
  PtrTextBuffer       pBu1;
  PtrSSchema          pExtSSch;

  if (typeNum == 0 || typeNum > pSS->SsNRules)
    return NULL;
  pEl = NULL;			/* no element has been (yet) created */
  if (Root)
    /* rule specifying the type of element */
    {
      error = FALSE;
      pSRule = pSS->SsRule->SrElem[typeNum - 1];
      if (pSRule->SrConstruct == CsNatureSchema)
        /* it's a rule for a nature change, the function will create an element
           having the type of the root of the new nature */
        {
          PSchName[0] = EOS;
          LoadNatureSchema (pSS, PSchName, typeNum, NULL, pDoc);
          AddSchemaGuestViews (pDoc, pSRule->SrSSchemaNat);
          if (pSRule->SrSSchemaNat == NULL)
            /* could not load the scheme */
            error = TRUE;
          else
            /* nature scheme is loaded */
            /* there's an additional object of this nature */
            {
              /* change the structure scheme */
              pSS = pSRule->SrSSchemaNat;
              /* type of the root of the new structure scheme */
              typeNum = pSS->SsRootElem;
            }
        }
      pSRule = pSS->SsRule->SrElem[typeNum - 1];
      if (!error)
        /* get an element */
        GetElement (&pEl);
      if (pEl != NULL)
        {
          pEl->ElStructSchema = pSS;
          pEl->ElTypeNumber = typeNum;
          if (withLabel)
            /* compute the value of the label */
            ConvertIntToLabel (NewLabel (pDoc), pEl->ElLabel);
          /* gives the attributes the default values defined in the structure
             scheme which specifies the element */
          AttachRequiredAttributes (pEl, pSRule, pSS, withAttr, pDoc);
          /* updates the attribute values which are imposed by the extensions*/
          /* of the structure scheme specifying the element */
          pExtSSch = pSS->SsNextExtens;
          while (pExtSSch != NULL)
            {
              pExtRule = ExtensionRule (pSS, typeNum, pExtSSch);
              if (pExtRule != NULL)
                AttachRequiredAttributes (pEl, pExtRule, pExtSSch, withAttr,
                                          pDoc);
              pExtSSch = pExtSSch->SsNextExtens;
            }

          /* links the blocks specific to the constructor */
          switch (pSRule->SrConstruct)
            {
            case CsReference:
              pEl->ElTerminal = TRUE;
              pEl->ElLeafType = LtReference;
              /* gets a reference descriptor */
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
                case tt_Picture:
                  CreateTextBuffer (pEl);
                  pEl->ElLeafType = LtPicture;
                  pEl->ElVolume = 0;
                  pEl->ElPictInfo = NULL;
                  break;
                case GraphicElem:
                  pEl->ElLeafType = LtGraphics;
                  pEl->ElGraph = EOS;
                  pEl->ElWideChar = 0;
                  pEl->ElVolume = 0;
                  break;
                case Symbol:
                  pEl->ElLeafType = LtSymbol;
                  pEl->ElGraph = EOS;
                  pEl->ElWideChar = 0;
                  pEl->ElVolume = 0;
                  break;
                case PageBreak:
                  pEl->ElLeafType = LtPageColBreak;
                  pEl->ElPageType = PgComputed;
                  pEl->ElPageNumber = 1;
                  pEl->ElPageNumber = 0;
                  pEl->ElViewPSchema = 0;
                  pEl->ElPageModified = FALSE;
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
              /* copies the value of the constant */
              i = 0;
              pBu1 = pEl->ElText;
              do
                {
                  pBu1->BuContent[i] =
                    pSS->SsConstBuffer[i + pSRule->SrIndexConst - 1];
                  i++;
                }
              while (pBu1->BuContent[i - 1] != EOS && i < THOT_MAX_CHAR);
              pBu1->BuContent[i - 1] = EOS;
              pEl->ElTextLength = i - 1;
              pEl->ElVolume = pEl->ElTextLength;
              pEl->ElText->BuLength = i - 1;
              pEl->ElLanguage = TtaGetDefaultLanguage ();
              break;
            case CsAny:
              break;
            default:
              break;
            }
        }
    }
  /* evaluates if the children should be generated */
  pSRule = pSS->SsRule->SrElem[typeNum - 1];
  if (!Desc)
    gener = FALSE;
  else
    {
      gener = TRUE;
      if (pSRule->SrRecursive)
        {
          /* recursive rule */
          if (pSRule->SrRecursDone)
            /* rule's already been applied */
            gener = FALSE;	/* don't generate its children */
          else
            /* it's not been yet applied */
            /* apply the rule and remember this */
            pSRule->SrRecursDone = TRUE;
        }
    }
  if (gener)
    /* generate the  according to the constructor of the rule */
    switch (pSRule->SrConstruct)
      {
      case CsNatureSchema:
        /* this case does not happen, we changed */
        /* the element's type (cf. here above) */
        break;
      case CsIdentity:
        /* structure is the same as that defined by another rule of the */
        /* same scheme */
        create = FALSE;
        pSRule2 = pSS->SsRule->SrElem[pSRule->SrIdentRule - 1];
        if (pSRule2->SrConstruct == CsBasicElement ||
            pSRule2->SrNInclusions > 0 ||
            pSRule2->SrNExclusions > 0 ||
            pSRule2->SrConstruct == CsConstant ||
            pSRule2->SrConstruct == CsChoice ||
            pSRule2->SrConstruct == CsPairedElement ||
            pSRule2->SrConstruct == CsReference ||
            pSRule2->SrConstruct == CsNatureSchema)
          create = TRUE;
        t1 = NewSubtree (pSRule->SrIdentRule, pSS, pDoc, Desc, create,
                         withAttr, withLabel);
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
        t1 = NewSubtree (pSRule->SrListItem, pSS, pDoc, Desc, TRUE,
                         withAttr, withLabel);
        if (pEl == NULL)
          pEl = t1;
        else
          InsertFirstChild (pEl, t1);
        if (t1 != NULL)
          for (i = 2; i <= pSRule->SrMinItems; i++)
            {
              t2 = NewSubtree (pSRule->SrListItem, pSS, pDoc, Desc, TRUE,
                               withAttr, withLabel);
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
            /* don't create the optional components */
            {
              t2 = NewSubtree (pSRule->SrComponent[i - 1], pSS, pDoc, Desc,
                               TRUE, withAttr, withLabel);
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
      case CsAny:
        if (!TypeHasException (ExcIsPlaceholder, typeNum, pSS))
          /* it's an element Any, but not a placeholder. Try to create
             a child placeholder */
          {
            /* is there an element Any defined as a placeholder in the
               structure schema ? */
            r = 0;
            for (i = 1; i < pSS->SsNRules && r == 0; i++)
              if (pSS->SsRule->SrElem[i - 1]->SrConstruct == CsAny &&
                  TypeHasException (ExcIsPlaceholder, i, pSS))
                /* this is a placeholder */
                r = i;
            if (r > 0)
              /* create a child placeholder */
              {
                t1 = NewSubtree (r, pSS, pDoc, Desc, TRUE, withAttr,withLabel);
                if (pEl == NULL)
                  pEl = t1;
                else
                  InsertFirstChild (pEl, t1);
              }
          }
        break;
      default:
        break;
      }

  if (pSRule->SrRecursive && pSRule->SrRecursDone && gener)
    pSRule->SrRecursDone = FALSE;	/* for next time.... */
  return pEl;
}

/*----------------------------------------------------------------------
  RemoveExcludedElem
  removes the excluded elements of the subtree pointed to by pEl.
  ----------------------------------------------------------------------*/
void       RemoveExcludedElem (PtrElement * pEl, PtrDocument pDoc)
{
  PtrSRule            pRule, pRuleExcl;
  PtrElement          pAsc, pChild, pNextChild;
  PtrSSchema          pSS, pExtSSch;
  int                 i;
  ThotBool            excluded;

  if ((*pEl) != NULL)
    {
      /* tests all the ancestors of the element */ 
      pAsc = (*pEl)->ElParent;
      excluded = FALSE;
      while (pAsc != NULL && !excluded)
        {
          /* examines all the exclusions defined for this type of element */
          /* first access the structuring rule */
          pSS = pAsc->ElStructSchema;
          pRule = pSS->SsRule->SrElem[pAsc->ElTypeNumber - 1];
          pExtSSch = NULL;
          do
            {
              if (pRule != NULL)
                /* examines all the exclusions defined by the rule */
                for (i = 1; i <= pRule->SrNExclusions; i++)
                  {
                    if (pRule->SrExclusion[i - 1] == (*pEl)->ElTypeNumber)
                      {
                        /* the current element has the same number of type that that
                           of an element excluded by the ancestor */
                        if ((*pEl)->ElTypeNumber <= MAX_BASIC_TYPE)
                          /* it's a base type, it's excluded */
                          excluded = TRUE;
                        else if (!strcmp (pSS->SsName, (*pEl)->ElStructSchema->SsName))
                          /* compares the identifiers of the structure schemes */
                          /* same structure schemes, excluded type */
                          excluded = TRUE;
                      }
                    if (!excluded)
                      if ((*pEl)->ElTypeNumber == (*pEl)->ElStructSchema->SsRootElem)
                        /* the current element is the root element of its 
                           structure scheme: it's a nature */
                        {
                          /* structure rule of the excluded current element */
                          pRuleExcl = pSS->SsRule->SrElem[pRule->SrExclusion[i - 1] - 1];
                          if (pRuleExcl->SrConstruct == CsNatureSchema)
                            /* the exclusion is a nature, the exclusion can be applied */
                            /* if the nature names are the same */
                            excluded = (strcmp ((*pEl)->ElStructSchema->SsName,
                                                pRuleExcl->SrName) == 0);
                        }
                    /* the 2nd element of a pair is excluded if the first one is excluded */
                    if (!excluded)
                      /* we still haven't excluded our element */
                      if (pRule->SrExclusion[i - 1] + 1 == (*pEl)->ElTypeNumber)
                        /* the precedent type is excluded */
                        if (!strcmp (pSS->SsName, (*pEl)->ElStructSchema->SsName))
                          /* we are in the correct structure scheme */
                          if ((*pEl)->ElStructSchema->SsRule->SrElem[(*pEl)->ElTypeNumber - 1]->SrConstruct == CsPairedElement)
                            /* the element is member of a pair */
                            if (!(*pEl)->ElStructSchema->SsRule->SrElem[(*pEl)->ElTypeNumber - 1]->SrFirstOfPair)
                              /* it's the 2nd member of the pair */
                              excluded = TRUE;
                  }
              /* go to the next extension scheme */
              if (pExtSSch == NULL)
                pExtSSch = pSS->SsNextExtens;
              else
                pExtSSch = pExtSSch->SsNextExtens;
              if (pExtSSch != NULL)
                /* there's still an extension scheme */
                {
                  pSS = pExtSSch;
                  /* searches in this scheme the extension rule for the ancestor */
                  pRule = ExtensionRule (pAsc->ElStructSchema, pAsc->ElTypeNumber, pExtSSch);
                }
            }
          while (pExtSSch != NULL);
          /* climbs to the ancestor element */
          pAsc = pAsc->ElParent;
        }
      if (excluded)
        {
          /* removes the subtree of it's tree and frees it */
          DeleteElement (pEl, pDoc);
          *pEl = NULL;
        }
      /* deals with pEl's subtree if it hasn't yet been suppressed */
      if ((*pEl) != NULL)
        if (!(*pEl)->ElTerminal)
          {
            pChild = (*pEl)->ElFirstChild;
            while (pChild != NULL)
              {
                pNextChild = pChild->ElNext;
                RemoveExcludedElem (&pChild, pDoc);
                pChild = pNextChild;
              }
          }
    }
}

/*----------------------------------------------------------------------
  RemoveElement
  removes the subtree whose root is pointed by pEl
  ----------------------------------------------------------------------*/
void RemoveElement (PtrElement pEl)
{
  PtrElement          pAsc;

  if (pEl != NULL)
    {
      /* removes the volume of thelement from the volume of its ancestors */
      pAsc = pEl->ElParent;
      while (pAsc != NULL)
        {
          pAsc->ElVolume = pAsc->ElVolume - pEl->ElVolume;
          pAsc = pAsc->ElParent;
        }
      /* removes the element from it's sibling's list */
      if (pEl->ElPrevious != NULL)
        pEl->ElPrevious->ElNext = pEl->ElNext;
      if (pEl->ElNext != NULL)
        pEl->ElNext->ElPrevious = pEl->ElPrevious;
      /* suppresses the linking with the parent, if it's the first child */
      if (pEl->ElParent != NULL)
        if (pEl->ElParent->ElFirstChild == pEl)
          pEl->ElParent->ElFirstChild = pEl->ElNext;
      pEl->ElPrevious = NULL;
      pEl->ElNext = NULL;
      pEl->ElParent = NULL;
    }
}

/*----------------------------------------------------------------------
  RemoveAttribute
  removes from the pEl element the pAttr attribute, without freeing
  this attribute.
  ----------------------------------------------------------------------*/
void RemoveAttribute (PtrElement pEl, PtrAttribute pAttr)
{
  PtrAttribute        pPrevAttr;
  ThotBool            stop;

  if (pEl != NULL && pAttr != NULL)
    {
      /* removes the attribute from the element's chain of attributes */
      if (pEl->ElFirstAttr == pAttr)
        {
          /* it's the first attribute of the element */
          pEl->ElFirstAttr = pAttr->AeNext;
          pAttr->AeNext = NULL;
        }
      else
        /* searches the attribute to remove in the element's chain of attributes */
        {
          pPrevAttr = pEl->ElFirstAttr;
          stop = FALSE;
          do
            if (pPrevAttr == NULL)
              /* reached the end of the chain without finding the attribute 
                 the attribute must have already removed */
              stop = TRUE;
            else if (pPrevAttr->AeNext == pAttr)
              /* found it, so unlink it */
              {
                pPrevAttr->AeNext = pAttr->AeNext;
                pAttr->AeNext = NULL;
                stop = TRUE;
              }
            else
              /* did not found it, try the next attribute */
              pPrevAttr = pPrevAttr->AeNext;
          while (!stop);
        }
    }
}

/*----------------------------------------------------------------------
  DeleteAttribute
  deletes from the pEl element the pAttr attribute.
  The function also removes from the element all the
  specific presentation rules associated to the attribute.
  ----------------------------------------------------------------------*/
void DeleteAttribute (PtrElement pEl, PtrAttribute pAttr)
{
  PtrPRule            pPR, pPRprev, pPRnext;
  PtrTextBuffer       buf, nextbuf;

  if (!pAttr)
    return;
  if (pEl != NULL)
    /* deletes the specific presentation rules which are linked
       to the element and that are associated to  the attribute */
    {
      pPRprev = NULL;
      pPR = pEl->ElFirstPRule;
      while (pPR != NULL)
        {
          pPRnext = pPR->PrNextPRule;
          if (pPR->PrSpecifAttr == pAttr->AeAttrNum
              && pPR->PrSpecifAttrSSchema == pAttr->AeAttrSSchema)
            /* the pPR rule directly depends on the attribute which
               will be deleted, so we unlink it and free it */
            {
              if (pPRprev == NULL)
                pEl->ElFirstPRule = pPRnext;
              else
                pPRprev->PrNextPRule = pPRnext;
              FreePresentRule (pPR, pEl->ElStructSchema);
              pPR = pPRnext;
            }
          else
            /* move on to the next rule */
            {
              pPRprev = pPR;
              pPR = pPRnext;
            }
        }
    }
  /* frees the memory allocated to the attribute */
  if (pAttr->AeAttrType == AtReferenceAttr)
    /* frees the reference */
    if (pAttr->AeAttrReference != NULL)
      {
      	DeleteReference (pAttr->AeAttrReference);
       	FreeReference (pAttr->AeAttrReference);
      }
  if (pAttr->AeAttrType == AtTextAttr)
    /* frees the text buffers */
    {
      buf = pAttr->AeAttrText;
      while (buf != NULL)
        {
          nextbuf = buf->BuNext;
          FreeTextBuffer (buf);
          buf = nextbuf;
        }
      pAttr->AeAttrText = NULL;
    }
  /* frees the attribute block */
  if (pAttr != NULL)
    FreeAttribute (pAttr);
}

/*----------------------------------------------------------------------
  DeleteElement
  deletes the pEl and all its dependents from the tree. It also
  cancels all the references pointing to it. 
  When the functions returns pEl is set to NULL.
  For each freed element, all corresponding abstract boxes, in all views,
  are also freed.
  ----------------------------------------------------------------------*/
void DeleteElement (PtrElement *pEl, PtrDocument pDoc)
{
  PtrElement          pChild, pNextChild;
  PtrTextBuffer       pBuf, pNextBuf;
  PtrPathSeg          pPa, pNextPa;
  PtrAttribute        pAttr, pNextAttr;
  PtrPRule            pRule, pNextRule;
  PtrElement          pEl1;
  PtrCopyDescr        pCD, pNextCD;
  ThotPictInfo       *image;
  int                 c;

  if (*pEl && (*pEl)->ElStructSchema)
    {
      pEl1 = *pEl;
      /* if its a text leaf, delete the text */
      if (pEl1->ElTerminal)
        {
          if ((pEl1->ElLeafType == LtText || pEl1->ElLeafType == LtPicture)
              && pEl1->ElText != NULL)
            /* deletes all the text buffers associated to the element */
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
              while (/*c < pEl1->ElTextLength &&*/ pBuf != NULL);
              pEl1->ElText = NULL;
              pEl1->ElTextLength = 0;

              /* frees the image descriptor */
              if (pEl1->ElLeafType == LtPicture && pEl1->ElPictInfo)
                {
                  image = (ThotPictInfo *)pEl1->ElPictInfo;
                  CleanPictInfo (image);
                  TtaFreeMemory (image->PicFileName);
                  TtaFreeMemory (pEl1->ElPictInfo);
                  pEl1->ElPictInfo = NULL;
                }
            }
          if (pEl1->ElLeafType == LtPolyLine)
            /* frees all the coordinate buffers associated to the element */
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
          if (pEl1->ElLeafType == LtPolyLine)
            /* frees all the path elements associated to the element */
            {
              pPa = pEl1->ElFirstPathSeg;
              while (pPa)
                {
                  pNextPa = pPa->PaNext;
                  FreePathSeg (pPa);
                  pPa = pNextPa;
                }
              pEl1->ElFirstPathSeg = NULL;
            }
          if (pEl1->ElLeafType == LtReference)
            /* frees and unlinks the reference */
            {
              if (pDoc)
                CancelReference (*pEl, pDoc);
              if (pEl1->ElReference)
                {
                  FreeReference (pEl1->ElReference);
                  pEl1->ElReference = NULL;
                }
            }
          if (pEl1->ElLeafType == LtPairedElem)
            if (pEl1->ElOtherPairedEl != NULL)
              pEl1->ElOtherPairedEl->ElOtherPairedEl = NULL;
        }
      else
        /* it's not a leaf, so delete the element's children */
        {
          pChild = pEl1->ElFirstChild;
          while (pChild)
            {
              pNextChild = pChild->ElNext;
              DeleteElement (&pChild, pDoc);
              pChild = pNextChild;
            }
        }
      /* deletes all the descriptors of the copy elements */
      pCD = pEl1->ElCopyDescr;
      while (pCD != NULL)
        {
          if (pCD->CdCopiedAb != NULL)
            pCD->CdCopiedAb->AbCopyDescr = NULL;
          pNextCD = pCD->CdNext;
          FreeDescCopy (pCD);
          pCD = pNextCD;
        }
      
      /* deletes the inclusion reference if it exists */
      if (pEl1->ElSource != NULL)
        {
          CancelReference (*pEl, pDoc);
          FreeReference (pEl1->ElSource);
        }
      /* deletes all the attributes */
      pAttr = pEl1->ElFirstAttr;
      while (pAttr != NULL)
        {
          pNextAttr = pAttr->AeNext;
          RemoveAttribute (*pEl, pAttr);
          DeleteAttribute (*pEl, pAttr);
          pAttr = pNextAttr;
        }
      /* deletes all the presentation rules of the element */
      pRule = pEl1->ElFirstPRule;
      pEl1->ElFirstPRule = NULL;
      while (pRule != NULL)
        {
          pNextRule = pRule->PrNextPRule;
          FreePresentRule (pRule, pEl1->ElStructSchema);
          pRule = pNextRule;
        }
      /* frees all the references to the element */
      DeleteAllReferences (*pEl);
      /* frees the descriptor of the referenced element */
      DeleteReferredElDescr (pEl1->ElReferredDescr);
      pEl1->ElReferredDescr = NULL;

      /* remove extension schemas */
      UnlinkAllSchemasExtens (pEl1);

      if (pDoc && !pEl1->ElTerminal)
        // remove the namespace entry
        RemoveNamespaceDeclaration (pDoc, pEl1);
      /* removes the element from the tree */
      RemoveElement (*pEl);
      /* frees all the Abstract boxes */
      if (pDoc)
        FreeAbEl (*pEl, pDoc);
      /* frees the memory */
      if (FirstSelectedElement == *pEl)
        {
          SelectedDocument = NULL;
          FirstSelectedElement = NULL;
          LastSelectedElement = NULL;
        }
      else if (LastSelectedElement == *pEl)
        LastSelectedElement = FirstSelectedElement;
      FreeElement (*pEl);
      *pEl = NULL;
    }
}

/*----------------------------------------------------------------------
  CopyGradient
  Copy the gradient (and its Gradient Stop children) attached to element
  pSource to element pEl.
  ----------------------------------------------------------------------*/
void CopyGradient (PtrElement pSource, PtrElement pEl)
{
#ifdef _GL
  GradientStop        *gStop, *prev, *next, *gStopCopy;

  pEl->ElGradientDef = pSource->ElGradientDef;
  if (!pSource->ElGradient)
    return;
  if (pSource->ElGradientDef)
    /* it's the definition of a gradient */
    {
      if (pEl->ElGradient)
	/* free the existing gradient and its stops before making a fresh copy*/
	{
	  gStop = pEl->ElGradient->firstStop;
	  while (gStop)
	    {
	      next = gStop->next;
	      TtaFreeMemory (gStop);
	      gStop = next;
	    }
	  TtaFreeMemory (pEl->ElGradient);
	}
      /* create a copy of the gradient */
      pEl->ElGradient = (Gradient *)TtaGetMemory (sizeof (Gradient));
      pSource->ElGradientCopy = pEl;
      pEl->ElGradient->userSpace = pSource->ElGradient->userSpace;
      pEl->ElGradient->gradTransform = pSource->ElGradient->gradTransform;
      if (pSource->ElGradient->gradTransform)
	pEl->ElGradient->gradTransform = (Transform*)TtaCopyTransform (pSource->ElGradient->gradTransform);
      pEl->ElGradient->spreadMethod = pSource->ElGradient->spreadMethod;
      pEl->ElGradient->el = pEl;
      pEl->ElGradient->firstStop = NULL;
      prev = NULL;
      gStop = pSource->ElGradient->firstStop;
      while (gStop)
	{
	  gStopCopy = (GradientStop *)TtaGetMemory (sizeof (GradientStop));
	  if (prev)
	    prev->next = gStopCopy;
	  else
	    pEl->ElGradient->firstStop = gStopCopy;
	  gStopCopy->r = gStop->r;
	  gStopCopy->g = gStop->g;
	  gStopCopy->b = gStop->b;
	  gStopCopy->a = gStop->a;
	  gStopCopy->offset = gStop->offset;
	  gStopCopy->el = gStop->el->ElCopy;
	  gStopCopy->next = NULL;
	  prev = gStopCopy;
	  gStop = gStop->next;
	}
      pEl->ElGradient->gradType = pSource->ElGradient->gradType;
      if (pEl->ElGradient->gradType == Linear)
	{
	  pEl->ElGradient->gradX1 = pSource->ElGradient->gradX1;
	  pEl->ElGradient->gradX2 = pSource->ElGradient->gradX2;
	  pEl->ElGradient->gradY1 = pSource->ElGradient->gradY1;
	  pEl->ElGradient->gradY2 = pSource->ElGradient->gradY2;
	}
      else if (pEl->ElGradient->gradType == Radial)
	{
	  pEl->ElGradient->gradCx = pSource->ElGradient->gradCx;
	  pEl->ElGradient->gradCy = pSource->ElGradient->gradCy;
	  pEl->ElGradient->gradFx = pSource->ElGradient->gradFx;
	  pEl->ElGradient->gradFy = pSource->ElGradient->gradFy;
	  pEl->ElGradient->gradR = pSource->ElGradient->gradR;
	}
    }
  else
    /* it's a reference to a gradient */
    {
      if (pSource->ElGradient->el && pSource->ElGradient->el->ElGradientCopy)
	/* the referred gradient was copied, refer to its copy */
	pEl->ElGradient = pSource->ElGradient->el->ElGradientCopy->ElGradient;
      /*      else
	 avoid to create a reference to a gradient that is not copied 
	pEl->ElGradient = NULL; */
    }
#endif /* _GL */
}

/*----------------------------------------------------------------------
  CopyTree
  creates a tree which is a copy of the tree (or subtree) pointed to by
  pSource. Page breaks are not copied.
  It returns a pointer to the root of the new tree (from now on, called
  "the copy") or NULL in case of failure.
  - pDocSource: pointer to the document descriptor to which belongs
  the source tree.
  - pSSchema: pointer to the structure scheme that the  target's elements
  should depend on.
  - pDocCopy: pointer to the document's descriptor to which the copy
  must belong to.
  - pParent: pointer to the element which will be the parent of the new
  tree.
  If checkAttr is TRUE, the function only copies the attributes of
  the source which can be applied to the copy. 
  If shareRef is TRUE, the referenced elements of the copy share their
  referenced element descriptor with the source.
  If deepCopy the whole subtree is copied, otherwise only the pSource
  element is copied.
  If copyRef, all reference attributes are copied. If copyRef is FALSE,
  reference attributes are NOT copied.
  ----------------------------------------------------------------------*/
PtrElement CopyTree (PtrElement pSource, PtrDocument pDocSource,
                     PtrSSchema pSSchema, PtrDocument pDocCopy,
                     PtrElement pParent, ThotBool checkAttr, ThotBool shareRef,
                     ThotBool keepAccess, ThotBool deepCopy, ThotBool copyRef)
{
  PtrElement          pEl, pS2, pC1, pC2;
  PtrReference        rf;
  int                 copyType, nR;
  PtrSRule            pSRule;
  PtrElement          pAsc;
  PtrSSchema          pSS;
  ThotBool            sameSSchema;
  ThotBool            doCopy;
 
  pEl = NULL;
  nR = 0;
  /* pointer to the element that will be created */
  if (pSource && pSSchema)
    {
      doCopy = TRUE;
      /* we don't copy the page breaks */
      if (pSource->ElTerminal && pSource->ElLeafType == LtPageColBreak)
        doCopy = FALSE;
      else if (pSource->ElSource)
        {
          /* this element is an inclusion copy. We don't copy it if it was
             created before or after a page break (e.g., the table headers) */
          if (TypeHasException (ExcPageBreakRepBefore, pSource->ElTypeNumber,
                                pSource->ElStructSchema))
            doCopy = FALSE;
          else if (TypeHasException (ExcPageBreakRepetition,
                                     pSource->ElTypeNumber,
                                     pSource->ElStructSchema))
            doCopy = FALSE;
        }
      if (doCopy)
        {
          copyType = pSource->ElTypeNumber;
          if (strcmp (pSource->ElStructSchema->SsName, pSSchema->SsName) != 0)
            {
              /* change the generic structure */
              if (pSource->ElStructSchema->SsRule->SrElem[pSource->ElTypeNumber - 1]->SrUnitElem ||
                  pSource->ElStructSchema->SsExtension ||
                  pSource->ElTypeNumber <= MAX_BASIC_TYPE)
                {
                  /* the source element is a unit or an element defined
                     in a scheme extension */
                  if (!checkAttr)
                    /* we don't verify the units. The copy will inherit the
                       structure scheme of the source */
                    pSSchema = pSource->ElStructSchema;
                  else
                    {
                      /* verifies if the "future" ancestor of the copy has an
                         element belonging to the scheme where the source
                         element is defined */
                      if (pParent == NULL)
                        pAsc = pDocCopy->DocDocElement;
                      else
                        pAsc = pParent;
                      sameSSchema = pSource->ElTypeNumber <= MAX_BASIC_TYPE;
                      if (pAsc != NULL && !sameSSchema)
                        do
                          {
                            if (!strcmp (pSource->ElStructSchema->SsName,
                                         pAsc->ElStructSchema->SsName))
                              {
                                /* the copy will inherit the structure scheme
                                   of its document */
                                pSSchema = pAsc->ElStructSchema;
                                sameSSchema = TRUE;
                              }
                            else if (pSource->ElStructSchema->SsExtension)
                              /* verifies if the ancestor has this scheme
                                 extension */
                              {
                                pSSchema = pSource->ElStructSchema;
                                if (ValidExtension (pAsc, &pSSchema))
                                  sameSSchema = TRUE;
                              }
                            else if (pAsc->ElStructSchema->SsExtension)
                              /* the ancestor is an extension */
                              {
                                pSS = pAsc->ElStructSchema;
                                if (ValidExtension (pSource, &pSS))
                                  {
                                    pSSchema = pSource->ElStructSchema;
                                    sameSSchema = TRUE;
                                  }
                              }
                            if (!sameSSchema)
                              {
                                /* climbs one level to the next ancestor
                                   element */
                                if ((pAsc->ElParent == NULL) &&
                                    (pAsc->ElStructSchema->SsExtension))
                                  pAsc = pDocCopy->DocDocElement;
                                else
                                  pAsc = pAsc->ElParent;
                              }
                          }
                        while (pAsc != NULL && !sameSSchema);
                      if (!sameSSchema)
                        /* No ancestor has this structure scheme, so the unit
                           is invalid */
                        copyType = 0;
                    }
                }
              else
                /* the source is another document or another nature, we must
                   load the schemes for the copy */
                {
                  /* loads the structure and presentation schemes for the
                     copy */
                  /* no preference for the presentation scheme */
                  nR = CreateNature (NULL, pSource->ElStructSchema->SsName,
                                     NULL, pSSchema, pDocCopy);
                  if (nR == 0)
                    /* could not load the schema */
                    copyType = 0;
                  else
                    /* schema is loaded, changes the structure scheme of
                       the copy */
                    {
                      pSRule = pSSchema->SsRule->SrElem[nR - 1];
                      pSSchema = pSRule->SrSSchemaNat;
                      AddSchemaGuestViews (pDocCopy, pSSchema);
                    }
                }
            }

          if (copyType != 0)
            {
              /* get an element for the copy */
              GetElement (&pEl);
              /* fills the copy */
              pEl->ElStructSchema = pSSchema;
              pEl->ElTypeNumber = copyType;
              /* copies the attributes */
              CopyAttributes (pSource, pEl, pDocSource, pDocCopy, checkAttr,
                              copyRef);
              if (!pEl->ElTerminal)
                // copy namespaces if needed
                CopyNamespaceDeclarations (pDocSource, pSource, pDocCopy, pEl);
              /* copy the specific presentation rules */
              CopyPresRules (pSource, pEl);	
              if (shareRef)
                {
                  strncpy (pEl->ElLabel, pSource->ElLabel, MAX_LABEL_LEN);
                  pEl->ElReferredDescr = pSource->ElReferredDescr;	
                  /* (temporarily) shares the descriptor of the element
                     referenced between the source element and the copy element
                     so that CopyCommand or the Paste procedures can
                     link the copied references to the copied elements */
                }
              else
                {
                  /* the copy is not referenced */
                  pEl->ElReferredDescr = NULL;
                  /* computes the value of the label */
                  ConvertIntToLabel (NewLabel (pDocCopy), pEl->ElLabel);
                }

              pSource->ElCopy = pEl;
              pEl->ElCopy = pSource;
              pEl->ElIsCopy = pSource->ElIsCopy;
              if (keepAccess)
                pEl->ElAccess = pSource->ElAccess;
              else
                pEl->ElAccess = Inherited;
              pEl->ElHolophrast = pSource->ElHolophrast;
              pEl->ElSystemOrigin = pSource->ElSystemOrigin;
#ifdef _GL
              if (pSource->ElAnimation)
                pEl->ElAnimation = TtaCopyAnim (pSource->ElAnimation);
              if (pSource->ElTransform)
                pEl->ElTransform = (Transform*)TtaCopyTransform (pSource->ElTransform);
#endif /* _GL */
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
                      /* copies the content of a text or of an image */
                      pEl->ElText = CopyText (pSource->ElText, pEl);
                      pEl->ElTextLength = pSource->ElTextLength;
                      pEl->ElVolume = pEl->ElTextLength;
                      break;
                    case LtText:
                      pEl->ElLanguage = pSource->ElLanguage;
                      /* copies the content of a texte or of an image */
                      pEl->ElText = CopyText (pSource->ElText, pEl);
                      pEl->ElTextLength = pSource->ElTextLength;
                      pEl->ElVolume = pEl->ElTextLength;
                      break;
                    case LtPolyLine:
                      pEl->ElPolyLineBuffer = CopyText (pSource->ElPolyLineBuffer, pEl);
                      pEl->ElNPoints = pSource->ElNPoints;
                      pEl->ElPolyLineType = pSource->ElPolyLineType;
                      pEl->ElVolume = pEl->ElNPoints;
                      break;
                    case LtPath:
                      /* copies the path elements */
                      pEl->ElFirstPathSeg = CopyPath(pSource->ElFirstPathSeg);
                      pEl->ElVolume = pSource->ElVolume;
                      break;
                    case LtSymbol:
                    case LtGraphics:
                      pEl->ElGraph = pSource->ElGraph;
                      pEl->ElWideChar = pSource->ElWideChar;
                      break;
                    case LtPageColBreak:
                      break;
                    case LtReference:
                      if (pSource->ElReference != NULL)
                        {
                          /* gets a reference */
                          GetReference (&rf);	
                          pEl->ElReference = rf;
                          /* fills the new reference */
                          CopyReference (pEl->ElReference, pSource->ElReference,
                                         &pEl);
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
              if (pSource->ElSource)
                {
                  /* gets a reference */
                  GetReference (&rf);	
                  pEl->ElSource = rf;
                  /* fills the new reference */
                  CopyReference (pEl->ElSource, pSource->ElSource, &pEl);
                }
              /* creates the copies of the elements of the children (if they
                 exist) */
              if (deepCopy && !pSource->ElTerminal && pSource->ElFirstChild)
                {
                  pS2 = pSource->ElFirstChild;
                  pC1 = NULL;
                  do
                    {
                      pC2 = CopyTree (pS2, pDocSource, pSSchema, pDocCopy,
                                      pEl, checkAttr, shareRef, keepAccess,
                                      deepCopy, copyRef);
                      if (pC2)
                        {
                          if (pC1 == NULL)
                            InsertFirstChild (pEl, pC2);
                          else
                            InsertElementAfter (pC1, pC2);
                          pC1 = pC2;
                        }
                      pS2 = pS2->ElNext;
                    }
                  while (pS2);
                }
#ifdef _GL
	      /* if it's a gradient element, its stop children are now copied.
		 We can copy the gradient structure */
	      CopyGradient (pSource, pEl);
#endif /* _GL */
            }
        }
    }
  return pEl;
}

/*----------------------------------------------------------------------
  ChangeLabels
  gives new labels to all the elements of the pEl subtree, excluding
  pEl.
  ----------------------------------------------------------------------*/
static void ChangeLabels (PtrElement pEl, PtrDocument pDoc)
{
  PtrElement          pE;

  if (!pEl->ElTerminal)
    {
      pE = pEl->ElFirstChild;
      while (pE != NULL)
        {
          ConvertIntToLabel (NewLabel (pDoc), pE->ElLabel);
          ChangeLabels (pE, pDoc);
          pE = pE->ElNext;
        }
    }
}


/*----------------------------------------------------------------------
  CopyIncludedElem
  copies the abstract tree of the element to include as a subtree of the
  pEl element.
  pEl points to an element representing an inclusing.
  pDoc points to the document to which pEl belongs to.
  ----------------------------------------------------------------------*/
void CopyIncludedElem (PtrElement pEl, PtrDocument pDoc)
{
  PtrReference        pRef;
  PtrElement          pSource, pS2, pC1, pC2, pE;
  PtrPathSeg          pPa, pNextPa;
  ThotBool            done;

  /* copy's not done yet */
  done = FALSE;
  if (pEl->ElTerminal)
    switch (pEl->ElLeafType)
      {
      case LtPicture:
      case LtText:
        done = pEl->ElTextLength > 0;
        break;
      case LtPolyLine:
        done = pEl->ElNPoints > 0;
      case LtSymbol:
      case LtGraphics:
        done = pEl->ElGraph != EOS;
        break;
      case LtPath:
        done = pEl->ElFirstPathSeg != NULL;
      case LtPageColBreak:
      case LtReference:
        break;
      default:
        break;
      }
  else
    done = pEl->ElFirstChild != NULL;
  if (!done)
    /* if we still haven't done the copy, let's do it */
    {
      /* reference to the source element */
      pRef = pEl->ElSource;
      /* searches the element which must be copied: pSource */
      pSource = ReferredElement (pRef);
      /* pSource points to the element to be copied, so let's do it */
      if (pSource)
        {
          /* the document containing pSource is loaded, so let's
             copy it's content */
          /* we copy the attributes */
          CopyAttributes (pSource, pEl, pDoc, pDoc, TRUE, TRUE);
          /* we copy the specific presentation rules */
          CopyPresRules (pSource, pEl);
#ifdef _GL
          if (pSource->ElAnimation)
            pEl->ElAnimation = TtaCopyAnim (pSource->ElAnimation);
          if (pSource->ElTransform)
            pEl->ElTransform = (Transform*)TtaCopyTransform (pSource->ElTransform);
	  if (pSource->ElGradientDef)
	    CopyGradient (pSource, pEl);
#endif /* _GL */
          if (pEl->ElTerminal)
            switch (pSource->ElLeafType)
              {
              case LtText:
              case LtPicture:
              case LtPolyLine:
                if (pSource->ElLeafType == LtText)
                  pEl->ElLanguage = pSource->ElLanguage;
                /* we copy the content of a text or of an image */
                if (pSource->ElLeafType == LtPolyLine)
                  {
                    if (pEl->ElLeafType == LtPolyLine &&
                        pEl->ElPolyLineBuffer != NULL)
                      {
                        ClearText (pEl->ElPolyLineBuffer);
                        FreeTextBuffer (pEl->ElPolyLineBuffer);
                      }
                    pEl->ElLeafType = LtPolyLine;
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
                /* adds the element's volume to that of its ancestors */
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
              case LtPath:
                pPa = pEl->ElFirstPathSeg;
                while (pPa)
                  {
                    pNextPa = pPa->PaNext;
                    FreePathSeg (pPa);
                    pPa = pNextPa;
                  }
                pEl->ElLeafType = LtPath;
                pEl->ElFirstPathSeg = CopyPath(pSource->ElFirstPathSeg);
                pEl->ElVolume = pSource->ElVolume;
                break;
              case LtSymbol:
              case LtGraphics:
                pEl->ElGraph = pSource->ElGraph;
                pEl->ElWideChar = pSource->ElWideChar;
                break;
              case LtPageColBreak:
                break;
              case LtReference:
                if (pSource->ElReference != NULL)
                  {
                    if (pEl->ElReference == NULL)
                      GetReference (&pEl->ElReference);
                    CopyReference (pEl->ElReference, pSource->ElReference,
                                   &pEl);
                  }
                break;
              default:
                break;
              }
          else if (pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber - 1]->
                   SrConstruct == CsChoice &&
                   (pEl->ElTypeNumber != pSource->ElTypeNumber ||
                    strcmp (pEl->ElStructSchema->SsName,
                            pSource->ElStructSchema->SsName)))
            {
              pC1 = CopyTree (pSource, pDoc, pEl->ElStructSchema,
                              pDoc, pEl, TRUE, TRUE, FALSE, TRUE, TRUE);
              if (pC1 != NULL)
                {
                  pC1->ElReferredDescr = NULL;
                  InsertElemInChoice (pEl, &pC1, pDoc, FALSE);
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
                  pC2 = CopyTree (pS2, pDoc, pEl->ElStructSchema,
                                  pDoc, pEl, TRUE, TRUE, FALSE, TRUE,
                                  TRUE);
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
      /* updates the internal references to the copied part */
      TransferReferences (pEl, pDoc, pEl, pDoc);
	   
      /* assigns new labels to the elements of the copy */
      ChangeLabels (pEl, pDoc);
	   
      /* protects the included subtree against any user
         modification */
      ProtectElement (pEl);
    }
}

/*----------------------------------------------------------------------
  ReplicateElement
  copies a node without copying its children.
  ----------------------------------------------------------------------*/
PtrElement          ReplicateElement (PtrElement pEl, PtrDocument pDoc)
{
  PtrElement          pNew;
  int                 view;

  GetElement (&pNew);
  *pNew = *pEl;
  pNew->ElParent = NULL;
  pNew->ElPrevious = NULL;
  pNew->ElNext = NULL;
  pNew->ElReferredDescr = NULL;
  /* copies the attributes without verifying because we don't change 
     the structure  scheme */
  CopyAttributes (pEl, pNew, pDoc, pDoc, FALSE, TRUE);
  /* copies the specific presentation rules */
  CopyPresRules (pEl, pNew);
  for (view = 0; view < MAX_VIEW_DOC; view++)
    pNew->ElAbstractBox[view] = NULL;
  /* keep same values for ElStructSchema and TypeNumber (/
     pNew->ElVolume = 0;
     pNew->ElCopyDescr = NULL;*/
  /* computes the label's value */
  ConvertIntToLabel (NewLabel (pDoc), pNew->ElLabel);
  pNew->ElCopy = NULL;
  pNew->ElTransform = NULL;
  pNew->ElAnimation = NULL;
  pNew->ElGradient = NULL;
  pNew->ElGradientDef = FALSE;
  pNew->ElFirstChild = NULL;
  pNew->ElText = NULL;
  pNew->ElTextLength = 0;
  pNew->ElLanguage = pEl->ElLanguage;
  return pNew;
}


/*----------------------------------------------------------------------
  GetTypedAttrForElem
  searches an attribute of type attNum eamong the attributes attached
  to the pEl element. 
  Returns a pointer to the attribute or NULL.
  ----------------------------------------------------------------------*/
PtrAttribute GetTypedAttrForElem (PtrElement pEl, int attNum, PtrSSchema pSSattr)
{
  ThotBool            found;
  PtrAttribute        pAttr;

  found = FALSE;
  pAttr = pEl->ElFirstAttr;
  /* first attribute of the element */
  while (pAttr != NULL && !found)
    {
      if (pAttr->AeAttrNum == attNum)
        {
          /* same attribute numbers */
          if (attNum == 1)
            /* it's the language attribute, no use to compare the schemes */
            found = TRUE;
          else if (!strcmp (pAttr->AeAttrSSchema->SsName, pSSattr->SsName))
            /* same schemes : it's the attribute we are looking for */
            found = TRUE;
        }
      if (!found)
        /* go on to the next attribute of the same element */
        pAttr = pAttr->AeNext;
    }
  return pAttr;
}


/*----------------------------------------------------------------------
  GetTypedAttrAncestor
  returns a pointer to the attribute of the first
  element which encloses pEl and that has an attribute
  of type attNum. The function returns pElAttr if
  the search is succesful, NULL otherwise.
  ----------------------------------------------------------------------*/
PtrAttribute GetTypedAttrAncestor (PtrElement pEl, int attNum,
                                   PtrSSchema pSSattr, PtrElement * pElAttr)
{
  PtrElement          pElAtt;
  PtrAttribute        pAttr;

  pAttr = NULL;
  *pElAttr = NULL;
  /* start with the element */
  pElAtt = pEl;
  /* searches its ancestors */
  while (pElAtt != NULL && pAttr == NULL)
    /* searches in the element's attributes */
    if ((pAttr = GetTypedAttrForElem (pElAtt, attNum, pSSattr)) == NULL)
      /* the element does not have this attribute, climb up to the ancestor */
      pElAtt = pElAtt->ElParent;
    else
      *pElAttr = pElAtt;
  return pAttr;
}


/*----------------------------------------------------------------------
  CheckLanguageAttr
  verifies that the root pEl has a language attribute. If not,
  we add one.
  ----------------------------------------------------------------------*/
void CheckLanguageAttr (PtrDocument pDoc, PtrElement pEl)
{
  PtrAttribute        pAttr, pA;
  Language            lang;
  unsigned char       text[100];
  int                 len;
 
  if (pEl && GetTypedAttrForElem (pEl, 1, NULL) == NULL)
    /* this element has no language attribute */
    {
      /* a priori, we'll take the default language */
      lang = TtaGetDefaultLanguage ();
      if (pEl != pDoc->DocDocElement)
        /* it's not the root of the principal tree, so we verify
           if the principal tree has has a language attribue. If yes, 
           we use that language */
        {
          pAttr = GetTypedAttrForElem (pDoc->DocDocElement, 1, NULL);
          if (pAttr && pAttr->AeAttrText)
            {
              CopyBuffer2MBs (pAttr->AeAttrText, 0, text, 99);
              lang = TtaGetLanguageIdFromName ((char *)text);
            }
        }
      /* changes the language of the text leaves */
      ChangeLanguageLeaves (pEl, lang);
      /* applies the langyuage attribute to the element */
      GetAttribute (&pAttr);
      pAttr->AeAttrSSchema = pDoc->DocSSchema;
      pAttr->AeAttrNum = 1;
      pAttr->AeDefAttr = FALSE;
      pAttr->AeAttrType = AtTextAttr;
      GetTextBuffer (&pAttr->AeAttrText);
      CopyStringToBuffer ((unsigned char *)TtaGetLanguageName (lang), pAttr->AeAttrText, &len);
      if (pEl->ElFirstAttr == NULL)
        /* it's the first attribute of the element */
        pEl->ElFirstAttr = pAttr;
      else
        {
          pA = pEl->ElFirstAttr;	/* first attribute of the element */
          while (pA->AeNext != NULL)	/* searches for the last attribute */
            pA = pA->AeNext;
          pA->AeNext = pAttr;	/* links the new attribute */
        }
      pAttr->AeNext = NULL;	/* it's the last attribute */
    }
}

/* ----------------------------------------------------------------------
   TtaGetMainRoot

   Returns the document element of the abstract tree representing a document.
   Parameter:
   document: the document.
   Return value:
   the document element of the abstract tree.
   ---------------------------------------------------------------------- */
Element TtaGetMainRoot (Document document)
{
  PtrElement          element;

  UserErrorCode = 0;
  /* Checks the parameter document */
  element = NULL;
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    /* Parameter document is ok */
    element = LoadedDocument[document - 1]->DocDocElement;
  return ((Element) element);
}

/* ----------------------------------------------------------------------
   TtaGetRootElement

   Returns the root element of the abstract tree representing a document.
   Parameter:
   document: the document.
   Return value:
   the root element of the abstract tree.
   ---------------------------------------------------------------------- */
Element             TtaGetRootElement (Document document)
{
  PtrElement          root, el;

  UserErrorCode = 0;
  /* Checks the parameter document */
  root = NULL;
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    /* Parameter document is ok */
    {
      el = LoadedDocument[document - 1]->DocDocElement;
      if (el)
        {
          el = el->ElFirstChild;
          while (el && !root)
            {
              if (el->ElStructSchema->SsRootElem == el->ElTypeNumber)
                root = el;
              else
                el = el->ElNext;
            }
        }
    }
  return ((Element) root);
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
Document            TtaGetDocument (Element element)
{

  PtrDocument         pDoc;
  Document            ret;

  UserErrorCode = 0;
  ret = 0;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else
    {
      pDoc = DocumentOfElement ((PtrElement) element);
      if (pDoc != NULL)
        ret = IdentDocument (pDoc);
    }
  return ret;
}

/* ----------------------------------------------------------------------
   TtaGetParent

   Returns the parent element (i.e. first ancestor) of a given element.
   Parameter:
   element: the element whose the parent is asked.
   Return value:
   the parent element, or NULL if there is no parent (root).
   ---------------------------------------------------------------------- */
Element             TtaGetParent (Element element)
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
   TtaGetElementType

   Returns the type of a given element.
   Parameter:
   element: the element.
   Return value:
   type of the element.
   ---------------------------------------------------------------------- */
ElementType TtaGetElementType (Element element)
{
  ElementType         elementType;

  UserErrorCode = 0;
  elementType.ElSSchema = NULL;
  elementType.ElTypeNum = 0;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else if (((PtrElement) element)->ElStructSchema != NULL)
    {
      elementType.ElSSchema = (SSchema) ((PtrElement) element)->ElStructSchema;
      elementType.ElTypeNum = ((PtrElement) element)->ElTypeNumber;
    }
  return elementType;
}

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
   element: the element that is the root of the tree (if scope = SearchInTree)
   or the starting element (if scope = SearchForward or SearchBackward).
   Return value:
   the element found, or NULL if no element has been found.
   ---------------------------------------------------------------------- */
Element TtaSearchTypedElement (ElementType searchedType, SearchDomain scope,
                               Element element)
{
  PtrElement          pEl;
  PtrElement          elementFound;
  ThotBool            ok;

  UserErrorCode = 0;
  elementFound = NULL;
  ok = TRUE;
  if (element == NULL)
    {
      TtaError (ERR_invalid_parameter);
      ok = FALSE;
    }
  else if (((PtrElement) element)->ElStructSchema == NULL)
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
        pEl = BackSearchTypedElem ((PtrElement) element,
                                   searchedType.ElTypeNum, (PtrSSchema) (searchedType.ElSSchema),
                                   NULL);
      else if (scope == SearchInTree)
        pEl = FwdSearchTypedElem ((PtrElement) element, searchedType.ElTypeNum,
                                  (PtrSSchema) (searchedType.ElSSchema), (PtrElement) element);
      else
        pEl = FwdSearchTypedElem ((PtrElement) element, searchedType.ElTypeNum,
                                  (PtrSSchema) (searchedType.ElSSchema), NULL);
      if (pEl != NULL)
        elementFound = pEl;
    }
  return ((Element) elementFound);
}

/* ----------------------------------------------------------------------
   TtaSearchTypedElementInTree

   Returns the first element of a given type. Searching can be done in
   a tree or starting from a given element towards the beginning or the
   end of the abstract tree. In any case the returned element must be
   part of the parent tree.
   Parameters:
   searchedType: type of element to be searched. If searchedType.ElSSchema
   is NULL, searchedType must be a basic type ; then the next basic
   element of that type will be returned, whatever its structure
   schema.
   scope: SearchForward, SearchBackward or SearchInTree.
   parent: the limited tree where the searching can be done.
   element: the element that is the starting point of the search. It must be
   a descendant of parent.
   Return value:
   the element found, or NULL if no element has been found.
   ---------------------------------------------------------------------- */
Element TtaSearchTypedElementInTree (ElementType searchedType,
                                     SearchDomain scope, Element parent,
                                     Element element)
{
  PtrElement          pEl;
  PtrElement          elementFound;
  ThotBool            ok;

  UserErrorCode = 0;
  elementFound = NULL;
  ok = TRUE;
  if (element == NULL)
    {
      TtaError (ERR_invalid_parameter);
      ok = FALSE;
    }
  else if (((PtrElement) element)->ElStructSchema == NULL)
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
        pEl = BackSearchTypedElem ((PtrElement) element,
                                   searchedType.ElTypeNum, (PtrSSchema) (searchedType.ElSSchema),
                                   (PtrElement) parent);
      else
        pEl = FwdSearchTypedElem ((PtrElement) element, searchedType.ElTypeNum,
                                  (PtrSSchema) (searchedType.ElSSchema), (PtrElement) parent);

      if (pEl != NULL)
        elementFound = pEl;
    }
  return ((Element) elementFound);
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
int TtaGetElementVolume (Element element)
{
  int                 vol = 0;

  UserErrorCode = 0;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else
    vol = ((PtrElement) element)->ElVolume;
  return vol;
}



/* -- Added by P. Geneves, to provide search for SVG animation in Amaya. 
   This is useful to get differently typed elements in the order they
   appear in the document. 
   -- */

/*----------------------------------------------------------------------
  FwdSearch5TypesInSubtree                                        
  ----------------------------------------------------------------------*/
PtrElement FwdSearch5TypesInSubtree (PtrElement pEl, ThotBool test, 
                                     int typeNum5, int typeNum4, 
                                     int typeNum3, int typeNum2, 
                                     int typeNum1, PtrSSchema pSS5, 
                                     PtrSSchema pSS4, PtrSSchema pSS3, 
                                     PtrSSchema pSS2, PtrSSchema pSS1)
{
  PtrElement          pRet, pChild;

  pRet = NULL;
  if (test)
    {
      if (EquivalentType (pEl, typeNum1, pSS1) ||
          EquivalentType (pEl, typeNum2, pSS2) || 
          EquivalentType (pEl, typeNum3, pSS3) ||
          EquivalentType (pEl, typeNum4, pSS4) ||
          EquivalentType (pEl, typeNum5, pSS5) ||
          typeNum1 == 0)
        /* element found */
        pRet = pEl;
    }
  if (pRet == NULL && !pEl->ElTerminal)
    /* search the children */
    {
      pChild = pEl->ElFirstChild;
      while (pChild != NULL && pRet == NULL)
        {
          pRet = FwdSearch5TypesInSubtree (pChild, TRUE, typeNum5,
                                           typeNum4, typeNum3,
                                           typeNum2, typeNum1,
                                           pSS5, pSS4, pSS3, pSS2, pSS1);
          pChild = pChild->ElNext;
        }
    }
  return pRet;
}


/*----------------------------------------------------------------------
  FwdSearch5Types
  starting from the element pointed by pEl
  forward searches in the tree an element of type typeNum1 (defined in the
  structure scheme pointed by pSS1) or of typeNum2 (defined in the
  structure scheme pointed by pSS2)...and so on... or of typeNum5 (defined in the
  structure scheme pointed by pSS5)

  If there is a pSSX NULL, the search will stop on any element whose
  type number is typeNum1 or typeNum2 or... typeNum5, independently
  of its structure schema.

  The function returns a pointed to the found element or NULL.
  ----------------------------------------------------------------------*/
PtrElement FwdSearch5Types (PtrElement pEl, int typeNum5, 
                            int typeNum4, int typeNum3, 
                            int typeNum2, int typeNum1, 
                            PtrSSchema pSS5, PtrSSchema pSS4,
                            PtrSSchema pSS3, PtrSSchema pSS2, 
                            PtrSSchema pSS1)
{
  PtrElement          pRet, pCur, pAsc;
  ThotBool            stop;

  pRet = NULL;
  if (pEl)
    /* searches the subtree of the element */
    {
      pRet = FwdSearch5TypesInSubtree (pEl, FALSE, typeNum5, typeNum4,
                                       typeNum3, typeNum2, typeNum1,
                                       pSS5, pSS4, pSS3, pSS2, pSS1);
      if (pRet == NULL)
        /* if failure, searches the subtrees of the next siblings of the element */
        {
          pCur = pEl->ElNext;
          while (pCur && pCur->ElStructSchema && pRet == NULL)
            {
              pRet = FwdSearch5TypesInSubtree (pCur, TRUE, typeNum5,
                                               typeNum4, typeNum3,
                                               typeNum2, typeNum1,
                                               pSS5, pSS4, pSS3, pSS2, pSS1);
              pCur = pCur->ElNext;
            }
          /* if failure, searches the first ancestor with a next sibling */
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
                /* verifies if this element is the one we're looking for */
                {
                  pAsc = pAsc->ElNext;
                  if (pAsc != NULL)
                    {
                      if (EquivalentType (pAsc, typeNum1, pSS1) || 
                          EquivalentType (pAsc, typeNum2, pSS2) || 
                          EquivalentType (pAsc, typeNum3, pSS3) || 
                          EquivalentType (pAsc, typeNum4, pSS4) || 
                          EquivalentType (pAsc, typeNum5, pSS5) || 
                          typeNum1 == 0)
                        pRet = pAsc;	/* found */
                      else
                        pRet = FwdSearch5Types (pAsc, typeNum5, typeNum4,
                                                typeNum3, typeNum2, typeNum1,
                                                pSS5, pSS4, pSS3, pSS2, pSS1);
                    }
                }
            }
        }
    }
  return pRet;
}

/*----------------------------------------------------------------------
  BackSearch5TypesInSubtree                                        
  ----------------------------------------------------------------------*/
PtrElement BackSearch5TypesInSubtree (PtrElement pEl,  
                                      int typeNum5, int typeNum4, 
                                      int typeNum3, int typeNum2, 
                                      int typeNum1, PtrSSchema pSS5, 
                                      PtrSSchema pSS4, PtrSSchema pSS3, 
                                      PtrSSchema pSS2, PtrSSchema pSS1)
{
  PtrElement          pRet, pChild;

  pRet = NULL;
  if (!pEl->ElTerminal)
    /* search the children */
    {
      pChild = pEl->ElFirstChild;
      if (pChild)
        while (pChild->ElNext)
          pChild = pChild->ElNext;
      while (pChild && pRet == NULL)
        {
          pRet = BackSearch5TypesInSubtree (pChild, typeNum5,
                                            typeNum4, typeNum3,
                                            typeNum2, typeNum1,
                                            pSS5, pSS4, pSS3, pSS2, pSS1);
          pChild = pChild->ElNext;
        }
    }
  if (pRet == NULL)
    {
      if (EquivalentType (pEl, typeNum1, pSS1) ||
          EquivalentType (pEl, typeNum2, pSS2) ||
          EquivalentType (pEl, typeNum3, pSS3) ||
          EquivalentType (pEl, typeNum4, pSS4) ||
          EquivalentType (pEl, typeNum3, pSS3))
        pRet = pEl;		/* found */
    }
  return pRet;
}


/*----------------------------------------------------------------------
  BackSearch5Types
  starting from the element pointed by pEl
  backward searches in the tree an element of type typeNum1 (defined in the
  structure scheme pointed by pSS1) or of typeNum2 (defined in the
  structure scheme pointed by pSS2)...and so on... or of typeNum5 (defined in the
  structure scheme pointed by pSS5)

  If there is a pSSX NULL, the search will stop on any element whose
  type number is typeNum1 or typeNum2 or... typeNum5, independently
  of its structure schema.

  The function returns a pointed to the found element or NULL.
  ----------------------------------------------------------------------*/
PtrElement BackSearch5Types (PtrElement pEl, int typeNum5, 
                             int typeNum4, int typeNum3, 
                             int typeNum2, int typeNum1, 
                             PtrSSchema pSS5, PtrSSchema pSS4,
                             PtrSSchema pSS3, PtrSSchema pSS2, 
                             PtrSSchema pSS1)
{
  PtrElement          pRet, pCur;

  pRet = NULL;			/* pRet: result of the search */
  if (pEl)
    /* searches in the subtrees of the siblings preceding the element */
    {
      pCur = pEl->ElPrevious;
      while (pCur != NULL && pRet == NULL)
        {
          pRet = BackSearch5TypesInSubtree (pCur, typeNum5, typeNum4,
                                            typeNum3, typeNum2, typeNum1,
                                            pSS5, pSS4, pSS3, pSS2, pSS1);
          pCur = pCur->ElPrevious;
        }
      /* if failure, verify if it's the parent, then search in the subtrees of the uncles
         of the element */
      if (pRet == NULL)
        if (pEl->ElParent)
          {
            pEl = pEl->ElParent;
            if (EquivalentType (pEl, typeNum1, pSS1) ||
                EquivalentType (pEl, typeNum2, pSS2) ||
                EquivalentType (pEl, typeNum3, pSS3) ||
                EquivalentType (pEl, typeNum4, pSS4) ||
                EquivalentType (pEl, typeNum5, pSS5))
              pRet = pEl;	/* found, it's the parent */
            else
              pRet = BackSearch5Types (pEl, typeNum5, typeNum4,
                                       typeNum3, typeNum2, typeNum1,
                                       pSS5, pSS4, pSS3, pSS2, pSS1);
          }
    }
  return pRet;
}


/* ----------------------------------------------------------------------
   TtaSearchElementAmong5Types

   Returns the first element typed among the 5 types provided
   Searching can be done in a tree or starting from a given element
   towards the beginning of the abstract tree.
   Parameters:
   typeX: types of element to be searched. If typeX.ElSSchema
   is NULL, typeX must be a basic type ; then the next basic
   element of that type will be returned, whatever its structure
   schema.
   scope: SearchForward or SearchInTree.
   element: the element that is the root of the tree
   (if scope = SearchInTree) or the starting element
   (if scope = SearchForward).
   Return value:
   the element found, or NULL if no element has been found.
   ---------------------------------------------------------------------- */
Element TtaSearchElementAmong5Types (ElementType type1, ElementType type2,
                                     ElementType type3, ElementType type4,
                                     ElementType type5, SearchDomain scope, 
                                     Element element)
{
  PtrElement          pEl;
  PtrElement          elementFound;
  ThotBool            ok;

  UserErrorCode = 0;
  elementFound = NULL;
  ok = TRUE;
  if (element == NULL)
    {
      TtaError (ERR_invalid_parameter);
      ok = FALSE;
    }
  else if (((PtrElement) element)->ElStructSchema == NULL)
    {
      TtaError (ERR_invalid_parameter);
      ok = FALSE;
    }
  else if ((type1.ElSSchema == NULL) ||
           (type2.ElSSchema == NULL) ||
           (type3.ElSSchema == NULL) ||
           (type4.ElSSchema == NULL) ||
           (type5.ElSSchema == NULL)) 
    {
      if ((type1.ElTypeNum > MAX_BASIC_TYPE) ||
          (type2.ElTypeNum > MAX_BASIC_TYPE) ||
          (type3.ElTypeNum > MAX_BASIC_TYPE) ||
          (type4.ElTypeNum > MAX_BASIC_TYPE) ||
          (type5.ElTypeNum > MAX_BASIC_TYPE))
        {
          TtaError (ERR_invalid_element_type);
          ok = FALSE;
        }
    }
  else if ((type1.ElTypeNum < 1 ||
            type1.ElTypeNum > ((PtrSSchema) (type1.ElSSchema))->SsNRules) ||
	    
           (type2.ElTypeNum < 1 ||
            type2.ElTypeNum > ((PtrSSchema) (type2.ElSSchema))->SsNRules) ||
	    
           (type3.ElTypeNum < 1 ||
            type3.ElTypeNum > ((PtrSSchema) (type3.ElSSchema))->SsNRules) ||
	    
           (type4.ElTypeNum < 1 ||
            type4.ElTypeNum > ((PtrSSchema) (type4.ElSSchema))->SsNRules) ||
	    
           (type5.ElTypeNum < 1 ||
            type5.ElTypeNum > ((PtrSSchema) (type5.ElSSchema))->SsNRules))
    {
      TtaError (ERR_invalid_element_type);
      ok = FALSE;
    }
   
  if (ok)
    {
      if (scope == SearchBackward)
        pEl = BackSearch5Types ((PtrElement) element,
                                type1.ElTypeNum,
                                type2.ElTypeNum,
                                type3.ElTypeNum,
                                type4.ElTypeNum,
                                type5.ElTypeNum,
                                (PtrSSchema) (type1.ElSSchema),
                                (PtrSSchema) (type2.ElSSchema),
                                (PtrSSchema) (type3.ElSSchema),
                                (PtrSSchema) (type4.ElSSchema),
                                (PtrSSchema) (type5.ElSSchema));
      else
        {
          pEl = FwdSearch5Types ((PtrElement) element,
                                 type1.ElTypeNum,
                                 type2.ElTypeNum,
                                 type3.ElTypeNum,
                                 type4.ElTypeNum,
                                 type5.ElTypeNum,
                                 (PtrSSchema) (type1.ElSSchema),
                                 (PtrSSchema) (type2.ElSSchema),
                                 (PtrSSchema) (type3.ElSSchema),
                                 (PtrSSchema) (type4.ElSSchema),
                                 (PtrSSchema) (type5.ElSSchema));
	   
          if (pEl && scope == SearchInTree &&
              !ElemIsWithinSubtree (pEl, (PtrElement) element))
            pEl = NULL;
        }
      if (pEl)
        elementFound = pEl;
    }
  return ((Element) elementFound);
}

/* ----------------------------------------------------------------------
   TtaIsElementWithSpacePreserve

   Returns TRUE if element white spaces should be preserved.
   ---------------------------------------------------------------------- */
ThotBool TtaIsElementWithSpacePreserve (Element el)
{
  PtrAttribute        pAttr;

  if (el)
    {
      /* test the exception xml:space="preserve" */
      pAttr = GetAttrElementWithException (ExcSpacePreserve, (PtrElement) el);
      if (pAttr && pAttr->AeAttrValue > 1)
        return TRUE;
    }
  return FALSE;
}

