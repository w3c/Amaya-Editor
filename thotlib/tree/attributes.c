/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2007
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Module traitant les attributs
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
#include "language.h"
#include "appstruct.h"
#include "appdialogue.h"
#include "typecorr.h"
#include "application.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "page_tv.h"
#include "select_tv.h"
#include "edit_tv.h"
#include "appdialogue_tv.h"
#include "frame_tv.h"

#include "applicationapi_f.h"
#include "attributes_f.h"
#include "attrpresent_f.h"
#include "attrmenu.h"
#include "attrmenu_f.h"
#include "boxselection_f.h"
#include "buildboxes_f.h"
#include "callback_f.h"
#include "changeabsbox_f.h"
#include "content_f.h"
#include "createabsbox_f.h"
#include "docs_f.h"
#include "displayview_f.h"
#include "documentapi_f.h"
#include "exceptions_f.h"
#include "frame_f.h"
#include "language_f.h"
#include "memory_f.h"
#include "presvariables_f.h"
#include "references_f.h"
#include "schemas_f.h"
#include "structcreation_f.h"
#include "structmodif_f.h"
#include "structselect_f.h"
#include "structschema_f.h"
#include "thotmsg_f.h"
#include "tree_f.h"
#include "undo_f.h"

/*----------------------------------------------------------------------
  Retourne Vrai si les deux elements pointes par pEl1 et pEl2     
  possedent les memes attributs avec les memes valeurs            
  ----------------------------------------------------------------------*/
ThotBool SameAttributes (PtrElement pEl1, PtrElement pEl2)
{
  PtrAttribute        pAttr1, pAttr2;
  int                 nAttr1, nAttr2;
  ThotBool            same = TRUE;

  /* nombre d'attributs du 1er element */
  pAttr1 = pEl1->ElFirstAttr;
  nAttr1 = 0;
  /* compte les attributs du 1er element */
  while (pAttr1 != NULL)
    {
      nAttr1++;
      pAttr1 = pAttr1->AeNext;
    }

  /* nombre d'attributs du 2eme element */
  pAttr2 = pEl2->ElFirstAttr;
  nAttr2 = 0;
  /* compte les attributs du 2eme element */
  while (pAttr2 != NULL)
    {
      nAttr2++;
      pAttr2 = pAttr2->AeNext;
    }

  /* compare le nombre d'attributs des deux elements */
  if (nAttr1 != nAttr2)
    same = FALSE;		/* nombres d'attributs differents, fin */
  else
    /* meme nombre d'attributs, compare les attributs et leurs valeurs */
    {
      pAttr1 = pEl1->ElFirstAttr;
      /* 1er attribut du 1er element */
      /* examine tous les attributs du 1er element */
      while (pAttr1 != NULL && same)
        /* cherche si le 2eme element possede cet attribut du 1er elem */
        {
          pAttr2 = GetAttributeOfElement (pEl2, pAttr1);
          if (pAttr2 == NULL)
            /* le 2eme element n'a pas cet attribut, fin */
            same = FALSE;
          else
            {
              if (pAttr1->AeDefAttr != pAttr2->AeDefAttr)
                /* valeurs differentes de cet attribut */
                same = FALSE;
              else
                switch (pAttr1->AeAttrType)
                  {
                  case AtNumAttr:
                  case AtEnumAttr:
                    if (pAttr1->AeAttrValue != pAttr2->AeAttrValue)
                      same = FALSE;
                    break;
                  case AtReferenceAttr:
                    same = FALSE;
                    break;
                  case AtTextAttr:
                    same = TextsEqual (pAttr2->AeAttrText, pAttr1->AeAttrText);
                    break;
                  default:
                    break;
                  }
            }
          if (same)
            /* meme valeur,passe a l'attribut suivant du 1er element */
            pAttr1 = pAttr1->AeNext;
        }
    }
  return same;
}

/*----------------------------------------------------------------------
  SetAttrReference fait pointer l'attribut reference pAttr sur    
  l'element pEl.							
  UNIQUEMENT pour une reference *INTERNE*                         
  ----------------------------------------------------------------------*/
static void SetAttrReference (PtrAttribute pAttr, PtrElement pEl)
{
  PtrReference        pPR1;

  if (pEl != NULL)
    {
      pPR1 = pAttr->AeAttrReference;
      if (pPR1 != NULL)
        {
          pPR1->RdNext = pEl->ElReferredDescr->ReFirstReference;
          if (pPR1->RdNext != NULL)
            pPR1->RdNext->RdPrevious = pAttr->AeAttrReference;
          pPR1->RdReferred = pEl->ElReferredDescr;
          pPR1->RdReferred->ReFirstReference = pAttr->AeAttrReference;
        }
    }
}

/*----------------------------------------------------------------------
  Met a l'element pEl l'attribut auquel est associe'              
  l'exception de numero ExceptNum et retourne un pointeur sur cet 
  attribut. On ne met l'attribut que si l'element ne le possede   
  pas deja. Dans tous les cas, la fonction retourne un pointeur   
  sur l'attribut.                                                 
  S'il s'agit d'un attribut reference, pReferredEl designe	
  l'element sur lequel la reference doit pointer.			
  UNIQUEMENT pour une reference *INTERNE*                         
  Si pReferredEl est NULL, le lien de reference n'est pas etabli. 
  Ce lien peut ensuite etre etabli par la procedure SetReference, 
  en particulier pour un lien externe.                            
  ----------------------------------------------------------------------*/
PtrAttribute AttachAttrByExceptNum (int ExceptNum, PtrElement pEl,
                                    PtrElement pReferredEl, PtrDocument pDoc)
{
  PtrAttribute        pAttr;
  PtrReference        pRef;
  PtrElement          pOldReferredEl;
  PtrSSchema          pSS;
  int                 attrNum;
  ThotBool            found;

  /* cherche d'abord le numero de l'attribut */
  pSS = pEl->ElStructSchema;
  attrNum = GetAttrWithException (ExceptNum, pSS);
  if (attrNum == 0)
    /* pas trouve' dans le schema de l'element, on cherche dans les */
    /* les extensions de schema du document. */
    {
      pSS = pDoc->DocSSchema->SsNextExtens;
      found = FALSE;
      /* parcourt tous les schemas d'extension du document */
      while (pSS != NULL && !found)
        {
          /* cherche le numero de l'attribut dans cette extension */
          attrNum = GetAttrWithException (ExceptNum, pSS);
          if (attrNum != 0)
            found = TRUE;
          else
            /* passe au schema d'extension suivant */
            pSS = pSS->SsNextExtens;
        }
    }

  /* cherche si l'attribut est deja present pour l'element */
  pAttr = pEl->ElFirstAttr;
  found = FALSE;
  while (!found && pAttr != NULL)
    if (pAttr->AeAttrNum == attrNum &&
        /* attrNum = 1: Langue, quel que soit le schema de structure */
        (attrNum == 1 ||
         !strcmp (pAttr->AeAttrSSchema->SsName, pSS->SsName)))
      found = TRUE;
    else
      pAttr = pAttr->AeNext;
  if (found)
    {
      /* l'element porte deja cet attribut */
      if (pAttr->AeAttrType == AtReferenceAttr && pReferredEl != NULL)
        {
          /* c'est un attribut reference, on verifie qu'il pointe bien */
          /* sur le bon element */
          /* on cherche d'abord l'element reference' par l'attribut */
          pOldReferredEl = NULL;
          /* a priori, pas d'element reference' */
          if (pAttr->AeAttrReference != NULL)
            if (pAttr->AeAttrReference->RdReferred != NULL)
              pOldReferredEl = pAttr->AeAttrReference->RdReferred->ReReferredElem;
          if (pOldReferredEl != pReferredEl)
            {
              /* l'attribut ne designe pas le bon element */
              /* coupe le lien avec l'ancien element reference' */
              DeleteReference (pAttr->AeAttrReference);
              if (pReferredEl->ElReferredDescr == NULL)
                /* le nouvel element a referencer ne l'est pas encore */
                /* attache a l'element un descripteur d'element reference' */
                {
                  pReferredEl->ElReferredDescr = NewReferredElDescr (pDoc);
                  pReferredEl->ElReferredDescr->ReReferredElem = pReferredEl;
                }
              /* fait pointer la reference sur l'element */
              SetAttrReference (pAttr, pReferredEl);
            }
        }
    }
  else
    {
      /* l'element ne porte pas cet attribut */
      /* cree et intialise l'attribut */
      GetAttribute (&pAttr);
      /* chaine le nouvel attribut en tete des attributs de l'element */
      pAttr->AeNext = pEl->ElFirstAttr;
      pEl->ElFirstAttr = pAttr;
      pAttr->AeAttrSSchema = pSS;
      pAttr->AeAttrNum = attrNum;
      pAttr->AeDefAttr = FALSE;
      pAttr->AeAttrType = pAttr->AeAttrSSchema->SsAttribute->TtAttr[pAttr->AeAttrNum - 1]->AttrType;
      if (pAttr->AeAttrType == AtReferenceAttr)
        {
          if (pReferredEl != NULL)
            /* l'element pointe' par pReferredEl va etre reference' */
            if (pReferredEl->ElReferredDescr == NULL)
              /* cet element n'a pas encore de descripteur d'element */
              /* reference', on lui en associe un */
              {
                pReferredEl->ElReferredDescr = NewReferredElDescr (pDoc);
                pReferredEl->ElReferredDescr->ReReferredElem = pReferredEl;
              }
          GetReference (&pRef);
          pAttr->AeAttrReference = pRef;
          pRef->RdElement = pEl;
          pRef->RdAttribute = pAttr;
          pRef->RdTypeRef = RefFollow;
          /* fait pointer la reference sur l'element */
          SetAttrReference (pAttr, pReferredEl);
        }
    }
  return pAttr;
}

/*----------------------------------------------------------------------
  On reaffiche un pave modifie                                    
  ----------------------------------------------------------------------*/
static void RedisplayAbsBox (PtrAbstractBox pAbsBox, int varNum,
                             PtrPSchema pPSchema, int frame, PtrElement pEl,
                             PtrDocument pDoc, PtrAttribute pAttr)
{
  int                 height;
  PtrAbstractBox      pAbChild;

  if (pAbsBox->AbPresentationBox)
    {
      /* c'est un pave de presentation */
      if (pAbsBox->AbPSchema == pPSchema && pAbsBox->AbVarNum == varNum)
        /* c'est bien un pave du type cherche' */
        /* recalcule la valeur de la variable de presentation */
        if (NewVariable (varNum, pAttr->AeAttrSSchema, pPSchema, pAbsBox,
                         pAttr, pDoc))
          {
            /* la variable de presentation a change' de valeur */
            pAbsBox->AbChange = TRUE;
            height = PageHeight;
            ChangeConcreteImage (frame, &height, pAbsBox);
          }
    }
  else
    /* ce n'est pas un pave' de pre'sentation */
    /* cherche parmi les fils les paves de presentation */
    /* de l'element et de ses descendants */
    {
      pAbChild = pAbsBox->AbFirstEnclosed;
      while (pAbChild != NULL)
        {
          RedisplayAbsBox (pAbChild, varNum, pPSchema, frame, pEl,pDoc, pAttr);
          /* next child abstract box */
          pAbChild = pAbChild->AbNext;
        }
    }
}

/*----------------------------------------------------------------------
  L'attribut pointe' par pAttr portant sur l'element              
  pointe' par pEl, dans le document pDoc, a change' de valeur.    
  Reafficher toutes les boites de presentation de cet element
  qui utilisent la valeur de cet attribut.
  ----------------------------------------------------------------------*/
void RedisplayAttribute (PtrAttribute pAttr, PtrElement pEl, PtrDocument pDoc)
{
  PtrPSchema          pPSchema;
  ThotBool            found;
  PtrAbstractBox      pAbsBox;
  PresVariable       *pPresVar;
  PresVarItem        *pItem;
  PtrHandlePSchema    pHd;
  int                 varNum, item;
  int                 frame, view, doc = 0;

  /* l'attribut dont la valeur a ete modifie' apparait-il dans une */
  /* variable de presentation ? */
  /* schema de presentation de l'attribut */
  pPSchema = PresentationSchema (pAttr->AeAttrSSchema, pDoc);
  pHd = NULL;

  while (pPSchema != NULL)
    {
      /* parcourt les variables de presentation du schema */
      for (varNum = 0; varNum < pPSchema->PsNVariables; varNum++)
        {
          pPresVar = pPSchema->PsVariable->PresVar[varNum];
          found = FALSE;
          /* examine les items de la variable */
          for (item = 0; item < pPresVar->PvNItems && !found; item++)
            {
              pItem = &pPresVar->PvItem[item];
              if (pItem->ViType == VarAttrValue)
                {
                  if (pItem->ViAttr == pAttr->AeAttrNum)
                    found = TRUE;
                }
              else if (pItem->ViType == VarNamedAttrValue)
                {
                  if (pPSchema->PsConstant[pItem->ViConstant - 1].PdString &&
                      !strcmp (pAttr->AeAttrSSchema->SsAttribute->TtAttr[pAttr->AeAttrNum - 1]->AttrName,
                               pPSchema->PsConstant[pItem->ViConstant - 1].PdString))
                    found = TRUE;
                }
            }
          if (found)
            /* l'attribut est utilise' dans la variable */
            /* cherche dans toutes les vues du document les paves de
               presentation de l'element qui utilisent cette variable comme
               contenu */
            {
              for (view = 0; view < MAX_VIEW_DOC; view++)
                {
                  pAbsBox = pEl->ElAbstractBox[view];
                  frame = pDoc->DocViewFrame[view];
                  if (frame > 0)
                    {
                      if (doc == 0)
                        doc = FrameTable[frame].FrDoc;
                      /* parcourt les paves de l'element */
                      while (pAbsBox != NULL)
                        if (pAbsBox->AbElement != pEl)
                          /* on a traite' tous les paves de l'element */
                          pAbsBox = NULL;
                        else
                          /* c'est un pave' de l'element */
                          {
                            RedisplayAbsBox (pAbsBox, varNum+1, pPSchema,
                                             frame, pEl, pDoc, pAttr);
                            pAbsBox = pAbsBox->AbNext;
                          }
                      /* on ne reaffiche pas si on est en train de calculer
                         les pages */
                      if (PageHeight == 0 &&
                          documentDisplayMode[doc - 1] == DisplayImmediately)
                        DisplayFrame (frame);
                    }
                }
            }
        }
      /* next style sheet (P schema extension, aka CSS style sheet) */
      if (pHd)
        pHd = pHd->HdNextPSchema;
      else if (CanApplyCSSToElement (pEl))
        /* it was the main P schema, get the first schema extension */
        pHd = FirstPSchemaExtension (pAttr->AeAttrSSchema, pDoc, pEl);

      if (pHd)
        pPSchema = pHd->HdPSchema;
      else
        /* no schema any more. stop */
        pPSchema = NULL;
    }
}

/*----------------------------------------------------------------------
  applique a l'element pointe' par pEl du document pDoc les       
  regles de presentation correspondant a l'attribut decrit dans   
  le bloc pointe' par pAttr.                                      
  ----------------------------------------------------------------------*/
void ApplyAttrPRulesToElem (PtrElement pEl, PtrDocument pDoc,
                            PtrAttribute pAttr, PtrElement pElAttr,
                            ThotBool inherit)
{
  PtrAttribute        pOldAttr;
  ThotBool            doIt;

  /* on ne traite pas les marques de page */
  if (!pEl->ElTerminal || pEl->ElLeafType != LtPageColBreak)
    {
      doIt = FALSE;

      /* cherche la valeur de cet attribut pour l'element */
      pOldAttr = GetAttributeOfElement (pEl, pAttr);
      if (inherit)
        /* traitement de la presentation par heritage */
        {
          if (pOldAttr == NULL || pOldAttr == pAttr)
            doIt = TRUE;
        }
      else
        /* presentation directe */
        {
          if (pOldAttr == NULL)
            doIt = TRUE;
          /* si pour cet element, cet attribut a une valeur imposee par */
          /* le schema de structure, on ne fait rien d'autre */
          else if (pOldAttr->AeDefAttr == 0)
            doIt = TRUE;
          /* s'il s'agit d'une suppression de l'attribut, on ne fait rien */
          if (doIt)
            switch (pAttr->AeAttrType)
              {
              case AtEnumAttr:
                doIt = pAttr->AeAttrValue > 0;
                break;
              case AtNumAttr:
                doIt = pAttr->AeAttrValue >= -MAX_INT_ATTR_VAL
                  && pAttr->AeAttrValue <= MAX_INT_ATTR_VAL;
                break;
              case AtTextAttr:
                if (pAttr->AeAttrText == NULL)
                  doIt = FALSE;
                break;
              case AtReferenceAttr:
                doIt = TRUE;
                break;
              default:
                break;
              }
        }

      if (doIt)
        {
          /* applique les regles de presentation de l'attribut */
          UpdatePresAttr (pEl, pAttr, pElAttr, pDoc, FALSE, inherit, NULL);
          /* reaffiche les variables de presentation qui utilisent */
          /* l'attribut */
          RedisplayAttribute (pAttr, pEl, pDoc);
        }
    }
}


/*----------------------------------------------------------------------
  Applique au sous-arbre pEl du document pDoc les     
  regles de presentation heritees de l'attribut pAttr		
  On arrete la recursion quand on rencontre un fils portant       
  lui-meme un attribut de meme type que pAttr			
  ----------------------------------------------------------------------*/
void ApplyAttrPRulesToSubtree (PtrElement pEl, PtrDocument pDoc,
                               PtrAttribute pAttr, PtrElement pElAttr)
{
  PtrElement          pChild;
  PtrAttribute        pOldAttr;
  PtrPSchema          pPS;
  InheritAttrTable   *inheritedAttr;
  PtrHandlePSchema    pHd;

  pOldAttr = GetAttributeOfElement (pEl, pAttr);
  if (pOldAttr == NULL || pOldAttr == pAttr)
    {
      /* on traite d'abord tout le sous-arbre */
      if (!pEl->ElTerminal)
        for (pChild = pEl->ElFirstChild; pChild != NULL;
             pChild = pChild->ElNext)
          if (pChild->ElStructSchema == pEl->ElStructSchema)
            /* same structure schema */
            ApplyAttrPRulesToSubtree (pChild, pDoc, pAttr, pElAttr);

      /* on traite l'element lui-meme */
      /* on cherche d'abord si pEl herite de pAttr */
      pPS = PresentationSchema (pEl->ElStructSchema, pDoc);
      pHd = NULL;
      while (pPS)
        {
          if  (pPS->PsNInheritedAttrs->Num[pEl->ElTypeNumber - 1] > 0)
            {
              /* pEl peut heriter d'au moins un attribut */
              if ((inheritedAttr = pPS->PsInheritedAttr->ElInherit[pEl->ElTypeNumber - 1]) == NULL)
                {
                  /* la table d'heritage n'existe pas. On la cree */
                  CreateInheritedAttrTable (pEl->ElTypeNumber,
                                            pEl->ElStructSchema, pPS, pDoc);
                  inheritedAttr = pPS->PsInheritedAttr->ElInherit[pEl->ElTypeNumber - 1];
                }
              if ((*inheritedAttr)[pAttr->AeAttrNum - 1])
                {
                  /* pEl herite de l'attribut pAttr */
                  /* on lui applique la presentation correspondante */
                  ApplyAttrPRulesToElem (pEl, pDoc, pAttr, pElAttr, TRUE);
                }
            }
          if  (pEl->ElTypeNumber > MAX_BASIC_TYPE &&
               pPS->PsNInheritedAttrs->Num[AnyType])
            {
              /* at least one attribute is inherited by any element type
                 (basic types are excluded). */
              if ((inheritedAttr = pPS->PsInheritedAttr->ElInherit[AnyType]) == NULL)
                {
                  /* la table d'heritage n'existe pas. On la cree */
                  CreateInheritedAttrTable (AnyType+1, pEl->ElStructSchema,
                                            pPS, pDoc);
                  inheritedAttr = pPS->PsInheritedAttr->ElInherit[AnyType];
                }
              if ((*inheritedAttr)[pAttr->AeAttrNum - 1])
                {
                  /* pEl herite de l'attribut pAttr */
                  /* on lui applique la presentation correspondante */
                  ApplyAttrPRulesToElem (pEl, pDoc, pAttr, pElAttr, TRUE);
                }
            }
          /* next P schema */
          if (pHd)
            /* get the next extension schema */
            pHd = pHd->HdNextPSchema;
          else if (CanApplyCSSToElement (pEl))
            /* extension schemas have not been checked yet */
            /* get the first extension schema */
            pHd = FirstPSchemaExtension (pEl->ElStructSchema, pDoc, pEl);

          if (pHd == NULL)
            /* no more extension schemas. Stop */
            pPS = NULL;
          else
            pPS = pHd->HdPSchema;	
        }
    }
}

/*----------------------------------------------------------------------
  Applique au sous-arbre pointe' par pEl du document pDoc les     
  regles de presentation des attributs dont les valeurs se	
  comparent a pAttr.						
  ----------------------------------------------------------------------*/
void ApplyAttrPRules (PtrElement pEl, PtrDocument pDoc, PtrAttribute pAttr)
{
  PtrElement          pChild;
  ComparAttrTable    *attrValComp;
  int                 i;
  PtrAttribute        pCompAttr;
  PtrPSchema          pPS;

  if (GetAttributeOfElement (pEl, pAttr) == NULL)
    {
      /* on traite d'abord les descendants */
      if (!pEl->ElTerminal)
        for (pChild = pEl->ElFirstChild; pChild != NULL; pChild = pChild->ElNext)
          if (pChild->ElStructSchema == pEl->ElStructSchema)
            /* meme schema de structure */
            ApplyAttrPRules (pChild, pDoc, pAttr);
    }

  /* on traite l'element lui-meme */
  /* cherche si pEl possede un attribut se comparant a pAttr */
  pPS = PresentationSchema (pEl->ElStructSchema, pDoc);
  if (pPS != NULL)
    {
      if ((attrValComp = pPS->PsComparAttr->CATable[pAttr->AeAttrNum - 1]) == NULL)
        {
          /* la table de comparaison n'existe pas. On la creee */
          CreateComparAttrTable (pAttr, pDoc, pEl);
          attrValComp = pPS->PsComparAttr->CATable[pAttr->AeAttrNum - 1];
        }
      if (attrValComp != NULL)
        for (i = pEl->ElStructSchema->SsNAttributes; i > 0; i--)
          {
            if ((*attrValComp)[i - 1])
              /* l'attribut de rang i se compare a pAttr */
              if ((pCompAttr = GetTypedAttrForElem (pEl, i, pAttr->AeAttrSSchema)) != NULL)
                /* pEl possede un attribut comme celui de rang i */
                ApplyAttrPRulesToElem (pEl, pDoc, pCompAttr, pEl, FALSE);
          }
    }
}


/*----------------------------------------------------------------------
  Chaine les elements liberes par les fusions de texte.           
  ----------------------------------------------------------------------*/
void KeepFreeElements (PtrElement pEl, PtrElement *pFirstFree)
{
  pEl->ElNext = *pFirstFree;
  *pFirstFree = pEl;
}


/*----------------------------------------------------------------------
  Change la langue de toutes les feuilles de texte dans le        
  sous-arbre de l'element pointe' par pEl, appartenant au         
  document dont le contexte est pointe' par pDoc                  
  Le parametre force est vrai quand on veut forcer le changement  
  de langue.                                                      
  ----------------------------------------------------------------------*/
void ChangeLanguage (PtrDocument pDoc, PtrElement pEl, Language lang,
                     ThotBool force)
{
  int                 view;
  PtrElement          pChild;
  PtrAbstractBox      pAbsBox;

  if (pEl == NULL)
    return;

  if (!pEl->ElTerminal)
    /* this element is not a leaf */
    {
      pChild = pEl->ElFirstChild;
      while (pChild != NULL)
        {
          /* si un descendant a un attribut Langue, on ne le change pas */
          if (GetTypedAttrForElem (pChild, 1, NULL) == NULL)
            ChangeLanguage (pDoc, pChild, lang, force);
          pChild = pChild->ElNext;
        }
    }
  else if (pEl->ElLeafType == LtText &&
           pEl->ElLanguage != lang &&
           (pEl->ElLanguage >= TtaGetFirstUserLanguage () || force))
    /* That's a text leaf in a different language */
    /* Changes the language of the element:
       When lang < TtaGetFirstUserLanguage (), lang actually represents a
       script, not a real language. */
    if (pEl->ElTextLength == 0 ||
        TtaGetScript (pEl->ElLanguage) == TtaGetScript (lang))
      {
        pEl->ElLanguage = lang;
        /* parcourt toutes les vues du document pour changer les */
        /* paves de l'element */
        for (view = 0; view < MAX_VIEW_DOC; view++)
          if (pEl->ElAbstractBox[view] != NULL)
            /* l'element a au moins un pave dans la vue */
            {
              pAbsBox = pEl->ElAbstractBox[view];
              /* saute les paves de presentation de l'element */
              while (pAbsBox->AbPresentationBox &&
                     pAbsBox->AbElement == pEl)
                pAbsBox = pAbsBox->AbNext;
              if (!pAbsBox->AbDead)
                /* traite le pave' principal de l'element */
                /* change la langue du pave */
                {
                  pAbsBox->AbLang = lang;
                  pAbsBox->AbChange = TRUE;
                  /* conserve le pointeur sur le pave a reafficher */
                  pDoc->DocViewModifiedAb[view] =
                    Enclosing (pAbsBox, pDoc->DocViewModifiedAb[view]);
                }
            }
      }
}


/*----------------------------------------------------------------------
  Verifie si l'attribut pointe' par pAttr et appartenant a pEl	
  contient un attribut qui initialise un (des) compteur(s).	
  Si oui, met a jour les valeurs de ce(s) compteur(s).		
  ----------------------------------------------------------------------*/
void UpdateCountersByAttr (PtrElement pEl, PtrAttribute pAttr, PtrDocument pDoc)
{
  int                 counter;
  PtrPSchema          pPS;
  Counter            *pCnt;

  /* parcourt les compteurs definis dans le schema de presentation */
  /* correspondant a l'attribut pour trouver si l'attribut est la valeur */
  /* initiale d'un compteur */
  {
    /* schema de presentation qui s'applique a l'attribut */
    pPS = PresentationSchema (pAttr->AeAttrSSchema, pDoc);
    if (pPS != NULL)
      for (counter = 0; counter < pPS->PsNCounters; counter++)
        {
          pCnt = &pPS->PsCounter[counter];
          if (pCnt->CnItem[0].CiInitAttr == pAttr->AeAttrNum ||
              pCnt->CnItem[0].CiReinitAttr == pAttr->AeAttrNum)
            /* Si c'est un compteur de pages, on renumerote toutes les */
            /* pages de l'arbre abstrait traite' */
            {
              if (pCnt->CnItem[0].CiCntrOp == CntrRank && pCnt->CnItem[0].
                  CiElemType == PageBreak + 1)
                /* c'est un compteur de la forme CntrRank of Page(view) */
                ComputePageNum (pEl, pDoc, pCnt->CnItem[0].CiViewNum);
              else
                /* c'est un compteur de la forme Set...Add n on Page(view) */
                if (pCnt->CnItem[0].CiCntrOp == CntrSet
                    && pCnt->CnItem[1].CiElemType == PageBreak + 1)
                  ComputePageNum (pEl, pDoc, pCnt->CnItem[1].CiViewNum);
              /* fait reafficher toutes les boites de presentation dependant */
              /* de la valeur de ce compteur */
              UpdateBoxesCounter (pEl, pDoc, counter + 1, pPS, pEl->ElStructSchema);
            }
        }
  }
}

/*----------------------------------------------------------------------
  RemoveAttrPresentation
  Retire la presentation associee a l'attribut pAttr dans les	
  images de l'element pEl.					
  Si pCompAttr != NULL, les regles de presentation dependant	
  de la comparaison doivent prendre pCompAttr comme attribut de   
  comparaison et ne pas en chercher d'autre dans les ascendants	
  de pEl.                                                         
  ----------------------------------------------------------------------*/
void RemoveAttrPresentation (PtrElement pEl, PtrDocument pDoc,
                             PtrAttribute pAttr, PtrElement pElAttr,
                             ThotBool inherit, PtrAttribute pCompAttr)
{
  /* on ne traite pas les marques page */
  if (!pEl->ElTerminal || pEl->ElLeafType != LtPageColBreak)
    {
      /* supprime la presentation attachee a la valeur de l'attribut, si */
      /* elle n'est pas nulle */
      UpdatePresAttr (pEl, pAttr, pElAttr, pDoc, TRUE, inherit, pCompAttr);

      /* reaffiche les variables de presentation qui utilisent */
      /* l'attribut */
      RedisplayAttribute (pAttr, pEl, pDoc);
      UpdateCountersByAttr (pEl, pAttr, pDoc);
    }
}

/*----------------------------------------------------------------------
  Retire du sous arbre pEl du document pDoc les  regles de	
  presentation heritees de l'attribut pAttr.			
  Si pEl porte lui-meme un attribut de type pAttr, on arrete.	
  ----------------------------------------------------------------------*/
void RemoveInheritedAttrPresent (PtrElement pEl, PtrDocument pDoc,
                                 PtrAttribute pAttr, PtrElement pElAttr)
{
  PtrElement          pChild;
  PtrPSchema          pPS;
  InheritAttrTable   *inheritedAttr;
  PtrHandlePSchema    pHd;

  if (GetAttributeOfElement (pEl, pAttr) == NULL)
    {
      /* pEl does not have an attribute of that type */
      /* process the subtree */
      if (!pEl->ElTerminal)
        for (pChild = pEl->ElFirstChild; pChild != NULL;
             pChild = pChild->ElNext)
          if (pChild->ElStructSchema == pEl->ElStructSchema)
            /* same structure schema */
            RemoveInheritedAttrPresent (pChild, pDoc, pAttr, pElAttr);

      /* process element pEl itself */
      pPS = PresentationSchema (pEl->ElStructSchema, pDoc);
      pHd = NULL;
      while (pPS)
        {
          if (pPS->PsNInheritedAttrs->Num[pEl->ElTypeNumber - 1])
            {
              /* pEl can inherit some presentation rules from attributes */
              if ((inheritedAttr = pPS->PsInheritedAttr->ElInherit[pEl->ElTypeNumber - 1]) == NULL)
                {
                  /* la table d'heritage n'existe pas. On la cree */
                  CreateInheritedAttrTable (pEl->ElTypeNumber,
                                            pEl->ElStructSchema, pPS, pDoc);
                  inheritedAttr = pPS->PsInheritedAttr->ElInherit[pEl->ElTypeNumber - 1];
                }
              if ((*inheritedAttr)[pAttr->AeAttrNum - 1])
                RemoveAttrPresentation (pEl, pDoc, pAttr, pElAttr, TRUE, NULL);
            }
          if (pEl->ElTypeNumber > MAX_BASIC_TYPE &&
              pPS->PsNInheritedAttrs->Num[AnyType])
            {
              /* pEl can inherit presentation rules from some attributes
                 (basic types are excluded) */
              if ((inheritedAttr = pPS->PsInheritedAttr->ElInherit[AnyType]) == NULL)
                {
                  /* la table d'heritage n'existe pas. On la cree */
                  CreateInheritedAttrTable (AnyType+1, pEl->ElStructSchema,
                                            pPS, pDoc);
                  inheritedAttr = pPS->PsInheritedAttr->ElInherit[AnyType];
                }
              if ((*inheritedAttr)[pAttr->AeAttrNum - 1])
                RemoveAttrPresentation (pEl, pDoc, pAttr, pElAttr, TRUE, NULL);
            }
          /* next P schema */
          if (pHd)
            /* get the next extension schema */
            pHd = pHd->HdNextPSchema;
          else if (CanApplyCSSToElement (pEl))
            /* extension schemas have not been checked yet */
            /* get the first extension schema */
            pHd = FirstPSchemaExtension (pEl->ElStructSchema, pDoc, pEl);

          if (pHd == NULL)
            /* no more extension schemas. Stop */
            pPS = NULL;
          else
            pPS = pHd->HdPSchema;
        }
    }
}


/*----------------------------------------------------------------------
  Retire du sous-arbre pointe' par pEl du document pDoc les     
  regles de presentation provenant de la comparaison avec		
  l'attribut pAttr.						
  On arrete la recursion quand on rencontre un fils portant       
  lui-meme un attribut de meme type que pAttr			
  ----------------------------------------------------------------------*/
void RemoveComparAttrPresent (PtrElement pEl, PtrDocument pDoc, PtrAttribute pAttr)
{
  PtrElement          pChild;
  PtrPSchema          pPS;
  ComparAttrTable    *attrValComp;
  int                 i;
  PtrAttribute        pCompAttr;

  if (GetAttributeOfElement (pEl, pAttr) == NULL)
    {
      /* on traite d'abord les descendants */
      if (!pEl->ElTerminal)
        for (pChild = pEl->ElFirstChild; pChild != NULL; pChild = pChild->ElNext)
          if (pChild->ElStructSchema == pEl->ElStructSchema)
            /* meme schema de structure */
            RemoveComparAttrPresent (pChild, pDoc, pAttr);
    }

  /* on traite l'element lui-meme */
  /* cherche si pEl possede un attribut se comparant a pAttr */
  pPS = PresentationSchema (pEl->ElStructSchema, pDoc);
  if (pPS != NULL)
    {
      if ((attrValComp = pPS->PsComparAttr->CATable[pAttr->AeAttrNum - 1]) == NULL)
        {
          /* la table de comparaison n'existe pas. On la creee */
          CreateComparAttrTable (pAttr, pDoc, pEl);
          attrValComp = pPS->PsComparAttr->CATable[pAttr->AeAttrNum - 1];
        }
      if (attrValComp != NULL)
        for (i = pEl->ElStructSchema->SsNAttributes; i > 0; i--)
          {
            if ((*attrValComp)[i - 1])
              /* l'attribut de rang i se compare a pAttr */
              if ((pCompAttr = GetTypedAttrForElem (pEl, i, pAttr->AeAttrSSchema)) != NULL)
                /* pEl possede un attribut comme celui de rang i */
                RemoveAttrPresentation (pEl, pDoc, pCompAttr, pEl,
                                        FALSE, pAttr);
          }
    }
}

/*----------------------------------------------------------------------
  Ajoute un attribut pNewAttr a l'element pEl avant l'attribut pAttrNext
  ----------------------------------------------------------------------*/
PtrAttribute AddAttrToElem (PtrElement pEl, PtrAttribute pNewAttr,
                            PtrAttribute pAttrNext)
{
  PtrAttribute        pAttr, pA;
  PtrReference        pRef;
  int                 len;

  GetAttribute (&pAttr);
  /* on chaine cet attribut apres le dernier attribut de  l'element */
  if (pEl)
    {
      if (pEl->ElFirstAttr == NULL || pEl->ElFirstAttr == pAttrNext)
        {
          /* c'est le 1er attribut de l'element */
          pAttrNext = pEl->ElFirstAttr;
          pEl->ElFirstAttr = pAttr;
        }
      else
        {
          /* cherche l'attribut qui doit etre le suivant */
          pA = pEl->ElFirstAttr;
          while (pA->AeNext != pAttrNext)
            pA = pA->AeNext;
          /* chaine le nouvel attribut */
          pA->AeNext = pAttr;
        }
    }
  pAttr->AeNext = pAttrNext;
  pAttr->AeAttrSSchema = pNewAttr->AeAttrSSchema;
  pAttr->AeAttrNum = pNewAttr->AeAttrNum;
  pAttr->AeDefAttr = FALSE;
  pAttr->AeAttrType = pNewAttr->AeAttrType;
  switch (pAttr->AeAttrType)
    {
    case AtEnumAttr:
    case AtNumAttr:
      pAttr->AeAttrValue = pNewAttr->AeAttrValue;
      break;
    case AtTextAttr:
      if (pNewAttr->AeAttrText != NULL)
        {
          GetTextBuffer (&pAttr->AeAttrText);
          CopyTextToText (pNewAttr->AeAttrText, pAttr->AeAttrText, &len);
        }
      break;
    case AtReferenceAttr:
      GetReference (&pRef);
      pAttr->AeAttrReference = pRef;
      pRef->RdElement = pEl;
      pRef->RdAttribute = pAttr;
      pRef->RdReferred = pNewAttr->AeAttrReference->RdReferred;
      pRef->RdTypeRef = pNewAttr->AeAttrReference->RdTypeRef;
      /* chaine la reference du nouvel attribut apres celle de */
      /* pNewAttr. */
      if (pEl)
        {
          pRef->RdNext = pNewAttr->AeAttrReference->RdNext;
          if (pRef->RdNext != NULL)
            pRef->RdNext->RdPrevious = pRef;
          pRef->RdPrevious = pNewAttr->AeAttrReference;
          if (pRef->RdPrevious != NULL)
            pRef->RdPrevious->RdNext = pRef;
        }
      break;
    default:
      break;
    }
  return (pAttr);
}

/*----------------------------------------------------------------------
  AttachAttrWithValue
  Met dans l'element pEl la valeur de l'attribut pNewAttr		
  Les regles de presentation de cette nouvelle valeur sont	
  appliquees a l'element.
  Egalement utilise' pour supprimer un attribut et desappliquer les
  regles de presentation correspondantes.
  ----------------------------------------------------------------------*/
void AttachAttrWithValue (PtrElement pEl, PtrDocument pDoc,
                          PtrAttribute pNewAttr, ThotBool history)
{
  PtrAttribute        pAttr, pAttrAsc, pAttrNext;
  PtrElement          pChild, pElAttr;
  PtrPSchema          pPS;
  PtrHandlePSchema    pHd;
  ThotBool            suppress, compare, inherit, mandatory, create, allowed;
  NotifyAttribute     notifyAttr;

  if (pNewAttr == NULL || pNewAttr->AeAttrNum == 0)
    /* it's not a valid attribute */
    return;
  mandatory = FALSE;
  pAttrNext = NULL;

  /* l'element porte-t-il deja un attribut du meme type ? */
  pAttr = AttributeValue (pEl, pNewAttr);
  create = (pAttr == NULL);

  /* est-ce une suppression d'attribut */
  suppress = FALSE;
  switch (pNewAttr->AeAttrType)
    {
    case AtEnumAttr:
      if (pNewAttr->AeAttrValue == 0)
        suppress = TRUE;
      else
        if (pAttr && pNewAttr->AeAttrValue == pAttr->AeAttrValue)
          /* the existing attribute already has this value */
          return;
      break;

    case AtNumAttr:
      if (pNewAttr->AeAttrValue < -MAX_INT_ATTR_VAL ||
          pNewAttr->AeAttrValue > MAX_INT_ATTR_VAL)
        suppress = TRUE;
      else
        if (pAttr && pNewAttr->AeAttrValue == pAttr->AeAttrValue)
          /* the existing attribut already has this value */
          return;
      break;

    case AtTextAttr:
      if (pNewAttr->AeAttrText == NULL)
        suppress = TRUE;
      else
        if (pAttr && TextsEqual (pAttr->AeAttrText, pNewAttr->AeAttrText))
          return;
      break;

    case AtReferenceAttr:
      if (pNewAttr->AeAttrReference == NULL ||
          pNewAttr->AeAttrReference->RdReferred == NULL)
        suppress = TRUE;
      else
        if (pAttr && pAttr->AeAttrReference &&
            pAttr->AeAttrReference->RdReferred ==
            pNewAttr->AeAttrReference->RdReferred)
          return;
      break;

    default:
      break;
    }

  /* peut-on appliquer l'attribut a l'element ? */
  allowed = CanAssociateAttr (pEl, pAttr, pNewAttr, &mandatory);
  if (!allowed && !suppress)
    return;

  if (suppress)
    {
      if (pAttr == NULL)
        /* suppression d'un attribut inexistant */
        return;
      if (!mandatory)
        /* si c'est un attribut Langue sur un element racine,
           il est obligatoire */
        if (pEl->ElParent == NULL)
          if (pNewAttr->AeAttrNum == 1)
            mandatory = TRUE;
      if (mandatory)
        /* suppression d'un attribut obligatoire. Interdit */
        return;
    }

  /* faut-il traiter des heritages et comparaisons d'attributs */
  inherit = FALSE;
  compare = FALSE;
  pPS = PresentationSchema (pNewAttr->AeAttrSSchema, pDoc);
  pHd = NULL;
  while (pPS)
    {
      inherit = inherit ||
        (pPS->PsNHeirElems->Num[pNewAttr->AeAttrNum - 1] != 0);
      compare = compare ||
        (pPS->PsNComparAttrs->Num[pNewAttr->AeAttrNum - 1] != 0);
      
      /* next P schema */
      if (pHd)
        /* get the next extension schema */
        pHd = pHd->HdNextPSchema;
      else if (CanApplyCSSToElement (pEl))
        /* extension schemas have not been checked yet */
        /* get the first extension schema */
        pHd = FirstPSchemaExtension (pNewAttr->AeAttrSSchema, pDoc, pEl);

      if (pHd == NULL)
        /* no more extension schemas. Stop */
        pPS = NULL;
      else
        pPS = pHd->HdPSchema;	
    }
  if (inherit || compare)
    /* cherche le premier attribut de meme type sur un ascendant de pEl */
    pAttrAsc = GetTypedAttrAncestor (pEl, pNewAttr->AeAttrNum,
                                     pNewAttr->AeAttrSSchema, &pElAttr);
  else
    pAttrAsc = NULL;

  /* prepare et envoie l'evenement pour l'application */
  if (create)
    notifyAttr.event = TteAttrCreate;
  else if (suppress)
    notifyAttr.event = TteAttrDelete;
  else
    notifyAttr.event = TteAttrModify;
  notifyAttr.document = (Document) IdentDocument (pDoc);
  notifyAttr.element = (Element) pEl;
  notifyAttr.info = 0; /* not sent by undo */
  notifyAttr.attribute = (Attribute) pAttr;
  notifyAttr.attributeType.AttrSSchema = (SSchema) (pNewAttr->AeAttrSSchema);
  notifyAttr.attributeType.AttrTypeNum = pNewAttr->AeAttrNum;
  if (!CallEventAttribute (&notifyAttr, TRUE))
    {
      /* l'application accepte l'operation */
      /* on supprime sur pEl et son sous arbre les regles de presentation
         anciennes liees a l'heritage d'un attribut du type pAttr porte'
         soit par pEl soit par un ascendant */
      if (pAttr)
        {
          /* register the attribute in history */
          if (history && ThotLocalActions[T_attraddhistory] != NULL)
            (*(Proc5)ThotLocalActions[T_attraddhistory]) (
                                                          (void *)pAttr,
                                                          (void *)pEl,
                                                          (void *)pDoc,
                                                          (void *)TRUE,
                                                          (void *)FALSE);
          /* detache l'attribut de l'element s'il y a lieu */
          pAttrNext = pAttr->AeNext;
          /* supprime l'attribut */
          RemoveAttribute (pEl, pAttr);
          /* heritage et comparaison sont lies a un attribut de pEl */
          /* On supprime d'abord les regles de presentation liees a
             l'attribut sur l'element lui-meme */
          RemoveAttrPresentation (pEl, pDoc, pAttr, pEl, FALSE, NULL);
          /* indique que le document a ete modifie' */
          /* un changement d'attribut vaut dix caracteres saisis */
          SetDocumentModified (pDoc, TRUE, 10);
          /* On supprime de pEl de son sous-arbre  la presentation venant
             de l'heritage de cet attribut par le sous-arbre, s'il existe
             des elements heritants de celui-ci */
          if (inherit)
            RemoveInheritedAttrPresent (pEl, pDoc, pAttr, pEl);
          /* On supprime des elements du sous arbre pEl la presentation
             venant de la comparaison d'un attribut du sous-arbre avec ce
             type d'attribut */
          if (!pEl->ElTerminal && compare)
            for (pChild = pEl->ElFirstChild; pChild != NULL;
                 pChild = pChild->ElNext)
              RemoveComparAttrPresent (pChild, pDoc, pAttr);
          DeleteAttribute (pEl, pAttr);
        }

      else if (pAttrAsc)
        {
          /* heritage et comparaison sont dus a un attribut d'un ascendant
             de pEl */
          /* on supprime du sous arbre pEl la presentation venant de
             l'heritage de cet attribut par le sous-arbre, s'il existe des
             elements heritants de celui-ci */
          if (inherit)
            RemoveInheritedAttrPresent (pEl, pDoc, pAttrAsc, pElAttr);
          /* on supprime du sous-arbre pEl la presentation venant de la
             comparaison d'un attribut du sous-arbre avec ce type d'attribut */
          if (compare)
            RemoveComparAttrPresent (pEl, pDoc, pAttrAsc);
        }

      if (inherit || compare)
        /* cherche le premier attribut de meme type sur un ascendant de pEl */
        pAttrAsc = GetTypedAttrAncestor (pEl, pNewAttr->AeAttrNum,
                                         pNewAttr->AeAttrSSchema, &pElAttr);
      else
        pAttrAsc = NULL;

      /* on met l'attribut */
      if (!suppress)
        {
          /* add a copy of the new attribute before pAttrNext */
          pAttr = AddAttrToElem (pEl, pNewAttr, pAttrNext);
          /* register the attribute in history */
          if (history && ThotLocalActions[T_attraddhistory] != NULL)
            (*(Proc5)ThotLocalActions[T_attraddhistory]) (
                                                          (void *)pAttr,
                                                          (void *)pEl,
                                                          (void *)pDoc,
                                                          (void *)FALSE,
                                                          (void *)TRUE);
          /* indique que le document a ete modifie' */
          /* un changement d'attribut vaut dix caracteres saisis */
          SetDocumentModified (pDoc, TRUE, 10);
        }

      else
        /* suppression */
        {
          /* valide les modifications */
          AbstractImageUpdated (pDoc);
          /* la suppression est maintenant prise en compte dans les
             copies-inclusions de l'element */
          RedisplayCopies (pEl, pDoc, TRUE);
        }

      /* on applique les regles de presentation */
      if (pAttr != NULL && !suppress)
        {
          /* applique les regles du nouvel attribut */
          /* applique d'abord les regles de presentation associees a
             l'attribut sur l'element lui-meme */
          ApplyAttrPRulesToElem (pEl, pDoc, pAttr, pEl, FALSE);
          /* applique les regles de presentation venant de l'heritage de
             cet attribut par le sous-arbre s'il existe des elements qui
             heritent */
          if (inherit)
            ApplyAttrPRulesToSubtree (pEl, pDoc, pAttr, pEl);
          /* applique sur les elements du sous-arbre les regles de
             presentation venant de la comparaison d'un attribut du
             sous-arbre avec ce type d'attribut */
          if (!pEl->ElTerminal && compare)
            for (pChild = pEl->ElFirstChild; pChild != NULL;
                 pChild = pChild->ElNext)
              ApplyAttrPRules (pChild, pDoc, pAttr);
          if (pAttr->AeAttrType == AtNumAttr)
            /* s'il s'agit d'un attribut initialisant un compteur, il */
            /* faut mettre a jour les boites utilisant ce compteur */
            UpdateCountersByAttr (pEl, pNewAttr, pDoc);

          /* le nouvel attribut est pris en compte dans les 
             les copies-inclusions de l'element */
          RedisplayCopies (pEl, pDoc, TRUE);
        }
      else if (pAttrAsc != NULL && suppress)
        {
          /* applique les regles venant de l'heritage ou de la comparaison
             avec un ascendant */
          /* applique sur les elements du sous-arbre les regles de
             presentation venant de l'heritage de cet attribut par le
             sous-arbre, s'il existe des elements qui heritent */
          ApplyAttrPRulesToSubtree (pEl, pDoc, pAttrAsc, pElAttr);
          /* applique sur les elements du sous-arbre les regles de
             presentation venant de la comparaison d'un attribut de
             sous-arbre avec ce type d'attribut */
          ApplyAttrPRules (pEl, pDoc, pAttrAsc);
        }

      /* prepare et envoie a l'application l'evenement TteAttrCreate.Post */
      if (notifyAttr.event == TteAttrCreate)
        notifyAttr.attribute = (Attribute) pAttr;
      else if (notifyAttr.event == TteAttrDelete)
        notifyAttr.attribute = NULL;
      CallEventAttribute (&notifyAttr, FALSE);
    }
}

/*----------------------------------------------------------------------
  Retourne un pointeur sur l'attribut associe' a l'exception      
  ExceptNum et porte' par l'element pEl.				
  On retourne NULL si l'element n'a pas cet attribut.		
  ----------------------------------------------------------------------*/
PtrAttribute        GetAttrByExceptNum (PtrElement pEl, int ExceptNum)
{
  PtrSSchema          pSS;
  PtrAttribute        pAttr;
  ThotBool            found;
  int                 attrNum;

  pSS = pEl->ElStructSchema;
  /* on recupere le numero d'attribut associe a l'exception */
  attrNum = GetAttrWithException (ExceptNum, pEl->ElStructSchema);
  if (attrNum != 0)
    {
      /* on cherche un attribut sur l'element */
      pAttr = pEl->ElFirstAttr;
      found = FALSE;
      while (pAttr != NULL && !found)
        if (!strcmp (pAttr->AeAttrSSchema->SsName, pSS->SsName) &&
            pAttr->AeAttrNum == attrNum)
          found = TRUE;
        else
          pAttr = pAttr->AeNext;
      return pAttr;
    }
  else
    return NULL;
}

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
Attribute TtaGetAttribute (Element element, AttributeType attributeType)
{
  PtrAttribute        pAttr;
  PtrAttribute        attribute;
  ThotBool            found;
  ThotBool            error;

  UserErrorCode = 0;
  attribute = NULL;
  if (element == NULL || (attributeType.AttrSSchema == NULL && attributeType.AttrTypeNum != 1))
    /* attributeType.AttrTypeNum = 1 : attribute Language in the whole schema */
    TtaError (ERR_invalid_parameter);
  else
    {
      error = FALSE;
      /* No other verification if the attibute is "language" */
      if (attributeType.AttrTypeNum != 1)
        if (attributeType.AttrTypeNum < 1 ||
            attributeType.AttrTypeNum > ((PtrSSchema) (attributeType.AttrSSchema))->SsNAttributes)
          error = TRUE;
      if (error)
        TtaError (ERR_invalid_attribute_type);
      else
        {
          attribute = NULL;
          pAttr = ((PtrElement) element)->ElFirstAttr;
          found = FALSE;
          while (pAttr != NULL && !found)
            {
              if (pAttr->AeAttrNum == attributeType.AttrTypeNum)
                {
                  /* Same attribute number */
                  if (attributeType.AttrSSchema == NULL)
                    /* The structure schema does not interest us */
                    found = TRUE;
                  else if (!strcmp (pAttr->AeAttrSSchema->SsName,
                                    ((PtrSSchema) (attributeType.AttrSSchema))->SsName))
                    /* Same schema of structure */
                    found = TRUE;
                }
              if (found)
                attribute = pAttr;
              else
                pAttr = pAttr->AeNext;
            }
        }
    }
  return ((Attribute) attribute);
}

/* ----------------------------------------------------------------------
   TtaGetTextAttributeLength
   Returns the length of a given attribute of type text.

   Parameter:
   attribute: the attribute of interest.
   Return values:
   length of the character string contained in the attribute.
   ---------------------------------------------------------------------- */
int TtaGetTextAttributeLength (Attribute attribute)
{
  int                 length;
  unsigned char       c[10], *ptr;
  int                 i;
  PtrTextBuffer       pBuf;

  UserErrorCode = 0;
  length = 0;
  if (attribute == NULL)
    TtaError (ERR_invalid_attribute_type);
  else if (((PtrAttribute) attribute)->AeAttrType != AtTextAttr)
    TtaError (ERR_invalid_attribute_type);
  else
    {
      pBuf = ((PtrAttribute) attribute)->AeAttrText;
      while (pBuf != NULL)
        {
          i = 0;
          while (i < pBuf->BuLength)
            {
              ptr = c;
              length += TtaWCToMBstring (pBuf->BuContent[i], &ptr);
              i++;
            }
          pBuf = pBuf->BuNext;
        }
    }
  return length;
}

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
void TtaGiveTextAttributeValue (Attribute attribute, char *buffer, int *length)
{
  UserErrorCode = 0;
  *buffer = EOS;
  if (attribute == NULL)
    TtaError (ERR_invalid_attribute_type);
  else if (((PtrAttribute) attribute)->AeAttrType != AtTextAttr)
    TtaError (ERR_invalid_attribute_type);
  else
    *length = CopyBuffer2MBs (((PtrAttribute) attribute)->AeAttrText, 0,
                              (unsigned char*)buffer, *length);
}

/* ----------------------------------------------------------------------
   TtaGetAttributeKind

   Returns the kind of the attribute: 0 = Enumerate, 1 = Integer, 2 = Text,
   3 = CsReference.
   Parameter:
   attType: type of the attribute.
   ---------------------------------------------------------------------- */
int TtaGetAttributeKind (AttributeType attType)
{
  PtrSSchema          pSS;
  int                 attrKind = 2;

  UserErrorCode = 0;
  if (attType.AttrSSchema == NULL || attType.AttrTypeNum <= 0 ||
      attType.AttrTypeNum > ((PtrSSchema)(attType.AttrSSchema))->SsNAttributes)
    TtaError (ERR_invalid_parameter);
  else
    {
      pSS = (PtrSSchema)(attType.AttrSSchema);
      switch (pSS->SsAttribute->TtAttr[attType.AttrTypeNum - 1]->AttrType)
        {
        case AtEnumAttr:
          attrKind = 0;
          break;
        case AtNumAttr:
          attrKind = 1;
          break;
        case AtTextAttr:
          attrKind = 2;
          break;
        case AtReferenceAttr:
          attrKind = 3;
          break;
        default:
          break;
        }
    } 
  return attrKind;
}
