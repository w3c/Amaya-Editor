/*
   crpages : procedures liees a la creation des pages et des colonnes
   appelees par crimabs pour la creation des images abstraites
   contient quelques procedures du module crimabs

   C. Roisin
 */


#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"

#include "message.h"
#include "language.h"

#undef EXPORT
#define EXPORT extern
#include "page.var"
#include "environ.var"
#include "edit.var"

#include "arbabs.f"
#include "attribut.f"
#include "attrherit.f"
#include "crimabs.f"
#include "crpages.f"
#include "debug.f"
#include "thotmsg.f"
#include "imabs.f"
#include "imagepres.f"
#include "img.f"
#include "memory.f"
#include "modif.f"
#include "modimabs.f"
#include "pres.f"
#include "pos.f"
#include "schemas.f"
#include "varpres.f"


#ifdef __COLPAGE__
/* ---------------------------------------------------------------------- */
/* |    CopiePav retourne un pave qui est la duplication du pave pAb ;   */
/* |         tous les champs sont recopies sauf ceux de chainage,         */
/* |         de position, de dimension et de modification qui sont        */
/* |         initialises par initpave.                                    */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static PtrAbstractBox      CopiePav (PtrAbstractBox pAb)
#else  /* __STDC__ */
static PtrAbstractBox      CopiePav (pAb)
PtrAbstractBox             pAb;
#endif /* __STDC__ */
{

   PtrAbstractBox             NouvPave;
   PtrElement          pEl;
   DocViewNumber           VueNb;
   int                 Vis;

   pEl = pAb->AbElement;
   VueNb = pAb->AbDocView;
   Vis = pAb->AbVisibility;
   NouvPave = initpave (pEl, VueNb, Vis);
   /* les modifications des champs position et dimension seront
      faites lorsque le chainage sera complet */
   NouvPave->AbPSchema = pAb->AbPSchema;
   NouvPave->AbCreatorAttr = pAb->AbCreatorAttr;
   NouvPave->AbCopyDescr = pAb->AbCopyDescr;
   NouvPave->AbUnderline = pAb->AbUnderline;
   NouvPave->AbThickness = pAb->AbThickness;
   NouvPave->AbIndent = pAb->AbIndent;
   NouvPave->AbDepth = pAb->AbDepth;
   NouvPave->AbTypeNum = pAb->AbTypeNum;
   NouvPave->AbFont = pAb->AbFont;
   NouvPave->AbHighlight = pAb->AbHighlight;
   NouvPave->AbSize = pAb->AbSize;
   NouvPave->AbSizeUnit = pAb->AbSizeUnit;
   NouvPave->AbHorizEnclosing = pAb->AbHorizEnclosing;
   NouvPave->AbVertEnclosing = pAb->AbVertEnclosing;
   NouvPave->AbCanBeModified = pAb->AbCanBeModified;
   NouvPave->AbSelected = pAb->AbSelected;
   NouvPave->AbPresentationBox = pAb->AbPresentationBox;
   NouvPave->AbLineSpacing = pAb->AbLineSpacing;
   NouvPave->AbAdjust = pAb->AbAdjust;
   NouvPave->AbJustify = pAb->AbJustify;
   NouvPave->AbLineSpacingUnit = pAb->AbLineSpacingUnit;
   NouvPave->AbIndentUnit = pAb->AbIndentUnit;
   NouvPave->AbAcceptLineBreak = pAb->AbAcceptLineBreak;
   NouvPave->AbAcceptPageBreak = pAb->AbAcceptPageBreak;
   NouvPave->AbSensitive = pAb->AbSensitive;
   NouvPave->AbFillPattern = pAb->AbFillPattern;
   NouvPave->AbBackground = pAb->AbBackground;
   NouvPave->AbForeground = pAb->AbForeground;
   NouvPave->AbLineStyle = pAb->AbLineStyle;
   NouvPave->AbLineWeight = pAb->AbLineWeight;
   NouvPave->AbLineWeightUnit = pAb->AbLineWeightUnit;

   NouvPave->AbLeafType = pAb->AbLeafType;
   switch (NouvPave->AbLeafType)
	 {
	    case LtCompound:
	       NouvPave->AbInLine = pAb->AbInLine;
	       NouvPave->AbTruncatedHead = pAb->AbTruncatedHead;
	       NouvPave->AbTruncatedTail = pAb->AbTruncatedTail;
	       break;
	    case LtText:
	       NouvPave->AbText = pAb->AbText;
	       NouvPave->AbLanguage = pAb->AbLanguage;
	       break;
	    case LtSymbol:
	    case LtGraphics:
	       NouvPave->AbShape = pAb->AbShape;
	       NouvPave->AbGraphAlphabet = pAb->AbGraphAlphabet;
	       NouvPave->AbRealShape = pAb->AbRealShape;
	       break;
	    case LtPicture:
	       NouvPave->AbImageDescriptor = pAb->AbImageDescriptor;
	       break;
	    case LtPairedElem:
	    case LtPlyLine:
	    case LtReference:
	    case LtPageColBreak:
	       break;
	 }

   return NouvPave;
}
#endif /* __COLPAGE__ */

#ifdef __COLPAGE__
 /* nouvelle procedure du module crimabs, utilisee dans CreePaves et Chaine */
/* ---------------------------------------------------------------------- */
/* |    RechPavPage retourne le premier pave corps de page qui          | */
/* |            precede (si EnAvant) ou suit (si non EnAvant)           | */
/* |            le pave corps de page cree pour pEl.                    | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
PtrAbstractBox             RechPavPage (PtrElement pEl, DocViewNumber VueNb, int VueSch,
				 boolean EnAvant)
#else  /* __STDC__ */
PtrAbstractBox             RechPavPage (pEl, VueNb, VueSch, EnAvant)
PtrElement          pEl;
DocViewNumber           VueNb;
int                 VueSch;
boolean             EnAvant;

#endif /* __STDC__ */
{

   PtrElement          pPage, pE;
   PtrAbstractBox             Retour, pAb;
   boolean             fini;

   pE = pEl;
   fini = False;
   Retour = NULL;
   while (!fini)
     {
	if (EnAvant)
	   /* on recherche dans l'arbre abstrait la marque de page precedente */
	   pPage = BackSearchTypedElem (pE, PageBreak + 1, NULL);
	else
	   /* on recherche dans l'arbre abstrait la marque de page suivante */
	   pPage = FwdSearchTypedElem (pE, PageBreak + 1, NULL);
	if (pPage == NULL)
	   /* c'est fini, on n'a pas trouve */
	   fini = True;
	else if (pPage->ElViewPSchema == VueSch
		 && (pPage->ElPageType == PgBegin
		     || pPage->ElPageType == PgComputed
		     || pPage->ElPageType == PgUser))
	   /* on a trouve une marque de page precedente dans la vue */
	   /* il faut chercher parmi ses paves celui qui est un corps de page */
	   /* et non un pave filet, haut et bas */
	  {
	     pAb = pPage->ElAbstractBox[VueNb - 1];
	     if (pAb != NULL)
	       {
		  while (pAb != NULL && !fini && pAb->AbElement == pPage)
		     if (pAb->AbLeafType == LtCompound && !pAb->AbPresentationBox)
		       {
			  fini = True;
			  Retour = pAb;
		       }
		     else
			/* c'est un pave filet, haut ou bas */
			/* il faut continuer la recherche */
			pAb = pAb->AbNext;
		  if (fini == False)
		    {
		       printf ("ERR-CP : RechPavPage ");
		       fini = True;
		    }
	       }
	     else
		fini = True;
	  }
	else
	   /* ce n'est pas une marque page pour la vue, on continue */
	   pE = pPage;
     }
   return Retour;

}
#endif /* __COLPAGE__ */

#ifdef __COLPAGE__

 /* nouvelle procedure du module crimabs, utilisee dans CreePaves  */
 /* et Applique                                                    */
/* ---------------------------------------------------------------------- */
/* |    RechPavPageCol retourne le premier pave corps de page ou colonne| */
/* |            qui precede (si EnAvant) ou suit (si non EnAvant)       | */
/* |            le pave corps de page cree pour pEl                     | */
/* |            Si il y a des colonnes sans pave, on les saute          | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static PtrAbstractBox      RechPavPageCol (PtrElement pEl, DocViewNumber VueNb, int VueSch,
				    boolean EnAvant)

#else  /* __STDC__ */
static PtrAbstractBox      RechPavPageCol (pEl, VueNb, VueSch, EnAvant)
PtrElement          pEl;
DocViewNumber           VueNb;
int                 VueSch;
boolean             EnAvant;

#endif /* __STDC__ */

{

   PtrElement          pPage, pE;
   PtrAbstractBox             Retour, pAb;
   boolean             fini;

   pE = pEl;
   fini = False;
   Retour = NULL;
   while (!fini)
     {
	if (EnAvant)
	   /* on recherche dans l'arbre abstrait la marque de page precedente */
	   pPage = BackSearchTypedElem (pE, PageBreak + 1, NULL);
	else
	   /* on recherche dans l'arbre abstrait la marque de page suivante */
	   pPage = FwdSearchTypedElem (pE, PageBreak + 1, NULL);
	if (pPage == NULL)
	   /* c'est fini, on n'a pas trouve */
	   fini = True;
	else if (pPage->ElViewPSchema == VueSch)
	   /* on a trouve une marque de page precedente dans la vue */
	   /* il faut chercher parmi ses paves celui qui est un corps de page */
	   /* et non un pave filet, haut et bas */
	  {
	     pAb = pPage->ElAbstractBox[VueNb - 1];
	     if (pAb != NULL)
	       {
		  while (pAb != NULL && !fini && pAb->AbElement == pPage)
		     if (pAb->AbLeafType == LtCompound && !pAb->AbPresentationBox)
		       {
			  fini = True;
			  Retour = pAb;
		       }
		     else
			/* c'est un pave filet, haut ou bas */
			/* il faut continuer la recherche */
			pAb = pAb->AbNext;
		  if (fini == False)
		    {
		       printf ("ERR-CP : RechPavPage ");
		       fini = True;
		    }
	       }
	     else if (pPage->ElPageType == ColBegin
		      || pPage->ElPageType == ColComputed
		      || pPage->ElPageType == ColUser
		      || pPage->ElPageType == ColGroup)
		/* on continue la recherche pour trouver un pave */
		pE = pPage;
	     else
		fini = True;
	  }
	else
	   /* ce n'est pas une marque page pour la vue, on continue */
	   pE = pPage;
     }
   return Retour;

}
#endif /* __COLPAGE__ */


/* ---------------------------------------------------------------------- */
/* |    TypeBPage cherche le type de boite page qui correspond a`       | */
/* |            l'element Marque Page pointe par pEl pour la vue VueNb. | */
 /* |    Rq : VueNb est un numero de vue de schema de pres       | */
/* |            On cherche les regles Page des elements englobants      | */
/* |            l'element Marque Page. Retourne le numero de boite      | */
/* |            decrivant la page et le schema de presentation ou`      | */
/* |            cette boite est definie.                                | */
 /*      Attention : maintenant on place les marques page AVANT  | */
 /*      l'element qui contient la regle page (sauf racine)      | */
 /*      la recherche de la regle est donc changee               | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
int                 TypeBPage (PtrElement pEl, int VueNb, PtrPSchema * pSchPPage)

#else  /* __STDC__ */
int                 TypeBPage (pEl, VueNb, pSchPPage)
PtrElement          pEl;
int                 VueNb;
PtrPSchema         *pSchPPage;

#endif /* __STDC__ */

{
   PtrElement          pPere;
   int                 TypeP, Entree;
   PtrPRule        pRegle;
   PtrSSchema        pSchS;
   boolean             stop;
   PtrPRule        pRe1;

#ifdef __COLPAGE__
   PtrElement          pSuiv;

#endif /* __COLPAGE__ */
   TypeP = 0;
   if (pEl != NULL)
     {
	pPere = pEl->ElParent;
	if (pPere != NULL)
	  {
#ifdef __COLPAGE__
	     /* si la marque page est une page debut, */
	     /* il faut rechercher la regle sur l'element suivant */
	     /* car les elements marque page debut sont places AVANT les elements */
	     /* qui portent la regle page */
	     if ((pEl->ElTypeNumber == PageBreak + 1)
		 && pEl->ElPageType == PgBegin)
		if (pEl->ElNext != NULL)
		  {
		     pSuiv = pEl->ElNext;
		     /* on saute les eventuelles marques de colonnes */
		     /* ou de page (pour d'autres vues par exemple) */
		     while (pSuiv != NULL
			    && pSuiv->ElTypeNumber == PageBreak + 1)
			pSuiv = pSuiv->ElNext;
		     /* on cherche sur pPsuiv car normalement l'element */
		     /* marque page debut a ete place juste devant l'element qui */
		     /* portait la regle page correspondante */
		     if (pSuiv != NULL)
		       {
			  ChSchemaPres (pSuiv, pSchPPage, &Entree, &pSchS);
			  pRegle = (*pSchPPage)->PsElemPRule[Entree - 1];
			  stop = False;
			  do
			     if (pRegle == NULL)
				stop = True;
			     else if (pRegle->PrType > PtFunction)
				stop = True;
			     else
			       {
				  pRe1 = pRegle;
				  if (pRe1->PrType == PtFunction
				      && pRe1->PrPresFunction == FnPage
				      && pRe1->PrViewNum == VueNb)
				     /* trouve', c'est une regle Page pour cette vue */
				    {
				       TypeP = pRe1->PrPresBox[0];
				       stop = True;
				       /* l'element Marque Page appartient au meme schema */
				       /* de structure que l'element qui porte la regle  */
				       /* Page. Ainsi, on utilisera le bon schema de */
				       /* presentation pour construire */
				       /* le contenu de l'element Marque Page */
				       pEl->ElSructSchema = pSuiv->ElSructSchema;
				    }
				  else
				     pRegle = pRe1->PrNextPRule;
				  /* passe a la regle suivante */
			       }
			  while (!(stop));
		       }	/* fin pSuiv != NULL */
		  }
	     /* si on n'a rien trouve, on fait la recherche sur les ascendants */
	     /* ou si ce n'est pas une regle de debut */
	     /* ou si le pere est la racine */
	     if (TypeP == 0)
#endif /* __COLPAGE__ */
		do
		   /* cherche une regle Page parmi les regles de */
		   /* presentation de l'element pPere */
		  {
		     ChSchemaPres (pPere, pSchPPage, &Entree, &pSchS);
		     pRegle = (*pSchPPage)->PsElemPRule[Entree - 1];
		     stop = False;
		     do
			if (pRegle == NULL)
			   stop = True;
			else if (pRegle->PrType > PtFunction)
			   stop = True;
			else
			  {
			     pRe1 = pRegle;
			     if (pRe1->PrType == PtFunction
				 && pRe1->PrPresFunction == FnPage
				 && pRe1->PrViewNum == VueNb)
				/* trouve', c'est une regle Page pour cette vue */
			       {
				  TypeP = pRe1->PrPresBox[0];
				  stop = True;
				  /* l'element Marque Page appartient au meme schema */
				  /* de structure que l'element qui porte la regle  */
				  /* Page. Ainsi, on utilisera le bon schema de */
				  /* presentation pour construire */
				  /* le contenu de l'element Marque Page */
#ifdef __COLPAGE__
				  if (pEl->ElTypeNumber == PageBreak + 1)
#endif /* __COLPAGE__ */
				     pEl->ElSructSchema = pPere->ElSructSchema;
			       }
			     else
				pRegle = pRe1->PrNextPRule;
			     /* passe a la regle suivante */
			  }
		     while (!(stop));
		     pPere = pPere->ElParent;
		     /* passe a l'element ascendant */
		  }
		while (!(TypeP != 0 || pPere == NULL));
	  }
     }				/* fin pEl != NULL */
   return TypeP;
}

#ifdef __COLPAGE__

/**CL*//**CL*//**CL*//**CL*//**CL*//**CL*//**CL*//**CL*//**CL*//**CL*/
/* nouvelle procedure pour les colonnes *//**CL*/
/* ---------------------------------------------------------------------- */
/* |    TypeBCol cherche le type de boite colonne qui correspond a`     | */
/* |            l'element Marque Page (de type colonne) pointe par pEl  | */
/* |            pour la vue VueNb.                                      | */
 /* |    Rq : VueNb est un numero de vue de schema de pres       | */
/* |            On cherche les regles Colonnes des elements englobants  | */
/* |            l'element Marque Page (de type colonne) ainsi que dans  | */
/* |            les  elements Marque Page qui precedent pEl             | */
/* |             Retourne le numero de boite                            | */
/* |            decrivant la colonne et le schema de presentation ou`   | */
/* |            cette boite est definie.                                | */
 /*      Attention :  on place les marques colonnes AVANT       | */
 /*      l'element qui contient la regle Column (sauf racine et MP)| */
/* |            on parcourt l'arbre abstrait jusqu'a rencontrer une regle | */
/* |            Column ou une regle Page contenant une regle Column.    | */
/* |            retourne dans NbCol, le nombre de colonnes de la regle  | */
/* |            Column */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
int                 TypeBCol (PtrElement pEl, int VueNb, PtrPSchema * pSchPPage, int *NbCol)

#else  /* __STDC__ */
int                 TypeBCol (pEl, VueNb, pSchPPage, NbCol)
PtrElement          pEl;
int                 VueNb;
PtrPSchema         *pSchPPage;
int                *NbCol;

#endif /* __STDC__ */

{
   PtrElement          pPere, pSuiv, pPrec;
   int                 TypeP, TypeCol, Entree;
   PRuleType           TRegle;
   PtrPRule        pRegle;
   PtrSSchema        pSchS;
   boolean             stop, debutdoc;
   PtrPRule        pRe1;

   TypeCol = 0;
   *NbCol = 0;
   if (pEl != NULL)
     {
	pPere = pEl->ElParent;
	/* on regarde si c'est une regle de debut de document : si c'est un */
	/* element marque page ou colonne avant non precede d'un element */
	/* autre qu'une marque page ou colonne */
	/* TODO pb si l'element marque page ou colonne a ete place suite */
	/* a une regle page ou colonne du premier fils de la racine ! */
	debutdoc = False;	/* a priori on n'est pas au debut du document */
	if (pEl->ElTypeNumber == PageBreak + 1
	  && (pEl->ElPageType == ColBegin || pEl->ElPageType == ColGroup)
	    && pPere->ElParent == NULL)
	  {
	     pPrec = pEl->ElPrevious;
	     stop = False;
	     while (!stop && !debutdoc)
		if (pPrec == NULL)
		   debutdoc = True;
		else if (pPrec->ElTypeNumber != PageBreak + 1)
		   stop = True;
		else
		   pPrec = pPrec->ElPrevious;
	  }

	if (pPere != NULL)	/* test bidon car toujours vrai ? */
	  {
	     /* si la marque page est une colonne debut ou groupees et que le pere */
	     /* n'est pas la racine, il faut rechercher la regle sur l'element suivant */
	     /* car les elements marque page debut sont places AVANT les elements */
	     /* qui portent la regle page ; sauf si la regle colonne est placee */
	     /* dans une boite page : la marque colonne se trouve alors apres */
	     /* la marque page */
	     if ((pEl->ElTypeNumber == PageBreak + 1)
		 && pEl->ElPageType == ColBegin)
		if (pPere->ElParent != NULL || !debutdoc)
		   if (pEl->ElNext != NULL)
		     {
			pSuiv = pEl->ElNext;
			/* on cherche uniquement sur pPsuiv car normalement l'element */
			/* marque page debut a ete place juste devant l'element qui */
			/* portait la regle page correspondante */
			/* on saute les eventuelles autres marques page (d'autres vues) */
			while (pSuiv != NULL && pSuiv->ElTypeNumber == PageBreak + 1)
			   pSuiv = pSuiv->ElNext;
			if (pSuiv != NULL)	/* pSuiv n'est pas une marque page */
			  {
			     ChSchemaPres (pSuiv, pSchPPage, &Entree, &pSchS);
			     pRegle = (*pSchPPage)->PsElemPRule[Entree - 1];
			     stop = False;
			     do
				if (pRegle == NULL)
				   stop = True;
				else if (pRegle->PrType > PtFunction)
				   stop = True;
				else
				  {
				     pRe1 = pRegle;
				     if (pRe1->PrType == PtFunction
					 && pRe1->PrPresFunction == FnColumn
					 && pRe1->PrViewNum == VueNb)
					/* trouve', c'est une regle Colonne pour cette vue */
				       {
					  if (pEl->ElPageType == ColBegin)
					     TypeCol = pRe1->PrPresBox[1];	/* on prend la */
					  /* premiere boite colonne */
					  else	/* (la boite 0 est ColGroup) */
					     TypeCol = pRe1->PrPresBox[0];
					  *NbCol = pRe1->PrNPresBoxes - 1;
					  /* la boite ColGroup ne compte pas */
					  stop = True;
					  /* l'element Marque Page appartient au meme schema */
					  /* de structure que l'element qui porte la regle  */
					  /* Page. Ainsi, on utilisera le bon schema de */
					  /* presentation pour construire */
					  /* le contenu de l'element Marque Page */
					  pEl->ElSructSchema = pSuiv->ElSructSchema;
				       }
				     else
					pRegle = pRe1->PrNextPRule;
				     /* passe a la regle suivante */
				  }
			     while (!(stop));
			  }
		     }

	     /* si on n'a rien trouve, on fait la recherche sur les precedents */
	     /* car cela peut etre une regle FnColumn placee dans une boite Page */
	     /* et dans ce cas, la marque colonne debut est placee apres la marque */
	     /* page : on recherche l'element marque page qui pourrait avoir une */
	     /* regle FnColumn */
	     if (TypeCol == 0 && pEl->ElTypeNumber == PageBreak + 1
		 && pEl->ElPageType == ColBegin)
		if (pEl->ElPrevious != NULL)
		  {
		     pPrec = pEl->ElPrevious;
		     /* on saute les eventuelles autres marques page (d'autres vues) */
		     while (pPrec != NULL && pPrec->ElTypeNumber == PageBreak + 1
			    && (pPrec->ElViewPSchema != VueNb
				|| pPrec->ElPageType == ColBegin
				|| pPrec->ElPageType == ColComputed
				|| pPrec->ElPageType == ColGroup
				|| pPrec->ElPageType == ColUser))
			pPrec = pPrec->ElPrevious;
		     if (pPrec != NULL && pPrec->ElTypeNumber == PageBreak + 1)
			/* on est bien sur un element Marque page de la meme vue */
		       {
			  /* on recherche si cet element porte une regle colonne */
			  /* on recherche la boite page correspondante */
			  TypeP = TypeBPage (pPrec, VueNb, pSchPPage);
			  pRegle = (*pSchPPage)->PsPresentBox[TypeP - 1].PbFirstPRule;
			  /* on recherche la regle colonne de la page */
			  TRegle = PtFunction;
			  ChercheReglePEl (&pRegle, pPrec, VueNb, TRegle, &pRegle);
			  stop = False;
			  while (!stop && pRegle != NULL)
			     if (pRegle->PrType != PtFunction)
				pRegle = NULL;
			     else if (pRegle->PrPresFunction == FnColumn
				      && pRegle->PrViewNum == VueNb)
			       {
				  TypeCol = pRegle->PrPresBox[1];
				  *NbCol = pRegle->PrNPresBoxes - 1;
				  /* la boite ColGroup ne compte pas */
				  stop = True;
			       }
			     else
				pRegle = pRegle->PrNextPRule;
		       }
		  }

	     /* si on n'a rien trouve, on fait la recherche sur les ascendants */
	     /* il faut rechercher conjointement les regles page et colonnes */
	     /* car la regle colonne qui s'applique peut se trouver dans une regle */
	     /* page */
	     /* ce code traite aussi le cas ou ce n'est pas une regle de debut */
	     if (TypeCol == 0)
		do
		   /* cherche une regle Page ou FnColumn parmi les regles de */
		   /* presentation de l'element pPere */
		  {
		     ChSchemaPres (pPere, pSchPPage, &Entree, &pSchS);
		     pRegle = (*pSchPPage)->PsElemPRule[Entree - 1];
		     stop = False;
		     do
			if (pRegle == NULL)
			   stop = True;
			else if (pRegle->PrType > PtFunction)
			   stop = True;
			else
			  {
			     pRe1 = pRegle;
			     if (pRe1->PrType == PtFunction
				 && pRe1->PrPresFunction == FnPage
				 && pRe1->PrViewNum == VueNb)
				/* trouve', c'est une regle Page pour cette vue */
			       {
				  TypeP = pRe1->PrPresBox[0];
				  /* on recherche si cette boite page contient une regle */
				  /* colonne */
				  pRegle = (*pSchPPage)->PsPresentBox[TypeP - 1]
				     .PbFirstPRule;
				  /* on recherche la regle colonne de la page */
				  TRegle = PtFunction;
				  ChercheReglePEl (&pRegle, pPere, VueNb, TRegle, &pRegle);
				  while (!stop && pRegle != NULL)
				     if (pRegle->PrType != PtFunction)
					pRegle = NULL;
				     else if (pRegle->PrPresFunction == FnColumn
					      && pRegle->PrViewNum == VueNb)
				       {
					  /* attention si ElPageNumber = 0, c'est la boite */
					  /* ColGroup, sinon c'est la colonne 1, 2, ... */
					  TypeCol = pRegle->PrPresBox[pEl->ElPageNumber];
					  *NbCol = pRegle->PrNPresBoxes - 1;
					  /* la boite ColGroup ne compte pas */
					  stop = True;
					  pEl->ElSructSchema = pPere->ElSructSchema;
				       }
				     else
					pRegle = pRegle->PrNextPRule;
				  if (!stop)
				     /* on passe a la regle suivante de pPere */
				     pRegle = pRe1->PrNextPRule;
			       }
			     else if (pRe1->PrType == PtFunction
				      && pRe1->PrPresFunction == FnColumn
				      && pRe1->PrViewNum == VueNb)
				/* trouve', c'est une regle Column pour cette vue */
			       {
				  /* on prend la bonne boite colonne en fonction */
				  /* du numero de page de pEl; ici considere comme */
				  /* un numero de colonne */
				  /* attention si ElPageNumber = 0, c'est la boite */
				  /* ColGroup, sinon c'est la colonne 1, 2, ... */
				  TypeCol = pRe1->PrPresBox[pEl->ElPageNumber];
				  *NbCol = pRe1->PrNPresBoxes - 1;
				  /* la boite ColGroup ne compte pas */
				  stop = True;
				  /* l'element Marque Page appartient au meme schema */
				  /* de structure que l'element qui porte la regle */
				  /* Page. Ainsi, on utilisera le bon schema de */
				  /* presentation pour construire */
				  /* le contenu de l'element Marque Page */
				  pEl->ElSructSchema = pPere->ElSructSchema;
			       }
			     else
				pRegle = pRe1->PrNextPRule;
			     /* passe a la regle suivante */
			  }
		     while (!(stop));
		     pPere = pPere->ElParent;
		     /* passe a l'element ascendant */
		  }
		while (!(TypeCol != 0 || pPere == NULL));
	  }			/* fin pPere != NULL */
     }				/* fin pEl != NULL */
   return TypeCol;
}
/* fin TypeBCol */
#endif /* __COLPAGE__ */

/* ---------------------------------------------------------------------- */
/* |    CptPage retourne le numero de compteur a` utiliser pour         | */
/* |            numeroter la marque de page pointee par pEl, dans la    | */
/* |            vue VueNb. Retourne egalement dans pSchPPage le schema  | */
/* |            de presentation ou est defini ce compteur. Retourne 0 si| */
/* |            cette page n'est pas numerotee.                         | */
	 /* VueNb = Vue dans le schema de presentation          | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
int                 CptPage (PtrElement pEl, int VueNb, PtrPSchema * pSchPPage)

#else  /* __STDC__ */
int                 CptPage (pEl, VueNb, pSchPPage)
PtrElement          pEl;
int                 VueNb;
PtrPSchema         *pSchPPage;

#endif /* __STDC__ */

{
   int                 bp;
   int                 cptpage;

   /* cherche d'abord la boite page */
   bp = TypeBPage (pEl, VueNb, pSchPPage);
   if (bp > 0)
      cptpage = (*pSchPPage)->PsPresentBox[bp - 1].PbPageCounter;
   else
      cptpage = 0;
   return cptpage;
}

#ifdef __COLPAGE__

 /* -------------------------------------------------------------- */
 /*  |    NbPages retourne le nombre de pages sous la racine pAb| */
 /* -------------------------------------------------------------- */


#ifdef __STDC__
int                 NbPages (PtrAbstractBox pAb)

#else  /* __STDC__ */
int                 NbPages (pAb)
PtrAbstractBox             pAb;

#endif /* __STDC__ */

{
   int                 nb;
   PtrAbstractBox             pP;

   nb = 0;
   pP = pAb->AbFirstEnclosed;
   while (pP != NULL)
     {
	/*     est-ce un pave corps de page ? */
	/*     on saute les marque page colonnes */
	if (pP->AbElement->ElTerminal
	    && pP->AbElement->ElLeafType == LtPageColBreak
	    && (pP->AbElement->ElPageType == PgBegin
		|| pP->AbElement->ElPageType == PgComputed
		|| pP->AbElement->ElPageType == PgUser)
	    && !pP->AbPresentationBox)
	   nb++;
	pP = pP->AbNext;
     }
   return nb;
}
#endif /* _COLPAGE__ */

#ifdef __COLPAGE__
/* ---------------------------------------------------------------------- */
/* |    PagePleine evalue si la page en cours de remplissage est pleine.| */
 /*     dans le cas ou le document est pagine.                  | */
 /*     si doc pagine, on s'arrete sur une page pleine : donc   | */
 /*     si pEl, qui est le prochain element a traiter,          | */
 /*     est une Marque Page: on positionne le volume            | */
 /*     libre de la vue a -1 (teste ensuite par VuePleine)      | */
 /*     Cette procedure detruit les paves crees inutiles        | */
 /*     (aux frontieres de page)                                        | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                PagePleine (DocViewNumber VueNb, PtrDocument pDoc, PtrElement pEl,
				boolean EnAvant)

#else  /* __STDC__ */
void                PagePleine (VueNb, pDoc, pEl, EnAvant)
DocViewNumber           VueNb;
PtrDocument         pDoc;
PtrElement          pEl;
boolean             EnAvant;

#endif /* __STDC__ */

{
   boolean             pleine, fini;
   PtrElement          pElRacine, pEl1, pPage;
   PtrAbstractBox             pRac, pP, pAb, PavPage, pPavDetruire, PavReaff,
                       PavR;
   int                 bp, Nb, NbAvant, volsupp;
   boolean             assoc, trouve;
   PtrPSchema          pSchPPage;
   int              VueSch;

   pleine = False;
   pP = NULL;
   PavReaff = NULL;
   assoc = VueAssoc (pEl);
   if (!assoc)
      /* ce n'est pas une vue d'elements associes */
     {
	fini = (pDoc->DocView[VueNb - 1].DvPSchemaView == 0);
	if (!fini)
	   pRac = pDoc->DocRootElement->ElAbstractBox[VueNb - 1];
     }
   else
      /* c'est une vue d'elements associes */
     {
	fini = (pDoc->DocAssocFrame[pEl->ElAssocNum - 1] == 0);
	if (!fini)
	  {
	     pElRacine = pDoc->DocAssocSubTree[pEl->ElAssocNum - 1];
	     if (pElRacine == NULL)
		pElRacine = pDoc->DocAssocRoot[pEl->ElAssocNum - 1];
	     if (pElRacine != NULL)
		pRac = pElRacine->ElAbstractBox[VueNb - 1];	/* VueNb est 1 */
	     else
	       {
		  fini = True;
		  pRac = NULL;
	       }
	  }
     }

   if (!fini)
      if (pRac != NULL && pRac->AbFirstEnclosed != NULL)
	{
	   /* dans tous les cas (pagine ou non) on positionne pleine */
	   /* si le volume restant est negatif */
	   if (!assoc)
	      pleine = (pDoc->DocViewFreeVolume[VueNb - 1] <= 0);
	   else
	      pleine = (pDoc->
			DocAssocFreeVolume[pEl->ElAssocNum - 1] < 0);
	   /* si la vue n'est pas pleine, on regarde si le document */
	   /* est pagine (pour mettre a jour le volume si le nombre de */
	   /* pages a augmente */
	   if (!pleine)
	      /* on determine si le document est pagine */
	      /* c'est-a-dire si un ascendant a une regle page (bp non nul) */
	     {
		/* on cherche la vue (de schema) a appliquer pour pEl */
		/* car elle peut etre differente de son pere d'ou vient */
		/* l'appel de VuePleine */
		VueSch = VueAAppliquer (pEl, NULL, pDoc, VueNb);
		bp = TypeBPage (pEl, VueSch, &pSchPPage);
		if (bp != 0)
		  {
		     if (!assoc)
			NbAvant = pDoc->DocViewNPages[VueNb - 1];
		     else
			NbAvant = pDoc->DocAssocNPages[pEl->ElAssocNum - 1];
		     /* si NbAvant est negatif, on s'arrete uniquement */
		     /* sur le volume, on ne considere pas les pages */
		     /* utilise' pour la pagination */
		     if (NbAvant >= 0)
		       {
			  Nb = NbPages (pRac);
			  /* on s'arrete si on a cree une page en plus et */
			  /* si la page est pleine : le prochain element a traiter */
			  /* est une marque de page pour la vue consideree */
			  /* pour s'arreter, on met un volume negatif */
			  if (Nb > NbAvant + 1
			      && (pEl->ElTypeNumber == (PageBreak + 1)
				  && pEl->ElViewPSchema == VueSch)
			      && (pEl->ElPageType == PgBegin
				  || pEl->ElPageType == PgComputed
				  || pEl->ElPageType == PgUser))
			     /* cas particulier : premiere et derniere page */
			     /* il ne faut pas s'arreter pour creer les paves */
			     /* de presentation de la racine */
			    {
			       trouve = False;
			       pEl1 = pEl;
			       while (!trouve && pEl1 != NULL)
				 {
				    if (EnAvant)
				       pPage = FwdSearchTypedElem (pEl1,
						PageBreak + 1, NULL);
				    else
				       pPage = BackSearchTypedElem (pEl1,
						PageBreak + 1, NULL);
				    /* on saute les colonnes */
				    if (pPage != NULL &&
					(pPage->ElPageType == PgBegin
					 || pPage->ElPageType == PgComputed
				    || pPage->ElPageType == PgUser)
					&& pPage->ElViewPSchema == VueSch)
				       trouve = True;
				    else
				       pEl1 = pPage;
				 }
			       if (trouve)
				  /* ce n'est pas le premiere ou derniere page */
				 {
				    /* si la page precedente est une page vide, */
				    /* on continue */
				    PavPage = RechPavPage (pEl, VueNb,
							   VueSch, EnAvant);
				    while (PavPage != NULL &&
					   PavPage->AbPresentationBox)
				       PavPage = PavPage->AbNext;
				    if (PavPage != NULL && PavPage->AbVolume > 1)
				       /* ce n'est ni une page d'extremite, */
				       /* ni une page qui suit une page vide */
				       /* cas page interieure dont la page prec est */
				       /* non vide ; on met un volume negatif pour */
				       /* que la procedure VuePleine rende vrai aux englobants */
				      {
					 if (!assoc)
					    pDoc->DocViewFreeVolume[VueNb - 1] = -1;
					 else
					    pDoc->DocAssocFreeVolume
					       [pEl->ElAssocNum - 1] = -1;
					 /* on detruit les eventuels paves vides (si la */
					 /* marque page sur laquelle on s'arrete etait premier */
					 /* fils d'un element) */
					 /* on ne detruit rien s'il est seul fils de son pere */
					 if (!(pEl->ElParent->ElFirstChild == pEl
					       && pEl->ElNext == NULL))
					   {
					      /* recherche du premier ascendant qui a un pave */
					      /* TODO */
					      pAb = pEl->ElParent->ElAbstractBox[VueNb - 1];
					      while (pAb != NULL && pAb->AbPresentationBox)
						 pAb = pAb->AbNext;
					      /* recherche l'ascendant de plus haut niveau a detruire */
					      trouve = False;
					      pPavDetruire = NULL;
					      while (!trouve && pAb != NULL
					      && pAb->AbElement->ElTypeNumber !=
						     PageBreak + 1
						     && pAb != pDoc->DocViewRootAb[VueNb - 1])
						{
						   /* on regarde si pAb est vide */
						   pP = pAb->AbFirstEnclosed;
						   while (pP != NULL && !trouve)
						      if (pP->AbPresentationBox)
							 pP = pP->AbNext;
						      else if (!pP->AbDead)
							 trouve = True;
						      else
							 /* pP est mort -> pAb est vide */
							 /* on regarde s'il y a un pave suivant */
							 /* cas !EnAvant */
							 pP = pP->AbNext;
						   if (!trouve)
						      /* pAb est vide, on le marque mort */
						      /* et on passe a l'englobant */
						     {
							TuePave (pAb);		/* et mise a jour du volume */
							SuppRfPave (pAb, &PavReaff, pDoc);
							pPavDetruire = pAb;
							pAb = pAb->AbEnclosing;
						     }
						}
					      if (pPavDetruire != NULL)
						{
						   /* cas ou on a detruit tout le contenu du corps */
						   /* de page : on lui met son volume a un pour */
						   /* ne s'affiche pas en grise         */
						   if (pPavDetruire->AbEnclosing->AbElement
						       ->ElTypeNumber == PageBreak + 1)
						      pPavDetruire->AbEnclosing->AbVolume = 1;
						   /* destruction des paves */
						   pAb = pPavDetruire;
						   PavR = NULL;
						   TuePresVoisin (pAb->AbPrevious, True,
								  pDoc, &PavR, &PavReaff, &volsupp, pAb, False);
						   TuePresVoisin (pAb->AbNext, False,
								  pDoc, &PavR, &PavReaff, &volsupp, pAb, False);
						   LibPavMort (pAb->AbEnclosing);
						}
					   }
				      }
				 }
			    }
		       }	/* fin NbAvant >= 0 */
		  }
	     }
	}

}
#endif /* _COLPAGE__ */

#ifdef __COLPAGE__

/* ---------------------------------------------------------------------- */
/* |    TestElHB teste si l'element pEl est un element reference        | */
/* |        vers un element associe place en haut ou bas de page        | */
/* |        retourne un pointeur vers l'element reference               | */
/* |        si il faut creer le pave de cet element.                    | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
PtrElement          TestElHB (PtrElement pEl, DocViewNumber VueNb)

#else  /* __STDC__ */
PtrElement          TestElHB (pEl, VueNb)
PtrElement          pEl;
DocViewNumber           VueNb;

#endif /* __STDC__ */

{
   boolean             premier;
   PtrElement          pE, Ret;
   PtrReference        pRef;

   Ret = NULL;
   if (pEl != NULL)
      if (pEl->ElTerminal && pEl->ElLeafType == LtReference)
	{


	   pRef = pEl->ElReference;
	   if (pRef != NULL && pRef->RdReferred != NULL)
	      if (!pRef->RdReferred->ReExternalRef)
		{
		   pE = pRef->RdReferred->ReReferredElem;
		   /* cet element est-il un element associe a placer */
		   /* en haut ou bas de page ? */
		   if (pEl->ElSructSchema->SsPSchema
		       ->PsInPageHeaderOrFooter[pE->ElTypeNumber - 1])
		      /* c'est bien un element a placer en haut ou bas de page */
		      /* faut-il creer son pave ? */
		      /* oui si il n'a pas encore de pave */
		      /* et si l'element pEl est la premiere reference a pE */
		      if (pE->ElAbstractBox[VueNb - 1] == NULL)
			{
			   premier = True;	/* a priori, pEl est la premiere */
			   /* reference a pE */
			   while (pRef->RdPrevious != NULL && premier)
			     {
				pRef = pRef->RdPrevious;
				if (pRef->RdElement != NULL)
				   if (!DansTampon (pRef->RdElement))
				      if (pRef->RdElement->ElAssocNum == 0)
					 /* il y a une reference precedente qui */
					 /* n'est pas dans le tampon */
					 /* et qui n'est pas un element associe' */
					 premier = False;
			     }
			   /* on ne prend en compte que la premiere reference a */
			   /* l'element qui n'est pas dans un element associe' */
			   if (premier)
			      /* si l'element reference' est dans le tampon */
			      /* couper/coller, on l'ignore */

			      if (!DansTampon (pE))
				 Ret = pE;
			}
		}
	}
   return Ret;
}
#endif /* __COLPAGE__ */

#ifdef __COLPAGE__

/* ---------------------------------------------------------------------- */
/* |     CrPavHB  cree les paves de l'element associe pEl.              | */
/* |             Ce pave doit etre place en haut ou bas de la           | */
/* |             page courante pElPage.                                 | */
/* |             Si l'element pere de pEl (c'est la racine de           | */
/* |             l'arbre associe) n'a pas de pave dans la page,         | */
/* |             il faut le  creer en meme temps.                       | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                CrPavHB (PtrElement pEl, PtrDocument pDoc, DocViewNumber VueNb,
			     PtrElement pElPage, boolean EnAvant)

#else  /* __STDC__ */
void                CrPavHB (pEl, pDoc, VueNb, pElPage, EnAvant)
PtrElement          pEl;
PtrDocument         pDoc;
DocViewNumber           VueNb;
PtrElement          pElPage;
boolean             EnAvant;

#endif /* __STDC__ */

{
   boolean             baspage, bool;
   PtrAbstractBox             pAb, pP1, pPere, ancPere, DerPav, pHB, pPRP, pPRP1,
                       pPRP2, PavReaff, pP;
   AbDimension       *pPavD1;
   PtrElement          pVoisin;
   PresentationBox             *pBo1;
   boolean             fin, trouve, perenouveau, premtour, complet, ok,
                       stop;
   boolean             nouveau;
   int                 volume, frame, h;
   PtrPRule        pRegle, pRCre, pRD, pRS, pR, pRV, pRegleDimV, pRStd;
   PtrPSchema          pSPR;
   PtrSSchema        pSS;
   PtrAttribute         pAttr;
   int                 vue, VueSch, Entree;
   int                 Hauteur, PosV, CarCoupe;		/* pour calcul page */
   FILE               *list;

   pHB = NULL;
   if (pElPage->ElAbstractBox[VueNb - 1] != NULL)
      /* la page a ete creee, on peut creer le pave de pEl */
      if (pEl != NULL && pEl->ElParent != NULL
	  && pEl->ElParent->ElParent == NULL)
	 /* le pere de pEl est bien la racine de l'arbre associe */
	{
	   /* on verifie d'abord si il existe bien une regle de */
	   /* creation des elements associes */
	   ok = False;
	   VueSch = pDoc->DocView[VueNb - 1].DvPSchemaView;
	   /* on recherche s'il a une regle de creation d'elements */
	   /* associes */
	   stop = False;
	   pRCre = ReglePEl (pElPage, &pSPR, &pSS, 0,
			     NULL, VueSch, PtFunction, True, False, &pAttr);
	   do
	      if (pRCre == NULL)
		 stop = True;
	      else if (pRCre->PrType != PtFunction)
		 stop = True;
	      else
		 /* c'est une regle de creation */
		 /* on regarde si la boite creee contient */
		 /* des elements associes du type de pEl */
		{
		   pBo1 = &pSPR->PsPresentBox[pRCre->PrPresBox[0] - 1];
		   if (pRCre->PrViewNum == VueSch
		       && pBo1->PbContent == ContElement
		       && pBo1->PbContElem == pEl->ElTypeNumber
		       && pSPR == pElPage->ElAbstractBox[VueNb - 1]->AbPSchema)
		      ok = True;
		   else
		      pRCre = pRCre->PrNextPRule;
		}
	   while (!(stop || ok));
	   if (ok)		/* on a trouve la regle, on recherche */
	      /* le pave de presentation correspondant */
	      /* c'est un pave haut ou bas de page */
	     {
		trouve = False;
		pHB = pElPage->ElAbstractBox[VueNb - 1];
		while (!trouve && pHB != NULL && pHB->AbElement == pElPage)
		   if (pHB->AbPresentationBox
		       && pHB->AbTypeNum == pRCre->PrPresBox[0])
		      trouve = True;
		   else
		      pHB = pHB->AbNext;
		if (!trouve)
		   /* le pave haut ou bas de page n'a pas ete cree */
		   /* il faut le creer */
		   pHB = CrPavPres (pElPage, pDoc, pRCre, pElPage->ElSructSchema,
				    NULL, VueNb, pSPR, False, True);
	     }
	   /* fin boucle de recherche de la regle pour trouver */
	   /* la regle de creation d'elements associes references */
	   /* normalement on doit avoir trouve la regle */
	   /* et pHB est le pave haut ou bas sous lequel sont mis les */
	   /* elements associes du type de pEl */
	   /* si la regle ou pHB ne sont pas trouves, on ne cree rien */
	   if (pHB != NULL)
	     {
		/* on recherche le pave du pere dans la page */
		perenouveau = False;
		trouve = False;
		if (pEl->ElParent->ElAbstractBox[VueNb - 1] != NULL)
		  {
		     pPere = pEl->ElParent->ElAbstractBox[VueNb - 1];
		     while (pPere->AbPresentationBox)
			pPere = pPere->AbNext;
		     while (!trouve && !perenouveau)
			if (pPere->AbEnclosing->AbElement == pElPage)
			   trouve = True;
			else if (pPere->AbEnclosing->AbElement->ElPageNumber
				 < pElPage->ElPageNumber)
			   if (pPere->AbNextRepeated != NULL)
			      pPere = pPere->AbNextRepeated;
			   else
			     {
				/* il faut creer le nouveau pave apres */
				pP1 = pPere;
				pPere = CopiePav (pP1);
				pP1->AbNextRepeated = pPere;
				pPere->AbPreviousRepeated = pP1;
				perenouveau = True;
			     }
			else
			   /* cas ou il faut creer le pave avant et le chainer */
			   /* aux paves existants */
			  {
			     pP1 = pPere;
			     pPere = CopiePav (pP1);
			     pPere->AbNextRepeated = pP1;
			     if (pP1->AbPreviousRepeated == NULL)
				pEl->ElParent->ElAbstractBox[VueNb - 1] = pPere;
			     pPere->AbPreviousRepeated = pP1->AbPreviousRepeated;
			     pP1->AbPreviousRepeated = pPere;
			     perenouveau = True;
			  }
		  }

		else
		   /* le pere n'a pas encore de pave, on le cree sans sa descendance */
		  {
		     pPere = CreePaves (pEl->ElParent, pDoc, VueNb, EnAvant, False,
					&complet);
		     if (pPere != NULL)
			perenouveau = True;
		     /* ce pave n'est pas coupe en tete */
		     if (pEl->ElParent->ElAbstractBox[VueNb - 1] != NULL)
			pEl->ElParent->ElAbstractBox[VueNb - 1]->AbTruncatedHead = False;
		  }
		if (perenouveau && pPere != NULL)
		   /* chainage sous le pave pHB  Haut ou Bas de page qui */
		   /* contient ces elements associes */
		   /* chainage particulier car on place un pave d'element */
		   /* sous un pave de presentation */
		  {
		     pPere->AbEnclosing = pHB;
		     /* cas simple si pHB n'a pas de fils */
		     if (pHB->AbFirstEnclosed == NULL)
			pHB->AbFirstEnclosed = pPere;
		     else
			/* placement parmi les autres paves de presentation du */
			/* haut ou bas de page : donne par ordre des regles ? */
			/* choix de placement : pour haut de page, placement */
			/* comme dernier fils de pHB, pour bas de page, placement */
			/* comme premier fils */
		     if (pSPR->PsPresentBox[pHB->AbTypeNum - 1].PbPageHeader)
			/* la boite correspondant au pave pHB est un haut de page */
		       {
			  pP1 = pHB->AbFirstEnclosed;
			  while (pP1->AbNext != NULL)
			     pP1 = pP1->AbNext;
			  pP1->AbNext = pPere;
			  pPere->AbPrevious = pP1;
			  baspage = False;
		       }
		     else
			/* (pSPR->PsPresentBox[pHB->AbTypeNum-1].PbPageFooter) */
			/* normalement toujours vrai */
		       {
			  pP1 = pHB->AbFirstEnclosed;
			  pHB->AbFirstEnclosed = pPere;
			  pPere->AbNext = pP1;
			  pP1->AbPrevious = pPere;
			  baspage = True;	/* pour mise a jour hauteur page */
		       }
		  }
		if (trouve || perenouveau)
		  {
		     /* on cree les paves de pEl et ceux des voisins non references */
		     volume = pDoc->DocViewFreeVolume[VueNb - 1];
		     pDoc->DocViewFreeVolume[VueNb - 1] = THOT_MAXINT;
		     pP1 = CreePaves (pEl, pDoc, VueNb, EnAvant, True, &complet);
		     /* probleme de chainage ? est-ce que les paves */
		     /* crees sont places sous le bon pave pere ? */
		     /* oui dans le cas de la creation de l'image abstraite */
		     /* pas forcement si nouvel element : il faut considerer */
		     /* le numero de la page */
		     if (pEl->ElAbstractBox[VueNb - 1]->AbEnclosing != pPere)
		       {
			  /* l'element pEl a ete mal chaine, il faut refaire le */
			  /* chainage */
			  pP1 = pEl->ElAbstractBox[VueNb - 1];
			  DerPav = pP1;
			  while (DerPav->AbNext != NULL
				 && DerPav->AbNext->AbElement == pEl)
			     DerPav = DerPav->AbNext;
			  /* on dechaine les paves de pEl de l'ancien pere */
			  if (pP1->AbEnclosing->AbFirstEnclosed == pP1)
			    {
			       pP1->AbEnclosing->AbFirstEnclosed = DerPav->AbNext;
			       if (DerPav->AbNext != NULL)
				  DerPav->AbNext->AbPrevious = NULL;
			    }
			  else
			    {
			       pP1->AbPrevious->AbNext = DerPav->AbNext;
			       if (DerPav->AbNext != NULL)
				  DerPav->AbNext->AbPrevious = pP1->AbPrevious;
			    }
			  /* on dechaine les paves de pEl de ses anciens voisins */
			  DerPav->AbNext = NULL;
			  pP1->AbPrevious = NULL;
			  ancPere = pP1->AbEnclosing;	/* pour reappliquer les regles */
			  /* chainage au nouveau pere : pPere */
			  if (pPere->AbFirstEnclosed == NULL)
			     pPere->AbFirstEnclosed = pP1;
			  else
			    {
			       pP = pPere->AbFirstEnclosed;
			       while (pP->AbPresentationBox
				      && pP->AbElement == pPere->AbElement
				      && pP->AbNext != NULL)
				  pP = pP->AbNext;
			       if (pP->AbElement == pPere->AbElement)
				  /* le pere n'a pas de fils mais que des paves de */
				  /* de presentation on chaine pP1 apres */
				  /* normalement il faut se placer avant les derniers */
				 {
				    pP->AbNext = pP1;
				    pP1->AbPrevious = pP;
				 }
			       else if (pP->AbElement == pEl->ElNext)
				  /* le premier fils de pPere est l'element suivant pEl */
				  /* on chaine les paves de pEl avant pP */
				 {
				    if (pP->AbPrevious == NULL)
				       pPere->AbFirstEnclosed = pP1;
				    else
				       pP->AbPrevious->AbNext = pP1;
				    pP1->AbPrevious = pP->AbPrevious;
				    DerPav->AbNext = pP;
				    pP->AbPrevious = DerPav;
				 }
			       else
				  /* pEl doit etre chaine en queue */
				  /* mais avant les paves de presentation derniers */
				  /* fils de pPere */
				 {
				    while (pP->AbNext != NULL
					   && pP->AbNext->AbElement !=
					   pPere->AbElement)
				       pP = pP->AbNext;
				    DerPav->AbNext = pP->AbNext;
				    if (pP->AbNext != NULL)
				       pP->AbNext->AbPrevious = DerPav;
				    pP->AbNext = pP1;
				    pP1->AbPrevious = pP;
				 }
			    }
			  while (pP1 != NULL && pP1->AbElement == pEl)
			    {
			       pP1->AbEnclosing = pPere;
			       pP1 = pP1->AbNext;
			    }
			  RecursEvalCP (pPere, pDoc);
			  RecursEvalCP (ancPere, pDoc);
		       }
		     /* modification des regles due aux nouveaux paves crees */
		     pP1 = pEl->ElAbstractBox[VueNb - 1];
		     DerPav = pP1;
		     while (DerPav->AbNext != NULL
			    && DerPav->AbNext->AbElement == pEl)
			DerPav = DerPav->AbNext;
		     NouvRfPave (pP1, DerPav, &PavReaff, pDoc);

		     /* verifie les elements associes voisins */
		     pVoisin = pEl->ElNext;
		     fin = False;
		     premtour = True;
		     do
		       {
			  if (pVoisin == NULL)
			     if (premtour && pEl->ElPrevious != NULL)
			       {
				  pVoisin = pEl->ElPrevious;
				  premtour = False;
			       }
			     else
				fin = True;
			  if (!fin)
			    {
			       ok = False;
			       if (pVoisin->ElAbstractBox[VueNb - 1] == NULL)
				  if (pVoisin->ElReferredDescr == NULL)
				     /* l'element voisin n'est pas reference, on */
				     /* va creer ses paves */
				     ok = True;
				  else if (pVoisin->ElReferredDescr->ReFirstReference == NULL)
				     ok = True;
				  else if (pVoisin->ElReferredDescr->ReFirstReference->RdElement == NULL)
				     ok = True;
				  else if (pVoisin->ElReferredDescr->ReFirstReference->RdElement->
					   ElAssocNum == pVoisin->ElAssocNum)
				     /* l'element voisin est reference' depuis */
				     /* un element associe' de meme */
				     /* type; on cree ses paves */
				     ok = True;
				  else
				     /* inutile de creer les paves du voisin, il seront */
				     /* crees lorsqu'on rencontrera sa 1ere reference */
				     ok = False;
			       if (ok)
				 {
				    /* cree les paves du voisin */
				    pP1 = CreePaves (pVoisin, pDoc, VueNb, True,
						     True, &complet);
				    /* modification des regles dues aux nouveaux paves crees */
				    pP1 = pVoisin->ElAbstractBox[VueNb - 1];
				    DerPav = pP1;
				    while (DerPav->AbNext != NULL
					   && DerPav->AbNext->AbElement == pVoisin)
				       DerPav = DerPav->AbNext;
				    NouvRfPave (pP1, DerPav, &PavReaff, pDoc);
				    if (premtour)
				       pVoisin = pVoisin->ElNext;
				    else
				       pVoisin = pVoisin->ElPrevious;
				 }
			       else
				  pVoisin = NULL;
			       /* on arrete de traiter les voisins dans ce sens */
			    }
		       }
		     while (!(fin));

		     /* retablit le volume libre reel */
		     pDoc->DocViewFreeVolume[VueNb - 1] =
			pDoc->DocViewFreeVolume[VueNb - 1] - THOT_MAXINT + volume;
		  }		/* fin creation du pave de l'element associe */
		if (perenouveau)
		  {
		     /* application des regles de presentation du pave pPere */
		     /* necessaire si c'est un pave resultat d'une duplication */
		     /* pRS : premiere regle de presentation */
		     /* specifique de la boite a creer */
		     ChSchemaPres (pEl->ElParent, &pSPR, &Entree, &pSS);
		     pRS = pSPR->PsElemPRule[Entree - 1];
		     /* pRD : premiere regle de presentation par defaut du schema de
		      */
		     /* presentation */
		     pRD = pSPR->PsFirstDefaultPRule;
		     pP1 = pDoc->DocRootElement->ElAbstractBox[VueNb - 1];	/* pour retarder */
		     /* on est toujours dans une vue de l'arbre principal */
		     do
		       {
			  pR = GetRule (&pRS, &pRD, pEl, NULL, pSS);
			  /* pointeur sur la regle a  appliquer pour la vue 1 */
			  if (pR != NULL)
			     /* cherche et applique les regles de tous les types */
			     /* pour la vue */
			     for (vue = 1; vue <= MAX_VIEW; vue++)
			       {
				  if (vue == 1)
				     pRV = NULL;
				  else
				     pRV = GetRuleView (&pRS, &pRD, pR->PrType, vue, pEl, NULL,
							pSS);
				  if (vue == VueSch)
				     /* applique la regle au pave cree'. */
				    {
				       if (pRV == NULL)
					  pRV = pR;
				       if (pRV->PrType == PtFunction)
					  /* le pave cree' cree un pave de presentation */
					  /* cree le pave de presentation */
					  pP1 = CrPavPres (pEl->ElParent, pDoc, pRV, pSS,
					    NULL, VueNb, pSPR, False, True);
				       else if (!Applique (pRV, pSPR, pPere, pDoc, NULL, &bool))
					  /* on n'a pas pu appliquer la regle, on */
					  /* l'appliquera lorsque le pave pere sera */
					  /* termine' */
					  /* retard sur la racine */
					  /* est-ce vraiment la qu'il faut les retarder ? */
					  Retarde (pRV, pSPR, pPere, NULL, pP1);
				    }
			       }
		       }
		     while (!(pR == NULL));
		     /* on applique les regles qui etaient retardees sur la */
		     /* racine des elements associes */
		     pP = pEl->ElParent->ElAbstractBox[VueNb - 1];
		     pPRP1 = pP;
		     /* saute les paves de presentation crees par FnCreateBefore */
		     stop = False;
		     do
			if (pP == NULL)
			   stop = True;
			else if (!pP->AbPresentationBox)
			   stop = True;
			else
			   pP = pP->AbNext;
		     while (!(stop));
		     /* on initialise le pave sur lequel des regles seront */
		     /* mises les regles retardees a nouveau */
		     pP1 = pDoc->DocRootElement->ElAbstractBox[VueNb - 1];
		     pPRP = pP;
		     pPRP2 = pP;
		     /* boucle de parcours de la chaine des paves dupliques */
		     while (pPRP != NULL)
		       {
			  do
			    {
			       pP = pPRP;	/* pP a ete modifie dans la boucle */
			       pPRP1 = pP;
			       /* on saute les paves de presentation repetes */
			       while (pP->AbPresentationBox)
				  pP = pP->AbNext;
			       pPRP2 = pP;
			       GetRet (&pRegle, &pSPR, &pP, &pAttr);
			       if (pRegle != NULL)
				  if (!Applique (pRegle, pSPR, pP, pDoc, pAttr, &bool))
				     /* cette regle n'a pas pu etre appliquee           */
				     /* c'est une regle correspondant a un attribut, on */
				     /* l'appliquera lorsque l'englobant sera complet   */
				     /* regles retardees a la racine */
				    {
				       if (pPRP != pP1)		/* pour ne pas boucler sur la racine */
					  Retarde (pRegle, pSPR, pP, pAttr, pP1);
				    }
			    }
			  while (!(pRegle == NULL));
			  NouvRfPave (pPRP1, pPRP2, &PavReaff, pDoc);
			  pPRP = pPRP->AbNextRepeated;
		       }

		     /*RecursEvalCP (pP1, pDoc); *//*Pour etre sur du bon positionnement */
		     /* des paves !! TODO a changer ! */
		  }		/* fin if perenouveau */
		/* on met a jour les variables de hauteur des paves de pages */
		/* pour permettre une bonne pagination et une hauteur correcte */
		/* du corps de page */
		if (PaginationEnCours)
		  {
		     /* pAb = pave haut de page (ou bas de page) */
		     pAb = pEl->ElAbstractBox[VueNb - 1]->AbEnclosing->AbEnclosing;
		     /* on signale au mediateur les nouveaux paves crees */
		     /* on appelle Modifvue a partir du pave haut (ou bas de page) */
		     h = 0;
		     frame = pDoc->DocViewFrame[VueNb - 1];
		     bool = ModifVue (frame, &h, pAb);
		     /* calcul de la hauteur du pave haut ou bas de page */
		     HautCoupure (pAb, True, &Hauteur, &PosV, &CarCoupe);
		     /* Hauteur = dimension verticale du pave haut (ou bas) de page */

		     if (baspage)	/* des paves ont ete ajoutes en bas de page */
		       {
			  /* mise a jour de HauteurCoupPage pour permettre */
			  /* une bonne pagination */
			  HauteurCoupPage = HauteurTotalePage - Hauteur;
			  /* mise a jour de HauteurBasPage */
			  HauteurBasPage = Hauteur;
		       }
		     else	/* des paves ont ete ajoutes en haut de page */
			/* la hauteur de coupure ne change pas car c'est toujours */
			/* la hauteur totale de la page moins le bas qui est inchange */
			HauteurHautPage = Hauteur;
		     /* mise a jour de la hauteur du corps de page */
		     /* on met (ou on modifie) une regle de presentation specifique */
		     /* cherche d'abord la regle qui s'applique a l'element */
		     pRStd = ReglePEl (pElPage, &pSPR, &pSS, 0, NULL, VueSch,
				       PtHeight, False, True, &pAttr);
		     /* cherche si l'element a deja une regle de hauteur specifique */
		     pRegleDimV = ChReglePres (pElPage, PtHeight, &nouveau, pDoc, VueNb);
		     if (nouveau)
			/* on a cree' une regle de hauteur pour l'element */
		       {
			  pR = pRegleDimV->PrNextPRule;		/* on recopie la regle standard */
			  *pRegleDimV = *pRStd;
			  pRegleDimV->PrNextPRule = pR;
			  pRegleDimV->PrCond = NULL;	/* pas de condition d'application associee */
			  pRegleDimV->PrViewNum = VueSch;	/* inutile ? */
		       }
		     /* change le parametre de la regle */
		     pRegleDimV->PrDimRule.DrValue =
			HauteurTotalePage - HauteurBasPage - HauteurHautPage;
		     pP = pElPage->ElAbstractBox[VueNb - 1];
		     while (pP->AbPresentationBox)
			pP = pP->AbNext;
		     /* pP = pave du corps de page */
		     /* applique la nouvelle regle specifique Verticale */
		     if (pRegleDimV != NULL)
			Applique (pRegleDimV, pSPR, pP, pDoc, pAttr, &bool);
		     pPavD1 = &pP->AbHeight;
		     pPavD1->DimMinimum = True;	/* regle de hauteur minimum */
		     /* TODO inutile, a supprimer car fait par applique */
		     pPavD1->DimValue = HauteurTotalePage - HauteurBasPage - HauteurHautPage;
		     pP->AbHeightChange = True;
		     /* on memorise le pave de pEl (ou son pere si il a ete cree) */
		     /* au cas ou la page deborde a cause de lui */
		     if (!ArretAvantCreation)
			if (perenouveau)
			   PavAssocADetruire = pEl->ElAbstractBox[VueNb - 1]->AbEnclosing;
			else
			   PavAssocADetruire = pEl->ElAbstractBox[VueNb - 1];
		  }		/* fin PaginationEnCours */
	     }
	}
   /* sauvegarde de l'image abstraite pour tests */
   list = fopen ("/perles/roisin/debug/pbfig", "w");
   if (list != NULL)
     {
	NumPav (pP1);
	AffPaves (pP1, 2, list);
	fclose (list);
     }
}
#endif /* __COLPAGE__ */

#ifdef __COLPAGE__

/* -------------------------------------------------------------- */
/* | FeuilleSuiv rend l'element feuille qui suit                | */
/* |           l'element pEl. Cet element peut etre             | */
/* |           un element compose sans fils                     | */
/* -------------------------------------------------------------- */

#ifdef __STDC__
static PtrElement   FeuilleSuiv (PtrElement pEl)

#else  /* __STDC__ */
static PtrElement   FeuilleSuiv (pEl)
PtrElement          pEl;

#endif /* __STDC__ */

{
   PtrElement          Retour, pElSuiv, pElPere;
   boolean             Trouve;

   Retour = NULL;
   Trouve = False;
   pElPere = pEl;
   pElSuiv = NULL;
   /* recherche du noeud suivant en remontant l'arbre si besoin */
   do
     {
	pElSuiv = pElPere->ElNext;
	if (pElSuiv == NULL)
	   pElPere = pElPere->ElParent;
     }
   while (pElSuiv == NULL && pElPere != NULL);
   if (pElSuiv != NULL)
      /* on va chercher la premiere feuille de ce sous-arbre */
      while (!Trouve)
	{
	   if (pElSuiv->ElTerminal
	       || (!pElSuiv->ElTerminal && pElSuiv->ElFirstChild == NULL))
	     {
		Trouve = True;
		Retour = pElSuiv;
	     }
	   else
	      /* pElSuiv est un element compose, non vide, on cherche sa premiere */
	      /* feuille */
	      pElSuiv = pElSuiv->ElFirstChild;
	}
   else
      Retour = NULL;		/* pas d'element feuille precedent */
   return Retour;

}
#endif /* __COLPAGE__ */

#ifdef __COLPAGE__
/* ---------------------------------------------------------------------- */
/* |    ArretCreationCol retourne vrai si il faut arreter la creation   | */
/* |            pour creer une nouvelle colonne (cas ou on a une colonne| */
/* |            et ou on doit changer de groupes de colonnes : il faut  | */
/* |            couper cette colonne en jajoutant un nouvelle marque).  | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static boolean      ArretCreationCol (PtrElement pEl, PtrDocument pDoc,
				      DocViewNumber VueNb, int VueSch)

#else  /* __STDC__ */
static boolean      ArretCreationCol (pEl, pDoc, VueNb, VueSch)
PtrElement          pEl;
PtrDocument         pDoc;
DocViewNumber           VueNb;
int                 VueSch;

#endif /* __STDC__ */

{
   PtrAbstractBox             pAb;
   int                 NbCol, TypeP;
   boolean             retour;
   PtrPSchema          pSchP;

   retour = False;		/* a priori, pas d'arret */

   /* on recherche si le groupe de colonnes precedent est */
   /* dans la meme page, c'est-a-dire si le pave page/col */
   /* precedent n'est pas le pave d'une marque page */
   /* si oui, on equilibre les colonnes du groupe precedent */
   /* et on continue la creation */
   pAb = RechPavPageCol (pEl, VueNb, VueSch, True);
   if (pAb != NULL &&
       (pAb->AbElement->ElPageType == ColBegin
	|| pAb->AbElement->ElPageType == ColComputed
	|| pAb->AbElement->ElPageType == ColUser))
      /* normalement, pAb est un pave de colonne simple */
      /* on remonte jusqu'au */
      /* pave de colonne groupee qui les englobe */
      /* on equilibre pour l'instant uniquement si le */
      /* groupe de colonnes ne contient qu'une colonne */
      /* et que la regle en comporte deux */
      /* TODO a generaliser */
      if (pAb->AbElement->ElPageNumber == 1)
	{
	   TypeP = TypeBCol (pAb->AbElement, VueSch, &pSchP, &NbCol);
	   if (NbCol == 2)
	     {
		pAb = pAb->AbEnclosing;
		if (pAb->AbElement->ElPageType == ColGroup)
		   /* toujours vrai normalement */
		  {
		     retour = True;
		     /* on sort de la procedure pour que Page */
		     /* ajoute une marque colonne et */
		     /* cree les paves des elements en consequence */
		     /* l'arret est remonte de la pile des appels en mettant le */
		     /* volume libre negatif (cf. VuePleine) */
		     /* volume mis a -1 : arret creation */
		     if (VueAssoc (pEl))
			pDoc->DocAssocFreeVolume[pEl->ElAssocNum - 1] = -1;
		     else
			pDoc->DocViewFreeVolume[VueNb - 1] = -1;
		  }
	     }
	}
   return retour;
}
/* end of function ArretCreationCol */
#endif /* __COLPAGE__ */

#ifdef __COLPAGE__
/*------------------------------------------------------------------------*/
/* |     InitPageCol determine les conditions de creation               | */
/* |             si l'element a creer est une colonne ou une page       | */
/* |             Au retour, Creation, complet, et ApplRegles sont       | */
/* |             positionnes. De plus pEl et pElSauv peuvent l'etre     | */
/* |             aussi.                                                 | */
/*------------------------------------------------------------------------*/
#ifdef __STDC__
void                InitPageCol (PtrElement * ppEl, PtrDocument pDoc, DocViewNumber VueNb,
			    int VueSch, boolean EnAvant, boolean * Creation,
	      boolean * complet, boolean * ApplRegles, PtrElement * pElSauv)
#else  /* __STDC__ */
void                InitPageCol (ppEl, pDoc, VueNb, VueSch, Creation, complet, ApplRegles,
				 pElSauv)
PtrElement         *ppEl;
PtrDocument         pDoc;
DocViewNumber           VueNb;
int                 VueSch;
boolean             EnAvant;
boolean            *Creation;
boolean            *complet;
boolean            *ApplRegles;
PtrElement         *pElSauv;

#endif /* __STDC__ */
{
   boolean             ArretPageRef;
   boolean             stop, FilsComplet;
   PtrElement          pEl, pEl1, pPage;
   PtrAbstractBox             pP;

   pEl = *ppEl;
   /* si pagination en cours et que l'element est une marque */
   /* colonne groupee, il faut arreter la creation et equilibrer */
   /* le groupe de colonnes precedent si besoin */
   /* PaginationEnCours et A_Equilibrer sont des variables globales */
   if (PaginationEnCours && A_Equilibrer)
      if (pEl->ElTerminal && pEl->ElLeafType == LtPageColBreak
	  && pEl->ElPageType == ColGroup
	  && pEl->ElViewPSchema == VueSch)
	 ArretColGroupee = ArretCreationCol (pEl, pDoc, VueNb, VueSch);
   /* si pagination en cours et que l'element est une marque page */
   /* ou un element reference sur un element associe place en haut */
   /* ou bas de page, on s'arrete si ArretAvantCreation est vrai */
   /* l'arret est remonte de la pile des appels en mettant le */
   /* volume libre negatif (cf. VuePleine) */
   ArretPageRef = False;
   if (PaginationEnCours && ArretAvantCreation)
      if (pEl->ElTerminal && pEl->ElLeafType == LtPageColBreak
	  && (pEl->ElPageType == PgBegin
	      || pEl->ElPageType == PgComputed
	      || pEl->ElPageType == PgUser))
	{
	   if (pEl->ElViewPSchema == VueSch)
	     {
		TrouveMPHB = True;	/* variable globale */
		ArretPageRef = True;
	     }
	}
      else if (TestElHB (pEl, VueNb) != NULL)
	{
	   TrouveMPHB = True;	/* variable globale */
	   ArretPageRef = True;
	}

   if (ArretPageRef)
      /* volume mis a -1 : arret creation */
      if (VueAssoc (pEl))
	 pDoc->DocAssocFreeVolume[pEl->ElAssocNum - 1] = -1;
      else
	 pDoc->DocViewFreeVolume[VueNb - 1] = -1;

   if (!ArretColGroupee && !ArretPageRef)
     {
	/* cas temporaire ou des colonnes ne sont pas englobees dans un pave */
	/* groupe de colonnes. On cree un element Marque ColGroup devant */
	/* la premiere colonne */
	if (pEl->ElTypeNumber == PageBreak + 1
	    && pEl->ElViewPSchema == VueSch
	 && (pEl->ElPageType == ColBegin || pEl->ElPageType == ColComputed))
	  {
	     pEl1 = BackSearchTypedElem (pEl, PageBreak + 1, NULL);
	     while (pEl1 != NULL && pEl1->ElViewPSchema != VueSch)
		pEl1 = BackSearchTypedElem (pEl1, PageBreak + 1, NULL);
	     if (pEl1 != NULL
		 && (pEl1->ElPageType == PgBegin
		     || pEl1->ElPageType == PgComputed
		     || pEl1->ElPageType == PgUser))
		/* pEl est une marque colonne de debut qui n'est pas precedee */
		/* d'une marque colonne groupee : on en ajoute une */
	       {
		  pEl1 = NewSubtree (PageBreak + 1, pEl->ElSructSchema,
		       pDoc, pEl->ElAssocNum, True, True, True, True);
		  InsertElementBefore (pEl, pEl1);
		  pEl1->ElPageType = ColGroup;
		  pEl1->ElViewPSchema = VueSch;
		  pEl1->ElPageNumber = 0;
		  /* numero attribue pour les groupes */
		  pP = CreePaves (pEl1, pDoc, VueNb, True, True, &FilsComplet);
	       }
	  }
	/* cas du scroll arriere : si pEl est une marque de page, il faut */
	/* creer le pave de la marque de page */
	/* precedente (si il n'existe pas). En effet, les paves qui */
	/* seront crees ensuite seront places dans cette page */
	/* cependant, la chaine des dupliques doit etre creee en */
	/* considerant le chemin entre la racine et pEl */
	if (!EnAvant && pEl->ElTerminal && pEl->ElLeafType == LtPageColBreak
	    && pEl->ElViewPSchema == VueSch)
	   if (pEl->ElAbstractBox[VueNb - 1] != NULL)
	      if (pEl->ElPageType == PgBegin || pEl->ElPageType == PgComputed
		  || pEl->ElPageType == PgUser)
		{		/* cas marque page avec pave */
		   pPage = pEl;
		   /* recherche de la marque page precedente pour la vue */
		   /* en sautant les marques colonnes */
		   stop = False;
		   do
		     {
			pEl1 = pPage;
			pPage = BackSearchTypedElem (pEl1, PageBreak + 1, NULL);
			if (pPage == NULL)
			   stop = True;
			else if (pPage->ElViewPSchema == VueSch
				 && (pPage->ElPageType == PgBegin
				     || pPage->ElPageType == PgComputed
				   || pPage->ElPageType == PgUser))
			   stop = True;
		     }
		   while (!stop);
		   if (pPage != NULL)
		     {
			/* il y a un element marque page precedent */
			if (pPage->ElAbstractBox[VueNb - 1] == NULL)
			   /* la marque de page precedente n'a pas de pave */
			   /* corps de page, il faut la creer : */
			   /* la suite de la procedure est executee avec l'element */
			   /* marque de page precedent (sauf duplication)  */
			  {
			     *Creation = True;
			     *complet = True;	/* un pave corps de page est complet */
			     *pElSauv = pEl;	/* a restaurer avant le retour */
			     /* de crimabs */
			     pEl = pPage;
			  }
			else
			   /* la marque de page precedente a deja un pave */
			   /* si la suppression des paves est faite correctement */
			   /* la chaine des dupliques existe aussi */
			   /* il ne faut rien creer */
			   /* faut-il y appliquer les regles de presentation ? */
			  {
			     *Creation = False;
			     *complet = True;	/* un pave corps de page est tj complet */
			  }
		     }
		}
	      else
		 /* cas ou pEl est une marque colonne */
		{
		   /* differenciation du cas colsimple / colgroupees */
		   if (pEl->ElPageType == ColGroup)
		      /* il faut prendre le colGroupee  precedent */
		     {
			/* recherche de la marque ColGroup precedente */
			pEl1 = pEl;
			do
			   pEl1 = BackSearchTypedElem (pEl1, PageBreak + 1, NULL);
			while (pEl1 != NULL && (pEl1->ElViewPSchema != VueSch
					     || pEl1->ElPageType == ColBegin
					  || pEl1->ElPageType == ColComputed
				    || pEl1->ElPageType == ColUser));
			if (pEl1 != NULL
			    && pEl1->ElPageType == ColGroup
			    && pEl1->ElAbstractBox[VueNb - 1] == NULL)
			  {
			     /* il y a un element marque ColGroup precedent */
			     /* qui n'a pas de pave */
			     /* la suite  est executee avec l'element */
			     /* colonne groupees precedent (sauf dup)  */
			     *Creation = True;
			     *complet = True;	/* un pave colonne est complet */
			     *pElSauv = pEl;	/* a restaurer avant le retour */
			     /* de crimabs */
			     pEl = pEl1;
			  }
			else
			   /* la marque precedente est une marque page */
			   /* ou c'est une colonne qui a deja un pave ; */
			   /* si la suppression des paves est faite correctement */
			   /* la chaine des dupliques existe aussi */
			   /* il ne faut rien creer */
			   /* faut-il y appliquer les regles de presentation ? */
			  {
			     *Creation = False;
			     *complet = True;
			  }
		     }		/* fin cas colonne groupee */
		   else
		      /* colonne simple */
		     {
			pPage = pEl;
			/* recherche de la marque colonne precedente pour la vue */
			/* en s'arretant si on trouve une marque page ou col groupee */
			do
			  {
			     pEl1 = pPage;
			     pPage = BackSearchTypedElem (pEl1, PageBreak + 1, NULL);
			  }
			while (pPage != NULL && pPage->ElViewPSchema != VueSch);
			if (pPage != NULL &&
			    (pPage->ElPageType == ColBegin
			     || pPage->ElPageType == ColComputed
			     || pPage->ElPageType == ColUser)
			    && pPage->ElAbstractBox[VueNb - 1] == NULL)
			   /* il y a un element marque colonne precedent */
			  {
			     /* la marque de colonne precedente n'a pas de pave */
			     /* la suite du code est executee avec l'element */
			     /* marque de colonne precedent (sauf duplication)  */
			     *Creation = True;
			     *complet = True;	/* un pave colonne est complet */
			     *pElSauv = pEl;	/* a restaurer avant le retour */
			     /* de crimabs */
			     pEl = pPage;
			  }
			else
			   /* la marque precedente est une marque page */
			   /* ou c'est une colonne qui a deja un pave ; */
			   /* si la suppression des paves est faite correctement */
			   /* la chaine des dupliques existe aussi */
			   /* il ne faut rien creer */
			   /* faut-il y appliquer les regles de presentation ? */
			  {
			     *Creation = False;
			     *complet = True;	/* un pave corps de page est tj complet */
			  }
		     }		/* fin colonne simple */
		}		/* fin cas colonne */
	   else
	      /* pas de pave pour pEl, on le creera */
	     {
		*Creation = True;
		*complet = True;	/* un pave corps de page est tj complet */
	     }
	/* TODO prevoir test de verification : l'element voisin */
	/* a droite est une marque de page (page vide) */
	else if (pEl->ElAbstractBox[VueNb - 1] != NULL)
	  {
	     /* pEl n'est pas une marque page ou sens = EnAvant */
	     /* le pave existe deja pour cette vue */
	     /* il faut sauter les paves de presentation et les paves dupliques */
	     /* si on va EnAvant */
	     pP = pEl->ElAbstractBox[VueNb - 1];
	     while (pP->AbPresentationBox)
		pP = pP->AbNext;
	     if (EnAvant)
		while (pP->AbNextRepeated != NULL)
		   pP = pP->AbNextRepeated;
	     *Creation = False;
	     if (pP->AbLeafType != LtCompound ||
	     /* on retire le cas mis en ligne */
	     /*  pEl->ElAbstractBox[VueNb - 1]->AbInLine || */
		 (pEl->ElTerminal && pEl->ElLeafType == LtPageColBreak))
		/* c'est une feuille ou un pave' mis en lignes,  */
		/* ou un corps de page */
		/* il a deja tout son contenu */
		*complet = True;
	     else if (EnAvant)
		*complet = !pP->AbTruncatedTail;
	     else
		*complet = !pP->AbTruncatedHead;

	     if (pP->AbSize == -1)
		*ApplRegles = True;
	     /* il faut lui appliquer ses regles de presentation */
	  }
	else
	   /* le pave n'existe pas pour cette vue, on essaiera de le creer. */
	  {
	     *Creation = True;
	     /* on ne sait pas si on pourra creer toute son image */
	     *complet = False;
	  }

     }
   *ppEl = pEl;			/* au retour pEl peut avoir ete modifie (cas !EnAvant) */
}				/* end of InitPageCol */
#endif /* __COLPAGE__ */


#ifndef __COLPAGE__
/* ---------------------------------------------------------------------- */
/* | ApplPage    Applique les regles de presentation au pave            | */
/* |       cree quand c'est une marque de page TypeP                    | */
/* |       et pSchPPage ont ete initialises dans ChercheVisib           | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                ApplPage (PtrElement pEl, PtrDocument pDoc, DocViewNumber VueNb, int VueSch,
			  int TypeP, PtrPSchema pSchPPage, PtrAbstractBox NouvPave)
#else  /* __STDC__ */
void                ApplPage (pEl, pDoc, VueNb, VueSch, TypeP, pSchPPage, NouvPave)
PtrElement          pEl;
PtrDocument         pDoc;
DocViewNumber           VueNb;
int                 VueSch;
int                 TypeP;
PtrPSchema          pSchPPage;
PtrAbstractBox             NouvPave;

#endif /* __STDC__ */
{
   int                 nv;
   PtrPRule        pRegle, pRegleV, pRSpec, pRDef;
   PtrAbstractBox             PavFils;
   AbDimension       *pPavD1;

   /* traitement particulier aux sauts de page */
   /* le cas des pages Rappel est supprime */
   /* c'est un saut de page, on va creer tout le contenu du pave */
   NouvPave->AbAcceptLineBreak = False;
   NouvPave->AbAcceptPageBreak = False;
   NouvPave->AbNotInLine = False;
   NouvPave->AbTruncatedHead = False;
   NouvPave->AbTruncatedTail = False;
   /* engendre le contenu de la boite de changement de page */
   PavFils = initpave (pEl, VueNb, 10);
   PavFils->AbPSchema = NouvPave->AbPSchema;
   NouvPave->AbFirstEnclosed = PavFils;
   /* chaine le pave cree' */
   PavFils->AbEnclosing = NouvPave;
   /* initialise le contenu de ce pave : un filet horizontal d'une */
   /* epaisseur de 1 pt */
   PavFils->AbLeafType = LtGraphics;
   PavFils->AbShape = 'h';
   PavFils->AbGraphAlphabet = 'L';
   PavFils->AbVolume = 1;
   PavFils->AbCanBeModified = False;
   PavFils->AbEnclosing->AbVolume = PavFils->AbVolume;
   PavFils->AbAcceptLineBreak = False;
   PavFils->AbAcceptPageBreak = False;
   PavFils->AbNotInLine = False;
   /* applique a ce pave les regles de presentation de la boite */
   /* page */
   pEl->ElAbstractBox[VueNb - 1] = PavFils;
   pRSpec = pSchPPage->PsPresentBox[TypeP - 1].PbFirstPRule;
   /* premiere regle de presentation par defaut */
   pRDef = pSchPPage->PsFirstDefaultPRule;
   do
     {
	pRegle = GetRule (&pRSpec, &pRDef, pEl, NULL, pEl->ElSructSchema);
	/* pointeur sur la regle a appliquer pour la vue 1 */
	if (pRegle != NULL)
	   if (pRegle->PrType == PtFunction)
	     {
		/* parmi les fonctions, on ne traite que les regles */
		/* CreateBefore et CreateAfter */
		if (pRegle->PrPresFunction == FnCreateBefore || pRegle->PrPresFunction == FnCreateAfter)
		   CrPavPres (pEl, pDoc, pRegle, pEl->ElSructSchema,
			      NULL, VueNb, pSchPPage, False, True);
	     }
	   else
	      /* ce n'est pas une fonction de presentation */
	      /* cherche et applique la regle de meme type pour la vue */
	      for (nv = 1; nv <= MAX_VIEW; nv++)
		{
		   if (nv == 1)
		      pRegleV = NULL;
		   else
		      pRegleV = GetRuleView (&pRSpec, &pRDef,
					     pRegle->PrType, nv, pEl, NULL,
					     pEl->ElSructSchema);
		   if (nv == VueSch && VueExiste (pEl, pDoc, VueNb))
		     {
			if (pRegleV == NULL)
			   pRegleV = pRegle;
			Applique (pRegleV, pSchPPage, PavFils,
				  pDoc, NULL);
		     }
		}
     }
   while (!(pRegle == NULL));
   pEl->ElAbstractBox[VueNb - 1] = NouvPave;
   /* impose les regles de la boite marque de page */
   /* hauteur d'une marque de page */
   pPavD1 = &PavFils->AbHeight;
   pPavD1->DimIsPosition = False;
   pPavD1->DimValue = 1;
   pPavD1->DimAbRef = NULL;
   pPavD1->DimUnit = UnPoint;
   pPavD1->DimUserSpecified = False;
   PavFils->AbSize = 1;
   PavFils->AbSizeUnit = UnPoint;
   PavFils->AbHighlight = 0;
   PavFils->AbUnderline = 0;
   PavFils->AbThickness = 0;
   PavFils->AbVisibility = PavFils->AbEnclosing->AbVisibility;
}				/* end of ApplPage */

#endif /* __COLPAGE__ */



#ifdef __COLPAGE__

/* ------------------------------------------------------------------------ */
/* |    CreePageCol     cree les paves de structure physique (page,       | */
/* |                    colonne) sous le pave racine et duplique les paves| */
/* |                    des elements peres de pEl.                        | */
/* |                    pEl est une marque de page ou de colonne qui      | */
/* |                    concerne la vue traitee                           | */
/* ------------------------------------------------------------------------ */
#ifdef __STDC__
void                CreePageCol (PtrElement pEl, PtrAbstractBox * AdrNouvPave, PtrDocument pDoc,
			       DocViewNumber VueNb, int VueSch, boolean EnAvant,
				 boolean * complet, int *lqueue,
	      PtrPRule queuePR[MAX_QUEUE_LEN], PtrAbstractBox queuePP[MAX_QUEUE_LEN],
				 PtrPSchema queuePS[MAX_QUEUE_LEN],
			PtrAttribute queuePA[MAX_QUEUE_LEN], PtrElement pElSauv,
			   PtrPSchema pSchPPage, int TypeP, boolean * arret)
#else  /* __STDC__ */
void                CreePageCol (pEl, AdrNouvPave, pDoc, VueNb, VueSch, EnAvant, complet,
			queuePA, queuePS, queuePP, queuePR, lqueue, pElSauv,
				 pSchPPage, TypeP, arret)
PtrElement          pEl;
PtrAbstractBox            *AdrNouvPave;
PtrDocument         pDoc;
DocViewNumber           VueNb;
int                 VueSch;
boolean             EnAvant;
boolean            *complet;
int                *lqueue;
PtrPRule        queuePR[MAX_QUEUE_LEN];
PtrAbstractBox             queuePP[MAX_QUEUE_LEN];
PtrPSchema          queuePS[MAX_QUEUE_LEN];
PtrAttribute         queuePA[MAX_QUEUE_LEN];
PtrElement          pElSauv;
PtrPSchema          pSchPPage;
int                 TypeP;
boolean            *arret;

#endif /* __STDC__ */
{
   PtrElement          pEl1, pElRacine, pPage, pPere, pNext;
   PtrAbstractBox             pP, NouvPave, PavPres;
   PtrAbstractBox             PavPagePrec, pDupPav, pPavDupFils, pAb, pPa1;
   PtrAbstractBox             PavRacine, pP1, pPAvant, pPApres, PavPere, pPsuiv;
   int                 frame, h, Entree, Hauteur, PosV, CarCoupe, cas,
                       nv;
   boolean             stop, trouve, adupliquer, filsdup, adetruire, premcol,
                       bool;
   boolean             FilsComplet;
   PtrPSchema          pSPres;
   PtrPRule        pRegle, pRegleV, pRSpecPage, pRDefPage, pRCre;
   PRuleType           TRegle;
   PtrSSchema        pSchS;
   PtrAttribute         pAttr;
   AbDimension       *pPavD1;
   PresentationBox             *pBo1;

   /* cas de la derniere marque de page : on ne genere pas de pave */
   /* c'est toujours un element fils de la racine *//* a verifier ?? */
   /* code a supprimer lorsqu'on aura supprime cette marque dans l'arbre */

   NouvPave = *AdrNouvPave;

   /* initialisation de pElRacine et de PavRacine */
   if (VueAssoc (pEl))
     {
	pElRacine = pDoc->DocAssocRoot[pEl->ElAssocNum - 1];
	PavRacine = pElRacine->ElAbstractBox[0];
     }
   else
     {
	pElRacine = pDoc->DocRootElement;
	PavRacine = pElRacine->ElAbstractBox[VueNb - 1];
     }
   if (pEl->ElParent == pElRacine)
      if (pEl->ElNext == NULL)
	 /* derniere marque de page de la vue : il ne faut pas creer de pave */
	{
	   /* Verifier que l'on a pas a liberer un descripteur d'image ? */
	   FreePave (NouvPave);
	   NouvPave = NULL;
	   *AdrNouvPave = NULL;	/* seul cas ou NouvPave est en retour */
	   *complet = True;
	}
      else
	 /* on cherche si les elements suivants ne sont que des marques de page */
	 /* pour d'autres vues. Si oui, pEl est la derniere marque de page */
	 /* de la vue : on ne cree pas son pave */
	{
	   pNext = pEl->ElNext;
	   trouve = False;
	   while (!trouve && pNext != NULL)
	     {
		if (pNext->ElTerminal && pNext->ElLeafType == LtPageColBreak
		    && pNext->ElViewPSchema != VueSch)
		   pNext = pNext->ElNext;
		else
		   trouve = True;
	     }
	   if (!trouve)
	     {
		/* Verifier que l'on a pas a liberer un descripteur d'image ? */
		FreePave (NouvPave);
		NouvPave = NULL;
		*AdrNouvPave = NULL;	/* seul cas ou NouvPave est en retour */
		*complet = True;
	     }
	}
   if (NouvPave != NULL)
     {
	premcol = False;
	pEl->ElAbstractBox[VueNb - 1] = NouvPave;	/* chainage du pave a son element */
	NouvPave->AbLeafType = LtCompound;	/* c'est un pave compose */
	/* chainage de NouvPave dans l'image abstraite */
	/* il ne faut plus se placer sous la racine mais sous le pave */
	/* de structure physique de plus bas niveau */
	/* ici c'est le pave de page. Si la colonne est la colonne gauche, */
	/* c'est-a-dire si EnAvant et pEl->ElPageNumber = 1, ou !EnAvant et */
	/* pEl->ElPageNumber = derniere colonne ; il faut inserer NouvPave */
	/* entre le corps de page et ses fils et ne pas faire de duplication */

	if (pEl->ElPageType == ColBegin || pEl->ElPageType == ColComputed
	    || pEl->ElPageType == ColGroup || pEl->ElPageType == ColUser)
	   /* identification des 12 cas possibles : EnAvant ou non, pEl est */
	   /* une colonne simple ou groupee, le pave de structure physique */
	   /* qui precede (ou suit) est un pave de page, colonne simple ou groupee */

	  {
	     cas = 0;
	     pP = RechPavPageCol (pEl, VueNb, VueSch, EnAvant);
	     /* analyse des 12 cas possibles */
	     if (pP == NULL)
		cas = 0;	/*erreur */
	     else if (pEl->ElPageType == ColBegin
		      || pEl->ElPageType == ColComputed
		      || pEl->ElPageType == ColUser)	/* cas colonne simple */
		if (EnAvant)
		   if (pP->AbElement->ElPageType == ColBegin
		       || pP->AbElement->ElPageType == ColComputed
		       || pP->AbElement->ElPageType == ColUser)
		      /* pEl n'est pas la premiere colonne du groupe de colonnes */
		      cas = 1;
		   else if (pP->AbElement->ElPageType == ColGroup)
		      /* pEl est la premiere colonne du groupe de colonnes */
		      cas = 2;
		   else
		      cas = 0;	/* erreur : pP ne peut etre un pave de page */
		else
		   /* cas !EnAvant */ if (pP->AbElement->ElPageType == ColBegin
				|| pP->AbElement->ElPageType == ColComputed
			    || pP->AbElement->ElPageType == ColUser)
		   /* pEl n'est pas la derniere colonne du groupe de colonnes */
		   cas = 3;
		else
		   /* cas 4 */
		   /* si pP->AbElement->ElPageType == ColGroup, c'est que */
		   /* pEl est la derniere colonne du groupe de colonnes */
		   /* et ce groupe de colonnes n'est pas le dernier de la page */
		   /* cas 5  traite en meme temps */
		   /* si pP->AbElement->ElPageType == page, c'est que */
		   /* pEl est la derniere colonne du groupe de colonnes */
		   /* et ce groupe de colonnes est le dernier de la page */
		   /* dans ces deux cas, il faut rechercher le pave Colonne */
		   /* groupee precedent en sautant les marques colonnes */
		   /* ce pave a ete cree par l'appel a CreePaves englobant */
		  {
		     pEl1 = pEl;
		     trouve = False;
		     while (pEl1 != NULL && !trouve)
		       {
			  pEl1 = BackSearchTypedElem (pEl1, PageBreak + 1, NULL);
			  trouve = (pEl1->ElViewPSchema == VueSch &&
				    pEl1->ElPageType == ColGroup);
		       }
		     if (trouve)
		       {
			  pP = pEl1->ElAbstractBox[VueNb - 1];
			  while (pP->AbPresentationBox)
			     pP = pP->AbNext;
		       }
		     else
		       {
			  /* erreur */
			  printf ("erreur i.a. pave corps page devrait etre cree pour la colonne \n");
			  ThotExit (1);
		       }
		     cas = 4;	/* cas 5 traite en meme temps */
		  }

	     else
		/* pEl est une colonne groupee */ if (EnAvant)
		if (pP->AbElement->ElPageType == ColBegin
		    || pP->AbElement->ElPageType == ColComputed
		    || pP->AbElement->ElPageType == ColUser)
		   /* pEl n'est pas la premiere colonne groupee de la page */
		   /* pP est le pave de la derniere colonne du groupe precedent */
		  {
		     /* on remonte d'un niveau pour chainer au meme niveau */
		     /* que le pave colgroupees precedent */
		     pP = pP->AbEnclosing;
		     cas = 6;
		  }
		else if (pP->AbElement->ElPageType == ColGroup)
		   cas = 0;	/* erreur */
		else
		   /* pP est un pave de page */
		   /* pEl est la premiere colonne groupee de la page */
		   cas = 7;
	     else
		/* cas !EnAvant */ if (pP->AbElement->ElPageType == ColBegin
				|| pP->AbElement->ElPageType == ColComputed
			    || pP->AbElement->ElPageType == ColUser)
		cas = 0;	/* erreur : les col simples ne peuvent etre creees */
	     /* avant la col groupee */
	     else if (pP->AbElement->ElPageType == ColGroup)
		/* pEl n'est pas la derniere colonne groupee de la page */
		cas = 8;
	     else
		/* pP est un pave corps de page */
		/* pEl est la derniere colonne groupee de la page */
		/* il faut rechercher le pave corps de page de la page */
		/* precedente en sautant les marques colonnes simples et */
		/* groupees. Le pave de cette page precedente  */
		/* a ete cree par l'appel de CreePaves englobant */
	       {
		  pEl1 = pEl;
		  trouve = False;
		  while (pEl1 != NULL && !trouve)
		    {
		       pEl1 = BackSearchTypedElem (pEl1, PageBreak + 1, NULL);
		       trouve = (pEl1->ElViewPSchema == VueSch &&
				 (pEl1->ElPageType == PgBegin
				  || pEl1->ElPageType == PgComputed
				  || pEl1->ElPageType == PgUser));
		    }
		  if (trouve)
		    {
		       pP = pEl1->ElAbstractBox[VueNb - 1];
		       while (pP->AbPresentationBox)
			  pP = pP->AbNext;
		    }
		  else
		    {
		       /* erreur */
		       printf ("erreur i.a. pave corps page devrait etre cree pour la colonne \n");
		       ThotExit (1);
		    }
		  cas = 9;
	       }
	     /* chainage au pere : traitement des cas en deux groupes */
	     switch (cas)
		   {
		      case 0:
			 /* cas d'erreur */
			 /* on chaine a la racine pour voir ce que c'est */
			 PavPere = PavRacine;
			 break;

		      case 2:
		      case 4:
		      case 7:
		      case 9:
			 /* EnAvant et premier ou !EnAvant et dernier */
			 /* pP est le pave du corps de page ou ColsGroupees */
			 NouvPave->AbEnclosing = pP;
			 /* chainage entre le corps et ses fils */
			 if (pP->AbFirstEnclosed == NULL)	/* pas de fils, chainage simple */
			    pP->AbFirstEnclosed = NouvPave;
			 else
			   {
			      NouvPave->AbFirstEnclosed = pP->AbFirstEnclosed;
			      pP->AbFirstEnclosed = NouvPave;
			      NouvPave->AbFirstEnclosed->AbEnclosing = NouvPave;
			      pPsuiv = NouvPave->AbFirstEnclosed;
			      while (pPsuiv != NULL)
				{
				   pPsuiv->AbEnclosing = NouvPave;
				   for (TRegle = PtVertRef; TRegle <= PtHorizPos; TRegle++)
				     {
					/* recherche de la regle */
					pRegle = LaRegle (pDoc, pPsuiv, &pSPres, TRegle, True, &pAttr);
					/* application de la regle */
					if (!Applique (pRegle, pSPres, pPsuiv, pDoc, pAttr, &bool))
					   Retarde (pRegle, pSPres, pPsuiv, pAttr, NouvPave);
				     }
				   pPsuiv = pPsuiv->AbNext;
				}
			      if (!NouvPave->AbFirstEnclosed->AbNew)
				 RecursEvalCP (NouvPave->AbFirstEnclosed, pDoc);
			   }
			 premcol = True;	/* premiere colonne, ou derniere si !EnAvant */
			 /* booleen pour ne pas dupliquer */
			 break;

		      case 1:
		      case 3:
		      case 6:
		      case 8:
			 /* EnAvant et pas premier ou !EnAvant et pas dernier */
			 PavPere = pP->AbEnclosing;
			 break;

		      default:
			 break;
		   }		/* fin switch de traitement des cas */
	  }
	/* fin cas colonne : a la sortie soit premcol est vrai */
	/* et le chainage a ete effectue, soit PavPere est correctement */
	/* initialise pour effectuer le chainage */
	else
	   /* pEl est une marque page et non une marque colonne */
	  {
	     pP = RechPavPage (pEl, VueNb, VueSch, EnAvant);
	     /* pour le chainage aux voisins */
	     PavPere = PavRacine;	/* le pere des pages est la racine */
	  }
	if (!premcol)
	  {			/* le chainage a deja ete fait si premcol */
	     NouvPave->AbEnclosing = PavPere;	/* les paves marque de page */
	     /* sont fils de la racine */

	     /* chainage avec les voisins : code commun pour page et colonnes */
	     /* dans tous les cas, pP contient le pave precedent (si EnAvant) */
	     /* ou suivant (si !EnAvant) de l'element de meme niveau que pEl */
	     if (EnAvant)
		if (pP == 0)
		  {		/* cas de la premiere page */
		     /* le cas de la premiere colonne est deja traite */
		     /* chainage comme premier fils de la racine */
		     /* on suppose qu'une marque de page a ete ajoutee dans
		        l'arbre abstrait pour la 1ere page. les marques sont
		        maintenant interpretees comme des debuts de page et
		        non comme des fins de page ; cette marque est le
		        premier fils de la racine */

		     /* prevoir de retirer ce code si on retarde l'application */
		     /* des regles de presentation de la racine */
		     /* on detruit les paves de presentation de la racine */
		     pAb = PavRacine->AbFirstEnclosed;
		     /* cas du premier pave de presentation */
		     if (pAb != NULL && pAb->AbPresentationBox
			 && pAb->AbElement == PavRacine->AbElement)
		       {
			  /* il y a des paves de presentation */
			  while (pAb != NULL && pAb->AbPresentationBox
			       && pAb->AbElement == PavRacine->AbElement)
			    {
			       TuePave (pAb);
			       SuppRfPave (pAb, &pP, pDoc);
			       pAb = pAb->AbNext;
			    }
			  h = 0;
			  if (VueAssoc (pEl))
			     frame = pDoc->DocAssocFrame[pEl->ElAssocNum - 1];
			  else
			     frame = pDoc->DocViewFrame[VueNb - 1];
			  bool = ModifVue (frame, &h, PavRacine);
			  LibPavMort (PavRacine);
			  /* on met les regles de creation en attente */
			  /* pour provoquer la recreation des paves de pres */
			  /* on cherche les regles a retarder */
			  ChSchemaPres (pEl->ElParent, &pSPres, &Entree, &pSchS);
			  pRegle = pSPres->PsElemPRule[Entree - 1];
			  do
			    {
			       pRegle = pRegle->PrNextPRule;
			       /* au premier tour on saute la regle de visibilite */
			       if (pRegle != NULL && pRegle->PrType != PtFunction)
				  pRegle = NULL;
			       else if (pRegle->PrPresFunction == FnCreateFirst)
				  Retarde (pRegle, pSPres, pEl->ElParent->ElAbstractBox[0],
					   NULL, NouvPave);
			    }
			  while (!(pRegle == NULL));
		       }
		     /* chainage de NouvPave a la racine */
		     NouvPave->AbNext = PavRacine->AbFirstEnclosed;
		     PavRacine->AbFirstEnclosed = NouvPave;
		     if (NouvPave->AbNext != NULL)
			NouvPave->AbNext->AbPrevious = NouvPave;
		  }		/* fin cas de la premiere page */
		else
		   /* EnAvant et pP != NULL */
		  {
		     /* chainage a pP en sautant ses eventuels paves de pres */
		     /* remarque : dans le cas des colonnes pP->Pavsuivant */
		     /* est normalement null car on n'a pas encore prevu de */
		     /* paves de presentation pour les colonnes */
		     while (pP->AbNext != NULL
			    && pP->AbNext->AbPresentationBox
			    && pP->AbNext->AbElement == pP->AbElement)
			pP = pP->AbNext;
		     NouvPave->AbNext = pP->AbNext;	/* NULL ? */
		     pP->AbNext = NouvPave;
		     NouvPave->AbPrevious = pP;	/* A FINIR ?? je ne sais quoi ! */
		  }
	     else
							/* cas !EnAvant */ if (pP != NULL)
							/* tj vrai ? */
	       {
		  /* chainage a pP en sautant ses eventuels paves de pres */
		  /* remarque : dans le cas des colonnes pP->AbPrevious */
		  /* est normalement null car on n'a pas encore prevu de */
		  /* paves de presentation pour les colonnes */
		  while (pP->AbPrevious != NULL
			 && pP->AbPrevious->AbPresentationBox
			 && pP->AbPrevious->AbElement == pP->AbElement)
		     pP = pP->AbPrevious;
		  /* chainage devant pP */
		  if (pP->AbEnclosing->AbFirstEnclosed == pP)
		     /* tj vrai ? */
		    {
		       NouvPave->AbNext = pP;
		       pP->AbEnclosing->AbFirstEnclosed = NouvPave;
		       pP->AbPrevious = NouvPave;
		    }
		  /* autres cas a considerer ? */
	       }
	     /* autres cas a considerer ? */
	  }			/* fin chainage dans cas !premcol */

	/* code de duplication des paves */
	/* si premiere page creee du document, c.a.d. pas de page precedente */
	/* dans le sens du parcours : dans ce cas pas de duplication */
	/* si EnAvant et premiere colonne simple ou groupee: pas de dup */
	/* si !EnAvant et derniere colonne simple ou groupee: pas de dup */
	/* de meme, si la page a creer est vide : la feuille suivante */
	/* est une marque page ; pas de duplication */
	/* si !EnAvant, pEl contient la marque de page precedente */
	/* donc on recherche dans tous les cas (EnAvant ou !EnAvant) */
	/* si la feuille suivante est une Marque page de la meme VueSch */
	/* TODO il faudrait faire une boucle pour chercher la feuille */
	/* suivante en sautant celles qui ne sont pas de la meme vue */
	pEl1 = FeuilleSuiv (pEl);
	PavPagePrec = RechPavPageCol (pEl, VueNb, VueSch, EnAvant);
	/* on descend la hierarchie des paves page et col */
	if (PavPagePrec != NULL)
	   while (PavPagePrec->AbFirstEnclosed != NULL
		  && PavPagePrec->AbFirstEnclosed->AbElement->ElTypeNumber == PageBreak + 1)
	     {
		PavPagePrec = PavPagePrec->AbFirstEnclosed;
		while (PavPagePrec->AbPresentationBox)		/* on saute les paves de pres */
		   PavPagePrec = PavPagePrec->AbNext;
		/* on est sur que dans la chaine un pave n'est pas pres, donc */
		/* en sortant de la boucle PavPagePrec n'est pas null */
	     }
	/* si le document est mis en colonnes, PavPavPrec est toujours un pave de col */
	if ((!premcol) && PavPagePrec != NULL
	    && !(pEl1 != NULL && pEl1->ElTypeNumber == PageBreak + 1
		 && pEl1->ElViewPSchema == VueSch
		 && (pEl1->ElPageType == PgBegin
		     || pEl1->ElPageType == PgComputed
		     || pEl1->ElPageType == PgUser)))
	  {
	     /* cas de duplication */
	     /* si !EnAvant, il faut dupliquer depuis l'element rencontre */
	     /* (celui sauvegarde dans pElSauv) jusqu'a la racine */
	     /* vrai aussi pour les colonnes simples et groupees */
	     if (!EnAvant)
		if (pElSauv != NULL)
		  {
		     pEl1 = pEl;
		     pEl = pElSauv;
		     pElSauv = pEl1;
		  }

	     /* duplication du chemin depuis le pave de l'element pere
	        de la marque de page jusqu'au pave de l'element racine
	        non compris (boucle ascendante) */
	     pPere = pEl->ElParent;	/* pere de la marque de page */
	     pDupPav = NULL;
	     pPavDupFils = NULL;
	     adupliquer = False;
	     filsdup = (pEl->ElPrevious == NULL && pEl->ElNext == NULL);
	     /* code change pour traiter le cas des colonnes vides */
	     /* cas particulier ou la page precedente est une page vide */
	     /* il faut rechercher la suite des paves a dupliquer dans */
	     /* la premiere page suivante ou precedente non vide */
	     stop = False;
	     while (!stop && PavPagePrec != NULL)
		/* boucle sur les pages ou les colonnes vides */
		if (PavPagePrec->AbFirstEnclosed != NULL)
		   stop = True;
		else
		  {
		     pP = PavPagePrec;
		     PavPagePrec = NULL;	/* a priori, pas de precedent (suivant) */
		     /* on saute les paves de presentation associes a PavPagePrec */
		     while ((EnAvant && pP->AbPrevious != NULL
			     && pP->AbPrevious->AbPresentationBox)
			    || (!EnAvant && pP->AbNext != NULL
				&& pP->AbNext->AbPresentationBox))
			if (EnAvant)
			   pP = pP->AbPrevious;
			else
			   pP = pP->AbNext;
		     /* ici, pP est soit PavPagePrec soit un pave de presentation */
		     /* dont le pave suivant ou precedent s'il existe n'est pas de pres */
		     /* on passe au pave page ou colonne suivant (precedent) */
		     /* s'il existe */
		     if (EnAvant && pP->AbPrevious != NULL)
			PavPagePrec = pP->AbPrevious;
		     else if (!EnAvant && pP->AbNext != NULL)
			PavPagePrec = pP->AbNext;
		     /* s'il n'existe pas comme voisin, on cherche dans la branche */
		     /* precedente (ou suivante) */
		     else
			/* cas des colonnes */ if (pP->AbEnclosing->AbElement->ElTypeNumber == PageBreak + 1)
		       {
			  pP = pP->AbEnclosing;
			  if (EnAvant && pP->AbPrevious != NULL)
			     pP = pP->AbPrevious;
			  else if (!EnAvant && pP->AbNext != NULL)
			     pP = pP->AbNext;
			  else
			     /* pas de precedent (suivant) */
			     /* on remonte d'un dernier niveau */ if (pP->AbEnclosing->AbElement->ElTypeNumber == PageBreak + 1)
			     if (EnAvant && pP->AbPrevious != NULL)
				pP = pP->AbPrevious;
			     else if (!EnAvant && pP->AbNext != NULL)
				pP = pP->AbNext;
			     else
				stop = True;
			  else	/* on est au plus haut niveau (page) */
			     stop = True;
			  if (!stop)
			     /* on a trouve un pave de structure physique */
			     /* d'une branche precedente (ou suivante) */
			     /* on redescend pour trouver le pave de */
			     /* structure physique de plus bas niveau */
			    {
			       while (pP->AbFirstEnclosed != NULL
				      && pP->AbFirstEnclosed->AbElement->ElTypeNumber == PageBreak + 1)
				 {
				    pP = pP->AbFirstEnclosed;
				    if (EnAvant)	/* on va sur le dernier fils */
				      {
					 while (pP->AbNext != NULL)
					    pP = pP->AbNext;
					 while (pP->AbPresentationBox)
					    pP = pP->AbPrevious;	/* pP jamais NULL */
				      }
				    else
				       /* on reste sur le premier mais on saute les */
				       /* paves de presentation */
				       while (pP->AbPresentationBox)
					  pP = pP->AbNext;	/* pP jamais NULL */
				 }
			       PavPagePrec = pP;
			    }
		       }	/* fin englobant = marquepage */
		     else	/* cas pas de precedent et pas englobant = marquepage */
			stop = True;
		  }		/* fin  PavPagePrec->AbFirstEnclosed != NULL */
	     /* donc fin de la boucle pour sauter les pages et colonnes vides */

	     if (PavPagePrec != NULL)
		while (pPere != pElRacine)	/* on ne duplique pas */
		   /*l'element racine */
		  {
		     /* recherche du pave a dupliquer : 
		        c'est le pave dont AbElement = pPere et qui est dans
		        le chemin depuis PavPagePrec et le pave de l'element pere
		        de la marque de page. Parcours descendant */
		     /* Remarque : si le document est mis en colonnes, PavPagePrec est */
		     /* toujours une colonne */
		     pAb = PavPagePrec->AbFirstEnclosed;
		     if (pAb != NULL)
		       {
			  stop = False;
			  while (!stop && pAb != NULL)
			    {
			       if (EnAvant)
				  /* on se positionne sur le chemin a dupliquer : */
				  /* si EnAvant, c'est le chemin le plus a droite, */
				  /* donc le dernier fils qui n'est pas de presentation */
				 {
				    while (pAb->AbNext != NULL)
				       pAb = pAb->AbNext;
				    /* on recule de facon a trouver le dernier pave */
				    /* qui n'est pas de presentation */
				    while (pAb != NULL && pAb->AbPresentationBox)
				       pAb = pAb->AbPrevious;
				 }
			       else
				  /* si !EnAvant, c'est le chemin le plus a gauche, */
				  /* donc le premier fils qui n'est pas de presentation */
				  while (pAb != NULL && pAb->AbPresentationBox)
				     pAb = pAb->AbNext;
			       /* on est sur un pave d'element, est-ce le pave a dupliquer? */
			       if (pAb != NULL)
				  if (pAb->AbElement == pPere)
				     stop = True;	/* on a trouve le pave a dupliquer */
				  else
				     /* on descend d'un niveau dans la hierarchie */
				     pAb = pAb->AbFirstEnclosed;
			       else
				  /*pAb = NULL */
				 {
				    printf ("erreur i.a.  rech. pave a duplique \n");
				    printf ("on n'a pas trouve le pave a dupliquer \n");
				    AffPaveDebug (pPere->ElAbstractBox[VueNb - 1]);
				 }
			    }	/* fin while */
		       }	/* fin recherche pave a dupliquer */
		     if (pAb != NULL)
			/* on cherche si il faut dupliquer pAb */
			/* on ne duplique pas les paves pere de la marque de page */
			/* si la marque n'a pas de frere suivant ou precedent */
			/* ce "ou" est exclusif : si la marque ou ses ascendants */
			/* sont des descendants uniques on les duplique. */
			/* c'est detecte par filsdup */
		       {
			  adupliquer = filsdup;
			  pEl1 = pEl;
			  if (!adupliquer)
			    {
			       /* on saute les marques colonnes de niveau inferieur */
			       /* ou d'une autre vue */
			       if (EnAvant)
				  if (pEl->ElPageType == PgBegin
				      || pEl->ElPageType == PgComputed
				      || pEl->ElPageType == PgUser)
				     while (pEl1->ElNext != NULL &&
					    pEl1->ElNext->ElTypeNumber == PageBreak + 1 &&
				     (pEl1->ElNext->ElPageType == ColBegin
				      || pEl1->ElNext->ElPageType == ColComputed
				      || pEl1->ElNext->ElPageType == ColUser
				      || pEl1->ElNext->ElPageType == ColGroup
				      || pEl1->ElNext->ElViewPSchema != VueSch))
					pEl1 = pEl1->ElNext;
				  else if (pEl->ElPageType == ColGroup)
				     while (pEl1->ElNext != NULL &&
					    pEl1->ElNext->ElTypeNumber == PageBreak + 1 &&
				     (pEl1->ElNext->ElPageType == ColBegin
				      || pEl1->ElNext->ElPageType == ColComputed
				      || pEl1->ElNext->ElPageType == ColUser
				      || pEl1->ElNext->ElViewPSchema != VueSch))
					pEl1 = pEl1->ElNext;
				  else	/* on saute uniquement les marques des autres vues */
				     while (pEl1->ElNext != NULL
					    && pEl1->ElNext->ElTypeNumber == PageBreak + 1
				     && pEl1->ElNext->ElViewPSchema != VueSch)
					pEl1 = pEl1->ElNext;
			       else	/* !EnAvant, quelque soit pEl, */
				  /* on saute uniquement les marques des autres vues */
				  while (pEl1->ElPrevious != NULL &&
					 pEl1->ElPrevious->ElTypeNumber == PageBreak + 1
				    && pEl1->ElNext->ElViewPSchema != VueSch)
				     pEl1 = pEl1->ElPrevious;
			       /* on remonte l'arbre tant qu'on n'a rien a dupliquer */
			       while (pEl1 != pAb->AbElement && !adupliquer)
				  if (EnAvant && pEl1->ElNext != NULL
				      && pEl1->ElNext->ElTypeNumber != PageBreak + 1)
				     adupliquer = True;
				  else if (!EnAvant && pEl1->ElPrevious != NULL
					   && pEl1->ElPrevious->ElTypeNumber != PageBreak + 1)
				     adupliquer = True;
				  else
				     pEl1 = pEl1->ElParent;	/* jamais NULL */
			    }
		       }
		     if (pAb != NULL && adupliquer)
		       {
			  /* duplication et chainage avec les paves voisins */
			  /* tous les champs du pave sont dupliques dans CopiePav sauf
			     ceux des positions, dimensions et changements */
			  pDupPav = CopiePav (pAb);
			  if (EnAvant)
			    {
			       pAb->AbTruncatedTail = True;	/* pAb coupe en queue */
			       pDupPav->AbTruncatedHead = True;	/* pDupPav coupe en tete */
			       /* pDupPav->AbTruncatedTail = pAb->AbTruncatedTail */
			       /* normalement AbTruncatedTail est vrai */
			       if (pAb->AbNextRepeated != NULL)
				  /* erreur */
				 {
				    printf ("erreur i.a. pave a dupliquer deja dup \n");
				    printf ("pAb->AbNextRepeated devrait etre NULL \n");
				    AffPaveDebug (pAb);
				 }
			       pAb->AbNextRepeated = pDupPav;
			       pDupPav->AbPreviousRepeated = pAb;
			       /* il ne faut pas recopier les champs */
			       /* AbPrevious, AbFirstEnclosed car ce sont des paves */
			       /* bien positionnes */
			    }
			  else
			    {
			       pAb->AbTruncatedHead = True;	/* pAb coupe en tete */
			       pDupPav->AbTruncatedTail = True;	/*pDupPav coupe en queue */
			       /* pDupPav->AbTruncatedHead = pAb->AbTruncatedHead */
			       /* normalement AbTruncatedHead est vrai */
			       if (pAb->AbPreviousRepeated != NULL)
				  /* erreur */
				 {
				    printf ("erreur i.a.  pave a dupliquer deja dup\n");
				    printf (" pAb->AbNextRepeated devrait etre NULL\n");
				    AffPaveDebug (pAb);
				 }
			       pDupPav->AbPreviousRepeated = pAb->AbPreviousRepeated;
			       pAb->AbPreviousRepeated = pDupPav;
			       pDupPav->AbNextRepeated = pAb;
			       /* l'element pointe maintenant sur ce nouveau pave */
			       pPere->ElAbstractBox[VueNb - 1] = pDupPav;
			    }
			  /* cas ou le saut de page a cree un pave compose vide : */
			  /* pAb est un pave d'element compose vide sauf */
			  /* des paves de presentation. Il faut rechainer les paves */
			  /* de presentation, rechainer l'element au pDupPav cree, */
			  /* et detruire le pave pAb. */
			  /* sauf si l'element de pAb n'a qu'un seul fils */
			  adetruire = False;
			  if (!pAb->AbElement->ElTerminal
			      && (pAb->AbElement->ElFirstChild != NULL	/* tj vrai ! */
				  && pAb->AbElement->ElFirstChild->ElNext != NULL))
			    {
			       /* cherche si le pave a des fils qui ne sont pas des */
			       /* paves de presentation, par exemple, les paves des */
			       /* elements freres de la marque de page si deja crees */
			       /* meme code si EnAvant et !EnAvant */
			       trouve = False;	/* pas de pave d'autres elements */
			       pP = pAb->AbFirstEnclosed;
			       while (pP != NULL && !trouve)
				  if (pP->AbPresentationBox || pP->AbDead)
				     pP = pP->AbNext;
				  else
				     trouve = True;
			       if (!trouve)
				  adetruire = True;
			    }
			  if (adetruire)
			    {
			       TuePave (pAb);
			       /* recherche du premier pave avant pAb */
			       pP = pAb;
			       stop = False;
			       do
				  if (pP->AbPrevious == NULL)
				     stop = True;
				  else if (pP->AbPrevious->AbElement !=
					   pAb->AbElement)
				     stop = True;
				  else
				     pP = pP->AbPrevious;
			       while (!stop);
			       pPAvant = pP;
			       /* recherche du dernier pave apres pAb */
			       pP = pAb;
			       stop = False;
			       do
				  if (pP->AbNext == NULL)
				     stop = True;
				  else if (pP->AbNext->AbElement !=
					   pAb->AbElement)
				     stop = True;
				  else
				     pP = pP->AbNext;
			       while (!stop);
			       pPApres = pP;
			       /* chainage des paves de presentation a pDupPav */
			       if (pPAvant != pAb)	/* il y a des paves pres avant */
				 {
				    pP = pPAvant;
				    while (pP->AbNext != pAb)
				       pP = pP->AbNext;
				    pP->AbNext = pDupPav;
				    pDupPav->AbPrevious = pP;
				 }
			       if (pPApres != pAb)	/* il y a des paves pres apres */
				 {
				    pP = pPApres;
				    while (pP->AbPrevious != pAb)
				       pP = pP->AbPrevious;
				    pP->AbPrevious = pDupPav;
				    pDupPav->AbNext = pP;
				 }
			       /* dechainage du pave pere (si besoin) */
			       if (pPAvant->AbEnclosing->AbFirstEnclosed == pPAvant)
				  pPAvant->AbEnclosing->AbFirstEnclosed =
				     pPApres->AbNext;
			       /* chainage de pPAvant au pere de pDupPav : fait */
			       /* lors de la prochaine boucle, pas encore de pere */
			       /* dechainage de la suite des paves du precedent */
			       if (pPAvant->AbPrevious != NULL)
				  pPAvant->AbPrevious->AbNext = NULL;
			       if (pPAvant != pAb)
				  pPAvant->AbPrevious = NULL;
			       /* dechainage de la suite des paves du suivant */
			       if (pPApres->AbNext != NULL)
				  pPApres->AbNext->AbPrevious = NULL;
			       if (pPApres != pAb)
				  pPApres->AbNext = NULL;
			       /* chainage des paves de pres fils sous pDupPav */
			       if (pAb->AbFirstEnclosed != NULL)
				 {
				    /* il y a des paves de presentation deja crees */
				    /* on les met sous le nouveau pave duplique */
				    pP = pAb->AbFirstEnclosed;
				    pDupPav->AbFirstEnclosed = pP;
				    do
				      {
					 pP->AbEnclosing = pDupPav;
					 pP = pP->AbNext;
				      }
				    while (pP != NULL);
				 }
			       /* traitements specifiques suivant en avant ou non */
			       if (EnAvant)
				 {
				    if (pAb->AbElement->ElAbstractBox[VueNb - 1] == pAb)
				       /* pas de pave de presentation avant */
				      {
					 /* chainage de pDupPav avec l'element */
					 pAb->AbElement->ElAbstractBox[VueNb - 1] = pDupPav;
					 pDupPav->AbTruncatedHead = True;
				      }
				    if (pAb->AbPreviousRepeated != NULL)
				       pAb->AbPreviousRepeated->AbNextRepeated = pDupPav;
				    pDupPav->AbPreviousRepeated = pAb->AbPreviousRepeated;
				 }	/* fin cas EnAvant */
			       else
				  /* cas !EnAvant */
				  /* plus simple car il ne faut pas considerer */
				  /* le chainage avec l'element */
				 {
				    if (pPAvant != pAb)
				       /* il y a des paves de presentation avant */
				       /* on  chaine pPAvant a l'element */
				       pAb->AbElement->ElAbstractBox[VueNb - 1] = pPAvant;
				    /* chainage avec les dupliques */
				    if (pAb->AbNextRepeated != NULL)
				       pAb->AbNextRepeated->AbPreviousRepeated =
					  pDupPav;
				    pDupPav->AbNextRepeated = pAb->AbNextRepeated;
				    /* mise a jour de AbTruncatedTail */
				    pDupPav->AbTruncatedTail = True;
				 }	/* fin cas !EnAvant */
			       /*reapplique les regles de pres des paves qui */
			       /* faisaient reference a pAb */
			       /* est-ce utile ? */
			       SuppRfPave (pAb, &pP, pDoc);
			       /* suppression du pave pAb, ne pas le signaler */
			       /* au mediateur s'il n'a pas ete affiche */
			       if (!pAb->AbNew)
				 {
				    /* ces paves ont deja ete vus par le mediateur */
				    /* il faut lui signaler leur destruction */
				    h = 0;
				    if (VueAssoc (pEl))
				       frame = pDoc->DocAssocFrame[pEl->ElAssocNum - 1];
				    else
				       frame = pDoc->DocViewFrame[VueNb - 1];
				    bool = ModifVue (frame, &h, pAb);
				 }
			       if (pAb->AbLeafType == LtPicture)
				 {
				    if (!pAb->AbElement->ElTerminal || pAb->AbElement->ElLeafType != LtPicture)
				       /* ce n'est pas un element image */
				       FreeImageDescriptor (pAb->AbImageDescriptor);
				    pAb->AbImageDescriptor = NULL;
				 }
			       FreePave (pAb);

			    }	/* fin cas a detruire */

			  /* chainage avec le pave pere et le pave fils s'il existe */
			  /* pPavDupFils est le premier pave de presentation avant */
			  if (pPavDupFils != NULL)
			    {
			       if (pDupPav->AbFirstEnclosed != NULL)
				 {
				    /* on a deja chaine des paves */
				    /* code plus complexe a cause des paves de pres */
				    /* repetes : il faut reperer la position du pave fils */
				    /* en regardant le type de regle des paves de pres */
				    /* fils de pDupPav */
				    pP = pDupPav->AbFirstEnclosed;
				    trouve = False;
				    pPAvant = NULL;
				    do
				      {
					 if (pP->AbPresentationBox && pP->AbElement ==
					     pDupPav->AbElement)
					   {
					      /* recherche la regle correspondant au pave */
					      /* de presentation pP ; */
					      pRCre = ReglePEl (pDupPav->AbElement,
								&pSPres, &pSchS, 0, NULL, VueSch,
								PtFunction, True, False, &pAttr);
					      stop = False;
					      do
						 if (pRCre == NULL)
						    stop = True;
						 else if (pRCre->PrType != PtFunction)
						    stop = True;
						 else
						    /* c'est une regle de creation */
						   {
						      pBo1 = &pSPres->PsPresentBox
							 [pRCre->PrPresBox[0] - 1];
						      if (pRCre->PrViewNum == VueSch
							  && pSPres == pDupPav->AbElement->
							  ElAbstractBox[VueNb - 1]->AbPSchema
							  && pP->AbTypeNum == pRCre->PrPresBox[0])
							{
							   /* on a trouve la regle  de pP */
							   stop = True;
							   /* regarde si c'est une regle de */
							   /* creation dernier fils */
							   if (pRCre->PrPresFunction == FnCreateLast)
							      trouve = True;
							   /* il faut inserer pPavDupFils avant pP */
							}
						      else
							 pRCre = pRCre->PrNextPRule;
						   }
					      while (!(stop || trouve));
					   }
					 else	/* pP n'est pas un pave de pres de l'elt */
					    fprintf ("erreur chainage dup \n");
					 if (!trouve)
					   {
					      pPAvant = pP;
					      pP = pP->AbNext;
					   }
				      }
				    while (!trouve && pP != NULL);
				    /* en sortie, pPAvant contient le dernier pave correspondant */
				    /* a une regle FnCreateFirst ; et pP contient le premier */
				    /* pave correspondant a une regle FnCreateLast */
				    /* on insere pPavDupFils entre pP et pPrec */
				    /* car il n'y a que des paves de pres premier fils */
				    if (pPAvant != NULL)
				      {
					 /* on insere pPavDupFils apres pPAvant */
					 pPAvant->AbNext = pPavDupFils;
					 pPavDupFils->AbPrevious = pPAvant;
				      }
				    if (pP != NULL)
				      {
					 pPavDupFils->AbNext = pP;
					 pP->AbPrevious = pPavDupFils;
				      }
				    /* si pP etait le premier fils de son pere, on chaine */
				    /* pPavDupFils a sa place */
				    if (pDupPav->AbFirstEnclosed == pP)
				       pDupPav->AbFirstEnclosed = pPavDupFils;
				 }	/* fin du cas ou pDupPav avait deja des fils */
			       else
				  pDupPav->AbFirstEnclosed = pPavDupFils;
			       /* chainage du (des) paves avec le pere */
			       pP = pPavDupFils;
			       while (pP != NULL &&
				  pP->AbElement == pPavDupFils->AbElement)
				 {
				    pP->AbEnclosing = pDupPav;
				    pP = pP->AbNext;
				 }
			    }	/* fin de 'PavDupFils non NULL */
			  /* pPavDupFils devient le premier pave de pres avant pDupPav */
			  /* pour boucle de duplication suivante */
			  pP = pDupPav;
			  while (pP->AbPrevious != NULL
				 && pP->AbPrevious->AbElement == pDupPav->AbElement)
			     pP = pP->AbPrevious;
			  pPavDupFils = pP;
		       }
		     filsdup = adupliquer;	/*pour savoir si le pere sera duplique */
		     pPere = pPere->ElParent;
		  }		/* fin boucle de duplication */
	     /* chainage du dernier pave duplique avec
	        le pave marque de page cree */
	     if (pPavDupFils != NULL)
	       {
		  /* chainage a l'englobant (y compris les paves de pres) */
		  /* pPavDupFils a ete positionne sur le 1er pave de pres */
		  pP1 = pPavDupFils;
		  /* pP1 est le premier pave de l'element pPavDupFils */
		  if (NouvPave->AbFirstEnclosed != NULL)
		     /* cas ou NouvPave a deja des paves fils (paves de presentation)
		      */
		     /* est-ce possible ? */
		    {
		       pP = NouvPave->AbFirstEnclosed;
		       while (pP->AbNext != NULL)
			  pP = pP->AbNext;
		       /* pP est le dernier fils, on chaine pP1 a pP */
		       pP->AbNext = pP1;
		       pP1->AbPrevious = pP;
		    }
		  else
		     /* le pave pP1 est le premier fils de NouvPave */
		     NouvPave->AbFirstEnclosed = pP1;
		  while (pP1 != NULL &&
			 pP1->AbElement == pPavDupFils->AbElement)
		    {
		       pP1->AbEnclosing = NouvPave;
		       pP1 = pP1->AbNext;
		    }
	       }

	     /* maintenant que le chainage est fini, on parcourt
	        le chemin cree pour appliquer les regles de dimension
	        et de position aux paves dupliques ; le parcours est
	        descendant a partir du dernier DupPave cree */
	     /* on applique aussi ces regles pour les paves de presentation */
	     pP = NouvPave->AbFirstEnclosed;
	     pDupPav = NULL;
	     while (pP != NULL)
	       {
		  for (TRegle = PtVertRef; TRegle <= PtHorizPos; TRegle++)
		    {
		       /* recherche de la regle */
		       pRegle = LaRegle (pDoc, pP, &pSPres, TRegle, True, &pAttr);
		       /* application de la regle */
		       if (!Applique (pRegle, pSPres, pP, pDoc, pAttr, &bool))
			  Retarde (pRegle, pSPres, pP, pAttr, NouvPave);
		    }
		  if (!pP->AbPresentationBox)
		    {
		       /* application des regles de creation repetees */
		       pRegle = LaRegle (pDoc, pP, &pSPres, PtFunction, True, &pAttr);
		       /* pRegle est la premiere regle de creation si il y en a */
		       do
			  if (pRegle != NULL)
			    {
			       if (pRegle->PrPresBoxRepeat
				   && (pRegle->PrPresFunction == FnCreateBefore
				       || pRegle->PrPresFunction == FnCreateFirst
				       || pRegle->PrPresFunction == FnCreateAfter
				       || pRegle->PrPresFunction == FnCreateLast))
				  if (pAttr == NULL)
				     PavPres = CrPavPres (pP->AbElement, pDoc, pRegle,
							  pP->AbElement->ElSructSchema, NULL, VueNb,
						       pSPres, False, True);
				  else
				     PavPres = CrPavPres (pP->AbElement, pDoc, pRegle,
					  pAttr->AeAttrSSchema, pAttr, VueNb,
							  pAttr->AeAttrSSchema->SsPSchema, False, True);
			       pRegle = pRegle->PrNextPRule;
			    }
		       while (pRegle != NULL && pRegle->PrType == PtFunction) ;
		    }
		  /* modification de la regle de position verticale ? */
		  /* il suffit de laisser PosAbRef a NULL car le mediateur */
		  /* appliquera cette regle par defaut */
		  /* on memorise le pave duplique (qui n'est pas de pres) */
		  if (!pP->AbPresentationBox)
		     pDupPav = pP;
		  /* on passe au pave suivant */
		  if (pP->AbNext == NULL)
		     if (pDupPav != NULL)
			/* on s'arrete si pP est le pave de presentation */
			/* fils du dernier duplique */
			/* on passe au duplique suivant */
		       {
			  pP = pDupPav->AbFirstEnclosed;
			  pDupPav = NULL;
		       }
		     else
			pP = NULL;
		  else
		     pP = pP->AbNext;
	       }		/* fin application des regles de positionnement */
	     /* on repositionne pEl et pElSauv si !EnAvant */
	     if (!EnAvant)
		if (pElSauv != NULL)
		  {
		     pEl1 = pEl;
		     pEl = pElSauv;
		     pElSauv = pEl1;
		  }
	  }			/* fin cas de duplication */
	/* on va appliquer les regles de presentation de la */
	/* boite page (pointees par TypeP), donc creer les */
	/* paves de presentation: haut, bas de page, filet */
	pPa1 = NouvPave;
	pPa1->AbAcceptLineBreak = True;
	pPa1->AbAcceptPageBreak = True;
	pPa1->AbInLine = False;
	pPa1->AbNotInLine = False;
	pPa1->AbTruncatedHead = False;
	pPa1->AbTruncatedTail = False;
	/* premiere regle de presentation de la boite page */
	pRSpecPage = pSchPPage->PsPresentBox[TypeP - 1].PbFirstPRule;
	/* premiere regle de presentation par defaut */
	pRDefPage = pSchPPage->PsFirstDefaultPRule;

	/* application des regles de la boite marque de page */
	/* ou colonne */

	/* code de creation des boites haut et bas de page */
	do
	  {
	     pRegle = GetRule (&pRSpecPage, &pRDefPage, pEl, NULL,
			       pEl->ElSructSchema);
	     /* pointeur sur la regle a appliquer pour la vue 1 */
	     if (pRegle != NULL)
		if (pRegle->PrType == PtFunction
		/* parmi les fonctions, on ne traite que les */
		/* regles CreateBefore et CreateAfter */
		    && (pRegle->PrPresFunction == FnCreateBefore
			|| pRegle->PrPresFunction == FnCreateAfter))
		   PavPres = CrPavPres (pEl, pDoc, pRegle,
					pEl->ElSructSchema,
					NULL, VueNb, pSchPPage,
					False, True);
		else
		   /* ce n'est pas une fonction de presentation */
		   /* cherche et applique la regle de meme type */
		   /* pour la vue */
		   /* on n'applique pas la regle de visibilite */
		   /* car si !EnAvant l'elt pere n'est pas cree */
		   /* et la regle donne 0 si visib = visib(pere) */

		if (pRegle->PrType != PtVisibility)
		   for (nv = 1; nv <= MAX_VIEW; nv++)
		     {
			if (nv == 1)
			   pRegleV = NULL;
			else
			   pRegleV = GetRuleView (&pRSpecPage,
					     &pRDefPage, pRegle->PrType, nv,
					       pEl, NULL, pEl->ElSructSchema);
			if (nv == VueSch &&
			    VueExiste (pEl, pDoc, VueNb))
			  {
			     if (pRegleV == NULL)
				pRegleV = pRegle;
			     if (!Applique (pRegleV, pSchPPage,
					    NouvPave, pDoc, NULL, arret))
				Attente (pRegleV, NouvPave, pSchPPage,
					 NULL, queuePA, queuePS,
					 queuePP, queuePR, lqueue);

			     else
				/* la regle a ete appliquee */
				/* on teste arret pour savoir si une le pave a ete */
				/* detruit suite a l'application d'une regle */
				/* page : il faut sortir de CreePaves pour */
				/* recommencer la creation des fils du pere de pEl */
				/* car un fils marque page a ete ajoute avant pEl */
				/* arret est un parametre retour de */
								/* CreePageCol */ if (*arret)
								/* la destruction de NouvPave a ete faite */
				return;		/* on sort de CreePageCol */
			     /* TODO : est-ce correct ? y-a-il des regles */
			     /* en attente ou retardees ??? */
			  }
		     }		/* fin du for nv =1 */
	  }
	while (!(pRegle == NULL));
	/* si pEl est une marque de page, */
	/* on calcule la hauteur du haut et du bas de la page */
	/* et HauteurCoupPage qui sont des variables */
	/* globales (page.var) */
	if (PaginationEnCours && (pEl->ElPageType == PgBegin
				  || pEl->ElPageType == PgComputed
				  || pEl->ElPageType == PgUser))
	  {
	     if (VueAssoc (pEl))
		frame = pDoc->DocAssocFrame[pEl->ElAssocNum - 1];
	     else
		frame = pDoc->DocViewFrame[VueNb - 1];
	     h = -1;		/* changement de signification de h */
	     /* on signale au mediateur les paves */
	     /* (au cas ou il ne les ait pas encore vus) */
	     /* on appelle Modifvue */
	     bool = ModifVue (frame, &h, PavRacine);
	     pP = pEl->ElAbstractBox[VueNb - 1];
	     if (pP->AbPresentationBox)
	       {
		  /* il y a un haut de page : on evalue sa hauteur */
		  pAb = pP;
		  /* calcul de la hauteur du pave haut de page */
		  HautCoupure (pAb, True, &Hauteur, &PosV, &CarCoupe);
		  HauteurHautPage = Hauteur;
	       }
	     /* on saute le corps de page pour voir s'il y a un bas de page */
	     while (pP->AbPresentationBox)
		pP = pP->AbNext;
	     /* pP est le corps de page */
	     if (pP->AbNext != NULL
		 && pP->AbNext->AbElement == pEl
		 && pP->AbNext->AbLeafType == LtCompound)
		/* pas filet */
	       {
		  /* il y a un bas : on evalue sa hauteur */
		  pAb = pP->AbNext;
		  /* calcul de la hauteur du pave bas de page */
		  HautCoupure (pAb, True, &Hauteur, &PosV, &CarCoupe);
		  HauteurBasPage = Hauteur;
	       }
	     if (pEl->ElPageType == PgBegin)
		/* nouvelle regle :on met a jour la hauteur totale de la page */
		/* hauteur corps = pP->AbHeight.DimValue toujours exprime en  */
		/* unite fixe (verifie par le compilo) */
		HauteurTotalePage = HauteurHautPage +
		   pP->AbHeight.DimValue + HauteurBasPage;
	     /* HauteurTotalePage = hauteur max totale de la page */
	     /* definie par l'utilisateur ; exemple A4 = 29.7 cm */
	     HauteurCoupPage = HauteurTotalePage - HauteurBasPage;
	  }			/* fin init HauteurTotalePage si PaginationEnCours */

	/* Applique les regles de presentation specifiques */
	/* de cet element */
	pRegle = pEl->ElFirstPRule;
	while (pRegle != NULL)
	   /* applique une regle si elle concerne la vue */
	  {
	     if (pRegle->PrViewNum == VueSch && NouvPave != NULL
		 && VueExiste (pEl, pDoc, VueNb))
		/* on ne considere pas les attributs */
		if (!Applique (pRegle, pSchPPage, NouvPave, pDoc, NULL, &bool))
		   Attente (pRegle, NouvPave, pSchPPage, NULL, queuePA,
			    queuePS, queuePP, queuePR, lqueue);
	     pRegle = pRegle->PrNextPRule;
	  }
	pPavD1 = &pPa1->AbHeight;
	/* en attendant que le compilateur genere la regle de */
	/* dimension minimale, on le fait a la main : la */
	/* valeur generee par la regle doit etre interpretee */
	/* par le mediateur comme minimale */
	pPavD1->DimMinimum = True;

	/* le contenu du filet de changement de page */
	/* est engendre directement par un pave de presentation */
	/* si le corps de page a un volume nul cas de page vide */
	/* on le force a 1 pour qu'il n'y ait pas de grise' */
	if (pPa1->AbVolume == 0)
	   pPa1->AbVolume = 1;
	if (!EnAvant && pElSauv != NULL &&
	    (pElSauv->ElPageType == PgBegin
	     || pElSauv->ElPageType == PgComputed
	     || pElSauv->ElPageType == PgUser))
	   /* si pElSauv est une marque page et si !EnAvant, il faut */
	   /* creer le pave ColGroup precedent pour permettre */
	   /* la creation des colonnes a placer dedans */
	  {
	     stop = False;
	     pEl1 = pElSauv;
	     while (pEl1 != NULL && !stop)
	       {
		  pEl1 = BackSearchTypedElem (pEl1, PageBreak + 1, NULL);
		  if (pEl1 != NULL)
		     /* on saute les marques des autres vues */
		     /* et les marques colonnes simples */
		     if (pEl1->ElViewPSchema == VueSch &&
			 (pEl1->ElPageType == ColGroup
			  || pEl1->ElPageType == PgBegin
			  || pEl1->ElPageType == PgComputed
			  || pEl1->ElPageType == PgUser))
			stop = True;
	       }
	     if (pEl1 != NULL && pEl1->ElPageType == ColGroup)
		/* si pEl1 est une marque ColGroup, c'est la derniere */
		/* de la page precedente : on cree son pave */
		/* pour permettre la suite de la creation */
		/* des colonnes situees avant pElSauv */
		pAb = CreePaves (pEl1, pDoc, VueNb, EnAvant, True,
				  &FilsComplet);
	  }
	if (!EnAvant && pEl->ElPageType == ColGroup)
	   /* si pEl est une ColGroup, */
	   /* il faut creer le pave de la derniere */
	   /* colonne simple de ce groupe de colonnes */
	   /* ceci est vrai meme si pElSauv != NULL */
	  {
	     stop = False;
	     pEl1 = pEl;
	     pPage = NULL;
	     while (pEl1 != NULL && !stop)
	       {		/* recherche en avant */
		  pEl1 = FwdSearchTypedElem (pEl1, PageBreak + 1, NULL);
		  if (pEl1 != NULL && pEl1->ElViewPSchema == VueSch)
		     if (pEl1->ElPageType == ColBegin
			 || pEl1->ElPageType == ColComputed
			 || pEl1->ElPageType == ColUser)
			pPage = pEl1;	/* on continue la recherche pour */
		  /* trouver la derniere colonne simple */
		     else	/* on est sur la marque Colgroupees suivante */
			/* ou sur la marque page suivante, donc l'element */
			/* stocke dans pPage est bien la derniere col */
			/* simple du groupe ;  on s'arrete */
			stop = True;
	       }
	     if (pPage != NULL)
		pAb = CreePaves (pPage, pDoc, VueNb, EnAvant, True,
				  &FilsComplet);
	  }
     }
}				/* end of CreePageCol */

#endif /* __COLPAGE__ */

/* end of module crpages.c */
