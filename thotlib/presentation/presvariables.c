
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*
   varpres.c -- Gestion des variables de presentation.
   Manipulation des variables de presentation pour
   les images abstraites des documents.
   V. Quint     Mai 1985 
   France Logiciel no de depot 88-39-001-00
   Major changes:
   IV : Septembre 92 adaptation Tool Kit
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "storage.h"
#include "language.h"

#include "tree_f.h"
#include "memory_f.h"
#include "changeabsbox_f.h"
#include "presrules_f.h"
#include "fileaccess_f.h"
#include "structschema_f.h"
#include "content_f.h"
#include "presvariables_f.h"


/* ---------------------------------------------------------------------- */
/* |    MakeTypeCptTempo cree un alias temporaire qui va servir a faire | */
/* |            une recherche multiple sur les elements du compteur     | */
/* |            on cree l'alias  MAX_RULES_SSCHEMA  pour les elements reset   | */
/* |            du compteur.  On cree l'alias  MAX_RULES_SSCHEMA + 1 pour les | */
/* |            elements add du compteur.                               | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static int  MakeTypeCptTempo (Counter * pCompteur, CounterOp op, PtrSSchema pSchStr)

#else  /* __STDC__ */
static int  MakeTypeCptTempo (pCompteur, op, pSchStr)
Counter           *pCompteur;
CounterOp           op;
PtrSSchema        pSchStr;

#endif /* __STDC__ */

{
   SRule              *NewAlias;
   int                 i;

   if (op == CntrSet)
      NewAlias = &pSchStr->SsRule[MAX_RULES_SSCHEMA];
   else if (op == CntrAdd)
      NewAlias = &pSchStr->SsRule[MAX_RULES_SSCHEMA + 1];
   else				/* on s'est trompe c'est un CntrRank */
      return pCompteur->CnItem[0].CiElemType;

   /* initialise l'alias temporaire */
   strncpy (NewAlias->SrName, "Alias de compteur", 17);
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

   /* initialise le constructeur choix de l;alias */
   for (i = 0; i < pCompteur->CnNItems; i++)
     {
	if (pCompteur->CnItem[i].CiCntrOp == op)
	  {
	     NewAlias->SrChoice[NewAlias->SrNChoices] =
		pCompteur->CnItem[i].CiElemType;
	     NewAlias->SrNChoices += 1;
	  }
     }

   if (op == CntrSet)
      return MAX_RULES_SSCHEMA + 1;
   else				/* op == CntrAdd */
      return MAX_RULES_SSCHEMA + 2;
}


/* ---------------------------------------------------------------------- */
/* |    GetCptElemVal renvoie la valeur numerique de set ou d'increment | */
/* |            associee a l'element pEl.                               | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static int          GetCptElemVal (Counter * pCompteur, PtrElement pEl, CounterOp op, PtrSSchema pSchStr)

#else  /* __STDC__ */
static int          GetCptElemVal (pCompteur, pEl, op, pSchStr)
Counter           *pCompteur;
PtrElement          pEl;
CounterOp           op;
PtrSSchema        pSchStr;

#endif /* __STDC__ */

{
   int                 i;

   for (i = 0; i < pCompteur->CnNItems; i++)
      if (pCompteur->CnItem[i].CiCntrOp == op)
	 if (EquivalentType (pEl, pCompteur->CnItem[i].CiElemType, pSchStr)
	     || EquivalentSRules (pEl->ElTypeNumber, pEl->ElStructSchema,
			pCompteur->CnItem[i].CiElemType, pSchStr, pEl))
	    if (pCompteur->CnItem[i].CiElemType != PageBreak + 1
		|| pEl->ElViewPSchema == pCompteur->CnItem[i].CiViewNum)
	       return pCompteur->CnItem[i].CiParamValue;
   return 0;			/* par defaut... */
}


/* ---------------------------------------------------------------------- */
/* |    InitCounterByAttribute                                          | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static boolean      InitCounterByAttribute (int *valinit, Counter * pCo, PtrElement pElNum, PtrSSchema pSS)

#else  /* __STDC__ */
static boolean      InitCounterByAttribute (valinit, pCo, pElNum, pSS)
int                *valinit;
Counter           *pCo;
PtrElement          pElNum;
PtrSSchema        pSS;

#endif /* __STDC__ */

{
   boolean             ret, stop;
   PtrElement          pEl;
   PtrAttribute         pAttr;

   ret = FALSE;
   *valinit = 0;
   if (pCo->CnItem[0].CiInitAttr > 0)
     {
	/* la valeur initiale du compteur est definie par un attribut */
	pEl = pElNum;
	/* cherche si l'element a numeroter ou l'un de ses ascendants */
	/* porte l'attribut qui intialise le compteur */
	while (!ret && pEl != NULL)
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
	     while (!(stop));
	     if (pAttr != NULL)
		/* l'element porte l'attribut qui initialise le compteur */
	       {
		  ret = TRUE;
		  *valinit = pAttr->AeAttrValue;	/* on prend la valeur de l'attribut */
	       }
	     else
		/* passe a l'element ascendant */
		pEl = pEl->ElParent;
	  }
     }
   return ret;
}


/* ---------------------------------------------------------------------- */
/* |    MinMaxComptVal retourne la valeur minimale ou maximale (selon   | */
/* |            que Maximum est faux ou vrai) retourne la valeur du     | */
/* |            compteur de numero NCompt (defini dans le schema de     | */
/* |            presentation  pointe' par pSchP, qui s'applique au      | */
/* |            schema de structure pointe' par pSS) pour l'element     | */
/* |            pointe' par pElNum.                                     | */
/* |            Vue indique la vue concernee (uniquement pour les       | */
/* |            compteurs de page).                                     | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
int                 MinMaxComptVal (int NCompt, PtrSSchema pSS, PtrPSchema pSchP, PtrElement pElNum, int Vue, boolean Maximum)

#else  /* __STDC__ */
int                 MinMaxComptVal (NCompt, pSS, pSchP, pElNum, Vue, Maximum)
int                 NCompt;
PtrSSchema        pSS;
PtrPSchema          pSchP;
PtrElement          pElNum;
int                 Vue;
boolean             Maximum;

#endif /* __STDC__ */

{
   int                 val, valinitattr;
   int                 TypeIncr, TypeSet, TypeRank;
   PtrSSchema        pSchIncr;
   boolean             stop, pstop, initattr;
   PtrElement          pEl;
   Counter           *pCo1;
   PtrElement          pEl2;
   PtrSSchema        pSchStr, pSSpr;

   valinitattr = 0;
   pCo1 = &pSchP->PsCounter[NCompt - 1];
   initattr = FALSE;		/* a priori, la valeur initiale du compteur ne
				   depend pas d'un attribut. */
   pstop = TRUE;

   /* Traitement de la partie initialisation du compteur */
   initattr = InitCounterByAttribute (&valinitattr, pCo1, pElNum, pSS);

   /* Traitement des compteurs de type CntrRank */
   if (pCo1->CnItem[0].CiCntrOp == CntrRank)
     {
	TypeRank = pCo1->CnItem[0].CiElemType;

	/* Cas particulier du compteur de page */
	if (TypeRank == PageBreak + 1)
	  {
	     /* c'est un compteur de pages (CntrRank of Page) */
	     if (initattr)
		val = valinitattr;
	     else
		val = 0;
	     /* On vient de trouver la valeur minimale */

	     if (Maximum)
	       {
		  /* mais on veut la valeur maximale; alors, on continue */
		  pEl = pElNum;
		  pEl2 = pEl;
		  while (pEl2->ElParent != NULL)
		     pEl2 = pEl2->ElParent;
		  pSSpr = pEl2->ElStructSchema;
		  /* vue a laquelle appartient la marque de page cherchee */
		  if (pCo1->CnItem[0].CiViewNum > 0)
		     Vue = pCo1->CnItem[0].CiViewNum;
		  if (pCo1->CnItem[0].CiViewNum == 0 && pElNum->ElAssocNum > 0)
		     Vue = pElNum->ElViewPSchema;
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
			       /* Saut de page d'une vue squelette */
			       /* on ne que compte les pages etrangeres au schema */
			       /* principal */
			       pstop = pEl->ElStructSchema->SsCode !=
				  pSSpr->SsCode;
			    else
			       pstop = pEl->ElStructSchema->SsCode ==
				  pSSpr->SsCode;

			    /* on ignore les pages qui ne concernent pas la vue */
#ifdef __COLPAGE__
			    /* ainsi que les marques colonnes */
			    if ((pEl->ElViewPSchema == Vue) &&
				(pEl->ElPageType != ColBegin
				 && pEl->ElPageType != ColUser
				 && pEl->ElPageType != ColComputed
				 && pEl->ElPageType != ColGroup)
				&& (pstop))
#else  /* __COLPAGE__ */
			    if ((pEl->ElViewPSchema == Vue) && (pstop))
#endif /* __COLPAGE__ */
			       stop = TRUE;
			 }
		    }
		  while (!(stop));

		  if (pEl == NULL)	/* pas de marque de page precedente */
		     if (initattr)
			val = valinitattr;
		     else
			val = 0;
		  else
		     /* on prend pour valeur le numero de la marque de page */
		     /* precedente */
		  if ((pEl->ElViewPSchema == Vue) && (pEl->ElPageNumber > val))
		     val = pEl->ElPageNumber;
		  /* A partir de cette marque de page, cherche en avant les */
		  /* suivantes jusqu'a ne plus en trouver */
		  while (pEl != NULL)
		    {
		       pEl = FwdSearchTypedElem (pEl, TypeRank, NULL);
		       if (pEl != NULL)
			  if ((pEl->ElViewPSchema == Vue) && (pEl->ElPageNumber > val))
			     val = pEl->ElPageNumber - 1;
		    }
		  if (pCo1->CnPageFooter)
		    {
		       /* Si on est dans un compteur en bas de page, il faut */
		       /* incrementer car on creera un saut de page en plus a */
		       /* la fin */
		       val++;
		    }
	       }
	  }
	else
	  {
	     /* Cas standard */
	     /* numero = rang de l'element dans la liste */
	     /* Cherche le premier element de type du rank */
	     /* englobant l'element a numeroter */
	     pEl = GetTypedAncestor (pElNum, TypeRank, pSS);
	     if (pEl == NULL)
		val = 0;	/* pas trouve' */
	     else
	       {
		  /* Cherche le rang de l'element trouve' parmi ses freres */
		  if (initattr)
		     val = valinitattr;
		  else
		     val = 1;
		  /* On vient de trouver la valeur minimale du compteur */

		  if (Maximum)
		    {
		       /* mais comme on cherche la valeur maximale, on fait ce */
		       /* qui suit */
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
			       val++;	/* meme type, on incremente */
			 }
		    }
	       }
	  }
     }
   /* fonction de comptage SET ON, ADD ON */
   else
     {
	/* type ou alias qui fait reset */
	TypeSet = MakeTypeCptTempo (pCo1, CntrSet, pSS);
	/* type ou alias qui incremente */
	TypeIncr = MakeTypeCptTempo (pCo1, CntrAdd, pSS);
	pSchStr = pSS;

	/* Cherche le premier element de type TypeSet */
	/* englobant l'element a numeroter */
	pEl = GetTypedAncestor (pElNum, TypeSet, pSS);
	/* s'il n' y a pas d'ascendant du type requis alors on reste sur pElNum */
	if (pEl == NULL)
	   pEl = pElNum;

	/* l'element trouve' est celui qui reinitialise le compteur 
	   Sa valeur est la valeur minimale du compteur */
	if (initattr)
	   val = valinitattr;
	else
	   val = GetCptElemVal (pCo1, pEl, CntrSet, pSS);

	if (Maximum)
	  {
	     /* On veut la valeur maximale du compteur     */
	     /* Cherche le premier element de type TypeSet */
	     /* englobant l'element a numeroter.           */
	     pEl = GetTypedAncestor (pElNum, TypeSet, pSS);

	     /* s'il n' y a pas d'ascendant du type requis alors on reste sur
	        pElNum */
	     if (pEl == NULL)
		pEl = pElNum;

	     /* l'element trouve' est celui qui reinitialise le compteur */
	     if (initattr)
		val = valinitattr;
	     else
		val = GetCptElemVal (pCo1, pEl, CntrSet, pSS);

	     /* a partir de l'element trouve', cherche en avant tous les */
	     /* elements ayant le type qui incremente le compteur, */
	     /* jusqu'a rencontrer un autre element qui reset le compteur */
	     if (TypeIncr <= MAX_BASIC_TYPE)
		/* c'est un type de base, on le cherche quel que soit son schema */
		pSchIncr = NULL;
	     else
		pSchIncr = pSchStr;	/* schema de struct. du type qui incremente */
	     if (TypeIncr > 0)
		do
		  {
		     pEl = FwdSearchElem2Types (pEl, TypeIncr, pElNum->ElTypeNumber, pSchIncr,
				       pElNum->ElStructSchema);
		     if (pEl != NULL)
			if (EquivalentType (pEl, TypeIncr, pSchIncr))
			   /* on a trouve' un element du type qui incremente */
			   val += GetCptElemVal (pCo1, pEl, CntrAdd, pSS);
		  }
		while (pEl != NULL && !EquivalentType (pEl, TypeSet, pSS));
	  }
     }
   if (val < 0)
      val = 0;

   return val;
}


/* ---------------------------------------------------------------------- */
/* |    ComptVal retourne la valeur du compteur de numero NCompt (defini| */
/* |            dans le schema de presentation  pointe' par pSchP, qui  | */
/* |            s'applique au schema de structure pointe' par pSS) pour | */
/* |            l'element pointe' par pElNum.                           | */
/* |            Vue indique la vue concernee (uniquement pour les       | */
/* |            compteurs de page).                                     | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
int                 ComptVal (int NCompt, PtrSSchema pSS, PtrPSchema pSchP, PtrElement pElNum, int Vue)

#else  /* __STDC__ */
int                 ComptVal (NCompt, pSS, pSchP, pElNum, Vue)
int                 NCompt;
PtrSSchema        pSS;
PtrPSchema          pSchP;
PtrElement          pElNum;
int                 Vue;

#endif /* __STDC__ */

{
   int                 val, valinitattr, level;
   int                 TypeIncr, TypeSet, TypeRank, TypeRLevel;
   PtrSSchema        pSchIncr;
   boolean             stop, pstop, initattr;
   PtrElement          pEl;
   Counter           *pCo1;
   PtrElement          pEl2;
   PtrSSchema        pSchStr, pSSpr;
   PtrAttribute         pAttr;
   PtrElement          pElReinit;

#define MaxAncetre 50
   PtrElement          PcWithin[MaxAncetre];

   pCo1 = &pSchP->PsCounter[NCompt - 1];
   initattr = FALSE;		/* a priori, la valeur initiale du compteur ne
				   depend pas d'un attribut. */
   pstop = TRUE;
   /* Traitement de la partie initialisation du compteur */
   initattr = InitCounterByAttribute (&valinitattr, pCo1, pElNum, pSS);

   /* Traitement des compteurs de type CntrRLevel */
   if (pCo1->CnItem[0].CiCntrOp == CntrRLevel)
     {
	TypeRLevel = pCo1->CnItem[0].CiElemType;
	pEl = pElNum;
	val = 0;
	/* parcourt les elements englobants de l'element sur lequel porte */
	/* le calcul du compteur */
	while (pEl != NULL)
	  {
	     if (pEl->ElTypeNumber == TypeRLevel &&
	     pEl->ElStructSchema->SsCode == pElNum->ElStructSchema->SsCode)
		/* cet element englobant a le type qui increment le compteur */
		val++;		/* incremente le compteur */
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
		Vue = pCo1->CnItem[0].CiViewNum;
	     if (pCo1->CnItem[0].CiViewNum == 0 && pElNum->ElAssocNum > 0)
		Vue = pElNum->ElViewPSchema;
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
			  /* Saut de page d'une vue squelette */
			  /*on ne que compte les pages etrangeres au schema principal */
			  pstop = pEl->ElStructSchema->SsCode != pSSpr->SsCode;
		       else
			  pstop = pEl->ElStructSchema->SsCode == pSSpr->SsCode;

		       /* on ignore les pages qui ne concernent pas la vue */
#ifdef __COLPAGE__
		       /* ainsi que les marques colonnes */
		       if ((pEl->ElViewPSchema == Vue) &&
			   (pEl->ElPageType != ColBegin &&
			    pEl->ElPageType != ColUser &&
			    pEl->ElPageType != ColComputed &&
			    pEl->ElPageType != ColGroup) &&
			   (pstop))
#else  /* __COLPAGE__ */
		       /* on ignore les pages qui ne concernent pas la vue */
		       if ((pEl->ElViewPSchema == Vue) && (pstop))
#endif /* __COLPAGE__ */
			  stop = TRUE;
		    }
	       }
	     while (!(stop));

	     if (pEl == NULL)	/* pas de marque de page precedente */
		if (initattr)
		  {
		     val = valinitattr;
		     if (val > 0)
			val--;
		  }
		else
		   val = 0;

	     else
		/* on prend pour valeur le numero de la marque de page precedente */
		val = pEl->ElPageNumber;
#ifdef __COLPAGE__
	     if (pElNum->ElTerminal && pElNum->ElLeafType == LtPageColBreak)
		/* cas page rappel supprime */
#else  /* __COLPAGE__ */
	     if (pElNum->ElTerminal && pElNum->ElLeafType == LtPageColBreak)
		/* on numerote une marque de page  */
#endif /* __COLPAGE__ */
		/* on incremente la valeur */
		val++;
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
		     level = MaxAncetre;
		  pEl = pElNum;
		  while (level > 0 && pEl != NULL)
		    {
		       if (pEl->ElTypeNumber == TypeRank &&
			   pEl->ElStructSchema->SsCode ==
			   pElNum->ElStructSchema->SsCode)
			  /* cet element englobant a le type qui incremente le compteur */
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
	     val = 0;
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
				  pAttr = pAttr->AeNext;		/* au suivant */
			    while (!(stop));
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
			  val++;	/* meme type, on incremente */
		       pEl = pEl->ElPrevious;
		    }
		  if (pAttr != NULL)
		     /* le compteur est reinitialise' par l'attribut pAttr */
		     val += pAttr->AeAttrValue;
		  else if (initattr)
		     val += valinitattr - 1;
	       }
	  }
     }
   /* fonction de comptage SET ON, ADD ON */
   else
     {
	/* type ou alias qui fait reset */
	TypeSet = MakeTypeCptTempo (pCo1, CntrSet, pSS);
	/* type ou alias qui incremente */
	TypeIncr = MakeTypeCptTempo (pCo1, CntrAdd, pSS);
	pSchStr = pSS;

	/* Cherche le premier element de type TypeSet */
	/* englobant l'element a numeroter */
	pEl = GetTypedAncestor (pElNum, TypeSet, pSS);
	/* s'il n' y a pas d'ascendant du type requis alors on reste sur pElNum */
	if (pEl == NULL)
	   pEl = pElNum;

	/* l'element trouve' est celui qui reinitialise le compteur */
	if (initattr)
	   val = valinitattr;
	else
	   val = GetCptElemVal (pCo1, pEl, CntrSet, pSS);

	/* a partir de l'element trouve', cherche en avant tous les */
	/* elements ayant le type qui incremente le compteur, */
	/* jusqu'a rencontrer l'element qui a cree la boite compteur. */
	if (TypeIncr <= MAX_BASIC_TYPE)
	   /* c'est un type de base, on le cherche quel que soit son schema */
	   pSchIncr = NULL;
	else
	   pSchIncr = pSchStr;	/* schema de struct. du type qui incremente */
	if (TypeIncr > 0)
	   do
	     {
		pEl = FwdSearchElem2Types (pEl, TypeIncr, pElNum->ElTypeNumber, pSchIncr,
				  pElNum->ElStructSchema);
		if (pEl != NULL)
		   if (EquivalentType (pEl, TypeIncr, pSchIncr))
		      /* on a trouve' un element du type qui incremente */
		      val += GetCptElemVal (pCo1, pEl, CntrAdd, pSS);
	     }
	   while (!(pEl == NULL || pEl == pElNum));
     }

   if (val < 0)
      val = 0;

   return val;
}

/* ---------------------------------------------------------------------- */
/* |    PavPresentModifiable    retourne vrai si le pave pAb est un    | */
/* |            pave de presentation dont le contenu peut etre modifie' | */
/* |            par l'utilisateur.                                      | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
boolean             PavPresentModifiable (PtrAbstractBox pAb)
#else  /* __STDC__ */
boolean             PavPresentModifiable (pAb)
PtrAbstractBox             pAb;

#endif /* __STDC__ */
{
   boolean             result;
   PresentationBox             *pBo;
   PresVariable            *pPr;

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


/* ---------------------------------------------------------------------- */
/* |    NouvVariable met dans le pave pAb le texte correspondant a la  | */
/* |            variable de numero NVar definie dans le schema de       | */
/* |            presentation pSchP (et qui correspond au schema de      | */
/* |            structure pSS). pDoc pointe sur le descripteur du       | */
/* |            document pour lequel on travaille. Si le pave avait     | */
/* |            deja un contenu et que ce contenu ne change pas la      | */
/* |            fonction retourne 'faux'. S'il y a un nouveau contenu,  | */
/* |            elle retourne 'vrai'.                                   | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
boolean             NouvVariable (int NVar, PtrSSchema pSS, PtrPSchema pSchP,
				  PtrAbstractBox pAb, PtrDocument pDoc)

#else  /* __STDC__ */
boolean             NouvVariable (NVar, pSS, pSchP, pAb, pDoc)
int                 NVar;
PtrSSchema        pSS;
PtrPSchema          pSchP;
PtrAbstractBox             pAb;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   int                 f, l;
   int                 i;
   long                tod;
   long               *pt;
   struct tm          *ptm;
   PtrTextBuffer      Ancien, Nouveau;
   PtrAttribute         pAttr;
   boolean             trouve;
   boolean             egal;
   PresVariable            *pPr1;
   PresVarItem            *pVa1;
   PresConstant          *pPres1;
   TtAttribute           *pAttr1;
   PtrElement          pEl;
   Counter           *pCo1;
   char                Nombre[20];
   PtrTextBuffer      pBTN, pBTA, pBTAPrec;

   /* sauve temporairement le contenu de ce pave de presentation */
   Ancien = pAb->AbText;
   /* acquiert un buffer de texte pour y calculer la (nouvelle) valeur */
   /* de la variable */
   GetBufConst (pAb);
   pAb->AbVolume = 0;
   pAttr = NULL;
   /* remplit le buffer avec le contenu defini par la variable */
   pPr1 = &pSchP->PsVariable[NVar - 1];
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
		    trouve = FALSE;
		    while (!trouve && pEl != NULL)
		      {
			 pAttr = pEl->ElFirstAttr;	/* premier attribut */
			 while (!trouve && pAttr != NULL)
			    if (pAttr->AeAttrNum == pVa1->ViAttr &&
				pAttr->AeAttrSSchema->SsCode == pSS->SsCode)
			       trouve = TRUE;
			    else
			       pAttr = pAttr->AeNext;
			 if (!trouve)
			    /* passe a l'element ascendant */
			    pEl = pEl->ElParent;
		      }
		    if (trouve)
		       /* l'element possede l'attribut */
		      {
			 switch (pAttr->AeAttrType)
			       {
				  case AtNumAttr:
				     /* traduit l'entier en ASCII selon le style voulu */
				     ConvertitNombre (pAttr->AeAttrValue, pVa1->ViStyle, Nombre, &l);
				     CopyStringToText (Nombre, pAb->AbText, &l);
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
				     CopyStringToText ("REF", pAb->AbText, &l);
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
		       i = MinMaxComptVal (pVa1->ViCounter, pSS, pSchP, pAb->AbElement,
			     pDoc->DocView[pAb->AbDocView - 1].DvPSchemaView, TRUE);
		    else if (pVa1->ViCounterVal == CntMinVal)
		       /* on cherche la valeur minimale du compteur */
		       i = MinMaxComptVal (pVa1->ViCounter, pSS, pSchP,
					   pAb->AbElement,
				    pDoc->DocView[pAb->AbDocView - 1].DvPSchemaView,
					   FALSE);
		    else
		       /* valeur courante du compteur */
		       i = ComptVal (pVa1->ViCounter, pSS, pSchP, pAb->AbElement,
				   pDoc->DocView[pAb->AbDocView - 1].DvPSchemaView);
#ifndef __COLPAGE__
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
#endif /* __COLPAGE__ */
		    /* traduit l'entier en ASCII */
		    ConvertitNombre (i, pVa1->ViStyle, Nombre, &l);
		    CopyStringToText (Nombre, pAb->AbText, &l);
		    pAb->AbVolume += l;
		    break;
		 case VarDate:
		    /* date en anglais */
		    pt = &tod;
		    *pt = time (NULL);
		    ptm = localtime (pt);

		    ConvertitNombre (ptm->tm_year, CntArabic, Nombre, &l);
		    CopyStringToText (Nombre, pAb->AbText, &l);
		    pAb->AbVolume += l;
		    CopyStringToText ("/", pAb->AbText, &l);
		    pAb->AbVolume += l;
		    ConvertitNombre (ptm->tm_mon + 1, CntArabic, Nombre, &l);
		    CopyStringToText (Nombre, pAb->AbText, &l);
		    pAb->AbVolume += l;
		    CopyStringToText ("/", pAb->AbText, &l);
		    pAb->AbVolume += l;
		    ConvertitNombre (ptm->tm_mday, CntArabic, Nombre, &l);
		    CopyStringToText (Nombre, pAb->AbText, &l);
		    pAb->AbVolume += l;
		    break;
		 case VarFDate:
		    /* date en francais */
		    pt = &tod;
		    *pt = time (NULL);
		    ptm = localtime (pt);
		    ConvertitNombre (ptm->tm_mday, CntArabic, Nombre, &l);
		    CopyStringToText (Nombre, pAb->AbText, &l);
		    pAb->AbVolume += l;
		    CopyStringToText ("/", pAb->AbText, &l);
		    pAb->AbVolume += l;
		    ConvertitNombre (ptm->tm_mon + 1, CntArabic, Nombre, &l);
		    CopyStringToText (Nombre, pAb->AbText, &l);
		    pAb->AbVolume += l;
		    CopyStringToText ("/", pAb->AbText, &l);
		    pAb->AbVolume += l;
		    ConvertitNombre (ptm->tm_year, CntArabic, Nombre, &l);
		    CopyStringToText (Nombre, pAb->AbText, &l);
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
		    /* vue ViView */
		    /* cherche en arriere la premiere marque de page de cette vue */
		    pEl = pAb->AbElement;
		    trouve = FALSE;
		    do
		      {
			 pEl = BackSearchTypedElem (pEl, PageBreak + 1, NULL);
			 if (pEl != NULL)
			    /* on ignore les pages qui ne concernent pas la vue */
			    if (pEl->ElViewPSchema == pVa1->ViView)
			       trouve = TRUE;
		      }
		    while ((!trouve) && (pEl != NULL));
		    if (pEl == NULL)
		       i = 1;	/* pas trouve', on considere que c'est la page 1 */
		    else
		       i = pEl->ElPageNumber;	/* numero de la page trouvee */
		    /* traduit le numero de page en ASCII selon le style voulu */
		    ConvertitNombre (i, pVa1->ViStyle, Nombre, &l);
		    CopyStringToText (Nombre, pAb->AbText, &l);
		    pAb->AbVolume += l;
		    break;
		 default:
		    break;
	      }
	/* termine la mise a jour du pave */
     }
   if (PavPresentModifiable (pAb))
      /* le contenu de ce pave de presentation est donc modifiable */
      pAb->AbCanBeModified = TRUE;
   pAb->AbLeafType = LtText;
   if (Ancien == NULL)
      egal = FALSE;		/* la variable n'avait pas de valeur */
   else
     {
	Nouveau = pAb->AbText;	/* nouveau contenu du pave de presentation */
	/* compare le nouveau et l'ancien contenu du pave */
	egal = TextsEqual (Ancien, Nouveau);
	if (!egal)
	   /* contenus differents */
	   /* recopie le nouveau contenu dans les anciens buffers */
	  {
	     pBTA = Ancien;
	     pBTAPrec = NULL;
	     pBTN = Nouveau;
	     while (pBTN != NULL)
	       {
		  if (pBTA == NULL)
		     pBTA = NewTextBuffer (pBTAPrec);
		  strcpy (pBTA->BuContent, pBTN->BuContent);
		  pBTA->BuLength = pBTN->BuLength;
		  pBTN = pBTN->BuNext;
		  pBTAPrec = pBTA;
		  pBTA = pBTA->BuNext;
	       }
	  }
	pAb->AbText = Ancien;
	/* libere les buffers nouveaux */
	do
	  {
	     pBTN = Nouveau->BuNext;
	     FreeBufTexte (Nouveau);
	     Nouveau = pBTN;
	  }
	while (Nouveau != NULL);
     }
   if (!egal)
      /* reaffiche les references qui copient cette variable */
      ChngRef (pAb, pDoc);
   return !egal;
}
/* End Of Module varpres */
