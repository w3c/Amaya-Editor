/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Traitement des exceptions definies dans les schemas  
 * de structure. 
 *
 * Author: V. Quint (INRIA)
 *
 */
#include "thot_gui.h"
#include "thot_sys.h"
#include "message.h"
#include "constmedia.h"
#include "typemedia.h"


#define THOT_EXPORT extern
#include "edit_tv.h"

#include "exceptions_f.h"
#include "structschema_f.h"

/*----------------------------------------------------------------------
  TypeHasException returns TRUE if the exception exceptNum is associated
  to the element typeNum in the structure schema pSS.
  ----------------------------------------------------------------------*/
ThotBool TypeHasException (int exceptNum, int typeNum, PtrSSchema pSS)
{
  ThotBool            ret;
  int                 e;
  PtrSRule            pSRule;
  PtrSSchema          pExtSS;

  ret = FALSE;
  if (pSS != NULL && typeNum > 0)
    {
      pSRule = pSS->SsRule->SrElem[typeNum - 1];
      if (pSRule->SrFirstExcept != 0)
        /* il y a des exceptions pour ce type d'element */
        {
          e = pSRule->SrFirstExcept;
          /* compare les exceptions de ce type d'element avec exceptNum */
          do
            if (pSS->SsException[e - 1] == exceptNum)
              ret = TRUE;
            else
              e++;
          while (!ret && e <= pSRule->SrLastExcept);
        }
      if (!ret)
        /* on n'a pas trouve', on cherche dans les extensions du schema */
        {
          /* passe a la premiere extension du schema */
          pExtSS = pSS->SsNextExtens;
          /* on examine les extensions successives tant qu'on n'a pas trouve' */
          while (pExtSS != NULL && !ret)
            {
              /* cherche la regle d'extension du type d'element traite' */
              pSRule = ExtensionRule (pSS, typeNum, pExtSS);
              if (pSRule != NULL)
                /* examine les exceptions de cette regle d'extension */
                if (pSRule->SrFirstExcept != 0)
                  {
                    e = pSRule->SrFirstExcept;
                    do
                      if (pExtSS->SsException[e - 1] == exceptNum)
                        ret = TRUE;
                      else
                        e++;
                    while (!ret && e <= pSRule->SrLastExcept);
                  }
              /* passe a l'extension de schema suivante */
              pExtSS = pExtSS->SsNextExtens;
            }
        }
    }
  return ret;
}


/*----------------------------------------------------------------------
  SearchTypeExcept looks in the element root for an element with:
  - exception TypeExcept
  - same schema
  - not a copy
  Skip elements with:
  - exception PageBreak
  - natures
  - copies
  Stop the research on element stopEl if Restrict = TRUE
  Returns the pointer to the found element trouve or NULL.
  ----------------------------------------------------------------------*/
PtrElement SearchTypeExcept (PtrElement root, PtrElement stopEl,
                             int TypeExcept, ThotBool Restrict)
{
  PtrElement          pE, Repetition;

  Repetition = NULL;
  if (root != NULL && !root->ElTerminal)
    {
      for (pE = root->ElFirstChild; pE != NULL; pE = pE->ElNext)
        {
          if (Restrict && pE == stopEl)
            /* on arrete la recherche */
            break;
          else
            {
              if (pE->ElStructSchema == root->ElStructSchema &&
                  /* same schema and not a copy */
                  pE->ElSource == NULL)
                {
                  if (TypeHasException (TypeExcept, pE->ElTypeNumber
                                        , pE->ElStructSchema))
                    /* exception ok */
                    {
                      Repetition = pE;
                      /* c'est donc le bon */
                      break;
                    }
                  else
                    /* recursion */ 
                    if (!TypeHasException (ExcPageBreak, pE->ElTypeNumber,
                                           pE->ElStructSchema))
                      SearchTypeExcept (pE, stopEl, TypeExcept, Restrict);
                }
            }
        }
    }
  return Repetition;
}


/*----------------------------------------------------------------------
  AttrHasException returns TRUE if the exception exceptNum is associated
  to the attribute attr in the structure schema pSS.
  ----------------------------------------------------------------------*/
ThotBool AttrHasException (int exceptNum, int attr, PtrSSchema pSS)
{
  ThotBool            ret;
  int                 e;
  PtrTtAttribute         pAtt;

  ret = FALSE;
  if (pSS != NULL)
    {
      pAtt = pSS->SsAttribute->TtAttr[attr - 1];
      if (pAtt->AttrFirstExcept != 0)
        /* il y a des exceptions pour cet attribut */
        {
          e = pAtt->AttrFirstExcept;
          /* compare les exceptions de cet attribut avec exceptNum */
          do
            if (pSS->SsException[e - 1] == exceptNum)
              ret = TRUE;	/* trouve' */
            else
              e++;
          /* exception suivante pour ce type d'element */
          while (!ret && e <= pAtt->AttrLastExcept);
        }
    }
  return ret;
}

/*----------------------------------------------------------------------
  GetAttrElementWithException returns the attribute attached to the
  element pEl with the exception exceptNum.
  Return NULL if not found.
  ----------------------------------------------------------------------*/
PtrAttribute GetAttrElementWithException (int exceptNum, PtrElement pEl)
{
  PtrAttribute        pAttr;

  if (pEl)
    {
      pAttr = pEl->ElFirstAttr;
      while (pAttr)
        if (AttrHasException (exceptNum, pAttr->AeAttrNum, pAttr->AeAttrSSchema))
          return pAttr;
        else
          pAttr = pAttr->AeNext;
    }
  return NULL;
}

/*----------------------------------------------------------------------
  GetAttrWithException looks for an attribute with the exception
  exceptNum within the structure schema pSS.
  Return 0 if the attribute number or 0 if not found.		
  ----------------------------------------------------------------------*/
int GetAttrWithException (int exceptNum, PtrSSchema pSS)
{
  int                 attr, ret;

  ret = 0;
  attr = 0;
  /* examine successivement tous les attributs definis dans le schema */
  if (pSS != NULL && exceptNum > 0)
    while (ret == 0 && attr < pSS->SsNAttributes)
      {
        attr++;
        if (AttrHasException (exceptNum, attr, pSS))
          /* on a trouve' l'attribut cherche' */
          ret = attr;
      }
  return ret;
}


/*----------------------------------------------------------------------
  GetElemWithException looks for an element with the exception
  exceptNum within the structure schema pSS.
  Return 0 if the element number or 0 if not found.
  ----------------------------------------------------------------------*/
int GetElemWithException (int exceptNum, PtrSSchema pSS)
{
  int                 typ, ret;

  ret = 0;
  typ = 0;
  /* examine successivement tous les types d'element definis dans le schema */
  while (ret == 0 && typ < pSS->SsNRules)
    {
      typ++;
      if (TypeHasException (exceptNum, typ, pSS))
        /* on a trouve' le type d'element cherche' */
        ret = typ;
    }
  return ret;
}
