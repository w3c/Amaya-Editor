
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*
   modimabs.c -- Modifications des images abstraites.
   Ce module effectue les modifications des images abstraites des documents
   et demande au Mediateur le reaffichage incremental. 
   V. Quint     Fevrier 1986
   France Logiciel no de depot 88-39-001-00

   IV : Septembre 92 adaptation Tool Kit
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"

#define EXPORT extern
#include "environ.var"
#include "page.var"

#include "tree_f.h"
#include "attributes_f.h"
#include "attrherit_f.h"
#include "search_f.h"
#include "createabsbox_f.h"
#include "createpages_f.h"
#include "structlist_f.h"
#include "frame_f.h"
#include "absboxes_f.h"
#include "buildboxes_f.h"
#include "memory_f.h"
#include "changeabsbox_f.h"
#include "changepresent_f.h"
#include "presrules_f.h"
#include "references_f.h"
#include "presvariables_f.h"
#include "content_f.h"

static PtrAbstractBox      PavDebut[MAX_VIEW_DOC];


/* ---------------------------------------------------------------------- */
/* |    EnglobeP retourne vrai si le pave pPav1 englobe le pave pPav2   | */
/* |            ou si les deux pointeurs pointent le meme pave.         | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static boolean      EnglobeP (PtrAbstractBox pPav1, PtrAbstractBox pPav2)

#else  /* __STDC__ */
static boolean      EnglobeP (pPav1, pPav2)
PtrAbstractBox             pPav1;
PtrAbstractBox             pPav2;

#endif /* __STDC__ */
{
   boolean             ret;
   PtrAbstractBox             pAbb;

   if (pPav2 == NULL || pPav1 == pPav2)
      ret = TRUE;
   else
     {
	ret = FALSE;
	pAbb = pPav2;
	do
	   if (pAbb->AbEnclosing == pPav1)
	      ret = TRUE;
	   else
	      pAbb = pAbb->AbEnclosing;
	while (!(ret || pAbb == NULL));
     }
   return ret;
}


/* ---------------------------------------------------------------------- */
/* |    Englobant retourne un pointeur sur le pave de plus bas niveau   | */
/* |            qui englobe a la fois les deux paves pPav1 et pPav2.    | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
PtrAbstractBox             Englobant (PtrAbstractBox pPav1, PtrAbstractBox pPav2)

#else  /* __STDC__ */
PtrAbstractBox             Englobant (pPav1, pPav2)
PtrAbstractBox             pPav1;
PtrAbstractBox             pPav2;

#endif /* __STDC__ */

{
   PtrAbstractBox             pAbb;
   boolean             found;

   if (pPav1 == NULL)
      pAbb = pPav2;
   else if (pPav2 == NULL)
      pAbb = pPav1;
   else if (pPav1 == pPav2)
      pAbb = pPav1;
   else
     {
	pAbb = pPav1;
	do
	  {
	     found = EnglobeP (pAbb, pPav2);
	     if (!found)
		pAbb = pAbb->AbEnclosing;
	  }
	while (!(found || pAbb == NULL));
     }
   return pAbb;
}



/* ---------------------------------------------------------------------- */
/* |    ChercheReglePEL cherche dans la chaine pRegle la regle du       | */
/* |            type TRegle a appliquer pour la vue Vue.                | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                ChercheReglePEl (PtrPRule * pRVue1, PtrElement pEl, int Vue, PRuleType TRegle, PtrPRule * pRegle)

#else  /* __STDC__ */
void                ChercheReglePEl (pRVue1, pEl, Vue, TRegle, pRegle)
PtrPRule       *pRVue1;
PtrElement          pEl;
int                 Vue;
PRuleType           TRegle;
PtrPRule       *pRegle;

#endif /* __STDC__ */

{
   boolean             trouve;
   PtrPRule        pR;

   pR = *pRegle;
   *pRVue1 = NULL;
   trouve = FALSE;
   while (!trouve && pR != NULL)
     {
	if (pR->PrType == TRegle)
	  {
	     /* regle du type cherche' */
	     if (pR->PrViewNum == Vue)
		/* pour la vue voulue */
		if (pR->PrCond == NULL ||
		    CondPresentation (pR->PrCond, pEl, NULL, Vue, pEl->ElSructSchema))
		   /* les conditions d'application de la regle sont satisfaites */
		   trouve = TRUE;
	     if (!trouve)
	       {
		  if (pR->PrViewNum == 1 && *pRVue1 == NULL)
		     /* regle du type cherche' pour la vue 1 */
		     if (pR->PrCond == NULL ||
			 CondPresentation (pR->PrCond, pEl, NULL, Vue, pEl->ElSructSchema))
			/* les conditions d'application de la regle sont satisfaites */
			/* on la garde pour le cas ou on ne trouve pas mieux */
			*pRVue1 = pR;
		  /* regle suivante */
		  pR = pR->PrNextPRule;
	       }
	  }
	else
	   /* ce n'est pas le type de regle cherche' */
	if (pR->PrType > TRegle)
	   /* il n'y a plus de regle de ce type, stop */
	   pR = NULL;
	else
	   /* regle suivante */
	   pR = pR->PrNextPRule;
     }
   if (trouve)
      *pRegle = pR;
   else
      *pRegle = NULL;
}


/* ---------------------------------------------------------------------- */
/* |    ReglePEl retourne un pointeur sur la regle de type TRegle       | */
/* |            a appliquer a l'element pointe' par pEl dans la vue de  | */
/* |            numero Vue.                                             | */
/* |            Retourne dans pSPR un pointeur sur le schema de         | */
/* |            presentation auquel appartient la regle retournee.      | */
/* |            Si NumPres est nul, on cherche la regle a appliquer     | */
/* |            a l'element lui-meme, sinon, on cherche de la regle a   | */
/* |            appliquer a son pave de presentation de numero NumPres. | */
/* |            Dans ce dernier cas, si pSchP est NULL, le pave de      | */
/* |            presentation est defini dans le schema de presentation  | */
/* |            a appliquer a l'element, sinon ce pave est defini dans  | */
/* |            le schema de presentation pointe' par pSchP.            | */
/* |            Dans le cas ou pEl pointe une marque de page, si Pag    | */
/* |            est vrai, il s'agit de la regle a appliquer au filet de | */
/* |            separation sinon il s'agit de la regle a appliquer au   | */
/* |            bloc de separation de page.                             | */
/* |            dans V4, le booleen Pag n'a plus aucun sens             | */
/* |            Si attr est vrai, on cherche parmi les regles de        | */
/* |            presentation des attributs de l'element, sinon, on      | */
/* |            ignore les regles de presentation des attributs.        | */
/* |            Si on trouve la regle parmi celles qui sont associees   | */
/* |            a un attribut, au retour pAttr pointe sur le bloc       | */
/* |            attribut correspondant, sinon pAttr est NULL.           | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
PtrPRule        ReglePEl (PtrElement pEl, PtrPSchema * pSPR, PtrSSchema * pSSR, int NumPres, PtrPSchema pSchP, int Vue, PRuleType TRegle, boolean Pag, boolean attr, PtrAttribute * pAttr)

#else  /* __STDC__ */
PtrPRule        ReglePEl (pEl, pSPR, pSSR, NumPres, pSchP, Vue, TRegle, Pag, attr, pAttr)
PtrElement          pEl;
PtrPSchema         *pSPR;
PtrSSchema       *pSSR;
int                 NumPres;
PtrPSchema          pSchP;
int                 Vue;
PRuleType           TRegle;
boolean             Pag;
boolean             attr;
PtrAttribute        *pAttr;

#endif /* __STDC__ */

{
   int                 Entree;
   PtrPRule        pRegle, pRSpecVue1, pRVue1;
   PtrSSchema        pSchS;
   boolean             stop;
   PtrPRule        pR;
   PtrAttribute         pA;
   PtrElement          pElAttr;
   InheritAttrTable     *table_herites;
   int                 l;
   PtrPSchema          pSP;
   PtrHandlePSchema    pHd;

#ifdef __COLPAGE__
   int                 nb;

#endif /* __COLPAGE__ */

   pRegle = NULL;
   *pAttr = NULL;
   *pSPR = NULL;
   if (pEl != NULL)
     {
	/* cherche d'abord parmi les regles de presentation specifique */
	/* associees a l'element, sauf s'il s'agit d'un pave de presentation */
	if (NumPres == 0)
	  {
	     pRegle = pEl->ElFirstPRule;
	     stop = FALSE;
	     while (pRegle != NULL && !stop)
		if (pRegle->PrType == TRegle && pRegle->PrViewNum == Vue)
		   /* regle du type cherche' pour la vue voulue, c'est bon */
		   stop = TRUE;
		else
		   /* regle suivante */
		   pRegle = pRegle->PrNextPRule;
	     if (pRegle != NULL)
	       {
		  /* si la regle est associee a un attribut, on retourne un */
		  /* pointeur sur cet attribut */
		  if (pRegle->PrSpecifAttr > 0)
		    {
		       *pAttr = pEl->ElFirstAttr;
		       stop = FALSE;
		       while (*pAttr != NULL && !stop)
			  if ((*pAttr)->AeAttrNum == pRegle->PrSpecifAttr
			      && (*pAttr)->AeAttrSSchema == pRegle->PrSpecifAttrSSchema)
			     stop = TRUE;
			  else
			     *pAttr = (*pAttr)->AeNext;
		    }
	       }
	  }
	else
	   /* les boites de presentation ne sont pas concernees par les */
	   /* attributs */
	   attr = FALSE;

	/* cherche parmi les regles de presentation des */
	/* attributs de l'element, si on n'a pas encore trouve' */
	if (pRegle == NULL && attr)
	  {
	     pA = pEl->ElFirstAttr;	/* premier attribut de l'element */
	     /* parcourt tous les attributs de l'element, de facon a garder la */
	     /* regle qui correspond au dernier attribut qui possede ce type de */
	     /* regle */
	     while (pA != NULL)
	       {
		  pR = NULL;
		  /* Si on travaille pour la vue principale, on traite d'abord les */
		  /* schemas de presentation additionnels les plus prioritaires, */
		  /* sinon on ignore les schemas additionnels. */
		  if (Vue == 1)
		     pHd = pA->AeAttrSSchema->SsFirstPSchemaExtens;
		  else
		     pHd = NULL;
		  if (pHd == NULL)
		     /* pas de schema additionnel. On prend le schema principal */
		     pSP = pA->AeAttrSSchema->SsPSchema;
		  else
		    {
		       while (pHd->HdNextPSchema != NULL)
			  pHd = pHd->HdNextPSchema;
		       pSP = pHd->HdPSchema;
		    }
		  while (pR == NULL && pSP != NULL)
		    {
		       /* premiere regle de presentation de cet attribut */
		       pR = ReglePresAttr (pA, pEl, FALSE, NULL, pSP);
		       /* parcourt les regles de presentation de l'attribut */
		       stop = FALSE;
		       while (pR != NULL && !stop)
			 {
			    if (pR->PrType == TRegle && pR->PrViewNum == Vue)
			       /* regle du type cherche' pour la vue voulue */
			       if (pR->PrCond == NULL ||
				   CondPresentation (pR->PrCond, pEl, pA, Vue, pA->AeAttrSSchema))
				  /* les conditions d'application de la regle sont */
				  /* satisfaites */
				 {
				    pRegle = pR;
				    *pAttr = pA;
				    *pSPR = pSP;
				    *pSSR = pA->AeAttrSSchema;
				    stop = TRUE;
				 }
			    if (!stop)
			       pR = pR->PrNextPRule;	/* regle suivante */
			 }
		       if (pR == NULL)
			  /* on n'a pas encore trouve'. On continue de chercher dans */
			  /* les schemas de presentation de moindre priorite' */
			 {
			    if (pHd == NULL)
			       /* on cherchait dans le schema de presentation principal. */
			       /* c'est fini */
			       pSP = NULL;
			    else
			      {
				 /* on prend le schema de presentation additionnel de */
				 /* moindre priorite' */
				 pHd = pHd->HdPrevPSchema;
				 if (pHd == NULL)
				    /* plus de schemas additionnels, on prend le schema princ */
				    pSP = pA->AeAttrSSchema->SsPSchema;
				 else
				    pSP = pHd->HdPSchema;
			      }
			 }
		    }
		  /* passe a l'attribut suivant de l'element */
		  pA = pA->AeNext;
	       }
	  }

	/* cherche parmi les regles de presentation des attributs */
	/* herites par l'element, si on n'a pas encore trouve' */
	if (pRegle == NULL)
	   /* on n'applique pas aux boites de presentation les regles de */
	   /* presentation des attributs herites */
	   if (NumPres == 0)
	     {
		if (pEl->ElSructSchema->SsPSchema->PsNInheritedAttrs[pEl->ElTypeNumber - 1])
		  {
		     /* il y a heritage possible */
		     if ((table_herites = pEl->ElSructSchema->SsPSchema->
			  PsInheritedAttr[pEl->ElTypeNumber - 1]) == NULL)
		       {
			  /* cette table n'existe pas on la genere */
			  CreateInheritedAttrTable (pEl);
			  table_herites = pEl->ElSructSchema->SsPSchema->
			     PsInheritedAttr[pEl->ElTypeNumber - 1];
		       }
		     for (l = 1; l <= pEl->ElSructSchema->SsNAttributes && *pSPR == NULL;
			  l++)
			if ((*table_herites)[l - 1])
			   /* pEl herite de l'attribut l */
			   /* cherche si l'attribut l existe au dessus */
			   if ((*pAttr = GetTypedAttrAncestor (pEl, l, pEl->ElSructSchema,
							 &pElAttr)) != NULL)
			     {
				/* on traite d'abord les schemas de presentation */
				/* additionnels les plus prioritaires si on travaille pour */
				/* la vue principale. Sinon, on ignore les schemas */
				/* additionnels */
				if (Vue == 1)
				   pHd = (*pAttr)->AeAttrSSchema->SsFirstPSchemaExtens;
				else
				   pHd = NULL;
				if (pHd == NULL)
				   /* pas de schema additionnel. Prend le schema principal */
				   pSP = (*pAttr)->AeAttrSSchema->SsPSchema;
				else
				   /* on cherche le schema additionnel le plus proritaire */
				  {
				     while (pHd->HdNextPSchema != NULL)
					pHd = pHd->HdNextPSchema;
				     pSP = pHd->HdPSchema;
				  }
				stop = FALSE;
				while (!stop && pSP != NULL)
				  {
				     /* premiere regle de presentation de cet attribut dans */
				     /* ce schema de presentation */
				     pRegle = ReglePresAttr (*pAttr, pEl, TRUE, NULL, pSP);
				     /* parcourt les regles de presentation de l'attribut */
				     while (pRegle != NULL && !stop)
				       {
					  if (pRegle->PrType == TRegle && pRegle->PrViewNum == Vue)
					     /* regle du type cherche' pour la vue voulue */
					     if (pRegle->PrCond == NULL ||
						 CondPresentation (pRegle->PrCond, pEl, *pAttr,
					       Vue, (*pAttr)->AeAttrSSchema))
						/* les conditions d'application de la regle */
						/* sont satisfaites */
					       {
						  *pSPR = pSP;
						  *pSSR = (*pAttr)->AeAttrSSchema;
						  stop = TRUE;
					       }
					  if (!stop)
					     /* regle suivante */
					     pRegle = pRegle->PrNextPRule;
				       }
				     if (!stop)
					/* on n'a pas encore trouve'. On continue de chercher */
					/* dans les schemas de presentation de moindre priorite */
				       {
					  if (pHd == NULL)
					     /* on cherchait dans le schema de presentation */
					     /* principal. C'est fini */
					     pSP = NULL;
					  else
					    {
					       /* on prend le schema de presentation additionnel de */
					       /* moindre priorite' */
					       pHd = pHd->HdPrevPSchema;
					       if (pHd == NULL)
						  /* plus de schemas additionnels, on prend le schema */
						  /* principal */
						  pSP = (*pAttr)->AeAttrSSchema->SsPSchema;
					       else
						  pSP = pHd->HdPSchema;
					    }
				       }

				  }
			     }
		  }
	     }

	if (pRegle == NULL)
	  {
	     /* on n'a pas encore trouve' */
	     *pAttr = NULL;
	     pSchS = pEl->ElSructSchema;
#ifdef __COLPAGE__
	     if (NumPres == 0 && pEl->ElTerminal && pEl->ElLeafType == LtPageColBreak)
	       {
		  /* on cherche le type de la boite page ou colonne */
		  if (pEl->ElPageType == PgBegin
		      || pEl->ElPageType == PgComputed
		      || pEl->ElPageType == PgUser)
		     NumPres = TypeBPage (pEl, Vue, &pSchP);
		  else		/* cas de colonne */
		     NumPres = TypeBCol (pEl, Vue, &pSchP, &nb);
#else  /* __COLPAGE__ */
	     if (NumPres == 0 && pEl->ElTerminal && pEl->ElLeafType == LtPageColBreak && Pag)
	       {
		  /* on cherche le type de la boite page */
		  NumPres = TypeBPage (pEl, Vue, &pSchP);
#endif /* __COLPAGE__ */
		  pSchS = pEl->ElSructSchema;
		  Entree = pEl->ElTypeNumber;
	       }
	     else if (NumPres == 0 || pSchP == NULL)
		/* cherche le schema de presentation de l'element */
		ChSchemaPres (pEl, &pSchP, &Entree, &pSchS);
	     /* on traite d'abord les schemas de presentation additionnels les */
	     /* plus prioritaires */
	     pHd = pSchS->SsFirstPSchemaExtens;
	     if (pHd == NULL)
		pSP = pSchP;
	     else
	       {
		  while (pHd->HdNextPSchema != NULL)
		     pHd = pHd->HdNextPSchema;
		  pSP = pHd->HdPSchema;
	       }
	     while (pRegle == NULL && pSP != NULL)
	       {
		  /* premiere regle de presentation de ce type d'element */
		  if (pSP != NULL)
		     if (NumPres > 0)
			pRegle = pSP->PsPresentBox[NumPres - 1].PbFirstPRule;
		     else
			pRegle = pSP->PsElemPRule[Entree - 1];
		  /* cherche une regle du type voulu, pour la vue voulue, parmi */
		  /* les regles du type d'element */
		  pRVue1 = NULL;
		  ChercheReglePEl (&pRVue1, pEl, Vue, TRegle, &pRegle);
		  if (pRegle == NULL)
		     /* on n'a pas encore trouve'. On continue de chercher dans les */
		     /* schemas de presentation de moindre priorite' */
		    {
		       if (pHd == NULL)
			  /* on cherchait dans le schema de presentation principal. */
			  /* c'est fini */
			  pSP = NULL;
		       else
			 {
			    /* on prend le schema de presentation additionnel de moindre */
			    /* priorite' */
			    pHd = pHd->HdPrevPSchema;
			    if (pHd == NULL)
			       /* plus de schemas additionnels, on prend le schema princ. */
			       pSP = pSchP;
			    else
			       pSP = pHd->HdPSchema;
			 }
		    }
	       }

	     if (pRegle == NULL && pSchP != NULL)
	       {
		  /* on n'a pas trouve, cherche la regle par defaut de ce type */
		  pRSpecVue1 = pRVue1;
		  /* premiere regle de presentation par defaut */
		  pRegle = pSchP->PsFirstDefaultPRule;
		  ChercheReglePEl (&pRVue1, pEl, Vue, TRegle, &pRegle);
		  if (pRegle == NULL)
		     if (pRSpecVue1 != NULL)
			pRegle = pRSpecVue1;
		     else
			pRegle = pRVue1;
	       }
	     *pSPR = pSchP;
	     *pSSR = pSchS;
	  }
     }
   return pRegle;
}


/* ---------------------------------------------------------------------- */
/* |    LaRegle retourne un pointeur sur la regle de type TRegle        | */
/* |            a appliquer au pave pAb.                               | */
/* |            Selon que attr est vrai ou faux, on tient compte ou non | */
/* |            des attributs porte's par l'element auquel correspond   | */
/* |            pAb.                                                   | */
/* |            Au retour, pSPR contient un pointeur sur le schema de   | */
/* |            structure auquel appartient la regle.                   | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
PtrPRule        LaRegle (PtrDocument pDoc, PtrAbstractBox pAb, PtrPSchema * pSPR, PRuleType TRegle, boolean attr, PtrAttribute * pAttr)

#else  /* __STDC__ */
PtrPRule        LaRegle (pDoc, pAb, pSPR, TRegle, attr, pAttr)
PtrDocument         pDoc;
PtrAbstractBox             pAb;
PtrPSchema         *pSPR;
PRuleType           TRegle;
boolean             attr;
PtrAttribute        *pAttr;

#endif /* __STDC__ */

{
   int                 NumPres;
   PtrAbstractBox             pAbbox1;
   PtrSSchema        pSSR;
   PtrPRule        RegleTrouvee;

   if (pAb == NULL)
      RegleTrouvee = NULL;
   else
     {
	pAbbox1 = pAb;
	if (pAbbox1->AbPresentationBox)
	   NumPres = pAbbox1->AbTypeNum;
	else
	   NumPres = 0;
	RegleTrouvee = ReglePEl (pAbbox1->AbElement, pSPR, &pSSR, NumPres,
		  pAbbox1->AbPSchema, pDoc->DocView[pAbbox1->AbDocView - 1].DvPSchemaView,
				 TRegle, FALSE, attr, pAttr);
     }
   return RegleTrouvee;
}



/* ---------------------------------------------------------------------- */
/* |    PositionDiff retourne vrai si la position Pos est differente de | */
/* |            la position Posit (variable de la fonction NouvRef).    | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static boolean      PositionDiff (AbPosition Pos, AbPosition * Posit, boolean Inverse)
#else  /* __STDC__ */
static boolean      PositionDiff (Pos, Posit, Inverse)
AbPosition         Pos;
AbPosition        *Posit;
boolean             Inverse;

#endif /* __STDC__ */
{
   AbPosition        *pAbbox1;
   boolean             different;

   pAbbox1 = &Pos;
   different = TRUE;
   if (pAbbox1->PosRefEdge == Posit->PosRefEdge &&
       pAbbox1->PosRefEdge == Posit->PosRefEdge &&
       pAbbox1->PosDistance == Posit->PosDistance &&
       pAbbox1->PosUnit == Posit->PosUnit &&
       pAbbox1->PosUserSpecified == Posit->PosUserSpecified &&
       pAbbox1->PosAbRef == Posit->PosAbRef)
      if (!Inverse)
	{
	   if (pAbbox1->PosEdge == Posit->PosEdge)
	      different = FALSE;
	}
      else
	{
	   if (pAbbox1->PosEdge == Right && Posit->PosEdge == Left)
	      different = FALSE;
	   else if (pAbbox1->PosEdge == Left && Posit->PosEdge == Right)
	      different = FALSE;
	   else if (pAbbox1->PosEdge == Top && Posit->PosEdge == Bottom)
	      different = FALSE;
	   else if (pAbbox1->PosEdge == Bottom && Posit->PosEdge == Top)
	      different = FALSE;
	}
   return different;
}



/* ---------------------------------------------------------------------- */
/* |    DimensionDiff retourne vrai si la dimension Dim est differente  | */
/* |            de la dimension Dimens (variable de la fonction         | */
/* |            NouvRef).                                               | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static boolean      DimensionDiff (AbDimension Dim, AbDimension * Dimens)

#else  /* __STDC__ */
static boolean      DimensionDiff (Dim, Dimens)
AbDimension        Dim;
AbDimension       *Dimens;

#endif /* __STDC__ */

{
   AbDimension       *pAbbox1;
   boolean             different;

   pAbbox1 = &Dim;
   if (pAbbox1->DimIsPosition)
      different = TRUE;
   else if (pAbbox1->DimAbRef == Dimens->DimAbRef &&
	    pAbbox1->DimValue == Dimens->DimValue &&
	    pAbbox1->DimUnit == Dimens->DimUnit &&
	    pAbbox1->DimUserSpecified == Dimens->DimUserSpecified &&
	    pAbbox1->DimSameDimension == Dimens->DimSameDimension)
      different = FALSE;
   else
      different = TRUE;
   return different;
}



/* ---------------------------------------------------------------------- */
/* |    NouvRef cherche si la regle pointee par pR (qui appartient au   | */
/* |            schema de presentation pointe' par pSPR) en s'appliquant| */
/* |            au pave pAb change de valeur.                          | */
/* |            Si oui, la fonction retourne vrai et applique la regle  | */
/* |            a pAb, sinon elle retourne faux.                       | */
/* |            Niv indique la position relative de pRef par rapport    | */
/* |            pAb.                                                   | */
/* |            pAttr pointe le bloc attribut auquel correspond la regle| */
/* |             pR (NULL si la regle n'est pas une regle d'attribut).  | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static boolean      NouvRef (PtrAbstractBox pAb, PtrPRule pR, PtrPSchema pSPR, Level Niv, PtrDocument pDoc, PtrAttribute pAttr)

#else  /* __STDC__ */
static boolean      NouvRef (pAb, pR, pSPR, Niv, pDoc, pAttr)
PtrAbstractBox             pAb;
PtrPRule        pR;
PtrPSchema          pSPR;
Level              Niv;
PtrDocument         pDoc;
PtrAttribute         pAttr;

#endif /* __STDC__ */

{
   boolean             ret;
   AbPosition         Posit;
   AbDimension        Dimens;
   PosRule           *pRe1;
   DimensionRule       *pRelD1;
   PtrPRule        pRegleDim;
   PtrPSchema          pSPRDim;
   PtrAttribute         pAttrDim;
   AbPosition         DimensElast;

#ifdef __COLPAGE__
   boolean             bool;

#else  /* __COLPAGE__ */
   boolean             aprespage;

#endif /* __COLPAGE__ */

   ret = FALSE;
   if (pR != NULL)
     {
#ifndef __COLPAGE__
	/* s'il s'agit d'un positionnement vertical et que le pave precedent */
	/* est une marque de page, il faut reevaluer la regle */
	aprespage = FALSE;
	if (pR->PrType == PtVertPos)
	   if (pAb->AbPrevious != NULL)
	      if (pAb->AbPrevious->AbElement->ElTypeNumber == PageBreak + 1)
		 aprespage = TRUE;
#endif /* __COLPAGE__ */
	switch (pR->PrType)
	      {
		 case PtVertRef:
		 case PtHorizRef:
		 case PtVertPos:
		 case PtHorizPos:
		    pRe1 = &pR->PrPosRule;
		    if (pRe1->PoRelation == Niv || pRe1->PoRelation == RlReferred
			|| (pRe1->PoRelation == RlSameLevel
			    && (Niv == RlPrevious || Niv == RlNext))
#ifndef __COLPAGE__
			|| aprespage
#endif /* __COLPAGE__ */
			|| (pR->PrType == PtVertPos && pAb->AbHeight.DimIsPosition)
			|| (pR->PrType == PtHorizPos && pAb->AbWidth.DimIsPosition))
		       switch (pR->PrType)
			     {
				case PtVertRef:
				   Posit = pAb->AbVertRef;
#ifdef __COLPAGE__
				   (void) Applique (pR, pSPR, pAb, pDoc, pAttr, &bool);
#else  /* __COLPAGE__ */
				   (void) Applique (pR, pSPR, pAb, pDoc, pAttr);
#endif /* __COLPAGE__ */
				   ret = PositionDiff (pAb->AbVertRef, &Posit, FALSE);
				   break;
				case PtHorizRef:
				   Posit = pAb->AbHorizRef;
#ifdef __COLPAGE__
				   (void) Applique (pR, pSPR, pAb, pDoc, pAttr, &bool);
#else  /* __COLPAGE__ */
				   (void) Applique (pR, pSPR, pAb, pDoc, pAttr);
#endif /* __COLPAGE__ */
				   ret = PositionDiff (pAb->AbHorizRef, &Posit, FALSE);
				   break;
				case PtVertPos:
				   if (pAb->AbHeight.DimIsPosition)
				      /* on reevalue la dimension d'un pave elastique en meme temps */
				      /* que sa position */
				     {
					/* on conserve la position et la dimension d'origine du pave' */
					Posit = pAb->AbVertPos;
					DimensElast = pAb->AbHeight.DimPosition;
					/* applique la regle de position */
#ifdef __COLPAGE__
					(void) Applique (pR, pSPR, pAb, pDoc, pAttr, &bool);
#else  /* __COLPAGE__ */
					(void) Applique (pR, pSPR, pAb, pDoc, pAttr);
#endif /* __COLPAGE__ */
					/* cherche et applique la regle de dimension */
					pRegleDim = LaRegle (pDoc, pAb, &pSPRDim, PtHeight, TRUE, &pAttrDim);
#ifdef __COLPAGE__
					(void) Applique (pRegleDim, pSPRDim, pAb, pDoc, pAttrDim, &bool);
#else  /* __COLPAGE__ */
					(void) Applique (pRegleDim, pSPRDim, pAb, pDoc, pAttrDim);
#endif /* __COLPAGE__ */
					/* compare la position et la dimension d'origine avec celles qui */
					/* qui viennent d'etre calculees */
					if (pAb->AbBox != NULL)
					   if (pAb->AbBox->BxVertInverted)
					      /* le mediateur avait inverse' position et dimension */
					     {
						/* compare avec inversion */
						if (PositionDiff (pAb->AbVertPos, &Posit, TRUE) ||
						    PositionDiff (pAb->AbHeight.DimPosition, &DimensElast, TRUE))
						   /* il y a eu au moins un changement */
						  {
						     ret = TRUE;
						     pAb->AbHeightChange = TRUE;
						     pAb->AbVertPosChange = TRUE;
						     if (pAb->AbLeafType == LtGraphics)
							/* retablit le caractere graphique qui a ete inverse' */
							if (pAb->AbElement->ElTerminal)
							   if (pAb->AbElement->ElLeafType == LtGraphics)
							      pAb->AbShape = pAb->AbElement->ElGraph;
						  }
						else
						   /* pas de changement, on retablit position et dimension */
						   /* d'origine */
						  {
						     ret = FALSE;
						     pAb->AbVertPos = Posit;
						     pAb->AbHeight.DimPosition = DimensElast;
						  }
					     }
					   else
					      /* pas d'inversion position/dimension */
					     {
						if (PositionDiff (pAb->AbVertPos, &Posit, FALSE) ||
						    PositionDiff (pAb->AbHeight.DimPosition, &DimensElast, FALSE))
						   /* il y a eu au moins un changement */
						  {
						     ret = TRUE;
						     pAb->AbHeightChange = TRUE;
						     pAb->AbVertPosChange = TRUE;
						  }
						else
						   /* pas de changement */
						   ret = FALSE;
					     }
				     }
				   else
				     {
					Posit = pAb->AbVertPos;
#ifdef __COLPAGE__
					(void) Applique (pR, pSPR, pAb, pDoc, pAttr, &bool);
#else  /* __COLPAGE__ */
					(void) Applique (pR, pSPR, pAb, pDoc, pAttr);
#endif /* __COLPAGE__ */
					ret = PositionDiff (pAb->AbVertPos, &Posit, FALSE);
				     }
				   break;
				case PtHorizPos:
				   if (pAb->AbWidth.DimIsPosition)
				      /* on reevalue la dimension d'un pave elastique en meme temps */
				      /* que sa position */
				     {
					/* on conserve la position et la dimension d'origine du pave' */
					Posit = pAb->AbHorizPos;
					DimensElast = pAb->AbWidth.DimPosition;
					/* applique la regle de position */
#ifdef __COLPAGE__
					(void) Applique (pR, pSPR, pAb, pDoc, pAttr, &bool);
#else  /* __COLPAGE__ */
					(void) Applique (pR, pSPR, pAb, pDoc, pAttr);
#endif /* __COLPAGE__ */
					/* cherche et applique la regle de dimension */
					pRegleDim = LaRegle (pDoc, pAb, &pSPRDim, PtWidth, TRUE, &pAttrDim);
#ifdef __COLPAGE__
					(void) Applique (pRegleDim, pSPRDim, pAb, pDoc, pAttrDim, &bool);
#else  /* __COLPAGE__ */
					(void) Applique (pRegleDim, pSPRDim, pAb, pDoc, pAttrDim);
#endif /* __COLPAGE__ */
					/* compare la position et la dimension d'origine avec celles qui */
					/* qui viennent d'etre calculees */
					if (pAb->AbBox != NULL)
					   if (pAb->AbBox->BxHorizInverted)
					      /* le mediateur avait inverse' position et dimension */
					     {
						/* compare avec inversion */
						if (PositionDiff (pAb->AbHorizPos, &Posit, TRUE) ||
						    PositionDiff (pAb->AbWidth.DimPosition, &DimensElast, TRUE))
						   /* il y a eu au moins un changement */
						  {
						     ret = TRUE;
						     pAb->AbWidthChange = TRUE;
						     pAb->AbHorizPosChange = TRUE;
						     if (pAb->AbLeafType == LtGraphics)
							/* retablit le caractere graphique qui a ete inverse' */
							if (pAb->AbElement->ElTerminal)
							   if (pAb->AbElement->ElLeafType == LtGraphics)
							      pAb->AbShape = pAb->AbElement->ElGraph;
						  }
						else
						   /* pas de changement, on retablit position et dimension */
						   /* d'origine */
						  {
						     ret = FALSE;
						     pAb->AbHorizPos = Posit;
						     pAb->AbWidth.DimPosition = DimensElast;
						  }
					     }
					   else
					      /* pas d'inversion position/dimension */
					     {
						if (PositionDiff (pAb->AbHorizPos, &Posit, FALSE) ||
						    PositionDiff (pAb->AbWidth.DimPosition, &DimensElast, FALSE))
						   /* il y a eu au moins un changement */
						  {
						     ret = TRUE;
						     pAb->AbWidthChange = TRUE;
						     pAb->AbHorizPosChange = TRUE;
						  }
						else
						   /* pas de changement */
						   ret = FALSE;
					     }
				     }
				   else
				     {
					Posit = pAb->AbHorizPos;
#ifdef __COLPAGE__
					(void) Applique (pR, pSPR, pAb, pDoc, pAttr, &bool);
#else  /* __COLPAGE__ */
					(void) Applique (pR, pSPR, pAb, pDoc, pAttr);
#endif /* __COLPAGE__ */
					ret = PositionDiff (pAb->AbHorizPos, &Posit, FALSE);
				     }
				   break;
				default:
				   break;
			     }

		    break;
		 case PtHeight:
		 case PtWidth:
		    /* sauve d'abord la dimension du pave' */
		    if (pR->PrType == PtHeight)
		       Dimens = pAb->AbHeight;
		    else
		       Dimens = pAb->AbWidth;
		    /* traitement selon la regle de dimension */
		    pRelD1 = &pR->PrDimRule;
		    if (pRelD1->DrPosition)
		      {
			 /* regle de dimension elastique */
			 if (!Dimens.DimIsPosition)
			    /* pave non elastique */
			    /* on applique la regle */
			   {
#ifdef __COLPAGE__
			      (void) Applique (pR, pSPR, pAb, pDoc, pAttr, &bool);
#else  /* __COLPAGE__ */
			      (void) Applique (pR, pSPR, pAb, pDoc, pAttr);
#endif /* __COLPAGE__ */
			      ret = TRUE;
			   }
		      }
		    else
		       /* regle de dimension non elastique */
		    if (!Dimens.DimIsPosition)
		       if (!pRelD1->DrAbsolute)
			  /* ce n'est pas une dimension absolue */
			  if (pRelD1->DrRelation == Niv
			      || (pRelD1->DrRelation == RlSameLevel
				  && (Niv == RlPrevious || Niv == RlNext)))
			     if (pR->PrType == PtHeight)
				/* hauteur */
			       {
#ifdef __COLPAGE__
				  (void) Applique (pR, pSPR, pAb, pDoc, pAttr, &bool);
#else  /* __COLPAGE__ */
				  (void) Applique (pR, pSPR, pAb, pDoc, pAttr);
#endif /* __COLPAGE__ */
				  ret = DimensionDiff (pAb->AbHeight, &Dimens);
			       }
			     else
				/* largeur */
			       {
#ifdef __COLPAGE__
				  (void) Applique (pR, pSPR, pAb, pDoc, pAttr, &bool);
#else  /* __COLPAGE__ */
				  (void) Applique (pR, pSPR, pAb, pDoc, pAttr);
#endif /* __COLPAGE__ */
				  ret = DimensionDiff (pAb->AbWidth, &Dimens);
			       }
		    break;
		 default:
		    break;
	      }

     }
   return (ret);
}



/* ---------------------------------------------------------------------- */
/* |    ReapplRef reapplique toutes les regles appliquees au pave pAb  | */
/* |            et qui font reference au pave pRef.                     | */
/* |            Retourne vrai si au moins une regle a ete reappliquee.  | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static boolean      ReapplRef (PtrAbstractBox pRef, PtrAbstractBox pAb, PtrDocument pDoc)

#else  /* __STDC__ */
static boolean      ReapplRef (pRef, pAb, pDoc)
PtrAbstractBox             pRef;
PtrAbstractBox             pAb;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   boolean             ret;
   PtrPSchema          pSPR;
   PtrAttribute         pAttr;
   PtrPRule        pR;
   PtrAbstractBox             pAbbox1;
   AbPosition        *pPosAb;
   AbDimension       *pDimAb;

#ifdef __COLPAGE__
   boolean             bool;

#endif /* __COLPAGE__ */

   ret = FALSE;
   pAbbox1 = pAb;
   if (pAbbox1->AbVertRef.PosAbRef == pRef)
     {
	pPosAb = &pAbbox1->AbVertRef;
	/* prend d'abord la regle standard */
	pPosAb->PosEdge = VertRef;
	pPosAb->PosRefEdge = VertRef;
	pPosAb->PosDistance = 0;
	pPosAb->PosAbRef = NULL;
	pPosAb->PosUserSpecified = FALSE;
	pR = LaRegle (pDoc, pAb, &pSPR, PtVertRef, TRUE, &pAttr);
#ifdef __COLPAGE__
	appl = Applique (pR, pSPR, pAb, pDoc, pAttr, &bool);
#else  /* __COLPAGE__ */
	(void) Applique (pR, pSPR, pAb, pDoc, pAttr);
#endif /* __COLPAGE__ */
	pAbbox1->AbVertRefChange = TRUE;
	ret = TRUE;
     }
   if (pAbbox1->AbHorizRef.PosAbRef == pRef)
     {
	pPosAb = &pAbbox1->AbHorizRef;
	/* prend d'abord la regle standard */
	pPosAb->PosEdge = HorizRef;
	pPosAb->PosRefEdge = HorizRef;
	pPosAb->PosDistance = 0;
	pPosAb->PosAbRef = NULL;
	pPosAb->PosUserSpecified = FALSE;
	pR = LaRegle (pDoc, pAb, &pSPR, PtHorizRef, TRUE, &pAttr);
#ifdef __COLPAGE__
	(void) Applique (pR, pSPR, pAb, pDoc, pAttr, &bool);
#else  /* __COLPAGE__ */
	(void) Applique (pR, pSPR, pAb, pDoc, pAttr);
#endif /* __COLPAGE__ */
	pAbbox1->AbHorizRefChange = TRUE;
	ret = TRUE;
     }
   if (pAbbox1->AbHeight.DimIsPosition)
     {
	pPosAb = &pAbbox1->AbHeight.DimPosition;
	if (pPosAb->PosAbRef == pRef)
	  {
	     pPosAb->PosEdge = Top;
	     pPosAb->PosRefEdge = Top;
	     pPosAb->PosDistance = 0;
	     pPosAb->PosAbRef = NULL;
	     pPosAb->PosUserSpecified = FALSE;
	     pR = LaRegle (pDoc, pAb, &pSPR, PtHeight, TRUE, &pAttr);
#ifdef __COLPAGE__
	     (void) Applique (pR, pSPR, pAb, pDoc, pAttr, &bool);
#else  /* __COLPAGE__ */
	     (void) Applique (pR, pSPR, pAb, pDoc, pAttr);
#endif /* __COLPAGE__ */
	     pAbbox1->AbHeightChange = TRUE;
	     /* force la reevaluation de la position verticale : lorsqu'on */
	     /* change la dimension d'une boite elastique, il faut aussi */
	     /* faire reevaluer sa position par le mediateur. */
	     pAbbox1->AbVertPos.PosAbRef = pRef;
	     ret = TRUE;
	  }
     }
   else if (pAbbox1->AbHeight.DimAbRef == pRef)
     {
	pDimAb = &pAbbox1->AbHeight;
	pDimAb->DimValue = 0;
	pDimAb->DimAbRef = NULL;
	pDimAb->DimUnit = UnRelative;
	pDimAb->DimSameDimension = TRUE;
	pDimAb->DimUserSpecified = FALSE;
	pR = LaRegle (pDoc, pAb, &pSPR, PtHeight, TRUE, &pAttr);
#ifdef __COLPAGE__
	(void) Applique (pR, pSPR, pAb, pDoc, pAttr, &bool);
#else  /* __COLPAGE__ */
	(void) Applique (pR, pSPR, pAb, pDoc, pAttr);
#endif /* __COLPAGE__ */
	pAbbox1->AbHeightChange = TRUE;
	ret = TRUE;
     }
   if (pAbbox1->AbWidth.DimIsPosition)
     {
	pPosAb = &pAbbox1->AbWidth.DimPosition;
	if (pPosAb->PosAbRef == pRef)
	  {
	     pPosAb->PosEdge = Left;
	     pPosAb->PosRefEdge = Left;
	     pPosAb->PosDistance = 0;
	     pPosAb->PosAbRef = NULL;
	     pR = LaRegle (pDoc, pAb, &pSPR, PtWidth, TRUE, &pAttr);
	     pPosAb->PosUserSpecified = FALSE;
#ifdef __COLPAGE__
	     (void) Applique (pR, pSPR, pAb, pDoc, pAttr, &bool);
#else  /* __COLPAGE__ */
	     (void) Applique (pR, pSPR, pAb, pDoc, pAttr);
#endif /* __COLPAGE__ */
	     pAbbox1->AbWidthChange = TRUE;
	     /* force la reevaluation de la position horizontale : lorsqu'on */
	     /* change la dimension d'une boite elastique, il faut aussi */
	     /* faire reevaluer sa position par le mediateur. */
	     pAbbox1->AbHorizPos.PosAbRef = pRef;
	     ret = TRUE;
	  }
     }
   else if (pAbbox1->AbWidth.DimAbRef == pRef)
     {
	pDimAb = &pAbbox1->AbWidth;
	pDimAb->DimValue = 0;
	pDimAb->DimAbRef = NULL;
	pDimAb->DimUnit = UnRelative;
	pDimAb->DimSameDimension = TRUE;
	pDimAb->DimUserSpecified = FALSE;
	pR = LaRegle (pDoc, pAb, &pSPR, PtWidth, TRUE, &pAttr);
#ifdef __COLPAGE__
	(void) Applique (pR, pSPR, pAb, pDoc, pAttr, &bool);
#else  /* __COLPAGE__ */
	(void) Applique (pR, pSPR, pAb, pDoc, pAttr);
#endif /* __COLPAGE__ */
	pAbbox1->AbWidthChange = TRUE;
	ret = TRUE;
     }
   if (pAbbox1->AbVertPos.PosAbRef == pRef)
     {
	pPosAb = &pAbbox1->AbVertPos;
	pPosAb->PosEdge = Top;
	pPosAb->PosRefEdge = Top;
	pPosAb->PosDistance = 0;
	pPosAb->PosAbRef = NULL;
	pPosAb->PosUserSpecified = FALSE;
	pR = LaRegle (pDoc, pAb, &pSPR, PtVertPos, TRUE, &pAttr);
#ifdef __COLPAGE__
	(void) Applique (pR, pSPR, pAb, pDoc, pAttr, &bool);
#else  /* __COLPAGE__ */
	(void) Applique (pR, pSPR, pAb, pDoc, pAttr);
#endif /* __COLPAGE__ */
	pAbbox1->AbVertPosChange = TRUE;
	if (pAbbox1->AbHeight.DimIsPosition)
	   /* force la reevaluation de la hauteur : lorsqu'on */
	   /* change la position d'une boite elastique, il faut aussi */
	   /* faire reevaluer sa dimension par le mediateur. */
	   pAbbox1->AbHeightChange = TRUE;
	ret = TRUE;
     }
   if (pAbbox1->AbHorizPos.PosAbRef == pRef)
     {
	pPosAb = &pAbbox1->AbHorizPos;
	pPosAb->PosEdge = Left;
	pPosAb->PosRefEdge = Left;
	pPosAb->PosDistance = 0;
	pPosAb->PosAbRef = NULL;
	pPosAb->PosUserSpecified = FALSE;
	pR = LaRegle (pDoc, pAb, &pSPR, PtHorizPos, TRUE, &pAttr);
#ifdef __COLPAGE__
	(void) Applique (pR, pSPR, pAb, pDoc, pAttr, &bool);
#else  /* __COLPAGE__ */
	(void) Applique (pR, pSPR, pAb, pDoc, pAttr);
#endif /* __COLPAGE__ */
	pAbbox1->AbHorizPosChange = TRUE;
	if (pAbbox1->AbWidth.DimIsPosition)
	   /* force la reevaluation de la largeur : lorsqu'on */
	   /* change la position d'une boite elastique, il faut aussi */
	   /* faire reevaluer sa dimension par le mediateur. */
	   pAbbox1->AbWidthChange = TRUE;
	ret = TRUE;
     }
   return ret;
}



/* ---------------------------------------------------------------------- */
/* |    RegleFonction retourne le pointeur sur la premiere regle        | */
/* |            de fonction de presentation associee a l'element pEl.   | */
/* |            Rend dans pSchP un pointeur sur le schema de            | */
/* |            presentation auquel appartient la regle.                | */
/* |            Retourne NULL s'il n'y a pas de regle de creation pour  | */
/* |            cet element                                             | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
PtrPRule        RegleFonction (PtrElement pEl, PtrPSchema * pSchP)

#else  /* __STDC__ */
PtrPRule        RegleFonction (pEl, pSchP)
PtrElement          pEl;
PtrPSchema         *pSchP;

#endif /* __STDC__ */

{
   PtrPRule        pRegle;
   int                 Entree;
   PtrSSchema        pSchS;

   pRegle = NULL;
   ChSchemaPres (pEl, pSchP, &Entree, &pSchS);
   if (*pSchP != NULL)
     {
	/* pRegle : premiere regle de presentation specifique a ce type */
	/* d'element */
	pRegle = (*pSchP)->PsElemPRule[Entree - 1];
	if (pRegle != NULL)
	  {
	     while (pRegle->PrType < PtFunction && pRegle->PrNextPRule != NULL)
		pRegle = pRegle->PrNextPRule;
	     if (pRegle->PrType != PtFunction)
		pRegle = NULL;
	  }
     }
   return pRegle;
}


/* ---------------------------------------------------------------------- */
/* |    TuePave marque Mort le pave pointe par pAb et met a jour       | */
/* |            le volume des paves englobants. Le pave mort sera       | */
/* |            detruit par la procedure AbstractImageUpdated, apres traitement     | */
/* |            par le Mediateur.                                       | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TuePave (PtrAbstractBox pAb)

#else  /* __STDC__ */
void                TuePave (pAb)
PtrAbstractBox             pAb;

#endif /* __STDC__ */

{
   int                 vol;
   PtrAbstractBox             pAbb;
   PtrAbstractBox             pAbbox1;

   pAb->AbDead = TRUE;
   /* met a jour le volume des paves englobants dans la vue */
   vol = pAb->AbVolume;
   if (vol > 0)
     {
	pAb->AbVolume = 0;
	pAbb = pAb->AbEnclosing;
	while (pAbb != NULL)
	  {
	     pAbbox1 = pAbb;
	     pAbbox1->AbVolume -= vol;
	     pAbb = pAbbox1->AbEnclosing;
	  }
     }
}


/* ---------------------------------------------------------------------- */
/* |    SuppRfPave cherche tous les paves qui font reference a un pave a| */
/* |            detruire pointe par pAb, et pour ces paves reapplique  | */
/* |            les regles qui font reference au pave a detruire.       | */
/* |            Retourne dans PavReaff un pointeur sur le pave a        | */
/* |            reafficher.                                             | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                SuppRfPave (PtrAbstractBox pAb, PtrAbstractBox * PavReaff, PtrDocument pDoc)

#else  /* __STDC__ */
void                SuppRfPave (pAb, PavReaff, pDoc)
PtrAbstractBox             pAb;
PtrAbstractBox            *PavReaff;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrAbstractBox             pAbb;

   *PavReaff = pAb;
   /* cherche d'abord dans le pave englobant */
   pAbb = pAb->AbEnclosing;
   if (pAbb != NULL)
      if (ReapplRef (pAb, pAbb, pDoc))
	 *PavReaff = pAbb;
   /* cherche dans les paves precedents */
   pAbb = pAb->AbPrevious;
   while (pAbb != NULL)
     {
	if (!pAbb->AbDead)
	   if (ReapplRef (pAb, pAbb, pDoc))
	      *PavReaff = pAbb->AbEnclosing;
	pAbb = pAbb->AbPrevious;
	/* cherche dans les paves suivants */
     }
   pAbb = pAb->AbNext;
   while (pAbb != NULL)
     {
	if (!pAbb->AbDead)
	   if (ReapplRef (pAb, pAbb, pDoc))
	      *PavReaff = pAbb->AbEnclosing;
	pAbb = pAbb->AbNext;
     }
}



/* ---------------------------------------------------------------------- */
/* |    NouvRfPave La suite de paves comprise entre pAbbFirst et pAbbLast   | */
/* |            vient d'etre creee, modifie les paves environnants qui  | */
/* |            peuvent se referer aux nouveaux paves.                  | */
/* |            Retourne dans PavReaff un pointeur sur le pave qui      | */
/* |            englobe tous les paves modifies, y compris les nouveaux | */
/* |            paves.                                                  | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                NouvRfPave (PtrAbstractBox pAbbFirst, PtrAbstractBox pAbbLast, PtrAbstractBox * PavReaff, PtrDocument pDoc)

#else  /* __STDC__ */
void                NouvRfPave (pAbbFirst, pAbbLast, PavReaff, pDoc)
PtrAbstractBox             pAbbFirst;
PtrAbstractBox             pAbbLast;
PtrAbstractBox            *PavReaff;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrAbstractBox             pAbb;
   PtrPRule        pRegle;
   PtrPSchema          pSPR;
   PtrAttribute         pAttr;

   /* cherche la pave minimum a reafficher a priori */
   if (pAbbLast == pAbbFirst)
      *PavReaff = pAbbFirst;
   /* on reaffichera le pave unique */
   else
      *PavReaff = pAbbFirst->AbEnclosing;
   /* il y a plusieurs paves pour cet */
   /* element, on reaffichera le pave englobant */
   /* cherche les paves environnants pour trouver ceux qui se referent aux */
   /* nouveaux paves */
   /* traite d'abord le pave englobant */
   pAbb = pAbbFirst->AbEnclosing;
   if (pAbb != NULL)
     {
	/* il y a un pave englobant */
	pRegle = LaRegle (pDoc, pAbb, &pSPR, PtVertRef, TRUE, &pAttr);
	if (NouvRef (pAbb, pRegle, pSPR, RlEnclosed, pDoc, pAttr))
	  {
	     *PavReaff = pAbb;
	     pAbb->AbVertRefChange = TRUE;
	  }
	pRegle = LaRegle (pDoc, pAbb, &pSPR, PtHorizRef, TRUE, &pAttr);
	if (NouvRef (pAbb, pRegle, pSPR, RlEnclosed, pDoc, pAttr))
	  {
	     *PavReaff = pAbb;
	     pAbb->AbHorizRefChange = TRUE;
	  }
	pRegle = LaRegle (pDoc, pAbb, &pSPR, PtHeight, TRUE, &pAttr);
	if (NouvRef (pAbb, pRegle, pSPR, RlEnclosed, pDoc, pAttr))
	  {
	     *PavReaff = pAbb;
	     pAbb->AbHeightChange = TRUE;
	  }
	pRegle = LaRegle (pDoc, pAbb, &pSPR, PtWidth, TRUE, &pAttr);
	if (NouvRef (pAbb, pRegle, pSPR, RlEnclosed, pDoc, pAttr))
	  {
	     *PavReaff = pAbb;
	     pAbb->AbWidthChange = TRUE;
	  }
     }
   /* cherche dans les paves precedents */
   if (pAbbFirst->AbPrevious != NULL)
     {
	pAbb = pAbbFirst->AbPrevious;
	do
	  {
	     if (!pAbb->AbDead)
		/* on ne traite pas les paves morts */
	       {
		  pRegle = LaRegle (pDoc, pAbb, &pSPR, PtVertPos, TRUE, &pAttr);
		  if (NouvRef (pAbb, pRegle, pSPR, RlNext, pDoc, pAttr))
		    {
		       *PavReaff = pAbb->AbEnclosing;
		       pAbb->AbVertPosChange = TRUE;
		    }
		  pRegle = LaRegle (pDoc, pAbb, &pSPR, PtHorizPos, TRUE, &pAttr);
		  if (NouvRef (pAbb, pRegle, pSPR, RlNext, pDoc, pAttr))
		    {
		       *PavReaff = pAbb->AbEnclosing;
		       pAbb->AbHorizPosChange = TRUE;
		    }
		  pRegle = LaRegle (pDoc, pAbb, &pSPR, PtHeight, TRUE, &pAttr);
		  if (NouvRef (pAbb, pRegle, pSPR, RlNext, pDoc, pAttr))
		    {
		       *PavReaff = pAbb->AbEnclosing;
		       pAbb->AbHeightChange = TRUE;
		    }
		  pRegle = LaRegle (pDoc, pAbb, &pSPR, PtWidth, TRUE, &pAttr);
		  if (NouvRef (pAbb, pRegle, pSPR, RlNext, pDoc, pAttr))
		    {
		       *PavReaff = pAbb->AbEnclosing;
		       pAbb->AbWidthChange = TRUE;
		    }
	       }
	     pAbb = pAbb->AbPrevious;
	  }
	while (!(pAbb == NULL));
     }
   /* cherche dans les paves suivants */
   if (pAbbLast->AbNext != NULL)
     {
	pAbb = pAbbLast->AbNext;
	do
	  {
	     if (!pAbb->AbDead)
	       {
		  pRegle = LaRegle (pDoc, pAbb, &pSPR, PtVertPos, TRUE, &pAttr);
		  if (NouvRef (pAbb, pRegle, pSPR, RlPrevious, pDoc, pAttr))
		    {
		       *PavReaff = pAbb->AbEnclosing;
		       pAbb->AbVertPosChange = TRUE;
		    }
		  pRegle = LaRegle (pDoc, pAbb, &pSPR, PtHorizPos, TRUE, &pAttr);
		  if (NouvRef (pAbb, pRegle, pSPR, RlPrevious, pDoc, pAttr))
		    {
		       *PavReaff = pAbb->AbEnclosing;
		       pAbb->AbHorizPosChange = TRUE;
		    }
		  pRegle = LaRegle (pDoc, pAbb, &pSPR, PtHeight, TRUE, &pAttr);
		  if (NouvRef (pAbb, pRegle, pSPR, RlPrevious, pDoc, pAttr))
		    {
		       *PavReaff = pAbb->AbEnclosing;
		       pAbb->AbHeightChange = TRUE;
		    }
		  pRegle = LaRegle (pDoc, pAbb, &pSPR, PtWidth, TRUE, &pAttr);
		  if (NouvRef (pAbb, pRegle, pSPR, RlPrevious, pDoc, pAttr))
		    {
		       *PavReaff = pAbb->AbEnclosing;
		       pAbb->AbWidthChange = TRUE;
		    }
	       }
	     pAbb = pAbb->AbNext;
	  }
	while (!(pAbb == NULL));
     }
}




/* ---------------------------------------------------------------------- */
/* |    PavPresType     cherche le pave de presentation de type         | */
/* |            BoxType (defini dans le schema de presentation pSchP) | */
/* |            qui a ete cree dans la meme vue que le pave pAbb,         | */
/* |            pour l'element de pAbb.                                   | */
/* |            On considere le premier pave (pAbb) et le dernier pave    | */
/* |            de la chaine des paves dupliques de l'element           | */
/* |            Retourne un pointeur sur ce pave ou NULL si pas trouve'.| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static PtrAbstractBox      PavPresType (PtrAbstractBox pAbb, PtrPRule pRPres, PtrPSchema pSchP)

#else  /* __STDC__ */
static PtrAbstractBox      PavPresType (pAbb, pRPres, pSchP)
PtrAbstractBox             pAbb;
PtrPRule        pRPres;
PtrPSchema          pSchP;

#endif /* __STDC__ */

{
   PtrAbstractBox             pAb, pPavPrincipal;
   boolean             trouve, stop;	/* 1er pave de l'element dans la vue */
   int        typeBoite;
   PtrAbstractBox             pAbbPres;


   typeBoite = pRPres->PrPresBox[0];	/* numero de type de la boite cherchee */
   /* Cas particulier des paves de presentation de la racine */
   /* TO DO ? */
   pAb = pAbb;
   trouve = FALSE;
   /* cherche d'abord le pave principal de l'element */
   pPavPrincipal = NULL;
   stop = FALSE;
   do
      if (pAb == NULL)
	 stop = TRUE;		/* pas trouve' */
      else if (pAb->AbElement != pAbb->AbElement)
	 stop = TRUE;		/* ce n'est pas un pave de l'element */
      else if (pAb->AbPresentationBox)
	 /* c'est un pave' de presentation, on le saute */
	 pAb = pAb->AbNext;
      else
	{
	   /* premier pave' de l'element qui ne soit pas un pave' de */
	   /* presentation, c'est le pave' principal */
	   pPavPrincipal = pAb;
	   stop = TRUE;
	}
   while (!stop);
   if (pPavPrincipal != NULL)
      /* on a trouve' le pave principal */
     {
#ifdef __COLPAGE__
	if (pRPres->PrPresFunction == FnCreateFirst)
	   pAb = pPavPrincipal->AbFirstEnclosed;	/* 1er descendant */
	else if (pRPres->PrPresFunction == FnCreateLast)
	  {
	     while (pPavPrincipal->AbNextRepeated != NULL)
		pPavPrincipal = pPavPrincipal->AbNextRepeated;
	     pAb = pPavPrincipal->AbFirstEnclosed;	/* 1er descendant */
	     /* cherche le dernier descendant du pave principal */
	     if (pAb != NULL)
		while (pAb->AbNext != NULL)
		   pAb = pAb->AbNext;
	  }
	else if (pRPres->PrPresFunction == FnCreateBefore)
	   pAb = pPavPrincipal->AbPrevious;
	else if (pRPres->PrPresFunction == FnCreateAfter)
	  {
	     while (pPavPrincipal->AbNextRepeated != NULL)
		pPavPrincipal = pPavPrincipal->AbNextRepeated;
	     pAb = pPavPrincipal->AbNext;
	  }
#else  /* __COLPAGE__ */
	/* positionne pAb sur le 1er pave a tester selon la regle de creation */
	if (pRPres->PrPresFunction == FnCreateFirst || pRPres->PrPresFunction == FnCreateLast)
	  {
	     pAb = pPavPrincipal->AbFirstEnclosed;	/* 1er descendant */
	     if (pRPres->PrPresFunction == FnCreateLast)
		/* cherche le dernier descendant du pave principal */
		if (pAb != NULL)
		   while (pAb->AbNext != NULL)
		      pAb = pAb->AbNext;
	  }
	else if (pRPres->PrPresFunction == FnCreateBefore)
	   pAb = pPavPrincipal->AbPrevious;
	else if (pRPres->PrPresFunction == FnCreateAfter || pRPres->PrPresFunction == FnCreateWith)
	   pAb = pPavPrincipal->AbNext;
#endif /* __COLPAGE__ */
	else
	   pAb = NULL;
	/* lance la recherche */
	while (pAb != NULL && !trouve)
	   if (pAb->AbElement != pAbb->AbElement)
	      /* ce pave n'appartient pas au meme element, echec */
	      pAb = NULL;
	   else
	      /* ce pave' appartient a l'element */
	      if (pAb->AbPresentationBox && pAb->AbTypeNum == typeBoite
		  && pAb->AbPSchema == pSchP)
	      trouve = TRUE;	/* pave' du type cherche': on a trouve' */
	   else
	      /* passe au suivant dans la bonne direction */
	      if (pRPres->PrPresFunction == FnCreateFirst ||
		  pRPres->PrPresFunction == FnCreateAfter)
	      pAb = pAb->AbNext;
	   else
	      pAb = pAb->AbPrevious;
     }
   if (trouve)
      pAbbPres = pAb;
   else
      pAbbPres = NULL;
   return pAbbPres;
}



/* ---------------------------------------------------------------------- */
/* |    TraiteChaineRegle                                               | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         TraiteChaineRegle (PtrPRule pRegle, PtrPSchema pSchP, PtrAttribute pAttr, PtrDocument pDoc, PtrElement pEl, boolean Plus, boolean Prem)

#else  /* __STDC__ */
static void         TraiteChaineRegle (pRegle, pSchP, pAttr, pDoc, pEl, Plus, Prem)
PtrPRule        pRegle;
PtrPSchema          pSchP;
PtrAttribute         pAttr;
PtrDocument         pDoc;
PtrElement          pEl;
boolean             Plus;
boolean             Prem;

#endif /* __STDC__ */

{
   int                 vue;
   PtrAbstractBox             pAb, PavReaff;
   boolean             stop;
   PtrPRule        pRe1;
   PtrCondition        pCond;

   if (pRegle != NULL)
      /* la chaine des regles de presentation n'est pas vide */
      if (pRegle->PrType == PtFunction)
	 /* la premiere regle de la chaine est bien une fonction */
	 /* cherche parmi les fonctions les regles de creation conditionelle */
	 do
	   {
	      pRe1 = pRegle;
	      if (pRe1->PrPresFunction == FnCreateBefore
		  || pRe1->PrPresFunction == FnCreateWith
		  || pRe1->PrPresFunction == FnCreateFirst
		  || pRe1->PrPresFunction == FnCreateLast
		  || pRe1->PrPresFunction == FnCreateAfter)
		 /* c'est une regle de creation */
		{
		   pCond = pRe1->PrCond;
		   while (pCond != NULL)
		     {
			/* verifie toutes les conditions qui s'appliquent a cette regle */

			if ((pCond->CoCondition == PcFirst && Prem) ||
			    (pCond->CoCondition == PcLast && !Prem))
			  {
			     if ((!pCond->CoNotNegative && Plus) ||
				 (pCond->CoNotNegative && !Plus))
				/* il faut creer la boite de presentation */
				/* dans toutes les vues, modifie les paves existants en */
				/* fonction des nouveaux paves crees. */
				for (vue = 1; vue <= MAX_VIEW_DOC; vue++)
				   /* cree le pave de presentation dans la vue */
				  {
				     pAb = CrPavPres (pEl, pDoc, pRegle, pEl->ElSructSchema,
					    pAttr, vue, pSchP, FALSE, TRUE);
				     if (pAb != NULL)
					/* un pave de presentation a ete cree */
					/* change les regles des autres paves qui peuvent */
					/* se referer au pave cree' */
				       {
					  NouvRfPave (pAb, pAb, &PavReaff, pDoc);
					  PavReaff = Englobant (pAb, PavReaff);
					  /* conserve le pointeur sur le pave a reafficher */
					  if (VueAssoc (pEl))
					     pDoc->DocAssocModifiedAb[pEl->ElAssocNum - 1] =
						Englobant (PavReaff,
						  pDoc->DocAssocModifiedAb[pEl->
							   ElAssocNum - 1]);
					  else
					     pDoc->DocViewModifiedAb[vue - 1] =
						Englobant (PavReaff, pDoc->DocViewModifiedAb[vue - 1]);
				       }
				  }
			     else
				/* il faut detruire la boite de presentation */
				/* cherche dans toutes les vues le pave qui a ete cree */
				/* par cette regle */
				for (vue = 1; vue <= MAX_VIEW_DOC; vue++)
				  {
				     pAb = PavPresType (pEl->ElAbstractBox[vue - 1], pRe1, pSchP);
				     if (pAb != NULL)
					/* detruit le pave trouve' */
				       {
					  TuePave (pAb);
					  /* change les regles des autres paves qui se */
					  /* referent au pave detruit */
					  SuppRfPave (pAb, &PavReaff, pDoc);
					  PavReaff = Englobant (pAb, PavReaff);
					  /* conserve le pointeur sur le pave a reafficher */
					  if (VueAssoc (pEl))
					     pDoc->DocAssocModifiedAb[pEl->ElAssocNum - 1] =
						Englobant (PavReaff, pDoc->
							   DocAssocModifiedAb[pEl->ElAssocNum - 1]);
					  else
					     pDoc->DocViewModifiedAb[vue - 1] =
						Englobant (PavReaff, pDoc->DocViewModifiedAb[vue - 1]);
				       }
				  }
			  }
			/* passe a la condition suivante pour la meme regle */
			pCond = pCond->CoNextCondition;
		     }
		}
	      pRegle = pRegle->PrNextPRule;
	      /* regle de presentation suivante */
	      if (pRegle == NULL)
		 stop = TRUE;
	      else if (pRegle->PrType > PtFunction)
		 stop = TRUE;
	      else
		 stop = FALSE;
	   }
	 while (!(stop));
}


/* ---------------------------------------------------------------------- */
/* |    PlusPremDer cree ou detruit les paves de presentation de        | */
/* |            l'element pEl qui sont conditionnes au fait qu'il soit  | */
/* |            le premier (si Prem est vrai) ou le dernier (si Prem est| */
/* |            faux) descendant de son pere.                           | */
/* |            Plus indique si l'element cesse d'etre premier ou       | */
/* |            dernier (vrai) ou au contraire s'il le devient (faux).  | */
/* |            pDoc pointe sur le descripteur du document ou on        | */
/* |            travaille.                                              | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                PlusPremDer (PtrElement pEl, PtrDocument pDoc, boolean Prem, boolean Plus)

#else  /* __STDC__ */
void                PlusPremDer (pEl, pDoc, Prem, Plus)
PtrElement          pEl;
PtrDocument         pDoc;
boolean             Prem;
boolean             Plus;

#endif /* __STDC__ */

{
   PtrPRule        pRPres;
   PtrPSchema          pSchP;
   PtrAttribute         pAttr;
   PtrElement          pElAttr;
   int                 l;
   InheritAttrTable     *table_herites;

   if (pEl != NULL)
     {
	/* cherche la 1ere fonction de presentation associee au type de */
	/* l'element */
	pRPres = RegleFonction (pEl, &pSchP);
	if (pSchP != NULL)
	  {
	     /* traite les regles de creation associees au type de l'element */
	     pAttr = NULL;
	     TraiteChaineRegle (pRPres, pSchP, pAttr, pDoc, pEl, Plus, Prem);
	     /* l'element herite-t-il d'attributs qui ont des fonctions de */
	     /* presentation */
	     if (pEl->ElSructSchema->SsPSchema->PsNInheritedAttrs[pEl->ElTypeNumber - 1])
	       {
		  /* il y a heritage possible */
		  if ((table_herites =
		       pEl->ElSructSchema->SsPSchema->PsInheritedAttr[pEl->ElTypeNumber - 1]) == NULL)
		    {
		       /* cette table n'existe pas on la genere */
		       CreateInheritedAttrTable (pEl);
		       table_herites = pEl->ElSructSchema->SsPSchema->PsInheritedAttr[pEl->ElTypeNumber - 1];
		    }
		  for (l = 1; l <= pEl->ElSructSchema->SsNAttributes; l++)
		     if ((*table_herites)[l - 1])	/* pEl herite de l'attribut l */
			/* cherche si l existe au dessus */
			if ((pAttr = GetTypedAttrAncestor (pEl, l, pEl->ElSructSchema,
							 &pElAttr)) != NULL)
			  {
			     pSchP = pAttr->AeAttrSSchema->SsPSchema;
			     pRPres = ReglePresAttr (pAttr, pEl, TRUE, NULL, pSchP);
			     /* traite les regles de creation associees a l'attribut */
			     TraiteChaineRegle (pRPres, pSchP, pAttr, pDoc, pEl, Plus, Prem);
			  }
	       }

	     /* l'element a-t-il des attributs qui ont des fonctions de */
	     /* presentation ? */
	     pAttr = pEl->ElFirstAttr;	/* 1er attribut de l'element */
	     /* boucle sur les attributs de l'element */
	     while (pAttr != NULL)
	       {
		  pSchP = pAttr->AeAttrSSchema->SsPSchema;
		  /* cherche le debut des regles de presentation associees a */
		  /* l'attribut */
		  pRPres = ReglePresAttr (pAttr, pEl, FALSE, NULL, pSchP);
		  /* traite les regles de creation associees a l'attribut */
		  TraiteChaineRegle (pRPres, pSchP, pAttr, pDoc, pEl, Plus, Prem);
		  /* passe a l'attribut suivant de l'element */
		  pAttr = pAttr->AeNext;
	       }
	  }
     }
}


/* ---------------------------------------------------------------------- */
/* |    PageElAssoc  cherche si l'element associe pEl doit etre affiche | */
/* |            dans une boite de haut ou de bas de page, pour la vue   | */
/* |            VueNb.                                                  | */
/* |            Si oui, retourne un pointeur sur l'element marque de    | */
/* |            page ou l'element associe doit s'afficher et retourne   | */
/* |            dans typeBoite le numero du type de boite de haut ou de | */
/* |            bas de page qui doit contenir l'element associe.        | */
/* |            Sinon, retourne Nil.                                    | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
PtrElement          PageElAssoc (PtrElement pEl, int VueNb, int *typeBoite)

#else  /* __STDC__ */
PtrElement          PageElAssoc (pEl, VueNb, typeBoite)
PtrElement          pEl;
int                 VueNb;
int                *typeBoite;

#endif /* __STDC__ */

{
   PtrPSchema          pSchP;
   int                 Entree, b;
   PtrElement          pElPage, pAsc, pElRef;
   PtrPRule        pR, pRP;
   PtrSSchema        pSchS;
   PtrReference        pRef;
   boolean             stop, stop1, ref;
   PtrPSchema          pSc1;
   PresentationBox             *pBo1;

   pElPage = NULL;
   *typeBoite = 0;
   /* cherche le schema de presentation a appliquer a l'element */
   ChSchemaPres (pEl, &pSchP, &Entree, &pSchS);
   if (pSchP != NULL)
      if (!VueAssoc (pEl))
	 /* il s'agit d'un element associe' a afficher dans une boite de */
	 /* haut ou de bas de page */
	{
	   pElRef = NULL;
	   /* l'element referencant n'est pas encore trouve' */
	   if (pEl->ElReferredDescr != NULL)
	      /* il y a des references a cet element */
	      if (pEl->ElReferredDescr->ReFirstReference != NULL)
		 /* on exclut les references par attribut et on evite les */
		 /* references appartenant a des elements associes */
		{
		   ref = FALSE;
		   pRef = pEl->ElReferredDescr->ReFirstReference;
		   /* 1ere reference */
		   do
		     {
			if (pRef->RdAttribute == NULL)

			   /* ce n'est pas une reference par attribut */
			  {
			     pElRef = pRef->RdElement;
			     /* l'element referencant */
			     if (pElRef->ElAssocNum == 0)
				/* l'element referencant n'est pas dans un element */
				/* associe', on a trouve' */
				ref = TRUE;
			  }
			if (!ref)
			   /* reference suivante a l'element */
			   pRef = pRef->RdNext;
		     }
		   while (!(ref || pRef == NULL));
		}
	   if (pElRef == NULL)
	      /* cet element n'est pas reference' par un autre element */
	     {
		pSc1 = pSchP;
		if (pSc1->PsInPageHeaderOrFooter[pEl->ElTypeNumber - 1])
		   /* l'element devrait s'afficher dans un haut ou bas de page */
		   /* cherche dans le schema de presentation le type de boite de */
		   /* presentation qui contient ce type d'element */
		  {
		     b = 1;
		     while (b <= pSc1->PsNPresentBoxes && *typeBoite == 0)
		       {
			  pBo1 = &pSc1->PsPresentBox[b - 1];
			  if (pBo1->PbContent == ContElement
			      && pBo1->PbContElem == pEl->ElTypeNumber)
			     *typeBoite = b;
			  else
			     b++;
		       }
		  }
	     }
	   else
	      /* cet element est reference' */
	      /* n premiere reference sur cet element */
	     {
		pElPage = pElRef;
		/* cherche le type de la boite de haut ou bas de page ou doit */
		/* s'afficher l'element associe. Pour cela cherche le premier */
		/* element qui englobe la 1ere reference et qui possede une regle */
		/* Page. */
		pAsc = pElPage;
		do
		   if (pAsc != NULL)
		      /* cherche le schema de presentation de l'element */
		     {
			ChSchemaPres (pAsc, &pSchP, &Entree, &pSchS);
			if (pSchP == NULL)
			   pR = NULL;
			else
			   /* 1ere regle de presentation de l'element */
			   pR = pSchP->PsElemPRule[Entree - 1];
			/* cherche les regles Page de cet element */
			stop = FALSE;
			do
			   if (pR == NULL)
			      stop = TRUE;
			   else if (pR->PrType > PtFunction
				    || *typeBoite != 0)
			      stop = TRUE;
			   else
			     {
				if (pR->PrType == PtFunction
				    && pR->PrPresFunction == FnPage
				    && pR->PrViewNum == VueNb)
				   /* c'est une regle Page concernant la vue traitee */
				   /* cherche les regles de creation de la boite page */
				   /* pour trouver les boites de haut et bas de page  */
				  {
				     pRP = pSchP->PsPresentBox[pR->PrPresBox[0] - 1].
					PbFirstPRule;
				     stop1 = FALSE;
				     do
					if (pRP == NULL)
					   stop1 = TRUE;
					else if (pRP->PrType > PtFunction
						 || *typeBoite != 0)
					   stop1 = TRUE;
					else
					  {
					     if (pRP->PrType == PtFunction
					       && (pRP->PrPresFunction == FnCreateBefore
					       || pRP->PrPresFunction == FnCreateAfter)
						 && pRP->PrViewNum == VueNb)
						/* c'est une regle de creation de boite de haut */
						/* ou de bas de page */
						/* la boite creee contient-elle ce type */
						/* d'elements associes ? */
					       {
						  pBo1 = &pSchP->PsPresentBox[pRP->
							    PrPresBox[0] - 1];
						  if (pBo1->PbContent == ContElement
						      && pBo1->PbContElem == pEl->ElTypeNumber)
						     *typeBoite = pRP->PrPresBox[0];
					       }
					     pRP = pRP->PrNextPRule;
					     /* regle suivante de la page */
					  }
				     while (!(stop1));
				  }
				pR = pR->PrNextPRule;
				/* regle suivante de l'element */
			     }
			while (!(stop));
			if (*typeBoite == 0)
			   pAsc = pAsc->ElParent;
			/* passe a l'element englobant */
		     }
		while (!(pAsc == NULL || *typeBoite != 0)) ;
		if (*typeBoite == 0)
		   if (pElPage->ElAssocNum != 0
		       && pElPage->ElAssocNum == pEl->ElAssocNum)
		      /* la reference est elle-meme dans un element associe' de */
		      /* meme type, on cherche l'element associe' en question */
		     {
			pAsc = pElPage;
			stop = FALSE;
			do
			   if (pAsc->ElSructSchema->
			       SsRule[pAsc->ElTypeNumber - 1].SrAssocElem)
			      stop = TRUE;
			   else
			      pAsc = pAsc->ElParent;
			while (!(stop || pAsc == NULL));
			if (pAsc == NULL)
			   pElPage = NULL;
			/* echec */
			else
			   /* cherche la marque de page ou s'affiche cet element */
			   /* *associe' qui contient la reference a pEl. */
			   pElPage = PageElAssoc (pAsc, VueNb, typeBoite);
		     }
		   else
		      pElPage = NULL;
		else
		   /* cherche, a partir de la 1ere reference, la premiere */
		   /* marque de page concernant la vue traitee */
		  {
		     pBo1 = &pSchP->PsPresentBox[*typeBoite - 1];
		     do
		       {
#ifdef __COLPAGE__
			  /* on cherche avant l'element reference que ce soit */
			  /* pour un affichage en haut ou en bas de page */
			  /* test haut ou bas de page supprime */
			  pElPage = BackSearchTypedElem (pElPage, PageBreak + 1, NULL);
			  if (pElPage == NULL)
			     stop = TRUE;
			  else if (pElPage->ElViewPSchema == VueNb
				   && (pElPage->ElPageType == PgBegin
				       || pElPage->ElPageType == PgComputed
				 || pElPage->ElPageType == PgUser))
			     stop = TRUE;
			  else
			     stop = FALSE;
#else  /* __COLPAGE__ */
			  if (pBo1->PbPageHeader)
			     pElPage = BackSearchTypedElem (pElPage, PageBreak + 1, NULL);
			  if (pBo1->PbPageFooter)
			     pElPage = FwdSearchTypedElem (pElPage, PageBreak + 1, NULL);
			  if (pElPage == NULL)
			     stop = TRUE;
			  else if (pElPage->ElViewPSchema == VueNb)
			     stop = TRUE;
			  else
			     stop = FALSE;
#endif /* __COLPAGE__ */
		       }
		     while (!(stop));
		  }
	     }
	}
   return pElPage;
}


/*              BoiteElAssoc N'EST PLUS UTILISEE            */

#ifndef __COLPAGE__

/* ---------------------------------------------------------------------- */
/* |    BoiteElAssoc verifie si l'element associe' pointe' par pEl doit | */
/* |            etre affiche' dans une boite de haut ou de bas de page, | */
/* |            pour la vue VueNb.                                      | */
/* |            Si oui, cree la boite de haut ou bas de page si elle    | */
/* |            n'existe pas et en cas de creation retourne l'adresse   | */
/* |            de son pave dans PavReaff.                              | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         BoiteElAssoc (PtrElement pEl, int VueNb, PtrDocument pDoc, PtrAbstractBox * PavReaff)

#else  /* __STDC__ */
static void         BoiteElAssoc (pEl, VueNb, pDoc, PavReaff)
PtrElement          pEl;
int                 VueNb;
PtrDocument         pDoc;
PtrAbstractBox            *PavReaff;

#endif /* __STDC__ */

{
   PtrPSchema          pSchP;
   int                 Entree, typeBoite, TypeP;
   PtrElement          pElPage;
   PtrAbstractBox             PavBoitePage, PavFilet;
   PtrPRule        pRegle;
   PtrSSchema        pSchS;
   boolean             trouve, stop;
   int                 VueSch;

   *PavReaff = NULL;
   PavFilet = NULL;
   VueSch = pDoc->DocView[VueNb - 1].DvPSchemaView;
   pElPage = PageElAssoc (pEl, VueSch, &typeBoite);
   if (pElPage != NULL)
     {
	/* L'element doit etre affiche' dans une boite de haut ou de bas de */
	/* page et pElPage pointe sur la marque de page dans l'arbre abstrait */
	ChSchemaPres (pEl, &pSchP, &Entree, &pSchS);
	if (pElPage->ElAbstractBox[VueNb - 1] == NULL)
	   /* cette marque de page n'a pas de pave, il faut les creer */
	   VerifAbsBoxe (pElPage, VueNb, pDoc, TRUE, TRUE);
	if (pElPage->ElAbstractBox[VueNb - 1] != NULL)
	   /* cherche dans les boites de la marque de page celle qui */
	   /* doit contenir notre element associe' */
	  {
	     PavBoitePage = pElPage->ElAbstractBox[VueNb - 1]->AbFirstEnclosed;
	     stop = FALSE;
	     do
		if (PavBoitePage == NULL)
		   stop = TRUE;
		else if (PavBoitePage->AbTypeNum == typeBoite)
		   stop = TRUE;
		else
		  {
		     if (!PavBoitePage->AbPresentationBox)
			PavFilet = PavBoitePage;
		     PavBoitePage = PavBoitePage->AbNext;
		  }
	     while (!(stop));
	     if (PavBoitePage == NULL)
		/* cette boite de haut ou bas de page n'existe pas, on la cree */
		/* cherche le type de boite page */
	       {
		  TypeP = TypeBPage (pElPage, VueSch, &pSchP);
		  if (TypeP > 0)
		     /* cherche parmi les regles de la boite page celle qui */
		     /* engendre ce type de boite */
		    {
		       pRegle = pSchP->PsPresentBox[TypeP - 1].PbFirstPRule;
		       trouve = FALSE;
		       if (pRegle != NULL)
			  do
			    {
			       if (pRegle->PrType > PtFunction)
				  pRegle = NULL;
			       else if (pRegle->PrType == PtFunction)
				  if (pRegle->PrPresFunction == FnCreateBefore
				      || pRegle->PrPresFunction == FnCreateAfter)
				     if (pRegle->PrPresBox[0] == typeBoite)
					if (pRegle->PrViewNum == VueSch)
					   trouve = TRUE;
			       if (pRegle != NULL && !trouve)
				  pRegle = pRegle->PrNextPRule;
			    }
			  while (!(trouve || pRegle == NULL));
		       if (pRegle != NULL)
			 {
			    pElPage->ElAbstractBox[VueNb - 1] = PavFilet;
			    PavBoitePage = CrPavPres (pElPage, pDoc, pRegle,
						 pElPage->ElSructSchema, NULL,
						 VueNb, pSchP, FALSE, TRUE);
			    *PavReaff = PavBoitePage;
			    /* il faut reafficher ce pave */
			    pElPage->ElAbstractBox[VueNb - 1] = PavFilet->
			       AbEnclosing;
			 }
		    }
	       }
	     if (PavBoitePage != NULL)
		/* la boite de haut ou bas de page existe, elle englobera */
		/* la boite de notre element */
		pEl->ElParent->ElAbstractBox[VueNb - 1] = PavBoitePage;
	  }
     }
}
#endif /* __COLPAGE__ */



/* ---------------------------------------------------------------------- */
/* |    TueNouveaux dechaine et libere la suite des paves comprise      | */
/* |            entre pAbbFirst et pAbbLast, ainsi que tous les paves       | */
/* |            englobes. Ces paves n'ont pas encore ete vus par le     | */
/* |            Mediateur, inutile de lui signaler leur disparition     | */
/* |            si cette suite correspond a un element sur plusieurs    | */
/* |            pages, on parcours la chaine des dupliques              | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         TueNouveaux (PtrAbstractBox * pAbbFirst, PtrAbstractBox * pAbbLast)

#else  /* __STDC__ */
static void         TueNouveaux (pAbbFirst, pAbbLast)
PtrAbstractBox            *pAbbFirst;
PtrAbstractBox            *pAbbLast;

#endif /* __STDC__ */

{
   int                 vol;
   PtrAbstractBox             pAb, pAbb;
   PtrAbstractBox             pAbbox1;

   pAb = *pAbbFirst;
   while (pAb != NULL)
      /* met a jour le volume des paves englobants dans la vue */
     {
	vol = pAb->AbVolume;
	if (vol > 0)
	  {
	     pAbb = pAb->AbEnclosing;
	     while (pAbb != NULL)
	       {
		  pAbbox1 = pAbb;
		  pAbbox1->AbVolume -= vol;
		  pAbb = pAbbox1->AbEnclosing;

	       }
	  }
	/* dechaine et libere le pave et ses paves englobes */
	LibAbbView (pAb);

	/* passe au pave suivant a supprimer */
	if (pAb == *pAbbLast)
	   pAb = NULL;
	/* on a traite' le dernier, on arrete */
	else
	   /* passe au pave' suivant */
#ifdef __COLPAGE__
	if (pAb->AbNext != NULL)
	   pAb = pAb->AbNext;
	else
	   /* cas ou l'element dont on detruit les paves s'etend sur */
	   /* plusieurs pages : pAb->AbNextRepeated != NULL. */
	if (pAb->AbNextRepeated != NULL)	/*normalement toujours vrai */
	   pAb = pAb->AbNextRepeated;
	/* a voir : parcours plus systematique entre pAbbFirst et pAbbLast ? */
	/* a voir : cas ou le pere devient vide ! */
#else  /* __COLPAGE__ */
	   pAb = pAb->AbNext;
#endif /* __COLPAGE__ */
     }
   *pAbbFirst = NULL;
   *pAbbLast = NULL;
}


#ifdef __COLPAGE__

/* ------------------------------------------------------------ */
/* | ContientEl recherche dans le sous-arbre de racine pEl      */
/* |            si il y a un element de type Typ.               */
/* |            Si Typ est une Marque Page, on verifie si       */
/* |            l'element trouve appartient a la vue VueSch.    */
/* ------------------------------------------------------------ */

#ifdef __STDC__
boolean             ContientEl (PtrElement pEl, int Typ, PtrSSchema pStr, int VueSch, PtrElement * pElCont)

#else  /* __STDC__ */
boolean             ContientEl (pEl, Typ, pStr, VueSch, pElCont)
PtrElement          pEl;
int         Typ;
PtrSSchema        pStr;
int                 VueSch;
PtrElement         *pElCont;

#endif /* __STDC__ */

{
   PtrElement          s;
   boolean             trouve;

   trouve = FALSE;
   (*pElCont) = NULL;
   if (EquivalentType (pEl, Typ, pStr))
      /* trouve ! C'est l'element lui-meme */
      if ((Typ != PageBreak + 1)
	  || (Typ == PageBreak + 1
	      && pEl->ElViewPSchema == VueSch))
	{
	   trouve = TRUE;
	   (*pElCont) = pEl;
	}
   if (!trouve && !pEl->ElTerminal)
      /* on cherche parmi les fils de l'element */
     {
	s = pEl->ElFirstChild;
	while (!trouve && s != NULL)
	  {
	     trouve = ContientEl (s, Typ, pStr, VueSch, pElCont);
	     s = s->ElNext;
	  }
     }
   return trouve;
}

#endif /* __COLPAGE__ */

#ifdef __COLPAGE__

/* ------------------------------------------------------------ */
/* |        Nouvelle procedure  pour les colonnes               */
/* ------------------------------------------------------------ */
/* | ContientReglePageCol recherche dans le sous-arbre de       */
/* |            racine pEl si il y a une regle Page ou Column   */
/* |            portee par un element pour la vue VueSch        */
/* |            cette procedure s'inspire de ReglePage_HautPage */
/* |            de page.c                                       */
/* ------------------------------------------------------------ */
#ifdef __STDC__
boolean             ContientReglePageCol (PtrElement pEl, int VueSch)

#else  /* __STDC__ */
boolean             ContientReglePageCol (pEl, VueSch)
PtrElement          pEl;
int                 VueSch;

#endif /* __STDC__ */

{
   PtrPRule        pRegle;
   PtrSSchema        pSchS;
   PtrPSchema          pSchP;
   PtrElement          pE;
   int                 Entree;
   boolean             trouve, stop;


   ChSchemaPres (pEl, &pSchP, &Entree, &pSchS);
   if (pSchP == NULL)
      pRegle = NULL;
   else
      pRegle = pSchP->PsElemPRule[Entree - 1];
   /* 1ere regle de pres. du type */
   trouve = FALSE;
   stop = FALSE;
   do
      if (pRegle == NULL)
	 stop = TRUE;
      else if (pRegle->PrType > PtFunction)
	 stop = TRUE;
      else if (pRegle->PrType == PtFunction &&
	       pRegle->PrViewNum == VueSch &&
	       (pRegle->PrPresFunction == FnPage
		|| pRegle->PrPresFunction == FnColumn))
	{			/* c'est une regle Page ou colonne */
	   trouve = TRUE;
	}
      else
	 pRegle = pRegle->PrNextPRule;
   while (!(stop) && !(trouve));
   if (!trouve && !pEl->ElTerminal)
     {
	pE = pEl->ElFirstChild;
	while (!trouve && pE != NULL)
	  {
	     trouve = ContientReglePageCol (pE, VueSch);
	     pE = pE->ElNext;
	  }
     }
   return trouve;
}

#endif /* __COLPAGE__ */

/* ---------------------------------------------------------------------- */
/* |    CreeTousPaves     Cree pour toutes les vues existantes tous les | */
/* |    paves de l'element pointe par pE appartenant au document pointe | */
/* |    par pDoc.                                                       | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                CreeTousPaves (PtrElement pE, PtrDocument pDoc)
#else  /* __STDC__ */
void                CreeTousPaves (pE, pDoc)
PtrElement          pE;
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   int                 vue;

   /* indique qu'il faut creer tous les paves sans limite de volume */
   if (!VueAssoc (pE))
      /* nombre de vues du document */
      for (vue = 1; vue <= MAX_VIEW_DOC; vue++)
	{
	   if (pDoc->DocView[vue - 1].DvPSchemaView > 0)
	      /* la vue est ouverte */
	      pDoc->DocViewFreeVolume[vue - 1] = THOT_MAXINT;
	}
   else
      /* vue d'elements associes */
   if (pDoc->DocAssocFrame[pE->ElAssocNum - 1] != 0)
      pDoc->DocAssocFreeVolume[pE->ElAssocNum - 1] = THOT_MAXINT;
   /* cree effectivement les paves dans toutes les vues existantes */
   CrPaveNouv (pE, pDoc, 0);
   /* applique les regles retardees concernant les paves cree's */
   ApplReglesRetard (pE, pDoc);
}


#ifdef __COLPAGE__
/* ---------------------------------------------------------------------- */
/*      procedure completement remaniee dans la V4                        */
/* ---------------------------------------------------------------------- */
/* |    CrPaveNouv   cree les paves du sous-arbre dont la racine est    | */
/* |            pointe par pEl, dans le document dont le contexte du    | */
/* |            document pDoc.                                          | */
/* |            Les paves sont crees dans toutes les vues si VueNb est  | */
/* |            nul, ou seulement dans la vue de numero VueNb           | */
/* |            si VueNb>0.                                             | */
/* |            Les paves existants affectes par la creation des        | */
/* |            nouveaux paves sont modifies. Les pointeurs sur les     | */
/* |            paves a reafficher du document sont mis a jour.         | */
/* |            si pEl est une PageBreak, l'image abstraite qui suit   | */
/* |            est detruite et ensuite recreee                         | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                CrPaveNouv (PtrElement pEl, PtrDocument pDoc, int VueNb)

#else  /* __STDC__ */
void                CrPaveNouv (pEl, pDoc, VueNb)
PtrElement          pEl;
PtrDocument         pDoc;
int                 VueNb;

#endif /* __STDC__ */

{
   int                 vue, premvue, dervue;
   PtrAbstractBox             pAb, PavReaff, PavR, pAbbFirst, pAbbLast, pAbbSibling;
   boolean             vueexiste, stop, assoc;
   boolean             complet;
   PtrAbstractBox             pAbbox1;
   int                 frame, h;
   boolean             VuePaginee, bool, trouve;
   PtrAbstractBox             pAbbRoot, pAbb1, pAbb;
   PtrElement          pEl1, pElCont;

   if (pEl != NULL)
     {
	if (VueNb == 0)
	  {
	     premvue = 1;
	     /* nombre de vues definies dans le schema de pres. du document */
	     dervue = MAX_VIEW_DOC;
	  }
	else
	  {
	     premvue = VueNb;
	     dervue = VueNb;
	  }
	/* pour toutes les vues demandees, cree les paves du sous-arbre de */
	/* l'element et reapplique les regles affectees par la creation des */
	/* nouveaux paves */
	for (vue = premvue; vue <= dervue; vue++)
	  {
	     assoc = VueAssoc (pEl);
	     if (!assoc)
		/* l'element ne s'affiche pas dans une vue */
		/* d'elements associes */
	       {
		  vueexiste = pDoc->DocView[vue - 1].DvPSchemaView > 0;
		  if (vueexiste)
		    {
		       pAbbRoot = pDoc->DocViewRootAb[vue - 1];
		       VuePaginee = (pAbbRoot->AbFirstEnclosed != NULL
				  && pAbbRoot->AbFirstEnclosed->AbElement->
				     ElTypeNumber == PageBreak + 1);
		       frame = pDoc->DocViewFrame[vue - 1];
		    }
	       }
	     else
		/* c'est une vue d'elements associes */
	       {
		  vueexiste = pDoc->DocAssocFrame[pEl->ElAssocNum - 1] != 0 && vue == 1;
		  if (vueexiste)
		    {
		       pAbbRoot = pDoc->DocAssocRoot[pEl->ElAssocNum - 1]
			  ->ElAbstractBox[0];
		       VuePaginee = (pAbbRoot->AbFirstEnclosed != NULL
				  && pAbbRoot->AbFirstEnclosed->AbElement->
				     ElTypeNumber == PageBreak + 1);
		       frame = pDoc->DocAssocFrame[pEl->ElAssocNum - 1];
		    }
	       }
	     if (vueexiste)
	       {
		  PavReaff = NULL;
		  /* il n'y a encore rien a reafficher */
		  pAbbFirst = NULL;
		  pAbbLast = NULL;
		  if (!(!assoc && pEl->ElAssocNum != 0
			&& pEl->ElParent != NULL
			&& pEl->ElParent->ElParent == NULL))
		     /* traitement particulier des elements associes */
		     /* ils seront crees au moment de la creation de la reference */
		     /* on ne fait rien ici si c'est un element reference (fils de */
		     /* la racine). Par contre, les fils de cet element sont traites */
		     /* comme des paves normaux */
		    {
		       /* mise a jour du nombre de pages si vue paginee */
		       if (VuePaginee)
			  if (assoc)
			    {
			       if (pDoc->DocAssocNPages[pEl->ElAssocNum - 1] != -1)
				  pDoc->DocAssocNPages[pEl->ElAssocNum - 1]
				     = NbPages (pAbbRoot);
			    }
			  else if (pDoc->DocViewNPages[vue - 1] != -1)
			     pDoc->DocViewNPages[vue - 1] = NbPages (pAbbRoot);
		       /* si pEl est une Marque Page, */
		       /* ou si pEl a un descendant Marque Page, */
		       /* ou si pEl (ou ses descendants) porte une regle page */
		       /* ou colonne */
		       /* on detruit toute l'image */
		       /* abstraite qui suit pour construire correctement la page */
		       if (ContientEl (pEl, PageBreak + 1, pDoc->DocSSchema,
				   pDoc->DocView[vue - 1].DvPSchemaView, &pElCont)
			   || ContientReglePageCol (pEl, pDoc->DocView[vue - 1].DvPSchemaView))
			 {	/* destruction des paves des elements qui suivent pEl */
			    /* on recherche le pave de l'element suivant */
			    pAbb1 = NULL;
			    pEl1 = pEl;
			    trouve = FALSE;
			    while (!trouve && pEl1 != NULL)
			       if (pEl1->ElNext != NULL)
				 {
				    pEl1 = pEl1->ElNext;
				    if (pEl1->ElAbstractBox[vue - 1] != NULL)
				      {
					 trouve = TRUE;
					 pAbb1 = pEl1->ElAbstractBox[vue - 1];
				      }
				 }
			       else
				 {
				    pEl1 = pEl1->ElParent;
				    if (pEl1 != NULL)
				       if (pEl1->ElAbstractBox[vue - 1] != NULL)
					 {
					    /* toujours vrai ? */
					    pAbb = pEl1->ElAbstractBox[vue - 1];
					    while (pAbb->AbPresentationBox)
					       pAbb = pAbb->AbNext;
					    /* on marque le pave coupe pour etre sur */
					    /* que le pave du nouvel element soit cree */
					    pAbb->AbTruncatedTail = TRUE;
					 }
				 }
			    /* je ne comprends plus ce code !!! */
			    /* il est incompatible avec ContientReglePageCol */
			    /* je le supprime donc. TODO : a verifier */
			    /* cas particulier ou l'element a partir duquel detruire */
			    /* contient un element marque page qui a un pave : il */
			    /* faut detruire a partir de cette marque de page */
			    /*      if (pEl1 != NULL) */
			    /*      if (ContientEl(pEl1, PageBreak+1, pDoc->DocSSchema, */
			    /*                  pDoc->DocView[vue - 1].DvPSchemaView, &pElCont)) */
			    /*          if (pElCont->ElAbstractBox[vue - 1] != NULL) */
			    /*            pAbb1 = pElCont->ElAbstractBox[vue - 1]; */
			    if (pAbb1 != NULL && pAbb1 != pAbbRoot)
			      {
				 DestrAbbNext (pAbb1, pDoc);
				 h = 0;
				 bool = ModifVue (frame, &h, pAbbRoot);
				 FreeDeadAbstractBoxes (pAbbRoot);
			      }
			    if (assoc)
			      {
				 if (pDoc->DocAssocNPages[pEl->ElAssocNum - 1] != -1)
				    pDoc->DocAssocNPages[pEl->ElAssocNum - 1]
				       = NbPages (pAbbRoot);
			      }
			    else if (pDoc->DocViewNPages[vue - 1] != -1)
			       pDoc->DocViewNPages[vue - 1] = NbPages (pAbbRoot);

			    /* TODO a discuter avec Vincent */
			    /* on repositionne la variable globale FirstCreation */
			    /* a faux car on est sur que pEl est soit une */
			    /* marque page, soit un element qui, comme il */
			    /* contient une marque page, ne peut pas */
			    /* avoir une position ou dimension donnee par */
			    /* l'utilisateur (role de FirstCreation) */
			    /* si on laisse ce booleen a vrai, tous les elements */
			    /* dont on cree les paves dans l'appel ci-dessous */
			    /* sont consideres comme nouvellement crees, donc */
			    /* les graphiques pealablement existants aussi */
			    FirstCreation = FALSE;
			    /* on cree les paves de l'element ainsi que des */
			    /* elements contenus dans la page */
			    pAb = CreePaves (pAbbRoot->AbElement, pDoc, vue, TRUE,
					      TRUE, &complet);
			 }	/* fin cas ContientEl != NULL */
		       /* ou regle page ou colonne pour pEl */
		       else
			  pAb = CreePaves (pEl, pDoc, vue, TRUE, TRUE, &complet);
		       if (pEl->ElAbstractBox[vue - 1] != NULL
			   && pEl->ElTypeNumber != PageBreak + 1)
			  /* l'element a au moins un pave dans la vue */
			  /* tout ce code est inutile pour les Marques Pages ??? */
			 {
			    /* cherche le premier pave cree pour le nouvel element */
			    pAbbFirst = pEl->ElAbstractBox[vue - 1];
			    /* cas modif 622 non traitee : pb. comment detecter */
			    /* que des paves des descendants ont ete crees alors */
			    /* que pEl a une visibilite nulle : la valeur pAb */
			    /* retournee par CreePaves n'est pas significative ! */
			    /* cherche le dernier pave cree pour le nouvel element */
			    /* il peut etre sur une autre page si l'element */
			    /* a une marque page comme fils */
			    pAbbLast = pEl->ElAbstractBox[vue - 1];
			    if (pAbbLast != NULL)
			      {
				 stop = FALSE;
				 do
				    if (pAbbLast->AbPresentationBox
					&& pAbbLast->AbNext != NULL
				    && pAbbLast->AbNext->AbElement == pEl)
				       pAbbLast = pAbbLast->AbNext;
				    else if (pAbbLast->AbNextRepeated != NULL)
				       pAbbLast = pAbbLast->AbNextRepeated;
				    else
				       stop = TRUE;
				 while (!(stop));
			      }
			    /* verifie que les paves precedents et suivants sont complets */
			    /* et les detruit s'ils sont incomplets */
			    if (pAbbFirst->AbPrevious != NULL)
			      {
				 pAbbox1 = pAbbFirst->AbPrevious;
				 /* on saute les eventuels paves de presentation */
				 while (pAbbox1 != NULL && pAbbox1->AbPresentationBox)
				    pAbbox1 = pAbbox1->AbPrevious;
				 if (pAbbox1 != NULL && pAbbox1->AbLeafType == LtCompound
				 /*                       && !pAbbox1->AbInLine  */
				     && pAbbox1->AbTruncatedTail)
				    /* le pave precedent est incomplet a la fin */
				    if (assoc || pDoc->DocView[vue - 1].DvSync)
				       /* La vue est synchronisee, on supprime tous les paves */
				       /* precedents */
				       /* code change car AbTruncatedHead ne suffit pas */
				       /* pour savoir ou s'arreter de detruire */
				       /* cas des elements sur plusieurs pages */
				       /* ainsi que des paves de presentation repetes */
				       /* on remonte la hierarchie jusqu'a la racine */
				      {
					 pAb = pAbbFirst;
					 while (pAb != NULL && pAb != pAbbRoot
					      && pAb->AbElement->ElTypeNumber !=
						PageBreak + 1)
					   {
					      pAbbSibling = pAb->AbPrevious;
					      while (pAbbSibling != NULL)
						{
						   TuePave (pAbbSibling);
						   SuppRfPave (pAbbSibling, &PavR, pDoc);
						   pAbbSibling = pAbbSibling->AbPrevious;
						}
					      pAb = pAb->AbEnclosing;
					      pAb->AbTruncatedHead = TRUE;
					   }
					 /* si la vue n'est pas paginee, on marque la racine */
					 /* coupee en tete */
					 if (pAb->AbEnclosing == NULL)
					    pAb->AbTruncatedHead = TRUE;
					 /* si la vue est paginee, on detruit les pages */
					 /* precedentes */
					 if (pAb->AbElement->ElTypeNumber == PageBreak + 1)
					   {
					      /* on saute les paves de */
					      /* presentation de la page courante: haut */
					      pAbbSibling = pAb;
					      while (pAbbSibling->AbPrevious != NULL
						     && pAbbSibling->AbPrevious->AbElement == pAb->AbElement)
						 pAbbSibling = pAbbSibling->AbPrevious;
					      pAbbSibling = pAbbSibling->AbPrevious;
					      /* dernier pave de la page precedente */
					      /* si il existe une page precedente */
					      /* on la detruit (et les precedentes) */
					      while (pAbbSibling != NULL)
						{
						   TuePave (pAbbSibling);
						   SuppRfPave (pAbbSibling, &PavR, pDoc);
						   pAbbSibling = pAbbSibling->AbPrevious;
						}
					      /* on marque la racine coupee en queue */
					      pAb->AbEnclosing->AbTruncatedTail = TRUE;
					   }
				      }		/* fin de la destruction des paves precedents */
				    else
				       /* ce n'est pas une vue synchronisee, on detruit les */
				       /* paves que l'on vient de creer */
				       TueNouveaux (&pAbbFirst, &pAbbLast);
			      }	/* fin pAbbFirst->AbPrevious != NULL */
			    if (pAbbLast != NULL)
			      {
				 pAbbox1 = pAbbLast->AbNext;
				 /* on saute les eventuels paves de presentation du suivant */
				 while (pAbbox1 != NULL && pAbbox1->AbPresentationBox)
				    pAbbox1 = pAbbox1->AbNext;
				 if ((pAbbox1 != NULL
				      && pAbbox1->AbLeafType == LtCompound
				      && pAbbox1->AbTruncatedHead
				      && (assoc || pDoc->DocView[vue - 1].DvSync))
				     || (pAbbLast->AbLeafType == LtCompound
					 && pAbbLast->AbTruncatedTail))
				   {
				      /* La vue est synchronisee, on supprime 
				         tous les paves suivants */
				      /* ou si on n'a pas pu creer tous les paves du contenu de */
				      /* l'element (ils depasseraient la capacite de la fenetre) */
				      /* il faut supprimer tous les paves suivant l'element */
				      /* code change car AbTruncatedTail ne suffit pas */
				      /* pour savoir ou s'arreter de detruire */
				      /* on remonte la hierarchie jusqu'a la racine */
				      pAb = pAbbLast;
				      while (pAb != NULL && pAb != pAbbRoot
					     && pAb->AbElement->ElTypeNumber !=
					     PageBreak + 1)
					{
					   /* supprime tous ses freres suivants */
					   /* TODO cas des paves de presentation repetes !! */
					   pAbbSibling = pAb->AbNext;
					   while (pAbbSibling != NULL)
					     {
						TuePave (pAbbSibling);
						/* change les regles des autres paves qui
						   se referent au pave detruit */
						SuppRfPave (pAbbSibling, &PavR, pDoc);
						pAbbSibling = pAbbSibling->AbNext;
					     }
					   pAb = pAb->AbEnclosing;
					   pAb->AbTruncatedTail = TRUE;
					}
				      /* si la vue n'est pas paginee, on marque la racine */
				      /* coupee en queue */
				      if (pAb->AbEnclosing == NULL)
					 pAb->AbTruncatedTail = TRUE;
				      /* si la vue est paginee, on detruit les pages */
				      /* suivantes */
				      if (pAb->AbElement->ElTypeNumber == PageBreak + 1)
					{
					   /* on saute les paves de */
					   /* presentation de la page courante: bas et filet */
					   pAbbSibling = pAb;
					   while (pAbbSibling->AbNext != NULL
						  && pAbbSibling->AbNext->AbElement == pAb->AbElement)
					      pAbbSibling = pAbbSibling->AbNext;
					   pAbbSibling = pAbbSibling->AbNext;		/* premier pave de la page suivante */
					   /* si il existe une page suivante */
					   /* on la detruit (et les suivantes) */
					   while (pAbbSibling != NULL)
					     {
						TuePave (pAbbSibling);
						SuppRfPave (pAbbSibling, &PavR, pDoc);
						pAbbSibling = pAbbSibling->AbNext;
					     }
					   /* on marque la racine coupee en queue */
					   pAb->AbEnclosing->AbTruncatedTail = TRUE;
					}
				   }	/* fin de la destruction des paves suivants */
				 if (pAbbox1 != NULL && pAbbox1->AbLeafType == LtCompound
				     && pAbbox1->AbTruncatedHead
				     && !(assoc || pDoc->DocView[vue - 1].DvSync))
				    /* ce n'est pas une vue synchronisee, on detruit les */
				    /* paves que l'on vient de creer */
				    TueNouveaux (&pAbbFirst, &pAbbLast);
			      }	/* fin pAbbLast != NULL */
			 }	/* fin pEl != marquepage */
		       /* modifie les paves qui peuvent se referer aux nouveaux paves */
		       /* fait dans CreePaves car les paves pAbbFirst et pAbbLast ne sont */
		       /* pas toujours freres */
		       /* NouvRfPave(pAbbFirst, pAbbLast, &PavR, pDoc); */
		       /* conserve le pointeur sur le pave a reafficher */
		       /* c'est la racine */
		       if (VueAssoc (pEl))
			  pDoc->DocAssocModifiedAb[pEl->ElAssocNum - 1] = pAbbRoot;
		       else
			  pDoc->DocViewModifiedAb[vue - 1] = pAbbRoot;
		       ApplReglesRetard (pEl, pDoc);
		    }		/* fin pEl != assoc en haut ou bas de page */
	       }		/* fin vueexiste */
	  }			/* boucle for sur les vues */
     }				/* fin pEl != NULL */
}

#else  /* __COLPAGE__ */

/* ---------------------------------------------------------------------- */
/* |    CrPaveNouv   cree les paves du sous-arbre dont la racine est    | */
/* |            pointe par pEl, dans le document dont le contexte du    | */
/* |            document pDoc.                                          | */
/* |            Les paves sont crees dans toutes les vues si VueNb est  | */
/* |            nul, ou seulement dans la vue de numero VueNb           | */
/* |            si VueNb>0.                                             | */
/* |            Les paves existants affectes par la creation des        | */
/* |            nouveaux paves sont modifies. Les pointeurs sur les     | */
/* |            paves a reafficher du document sont mis a jour.         | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                CrPaveNouv (PtrElement pEl, PtrDocument pDoc, int VueNb)

#else  /* __STDC__ */
void                CrPaveNouv (pEl, pDoc, VueNb)
PtrElement          pEl;
PtrDocument         pDoc;
int                 VueNb;

#endif /* __STDC__ */

{
   int                 vue, premvue, dervue;
   PtrAbstractBox             pAb, PavReaff, PavR, pAbbFirst, pAbbLast, pAbbSibling;
   boolean             vueexiste, stop, assoc;
   boolean             complet;
   PtrAbstractBox             pAbbox1;

   if (pEl != NULL)
     {
	if (VueNb == 0)
	  {
	     premvue = 1;
	     /* nombre de vues definies dans le schema de pres. du document */
	     dervue = MAX_VIEW_DOC;
	  }
	else
	  {
	     premvue = VueNb;
	     dervue = VueNb;
	  }
	/* pour toutes les vues demandees, cree les paves du sous-arbre de */
	/* l'element et reapplique les regles affectees par la creation des */
	/* nouveaux paves */
	for (vue = premvue; vue <= dervue; vue++)
	  {
	     assoc = VueAssoc (pEl);
	     if (!assoc)
		/* l'element ne s'affiche pas dans une vue */
		/* d'elements associes */
		vueexiste = pDoc->DocView[vue - 1].DvPSchemaView > 0;
	     else
		/* c'est une vue d'elements associes */
		vueexiste = pDoc->DocAssocFrame[pEl->ElAssocNum - 1] != 0 && vue == 1;
	     if (vueexiste)
	       {
		  PavReaff = NULL;
		  /* il n'y a encore rien a reafficher */
		  pAbbFirst = NULL;
		  pAbbLast = NULL;
		  if (!assoc)
		     /* traitement particulier des elements associes */
		    {
		       BoiteElAssoc (pEl, vue, pDoc, &PavReaff);
		       pAbbFirst = PavReaff;
		       pAbbLast = PavReaff;
		    }
		  /* cree et chaine les paves correspondant a l'element, si ca n'a */
		  /* pas deja ete fait par BoiteElAssoc */
		  if (PavReaff == NULL)
		     pAb = CreePaves (pEl, pDoc, vue, TRUE, TRUE, &complet);
		  else
		     pAb = NULL;

		  if (pAb != NULL || pAbbFirst != NULL)
		     /* l'element a au moins un pave dans la vue */
		    {
		       if (pAbbFirst == NULL)
			  /* cherche le premier pave cree pour le nouvel element */
			 {
			    pAbbFirst = pAb;
			    stop = FALSE;
			    do
			       if (pAbbFirst->AbPrevious == NULL)
				  stop = TRUE;
			       else if (pAbbFirst->AbPrevious->AbElement == pEl)
				  /* ce pave appartient a l'element */
				  pAbbFirst = pAbbFirst->AbPrevious;
			       else if (ElemIsAnAncestor (pEl, pAbbFirst->AbPrevious->AbElement))
				  /* ce pave appartient a un descendant de */
				  /* l'element (l'element lui-meme a une */
				  /* visibilite' nulle dans la vue et n'a donc */
				  /* pas de pave dans cette vue) */
				  pAbbFirst = pAbbFirst->AbPrevious;
			       else
				  /* ce pave n'appartient pas a l'element */
				  stop = TRUE;
			    while (!(stop));
			    /* cherche le dernier pave cree pour le nouvel element */
			    pAbbLast = pAb;
			    stop = FALSE;
			    do
			       if (pAbbLast->AbNext == NULL)
				  stop = TRUE;
			       else if (pAbbLast->AbNext->AbElement == pEl)
				  /* ce pave appartient a l'element */
				  pAbbLast = pAbbLast->AbNext;
			       else if (ElemIsAnAncestor (pEl, pAbbLast->AbNext->AbElement))
				  /* ce pave appartient a un descendant de */
				  /* l'element (l'element lui-meme a une */
				  /* visibilite' nulle dans la vue et n'a donc */
				  /* pas de pave dans cette vue) */
				  pAbbLast = pAbbLast->AbNext;
			       else
				  /* ce pave n'appartient pas a l'element */
				  stop = TRUE;
			    while (!(stop));
			 }
		       PavReaff = Englobant (pAbbFirst, pAbbLast);
		       /* verifie que les paves precedents et suivants sont complets */
		       /* et les detruit s'ils sont incomplets */
		       if (pAbbFirst->AbPrevious != NULL)
			 {
			    pAbbox1 = pAbbFirst->AbPrevious;
			    if (pAbbox1->AbLeafType == LtCompound
				&& !pAbbox1->AbInLine
				&& pAbbox1->AbTruncatedTail)
			       /* le pave precedent est incomplet a la fin */
			       if (assoc || pDoc->DocView[vue - 1].DvSync)
				  /* La vue est synchronisee, on supprime tous les paves */
				  /* precedents */
				 {
				    pAb = pAbbFirst;
				    do
				      {
					 pAbbSibling = pAb->AbPrevious;
					 while (pAbbSibling != NULL)
					   {
					      TuePave (pAbbSibling);
					      SuppRfPave (pAbbSibling, &PavR, pDoc);
					      PavReaff = Englobant (PavR, PavReaff);
					      pAbbSibling = pAbbSibling->AbPrevious;
					   }
					 pAb = pAb->AbEnclosing;
					 if (pAb != NULL)
					    if (pAb->AbTruncatedHead)
					       /* le pave englobant est deja coupe' en tete. */
					       pAb = NULL;
					    else
					       /* le pave englobant est coupe' en tete, il faut */
					       /* encore detruire les paves qui le precedent. */
					       pAb->AbTruncatedHead = TRUE;
				      }
				    while (!(pAb == NULL));
				 }
			       else
				  /* ce n'est pas une vue synchronisee, on detruit les */
				  /* paves que l'on vient de creer */
				  TueNouveaux (&pAbbFirst, &pAbbLast);
			 }
		       if (pAbbLast != NULL)
			  if (pAbbLast->AbNext != NULL)
			    {
			       pAbbox1 = pAbbLast->AbNext;
			       if (pAbbox1->AbLeafType == LtCompound
				   && !pAbbox1->AbInLine
				   && pAbbox1->AbTruncatedHead)
				  /* le pave suivant est incomplet au debut */
				  if (assoc || pDoc->DocView[vue - 1].DvSync)
				     /* La vue est synchronisee, on supprime 
				        tous les paves suivants */
				    {
				       pAb = pAbbLast;
				       do
					 {
					    pAbbSibling = pAb->AbNext;
					    while (pAbbSibling != NULL)
					      {
						 TuePave (pAbbSibling);
						 SuppRfPave (pAbbSibling, &PavR, pDoc);
						 PavReaff = Englobant (PavR, PavReaff);
						 pAbbSibling = pAbbSibling->AbNext;
					      }
					    pAb = pAb->AbEnclosing;
					    if (pAb != NULL)
					       if (pAb->AbTruncatedTail)
						  pAb = NULL;
					       else
						  /* le pave englobant est coupe' en queue */
						  pAb->AbTruncatedTail =
						     TRUE;
					 }
				       while (!(pAb == NULL));
				    }
				  else
				     /* ce n'est pas une vue synchronisee, on detruit les */
				     /* paves que l'on vient de creer */
				     TueNouveaux (&pAbbFirst, &pAbbLast);
			    }
		       /* si on n'a pas pu creer tous les paves du contenu de */
		       /* l'element (ils depasseraient la capacite de la fenetre), */
		       /* il faut supprimer tous les paves suivant l'element */
		       if (pAbbLast != NULL)
			  if (pAbbLast->AbLeafType == LtCompound)
			     if (!pAbbLast->AbInLine)
				if (pAbbLast->AbTruncatedTail)
				   /* on n'a pas pu creer tous les paves */
				  {
				     pAb = pAbbLast;
				     /* traite le pave de l'element et les paves englobants */
				     do		/* marque le pave coupe' */
				       {
					  pAb->AbTruncatedTail = TRUE;
					  /* supprime tous ses freres suivants */
					  pAbbSibling = pAb->AbNext;
					  while (pAbbSibling != NULL)
					    {
					       if (!pAbbSibling->AbDead)
						  /* detruit le pave' */
						 {
						    TuePave (pAbbSibling);
						    /* change les regles des autres paves qui 
						       se referent au pave detruit */
						    SuppRfPave (pAbbSibling, &PavR, pDoc);
						    PavReaff = Englobant (PavR, PavReaff);
						 }
					       pAbbSibling = pAbbSibling->AbNext;
					    }
					  pAb = pAb->AbEnclosing;
					  if (pAb != NULL)
					     if (pAb->AbTruncatedTail)
						pAb = NULL;
					  /* pave deja coupe', on s'arrete */
				       }
				     while (!(pAb == NULL));
				  }
		    }
		  if (pAbbFirst != NULL)
		     /* modifie les paves qui peuvent se referer aux nouveaux paves */
		    {
		       NouvRfPave (pAbbFirst, pAbbLast, &PavR, pDoc);
		       ApplReglesRetard (pEl, pDoc);
		       PavReaff = Englobant (PavR, PavReaff);
		       /* conserve le pointeur sur le pave a reafficher */
		       if (VueAssoc (pEl))
			  pDoc->DocAssocModifiedAb[pEl->ElAssocNum - 1] =
			     Englobant (PavReaff,
				 pDoc->DocAssocModifiedAb[pEl->ElAssocNum - 1]);
		       else
			  pDoc->DocViewModifiedAb[vue - 1] =
			     Englobant (PavReaff, pDoc->DocViewModifiedAb[vue - 1]);
		    }
	       }
	  }
     }
}
#endif /* __COLPAGE__ */


#ifdef __COLPAGE__
/* ---------------------------------------------------------------------- */
/* |    PosBoolPavCh met a vrai les booleens de position verticale      | */
/* |            et horizontale                                          | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         PosBoolPavCh (PtrAbstractBox pAb)

#else  /* __STDC__ */
static void         PosBoolPavCh (pAb)
PtrAbstractBox             pAb;

#endif /* __STDC__ */

{
   if (pAb != NULL)
     {
	pAb->AbVertPosChange = TRUE;
	pAb->AbVertRefChange = TRUE;
	pAb->AbHorizPosChange = TRUE;
	pAb->AbHorizRefChange = TRUE;
	pAb = pAb->AbFirstEnclosed;
	while (pAb != NULL)
	  {
	     PosBoolPavCh (pAb);
	     pAb = pAb->AbNext;
	  }
     }
}

#endif /* __COLPAGE__ */

#ifdef __COLPAGE__
/* ---------------------------------------------------------------------- */
/*      procedure completement remaniee dans la V4                        */
/* ---------------------------------------------------------------------- */
/* |    DetPavVue detruit pour la vue de numero vue les paves du        | */
/* |            sous-arbre dont la racine est pointee par pEl, dans le  | */
/* |            document pDoc.                                          | */
/* |            Destruction des paves suite a la destruction de pEl     | */
/* |            Les paves existants affectes par la destruction sont    | */
/* |            modifies. Les pointeurs de paves modifies du document   | */
/* |            sont mis a jour                                         | */
/* |            Si Verif est vrai, on verifie si les paves englobants   | */
/* |            deviennent complets. En effet la suppression d'un pave  | */
/* |            incomplet a une extremite' peut rendre le pave englobant| */
/* |            complet a cette extremite.                              | */
/* |            Attention : on peut detruire des paves de plusieurs     | */
/* |            sous-arbres car pEl peut avoir des paves dupliques      | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                DetPavVue (PtrElement pEl, PtrDocument pDoc, boolean Verif, int vue)

#else  /* __STDC__ */
void                DetPavVue (pEl, pDoc, Verif, vue)
PtrElement          pEl;
PtrDocument         pDoc;
boolean             Verif;
int                 vue;

#endif /* __STDC__ */

{
   PtrAbstractBox             pAb, PavReaff, PavR, pAbb, pPere, PcFirst, PcLast;
   PtrElement          pElFils;
   boolean             stop;
   PtrElement          pEl1;
   PtrAbstractBox             pPavSuiv;
   FILE               *list;

   PavReaff = NULL;
   pAb = pEl->ElAbstractBox[vue - 1];
   if (pAb == NULL)
      /* pas de pave pour cet element dans cette vue, alors que */
      /* la vue existe */
     {
	if (!pEl->ElTerminal)
	   /* cherche les paves des descendants de l'element */
	  {
	     pElFils = pEl->ElFirstChild;
	     while (pElFils != NULL)
	       {
		  DetPavVue (pElFils, pDoc, Verif, vue);
		  pElFils = pElFils->ElNext;
	       }
	  }
     }
   else
      /* l'element a au moins un pave dans la vue */
     {
	pEl1 = pEl;
	PcFirst = NULL;
	PcLast = NULL;
	pPavSuiv = NULL;
	PavReaff = pAb;	/* on reaffichera ce pave */
	/* traite tous les paves correspondant a cet element */
	/* les marque d'abord tous 'morts' en verifiant si leur */
	/* suppression complete le pave englobant */
	do
	  {
	     if (Verif)
		if (pAb->AbLeafType == LtCompound && !pAb->AbPresentationBox)
		   /* les booleens AbTruncatedHead et AbTruncatedTail ne sont pas */
		   /* significatifs pour les paves de presentation */
		   /* && !pAb->AbInLine) */
		  {
		     if (pAb->AbTruncatedHead && pAb->AbPreviousRepeated == NULL)
			/* si le pave a un dup precedent, il ne faut pas rendre les */
			/* peres complets car ils sont eux aussi dupliques */
			/* remarque: si un pave est coupe et non duplique, */
			/* alors ses peres ne peuvent pas etre dupliques */
			if (pEl->ElPrevious == NULL)
			   /* c'est le pave du premier element */
			  {
			     pPere = pAb->AbEnclosing;
			     while (pPere != NULL)
			       {
				  pAbb = PaveCoupe (pPere, FALSE, TRUE, pDoc);
				  if (pAbb != NULL)
				     PavReaff = Englobant (PavReaff, pAbb);
				  if (pPere->AbElement->ElPrevious != NULL)
				     pPere = NULL;
				  else
				     pPere = pPere->AbEnclosing;
			       }
			  }
		     if (pAb->AbTruncatedTail && pAb->AbNextRepeated == NULL)
			/* si le pave a un dup suivant, il ne faut pas rendre les */
			/* peres complets car ils sont eux aussi dupliques */
			if (pEl->ElNext == NULL)
			   /* c'est le pave du dernier element */
			  {
			     pPere = pAb->AbEnclosing;
			     while (pPere != NULL)
			       {
				  pAbb = PaveCoupe (pPere, FALSE, FALSE, pDoc);
				  if (pAbb != NULL)
				     PavReaff = Englobant (PavReaff, pAbb);
				  pPere = pPere->AbEnclosing;
				  if (pPere != NULL)
				     if (pPere->AbElement->ElNext != NULL)
					pPere = NULL;
			       }
			  }
		  }		/* fin cas verif */
	     TuePave (pAb);
	     if (PcFirst == NULL)
		PcFirst = pAb;
	     PcLast = pAb;
	     /* passe au pave suivant du meme element */
	     /* cas particulier de la destruction d'un element marque de page */
	     /* destruction des paves de l'element */
	     /* et du reste de l'image abstraite : pour refaire une */
	     /* duplication correcte */
	     if (pEl->ElTypeNumber == PageBreak + 1)
		pAb = pAb->AbNext;
	     else
	       {
		  /* on memorise les paves qui ne sont pas de presentation */
		  /* pour le passage au pave duplique suivant */
		  if (!pAb->AbPresentationBox)
		     pPavSuiv = pAb;
		  if (pAb->AbNext != NULL)
		     if (pAb->AbNext->AbElement == pEl)
			/* il s'agit d'un autre pave de l'element */
			pAb = pAb->AbNext;
		     else
			pAb = NULL;	/* pave de l'element suivant */
		  else if (pPavSuiv != NULL)
		     pAb = pPavSuiv->AbNextRepeated;
		  /* on prend le pave duplique suivant */
		  else
		     pAb = NULL;	/* cas jamais possible ?? */
	       }
	  }
	while (!(pAb == NULL));
	/* si on detruit une marque de page, on a detruit toutes les pages */
	/* suivantes : il faut mettre la racine coupee en queue */
	if (pEl->ElTypeNumber == PageBreak + 1)
	  {
	     pAbb = pEl->ElAbstractBox[vue - 1]->AbEnclosing;
	     while (pAbb != NULL)
	       {
		  pAbb->AbTruncatedTail = TRUE;
		  pAbb = pAbb->AbEnclosing;	/* en prevision des colonnes */
	       }
	  }
	if (PcFirst != PcLast)
	   /* il y a plusieurs paves pour cet element, on reaffichera */
	   /* le pave englobant */
	   PavReaff = Englobant (PavReaff, PcFirst->AbEnclosing);
	if (pEl->ElSructSchema->SsRule[pEl->ElTypeNumber - 1].SrAssocElem)
	   if (PcFirst->AbPSchema->PsInPageHeaderOrFooter[pEl->ElTypeNumber - 1])
	      /* c'est un element qui s'affiche en haut ou bas de page */
	      /* cherche si le pave englobant est vide (ne contient */
	      /* que des paves morts ou de presentation) */
	     {
		pAbb = PcFirst->AbEnclosing->AbFirstEnclosed;
		stop = FALSE;
		do
		   if (pAbb == NULL)
		      stop = TRUE;
		   else if (!pAbb->AbDead && !pAbb->AbPresentationBox)
		      stop = TRUE;
		   else
		      pAbb = pAbb->AbNext;
		while (!(stop));
		if (pAbb == NULL)
		   /* il n'y a que des paves morts, on tue le pave */
		   /* englobant */
		  {
		     PcFirst = PcFirst->AbEnclosing;
		     PcLast = PcFirst;
		     if (pEl->ElParent->ElAbstractBox[vue - 1] == PcFirst)
			pEl->ElParent->ElAbstractBox[vue - 1] = NULL;
		     TuePave (PcFirst);
		     PavReaff = Englobant (PavReaff, PcFirst);
		  }
	     }
	pAb = PcFirst;
	do
	  {
	     /* cherche tous les paves qui font reference au pave a */
	     /* detruire et pour ces paves, reapplique les regles qui font */
	     /* reference au pave a detruire */
	     SuppRfPave (pAb, &PavR, pDoc);
	     PavReaff = Englobant (PavReaff, PavR);
	     /* passe au pave mort suivant */
	     if (pAb == PcLast)
		pAb = NULL;
	     else if (pEl->ElTypeNumber == PageBreak + 1)
		pAb = pAb->AbNext;
	     else
	       {
		  /* on memorise les paves qui ne sont pas de presentation */
		  /* pour le passage au pave duplique suivant */
		  if (!pAb->AbPresentationBox)
		     pPavSuiv = pAb;
		  if (pAb->AbNext != NULL)
		     if (pAb->AbNext->AbElement == pEl)
			/* il s'agit d'un autre pave de l'element */
			pAb = pAb->AbNext;
		     else
			pAb = NULL;	/* pave de l'element suivant */
		  else if (pPavSuiv != NULL)
		     pAb = pPavSuiv->AbNextRepeated;
		  /* on prend le pave duplique suivant */
		  else
		     pAb = NULL;	/* cas jamais possible ?? */
	       }
	  }
	while (!(pAb == NULL));
	/* conserve le pointeur sur le pave a reafficher */
	if (VueAssoc (pEl))
	   pDoc->DocAssocModifiedAb[pEl->ElAssocNum - 1] =
	      Englobant (PavReaff, pDoc->DocAssocModifiedAb[pEl->ElAssocNum - 1]);
	else
	   pDoc->DocViewModifiedAb[vue - 1] = Englobant (PavReaff, pDoc->DocViewModifiedAb[vue - 1]);
	/* sauvegarde de l'image abstraite pour tests */
	list = fopen ("/perles/roisin/debug/detpav", "w");
	if (list != NULL)
	  {
	     NumPav (pDoc->DocViewRootAb[0]);
	     AffPaves (pDoc->DocViewRootAb[0], 2, list);
	     fclose (list);
	  }
     }				/* fin du else : l'element a un pave dans la vue */
}


#else  /* __COLPAGE__ */
/* ---------------------------------------------------------------------- */
/* |    DetPavVue detruit pour la vue de numero vue les paves du        | */
/* |            sous-arbre dont la racine est pointee par pEl, dans le  | */
/* |            document pDoc.                                          | */
/* |            Les paves existants affectes par la destruction sont    | */
/* |            modifies. Les pointeurs de paves modifies du document   | */
/* |            sont mis a jour                                         | */
/* |            Si Verif est vrai, on verifie si les paves englobants   | */
/* |            deviennent complets. En effet la suppression d'un pave  | */
/* |            incomplet a une extremite' peut rendre le pave englobant| */
/* |            complet a cette extremite.                              | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                DetPavVue (PtrElement pEl, PtrDocument pDoc, boolean Verif, int vue)

#else  /* __STDC__ */
void                DetPavVue (pEl, pDoc, Verif, vue)
PtrElement          pEl;
PtrDocument         pDoc;
boolean             Verif;
int                 vue;

#endif /* __STDC__ */

{
   PtrAbstractBox             pAb, PavReaff, PavR, pAbb, pPere, PcFirst, PcLast;
   PtrElement          pElFils;
   boolean             stop;
   PtrElement          pEl1;
   PtrAbstractBox             pAbbox1;

   pAb = pEl->ElAbstractBox[vue - 1];
   if (pAb == NULL)
     {
	/* pas de pave pour cet element dans cette vue, alors que */
	/* la vue existe */
	if (!pEl->ElTerminal)
	   /* cherche les paves des descendants de l'element */
	  {
	     pElFils = pEl->ElFirstChild;
	     while (pElFils != NULL)
	       {
		  DetPavVue (pElFils, pDoc, Verif, vue);
		  pElFils = pElFils->ElNext;
	       }
	  }
     }
   else
      /* l'element a au moins un pave dans la vue */
     {
	/* si l'element cree un pave englobant par la regle FnCreateEnclosing, */
	/* c'est ce pave englobant qu'il faut detruire */
	if (pAb->AbEnclosing != NULL)
	   if (pAb->AbEnclosing->AbPresentationBox)
	      if (pAb->AbEnclosing->AbElement == pAb->AbElement)
		 pAb = pAb->AbEnclosing;
	pEl1 = pEl;
	if (pEl1->ElSructSchema->SsRule[pEl1->ElTypeNumber - 1].SrAssocElem)
	   /* traitement particulier des elements associes : le pere */
	   /* de l'element a pour pave le pave englobant de l'element. */
	   /* Dans le cas ou ces elements associes sont affiches dans */
	   /* une boite de haut ou de bas de page, le pave de l'element */
	   /* englobant pourrait etre dans une autre page... */
	   pEl->ElParent->ElAbstractBox[vue - 1] = pAb->AbEnclosing;
	PcFirst = NULL;
	PcLast = NULL;
	PavReaff = pAb;	/* on reaffichera ce pave */
	/* traite tous les paves correspondant a cet element */
	/* les marque d'abord tous 'morts' en verifiant si leur */
	/* suppression complete le pave englobant */
	do
	  {
	     if (Verif)
		if (pAb->AbLeafType == LtCompound && !pAb->AbInLine)
		  {
		     if (pAb->AbTruncatedHead)
			if (pEl->ElPrevious == NULL)
			   /* c'est le pave du premier element */
			  {
			     pPere = pAb->AbEnclosing;
			     while (pPere != NULL)
			       {
				  pAbb = PaveCoupe (pPere, FALSE, TRUE, pDoc);
				  if (pAbb != NULL)
				     PavReaff = Englobant (PavReaff, pAbb);
				  if (pPere->AbElement->ElPrevious != NULL)
				     pPere = NULL;
				  else
				     pPere = pPere->AbEnclosing;
			       }
			  }
		     if (pAb->AbTruncatedTail)
			if (pEl->ElNext == NULL)
			   /* c'est le pave du dernier element */
			  {
			     pPere = pAb->AbEnclosing;
			     while (pPere != NULL)
			       {
				  pAbb = PaveCoupe (pPere, FALSE, FALSE, pDoc);
				  if (pAbb != NULL)
				     PavReaff = Englobant (PavReaff, pAbb);
				  pPere = pPere->AbEnclosing;
				  if (pPere != NULL)
				     if (pPere->AbElement->ElNext != NULL)
					pPere = NULL;
			       }
			  }
		  }
	     TuePave (pAb);
	     if (PcFirst == NULL)
		PcFirst = pAb;
	     PcLast = pAb;
	     /* passe au pave suivant du meme element */
	     pAb = pAb->AbNext;
	     if (pAb != NULL)
	       {
		  pAbbox1 = pAb;
		  if (pAbbox1->AbElement != pEl)
		     /* il s'agit d'un autre element, on arrete */
		     pAb = NULL;
		  else if (pAbbox1->AbElement->ElTerminal
			   && pAbbox1->AbElement->ElLeafType == LtPageColBreak)
		     /* c'est un pave de haut ou bas de page, on n'en */
		     /* traite qu'un */
		     pAb = NULL;
	       }
	  }
	while (!(pAb == NULL));
	if (PcFirst != PcLast)
	   /* il y a plusieurs paves pour cet element, on reaffichera */
	   /* le pave englobant */
	   PavReaff = Englobant (PavReaff, PcFirst->AbEnclosing);
	/* Est-ce un pave d'un element associe qui s'affiche en haut */
	/* ou en bas de page ? */
	if (pEl->ElSructSchema->SsRule[pEl->ElTypeNumber - 1].SrAssocElem)
	   if (PcFirst->AbEnclosing->AbElement->ElTypeNumber == PageBreak + 1)
	      /* le pave englobant est une boite de page */
	      /* cherche si le pave englobant est vide (ne contient */
	      /* que des paves morts ou de presentation) */
	     {
		pAbb = PcFirst->AbEnclosing->AbFirstEnclosed;
		stop = FALSE;
		do
		   if (pAbb == NULL)
		      stop = TRUE;
		   else if (!pAbb->AbDead && !pAbb->AbPresentationBox)
		      stop = TRUE;
		   else
		      pAbb = pAbb->AbNext;
		while (!(stop));
		if (pAbb == NULL)
		   /* il n'y a que des paves morts, on tue le pave */
		   /* englobant */
		  {
		     PcFirst = PcFirst->AbEnclosing;
		     PcLast = PcFirst;
		     if (pEl->ElParent->ElAbstractBox[vue - 1] == PcFirst)
			pEl->ElParent->ElAbstractBox[vue - 1] = NULL;
		     TuePave (PcFirst);
		     PavReaff = Englobant (PavReaff, PcFirst);
		     /* traite ensuite les paves qui se referent aux paves morts */
		  }
	     }
	pAb = PcFirst;
	do
	  {
	     /* cherche tous les paves qui font reference au pave a */
	     /* detruire et pour ces paves, reapplique les regles qui font */
	     /* reference au pave a detruire */
	     SuppRfPave (pAb, &PavR, pDoc);
	     PavReaff = Englobant (PavReaff, PavR);
	     /* passe au pave mort suivant */
	     if (pAb == PcLast)
		pAb = NULL;
	     else
		pAb = pAb->AbNext;
	  }
	while (!(pAb == NULL));
	/* conserve le pointeur sur le pave a reafficher */
	if (VueAssoc (pEl))
	   pDoc->DocAssocModifiedAb[pEl->ElAssocNum - 1] =
	      Englobant (PavReaff, pDoc->DocAssocModifiedAb[pEl->ElAssocNum - 1]);
	else
	   pDoc->DocViewModifiedAb[vue - 1] = Englobant (PavReaff, pDoc->DocViewModifiedAb[vue - 1]);
     }
}
#endif /* __COLPAGE__ */


/* ---------------------------------------------------------------------- */
/* |    DetrPaves detruit les paves du sous-arbre dont la racine est    | */
/* |            pointee par pEl, dans le document pDoc.                 | */
/* |            Les paves existants affectes par la destruction sont    | */
/* |            modifies. Les pointeurs de paves modifies du document   | */
/* |            sont mis a jour                                         | */
/* |            Si Verif est vrai, on verifie si les paves englobants   | */
/* |            deviennent complets. En effet la suppression d'un pave  | */
/* |            incomplet a une extremite' peut rendre le pave englobant| */
/* |            complet a cette extremite.                              | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                DetrPaves (PtrElement pEl, PtrDocument pDoc, boolean Verif)

#else  /* __STDC__ */
void                DetrPaves (pEl, pDoc, Verif)
PtrElement          pEl;
PtrDocument         pDoc;
boolean             Verif;

#endif /* __STDC__ */

{
   int                 vue;
   boolean             vueexiste;

   if (pEl != NULL)
      /* traite les paves de toutes les vues */
      for (vue = 1; vue <= MAX_VIEW_DOC; vue++)
	 /* teste si la vue  existe */
	{
	   if (!VueAssoc (pEl))
	      /* ce n'est pas un element associe */
	      vueexiste = pDoc->DocView[vue - 1].DvPSchemaView > 0;
	   else
	      /* c'est un element associe */
	   if (pDoc->DocAssocFrame[pEl->ElAssocNum - 1] == 0)
	      vueexiste = FALSE;
	   else
	      vueexiste = vue == 1;
	   if (vueexiste)
	      /* detruit les paves de cette vue */
	      DetPavVue (pEl, pDoc, Verif, vue);
	}
}



/* ---------------------------------------------------------------------- */
/* |    ReafReference Reaffiche les paves de la reference pointee par   | */
/* |            pRef appartenant au document pointe' par pDocRef.       | */
/* |            Si pAb est NULL, tous les paves (sauf les paves de     | */
/* |            presentation) de la reference, dans toutes les vues,    | */
/* |            sont recalcules et reaffiches ; sinon, seuls les paves  | */
/* |            qui copient le pave pointe' par pAb sont recalcules et | */
/* |            reaffiches s'ils changent.                              | */
/* |            Note: cette nouvelle procedure est extraite du code de  | */
/* |            l'ancienne procedure ChngRef.                           | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                ReafReference (PtrReference pRef, PtrAbstractBox pAb, PtrDocument pDocRef)

#else  /* __STDC__ */
void                ReafReference (pRef, pAb, pDocRef)
PtrReference        pRef;
PtrAbstractBox             pAb;
PtrDocument         pDocRef;

#endif /* __STDC__ */

{
   PtrElement          pElRef;
   PtrAbstractBox             pAbb, pPavRef;
   int                 v, frame, j, h;
   PtrPRule        pRegle;
   boolean             trouve, reaff, stop, identique;
   PtrTextBuffer      BufCopie, BufOriginal;
   PtrPSchema          pSPR;
   PtrAttribute         pAttr;
   PtrDocument         pDo1;
   PtrElement          pEl1;
   PtrPRule        pRe1;
   PtrAbstractBox             pAbbox1;
   boolean             complet;

#ifdef __COLPAGE__
   PtrElement          pE;
   int                 nb;

#endif /* __COLPAGE__ */

   pElRef = pRef->RdElement;
   /* element qui reference */
   if (pRef->RdAttribute == NULL)
      /* on ne traite pas les */
      /* references par attribut */
      for (v = 1; v <= MAX_VIEW_DOC; v++)
	 if (pElRef->ElAbstractBox[v - 1] != NULL)
	    /* la reference a au moins un pave dans la vue */
	    /* saute les paves de presentation */
	   {
	      pPavRef = pElRef->ElAbstractBox[v - 1];
	      stop = FALSE;
	      do
		 if (pPavRef == NULL)
		    stop = TRUE;
		 else if (!pPavRef->AbPresentationBox)
		    stop = TRUE;
		 else
		    pPavRef = pPavRef->AbNext;
	      while (!(stop));
	      reaff = FALSE;
	      if (pRef->RdTypeRef == RefInclusion)
		 /* c'est une inclusion, on cree ses paves si ce n'est pas */
		 /* deja fait */
		 /* indique le volume que pourront prendre les paves cree's */
		{
		   pDo1 = pDocRef;
		   if (!VueAssoc (pElRef))
		     {
			if (pDo1->DocView[v - 1].DvPSchemaView > 0)
#ifdef __COLPAGE__
			  {
			     /* le document est-il pagine dans cette vue ? */
			     /* si oui, on compte le nombre de pages actuel */
			     pAbb = pDo1->DocViewRootAb[v - 1];
			     if (pAbb->AbFirstEnclosed != NULL
				 && pAbb->AbFirstEnclosed->AbElement->ElTypeNumber ==
				 PageBreak + 1)
			       {
				  nb = NbPages (pAbb);
				  pDo1->DocViewNPages[v - 1] = nb;
				  pDo1->DocViewFreeVolume[v - 1] = THOT_MAXINT;
			       }
			     else
				pDo1->DocViewFreeVolume[v - 1] = pDo1->DocViewVolume[v - 1];
			  }
#else  /* __COLPAGE__ */
			   pDo1->DocViewFreeVolume[v - 1] = pDo1->DocViewVolume[v - 1];
#endif /* __COLPAGE__ */
		     }
		   else
		      /* element associe */
		     {
			pEl1 = pElRef;
			if (pDo1->DocAssocFrame[pEl1->ElAssocNum - 1] > 0)
#ifdef __COLPAGE__
			  {
			     pE = pDo1->DocAssocRoot[pEl1->ElAssocNum - 1];
			     pAbb = pE->ElAbstractBox[v - 1];
			     if (pAbb != NULL && pAbb->AbFirstEnclosed != NULL
				 && pAbb->AbFirstEnclosed->AbElement->ElTypeNumber ==
				 PageBreak + 1)
			       {
				  nb = NbPages (pAbb);
				  pDo1->DocAssocNPages[pEl1->ElAssocNum - 1] = nb;
				  pDo1->DocAssocFreeVolume[pEl1->ElAssocNum - 1] = THOT_MAXINT;
			       }
			     else
				pDo1->DocAssocFreeVolume[pEl1->ElAssocNum - 1] =
				   pDo1->DocAssocVolume[pEl1->ElAssocNum - 1];
			  }
#else  /* __COLPAGE__ */
			   pDo1->DocAssocFreeVolume[pEl1->ElAssocNum - 1] =
			      pDo1->DocAssocVolume[pEl1->ElAssocNum - 1];
#endif /* __COLPAGE__ */
		     }
		   pAbb = CreePaves (pElRef, pDocRef, v, TRUE, TRUE, &complet);
		   reaff = pAbb != NULL;
		}
	      else
		 /* C'est un element reference. L'element reference copie-t-il */
		 /* ce type de pave ? */
		 /* cherche dans les regles de presentation de la */
		 /* reference une regle FnCopy qui copie ce type de pave */
		{
		   trouve = FALSE;
		   pRegle = LaRegle (pDocRef, pPavRef, &pSPR, PtFunction, TRUE, &pAttr);
		   stop = FALSE;
		   do
		      if (pRegle == NULL)
			 stop = TRUE;
		      else if (pRegle->PrType != PtFunction || trouve)
			 stop = TRUE;
		      else
			{
			   pRe1 = pRegle;
			   if (pRe1->PrViewNum == pDocRef->DocView[v - 1].DvPSchemaView
			       && pRe1->PrPresFunction == FnCopy)
			      if (pAb == NULL)
				 trouve = TRUE;
			      else
				{
				   pAbbox1 = pAb;
				   if (pRe1->PrNPresBoxes == 0)
				      trouve = (strcmp (pRe1->PrPresBoxName,
							pAbbox1->AbPSchema->PsPresentBox[pAbbox1->AbTypeNum - 1].PbName) == 0);
				   else
				      trouve = pRe1->PrPresBox[0] == pAbbox1->AbTypeNum;
				}
			   if (!trouve)
			      pRegle = pRegle->PrNextPRule;
			}
		   while (!(stop));
		   if (trouve)
		      /* oui, la reference copie ce type de pave */
		     {
			if (pAb == NULL)
			   identique = FALSE;
			else
			   /* la copie est-elle deja identique a l'original ? */
			  {
			     BufCopie = pPavRef->AbText;
			     BufOriginal = pAb->AbText;
			     j = 1;
			     while (BufOriginal->BuContent[j - 1] == BufCopie->BuContent[j - 1]
				    && BufOriginal->BuContent[j - 1] != '\0'
				    && BufCopie->BuContent[j - 1] != '\0')
				j++;
			     identique = BufOriginal->BuContent[j - 1] == '\0'
				&& BufCopie->BuContent[j - 1] == '\0';
			  }
			if (!identique)
			   /* contenus differents, applique la regle de copie */
#ifdef __COLPAGE__
			   if (Applique (pRegle, pSPR, pPavRef, pDocRef, pAttr, &bool))
#else  /* __COLPAGE__ */
			   if (Applique (pRegle, pSPR, pPavRef, pDocRef, pAttr))
#endif /* __COLPAGE__ */
			      reaff = TRUE;
		     }
		}
	      if (reaff)
		 /* on ne reaffiche pas les paves qui n'ont pas encore */
		 /* ete affiches */
		 if (!pPavRef->AbNew)
		   {
		      pPavRef->AbChange = TRUE;
		      if (VueAssoc (pElRef))
			 frame = pDocRef->DocAssocFrame[pElRef->ElAssocNum - 1];
		      else
			 frame = pDocRef->DocViewFrame[v - 1];
#ifdef __COLPAGE__
		      h = HauteurCoupPage;
#else  /* __COLPAGE__ */
		      h = HauteurPage;
#endif /* __COLPAGE__ */
		      (void) ModifVue (frame, &h, pPavRef);
		      /* on ne reaffiche pas si on est en train de calculer */
		      /* les pages */
#ifdef __COLPAGE__
		      if (HauteurCoupPage == 0)
#else  /* __COLPAGE__ */
		      if (HauteurPage == 0)
#endif /* __COLPAGE__ */
			 DisplayFrame (frame);
		   }
	   }
}



/* ---------------------------------------------------------------------- */
/* |    ChngRef Le pave de presentation pointe' par pAb a change'      | */
/* |            de contenu. Cherche toutes les references a un element  | */
/* |            englobant de l'element creant ce pave et qui copient    | */
/* |            ce pave. Demande leur reaffichage                       | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                ChngRef (PtrAbstractBox pAb, PtrDocument pDoc)

#else  /* __STDC__ */
void                ChngRef (pAb, pDoc)
PtrAbstractBox             pAb;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrElement          pEl;
   PtrReference        pRef;
   int                 niv;
   PtrExternalDoc       pDocExt;
   PtrDocument         pDocRef;

   pEl = pAb->AbElement;
   /* traite l'element qui a cree' ce pave et les elements englobants */
   /* sur 3 niveaux */
   niv = 0;
   pDocExt = NULL;
   pRef = NULL;
   while (pEl != NULL && niv < 3)
     {
	pRef = NextReferenceToEl (pEl, pDoc, FALSE, pRef, &pDocRef, &pDocExt, TRUE);
	if (pRef != NULL)
	   /* cet element est reference' par au moins un autre element */
	   /* parcourt la chaine des elements qui le referencent */
	   while (pRef != NULL)
	      /* reaffiche les paves de la reference qui copient le pave */
	     {
		ReafReference (pRef, pAb, pDocRef);
		/* passe a la reference suivante */
		pRef = NextReferenceToEl (pEl, pDoc, FALSE, pRef, &pDocRef, &pDocExt, TRUE);
		/* passe au niveau superieur */
	     }
	pEl = pEl->ElParent;
	niv++;
     }
}

/* ---------------------------------------------------------------------- */
/* |    Av1Ch                                                           | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static PtrAbstractBox      Av1Ch (PtrAbstractBox pPav1, boolean Test, PtrSSchema pSchStr, PtrPSchema pSchP, int Typ, boolean Pres)

#else  /* __STDC__ */
static PtrAbstractBox      Av1Ch (pPav1, Test, pSchStr, pSchP, Typ, Pres)
PtrAbstractBox             pPav1;
boolean             Test;
PtrSSchema        pSchStr;
PtrPSchema          pSchP;
int        Typ;
boolean             Pres;

#endif /* __STDC__ */

{
   PtrAbstractBox             p, s;
   PtrAbstractBox             pAbbox1;

   p = NULL;
   if (Test)
     {
	pAbbox1 = pPav1;
	if (Pres)
	  {
	     if (pAbbox1->AbPresentationBox && pAbbox1->AbTypeNum == Typ)
	       {
		  if (pAbbox1->AbPSchema == pSchP)
		     p = pPav1;
		  /* trouve ! C'est le pave lui-meme */
	       }
	  }
	else if (!pAbbox1->AbPresentationBox
		 && pAbbox1->AbElement->ElTypeNumber == Typ
		 && (pSchStr == NULL
		     || pAbbox1->AbElement->ElSructSchema == pSchStr))
	   p = pPav1;
     }
   if (p == NULL)
      /* on cherche parmi les fils du pave */
     {
	s = pPav1->AbFirstEnclosed;
	while (s != NULL && p == NULL)
	  {
	     p = Av1Ch (s, TRUE, pSchStr, pSchP, Typ, Pres);
	     s = s->AbNext;
	  }
     }
   return p;
}



/* ---------------------------------------------------------------------- */
/* |    PavCherche cherche un pave en avant dans un arbre de paves, a   | */
/* |            partir du pave pointe' par pAb. Si Pres est vrai, on   | */
/* |            cherche un pave de presentation du type Typ defini dans | */
/* |            le schema de presentation pSchP. Si Pres est faux, on   | */
/* |            cherche un pave d'un element structure de type Typ      | */
/* |            defini dans le schema de structure pointe' par pSchStr. | */
/* |            Retourne un pointeur sur le pave trouve' ou             | */
/* |            NULL si echec.                                          | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
PtrAbstractBox             PavCherche (PtrAbstractBox pAb, boolean Pres, int Typ, PtrPSchema pSchP, PtrSSchema pSchStr)
#else  /* __STDC__ */
PtrAbstractBox             PavCherche (pAb, Pres, Typ, pSchP, pSchStr)
PtrAbstractBox             pAb;
boolean             Pres;
int        Typ;
PtrPSchema          pSchP;
PtrSSchema        pSchStr;

#endif /* __STDC__ */
{
   PtrAbstractBox             pt, pf, pp;
   boolean             stop;
   PtrAbstractBox             pAbbox1;

   pt = NULL;
   if (pAb != NULL)
      /* cherche dans le sous-arbre du pave */
     {
	pt = Av1Ch (pAb, FALSE, pSchStr, pSchP, Typ, Pres);
	if (pt == NULL)
	   /* si echec, cherche dans les sous-arbres des freres suivants */
	  {
	     pf = pAb->AbNext;
	     while (pf != NULL && pt == NULL)
	       {
		  pt = Av1Ch (pf, TRUE, pSchStr, pSchP, Typ, Pres);
		  pf = pf->AbNext;
		  /* si echec, cherche le premier ascendant avec un frere suivant */
	       }
	     if (pt == NULL)
	       {
		  stop = FALSE;
		  pp = pAb;
		  do
		    {
		       pp = pp->AbEnclosing;
		       if (pp == NULL)
			  stop = TRUE;
		       else if (pp->AbNext != NULL)
			  stop = TRUE;
		    }
		  while (!(stop));
		  if (pp != NULL)
		     /* cherche si ce pave est celui cherche */
		    {
		       pp = pp->AbNext;
		       if (pp != NULL)
			 {
			    pAbbox1 = pp;
			    if (Pres)
			      {
				 if (pAbbox1->AbPresentationBox && pAbbox1->AbTypeNum == Typ)
				   {
				      if (pAbbox1->AbPSchema == pSchP)
					 pt = pp;
				      /* trouve */
				   }
			      }
			    else if (!pAbbox1->AbPresentationBox
				     && pAbbox1->AbElement->ElTypeNumber == Typ
				     && (pSchStr == NULL
			       || pAbbox1->AbElement->ElSructSchema == pSchStr))
			       pt = pp;
			    /* trouve */
			    if (pt == NULL)
			       pt = PavCherche (pp, Pres, Typ, pSchP, pSchStr);
			 }
		    }
	       }
	  }
     }
   return pt;
}



/* ---------------------------------------------------------------------- */
/* |    PavDeb  Si PavDebut n'a pas de valeur pour la vue nv, met dans  | */
/* |            PavDebut[nv] un pointeur sur le premier pave            | */
/* |            correspondant a l'element pElDebut dans la vue nv.      | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         PavDeb (PtrElement pElDebut, int nv)
#else  /* __STDC__ */
static void         PavDeb (pElDebut, nv)
PtrElement          pElDebut;
int                 nv;

#endif /* __STDC__ */
{
   PtrElement          pEl;
   boolean             stop;

   if (PavDebut[nv - 1] == NULL)
     {
	pEl = pElDebut;
	stop = FALSE;
	do
	   if (pEl == NULL)
	      stop = TRUE;
	   else if (pEl->ElAbstractBox[nv - 1] != NULL)
	      stop = TRUE;
	   else
	      /* l'element n'a pas de pave dans la vue, */
	      /* ?cherche un element suivant qui ait un pave */
	      pEl = FwdSearchTypedElem (pEl, 0, NULL);
	while (!(stop));
	if (pEl != NULL)
	   PavDebut[nv - 1] = pEl->ElAbstractBox[nv - 1];
     }
}



/* ---------------------------------------------------------------------- */
/* |    ReNumPages      renumerote toutes les pages qui concernent      | */
/* |            la vue vue a partir de l'element pointe' par pEl,       | */
/* |            lui-meme compris.                                       | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                ReNumPages (PtrElement pEl, int vue)
#else  /* __STDC__ */
void                ReNumPages (pEl, vue)
PtrElement          pEl;
int                 vue;

#endif /* __STDC__ */
{
   PtrElement          pPage;
   PtrPSchema          pSchP;
   boolean             stop;
   int                 numpageprec, cpt;
   PtrElement          pEl1;

   stop = FALSE;
   pPage = pEl;
   numpageprec = 0;
   /* renumerote toutes les pages qui suivent pour la meme vue */
   do
     {
	pPage = FwdSearchTypedElem (pPage, PageBreak + 1, NULL);
	if (pPage == NULL)
	   /* c'etait la derniere page, on s'arrete */
	   stop = TRUE;
#ifdef __COLPAGE__
	else if (pPage->ElViewPSchema == vue
		 && (pPage->ElPageType == PgBegin
		     || pPage->ElPageType == PgComputed
		     || pPage->ElPageType == PgUser))
	   /* on ne considere pas les colonnes TODO a changer ?? */
#else  /* __COLPAGE__ */
	else if (pPage->ElViewPSchema == vue)
#endif /* __COLPAGE__ */
	   /* cette page concerne la vue, on la traite */
	  {
	     pEl1 = pPage;
	     numpageprec = pEl1->ElPageNumber;
	     /* cherche le compteur de page a appliquer a cette page */
	     cpt = CptPage (pPage, vue, &pSchP);
	     if (cpt == 0)
		/* page non numerotee, on s'arrete */
		stop = TRUE;
	     else
		/* calcule le nouveau numero de page */
	       {
		  pEl1->ElPageNumber = ComptVal (cpt, pPage->ElSructSchema, pSchP, pPage, vue);
		  if (pEl1->ElPageNumber == numpageprec)
		     /* le numero de page n'a pas change', on s'arrete */
		     stop = TRUE;
	       }
	  }
     }
   while (!(stop));
}


/* ---------------------------------------------------------------------- */
/* |    traiteContenu   recalcule le contenu de toutes les boites de    | */
/* |            presentation du type typeBoite (dans le schema de       | */
/* |            presentation pSchP) qui sont apres pElDebut, dans la    | */
/* |            vue nv.                                                 | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         traiteContenu (int typeBoite, int nv, PtrDocument pDoc, PtrSSchema pSS, PtrPSchema pSchP, PtrElement pElDebut, boolean reaff)
#else  /* __STDC__ */
static void         traiteContenu (typeBoite, nv, pDoc, pSS, pSchP, pElDebut, reaff)
int                 typeBoite;
int                 nv;
PtrDocument         pDoc;
PtrSSchema        pSS;
PtrPSchema          pSchP;
PtrElement          pElDebut;
boolean             reaff;

#endif /* __STDC__ */
{
   PtrAbstractBox             pAb;
   int                 frame, h;
   PtrAbstractBox             pAbbox1;

   PavDeb (pElDebut, nv);
   pAb = PavDebut[nv - 1];
   while (pAb != NULL)
     {
	pAbbox1 = pAb;
	if (pAbbox1->AbPresentationBox
	    && pAbbox1->AbTypeNum == typeBoite
	    && pAbbox1->AbPSchema == pSchP)
	   /* fait reafficher le pave de presentation si le contenu a */
	   /* change' */
	   if (NouvVariable (pSchP->PsPresentBox[typeBoite - 1].PbContVariable, pSS, pSchP, pAb, pDoc))
	      /* et si le pave a deja ete traite' par le mediateur */
	      if (!pAb->AbNew)
		{
		   pAbbox1->AbChange = TRUE;
		   if (VueAssoc (pElDebut))
		      frame = pDoc->DocAssocFrame[pElDebut->ElAssocNum - 1];
		   else
		      frame = pDoc->DocViewFrame[nv - 1];
#ifdef __COLPAGE__
		   h = HauteurCoupPage;
#else  /* __COLPAGE__ */
		   h = HauteurPage;
#endif /* __COLPAGE__ */
		   ModifVue (frame, &h, pAb);
		   /* on ne reaffiche pas si on est en train de calculer les */
		   /* pages */
#ifdef __COLPAGE__
		   if (HauteurCoupPage == 0 && reaff)
#else  /* __COLPAGE__ */
		   if (HauteurPage == 0 && reaff)
#endif /* __COLPAGE__ */
		      DisplayFrame (frame);
		   /* cherche le pave de presentation suivant de ce type */
		}
	pAb = PavCherche (pAb, TRUE, typeBoite, pSchP, NULL);
     }
}


/* ---------------------------------------------------------------------- */
/* |    traiteBoiteCreee reevalue les conditions de creation de toutes  | */
/* |            les boites de presentation du type typeBoite (dans le   | */
/* |            schema de presentation pSchP) qui sont apres pElDebut,  | */
/* |            dans la vue nv.                                         | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         traiteBoiteCreee (int typeBoite, int nv, PtrPSchema pSchP, PtrDocument pDoc, PtrElement pElDebut, boolean reaff)
#else  /* __STDC__ */
static void         traiteBoiteCreee (typeBoite, nv, pSchP, pDoc, pElDebut, reaff)
int                 typeBoite;
int                 nv;
PtrPSchema          pSchP;
PtrDocument         pDoc;
PtrElement          pElDebut;
boolean             reaff;

#endif /* __STDC__ */
{
   PtrAbstractBox             pAb, PavSuiv;
   int                 frame, h;
   boolean             trouve, stop;
   PtrPRule        pRCre;
   PtrPSchema          pSPR;
   PtrSSchema        pSSR;
   PtrAttribute         pAttr;
   int                 VueSch;
   PtrAbstractBox             pAbbox1;
   PtrPRule        pRe1;
   int                 NumPres;

   PavDeb (pElDebut, nv);
   pAb = PavDebut[nv - 1];
   PavSuiv = NULL;
   VueSch = pDoc->DocView[nv - 1].DvPSchemaView;
   while (pAb != NULL)
     {
	pAbbox1 = pAb;
	if (pAbbox1->AbPresentationBox && !pAb->AbDead
	    && pAbbox1->AbTypeNum == typeBoite
	    && pAbbox1->AbPSchema == pSchP)
	   /* cherche la regle de l'element createur, regle qui cree */
	   /* cette boite */
	  {
	     trouve = FALSE;
	     NumPres = 0;	/* a priori, le createur n'est pas un pave de pres */
	     /* cas ou le createur est lui-meme un pave de presentation */
	     /* identifie par le fait que le pere est un pave de pres */
	     /* en effet les paves de pres ne peuvent creer que des fils */
	     if (pAbbox1->AbEnclosing->AbPresentationBox)
		NumPres = pAbbox1->AbEnclosing->AbTypeNum;
	     pRCre = ReglePEl (pAbbox1->AbElement, &pSPR, &pSSR, NumPres, NULL,
			       VueSch, PtFunction, TRUE, FALSE, &pAttr);
	     stop = FALSE;
	     do
		if (pRCre == NULL)
		   stop = TRUE;
		else if (pRCre->PrType != PtFunction)
		   stop = TRUE;
		else
		  {
		     pRe1 = pRCre;
		     if (pRe1->PrViewNum == VueSch
			 && pRe1->PrPresBox[0] == typeBoite
			 && pSchP == pSPR
			 && (pRe1->PrPresFunction == FnCreateBefore
#ifndef __COLPAGE__
			     || pRe1->PrPresFunction == FnCreateWith
#endif /* __COLPAGE__ */
			     || pRe1->PrPresFunction == FnCreateAfter
			     || pRe1->PrPresFunction == FnCreateFirst
			     || pRe1->PrPresFunction == FnCreateLast))
			trouve = TRUE;
		     else
			pRCre = pRe1->PrNextPRule;
		  }
	     while (!(stop || trouve));
	     if (trouve)
		/* reevalue les conditions d'application de la regle */
		if (!CondPresentation (pRCre->PrCond, pAb->AbElement, NULL, VueSch,
				       pAb->AbElement->ElSructSchema))
		   /* On va detruire le pave, on cherche d'abord le pave de */
		   /* presentation suivant de meme type */
		  {
		     PavSuiv = PavCherche (pAb, TRUE, typeBoite, pSchP, NULL);
		     /* tue le pave */
		     TuePave (pAb);
		     /* signale le pave mort au mediateur */
		     if (VueAssoc (pElDebut))
			frame = pDoc->DocAssocFrame[pElDebut->ElAssocNum - 1];
		     else
			frame = pDoc->DocViewFrame[nv - 1];
#ifdef __COLPAGE__
		     h = HauteurCoupPage;
#else  /* __COLPAGE__ */
		     h = HauteurPage;
#endif /* __COLPAGE__ */
		     ModifVue (frame, &h, pAb);
		     /* on ne reaffiche pas si on est en train de calculer les */
		     /* pages */
#ifdef __COLPAGE__
		     if (HauteurCoupPage == 0 && reaff)
#else  /* __COLPAGE__ */
		     if (HauteurPage == 0 && reaff)
#endif /* __COLPAGE__ */
			DisplayFrame (frame);
		     /* libere le pave tue' */
		     FreeDeadAbstractBoxes (pAb);
		     pAb = NULL;
		     /* cherche le pave de presentation suivant de ce type */
		  }
	  }
	if (PavSuiv == NULL)
	  {
	     if (pAb != NULL)
		pAb = PavCherche (pAb, TRUE, typeBoite, pSchP, NULL);
	  }
	else
	  {
	     pAb = PavSuiv;
	     PavSuiv = NULL;
	  }
     }
}


/* ---------------------------------------------------------------------- */
/* |    traiteCreation  pour toutes les boites du type typeBoite (dans  | */
/* |            le schema de presentation pSchP) qui sont apres         | */
/* |            pElDebut, dans la vue nv, reevalue les conditions de    | */
/* |            creation de boites de presentation.                     | */
/* |            Pres indique si typeBoite est un type de boite de       | */
/* |            presentation ou de boite d'element structure'.          | */
/* |            Pour toutes les conditions de creation qui dependent du | */
/* |            compteur Cpt et qui sont satisfaites, la boite est      | */
/* |            creee si elle n'existe pas deja.                        | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         traiteCreation (int typeBoite, boolean Pres, int Cpt, int nv, PtrSSchema pSS, PtrPSchema pSchP, PtrDocument pDoc, PtrElement pElDebut, boolean reaff)
#else  /* __STDC__ */
static void         traiteCreation (typeBoite, Pres, Cpt, nv, pSS, pSchP, pDoc, pElDebut, reaff)
int                 typeBoite;
boolean             Pres;
int                 Cpt;
int                 nv;
PtrSSchema        pSS;
PtrPSchema          pSchP;
PtrDocument         pDoc;
PtrElement          pElDebut;
boolean             reaff;

#endif /* __STDC__ */
{
   PtrAbstractBox             pAb, pAbb, pAbbNext;
   PtrElement          pEl;
   int                 frame, h;
   boolean             stop, dejacree, depend, boxok, page;
   PtrPRule        pRCre;
   PtrPSchema          pSPR, pSchPOrig;
   PtrSSchema        pSSR;
   PtrAttribute         pAttr;
   int                 VueSch;
   PtrAbstractBox             pAbbox1;
   PtrPRule        pRe1;
   PtrCondition        pCond;
   int                 NumPres;

#ifdef __COLPAGE__
   PtrAbstractBox             pPrevious, pRac;
   PtrElement          pElRac;

#endif /* __COLPAGE__ */

   PavDeb (pElDebut, nv);
   pAb = PavDebut[nv - 1];
   VueSch = pDoc->DocView[nv - 1].DvPSchemaView;
   page = FALSE;
   if (Pres)
      if (pSchP->PsPresentBox[typeBoite - 1].PbPageBox)
	 /* c'est une boite page */
	 page = TRUE;
   while (pAb != NULL)
     {
#ifdef __COLPAGE__
	if (!pAb->AbDead)
	  {
#endif /* __COLPAGE__ */
	     boxok = FALSE;
	     pAbbox1 = pAb;
	     if (pAbbox1->AbPresentationBox == Pres
		 && pAbbox1->AbTypeNum == typeBoite
		 && pAbbox1->AbPSchema == pSchP)
		boxok = TRUE;
	     else if (Pres)
		if (pSchP->PsPresentBox[typeBoite - 1].PbPageBox)
		   if (pAbbox1->AbElement->ElTerminal
		       && pAbbox1->AbElement->ElLeafType == LtPageColBreak
#ifdef __COLPAGE__
		       && (pAbbox1->AbElement->ElPageType == PgBegin
			   || pAbbox1->AbElement->ElPageType == PgComputed
			 || pAbbox1->AbElement->ElPageType == PgUser)
#endif /* __COLPAGE__ */
		       && pAbbox1->AbLeafType == LtCompound)
		      /* c'est un element Marque de page */
		      /* mais pas un pave de colonne */
		     {
			/*attention TypeBPage est susceptible de modifier pSchP 
			   il faut donc prendre des precautions */
			pSchPOrig = pSchP;
			if (typeBoite == TypeBPage (pAbbox1->AbElement, VueSch, &pSchPOrig)
			    && pSchPOrig == pSchP)
			   /* c'est bien ce type de boite page */
			   boxok = TRUE;
		     }

	     if (boxok)
		/* cherche toutes les regles de creation de cette boite */
	       {
		  NumPres = 0;	/* a priori pAb n'est pas une boite de presentation */
		  if (pAb->AbPresentationBox)
		     NumPres = typeBoite;
		  pRCre = ReglePEl (pAb->AbElement, &pSPR, &pSSR, NumPres, NULL,
				    VueSch, PtFunction, TRUE, FALSE, &pAttr);
		  stop = FALSE;
		  do
		     if (pRCre == NULL)
			/* il n' y a plus de regles */
			stop = TRUE;
		     else if (pRCre->PrType != PtFunction)
			/* il n'y a plus de regle fonction de presentation */
			stop = TRUE;
		     else
			/* la regle est une fonction de presentation */
		       {
			  pRe1 = pRCre;
			  if (pRe1->PrViewNum == VueSch
			      && (pRe1->PrPresFunction == FnCreateBefore
#ifndef __COLPAGE__
				  || pRe1->PrPresFunction == FnCreateWith
#endif /* __COLPAGE__ */
				  || pRe1->PrPresFunction == FnCreateAfter
				  || pRe1->PrPresFunction == FnCreateFirst
				  || pRe1->PrPresFunction == FnCreateLast))
			     /* c'est une regle de creation pour cette vue */
			     /* la creation depend-elle du compteur Cpt ? */
			    {
			       depend = FALSE;
			       pCond = pRe1->PrCond;
			       while (pCond != NULL && !depend)
				 {
				    if (pCond->CoCondition == PcInterval ||
					pCond->CoCondition == PcEven ||
					pCond->CoCondition == PcOdd ||
					pCond->CoCondition == PcOne)
				       if (pCond->CoCounter == Cpt)
					  depend = TRUE;
				    pCond = pCond->CoNextCondition;
				 }
			       if (depend)
				  /* reevalue les conditions d'application de la regle */
				  if (CondPresentation (pRCre->PrCond, pAb->AbElement,
							NULL, VueSch, pAb->AbElement->ElSructSchema))
				     /* cherche si le pave est deja cree' */
				    {
				       dejacree = FALSE;
#ifdef __COLPAGE__
				       /* cas page supprime, ce n'est plus un cas particulier */
				       if (pRe1->PrPresFunction == FnCreateFirst
					   || pRe1->PrPresFunction == FnCreateLast)
					 {
					    /* cas particulier des paves de presentation de */
					    /* la racine : ils sont sous la premiere et la */
					    /* derniere page si le document est pagine */
					    if (VueAssoc (pAb->AbElement))
					      {
						 pElRac = pDoc->
						    DocAssocRoot[pAb->AbElement->ElAssocNum - 1];
						 pRac = pElRac->ElAbstractBox[0];
					      }
					    else
					      {
						 pElRac = pDoc->DocRootElement;
						 pRac = pElRac->ElAbstractBox[nv - 1];
					      }
					    if (pAb == pRac
						&& (!pAb->AbTruncatedHead || !pAb->AbTruncatedTail))
					      {
						 /* le document est-il pagine ? */
						 /* cad les paves fils sont des paves de page */
						 pAbb = pAb->AbFirstEnclosed;
						 if (pAbb != NULL
						     && pAbb->AbElement->ElTerminal
						     && pAbb->AbElement->ElLeafType == LtPageColBreak)
						   {
						      pEl = pAbb->AbElement;
						      /* cherche le premier corps de page: on saute */
						      /* le (les ?) premier haut de page eventuel */
						      while (pAbb != NULL && pAbb->AbElement == pEl
							  && pAbb->AbPresentationBox)
							 pAbb = pAbb->AbNext;
						      /* pAbb est le premier corps de page */
						      /* les premiers fils de la racine sont ses */
						      /* premiers fils */
						      if (!pAb->AbTruncatedHead)
							{
							   if (pAbb != NULL)
							      pAbb = pAbb->AbFirstEnclosed;
							   while (!dejacree && pAbb != NULL
								  && pAbb->AbElement == pElRac)
							      if (pAbb->AbPresentationBox
								  && pAbb->AbTypeNum == pRe1->PrPresBox[0]
								  && pAbb->AbPSchema == pSPR)
								 dejacree = TRUE;
							      else
								 pAbb = pAbb->AbNext;
							}
						      /* cas de la derniere page (derniers fils) */
						      if (!pAb->AbTruncatedTail && !dejacree)
							{
							   /* on cherche le dernier pave corps */
							   /* de page */
							   pPrevious = NULL;
							   pAbb = pAb->AbFirstEnclosed;
							   while (pAbb != NULL)
							     {
								if (!pAbb->AbPresentationBox)
								   pPrevious = pAbb;
								pAbb = pAbb->AbNext;
							     }
							   /* pPrevious est le dernier pave corps de page */
							   if (pPrevious != NULL)
							     {
								pAbb = pPrevious->AbFirstEnclosed;
								if (pAbb != NULL)
								   while (pAbb->AbNext != NULL)
								      pAbb = pAbb->AbNext;
								/*pAbb est le dernier pave fils du corps de page */
								while (!dejacree && pAbb != NULL
								       && pAbb->AbElement == pElRac)
								   if (pAbb->AbPresentationBox
								       && pAbb->AbTypeNum == pRe1->PrPresBox[0]
								       && pAbb->AbPSchema == pSPR)
								      dejacree = TRUE;
								   else
								      pAbb = pAbb->AbPrevious;
							     }
							}
						   }
					      }
					    /* fin du cas particulier de la racine */
					    else
					      {
						 pAbb = pAb->AbFirstEnclosed;
						 while (!dejacree && pAbb != NULL)
						    if (pAbb->AbPresentationBox
							&& pAbb->AbTypeNum == pRe1->PrPresBox[0]
							&& pAbb->AbPSchema == pSPR)
						       dejacree = TRUE;
						    else if (pAbb->AbNext != NULL)
						       pAbb = pAbb->AbNext;
						    else
						       pAbb = pAbb->AbNextRepeated;
					      }
					 }
#else  /* __COLPAGE__ */
				       if (page || pRe1->PrPresFunction == FnCreateFirst
					   || pRe1->PrPresFunction == FnCreateLast)
					 {
					    pAbb = pAb->AbFirstEnclosed;
					    while (!dejacree && pAbb != NULL)
					       if (pAbb->AbPresentationBox
						   && pAbb->AbTypeNum == pRe1->PrPresBox[0]
						   && pAbb->AbPSchema == pSPR)
						  dejacree = TRUE;
					       else
						  pAbb = pAbb->AbNext;
					 }
#endif /* __COLPAGE__ */
				       else
					 {
					    pAbb = pAb;
					    while (!dejacree && pAbb != NULL
						   && pAbb->AbElement == pAb->AbElement)
					       if (pAbb->AbPresentationBox
						   && pAbb->AbTypeNum == pRe1->PrPresBox[0]
						   && pAbb->AbPSchema == pSPR)
						  dejacree = TRUE;
					       else
						  pAbb = pAbb->AbNext;
					 }
				       if (!dejacree)
					  /* on cree le pave de presentation */
					 {
					    pEl = pAb->AbElement;
#ifndef __COLPAGE__
					    /* code supprime car les regles de creation des hauts et bas de page */
					    /* sont definies a partir du corps de page qui est au meme niveau */
					    /* que pAb */
					    if (pEl->ElTerminal
					     && pEl->ElLeafType == LtPageColBreak)
					      {
						 pAbbNext = pEl->ElAbstractBox[nv - 1];
						 pAbb = pAbbNext->AbFirstEnclosed;
						 if (pAbb != NULL)
						   {
						      while (pAbb->AbPresentationBox)
							 pAbb = pAbb->AbNext;
						      pEl->ElAbstractBox[nv - 1] = pAbb;
						   }
					      }
					    else
					       pAbbNext = NULL;
#endif /* __COLPAGE__ */
					    pAbb = CrPavPres (pEl, pDoc, pRCre, pSS, pAttr,
						    nv, pSchP, FALSE, TRUE);
					    if (pAbbNext != NULL)
					       pEl->ElAbstractBox[nv - 1] = pAbbNext;
					    if (pAbb != NULL)
					      {
						 if (VueAssoc (pElDebut))
						    frame =
						       pDoc->DocAssocFrame[pElDebut->ElAssocNum - 1];
						 else
						    frame = pDoc->DocViewFrame[nv - 1];
#ifdef __COLPAGE__
						 h = HauteurCoupPage;
#else  /* __COLPAGE__ */
						 h = HauteurPage;
#endif /* __COLPAGE__ */
						 ModifVue (frame, &h, pAbb);
						 /* on ne reaffiche pas si on est en
						    train de calculer les pages */
#ifdef __COLPAGE__
						 if (HauteurCoupPage == 0 && reaff)
#else  /* __COLPAGE__ */
						 if (HauteurPage == 0 && reaff)
#endif /* __COLPAGE__ */
						    DisplayFrame (frame);
						 /* on passe a la regle suivante */
					      }
					 }
				    }
			    }
			  pRCre = pRe1->PrNextPRule;
		       }
		  while (!(stop));
	       }
#ifdef __COLPAGE__
	  }			/* fin si pAb pas mort */
#endif /* __COLPAGE__ */
	/* cherche le pave suivant de ce type */
	if (page)
	   /* on cherche une boite page */
	   pAb = PavCherche (pAb, FALSE, PageBreak + 1, NULL, NULL);
	else
	   pAb = PavCherche (pAb, Pres, typeBoite, pSchP, NULL);
     }
}


/* ---------------------------------------------------------------------- */
/* |    AttachValCompt Transmet a l'attribut de nom NmAttr              | */
/* |            la valeur du compteur cpt defini dans pSchP associe' a  | */
/* |            pSchS pour pEl. pEl peut est une reference a un document| */
/* |            inclus pElInclus peut etre soit le document inclus, soit| */
/* |            le document inclus dans sa forme demi expansee: il est  | */
/* |            alors dans le  document l'incluant.                     | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         AttachValCompt (PtrElement pEl, PtrElement pElInclus, PtrDocument pDocInclus, Name NmAttr, int cpt, PtrPSchema pSchP, PtrSSchema pSchS)

#else  /* __STDC__ */
static void         AttachValCompt (pEl, pElInclus, pDocInclus, NmAttr, cpt, pSchP, pSchS)
PtrElement          pEl;
PtrElement          pElInclus;
PtrDocument         pDocInclus;
Name                 NmAttr;
int                 cpt;
PtrPSchema          pSchP;
PtrSSchema        pSchS;

#endif /* __STDC__ */

{
   int                 att;
   PtrAttribute         pAttr;
   TtAttribute           *DefAttr;
   boolean             trouve;

   if (pElInclus != NULL)
     {
	/* le document inclus est charge', cherche l'attribut dans */
	/* son schema de structure */
	att = 0;
	trouve = FALSE;
	while (att < pElInclus->ElSructSchema->SsNAttributes && !trouve)
	  {
	     att++;
	     DefAttr = &(pElInclus->ElSructSchema->SsAttribute[att - 1]);
	     if (DefAttr->AttrType == AtNumAttr)
		/* c'est un attribut numerique */
		if (strncmp (DefAttr->AttrOrigName, NmAttr, sizeof (Name)) == 0)
		   /* il a le nom cherche' */
		   trouve = TRUE;
	  }
	if (trouve)
	  {
	     /* l'attribut est bien defini dans le schema de structure du */
	     /* document inclus, on le met sur la racine */
	     GetAttr (&pAttr);
	     pAttr->AeAttrSSchema = pElInclus->ElSructSchema;
	     pAttr->AeAttrNum = att;
	     pAttr->AeAttrType = AtNumAttr;
	     pAttr->AeAttrValue = ComptVal (cpt, pSchS, pSchP, pEl, 1);
	     AttachAttrWithValue (pElInclus, pDocInclus, pAttr);
	     DeleteAttribute (NULL, pAttr);
	  }
     }
}



/* ---------------------------------------------------------------------- */
/* |    TransmetValCompt pEl (appartenant au document pDoc) est une     | */
/* |            inclusion de document externe. Transmet a l'attribut de | */
/* |            nom NmAttr du document inclus la valeur du compteur     | */
/* |            cpt defini dans le schema de presentation pSchP associe'| */
/* |            au schema de structure pSchS.                           | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TransmetValCompt (PtrElement pEl, PtrDocument pDoc, Name NmAttr, int cpt, PtrPSchema pSchP, PtrSSchema pSchS)

#else  /* __STDC__ */
void                TransmetValCompt (pEl, pDoc, NmAttr, cpt, pSchP, pSchS)
PtrElement          pEl;
PtrDocument         pDoc;
Name                 NmAttr;
int                 cpt;
PtrPSchema          pSchP;
PtrSSchema        pSchS;

#endif /* __STDC__ */

{
   PtrElement          pElInclus;
   PtrReference        pRef;
   PtrDocument         pDocInclus;
   DocumentIdentifier     IdentDocInclus;

   /* verifie d'abord qu'il s'agit bien d'une inclusion de document */
   {
      if (pEl->ElSructSchema->SsRule[pEl->ElTypeNumber - 1].SrConstruct == CsReference)
	 pRef = pEl->ElReference;
      else
	 pRef = pEl->ElSource;
      /* accede au document inclus (a sa racine) */
      pElInclus = ReferredElement (pRef, &IdentDocInclus, &pDocInclus);

      /* Transmet au document inclus, en externe */
      AttachValCompt (pEl, pElInclus, pDocInclus, NmAttr, cpt, pSchP, pSchS);
      /* Transmet au document inclus, semi expanse */
      AttachValCompt (pEl, pEl, pDoc, NmAttr, cpt, pSchP, pSchS);
   }
}



/* ---------------------------------------------------------------------- */
/* |    ChngBoiteCompteur dans le document dont le contexte pDoc, change| */
/* |            le contenu de toutes les boites de presentation qui sont| */
/* |            affectees par le compteur cpt du schema de presentation | */
/* |            pSchP, apartir de l'element pElDebut.                   | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         ChngBoiteCompteur (PtrElement pElDebut, PtrDocument pDoc, int cpt, PtrPSchema pSchP, PtrSSchema pSS, boolean reaff)

#else  /* __STDC__ */
static void         ChngBoiteCompteur (pElDebut, pDoc, cpt, pSchP, pSS, reaff)
PtrElement          pElDebut;
PtrDocument         pDoc;
int                 cpt;
PtrPSchema          pSchP;
PtrSSchema        pSS;
boolean             reaff;

#endif /* __STDC__ */

{
   int                 util, vue;
   Counter           *pCo1;
   PtrElement          pEl;
   int                 regle;
   Counter           *pCompt;
   PtrElement          pElRoot;

   /* ce code etait precedemment plus loin */
   /* On traite toutes les boites qui utilisent ce compteur comme contenu */
   pCo1 = &pSchP->PsCounter[cpt - 1];
   for (util = 1; util <= pCo1->CnNPresBoxes; util++)
      if (VueAssoc (pElDebut))
	 traiteContenu (pCo1->CnPresBox[util - 1], 1, pDoc, pSS, pSchP, pElDebut,
			reaff);
      else
	 for (vue = 1; vue <= MAX_VIEW_DOC; vue++)
	    if (pDoc->DocView[vue - 1].DvPSchemaView > 0)
	      {
		 /* Il faut determiner si on doit reevaluer du debut de l'image
		    abstraite ou de pElDebut; on ne reevalue que si CnMinMaxPresBox est
		    TRUE, i.e. une boite est creee par une condition de min ou de max
		    du compteur */
		 if (!pCo1->CnMinMaxPresBox[util - 1])
		    traiteContenu (pCo1->CnPresBox[util - 1], vue, pDoc, pSS, pSchP,
				   pElDebut, reaff);
		 else
		   {
		      /* On determine le debut de l'image abstraite */
		      if (pDoc->DocViewRootAb[vue - 1] != NULL)
			 pElRoot = pDoc->DocViewRootAb[vue - 1]->AbElement;
		      else
			 pElRoot = NULL;
		      if (pElRoot != NULL)
			{
			   PavDebut[vue - 1] = NULL;
			   traiteContenu (pCo1->CnPresBox[util - 1], vue, pDoc, pSS, pSchP,
					  pElRoot, reaff);
			}
		      else
			 traiteContenu (pCo1->CnPresBox[util - 1], vue, pDoc, pSS, pSchP,
					pElDebut, reaff);
		   }
	      }
   /* On traite toutes les boites dont la creation est conditionnee par */
   /* la valeur de ce compteur */
   for (util = 1; util <= pCo1->CnNCreatedBoxes; util++)
      if (VueAssoc (pElDebut))
	 traiteBoiteCreee (pCo1->CnCreatedBox[util - 1], 1, pSchP, pDoc, pElDebut,
			   reaff);
      else
	 for (vue = 1; vue <= MAX_VIEW_DOC; vue++)
	    if (pDoc->DocView[vue - 1].DvPSchemaView > 0)
	      {
		 /* Il faut determiner si on doit reevaluer depuis le debut de l'image
		    abstraite ou depuis pElDebut; on ne reevalue que si CnMinMaxCreatedBox
		    est TRUE, i.e. si une boite est creee par une condition de min ou
		    de max du compteur */
		 if (!pCo1->CnMinMaxCreatedBox[util - 1])
		    traiteBoiteCreee (pCo1->CnCreatedBox[util - 1], vue, pSchP, pDoc,
				      pElDebut, reaff);
		 else
		   {
		      /* On determine le debut de l'image abstraite */
		      if (pDoc->DocViewRootAb[vue - 1] != NULL)
			 pElRoot = pDoc->DocViewRootAb[vue - 1]->AbElement;
		      else
			 pElRoot = NULL;
		      if (pElRoot != NULL)
			{
			   PavDebut[vue - 1] = NULL;
			   traiteBoiteCreee (pCo1->CnCreatedBox[util - 1], vue, pSchP, pDoc,
					     pElRoot, reaff);
			}
		      else
			 traiteBoiteCreee (pCo1->CnCreatedBox[util - 1], vue, pSchP, pDoc,
					   pElDebut, reaff);
		   }
	      }
   /* On traite toutes les boites qui creent d'autres boites selon la */
   /* valeur de ce compteur */
   for (util = 1; util <= pCo1->CnNCreators; util++)
      if (VueAssoc (pElDebut))
	 traiteCreation (pCo1->CnCreator[util - 1],
			 pCo1->CnPresBoxCreator[util - 1], cpt, 1, pSS, pSchP,
			 pDoc, pElDebut, reaff);
      else
	 for (vue = 1; vue <= MAX_VIEW_DOC; vue++)
	    if (pDoc->DocView[vue - 1].DvPSchemaView > 0)
	      {
		 /* Il faut determiner si on doit reevaluer du debut de l'image
		    abstraite ou de pElDebut; on ne reevalue que si CnMinMaxCreator est
		    TRUE, i.e. si une boite est creee par une condition de min ou de max
		    du compteur */
		 if (!pCo1->CnMinMaxCreator[util - 1])
		    traiteCreation (pCo1->CnCreator[util - 1],
				  pCo1->CnPresBoxCreator[util - 1], cpt, vue,
				    pSS, pSchP, pDoc, pElDebut, reaff);
		 else
		   {
		      /* On determine le debut de l'image abstraite */
		      if (pDoc->DocViewRootAb[vue - 1] != NULL)
			 pElRoot = pDoc->DocViewRootAb[vue - 1]->AbElement;
		      else
			 pElRoot = NULL;
		      if (pElRoot != NULL)
			{
			   PavDebut[vue - 1] = NULL;
			   traiteCreation (pCo1->CnCreator[util - 1],
				  pCo1->CnPresBoxCreator[util - 1], cpt, vue,
					   pSS, pSchP, pDoc, pElRoot, reaff);
			}
		      else
			 traiteCreation (pCo1->CnCreator[util - 1],
					 pCo1->CnPresBoxCreator[util - 1], cpt, vue, pSS, pSchP,
					 pDoc, pElDebut, reaff);
		   }
	      }
   /* On traite les regles de transmission des valeurs du compteur */
   pCompt = &pSchP->PsCounter[cpt - 1];
   /* examine toutes les regles Transmit du compteur */
   for (regle = 1; regle <= pCompt->CnNTransmAttrs; regle++)
     {
	pEl = pElDebut;
	while (pEl != NULL)
	  {
	     if (pEl->ElTypeNumber == pCompt->CnTransmSSchemaAttr[regle - 1])
		/* c'est le type de document auquel le compteur est */
		/* transmis, on applique la regle de transmission */
		TransmetValCompt (pEl, pDoc, pCompt->CnTransmAttr[regle - 1],
				  cpt, pSchP, pSS);
	     /* cherche le document inclus suivant */
	     pEl = FwdSearchTypedElem (pEl, pCompt->CnTransmSSchemaAttr[regle - 1], pSS);
	  }
     }
}



/* ---------------------------------------------------------------------- */
/* |    MajNum1Elem met a jour et fait reafficher les numeros qui       | */
/* |            apparaissent a partir du sous-arbre pointe par pElDebut | */
/* |            (lui-meme compris) et qui sont affectes par l'element   | */
/* |            pElModif.                                               | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         MajNum1Elem (PtrElement pElDebut, PtrElement pElModif, PtrDocument pDoc, boolean reaff)

#else  /* __STDC__ */
static void         MajNum1Elem (pElDebut, pElModif, pDoc, reaff)
PtrElement          pElDebut;
PtrElement          pElModif;
PtrDocument         pDoc;
boolean             reaff;

#endif /* __STDC__ */

{
   int                 cpt, oper, i;
   PtrPSchema          pSchP;
   PtrSSchema        pSS;
   int                 Entree;
   boolean             declanche;
   Counter           *pCo1;
   CntrItem            *pCp1;
   SRule              *pRe1;

   /* si l'element pElModif est une marque de page, renumerote les */
   /* les sauts de page qui suivent, a partir de pElDebut. */
   if (pElModif->ElTerminal && pElModif->ElLeafType == LtPageColBreak)
      ReNumPages (pElDebut, pElModif->ElViewPSchema);
   /* cherche le schema de presentation de l'element : pSchP */
   ChSchemaPres (pElModif, &pSchP, &Entree, &pSS);
   if (pSchP != NULL)
      /* cherche les compteurs affectes par pElModif */
      for (cpt = 1; cpt <= pSchP->PsNCounters; cpt++)
	 /* pour tous les compteurs du schema */
	{
	   pCo1 = &pSchP->PsCounter[cpt - 1];
	   /* examine toutes les operations du compteur */
	   for (oper = 1; oper <= pCo1->CnNItems; oper++)
	     {
		pCp1 = &pCo1->CnItem[oper - 1];
		/* teste si le type de l'element pElModif declanche */
		/* l'operation sur le compteur */
		declanche = FALSE;
		if (pCp1->CiElemType == Entree)
		   /* l'element a le type qui declanche l'operation */
		   declanche = TRUE;
		else
		   /* cherche si le type de l'element est equivalent a celui */
		   /* /qui declanche l'operation */
		  {
		     pRe1 = &pSS->SsRule[pCp1->CiElemType - 1];
		     if (pRe1->SrConstruct == CsChoice && pRe1->SrNChoices > 0)
		       {
			  i = 0;
			  do
			    {
			       i++;
			       if (pRe1->SrChoice[i - 1] == Entree)
				  declanche = TRUE;
			    }
			  while (!(declanche || i >= pRe1->SrNChoices));
		       }
		  }
		if (declanche)
		   /* l'operation du compteur cpt est declanchee par les */
		   /* elements du type de pElModif. */
		   ChngBoiteCompteur (pElDebut, pDoc, cpt, pSchP, pSS, reaff);
	     }
	}
}



/* ---------------------------------------------------------------------- */
/* |    MajNum                                                          | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         MajNum (PtrElement pElD, PtrElement pElM, PtrDocument pDoc, boolean reaff)

#else  /* __STDC__ */
static void         MajNum (pElD, pElM, pDoc, reaff)
PtrElement          pElD;
PtrElement          pElM;
PtrDocument         pDoc;
boolean             reaff;

#endif /* __STDC__ */

{
   PtrElement          pEl;

   if (pElD != NULL && pElM != NULL)
      /* traite recursivement le sous-arbre pointe par pElM */
     {
	MajNum1Elem (pElD, pElM, pDoc, reaff);
	if (!pElM->ElTerminal)
	  {
	     pEl = pElM->ElFirstChild;
	     while (pEl != NULL)
	       {
		  MajNum (pElD, pEl, pDoc, reaff);
		  pEl = pEl->ElNext;
	       }
	  }
     }
}



/* ---------------------------------------------------------------------- */
/* |    MajNumeros pour le document dont le contexte pDocu, met a jour  | */
/* |            et fait reafficher les numeros qui apparaissent a partir| */
/* |            du sous-arbre pointe par pElDebut (lui-meme compris) et | */
/* |            qui sont affectes par les elements du sous-arbre        | */
/* |            (racine comprise) pointe par pElModif.                  | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                MajNumeros (PtrElement pElDebut, PtrElement pElModif, PtrDocument pDocu, boolean reaff)

#else  /* __STDC__ */
void                MajNumeros (pElDebut, pElModif, pDocu, reaff)
PtrElement          pElDebut;
PtrElement          pElModif;
PtrDocument         pDocu;
boolean             reaff;

#endif /* __STDC__ */

{
   int                 i;

   for (i = 1; i <= MAX_VIEW_DOC; i++)
      PavDebut[i - 1] = NULL;
   MajNum (pElDebut, pElModif, pDocu, reaff);
}



/* ---------------------------------------------------------------------- */
/* |    ChngBtCompt reaffiche toutes les boites de presentation de pDoc,| */
/* |            qui se trouvent a l'interieur de et apres l'element     | */
/* |            pointe' par pElDebut et dont le contenu depend du       | */
/* |            compteur cpt defini dans le schema de presentation      | */
/* |            pSchP.                                                  | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                ChngBtCompt (PtrElement pElDebut, PtrDocument pDoc, int cpt, PtrPSchema pSchP, PtrSSchema pSS)

#else  /* __STDC__ */
void                ChngBtCompt (pElDebut, pDoc, cpt, pSchP, pSS)
PtrElement          pElDebut;
PtrDocument         pDoc;
int                 cpt;
PtrPSchema          pSchP;
PtrSSchema        pSS;

#endif /* __STDC__ */

{
   int                 i;

   for (i = 1; i <= MAX_VIEW_DOC; i++)
      PavDebut[i - 1] = NULL;
   ChngBoiteCompteur (pElDebut, pDoc, cpt, pSchP, pSS, TRUE);
}



/* ---------------------------------------------------------------------- */
/* |    MarqueChng marque dans le pave pAbb que la regle de type TRegle a | */
/* |            change'.                                                | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         MarqueChng (PtrAbstractBox pAbb, PRuleType TRegle)

#else  /* __STDC__ */
static void         MarqueChng (pAbb, TRegle)
PtrAbstractBox             pAbb;
PRuleType           TRegle;

#endif /* __STDC__ */

{

   switch (TRegle)
	 {
	    case PtWidth:
	       pAbb->AbWidthChange = TRUE;
	       break;
	    case PtHeight:
	       pAbb->AbHeightChange = TRUE;
	       break;
	    case PtHorizPos:
	       pAbb->AbHorizPosChange = TRUE;
	       break;
	    case PtVertPos:
	       pAbb->AbVertPosChange = TRUE;
	       break;
	    case PtHorizRef:
	       pAbb->AbHorizRefChange = TRUE;
	       break;
	    case PtVertRef:
	       pAbb->AbVertRefChange = TRUE;
	       break;
	    case PtSize:
	       pAbb->AbSizeChange = TRUE;
	       break;
	    case PtDepth:
	    case PtLineStyle:
	    case PtLineWeight:
	    case PtFillPattern:
	    case PtBackground:
	    case PtForeground:
	       pAbb->AbAspectChange = TRUE;
	       break;
	    default:
	       pAbb->AbChange = TRUE;
	       break;
	 }

}


/* ---------------------------------------------------------------------- */
/* |    HeritPres si la regle de presentation de type TReglePres        | */
/* |            qui doit s'appliquer au pave pAb est une regle         | */
/* |            d'heritage, on applique cette regle au pave' pointe'    | */
/* |            par pAb, et on fait de meme sur son sous-arbre.        | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         HeritPres (PtrAbstractBox pAb, PRuleType TReglePres, PtrDocument pDoc)

#else  /* __STDC__ */
static void         HeritPres (pAb, TReglePres, pDoc)
PtrAbstractBox             pAb;
PRuleType           TReglePres;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrPRule        pRPres;
   PtrAbstractBox             pAbbChild;
   PtrPSchema          pSPR;
   PtrAttribute         pA;

#ifdef __COLPAGE__
   boolean             bool;

#endif /* __COLPAGE__ */

   /* on ne traite que si le type de la regle permet l'heritage */
   if (TReglePres == PtVisibility
       || TReglePres == PtDepth
       || TReglePres == PtSize
       || TReglePres == PtStyle
       || TReglePres == PtFont
       || TReglePres == PtUnderline
       || TReglePres == PtThickness
       || TReglePres == PtIndent
       || TReglePres == PtLineSpacing
       || TReglePres == PtAdjust
       || TReglePres == PtJustify
       || TReglePres == PtHyphenate
       || TReglePres == PtLineStyle
       || TReglePres == PtLineWeight
       || TReglePres == PtFillPattern
       || TReglePres == PtBackground
       || TReglePres == PtForeground)
      /* cherche la regle de ce type qui s'applique au pave' */
     {
	pRPres = LaRegle (pDoc, pAb, &pSPR, TReglePres, TRUE, &pA);
	if (pRPres->PrPresMode == PresInherit)
	   /* c'est une regle d'heritage, on l'applique au pave' */
#ifdef __COLPAGE__
	   if (Applique (pRPres, pSPR, pAb, pDoc, pA, &bool))
#else  /* __COLPAGE__ */
	   if (Applique (pRPres, pSPR, pAb, pDoc, pA))
#endif /* __COLPAGE__ */
	      /* le pave est modifie' */
	     {
		if (TReglePres == PtSize)
		   pAb->AbSizeChange = TRUE;
		else if (TReglePres == PtDepth || TReglePres == PtLineStyle ||
			 TReglePres == PtLineWeight ||
			 TReglePres == PtFillPattern ||
		  TReglePres == PtBackground || TReglePres == PtForeground)
		   pAb->AbAspectChange = TRUE;
		else
		   pAb->AbChange = TRUE;
		/* traite les paves fils */
		pAbbChild = pAb->AbFirstEnclosed;
		while (pAbbChild != NULL)
		  {
		     HeritPres (pAbbChild, TReglePres, pDoc);
		     pAbbChild = pAbbChild->AbNext;
		  }
	     }
     }
}


/* ---------------------------------------------------------------------- */
/* |    AppliqueRPres applique au pave pAb appartenant au document     | */
/* |            pDoc la regle de presentation pRegle appartenant au     | */
/* |            schema de presentation pSchP.                           | */
/* |            S'il s'agit d'une regle de position ou de dimension,    | */
/* |            que le pave pAb est elastique et que sa boite a ete    | */
/* |            inversee, on reapplique la regle complementaire         | */
/* |            (regle de dimension pour une regle de position et       | */
/* |            inversement).                                           | */
/* |            Retourne Vrai si la regle a ete appliquee.              | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static boolean      AppliqueRPres (PtrPRule pRegle, PtrPSchema pSchP, PtrAbstractBox pAb, PtrDocument pDoc, PtrAttribute pAttr)

#else  /* __STDC__ */
static boolean      AppliqueRPres (pRegle, pSchP, pAb, pDoc, pAttr)
PtrPRule        pRegle;
PtrPSchema          pSchP;
PtrAbstractBox             pAb;
PtrDocument         pDoc;
PtrAttribute         pAttr;

#endif /* __STDC__ */

{
   PtrPRule        pRegle2;
   PtrPSchema          pSchP2;
   PtrAttribute         pAttr2;
   boolean             ret;

#ifdef __COLPAGE__
   boolean             bool;

#endif /* __COLPAGE__ */

   /* applique la regle a` appliquer */
#ifdef __COLPAGE__
   ret = Applique (pRegle, pSchP, pAb, pDoc, pAttr, &bool);
#else  /* __COLPAGE__ */
   ret = Applique (pRegle, pSchP, pAb, pDoc, pAttr);
#endif /* __COLPAGE__ */
   /* verifie si c'est une regle de dimension ou de position */
   /* qui s'applique a un pave' elastique dont la boite a ete */
   /* inversee. Dans ce cas reapplique la regle de position ou */
   /* de dimension */
   if (ret && pRegle != NULL)
      if (pRegle->PrType == PtVertPos || pRegle->PrType == PtHeight)
	 /* c'est une regle de dimension ou de position verticale */
	{
	   if (pAb->AbHeight.DimIsPosition)
	      /* le pave est elastique verticalement */
	      if (pAb->AbBox != NULL)
		 /* le pave possede une boite dans l'image concrete */
		 if (pAb->AbBox->BxVertInverted)
		    /* la boite a ete inverse'e verticalement, on */
		    /* reapplique la regle complementaire */
		    if (pRegle->PrType == PtVertPos)
		      {
			 pRegle2 = LaRegle (pDoc, pAb, &pSchP2, PtHeight,
					    TRUE, &pAttr2);
			 if (pRegle2 != NULL)
#ifdef __COLPAGE__
			    if (Applique (pRegle2, pSchP2, pAb, pDoc, pAttr2, &bool))
#else  /* __COLPAGE__ */
			    if (Applique (pRegle2, pSchP2, pAb, pDoc, pAttr2))
#endif /* __COLPAGE__ */
			       pAb->AbHeightChange = TRUE;
		      }
		    else
		      {
			 pRegle2 = LaRegle (pDoc, pAb, &pSchP2, PtVertPos,
					    TRUE, &pAttr2);
			 if (pRegle2 != NULL)
#ifdef __COLPAGE__
			    if (Applique (pRegle2, pSchP2, pAb, pDoc, pAttr2, &bool))
#else  /* __COLPAGE__ */
			    if (Applique (pRegle2, pSchP2, pAb, pDoc, pAttr2))
#endif /* __COLPAGE__ */
			       pAb->AbVertPosChange = TRUE;
		      }
	}
      else if (pRegle->PrType == PtHorizPos || pRegle->PrType == PtWidth)

	 /* c'est une regle de dimension ou de position horiz. */
	{
	   if (pAb->AbWidth.DimIsPosition)
	      /* le pave est elastique horizontalement */
	      if (pAb->AbBox != NULL)
		 /* le pave possede une boite dans l'image concrete */
		 if (pAb->AbBox->BxHorizInverted)
		    /* la boite a ete inverse'e horizontalement, on */
		    /* reapplique la regle complementaire */
		    if (pRegle->PrType == PtHorizPos)
		      {
			 pRegle2 = LaRegle (pDoc, pAb, &pSchP2, PtWidth,
					    TRUE, &pAttr2);
			 if (pRegle2 != NULL)
#ifdef __COLPAGE__
			    if (Applique (pRegle2, pSchP2, pAb, pDoc, pAttr2, &bool))
#else  /* __COLPAGE__ */
			    if (Applique (pRegle2, pSchP2, pAb, pDoc, pAttr2))
#endif /* __COLPAGE__ */
			       pAb->AbWidthChange = TRUE;
		      }
		    else
		      {
			 pRegle2 = LaRegle (pDoc, pAb, &pSchP2, PtHorizPos,
					    TRUE, &pAttr2);
			 if (pRegle2 != NULL)
#ifdef __COLPAGE__
			    if (Applique (pRegle2, pSchP2, pAb, pDoc, pAttr2, &bool))
#else  /* __COLPAGE__ */
			    if (Applique (pRegle2, pSchP2, pAb, pDoc, pAttr2))
#endif /* __COLPAGE__ */
			       pAb->AbHorizPosChange = TRUE;
		      }
	}
   return ret;
}



/* ---------------------------------------------------------------------- */
/* |    ChngPresAttr Pour l'element pEl dans le document pDoc, supprime | */
/* |            ou applique (selon Suppr) la presentation attachee a    | */
/* |            l'attribut pointe par pAttr.                            | */
/* |            Ce changement de la presentation a lieu egalement sur   | */
/* |            le sous-arbre de l'element, si les parametres de        | */
/* |            presentation associes a l'attribut sont herites.        | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                ChngPresAttr (PtrElement pEl, PtrAttribute pAttr, PtrDocument pDoc, boolean Suppr, boolean herit, PtrAttribute pAttrComp)

#else  /* __STDC__ */
void                ChngPresAttr (pEl, pAttr, pDoc, Suppr, herit, pAttrComp)
PtrElement          pEl;
PtrAttribute         pAttr;
PtrDocument         pDoc;
boolean             Suppr;
boolean             herit;
PtrAttribute         pAttrComp;

#endif /* __STDC__ */

{
   PtrPRule        pR, pRVue1, pRSuiv, pRNA, pRSpecif, pRS, pRP;
   int                 vue, VueSch, val;
   PRuleType           TRegle;
   FunctionType           TFonct;
   PtrAbstractBox             pAbb, pReaff, pPR, pAbbNext, pFils;
   boolean             appl, stop, memetype, trouve;
   PtrPSchema          pSchP, pSPR;
   PtrAttribute         pAttrib;
   PtrElement          pEl1;
   boolean             vueexiste;
   boolean             complet;
   PtrHandlePSchema    pHd;
   TypeUnit            unit;

#ifdef __COLPAGE__
   boolean             TRep;
   PtrAbstractBox             pPE;

#endif /* __COLPAGE__ */

   pEl1 = pEl;
   TRegle = (PRuleType) 0;
   TFonct = (FunctionType) 0;
   /* on cherche d'abord dans les schemas de presentation additionnels les */
   /* plus prioritaires */
   pHd = pAttr->AeAttrSSchema->SsFirstPSchemaExtens;
   if (pHd == NULL)
      /* pas de schema additionnel. Prend le schema de presentation principal */
      pSchP = pAttr->AeAttrSSchema->SsPSchema;
   else
     {
	while (pHd->HdNextPSchema != NULL)
	   pHd = pHd->HdNextPSchema;
	pSchP = pHd->HdPSchema;
     }
   while (pSchP != NULL)
     {
	/* pR: premiere regle correspondant a l'attribut */
	pR = ReglePresAttr (pAttr, pEl1, herit, pAttrComp, pSchP);
	if (pR != NULL)
	  {
	     TRegle = pR->PrType;
	     /* type des regles courantes */
	     if (TRegle == PtFunction)
#ifdef __COLPAGE__
	       {
		  TRep = pR->PrPresBoxRepeat;
		  TFonct = pR->PrPresFunction;
	       }
#else  /* __COLPAGE__ */
		TFonct = pR->PrPresFunction;
#endif /* __COLPAGE__ */
	  }
	pRVue1 = NULL;
	/* regle de ce type pour la vue 1 */
	/* traite toutes les regles associees a cette valeur d'attribut dans ce */
	/* schema de presentation */
	while (pR != NULL)
	   /* chaque regle est traitee dans toutes les vues ou l'element */
	   /* a un pave */
	  {
	     if (pR->PrViewNum == 1)
		/* regle pour la vue 1 */
		pRVue1 = pR;	/* on la garde pour le cas ou on ne trouve pas mieux */
	     pRSuiv = pR->PrNextPRule;	/* regle suivante */
	     for (vue = 1; vue <= MAX_VIEW_DOC; vue++)
	       {
		  /* verifie d'abord si la vue existe */
		  if (VueAssoc (pEl))
		     /* element associe */
		     vueexiste = pDoc->DocAssocFrame[pEl->ElAssocNum - 1] != 0 && vue == 1;
		  else
		     vueexiste = pDoc->DocView[vue - 1].DvPSchemaView > 0;
		  if (vueexiste)
		    {
		       VueSch = VueAAppliquer (pEl, pAttr, pDoc, vue);
		       /* on ne considere les schemas de presentation additionnels que */
		       /* pour la vue principale */
		       vueexiste = (pHd == NULL || VueSch == 1);
		    }
		  if (vueexiste)
		     if (pR->PrCond != NULL)
			if (!CondPresentation (pR->PrCond, pEl, pAttr, VueSch,
					       pAttr->AeAttrSSchema))
			   /* les conditions d'application de la regle ne sont pas */
			   /* satisfaites, on n'applique pas la regle */
			   vueexiste = FALSE;
		  if (vueexiste)
		    {
		       /* si c'est une regle de creation on l'applique a toutes les */
		       /* vues */
		       if (pR->PrType != PtFunction
			   || !(pR->PrPresFunction == FnCreateBefore || pR->PrPresFunction == FnCreateFirst
#ifndef __COLPAGE__
				|| pR->PrPresFunction == FnCreateWith
#endif /* __COLPAGE__ */
				|| pR->PrPresFunction == FnCreateLast || pR->PrPresFunction == FnCreateAfter))
			  /* ce n'est pas une regle de creation ; on cherche la regle */
			  /* de meme type qui concerne cette vue */
			  if (pRSuiv != NULL)
			    {
			       memetype = FALSE;
			       if (pRSuiv->PrType == TRegle)
				  if (TRegle == PtFunction)
				    {
				       if (pR->PrPresFunction == pRSuiv->PrPresFunction)
					  memetype = TRUE;
				    }
				  else
				     memetype = TRUE;
			       if (memetype && pRSuiv->PrViewNum == VueSch)
				 {
				    pR = pRSuiv;
				    pRSuiv = pR->PrNextPRule;
				 }
			    }
		       pReaff = NULL;
		       /* il n'y a rien (encore) a reafficher */
		       pAbb = NULL;
		       /* on n'a pas cree de pave */
		       if (pEl1->ElAbstractBox[vue - 1] == NULL && TRegle == PtVisibility)
			  /* le pave n'existe pas et c'est une regle de visibilite */
			 {
			    val = valintregle (pR, pEl, vue, &appl, &unit, pAttr);
			    if ((!Suppr && val > 0) || (Suppr && val <= 0))
			       /* cette regle rend le pave visible et ce n'est pas une suppression
			          ou c'est une suppression et le pave etait rendu invisible par la regle
			          on cree le pave et ses paves de presentation eventuels */
			       pAbb = CreePaves (pEl, pDoc, vue, TRUE, TRUE, &complet);
			 }
		       if (pEl1->ElAbstractBox[vue - 1] != NULL && TRegle == PtFunction
			   && (TFonct == FnCreateBefore || TFonct == FnCreateAfter
#ifndef __COLPAGE__
			       || TFonct == FnCreateWith
#endif /* __COLPAGE__ */
			 || TFonct == FnCreateFirst || TFonct == FnCreateLast)
			   && !Suppr)
			  /* il faut creer un pave de presentation */
			  pAbb = CrPavPres (pEl, pDoc, pR, pAttr->AeAttrSSchema, pAttr, vue,
					  pSchP, FALSE, TRUE);
		       /* traite les paves crees par la regle de visibilite ou de */
		       /* creation */
		       /* code inutile et incorrect (fait dans CreePaves) si saut de page */
		       /* il reste correct si on a cree seulement des paves de presentation */
		       if (pAbb != NULL)
			 {
			    pAbbNext = pAbb;
			    stop = FALSE;
			    do
			       if (pAbbNext->AbNext == NULL)
				  stop = TRUE;
			       else if (pAbbNext->AbNext->AbElement != pEl)
				  stop = TRUE;
			       else
				  pAbbNext = pAbbNext->AbNext;
			    while (!(stop));
			    /* les nouveaux paves doivent etre pris en compte par */
			    /* leurs voisins */
			    NouvRfPave (pAbb, pAbbNext, &pReaff, pDoc);
			 }

		       /* traite les paves qui existaient deja */
		       /* il faut faire une boucle de parcours des paves dupliques de l'element */
		       if (pEl1->ElAbstractBox[vue - 1] != NULL && pAbb == NULL)
			 {
			    pAbb = pEl1->ElAbstractBox[vue - 1];
			    /* saute les paves de presentation de l'element */
			    while (pAbb->AbNext != NULL && pAbb->AbPresentationBox && pAbb->AbElement == pEl)
			       pAbb = pAbb->AbNext;
#ifdef __COLPAGE__
			    /* debut de la boucle */
			    do
			      {
#endif /* __COLPAGE__ */
				 appl = FALSE;
				 /* on n'a pas applique' la regle */
				 /* applique la regle au pave de l'element s'il n'est pas mort */
				 if (!pAbb->AbDead)
				    if (pR->PrType == TRegle && pR->PrViewNum == VueSch)
				       /* regle specifique de la vue traitee */
				       if (Suppr)
					 {
					    pRNA = LaRegle (pDoc, pAbb, &pSPR, TRegle, TRUE, &pAttrib);
					    appl = AppliqueRPres (pRNA, pSPR, pAbb, pDoc, pAttrib);
					 }
				       else
					  appl = AppliqueRPres (pR, pSchP, pAbb, pDoc, pAttr);
				    else
				       /* applique la regle de la vue 1 si elle existe */
				    if (pRVue1 != NULL)
				       if (Suppr)
					 {
					    pRNA = LaRegle (pDoc, pAbb, &pSPR, TRegle, TRUE, &pAttrib);
					    appl = AppliqueRPres (pRNA, pSPR, pAbb, pDoc, pAttrib);
					 }
				       else
					  appl = AppliqueRPres (pRVue1, pSchP, pAbb, pDoc, pAttr);
				    else
				       appl = FALSE;
				 if (appl)
				    /* on a change' la presentation du pave */
				   {
				      if (!Suppr)
					 /* ce n'est pas une suppression d'attribut */
					 /* cherche si l'element possede pour cette vue une */
					 /* regle de presentation specifique de meme type que */
					 /* celle qu'on vient d'appliquer */
					{
					   pRSpecif = pEl1->ElFirstPRule;
					   pRP = NULL;
					   stop = FALSE;
					   do
					      if (pRSpecif == NULL)
						 stop = TRUE;
					      else if (pRSpecif->PrViewNum == VueSch &&
						 pRSpecif->PrType == TRegle)
						 stop = TRUE;	/* trouve' */
					      else
						{
						   pRP = pRSpecif;
						   pRSpecif = pRSpecif->PrNextPRule;
						}
					   while (!(stop));
					   if (pRSpecif != NULL)
					      /* l'element possede cette regle specifique, on */
					      /* la supprime : l'attribut a priorite' sur la */
					      /* presentation specifique */
					     {
						pRS = pRSpecif->PrNextPRule;
						if (pRP == NULL)
						   /* c'etait la 1ere */
						   pEl1->ElFirstPRule = pRS;
						else
						   pRP->PrNextPRule = pRS;
						FreeReglePres (pRSpecif);
						/* libere la regle */
					     }
					}

				      if (pAbb->AbVisibility <= 0)
					 /* l'attribut rend le pave invisible */
					{
#ifdef __COLPAGE__
					   /* on se decale au premier pave de cette suite de pave */
					   /* sans passer par l'element (la suite est discontinue a */
					   /* cause des dupliques) */
					   while (pAbb->AbPrevious != NULL
						  && pAbb->AbPrevious->AbElement == pAbb->AbElement)
					      pAbb = pAbb->AbPrevious;
#else  /* __COLPAGE__ */
					   pAbb = pEl1->ElAbstractBox[vue - 1];
#endif /* __COLPAGE__ */
					   /* 1er pave a tuer */
					   pReaff = pAbb;
					   /* on reaffichera au moins ce pave */
					   do
					      /* on tue tous les paves de l'element */
					     {
						TuePave (pAbb);
						/* tue un pave */
						/* change les regles des autres paves qui se */
						/* referent au pave detruit */
						SuppRfPave (pAbb, &pPR, pDoc);
						pReaff = Englobant (pReaff, pPR);
						pAbb = pAbb->AbNext;
						if (pAbb == NULL)
						   stop = TRUE;
						else if (pAbb->AbElement != pEl)
						   stop = TRUE;
						else
						   stop = FALSE;
					     }
					   while (!(stop));
					}

				      else
					 /* le pave est toujours visible, mais a change' */
					{
					   pReaff = pAbb;
					   MarqueChng (pAbb, TRegle);
					   /* le parametre de presentation qui vient d'etre */
					   /* change' peut se transmettre au sous-arbre par */
					   /* heritage. On traite le sous-arbre. */
					   pFils = pAbb->AbFirstEnclosed;
					   while (pFils != NULL)
					     {
						HeritPres (pFils, TRegle, pDoc);
						pFils = pFils->AbNext;
					     }
					}
				   }
#ifdef __COLPAGE__
				 /* on passe au duplique suivant */
				 pAbb = pAbb->AbNextRepeated;
			      }
			    while (pAbb != NULL);		/* fin boucle des dupliques */
#endif /* __COLPAGE__ */
			 }

		       if (pEl1->ElAbstractBox[vue - 1] != NULL
			   && TRegle == PtFunction
			   && (TFonct == FnCreateBefore || TFonct == FnCreateAfter
#ifndef __COLPAGE__
			       || TFonct == FnCreateWith
#endif /* __COLPAGE__ */
			 || TFonct == FnCreateFirst || TFonct == FnCreateLast)
			   && Suppr)
			  /* on supprime un attribut qui portait une regle de */
			  /* creation ; il faut detruire le pave de presentation que */
			  /* cree la regle pR */
			  /* cherche d'abord le pave de l'element */
			 {
			    pAbb = pEl1->ElAbstractBox[vue - 1];
			    /* saute les paves de presentation de l'element */
			    while (pAbb->AbNext != NULL && pAbb->AbPresentationBox && pAbb->AbElement == pEl)
			       pAbb = pAbb->AbNext;
#ifdef __COLPAGE__
			    if (TFonct == FnCreateAfter || TFonct == FnCreateLast)
			       while (pAbb->AbNextRepeated != NULL)
				  pAbb = pAbb->AbNextRepeated;
			    pPE = pAbb;	/* pour paves repetes */
			    /* on boucle sur les paves de presentation repetes */
			    do
			      {
				 pAbb = pPE;
#endif /* __COLPAGE__ */
				 /* cherche le pave de presentation a detruire */
				 trouve = FALSE;
				 switch (TFonct)
				       {
					  case FnCreateBefore:
					     pAbb = pAbb->AbPrevious;
					     break;
					  case FnCreateAfter:
#ifndef __COLPAGE__
					  case FnCreateWith:
#endif /* __COLPAGE__ */
					     pAbb = pAbb->AbNext;
					     break;
					  case FnCreateFirst:
					     pAbb = pAbb->AbFirstEnclosed;
					     break;
					  case FnCreateLast:
					     /* cherche le dernier pave englobe' */
					     pAbb = pAbb->AbFirstEnclosed;
					     if (pAbb != NULL)
						while (pAbb->AbNext != NULL)
						   pAbb = pAbb->AbNext;
					     break;
					  default:
					     break;
				       }
				 if (pAbb != NULL)
				    do
				       /* pas de cas special pour les marques de page dans V4 */
#ifndef __COLPAGE__
				       if (pAbb->AbElement->ElTypeNumber == PageBreak + 1)
					  /* c'est un pave marque de page,passe au pave voisin */
					  if (TFonct == FnCreateBefore || TFonct == FnCreateLast)
					     pAbb = pAbb->AbPrevious;
					  else
					     pAbb = pAbb->AbNext;
				       else
#endif /* __COLPAGE__ */
					  /* ce n'est pas un pave' marque de saut de page */
				       if (!pAbb->AbPresentationBox || pAbb->AbElement != pEl)
					  /* ce n'est pas un pave de presentation ou ce pave */
					  /* n'appartient pas a l'element */
					  pAbb = NULL;
				       else
					  /* c'est un pave de presentation de l'element */
				       if (pAbb->AbTypeNum == pR->PrPresBox[0] && pAbb->AbPSchema == pSchP)
					  /* ce pave a la type cherche', on a trouve' */
					  trouve = TRUE;
				       else
					  /* passe au pave voisin */
				       if (TFonct == FnCreateBefore || TFonct == FnCreateLast)
					  pAbb = pAbb->AbPrevious;
				       else
					  pAbb = pAbb->AbNext;
				    while (!(pAbb == NULL || trouve));
				 if (trouve)
				    /* le pave a detruire est pointe' par pAbb */
				    if (!pAbb->AbDead)
				      {
					 pReaff = pAbb;
					 /* on reaffichera au moins ce pave */
					 TuePave (pAbb);
					 /* tue le pave */
					 /* change les regles des autres paves qui se referent */
					 /* au pave detruit */
					 SuppRfPave (pAbb, &pPR, pDoc);
					 pReaff = Englobant (pReaff, pPR);
				      }
#ifdef __COLPAGE__
				 /* on passe au pave duplique suivant pour trouver le pave */
				 /* de presentation repete */
				 if (!TRep)
				    pPE = NULL;
				 else if (TFonct == FnCreateBefore || TFonct == FnCreateFirst)
				    pPE = pPE->AbNextRepeated;
				 else
				    pPE = pPE->AbPreviousRepeated;
			      }
			    while (pPE != NULL);
#endif /* __COLPAGE__ */
			 }
		       /* conserve le pointeur sur le pave a reafficher */
		       if (pReaff != NULL)
			 {
			    if (VueAssoc (pEl))
			       pDoc->DocAssocModifiedAb[pEl1->ElAssocNum - 1] =
				  Englobant (pReaff, pDoc->DocAssocModifiedAb[pEl1->ElAssocNum - 1]);
			    else
			       pDoc->DocViewModifiedAb[vue - 1] =
				  Englobant (pReaff, pDoc->DocViewModifiedAb[vue - 1]);
			    /* fin de la boucle sur les vues */
			 }
		    }
	       }

	     /* passe a la regle de presentation suivante pour l'attribut */
	     pR = pRSuiv;
	     if (pR != NULL)
		if (pR->PrType != TRegle ||
		    (TRegle == PtFunction && pR->PrPresFunction != TFonct))
		   /* on change de type de regle */
		  {
		     TRegle = pR->PrType;
		     if (TRegle == PtFunction)
			TFonct = pR->PrPresFunction;
		     pRVue1 = NULL;
		  }
	  }
	/* on traite les schemas de presentation de moindre priorite' */
	if (pHd == NULL)
	   /* on cherchait dans le schema de presentation principal. */
	   /* c'est fini */
	   pSchP = NULL;
	else
	  {
	     /* on prend le schema de presentation additionnel de priorite' */
	     /* inferieure */
	     pHd = pHd->HdPrevPSchema;
	     if (pHd == NULL)
		/* plus de schemas additionnels, on prend le schema princ */
		pSchP = pAttr->AeAttrSSchema->SsPSchema;
	     else
		pSchP = pHd->HdPSchema;
	  }
     }
}

 /* procedure deplacee pour la pagination dans le print */
 /* elle etait avant dans modif.c */
 /* procedure appelee par les procedures de modif.c et par page.c */
/* ---------------------------------------------------------------------- */
/* |    MemeTexte       retourne 'vrai' si l'element pointe par pEl     | */
/* |    (appartenant au document pointe par pDoc) a un frere suivant et | */
/* |    si les deux elements sont des feuilles de texte dans le meme    | */
/* |    alphabet, avec les memes attributs et les memes regles de       | */
/* |    presentation specifiques.                                       | */
/* |    Dans ce cas, les deux elements sont fusionnes et l'image        | */
/* |    abstraite est mise a jour. pLib contient au retour un pointeur  | */
/* |    sur l'element libere par la fusion (cet element n'est pas rendu | */
/* |    a la memoire libre).                                            | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
boolean             MemeTexte (PtrElement pEl, PtrDocument pDoc, PtrElement * pLib)

#else  /* __STDC__ */
boolean             MemeTexte (pEl, pDoc, pLib)
PtrElement          pEl;
PtrDocument         pDoc;
PtrElement         *pLib;

#endif /* __STDC__ */

{
   boolean             egal, stop;
   PtrElement          pEl2, pEVoisin;
   int                 vue, dvol;
   PtrAbstractBox             pAb;
   PtrElement          pEl1;
   PtrAbstractBox             pAbbox1;

   egal = FALSE;
   if (pEl != NULL)
     {
	pEl1 = pEl;
	pEl2 = pEl1->ElNext;
	if (pEl2 != NULL && pEl1->ElLeafType == LtText && pEl1->ElTerminal)
	   if (pEl2->ElTerminal && pEl2->ElLeafType == LtText)
	      if (pEl2->ElLanguage == pEl1->ElLanguage)
		 if (!pEl2->ElHolophrast && !pEl1->ElHolophrast)
		    if (MemesAttributs (pEl, pEl2))
		       if (pEl->ElSource == NULL && pEl2->ElSource == NULL)
			  if (MemesRegleSpecif (pEl, pEl2))
			     if (pEl1->ElSructSchema->SsRule[pEl1->ElTypeNumber - 1].SrConstruct !=
				 CsConstant)
				if (pEl2->ElSructSchema->SsRule[pEl2->ElTypeNumber - 1].SrConstruct
				    != CsConstant)
				  {
				     egal = TRUE;
				     /* detruit les paves du 2eme element */
				     DetrPaves (pEl1->ElNext, pDoc, FALSE);
				     /* fusionne les deux elements de texte */
				     MergeTextElements (pEl, pLib, pDoc, TRUE);
				     /* teste si pEl est le dernier fils de son pere, */
				     /* abstraction faite des marques de page */
				     pEVoisin = pEl->ElNext;
				     stop = FALSE;
				     do
					if (pEVoisin == NULL)
					   /* pEl devient le dernier fils de son pere */
					  {
					     PlusPremDer (pEl, pDoc, FALSE, FALSE);
					     stop = TRUE;
					  }
					else if (!pEVoisin->ElTerminal
					|| pEVoisin->ElLeafType != LtPageColBreak)
					   stop = TRUE;
					else
					   pEVoisin = pEVoisin->ElNext;
				     while (!(stop));
				     /* met a jour le volume des paves correspondants */
				     for (vue = 1; vue <= MAX_VIEW_DOC; vue++)
				       {
					  pAb = pEl1->ElAbstractBox[vue - 1];
					  if (pAb != NULL)
					    {
					       pAb->AbChange = TRUE;
					       if (!VueAssoc (pEl))
						  pDoc->DocViewModifiedAb[vue - 1] =
						     Englobant (pAb, pDoc->DocViewModifiedAb[vue - 1]);
					       else
						 {
						    pEl2 = pEl;
						    pDoc->DocAssocModifiedAb[pEl2->ElAssocNum - 1] =
						       Englobant (pAb, pDoc->DocAssocModifiedAb[pEl2->ElAssocNum - 1]);
						 }
					       dvol = pEl1->ElTextLength - pAb->AbVolume;

					       if (pAb->AbDead && pAb->AbNext != NULL)
						  pAb->AbNext->AbVolume += dvol;

					       do
						 {
						    pAbbox1 = pAb;
						    pAbbox1->AbVolume += dvol;
						    pAb = pAbbox1->AbEnclosing;
						 }
					       while (!(pAb == NULL));
					    }
				       }
				  }
     }
   return egal;
}
