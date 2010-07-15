/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2010
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Manipulation des variables de presentation pour
 * les images abstraites des documents.
 *
 * Author: V. Quint (INRIA)
 *
 */
#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "fileaccess.h"
#include "language.h"

#include "tree_f.h"
#include "memory_f.h"
#include "changeabsbox_f.h"
#include "createabsbox_f.h"
#include "presrules_f.h"
#include "fileaccess_f.h"
#include "structschema_f.h"
#include "content_f.h"
#include "presvariables_f.h"
#include "schemas_f.h"

/*----------------------------------------------------------------------
  MakeAliasTypeCount
  Create a temporary structure rule in schema pSchStr to allow
  searching of multiple types used in a counter.
  Create alias rule SsNRules+1 for reset elements
  Create alias rule SsNRules+2 for add elements
  ----------------------------------------------------------------------*/
static int MakeAliasTypeCount (Counter *pCounter, CounterOp op,
                               PtrSSchema pSchStr)
{
  PtrSRule            NewAlias;
  int                 i, rule;

  if (op == CntrSet)
    rule = pSchStr->SsNRules + 1;
  else if (op == CntrAdd)
    rule = pSchStr->SsNRules + 2;
  else				/* on s'est trompe c'est un CntrRank */
    return pCounter->CnItem[0].CiElemType;

  NewAlias = (PtrSRule) TtaGetMemory (sizeof (SRule));
  pSchStr->SsRule->SrElem[rule -1] = NewAlias;

  /* initialise l'alias temporaire */
  NewAlias->SrName = TtaStrdup ("Counter_alias");
  NewAlias->SrNDefAttrs = 0;
  NewAlias->SrNLocalAttrs = 0;
  NewAlias->SrLocalAttr = NULL;
  NewAlias->SrRequiredAttr = NULL;
  NewAlias->SrUnitElem = FALSE;
  NewAlias->SrRecursive = FALSE;
  NewAlias->SrExportedElem = FALSE;
  NewAlias->SrFirstExcept = 0;
  NewAlias->SrLastExcept = 0;
  NewAlias->SrNInclusions = 0;
  NewAlias->SrNExclusions = 0;
  NewAlias->SrConstruct = CsChoice;
  NewAlias->SrNChoices = 0;
  /* initialise le constructeur choix de l'alias */
  for (i = 0; i < pCounter->CnNItems; i++)
    {
      if (pCounter->CnItem[i].CiCntrOp == op)
        {
          NewAlias->SrChoice[NewAlias->SrNChoices] =
            pCounter->CnItem[i].CiElemType;
          NewAlias->SrNChoices += 1;
        }
    }
  return rule;
}

/*----------------------------------------------------------------------
  ReleaseAliasTypeCount
  Destroys a provisional structure rule created for evaluating
  a counter.
  typeInt is the rule number and pSS is the structure schema where
  it was created.
  ----------------------------------------------------------------------*/
static void ReleaseAliasTypeCount (int typeNum, PtrSSchema pSS)
{
  TtaFreeMemory (pSS->SsRule->SrElem[typeNum -1]->SrName);
  TtaFreeMemory (pSS->SsRule->SrElem[typeNum -1]);
  pSS->SsRule->SrElem[typeNum -1] = NULL;
}

/*----------------------------------------------------------------------
  GetCounterValEl renvoie la valeur numerique de set ou d'increment 
  associee a l'element pEl.                               
  ----------------------------------------------------------------------*/
static int GetCounterValEl (Counter * pCounter, PtrElement pEl, CounterOp op,
                            PtrSSchema pSchStr)
{
  int                 i;

  for (i = 0; i < pCounter->CnNItems; i++)
    if (pCounter->CnItem[i].CiCntrOp == op)
      if (EquivalentType (pEl, pCounter->CnItem[i].CiElemType, pSchStr)
          || EquivalentSRules (pEl->ElTypeNumber, pEl->ElStructSchema,
                               pCounter->CnItem[i].CiElemType, pSchStr, pEl))
        if (pCounter->CnItem[i].CiElemType != PageBreak + 1
            || pEl->ElViewPSchema == pCounter->CnItem[i].CiViewNum)
          return pCounter->CnItem[i].CiParamValue;
  return 0;			/* par defaut... */
}

/*----------------------------------------------------------------------
  InitCounterByAttribute                                          
  ----------------------------------------------------------------------*/
static ThotBool InitCounterByAttribute (int *valinit, Counter * pCo,
                                        PtrElement pElNum, PtrSSchema pSS)
{
  ThotBool            result, stop;
  PtrElement          pEl;
  PtrAttribute        pAttr;

  result = FALSE;
  *valinit = 0;
  if (pCo->CnItem[0].CiInitAttr > 0)
    {
      /* la valeur initiale du compteur est definie par un attribut */
      pEl = pElNum;
      /* cherche si l'element a numeroter ou l'un de ses ascendants */
      /* porte l'attribut qui intialise le compteur */
      while (!result && pEl != NULL)
        {
          /* cet attribut est-il present sur l'element courant */
          pAttr = pEl->ElFirstAttr;
          stop = FALSE;	/* parcourt les attributs de l'element */
          do
            if (pAttr == NULL)
              stop = TRUE;	/* dernier attribut */
            else if (pAttr->AeAttrNum == pCo->CnItem[0].CiInitAttr &&
                     !strcmp (pAttr->AeAttrSSchema->SsName, pSS->SsName))
              stop = TRUE;	/* c'est l'attribut cherche' */
            else
              pAttr = pAttr->AeNext;	/* au suivant */
          while (!stop);
          if (pAttr != NULL)
            /* l'element porte l'attribut qui initialise le compteur */
            {
              result = TRUE;
              *valinit = pAttr->AeAttrValue;	/* on prend la valeur de l'attribut */
            }
          else
            /* passe a l'element ascendant */
            pEl = pEl->ElParent;
        }
    }
  return result;
}

/*----------------------------------------------------------------------
  CondAttrOK
  return TRUE if the attribute condition in counter item pCountItem is
  satisfied for element pEl.
  ----------------------------------------------------------------------*/
static ThotBool CondAttrOK (CntrItem * pCountItem, PtrElement pEl,
                            PtrSSchema pSS)
{
  ThotBool            result, stop;
  PtrAttribute        pAttr;

  if (pCountItem->CiCondAttr == 0)
    /* there is no condition about attribute in this counter item */
    return TRUE;

  /* cet attribut est-il present sur l'element courant ? */
  pAttr = pEl->ElFirstAttr;
  stop = FALSE;	/* parcourt les attributs de l'element */
  do
    if (pAttr == NULL)
      stop = TRUE;	/* dernier attribut */
    else if (pAttr->AeAttrNum == pCountItem->CiCondAttr &&
             !strcmp (pAttr->AeAttrSSchema->SsName, pSS->SsName))
      stop = TRUE;	/* c'est l'attribut cherche' */
    else
      pAttr = pAttr->AeNext;	/* au suivant */
  while (!stop);
  if (pAttr)
    if (pAttr->AeAttrType == AtTextAttr)
      {
	if (!StringAndTextEqual(pCountItem->CiCondAttrTextValue, pAttr->AeAttrText))
	  pAttr = NULL;
      }
    else if (pAttr->AeAttrType == AtNumAttr || pAttr->AeAttrType == AtEnumAttr)
      {
	if (pCountItem->CiCondAttrIntValue != pAttr->AeAttrValue)
	  pAttr = NULL;
      }
  if (pCountItem->CiCondAttrPresent)
    result = (pAttr != NULL);
  else
    result = (pAttr == NULL);
  return result;
}

/*----------------------------------------------------------------------
  GetCounterItem
  return the rank of the item of counter pCounter which applies an operation
  cntrOp to element pEl.
  return -1 if not found.
  ----------------------------------------------------------------------*/
static int GetCounterItem (Counter * pCounter, CounterOp cntrOp,
                           PtrSSchema pSS, PtrElement pEl)
{
  int		i;

  if (pEl)
    for (i = 0; i < pCounter->CnNItems; i++)
      if (pCounter->CnItem[i].CiCntrOp == cntrOp)
        if (EquivalentType (pEl, pCounter->CnItem[i].CiElemType, pSS))
   	      return i;
  return (-1);
}

/*----------------------------------------------------------------------
  CounterValMinMax
  retourne la valeur minimale ou maximale (selon que Maximum est faux ou vrai)
  du compteur de numero NCompt (defini dans le schema de presentation pSchP,
  qui s'applique au schema de structure pSS) pour l'element pElNum.
  view indique la vue concernee (uniquement pour les compteurs de page).
  ----------------------------------------------------------------------*/
int CounterValMinMax (int counterNum, PtrSSchema pSS, PtrPSchema pSchP,
                      PtrElement pElNum, int view, ThotBool Maximum)
{
  int                 value, valueinitattr, i;
  int                 TypeIncr, TypeSet, TypeRank;
  PtrSSchema          pSchIncr;
  ThotBool            stop, pstop, initattr, CondAttr, found;
  PtrElement          pEl;
  Counter            *pCo1;
  PtrElement          pEl2;
  PtrSSchema          pSchStr, pSSpr;

  valueinitattr = 0;
  pCo1 = &pSchP->PsCounter[counterNum - 1];
  initattr = FALSE;		/* a priori, la valeur initiale du compteur ne
                         depend pas d'un attribut. */
  pstop = TRUE;

  /* Traitement de la partie initialisation du compteur */
  initattr = InitCounterByAttribute (&valueinitattr, pCo1, pElNum, pSS);

  /* on ne traite pas les compteurs de type CntrRLevel */

  /* Traitement des compteurs de type CntrRank */
  if (pCo1->CnItem[0].CiCntrOp == CntrRank)
    {
      TypeRank = pCo1->CnItem[0].CiElemType;

      /* Cas particulier du compteur de page */
      if (TypeRank == PageBreak + 1)
        {
          /* c'est un compteur de pages (CntrRank of Page) */
          if (initattr)
            value = valueinitattr;
          else
            value = 0;
          /* On vient de trouver la valeur minimale */

          if (Maximum)
            {
              /* mais on veut la valeur maximale; alors, on continue */
              pEl = pElNum;
              pEl2 = pEl;
              while (pEl2->ElParent != NULL)
                pEl2 = pEl2->ElParent;
              pSSpr = pEl2->ElStructSchema;
              /* view a laquelle appartient la marque de page cherchee */
              if (pCo1->CnItem[0].CiViewNum > 0)
                view = pCo1->CnItem[0].CiViewNum;
              /* cherche la marque de page qui precede l'element */
              stop = FALSE;
              do
                {
                  pEl = BackSearchTypedElem (pEl, TypeRank, NULL, NULL);
                  if (pEl == NULL)
                    /* pas de marque de page precedente */
                    stop = TRUE;
                  else
                    {
                      if (strcmp (pElNum->ElStructSchema->SsName, pSSpr->SsName))
                        /* Saut de page d'une view squelette */
                        /* on ne que compte les pages etrangeres au schema */
                        /* principal */
                        pstop = (strcmp (pEl->ElStructSchema->SsName, pSSpr->SsName) != 0);
                      else
                        pstop = (strcmp (pEl->ElStructSchema->SsName, pSSpr->SsName) == 0);

                      /* on ignore les pages qui ne concernent pas la view */
                      if ((pEl->ElViewPSchema == view) && (pstop))
                        stop = TRUE;
                    }
                }
              while (!stop);

              if (pEl == NULL)
                {
                  /* pas de marque de page precedente */
                  if (initattr)
                    value = valueinitattr;
                  else
                    value = 0;
                }
              else
                /* on prend pour valeur le numero de la marque de page */
                /* precedente */
                if ((pEl->ElViewPSchema == view) && (pEl->ElPageNumber > value))
                  value = pEl->ElPageNumber;
              /* A partir de cette marque de page, cherche en avant les */
              /* suivantes jusqu'a ne plus en trouver */
              while (pEl != NULL)
                {
                  pEl = FwdSearchTypedElem (pEl, TypeRank, NULL, NULL);
                  if (pEl != NULL)
                    if ((pEl->ElViewPSchema == view) && (pEl->ElPageNumber > value))
                      value = pEl->ElPageNumber - 1;
                }
              if (pCo1->CnPageFooter)
                {
                  /* Si on est dans un compteur en bas de page, il faut */
                  /* incrementer car on creera un saut de page en plus a */
                  /* la fin */
                  value++;
                }
            }
        }
      else
        {
          /* Cas standard des compteurs CntrRank */
          /* numero = rang de l'element dans la liste */
          /* Cherche le premier element de type du rank */
          /* englobant l'element a numeroter */
          pEl = GetTypedAncestor (pElNum, TypeRank, pSS);
          if (pEl == NULL)
            value = 0;	/* not found */
          else
            {
              /* Compute the rank of the element found among its siblings*/
              if (initattr)
                value = valueinitattr;
              else
                value = 1;
              /* On vient de trouver la valeur minimale du compteur */

              if (Maximum)
                {
                  /* on cherche la valeur maximale */
                  if (pEl->ElPrevious == NULL && pEl->ElNext == NULL)
                    /* l'element dont on veut le rang n'a pas de frere... */
                    if (pEl->ElParent != NULL)
                      /* ... mais il a un pere... */
                      if (pEl->ElParent->ElStructSchema->SsRule->SrElem[pEl->ElParent->
                                                                        ElTypeNumber - 1]->SrConstruct == CsChoice)
                        /* ... et son pere est un choix */
                        /* on prendra le rang du pere */
                        pEl = pEl->ElParent;

                  /* D'abord, on remonte au premier frere... */
                  while (pEl->ElPrevious != NULL)
                    pEl = pEl->ElPrevious;
                  /* ...puis on compte jusqu'au dernier */
                  while (pEl->ElNext != NULL)
                    {
                      pEl = pEl->ElNext;
                      /* on ne compte que les elements du type a compter */
                      if (EquivalentType (pEl, TypeRank, pSS))
                        if (CondAttrOK (&pCo1->CnItem[0], pEl, pSS))
                          value++;
                    }
                }
            }
        }
    }
  /* fonction de comptage SET ON, ADD ON */
  else
    {
      /* type ou alias qui fait reset */
      TypeSet = MakeAliasTypeCount (pCo1, CntrSet, pSS);
      /* type ou alias qui incremente */
      TypeIncr = MakeAliasTypeCount (pCo1, CntrAdd, pSS);
      CondAttr = FALSE;
      for (i = 0; i < pCo1->CnNItems && !CondAttr; i++)
        if (pCo1->CnItem[i].CiCondAttr > 0)
          CondAttr = TRUE;
      pSchStr = pSS;

      /* Find the first element of type TypeSet that is before the element
	 to be numbered */
      pEl = BackSearchTypedElem (pElNum, TypeSet, pSS, NULL);
      if (CondAttr)
        /* there is a condition on attributes for this counter */
        {
          found = FALSE;
          do
            {
              i = GetCounterItem (pCo1, CntrSet, pSS, pEl);
              if (i < 0)
                pEl = NULL;
              else
                if (CondAttrOK (&pCo1->CnItem[i], pEl, pSS))
                  found = TRUE;
                else
		  pEl = BackSearchTypedElem (pElNum, TypeSet, pSS, NULL);
            }
          while (!found && pEl);
        }
      /* s'il n' y a pas de précédent du type requis alors on reste sur pElNum */
      if (pEl == NULL)
        pEl = pElNum;

      /* l'element trouve' est celui qui reinitialise le compteur 
         Sa valeur est la valeur minimale du compteur */
      if (initattr)
        value = valueinitattr;
      else
        value = GetCounterValEl (pCo1, pEl, CntrSet, pSS);

      if (Maximum)
        {
          /* On veut la valeur maximale du compteur a partir de l'element
             trouve', cherche en avant tous les elements ayant le type qui
             incremente le compteur, jusqu'a rencontrer un autre element qui
             reset le compteur */
          if (TypeIncr <= MAX_BASIC_TYPE)
            /* c'est un type de base, on le cherche quel que soit son schema */
            pSchIncr = NULL;
          else
            pSchIncr = pSchStr; /* schema de struct. du type qui incremente*/
          if (TypeIncr > 0)
            do
              {
                pEl = FwdSearchElem2Types (pEl, TypeIncr, pElNum->ElTypeNumber,
                                         pSchIncr, pElNum->ElStructSchema, NULL);
                if (pEl != NULL &&
                    EquivalentType (pEl, TypeIncr, pSchIncr))
                  {
                    /* on a trouve' un element du type qui incremente */
                    if (!CondAttr)
                      value += GetCounterValEl (pCo1, pEl, CntrAdd, pSS);
                    else
                      /* check conditions on attributes */
                      {
                        i = GetCounterItem (pCo1, CntrAdd, pSS, pEl);
                        if (i >= 0)
                          if (CondAttrOK (&pCo1->CnItem[i], pEl, pSS))
                            value += GetCounterValEl (pCo1, pEl, CntrAdd, pSS);
                      }
                  }
              }
            while (pEl != NULL && !EquivalentType (pEl, TypeSet, pSS));
        }
      ReleaseAliasTypeCount (TypeSet, pSS);
      ReleaseAliasTypeCount (TypeIncr, pSS);
    }
  if (value < 0)
    value = 0;

  return value;
}

/*----------------------------------------------------------------------
  CounterVal retourne la valeur du compteur de numero counterNum (defini
  dans le schema de presentation  pointe' par pSchP, qui  
  s'applique au schema de structure pointe' par pSS) pour 
  l'element pointe' par pElNum.                           
  view indique la view concernee (uniquement pour les       
  compteurs de page).                                     
  ----------------------------------------------------------------------*/
int CounterVal (int counterNum, PtrSSchema pSS, PtrPSchema pSchP,
                PtrElement pElNum, int view, PtrDocument pDoc)
{
  int                 i, value, valueinitattr, level, Nincr, incrVal;
  int                 TypeIncr, TypeSet, TypeRank, TypeRLevel;
  PtrSSchema          pSchIncr, pSchSet;
  ThotBool            stop, pstop, initattr;
  Counter            *pCo1;
  PtrSSchema          pSchStr, pSSpr;
  PtrAttribute        pAttr;
  PtrElement          pEl, pElReinit, pEl2;
  PtrCondition        CondSet, CondIncr;
  ThotBool	      CondAttr;
#define MaxAncestor 50
  PtrElement          PcWithin[MaxAncestor];

  pCo1 = &pSchP->PsCounter[counterNum - 1];
  initattr = FALSE;   /* a priori, la valeur initiale du compteur ne
                         depend pas d'un attribut. */
  value = 0;
  pstop = TRUE;
  Nincr = 0;
  /* Traitement de la partie initialisation du compteur */
  initattr = InitCounterByAttribute (&valueinitattr, pCo1, pElNum, pSS);

  /* Traitement des compteurs de type CntrRLevel */
  if (pCo1->CnItem[0].CiCntrOp == CntrRLevel)
    {
      TypeRLevel = pCo1->CnItem[0].CiElemType;
      pEl = pElNum;
      /* parcourt les elements englobants de l'element sur lequel porte */
      /* le calcul du compteur */
      while (pEl != NULL)
        {
          if (pEl->ElTypeNumber == TypeRLevel &&
              !strcmp (pEl->ElStructSchema->SsName,
                       pElNum->ElStructSchema->SsName))
            /* cet element englobant a le type qui incremente le compteur*/
            /* if there is a condition about an attribute attached to the
               element, check that condition */
            if (CondAttrOK (&pCo1->CnItem[0], pEl, pElNum->ElStructSchema))
              value++;	/* incremente le compteur */
          pEl = pEl->ElParent;
        }
    }
  else
    /* Traitement des compteurs de type CntrRank */
    /* TODO traiter separement le cas des colonnes et des pages */
    if (pCo1->CnItem[0].CiCntrOp == CntrRank)
      {
        TypeRank = pCo1->CnItem[0].CiElemType;

        /* Cas particulier du compteur de page */
        if (TypeRank == PageBreak + 1)
          {
            /* c'est un compteur de pages (CntrRank of Page) */
            pEl = pElNum;
            pEl2 = pEl;
            while (pEl2->ElParent != NULL)
              pEl2 = pEl2->ElParent;
            pSSpr = pEl2->ElStructSchema;
            if (pCo1->CnItem[0].CiViewNum > 0)
              /* vue a laquelle appartient la marque de page cherchee */
              view = pCo1->CnItem[0].CiViewNum;
            /* cherche la marque de page qui precede l'element */
            stop = FALSE;
            do
              {
                pEl = BackSearchTypedElem (pEl, TypeRank, NULL, NULL);
                if (pEl == NULL)
                  /* pas de marque de page precedente */
                  stop = TRUE;
                else
                  {
                    if (strcmp (pElNum->ElStructSchema->SsName, pSSpr->SsName))
                      /* Saut de page d'une view squelette */
                      /*on ne que compte les pages etrangeres au schema principal */
                      pstop = (strcmp(pEl->ElStructSchema->SsName, pSSpr->SsName) != 0);
                    else
                      pstop = (strcmp(pEl->ElStructSchema->SsName, pSSpr->SsName) == 0);

                    /* on ignore les pages qui ne concernent pas la view */
                    /* on ignore les pages qui ne concernent pas la view */
                    if ((pEl->ElViewPSchema == view) && (pstop))
                      stop = TRUE;
                  }
              }
            while (!stop);

            if (pEl == NULL)	/* pas de marque de page precedente */
              if (initattr)
                {
                  value = valueinitattr;
                  if (value > 0)
                    value--;
                }
              else
                value = 0;

            else
              /* on prend pour valeur le numero de la marque de page precedente */
              value = pEl->ElPageNumber;
            if (pElNum->ElTerminal && pElNum->ElLeafType == LtPageColBreak)
              /* on numerote une marque de page  */
              /* on incremente la valeur */
              value++;
          }

        /* Cas standard des compteurs CntrRank */
        else
          {
            if (pCo1->CnItem[0].CiAscendLevel == 0)
              /* numero = rang de l'element dans la liste */
              /* Cherche le premier element de type du rank */
              /* englobant l'element a numeroter */
              pEl = GetTypedAncestor (pElNum, TypeRank, pSS);
            else
              {
                /* Cherche le nieme element de type TypeRank qui englobe */
                /* l'element a numeroter */
                if (pCo1->CnItem[0].CiAscendLevel < 0)
                  /* on compte les ascendants en remontant de l'element */
                  /* concerne' vers la racine */
                  level = -pCo1->CnItem[0].CiAscendLevel;
                else
                  /* on compte les ascendants en descendant de la racine vers */
                  /* l'element concerne'. Pour cela on commence par enregistrer */
                  /* le chemin de l'element concerne' vers la racine */
                  level = MaxAncestor;
                pEl = pElNum;
                while (level > 0 && pEl != NULL)
                  {
                    if (pEl->ElTypeNumber == TypeRank &&
                        !strcmp (pEl->ElStructSchema->SsName,
                                 pElNum->ElStructSchema->SsName))
                      /* cet element englobant a le type qui incremente le
                         compteur */
                      {
                        level--;
                        PcWithin[level] = pEl;
                      }
                    if (level > 0)
                      pEl = pEl->ElParent;
                  }
                if (pCo1->CnItem[0].CiAscendLevel > 0)
                  /* il faut redescendre a partir de la racine */
                  pEl = PcWithin[level + pCo1->CnItem[0].CiAscendLevel - 1];
              }
            value = 0;
            if (pEl != NULL)
              {
                /* Cherche le rang de l'element trouve' parmi ses freres */
                if (pEl->ElPrevious == NULL && pEl->ElNext == NULL)
                  /* l'element dont on veut le rang n'a pas de frere... */
                  if (pEl->ElParent != NULL)
                    /* ... mais il a un pere... */
                    if (pEl->ElParent->ElStructSchema->SsRule->SrElem[pEl->ElParent->ElTypeNumber - 1]->SrConstruct
                        == CsChoice)
                      /* ... et son pere est un choix */
                      /* on prendra le rang du pere */
                      pEl = pEl->ElParent;
                pAttr = NULL;
                pElReinit = NULL;
                if (pCo1->CnItem[0].CiReinitAttr != 0)
                  /* le compteur est reinitialise' par un attribut */
                  /* cherche si l'element ou un de ses freres precedents */
                  /* porte cet attribut */
                  {
                    pElReinit = pEl;
                    while (pElReinit != NULL && pAttr == NULL)
                      {
                        pAttr = pElReinit->ElFirstAttr;
                        stop = FALSE;	/* parcourt les attributs de l'element */
                        do
                          if (pAttr == NULL)
                            stop = TRUE;	/* dernier attribut */
                          else if (pAttr->AeAttrNum == pCo1->CnItem[0].CiReinitAttr &&
                                   !strcmp (pAttr->AeAttrSSchema->SsName, pSS->SsName))
                            stop = TRUE;	/* c'est l'attribut cherche' */
                          else
                            pAttr = pAttr->AeNext;	/* au suivant */
                        while (!stop);
                        if (pAttr == NULL)
                          pElReinit = pElReinit->ElPrevious;
                      }
                  }
                /* on compte les elements qui precedent l'element a numeroter */
                /* en s'arretant sur l'element qui reinitialise le compteur */
                /* s'il y en a un */
                while (pEl != pElReinit)
                  {
                    if (EquivalentType (pEl, TypeRank, pSS))
                      /* on ne compte que les elements du type a compter */
                      /* if there is a condition about an attribute
                         attached to the element, check that condition */
                      if (CondAttrOK (&pCo1->CnItem[0], pEl,
                                      pElNum->ElStructSchema))
                        value++;
                    pEl = pEl->ElPrevious;
                  }
                if (pAttr != NULL)
                  /* le compteur est reinitialise' par l'attribut pAttr */
                  value += pAttr->AeAttrValue;
                else if (initattr)
                  value += valueinitattr - 1;
              }
          }
      }
    /* the counting function is SET ON, ADD ON (Thot P language) or
       counter-reset, counter-increment (CSS) */
    else
      {
        /* type or alias that resets the counter */
        TypeSet = MakeAliasTypeCount (pCo1, CntrSet, pSS);
        /* type or alias that increments the counter */
        TypeIncr = MakeAliasTypeCount (pCo1, CntrAdd, pSS);
        /* is there a condition on attributes for this counter? */
        CondAttr = FALSE;
        CondSet = NULL;
        CondIncr = NULL;
        for (i = 0; i < pCo1->CnNItems; i++)
	  {
	    if (pCo1->CnItem[i].CiCondAttr > 0)
	      CondAttr = TRUE;
	    if (pCo1->CnItem[i].CiCntrOp == CntrSet)
	      CondSet = pCo1->CnItem[i].CiCond;
	    if (pCo1->CnItem[i].CiCntrOp == CntrAdd)
	      CondIncr = pCo1->CnItem[i].CiCond;
	  }
        pSchStr = pSS;
        if (initattr)
          value = valueinitattr;

	/* Count all preceding elements of type TypeIncr */
	/* up to the first element of type TypeSet. */
	if (TypeSet <= MAX_BASIC_TYPE)
	  /* it's a basic type, ignore the schema */
	  pSchSet = NULL;
	else
	  pSchSet = pSchStr; /* schema of element that (re)sets the counter */
	if (TypeIncr <= MAX_BASIC_TYPE)
	  /* c'est un type de base, on le cherche quel que soit son schema */
	  pSchIncr = NULL;
	else
	  pSchIncr = pSchStr; /* schema of the element that increments the
				 counter */
	pEl = pElNum;
	incrVal = 0;
	if (EquivalentType (pEl, TypeIncr, pSchIncr))
	  {
	    if (!CondAttr)
	      {
		Nincr = 1;
		incrVal = GetCounterValEl (pCo1, pEl, CntrAdd, pSS);
	      }
	    else
	      /* check conditions on attributes */
	      {
		i = GetCounterItem (pCo1, CntrAdd, pSS, pEl);
		if (i >= 0)
		  if (CondAttrOK (&pCo1->CnItem[i], pEl, pSS))
		    {
		      Nincr = 1;
		      incrVal = GetCounterValEl (pCo1, pEl, CntrAdd, pSS);
		    }
	      }
	  }
	else
	  Nincr = 0;
	if (TypeSet > 0 && TypeIncr > 0)
	  do
	    {
	      pEl = BackSearchElem2Types (pEl, TypeSet, TypeIncr, pSchSet,
					  pSchIncr, NULL);
	      if (pEl)
		{
		  if (EquivalentType (pEl, TypeSet, pSchSet))
		    /* we have found an element that (re)sets the counter */
		    {
		      if (!CondAttr)
			{
			  if (!CondSet ||
			      (CondSet &&
			       CondPresentation (CondSet, pEl, NULL, NULL,
						 NULL, view, pSS, pDoc)))
			    {
			      if (!initattr)
				value = GetCounterValEl (pCo1, pEl, CntrSet,
							 pSS);
			      pEl = NULL;	/* stop */
			    }
			}
		      else
			/* check conditions on attributes */
			{
			  i = GetCounterItem (pCo1, CntrSet, pSS, pEl);
			  if (i >= 0)
			    if (CondAttrOK (&pCo1->CnItem[i], pEl, pSS))
			      {
				if (!initattr)
				  value = GetCounterValEl (pCo1, pEl, CntrSet, pSS);
				pEl = NULL;	/* stop */
			      }
			}
		    }
		  if (pEl && EquivalentType (pEl, TypeIncr, pSchIncr))
		    /* we have found an element that increments the counter */
		    {
		      if (!CondAttr)
			{
			  if (!CondIncr ||
			      (CondIncr &&
			       CondPresentation (CondIncr, pEl, NULL, NULL,
						 NULL, view, pSS, pDoc)))
			    {
			      Nincr++;
			      if (incrVal == 0)
				incrVal = GetCounterValEl (pCo1, pEl, CntrAdd,
							   pSS);
			    }
			}
		      else
			/* check conditions on attributes */
			{
			  i = GetCounterItem (pCo1, CntrAdd, pSS, pEl);
			  if (i >= 0)
			    if (CondAttrOK (&pCo1->CnItem[i], pEl, pSS))
			      {
				Nincr++;
				if (incrVal == 0)
				  incrVal = GetCounterValEl (pCo1, pEl, CntrAdd,
							     pSS);
			      }
			}
		    }
		}
	    }
	  while (pEl);
	value += Nincr * incrVal;

        ReleaseAliasTypeCount (TypeSet, pSS);
        ReleaseAliasTypeCount (TypeIncr, pSS);
      }

  if (value < 0)
    value = 0;

  return value;
}

/*----------------------------------------------------------------------
  PresAbsBoxUserEditable    retourne vrai si le pave pAb est un    
  pave de presentation dont le contenu peut etre modifie' 
  par l'utilisateur.                                      
  ----------------------------------------------------------------------*/
ThotBool PresAbsBoxUserEditable (PtrAbstractBox pAb)
{
  ThotBool            result;
  PresentationBox    *pBo;
  PresVariable       *pPr;

  result = FALSE;
  if (pAb != NULL)
    if (pAb->AbPresentationBox && pAb->AbTypeNum > 0)
      /* c'est un pave de presentation */
      {
        /* cherche la boite de presentation correspondant au pave' */
        pBo = pAb->AbPSchema->PsPresentBox->PresBox[pAb->AbTypeNum - 1];
        if (pBo->PbContent == ContVariable)
          /* une boite dont le contenu est une variable de presentation */
          {
            pPr = pAb->AbPSchema->PsVariable->PresVar[pBo->PbContVariable - 1];
            if (pPr->PvNItems == 1)
              /* la variable n'a qu'un element */
              if (pPr->PvItem[0].ViType == VarAttrValue ||
                  pPr->PvItem[0].ViType == VarNamedAttrValue)
                /* cet element est la valeur d'un attribut */
                if (pAb->AbCreatorAttr != NULL)
                  if (pAb->AbCreatorAttr->AeAttrType == AtNumAttr ||
                      pAb->AbCreatorAttr->AeAttrType == AtTextAttr)
                    /* l'attribut est numerique ou textuel */
                    /* le contenu du pave est editable */
                    result = TRUE;
          }
      }
  return result;
}

/*----------------------------------------------------------------------
  GetCounterValue	converts the value number into a character	
  string according to a given style (decimal, roman, latin, greek...).
  ----------------------------------------------------------------------*/
void GetCounterValue (int number, CounterStyle style, char *string, int *len)
{
  int                 c, i, begin, digit;
  unsigned char*      ptr;

  *len = 0;
  if (number < 0)
    {
      string[(*len)++] = '-';
      number = -number;
    }
  switch (style)
    {
    case CntDisc:
      ptr = (unsigned char*) (&string[*len]);
      *len += TtaWCToMBstring ((wchar_t) 0x25cf, &ptr);
      break;
    case CntCircle:
      ptr = (unsigned char*) (&string[*len]);
      *len += TtaWCToMBstring ((wchar_t) 0x25cb, &ptr);
      break;
    case CntSquare:
      ptr = (unsigned char*) (&string[*len]);
      *len += TtaWCToMBstring ((wchar_t) 0x25a0, &ptr);
      break;
    case CntDecimal:
    case CntZLDecimal:
      if (style == CntZLDecimal && number < 10)
        string[(*len)++] = '0';
      if (number >= 100000)
        {
          string[(*len)++] = '?';
          number = number % 100000;
        }
      if (number >= 10000)
        c = 5;
      else if (number >= 1000)
        c = 4;
      else if (number >= 100)
        c = 3;
      else if (number >= 10)
        c = 2;
      else
        c = 1;
      *len += c;
      i = *len;
      do
        {
          string[i - 1] = (char) ((int) ('0') + number % 10);
          i--;
          number = number / 10;
        }
      while (number > 0);
      break;
    case CntURoman:
    case CntLRoman:
      if (number >= 4000)
        string[(*len)++] = '?';
      else
        {
          begin = *len + 1;
          while (number >= 1000)
            {
              string[(*len)++] = 'M';
              number -= 1000;
            }
          if (number >= 900)
            {
              string[(*len)++] = 'C';
              string[(*len)++] = 'M';
              number -= 900;
            }
          else if (number >= 500)
            {
              string[(*len)++] = 'D';
              number -= 500;
            }
          else if (number >= 400)
            {
              string[(*len)++] = 'C';
              string[(*len)++] = 'D';
              number -= 400;
            }
          while (number >= 100)
            {
              string[(*len)++] = 'C';
              number -= 100;
            }
          if (number >= 90)
            {
              string[(*len)++] = 'X';
              string[(*len)++] = 'C';
              number -= 90;
            }
          else if (number >= 50)
            {
              string[(*len)++] = 'L';
              number -= 50;
            }
          else if (number >= 40)
            {
              string[(*len)++] = 'X';
              string[(*len)++] = 'L';
              number -= 40;
            }
          while (number >= 10)
            {
              string[(*len)++] = 'X';
              number -= 10;
            }
          if (number >= 9)
            {
              string[(*len)++] = 'I';
              string[(*len)++] = 'X';
              number -= 9;
            }
          else if (number >= 5)
            {
              string[(*len)++] = 'V';
              number -= 5;
            }
          else if (number >= 4)
            {
              string[(*len)++] = 'I';
              string[(*len)++] = 'V';
              number -= 4;
            }
          while (number >= 1)
            {
              string[(*len)++] = 'I';
              number--;
            }
          if (style == CntLRoman)
            /* UPPERCASE --> lowercase */
            for (i = begin; i <= *len; i++)
              if (string[i - 1] != '?')
                string[i - 1] = (char) ((int) (string[i - 1]) + 32);
        }
      break;
    case CntUppercase:
    case CntLowercase:
      if (number > 475354)
        {
          string[(*len)++] = '?';
          number = number % 475254;
        }
      if (number > 18278)
        c = 4;
      else if (number > 702)
        c = 3;
      else if (number > 26)
        c = 2;
      else
        c = 1;
      *len += c;
      i = *len;
      do
        {
          number --;
          if (style == CntUppercase)
            string[i - 1] = (char) ((number % 26) + (int) ('A'));
          else
            string[i - 1] = (char) ((number % 26) + (int) ('a'));
          i --;
          c --;
          number = number / 26;
        }
      while (c > 0);
      break;
    case CntLGreek:
    case CntUGreek:
      if (number > 346200)
        {
          string[(*len)++] = '?';
          number = number % 346200;
        }
      if (number > 14424)
        c = 4;
      else if (number > 600)
        c = 3;
      else if (number > 24)
        c = 2;
      else
        c = 1;
      *len += c * 2;
      i = *len;
      do
        {
          number --;
          digit = number % 24;
          /* skip final sigma */
          if (digit >= 17)
            digit++;
          i -= 2;
          ptr = (unsigned char*) (&string[i]);
          if (style == CntUGreek)
            TtaWCToMBstring ((wchar_t) (digit + 0x0391), &ptr);
          else
            TtaWCToMBstring ((wchar_t) (digit + 0x03b1), &ptr);
          c --;
          number = number / 24;
        }
      while (c > 0);
      string[*len] = EOS;
      (*len)++;
      break;
    case CntNone:
      break;
    default:
      break;
    }
  string[*len] = EOS;
}

/*----------------------------------------------------------------------
  NewVariable met dans le pave pAb le texte correspondant a la
  variable de numero varNum definie dans le schema de presentation pSchP
  (et qui correspond au schema de structure pSS).
  Si pAttr n'est pas nul, c'est pour cet attribut qu'on calcule la variable.
  pDoc pointe sur le descripteur du document pour lequel on travaille.
  Si le pave avait deja un contenu et que ce contenu ne change pas la      
  fonction retourne 'faux'. S'il y a un nouveau contenu, elle retourne 'vrai'.
  ----------------------------------------------------------------------*/
ThotBool NewVariable (int varNum, PtrSSchema pSS, PtrPSchema pSchP,
                      PtrAbstractBox pAb, PtrAttribute pAttr, PtrDocument pDoc)
{
  int                 f, l;
  int                 i;
  time_t              tod;
  time_t             *pt;
  struct tm          *ptm;
  PtrTextBuffer       isOld, isNew;
  PtrAttribute        pA;
  ThotBool            found;
  ThotBool            equal;
  PresVariable       *pPr1;
  PresVarItem        *pVa1;
  PresConstant       *pPres1;
  PtrTtAttribute      pAttr1;
  PtrElement          pEl;
  Counter            *pCo1;
  char                number[20];
  PtrTextBuffer       pBTN, pBTA, pBTAPrec;

  if (varNum <= 0)
    return FALSE;
  /* sauve temporairement le contenu de ce pave de presentation */
  isOld = pAb->AbText;
  /* acquiert un buffer de texte pour y calculer la (nouvelle) valeur */
  /* de la variable */
  GetConstantBuffer (pAb);
  pAb->AbVolume = 0;
  pAb->AbLeafType = LtText;
  pA = NULL;
  /* remplit le buffer avec le contenu defini par la variable */
  pPr1 = pSchP->PsVariable->PresVar[varNum - 1];
  for (f = 1; f <= pPr1->PvNItems; f++)
    /* boucle sur les elements de la variable */
    {
      pVa1 = &pPr1->PvItem[f - 1];
      switch (pVa1->ViType)
        {
          /* traitement specifique selon le type de l'element de variable */
        case VarText:
          /* constante textuelle */
          pPres1 = &pSchP->PsConstant[pVa1->ViConstant - 1];
          CopyStringToBuffer ((unsigned char *)pPres1->PdString,
                              pAb->AbText, &l);
          pAb->AbVolume += l;
          pAb->AbLang = 0; /* default */
          break;

        case VarAttrValue:
        case VarNamedAttrValue:
          /* valeur d'un attribut */
          found = FALSE;
          if (pVa1->ViAttr == 0)
            /* Content: AttributeValue */
            {
              if (pAttr)
                {
                  pA = pAttr;
                  found = TRUE;
                }
            }
          else
            {
              /* cherche si l'element auquel se rapporte le pave (ou l'un */
              /* de ses ascendants) possede cet attribut */
              pEl = pAb->AbElement;
              while (!found && pEl != NULL)
                {
                  pA = pEl->ElFirstAttr;	/* premier attribut */
                  while (!found && pA != NULL)
                    if (pVa1->ViType == VarAttrValue &&
                        pA->AeAttrNum == pVa1->ViAttr &&
                        !strcmp (pA->AeAttrSSchema->SsName, pSS->SsName))
                      found = TRUE;
                    else if (pVa1->ViType == VarNamedAttrValue &&
                             pSchP->PsConstant[pVa1->ViConstant - 1].PdString &&
                             !strcmp (pA->AeAttrSSchema->SsAttribute->TtAttr[pA->AeAttrNum - 1]->AttrName,
                                      pSchP->PsConstant[pVa1->ViConstant - 1].PdString) &&
                             !strcmp (pA->AeAttrSSchema->SsName, pSS->SsName))
                      found = TRUE;
                    else
                      pA = pA->AeNext;
                  if (!found)
                    {
                      if (pVa1->ViType == VarAttrValue)
                        /* passe a l'element ascendant */
                        pEl = pEl->ElParent;
                      else
                        /* for VarNamedAttrValue looks only at the element
                           itself */
                        pEl = NULL;
                    }
                }
            }
          if (found)
            /* l'element possede l'attribut */
            {
              switch (pA->AeAttrType)
                {
                case AtNumAttr:
                  /* traduit l'entier en ASCII selon le style voulu */
                  GetCounterValue (pA->AeAttrValue, pVa1->ViStyle, number, &l);
                  CopyStringToBuffer ((unsigned char *)number, pAb->AbText, &l);
                  pAb->AbVolume += l;
                  pAb->AbCreatorAttr = pA;
                  break;
                case AtTextAttr:
                  if (pA->AeAttrText != NULL)
                    {
                      CopyTextToText (pA->AeAttrText,
                                      pAb->AbText, &l);
                      pAb->AbVolume += l;
                    }
                  pAb->AbCreatorAttr = pA;
                  break;
                case AtReferenceAttr:
                  CopyStringToBuffer ((unsigned char *)"REF", pAb->AbText, &l);
                  pAb->AbVolume += l;
                  break;
                case AtEnumAttr:
                  pAttr1 = pSS->SsAttribute->TtAttr[pA->AeAttrNum - 1];
                  CopyStringToBuffer ((unsigned char *)pAttr1->AttrEnumValue[pA->AeAttrValue-1],
                                      pAb->AbText, &l);
                  pAb->AbVolume += l;
                  pAb->AbCreatorAttr = pA;
                  break;
                }
              /* end case AttrType */
            }
          break;

        case VarCounter:
          /* valeur d'un compteur */
          pCo1 = &pSchP->PsCounter[pVa1->ViCounter - 1];
          if (pVa1->ViCounterVal == CntMaxVal)
            /* on cherche la valeur maximale du compteur */
            i = CounterValMinMax (pVa1->ViCounter, pSS, pSchP, pAb->AbElement,
                                  pDoc->DocView[pAb->AbDocView - 1].DvPSchemaView, TRUE);
          else if (pVa1->ViCounterVal == CntMinVal)
            /* on cherche la valeur minimale du compteur */
            i = CounterValMinMax (pVa1->ViCounter, pSS, pSchP,
                                  pAb->AbElement,
                                  pDoc->DocView[pAb->AbDocView - 1].DvPSchemaView,
                                  FALSE);
          else
            /* valeur courante du compteur */
            i = CounterVal (pVa1->ViCounter, pSS, pSchP, pAb->AbElement,
                            pDoc->DocView[pAb->AbDocView - 1].DvPSchemaView,
			    pDoc);
          /* le cas particulier des compteurs en bas de page (ou il */
          /* fallait decrementer la valeur) est supprime dans V4 car  */
          /* le bas de page est associe a la page courante et non la */
          /* page suivante comme avant */
          if (pCo1->CnPageFooter)
            /* c'est un compteur qui s'affiche en bas de page */
            if (pAb->AbElement->ElTerminal
                && pAb->AbElement->ElLeafType == LtPageColBreak)
              {
                /* on calcule sa valeur pour une marque de page, on */
                /* reduit la valeur du compteur */
                if (pCo1->CnItem[0].CiCntrOp == CntrRank)
                  i--;
                else
                  i -= pCo1->CnItem[1].CiParamValue;
              }
          /* traduit l'entier en ASCII */
          GetCounterValue (i, pVa1->ViStyle, number, &l);
          CopyStringToBuffer ((unsigned char *)number, pAb->AbText, &l);
          pAb->AbVolume += l;
          break;

        case VarDate:
          /* date en anglais */
          pt = &tod;
          *pt = time (NULL);
          ptm = localtime (pt);
          GetCounterValue (ptm->tm_year, CntDecimal, number, &l);
          CopyStringToBuffer ((unsigned char *)number, pAb->AbText, &l);
          pAb->AbVolume += l;
          CopyStringToBuffer ((unsigned char *)"/", pAb->AbText, &l);
          pAb->AbVolume += l;
          GetCounterValue (ptm->tm_mon + 1, CntDecimal, number, &l);
          CopyStringToBuffer ((unsigned char *)number, pAb->AbText, &l);
          pAb->AbVolume += l;
          CopyStringToBuffer ((unsigned char *)"/", pAb->AbText, &l);
          pAb->AbVolume += l;
          GetCounterValue (ptm->tm_mday, CntDecimal, number, &l);
          CopyStringToBuffer ((unsigned char *)number, pAb->AbText, &l);
          pAb->AbVolume += l;
          break;

        case VarFDate:
          /* date en francais */
          pt = &tod;
          *pt = time (NULL);
          ptm = localtime (pt);
          GetCounterValue (ptm->tm_mday, CntDecimal, number, &l);
          CopyStringToBuffer ((unsigned char *)number, pAb->AbText, &l);
          pAb->AbVolume += l;
          CopyStringToBuffer ((unsigned char *)"/", pAb->AbText, &l);
          pAb->AbVolume += l;
          GetCounterValue (ptm->tm_mon + 1, CntDecimal, number, &l);
          CopyStringToBuffer ((unsigned char *)number, pAb->AbText, &l);
          pAb->AbVolume += l;
          CopyStringToBuffer ((unsigned char *)"/", pAb->AbText, &l);
          pAb->AbVolume += l;
          GetCounterValue (ptm->tm_year, CntDecimal, number, &l);
          CopyStringToBuffer ((unsigned char *)number, pAb->AbText, &l);
          pAb->AbVolume += l;
          break;

        case VarDocName:
          /* Name du document */
          CopyStringToBuffer ((unsigned char *)pDoc->DocDName, pAb->AbText, &l);
          pAb->AbVolume += l;
          break;

        case VarDirName:
          /* Name du document */
          CopyStringToBuffer ((unsigned char *)pDoc->DocDirectory, pAb->AbText, &l);
          pAb->AbVolume += l;
          break;

        case VarElemName:
          /* Name de l'element */
          pEl = pAb->AbElement;
          CopyStringToBuffer ((unsigned char *)pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber - 1]->SrName, pAb->AbText, &l);
          pAb->AbVolume += l;
#ifdef IV	    
          /* Show the namespace declaration for the root element and */
          /* when an element is not in the same namespace than its parent */
          if (pEl->ElStructSchema->SsRootElem == pEl->ElTypeNumber ||
              (pEl->ElParent && pEl->ElStructSchema != pEl->ElParent->ElStructSchema))
            {
              uri = GiveCurrentNsUri (pDoc, pEl);
              if (uri)
                {
                  CopyStringToBuffer ((unsigned char *)" xmlns=\"", pAb->AbText, &l);
                  pAb->AbVolume += l;
                  CopyStringToBuffer ((unsigned char *)uri, pAb->AbText, &l);
                  pAb->AbVolume += l;
                  CopyStringToBuffer ((unsigned char *)"\"", pAb->AbText, &l);
                  pAb->AbVolume += l;
                }
            }
#endif
          break;

        case VarAttrName:
          /* Nom de l'attribut */
          CopyStringToBuffer ((unsigned char *)pAttr->AeAttrSSchema->SsAttribute->TtAttr[pAttr->AeAttrNum - 1]->AttrName, pAb->AbText, &l);
          pAb->AbVolume += l;
          break;

        case VarPageNumber:
          /* numero de la marque de page precedente dans la */
          /* view ViView */
          /* cherche en arriere la premiere marque de page de cette view */
          pEl = pAb->AbElement;
          found = FALSE;
          do
            {
              pEl = BackSearchTypedElem (pEl, PageBreak + 1, NULL, NULL);
              if (pEl != NULL &&
                  /* on ignore les pages qui ne concernent pas la view */
                  pEl->ElViewPSchema == pVa1->ViView)
                found = TRUE;
            }
          while ((!found) && (pEl != NULL));
          if (pEl == NULL)
            i = 1;	/* pas trouve', on considere que c'est la page 1 */
          else
            i = pEl->ElPageNumber;	/* numero de la page trouvee */
          /* traduit le numero de page en ASCII selon le style voulu */
          GetCounterValue (i, pVa1->ViStyle, number, &l);
          CopyStringToBuffer ((unsigned char *)number, pAb->AbText, &l);
          pAb->AbVolume += l;
          break;

        default:
          break;
        }
    }
  /* termine la mise a jour du pave */
  pAb->AbVarNum = varNum;
  if (PresAbsBoxUserEditable (pAb))
    /* le contenu de ce pave de presentation est donc modifiable */
    pAb->AbCanBeModified = TRUE;
  if (isOld == NULL)
    equal = FALSE;		/* la variable n'avait pas de valeur */
  else
    {
      isNew = pAb->AbText;	/* isNew contenu du pave de presentation */
      /* compare le isNew et l'ancien contenu du pave */
      equal = TextsEqual (isOld, isNew);
      if (!equal)
        /* contenus differents */
        /* recopie le isNew contenu dans les anciens buffers */
        {
          pBTA = isOld;
          pBTAPrec = NULL;
          pBTN = isNew;
          while (pBTN != NULL)
            {
              if (pBTA == NULL)
                pBTA = NewTextBuffer (pBTAPrec);
              ustrcpy (pBTA->BuContent, pBTN->BuContent);
              pBTA->BuLength = pBTN->BuLength;
              pBTN = pBTN->BuNext;
              pBTAPrec = pBTA;
              pBTA = pBTA->BuNext;
            }
        }
      pAb->AbText = isOld;
      /* libere les buffers nouveaux */
      do
        {
          pBTN = isNew->BuNext;
          FreeTextBuffer (isNew);
          isNew = pBTN;
        }
      while (isNew != NULL);
    }
  if (!equal)
    /* reaffiche les references qui copient cette variable */
    RedispAllReferences (pAb, pDoc);
  return !equal;
}
