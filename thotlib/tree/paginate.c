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
 * gestion de la pagination d'un arbre abstrait. Insere les diverses
 * marques de saut de page dans l'AA. Les effets de bord sont nombreux.
 * Ce module insere les marques de saut de page dans la
 * structure abstraite des documents.
 *
 * Authors: V. Quint (INRIA)
 *          I. Vatton (INRIA)
 *          C. Roisin (INRIA) - Pagination at printing time
 *
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "libmsg.h"
#include "message.h"
#include "appdialogue.h"
#include "fileaccess.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "appdialogue_tv.h"

#undef THOT_EXPORT
#define THOT_EXPORT
#include "page_tv.h"


#include "absboxes_f.h"
#include "applicationapi_f.h"
#include "attributes_f.h"
#include "buildboxes_f.h"
#include "boxpositions_f.h"
#include "callback_f.h"
#include "changeabsbox_f.h"
#include "changepresent_f.h"
#include "content_f.h"
#include "createabsbox_f.h"
#include "createpages_f.h"
#include "exceptions_f.h"
#include "font_f.h"
#include "frame_f.h"
#include "paginate_f.h"
#include "pagecommands_f.h"
#include "presrules_f.h"
#include "print_f.h"
#include "structcreation_f.h"
#include "structlist_f.h"
#include "structmodif_f.h"
#include "structselect_f.h"
#include "presvariables_f.h"
#include "tree_f.h"
#include "writepivot_f.h"

#define MaxPageLib 20
#define HMinPage 60		/* Hauteur minimum du corps de la page */
#ifdef __COLPAGE__

static int          bottomPageHeightRef;/* Hauteur minimum des bas de page */
static int          topPageHeightRef;	/* Hauteur minimum des hauts de page */

#endif /* __COLPAGE__ */
static int          pagesCounter;

#ifdef __COLPAGE__
 /*----------------------------------------------------------------------
    ChangeRHPage change la regle de dimension verticale de tous       
    les paves corps de page sous rootAbsBox.               
    On force la position du bas de page et du filet de    
    telle sorte qu'elle soit plus bas que la hauteur max  
    de la hauteur de coupure                              
   ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ChangeRHPage (PtrAbstractBox rootAbsBox, PtrDocument pDoc, int nbView)
#else  /* __STDC__ */
static void         ChangeRHPage (rootAbsBox, pDoc, nbView)
PtrAbstractBox      rootAbsBox;
PtrDocument         pDoc;
int                 nbView;
#endif /* __STDC__ */
{
   PtrAbstractBox      pP;
   AbDimension        *pDim;
   AbPosition         *pPosV;
   PtrAbstractBox      pPChild, pCol;

   pP = rootAbsBox->AbFirstEnclosed;
   /* on saute les paves morts (si appele depuis KillAbsBoxBeforePage) */
   while (pP != NULL && pP->AbDead)
      pP = pP->AbNext;
   while (pP != NULL)
     {
	if (pP->AbElement->ElTypeNumber == PageBreak + 1
	    /* tj vrai ? */
	    && !pP->AbPresentationBox)
	  {
	     /* c'est un corps de page */
	     /* on change sa regle de hauteur */
	     pDim = &pP->AbHeight;
	     pDim->DimIsPosition = FALSE;
	     pDim->DimValue = 0;
	     pDim->DimAbRef = NULL;
	     pDim->DimUnit = UnRelative;
	     pDim->DimSameDimension = TRUE;
	     pDim->DimUserSpecified = FALSE;
	     pDim->DimMinimum = FALSE;
	     pP->AbHeightChange = TRUE;
	     /* si le fils est une colonne, on fait pareil */
	     pPChild = pP->AbFirstEnclosed;
	     while (pPChild != NULL
		    && pPChild->AbElement->ElTypeNumber == PageBreak + 1)
		/* on parcourt la suite des groupes de colonnes en */
		/* sautant les eventuels paves de presentation */
		/* TODO parcours en profondeur quand regles incluses ! */
	       {
		  while (pPChild->AbPresentationBox)
		     pPChild = pPChild->AbNext;
		  if (pPChild->AbElement->ElTypeNumber == PageBreak + 1)
		    {
		       pDim = &pPChild->AbHeight;
		       pDim->DimIsPosition = FALSE;
		       pDim->DimValue = 0;
		       pDim->DimAbRef = NULL;
		       pDim->DimUnit = UnRelative;
		       pDim->DimSameDimension = TRUE;
		       pDim->DimUserSpecified = FALSE;
		       pDim->DimMinimum = FALSE;
		       pPChild->AbHeightChange = TRUE;
		       /* si le fils est une colonne, on fait pareil */
		       pCol = pPChild->AbFirstEnclosed;
		       while (pCol != NULL
			  && pCol->AbElement->ElTypeNumber == PageBreak + 1)
			  /* on parcourt la suite des colonnes en */
			  /* sautant les eventuels paves de presentation */
			  /* TODO parcours en profondeur quand regles incluses ! */
			 {
			    while (pCol->AbPresentationBox)
			       pCol = pCol->AbNext;
			    if (pCol->AbElement->ElTypeNumber == PageBreak + 1)
			      {
				 pDim = &pCol->AbHeight;
				 pDim->DimIsPosition = FALSE;
				 pDim->DimValue = 0;
				 pDim->DimAbRef = NULL;
				 pDim->DimUnit = UnRelative;
				 pDim->DimSameDimension = TRUE;
				 pDim->DimUserSpecified = FALSE;
				 pDim->DimMinimum = FALSE;
				 pCol->AbHeightChange = TRUE;
			      }
			    pCol = pCol->AbNext;
			 }
		    }
		  pPChild = pPChild->AbNext;
	       }
	  }
	pP = pP->AbNext;
	/* change la position des paves suivants pour qu'ils n'interferent */
	/* pas lors du placement de la coupure de page par le mediateur */
	while (pP != NULL && pP->AbPresentationBox &&
	       pP->AbElement == pP->AbPrevious->AbElement)
	  {
	     pPosV = &pP->AbVertPos;
	     pPosV->PosAbRef = NULL;
	     pPosV->PosDistance = WholePageHeight + 10;
	     pPosV->PosUnit == UnPoint;
	     pPosV->PosUserSpecified = FALSE;
	     pP->AbVertPosChange = TRUE;
	     pP = pP->AbNext;
	  }
     }
}

#endif /* __COLPAGE__ */

#ifndef PAGINEETIMPRIME

/*----------------------------------------------------------------------
   AbortPageSelection  Annule et deplace si besoin la selection	
   			  courante du document.				
   			  Retourne les valeurs de cette selection 	
   			  dans firstSelection, lastSelection,           
   FirstSelectedChar et LastSelectedChar	        
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static boolean      AbortPageSelection (PtrDocument pDoc, int schView, PtrElement * firstSelection, PtrElement * lastSelection, int *FirstSelectedChar, int *LastSelectedChar)

#else  /* __STDC__ */
static boolean      AbortPageSelection (pDoc, schView, firstSelection, lastSelection, FirstSelectedChar, LastSelectedChar)
PtrDocument         pDoc;
int                 schView;
PtrElement         *firstSelection;
PtrElement         *lastSelection;
int                *FirstSelectedChar;
int                *LastSelectedChar;

#endif /* __STDC__ */

{
   PtrDocument         SelDoc;
   PtrElement          pEl1, pEl2, first, last;
   boolean             sel;

   /* demande quelle est la selection courante */
   sel = GetCurrentSelection (&SelDoc, &first, &last, FirstSelectedChar, LastSelectedChar);
   if (sel && SelDoc != pDoc)
      sel = FALSE;
   /* annule la selection si elle est dans le document a paginer */
   if (sel)
     {
	CancelSelection ();
	/* on verifie si la selection commence ou se termine sur une marque */
	/* de page qui va disparaitre et dans ce cas on change la selection */
	pEl1 = first;		/* debut de la selection */
	if (pEl1->ElTypeNumber == PageBreak + 1)
	   if (pEl1->ElViewPSchema == schView)
#ifdef __COLPAGE__
	      if (pEl1->ElPageType == PgComputed
		  || pEl1->ElPageType == ColComputed
		  || pEl1->ElPageType == ColBegin
		  || pEl1->ElPageType == ColGroup)
#else  /* __COLPAGE__ */
	      if (pEl1->ElPageType == PgComputed)
#endif /* __COLPAGE__ */
		 /* c'est une marque de page qui va disparaitre */
		 if (pEl1->ElNext != NULL)
		    /* on selectionne l'element suivant la marque de page */
		   {
		      if (last == first)
			 last = pEl1->ElNext;
		      first = pEl1->ElNext;
		   }
		 else
		    /* pas d'element suivant la marque de page */ if (pEl1->ElPrevious != NULL)
		    /* on selectionne l'element precedent la marque de page */
		   {
		      if (last == first)
			 last = pEl1->ElPrevious;
		      first = pEl1->ElPrevious;
		   }
		 else
		    /* pas de suivant ni de precedent */
		    /* on selectionne l'element englobant la marque de page */
		   {
		      first = pEl1->ElParent;
		      last = pEl1->ElParent;
		   }
	pEl1 = last;
	/* dernier element de la selection */
	if (pEl1->ElTypeNumber == PageBreak + 1)
	   if (pEl1->ElViewPSchema == schView)
#ifdef __COLPAGE__
	      if (pEl1->ElPageType == PgComputed
		  || pEl1->ElPageType == ColComputed
		  || pEl1->ElPageType == ColBegin
		  || pEl1->ElPageType == ColGroup)
#else  /* __COLPAGE__ */
	      if (pEl1->ElPageType == PgComputed)
#endif /* __COLPAGE__ */
		 /* le dernier element de la selection est une marque de */
		 /* page qui va disparaitre */
		 if (pEl1->ElPrevious != NULL)
		    last = pEl1->ElPrevious;
	/* on selectionne le precedent */
		 else if (pEl1->ElNext != NULL)
		    last = pEl1->ElNext;
	/* on selectionne le suivant */
		 else
		    /* on selectionne l'englobant */
		   {
		      first = pEl1->ElParent;
		      last = pEl1->ElParent;
		   }
	/* le debut de la selection est-il dans une feuille de texte qui
	   n'est separee de la precedente que par une marque de page ? Dans
	   ce cas il y aura fusion des deux feuilles et la deuxieme
	   n'existera plus. */
	pEl1 = first;		/* debut de la selection */
	if (pEl1->ElTerminal)
	   if (pEl1->ElLeafType == LtText)
	      /* la selection debute dans une feuille de texte */
	      if (pEl1->ElPrevious != NULL)
		 if (pEl1->ElPrevious->ElTypeNumber == PageBreak + 1)
		    if (pEl1->ElPrevious->ElViewPSchema == schView)
#ifdef __COLPAGE__
		       if (pEl1->ElPrevious->ElPageType == PgComputed
			   || pEl1->ElPrevious->ElPageType == ColComputed
			   || pEl1->ElPrevious->ElPageType == ColBegin
			   || pEl1->ElPrevious->ElPageType == ColGroup)
#else  /* __COLPAGE__ */
		       if (pEl1->ElPrevious->ElPageType == PgComputed)
#endif /* __COLPAGE__ */
			  /* la feuille de texte est precedee d'une marque de
			     page qui va disparaitre, on examine l'element
			     precedent la marque de page */
			 {
			    pEl2 = pEl1->ElPrevious->ElPrevious;
			    if (pEl2 != NULL)
			       if (pEl2->ElTerminal &&
				   pEl2->ElLeafType == LtText)
				  /* c'est une feuille de texte */
				  if (pEl2->ElLanguage == pEl1->ElLanguage)
				     /* meme langue */
				     if (SameAttributes (first, pEl2))
					/* memes attributs */
					if (BothHaveNoSpecRules (first, pEl2))
					   /* meme present. */
					   /* les elements vont fusionner, on selectionne le 1er */
					  {
					     if (last == first)
					       {
						  last = pEl2;
						  *LastSelectedChar = 0;
					       }
					     first = pEl2;
					     *LastSelectedChar = 0;
					  }
			 }
	/* la fin de la selection est-il dans une feuille de texte qui
	   n'est separee de la precedente que par une marque de page ? Dans
	   ce cas il y aura fusion des deux feuilles et la deuxieme
	   n'existera plus. */
	pEl1 = last;
	/* fin de la selection */
	if (pEl1->ElTerminal)
	   if (pEl1->ElLeafType == LtText)
	      /* la selection se termine dans une feuille de texte */
	      if (pEl1->ElPrevious != NULL)
		 if (pEl1->ElPrevious->ElTypeNumber == PageBreak + 1)
		    if (pEl1->ElPrevious->ElViewPSchema == schView)
#ifdef __COLPAGE__
		       if (pEl1->ElPrevious->ElPageType == PgComputed
			   || pEl1->ElPrevious->ElPageType == ColComputed
			   || pEl1->ElPrevious->ElPageType == ColBegin
			   || pEl1->ElPrevious->ElPageType == ColGroup)
#else  /* __COLPAGE__ */
		       if (pEl1->ElPrevious->ElPageType == PgComputed)
#endif /* __COLPAGE__ */
			  /* la feuille de texte est precedee d'une marque de
			     page qui va disparaitre, on examine l'element
			     precedent la marque de page */
			 {
			    pEl2 = pEl1->ElPrevious->ElPrevious;
			    if (pEl2 != NULL)
			       if (pEl2->ElTerminal &&
				   pEl2->ElLeafType == LtText)
				  /* c'est une feuille de texte */
				  if (pEl2->ElLanguage == pEl1->ElLanguage)
				     /* meme langue */
				     if (SameAttributes (last, pEl2))
					/* memes attributs */
					if (BothHaveNoSpecRules (last, pEl2))
					   /* meme present. */
					   /* les elements vont fusionner, on selectionne le 1er */
					  {
					     last = pEl2;
					     *LastSelectedChar = 0;
					  }

			 }
	*firstSelection = first;
	*lastSelection = last;
     }				/* fin if (sel) */
   return sel;
}				/* AbortPageSelection */
#endif /* PAGINEETIMPRIME */


/*----------------------------------------------------------------------
   SuppressPageMark supprime la marque de page pointee par pPage et	
   		essaie de fusionner l'element precedent avec l'element	
   		suivant.						
   		Retourne dans pLib un pointeur sur l'element a libere	
   		resultant de la fusion, si elle a pu se faire.		
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         SuppressPageMark (PtrElement pPage, PtrDocument pDoc, PtrElement * pLib)

#else  /* __STDC__ */
static void         SuppressPageMark (pPage, pDoc, pLib)
PtrElement          pPage;
PtrDocument         pDoc;
PtrElement         *pLib;

#endif /* __STDC__ */

{
   PtrElement          pPrevious;
   NotifyElement       notifyEl;
   int                 NSiblings;

   *pLib = NULL;
   /* envoie l'evenement ElemDelete.Pre */
   notifyEl.event = TteElemDelete;
   notifyEl.document = (Document) IdentDocument (pDoc);
   notifyEl.element = (Element) (pPage);
   notifyEl.elementType.ElTypeNum = pPage->ElTypeNumber;
   notifyEl.elementType.ElSSchema = (SSchema) (pPage->ElStructSchema);
   notifyEl.position = TTE_STANDARD_DELETE_LAST_ITEM;
   if (!CallEventType ((NotifyEvent *) & notifyEl, TRUE))
     {
	/* traitement de la suppression des pages dans les structures avec */
	/* coupures speciales */
	if (ThotLocalActions[T_deletepage] != NULL)
	   (*ThotLocalActions[T_deletepage]) (pPage, pDoc);
	pPrevious = pPage->ElPrevious;
	/* prepare l'evenement ElemDelete.Post */
	notifyEl.event = TteElemDelete;
	notifyEl.document = (Document) IdentDocument (pDoc);
	notifyEl.element = (Element) (pPage->ElParent);
	notifyEl.elementType.ElTypeNum = pPage->ElTypeNumber;
	notifyEl.elementType.ElSSchema = (SSchema) (pPage->ElStructSchema);
	NSiblings = 0;
	DeleteElement (&pPage);
	*pLib = NULL;
	if (pPrevious != NULL)
	  {
	     /* il y avait un element avant la marque de page, on essaie de le */
	     /* fusionner avec l'element qui le suit maintenant. */
	     if (!IsIdenticalTextType (pPrevious, pDoc, pLib))
		*pLib = NULL;
	     while (pPrevious != NULL)
	       {
		  NSiblings++;
		  pPrevious = pPrevious->ElPrevious;
	       }
	  }
	notifyEl.position = NSiblings;
	CallEventType ((NotifyEvent *) & notifyEl, FALSE);
     }
}


/*----------------------------------------------------------------------
   	DestroyPageMarks	detruit toutes les marques de page de la
   		vue schView, sauf les marques placees par l'utilisateur	
   		et celles de debut des elements portant une regle Page.	
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         DestroyPageMarks (PtrDocument pDoc, PtrElement pRootEl, int schView)

#else  /* __STDC__ */
static void         DestroyPageMarks (pDoc, pRootEl, schView)
PtrDocument         pDoc;
PtrElement          pRootEl;
int                 schView;

#endif /* __STDC__ */

{
   PtrElement          pEl, pElPage, pElLib;


   pEl = pRootEl;
   pElPage = NULL;
   /* pas encore de marque de page a supprimer */
   while (pEl != NULL)
      /* cherche la prochaine marque de page */
     {
	pEl = FwdSearchTypedElem (pEl, PageBreak + 1, NULL);
	if (pEl != NULL)
	   if (pEl->ElViewPSchema == schView)
	      /* on a trouve' une marque de page concernant la vue */
#ifdef __COLPAGE__
	      if (pEl->ElPageType == PgComputed
		  || pEl->ElPageType == ColComputed
		  || pEl->ElPageType == ColBegin
		  || pEl->ElPageType == ColGroup)
#else  /* __COLPAGE__ */
	      if (pEl->ElPageType == PgComputed)
#endif /* __COLPAGE__ */
		 /* c'est une marque de page calculee */
		{
		   if (pElPage != NULL)
		      /* il y a deja une marque de page a supprimer, on la supprime */
		     {
			SuppressPageMark (pElPage, pDoc, &pElLib);
			if (pElLib != NULL)
			   DeleteElement (&pElLib);
		     }
		   /* on supprimera cette marque de page au tour suivant */
		   pElPage = pEl;
		}
	      else
		 /* c'est une marque de page a conserver */
		 /* on ne creera pas tout de suite ses boites de haut de page */
		 /* contenant des elements associes. */
		 pEl->ElAssocHeader = FALSE;
     }
   if (pElPage != NULL)
      /* il reste une marque de page a supprimer, on la supprime */
     {
	SuppressPageMark (pElPage, pDoc, &pElLib);
	if (pElLib != NULL)
	   DeleteElement (&pElLib);
     }

}				/*DestroyPageMarks */

#ifndef PAGINEETIMPRIME

/*----------------------------------------------------------------------
   	DisplayPageMsg 	Affiche un message avec le nom de la vue 	
   			et le numero de page de l'element pEl		
   			firstPage indique si c'est la premiere page de 	
   			la vue						
   			procedure utilisee dans la pagination sous	
   			l'editeur (version vide pour l'appel depuis	
   			la commande d'impression)			
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         DisplayPageMsg (PtrDocument pDoc, PtrElement pRootEl, PtrElement pEl, int schView, boolean Assoc, boolean * firstPage)
#else  /* __STDC__ */
static void         DisplayPageMsg (pDoc, pRootEl, pEl, schView, Assoc, firstPage)
PtrDocument         pDoc;
PtrElement          pRootEl;
PtrElement          pEl;
int                 schView;
boolean             Assoc;
boolean            *firstPage;

#endif /* __STDC__ */
{
   char               *name;

   /* affiche un message avec le numero de page */
   /* affiche d'abord le nom de la vue */
   if (Assoc)
      name = pRootEl->ElStructSchema->SsRule[pRootEl->ElTypeNumber - 1].SrName;
   else
      name = pDoc->DocSSchema->SsPSchema->PsView[schView - 1];

   if (*firstPage)
     {
	/* Affiche un message normal pour la 1ere fois */
	TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_PAGE), name, (char *) pEl->ElPageNumber);
	*firstPage = FALSE;
     }
   else
      /* Sinon ecrase le message precedent */
      TtaDisplayMessage (OVERHEAD, TtaGetMessage (LIB, TMSG_PAGE), name, (char *) pEl->ElPageNumber);
}
#endif /* PAGINEETIMPRIME */

#ifndef PAGINEETIMPRIME

/*----------------------------------------------------------------------
   DisplaySelectPages Apres la pagination sous l'editeur, il faut	
   			recreer l'image et retablir la selection.	
   			Procedure utilisee dans la pagination sous	
   			l'editeur (version vide pour l'appel depuis	
   			la commande d'impression)			
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         DisplaySelectPages (PtrDocument pDoc, PtrElement firstPage, int view, boolean Assoc, boolean sel, PtrElement firstSelection, PtrElement lastSelection, int FirstSelectedChar, int LastSelectedChar)
#else  /* __STDC__ */
static void         DisplaySelectPages (pDoc, firstPage, view, Assoc, sel, firstSelection, lastSelection, FirstSelectedChar, LastSelectedChar)
PtrDocument         pDoc;
PtrElement          firstPage;
int                 view;
boolean             Assoc;
boolean             sel;
PtrElement          firstSelection;
PtrElement          lastSelection;
int                 FirstSelectedChar;
int                 LastSelectedChar;

#endif /* __STDC__ */

{
   PtrElement          pRootEl;
   PtrAbstractBox      rootAbsBox /* , pP */ ;
   int                 v, schView, frame, h;
   boolean             complete;

   /* reconstruit l'image de la vue et l'affiche */
   /* si on n'est pas en batch */
#ifdef __COLPAGE__
   BreakPageHeight = 0;
   WholePageHeight = 0;
   bottomPageHeightRef = 0;
#else  /* __COLPAGE__ */
   PageHeight = 0;
   PageFooterHeight = 0;
#endif /* __COLPAGE__ */

   /* cree l'image abstraite des vues concernees */
   if (Assoc)
     {
#ifdef __COLPAGE__
	pDoc->DocAssocFreeVolume[view - 1] = THOT_MAXINT;
	pDoc->DocAssocNPages[view - 1] = 0;
#else  /* __COLPAGE__ */
	pDoc->DocAssocFreeVolume[view - 1] = pDoc->DocAssocVolume[view - 1];
#endif /* __COLPAGE__ */
	pRootEl = pDoc->DocAssocRoot[view - 1];
	rootAbsBox = pRootEl->ElAbstractBox[0];
	frame = pDoc->DocAssocFrame[view - 1];
	AbsBoxesCreate (pRootEl, pDoc, 1, TRUE, TRUE, &complete);
	h = 0;
	(void) ChangeConcreteImage (frame, &h, rootAbsBox);
	if (!sel)
	   DisplayFrame (frame);
     }
   else
     {
	pRootEl = pDoc->DocRootElement;
	schView = AppliedView (pDoc->DocRootElement, NULL, pDoc, view);
	for (v = 1; v <= MAX_VIEW_DOC; v++)
	   if (pDoc->DocView[v - 1].DvPSchemaView == schView)
	     {
#ifdef __COLPAGE__
		pDoc->DocViewFreeVolume[v - 1] = THOT_MAXINT;
		pDoc->DocViewNPages[v - 1] = 0;
#else  /* __COLPAGE__ */
		pDoc->DocViewFreeVolume[v - 1] = pDoc->DocViewVolume[v - 1];
#endif /* __COLPAGE__ */
		rootAbsBox = pDoc->DocViewRootAb[v - 1];
		frame = pDoc->DocViewFrame[v - 1];
		AbsBoxesCreate (pRootEl, pDoc, v, TRUE, TRUE, &complete);
		h = 0;
		(void) ChangeConcreteImage (frame, &h, rootAbsBox);
		if (!sel)
		   DisplayFrame (frame);
	     }
     }

   /* retablit la selection si elle ete supprimee avant le formatage */
   if (sel)
      MergeAndSelect (pDoc, firstSelection, lastSelection, FirstSelectedChar, LastSelectedChar);

   /* met a jour les numeros qui changent dans les autres vues a cause */
   /* de la creation des nouvelles marques de page */
   if (firstPage != NULL)
      UpdateNumbers (NextElement (firstPage), firstPage, pDoc, TRUE);

}				/* DisplaySelectPages */
#endif /* PAGINEETIMPRIME */


/*----------------------------------------------------------------------
   	Cut coupe l'element de texte pointe par pEl apres le		
   		caractere de rang cutChar et met a jour les paves	
   		correspondant.						
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         Cut (PtrElement pEl, int cutChar, PtrDocument pDoc, int nbView)

#else  /* __STDC__ */
static void         Cut (pEl, cutChar, pDoc, nbView)
PtrElement          pEl;
int                 cutChar;
PtrDocument         pDoc;
int                 nbView;

#endif /* __STDC__ */

{
   PtrElement	       pSecond;

   SplitTextElement (pEl, cutChar + 1, pDoc, TRUE, &pSecond);
   /* reduit le volume du pave de l'element precedant le point de */
   /* coupure et de ses paves englobants, si ces paves existent dans la */
   /* vue traitee. */
   UpdateAbsBoxVolume (pEl, nbView - 1, pDoc);
   /* prepare la creation des paves de la 2eme partie */
   if (!AssocView (pEl))
      if (pDoc->DocView[nbView - 1].DvPSchemaView > 0)
	 pDoc->DocViewFreeVolume[nbView - 1] = THOT_MAXINT;
      else if (pDoc->DocAssocFrame[pEl->ElAssocNum - 1] != 0)
	 pDoc->DocAssocFreeVolume[pEl->ElAssocNum - 1] = THOT_MAXINT;
   /* cree les paves de la deuxieme partie */
   CreateNewAbsBoxes (pSecond, pDoc, nbView);
   ApplDelayedRule (pSecond, pDoc);
}

/*----------------------------------------------------------------------
   	Divisible retourne 'Vrai' si le pave pointe' par
   		pAb est secable et 'Faux' sinon.			
   		Au retour, si la boite est secable, pR1 pointe sur la	
   		regle NoBreak1 a appliquer a l'element			
   		(pR1 est NULL s'il n'y a pas de regle NoBreak1 a
   		appliquer)						
   		pAt1 pointe sur le bloc de l'attribut auquel correspond	
   		la regle pR1, si c'est une regle d'attribut (pAt1=NULL	
   		sinon),							
   		pR2 pointe sur la regle NoBreak2 a appliquer a l'element
   		(pR2 est NULL s'il n'y a pas de regle NoBreak2 a
   		appliquer),						
   		pAt2 pointe sur le bloc de l'attribut auquel correspond	
   		la regle pR2, si c'est une regle d'attribut		
   		(pAt2=NULL sinon). 					
  ----------------------------------------------------------------------*/


#ifdef __STDC__
static boolean      Divisible (PtrAbstractBox pAb, PtrPRule * pR1, PtrAttribute * pAt1, PtrPRule * pR2, PtrAttribute * pAt2, int schView)

#else  /* __STDC__ */
static boolean      Divisible (pAb, pR1, pAt1, pR2, pAt2, schView)
PtrAbstractBox      pAb;
PtrPRule           *pR1;
PtrAttribute       *pAt1;
PtrPRule           *pR2;
PtrAttribute       *pAt2;
int                 schView;

#endif /* __STDC__ */

{
   PtrPSchema          pSchP;
   int                 entry;
   boolean             ret;
   PtrSSchema          pSchS;

   *pR1 = NULL;
   *pR2 = NULL;

   if (!pAb->AbAcceptPageBreak)
      /* le pave est insecable */
      ret = FALSE;
   else
      /* le pave est secable */
   if (pAb->AbPresentationBox)
      /* c'est un pave de presentation */
      ret = TRUE;
   else
      /* c'est le pave principal d'un element, on cherche les regles */
      /* NoBreak1 et NoBreak2 */
     {
	/* cherche le schema de presentation a appliquer a l'element */
	SearchPresSchema (pAb->AbElement, &pSchP, &entry, &pSchS);
	ret = TRUE;
	/* cherche la regle NoBreak1 qui s'applique au pave */
	*pR1 = GlobalSearchRulepEl (pAb->AbElement, &pSchP, &pSchS, 0, NULL, schView, PtBreak1, FnAny,
				    FALSE, TRUE, pAt1);
	/* cherche la regle NoBreak2 qui s'applique au pave */
	*pR2 = GlobalSearchRulepEl (pAb->AbElement, &pSchP, &pSchS, 0, NULL, schView, PtBreak2, FnAny,
				    FALSE, TRUE, pAt2);
     }
   return ret;
}


/*----------------------------------------------------------------------
   	PageBrk Page	indique si l'element pEl debute par un saut de	
   		page de la vue schView					
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static PtrElement   PageBrk (PtrElement pEl, int schView)
#else  /* __STDC__ */
static PtrElement   PageBrk (pEl, schView)
PtrElement          pEl;
int                 schView;

#endif /* __STDC__ */

{
   PtrElement          pE;
   boolean             found;

   if (pEl->ElTerminal)
      return NULL;
   else if (pEl->ElFirstChild == NULL)
      return NULL;
   else if (pEl->ElFirstChild->ElTypeNumber == PageBreak + 1)
     {
	/* on cherche dans les premiers fils une marque de la vue */
	found = FALSE;
	pE = pEl->ElFirstChild;
	while (!found && pE != NULL &&
	       pE->ElTypeNumber == PageBreak + 1)
	   if (pE->ElViewPSchema == schView)
	      found = TRUE;
	   else
	      pE = pE->ElNext;
	if (found)
	   return (pE);
	else
	   return (NULL);
     }
   else
      return (PageBrk (pEl->ElFirstChild, schView));
}


/*----------------------------------------------------------------------
   	InsertMark insere une Marque de Page avant l'element auquel	
   		correspond le pave pointe' par pP. Si la Marque doit	
   		etre placee dans un element mis en lignes, en premiere	
   		position, elle est placee avant cet element. 		
   		Retourne un pointeur sur l'element Marque de Page	
   		insere'.						
    On detruit la partie de l'i.a. qui suit cette marque    
    et on reconstruit l'i.a. (donc le pave pAb change ! )  
  ----------------------------------------------------------------------*/
#ifdef __COLPAGE__
#ifdef __STDC__
static PtrElement   InsertMark (PtrAbstractBox pAb, int frame, int nbView, PtrAbstractBox * origCutAbsBox, boolean * absBoxTooHigh, int schView, PtrDocument pDoc, PtrElement rootEl)

#else  /* __STDC__ */
static PtrElement   InsertMark (pAb, frame, nbView, origCutAbsBox, absBoxTooHigh, schView, pDoc, rootEl)
PtrAbstractBox      pAb;
int                 frame;
int                 nbView;
PtrAbstractBox     *origCutAbsBox;
boolean            *absBoxTooHigh;
int                 schView;
PtrDocument         pDoc;
PtrElement          rootEl;

#endif /* __STDC__ */

#else  /* __COLPAGE__ */
#ifdef __STDC__
static PtrElement   InsertMark (PtrAbstractBox pAb, int frame, int nbView, PtrAbstractBox * origCutAbsBox, boolean * absBoxTooHigh, int schView, PtrDocument pDoc, PtrElement rootEl)

#else  /* __STDC__ */
static PtrElement   InsertMark (pAb, frame, nbView, origCutAbsBox, absBoxTooHigh, schView, pDoc, rootEl)
PtrAbstractBox      pAb;
int                 frame;
int                 nbView;
PtrAbstractBox     *origCutAbsBox;
boolean            *absBoxTooHigh;
int                 schView;
PtrDocument         pDoc;
PtrElement          rootEl;

#endif /* __STDC__ */

#endif /* __COLPAGE__ */

{
   PtrElement          pElPage, pEl;
   boolean             stop, inTop, ElemIsBefore, cut;
   PtrAbstractBox      pP1, pP;
   int                 cpt, h;
   PtrPSchema          pSchP;
   PtrAbstractBox      pPa1;
   AbPosition         *pPavP1;
   PtrElement          pEl1;
   int                 NSiblings;
   PtrElement          pF;
   NotifyElement       notifyEl;
   PtrPRule            pRule;

#ifdef __COLPAGE__
   PtrElement          pRootEl, pElCol, pElColG;
   PtrAbstractBox      pRoot, pP2, PavR;
   boolean             complete, found;
   int                 b, NbCol;

#else  /* __COLPAGE__ */
   PtrAbstractBox      modifAbsBox, topPageAbsBox, savePageAbsBox;

#endif /* __COLPAGE__ */

#ifdef __COLPAGE__

   pElPage = NULL;
   pP = pAb;
   /* si pAb est un pave de marque page, inutile d'inserer */
   /* une marque en plus : le decoupage courant est satisfaisant */
   if (!(pP->AbElement->ElTypeNumber == PageBreak + 1
	 && (pP->AbElement->ElPageType == PgBegin
	     || pP->AbElement->ElPageType == PgComputed
	     || pP->AbElement->ElPageType == PgUser)))
     {
#else  /* __COLPAGE__ */
   PageHeaderRefAssoc = NULL;
   pElPage = NULL;
   pP = pAb;
#endif /* __COLPAGE__ */
   /* teste si le pave est le premier d'un pave mis en lignes */
   if (pP->AbEnclosing != NULL)
      if (pP->AbPrevious == NULL)
	{
	   pPa1 = pP->AbEnclosing;
	   if (pPa1->AbLeafType == LtCompound && pPa1->AbInLine)
	      /* on inserera la marque de page avant le pave englobant */
	      pP = pP->AbEnclosing;
	}
   /* teste si le pave est en haut de son englobant */
   do
     {
	stop = TRUE;
	inTop = FALSE;
	if (pP->AbEnclosing != NULL)
	  {
	     pPavP1 = &pP->AbVertPos;
	     if (pPavP1->PosAbRef == NULL)
	       {
		  /* postion verticale par defaut = en haut de l'englobant */
		  inTop = TRUE;
		  /* si l'englobant est mis en ligne et que le pave n'est pas */
		  /* le premier des paves mis en ligne, il n'est pas en haut */
		  /* de son englobant */
		  if (pP->AbEnclosing->AbInLine)
		     if (pP->AbPrevious != NULL)
			inTop = FALSE;
	       }
	     else if (pPavP1->PosAbRef == pP->AbEnclosing)
		if (pPavP1->PosDistance == 0)
		   if (pPavP1->PosEdge == Top)
		      if (pPavP1->PosRefEdge == Top)
			 inTop = TRUE;
	  }
	if (inTop)
	   /* le pave est en haut de son englobant, on place la */
	   /* marque de page avant l'englobant */
	  {
	     pP = pP->AbEnclosing;
	     stop = FALSE;
	  }
     }
   while (!(stop));

   pEl = pP->AbElement;
   ElemIsBefore = TRUE;		/* on inserera la nouvelle marque de page avant pEl */
   /*on regarde s'il n'y a pas deja une marque de page juste avant pEl */
   pEl1 = pEl;
   if (pEl->ElPrevious != NULL)
     {

	/* On ignore les elements repetes en haut de page */
	/* (tetieres de tableaux par exemple)             */
	stop = FALSE;
	do
	   if (pEl1->ElPrevious == NULL)
	      stop = TRUE;
	   else if (pEl1->ElPrevious->ElIsCopy &&
		    TypeHasException (ExcPageBreakRepetition, pEl1->ElPrevious->ElTypeNumber, pEl1->ElPrevious->ElStructSchema))
	      pEl1 = pEl1->ElPrevious;
	   else
	      stop = TRUE;
	while (!stop);
     }
   pEl1 = PreviousLeaf (pEl1);
   if (pEl1 != NULL)
      if (pEl1->ElTerminal && pEl1->ElLeafType == LtPageColBreak)
	 if (pEl1->ElViewPSchema == schView)
	    /* il y a deja devant l'element pEl une marque de page
	       pour cette vue */
	    /* on mettra la nouvelle marque apres l'element pEl */
	   {
	      ElemIsBefore = FALSE;
	      /* il y a un pave plus haut que la page avant la */
	      /* nouvelle marque de page que l'on va inserer */
	      *absBoxTooHigh = TRUE;
	   }

   /* si le pave pP est un pave de presentation de l'element place' */
   /* apres ou comme dernier fils, on insere l'element marque de page */
   /* apres cet element. Rq: la page sera trop longue ! */
   if (pP->AbPresentationBox)
     {
	pRule = FunctionRule (pEl, &pSchP);
	while (pRule != NULL && ElemIsBefore == TRUE)
	  {
	     if (pP->AbTypeNum == pRule->PrPresBox[0]
		 && pP->AbPSchema == pSchP)
		/* c'est la regle correspondant a ce pave */
		if (pRule->PrPresFunction == FnCreateAfter
		    || pRule->PrPresFunction == FnCreateLast)
		   /* on insere la marque apres l'element */
		  {
		     *origCutAbsBox = NULL;
		     ElemIsBefore = FALSE;
		  }
	     pRule = pRule->PrNextPRule;
	     if (pRule != NULL && pRule->PrType > PtFunction)
		pRule = NULL;
	  }
     }

   pEl1 = pEl;
   if (!ElemIsBefore)
      if (*origCutAbsBox != NULL)
	 /* Il y a un pave insecable plus haut qu'une page, on inserera */
	 /* la marque de page apres l'element de ce pave */
	 pEl1 = (*origCutAbsBox)->AbElement;
   if (pEl1->ElParent == NULL)
      /* si pEl1 est la racine il faut descendre d'un niveau */
     if (ElemIsBefore)
	pEl1 = pEl1->ElFirstChild;
     else
	{
	pEl1 = pEl1->ElFirstChild;
	while (pEl1->ElNext != NULL)
	   pEl1 = pEl1->ElNext;
	}
   if (ElemIsBefore && pEl1->ElTypeNumber == PageBreak + 1)
      return pEl1;
   if (ElemIsBefore)
     {
	pElPage = PageBrk (pEl1, schView);
	if (pElPage != NULL)
	   /* on veut inserer un saut de page devant un element qui a la regle */
	   /* Page. On n'insere pas de nouveau saut de page et on retourne le */
	   /* saut de page de l'element suivant */
	   return pElPage;
     }
   /* envoie l'evenement ElemNew.Pre */
   notifyEl.event = TteElemNew;
   notifyEl.document = (Document) IdentDocument (pDoc);
   notifyEl.element = (Element) (pEl->ElParent);
   notifyEl.elementType.ElTypeNum = PageBreak + 1;
   notifyEl.elementType.ElSSchema = (SSchema) (rootEl->ElStructSchema);
   NSiblings = 0;
   pF = pEl1;
   while (pF->ElPrevious != NULL)
     {
	NSiblings++;
	pF = pF->ElPrevious;
     }
   if (!ElemIsBefore)
      NSiblings++;
   notifyEl.position = NSiblings;
   CallEventType ((NotifyEvent *) & notifyEl, TRUE);
   /* cree l'element Marque de Page */
   pElPage = NewSubtree (PageBreak + 1, rootEl->ElStructSchema,
			 pDoc, pEl->ElAssocNum, TRUE, TRUE, TRUE, TRUE);
   /* insere l'element dans l'arbre abstrait */
   if (pEl1->ElParent != NULL)
      if (ElemIsBefore)
	 InsertElementBefore (pEl1, pElPage);
      else
	 InsertElementAfter (pEl1, pElPage);
   pEl1 = pElPage;
#ifdef __COLPAGE__
   /* tout le reste du code de la procedure est different */
   pElCol = NULL;		/* a priori, pas d'element colonne simple cree en plus */
   pElColG = NULL;		/* a priori, pas d'element colonne groupees cree en plus */
   /* remplit l'element cree' */
   /* test si l'element suivant porte une regle page ou colonne */
   /* TODO */
   /* si ce n'est pas le cas, on regarde si la marque inseree */
   /* doit etre une marque de page ou de colonne : pour cela, on */
   /* regarde si il y a un (ou plusieurs) pave de colonne dans */
   /* la page ; si oui, on recherche la regle colonne pour */
   /* savoir si il reste des colonnes a creer et on la cree. */
   /* Sinon, on  cree une page. */
   pP = pAb;
   found = FALSE;
   while (pP->AbElement->ElTypeNumber != PageBreak + 1)
      pP = pP->AbEnclosing;
   /* les paves de page ou colonnes englobent leur contenu */
   if (pP->AbElement->ElPageType == ColBegin
       || pP->AbElement->ElPageType == ColComputed
       || pP->AbElement->ElPageType == ColUser)
     {
	/* on recherche la regle colonne pour comparer */
	/* le numero de cette colonne avec le nombre de */
	/* colonnes specifie dans la regle */
	b = TypeBCol (pP->AbElement, nbView, &pSchP, &NbCol);
	if (b == 0)
	   /* erreur */
	   AffPaveDebug (pAb);
	else if (pP->AbElement->ElPageNumber < NbCol)
	   /* la nouvelle marque doit etre une colonne */
	  {
	     pEl1->ElPageType = ColComputed;
	     pEl1->ElPageNumber = pP->AbElement->ElPageNumber + 1;
	  }
	else
	  {
	     /* il faut creer une nouvelle marque de page, */
	     pEl1->ElPageType = PgComputed;
	     /* creer une nouvelle marque de colonnes ColGroup */
	     pElColG = NewSubtree (PageBreak + 1, rootEl->ElStructSchema,
			     pDoc, pEl->ElAssocNum, TRUE, TRUE, TRUE, TRUE);
	     /* insere l'element dans l'arbre abstrait */
	     InsertElementAfter (pEl1, pElColG);
	     pElColG->ElPageType = ColGroup;
	     pElColG->ElPageNumber = 0;
	     pElColG->ElViewPSchema = schView;
	     /* et creer une nouvelle marque de colonne gauche */
	     pElCol = NewSubtree (PageBreak + 1, rootEl->ElStructSchema,
			     pDoc, pEl->ElAssocNum, TRUE, TRUE, TRUE, TRUE);
	     /* insere l'element dans l'arbre abstrait */
	     InsertElementAfter (pElColG, pElCol);
	     pElCol->ElPageType = ColComputed;
	     pElCol->ElPageNumber = 1;
	     pElCol->ElViewPSchema = schView;
	  }
     }
   else
      pEl1->ElPageType = PgComputed;
   pEl1->ElViewPSchema = schView;
   if (pEl1->ElPageType == PgComputed
       || pEl1->ElPageType == PgBegin
       || pEl1->ElPageType == PgUser)
     {
	/* ajout au 6 dec 93 : compteur de page. Est-ce necessaire ? */
	/* cherche le compteur de page a appliquer */
	cpt = GetPageCounter (pElPage, schView, &pSchP);
	if (cpt == 0)		/* page non numerotee */
	   /* on entretient un compteur de pages pour pouvoir afficher un */
	   /* message indiquant la progression du formatage */
	   pEl1->ElPageNumber = pagesCounter;
	else			/* calcule le numero de page */
	   pEl1->ElPageNumber = CounterVal (cpt, pElPage->ElStructSchema, pSchP,
					    pElPage, schView);
     }
   /* envoie l'evenement ElemNew.Post */
#ifndef PAGINEETIMPRIME
   NotifySubTree (TteElemNew, pDoc, pElPage, 0);
#endif /* PAGINEETIMPRIME */
   NbBoxesPageHeaderToCreate = 0;
   /* code de creation des hauts de page supprime */

   /* on ne peut creer les paves de la nouvelle marque de page */
   /* que si tous les paves des elements suivants  */
   /* ont ete detruits */
   /* on tue tous les freres droits en remontant l'arbre jusqu'au */
   /* corps de page */
   pP = pEl->ElAbstractBox[nbView - 1];
   if (!ElemIsBefore && *origCutAbsBox != NULL)
      pP = (*origCutAbsBox)->AbElement->ElAbstractBox[nbView - 1];
   /* on detruit les paves a partir du premier pave qui */
   /* se trouve dans la page ou colonne courante (si il n'y */
   /* a pas de colonne, c'est le pave chaine a l'element */
   /* car il ne peut y avoir de duplique precedent sur une */
   /* colonne precedente pour cet element  */
   pP1 = pP;
   if (ElemIsBefore)
     {				/* destruction des paves de l'element pEl */
	pP2 = pP1;
	while (pP2 != NULL && pP2->AbElement == pEl)
	  {
	     SetDeadAbsBox (pP2);
	     ApplyRefAbsBoxSupp (pP2, &PavR, pDoc);
	     if (pP2->AbNext == NULL)
		pP1 = pP2;	/* pP1 = dernier pave non null */
	     pP2 = pP2->AbNext;
	  }
     }
   else
     {
	while (pP1->AbNext != NULL
	       && pP1->AbNext->AbElement == pP->AbElement)
	   pP1 = pP1->AbNext;	/* on saute les paves de l'element */
	while (pP1->AbNextRepeated != NULL)
	   pP1 = pP1->AbNextRepeated;
	while (pP1->AbNext != NULL
	       && pP1->AbNext->AbElement == pP->AbElement)
	   pP1 = pP1->AbNext;	/* on saute les derniers paves de pres */
	/* mais en laissant pP1 non nul */
     }
   /* on tue tous les paves de presentation a droite */
   /* sauf ceux des pages et des colonnes */
   /* en mettant a jour le booleen AbTruncatedTail */
   KillPresRight (pP1, pDoc);
   /* on tue tous les paves a droite en remontant l'arbre */
   /* on cherche d'abord le premier pave a detruire */
   found = FALSE;
   pP = pP1;
   pP1 = NULL;
   while (!found && pP != NULL)
      if (pP->AbNext != NULL)
	{
	   pP1 = pP->AbNext;
	   found = TRUE;
	}
      else
	 pP = pP->AbEnclosing;
   if (pP1 != NULL && pP1->AbElement->ElTypeNumber == PageBreak + 1
       && (pP1->AbElement->ElPageType == PgBegin
	   || pP1->AbElement->ElPageType == PgComputed
	   || pP1->AbElement->ElPageType == PgUser))
      /* et si on est sur une colonne, on detruira bien la colonne */
      /* suivante */
      /* si on est sur une page, on saute tous ses paves */
      /* remarque : pP = pP1->AbPrevious */
      while (pP1 != NULL && pP1->AbElement == pP->AbElement)
	 pP1 = pP1->AbNext;
   if (pP1 != NULL && pP1->AbEnclosing != NULL)
      /* on ne detruit pas la racine */
      DestrAbbNext (pP1, pDoc);
   /* cree les paves de l'element Marque de Page qu'on vient */
   /* d'inserer */

   /* creation par appel de AbsBoxesCreate et non CreateNewAbsBoxes car il n'y */
   /* a plus de localite dans l'i.a. (les paves de page sont sous */
   /* la racine) */
   if (AssocView (pEl))
     {
	pRootEl = pDoc->DocAssocRoot[pEl->ElAssocNum - 1];
	pRoot = pRootEl->ElAbstractBox[0];
     }
   else
     {
	pRootEl = pDoc->DocRootElement;
	pRoot = pRootEl->ElAbstractBox[nbView - 1];
     }
   /* on nettoie d'abord l'image abstraite des paves morts */
   h = -1;			/* changement de signification de la valeur de h */
   bool = ChangeConcreteImage (frame, &h, pRoot);
   FreeDeadAbstractBoxes (pRoot);
   /* appel de AbsBoxesCreate */
   /* TODO : a mettre en coherence ->CreateNewAbsBoxes pour appel ApplRegleRet */
   pPa1 = AbsBoxesCreate (pElPage, pDoc, nbView, TRUE, TRUE, &complete);
   ApplDelayedRule (pElPage, pDoc);
   /* si on a cree  une marque colonne groupees, on cree son pave */
   if (pElColG != NULL)
     {
	pPa1 = AbsBoxesCreate (pElColG, pDoc, nbView, TRUE, TRUE, &complete);
	ApplDelayedRule (pElColG, pDoc);
     }
   if (pElCol != NULL)
     {
	pPa1 = AbsBoxesCreate (pElCol, pDoc, nbView, TRUE, TRUE, &complete);
	ApplDelayedRule (pElCol, pDoc);
	/* on a cree aussi une marque colonne, on cree son pave */
     }
   /* AbstractBox modifie au niveau de la racine */
   /* signale ces paves au Mediateur, en faisant reevaluer */
   /* TODO utile ? ... a supprimer */
   /* la coupure de page pour le cas ou on traite des colonnes */
   ChangeRHPage (pRoot, pDoc, nbView);
   RealPageHeight = BreakPageHeight;
   bool = ChangeConcreteImage (frame, &RealPageHeight, pRoot);
   pPa1 = pElPage->ElAbstractBox[nbView - 1];
   /* tous les paves de la page prec ne sont plus marques surpage et horspage */
   /* inutile .... et incomplet: il faudrait faire toute la hierarchie */
   if (pPa1 == NULL)
      printf ("erreur insere marque : pas de pave page cree ", "\n");
   else
     {
	pPa1 = pPa1->AbPrevious;
	while (pPa1 != NULL)
	  {
	     pPa1->AbOnPageBreak = FALSE;
	     pPa1->AbAfterPageBreak = FALSE;
	     pPa1 = pPa1->AbPrevious;
	  }
     }
   /* pb car lors de la creation de l'ia pour la nouvelle marque de page */
   /* et pour les elements suivants, pAb a change : c'est un descendant */
   /* du pave de la marque de page (il est dans la nouvelle page) */
   /* le code ci-dessous est-il a changer ? ... supprime  ! */
}				/* fin cas ou pP n'est pas une marquePage */

#else  /* __COLPAGE__ */
   /* remplit l'element page cree' */
   pEl1->ElPageType = PgComputed;
   pEl1->ElViewPSchema = schView;
   /* cherche le compteur de page a appliquer */
   cpt = GetPageCounter (pElPage, schView, &pSchP);
   if (cpt == 0)		/* page non numerotee */
      /* on entretient un compteur de pages pour pouvoir afficher un */
      /* message indiquant la progression du formatage */
     {
	pagesCounter++;
	pEl1->ElPageNumber = pagesCounter;
     }
   else				/* calcule le numero de page */
      pEl1->ElPageNumber = CounterVal (cpt, pElPage->ElStructSchema, pSchP,
				       pElPage, schView);
   /* envoie l'evenement ElemNew.Post */
#ifndef PAGINEETIMPRIME
   NotifySubTree (TteElemNew, pDoc, pElPage, 0);
#endif /* PAGINEETIMPRIME */

   /* Si la page precedente prevoit des elements associes en haut de */
   /* page, on peut les creer maintenant */
   if (NbBoxesPageHeaderToCreate > 0)
      /* cherche d'abord la boite du filet de separation de pages: */
      /* c'est la premiere boite contenue dans la boite de saut de page */
      /* qui n'est pas une boite de presentation. */
     {
	if (WorkingPage->ElAbstractBox[nbView - 1] != NULL)
	  {
	     pP1 = WorkingPage->ElAbstractBox[nbView - 1]->AbFirstEnclosed;
	     stop = FALSE;
	     do
		if (pP1 == NULL)
		   stop = TRUE;
		else if (!pP1->AbPresentationBox)
		   stop = TRUE;
		else
		   pP1 = pP1->AbNext;
	     while (!(stop));
	     savePageAbsBox = WorkingPage->ElAbstractBox[nbView - 1];
	     WorkingPage->ElAbstractBox[nbView - 1] = pP1;
	     PageHeaderRefAssoc = NULL;
	     /* au retour de CrAbsBoxesPres, */
	     /* PageHeaderRefAssoc pointera sur la premiere reference */
	     /* qui designe le premier element associe' mis dans la */
	     /* boite de haut de page */
	     topPageAbsBox = CrAbsBoxesPres (WorkingPage, pDoc, PageCreateRule,
				  WorkingPage->ElStructSchema, NULL, nbView,
					     PageSchPresRule, FALSE, TRUE);
	     WorkingPage->ElAbstractBox[nbView - 1] = savePageAbsBox;
	     if (topPageAbsBox == NULL)
		PageHeaderRefAssoc = NULL;
	     else
		/* signale ces paves au Mediateur, sans faire reevaluer la coupure de page. */
	       {
		  h = 0;
		  (void) ChangeConcreteImage (frame, &h, topPageAbsBox);
	       }
	  }
	NbBoxesPageHeaderToCreate = 0;
     }
   /* cree les paves de l'element Marque de Page qu'on vient */
   /* d'inserer, mais sans les paves des eventuels elements associes */
   /* qui s'affichent en haut de page. Ceux-ci ne pourront etre crees */
   /* que lorsque la marque de page suivante sera placee, puisque les */
   /* elements associes a placer en haut de page sont ceux qui sont */
   /* designes par une reference qui se trouve entre cette marque de */
   /* page et la suivante. */
   pElPage->ElAssocHeader = FALSE;
   /* traitement de l'insertion des pages dans les structures avec coupures speciales */
   cut = FALSE;		/* a priori pas de coupure effectuee par l'exception */
   if (ThotLocalActions[T_insertpage] != NULL)
      (*ThotLocalActions[T_insertpage]) (pElPage, pDoc, nbView, &cut);
   if (!cut)
      CreateNewAbsBoxes (pElPage, pDoc, nbView);
   if (!AssocView (pEl))
      modifAbsBox = pDoc->DocViewModifiedAb[nbView - 1];
   else
      modifAbsBox = pDoc->DocAssocModifiedAb[pEl->ElAssocNum - 1];
   /* signale ces paves au Mediateur, sans faire reevaluer la coupure de page. */
   if (modifAbsBox != NULL)
     {
	h = 0;
	(void) ChangeConcreteImage (frame, &h, modifAbsBox);
	pPa1 = pElPage->ElAbstractBox[nbView - 1];
	pPa1->AbOnPageBreak = FALSE;
	pPa1->AbAfterPageBreak = FALSE;
	/* les paves devant lesquels on a mis la marque de page ne doivent */
	/* plus etre traites. On les marque hors page. */
	stop = FALSE;
	do
	  {
	     pAb->AbOnPageBreak = FALSE;
	     if (pAb == pP)
		stop = TRUE;
	     else
		pAb = pAb->AbEnclosing;
	  }
	while (!(stop));
     }
#endif /* __COLPAGE__ */
   return pElPage;
}


/*----------------------------------------------------------------------
  MoveCut cherche dans les paves du sous-arbre de racine pAb, en
  appliquant les regles de condition de coupure NoBreak1 (ou NoBreak2,
  selon NoBr1), le premier pave qui necessite un deplacement (vers le haut)
  de la frontiere de page. Retourne la nouvelle frontiere de page, en
  points typographiques, ou 0 si la coupure de page convient.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          MoveCut (PtrAbstractBox pAb, boolean NoBr1, int schView)
#else  /* __STDC__ */
static int          MoveCut (pAb, NoBr1, schView)
PtrAbstractBox      pAb;
boolean             NoBr1;
int                 schView;
#endif /* __STDC__ */
{
  int                 ret, High, PosV, cutChar, Min, i;
  PtrPRule            pRNoBr1, pRNoBr2;
  PtrAttribute        pA1, pA2;
  boolean             cutAbsBox;
  PtrPRule            pRe1;

  ret = 0;
  /* cherche si la coupure de page convient au pave */
  if (pAb->AbOnPageBreak)
    if (!Divisible (pAb, &pRNoBr1, &pA1, &pRNoBr2, &pA2, schView))
      /* le pave est insecable, on coupe avant ce pave */
      {
	SetPageHeight (pAb, TRUE, &High, &PosV, &cutChar);
	ret = PosV;
      }
    else
      /* le pave' n'est pas insecable */ if (NoBr1)
	/* verifie si la regle NoBreak1 est respectee */
	{
	  if (pRNoBr1 != NULL)
	    /* une regle NoBreak1 doit etre appliquee a l'element */
	    /* on ne l'applique que si le pave est complet */
	    {
	      if (pAb->AbLeafType != LtCompound)
		cutAbsBox = FALSE;
	      else if (pAb->AbInLine)
		cutAbsBox = FALSE;
	      else
		cutAbsBox = pAb->AbTruncatedHead;
	      /* si la boite est eclatee, on ignore la regle NoBreak1 */
	      if (pAb->AbBox != NULL)
		if (pAb->AbBox->BxType == BoGhost)
		  cutAbsBox = TRUE;
	      if (!cutAbsBox)
		{
		  pRe1 = pRNoBr1;
		  /* demande au Mediateur la position du haut du pave dans la page */
		  SetPageHeight (pAb, TRUE, &High, &PosV, &cutChar);
		  /* calcule la hauteur minimum avant coupure, en points typographiques */
		  if (pRe1->PrMinAttr)
		    i = AttrValue (pA1);
		  else
		    i = pRe1->PrMinValue;
		  Min = PixelValue (i, pRe1->PrMinUnit, pAb);
		  if (RealPageHeight - PosV < Min)
		    /* la hauteur minimum n'y est pas, on deplace la */
		    /* hauteur de page en haut du pave */
		    ret = PosV;
		}
	    }
	}
      else
	/* verifie si la regle NoBreak2 est respectee */ if (pRNoBr2 != NULL)
	  /* une regle NoBreak2 doit etre appliquee a l'element */
	  /* on ne l'applique que si le pave est complet */
	  {
	    if (pAb->AbLeafType != LtCompound)
	      cutAbsBox = FALSE;
#ifdef __COLPAGE__
	    /* si un pave est mis en ligne, il peut etre coupe. */
	    /* ce cas est retire */
#else  /* __COLPAGE__ */
	    else if (pAb->AbInLine)
	      cutAbsBox = FALSE;
#endif /* __COLPAGE__ */
	    else
	      cutAbsBox = pAb->AbTruncatedTail;
	    /* si la boite est eclatee, on ignore la regle NoBreak2 */
	    if (pAb->AbBox != NULL)
	      if (pAb->AbBox->BxType == BoGhost)
		cutAbsBox = TRUE;
	    if (!cutAbsBox)
	      {
		pRe1 = pRNoBr2;
		/* demande au Mediateur ou se place le pave dans la page */
		SetPageHeight (pAb, TRUE, &High, &PosV, &cutChar);
		/* calcule la hauteur minimum apres coupure, en points typographiques */
		if (pRe1->PrMinAttr)
		  i = AttrValue (pA2);
		else
		  i = pRe1->PrMinValue;
		Min = PixelValue (i, pRe1->PrMinUnit, pAb);
		if (PosV + High - RealPageHeight < Min)
		  /* la frontiere de page passe trop pres du bas du */
		  /* pave, on la ramene en haut de la zone autorisee */
		  ret = PosV + High - Min;
	      }
	  }
  if (ret == 0)
    /* la coupure convient au pave, cherche si elle convient aux paves englobes. */
    {
      pAb = pAb->AbFirstEnclosed;
#ifdef __COLPAGE__
      /* on saute les paves de colonnes pour arriver a la derniere */
      /* cf. procedure CoupSurPage de pos.c (lignes #800) */
      if (pAb != NULL)
	while (pAb->AbElement->ElTypeNumber == PageBreak + 1
	       && (pAb->AbElement->ElPageType == ColBegin
		   || pAb->AbElement->ElPageType == ColComputed
		   || pAb->AbElement->ElPageType == ColUser
		   || pAb->AbElement->ElPageType == ColGroup)
	       && pAb->AbNext != NULL)
	  pAb = pAb->AbNext;
#endif /* __COLPAGE__ */
      while (ret == 0 && pAb != NULL)
	{
	  ret = MoveCut (pAb, NoBr1, schView);
	  pAb = pAb->AbNext;
	}
    }
  return ret;
}


#ifdef __COLPAGE__
/*----------------------------------------------------------------------
   	SetMark place dans l'arbre de racine pAb les marques de	
   		page en fonction de la position des paves relativement	
   		a la limite de page					
    cette procedure n'est appelee que sur un pave d'element 
    et elle saut les paves de presentation.                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         SetMark (PtrAbstractBox pAb, PtrElement rootEl, PtrDocument pDoc, int schView, boolean * absBoxTooHigh, PtrAbstractBox * origCutAbsBox, int nbView, int frame, PtrElement * pPage)
#else  /* __STDC__ */
static void         SetMark (pAb, rootEl, pDoc, schView, absBoxTooHigh, origCutAbsBox, nbView, frame, pPage)
PtrAbstractBox      pAb;
PtrElement          rootEl;
PtrDocument         pDoc;
int                 schView;
boolean            *absBoxTooHigh;
PtrAbstractBox     *origCutAbsBox;
int                 nbView;
int                 frame;
PtrElement         *pPage;
#endif /* __STDC__ */
{
   int                 High, PosV, cutChar;
   boolean             done;
   PtrAbstractBox      pPa1;

   if (pAb != NULL && !pAb->AbPresentationBox)
     {
	pPa1 = pAb;
	if (pPa1->AbOnPageBreak)
	   /* le pave' est traverse' par la limite de page */
	   if (pPa1->AbFirstEnclosed == NULL)
	      /* c'est un pave' feuille */
	     {
		/* demande au mediateur sur quel caractere a lieu la coupure */
		/* (si ce n'est pas une feuille de texte, on placera la marque */
		/* de page avant le pave) */
		SetPageHeight (pAb, TRUE, &High, &PosV, &cutChar);
		if (cutChar <= 0)
		   /* place la marque de page avant le pave */
		   *pPage = InsertMark (pAb, frame, nbView,
					origCutAbsBox, absBoxTooHigh,
					schView, pDoc, rootEl);
		else if (cutChar >= pPa1->AbElement->ElTextLength)
		   /* la coupure tombe a la fin du pave */
		  {
		     *pPage = NULL;
		     pPa1->AbOnPageBreak = FALSE;
		  }
		else
		   /* coupe l'element de texte */
		  {
		     Cut (pPa1->AbElement, cutChar, pDoc, nbView);
		     pPa1->AbOnPageBreak = FALSE;
		     pAb = pPa1->AbNext;
		     pAb->AbAfterPageBreak = TRUE;
		     *pPage = InsertMark (pAb, frame, nbView,
					  origCutAbsBox, absBoxTooHigh,
					  schView, pDoc, rootEl);
		  }
	     }
	   else
	      /* ce n'est pas un pave feuille, on examine tous les paves */
	      /* englobes par ce pave' */
	      /* sauf si c'est un pave de colonne suivi d'autres colonnes */
	      /* on saute les paves de colonnes pour arriver a la derniere */
	      /* cf. procedure CoupSurPage de pos.c (lignes #800) */
	     {
		pAb = pAb->AbFirstEnclosed;
		while (pAb->AbElement->ElTypeNumber == PageBreak + 1
		       && (pAb->AbElement->ElPageType == ColBegin
			   || pAb->AbElement->ElPageType == ColComputed
			   || pAb->AbElement->ElPageType == ColUser
			   || pAb->AbElement->ElPageType == ColGroup)
		       && pAb->AbNext != NULL)
		   pAb = pAb->AbNext;
		done = FALSE;
		/* on s'arrete des qu'on a cree une marque de page */
		while (pAb != NULL && !done)
		  {
		     /* si pAb est un pave de presentation, on passe */
		     /* au pave suivant */
		     if (pAb->AbOnPageBreak && !pAb->AbPresentationBox)
			/* la frontiere de page traverse ce pave, on place une */
			/* marque de page a l'interieur */
		       {
			  SetMark (pAb, rootEl, pDoc, schView, absBoxTooHigh,
				   origCutAbsBox, nbView, frame, pPage);
			  done = *pPage != NULL;
		       }
		     else
			/* si le pave est un filet, on considere qu'il */
			/* n'est pas sur la limite. (page complete) */
			/* a voir si c'est necessaire !!! */
			if (pAb->AbOnPageBreak
			    && pAb->AbElement->ElTypeNumber == PageBreak + 1
			    && pAb->AbLeafType == LtGraphics)
		       {
			  pAb->AbOnPageBreak = FALSE;
			  *pPage = pPa1->AbElement;
			  /* on retourne l'element MP prec */
			  /* pour detruire l'i.a. correspondante */
		       }
		     else if (pAb->AbAfterPageBreak && !pAb->AbPresentationBox)
			if (!done)
			   /* c'est le premier pave englobe' au-dela de la */
			   /* frontiere, on pose une marque de page devant lui */
			  {
			     *pPage = InsertMark (pAb, frame, nbView,
						  origCutAbsBox, absBoxTooHigh, schView, pDoc, rootEl);
			     done = TRUE;
			  }
		     if (pAb != NULL && !done)
			pAb = pAb->AbNext;
		  }		/* fin du while */
	     }
     }
}

#else  /* __COLPAGE__ */

/*----------------------------------------------------------------------
  SetMark place dans l'arbre de racine pAb la marque de page en fonction
  de la position des paves relativement a la limite de page.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         SetMark (PtrAbstractBox pAb, PtrElement rootEl, PtrDocument pDoc, int schView, boolean * absBoxTooHigh, PtrAbstractBox * origCutAbsBox, int nbView, int frame, PtrElement * pPage)
#else  /* __STDC__ */
static void         SetMark (pAb, rootEl, pDoc, schView, absBoxTooHigh, origCutAbsBox, nbView, frame, pPage)
PtrAbstractBox      pAb;
PtrElement          rootEl;
PtrDocument         pDoc;
int                 schView;
boolean            *absBoxTooHigh;
PtrAbstractBox     *origCutAbsBox;
int                 nbView;
int                 frame;
PtrElement         *pPage;
#endif /* __STDC__ */
{
  int                 High, PosV, cutChar;
  boolean             done;
  PtrAbstractBox      pPa1;
  boolean             toCut;
  PtrAbstractBox      pCreator;

  if (pAb != NULL)
    {
      pPa1 = pAb;
      if (pPa1->AbOnPageBreak)
	/* le pave' est traverse' par la limite de page */
	if (pPa1->AbFirstEnclosed == NULL)
	  /* c'est un pave' feuille */
	  {
	    /* a priori on va le couper en deux */
	    toCut = TRUE;
	    if (pAb->AbPresentationBox)
	      /* c'est un pave de presentation */
	      {
		/* on cherche le pave qui l'a cree' pour connaitre sa regle */
		/* de creation */
		pCreator = pAb->AbElement->ElAbstractBox[pAb->AbDocView - 1];
		while (pCreator->AbPresentationBox)
		  pCreator = pCreator->AbNext;
		/* on cherche la regle de creation */
		if (TypeCreatedRule (pDoc, pCreator, pAb) == FnCreateWith)
		  /* c'est une regle CreateWith, on ne fait rien */
		  toCut = FALSE;
	      }
	    if (toCut)
	      {
		/* demande au mediateur sur quel caractere a lieu la coupure */
		/* (si ce n'est pas une feuille de texte, on placera la marque */
		/* de page avant le pave) */
		SetPageHeight (pAb, TRUE, &High, &PosV, &cutChar);
		if (cutChar <= 0)
		  /* place la marque de page avant le pave */
		  *pPage = InsertMark (pAb, frame, nbView, origCutAbsBox,
				       absBoxTooHigh, schView, pDoc, rootEl);
		else if (cutChar >= pPa1->AbElement->ElTextLength)
		  /* la coupure tombe a la fin du pave */
		  pPa1->AbOnPageBreak = FALSE;
		else
		  /* coupe l'element de texte */
		  {
		    Cut (pPa1->AbElement, cutChar, pDoc, nbView);
		    pPa1->AbOnPageBreak = FALSE;
		    pAb = pPa1->AbNext;
		    pAb->AbAfterPageBreak = TRUE;
		    *pPage = InsertMark (pAb, frame, nbView,
					 origCutAbsBox, absBoxTooHigh,
					 schView, pDoc, rootEl);
		  }
	      }
	  }
	else
	  /* ce n'est pas un pave feuille, on examine tous les paves */
	  /* englobes par ce pave' */
	  {
	    pAb = pAb->AbFirstEnclosed;
	    done = FALSE;
	    while (pAb != NULL && !done)
	      {
		if (pAb->AbOnPageBreak)
		  /* la frontiere de page traverse ce pave, on place une */
		  /* marque de page a l'interieur */
		  {
		    SetMark (pAb, rootEl, pDoc, schView, absBoxTooHigh,
			     origCutAbsBox, nbView, frame, pPage);
		    done = (*pPage != NULL);
		  }
		else if (pAb->AbAfterPageBreak)
		  if (!done)
		    /* c'est le premier pave englobe' au-dela de la */
		    /* frontiere, on pose une marque de page devant lui */
		    {
		      *pPage = InsertMark (pAb, frame, nbView, origCutAbsBox,
					   absBoxTooHigh, schView, pDoc, rootEl);
		      done = TRUE;
		      pAb = NULL;
		    }
		if (pAb != NULL)
		  pAb = pAb->AbNext;
	      }
	  }
    }
}
#endif /* __COLPAGE__ */

/*----------------------------------------------------------------------
  SetPage place la marque de page en respectant la hauteur de page
  demandee et les conditions de coupure des paves de la page.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         SetPage (PtrElement * pPage, int frame, PtrAbstractBox * origCutAbsBox, boolean * absBoxTooHigh, PtrDocument pDoc, int schView, int nbView, PtrElement rootEl)
#else  /* __STDC__ */
static void         SetPage (pPage, frame, origCutAbsBox, absBoxTooHigh, pDoc, schView, nbView, rootEl)
PtrElement         *pPage;
int                 frame;
PtrAbstractBox     *origCutAbsBox;
boolean            *absBoxTooHigh;
PtrDocument         pDoc;
int                 schView;
int                 nbView;
PtrElement          rootEl;
#endif /* __STDC__ */
{
  int                 turn, newPageHight, oldPageHight;
  boolean             NoBr1;

  /* explore deux fois l'arbre des paves a la recherche des paves */
  /* traverses par la frontiere de page et decale la frontiere de page */
  /* vers le haut pour respecter les regles NoBreak1 et NoBreak2 des */
  /* paves traverses par la frontiere de page. Le premier tour traite */
  /* les regles NoBreak2 et le 2eme tour les regles NoBreak1. */
  for (turn = 1; turn <= 2; turn++)
    {
      NoBr1 = turn == 2;
      /* on traite les regles NoBreak1 au 2eme tour */
      do
	/* on commence par la racine de la vue */
	{
	  newPageHight = MoveCut (rootEl->ElAbstractBox[nbView - 1], NoBr1, schView);
	  if (newPageHight != 0)
	    {
	      oldPageHight = RealPageHeight;
	      RealPageHeight = newPageHight;
	      /* demande au mediateur de positionner la nouvelle */
	      /* frontiere de page par rapport a tous les paves de la vue */
	      (void) SetPageBreakPosition (rootEl->ElAbstractBox[nbView - 1], &RealPageHeight);
	      /* verifie que le mediateur a effectivement change' la */
	      /* hauteur de page */
	      if (RealPageHeight == oldPageHight)
		/* pas de changement, inutile de continuer sinon ca va */
		/* boucler indefiniment */
		newPageHight = 0;
	    }
	}
      while (!(newPageHight == 0));
    }
  /* place la marque de page dans l'arbre abstrait */
  SetMark (rootEl->ElAbstractBox[nbView - 1], rootEl, pDoc, schView,
	   absBoxTooHigh, origCutAbsBox, nbView, frame, pPage);
}


/*----------------------------------------------------------------------
  PutMark pour la vue de numero nbView, dans le document pDoc,	
  insere dans l'arbre abstrait de racine rootEl un	
  element Marque de Page a la frontiere de page et detruit
  tous les paves qui precedent cet element.		
  Retourne 'vrai' si l'image restante est plus petite qu'une page.
  Met a jour les pointeurs pT:pPageTraitee et pAT:pPageATraiter qui sont
  deux parametres en plus
  Retourne l'element marque page creee.
  ----------------------------------------------------------------------*/
#ifdef __COLPAGE__
#ifdef __STDC__
static void         PutMark (PtrElement rootEl, int nbView, PtrDocument pDoc, int frame, PtrAbstractBox * pT, PtrAbstractBox * pAT, int schView)
#else  /* __STDC__ */
static void         PutMark (rootEl, nbView, pDoc, frame, pT, pAT, schView)
PtrElement          rootEl;
int                 nbView;
PtrDocument         pDoc;
int                 frame;
PtrAbstractBox     *pT;
PtrAbstractBox     *pAT;
int                 schView;
#endif /* __STDC__ */
#else  /* __COLPAGE__ */
#ifdef __STDC__
static PtrElement   PutMark (PtrElement rootEl, int nbView, PtrDocument pDoc, int frame, int schView)
#else  /* __STDC__ */
static PtrElement   PutMark (rootEl, nbView, pDoc, frame, schView)
PtrElement          rootEl;
int                 nbView;
PtrDocument         pDoc;
int                 frame;
int                 schView;
#endif /* __STDC__ */
#endif /* __COLPAGE__ */
{
   PtrAbstractBox      pAb;
   PtrAbstractBox      origCutAbsBox;
   PtrElement          pPage;
   boolean             absBoxTooHigh;
#ifndef __COLPAGE__
   PtrElement          pElLib;
   PtrAbstractBox      previousAbsBox, RedispAbsBox;
   int                 High, PosV, putVThread, cutChar, h, dh, normalPageHeight;
   boolean             stop;
#endif /* __COLPAGE__ */

   pPage = NULL;
   absBoxTooHigh = FALSE;
   /* on recherche le pave de plus haut niveau qui soit insecable et */
   /* traverse' par la frontiere normale de page. On aura besoin de */
   /* connaitre ce pave' s'il est plus haut qu'une page et qu'on doit */
   /* inserer une marque de page apres lui. */
   origCutAbsBox = NULL;	/* on n'a pas encore trouve' ce pave' */
   pAb = rootEl->ElAbstractBox[nbView - 1];	/* on commence par le pave racine */
   do
      if (pAb->AbAfterPageBreak)
	 origCutAbsBox = pAb;	/* pave au-dela de la limite de page */
      else if (pAb->AbOnPageBreak)
	{
	  if (!pAb->AbAcceptPageBreak)
	    /* pave insecable */
	    origCutAbsBox = pAb;

	  else if (pAb->AbFirstEnclosed == NULL)
	    origCutAbsBox = pAb;	/* pave feuille */
	  else
#ifdef __COLPAGE__
	    /* on saute les paves de colonnes pour arriver a la derniere */
	    /* cf. procedure CoupSurPage de pos.c (lignes #800) */
	    {
	      pAb = pAb->AbFirstEnclosed;
	      while (pAb->AbElement->ElTypeNumber == PageBreak + 1
		     && (pAb->AbElement->ElPageType == ColBegin
			 || pAb->AbElement->ElPageType == ColComputed
			 || pAb->AbElement->ElPageType == ColUser
			 || pAb->AbElement->ElPageType == ColGroup)
		     && pAb->AbNext != NULL)
		pAb = pAb->AbNext;
	    }
#else  /* __COLPAGE__ */
	   pAb = pAb->AbFirstEnclosed;
#endif /* __COLPAGE__ */
	}
      else			/* le pave est avant la limite de page */
	 pAb = pAb->AbNext;	/* passe au pave suivant */
   while (!(origCutAbsBox != NULL || pAb == NULL));
   /* place les marques de page sans tenir compte des boites de haut et de */
   /* bas de page de hauteur variable (notes de bas de page par exemple) */
   /* dans la version de Vincent, ChangeConcreteImage est appelee avec une */
   /* Hauteur de Page fixe : maintenamt, on va chercher a la mettre a */
   /* jour a chaque fois que on ajoutera un elt associe en bas */
   /* de page ; ce sera fait dans AbsBoxesCreate */
   SetPage (&pPage, frame, &origCutAbsBox, &absBoxTooHigh, pDoc, schView, nbView, rootEl);
#ifdef __COLPAGE__
   /* le cas des pages trop hautes est supprime */
   if (pPage != NULL
       && pPage->ElPageType == PgComputed)
      /* on met a jour les pointeurs pT (page Traitee = page precedente) */
      /* et pAT (page A Traiter = premier pave de la page suivante) */
      /* avant de detruire les paves de la page precedente */
      if (*pT != NULL)
	{
	   *pAT = (*pT)->AbNext;
	   /* si pas d'erreur, pAT point sur le premier pave */
	   /* de l'element marque page qui vient d'etre cree, */
	   /* a savoir pPage */
	   /* ce n'est plus toujours vrai avec les colonnes */
	   if ((*pAT)->AbElement != pPage)
	      printf ("erreur SetMark : les paves MP crees sont mal chaines", "\n");
	   *pT = NULL;		/* le pave correspondant risque d'etre detruit */
	}
#else  /* __COLPAGE__ */
   if (pPage != NULL)
      /* on a insere' au moins une marque de page dans l'arbre abstrait */
      /* On verifie que la page n'est pas trop haute (il peut y avoir des */
      /* boites de haut ou de bas de page qui augmentent la hauteur de page). */
      /* Inutile d'essayer de reduire la hauteur de la page s'il y a un */
      /* pave insecable plus haut que la page */
      if (!absBoxTooHigh)
	 /* cherche d'abord la boite du filet de separation de pages: c'est */
	 /* la premiere boite contenue dans la boite de page qui n'est */
	 /* pas une boite de presentation. */
	{
	   if (pPage->ElAbstractBox[nbView - 1] != NULL)
	      pAb = pPage->ElAbstractBox[nbView - 1]->AbFirstEnclosed;
	   else
	      pAb = NULL;
	   stop = FALSE;
	   do
	      if (pAb == NULL)
		 stop = TRUE;
	      else if (!pAb->AbPresentationBox)
		 stop = TRUE;
	      else
		 pAb = pAb->AbNext;
	   while (!(stop));
	   if (pAb == NULL)
	      putVThread = 0;
	   else
	      /* demande au mediateur la position verticale de cette boite filet */
	      SetPageHeight (pAb, TRUE, &High, &putVThread, &cutChar);
	   /* verifie la hauteur de la page */
	   if (putVThread > RealPageHeight + PageFooterHeight)
	      /* la page est trop haute */
	      /* dh: hauteur qui depasse de la page standard */
	     {
		dh = putVThread - PageHeight - PageFooterHeight;
		/* cherche le pave qui precede la marque de page */
		previousAbsBox = pPage->ElAbstractBox[nbView - 1];
		stop = FALSE;
		do
		   if (previousAbsBox == NULL)
		      stop = TRUE;
		   else if (previousAbsBox->AbPrevious != NULL)
		      stop = TRUE;
		   else
		      previousAbsBox = previousAbsBox->AbEnclosing;
		while (!(stop));
		if (previousAbsBox != NULL)
		   /* la page n'est pas vide */
		   /* sauve la hauteur de page normale */
		  {
		     normalPageHeight = PageHeight;
		     /* reduit la hauteur de page */
		     PageHeight = RealPageHeight - dh;
		     if (PageHeaderRefAssoc != NULL)
			if (PageHeaderRefAssoc->ElAbstractBox[nbView - 1] != NULL)
			   /* la page deborde parce qu'on vient de creer dans la boite */
			   /* de saut de page precedente une boite de haut de page */
			   /* contenant des elements associes. On deplace la marque de */
			   /* page courante juste au-dessus de la premiere reference au */
			   /* premier element associe' du haut de page. */
			  {
			     SetPageHeight (PageHeaderRefAssoc->ElAbstractBox[nbView - 1], TRUE,
					    &High, &PosV, &cutChar);
			     if (normalPageHeight < PosV)
				PageHeight = PosV;
			     else
				PageHeight = normalPageHeight;
			  }
		     if (PageHeight < HMinPage)
			PageHeight = HMinPage;
		     /* detruit le saut de page et ses paves */
		     DestroyAbsBoxesView (pPage, pDoc, FALSE, nbView);
		     /* traitement des elements demandant des coupures speciales */
		     if (ThotLocalActions[T_deletepageab] != NULL)
			(*ThotLocalActions[T_deletepageab])
			   (pPage, pDoc, nbView);
		     if (WorkingPage == pPage)
			NbBoxesPageHeaderToCreate = 0;
		     /* signale les paves morts au Mediateur */
		     if (!AssocView (rootEl))
			RedispAbsBox = pDoc->DocViewModifiedAb[nbView - 1];
		     else
			RedispAbsBox = pDoc->DocAssocModifiedAb[rootEl->ElAssocNum - 1];
		     /*RealPageHeight = PageHeight;*/
		     (void) ChangeConcreteImage (frame, &RealPageHeight, RedispAbsBox);
		     /* libere tous les paves morts de la vue */
		     FreeDeadAbstractBoxes (pAb);
		     /* detruit la marque de page a liberer dans l'arbre abstrait */
		     SuppressPageMark (pPage, pDoc, &pElLib);
		     /* signale au Mediateur les paves morts par suite de */
		     /* fusion des elements precedent et suivant les marques */
		     /* supprimees. */
		     if (!AssocView (rootEl))
			RedispAbsBox = pDoc->DocViewModifiedAb[nbView - 1];
		     else
			RedispAbsBox = pDoc->DocAssocModifiedAb[rootEl->ElAssocNum - 1];
		     if (RedispAbsBox != NULL)
		       {
			  h = RealPageHeight;
			  (void) ChangeConcreteImage (frame, &h, RedispAbsBox);
		       }
		     /* libere les elements rendus inutiles par les fusions */
		     DeleteElement (&pElLib);
		     pPage = NULL;
		     pagesCounter--;
		     /* on place les marques de page plus haut */
		     SetPage (&pPage, frame, &origCutAbsBox, &
			      absBoxTooHigh, pDoc, schView, nbView, rootEl);
		     /* retablit la hauteur de page */
		     PageHeight = normalPageHeight;
		  }
	     }
	}
   return pPage;
#endif /* __COLPAGE__ */
}


#ifdef __COLPAGE__
/*----------------------------------------------------------------------
   PageHeaderFooter met a jour les variables BreakPageHeight et PageFooterHeight
   		selon le type de page auquel appartient l'element	
   		Marque Page pointe par pElPage.				
   		Vue indique le numero de la vue pour laquelle on	
   		construit des pages.					
    schView indique le numero de la vue dans le schema.      
    BreakPageHeight : variable globale de thot (partagee    
    entre page.c et crimabs.c).		     		
    bottomPageHeightRef : variable globale du module page.	
    Cette procedure ne fait rien si pElPage est une marque  
    colonne.                                                
  ----------------------------------------------------------------------*/


#ifdef __STDC__
static void         PageHeaderFooter (PtrElement pElPage, int view, int schView, int frame, PtrDocument pDoc)

#else  /* __STDC__ */
static void         PageHeaderFooter (pElPage, view, schView, frame, pDoc)
PtrElement          pElPage;
int                 view;
int                 schView;
int                 frame;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrPRule            pR;
   PtrPSchema          pSchP;
   boolean             stop;
   PtrPSchema          pSc1;
   PtrAbstractBox      pP, pAb, pCorps;
   int                 High, PosV, cutChar;	/* pour calcul page */
   int                 h;
   boolean             bool, New;
   PtrPSchema          pSPR;
   PtrSSchema          pSS;
   PtrAttribute        pAttr;
   PtrPRule            pRuleDimV, pRStd;
   AbDimension        *pPavD1;
   PtrElement          pNext;

   /* on ne fait rien si pElPage est une marque colonne car pour */
   /* l'instant, on ne permet pas la creation de haut ou bas de */
   /* colonnes */
   if (pElPage->ElPageType == PgBegin
       || pElPage->ElPageType == PgComputed
       || pElPage->ElPageType == PgUser)
     {
	if (pElPage->ElPageType == PgBegin)
	  {
	     /* Nouveau type de page, on determine la hauteur des pages */
	     pR = NULL;
	     /* il faut rechercher la regle sur l'element suivant */
	     /* car les elements marque page debut sont places AVANT les elements */
	     /* qui portent la regle page */
	     if (pElPage->ElNext != NULL)
	       {
		  pNext = pElPage->ElNext;
		  /* on saute les eventuelles marques de colonnes */
		  /* ou de page (pour d'autres vues par exemple ?) */
		  while (pNext != NULL
			 && pNext->ElTypeNumber == PageBreak + 1)
		     pNext = pNext->ElNext;
		  /* on cherche uniquement sur pPsuiv car normalement l'element */
		  /* marque page debut a ete place juste devant l'element qui */
		  /* portait la regle page correspondante */
		  if (pNext != NULL)
		     pR = GetPageRule (pNext, pElPage->ElViewPSchema, &pSchP);
	       }
	     if (pR == NULL && pElPage->ElParent != NULL)
		/* si pNext ne portait pas de regle, */
		/* l'element englobant porte-t-il une regle page ? */
		pR = GetPageRule (pElPage->ElParent,
				  pElPage->ElViewPSchema, &pSchP);

	     if (pR != NULL)	/* on a trouve la regle page */
	       {
		  pSc1 = pSchP;
		  /* bottomPageHeightRef = Hauteur minimum du bas de page */
		  /* correspond a la taille des elements fixes places en bas de */
		  /* page */
		  bottomPageHeightRef = pSc1->PsPresentBox[pR->PrPresBox[0] - 1].PbFooterHeight;
		  /*  topPageHeightRef = pSc1->PsPresentBox[pR->PrPresBox[0]-1].PbHeaderHeight; */
		  topPageHeightRef = bottomPageHeightRef;	/* temporaire */
		  /* code inutile : le plus souvent, le compilateur ne peut pas  */
		  /* connaitre la hauteur des hauts et bas de page car ils sont  */
		  /* composes de plusieurs boites de presentation */
		  /* il faut attendre que le mediateur ait calcule leur hauteur */
		  /* pour pouvoir avoir une hauteur de reference de la page */
		  /* cherche la regle de hauteur de la boite page */
		  /* c'est maintenamt la hauteur du corps */
		  pR = pSc1->PsPresentBox[pR->PrPresBox[0] - 1].PbFirstPRule;
		  stop = FALSE;
		  do
		     if (pR == NULL)
			stop = TRUE;
		     else if (pR->PrType >= PtHeight)
			stop = TRUE;
		     else
			pR = pR->PrNextPRule;
		  while (!(stop));
		  if (pR != NULL)
		     if (pR->PrType == PtHeight)
			if (!pR->PrDimRule.DrPosition)
			   WholePageHeight = pR->PrDimRule.DrValue +
			      topPageHeightRef + bottomPageHeightRef;
		  /* WholePageHeight = hauteur max totale de la page */
		  /* definie par l'utilisateur ; exemple A4 = 29.7 cm */
		  /* toujours exprimee en unite fixe (verifie par le compilo) */
	       }
	  }			/* fin pElPage->ElPageType == PgBegin */

	/* si les paves Haut et Bas ont ete crees, on fait reevaluer */
	/* leur hauteur par le mediateur pour positionner correctement */
	/* les differentes variables de hauteur */

	if (pElPage->ElAbstractBox[view - 1] != NULL)
	  {
	     /* l'element a un pave dans la vue. On recherche ses paves */
	     /* haut et bas */
	     pP = pElPage->ElAbstractBox[view - 1];
	     if (pP->AbPresentationBox)
	       {
		  /* il y a un haut de page : on evalue sa hauteur */
		  pAb = pP;
		  h = -1;	/* changement de signification de h */
		  /* on signale au mediateur les paves (au cas ou il ne les */
		  /* ait pas encore vus) */
		  /* on appelle Modifvue a partir du pave haut de page */
		  bool = ChangeConcreteImage (frame, &h, pAb);
		  /* calcul de la hauteur du pave haut ou bas de page */
		  SetPageHeight (pAb, TRUE, &High, &PosV, &cutChar);
		  topPageHeightRef = High;
	       }
	     /* on saute le corps de page pour voir s'il y a un bas de page */
	     while (pP->AbPresentationBox)
		pP = pP->AbNext;
	     pCorps = pP;	/* on memorise le corps de page */
	     if (pP->AbNext != NULL
		 && pP->AbNext->AbElement == pElPage
		 && pP->AbNext->AbLeafType == LtCompound)	/* pas filet */
	       {
		  /* il y a un bas : on evalue sa hauteur */
		  pAb = pP->AbNext;
		  h = -1;	/* changement de signification de h */
		  /* on signale au mediateur les paves (au cas ou il ne les */
		  /* ait pas encore vus) */
		  /* on appelle Modifvue a partir du pave haut de page */
		  bool = ChangeConcreteImage (frame, &h, pAb);
		  /* calcul de la hauteur du pave haut ou bas de page */
		  SetPageHeight (pAb, TRUE, &High, &PosV, &cutChar);
		  bottomPageHeightRef = High;
	       }
	     /* mise a jour de la hauteur de reference de la page */
	     /* mise a jour de la hauteur du corps de page */
	     /* on met (ou on modifie) une regle de presentation specifique */
	     /* cherche d'abord la regle qui s'applique a l'element */
	     pRStd = GlobalSearchRulepEl (pElPage, &pSPR, &pSS, 0, NULL, schView,
					  PtHeight, FnAny, FALSE, TRUE, &pAttr);
	     /* cherche si l'element a deja une regle de hauteur specifique */
	     pRuleDimV = SearchPresRule (pElPage, PtHeight, 0, &New, pDoc, view);
	     if (New)
		/* on a cree' une regle de hauteur pour l'element */
	       {
		  pR = pRuleDimV->PrNextPRule;	/* on recopie la regle standard */
		  *pRuleDimV = *pRStd;
		  pRuleDimV->PrNextPRule = pR;
		  pRuleDimV->PrCond = NULL;	/* pas de condition associee */
		  pRuleDimV->PrViewNum = schView;	/* inutile ? */
	       }
	     /* change le parametre de la regle */
	     pRuleDimV->PrDimRule.DrValue =
		WholePageHeight - bottomPageHeightRef - topPageHeightRef;
	     /* pCorps = pave du corps de page */
	     /* applique la nouvelle regle specifique Verticale */
	     if (pRuleDimV != NULL)
		ApplyRule (pRuleDimV, pSPR, pCorps, pDoc, pAttr, &bool);
	     pPavD1 = &pCorps->AbHeight;
	     pPavD1->DimMinimum = TRUE;		/* regle de hauteur minimum */

	  }

	/* A chaque nouvel element Marque Page, on met a jour les */
	/* hauteurs significatives de la page */
	BreakPageHeight = WholePageHeight - bottomPageHeightRef;
	/* BreakPageHeight = hauteur haut de page + hauteur du corps */
	/* Attention on suppose que le haut de page ne peut pas */
	/* depasser la valeur BreakPageHeight ... faut-il prevoir */
	/* des controles pour le verifier (cas des schemas !) ? */

	PageFooterHeight = bottomPageHeightRef;
	PageHeaderHeight = topPageHeightRef;
     }				/* fin cas ou pElPage est une marque page */
}


#endif /* __COLPAGE__ */


/*----------------------------------------------------------------------
   	DetrImAbs detruit l'image abstraite de la vue concernee et	
   		efface sa frame si la vue est une vue pour VusSch	
    Vue = numero d'elt assoc si vue associee sinon      
    Vue = numero de vue si vue d'arbre principal        
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         DestroyImAbsPages (int view, boolean Assoc, PtrDocument pDoc, int schView)

#else  /* __STDC__ */
static void         DestroyImAbsPages (view, Assoc, pDoc, schView)
int                 view;
boolean             Assoc;
PtrDocument         pDoc;
int                 schView;

#endif /* __STDC__ */

{
   PtrAbstractBox      pAb;
   int                 h;

   /* boolean        tropcourt; */
   int                 frame;
   PtrAbstractBox      rootAbsBox;
   boolean             toDestroy;

   frame = 1;			/* initialisation (pour le compilateur !) */
   rootAbsBox = NULL;		/* initialisation (pour le compilateur !) */
   if (Assoc)
     {
	/* c'est une vue d'elements associes */
	rootAbsBox = pDoc->DocAssocRoot[view - 1]->ElAbstractBox[0];
	frame = pDoc->DocAssocFrame[view - 1];
	toDestroy = (rootAbsBox != NULL);
     }
   else
      /* on verifie si c'est bien une vue correspondant a la vue du schema */
   if (pDoc->DocView[view - 1].DvPSchemaView == schView)
     {
	rootAbsBox = pDoc->DocViewRootAb[view - 1];
	frame = pDoc->DocViewFrame[view - 1];
	toDestroy = (rootAbsBox != NULL);
     }
   else
      toDestroy = FALSE;	/* rien a faire */
   if (toDestroy)
     {
	/* tous les paves englobes par le pave racine de la vue sont marques */
	/* morts */
	pAb = rootAbsBox->AbFirstEnclosed;
	while (pAb != NULL)
	  {
	     SetDeadAbsBox (pAb);
	     pAb = pAb->AbNext;
	  }
	/* ceci est signale au Mediateur */
#ifdef __COLPAGE__
	/* vide la chaine des regles en retard sur la racine */
	/* normalement doit etre deja vide ! */
	ApplDelayedRule (rootAbsBox->AbFirstEnclosed->AbElement, pDoc);
	/* libere tous les paves morts de la vue */
	/* ceci est signale au Mediateur */
	h = -1;			/* changement de signification de h */
#else  /* __COLPAGE__ */
	h = 0;
#endif /* __COLPAGE__ */
	(void) ChangeConcreteImage (frame, &h, rootAbsBox);
	/* libere tous les paves morts de la vue */
	FreeDeadAbstractBoxes (rootAbsBox);
	/* indique qu'il faudra reappliquer les regles de presentation du */
	/* pave racine, par exemple pour recreer les boites de presentation */
	/* creees par lui et qui viennent d'etre detruites. */
	rootAbsBox->AbSize = -1;
#ifdef __COLPAGE__
	rootAbsBox->AbTruncatedTail = TRUE;
#endif /* __COLPAGE__ */
	/* on marque le pave racine complet en tete pour que AbsBoxesCreate */
	/* engendre effectivement les paves de presentation cree's en tete */
	/* par l'element racine (regles CreateFirst). */
	if (rootAbsBox->AbLeafType == LtCompound)
	   rootAbsBox->AbTruncatedHead = FALSE;

     }				/* fin toDestroy */
}


#ifdef __COLPAGE__
	/* AddLastPageBreak supprime' */
#else  /* __COLPAGE__ */

/*----------------------------------------------------------------------
   AddLastPageBreak	ajoute une marque de page a la fin de la vue	
   	schView de l'arbre de racine pRootEl s'il n'y en a pas deja une
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                AddLastPageBreak (PtrElement pRootEl, int schView, PtrDocument pDoc,
				      boolean withAPP)

#else  /* __STDC__ */
void                AddLastPageBreak (pRootEl, schView, pDoc, withAPP)
PtrElement          pRootEl;
int                 schView;
PtrDocument         pDoc;
boolean             withAPP;

#endif /* __STDC__ */

{
   PtrElement          pEl;
   PtrElement          pElPage;
   PtrPSchema          pSchP;
   int                 cpt;
   boolean             pageAtEnd;
   boolean             stop, stop1, ok;
   NotifyElement       notifyEl;
   int                 NSiblings;

   /* cherche d'abord s'il n'y en pas deja une */
   pageAtEnd = FALSE;		/* on n'en pas encore vu */
   if (pRootEl == NULL)
      pEl = NULL;
   else
      pEl = pRootEl->ElFirstChild;
   stop = FALSE;		/* descend l'arbre */
   if (pEl != NULL)
      do
	 if (pEl->ElTypeNumber == PageBreak + 1 && pEl->ElViewPSchema == schView)
	   {
	      /* c'est une marque de page pour cette vue */
	      /* saute les marques de page suivantes, qui concernent d'autres vues */
	      stop1 = FALSE;
	      do
		 if (pEl->ElNext == NULL)
		    /* dernier element de ce niveau */
		    pageAtEnd = TRUE;
	      /* la marque trouvee est bien en fin de vue */
		 else
		   {		/* examine l'element suivant */
		      pEl = pEl->ElNext;
		      if (pEl->ElTypeNumber != PageBreak + 1)
			 stop1 = TRUE;
		      /* ce n'est pas une marque de page, la */
		      /* marque trouvee n'est donc pas en fin */
		   }
	      while (!(stop1 || pageAtEnd));
	   }
	 else
	    /* ce n'est pas une marque de page pour la vue */
	 if (pEl->ElNext != NULL)
	    pEl = pEl->ElNext;	/* passe au suivant */
	 else
	    /* il n'y a pas d'element suivant */
	 if (pEl->ElTerminal)
	    stop = TRUE;	/* c'est une feuille, on s'arrete */
	 else
	    pEl = pEl->ElFirstChild;	/* descend d'un niveau */
      while (!(stop || pEl == NULL || pageAtEnd));

   pEl = pRootEl->ElFirstChild;
   if (pEl != NULL && !pageAtEnd)
      /* il n'y a pas de marque de page a la fin de la vue */
      /* cree une marque de page */
     {
	/* cherche le dernier fils de la racine */
	NSiblings = 1;
	while (pEl->ElNext != NULL)
	  {
	     NSiblings++;
	     pEl = pEl->ElNext;
	  }
	if (withAPP)
	  {
	     /* envoie l'evenement ElemNew.Pre */
	     notifyEl.event = TteElemNew;
	     notifyEl.document = (Document) IdentDocument (pDoc);
	     notifyEl.element = (Element) (pEl->ElParent);
	     notifyEl.elementType.ElTypeNum = PageBreak + 1;
	     notifyEl.elementType.ElSSchema = (SSchema) (pRootEl->ElStructSchema);
	     notifyEl.position = NSiblings;
	     ok = !CallEventType ((NotifyEvent *) & notifyEl, TRUE);
	  }
	else
	   ok = TRUE;
	if (ok)
	  {
	     /* cree l'element marque de page */
	     pElPage = NewSubtree (PageBreak + 1, pRootEl->ElStructSchema, pDoc,
				   pEl->ElAssocNum, TRUE, TRUE, TRUE, TRUE);
	     /* insere la nouvelle marque de page apres le dernier fils */
	     InsertElementAfter (pEl, pElPage);		/* remplit cette marque de page */
	     pElPage->ElPageType = PgComputed;
	     pElPage->ElViewPSchema = schView;
	     /* cherche le compteur de page a appliquer */
	     cpt = GetPageCounter (pElPage, schView, &pSchP);
	     if (cpt == 0)
		/* page non numerotee */
		pElPage->ElPageNumber = 1;
	     else
		/* calcule le numero de page */
		pElPage->ElPageNumber = CounterVal (cpt, pElPage->ElStructSchema, pSchP,
						    pElPage, schView);
#ifndef PAGINEETIMPRIME
	     /* envoie l'evenement ElemNew.Post */
	     if (withAPP)
		NotifySubTree (TteElemNew, pDoc, pElPage, 0);
#endif /* PAGINEETIMPRIME */
	  }
     }
}
#endif /* __COLPAGE__ */

#ifdef __COLPAGE__
/*      Nouvelle procedure pour les colonnes              */

/*----------------------------------------------------------------------
   	BalanceColumn equilibre le groupe de colonnes contenues dans le
   		dernier pave de groupe de colonnes de rootAbsBox         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         BalanceColumn (PtrDocument pDoc, PtrAbstractBox rootAbsBox, int nbView, int schView)
#else  /* __STDC__ */
static void         BalanceColumn (pDoc, rootAbsBox, nbView, schView)
PtrDocument         pDoc;
PtrAbstractBox      rootAbsBox;
int                 nbView;
int                 schView;

#endif /* __STDC__ */
{
   PtrAbstractBox      pP;
   int                 High, PosV, cutChar, h;
   PtrElement          pPage, rootEl;
   boolean             bool;
   boolean             absBoxTooHigh;
   PtrAbstractBox      origCutAbsBox;
   int                 frame;
   FILE               *list;

   pP = rootAbsBox->AbFirstEnclosed;
   while (pP != NULL && pP->AbPresentationBox)
      pP = pP->AbNext;		/* on est sur le corps de page */
   if (pP != NULL)
      pP = pP->AbFirstEnclosed;	/* premier groupe de colonnes */
   while (pP != NULL && pP->AbNext != NULL)
      pP = pP->AbNext;		/* dernier groupe de colonnes */
   if (pP->AbElement->ElTypeNumber == PageBreak + 1
       && pP->AbElement->ElPageType == ColGroup)
     {
	/* version simple : le groupe de colonnes ne contient qu'une seule */
	/* colonne qu'il faut couper en deux */
	/* TODO a generaliser a plus de deux colonnes */
	frame = pDoc->DocViewFrame[nbView - 1];
	/* on signale les paves au mediateur pour qu'il cree les boites */
	h = 0;
	bool = ChangeConcreteImage (frame, &h, rootAbsBox);
	pP = pP->AbFirstEnclosed;	/* pP pave de colonne gauche */
	/* calcul de la hauteur du pave colonne gauche */
	SetPageHeight (pP, TRUE, &High, &PosV, &cutChar);
	/* Hauteur = dimension verticale du pave colonne simple */
	/* on fait evaluer la coupure de colonne avec h = Hauteur / 2 */
	/* on appelle Modifvue a partir du pave colonne simple */
	ChangeRHPage (rootAbsBox, pDoc, nbView);
	h = PosV + (High / 2);
	bool = ChangeConcreteImage (frame, &h, rootAbsBox);
	if (!bool)
	  {
	     /* on insere une marque colonne */
	     pPage = NULL;
	     origCutAbsBox = NULL;
	     absBoxTooHigh = FALSE;
	     rootEl = rootAbsBox->AbElement;
	     SetPage (&pPage, frame, &origCutAbsBox, &absBoxTooHigh, pDoc,
		      schView, nbView, rootEl);
	     /* on remet le volume libre a -1 */
	  }
     }
}

#endif /* __COLPAGE__ */

/*----------------------------------------------------------------------
  PaginateView l'utilisateur demande le (re)decoupage en pages de la	
  vue de numero Vue pour le document pointe' par pDoc.	
  Si Assoc est vrai, c'est la vue d'elements associes de	
  numero Vue qui doit etre traitee			
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                PaginateView (PtrDocument pDoc, int view, boolean Assoc)
#else  /* __STDC__ */
void                PaginateView (pDoc, view, Assoc)
PtrDocument         pDoc;
int                 view;
boolean             Assoc;
#endif /* __STDC__ */
{
   PtrPSchema          pSchP;
   PtrElement          pRootEl, firstPage, pPage;
   PtrElement          pEl1;
   PtrAbstractBox      rootAbsBox, pP;
   PtrAbstractBox      previousPageAbBox;
#ifdef PAGINEETIMPRIME
   int                 h;
#else /*  PAGINEETIMPRIME */
   PtrElement          firstSelection, lastSelection;
   int                 FirstSelectedChar = 0, LastSelectedChar = 0;
   boolean             sel;
#endif /* PAGINEETIMPRIME */
   int                 schView;
   int                 v, clipOrg;
   int                 frame, volume, volprec, nbView, cpt;
   boolean             tooShort;
   boolean             complete;
   boolean             isFirstPage;
#ifdef __COLPAGE__
   PtrElement          pElPage1;
   PtrAbstractBox      pBody, pTreatedPage, pPageToTreat, PavR;
   PtrPRule            pRuleDimV;
   FILE               *list;
   int                 High, PosV, cutChar, h;	/* pour calcul page */
   boolean             New;
#else  /* __COLPAGE__ */
   PtrPSchema          pSchPage;
   int                 b;
   boolean             nothingAdded;
#endif /* __COLPAGE__ */

   RunningPaginate = TRUE;
#ifdef __COLPAGE__
   pagesCounter = 0;
#endif /* __COLPAGE__ */
   clipOrg = 0;
   isFirstPage = TRUE;
   pagesCounter = 1;
   firstPage = NULL;
   previousPageAbBox = NULL;
   NbBoxesPageHeaderToCreate = 0;
   if (Assoc)
     /* c'est une vue d'elements associes */
     /* le numero d'element associe est Vue */
     {
       /* numero dans le document de la vue a paginer */
       nbView = 1;
       /* numero dans le schema de la vue a paginer */
       schView = 1;
       pRootEl = pDoc->DocAssocRoot[view - 1];
       frame = pDoc->DocAssocFrame[view - 1];
     }
   else
     {
       /* numero dans le document de la vue a paginer */
       nbView = view;
       /* numero dans le schema de la vue a paginer */
       schView = AppliedView (pDoc->DocRootElement, NULL, pDoc, nbView);
       pRootEl = pDoc->DocRootElement;
       frame = pDoc->DocViewFrame[view - 1];
     }
#ifdef __COLPAGE__
   /* si le premier fils de la racine n'est pas une marque de page, le */
   /* document n'est pas mis en page */
   /* le compilateur prs doit verifier si la racine possede une regle */
   /* page; donc si le doc est pagine, on doit toujours */
   /* trouver un premier pave corps */
   /* de page correspondant a la marque de debut d'element */
   /* (premier fils de la racine) */
   /* recherche du premier element Marque Page de la vue schView */
   pElPage1 = pRootEl->ElFirstChild;
   while (pElPage1 != NULL && pElPage1->ElTypeNumber == PageBreak + 1
	  && pElPage1->ElViewPSchema != schView)
     pElPage1 = pElPage1->ElNext;

   if (pElPage1 != NULL && pElPage1->ElTypeNumber == PageBreak + 1)
     {
       /* document mis en pages, on peut continuer la procedure */
#endif /* __COLPAGE__ */

#ifndef PAGINEETIMPRIME
       sel = AbortPageSelection (pDoc, schView, &firstSelection, &lastSelection, &FirstSelectedChar, &LastSelectedChar);
#endif /* PAGINEETIMPRIME */
       /* detruit l'image abstraite de la vue concernee, en conservant la racine */
       if (Assoc)
	 /* le numero d'element associe est Vue */
	 DestroyImAbsPages (view, Assoc, pDoc, schView);
       else
	 for (v = 1; v <= MAX_VIEW_DOC; v++)
	   DestroyImAbsPages (v, Assoc, pDoc, schView);
       /* destruction des marques de page */
       DestroyPageMarks (pDoc, pRootEl, schView);
       
#ifdef __COLPAGE__
       /* la suite du code est different */
       BreakPageHeight = 0;
       WholePageHeight = 0;
       bottomPageHeightRef = 0;
       /* cree les paves du debut de la vue */
       pTreatedPage = NULL;
       if (Assoc)
	 {
	   /* ATTENTION si Assoc est vrai, Vue est le numero d'element associe */
	   pDoc->DocAssocNPages[view - 1] = -1;
	   /* creation des paves par le volume */
	   pDoc->DocAssocFreeVolume[view - 1] = pDoc->DocAssocVolume[view - 1];
	 }
       else
	 {
	   pDoc->DocViewNPages[view - 1] = -1;
	   /* creation des paves par le volume */
	   pDoc->DocViewFreeVolume[view - 1] = pDoc->DocViewVolume[view - 1];
	   /* modification de ce volume si trop faible */
	   if (pDoc->DocViewFreeVolume[view - 1] < 100)
	     pDoc->DocViewFreeVolume[view - 1] = 100;
	 }
       /* on fait evaluer BreakPageHeight avant d'appeler AbsBoxesCreate */
       /* car BreakPageHeight peut eventuellement dynamiquement changer si */
       /* il y a des elements en bas de page */
       /* pElPage1 contient le 1er element marquepage de cette vue */
       /* attention Vue = no d'elt associe (si vue assoc) */
       /*                 ou nbView si arbre principal */
       /*           nbView = vue d'affichage (tj 1 si vue assoc) */
       /* mis en commentaire pour tester l'equilibrage ?? */
       /* PageHeaderFooter(pElPage1, nbView, schView, frame, pDoc); */
       StopBeforeCreation = FALSE;	/*  on veut creer la 1ere MP */
       FoundPageHF = FALSE;	/* pour savoir la cause de l'arret de */
       ToBalance = TRUE;
       /* creation des paves: soit volume soit MP ou d'un elt asscoc HB */
       /* AbsBoxesCreate doit s'arreter apres creation d'1 MP ou elt assoc */
       /* le booleen FoundPageHF n'est jamais mis a faux par AbsBoxesCreate */
       /* il est seulement affecte a vrai lorsqu'une MP ou un elt assoc */
       /* est trouve par AbsBoxesCreate. C'est a Page de le remettre a faux */
       HFPageRefAssoc = NULL;	/* on initialise HFPageRefAssoc */
       pP = AbsBoxesCreate (pRootEl, pDoc, nbView, TRUE, TRUE, &complete);
       /* mise a jour de rootAbsBox apres la creation des paves */
       /* dans le cas de l'appel depuis print, il n'y avait aucun pave cree */
       if (Assoc)
	 rootAbsBox = pRootEl->ElAbstractBox[0];
       else
	 rootAbsBox = pDoc->DocViewRootAb[view - 1];
       volume = 0;
       /* on change la regle des paves corps de page (sauf si MP mise */
       /* par l'utilisateur) : hauteur = celle du contenu */
       /* pour permettre a ChangeConcreteImage de determiner la coupure de page */
       ChangeRHPage (rootAbsBox, pDoc, nbView);
       /* fait calculer l'image par le Mediateur */
       RealPageHeight = BreakPageHeight;
       tooShort = ChangeConcreteImage (frame, &RealPageHeight, rootAbsBox);
       /* cherche le pave de la premiere marque de page */
       pPageToTreat = rootAbsBox->AbFirstEnclosed;
       /* c'est le premier fils de la racine */
       while (pPageToTreat != NULL)
	 /* traite une page apres l'autre */
	 /* pPageToTreat contient le premier pave de la page a traiter */
	 /* pTreatedPage contient le dernier pave de la derniere page traitee */
	 {
	   pP = pPageToTreat;
	   /* On prend la hauteur de ce type de page */
	   /* cette hauteur change si la page est une PgBegin (nouveau */
	   /* type de marque page) */
	   /* sinon, PageHeaderFooter positionne BreakPageHeight a la valeur donnee */
	   /* par la regle courante. Remise a jour necessaire car la page */
	   /* precedente pouvait avoir des elements en bas de page qui */
	   /* ont fait changer la hauteur de page. Cf. CrPavHB */
	   /* PageHeaderFooter(pP->AbElement, nbView, schView, frame, pDoc); */
	   /* TODO faut-il verifier que pPageToTreat n'est pas NULL ? */
	   /* detruit les paves de la page precedente (si existe) pour */
	   /* avoir une coupure correcte lors du prochain ChangeConcreteImage */
	   /* calcule le volume de ce qui a ete detruit */
	   /* pour en regenerer autant ensuite */
	   volprec = rootAbsBox->AbVolume;
	   /* KillAbsBoxBeforePage appelle ChangeConcreteImage apres destruction */
	   tooShort = KillAbsBoxBeforePage (pP, frame, pDoc, nbView, &clipOrg);
	   rootAbsBox->AbTruncatedHead = TRUE;
	   /* si on a detruit des paves, la racine est coupee en tete */
	   volume += volprec - rootAbsBox->AbVolume;
	   if (pP->AbElement->ElTypeNumber != PageBreak + 1)
	     printf ("erreur pagination : pave <> page sous racine", "\n");
	   /* on renumerote cette marque de page */
	   pEl1 = pP->AbElement;
	   /* cherche le compteur de page a appliquer a cette page */
	   cpt = GetPageCounter (pP->AbElement, schView, &pSchP);
	   if (cpt == 0)
	     /* page non numerotee */
	     {
	       pagesCounter++;
	       pEl1->ElPageNumber = pagesCounter;
	     }
	   else		/* calcule le numero de page */
	     pEl1->ElPageNumber =
	       CounterVal (cpt, pEl1->ElStructSchema, pSchP, pP->AbElement, schView);
	   /* affiche un message avec le numero de page */
	   DisplayPageMsg (pDoc, pRootEl, pP->AbElement, schView, Assoc, &isFirstPage)
	     
	     if (firstPage == NULL)
	       firstPage = pP->AbElement;
	   /* cherche le dernier pave de la marque de page */
	   while (pP->AbNext != NULL && pP->AbElement == pP->AbNext->AbElement)
	     {
	       if (!pP->AbPresentationBox)
		 pBody = pP;	/* on memorise le corps de page */
	       pP = pP->AbNext;
	     }
	   /* remarque : pBody existe toujours */
	   /* on conserve ce pointeur pour ne pas revenir sur cette page */
	   /* et pour determiner quand la page suivante sera creee */
	   pTreatedPage = pP;
	   /* cette page est finie de traiter */
	   /* on passe a la suivante */
	   pPageToTreat = pP->AbNext;
	   /* si une page suivante existe, on ne la prend en compte */
	   /* que si le corps de la page qui vient d'etre traitee n'est */
	   /* pas coupee par la limite */
	   if (pPageToTreat != NULL && pBody->AbOnPageBreak)
	     pPageToTreat = NULL;
	   while (pPageToTreat == NULL && (rootAbsBox->AbTruncatedTail || !tooShort))
	     {
	       /* on boucle jusqu'a creer le pave de la marque page suivante */
	       /* soit marque page utilisateur soit mise par PutMark */
	       /* mais on ne veut pas en creer trop a la fois pour prendre */
	       /* correctement en compte la hauteur de chaque page en fonction */
	       /* des elements associes en haut et bas de page */
	       /* c'est dans cette boucle que l'on cree les colonnes */
	       /* car on n'utilise pas pPageToTreat pour les colonnes */
	       StopBeforeCreation = TRUE;	/* var globale pour AbsBoxesCreate */
	       /* AbsBoxesCreate doit s'arreter AVANT creation */
	       /* d'1 MP ou d'un elt asscoc HB */
	       ToBalance = TRUE;
	       while (pPageToTreat == NULL && rootAbsBox->AbTruncatedTail && tooShort)
		 /* boucle d'ajout des paves */
		 {
		   HFPageRefAssoc = NULL;	/* on reinitialise HFPageRefAssoc */
		   AbsBoxAssocToDestroy = NULL;	/* et AbsBoxAssocToDestroy */
		   FoundPageHF = FALSE;	/* pour savoir la cause de l'arret de */
		   /* creation des paves: soit volume soit MP soit elt assoc HB */
		   /* on boucle jusqu'a avoir assez de paves pour faire */
		   /* une page */
		   /* on ajoute au moins 1000 caracteres a l'image */
		   if (volume < 1000)
		     /* indique le volume qui peut etre cree */
		     volume = 1000;
		   do
		     {
		       if (volume == 0)
			 /* cas ou on boucle et ou volume depasse la taille max */
			 /* cas d'erreur */
			 printf ("erreur pagination : boucle dans creation image ", "\n");
		       if (Assoc)
			 /* ATTENTION si Assoc est vrai, il faut utiliser Vue */
			 /* et non nbView, car Vue est le numero d'element associe */
			 pDoc->DocAssocFreeVolume[view - 1] = volume;
		       else
			 pDoc->DocViewFreeVolume[nbView - 1] = volume;
		       volprec = rootAbsBox->AbVolume;
		       /* volume de la vue avant */
		       /* demande la creation de paves supplementaires */
		       StopGroupCol = FALSE;
		       if (ToBalance && StopGroupCol)
			 {
			   /* mise a jour de StopGroupCol et du vollibre */
			   /* pour permettre la creation des paves dans la */
			   /* procedure d'equilibrage */
			   StopGroupCol = FALSE;
			   if (Assoc)
			     /* ATTENTION si Assoc est vrai, il faut utiliser Vue */
			     /* et non nbView, car Vue est le numero d'element associe */
			     pDoc->DocAssocFreeVolume[view - 1] = volume;
			   else
			     pDoc->DocViewFreeVolume[nbView - 1] = volume;
			   BalanceColumn (pDoc, rootAbsBox, nbView, schView);
			   /*       ToBalance = FALSE; *//* TODO A revoir */
			   /* pour ne pas recommencer */
			   /* si l'equilibrage a deja ete fait */
			 }
		       if (rootAbsBox->AbVolume <= volprec)
			 /* rien n'a ete cree, augmente le
			    volume de ce qui peut etre cree' */
			 /* et on deverrouille la creation avec */
			 /* StopBeforeCreation */
			 /* (cas ou le premier element a ajouter est une */
			 /* MP ou un elt assoc HB) */
			 {
			   volume = 2 * volume;
			   StopBeforeCreation = FALSE;
			   FoundPageHF = FALSE;
			   /* AbsBoxesCreate doit s'arreter apres creation d'1 MP */
			   /* ou d'un elt asscoc HB */
			 }
		     }
		   while (!(rootAbsBox->AbVolume > volprec ||
			    !rootAbsBox->AbTruncatedTail));

		   /* pour l prochain ajout de paves */
		   volume = rootAbsBox->AbVolume;
		   /* appelle ChangeConcreteImage pour savoir si au moins une */
		   /* boite est traversee par une frontiere de page apres     */
		   /* l'ajout des paves supplementaires */
		   RealPageHeight = BreakPageHeight;
		   /* on change la regle des paves corps de page (sauf si MP mise */
		   /* par l'utilisateur) : hauteur = celle du contenu */
		   /* et on decale la position du bas et du filet de page */
		   ChangeRHPage (rootAbsBox, pDoc, nbView);
		   tooShort = ChangeConcreteImage (frame, &RealPageHeight, rootAbsBox);
		   /* si tropcourt, et si l'arret de creation est du^ */
		   /* a un element MP ou ref assoc HB (FoundPageHF = vrai) */
		   /* on bascule StopBeforeCreation pour permettre */
		   /* de continuer le calcul de l'i.a. (la creation */
		   /* s'etait arretee avant ou apres une MP ou une ref */
		   /* d'un elt assoc a placer en haut ou bas de page */
		   if (tooShort && FoundPageHF)
		     {
		       StopBeforeCreation = !StopBeforeCreation;
		       FoundPageHF = FALSE;	/* pour le tour suivant */
		     }
		   if (!tooShort && FoundPageHF && HFPageRefAssoc != NULL)
		     /* si on a cree une reference a un element associe */
		     /* qui a provoque la creation des paves de cet element */
		     /* associe en faisant deborder la page, il faut */
		     /* forcer la coupure avant la reference et supprimer */
		     /* si besoin (si c'est en haut de page et */
		     /* si cet elt assoc n'est pas deja reference */
		     /* dans la page) les paves de l'elt assoc et/ou de */
		     /* l'englobant si pas d'autre elt assoc dans la page */
		     {
		       if (AbsBoxAssocToDestroy != NULL)
			 {
			   /* on detruit le pave et ses eventuels paves de pres */
			   pP = AbsBoxAssocToDestroy;
			   while (pP->AbPrevious != NULL
				  && pP->AbPrevious->AbElement == pP->AbElement)
			     {
			       pP = pP->AbPrevious;
			       SetDeadAbsBox (pP);
			       ApplyRefAbsBoxSupp (pP, &PavR, pDoc);
			     }
			   pP = AbsBoxAssocToDestroy;
			   while (pP->AbNext != NULL
				  && pP->AbNext->AbElement == pP->AbElement)
			     {
			       pP = pP->AbNext;
			       SetDeadAbsBox (pP);
			       ApplyRefAbsBoxSupp (pP, &PavR, pDoc);
			     }
			   SetDeadAbsBox (AbsBoxAssocToDestroy);
			   ApplyRefAbsBoxSupp (AbsBoxAssocToDestroy, &PavR, pDoc);
			   /* on signale les paves detruits au mediateur */
			   h = -1;	/* changement de signification de h */
			   tooShort = ChangeConcreteImage (frame, &h, rootAbsBox);
			   /* on libere les paves */
			   pP = AbsBoxAssocToDestroy->AbEnclosing;
			   FreeDeadAbstractBoxes (AbsBoxAssocToDestroy->AbEnclosing);
			   /* on recherche le pave englobant haut ou bas de page */
			   while (pP->AbElement != pBody->AbElement)
			     pP = pP->AbEnclosing;
			   /* calcul de la hauteur du pave haut ou bas de page */
			   SetPageHeight (pP, TRUE, &High, &PosV, &cutChar);
			   /* Hauteur = dim verticale du haut (ou bas) de page */
			   if (pP->AbPrevious == pBody)
			     /* des paves ont ete ajoutes en bas de page */
			     PageFooterHeight = High;
			   else	/* des paves ont ete ajoutes en haut de page */
			     PageHeaderHeight = High;
			   /* BreakPageHeight = hauteur totale - hauteur bas */
			   BreakPageHeight = WholePageHeight - PageFooterHeight;
			   
			   /* on modifie la regle de presentation specifique */
			   /* du corps de page */
			   pRuleDimV = SearchPresRule (pBody->AbElement,
						       PtHeight, 0, &New, pDoc, nbView);
			   /* change le parametre de la regle */
			   pRuleDimV->PrDimRule.DrValue =
			     WholePageHeight - PageFooterHeight - PageHeaderHeight;
			 }
		       /* on fait evaluer la position du pave reference */
		       SetPageHeight (HFPageRefAssoc->ElAbstractBox[nbView - 1], TRUE,
				      &High, &PosV, &cutChar);
		       /* on force la coupure a cette hauteur */
		       h = PosV;
		       tooShort = ChangeConcreteImage (frame, &h, rootAbsBox);
		       /* normalement tropcourt est tj faux */
		     }	/* fin cas page trop grande a cause d'elt ref HB */
		   /* si on a cree les paves de la page suivante (page */
		   /* utilisateur ou nouvelle regle page), on met a jour */
		   /* pPageToTreat */
		   pPageToTreat = pTreatedPage->AbNext;
		 }		/* fin boucle d'ajout de paves */
	       if (!tooShort && pBody->AbOnPageBreak)
		 {
		   /* il faut inserer une marque de page */
		   /* l'image fait plus d'une hauteur de page */
		   /* donc si une page a ete ajoutee (page utilisateur) */
		   /* elle est trop loin, il faut donc remettre pPageToTreat */
		   /* a NULL */
		   pPageToTreat = NULL;
		   volprec = rootAbsBox->AbVolume;
		   /* Insere un element marque page a la frontiere de page et */
		   /* detruit tous les paves qui precedent cette frontiere. */
		   /* on repositionne StopBeforeCreation a faux pour */
		   /* permettre la creation des paves du nouvel elt MP */
		   StopBeforeCreation = FALSE;
		   PutMark (pRootEl, nbView, pDoc, frame, &pTreatedPage, &pPageToTreat, schView);
		   /* calcule le volume qui a ete detruit pour en regenerer */
		   /* autant ensuite */
		   tooShort = TRUE; /* pour forcer la creation de nouveaux paves */
		   /* et l'appel a modifvue */
		   volume = volume + volprec - rootAbsBox->AbVolume;
		 }
	       else if (!tooShort && !pBody->AbOnPageBreak)
		 /* cas ou la coupure est sur le bas de page ou le filet */
		 /* il faut passer a la page suivante sans inserer de */
		 /* marque page supplementaire : on reboucle pour */
		 /* ajouter eventuellement des paves si il n'y a pas */
		 /* encore de paves de la page suivante */
		 /* pour cela, on force tropcourt a vrai */
		 if (pPageToTreat == NULL)
		   tooShort = TRUE;
	       /* TODO faut-il mettre a jour AbOnPageBreak et AbAfterPageBreak */
	     } /* fin boucle pPageToTreat == NULL et !fin doc */
	 } /* fin boucle creation des pages une a une */
       
       /* fin de la vue */
       /* ajoute une marque de page a la fin s'il n'y en a pas deja une */
       /* ce n'est plus necessaire : code supprime */
       
       RunningPaginate = FALSE;
       /* detruit l'image abstraite de la fin du document */
       DestroyImAbsPages (view, Assoc, pDoc, schView);
       /* reconstruit l'image de la vue et l'affiche */
       DisplaySelectPages (pDoc, firstPage, view, Assoc, sel, firstSelection,
			   lastSelection, FirstSelectedChar, LastSelectedChar);
       /* paginer un document le modifie ... */
       pDoc->DocModified = TRUE;
     } /* fin du cas ou le document est mis en pages */
   
#else  /* __COLPAGE__ */

   PageHeight = 0;
   PageFooterHeight = 0;	/* cree les paves du debut de la vue */
   if (Assoc)
     pDoc->DocAssocFreeVolume[view - 1] = pDoc->DocAssocVolume[view - 1];
   else
     pDoc->DocViewFreeVolume[view - 1] = pDoc->DocViewVolume[view - 1];
   pP = AbsBoxesCreate (pRootEl, pDoc, nbView, TRUE, TRUE, &complete);
   volume = 0;
   /* mise a jour de rootAbsBox apres la creation des paves */
   /* dans le cas de l'appel depuis print, il n'y avait aucun pave cree */
   if (Assoc)
     rootAbsBox = pRootEl->ElAbstractBox[0];
   else
     {
       rootAbsBox = pRootEl->ElAbstractBox[view - 1];
       if (pDoc->DocViewRootAb[view - 1] == NULL)
	 pDoc->DocViewRootAb[view - 1] = rootAbsBox;
     }
   pP = rootAbsBox;
   /* cherche la 1ere marque de page dans le debut de l'image du document, */
   /* pour connaitre la hauteur des pages */
   /* cherche d'abord le 1er pave feuille ou la premiere marque de page */
   while (pP->AbFirstEnclosed != NULL &&
	  pP->AbElement->ElTypeNumber != PageBreak + 1)
     pP = pP->AbFirstEnclosed;
   if (pP->AbElement->ElTypeNumber != PageBreak + 1)
       /* le document ne commence pas par une marque de page pour cette */
       /* vue ; on cherche la premiere marque de page qui suit */
       pP = AbsBoxFromElOrPres (pP, FALSE, PageBreak + 1, NULL, NULL);

   previousPageAbBox = pP;
   if (pP != NULL && pP->AbElement->ElTypeNumber == PageBreak + 1)
       /* on a trouve une marque de page, on determine */
       /* la hauteur de ce type de page */
       PageHeaderFooter (pP->AbElement, schView, &b, &pSchPage);

   /* fait calculer l'image par le Mediateur */
   RealPageHeight = PageHeight;
   tooShort = ChangeConcreteImage (frame, &RealPageHeight, rootAbsBox);
   do
     /* traite une page apres l'autre */
     {
       if (previousPageAbBox != NULL)
	 pP = previousPageAbBox;
       else
	 pP = AbsBoxFromElOrPres (rootAbsBox, FALSE, PageBreak + 1, NULL, NULL);
       if (pP)
	 do
	 /* cherche les marques de page correspondant au debut d'un element */
	 /* portant une regle Page ou les marques mises par l'utilisateur */
	 {
	   if (pP->AbElement->ElTypeNumber == PageBreak + 1)
	     /* c'est une marque de page */
	     /* affiche un message avec le numero de page */
	     {
	       previousPageAbBox = pP;	/* pave de la page a imprimer */
#ifndef PAGINEETIMPRIME
	       DisplayPageMsg (pDoc, pRootEl, pP->AbElement, schView, Assoc, &isFirstPage);
#endif /* PAGINEETIMPRIME */
	       
	       /* On prend la hauteur de ce type de page */
	       PageHeaderFooter (pP->AbElement, schView, &b, &pSchPage);
	       if (firstPage == NULL)
		 firstPage = pP->AbElement;
	       /* cherche la derniere feuille dans la marque de page */
	       while (pP->AbFirstEnclosed != NULL)
		 {
		   pP = pP->AbFirstEnclosed;
		   while (pP->AbNext != NULL)
		     pP = pP->AbNext;
		 }
	     }
	   /* cherche la marque de la page suivante */
	   pP = AbsBoxFromElOrPres (pP, FALSE, PageBreak + 1, NULL, NULL);
	   if (pP != NULL)
	     /* on a trouve' une marque de page. C'est une page */
	     /* de debut d'element ou une page creee par l'utilisateur. */
	     if (pP->AbAfterPageBreak)
	       /* cette marque de page est trop loin, il faut inserer */
	       /* une marque de page avant */
	       pP = NULL;	/* pour sortir de la boucle */
	     else
	       {
		 /* on renumerote cette marque de page */
		 pEl1 = pP->AbElement;
		 /* cherche le compteur de page a appliquer a cette page */
		 cpt = GetPageCounter (pP->AbElement, schView, &pSchP);
		 if (cpt == 0)
		   /* page non numerotee */
		   {
		     pagesCounter++;
		     pEl1->ElPageNumber = pagesCounter;
		   }
		 else
		   {
		     /* calcule le numero de page */
		     pEl1->ElPageNumber =
		       CounterVal (cpt, pEl1->ElStructSchema, pSchP, pP->AbElement, schView);
		     /* on met a jour les boites de presentation des compteurs des */
		     /* pages suivantes dans le cas de la pagination depuis l'impression */
		     /* cet appel est fait tout a la fin dans le cas d'une pagination */
		     /* normale */
		     
#ifdef PAGINEETIMPRIME
		     UpdateNumbers (NextElement (pEl1), pEl1, pDoc, TRUE);
		     /* serait-ce plus rapide si on faisait durectement l'appel : */
		     /* ChngBoiteCompteur(pEl1, pDoc, cpt, pSchP, pEl1->ElStructSchema, TRUE); */
#endif /* PAGINEETIMPRIME */
		   }
#ifndef PAGINEETIMPRIME
		 /* affiche un message avec le numero de page */
		 DisplayPageMsg (pDoc, pRootEl, pEl1, schView, Assoc, &isFirstPage);
#endif /* PAGINEETIMPRIME */
		 /* On prend la hauteur de ce type de page */
		 PageHeaderFooter (pP->AbElement, schView, &b, &pSchPage);
		 /* la marque de page est avant la limite de page calculee, */
		 /* on detruit tous les paves qui precedent la marque de page */
		 volprec = rootAbsBox->AbVolume;
		 /* avant de detruire la page precedente, on l'imprime */
		 /* si la demande a ete faite */
		 /* previousPageAbBox contient le pave de la page precedente */
		 /* (sauf cas de la premiere page) */
#ifdef PAGINEETIMPRIME
		 if (previousPageAbBox != pP)
		   PrintOnePage (pDoc, previousPageAbBox, pP, view, clipOrg, Assoc);
#endif /* PAGINEETIMPRIME */
		 /* on met a jour previousPageAbBox pour le tour suivant (au cas ou */
		 /* on soit a la fin du document) */
		 previousPageAbBox = pP;
		 tooShort = KillAbsBoxBeforePage (pP, frame, pDoc, nbView, &clipOrg);
		 /* calcule le volume de ce qui a ete detruit */
		 /* pour en regenerer autant ensuite */
		 if (rootAbsBox->AbVolume < 0)
		   rootAbsBox->AbVolume = 0;
		 volume += volprec - rootAbsBox->AbVolume;
	       }
	 }
       while (pP != NULL);

       if (!tooShort)
	 /* l'image fait plus d'une hauteur de page */
	 {
	   volprec = rootAbsBox->AbVolume;
	   /* Insere un element marque de page a la frontiere de page et */
	   /* detruit tous les paves qui precedent cette frontiere. */
	   pPage = PutMark (pRootEl, nbView, pDoc, frame, schView);
	   /* une nouvelle page vient d'etre calculee, on l'imprime */
	   if ((pPage != NULL) && (pPage->ElAbstractBox[nbView - 1] != NULL))
	     {
	       /* avant de detruire la page precedente, on l'imprime */
	       /* si la demande a ete faite */
	       /* previousPageAbBox contient le pave de la page precedente */
#ifdef PAGINEETIMPRIME
	       PrintOnePage (pDoc, previousPageAbBox, pPage->ElAbstractBox[nbView - 1],
			     view, clipOrg, Assoc);
#endif /* PAGINEETIMPRIME */
	       /* detruit tous les paves qui precedent la nouvelle frontiere */
	       tooShort = KillAbsBoxBeforePage (pPage->ElAbstractBox[nbView - 1],
						frame, pDoc, nbView, &clipOrg);
	       /* previousPageAbBox devient la nouvelle page cree */
	       previousPageAbBox = pPage->ElAbstractBox[nbView - 1];
	       /* calcule le volume de ce qui a ete detruit pour en regenerer */
	       /* autant ensuite */
	       if (rootAbsBox->AbVolume < 0)
		 rootAbsBox->AbVolume = 0;
	       volume = volume + volprec - rootAbsBox->AbVolume;
	     }
	 }
       /* complete l'image abstraite de cette vue jusqu'a ce qu'elle */
       /* contienne une boite traversee par une frontiere de page ou qu'on */
       /* soit arrive' a la fin de la vue. */
       nothingAdded = TRUE;	/* on n'a encore rien ajoute' a l'image */
       while (tooShort && rootAbsBox->AbTruncatedTail)
	 /* on ajoute au moins 100 caracteres a l'image */
	 {
	   if (volume < 100)
	     /* indique le volume qui peut etre cree */
	     volume = 100;
	   do
	     {
	       if (Assoc)
		 pDoc->DocAssocFreeVolume[view - 1] = volume;
	       else
		 pDoc->DocViewFreeVolume[view - 1] = volume;
	       volprec = rootAbsBox->AbVolume;
	       /* volume de la vue avant */
	       /* demande la creation de paves supplementaires */
	       AddAbsBoxes (rootAbsBox, pDoc, FALSE);
	       if (rootAbsBox->AbVolume <= volprec)
		 /* rien n'a ete cree, augmente le
		    volume de ce qui peut etre cree' */
		 volume = 2 * volume;
	       else	/* on a ajoute' de nouveaux paves */
		 nothingAdded = FALSE;
	     }
	   while (!(rootAbsBox->AbVolume > volprec ||
		    !rootAbsBox->AbTruncatedTail));
	   volume = 0;	/* plus rien a generer */
	   /* appelle ChangeConcreteImage pour savoir si au moins une boite est */
	   /* traversee par une frontiere de page apres l'ajout des
	      paves supplementaires */
	   RealPageHeight = PageHeight;
	   if (Assoc)
	     {
	       if (pDoc->DocAssocModifiedAb[view - 1] != NULL)
		 {
		   tooShort = ChangeConcreteImage (frame, &RealPageHeight, pDoc->DocAssocModifiedAb[view - 1]);
		   pDoc->DocAssocModifiedAb[view - 1] = NULL;
		 }
	     }
	   else if (pDoc->DocViewModifiedAb[view - 1] != NULL)
	     {
	       tooShort = ChangeConcreteImage (frame, &RealPageHeight, pDoc->DocViewModifiedAb[view - 1]);
	       pDoc->DocViewModifiedAb[view - 1] = NULL;
	       /* si de nouveaux paves ont ete crees, on refait un tour pour */
	       /* traiter les marques de pages qu'ils contiennent */
	     }
	 }
     }
   while (!(tooShort && nothingAdded && !rootAbsBox->AbTruncatedTail));
   /* quand on sort de la boucle, previousPageAbBox point sur le pave de la */
   /* derniere page (avant que l'editeur n'ait insere la marque de fin) */
   
   /* fin de la vue */
   rootAbsBox->AbTruncatedTail = FALSE;

   /* Ajoute le saut de page qui manque eventuellement a la fin */
   AddLastPageBreak (pRootEl, schView, pDoc, TRUE);

#ifdef PAGINEETIMPRIME
   /* il faut imprimer la derniere page */
   /* on cree d'abord son pave */
   if (Assoc)
     pDoc->DocAssocFreeVolume[view - 1] = 100;
   else
     pDoc->DocViewFreeVolume[view - 1] = 100;
   rootAbsBox->AbTruncatedTail = TRUE;	/* il reste des paves a creer : */
   /* ce sont ceux de la nouvelle marque de page */
   AddAbsBoxes (rootAbsBox, pDoc, FALSE);
   
   /* cherche la marque de page qui vient d'etre inseree */
   /* cherche la derniere feuille dans la marque de page precedente */
   pP = previousPageAbBox;
   if (pP != NULL)
     {
       while (pP->AbFirstEnclosed != NULL)
	 {
	   pP = pP->AbFirstEnclosed;
	   while (pP->AbNext != NULL)
	     pP = pP->AbNext;
	 }
       pP = AbsBoxFromElOrPres (pP, FALSE, PageBreak + 1, NULL, NULL);
     }
   if (pP != NULL)
     /* on fait calculer l'image par le mediateur avant d'appeler */
     /* l'impression */
     {
       h = 0;		/* on ne fait pas evaluer la hauteur de coupure */
       if (Assoc)
	 {
	   if (pDoc->DocAssocModifiedAb[view - 1] != NULL)
	     {
	       tooShort = ChangeConcreteImage (frame, &h, pDoc->DocAssocModifiedAb[view - 1]);
	       pDoc->DocAssocModifiedAb[view - 1] = NULL;
	     }
	 }
       else if (pDoc->DocViewModifiedAb[view - 1] != NULL)
	 {
	   tooShort = ChangeConcreteImage (frame, &h, pDoc->DocViewModifiedAb[view - 1]);
	   pDoc->DocViewModifiedAb[view - 1] = NULL;
	 }
       PrintOnePage (pDoc, previousPageAbBox, pP, view, clipOrg, Assoc);
     }
#endif /* PAGINEETIMPRIME */
   
   RunningPaginate = FALSE;
   /* detruit l'image abstraite de la fin du document */
   DestroyImAbsPages (view, Assoc, pDoc, schView);
   /* reconstruit l'image de la vue et l'affiche */
#ifndef PAGINEETIMPRIME
   DisplaySelectPages (pDoc, firstPage, view, Assoc, sel, firstSelection,
		       lastSelection, FirstSelectedChar, LastSelectedChar);
#endif /* PAGINEETIMPRIME */
   /* paginer un document le modifie ... */
   pDoc->DocModified = TRUE;
#endif /* __COLPAGE__ */
}
/* End Of Module page */
