/*
 *
 *  (c) COPYRIGHT INRIA, Grif, 1996.
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
 * Ce module effectue la creation des images abstraites
 *
 * Authors: V. Quint (INRIA)
 *          C. Roisin (INRIA) - Columns and pages
 *
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "picture.h"
#include "message.h"
#include "language.h"
#include "appdialogue.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "page_tv.h"
#include "platform_tv.h"
#include "edit_tv.h"
#include "appdialogue_tv.h"

#include "tree_f.h"
#include "attrpresent_f.h"
#include "attributes_f.h"
#include "createabsbox_f.h"
#include "createpages_f.h"
#include "exceptions_f.h"
#include "absboxes_f.h"
#include "abspictures_f.h"
#include "memory_f.h"
#include "structmodif_f.h"
#include "changeabsbox_f.h"
#include "presrules_f.h"
#include "references_f.h"
#include "schemas_f.h"
#include "boxselection_f.h"
#include "content_f.h"
#include "presvariables_f.h"



/*----------------------------------------------------------------------
   GetRule rend la regle de presentation a` appliquer pour la vue  
   numero 1. pRSpecif est la regle specifique courante,    
   pRDefault est la regle par defaut courante. Au retour    
   ces deux pointeurs sont mis a` jour pour la regle       
   suivante.                                               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PtrPRule            GetRule (PtrPRule * pRSpecif, PtrPRule * pRDefault,
			 PtrElement pEl, PtrAttribute pAttr, PtrSSchema pSS)
#else  /* __STDC__ */
PtrPRule            GetRule (pRSpecif, pRDefault, pEl, pAttr, pSS)
PtrPRule           *pRSpecif;
PtrPRule           *pRDefault;
PtrElement          pEl;
PtrAttribute        pAttr;
PtrSSchema          pSS;
#endif /* __STDC__ */
{
   PtrPRule            pPR;
   boolean             stop;

   pPR = NULL;
   if (*pRSpecif != NULL && *pRDefault != NULL)
     {
	/* Il n'y a pas de fonction par defaut. Donc si on est sur une regle */
	/* specifique de type fonction, on la prend, si ses conditions */
	/* d'application sont satisfaites. */
	if ((*pRSpecif)->PrType == PtFunction &&
	    (*pRDefault)->PrType > PtFunction)
	  {
	     stop = FALSE;
	     do
	       {
		  if ((*pRSpecif)->PrCond == NULL ||
		  CondPresentation ((*pRSpecif)->PrCond, pEl, pAttr, 1, pSS))
		     /* condition d'application satisfaites */
		    {
		       pPR = *pRSpecif;
		       stop = TRUE;
		    }
		  /* prochaine regle specifique a traiter */
		  *pRSpecif = (*pRSpecif)->PrNextPRule;
		  if (!stop)
		     /* la condition n'est pas satisfaite, on essaie les
			fonctions suivantes */
		     if (*pRSpecif == NULL)
			/* pas de regle specifique suivante, on arrete */
			stop = TRUE;
		     else if ((*pRSpecif)->PrType != PtFunction)
			/* la regle specifique suivante n'est pas une Fonction,
			   on arrete */
			stop = TRUE;
	       }
	     while (!stop);
	  }
     }
   if (pPR == NULL)
      if (*pRDefault != NULL)
	{
	   if (*pRSpecif != NULL)
	      if ((*pRSpecif)->PrType == (*pRDefault)->PrType &&
		  (*pRSpecif)->PrViewNum == 1)
		 /* pour la vue 1, la regle specifique de meme type que la */
		 /* regle par defaut existe */
		{
		   stop = FALSE;
		   /* on traite toutes les regles specifiques successives de ce
		      type et qui concernent la vue 1, et on retient la
		      premiere dont les conditions d'application sont OKs */
		   do
		     {
			if (pPR == NULL)
			   /* on n'a pas encore trouve' la bonne regle */
			   if ((*pRSpecif)->PrCond == NULL ||
			       CondPresentation ((*pRSpecif)->PrCond, pEl,
								pAttr, 1, pSS))
			      /* les conditions d'application sont satisfaites,
				 on prend cette regle */
			      pPR = *pRSpecif;
			if ((*pRSpecif)->PrNextPRule == NULL)
			   stop = TRUE;
			else if ((*pRSpecif)->PrNextPRule->PrType ==
						     (*pRDefault)->PrType &&
				 (*pRSpecif)->PrNextPRule->PrViewNum == 1)
			   *pRSpecif = (*pRSpecif)->PrNextPRule;
			else
			   stop = TRUE;
		     }
		   while (!stop);
		   /* avance d'une regle */
		   if (*pRSpecif != NULL)
		      *pRSpecif = (*pRSpecif)->PrNextPRule;
		   if (pPR != NULL)
		      *pRDefault = (*pRDefault)->PrNextPRule;
		}
	   if (pPR == NULL)
	      /* on prend la regle par defaut */
	      /* les regles par defaut n'ont pas de condition */
	     {
		pPR = *pRDefault;
		*pRDefault = (*pRDefault)->PrNextPRule;
	     }
	}
   return pPR;
}


/*----------------------------------------------------------------------
   GetRuleView donne la regle du type Typ a` appliquer pour la vue 
   Vue. pRSpecif et pRDefault sont mis a` jour. La fonction 
   retourne NULL si il n'y a pas de regle de ce type pour  
   cette vue (dans ce cas, il faut alors appliquer la      
   regle de la vue 1).                                     
  ----------------------------------------------------------------------*/

#ifdef __STDC__
PtrPRule            GetRuleView (PtrPRule * pRSpecif, PtrPRule * pRDefault,
				 PRuleType Typ, int Vue, PtrElement pEl,
				 PtrAttribute pAttr, PtrSSchema pSS)

#else  /* __STDC__ */
PtrPRule            GetRuleView (pRSpecif, pRDefault, Typ, Vue, pEl, pAttr, pSS)
PtrPRule           *pRSpecif;
PtrPRule           *pRDefault;
PRuleType           Typ;
int                 Vue;
PtrElement          pEl;
PtrAttribute        pAttr;
PtrSSchema          pSS;

#endif /* __STDC__ */

{
   PtrPRule            pPR;
   boolean             stop;

   pPR = NULL;
   if (*pRSpecif != NULL)
      if ((*pRSpecif)->PrViewNum == Vue)
	 if ((*pRSpecif)->PrType == Typ)
	   {
	      stop = FALSE;
	      /* on traite toutes les regles successives de meme type et qui */
	      /* concernent la meme vue */
	      do
		{
		   if (pPR == NULL)
		      /* on n'a pas encore trouve' la regle qui s'applique */
		      if ((*pRSpecif)->PrCond == NULL ||
			  CondPresentation ((*pRSpecif)->PrCond, pEl, pAttr, Vue, pSS))
			 /* les conditions d'application de la regle sont satisfaites, */
			 /* on la prend */
			 pPR = *pRSpecif;
		   if ((*pRSpecif)->PrNextPRule == NULL)
		      stop = TRUE;
		   else if ((*pRSpecif)->PrNextPRule->PrViewNum == Vue &&
			    (*pRSpecif)->PrNextPRule->PrType == Typ)
		      *pRSpecif = (*pRSpecif)->PrNextPRule;
		   else
		      stop = TRUE;
		}
	      while (!stop);
	      /* avance d'une regle */
	      if (*pRSpecif != NULL)
		 *pRSpecif = (*pRSpecif)->PrNextPRule;
	      if (*pRDefault != NULL)
		 if ((*pRDefault)->PrType == Typ
		     && (*pRDefault)->PrViewNum == Vue)
		    *pRDefault = (*pRDefault)->PrNextPRule;
	   }
   if (pPR == NULL)
      if (*pRDefault != NULL)
	 if ((*pRDefault)->PrType == Typ)
	    if ((*pRDefault)->PrViewNum == Vue)
	       /* on prend la regle par defaut */
	      {
		 pPR = *pRDefault;
		 *pRDefault = (*pRDefault)->PrNextPRule;
	      }
   return pPR;
}


/*----------------------------------------------------------------------
   InitAbsBoxes cree et initialise un pave pour l'element pEl et dans  
   la vue view, avec la visibilite Visib.                    
  ----------------------------------------------------------------------*/

#ifdef __STDC__
PtrAbstractBox      InitAbsBoxes (PtrElement pEl, DocViewNumber view, int Visib)

#else  /* __STDC__ */
PtrAbstractBox      InitAbsBoxes (pEl, view, Visib)
PtrElement          pEl;
DocViewNumber       view;
int                 Visib;

#endif /* __STDC__ */

{
   PtrAbstractBox      pAb;
   PtrElement          pAscend;
   boolean             stop;

   GetAbstractBox (&pAb);
   pAb->AbElement = pEl;
   pAb->AbEnclosing = NULL;
   pAb->AbNext = NULL;
   pAb->AbPrevious = NULL;
   pAb->AbFirstEnclosed = NULL;
   pAb->AbPSchema = NULL;
   pAb->AbCreatorAttr = NULL;
   pAb->AbDocView = view;
   pAb->AbInLine = TRUE;
   pAb->AbVisibility = Visib;

   pAb->AbHorizRef.PosEdge = HorizRef;
   pAb->AbHorizRef.PosRefEdge = HorizRef;
   pAb->AbHorizRef.PosDistance = 0;
   pAb->AbHorizRef.PosUnit = UnRelative;
   pAb->AbHorizRef.PosAbRef = NULL;
   pAb->AbHorizRef.PosUserSpecified = FALSE;

   pAb->AbVertRef.PosEdge = VertRef;
   pAb->AbVertRef.PosRefEdge = VertRef;
   pAb->AbVertRef.PosDistance = 0;
   pAb->AbVertRef.PosUnit = UnRelative;
   pAb->AbVertRef.PosAbRef = NULL;
   pAb->AbVertRef.PosUserSpecified = FALSE;

   pAb->AbVertPos.PosEdge = Top;
   pAb->AbVertPos.PosRefEdge = Top;
   pAb->AbVertPos.PosDistance = 0;
   pAb->AbVertPos.PosUnit = UnRelative;
   pAb->AbVertPos.PosAbRef = NULL;
   pAb->AbVertPos.PosUserSpecified = FALSE;

   pAb->AbHorizPos.PosEdge = Left;
   pAb->AbHorizPos.PosRefEdge = Left;
   pAb->AbHorizPos.PosDistance = 0;
   pAb->AbHorizPos.PosUnit = UnRelative;
   pAb->AbHorizPos.PosAbRef = NULL;
   pAb->AbHorizPos.PosUserSpecified = FALSE;

   pAb->AbBox = NULL;
   pAb->AbSelected = FALSE;

   pAb->AbHeight.DimIsPosition = FALSE;
   pAb->AbHeight.DimValue = 0;
   pAb->AbHeight.DimAbRef = NULL;
   pAb->AbHeight.DimUnit = UnRelative;
   pAb->AbHeight.DimSameDimension = TRUE;
   pAb->AbHeight.DimUserSpecified = FALSE;
   pAb->AbHeight.DimMinimum = FALSE;

   pAb->AbWidth.DimIsPosition = FALSE;
   pAb->AbWidth.DimValue = 0;
   pAb->AbWidth.DimAbRef = NULL;
   pAb->AbWidth.DimUnit = UnRelative;
   pAb->AbWidth.DimSameDimension = TRUE;
   pAb->AbWidth.DimUserSpecified = FALSE;
   pAb->AbWidth.DimMinimum = FALSE;

   pAb->AbVolume = 0;
   pAb->AbIndent = 0;
   pAb->AbDepth = 0;
   pAb->AbNum = 0;
   pAb->AbVisibility = Visib;
   pAb->AbFont = 'T';
   pAb->AbHighlight = 0;
   pAb->AbUnderline = 0;
   pAb->AbThickness = 0;
   pAb->AbSize = 1;
   pAb->AbSizeUnit = UnRelative;
   pAb->AbFillPattern = 0;
   pAb->AbBackground = 0;	/* blanc */
   pAb->AbForeground = 1;	/* noir */
   pAb->AbLineStyle = 'S';
   pAb->AbLineWeight = 1;
   pAb->AbLineWeightUnit = UnPoint;
   /* les copies de parametres ou d'elements inclus ne sont pas */
   /* modifiables par l'utilisateur */
   pAb->AbCanBeModified = !pEl->ElIsCopy;
   /* les constantes ne sont pas modifiables par l'utilisateur */
   if (pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrConstruct == CsConstant)
      pAb->AbCanBeModified = FALSE;
   pAb->AbSelected = FALSE;
   pAb->AbOnPageBreak = FALSE;
   pAb->AbAfterPageBreak = FALSE;
   pAb->AbLineSpacing = 10;
   pAb->AbAdjust = AlignLeft;
   pAb->AbJustify = FALSE;
   pAb->AbLineSpacingUnit = UnRelative;
   pAb->AbIndentUnit = UnRelative;
   pAb->AbAcceptLineBreak = TRUE;
   pAb->AbAcceptPageBreak = TRUE;
   pAb->AbNotInLine = FALSE;
   pAb->AbSensitive = FALSE;
   /* cherche si l'element est une reference ou une copie d'element inclus */
   if (pEl->ElTerminal)
      if (pEl->ElLeafType == LtReference)
	 /* une reference = un pave actif (double-cliquable) */
	 pAb->AbSensitive = TRUE;
   if (pEl->ElIsCopy)
      /* un element appartenant a un element copie' = pave actif, sauf */
      /* s'il s'agit d'un parametre. */
     {
	pAb->AbSensitive = TRUE;
	pAscend = pEl;
	while (pAscend != NULL)	/* un ascendant est-il un parametre ? */
	   if (pAscend->ElStructSchema->SsRule[pAscend->ElTypeNumber - 1].SrParamElem)
	     {
		pAscend = NULL;
		pAb->AbSensitive = FALSE;	/* un parametre n'est pas actif */
	     }
	   else
	      pAscend = pAscend->ElParent;
     }
   pAb->AbReadOnly = FALSE;
   if (ElementIsReadOnly (pEl))
     {
	pAb->AbCanBeModified = FALSE;
	pAb->AbReadOnly = TRUE;
     }
   else
     {
	/* cherche si un ascendant est un parametre */
	pAscend = pEl;
	stop = FALSE;
	do
	   if (pAscend == NULL)
	      stop = TRUE;
	   else if (pAscend->ElStructSchema->SsRule[pAscend->ElTypeNumber - 1].SrParamElem)
	     {
		/* le contenu d'un parametre ne peut pas etre modifie */
		stop = TRUE;
		pAb->AbReadOnly = TRUE;
		pAb->AbCanBeModified = FALSE;
	     }
	   else
	      pAscend = pAscend->ElParent;
	while (!stop);
     }
   pAb->AbHyphenate = FALSE;
   pAb->AbPresentationBox = FALSE;
#ifdef __COLPAGE__
   pAb->AbRepeatedPresBox = FALSE;
#endif /* __COLPAGE__ */
   pAb->AbTypeNum = pEl->ElTypeNumber;
   /* ce pave respecte les regles d'englobement */
   pAb->AbHorizEnclosing = TRUE;
   pAb->AbVertEnclosing = TRUE;
   pAb->AbDelayedPRule = NULL;
   pAb->AbText = NULL;
   pAb->AbNew = TRUE;
   pAb->AbDead = FALSE;
   pAb->AbWidthChange = FALSE;
   pAb->AbHeightChange = FALSE;
   pAb->AbHorizPosChange = FALSE;
   pAb->AbVertPosChange = FALSE;
   pAb->AbHorizRefChange = FALSE;
   pAb->AbVertRefChange = FALSE;
   pAb->AbSizeChange = FALSE;
   pAb->AbAspectChange = FALSE;
   pAb->AbChange = FALSE;

   if (pEl->ElTerminal && pEl->ElLeafType == LtText)
     {
	pAb->AbLeafType = LtText;
	pAb->AbLanguage = TtaGetDefaultLanguage ();
     }
   return pAb;
}


/*----------------------------------------------------------------------
   ConstantCopy met dans le pave pAb le text correspondant a`  
   la constante de numero NConst dans le schema de         
   presentation pSchP.                                     
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                ConstantCopy (int NConst, PtrPSchema pSchP, PtrAbstractBox pAb)

#else  /* __STDC__ */
void                ConstantCopy (NConst, pSchP, pAb)
int                 NConst;
PtrPSchema          pSchP;
PtrAbstractBox      pAb;

#endif /* __STDC__ */

{
   PresConstant       *pConst;

   pConst = &pSchP->PsConstant[NConst - 1];
   switch (pConst->PdType)
	 {
	    case GraphicElem:
	       pAb->AbLeafType = LtGraphics;
	       pAb->AbShape = pConst->PdString[0];
	       pAb->AbGraphAlphabet = 'G';
	       pAb->AbVolume = 1;
	       break;
	    case Symbol:
	       pAb->AbLeafType = LtSymbol;
	       pAb->AbShape = pConst->PdString[0];
	       pAb->AbGraphAlphabet = 'G';
	       if (pAb->AbShape == EOS)
		  pAb->AbVolume = 0;
	       else
		  pAb->AbVolume = 1;
	       break;
	    case CharString:
	       pAb->AbLeafType = LtText;
	       if (pAb->AbText == NULL)
		  GetConstantBuffer (pAb);
	       strncpy (pAb->AbText->BuContent, pConst->PdString, THOT_MAX_CHAR - 1);
	       pAb->AbText->BuContent[THOT_MAX_CHAR - 1] = EOS;
	       pAb->AbText->BuLength = strlen (pAb->AbText->BuContent);
	       pAb->AbLanguage = TtaGetLanguageIdFromAlphabet (pConst->PdAlphabet);
	       pAb->AbVolume = pAb->AbText->BuLength;
	       break;
	    case Picture:
	       NewPictInfo (pAb, pConst->PdString, UNKNOWN_FORMAT);
	       pAb->AbLeafType = LtPicture;
	       pAb->AbVolume = 100;
	       break;
	    default:
	       break;
	 }
}


/*----------------------------------------------------------------------
   AssocView retourne vrai si l'element pEl s'affiche dans une vue  
   d'elements associes.                                    
  ----------------------------------------------------------------------*/

#ifdef __STDC__
boolean             AssocView (PtrElement pEl)

#else  /* __STDC__ */
boolean             AssocView (pEl)
PtrElement          pEl;

#endif /* __STDC__ */

{
   int                 nR;
   boolean             assocView;

   assocView = FALSE;
   if (pEl != NULL)
      if (pEl->ElAssocNum != 0)
	 /* l'element est dans un element associe */
	{
	   /* on remonte a la racine de l'arbre associe' */
	   while (pEl->ElParent != NULL)
	      pEl = pEl->ElParent;
	   nR = pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrListItem;
	   /* si l'element associe ne s'affiche pas en haut ou en bas de */
	   /* page, il s'affiche donc dans une vue specifique */
	   if (pEl->ElStructSchema->SsPSchema != NULL)
	      assocView = !pEl->ElStructSchema->SsPSchema->PsInPageHeaderOrFooter[nR - 1];
	}
   return assocView;
}


/*----------------------------------------------------------------------
   DoesViewExist retourne vrai si la vue de numero viewNb ou` doit      
   s'afficher l'element pEl existe.                        
  ----------------------------------------------------------------------*/

#ifdef __STDC__
boolean             DoesViewExist (PtrElement pEl, PtrDocument pDoc, DocViewNumber viewNb)

#else  /* __STDC__ */
boolean             DoesViewExist (pEl, pDoc, viewNb)
PtrElement          pEl;
PtrDocument         pDoc;
DocViewNumber       viewNb;

#endif /* __STDC__ */

{
   boolean             existView;

   if (AssocView (pEl))
     {
	/* c'est une vue d'elements associes, elle existe */
	/* si la fenetre correspondante existe */
	existView = pDoc->DocAssocVolume[pEl->ElAssocNum - 1] != 0;
     }
   else
     {
	/* c'est une vue de l'arbre principal du document, elle existe */
	/* si l'entree correspondante de la table des vues du document */
	/* n'est pas libre. */
	existView = pDoc->DocView[viewNb - 1].DvPSchemaView != 0;
     }
   return existView;
}

/*----------------------------------------------------------------------
   AppliedView retourne le numero de la vue (numero dans le      
   schema de presentation qui s'applique a` l'element pEl  
   ou a` l'attribut pAttr) dont il faut appliquer les      
   regles de presentation a` l'element pEl pour obtenir    
   son image dans la vue viewNb du document pDoc.           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 AppliedView (PtrElement pEl, PtrAttribute pAttr, PtrDocument pDoc,
				 DocViewNumber viewNb)
#else  /* __STDC__ */
int                 AppliedView (pEl, pAttr, pDoc, viewNb)
PtrElement          pEl;
PtrAttribute        pAttr;
PtrDocument         pDoc;
DocViewNumber       viewNb;

#endif /* __STDC__ */
{
   int                 viewSch;
   PtrElement          pAsc;
   PtrSSchema          pSS;
   DocViewDescr       *pView;

   viewSch = 0;
   if (pEl->ElAssocNum != 0)
     {
	/* c'est un element associe', on applique */
	/* toujours les regles de la vue 1. */
	viewSch = 1;
     }
   else if (pDoc->DocView[viewNb - 1].DvPSchemaView != 0)
     {
	if (pAttr != NULL)
	   pSS = pAttr->AeAttrSSchema;
	else if (pEl != NULL)
	   pSS = pEl->ElStructSchema;
	else
	   pSS = NULL;
	if (pSS != NULL)
	  {
	     pView = &pDoc->DocView[viewNb - 1];
	     if (pView->DvSSchema == pDoc->DocSSchema)
		if (pSS->SsCode == pDoc->DocSSchema->SsCode)
		  {
		     /* c'est une element du document lui-meme */
		     viewSch = pView->DvPSchemaView;
		     /* numero de la vue dans le schema de */
		     /* presentation qui la definit */
		  }
		else
		   /* c'est un element d'un objet d'une autre nature */
		   viewSch = 1;
	     /* c'est une vue propre a une nature d'objets */
	     else if (pSS->SsPSchema == pView->DvSSchema->SsPSchema
		      || pEl->ElParent == NULL)
		/* c'est un objet de cette nature ou la racine du document */
		/* numero de la vue dans le schema de */
		/* presentation qui la definit */
		viewSch = pView->DvPSchemaView;
	     /* on applique les regles de presentation de la vue */
	     /* 1 de cette nature pour toutes les vues du document */
	     else
		/* l'element est-il dans un objet de cette nature ? */
		/* a priori non, donc l'element ne doit pas apparaitre dans */
		/* cette vue */
	       {
		  pAsc = pEl;
		  while (pAsc != NULL && viewSch == 0)
		     if (pAsc->ElStructSchema->SsPSchema == pView->DvSSchema->SsPSchema)
			/* on est dans un objet de cette nature */
			/* on applique les regles de la vue 1 */
			viewSch = 1;
		     else
			pAsc = pAsc->ElParent;
	       }
	  }
     }
   return viewSch;
}


/*----------------------------------------------------------------------
   Delay met une regle de presentation en attente au niveau du   
   pave englobant le pave pPRP. Cette regle sera           
   reappliquee lorsque tous les paves freres de pPRP auront
   ete crees.                                              
   - pR: SRule de presentation a` mettre en attente.       
   - pSP: schema de presentation auquel appartient cette   
   regle.                                                  
   - pAbb: pave auquel doit s'appliquer la regle pR.         
   - pAttr: attribut auquel correspond cette regle ou NULL 
   si ce n'est pas une regle de presentation d'attribut.   
   Si l'englobant est un pave page ou un pave duplique,    
   on met la regle en attente au niveau de la racine.      
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                Delay (PtrPRule pR, PtrPSchema pSP, PtrAbstractBox pAbb, PtrAttribute pAttr,
			   PtrAbstractBox pPRP)

#else  /* __STDC__ */
void                Delay (pR, pSP, pAbb, pAttr, pPRP)
PtrPRule            pR;
PtrPSchema          pSP;
PtrAbstractBox      pAbb;
PtrAttribute        pAttr;
PtrAbstractBox      pPRP;

#endif /* __STDC__ */

{
   PtrDelayedPRule     pDelR;
   PtrDelayedPRule     NpDelR;
   PtrAbstractBox      pAb;

   pAb = NULL;
   if (pPRP->AbEnclosing != NULL)
     {
	pAb = pPRP->AbEnclosing;
	/* si ce pave est un pave de presentation cree par la regle */
	/* FnCreateEnclosing, on met la regle en attente sur le pave englobant */
	if (pAb->AbEnclosing != NULL)
	   if (pAb->AbPresentationBox &&
	       pAb->AbElement == pPRP->AbElement)
	      pAb = pAb->AbEnclosing;
     }
#ifdef __COLPAGE__
   /* si pPRP->AbEnclosing est NULL, on retarde sur ce pave */
   else
      pAb = pPRP;
   /* si l'englobant est un pave de page, */
   /* on retarde la regle sur le pave racine */
   /* si le pave englobant est un pave duplique, on fait de meme */
   if ((pAb->AbElement->ElTerminal && pAb->AbElement->ElLeafType == LtPageColBreak)
       || pAb->AbNextRepeated != NULL || pAb->AbPreviousRepeated != NULL)
      while (pAb->AbEnclosing != NULL)
	 pAb = pAb->AbEnclosing;	/* pAb = pave racine */
#endif /* __COLPAGE__ */

   if (pAb != NULL)
     {
	GetDifferedRule (&NpDelR);
	NpDelR->DpPRule = pR;
	NpDelR->DpPSchema = pSP;
	NpDelR->DpAbsBox = pAbb;
	NpDelR->DpNext = NULL;
	NpDelR->DpAttribute = pAttr;
	pDelR = pAb->AbDelayedPRule;
	if (pDelR == NULL)
	   pAb->AbDelayedPRule = NpDelR;
	else
	  {
	     while (pDelR->DpNext != NULL)
		pDelR = pDelR->DpNext;
	     pDelR->DpNext = NpDelR;
	  }
     }
}


/*----------------------------------------------------------------------
   GetDelayedRule recupere une regle de presentation qui etait retardee    
   au niveau du pave pAbb. Au retour pR contient la regle    
   retardee, pSP contient le schema de presentation auquel 
   appartient cette regle, pAbb contient le pave auquel      
   s'applique la regle retardee et pAttr contient          
   l'attribut auquel correspond la regle, si c'est une     
   regle d'attribut (NULL sinon). S'il n'y a pas de regle  
   retardee, pR et pAbb contiennent NULL au retour.          
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                GetDelayedRule (PtrPRule * pR, PtrPSchema * pSP, PtrAbstractBox * pAbb, PtrAttribute * pAttr)

#else  /* __STDC__ */
void                GetDelayedRule (pR, pSP, pAbb, pAttr)
PtrPRule           *pR;
PtrPSchema         *pSP;
PtrAbstractBox     *pAbb;
PtrAttribute       *pAttr;

#endif /* __STDC__ */

{
   PtrDelayedPRule     pDelR;
   PtrAbstractBox      pAb2;
   boolean             stop;
   PtrElement          pEl;

   pAb2 = *pAbb;
   /* a priori, pas de regle retardee */
   *pR = NULL;
   *pSP = NULL;
   *pAbb = NULL;
   *pAttr = NULL;
   if (pAb2 != NULL)
     {
	/* cherche le pave de l'element dans cette vue */
	/* saute les paves de presentation */
	stop = FALSE;
	pEl = pAb2->AbElement;
	do
	   if (pAb2 == NULL)
	      stop = TRUE;
	   else if (pAb2->AbElement != pEl)
	     {
		stop = TRUE;
		pAb2 = NULL;
	     }
	   else if (!pAb2->AbPresentationBox)
	      stop = TRUE;
	   else
	      pAb2 = pAb2->AbNext;
	while (!stop);
	if (pAb2 != NULL)
	  {
	     pDelR = pAb2->AbDelayedPRule;
	     if (pDelR != NULL)
	       {
		  *pR = pDelR->DpPRule;
		  *pSP = pDelR->DpPSchema;
		  *pAbb = pDelR->DpAbsBox;
		  *pAttr = pDelR->DpAttribute;
		  pAb2->AbDelayedPRule = pDelR->DpNext;
		  FreeDifferedRule (pDelR);
	       }
	  }
     }
}

#ifdef __COLPAGE__

/*----------------------------------------------------------------------
   ApplDelayedRule applique les regles retardees conservees pour  
   les paves de l'element El du document pDoc.             
   on remonte les ascendants jusqu'a la racine             
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                ApplDelayedRule (PtrElement pEl, PtrDocument pDoc)

#else  /* __STDC__ */
void                ApplDelayedRule (pEl, pDoc)
PtrElement          pEl;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrPRule            pRule;
   PtrPSchema          pSPres;
   PtrAttribute        pAttr;
   PtrAbstractBox      pAb;
   int                 view, lastView;
   PtrAbstractBox      pAbb;
   boolean             bool;

   if (AssocView (pEl))
      /* vue d'elements associes */
      lastView = 1;
   else
      /* nombre de vues du document */
      lastView = MAX_VIEW_DOC;
   for (view = 0; view < lastView; view++)
     {
	pAb = pEl->ElAbstractBox[view];
	if (pAb != NULL)
	   /* l'element a un pave dans cette vue */
	  {
	     pAb = pEl->ElAbstractBox[view]->AbEnclosing;
	     /* pAb 1er ascendant sur lequel chercher une regle */
	     while (pAb != NULL)
	       {
		  do
		    {
		       /* la procedure ApplyRule modifie pAbb, on le retablit */
		       pAbb = pAb;
		       GetDelayedRule (&pRule, &pSPres, &pAbb, &pAttr);
		       if (pRule != NULL)
			  ApplyRule (pRule, pSPres, pAbb, pDoc, pAttr, &bool);
		    }
		  while (pRule != NULL);
		  pAb = pAb->AbEnclosing;
	       }
	  }
     }
}

#else  /* __COLPAGE__ */

/*----------------------------------------------------------------------
   ApplDelayedRule applique les regles retardees conservees pour  
   les paves de l'element El du document pDoc.             
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                ApplDelayedRule (PtrElement pEl, PtrDocument pDoc)

#else  /* __STDC__ */
void                ApplDelayedRule (pEl, pDoc)
PtrElement          pEl;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrPRule            pRule;
   PtrPSchema          pSPres;
   PtrAttribute        pAttr;
   PtrAbstractBox      pAb, pAbb;
   int                 view, lastView;

   if (pEl == NULL || pDoc == NULL)
      return;
   if (AssocView (pEl))
      /* vue d'elements associes */
      lastView = 1;
   else
      /* nombre de vues du document */
      lastView = MAX_VIEW_DOC;
   for (view = 0; view < lastView; view++)
     {
	pAb = pEl->ElAbstractBox[view];
	if (pAb != NULL)
	   /* l'element a un pave dans cette vue */
	   if (pAb->AbEnclosing != NULL)
	      /* et ce pave a un pave englobant */
	     {
		if (pAb->AbEnclosing->AbPresentationBox &&
		    pAb->AbEnclosing->AbElement == pEl)
		   /* le pave englobant a ete cree' par la regle FnCreateEnclosing */
		   /* les regles retardees sont sur le pave' englobant */
		   pAb = pAb->AbEnclosing->AbEnclosing;
		else
		   pAb = pAb->AbEnclosing;
		do
		  {
		     /* la procedure ApplyRule modifie pAb, on le retablit */
		     pAbb = pAb;
		     GetDelayedRule (&pRule, &pSPres, &pAbb, &pAttr);
		     if (pRule != NULL)
			ApplyRule (pRule, pSPres, pAbb, pDoc, pAttr);
		  }
		while (pRule != NULL);
	     }
     }
}
#endif /* __COLPAGE__ */


/*----------------------------------------------------------------------
   CondPresentation evalue les conditions d'application d'une      
   regle de presentation qui s'applique a` l'element pEl,  
   pour la vue de numero View.                              
   pCond est la premiere condition de la chaine des        
   conditions qui s'appliquent a la regle de presentation. 
   pSS est le schema de structure correspondant au schema  
   de presentation auquel appartient la regle.             
   Retourne vrai si les conditions sont toutes satisfaites.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
boolean             CondPresentation (PtrCondition pCond, PtrElement pEl, PtrAttribute pAttr,
				      int view, PtrSSchema pSS)

#else  /* __STDC__ */
boolean             CondPresentation (pCond, pEl, pAttr, view, pSS)
PtrCondition        pCond;
PtrElement          pEl;
PtrAttribute        pAttr;
int                 view;
PtrSSchema          pSS;

#endif /* __STDC__ */

{
   boolean             ok, currentCond, stop, equal;
   int                 valcompt, valmaxi, valmini, i;
   PtrPSchema          pSchP = NULL;
   PtrElement          pElSibling, pAsc, pElem;
   PtrReference        pRef;
   DocumentIdentifier  IDocExt;
   PtrDocument         pDocExt;
   PtrAttribute        pA;

   /* a priori les conditions sont satisfaites */
   ok = TRUE;
   /* on examine toutes les conditions de la chaine */
   while (pCond != NULL && ok)
     {
	if (!pCond->CoTarget)
	   pElem = pEl;
	else
	   /* la condition porte sur l'element pointe' par pEl ou pAttr.
	      on cherche cet element pointe' */
	  {
	     pElem = NULL;
	     if (pAttr != NULL &&
		 pAttr->AeAttrSSchema->SsAttribute[pAttr->AeAttrNum - 1].AttrType ==
		 AtReferenceAttr)
		/* c'est un attribut reference */
		pRef = pAttr->AeAttrReference;
	     else
		/* l'element est-il une reference ? */
	     if (pEl->ElTerminal && pEl->ElLeafType == LtReference)
		pRef = pEl->ElReference;
	     else
		/* c'est peut-etre une inclusion */
		pRef = pEl->ElSource;
	     if (pRef != NULL)
		pElem = ReferredElement (pRef, &IDocExt, &pDocExt);
	  }
	valcompt = 0;
	if (pElem == NULL)
	   ok = FALSE;
	else if (pCond->CoCondition == PcEven || pCond->CoCondition == PcOdd
	 || pCond->CoCondition == PcOne || pCond->CoCondition == PcInterval)
	   /* evalue le compteur */
	  {
	     pSchP = pSS->SsPSchema;
	     if (pSchP != NULL)
		valcompt = CounterVal (pCond->CoCounter, pSS, pSchP, pElem, view);
	  }
	if (pElem != NULL)
	   switch (pCond->CoCondition)
		 {

		    case PcFirst:
		       /* on saute les marques de page precedentes */
		       pElSibling = pElem->ElPrevious;
		       stop = FALSE;
		       do
			  if (pElSibling == NULL)
			     stop = TRUE;
			  else if (pElSibling->ElTypeNumber == PageBreak + 1)
			     pElSibling = pElSibling->ElPrevious;
			  else
			     stop = TRUE;
		       while (!stop);
		       currentCond = pElSibling == NULL;
		       break;

		    case PcLast:
		       /* on saute les marques de page suivantes */
		       pElSibling = pElem->ElNext;
		       stop = FALSE;
		       do
			  if (pElSibling == NULL)
			     stop = TRUE;
			  else if (pElSibling->ElTypeNumber == PageBreak + 1)
			     pElSibling = pElSibling->ElNext;
			  else
			     stop = TRUE;
		       while (!stop);
		       currentCond = pElSibling == NULL;
		       /* traitement particulier pour les lignes de tableau */
		       if (ThotLocalActions[T_condlast] != NULL)
			  (*ThotLocalActions[T_condlast]) (pElem, &currentCond);
		       break;

		    case PcReferred:
		       /* la condition est satisfaite si l'element (ou le */
		       /* premier de ses ascendants sur lequel peut porter une */
		       /* reference) est reference' au moins une fois. */
		       currentCond = FALSE;
		       pAsc = pElem;
		       do
			 {
			    if (pAsc->ElReferredDescr != NULL)
			       /* l'element est reference' */
			       currentCond = TRUE;
			    if (!currentCond)
			       /* l'element n'est pas reference' */
			       /* on examine l'element ascendant */
			       pAsc = pAsc->ElParent;
			 }
		       while (pAsc != NULL && !currentCond);
		       break;

		    case PcFirstRef:
		    case PcLastRef:
		       /* la condition est satisfaite s'il s'agit de la premiere ou
		          de la derniere reference a l'element reference' */
		       pRef = NULL;
		       if (pAttr != NULL &&
			   pAttr->AeAttrSSchema->SsAttribute[pAttr->AeAttrNum - 1].AttrType == AtReferenceAttr)
			  /* c'est un attribut reference */
			  pRef = pAttr->AeAttrReference;
		       else if (pElem->ElTerminal && pElem->ElLeafType == LtReference)
			  /* l'element est une reference */
			  pRef = pElem->ElReference;
		       if (pRef != NULL)
			  if (pCond->CoCondition == PcFirstRef)
			     currentCond = pEl->ElReference->RdPrevious == NULL;
			  else
			     currentCond = pEl->ElReference->RdNext == NULL;
		       else
			  currentCond = FALSE;
		       break;

		    case PcExternalRef:
		    case PcInternalRef:
		       /* la condition est satisfaite s'il s'agit d'un */
		       /* element ou d'un attribut reference externe (ou interne) */
		       pRef = NULL;
		       currentCond = FALSE;
		       if (pAttr != NULL &&
			   pAttr->AeAttrSSchema->SsAttribute[pAttr->AeAttrNum - 1].AttrType == AtReferenceAttr)
			  /* c'est un attribut reference */
			  pRef = pAttr->AeAttrReference;
		       else
			  /* l'element est-il une reference ? */
		       if (pElem->ElTerminal && pElem->ElLeafType == LtReference)
			  pRef = pElem->ElReference;
		       else
			  /* c'est peut-etre une inclusion */
			  pRef = pElem->ElSource;
		       if (pRef == NULL)
			  currentCond = FALSE;
		       else if (pCond->CoCondition == PcInternalRef)
			  currentCond = pRef->RdInternalRef;
		       else
			  currentCond = !pRef->RdInternalRef;
		       break;

		    case PcCopyRef:
		       /* la condition est satisfaite si l'element est une copie */
		       currentCond = pElem->ElIsCopy;
		       break;

		    case PcAnyAttributes:
		       /* la condition est satisfaite si l'element */
		       /* porte des attributs */
		       currentCond = pElem->ElFirstAttr != NULL;
		       break;

		    case PcFirstAttr:
		       /* TODO */
		       /* la condition est satisfaite si le bloc */
		       /* attribut pAttr est le 1er de l'element */
		       if (pAttr != NULL)
			  currentCond = pAttr == pEl->ElFirstAttr;
		       break;

		    case PcLastAttr:
		       /* TODO */
		       /* la condition est satisfaite si le bloc     */
		       /* attribut pAttr est le dernier de l'element */
		       if (pAttr != NULL)
			  currentCond = pAttr->AeNext == NULL;
		       break;

		    case PcUserPage:
		       /* la condition est satisfaite si l'element
		          est un saut de page utilisateur */
		       if (pElem->ElTypeNumber == PageBreak + 1)
			  currentCond = pElem->ElPageType == PgUser;
		       break;

		    case PcStartPage:
		       /* la condition est satisfaite si l'element
		          est un saut de page de debut */
		       if (pElem->ElTypeNumber == PageBreak + 1)
			  currentCond = pElem->ElPageType == PgBegin;
		       break;

		    case PcComputedPage:
		       /* la condition est satisfaite si l'element
		          est un saut de page calcule */
		       if (pElem->ElTypeNumber == PageBreak + 1)
			  currentCond = pElem->ElPageType == PgComputed;
		       break;

		    case PcEmpty:
		       /* la condition est satisfaite si l'element est vide */
		       if (pElem->ElTerminal)
			  if (pElem->ElLeafType == LtReference)
			     currentCond = pElem->ElReference == NULL;
			  else
			     currentCond = pElem->ElVolume == 0;
		       else
			  currentCond = pElem->ElFirstChild == NULL;
		       break;

		    case PcEven:
		       currentCond = !(valcompt & 1);
		       break;

		    case PcOdd:
		       currentCond = (valcompt & 1);
		       break;

		    case PcOne:
		       currentCond = (valcompt == 1);
		       break;

		    case PcInterval:
		       if (pCond->CoValCounter == CntMinVal)
			 {
			    /* Calcule la valeur mini du compteur */
			    valmini = CounterValMinMax (pCond->CoCounter, pSS, pSchP, pElem,
							view, FALSE);
			    currentCond = (valmini <= pCond->CoMaxCounter) &&
			       (valmini >= pCond->CoMinCounter);
			 }
		       else if (pCond->CoValCounter == CntMaxVal)
			 {
			    /* Calcule la valeur maxi du compteur */
			    valmaxi = CounterValMinMax (pCond->CoCounter, pSS, pSchP, pElem,
							view, TRUE);
			    currentCond = (valmaxi <= pCond->CoMaxCounter) &&
			       (valmaxi >= pCond->CoMinCounter);
			 }
		       else
			  /* Calcule la valeur courante du compteur */
			  currentCond = (valcompt <= pCond->CoMaxCounter) &&
			     (valcompt >= pCond->CoMinCounter);
		       break;
		    case PcWithin:
		       /* condition sur le nombre d'ancetres d'un type donne' */
		       pAsc = pElem->ElParent;
		       if (pAsc == NULL)
			  /* aucun ancetre, condition non satisfaite */
			  currentCond = FALSE;
		       else
			 {
			    i = 0;
			    if (pCond->CoImmediate)
			       /* Condition: If immediately within n element-type */
			       /* Les n premiers ancetres successifs doivent etre du type */
			       /* CoTypeAncestor, sans comporter d'elements d'autres type */
			       /* on compte les ancetres successifs de ce type */
			       while (pAsc != NULL)
				 {
				    if (pCond->CoTypeAncestor != 0)
				       equal = ((pAsc->ElTypeNumber == pCond->CoTypeAncestor) &&
						(pAsc->ElStructSchema->SsCode == pSS->SsCode));
				    else
				       equal = (strcmp (pCond->CoAncestorName, pAsc->ElStructSchema->SsRule[pAsc->ElTypeNumber - 1].SrName) == 0 &&
						strcmp (pCond->CoSSchemaName, pAsc->ElStructSchema->SsName) == 0);
				    if (equal)
				      {
					 i++;
					 pAsc = pAsc->ElParent;
				      }
				    else
				       pAsc = NULL;
				 }
			    else
			       /* Condition: If within n element-type */
			       /* on compte tous les ancetres de ce type */
			       while (pAsc != NULL)
				 {
				    if (pCond->CoTypeAncestor != 0)
				       equal = ((pAsc->ElTypeNumber == pCond->CoTypeAncestor) &&
						(pAsc->ElStructSchema->SsCode == pSS->SsCode));
				    else
				       equal = (strcmp (pCond->CoAncestorName, pAsc->ElStructSchema->SsRule[pAsc->ElTypeNumber - 1].SrName) == 0 &&
						strcmp (pCond->CoSSchemaName, pAsc->ElStructSchema->SsName) == 0);
				    if (equal)
				       i++;
				    pAsc = pAsc->ElParent;	/* passe a l'element ascendant */
				 }
			    if (pCond->CoAncestorRel == CondEquals)
			       currentCond = i == pCond->CoRelation;
			    else if (pCond->CoAncestorRel == CondGreater)
			       currentCond = i > pCond->CoRelation;
			    else if (pCond->CoAncestorRel == CondLess)
			       currentCond = i < pCond->CoRelation;
			 }
		       break;

		    case PcElemType:
		       /* verifie si l'attribut est attache' a un element du type voulu */
		       currentCond = (pElem->ElTypeNumber == pCond->CoTypeElAttr);
		       break;

		    case PcAttribute:
		       /* verifie si l'element possede cet attribut */
		       pA = pElem->ElFirstAttr;
		       currentCond = FALSE;
		       while (pA != NULL && !currentCond)
			  /* boucle sur les attributs de l'element */
			 {
			    if (pA->AeAttrNum == pCond->CoTypeElAttr)
			       currentCond = TRUE;
			    else
			       pA = pA->AeNext;		/* attribut suivant */
			 }
		       break;

		    case PcNoCondition:
		       currentCond = TRUE;
		       break;

		    case PcDefaultCond:
		       currentCond = TRUE;
		       break;
		 }

	if (!pCond->CoNotNegative)
	   currentCond = !currentCond;
	ok = ok && currentCond;
	pCond = pCond->CoNextCondition;
     }

   return ok;
}

#ifndef __COLPAGE__

/*----------------------------------------------------------------------
   NextElRef cherche (en arriere si backward est vrai, en avant     
   sinon) a` partir de l'element pElRef, un element du     
   type TypEl defini dans le schema pSchStr ou une marque  
   de page concernant la vue traitee. Si une marque de     
   page est trouvee, la fonction retourne NULL. Sinon      
   elle retourne un pointeur sur l'element reference' par  
   l'element trouve' et pElRef est l'element trouve'.      
   L'element recherche' (TypEl) doit etre une reference.   
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static PtrElement   NextElRef (PtrElement * pElRef, int TypEl,
			 PtrSSchema pSchStr, boolean backward, int *viewSch)

#else  /* __STDC__ */
static PtrElement   NextElRef (pElRef, TypEl, pSchStr, backward, viewSch)
PtrElement         *pElRef;
int                 TypEl;
PtrSSchema          pSchStr;
boolean             backward;
int                *viewSch;

#endif /* __STDC__ */

{
   PtrElement          pElRet;
   boolean             finish;
   boolean             first;
   PtrReference        pRef;

   pElRet = NULL;
   finish = FALSE;
   do
     {
	if (backward)
	  {
	     /* cherche en arriere */
	     *pElRef = BackSearchElem2Types (*pElRef, PageBreak + 1, TypEl, NULL, pSchStr);
	  }
	else
	  {
	     /* cherche en avant */
	     *pElRef = FwdSearchElem2Types (*pElRef, PageBreak + 1, TypEl, NULL, pSchStr);
	  }
	if (*pElRef == NULL)	/* on n'a rien trouve'. Fin sans succes. */
	   finish = TRUE;
	else if ((*pElRef)->ElTypeNumber == PageBreak + 1
		 && (*pElRef)->ElViewPSchema == *viewSch)
	   /* on a trouve' une marque de page pour la vue traitee. */
	   /* Fin sans succes. */
	   finish = TRUE;
	if (!finish)
	   /* on a trouve une marque de page pour une autre vue que la */
	   /* vue traitee (dans ce cas on continue la recherche), ou on a */
	   /* trouve' le type d'element cherche' */
	   if ((*pElRef)->ElTypeNumber == TypEl)
	      /* c'est le type d'element cherche', qui doit etre une */
	      /* reference. Cherche l'element reference' : pElRet */
	      if ((*pElRef)->ElReference != NULL)
		{
		   first = TRUE;
		   /* cherche les references precedentes au meme element */
		   /* et qui ne sont pas dans des elements associes */
		   pRef = (*pElRef)->ElReference;
		   while (pRef->RdPrevious != NULL && first)
		     {
			pRef = pRef->RdPrevious;
			if (pRef->RdElement != NULL)
			   if (!IsASavedElement (pRef->RdElement))
			      if (pRef->RdElement->ElAssocNum == 0)
				 /* il y a une reference precedente qui */
				 /* n'est pas dans le tampon */
				 /* et qui n'est pas un element associe' */
				 first = FALSE;
		     }
		   /* on ne prend en compte que la premiere reference a */
		   /* l'element qui n'est pas dans un element associe' */
		   if (first)
		      if ((*pElRef)->ElReference->RdReferred != NULL)
			 /* si l'element reference' est dans le tampon */
			 /* couper/coller, on l'ignore */
			{
			   if ((*pElRef)->ElReference->RdReferred->ReExternalRef)
			      pElRet = NULL;
			   else
			      pElRet = (*pElRef)->ElReference->RdReferred->ReReferredElem;
			   if (pElRet != NULL)
			      if (IsASavedElement (pElRet))
				 pElRet = NULL;
			   /* l'element reference' est */
			   /* dans le buffer couper-coller */
			      else
				{
				   finish = TRUE;	/* on a trouve' */
				   if (PageHeaderRefAssoc == NULL && !backward)
				      PageHeaderRefAssoc = *pElRef;
				   /* on se souvient de l'element */
				   /* reference pour le module page */
				}
			}
		}
     }
   while (!finish);

   return pElRet;
}
#endif /* __COLPAGE__ */


#ifdef __COLPAGE__
/*----------------------------------------------------------------------
   initpavpres  procedure appelee par CrAbsBoxesPres pour initialiser      
   le contenu du pave pAb de presentation nouvellement cree    
   avec la regle pRCre                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         initpavpres (PtrAbstractBox pAb, PtrDocument pDoc, PtrPRule pRCre,
			     PtrPSchema pSchP, PtrSSchema pSS, PtrPRule pRD,
				 PtrPRule pRS, PtrAttribute pAttr,
				 boolean completeCreator)
#else  /* __STDC__ */
static void         initpavpres (pAb, pDoc, pRCre, pSchP, pSS, pRD, pRS, pAttr,
				 completeCreator)
PtrAbstractBox      pAb;
PtrDocument         pDoc;
PtrPRule            pRCre;
PtrPSchema          pSchP;
PtrSSchema          pSS;
PtrPRule            pRD;
PtrPRule            pRS;
PtrAttribute        pAttr;
boolean             completeCreator;
#endif /* __STDC__ */
{
   PtrPRule            pR, pRV;
   PtrAbstractBox      pAb1, pAbbNext;
   PtrPSchema          pSP;
   PtrAttribute        pA;
   PtrElement          pEl;
   PresentationBox    *pBox;
   int                 viewNb, view;
   int                 viewSch;
   boolean             ok;
   boolean             bool;

   if (pAb != NULL)
      /* rend non modifiable le pave de presentation */
     {
	pEl = pAb->AbElement;
	viewNb = pAb->AbDocView;
	viewSch = pDoc->DocView[viewNb - 1].DvPSchemaView;
	pAb->AbPresentationBox = TRUE;
	/* c'est un pave de presentation */
	pAb->AbCreatorAttr = pAttr;
	/* on se souvient de l'attribut qui a cree' le pave' */
	pAb->AbTypeNum = pRCre->PrPresBox[0];
	pAb->AbCanBeModified = FALSE;
	pAb->AbPSchema = pSchP;
	pAb->AbAcceptLineBreak =
	   pSchP->PsPresentBox[pAb->AbTypeNum - 1].PbAcceptLineBreak;
	pAb->AbAcceptPageBreak =
	   pSchP->PsPresentBox[pAb->AbTypeNum - 1].PbAcceptPageBreak;
	if (pRCre->PrPresBoxRepeat)
	   pAb->AbRepeatedPresBox = TRUE;
	pAb->AbLeafType = LtCompound;
	pAb->AbVolume = 0;
	pAb->AbInLine = FALSE;
	pAb->AbTruncatedHead = FALSE;
	pAb->AbTruncatedTail = FALSE;
	/* suppression code relatif aux elements associes en haut ou bas */
	/* applique les regles de presentation de la boite creee. */
	do
	  {
	     pR = GetRule (&pRS, &pRD, pEl, NULL, pEl->ElStructSchema);
	     /* pointeur sur la regle a  appliquer pour la view 1 */
	     if (pR != NULL)
		/* cherche et applique les regles de tous les types */
		/* pour la view */
		for (view = 1; view <= MAX_VIEW; view++)
		  {
		     if (view == 1)
			pRV = NULL;
		     else
			pRV = GetRuleView (&pRS, &pRD, pR->PrType, view, pEl, NULL,
					   pEl->ElStructSchema);
		     if (view == viewSch)
			/* applique la regle au pave cree'. */
		       {
			  if (pRV == NULL)
			     pRV = pR;
			  if (pRV->PrType == PtFunction
			      && (pRV->PrPresFunction == FnCreateFirst || pRV->PrPresFunction == FnCreateLast))
			     /* le pave cree' cree un pave de presentation */
			    {
			       /* cas boite bas de page supprime */
			       /* sauve le pointeur de pave de l'element */
			       pAbbNext = pEl->ElAbstractBox[viewNb - 1];
			       /* change le pointeur de pave de l'element, */
			       /* pour un chainage correct du pave a creer */
			       pEl->ElAbstractBox[viewNb - 1] = pAb;
			       pAb->AbPresentationBox = FALSE;
			       /* cree le pave de presentation */
			       pAb1 = CrAbsBoxesPres (pEl, pDoc, pRV, pSS, NULL, viewNb, pSchP, FALSE, TRUE);
			       /* restaure le pointeur de pave de  l'element */
			       pEl->ElAbstractBox[viewNb - 1] = pAbbNext;
			    }
			  else if (!completeCreator && pRV->PrPresMode == PresInherit
				   && pRV->PrInheritMode == InheritCreator)
			     /* toutes les regles de presentation n'ont */
			     /* pas encore ete appliquees au pave */
			     /* et le pave cree herite du createur, on */
			     /* differe l'application de la regle */
			     Delay (pRV, pSchP, pAb, NULL, pAb);
			  else if (!ApplyRule (pRV, pSchP, pAb, pDoc, NULL, &bool))
			     /* on n'a pas pu appliquer la regle, on */
			     /* l'appliquera lorsque le pave pere */
			     /* sera  termine' */
			     Delay (pRV, pSchP, pAb, NULL, pAb);
		       }
		  }
	  }
	while (pR != NULL);
	pAb->AbPresentationBox = TRUE;
	/* met le contenu dans le pave cree */
	pBox = &pSchP->PsPresentBox[pRCre->PrPresBox[0] - 1];
	switch (pBox->PbContent)
	      {
		 case FreeContent:
		    break;
		 case ContVariable:
		    ok = NewVariable (pBox->PbContVariable, pSS, pSchP, pAb, pDoc);
		    break;
		 case ContConst:
		    ConstantCopy (pBox->PbContConstant, pSchP, pAb);
		    break;
		 case ContElement:
		    /* une boite qui regroupe des elements associes */
		    /* rend modifiable le pave de presentation */
		    pAb->AbCanBeModified = TRUE;
		    /* la creation des elements reference's */
		    /* n'est pas traitee ici */
		    break;
	      }
	/* plus de creation stockee en attente dans queuePR */
	/* applique les regles retardees */
	do
	  {
	     pAb1 = pAb;
	     GetDelayedRule (&pR, &pSP, &pAb1, &pA);
	     if (pR != NULL)
		if (!ApplyRule (pR, pSP, pAb1, pDoc, pA, &bool))
		   Delay (pR, pSP, pAb1, pA, pAb);
	  }
	while (pR != NULL);
	/* ajoute le volume du pave cree' a celui de tous ses */
	/* englobants */
	if (pAb->AbVolume > 0)
	  {
	     pAb1 = pAb->AbEnclosing;
	     while (pAb1 != NULL)
	       {
		  pAb1->AbVolume += pAb->AbVolume;
		  pAb1 = pAb1->AbEnclosing;
	       }
	  }
	/* met a jour le volume libre restant dans la view */
	UpdateFreeVol (pAb, pDoc);
     }
}
#endif /* __COLPAGE__ */

#ifdef __COLPAGE__

/*----------------------------------------------------------------------
   CrAbsBoxesPres applique a` la vue viewNb la regle de creation de boite
   de presentation pRCre dans le document pDoc, pour       
   l'element pEl. Cette regle vient du schema de           
   presentation associe' au schema de structure pSS.       
   Retourne le pave cree'. DansBoiteAssoc indique si le    
   createur est une boite de haut ou de bas de page        
   affichant des elements associes.                        
   completeCreator indique si toutes les regles de         
   presentation ont deja ete appliquees au pave createur.  
  ----------------------------------------------------------------------*/

#ifdef __STDC__
PtrAbstractBox      CrAbsBoxesPres (PtrElement pEl, PtrDocument pDoc, PtrPRule pRCre,
		   PtrSSchema pSS, PtrAttribute pAttr, DocViewNumber viewNb,
				    PtrPSchema pSchP, boolean InAssocBox,
				    boolean completeCreator)

#else  /* __STDC__ */
PtrAbstractBox      CrAbsBoxesPres (pEl, pDoc, pRCre, pSS, pAttr, viewNb, pSchP, InAssocBox,
				    completeCreator)
PtrElement          pEl;
PtrDocument         pDoc;
PtrPRule            pRCre;
PtrSSchema          pSS;
PtrAttribute        pAttr;
DocViewNumber       viewNb;
PtrPSchema          pSchP;
boolean             InAssocBox;
boolean             completeCreator;

#endif /* __STDC__ */

{
  int                 view, vis;
  int                 viewSch;
  PtrPRule            pRD, pRS;
  PtrPRule            pR, pR1;
  PtrAbstractBox      pAb, pAbb1;
  boolean             ok, stop;
  TypeUnit            unit;
  PtrAbstractBox      pAbboxEl;
  boolean             isCreated;

  pAb = NULL;
  ok = FALSE;
  if (DoesViewExist (pEl, pDoc, viewNb))	/* la vue existe */
    {
      viewSch = AppliedView (pEl, pAttr, pDoc, viewNb);
      /* faut-il reellement creer ce pave ? */
      if (pRCre->PrCond == NULL)
	/* creation inconditionnelle */
	ok = TRUE;
      else
	/* On verifie les conditions d'application de la regle de creation */
	ok = CondPresentation (pRCre->PrCond, pEl, pAttr, viewSch, pSS);
    }
  /* on ne cree un pave de presentation que si le pave de l'element qui */
  /* provoque la creation existe dans la vue. */
  if (ok)
    if (pEl->ElAbstractBox[viewNb - 1] == NULL)
      ok = FALSE;
  /* on ne cree pas de pave fils pour un element holophraste' */
  if (ok)
    if (pEl->ElHolophrast)
      if (pRCre->PrPresFunction == FnCreateFirst || pRCre->PrPresFunction == FnCreateLast)
	ok = FALSE;
  /* on ne cree pas de pave de presentation fils pour un element terminal */
  if (ok)
    if (pEl->ElTerminal)
      if (pRCre->PrPresFunction == FnCreateFirst || pRCre->PrPresFunction == FnCreateLast)
	ok = FALSE;
  /* on ne cree pas de pave de presentation qui soit un frere ou le pere du */
  /* pave racine de la vue. */
  if (ok)
    if (pEl->ElAbstractBox[viewNb - 1]->AbEnclosing == NULL
	&& (pRCre->PrPresFunction == FnCreateBefore || pRCre->PrPresFunction == FnCreateAfter ||
	    pRCre->PrPresFunction == FnCreateEnclosing))
      ok = FALSE;
  
  /* si c'est une boite de haut ou de bas de page qui regroupe les */
  /* elements associes reference's dans la page, on verifie qu'il */
  /* existe bien de tels elements */
  /* on ne cree jamais le pave correspondant a la racine des */
  /* elements associes, meme s'il y en a dans la page */
  /* car les elements associes seront places au fur et a */
  /* mesure de l'apparition de references dans la page. */
  /* lors de la creation dans la page du premier pave reference, */
  /* on creera le pave pere (pave de l'element racine) */
  /* on cree les paves de presentation de haut et bas de page */
  /* avant, donc sans savoir ce qu'il y a dans la page */
  /* Remarque : pas de traitement differencie entre haut */
  /* et bas de page */
  /* plus rien a faire car la regle content est dans la */
  /* boite de haut ou bas de page */
  /* il n'y a plus de boite de presentation correspondant */
  /* a la racine des elements associes : c'est un pave */
  /* d'element normal : il n'apparait pas ici */
  if (ok)
    /* s'il s'agit d'une vue affichant seulement un sous-arbre, on */
    /* ne cree pas les paves de presentation des elements ascendants */
    /* du sous-arbre en question. */
    if (pEl->ElAssocNum > 0)
      {
	/* element associe' */
	if (pDoc->DocAssocSubTree[pEl->ElAssocNum - 1] != NULL)
	  /* on n'affiche qu'un sous-arbre de ces elements associes */
	  if (ElemIsAnAncestor (pEl, pDoc->DocAssocSubTree[pEl->ElAssocNum - 1]))
	    {
	      /* l'element createur englobe le sous-arbre affichable */
	      ok = FALSE;
	      /* on ne cree pas le pave' de presentation */
	    }
      }
    else
      {
	/* vue de l'arbre principal */
	if (pDoc->DocViewSubTree[viewNb - 1] != NULL)
	  /* on n'affiche qu'un sous-arbre dans cette vue */
	  if (ElemIsAnAncestor (pEl, pDoc->DocViewSubTree[viewNb - 1]))
	    {
	      /* l'element createur englobe le sous-arbre affichable */
	      ok = FALSE;
	      /* on ne cree pas le pave' de presentation */
	    }
      }
  if (ok)
    {
      /* on cree le pave (ou non, selon sa visibilite) */
      /* pRS : premiere regle de presentation */
      /* specifique de la boite a creer */
      pRS = pSchP->PsPresentBox[pRCre->PrPresBox[0] - 1].PbFirstPRule;
      /* pRD : premiere regle de presentation par defaut du schema de */
      /* presentation */
      pRD = pSchP->PsFirstDefaultPRule;
      /* le volume des paves englobant ne comprend pas le volume du pave */
      /* que l'on va creer */
      
      /* parcourt toutes les vues pour trouver toutes les regles de */
      /* visibilite */
      vis = 0;
      for (view = 1; view <= MAX_VIEW; view++)
	{
	  if (view == 1)
	    {
	      /* view principale */
	      pR1 = GetRule (&pRS, &pRD, pEl, NULL, pEl->ElStructSchema);
	      /* regle de visibilite pour la view 1 */
	      pR = pR1;
	    }
	  else
	    {
	      /* s'il y a une regle de visibilite pour cette vue, on la  *prend */
	      pR = GetRuleView (&pRS, &pRD, PtVisibility, view, pEl, NULL,
				pEl->ElStructSchema);
	      if (pR == NULL)
		/* sinon, on prend celle de la vue 1 */
		pR = pR1;
	    }
	  if (view == viewSch)
	    {
	      /* c'est la regle de la vue traitee */
	      vis = IntegerRule (pR, pEl, viewNb, &ok, &unit, NULL);
	      /* si la regle de visibilite n'a pas pu etre appliquee, */
	      /* on prend la visibilite du pave de l'element createur */
	      if (!ok)
		vis = pEl->ElAbstractBox[viewNb - 1]->AbVisibility;
	    }
	}
      
      if (vis >= 1)
	/* le pave a creer est visible dans la vue */
	{
	  pAbboxEl = pEl->ElAbstractBox[viewNb - 1];
	  /* pAbboxEl:1er pave de l'element createur */
	  switch (pRCre->PrPresFunction)
	    {
	    case FnCreateFirst:
	      pAbb1 = pAbboxEl;
	      /* cas des paves de presentation de la racine, */
	      /* il faut les placer */
	      /* sous le pave corps de page s'il existe */
	      if (pAbb1->AbEnclosing == NULL && pAbb1->AbFirstEnclosed != NULL
		  && pAbb1->AbFirstEnclosed->AbElement->ElTypeNumber ==
		  PageBreak + 1)
		{
		  pAbb1 = pAbb1->AbFirstEnclosed;
		  /* recherche du premier pave corps de page (on saute le */
		  /* haut de page s'il existe */
		  while (pAbb1 != NULL && pAbb1->AbPresentationBox)
		    pAbb1 = pAbb1->AbNext;
		  /* on descend d'un niveau pour les colonnes */
		  while (pAbb1 != NULL &&
			 pAbb1->AbFirstEnclosed != NULL &&
			 pAbb1->AbFirstEnclosed->AbElement->ElTypeNumber
			 == PageBreak + 1)
		    {
		      pAbb1 = pAbb1->AbFirstEnclosed;
		      /* on saute les eventuels paves de presentation */
		      while (pAbb1 != NULL && pAbb1->AbPresentationBox)
			pAbb1 = pAbb1->AbNext;
		    }
		  if (pAbb1 == NULL)
		    {
		      printf ("erreur i.a. creation pav pres racine  \n");
		      AffPaveDebug (pEl->ElAbstractBox[viewNb - 1]);
		    }
		}
	      else
		/* saute les paves de presentation deja crees */
		/* avec la regle CreateBefore */
		while (pAbb1->AbPresentationBox)
		  pAbb1 = pAbb1->AbNext;
	      /* pAbb1 : pave de la chaine des paves dupliques */
	      pAbboxEl = pAbb1;	/* pour boucle sur les paves dupliques */
	      /* boucle sur les paves dupliques de l'element si la regle */
	      /* a l'option REP */
	      do
		{
		  isCreated = FALSE;
		  pAbb1 = pAbboxEl;
		  if (pAbb1->AbFirstEnclosed != NULL)
		    /* il y a deja des paves englobes */
		    {
		      pAbb1 = pAbb1->AbFirstEnclosed;
		      /* saute les paves deja crees par une regle */
		      /* FnCreateFirst et les paves des marques de page */
		      /* de debut d'element */
		      /* et verifie si le pave de presentation existe deja */
		      stop = FALSE;
		      do
			if (pAbb1->AbPresentationBox && pAbb1->AbElement == pEl)
			  /* suppression de la condition sur le pave marque de page */
			  /* c'est un pave de presentation de l'element */
			  if (pAbb1->AbTypeNum == pRCre->PrPresBox[0]
			      && pAbb1->AbPresentationBox
			      && pAbb1->AbPSchema == pSchP
			      && (!pAbb1->AbDead))
			    /* c'est le meme que celui qu'on veut creer */
			    {
			      isCreated = TRUE;
			      stop = TRUE;
			    }
			  else if (pAbb1->AbNext == NULL)
			    stop = TRUE;
			  else
			    pAbb1 = pAbb1->AbNext;
			else
			  /* ce n'est ni pave de presentation de l'element */
			  /* ni un saut de page de debut d'element */
			  stop = TRUE;
		      while (!stop);
		    }
		  if (!isCreated)
		    {
		      /* on cree le pave */
		      pAb = InitAbsBoxes (pEl, viewNb, vis);
		      pAb->AbEnclosing = pAbboxEl;
		      pAb->AbReadOnly = pAbboxEl->AbReadOnly;
		      if (pAb->AbEnclosing->AbFirstEnclosed == NULL)
			/* c'est le premier pave englobe' */
			pAbboxEl->AbFirstEnclosed = pAb;
		      else
			/* pAbb1 est le pave a cote de qui il faut chainer pAb */
			if (pAbb1->AbElement == pEl)
			  /* chaine le nouveau pave apres le dernier pave */
			  /* de presentation cree par une regle FnCreateFirst */
			  /* ou la derniere marque de page de debut d'elem. */
			  {
			    pAb->AbPrevious = pAbb1;
			    pAb->AbNext = pAbb1->AbNext;
			    pAb->AbPrevious->AbNext = pAb;
			    if (pAb->AbNext != NULL)
			      pAb->AbNext->AbPrevious = pAb;
			  }
			else
			  /* chaine le nouveau pave avant le premier pave */
			  /* du premier fils du createur */
			  /* ce traitement est correct meme pout les paves */
			  /* de presentation fils de la racine car ils sont */
			  /* toujours crees apres les paves du */
			  /* premier element sous le premier corps de page */
			  {
			    pAb->AbNext = pAbb1;
			    pAb->AbPrevious = pAbb1->AbPrevious;
			    pAb->AbNext->AbPrevious = pAb;
			    if (pAb->AbPrevious != NULL)
			      pAb->AbPrevious->AbNext = pAb;
			    if (pAb->AbEnclosing->AbFirstEnclosed == pAbb1)
			      pAb->AbEnclosing->AbFirstEnclosed = pAb;
			  }
		      initpavpres (pAb, pDoc, pRCre, pSchP, pSS, pRD, pRS,
				   pAttr, completeCreator);
		    }
		  if (pRCre->PrPresBoxRepeat)
		    pAbboxEl = pAbboxEl->AbNextRepeated;
		  else
		    pAbboxEl = NULL;
		}
	      while (pAbboxEl != NULL);	/* fin boucle de parcours des dup */
	      break;
	    case FnCreateLast:
	      pAbb1 = pAbboxEl;
	      /* cas des paves de presentation de la racine, il faut les placer */
	      /* sous le dernier pave corps de page s'il en existe */
	      if (pAbb1->AbEnclosing == NULL && pAbb1->AbFirstEnclosed != NULL
		  && pAbb1->AbFirstEnclosed->AbElement->ElTypeNumber ==
		  PageBreak + 1)
		{
		  pAbb1 = pAbb1->AbFirstEnclosed;
		  /* recherche du dernier pave corps de page  */
		  while (pAbb1->AbNext != NULL)
		    pAbb1 = pAbb1->AbNext;
		  /* on saute les paves bas et filet de page s'ils existent */
		  while (pAbb1 != NULL
			 && (pAbb1->AbPresentationBox || pAbb1->AbLeafType != LtCompound))
		    pAbb1 = pAbb1->AbPrevious;
		  
		  /* on descend la hierarchie des colonnes */
		  while (pAbb1->AbFirstEnclosed != NULL &&
			 pAbb1->AbFirstEnclosed->AbElement->ElTypeNumber == PageBreak + 1)
		    {
		      pAbb1 = pAbb1->AbFirstEnclosed;
		      while (pAbb1->AbNext != NULL)
			pAbb1 = pAbb1->AbNext;
		      while (pAbb1->AbPresentationBox)
			pAbb1 = pAbb1->AbPrevious;
		    }
		}
	      else
		{
		  /* saute les paves de presentation deja crees */
		  /* avec la regle CreateBefore */
		  while (pAbb1->AbPresentationBox)
		    pAbb1 = pAbb1->AbNext;
		  /* on se positionne sur le dernier pave duplique de l'element */
		  while (pAbb1->AbNextRepeated != NULL)
		    pAbb1 = pAbb1->AbNextRepeated;
		}
	      pAbboxEl = pAbb1;	/* pour parcours des dup */
	      do
		{	/* parcours des dup si le booleen pRCre->PrPresBoxRepeat est vrai */
		  isCreated = FALSE;	/* a priori ce pave n'est pas deja cree */
		  pAbb1 = pAbboxEl;
		  /* on verifie que le pave de presentation n'existe pas deja */
		  if (pAbboxEl->AbFirstEnclosed != NULL)
		    {
		      pAbb1 = pAbboxEl->AbFirstEnclosed;
		      /* cherche le dernier fils du pave createur */
		      /* et verifie si le pave de presentation existe deja */
		      stop = FALSE;
		      do
			if (pAbb1->AbTypeNum == pRCre->PrPresBox[0]
			    && pAbb1->AbPresentationBox
			    && pAbb1->AbPSchema == pSchP
			    && (!pAbb1->AbDead))
			  /* ce pave de presentation existe deja */
			  {
			    isCreated = TRUE;
			    stop = TRUE;
			  }
			else if (pAbb1->AbNext == NULL)
			  stop = TRUE;
			else
			  pAbb1 = pAbb1->AbNext;
		      while (!stop);
		    }
		  /* chaine le nouveau pave apres le dernier pave */
		  /* fils du pave createur */
		  /* ce traitement reste correct meme dans le cas des paves */
		  /* de presentation fils de la racine car ils seront */
		  /* toujours crees apres les paves de presentation du */
		  /* dernier element sous le dernier corps de page */
		  if (!isCreated)
		    {
		      /* on cree le pave */
		      pAb = InitAbsBoxes (pEl, viewNb, vis);
		      pAb->AbEnclosing = pAbboxEl;
		      pAb->AbReadOnly = pAbboxEl->AbReadOnly;
		      if (pAb->AbEnclosing->AbFirstEnclosed == NULL)
			/* c'est le premier pave englobe' */
			pAbboxEl->AbFirstEnclosed = pAb;
		      else
			/* pAbb1 est le pave a cote de qui il faut chainer pAb */
			{
			  pAb->AbPrevious = pAbb1;
			  pAb->AbNext = pAbb1->AbNext;
			  pAbb1->AbNext = pAb;
			}
		      initpavpres (pAb, pDoc, pRCre, pSchP, pSS, pRD, pRS,
				   pAttr, completeCreator);
		    }
		  if (pRCre->PrPresBoxRepeat)
		    pAbboxEl = pAbboxEl->AbPreviousRepeated;
		  else
		    pAbboxEl = NULL;
		}
	      while (pAbboxEl != NULL);	/* fin boucle de parcours des dup */
	      break;
	    case FnCreateBefore:
	      do
		{	/* boucle des dup si pRCre->PrPresBoxRepeat est vrai */
		  isCreated = FALSE;	/* a priori ce pave n'est pas deja cree */
		  pAbb1 = pAbboxEl;
		  /* saute les paves de presentation deja crees */
		  /* avec la regle CreateBefore */
		  while (pAbb1->AbPresentationBox)
		    {
		      if (pAbb1->AbTypeNum == pRCre->PrPresBox[0]
			  && pAbb1->AbPresentationBox
			  && pAbb1->AbPSchema == pSchP
			  && (!pAbb1->AbDead))
			/* ce pave de presentation existe deja */
			isCreated = TRUE;
		      pAbb1 = pAbb1->AbNext;
		    }
		  pAbboxEl = pAbb1;
		  if (!isCreated)
		    {
		      /* on cree le pave */
		      pAb = InitAbsBoxes (pEl, viewNb, vis);
		      pAb->AbReadOnly = pAbb1->AbReadOnly;
		      pAb->AbEnclosing = pAbb1->AbEnclosing;
		      if (pAb->AbEnclosing->AbFirstEnclosed == pAbb1)
			pAb->AbEnclosing->AbFirstEnclosed = pAb;
		      pAb->AbNext = pAbb1;
		      pAb->AbPrevious = pAbb1->AbPrevious;
		      pAbb1->AbPrevious = pAb;
		      if (pAb->AbPrevious != NULL)
			pAb->AbPrevious->AbNext = pAb;
		      if (pAbb1->AbPreviousRepeated == NULL)
			if (pAb->AbPrevious == NULL)
			  pEl->ElAbstractBox[viewNb - 1] = pAb;
			else if (pAb->AbPrevious->AbElement != pEl)
			  pEl->ElAbstractBox[viewNb - 1] = pAb;
		      initpavpres (pAb, pDoc, pRCre, pSchP, pSS, pRD, pRS,
				   pAttr, completeCreator);
		    }
		  if (pRCre->PrPresBoxRepeat)
		    {
		      pAbboxEl = pAbboxEl->AbNextRepeated;
		      if (pAbboxEl != NULL)
			while (pAbboxEl->AbPrevious != NULL
			       && pAbboxEl->AbPrevious->AbElement == pEl)
			  pAbboxEl = pAbboxEl->AbPrevious;
		      /* on se positionne sur le premier pave de pEl */
		      /* pour verifier si le pave de presentation */
		      /* n'est pas deja cree */
		    }
		  else
		    pAbboxEl = NULL;
		}
	      while (pAbboxEl != NULL);	/* fin boucle de parcours des dup */
	      break;
	    case FnCreateAfter:
	      pAbb1 = pAbboxEl;
	      /* on se positionne sur le dernier pave duplique de l'element */
	      while (pAbb1->AbPresentationBox)	/* on saute les paves de pres avant */
		pAbb1 = pAbb1->AbNext;
	      while (pAbb1->AbNextRepeated != NULL)
		pAbb1 = pAbb1->AbNextRepeated;
	      pAbboxEl = pAbb1;	/* pour parcours des dup */
	      do
		{	/* boucle des dup si pRCre->PrPresBoxRepeat est vrai */
		  isCreated = FALSE;	/* a priori ce pave n'est pas deja cree */
		  pAbb1 = pAbboxEl;
		  stop = FALSE;
		  do
		    if (pAbb1->AbNext == NULL)
		      stop = TRUE;
		    else if (pAbb1->AbNext->AbElement != pEl)
		      stop = TRUE;
		    else
		      {
			if (pAbb1->AbNext->AbPresentationBox)
			  if (pAbb1->AbNext->AbTypeNum == pRCre->PrPresBox[0]
			      && pAbb1->AbNext->AbPresentationBox
			      && pAbb1->AbNext->AbPSchema == pSchP
			      && (!pAbb1->AbNext->AbDead))
			    /* ce pave de presentation existe deja */
			    isCreated = TRUE;
			pAbb1 = pAbb1->AbNext;
		      }
		  while (!stop);
		  if (!isCreated)
		    {
		      /* on cree le pave */
		      pAb = InitAbsBoxes (pEl, viewNb, vis);
		      pAb->AbReadOnly = pAbb1->AbReadOnly;
		      pAb->AbEnclosing = pAbb1->AbEnclosing;
		      pAb->AbNext = pAbb1->AbNext;
		      pAb->AbPrevious = pAbb1;
		      pAbb1->AbNext = pAb;
		      if (pAb->AbNext != NULL)
			pAb->AbNext->AbPrevious = pAb;
		      initpavpres (pAb, pDoc, pRCre, pSchP, pSS, pRD, pRS,
				   pAttr, completeCreator);
		    }
		  if (pRCre->PrPresBoxRepeat)
		    pAbboxEl = pAbboxEl->AbPreviousRepeated;
		  else
		    pAbboxEl = NULL;
		}
	      while (pAbboxEl != NULL);	/* fin boucle de parcours des dup */
	      break;
	    default:;
	      break;
	    }
	}
    }
  return pAb;
}

#else  /* __COLPAGE__ */

/*----------------------------------------------------------------------
  CrAbsBoxesPres applique a` la vue viewNb la regle de creation de boite
  de presentation pRCre dans le document pDoc, pour l'element pEl.
  Cette regle vient du schema de presentation associe au schema de structure
  pSS.
  Retourne le pave cree'. InAssocBox indique si le createur est une boite de
  haut ou de bas de page affichant des elements associes.
  completeCreator indique si toutes les regles de presentation ont deja ete
  appliquees au pave createur.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PtrAbstractBox      CrAbsBoxesPres (PtrElement pEl, PtrDocument pDoc, PtrPRule pRCre,
		   PtrSSchema pSS, PtrAttribute pAttr, DocViewNumber viewNb,
				    PtrPSchema pSchP, boolean InAssocBox,
				    boolean completeCreator)
#else  /* __STDC__ */
PtrAbstractBox      CrAbsBoxesPres (pEl, pDoc, pRCre, pSS, pAttr, viewNb, pSchP, InAssocBox,
				    completeCreator)
PtrElement          pEl;
PtrDocument         pDoc;
PtrPRule            pRCre;
PtrSSchema          pSS;
PtrAttribute        pAttr;
DocViewNumber       viewNb;
PtrPSchema          pSchP;
boolean             InAssocBox;
boolean             completeCreator;
#endif /* __STDC__ */
{
  int                 view, vis;
  int                 viewSch, viewIndex;
  int                 volume;
  PtrPRule            pRD, pRS;
  PtrPRule            pR, pR1, pRV;
  PtrAbstractBox      pAb, pAbb1, pAbbNext;
  PtrAbstractBox      pAbbCreated;
  PtrElement          pE, pER, pElSibling;
  boolean             ok, stop, finish, volok;
  PtrPRule            queuePR[MAX_QUEUE_LEN];
  int                 lqueue, pqueue;
  PtrPSchema          pSP;
  PtrAttribute        pA;
  PresentationBox    *pBox;
  boolean             complete;
  TypeUnit            unit;

  pAbbCreated = NULL;
  pAb = NULL;
  ok = FALSE;
  pER = NULL;
  viewIndex = viewNb - 1;
  if (DoesViewExist (pEl, pDoc, viewNb))	/* la vue existe */
    {
      viewSch = AppliedView (pEl, pAttr, pDoc, viewNb);
      /* faut-il reellement creer ce pave ? */
      if (pRCre->PrCond == NULL)
	ok = TRUE;
      else
	/* On verifie les conditions d'application de la regle de creation */
	ok = CondPresentation (pRCre->PrCond, pEl, pAttr, viewSch, pSS);
    }
  /* on ne cree un pave de presentation que si le pave de l'element qui */
  /* provoque la creation existe dans la vue. */
  if (ok)
    if (pEl->ElAbstractBox[viewIndex] == NULL)
      ok = FALSE;
  /* on ne cree pas de pave fils pour un element holophraste' */
  if (ok)
    if (pEl->ElHolophrast)
      if (pRCre->PrPresFunction == FnCreateFirst || pRCre->PrPresFunction == FnCreateLast)
	ok = FALSE;
  /* on ne cree pas de pave de presentation qui soit un frere ou le pere du */
  /* pave racine de la vue. */
  if (ok)
    if (pEl->ElAbstractBox[viewIndex]->AbEnclosing == NULL
	&& (pRCre->PrPresFunction == FnCreateBefore || pRCre->PrPresFunction == FnCreateAfter ||
	    pRCre->PrPresFunction == FnCreateWith || pRCre->PrPresFunction == FnCreateEnclosing))
      ok = FALSE;
  /* si c'est une boite de haut de page et qu'il s'agit de la derniere */
  /* marque de page du document, on ne cree pas la boite */
  if (ok)
    if (pSchP->PsPresentBox[pRCre->PrPresBox[0] - 1].PbPageHeader)
      /* c'est une boite de haut de page */
      {
	pE = pEl;
	do
	  /* cherche le 1er frere suivant de pE qui ne */
	  /* soit pas un saut de page */
	  {
	    pElSibling = pE->ElNext;
	    stop = FALSE;
	    do
	      if (pElSibling == NULL)
		{
		  stop = TRUE;
		  /* c'etait le dernier frere */
		}
	      else
		{
		  /* il y a un frere suivant */
		  if (pElSibling->ElTerminal && pElSibling->ElTypeNumber == PageBreak + 1)
		    /* c'est une marque de page, on passe au suivant */
		    pElSibling = pElSibling->ElNext;
		  else
		    /* ce n'est pas une marque de page, on a trouve' */
		    stop = TRUE;
		}
	    while (!stop);

	    if (pElSibling == NULL)
	      /* on n'a pas trouve de suivant qui ne soit */
	      /* pas une marque de page */
	      {
		pE = pE->ElParent;
		/* cherche au niveau superieur */
		if (pE == NULL)
		  /* on est a la racine, c'est donc la */
		  /* derniere marque de page du document */
		  ok = FALSE;
		/* on ne cree pas la boite */
	      }
	    else
	      /* il y a un suivant, on arrete */
	      pE = NULL;
	  }
	while (pE != NULL);
      }
  /* si c'est une boite de haut ou de bas de page qui regroupe les */
  /* elements associes reference's dans la page, on verifie qu'il */
  /* existe bien de tels elements */
  if (ok)
    {
      pBox = &pSchP->PsPresentBox[pRCre->PrPresBox[0] - 1];
      if (pBox->PbContent == ContElement)
	/* il faut creer une boite qui regroupe des elements associes */
	/* on ne fait rien si c'est une boite de haut de page et que */
	/* son contenu ne doit pas etre cree */
	if (pBox->PbPageHeader && !pEl->ElAssocHeader)
	  {
	    ok = FALSE;
	    /* on ne cree pas la boite */
	    /* indique qu'il faudra creer cette boite plus tard. */
	    /* Ce sera fait par le module page. */
	    NbBoxesPageHeaderToCreate = 1;
	    WorkingPage = pEl;
	    PageCreateRule = pRCre;
	    PageSchPresRule = pSchP;
	    pEl->ElAssocHeader = TRUE;
	  }
	else
	  {
	    /* cherche s'il y a dans la page une reference a un element
	       associe de ce type */
	    pE = pEl;
	    stop = FALSE;
	    do
	      {
		pER = NextElRef (&pE, pBox->PbContRefElem, pEl->ElStructSchema,
				 pBox->PbPageFooter, &viewSch);
		if (pER == NULL)
		  /* pas de reference dans la page */
		  {
		    stop = TRUE;
		    ok = FALSE;
		    /* on ne cree pas la boite de haut ou bas de page */
		  }
		else
		  /* on a trouve' dans la page une reference */
		  if (IsASavedElement (pER))
		    /* l'element reference' est dans le buffer des elements */
		    /* coupe's, il n'apparait donc pas dans la boite a creer */
		    /* et on cherche un autre element reference' dans la page */
		    pER = NULL;
		  else
		    {
		      /* l'element reference' existe bien, il faut creer la */
		      /* boite de bas ou de haut de page */
		      stop = TRUE;
		    }
	      }
	    while (!stop);
	  }
    }

  if (ok)
    /* s'il s'agit d'une vue affichant seulement un sous-arbre, on */
    /* ne cree pas les paves de presentation des elements ascendants */
    /* du sous-arbre en question. */
    if (pEl->ElAssocNum > 0)
      {
	/* element associe' */
	if (pDoc->DocAssocSubTree[pEl->ElAssocNum - 1] != NULL)
	  /* on n'affiche qu'un sous-arbre de ces elements associes */
	  if (ElemIsAnAncestor (pEl, pDoc->DocAssocSubTree[pEl->ElAssocNum - 1]))
	    /* l'element createur englobe le sous-arbre affichable */
	    /* on ne cree pas le pave' de presentation */
	    ok = FALSE;
      }
    else
      {
	/* vue de l'arbre principal */
	if (pDoc->DocViewSubTree[viewIndex] != NULL)
	  /* on n'affiche qu'un sous-arbre dans cette vue */
	  if (ElemIsAnAncestor (pEl, pDoc->DocViewSubTree[viewIndex]))
	    /* l'element createur englobe le sous-arbre affichable */
	    /* on ne cree pas le pave' de presentation */
	    ok = FALSE;
      }

   if (ok)
     {
       /* on cree le pave (ou non, selon sa visibilite) */
       /* pRS : premiere regle de presentation */
       /* specifique de la boite a creer */
       pRS = pSchP->PsPresentBox[pRCre->PrPresBox[0] - 1].PbFirstPRule;
       /* pRD : premiere regle de presentation par defaut du schema de */
       /* presentation */
       pRD = pSchP->PsFirstDefaultPRule;
       /* le volume des paves englobant ne comprend pas le volume du pave */
       /* que l'on va creer */
       volok = FALSE;
       /* parcourt toutes les vues pour trouver toutes les regles de */
       /* visibilite */
       vis = 0;
       pR1 = NULL;
       for (view = 1; view <= MAX_VIEW; view++)
	 {
	   if (view == 1)
	     {
	       /* vue principale */
	       pR1 = GetRule (&pRS, &pRD, pEl, pAttr, pEl->ElStructSchema);
	       /* regle de visibilite pour la vue 1 */
	       pR = pR1;
	     }
	   else
	     {
	       /* s'il y a une regle de visibilite pour cette vue, on la prend */
	       pR = GetRuleView (&pRS, &pRD, PtVisibility, view, pEl, pAttr,
				 pEl->ElStructSchema);
	       if (pR == NULL)
		 /* sinon, on prend celle de la vue 1 */
		 pR = pR1;
	     }
	   if (view == viewSch)
	     {
	       /* c'est la regle de la vue traitee */
	       vis = IntegerRule (pR, pEl, viewNb, &ok, &unit, NULL);
	       /* si la regle de visibilite n'a pas pu etre appliquee, */
	       /* on prend la visibilite du pave de l'element createur */
	       if (!ok)
		 vis = pEl->ElAbstractBox[viewIndex]->AbVisibility;
	     }
	 }

       if (vis >= 1)
	 /* le pave a creer est visible dans la vue */
	 {
	   lqueue = 0;
	   pqueue = 0;
	   pAb = InitAbsBoxes (pEl, viewNb, vis);
	   /* pAb: pave cree */
	   pAbbCreated = pAb;
	   pAb->AbPresentationBox = TRUE;
	   /* c'est un pave de presentation */
	   pAb->AbCreatorAttr = pAttr;
	   /* on se souvient de l'attribut qui a cree' le pave' */
	   pAb->AbTypeNum = pRCre->PrPresBox[0];
	   pAb->AbCanBeModified = FALSE;
	   pAb->AbPSchema = pSchP;
	   pAb->AbAcceptLineBreak = pSchP->PsPresentBox[pAb->AbTypeNum - 1].PbAcceptLineBreak;
	   pAb->AbAcceptPageBreak = pSchP->PsPresentBox[pAb->AbTypeNum - 1].PbAcceptPageBreak;
	   pAbb1 = pEl->ElAbstractBox[viewIndex];
	   /* pAbb1: 1er pave de l'element createur */
	   /* chaine le pave cree */
	   switch (pRCre->PrPresFunction)
	     {
	     case FnCreateFirst:
	       pAb->AbSelected = FALSE;
	       /* saute les paves de presentation deja crees */
	       /* avec la regle CreateBefore */
	       while (pAbb1->AbPresentationBox)
		 pAbb1 = pAbb1->AbNext;
	       pAb->AbEnclosing = pAbb1;
	       pAb->AbReadOnly = pAbb1->AbReadOnly;
	       if (pAb->AbEnclosing->AbFirstEnclosed == NULL)
		 /* c'est le premier pave englobe' */
		 pAb->AbEnclosing->AbFirstEnclosed = pAb;
	       else
		 /* il y a deja des paves englobes */
		 {
		   pAbb1 = pAb->AbEnclosing->AbFirstEnclosed;
		   /* saute les paves deja crees par une regle */
		   /* FnCreateFirst et les paves des marques de page */
		   /* de debut d'element */
		   /* et verifie si le pave de presentation existe deja */
		   stop = FALSE;
		   do
		     if ((pAbb1->AbPresentationBox && pAbb1->AbElement == pEl)
			 || (pAbb1->AbElement->ElTypeNumber == PageBreak + 1
			     && pAbb1->AbElement->ElPageType == PgBegin))
		       /* c'est un pave de presentation de l'element ou */
		       /* un saut de page de debut d'element */
		       if (pAbb1->AbTypeNum == pAb->AbTypeNum
			   && pAbb1->AbPresentationBox
			   && pAbb1->AbPSchema == pSchP
			   && (!pAbb1->AbDead))
			 /* c'est le meme que celui qu'on veut creer */
			 {
			   pAbbCreated = NULL;
			   stop = TRUE;
			 }
		       else if (pAbb1->AbNext == NULL)
			 stop = TRUE;
		       else
			 pAbb1 = pAbb1->AbNext;
		     else
		       /* ce n'est ni pave de presentation de l'element */
		       /* ni un saut de page de debut d'element */
		       stop = TRUE;
		   while (!stop);
		   
		   if (pAbbCreated != NULL)
		     if (pAbb1->AbElement == pEl
			 || (pAbb1->AbElement->ElTypeNumber == PageBreak + 1
			     && pAbb1->AbElement->ElPageType == PgBegin))
		       /* chaine le nouveau pave apres le dernier pave */
		       /* de presentation cree par une regle FnCreateFirst */
		       /* ou la derniere marque de page de debut d'elem. */
		       {
			 pAb->AbPrevious = pAbb1;
			 pAb->AbNext = pAbb1->AbNext;
			 pAb->AbPrevious->AbNext = pAb;
			 if (pAb->AbNext != NULL)
			   pAb->AbNext->AbPrevious = pAb;
		       }
		     else
		       /* chaine le nouveau pave avant le premier pave */
		       /* du premier fils du createur */
		       {
			 pAb->AbNext = pAbb1;
			 pAb->AbPrevious = pAbb1->AbPrevious;
			 pAb->AbNext->AbPrevious = pAb;
			 if (pAb->AbPrevious != NULL)
			   pAb->AbPrevious->AbNext = pAb;
			 if (pAb->AbEnclosing->AbFirstEnclosed == pAbb1)
			   pAb->AbEnclosing->AbFirstEnclosed = pAb;
		       }
		 }
	       break;
	     case FnCreateLast:
	       pAb->AbSelected = FALSE;
	       /* saute les paves de presentation deja crees */
	       /* avec la regle CreateBefore */
	       while (pAbb1->AbPresentationBox)
		 pAbb1 = pAbb1->AbNext;
	       pAb->AbEnclosing = pAbb1;
	       pAb->AbReadOnly = pAbb1->AbReadOnly;
	       if (pAb->AbEnclosing->AbFirstEnclosed == NULL)
		 pAb->AbEnclosing->AbFirstEnclosed = pAb;
	       else
		 {
		   pAbb1 = pAb->AbEnclosing->AbFirstEnclosed;
		   /* cherche le dernier fils du pave createur */
		   /* et verifie si le pave de presentation existe deja */
		   stop = FALSE;
		   do
		     if (pAbb1->AbTypeNum == pAb->AbTypeNum
			 && pAbb1->AbPresentationBox == pAb->AbPresentationBox
			 && pAbb1->AbPSchema == pSchP
			 && (!pAbb1->AbDead))
		       /* ce pave de presentation existe deja */
		       {
			 pAbbCreated = NULL;
			 stop = TRUE;
		       }
		     else if (pAbb1->AbNext == NULL)
		       stop = TRUE;
		     else
		       pAbb1 = pAbb1->AbNext;
		   while (!stop);
		   /* chaine le nouveau pave apres le dernier pave */
		   /* fils du pave createur */
		   if (pAbbCreated != NULL)
		     {
		       pAb->AbPrevious = pAbb1;
		       pAb->AbNext = pAbb1->AbNext;
		       pAb->AbPrevious->AbNext = pAb;
		     }
		 }
	       break;
	     case FnCreateBefore:
	       /* saute les paves de presentation deja crees */
	       /* avec la regle CreateBefore */
	       while (pAbb1->AbPresentationBox)
		 {
		   if ((pAbb1->AbTypeNum == pAb->AbTypeNum)
		       && (pAbb1->AbPresentationBox == pAb->AbPresentationBox)
		       && (pAbb1->AbPSchema == pSchP)
		       && (!pAbb1->AbDead))
		     /* ce pave de presentation existe deja */
		     pAbbCreated = NULL;
		   pAbb1 = pAbb1->AbNext;
		 }
	       if (pAbbCreated != NULL)
		 {
		   pAb->AbReadOnly = pAbb1->AbReadOnly;
		   pAb->AbEnclosing = pAbb1->AbEnclosing;
		   if (pAb->AbEnclosing->AbFirstEnclosed == pAbb1)
		     pAb->AbEnclosing->AbFirstEnclosed = pAb;
		   pAb->AbNext = pAbb1;
		   pAb->AbPrevious = pAbb1->AbPrevious;
		   pAbb1->AbPrevious = pAb;
		   if (pAb->AbPrevious != NULL)
		     pAb->AbPrevious->AbNext = pAb;
		   if (pAb->AbPrevious == NULL)
		     pEl->ElAbstractBox[viewIndex] = pAb;
		   else if (pAb->AbPrevious->AbElement != pEl)
		     pEl->ElAbstractBox[viewIndex] = pAb;
		 }
	       break;
	     case FnCreateAfter:
	     case FnCreateWith:
	       stop = FALSE;
	       do
		 if (pAbb1->AbNext == NULL)
		   stop = TRUE;
		 else if (pAbb1->AbNext->AbElement != pEl)
		   stop = TRUE;
		 else
		   {
		     if (pAbb1->AbNext->AbPresentationBox)
		       if (pAbb1->AbNext->AbTypeNum == pAb->AbTypeNum
			   && pAbb1->AbNext->AbPresentationBox == pAb->AbPresentationBox
			   && pAbb1->AbNext->AbPSchema == pSchP
			   && (!pAbb1->AbNext->AbDead))
			 /* ce pave de presentation existe deja */
			 pAbbCreated = NULL;
		     pAbb1 = pAbb1->AbNext;
		   }
	       while (!stop);
	       if (pAbbCreated != NULL)
		 {
		   pAb->AbReadOnly = pAbb1->AbReadOnly;
		   pAb->AbEnclosing = pAbb1->AbEnclosing;
		   pAb->AbNext = pAbb1->AbNext;
		   pAb->AbPrevious = pAbb1;
		   pAbb1->AbNext = pAb;
		   if (pAb->AbNext != NULL)
		     pAb->AbNext->AbPrevious = pAb;
		 }
	       break;
	     case FnCreateEnclosing:
	       if (pAbb1->AbEnclosing != NULL)
		 if (pAbb1->AbEnclosing->AbPresentationBox)
		   if (pAbb1->AbEnclosing->AbElement == pEl)
		     /* l'element a deja un pave de presentation englobant. */
		     /* on refuse d'en creer un autre */
		     pAbbCreated = NULL;
	       if (pAbbCreated != NULL)
		 {
		   pAb->AbReadOnly = pAbb1->AbReadOnly;
		   pAb->AbEnclosing = pAbb1->AbEnclosing;
		   if (pAb->AbEnclosing->AbFirstEnclosed == pAbb1)
		     pAb->AbEnclosing->AbFirstEnclosed = pAb;
		   pAb->AbPrevious = pAbb1->AbPrevious;
		   pAbb1->AbPrevious = NULL;
		   if (pAb->AbPrevious != NULL)
		     pAb->AbPrevious->AbNext = pAb;
		   pAb->AbFirstEnclosed = pAbb1;
		   /* traite les paves de presentation deja crees par les */
		   /* regles CreateBefore et CreateAfter */
		   stop = FALSE;
		   while (!stop)
		     {
		       pAbb1->AbEnclosing = pAb;
		       if (pAbb1->AbNext == NULL)
			 stop = TRUE;
		       else if (pAbb1->AbNext->AbElement != pEl)
			 stop = TRUE;
		       else
			 pAbb1 = pAbb1->AbNext;
		     }
		   /* traite le dernier pave' de l'element */
		   pAb->AbNext = pAbb1->AbNext;
		   pAbb1->AbNext = NULL;
		   if (pAb->AbNext != NULL)
		     pAb->AbNext->AbPrevious = pAb;
		 }
	       break;
	     default:
	       break;
	     }

	   if (pAbbCreated == NULL)	/* pave deja cree' */
	     {
	       if (pAb->AbBox != NULL)
		 {
		   /* libere le pave */
		   if (pAb->AbLeafType == LtPicture)
		     {
		       if (!pAb->AbElement->ElTerminal ||
			   pAb->AbElement->ElLeafType != LtPicture)
			 /* ce n'est pas un element image */
			 FreePictInfo (pAb->AbPictInfo);
		       pAb->AbPictInfo = NULL;
		     }
		 }

	       if (pAb->AbLeafType == LtCompound && pAb->AbPictBackground != NULL)
		 {
		   /* in this particular case we need to free filename */
		   TtaFreeMemory (((PictInfo *)(pAb->AbPictBackground))->PicFileName);
		   FreePictInfo (pAb->AbPictBackground);
		   pAb->AbPictBackground = NULL;
		 }
	       FreeAbstractBox (pAb);
	     }
	   else
	     /* rend non modifiable le pave de presentation */
	     {
	       pAb->AbCanBeModified = FALSE;
	       pAb->AbLeafType = LtCompound;
	       pAb->AbVolume = 0;
	       pAb->AbInLine = FALSE;
	       pAb->AbTruncatedHead = FALSE;
	       pAb->AbTruncatedTail = FALSE;
	     }

	   if (pAbbCreated != NULL)
	     {
	       if (pER != NULL)
		 /* change le pointeur de pave de l'element englobant les */
		 /* elements associes a mettre dans la boite */
		 {
		   pER->ElParent->ElAbstractBox[viewIndex] = pAbbCreated;
		   pAbbCreated->AbPresentationBox = FALSE;
		 }
	       /* applique les regles de presentation de la boite creee. */
	       do
		 {
		   pR = GetRule (&pRS, &pRD, pEl, pAttr, pEl->ElStructSchema);
		   /* pointeur sur la regle a  appliquer pour la vue 1 */
		   if (pR != NULL)
		     /* cherche et applique les regles de tous les types */
		     /* pour la vue */
		     for (view = 1; view <= MAX_VIEW; view++)
		       {
			 if (view == 1)
			   pRV = NULL;
			 else
			   pRV = GetRuleView (&pRS, &pRD, pR->PrType, view, pEl,
					      pAttr, pEl->ElStructSchema);
			 if (view == viewSch)
			   /* applique la regle au pave cree'. */
			   {
			     if (pRV == NULL)
			       pRV = pR;
			     if (pRV->PrType == PtFunction
				 && (pRV->PrPresFunction == FnCreateFirst ||
				     pRV->PrPresFunction == FnCreateLast))
			       /* le pave cree' cree un pave de presentation */
			       {
				 pBox = &pSchP->PsPresentBox[pRCre->PrPresBox[0] - 1];
				 if (pBox->PbContent == ContElement
				     && (pBox->PbPageFooter || pBox->PbPageHeader))
				   /* une boite de bas de page qui affiche des */
				   /* elements associes, on ne creera la boite */
				   /* a creer que lorsque les elements */
				   /* associes  auront ete crees. */
				   {
				     if (lqueue < MAX_QUEUE_LEN)
				       {
					 lqueue++;
					 queuePR[lqueue - 1] = pRV;
				       }
				   }
				 else
				   /* sauve le pointeur de pave de l'element */
				   {
				     pAbbNext = pEl->ElAbstractBox[viewIndex];
				     /* change le pointeur de pave de l'element */
				     /* pour un chainage correct du pave a creer */
				     pEl->ElAbstractBox[viewIndex] = pAbbCreated;
				     pAbbCreated->AbPresentationBox = FALSE;
				     /* cree le pave de presentation */
				     pAbb1 = CrAbsBoxesPres (pEl, pDoc, pRV, pSS, NULL,
							     viewNb, pSchP, FALSE, TRUE);
				     /* restaure le pointeur de pave de l'elem */
				     pEl->ElAbstractBox[viewIndex] = pAbbNext;
				   }
			       }
			     else if (!completeCreator && pRV->PrPresMode == PresInherit
				      && pRV->PrInheritMode == InheritCreator)
			       /* toutes les regles de presentation n'ont */
			       /* pas encore ete appliquees au pave */
			       /* et le pave cree herite du createur, on */
			       /* differe l'application de la regle */
			       Delay (pRV, pSchP, pAbbCreated, NULL, pAbbCreated);
			     else if (!ApplyRule (pRV, pSchP, pAbbCreated, pDoc, NULL))
			       /* on n'a pas pu appliquer la regle, on */
			       /* l'appliquera lorsque le pave pere */
			       /* sera  termine' */
			       Delay (pRV, pSchP, pAbbCreated, NULL, pAbbCreated);
			   }
		       }
		 }
	       while (pR != NULL);

	       pAbbCreated->AbPresentationBox = TRUE;
	       /* met le contenu dans le pave cree */
	       pBox = &pSchP->PsPresentBox[pRCre->PrPresBox[0] - 1];
	       switch (pBox->PbContent)
		 {
		 case FreeContent:
		   break;
		 case ContVariable:
		   ok = NewVariable (pBox->PbContVariable, pSS, pSchP, pAb, pDoc);
		   break;
		 case ContConst:
		   ConstantCopy (pBox->PbContConstant, pSchP, pAb);
		   break;
		 case ContElement:
		   /* une boite qui regroupe des elements associes */
		   /* rend modifiable le pave de presentation */
		   pAbbCreated->AbCanBeModified = TRUE;
		   /* cherche les references a ces elements jusqu'a la */
		   /* marque de page precedente qui concerne cette vue, et */
		   /* cree les paves des elements reference's. */
		   stop = FALSE;
		   pE = pEl;
		   pAbb1 = NULL;
		   do
		     {
		       pER = NextElRef (&pE, pBox->PbContRefElem, pEl->ElStructSchema,
					pBox->PbPageFooter, &viewSch);
		       if (pER == NULL)
			 /* il n'y a plus de reference dans la page */
			 stop = TRUE;
		       else
			 /* on a trouve' une reference */
			 if (!IsASavedElement (pER))
			   /* on ne traite pas l'element reference' s'il fait */
			   /* partie des elements qui ont ete coupe's */
			   if (pER->ElAbstractBox[viewIndex] == NULL)
			     /* les paves de l'element reference' n'ont pas */
			     /* encore ete cree's. */
			     /* Volume libre infini pour que tout le contenu */
			     /* de ces elem. soit cree' dans l'image abstraite */
			     {
			       if (AssocView (pER))
				 {
				   volume = pDoc->DocAssocFreeVolume[pER->ElAssocNum - 1];
				   pDoc->DocAssocFreeVolume[pER->ElAssocNum - 1] = THOT_MAXINT;
				 }
			       else
				 {
				   volume = pDoc->DocViewFreeVolume[viewIndex];
				   pDoc->DocViewFreeVolume[viewIndex] = THOT_MAXINT;
				 }
			       /* cree les paves de l'element reference' */
			       pAbb1 = AbsBoxesCreate (pER, pDoc, viewNb, TRUE, TRUE,
						       &complete);
			       /* verifie les elements associes voisins */
			       pElSibling = pER;
			       finish = FALSE;
			       do
				 {
				   if (pBox->PbPageFooter)
				     pElSibling = pElSibling->ElNext;
				   else
				     pElSibling = pElSibling->ElPrevious;
				   if (pElSibling == NULL)
				     finish = TRUE;
				   else
				     {
				       if (pElSibling->ElReferredDescr == NULL)
					 
					 /* l'element voisin n'est pas reference', on */
					 /* va creer ses paves */
					 ok = TRUE;
				       else if (pElSibling->ElReferredDescr->ReFirstReference == NULL)
					 ok = TRUE;
				       else if (pElSibling->ElReferredDescr->ReFirstReference->RdElement == NULL)
					 ok = TRUE;
				       else if (pElSibling->ElReferredDescr->ReFirstReference->RdElement->
						ElAssocNum == pElSibling->ElAssocNum)
					 /* l'element voisin est reference' depuis */
					 /* un element associe' de meme */
					 
					 /* type; on cree ses paves */
					 ok = TRUE;
				       else
					 /* inutile de creer les paves du voisin, il seront */
					 /* crees lorsqu'on rencontrera sa 1ere reference */
					 ok = FALSE;
				       if (ok)
					 /* cree les paves du voisin */
					 pAbb1 = AbsBoxesCreate (pElSibling, pDoc, viewNb, TRUE, TRUE, &complete);
				       else
					 /* on arrete de traiter les voisins */
					 finish = TRUE;
				     }
				 }
			       while (!finish);

			       /* retablit le volume libre reel */
			       if (AssocView (pER))
				 {
				   pDoc->DocAssocFreeVolume[pER->ElAssocNum - 1] = pDoc->DocAssocFreeVolume[pER->ElAssocNum - 1] - THOT_MAXINT + volume;
				   if (pDoc->DocAssocFreeVolume[pER->ElAssocNum - 1] < 0)
				     pDoc->DocAssocFreeVolume[pER->ElAssocNum - 1] = 0;
				 }
			       else
				 {
				   pDoc->DocViewFreeVolume[viewIndex] = pDoc->DocViewFreeVolume[viewIndex] - THOT_MAXINT + volume;
				   if (pDoc->DocViewFreeVolume[viewIndex] < 0)
				     pDoc->DocViewFreeVolume[viewIndex] = 0;
				 }
			     }
		     }
		   while (!stop);
		   /* le volume des paves englobants prend deja en compte le */
		   /* volume de la boite cree */
		   volok = TRUE;
		   break;
		 }
	       do
		 if (pqueue >= lqueue)
		   pR = NULL;
		 else
		   {
		     pqueue++;
		     pR = queuePR[pqueue - 1];
		     /* sauve le pointeur de pave de l'element */
		     pAbbNext = pEl->ElAbstractBox[viewIndex];
		     /* change le pointeur de pave de l'element, pour un */
		     /* chainage correct du pave a creer */
		     pEl->ElAbstractBox[viewIndex] = pAbbCreated;
		     pAbbCreated->AbPresentationBox = FALSE;
		     /* cree le pave de presentation */
		     pAbb1 = CrAbsBoxesPres (pEl, pDoc, pR, pSS, NULL, viewNb, pSchP,
					     TRUE, TRUE);
		     /* restaure le pointeur de pave de l'element */
		     pEl->ElAbstractBox[viewIndex] = pAbbNext;
		   }
	       while (pR != NULL);
	       do		/* applique les regles retardees */
		 {
		   pAbb1 = pAbbCreated;
		   GetDelayedRule (&pR, &pSP, &pAbb1, &pA);
		   if (pR != NULL)
		     if (!ApplyRule (pR, pSP, pAbb1, pDoc, pA))
		       Delay (pR, pSP, pAbb1, pA, pAbbCreated);
		 }
	       while (pR != NULL);
	       /* retablit AbPresentationBox qui a ete modifie' pour les boites de */
	       /* haut ou de bas de page qui regroupent des elements associes */
	       pAbbCreated->AbPresentationBox = TRUE;
	       /* ajoute le volume du pave cree' a celui de tous ses */
	       /* englobants */
	       if (pAbbCreated->AbVolume > 0 && !volok)
		 {
		   pAbb1 = pAbbCreated->AbEnclosing;
		   while (pAbb1 != NULL)
		     {
		       pAbb1->AbVolume += pAbbCreated->AbVolume;
		       pAbb1 = pAbb1->AbEnclosing;
		     }
		 }
	       /* met a jour le volume libre restant dans la vue */
	       UpdateFreeVol (pAbbCreated, pDoc);
	       if (pEl->ElTypeNumber == PageBreak + 1)
		 /* c'est une boite de haut ou bas de page. Sa creation */
		 /* affecte peut-etre les autres boites de haut ou bas de page */
		 /* deja creees. */
		 if (InAssocBox && !pAbbCreated->AbEnclosing->AbPresentationBox)
		   {
		     pAbbCreated->AbEnclosing->AbPresentationBox = TRUE;
		     ApplyRefAbsBoxNew (pAbbCreated, pAbbCreated, &pAbb1, pDoc);
		     pAbbCreated->AbEnclosing->AbPresentationBox = FALSE;
		   }
		 else
		   ApplyRefAbsBoxNew (pAbbCreated, pAbbCreated, &pAbb1, pDoc);
	       
	       /* si c'est une boite contenant une image, choisit le mode */
	       /* de presentation de l'image en accord avec les regle de */
	       /* dimensions du pave */
	       if (pAbbCreated->AbLeafType == LtPicture)
		 {
		   /* a priori l'image prendra les dimensions de son pave' */
		   ((PictInfo *) (pAbbCreated->AbPictInfo))->PicPresent = ReScale;
		   if (!pAbbCreated->AbWidth.DimIsPosition
		       && pAbbCreated->AbWidth.DimValue == 0
		       && pAbbCreated->AbWidth.DimAbRef == NULL
		       && !pAbbCreated->AbHeight.DimIsPosition
		       && pAbbCreated->AbHeight.DimValue == 0
		       && pAbbCreated->AbHeight.DimAbRef == NULL)
		     /* le pave prend la hauteur et la largeur de son
			contenu */
		     /* l'image doit etre affichee telle quelle */
		     ((PictInfo *) (pAbbCreated->AbPictInfo))->PicPresent = RealSize;
		 }
	     }
	 }
     }
   return (pAbbCreated);
}
#endif /* __COLPAGE__ */


/*----------------------------------------------------------------------
   AttrPresRule retourne la premiere regle de la chaine des regles
   de presentation a` appliquer pour l'attribut pAttr.     
   - si inheritRule = true on ne s'interesse pas aux regles   
   par defaut (pRPdef).                                    
   - si inheritRule = false on ne s'interesse pas aux regles  
   d'heritage (pRPherit).                                  
   - si PAttrComp != NULL alors si l'attribut est numerique
   et que ses regles sont des comparaisons on utilisera    
   pAttrComp au lieu de rechercher l'attribut de           
   comparaison dans les ascendants de pEl.                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PtrPRule            AttrPresRule (PtrAttribute pAttr, PtrElement pEl, boolean inheritRule,
				  PtrAttribute pAttrComp, PtrPSchema pSchP)
#else  /* __STDC__ */
PtrPRule            AttrPresRule (pAttr, pEl, inheritRule, pAttrComp, pSchP)
PtrAttribute        pAttr;
PtrElement          pEl;
boolean             inheritRule;
PtrAttribute        pAttrComp;
PtrPSchema          pSchP;
#endif /* __STDC__ */

{
   int                 i;
   boolean             found;
   PtrPRule            pRule;
   PtrAttribute        pAt2;
   PtrElement          pElAttr;
   AttributePres      *pAPRule, *pPRdef, *pPRinherit, *pPRclass;
   NumAttrCase        *pCase;

   pRule = NULL;
   if (pSchP == NULL)
      return (NULL);
   pAPRule = pSchP->PsAttrPRule[pAttr->AeAttrNum - 1];

   /* on cherche quel est le paquet de regles qui s'applique */
   /* pPRdef designera le paquet de regles s'appliquant a tous les elements */
   /* c'est a dire celui pour lequel pAPRule->ApElemType = 0  */
   /* pPRinherit  designera le paquet pour lequel pAPRule->ApElemType = pEl->ElTypeNumber */
   /* pPRclass  designera le paquet pour lequel pAPRule->ApString et      */
   /* pAttr->AeAttrText sont des chaines identiques         */

   pPRclass = pPRdef = pPRinherit = NULL;
   for (i = pSchP->PsNAttrPRule[pAttr->AeAttrNum - 1]; i-- > 0;
	pAPRule = pAPRule->ApNextAttrPres)
     {
	if (pAPRule->ApElemType == 0)
	   pPRdef = pAPRule;
	else if (pAPRule->ApElemType == pEl->ElTypeNumber)
	   pPRinherit = pAPRule;
	if ((pAttr->AeAttrType == AtTextAttr) &&
	    (pAttr->AeAttrText != NULL) &&
	    (StringAndTextEqual (pAPRule->ApString, pAttr->AeAttrText)))
	   pPRclass = pAPRule;
     }

   if (inheritRule)
     {
	if (pPRinherit != NULL)
	   pAPRule = pPRinherit;
	else
	   /* ce n'est pas la peine de continuer */
	   return (NULL);
     }
   else
     {
	if (pPRdef != NULL)
	   pAPRule = pPRdef;
	else
	   /* ce n'est pas la peine de continuer */
	   return (NULL);
     }

   /* selon le type de l'attribut on cherche le debut de la chaine  */
   /* de regles de presentation */
   switch (pAttr->AeAttrType)
	 {
	    case AtNumAttr:
	       i = 1;
	       found = FALSE;
	       while (i <= pAPRule->ApNCases && !found)
		 {
		    pCase = &pAPRule->ApCase[i - 1];
		    if (pCase->CaComparType == ComparConstant)
		      {
			 /* la valeur de comparaison est une cste */
			 if (pAttr->AeAttrValue >= pCase->CaLowerBound &&
			     pAttr->AeAttrValue <= pCase->CaUpperBound)
			   {
			      found = TRUE;
			      pRule = pCase->CaFirstPRule;
			   }
		      }
		    else
		      {
			 /* la valeur de comparaison est un attribut */
			 if ((pCase->CaLowerBound != -1) && (pCase->CaUpperBound != -1))
			   {
			      /* attr = enclosing */
			      if (pAttrComp != NULL)
				 pAt2 = pAttrComp;
			      else
				 pAt2 = GetTypedAttrAncestor (pEl, pCase->CaLowerBound,
					    pAttr->AeAttrSSchema, &pElAttr);
			      if (pAt2 != NULL)
				 if (pAttr->AeAttrValue == pAt2->AeAttrValue)
				   {
				      found = TRUE;
				      pRule = pCase->CaFirstPRule;
				   }
			   }
			 else if (pCase->CaLowerBound == -1)
			   {
			      /* attr < enclosing */
			      if (pAttrComp != NULL)
				 pAt2 = pAttrComp;
			      else
				 pAt2 = GetTypedAttrAncestor (pEl, pCase->CaUpperBound,
					    pAttr->AeAttrSSchema, &pElAttr);
			      if (pAt2 != NULL)
				 if (pAttr->AeAttrValue < pAt2->AeAttrValue)
				   {
				      found = TRUE;
				      pRule = pCase->CaFirstPRule;
				   }
			   }
			 else
			   {
			      /* attr > enclosing */
			      if (pAttrComp != NULL)
				 pAt2 = pAttrComp;
			      else
				 pAt2 = GetTypedAttrAncestor (pEl, pCase->CaLowerBound,
					    pAttr->AeAttrSSchema, &pElAttr);
			      if (pAt2 != NULL)
				 if (pAttr->AeAttrValue > pAt2->AeAttrValue)
				   {
				      found = TRUE;
				      pRule = pCase->CaFirstPRule;
				   }
			   }
		      }
		    i++;
		 }
	       break;
	    case AtTextAttr:
	       if (pPRclass)
		  pRule = pPRclass->ApTextFirstPRule;
	       else if (pAPRule->ApString[0] == EOS)
		  pRule = pAPRule->ApTextFirstPRule;
	       break;
	    case AtReferenceAttr:
	       pRule = pAPRule->ApRefFirstPRule;
	       break;
	    case AtEnumAttr:
	       /* on verifie que la valeur est correcte */
	       if (pAttr->AeAttrValue < 0 || pAttr->AeAttrSSchema->SsAttribute[pAttr->AeAttrNum - 1].AttrNEnumValues < pAttr->AeAttrValue)
		  /* valeur incorrecte, on prend les regles qui s'appliquent a */
		  /* n'importe quelle valeur */
		  pRule = pAPRule->ApEnumFirstPRule[0];
	       else if (pAPRule->ApEnumFirstPRule[pAttr->AeAttrValue] == NULL)
		  /* pas de regles pour cette valeur, on prend les regles */
		  /* qui s'appliquent a n'importe quelle valeur */
		  pRule = pAPRule->ApEnumFirstPRule[0];
	       else
		  /* on prend les regles qui s'appliquent a cette valeur */
		  pRule = pAPRule->ApEnumFirstPRule[pAttr->AeAttrValue];
	       break;
	    default:
	       pRule = NULL;
	       break;
	 }

   return pRule;
}


/*----------------------------------------------------------------------
   ApplCrPresRule determine les regles de creation a appliquer    
   au pave pAb en fonction de head                        
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         ApplCrPresRule (PtrSSchema pSS, PtrPSchema pSP, PtrAbstractBox * pAbbCreated,
		   PtrAttribute pAttr, PtrDocument pDoc, PtrAbstractBox pAb,
				    boolean head, PtrPRule pRule)

#else  /* __STDC__ */
static void         ApplCrPresRule (pSS, pSP, pAbbCreated, pAttr, pDoc, pAb, head, pRule)
PtrSSchema          pSS;
PtrPSchema          pSP;
PtrAbstractBox     *pAbbCreated;
PtrAttribute        pAttr;
PtrDocument         pDoc;
PtrAbstractBox      pAb;
boolean             head;
PtrPRule            pRule;

#endif /* __STDC__ */

{
   PtrAbstractBox      pAbb, pAbbR;
   boolean             stop;

   /* saute les regles precedant les fonctions */
   stop = FALSE;
   do
      if (pRule == NULL)
	 stop = TRUE;
      else if (pRule->PrType > PtFunction)
	{
	   stop = TRUE;
	   pRule = NULL;
	   /* pas de fonction de presentation */
	}
      else if (pRule->PrType == PtFunction)
	 stop = TRUE;
      else
	 pRule = pRule->PrNextPRule;
   while (!stop);
   /* cherche toutes les fonctions de creation */
   stop = FALSE;
   do
      if (pRule == NULL)
	 stop = TRUE;
      else if (pRule->PrType != PtFunction)
	 stop = TRUE;
      else
	{
	   /* applique les fonctions de creation qui correspondent a */
	   /* l'extremite concernee */
	   /* si la regle de creation possede l'indication de repetition */
	   /* on appelle la procedure de creation systematiquement */
#ifdef __COLPAGE__
	   if ((head
		&& (pRule->PrPresFunction == FnCreateBefore
		    || pRule->PrPresFunction == FnCreateFirst))
	       || (!head
		   && (pRule->PrPresFunction == FnCreateAfter
		       || pRule->PrPresFunction == FnCreateLast))
	       || (pRule->PrPresBoxRepeat
		   && (pRule->PrPresFunction == FnCreateBefore
		       || pRule->PrPresFunction == FnCreateFirst
		       || pRule->PrPresFunction == FnCreateAfter
		       || pRule->PrPresFunction == FnCreateLast)))
#else  /* __COLPAGE__ */
	   if ((head
		&& (pRule->PrPresFunction == FnCreateBefore
		    || pRule->PrPresFunction == FnCreateFirst))
	       || (!head
		   && (pRule->PrPresFunction == FnCreateAfter
		       || pRule->PrPresFunction == FnCreateLast)))
#endif /* __COLPAGE__ */
	     {
		pAbb = CrAbsBoxesPres (pAb->AbElement, pDoc, pRule,
			      pSS, pAttr, pAb->AbDocView, pSP, FALSE, TRUE);
		if (pAbb != NULL)
		   /* TODO : valeur de pAbb si plusieurs paves crees avec Rep ?? */
		  {
		     if (!head)
			*pAbbCreated = pAbb;
		     else if (*pAbbCreated == NULL)
			*pAbbCreated = pAbb;
		     /* modifie les paves environnant */
		     /* qui dependent du pave cree */
		     /* TODO : si Rep est vrai, plusieurs paves ont ete crees */
		     /* faut-il appeler ApplyRefAbsBoxNew dans CrAbsBoxesPres ?? */
		     ApplyRefAbsBoxNew (*pAbbCreated, *pAbbCreated, &pAbbR, pDoc);
		     /* passe a la regle suivante */
		  }
	     }
	   pRule = pRule->PrNextPRule;
	}
   while (!stop);
}


/*----------------------------------------------------------------------
   TruncateOrCompleteAbsBox coupe ou complete le pave pAb. pAb est coupe' si    
   truncate est vrai ou complete si truncate est faux. La coupure 
   ou la completion concerne la tete du pave si head est   
   vrai ou la queue du pave si head est faux. Marque cette 
   information dans le pave. S'il s'agit d'un pave qui     
   devient complet, cree ses paves de presentation a`      
   l'extremite qui devient complet. Retourne un pointeur  
   sur le dernier pave de presentation cree ou NULL si     
   aucun pave n'est cree.                                  
  ----------------------------------------------------------------------*/

#ifdef __STDC__
PtrAbstractBox      TruncateOrCompleteAbsBox (PtrAbstractBox pAb, boolean truncate, boolean head, PtrDocument pDoc)

#else  /* __STDC__ */
PtrAbstractBox      TruncateOrCompleteAbsBox (pAb, truncate, head, pDoc)
PtrAbstractBox      pAb;
boolean             truncate;
boolean             head;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrPRule            pRule;
   PtrPSchema          pSchP;
   int                 index;
   PtrSSchema          pSchS;
   PtrAbstractBox      pAbbCreated;
   PtrAttribute        pAttr;
   PtrElement          pElAttr;
   int                 l;
   InheritAttrTable   *inheritTable;
   PtrHandlePSchema    pHd;

   pAbbCreated = NULL;
   if (pAb != NULL)
#ifdef __COLPAGE__
      if (!(pAb->AbElement->ElTerminal &&
	    pAb->AbElement->ElLeafType == LtPageColBreak))
	{
#endif /* __COLPAGE__ */
	   if (pAb->AbLeafType == LtCompound)
	      if (pAb->AbInLine)
		{
		   if (!truncate)
		      if (head)
			 pAb->AbTruncatedHead = FALSE;
		      else
			 pAb->AbTruncatedTail = FALSE;
		}
	      else if ((head && pAb->AbTruncatedHead != truncate)
		       || (!head && pAb->AbTruncatedTail != truncate))
		 /* il y a effectivement changement */
		{
		   if (!truncate)
		     {
			/* le pave n'est plus coupe' a` une extremite. Cree les */
			/* paves de presentation a` cette extremite. */
			/* cherche la 1ere regle de presentation associee a ce type */
			/* d'element */
			SearchPresSchema (pAb->AbElement, &pSchP, &index, &pSchS);
			pRule = pSchP->PsElemPRule[index - 1];

			/* traite les regles de creation associees au type de l'element */
			pAttr = NULL;
			ApplCrPresRule (pSchS, pSchP, &pAbbCreated, NULL, pDoc,
					pAb, head, pRule);
			/* traite les regles de creation dues a */
			/* l'heritage des attributs */
			if (pAb->AbElement->ElStructSchema->SsPSchema != NULL)
			   if (pAb->AbElement->ElStructSchema->SsPSchema->
			       PsNInheritedAttrs[pAb->AbElement->ElTypeNumber - 1])
			     {
				/* il y a heritage possible */
				if ((inheritTable = pAb->AbElement->ElStructSchema->SsPSchema->
				     PsInheritedAttr[pAb->AbElement->ElTypeNumber - 1])
				    == NULL)
				  {
				     /* cette table n'existe pas on la genere */
				     CreateInheritedAttrTable (pAb->AbElement);
				     inheritTable = pAb->AbElement->ElStructSchema->
					SsPSchema->PsInheritedAttr[pAb->AbElement->ElTypeNumber - 1];
				  }
				for (l = 1; l <= pAb->AbElement->ElStructSchema->SsNAttributes; l++)
				   if ((*inheritTable)[l - 1])
				      /* pAb->AbElement herite de l'attribut l */
				      if ((pAttr = GetTypedAttrAncestor (pAb->AbElement, l,
									 pAb->AbElement->ElStructSchema, &pElAttr)) != NULL)
					 /* cherche si l existe au dessus */
					{
					   /* on cherchera d'abord dans le schema de */
					   /* presentation principal de l'attribut */
					   pSchP = pAttr->AeAttrSSchema->SsPSchema;
					   pHd = NULL;
					   while (pSchP != NULL)
					     {
						pRule = AttrPresRule (pAttr, pAb->AbElement,
							 TRUE, NULL, pSchP);
						ApplCrPresRule (pAttr->AeAttrSSchema, pSchP,
								&pAbbCreated, pAttr, pDoc, pAb,
								head, pRule);
						if (pHd == NULL)
						  {
						     /* on n'a pas encore cherche' dans les schemas
						        de presentation additionnels. On prend le
						        premier schema additionnel si on travaille
						        pour la vue principale, sinon on ignore les
						        schemas additionnels */
						     if (pDoc->DocView[pAb->AbDocView - 1].DvPSchemaView == 1)
							pHd = pAttr->AeAttrSSchema->SsFirstPSchemaExtens;
						  }
						else
						   /* passe au schema additionnel suivant */
						   pHd = pHd->HdNextPSchema;
						if (pHd == NULL)
						   /* il n'y a pas (ou plus) de schemas
						      additionnels a prendre en compte */
						   pSchP = NULL;
						else
						   pSchP = pHd->HdPSchema;
					     }
					}
			     }
			/* traite les regles de creation associees aux attributs de */
			/* l'element */
			pAttr = pAb->AbElement->ElFirstAttr;
			/* 1er attribut de l'element */
			/* boucle sur les attributs de l'element */
			while (pAttr != NULL)
			  {
			     /* cherche le debut des regles de presentation a */
			     /* appliquer pour l'attribut */
			     /* on cherchera d'abord dans le schema de */
			     /* presentation principal de l'attribut */
			     pSchP = pAttr->AeAttrSSchema->SsPSchema;
			     pHd = NULL;
			     while (pSchP != NULL)
			       {
				  pRule = AttrPresRule (pAttr, pAb->AbElement, FALSE,
							NULL, pSchP);
				  ApplCrPresRule (pAttr->AeAttrSSchema, pSchP, &pAbbCreated, pAttr,
						  pDoc, pAb, head, pRule);
				  if (pHd == NULL)
				    {
				       /* on n'a pas encore cherche' dans les schemas de
				          presentation additionnels. On prend le premier schema
				          additionnel si on travaille pour la vue principale,
				          sinon on ignore les schemas additionnels. */
				       if (pDoc->DocView[pAb->AbDocView - 1].DvPSchemaView == 1)
					  pHd = pAttr->AeAttrSSchema->SsFirstPSchemaExtens;
				    }
				  else
				     /* passe au schema additionnel suivant */
				     pHd = pHd->HdNextPSchema;
				  if (pHd == NULL)
				     /* il n'y a pas (ou plus) de schemas additionnels a
				        prendre en compte */
				     pSchP = NULL;
				  else
				     pSchP = pHd->HdPSchema;
			       }
			     /* passe a l'attribut suivant de l'element */
			     pAttr = pAttr->AeNext;
			  }
		     }
		   if (head)
		      pAb->AbTruncatedHead = truncate;
		   else
		      pAb->AbTruncatedTail = truncate;
		}
#ifdef __COLPAGE__
	}
#endif /* __COLPAGE__ */

   return (pAbbCreated);
}


/*----------------------------------------------------------------------
   IsViewFull retourne vrai si la vue viewNb est pleine.             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             IsViewFull (DocViewNumber viewNb, PtrDocument pDoc, PtrElement pEl)
#else  /* __STDC__ */
boolean             IsViewFull (viewNb, pDoc, pEl)
DocViewNumber       viewNb;
PtrDocument         pDoc;
PtrElement          pEl;
#endif /* __STDC__ */

{
  int               i;
  boolean           full;

  if (!AssocView (pEl))
    {
      /* ce n'est pas une vue d'elements associes */
      i =  viewNb - 1;
      full = (pDoc->DocView[i].DvPSchemaView == 0 || pDoc->DocViewFreeVolume[i] <= 0);
    }
  else
    {
      /* c'est une vue d'elements associes */
      i = pEl->ElAssocNum - 1;
      full = (pDoc->DocAssocFrame[i] == 0 || pDoc->DocAssocFreeVolume[i] < 0);
    }
  return (full);
}


/*----------------------------------------------------------------------
   WaitingRule met en attente la regle pR appartenant au schema de     
   presentation pSP. Cette regle sera reappliquee au pave  
   pAbb lorsque la descendance de ce pave aura ete creee.    
   pA est l'attribut auquel correspond la regle (NULL si   
   ce n'est pas une regle d'attribut).                     
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                WaitingRule (PtrPRule pR, PtrAbstractBox pAbb, PtrPSchema pSP, PtrAttribute pA,
     PtrAttribute queuePA[MAX_QUEUE_LEN], PtrPSchema queuePS[MAX_QUEUE_LEN],
     PtrAbstractBox queuePP[MAX_QUEUE_LEN], PtrPRule queuePR[MAX_QUEUE_LEN],
				 int *lqueue)

#else  /* __STDC__ */
void                WaitingRule (pR, pAbb, pSP, pA, queuePA, queuePS, queuePP, queuePR, lqueue)
PtrPRule            pR;
PtrAbstractBox      pAbb;
PtrPSchema          pSP;
PtrAttribute        pA;
PtrAttribute        queuePA[MAX_QUEUE_LEN];
PtrPSchema          queuePS[MAX_QUEUE_LEN];
PtrAbstractBox      queuePP[MAX_QUEUE_LEN];
PtrPRule            queuePR[MAX_QUEUE_LEN];
int                *lqueue;

#endif /* __STDC__ */

{
   if (*lqueue >= MAX_QUEUE_LEN)
      printf ("Increase MAX_QUEUE_LEN\n");
   else
     {
	(*lqueue)++;
	queuePR[*lqueue - 1] = pR;
	queuePP[*lqueue - 1] = pAbb;
	queuePS[*lqueue - 1] = pSP;
	queuePA[*lqueue - 1] = pA;
     }
}


/*----------------------------------------------------------------------
   GetAtt recupere une regle de presentation qui etait en attente. 
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         GetAtt (PtrPRule * pR, PtrAbstractBox * pAbb, PtrPSchema * pSP,
		     PtrAttribute * pA, PtrAttribute queuePA[MAX_QUEUE_LEN],
   PtrPSchema queuePS[MAX_QUEUE_LEN], PtrAbstractBox queuePP[MAX_QUEUE_LEN],
		  PtrPRule queuePR[MAX_QUEUE_LEN], int *lqueue, int *pqueue)

#else  /* __STDC__ */
static void         GetAtt (pR, pAbb, pSP, pA, queuePA, queuePS, queuePP, queuePR, lqueue,
			    pqueue)
PtrPRule           *pR;
PtrAbstractBox     *pAbb;
PtrPSchema         *pSP;
PtrAttribute       *pA;
PtrAttribute        queuePA[MAX_QUEUE_LEN];
PtrPSchema          queuePS[MAX_QUEUE_LEN];
PtrAbstractBox      queuePP[MAX_QUEUE_LEN];
PtrPRule            queuePR[MAX_QUEUE_LEN];
int                *lqueue;
int                *pqueue;

#endif /* __STDC__ */

{
   *pR = NULL;
   while (*pR == NULL && *pqueue < *lqueue)
     {
	(*pqueue)++;
	*pR = queuePR[*pqueue - 1];
	*pAbb = queuePP[*pqueue - 1];
	*pSP = queuePS[*pqueue - 1];
	*pA = queuePA[*pqueue - 1];
     }
}


/*----------------------------------------------------------------------
   ApplCrRule verifie que la regle pRuleCr appartenant au   
   schema de presentation pSchPres (correspondant au       
   schema de structure pSS), est une regle de creation et, 
   si oui, tente de l'appliquer a` l'element pEl. La       
   fonction retourne Vrai s'il s'agit bien d'une regle de  
   creation. pA est l'attribut auquel correspond la regle, 
   s'il s'agit d'une regle de presentation d'attribut (NULL
   sinon).                                                 
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static boolean      ApplCrRule (PtrPRule pRuleCr, PtrSSchema pSS,
				PtrPSchema pSchPres, PtrAttribute pA,
			  PtrAbstractBox * pAbbReturn, DocViewNumber viewNb,
				PtrDocument pDoc, PtrElement pEl,
				boolean forward, int *lqueue,
				PtrPRule queuePR[MAX_QUEUE_LEN],
				PtrAbstractBox queuePP[MAX_QUEUE_LEN],
				PtrPSchema queuePS[MAX_QUEUE_LEN],
				PtrAttribute queuePA[MAX_QUEUE_LEN],
				PtrAbstractBox pNewAbbox)

#else  /* __STDC__ */
static boolean      ApplCrRule (pRuleCr, pSS, pSchPres, pA, pAbbReturn, viewNb,
				pDoc, pEl, forward, lqueue, queuePR, queuePP,
				queuePS, queuePA, pNewAbbox)
PtrPRule            pRuleCr;
PtrSSchema          pSS;
PtrPSchema          pSchPres;
PtrAttribute        pA;
PtrAbstractBox     *pAbbReturn;
DocViewNumber       viewNb;
PtrDocument         pDoc;
PtrElement          pEl;
boolean             forward;
int                *lqueue;
PtrPRule            queuePR[MAX_QUEUE_LEN];
PtrAbstractBox      queuePP[MAX_QUEUE_LEN];
PtrPSchema          queuePS[MAX_QUEUE_LEN];
PtrAttribute        queuePA[MAX_QUEUE_LEN];
PtrAbstractBox      pNewAbbox;

#endif /* __STDC__ */

{
   boolean             result, toCreate;
   PtrAbstractBox      pAbb;

   if (pRuleCr->PrType != PtFunction)
     {
	result = FALSE;
	/* ce n'est pas une regle de creation */
     }
   else
     {
	/* c'est une fonction de presentation */
	result = pRuleCr->PrPresFunction == FnCreateFirst
	   || pRuleCr->PrPresFunction == FnCreateLast
	   || pRuleCr->PrPresFunction == FnCreateBefore
	   || pRuleCr->PrPresFunction == FnCreateWith
	   || pRuleCr->PrPresFunction == FnCreateAfter
	   || pRuleCr->PrPresFunction == FnCreateEnclosing;
	toCreate = FALSE;	/* a priori il n' y a pas de pave a creer */
	if (pNewAbbox != NULL)
	   switch (pRuleCr->PrPresFunction)
		 {
		    case FnCreateFirst:
		       if (pNewAbbox->AbLeafType != LtCompound
			   || pNewAbbox->AbInLine
#ifdef __COLPAGE__
			   || pRuleCr->PrPresBoxRepeat
#endif /* __COLPAGE__ */
			   || !pNewAbbox->AbTruncatedHead)
			  toCreate = TRUE;
		       break;
		    case FnCreateLast:
#ifdef __COLPAGE__
		       if (pRuleCr->PrPresBoxRepeat)
			  toCreate = TRUE;
		       else
#endif /* __COLPAGE__ */
			  if (pNewAbbox->AbLeafType != LtCompound
			      || pNewAbbox->AbInLine
			      || !pNewAbbox->AbTruncatedTail)
			  /* on appliquera la regle de creation quand */
			  /* tous les paves descendants de l'element */
			  /* seront crees */
			  WaitingRule (pRuleCr, pNewAbbox, pSchPres, pA, queuePA, queuePS, queuePP,
				       queuePR, lqueue);
		       break;
		    case FnCreateBefore:
#ifdef __COLPAGE__
		       if (pRuleCr->PrPresBoxRepeat)
			  toCreate = TRUE;
		       else
#endif /* __COLPAGE__ */
		       if (forward)
			  /* on ne cree le pave de presentation que si */
			  /* le pave de l'element est complete en tete */
			 {
			    if (pNewAbbox->AbLeafType != LtCompound
				|| pNewAbbox->AbInLine
				|| !pNewAbbox->AbTruncatedHead)
			       toCreate = TRUE;
			 }
		       else if (pNewAbbox->AbLeafType != LtCompound
				|| pNewAbbox->AbInLine)
			  /* on appliquera la regle de creation quand */
			  /* tous les paves descendants de l'element */
			  /* seront crees */
			  WaitingRule (pRuleCr, pNewAbbox, pSchPres, pA, queuePA, queuePS, queuePP,
				       queuePR, lqueue);
		       break;
		    case FnCreateWith:
		    case FnCreateEnclosing:
		       toCreate = TRUE;
		       break;
		    case FnCreateAfter:
#ifdef __COLPAGE__
		       if (pRuleCr->PrPresBoxRepeat)
			  toCreate = TRUE;
		       else
#endif /* __COLPAGE__ */
		       if (forward)
			 {
			    if (pNewAbbox->AbLeafType != LtCompound || pNewAbbox->AbInLine)
			       /* on appliquera la regle de creation quand */
			       /* tous les paves descendants de l'element */
			       /* seront crees */
			       WaitingRule (pRuleCr, pNewAbbox, pSchPres, pA, queuePA, queuePS,
					    queuePP, queuePR, lqueue);
			 }
		       else
			  /* on ne cree le pave de presentation que si */
			  /* le pave de l'element est complete en queue */
			  if (pNewAbbox->AbLeafType != LtCompound || pNewAbbox->AbInLine ||
			      !pNewAbbox->AbTruncatedTail)
			  toCreate = TRUE;
		       break;
		    default:
		       break;
		 }
	/* c'est une creation */
	if (toCreate)
	   /* on cree le pave de presentation */
	  {
	     pAbb = CrAbsBoxesPres (pEl, pDoc, pRuleCr, pSS, pA, viewNb, pSchPres, FALSE,
				    FALSE);
	     if (pAbb != NULL)
		/* le pave de presentation a ete cree */
		if (pRuleCr->PrPresFunction == FnCreateBefore ||
		    pRuleCr->PrPresFunction == FnCreateEnclosing)
		   /* il a ete cree devant */
		   if (*pAbbReturn == pNewAbbox)
		      /* on s'appretait a retourner */
		      /* un pointeur sur le pave createur */
		      /* on retourne un pointeur sur la pave cree, qui est */
		      /* le premier pave de l'element */
		      *pAbbReturn = pAbb;
	  }
     }
   return result;
}


/*----------------------------------------------------------------------
   DescVisible cherche le premier descendant de l'element pE qui   
   ait un pave dans la vue viewNb.                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static PtrElement   DescVisible (PtrElement pE, DocViewNumber viewNb, boolean forward)
#else  /* __STDC__ */
static PtrElement   DescVisible (pE, viewNb, forward)
PtrElement          pE;
DocViewNumber       viewNb;
boolean             forward;

#endif /* __STDC__ */
{
   PtrElement          pElem, pDesc;

   pDesc = NULL;
   /* a priori, pas de descendant visible */

   if (pE != NULL)
      if (!pE->ElTerminal && pE->ElFirstChild != NULL)
	 /* l'element a bien une descendance */
	 /* on va traiter tous ses fils, en commencant par le premier, si */
	 /* la creation a lieu en arriere... */
	{
	   pElem = pE->ElFirstChild;
	   /* ...ou par le dernier si elle a lieu en avant */
	   if (forward)
	      while (pElem->ElNext != NULL)
		 pElem = pElem->ElNext;
#ifdef __COLPAGE__
	   do
	     {
		/* on saute les elements marque page si !forward car l'ordre n'est */
		/* plus respecte */
		if (pElem->ElAbstractBox[viewNb - 1] == NULL
		    || (!forward && pElem->ElTypeNumber == PageBreak + 1))
		   /* l'element n'a pas de pave dans la vue cherche le premier */
		   /* descendant qui ait un pave dans la vue */
		   pDesc = DescVisible (pElem, viewNb, forward);
		else
		   /* l'element a un pave dans la vue, on a trouve' */
		   pDesc = pElem;
		if (pDesc == NULL)
		   /* aucun descendant n'a de pave, on passe a l'element */
		   /* suivant */
		   if (forward)
		      pElem = pElem->ElPrevious;
		   else
		      pElem = pElem->ElNext;
		/* on arrete quand on a trouve ou quand on a traite' tous les */
		/* fils de pE. */
	     }
	   while (pDesc == NULL && pElem != NULL);
#else  /* __COLPAGE__ */
	   do
	      if (pElem->ElAbstractBox[viewNb - 1] != NULL)
		 /* l'element a un pave dans la vue, on a trouve' */
		 pDesc = pElem;
	      else
		 /* l'element n'a pas de pave dans la vue cherche le premier */
		 /* descendant qui ait un pave dans la vue */
		{
		   pDesc = DescVisible (pElem, viewNb, forward);
		   if (pDesc == NULL)
		      /* aucun descendant n'a de pave, on passe a l'element */
		      /* suivant */
		      if (forward)
			 pElem = pElem->ElPrevious;
		      else
			 pElem = pElem->ElNext;
		   /* on arrete quand on a trouve ou quand on a traite' tous les */
		   /* fils de pE. */
		}
	   while (pDesc == NULL && pElem != NULL);
#endif /* __COLPAGE__ */
	}
   return pDesc;
}


/*----------------------------------------------------------------------
   ApplyAttrPresRules applique a` l'element toutes les regles de   
   presentation de l'attribut pAttr.                       
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ApplyAttrPresRules (PtrSSchema pSS, PtrPSchema pSchPres,
			    PtrAttribute pAttr, PtrAbstractBox * pAbbReturn,
				     DocViewNumber viewNb, PtrDocument pDoc,
			       PtrElement pEl, boolean forward, int *lqueue,
					PtrPRule queuePR[MAX_QUEUE_LEN],
				      PtrAbstractBox queuePP[MAX_QUEUE_LEN],
					PtrPSchema queuePS[MAX_QUEUE_LEN],
					PtrAttribute queuePA[MAX_QUEUE_LEN],
			      PtrAbstractBox pNewAbbox, boolean inheritRule)
#else  /* __STDC__ */
static void         ApplyAttrPresRules (pSS, pSchPres, pAttr, pAbbReturn, viewNb, pDoc, pEl,
				 forward, lqueue, queuePR, queuePP, queuePS,
					queuePA, pNewAbbox, inheritRule)
PtrSSchema          pSS;
PtrPSchema          pSchPres;
PtrAttribute        pAttr;
PtrAbstractBox     *pAbbReturn;
DocViewNumber       viewNb;
PtrDocument         pDoc;
PtrElement          pEl;
boolean             forward;
int                *lqueue;
PtrPRule            queuePR[MAX_QUEUE_LEN];
PtrAbstractBox      queuePP[MAX_QUEUE_LEN];
PtrPSchema          queuePS[MAX_QUEUE_LEN];
PtrAttribute        queuePA[MAX_QUEUE_LEN];
PtrAbstractBox      pNewAbbox;
boolean             inheritRule;

#endif /* __STDC__ */

{
   PtrPRule            pR, pRuleView1, pRuleToApply;
   int                 view, i;
   PtrPSchema          pSchP;
   PtrHandlePSchema    pHd;
   boolean             apply;

#ifdef __COLPAGE__
   boolean             bool;

#endif /* __COLPAGE__ */

   apply = TRUE;
   /* exceptions pour les attributs d'un tableau */
   if (ThotLocalActions[T_ruleattr] != NULL)
      (*ThotLocalActions[T_ruleattr])
	 (pEl, pAttr, pDoc, &apply);
   if (apply)
     {
	view = AppliedView (pEl, pAttr, pDoc, viewNb);
	/* on cherchera d'abord dans le schema de presentation principal de */
	/* l'attribut */
	pSchP = pAttr->AeAttrSSchema->SsPSchema;
	pHd = NULL;
	/* on examine le schema de presentation principal, puis les schemas */
	/* additionnels */
	while (pSchP != NULL)
	  {
	     pRuleView1 = NULL;
	     /* 1ere regle de presentation pour cette valeur de l'attribut */
	     pR = AttrPresRule (pAttr, pEl, inheritRule, NULL, pSchP);

	     /* boucle sur la liste de regles de presentation associees a cette */
	     /* valeur de l'attribut */
	     while (pR != NULL)
	       {
		  /* verifie si c'est une regle de creation et si oui applique */
		  /* la regle de creation */
		  if (!ApplCrRule (pR, pAttr->AeAttrSSchema, pSchP, pAttr, pAbbReturn,
				viewNb, pDoc, pEl, forward, lqueue, queuePR,
				   queuePP, queuePS, queuePA, pNewAbbox))
		    {
		       /* ce n'est pas une regle de creation, applique la */
		       /* regle si elle concerne la vue */
		       /* ou applique la regle pour la vue 1 si elle existe */
		       pRuleToApply = NULL;

		       if (pR->PrViewNum == 1)
			 {
			    /* la regle pour la vue 1 */
			    if (pR->PrCond == NULL ||
				CondPresentation (pR->PrCond, pEl, pAttr, 1, pSS))
			       /* la condition d'application est satisfaite */
			      {
				 /* On la conserve au cas ou on ne trouve pas mieux */
				 pRuleView1 = pR;
				 if (view == 1)
				    /* on est dans la vue 1. Donc c'est la bone regle */
				    pRuleToApply = pR;
				 else
				   {
				      /* on cherche s'il existe une regle de meme type pour la */
				      /* vue view, dont les conditions d'application soient */
				      /* satisfaites */
				      while (pR->PrNextPRule && pR->PrNextPRule->PrType ==
					     pRuleView1->PrType)
					{
					   /* la boucle parcourt toutes les regles de meme type */
					   pR = pR->PrNextPRule;
					   if (pR->PrViewNum == view)
					      if (pR->PrCond == NULL ||
						  CondPresentation (pR->PrCond, pEl, pAttr, view, pSS))
						 pRuleToApply = pR;
					}
				      if (pRuleToApply == NULL)
					 /* il n'y a pas de regle specifique pour la vue view. */
					 /* on prend la vue 1 */
					 pRuleToApply = pRuleView1;
				   }
			      }
			 }
		       else
			  /* ce n'est pas une regle pour la vue 1 */
			  /* cette regle ne s'applique que si le numero de vue correspond */
		       if (view == pR->PrViewNum)
			  if (pR->PrCond == NULL ||
			      CondPresentation (pR->PrCond, pEl, pAttr, view, pSS))
			     pRuleToApply = pR;

		       if (pRuleToApply && DoesViewExist (pEl, pDoc, viewNb))
			 {
			    /* Desapplique la regle associee au type de l'element */
			    if (pRuleToApply->PrType == PtVertPos)
			      {
				 pNewAbbox->AbVertPos.PosEdge = Top;
				 pNewAbbox->AbVertPos.PosRefEdge = Top;
				 pNewAbbox->AbVertPos.PosDistance = 0;
				 pNewAbbox->AbVertPos.PosUnit = UnRelative;
				 pNewAbbox->AbVertPos.PosAbRef = NULL;
				 pNewAbbox->AbVertPos.PosUserSpecified = FALSE;
			      }
			    if (pRuleToApply->PrType == PtHorizPos)
			      {
				 pNewAbbox->AbHorizPos.PosEdge = Left;
				 pNewAbbox->AbHorizPos.PosRefEdge = Left;
				 pNewAbbox->AbHorizPos.PosDistance = 0;
				 pNewAbbox->AbHorizPos.PosUnit = UnRelative;
				 pNewAbbox->AbHorizPos.PosAbRef = NULL;
				 pNewAbbox->AbHorizPos.PosUserSpecified = FALSE;
			      }
			    /* retire la regle associee au type de */
			    /* l'element si elle est en attente */
			    for (i = 1; i <= *lqueue; i++)
			       if (queuePR[i - 1] != NULL)
				  if (queuePR[i - 1]->PrType == pRuleToApply->PrType)
				     if (queuePP[i - 1] == pNewAbbox)
					queuePR[i - 1] = NULL;
			    /* applique la regle */
#ifdef __COLPAGE__
			    if (!ApplyRule (pRuleToApply, pSchP, pNewAbbox, pDoc, pAttr,
					    &bool))
#else  /* __COLPAGE__ */
			    if (!ApplyRule (pRuleToApply, pSchP, pNewAbbox, pDoc, pAttr))
#endif /* __COLPAGE__ */

			       /* la regle n'a pas pu etre appliquee, on l'appliquera */
			       /* quand les paves de la descendance auront ete crees. */
			       WaitingRule (pRuleToApply, pNewAbbox, pSchP, pAttr, queuePA,
					 queuePS, queuePP, queuePR, lqueue);
			 }
		    }
		  pR = pR->PrNextPRule;
	       }
	     if (pHd == NULL)
	       {
		  /* on n'a pas encore traite' les schemas de presentation additionnels.
		     On prend le premier schema additionnel si on travaille pour la vue
		     principale, sinon on ignore les schemas additionnels. */
		  if (pDoc->DocView[viewNb - 1].DvPSchemaView == 1)
		     pHd = pAttr->AeAttrSSchema->SsFirstPSchemaExtens;
	       }
	     else
		/* passe au schema additionnel suivant */
		pHd = pHd->HdNextPSchema;
	     if (pHd == NULL)
		/* il n'y a pas (ou plus) de schemas additionnels a prendre en compte */
		pSchP = NULL;
	     else
		pSchP = pHd->HdPSchema;
	  }
     }
}


/*----------------------------------------------------------------------
   ApplyVisibRuleAttr modifie le parametre vis selon la regle de   
   visibilite de pAttr.                                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ApplyVisibRuleAttr (PtrElement pEl, PtrAttribute pAttr,
			   PtrDocument pDoc, int *vis, DocViewNumber viewNb,
					boolean * ok, boolean inheritRule)
#else  /* __STDC__ */
static void         ApplyVisibRuleAttr (pEl, pAttr, pDoc, vis, viewNb, ok, inheritRule)
PtrElement          pEl;
PtrAttribute        pAttr;
PtrDocument         pDoc;
int                *vis;
DocViewNumber       viewNb;
boolean            *ok;
boolean             inheritRule;

#endif /* __STDC__ */

{
   PtrPRule            pR, pRuleView1;
   int                 view;
   boolean             stop, useView1;
   PtrPSchema          pSchP;
   PtrHandlePSchema    pHd;
   TypeUnit            unit;

   /* on cherchera d'abord dans le schema de presentation principal de */
   /* l'attribut */
   pSchP = pAttr->AeAttrSSchema->SsPSchema;
   pHd = NULL;
   /* on examine le schema de presentation principal, puis les schemas */
   /* additionnels */
   while (pSchP != NULL)
     {
	/* cherche la premiere regle de presentation pour cette valeur */
	/* de l'attribut, dans ce schema de presentation */
	pR = AttrPresRule (pAttr, pEl, inheritRule, NULL, pSchP);
	pRuleView1 = NULL;
	if (pR != NULL)
	   if (pR->PrType == PtVisibility)
	      /* cette valeur d'attribut a une regle de visibilite' */
	      /* calcule le numero de la vue concernee par l'attribut */
	     {
		view = AppliedView (pEl, pAttr, pDoc, viewNb);
		stop = FALSE;
		useView1 = TRUE;

		/* cherche s'il y a une regle de visibilite pour la vue */
		while (!stop)
		  {
		     if (pR->PrViewNum == 1)
			if (pR->PrCond == NULL ||
			    CondPresentation (pR->PrCond, pEl, pAttr, 1,
					      pAttr->AeAttrSSchema))
			  {
			     pRuleView1 = pR;
			     if (view == 1)
				stop = TRUE;
			     else
				/* saute les regles de visibilite' suivantes de la vue 1 */
				while (pR->PrNextPRule != NULL &&
				  pR->PrNextPRule->PrType == PtVisibility &&
				       pR->PrNextPRule->PrViewNum == 1)
				   pR = pR->PrNextPRule;
			  }
		     if (!stop)
			if (pR->PrViewNum == view &&
			    CondPresentation (pR->PrCond, pEl, pAttr, view,
					      pAttr->AeAttrSSchema))
			  {
			     /* regle trouvee, on l'evalue */
			     *vis = IntegerRule (pR, pEl, viewNb, ok, &unit, pAttr);
			     useView1 = FALSE;
			     stop = TRUE;
			  }
			else if (pR->PrNextPRule == NULL)
			   stop = TRUE;
			else
			  {
			     pR = pR->PrNextPRule;
			     if (pR->PrType != PtVisibility)
				stop = TRUE;
			  }
		  }

		if (useView1 && pRuleView1 != NULL)
		   /* on n'a pas trouve de regle specifique pour la vue view */
		   /* On utilise la regle de visibilite de la vue 1 si elle existe */
		   *vis = IntegerRule (pRuleView1, pEl, viewNb, ok, &unit, pAttr);
	     }
	if (pHd == NULL)
	  {
	     /* on n'a pas encore traite' les schemas de presentation additionnels.
	        On prend le premier schema additionnel si on travaille pour la vue
	        principale, sinon on ignore les schemas additionnels. */
	     if (pDoc->DocView[viewNb - 1].DvPSchemaView == 1)
		pHd = pAttr->AeAttrSSchema->SsFirstPSchemaExtens;
	  }
	else
	   /* passe au schema additionnel suivant */
	   pHd = pHd->HdNextPSchema;
	if (pHd == NULL)
	   /* il n'y a pas (ou plus) de schemas additionnels a prendre en compte */
	   pSchP = NULL;
	else
	   pSchP = pHd->HdPSchema;
     }
}

/*----------------------------------------------------------------------
   ComputeVisib determine la visibilite de l'element a creer en         
   fonction de la regle de visibilite associee au type de     
   l'element ou a ses attributs                               
   pAbbReturnne la visibilite calculee dans vis                   
   Met a jour vis, pRSpec, pRDef, TypeP et pSchPPage          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ComputeVisib (PtrElement pEl, PtrDocument pDoc, DocViewNumber viewNb,
				  int viewSch, PtrPRule * pRSpec,
				  PtrPRule * pRDef, int *vis,
				boolean * ignoreDescent, boolean * complete,
				  int *TypeP, PtrPSchema * pSchPPage)
#else  /* __STDC__ */
static void         ComputeVisib (pEl, pDoc, viewNb, viewSch, pRSpec, pRDef, vis,
				  ignoreDescent, complete, TypeP, pSchPPage)
PtrElement          pEl;
PtrDocument         pDoc;
DocViewNumber       viewNb;
int                 viewSch;
PtrPRule           *pRSpec;
PtrPRule           *pRDef;
int                *vis;
boolean            *ignoreDescent;
boolean            *complete;
int                *TypeP;
PtrPSchema         *pSchPPage;

#endif /* __STDC__ */
{
   int                 view, l;
   PtrPRule            pRule, pRegleV;
   PtrElement          pPrevious, pNext, pElAssociatedPage, pAsc, pElAttr;
   PtrAttribute        pAttr;
   InheritAttrTable   *inheritTable;
   boolean             ok;
   TypeUnit            unit;

#ifdef __COLPAGE__
   int                 NbCol;
   PtrElement          pEl1;

#endif /* __COLPAGE__ */

   pRule = GetRule (pRSpec, pRDef, pEl, NULL, pEl->ElStructSchema);
   /* pointeur sur la 1ere regle a appliquer */

   /* la premiere regle est la regle de visiblite pour la vue 1 */
   *vis = 0;
   /* parcourt toutes les vues definies dans le schema de presentation */
   /* pour trouver la regle de visibilite pour la vue traitee */
   for (view = 1; view <= MAX_VIEW; view++)
     {
	/* Cherche la regle de visibilite a appliquer */
	if (view == 1)
	   pRegleV = NULL;
	else
	   pRegleV = GetRuleView (pRSpec, pRDef, PtVisibility, view, pEl, NULL,
				  pEl->ElStructSchema);
	if (view == viewSch && DoesViewExist (pEl, pDoc, viewNb))
	   /* s'il y a une regle de visibilite pour cette vue, on */
	   /* la prend */
	   if (pRegleV != NULL)
	      *vis = IntegerRule (pRegleV, pEl, viewNb, &ok, &unit, NULL);
	/* sinon, on prend celle de la vue 1 */
	   else
	      *vis = IntegerRule (pRule, pEl, viewNb, &ok, &unit, NULL);
     }

   /* cherche si les attributs herites par l'element modifient la */
   /* visibilite */
   if (pEl->ElStructSchema->SsPSchema->PsNInheritedAttrs[pEl->ElTypeNumber - 1])
     {
	/* il y a heritage possible */
	if ((inheritTable = pEl->ElStructSchema->SsPSchema->
	     PsInheritedAttr[pEl->ElTypeNumber - 1]) == NULL)
	  {
	     /* cette table n'existe pas on la genere */
	     CreateInheritedAttrTable (pEl);
	     inheritTable = pEl->ElStructSchema->SsPSchema->
		PsInheritedAttr[pEl->ElTypeNumber - 1];
	  }
	for (l = 1; l <= pEl->ElStructSchema->SsNAttributes; l++)
	   if ((*inheritTable)[l - 1])	/* pEl herite de l'attribut l */
	      if ((pAttr = GetTypedAttrAncestor (pEl, l, pEl->ElStructSchema,
						 &pElAttr)) != NULL)
		 /* cherche si l existe au dessus */
		 ApplyVisibRuleAttr (pEl, pAttr, pDoc, vis, viewNb, &ok, TRUE);
     }

   /* cherche si les attributs de l'element modifient la visibilite */
   pAttr = pEl->ElFirstAttr;
   while (pAttr != NULL)
      /* boucle sur les attributs de l'element */
     {
	ApplyVisibRuleAttr (pEl, pAttr, pDoc, vis, viewNb, &ok, FALSE);
	pAttr = pAttr->AeNext;	/* attribut suivant de l'element */
     }

   /* force la visibilite du pave racine si elle n'a pas ete evaluee, */
   /* mais seulement pour l'arbre principal ou la vue 1 des elements */
   /* associes. */
   if (!ok && pEl->ElParent == NULL)
      if (pEl->ElAssocNum == 0 || viewSch == 1)
	 *vis = 10;

   /* Si c'est un element associe', il n'est visible que dans la */
   /* vue principale, meme s'il s'affiche en haut ou bas de page. */
   /* NOTE : seule la vue principale peut afficher des */
   /* elements associes en haut ou bas de page */
   if (pEl->ElAssocNum != 0)
      if (!AssocView (pEl))
	 if (pDoc->DocView[viewNb - 1].DvPSchemaView != 1)
	    *vis = 0;

   /* on ne cree une marque de page que si elle concerne la vue et */
   /* s'il existe des regles de presentation de la page */
   if (pEl->ElTerminal && pEl->ElLeafType == LtPageColBreak)
      /* c'est une marque de page */
      if (pEl->ElViewPSchema != viewSch)
	 /* cette marque ne concerne pas la vue traitee, on ne cree */
	 /* rien */
	{
	   *vis = 0;
	   /* on ne pourra jamais creer de pave' pour cet element dans */
	   /* cette vue */
	   *complete = TRUE;
	}
      else
	 /* cherche le type de boite page ou colonne a creer en  */
	 /* cherchant les regles Page ou Column des elements englobants */
	 /* ou precedants */
	{
#ifdef __COLPAGE__
	   NbCol = 0;
	   /* NbCol indique si pEl est une marque page (=0) */
	   /* ou une marque colonne (>1) */
	   if (pEl->ElPageType == PgBegin
	       || pEl->ElPageType == PgComputed
	       || pEl->ElPageType == PgUser)
	      *TypeP = GetPageBoxType (pEl, viewSch, pSchPPage);
	   else			/* (pEl->ElPageType == ColBegin */
	      /*  || pEl->ElPageType == ColComputed */
	      /* || pPage->ElPageType == ColGroup */
	      /*  || pEl->ElPageType == ColUser) */
	      *TypeP = TypeBCol (pEl, viewSch, pSchPPage, &NbCol);
#else  /* __COLPAGE__ */
	   *TypeP = GetPageBoxType (pEl, viewSch, pSchPPage);
#endif /* __COLPAGE__ */
	   if (*TypeP == 0)
	      /* pas de page definie, on ne cree rien */
	      *vis = 0;
	   else if (*vis <= 0)
#ifdef __COLPAGE__
	      *vis = 10;
#else  /* __COLPAGE__ */
	      *vis = 1;
#endif /* __COLPAGE__ */
	}

   /* si la vue n'affiche qu'un sous-arbre, l'element n'est visible */
   /* que s'il est dans le sous-arbre en question ou sur le chemin */
   /* entre la racine et le sous-arbre. */
   if (pEl->ElAssocNum > 0)
     {				/* element associe' */
	if (pDoc->DocAssocSubTree[pEl->ElAssocNum - 1] != NULL)
	  {
	     /* on n'affiche qu'un sous-arbre de ces elements associes */
	     if (!ElemIsAnAncestor (pDoc->DocAssocSubTree[pEl->ElAssocNum - 1], pEl) &&
		 pDoc->DocAssocSubTree[pEl->ElAssocNum - 1] != pEl)
		/* l'elem. traite' n'est pas dans le sous-arbre affichable */
		if (!ElemIsAnAncestor (pEl, pDoc->DocAssocSubTree[pEl->ElAssocNum - 1]))
		   /* il n'englobe pas le sous-arbre affichable */
		   *vis = 0;	/* on ne cree pas son pave' */
	  }
     }
   else
     {				/* vue de l'arbre principal */
	if (pDoc->DocViewSubTree[viewNb - 1] != NULL)
	  {			/* on n'affiche qu'un sous-arbre dans cette vue */
	     if (!ElemIsAnAncestor (pDoc->DocViewSubTree[viewNb - 1], pEl) &&
		 pDoc->DocViewSubTree[viewNb - 1] != pEl)
		/* l'elem. traite' n'est pas dans le sous-arbre affichable */
		if (!ElemIsAnAncestor (pEl, pDoc->DocViewSubTree[viewNb - 1]))
		   /* il n'englobe pas le sous-arbre affichable */
		   *vis = 0;	/* on ne cree pas son pave' */
	  }
     }

   /* si l'element est une copie, qu'il porte une exception
      ExcPageBreakRepetition ou ExcPageBreakRepBefore
      qu'il est associe a un saut de page et que ce saut 
      de page n'appartient pas a la vue concernee, alors
      on ne cree pas son pave. */
   if (pEl->ElSource)
     {
	pElAssociatedPage = NULL;
	if (TypeHasException (ExcPageBreakRepBefore, pEl->ElTypeNumber,
			      pEl->ElStructSchema))
	  {
	     pNext = pEl->ElNext;
	     while (pNext)
	       {
		  if (pNext->ElTypeNumber == PageBreak + 1)
		    {
		       pElAssociatedPage = pNext;
		       break;
		    }
		  else if (pNext->ElSource)
		     pNext = pNext->ElNext;
		  else
		     break;
	       }
	  }

	else if (TypeHasException (ExcPageBreakRepetition, pEl->ElTypeNumber,
				   pEl->ElStructSchema))
	  {
	     pPrevious = pEl->ElPrevious;
	     while (pPrevious)
	       {
		  if (pPrevious->ElTypeNumber == PageBreak + 1)
		    {
		       pElAssociatedPage = pPrevious;
		       break;
		    }
		  else if (pPrevious->ElSource)
		     pPrevious = pPrevious->ElPrevious;
		  else
		     break;
	       }
	  }
	if (pElAssociatedPage)
	   /* La copie est associee a un saut de page */
	   if (pElAssociatedPage->ElViewPSchema != viewSch)
	     {
		/* cette marque ne concerne pas la vue traitee, */
		/* on ne cree rien */
		*ignoreDescent = TRUE;
		*vis = 0;
	     }
	   else
	     {
		/* cherche le type de boite page a creer en cherchant */
		/* les regles Page des elements englobants */
#ifdef __COLPAGE__
		pEl1 = pElAssociatedPage;
		NbCol = 0;
		/* NbCol indique si pEl est une marque page (=0) */
		/* ou une marque colonne (>1) */
		if (pEl1->ElPageType == PgBegin
		    || pEl1->ElPageType == PgComputed
		    || pEl1->ElPageType == PgUser)
		   *TypeP = GetPageBoxType (pEl1, viewSch, pSchPPage);
		else		/* (pEl1->ElPageType == ColBegin */
		   /*  || pEl1->ElPageType == ColComputed */
		   /*  || pPage->ElPageType == ColGroup */
		   /*  || pEl1->ElPageType == ColUser) */
		   *TypeP = TypeBCol (pEl1, viewSch, pSchPPage, &NbCol);
#else  /* __COLPAGE__ */
		*TypeP = GetPageBoxType (pElAssociatedPage, viewSch, pSchPPage);
#endif /* __COLPAGE__ */
		if (*TypeP == 0)
		  {
		     /* pas de page definie, on ne cree rien */
		     *ignoreDescent = TRUE;
		     *vis = 0;
		  }
	     }
     }
   /* si un element ascendant est rendu invisible, on annule la */
   /* visibilite' de l'element */
   pAsc = pEl;
   while (pAsc != NULL)
      if (pAsc->ElAccess == AccessHidden)
	{
	   *vis = 0;
	   pAsc = NULL;
	}
      else
	 pAsc = pAsc->ElParent;
}


/*----------------------------------------------------------------------
   ApplPresRules   applique les regles de presentation au pave cree  
   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ApplPresRules (PtrElement pEl, PtrDocument pDoc, DocViewNumber viewNb,
			    int viewSch, PtrSSchema pSchS, PtrPSchema pSchP,
				   PtrPRule * pRSpec, PtrPRule * pRDef,
		  PtrAbstractBox * pAbbReturn, boolean forward, int *lqueue,
				   PtrPRule queuePR[MAX_QUEUE_LEN],
				   PtrAbstractBox queuePP[MAX_QUEUE_LEN],
				   PtrPSchema queuePS[MAX_QUEUE_LEN],
	      PtrAttribute queuePA[MAX_QUEUE_LEN], PtrAbstractBox pNewAbbox)

#else  /* __STDC__ */
static void         ApplPresRules (pEl, pDoc, viewNb, viewSch, pSchS, pSchP, pRSpec,
		       pRDef, pAbbReturn, forward, lqueue, queuePR, queuePP,
				   queuePS, queuePA, pNewAbbox)
PtrElement          pEl;
PtrDocument         pDoc;
DocViewNumber       viewNb;
int                 viewSch;
PtrSSchema          pSchS;
PtrPSchema          pSchP;
PtrPRule           *pRSpec;
PtrPRule           *pRDef;
PtrAbstractBox     *pAbbReturn;
boolean             forward;
int                *lqueue;
PtrPRule            queuePR[MAX_QUEUE_LEN];
PtrAbstractBox      queuePP[MAX_QUEUE_LEN];
PtrPSchema          queuePS[MAX_QUEUE_LEN];
PtrAttribute        queuePA[MAX_QUEUE_LEN];
PtrAbstractBox      pNewAbbox;

#endif /* __STDC__ */

{
   int                 view, l;
   PtrPRule            pRule, pRuleView, pR;
   PtrAttribute        pAttr;
   PtrElement          pElAttr;
   InheritAttrTable   *inheritTable;
   boolean             stop;
   PtrHandlePSchema    pHd;
   PtrPSchema          pSchPadd;
#ifdef __COLPAGE__
   boolean             bool;
#endif /* __COLPAGE__ */

   pRuleView = NULL;
   do
     {
	pRule = GetRule (pRSpec, pRDef, pEl, NULL, pSchS);
	/* pointeur sur la regle a appliquer pour la vue 1 */
	if (pRule != NULL)
	   /* si c'est une regle de creation, on l'applique */
	   if (!ApplCrRule (pRule, pSchS, pSchP, NULL, pAbbReturn, viewNb,
			    pDoc, pEl, forward, lqueue, queuePR, queuePP,
			    queuePS, queuePA, pNewAbbox))
	      /* ce n'est pas une regle de creation */
	      /* cherche et applique la regle de meme type pour la vue */
	      for (view = 1; view <= MAX_VIEW; view++)
		{
		   if (view == 1)
		      pRuleView = NULL;
		   else
		      pRuleView = GetRuleView (pRSpec, pRDef, pRule->PrType, view, pEl,
					       NULL, pSchS);
		   if (view == viewSch && pNewAbbox != NULL &&
		       DoesViewExist (pEl, pDoc, viewNb))
		     {
			if (pRuleView == NULL)
			   pRuleView = pRule;
#ifdef __COLPAGE__
			if (!ApplyRule (pRuleView, pSchP, pNewAbbox, pDoc, NULL, &bool))
#else  /* __COLPAGE__ */
			if (!ApplyRule (pRuleView, pSchP, pNewAbbox, pDoc, NULL))
#endif /* __COLPAGE__ */
			   WaitingRule (pRuleView, pNewAbbox, pSchP, NULL,
					queuePA, queuePS, queuePP,
					queuePR, lqueue);
		     }
		}
     }
   while (pRule != NULL);

   /* ApplyRule les regles de presentation pour ce type d'element contenues */
   /* dans les schemas de presentation additionnels du document */
   /* On n'applique les schemas additionnels que pour la vue principale d'un */
   /* document */
   if (pNewAbbox != NULL && viewSch == 1)
     {
	pHd = pEl->ElStructSchema->SsFirstPSchemaExtens;
	while (pHd != NULL)
	  {
	     pSchPadd = pHd->HdPSchema;
	     if (pSchPadd != NULL)
	       {
		  /* applique toutes les regles de presentation associees au type de
		     l'element */
		  pRule = pSchPadd->PsElemPRule[pEl->ElTypeNumber - 1];
		  while (pRule != NULL)
		    {
		       if (pRule->PrCond == NULL ||
			   CondPresentation (pRule->PrCond, pEl, NULL, 1, pEl->ElStructSchema))

			  /* les conditions d'application de la regle sont satisfaites, */

#ifdef __COLPAGE__
			  if (!ApplyRule (pRule, pSchPadd, pNewAbbox, pDoc, NULL, &bool))
#else  /* __COLPAGE__ */
			  if (!ApplyRule (pRule, pSchPadd, pNewAbbox, pDoc, NULL))
#endif /* __COLPAGE__ */
			     WaitingRule (pRuleView, pNewAbbox, pSchP, NULL, queuePA, queuePS,
					  queuePP, queuePR, lqueue);
		       pRule = pRule->PrNextPRule;
		    }
	       }
	     pHd = pHd->HdNextPSchema;
	  }
     }

   /* ApplyRule les regles de presentation heritees des attributs  */
   /* poses sur les elements englobants s'il y a heritage, */
   /* alors la table a deja ete calcule precedemment */
   /* on remet l'affectation pour decouper le code */
   inheritTable = pEl->ElStructSchema->SsPSchema->PsInheritedAttr[pEl->ElTypeNumber - 1];
   if (pNewAbbox != NULL)
      if (pEl->ElStructSchema->SsPSchema->PsNInheritedAttrs[pEl->ElTypeNumber - 1])
	 /* il y a heritage possible */
	 for (l = 1; l <= pEl->ElStructSchema->SsNAttributes; l++)
	    if ((*inheritTable)[l - 1])		/* pEl herite de l'attribut l */
	       /* cherche si l'attribut l existe au dessus */
	       if ((pAttr = GetTypedAttrAncestor (pEl, l, pEl->ElStructSchema, &pElAttr)) != NULL)
		  ApplyAttrPresRules (pAttr->AeAttrSSchema,
				      pAttr->AeAttrSSchema->SsPSchema,
				      pAttr, pAbbReturn, viewNb, pDoc, pEl,
				      forward, lqueue, queuePR,
				      queuePP, queuePS, queuePA,
				      pNewAbbox, TRUE);
   /* ApplyRule les regles de presentation des attributs de l'element. */
   pAttr = pEl->ElFirstAttr;	/* 1er attribut de l'element */
   if (pNewAbbox != NULL)
      while (pAttr != NULL)	/* boucle sur les attributs de l'element */
	{
	   ApplyAttrPresRules (pAttr->AeAttrSSchema,
			       pAttr->AeAttrSSchema->SsPSchema,
			       pAttr, pAbbReturn, viewNb, pDoc, pEl,
			       forward, lqueue, queuePR,
			       queuePP, queuePS, queuePA, pNewAbbox, FALSE);
	   pAttr = pAttr->AeNext;
	}

   /* ApplyRule les regles de presentation specifiques associees a cet */
   /* element */
   pR = pEl->ElFirstPRule;
   while (pR != NULL)
      /* applique une regle si elle concerne la vue */
      /* et si ce n'est pas une hauteur de page */
     {
	if (!(pEl->ElTypeNumber == PageBreak + 1 && pR->PrType == PtHeight))
	   if ((pR->PrViewNum == viewSch
		|| pR->PrType == PtPictInfo)
	       && pNewAbbox != NULL
	       && DoesViewExist (pEl, pDoc, viewNb))
	     {
		if (pR->PrSpecifAttr == 0)
		   pAttr = NULL;
		/* cette regle ne depend pas d'un attribut */
		else
		   /* cherche l'attribut dont depend la regle */
		  {
		     pAttr = pEl->ElFirstAttr;
		     stop = FALSE;
		     while (pAttr != NULL && !stop)
			if (pAttr->AeAttrNum == pR->PrSpecifAttr
			    && pAttr->AeAttrSSchema->SsCode ==
			    pR->PrSpecifAttrSSchema->SsCode)
			   stop = TRUE;
			else
			   pAttr = pAttr->AeNext;
		  }
#ifdef __COLPAGE__
		if (!ApplyRule (pR, pSchP, pNewAbbox, pDoc, pAttr, &bool))
#else  /* __COLPAGE__ */
		if (!ApplyRule (pR, pSchP, pNewAbbox, pDoc, pAttr))
#endif /* __COLPAGE__ */
		   WaitingRule (pR, pNewAbbox, pSchP, pAttr, queuePA,
				queuePS, queuePP, queuePR, lqueue);
	     }
	pR = pR->PrNextPRule;
     }
}

#ifdef __COLPAGE__
/*----------------------------------------------------------------------
   	Chaine	chaine et remplit le pave pointe par pAb, correspondant
   		a l'element pointe par pEl dans la vue nv du document	
   		dont le contexte est pointe par pDoc.			
   forward dit si la creation se fait en avant ou non		
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         Chaine (PtrAbstractBox pAb, PtrElement pEl, DocViewNumber nv, int viewSch, PtrDocument pDoc, boolean forward)
#else  /* __STDC__ */
static void         Chaine (pAb, pEl, nv, viewSch, pDoc, forward)
PtrAbstractBox      pAb;
PtrElement          pEl;
DocViewNumber       nv;
int                 viewSch;
PtrDocument         pDoc;
boolean             forward;

#endif /* __STDC__ */
{
   PtrElement          pE;
   PtrAbstractBox      pP;
   boolean             stop;
   PtrAbstractBox      pPa1;
   PtrElement          pEl1;
   PtrAbstractBox      pPage, pPEnglobant;
   boolean             found, peretrouve;

   pPage = NULL;
   pPa1 = pAb;
   pE = pEl;
   pP = NULL;
   peretrouve = FALSE;
   if (forward)
      /* cas simples de chainage si on insere un pave au milieu de l'i.a.  */
     {
	/* si l'element a un frere precedent (non MP) qui a un pave */
	/* et qui ne contient pas d'element MP de cette vue, */
	/* alors le nouveau pave a pour pere le pere du dernier dup precedent */
	if (pEl->ElPrevious != NULL
	    && !((pEl->ElPrevious->ElTypeNumber == PageBreak + 1)
		 && pEl->ElPrevious->ElViewPSchema == viewSch))
	  {
	     pE = pEl->ElPrevious;
	     /* on cherche si pE contient une MP de la vue */
	     /* pour cela on recherche le pave marque page precedent et */
	     /* on regarde si l'element correspondant a pour ascendant pE */
	     pP = RechPavPage (pE, nv, viewSch, FALSE);
	     if (pP != NULL)	/* document pagine */
	       {
		  pEl1 = pP->AbElement;
		  found = FALSE;
		  while (!found && pEl1 != NULL)
		     if (pEl1->ElParent == pE)
			found = TRUE;
		     else
			pEl1 = pEl1->ElParent;
		  if (!found)	/* si la derniere MP n'est pas fils de pE */
		     /* on peut chainer pAb au pave pere de pP */
		    {
		       pP = pE->ElAbstractBox[nv - 1];
		       if (pP != NULL)
			 {
			    while (pP->AbPresentationBox
			       && pP->AbElement == pE && pP->AbNext != NULL)
			       pP = pP->AbNext;
			    /* on va a la fin de la chaine des dupliques */
			    /* de l'element precedent */
			    while (pP->AbNextRepeated != NULL)
			       pP = pP->AbNextRepeated;
			    /* on prend le pere de ce pave : ce sera le pere de */
			    /* de pAb */
			    pP = pP->AbEnclosing;
			    peretrouve = TRUE;
			 }

		       /* dans tous les autres cas, on revient au cas general */
		    }
	       }
	  }
	else
	   /* sinon, si l'element a un frere suivant (non MP) qui a un pave, */
	   /* le pere du pave sera le premier pave du pere du frere */
	   /* de l'element suivant */
	   if (pEl->ElNext != NULL
	       && !((pEl->ElNext->ElTypeNumber == PageBreak + 1)
		    && pEl->ElNext->ElViewPSchema == viewSch))
	  {
	     pE = pEl->ElNext;
	     pP = pE->ElAbstractBox[nv - 1];
	     if (pP != NULL)
	       {
		  /* on prend le pere de ce pave : ce sera le pere de pAb */
		  pP = pP->AbEnclosing;
		  peretrouve = TRUE;
	       }
	  }

	/* dans les autres cas (!peretrouve), c'est la panique si */
	/* l'insersion se fait au milieu de l'i.a. ! */
	/* il faut recreer l'image abstraite des pages concernees */
	/* pour l'instant on applique le code general */
     }
   if (!peretrouve)
     {
	/* cherche dans cette vue le premier pave d'un element ascendant */
	pP = NULL;
	pE = pEl;
	while (pE->ElParent != NULL && pP == NULL)
	  {
	     pE = pE->ElParent;
	     pP = pE->ElAbstractBox[nv - 1];
	  }
     }
   if (pP == NULL)
      /* il n'y a pas de pave englobant pAb */
     {
	if (pEl->ElAbstractBox[nv - 1] == NULL)
	   pEl->ElAbstractBox[nv - 1] = pAb;
     }
   else
      /* pAb n'est pas le pave le plus englobant pour cette vue */
      /*  pP: 1er pave de l'element ascendant pour cette vue */
     {
	if (!peretrouve)
	  {
	     /* le premier pave qui n'est pas de presentation est l'englobant */
	     /* sauf si le pere est la racine : dans ce cas, l'englobant est */
	     /* le pave de la marque de page precedente */
	     if ((!AssocView (pEl) && (pE == pDoc->DocRootElement))
		 || (AssocView (pEl)
		     && (pE == pDoc->DocAssocRoot[pEl->ElAssocNum - 1])))
		/* cas ou le pere est la racine */
		/* recherche du pave page ou le pave pAb doit etre inclus */
		pPage = RechPavPage (pEl, nv, viewSch, TRUE);
	     if (pPage == NULL)	/* le pere n'est pas la racine */
		/* ou le document est sans pagination */
	       {
		  /* saute les paves de presentation que l'ascendant a crees par la */
		  /* regle CreateBefore */
		  while (pP->AbPresentationBox && pP->AbElement == pE &&
			 pP->AbNext != NULL)
		     pP = pP->AbNext;
		  if (forward)
		     while (pP->AbNextRepeated != NULL)		/* on se positionne sur le */
			/* dernier pave duplique */
			/* cas en avant */
			pP = pP->AbNextRepeated;
		  /* cas ou l'on fonctionne en arriere : on est sur le bon pave */
	       }
	     else
	       {
		  /* il faut descendre la hierarchie des pages et colonnes */
		  /* pPage est le pave corps de page de plus haut niveau */
		  found = FALSE;
		  while (!found)
		    {
		       if (pPage->AbFirstEnclosed != NULL
		       && pPage->AbFirstEnclosed->AbElement->ElTypeNumber ==
			   PageBreak + 1)
			  pPage = pPage->AbFirstEnclosed;
		       else
			  found = TRUE;
		       if (!found)
			  while (pPage->AbPresentationBox)
			     pPage = pPage->AbNext;
		       /* si forward on se place sur la derniere colonne */
		       if (!found && forward)	/* TODO et cas !forward ??? */
			 {
			    while (pPage->AbNext != NULL)
			       pPage = pPage->AbNext;
			    while (pPage->AbPresentationBox)
			       pPage = pPage->AbPrevious;
			 }
		    }
		  pP = pPage;
	       }
	  }			/* fin du cas ou !peretrouve */
	pPa1->AbEnclosing = pP;
	if (pPa1->AbEnclosing->AbFirstEnclosed == NULL)
	   /* c'est le premier pave englobe */
	  {
	     pPa1->AbEnclosing->AbFirstEnclosed = pAb;
	     if (pEl->ElAbstractBox[nv - 1] == NULL)
		pEl->ElAbstractBox[nv - 1] = pAb;
	     /* 1er pave de l'element */
	  }
	else
	   /* il y a deja des paves englobes */
	   /* cas retire : les paves de corps, haut, bas et filet */
	   /* ne sont pas chaines avec la procedure Chaine */
	   /* test sur marque de page supprime */
	  {
	     if (pEl->ElAbstractBox[nv - 1] != NULL)
		/* cet element a deja des paves dans cette vue */
		/* saute les paves de l'element */
		/* en tenant compte des eventuels paves dupliques de l'element */
	       {
		  pP = pEl->ElAbstractBox[nv - 1];
		  stop = FALSE;
		  do
		     if (pP->AbNext == NULL && pP->AbNextRepeated == NULL)
			stop = TRUE;
		     else if (pP->AbNext != NULL)
			if (pP->AbNext->AbElement != pEl)
			   stop = TRUE;
			else
			   pP = pP->AbNext;
		     else	/* pP->AbNext == NULL et pP->AbNextRepeated != NULL */
			pP = pP->AbNextRepeated;
		  while (!(stop));
		  /* insere le nouveau pave apres */
		  pPa1->AbPrevious = pP;
		  pPa1->AbNext = pP->AbNext;
	       }
	     else
		/* cet element n'a pas encore de paves dans cette vue */
	       {
		  pEl->ElAbstractBox[nv - 1] = pAb;
		  /* 1er pave de l'element */
		  /* cherche l'element precedent ayant un pave dans la vue */
		  /* et qui soit dans le sous-arbre defini par l'element */
		  /* du pave pere */
		  /* on saute des les elements marque de page */
		  /* dans le cas ou le pave pere est un pave corps de page, */
		  /* on ne found jamais d'element precedent (ok) */
		  pE = pEl;
		  do
		     pE = BackSearchVisibleElem (pPa1->AbEnclosing->AbElement, pE, nv);
		  while ((pE != NULL) && (pE->ElTypeNumber == PageBreak + 1));
		  /* verifie si le pave found pour un element precedent */
		  /* est bien inclus dans le meme pave englobant. */
		  /* il faut parcourir la suite des paves de l'element */
		  /* precedent (il peut avoir plusieurs paves dup) */
		  /* Par exemple deux notes de bas de page successives */
		  /* peuvent avoir leurs paves dans des boites de bas de */
		  /* page differentes */
		  /* ce dernier cas n'est plus un cas particulier car les */
		  /* paves relatifs a l'element Notes sont chaines avec dup */
		  if (pE != NULL)
		    {
		       /* pave de l'element precedent */
		       pP = pE->ElAbstractBox[nv - 1];
		       /* on saute les paves de presentation */
		       while (pP->AbPresentationBox)
			  pP = pP->AbNext;
		       found = FALSE;
		       while (!found && pP != NULL)
			 {
			    /* boucle de recherche dans les ascendants */
			    /* pourquoi ne pas rester au niveau du pere ? */
			    pPEnglobant = pP->AbEnclosing;
			    while (pPEnglobant != pPa1->AbEnclosing &&
				   pPEnglobant != NULL)
			       pPEnglobant = pPEnglobant->AbEnclosing;
			    /*  si pPEnglobant=NULL, pas found, on passe au pave */
			    /* duplique suivant de l'element precedent */
			    if (pPEnglobant == NULL)
			       pP = pP->AbNextRepeated;
			    else
			       found = TRUE;
			 }
		       if (!found)
			  /* ils n'ont pas le meme pave englobant, on ne */
			  /* chainera pas le pave au pave de l'element precedent */
			  pE = NULL;
		    }
		  /* cas particulier ou pE = PageBreak Debut supprime */
		  if (pE == NULL)
		     /* pas de pave d'element (hors marque de page) precedent */
		    {
		       pP = pPa1->AbEnclosing->AbFirstEnclosed;
		       if (pP != NULL)
			  /* on saute les paves de presentation de l'englobant (ou */
			  /* de la racine si l'englobant est une marque de page ) */
			  if ((pP->AbElement == pPa1->AbEnclosing->AbElement) ||
			      ((pPa1->AbEnclosing->AbElement->ElTypeNumber
				== PageBreak + 1)
			       && (pP->AbElement
			     == pPa1->AbEnclosing->AbEnclosing->AbElement)))
			     if (TypeCreatedRule (pDoc, pPa1->AbEnclosing, pP) == FnCreateLast)
				/* le pave existant doit etre le dernier, on insere */
				/* le nouveau pave devant lui */
			       {
				  pPa1->AbNext = pP;
				  pPa1->AbEnclosing->AbFirstEnclosed = pAb;
			       }
			     else
				/* on saute les paves crees par une regle Create de */
				/* l'englobant (ou de la racine si marque de page) */
			       {
				  stop = FALSE;
				  do
				     if (pP->AbNext == NULL)
					stop = TRUE;
				     else if (pPa1->AbEnclosing->AbElement->ElTypeNumber ==
					      PageBreak + 1)
					if (pP->AbNext->AbElement !=
					    pPa1->AbEnclosing->AbEnclosing->AbElement)
					   stop = TRUE;
					else
					   pP = pP->AbNext;
				     else if (pP->AbNext->AbElement !=
					      pPa1->AbEnclosing->AbElement)
					stop = TRUE;
				     else if (TypeCreatedRule (pDoc, pPa1->AbEnclosing, pP->AbNext) ==
					      FnCreateLast)
					/* le pave suivant doit etre le dernier */
					stop = TRUE;
				     else
					pP = pP->AbNext;
				  while (!stop);
				  /* on insere le nouveau pave apres */
				  pPa1->AbPrevious = pP;
				  pPa1->AbNext = pP->AbNext;
			       }
			  else
			     /* insere le nouveau pave en tete */
			    {
			       pPa1->AbNext = pP;
			       pPa1->AbEnclosing->AbFirstEnclosed = pAb;
			    }
		    }
		  else
		     /* il y a un pave d'un element precedent */
		     /* qui a le meme englobant */
		     /* on cherche le dernier pave de l'element precedent */
		    {
		       /* pP est le premier pave du precedent qui a le meme pere */
		       /* on insere le nouveau pave apres le dernier pave de pP */
		       /* tout en restant dans le meme sous arbre (meme pere) */
		       /*        pP = pE->ElAbstractBox[nv - 1]; */
		       stop = FALSE;
		       do
			  if (pP->AbNext == NULL)
			     stop = TRUE;
			  else if (pP->AbNext->AbElement != pE)
			     stop = TRUE;
			  else
			     pP = pP->AbNext;
		       while (!(stop));
		       /* insere le nouveau pave apres pP */
		       pPa1->AbPrevious = pP;
		       pPa1->AbNext = pP->AbNext;
		    }
	       }
	     if (pPa1->AbPrevious != NULL)
		pPa1->AbPrevious->AbNext = pAb;
	     if (pPa1->AbNext != NULL)
		pPa1->AbNext->AbPrevious = pAb;
	  }
     }
   if (pEl->ElHolophrast || (pEl->ElTerminal && pEl->ElLeafType != LtPageColBreak))
      /* met le contenu de l'element dans le pave, sauf si c'est un */
      /* element de haut ou de bas de page */
     {
	FillContent (pEl, pAb, pDoc);
	/* ajoute le volume du pave a celui de tous ses englobants */
	if (pPa1->AbVolume > 0)
	  {
	     pP = pPa1->AbEnclosing;
	     while (pP != NULL)
	       {
		  pP->AbVolume += pPa1->AbVolume;
		  pP = pP->AbEnclosing;
	       }
	  }
     }
   else
     {
	pPa1->AbLeafType = LtCompound;
	pPa1->AbVolume = 0;
	pPa1->AbInLine = FALSE;
	pPa1->AbTruncatedHead = TRUE;
	pPa1->AbTruncatedTail = TRUE;
     }
}

#else  /* __COLPAGE__ */

/*----------------------------------------------------------------------
   	Attach	chaine et remplit le pave pointe par pAb, correspondant
   		a l'element pointe par pEl dans la vue nv du document	
   		dont le contexte est pointe par pDoc.			
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         Attach (PtrAbstractBox pAb, PtrElement pEl, DocViewNumber nv, PtrDocument pDoc)

#else  /* __STDC__ */
static void         Attach (pAb, pEl, nv, pDoc)
PtrAbstractBox      pAb;
PtrElement          pEl;
DocViewNumber       nv;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrElement          pE;
   PtrAbstractBox      pP;
   boolean             stop;
   PtrAbstractBox      pPa1;
   PtrElement          pEl1;

   pPa1 = pAb;
   /* cherche dans cette vue le premier element ascendant qui ait un pave' */
   pP = NULL;
   pE = pEl;
   while (pE->ElParent != NULL && pP == NULL)
     {
	pE = pE->ElParent;
	pP = pE->ElAbstractBox[nv - 1];
     }
   if (pP == NULL)
      /* il n'y a pas de pave englobant pAb */
     {
	if (pEl->ElAbstractBox[nv - 1] == NULL)
	   pEl->ElAbstractBox[nv - 1] = pAb;
     }
   else
      /* pAb n'est pas le pave le plus englobant pour cette vue */
      /*  pP: 1er pave de l'element ascendant pour cette vue */
      /* saute les paves de presentation que l'ascendant a crees par la */
      /* regle CreateBefore */
     {
	while (pP->AbPresentationBox && pP->AbElement == pE && pP->AbNext != NULL)
	   pP = pP->AbNext;
	/* le premier pave qui n'est pas de presentation est l'englobant */
	pPa1->AbEnclosing = pP;
	if (pPa1->AbEnclosing->AbFirstEnclosed == NULL)
	   /* c'est le premier pave englobe */
	  {
	     pPa1->AbEnclosing->AbFirstEnclosed = pAb;
	     if (pEl->ElAbstractBox[nv - 1] == NULL)
		pEl->ElAbstractBox[nv - 1] = pAb;
	     /* 1er pave de l'element */
	  }
	else
	   /* il y a deja des paves englobes */
	   if (pEl->ElTypeNumber == PageBreak + 1
	       && pEl->ElPageType == PgBegin)
	   /* c'est une marque de page de debut d'element, on la chaine */
	   /* en tete */
	  {
	     pPa1->AbNext = pPa1->AbEnclosing->AbFirstEnclosed;
	     if (pPa1->AbNext != NULL)
		pPa1->AbNext->AbPrevious = pAb;
	     pPa1->AbEnclosing->AbFirstEnclosed = pAb;
	  }
	else
	  {
	     if (pEl->ElAbstractBox[nv - 1] != NULL)
		/* cet element a deja des paves dans cette vue */
		/* saute les paves de l'element */
	       {
		  pP = pEl->ElAbstractBox[nv - 1];
		  stop = FALSE;
		  do
		     if (pP->AbNext == NULL)
			stop = TRUE;
		     else if (pP->AbNext->AbElement != pEl)
			stop = TRUE;
		     else
			pP = pP->AbNext;
		  while (!(stop));
		  /* insere le nouveau pave apres */
		  pPa1->AbPrevious = pP;
		  pPa1->AbNext = pP->AbNext;
	       }
	     else
		/* cet element n'a pas encore de paves dans cette vue */
	       {
		  pEl->ElAbstractBox[nv - 1] = pAb;
		  /* 1er pave de l'element */
		  /* cherche l'element precedent ayant un pave dans la vue */
		  pE = BackSearchVisibleElem (pPa1->AbEnclosing->AbElement, pEl, nv);
		  if (pE != NULL)
		     /* verifie si le pave found pour un element precedent */
		     /* est bien inclus dans le meme pave englobant. */
		     /* Par exemple deux notes de bas de page successives */
		     /* peuvent avoir leurs paves dans des boites de bas de */
		     /* page differentes */
		    {
		       pP = pE->ElAbstractBox[nv - 1];
		       /* pave de l'element precedent */
		       do
			  pP = pP->AbEnclosing;
		       while (!(pP == pPa1->AbEnclosing || pP == NULL));
		       if (pP == NULL)
			  /* ils n'ont pas le meme pave englobant, on ne */
			  /* chainera pas le pave au pave de l'element precedent */
			  pE = NULL;
		    }
		  if (pE != NULL)
		     if (pE->ElTypeNumber == PageBreak + 1
			 && pE->ElPageType == PgBegin)
			/* le precedent est une marque de page de debut */
			/* d'element, on verifie si elle est suivie par des */
			/* paves de presentation de l'englobant */
		       {
			  pP = pE->ElAbstractBox[nv - 1];
			  /* pave de l'element precedent */
			  if (pP->AbNext != NULL)
			     if (pP->AbNext->AbElement ==
				 pPa1->AbEnclosing->AbElement)
				/* la marque de page est suivie par un pave cree */
				/* par l'englobant */
				pE = NULL;
		       }
		  if (pE == NULL)
		     /* pas de pave d'element precedent */
		    {
		       pP = pPa1->AbEnclosing->AbFirstEnclosed;
		       /* saute les eventuelles marques de page de debut */
		       /* d'element */
		       stop = FALSE;
		       do
			  if (pP == NULL)
			     stop = TRUE;
			  else
			    {
			       pEl1 = pP->AbElement;
			       if (pEl1->ElTypeNumber == PageBreak + 1 &&
				   pEl1->ElPageType == PgBegin)
				  pP = pP->AbNext;
			       else
				  stop = TRUE;
			    }
		       while (!stop);
		       if (pP != NULL)
			  if (pP->AbElement == pPa1->AbEnclosing->AbElement)
			     if (TypeCreatedRule (pDoc, pPa1->AbEnclosing, pP) == FnCreateLast)
				/* le pave existant doit etre le dernier, on insere */
				/* le nouveau pave devant lui */
			       {
				  pPa1->AbNext = pP;
				  pPa1->AbEnclosing->AbFirstEnclosed = pAb;
			       }
			     else
				/* on saute les paves crees par une regle */
				/* CreateFirst de l'englobant */
			       {
				  stop = FALSE;
				  do
				     if (pP->AbNext == NULL)
					stop = TRUE;
				     else if (pP->AbNext->AbElement !=
					      pPa1->AbEnclosing->AbElement)
					stop = TRUE;
				     else if (TypeCreatedRule (pDoc, pPa1->AbEnclosing, pP->AbNext) ==
					      FnCreateLast)
					/* le pave suivant doit etre le dernier */
					stop = TRUE;
				     else
					pP = pP->AbNext;
				  while (!(stop));
				  /* on insere le nouveau pave apres */
				  pPa1->AbPrevious = pP;
				  pPa1->AbNext = pP->AbNext;
			       }
			  else
			     /* insere le nouveau pave en tete */
			    {
			       pPa1->AbNext = pP;
			       pPa1->AbEnclosing->AbFirstEnclosed = pAb;
			    }
		    }
		  else
		     /* il y a un pave d'un element precedent */
		    {
		       pP = pE->ElAbstractBox[nv - 1];
		       if (pP->AbEnclosing != NULL &&
			   pP->AbEnclosing->AbPresentationBox &&
			   pP->AbEnclosing->AbElement == pE)
			  /* cet element a cree' un pave englobant par la regle */
			  /* FnCreateEnclosing, on se place au niveau de ce pave' */
			  /* englobant */
			  pP = pP->AbEnclosing;
		       else
			 {
			    /* on cherche le dernier pave de l'element precedent */
			    stop = FALSE;
			    do
			       if (pP->AbNext == NULL)
				  stop = TRUE;
			       else if (pP->AbNext->AbElement != pE)
				  stop = TRUE;
			       else
				  pP = pP->AbNext;
			    while (!(stop));
			 }
		       /* insere le nouveau pave apres pP */
		       pPa1->AbPrevious = pP;
		       pPa1->AbNext = pP->AbNext;
		    }
	       }
	     if (pPa1->AbPrevious != NULL)
		pPa1->AbPrevious->AbNext = pAb;
	     if (pPa1->AbNext != NULL)
		pPa1->AbNext->AbPrevious = pAb;
	  }
     }
   if (pEl->ElHolophrast || (pEl->ElTerminal && pEl->ElLeafType != LtPageColBreak))
      /* met le contenu de l'element dans le pave, sauf si c'est un */
      /* element de haut ou de bas de page */
     {
	FillContent (pEl, pAb, pDoc);
	/* ajoute le volume du pave a celui de tous ses englobants */
	if (pPa1->AbVolume > 0)
	  {
	     pP = pPa1->AbEnclosing;
	     while (pP != NULL)
	       {
		  pP->AbVolume += pPa1->AbVolume;
		  pP = pP->AbEnclosing;
	       }
	  }
     }
   else
     {
	pPa1->AbLeafType = LtCompound;
	pPa1->AbVolume = 0;
	pPa1->AbInLine = FALSE;
	pPa1->AbTruncatedHead = TRUE;
	pPa1->AbTruncatedTail = TRUE;
     }
}
#endif /* __COLPAGE__ */




/*----------------------------------------------------------------------
   AbsBoxesCreate cree les paves correspondant au sous-arbre de         
   l'element pEl du document pDoc, uniquement pour la vue  
   viewNb. Certains paves peuvent deja exister; ils ne sont 
   pas touche's par AbsBoxesCreate. Si Desc est faux, seuls les 
   paves de l'element pEl sont crees, sinon, on cree les   
   paves de tout le sous arbre. La creation de paves       
   s'arrete lorsque tous les paves du sous-arbre sont      
   crees ou de`s qu'il n'y a plus de volume libre dans la  
   vue. En ElemIsBefore indique si la creation a lieu vers l'avant
   ou vers l'arriere. pAbbReturnne le dernier (dans l'ordre    
   defini par forward) pave de plus haut niveau cree'.     
   Au retour, complete indique si l'extremite' (indiquee    
   par forward) de l'image abstraite de l'element a pu     
   etre creee ou non.                                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PtrAbstractBox      AbsBoxesCreate (PtrElement pEl, PtrDocument pDoc, DocViewNumber viewNb,
		       boolean forward, boolean descent, boolean *complete)
#else  /* __STDC__ */
PtrAbstractBox      AbsBoxesCreate (pEl, pDoc, viewNb, forward, descent, complete)
PtrElement          pEl;
PtrDocument         pDoc;
DocViewNumber       viewNb;
boolean             forward;
boolean             descent;
boolean            *complete;

#endif /* __STDC__ */

{
   PtrPSchema          pSchP, pSchPPage, pSPres;
   PtrPRule            pRule, pRDef, pRSpec;
   PtrElement          pElChild, pElParent, pAsc;
   PtrAbstractBox      pAbb, pAbbChild, pNewAbbox, pAbbReturn, pAbbPres;
   PtrAbstractBox      pPRP, pAbbParentAssoc;
   PtrAbstractBox      pAb1;
   PtrSSchema          pSchS;
   PtrAttribute        pAttr;
   PtrPRule            queuePR[MAX_QUEUE_LEN];
   PtrAbstractBox      queuePP[MAX_QUEUE_LEN];
   PtrPSchema          queuePS[MAX_QUEUE_LEN];
   PtrAttribute        queuePA[MAX_QUEUE_LEN];
   int                 vis, typePres;
   int                 viewSch;
   int                 index;
   int                 lqueue, pqueue;
   boolean             completeChild;
   boolean             stop, ok, crAbsBox, truncate, notBreakable, ignoreDescent;
   boolean             Creation, ApplyRules, PcFirst, PcLast;
#ifdef __COLPAGE__
   int                 view;
   boolean             bool;
   PtrElement          pElSauv, pElRoot, pEl1, pElRef, pElPage;
   PtrAbstractBox      pAbbRoot, pAb, pAbb1;
   PtrPRule            pRuleView;
#endif /* __COLPAGE__ */

   pAbbReturn = NULL;
   lqueue = 0;
   pqueue = 0;
   /* Abstract boxes of the element are not created */
   *complete = FALSE;
   if (pEl != NULL && pEl->ElStructSchema != NULL)
     {
	viewSch = AppliedView (pEl, NULL, pDoc, viewNb);
	pAbbParentAssoc = NULL;
	/* pointeur sur le pave qui sera cree' pour l'element */
	pNewAbbox = NULL;
	ApplyRules = FALSE;
	ignoreDescent = FALSE;
	pAbbPres = NULL;
	notBreakable = FALSE;
	/* pas tous crees */
	Creation = FALSE;	/* a priori rien a creer */
#ifdef __COLPAGE__
	/* pour memoriser pEl lors de la creation des pages */
	pElSauv = NULL;
	/* arriere */
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
#endif /* __COLPAGE__ */
#ifdef __COLPAGE__
	InitPageCol (&pEl, pDoc, viewNb, viewSch, forward, &Creation, complete, &ApplyRules, &pElSauv);
#else  /* __COLPAGE__ */
	pAb1 = pEl->ElAbstractBox[viewNb - 1];
	if (pAb1 != NULL)
	   /* le pave existe deja pour cette vue */
	  {
	     Creation = FALSE;
	     /* on saute les paves de presentation crees par CreateWith */
	     while (pAb1->AbPresentationBox && pAb1->AbNext != NULL)
		pAb1 = pAb1->AbNext;
	     if (pAb1->AbLeafType != LtCompound || pAb1->AbInLine)
		/* c'est une feuille ou un pave' mis en lignes, il */
		/* a deja tout son contenu */
		*complete = TRUE;
	     else if (forward)
		*complete = !pAb1->AbTruncatedTail;
	     else
		*complete = !pAb1->AbTruncatedHead;
	     if (pAb1->AbSize == -1)
		/* il faut lui appliquer ses regles de presentation */
		ApplyRules = TRUE;
	  }
	else
	   /* le pave n'existe pas pour cette vue, on essaiera de le creer. */
	  {
	     Creation = TRUE;
	     /* on ne sait pas si on pourra creer toute son image */
	     *complete = FALSE;
	  }
#endif /* __COLPAGE__ */
	/* on ne cherche a creer que les paves qui n'existent pas deja */
	if (Creation || ApplyRules)
	   /* cherche le schema de presentation a appliquer */
	  {
	     SearchPresSchema (pEl, &pSchP, &index, &pSchS);
	     /* pRSpec: premiere regle de presentation specifique. */
	     pRSpec = pSchP->PsElemPRule[index - 1];
	     /* premiere regle de presentation par defaut */
	     pRDef = pSchP->PsFirstDefaultPRule;
	     /* TODO code inutile pour la V4 ? */
	     if (pSchP->PsInPageHeaderOrFooter[index - 1])
		/* c'est un element associe a afficher dans une boite */
		/* de haut ou de bas de page */
		/* cherche le pave contenant ces elements associes */
	       {
		  if (pEl->ElParent != NULL)
		     pAbbParentAssoc = pEl->ElParent->ElAbstractBox[viewNb - 1];
		  if (pAbbParentAssoc != NULL)
		     if (pAbbParentAssoc->AbPresentationBox)
			/* Il devient temporairement un pave d'element structure' */
			/* pour appliquer correctement les regles de l'element */
			pAbbParentAssoc->AbPresentationBox = FALSE;
		     else
			/* ca a deja ete fait par CrAbsBoxesPres */
			pAbbParentAssoc = NULL;
	       }
	     /* TODO fin de code inutile pour la V4  */
	     /* initialise la file des regles qui n'ont pas pu etre appliquees */
	     lqueue = 0;
	     pqueue = 0;

	     /* determine la visibilite du pave a creer */
	     ComputeVisib (pEl, pDoc, viewNb, viewSch, &pRSpec, &pRDef, &vis,
			   &ignoreDescent, complete, &typePres, &pSchPPage);

	     /* si l'element est visible dans cette vue ou si c'est la racine, */
	     /* on cree son pave, si ce n'est deja fait */
	     if (vis >= 1 ||
		 (pEl->ElParent == NULL && (pEl->ElAssocNum == 0 || viewSch == 1)))
		if (ApplyRules)
		   /* on applique seulement les regles de presentation a un */
		   /* pave qui existe deja */
		   /* pour appliquer les regles, on initialise pNewAbbox au */
		   /* pave de l'element */
		  {
		     pNewAbbox = pEl->ElAbstractBox[viewNb - 1];
		     pNewAbbox->AbVisibility = vis;
		     /* Si c'est un element racine qui n'a pas de descendants, */
		     /* alors son pave est complete */
		     if (descent)
			if (pEl->ElParent == NULL)
			   if (pEl->ElTerminal || pEl->ElFirstChild == NULL)
			     {
				*complete = TRUE;
				if (pNewAbbox->AbLeafType == LtCompound)
#ifndef __COLPAGE__
				   if (!pNewAbbox->AbInLine)
#endif /* __COLPAGE__ */
				      if (forward)
					 /* on creera au moins le 1er pave inclus */
					 pNewAbbox->AbTruncatedHead = FALSE;
				      else
					 /* on creera au moins le dernier pave inclus */
					 pNewAbbox->AbTruncatedTail = FALSE;
			     }
		  }
		else
		   /* cree et initialise un nouveau pave */
		   /* plusieurs paves sont crees si l'element est une marque */
		   /* de pages : paves corps de page, plus paves dupliques (V4)  */
		  {
		     pNewAbbox = InitAbsBoxes (pEl, viewNb, vis);
		     pNewAbbox->AbPSchema = pSchP;
		     pNewAbbox->AbAcceptLineBreak = pSchP->PsAcceptLineBreak[pEl->ElTypeNumber - 1];
		     pNewAbbox->AbAcceptPageBreak = pSchP->PsAcceptPageBreak[pEl->ElTypeNumber - 1];
		     if (pDoc->DocReadOnly)
		       {
			  /* document en lecture seule */
			  pNewAbbox->AbReadOnly = TRUE;
			  /* on rend les feuilles non modifiables par le Mediateur */
			  if (pEl->ElTerminal)
			     pNewAbbox->AbCanBeModified = FALSE;
		       }
#ifdef __COLPAGE__
		     if (pEl->ElTerminal && pEl->ElLeafType == LtPageColBreak)
		       {
			  if (pEl->ElViewPSchema == viewSch)
			     /* creation des paves de page et duplication  */
			     /* des paves situes dessous les paves PavCorps */
			     CreePageCol (pEl, &pNewAbbox, pDoc, viewNb, viewSch, forward,
			       complete, &lqueue, queuePR, queuePP, queuePS,
			      queuePA, pElSauv, pSchPPage, typePres, &stop);
			  if (stop)
			     /* on teste stop pour savoir si une le pave a ete */
			     /* detruit suite a l'application d'une regle */
			     /* page : il faut sortir de AbsBoxesCreate pour */
			     /* recommencer la creation des fils du pere de pEl */
			     /* car un fils marque page a ete ajoute avant pEl */
			     /* si stop est vrai, la destruction de pNewAbbox a ete faite */
			    {
			       pAbbReturn = NULL;
			       return pAbbReturn;	/* on sort de AbsBoxesCreate */
			       /* TODO : est-ce correct ? y-a-il des regles */
			       /* en attente ou retardees ??? */
			    }
		       }
		     else
			/* chaine le nouveau pave dans l'arbre de l'image abstraite */

			Attach (pNewAbbox, pEl, viewNb, viewSch, pDoc, forward);

		     /* si ce pave modifie la position de paves voisins, on applique */
		     /* les regles correspondantes : appel de ApplyRefAbsBoxNew */
		     /* y compris dans le cas ou pNewAbbox est un pave corps de page */
		     if (pNewAbbox != NULL &&
			 ((pNewAbbox->AbPrevious != NULL &&
			   !pNewAbbox->AbPrevious->AbNew)
			  || (pNewAbbox->AbEnclosing != NULL &&
			      !pNewAbbox->AbEnclosing->AbNew)
			  || (pNewAbbox->AbNext != NULL &&
			      !pNewAbbox->AbNext->AbNew)))
			ApplyRefAbsBoxNew (pNewAbbox, pNewAbbox, &pAbb, pDoc);	/* pAbb inutilise */
		     /* si le pave cree est la racine, on met a jour pAbbRoot */
		     if (pAbbRoot == NULL && pNewAbbox != NULL
			 && pEl->ElParent == NULL)
			pAbbRoot = pNewAbbox;
#else  /* __COLPAGE__ */
		     /* chaine le nouveau pave dans l'arbre de l'image abstraite */
		     Attach (pNewAbbox, pEl, viewNb, pDoc);
#endif /* __COLPAGE__ */
		     pAbbReturn = pNewAbbox;
		     if (descent)	/* on va creer les paves inclus */
		       {
			  pAb1 = pNewAbbox;
			  if (pAb1->AbLeafType == LtCompound)
			     if (!pAb1->AbInLine)
				if (forward)
				   /* on creera au moins le 1er pave inclus */
				   pAb1->AbTruncatedHead = FALSE;
				else
				   /* on creera au moins le dernier pave inclus */
				   pAb1->AbTruncatedTail = FALSE;
			     else
				/* pave mis en ligne, on cree tout */
				*complete = TRUE;
			  else
			     /* pave feuille, on cree tout */
			     *complete = TRUE;
		       }
		  }
	  }
	if (pNewAbbox == NULL && Creation && !ignoreDescent && !pEl->ElHolophrast)
	  {
	     /* cet element n'est pas visible dans la vue, on cherche a creer les */
	     /* paves d'un descendant visible */
	     if (descent)
		if (pEl->ElTerminal)
		   *complete = TRUE;
		else if (pEl->ElFirstChild == NULL)
		   *complete = TRUE;
		else
		   /* cherche d'abord le premier descendant visible dans la vue */
		  {
		     pElChild = DescVisible (pEl, viewNb, forward);
		     if (pElChild == NULL)
			/* pas de descendant visible, on traite les fils */
		       {
			  pElChild = pEl->ElFirstChild;
			  /* premier fils de l'element */
			  /* cherche le dernier fils si la creation a lieu en arriere */
			  if (!forward && pElChild != NULL)
			     while (pElChild->ElNext != NULL)
				pElChild = pElChild->ElNext;
		       }
		     if (pElChild != NULL)
			/* on cherche si un ascendant a un pave insecable dans la vue */
		       {
			  notBreakable = FALSE;		/* a priori, non */
			  pAsc = pEl;
			  pAbb = NULL;
			  while (pAsc != NULL && pAbb == NULL)
			    {
			       pAbb = pAsc->ElAbstractBox[viewNb - 1];
			       /* on cherche le pave principal de cet ascendant dans la vue */
			       stop = FALSE;
			       do
				  if (pAbb == NULL)
				     stop = TRUE;
				  else if (pAbb->AbPresentationBox)
				     pAbb = pAbb->AbNext;
				  else
				     stop = TRUE;
			       while (!stop);
			       if (pAbb != NULL)
				  /* cet ascendant a un pave, est-il secable ? */
				  notBreakable = !(IsBreakable (pAbb));
			       else
				  /* pas de pave, on passe a l'ascendant du dessus */
				  pAsc = pAsc->ElParent;
			    }
		       }
		     while (pElChild != NULL)
			/* cree les paves d'un descendant */
		       {
			  pAbb = AbsBoxesCreate (pElChild, pDoc, viewNb, forward, descent, &completeChild);
			  if (pAbb != NULL)
			     pAbbReturn = pAbb;
			  /* passe au fils suivant ou precedent, selon le sens de */
			  /* creation */
			  if (forward)
			     pElChild = pElChild->ElNext;
			  else
			     pElChild = pElChild->ElPrevious;
			  if (pElChild == NULL)
			     /* on a cree' les paves du dernier fils */
			     *complete = completeChild;
			  else
			     /* verifie que la vue n'est pas pleine, mais si un */
			     /* ascendant est insecable, on continue quand meme */
			  if (IsViewFull (viewNb, pDoc, pEl) && !notBreakable)
			     /* vue pleine, on arrete la creation des paves des fils */
			     pElChild = NULL;
		       }
		  }
	  }			/* fin if (descent) */
	else if (!ignoreDescent)
	   /* l'element est visible dans la vue */
	  {
	     if ((Creation || ApplyRules) && descent)
	       {
#ifdef __COLPAGE__
		  if (!(pEl->ElTerminal && pEl->ElLeafType == LtPageColBreak))
		     /* dans la V4 les regles de la page ont deja */
		     /* ete appliquees apres duplication dans CreePageCol */
#endif /* __COLPAGE__ */
		     ApplPresRules (pEl, pDoc, viewNb, viewSch, pSchS,
			       pSchP, &pRSpec, &pRDef, &pAbbReturn, forward,
		     &lqueue, queuePR, queuePP, queuePS, queuePA, pNewAbbox);

		  /* traitement particulier aux sauts de page (il faut prendre */
		  /* le bon schema de presentation) */
		  /* uniquement pour la V3 car dans la V4 les regles ont deja */
		  /* ete appliquees apres duplication dans CreePageCol */
#ifndef __COLPAGE__
		  if (pEl->ElTerminal && pEl->ElLeafType == LtPageColBreak &&
		      !pEl->ElHolophrast)
		     /* c'est une marque de page */
		     if (pEl->ElViewPSchema == viewSch)
			/* cette marque concerne la vue traitee */
			ApplPage (pEl, pDoc, viewNb, viewSch, typePres, pSchPPage,
				  pNewAbbox);
#endif /* __COLPAGE__ */
	       }
	     if (pEl->ElTerminal)
		*complete = TRUE;
	     if (descent && !pEl->ElTerminal && !pEl->ElHolophrast)
		/* cree les paves des fils de l'element, dans la limite de la */
		/* capacite de la vue */
		/* determine d'abord si l'element est secable et pour cela */
		/* cherche le 1er pave de l'element qui ne soit pas un pave de */
		/* presentation */
	       {
		  pAbbPres = NULL;
		  pAbb = pEl->ElAbstractBox[viewNb - 1];
		  stop = FALSE;
		  do
		     if (pAbb == NULL)
			stop = TRUE;
		     else if (pAbb->AbPresentationBox)
			pAbb = pAbb->AbNext;
		     else
			stop = TRUE;
		  while (!stop);
		  notBreakable = !(IsBreakable (pAbb));
		  /* determine le 1er pave fils a creer */
		  pElChild = pEl->ElFirstChild;		/* premier fils de l'element */
		  if (pElChild == NULL)
		     /* le pave n'a pas de descendance */
		     /* pave vide, il n'est pas coupe' */
		    {
		       *complete = TRUE;
		       pAbbPres = TruncateOrCompleteAbsBox (pNewAbbox, FALSE, !forward, pDoc);
		       if (pAbbPres != NULL)
			  /* on a cree des paves de presentation */
			  if (pAbbPres->AbEnclosing != pNewAbbox)
			     pAbbReturn = pAbbPres;
		    }
		  else if (Creation || notBreakable)
		     /* il faut creer les paves de tous les fils */
		     /* creation en avant: on commence par le 1er fils */
		     /* creation en reculant: commence par le dernier fils */
		    {
		       if (!forward)
			 {
			    stop = FALSE;
			    do
			       if (pElChild == NULL)
				  stop = TRUE;
			       else if (pElChild->ElNext == NULL)
				  stop = TRUE;
			       else
				  pElChild = pElChild->ElNext;
			    while (!stop);
			 }
		    }
		  else
		     /* il y a peut-etre deja un pave fils */
		     /* on ne cree des paves qu'a partir de l'element descendant */
		     /* qui possede deja un pave dans la vue */
		     /* cherche le premier descendant qui ait un pave dans la vue */
		    {
		       pElChild = DescVisible (pEl, viewNb, forward);
		       if (pElChild == NULL)
			  /* aucun descendant n'a de pave dans la vue */
			  /* on cree les paves de tous les fils */
			 {
			    pElChild = pEl->ElFirstChild;
			    /* premier fils de l'element */
			    if (!forward)
			       /* creation en reculant ; cherche le dernier fils */
			      {
				 stop = FALSE;
				 do
				    if (pElChild == NULL)
				       stop = TRUE;
				    else if (pElChild->ElNext == NULL)
				       stop = TRUE;
				    else
				       pElChild = pElChild->ElNext;
				 while (!stop);
			      }
			 }
		    }
		  /* cree les paves des fils successifs */
		  while (pElChild != NULL)
		     /* verifie que la vue n'est pas pleine */
#ifdef __COLPAGE__
		    {
		       PagePleine (viewNb, pDoc, pElChild, forward);
		       /* on arrete la creation des paves si la vue et pleine */
		       /* ET si la page en cours est pleine : c'est a dire */
		       /* si l'element suivant a creer est une marque de page */
		       /* test fait dans PagePleine : on lui passe pElChild */
		       /* on appelle PagePleine pour positionner le volume */
		       /* a -1 si la page en cours de creation est pleine */
		       /* PagePleine detruit eventuellement des paves vides */
#endif /* __COLPAGE__ */
		       if (IsViewFull (viewNb, pDoc, pEl) && !notBreakable)
			  /* vue pleine, on arrete la creation des paves des fils */
			 {
			    pElChild = NULL;
			    /* le pave englobant des fils est incomplet */
			    pElParent = pEl;
			    /* cherche le 1er ascendant visible dans la vue */
			    stop = FALSE;
			    do
			       if (pElParent == NULL)
				  stop = TRUE;
			       else if (pElParent->ElAbstractBox[viewNb - 1] != NULL)
				  stop = TRUE;
			       else
				  pElParent = pElParent->ElParent;
			    while (!stop);
			    /* marque ce pave coupe' */
			    if (pElParent != NULL)
#ifdef __COLPAGE__
			      {
				 /* la recherche se fait par l'arbre abstrait, il faut */
				 /* considerer la chaine des dupliques pour trouver */
				 /* le pave a couper */
				 pAbb = pElParent->ElAbstractBox[viewNb - 1];
				 /* on saute les eventuels paves de presentation */
				 while (pAbb->AbPresentationBox)
				    pAbb = pAbb->AbNext;
				 if (forward)
				    while (pAbb->AbNextRepeated != NULL)
				       pAbb = pAbb->AbNextRepeated;
				 pAbbPres = TruncateOrCompleteAbsBox (pAbb, TRUE, !forward, pDoc);
			      }
#else  /* __COLPAGE__ */
			       pAbbPres = TruncateOrCompleteAbsBox (pElParent->ElAbstractBox[viewNb - 1], TRUE, !forward, pDoc);
#endif /* __COLPAGE__ */
			 }
		       else
			  /* Cree les paves d'un fils et passe a un autre fils */
			 {
			    ok = TRUE;	/* a priori on va creer les paves du fils */
			    if (!ApplyRules)	/* verifie si le fils est deja complete */
			       if (pElChild->ElAbstractBox[viewNb - 1] != NULL)
				  /* le fils a deja des paves dans cette vue */
				 {
				    pAbbChild = pElChild->ElAbstractBox[viewNb - 1];
#ifdef __COLPAGE__
				    /* on saute les paves de presentation pour tester */
				    /* CT et CQ correctement : en effet le pave  */
				    /* principal peut etre coupe tout en ayant des */
				    /* paves de presentation (si marque Page est premier */
				    /* ou dernier fils. cf. destruction lors de la dup */
				    /* cas des paves de presentation repetes */
				    while (pAbbChild->AbPresentationBox)
				       pAbbChild = pAbbChild->AbNext;
				    if (forward)
				       /* cherche le dernier pave du fils */
				       while (pAbbChild->AbNextRepeated != NULL)
					  pAbbChild = pAbbChild->AbNextRepeated;
				    /* est-ce ce que l'on veut ? */
				    pAb1 = pAbbChild;
				    /* on appellera AbsBoxesCreate s'il faut appliquer les */
				    /* regles de presentation de ce fils */
				    if (pAb1->AbSize != -1
					&& !(!forward && pElChild->ElTypeNumber == PageBreak + 1))
				       /* si !forward, il faut appeler AbsBoxesCreate si */
				       /* l'element est une marque de page */
				       if (pAb1->AbLeafType != LtCompound)
					  /* supprime                  || pAb1->AbInLine) */
					  ok = FALSE;	/* pave fils complete */
				       else if (forward)
					 {
					    if (!pAb1->AbTruncatedTail)
					       ok = FALSE;	/* pave fils complete */
					 }
				       else if (!pAb1->AbTruncatedHead)
					  ok = FALSE;	/* pave fils complete */
#else  /* __COLPAGE__ */
				    /* saute les paves de presentation du fils */
				    stop = FALSE;
				    do
				       if (pAbbChild->AbNext == NULL)
					  stop = TRUE;
				       else if (pAbbChild->AbPresentationBox)
					  pAbbChild = pAbbChild->AbNext;
				       else
					  stop = TRUE;
				    while (!stop);
				    /* on appellera AbsBoxesCreate s'il faut appliquer les */
				    /* regles de presentation de ce fils */
				    if (pAbbChild->AbSize != -1)
				       if (pAbbChild->AbLeafType != LtCompound ||
					   pAbbChild->AbInLine)
					  ok = FALSE;	/* pave fils complete */
				       else if (forward)
					 {
					    if (!pAbbChild->AbTruncatedTail)
					       ok = FALSE;	/* pave fils complete */
					 }
				       else if (!pAbbChild->AbTruncatedHead)
					  ok = FALSE;	/* pave fils complete */
#endif /* __COLPAGE__ */
				 }
			    if (ok)
#ifdef __COLPAGE__
			      {
#endif /* __COLPAGE__ */
				 /* on cree effectivement les paves du fils */
				 pAbbChild = AbsBoxesCreate (pElChild, pDoc, viewNb, forward, descent, &completeChild);
#ifdef __COLPAGE__
				 /* cas particulier ou le fils avait une regle page */
				 /* qui a provoque la creation d'un element marque page */
				 /* avant l'element pElChild : les paves de pElChild ont ete */
				 /* detruits dans la procedure applPage_Applique et ils */
				 /* faut recommencer l'appel de AbsBoxesCreate pour pElChild */
				 /* pour qu'ils soient correctement places sous la nouvelle */
				 /* page */
				 /* (les paves de la nouvelle page ont deja ete crees */
				 /* sauf si la vue etait pleine ) */
				 if (pAbbChild == NULL && pElChild->ElPrevious != NULL)
				   {
				      pEl1 = pElChild->ElPrevious;
				      if (pEl1->ElTerminal
				       && pEl1->ElLeafType == LtPageColBreak
					  && pEl1->ElViewPSchema == viewSch
					  && (pEl1->ElPageType == PgBegin
					   || pEl1->ElPageType == ColBegin))
					{
					   PagePleine (viewNb, pDoc, pElChild, forward);
					   if (IsViewFull (viewNb, pDoc, pElChild))
					      pElChild = NULL;
					   else
					      pAbbChild = AbsBoxesCreate (pElChild, pDoc, viewNb,
									  forward, descent, &completeChild);
					}
				      /* remarque: il se peut que pElChild soit invisible */
				      /* pour cette vue et que pAbbChild soit null a ce */
				      /* retour de procedure */
				   }
			      }
#endif /* __COLPAGE__ */
			    else
			       /* le pave du fils etait deja complete */
			      {
				 pAbbChild = NULL;
				 completeChild = TRUE;
			      }
#ifdef __COLPAGE__
			    if (pElChild != NULL)
			      {
#endif /* __COLPAGE__ */
				 if (pNewAbbox == NULL)
				    if (pAbbChild != NULL)
				       pAbbReturn = pAbbChild;
#ifdef __COLPAGE__
				 /* cas particulier de la pagination : si on appelle */
				 /* AbsBoxesCreate pour un fils non cree a cause du booleen */
				 /* StopBeforeCreation, il ne faut pas executer le code */
				 /* premier ou dernier */
				 if (ok && RunningPaginate && StopBeforeCreation
				  && pElChild->ElTerminal && !completeChild)
				    /* pElChild est soit une marque page soit une reference */
				    /* a un element associe en haut ou bas de page */
				   {
				      PcFirst = FALSE;
				      PcLast = FALSE;
				   }
				 else
#endif /* __COLPAGE__ */
				 if (forward)
				   {
				      PcLast = (pElChild->ElNext == NULL);
				      PcFirst = FALSE;
				   }
				 else
				   {
				      PcFirst = (pElChild->ElPrevious == NULL);
				      PcLast = FALSE;
				   }
				 pAsc = pElChild->ElParent;
				 while (pAsc != pEl)
				   {
				      if (PcFirst)
					 PcFirst = (pAsc->ElPrevious == NULL);
				      if (PcLast)
					 PcLast = (pAsc->ElNext == NULL);
				      pAsc = pAsc->ElParent;
				   }

				 pElParent = pElChild->ElParent;
				 if (forward)
				    if (pElChild->ElNext != NULL)
				       /* passe au fils suivant */
				       pElChild = pElChild->ElNext;
				    else
				       /* pas de fils suivant */
				      {
					 /* on cherche le premier englobant qui ne soit pas */
					 /* l'element traite' et qui ait un voisin */
					 stop = FALSE;
					 do
					    if (pElChild->ElParent == pEl)
					       /* on traitait le pere, on s'arrete */
					      {
						 pElChild = NULL;
						 stop = TRUE;
					      }
					    else
					       /* on traite le frere suivant du pere */
					      {
						 pElChild = pElChild->ElParent;
						 if (pElChild == NULL)
						    stop = TRUE;
						 else if (pElChild->ElNext != NULL)
						   {
						      pElChild = pElChild->ElNext;
						      stop = TRUE;
						   }
					      }
					 while (!stop);
				      }
				 else if (pElChild->ElPrevious != NULL)
				    pElChild = pElChild->ElPrevious;
				 /* passe au fils precedent */
				 else
				    /* pas de fils precedent */
				   {
				      /* on cherche le premier englobant qui ne soit pas */
				      /* l'element traite' et qui ait un voisin */
				      stop = FALSE;
				      do
					 if (pElChild->ElParent == pEl)
					    /* on traitait le pere, on s'arrete */
					   {
					      pElChild = NULL;
					      stop = TRUE;
					   }
					 else
					    /* on traite le frere precedent du pere */
					   {
					      pElChild = pElChild->ElParent;
					      if (pElChild == NULL)
						 stop = TRUE;
					      else if (pElChild->ElPrevious != NULL)
						{
						   pElChild = pElChild->ElPrevious;
						   stop = TRUE;
						}
					   }
				      while (!stop);
				   }
				 if (PcLast || PcFirst)
				    /* on a cree' les paves du premier (ou dernier) fils */
				    /* de pEl, le pave englobant des fils de pEl est  */
				    /* complete si le pave du premier (ou dernier) fils est */
				    /* complete ou s'il ne contient rien (tous ses */
				    /* descendants ont une visibilite' nulle) */
				   {
				      stop = FALSE;
				      do
					 if (pElParent != NULL)
					    if (pElParent->ElAbstractBox[viewNb - 1] != NULL)
					       stop = TRUE;
					    else if ((forward && pElParent->ElNext == NULL)
						     || (!forward
							 && pElParent->ElPrevious == NULL))
					       pElParent = pElParent->ElParent;
					    else
					       pElParent = NULL;
				      while (!stop && pElParent != NULL) ;
				      if (pElParent != NULL)
					{
					   pAbb = pElParent->ElAbstractBox[viewNb - 1];
					   /* saute les paves de presentation crees par */
					   /* FnCreateBefore */
					   stop = FALSE;
					   do
					      if (pAbb == NULL)
						 stop = TRUE;
					      else if (!pAbb->AbPresentationBox)
						 stop = TRUE;
					      else
						 pAbb = pAbb->AbNext;
					   while (!stop);
					   if (pAbb != NULL)
					     {
						pAbbChild = pAbb->AbFirstEnclosed;
						pAbbPres = NULL;
						if (PcLast)
						  {
#ifdef __COLPAGE__
						     /* on se positionne sur le dernier pave */
						     /* de l'element pElParent */
						     while (pAbb->AbNextRepeated != NULL)
							pAbb = pAbb->AbNextRepeated;
						     pAbbChild = pAbb->AbFirstEnclosed;
						     /* cas particulier de la racine: il faut */
						     /* aller sous le dernier corps de page */
						     if (pAbbChild != NULL
							 && pAbbChild->AbElement->ElTypeNumber
							 == PageBreak + 1)
						       {
							  while (pAbbChild->AbNext != NULL)
							     pAbbChild = pAbbChild->AbNext;
							  /* pAbbChild  dernier fils de la racine */
							  while (pAbbChild->AbPresentationBox)
							     pAbbChild = pAbbChild->AbPrevious;
							  /* pAbbChild est le dernier corps de page
							     on descend la hierarchie des colonnes */
							  while (pAbbChild->AbFirstEnclosed != NULL &&
								 pAbbChild->AbFirstEnclosed->AbElement->ElTypeNumber == PageBreak + 1)
							    {
							       pAbbChild = pAbbChild->AbFirstEnclosed;
							       while (pAbbChild->AbNext != NULL)
								  pAbbChild = pAbbChild->AbNext;
							       while (pAbbChild->AbPresentationBox)
								  pAbbChild = pAbbChild->AbPrevious;
							    }
							  pAbbChild = pAbbChild->AbFirstEnclosed;
						       }
#endif /* __COLPAGE__ */
						     if (pAbbChild == NULL)
							/* tous les descendants ont une visibilite' */
							/* nulle; le pave est donc complete en queue */
							truncate = FALSE;
						     else
						       {
							  /* cherche le dernier pave' fils */
							  while (pAbbChild->AbNext != NULL)
							     pAbbChild = pAbbChild->AbNext;
							  /* ignore les paves de presentation */
							  while (pAbbChild->AbPresentationBox &&
								 pAbbChild->AbPrevious != NULL)
							     pAbbChild = pAbbChild->AbPrevious;
							  if (pAbbChild->AbDead)
							     truncate = TRUE;
							  else
							     truncate = !completeChild;

						       }
						     if (pAbbParentAssoc != NULL)
							pAbbParentAssoc->AbPresentationBox = TRUE;
						     pAbbPres = TruncateOrCompleteAbsBox (pAbb, truncate, FALSE, pDoc);
						     if (forward && !truncate)
							*complete = TRUE;
						     if (pAbbParentAssoc != NULL)
							pAbbParentAssoc->AbPresentationBox = FALSE;
						  }
						if (PcFirst)
						  {
						     pAbbChild = pAbb->AbFirstEnclosed;
#ifdef __COLPAGE__
						     /* cas particulier de la racine : il faut */
						     /* aller sous le premier corps de page */
						     if (pAbbChild != NULL
							 && pAbbChild->AbElement->ElTypeNumber
							 == PageBreak + 1)
						       {
							  while (pAbbChild->AbPresentationBox)
							     pAbbChild = pAbbChild->AbNext;
							  /* pAbbChild premier corps de page. On */
							  /* descend la hierarchie des colonnes */
							  while (pAbbChild->AbFirstEnclosed != NULL &&
								 pAbbChild->AbFirstEnclosed->AbElement->ElTypeNumber == PageBreak + 1)
							    {
							       pAbbChild = pAbbChild->AbFirstEnclosed;
							       while (pAbbChild->AbPresentationBox)
								  pAbbChild = pAbbChild->AbNext;
							    }
							  pAbbChild = pAbbChild->AbFirstEnclosed;
						       }
#endif /* __COLPAGE__ */
						     if (pAbbChild == NULL)
							/* tous les descendants ont une visibilite' */
							/* nulle ; le pave est donc complete en tete */
							truncate = FALSE;
						     else
						       {
							  /* ignore les paves de presentation */
							  while (pAbbChild->AbPresentationBox &&
								 pAbbChild->AbNext != NULL)
							     pAbbChild = pAbbChild->AbNext;
							  if (pAbbChild->AbDead)
							     truncate = TRUE;
							  else
							     truncate = !completeChild;
						       }
						     if (pAbbParentAssoc != NULL)
							pAbbParentAssoc->AbPresentationBox = TRUE;
						     pAbbPres = TruncateOrCompleteAbsBox (pAbb, truncate, TRUE, pDoc);
						     if (!forward && !truncate)
							*complete = TRUE;
						     if (pAbbParentAssoc != NULL)
							pAbbParentAssoc->AbPresentationBox = FALSE;
						  }
						if (pAbbPres != NULL)
						   /* on a cree des paves de presentation */
#ifdef __COLPAGE__
						   /* retour doit etre le pave de plus haut */
						   /* niveau cree : mais attention le pave */
						   /* de presentation cree est peut etre sur */
						   /* une autre page */
						   if (pAbbReturn == NULL ||
						       (!Creation || (pAbbPres->AbEnclosing != pAbb
						      && pAbb != pAbbRoot)))
#else  /* __COLPAGE__ */
						   if (!Creation || pAbbPres->AbEnclosing != pAbb)
#endif /* __COLPAGE__ */
						      pAbbReturn = pAbbPres;
					     }
					}
				   }
#ifdef __COLPAGE__
			      }	/* fin pElChild != NULL apres appel creation fils */
#endif /* __COLPAGE__ */
			 }	/* fin: Cree les paves d'un fils et passe a un autre fils */
#ifdef __COLPAGE__
		    }		/* fin while( pElChild != NULL) */
#endif /* __COLPAGE__ */
		  /* met les valeurs par defaut des axes de reference si aucune */
		  /* regle de positionnement des axes de reference n'a ete  */
		  /* appliquee */
		  if (Creation)
		     if (pNewAbbox != NULL)
		       {
			  if (pNewAbbox->AbVertRef.PosAbRef == NULL)
			     pNewAbbox->AbVertRef.PosAbRef = pNewAbbox->AbFirstEnclosed;
			  if (pNewAbbox->AbHorizRef.PosAbRef == NULL)
			     pNewAbbox->AbHorizRef.PosAbRef = pNewAbbox->AbFirstEnclosed;
		       }
	       }		/* fin creation des fils */
	     if ((Creation || ApplyRules) && descent)
		/* applique les regles en attente */
		do
		  {
		     GetAtt (&pRule, &pAbb, &pSPres, &pAttr, queuePA, queuePS, queuePP,
			     queuePR, &lqueue, &pqueue);
		     /* recupere une regle en attente */
#ifdef __COLPAGE__
		     if (pRule != NULL && !pAbb->AbDead)
#else  /* __COLPAGE__ */
		     if (pRule != NULL)
#endif /* __COLPAGE__ */
		       {
			  crAbsBox = FALSE;
			  /* a priori ce n'est pas une regle de creation */
			  if (pRule->PrType == PtFunction)
			     /* on applique les regles de creation systematiquement:
			        on a verifie' lors de la mise en attente que le pave
			        createur n'etait pas coupe'. */
			    {
			       if (pRule->PrPresFunction == FnCreateBefore
				   || pRule->PrPresFunction == FnCreateWith
				   || pRule->PrPresFunction == FnCreateAfter
			       || pRule->PrPresFunction == FnCreateEnclosing
				   || pRule->PrPresFunction == FnCreateFirst
				   || pRule->PrPresFunction == FnCreateLast)
				 {
				    crAbsBox = TRUE;
				    if (pAttr != NULL)
				       pAbbPres = CrAbsBoxesPres (pEl, pDoc, pRule,
					pAttr->AeAttrSSchema, pAttr, viewNb,
					    pAttr->AeAttrSSchema->SsPSchema,
							       FALSE, TRUE);
				    else
				       pAbbPres = CrAbsBoxesPres (pEl, pDoc, pRule,
					  pEl->ElStructSchema, NULL, viewNb,
						       pSPres, FALSE, TRUE);
				 }
			       switch (pRule->PrPresFunction)
				     {
					case FnCreateBefore:
					   if (!forward && pAbbPres != NULL)
					      pAbbReturn = pAbbPres;
					   break;
					case FnCreateAfter:
					case FnCreateWith:
					   if (forward && pAbbPres != NULL)
					      pAbbReturn = pAbbPres;
					   break;
					case FnCreateEnclosing:
					   pAbbReturn = pAbbPres;
					   break;
					default:
					   break;
				     }
			    }
			  if (!crAbsBox)
			     /* ce n'est pas une regle de creation */
#ifdef __COLPAGE__
			     if (!ApplyRule (pRule, pSPres, pAbb, pDoc, pAttr, &bool))
				/* regles retardees a la racine */
				/* TODO est-ce necessaire ? */
			       {
				  if (AssocView (pEl))
				     pAb =
					pDoc->DocAssocRoot[pEl->ElAssocNum - 1]->ElAbstractBox[0];
				  else
				     pAb = pDoc->DocRootElement->ElAbstractBox[viewNb - 1];
				  Delay (pRule, pSPres, pAbb, pAttr, pAb);
			       }
#else  /* __COLPAGE__ */
			     if (!ApplyRule (pRule, pSPres, pAbb, pDoc, pAttr))
				Delay (pRule, pSPres, pAbb, pAttr, pAbb);
#endif /* __COLPAGE__ */
		       }
		  }
		while (pRule != NULL);
#ifdef __COLPAGE__
	     if ((Creation || ApplyRules) && descent
		 && forward && pEl->ElParent == NULL
		 && pEl->ElAbstractBox[viewNb - 1] != NULL
		 && pEl->ElAbstractBox[viewNb - 1]->AbTruncatedHead)
	       {
		  /* cas des regles de creation des boites de presentation */
		  /* de la racine qui ont ete detruites par la creation de */
		  /* pages ou de colonnes */
		  do
		    {
		       pRule = GetRule (&pRSpec, &pRDef, pEl, NULL, pSchS);

		       /* pointeur sur la regle a */
		       /* appliquer pour la vue 1 */
		       if (pRule != NULL)
			  /* si c'est une regle de creation, on l'applique */
			  if (!ApplCrRule (pRule, pSchS, pSchP, NULL, &pAbbReturn,
					   viewNb, pDoc, pEl, forward,
					   &lqueue, queuePR, queuePP,
					   queuePS, queuePA, pNewAbbox))
			     /* ce n'est pas une regle de creation */
			     /* cherche et applique la regle de meme type pour la vue */
			     for (view = 1; view <= MAX_VIEW; view++)
			       {
				  if (view == 1)
				     pRuleView = NULL;
				  else
				     pRuleView = GetRuleView (&pRSpec, &pRDef, pRule->PrType, view, pEl, NULL, pSchS);
				  if (view == viewSch && pNewAbbox != NULL
				      && DoesViewExist (pEl, pDoc, viewNb))
				    {
				       if (pRuleView == NULL)
					  pRuleView = pRule;
				       if (!ApplyRule (pRuleView, pSchP, pNewAbbox, pDoc, NULL, &bool))
					  WaitingRule (pRuleView, pNewAbbox, pSchP, NULL,
						  queuePA, queuePS, queuePP,
						       queuePR, &lqueue);
				       else
					  /* la regle a ete appliquee */
					  /* on teste bool pour savoir si une le pave a ete */
					  /* detruit suite a l'application d'une regle */
					  /* page : il faut sortir de AbsBoxesCreate pour */
					  /* recommencer la creation des fils du pere de pEl */
					  /* car un fils marque page a ete ajoute avant pEl */ if (bool)
					  /* la destruction de pNewAbbox a ete faite */
					 {
					    pAbbReturn = NULL;
					    return pAbbReturn;	/* on sort de AbsBoxesCreate */
					    /* TODO : est-ce correct ? y-a-il des regles */
					    /* en attente ou retardees ??? */
					 }
				    }
			       }
		    }
		  while (pRule != NULL);
	       }
#endif /* __COLPAGE__ */
	     /* applique toutes les regles en retard des descendants */
	     if (descent)
#ifdef __COLPAGE__
	       {
		  /* applique toutes les regles en retard des descendants */
		  /* y compris les regles des descendants des paves dupliques */
		  pAbb = pEl->ElAbstractBox[viewNb - 1];
		  /* saute les paves de presentation crees par FnCreateBefore */
		  stop = FALSE;
		  do
		     if (pAbb == NULL)
			stop = TRUE;
		     else if (!pAbb->AbPresentationBox)
			stop = TRUE;
		     else if (pAbb->AbFirstEnclosed != NULL &&
			      pAbb->AbFirstEnclosed->AbElement == pEl)
			/* pave cree' par la regle FnCreateEnclosing */
			stop = TRUE;
		     else
			pAbb = pAbb->AbNext;
		  while (!stop);
		  /* on initialise le pave sur lequel des regles seront */
		  /* mises les regles retardees a nouveau */
		  if (AssocView (pEl))
		     pAb = pDoc->DocAssocRoot[pEl->ElAssocNum - 1]->ElAbstractBox[0];
		  else
		     pAb = pDoc->DocRootElement->ElAbstractBox[viewNb - 1];
		  pPRP = pAbb;
		  /* boucle de parcours de la chaine des paves dupliques */
		  while (pPRP != NULL)
		    {
		       do
			 {
			    pAbb = pPRP;	/* pAbb a ete modifie dans la boucle */
			    GetDelayedRule (&pRule, &pSPres, &pAbb, &pAttr);
			    if (pRule != NULL && !pAbb->AbDead)
			       if (pRule->PrType == PtFunction)
				  /* cas des regles de creation des boites de pres */
				  /* de la racine qu'il faut recree */
				  pAbb1 = CrAbsBoxesPres (pAbb->AbElement, pDoc, pRule,
				     pAbb->AbElement->ElStructSchema, pAttr,
					      viewNb, pSPres, FALSE, FALSE);
			       else if (!ApplyRule (pRule, pSPres, pAbb, pDoc, pAttr, &bool))
				  /* cette regle n'a pas pu etre appliquee           */
				  /* c'est une regle correspondant a un attribut, on */
				  /* l'appliquera lorsque l'englobant sera complete   */
				  /* regles retardees a la racine */
				 {
				    if (pPRP != pAb)
				       /* pour ne pas boucler sur la racine */
				       Delay (pRule, pSPres, pAbb, pAttr, pAb);
				 }
			 }
		       while (pRule != NULL);
		       pPRP = pPRP->AbNextRepeated;
		    }
	       }
#else  /* __COLPAGE__ */
		do
		  {
		     pAbb = pEl->ElAbstractBox[viewNb - 1];
		     /* saute les paves de presentation crees par FnCreateBefore */
		     stop = FALSE;
		     do
			if (pAbb == NULL)
			   stop = TRUE;
			else if (!pAbb->AbPresentationBox)
			   stop = TRUE;
			else if (pAbb->AbFirstEnclosed != NULL &&
				 pAbb->AbFirstEnclosed->AbElement == pEl)
			   /* pave cree' par la regle FnCreateEnclosing */
			   stop = TRUE;
			else
			   pAbb = pAbb->AbNext;
		     while (!stop);
		     pPRP = pAbb;
		     GetDelayedRule (&pRule, &pSPres, &pAbb, &pAttr);
		     if (pRule != NULL)
			if (!ApplyRule (pRule, pSPres, pAbb, pDoc, pAttr))
			   /* cette regle n'a pas pu etre appliquee           */
			   /* c'est une regle correspondant a un attribut, on */
			   /* l'appliquera lorsque l'englobant sera complete   */
			   Delay (pRule, pSPres, pAbb, pAttr, pPRP);
		  }
		while (pRule != NULL);
#endif /* __COLPAGE__ */
	  }			/* fin de !ignoreDescent */
	if (pAbbParentAssoc != NULL)
	   /* retablit AbPresentationBox qui a ete modifie' pour une boite de haut */
	   /* ou bas de page qui regroupe des elements associes. */
	   pAbbParentAssoc->AbPresentationBox = TRUE;
#ifdef __COLPAGE__
	if (pNewAbbox != NULL && !pNewAbbox->AbDead)
	   if (!AbsBoxOk (pNewAbbox))
	      printf ("erreur fin AbsBoxesCreate \n");
	/* si on a cree une marque de page precedente, il faut */
	/* repositionner pEl a l'ancienne valeur. Utile ? */
	if (pElSauv != NULL)
	   pEl = pElSauv;
	/* creation des elements associes en haut ou bas de page */
	/* si l'element pEl est un element reference vers ce type */
	/* d'element associe et si l'elt associe n'est pas deja cree */
	pElRef = NULL;
	if (pEl->ElAbstractBox[viewNb - 1] != NULL)
	   pElRef = TestElHB (pEl, viewNb);
	if (pElRef != NULL)
	  {
	     /* recherche de l'element marque de page precedent */
	     stop = FALSE;
	     pElPage = pEl;
	     do
	       {
		  pElPage = BackSearchTypedElem (pElPage, PageBreak + 1, NULL);
		  if (pElPage == NULL)
		     stop = TRUE;
		  else if (pElPage->ElViewPSchema == viewSch &&
			   (pElPage->ElPageType == PgBegin ||
			    pElPage->ElPageType == PgComputed ||
			    pElPage->ElPageType == PgUser))
		     stop = TRUE;
	       }
	     while (!stop);
	     if (pElPage != NULL)
	       {
		  CrPavHB (pElRef, pDoc, viewNb, pElPage, forward);
		  if (RunningPaginate && !StopBeforeCreation)
		    {
		       /* on memorise la reference a l'elt assoc */
		       HFPageRefAssoc = pEl;
		       /* et on positionne le booleen FoundPageHF */
		       /* pour provoquer l'arret de la creation */
		       FoundPageHF = TRUE;
		    }
	       }
	  }
	if (pAbbReturn != NULL && pAbbReturn->AbDead)
	   pAbbReturn = NULL;
	/* si pagination en cours et que l'element est une marque page */
	/* ou un element reference sur un element associe place en haut */
	/* ou bas de page, on s'arrete si StopBeforeCreation est FAUX */
	/* (c-a-d l'arret se fait apres creation) */
	/* l'arret est remonte de la pile des appels en mettant le */
	/* volume libre negatif (cf. IsViewFull) */
	if (RunningPaginate && !FoundPageHF && !StopBeforeCreation)
	   if (pEl->ElTerminal && pEl->ElLeafType == LtPageColBreak
	       && (pEl->ElPageType == PgBegin
		   || pEl->ElPageType == PgComputed
		   || pEl->ElPageType == PgUser)
	       && pEl->ElAbstractBox[viewNb - 1] != NULL)
	      if (pEl->ElViewPSchema == viewSch)
		 FoundPageHF = TRUE;
	if (RunningPaginate && FoundPageHF)
	   /* volume mis a -1 : arret creation */
	   if (AssocView (pEl))
	      pDoc->DocAssocFreeVolume[pEl->ElAssocNum - 1] = -1;
	   else
	      pDoc->DocViewFreeVolume[viewNb - 1] = -1;
#endif /* __COLPAGE__ */
     }
   return pAbbReturn;
}
