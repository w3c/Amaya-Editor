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

/* #define PRINT_DEBUG*/ 

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
#include "documentapi_f.h"
#include "docs_f.h"
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
#include "units_f.h"
#include "writepivot_f.h"

#define MaxPageLib 20
#define HMinPage 60		/* Hauteur minimum du corps de la page */
static int          pagesCounter;


#ifndef PAGINEETIMPRIME

/*----------------------------------------------------------------------
   AbortPageSelection  Annule et deplace si besoin la selection	
   			  courante du document.				
   			  Retourne les valeurs de cette selection 	
   			  dans firstSelection, lastSelection,           
   FirstSelectedChar et LastSelectedChar	        
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static ThotBool     AbortPageSelection (PtrDocument pDoc, int schView, PtrElement * firstSelection, PtrElement * lastSelection, int *FirstSelectedChar, int *LastSelectedChar)

#else  /* __STDC__ */
static ThotBool     AbortPageSelection (pDoc, schView, firstSelection, lastSelection, FirstSelectedChar, LastSelectedChar)
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
   ThotBool            sel;

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
	      if (pEl1->ElPageType == PgComputed)
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
	      if (pEl1->ElPageType == PgComputed)
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
		       if (pEl1->ElPrevious->ElPageType == PgComputed)
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
		       if (pEl1->ElPrevious->ElPageType == PgComputed)
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
   notifyEl.info = 0;
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
	DeleteElement (&pPage, pDoc);
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
	notifyEl.info = 0;
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
	      if (pEl->ElPageType == PgComputed)
		 /* c'est une marque de page calculee */
		{
		   if (pElPage != NULL)
		      /* il y a deja une marque de page a supprimer, on la supprime */
		     {
			SuppressPageMark (pElPage, pDoc, &pElLib);
			if (pElLib != NULL)
			   DeleteElement (&pElLib, pDoc);
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
	   DeleteElement (&pElLib, pDoc);
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
static void         DisplayPageMsg (PtrDocument pDoc, PtrElement pRootEl, PtrElement pEl, int schView, ThotBool Assoc, ThotBool * firstPage)
#else  /* __STDC__ */
static void         DisplayPageMsg (pDoc, pRootEl, pEl, schView, Assoc, firstPage)
PtrDocument         pDoc;
PtrElement          pRootEl;
PtrElement          pEl;
int                 schView;
ThotBool            Assoc;
ThotBool           *firstPage;

#endif /* __STDC__ */
{
   STRING name;

   /* affiche un message avec le numero de page */
   /* affiche d'abord le nom de la vue */
   if (Assoc)
      name = pRootEl->ElStructSchema->SsRule[pRootEl->ElTypeNumber - 1].SrName;
   else
      name = pDoc->DocSSchema->SsPSchema->PsView[schView - 1];

   if (*firstPage)
     {
	/* Affiche un message normal pour la 1ere fois */
	TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_PAGE), name, pEl->ElPageNumber);
	*firstPage = FALSE;
     }
   else
      /* Sinon ecrase le message precedent */
      TtaDisplayMessage (OVERHEAD, TtaGetMessage (LIB, TMSG_PAGE), name, pEl->ElPageNumber);
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
static void         DisplaySelectPages (PtrDocument pDoc, PtrElement firstPage, int view, ThotBool Assoc, ThotBool sel, PtrElement firstSelection, PtrElement lastSelection, int FirstSelectedChar, int LastSelectedChar)
#else  /* __STDC__ */
static void         DisplaySelectPages (pDoc, firstPage, view, Assoc, sel, firstSelection, lastSelection, FirstSelectedChar, LastSelectedChar)
PtrDocument         pDoc;
PtrElement          firstPage;
int                 view;
ThotBool            Assoc;
ThotBool            sel;
PtrElement          firstSelection;
PtrElement          lastSelection;
int                 FirstSelectedChar;
int                 LastSelectedChar;

#endif /* __STDC__ */

{
   PtrElement          pRootEl;
   PtrAbstractBox      rootAbsBox /* , pP */ ;
   int                 v, schView, frame, h;
   ThotBool            complete;

   /* reconstruit l'image de la vue et l'affiche */
   /* si on n'est pas en batch */
   PageHeight = 0;
   PageFooterHeight = 0;

   /* cree l'image abstraite des vues concernees */
   if (Assoc)
     {
	pDoc->DocAssocFreeVolume[view - 1] = pDoc->DocAssocVolume[view - 1];
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
		pDoc->DocViewFreeVolume[v - 1] = pDoc->DocViewVolume[v - 1];
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
      SelectRange (pDoc, firstSelection, lastSelection, FirstSelectedChar,
		   LastSelectedChar);

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
static ThotBool     Divisible (PtrAbstractBox pAb, PtrPRule * pR1, PtrAttribute * pAt1, PtrPRule * pR2, PtrAttribute * pAt2, int schView)

#else  /* __STDC__ */
static ThotBool     Divisible (pAb, pR1, pAt1, pR2, pAt2, schView)
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
   ThotBool            ret;
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
   ThotBool            found;

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
  InsertMark insere une Marque de Page avant l'element auquel correspond
  le pave pointe' par pAb.
  Si la Marque doit etre placee dans un element mis en lignes, en premiere	
  position, elle est placee avant cet element. 		
  Retourne un pointeur sur l'element Marque de Page insere'.
  On detruit la partie de l'i.a. qui suit cette marque  et on reconstruit
  l'i.a. (donc le pave pAb change ! )
  position prend 3 valeurs :
    0 : insere une Marque de Page avant l'element.
    1 : insere apres l'element.
    2 : insere comme premier fils de l'element.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static PtrElement   InsertMark (PtrAbstractBox pAb, int frame, int nbView, PtrAbstractBox *origCutAbsBox, ThotBool *absBoxTooHigh, int schView, PtrDocument pDoc, PtrElement rootEl, int position)

#else  /* __STDC__ */
static PtrElement   InsertMark (pAb, frame, nbView, origCutAbsBox, absBoxTooHigh, schView, pDoc, rootEl, position)
PtrAbstractBox      pAb;
int                 frame;
int                 nbView;
PtrAbstractBox     *origCutAbsBox;
ThotBool           *absBoxTooHigh;
int                 schView;
PtrDocument         pDoc;
PtrElement          rootEl;
int                 position;
#endif /* __STDC__ */
{
   PtrElement          pElPage, pEl;
   PtrPRule            pRule;
   PtrAbstractBox      modifAbsBox, topPageAbsBox, savePageAbsBox;
   PtrAbstractBox      pP1, pP;
   PtrPSchema          pSchP;
   AbPosition         *pPos;
#ifndef PAGINEETIMPRIME
   PtrElement          pF;
   NotifyElement       notifyEl;
   int                 NSiblings;
#endif
   int                 cpt, h;
   ThotBool            stop, inTop;
   ThotBool            ElemIsChild, ElemIsBefore, cut;

   PageHeaderRefAssoc = NULL;
   pElPage = NULL;
   pP = pAb;
   pP1 = pP->AbEnclosing;
   pEl = pP->AbElement;
   ElemIsChild = FALSE;
   if (position == 0)
     {
       /* teste si le pave est le premier d'un pave mis en lignes */
       if (pP1 != NULL && pP->AbPrevious == NULL)
	 {
	   if (pP1->AbInLine)
	     /* on inserera la marque de page avant le pave englobant */
	     pP = pP1;
	 }

       /* teste si le pave est en haut de son englobant */
       do
	 {
	   stop = TRUE;
	   inTop = FALSE;
	   if (pP->AbEnclosing != NULL)
	     {
	       pPos = &pP->AbVertPos;
	       if (pPos->PosAbRef == NULL)
		 {
		   /* postion verticale par defaut = en haut de l'englobant */
		   inTop = TRUE;
		   /* si l'englobant est mis en ligne et que le pave n'est pas */
		   /* le premier des paves mis en ligne, il n'est pas en haut */
		   /* de son englobant */
		   if (pP->AbEnclosing->AbInLine && pP->AbPrevious != NULL)
		     inTop = FALSE;
		 }
	       else if (pPos->PosAbRef == pP->AbEnclosing &&
			pPos->PosDistance == 0 && pPos->PosEdge == Top &&
			pPos->PosRefEdge == Top)
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
       while (!stop);

       pEl = pP->AbElement;
       /* on inserera la nouvelle marque de page avant pEl */
       ElemIsBefore = TRUE;

       /*on regarde s'il n'y a pas deja une marque de page juste avant pEl */
       if (pEl->ElPrevious != NULL)
	 {
	   /* On ignore les elements repetes en haut de page */
	   /* (tetieres de tableaux par exemple)             */
	   stop = FALSE;
	   do
	     if (pEl->ElPrevious == NULL)
	       stop = TRUE;
	     else if (pEl->ElPrevious->ElIsCopy &&
		      TypeHasException (ExcPageBreakRepetition, pEl->ElPrevious->ElTypeNumber, pEl->ElPrevious->ElStructSchema))
	       pEl = pEl->ElPrevious;
	     else
	       stop = TRUE;
	   while (!stop);
	 }

       pEl = PreviousLeaf (pEl);
       if (pEl != NULL && pEl->ElTerminal && pEl->ElLeafType == LtPageColBreak &&
	   pEl->ElViewPSchema == schView)
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
     }
   else if (position == 1)
       /* insert after */
       ElemIsBefore = FALSE;
   else
     {
       /* insert as first child */
       ElemIsBefore = FALSE;
       ElemIsChild = TRUE;
     }

   pEl = pP->AbElement;
   if (!ElemIsBefore && position == 0 && *origCutAbsBox != NULL)
	 /* Il y a un pave insecable plus haut qu'une page, on inserera */
	 /* la marque de page apres l'element de ce pave */
	 pEl = (*origCutAbsBox)->AbElement;

   if (pEl->ElParent == NULL)
      /* si pEl est la racine il faut descendre d'un niveau */
     if (ElemIsBefore)
       pEl = pEl->ElFirstChild;
     else if (!ElemIsChild)
       {
	 pEl = pEl->ElFirstChild;
	 while (pEl->ElNext != NULL)
	   pEl = pEl->ElNext;
       }

   if (ElemIsBefore)
     {
       if (pEl->ElTypeNumber == PageBreak + 1)
	 return pEl;
	pElPage = PageBrk (pEl, schView);
	if (pElPage != NULL)
	   /* on veut inserer un saut de page devant un element qui a la regle */
	   /* Page. On n'insere pas de nouveau saut de page et on retourne le */
	   /* saut de page de l'element suivant */
	   return pElPage;
     }

#ifndef PAGINEETIMPRIME
   /* envoie l'evenement ElemNew.Pre */
   notifyEl.event = TteElemNew;
   notifyEl.document = (Document) IdentDocument (pDoc);
   NSiblings = 0;
   if (ElemIsChild)
     notifyEl.element = (Element) (pP->AbElement);
   else
     {
       notifyEl.element = (Element) (pP->AbElement->ElParent);
       pF = pEl;
       while (pF->ElPrevious != NULL)
	 {
	   NSiblings++;
	   pF = pF->ElPrevious;
	 }
       if (!ElemIsBefore)
	 NSiblings++;
     }
   notifyEl.position = NSiblings;
   notifyEl.elementType.ElTypeNum = PageBreak + 1;
   notifyEl.elementType.ElSSchema = (SSchema) (rootEl->ElStructSchema);
   CallEventType ((NotifyEvent *) & notifyEl, TRUE);
#endif /* PAGINEETIMPRIME */
   /* cree l'element Marque de Page */
   pElPage = NewSubtree (PageBreak + 1, rootEl->ElStructSchema,
			 pDoc, pEl->ElAssocNum, TRUE, TRUE, TRUE, TRUE);
   /* insere l'element dans l'arbre abstrait */
   if (ElemIsChild)
     InsertFirstChild (pEl, pElPage);
   else if (pEl->ElParent != NULL)
     {
       if (ElemIsBefore)
	 InsertElementBefore (pEl, pElPage);
       else
	 InsertElementAfter (pEl, pElPage);
     }

   /* remplit l'element page cree' */
   pElPage->ElPageType = PgComputed;
   pElPage->ElViewPSchema = schView;
   /* cherche le compteur de page a appliquer */
   cpt = GetPageCounter (pElPage, schView, &pSchP);
   if (cpt == 0)		/* page non numerotee */
      /* on entretient un compteur de pages pour pouvoir afficher un */
      /* message indiquant la progression du formatage */
     {
	pagesCounter++;
	pElPage->ElPageNumber = pagesCounter;
     }
   else				/* calcule le numero de page */
      pElPage->ElPageNumber = CounterVal (cpt, pElPage->ElStructSchema, pSchP,
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
	pP1 = pElPage->ElAbstractBox[nbView - 1];
	if (position == 1)
	  {
	    pPos = &pP1->AbVertPos;
	    pPos->PosAbRef = pAb;
	    pPos->PosDistance = PixelToPoint (RealPageHeight - pAb->AbBox->BxYOrg - pAb->AbBox->BxHeight);
	    pPos->PosEdge = Top;
	    pPos->PosRefEdge = Bottom;
	    pPos->PosUnit = UnPoint;
	    pPos->PosUserSpecified = FALSE;
	    pP1->AbVertPosChange = TRUE;
	  }
	else if (position == 2)
	  {
	    pPos = &pP1->AbVertPos;
	    pPos->PosAbRef = pAb;
	    pPos->PosDistance = PixelToPoint (RealPageHeight - pAb->AbBox->BxYOrg);
	    pPos->PosEdge = Top;
	    pPos->PosRefEdge = Top;
	    pPos->PosUnit = UnPoint;
	    pPos->PosUserSpecified = FALSE;
	    pP1->AbVertPosChange = TRUE;
	  }
	(void) ChangeConcreteImage (frame, &h, modifAbsBox);
	pP1->AbOnPageBreak = FALSE;
	pP1->AbAfterPageBreak = FALSE;
	/* les paves devant lesquels on a mis la marque de page ne doivent */
	/* plus etre traites. On les marque hors page. */
	if (ElemIsBefore)
	  {
	    stop = FALSE;
	    do
	      {
		pAb->AbOnPageBreak = FALSE;
		pAb->AbAfterPageBreak = TRUE;
		if (pAb == pP)
		  stop = TRUE;
		else
		  pAb = pAb->AbEnclosing;
	      }
	    while (!stop);
	  }
	else if (!ElemIsChild)
	  {
	    pAb->AbOnPageBreak = FALSE;
	    pAb->AbAfterPageBreak = FALSE;
	  }
     }
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
static int          MoveCut (PtrAbstractBox pAb, ThotBool NoBr1, int schView)
#else  /* __STDC__ */
static int          MoveCut (pAb, NoBr1, schView)
PtrAbstractBox      pAb;
ThotBool            NoBr1;
int                 schView;
#endif /* __STDC__ */
{
  int                 ret, High, PosV, cutChar, Min, i;
  PtrPRule            pRNoBr1, pRNoBr2;
  PtrAttribute        pA1, pA2;
  ThotBool            cutAbsBox;
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
		  Min = PixelValue (i, pRe1->PrMinUnit, pAb, 0);
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
	    else if (pAb->AbInLine)
	      cutAbsBox = FALSE;
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
		Min = PixelValue (i, pRe1->PrMinUnit, pAb, 0);
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
      while (ret == 0 && pAb != NULL)
	{
	  ret = MoveCut (pAb, NoBr1, schView);
	  pAb = pAb->AbNext;
	}
    }
  return ret;
}


/*----------------------------------------------------------------------
  SetMark place dans l'arbre de racine pAb la marque de page en fonction
  de la position des paves relativement a la limite de page.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         SetMark (PtrAbstractBox pAb, PtrElement rootEl, PtrDocument pDoc, int schView, ThotBool * absBoxTooHigh, PtrAbstractBox * origCutAbsBox, int nbView, int frame, PtrElement * pPage)
#else  /* __STDC__ */
static void         SetMark (pAb, rootEl, pDoc, schView, absBoxTooHigh, origCutAbsBox, nbView, frame, pPage)
PtrAbstractBox      pAb;
PtrElement          rootEl;
PtrDocument         pDoc;
int                 schView;
ThotBool           *absBoxTooHigh;
PtrAbstractBox     *origCutAbsBox;
int                 nbView;
int                 frame;
PtrElement         *pPage;
#endif /* __STDC__ */
{
  int                 High, PosV, cutChar;
  ThotBool            done;
  PtrAbstractBox      pChild;
  ThotBool            toCut;
  PtrAbstractBox      pCreator;

  if (pAb != NULL)
    {
      if (pAb->AbOnPageBreak)
	/* le pave' est traverse' par la limite de page */
	if (pAb->AbFirstEnclosed == NULL)
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
		else if (pAb->AbLeafType == LtGraphics && pCreator->AbFirstEnclosed != NULL)
		  /* c'est un graphique de presentation d'un pave compose */
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
				       absBoxTooHigh, schView, pDoc, rootEl, 0);
		else if (cutChar >= pAb->AbElement->ElTextLength)
		  /* la coupure tombe a la fin du pave */
		  pAb->AbOnPageBreak = FALSE;
		else
		  /* coupe l'element de texte */
		  {
		    Cut (pAb->AbElement, cutChar, pDoc, nbView);
		    pAb->AbOnPageBreak = FALSE;
		    pAb = pAb->AbNext;
		    pAb->AbAfterPageBreak = TRUE;
		    *pPage = InsertMark (pAb, frame, nbView,
					 origCutAbsBox, absBoxTooHigh,
					 schView, pDoc, rootEl, 0);
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
		    if (TypeHasException (ExcIsCell, pAb->AbElement->ElTypeNumber, pAb->AbElement->ElStructSchema) &&
			!done)
		      {
			/* the cell has to be split but the page break was not given */
			/* look for the abstract box of the cell and skip
			 presentation boxes */
			pCreator = pAb->AbElement->ElAbstractBox[pAb->AbDocView - 1];
			while (pCreator->AbPresentationBox)
			  pCreator = pCreator->AbNext;
			/* insert the page break at the bottom of the cell */
			pChild = pCreator->AbFirstEnclosed;
			/* locate the last child */
			while (pChild != NULL && pChild->AbNext != NULL)
			  pChild = pChild->AbNext;
			/* skip bacwards presentation boxes of the cell */
			while (pChild != NULL && pChild->AbElement == pCreator->AbElement)
			  pChild = pChild->AbPrevious;
			if (pChild == NULL)
			  /* insert as a child */
			  *pPage = InsertMark (pCreator, frame, nbView, origCutAbsBox,
				      absBoxTooHigh, schView, pDoc, rootEl, 2); 
			else
			  *pPage = InsertMark (pChild, frame, nbView, origCutAbsBox,
				      absBoxTooHigh, schView, pDoc, rootEl, 1);
			pCreator->AbOnPageBreak = TRUE;
			done = (*pPage != NULL);
		      }
		  }
		else if (pAb->AbAfterPageBreak)
		  if (!done)
		    /* c'est le premier pave englobe' au-dela de la */
		    /* frontiere, on pose une marque de page devant lui */
		    {
		      *pPage = InsertMark (pAb, frame, nbView, origCutAbsBox,
					   absBoxTooHigh, schView, pDoc, rootEl, 0);
		      done = TRUE;
		      pAb = NULL;
		    }
		if (pAb != NULL)
		  pAb = pAb->AbNext;
	      }
	  }
    }
}


/*----------------------------------------------------------------------
  SetPage place la marque de page en respectant la hauteur de page
  demandee et les conditions de coupure des paves de la page.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         SetPage (PtrElement * pPage, int frame, PtrAbstractBox * origCutAbsBox, ThotBool * absBoxTooHigh, PtrDocument pDoc, int schView, int nbView, PtrElement rootEl)
#else  /* __STDC__ */
static void         SetPage (pPage, frame, origCutAbsBox, absBoxTooHigh, pDoc, schView, nbView, rootEl)
PtrElement         *pPage;
int                 frame;
PtrAbstractBox     *origCutAbsBox;
ThotBool           *absBoxTooHigh;
PtrDocument         pDoc;
int                 schView;
int                 nbView;
PtrElement          rootEl;
#endif /* __STDC__ */
{
  int                 turn, newPageHight, oldPageHight;
  ThotBool            NoBr1;

#ifdef PRINT_DEBUG
FILE     *list;
CHAR_T      localname[50];
static int       n = 1;

   sprintf (localname, "/home/stephane/.amaya/printpage%d.debug", n);
   n++;
   list = fopen (localname, "w");
   TtaListBoxes (1, 1, list);
   fclose (list);
#endif
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
      while (newPageHight != 0);
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
{
   PtrAbstractBox      pAb;
   PtrAbstractBox      origCutAbsBox;
   PtrElement          pPage;
   PtrElement          pElLib;
   PtrAbstractBox      previousAbsBox, redispAb;
   int                 High, PosV, putVThread, cutChar, h, dh, normalPageHeight;
   ThotBool            stop;
   ThotBool            absBoxTooHigh;

#ifdef PRINT_DEBUG
   FILE     *list;
   CHAR_T      localname[50];
   static int       n = 1;
   
   sprintf (localname, "/home/stephane/.amaya/print%d.debug", n);
   n++;
   list = fopen (localname, "w");
   TtaListBoxes (1, 1, list);
   fclose (list);
#endif
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
	   pAb = pAb->AbFirstEnclosed;
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
	   while (!stop);
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
			redispAb = pDoc->DocViewModifiedAb[nbView - 1];
		     else
			redispAb = pDoc->DocAssocModifiedAb[rootEl->ElAssocNum - 1];
		     /*RealPageHeight = PageHeight;*/
		     (void) ChangeConcreteImage (frame, &RealPageHeight, redispAb);
		     /* libere tous les paves morts de la vue */
		     FreeDeadAbstractBoxes (pAb, frame);
		     /* detruit la marque de page a liberer dans l'arbre abstrait */
		     SuppressPageMark (pPage, pDoc, &pElLib);
		     /* signale au Mediateur les paves morts par suite de */
		     /* fusion des elements precedent et suivant les marques */
		     /* supprimees. */
		     if (!AssocView (rootEl))
			redispAb = pDoc->DocViewModifiedAb[nbView - 1];
		     else
			redispAb = pDoc->DocAssocModifiedAb[rootEl->ElAssocNum - 1];
		     if (redispAb != NULL)
		       {
			  h = RealPageHeight;
			  (void) ChangeConcreteImage (frame, &h, redispAb);
		       }
		     /* libere les elements rendus inutiles par les fusions */
		     DeleteElement (&pElLib, pDoc);
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
}




/*----------------------------------------------------------------------
   	DetrImAbs detruit l'image abstraite de la vue concernee et	
   		efface sa frame si la vue est une vue pour VusSch	
    Vue = numero d'elt assoc si vue associee sinon      
    Vue = numero de vue si vue d'arbre principal        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         DestroyImAbsPages (int view, ThotBool Assoc, PtrDocument pDoc, int schView)
#else  /* __STDC__ */
static void         DestroyImAbsPages (view, Assoc, pDoc, schView)
int                 view;
ThotBool            Assoc;
PtrDocument         pDoc;
int                 schView;
#endif /* __STDC__ */
{
   PtrAbstractBox      pAb;
   int                 h;

   /* ThotBool       tropcourt; */
   int                 frame;
   PtrAbstractBox      rootAbsBox;
   ThotBool            toDestroy;

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
	h = 0;
	(void) ChangeConcreteImage (frame, &h, rootAbsBox);
	/* libere tous les paves morts de la vue */
	FreeDeadAbstractBoxes (rootAbsBox, frame);
	/* indique qu'il faudra reappliquer les regles de presentation du */
	/* pave racine, par exemple pour recreer les boites de presentation */
	/* creees par lui et qui viennent d'etre detruites. */
	rootAbsBox->AbSize = -1;
	/* on marque le pave racine complet en tete pour que AbsBoxesCreate */
	/* engendre effectivement les paves de presentation cree's en tete */
	/* par l'element racine (regles CreateFirst). */
	if (rootAbsBox->AbLeafType == LtCompound)
	   rootAbsBox->AbTruncatedHead = FALSE;

     }				/* fin toDestroy */
}


/*----------------------------------------------------------------------
   AddLastPageBreak	ajoute une marque de page a la fin de la vue	
   	schView de l'arbre de racine pRootEl s'il n'y en a pas deja une
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                AddLastPageBreak (PtrElement pRootEl, int schView, PtrDocument pDoc,
				      ThotBool withAPP)
#else  /* __STDC__ */
void                AddLastPageBreak (pRootEl, schView, pDoc, withAPP)
PtrElement          pRootEl;
int                 schView;
PtrDocument         pDoc;
ThotBool            withAPP;
#endif /* __STDC__ */

{
   PtrElement          pEl;
   PtrElement          pElPage;
   PtrPSchema          pSchP;
   int                 cpt;
   ThotBool            pageAtEnd;
   ThotBool            stop, stop1, ok;
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

/*----------------------------------------------------------------------
  PaginateView l'utilisateur demande le (re)decoupage en pages de la	
  vue de numero Vue pour le document pointe' par pDoc.	
  Si Assoc est vrai, c'est la vue d'elements associes de	
  numero Vue qui doit etre traitee			
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                PaginateView (PtrDocument pDoc, int view, ThotBool Assoc)
#else  /* __STDC__ */
void                PaginateView (pDoc, view, Assoc)
PtrDocument         pDoc;
int                 view;
ThotBool            Assoc;
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
   ThotBool            sel;
#endif /* PAGINEETIMPRIME */
   int                 schView;
   int                 v, clipOrg;
   int                 frame, volume, volprec, nbView, cpt;
   ThotBool            tooShort;
   ThotBool            complete;
   ThotBool            isFirstPage;
   PtrPSchema          pSchPage;
   int                 b;
   ThotBool            nothingAdded;

   RunningPaginate = TRUE;
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

#ifndef PAGINEETIMPRIME
   sel = AbortPageSelection (pDoc, schView, &firstSelection, &lastSelection, &FirstSelectedChar, &LastSelectedChar);
#endif /* PAGINEETIMPRIME */
   if (Assoc)
     {
       /* detruit l'image abstraite de la vue concernee, en conservant la racine */
       DestroyImAbsPages (view, Assoc, pDoc, schView);
       /* destruction des marques de page */
       DestroyPageMarks (pDoc, pRootEl, schView);
       pDoc->DocAssocFreeVolume[view - 1] = pDoc->DocAssocVolume[view - 1];
     }
   else
     {
       /* detruit l'image abstraite de la vue concernee, en conservant la racine */
       for (v = 1; v <= MAX_VIEW_DOC; v++)
	 DestroyImAbsPages (v, Assoc, pDoc, schView);
       /* destruction des marques de page */
       DestroyPageMarks (pDoc, pRootEl, schView);
       pDoc->DocViewFreeVolume[view - 1] = pDoc->DocViewVolume[view - 1];
     }
       
   PageHeight = 0;
   PageFooterHeight = 0;	/* cree les paves du debut de la vue */
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
		   PrintOnePage (pDoc, previousPageAbBox, pP, rootAbsBox, clipOrg);
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
	       /* si la marque de page existait auparavant, on la renumerote */
	       if (! (pPage->ElPageType == PgComputed))
		 {
		 /* cherche le compteur de page a appliquer a cette page */
		   cpt = GetPageCounter (pPage, schView, &pSchP);
		   if (cpt == 0)
		     /* page non numerotee */
		     {
		       pagesCounter++;
		       pPage->ElPageNumber = pagesCounter;
		     }
		   else
		     {
		       /* calcule le numero de page */
		       pPage->ElPageNumber = CounterVal (cpt, 
							 pPage->ElStructSchema, 
							 pSchP, 
							 pPage, 
							 schView);
		       /* on met a jour les boites de presentation des compteurs des */
		       /* pages suivantes dans le cas de la pagination depuis l'impression */
		       /* cet appel est fait tout a la fin dans le cas d'une pagination */
		       /* normale */
		       
		       UpdateNumbers (pPage, pPage, pDoc, TRUE);
		     }
		   PageHeaderFooter (pPage, schView, &b, &pSchPage);
		 }
	       PrintOnePage (pDoc, previousPageAbBox, pPage->ElAbstractBox[nbView - 1],
			     rootAbsBox, clipOrg);
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
   /* quand on sort de la boucle, previousPageAbBox pointe sur le pave de la */
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
       PrintOnePage (pDoc, previousPageAbBox, pP, rootAbsBox, clipOrg);
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
   SetDocumentModified (pDoc, TRUE, 0);
}
/* End Of Module page */
