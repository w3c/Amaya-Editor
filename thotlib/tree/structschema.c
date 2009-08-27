/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Some functions for handling structure rules
 *
 * Authors: V. Quint (INRIA)
 *          C. Roisin (INRIA) - Columns and pages
 *
 */
#include "thot_gui.h"
#include "thot_sys.h"
#include "libmsg.h"
#include "message.h"
#include "constmedia.h"

#include "typemedia.h"
#include "view.h"
#include "document.h"
#include "appdialogue.h"

#define THOT_EXPORT extern
#include "edit_tv.h"
#include "platform_tv.h"
#include "appdialogue_tv.h"

#include "exceptions_f.h"
#include "memory_f.h"
#include "schemas_f.h"
#include "structschema_f.h"
#include "tree_f.h"

/*----------------------------------------------------------------------
  CreateDocument acquiert et initialise un contexte de document.    
  Au retour, pDoc contient un pointeur sur le contexte de 
  document.                                               
  ----------------------------------------------------------------------*/
void CreateDocument (PtrDocument *pDoc, Document *document)
{
  Document        doc;

  if (*document == 0 || *document >= MAX_DOCUMENTS)
    {
      /* cherche un pointeur de descripteur de document libre */
      doc = 0;
      while (doc < MAX_DOCUMENTS && LoadedDocument[doc] != NULL)
        doc++;
      if (doc >= MAX_DOCUMENTS)
        {
          *pDoc = NULL;
          *document = 0;
          return;
        }
      else
        *document = doc + 1;
    }
  else
    doc = *document - 1;

  /* acquiert un descripteur de document */
  GetDocument (&LoadedDocument[doc]);
  *pDoc = LoadedDocument[doc];
  /* initialise le mode d'affichage */
  documentDisplayMode[doc] = DisplayImmediately;
  (*pDoc)->DocCheckingMode = (unsigned char)DEFAULT_CHECK_MASK;
  (*pDoc)->DocCharset = ISO_8859_1;
  (*pDoc)->DocDefaultCharset = TRUE;
}

/*----------------------------------------------------------------------
  GetSRuleFromName cherche dans le schema de structure pSS (et dans les    
  extensions et schemas de structures utilises comme      
  natures par ce schema), une regle de nom typeName
  si whichName = USER_NAME typeName est le nom traduit
  si whichName = SCHEMA_NAME typeName est le nom defini dans le schema
  Retourne dans pSS le schema contenant la                
  regle trouvee et dans typeNum le numero de la regle     
  trouvee dans ce schema. typeNum vaut zero si le nom     
  n'est pas trouve'.                                      
  ----------------------------------------------------------------------*/
void GetSRuleFromName (int *typeNum, PtrSSchema *pSS, char *typeName,
                       int whichName)
{
  int                 ruleNum;
  PtrSSchema          pSSch;
  char               *ruleName;

  /* on n'a pas encore trouve' */
  *typeNum = 0;
  /* pointeur sur le schema ou l'on cherche */
  pSSch = *pSS;
  ruleNum = 0;
  if (pSSch != NULL)
    {
      do
        /* cherche dans les regles de structure du schema */
        {
          /* si c'est une regle de changement de nature, on prendra la
             regle racine de la nature */
          if (whichName == SCHEMA_NAME)
            ruleName = pSSch->SsRule->SrElem[ruleNum]->SrOrigName;
          else
            ruleName = pSSch->SsRule->SrElem[ruleNum]->SrName;
          if (strcmp (typeName, ruleName) == 0
              && pSSch->SsRule->SrElem[ruleNum]->SrConstruct != CsNatureSchema)
            /* trouve' */
            {
              *typeNum = ruleNum + 1;
              *pSS = pSSch;
            }
          else if (pSSch->SsRule->SrElem[ruleNum]->SrConstruct == CsNatureSchema)
            /* une nature, cherche dans son schema de structure */
            {
              *pSS = pSSch->SsRule->SrElem[ruleNum]->SrSSchemaNat;
              GetSRuleFromName (typeNum, pSS, typeName, whichName);
            }
          ruleNum++;
        }
      while (*typeNum == 0 && ruleNum < pSSch->SsNRules);
      if (*typeNum == 0)
        /* on n'a pas trouve' dans le schema ni dans ses natures, on cherche */
        /* dans les extensions du schema */
        if (pSSch->SsNextExtens != NULL)
          {
            *pSS = pSSch->SsNextExtens;
            GetSRuleFromName (typeNum, pSS, typeName, whichName);
          }
    }
}

/*----------------------------------------------------------------------
  GetAttrRuleFromName cherche dans les schema de structure de pEl et de
  ses ancetres (et dans les extensions de ces schemas), un attribut de 
  nom attrName, pouvant etre attache a l'element pEl
  si whichName = USER_NAME attrName est le nom traduit
  si whichName = SCHEMA_NAME attrName est le nom defini dans le schema
  Retourne dans pSSch le schema contenant la                
  regle trouvee et dans attrNum le numero de l'attribut     
  trouve dans ce schema. attrNum vaut zero si le nom     
  n'est pas trouve'.                                      
  ----------------------------------------------------------------------*/
void GetAttrRuleFromName (int *attrNum, PtrSSchema *pSSch,
                          PtrElement pElem, const char *attrName, int whichName)
{
  int                 i;
  ThotBool            found, newCshema;
  PtrSSchema          pSS;
  PtrElement	      pEl;
  PtrSRule            pRe1;
#define MaxSch 20
  PtrSSchema          attrStruct[MaxSch];
  char               *name;
  int                 att, schNumber;

  name = NULL;
  pSS = NULL;
  att = 0;
  found = FALSE;
  schNumber = 0;
  pEl = pElem;
  /* looks for all structure schemas used by the ancestors elements */
  while (pEl != NULL && !found)
    {
      /* the structure schema of the current element */
      pSS = pEl->ElStructSchema;
      /* one go throw all extension schemas of this one */
      do
        {
          /* is this schema already treated ? */
          newCshema = TRUE;
          for (i = 1; i <= schNumber; i++)	/* glance of the table */
            if (pSS == attrStruct[i - 1])	/* already in the table */
              newCshema = FALSE;
          if (newCshema)
            /* The element uses a structure schema not found yet */
            {
              /* Puts the structure schema in the table */
              if (schNumber < MaxSch)
                {
                  schNumber++;
                  attrStruct[schNumber - 1] = pSS;
                }
              /* verifies all the global attributes of this schema */
              att = 0;
              while (att < pSS->SsNAttributes && !found)
                {
                  att++;
                  /* The local attributes are not considered */
                  if (pSS->SsAttribute->TtAttr[att - 1]->AttrGlobal)
                    {
                      if (whichName == SCHEMA_NAME)
                        name = pSS->SsAttribute->TtAttr[att - 1]->AttrOrigName;
                      else
                        name = pSS->SsAttribute->TtAttr[att - 1]->AttrName;
                      if (strcmp (attrName, name) == 0)
                        found = TRUE;
                    }
                }
            }
          if (!found)
            /* Go to the next extension schema */
            pSS = pSS->SsNextExtens;
        }
      while (pSS != NULL && !found);
      pEl = pEl->ElParent;	/* Go the the ancestor element */
    }
  if (!found)
    {
      /* looks in the local attributes of the element */
      /* at first, looks at the rule defining this element */
      pSS = pElem->ElStructSchema;
      pRe1 = pSS->SsRule->SrElem[pElem->ElTypeNumber - 1];
      do
        {
          if (pRe1 != NULL)
            /* verify the local attributes defined in this rule */
            for (i = 1; i <= pRe1->SrNLocalAttrs && !found; i++)
              {
                att = pRe1->SrLocalAttr->Num[i - 1];
                if (whichName == SCHEMA_NAME)
                  name = pSS->SsAttribute->TtAttr[att - 1]->AttrOrigName;
                else
                  name = pSS->SsAttribute->TtAttr[att - 1]->AttrName;
                if (strcmp (attrName, name) == 0)
                  found = TRUE;
              }
          if (!found)
            {
              /* Go to the next extension of the structure schema */
              pSS = pSS->SsNextExtens;
              /* look in schema extension the extension rule for the element */
              if (pSS != NULL)
                pRe1 = ExtensionRule (pElem->ElStructSchema,
                                      pElem->ElTypeNumber, pSS);
            }
        }
      while (pSS != NULL && !found);
    }
  if (found)
    {
      *attrNum = att;
      *pSSch = pSS;
    }
  else
    *attrNum = 0;
}

/*----------------------------------------------------------------------
  GetTypeNumIdentity						
  ----------------------------------------------------------------------*/
int                 GetTypeNumIdentity (int typeNum, PtrSSchema pSS)
{
  while (pSS->SsRule->SrElem[typeNum - 1]->SrConstruct == CsIdentity)
    typeNum = pSS->SsRule->SrElem[typeNum - 1]->SrIdentRule;
  return typeNum;
}


/*----------------------------------------------------------------------
  SameSRules teste si les regles de numero typeNum1 et		
  typeNum2 dans les schemas de structure pSS1 et pSS2 sont egales	
  ----------------------------------------------------------------------*/
ThotBool            SameSRules (int typeNum1, PtrSSchema pSS1, int typeNum2,
                                PtrSSchema pSS2)
{
  ThotBool            ret;
  
  ret = FALSE;
  if (typeNum1 == typeNum2)
    {
      if (typeNum2 <= MAX_BASIC_TYPE)
        /* les types de base sont les memes dans tous les schemas */
        ret = TRUE;
      else if ((typeNum1 < pSS1->SsFirstDynNature || pSS1->SsFirstDynNature == 0) &&
               !strcmp (pSS1->SsName, pSS2->SsName))
        /* ce n'est pas une regle ajoutee par l'editeur pour une */
        /* nature chargee dynamiquement */
        ret = TRUE;
    }
  return ret;
}

/*----------------------------------------------------------------------
  EquivalentSRules retourne vrai si la regle 1 (numero typeNum1 dans le	
  schema de structure pointe par pSS1) peut conduire a la regle 2	
  (numero typeNum2 dans le schema de structure pointe par pSS2)	
  uniquement par des regles d'CsIdentity, de nature et de CsChoice.	
  pEl est l'element d'arbre abstrait pour lequel on travaille.	
  ----------------------------------------------------------------------*/
ThotBool            EquivalentSRules (int typeNum1, PtrSSchema pSS1,
                                      int typeNum2, PtrSSchema pSS2,
                                      PtrElement pEl)
{
  ThotBool            ret;
  int                 i;
  ThotBool            test;
  PtrSRule            pSRule;
  PtrElement          pAsc, pPrev;
  ThotBool            ok;
  PtrSSchema          pSSch;

  if (typeNum1 == 0 || typeNum2 == 0 || pSS1 == NULL || pSS2 == NULL)
    ret = FALSE;
  else if (SameSRules (typeNum1, pSS1, typeNum2, pSS2))
    ret = TRUE;
  else
    {
      ret = FALSE;
      test = TRUE;
      pSRule = pSS1->SsRule->SrElem[typeNum1 - 1];
      if (pSRule->SrRecursive)
        {
          if (pSRule->SrRecursDone)
            test = FALSE;
          else
            pSRule->SrRecursDone = TRUE;
        }
      if (test)
        switch (pSRule->SrConstruct)
          {
          case CsIdentity:
            if (pSRule->SrIdentRule > MAX_BASIC_TYPE)
              ret = EquivalentSRules (pSRule->SrIdentRule, pSS1, typeNum2, pSS2, pEl);
            break;
          case CsAny:
            ret = TRUE;
            break;
          case CsNatureSchema:
            if (pSRule->SrSSchemaNat == NULL)
              /* structure schema is not loaded. Compare names */
              ret = !strcmp (pSRule->SrOrigName,
                             pSS2->SsRule->SrElem[typeNum2 -1]->SrOrigName);
            else
              /* schema is loaded, compare rules */
              ret = SameSRules (pSRule->SrSSchemaNat->SsRootElem,
                                pSRule->SrSSchemaNat, typeNum2, pSS2);
            break;
          case CsChoice:
            if (pSRule->SrNChoices == -1)
              /* equivalent only if external nature */
              if (typeNum2 == pSS2->SsRootElem)
                ret = TRUE;
              else
                ret = FALSE;
            else if (pSRule->SrNChoices == 0)
              /* UNIT rule */
              /* equivalent only if basic type or exported unit */
              {
                if (typeNum2 <= MAX_BASIC_TYPE)
                  /* basic type */
                  ret = TRUE;
                else if (pSS2->SsRule->SrElem[typeNum2 - 1]->SrUnitElem)
                  /* exported unit */
                  ret = TRUE;
                else
                  /* searches all units in the structure schema
                     and in all its extensions */
                  {
                    pSSch = pSS1;
                    /* if it's an extension, return to initial
                       schema for handling all extensions */
                    while (pSSch->SsPrevExtens != NULL)
                      pSSch = pSSch->SsPrevExtens;
                    /* process all extensions */
                    do
                      {
                        /* search units defined in this schema */
                        i = 0;
                        do
                          if (pSSch->SsRule->SrElem[i++]->SrUnitElem)
                            ret = EquivalentSRules (i, pSSch, typeNum2, pSS2, pEl);
                        while (!ret && i < pSSch->SsNRules) ;
                        if (!ret)
                          pSSch = pSSch->SsNextExtens;
                      }
                    while (!ret && pSSch != NULL);
                  }
                if (!ret && pEl != NULL)
                  /* not found. Is it an unit defined in the */
                  /* schema of an ancestor element? */
                  {
                    pAsc = pEl;
                    pPrev = NULL;
                    while (pAsc != NULL && !ret)
                      {
                        if (pPrev == NULL)
                          ok = TRUE;
                        else
                          ok = pAsc->ElStructSchema != pPrev->ElStructSchema;
                        if (ok)
                          /* the ancestor belongs to a different
                             structure schema */
                          {
                            pSSch = pAsc->ElStructSchema;
                            /* if it's an extension, return
                               to the initial schema for handling
                               all other extensions */
                            while (pSSch->SsPrevExtens != NULL)
                              pSSch = pSSch->SsPrevExtens;
                            do
                              {
                                for (i = 0; i < pSSch->SsNRules && !ret; i++)
                                  if (pSSch->SsRule->SrElem[i]->SrUnitElem)
                                    ret = EquivalentSRules (i + 1, pSSch, typeNum2, pSS2, pAsc);
                                if (!ret)
                                  pSSch = pSSch->SsNextExtens;
                              }
                            while (!ret && pSSch != NULL);
                          }
                        /* next ancestor */
                        pPrev = pAsc;
                        pAsc = pAsc->ElParent;
                      }
                  }
              }
            else
              /* choice. Check all options */
              {
                i = 0;
                do
                  {
                    if (pSS1->SsRule->SrElem[pSRule->SrChoice[i] - 1]->SrConstruct
                        == CsNatureSchema ||
                        /* current option is a nature */
                        pSS1->SsRule->SrElem[pSRule->SrChoice[i] - 1]->SrConstruct
                        == CsChoice)
                      /* current option is a choice */
                      ret = EquivalentSRules (pSRule->SrChoice[i], pSS1,
                                              typeNum2, pSS2, pEl);
                    else
                      ret = SameSRules (pSRule->SrChoice[i], pSS1,
                                        typeNum2, pSS2);
                    i++;
                  }
                while (!ret && i < pSRule->SrNChoices);
              }
            break;
          default:
            break;
          }
      if (pSRule->SrRecursive && pSRule->SrRecursDone && test)
        pSRule->SrRecursDone = FALSE;
    }
  return ret;
}

/*----------------------------------------------------------------------
  IdentRuleOfElem  si l'element de type (typeNum, pSS) peut
  etre un fils d'un element CsIdentity, retourne le numero de
  la regle definissant cet element CsIdentity.
  Retourne 0 sinon.
  ----------------------------------------------------------------------*/
int                 IdentRuleOfElem (int typeNum, PtrSSchema pSS)
{
  int                 rule, ret;

  ret = 0;
  for (rule = MAX_BASIC_TYPE; ret == 0 && rule < pSS->SsNRules; rule++)
    if (pSS->SsRule->SrElem[rule]->SrConstruct == CsIdentity)
      if (pSS->SsRule->SrElem[rule]->SrIdentRule == typeNum)
        ret = rule + 1;
  return ret;
}

/*----------------------------------------------------------------------
  ListRuleOfElem  si l'element de type (typeNum, pSS) peut	
  etre un fils d'un element CsList, retourne le numero de  
  la regle definissant cet element CsList.                 
  Retourne 0 sinon.                                       
  ----------------------------------------------------------------------*/
int                 ListRuleOfElem (int typeNum, PtrSSchema pSS)
{
  int                 rule, ret;

  ret = 0;
  for (rule = MAX_BASIC_TYPE; ret == 0 && rule < pSS->SsNRules; rule++)
    if (pSS->SsRule->SrElem[rule]->SrConstruct == CsList)
      if (EquivalentSRules (pSS->SsRule->SrElem[rule]->SrListItem, pSS, typeNum, pSS,
                            NULL))
        ret = rule + 1;
  return ret;
}

/*----------------------------------------------------------------------
  AggregateRuleOfElem    si l'element de type (typeNum, pSS) peut	
  etre un fils d'un element CsAggregate, retourne le numero   
  de la regle definissant cet element CsAggregate.            
  Retourne 0 sinon.                                       
  ----------------------------------------------------------------------*/
int                 AggregateRuleOfElem (int typeNum, PtrSSchema pSS)
{
  int                 rule, comp, ret;
  PtrSRule            pRule;

  ret = 0;
  for (rule = MAX_BASIC_TYPE; ret == 0 && rule < pSS->SsNRules; rule++)
    {
      pRule = pSS->SsRule->SrElem[rule];
      if (pRule->SrConstruct == CsAggregate ||
          pRule->SrConstruct == CsUnorderedAggregate)
        for (comp = 0; ret == 0 && comp < pRule->SrNComponents; comp++)
          if (EquivalentSRules (pRule->SrComponent[comp], pSS, typeNum,
                                pSS, NULL))
            ret = rule + 1;
    }
  return ret;
}

/*----------------------------------------------------------------------
  ExcludedType verifie si l'element pointe par pEl et ses ascendants
  excluent le type d'element de numero typeNum defini dans le	
  schema de structure pSS.					
  ----------------------------------------------------------------------*/
ThotBool            ExcludedType (PtrElement pEl, int typeNum, PtrSSchema pSS)
{
  PtrSRule            pRule;
  int                 i;
  PtrSSchema          pSSasc, pSSExt;
  ThotBool            StrSchOK;
  ThotBool            ret;

  /* a priori ce type n'est pas exclus */
  ret = FALSE;
  /* s'il s'agit d'un type de base, peu importe le schema de */
  /* structure auquel il appartient */
  if (typeNum <= MAX_BASIC_TYPE)
    pSS = NULL;
  /* examine la regle de structure de l'element et de ses ascendants */
  while (pEl != NULL && !ret)
    {
      /* accede a la regle de structure de l'element */
      pSSasc = pEl->ElStructSchema;
      pRule = pSSasc->SsRule->SrElem[pEl->ElTypeNumber - 1];
      pSSExt = NULL;
      do
        {
          if (pRule != NULL)
            {
              if (pSS == NULL)
                /* n'importe quel schema de structure convient */
                StrSchOK = TRUE;
              else
                /* On compare les codes identifiant les schemas de structure */
                StrSchOK = !strcmp (pSSasc->SsName, pSS->SsName);
              if (StrSchOK)
                /* les schemas de structure correspondent */
                {

                  /* examine toutes les exclusions definies dans la regle */
                  for (i = 0; i < pRule->SrNExclusions; i++)
                    if (pRule->SrExclusion[i] == typeNum)
                      /* le type demande' fait partie des exclusions */
                      ret = TRUE;
                    else
                      /* le 2eme element d'une paire est exclus si le */
                      /* 1er est exclus */
                      if (pRule->SrExclusion[i] + 1 == typeNum)
                        /* le type precedent est exclus */
                        if (pSS->SsRule->SrElem[typeNum - 1]->SrConstruct == CsPairedElement)
                          /* l'element est un membre de paire */
                          if (!pSS->SsRule->SrElem[typeNum - 1]->SrFirstOfPair)
                            /* c'est le 2eme membre de la paire */
                            ret = TRUE;
                }
            }
          /* passe a l'extension de schema suivante */
          if (pSSExt == NULL)
            pSSExt = pSSasc->SsNextExtens;
          else
            pSSExt = pSSExt->SsNextExtens;
          if (pSSExt != NULL)
            /* il y a encore un schema d'extension */
            {
              pSSasc = pSSExt;
              /* cherche dans ce schema la regle d'extension pour l'ascendant */
              pRule = ExtensionRule (pEl->ElStructSchema, pEl->ElTypeNumber, pSSExt);
            }
        }
      while (pSSExt != NULL && !ret);
      pEl = pEl->ElParent;	/* passe a l'ascendant */
    }
  return ret;
}

/*----------------------------------------------------------------------
  AllowedIncludedElem evalue si un element de type typeNum (defini 
  dans le schema de structure pSS) peut etre une inclusion dans	
  la descendance de l'element pEl.				
  Retourne "Vrai" si l'inclusion est autorisee.                   
  ----------------------------------------------------------------------*/
ThotBool AllowedIncludedElem (PtrDocument pDoc, PtrElement pEl,
                              int typeNum, PtrSSchema pSS)
{
  ThotBool            ret;
  int                 i;
  PtrSRule            pRule;
  PtrElement          pAsc;
  PtrSSchema          pSSrule, pSSExt;

  ret = FALSE;
  // template elements
  if (pSS && pSS->SsName && !strcmp (pSS->SsName, "Template"))
    return TRUE;
  else if (pSS && pEl && typeNum > MAX_BASIC_TYPE && pSS != pEl->ElStructSchema)
    return FALSE;

  if (pSS &&
      pSS->SsRule->SrElem[typeNum - 1]->SrConstruct == CsPairedElement &&
      !pSS->SsRule->SrElem[typeNum - 1]->SrFirstOfPair)
    /* c'est un element de fin de paire, on fait comme si */
    /* c'etait l'element de debut de paire */
    typeNum--;

  pAsc = pEl;
  /* examine les elements ascendants */
  while (pAsc != NULL && (!ret))
    {
      /* regle de structure de l'ascendant courant */
      pSSrule = pAsc->ElStructSchema;
      pRule = pSSrule->SsRule->SrElem[pAsc->ElTypeNumber - 1];
      pSSExt = NULL;
      do
        {
          if (pRule != NULL)
            {
              /* parcourt la liste de ses extensions */
              for (i = 0; i < pRule->SrNInclusions && (!ret); i++)
                if (pRule->SrInclusion[i] == typeNum)
                  /* l'element est du type de l'extension */
                  if (!ExcludedType (pEl, pRule->SrInclusion[i],
                                     pSSrule))
                    /* cette extension n'est pas une exclusion */
                    ret = TRUE;
            }
          /* passe a l'extension de schema suivante */
          if (pSSExt == NULL)
            pSSExt = pSSrule->SsNextExtens;
          else
            pSSExt = pSSExt->SsNextExtens;
          if (pSSExt != NULL)
            /* il y a encore un schema d'extension */
            {
              pSSrule = pSSExt;
              /* cherche dans ce schema la regle d'extension pour l'ascendant */
              pRule = ExtensionRule (pAsc->ElStructSchema, pAsc->ElTypeNumber,
                                     pSSExt);
            }
        }
      while (pSSExt != NULL && (!ret));
      /* passe a l'element ascendant */
      pAsc = pAsc->ElParent;
    }
  return ret;
}

/*----------------------------------------------------------------------
  ListOrAggregateRule
  Cherche si l'element pEl qui fait partie d'un	arbre abstrait peut etre
  obtenu a partir de la regle typeNum du schema de structure pSS, en passant
  par une regle CsList ou CsAggregate. Si oui, retourne dans pSS le pointeur
  sur le schema de structure contenant la regle CsList ou CsAggregate et
  dans typeNum le numero de cette regle.
  Si non retourne 0 dans typeNum et NULL dans pSS.		
  ----------------------------------------------------------------------*/
void ListOrAggregateRule (PtrDocument pDoc, PtrElement pEl,
                          int *typeNum, PtrSSchema * pSS)
{
  int                 c;
  ThotBool            test, equiv;
  PtrSRule            pSRule;

  equiv = FALSE;
  if (*typeNum != 0 && *pSS != NULL)
    {
      pSRule = (*pSS)->SsRule->SrElem[*typeNum - 1];
      test = TRUE;
      if (pSRule->SrRecursive)
        {
          if (pSRule->SrRecursDone)
            {
              test = FALSE;
              *typeNum = 0;
              *pSS = NULL;
            }
          else
            pSRule->SrRecursDone = TRUE;
        }
      if (test)
        switch (pSRule->SrConstruct)
          {
          case CsList:
            if (!EquivalentSRules (pSRule->SrListItem, *pSS, pEl->ElTypeNumber, pEl->ElStructSchema, pEl))
              /* ce n'est pas le type prevu des elements de la liste */
              /* c'est peut-etre une inclusion */
              if (!AllowedIncludedElem (pDoc, pEl->ElParent, pEl->ElTypeNumber, pEl->ElStructSchema))
                {
                  *typeNum = 0;
                  *pSS = NULL;
                }
            break;
          case CsUnorderedAggregate:
          case CsAggregate:
            for (c = 0; c < pSRule->SrNComponents; c++)
              {

                equiv = EquivalentSRules (pSRule->SrComponent[c], *pSS, pEl->ElTypeNumber, pEl->ElStructSchema, pEl);

                if (equiv)
                  break;
              }
            if (!equiv)
              /* ce n'est pas un composant de l'agregat, c'est
                 peut-etre une inclusion */
              equiv = AllowedIncludedElem (pDoc, pEl->ElParent, pEl->ElTypeNumber, pEl->ElStructSchema);
            if (!equiv)
              {
                *typeNum = 0;
                *pSS = NULL;
              }
            break;
          case CsIdentity:
            *typeNum = pSRule->SrIdentRule;
            ListOrAggregateRule (pDoc, pEl, typeNum, pSS);
            break;
          case CsAny:
            *typeNum = 0;
            break;
          default:
            *typeNum = 0;
            *pSS = NULL;
            break;
          }
      if (pSRule->SrRecursive && pSRule->SrRecursDone && test)
        pSRule->SrRecursDone = FALSE;
    }
}

/*----------------------------------------------------------------------
  Rend le type de constructeur d'un element
  et, dans nComp, le nombre de composants definis par le schema s'il
  s'agit d'un agregat.
  ----------------------------------------------------------------------*/
RConstruct GetElementConstruct (PtrElement pEl, int *nComp)
{
  RConstruct          constr;
  int                 typ = 0;

  constr = CsNatureSchema;
  *nComp = 0;
  if (pEl != NULL)
    {
      typ = pEl->ElTypeNumber;
      constr = pEl->ElStructSchema->SsRule->SrElem[typ - 1]->SrConstruct;
      while (constr == CsIdentity)
        {
          typ = pEl->ElStructSchema->SsRule->SrElem[typ - 1]->SrIdentRule;
          constr = pEl->ElStructSchema->SsRule->SrElem[typ - 1]->SrConstruct;
        }
    }
  if (constr == CsUnorderedAggregate || constr == CsAggregate)
    *nComp = pEl->ElStructSchema->SsRule->SrElem[typ - 1]->SrNComponents;
  return constr;
}

/*----------------------------------------------------------------------
  Teste si un element a un ancetre de type liste, mais sans       
  agregat entre l'element et cet ancetre. Rend cet ancetre ou	
  NULL si ce n'est pas le cas.					
  ----------------------------------------------------------------------*/
PtrElement AncestorList (PtrElement pEl)
{
  ThotBool            stop;
  RConstruct          constr;
  int		       nComp;

  stop = FALSE;
  if (pEl != NULL)
    {
      pEl = pEl->ElParent;
      do
        if (pEl == NULL)
          stop = TRUE;
        else
          {
            constr = GetElementConstruct (pEl, &nComp);
            if ((constr == CsAggregate || constr == CsUnorderedAggregate) &&
                /* skip aggregates that have only 1 component or that are in
                   the Template namespace */
                nComp > 1 && strcmp (pEl->ElStructSchema->SsName, "Template"))
              {
                pEl = NULL;
                stop = TRUE;
              }
            else if (constr == CsList)
              stop = TRUE;
            else
              pEl = pEl->ElParent;
          }
      while (!stop);
    }
  return pEl;
}

/*----------------------------------------------------------------------
  Teste si les cardinalites minimales et maximales de la liste	
  pEl permettent d'ajouter ou retrancher delta elements.		
  ----------------------------------------------------------------------*/
ThotBool CanChangeNumberOfElem (PtrElement pEl, int delta)
{
  PtrElement          pChild;
  int                 len;
  ThotBool            ret;
  PtrSRule            rule;
  int                 RIdent;

  ret = FALSE;
  if (pEl != NULL)
    {
      rule = pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber - 1];
      /* A CsAny rule have no cardinality, return TRUE */
      if (rule->SrConstruct == CsAny || rule->SrConstruct == CsAggregate)
        return TRUE;
      /* la regle de l'element n'est peut-etre pas une regle liste. */
      /* C'est peut-etre une identite. On cherche la regle liste */
      /* a laquelle renvoie la regle identite de l'element. */
      while (rule->SrConstruct == CsIdentity)
        {
          RIdent = rule->SrIdentRule;
          rule = pEl->ElStructSchema->SsRule->SrElem[RIdent - 1];
        }
      if (rule->SrConstruct == CsList)
        {
          /* count the number of children */
          pChild = pEl->ElFirstChild;
          /* ignore page breaks */
          for (len = 0; pChild != NULL; pChild = pChild->ElNext)
            {
              if (!pChild->ElTerminal || pChild->ElLeafType != LtPageColBreak)
                /* on ne compte que les elements du type (ou equivalent) */
                /* prevu par la liste. (Inclusion possibles) */
                if (EquivalentSRules (rule->SrListItem, pEl->ElStructSchema,
                                      pChild->ElTypeNumber, pChild->ElStructSchema, pEl))
                  len++;
            }
          ret = TRUE;
          if (delta >= 0)
            if (len + delta > rule->SrMaxItems)
              ret = FALSE;
          if (delta <= 0)
            if (len + delta < rule->SrMinItems)
              ret = FALSE;
        }
    }
  return ret;
}

/*----------------------------------------------------------------------
  Test if an element of type Any has a parent of the same type
  Return that parent or NULL				
  ----------------------------------------------------------------------*/
PtrElement      ParentAny (PtrElement pEl)
{
  int	        nComp;

  if (pEl != NULL)
    {
      if ((GetElementConstruct (pEl, &nComp) == CsAny) &&
          (pEl->ElParent != NULL) && 
          (GetElementConstruct (pEl->ElParent, &nComp) == CsAny))
        pEl = pEl->ElParent;
      else
        {
          if ((GetElementConstruct (pEl, &nComp) == CsAny) &&
              (pEl->ElParent != NULL) && 
              (GetElementConstruct (pEl->ElParent, &nComp) == CsAggregate))
            pEl = pEl->ElParent;
          else
            pEl = NULL;
        }
    }
   
  return pEl;
}

/*----------------------------------------------------------------------
  SRuleForSibling cherche le type d'element qui peut etre cree comme	
  voisin de l'element pointe par pEl. On ne tient pas compte des	
  inclusions possibles.						
  before indique s'il s'agit d'un voisin precedent (before=vrai)	
  ou suivant (before=faux).					
  distance est la distance (nombre d'elements possibles) entre	
  l'element pEl et le type d'element a creer.			
  Retourne dans typeNum le numero de la regle definissant le type	
  du voisin possible, dans pSS un pointeur sur le schema de	
  structure de ce type, et list vaut 'vrai' si le constructeur	
  qui permet le voisinage est une liste, 'faux' si c'est un	
  agregat. Au retour, optional vaut vrai si le voisin possible	
  est un composant optionnel d'agregat ou un composant d'un	
  agregat non ordonne'.						
  ----------------------------------------------------------------------*/
void SRuleForSibling (PtrDocument pDoc, PtrElement pEl,
                      ThotBool before, int distance,
                      int *typeNum, PtrSSchema *pSS,
                      ThotBool * list, ThotBool * optional)
{
  int                 i;
  PtrElement          pE;
  ThotBool            stop;
  PtrElement          pEquivEl;
  PtrSRule            pRule;
  ThotBool            reverse;

  *typeNum = 0;
  *optional = FALSE;
  pEquivEl = pEl;
  i = 0;
  if (pEl != NULL)
    /* la racine d'un arbre ne peut pas avoir de voisin */
    if (pEl->ElParent != NULL)
      {
        /* Si l'element pointe par pEl est une marque de page, il n'a pas */
        /* de voisin possible */
        if (!pEl->ElTerminal || pEl->ElLeafType != LtPageColBreak)
          /* Teste si l'element pointe par pEl est un element de liste */
          /* ou d'agregat */
          {
            // skip Template elements
            pE = pEl;
            do
              {
                pE = pE->ElParent;
               *typeNum = pE->ElTypeNumber;
                *pSS = pE->ElStructSchema;
              }
            while (pE->ElParent && *pSS && (*pSS)->SsName &&
                   !strcmp ((*pSS)->SsName, "Template"));
            ListOrAggregateRule (pDoc, pEl, typeNum, pSS);
            pEquivEl = pEl;
          }
        if (*typeNum <= 0)
          /* l'element parent n'est ni une liste ni un agregat */
          {
            /* s'il est de la forme X = TEXT, on accepte de creer une
               autre feuille de texte */
            if ((pEl->ElParent->ElStructSchema)->SsRule->SrElem[pEl->ElParent->ElTypeNumber - 1]->SrConstruct == CsIdentity)
              {
                if (pEl->ElTypeNumber == CharString + 1)
                  {
                    *pSS = pEl->ElParent->ElStructSchema;
                    *typeNum = pEl->ElTypeNumber;
                  }
              }
            else if ((pEl->ElParent->ElStructSchema)->SsRule->SrElem[pEl->ElParent->ElTypeNumber - 1]->SrConstruct == CsAny)
              /* It is defined as X = ANY.  Look in the structure schema
                 the first rule of the form X = ANY with exception
                 IsPlaceholder */
              {
                *pSS = pEl->ElParent->ElStructSchema;
                for (i = 1; i < (*pSS)->SsNRules && *typeNum <= 0; i++)
                  if ((*pSS)->SsRule->SrElem[i - 1]->SrConstruct == CsAny &&
                      TypeHasException (ExcIsPlaceholder, i, *pSS))
                    *typeNum = i;
              }
          }
        else
          /* c'est un element de liste ou d'agregat */
          if ((*pSS)->SsRule->SrElem[*typeNum - 1]->SrConstruct == CsList)
            /* c'est un element de liste */
            {
              *list = TRUE;
              if (distance > 1)
                /* on ne peut creer dans une liste que des voisins immediats */
                {
                  *typeNum = 0;
                  *pSS = NULL;
                }
              else
                /* verifie si le nombre max. d'elements est atteint */
                {
                  if (!CanChangeNumberOfElem (pEquivEl->ElParent, 1))
                    *typeNum = 0;
                  else
                    /* on va creer un sous-arbre du type des elements */
                    /* qui constituent la liste */
                    *typeNum = (*pSS)->SsRule->SrElem[*typeNum - 1]->SrListItem;
                }
            }
          else
            {
              /* on est dans un agregat */
              *list = FALSE;
              pRule = (*pSS)->SsRule->SrElem[*typeNum - 1]; /* regle de l'agregat */
              if (pRule->SrConstruct == CsUnorderedAggregate)
                /* agregat sans ordre */
                {
                  *optional = TRUE;
                  /* reponse negative a priori */
                  *typeNum = 0;
                  /* examine tous les composants declare's dans la */
                  /* regle de l'agregat */
                  i = 0;
                  while (i < pRule->SrNComponents && *typeNum == 0)
                    {
                      /* cherche si ce composant a ete cree */
                      pE = pEquivEl->ElParent->ElFirstChild;
                      stop = FALSE;
                      while (pE != NULL && !stop)
                        if (EquivalentSRules (pRule->SrComponent[i], *pSS, pE->ElTypeNumber, pE->ElStructSchema, pEquivEl->ElParent))
                          /* c'est le composant cherche' */
                          stop = TRUE;
                        else
                          /* l'element n'a pas le type du composant cherche' */
                          pE = pE->ElNext;
                      if (pE != NULL)
                        /* ce composant existe deja , essaie le suivant */
                        {
                          if (i < distance)
                            distance++;
                          i++;
                        }
                      else
                        /* ce composant n'a pas encore ete cree */
                        if (i + 1 >= distance)
                          /* c'est le 1er composant qui convient */
                          *typeNum = pRule->SrComponent[i];
                        else
                          i++;
                    }
                }
              else if (pRule->SrConstruct == CsAggregate)
                /* agregat ordonne' */
                {
                  /* cherche dans cet agregat le type de l'element */
                  /* voisin de pEl */
                  pEl = pEquivEl;
                  /* cherche d'abord le rang de l'element pEl dans l'agregat */
                  *typeNum = 0;
                  reverse = FALSE;
                  while (*typeNum == 0 && pEl != NULL)
                    {
                      i = 0;
                      do
                        if (EquivalentSRules (pRule->SrComponent[i++], *pSS, pEl->ElTypeNumber,
                                              pEl->ElStructSchema, pEl->ElParent))
                          /* trouve' */
                          *typeNum = i;

                      while (*typeNum == 0 && i < pRule->SrNComponents) ;

                      if (*typeNum == 0)
                        {
                          /* l'element n'est pas un des composants de
                             l'agregat, c'est sans doute une inclusion */
                          if (AllowedIncludedElem (pDoc, pEl->ElParent, pEl->ElTypeNumber, pEl->ElStructSchema))
                            /* c'est une inclusion, on essaie avec
                               l'element voisin */
                            {
                              if (before)
                                /* cherche le voisin possible de
                                   l'element qui suit l'element inclusion */
                                if (pEl->ElNext != NULL)
                                  pEl = pEl->ElNext;
                                else
                                  /* pas de suivant, on change de sens, mais une */
                                  /* seule fois */
                                  if (reverse)
                                    pEl = NULL;	/* on a deja change' de sens */
                                  else
                                    {
                                      pEl = pEl->ElPrevious;
                                      reverse = TRUE;
                                      before = !before;
                                    }
                              else
                                /* cherche le voisin possible de l'element */
                                /* qui precede l'element inclusion */
                                if (pEl->ElPrevious != NULL)
                                  pEl = pEl->ElPrevious;
                                else
                                  /* pas de precedent, on change de sens,
                                     mais une seule fois */
                                  if (reverse)
                                    /* on a deja change' de sens */
                                    pEl = NULL;
                                  else
                                    {
                                      pEl = pEl->ElNext;
                                      reverse = TRUE;
                                      before = !before;
                                    }
                            }
                          else
                            /* ce n'est pas une inclusion, on arrete */
                            pEl = NULL;
                        }
                    }

                  if (*typeNum != 0)
                    {
                      /* trouve' */
                      if (before)
                        {
                          /* on cherche a creer avant */
                          if (i <= distance)
                            *typeNum = 0;
                          else
                            /* type de l'element possible */
                            {
                              *optional = pRule->SrOptComponent[*typeNum - distance - 1];
                              *typeNum = pRule->SrComponent[*typeNum - distance - 1];
                              /* cherche si ce type d'element existe deja */
                              pE = pEl->ElPrevious;
                              while (pE != NULL && *typeNum != 0)
                                {
                                  if (EquivalentSRules (*typeNum, *pSS, pE->ElTypeNumber, pE->ElStructSchema, pEl->ElParent))
                                    /* il existe deja */
                                    {
                                      *optional = FALSE;
                                      *typeNum = 0;
                                    }
                                  pE = pE->ElPrevious;
                                }
                            }
                        }
                      else if (*typeNum + distance > pRule->SrNComponents)
                        /* on cherche a creer apres */
                        *typeNum = 0;
                      else
                        /* type de l'element possible */
                        {
                          *optional = pRule->SrOptComponent[*typeNum + distance - 1];
                          *typeNum = pRule->SrComponent[*typeNum + distance - 1];
                          /* cherche si ce type d'element existe deja */
                          pE = pEl->ElNext;
                          while (pE != NULL && *typeNum != 0)
                            {
                              if (EquivalentSRules (*typeNum, *pSS, pE->ElTypeNumber, pE->ElStructSchema, pEl->ElParent))
                                {
                                  *optional = FALSE;
                                  *typeNum = 0;		/* il existe deja */
                                }
                              pE = pE->ElNext;
                            }
                        }
                    }
                }
            }
      }
  if (*typeNum == 0)
    *pSS = NULL;
}

/*----------------------------------------------------------------------
  ReferredType   pRefEl est un pointeur sur un element reference.	
  pRefAttr est un pointeur sur un attribut reference. L'un des	
  deux pointeurs est NULL. La procedure retourne dans typeNum le	
  numero du type et dans pSS un pointeur sur le schema de		
  structure qui definit le type des elements reference's par	
  pRefEl ou pRefAttr.						
  Retourne typeNum = 0 et pSS = NULL s'il s'agit d'une reference	
  non typee : CsReference(Any)					
  ----------------------------------------------------------------------*/
void ReferredType (PtrElement pRefEl, PtrAttribute pRefAttr,
                   PtrSSchema * pSS, int *typeNum,
                   PtrDocument pDoc)
{
  int                 referredNature;
  PtrSRule            pRule;
  PtrTtAttribute      pAtt;

  if (pRefEl != NULL)
    {
      /* c'est un element reference */
      if (pRefEl->ElSource != NULL)
        /* c'est une reference inclusion */
        {
          *pSS = pRefEl->ElStructSchema;
          *typeNum = pRefEl->ElTypeNumber;
        }
      else
        {
          /* la regle definissant la reference */
          pRule = pRefEl->ElStructSchema->SsRule->SrElem[pRefEl->ElTypeNumber - 1];
          /* cherche le type de l'element reference' prevu par le schema */
          /* de structure */
          *typeNum = pRule->SrReferredType;
          /* schema de structure ou est definie la reference */
          if (*typeNum == 0)
            *pSS = NULL;
          else
            *pSS = pRefEl->ElStructSchema;
          if (pRule->SrRefTypeNat[0] != EOS)
            /* le type reference' est defini dans un autre schema de */
            /* structure */
            /* cherche, ou charge si ce n'est pas deja fait, le schema de */
            /* structure qui definit le type reference' */
            {
              /* pas de schema de presentation prefere' */
              referredNature = CreateNature (NULL, pRule->SrRefTypeNat, NULL,
                                             *pSS, pDoc);
              if (referredNature == 0)
                *pSS = NULL;
              else
                {
                  /* pointeur sur le schema de structure qui definit le */
                  /* type d'element reference' */
                  *pSS = (*pSS)->SsRule->SrElem[referredNature - 1]->SrSSchemaNat;
                  AddSchemaGuestViews (pDoc, *pSS);
                }
            }
        }
    }
  if (pRefAttr != NULL)
    /* c'est un attribut reference */
    {
      /* regle de definition de l'attribut reference */
      pAtt = pRefAttr->AeAttrSSchema->SsAttribute->TtAttr[pRefAttr->AeAttrNum - 1];
      /* cherche le type de l'element reference' prevu par le schema */
      /* de structure */
      *typeNum = pAtt->AttrTypeRef;
      if (*typeNum == 0)
        *pSS = NULL;
      else
        /* schema de structure qui definit l'attribut */
        *pSS = pRefAttr->AeAttrSSchema;

      if (pAtt->AttrTypeRefNature[0] != EOS)
        /* le type reference' est defini dans un autre schema de */
        /* structure */
        /* cherche, ou charge si ce n'est pas deja fait, le schema de */
        /* structure qui definit le type reference' */
        {
          /* pas de schema de presentation prefere' */
          referredNature = CreateNature (NULL, pAtt->AttrTypeRefNature,
                                         NULL, *pSS, pDoc);
          if (referredNature == 0)
            *pSS = NULL;
          else
            {
              /* pointeur sur le schema de structure qui definit le type */
              /* d'element reference' par l'attribut */
              *pSS = (*pSS)->SsRule->SrElem[referredNature - 1]->SrSSchemaNat;
              AddSchemaGuestViews (pDoc, *pSS);
            }
        }
    }
}

/*----------------------------------------------------------------------
  CanCutElement       indique si on peut couper l'element pEl.		
  On considere l'exception NoCut et les composants obligatoires	
  d'agregats et le nombre minimum d'elements des listes.		
  pElCut sur le premier des elements deja coupes			
  ----------------------------------------------------------------------*/
ThotBool CanCutElement (PtrElement pEl, PtrDocument pDoc, PtrElement pElCut)
{
  int                 typeNum;
  PtrSSchema          pSS;
  PtrSRule            pRule;
  int                 i;
  int                 view;
  PtrElement          viewRoot;
  ThotBool            ret, InCutBuffer;

  /* a priori, on peut couper l'element */
  ret = TRUE;
  InCutBuffer = FALSE;
  if (pEl != NULL)
    {
      if (pEl->ElTypeNumber == pEl->ElStructSchema->SsDocument)
        /* do not delete the root element */
        ret = FALSE;
      else  if (pEl->ElTypeNumber == pEl->ElStructSchema->SsRootElem &&
                pEl->ElParent &&
                pEl->ElParent->ElTypeNumber == pEl->ElParent->ElStructSchema->SsDocument)
        /* do not delete the root element */
        ret = FALSE;
      else if (TypeHasException (ExcNoCut, pEl->ElTypeNumber,
                                 pEl->ElStructSchema))
        /* l'exception NoCut est associee au type de l'element */
        ret = FALSE;
      else if (pDoc->DocCheckingMode & COMPLETE_CHECK_MASK)
        /*else if (FullStructureChecking)*/
        /* on est en mode de controle strict de la structure */
        if (pEl->ElParent != NULL)
          {
            /* teste si l'element pointe par pEl est un element de liste */
            /* ou d'agregat */
            typeNum = pEl->ElParent->ElTypeNumber;
            pSS = pEl->ElParent->ElStructSchema;
            ListOrAggregateRule (pDoc, pEl, &typeNum, &pSS);
            if (typeNum > 0)
              /* c'est un element de liste ou d'agregat */
              {
                /* SRule qui definit la liste ou l'agregat */
                pRule = pSS->SsRule->SrElem[typeNum - 1];
                if (pRule->SrConstruct == CsList)
                  /* c'est un element de liste */
                  {
                    if (pRule->SrMinItems > 0)
                      /* il y a un nombre minmum d'elements a respecter */
                      ret = CanChangeNumberOfElem (pEl->ElParent, -1);
                  }
                else
                  /* c'est un element d'agregat */
                  {
                    /* cherche le rang de ce composant dans l'agregat */
                    i = 0;
                    while (i < pRule->SrNComponents)
                      if (pRule->SrComponent[i] == pEl->ElTypeNumber)
                        {
                          /* on ne peut couper que les composants optionnels */
                          ret = pRule->SrOptComponent[i];
                          /* a non-optional component can be cut if it has
                             exception ExcCanCut */
                          if (!ret)
                            ret = TypeHasException (ExcCanCut,
                                                    pRule->SrComponent[i], pSS);
                          i = pRule->SrNComponents;
                        }
                      else
                        i++;
                  }
              }
          }
    }
  if (ret)
    {
      /* est-ce la racine d'un sous-arbre d'affichage */
      /* parcourt toutes les vues ouvertes jusqu'a en trouver une */
      /* dont l'element est la racine */
      for (view = 0; view < MAX_VIEW_DOC && ret; view++)
        if (pDoc->DocView[view].DvPSchemaView != 0)
          /* vue ouverte */
          {
            viewRoot = pDoc->DocViewSubTree[view];
            ret = (viewRoot == NULL ||
                   (pEl != viewRoot && !ElemIsAnAncestor (pEl, viewRoot)));
          }
    }
  return ret;
}

/*----------------------------------------------------------------------
  AllowedSibling  retourne vrai si on peut inserer un element de  
  type typeNum (defini dans le schema de structure pSS) comme	
  frere de l'element pEl qui appartient au document pDoc.		
  before indique s'il s'agit d'un frere precedent ou suivant.	
  user indique si l'insertion est demandee par l'utilisateur.	
  inTree indique si l'element est deja en place dans l'arbre.	
  ----------------------------------------------------------------------*/
ThotBool AllowedSibling (PtrElement pEl, PtrDocument pDoc, int typeNum, PtrSSchema pSS,
                         ThotBool before, ThotBool user,
                         ThotBool inTree)
{
  PtrSSchema          pAscSS;
  PtrSRule            pRule;
  PtrElement          pEl1, pAsc;
  ThotBool            ok;
  ThotBool            isPageBrOrIncl;
  ThotBool            optional;
  ThotBool            stop;
  int                 ascTypeNum;
  int                 compNum;
  int                 i;
  ThotBool            beforeElement;

  beforeElement = before;
  ok = FALSE;
  if (pEl != NULL)
    /* on ne peut rien inserer a cote' de la racine d'un arbre */
    if (pEl->ElParent != NULL)
      {
        /* on peut inserer une marque de page n'importe ou` */
        /* OK si c'est une inclusion pour l'un des ascendants */
        isPageBrOrIncl = (typeNum == PageBreak + 1 ||
                          AllowedIncludedElem (pDoc, pEl->ElParent, typeNum, pSS));
        if ( isPageBrOrIncl || AllowedIncludedElem (pDoc, pEl->ElParent, pEl->ElTypeNumber, pEl->ElStructSchema)) 
          /* dans le cas d'une inclusion ou d'une marque de page */
          /* on ne peut quand meme pas inserer comme descendant direct */
          /* d'un noeud CsChoice ou CsIdentity' */
          {
            ok = isPageBrOrIncl; 
            pRule = pEl->ElParent->ElStructSchema->SsRule->SrElem[pEl->ElParent->ElTypeNumber - 1];
            /* on ne fait pas cette verification si l'element y est deja */
            if (!inTree)
              {
                if (pRule->SrConstruct == CsChoice)
                  ok = FALSE;
                else if (pRule->SrConstruct == CsIdentity &&
                         EquivalentSRules (pEl->ElParent->ElTypeNumber,
                                           pEl->ElParent->ElStructSchema, 
                                           pEl->ElTypeNumber, pEl->ElStructSchema,
                                           pEl))
                  /* le fils et le pere sont de type equivalent, refus */
                  ok = FALSE;
              }
          }
        if(!isPageBrOrIncl)
          {
            if ((pEl->ElTerminal && pEl->ElLeafType == LtPageColBreak) ||
                AllowedIncludedElem (pDoc, pEl->ElParent, pEl->ElTypeNumber, pEl->ElStructSchema)) 
              /* on veut inserer a cote' d'une marque de page ou d'une inclusion. */
              {
                pEl1 = pEl;
                stop = FALSE;
                /* si on insere avant, on cherche le premier suivant */
                /* qui n'est pas une marque de page, sinon le precedent */
                while (!stop)
                  if ((!pEl->ElTerminal || pEl->ElLeafType != LtPageColBreak) &&
                      !AllowedIncludedElem (pDoc, pEl->ElParent, pEl->ElTypeNumber, pEl->ElStructSchema))
                    /* ce n'est pas une page, ni une inclusion, on a trouve' */
                    stop = TRUE;
                  else
                    /* c'est une page ou une inclusion, on continue */
                    {
                      if (before)
                        pEl = pEl->ElNext;
                      else
                        pEl = pEl->ElPrevious;
                      if (pEl == NULL)
                        stop = TRUE;
                    }
                if (pEl == NULL && !inTree)
                  /* pas encore trouve', on cherche dans l'autre sens */
                  /* si l'element n'est pas deja a sa place */
                  {
                    pEl = pEl1;
                    stop = FALSE;
                    while (!stop)
                      if ((!pEl->ElTerminal || pEl->ElLeafType != LtPageColBreak) &&
                          !AllowedIncludedElem (pDoc, pEl->ElParent, pEl->ElTypeNumber, pEl->ElStructSchema)) 
                        /* ce n'est pas une page ni une inclusion, on a trouve' */
                        stop = TRUE;
                      else
                        /* c'est une page ou une inclusion , on continue */
                        {
                          if (before)
                            pEl = pEl->ElPrevious;
                          else
                            pEl = pEl->ElNext;
                          if (pEl == NULL)
                            stop = TRUE;
                        }
                    before = !before;
                  }
                if (pEl == NULL)
                  /* il n'y a que des sauts de page et des inclusions, c'est comme si */
                  /* l'element etait vide */
                  ok = AllowedFirstChild (pEl1->ElParent, pDoc, typeNum, pSS,
                                          user, inTree);
              }
            if (pEl != NULL)
              {
                /* Teste si l'element pointe par pEl est un element de */
                /* liste ou d'agregat */
                pAsc = pEl->ElParent;
                ascTypeNum = pAsc->ElTypeNumber;
                pAscSS = pAsc->ElStructSchema;
                while (pAscSS && pAsc && !strcmp (pAscSS->SsName, "Template"))
                  {
                    pAsc = pAsc->ElParent;
                    if (pAsc)
                      {
                        ascTypeNum = pAsc->ElTypeNumber;
                        pAscSS = pAsc->ElStructSchema;
                      }
                  }
                if (pAscSS->SsRule->SrElem[ascTypeNum - 1]->SrConstruct == CsAny)
                  /* the parent element does not put any constraint on its
                     content */
                  ok = TRUE;
                else
                  {
                    ListOrAggregateRule (pDoc, pEl, &ascTypeNum, &pAscSS);
                    if (ascTypeNum > 0)
                      /* c'est un element de liste ou d'agregat */
                      {
                        pRule = pAscSS->SsRule->SrElem[ascTypeNum - 1];
                        if (pRule->SrConstruct == CsList)
                          /* c'est un element de liste */
                          {
                            ok = TRUE;
                            if (!inTree &&
                                /*  verifie qu'on peut ajouter un element a la liste */
                                !CanChangeNumberOfElem (pEl->ElParent, 1))
                              /* liste pleine */
                              ok = FALSE;
                            /* verifie le type des elements de liste */
                            if (ok)
                              ok = EquivalentSRules (pRule->SrListItem, pAscSS, typeNum,
                                                     pSS, pEl->ElParent);
                          }
                        else
                          /* c'est un element d'agregat */
                          {
                            if (pRule->SrConstruct == CsUnorderedAggregate)
                              /* examine tous les composants declare's dans la regle */
                              for (i = 0; i < pRule->SrNComponents && !ok; i++)
                                if (pRule->SrComponent[i] == typeNum &&
                                    !strcmp (pAscSS->SsName, pSS->SsName))
                                  {
                                    ok = TRUE;
                                    compNum = i + 1;
                                  }
                            if (pRule->SrConstruct == CsAggregate)
                              /* agregat ordonne' */
                              {
                                /* cherche le rang compNum de l'element pEl dans l'agregat */
                                compNum = 0;
                                for (i = 0; i < pRule->SrNComponents && compNum == 0; i++)
                                  if (pRule->SrComponent[i] == pEl->ElTypeNumber &&
                                      !strcmp (pAscSS->SsName, pEl->ElStructSchema->SsName))
                                    compNum = i + 1;
                                if (compNum == 0)
                                  /* l'element pEl n'est pas un composant prevu de */
                                  /* l'agregat (c'est sans doute une inclusion) */
                                  {
                                    pEl1 = pEl;
                                    stop = FALSE;
                                    /* on cherche le frere le plus proche qui ne soit */
                                    /* ni une marque de page ni une inclusion */
                                    while (!stop)
                                      if (!(pEl->ElTerminal && pEl->ElLeafType == LtPageColBreak) && !AllowedIncludedElem (pDoc, pEl->ElParent, pEl->ElTypeNumber, pEl->ElStructSchema))
                                        /* ce n'est ni une page ni une inclusion, on a trouve' */
                                        stop = TRUE;
                                      else
                                        /* on continue */
                                        {
                                          if (before)
                                            pEl = pEl->ElNext;
                                          else
                                            pEl = pEl->ElPrevious;
                                          if (pEl == NULL)
                                            stop = TRUE;
                                        }
                                    if (pEl == NULL && !inTree)
                                      /* pas encore trouve', on cherche dans l'autre */
                                      /* sens si l'element n'est pas deja a sa place */
                                      {
                                        pEl = pEl1;
                                        stop = FALSE;
                                        while (!stop)
                                          if (!(pEl->ElTerminal && pEl->ElLeafType == LtPageColBreak) && !AllowedIncludedElem (pDoc, pEl->ElParent, pEl->ElTypeNumber, pEl->ElStructSchema))
                                            /* ce n'est ni une page ni une inclusion, on a trouve' */
                                            stop = TRUE;
                                          else
                                            /* on continue */
                                            {
                                              if (before)
                                                pEl = pEl->ElPrevious;
                                              else
                                                pEl = pEl->ElNext;
                                              if (pEl == NULL)
                                                stop = TRUE;
                                            }
                                        before = !before;
                                      }
                                    if (pEl == NULL)
                                      /* il n'y a que des sauts de page et des */
                                      /* inclusions, c'est comme si le pere etait vide */
                                      ok = AllowedFirstChild (pEl1->ElParent, pDoc, typeNum,
                                                              pSS, user, inTree);
                                    else
                                      /* on a trouve un frere qui n'est ni un saut de */
                                      /* page ni une inclusion. On cherche son */
                                      /* rang dans la regle qui definit l'agregat */
                                      for (i = 0; i < pRule->SrNComponents && compNum == 0; i++)
                                        if (pRule->SrComponent[i] == pEl->ElTypeNumber &&
                                            !strcmp (pAscSS->SsName, pEl->ElStructSchema->SsName))
                                          compNum = i + 1;
                                  }
                                if (pEl != NULL)
                                  {
                                    /* tous les composants sont optionnels si on n'est */
                                    /* pas en mode de controle strict */
                                    optional = TRUE;
                                    if (before && compNum > 1)
                                      /* pas de composant avant le premier */
                                      for (i = compNum - 1; i >= 1 && optional && !ok; i--)
                                        {
                                          if (pRule->SrComponent[i-1] == typeNum &&
                                              !strcmp (pAscSS->SsName, pSS->SsName))
                                            ok = TRUE;
                                          else
                                            optional = (pRule->SrOptComponent[i - 1] ||
                                                        !(pDoc->DocCheckingMode & COMPLETE_CHECK_MASK)
                                                        /*!FullStructureChecking*/);
                                        }
                                    else if (compNum < pRule->SrNComponents)
                                      /* on veut inserer apres pEl */
                                      {
                                        /* pas de composant apres le dernier */
                                        for (i = compNum + 1; i <= pRule->SrNComponents &&
                                               optional && !ok; i++)
                                          {
                                            if (pRule->SrComponent[i-1] == typeNum &&
                                                !strcmp (pAscSS->SsName, pSS->SsName))
                                              ok = TRUE;
                                            else
                                              optional = (pRule->SrOptComponent[i - 1] ||
                                                          !(pDoc->DocCheckingMode & COMPLETE_CHECK_MASK)
                                                          /*!FullStructureChecking*/);
                                          }
                                      }
                                  }
                              }
                            /* verifie si ce composant existe deja */
                            if (ok && !inTree && pEl != NULL)
                              {
                                pEl1 = pEl->ElParent->ElFirstChild;
                                while (pEl1 != NULL && ok)
                                  if (typeNum == pEl1->ElTypeNumber)
                                    /* il existe deja */
                                    ok = FALSE;
                                  else
                                    pEl1 = pEl1->ElNext;
                              }
                          }
                      }
                    if (!ok && pEl != NULL &&
                        typeNum == CharString + 1 &&
                        pEl->ElTypeNumber == CharString + 1)
                      /* on veut inserer un element Texte a cote' d'un autre */
                      /* element texte, OK */
                      ok = TRUE;
                  }
              }
          }
        if (!ok)
          {
            /* si l'element a verifier porte une exception ExcPageBreakRepetition */
            /* ExcPageBreakRepBefore, on l'autorise quand meme */
            if (TypeHasException (ExcPageBreakRepBefore, typeNum, pSS))
              ok = TRUE;
            else if (TypeHasException (ExcPageBreakRepetition, typeNum, pSS))
              ok = TRUE;
          }
        if (pEl != NULL)
          {
            if (!ok)
              {
                /* si l'element voisin porte une exception ExcPageBreakRepetition */
                /* ExcPageBreakRepBefore, on l'autorise quand meme */
                if (TypeHasException (ExcPageBreakRepBefore, pEl->ElTypeNumber, pEl->ElStructSchema))
                  ok = TRUE;
                else if (TypeHasException (ExcPageBreakRepetition, pEl->ElTypeNumber, pEl->ElStructSchema))
                  ok = TRUE;
              }
            if (ok &&
                /* refus si c'est une exclusion pour l'un des ascendants */
                ExcludedType (pEl->ElParent, typeNum, pSS))
              ok = FALSE;
            if (ok && user)
              {
                /* l'insertion est demande'e par l'utilisateur */
                if (CannotInsertNearElement (pEl, beforeElement))
                  ok = FALSE;
              }
          }
      }
  return ok;
}

/*----------------------------------------------------------------------
  AllowedFirstComponent      retourne vrai si on peut inserer un     
  element de type compTypeNum (defini dans le schema de structure	
  pCompSS) comme premier fils dans l'agregat defini par la regle	
  de numero aggrTypeNum du schema de structure pCompSS.		
  ----------------------------------------------------------------------*/
ThotBool AllowedFirstComponent (int aggrTypeNum, PtrSSchema pAggrSS,
                                int compTypeNum, PtrSSchema pCompSS)
{
  int                 i;
  PtrSRule            pRule;
  ThotBool            ok, optional;

  ok = FALSE;
  optional = FALSE;
  pRule = pAggrSS->SsRule->SrElem[aggrTypeNum - 1];
  switch (pRule->SrConstruct)
    {
    case CsAggregate:
      /* on regarde si le type de l'element a inserer est celui */
      /* des premiers composants optionnels de l'agregat jusqu'au */
      /* premier composant obligatoire */
      if (pRule->SrNComponents > 0)
        {
          i = 0;
          do
            if (pAggrSS->SsRule->SrElem[pRule->SrComponent[i] - 1]->SrConstruct == CsChoice && pAggrSS->SsRule->SrElem[pRule->SrComponent[i] - 1]->SrNChoices == -1)
              /* le composant pre'vu est NATURE */
              {
                if (compTypeNum == pCompSS->SsRootElem)
                  /* l'element a inserer est la racine de son */
                  /* schema */
                  ok = TRUE;
              }
            else if (pRule->SrComponent[i] == compTypeNum &&
                     !strcmp (pAggrSS->SsName, pCompSS->SsName))
              ok = TRUE;
            else
              {
                optional = pRule->SrOptComponent[i];
                if (!optional)
                  if (!FullStructureChecking)
                    /* on n'est pas en mode de controle strict de
                       la structure: tous les voisins sont optionnels */
                    optional = TRUE;
                i++;
              }
          while (i < pRule->SrNComponents && !ok && optional);
        }
      break;
    case CsUnorderedAggregate:
      /* on regarde si le type de l'element a inserer est parmi */
      /* les types des composants de l'agregat */
      for (i = 0; i < pRule->SrNComponents && !ok; i++)
        if (pAggrSS->SsRule->SrElem[pRule->SrComponent[i] - 1]->SrConstruct == CsChoice
            && pAggrSS->SsRule->SrElem[pRule->SrComponent[i] - 1]->SrNChoices == -1)
          /* le composant pre'vu est NATURE */
          {
            if (compTypeNum == pCompSS->SsRootElem)
              /* l'element a inserer est la racine de son schema */
              ok = TRUE;
          }
        else if (pRule->SrComponent[i] == compTypeNum &&
                 !strcmp (pAggrSS->SsName, pCompSS->SsName))
          ok = TRUE;
      break;
    default:
      break;
    }
  return ok;
}

/*----------------------------------------------------------------------
  AllowedFirstChild     retourne vrai si on peut inserer un     
  element de type typeNum (defini dans le schema de structure pSS)
  comme premier fils de l'element pEl qui appartient au document  
  pDoc.                                                           
  user indique si l'insertion est demandee par l'utilisateur.     
  inTree indique si l'element est deja en place dans l'arbre.	
  ----------------------------------------------------------------------*/
ThotBool AllowedFirstChild (PtrElement pEl, PtrDocument pDoc, int typeNum,
                            PtrSSchema pSS, ThotBool user, ThotBool inTree)
{
  PtrSRule            pRule1, pRule2;
  PtrElement          pAsc, pChild;
  int                 t;
  int                 i;
  ThotBool            ok, empty;
  ThotBool            stop;

  ok = FALSE;
  if (pEl != NULL)
    /* on ne peut rien inserer dans un element terminal */
    if (!pEl->ElTerminal)
      {
        if (inTree)
          /* si le fils est deja dans l'arbre, on fait comme si son pere */
          /* etait vide */
          empty = TRUE;
        else if (pEl->ElFirstChild == NULL)
          empty = TRUE;
        else
          /* l'element a deja au moins un fils */
          {
            pChild = pEl->ElFirstChild;
            /* on saute les marques de pages et les inclusions */
            stop = FALSE;
            do
              {
                if (pChild == NULL)
                  /* on a saute' tous les fils */
                  stop = TRUE;
                else if (pChild->ElTerminal && pChild->ElLeafType == LtPageColBreak)
                  /* ce fils est une marque de page, on le saute */
                  pChild = pChild->ElNext;
                else if (AllowedIncludedElem (pDoc, pEl, pChild->ElTypeNumber, pChild->ElStructSchema))
                  /* ce fils est une inclusion, on le saute */
                  pChild = pChild->ElNext;
                else
                  /* ce fils n'est ni un saut de page ni une inclusion */
                  /* on le garde */
                  stop = TRUE;
              }
            while (!stop);
            if (pChild == NULL)
              /* l'element ne contient que des marques de page, il est vide */
              empty = TRUE;
            else
              {
                empty = FALSE;
                ok = AllowedSibling (pChild, pDoc, typeNum, pSS, TRUE, user, inTree);
              }
          }
        if (empty &&
            pEl->ElTypeNumber > 0 && pEl->ElTypeNumber <= pEl->ElStructSchema->SsNRules)
          {
            /* on peut inserer une marque de page n'importe ou` */
            if (typeNum == PageBreak + 1)
              ok = TRUE;
            /* OK si c'est une inclusion pour l'un des ascendants */
            else if (AllowedIncludedElem (pDoc, pEl, typeNum, pSS))
              ok = TRUE;
            pRule1 = pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber - 1];
            if (ok)
              /* dans le cas d'une inclusion ou d'une marque de page */
              /* on ne peut quand meme pas inserer comme descendant */
              /* direct d'un noeud CsChoice */
              {
                if (!inTree)
                  if (pRule1->SrConstruct == CsChoice)
                    ok = FALSE;
              }
            else
              {
                /* traitement selon le constructeur du type de l'element */
                switch (pRule1->SrConstruct)
                  {
                  case CsChoice:
                    if (pRule1->SrNChoices == -1)
                      /* c'est une regle NATURE */
                      {
                        if (typeNum == pSS->SsRootElem)
                          /* on veut inserer un element construit selon la */
                          /* regle racine de son schema de structure, OK */
                          ok = TRUE;
                      }
                    else if (pRule1->SrNChoices == 0)
                      /* c'est une regle UNIT */
                      if (typeNum <= MAX_BASIC_TYPE)
                        /* on veut inserer un element de base, OK */
                        ok = TRUE;
                      else
                        {
                          if (pSS->SsRule->SrElem[typeNum - 1]->SrUnitElem)
                            /* l'element a inserer est defini comme une unite' */
                            /* dans son schema. On cherche si ce schema est */
                            /* celui d'un des ascendants ou une extension */
                            {
                              pAsc = pEl;
                              while (pAsc != NULL && !ok)
                                if (!strcmp (pAsc->ElStructSchema->SsName,
                                             pSS->SsName))
                                  ok = TRUE;
                                else if (ValidExtension (pAsc, &pSS))
                                  ok = TRUE;
                                else
                                  pAsc = pAsc->ElParent;
                            }
                        }
                    else
                      /* c'est un choix avec indication des types possibles */
                      /* on regarde si le type a inserer est parmi les types */
                      /* possibles de ce choix */
                      {
                        i = 0;
                        while (!ok && i < pRule1->SrNChoices)
                          {
                            ok = EquivalentSRules (pRule1->SrChoice[i], pEl->ElStructSchema,
                                                   typeNum, pSS, pEl);
                            i++;
                          }
                      }
                    break;
                  case CsAny:
                    ok = TRUE;
                    break;
                  case CsAggregate:
                  case CsUnorderedAggregate:
                    ok = AllowedFirstComponent (pEl->ElTypeNumber, pEl->ElStructSchema,
                                                typeNum, pSS);
                    break;
                  case CsList:
                    /* on regarde si le type de l'element a inserer est */
                    /* equivalent au type des elements de la liste */
                    ok = EquivalentSRules (pRule1->SrListItem, pEl->ElStructSchema,
                                           typeNum, pSS, pEl->ElParent);
                    break;
                  case CsNatureSchema:
                    /* le type a inserer doit etre la racine de la nature */
                    if (typeNum == pSS->SsRootElem &&
                        pRule1->SrSSchemaNat != NULL &&
                        !strcmp (pRule1->SrSSchemaNat->SsName, pSS->SsName))
                      ok = TRUE;
                    break;
                  case CsIdentity:
                    /* on verifie d'abord si les types sont equivalents */
                    ok = EquivalentSRules (pRule1->SrIdentRule, pEl->ElStructSchema,
                                           typeNum, pSS, pEl);
                    if (!ok)
                      /* les types ne sont pas directement equivalents */
                      /* peut-on passer par une regle CsList ou CsAggregate ? */
                      {
                        /* suit la chaine des identite's jusqu'au dernier */
                        /* type identique au type de pEl */
                        pRule2 = pRule1;
                        do
                          {
                            t = pRule2->SrIdentRule;
                            pRule2 = pEl->ElStructSchema->SsRule->SrElem[t - 1];
                          }
                        while (pRule2->SrConstruct == CsIdentity);
                        /* s'il s'agit d'une liste, le type a inserer doit */
                        /* etre equivalent aux elements de la liste */
                        if (pRule2->SrConstruct == CsList)
                          ok = EquivalentSRules (pRule2->SrListItem, pEl->ElStructSchema,
                                                 typeNum, pSS, pEl);
                        else if (pRule2->SrConstruct == CsAggregate ||
                                 pRule2->SrConstruct == CsUnorderedAggregate)
                          /* c'est une regle CsAggregate */
                          ok = AllowedFirstComponent (t, pEl->ElStructSchema, typeNum,
                                                      pSS);
                      }
                    break;
                  case CsBasicElement:
                  case CsReference:
                  case CsConstant:
                  case CsPairedElement:
                    /* pas de descendance possible */
                    break;
                  default:
                    break;
                  }
              }
            if (ok && ExcludedType (pEl, typeNum, pSS))
              ok = FALSE;
            if (ok && user)
              {
                /* l'insertion est demande'e par l'utilisateur */
                if (ElementIsReadOnly (pEl))
                  ok = FALSE;
                else if (TypeHasException (ExcNoCreate, typeNum, pSS))
                  ok = FALSE;
              }
          }
      }
  return ok;
}

/*----------------------------------------------------------------------
  InsertChildFirst   Insere dans l'arbre abstrait l'element pChild	
  comme premier fils de l'element pEl.				
  ----------------------------------------------------------------------*/
void InsertChildFirst (PtrElement pEl, PtrElement pChild,
                       PtrElement *pLeaf, PtrDocument pDoc)
{
  ThotBool            same;

  if (pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber - 1]->SrConstruct == CsChoice)
    {
      /* verifie si pChild est la feuille qu'on doit retourner */
      same = pChild == *pLeaf;
      InsertElemInChoice (pEl, &pChild, pDoc, FALSE);
      if (same)
        /* InsertElemInChoice peut avoir modifie' pChild. La feuille qu'on */
        /* retournera doit rester l'element pChild */
        *pLeaf = pChild;
    }
  else
    InsertFirstChild (pEl, pChild);
}

/*----------------------------------------------------------------------
  CreateDescendant  tries to create a child of the pParent element
  according to the typeNum rule of the pSS schema definition until an
  element of the descTypeNum in pDescSS is created.

  Cherche a creer, pour un element defini par la	
  regle typeNum du schema de structure pSS, une descendance	
  jusqu'a un element de type descTypeNum defini dans le schema de	
  structure pDescSS.						
  Retourne le pointeur le premier element de la descendance creee,
  ou NULL s'il n'est pas possible de creer une telle descendance.	
  pDoc: Pointeur sur le descripteur du document concerne.		
  pLeaf: si succes, pointeur sur l'element de plus bas niveau	
  cree: c'est un element de type descTypeNum.			
  ----------------------------------------------------------------------*/
PtrElement CreateDescendant (int typeNum, PtrSSchema pSS,
                             PtrDocument pDoc, PtrElement *pLeaf,
                             int descTypeNum, PtrSSchema pDescSS, PtrElement pParent)
{
  PtrElement          pEl, pDesc, pEl1, pEl2;
  int                 i, j;
  Name                N;
  ThotBool            stop;
  PtrSRule            pRule1, pRule2;

  if (SameSRules (typeNum, pSS, descTypeNum, pDescSS))
    /* c'est un element du type voulu, on le cree */
    {
      pEl = NewSubtree (descTypeNum, pDescSS, pDoc, FALSE, TRUE, TRUE, TRUE);
      *pLeaf = pEl;
    }
  else
    {
      pRule1 = pSS->SsRule->SrElem[typeNum - 1];
      pEl = NULL;
      stop = FALSE;
      if (pRule1->SrRecursive)
        {
          /* SRule recursive */
          if (pRule1->SrRecursDone)
            /* on est deja passe' sur cette regle, on arrete */
            stop = TRUE;
          else
            /* on marque qu'on est passe' sur cette regle */
            pRule1->SrRecursDone = TRUE;
        }
      if (!stop)
        switch (pRule1->SrConstruct)
          {
          case CsBasicElement:
          case CsReference:
          case CsConstant:
          case CsPairedElement:
          case CsEmpty:
            /* regles terminales */
            break;
          case CsNatureSchema:
            N[0] = EOS; /* pas de schema de presentation prefere'*/
            LoadNatureSchema (pSS, N, typeNum, NULL, pDoc);
            AddSchemaGuestViews (pDoc, pRule1->SrSSchemaNat);
            if (pRule1->SrSSchemaNat != NULL)
              {
                pEl = CreateDescendant (pRule1->SrSSchemaNat->SsRootElem,
                                        pRule1->SrSSchemaNat, pDoc, pLeaf,
                                        descTypeNum, pDescSS, pParent);
                if (pEl != NULL)
                  if (pEl->ElTypeNumber != pRule1->SrSSchemaNat->SsRootElem)
                    /* cree un element du type de la regle racine */
                    {
                      pEl1 = NewSubtree (pRule1->SrSSchemaNat->SsRootElem,
                                         pRule1->SrSSchemaNat, pDoc,
                                         FALSE, TRUE, TRUE, TRUE);
                      InsertChildFirst (pEl1, pEl, pLeaf, pDoc);
                      pEl = pEl1;
                    }
              }
            break;
          case CsIdentity:
            if (pRule1->SrIdentRule <= 0)
              pEl = NULL;
            else
              {
                pEl = CreateDescendant (pRule1->SrIdentRule, pSS, pDoc, pLeaf,
                                        descTypeNum, pDescSS, pParent);
                if (pEl != NULL)
                  /* on a effectivement cree une descendance */
                  if (pEl->ElTypeNumber != pRule1->SrIdentRule)
                    /* le 1er element de la descendance n'est pas du type */
                    /* identique */
                    {
                      pRule2 = pSS->SsRule->SrElem[pRule1->SrIdentRule - 1];
                      if (pRule2->SrNInclusions > 0 || pRule2->SrNExclusions > 0)
                        /* le type identique porte des inclusions ou des */
                        /* exclusions, on le cree */
                        {
                          pEl1 = NewSubtree (pRule1->SrIdentRule, pSS, pDoc,
                                             FALSE, TRUE, TRUE, TRUE);
                          InsertChildFirst (pEl1, pEl, pLeaf, pDoc);
                          pEl = pEl1;
                        }
                    }
              }
	       
            break;
          case CsList:
            if (pSS && pSS->SsName && !strcmp (pSS->SsName, "Template"))
              {
                // look for an enclosing CsList constructor
                pEl1 = pParent->ElParent;
                while (pEl1 && pSS && pSS->SsName && !strcmp (pSS->SsName, "Template"))
                  {
                    pSS = pEl1->ElStructSchema;
                    typeNum = pEl1->ElTypeNumber;
                    pRule1 = pSS->SsRule->SrElem[typeNum - 1];
                  }
                if (descTypeNum == 1)
                  // update the schema of the inserted text unit
                  pDescSS = pSS;
              }
            if (pRule1)
              pEl = CreateDescendant (pRule1->SrListItem, pSS, pDoc, pLeaf,
                                      descTypeNum, pDescSS, pParent);
            if (pEl != NULL)
              {
                pRule2 = pSS->SsRule->SrElem[pRule1->SrListItem - 1];
                /* si les elements de la liste portent des inclusions */
                /* ou des exclusions, on les cree */
                if ((pRule2->SrConstruct == CsNatureSchema ||
                     EquivalentSRules (pEl->ElTypeNumber, pEl->ElStructSchema,
                                       pRule1->SrListItem, pSS, pEl))
                    && pRule2->SrNInclusions == 0
                    && pRule2->SrNExclusions == 0)
                  pEl1 = pEl;
                else
                  {
                    pEl1 = NewSubtree (pRule1->SrListItem, pSS, pDoc,
                                       FALSE, TRUE, TRUE, TRUE);
                    InsertChildFirst (pEl1, pEl, pLeaf, pDoc);
                    pEl = pEl1;
                  }
                for (i = 2; i <= pRule1->SrMinItems; i++)
                  {
                    pEl2 = NewSubtree (pRule1->SrListItem, pSS, pDoc,
                                       TRUE, TRUE, TRUE, TRUE);
                    if (pEl2 != NULL)
                      {
                        InsertElementAfter (pEl1, pEl2);
                        pEl1 = pEl2;
                      }
                  }
              }
            break;
          case CsAny:
            pEl = NewSubtree (descTypeNum, pSS, pDoc, TRUE, TRUE, TRUE,
                              TRUE);
            if (descTypeNum <= MAX_BASIC_TYPE)
              *pLeaf = pEl;
            break;
          case CsChoice:
            if (pRule1->SrNChoices == 0)
              {
                /* c'est une regle UNIT */
                if (descTypeNum <= MAX_BASIC_TYPE)
                  /* on cherche a creer un element d'un type de base */
                  /* Cree une feuille du type voulu */
                  {
                    pEl = NewSubtree (descTypeNum, pSS, pDoc,
                                      TRUE, TRUE, TRUE, TRUE);
                    *pLeaf = pEl;
                  }
              }
            else if (pRule1->SrNChoices == -1)
              /* c'est une regle NATURE */
              /* essaie toutes les regles de nature du schema */
              {
                i = MAX_BASIC_TYPE - 1;
                do
                  {
                    if (pSS->SsRule->SrElem[i++]->SrConstruct == CsNatureSchema)
                      pEl = CreateDescendant (i, pSS, pDoc, pLeaf,
                                              descTypeNum, pDescSS, pParent);
                  }
                while (pEl == NULL && i < pSS->SsNRules);
              }
            else
              /* C'est un choix parmi des types en nombre limite */
              {
                i = 0;
                pDesc = NULL;
                // @@@@@ check current choice types before CreateDescendant
                do
                  {
                    if (SameSRules (pRule1->SrChoice[i], pSS, descTypeNum, pDescSS))
                      /* c'est un element du type voulu, on le cree */
                      {
                        pDesc = NewSubtree (descTypeNum, pDescSS, pDoc, FALSE, TRUE, TRUE, TRUE);
                        *pLeaf = pDesc;
                      }
                    i++;
                   }
                while (pDesc == NULL && i < pRule1->SrNChoices);
                if (pDesc == NULL)
                  {
                    i = 0;
                    do
                      pDesc = CreateDescendant (pRule1->SrChoice[i++], pSS, pDoc,
                                                pLeaf, descTypeNum, pDescSS, pParent);
                    while (pDesc == NULL && i < pRule1->SrNChoices);
                  }
                if (pDesc != NULL)
                  {
                    /* une descendance possible du type voulu */
                    if ((pRule1->SrChoice[i - 1] <= MAX_BASIC_TYPE ||
                         pDesc->ElTypeNumber == pRule1->SrChoice[i - 1]) &&
                        (!strcmp (pDesc->ElStructSchema->SsName,
                                  pSS->SsName) ||
                         pSS->SsRule->SrElem[pRule1->SrChoice[i - 1] - 1]->SrConstruct == CsNatureSchema))
                      pEl = pDesc;
                    else
                      {
                        pEl = NewSubtree (pRule1->SrChoice[i - 1],
                                          pSS, pDoc, FALSE, TRUE,
                                          TRUE, TRUE);
                        if (pEl != NULL)
                          InsertChildFirst (pEl, pDesc, pLeaf, pDoc);
                      }
                  }
              }
            break;
          case CsAggregate:
          case CsUnorderedAggregate:
            pDesc = NULL;
            /* le type a creer est-il un des composants de l'agregat? */
            for (i = 0; pDesc == NULL && i < pRule1->SrNComponents; i++)
              if (SameSRules (pRule1->SrComponent[i], pSS,
                              descTypeNum, pDescSS))
                /* c'est un element du type voulu, on le cree */
                {
                  pDesc = NewSubtree (descTypeNum, pDescSS, pDoc,
                                      FALSE, TRUE, TRUE, TRUE);
                  *pLeaf = pDesc;
                }
            if (pDesc == NULL)
              /* on tente de creer une descendance pour les */
              /* seuls composants obligatoires de l'agregat */
              for (i = 0; pDesc == NULL && i < pRule1->SrNComponents; i++)
                if (!pRule1->SrOptComponent[i])
                  pDesc = CreateDescendant (pRule1->SrComponent[i], pSS, pDoc,
                                            pLeaf, descTypeNum, pDescSS, pParent);
            if (pDesc == NULL)
              /* on n'a rien pu creer en ne prenant que les composants */
              /* obligatoires, on essaie maintenant les composants optionnels */
              for (i = 0; pDesc == NULL && i < pRule1->SrNComponents; i++)
                if (pRule1->SrOptComponent[i])
                  pDesc = CreateDescendant (pRule1->SrComponent[i], pSS, pDoc,
                                            pLeaf, descTypeNum, pDescSS, pParent);
            if (pDesc != NULL)
              /* on a pu creer une descendance */
              {
                i --;
                if ((pRule1->SrComponent[i] <= MAX_BASIC_TYPE ||
                     pDesc->ElTypeNumber == pRule1->SrComponent[i]) &&
                    (!strcmp (pDesc->ElStructSchema->SsName,
                              pSS->SsName) ||
                     pSS->SsRule->SrElem[pRule1->SrComponent[i] - 1]->SrConstruct == CsNatureSchema))
                  pEl1 = pDesc;
                else
                  {
                    pEl1 = NewSubtree (pRule1->SrComponent[i], pSS, pDoc,
                                       FALSE, TRUE, TRUE, TRUE);
                    if (pEl1 != NULL)
                      InsertChildFirst (pEl1, pDesc, pLeaf, pDoc);
                  }
                if (i == 0)
                  pEl = pEl1;
                /* cree les autres composants obligatoires de l'agregat */
                for (j = 0; j < pRule1->SrNComponents; j++)
                  if (j == i)
                    {
                      if (pEl == NULL)
                        pEl = pEl1;
                    }
                  else if (!pRule1->SrOptComponent[j])
                    {
                      pEl2 = NewSubtree (pRule1->SrComponent[j], pSS, pDoc,
                                         TRUE, TRUE, TRUE, TRUE);
                      if (pEl2 != NULL)
                        {
                          if (pEl == NULL)
                            pEl = pEl2;
                          if (i > j)
                            InsertElementBefore (pEl1, pEl2);
                          else
                            {
                              InsertElementAfter (pEl1, pEl2);
                              pEl1 = pEl2;
                            }
                        }
                    }
              }
            break;
          default:
            break;
          }
      if (pRule1->SrRecursive && pRule1->SrRecursDone && !stop)
        pRule1->SrRecursDone = FALSE;
    }
  return pEl;
}

/*----------------------------------------------------------------------
  TtaGetSchemaExtension

  Returns the structure schema extension that is associated with a given
  document and that has a given name.
  Parameters:
  document:
  NomExtension:
  Return value:
  NULL if the extension is not associated with the document.
  ----------------------------------------------------------------------*/
SSchema TtaGetSchemaExtension (Document document, char *NomExtension)
{
  PtrSSchema          pSS;
  PtrSSchema          pExtSS;
  PtrDocument         pDoc;

  pDoc = LoadedDocument[document - 1];
  pExtSS = NULL;
  if (pDoc != NULL && NomExtension != NULL)
    {
      pSS = pDoc->DocSSchema;
      while (pSS != NULL && pExtSS == NULL)
        if (strcmp (pSS->SsName, NomExtension) == 0)
          pExtSS = pSS;
        else
          pSS = pSS->SsNextExtens;
    }
  return (SSchema) pExtSS;
}

/*----------------------------------------------------------------------
  ExtensionRule retourne un pointeur sur la regle d'extension       
  qui se trouve dans le schema d'extension pExtSS et qui          
  s'applique aux elements de type typeNum definis dans le schema  
  de structure pSS.                                               
  ----------------------------------------------------------------------*/
SRule *ExtensionRule (PtrSSchema pSS, int typeNum, PtrSSchema pExtSS)
{
  PtrSRule            RegleExt, Rule;
  char               *typeName = NULL;
  int                 r;

  RegleExt = NULL;
  if (pExtSS && pSS && pExtSS->SsExtension && !pSS->SsExtension)
    {
      typeName = pSS->SsRule->SrElem[typeNum - 1]->SrName;
      /*on cherche d'abord une regle d'extension ayant le nom du type */
      r = 0;
      while (RegleExt == NULL && r < pExtSS->SsNExtensRules)
        if (strcmp (typeName, pExtSS->SsExtensBlock->EbExtensRule[r].SrName) == 0)
          RegleExt = &(pExtSS->SsExtensBlock->EbExtensRule[r]);
        else
          r++;
      if (RegleExt == NULL)
        /* on n'a pas trouve' de regle de meme nom */
        {
          Rule = pSS->SsRule->SrElem[typeNum - 1];
          if (typeNum == pSS->SsRootElem)
            /* il s'agit d'une racine de document. */
            /* On cherche une regle d'extension qui */
            /* s'applique aux racines (regle avec SrName vide). */
            {
              r = 0;
              while (RegleExt == NULL && r < pExtSS->SsNExtensRules)
                {
                  if (pExtSS->SsExtensBlock->EbExtensRule[r].SrName[0] == EOS)
                    RegleExt = &(pExtSS->SsExtensBlock->EbExtensRule[r]);
                  else
                    r++;
                }
            }
        }
    }
  return RegleExt;
}

/*----------------------------------------------------------------------
  ValidExtension retourne vrai si l'extension de schema pExt     
  peut etre appliquee au schema de l'element pEl et, dans ce cas, 
  retourne dans pExt le schema a appliquer.                       
  ----------------------------------------------------------------------*/
ThotBool            ValidExtension (PtrElement pEl, PtrSSchema * pExt)
{
  PtrSSchema          pExtSS, pSS;
  ThotBool            result;

  result = FALSE;
  if (pEl == NULL)
    return FALSE;
  pSS = pEl->ElStructSchema;
  while (pSS != NULL && *pExt != NULL && !result)
    {
      /* parcourt les extensions du schema pSS */
      pExtSS = pSS->SsNextExtens;
      while (pExtSS != NULL && !result)
        if (!strcmp (pExtSS->SsName, (*pExt)->SsName))
          {
            *pExt = pExtSS;
            result = TRUE;
          }
        else
          pExtSS = pExtSS->SsNextExtens;
      if (!result)
        /* pas trouve' */
        /* cherche un autre schema dans les elements ascendants */
        {
          pEl = pEl->ElParent;
          while (pEl != NULL && pEl->ElStructSchema == pSS)
            pEl = pEl->ElParent;
          if (pEl == NULL)
            pSS = NULL;
          else
            pSS = pEl->ElStructSchema;
        }
    }
  return result;
}

/*----------------------------------------------------------------------
  Cherche parmi les attributs attaches a l'element pointe' par    
  pEl s'il en existe un du meme type (eventuellement avec une     
  valeur differente) que celui pointe par pAttr.                  
  Retourne un pointeur sur cet attribut ou NULL s'il n'existe pas 
  ----------------------------------------------------------------------*/
PtrAttribute        GetAttributeOfElement (PtrElement pEl, PtrAttribute pAttr)
{
  ThotBool            found;
  PtrAttribute        pA;

  found = FALSE;
  pA = pEl->ElFirstAttr;
  while (pA != NULL && !found)
    {
      /* attribute 1: Langue, quel que soit le schema */
      if (pA->AeAttrNum == pAttr->AeAttrNum &&
          (pA->AeAttrNum == 1 ||
           !strcmp (pA->AeAttrSSchema->SsName, pAttr->AeAttrSSchema->SsName)))
        /* c'est l'attribut cherche' */
        found = TRUE;
      else
        /* passe a l'attribut suivant du meme element */
        pA = pA->AeNext;
    }
  return pA;
}

/*----------------------------------------------------------------------
  Retourne un pointeur vers l'attribut qui contient la valeur	
  pour l'element pEl de l'attribut pAttr				
  Si cet attribut n'a pas de valeur pour l'element pEl, retourne	
  NULL.								
  ----------------------------------------------------------------------*/
PtrAttribute        AttributeValue (PtrElement pEl, PtrAttribute pAttr)
{
  if (pEl != NULL)
    return (GetAttributeOfElement (pEl, pAttr));
  else
    return (NULL);
}

/*----------------------------------------------------------------------
  CanAssociateAttr indique si on peut associer un attribut du	
  type de pNewAttr a l'element pEl qui porte eventuellement un	
  attribut pAttr de meme type que pNewAttr.			
  ----------------------------------------------------------------------*/
ThotBool CanAssociateAttr (PtrElement pEl, PtrAttribute pAttr,
                           PtrAttribute pNewAttr,
                           ThotBool *mandatory)
{
  ThotBool            requested, allowed;
  PtrSRule            pRe1;
  int                 locAtt;

  if (pEl == NULL || pEl->ElStructSchema == NULL)
    return FALSE;
  /* cherche si l'attribut a une valeur imposee pour cet element */
  requested = FALSE;
  *mandatory = FALSE;
  if (pAttr != NULL)
    /* l'attribut a une valeur */
    if (pAttr->AeDefAttr)
      /* on peut modifier un attribut reference, meme impose' */
      if (pAttr->AeAttrType != AtReferenceAttr)
        requested = TRUE;

  /* si c'est un attribut local, il ne s'applique que s'il correspond au */
  /* type de l'element */
  if (pNewAttr->AeAttrSSchema->SsAttribute->TtAttr[pNewAttr->AeAttrNum - 1]->AttrGlobal)
    /*c'est un attribut global, il peut s'appliquer a tous les types d'elements */
    allowed = TRUE;
  else
    /* c'est un attribut local */
    {
      /* a priori, on ne peut pas l'appliquer a ce type d'element */
      allowed = FALSE;
      if (!strcmp (pEl->ElStructSchema->SsName, pNewAttr->AeAttrSSchema->SsName))
        {
          /* l'attribut est defini dans le meme schema que l'element */
          pRe1 = pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber - 1];
          for (locAtt = 0; locAtt < pRe1->SrNLocalAttrs; locAtt++)
            if (pRe1->SrLocalAttr->Num[locAtt] == pNewAttr->AeAttrNum)
              /* l'attribut fait partie des attributs locaux de ce */
              /* type d'element, on peut l'appliquer a l'element */
              {
                allowed = TRUE;
                *mandatory = pRe1->SrRequiredAttr->Bln[locAtt];
              }
        }
      else if (pNewAttr->AeAttrSSchema->SsExtension)
        /* il s'agit d'un attribut defini dans une extension de schema */
        if (ValidExtension (pEl, &pNewAttr->AeAttrSSchema))
          /* cette extension s'applique au schema definissant l'element */
          {
            pRe1 = ExtensionRule (pEl->ElStructSchema, pEl->ElTypeNumber,
                                  pNewAttr->AeAttrSSchema);
            if (pRe1 != NULL)
              /* ce type d'element a une regle d'extension: pRe1 */
              /* on cherche si l'attribut fait partie des attributs locaux */
              /* de cette regle d'extension */
              for (locAtt = 0; locAtt < pRe1->SrNLocalAttrs; locAtt++)
                if (pRe1->SrLocalAttr->Num[locAtt] == pNewAttr->AeAttrNum)
                  /* l'attribut fait partie des attributs locaux */
                  /* on peut l'appliquer a l'element */
                  {
                    allowed = TRUE;
                    *mandatory = pRe1->SrRequiredAttr->Bln[locAtt];
                  }
          }
    }

  return (!requested && allowed);
}

/*----------------------------------------------------------------------
  CanSplitElement: Indique si on peut diviser un element en deux, 
  au caractere de rang firstChar dans l'element firstEl	
  lineBlock indique si on veut diviser un line block ou	
  un element non mis en ligne.                            
  Retourne TRUE si c'est possible et dans ce cas :                
  pList : l'element List qui englobe l'element a diviser	
  pEl : l'element devant lequel se fera la division.      
  pSplitEl : l'element qui va etre divise'.		
  ----------------------------------------------------------------------*/
ThotBool CanSplitElement (PtrElement firstEl, int firstChar,
                          ThotBool lineBlock, PtrElement * pList,
                          PtrElement * pEl, PtrElement * pSplitEl)
{
  PtrElement          pE;
  int		       nComp;
  ThotBool            exctab;
  RConstruct          constr;

  *pList = NULL;
  *pEl = NULL;
  *pSplitEl = NULL;
  if (firstEl != NULL)
    {
      *pEl = firstEl;
      if (lineBlock)
        /* on veut couper un line block en deux */
        {
          /* Verification des conditions d'application: l'element courant
             doit etre un atome contenu dans une liste qui elle-meme est une
             descendante d'une autre liste (sans agregat entre les deux)
             ou sinon un atome contenu dans une liste */
          if (!ElementIsReadOnly (firstEl) &&
              firstEl->ElTerminal &&
              firstEl->ElLeafType != LtPageColBreak &&
              firstEl->ElParent &&
              (firstEl->ElPrevious ||
               firstEl->ElLeafType == LtText ||
               (firstEl->ElLeafType == LtPicture && firstChar > 0)))
            {
              /* on cherche d'abord si un element ascendant
                 possede une exception ParagraphBreak */
              pE = firstEl->ElParent;
              while (pE && *pList == NULL)
                {
                  if (TypeHasException (ExcParagraphBreak,
                                        pE->ElTypeNumber, pE->ElStructSchema))
                    *pList = AncestorList (pE);
                  else
                    pE = pE->ElParent;
                }
              if (*pList == NULL)
                {
                  constr = GetElementConstruct (firstEl->ElParent, &nComp);
                  if (constr == CsList)
                    *pList = AncestorList (firstEl->ElParent);
                  else if (constr == CsAny)
                    {
                      if (firstEl->ElParent->ElParent != NULL)
                        {
                          constr = GetElementConstruct (firstEl->ElParent->ElParent, &nComp);
                          if (constr == CsAny || constr == CsList)
                            *pList = firstEl->ElParent->ElParent;
                        }
                    }
                  else
                    {
                      pE = firstEl;
                      if (constr == CsChoice)
                        if (firstEl->ElParent->ElParent != NULL)
                          if (GetElementConstruct (firstEl->ElParent->ElParent, &nComp) == CsList)
                            pE = firstEl->ElParent->ElParent;
                      *pList = AncestorList (pE);
                      if (*pList == NULL)
                        /* Search if the type of the parent is Any */
                        *pList = ParentAny (firstEl->ElParent);
                    }
                }
            }
          if (*pList &&
              TypeHasException (ExcNoCreate,
                                (*pList)->ElFirstChild->ElTypeNumber,
                                (*pList)->ElFirstChild->ElStructSchema))
            *pList = NULL;
          if (*pList && !CanChangeNumberOfElem (*pList, 1))
            *pList = NULL;
        }
      else
        /* on veut diviser un "gros" element en deux */
        {
          pE = firstEl;
          if (firstChar <= 1)
            /* la selection est en debut d'element */
            {
              /* tant qu'il n'y a pas de frere precedent, remonte au pere */
              while (pE->ElParent != NULL && pE->ElPrevious == NULL)
                pE = pE->ElParent;
              if (pE->ElPrevious == NULL)
                /* il n'y a pas de frere precedent, on ne peut pas couper */
                pE = NULL;
            }

          /* forbid splitting ReadOnly element, except after the end or
             before the beginning of	a character string */
          if (pE && ElementIsReadOnly (pE))
            {
              if (pE->ElTypeNumber != CharString + 1)
                pE = NULL;
              else
                if (firstChar > 1 && firstChar <= pE->ElTextLength)
                  pE = NULL;
            }

          if (pE)
            {
              /* l'element devant (ou sur) lequel on va couper */
              *pEl = pE;
              /* on cherche le premier element CsList ascendant */
              pE = pE->ElParent;
              while (pE != NULL && *pList == NULL)
                {
                  pE = pE->ElParent;
                  if (pE != NULL)
                    if (GetElementConstruct (pE, &nComp) == CsList)
                      *pList = pE;
                }
            }
        }
    }
  if (*pList)
    {
      /* cherche l'element de plus haut niveau a dupliquer */
      pE = (*pEl)->ElParent;
      *pSplitEl = pE;
      while (pE != *pList)
        {
          *pSplitEl = pE;
          pE = pE->ElParent;
        }
      exctab = FALSE;
      if (exctab)
        /* c'est la division d'un tableau, on refuse */
        {
          *pList = NULL;
          *pSplitEl = NULL;
          *pEl = NULL;
        }
    }
  return (*pList != NULL);
}
