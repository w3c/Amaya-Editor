
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */


/*
   Module de manipulations des images abstraites.
   V. Quint     Mai 1985
   imabs.c -- Gestion des images abstraites IA.

   IV : Septembre 92 adaptation Tool Kit
 */


#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"

#define EXPORT extern
#include "page.var"

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

static char         texte[MAX_TXT_LEN];

#ifdef __STDC__
extern void         DisplayFrame (int);

#else  /* __STDC__ */
extern void         DisplayFrame ();

#endif /* __STDC__ */

/* ---------------------------------------------------------------------- */
/* |    AbsBoxType  rend un pointeur sur un buffer qui contient           | */
/* |            le type de l'element de structure auquel correspond le  | */
/* |            pave pointe' par pAb.                                  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
char               *AbsBoxType (PtrAbstractBox pAb)

#else  /* __STDC__ */
char               *AbsBoxType (pAb)
PtrAbstractBox             pAb;

#endif /* __STDC__ */

{
   SRule              *pRe1;
   PresentationBox             *pBo1;
   PtrElement          pEl1;

   if (pAb == NULL)
      strcpy (texte, " ");
   else
     {
	if (pAb->AbPresentationBox)
	  {
	     pRe1 = &pAb->AbElement->ElSructSchema->SsRule[pAb->AbElement->ElTypeNumber - 1];
	     /* copie le nom du type d'element structure qui a cree la boite */
	     strcpy (texte, pRe1->SrName);
	     strcat (texte, ".");
	     /* copie a la suite le nom du type de boite de presentation */
	     pBo1 = &pAb->AbPSchema->PsPresentBox[pAb->AbTypeNum - 1];
	     strcat (texte, pBo1->PbName);
	  }
	else
	   /* pave d'un element de structure */
	  {
	     pEl1 = pAb->AbElement;
	     strcpy (texte, pEl1->ElSructSchema->SsRule[pEl1->ElTypeNumber - 1].SrName);
	  }
     }
   return (texte);
}



/* ---------------------------------------------------------------------- */
/* |    LibAbbView libere, pour une seule vue, tous les paves            | */
/* |            englobes par le pave pointe par pAb, lui-meme compris. | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                LibAbbView (PtrAbstractBox pAb)

#else  /* __STDC__ */
void                LibAbbView (pAb)
PtrAbstractBox             pAb;

#endif /* __STDC__ */

{
   PtrAbstractBox             pAbb, pAbbNext;
   PtrTextBuffer      pBT, pBTSuiv;
   PtrDelayedPRule     pDelPR, pNextDelPR;
   PtrAbstractBox             pAbbox1;
   PtrElement          pEl1;
   boolean             libpav;

#ifdef __COLPAGE__
   boolean             ok;

#else  /* __COLPAGE__ */
   PresentationBox             *pBox;
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
	pAbbox1 = pAb;
	if (pAbbox1->AbBox != NULL)
	  {
	     pEl1 = pAbbox1->AbElement;
	     printf ("Box non liberee: %s", pEl1->ElSructSchema->SsRule[pEl1->ElTypeNumber - 1].SrName);
	     if (pAbbox1->AbPresentationBox)
		printf (".%s\n", pAbbox1->AbPSchema->PsPresentBox[pAbbox1->AbTypeNum - 1].PbName);
	     else
		printf ("\n");
	  }
#ifdef __COLPAGE__
	/* debug */
	ok = AbsBoxOk (pAb);
	if (!ok)
#endif /* __COLPAGE__ */
	   /* dechaine pAb des autres paves */
	   if (pAbbox1->AbNext != NULL)
	      pAbbox1->AbNext->AbPrevious = pAbbox1->AbPrevious;
	if (pAbbox1->AbPrevious != NULL)
	   pAbbox1->AbPrevious->AbNext = pAbbox1->AbNext;
	if (pAbbox1->AbEnclosing != NULL)
	   if (pAbbox1->AbEnclosing->AbFirstEnclosed == pAb)
	      pAbbox1->AbEnclosing->AbFirstEnclosed = pAbbox1->AbNext;
#ifdef __COLPAGE__
	if (pAbbox1->AbNextRepeated != NULL)
	   pAbbox1->AbNextRepeated->AbPreviousRepeated =
	      pAbbox1->AbPreviousRepeated;
	if (pAbbox1->AbPreviousRepeated != NULL)
	   pAbbox1->AbPreviousRepeated->AbNextRepeated =
	      pAbbox1->AbNextRepeated;
	/* debug */
	ok = AbsBoxOk (pAb);
	if (!ok)
	   printf ("erreur apres dechainage LibAbbView \n");
#endif /* __COLPAGE__ */
	/* Si c'est un pave obtenu par la regle de presentation Copy,
	   libere le descripteur d'element copie' */
	if (pAbbox1->AbCopyDescr != NULL)
	  {
	     if (pAbbox1->AbCopyDescr->CdPrevious == NULL)
		pAbbox1->AbCopyDescr->CdCopiedElem->ElCopyDescr =
		   pAbbox1->AbCopyDescr->CdNext;
	     else
		pAbbox1->AbCopyDescr->CdPrevious->CdNext =
		   pAbbox1->AbCopyDescr->CdNext;
	     if (pAbbox1->AbCopyDescr->CdNext != NULL)
		pAbbox1->AbCopyDescr->CdNext->CdPrevious =
		   pAbbox1->AbCopyDescr->CdPrevious;
	     FreeDescCopie (pAbbox1->AbCopyDescr);
	  }
	/* si c'est un pave de presentation ou le pave d'une reference ou */
	/* celui d'une marque de paire, on libere les buffers */
	libpav = FALSE;
	if (pAbbox1->AbPresentationBox)
	   if (pAbbox1->AbLeafType == LtText || pAbbox1->AbLeafType == LtPlyLine ||
	       pAbbox1->AbLeafType == LtPicture)
	      libpav = TRUE;
	if (!libpav)
	   if (!pAbbox1->AbPresentationBox)
	      if (pAbbox1->AbElement != NULL)
		 if (pAbbox1->AbElement->ElTerminal)
		    if (pAbbox1->AbElement->ElLeafType == LtReference ||
			pAbbox1->AbElement->ElLeafType == LtPairedElem)
		       libpav = TRUE;
	if (libpav)
	   switch (pAbbox1->AbLeafType)
		 {
		    case LtText:
		    case LtPlyLine:
		       if (pAbbox1->AbLeafType == LtPlyLine)
			  pBT = pAbbox1->AbPolyLineBuffer;
		       else
			  pBT = pAbbox1->AbText;
		       while (pBT != NULL)
			 {
			    pBTSuiv = pBT->BuNext;
			    DeleteTextBuffer (&pBT);
			    pBT = pBTSuiv;
			 }
		       break;
		    case LtPicture:
		       if (!pAbbox1->AbElement->ElTerminal || pAbbox1->AbElement->ElLeafType != LtPicture)
			 {
			    /* ce n'est pas un element image */
			    FreeImageDescriptor (pAbbox1->AbPictInfo);
			    pAbbox1->AbPictInfo = NULL;
			 }
		       break;
		    default:
		       break;
		 }
	/* dechaine pAb de son element */
	if (pAbbox1->AbElement->ElAbstractBox[pAbbox1->AbDocView - 1] == pAb)
#ifdef __COLPAGE__
	   if (!pAbbox1->AbPresentationBox)
	      /* si le pave pAb est le pave principal (non presentation) */
	      /* l'elt va pointer sur le dup (s'il existe) */
	      /* On ne considere pas les paves de pres rep suivants TODO ? */
	      pAbbox1->AbElement->ElAbstractBox[pAbbox1->AbDocView - 1] = pAbbox1->AbNextRepeated;
	   else
#endif /* __COLPAGE__ */
	   if (pAbbox1->AbNext != NULL)
	      if (pAbbox1->AbNext->AbElement == pAbbox1->AbElement)
		 pAbbox1->AbElement->ElAbstractBox[pAbbox1->AbDocView - 1] = pAbbox1->AbNext;
	      else
		 pAbbox1->AbElement->ElAbstractBox[pAbbox1->AbDocView - 1] = NULL;
	   else
#ifdef __COLPAGE__
	      /* inutile ? car un pave de pres ne peut avoir de dup */
	      pAbbox1->AbElement->ElAbstractBox[pAbbox1->AbDocView - 1] = pAbbox1->AbNextRepeated;
	/* fin du cas ou l'element pointait sur le pave */
#else  /* __COLPAGE__ */
	      pAbbox1->AbElement->ElAbstractBox[pAbbox1->AbDocView - 1] = NULL;
#endif /* __COLPAGE__ */
#ifndef __COLPAGE__
/*** debut ajout ***/
	/* Ce code ne marche pas avec CP */
	/* il est inutile car le pave de la racine des elements associes */
	/* (places en haut ou bas de page) n'est plus considere comme un */
	/* pave de presentation : son dechainage de l'element se fait */
	/* comme tout pave d'element */
	/* (il y a un niveau de pave en plus) */
	else
	   /* est-ce une boite de haut ou bas de page contenant des */
	   /* elements associes ? */
	if (pAbbox1->AbPresentationBox && pAbbox1->AbElement->ElTypeNumber == PageBreak + 1)
	  {
	     pBox = &pAbbox1->AbPSchema->PsPresentBox[pAbbox1->AbTypeNum - 1];
	     if ((pBox->PbPageHeader || pBox->PbPageFooter) &&
		 pBox->PbContent == ContElement)
		/* c'est bien une boite de haut ou bas de page contenant des */
		/* elements associes. Le pave est-il associe' a l'un des */
		/* elements associes du document ? */
	       {
		  pDoc = DocumentOfElement (pAbbox1->AbElement);
		  if (pDoc != NULL)
		    {
		       for (assoc = 0; assoc < MAX_ASSOC_DOC; assoc++)
			  if (pDoc->DocAssocRoot[assoc] != NULL)
			     if (pDoc->DocAssocRoot[assoc]->ElAbstractBox[pAbbox1->AbDocView - 1] == pAbbox1)
				/* cet element n'a plus de pave */
				pDoc->DocAssocRoot[assoc]->ElAbstractBox[pAbbox1->AbDocView - 1] = NULL;
		    }
	       }
	  }
	else
	   /* est-ce un pave de presentation cree' par une regle FnCreateEnclosing */
	if (pAbbox1->AbPresentationBox)
	   if (pAbbox1->AbElement->ElAbstractBox[pAbbox1->AbDocView - 1] != NULL)
	      if (pAbbox1->AbElement->ElAbstractBox[pAbbox1->AbDocView - 1]->AbEnclosing == pAbbox1)
		 pAbbox1->AbElement->ElAbstractBox[pAbbox1->AbDocView - 1] = NULL;
/*** fin ajout ***/
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
		  FreeRRetard (pDelPR);
		  pDelPR = pNextDelPR;
	       }
	  }
	FreePave (pAb);
     }
}


/* ---------------------------------------------------------------------- */
/* |    LibAbbEl libere, dans toutes les vues, tous les paves de      | */
/* |            l'element pointe par pEl.                               | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                LibAbbEl (PtrElement pEl)

#else  /* __STDC__ */
void                LibAbbEl (pEl)
PtrElement          pEl;

#endif /* __STDC__ */

#ifdef __COLPAGE__
{
   int                 v;
   PtrAbstractBox             pAbb, pAbbNext;
   PtrAbstractBox             pDupSuiv;
   boolean             stop;
   boolean             stop1;

   /* ce code ne traite que le cas du pave de l'element et de */
   /* paves de presentation avant et apres */
   /* prevoir cas des paves de presentation de la racine ?? TODO */
   if (pEl != NULL)
      for (v = 1; v <= MAX_VIEW_DOC; v++)
	{
	   pAbb = pEl->ElAbstractBox[v - 1];
	   if (pAbb != NULL)
	      if (pAbb->AbEnclosing != NULL)
		 if (pAbb->AbEnclosing->AbPresentationBox &&
		     pAbb->AbEnclosing->AbElement == pEl)
		    /* le pave englobant est un pave' de presentation cree' par */
		    /* une regle FnCreateEnclosing */
		    pAbb = pAbb->AbEnclosing;
	   stop1 = FALSE;
	   pDupSuiv = NULL;
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
			pDupSuiv = pAbb->AbNextRepeated;
			LibAbbView (pAbb);
			pAbb = pAbbNext;
		     }
		while (!(stop));
		if (pDupSuiv != NULL)	/* on parcourt la liste des dupliques */
		   pAbb = pDupSuiv;
		else
		   stop1 = TRUE;
	     }
	   while (!stop1);
	}
}

#else  /* __COLPAGE__ */

{
   int                 v;
   PtrAbstractBox             pAbb, pAbbNext;
   boolean             stop;

   if (pEl != NULL)
      for (v = 1; v <= MAX_VIEW_DOC; v++)
	{
	   pAbb = pEl->ElAbstractBox[v - 1];
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
	   while (!(stop));
	}
}

#endif /* __COLPAGE__ */


/* ---------------------------------------------------------------------- */
/* |    FreeDeadAbstractBoxes libere tous les paves marques Mort dans le           | */
/* |            sous-arbre de racine pAb.                              | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                FreeDeadAbstractBoxes (PtrAbstractBox pAb)

#else  /* __STDC__ */
void                FreeDeadAbstractBoxes (pAb)
PtrAbstractBox             pAb;

#endif /* __STDC__ */

{
   PtrAbstractBox             pAbb, pAbbNext;


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
 /* ---------------------------------------------------------- */
 /* |     RecursEvalCP appelle NouvRfPave pour tous les paves | */
 /* |                du sous-arbre pAb                     | */
 /* ---------------------------------------------------------- */


#ifdef __STDC__
void                RecursEvalCP (PtrAbstractBox pAb, PtrDocument pDoc)

#else  /* __STDC__ */
void                RecursEvalCP (pAb, pDoc)
PtrAbstractBox             pAb;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrAbstractBox             pAbb, PavR;

   pAbb = pAb;
   if (pAbb != NULL)
     {
	NouvRfPave (pAbb, pAbb, &PavR, pDoc);
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


/* ---------------------------------------------------------------------- */
/* |    AddAbsBoxes complete la vue dont pAbbRoot est le pave racine   | */
/* |            en ajoutant des paves, en tete si Tete est vrai,        | */
/* |            en queue sinon.                                         | */
 /*        pAbbRoot est une vraie racine de paves               | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                AddAbsBoxes (PtrAbstractBox pAbbRoot, PtrDocument pDoc, boolean Tete)

#else  /* __STDC__ */
void                AddAbsBoxes (pAbbRoot, pDoc, Tete)
PtrAbstractBox             pAbbRoot;
PtrDocument         pDoc;
boolean             Tete;

#endif /* __STDC__ */

#ifdef __COLPAGE__
{
   PtrAbstractBox             pAb;
   PtrAbstractBox             pAbbox1;
   PtrElement          pEl;
   boolean             complet;

   if ((Tete && pAbbRoot->AbTruncatedHead) || (!Tete && pAbbRoot->AbTruncatedTail))
     {
	/* cree les paves de la partie coupee jusqu'a concurrence du volume libre */
	pEl = pAbbRoot->AbElement;
	pAbbox1 = pAbbRoot;
	pAb = CreePaves (pEl, pDoc, pAbbox1->AbDocView, !Tete, TRUE, &complet);
	/* on reapplique les regles  a tous les paves */
	/* TO DO a affiner ! */ RecursEvalCP (pAbbRoot, pDoc);
	if (VueAssoc (pEl))
	   pDoc->DocAssocModifiedAb[pEl->ElAssocNum - 1] = pAbbRoot;
	else
	   pDoc->DocViewModifiedAb[pAbbRoot->AbDocView - 1] = pAbbRoot;
     }
}

#else  /* __COLPAGE__ */

{
   PtrAbstractBox             pAb, PavReaff, PavR, PcFirst, PcLast, pAbb;
   boolean             stop;
   PtrAbstractBox             pAbbox1;
   PtrDocument         pDo1;
   PtrElement          pEl1;
   boolean             complet;

   if ((Tete && pAbbRoot->AbTruncatedHead) || (!Tete && pAbbRoot->AbTruncatedTail))
     {
	/* cree les paves de la partie coupee jusqu'a concurence du volume libre */
	pAbbox1 = pAbbRoot;
	pAb = CreePaves (pAbbox1->AbElement, pDoc, pAbbox1->AbDocView, !Tete, TRUE, &complet);
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
	     while (!(stop));
	     stop = FALSE;
	     do
		if (PcFirst->AbPrevious == NULL)
		   stop = TRUE;
		else if (!PcFirst->AbPrevious->AbNew)
		   stop = TRUE;
		else
		   PcFirst = PcFirst->AbPrevious;
	     while (!(stop));
	     if (PcFirst == PcLast)
		/* un seul pave cree a ce niveau, c'est lui qu'il faudra */
		/* reafficher. */
		PavReaff = pAb;
	     else
		/* plusieurs paves crees, on reaffichera l'englobant */
		PavReaff = pAb->AbEnclosing;
	     /* modifie les paves environnant les paves crees */
	     NouvRfPave (PcFirst, PcLast, &PavR, pDoc);
	     PavReaff = Englobant (PavReaff, PavR);	/* conserve le pointeur sur le pave a reafficher */

	     pDo1 = pDoc;
	     pEl1 = pAbbRoot->AbElement;
	     if (VueAssoc (pAbbRoot->AbElement))
		pDo1->DocAssocModifiedAb[pEl1->ElAssocNum - 1] =
		   Englobant (PavReaff, pDo1->DocAssocModifiedAb[pEl1->ElAssocNum - 1]);
	     else
		pDo1->DocViewModifiedAb[pAbbRoot->AbDocView - 1] =
		   Englobant (PavReaff, pDo1->DocViewModifiedAb[pAbbRoot->AbDocView - 1]);
	     /* passe au niveau inferieur */
	     if (Tete)
		pAb = PcLast->AbNext;
	     else
		pAb = PcFirst->AbPrevious;
	     if (pAb != NULL)
	       {
		  /* saute les paves de presentation produits par CreateWith */
		  if (Tete)
		     while (pAb->AbPresentationBox && pAb->AbNext != NULL)
			pAb = pAb->AbNext;
		  else
		     while (pAb->AbPresentationBox && pAb->AbPrevious != NULL)
			pAb = pAb->AbPrevious;
		  /* passe au premier pave fils */
		  pAb = pAb->AbFirstEnclosed;
		  if (pAb != NULL)
		     if (Tete)
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
			     while (!(stop));
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
			     while (!(stop));
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
			       while (!(stop));
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

#endif /* __COLPAGE__ */


/* ---------------------------------------------------------------------- */
/* |    NonSecable retourne vrai si le pave pointe par pAb est         | */
/* |            englobe (a n'importe quel niveau) par un pave           | */
/* |            mis en ligne ou explicitement non secable. Les marques  | */
/* |            de page son traitees comme non-secables.                | */
/* |**CP*       V4 : Les paves de page sont traites comme secables.     | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
boolean             NonSecable (PtrAbstractBox pAb)

#else  /* __STDC__ */
boolean             NonSecable (pAb)
PtrAbstractBox             pAb;

#endif /* __STDC__ */

{
   boolean             retour;
   int                 Entree;
   PtrPSchema          pSchP;
   PtrSSchema        pSchS;
   PtrAbstractBox             pAbbox1;

   retour = FALSE;
   /* a priori le pave est secable */
   /* boucle sur les paves englobants */
   while (pAb != NULL && !retour)
     {
	pAbbox1 = pAb;
#ifndef __COLPAGE__
	if (pAbbox1->AbLeafType == LtCompound)
	   /* pave' compose' */
	   if (pAbbox1->AbElement->ElTypeNumber == PageBreak + 1)
	      /* c'est une marque de saut de page, non-secable */
	      retour = TRUE;
	   else
	      /* un pave compose' est non-secable s'il est mis en lignes */
	      retour = pAbbox1->AbInLine;
	/* regarde dans le schema de presentation du pave s'il est secable */
	if (!retour)
#endif /* __COLPAGE__ */
	  {
	     ChSchemaPres (pAbbox1->AbElement, &pSchP, &Entree, &pSchS);
	     retour = pSchP->PsBuildAll[Entree - 1];
	  }
	pAb = pAbbox1->AbEnclosing;
	/* passe a l'englobant */
     }
   return retour;
}

#ifdef __COLPAGE__

/*------------------------------------------------------------------------------*/
 /*KillPresRight tue tous les paves de presentation a droite de pAb     */
 /*              on ne considere que les paves a l'interieur d'une page */
 /*             sauf les REPEATED                                      */
/*------------------------------------------------------------------------------*/



#ifdef __STDC__
void                KillPresRight (PtrAbstractBox pAb, PtrDocument pDoc)

#else  /* __STDC__ */
void                KillPresRight (pAb, pDoc)
PtrAbstractBox             pAb;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrAbstractBox             PavEnglob, pAbb, PavR;
   boolean             stop;

   if (pAb->AbElement->ElTypeNumber != PageBreak + 1)
     {
	PavEnglob = pAb->AbEnclosing;
	while (PavEnglob != NULL && !(PavEnglob->AbTruncatedTail))
	  {
	     /* on saute le pave corps de page, pour ne pas detruire ses paves */
	     /* de presentation */
	     if (!(PavEnglob->AbElement->ElTerminal
		   && PavEnglob->AbElement->ElLeafType == LtPageColBreak))
	       {
		  PavEnglob->AbTruncatedTail = TRUE;
		  /* cherche et supprime les paves crees par CreateLast */
		  /* si PavEnglob est la racine, ses paves de presentation */
		  /* sont sous le dernier pave corps de page */
		  if (PavEnglob == pDoc->DocViewRootAb[pAb->AbDocView - 1])
		    {
		       pAbb = PavEnglob->AbFirstEnclosed;
		       if (pAbb != NULL && pAbb->AbElement->ElTerminal
			   && pAbb->AbElement->ElLeafType == LtPageColBreak)
			  /* le document est mis en page */
			 {
			    /* on se place sur le dernier pave */
			    while (pAbb->AbNext != NULL)
			       pAbb = pAbb->AbNext;
			    while (pAbb->AbPresentationBox)
			       pAbb = pAbb->AbPrevious;	/* on saut le bas de page */
			    /* et le filet */
			    pAbb = pAbb->AbFirstEnclosed;	/* 1er element du corps de page */
			    /* TODO descendre les paves de colonne */
			 }
		    }
		  else
		     pAbb = PavEnglob->AbFirstEnclosed;
		  /* cherche d'abord le dernier pave englobe */
		  stop = FALSE;
		  if (pAbb != NULL)
		     do
			if (pAbb->AbNext == NULL)
			   stop = TRUE;
			else
			   pAbb = pAbb->AbNext;
		     while (!(stop));
		  stop = FALSE;
		  do
		     if (pAbb == NULL)
			stop = TRUE;
		     else if (!pAbb->AbPresentationBox || pAbb->AbDead
			      || pAbb->AbElement != PavEnglob->AbElement)
			stop = TRUE;
		     else
		       {
			  if (!pAbb->AbRepeatedPresBox)
			    {
			       TuePave (pAbb);
			       /* traite les paves qui se referent au pave detruit */
			       SuppRfPave (pAbb, &PavR, pDoc);
			    }
			  pAbb = pAbb->AbPrevious;
		       }
		  while (!(stop));
		  /* cherche et supprime les paves crees par CreateAfter */
		  pAbb = PavEnglob->AbNext;
		  stop = FALSE;
		  do
		     if (pAbb == NULL)
			stop = TRUE;
		     else if (!pAbb->AbPresentationBox
			      || pAbb->AbDead
			      || pAbb->AbElement != PavEnglob->AbElement)
			stop = TRUE;
		     else
		       {
			  if (!pAbb->AbRepeatedPresBox)
			    {
			       TuePave (pAbb);
			       /* traite les paves qui se referent au pave detruit */
			       SuppRfPave (pAbb, &PavR, pDoc);
			    }
			  pAbb = pAbb->AbNext;
		       }
		  while (!(stop));
	       }
	     /* passe au niveau superieur */
	     PavEnglob = PavEnglob->AbEnclosing;
	  }
     }
}				/* fin KillPresRight */
#endif /* __COLPAGE__ */

#ifdef __COLPAGE__

 /* -------------------------------------------------------- */
 /* |  DestrAbbNext detruit les paves a partir         | */
 /* |               de pAb                               | */
 /* -------------------------------------------------------- */


#ifdef __STDC__
void                DestrAbbNext (PtrAbstractBox pAb, PtrDocument pDoc)

#else  /* __STDC__ */
void                DestrAbbNext (pAb, pDoc)
PtrAbstractBox             pAb;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrAbstractBox             PavRac, pAbb2, pAbb1, PavR;

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
	TuePave (pAbb1);
	SuppRfPave (pAbb1, &PavR, pDoc);
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
		  TuePave (pAbb2);
		  SuppRfPave (pAbb2, &PavR, pDoc);
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
	     TuePave (pAbb1);
	     SuppRfPave (pAbb1, &PavR, pDoc);
	     pAbb1 = pAbb1->AbNext;
	  }
     }
   if (PavRac != NULL)
      /* on marque la racine coupee en queue */
      PavRac->AbTruncatedTail = TRUE;
}				/* fin DestrAbbNext */
#endif /* __COLPAGE__ */


#ifdef __COLPAGE__
/* ---------------------------------------------------------------------- */
/* |    Procedure differente dans V4 : booleen SaufRep a la place de    | */
/* |    SaufCreeAvec                                                    | */
/* ---------------------------------------------------------------------- */
/* |    KillPresSibling detruit les paves de presentation crees par les   | */
/* |    regles CreateBefore et CreateAfter de pAb.                     | */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
void                KillPresSibling (PtrAbstractBox pVoisin, boolean ElemIsBefore, PtrDocument pDoc, PtrAbstractBox * PavR, PtrAbstractBox * PavReaff, int *volsupp, PtrAbstractBox pAb, boolean SaufRep)

#else  /* __STDC__ */
void                KillPresSibling (pVoisin, ElemIsBefore, pDoc, PavR, PavReaff, volsupp, pAb, SaufRep)
PtrAbstractBox             pVoisin;
boolean             ElemIsBefore;
PtrDocument         pDoc;
PtrAbstractBox            *PavR;
PtrAbstractBox            *PavReaff;
int                *volsupp;
PtrAbstractBox             pAb;
boolean             SaufRep;

#endif /* __STDC__ */

{
   boolean             stop;

   stop = FALSE;
   do
      if (pVoisin == NULL)
	 stop = TRUE;
      else if (pVoisin->AbElement != pAb->AbElement)
	 stop = TRUE;
      else
	{
	   if (pVoisin->AbPresentationBox)
	      if (!(SaufRep && pVoisin->AbRepeatedPresBox))
		{
		   *volsupp += pVoisin->AbVolume;
		   TuePave (pVoisin);
		   *PavReaff = Englobant (*PavReaff, pVoisin);
		   /* traite les paves qui se referent au pave detruit */
		   SuppRfPave (pVoisin, PavR, pDoc);
		   *PavReaff = Englobant (*PavReaff, *PavR);
		}
	   if (ElemIsBefore)
	      pVoisin = pVoisin->AbPrevious;
	   else
	      pVoisin = pVoisin->AbNext;
	}
   while (!(stop));
}

#else  /* __COLPAGE__ */

/* ---------------------------------------------------------------------- */
/* |    KillPresSibling detruit les paves de presentation crees par les   | */
/* |    regles CreateBefore et CreateAfter de pAb.                     | */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
static void         KillPresSibling (PtrAbstractBox pVoisin, boolean ElemIsBefore, PtrDocument pDoc, PtrAbstractBox * PavR, PtrAbstractBox * PavReaff, int *volsupp, PtrAbstractBox pAb, boolean SaufCreeAvec)

#else  /* __STDC__ */
static void         KillPresSibling (pVoisin, ElemIsBefore, pDoc, PavR, PavReaff, volsupp, pAb, SaufCreeAvec)
PtrAbstractBox             pVoisin;
boolean             ElemIsBefore;
PtrDocument         pDoc;
PtrAbstractBox            *PavR;
PtrAbstractBox            *PavReaff;
int                *volsupp;
PtrAbstractBox             pAb;
boolean             SaufCreeAvec;

#endif /* __STDC__ */

{
   boolean             stop;

   stop = FALSE;
   do
      if (pVoisin == NULL)
	 stop = TRUE;
      else if (pVoisin->AbElement != pAb->AbElement)
	 stop = TRUE;
      else
	{
	   if (pVoisin->AbPresentationBox)
	      if (!SaufCreeAvec || RegleCree (pDoc, pAb, pVoisin) != FnCreateWith)
		{
		   *volsupp += pVoisin->AbVolume;
		   TuePave (pVoisin);
		   *PavReaff = Englobant (*PavReaff, pVoisin);
		   /* traite les paves qui se referent au pave detruit */
		   SuppRfPave (pVoisin, PavR, pDoc);
		   *PavReaff = Englobant (*PavReaff, *PavR);
		}
	   if (ElemIsBefore)
	      pVoisin = pVoisin->AbPrevious;
	   else
	      pVoisin = pVoisin->AbNext;
	}
   while (!(stop));
}
#endif /* __COLPAGE__ */

#ifdef __COLPAGE__
/* ---------------------------------------------------------------------- */
/* |    Procedure differente dans V4 : booleen SaufRep a la place de    | */
/* |    SaufCreeAvec                                                    | */
/* ---------------------------------------------------------------------- */
/* |    KillPresEnclosing supprime tous les paves de presentation           | */
/* |            crees par pAb et les paves de presentation crees par   | */
/* |            les paves englobants a l'aide de regles CreateFirst et  | */
/* |            CreateBefore (si Tete est vrai) ou CreateAfter et       | */
/* |            CreateLast (si Tete est faux).                          | */
/* |            Au retour volsupp indique le volume des paves de        | */
/* |            presentation tues et PavReaff le pave a reafficher.     | */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
static void         KillPresEnclosing (PtrAbstractBox pAb, boolean Tete, PtrDocument pDoc, PtrAbstractBox * PavReaff, int *volsupp, boolean SaufRep)

#else  /* __STDC__ */
static void         KillPresEnclosing (pAb, Tete, pDoc, PavReaff, volsupp, SaufRep)
PtrAbstractBox             pAb;
boolean             Tete;
PtrDocument         pDoc;
PtrAbstractBox            *PavReaff;
int                *volsupp;
boolean             SaufRep;

#endif /* __STDC__ */

{
   PtrAbstractBox             PavEnglob, pAbb, PavR;
   boolean             stop;

   *volsupp = 0;
   /* Detruit les paves de presentation crees par les regles */
   /* CreateBefore et CreateAfter de pAb. */
   KillPresSibling (pAb->AbPrevious, TRUE,
		  pDoc, &PavR, PavReaff, volsupp, pAb, SaufRep);
   KillPresSibling (pAb->AbNext, FALSE,
		  pDoc, &PavR, PavReaff, volsupp, pAb, SaufRep);
   /* traite les paves englobants */
   PavEnglob = pAb->AbEnclosing;
   while (PavEnglob != NULL)
      /* le test sur AbTruncatedHead ou AbTruncatedTail n'est plus significatif */
      /* a cause des paves de presentation repetes */
      /*    if (Tete && PavEnglob->AbTruncatedHead  */
      /*     || !Tete && PavEnglob->AbTruncatedTail) */
      /* pave deja traite', on s'arrete */
      /*      PavEnglob = NULL; */
      /*    else */
     {
	/* on saute le pave corps de page, pour ne pas detruire ses paves de pres */
	if (!(PavEnglob->AbElement->ElTerminal
	      && PavEnglob->AbElement->ElLeafType == LtPageColBreak))
	   if (!NonSecable (PavEnglob))
	      if (Tete)
		{
		   PavEnglob->AbTruncatedHead = TRUE;
		   /* si PavEnglob est la racine, ses paves de presentation */
		   /* sont sous le pave corps de page ou de colonne */
		   if (PavEnglob->AbEnclosing == NULL)
		     {
			pAbb = PavEnglob->AbFirstEnclosed;
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
		      pAbb = PavEnglob->AbFirstEnclosed;
		   /* cherche et supprime les paves crees par CreateFirst */
		   /* mais ne supprime pas ceux qui sont repetes */
		   stop = FALSE;
		   do
		      if (pAbb == NULL)
			 stop = TRUE;
		      else if (!pAbb->AbPresentationBox || pAbb->AbDead
			       || pAbb->AbElement != PavEnglob->AbElement)
			 /* suppression du saut des marques de page de debut d'element */
			 stop = TRUE;
		      else
			{
			   if (!pAbb->AbRepeatedPresBox)
			     {
				*volsupp += pAbb->AbVolume;
				TuePave (pAbb);
				*PavReaff = Englobant (*PavReaff, pAbb);
				/* traite les paves qui se referent au pave detruit */
				SuppRfPave (pAbb, &PavR, pDoc);
				*PavReaff = Englobant (*PavReaff, PavR);
			     }
			   pAbb = pAbb->AbNext;
			}
		   while (!(stop));
		   /* cherche et supprime les paves crees par CreateBefore */
		   /* mais ne supprime pas ceux qui sont repetes */
		   pAbb = PavEnglob->AbPrevious;
		   stop = FALSE;
		   do
		      if (pAbb == NULL)
			 stop = TRUE;
		      else if (!pAbb->AbPresentationBox || pAbb->AbDead
			       || pAbb->AbElement != PavEnglob->AbElement)
			 stop = TRUE;
		      else
			{
			   if (!pAbb->AbRepeatedPresBox)
			     {
				*volsupp += pAbb->AbVolume;
				TuePave (pAbb);
				*PavReaff = Englobant (*PavReaff, pAbb);
				/* traite les paves qui se referent au pave detruit */
				SuppRfPave (pAbb, &PavR, pDoc);
				*PavReaff = Englobant (*PavReaff, PavR);
			     }
			   pAbb = pAbb->AbPrevious;
			}
		   while (!(stop));
		}
	      else
		{
		   /* Tete = FALSE */
		   PavEnglob->AbTruncatedTail = TRUE;
		   /* cherche et supprime les paves crees par CreateLast */
		   /* mais ne supprime pas ceux qui sont repetes */
		   /* si PavEnglob est la racine, ses paves de presentation */
		   /* sont sous le dernier pave corps de page */
		   if (PavEnglob->AbEnclosing == NULL)
		     {
			pAbb = PavEnglob->AbFirstEnclosed;
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
		      pAbb = PavEnglob->AbFirstEnclosed;
		   /* cherche d'abord le dernier pave englobe */
		   stop = FALSE;
		   if (pAbb != NULL)
		      do
			 if (pAbb->AbNext == NULL)
			    stop = TRUE;
			 else
			    pAbb = pAbb->AbNext;
		      while (!(stop));
		   stop = FALSE;
		   do
		      if (pAbb == NULL)
			 stop = TRUE;
		      else if (!pAbb->AbPresentationBox || pAbb->AbDead
			       || pAbb->AbElement != PavEnglob->AbElement)
			 stop = TRUE;
		      else
			{
			   if (!pAbb->AbRepeatedPresBox)
			     {
				*volsupp += pAbb->AbVolume;
				TuePave (pAbb);
				*PavReaff = Englobant (*PavReaff, pAbb);
				/* traite les paves qui se referent au pave detruit */
				SuppRfPave (pAbb, &PavR, pDoc);
				*PavReaff = Englobant (*PavReaff, PavR);
			     }
			   pAbb = pAbb->AbPrevious;
			}
		   while (!(stop));
		   /* cherche et supprime les paves crees par CreateAfter */
		   /* mais ne supprime pas ceux crees par CreateWith */
		   pAbb = PavEnglob->AbNext;
		   stop = FALSE;
		   do
		      if (pAbb == NULL)
			 stop = TRUE;
		      else if (!pAbb->AbPresentationBox
			       || pAbb->AbDead
			       || pAbb->AbElement != PavEnglob->AbElement)
			 stop = TRUE;
		      else
			{
			   if (!pAbb->AbRepeatedPresBox)
			     {
				*volsupp += pAbb->AbVolume;
				TuePave (pAbb);
				*PavReaff = Englobant (*PavReaff, pAbb);
				/* traite les paves qui se referent au pave detruit */
				SuppRfPave (pAbb, &PavR, pDoc);
				*PavReaff = Englobant (*PavReaff, PavR);
			     }
			   pAbb = pAbb->AbNext;
			}
		   while (!(stop));
		}
	/* passe au niveau superieur */
	PavEnglob = PavEnglob->AbEnclosing;
     }
}

#else  /* __COLPAGE__ */

/* ---------------------------------------------------------------------- */
/* |    KillPresEnclosing supprime tous les paves de presentation           | */
/* |            crees par pAb et les paves de presentation crees par   | */
/* |            les paves englobants a l'aide de regles Create et       | */
/* |            CreateBefore (si Tete est vrai) ou CreateAfter et       | */
/* |            CreateLast (si Tete est faux).                          | */
/* |            Au retour volsupp indique le volume des paves de        | */
/* |            presentation tues et PavReaff le pave a reafficher.     | */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
static void         KillPresEnclosing (PtrAbstractBox pAb, boolean Tete, PtrDocument pDoc, PtrAbstractBox * PavReaff, int *volsupp, boolean SaufCreeAvec)

#else  /* __STDC__ */
static void         KillPresEnclosing (pAb, Tete, pDoc, PavReaff, volsupp, SaufCreeAvec)
PtrAbstractBox             pAb;
boolean             Tete;
PtrDocument         pDoc;
PtrAbstractBox            *PavReaff;
int                *volsupp;
boolean             SaufCreeAvec;

#endif /* __STDC__ */

{
   PtrAbstractBox             PavEnglob, pAbb, PavR;
   boolean             stop;
   PtrElement          pEl1;

   *volsupp = 0;
   /* Detruit les paves de presentation crees par les regles */
   /* CreateBefore et CreateAfter de pAb. */
   KillPresSibling (pAb->AbPrevious, TRUE,
		  pDoc, &PavR, PavReaff, volsupp, pAb, SaufCreeAvec);
   KillPresSibling (pAb->AbNext, FALSE,
		  pDoc, &PavR, PavReaff, volsupp, pAb, SaufCreeAvec);
   /* traite les paves englobants */
   PavEnglob = pAb->AbEnclosing;
   while (PavEnglob != NULL)
      if ((Tete && PavEnglob->AbTruncatedHead)
	  || (!Tete && PavEnglob->AbTruncatedTail))
	 /* pave deja traite', on s'arrete */
	 PavEnglob = NULL;
      else
	{
	   if (!NonSecable (PavEnglob))
	      if (Tete)
		{
		   PavEnglob->AbTruncatedHead = TRUE;
		   /* cherche et supprimes les paves crees par CreateFirst */
		   pAbb = PavEnglob->AbFirstEnclosed;
		   stop = FALSE;
		   do
		      if (pAbb == NULL)
			 stop = TRUE;
		      else if (!pAbb->AbPresentationBox || pAbb->AbDead
			       || pAbb->AbElement != PavEnglob->AbElement)
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
			   TuePave (pAbb);
			   *PavReaff = Englobant (*PavReaff, pAbb);
			   /* traite les paves qui se referent au pave detruit */
			   SuppRfPave (pAbb, &PavR, pDoc);
			   *PavReaff = Englobant (*PavReaff, PavR);
			   pAbb = pAbb->AbNext;
			}
		   while (!(stop));
		   /* cherche et supprime les paves crees par CreateBefore */
		   /* mais ne supprime pas ceux crees par CreateWith */
		   pAbb = PavEnglob->AbPrevious;
		   stop = FALSE;
		   do
		      if (pAbb == NULL)
			 stop = TRUE;
		      else if (!pAbb->AbPresentationBox || pAbb->AbDead
			       || pAbb->AbElement != PavEnglob->AbElement)
			 stop = TRUE;
		      else
			{
			   if (RegleCree (pDoc, PavEnglob, pAbb) != FnCreateWith)
			     {
				*volsupp += pAbb->AbVolume;
				TuePave (pAbb);
				*PavReaff = Englobant (*PavReaff, pAbb);
				/* traite les paves qui se referent au pave detruit */
				SuppRfPave (pAbb, &PavR, pDoc);
				*PavReaff = Englobant (*PavReaff, PavR);
			     }
			   pAbb = pAbb->AbPrevious;
			}
		   while (!(stop));
		}
	      else
		{
		   /* Tete = FALSE */
		   PavEnglob->AbTruncatedTail = TRUE;
		   /* cherche et supprime les paves crees par CreateLast */
		   pAbb = PavEnglob->AbFirstEnclosed;
		   /* cherche d'abord le dernier pave englobe */
		   stop = FALSE;
		   if (pAbb != NULL)
		      do
			 if (pAbb->AbNext == NULL)
			    stop = TRUE;
			 else
			    pAbb = pAbb->AbNext;
		      while (!(stop));
		   stop = FALSE;
		   do
		      if (pAbb == NULL)
			 stop = TRUE;
		      else if (!pAbb->AbPresentationBox || pAbb->AbDead
			       || pAbb->AbElement != PavEnglob->AbElement)
			 stop = TRUE;
		      else
			{
			   *volsupp += pAbb->AbVolume;
			   TuePave (pAbb);
			   *PavReaff = Englobant (*PavReaff, pAbb);
			   /* traite les paves qui se referent au pave detruit */
			   SuppRfPave (pAbb, &PavR, pDoc);
			   *PavReaff = Englobant (*PavReaff, PavR);
			   pAbb = pAbb->AbPrevious;
			}
		   while (!(stop));
		   /* cherche et supprime les paves crees par CreateAfter */
		   /* mais ne supprime pas ceux crees par CreateWith */
		   pAbb = PavEnglob->AbNext;
		   stop = FALSE;
		   do
		      if (pAbb == NULL)
			 stop = TRUE;
		      else if (!pAbb->AbPresentationBox || pAbb->AbDead
			       || pAbb->AbElement != PavEnglob->AbElement)
			 stop = TRUE;
		      else
			{
			   if (RegleCree (pDoc, PavEnglob, pAbb) != FnCreateWith)
			     {
				*volsupp += pAbb->AbVolume;
				TuePave (pAbb);
				*PavReaff = Englobant (*PavReaff, pAbb);
				/* traite les paves qui se referent au pave detruit */
				SuppRfPave (pAbb, &PavR, pDoc);
				*PavReaff = Englobant (*PavReaff, PavR);
			     }
			   pAbb = pAbb->AbNext;
			}
		   while (!(stop));
		}
	   /* passe au niveau superieur */
	   PavEnglob = PavEnglob->AbEnclosing;
	}
}
#endif /* __COLPAGE__ */



/* ---------------------------------------------------------------------- */
/* |    SupprPaves reduit de dvol le volume du pave pAbbRoot en        | */
/* |            supprimant des paves, en tete si Tete est vrai, en      | */
/* |            queue sinon.                                            | */
/* |            Au retour dvol contient le volume qui reste a supprimer.| */
/* |            suppression page par page si vue avec pages             | */
/* |            cette procedure est toujours appelee avec la racine de  | */
/* |            la vue si le document est mis en pages.                 | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         SupprPaves (PtrAbstractBox pAbbRoot, PtrDocument pDoc, boolean Tete, int *dvol)

#else  /* __STDC__ */
static void         SupprPaves (pAbbRoot, pDoc, Tete, dvol)
PtrAbstractBox             pAbbRoot;
PtrDocument         pDoc;
boolean             Tete;
int                *dvol;

#endif /* __STDC__ */

{
   PtrAbstractBox             pAb, pAbbSibling, PavReaff, PavR;
   int                 volsupp, volpres;
   boolean             stop, stop1;
   PtrElement          pEl1;

#ifdef __COLPAGE__
   PtrAbstractBox             pAbb;
   int                 Vue, nbpages;
   boolean             caspage;

#endif /* __COLPAGE__ */

   /* cherche le premier pave englobe' de volume inferieur a dvol et qui */
   /* soit secable */
   pAb = pAbbRoot;
#ifdef __COLPAGE__
   PavReaff = NULL;
   /* suppression d'une page si la vue est paginee */
   caspage = FALSE;
   Vue = pAb->AbDocView;
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
	   else if (pAb->AbVolume + volpres <= *dvol || NonSecable (pAb))
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
		     if (!Tete)
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
			     if (Tete)
				pAb = pAb->AbNext;
			     else
				pAb = pAb->AbPrevious;
			  }
		     while (!(stop1));
		  }
	     }
	while (!(stop));
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
		  if (!Tete)
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
				  KillPresEnclosing (pAbb, Tete, pDoc, &PavReaff, &volsupp, FALSE);
				  *dvol -= volsupp;
				  TuePave (pAbb);
				  SuppRfPave (pAbb, &PavR, pDoc);
				  nbpages--;
			       }
			     else
				pAbb = NULL;
		       if (pAbb != NULL)
			  if (Tete)
			     pAbb = pAbb->AbNext;
			  else
			     pAbb = pAbb->AbPrevious;
		    }
		  if (VueAssoc (pAbbRoot->AbElement))
		     pDoc->DocAssocModifiedAb[pAbbRoot->AbElement->ElAssocNum - 1] = pAbbRoot;

		  else
		     pDoc->DocViewModifiedAb[Vue - 1] = pAbbRoot;
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
		  PavReaff = pAb;
		  /* il faudra reafficher au moins pAb */
		  /* tous les paves englobant pAb sont coupe's. On supprime leurs */
		  /* paves de presentation. On supprime aussi ceux crees par le pave */
		  /* tue'. */
		  KillPresEnclosing (pAb, Tete, pDoc, &PavReaff, &volsupp, FALSE);
		  *dvol -= volsupp;
		  /* detruit le pave trouve' et toute sa descendance */
		  TuePave (pAb);
		  /* detruit les paves freres tant qu'il reste du volume a supprimer */
		  pAbbSibling = pAb;
		  while (*dvol > 0 && pAbbSibling != NULL)
		    {
		       if (Tete)
			  pAbbSibling = pAbbSibling->AbNext;
		       else
			  pAbbSibling = pAbbSibling->AbPrevious;
		       if (pAbbSibling != NULL)
			  /* on ne supprime pas les pave's de presentation, sauf ceux qui */
			  /* ont ete cree's par une regle FnCreateEnclosing */
			  if (!pAbbSibling->AbPresentationBox ||
			      (pAbbSibling->AbPresentationBox && pAbbSibling->AbFirstEnclosed != NULL &&
			       pAbbSibling->AbFirstEnclosed->AbElement == pAbbSibling->AbElement))
			     SupprPaves (pAbbSibling, pDoc, Tete, dvol);
		       /* traite ensuite les paves qui se referent au pave detruit */
		    }
		  SuppRfPave (pAb, &PavR, pDoc);
		  PavReaff = Englobant (PavReaff, PavR);
		  pEl1 = pAbbRoot->AbElement;
		  if (VueAssoc (pAbbRoot->AbElement))
		     pDoc->DocAssocModifiedAb[pEl1->ElAssocNum - 1] =
			Englobant (PavReaff, pDoc->DocAssocModifiedAb[pEl1->ElAssocNum - 1]);
		  else
		     pDoc->DocViewModifiedAb[pAbbRoot->AbDocView - 1] =
			Englobant (PavReaff, pDoc->DocViewModifiedAb[pAbbRoot->AbDocView - 1]);
		  *dvol = 0;
#ifdef __COLPAGE__
	       }		/* fin cas sans pages */
#endif /* __COLPAGE__ */
	  }			/* fin dvol > 0 */
#ifdef __COLPAGE__
     }				/* fin !caspage */
#endif /* __COLPAGE__ */
}



/* ---------------------------------------------------------------------- */
/* |    AjVolVue ajuste le volume de la vue dont le pave pAbbRoot      | */
/* |            est le pave racine.                                     | */
/* |            VolOpt est le volume optimum de cette vue.              | */
/* |            ElMilieu est l'element qui devrait etre au milieu       | */
/* |            de la vue.                                              | */
  /*     pAbbRoot est une vraie racine d'image abstraite        | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         AjVolVue (int VolOpt, PtrAbstractBox pAbbRoot, PtrElement ElMilieu, PtrDocument pDoc)

#else  /* __STDC__ */
static void         AjVolVue (VolOpt, pAbbRoot, ElMilieu, pDoc)
int                 VolOpt;
PtrAbstractBox             pAbbRoot;
PtrElement          ElMilieu;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   boolean             ajoute, supprime, miltete, stop;
   int                 vue, volprec, dvol;
   PtrAbstractBox             pAbbPrevious, pAbbParent;

   /* evalue d'abord s'il faut ajouter ou supprimer des paves */
   ajoute = FALSE;
   supprime = FALSE;
   if (!NonSecable (pAbbRoot))
      if (pAbbRoot->AbVolume < VolOpt - VolOpt / 8)
	 /* il faudrait creer de nouveaux paves dans cette vue */
	 ajoute = TRUE;
      else if (pAbbRoot->AbVolume > VolOpt + VolOpt / 8)
	 /* il faudrait supprimer des paves dans cette vue */
	 supprime = TRUE;
   if (ajoute || supprime)
     {
	/* calcule la position actuelle de ElMilieu dans la vue, pour savoir */
	/* s'il faut modifier le debut ou la fin de la vue */
	vue = pAbbRoot->AbDocView;
	if (ElMilieu->ElAbstractBox[vue - 1] == NULL)
	   /* ElMilieu n'a pas de pave dans cette vue */
	   /* cherche le 1er element englobant ElMilieu qui ait un pave dans */
	   /* la vue */
	  {
	     stop = FALSE;
	     do
	       {
		  ElMilieu = ElMilieu->ElParent;
		  if (ElMilieu == NULL)
		    {
		       stop = TRUE;
		       ElMilieu = pAbbRoot->AbElement;
		    }
		  else if (ElMilieu->ElAbstractBox[vue - 1] != NULL)
		     stop = TRUE;
	       }
	     while (!(stop));
	  }
	/* ElMilieu a un pave dans cette vue */
	/* calcule le volume des paves qui le precedent */
	volprec = 0;
	pAbbParent = ElMilieu->ElAbstractBox[vue - 1];
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
	while (!(pAbbParent == NULL));
	miltete = volprec < VolOpt / 2;
	if (ajoute)
	   /* il faut ajouter des paves */
	   if (miltete)
	      /* l'element qui devrait etre au milieu est dans la 1ere moitie' */
	      if (pAbbRoot->AbTruncatedHead)
		 /* le debut de l'image n'est pas complet, on ajoute en tete */
		 AddAbsBoxes (pAbbRoot, pDoc, TRUE);
	      else
		{
		   /* le debut de l'image est complet */
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
		/* idebut n'est pas complet */
		if (pAbbRoot->AbTruncatedHead)
		   AddAbsBoxes (pAbbRoot, pDoc, TRUE);
	     }
	else if (supprime)
	   /* supprime des paves en queue si l'element qui devrait etre au */
	   /* milieu est dans la premiere moitie de l'image, et inversement */
	  {
	     dvol = pAbbRoot->AbVolume - VolOpt;
	     SupprPaves (pAbbRoot, pDoc, !miltete, &dvol);
	  }
     }
}



/* ---------------------------------------------------------------------- */
/* |    AdjustVolume pour toutes les vues du document pointe' par pDoc  | */
/* |            ajuste (reduit ou augmente) le volume des images        | */
/* |            abstraites en conservant l'element pointe par pEl au    | */
/* |            milieu (ou a peu pres) de l'image abstraite.            | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                AdjustVolume (PtrElement pEl, PtrDocument pDoc)

#else  /* __STDC__ */
void                AdjustVolume (pEl, pDoc)
PtrElement          pEl;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   int                 vue;
   PtrDocument         pDo1;
   PtrElement          pEl1;

#ifdef __COLPAGE__
   PtrAbstractBox             pAb;
   int                 nb;

#endif /* __COLPAGE__ */

   if (pEl != NULL && pDoc != NULL)
     {
	pDo1 = pDoc;
	pEl1 = pEl;
	if (!VueAssoc (pEl))
	   /* une vue de l'arbre principal */
	   for (vue = 1; vue <= MAX_VIEW_DOC; vue++)
	     {
		/* traite toutes les vues */
		if (pDo1->DocView[vue - 1].DvPSchemaView > 0)
		   /* la vue existe */
		  {
#ifdef __COLPAGE__
		     /* le document est-il pagine dans cette vue ? */
		     /* si oui, on compte le nombre de pages actuel */
		     /* pour etre sur d'ajouter au moins une page */
		     /* (sauf si fin de vue) */
		     pAb = pDo1->DocViewRootAb[vue - 1];
		     if (pAb->AbFirstEnclosed != NULL
			 && pAb->AbFirstEnclosed->AbElement->ElTypeNumber ==
			 PageBreak + 1)
		       {
			  nb = NbPages (pAb);
			  pDo1->DocViewNPages[vue - 1] = nb;
			  pDo1->DocViewFreeVolume[vue - 1] = THOT_MAXINT;
		       }
		     else
#endif /* __COLPAGE__ */
			pDo1->DocViewFreeVolume[vue - 1] =
			   pDo1->DocViewVolume[vue - 1] - pDo1->DocViewRootAb[vue - 1]->AbVolume;
		     AjVolVue (pDo1->DocViewVolume[vue - 1], pDo1->DocViewRootAb[vue - 1],
			       pEl, pDoc);
		  }
	     }
	else
	   /* element associe */
	if (pDo1->DocAssocFrame[pEl1->ElAssocNum - 1] > 0)
	   /* la vue de ces elements associes a ete creee */
	   if (pDo1->DocAssocVolume[pEl1->ElAssocNum - 1] > 0)
	      /* on ne fait rien si ces elements associes sont affiches */
	      /* dans des boites de haut ou bas de page */
	     {
#ifdef __COLPAGE__
		pAb = pDo1->DocAssocRoot[pEl1->ElAssocNum - 1]->ElAbstractBox[0];
		if (pAb->AbFirstEnclosed != NULL
		    && pAb->AbFirstEnclosed->AbElement->ElTypeNumber ==
		    PageBreak + 1)
		   /* vue du document paginee */
		  {
		     nb = NbPages (pAb);
		     pDo1->DocAssocNPages[vue - 1] = nb;
		     pDo1->DocAssocFreeVolume[pEl1->ElAssocNum - 1] = THOT_MAXINT;
		  }
		else
#endif /* __COLPAGE__ */
		   pDo1->DocAssocFreeVolume[pEl1->ElAssocNum - 1] =
		      pDo1->DocAssocVolume[pEl1->ElAssocNum - 1] -
		      pDo1->DocAssocRoot[pEl1->ElAssocNum - 1]->ElAbstractBox[0]->AbVolume;
		AjVolVue (pDo1->DocAssocVolume[pEl1->ElAssocNum - 1],
		     pDo1->DocAssocRoot[pEl1->ElAssocNum - 1]->ElAbstractBox[0],
			  pEl, pDoc);
	     }
     }
}



/* ---------------------------------------------------------------------- */
/* |    IncreaseVolume Le Mediateur augmente de dVol le volume affichable  | */
/* |            dans la fenetre ViewFrame. Met a jour la capacite de la   | */
/* |            vue affichee dans cette frame et cree de nouveaux       | */
/* |            paves en tete ou en queue, selon le booleen EnTete,     | */
/* |            de l'image abstraite affichee dans ViewFrame.             | */
/* |            On cree des paves, le Mediateur se charge du            | */
/* |            reaffichage                                             | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                IncreaseVolume (boolean EnTete, int dVol, int frame)

#else  /* __STDC__ */
void                IncreaseVolume (EnTete, dVol, frame)
boolean             EnTete;
int                 dVol;
int                 frame;

#endif /* __STDC__ */

{
   PtrDocument         pDoc;
   int                 vue, h;
   boolean             assoc;
   PtrAbstractBox             pAb;

#ifdef __COLPAGE__
   PtrElement          pElRacine, pEl1;
   int                 nb, VueSch;
   PtrPSchema          pSchPage;
   FILE               *list;

#endif /* __COLPAGE__ */


   DocVueFen (frame, &pDoc, &vue, &assoc);
   /* met a jour la nouvelle capacite de la vue, indique dans le contexte */
   /* du document le volume des paves a creer et cherche le pave racine de */
   /* la vue */
   if (pDoc == NULL)
      printf ("\nErreur IncreaseVolume: frame incorrecte\n");
   else
     {
	if (assoc)
	  {
	     /* element associe */
	     pAb = pDoc->DocAssocRoot[vue - 1]->ElAbstractBox[0];
#ifdef __COLPAGE__
	     /* attention, vue contient le numero d'element associe */
	     pElRacine = pDoc->DocAssocRoot[vue - 1];
#endif /* __COLPAGE__ */
	     pDoc->DocAssocVolume[vue - 1] = pAb->AbVolume + dVol;
	     pDoc->DocAssocFreeVolume[vue - 1] = dVol;
	  }
	else
	  {
	     /* element de l'arbre principal */
#ifdef __COLPAGE__
	     pElRacine = pDoc->DocRootElement;
#endif /* __COLPAGE__ */
	     pAb = pDoc->DocViewRootAb[vue - 1];
	     pDoc->DocViewVolume[vue - 1] = pAb->AbVolume + dVol;
	     pDoc->DocViewFreeVolume[vue - 1] = dVol;
	  }
#ifdef __COLPAGE__
	/*  test si vue paginee */
	pEl1 = pElRacine->ElFirstChild;
	/* recherche le numero de vue defini dans le schema de presentation */
	/* Attention il faut appeler VueAAppliquer avec pElRacine */
	VueSch = VueAAppliquer (pElRacine, NULL, pDoc, vue);
	if (TypeBPage (pEl1, VueSch, &pSchPage) != 0)

	   /* le document est-il pagine dans cette vue ? */
	   /* si oui, on compte le nombre de pages actuel */
	   /* pour etre sur d'ajouter au moins une page */
	   /* (sauf si fin de vue) */
	  {
	     nb = NbPages (pAb);
	     if (assoc)
		/* element associe */
	       {
		  pDoc->DocAssocFreeVolume[vue - 1] = THOT_MAXINT;
		  pDoc->DocAssocNPages[vue - 1] = nb;
	       }
	     else
	       {
		  /* element de l'arbre principal */
		  pDoc->DocViewFreeVolume[vue - 1] = THOT_MAXINT;
		  pDoc->DocViewNPages[vue - 1] = nb;
	       }
	  }
#endif /* __COLPAGE__ */
	if (!NonSecable (pAb))
	  {
	     /* cree les paves de la partie qui va apparaitre */
	     AddAbsBoxes (pAb, pDoc, EnTete);

	     /* signale au Mediateur les paves crees et detruits */
#ifdef __COLPAGE__
	     h = HauteurCoupPage;
#else  /* __COLPAGE__ */
	     h = HauteurPage;
#endif /* __COLPAGE__ */
	     if (assoc)
	       {
		  if (pDoc->DocAssocModifiedAb[vue - 1] != NULL)
		    {
		       (void) ChangeConcreteImage (frame, &h, pDoc->DocAssocModifiedAb[vue - 1]);
		       FreeDeadAbstractBoxes (pDoc->DocAssocModifiedAb[vue - 1]);
		       pDoc->DocAssocModifiedAb[vue - 1] = NULL;
		    }
	       }
	     else if (pDoc->DocViewModifiedAb[vue - 1] != NULL)
	       {
#ifdef __COLPAGE__
		  list = fopen ("/perles/roisin/debug/volaug", "w");
		  if (list != NULL)
		    {
		       NumPav (pDoc->DocViewRootAb[vue - 1]);
		       AffPaves (pDoc->DocViewRootAb[0], 2, list);
		       fclose (list);
		    }
		  /* appel de modifVue depuis la racine car de nouvelles */
		  /* pages ont pu etre creees TODO incoherent ??? */
#endif /* __COLPAGE__ */
		  (void) ChangeConcreteImage (frame, &h, pDoc->DocViewModifiedAb[vue - 1]);
		  FreeDeadAbstractBoxes (pDoc->DocViewModifiedAb[vue - 1]);
		  pDoc->DocViewModifiedAb[vue - 1] = NULL;
	       }
	  }
     }
}



/* ---------------------------------------------------------------------- */
/* |    DecreaseVolume Le Mediateur reduit de dVol le volume affichable      | */
/* |            dans la fenetre frame. Met a jour la capacite de la     | */
/* |            vue affichee dans cette frame et supprime des paves     | */
/* |            en tete ou en queue, selon le booleen EnTete, de        | */
/* |            l'image abstraite affichee dans frame.          | */
/* |            On supprime des paves, le Mediateur se charge du        | */
/* |            reaffichage                                             | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                DecreaseVolume (boolean EnTete, int dVol, int frame)

#else  /* __STDC__ */
void                DecreaseVolume (EnTete, dVol, frame)
boolean             EnTete;
int                 dVol;
int                 frame;

#endif /* __STDC__ */

{
   PtrDocument         pDoc;
   int                 vue, h;
   boolean             assoc;
   PtrAbstractBox             pAb;

#ifdef __COLPAGE__
   FILE               *list;

#endif /* __COLPAGE__ */

   DocVueFen (frame, &pDoc, &vue, &assoc);
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
	     pAb = pDoc->DocAssocRoot[vue - 1]->ElAbstractBox[0];
	     pDoc->DocAssocVolume[vue - 1] = pAb->AbVolume - dVol;
	  }
	else
	  {
	     /* element de l'arbre principal */
	     pAb = pDoc->DocViewRootAb[vue - 1];
	     pDoc->DocViewVolume[vue - 1] = pAb->AbVolume - dVol;
	  }
	if (pAb != NULL)
#ifdef __COLPAGE__
	  {
#endif /* __COLPAGE__ */
	     if (dVol >= pAb->AbVolume)
		printf ("Erreur DecreaseVolume: dVol=%3d volume vue=%3d\n", dVol, pAb->AbVolume);
	     /* supprime les paves */
	     SupprPaves (pAb, pDoc, EnTete, &dVol);
#ifdef __COLPAGE__
	     /* sauvegarde de l'image abstraite pour tests */
	     list = fopen ("/perles/roisin/debug/volred", "w");
	     if (list != NULL)
	       {
		  NumPav (pAb);
		  AffPaves (pAb, 2, list);
		  fclose (list);
	       }
	     /* signale au Mediateur les paves modifies */
	     h = HauteurCoupPage;
	     /* appel de modifVue depuis la racine car de nouvelles */
	     /* pages ont pu etre detruites */
	     (void) ChangeConcreteImage (frame, &h, pAb);
	     /* meme chose pour FreeDeadAbstractBoxes */
	     FreeDeadAbstractBoxes (pAb);
	     /* DocAssocModifiedAb et DocViewModifiedAb non utilises */
	     if (assoc)
		pDoc->DocAssocModifiedAb[vue - 1] = NULL;
	     else
		pDoc->DocViewModifiedAb[vue - 1] = NULL;
	  }
#else  /* __COLPAGE__ */
	     /* signale au Mediateur les paves modifies */
	     h = HauteurPage;
	     if (assoc)
	       {
		  if (pDoc->DocAssocModifiedAb[vue - 1] != NULL)
		    {
		       (void) ChangeConcreteImage (frame, &h, pDoc->DocAssocModifiedAb[vue - 1]);
		       FreeDeadAbstractBoxes (pDoc->DocAssocModifiedAb[vue - 1]);
		       pDoc->DocAssocModifiedAb[vue - 1] = NULL;
		    }
	       }
	     else if (pDoc->DocViewModifiedAb[vue - 1] != NULL)
	       {
		  (void) ChangeConcreteImage (frame, &h, pDoc->DocViewModifiedAb[vue - 1]);
		  FreeDeadAbstractBoxes (pDoc->DocViewModifiedAb[vue - 1]);
		  pDoc->DocViewModifiedAb[vue - 1] = NULL;
	       }
#endif /* __COLPAGE__ */
	  }
     }



/* ---------------------------------------------------------------------- */
/* |    VerifAbsBoxe verifie que l'element pointe' par pEl a au moins      | */
/* |            un pave dans la vue Vue. S'il n'en a pas, essaie d'en   | */
/* |            creer un en modifiant l'image abstraite de cette vue.   | */
/* |            Si debut est vrai, on cree l'image de la vue en         | */
/* |            commencant par l'element pointe' par pEl, sinon, on     | */
/* |            place cet element au milieu de l'image creee.           | */
/* |            Si affiche est Vrai, l'image est reaffichee.            | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
   void                VerifAbsBoxe (PtrElement pEl, int Vue, PtrDocument pDoc, boolean debut, boolean affiche)
#else  /* __STDC__ */
   void                VerifAbsBoxe (pEl, Vue, pDoc, debut, affiche)
   PtrElement          pEl;
   int                 Vue;
   PtrDocument         pDoc;
   boolean             debut;
   boolean             affiche;

#endif /* __STDC__ */
   {
      boolean             vueexiste, creation, stop;
      PtrElement          pAncetre, pElPage;
      PtrElement          pAsc[MaxAsc];
      int                 NumAsc, i, volsupp, frame, nAssoc, typeBoite,
                          h;
      PtrAbstractBox             PavDetruit, PavRestant, DerCreVide, PremCreVide,
                          PavReaff, pAbbRoot, pPrevious;

#ifndef __COLPAGE__
      int         nR;

#endif /* __COLPAGE__ */
      PtrElement          pEl1;
      PtrDocument         pDo1;
      PtrAbstractBox             pAbbox1;
      boolean             complet;

#ifdef __COLPAGE__
      int                 VueSch, tour;
      PtrAbstractBox             pPavPage, pAbb;
      PtrElement          pElRacine, pFils, pElRef;
      boolean             trouve, VuePaginee, vueassoc, acreer;
      PtrPSchema          pSchPage;
      FILE               *list;

      VuePaginee = FALSE;
      pElPage = NULL;
      if (pEl != NULL)
	{
	   nAssoc = pEl->ElAssocNum;
	   /* verifie si la vue a ete creee */
	   if (VueAssoc (pEl))
	      /* element associe qui s'affiche dans une autre vue que */
	      /* la vue principale */
	     {
		vueexiste = pDoc->DocAssocFrame[nAssoc - 1] != 0 && Vue == 1;
		pElRacine = pDoc->DocAssocSubTree[nAssoc - 1];
		if (pElRacine == NULL)
		   pElRacine = pDoc->DocAssocRoot[nAssoc - 1];
		if (pElRacine != NULL)
		   pAbbRoot = pElRacine->ElAbstractBox[Vue - 1];
		frame = pDoc->DocAssocFrame[nAssoc - 1];
		vueassoc = TRUE;
	     }
	   else
	     {
		vueexiste = pDoc->DocView[Vue - 1].DvPSchemaView > 0;
		pElRacine = pDoc->DocRootElement;
		pAbbRoot = pElRacine->ElAbstractBox[Vue - 1];
		frame = pDoc->DocViewFrame[Vue - 1];
		vueassoc = FALSE;
	     }
	   /*  test si vue paginee */
	   pEl1 = pElRacine->ElFirstChild;
	   /* recherche le numero de vue defini dans le schema de presentation */
	   /* Attention il faut appeler VueAAppliquer avec pElRacine */
	   VueSch = VueAAppliquer (pElRacine, NULL, pDoc, Vue);
	   if (TypeBPage (pEl1, VueSch, &pSchPage) != 0)
	      VuePaginee = TRUE;
	   acreer = vueexiste;
	   /* a priori on cree les paves de l'element */
	   if (VuePaginee && acreer)
	      /* Vue paginee : code specifique */
	     {
		/* si l'element a un pave, est-ce le premier des dup ? */
		/* si c'est le cas (le pave n'est pas coupe en tete), */
		/* on ne cree rien car l'element a son premier pave deja cree */
		trouve = FALSE;
		pAbb = pEl->ElAbstractBox[Vue - 1];
		if (pAbb != NULL)
		  {
		     acreer = FALSE;	/* a priori rien a creer */
		     while (pAbb != NULL && pAbb->AbPresentationBox && pAbb->AbNext != NULL)
			pAbb = pAbb->AbNext;
		     if (pAbb != NULL && pAbb->AbElement == pEl && pAbb->AbLeafType == LtCompound)
			acreer = (pAbb->AbTruncatedHead);
		  }
		if (acreer)
		  {
		     /* est-ce l'element racine d'un arbre d'elements associes ? */
		     pElRef = pEl;
		     if (pElRef->ElParent == NULL)
			/* c'est une racine */
			if (nAssoc != 0 && vueassoc == FALSE)
			   /* c'est un element associe racine d'elements */
			  {
			     /* s'affichant en haut ou bas de page */
			     /* on creera son pave sur la page de la premiere */
			     /* reference a son premier fils */
			     pElRef = pElRef->ElFirstChild;
			     if (pElRef == NULL)
				acreer = FALSE;
			  }
		  }
		if (acreer)
		   /* si c'est un element associe qui s'affiche en haut ou bas de page */
		   /* il faut rechercher la premiere reference et afficher la page */
		   /* de cet element reference. CreePaves creera les paves de l'element */
		   /* associe par appel de CrPavHB */
		   if (nAssoc != 0 && vueassoc == FALSE)
		      /* on recherche l'ascendant qui est l'element reference (pEl peut */
		      /* etre un fils de cet element) */
		      /* il faut utiliser pElRef et non pEl */
		     {
			while (pElRef->ElParent->ElParent != NULL)
			   pElRef = pElRef->ElParent;
			tour = 1;
			pEl1 = pElRef;
			while (!trouve && pEl1 != NULL)
			  {
			     pElPage = PageElAssoc (pEl1, VueSch, &typeBoite);
			     if (pElPage == NULL)
				/* c'est un element non reference, il faut creer son */
				/* pave a cote d'un element reference frere */
				/* si il n'y a aucun frere (seul elt) TODO plus tard */
				if (tour == 1)
				   if (pEl1->ElPrevious != NULL)
				      pEl1 = pEl1->ElPrevious;
				   else
				     {
					tour = 2;
					/* au 2eme tour on prend les freres suivants */
					pEl1 = pElRef->ElNext;
				     }
				else
				  {
				     pEl1 = pEl1->ElNext;
				  }
			     else	/* pElPage != NULL */
				trouve = TRUE;	/* on a trouver la page a creer */
			  }
			if (pEl1 == NULL)
			   acreer = FALSE;	/* TODO plus tard */
		     }

		if (acreer && !trouve)
		  {
		     /* on recherche la page dans laquelle se trouve pEl */
		     trouve = FALSE;
		     /* si pEl est une PageBreak, c'est fini */
		     if (pEl->ElTypeNumber == PageBreak + 1
			 && (pEl->ElPageType == PgBegin
			     || pEl->ElPageType == PgComputed
			     || pEl->ElPageType == PgUser)
			 && pEl->ElViewPSchema == VueSch)
		       {
			  trouve = TRUE;
			  pElPage = pEl;
		       }
		     else
		       {
			  pEl1 = pEl;
			  while (!trouve && pEl1 != NULL)
			    {
			       pElPage = BackSearchTypedElem (pEl1, PageBreak + 1, NULL);
			       if (pElPage != NULL && pElPage->ElViewPSchema == VueSch
				   && (pElPage->ElPageType == PgBegin
				       || pElPage->ElPageType == PgComputed
				 || pElPage->ElPageType == PgUser))
				  trouve = TRUE;
			       else
				  pEl1 = pElPage;
			    }
		       }
		     if (trouve && pElPage->ElAbstractBox[Vue - 1] != NULL)
			/* si trouve et si l'element marque page a un pave dans la vue */
			/* c'est fini: l'element pEl n'est pas visible dans cette vue, */
			/* sinon il aurait eu un pave */
			acreer = FALSE;
		     if (!trouve)
			/* si on n'a pas trouve d'element marque page precedent */
			/* c'est que pEl est la racine ou un element marque page */
			/* d'une autre vue */
			/* on creera l'image a partir de la racine */
			/* sans partir d'une marque de page */
			/* sinon erreur */
			if (!(pEl->ElParent == NULL
			      || (pEl->ElTypeNumber == PageBreak + 1
				  && pEl->ElViewPSchema != VueSch)))
			  {
			     printf ("peut etre erreur VerifAbsBoxe : pas trouve de marque page %s",
				     pEl->ElSructSchema->SsRule[pEl->ElTypeNumber - 1].SrName);
			     printf ("\n");
			     acreer = FALSE;
			  }
		  }
		if (acreer)
		  {
		     /* on detruit l'i.a. sauf la racine */
		     /* pour recreer correctement la partie de l'i.a. demandee */
		     pAbbRoot = pElRacine->ElAbstractBox[Vue - 1];
		     if (pAbbRoot != NULL)
		       {
			  pAbbox1 = pAbbRoot->AbFirstEnclosed;
			  while (pAbbox1 != NULL)
			    {
			       TuePave (pAbbox1);
			       pAbbox1 = pAbbox1->AbNext;
			    }
			  pAbbRoot->AbDead = TRUE;	/* pour que sa boite soit */
			  /* detruite */
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
			  pAbbRoot = CreePaves (pElRacine, pDoc, Vue, TRUE,
						 FALSE, &complet);
			  pDoc->DocViewRootAb[Vue - 1] = pAbbRoot;
			  pAbbRoot->AbTruncatedHead = TRUE;
			  pAbbRoot->AbTruncatedTail = TRUE;
		       }
		  }
		/* on cree les paves de la Marque Page */
		/* par construction (cf. CreePaves), ils seront mis sous */
		/* la racine */
		if (trouve)
		  {
		     pPavPage = CreePaves (pElPage, pDoc, Vue, TRUE, TRUE, &complet);
		     /* on cree les paves de ses ascendants en les marquant CT et CQ */
		     pAncetre = pElPage->ElParent;
		     pFils = pElPage;
		     NumAsc = 0;
		     stop = FALSE;
		     do
			if (pAncetre == NULL)
			   stop = TRUE;
			else if (pAncetre->ElAbstractBox[Vue - 1] != NULL)
			   /* normalement c'est la racine */
			   stop = TRUE;
			else
			  {
			     /* on ne creera le pave de l'ascendant que si il */
			     /* doit avoir des fils dans la page */
			     if (!(NumAsc == 0 && pFils->ElNext == NULL))
			       {
				  if (NumAsc < MaxAsc)
				     NumAsc++;
				  pAsc[NumAsc - 1] = pAncetre;
			       }
			     pFils = pAncetre;
			     pAncetre = pAncetre->ElParent;
			  }
		     while (!stop);
		     /* cree les paves de ces elements, en commencant par */
		     /* celui qui contient tous les autres.  */
		     PremCreVide = NULL;
		     i = NumAsc;
		     while (i > 0)
		       {
			  /* cree juste le pave, sans sa descendance et sans lui */
			  /* appliquer les regles de presentation. */
			  pPrevious = CreePaves (pAsc[i - 1], pDoc, Vue, TRUE, FALSE,
					     &complet);
			  pPrevious = pAsc[i - 1]->ElAbstractBox[Vue - 1];
			  if (pPrevious != NULL)
			    {
			       /* il faudra appliquer au pave ses */
			       /* regles de presentation (sera fait par CreePaves) */
			       pPrevious->AbSize = -1;
			       /* on le marque coupe */
			       pPrevious->AbTruncatedHead = TRUE;
			       pPrevious->AbTruncatedTail = TRUE;
			       if (PremCreVide == NULL)
				  /* 1er pave cree' */
				 {
				    PremCreVide = pPrevious;
				    /* on chaine ce pave sous le corps de page */
				    while (pPavPage != NULL && pPavPage->AbPresentationBox)
				       pPavPage = pPavPage->AbNext;
				    if (pPavPage != NULL)	/* toujours vrai ? */
				      {
					 if (pPrevious->AbEnclosing->AbFirstEnclosed ==
					     pPrevious)
					    pPrevious->AbEnclosing->AbFirstEnclosed =
					       pPrevious->AbNext;
					 if (pPrevious->AbNext != NULL)
					    pPrevious->AbNext->AbPrevious = NULL;
					 pPrevious->AbNext = NULL;
					 pPrevious->AbEnclosing = pPavPage;
					 if (pPavPage->AbFirstEnclosed == NULL)
					    pPavPage->AbFirstEnclosed = pPrevious;
					 else
					   {
					      pAbbox1 = pPavPage->AbFirstEnclosed;
					      while (pAbbox1->AbNext != NULL)
						 pAbbox1 = pAbbox1->AbNext;
					      pAbbox1->AbNext = pPrevious;		/* en queue */
					      pPrevious->AbPrevious = pAbbox1;
					   }
				      }
				 }
			    }
			  i--;
		       }
		  }		/* fin cas creation paves de page et des ascendants */
		/* trouve est vrai */
		if (trouve || acreer)
		   /* on appelle IncreaseVolume pour creer l'i.a. page a page, */
		   /* appliquer les regles de presentation des paves deja crees et */
		   /* et creer l'image concrete */
		  {
		     IncreaseVolume (FALSE, THOT_MAXINT, frame);
		     /* on appelle MontrerBoite pour positionner correctement l'i.a. */
		     /* dans la fenetre */
		     if (affiche && !debut && pEl->ElAbstractBox[Vue - 1] != NULL)
			/* TODO : que faire dans les autres cas */
			MontrerBoite (frame, pEl->ElAbstractBox[Vue - 1]->AbBox, 1, 0);
		     /* sauvegarde de l'image abstraite pour tests */
		     list = fopen ("/perles/roisin/debug/verifpav", "w");
		     if (list != NULL)
		       {
			  NumPav (pAbbRoot);
			  AffPaves (pAbbRoot, 2, list);
			  fclose (list);
		       }

		     if (affiche)
			DisplayFrame (frame);
		  }
	     }
	   else
	      /* vue non paginee : on garde l'ancien code */ if (pEl->ElAbstractBox[Vue - 1] == NULL)
	     {
#else  /* __COLPAGE__ */

      if (pEl != NULL)
	 if (pEl->ElAbstractBox[Vue - 1] == NULL)
	   {
	      nAssoc = pEl->ElAssocNum;
	      /* verifie si la vue a ete creee */
	      if (VueAssoc (pEl))
		 /* element associe */
		 vueexiste = pDoc->DocAssocFrame[nAssoc - 1] != 0 && Vue == 1;
	      else
		 vueexiste = pDoc->DocView[Vue - 1].DvPSchemaView > 0;
	      /* est-ce l'element racine d'un arbre d'elements associes ? */
	      pEl1 = pEl;
	      if (pEl1->ElParent == NULL)
		 /* c'est une racine */
		 if (nAssoc != 0)
		    /* c'est un element associe */
		    /* nR: type des elements associes */
		   {
		      nR = pEl1->ElSructSchema->SsRule[pEl1->ElTypeNumber - 1].SrListItem;
		      /* si les elements associes s'affichent en haut ou en bas de */
		      /* page, la racine n'a jamais de pave */
		      vueexiste = !pEl1->ElSructSchema->SsPSchema->PsInPageHeaderOrFooter[nR - 1];
		      /* si la vue n'est pas creee, il n'y a rien a faire */
		   }
	      if (vueexiste)
		{
#endif /* __COLPAGE__ */
		   /* cherche les elements ascendants qui n'ont pas de pave dans */
		   /* cette vue */
		   NumAsc = 0;
		   /* niveau dans la pile des elements dont il faut */
		   /* creer un pave */
		   pAncetre = pEl;
		   stop = FALSE;
		   do
		      if (pAncetre == NULL)
			 stop = TRUE;
		      else if (pAncetre->ElAbstractBox[Vue - 1] != NULL)
			 stop = TRUE;
		   /* met un element dans la pile */
		      else
			{
			   if (NumAsc < MaxAsc)
			      NumAsc++;
			   pAsc[NumAsc - 1] = pAncetre;
			   /* passe a l'ascendant */
			   pEl1 = pAncetre;
			   if (pEl1->ElSructSchema->SsRule[pEl1->ElTypeNumber - 1].SrAssocElem)
			      /* on vient de traiter un element associe' */
			      /* Serait-ce un element qui s'affiche dans une boite de */
			      /* haut ou de bas de page ? */
			     {
				pElPage = PageElAssoc (pAncetre, pDoc->DocView[Vue - 1].
						       DvPSchemaView, &typeBoite);
				if (pElPage != NULL)

				   /* Il s'affiche dans une haut ou bas de page, c'est la */
				   /* marque de page a laquelle il est associe qu'il faut */
				   /* creer */
				  {
				     NumAsc = 1;
				     pAsc[NumAsc - 1] = pElPage;
				     pAncetre = pElPage->ElParent;
				     /* ce n'est pas une vue d'elements associes */
				     nAssoc = 0;
				  }
				else if (typeBoite == 0)

				   /* il ne s'affiche pas dans une boite de haut ou de */
				   /* bas de page */
				   if (Vue == 1)
				      pAncetre = pEl1->ElParent;
				   else
				      /* ce n'est pas la vue 1, l'element (associe') */
				      /* n'a pas d'image dans cette vue */
				     {
					stop = TRUE;
					vueexiste = FALSE;
				     }
				else
				   /* il devrait s'afficher dans une boite de haut ou */
				   /* de bas de page, mais il n'y a pas de page */
				  {
				     stop = TRUE;
				     vueexiste = FALSE;
				  }
			     }
			   else
			      pAncetre = pEl1->ElParent;
			}
		   while (!(stop));
		   if (pAsc[NumAsc - 1]->ElParent == NULL)
		      /* la racine de l'arbre n'a pas de pave dans cette vue */
		      creation = TRUE;
		   /* c'est une creation de vue */
		   else
		      creation = FALSE;
		   /* essaie de creer les paves de ces elements, en commencant par */
		   /* celui qui contient tous les autres. Il s'agit seulement de */
		   /* trouver s'il y a un pave ascendant non encore cree et visible */
		   PremCreVide = NULL;
		   if (vueexiste)
		     {
			i = NumAsc;
			do
			  {
			     pEl1 = pAsc[i - 1];

			     /* cree juste le pave, sans sa descendance et sans lui */
			     /* appliquer les regles de presentation. */
			     pPrevious = CreePaves (pAsc[i - 1], pDoc, Vue, TRUE, FALSE, &complet);
			     if (pEl1->ElAbstractBox[Vue - 1] != NULL)
				pPrevious = pEl1->ElAbstractBox[Vue - 1];
			     if (pPrevious != NULL)

				/* marque sur le pave cree qu'il faudra lui appliquer ses */
				/* regles de presentation (ce sera fait par CreePaves) */
				pPrevious->AbSize = -1;

			     if (pPrevious != NULL)
				/* on a cree un pave */
			       {
				  DerCreVide = pPrevious;
				  /* dernier pave cree' */
				  if (PremCreVide == NULL)
				     PremCreVide = pPrevious;
				  /* 1er pave cree' */
			       }
			     i--;
			  }
			while (!(i == 0));
		     }
		   /* si aucun pave n'a ete cree', il n'y a rien d'autre a faire */
		   if (PremCreVide != NULL)
		     {
			if (creation)
			   /* initialise le pointeur sur la racine de la vue si c'est */
			   /* une creation de vue */
			  {
			     pDo1 = pDoc;
			     if (nAssoc > 0)
				/* vue d'elements associes */
				/* le premier pave que l'on vient de creer est la racine */
				/* de l'image */
			       {
				  pDo1->DocAssocRoot[nAssoc - 1]->ElAbstractBox[0] = PremCreVide;
				  pAbbRoot = pDo1->DocAssocRoot[nAssoc - 1]->ElAbstractBox[0];
				  frame = pDo1->DocAssocFrame[nAssoc - 1];
			       }
			     else
				/* vue de l'arbre principal */
			       {
				  pDo1->DocViewRootAb[Vue - 1] = PremCreVide;
				  pAbbRoot = pDo1->DocViewRootAb[Vue - 1];
				  frame = pDo1->DocViewFrame[Vue - 1];
			       }
			  }
			else
			   /* ce n'est pas une creation de vue */
			   /* detruit l'ancien contenu de la vue */
			  {
			     PavRestant = PremCreVide;
			     /* tue les paves de presentation des elements englobants */
			     PavReaff = NULL;
			     KillPresEnclosing (PavRestant, TRUE, pDoc, &PavReaff, &volsupp, TRUE);
			     PavReaff = NULL;
			     KillPresEnclosing (PavRestant, FALSE, pDoc, &PavReaff, &volsupp, TRUE);
			     /* detruit les paves qui precedent et qui suivent le pave */
			     /* cree de plus haut niveau et ses paves englobants */
			     while (PavRestant != NULL)
			       {
				  PavDetruit = PavRestant->AbPrevious;
				  while (PavDetruit != NULL)
				    {
				       if (!PavDetruit->AbDead)
					  /* on ne detruit pas les paves des elements restant */
					  if (PavDetruit->AbElement != PavRestant->AbElement)
					    {
					       TuePave (PavDetruit);
					       PavReaff = NULL;
					       SuppRfPave (PavDetruit, &PavReaff, pDoc);
					    }
				       PavDetruit = PavDetruit->AbPrevious;
				    }
				  PavDetruit = PavRestant->AbNext;
				  while (PavDetruit != NULL)
				    {
				       if (!PavDetruit->AbDead)
					  /* on ne detruit pas les paves des elements restant */
					  if (PavDetruit->AbElement != PavRestant->AbElement)
					    {
					       TuePave (PavDetruit);
					       PavReaff = NULL;
					       SuppRfPave (PavDetruit, &PavReaff, pDoc);
					    }
				       PavDetruit = PavDetruit->AbNext;
				    }
				  PavRestant = PavRestant->AbEnclosing;
				  /* passe a l'englobant */
			       }
			     /* fait effacer tout le contenu de la vue par le Mediateur */
			     h = 0;
			     /* on ne s'occupe pas de la hauteur de page */
			     pDo1 = pDoc;
			     if (nAssoc > 0)
				/* vue d'elements associes */
			       {
				  pAbbRoot = pDo1->DocAssocRoot[nAssoc - 1]->ElAbstractBox[0];
				  frame = pDo1->DocAssocFrame[nAssoc - 1];
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
				       pDo1->DocAssocModifiedAb[nAssoc - 1] = NULL;
				    }
			       }
			     else
				/* meme traitement pour une vue de l'arbre principal */
			       {
				  pAbbRoot = pDo1->DocViewRootAb[Vue - 1];
				  frame = pDo1->DocViewFrame[Vue - 1];
				  if (frame != 0)
				    {
				       pAbbRoot->AbDead = TRUE;
				       ChangeConcreteImage (frame, &h, pAbbRoot);
				       pAbbRoot->AbDead = FALSE;
				       FreeDeadAbstractBoxes (pAbbRoot);
				       pDo1->DocViewModifiedAb[Vue - 1] = NULL;
				    }
			       }
			  }
			pDo1 = pDoc;
			if (nAssoc > 0)
			   /* vue d'elements associes */
			   if (debut)
			      /* on creera tout le volume de la vue d'un coup */
			      pDo1->DocAssocFreeVolume[nAssoc - 1] = pDo1->DocAssocVolume[nAssoc - 1];
			   else
			      /* on creera la moitie du volume max. derriere les */
			      /* nouveaux paves, et une autre moitie devant */
			      pDo1->DocAssocFreeVolume[nAssoc - 1] =
				 pDo1->DocAssocVolume[nAssoc - 1] / 2;
			/* vue de l'arbre principal */
			else if (debut)
			   pDo1->DocViewFreeVolume[Vue - 1] = pDo1->DocViewVolume[Vue - 1];
			else
			   pDo1->DocViewFreeVolume[Vue - 1] = pDo1->DocViewVolume[Vue - 1] / 2;
			/* marque comme anciens tous les paves conserves (pour */
			/* que AjoutePave travaille correctement) */
			PavRestant = DerCreVide;
			while (PavRestant != NULL)
			  {
			     pAbbox1 = PavRestant;
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
			     PavRestant = pAbbox1->AbEnclosing;
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
			     if (pEl1->ElSructSchema->SsPSchema->PsBuildAll[pEl1
							      ->ElTypeNumber - 1])
				/* cet element a la regle Gather */
				/* cree le pave avec toute sa descendance, si */
				/* ce n'est pas encore fait */
			       {
				  pPrevious = CreePaves (pAsc[i - 1], pDoc, Vue, TRUE, TRUE, &complet);
				  i = 1;
			       }
			     i--;
			  }
			while (!(i == 0));
			if (!debut)

			   /* cree d'autres paves devant, jusqu'a remplir le volume de */
			   /* la fenetre */
			  {
			     pDo1 = pDoc;
			     if (nAssoc > 0)
				pDo1->DocAssocFreeVolume[nAssoc - 1] =
				   pDo1->DocAssocVolume[nAssoc - 1] / 2;
			     else
				pDo1->DocViewFreeVolume[Vue - 1] =
				   pDo1->DocViewVolume[Vue - 1] / 2;
			     /* marque comme anciens tous les paves de presentation qui */
			     /* viennent d'etre crees par AddAbsBoxes devant les paves */
			     /* conserves. Ces paves de presentation seront ainsi traites */
			     /* correctement lors du prochain appel de AddAbsBoxes. */
			     PavRestant = DerCreVide;
			     while (PavRestant != NULL)
			       {
				  pPrevious = PavRestant->AbPrevious;
				  stop = FALSE;
				  do
				     if (pPrevious == NULL)
					stop = TRUE;
				     else if (pPrevious->AbElement != PavRestant->AbElement)
					stop = TRUE;
				     else
				       {
					  pPrevious->AbNew = FALSE;
					  pPrevious = pPrevious->AbPrevious;
				       }
				  while (!(stop));
				  PavRestant = PavRestant->AbEnclosing;
				  /* passe a l'englobant */
			       }
			     /* cree de nouveaux paves */
			     AddAbsBoxes (pAbbRoot, pDoc, TRUE);
			     /* marque comme nouveaux tous les paves de presentation qui */
			     /* viennent d'etre marques anciens. Ces paves de presentation */
			     /* seront ainsi traites correctement par le Mediateur. */
			     PavRestant = DerCreVide;
			     while (PavRestant != NULL)
			       {
				  pPrevious = PavRestant->AbPrevious;
				  stop = FALSE;
				  do
				     if (pPrevious == NULL)
					stop = TRUE;
				     else if (pPrevious->AbElement != PavRestant->AbElement)
					stop = TRUE;
				     else
				       {
					  pPrevious->AbNew = TRUE;
					  pPrevious = pPrevious->AbPrevious;
				       }
				  while (!(stop));
				  PavRestant = PavRestant->AbEnclosing;
				  /* passe a l'englobant */
			       }
			  }
			/* marque comme nouveaux tous les paves conserves (pour qu'ils */
			/* soient traites correctement par le Mediateur) */
			PavRestant = DerCreVide;
			while (PavRestant != NULL)
			  {
			     pAbbox1 = PavRestant;
			     pAbbox1->AbNew = TRUE;
			     PavRestant = pAbbox1->AbEnclosing;
			     /* passe a l'englobant */
			  }
			/* indique les nouvelles modifications au Mediateur et */
			/*  lui fait reafficher toute la vue */
			if (frame != 0)
			  {
			     h = 0;
			     ChangeConcreteImage (frame, &h, pAbbRoot);
			     if (affiche)
				DisplayFrame (frame);
			     /* il n'y a plus rien a reafficher dans cette vue */
			     if (nAssoc > 0)
				pDoc->DocAssocModifiedAb[nAssoc - 1] = NULL;
			     else
				pDoc->DocViewModifiedAb[Vue - 1] = NULL;
			  }
		     }
		}		/* V4 : fin cas vue non paginee , V3 fin Vue existe */
	   }			/* V4 : fin pEl != NULL , V3 : fin pEl->ElAbstractBox[] == NULL */
   }


/* ---------------------------------------------------------------------- */
/* |    VolumeTree    retourne                                        | */
/* |       VolAvant: le volume total des elements de l'arbre abstrait   | */
/* |                 qui precedent l'element auquel appartient le pave  | */
/* |                 pAbbFirst.                                           | */
/* |       VolApres: le volume total des elements de l'arbre abstrait   | */
/* |                 qui se trouvent apres l'element auquel appartient  | */
/* |                 le pave pAbbLast.                                    | */
/* |       VolArbre: le volume total de l'arbre abstrait.               | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
   void                VolumeTree (PtrAbstractBox pAbbRoot, PtrAbstractBox pAbbFirst, PtrAbstractBox pAbbLast, int *VolAvant, int *VolApres, int *VolArbre)
#else  /* __STDC__ */
   void                VolumeTree (pAbbRoot, pAbbFirst, pAbbLast, VolAvant, VolApres, VolArbre)
   PtrAbstractBox             pAbbRoot;
   PtrAbstractBox             pAbbFirst;
   PtrAbstractBox             pAbbLast;
   int                *VolAvant;
   int                *VolApres;
   int                *VolArbre;

#endif /* __STDC__ */
   {
      PtrElement          pEl;

      if (pAbbFirst == NULL || pAbbLast == NULL)
	 /* les parametres d'appels sont errone's */
	{
	   *VolAvant = 0;
	   *VolApres = 0;
	   *VolArbre = 0;
	}
      else
	{
	   /* calcule d'abord le volume total de l'arbre abstrait */
	   *VolArbre = pAbbRoot->AbElement->ElVolume;

	   /* on calcule maintenant le volume de ce qui precede */
	   *VolAvant = 0;
	   /* si le premier pave' correspond a un element associe' affiche' */
	   /* dans un haut ou bas de page, on prend le pave' saut de page */
	   while (pAbbFirst->AbElement->ElAssocNum !=
		  pAbbRoot->AbElement->ElAssocNum)
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
		     *VolAvant += pEl->ElVolume;
		  }
		/* passe a l'ascendant */
		pEl = pEl->ElParent;
	     }
	   while (pEl != NULL);

	   /* on calcule le volume de ce qui suit */
	   *VolApres = 0;
	   /* si le dernier pave' correspond a un element associe' affiche' */
	   /* dans un haut ou bas de page, on prend le pave' saut de page */
	   while (pAbbLast->AbElement->ElAssocNum !=
		  pAbbRoot->AbElement->ElAssocNum)
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
		     *VolApres += pEl->ElVolume;
		  }
		/* passe a l'ascendant */
		pEl = pEl->ElParent;
	     }
	   while (pEl != NULL);
	}
   }

/* ---------------------------------------------------------------------- */
/* |    JumpIntoView fait afficher dans la fenetre la partie de        | */
/* |            document qui se trouve a la distance indiquee du debut  | */
/* |            de l'arbre abstrait.                                    | */
/* |            distance est un pourcentage : 0 <= distance <= 100      | */
/* |            Si distance = 0, on fait afficher le debut de l'arbre.  | */
/* |            Si distance = 100, on fait afficher la fin de l'arbre.  | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
   void                JumpIntoView (int frame, int distance)

#else  /* __STDC__ */
   void                JumpIntoView (frame, distance)
   int                 frame;
   int                 distance;

#endif /* __STDC__ */

   {
      PtrDocument         pDoc;
      int                 Vue;
      boolean             Assoc;
      PtrElement          pEl;
      int                 VolAvant, VolCourant;
      boolean             apres;
      int                 position;
      PtrAbstractBox             PaveRacine, pAb;
      PtrElement          Suiv;

      position = 0;
      if (distance >= 0 && distance <= 100)
	 /* la distance demandee est valide */
	{
	   /* cherche le document et la vue correspondant a la fenetre */
	   DocVueFen (frame, &pDoc, &Vue, &Assoc);
	   if (pDoc != NULL)
	     {
		/* cherche la racine de l'arbre affiche' dans cette frame */
		if (Assoc)
		  {
		     pEl = pDoc->DocAssocRoot[Vue - 1];
		     Vue = 1;
		  }
		else
		   pEl = pDoc->DocRootElement;
		if (pEl != NULL)
		  {
		     PaveRacine = pEl->ElAbstractBox[Vue - 1];
		     /* volume avant l'element a rendre visible */
		     VolAvant = (pEl->ElVolume * distance) / 100;
		     /* cherche l'element a rendre visible */
		     VolCourant = 0;
		     while (!pEl->ElTerminal && pEl->ElFirstChild != NULL)
		       {
			  pEl = pEl->ElFirstChild;
			  while (VolCourant + pEl->ElVolume <= VolAvant
				 && pEl->ElNext != NULL)
			    {
			       VolCourant += pEl->ElVolume;
			       pEl = pEl->ElNext;
			    }
		       }
		     if (pEl == NULL)
			pAb = NULL;
		     else
		       {
			  pAb = pEl->ElAbstractBox[Vue - 1];
			  if (pAb == NULL)
			    {
			       VerifAbsBoxe (pEl, Vue, pDoc, FALSE, FALSE);
			       pAb = pEl->ElAbstractBox[Vue - 1];
			    }
		       }
		     apres = TRUE;
		     while (pEl != NULL && pAb == NULL)
			/* pEl n'a pas de pave dans le vue traitee. */
			/* On cherche la feuille la plus proche de pEl qui */
			/* soit visible dans la vue */
		       {
			  if (apres)
			     Suiv = NextElement (pEl);
			  else
			     Suiv = NULL;
			  if (Suiv != NULL)
			     /* on prend la feuille suivante */
			     if (!Suiv->ElTerminal)
				pEl = FirstLeaf (Suiv);
			     else
				pEl = Suiv;
			  else
			     /* on prend la feuille precedente */
			    {
			       apres = FALSE;
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
			       VerifAbsBoxe (pEl, Vue, pDoc, FALSE, FALSE);
			       pAb = pEl->ElAbstractBox[Vue - 1];
			    }
		       }

		     pAb = NULL;
		     /* positionne l'image dans sa frame, en l'affichant */
		     if (distance == 0)
			/* boite racine de la vue en haut de la fenetre */
		       {
			  position = 0;
			  pAb = PaveRacine;
		       }
		     else if (distance == 100)
			/* boite racine de la vue en bas de la fenetre */
		       {
			  position = 2;
			  pAb = PaveRacine;
		       }
		     else if (pEl != NULL)
			/* boite de l'element au milieu de la fenetre */
		       {
			  if (pEl->ElAbstractBox[Vue - 1] == NULL)
			     /* cherche le premier element precedent qui a */
			     /* un pave' dans la vue */
			     pEl = BackSearchVisibleElem (PaveRacine->AbElement, pEl, Vue);
			  if (pEl != NULL)
			     pAb = pEl->ElAbstractBox[Vue - 1];
			  position = 1;
		       }
		     if (pAb != NULL)
			MontrerBoite (frame, pAb->AbBox, position, 0);

		     /* Allume la selection */
		     AllumeSelection (FALSE, FALSE);
		  }
	     }
	}
   }

#ifdef __COLPAGE__

/** debut ajout */
/* affichage du contexte de pAb pour debug */

#ifdef __STDC__
   void                AffPaveDebug (PtrAbstractBox pAb)

#else  /* __STDC__ */
   void                AffPaveDebug (pAb)
   PtrAbstractBox             pAb;

#endif /* __STDC__ */

   {
      PtrElement          pEl;

      if (pAb != NULL)
	{
	   pEl = pAb->AbElement;
	   printf ("contexte pAb = %x", (int) pAb, " %s",
		   pEl->ElSructSchema->SsRule[pEl->ElTypeNumber - 1].SrName);
	   printf ("pres = %d", pAb->AbPresentationBox);
	   printf ("\n");
	   printf ("pere = %x", (int) pAb->AbEnclosing);
	   if (pAb->AbEnclosing != NULL)
	      printf (" %s",
		      pAb->AbEnclosing->AbElement->ElSructSchema->SsRule[pAb->AbEnclosing->AbElement->ElTypeNumber - 1].SrName);
	   printf ("\n");
	   printf ("frere avant = %x", (int) pAb->AbPrevious);
	   if (pAb->AbPrevious != NULL)
	      printf (" %s",
		      pAb->AbPrevious->AbElement->ElSructSchema->SsRule[pAb->AbPrevious->AbElement->ElTypeNumber - 1].SrName);
	   printf ("\n");
	   printf ("frere apres  = %x", (int) pAb->AbNext);
	   if (pAb->AbNext != NULL)
	      printf (" %s",
		      pAb->AbNext->AbElement->ElSructSchema->SsRule[pAb->AbNext->AbElement->ElTypeNumber
							     - 1].SrName);
	   printf ("\n");
	   if (pAb->AbPreviousRepeated != NULL)
	      printf (" dup precedent = %x", (int) pAb->AbPreviousRepeated);
	   if (pAb->AbNextRepeated != NULL)
	      printf (" dup suivant  = %x", (int) pAb->AbNextRepeated);
	   printf ("\n");
	}
   }

   /* ------------------------------------------------------------------ */
   /*  AbsBoxOk verifie les conditions d'invariance sur le pave pAb| */
   /*            et affiche un message d'erreur si une condition       | */
   /*            pas respectee. On verifie que :                       | */
   /*            1. la suite des paves de l'element pAb               | */
   /*               ne contienne pas que des paves de presentation,    | */
   /*           2. si pAb->AbPreviousRepeated != NULL                    | */
   /*               alors l'element ne pointe pas sur pAb             | */
   /*            3. si pAb->AbNextRepeated(ou Prec)  != NULL           | */
   /*               alors tous les ascendants doivent l'etre aussi     | */
   /* ------------------------------------------------------------------ */

#ifdef __STDC__
   boolean             AbsBoxOk (PtrAbstractBox pAb)

#else  /* __STDC__ */
   boolean             AbsBoxOk (pAb)
   PtrAbstractBox             pAb;

#endif /* __STDC__ */

   {
      PtrAbstractBox             pAbb, pAbbox1;
      PtrElement          pEl;
      boolean             correct, tousmorts;
      int                 AbDocView;

      pAbbox1 = pAb;
      AbDocView = pAb->AbDocView;
      /* a priori le pave est correct */
      correct = TRUE;
      tousmorts = TRUE;
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
		   tousmorts = FALSE;
		if (pAbb->AbNext != NULL)
		   if (pAbb->AbNext->AbElement == pAbb->AbElement)
		      pAbb = pAbb->AbNext;
		   else
		      pAbb = NULL;
		else
		   pAbb = NULL;
	     }
	   if (pAbb == NULL && !tousmorts)
	      /* erreur dans l'image abstraite */
	     {
		correct = FALSE;
		printf ("Erreur dans i.a : l'elem n'a que des paves pres %s",
		    pEl->ElSructSchema->SsRule[pEl->ElTypeNumber - 1].SrName);
		printf ("\n");
	     }
	}

      /* un pave de presentation ne peut pas etre duplique ?? */
      if (pAb->AbPresentationBox &&
	  (pAb->AbPreviousRepeated != NULL || pAb->AbNextRepeated != NULL))
	{
	   correct = FALSE;
	   printf ("Erreur dans l'i.a. : pave de pres duplique %s",
		   pEl->ElSructSchema->SsRule[pEl->ElTypeNumber - 1].SrName);
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
		    pEl->ElSructSchema->SsRule[pEl->ElTypeNumber - 1].SrName);
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
		    pEl->ElSructSchema->SsRule[pEl->ElTypeNumber - 1].SrName);
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
				  pAbb->AbElement->ElSructSchema->SsRule[pAbb->AbElement->ElTypeNumber
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
		     pEl->ElSructSchema->SsRule[pEl->ElTypeNumber - 1].SrName);
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

/* End Of Module imabs */
