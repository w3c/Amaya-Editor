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
 * procedures liees a la creation des pages et des colonnes
 * appelees par crimabs pour la creation des images abstraites
 * contient quelques procedures du module crimabs
 *
 * Author: C. Roisin (INRIA)
 *
 */


#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"

#include "message.h"
#include "language.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "page_tv.h"
#include "platform_tv.h"
#include "edit_tv.h"

#include "tree_f.h"
#include "attributes_f.h"
#include "attrpresent_f.h"
#include "createabsbox_f.h"
#include "createpages_f.h"
#include "structlist_f.h"
#include "thotmsg_f.h"
#include "absboxes_f.h"
#include "abspictures_f.h"
#include "buildboxes_f.h"
#include "memory_f.h"
#include "structmodif_f.h"
#include "changeabsbox_f.h"
#include "presrules_f.h"
#include "boxpositions_f.h"
#include "schemas_f.h"
#include "presvariables_f.h"
#include "applicationapi_f.h"


#ifdef __COLPAGE__
/*----------------------------------------------------------------------
   CopyAbsBox retourne un pave qui est la duplication du pave pAb ;   
   tous les champs sont recopies sauf ceux de chainage,         
   de position, de dimension et de modification qui sont        
   initialises par InitAbsBoxes.                                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static PtrAbstractBox CopyAbsBox (PtrAbstractBox pAb)
#else  /* __STDC__ */
static PtrAbstractBox CopyAbsBox (pAb)
PtrAbstractBox      pAb;

#endif /* __STDC__ */
{

   PtrAbstractBox      pNewAbbox;
   PtrElement          pEl;
   DocViewNumber       viewNb;
   int                 Vis;

   pEl = pAb->AbElement;
   viewNb = pAb->AbDocView;
   Vis = pAb->AbVisibility;
   pNewAbbox = InitAbsBoxes (pEl, viewNb, Vis);
   /* les modifications des champs position et dimension seront
      faites lorsque le chainage sera complete */
   pNewAbbox->AbPSchema = pAb->AbPSchema;
   pNewAbbox->AbCreatorAttr = pAb->AbCreatorAttr;
   pNewAbbox->AbCopyDescr = pAb->AbCopyDescr;
   pNewAbbox->AbUnderline = pAb->AbUnderline;
   pNewAbbox->AbThickness = pAb->AbThickness;
   pNewAbbox->AbIndent = pAb->AbIndent;
   pNewAbbox->AbDepth = pAb->AbDepth;
   pNewAbbox->AbTypeNum = pAb->AbTypeNum;
   pNewAbbox->AbFont = pAb->AbFont;
   pNewAbbox->AbHighlight = pAb->AbHighlight;
   pNewAbbox->AbSize = pAb->AbSize;
   pNewAbbox->AbSizeUnit = pAb->AbSizeUnit;
   pNewAbbox->AbHorizEnclosing = pAb->AbHorizEnclosing;
   pNewAbbox->AbVertEnclosing = pAb->AbVertEnclosing;
   pNewAbbox->AbCanBeModified = pAb->AbCanBeModified;
   pNewAbbox->AbSelected = pAb->AbSelected;
   pNewAbbox->AbPresentationBox = pAb->AbPresentationBox;
   pNewAbbox->AbLineSpacing = pAb->AbLineSpacing;
   pNewAbbox->AbAdjust = pAb->AbAdjust;
   pNewAbbox->AbJustify = pAb->AbJustify;
   pNewAbbox->AbLineSpacingUnit = pAb->AbLineSpacingUnit;
   pNewAbbox->AbIndentUnit = pAb->AbIndentUnit;
   pNewAbbox->AbAcceptLineBreak = pAb->AbAcceptLineBreak;
   pNewAbbox->AbAcceptPageBreak = pAb->AbAcceptPageBreak;
   pNewAbbox->AbSensitive = pAb->AbSensitive;
   pNewAbbox->AbFillPattern = pAb->AbFillPattern;
   pNewAbbox->AbBackground = pAb->AbBackground;
   pNewAbbox->AbForeground = pAb->AbForeground;
   pNewAbbox->AbLineStyle = pAb->AbLineStyle;
   pNewAbbox->AbLineWeight = pAb->AbLineWeight;
   pNewAbbox->AbLineWeightUnit = pAb->AbLineWeightUnit;

   pNewAbbox->AbLeafType = pAb->AbLeafType;
   switch (pNewAbbox->AbLeafType)
	 {
	    case LtCompound:
	       pNewAbbox->AbInLine = pAb->AbInLine;
	       pNewAbbox->AbTruncatedHead = pAb->AbTruncatedHead;
	       pNewAbbox->AbTruncatedTail = pAb->AbTruncatedTail;
	       break;
	    case LtText:
	       pNewAbbox->AbText = pAb->AbText;
	       pNewAbbox->AbLanguage = pAb->AbLanguage;
	       break;
	    case LtSymbol:
	    case LtGraphics:
	       pNewAbbox->AbShape = pAb->AbShape;
	       pNewAbbox->AbGraphAlphabet = pAb->AbGraphAlphabet;
	       pNewAbbox->AbRealShape = pAb->AbRealShape;
	       break;
	    case LtPicture:
	       pNewAbbox->AbPictInfo = pAb->AbPictInfo;
	       break;
	    case LtPairedElem:
	    case LtPolyLine:
	    case LtReference:
	    case LtPageColBreak:
	       break;
	 }

   return pNewAbbox;
}
#endif /* __COLPAGE__ */

#ifdef __COLPAGE__
 /* nouvelle procedure du module crimabs, utilisee dans AbsBoxesCreate et Chaine */
/*----------------------------------------------------------------------
   RechPavPage retourne le premier pave corps de page qui          
   precede (si forward) ou suit (si non forward)           
   le pave corps de page cree pour pEl.                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PtrAbstractBox      RechPavPage (PtrElement pEl, DocViewNumber viewNb, int viewSch,
				 boolean forward)
#else  /* __STDC__ */
PtrAbstractBox      RechPavPage (pEl, viewNb, viewSch, forward)
PtrElement          pEl;
DocViewNumber       viewNb;
int                 viewSch;
boolean             forward;

#endif /* __STDC__ */
{

   PtrElement          pPage, pE;
   PtrAbstractBox      result, pAb;
   boolean             complete;

   pE = pEl;
   complete = FALSE;
   result = NULL;
   while (!complete)
     {
	if (forward)
	   /* on recherche dans l'arbre abstrait la marque de page precedente */
	   pPage = BackSearchTypedElem (pE, PageBreak + 1, NULL);
	else
	   /* on recherche dans l'arbre abstrait la marque de page suivante */
	   pPage = FwdSearchTypedElem (pE, PageBreak + 1, NULL);
	if (pPage == NULL)
	   /* c'est complete, on n'a pas trouve */
	   complete = TRUE;
	else if (pPage->ElViewPSchema == viewSch
		 && (pPage->ElPageType == PgBegin
		     || pPage->ElPageType == PgComputed
		     || pPage->ElPageType == PgUser))
	   /* on a trouve une marque de page precedente dans la vue */
	   /* il faut chercher parmi ses paves celui qui est un corps de page */
	   /* et non un pave filet, haut et bas */
	  {
	     pAb = pPage->ElAbstractBox[viewNb - 1];
	     if (pAb != NULL)
	       {
		  while (pAb != NULL && !complete && pAb->AbElement == pPage)
		     if (pAb->AbLeafType == LtCompound && !pAb->AbPresentationBox)
		       {
			  complete = TRUE;
			  result = pAb;
		       }
		     else
			/* c'est un pave filet, haut ou bas */
			/* il faut continuer la recherche */
			pAb = pAb->AbNext;
		  if (complete == FALSE)
		    {
		       printf ("ERR-CP : RechPavPage ");
		       complete = TRUE;
		    }
	       }
	     else
		complete = TRUE;
	  }
	else
	   /* ce n'est pas une marque page pour la vue, on continue */
	   pE = pPage;
     }
   return result;

}
#endif /* __COLPAGE__ */

#ifdef __COLPAGE__

 /* nouvelle procedure du module crimabs, utilisee dans AbsBoxesCreate  */
 /* et ApplyRule                                                    */
/*----------------------------------------------------------------------
   RechPavPageCol retourne le premier pave corps de page ou colonne
   qui precede (si forward) ou suit (si non forward)       
   le pave corps de page cree pour pEl                     
   Si il y a des colonnes sans pave, on les saute          
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static PtrAbstractBox RechPavPageCol (PtrElement pEl, DocViewNumber viewNb, int viewSch,
				      boolean forward)

#else  /* __STDC__ */
static PtrAbstractBox RechPavPageCol (pEl, viewNb, viewSch, forward)
PtrElement          pEl;
DocViewNumber       viewNb;
int                 viewSch;
boolean             forward;

#endif /* __STDC__ */

{

   PtrElement          pPage, pE;
   PtrAbstractBox      result, pAb;
   boolean             complete;

   pE = pEl;
   complete = FALSE;
   result = NULL;
   while (!complete)
     {
	if (forward)
	   /* on recherche dans l'arbre abstrait la marque de page precedente */
	   pPage = BackSearchTypedElem (pE, PageBreak + 1, NULL);
	else
	   /* on recherche dans l'arbre abstrait la marque de page suivante */
	   pPage = FwdSearchTypedElem (pE, PageBreak + 1, NULL);
	if (pPage == NULL)
	   /* c'est fini, on n'a pas trouve */
	   complete = TRUE;
	else if (pPage->ElViewPSchema == viewSch)
	   /* on a trouve une marque de page precedente dans la vue */
	   /* il faut chercher parmi ses paves celui qui est un corps de page */
	   /* et non un pave filet, haut et bas */
	  {
	     pAb = pPage->ElAbstractBox[viewNb - 1];
	     if (pAb != NULL)
	       {
		  while (pAb != NULL && !complete && pAb->AbElement == pPage)
		     if (pAb->AbLeafType == LtCompound && !pAb->AbPresentationBox)
		       {
			  complete = TRUE;
			  result = pAb;
		       }
		     else
			/* c'est un pave filet, haut ou bas */
			/* il faut continuer la recherche */
			pAb = pAb->AbNext;
		  if (complete == FALSE)
		    {
		       printf ("ERR-CP : RechPavPage ");
		       complete = TRUE;
		    }
	       }
	     else if (pPage->ElPageType == ColBegin
		      || pPage->ElPageType == ColComputed
		      || pPage->ElPageType == ColUser
		      || pPage->ElPageType == ColGroup)
		/* on continue la recherche pour trouver un pave */
		pE = pPage;
	     else
		complete = TRUE;
	  }
	else
	   /* ce n'est pas une marque page pour la vue, on continue */
	   pE = pPage;
     }
   return result;

}
#endif /* __COLPAGE__ */


/*----------------------------------------------------------------------
   GetPageBoxType cherche le type de boite page qui correspond a`       
   l'element Marque Page pointe par pEl pour la vue viewNb. 
    Rq : viewNb est un numero de vue de schema de pres       
   On cherche les regles Page des elements englobants      
   l'element Marque Page. Retourne le numero de boite      
   decrivant la page et le schema de presentation ou`      
   cette boite est definie.                                
    Attention : maintenant on place les marques page AVANT  
    l'element qui contient la regle page (sauf racine)      
    la recherche de la regle est donc changee               
  ----------------------------------------------------------------------*/

#ifdef __STDC__
int                 GetPageBoxType (PtrElement pEl, int viewNb, PtrPSchema * pSchPPage)

#else  /* __STDC__ */
int                 GetPageBoxType (pEl, viewNb, pSchPPage)
PtrElement          pEl;
int                 viewNb;
PtrPSchema         *pSchPPage;

#endif /* __STDC__ */

{
   PtrElement          pElAscent;
   int                 TypeP, index;
   PtrPRule            pRule;
   PtrSSchema          pSchS;
   boolean             stop;
   PtrPRule            pRe1;

#ifdef __COLPAGE__
   PtrElement          pNext;

#endif /* __COLPAGE__ */
   TypeP = 0;
   if (pEl != NULL)
     {
	pElAscent = pEl->ElParent;
	if (pElAscent != NULL)
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
		     pNext = pEl->ElNext;
		     /* on saute les eventuelles marques de colonnes */
		     /* ou de page (pour d'autres vues par exemple) */
		     while (pNext != NULL
			    && pNext->ElTypeNumber == PageBreak + 1)
			pNext = pNext->ElNext;
		     /* on cherche sur pPsuiv car normalement l'element */
		     /* marque page debut a ete place juste devant l'element qui */
		     /* portait la regle page correspondante */
		     if (pNext != NULL)
		       {
			  SearchPresSchema (pNext, pSchPPage, &index, &pSchS);
			  pRule = (*pSchPPage)->PsElemPRule[index - 1];
			  stop = FALSE;
			  do
			     if (pRule == NULL)
				stop = TRUE;
			     else if (pRule->PrType > PtFunction)
				stop = TRUE;
			     else
			       {
				  pRe1 = pRule;
				  if (pRe1->PrType == PtFunction
				      && pRe1->PrPresFunction == FnPage
				      && pRe1->PrViewNum == viewNb)
				     /* trouve', c'est une regle Page pour cette vue */
				    {
				       TypeP = pRe1->PrPresBox[0];
				       stop = TRUE;
				       /* l'element Marque Page appartient au meme schema */
				       /* de structure que l'element qui porte la regle  */
				       /* Page. Ainsi, on utilisera le bon schema de */
				       /* presentation pour construire */
				       /* le contenu de l'element Marque Page */
				       pEl->ElStructSchema = pNext->ElStructSchema;
				    }
				  else
				     pRule = pRe1->PrNextPRule;
				  /* passe a la regle suivante */
			       }
			  while (!stop);
		       }	/* fin pNext != NULL */
		  }
	     /* si on n'a rien trouve, on fait la recherche sur les ascendants */
	     /* ou si ce n'est pas une regle de debut */
	     /* ou si le pere est la racine */
	     if (TypeP == 0)
#endif /* __COLPAGE__ */
		do
		   /* cherche une regle Page parmi les regles de */
		   /* presentation de l'element pElAscent */
		  {
		     SearchPresSchema (pElAscent, pSchPPage, &index, &pSchS);
		     pRule = (*pSchPPage)->PsElemPRule[index - 1];
		     stop = FALSE;
		     do
			if (pRule == NULL)
			   stop = TRUE;
			else if (pRule->PrType > PtFunction)
			   stop = TRUE;
			else
			  {
			     pRe1 = pRule;
			     if (pRe1->PrType == PtFunction
				 && pRe1->PrPresFunction == FnPage
				 && pRe1->PrViewNum == viewNb)
				/* trouve', c'est une regle Page pour cette vue */
			       {
				  TypeP = pRe1->PrPresBox[0];
				  stop = TRUE;
				  /* l'element Marque Page appartient au meme schema */
				  /* de structure que l'element qui porte la regle  */
				  /* Page. Ainsi, on utilisera le bon schema de */
				  /* presentation pour construire */
				  /* le contenu de l'element Marque Page */
#ifdef __COLPAGE__
				  if (pEl->ElTypeNumber == PageBreak + 1)
#endif /* __COLPAGE__ */
				     pEl->ElStructSchema = pElAscent->ElStructSchema;
			       }
			     else
				pRule = pRe1->PrNextPRule;
			     /* passe a la regle suivante */
			  }
		     while (!stop);
		     pElAscent = pElAscent->ElParent;
		     /* passe a l'element ascendant */
		  }
		while (TypeP == 0 && pElAscent != NULL);
	  }
     }				/* fin pEl != NULL */
   return TypeP;
}

#ifdef __COLPAGE__

/**CL*//**CL*//**CL*//**CL*//**CL*//**CL*//**CL*//**CL*//**CL*//**CL*/
/* nouvelle procedure pour les colonnes *//**CL*/
/*----------------------------------------------------------------------
   TypeBCol cherche le type de boite colonne qui correspond a`     
   l'element Marque Page (de type colonne) pointe par pEl  
   pour la vue viewNb.                                      
    Rq : viewNb est un numero de vue de schema de pres       
   On cherche les regles Colonnes des elements englobants  
   l'element Marque Page (de type colonne) ainsi que dans  
   les  elements Marque Page qui precedent pEl             
   Retourne le numero de boite                            
   decrivant la colonne et le schema de presentation ou`   
   cette boite est definie.                                
    Attention :  on place les marques colonnes AVANT       
    l'element qui contient la regle Column (sauf racine et MP)
   on parcourt l'arbre abstrait jusqu'a rencontrer une regle 
   Column ou une regle Page contenant une regle Column.    
   retourne dans NbCol, le nombre de colonnes de la regle  
   Column 
  ----------------------------------------------------------------------*/

#ifdef __STDC__
int                 TypeBCol (PtrElement pEl, int viewNb, PtrPSchema * pSchPPage, int *NbCol)

#else  /* __STDC__ */
int                 TypeBCol (pEl, viewNb, pSchPPage, NbCol)
PtrElement          pEl;
int                 viewNb;
PtrPSchema         *pSchPPage;
int                *NbCol;

#endif /* __STDC__ */

{
   PtrElement          pElAscent, pNext, pPrevious;
   int                 TypeP, TypeCol, index;
   PRuleType           typeRule;
   PtrPRule            pRule;
   PtrSSchema          pSchS;
   boolean             stop, beginDoc;
   PtrPRule            pRe1;

   TypeCol = 0;
   *NbCol = 0;
   if (pEl != NULL)
     {
	pElAscent = pEl->ElParent;
	/* on regarde si c'est une regle de debut de document : si c'est un */
	/* element marque page ou colonne avant non precede d'un element */
	/* autre qu'une marque page ou colonne */
	/* TODO pb si l'element marque page ou colonne a ete place suite */
	/* a une regle page ou colonne du premier fils de la racine ! */
	beginDoc = FALSE;	/* a priori on n'est pas au debut du document */
	if (pEl->ElTypeNumber == PageBreak + 1
	    && (pEl->ElPageType == ColBegin || pEl->ElPageType == ColGroup)
	    && pElAscent->ElParent == NULL)
	  {
	     pPrevious = pEl->ElPrevious;
	     stop = FALSE;
	     while (!stop && !beginDoc)
		if (pPrevious == NULL)
		   beginDoc = TRUE;
		else if (pPrevious->ElTypeNumber != PageBreak + 1)
		   stop = TRUE;
		else
		   pPrevious = pPrevious->ElPrevious;
	  }

	if (pElAscent != NULL)	/* test bidon car toujours vrai ? */
	  {
	     /* si la marque page est une colonne debut ou groupees et que le pere */
	     /* n'est pas la racine, il faut rechercher la regle sur l'element suivant */
	     /* car les elements marque page debut sont places AVANT les elements */
	     /* qui portent la regle page ; sauf si la regle colonne est placee */
	     /* dans une boite page : la marque colonne se trouve alors apres */
	     /* la marque page */
	     if ((pEl->ElTypeNumber == PageBreak + 1)
		 && pEl->ElPageType == ColBegin)
		if (pElAscent->ElParent != NULL || !beginDoc)
		   if (pEl->ElNext != NULL)
		     {
			pNext = pEl->ElNext;
			/* on cherche uniquement sur pPsuiv car normalement l'element */
			/* marque page debut a ete place juste devant l'element qui */
			/* portait la regle page correspondante */
			/* on saute les eventuelles autres marques page (d'autres vues) */
			while (pNext != NULL && pNext->ElTypeNumber == PageBreak + 1)
			   pNext = pNext->ElNext;
			if (pNext != NULL)	/* pNext n'est pas une marque page */
			  {
			     SearchPresSchema (pNext, pSchPPage, &index, &pSchS);
			     pRule = (*pSchPPage)->PsElemPRule[index - 1];
			     stop = FALSE;
			     do
				if (pRule == NULL)
				   stop = TRUE;
				else if (pRule->PrType > PtFunction)
				   stop = TRUE;
				else
				  {
				     pRe1 = pRule;
				     if (pRe1->PrType == PtFunction
					 && pRe1->PrPresFunction == FnColumn
					 && pRe1->PrViewNum == viewNb)
					/* trouve', c'est une regle Colonne pour cette vue */
				       {
					  if (pEl->ElPageType == ColBegin)
					     TypeCol = pRe1->PrPresBox[1];	/* on prend la */
					  /* premiere boite colonne */
					  else	/* (la boite 0 est ColGroup) */
					     TypeCol = pRe1->PrPresBox[0];
					  *NbCol = pRe1->PrNPresBoxes - 1;
					  /* la boite ColGroup ne compte pas */
					  stop = TRUE;
					  /* l'element Marque Page appartient au meme schema */
					  /* de structure que l'element qui porte la regle  */
					  /* Page. Ainsi, on utilisera le bon schema de */
					  /* presentation pour construire */
					  /* le contenu de l'element Marque Page */
					  pEl->ElStructSchema = pNext->ElStructSchema;
				       }
				     else
					pRule = pRe1->PrNextPRule;
				     /* passe a la regle suivante */
				  }
			     while (!stop);
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
		     pPrevious = pEl->ElPrevious;
		     /* on saute les eventuelles autres marques page (d'autres vues) */
		     while (pPrevious != NULL && pPrevious->ElTypeNumber == PageBreak + 1
			    && (pPrevious->ElViewPSchema != viewNb
				|| pPrevious->ElPageType == ColBegin
				|| pPrevious->ElPageType == ColComputed
				|| pPrevious->ElPageType == ColGroup
				|| pPrevious->ElPageType == ColUser))
			pPrevious = pPrevious->ElPrevious;
		     if (pPrevious != NULL && pPrevious->ElTypeNumber == PageBreak + 1)
			/* on est bien sur un element Marque page de la meme vue */
		       {
			  /* on recherche si cet element porte une regle colonne */
			  /* on recherche la boite page correspondante */
			  TypeP = GetPageBoxType (pPrevious, viewNb, pSchPPage);
			  pRule = (*pSchPPage)->PsPresentBox[TypeP - 1].PbFirstPRule;
			  /* on recherche la regle colonne de la page */
			  typeRule = PtFunction;
			  SimpleSearchRulepEl (&pRule, pPrevious, viewNb, typeRule, FnColumn, &pRule);
			  stop = FALSE;
			  while (!stop && pRule != NULL)
			     if (pRule->PrType != PtFunction)
				pRule = NULL;
			     else if (pRule->PrPresFunction == FnColumn
				      && pRule->PrViewNum == viewNb)
			       {
				  TypeCol = pRule->PrPresBox[1];
				  *NbCol = pRule->PrNPresBoxes - 1;
				  /* la boite ColGroup ne compte pas */
				  stop = TRUE;
			       }
			     else
				pRule = pRule->PrNextPRule;
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
		   /* presentation de l'element pElAscent */
		  {
		     SearchPresSchema (pElAscent, pSchPPage, &index, &pSchS);
		     pRule = (*pSchPPage)->PsElemPRule[index - 1];
		     stop = FALSE;
		     do
			if (pRule == NULL)
			   stop = TRUE;
			else if (pRule->PrType > PtFunction)
			   stop = TRUE;
			else
			  {
			     pRe1 = pRule;
			     if (pRe1->PrType == PtFunction
				 && pRe1->PrPresFunction == FnPage
				 && pRe1->PrViewNum == viewNb)
				/* trouve', c'est une regle Page pour cette vue */
			       {
				  TypeP = pRe1->PrPresBox[0];
				  /* on recherche si cette boite page contient une regle */
				  /* colonne */
				  pRule = (*pSchPPage)->PsPresentBox[TypeP - 1]
				     .PbFirstPRule;
				  /* on recherche la regle colonne de la page */
				  typeRule = PtFunction;
				  SimpleSearchRulepEl (&pRule, pElAscent, viewNb, typeRule, FnColumn, &pRule);
				  while (!stop && pRule != NULL)
				     if (pRule->PrType != PtFunction)
					pRule = NULL;
				     else if (pRule->PrPresFunction == FnColumn
					      && pRule->PrViewNum == viewNb)
				       {
					  /* attention si ElPageNumber = 0, c'est la boite */
					  /* ColGroup, sinon c'est la colonne 1, 2, ... */
					  TypeCol = pRule->PrPresBox[pEl->ElPageNumber];
					  *NbCol = pRule->PrNPresBoxes - 1;
					  /* la boite ColGroup ne compte pas */
					  stop = TRUE;
					  pEl->ElStructSchema = pElAscent->ElStructSchema;
				       }
				     else
					pRule = pRule->PrNextPRule;
				  if (!stop)
				     /* on passe a la regle suivante de pElAscent */
				     pRule = pRe1->PrNextPRule;
			       }
			     else if (pRe1->PrType == PtFunction
				      && pRe1->PrPresFunction == FnColumn
				      && pRe1->PrViewNum == viewNb)
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
				  stop = TRUE;
				  /* l'element Marque Page appartient au meme schema */
				  /* de structure que l'element qui porte la regle */
				  /* Page. Ainsi, on utilisera le bon schema de */
				  /* presentation pour construire */
				  /* le contenu de l'element Marque Page */
				  pEl->ElStructSchema = pElAscent->ElStructSchema;
			       }
			     else
				pRule = pRe1->PrNextPRule;
			     /* passe a la regle suivante */
			  }
		     while (!stop);
		     pElAscent = pElAscent->ElParent;
		     /* passe a l'element ascendant */
		  }
		while (TypeCol == 0 && pElAscent != NULL);
	  }			/* fin pElAscent != NULL */
     }				/* fin pEl != NULL */
   return TypeCol;
}
/* fin TypeBCol */
#endif /* __COLPAGE__ */

/*----------------------------------------------------------------------
   GetPageCounter retourne le numero de compteur a` utiliser pour         
   numeroter la marque de page pointee par pEl, dans la    
   vue viewNb. Retourne egalement dans pSchPPage le schema  
   de presentation ou est defini ce compteur. Retourne 0 si
   cette page n'est pas numerotee.                         
	    viewNb = Vue dans le schema de presentation          
  ----------------------------------------------------------------------*/

#ifdef __STDC__
int                 GetPageCounter (PtrElement pEl, int viewNb, PtrPSchema * pSchPPage)

#else  /* __STDC__ */
int                 GetPageCounter (pEl, viewNb, pSchPPage)
PtrElement          pEl;
int                 viewNb;
PtrPSchema         *pSchPPage;

#endif /* __STDC__ */

{
   int                 bp;
   int                 cptpage;

   /* cherche d'abord la boite page */
   bp = GetPageBoxType (pEl, viewNb, pSchPPage);
   if (bp > 0)
      cptpage = (*pSchPPage)->PsPresentBox[bp - 1].PbPageCounter;
   else
      cptpage = 0;
   return cptpage;
}

#ifdef __COLPAGE__

 /*----------------------------------------------------------------------
    NbPages retourne le nombre de pages sous la racine pAb
   ----------------------------------------------------------------------*/


#ifdef __STDC__
int                 NbPages (PtrAbstractBox pAb)

#else  /* __STDC__ */
int                 NbPages (pAb)
PtrAbstractBox      pAb;

#endif /* __STDC__ */

{
   int                 nb;
   PtrAbstractBox      pAbb;

   nb = 0;
   pAbb = pAb->AbFirstEnclosed;
   while (pAbb != NULL)
     {
	/*     est-ce un pave corps de page ? */
	/*     on saute les marque page colonnes */
	if (pAbb->AbElement->ElTerminal
	    && pAbb->AbElement->ElLeafType == LtPageColBreak
	    && (pAbb->AbElement->ElPageType == PgBegin
		|| pAbb->AbElement->ElPageType == PgComputed
		|| pAbb->AbElement->ElPageType == PgUser)
	    && !pAbb->AbPresentationBox)
	   nb++;
	pAbb = pAbb->AbNext;
     }
   return nb;
}
#endif /* _COLPAGE__ */

#ifdef __COLPAGE__
/*----------------------------------------------------------------------
   PagePleine evalue si la page en cours de remplissage est pleine.
    dans le cas ou le document est pagine.                  
    si doc pagine, on s'arrete sur une page pleine : donc   
    si pEl, qui est le prochain element a traiter,          
    est une Marque Page: on positionne le volume            
    libre de la vue a -1 (teste ensuite par IsViewFull)      
    Cette procedure detruit les paves crees inutiles        
    (aux frontieres de page)                                        
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                PagePleine (DocViewNumber viewNb, PtrDocument pDoc, PtrElement pEl,
				boolean forward)

#else  /* __STDC__ */
void                PagePleine (viewNb, pDoc, pEl, forward)
DocViewNumber       viewNb;
PtrDocument         pDoc;
PtrElement          pEl;
boolean             forward;

#endif /* __STDC__ */

{
   boolean             full, complete;
   PtrElement          pElRoot, pEl1, pPage;
   PtrAbstractBox      pAbbRoot, pAbb, pAb, pAbbPage, pAbbDestroy, pAbbReDisp,
                       pAbbR;
   int                 bp, Nb, NbAvant, volsupp;
   boolean             assoc, found;
   PtrPSchema          pSchPPage;
   int                 viewSch;

   full = FALSE;
   pAbb = NULL;
   pAbbReDisp = NULL;
   assoc = AssocView (pEl);
   if (!assoc)
      /* ce n'est pas une vue d'elements associes */
     {
	complete = (pDoc->DocView[viewNb - 1].DvPSchemaView == 0);
	if (!complete)
	   pAbbRoot = pDoc->DocRootElement->ElAbstractBox[viewNb - 1];
     }
   else
      /* c'est une vue d'elements associes */
     {
	complete = (pDoc->DocAssocFrame[pEl->ElAssocNum - 1] == 0);
	if (!complete)
	  {
	     pElRoot = pDoc->DocAssocSubTree[pEl->ElAssocNum - 1];
	     if (pElRoot == NULL)
		pElRoot = pDoc->DocAssocRoot[pEl->ElAssocNum - 1];
	     if (pElRoot != NULL)
		pAbbRoot = pElRoot->ElAbstractBox[viewNb - 1];	/* viewNb est 1 */
	     else
	       {
		  complete = TRUE;
		  pAbbRoot = NULL;
	       }
	  }
     }

   if (!complete)
      if (pAbbRoot != NULL && pAbbRoot->AbFirstEnclosed != NULL)
	{
	   /* dans tous les cas (pagine ou non) on positionne full */
	   /* si le volume restant est negatif */
	   if (!assoc)
	      full = (pDoc->DocViewFreeVolume[viewNb - 1] <= 0);
	   else
	      full = (pDoc->
		      DocAssocFreeVolume[pEl->ElAssocNum - 1] < 0);
	   /* si la vue n'est pas pleine, on regarde si le document */
	   /* est pagine (pour mettre a jour le volume si le nombre de */
	   /* pages a augmente */
	   if (!full)
	      /* on determine si le document est pagine */
	      /* c'est-a-dire si un ascendant a une regle page (bp non nul) */
	     {
		/* on cherche la vue (de schema) a appliquer pour pEl */
		/* car elle peut etre differente de son pere d'ou vient */
		/* l'appel de IsViewFull */
		viewSch = AppliedView (pEl, NULL, pDoc, viewNb);
		bp = GetPageBoxType (pEl, viewSch, &pSchPPage);
		if (bp != 0)
		  {
		     if (!assoc)
			NbAvant = pDoc->DocViewNPages[viewNb - 1];
		     else
			NbAvant = pDoc->DocAssocNPages[pEl->ElAssocNum - 1];
		     /* si NbAvant est negatif, on s'arrete uniquement */
		     /* sur le volume, on ne considere pas les pages */
		     /* utilise' pour la pagination */
		     if (NbAvant >= 0)
		       {
			  Nb = NbPages (pAbbRoot);
			  /* on s'arrete si on a cree une page en plus et */
			  /* si la page est pleine : le prochain element a traiter */
			  /* est une marque de page pour la vue consideree */
			  /* pour s'arreter, on met un volume negatif */
			  if (Nb > NbAvant + 1
			      && (pEl->ElTypeNumber == (PageBreak + 1)
				  && pEl->ElViewPSchema == viewSch)
			      && (pEl->ElPageType == PgBegin
				  || pEl->ElPageType == PgComputed
				  || pEl->ElPageType == PgUser))
			     /* cas particulier : premiere et derniere page */
			     /* il ne faut pas s'arreter pour creer les paves */
			     /* de presentation de la racine */
			    {
			       found = FALSE;
			       pEl1 = pEl;
			       while (!found && pEl1 != NULL)
				 {
				    if (forward)
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
					&& pPage->ElViewPSchema == viewSch)
				       found = TRUE;
				    else
				       pEl1 = pPage;
				 }
			       if (found)
				  /* ce n'est pas le premiere ou derniere page */
				 {
				    /* si la page precedente est une page vide, */
				    /* on continue */
				    pAbbPage = RechPavPage (pEl, viewNb,
							  viewSch, forward);
				    while (pAbbPage != NULL &&
					   pAbbPage->AbPresentationBox)
				       pAbbPage = pAbbPage->AbNext;
				    if (pAbbPage != NULL && pAbbPage->AbVolume > 1)
				       /* ce n'est ni une page d'extremite, */
				       /* ni une page qui suit une page vide */
				       /* cas page interieure dont la page prec est */
				       /* non vide ; on met un volume negatif pour */
				       /* que la procedure IsViewFull rende vrai aux englobants */
				      {
					 if (!assoc)
					    pDoc->DocViewFreeVolume[viewNb - 1] = -1;
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
					      pAb = pEl->ElParent->ElAbstractBox[viewNb - 1];
					      while (pAb != NULL && pAb->AbPresentationBox)
						 pAb = pAb->AbNext;
					      /* recherche l'ascendant de plus haut niveau a detruire */
					      found = FALSE;
					      pAbbDestroy = NULL;
					      while (!found && pAb != NULL
						     && pAb->AbElement->ElTypeNumber !=
						     PageBreak + 1
						     && pAb != pDoc->DocViewRootAb[viewNb - 1])
						{
						   /* on regarde si pAb est vide */
						   pAbb = pAb->AbFirstEnclosed;
						   while (pAbb != NULL && !found)
						      if (pAbb->AbPresentationBox)
							 pAbb = pAbb->AbNext;
						      else if (!pAbb->AbDead)
							 found = TRUE;
						      else
							 /* pAbb est mort -> pAb est vide */
							 /* on regarde s'il y a un pave suivant */
							 /* cas !forward */
							 pAbb = pAbb->AbNext;
						   if (!found)
						      /* pAb est vide, on le marque mort */
						      /* et on passe a l'englobant */
						     {
							SetDeadAbsBox (pAb);	/* et mise a jour du volume */
							ApplyRefAbsBoxSupp (pAb, &pAbbReDisp, pDoc);
							pAbbDestroy = pAb;
							pAb = pAb->AbEnclosing;
						     }
						}
					      if (pAbbDestroy != NULL)
						{
						   /* cas ou on a detruit tout le contenu du corps */
						   /* de page : on lui met son volume a un pour */
						   /* ne s'affiche pas en grise         */
						   if (pAbbDestroy->AbEnclosing->AbElement
						       ->ElTypeNumber == PageBreak + 1)
						      pAbbDestroy->AbEnclosing->AbVolume = 1;
						   /* destruction des paves */
						   pAb = pAbbDestroy;
						   pAbbR = NULL;
						   KillPresSibling (pAb->AbPrevious, TRUE,
								    pDoc, &pAbbR, &pAbbReDisp, &volsupp, pAb, FALSE);
						   KillPresSibling (pAb->AbNext, FALSE,
								    pDoc, &pAbbR, &pAbbReDisp, &volsupp, pAb, FALSE);
						   FreeDeadAbstractBoxes (pAb->AbEnclosing);
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

/*----------------------------------------------------------------------
   TestElHB teste si l'element pEl est un element reference        
   vers un element associe place en haut ou bas de page        
   retourne un pointeur vers l'element reference               
   si il faut creer le pave de cet element.                    
  ----------------------------------------------------------------------*/

#ifdef __STDC__
PtrElement          TestElHB (PtrElement pEl, DocViewNumber viewNb)

#else  /* __STDC__ */
PtrElement          TestElHB (pEl, viewNb)
PtrElement          pEl;
DocViewNumber       viewNb;

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
		   if (pEl->ElStructSchema->SsPSchema
		       ->PsInPageHeaderOrFooter[pE->ElTypeNumber - 1])
		      /* c'est bien un element a placer en haut ou bas de page */
		      /* faut-il creer son pave ? */
		      /* oui si il n'a pas encore de pave */
		      /* et si l'element pEl est la premiere reference a pE */
		      if (pE->ElAbstractBox[viewNb - 1] == NULL)
			{
			   premier = TRUE;	/* a priori, pEl est la premiere */
			   /* reference a pE */
			   while (pRef->RdPrevious != NULL && premier)
			     {
				pRef = pRef->RdPrevious;
				if (pRef->RdElement != NULL)
				   if (!IsASavedElement (pRef->RdElement))
				      if (pRef->RdElement->ElAssocNum == 0)
					 /* il y a une reference precedente qui */
					 /* n'est pas dans le tampon */
					 /* et qui n'est pas un element associe' */
					 premier = FALSE;
			     }
			   /* on ne prend en compte que la premiere reference a */
			   /* l'element qui n'est pas dans un element associe' */
			   if (premier)
			      /* si l'element reference' est dans le tampon */
			      /* couper/coller, on l'ignore */

			      if (!IsASavedElement (pE))
				 Ret = pE;
			}
		}
	}
   return Ret;
}
#endif /* __COLPAGE__ */

#ifdef __COLPAGE__

/*----------------------------------------------------------------------
   CrPavHB  cree les paves de l'element associe pEl.              
   Ce pave doit etre place en haut ou bas de la page courante pElPage.            
   Si l'element pere de pEl (c'est la racine de l'arbre associe) n'a pas
   de pave dans la page, il faut le  creer en meme temps. 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CrPavHB (PtrElement pEl, PtrDocument pDoc, DocViewNumber viewNb,
			     PtrElement pElPage, boolean forward)
#else  /* __STDC__ */
void                CrPavHB (pEl, pDoc, viewNb, pElPage, forward)
PtrElement          pEl;
PtrDocument         pDoc;
DocViewNumber       viewNb;
PtrElement          pElPage;
boolean             forward;
#endif /* __STDC__ */

{
   boolean             baspage, bool;
   PtrAbstractBox      pAb, pAbb1, pElAscent, ancPere, pAbbLast, pHB, pPRP,
                       pPRP1, pPRP2, pAbbReDisp, pAbb;
   AbDimension        *pDimAb;
   PtrElement          pVoisin;
   PresentationBox    *pBo1;
   boolean             fin, found, perenouveau, premtour, complete, ok,
                       stop;
   boolean             nouveau;
   int                 volume, frame, h;
   PtrPRule            pRule, pRCre, pRD, pRS, pR, pRV, pRuleDimV, pRStd;
   PtrPSchema          pSPR;
   PtrSSchema          pSS;
   PtrAttribute        pAttr;
   int                 vue, viewSch, index;
   int                 Hauteur, PosV, CarCoupe;		/* pour calcul page */
   FILE               *list;

   pHB = NULL;
   if (pElPage->ElAbstractBox[viewNb - 1] != NULL)
      /* la page a ete creee, on peut creer le pave de pEl */
      if (pEl != NULL && pEl->ElParent != NULL
	  && pEl->ElParent->ElParent == NULL)
	 /* le pere de pEl est bien la racine de l'arbre associe */
	{
	   /* on verifie d'abord si il existe bien une regle de */
	   /* creation des elements associes */
	   ok = FALSE;
	   viewSch = pDoc->DocView[viewNb - 1].DvPSchemaView;
	   /* on recherche s'il a une regle de creation d'elements */
	   /* associes */
	   stop = FALSE;
	   pRCre = GlobalSearchRulepEl (pElPage, &pSPR, &pSS, 0,
			    NULL, viewSch, PtFunction, FnAny, TRUE, FALSE, &pAttr);
	   do
	      if (pRCre == NULL)
		 stop = TRUE;
	      else if (pRCre->PrType != PtFunction)
		 stop = TRUE;
	      else
		 /* c'est une regle de creation */
		 /* on regarde si la boite creee contient */
		 /* des elements associes du type de pEl */
		{
		   pBo1 = &pSPR->PsPresentBox[pRCre->PrPresBox[0] - 1];
		   if (pRCre->PrViewNum == viewSch
		       && pBo1->PbContent == ContElement
		       && pBo1->PbContElem == pEl->ElTypeNumber
		   && pSPR == pElPage->ElAbstractBox[viewNb - 1]->AbPSchema)
		      ok = TRUE;
		   else
		      pRCre = pRCre->PrNextPRule;
		}
	   while (!stop && !ok);
	   if (ok)		/* on a found la regle, on recherche */
	      /* le pave de presentation correspondant */
	      /* c'est un pave haut ou bas de page */
	     {
		found = FALSE;
		pHB = pElPage->ElAbstractBox[viewNb - 1];
		while (!found && pHB != NULL && pHB->AbElement == pElPage)
		   if (pHB->AbPresentationBox
		       && pHB->AbTypeNum == pRCre->PrPresBox[0])
		      found = TRUE;
		   else
		      pHB = pHB->AbNext;
		if (!found)
		   /* le pave haut ou bas de page n'a pas ete cree */
		   /* il faut le creer */
		   pHB = CrAbsBoxesPres (pElPage, pDoc, pRCre, pElPage->ElStructSchema,
					 NULL, viewNb, pSPR, FALSE, TRUE);
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
		perenouveau = FALSE;
		found = FALSE;
		if (pEl->ElParent->ElAbstractBox[viewNb - 1] != NULL)
		  {
		     pElAscent = pEl->ElParent->ElAbstractBox[viewNb - 1];
		     while (pElAscent->AbPresentationBox)
			pElAscent = pElAscent->AbNext;
		     while (!found && !perenouveau)
			if (pElAscent->AbEnclosing->AbElement == pElPage)
			   found = TRUE;
			else if (pElAscent->AbEnclosing->AbElement->ElPageNumber
				 < pElPage->ElPageNumber)
			   if (pElAscent->AbNextRepeated != NULL)
			      pElAscent = pElAscent->AbNextRepeated;
			   else
			     {
				/* il faut creer le nouveau pave apres */
				pAbb1 = pElAscent;
				pElAscent = CopyAbsBox (pAbb1);
				pAbb1->AbNextRepeated = pElAscent;
				pElAscent->AbPreviousRepeated = pAbb1;
				perenouveau = TRUE;
			     }
			else
			   /* cas ou il faut creer le pave avant et le chainer */
			   /* aux paves existants */
			  {
			     pAbb1 = pElAscent;
			     pElAscent = CopyAbsBox (pAbb1);
			     pElAscent->AbNextRepeated = pAbb1;
			     if (pAbb1->AbPreviousRepeated == NULL)
				pEl->ElParent->ElAbstractBox[viewNb - 1] = pElAscent;
			     pElAscent->AbPreviousRepeated = pAbb1->AbPreviousRepeated;
			     pAbb1->AbPreviousRepeated = pElAscent;
			     perenouveau = TRUE;
			  }
		  }

		else
		   /* le pere n'a pas encore de pave, on le cree sans sa descendance */
		  {
		     pElAscent = AbsBoxesCreate (pEl->ElParent, pDoc, viewNb, forward, FALSE,
						 &complete);
		     if (pElAscent != NULL)
			perenouveau = TRUE;
		     /* ce pave n'est pas coupe en tete */
		     if (pEl->ElParent->ElAbstractBox[viewNb - 1] != NULL)
			pEl->ElParent->ElAbstractBox[viewNb - 1]->AbTruncatedHead = FALSE;
		  }
		if (perenouveau && pElAscent != NULL)
		   /* chainage sous le pave pHB  Haut ou Bas de page qui */
		   /* contient ces elements associes */
		   /* chainage particulier car on place un pave d'element */
		   /* sous un pave de presentation */
		  {
		     pElAscent->AbEnclosing = pHB;
		     /* cas simple si pHB n'a pas de fils */
		     if (pHB->AbFirstEnclosed == NULL)
			pHB->AbFirstEnclosed = pElAscent;
		     else
			/* placement parmi les autres paves de presentation du */
			/* haut ou bas de page : donne par ordre des regles ? */
			/* choix de placement : pour haut de page, placement */
			/* comme dernier fils de pHB, pour bas de page, placement */
			/* comme premier fils */
		     if (pSPR->PsPresentBox[pHB->AbTypeNum - 1].PbPageHeader)
			/* la boite correspondant au pave pHB est un haut de page */
		       {
			  pAbb1 = pHB->AbFirstEnclosed;
			  while (pAbb1->AbNext != NULL)
			     pAbb1 = pAbb1->AbNext;
			  pAbb1->AbNext = pElAscent;
			  pElAscent->AbPrevious = pAbb1;
			  baspage = FALSE;
		       }
		     else
			/* (pSPR->PsPresentBox[pHB->AbTypeNum-1].PbPageFooter) */
			/* normalement toujours vrai */
		       {
			  pAbb1 = pHB->AbFirstEnclosed;
			  pHB->AbFirstEnclosed = pElAscent;
			  pElAscent->AbNext = pAbb1;
			  pAbb1->AbPrevious = pElAscent;
			  baspage = TRUE;	/* pour mise a jour hauteur page */
		       }
		  }
		if (found || perenouveau)
		  {
		     /* on cree les paves de pEl et ceux des voisins non references */
		     volume = pDoc->DocViewFreeVolume[viewNb - 1];
		     pDoc->DocViewFreeVolume[viewNb - 1] = THOT_MAXINT;
		     pAbb1 = AbsBoxesCreate (pEl, pDoc, viewNb, forward, TRUE, &complete);
		     /* probleme de chainage ? est-ce que les paves */
		     /* crees sont places sous le bon pave pere ? */
		     /* oui dans le cas de la creation de l'image abstraite */
		     /* pas forcement si nouvel element : il faut considerer */
		     /* le numero de la page */
		     if (pEl->ElAbstractBox[viewNb - 1]->AbEnclosing != pElAscent)
		       {
			  /* l'element pEl a ete mal chaine, il faut refaire le */
			  /* chainage */
			  pAbb1 = pEl->ElAbstractBox[viewNb - 1];
			  pAbbLast = pAbb1;
			  while (pAbbLast->AbNext != NULL
				 && pAbbLast->AbNext->AbElement == pEl)
			     pAbbLast = pAbbLast->AbNext;
			  /* on dechaine les paves de pEl de l'ancien pere */
			  if (pAbb1->AbEnclosing->AbFirstEnclosed == pAbb1)
			    {
			       pAbb1->AbEnclosing->AbFirstEnclosed = pAbbLast->AbNext;
			       if (pAbbLast->AbNext != NULL)
				  pAbbLast->AbNext->AbPrevious = NULL;
			    }
			  else
			    {
			       pAbb1->AbPrevious->AbNext = pAbbLast->AbNext;
			       if (pAbbLast->AbNext != NULL)
				  pAbbLast->AbNext->AbPrevious = pAbb1->AbPrevious;
			    }
			  /* on dechaine les paves de pEl de ses anciens voisins */
			  pAbbLast->AbNext = NULL;
			  pAbb1->AbPrevious = NULL;
			  ancPere = pAbb1->AbEnclosing;		/* pour reappliquer les regles */
			  /* chainage au nouveau pere : pElAscent */
			  if (pElAscent->AbFirstEnclosed == NULL)
			     pElAscent->AbFirstEnclosed = pAbb1;
			  else
			    {
			       pAbb = pElAscent->AbFirstEnclosed;
			       while (pAbb->AbPresentationBox
				  && pAbb->AbElement == pElAscent->AbElement
				      && pAbb->AbNext != NULL)
				  pAbb = pAbb->AbNext;
			       if (pAbb->AbElement == pElAscent->AbElement)
				  /* le pere n'a pas de fils mais que des paves de */
				  /* de presentation on chaine pAbb1 apres */
				  /* normalement il faut se placer avant les derniers */
				 {
				    pAbb->AbNext = pAbb1;
				    pAbb1->AbPrevious = pAbb;
				 }
			       else if (pAbb->AbElement == pEl->ElNext)
				  /* le premier fils de pElAscent est l'element suivant pEl */
				  /* on chaine les paves de pEl avant pAbb */
				 {
				    if (pAbb->AbPrevious == NULL)
				       pElAscent->AbFirstEnclosed = pAbb1;
				    else
				       pAbb->AbPrevious->AbNext = pAbb1;
				    pAbb1->AbPrevious = pAbb->AbPrevious;
				    pAbbLast->AbNext = pAbb;
				    pAbb->AbPrevious = pAbbLast;
				 }
			       else
				  /* pEl doit etre chaine en queue */
				  /* mais avant les paves de presentation derniers */
				  /* fils de pElAscent */
				 {
				    while (pAbb->AbNext != NULL
					   && pAbb->AbNext->AbElement !=
					   pElAscent->AbElement)
				       pAbb = pAbb->AbNext;
				    pAbbLast->AbNext = pAbb->AbNext;
				    if (pAbb->AbNext != NULL)
				       pAbb->AbNext->AbPrevious = pAbbLast;
				    pAbb->AbNext = pAbb1;
				    pAbb1->AbPrevious = pAbb;
				 }
			    }
			  while (pAbb1 != NULL && pAbb1->AbElement == pEl)
			    {
			       pAbb1->AbEnclosing = pElAscent;
			       pAbb1 = pAbb1->AbNext;
			    }
			  RecursEvalCP (pElAscent, pDoc);
			  RecursEvalCP (ancPere, pDoc);
		       }
		     /* modification des regles due aux nouveaux paves crees */
		     pAbb1 = pEl->ElAbstractBox[viewNb - 1];
		     pAbbLast = pAbb1;
		     while (pAbbLast->AbNext != NULL
			    && pAbbLast->AbNext->AbElement == pEl)
			pAbbLast = pAbbLast->AbNext;
		     ApplyRefAbsBoxNew (pAbb1, pAbbLast, &pAbbReDisp, pDoc);

		     /* verifie les elements associes voisins */
		     pVoisin = pEl->ElNext;
		     fin = FALSE;
		     premtour = TRUE;
		     do
		       {
			  if (pVoisin == NULL)
			     if (premtour && pEl->ElPrevious != NULL)
			       {
				  pVoisin = pEl->ElPrevious;
				  premtour = FALSE;
			       }
			     else
				fin = TRUE;
			  if (!fin)
			    {
			       ok = FALSE;
			       if (pVoisin->ElAbstractBox[viewNb - 1] == NULL)
				  if (pVoisin->ElReferredDescr == NULL)
				     /* l'element voisin n'est pas reference, on */
				     /* va creer ses paves */
				     ok = TRUE;
				  else if (pVoisin->ElReferredDescr->ReFirstReference == NULL)
				     ok = TRUE;
				  else if (pVoisin->ElReferredDescr->ReFirstReference->RdElement == NULL)
				     ok = TRUE;
				  else if (pVoisin->ElReferredDescr->ReFirstReference->RdElement->
					   ElAssocNum == pVoisin->ElAssocNum)
				     /* l'element voisin est reference' depuis */
				     /* un element associe' de meme */
				     /* type; on cree ses paves */
				     ok = TRUE;
				  else
				     /* inutile de creer les paves du voisin, il seront */
				     /* crees lorsqu'on rencontrera sa 1ere reference */
				     ok = FALSE;
			       if (ok)
				 {
				    /* cree les paves du voisin */
				    pAbb1 = AbsBoxesCreate (pVoisin, pDoc, viewNb, TRUE,
							    TRUE, &complete);
				    /* modification des regles dues aux nouveaux paves crees */
				    pAbb1 = pVoisin->ElAbstractBox[viewNb - 1];
				    pAbbLast = pAbb1;
				    while (pAbbLast->AbNext != NULL
					   && pAbbLast->AbNext->AbElement == pVoisin)
				       pAbbLast = pAbbLast->AbNext;
				    ApplyRefAbsBoxNew (pAbb1, pAbbLast, &pAbbReDisp, pDoc);
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
		     while (!fin);

		     /* retablit le volume libre reel */
		     pDoc->DocViewFreeVolume[viewNb - 1] =
			pDoc->DocViewFreeVolume[viewNb - 1] - THOT_MAXINT + volume;
		  }		/* fin creation du pave de l'element associe */
		if (perenouveau)
		  {
		     /* application des regles de presentation du pave pElAscent */
		     /* necessaire si c'est un pave resultat d'une duplication */
		     /* pRS : premiere regle de presentation */
		     /* specifique de la boite a creer */
		     SearchPresSchema (pEl->ElParent, &pSPR, &index, &pSS);
		     pRS = pSPR->PsElemPRule[index - 1];
		     /* pRD : premiere regle de presentation par defaut du schema de
		      */
		     /* presentation */
		     pRD = pSPR->PsFirstDefaultPRule;
		     pAbb1 = pDoc->DocRootElement->ElAbstractBox[viewNb - 1];	/* pour retarder */
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
				  if (vue == viewSch)
				     /* applique la regle au pave cree'. */
				    {
				       if (pRV == NULL)
					  pRV = pR;
				       if (pRV->PrType == PtFunction)
					  /* le pave cree' cree un pave de presentation */
					  /* cree le pave de presentation */
					  pAbb1 = CrAbsBoxesPres (pEl->ElParent, pDoc, pRV, pSS,
					   NULL, viewNb, pSPR, FALSE, TRUE);
				       else if (!ApplyRule (pRV, pSPR, pElAscent, pDoc, NULL, &bool))
					  /* on n'a pas pu appliquer la regle, on */
					  /* l'appliquera lorsque le pave pere sera */
					  /* termine' */
					  /* retard sur la racine */
					  /* est-ce vraiment la qu'il faut les retarder ? */
					  Delay (pRV, pSPR, pElAscent, NULL, pAbb1);
				    }
			       }
		       }
		     while (pR != NULL);
		     /* on applique les regles qui etaient retardees sur la */
		     /* racine des elements associes */
		     pAbb = pEl->ElParent->ElAbstractBox[viewNb - 1];
		     pPRP1 = pAbb;
		     /* saute les paves de presentation crees par FnCreateBefore */
		     stop = FALSE;
		     do
			if (pAbb == NULL)
			   stop = TRUE;
			else if (!pAbb->AbPresentationBox)
			   stop = TRUE;
			else
			   pAbb = pAbb->AbNext;
		     while (!stop);
		     /* on initialise le pave sur lequel des regles seront */
		     /* mises les regles retardees a nouveau */
		     pAbb1 = pDoc->DocRootElement->ElAbstractBox[viewNb - 1];
		     pPRP = pAbb;
		     pPRP2 = pAbb;
		     /* boucle de parcours de la chaine des paves dupliques */
		     while (pPRP != NULL)
		       {
			  do
			    {
			       pAbb = pPRP;	/* pAbb a ete modifie dans la boucle */
			       pPRP1 = pAbb;
			       /* on saute les paves de presentation repetes */
			       while (pAbb->AbPresentationBox)
				  pAbb = pAbb->AbNext;
			       pPRP2 = pAbb;
			       GetDelayedRule (&pRule, &pSPR, &pAbb, &pAttr);
			       if (pRule != NULL)
				  if (!ApplyRule (pRule, pSPR, pAbb, pDoc, pAttr, &bool))
				     /* cette regle n'a pas pu etre appliquee           */
				     /* c'est une regle correspondant a un attribut, on */
				     /* l'appliquera lorsque l'englobant sera complete   */
				     /* regles retardees a la racine */
				    {
				       if (pPRP != pAbb1)	/* pour ne pas boucler sur la racine */
					  Delay (pRule, pSPR, pAbb, pAttr, pAbb1);
				    }
			    }
			  while (pRule != NULL);
			  ApplyRefAbsBoxNew (pPRP1, pPRP2, &pAbbReDisp, pDoc);
			  pPRP = pPRP->AbNextRepeated;
		       }

		     /*RecursEvalCP (pAbb1, pDoc); *//*Pour etre sur du bon positionnement */
		     /* des paves !! TODO a changer ! */
		  }		/* fin if perenouveau */
		/* on met a jour les variables de hauteur des paves de pages */
		/* pour permettre une bonne pagination et une hauteur correcte */
		/* du corps de page */
		if (RunningPaginate)
		  {
		     /* pAb = pave haut de page (ou bas de page) */
		     pAb = pEl->ElAbstractBox[viewNb - 1]->AbEnclosing->AbEnclosing;
		     /* on signale au mediateur les nouveaux paves crees */
		     /* on appelle Modifvue a partir du pave haut (ou bas de page) */
		     h = 0;
		     frame = pDoc->DocViewFrame[viewNb - 1];
		     bool = ChangeConcreteImage (frame, &h, pAb);
		     /* calcul de la hauteur du pave haut ou bas de page */
		     SetPageHeight (pAb, TRUE, &Hauteur, &PosV, &CarCoupe);
		     /* Hauteur = dimension verticale du pave haut (ou bas) de page */

		     if (baspage)	/* des paves ont ete ajoutes en bas de page */
		       {
			  /* mise a jour de BreakPageHeight pour permettre */
			  /* une bonne pagination */
			  BreakPageHeight = WholePageHeight - Hauteur;
			  /* mise a jour de PageFooterHeight */
			  PageFooterHeight = Hauteur;
		       }
		     else	/* des paves ont ete ajoutes en haut de page */
			/* la hauteur de coupure ne change pas car c'est toujours */
			/* la hauteur totale de la page moins le bas qui est inchange */
			PageHeaderHeight = Hauteur;
		     /* mise a jour de la hauteur du corps de page */
		     /* on met (ou on modifie) une regle de presentation specifique */
		     /* cherche d'abord la regle qui s'applique a l'element */
		     pRStd = GlobalSearchRulepEl (pElPage, &pSPR, &pSS, 0, NULL, viewSch,
					     PtHeight, FnAny, FALSE, TRUE, &pAttr);
		     /* cherche si l'element a deja une regle de hauteur specifique */
		     pRuleDimV = SearchPresRule (pElPage, PtHeight, 0,
						 &nouveau, pDoc, viewNb);
		     if (nouveau)
			/* on a cree' une regle de hauteur pour l'element */
		       {
			  pR = pRuleDimV->PrNextPRule;	/* on recopie la regle standard */
			  *pRuleDimV = *pRStd;
			  pRuleDimV->PrNextPRule = pR;
			  pRuleDimV->PrCond = NULL;	/* pas de condition d'application associee */
			  pRuleDimV->PrViewNum = viewSch;	/* inutile ? */
		       }
		     /* change le parametre de la regle */
		     pRuleDimV->PrDimRule.DrValue =
			WholePageHeight - PageFooterHeight - PageHeaderHeight;
		     pAbb = pElPage->ElAbstractBox[viewNb - 1];
		     while (pAbb->AbPresentationBox)
			pAbb = pAbb->AbNext;
		     /* pAbb = pave du corps de page */
		     /* applique la nouvelle regle specifique Verticale */
		     if (pRuleDimV != NULL)
			ApplyRule (pRuleDimV, pSPR, pAbb, pDoc, pAttr, &bool);
		     pDimAb = &pAbb->AbHeight;
		     pDimAb->DimMinimum = TRUE;		/* regle de hauteur minimum */
		     /* TODO inutile, a supprimer car fait par applique */
		     pDimAb->DimValue = WholePageHeight - PageFooterHeight - PageHeaderHeight;
		     pAbb->AbHeightChange = TRUE;
		     /* on memorise le pave de pEl (ou son pere si il a ete cree) */
		     /* au cas ou la page deborde a cause de lui */
		     if (!StopBeforeCreation)
			if (perenouveau)
			   AbsBoxAssocToDestroy = pEl->ElAbstractBox[viewNb - 1]->AbEnclosing;
			else
			   AbsBoxAssocToDestroy = pEl->ElAbstractBox[viewNb - 1];
		  }		/* fin RunningPaginate */
	     }
	}
   /* sauvegarde de l'image abstraite pour tests */
}
#endif /* __COLPAGE__ */

#ifdef __COLPAGE__

/*----------------------------------------------------------------------
   FeuilleSuiv rend l'element feuille qui suit                
   l'element pEl. Cet element peut etre             
   un element compose sans fils                     
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static PtrElement   FeuilleSuiv (PtrElement pEl)

#else  /* __STDC__ */
static PtrElement   FeuilleSuiv (pEl)
PtrElement          pEl;

#endif /* __STDC__ */

{
   PtrElement          result, pElSuiv, pElPere;
   boolean             Trouve;

   result = NULL;
   Trouve = FALSE;
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
		Trouve = TRUE;
		result = pElSuiv;
	     }
	   else
	      /* pElSuiv est un element compose, non vide, on cherche sa premiere */
	      /* feuille */
	      pElSuiv = pElSuiv->ElFirstChild;
	}
   else
      result = NULL;		/* pas d'element feuille precedent */
   return result;

}
#endif /* __COLPAGE__ */

#ifdef __COLPAGE__
/*----------------------------------------------------------------------
   ArretCreationCol retourne vrai si il faut arreter la creation   
   pour creer une nouvelle colonne (cas ou on a une colonne
   et ou on doit changer de groupes de colonnes : il faut  
   couper cette colonne en jajoutant un nouvelle marque).  
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static boolean      ArretCreationCol (PtrElement pEl, PtrDocument pDoc,
				      DocViewNumber viewNb, int viewSch)

#else  /* __STDC__ */
static boolean      ArretCreationCol (pEl, pDoc, viewNb, viewSch)
PtrElement          pEl;
PtrDocument         pDoc;
DocViewNumber       viewNb;
int                 viewSch;

#endif /* __STDC__ */

{
   PtrAbstractBox      pAb;
   int                 NbCol, TypeP;
   boolean             retour;
   PtrPSchema          pSchP;

   retour = FALSE;		/* a priori, pas d'arret */

   /* on recherche si le groupe de colonnes precedent est */
   /* dans la meme page, c'est-a-dire si le pave page/col */
   /* precedent n'est pas le pave d'une marque page */
   /* si oui, on equilibre les colonnes du groupe precedent */
   /* et on continue la creation */
   pAb = RechPavPageCol (pEl, viewNb, viewSch, TRUE);
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
	   TypeP = TypeBCol (pAb->AbElement, viewSch, &pSchP, &NbCol);
	   if (NbCol == 2)
	     {
		pAb = pAb->AbEnclosing;
		if (pAb->AbElement->ElPageType == ColGroup)
		   /* toujours vrai normalement */
		  {
		     retour = TRUE;
		     /* on sort de la procedure pour que Page */
		     /* ajoute une marque colonne et */
		     /* cree les paves des elements en consequence */
		     /* l'arret est remonte de la pile des appels en mettant le */
		     /* volume libre negatif (cf. IsViewFull) */
		     /* volume mis a -1 : arret creation */
		     if (AssocView (pEl))
			pDoc->DocAssocFreeVolume[pEl->ElAssocNum - 1] = -1;
		     else
			pDoc->DocViewFreeVolume[viewNb - 1] = -1;
		  }
	     }
	}
   return retour;
}
/* end of function ArretCreationCol */
#endif /* __COLPAGE__ */

#ifdef __COLPAGE__
/*----------------------------------------------------------------------
   InitPageCol determine les conditions de creation               
   si l'element a creer est une colonne ou une page       
   Au retour, Creation, complete, et ApplRegles sont       
   positionnes. De plus pEl et pElSauv peuvent l'etre     
   aussi.                                                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                InitPageCol (PtrElement * ppEl, PtrDocument pDoc, DocViewNumber viewNb,
			   int viewSch, boolean forward, boolean * Creation,
	     boolean * complete, boolean * ApplRegles, PtrElement * pElSauv)
#else  /* __STDC__ */
void                InitPageCol (ppEl, pDoc, viewNb, viewSch, Creation, complete, ApplRegles,
				 pElSauv)
PtrElement         *ppEl;
PtrDocument         pDoc;
DocViewNumber       viewNb;
int                 viewSch;
boolean             forward;
boolean            *Creation;
boolean            *complete;
boolean            *ApplRegles;
PtrElement         *pElSauv;

#endif /* __STDC__ */
{
   boolean             ArretPageRef;
   boolean             stop, FilsComplet;
   PtrElement          pEl, pEl1, pPage;
   PtrAbstractBox      pAbb;

   pEl = *ppEl;
   /* si pagination en cours et que l'element est une marque */
   /* colonne groupee, il faut arreter la creation et equilibrer */
   /* le groupe de colonnes precedent si besoin */
   /* RunningPaginate et ToBalance sont des variables globales */
   if (RunningPaginate && ToBalance)
      if (pEl->ElTerminal && pEl->ElLeafType == LtPageColBreak
	  && pEl->ElPageType == ColGroup
	  && pEl->ElViewPSchema == viewSch)
	 StopGroupCol = ArretCreationCol (pEl, pDoc, viewNb, viewSch);
   /* si pagination en cours et que l'element est une marque page */
   /* ou un element reference sur un element associe place en haut */
   /* ou bas de page, on s'arrete si StopBeforeCreation est vrai */
   /* l'arret est remonte de la pile des appels en mettant le */
   /* volume libre negatif (cf. IsViewFull) */
   ArretPageRef = FALSE;
   if (RunningPaginate && StopBeforeCreation)
      if (pEl->ElTerminal && pEl->ElLeafType == LtPageColBreak
	  && (pEl->ElPageType == PgBegin
	      || pEl->ElPageType == PgComputed
	      || pEl->ElPageType == PgUser))
	{
	   if (pEl->ElViewPSchema == viewSch)
	     {
		FoundPageHF = TRUE;	/* variable globale */
		ArretPageRef = TRUE;
	     }
	}
      else if (TestElHB (pEl, viewNb) != NULL)
	{
	   FoundPageHF = TRUE;	/* variable globale */
	   ArretPageRef = TRUE;
	}

   if (ArretPageRef)
      /* volume mis a -1 : arret creation */
      if (AssocView (pEl))
	 pDoc->DocAssocFreeVolume[pEl->ElAssocNum - 1] = -1;
      else
	 pDoc->DocViewFreeVolume[viewNb - 1] = -1;

   if (!StopGroupCol && !ArretPageRef)
     {
	/* cas temporaire ou des colonnes ne sont pas englobees dans un pave */
	/* groupe de colonnes. On cree un element Marque ColGroup devant */
	/* la premiere colonne */
	if (pEl->ElTypeNumber == PageBreak + 1
	    && pEl->ElViewPSchema == viewSch
	 && (pEl->ElPageType == ColBegin || pEl->ElPageType == ColComputed))
	  {
	     pEl1 = BackSearchTypedElem (pEl, PageBreak + 1, NULL);
	     while (pEl1 != NULL && pEl1->ElViewPSchema != viewSch)
		pEl1 = BackSearchTypedElem (pEl1, PageBreak + 1, NULL);
	     if (pEl1 != NULL
		 && (pEl1->ElPageType == PgBegin
		     || pEl1->ElPageType == PgComputed
		     || pEl1->ElPageType == PgUser))
		/* pEl est une marque colonne de debut qui n'est pas precedee */
		/* d'une marque colonne groupee : on en ajoute une */
	       {
		  pEl1 = NewSubtree (PageBreak + 1, pEl->ElStructSchema,
			     pDoc, pEl->ElAssocNum, TRUE, TRUE, TRUE, TRUE);
		  InsertElementBefore (pEl, pEl1);
		  pEl1->ElPageType = ColGroup;
		  pEl1->ElViewPSchema = viewSch;
		  pEl1->ElPageNumber = 0;
		  /* numero attribue pour les groupes */
		  pAbb = AbsBoxesCreate (pEl1, pDoc, viewNb, TRUE, TRUE, &FilsComplet);
	       }
	  }
	/* cas du scroll arriere : si pEl est une marque de page, il faut */
	/* creer le pave de la marque de page */
	/* precedente (si il n'existe pas). En effet, les paves qui */
	/* seront crees ensuite seront places dans cette page */
	/* cependant, la chaine des dupliques doit etre creee en */
	/* considerant le chemin entre la racine et pEl */
	if (!forward && pEl->ElTerminal && pEl->ElLeafType == LtPageColBreak
	    && pEl->ElViewPSchema == viewSch)
	   if (pEl->ElAbstractBox[viewNb - 1] != NULL)
	      if (pEl->ElPageType == PgBegin || pEl->ElPageType == PgComputed
		  || pEl->ElPageType == PgUser)
		{		/* cas marque page avec pave */
		   pPage = pEl;
		   /* recherche de la marque page precedente pour la vue */
		   /* en sautant les marques colonnes */
		   stop = FALSE;
		   do
		     {
			pEl1 = pPage;
			pPage = BackSearchTypedElem (pEl1, PageBreak + 1, NULL);
			if (pPage == NULL)
			   stop = TRUE;
			else if (pPage->ElViewPSchema == viewSch
				 && (pPage->ElPageType == PgBegin
				     || pPage->ElPageType == PgComputed
				     || pPage->ElPageType == PgUser))
			   stop = TRUE;
		     }
		   while (!stop);
		   if (pPage != NULL)
		     {
			/* il y a un element marque page precedent */
			if (pPage->ElAbstractBox[viewNb - 1] == NULL)
			   /* la marque de page precedente n'a pas de pave */
			   /* corps de page, il faut la creer : */
			   /* la suite de la procedure est executee avec l'element */
			   /* marque de page precedent (sauf duplication)  */
			  {
			     *Creation = TRUE;
			     *complete = TRUE;	/* un pave corps de page est complete */
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
			     *Creation = FALSE;
			     *complete = TRUE;	/* un pave corps de page est tj complete */
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
			while (pEl1 != NULL && (pEl1->ElViewPSchema != viewSch
					     || pEl1->ElPageType == ColBegin
					  || pEl1->ElPageType == ColComputed
					   || pEl1->ElPageType == ColUser));
			if (pEl1 != NULL
			    && pEl1->ElPageType == ColGroup
			    && pEl1->ElAbstractBox[viewNb - 1] == NULL)
			  {
			     /* il y a un element marque ColGroup precedent */
			     /* qui n'a pas de pave */
			     /* la suite  est executee avec l'element */
			     /* colonne groupees precedent (sauf dup)  */
			     *Creation = TRUE;
			     *complete = TRUE;	/* un pave colonne est complete */
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
			     *Creation = FALSE;
			     *complete = TRUE;
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
			while (pPage != NULL && pPage->ElViewPSchema != viewSch);
			if (pPage != NULL &&
			    (pPage->ElPageType == ColBegin
			     || pPage->ElPageType == ColComputed
			     || pPage->ElPageType == ColUser)
			    && pPage->ElAbstractBox[viewNb - 1] == NULL)
			   /* il y a un element marque colonne precedent */
			  {
			     /* la marque de colonne precedente n'a pas de pave */
			     /* la suite du code est executee avec l'element */
			     /* marque de colonne precedent (sauf duplication)  */
			     *Creation = TRUE;
			     *complete = TRUE;	/* un pave colonne est complete */
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
			     *Creation = FALSE;
			     *complete = TRUE;	/* un pave corps de page est tj complete */
			  }
		     }		/* fin colonne simple */
		}		/* fin cas colonne */
	   else
	      /* pas de pave pour pEl, on le creera */
	     {
		*Creation = TRUE;
		*complete = TRUE;	/* un pave corps de page est tj complete */
	     }
	/* TODO prevoir test de verification : l'element voisin */
	/* a droite est une marque de page (page vide) */
	else if (pEl->ElAbstractBox[viewNb - 1] != NULL)
	  {
	     /* pEl n'est pas une marque page ou sens = forward */
	     /* le pave existe deja pour cette vue */
	     /* il faut sauter les paves de presentation et les paves dupliques */
	     /* si on va forward */
	     pAbb = pEl->ElAbstractBox[viewNb - 1];
	     while (pAbb->AbPresentationBox)
		pAbb = pAbb->AbNext;
	     if (forward)
		while (pAbb->AbNextRepeated != NULL)
		   pAbb = pAbb->AbNextRepeated;
	     *Creation = FALSE;
	     if (pAbb->AbLeafType != LtCompound ||
	     /* on retire le cas mis en ligne */
	     /*  pEl->ElAbstractBox[viewNb - 1]->AbInLine || */
		 (pEl->ElTerminal && pEl->ElLeafType == LtPageColBreak))
		/* c'est une feuille ou un pave' mis en lignes,  */
		/* ou un corps de page */
		/* il a deja tout son contenu */
		*complete = TRUE;
	     else if (forward)
		*complete = !pAbb->AbTruncatedTail;
	     else
		*complete = !pAbb->AbTruncatedHead;

	     if (pAbb->AbSize == -1)
		*ApplRegles = TRUE;
	     /* il faut lui appliquer ses regles de presentation */
	  }
	else
	   /* le pave n'existe pas pour cette vue, on essaiera de le creer. */
	  {
	     *Creation = TRUE;
	     /* on ne sait pas si on pourra creer toute son image */
	     *complete = FALSE;
	  }

     }
   *ppEl = pEl;			/* au retour pEl peut avoir ete modifie (cas !forward) */
}				/* end of InitPageCol */
#endif /* __COLPAGE__ */


#ifndef __COLPAGE__
/*----------------------------------------------------------------------
   ApplPage    ApplyRule les regles de presentation au pave            
   cree quand c'est une marque de page TypeP                    
   et pSchPPage ont ete initialises dans ChercheVisib           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ApplPage (PtrElement pEl, PtrDocument pDoc, DocViewNumber viewNb, int viewSch,
		  int TypeP, PtrPSchema pSchPPage, PtrAbstractBox pNewAbbox)
#else  /* __STDC__ */
void                ApplPage (pEl, pDoc, viewNb, viewSch, TypeP, pSchPPage, pNewAbbox)
PtrElement          pEl;
PtrDocument         pDoc;
DocViewNumber       viewNb;
int                 viewSch;
int                 TypeP;
PtrPSchema          pSchPPage;
PtrAbstractBox      pNewAbbox;

#endif /* __STDC__ */
{
   int                 nv;
   PtrPRule            pRule, pRegleV, pRSpec, pRDef;
   PtrAbstractBox      pAbbChild;
   AbDimension        *pDimAb;

   /* traitement particulier aux sauts de page */
   /* le cas des pages Rappel est supprime */
   /* c'est un saut de page, on va creer tout le contenu du pave */
   pNewAbbox->AbAcceptLineBreak = FALSE;
   pNewAbbox->AbAcceptPageBreak = FALSE;
   pNewAbbox->AbNotInLine = FALSE;
   pNewAbbox->AbTruncatedHead = FALSE;
   pNewAbbox->AbTruncatedTail = FALSE;
   /* engendre le contenu de la boite de changement de page */
   pAbbChild = InitAbsBoxes (pEl, viewNb, 10);
   pAbbChild->AbPSchema = pNewAbbox->AbPSchema;
   pNewAbbox->AbFirstEnclosed = pAbbChild;
   /* chaine le pave cree' */
   pAbbChild->AbEnclosing = pNewAbbox;
   /* initialise le contenu de ce pave : un filet horizontal d'une */
   /* epaisseur de 1 pt */
   pAbbChild->AbLeafType = LtGraphics;
   pAbbChild->AbShape = 'h';
   pAbbChild->AbGraphAlphabet = 'L';
   pAbbChild->AbVolume = 1;
   pAbbChild->AbCanBeModified = FALSE;
   pAbbChild->AbEnclosing->AbVolume = pAbbChild->AbVolume;
   pAbbChild->AbAcceptLineBreak = FALSE;
   pAbbChild->AbAcceptPageBreak = FALSE;
   pAbbChild->AbNotInLine = FALSE;
   /* applique a ce pave les regles de presentation de la boite */
   /* page */
   pEl->ElAbstractBox[viewNb - 1] = pAbbChild;
   pRSpec = pSchPPage->PsPresentBox[TypeP - 1].PbFirstPRule;
   /* premiere regle de presentation par defaut */
   pRDef = pSchPPage->PsFirstDefaultPRule;
   do
     {
	pRule = GetRule (&pRSpec, &pRDef, pEl, NULL, pEl->ElStructSchema);
	/* pointeur sur la regle a appliquer pour la vue 1 */
	if (pRule != NULL)
	   if (pRule->PrType == PtFunction)
	     {
		/* parmi les fonctions, on ne traite que les regles */
		/* CreateBefore et CreateAfter */
		if (pRule->PrPresFunction == FnCreateBefore || pRule->PrPresFunction == FnCreateAfter)
		   CrAbsBoxesPres (pEl, pDoc, pRule, pEl->ElStructSchema,
				   NULL, viewNb, pSchPPage, FALSE, TRUE);
		/*else
		  {
		    if (ApplyRule (pRule, pSchPPage, pNewAbbox, pDoc, NULL))
		      pNewAbbox->AbAspectChange = TRUE;
		  }*/
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
					     pRule->PrType, nv, pEl, NULL,
					     pEl->ElStructSchema);
		   if (nv == viewSch && DoesViewExist (pEl, pDoc, viewNb))
		     {
			if (pRegleV == NULL)
			   pRegleV = pRule;
			if (!ApplyRule (pRegleV, pSchPPage, pAbbChild, pDoc, NULL))
			   if (pRegleV->PrType == PtVisibility)
			     pAbbChild->AbVisibility = pNewAbbox->AbVisibility;
		     }
		}
     }
   while (pRule != NULL);
   pEl->ElAbstractBox[viewNb - 1] = pNewAbbox;
   /* impose les regles de la boite marque de page */
   /* hauteur d'une marque de page */
   pDimAb = &pAbbChild->AbHeight;
   pDimAb->DimIsPosition = FALSE;
   pDimAb->DimValue = 1;
   pDimAb->DimAbRef = NULL;
   pDimAb->DimUnit = UnPoint;
   pDimAb->DimUserSpecified = FALSE;
   pAbbChild->AbSize = 1;
   pAbbChild->AbSizeUnit = UnPoint;
   pAbbChild->AbHighlight = 0;
   pAbbChild->AbUnderline = 0;
   pAbbChild->AbThickness = 0;
   pAbbChild->AbLineWeight = 1;
   pAbbChild->AbLineWeightUnit = UnPoint;
   pAbbChild->AbVisibility = pNewAbbox->AbVisibility;
}				/* end of ApplPage */

#endif /* __COLPAGE__ */



#ifdef __COLPAGE__

/*----------------------------------------------------------------------
   CreePageCol     cree les paves de structure physique (page,       
   colonne) sous le pave racine et duplique les paves
   des elements peres de pEl.                        
   pEl est une marque de page ou de colonne qui      
   concerne la vue traitee                           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreePageCol (PtrElement pEl, PtrAbstractBox * AdrNouvPave, PtrDocument pDoc,
			 DocViewNumber viewNb, int viewSch, boolean forward,
				 boolean * complete, int *lqueue,
     PtrPRule queuePR[MAX_QUEUE_LEN], PtrAbstractBox queuePP[MAX_QUEUE_LEN],
				 PtrPSchema queuePS[MAX_QUEUE_LEN],
		    PtrAttribute queuePA[MAX_QUEUE_LEN], PtrElement pElSauv,
			   PtrPSchema pSchPPage, int TypeP, boolean * arret)
#else  /* __STDC__ */
void                CreePageCol (pEl, AdrNouvPave, pDoc, viewNb, viewSch, forward, complete,
			queuePA, queuePS, queuePP, queuePR, lqueue, pElSauv,
				 pSchPPage, TypeP, arret)
PtrElement          pEl;
PtrAbstractBox     *AdrNouvPave;
PtrDocument         pDoc;
DocViewNumber       viewNb;
int                 viewSch;
boolean             forward;
boolean            *complete;
int                *lqueue;
PtrPRule            queuePR[MAX_QUEUE_LEN];
PtrAbstractBox      queuePP[MAX_QUEUE_LEN];
PtrPSchema          queuePS[MAX_QUEUE_LEN];
PtrAttribute        queuePA[MAX_QUEUE_LEN];
PtrElement          pElSauv;
PtrPSchema          pSchPPage;
int                 TypeP;
boolean            *arret;

#endif /* __STDC__ */
{
   PtrElement          pEl1, pElRoot, pPage, pElAscent, pNext;
   PtrAbstractBox      pAbb, pNewAbbox, pAbbPres;
   PtrAbstractBox      PavPagePrec, pDupPav, pPavDupFils, pAb, pAbbox1;
   PtrAbstractBox      pAbbRoot, pAbb1, pPAvant, pPApres, pAbbParent, pPsuiv;
   int                 frame, h, index, Hauteur, PosV, CarCoupe, cas, nv;
   boolean             stop, found, adupliquer, filsdup, adetruire, premcol,
                       bool;
   boolean             FilsComplet;
   PtrPSchema          pSPres;
   PtrPRule            pRule, pRegleV, pRSpecPage, pRDefPage, pRCre;
   PRuleType           typeRule;
   PtrSSchema          pSchS;
   PtrAttribute        pAttr;
   AbDimension        *pDimAb;
   PresentationBox    *pBo1;

   /* cas de la derniere marque de page : on ne genere pas de pave */
   /* c'est toujours un element fils de la racine *//* a verifier ?? */
   /* code a supprimer lorsqu'on aura supprime cette marque dans l'arbre */

   pNewAbbox = *AdrNouvPave;

   /* initialisation de pElRoot et de pAbbRoot */
   if (AssocView (pEl))
     {
	pElRoot = pDoc->DocAssocRoot[pEl->ElAssocNum - 1];
	pAbbRoot = pElRoot->ElAbstractBox[0];
     }
   else
     {
	pElRoot = pDoc->DocRootElement;
	pAbbRoot = pElRoot->ElAbstractBox[viewNb - 1];
     }
   if (pEl->ElParent == pElRoot)
      if (pEl->ElNext == NULL)
	 /* derniere marque de page de la vue : il ne faut pas creer de pave */
	{
	   /* Verifier que l'on a pas a liberer un descripteur d'image ? */
	   FreeAbstractBox (pNewAbbox);
	   pNewAbbox = NULL;
	   *AdrNouvPave = NULL;	/* seul cas ou pNewAbbox est en retour */
	   *complete = TRUE;
	}
      else
	 /* on cherche si les elements suivants ne sont que des marques de page */
	 /* pour d'autres vues. Si oui, pEl est la derniere marque de page */
	 /* de la vue : on ne cree pas son pave */
	{
	   pNext = pEl->ElNext;
	   found = FALSE;
	   while (!found && pNext != NULL)
	     {
		if (pNext->ElTerminal && pNext->ElLeafType == LtPageColBreak
		    && pNext->ElViewPSchema != viewSch)
		   pNext = pNext->ElNext;
		else
		   found = TRUE;
	     }
	   if (!found)
	     {
		/* Verifier que l'on a pas a liberer un descripteur d'image ? */
		FreeAbstractBox (pNewAbbox);
		pNewAbbox = NULL;
		*AdrNouvPave = NULL;	/* seul cas ou pNewAbbox est en retour */
		*complete = TRUE;
	     }
	}
   if (pNewAbbox != NULL)
     {
	premcol = FALSE;
	pEl->ElAbstractBox[viewNb - 1] = pNewAbbox;	/* chainage du pave a son element */
	pNewAbbox->AbLeafType = LtCompound;	/* c'est un pave compose */
	/* chainage de pNewAbbox dans l'image abstraite */
	/* il ne faut plus se placer sous la racine mais sous le pave */
	/* de structure physique de plus bas niveau */
	/* ici c'est le pave de page. Si la colonne est la colonne gauche, */
	/* c'est-a-dire si forward et pEl->ElPageNumber = 1, ou !forward et */
	/* pEl->ElPageNumber = derniere colonne ; il faut inserer pNewAbbox */
	/* entre le corps de page et ses fils et ne pas faire de duplication */

	if (pEl->ElPageType == ColBegin || pEl->ElPageType == ColComputed
	    || pEl->ElPageType == ColGroup || pEl->ElPageType == ColUser)
	   /* identification des 12 cas possibles : forward ou non, pEl est */
	   /* une colonne simple ou groupee, le pave de structure physique */
	   /* qui precede (ou suit) est un pave de page, colonne simple ou groupee */

	  {
	     cas = 0;
	     pAbb = RechPavPageCol (pEl, viewNb, viewSch, forward);
	     /* analyse des 12 cas possibles */
	     if (pAbb == NULL)
		cas = 0;	/*erreur */
	     else if (pEl->ElPageType == ColBegin
		      || pEl->ElPageType == ColComputed
		      || pEl->ElPageType == ColUser)	/* cas colonne simple */
		if (forward)
		   if (pAbb->AbElement->ElPageType == ColBegin
		       || pAbb->AbElement->ElPageType == ColComputed
		       || pAbb->AbElement->ElPageType == ColUser)
		      /* pEl n'est pas la premiere colonne du groupe de colonnes */
		      cas = 1;
		   else if (pAbb->AbElement->ElPageType == ColGroup)
		      /* pEl est la premiere colonne du groupe de colonnes */
		      cas = 2;
		   else
		      cas = 0;	/* erreur : pAbb ne peut etre un pave de page */
		else
		   /* cas !forward */ if (pAbb->AbElement->ElPageType == ColBegin
			       || pAbb->AbElement->ElPageType == ColComputed
				  || pAbb->AbElement->ElPageType == ColUser)
		   /* pEl n'est pas la derniere colonne du groupe de colonnes */
		   cas = 3;
		else
		   /* cas 4 */
		   /* si pAbb->AbElement->ElPageType == ColGroup, c'est que */
		   /* pEl est la derniere colonne du groupe de colonnes */
		   /* et ce groupe de colonnes n'est pas le dernier de la page */
		   /* cas 5  traite en meme temps */
		   /* si pAbb->AbElement->ElPageType == page, c'est que */
		   /* pEl est la derniere colonne du groupe de colonnes */
		   /* et ce groupe de colonnes est le dernier de la page */
		   /* dans ces deux cas, il faut rechercher le pave Colonne */
		   /* groupee precedent en sautant les marques colonnes */
		   /* ce pave a ete cree par l'appel a AbsBoxesCreate englobant */
		  {
		     pEl1 = pEl;
		     found = FALSE;
		     while (pEl1 != NULL && !found)
		       {
			  pEl1 = BackSearchTypedElem (pEl1, PageBreak + 1, NULL);
			  found = (pEl1->ElViewPSchema == viewSch &&
				   pEl1->ElPageType == ColGroup);
		       }
		     if (found)
		       {
			  pAbb = pEl1->ElAbstractBox[viewNb - 1];
			  while (pAbb->AbPresentationBox)
			     pAbb = pAbb->AbNext;
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
		/* pEl est une colonne groupee */ if (forward)
		if (pAbb->AbElement->ElPageType == ColBegin
		    || pAbb->AbElement->ElPageType == ColComputed
		    || pAbb->AbElement->ElPageType == ColUser)
		   /* pEl n'est pas la premiere colonne groupee de la page */
		   /* pAbb est le pave de la derniere colonne du groupe precedent */
		  {
		     /* on remonte d'un niveau pour chainer au meme niveau */
		     /* que le pave colgroupees precedent */
		     pAbb = pAbb->AbEnclosing;
		     cas = 6;
		  }
		else if (pAbb->AbElement->ElPageType == ColGroup)
		   cas = 0;	/* erreur */
		else
		   /* pAbb est un pave de page */
		   /* pEl est la premiere colonne groupee de la page */
		   cas = 7;
	     else
		/* cas !forward */ if (pAbb->AbElement->ElPageType == ColBegin
			       || pAbb->AbElement->ElPageType == ColComputed
				  || pAbb->AbElement->ElPageType == ColUser)
		cas = 0;	/* erreur : les col simples ne peuvent etre creees */
	     /* avant la col groupee */
	     else if (pAbb->AbElement->ElPageType == ColGroup)
		/* pEl n'est pas la derniere colonne groupee de la page */
		cas = 8;
	     else
		/* pAbb est un pave corps de page */
		/* pEl est la derniere colonne groupee de la page */
		/* il faut rechercher le pave corps de page de la page */
		/* precedente en sautant les marques colonnes simples et */
		/* groupees. Le pave de cette page precedente  */
		/* a ete cree par l'appel de AbsBoxesCreate englobant */
	       {
		  pEl1 = pEl;
		  found = FALSE;
		  while (pEl1 != NULL && !found)
		    {
		       pEl1 = BackSearchTypedElem (pEl1, PageBreak + 1, NULL);
		       found = (pEl1->ElViewPSchema == viewSch &&
				(pEl1->ElPageType == PgBegin
				 || pEl1->ElPageType == PgComputed
				 || pEl1->ElPageType == PgUser));
		    }
		  if (found)
		    {
		       pAbb = pEl1->ElAbstractBox[viewNb - 1];
		       while (pAbb->AbPresentationBox)
			  pAbb = pAbb->AbNext;
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
			 pAbbParent = pAbbRoot;
			 break;

		      case 2:
		      case 4:
		      case 7:
		      case 9:
			 /* forward et premier ou !forward et dernier */
			 /* pAbb est le pave du corps de page ou ColsGroupees */
			 pNewAbbox->AbEnclosing = pAbb;
			 /* chainage entre le corps et ses fils */
			 if (pAbb->AbFirstEnclosed == NULL)	/* pas de fils, chainage simple */
			    pAbb->AbFirstEnclosed = pNewAbbox;
			 else
			   {
			      pNewAbbox->AbFirstEnclosed = pAbb->AbFirstEnclosed;
			      pAbb->AbFirstEnclosed = pNewAbbox;
			      pNewAbbox->AbFirstEnclosed->AbEnclosing = pNewAbbox;
			      pPsuiv = pNewAbbox->AbFirstEnclosed;
			      while (pPsuiv != NULL)
				{
				   pPsuiv->AbEnclosing = pNewAbbox;
				   for (typeRule = PtVertRef; typeRule <= PtHorizPos; typeRule++)
				     {
					/* recherche de la regle */
					pRule = SearchRulepAb (pDoc, pPsuiv, &pSPres, typeRule, FnAny, TRUE, &pAttr);
					/* application de la regle */
					if (!ApplyRule (pRule, pSPres, pPsuiv, pDoc, pAttr, &bool))
					   Delay (pRule, pSPres, pPsuiv, pAttr, pNewAbbox);
				     }
				   pPsuiv = pPsuiv->AbNext;
				}
			      if (!pNewAbbox->AbFirstEnclosed->AbNew)
				 RecursEvalCP (pNewAbbox->AbFirstEnclosed, pDoc);
			   }
			 premcol = TRUE;	/* premiere colonne, ou derniere si !forward */
			 /* booleen pour ne pas dupliquer */
			 break;

		      case 1:
		      case 3:
		      case 6:
		      case 8:
			 /* forward et pas premier ou !forward et pas dernier */
			 pAbbParent = pAbb->AbEnclosing;
			 break;

		      default:
			 break;
		   }		/* fin switch de traitement des cas */
	  }
	/* fin cas colonne : a la sortie soit premcol est vrai */
	/* et le chainage a ete effectue, soit pAbbParent est correctement */
	/* initialise pour effectuer le chainage */
	else
	   /* pEl est une marque page et non une marque colonne */
	  {
	     pAbb = RechPavPage (pEl, viewNb, viewSch, forward);
	     /* pour le chainage aux voisins */
	     pAbbParent = pAbbRoot;	/* le pere des pages est la racine */
	  }
	if (!premcol)
	  {			/* le chainage a deja ete fait si premcol */
	     pNewAbbox->AbEnclosing = pAbbParent;	/* les paves marque de page */
	     /* sont fils de la racine */

	     /* chainage avec les voisins : code commun pour page et colonnes */
	     /* dans tous les cas, pAbb contient le pave precedent (si forward) */
	     /* ou suivant (si !forward) de l'element de meme niveau que pEl */
	     if (forward)
		if (pAbb == 0)
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
		     pAb = pAbbRoot->AbFirstEnclosed;
		     /* cas du premier pave de presentation */
		     if (pAb != NULL && pAb->AbPresentationBox
			 && pAb->AbElement == pAbbRoot->AbElement)
		       {
			  /* il y a des paves de presentation */
			  while (pAb != NULL && pAb->AbPresentationBox
				 && pAb->AbElement == pAbbRoot->AbElement)
			    {
			       SetDeadAbsBox (pAb);
			       ApplyRefAbsBoxSupp (pAb, &pAbb, pDoc);
			       pAb = pAb->AbNext;
			    }
			  h = 0;
			  if (AssocView (pEl))
			     frame = pDoc->DocAssocFrame[pEl->ElAssocNum - 1];
			  else
			     frame = pDoc->DocViewFrame[viewNb - 1];
			  bool = ChangeConcreteImage (frame, &h, pAbbRoot);
			  FreeDeadAbstractBoxes (pAbbRoot);
			  /* on met les regles de creation en attente */
			  /* pour provoquer la recreation des paves de pres */
			  /* on cherche les regles a retarder */
			  SearchPresSchema (pEl->ElParent, &pSPres, &index, &pSchS);
			  pRule = pSPres->PsElemPRule[index - 1];
			  do
			    {
			       pRule = pRule->PrNextPRule;
			       /* au premier tour on saute la regle de visibilite */
			       if (pRule != NULL && pRule->PrType != PtFunction)
				  pRule = NULL;
			       else if (pRule->PrPresFunction == FnCreateFirst)
				  Delay (pRule, pSPres, pEl->ElParent->ElAbstractBox[0],
					 NULL, pNewAbbox);
			    }
			  while (pRule != NULL);
		       }
		     /* chainage de pNewAbbox a la racine */
		     pNewAbbox->AbNext = pAbbRoot->AbFirstEnclosed;
		     pAbbRoot->AbFirstEnclosed = pNewAbbox;
		     if (pNewAbbox->AbNext != NULL)
			pNewAbbox->AbNext->AbPrevious = pNewAbbox;
		  }		/* fin cas de la premiere page */
		else
		   /* forward et pAbb != NULL */
		  {
		     /* chainage a pAbb en sautant ses eventuels paves de pres */
		     /* remarque : dans le cas des colonnes pAbb->Pavsuivant */
		     /* est normalement null car on n'a pas encore prevu de */
		     /* paves de presentation pour les colonnes */
		     while (pAbb->AbNext != NULL
			    && pAbb->AbNext->AbPresentationBox
			    && pAbb->AbNext->AbElement == pAbb->AbElement)
			pAbb = pAbb->AbNext;
		     pNewAbbox->AbNext = pAbb->AbNext;	/* NULL ? */
		     pAbb->AbNext = pNewAbbox;
		     pNewAbbox->AbPrevious = pAbb;	/* A FINIR ?? je ne sais quoi ! */
		  }
	     else
		/* cas !forward */ if (pAbb != NULL)
		/* tj vrai ? */
	       {
		  /* chainage a pAbb en sautant ses eventuels paves de pres */
		  /* remarque : dans le cas des colonnes pAbb->AbPrevious */
		  /* est normalement null car on n'a pas encore prevu de */
		  /* paves de presentation pour les colonnes */
		  while (pAbb->AbPrevious != NULL
			 && pAbb->AbPrevious->AbPresentationBox
			 && pAbb->AbPrevious->AbElement == pAbb->AbElement)
		     pAbb = pAbb->AbPrevious;
		  /* chainage devant pAbb */
		  if (pAbb->AbEnclosing->AbFirstEnclosed == pAbb)
		     /* tj vrai ? */
		    {
		       pNewAbbox->AbNext = pAbb;
		       pAbb->AbEnclosing->AbFirstEnclosed = pNewAbbox;
		       pAbb->AbPrevious = pNewAbbox;
		    }
		  /* autres cas a considerer ? */
	       }
	     /* autres cas a considerer ? */
	  }			/* fin chainage dans cas !premcol */

	/* code de duplication des paves */
	/* si premiere page creee du document, c.a.d. pas de page precedente */
	/* dans le sens du parcours : dans ce cas pas de duplication */
	/* si forward et premiere colonne simple ou groupee: pas de dup */
	/* si !forward et derniere colonne simple ou groupee: pas de dup */
	/* de meme, si la page a creer est vide : la feuille suivante */
	/* est une marque page ; pas de duplication */
	/* si !forward, pEl contient la marque de page precedente */
	/* donc on recherche dans tous les cas (forward ou !forward) */
	/* si la feuille suivante est une Marque page de la meme viewSch */
	/* TODO il faudrait faire une boucle pour chercher la feuille */
	/* suivante en sautant celles qui ne sont pas de la meme vue */
	pEl1 = FeuilleSuiv (pEl);
	PavPagePrec = RechPavPageCol (pEl, viewNb, viewSch, forward);
	/* on descend la hierarchie des paves page et col */
	if (PavPagePrec != NULL)
	   while (PavPagePrec->AbFirstEnclosed != NULL
		  && PavPagePrec->AbFirstEnclosed->AbElement->ElTypeNumber == PageBreak + 1)
	     {
		PavPagePrec = PavPagePrec->AbFirstEnclosed;
		while (PavPagePrec->AbPresentationBox)	/* on saute les paves de pres */
		   PavPagePrec = PavPagePrec->AbNext;
		/* on est sur que dans la chaine un pave n'est pas pres, donc */
		/* en sortant de la boucle PavPagePrec n'est pas null */
	     }
	/* si le document est mis en colonnes, PavPavPrec est toujours un pave de col */
	if ((!premcol) && PavPagePrec != NULL
	    && !(pEl1 != NULL && pEl1->ElTypeNumber == PageBreak + 1
		 && pEl1->ElViewPSchema == viewSch
		 && (pEl1->ElPageType == PgBegin
		     || pEl1->ElPageType == PgComputed
		     || pEl1->ElPageType == PgUser)))
	  {
	     /* cas de duplication */
	     /* si !forward, il faut dupliquer depuis l'element rencontre */
	     /* (celui sauvegarde dans pElSauv) jusqu'a la racine */
	     /* vrai aussi pour les colonnes simples et groupees */
	     if (!forward)
		if (pElSauv != NULL)
		  {
		     pEl1 = pEl;
		     pEl = pElSauv;
		     pElSauv = pEl1;
		  }

	     /* duplication du chemin depuis le pave de l'element pere
	        de la marque de page jusqu'au pave de l'element racine
	        non compris (boucle ascendante) */
	     pElAscent = pEl->ElParent;		/* pere de la marque de page */
	     pDupPav = NULL;
	     pPavDupFils = NULL;
	     adupliquer = FALSE;
	     filsdup = (pEl->ElPrevious == NULL && pEl->ElNext == NULL);
	     /* code change pour traiter le cas des colonnes vides */
	     /* cas particulier ou la page precedente est une page vide */
	     /* il faut rechercher la suite des paves a dupliquer dans */
	     /* la premiere page suivante ou precedente non vide */
	     stop = FALSE;
	     while (!stop && PavPagePrec != NULL)
		/* boucle sur les pages ou les colonnes vides */
		if (PavPagePrec->AbFirstEnclosed != NULL)
		   stop = TRUE;
		else
		  {
		     pAbb = PavPagePrec;
		     PavPagePrec = NULL;	/* a priori, pas de precedent (suivant) */
		     /* on saute les paves de presentation associes a PavPagePrec */
		     while ((forward && pAbb->AbPrevious != NULL
			     && pAbb->AbPrevious->AbPresentationBox)
			    || (!forward && pAbb->AbNext != NULL
				&& pAbb->AbNext->AbPresentationBox))
			if (forward)
			   pAbb = pAbb->AbPrevious;
			else
			   pAbb = pAbb->AbNext;
		     /* ici, pAbb est soit PavPagePrec soit un pave de presentation */
		     /* dont le pave suivant ou precedent s'il existe n'est pas de pres */
		     /* on passe au pave page ou colonne suivant (precedent) */
		     /* s'il existe */
		     if (forward && pAbb->AbPrevious != NULL)
			PavPagePrec = pAbb->AbPrevious;
		     else if (!forward && pAbb->AbNext != NULL)
			PavPagePrec = pAbb->AbNext;
		     /* s'il n'existe pas comme voisin, on cherche dans la branche */
		     /* precedente (ou suivante) */
		     else
			/* cas des colonnes */ if (pAbb->AbEnclosing->AbElement->ElTypeNumber == PageBreak + 1)
		       {
			  pAbb = pAbb->AbEnclosing;
			  if (forward && pAbb->AbPrevious != NULL)
			     pAbb = pAbb->AbPrevious;
			  else if (!forward && pAbb->AbNext != NULL)
			     pAbb = pAbb->AbNext;
			  else
			     /* pas de precedent (suivant) */
			     /* on remonte d'un dernier niveau */ if (pAbb->AbEnclosing->AbElement->ElTypeNumber == PageBreak + 1)
			     if (forward && pAbb->AbPrevious != NULL)
				pAbb = pAbb->AbPrevious;
			     else if (!forward && pAbb->AbNext != NULL)
				pAbb = pAbb->AbNext;
			     else
				stop = TRUE;
			  else	/* on est au plus haut niveau (page) */
			     stop = TRUE;
			  if (!stop)
			     /* on a trouve un pave de structure physique */
			     /* d'une branche precedente (ou suivante) */
			     /* on redescend pour trouver le pave de */
			     /* structure physique de plus bas niveau */
			    {
			       while (pAbb->AbFirstEnclosed != NULL
				      && pAbb->AbFirstEnclosed->AbElement->ElTypeNumber == PageBreak + 1)
				 {
				    pAbb = pAbb->AbFirstEnclosed;
				    if (forward)	/* on va sur le dernier fils */
				      {
					 while (pAbb->AbNext != NULL)
					    pAbb = pAbb->AbNext;
					 while (pAbb->AbPresentationBox)
					    pAbb = pAbb->AbPrevious;	/* pAbb jamais NULL */
				      }
				    else
				       /* on reste sur le premier mais on saute les */
				       /* paves de presentation */
				       while (pAbb->AbPresentationBox)
					  pAbb = pAbb->AbNext;	/* pAbb jamais NULL */
				 }
			       PavPagePrec = pAbb;
			    }
		       }	/* fin englobant = marquepage */
		     else	/* cas pas de precedent et pas englobant = marquepage */
			stop = TRUE;
		  }		/* fin  PavPagePrec->AbFirstEnclosed != NULL */
	     /* donc fin de la boucle pour sauter les pages et colonnes vides */

	     if (PavPagePrec != NULL)
		while (pElAscent != pElRoot)	/* on ne duplique pas */
		   /*l'element racine */
		  {
		     /* recherche du pave a dupliquer : 
		        c'est le pave dont AbElement = pElAscent et qui est dans
		        le chemin depuis PavPagePrec et le pave de l'element pere
		        de la marque de page. Parcours descendant */
		     /* Remarque : si le document est mis en colonnes, PavPagePrec est */
		     /* toujours une colonne */
		     pAb = PavPagePrec->AbFirstEnclosed;
		     if (pAb != NULL)
		       {
			  stop = FALSE;
			  while (!stop && pAb != NULL)
			    {
			       if (forward)
				  /* on se positionne sur le chemin a dupliquer : */
				  /* si forward, c'est le chemin le plus a droite, */
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
				  /* si !forward, c'est le chemin le plus a gauche, */
				  /* donc le premier fils qui n'est pas de presentation */
				  while (pAb != NULL && pAb->AbPresentationBox)
				     pAb = pAb->AbNext;
			       /* on est sur un pave d'element, est-ce le pave a dupliquer? */
			       if (pAb != NULL)
				  if (pAb->AbElement == pElAscent)
				     stop = TRUE;	/* on a trouve le pave a dupliquer */
				  else
				     /* on descend d'un niveau dans la hierarchie */
				     pAb = pAb->AbFirstEnclosed;
			       else
				  /*pAb = NULL */
				 {
				    printf ("erreur i.a.  rech. pave a duplique \n");
				    printf ("on n'a pas trouve le pave a dupliquer \n");
				    AffPaveDebug (pElAscent->ElAbstractBox[viewNb - 1]);
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
			       if (forward)
				  if (pEl->ElPageType == PgBegin
				      || pEl->ElPageType == PgComputed
				      || pEl->ElPageType == PgUser)
				     while (pEl1->ElNext != NULL &&
					    pEl1->ElNext->ElTypeNumber == PageBreak + 1 &&
				       (pEl1->ElNext->ElPageType == ColBegin
					|| pEl1->ElNext->ElPageType == ColComputed
				      || pEl1->ElNext->ElPageType == ColUser
				     || pEl1->ElNext->ElPageType == ColGroup
					|| pEl1->ElNext->ElViewPSchema != viewSch))
					pEl1 = pEl1->ElNext;
				  else if (pEl->ElPageType == ColGroup)
				     while (pEl1->ElNext != NULL &&
					    pEl1->ElNext->ElTypeNumber == PageBreak + 1 &&
				       (pEl1->ElNext->ElPageType == ColBegin
					|| pEl1->ElNext->ElPageType == ColComputed
				      || pEl1->ElNext->ElPageType == ColUser
					|| pEl1->ElNext->ElViewPSchema != viewSch))
					pEl1 = pEl1->ElNext;
				  else	/* on saute uniquement les marques des autres vues */
				     while (pEl1->ElNext != NULL
					    && pEl1->ElNext->ElTypeNumber == PageBreak + 1
					    && pEl1->ElNext->ElViewPSchema != viewSch)
					pEl1 = pEl1->ElNext;
			       else	/* !forward, quelque soit pEl, */
				  /* on saute uniquement les marques des autres vues */
				  while (pEl1->ElPrevious != NULL &&
					 pEl1->ElPrevious->ElTypeNumber == PageBreak + 1
				  && pEl1->ElNext->ElViewPSchema != viewSch)
				     pEl1 = pEl1->ElPrevious;
			       /* on remonte l'arbre tant qu'on n'a rien a dupliquer */
			       while (pEl1 != pAb->AbElement && !adupliquer)
				  if (forward && pEl1->ElNext != NULL
				      && pEl1->ElNext->ElTypeNumber != PageBreak + 1)
				     adupliquer = TRUE;
				  else if (!forward && pEl1->ElPrevious != NULL
					   && pEl1->ElPrevious->ElTypeNumber != PageBreak + 1)
				     adupliquer = TRUE;
				  else
				     pEl1 = pEl1->ElParent;	/* jamais NULL */
			    }
		       }
		     if (pAb != NULL && adupliquer)
		       {
			  /* duplication et chainage avec les paves voisins */
			  /* tous les champs du pave sont dupliques dans CopyAbsBox sauf
			     ceux des positions, dimensions et changements */
			  pDupPav = CopyAbsBox (pAb);
			  if (forward)
			    {
			       pAb->AbTruncatedTail = TRUE;	/* pAb coupe en queue */
			       pDupPav->AbTruncatedHead = TRUE;		/* pDupPav coupe en tete */
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
			       pAb->AbTruncatedHead = TRUE;	/* pAb coupe en tete */
			       pDupPav->AbTruncatedTail = TRUE;		/*pDupPav coupe en queue */
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
			       pElAscent->ElAbstractBox[viewNb - 1] = pDupPav;
			    }
			  /* cas ou le saut de page a cree un pave compose vide : */
			  /* pAb est un pave d'element compose vide sauf */
			  /* des paves de presentation. Il faut rechainer les paves */
			  /* de presentation, rechainer l'element au pDupPav cree, */
			  /* et detruire le pave pAb. */
			  /* sauf si l'element de pAb n'a qu'un seul fils */
			  adetruire = FALSE;
			  if (!pAb->AbElement->ElTerminal
			      && (pAb->AbElement->ElFirstChild != NULL	/* tj vrai ! */
			   && pAb->AbElement->ElFirstChild->ElNext != NULL))
			    {
			       /* cherche si le pave a des fils qui ne sont pas des */
			       /* paves de presentation, par exemple, les paves des */
			       /* elements freres de la marque de page si deja crees */
			       /* meme code si forward et !forward */
			       found = FALSE;	/* pas de pave d'autres elements */
			       pAbb = pAb->AbFirstEnclosed;
			       while (pAbb != NULL && !found)
				  if (pAbb->AbPresentationBox || pAbb->AbDead)
				     pAbb = pAbb->AbNext;
				  else
				     found = TRUE;
			       if (!found)
				  adetruire = TRUE;
			    }
			  if (adetruire)
			    {
			       SetDeadAbsBox (pAb);
			       /* recherche du premier pave avant pAb */
			       pAbb = pAb;
			       stop = FALSE;
			       do
				  if (pAbb->AbPrevious == NULL)
				     stop = TRUE;
				  else if (pAbb->AbPrevious->AbElement !=
					   pAb->AbElement)
				     stop = TRUE;
				  else
				     pAbb = pAbb->AbPrevious;
			       while (!stop);
			       pPAvant = pAbb;
			       /* recherche du dernier pave apres pAb */
			       pAbb = pAb;
			       stop = FALSE;
			       do
				  if (pAbb->AbNext == NULL)
				     stop = TRUE;
				  else if (pAbb->AbNext->AbElement !=
					   pAb->AbElement)
				     stop = TRUE;
				  else
				     pAbb = pAbb->AbNext;
			       while (!stop);
			       pPApres = pAbb;
			       /* chainage des paves de presentation a pDupPav */
			       if (pPAvant != pAb)	/* il y a des paves pres avant */
				 {
				    pAbb = pPAvant;
				    while (pAbb->AbNext != pAb)
				       pAbb = pAbb->AbNext;
				    pAbb->AbNext = pDupPav;
				    pDupPav->AbPrevious = pAbb;
				 }
			       if (pPApres != pAb)	/* il y a des paves pres apres */
				 {
				    pAbb = pPApres;
				    while (pAbb->AbPrevious != pAb)
				       pAbb = pAbb->AbPrevious;
				    pAbb->AbPrevious = pDupPav;
				    pDupPav->AbNext = pAbb;
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
				    pAbb = pAb->AbFirstEnclosed;
				    pDupPav->AbFirstEnclosed = pAbb;
				    do
				      {
					 pAbb->AbEnclosing = pDupPav;
					 pAbb = pAbb->AbNext;
				      }
				    while (pAbb != NULL);
				 }
			       /* traitements specifiques suivant en avant ou non */
			       if (forward)
				 {
				    if (pAb->AbElement->ElAbstractBox[viewNb - 1] == pAb)
				       /* pas de pave de presentation avant */
				      {
					 /* chainage de pDupPav avec l'element */
					 pAb->AbElement->ElAbstractBox[viewNb - 1] = pDupPav;
					 pDupPav->AbTruncatedHead = TRUE;
				      }
				    if (pAb->AbPreviousRepeated != NULL)
				       pAb->AbPreviousRepeated->AbNextRepeated = pDupPav;
				    pDupPav->AbPreviousRepeated = pAb->AbPreviousRepeated;
				 }	/* fin cas forward */
			       else
				  /* cas !forward */
				  /* plus simple car il ne faut pas considerer */
				  /* le chainage avec l'element */
				 {
				    if (pPAvant != pAb)
				       /* il y a des paves de presentation avant */
				       /* on  chaine pPAvant a l'element */
				       pAb->AbElement->ElAbstractBox[viewNb - 1] = pPAvant;
				    /* chainage avec les dupliques */
				    if (pAb->AbNextRepeated != NULL)
				       pAb->AbNextRepeated->AbPreviousRepeated =
					  pDupPav;
				    pDupPav->AbNextRepeated = pAb->AbNextRepeated;
				    /* mise a jour de AbTruncatedTail */
				    pDupPav->AbTruncatedTail = TRUE;
				 }	/* fin cas !forward */
			       /*reapplique les regles de pres des paves qui */
			       /* faisaient reference a pAb */
			       /* est-ce utile ? */
			       ApplyRefAbsBoxSupp (pAb, &pAbb, pDoc);
			       /* suppression du pave pAb, ne pas le signaler */
			       /* au mediateur s'il n'a pas ete affiche */
			       if (!pAb->AbNew)
				 {
				    /* ces paves ont deja ete vus par le mediateur */
				    /* il faut lui signaler leur destruction */
				    h = 0;
				    if (AssocView (pEl))
				       frame = pDoc->DocAssocFrame[pEl->ElAssocNum - 1];
				    else
				       frame = pDoc->DocViewFrame[viewNb - 1];
				    bool = ChangeConcreteImage (frame, &h, pAb);
				 }
			       if (pAb->AbLeafType == LtPicture)
				 {
				    if (!pAb->AbElement->ElTerminal || pAb->AbElement->ElLeafType != LtPicture)
				       /* ce n'est pas un element image */
				       FreePictInfo (pAb->AbPictInfo);
				    pAb->AbPictInfo = NULL;
				 }
			       FreeAbstractBox (pAb);

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
				    pAbb = pDupPav->AbFirstEnclosed;
				    found = FALSE;
				    pPAvant = NULL;
				    do
				      {
					 if (pAbb->AbPresentationBox && pAbb->AbElement ==
					     pDupPav->AbElement)
					   {
					      /* recherche la regle correspondant au pave */
					      /* de presentation pAbb ; */
					      pRCre = GlobalSearchRulepEl (pDupPav->AbElement,
									   &pSPres, &pSchS, 0, NULL, viewSch,
									   PtFunction, FnAny, TRUE, FALSE, &pAttr);
					      stop = FALSE;
					      do
						 if (pRCre == NULL)
						    stop = TRUE;
						 else if (pRCre->PrType != PtFunction)
						    stop = TRUE;
						 else
						    /* c'est une regle de creation */
						   {
						      pBo1 = &pSPres->PsPresentBox
							 [pRCre->PrPresBox[0] - 1];
						      if (pRCre->PrViewNum == viewSch
							  && pSPres == pDupPav->AbElement->
							  ElAbstractBox[viewNb - 1]->AbPSchema
							  && pAbb->AbTypeNum == pRCre->PrPresBox[0])
							{
							   /* on a trouve la regle  de pAbb */
							   stop = TRUE;
							   /* regarde si c'est une regle de */
							   /* creation dernier fils */
							   if (pRCre->PrPresFunction == FnCreateLast)
							      found = TRUE;
							   /* il faut inserer pPavDupFils avant pAbb */
							}
						      else
							 pRCre = pRCre->PrNextPRule;
						   }
					      while (!stop && !found);
					   }
					 else	/* pAbb n'est pas un pave de pres de l'elt */
					    fprintf ("erreur chainage dup \n");
					 if (!found)
					   {
					      pPAvant = pAbb;
					      pAbb = pAbb->AbNext;
					   }
				      }
				    while (!found && pAbb != NULL);
				    /* en sortie, pPAvant contient le dernier pave correspondant */
				    /* a une regle FnCreateFirst ; et pAbb contient le premier */
				    /* pave correspondant a une regle FnCreateLast */
				    /* on insere pPavDupFils entre pAbb et pPrevious */
				    /* car il n'y a que des paves de pres premier fils */
				    if (pPAvant != NULL)
				      {
					 /* on insere pPavDupFils apres pPAvant */
					 pPAvant->AbNext = pPavDupFils;
					 pPavDupFils->AbPrevious = pPAvant;
				      }
				    if (pAbb != NULL)
				      {
					 pPavDupFils->AbNext = pAbb;
					 pAbb->AbPrevious = pPavDupFils;
				      }
				    /* si pAbb etait le premier fils de son pere, on chaine */
				    /* pPavDupFils a sa place */
				    if (pDupPav->AbFirstEnclosed == pAbb)
				       pDupPav->AbFirstEnclosed = pPavDupFils;
				 }	/* fin du cas ou pDupPav avait deja des fils */
			       else
				  pDupPav->AbFirstEnclosed = pPavDupFils;
			       /* chainage du (des) paves avec le pere */
			       pAbb = pPavDupFils;
			       while (pAbb != NULL &&
				  pAbb->AbElement == pPavDupFils->AbElement)
				 {
				    pAbb->AbEnclosing = pDupPav;
				    pAbb = pAbb->AbNext;
				 }
			    }	/* fin de 'PavDupFils non NULL */
			  /* pPavDupFils devient le premier pave de pres avant pDupPav */
			  /* pour boucle de duplication suivante */
			  pAbb = pDupPav;
			  while (pAbb->AbPrevious != NULL
				 && pAbb->AbPrevious->AbElement == pDupPav->AbElement)
			     pAbb = pAbb->AbPrevious;
			  pPavDupFils = pAbb;
		       }
		     filsdup = adupliquer;	/*pour savoir si le pere sera duplique */
		     pElAscent = pElAscent->ElParent;
		  }		/* fin boucle de duplication */
	     /* chainage du dernier pave duplique avec
	        le pave marque de page cree */
	     if (pPavDupFils != NULL)
	       {
		  /* chainage a l'englobant (y compris les paves de pres) */
		  /* pPavDupFils a ete positionne sur le 1er pave de pres */
		  pAbb1 = pPavDupFils;
		  /* pAbb1 est le premier pave de l'element pPavDupFils */
		  if (pNewAbbox->AbFirstEnclosed != NULL)
		     /* cas ou pNewAbbox a deja des paves fils (paves de presentation)
		      */
		     /* est-ce possible ? */
		    {
		       pAbb = pNewAbbox->AbFirstEnclosed;
		       while (pAbb->AbNext != NULL)
			  pAbb = pAbb->AbNext;
		       /* pAbb est le dernier fils, on chaine pAbb1 a pAbb */
		       pAbb->AbNext = pAbb1;
		       pAbb1->AbPrevious = pAbb;
		    }
		  else
		     /* le pave pAbb1 est le premier fils de pNewAbbox */
		     pNewAbbox->AbFirstEnclosed = pAbb1;
		  while (pAbb1 != NULL &&
			 pAbb1->AbElement == pPavDupFils->AbElement)
		    {
		       pAbb1->AbEnclosing = pNewAbbox;
		       pAbb1 = pAbb1->AbNext;
		    }
	       }

	     /* maintenant que le chainage est fini, on parcourt
	        le chemin cree pour appliquer les regles de dimension
	        et de position aux paves dupliques ; le parcours est
	        descendant a partir du dernier DupPave cree */
	     /* on applique aussi ces regles pour les paves de presentation */
	     pAbb = pNewAbbox->AbFirstEnclosed;
	     pDupPav = NULL;
	     while (pAbb != NULL)
	       {
		  for (typeRule = PtVertRef; typeRule <= PtHorizPos; typeRule++)
		    {
		       /* recherche de la regle */
		       pRule = SearchRulepAb (pDoc, pAbb, &pSPres, typeRule, FnAny, TRUE, &pAttr);
		       /* application de la regle */
		       if (!ApplyRule (pRule, pSPres, pAbb, pDoc, pAttr, &bool))
			  Delay (pRule, pSPres, pAbb, pAttr, pNewAbbox);
		    }
		  if (!pAbb->AbPresentationBox)
		    {
		       /* application des regles de creation repetees */
		       pRule = SearchRulepAb (pDoc, pAbb, &pSPres, PtFunction, FnAny, TRUE, &pAttr);
		       /* pRule est la premiere regle de creation si il y en a */
		       do
			  if (pRule != NULL)
			    {
			       if (pRule->PrPresBoxRepeat
				 && (pRule->PrPresFunction == FnCreateBefore
				   || pRule->PrPresFunction == FnCreateFirst
				   || pRule->PrPresFunction == FnCreateAfter
				  || pRule->PrPresFunction == FnCreateLast))
				  if (pAttr == NULL)
				     pAbbPres = CrAbsBoxesPres (pAbb->AbElement, pDoc, pRule,
								pAbb->AbElement->ElStructSchema, NULL, viewNb,
						       pSPres, FALSE, TRUE);
				  else
				     pAbbPres = CrAbsBoxesPres (pAbb->AbElement, pDoc, pRule,
					pAttr->AeAttrSSchema, pAttr, viewNb,
								pAttr->AeAttrSSchema->SsPSchema, FALSE, TRUE);
			       pRule = pRule->PrNextPRule;
			    }
		       while (pRule != NULL && pRule->PrType == PtFunction) ;
		    }
		  /* modification de la regle de position verticale ? */
		  /* il suffit de laisser PosAbRef a NULL car le mediateur */
		  /* appliquera cette regle par defaut */
		  /* on memorise le pave duplique (qui n'est pas de pres) */
		  if (!pAbb->AbPresentationBox)
		     pDupPav = pAbb;
		  /* on passe au pave suivant */
		  if (pAbb->AbNext == NULL)
		     if (pDupPav != NULL)
			/* on s'arrete si pAbb est le pave de presentation */
			/* fils du dernier duplique */
			/* on passe au duplique suivant */
		       {
			  pAbb = pDupPav->AbFirstEnclosed;
			  pDupPav = NULL;
		       }
		     else
			pAbb = NULL;
		  else
		     pAbb = pAbb->AbNext;
	       }		/* fin application des regles de positionnement */
	     /* on repositionne pEl et pElSauv si !forward */
	     if (!forward)
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
	pAbbox1 = pNewAbbox;
	pAbbox1->AbAcceptLineBreak = TRUE;
	pAbbox1->AbAcceptPageBreak = TRUE;
	pAbbox1->AbInLine = FALSE;
	pAbbox1->AbNotInLine = FALSE;
	pAbbox1->AbTruncatedHead = FALSE;
	pAbbox1->AbTruncatedTail = FALSE;
	/* premiere regle de presentation de la boite page */
	pRSpecPage = pSchPPage->PsPresentBox[TypeP - 1].PbFirstPRule;
	/* premiere regle de presentation par defaut */
	pRDefPage = pSchPPage->PsFirstDefaultPRule;

	/* application des regles de la boite marque de page */
	/* ou colonne */

	/* code de creation des boites haut et bas de page */
	do
	  {
	     pRule = GetRule (&pRSpecPage, &pRDefPage, pEl, NULL,
			      pEl->ElStructSchema);
	     /* pointeur sur la regle a appliquer pour la vue 1 */
	     if (pRule != NULL)
		if (pRule->PrType == PtFunction
		/* parmi les fonctions, on ne traite que les */
		/* regles CreateBefore et CreateAfter */
		    && (pRule->PrPresFunction == FnCreateBefore
			|| pRule->PrPresFunction == FnCreateAfter))
		   pAbbPres = CrAbsBoxesPres (pEl, pDoc, pRule,
					      pEl->ElStructSchema,
					      NULL, viewNb, pSchPPage,
					      FALSE, TRUE);
		else
		   /* ce n'est pas une fonction de presentation */
		   /* cherche et applique la regle de meme type */
		   /* pour la vue */
		   /* on n'applique pas la regle de visibilite */
		   /* car si !forward l'elt pere n'est pas cree */
		   /* et la regle donne 0 si visib = visib(pere) */

		if (pRule->PrType != PtVisibility)
		   for (nv = 1; nv <= MAX_VIEW; nv++)
		     {
			if (nv == 1)
			   pRegleV = NULL;
			else
			   pRegleV = GetRuleView (&pRSpecPage,
					      &pRDefPage, pRule->PrType, nv,
					    pEl, NULL, pEl->ElStructSchema);
			if (nv == viewSch &&
			    DoesViewExist (pEl, pDoc, viewNb))
			  {
			     if (pRegleV == NULL)
				pRegleV = pRule;
			     if (!ApplyRule (pRegleV, pSchPPage,
					     pNewAbbox, pDoc, NULL, arret))
				WaitingRule (pRegleV, pNewAbbox, pSchPPage,
					     NULL, queuePA, queuePS,
					     queuePP, queuePR, lqueue);

			     else
				/* la regle a ete appliquee */
				/* on teste arret pour savoir si une le pave a ete */
				/* detruit suite a l'application d'une regle */
				/* page : il faut sortir de AbsBoxesCreate pour */
				/* recommencer la creation des fils du pere de pEl */
				/* car un fils marque page a ete ajoute avant pEl */
				/* arret est un parametre retour de */
				/* CreePageCol */ if (*arret)
				/* la destruction de pNewAbbox a ete faite */
				return;		/* on sort de CreePageCol */
			     /* TODO : est-ce correct ? y-a-il des regles */
			     /* en attente ou retardees ??? */
			  }
		     }		/* fin du for nv =1 */
	  }
	while (pRule != NULL);
	/* si pEl est une marque de page, */
	/* on calcule la hauteur du haut et du bas de la page */
	/* et BreakPageHeight qui sont des variables */
	/* globales (page_tv.h) */
	if (RunningPaginate && (pEl->ElPageType == PgBegin
				|| pEl->ElPageType == PgComputed
				|| pEl->ElPageType == PgUser))
	  {
	     if (AssocView (pEl))
		frame = pDoc->DocAssocFrame[pEl->ElAssocNum - 1];
	     else
		frame = pDoc->DocViewFrame[viewNb - 1];
	     h = -1;		/* changement de signification de h */
	     /* on signale au mediateur les paves */
	     /* (au cas ou il ne les ait pas encore vus) */
	     /* on appelle Modifvue */
	     bool = ChangeConcreteImage (frame, &h, pAbbRoot);
	     pAbb = pEl->ElAbstractBox[viewNb - 1];
	     if (pAbb->AbPresentationBox)
	       {
		  /* il y a un haut de page : on evalue sa hauteur */
		  pAb = pAbb;
		  /* calcul de la hauteur du pave haut de page */
		  SetPageHeight (pAb, TRUE, &Hauteur, &PosV, &CarCoupe);
		  PageHeaderHeight = Hauteur;
	       }
	     /* on saute le corps de page pour voir s'il y a un bas de page */
	     while (pAbb->AbPresentationBox)
		pAbb = pAbb->AbNext;
	     /* pAbb est le corps de page */
	     if (pAbb->AbNext != NULL
		 && pAbb->AbNext->AbElement == pEl
		 && pAbb->AbNext->AbLeafType == LtCompound)
		/* pas filet */
	       {
		  /* il y a un bas : on evalue sa hauteur */
		  pAb = pAbb->AbNext;
		  /* calcul de la hauteur du pave bas de page */
		  SetPageHeight (pAb, TRUE, &Hauteur, &PosV, &CarCoupe);
		  PageFooterHeight = Hauteur;
	       }
	     if (pEl->ElPageType == PgBegin)
		/* nouvelle regle :on met a jour la hauteur totale de la page */
		/* hauteur corps = pAbb->AbHeight.DimValue toujours exprime en  */
		/* unite fixe (verifie par le compilo) */
		WholePageHeight = PageHeaderHeight +
		   pAbb->AbHeight.DimValue + PageFooterHeight;
	     /* WholePageHeight = hauteur max totale de la page */
	     /* definie par l'utilisateur ; exemple A4 = 29.7 cm */
	     BreakPageHeight = WholePageHeight - PageFooterHeight;
	  }			/* fin init WholePageHeight si RunningPaginate */

	/* ApplyRule les regles de presentation specifiques */
	/* de cet element */
	pRule = pEl->ElFirstPRule;
	while (pRule != NULL)
	   /* applique une regle si elle concerne la vue */
	  {
	     if (pRule->PrViewNum == viewSch && pNewAbbox != NULL
		 && DoesViewExist (pEl, pDoc, viewNb))
		/* on ne considere pas les attributs */
		if (!ApplyRule (pRule, pSchPPage, pNewAbbox, pDoc, NULL, &bool))
		   WaitingRule (pRule, pNewAbbox, pSchPPage, NULL, queuePA,
				queuePS, queuePP, queuePR, lqueue);
	     pRule = pRule->PrNextPRule;
	  }
	pDimAb = &pAbbox1->AbHeight;
	/* en attendant que le compilateur genere la regle de */
	/* dimension minimale, on le fait a la main : la */
	/* valeur generee par la regle doit etre interpretee */
	/* par le mediateur comme minimale */
	pDimAb->DimMinimum = TRUE;

	/* le contenu du filet de changement de page */
	/* est engendre directement par un pave de presentation */
	/* si le corps de page a un volume nul cas de page vide */
	/* on le force a 1 pour qu'il n'y ait pas de grise' */
	if (pAbbox1->AbVolume == 0)
	   pAbbox1->AbVolume = 1;
	if (!forward && pElSauv != NULL &&
	    (pElSauv->ElPageType == PgBegin
	     || pElSauv->ElPageType == PgComputed
	     || pElSauv->ElPageType == PgUser))
	   /* si pElSauv est une marque page et si !forward, il faut */
	   /* creer le pave ColGroup precedent pour permettre */
	   /* la creation des colonnes a placer dedans */
	  {
	     stop = FALSE;
	     pEl1 = pElSauv;
	     while (pEl1 != NULL && !stop)
	       {
		  pEl1 = BackSearchTypedElem (pEl1, PageBreak + 1, NULL);
		  if (pEl1 != NULL)
		     /* on saute les marques des autres vues */
		     /* et les marques colonnes simples */
		     if (pEl1->ElViewPSchema == viewSch &&
			 (pEl1->ElPageType == ColGroup
			  || pEl1->ElPageType == PgBegin
			  || pEl1->ElPageType == PgComputed
			  || pEl1->ElPageType == PgUser))
			stop = TRUE;
	       }
	     if (pEl1 != NULL && pEl1->ElPageType == ColGroup)
		/* si pEl1 est une marque ColGroup, c'est la derniere */
		/* de la page precedente : on cree son pave */
		/* pour permettre la suite de la creation */
		/* des colonnes situees avant pElSauv */
		pAb = AbsBoxesCreate (pEl1, pDoc, viewNb, forward, TRUE,
				      &FilsComplet);
	  }
	if (!forward && pEl->ElPageType == ColGroup)
	   /* si pEl est une ColGroup, */
	   /* il faut creer le pave de la derniere */
	   /* colonne simple de ce groupe de colonnes */
	   /* ceci est vrai meme si pElSauv != NULL */
	  {
	     stop = FALSE;
	     pEl1 = pEl;
	     pPage = NULL;
	     while (pEl1 != NULL && !stop)
	       {		/* recherche en avant */
		  pEl1 = FwdSearchTypedElem (pEl1, PageBreak + 1, NULL);
		  if (pEl1 != NULL && pEl1->ElViewPSchema == viewSch)
		     if (pEl1->ElPageType == ColBegin
			 || pEl1->ElPageType == ColComputed
			 || pEl1->ElPageType == ColUser)
			pPage = pEl1;	/* on continue la recherche pour */
		  /* trouver la derniere colonne simple */
		     else	/* on est sur la marque Colgroupees suivante */
			/* ou sur la marque page suivante, donc l'element */
			/* stocke dans pPage est bien la derniere col */
			/* simple du groupe ;  on s'arrete */
			stop = TRUE;
	       }
	     if (pPage != NULL)
		pAb = AbsBoxesCreate (pPage, pDoc, viewNb, forward, TRUE,
				      &FilsComplet);
	  }
     }
}				/* end of CreePageCol */

#endif /* __COLPAGE__ */
