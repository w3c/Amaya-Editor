/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Module de manipulations des images abstraites.
 *
 * Authors: V. Quint (INRIA)
 *          C. Roisin (INRIA) - Columns and pages
 *          R. Guetari (W3C/INRIA) - Unicode and Windows version
 *
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "picture.h"
#include "appdialogue.h"

#define THOT_EXPORT extern
#include "page_tv.h"
#include "appdialogue_tv.h"

#include "absboxes_f.h"
#include "abspictures_f.h"
#include "buildboxes_f.h"
#include "changeabsbox_f.h"
#include "content_f.h"
#include "createabsbox_f.h"
#include "createpages_f.h"
#include "frame_f.h"
#include "memory_f.h"
#include "presrules_f.h"
#include "scroll_f.h"
#include "structlist_f.h"
#include "structselect_f.h"
#include "tree_f.h"
#include "ustring_f.h"
#include "viewcommands_f.h"
#include "views_f.h"


#define MaxAsc 30
/* static CHAR_T       text[MAX_TXT_LEN]; */


/*----------------------------------------------------------------------
   AbsBoxType  rend un pointeur sur un buffer qui contient           
   le type de l'element de structure auquel correspond le  
   pave pointe' par pAb.                                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
CHAR_T*             AbsBoxType (PtrAbstractBox pAb, ThotBool origName)

#else  /* __STDC__ */
CHAR_T*             AbsBoxType (pAb, origName)
PtrAbstractBox      pAb;
ThotBool		    origName;

#endif /* __STDC__ */

{
   PtrElement       pEl;
   CHAR_T*          text;

   text = TtaAllocString (MAX_TXT_LEN);

   if (pAb == NULL)
      ustrcpy (text, TEXT(" "));
   else
     {
	pEl = pAb->AbElement;
	/* copie le nom du type d'element structure auquel appartient la boite */
	if (origName)
	   ustrcpy (text, pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrOrigName);
	else
	   ustrcpy (text, pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrName);
	if (pAb->AbPresentationBox)
	  /* Ajoute le nom du type de boite de presentation */
	  {
	     ustrcat (text, TEXT("."));
	     ustrcat (text, pAb->AbPSchema->PsPresentBox[pAb->AbTypeNum - 1].PbName);
	  }
     }
   return (text);
}



/*----------------------------------------------------------------------
   FreeAbView libere, pour une seule vue, tous les paves englobes par le
   pave pointe par pAb, lui-meme compris.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FreeAbView (PtrAbstractBox pAb, int frame)
#else  /* __STDC__ */
void                FreeAbView (pAb, frame)
PtrAbstractBox      pAb;
int                 frame;
#endif /* __STDC__ */
{
  PtrAbstractBox      pAbb, pAbbNext;
  PtrTextBuffer       pBT, pBTSuiv;
  PtrDelayedPRule     pDelPR, pNextDelPR;
  ThotBool            libAb;

  if (pAb != NULL)
    {
      /* reformat the last edited paragraph if needed */
      if (ThotLocalActions[T_updateparagraph] != NULL)
	(*ThotLocalActions[T_updateparagraph]) (pAb, frame);
      pAbb = pAb->AbFirstEnclosed;
      /* libere tous les paves englobes */
      while (pAbb != NULL)
	{
	     pAbbNext = pAbb->AbNext;
	     FreeAbView (pAbb, frame);
	     pAbb = pAbbNext;
	}

      if (pAb->AbBox != NULL)
	/* remove the box before freeing the abstract box */
	ComputeUpdates (pAb, frame);

      /* dechaine pAb des autres paves */
      if (pAb->AbNext != NULL)
	pAb->AbNext->AbPrevious = pAb->AbPrevious;
      if (pAb->AbPrevious != NULL)
	pAb->AbPrevious->AbNext = pAb->AbNext;
      if (pAb->AbEnclosing != NULL)
	if (pAb->AbEnclosing->AbFirstEnclosed == pAb)
	  pAb->AbEnclosing->AbFirstEnclosed = pAb->AbNext;
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
	    if (pAb->AbPresentationBox)
	      {
		/* c'est une boite de presentation image */
		FreePictInfo ((PictInfo *)(pAb->AbPictInfo));
		TtaFreeMemory (pAb->AbPictInfo);
		pAb->AbPictInfo = NULL;
	      }
	    break;
	  case LtCompound:
	    if (pAb->AbPictBackground != NULL)
	      {
		/* in this particular case we need to free filename */
		TtaFreeMemory (((PictInfo *)(pAb->AbPictBackground))->PicFileName);
		/* ce n'est pas un element image */
		FreePictInfo ((PictInfo *)(pAb->AbPictBackground));
		TtaFreeMemory (pAb->AbPictBackground);
		pAb->AbPictBackground = NULL;
	      }
	    break;
	  default:
	    break;
	  }

      /* dechaine pAb de son element */
      if (pAb->AbElement != NULL)
        if (pAb->AbElement->ElAbstractBox[pAb->AbDocView - 1] == pAb)
	  {
	    if (pAb->AbNext != NULL)
	      if (pAb->AbNext->AbElement == pAb->AbElement)
		pAb->AbElement->ElAbstractBox[pAb->AbDocView - 1] = pAb->AbNext;
	      else
		pAb->AbElement->ElAbstractBox[pAb->AbDocView - 1] = NULL;
	    else
	      pAb->AbElement->ElAbstractBox[pAb->AbDocView - 1] = NULL;
	  }
	else
	  /* est-ce un pave de presentation cree' par une regle FnCreateEnclosing */
	  if (pAb->AbPresentationBox)
	    if (pAb->AbElement->ElAbstractBox[pAb->AbDocView - 1] != NULL)
	      if (pAb->AbElement->ElAbstractBox[pAb->AbDocView - 1]->AbEnclosing == pAb)
		pAb->AbElement->ElAbstractBox[pAb->AbDocView - 1] = NULL;
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
   FreeAbEl libere, dans toutes les vues, tous les paves de      
   l'element pointe par pEl.                               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FreeAbEl (PtrElement pEl, PtrDocument pDoc)
#else  /* __STDC__ */
void                FreeAbEl (pEl, pDoc)
PtrElement          pEl;
#endif /* __STDC__ */

{
   PtrAbstractBox      pAbb, pAbbNext;
   int                 v, asView;
   int                 frame;
   ThotBool            stop, assoc;

   assoc = AssocView (pEl);
   asView = pEl->ElAssocNum - 1;
   if (pEl != NULL && pDoc != NULL)
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
		   if (assoc)
		     {
		       frame = pDoc->DocAssocFrame[asView];
		       if (pDoc->DocAssocRoot[asView]->ElAbstractBox[0] == pAbb)
			 pDoc->DocAssocRoot[asView]->ElAbstractBox[0] = NULL;
		       FreeAbView (pAbb, frame);
		     }
		   else
		     {
		       frame = pDoc->DocViewFrame[v];
		       FreeAbView (pAbb, frame);
		     }
		   pAbb = pAbbNext;
		}
	   while (!stop);
	}
}

/*----------------------------------------------------------------------
   FreeDeadAbstractBoxes libere tous les paves marques Mort dans le           
   sous-arbre de racine pAb.                              
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FreeDeadAbstractBoxes (PtrAbstractBox pAb, int frame)
#else  /* __STDC__ */
void                FreeDeadAbstractBoxes (pAb, frame)
PtrAbstractBox      pAb;
int                 frame;
#endif /* __STDC__ */
{
   PtrAbstractBox      pAbb, pAbbNext;


   if (pAb != NULL)
      if (pAb->AbDead)
	 FreeAbView (pAb, frame);
      else
	{
	   pAbb = pAb->AbFirstEnclosed;
	   /* teste tous les paves englobes */
	   while (pAbb != NULL)
	     {
		pAbbNext = pAbb->AbNext;
		FreeDeadAbstractBoxes (pAbb, frame);
		pAbb = pAbbNext;
	     }
	}
}



/*----------------------------------------------------------------------
   AddAbsBoxes complete la vue dont pAbbRoot est le pave racine   
   en ajoutant des paves, en tete si head est vrai,        
   en queue sinon.                                         
    pAbbRoot est une vraie racine de paves               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                AddAbsBoxes (PtrAbstractBox pAbbRoot, PtrDocument pDoc, ThotBool head)
#else  /* __STDC__ */
void                AddAbsBoxes (pAbbRoot, pDoc, head)
PtrAbstractBox      pAbbRoot;
PtrDocument         pDoc;
ThotBool            head;
#endif /* __STDC__ */
{
   PtrAbstractBox      pAb;
   PtrElement          pEl;
   ThotBool            complete;
   PtrAbstractBox      pAbbReDisp, pAbbR, PcFirst, PcLast, pAbb;
   ThotBool            stop;

   if ((head && pAbbRoot->AbTruncatedHead) ||
       (!head && pAbbRoot->AbTruncatedTail))
     {
       /* cree les paves de la partie coupee jusqu'a concurrence du volume libre */
       pEl = pAbbRoot->AbElement;
       pAb = AbsBoxesCreate (pEl, pDoc, pAbbRoot->AbDocView, (ThotBool)(!head), TRUE, (ThotBool*)(&complete));
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
ThotBool            IsBreakable (PtrAbstractBox pAb)
#else  /* __STDC__ */
ThotBool            IsBreakable (pAb)
PtrAbstractBox      pAb;
#endif /* __STDC__ */
{
   ThotBool            unbreakable;
   int                 index;
   PtrPSchema          pSchP;
   PtrSSchema          pSchS;

   unbreakable = FALSE;
   /* boucle sur les paves englobants */
   while (pAb != NULL && !unbreakable)
     {
	if (pAb->AbLeafType == LtCompound)
	   /* pave' compose' */
	   if (pAb->AbElement->ElTypeNumber == PageBreak + 1)
	      /* c'est une marque de saut de page, non-secable */
	      unbreakable = TRUE;
	   else
	      /* un pave compose' est non-secable s'il est mis en lignes */
	      unbreakable = pAb->AbInLine;
	/* regarde dans le schema de presentation du pave s'il est secable */
	if (!unbreakable)
	  {
	     SearchPresSchema (pAb->AbElement, &pSchP, &index, &pSchS);
	     unbreakable = (pSchP->PsBuildAll[index - 1]);
	  }
	pAb = pAb->AbEnclosing;
	/* passe a l'englobant */
     }
   return (!unbreakable);
}


/*----------------------------------------------------------------------
   KillPresSibling detruit les paves de presentation crees par les   
   regles CreateBefore et CreateAfter de pAb.                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         KillPresSibling (PtrAbstractBox pAbbSibling, ThotBool ElemIsBefore, PtrDocument pDoc, PtrAbstractBox * pAbbR, PtrAbstractBox * pAbbReDisp, int *volsupp, PtrAbstractBox pAb, ThotBool exceptCrWith)
#else  /* __STDC__ */
static void         KillPresSibling (pAbbSibling, ElemIsBefore, pDoc, pAbbR, pAbbReDisp, volsupp, pAb, exceptCrWith)
PtrAbstractBox      pAbbSibling;
ThotBool            ElemIsBefore;
PtrDocument         pDoc;
PtrAbstractBox     *pAbbR;
PtrAbstractBox     *pAbbReDisp;
int                *volsupp;
PtrAbstractBox      pAb;
ThotBool            exceptCrWith;
#endif /* __STDC__ */

{
   ThotBool            stop;

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
static void         KillPresEnclosing (PtrAbstractBox pAb, ThotBool head, PtrDocument pDoc, PtrAbstractBox * pAbbReDisp, int *volsupp, ThotBool exceptCrWith)
#else  /* __STDC__ */
static void         KillPresEnclosing (pAb, head, pDoc, pAbbReDisp, volsupp, exceptCrWith)
PtrAbstractBox      pAb;
ThotBool            head;
PtrDocument         pDoc;
PtrAbstractBox     *pAbbReDisp;
int                *volsupp;
ThotBool            exceptCrWith;
#endif /* __STDC__ */

{
   PtrAbstractBox      pAbbEnclosing, pAbb, pAbbR;
   ThotBool            stop;
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
static void         SupprAbsBoxes (PtrAbstractBox pAbbRoot, PtrDocument pDoc, ThotBool head, int *dvol)
#else  /* __STDC__ */
static void         SupprAbsBoxes (pAbbRoot, pDoc, head, dvol)
PtrAbstractBox      pAbbRoot;
PtrDocument         pDoc;
ThotBool            head;
int                *dvol;
#endif /* __STDC__ */
{
   PtrAbstractBox      pAb, pAbbSibling, pAbbReDisp, pAbbR;
   int                 volsupp, volpres;
   ThotBool            stop, stop1;
   PtrElement          pEl1;

   /* cherche le premier pave englobe' de volume inferieur a dvol et qui */
   /* soit secable */
   pAb = pAbbRoot;
	volpres = 0;
	/* volume des paves de presentation des elem englobants */
	stop = FALSE;
	do
	   if (pAb == NULL)
	      stop = TRUE;
	   else if (pAb->AbEnclosing != NULL &&
		    (pAb->AbVolume + volpres <= *dvol || !IsBreakable (pAb)))
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
	else if (pAb->AbEnclosing == NULL)
	   /* don't destroy the Root box */
	   *dvol = 0;
	else if (pAb->AbVolume + volpres > *dvol)
	   /* on ne peut rien supprimer */
	   *dvol = 0;
	if (*dvol > 0)
	  {
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
	  }			/* fin dvol > 0 */
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
   ThotBool            add, suppress, midHead, stop;
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
	     SupprAbsBoxes (pAbbRoot, pDoc, (ThotBool)(!midHead), &dvol);
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
void                IncreaseVolume (ThotBool head, int dVol, int frame)
#else  /* __STDC__ */
void                IncreaseVolume (head, dVol, frame)
ThotBool            head;
int                 dVol;
int                 frame;
#endif /* __STDC__ */
{
  PtrDocument         pDoc;
  PtrAbstractBox      pAb;
  int                 view, h;
  ThotBool            assoc;

  if (dVol <= 0)
    return;

  GetDocAndView (frame, &pDoc, &view, &assoc);
  /* met a jour la nouvelle capacite de la vue, indique dans le contexte */
  /* du document le volume des paves a creer et cherche le pave racine de */
  /* la vue */
  if (pDoc != NULL)
    {
      if (assoc)
	{
	  /* element associe */
	  pAb = pDoc->DocAssocRoot[view - 1]->ElAbstractBox[0];
	  pDoc->DocAssocVolume[view - 1] = pAb->AbVolume + dVol;
	  pDoc->DocAssocFreeVolume[view - 1] = dVol;
	}
      else
	{
	  /* element de l'arbre principal */
	  pAb = pDoc->DocViewRootAb[view - 1];
	  pDoc->DocViewVolume[view - 1] = pAb->AbVolume + dVol;
	  pDoc->DocViewFreeVolume[view - 1] = dVol;
	}

      if (IsBreakable (pAb))
	{
	  /* cree les paves de la partie qui va apparaitre */
	  AddAbsBoxes (pAb, pDoc, head);
	  
	  /* signale au Mediateur les paves crees et detruits */
	  h = PageHeight;
	  if (assoc)
	    {
	      if (pDoc->DocAssocModifiedAb[view - 1] != NULL)
		{
		  pAb = pDoc->DocAssocModifiedAb[view - 1];
		  pDoc->DocAssocModifiedAb[view - 1] = NULL;
		  (void) ChangeConcreteImage (frame, &h, pAb);
		  FreeDeadAbstractBoxes (pAb, pDoc->DocAssocFrame[view - 1]);
		}
	    }
	  else if (pDoc->DocViewModifiedAb[view - 1] != NULL)
	    {
	      pAb = pDoc->DocViewModifiedAb[view - 1];
	      pDoc->DocViewModifiedAb[view - 1] = NULL;
	      (void) ChangeConcreteImage (frame, &h, pAb);
	      FreeDeadAbstractBoxes (pAb, pDoc->DocViewFrame[view - 1]);
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
void                DecreaseVolume (ThotBool head, int dVol, int frame)
#else  /* __STDC__ */
void                DecreaseVolume (head, dVol, frame)
ThotBool            head;
int                 dVol;
int                 frame;
#endif /* __STDC__ */

{
  PtrDocument         pDoc;
  PtrAbstractBox      pAb;
  int                 view, h;
  ThotBool            assoc;

  if (dVol <= 0)
    return;

  GetDocAndView (frame, &pDoc, &view, &assoc);
  /* met a jour la nouvelle capacite de la vue et cherche le pave racine */
  /* de la vue */
  if (pDoc != NULL)
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

      /* supprime les paves */
      SupprAbsBoxes (pAb, pDoc, head, &dVol);
      /* signale au Mediateur les paves modifies */
      h = PageHeight;
      if (assoc)
	{
	  if (pDoc->DocAssocModifiedAb[view - 1] != NULL)
	    {
	      pAb = pDoc->DocAssocModifiedAb[view - 1];
	      pDoc->DocAssocModifiedAb[view - 1] = NULL;
	      (void) ChangeConcreteImage (frame, &h, pAb);
	      FreeDeadAbstractBoxes (pAb, pDoc->DocAssocFrame[view - 1]);
	    }
	}
      else if (pDoc->DocViewModifiedAb[view - 1] != NULL)
	{
	  pAb = pDoc->DocViewModifiedAb[view - 1];
	  pDoc->DocViewModifiedAb[view - 1] = NULL;
	  (void) ChangeConcreteImage (frame, &h, pAb);
	  FreeDeadAbstractBoxes (pAb, pDoc->DocViewFrame[view - 1]);
	}
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
void                CheckAbsBox (PtrElement pEl, int view, PtrDocument pDoc, ThotBool begin, ThotBool display)
#else  /* __STDC__ */
void                CheckAbsBox (pEl, view, pDoc, begin, display)
PtrElement          pEl;
int                 view;
PtrDocument         pDoc;
ThotBool            begin;
ThotBool            display;

#endif /* __STDC__ */
{
  ThotBool            openedView, creation, stop;
  PtrElement          pElAscent, pElPage;
  PtrElement          pAsc[MaxAsc];
  int                 NumAsc, i, volsupp, frame, nAssoc, boxType, h;
  PtrAbstractBox      pAbbDestroyed, pAbbRemain, pAbbLastEmptyCr, pAbbFirstEmptyCr;
  PtrAbstractBox      pAbbReDisp, pAbbRoot, pPrevious;
  PtrElement          pEl1;
  PtrAbstractBox      pAbbox1;
  ThotBool            complete;
  int                 nR;
	  
  pAbbLastEmptyCr = NULL;
  pAbbRoot = NULL;
  for (i = 0; i < MaxAsc; i++)
    pAsc[i] = NULL;
  if (pEl != NULL && pEl->ElAbstractBox[view - 1] == NULL)
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
      if (pEl1->ElParent == NULL && nAssoc != 0)
	/* c'est un element associe */
	/* nR: type des elements associes */
	{
	  nR = pEl1->ElStructSchema->SsRule[pEl1->ElTypeNumber - 1].SrListItem;
	  /* si les elements associes s'affichent en haut ou en bas de */
	  /* page, la racine n'a jamais de pave */
	  openedView = !pEl1->ElStructSchema->SsPSchema->PsInPageHeaderOrFooter[nR - 1];
	}

      /* si la vue n'est pas creee, il n'y a rien a faire */
      if (openedView)
	{
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
		  {
		    pAsc[NumAsc] = pElAscent;
		    NumAsc++;
		  }
		/* passe a l'ascendant */
		pEl1 = pElAscent;
		if (pEl1->ElStructSchema->SsRule[pEl1->ElTypeNumber - 1].SrAssocElem)
		  /* on vient de traiter un element associe' */
		  /* Serait-ce un element qui s'affiche dans une boite de */
		  /* haut ou de bas de page ? */
		  {
		    pElPage = GetPageBreakForAssoc (pElAscent, pDoc->DocView[view - 1].DvPSchemaView, &boxType);
		    if (pElPage != NULL)
		      /* Il s'affiche dans une haut ou bas de page, c'est la */
		      /* marque de page a laquelle il est associe qu'il faut */
		      /* creer */
		      {
			NumAsc = 1;
			pAsc[0] = pElPage;
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
	  if (openedView && NumAsc > 0)
	    {
	      i = NumAsc;
	      do
		{
		  i--;
		  pEl1 = pAsc[i];
		  /* cree juste le pave, sans sa descendance et sans */
		  /* lui appliquer les regles de presentation. */
		  pPrevious = AbsBoxesCreate (pAsc[i], pDoc, view, TRUE, FALSE, &complete);
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
			pAbbFirstEmptyCr = pPrevious; /* 1er pave cree' */
		    }
		}
	      while (i > 0);
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
			    FreeDeadAbstractBoxes (pAbbRoot, frame);
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
			    pDoc->DocViewModifiedAb[view - 1] = NULL;
			    pAbbRoot->AbDead = TRUE;
			    ChangeConcreteImage (frame, &h, pAbbRoot);
			    pAbbRoot->AbDead = FALSE;
			    FreeDeadAbstractBoxes (pAbbRoot, frame);
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
		      if (pAbbox1->AbLeafType == LtCompound
			  && !pAbbox1->AbInLine)
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
		    /* il n'y a plus rien a reafficher dans cette vue */
		    if (nAssoc > 0)
		      pDoc->DocAssocModifiedAb[nAssoc - 1] = NULL;
		    else
		      pDoc->DocViewModifiedAb[view - 1] = NULL;
		    ChangeConcreteImage (frame, &h, pAbbRoot);
		    if (display)
		      DisplayFrame (frame);
		  }
	      }
	  }
      }
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
  ThotBool            isAssoc;
  PtrElement          pEl;
  int                 volBefore, volPresent;
  ThotBool            after;
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
	      if (pAb != NULL && ThotLocalActions[T_showbox] != NULL)
		(*ThotLocalActions[T_showbox]) (frame, pAb->AbBox, position, 0);
	      
	      /* Allume la selection */
	      HighlightSelection (FALSE, TRUE);
	    }
	}
    }
}

