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

#include "ustring.h"
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

#include "absboxes_f.h"
#include "applicationapi_f.h"
#include "attributes_f.h"
#include "attrpresent_f.h"
#include "boxpositions_f.h"
#include "buildboxes_f.h"
#include "changeabsbox_f.h"
#include "createabsbox_f.h"
#include "createpages_f.h"
#include "memory_f.h"
#include "presvariables_f.h"
#include "presrules_f.h"
#include "schemas_f.h"
#include "structlist_f.h"
#include "structmodif_f.h"
#include "thotmsg_f.h"
#include "tree_f.h"


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
   ThotBool            stop;
   PtrPRule            pRe1;

   TypeP = 0;
   if (pEl != NULL)
     {
	pElAscent = pEl->ElParent;
	if (pElAscent != NULL)
	  {
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
   pAbbChild = InitAbsBoxes (pEl, viewNb, 10, TRUE);
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
   pAbbChild->AbFontStyle = 0;
   pAbbChild->AbFontWeight = 0;
   pAbbChild->AbUnderline = 0;
   pAbbChild->AbThickness = 0;
   pAbbChild->AbLineWeight = 1;
   pAbbChild->AbLineWeightUnit = UnPoint;
   pAbbChild->AbVisibility = pNewAbbox->AbVisibility;
}
