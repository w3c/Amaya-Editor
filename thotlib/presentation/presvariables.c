/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Warning:
 * This module is part of the Thot library, which was originally
 * developed in French. That's why some comments are still in
 * French, but their translation is in progress and the full module
 * will be available in English in the next release.
 * 
 */
 
/*
 * Manipulation des variables de presentation pour
 * les images abstraites des documents.
 *
 * Author: V. Quint (INRIA)
 *
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "fileaccess.h"
#include "language.h"

#include "tree_f.h"
#include "memory_f.h"
#include "changeabsbox_f.h"
#include "presrules_f.h"
#include "fileaccess_f.h"
#include "structschema_f.h"
#include "content_f.h"
#include "presvariables_f.h"


/*----------------------------------------------------------------------
   MakeAliasTypeCount cree un alias temporaire qui va servir a faire 
   une recherche multiple sur les elements du compteur     
   on cree l'alias  MAX_RULES_SSCHEMA  pour les elements reset   
   du compteur.  On cree l'alias  MAX_RULES_SSCHEMA + 1 pour les 
   elements add du compteur.                               
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static int          MakeAliasTypeCount (Counter * pCounter, CounterOp op, PtrSSchema pSchStr)

#else  /* __STDC__ */
static int          MakeAliasTypeCount (pCounter, op, pSchStr)
Counter            *pCounter;
CounterOp           op;
PtrSSchema          pSchStr;

#endif /* __STDC__ */

{
   SRule              *NewAlias;
   int                 i;

   if (op == CntrSet)
      NewAlias = &pSchStr->SsRule[MAX_RULES_SSCHEMA];
   else if (op == CntrAdd)
      NewAlias = &pSchStr->SsRule[MAX_RULES_SSCHEMA + 1];
   else				/* on s'est trompe c'est un CntrRank */
      return pCounter->CnItem[0].CiElemType;

   /* initialise l'alias temporaire */
   ustrcpy (NewAlias->SrName, TEXT("Counter alias"));
   NewAlias->SrNDefAttrs = 0;
   NewAlias->SrNLocalAttrs = 0;
   NewAlias->SrAssocElem = FALSE;
   NewAlias->SrAssocElem = FALSE;
   NewAlias->SrParamElem = FALSE;
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

   if (op == CntrSet)
      return MAX_RULES_SSCHEMA + 1;
   else				/* op == CntrAdd */
      return MAX_RULES_SSCHEMA + 2;
}


/*----------------------------------------------------------------------
   GetCounterValEl renvoie la valeur numerique de set ou d'increment 
   associee a l'element pEl.                               
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static int          GetCounterValEl (Counter * pCounter, PtrElement pEl, CounterOp op, PtrSSchema pSchStr)

#else  /* __STDC__ */
static int          GetCounterValEl (pCounter, pEl, op, pSchStr)
Counter            *pCounter;
PtrElement          pEl;
CounterOp           op;
PtrSSchema          pSchStr;

#endif /* __STDC__ */

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
#ifdef __STDC__
static ThotBool     InitCounterByAttribute (int *valinit, Counter * pCo, PtrElement pElNum, PtrSSchema pSS)

#else  /* __STDC__ */
static ThotBool     InitCounterByAttribute (valinit, pCo, pElNum, pSS)
int                *valinit;
Counter            *pCo;
PtrElement          pElNum;
PtrSSchema          pSS;

#endif /* __STDC__ */

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
			 pAttr->AeAttrSSchema->SsCode == pSS->SsCode)
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
#ifdef __STDC__
static ThotBool     CondAttrOK (CntrItem * pCountItem, PtrElement pEl, PtrSSchema pSS)

#else  /* __STDC__ */
static ThotBool     CondAttrOK (pCountItem, pEl, pSS)
CntrItem            *pCountItem;
PtrElement          pEl;
PtrSSchema          pSS;

#endif /* __STDC__ */

{
   ThotBool            result, stop;
   PtrAttribute        pAttr;

   if (pCountItem->CiCondAttr == 0)
      /* there is no condition about attribute in this counter item */
      return TRUE;

   /* cet attribut est-il present sur l'element courant */
   pAttr = pEl->ElFirstAttr;
   stop = FALSE;	/* parcourt les attributs de l'element */
   do
	if (pAttr == NULL)
	   stop = TRUE;	/* dernier attribut */
	else if (pAttr->AeAttrNum == pCountItem->CiCondAttr &&
		 pAttr->AeAttrSSchema->SsCode == pSS->SsCode)
	   stop = TRUE;	/* c'est l'attribut cherche' */
	else
	   pAttr = pAttr->AeNext;	/* au suivant */
   while (!stop);
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
#ifdef __STDC__
static int      GetCounterItem (Counter * pCounter, CounterOp cntrOp, PtrSSchema pSS, PtrElement pEl)

#else  /* __STDC__ */
static int      GetCounterItem (pCounter, cntrOp, pSS, pEl)
Counter            *pCounter;
CounterOp	    cntrOp;
PtrSSchema          pSS;
PtrElement          pEl;

#endif /* __STDC__ */

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

#ifdef __STDC__
int                 CounterValMinMax (int counterNum, PtrSSchema pSS, PtrPSchema pSchP, PtrElement pElNum, int view, ThotBool Maximum)

#else  /* __STDC__ */
int                 CounterValMinMax (counterNum, pSS, pSchP, pElNum, view, Maximum)
int                 counterNum;
PtrSSchema          pSS;
PtrPSchema          pSchP;
PtrElement          pElNum;
int                 view;
ThotBool            Maximum;

#endif /* __STDC__ */

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
		  if (pCo1->CnItem[0].CiViewNum == 0 && pElNum->ElAssocNum > 0)
		     view = pElNum->ElViewPSchema;
		  /* cherche la marque de page qui precede l'element */
		  stop = FALSE;
		  do
		    {
		       pEl = BackSearchTypedElem (pEl, TypeRank, NULL);
		       if (pEl == NULL)
			  /* pas de marque de page precedente */
			  stop = TRUE;
		       else
			 {
			    if (pElNum->ElStructSchema->SsCode != pSSpr->SsCode)
			       /* Saut de page d'une view squelette */
			       /* on ne que compte les pages etrangeres au schema */
			       /* principal */
			       pstop = pEl->ElStructSchema->SsCode !=
				  pSSpr->SsCode;
			    else
			       pstop = pEl->ElStructSchema->SsCode ==
				  pSSpr->SsCode;

			    /* on ignore les pages qui ne concernent pas la view */
			    if ((pEl->ElViewPSchema == view) && (pstop))
			       stop = TRUE;
			 }
		    }
		  while (!stop);

		  if (pEl == NULL)	/* pas de marque de page precedente */
		     if (initattr)
			value = valueinitattr;
		     else
			value = 0;
		  else
		     /* on prend pour valeur le numero de la marque de page */
		     /* precedente */
		  if ((pEl->ElViewPSchema == view) && (pEl->ElPageNumber > value))
		     value = pEl->ElPageNumber;
		  /* A partir de cette marque de page, cherche en avant les */
		  /* suivantes jusqu'a ne plus en trouver */
		  while (pEl != NULL)
		    {
		       pEl = FwdSearchTypedElem (pEl, TypeRank, NULL);
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
			     if (pEl->ElParent->ElStructSchema->SsRule[pEl->ElParent->
				  ElTypeNumber - 1].SrConstruct == CsChoice)
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

	/* Cherche le premier element de type TypeSet */
	/* englobant l'element a numeroter */
	pEl = GetTypedAncestor (pElNum, TypeSet, pSS);
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
		   pEl = GetTypedAncestor (pEl, TypeSet, pSS);
	     }
	  while (!found && pEl != NULL);
	  }
	/* s'il n' y a pas d'ascendant du type requis alors on reste sur pElNum */
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
	     /* On veut la valeur maximale du compteur     */
	     /* a partir de l'element trouve', cherche en avant tous les */
	     /* elements ayant le type qui incremente le compteur, */
	     /* jusqu'a rencontrer un autre element qui reset le compteur */
	     if (TypeIncr <= MAX_BASIC_TYPE)
		/* c'est un type de base, on le cherche quel que soit son schema */
		pSchIncr = NULL;
	     else
		pSchIncr = pSchStr; /* schema de struct. du type qui incremente */
	     if (TypeIncr > 0)
		do
		  {
		     pEl = FwdSearchElem2Types (pEl, TypeIncr, pElNum->ElTypeNumber, pSchIncr,
						pElNum->ElStructSchema);
		     if (pEl != NULL)
			if (EquivalentType (pEl, TypeIncr, pSchIncr))
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
		while (pEl != NULL && !EquivalentType (pEl, TypeSet, pSS));
	  }
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
#ifdef __STDC__
int                 CounterVal (int counterNum, PtrSSchema pSS, PtrPSchema pSchP, PtrElement pElNum, int view)
#else  /* __STDC__ */
int                 CounterVal (counterNum, pSS, pSchP, pElNum, view)
int                 counterNum;
PtrSSchema          pSS;
PtrPSchema          pSchP;
PtrElement          pElNum;
int                 view;
#endif /* __STDC__ */

{
   int                 i, value, valueinitattr, level, Nincr, incrVal;
   int                 TypeIncr, TypeSet, TypeRank, TypeRLevel;
   PtrSSchema          pSchIncr, pSchSet;
   ThotBool            stop, pstop, initattr;
   PtrElement          pEl;
   Counter            *pCo1;
   PtrElement          pEl2;
   PtrSSchema          pSchStr, pSSpr;
   PtrAttribute        pAttr;
   PtrElement          pElReinit;
   ThotBool	       CondAttr, found;
#define MaxAncestor 50
   PtrElement          PcWithin[MaxAncestor];

   pCo1 = &pSchP->PsCounter[counterNum - 1];
   initattr = FALSE;		/* a priori, la valeur initiale du compteur ne
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
		 pEl->ElStructSchema->SsCode == pElNum->ElStructSchema->SsCode)
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
	     if (pCo1->CnItem[0].CiViewNum == 0 && pElNum->ElAssocNum > 0)
		view = pElNum->ElViewPSchema;
	     /* cherche la marque de page qui precede l'element */
	     stop = FALSE;
	     do
	       {
		  pEl = BackSearchTypedElem (pEl, TypeRank, NULL);
		  if (pEl == NULL)
		     /* pas de marque de page precedente */
		     stop = TRUE;
		  else
		    {
		       if (pElNum->ElStructSchema->SsCode != pSSpr->SsCode)
			  /* Saut de page d'une view squelette */
			  /*on ne que compte les pages etrangeres au schema principal */
			  pstop = pEl->ElStructSchema->SsCode != pSSpr->SsCode;
		       else
			  pstop = pEl->ElStructSchema->SsCode == pSSpr->SsCode;

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
			   pEl->ElStructSchema->SsCode ==
			   pElNum->ElStructSchema->SsCode)
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
			if (pEl->ElParent->ElStructSchema->SsRule[pEl->ElParent->ElTypeNumber - 1].SrConstruct
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
				pAttr->AeAttrSSchema->SsCode == pSS->SsCode)
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
   /* fonction de comptage SET ON, ADD ON */
   else
     {
	/* type ou alias qui fait reset */
	TypeSet = MakeAliasTypeCount (pCo1, CntrSet, pSS);
	/* type ou alias qui incremente */
	TypeIncr = MakeAliasTypeCount (pCo1, CntrAdd, pSS);
	/* is there a condition on attributes for this counter? */
	CondAttr = FALSE;
	for (i = 0; i < pCo1->CnNItems && !CondAttr; i++)
	   if (pCo1->CnItem[i].CiCondAttr > 0)
	      CondAttr = TRUE;
	pSchStr = pSS;
	if (initattr)
	   value = valueinitattr;

	/* Cherche le premier element de type TypeSet englobant l'element */
	/* a numeroter */
	pEl = GetTypedAncestor (pElNum, TypeSet, pSS);
	if (CondAttr && pEl)
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
		   pEl = GetTypedAncestor (pEl, TypeSet, pSS);
	     }
	  while (!found && pEl != NULL);
	  }
	if (pEl != NULL)
	  {
	  /* l'element trouve' est celui qui reinitialise le compteur */
	  if (!initattr)
	      value = GetCounterValEl (pCo1, pEl, CntrSet, pSS);
	  /* a partir de l'element trouve', cherche en avant tous les */
	  /* elements ayant le type qui incremente le compteur, */
	  /* jusqu'a rencontrer l'element qui a cree la boite compteur. */
	  if (TypeIncr <= MAX_BASIC_TYPE)
	     /* c'est un type de base, on le cherche quel que soit son schema*/
	     pSchIncr = NULL;
	  else
	     pSchIncr = pSchStr; /* schema de struct. du type qui incremente */
	  if (TypeIncr > 0)
	     do
	       {
		  pEl = FwdSearchElem2Types (pEl, TypeIncr,
					     pElNum->ElTypeNumber, pSchIncr,
					     pElNum->ElStructSchema);
		  if (pEl != NULL)
		     if (EquivalentType (pEl, TypeIncr, pSchIncr))
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
	     while (pEl != NULL && pEl != pElNum);
	  }

	else
	  /* il n' y a pas d'ascendant du type qui initialise le compteur. */
	  /* On compte tous les elements precedents de type TypeIncr */
	  /* jusqu'a en trouver un de type TypeSet */
	  {
	  if (TypeSet <= MAX_BASIC_TYPE)
	     /* c'est un type de base, on le cherche quel que soit son schema*/
	     pSchSet = NULL;
	  else
	     pSchSet = pSchStr; /* schema de struct. du type qui initialise */
	  if (TypeIncr <= MAX_BASIC_TYPE)
	     /* c'est un type de base, on le cherche quel que soit son schema*/
	     pSchIncr = NULL;
	  else
	     pSchIncr = pSchStr; /* schema de struct. du type qui incremente */
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
					      pSchIncr);
		  if (pEl != NULL)
		     if (EquivalentType (pEl, TypeIncr, pSchIncr))
			/* on a trouve' un element du type qui incremente */
			{
			if (!CondAttr)
			   {
			   Nincr++;
			   if (incrVal == 0)
			      incrVal = GetCounterValEl (pCo1, pEl, CntrAdd, pSS);
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
				    incrVal = GetCounterValEl (pCo1, pEl, CntrAdd, pSS);
				 }
			   }
			}
		     else if (EquivalentType (pEl, TypeSet, pSchSet))
			/* on a trouve' un element du type qui initialise */
			{
			if (!CondAttr)
			   {
			   if (!initattr)
			     value = GetCounterValEl (pCo1, pEl, CntrSet, pSS);
			   pEl = NULL;	/* on arrete */
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
				 pEl = NULL;	/* on arrete */
				 }
			   }
			}
	       }
	     while (pEl != NULL);
	  value += Nincr * incrVal;
	  }
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
#ifdef __STDC__
ThotBool            PresAbsBoxUserEditable (PtrAbstractBox pAb)
#else  /* __STDC__ */
ThotBool            PresAbsBoxUserEditable (pAb)
PtrAbstractBox      pAb;

#endif /* __STDC__ */
{
   ThotBool            result;
   PresentationBox    *pBo;
   PresVariable       *pPr;

   result = FALSE;
   if (pAb != NULL)
      if (pAb->AbPresentationBox)
	 /* c'est un pave de presentation */
	{
	   /* cherche la boite de presentation correspondant au pave' */
	   pBo = &pAb->AbPSchema->PsPresentBox[pAb->AbTypeNum - 1];
	   if (pBo->PbContent == ContElement)
	      /* une boite qui regroupe des elements associes */
	      /* son contenu est modifiable */
	      result = TRUE;
	   else if (pBo->PbContent == ContVariable)
	      /* une boite dont le contenu est une variable de presentation */
	     {
		pPr = &pAb->AbPSchema->PsVariable[pBo->PbContVariable - 1];
		if (pPr->PvNItems == 1)
		   /* la variable n'a qu'un element */
		   if (pPr->PvItem[0].ViType == VarAttrValue)
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
   NewVariable met dans le pave pAb le texte correspondant a la  
   variable de numero varNum definie dans le schema de       
   presentation pSchP (et qui correspond au schema de      
   structure pSS). pDoc pointe sur le descripteur du       
   document pour lequel on travaille. Si le pave avait     
   deja un contenu et que ce contenu ne change pas la      
   fonction retourne 'faux'. S'il y a un isNew contenu,  
   elle retourne 'vrai'.                                   
  ----------------------------------------------------------------------*/

#ifdef __STDC__
ThotBool            NewVariable (int varNum, PtrSSchema pSS, PtrPSchema pSchP,
				 PtrAbstractBox pAb, PtrDocument pDoc)

#else  /* __STDC__ */
ThotBool            NewVariable (varNum, pSS, pSchP, pAb, pDoc)
int                 varNum;
PtrSSchema          pSS;
PtrPSchema          pSchP;
PtrAbstractBox      pAb;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   int                 f, l;
   int                 i;
   long                tod;
   long               *pt;
   struct tm          *ptm;
   PtrTextBuffer       isOld, isNew;
   PtrAttribute        pAttr;
   ThotBool            found;
   ThotBool            equal;
   PresVariable       *pPr1;
   PresVarItem        *pVa1;
   PresConstant       *pPres1;
   TtAttribute        *pAttr1;
   PtrElement          pEl;
   Counter            *pCo1;
   CHAR_T              number[20];
   PtrTextBuffer       pBTN, pBTA, pBTAPrec;

   /* sauve temporairement le contenu de ce pave de presentation */
   isOld = pAb->AbText;
   /* acquiert un buffer de texte pour y calculer la (nouvelle) valeur */
   /* de la variable */
   GetConstantBuffer (pAb);
   pAb->AbVolume = 0;
   pAttr = NULL;
   /* remplit le buffer avec le contenu defini par la variable */
   pPr1 = &pSchP->PsVariable[varNum - 1];
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
		    CopyStringToText (pPres1->PdString, pAb->AbText, &l);
		    pAb->AbVolume += l;
		    pAb->AbLanguage = TtaGetLanguageIdFromAlphabet (pPres1->PdAlphabet);
		    break;

		 case VarAttrValue:
		    /* valeur d'un attribut */
		    /* cherche si l'element auquel se rapporte le pave (ou l'un de */
		    /* ses ascendants) possede cet attribut */
		    pEl = pAb->AbElement;
		    found = FALSE;
		    while (!found && pEl != NULL)
		      {
			 pAttr = pEl->ElFirstAttr;	/* premier attribut */
			 while (!found && pAttr != NULL)
			    if (pAttr->AeAttrNum == pVa1->ViAttr &&
				pAttr->AeAttrSSchema->SsCode == pSS->SsCode)
			       found = TRUE;
			    else
			       pAttr = pAttr->AeNext;
			 if (!found)
			    /* passe a l'element ascendant */
			    pEl = pEl->ElParent;
		      }
		    if (found)
		       /* l'element possede l'attribut */
		      {
			 switch (pAttr->AeAttrType)
			       {
				  case AtNumAttr:
				     /* traduit l'entier en ASCII selon le style voulu */
				     GetCounterValue (pAttr->AeAttrValue, pVa1->ViStyle, number, &l);
				     CopyStringToText (number, pAb->AbText, &l);
				     pAb->AbVolume += l;
				     pAb->AbCreatorAttr = pAttr;
				     break;
				  case AtTextAttr:
				     if (pAttr->AeAttrText != NULL)
				       {
					  CopyTextToText (pAttr->AeAttrText,
							  pAb->AbText, &l);
					  pAb->AbVolume += l;
				       }
				     pAb->AbCreatorAttr = pAttr;
				     break;
				  case AtReferenceAttr:
				     CopyStringToText (REF_EXT2, pAb->AbText, &l);
				     pAb->AbVolume += l;
				     break;
				  case AtEnumAttr:
				     pAttr1 = &pSS->SsAttribute[pVa1->ViAttr - 1];
				     CopyStringToText (pAttr1->AttrEnumValue[pAttr->AeAttrValue - 1],
						       pAb->AbText, &l);
				     pAb->AbVolume += l;
				     pAb->AbCreatorAttr = pAttr;
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
			   pDoc->DocView[pAb->AbDocView - 1].DvPSchemaView);
		    /* le cas particulier des compteurs en bas de page (ou il */
		    /* fallait decrementer la valeur) est supprime dans V4 car  */
		    /* le bas de page est associe a la page courante et non la */
		    /* page suivante comme avant */
		    if (pCo1->CnPageFooter)
		       /* c'est un compteur qui s'affiche en bas de page */
		       if (pAb->AbElement->ElTerminal
			   && pAb->AbElement->ElLeafType == LtPageColBreak)
			  /* on calcule sa valeur pour une marque de page, on */
			  /* reduit la valeur du compteur */
			  if (pCo1->CnItem[0].CiCntrOp == CntrRank)
			     i--;
			  else
			     i -= pCo1->CnItem[1].CiParamValue;
		    /* traduit l'entier en ASCII */
		    GetCounterValue (i, pVa1->ViStyle, number, &l);
		    CopyStringToText (number, pAb->AbText, &l);
		    pAb->AbVolume += l;
		    break;
		 case VarDate:
		    /* date en anglais */
		    pt = &tod;
		    *pt = time (NULL);
		    ptm = localtime (pt);

		    GetCounterValue (ptm->tm_year, CntArabic, number, &l);
		    CopyStringToText (number, pAb->AbText, &l);
		    pAb->AbVolume += l;
		    CopyStringToText (TEXT("/"), pAb->AbText, &l);
		    pAb->AbVolume += l;
		    GetCounterValue (ptm->tm_mon + 1, CntArabic, number, &l);
		    CopyStringToText (number, pAb->AbText, &l);
		    pAb->AbVolume += l;
		    CopyStringToText (TEXT("/"), pAb->AbText, &l);
		    pAb->AbVolume += l;
		    GetCounterValue (ptm->tm_mday, CntArabic, number, &l);
		    CopyStringToText (number, pAb->AbText, &l);
		    pAb->AbVolume += l;
		    break;
		 case VarFDate:
		    /* date en francais */
		    pt = &tod;
		    *pt = time (NULL);
		    ptm = localtime (pt);
		    GetCounterValue (ptm->tm_mday, CntArabic, number, &l);
		    CopyStringToText (number, pAb->AbText, &l);
		    pAb->AbVolume += l;
		    CopyStringToText (TEXT("/"), pAb->AbText, &l);
		    pAb->AbVolume += l;
		    GetCounterValue (ptm->tm_mon + 1, CntArabic, number, &l);
		    CopyStringToText (number, pAb->AbText, &l);
		    pAb->AbVolume += l;
		    CopyStringToText (TEXT("/"), pAb->AbText, &l);
		    pAb->AbVolume += l;
		    GetCounterValue (ptm->tm_year, CntArabic, number, &l);
		    CopyStringToText (number, pAb->AbText, &l);
		    pAb->AbVolume += l;
		    break;
		 case VarDocName:
		    /* Name du document */
		    CopyStringToText (pDoc->DocDName, pAb->AbText, &l);
		    pAb->AbVolume += l;
		    break;
		 case VarDirName:
		    /* Name du document */
		    CopyStringToText (pDoc->DocDirectory, pAb->AbText, &l);
		    pAb->AbVolume += l;
		    break;
		 case VarElemName:
		    /* Name de l'element */
		    pEl = pAb->AbElement;
		    CopyStringToText (pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].
				      SrName, pAb->AbText, &l);
		    pAb->AbVolume += l;
		    break;
		 case VarAttrName:
		    /* Name de l'attribut */
		    /* non implemente' */
		    break;
		 case VarPageNumber:
		    /* numero de la marque de page precedente dans la */
		    /* view ViView */
		    /* cherche en arriere la premiere marque de page de cette view */
		    pEl = pAb->AbElement;
		    found = FALSE;
		    do
		      {
			 pEl = BackSearchTypedElem (pEl, PageBreak + 1, NULL);
			 if (pEl != NULL)
			    /* on ignore les pages qui ne concernent pas la view */
			    if (pEl->ElViewPSchema == pVa1->ViView)
			       found = TRUE;
		      }
		    while ((!found) && (pEl != NULL));
		    if (pEl == NULL)
		       i = 1;	/* pas trouve', on considere que c'est la page 1 */
		    else
		       i = pEl->ElPageNumber;	/* numero de la page trouvee */
		    /* traduit le numero de page en ASCII selon le style voulu */
		    GetCounterValue (i, pVa1->ViStyle, number, &l);
		    CopyStringToText (number, pAb->AbText, &l);
		    pAb->AbVolume += l;
		    break;
		 default:
		    break;
	      }
	/* termine la mise a jour du pave */
     }
   if (PresAbsBoxUserEditable (pAb))
      /* le contenu de ce pave de presentation est donc modifiable */
      pAb->AbCanBeModified = TRUE;
   pAb->AbLeafType = LtText;
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
