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
 * Module de manipulations des images abstraites.
 *
 * Authors: V. Quint (INRIA)
 *          C. Roisin (INRIA) - Columns and pages
 *
 */


#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "picture.h"

#define THOT_EXPORT extern
#include "page_tv.h"
#include "tree_f.h"
#include "createabsbox_f.h"
#include "createpages_f.h"
#include "scroll_f.h"
#include "structlist_f.h"
#include "views_f.h"
#include "viewcommands_f.h"
#include "absboxes_f.h"
#include "abspictures_f.h"
#include "buildboxes_f.h"
#include "memory_f.h"
#include "changeabsbox_f.h"
#include "presrules_f.h"
#include "structselect_f.h"
#include "content_f.h"


#define MaxAsc 30

static char         text[MAX_TXT_LEN];

#ifdef __STDC__
extern void         DisplayFrame (int);

#else  /* __STDC__ */
extern void         DisplayFrame ();

#endif /* __STDC__ */

/*----------------------------------------------------------------------
   AbsBoxType  rend un pointeur sur un buffer qui contient           
   le type de l'element de structure auquel correspond le  
   pave pointe' par pAb.                                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
char               *AbsBoxType (PtrAbstractBox pAb)

#else  /* __STDC__ */
char               *AbsBoxType (pAb)
PtrAbstractBox      pAb;

#endif /* __STDC__ */

{
   SRule              *pR;
   PresentationBox    *pBo1;
   PtrElement          pEl;

   if (pAb == NULL)
      strcpy (text, " ");
   else
     {
	if (pAb->AbPresentationBox)
	  {
	     pR = &pAb->AbElement->ElStructSchema->SsRule[pAb->AbElement->ElTypeNumber - 1];
	     /* copie le nom du type d'element structure qui a cree la boite */
	     strcpy (text, pR->SrName);
	     strcat (text, ".");
	     /* copie a la suite le nom du type de boite de presentation */
	     pBo1 = &pAb->AbPSchema->PsPresentBox[pAb->AbTypeNum - 1];
	     strcat (text, pBo1->PbName);
	  }
	else
	   /* pave d'un element de structure */
	  {
	     pEl = pAb->AbElement;
	     strcpy (text, pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrName);
	  }
     }
   return (text);
}



/*----------------------------------------------------------------------
   LibAbbView libere, pour une seule vue, tous les paves            
   englobes par le pave pointe par pAb, lui-meme compris. 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                LibAbbView (PtrAbstractBox pAb)
#else  /* __STDC__ */
void                LibAbbView (pAb)
PtrAbstractBox      pAb;
#endif /* __STDC__ */
{
  PtrAbstractBox      pAbb, pAbbNext;
  PtrTextBuffer       pBT, pBTSuiv;
  PtrDelayedPRule     pDelPR, pNextDelPR;
  PtrElement          pEl;
  boolean             libAb;
#ifdef __COLPAGE__
  boolean             ok;
#else  /* __COLPAGE__ */
  PresentationBox    *pBox;
  PtrDocument         pDoc;
  int                 assoc;
#endif /* __COLPAGE__ */

  if (pAb != NULL)
    {
      pAbb = pAb->AbFirstEnclosed;
      /* libere tous les paves englobes */
      while (pAbb != NULL)
	{
	     pAbbNext = pAbb->AbNext;
	     LibAbbView (pAbb);
	     pAbb = pAbbNext;
	}
      if (pAb->AbBox != NULL)
	{
	  pEl = pAb->AbElement;
	  printf ("Box non liberee: %s", pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrName);
	  if (pAb->AbPresentationBox)
	    printf (".%s\n", pAb->AbPSchema->PsPresentBox[pAb->AbTypeNum - 1].PbName);
	     else
	       printf ("\n");
	}
#ifdef __COLPAGE__
      /* debug */
      ok = AbsBoxOk (pAb);
      if (!ok)
#endif /* __COLPAGE__ */
	/* dechaine pAb des autres paves */
	if (pAb->AbNext != NULL)
	  pAb->AbNext->AbPrevious = pAb->AbPrevious;
      if (pAb->AbPrevious != NULL)
	pAb->AbPrevious->AbNext = pAb->AbNext;
      if (pAb->AbEnclosing != NULL)
	if (pAb->AbEnclosing->AbFirstEnclosed == pAb)
	  pAb->AbEnclosing->AbFirstEnclosed = pAb->AbNext;
#ifdef __COLPAGE__
      if (pAb->AbNextRepeated != NULL)
	pAb->AbNextRepeated->AbPreviousRepeated = pAb->AbPreviousRepeated;
      if (pAb->AbPreviousRepeated != NULL)
	pAb->AbPreviousRepeated->AbNextRepeated = pAb->AbNextRepeated;
      /* debug */
      ok = AbsBoxOk (pAb);
      if (!ok)
	printf ("erreur apres dechainage LibAbbView \n");
#endif /* __COLPAGE__ */
      /* Si c'est un pave obtenu par la regle de presentation Copy,
	 libere le descripteur d'element copie' */
      if (pAb->AbCopyDescr != NULL)
	{
	  if (pAb->AbCopyDescr->CdPrevious == NULL)
	    pAb->AbCopyDescr->CdCopiedElem->ElCopyDescr = pAb->AbCopyDescr->CdNext;
	  else
	    pAb->AbCopyDescr->CdPrevious->CdNext = pAb->AbCopyDescr->CdNext;
	  if (pAb->AbCopyDescr->CdNext != NULL)
	    pAb->AbCopyDescr->CdNext->CdPrevious = pAb->AbCopyDescr->CdPrevious;
	  FreeDescCopy (pAb->AbCopyDescr);
	}
      /* si c'est un pave de presentation ou le pave d'une reference ou */
      /* celui d'une marque de paire, on libere les buffers */
      libAb = FALSE;
      if (pAb->AbPresentationBox)
	if (pAb->AbLeafType == LtText || pAb->AbLeafType == LtPolyLine ||
	    pAb->AbLeafType == LtPicture)
	  libAb = TRUE;
      if (!libAb)
	if (!pAb->AbPresentationBox)
	  if (pAb->AbElement != NULL)
	    if (pAb->AbElement->ElTerminal)
	      if (pAb->AbElement->ElLeafType == LtReference ||
		  pAb->AbElement->ElLeafType == LtPairedElem)
		libAb = TRUE;
      if (libAb)
	switch (pAb->AbLeafType)
	  {
	  case LtText:
	  case LtPolyLine:
	    if (pAb->AbLeafType == LtPolyLine)
	      pBT = pAb->AbPolyLineBuffer;
	    else
	      pBT = pAb->AbText;
	    while (pBT != NULL)
	      {
		pBTSuiv = pBT->BuNext;
		DeleteTextBuffer (&pBT);
		pBT = pBTSuiv;
	      }
	    break;
	  case LtPicture:
	    if (!pAb->AbElement->ElTerminal || pAb->AbElement->ElLeafType != LtPicture)
	      {
		/* ce n'est pas un element image */
		FreePictInfo (pAb->AbPictInfo);
		pAb->AbPictInfo = NULL;
	      }
	  case LtCompound:
	    if (pAb->AbPictBackground != NULL)
	      {
		/* in this particular case we need to free filename */
		TtaFreeMemory (((PictInfo *)(pAb->AbPictBackground))->PicFileName);
		/* ce n'est pas un element image */
		FreePictInfo (pAb->AbPictBackground);
		pAb->AbPictBackground = NULL;
	      }
	    break;
	  default:
	    break;
	  }

      /* dechaine pAb de son element */
      if (pAb->AbElement != NULL)
        if (pAb->AbElement->ElAbstractBox[pAb->AbDocView - 1] == pAb)
#ifdef __COLPAGE__
	if (!pAb->AbPresentationBox)
	  /* si le pave pAb est le pave principal (non presentation) */
	  /* l'elt va pointer sur le dup (s'il existe) */
	  /* On ne considere pas les paves de pres rep suivants TODO ? */
	  pAb->AbElement->ElAbstractBox[pAb->AbDocView - 1] = pAb->AbNextRepeated;
	else
#endif /* __COLPAGE__ */
	  if (pAb->AbNext != NULL)
	    if (pAb->AbNext->AbElement == pAb->AbElement)
	      pAb->AbElement->ElAbstractBox[pAb->AbDocView - 1] = pAb->AbNext;
	    else
	      pAb->AbElement->ElAbstractBox[pAb->AbDocView - 1] = NULL;
	  else
#ifdef __COLPAGE__
	    /* inutile ? car un pave de pres ne peut avoir de dup */
	    pAb->AbElement->ElAbstractBox[pAb->AbDocView - 1] = pAb->AbNextRepeated;
      /* fin du cas ou l'element pointait sur le pave */
#else  /* __COLPAGE__ */
            pAb->AbElement->ElAbstractBox[pAb->AbDocView - 1] = NULL;
#endif /* __COLPAGE__ */
#ifndef __COLPAGE__
      /* Ce code ne marche pas avec CP */
      /* il est inutile car le pave de la racine des elements associes */
      /* (places en haut ou bas de page) n'est plus considere comme un */
      /* pave de presentation : son dechainage de l'element se fait */
      /* comme tout pave d'element */
      /* (il y a un niveau de pave en plus) */
      else
	/* est-ce une boite de haut ou bas de page contenant des */
	/* elements associes ? */
	if (pAb->AbPresentationBox && pAb->AbElement->ElTypeNumber == PageBreak + 1)
	  {
	    pBox = &pAb->AbPSchema->PsPresentBox[pAb->AbTypeNum - 1];
	    if ((pBox->PbPageHeader || pBox->PbPageFooter) &&
		pBox->PbContent == ContElement)
	      /* c'est bien une boite de haut ou bas de page contenant des */
	      /* elements associes. Le pave est-il associe' a l'un des */
	      /* elements associes du document ? */
	      {
		pDoc = DocumentOfElement (pAb->AbElement);
		if (pDoc != NULL)
		  {
		    for (assoc = 0; assoc < MAX_ASSOC_DOC; assoc++)
		      if (pDoc->DocAssocRoot[assoc] != NULL)
			if (pDoc->DocAssocRoot[assoc]->ElAbstractBox[pAb->AbDocView - 1] == pAb)
			  /* cet element n'a plus de pave */
			  pDoc->DocAssocRoot[assoc]->ElAbstractBox[pAb->AbDocView - 1] = NULL;
		  }
	      }
	  }
	else
	  /* est-ce un pave de presentation cree' par une regle FnCreateEnclosing */
	  if (pAb->AbPresentationBox)
	    if (pAb->AbElement->ElAbstractBox[pAb->AbDocView - 1] != NULL)
	      if (pAb->AbElement->ElAbstractBox[pAb->AbDocView - 1]->AbEnclosing == pAb)
		pAb->AbElement->ElAbstractBox[pAb->AbDocView - 1] = NULL;
#endif /* __COLPAGE__ */
#ifdef __COLPAGE__
      /* debug */
      ok = AbsBoxOk (pAb);
      if (!ok)
	printf ("erreur avant dechainage elem LibAbbView \n");
#endif /* __COLPAGE__ */
      /* libere les regles retardees qui n'ont pas ete appliquees */
      if (pAb->AbDelayedPRule != NULL)
	{
	  pDelPR = pAb->AbDelayedPRule;
	  while (pDelPR != NULL)
	    {
	      pNextDelPR = pDelPR->DpNext;
	      FreeDifferedRule (pDelPR);
	      pDelPR = pNextDelPR;
	    }
	}
      FreeAbstractBox (pAb);
    }
}


/*----------------------------------------------------------------------
   LibAbbEl libere, dans toutes les vues, tous les paves de      
   l'element pointe par pEl.                               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                LibAbbEl (PtrElement pEl)
#else  /* __STDC__ */
void                LibAbbEl (pEl)
PtrElement          pEl;
#endif /* __STDC__ */
#ifdef __COLPAGE__
{
   int                 v;
   PtrAbstractBox      pAbb, pAbbNext;
   PtrAbstractBox      pDupNext;
   boolean             stop;
   boolean             stop1;

   /* ce code ne traite que le cas du pave de l'element et de */
   /* paves de presentation avant et apres */
   /* prevoir cas des paves de presentation de la racine ?? TODO */
   if (pEl != NULL)
      for (v = 0; v < MAX_VIEW_DOC; v++)
	{
	   pAbb = pEl->ElAbstractBox[v];
	   if (pAbb != NULL)
	      if (pAbb->AbEnclosing != NULL)
		 if (pAbb->AbEnclosing->AbPresentationBox &&
		     pAbb->AbEnclosing->AbElement == pEl)
		    /* le pave englobant est un pave' de presentation cree' par */
		    /* une regle FnCreateEnclosing */
		    pAbb = pAbb->AbEnclosing;
	   stop1 = FALSE;
	   pDupNext = NULL;
	   do
	     {
		stop = FALSE;
		do
		   if (pAbb == NULL)
		      stop = TRUE;
		   else if (pAbb->AbElement != pEl)
		      stop = TRUE;
		   else
		     {
			pAbbNext = pAbb->AbNext;
			pDupNext = pAbb->AbNextRepeated;
			LibAbbView (pAbb);
			pAbb = pAbbNext;
		     }
		while (!stop);
		if (pDupNext != NULL)	/* on parcourt la liste des dupliques */
		   pAbb = pDupNext;
		else
		   stop1 = TRUE;
	     }
	   while (!stop1);
	}
}

#else  /* __COLPAGE__ */

{
   int                 v;
   PtrAbstractBox      pAbb, pAbbNext;
   boolean             stop;

   if (pEl != NULL)
      for (v = 0; v < MAX_VIEW_DOC; v++)
	{
	   pAbb = pEl->ElAbstractBox[v];
	   if (pAbb != NULL)
	      if (pAbb->AbEnclosing != NULL)
		 if (pAbb->AbEnclosing->AbPresentationBox &&
		     pAbb->AbEnclosing->AbElement == pEl)
		    /* le pave englobant est un pave' de presentation cree' par */
		    /* une regle FnCreateEnclosing */
		    pAbb = pAbb->AbEnclosing;
	   stop = FALSE;
	   do
	      if (pAbb == NULL)
		 stop = TRUE;
	      else if (pAbb->AbElement != pEl)
		 stop = TRUE;
	      else
		{
		   pAbbNext = pAbb->AbNext;
		   LibAbbView (pAbb);
		   pAbb = pAbbNext;
		}
	   while (!stop);
	}
}

#endif /* __COLPAGE__ */


/*----------------------------------------------------------------------
   FreeDeadAbstractBoxes libere tous les paves marques Mort dans le           
   sous-arbre de racine pAb.                              
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FreeDeadAbstractBoxes (PtrAbstractBox pAb)
#else  /* __STDC__ */
void                FreeDeadAbstractBoxes (pAb)
PtrAbstractBox      pAb;
#endif /* __STDC__ */
{
   PtrAbstractBox      pAbb, pAbbNext;


   if (pAb != NULL)
      if (pAb->AbDead)
	 LibAbbView (pAb);
      else
	{
	   pAbb = pAb->AbFirstEnclosed;
	   /* teste tous les paves englobes */
	   while (pAbb != NULL)
	     {
		pAbbNext = pAbb->AbNext;
		FreeDeadAbstractBoxes (pAbb);
		pAbb = pAbbNext;
	     }
	}
}

#ifdef __COLPAGE__
 /* procedure de reevaluation des regles recursives */
 /*----------------------------------------------------------------------
    RecursEvalCP appelle ApplyRefAbsBoxNew pour tous les paves 
    du sous-arbre pAb                     
   ----------------------------------------------------------------------*/
#ifdef __STDC__
void                RecursEvalCP (PtrAbstractBox pAb, PtrDocument pDoc)
#else  /* __STDC__ */
void                RecursEvalCP (pAb, pDoc)
PtrAbstractBox      pAb;
PtrDocument         pDoc;
#endif /* __STDC__ */
{
   PtrAbstractBox      pAbb, pAbbR;

   pAbb = pAb;
   if (pAbb != NULL)
     {
	ApplyRefAbsBoxNew (pAbb, pAbb, &pAbbR, pDoc);
	pAbb = pAbb->AbFirstEnclosed;
	while (pAbb != NULL)
	  {
	     RecursEvalCP (pAbb, pDoc);
	     pAbb = pAbb->AbNext;
	  }
     }
}
 /*fin de procedure RecursEval */
#endif /* __COLPAGE__ */


/*----------------------------------------------------------------------
   AddAbsBoxes complete la vue dont pAbbRoot est le pave racine   
   en ajoutant des paves, en tete si head est vrai,        
   en queue sinon.                                         
    pAbbRoot est une vraie racine de paves               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                AddAbsBoxes (PtrAbstractBox pAbbRoot, PtrDocument pDoc, boolean head)
#else  /* __STDC__ */
void                AddAbsBoxes (pAbbRoot, pDoc, head)
PtrAbstractBox      pAbbRoot;
PtrDocument         pDoc;
boolean             head;
#endif /* __STDC__ */
{
   PtrAbstractBox      pAb;
   PtrElement          pEl;
   boolean             complete;
#ifndef __COLPAGE__
   PtrAbstractBox      pAbbReDisp, pAbbR, PcFirst, PcLast, pAbb;
   boolean             stop;
#endif  /* __COLPAGE__ */

   if ((head && pAbbRoot->AbTruncatedHead) ||
       (!head && pAbbRoot->AbTruncatedTail))
     {
       /* cree les paves de la partie coupee jusqu'a concurrence du volume libre */
       pEl = pAbbRoot->AbElement;
       pAb = AbsBoxesCreate (pEl, pDoc, pAbbRoot->AbDocView, !head, TRUE, &complete);
#ifdef __COLPAGE__
       /* on reapplique les regles  a tous les paves */
       /* TO DO a affiner ! */ RecursEvalCP (pAbbRoot, pDoc);
       if (AssocView (pEl))
	 pDoc->DocAssocModifiedAb[pEl->ElAssocNum - 1] = pAbbRoot;
       else
	 pDoc->DocViewModifiedAb[pAbbRoot->AbDocView - 1] = pAbbRoot;
     }
#else  /* __COLPAGE__ */
       /* recherche tous les paves crees, a partir du premier pave de plus */
       /* haut niveau cree', et aux niveaux inferieurs. */
       while (pAb != NULL)
	 {
	   /* cherche le premier et le dernier freres crees a ce niveau */
	   PcFirst = pAb;
	   PcLast = pAb;
	   stop = FALSE;
	   do
	     if (PcLast->AbNext == NULL)
	       stop = TRUE;
	     else if (!PcLast->AbNext->AbNew)
	       stop = TRUE;
	     else
	       PcLast = PcLast->AbNext;
	   while (!stop);
	   stop = FALSE;
	   do
	     if (PcFirst->AbPrevious == NULL)
	       stop = TRUE;
	     else if (!PcFirst->AbPrevious->AbNew)
	       stop = TRUE;
	     else
	       PcFirst = PcFirst->AbPrevious;
	   while (!stop);
	   if (PcFirst == PcLast)
	     /* un seul pave cree a ce niveau, c'est lui qu'il faudra */
	     /* reafficher. */
	     pAbbReDisp = pAb;
	   else
	     /* plusieurs paves crees, on reaffichera l'englobant */
	     pAbbReDisp = pAb->AbEnclosing;
	   /* modifie les paves environnant les paves crees */
	   ApplyRefAbsBoxNew (PcFirst, PcLast, &pAbbR, pDoc);
	   pAbbReDisp = Enclosing (pAbbReDisp, pAbbR);	/* conserve le pointeur sur le pave a reafficher */
	   
	   pEl = pAbbRoot->AbElement;
	   if (AssocView (pAbbRoot->AbElement))
	     pDoc->DocAssocModifiedAb[pEl->ElAssocNum - 1] =
	       Enclosing (pAbbReDisp, pDoc->DocAssocModifiedAb[pEl->ElAssocNum - 1]);
	   else
	     pDoc->DocViewModifiedAb[pAbbRoot->AbDocView - 1] =
	       Enclosing (pAbbReDisp, pDoc->DocViewModifiedAb[pAbbRoot->AbDocView - 1]);
	   /* passe au niveau inferieur */
	   if (head)
	     pAb = PcLast->AbNext;
	   else
	     pAb = PcFirst->AbPrevious;
	   if (pAb != NULL)
	     {
	       /* saute les paves de presentation produits par CreateWith */
	       if (head)
		 while (pAb->AbPresentationBox && pAb->AbNext != NULL)
		   pAb = pAb->AbNext;
	       else
		 while (pAb->AbPresentationBox && pAb->AbPrevious != NULL)
		   pAb = pAb->AbPrevious;
	       /* passe au premier pave fils */
	       pAb = pAb->AbFirstEnclosed;
	       if (pAb != NULL)
		 if (head)
		   if (pAb->AbNew)
		     {
		       /* verifie s'il y a un pave ancien a ce niveau */
		       pAbb = pAb;
		       stop = FALSE;
		       do
			 if (pAbb == NULL)
			   {
			     /* pas d'ancien, on s'arrete si l'englobant est */
			     /* lui-meme un nouveau pave */
			     if (pAb->AbEnclosing->AbNew)
			       pAb = NULL;
			     stop = TRUE;
			   }
			 else if (!pAbb->AbNew)
			   stop = TRUE;
			 else
			   pAbb = pAbb->AbNext;
		       while (!stop);
		     }
		   else
		     {
		       /* cherche le premier niveau ou il y a un nouveau en tete */
		       stop = FALSE;
		       do
			 if (pAb == NULL)
			   stop = TRUE;
			 else if (pAb->AbNew)
			   stop = TRUE;
			 else
			   {
			     /* saute les paves de presentation produits par CreateWith */
			     while (pAb->AbPresentationBox && pAb->AbNext != NULL)
			       pAb = pAb->AbNext;
			     pAb = pAb->AbFirstEnclosed;
			   }
		       while (!stop);
		     }
		 else
		   {
		     /* on a ajoute' en queue */
		     /* on cherche le premier cree' a ce niveau */
		     if (pAb->AbNew)
		       {
			 /* il n'y a que des paves nouveaux, on arrete si */
			 /* l'englobant est lui-meme nouveau */
			 if (pAb->AbEnclosing->AbNew)
			   pAb = NULL;
		       }
		     else
		       {
			 stop = FALSE;
			 do
			   if (pAb == NULL)
			     stop = TRUE;
			   else if (pAb->AbNew)
			     stop = TRUE;
			   else if (pAb->AbNext == NULL)
			     {
			       /* saute les paves de presentation produits par CreateWith */
			       while (pAb->AbPresentationBox && pAb->AbPrevious != NULL)
				 pAb = pAb->AbPrevious;
			       pAb = pAb->AbFirstEnclosed;
			     }
			   else
			     pAb = pAb->AbNext;
			 while (!stop);
		       }
		     /* cherche le dernier  cree' */
		     if (pAb != NULL)
		       while (pAb->AbNext != NULL)
			 pAb = pAb->AbNext;
		   }
	     }
	 }
#endif /* __COLPAGE__ */
    }
}


/*----------------------------------------------------------------------
   IsBreakable retourne vrai si le pave pointe par pAb est         
   englobe (a n'importe quel niveau) par un pave           
   non mis en ligne ou explicitement secable. Les marques  
   de page son traitees comme non-secables.                
   V4 : Les paves de page sont traites comme secables.     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             IsBreakable (PtrAbstractBox pAb)
#else  /* __STDC__ */
boolean             IsBreakable (pAb)
PtrAbstractBox      pAb;
#endif /* __STDC__ */
{
   boolean             result;
   int                 index;
   PtrPSchema          pSchP;
   PtrSSchema          pSchS;

   result = FALSE;
   /* boucle sur les paves englobants */
   while (pAb != NULL && !result)
     {
#ifndef __COLPAGE__
	if (pAb->AbLeafType == LtCompound)
	   /* pave' compose' */
	   if (pAb->AbElement->ElTypeNumber == PageBreak + 1)
	      /* c'est une marque de saut de page, non-secable */
	      result = TRUE;
	   else
	      /* un pave compose' est non-secable s'il est mis en lignes */
	      result = pAb->AbInLine;
	/* regarde dans le schema de presentation du pave s'il est secable */
	if (!result)
#endif /* __COLPAGE__ */
	  {
	     SearchPresSchema (pAb->AbElement, &pSchP, &index, &pSchS);
	     result = (pSchP->PsBuildAll[index - 1]);
	  }
	pAb = pAb->AbEnclosing;
	/* passe a l'englobant */
     }
   return (!result);
}

#ifdef __COLPAGE__

/*----------------------------------------------------------------------
    KillPresRight tue tous les paves de presentation a droite de pAb     
    on ne considere que les paves a l'interieur d'une page 
    sauf les REPEATED                                      
  ----------------------------------------------------------------------*/



#ifdef __STDC__
void                KillPresRight (PtrAbstractBox pAb, PtrDocument pDoc)

#else  /* __STDC__ */
void                KillPresRight (pAb, pDoc)
PtrAbstractBox      pAb;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrAbstractBox      pAbbEnclosing, pAbb, pAbbR;
   boolean             stop;

   if (pAb->AbElement->ElTypeNumber != PageBreak + 1)
     {
	pAbbEnclosing = pAb->AbEnclosing;
	while (pAbbEnclosing != NULL && !(pAbbEnclosing->AbTruncatedTail))
	  {
	     /* on saute le pave corps de page, pour ne pas detruire ses paves */
	     /* de presentation */
	     if (!(pAbbEnclosing->AbElement->ElTerminal
		 && pAbbEnclosing->AbElement->ElLeafType == LtPageColBreak))
	       {
		  pAbbEnclosing->AbTruncatedTail = TRUE;
		  /* cherche et supprime les paves crees par CreateLast */
		  /* si pAbbEnclosing est la racine, ses paves de presentation */
		  /* sont sous le dernier pave corps de page */
		  if (pAbbEnclosing == pDoc->DocViewRootAb[pAb->AbDocView - 1])
		    {
		       pAbb = pAbbEnclosing->AbFirstEnclosed;
		       if (pAbb != NULL && pAbb->AbElement->ElTerminal
			   && pAbb->AbElement->ElLeafType == LtPageColBreak)
			  /* le document est mis en page */
			 {
			    /* on se place sur le dernier pave */
			    while (pAbb->AbNext != NULL)
			       pAbb = pAbb->AbNext;
			    while (pAbb->AbPresentationBox)
			       pAbb = pAbb->AbPrevious;		/* on saut le bas de page */
			    /* et le filet */
			    pAbb = pAbb->AbFirstEnclosed;	/* 1er element du corps de page */
			    /* TODO descendre les paves de colonne */
			 }
		    }
		  else
		     pAbb = pAbbEnclosing->AbFirstEnclosed;
		  /* cherche d'abord le dernier pave englobe */
		  stop = FALSE;
		  if (pAbb != NULL)
		     do
			if (pAbb->AbNext == NULL)
			   stop = TRUE;
			else
			   pAbb = pAbb->AbNext;
		     while (!stop);
		  stop = FALSE;
		  do
		     if (pAbb == NULL)
			stop = TRUE;
		     else if (!pAbb->AbPresentationBox || pAbb->AbDead
			      || pAbb->AbElement != pAbbEnclosing->AbElement)
			stop = TRUE;
		     else
		       {
			  if (!pAbb->AbRepeatedPresBox)
			    {
			       SetDeadAbsBox (pAbb);
			       /* traite les paves qui se referent au pave detruit */
			       ApplyRefAbsBoxSupp (pAbb, &pAbbR, pDoc);
			    }
			  pAbb = pAbb->AbPrevious;
		       }
		  while (!stop);
		  /* cherche et supprime les paves crees par CreateAfter */
		  pAbb = pAbbEnclosing->AbNext;
		  stop = FALSE;
		  do
		     if (pAbb == NULL)
			stop = TRUE;
		     else if (!pAbb->AbPresentationBox
			      || pAbb->AbDead
			      || pAbb->AbElement != pAbbEnclosing->AbElement)
			stop = TRUE;
		     else
		       {
			  if (!pAbb->AbRepeatedPresBox)
			    {
			       SetDeadAbsBox (pAbb);
			       /* traite les paves qui se referent au pave detruit */
			       ApplyRefAbsBoxSupp (pAbb, &pAbbR, pDoc);
			    }
			  pAbb = pAbb->AbNext;
		       }
		  while (!stop);
	       }
	     /* passe au niveau superieur */
	     pAbbEnclosing = pAbbEnclosing->AbEnclosing;
	  }
     }
}				/* fin KillPresRight */
#endif /* __COLPAGE__ */

#ifdef __COLPAGE__

 /*----------------------------------------------------------------------
    DestrAbbNext detruit les paves a partir         
    de pAb                               
   ----------------------------------------------------------------------*/


#ifdef __STDC__
void                DestrAbbNext (PtrAbstractBox pAb, PtrDocument pDoc)

#else  /* __STDC__ */
void                DestrAbbNext (pAb, pDoc)
PtrAbstractBox      pAb;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrAbstractBox      PavRac, pAbb2, pAbb1, pAbbR;

   PavRac = NULL;
   pAbb1 = pAb;
   if (pAbb1 != NULL
       && pAbb1->AbEnclosing != NULL
       && !(pAbb1->AbElement->ElTypeNumber == PageBreak + 1
	    && (pAbb1->AbElement->ElPageType == PgBegin
		|| pAbb1->AbElement->ElPageType == PgComputed
		|| pAbb1->AbElement->ElPageType == PgUser)))
      /* on ne tue pas les paves page, mais on tue les paves */
      /* de colonne */
     {
	/* pAbb1 est le premier pave a detruire de la page courante */
	/* on tue tous les paves de presentation a droite */
	/* en mettant a jour le booleen AbTruncatedTail */
	KillPresRight (pAbb1, pDoc);
	/* on tue pAbb1 */
	SetDeadAbsBox (pAbb1);
	ApplyRefAbsBoxSupp (pAbb1, &pAbbR, pDoc);
	/* on tue tous les paves a droite en remontant l'arbre */
	while (pAbb1->AbEnclosing != NULL
	       && !(pAbb1->AbElement->ElTypeNumber == PageBreak + 1
		    && (pAbb1->AbElement->ElPageType == PgBegin
			|| pAbb1->AbElement->ElPageType == PgComputed
			|| pAbb1->AbElement->ElPageType == PgUser)))
	   /* on ne tue pas les paves page, mais on tue les paves */
	   /* de colonne */
	  {
	     pAbb2 = pAbb1->AbNext;
	     while (pAbb2 != NULL)
	       {
		  SetDeadAbsBox (pAbb2);
		  ApplyRefAbsBoxSupp (pAbb2, &pAbbR, pDoc);
		  pAbb2 = pAbb2->AbNext;
	       }
	     pAbb1 = pAbb1->AbEnclosing;
	  }
	/* pAbb1 est un pave page (en general le corps de page, sauf si */
	/* pEl est lui-meme un element marque de page) */
	if (pAbb1 != NULL)	/* toujours vrai ? */
	   /* on saute les paves de */
	   /* presentation de la page courante: bas et filet */
	  {
	     pAbb2 = pAbb1;
	     while (pAbb2->AbNext != NULL
		    && pAbb2->AbNext->AbElement == pAbb1->AbElement)
		pAbb2 = pAbb2->AbNext;	/* on saute les paves de cette page */
	     pAbb2 = pAbb2->AbNext;	/* premier pave de la page suivante */
	     /* on memorise le pave racine */
	     PavRac = pAbb1->AbEnclosing;
	  }
	pAbb1 = pAbb2;		/* pAbb1 1er pave de la page suivante ou null si */
	/* pas de page suivante */

     }
   if (pAbb1 != NULL)		/* il existe une page suivante */
      /* on la detruit (et les suivantes) */
     {
	PavRac = pAbb1->AbEnclosing;
	while (pAbb1 != NULL)
	  {
	     SetDeadAbsBox (pAbb1);
	     ApplyRefAbsBoxSupp (pAbb1, &pAbbR, pDoc);
	     pAbb1 = pAbb1->AbNext;
	  }
     }
   if (PavRac != NULL)
      /* on marque la racine coupee en queue */
      PavRac->AbTruncatedTail = TRUE;
}				/* fin DestrAbbNext */
#endif /* __COLPAGE__ */


#ifdef __COLPAGE__
/*----------------------------------------------------------------------
   Procedure differente dans V4 : booleen exceptRep a la place de    
   exceptCrWith                                                    
   KillPresSibling detruit les paves de presentation crees par les   
   regles CreateBefore et CreateAfter de pAb.                     
  ----------------------------------------------------------------------*/


#ifdef __STDC__
void                KillPresSibling (PtrAbstractBox pAbbSibling, boolean ElemIsBefore, PtrDocument pDoc, PtrAbstractBox * pAbbR, PtrAbstractBox * pAbbReDisp, int *volsupp, PtrAbstractBox pAb, boolean exceptRep)

#else  /* __STDC__ */
void                KillPresSibling (pAbbSibling, ElemIsBefore, pDoc, pAbbR, pAbbReDisp, volsupp, pAb, exceptRep)
PtrAbstractBox      pAbbSibling;
boolean             ElemIsBefore;
PtrDocument         pDoc;
PtrAbstractBox     *pAbbR;
PtrAbstractBox     *pAbbReDisp;
int                *volsupp;
PtrAbstractBox      pAb;
boolean             exceptRep;

#endif /* __STDC__ */

{
   boolean             stop;

   stop = FALSE;
   do
      if (pAbbSibling == NULL)
	 stop = TRUE;
      else if (pAbbSibling->AbElement != pAb->AbElement)
	 stop = TRUE;
      else
	{
	   if (pAbbSibling->AbPresentationBox)
	      if (!(exceptRep && pAbbSibling->AbRepeatedPresBox))
		{
		   *volsupp += pAbbSibling->AbVolume;
		   SetDeadAbsBox (pAbbSibling);
		   *pAbbReDisp = Enclosing (*pAbbReDisp, pAbbSibling);
		   /* traite les paves qui se referent au pave detruit */
		   ApplyRefAbsBoxSupp (pAbbSibling, pAbbR, pDoc);
		   *pAbbReDisp = Enclosing (*pAbbReDisp, *pAbbR);
		}
	   if (ElemIsBefore)
	      pAbbSibling = pAbbSibling->AbPrevious;
	   else
	      pAbbSibling = pAbbSibling->AbNext;
	}
   while (!stop);
}

#else  /* __COLPAGE__ */

/*----------------------------------------------------------------------
   KillPresSibling detruit les paves de presentation crees par les   
   regles CreateBefore et CreateAfter de pAb.                     
  ----------------------------------------------------------------------*/


#ifdef __STDC__
static void         KillPresSibling (PtrAbstractBox pAbbSibling, boolean ElemIsBefore, PtrDocument pDoc, PtrAbstractBox * pAbbR, PtrAbstractBox * pAbbReDisp, int *volsupp, PtrAbstractBox pAb, boolean exceptCrWith)

#else  /* __STDC__ */
static void         KillPresSibling (pAbbSibling, ElemIsBefore, pDoc, pAbbR, pAbbReDisp, volsupp, pAb, exceptCrWith)
PtrAbstractBox      pAbbSibling;
boolean             ElemIsBefore;
PtrDocument         pDoc;
PtrAbstractBox     *pAbbR;
PtrAbstractBox     *pAbbReDisp;
int                *volsupp;
PtrAbstractBox      pAb;
boolean             exceptCrWith;

#endif /* __STDC__ */

{
   boolean             stop;

   stop = FALSE;
   do
      if (pAbbSibling == NULL)
	 stop = TRUE;
      else if (pAbbSibling->AbElement != pAb->AbElement)
	 stop = TRUE;
      else
	{
	   if (pAbbSibling->AbPresentationBox)
	      if (!exceptCrWith || TypeCreatedRule (pDoc, pAb, pAbbSibling) != FnCreateWith)
		{
		   *volsupp += pAbbSibling->AbVolume;
		   SetDeadAbsBox (pAbbSibling);
		   *pAbbReDisp = Enclosing (*pAbbReDisp, pAbbSibling);
		   /* traite les paves qui se referent au pave detruit */
		   ApplyRefAbsBoxSupp (pAbbSibling, pAbbR, pDoc);
		   *pAbbReDisp = Enclosing (*pAbbReDisp, *pAbbR);
		}
	   if (ElemIsBefore)
	      pAbbSibling = pAbbSibling->AbPrevious;
	   else
	      pAbbSibling = pAbbSibling->AbNext;
	}
   while (!stop);
}
#endif /* __COLPAGE__ */

#ifdef __COLPAGE__
/*----------------------------------------------------------------------
   Procedure differente dans V4 : booleen exceptRep a la place de    
   exceptCrWith                                                    
   KillPresEnclosing supprime tous les paves de presentation           
   crees par pAb et les paves de presentation crees par   
   les paves englobants a l'aide de regles CreateFirst et  
   CreateBefore (si head est vrai) ou CreateAfter et       
   CreateLast (si head est faux).                          
   Au retour volsupp indique le volume des paves de        
   presentation tues et pAbbReDisp le pave a reafficher.     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         KillPresEnclosing (PtrAbstractBox pAb, boolean head, PtrDocument pDoc, PtrAbstractBox * pAbbReDisp, int *volsupp, boolean exceptRep)

#else  /* __STDC__ */
static void         KillPresEnclosing (pAb, head, pDoc, pAbbReDisp, volsupp, exceptRep)
PtrAbstractBox      pAb;
boolean             head;
PtrDocument         pDoc;
PtrAbstractBox     *pAbbReDisp;
int                *volsupp;
boolean             exceptRep;

#endif /* __STDC__ */

{
   PtrAbstractBox      pAbbEnclosing, pAbb, pAbbR;
   boolean             stop;

   *volsupp = 0;
   /* Detruit les paves de presentation crees par les regles */
   /* CreateBefore et CreateAfter de pAb. */
   KillPresSibling (pAb->AbPrevious, TRUE,
		    pDoc, &pAbbR, pAbbReDisp, volsupp, pAb, exceptRep);
   KillPresSibling (pAb->AbNext, FALSE,
		    pDoc, &pAbbR, pAbbReDisp, volsupp, pAb, exceptRep);
   /* traite les paves englobants */
   pAbbEnclosing = pAb->AbEnclosing;
   while (pAbbEnclosing != NULL)
      /* le test sur AbTruncatedHead ou AbTruncatedTail n'est plus significatif */
      /* a cause des paves de presentation repetes */
      /*    if (head && pAbbEnclosing->AbTruncatedHead  */
      /*     || !head && pAbbEnclosing->AbTruncatedTail) */
      /* pave deja traite', on s'arrete */
      /*      pAbbEnclosing = NULL; */
      /*    else */
     {
	/* on saute le pave corps de page, pour ne pas detruire ses paves de pres */
	if (!(pAbbEnclosing->AbElement->ElTerminal
	      && pAbbEnclosing->AbElement->ElLeafType == LtPageColBreak))
	   if (IsBreakable (pAbbEnclosing))
	      if (head)
		{
		   pAbbEnclosing->AbTruncatedHead = TRUE;
		   /* si pAbbEnclosing est la racine, ses paves de presentation */
		   /* sont sous le pave corps de page ou de colonne */
		   if (pAbbEnclosing->AbEnclosing == NULL)
		     {
			pAbb = pAbbEnclosing->AbFirstEnclosed;
			if (pAbb != NULL && pAbb->AbElement->ElTerminal
			    && pAbb->AbElement->ElLeafType == LtPageColBreak)
			   /* le document est mis en page */
			   while (pAbb->AbElement->ElTypeNumber == PageBreak + 1)
			     {
				/* on saute les paves de pres page et colonnes */
				while (pAbb->AbPresentationBox)
				   pAbb = pAbb->AbNext;
				pAbb = pAbb->AbFirstEnclosed;
				/* pAbb : premier pave du corps de page ou colonne */
				/* on suppose qu'il n'y a pas de creer first pour */
				/* les pages et colonnes TODO a controler dans prs */
			     }
		     }
		   else
		      pAbb = pAbbEnclosing->AbFirstEnclosed;
		   /* cherche et supprime les paves crees par CreateFirst */
		   /* mais ne supprime pas ceux qui sont repetes */
		   stop = FALSE;
		   do
		      if (pAbb == NULL)
			 stop = TRUE;
		      else if (!pAbb->AbPresentationBox || pAbb->AbDead
			     || pAbb->AbElement != pAbbEnclosing->AbElement)
			 /* suppression du saut des marques de page de debut d'element */
			 stop = TRUE;
		      else
			{
			   if (!pAbb->AbRepeatedPresBox)
			     {
				*volsupp += pAbb->AbVolume;
				SetDeadAbsBox (pAbb);
				*pAbbReDisp = Enclosing (*pAbbReDisp, pAbb);
				/* traite les paves qui se referent au pave detruit */
				ApplyRefAbsBoxSupp (pAbb, &pAbbR, pDoc);
				*pAbbReDisp = Enclosing (*pAbbReDisp, pAbbR);
			     }
			   pAbb = pAbb->AbNext;
			}
		   while (!stop);
		   /* cherche et supprime les paves crees par CreateBefore */
		   /* mais ne supprime pas ceux qui sont repetes */
		   pAbb = pAbbEnclosing->AbPrevious;
		   stop = FALSE;
		   do
		      if (pAbb == NULL)
			 stop = TRUE;
		      else if (!pAbb->AbPresentationBox || pAbb->AbDead
			     || pAbb->AbElement != pAbbEnclosing->AbElement)
			 stop = TRUE;
		      else
			{
			   if (!pAbb->AbRepeatedPresBox)
			     {
				*volsupp += pAbb->AbVolume;
				SetDeadAbsBox (pAbb);
				*pAbbReDisp = Enclosing (*pAbbReDisp, pAbb);
				/* traite les paves qui se referent au pave detruit */
				ApplyRefAbsBoxSupp (pAbb, &pAbbR, pDoc);
				*pAbbReDisp = Enclosing (*pAbbReDisp, pAbbR);
			     }
			   pAbb = pAbb->AbPrevious;
			}
		   while (!stop);
		}
	      else
		{
		   /* head = FALSE */
		   pAbbEnclosing->AbTruncatedTail = TRUE;
		   /* cherche et supprime les paves crees par CreateLast */
		   /* mais ne supprime pas ceux qui sont repetes */
		   /* si pAbbEnclosing est la racine, ses paves de presentation */
		   /* sont sous le dernier pave corps de page */
		   if (pAbbEnclosing->AbEnclosing == NULL)
		     {
			pAbb = pAbbEnclosing->AbFirstEnclosed;
			if (pAbb != NULL && pAbb->AbElement->ElTerminal
			    && pAbb->AbElement->ElLeafType == LtPageColBreak)
			   /* le document est mis en page */
			   /* on se place sur le dernier pave non page ou colonne */
			   while (pAbb != NULL && pAbb->AbElement->ElTypeNumber == PageBreak + 1)
			     {
				while (pAbb->AbNext != NULL)
				   pAbb = pAbb->AbNext;
				while (pAbb->AbPresentationBox)
				   pAbb = pAbb->AbPrevious;
				pAbb = pAbb->AbFirstEnclosed;
			     }
		     }
		   else
		      pAbb = pAbbEnclosing->AbFirstEnclosed;
		   /* cherche d'abord le dernier pave englobe */
		   stop = FALSE;
		   if (pAbb != NULL)
		      do
			 if (pAbb->AbNext == NULL)
			    stop = TRUE;
			 else
			    pAbb = pAbb->AbNext;
		      while (!stop);
		   stop = FALSE;
		   do
		      if (pAbb == NULL)
			 stop = TRUE;
		      else if (!pAbb->AbPresentationBox || pAbb->AbDead
			     || pAbb->AbElement != pAbbEnclosing->AbElement)
			 stop = TRUE;
		      else
			{
			   if (!pAbb->AbRepeatedPresBox)
			     {
				*volsupp += pAbb->AbVolume;
				SetDeadAbsBox (pAbb);
				*pAbbReDisp = Enclosing (*pAbbReDisp, pAbb);
				/* traite les paves qui se referent au pave detruit */
				ApplyRefAbsBoxSupp (pAbb, &pAbbR, pDoc);
				*pAbbReDisp = Enclosing (*pAbbReDisp, pAbbR);
			     }
			   pAbb = pAbb->AbPrevious;
			}
		   while (!stop);
		   /* cherche et supprime les paves crees par CreateAfter */
		   /* mais ne supprime pas ceux crees par CreateWith */
		   pAbb = pAbbEnclosing->AbNext;
		   stop = FALSE;
		   do
		      if (pAbb == NULL)
			 stop = TRUE;
		      else if (!pAbb->AbPresentationBox
			       || pAbb->AbDead
			     || pAbb->AbElement != pAbbEnclosing->AbElement)
			 stop = TRUE;
		      else
			{
			   if (!pAbb->AbRepeatedPresBox)
			     {
				*volsupp += pAbb->AbVolume;
				SetDeadAbsBox (pAbb);
				*pAbbReDisp = Enclosing (*pAbbReDisp, pAbb);
				/* traite les paves qui se referent au pave detruit */
				ApplyRefAbsBoxSupp (pAbb, &pAbbR, pDoc);
				*pAbbReDisp = Enclosing (*pAbbReDisp, pAbbR);
			     }
			   pAbb = pAbb->AbNext;
			}
		   while (!stop);
		}
	/* passe au niveau superieur */
	pAbbEnclosing = pAbbEnclosing->AbEnclosing;
     }
}

#else  /* __COLPAGE__ */
/*----------------------------------------------------------------------
   KillPresEnclosing supprime tous les paves de presentation           
   crees par pAb et les paves de presentation crees par   
   les paves englobants a l'aide de regles Create et       
   CreateBefore (si head est vrai) ou CreateAfter et       
   CreateLast (si head est faux).                          
   Au retour volsupp indique le volume des paves de        
   presentation tues et pAbbReDisp le pave a reafficher.     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         KillPresEnclosing (PtrAbstractBox pAb, boolean head, PtrDocument pDoc, PtrAbstractBox * pAbbReDisp, int *volsupp, boolean exceptCrWith)
#else  /* __STDC__ */
static void         KillPresEnclosing (pAb, head, pDoc, pAbbReDisp, volsupp, exceptCrWith)
PtrAbstractBox      pAb;
boolean             head;
PtrDocument         pDoc;
PtrAbstractBox     *pAbbReDisp;
int                *volsupp;
boolean             exceptCrWith;
#endif /* __STDC__ */

{
   PtrAbstractBox      pAbbEnclosing, pAbb, pAbbR;
   boolean             stop;
   PtrElement          pEl1;

   *volsupp = 0;
   /* Detruit les paves de presentation crees par les regles */
   /* CreateBefore et CreateAfter de pAb. */
   KillPresSibling (pAb->AbPrevious, TRUE,
		    pDoc, &pAbbR, pAbbReDisp, volsupp, pAb, exceptCrWith);
   KillPresSibling (pAb->AbNext, FALSE,
		    pDoc, &pAbbR, pAbbReDisp, volsupp, pAb, exceptCrWith);
   /* traite les paves englobants */
   pAbbEnclosing = pAb->AbEnclosing;
   while (pAbbEnclosing != NULL)
      if ((head && pAbbEnclosing->AbTruncatedHead)
	  || (!head && pAbbEnclosing->AbTruncatedTail))
	 /* pave deja traite', on s'arrete */
	 pAbbEnclosing = NULL;
      else
	{
	   if (IsBreakable (pAbbEnclosing))
	      if (head)
		{
		   pAbbEnclosing->AbTruncatedHead = TRUE;
		   /* cherche et supprimes les paves crees par CreateFirst */
		   pAbb = pAbbEnclosing->AbFirstEnclosed;
		   stop = FALSE;
		   do
		      if (pAbb == NULL)
			 stop = TRUE;
		      else if (!pAbb->AbPresentationBox || pAbb->AbDead
			     || pAbb->AbElement != pAbbEnclosing->AbElement)
			 /* saute les marques de page de debut d'element */
			{
			   pEl1 = pAbb->AbElement;
			   if (pEl1->ElTypeNumber == PageBreak + 1
			       && pEl1->ElPageType == PgBegin)
			      pAbb = pAbb->AbNext;
			   else
			      stop = TRUE;
			}
		      else
			{
			   *volsupp += pAbb->AbVolume;
			   SetDeadAbsBox (pAbb);
			   *pAbbReDisp = Enclosing (*pAbbReDisp, pAbb);
			   /* traite les paves qui se referent au pave detruit */
			   ApplyRefAbsBoxSupp (pAbb, &pAbbR, pDoc);
			   *pAbbReDisp = Enclosing (*pAbbReDisp, pAbbR);
			   pAbb = pAbb->AbNext;
			}
		   while (!stop);
		   /* cherche et supprime les paves crees par CreateBefore */
		   /* mais ne supprime pas ceux crees par CreateWith */
		   pAbb = pAbbEnclosing->AbPrevious;
		   stop = FALSE;
		   do
		      if (pAbb == NULL)
			 stop = TRUE;
		      else if (!pAbb->AbPresentationBox || pAbb->AbDead
			     || pAbb->AbElement != pAbbEnclosing->AbElement)
			 stop = TRUE;
		      else
			{
			   if (TypeCreatedRule (pDoc, pAbbEnclosing, pAbb) != FnCreateWith)
			     {
				*volsupp += pAbb->AbVolume;
				SetDeadAbsBox (pAbb);
				*pAbbReDisp = Enclosing (*pAbbReDisp, pAbb);
				/* traite les paves qui se referent au pave detruit */
				ApplyRefAbsBoxSupp (pAbb, &pAbbR, pDoc);
				*pAbbReDisp = Enclosing (*pAbbReDisp, pAbbR);
			     }
			   pAbb = pAbb->AbPrevious;
			}
		   while (!stop);
		}
	      else
		{
		   /* head = FALSE */
		   pAbbEnclosing->AbTruncatedTail = TRUE;
		   /* cherche et supprime les paves crees par CreateLast */
		   pAbb = pAbbEnclosing->AbFirstEnclosed;
		   /* cherche d'abord le dernier pave englobe */
		   stop = FALSE;
		   if (pAbb != NULL)
		      do
			 if (pAbb->AbNext == NULL)
			    stop = TRUE;
			 else
			    pAbb = pAbb->AbNext;
		      while (!stop);
		   stop = FALSE;
		   do
		      if (pAbb == NULL)
			 stop = TRUE;
		      else if (!pAbb->AbPresentationBox || pAbb->AbDead
			     || pAbb->AbElement != pAbbEnclosing->AbElement)
			 stop = TRUE;
		      else
			{
			   *volsupp += pAbb->AbVolume;
			   SetDeadAbsBox (pAbb);
			   *pAbbReDisp = Enclosing (*pAbbReDisp, pAbb);
			   /* traite les paves qui se referent au pave detruit */
			   ApplyRefAbsBoxSupp (pAbb, &pAbbR, pDoc);
			   *pAbbReDisp = Enclosing (*pAbbReDisp, pAbbR);
			   pAbb = pAbb->AbPrevious;
			}
		   while (!stop);
		   /* cherche et supprime les paves crees par CreateAfter */
		   /* mais ne supprime pas ceux crees par CreateWith */
		   pAbb = pAbbEnclosing->AbNext;
		   stop = FALSE;
		   do
		      if (pAbb == NULL)
			 stop = TRUE;
		      else if (!pAbb->AbPresentationBox || pAbb->AbDead
			     || pAbb->AbElement != pAbbEnclosing->AbElement)
			 stop = TRUE;
		      else
			{
			   if (TypeCreatedRule (pDoc, pAbbEnclosing, pAbb) != FnCreateWith)
			     {
				*volsupp += pAbb->AbVolume;
				SetDeadAbsBox (pAbb);
				*pAbbReDisp = Enclosing (*pAbbReDisp, pAbb);
				/* traite les paves qui se referent au pave detruit */
				ApplyRefAbsBoxSupp (pAbb, &pAbbR, pDoc);
				*pAbbReDisp = Enclosing (*pAbbReDisp, pAbbR);
			     }
			   pAbb = pAbb->AbNext;
			}
		   while (!stop);
		}
	   /* passe au niveau superieur */
	   pAbbEnclosing = pAbbEnclosing->AbEnclosing;
	}
}
#endif /* __COLPAGE__ */


/*----------------------------------------------------------------------
   SupprAbsBoxes reduit de dvol le volume du pave pAbbRoot en        
   supprimant des paves, en tete si head est vrai, en      
   queue sinon.                                            
   Au retour dvol contient le volume qui reste a supprimer.
   suppression page par page si vue avec pages             
   cette procedure est toujours appelee avec la racine de  
   la vue si le document est mis en pages.                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         SupprAbsBoxes (PtrAbstractBox pAbbRoot, PtrDocument pDoc, boolean head, int *dvol)
#else  /* __STDC__ */
static void         SupprAbsBoxes (pAbbRoot, pDoc, head, dvol)
PtrAbstractBox      pAbbRoot;
PtrDocument         pDoc;
boolean             head;
int                *dvol;
#endif /* __STDC__ */
{
   PtrAbstractBox      pAb, pAbbSibling, pAbbReDisp, pAbbR;
   int                 volsupp, volpres;
   boolean             stop, stop1;
   PtrElement          pEl1;

#ifdef __COLPAGE__
   PtrAbstractBox      pAbb;
   int                 view, nbpages;
   boolean             caspage;

#endif /* __COLPAGE__ */

   /* cherche le premier pave englobe' de volume inferieur a dvol et qui */
   /* soit secable */
   pAb = pAbbRoot;
#ifdef __COLPAGE__
   pAbbReDisp = NULL;
   /* suppression d'une page si la vue est paginee */
   caspage = FALSE;
   view = pAb->AbDocView;
   pAbb = pAbbRoot->AbFirstEnclosed;
   if (pAbb != NULL && pAbb->AbElement->ElLeafType == LtPageColBreak)
      caspage = TRUE;
   else
      caspage = FALSE;

   if (!caspage)
     {
	/* inutile si destruction par pages entieres */
#endif /* __COLPAGE__ */
	volpres = 0;
	/* volume des paves de presentation des elem englobants */
	stop = FALSE;
	do
	   if (pAb == NULL)
	      stop = TRUE;
	   else if (pAb->AbVolume + volpres <= *dvol || !(IsBreakable (pAb)))
	      stop = TRUE;
	   else
	     {
		/* premier pave du niveau inferieur */
		pAb = pAb->AbFirstEnclosed;
		if (pAb != NULL)
		   /* si on supprime en tete, cherche le premier pave qui ne soit */
		   /* pas un pave de presentation */
		   /* si on supprime en queue, cherche le dernier pave qui ne soit */
		   /* pas un pave de presentation */
		  {
		     if (!head)
			while (pAb->AbNext != NULL)
			   pAb = pAb->AbNext;
		     stop1 = FALSE;
		     do
			if (pAb == NULL)
			   stop1 = TRUE;
			else if (!pAb->AbPresentationBox)
			   stop1 = TRUE;
			else if (pAb->AbFirstEnclosed != NULL &&
			  pAb->AbFirstEnclosed->AbElement == pAb->AbElement)
			   /* un pave de presentation cree' par la regle FnCreateEnclosing */
			   /* on le considere comme un pave' d'element structure' */
			   stop1 = TRUE;
			else
			  {
			     volpres += pAb->AbVolume;
			     if (head)
				pAb = pAb->AbNext;
			     else
				pAb = pAb->AbPrevious;
			  }
		     while (!stop1);
		  }
	     }
	while (!stop);
	if (pAb == NULL)
	   *dvol = 0;
	else if (pAb->AbVolume + volpres > *dvol)
	   /* on ne peut rien supprimer */
	   *dvol = 0;
	if (*dvol > 0)
	  {
#ifdef __COLPAGE__
	     if (caspage)
	       {
		  /* pAbb est le premier fils de la racine */
		  /* on compte le nombre de pages (pour en laisser au moins une */
		  nbpages = NbPages (pAbbRoot);
		  if (!head)
		     while (pAbb->AbNext != NULL)
			pAbb = pAbb->AbNext;
		  /* suppression des paves de page dans la limite de dvol */
		  /* tout en laissant au moins une page */
		  /* TODO faut-il prevoir de verifier qu'on ne laisse pas une page vide */
		  while (*dvol > 0 && pAbb != NULL && nbpages > 1)
		    {
		       if (pAbb->AbElement->ElLeafType == LtPageColBreak)
			  if (pAbb->AbLeafType == LtCompound && !pAbb->AbPresentationBox)
			     if (*dvol - pAbb->AbVolume >= 0)
			       {
				  *dvol -= pAbb->AbVolume;
				  KillPresEnclosing (pAbb, head, pDoc, &pAbbReDisp, &volsupp, FALSE);
				  *dvol -= volsupp;
				  SetDeadAbsBox (pAbb);
				  ApplyRefAbsBoxSupp (pAbb, &pAbbR, pDoc);
				  nbpages--;
			       }
			     else
				pAbb = NULL;
		       if (pAbb != NULL)
			  if (head)
			     pAbb = pAbb->AbNext;
			  else
			     pAbb = pAbb->AbPrevious;
		    }
		  if (AssocView (pAbbRoot->AbElement))
		     pDoc->DocAssocModifiedAb[pAbbRoot->AbElement->ElAssocNum - 1] = pAbbRoot;

		  else
		     pDoc->DocViewModifiedAb[view - 1] = pAbbRoot;
		  *dvol = 0;
	       }
	     else
	       {
		  /* pas de pages : pas de changement de code */

#endif /* __COLPAGE__ */
		  /* on peut supprimer le pave pAb */
		  /* calcule le nouveau volume qui restera a supprimer apres la */
		  /* suppression de pAb */
		  *dvol -= pAb->AbVolume;
		  pAbbReDisp = pAb;
		  /* il faudra reafficher au moins pAb */
		  /* tous les paves englobant pAb sont coupe's. On supprime leurs */
		  /* paves de presentation. On supprime aussi ceux crees par le pave */
		  /* tue'. */
		  KillPresEnclosing (pAb, head, pDoc, &pAbbReDisp, &volsupp, FALSE);
		  *dvol -= volsupp;
		  /* detruit le pave trouve' et toute sa descendance */
		  SetDeadAbsBox (pAb);
		  /* detruit les paves freres tant qu'il reste du volume a supprimer */
		  pAbbSibling = pAb;
		  while (*dvol > 0 && pAbbSibling != NULL)
		    {
		       if (head)
			  pAbbSibling = pAbbSibling->AbNext;
		       else
			  pAbbSibling = pAbbSibling->AbPrevious;
		       if (pAbbSibling != NULL)
			  /* on ne supprime pas les pave's de presentation, sauf ceux qui */
			  /* ont ete cree's par une regle FnCreateEnclosing */
			  if (!pAbbSibling->AbPresentationBox ||
			      (pAbbSibling->AbPresentationBox && pAbbSibling->AbFirstEnclosed != NULL &&
			       pAbbSibling->AbFirstEnclosed->AbElement == pAbbSibling->AbElement))
			     SupprAbsBoxes (pAbbSibling, pDoc, head, dvol);
		       /* traite ensuite les paves qui se referent au pave detruit */
		    }
		  ApplyRefAbsBoxSupp (pAb, &pAbbR, pDoc);
		  pAbbReDisp = Enclosing (pAbbReDisp, pAbbR);
		  pEl1 = pAbbRoot->AbElement;
		  if (AssocView (pAbbRoot->AbElement))
		     pDoc->DocAssocModifiedAb[pEl1->ElAssocNum - 1] =
			Enclosing (pAbbReDisp, pDoc->DocAssocModifiedAb[pEl1->ElAssocNum - 1]);
		  else
		     pDoc->DocViewModifiedAb[pAbbRoot->AbDocView - 1] =
			Enclosing (pAbbReDisp, pDoc->DocViewModifiedAb[pAbbRoot->AbDocView - 1]);
		  *dvol = 0;
#ifdef __COLPAGE__
	       }		/* fin cas sans pages */
#endif /* __COLPAGE__ */
	  }			/* fin dvol > 0 */
#ifdef __COLPAGE__
     }				/* fin !caspage */
#endif /* __COLPAGE__ */
}



/*----------------------------------------------------------------------
   AddVolView ajuste le volume de la vue dont le pave pAbbRoot      
   est le pave racine.                                     
   VolOpt est le volume optimum de cette vue.              
   ElMilieu est l'element qui devrait etre au milieu       
   de la vue.                                              
     pAbbRoot est une vraie racine d'image abstraite        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         AddVolView (int VolOpt, PtrAbstractBox pAbbRoot, PtrElement pElMiddle, PtrDocument pDoc)
#else  /* __STDC__ */
static void         AddVolView (VolOpt, pAbbRoot, pElMiddle, pDoc)
int                 VolOpt;
PtrAbstractBox      pAbbRoot;
PtrElement          pElMiddle;
PtrDocument         pDoc;
#endif /* __STDC__ */
{
   boolean             add, suppress, midHead, stop;
   int                 view, volprec, dvol;
   PtrAbstractBox      pAbbPrevious, pAbbParent;

   /* evalue d'abord s'il faut ajouter ou supprimer des paves */
   add = FALSE;
   suppress = FALSE;
   if (IsBreakable (pAbbRoot))
      if (pAbbRoot->AbVolume < VolOpt - VolOpt / 8)
	 /* il faudrait creer de nouveaux paves dans cette vue */
	 add = TRUE;
      else if (pAbbRoot->AbVolume > VolOpt + VolOpt / 8)
	 /* il faudrait supprimer des paves dans cette vue */
	 suppress = TRUE;
   if (add || suppress)
     {
	/* calcule la position actuelle de pElMiddle dans la vue, pour savoir */
	/* s'il faut modifier le debut ou la fin de la vue */
	view = pAbbRoot->AbDocView;
	if (pElMiddle->ElAbstractBox[view - 1] == NULL)
	   /* pElMiddle n'a pas de pave dans cette vue */
	   /* cherche le 1er element englobant pElMiddle qui ait un pave dans */
	   /* la vue */
	  {
	     stop = FALSE;
	     do
	       {
		  pElMiddle = pElMiddle->ElParent;
		  if (pElMiddle == NULL)
		    {
		       stop = TRUE;
		       pElMiddle = pAbbRoot->AbElement;
		    }
		  else if (pElMiddle->ElAbstractBox[view - 1] != NULL)
		     stop = TRUE;
	       }
	     while (!stop);
	  }
	/* pElMiddle a un pave dans cette vue */
	/* calcule le volume des paves qui le precedent */
	volprec = 0;
	pAbbParent = pElMiddle->ElAbstractBox[view - 1];
	do
	  {
	     pAbbPrevious = pAbbParent->AbPrevious;
	     while (pAbbPrevious != NULL)
	       {
		  volprec += pAbbPrevious->AbVolume;
		  pAbbPrevious = pAbbPrevious->AbPrevious;
	       }
	     pAbbParent = pAbbParent->AbEnclosing;
	  }
	while (pAbbParent != NULL);
	midHead = volprec < VolOpt / 2;
	if (add)
	   /* il faut ajouter des paves */
	   if (midHead)
	      /* l'element qui devrait etre au milieu est dans la 1ere moitie' */
	      if (pAbbRoot->AbTruncatedHead)
		 /* le debut de l'image n'est pas complete, on ajoute en tete */
		 AddAbsBoxes (pAbbRoot, pDoc, TRUE);
	      else
		{
		   /* le debut de l'image est complete */
		   /* on ajoute en queue, si la queue n'est pas complete */
		   if (pAbbRoot->AbTruncatedTail)
		      AddAbsBoxes (pAbbRoot, pDoc, FALSE);
		}
	   else
	      /* l'element qui devrait etre au milieu est dans la 2eme moitie' */
	   if (pAbbRoot->AbTruncatedTail)
	      /* la fin de l'image n'est pas complete, on ajoute en queue */
	      AddAbsBoxes (pAbbRoot, pDoc, FALSE);
	   else
	     {
		/* la fin de l'image est complete, on ajoute en tete si le */
		/* idebut n'est pas complete */
		if (pAbbRoot->AbTruncatedHead)
		   AddAbsBoxes (pAbbRoot, pDoc, TRUE);
	     }
	else if (suppress)
	   /* supprime des paves en queue si l'element qui devrait etre au */
	   /* milieu est dans la premiere moitie de l'image, et inversement */
	  {
	     dvol = pAbbRoot->AbVolume - VolOpt;
	     SupprAbsBoxes (pAbbRoot, pDoc, !midHead, &dvol);
	  }
     }
}



/*----------------------------------------------------------------------
   AdjustVolume pour toutes les vues du document pointe' par pDoc  
   ajuste (reduit ou augmente) le volume des images        
   abstraites en conservant l'element pointe par pEl au    
   milieu (ou a peu pres) de l'image abstraite.            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                AdjustVolume (PtrElement pEl, PtrDocument pDoc)
#else  /* __STDC__ */
void                AdjustVolume (pEl, pDoc)
PtrElement          pEl;
PtrDocument         pDoc;
#endif /* __STDC__ */
{
  int                 view;
#ifdef __COLPAGE__
  PtrAbstractBox      pAb;
  int                 nb, vol;
#endif /* __COLPAGE__ */

  if (pEl != NULL && pDoc != NULL)
    {
      if (!AssocView (pEl))
	/* une vue de l'arbre principal */
	for (view = 0; view < MAX_VIEW_DOC; view++)
	  {
	    /* traite toutes les vues */
	    if (pDoc->DocView[view].DvPSchemaView > 0)
	      /* la vue existe */
	      {
#ifdef __COLPAGE__
		/* le document est-il pagine dans cette vue ? */
		/* si oui, on compte le nombre de pages actuel */
		/* pour etre sur d'ajouter au moins une page */
		/* (sauf si fin de vue) */
		pAb = pDoc->DocViewRootAb[view];
		if (pAb->AbFirstEnclosed != NULL
		    && pAb->AbFirstEnclosed->AbElement->ElTypeNumber == PageBreak + 1)
		  {
		    nb = NbPages (pAb);
		    pDoc->DocViewNPages[view] = nb;
		    pDoc->DocViewFreeVolume[view] = THOT_MAXINT;
		  }
		else
#endif /* __COLPAGE__ */
		  pDoc->DocViewFreeVolume[view] = pDoc->DocViewVolume[view] - pDoc->DocViewRootAb[view]->AbVolume;
		AddVolView (pDoc->DocViewVolume[view], pDoc->DocViewRootAb[view],
			    pEl, pDoc);
	      }
	  }
      else if (pDoc->DocAssocFrame[pEl->ElAssocNum - 1] > 0)
	/* element associe */
	/* la vue de ces elements associes a ete creee */
	if (pDoc->DocAssocVolume[pEl->ElAssocNum - 1] > 0)
	  /* on ne fait rien si ces elements associes sont affiches */
	  /* dans des boites de haut ou bas de page */
	  {
#ifdef __COLPAGE__
	    pAb = pDoc->DocAssocRoot[pEl->ElAssocNum - 1]->ElAbstractBox[0];
	    if (pAb->AbFirstEnclosed != NULL
		&& pAb->AbFirstEnclosed->AbElement->ElTypeNumber ==
		PageBreak + 1)
	      /* vue du document paginee */
	      {
		nb = NbPages (pAb);
		pDoc->DocAssocNPages[view] = nb;
		pDoc->DocAssocFreeVolume[pEl->ElAssocNum - 1] = THOT_MAXINT;
	      }
	    else
#endif /* __COLPAGE__ */
	      pDoc->DocAssocFreeVolume[pEl->ElAssocNum - 1] =
		pDoc->DocAssocVolume[pEl->ElAssocNum - 1] -
		pDoc->DocAssocRoot[pEl->ElAssocNum - 1]->ElAbstractBox[0]->AbVolume;
	    AddVolView (pDoc->DocAssocVolume[pEl->ElAssocNum - 1],
			pDoc->DocAssocRoot[pEl->ElAssocNum - 1]->ElAbstractBox[0],
			pEl, pDoc);
	  }
    }
}



/*----------------------------------------------------------------------
  IncreaseVolume Le Mediateur augmente de dVol le volume affichable  
  dans la fenetre ViewFrame.
  Met a jour la capacite de la vue affichee dans cette frame et cree de
  nouveaux paves en tete ou en queue, selon le booleen head, de l'image
  abstraite affichee dans ViewFrame.             
  On cree des paves, le Mediateur se charge du reaffichage
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                IncreaseVolume (boolean head, int dVol, int frame)
#else  /* __STDC__ */
void                IncreaseVolume (head, dVol, frame)
boolean             head;
int                 dVol;
int                 frame;
#endif /* __STDC__ */
{
   PtrDocument         pDoc;
   int                 view, h;
   boolean             assoc;
   PtrAbstractBox      pAb;
#ifdef __COLPAGE__
   PtrElement          pElRoot, pEl;
   int                 nb, viewSch;
   PtrPSchema          pSchPage;
#endif /* __COLPAGE__ */

   if (dVol <= 0)
     return;

   GetDocAndView (frame, &pDoc, &view, &assoc);
   /* met a jour la nouvelle capacite de la vue, indique dans le contexte */
   /* du document le volume des paves a creer et cherche le pave racine de */
   /* la vue */
   if (pDoc == NULL)
      printf ("\nError IncreaseVolume: bad frame\n");
   else
     {
	if (assoc)
	  {
	     /* element associe */
	     pAb = pDoc->DocAssocRoot[view - 1]->ElAbstractBox[0];
#ifdef __COLPAGE__
	     /* attention, view contient le numero d'element associe */
	     pElRoot = pDoc->DocAssocRoot[view - 1];
#endif /* __COLPAGE__ */
	     pDoc->DocAssocVolume[view - 1] = pAb->AbVolume + dVol;
	     pDoc->DocAssocFreeVolume[view - 1] = dVol;
	  }
	else
	  {
	     /* element de l'arbre principal */
#ifdef __COLPAGE__
	     pElRoot = pDoc->DocRootElement;
#endif /* __COLPAGE__ */
	     pAb = pDoc->DocViewRootAb[view - 1];
	     pDoc->DocViewVolume[view - 1] = pAb->AbVolume + dVol;
	     pDoc->DocViewFreeVolume[view - 1] = dVol;
	  }

#ifdef __COLPAGE__
	/*  test si vue paginee */
	pEl = pElRoot->ElFirstChild;
	/* recherche le numero de vue defini dans le schema de presentation */
	/* Attention il faut appeler AppliedView avec pElRoot */
	viewSch = AppliedView (pElRoot, NULL, pDoc, view);
	if (GetPageBoxType (pEl, viewSch, &pSchPage) != 0)

	   /* le document est-il pagine dans cette vue ? */
	   /* si oui, on compte le nombre de pages actuel */
	   /* pour etre sur d'ajouter au moins une page */
	   /* (sauf si fin de view) */
	  {
	     nb = NbPages (pAb);
	     if (assoc)
		/* element associe */
	       {
		  pDoc->DocAssocFreeVolume[view - 1] = THOT_MAXINT;
		  pDoc->DocAssocNPages[view - 1] = nb;
	       }
	     else
	       {
		  /* element de l'arbre principal */
		  pDoc->DocViewFreeVolume[view - 1] = THOT_MAXINT;
		  pDoc->DocViewNPages[view - 1] = nb;
	       }
	  }
#endif /* __COLPAGE__ */
	if (IsBreakable (pAb))
	  {
	     /* cree les paves de la partie qui va apparaitre */
	     AddAbsBoxes (pAb, pDoc, head);

	     /* signale au Mediateur les paves crees et detruits */
#ifdef __COLPAGE__
	     h = BreakPageHeight;
#else  /* __COLPAGE__ */
	     h = PageHeight;
#endif /* __COLPAGE__ */
	     if (assoc)
	       {
		  if (pDoc->DocAssocModifiedAb[view - 1] != NULL)
		    {
		       (void) ChangeConcreteImage (frame, &h, pDoc->DocAssocModifiedAb[view - 1]);
		       FreeDeadAbstractBoxes (pDoc->DocAssocModifiedAb[view - 1]);
		       pDoc->DocAssocModifiedAb[view - 1] = NULL;
		    }
	       }
	     else if (pDoc->DocViewModifiedAb[view - 1] != NULL)
	       {
		  (void) ChangeConcreteImage (frame, &h, pDoc->DocViewModifiedAb[view - 1]);
		  FreeDeadAbstractBoxes (pDoc->DocViewModifiedAb[view - 1]);
		  pDoc->DocViewModifiedAb[view - 1] = NULL;
	       }
	  }
     }
}



/*----------------------------------------------------------------------
  DecreaseVolume Le Mediateur reduit de dVol le volume affichable      
  dans la fenetre frame.
  Met a jour la capacite de la vue affichee dans cette frame et supprime
  des paves en tete ou en queue, selon le booleen head, de l'image abstraite
  affichee dans frame.
  On supprime des paves, le Mediateur se charge du reaffichage
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DecreaseVolume (boolean head, int dVol, int frame)
#else  /* __STDC__ */
void                DecreaseVolume (head, dVol, frame)
boolean             head;
int                 dVol;
int                 frame;
#endif /* __STDC__ */

{
   PtrDocument         pDoc;
   int                 view, h;
   boolean             assoc;
   PtrAbstractBox      pAb;

   if (dVol <= 0)
     return;

   GetDocAndView (frame, &pDoc, &view, &assoc);
   /* met a jour la nouvelle capacite de la vue et cherche le pave racine */
   /* de la vue */
   if (pDoc == NULL)
      printf ("\nErreur DecreaseVolume: frame incorrecte\n");
   else
     {
	if (assoc)
	  {
	     /* element associe */
	     /* attention, vue contient le numero d'element associe */
	     pAb = pDoc->DocAssocRoot[view - 1]->ElAbstractBox[0];
	     pDoc->DocAssocVolume[view - 1] = pAb->AbVolume - dVol;
	  }
	else
	  {
	     /* element de l'arbre principal */
	     pAb = pDoc->DocViewRootAb[view - 1];
	     pDoc->DocViewVolume[view - 1] = pAb->AbVolume - dVol;
	  }
	if (pAb != NULL)
#ifdef __COLPAGE__
	  {
#endif /* __COLPAGE__ */
	     if (dVol >= pAb->AbVolume)
		printf ("Erreur DecreaseVolume: dVol=%3d volume view=%3d\n", dVol, pAb->AbVolume);
	     /* supprime les paves */
	     SupprAbsBoxes (pAb, pDoc, head, &dVol);
#ifdef __COLPAGE__
	     /* signale au Mediateur les paves modifies */
	     h = BreakPageHeight;
	     /* appel de modifVue depuis la racine car de nouvelles */
	     /* pages ont pu etre detruites */
	     (void) ChangeConcreteImage (frame, &h, pAb);
	     /* meme chose pour FreeDeadAbstractBoxes */
	     FreeDeadAbstractBoxes (pAb);
	     /* DocAssocModifiedAb et DocViewModifiedAb non utilises */
	     if (assoc)
		pDoc->DocAssocModifiedAb[view - 1] = NULL;
	     else
		pDoc->DocViewModifiedAb[view - 1] = NULL;
	  }
#else  /* __COLPAGE__ */
	     /* signale au Mediateur les paves modifies */
	     h = PageHeight;
	     if (assoc)
	       {
		  if (pDoc->DocAssocModifiedAb[view - 1] != NULL)
		    {
		       (void) ChangeConcreteImage (frame, &h, pDoc->DocAssocModifiedAb[view - 1]);
		       FreeDeadAbstractBoxes (pDoc->DocAssocModifiedAb[view - 1]);
		       pDoc->DocAssocModifiedAb[view - 1] = NULL;
		    }
	       }
	     else if (pDoc->DocViewModifiedAb[view - 1] != NULL)
	       {
		  (void) ChangeConcreteImage (frame, &h, pDoc->DocViewModifiedAb[view - 1]);
		  FreeDeadAbstractBoxes (pDoc->DocViewModifiedAb[view - 1]);
		  pDoc->DocViewModifiedAb[view - 1] = NULL;
	       }
#endif /* __COLPAGE__ */
     }
}


/*----------------------------------------------------------------------
   CheckAbsBox verifie que l'element pointe' par pEl a au moins      
   un pave dans la vue view. S'il n'en a pas, essaie d'en   
   creer un en modifiant l'image abstraite de cette vue.   
   Si debut est vrai, on cree l'image de la vue en         
   commencant par l'element pointe' par pEl, sinon, on     
   place cet element au milieu de l'image creee.           
   Si affiche est Vrai, l'image est reaffichee.            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CheckAbsBox (PtrElement pEl, int view, PtrDocument pDoc, boolean begin, boolean display)
#else  /* __STDC__ */
void                CheckAbsBox (pEl, view, pDoc, begin, display)
PtrElement          pEl;
int                 view;
PtrDocument         pDoc;
boolean             begin;
boolean             display;

#endif /* __STDC__ */
{
  boolean             openedView, creation, stop;
  PtrElement          pElAscent, pElPage;
  PtrElement          pAsc[MaxAsc];
  int                 NumAsc, i, volsupp, frame, nAssoc, boxType, h;
  PtrAbstractBox      pAbbDestroyed, pAbbRemain, pAbbLastEmptyCr, pAbbFirstEmptyCr;
  PtrAbstractBox      pAbbReDisp, pAbbRoot, pPrevious;
  PtrElement          pEl1;
  PtrAbstractBox      pAbbox1;
  boolean             complete;
  
#ifdef __COLPAGE__
  int                 viewSch, cycle;
  PtrAbstractBox      pAbbPage, pAbb;
  PtrElement          pElRoot, pElChild, pElRef;
  boolean             found, pagedView, isAssoc, toCreate;
  PtrPSchema          pSchPage;

  pagedView = FALSE;
  pAbbLastEmptyCr = NULL;
  pElPage = NULL;
  if (pEl != NULL && pEl->ElStructSchema != NULL)
    {
      nAssoc = pEl->ElAssocNum;
      /* verifie si la vue a ete creee */
      isAssoc = AssocView (pEl);
      if (isAssoc)
	/* element associe qui s'affiche dans une autre vue que */
	/* la vue principale */
	{
	  openedView = pDoc->DocAssocFrame[nAssoc - 1] != 0 && view == 1;
	  pElRoot = pDoc->DocAssocSubTree[nAssoc - 1];
	  if (pElRoot == NULL)
	    pElRoot = pDoc->DocAssocRoot[nAssoc - 1];
	  if (pElRoot != NULL)
	    pAbbRoot = pElRoot->ElAbstractBox[view - 1];
	  frame = pDoc->DocAssocFrame[nAssoc - 1];
	}
      else
	{
	  openedView = pDoc->DocView[view - 1].DvPSchemaView > 0;
	  pElRoot = pDoc->DocRootElement;
	  pAbbRoot = pElRoot->ElAbstractBox[view - 1];
	  frame = pDoc->DocViewFrame[view - 1];
	}
      /*  test si vue paginee */
      pEl1 = pElRoot->ElFirstChild;
      /* recherche le numero de vue defini dans le schema de presentation */
      /* Attention il faut appeler AppliedView avec pElRoot */
      viewSch = AppliedView (pElRoot, NULL, pDoc, view);
      if (GetPageBoxType (pEl1, viewSch, &pSchPage) != 0)
	pagedView = TRUE;
      toCreate = openedView;
      /* a priori on cree les paves de l'element */
      if (pagedView && toCreate)
	/* Vue paginee : code specifique */
	{
	  /* si l'element a un pave, est-ce le premier des dup ? */
	  /* si c'est le cas (le pave n'est pas coupe en tete), */
	  /* on ne cree rien car l'element a son premier pave deja cree */
	  found = FALSE;
	  pAbb = pEl->ElAbstractBox[view - 1];
	  if (pAbb != NULL)
	    {
	      toCreate = FALSE;	/* a priori rien a creer */
	      while (pAbb != NULL && pAbb->AbPresentationBox && pAbb->AbNext != NULL)
		pAbb = pAbb->AbNext;
	      if (pAbb != NULL && pAbb->AbElement == pEl && pAbb->AbLeafType == LtCompound)
		toCreate = (pAbb->AbTruncatedHead);
	    }
	  if (toCreate)
	    {
	      /* est-ce l'element racine d'un arbre d'elements associes ? */
	      pElRef = pEl;
	      if (pElRef->ElParent == NULL)
		/* c'est une racine */
		if (nAssoc != 0 && !isAssoc)
		  /* c'est un element associe racine d'elements */
		  {
		    /* s'affichant en haut ou bas de page */
		    /* on creera son pave sur la page de la premiere */
		    /* reference a son premier fils */
		    pElRef = pElRef->ElFirstChild;
		    if (pElRef == NULL)
		      toCreate = FALSE;
		  }
	    }
	  if (toCreate)
	    /* si c'est un element associe qui s'affiche en haut ou bas de page */
	    /* il faut rechercher la premiere reference et afficher la page */
	    /* de cet element reference. AbsBoxesCreate creera les paves de l'element */
	    /* associe par appel de CrPavHB */
	    if (nAssoc != 0 && !isAssoc)
	      /* on recherche l'ascendant qui est l'element reference (pEl peut */
	      /* etre un fils de cet element) */
	      /* il faut utiliser pElRef et non pEl */
	      {
		while (pElRef->ElParent->ElParent != NULL)
		  pElRef = pElRef->ElParent;
		cycle = 1;
		pEl1 = pElRef;
		while (!found && pEl1 != NULL)
		  {
		    pElPage = GetPageBreakForAssoc (pEl1, viewSch, &boxType);
		    if (pElPage == NULL)
		      /* c'est un element non reference, il faut creer son */
		      /* pave a cote d'un element reference frere */
		      /* si il n'y a aucun frere (seul elt) TODO plus tard */
		      if (cycle == 1)
			if (pEl1->ElPrevious != NULL)
			  pEl1 = pEl1->ElPrevious;
			else
			  {
			    cycle = 2;
			    /* au 2eme tour on prend les freres suivants */
			    pEl1 = pElRef->ElNext;
			  }
		      else
			{
			  pEl1 = pEl1->ElNext;
			}
		    else	/* pElPage != NULL */
		      found = TRUE;	/* on a trouver la page a creer */
		  }
		if (pEl1 == NULL)
		  toCreate = FALSE;	/* TODO plus tard */
	      }
	  
	  if (toCreate && !found)
	    {
	      /* on recherche la page dans laquelle se trouve pEl */
	      found = FALSE;
	      /* si pEl est une PageBreak, c'est fini */
	      if (pEl->ElTypeNumber == PageBreak + 1
		  && (pEl->ElPageType == PgBegin
		      || pEl->ElPageType == PgComputed
		      || pEl->ElPageType == PgUser)
		  && pEl->ElViewPSchema == viewSch)
		{
		  found = TRUE;
		  pElPage = pEl;
		}
	      else
		{
		  pEl1 = pEl;
		  while (!found && pEl1 != NULL)
		    {
		      pElPage = BackSearchTypedElem (pEl1, PageBreak + 1, NULL);
		      if (pElPage != NULL && pElPage->ElViewPSchema == viewSch
			  && (pElPage->ElPageType == PgBegin
			      || pElPage->ElPageType == PgComputed
			      || pElPage->ElPageType == PgUser))
			found = TRUE;
		      else
			pEl1 = pElPage;
		    }
		}
	      if (found && pElPage->ElAbstractBox[view - 1] != NULL)
		/* si trouve et si l'element marque page a un pave dans la vue */
		/* c'est fini: l'element pEl n'est pas visible dans cette vue, */
		/* sinon il aurait eu un pave */
		toCreate = FALSE;
	      if (!found)
		/* si on n'a pas trouve d'element marque page precedent */
		/* c'est que pEl est la racine ou un element marque page */
		/* d'une autre vue */
		/* on creera l'image a partir de la racine */
		/* sans partir d'une marque de page */
		/* sinon erreur */
		if (!(pEl->ElParent == NULL
		      || (pEl->ElTypeNumber == PageBreak + 1
			  && pEl->ElViewPSchema != viewSch)))
		  {
		    printf ("peut etre erreur CheckAbsBox : pas trouve de marque page %s",
			    pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrName);
		    printf ("\n");
		    toCreate = FALSE;
		  }
	    }
	  if (toCreate)
	    {
	      /* on detruit l'i.a. sauf la racine */
	      /* pour recreer correctement la partie de l'i.a. demandee */
	      pAbbRoot = pElRoot->ElAbstractBox[view - 1];
	      if (pAbbRoot != NULL)
		{
		  pAbbox1 = pAbbRoot->AbFirstEnclosed;
		  while (pAbbox1 != NULL)
		    {
		      SetDeadAbsBox (pAbbox1);
		      pAbbox1 = pAbbox1->AbNext;
		    }
		  /* pour que sa boite soit detruite */
		  pAbbRoot->AbDead = TRUE;
		  h = 0;
		  ChangeConcreteImage (frame, &h, pAbbRoot);
		  pAbbRoot->AbDead = FALSE;
		  pAbbRoot->AbNew = TRUE;
		  pAbbRoot->AbTruncatedHead = TRUE;
		  pAbbRoot->AbTruncatedTail = TRUE;
		  FreeDeadAbstractBoxes (pAbbRoot);
		}
	      else
		{
		  /* la racine n'a pas de pave, on le cree (sans la descendance) */
		  pAbbRoot = AbsBoxesCreate (pElRoot, pDoc, view, TRUE,
					     FALSE, &complete);
		  pDoc->DocViewRootAb[view - 1] = pAbbRoot;
		  pAbbRoot->AbTruncatedHead = TRUE;
		  pAbbRoot->AbTruncatedTail = TRUE;
		}
	    }
	  /* on cree les paves de la Marque Page */
	  /* par construction (cf. AbsBoxesCreate), ils seront mis sous */
	  /* la racine */
	  if (found)
	    {
	      pAbbPage = AbsBoxesCreate (pElPage, pDoc, view, TRUE, TRUE, &complete);
	      /* on cree les paves de ses ascendants en les marquant CT et CQ */
	      pElAscent = pElPage->ElParent;
	      pElChild = pElPage;
	      NumAsc = 0;
	      stop = FALSE;
	      do
		if (pElAscent == NULL)
		  stop = TRUE;
		else if (pElAscent->ElAbstractBox[view - 1] != NULL)
		  /* normalement c'est la racine */
		  stop = TRUE;
		else
		  {
		    /* on ne creera le pave de l'ascendant que si il */
		    /* doit avoir des fils dans la page */
		    if (!(NumAsc == 0 && pElChild->ElNext == NULL))
		      {
			if (NumAsc < MaxAsc)
			  NumAsc++;
			pAsc[NumAsc - 1] = pElAscent;
		      }
		    pElChild = pElAscent;
		    pElAscent = pElAscent->ElParent;
		  }
	      while (!stop);
	      /* cree les paves de ces elements, en commencant par */
	      /* celui qui contient tous les autres.  */
	      pAbbFirstEmptyCr = NULL;
	      i = NumAsc;
	      while (i > 0)
		{
		  /* cree juste le pave, sans sa descendance et sans lui */
		  /* appliquer les regles de presentation. */
		  pPrevious = AbsBoxesCreate (pAsc[i - 1], pDoc, view, TRUE, FALSE, &complete);
		  pPrevious = pAsc[i - 1]->ElAbstractBox[view - 1];
		  if (pPrevious != NULL)
		    {
		      /* il faudra appliquer au pave ses */
		      /* regles de presentation (sera fait par AbsBoxesCreate) */
		      pPrevious->AbSize = -1;
		      /* on le marque coupe */
		      pPrevious->AbTruncatedHead = TRUE;
		      pPrevious->AbTruncatedTail = TRUE;
		      if (pAbbFirstEmptyCr == NULL)
			/* 1er pave cree' */
			{
			  pAbbFirstEmptyCr = pPrevious;
			  /* on chaine ce pave sous le corps de page */
			  while (pAbbPage != NULL && pAbbPage->AbPresentationBox)
			    pAbbPage = pAbbPage->AbNext;
			  /* toujours vrai ? */
			  if (pAbbPage != NULL)
			    {
			      if (pPrevious->AbEnclosing->AbFirstEnclosed == pPrevious)
				pPrevious->AbEnclosing->AbFirstEnclosed =
				  pPrevious->AbNext;
			      if (pPrevious->AbNext != NULL)
				pPrevious->AbNext->AbPrevious = NULL;
			      pPrevious->AbNext = NULL;
			      pPrevious->AbEnclosing = pAbbPage;
			      if (pAbbPage->AbFirstEnclosed == NULL)
				pAbbPage->AbFirstEnclosed = pPrevious;
			      else
				{
				  pAbbox1 = pAbbPage->AbFirstEnclosed;
				  while (pAbbox1->AbNext != NULL)
				    pAbbox1 = pAbbox1->AbNext;
				  /* en queue */
				  pAbbox1->AbNext = pPrevious;
				  pPrevious->AbPrevious = pAbbox1;
				}
			    }
			}
		    }
		  i--;
		}
	    }		/* fin cas creation paves de page et des ascendants */
	  /* trouve est vrai */
	  if (found || toCreate)
	    /* on appelle IncreaseVolume pour creer l'i.a. page a page, */
	    /* appliquer les regles de presentation des paves deja crees et */
	    /* et creer l'image concrete */
	    {
	      IncreaseVolume (FALSE, THOT_MAXINT, frame);
	      /* on appelle ShowBox pour positionner correctement l'i.a. */
	      /* dans la fenetre */
	      if (display && !begin && pEl->ElAbstractBox[view - 1] != NULL)
		/* TODO : que faire dans les autres cas */
		ShowBox (frame, pEl->ElAbstractBox[view - 1]->AbBox, 1, 0);
	      
	      if (display)
		DisplayFrame (frame);
	    }
	}
      else if (pEl->ElAbstractBox[view - 1] == NULL)
	/* vue non paginee : on garde l'ancien code */
	{
#else  /* __COLPAGE__ */
	  int         nR;
	  
	  pAbbLastEmptyCr = NULL;
	  if (pEl != NULL)
	    if (pEl->ElAbstractBox[view - 1] == NULL)
	      {
		nAssoc = pEl->ElAssocNum;
		/* verifie si la vue a ete creee */
		if (AssocView (pEl))
		  /* element associe */
		  openedView = pDoc->DocAssocFrame[nAssoc - 1] != 0 && view == 1;
		else
		  openedView = pDoc->DocView[view - 1].DvPSchemaView > 0;
		/* est-ce l'element racine d'un arbre d'elements associes ? */
		pEl1 = pEl;
		if (pEl1->ElParent == NULL)
		  /* c'est une racine */
		  if (nAssoc != 0)
		    /* c'est un element associe */
		    /* nR: type des elements associes */
		    {
		      nR = pEl1->ElStructSchema->SsRule[pEl1->ElTypeNumber - 1].SrListItem;
		      /* si les elements associes s'affichent en haut ou en bas de */
		      /* page, la racine n'a jamais de pave */
		      openedView = !pEl1->ElStructSchema->SsPSchema->PsInPageHeaderOrFooter[nR - 1];
		      /* si la vue n'est pas creee, il n'y a rien a faire */
		    }
		if (openedView)
		  {
#endif /* __COLPAGE__ */
		    /* cherche les elements ascendants qui n'ont pas de pave dans */
		    /* cette vue */
		    NumAsc = 0;
		    /* niveau dans la pile des elements dont il faut */
		    /* creer un pave */
		    pElAscent = pEl;
		    stop = FALSE;
		    do
		      if (pElAscent == NULL)
			stop = TRUE;
		      else if (pElAscent->ElAbstractBox[view - 1] != NULL)
			stop = TRUE;
		    /* met un element dans la pile */
		      else
			{
			  if (NumAsc < MaxAsc)
			    NumAsc++;
			   pAsc[NumAsc - 1] = pElAscent;
			   /* passe a l'ascendant */
			   pEl1 = pElAscent;
			   if (pEl1->ElStructSchema->SsRule[pEl1->ElTypeNumber - 1].SrAssocElem)
			      /* on vient de traiter un element associe' */
			      /* Serait-ce un element qui s'affiche dans une boite de */
			      /* haut ou de bas de page ? */
			     {
				pElPage = GetPageBreakForAssoc (pElAscent, pDoc->DocView[view - 1].
						   DvPSchemaView, &boxType);
				if (pElPage != NULL)

				   /* Il s'affiche dans une haut ou bas de page, c'est la */
				   /* marque de page a laquelle il est associe qu'il faut */
				   /* creer */
				  {
				     NumAsc = 1;
				     pAsc[NumAsc - 1] = pElPage;
				     pElAscent = pElPage->ElParent;
				     /* ce n'est pas une vue d'elements associes */
				     nAssoc = 0;
				  }
				else if (boxType == 0)

				   /* il ne s'affiche pas dans une boite de haut ou de */
				   /* bas de page */
				   if (view == 1)
				      pElAscent = pEl1->ElParent;
				   else
				      /* ce n'est pas la vue 1, l'element (associe') */
				      /* n'a pas d'image dans cette vue */
				     {
					stop = TRUE;
					openedView = FALSE;
				     }
				else
				   /* il devrait s'afficher dans une boite de haut ou */
				   /* de bas de page, mais il n'y a pas de page */
				  {
				     stop = TRUE;
				     openedView = FALSE;
				  }
			     }
			   else
			      pElAscent = pEl1->ElParent;
			}
		   while (!stop);
		   if (pAsc[NumAsc - 1]->ElParent == NULL)
		      /* la racine de l'arbre n'a pas de pave dans cette vue */
		      creation = TRUE;
		   /* c'est une creation de vue */
		   else
		      creation = FALSE;
		   /* essaie de creer les paves de ces elements, en commencant par */
		   /* celui qui contient tous les autres. Il s'agit seulement de */
		   /* trouver s'il y a un pave ascendant non encore cree et visible */
		   pAbbFirstEmptyCr = NULL;
		   if (openedView)
		     {
			i = NumAsc;
			do
			  {
			     pEl1 = pAsc[i - 1];

			     /* cree juste le pave, sans sa descendance et sans */
			     /* lui appliquer les regles de presentation. */
			     pPrevious = AbsBoxesCreate (pAsc[i - 1], pDoc, view, TRUE, FALSE, &complete);
			     if (pEl1->ElAbstractBox[view - 1] != NULL)
				pPrevious = pEl1->ElAbstractBox[view - 1];
			     if (pPrevious != NULL)

				/* marque sur le pave cree qu'il faudra lui appliquer ses */
				/* regles de presentation (ce sera fait par AbsBoxesCreate) */
				pPrevious->AbSize = -1;

			     if (pPrevious != NULL)
				/* on a cree un pave */
			       {
				  pAbbLastEmptyCr = pPrevious;
				  /* dernier pave cree' */
				  if (pAbbFirstEmptyCr == NULL)
				     pAbbFirstEmptyCr = pPrevious;
				  /* 1er pave cree' */
			       }
			     i--;
			  }
			while (i != 0);
		     }
		   /* si aucun pave n'a ete cree', il n'y a rien d'autre a faire */
		   if (pAbbFirstEmptyCr != NULL)
		     {
			if (creation)
			   /* initialise le pointeur sur la racine de la vue si */
			   /* c'est une creation de vue */
			  {
			     if (nAssoc > 0)
				/* vue d'elements associes */
				/* le premier pave que l'on vient de creer est */
				/* la racine de l'image */
			       {
				  pDoc->DocAssocRoot[nAssoc - 1]->ElAbstractBox[0] = pAbbFirstEmptyCr;
				  pAbbRoot = pDoc->DocAssocRoot[nAssoc - 1]->ElAbstractBox[0];
				  frame = pDoc->DocAssocFrame[nAssoc - 1];
			       }
			     else
				/* vue de l'arbre principal */
			       {
				  pDoc->DocViewRootAb[view - 1] = pAbbFirstEmptyCr;
				  pAbbRoot = pDoc->DocViewRootAb[view - 1];
				  frame = pDoc->DocViewFrame[view - 1];
			       }
			  }
			else
			   /* ce n'est pas une creation de vue */
			   /* detruit l'ancien contenu de la vue */
			  {
			     pAbbRemain = pAbbFirstEmptyCr;
			     /* tue les paves de presentation des elements englobants */
			     pAbbReDisp = NULL;
			     KillPresEnclosing (pAbbRemain, TRUE, pDoc, &pAbbReDisp, &volsupp, TRUE);
			     pAbbReDisp = NULL;
			     KillPresEnclosing (pAbbRemain, FALSE, pDoc, &pAbbReDisp, &volsupp, TRUE);
			     /* detruit les paves qui precedent et qui suivent le pave */
			     /* cree de plus haut niveau et ses paves englobants */
			     while (pAbbRemain != NULL)
			       {
				  pAbbDestroyed = pAbbRemain->AbPrevious;
				  while (pAbbDestroyed != NULL)
				    {
				       if (!pAbbDestroyed->AbDead)
					  /* on ne detruit pas les paves des elements restant */
					  if (pAbbDestroyed->AbElement != pAbbRemain->AbElement)
					    {
					       SetDeadAbsBox (pAbbDestroyed);
					       pAbbReDisp = NULL;
					       ApplyRefAbsBoxSupp (pAbbDestroyed, &pAbbReDisp, pDoc);
					    }
				       pAbbDestroyed = pAbbDestroyed->AbPrevious;
				    }
				  pAbbDestroyed = pAbbRemain->AbNext;
				  while (pAbbDestroyed != NULL)
				    {
				       if (!pAbbDestroyed->AbDead)
					  /* on ne detruit pas les paves des elements restant */
					  if (pAbbDestroyed->AbElement != pAbbRemain->AbElement)
					    {
					       SetDeadAbsBox (pAbbDestroyed);
					       pAbbReDisp = NULL;
					       ApplyRefAbsBoxSupp (pAbbDestroyed, &pAbbReDisp, pDoc);
					    }
				       pAbbDestroyed = pAbbDestroyed->AbNext;
				    }
				  pAbbRemain = pAbbRemain->AbEnclosing;
				  /* passe a l'englobant */
			       }
			     /* fait effacer tout le contenu de la vue par le Mediateur */
			     h = 0;
			     /* on ne s'occupe pas de la hauteur de page */
			     if (nAssoc > 0)
				/* vue d'elements associes */
			       {
				  pAbbRoot = pDoc->DocAssocRoot[nAssoc - 1]->ElAbstractBox[0];
				  frame = pDoc->DocAssocFrame[nAssoc - 1];
				  if (frame != 0)
				    {
				       pAbbRoot->AbDead = TRUE;
				       /* marque mort le pave racine */
				       /* le signale au Mediateur */
				       ChangeConcreteImage (frame, &h, pAbbRoot);
				       pAbbRoot->AbDead = FALSE;
				       /* resucite le pave racine */
				       FreeDeadAbstractBoxes (pAbbRoot);
				       /* libere tous les pave detruits */
				       pDoc->DocAssocModifiedAb[nAssoc - 1] = NULL;
				    }
			       }
			     else
				/* meme traitement pour une vue de l'arbre principal */
			       {
				  pAbbRoot = pDoc->DocViewRootAb[view - 1];
				  frame = pDoc->DocViewFrame[view - 1];
				  if (frame != 0)
				    {
				       pAbbRoot->AbDead = TRUE;
				       ChangeConcreteImage (frame, &h, pAbbRoot);
				       pAbbRoot->AbDead = FALSE;
				       FreeDeadAbstractBoxes (pAbbRoot);
				       pDoc->DocViewModifiedAb[view - 1] = NULL;
				    }
			       }
			  }
			if (nAssoc > 0)
			   /* vue d'elements associes */
			   if (begin)
			      /* on creera tout le volume de la vue d'un coup */
			      pDoc->DocAssocFreeVolume[nAssoc - 1] = pDoc->DocAssocVolume[nAssoc - 1];
			   else
			      /* on creera la moitie du volume max. derriere les */
			      /* nouveaux paves, et une autre moitie devant */
			      pDoc->DocAssocFreeVolume[nAssoc - 1] = pDoc->DocAssocVolume[nAssoc - 1] / 2;
			/* vue de l'arbre principal */
			else if (begin)
			   pDoc->DocViewFreeVolume[view - 1] = pDoc->DocViewVolume[view - 1];
			else
			   pDoc->DocViewFreeVolume[view - 1] = pDoc->DocViewVolume[view - 1] / 2;
			/* marque comme anciens tous les paves conserves (pour */
			/* que AjoutePave travaille correctement) */
			pAbbRemain = pAbbLastEmptyCr;
			while (pAbbRemain != NULL)
			  {
			     pAbbox1 = pAbbRemain;
			     pAbbox1->AbNew = FALSE;
			     if (creation)
				/* a priori les paves ne sont pas complets */
#ifdef __COLPAGE__
				if (pAbbox1->AbLeafType == LtCompound)
#else  /* __COLPAGE__ */
				if (pAbbox1->AbLeafType == LtCompound
				    && !pAbbox1->AbInLine)
#endif /* __COLPAGE__ */
				  {
				     pAbbox1->AbTruncatedHead = TRUE;
				     pAbbox1->AbTruncatedTail = TRUE;
				  }
			     pAbbRemain = pAbbox1->AbEnclosing;
			     /* passe a l'englobant */
			  }
			/* applique les regles des paves nouvellement crees et cree */
			/* d'autres paves derriere */
			AddAbsBoxes (pAbbRoot, pDoc, FALSE);
			/* complete les boites qui doivent etre completes */
			i = NumAsc;
			do
			  {
			     pEl1 = pAsc[i - 1];
			     if (pEl1->ElStructSchema->SsPSchema->PsBuildAll[pEl1->ElTypeNumber - 1])
				/* cet element a la regle Gather */
				/* cree le pave avec toute sa descendance, si */
				/* ce n'est pas encore fait */
			       {
				  pPrevious = AbsBoxesCreate (pAsc[i - 1], pDoc, view, TRUE, TRUE, &complete);
				  i = 1;
			       }
			     i--;
			  }
			while (i != 0);
			if (!begin)

			   /* cree d'autres paves devant, jusqu'a remplir le volume de */
			   /* la fenetre */
			  {
			     if (nAssoc > 0)
				pDoc->DocAssocFreeVolume[nAssoc - 1] = pDoc->DocAssocVolume[nAssoc - 1] / 2;
			     else
				pDoc->DocViewFreeVolume[view - 1] = pDoc->DocViewVolume[view - 1] / 2;
			     /* marque comme anciens tous les paves de presentation qui */
			     /* viennent d'etre crees par AddAbsBoxes devant les paves */
			     /* conserves. Ces paves de presentation seront ainsi traites */
			     /* correctement lors du prochain appel de AddAbsBoxes. */
			     pAbbRemain = pAbbLastEmptyCr;
			     while (pAbbRemain != NULL)
			       {
				  pPrevious = pAbbRemain->AbPrevious;
				  stop = FALSE;
				  do
				     if (pPrevious == NULL)
					stop = TRUE;
				     else if (pPrevious->AbElement != pAbbRemain->AbElement)
					stop = TRUE;
				     else
				       {
					  pPrevious->AbNew = FALSE;
					  pPrevious = pPrevious->AbPrevious;
				       }
				  while (!stop);
				  pAbbRemain = pAbbRemain->AbEnclosing;
				  /* passe a l'englobant */
			       }
			     /* cree de nouveaux paves */
			     AddAbsBoxes (pAbbRoot, pDoc, TRUE);
			     /* marque comme nouveaux tous les paves de presentation qui */
			     /* viennent d'etre marques anciens. Ces paves de presentation */
			     /* seront ainsi traites correctement par le Mediateur. */
			     pAbbRemain = pAbbLastEmptyCr;
			     while (pAbbRemain != NULL)
			       {
				  pPrevious = pAbbRemain->AbPrevious;
				  stop = FALSE;
				  do
				     if (pPrevious == NULL)
					stop = TRUE;
				     else if (pPrevious->AbElement != pAbbRemain->AbElement)
					stop = TRUE;
				     else
				       {
					  pPrevious->AbNew = TRUE;
					  pPrevious = pPrevious->AbPrevious;
				       }
				  while (!stop);
				  pAbbRemain = pAbbRemain->AbEnclosing;
				  /* passe a l'englobant */
			       }
			  }
			/* marque comme nouveaux tous les paves conserves (pour qu'ils */
			/* soient traites correctement par le Mediateur) */
			pAbbRemain = pAbbLastEmptyCr;
			while (pAbbRemain != NULL)
			  {
			     pAbbox1 = pAbbRemain;
			     pAbbox1->AbNew = TRUE;
			     pAbbRemain = pAbbox1->AbEnclosing;
			     /* passe a l'englobant */
			  }
			/* indique les nouvelles modifications au Mediateur et */
			/*  lui fait reafficher toute la vue */
			if (frame != 0)
			  {
			     h = 0;
			     ChangeConcreteImage (frame, &h, pAbbRoot);
			     if (display)
				DisplayFrame (frame);
			     /* il n'y a plus rien a reafficher dans cette vue */
			     if (nAssoc > 0)
				pDoc->DocAssocModifiedAb[nAssoc - 1] = NULL;
			     else
				pDoc->DocViewModifiedAb[view - 1] = NULL;
			  }
		     }
		}		/* V4 : fin cas vue non paginee , V3 fin Vue existe */
	   }			/* V4 : fin pEl != NULL , V3 : fin pEl->ElAbstractBox[] == NULL */
   }


/*----------------------------------------------------------------------
   VolumeTree    retourne                                        
   *volBefore: le volume total des elements de l'arbre abstrait   
   qui precedent l'element auquel appartient le pave  
   pAbbFirst.                                           
   *volAfter: le volume total des elements de l'arbre abstrait   
   qui se trouvent apres l'element auquel appartient  
   le pave pAbbLast.                                    
   volTree: le volume total de l'arbre abstrait.               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                VolumeTree (PtrAbstractBox pAbbRoot, PtrAbstractBox pAbbFirst, PtrAbstractBox pAbbLast, int *volBefore, int *volAfter, int *volTree)
#else  /* __STDC__ */
void                VolumeTree (pAbbRoot, pAbbFirst, pAbbLast, volBefore, volAfter, volTree)
PtrAbstractBox      pAbbRoot;
PtrAbstractBox      pAbbFirst;
PtrAbstractBox      pAbbLast;
int                *volBefore;
int                *volAfter;
int                *volTree;
#endif /* __STDC__ */
{
  PtrElement          pEl;
  
  if (pAbbFirst == NULL || pAbbLast == NULL)
    /* les parametres d'appels sont errone's */
    {
      *volBefore = 0;
      *volAfter = 0;
      *volTree = 0;
    }
  else
    {
      /* calcule d'abord le volume total de l'arbre abstrait */
      *volTree = pAbbRoot->AbElement->ElVolume;
      
      /* on calcule maintenant le volume de ce qui precede */
      *volBefore = 0;
      if (pAbbRoot->AbTruncatedHead)
	{
	  /* si le premier pave' correspond a un element associe' affiche' */
	  /* dans un haut ou bas de page, on prend le pave' saut de page */
	  while (pAbbFirst->AbElement->ElAssocNum != pAbbRoot->AbElement->ElAssocNum)
	    pAbbFirst = pAbbFirst->AbEnclosing;
	  pEl = pAbbFirst->AbElement;
	  /* pour l'element et tous ses ascendants, on accumule le */
	  /* volume de tous leurs freres precedents */
	  do
	    {
	      /* traite tous les freres precedents */
	      while (pEl->ElPrevious != NULL)
		{
		  pEl = pEl->ElPrevious;
		  *volBefore += pEl->ElVolume;
		}
	      /* passe a l'ascendant */
	      pEl = pEl->ElParent;
	    }
	  while (pEl != NULL);
	}
      
      /* on calcule le volume de ce qui suit */
      *volAfter = 0;
      if (pAbbRoot->AbTruncatedTail)
	{
	  /* si le dernier pave' correspond a un element associe' affiche' */
	  /* dans un haut ou bas de page, on prend le pave' saut de page */
	  while (pAbbLast->AbElement->ElAssocNum != pAbbRoot->AbElement->ElAssocNum)
	    pAbbLast = pAbbLast->AbEnclosing;
	  pEl = pAbbLast->AbElement;
	  /* pour l'element et tous ses ascendants, on accumule le */
	  /* volume de tous leurs freres suivants */
	  do
	    {
	      /* traite tous les freres suivants */
	      while (pEl->ElNext != NULL)
		{
		  pEl = pEl->ElNext;
		  *volAfter += pEl->ElVolume;
		}
	      /* passe a l'ascendant */
	      pEl = pEl->ElParent;
	    }
	  while (pEl != NULL);
	}
    }
}


/*----------------------------------------------------------------------
   JumpIntoView fait afficher dans la fenetre la partie de        
   document qui se trouve a la distance indiquee du debut  
   de l'arbre abstrait.                                    
   distance est un pourcentage : 0 <= distance <= 100      
   Si distance = 0, on fait afficher le debut de l'arbre.  
   Si distance = 100, on fait afficher la fin de l'arbre.  
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                JumpIntoView (int frame, int distance)

#else  /* __STDC__ */
void                JumpIntoView (frame, distance)
int                 frame;
int                 distance;
#endif /* __STDC__ */
{
  PtrDocument         pDoc;
  int                 view;
  boolean             isAssoc;
  PtrElement          pEl;
  int                 volBefore, volPresent;
  boolean             after;
  int                 position;
  PtrAbstractBox      pAbbRoot, pAb;
  PtrElement          pElSuiv;
  
  position = 0;
  if (distance >= 0 && distance <= 100)
    /* la distance demandee est valide */
    {
      /* cherche le document et la vue correspondant a la fenetre */
      GetDocAndView (frame, &pDoc, &view, &isAssoc);
      if (pDoc != NULL)
	{
	  /* cherche la racine de l'arbre affiche' dans cette frame */
	  if (isAssoc)
	    {
	      pEl = pDoc->DocAssocRoot[view - 1];
	      view = 1;
	    }
	  else
	    pEl = pDoc->DocRootElement;
	  if (pEl != NULL)
	    {
	      pAbbRoot = pEl->ElAbstractBox[view - 1];
	      /* volume avant l'element a rendre visible */
	      volBefore = (pEl->ElVolume * distance) / 100;
	      /* cherche l'element a rendre visible */
	      volPresent = 0;
	      while (!pEl->ElTerminal && pEl->ElFirstChild != NULL)
		{
		  pEl = pEl->ElFirstChild;
		  while (volPresent + pEl->ElVolume <= volBefore
			 && pEl->ElNext != NULL)
		    {
		      volPresent += pEl->ElVolume;
		      pEl = pEl->ElNext;
		    }
		}
	      if (pEl == NULL)
		pAb = NULL;
	      else
		{
		  pAb = pEl->ElAbstractBox[view - 1];
		  if (pAb == NULL)
		    {
		      CheckAbsBox (pEl, view, pDoc, FALSE, FALSE);
		      pAb = pEl->ElAbstractBox[view - 1];
		    }
		}
	      after = TRUE;
	      while (pEl != NULL && pAb == NULL)
		/* pEl n'a pas de pave dans le vue traitee. */
		/* On cherche la feuille la plus proche de pEl qui */
		/* soit visible dans la vue */
		{
		  if (after)
		    pElSuiv = NextElement (pEl);
		  else
		    pElSuiv = NULL;
		  if (pElSuiv != NULL)
		    /* on prend la feuille pElSuivante */
		    if (!pElSuiv->ElTerminal)
		      pEl = FirstLeaf (pElSuiv);
		    else
		      pEl = pElSuiv;
		  else
		    /* on prend la feuille precedente */
		    {
		      after = FALSE;
		      while (pEl->ElPrevious == NULL && pEl->ElParent != NULL)
			pEl = pEl->ElParent;
		      pEl = pEl->ElPrevious;
		      if (pEl != NULL)
			while (!pEl->ElTerminal && pEl->ElFirstChild != NULL)
			  {
			    pEl = pEl->ElFirstChild;
			    while (pEl->ElNext != NULL)
			      pEl = pEl->ElNext;
			  }
		    }
		  
		  /* rend l'element visible dans sa frame, sans l'afficher */
		  if (pEl != NULL)
		    {
		      CheckAbsBox (pEl, view, pDoc, FALSE, FALSE);
		      pAb = pEl->ElAbstractBox[view - 1];
		    }
		}
	      
	      pAb = NULL;
	      /* positionne l'image dans sa frame, en l'affichant */
	      if (distance == 0)
		/* boite racine de la vue en haut de la fenetre */
		{
		  position = 0;
		  pAb = pAbbRoot;
		}
	      else if (distance == 100)
		/* boite racine de la vue en bas de la fenetre */
		{
		  position = 2;
		  pAb = pAbbRoot;
		}
	      else if (pEl != NULL)
		/* boite de l'element au milieu de la fenetre */
		{
		  if (pEl->ElAbstractBox[view - 1] == NULL)
		    /* cherche le premier element precedent qui a */
		    /* un pave' dans la vue */
		    pEl = BackSearchVisibleElem (pAbbRoot->AbElement, pEl, view);
		  if (pEl != NULL)
		    pAb = pEl->ElAbstractBox[view - 1];
		  position = 1;
		}
	      if (pAb != NULL)
		ShowBox (frame, pAb->AbBox, position, 0);
	      
	      /* Allume la selection */
	      HighlightSelection (FALSE);
	    }
	}
    }
}

#ifdef __COLPAGE__
/* affichage du contexte de pAb pour debug */
#ifdef __STDC__
   void                AffPaveDebug (PtrAbstractBox pAb)
#else  /* __STDC__ */
   void                AffPaveDebug (pAb)
   PtrAbstractBox      pAb;
#endif /* __STDC__ */
   {
      PtrElement          pEl;

      if (pAb != NULL)
	{
	   pEl = pAb->AbElement;
	   printf ("contexte pAb = %x", (int) pAb, " %s",
		 pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrName);
	   printf ("pres = %d", pAb->AbPresentationBox);
	   printf ("\n");
	   printf ("pere = %x", (int) pAb->AbEnclosing);
	   if (pAb->AbEnclosing != NULL)
	      printf (" %s",
		      pAb->AbEnclosing->AbElement->ElStructSchema->SsRule[pAb->AbEnclosing->AbElement->ElTypeNumber - 1].SrName);
	   printf ("\n");
	   printf ("frere avant = %x", (int) pAb->AbPrevious);
	   if (pAb->AbPrevious != NULL)
	      printf (" %s",
		      pAb->AbPrevious->AbElement->ElStructSchema->SsRule[pAb->AbPrevious->AbElement->ElTypeNumber - 1].SrName);
	   printf ("\n");
	   printf ("frere after  = %x", (int) pAb->AbNext);
	   if (pAb->AbNext != NULL)
	      printf (" %s",
		      pAb->AbNext->AbElement->ElStructSchema->SsRule[pAb->AbNext->AbElement->ElTypeNumber
							       - 1].SrName);
	   printf ("\n");
	   if (pAb->AbPreviousRepeated != NULL)
	      printf (" dup precedent = %x", (int) pAb->AbPreviousRepeated);
	   if (pAb->AbNextRepeated != NULL)
	      printf (" dup suivant  = %x", (int) pAb->AbNextRepeated);
	   printf ("\n");
	}
   }

   /*----------------------------------------------------------------------
      AbsBoxOk verifie les conditions d'invariance sur le pave pAb
      et affiche un message d'erreur si une condition       
      pas respectee. On verifie que :                       
      1. la suite des paves de l'element pAb               
      ne contienne pas que des paves de presentation,    
      2. si pAb->AbPreviousRepeated != NULL                    
      alors l'element ne pointe pas sur pAb             
      3. si pAb->AbNextRepeated(ou Prec)  != NULL           
      alors tous les ascendants doivent l'etre aussi     
     ----------------------------------------------------------------------*/

#ifdef __STDC__
   boolean             AbsBoxOk (PtrAbstractBox pAb)

#else  /* __STDC__ */
   boolean             AbsBoxOk (pAb)
   PtrAbstractBox      pAb;

#endif /* __STDC__ */

   {
      PtrAbstractBox      pAbb, pAbbox1;
      PtrElement          pEl;
      boolean             correct, allKill;
      int                 AbDocView;

      pAbbox1 = pAb;
      AbDocView = pAb->AbDocView;
      /* a priori le pave est correct */
      correct = TRUE;
      allKill = TRUE;
      pEl = pAb->AbElement;

      /* premiere verification */
      /* message d'erreur si la suite des paves pointes par */
      /* pEl->ElAbstractBox ne contient que  */
      /* des paves de presentation  dont certains sont non morts */
      pAbb = pEl->ElAbstractBox[pAbbox1->AbDocView - 1];
      if (pAbb != NULL)		/* il existe des paves pour l'element */
	{
	   while (pAbb != NULL && pAbb->AbPresentationBox)
	     {
		if (!pAbb->AbDead)
		   allKill = FALSE;
		if (pAbb->AbNext != NULL)
		   if (pAbb->AbNext->AbElement == pAbb->AbElement)
		      pAbb = pAbb->AbNext;
		   else
		      pAbb = NULL;
		else
		   pAbb = NULL;
	     }
	   if (pAbb == NULL && !allKill)
	      /* erreur dans l'image abstraite */
	     {
		correct = FALSE;
		printf ("Erreur dans i.a : l'elem n'a que des paves pres %s",
		 pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrName);
		printf ("\n");
	     }
	}

      /* un pave de presentation ne peut pas etre duplique ?? */
      if (pAb->AbPresentationBox &&
	  (pAb->AbPreviousRepeated != NULL || pAb->AbNextRepeated != NULL))
	{
	   correct = FALSE;
	   printf ("Erreur dans l'i.a. : pave de pres duplique %s",
		 pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrName);
	   printf ("\n");
	}
      /* deuxieme verification */
      if (pAb->AbPreviousRepeated != NULL)
	{
	   if (pEl->ElAbstractBox[AbDocView - 1] == pAb)
	      /* l'element pointe sur pAb alors qu'il a un dupPrec */
	     {
		correct = FALSE;
		printf ("Erreur dans l'i.a.:l'elem pointe sur pAb %s",
		 pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrName);
		printf ("\n");
	     }
	}

      /* troisieme verification */
      /* si pAb a AbNextRepeated(ou Prec) != NULL alors tous ses ascendants ont */
      /* AbNextRepeated != NULL (jusqu'a corps de page) */
      if (pAb->AbNextRepeated != NULL || pAb->AbPreviousRepeated != NULL)
	{
	   pAbb = pAb->AbEnclosing;
	   if (pAbb == NULL)
	     {
		correct = FALSE;
		printf ("Erreur dans l'i.a.: la racine a des dupliques %s",
		 pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrName);
		printf ("\n");
	     }
	   else
	     {
		while (pAbb != NULL && pAbb->AbElement->ElTypeNumber != PageBreak + 1)
		  {
		     if ((pAb->AbNextRepeated != NULL && pAbb->AbNextRepeated == NULL)
			 || (pAb->AbPreviousRepeated != NULL && pAbb->AbPreviousRepeated == NULL))
		       {
			  /*cas d'erreur */
			  correct = FALSE;
			  printf ("Erreur dans l'i.a.: l'ascendant devrait etre dup %s",
				  pAbb->AbElement->ElStructSchema->SsRule[pAbb->AbElement->ElTypeNumber
							       - 1].SrName);
			  printf ("\n");
		       }
		     pAbb = pAbb->AbEnclosing;
		  }
		if (pAbb == NULL)
		   /* on n'a pas rencontre le corps de page -> erreur */
		  {
		     correct = FALSE;
		     printf ("Erreur i.a.: dup en dehors un corps de page %s",
			     pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrName);
		     printf ("\n");
		  }
	     }
	}
      if (!correct)
	 /* affichage du contexte */
	 AffPaveDebug (pAb);

      return correct;
   }
/** fin ajout */
#endif /* __COLPAGE__ */
