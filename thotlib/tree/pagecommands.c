/* printpage.c : module de traitement des regles page
   ce module contient les procedures utilisees par le
   paginateur et l'imprimeur (extraites et modifiees
   de page.c et print.c)
 */

/*      C. Roisin       Septembre 1995 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"

#undef EXPORT
#define EXPORT extern
#include "page.var"


#include "buildboxes_f.h"
#include "absboxes_f.h"
#include "changeabsbox_f.h"
#include "presrules_f.h"
#include "boxpositions_f.h"
#include "pagecommands_f.h"


/* ---------------------------------------------------------------------- */
/* |    ReglePage cherche la regle page associee aux elements du        | */
/* |            type de l'element pointe' par pEl dans la vue Vue       | */
/* |            du schema de presentation ou elle est definie           | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
PtrPRule        ReglePage (PtrElement pEl, int Vue, PtrPSchema * pSchP)

#else  /* __STDC__ */
PtrPRule        ReglePage (pEl, Vue, pSchP)
PtrElement          pEl;
int                 Vue;
PtrPSchema         *pSchP;

#endif /* __STDC__ */

{
   PtrPRule        pRegle, pRPage;
   PtrSSchema        pSchS;
   int                 Entree;
   boolean             stop;

   pRPage = NULL;
   ChSchemaPres (pEl, pSchP, &Entree, &pSchS);
   pRegle = (*pSchP)->PsElemPRule[Entree - 1];
   /* 1ere regle de pres. du type */
   stop = FALSE;
   do
      if (pRegle == NULL)
	 stop = TRUE;
      else if (pRegle->PrType > PtFunction)
	 stop = TRUE;
      else if (pRegle->PrType == PtFunction &&
	 /**TODO*//* valeur de Vue si vue d'element associes ?? */
	       pRegle->PrViewNum == Vue &&
	       pRegle->PrPresFunction == FnPage)
	{			/* c'est une regle Page */
	   pRPage = pRegle;
	   stop = TRUE;
	}
      else
	 pRegle = pRegle->PrNextPRule;
   while (!(stop));
   return pRPage;
}

/* ---------------------------------------------------------------------- */
/* |RegleType_HautPage  cherche parmi les regles de presentation de la  | */
/* |                   page, la regle du type TypeR  et retourne un     | */
/* |                   pointeur sur la regle trouvee ou NULL si pas     | */
/* |                   trouvee.                                         | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
PtrPRule        RegleType_HautPage (PRuleType TypeR, int b, PtrPSchema pSchP)

#else  /* __STDC__ */
PtrPRule        RegleType_HautPage (TypeR, b, pSchP)
PRuleType           TypeR;
int                 b;
PtrPSchema          pSchP;

#endif /* __STDC__ */

{
   boolean             stop, existe;
   PtrPRule        pR;

   pR = pSchP->PsPresentBox[b - 1].PbFirstPRule;
   stop = FALSE;
   existe = FALSE;
   do
     {
	if (pR == NULL)
	   stop = TRUE;
	else if (pR->PrType > TypeR)
	   stop = TRUE;
	else if (pR->PrType == TypeR)
	  {
	     stop = TRUE;
	     existe = TRUE;
	  }
	if (!stop)
	   pR = pR->PrNextPRule;
     }
   while (!(stop));
   if (existe)
      return pR;
   else
      return NULL;
}

#ifdef __COLPAGE__
/* ---------------------------------------------------------------------- */
/* | ReglePageDebut     recherche la regle page qui a cree la Marque de | */
/* |                    Page Debut pElPage.                             | */
/* |                    La recherche parcourt les elements qui sont     | */
/* |                    apres pElPage dans l'arbre abstrait.            | */
/* |                    retourne dans pSchP le schema de presentation   | */
/* |                    procedure utilisee dans print et page           | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
PtrPRule        ReglePageDebut (PtrElement pElPage, PtrPSchema * pSchP)

#else  /* __STDC__ */
PtrPRule        ReglePageDebut (pElPage, pSchP)
PtrElement          pElPage;
PtrPSchema         *pSchP;

#endif /* __STDC__ */

{
   PtrPRule        pR;
   PtrElement          pSuiv;

   pR = NULL;

   if (pElPage->ElPageType == PgBegin)
     {
	/* Nouveau type de page, on determine la hauteur des pages */
	/* il faut rechercher la regle sur l'element suivant */
	/* car les elements marque page debut sont places AVANT les elements */
	/* qui portent la regle page */
	if (pElPage->ElNext != NULL)
	  {
	     pSuiv = pElPage->ElNext;
	     /* on saute les eventuelles marques de colonnes */
	     /* ou de page (pour d'autres vues par exemple ?) */
	     while (pSuiv != NULL
		    && pSuiv->ElTypeNumber == PageBreak + 1)
		pSuiv = pSuiv->ElNext;
	     /* on cherche uniquement sur pPsuiv car normalement l'element */
	     /* marque page debut a ete place juste devant l'element qui */
	     /* portait la regle page correspondante */
	     if (pSuiv != NULL)
		pR = ReglePage (pSuiv, pElPage->ElViewPSchema, pSchP);
	  }
	if (pR == NULL && pElPage->ElParent != NULL)
	   /* si pSuiv ne portait pas de regle, */
	   /* l'element englobant porte-t-il une regle page ? */
	   pR = ReglePage (pElPage->ElParent,
			   pElPage->ElViewPSchema, pSchP);
     }
   return pR;
}

#endif /* __COLPAGE */


#ifndef __COLPAGE__

/* ---------------------------------------------------------------------- */
 /* |   HautPage met a jour les variables HauteurPage et HauteurBasPage | */
/* |            selon le type de page auquel appartient l'element       | */
/* |            Marque Page pointe par pElPage.                         | */
/* |            Vue indique le numero de la vue dans le schema de       | */
/* |            presentation pour laquelle on construit des pages.      | */
/* |            (c'est toujours 1 pour les vues d'elements associes).   | */
/* |            Procedure utilisee pour la pagination et l'impression   | */
/* |            les parametres b et pSchP sont utilises pour le print   | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                HautPage (PtrElement pElPage, int Vue, int *b, PtrPSchema * pSchP)

#else  /* __STDC__ */
void                HautPage (pElPage, Vue, b, pSchP)
PtrElement          pElPage;
int                 Vue;
int                *b;
PtrPSchema         *pSchP;

#endif /* __STDC__ */

{
   PtrPRule        pR;
   PtrPSchema          pSP;

   *b = 0;
   *pSchP = NULL;
   if (pElPage->ElPageType == PgBegin)
     {
	/* Nouveau type de page, on determine la hauteur des pages */
	pR = NULL;
	if (pElPage->ElParent != NULL)
	   /* l'element englobant porte-t-il une regle page ? */
	   pR = ReglePage (pElPage->ElParent, Vue, &pSP);
	if (pR != NULL)		/* on a trouve la regle page */
	  {
	     *b = pR->PrPresBox[0];	/* parametre retour */
	     /* Hauteur minimum du bas de page */
	     HauteurBasPage = pSP->PsPresentBox[(*b) - 1].PbFooterHeight;
	     /* cherche la regle de hauteur de la boite page */
	     pR = RegleType_HautPage (PtHeight, *b, pSP);
	     if (pR != NULL)
		if (!pR->PrDimRule.DrPosition)
		   HauteurPage = pR->PrDimRule.DrValue - HauteurBasPage;
	     *pSchP = pSP;	/* parametre retour */
	  }
     }
}

#endif /* __COLPAGE__ */


#ifdef __COLPAGE__
 /* V4 : procedure DansPage supprimee */
 /* V4 : procedure TuePavesAuDessus supprimee */
#else  /* __COLPAGE__ */

/* ---------------------------------------------------------------------- */
/* |    DansPage marque tous les paves du sous arbre de racine pAb     | */
/* |            comme faisant partie de la page courante                | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                DansPage (PtrAbstractBox pAb)

#else  /* __STDC__ */
void                DansPage (pAb)
PtrAbstractBox             pAb;

#endif /* __STDC__ */

{
   PtrAbstractBox             pPa1;


   pPa1 = pAb;
   pPa1->AbAfterPageBreak = FALSE;
   pPa1->AbOnPageBreak = FALSE;
   pPa1 = pPa1->AbFirstEnclosed;
   while (pPa1 != NULL)
     {
	DansPage (pPa1);
	pPa1 = pPa1->AbNext;
     }
}

/* ---------------------------------------------------------------------- */
/* |    TuePavesAuDessus tue tous les paves contenus dans le            | */
/* |            pave pointe' par pP et qui sont au-dessus de la limite. | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TuePavesAuDessus (PtrAbstractBox pP, int limite, int VueNb, PtrDocument pDoc, PtrAbstractBox * PavReaff)

#else  /* __STDC__ */
void                TuePavesAuDessus (pP, limite, VueNb, pDoc, PavReaff)
PtrAbstractBox             pP;
int                 limite;
int                 VueNb;
PtrDocument         pDoc;
PtrAbstractBox            *PavReaff;

#endif /* __STDC__ */

{
   int                 haut, y, NCar;


   pP = pP->AbFirstEnclosed;
   while (pP != NULL)
     {
	/* on ignore les paves deja morts */
	if (!pP->AbDead)
	   if (pP->AbElement->ElTypeNumber != PageBreak + 1)
	      /* demande au Mediateur la position et la hauteur du pave */
	     {
		HautCoupure (pP, TRUE, &haut, &y, &NCar);
		if (y < limite)
		   /* le haut du pave est au-dessus de la limite */
		   /* on ne tue pas les paves qui contiennent une marque de page */
		   if (y + haut <= limite && !pP->AbOnPageBreak)
		      if (pP->AbPresentationBox)
			 /* Tue les paves de presentation */
			{
			   TuePave (pP);
			   SuppRfPave (pP, PavReaff, pDoc);
			}
		      else
			 DetPavVue (pP->AbElement, pDoc, FALSE, VueNb);
		   else
		      /* le pave est traverse par la limite */
		      TuePavesAuDessus (pP, limite, VueNb, pDoc, PavReaff);
	     }
	pP = pP->AbNext;
     }
}
#endif /* __COLPAGE__ */


#ifdef __COLPAGE__
 /* V4 : la procedure TueAvantPage est completement changee */
/* ---------------------------------------------------------------------- */
/* |    TueAvantPage detruit tous les paves qui precedent la page pPage | */
 /*|      on detruit les paves haut, corps, bas de page et filet */
 /*|      des pages precedentes */
 /*|      cette procedure est plus simple qu'avant */
/* |            Retourne vrai si l'image restante est plus petite       | */
/* |            qu'une page.                                            | */
       /*CP *//* |     On garde la page courante (pour savoir ou continuer.    | */
       /*CP *//* |     Destruction page par page (y compris le filet)          | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
boolean             TueAvantPage (PtrAbstractBox pPage, int frame, PtrDocument pDoc, int VueNb)

#else  /* __STDC__ */
boolean             TueAvantPage (pPage, frame, pDoc, VueNb)
PtrAbstractBox             pPage;
int                 frame;
PtrDocument         pDoc;
int                 VueNb;

#endif /* __STDC__ */

{
   PtrAbstractBox             pAb, PavR, PavRac;
   boolean             ret, det;

   ret = TRUE;			/* initialisation de ret, est-ce bon ? */

   /* on detruit les paves haut, corps, bas de page et filet */
   /* des pages precedentes */
   /* cette procedure est plus simple qu'avant */

   /* Test pour verifier que les paves sont bien des marques */
   /* de page et non de colonnes */
   if (pPage->AbElement->ElTypeNumber == PageBreak + 1
       && (pPage->AbElement->ElPageType == PgBegin
	   || pPage->AbElement->ElPageType == PgComputed
	   || pPage->AbElement->ElPageType == PgUser))
     {
	pAb = pPage->AbElement->ElAbstractBox[VueNb - 1];
	/* premier pave de la page */
	PavRac = pAb->AbEnclosing;	/* racine */
	if (PavRac == NULL || PavRac->AbEnclosing != NULL)
	   /* erreur image abstraite */
	   AffPaveDebug (pPage);
	/* on detruit les paves precedents */
	/* mais on laisse le pave racine non coupe en tete pour */
	/* que les paves detruits ne soient pas recrees dans l'appel */
	/* de AfFinFenetre (pour le print) */
	pAb = pAb->AbPrevious;
	det = FALSE;		/* a priori pas de paves detruits */
	while (pAb != NULL)
	  {
	     TuePave (pAb);
	     SuppRfPave (pAb, &PavReaff, pDoc);
	     pAb = pAb->AbPrevious;
	     det = TRUE;	/* des paves ont ete detruits */
	  }
	/* signale les paves morts au Mediateur */
	/* si pas de destruction, on appelle ModifVue pour positionner ret */
	/* et refait evaluer la coupure de page */
	Hauteurffective = HauteurCoupPage;
	ChangeRHPage (PavRac, pDoc, VueNb);
	ret = ModifVue (frame, &Hauteurffective, PavRac);
	if (det)
	   /* libere tous les paves morts de la vue */
	   LibPavMort (PavRac);
	/* cherche le rappel suivant de ce saut de page */
	/* supprime */
     }				/* fin cas ou pPage est bien une marque de page */
   return ret;
}

#else  /* __COLPAGE__ */

/* ---------------------------------------------------------------------- */
/* |    TueAvantPage detruit tous les paves qui precedent le filet      | */
/* |            marquant la frontiere de page qui est a l'interieur du  | */
/* |            pave Marque de Page pointe' par pPage.                  | */
/* |            Retourne vrai si l'image restante est plus petite       | */
/* |            qu'une page.                                            | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
boolean             TueAvantPage (PtrAbstractBox pPage, int frame, PtrDocument pDoc, int VueNb)

#else  /* __STDC__ */
boolean             TueAvantPage (pPage, frame, pDoc, VueNb)
PtrAbstractBox             pPage;
int                 frame;
PtrDocument         pDoc;
int                 VueNb;

#endif /* __STDC__ */

{
   PtrAbstractBox             pAb, PavReaff, PavRac, pSuiv;
   boolean             stop, ret;
   int                 h, yhaut, NbCar, yfilet;
   PtrAbstractBox             pPa1;

   /* cherche d'abord le pave racine de la vue */
   PavRac = pPage;
   while (PavRac->AbEnclosing != NULL)
      PavRac = PavRac->AbEnclosing;
   /* marque tous les paves comme faisant partie de la page */
   DansPage (PavRac);
   /* detruit, dans le pave Marque Page, les boites de bas de page qui */
   /* precedent le filet marquant le saut de page. */
   pAb = pPage->AbFirstEnclosed;
   stop = FALSE;
   do
      if (pAb == NULL)
	 stop = TRUE;
      else if (!pAb->AbPresentationBox)
	 /* Note: le filet n'est pas un pave de presentation, alors que */
	 /* toutes les autres boites de bas de page sont des paves de */
	 /* presentation */
	{
	   stop = TRUE;
	   /* demande au Mediateur la position verticale du filet */
	   HautCoupure (pAb, TRUE, &h, &yfilet, &NbCar);
	}
      else
	{
	   TuePave (pAb);
	   pAb = pAb->AbNext;
	}
   while (!(stop));
   /* detruit tous les paves qui precedent le pave Marque Page et ses */
   /* paves englobants, mais pas ceux qui contiennent un saut de page */
   pAb = pPage;
   while (pAb != NULL)
     {
	while (pAb->AbPrevious != NULL)
	  {
	     pAb = pAb->AbPrevious;
	     pPa1 = pAb;
	     if (!pPa1->AbOnPageBreak)
		if (pPa1->AbPresentationBox)
		   /* Tue les paves de presentation */
		  {
		     TuePave (pAb);
		     SuppRfPave (pAb, &PavReaff, pDoc);
		  }
		else
		   DetPavVue (pPa1->AbElement, pDoc, FALSE, VueNb);
	  }
	pAb = pAb->AbEnclosing;
	/* marque les paves englobant la marque de page */
	if (pAb != NULL)
	   pAb->AbOnPageBreak = TRUE;
     }
   /* Verifie les paves suivant la marque de page aux niveaux superieurs */
   pAb = pPage;
   while (pAb != NULL)
     {
	pSuiv = pAb->AbNext;
	while (pSuiv != NULL)
	  {
	     if (!pSuiv->AbDead)
	       {
		  /* demande au Mediateur la position et la hauteur du pave */
		  HautCoupure (pSuiv, TRUE, &h, &yhaut, &NbCar);
		  if (yhaut < yfilet)
		     /* le haut du pave est au-dessus du saut de page */
		     if (yhaut + h <= yfilet && !pSuiv->AbOnPageBreak)
			if (pSuiv->AbPresentationBox)
			   /* Tue les paves de presentation */
			  {
			     TuePave (pSuiv);
			     SuppRfPave (pSuiv, &PavReaff, pDoc);
			  }
			else
			   DetPavVue (pSuiv->AbElement, pDoc, FALSE, VueNb);
		     else
			/* le pave est traverse par le saut de page */
			TuePavesAuDessus (pSuiv, yfilet, VueNb, pDoc, &PavReaff);
	       }
	     pSuiv = pSuiv->AbNext;
	  }
	pAb = pAb->AbEnclosing;
     }
   /* signale les paves morts au Mediateur */
   Hauteurffective = HauteurPage;
   ret = ModifVue (frame, &Hauteurffective, PavRac);
   /* libere tous les paves morts de la vue */
   LibPavMort (PavRac);
   return ret;
}
#endif /* __COLPAGE__ */
