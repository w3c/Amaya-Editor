
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

#include "arbabs.f"
#include "crimabs.f"
#include "crpages.f"
#include "def.f"
#include "debug.f"
#include "docvues.f"
#include "edit.f"
#include "imabs.f"
#include "imagepres.f"
#include "img.f"
#include "memory.f"
#include "modimabs.f"
#include "pres.f"
#include "select.f"
#include "textelem.f"

#define MaxAsc 30

static char         texte[MAX_TXT_LEN];

#ifdef __STDC__
extern void         AfficherVue (int);

#else  /* __STDC__ */
extern void         AfficherVue ();

#endif /* __STDC__ */

/* ---------------------------------------------------------------------- */
/* |    TypePave  rend un pointeur sur un buffer qui contient           | */
/* |            le type de l'element de structure auquel correspond le  | */
/* |            pave pointe' par pPav.                                  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
char               *TypePave (PtrAbstractBox pPav)

#else  /* __STDC__ */
char               *TypePave (pPav)
PtrAbstractBox             pPav;

#endif /* __STDC__ */

{
   SRule              *pRe1;
   PresentationBox             *pBo1;
   PtrElement          pEl1;

   if (pPav == NULL)
      strcpy (texte, " ");
   else
     {
	if (pPav->AbPresentationBox)
	  {
	     pRe1 = &pPav->AbElement->ElSructSchema->SsRule[pPav->AbElement->ElTypeNumber - 1];
	     /* copie le nom du type d'element structure qui a cree la boite */
	     strcpy (texte, pRe1->SrName);
	     strcat (texte, ".");
	     /* copie a la suite le nom du type de boite de presentation */
	     pBo1 = &pPav->AbPSchema->PsPresentBox[pPav->AbTypeNum - 1];
	     strcat (texte, pBo1->PbName);
	  }
	else
	   /* pave d'un element de structure */
	  {
	     pEl1 = pPav->AbElement;
	     strcpy (texte, pEl1->ElSructSchema->SsRule[pEl1->ElTypeNumber - 1].SrName);
	  }
     }
   return (texte);
}



/* ---------------------------------------------------------------------- */
/* |    LibPavVue libere, pour une seule vue, tous les paves            | */
/* |            englobes par le pave pointe par pPav, lui-meme compris. | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                LibPavVue (PtrAbstractBox pPav)

#else  /* __STDC__ */
void                LibPavVue (pPav)
PtrAbstractBox             pPav;

#endif /* __STDC__ */

{
   PtrAbstractBox             pP, pPSuiv;
   PtrTextBuffer      pBT, pBTSuiv;
   PtrDelayedPRule     pDelPR, pNextDelPR;
   PtrAbstractBox             pPa1;
   PtrElement          pEl1;
   boolean             libpav;

#ifdef __COLPAGE__
   boolean             ok;

#else  /* __COLPAGE__ */
   PresentationBox             *pBoite;
   PtrDocument         pDoc;
   int                 assoc;

#endif /* __COLPAGE__ */

   if (pPav != NULL)
     {
	pP = pPav->AbFirstEnclosed;
	/* libere tous les paves englobes */
	while (pP != NULL)
	  {
	     pPSuiv = pP->AbNext;
	     LibPavVue (pP);
	     pP = pPSuiv;
	  }
	pPa1 = pPav;
	if (pPa1->AbBox != NULL)
	  {
	     pEl1 = pPa1->AbElement;
	     printf ("Box non liberee: %s", pEl1->ElSructSchema->SsRule[pEl1->ElTypeNumber - 1].SrName);
	     if (pPa1->AbPresentationBox)
		printf (".%s\n", pPa1->AbPSchema->PsPresentBox[pPa1->AbTypeNum - 1].PbName);
	     else
		printf ("\n");
	  }
#ifdef __COLPAGE__
	/* debug */
	ok = PaveCorrect (pPav);
	if (!ok)
#endif /* __COLPAGE__ */
	   /* dechaine pPav des autres paves */
	   if (pPa1->AbNext != NULL)
	      pPa1->AbNext->AbPrevious = pPa1->AbPrevious;
	if (pPa1->AbPrevious != NULL)
	   pPa1->AbPrevious->AbNext = pPa1->AbNext;
	if (pPa1->AbEnclosing != NULL)
	   if (pPa1->AbEnclosing->AbFirstEnclosed == pPav)
	      pPa1->AbEnclosing->AbFirstEnclosed = pPa1->AbNext;
#ifdef __COLPAGE__
	if (pPa1->AbNextRepeated != NULL)
	   pPa1->AbNextRepeated->AbPreviousRepeated =
	      pPa1->AbPreviousRepeated;
	if (pPa1->AbPreviousRepeated != NULL)
	   pPa1->AbPreviousRepeated->AbNextRepeated =
	      pPa1->AbNextRepeated;
	/* debug */
	ok = PaveCorrect (pPav);
	if (!ok)
	   printf ("erreur apres dechainage LibPavVue \n");
#endif /* __COLPAGE__ */
	/* Si c'est un pave obtenu par la regle de presentation Copy,
	   libere le descripteur d'element copie' */
	if (pPa1->AbCopyDescr != NULL)
	  {
	     if (pPa1->AbCopyDescr->CdPrevious == NULL)
		pPa1->AbCopyDescr->CdCopiedElem->ElCopyDescr =
		   pPa1->AbCopyDescr->CdNext;
	     else
		pPa1->AbCopyDescr->CdPrevious->CdNext =
		   pPa1->AbCopyDescr->CdNext;
	     if (pPa1->AbCopyDescr->CdNext != NULL)
		pPa1->AbCopyDescr->CdNext->CdPrevious =
		   pPa1->AbCopyDescr->CdPrevious;
	     FreeDescCopie (pPa1->AbCopyDescr);
	  }
	/* si c'est un pave de presentation ou le pave d'une reference ou */
	/* celui d'une marque de paire, on libere les buffers */
	libpav = False;
	if (pPa1->AbPresentationBox)
	   if (pPa1->AbLeafType == LtText || pPa1->AbLeafType == LtPlyLine ||
	       pPa1->AbLeafType == LtPicture)
	      libpav = True;
	if (!libpav)
	   if (!pPa1->AbPresentationBox)
	      if (pPa1->AbElement != NULL)
		 if (pPa1->AbElement->ElTerminal)
		    if (pPa1->AbElement->ElLeafType == LtReference ||
			pPa1->AbElement->ElLeafType == LtPairedElem)
		       libpav = True;
	if (libpav)
	   switch (pPa1->AbLeafType)
		 {
		    case LtText:
		    case LtPlyLine:
		       if (pPa1->AbLeafType == LtPlyLine)
			  pBT = pPa1->AbPolyLineBuffer;
		       else
			  pBT = pPa1->AbText;
		       while (pBT != NULL)
			 {
			    pBTSuiv = pBT->BuNext;
			    SuppBufTexte (&pBT);
			    pBT = pBTSuiv;
			 }
		       break;
		    case LtPicture:
		       if (!pPa1->AbElement->ElTerminal || pPa1->AbElement->ElLeafType != LtPicture)
			 {
			    /* ce n'est pas un element image */
			    FreeImageDescriptor (pPa1->AbImageDescriptor);
			    pPa1->AbImageDescriptor = NULL;
			 }
		       break;
		    default:
		       break;
		 }
	/* dechaine pPav de son element */
	if (pPa1->AbElement->ElAbstractBox[pPa1->AbDocView - 1] == pPav)
#ifdef __COLPAGE__
	   if (!pPa1->AbPresentationBox)
	      /* si le pave pPav est le pave principal (non presentation) */
	      /* l'elt va pointer sur le dup (s'il existe) */
	      /* On ne considere pas les paves de pres rep suivants TODO ? */
	      pPa1->AbElement->ElAbstractBox[pPa1->AbDocView - 1] = pPa1->AbNextRepeated;
	   else
#endif /* __COLPAGE__ */
	   if (pPa1->AbNext != NULL)
	      if (pPa1->AbNext->AbElement == pPa1->AbElement)
		 pPa1->AbElement->ElAbstractBox[pPa1->AbDocView - 1] = pPa1->AbNext;
	      else
		 pPa1->AbElement->ElAbstractBox[pPa1->AbDocView - 1] = NULL;
	   else
#ifdef __COLPAGE__
	      /* inutile ? car un pave de pres ne peut avoir de dup */
	      pPa1->AbElement->ElAbstractBox[pPa1->AbDocView - 1] = pPa1->AbNextRepeated;
	/* fin du cas ou l'element pointait sur le pave */
#else  /* __COLPAGE__ */
	      pPa1->AbElement->ElAbstractBox[pPa1->AbDocView - 1] = NULL;
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
	if (pPa1->AbPresentationBox && pPa1->AbElement->ElTypeNumber == ord (PageBreak) + 1)
	  {
	     pBoite = &pPa1->AbPSchema->PsPresentBox[pPa1->AbTypeNum - 1];
	     if ((pBoite->PbPageHeader || pBoite->PbPageFooter) &&
		 pBoite->PbContent == ContElement)
		/* c'est bien une boite de haut ou bas de page contenant des */
		/* elements associes. Le pave est-il associe' a l'un des */
		/* elements associes du document ? */
	       {
		  pDoc = DocuDeElem (pPa1->AbElement);
		  if (pDoc != NULL)
		    {
		       for (assoc = 0; assoc < MAX_ASSOC_DOC; assoc++)
			  if (pDoc->DocAssocRoot[assoc] != NULL)
			     if (pDoc->DocAssocRoot[assoc]->ElAbstractBox[pPa1->AbDocView - 1] == pPa1)
				/* cet element n'a plus de pave */
				pDoc->DocAssocRoot[assoc]->ElAbstractBox[pPa1->AbDocView - 1] = NULL;
		    }
	       }
	  }
	else
	   /* est-ce un pave de presentation cree' par une regle FnCreateEnclosing */
	if (pPa1->AbPresentationBox)
	   if (pPa1->AbElement->ElAbstractBox[pPa1->AbDocView - 1] != NULL)
	      if (pPa1->AbElement->ElAbstractBox[pPa1->AbDocView - 1]->AbEnclosing == pPa1)
		 pPa1->AbElement->ElAbstractBox[pPa1->AbDocView - 1] = NULL;
/*** fin ajout ***/
#endif /* __COLPAGE__ */
#ifdef __COLPAGE__
	/* debug */
	ok = PaveCorrect (pPav);
	if (!ok)
	   printf ("erreur avant dechainage elem LibPavVue \n");
#endif /* __COLPAGE__ */
	/* libere les regles retardees qui n'ont pas ete appliquees */
	if (pPav->AbDelayedPRule != NULL)
	  {
	     pDelPR = pPav->AbDelayedPRule;
	     while (pDelPR != NULL)
	       {
		  pNextDelPR = pDelPR->DpNext;
		  FreeRRetard (pDelPR);
		  pDelPR = pNextDelPR;
	       }
	  }
	FreePave (pPav);
     }
}


/* ---------------------------------------------------------------------- */
/* |    LibPavElem libere, dans toutes les vues, tous les paves de      | */
/* |            l'element pointe par pEl.                               | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                LibPavElem (PtrElement pEl)

#else  /* __STDC__ */
void                LibPavElem (pEl)
PtrElement          pEl;

#endif /* __STDC__ */

#ifdef __COLPAGE__
{
   int                 v;
   PtrAbstractBox             pP, pPSuiv;
   PtrAbstractBox             pDupSuiv;
   boolean             stop;
   boolean             stop1;

   /* ce code ne traite que le cas du pave de l'element et de */
   /* paves de presentation avant et apres */
   /* prevoir cas des paves de presentation de la racine ?? TODO */
   if (pEl != NULL)
      for (v = 1; v <= MAX_VIEW_DOC; v++)
	{
	   pP = pEl->ElAbstractBox[v - 1];
	   if (pP != NULL)
	      if (pP->AbEnclosing != NULL)
		 if (pP->AbEnclosing->AbPresentationBox &&
		     pP->AbEnclosing->AbElement == pEl)
		    /* le pave englobant est un pave' de presentation cree' par */
		    /* une regle FnCreateEnclosing */
		    pP = pP->AbEnclosing;
	   stop1 = False;
	   pDupSuiv = NULL;
	   do
	     {
		stop = False;
		do
		   if (pP == NULL)
		      stop = True;
		   else if (pP->AbElement != pEl)
		      stop = True;
		   else
		     {
			pPSuiv = pP->AbNext;
			pDupSuiv = pP->AbNextRepeated;
			LibPavVue (pP);
			pP = pPSuiv;
		     }
		while (!(stop));
		if (pDupSuiv != NULL)	/* on parcourt la liste des dupliques */
		   pP = pDupSuiv;
		else
		   stop1 = True;
	     }
	   while (!stop1);
	}
}

#else  /* __COLPAGE__ */

{
   int                 v;
   PtrAbstractBox             pP, pPSuiv;
   boolean             stop;

   if (pEl != NULL)
      for (v = 1; v <= MAX_VIEW_DOC; v++)
	{
	   pP = pEl->ElAbstractBox[v - 1];
	   if (pP != NULL)
	      if (pP->AbEnclosing != NULL)
		 if (pP->AbEnclosing->AbPresentationBox &&
		     pP->AbEnclosing->AbElement == pEl)
		    /* le pave englobant est un pave' de presentation cree' par */
		    /* une regle FnCreateEnclosing */
		    pP = pP->AbEnclosing;
	   stop = False;
	   do
	      if (pP == NULL)
		 stop = True;
	      else if (pP->AbElement != pEl)
		 stop = True;
	      else
		{
		   pPSuiv = pP->AbNext;
		   LibPavVue (pP);
		   pP = pPSuiv;
		}
	   while (!(stop));
	}
}

#endif /* __COLPAGE__ */


/* ---------------------------------------------------------------------- */
/* |    LibPavMort libere tous les paves marques Mort dans le           | */
/* |            sous-arbre de racine pPav.                              | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                LibPavMort (PtrAbstractBox pPav)

#else  /* __STDC__ */
void                LibPavMort (pPav)
PtrAbstractBox             pPav;

#endif /* __STDC__ */

{
   PtrAbstractBox             pP, pPSuiv;


   if (pPav != NULL)
      if (pPav->AbDead)
	 LibPavVue (pPav);
      else
	{
	   pP = pPav->AbFirstEnclosed;
	   /* teste tous les paves englobes */
	   while (pP != NULL)
	     {
		pPSuiv = pP->AbNext;
		LibPavMort (pP);
		pP = pPSuiv;
	     }
	}
}

#ifdef __COLPAGE__
 /* procedure de reevaluation des regles recursives */
 /* ---------------------------------------------------------- */
 /* |     RecursEvalCP appelle NouvRfPave pour tous les paves | */
 /* |                du sous-arbre pPav                     | */
 /* ---------------------------------------------------------- */


#ifdef __STDC__
void                RecursEvalCP (PtrAbstractBox pPav, PtrDocument pDoc)

#else  /* __STDC__ */
void                RecursEvalCP (pPav, pDoc)
PtrAbstractBox             pPav;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrAbstractBox             pP, PavR;

   pP = pPav;
   if (pP != NULL)
     {
	NouvRfPave (pP, pP, &PavR, pDoc);
	pP = pP->AbFirstEnclosed;
	while (pP != NULL)
	  {
	     RecursEvalCP (pP, pDoc);
	     pP = pP->AbNext;
	  }
     }
}
 /*fin de procedure RecursEval */
#endif /* __COLPAGE__ */


/* ---------------------------------------------------------------------- */
/* |    AjoutePaves complete la vue dont PavRacine est le pave racine   | */
/* |            en ajoutant des paves, en tete si Tete est vrai,        | */
/* |            en queue sinon.                                         | */
 /*        PavRacine est une vraie racine de paves               | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                AjoutePaves (PtrAbstractBox PavRacine, PtrDocument pDoc, boolean Tete)

#else  /* __STDC__ */
void                AjoutePaves (PavRacine, pDoc, Tete)
PtrAbstractBox             PavRacine;
PtrDocument         pDoc;
boolean             Tete;

#endif /* __STDC__ */

#ifdef __COLPAGE__
{
   PtrAbstractBox             pPav;
   PtrAbstractBox             pPa1;
   PtrElement          pEl;
   boolean             complet;

   if ((Tete && PavRacine->AbTruncatedHead) || (!Tete && PavRacine->AbTruncatedTail))
     {
	/* cree les paves de la partie coupee jusqu'a concurrence du volume libre */
	pEl = PavRacine->AbElement;
	pPa1 = PavRacine;
	pPav = CreePaves (pEl, pDoc, pPa1->AbDocView, !Tete, True, &complet);
	/* on reapplique les regles  a tous les paves */
	/* TO DO a affiner ! */ RecursEvalCP (PavRacine, pDoc);
	if (VueAssoc (pEl))
	   pDoc->DocAssocModifiedAb[pEl->ElAssocNum - 1] = PavRacine;
	else
	   pDoc->DocViewModifiedAb[PavRacine->AbDocView - 1] = PavRacine;
     }
}

#else  /* __COLPAGE__ */

{
   PtrAbstractBox             pPav, PavReaff, PavR, PcFirst, PcLast, pP;
   boolean             stop;
   PtrAbstractBox             pPa1;
   PtrDocument         pDo1;
   PtrElement          pEl1;
   boolean             complet;

   if ((Tete && PavRacine->AbTruncatedHead) || (!Tete && PavRacine->AbTruncatedTail))
     {
	/* cree les paves de la partie coupee jusqu'a concurence du volume libre */
	pPa1 = PavRacine;
	pPav = CreePaves (pPa1->AbElement, pDoc, pPa1->AbDocView, !Tete, True, &complet);
	/* recherche tous les paves crees, a partir du premier pave de plus */
	/* haut niveau cree', et aux niveaux inferieurs. */
	while (pPav != NULL)
	  {
	     /* cherche le premier et le dernier freres crees a ce niveau */
	     PcFirst = pPav;
	     PcLast = pPav;
	     stop = False;
	     do
		if (PcLast->AbNext == NULL)
		   stop = True;
		else if (!PcLast->AbNext->AbNew)
		   stop = True;
		else
		   PcLast = PcLast->AbNext;
	     while (!(stop));
	     stop = False;
	     do
		if (PcFirst->AbPrevious == NULL)
		   stop = True;
		else if (!PcFirst->AbPrevious->AbNew)
		   stop = True;
		else
		   PcFirst = PcFirst->AbPrevious;
	     while (!(stop));
	     if (PcFirst == PcLast)
		/* un seul pave cree a ce niveau, c'est lui qu'il faudra */
		/* reafficher. */
		PavReaff = pPav;
	     else
		/* plusieurs paves crees, on reaffichera l'englobant */
		PavReaff = pPav->AbEnclosing;
	     /* modifie les paves environnant les paves crees */
	     NouvRfPave (PcFirst, PcLast, &PavR, pDoc);
	     PavReaff = Englobant (PavReaff, PavR);	/* conserve le pointeur sur le pave a reafficher */

	     pDo1 = pDoc;
	     pEl1 = PavRacine->AbElement;
	     if (VueAssoc (PavRacine->AbElement))
		pDo1->DocAssocModifiedAb[pEl1->ElAssocNum - 1] =
		   Englobant (PavReaff, pDo1->DocAssocModifiedAb[pEl1->ElAssocNum - 1]);
	     else
		pDo1->DocViewModifiedAb[PavRacine->AbDocView - 1] =
		   Englobant (PavReaff, pDo1->DocViewModifiedAb[PavRacine->AbDocView - 1]);
	     /* passe au niveau inferieur */
	     if (Tete)
		pPav = PcLast->AbNext;
	     else
		pPav = PcFirst->AbPrevious;
	     if (pPav != NULL)
	       {
		  /* saute les paves de presentation produits par CreateWith */
		  if (Tete)
		     while (pPav->AbPresentationBox && pPav->AbNext != NULL)
			pPav = pPav->AbNext;
		  else
		     while (pPav->AbPresentationBox && pPav->AbPrevious != NULL)
			pPav = pPav->AbPrevious;
		  /* passe au premier pave fils */
		  pPav = pPav->AbFirstEnclosed;
		  if (pPav != NULL)
		     if (Tete)
			if (pPav->AbNew)
			  {
			     /* verifie s'il y a un pave ancien a ce niveau */
			     pP = pPav;
			     stop = False;
			     do
				if (pP == NULL)
				  {
				     /* pas d'ancien, on s'arrete si l'englobant est */
				     /* lui-meme un nouveau pave */
				     if (pPav->AbEnclosing->AbNew)
					pPav = NULL;
				     stop = True;
				  }
				else if (!pP->AbNew)
				   stop = True;
				else
				   pP = pP->AbNext;
			     while (!(stop));
			  }
			else
			  {
			     /* cherche le premier niveau ou il y a un nouveau en tete */
			     stop = False;
			     do
				if (pPav == NULL)
				   stop = True;
				else if (pPav->AbNew)
				   stop = True;
				else
				  {
				     /* saute les paves de presentation produits par CreateWith */
				     while (pPav->AbPresentationBox && pPav->AbNext != NULL)
					pPav = pPav->AbNext;
				     pPav = pPav->AbFirstEnclosed;
				  }
			     while (!(stop));
			  }
		     else
		       {
			  /* on a ajoute' en queue */
			  /* on cherche le premier cree' a ce niveau */
			  if (pPav->AbNew)
			    {
			       /* il n'y a que des paves nouveaux, on arrete si */
			       /* l'englobant est lui-meme nouveau */
			       if (pPav->AbEnclosing->AbNew)
				  pPav = NULL;
			    }
			  else
			    {
			       stop = False;
			       do
				  if (pPav == NULL)
				     stop = True;
				  else if (pPav->AbNew)
				     stop = True;
				  else if (pPav->AbNext == NULL)
				    {
				       /* saute les paves de presentation produits par CreateWith */
				       while (pPav->AbPresentationBox && pPav->AbPrevious != NULL)
					  pPav = pPav->AbPrevious;
				       pPav = pPav->AbFirstEnclosed;
				    }
				  else
				     pPav = pPav->AbNext;
			       while (!(stop));
			    }
			  /* cherche le dernier  cree' */
			  if (pPav != NULL)
			     while (pPav->AbNext != NULL)
				pPav = pPav->AbNext;
		       }
	       }
	  }
     }
}

#endif /* __COLPAGE__ */


/* ---------------------------------------------------------------------- */
/* |    NonSecable retourne vrai si le pave pointe par pPav est         | */
/* |            englobe (a n'importe quel niveau) par un pave           | */
/* |            mis en ligne ou explicitement non secable. Les marques  | */
/* |            de page son traitees comme non-secables.                | */
/* |**CP*       V4 : Les paves de page sont traites comme secables.     | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
boolean             NonSecable (PtrAbstractBox pPav)

#else  /* __STDC__ */
boolean             NonSecable (pPav)
PtrAbstractBox             pPav;

#endif /* __STDC__ */

{
   boolean             retour;
   int                 Entree;
   PtrPSchema          pSchP;
   PtrSSchema        pSchS;
   PtrAbstractBox             pPa1;

   retour = False;
   /* a priori le pave est secable */
   /* boucle sur les paves englobants */
   while (pPav != NULL && !retour)
     {
	pPa1 = pPav;
#ifndef __COLPAGE__
	if (pPa1->AbLeafType == LtCompound)
	   /* pave' compose' */
	   if (pPa1->AbElement->ElTypeNumber == ord (PageBreak) + 1)
	      /* c'est une marque de saut de page, non-secable */
	      retour = True;
	   else
	      /* un pave compose' est non-secable s'il est mis en lignes */
	      retour = pPa1->AbInLine;
	/* regarde dans le schema de presentation du pave s'il est secable */
	if (!retour)
#endif /* __COLPAGE__ */
	  {
	     ChSchemaPres (pPa1->AbElement, &pSchP, &Entree, &pSchS);
	     retour = pSchP->PsBuildAll[Entree - 1];
	  }
	pPav = pPa1->AbEnclosing;
	/* passe a l'englobant */
     }
   return retour;
}

#ifdef __COLPAGE__

/*------------------------------------------------------------------------------*/
 /*TuePresDroite tue tous les paves de presentation a droite de pPav     */
 /*              on ne considere que les paves a l'interieur d'une page */
 /*             sauf les REPEATED                                      */
/*------------------------------------------------------------------------------*/



#ifdef __STDC__
void                TuePresDroite (PtrAbstractBox pPav, PtrDocument pDoc)

#else  /* __STDC__ */
void                TuePresDroite (pPav, pDoc)
PtrAbstractBox             pPav;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrAbstractBox             PavEnglob, pP, PavR;
   boolean             stop;

   if (pPav->AbElement->ElTypeNumber != ord (PageBreak) + 1)
     {
	PavEnglob = pPav->AbEnclosing;
	while (PavEnglob != NULL && !(PavEnglob->AbTruncatedTail))
	  {
	     /* on saute le pave corps de page, pour ne pas detruire ses paves */
	     /* de presentation */
	     if (!(PavEnglob->AbElement->ElTerminal
		   && PavEnglob->AbElement->ElLeafType == LtPageColBreak))
	       {
		  PavEnglob->AbTruncatedTail = True;
		  /* cherche et supprime les paves crees par CreateLast */
		  /* si PavEnglob est la racine, ses paves de presentation */
		  /* sont sous le dernier pave corps de page */
		  if (PavEnglob == pDoc->DocViewRootAb[pPav->AbDocView - 1])
		    {
		       pP = PavEnglob->AbFirstEnclosed;
		       if (pP != NULL && pP->AbElement->ElTerminal
			   && pP->AbElement->ElLeafType == LtPageColBreak)
			  /* le document est mis en page */
			 {
			    /* on se place sur le dernier pave */
			    while (pP->AbNext != NULL)
			       pP = pP->AbNext;
			    while (pP->AbPresentationBox)
			       pP = pP->AbPrevious;	/* on saut le bas de page */
			    /* et le filet */
			    pP = pP->AbFirstEnclosed;	/* 1er element du corps de page */
			    /* TODO descendre les paves de colonne */
			 }
		    }
		  else
		     pP = PavEnglob->AbFirstEnclosed;
		  /* cherche d'abord le dernier pave englobe */
		  stop = False;
		  if (pP != NULL)
		     do
			if (pP->AbNext == NULL)
			   stop = True;
			else
			   pP = pP->AbNext;
		     while (!(stop));
		  stop = False;
		  do
		     if (pP == NULL)
			stop = True;
		     else if (!pP->AbPresentationBox || pP->AbDead
			      || pP->AbElement != PavEnglob->AbElement)
			stop = True;
		     else
		       {
			  if (!pP->AbRepeatedPresBox)
			    {
			       TuePave (pP);
			       /* traite les paves qui se referent au pave detruit */
			       SuppRfPave (pP, &PavR, pDoc);
			    }
			  pP = pP->AbPrevious;
		       }
		  while (!(stop));
		  /* cherche et supprime les paves crees par CreateAfter */
		  pP = PavEnglob->AbNext;
		  stop = False;
		  do
		     if (pP == NULL)
			stop = True;
		     else if (!pP->AbPresentationBox
			      || pP->AbDead
			      || pP->AbElement != PavEnglob->AbElement)
			stop = True;
		     else
		       {
			  if (!pP->AbRepeatedPresBox)
			    {
			       TuePave (pP);
			       /* traite les paves qui se referent au pave detruit */
			       SuppRfPave (pP, &PavR, pDoc);
			    }
			  pP = pP->AbNext;
		       }
		  while (!(stop));
	       }
	     /* passe au niveau superieur */
	     PavEnglob = PavEnglob->AbEnclosing;
	  }
     }
}				/* fin TuePresDroite */
#endif /* __COLPAGE__ */

#ifdef __COLPAGE__

 /* -------------------------------------------------------- */
 /* |  DetrPaveSuivants detruit les paves a partir         | */
 /* |               de pPav                               | */
 /* -------------------------------------------------------- */


#ifdef __STDC__
void                DetrPaveSuivants (PtrAbstractBox pPav, PtrDocument pDoc)

#else  /* __STDC__ */
void                DetrPaveSuivants (pPav, pDoc)
PtrAbstractBox             pPav;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrAbstractBox             PavRac, pP2, pP1, PavR;

   PavRac = NULL;
   pP1 = pPav;
   if (pP1 != NULL
       && pP1->AbEnclosing != NULL
       && !(pP1->AbElement->ElTypeNumber == ord (PageBreak) + 1
	    && (pP1->AbElement->ElPageType == PgBegin
		|| pP1->AbElement->ElPageType == PgComputed
		|| pP1->AbElement->ElPageType == PgUser)))
      /* on ne tue pas les paves page, mais on tue les paves */
      /* de colonne */
     {
	/* pP1 est le premier pave a detruire de la page courante */
	/* on tue tous les paves de presentation a droite */
	/* en mettant a jour le booleen AbTruncatedTail */
	TuePresDroite (pP1, pDoc);
	/* on tue pP1 */
	TuePave (pP1);
	SuppRfPave (pP1, &PavR, pDoc);
	/* on tue tous les paves a droite en remontant l'arbre */
	while (pP1->AbEnclosing != NULL
	       && !(pP1->AbElement->ElTypeNumber == ord (PageBreak) + 1
		    && (pP1->AbElement->ElPageType == PgBegin
			|| pP1->AbElement->ElPageType == PgComputed
			|| pP1->AbElement->ElPageType == PgUser)))
	   /* on ne tue pas les paves page, mais on tue les paves */
	   /* de colonne */
	  {
	     pP2 = pP1->AbNext;
	     while (pP2 != NULL)
	       {
		  TuePave (pP2);
		  SuppRfPave (pP2, &PavR, pDoc);
		  pP2 = pP2->AbNext;
	       }
	     pP1 = pP1->AbEnclosing;
	  }
	/* pP1 est un pave page (en general le corps de page, sauf si */
	/* pEl est lui-meme un element marque de page) */
	if (pP1 != NULL)	/* toujours vrai ? */
	   /* on saute les paves de */
	   /* presentation de la page courante: bas et filet */
	  {
	     pP2 = pP1;
	     while (pP2->AbNext != NULL
		    && pP2->AbNext->AbElement == pP1->AbElement)
		pP2 = pP2->AbNext;	/* on saute les paves de cette page */
	     pP2 = pP2->AbNext;	/* premier pave de la page suivante */
	     /* on memorise le pave racine */
	     PavRac = pP1->AbEnclosing;
	  }
	pP1 = pP2;		/* pP1 1er pave de la page suivante ou null si */
	/* pas de page suivante */

     }
   if (pP1 != NULL)		/* il existe une page suivante */
      /* on la detruit (et les suivantes) */
     {
	PavRac = pP1->AbEnclosing;
	while (pP1 != NULL)
	  {
	     TuePave (pP1);
	     SuppRfPave (pP1, &PavR, pDoc);
	     pP1 = pP1->AbNext;
	  }
     }
   if (PavRac != NULL)
      /* on marque la racine coupee en queue */
      PavRac->AbTruncatedTail = True;
}				/* fin DetrPaveSuivants */
#endif /* __COLPAGE__ */


#ifdef __COLPAGE__
/* ---------------------------------------------------------------------- */
/* |    Procedure differente dans V4 : booleen SaufRep a la place de    | */
/* |    SaufCreeAvec                                                    | */
/* ---------------------------------------------------------------------- */
/* |    TuePresVoisin detruit les paves de presentation crees par les   | */
/* |    regles CreateBefore et CreateAfter de pPav.                     | */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
void                TuePresVoisin (PtrAbstractBox pVoisin, boolean Avant, PtrDocument pDoc, PtrAbstractBox * PavR, PtrAbstractBox * PavReaff, int *volsupp, PtrAbstractBox pPav, boolean SaufRep)

#else  /* __STDC__ */
void                TuePresVoisin (pVoisin, Avant, pDoc, PavR, PavReaff, volsupp, pPav, SaufRep)
PtrAbstractBox             pVoisin;
boolean             Avant;
PtrDocument         pDoc;
PtrAbstractBox            *PavR;
PtrAbstractBox            *PavReaff;
int                *volsupp;
PtrAbstractBox             pPav;
boolean             SaufRep;

#endif /* __STDC__ */

{
   boolean             stop;

   stop = False;
   do
      if (pVoisin == NULL)
	 stop = True;
      else if (pVoisin->AbElement != pPav->AbElement)
	 stop = True;
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
	   if (Avant)
	      pVoisin = pVoisin->AbPrevious;
	   else
	      pVoisin = pVoisin->AbNext;
	}
   while (!(stop));
}

#else  /* __COLPAGE__ */

/* ---------------------------------------------------------------------- */
/* |    TuePresVoisin detruit les paves de presentation crees par les   | */
/* |    regles CreateBefore et CreateAfter de pPav.                     | */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
static void         TuePresVoisin (PtrAbstractBox pVoisin, boolean Avant, PtrDocument pDoc, PtrAbstractBox * PavR, PtrAbstractBox * PavReaff, int *volsupp, PtrAbstractBox pPav, boolean SaufCreeAvec)

#else  /* __STDC__ */
static void         TuePresVoisin (pVoisin, Avant, pDoc, PavR, PavReaff, volsupp, pPav, SaufCreeAvec)
PtrAbstractBox             pVoisin;
boolean             Avant;
PtrDocument         pDoc;
PtrAbstractBox            *PavR;
PtrAbstractBox            *PavReaff;
int                *volsupp;
PtrAbstractBox             pPav;
boolean             SaufCreeAvec;

#endif /* __STDC__ */

{
   boolean             stop;

   stop = False;
   do
      if (pVoisin == NULL)
	 stop = True;
      else if (pVoisin->AbElement != pPav->AbElement)
	 stop = True;
      else
	{
	   if (pVoisin->AbPresentationBox)
	      if (!SaufCreeAvec || RegleCree (pDoc, pPav, pVoisin) != FnCreateWith)
		{
		   *volsupp += pVoisin->AbVolume;
		   TuePave (pVoisin);
		   *PavReaff = Englobant (*PavReaff, pVoisin);
		   /* traite les paves qui se referent au pave detruit */
		   SuppRfPave (pVoisin, PavR, pDoc);
		   *PavReaff = Englobant (*PavReaff, *PavR);
		}
	   if (Avant)
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
/* |    TuePresEnglob supprime tous les paves de presentation           | */
/* |            crees par pPav et les paves de presentation crees par   | */
/* |            les paves englobants a l'aide de regles CreateFirst et  | */
/* |            CreateBefore (si Tete est vrai) ou CreateAfter et       | */
/* |            CreateLast (si Tete est faux).                          | */
/* |            Au retour volsupp indique le volume des paves de        | */
/* |            presentation tues et PavReaff le pave a reafficher.     | */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
static void         TuePresEnglob (PtrAbstractBox pPav, boolean Tete, PtrDocument pDoc, PtrAbstractBox * PavReaff, int *volsupp, boolean SaufRep)

#else  /* __STDC__ */
static void         TuePresEnglob (pPav, Tete, pDoc, PavReaff, volsupp, SaufRep)
PtrAbstractBox             pPav;
boolean             Tete;
PtrDocument         pDoc;
PtrAbstractBox            *PavReaff;
int                *volsupp;
boolean             SaufRep;

#endif /* __STDC__ */

{
   PtrAbstractBox             PavEnglob, pP, PavR;
   boolean             stop;

   *volsupp = 0;
   /* Detruit les paves de presentation crees par les regles */
   /* CreateBefore et CreateAfter de pPav. */
   TuePresVoisin (pPav->AbPrevious, True,
		  pDoc, &PavR, PavReaff, volsupp, pPav, SaufRep);
   TuePresVoisin (pPav->AbNext, False,
		  pDoc, &PavR, PavReaff, volsupp, pPav, SaufRep);
   /* traite les paves englobants */
   PavEnglob = pPav->AbEnclosing;
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
		   PavEnglob->AbTruncatedHead = True;
		   /* si PavEnglob est la racine, ses paves de presentation */
		   /* sont sous le pave corps de page ou de colonne */
		   if (PavEnglob->AbEnclosing == NULL)
		     {
			pP = PavEnglob->AbFirstEnclosed;
			if (pP != NULL && pP->AbElement->ElTerminal
			    && pP->AbElement->ElLeafType == LtPageColBreak)
			   /* le document est mis en page */
			   while (pP->AbElement->ElTypeNumber == ord (PageBreak) + 1)
			     {
				/* on saute les paves de pres page et colonnes */
				while (pP->AbPresentationBox)
				   pP = pP->AbNext;
				pP = pP->AbFirstEnclosed;
				/* pP : premier pave du corps de page ou colonne */
				/* on suppose qu'il n'y a pas de creer first pour */
				/* les pages et colonnes TODO a controler dans prs */
			     }
		     }
		   else
		      pP = PavEnglob->AbFirstEnclosed;
		   /* cherche et supprime les paves crees par CreateFirst */
		   /* mais ne supprime pas ceux qui sont repetes */
		   stop = False;
		   do
		      if (pP == NULL)
			 stop = True;
		      else if (!pP->AbPresentationBox || pP->AbDead
			       || pP->AbElement != PavEnglob->AbElement)
			 /* suppression du saut des marques de page de debut d'element */
			 stop = True;
		      else
			{
			   if (!pP->AbRepeatedPresBox)
			     {
				*volsupp += pP->AbVolume;
				TuePave (pP);
				*PavReaff = Englobant (*PavReaff, pP);
				/* traite les paves qui se referent au pave detruit */
				SuppRfPave (pP, &PavR, pDoc);
				*PavReaff = Englobant (*PavReaff, PavR);
			     }
			   pP = pP->AbNext;
			}
		   while (!(stop));
		   /* cherche et supprime les paves crees par CreateBefore */
		   /* mais ne supprime pas ceux qui sont repetes */
		   pP = PavEnglob->AbPrevious;
		   stop = False;
		   do
		      if (pP == NULL)
			 stop = True;
		      else if (!pP->AbPresentationBox || pP->AbDead
			       || pP->AbElement != PavEnglob->AbElement)
			 stop = True;
		      else
			{
			   if (!pP->AbRepeatedPresBox)
			     {
				*volsupp += pP->AbVolume;
				TuePave (pP);
				*PavReaff = Englobant (*PavReaff, pP);
				/* traite les paves qui se referent au pave detruit */
				SuppRfPave (pP, &PavR, pDoc);
				*PavReaff = Englobant (*PavReaff, PavR);
			     }
			   pP = pP->AbPrevious;
			}
		   while (!(stop));
		}
	      else
		{
		   /* Tete = False */
		   PavEnglob->AbTruncatedTail = True;
		   /* cherche et supprime les paves crees par CreateLast */
		   /* mais ne supprime pas ceux qui sont repetes */
		   /* si PavEnglob est la racine, ses paves de presentation */
		   /* sont sous le dernier pave corps de page */
		   if (PavEnglob->AbEnclosing == NULL)
		     {
			pP = PavEnglob->AbFirstEnclosed;
			if (pP != NULL && pP->AbElement->ElTerminal
			    && pP->AbElement->ElLeafType == LtPageColBreak)
			   /* le document est mis en page */
			   /* on se place sur le dernier pave non page ou colonne */
			   while (pP != NULL && pP->AbElement->ElTypeNumber == ord (PageBreak) + 1)
			     {
				while (pP->AbNext != NULL)
				   pP = pP->AbNext;
				while (pP->AbPresentationBox)
				   pP = pP->AbPrevious;
				pP = pP->AbFirstEnclosed;
			     }
		     }
		   else
		      pP = PavEnglob->AbFirstEnclosed;
		   /* cherche d'abord le dernier pave englobe */
		   stop = False;
		   if (pP != NULL)
		      do
			 if (pP->AbNext == NULL)
			    stop = True;
			 else
			    pP = pP->AbNext;
		      while (!(stop));
		   stop = False;
		   do
		      if (pP == NULL)
			 stop = True;
		      else if (!pP->AbPresentationBox || pP->AbDead
			       || pP->AbElement != PavEnglob->AbElement)
			 stop = True;
		      else
			{
			   if (!pP->AbRepeatedPresBox)
			     {
				*volsupp += pP->AbVolume;
				TuePave (pP);
				*PavReaff = Englobant (*PavReaff, pP);
				/* traite les paves qui se referent au pave detruit */
				SuppRfPave (pP, &PavR, pDoc);
				*PavReaff = Englobant (*PavReaff, PavR);
			     }
			   pP = pP->AbPrevious;
			}
		   while (!(stop));
		   /* cherche et supprime les paves crees par CreateAfter */
		   /* mais ne supprime pas ceux crees par CreateWith */
		   pP = PavEnglob->AbNext;
		   stop = False;
		   do
		      if (pP == NULL)
			 stop = True;
		      else if (!pP->AbPresentationBox
			       || pP->AbDead
			       || pP->AbElement != PavEnglob->AbElement)
			 stop = True;
		      else
			{
			   if (!pP->AbRepeatedPresBox)
			     {
				*volsupp += pP->AbVolume;
				TuePave (pP);
				*PavReaff = Englobant (*PavReaff, pP);
				/* traite les paves qui se referent au pave detruit */
				SuppRfPave (pP, &PavR, pDoc);
				*PavReaff = Englobant (*PavReaff, PavR);
			     }
			   pP = pP->AbNext;
			}
		   while (!(stop));
		}
	/* passe au niveau superieur */
	PavEnglob = PavEnglob->AbEnclosing;
     }
}

#else  /* __COLPAGE__ */

/* ---------------------------------------------------------------------- */
/* |    TuePresEnglob supprime tous les paves de presentation           | */
/* |            crees par pPav et les paves de presentation crees par   | */
/* |            les paves englobants a l'aide de regles Create et       | */
/* |            CreateBefore (si Tete est vrai) ou CreateAfter et       | */
/* |            CreateLast (si Tete est faux).                          | */
/* |            Au retour volsupp indique le volume des paves de        | */
/* |            presentation tues et PavReaff le pave a reafficher.     | */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
static void         TuePresEnglob (PtrAbstractBox pPav, boolean Tete, PtrDocument pDoc, PtrAbstractBox * PavReaff, int *volsupp, boolean SaufCreeAvec)

#else  /* __STDC__ */
static void         TuePresEnglob (pPav, Tete, pDoc, PavReaff, volsupp, SaufCreeAvec)
PtrAbstractBox             pPav;
boolean             Tete;
PtrDocument         pDoc;
PtrAbstractBox            *PavReaff;
int                *volsupp;
boolean             SaufCreeAvec;

#endif /* __STDC__ */

{
   PtrAbstractBox             PavEnglob, pP, PavR;
   boolean             stop;
   PtrElement          pEl1;

   *volsupp = 0;
   /* Detruit les paves de presentation crees par les regles */
   /* CreateBefore et CreateAfter de pPav. */
   TuePresVoisin (pPav->AbPrevious, True,
		  pDoc, &PavR, PavReaff, volsupp, pPav, SaufCreeAvec);
   TuePresVoisin (pPav->AbNext, False,
		  pDoc, &PavR, PavReaff, volsupp, pPav, SaufCreeAvec);
   /* traite les paves englobants */
   PavEnglob = pPav->AbEnclosing;
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
		   PavEnglob->AbTruncatedHead = True;
		   /* cherche et supprimes les paves crees par CreateFirst */
		   pP = PavEnglob->AbFirstEnclosed;
		   stop = False;
		   do
		      if (pP == NULL)
			 stop = True;
		      else if (!pP->AbPresentationBox || pP->AbDead
			       || pP->AbElement != PavEnglob->AbElement)
			 /* saute les marques de page de debut d'element */
			{
			   pEl1 = pP->AbElement;
			   if (pEl1->ElTypeNumber == ord (PageBreak) + 1
			       && pEl1->ElPageType == PgBegin)
			      pP = pP->AbNext;
			   else
			      stop = True;
			}
		      else
			{
			   *volsupp += pP->AbVolume;
			   TuePave (pP);
			   *PavReaff = Englobant (*PavReaff, pP);
			   /* traite les paves qui se referent au pave detruit */
			   SuppRfPave (pP, &PavR, pDoc);
			   *PavReaff = Englobant (*PavReaff, PavR);
			   pP = pP->AbNext;
			}
		   while (!(stop));
		   /* cherche et supprime les paves crees par CreateBefore */
		   /* mais ne supprime pas ceux crees par CreateWith */
		   pP = PavEnglob->AbPrevious;
		   stop = False;
		   do
		      if (pP == NULL)
			 stop = True;
		      else if (!pP->AbPresentationBox || pP->AbDead
			       || pP->AbElement != PavEnglob->AbElement)
			 stop = True;
		      else
			{
			   if (RegleCree (pDoc, PavEnglob, pP) != FnCreateWith)
			     {
				*volsupp += pP->AbVolume;
				TuePave (pP);
				*PavReaff = Englobant (*PavReaff, pP);
				/* traite les paves qui se referent au pave detruit */
				SuppRfPave (pP, &PavR, pDoc);
				*PavReaff = Englobant (*PavReaff, PavR);
			     }
			   pP = pP->AbPrevious;
			}
		   while (!(stop));
		}
	      else
		{
		   /* Tete = False */
		   PavEnglob->AbTruncatedTail = True;
		   /* cherche et supprime les paves crees par CreateLast */
		   pP = PavEnglob->AbFirstEnclosed;
		   /* cherche d'abord le dernier pave englobe */
		   stop = False;
		   if (pP != NULL)
		      do
			 if (pP->AbNext == NULL)
			    stop = True;
			 else
			    pP = pP->AbNext;
		      while (!(stop));
		   stop = False;
		   do
		      if (pP == NULL)
			 stop = True;
		      else if (!pP->AbPresentationBox || pP->AbDead
			       || pP->AbElement != PavEnglob->AbElement)
			 stop = True;
		      else
			{
			   *volsupp += pP->AbVolume;
			   TuePave (pP);
			   *PavReaff = Englobant (*PavReaff, pP);
			   /* traite les paves qui se referent au pave detruit */
			   SuppRfPave (pP, &PavR, pDoc);
			   *PavReaff = Englobant (*PavReaff, PavR);
			   pP = pP->AbPrevious;
			}
		   while (!(stop));
		   /* cherche et supprime les paves crees par CreateAfter */
		   /* mais ne supprime pas ceux crees par CreateWith */
		   pP = PavEnglob->AbNext;
		   stop = False;
		   do
		      if (pP == NULL)
			 stop = True;
		      else if (!pP->AbPresentationBox || pP->AbDead
			       || pP->AbElement != PavEnglob->AbElement)
			 stop = True;
		      else
			{
			   if (RegleCree (pDoc, PavEnglob, pP) != FnCreateWith)
			     {
				*volsupp += pP->AbVolume;
				TuePave (pP);
				*PavReaff = Englobant (*PavReaff, pP);
				/* traite les paves qui se referent au pave detruit */
				SuppRfPave (pP, &PavR, pDoc);
				*PavReaff = Englobant (*PavReaff, PavR);
			     }
			   pP = pP->AbNext;
			}
		   while (!(stop));
		}
	   /* passe au niveau superieur */
	   PavEnglob = PavEnglob->AbEnclosing;
	}
}
#endif /* __COLPAGE__ */



/* ---------------------------------------------------------------------- */
/* |    SupprPaves reduit de dvol le volume du pave PavRacine en        | */
/* |            supprimant des paves, en tete si Tete est vrai, en      | */
/* |            queue sinon.                                            | */
/* |            Au retour dvol contient le volume qui reste a supprimer.| */
/* |            suppression page par page si vue avec pages             | */
/* |            cette procedure est toujours appelee avec la racine de  | */
/* |            la vue si le document est mis en pages.                 | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         SupprPaves (PtrAbstractBox PavRacine, PtrDocument pDoc, boolean Tete, int *dvol)

#else  /* __STDC__ */
static void         SupprPaves (PavRacine, pDoc, Tete, dvol)
PtrAbstractBox             PavRacine;
PtrDocument         pDoc;
boolean             Tete;
int                *dvol;

#endif /* __STDC__ */

{
   PtrAbstractBox             pPav, pFrere, PavReaff, PavR;
   int                 volsupp, volpres;
   boolean             stop, stop1;
   PtrElement          pEl1;

#ifdef __COLPAGE__
   PtrAbstractBox             pP;
   int                 Vue, nbpages;
   boolean             caspage;

#endif /* __COLPAGE__ */

   /* cherche le premier pave englobe' de volume inferieur a dvol et qui */
   /* soit secable */
   pPav = PavRacine;
#ifdef __COLPAGE__
   PavReaff = NULL;
   /* suppression d'une page si la vue est paginee */
   caspage = False;
   Vue = pPav->AbDocView;
   pP = PavRacine->AbFirstEnclosed;
   if (pP != NULL && pP->AbElement->ElLeafType == LtPageColBreak)
      caspage = True;
   else
      caspage = False;

   if (!caspage)
     {
	/* inutile si destruction par pages entieres */
#endif /* __COLPAGE__ */
	volpres = 0;
	/* volume des paves de presentation des elem englobants */
	stop = False;
	do
	   if (pPav == NULL)
	      stop = True;
	   else if (pPav->AbVolume + volpres <= *dvol || NonSecable (pPav))
	      stop = True;
	   else
	     {
		/* premier pave du niveau inferieur */
		pPav = pPav->AbFirstEnclosed;
		if (pPav != NULL)
		   /* si on supprime en tete, cherche le premier pave qui ne soit */
		   /* pas un pave de presentation */
		   /* si on supprime en queue, cherche le dernier pave qui ne soit */
		   /* pas un pave de presentation */
		  {
		     if (!Tete)
			while (pPav->AbNext != NULL)
			   pPav = pPav->AbNext;
		     stop1 = False;
		     do
			if (pPav == NULL)
			   stop1 = True;
			else if (!pPav->AbPresentationBox)
			   stop1 = True;
			else if (pPav->AbFirstEnclosed != NULL &&
			pPav->AbFirstEnclosed->AbElement == pPav->AbElement)
			   /* un pave de presentation cree' par la regle FnCreateEnclosing */
			   /* on le considere comme un pave' d'element structure' */
			   stop1 = True;
			else
			  {
			     volpres += pPav->AbVolume;
			     if (Tete)
				pPav = pPav->AbNext;
			     else
				pPav = pPav->AbPrevious;
			  }
		     while (!(stop1));
		  }
	     }
	while (!(stop));
	if (pPav == NULL)
	   *dvol = 0;
	else if (pPav->AbVolume + volpres > *dvol)
	   /* on ne peut rien supprimer */
	   *dvol = 0;
	if (*dvol > 0)
	  {
#ifdef __COLPAGE__
	     if (caspage)
	       {
		  /* pP est le premier fils de la racine */
		  /* on compte le nombre de pages (pour en laisser au moins une */
		  nbpages = NbPages (PavRacine);
		  if (!Tete)
		     while (pP->AbNext != NULL)
			pP = pP->AbNext;
		  /* suppression des paves de page dans la limite de dvol */
		  /* tout en laissant au moins une page */
		  /* TODO faut-il prevoir de verifier qu'on ne laisse pas une page vide */
		  while (*dvol > 0 && pP != NULL && nbpages > 1)
		    {
		       if (pP->AbElement->ElLeafType == LtPageColBreak)
			  if (pP->AbLeafType == LtCompound && !pP->AbPresentationBox)
			     if (*dvol - pP->AbVolume >= 0)
			       {
				  *dvol -= pP->AbVolume;
				  TuePresEnglob (pP, Tete, pDoc, &PavReaff, &volsupp, False);
				  *dvol -= volsupp;
				  TuePave (pP);
				  SuppRfPave (pP, &PavR, pDoc);
				  nbpages--;
			       }
			     else
				pP = NULL;
		       if (pP != NULL)
			  if (Tete)
			     pP = pP->AbNext;
			  else
			     pP = pP->AbPrevious;
		    }
		  if (VueAssoc (PavRacine->AbElement))
		     pDoc->DocAssocModifiedAb[PavRacine->AbElement->ElAssocNum - 1] = PavRacine;

		  else
		     pDoc->DocViewModifiedAb[Vue - 1] = PavRacine;
		  *dvol = 0;
	       }
	     else
	       {
		  /* pas de pages : pas de changement de code */

#endif /* __COLPAGE__ */
		  /* on peut supprimer le pave pPav */
		  /* calcule le nouveau volume qui restera a supprimer apres la */
		  /* suppression de pPav */
		  *dvol -= pPav->AbVolume;
		  PavReaff = pPav;
		  /* il faudra reafficher au moins pPav */
		  /* tous les paves englobant pPav sont coupe's. On supprime leurs */
		  /* paves de presentation. On supprime aussi ceux crees par le pave */
		  /* tue'. */
		  TuePresEnglob (pPav, Tete, pDoc, &PavReaff, &volsupp, False);
		  *dvol -= volsupp;
		  /* detruit le pave trouve' et toute sa descendance */
		  TuePave (pPav);
		  /* detruit les paves freres tant qu'il reste du volume a supprimer */
		  pFrere = pPav;
		  while (*dvol > 0 && pFrere != NULL)
		    {
		       if (Tete)
			  pFrere = pFrere->AbNext;
		       else
			  pFrere = pFrere->AbPrevious;
		       if (pFrere != NULL)
			  /* on ne supprime pas les pave's de presentation, sauf ceux qui */
			  /* ont ete cree's par une regle FnCreateEnclosing */
			  if (!pFrere->AbPresentationBox ||
			      (pFrere->AbPresentationBox && pFrere->AbFirstEnclosed != NULL &&
			       pFrere->AbFirstEnclosed->AbElement == pFrere->AbElement))
			     SupprPaves (pFrere, pDoc, Tete, dvol);
		       /* traite ensuite les paves qui se referent au pave detruit */
		    }
		  SuppRfPave (pPav, &PavR, pDoc);
		  PavReaff = Englobant (PavReaff, PavR);
		  pEl1 = PavRacine->AbElement;
		  if (VueAssoc (PavRacine->AbElement))
		     pDoc->DocAssocModifiedAb[pEl1->ElAssocNum - 1] =
			Englobant (PavReaff, pDoc->DocAssocModifiedAb[pEl1->ElAssocNum - 1]);
		  else
		     pDoc->DocViewModifiedAb[PavRacine->AbDocView - 1] =
			Englobant (PavReaff, pDoc->DocViewModifiedAb[PavRacine->AbDocView - 1]);
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
/* |    AjVolVue ajuste le volume de la vue dont le pave PavRacine      | */
/* |            est le pave racine.                                     | */
/* |            VolOpt est le volume optimum de cette vue.              | */
/* |            ElMilieu est l'element qui devrait etre au milieu       | */
/* |            de la vue.                                              | */
  /*     PavRacine est une vraie racine d'image abstraite        | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         AjVolVue (int VolOpt, PtrAbstractBox PavRacine, PtrElement ElMilieu, PtrDocument pDoc)

#else  /* __STDC__ */
static void         AjVolVue (VolOpt, PavRacine, ElMilieu, pDoc)
int                 VolOpt;
PtrAbstractBox             PavRacine;
PtrElement          ElMilieu;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   boolean             ajoute, supprime, miltete, stop;
   int                 vue, volprec, dvol;
   PtrAbstractBox             pavPrec, pavPere;

   /* evalue d'abord s'il faut ajouter ou supprimer des paves */
   ajoute = False;
   supprime = False;
   if (!NonSecable (PavRacine))
      if (PavRacine->AbVolume < VolOpt - VolOpt / 8)
	 /* il faudrait creer de nouveaux paves dans cette vue */
	 ajoute = True;
      else if (PavRacine->AbVolume > VolOpt + VolOpt / 8)
	 /* il faudrait supprimer des paves dans cette vue */
	 supprime = True;
   if (ajoute || supprime)
     {
	/* calcule la position actuelle de ElMilieu dans la vue, pour savoir */
	/* s'il faut modifier le debut ou la fin de la vue */
	vue = PavRacine->AbDocView;
	if (ElMilieu->ElAbstractBox[vue - 1] == NULL)
	   /* ElMilieu n'a pas de pave dans cette vue */
	   /* cherche le 1er element englobant ElMilieu qui ait un pave dans */
	   /* la vue */
	  {
	     stop = False;
	     do
	       {
		  ElMilieu = ElMilieu->ElParent;
		  if (ElMilieu == NULL)
		    {
		       stop = True;
		       ElMilieu = PavRacine->AbElement;
		    }
		  else if (ElMilieu->ElAbstractBox[vue - 1] != NULL)
		     stop = True;
	       }
	     while (!(stop));
	  }
	/* ElMilieu a un pave dans cette vue */
	/* calcule le volume des paves qui le precedent */
	volprec = 0;
	pavPere = ElMilieu->ElAbstractBox[vue - 1];
	do
	  {
	     pavPrec = pavPere->AbPrevious;
	     while (pavPrec != NULL)
	       {
		  volprec += pavPrec->AbVolume;
		  pavPrec = pavPrec->AbPrevious;
	       }
	     pavPere = pavPere->AbEnclosing;
	  }
	while (!(pavPere == NULL));
	miltete = volprec < VolOpt / 2;
	if (ajoute)
	   /* il faut ajouter des paves */
	   if (miltete)
	      /* l'element qui devrait etre au milieu est dans la 1ere moitie' */
	      if (PavRacine->AbTruncatedHead)
		 /* le debut de l'image n'est pas complet, on ajoute en tete */
		 AjoutePaves (PavRacine, pDoc, True);
	      else
		{
		   /* le debut de l'image est complet */
		   /* on ajoute en queue, si la queue n'est pas complete */
		   if (PavRacine->AbTruncatedTail)
		      AjoutePaves (PavRacine, pDoc, False);
		}
	   else
	      /* l'element qui devrait etre au milieu est dans la 2eme moitie' */
	   if (PavRacine->AbTruncatedTail)
	      /* la fin de l'image n'est pas complete, on ajoute en queue */
	      AjoutePaves (PavRacine, pDoc, False);
	   else
	     {
		/* la fin de l'image est complete, on ajoute en tete si le */
		/* idebut n'est pas complet */
		if (PavRacine->AbTruncatedHead)
		   AjoutePaves (PavRacine, pDoc, True);
	     }
	else if (supprime)
	   /* supprime des paves en queue si l'element qui devrait etre au */
	   /* milieu est dans la premiere moitie de l'image, et inversement */
	  {
	     dvol = PavRacine->AbVolume - VolOpt;
	     SupprPaves (PavRacine, pDoc, !miltete, &dvol);
	  }
     }
}



/* ---------------------------------------------------------------------- */
/* |    AjusteVolume pour toutes les vues du document pointe' par pDoc  | */
/* |            ajuste (reduit ou augmente) le volume des images        | */
/* |            abstraites en conservant l'element pointe par pEl au    | */
/* |            milieu (ou a peu pres) de l'image abstraite.            | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                AjusteVolume (PtrElement pEl, PtrDocument pDoc)

#else  /* __STDC__ */
void                AjusteVolume (pEl, pDoc)
PtrElement          pEl;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   int                 vue;
   PtrDocument         pDo1;
   PtrElement          pEl1;

#ifdef __COLPAGE__
   PtrAbstractBox             pPav;
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
		     pPav = pDo1->DocViewRootAb[vue - 1];
		     if (pPav->AbFirstEnclosed != NULL
			 && pPav->AbFirstEnclosed->AbElement->ElTypeNumber ==
			 ord (PageBreak) + 1)
		       {
			  nb = NbPages (pPav);
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
		pPav = pDo1->DocAssocRoot[pEl1->ElAssocNum - 1]->ElAbstractBox[0];
		if (pPav->AbFirstEnclosed != NULL
		    && pPav->AbFirstEnclosed->AbElement->ElTypeNumber ==
		    ord (PageBreak) + 1)
		   /* vue du document paginee */
		  {
		     nb = NbPages (pPav);
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
/* |    VolAugmente Le Mediateur augmente de dVol le volume affichable  | */
/* |            dans la fenetre ViewFrame. Met a jour la capacite de la   | */
/* |            vue affichee dans cette frame et cree de nouveaux       | */
/* |            paves en tete ou en queue, selon le booleen EnTete,     | */
/* |            de l'image abstraite affichee dans ViewFrame.             | */
/* |            On cree des paves, le Mediateur se charge du            | */
/* |            reaffichage                                             | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                VolAugmente (boolean EnTete, int dVol, int frame)

#else  /* __STDC__ */
void                VolAugmente (EnTete, dVol, frame)
boolean             EnTete;
int                 dVol;
int                 frame;

#endif /* __STDC__ */

{
   PtrDocument         pDoc;
   int                 vue, h;
   boolean             assoc;
   PtrAbstractBox             pPav;

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
      printf ("\nErreur VolAugmente: frame incorrecte\n");
   else
     {
	if (assoc)
	  {
	     /* element associe */
	     pPav = pDoc->DocAssocRoot[vue - 1]->ElAbstractBox[0];
#ifdef __COLPAGE__
	     /* attention, vue contient le numero d'element associe */
	     pElRacine = pDoc->DocAssocRoot[vue - 1];
#endif /* __COLPAGE__ */
	     pDoc->DocAssocVolume[vue - 1] = pPav->AbVolume + dVol;
	     pDoc->DocAssocFreeVolume[vue - 1] = dVol;
	  }
	else
	  {
	     /* element de l'arbre principal */
#ifdef __COLPAGE__
	     pElRacine = pDoc->DocRootElement;
#endif /* __COLPAGE__ */
	     pPav = pDoc->DocViewRootAb[vue - 1];
	     pDoc->DocViewVolume[vue - 1] = pPav->AbVolume + dVol;
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
	     nb = NbPages (pPav);
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
	if (!NonSecable (pPav))
	  {
	     /* cree les paves de la partie qui va apparaitre */
	     AjoutePaves (pPav, pDoc, EnTete);

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
		       (void) ModifVue (frame, &h, pDoc->DocAssocModifiedAb[vue - 1]);
		       LibPavMort (pDoc->DocAssocModifiedAb[vue - 1]);
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
		  (void) ModifVue (frame, &h, pDoc->DocViewModifiedAb[vue - 1]);
		  LibPavMort (pDoc->DocViewModifiedAb[vue - 1]);
		  pDoc->DocViewModifiedAb[vue - 1] = NULL;
	       }
	  }
     }
}



/* ---------------------------------------------------------------------- */
/* |    VolReduit Le Mediateur reduit de dVol le volume affichable      | */
/* |            dans la fenetre frame. Met a jour la capacite de la     | */
/* |            vue affichee dans cette frame et supprime des paves     | */
/* |            en tete ou en queue, selon le booleen EnTete, de        | */
/* |            l'image abstraite affichee dans frame.          | */
/* |            On supprime des paves, le Mediateur se charge du        | */
/* |            reaffichage                                             | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                VolReduit (boolean EnTete, int dVol, int frame)

#else  /* __STDC__ */
void                VolReduit (EnTete, dVol, frame)
boolean             EnTete;
int                 dVol;
int                 frame;

#endif /* __STDC__ */

{
   PtrDocument         pDoc;
   int                 vue, h;
   boolean             assoc;
   PtrAbstractBox             pPav;

#ifdef __COLPAGE__
   FILE               *list;

#endif /* __COLPAGE__ */

   DocVueFen (frame, &pDoc, &vue, &assoc);
   /* met a jour la nouvelle capacite de la vue et cherche le pave racine */
   /* de la vue */
   if (pDoc == NULL)
      printf ("\nErreur VolReduit: frame incorrecte\n");
   else
     {
	if (assoc)
	  {
	     /* element associe */
	     /* attention, vue contient le numero d'element associe */
	     pPav = pDoc->DocAssocRoot[vue - 1]->ElAbstractBox[0];
	     pDoc->DocAssocVolume[vue - 1] = pPav->AbVolume - dVol;
	  }
	else
	  {
	     /* element de l'arbre principal */
	     pPav = pDoc->DocViewRootAb[vue - 1];
	     pDoc->DocViewVolume[vue - 1] = pPav->AbVolume - dVol;
	  }
	if (pPav != NULL)
#ifdef __COLPAGE__
	  {
#endif /* __COLPAGE__ */
	     if (dVol >= pPav->AbVolume)
		printf ("Erreur VolReduit: dVol=%3d volume vue=%3d\n", dVol, pPav->AbVolume);
	     /* supprime les paves */
	     SupprPaves (pPav, pDoc, EnTete, &dVol);
#ifdef __COLPAGE__
	     /* sauvegarde de l'image abstraite pour tests */
	     list = fopen ("/perles/roisin/debug/volred", "w");
	     if (list != NULL)
	       {
		  NumPav (pPav);
		  AffPaves (pPav, 2, list);
		  fclose (list);
	       }
	     /* signale au Mediateur les paves modifies */
	     h = HauteurCoupPage;
	     /* appel de modifVue depuis la racine car de nouvelles */
	     /* pages ont pu etre detruites */
	     (void) ModifVue (frame, &h, pPav);
	     /* meme chose pour LibPavMort */
	     LibPavMort (pPav);
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
		       (void) ModifVue (frame, &h, pDoc->DocAssocModifiedAb[vue - 1]);
		       LibPavMort (pDoc->DocAssocModifiedAb[vue - 1]);
		       pDoc->DocAssocModifiedAb[vue - 1] = NULL;
		    }
	       }
	     else if (pDoc->DocViewModifiedAb[vue - 1] != NULL)
	       {
		  (void) ModifVue (frame, &h, pDoc->DocViewModifiedAb[vue - 1]);
		  LibPavMort (pDoc->DocViewModifiedAb[vue - 1]);
		  pDoc->DocViewModifiedAb[vue - 1] = NULL;
	       }
#endif /* __COLPAGE__ */
	  }
     }



/* ---------------------------------------------------------------------- */
/* |    VerifPave verifie que l'element pointe' par pEl a au moins      | */
/* |            un pave dans la vue Vue. S'il n'en a pas, essaie d'en   | */
/* |            creer un en modifiant l'image abstraite de cette vue.   | */
/* |            Si debut est vrai, on cree l'image de la vue en         | */
/* |            commencant par l'element pointe' par pEl, sinon, on     | */
/* |            place cet element au milieu de l'image creee.           | */
/* |            Si affiche est Vrai, l'image est reaffichee.            | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
   void                VerifPave (PtrElement pEl, int Vue, PtrDocument pDoc, boolean debut, boolean affiche)
#else  /* __STDC__ */
   void                VerifPave (pEl, Vue, pDoc, debut, affiche)
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
                          PavReaff, PavRacine, pPrec;

#ifndef __COLPAGE__
      int         nR;

#endif /* __COLPAGE__ */
      PtrElement          pEl1;
      PtrDocument         pDo1;
      PtrAbstractBox             pPa1;
      boolean             complet;

#ifdef __COLPAGE__
      int                 VueSch, tour;
      PtrAbstractBox             pPavPage, pP;
      PtrElement          pElRacine, pFils, pElRef;
      boolean             trouve, VuePaginee, vueassoc, acreer;
      PtrPSchema          pSchPage;
      FILE               *list;

      VuePaginee = False;
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
		   PavRacine = pElRacine->ElAbstractBox[Vue - 1];
		frame = pDoc->DocAssocFrame[nAssoc - 1];
		vueassoc = True;
	     }
	   else
	     {
		vueexiste = pDoc->DocView[Vue - 1].DvPSchemaView > 0;
		pElRacine = pDoc->DocRootElement;
		PavRacine = pElRacine->ElAbstractBox[Vue - 1];
		frame = pDoc->DocViewFrame[Vue - 1];
		vueassoc = False;
	     }
	   /*  test si vue paginee */
	   pEl1 = pElRacine->ElFirstChild;
	   /* recherche le numero de vue defini dans le schema de presentation */
	   /* Attention il faut appeler VueAAppliquer avec pElRacine */
	   VueSch = VueAAppliquer (pElRacine, NULL, pDoc, Vue);
	   if (TypeBPage (pEl1, VueSch, &pSchPage) != 0)
	      VuePaginee = True;
	   acreer = vueexiste;
	   /* a priori on cree les paves de l'element */
	   if (VuePaginee && acreer)
	      /* Vue paginee : code specifique */
	     {
		/* si l'element a un pave, est-ce le premier des dup ? */
		/* si c'est le cas (le pave n'est pas coupe en tete), */
		/* on ne cree rien car l'element a son premier pave deja cree */
		trouve = False;
		pP = pEl->ElAbstractBox[Vue - 1];
		if (pP != NULL)
		  {
		     acreer = False;	/* a priori rien a creer */
		     while (pP != NULL && pP->AbPresentationBox && pP->AbNext != NULL)
			pP = pP->AbNext;
		     if (pP != NULL && pP->AbElement == pEl && pP->AbLeafType == LtCompound)
			acreer = (pP->AbTruncatedHead);
		  }
		if (acreer)
		  {
		     /* est-ce l'element racine d'un arbre d'elements associes ? */
		     pElRef = pEl;
		     if (pElRef->ElParent == NULL)
			/* c'est une racine */
			if (nAssoc != 0 && vueassoc == False)
			   /* c'est un element associe racine d'elements */
			  {
			     /* s'affichant en haut ou bas de page */
			     /* on creera son pave sur la page de la premiere */
			     /* reference a son premier fils */
			     pElRef = pElRef->ElFirstChild;
			     if (pElRef == NULL)
				acreer = False;
			  }
		  }
		if (acreer)
		   /* si c'est un element associe qui s'affiche en haut ou bas de page */
		   /* il faut rechercher la premiere reference et afficher la page */
		   /* de cet element reference. CreePaves creera les paves de l'element */
		   /* associe par appel de CrPavHB */
		   if (nAssoc != 0 && vueassoc == False)
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
				trouve = True;	/* on a trouver la page a creer */
			  }
			if (pEl1 == NULL)
			   acreer = False;	/* TODO plus tard */
		     }

		if (acreer && !trouve)
		  {
		     /* on recherche la page dans laquelle se trouve pEl */
		     trouve = False;
		     /* si pEl est une PageBreak, c'est fini */
		     if (pEl->ElTypeNumber == ord (PageBreak) + 1
			 && (pEl->ElPageType == PgBegin
			     || pEl->ElPageType == PgComputed
			     || pEl->ElPageType == PgUser)
			 && pEl->ElViewPSchema == VueSch)
		       {
			  trouve = True;
			  pElPage = pEl;
		       }
		     else
		       {
			  pEl1 = pEl;
			  while (!trouve && pEl1 != NULL)
			    {
			       pElPage = ArCherche (pEl1, ord (PageBreak) + 1, NULL);
			       if (pElPage != NULL && pElPage->ElViewPSchema == VueSch
				   && (pElPage->ElPageType == PgBegin
				       || pElPage->ElPageType == PgComputed
				 || pElPage->ElPageType == PgUser))
				  trouve = True;
			       else
				  pEl1 = pElPage;
			    }
		       }
		     if (trouve && pElPage->ElAbstractBox[Vue - 1] != NULL)
			/* si trouve et si l'element marque page a un pave dans la vue */
			/* c'est fini: l'element pEl n'est pas visible dans cette vue, */
			/* sinon il aurait eu un pave */
			acreer = False;
		     if (!trouve)
			/* si on n'a pas trouve d'element marque page precedent */
			/* c'est que pEl est la racine ou un element marque page */
			/* d'une autre vue */
			/* on creera l'image a partir de la racine */
			/* sans partir d'une marque de page */
			/* sinon erreur */
			if (!(pEl->ElParent == NULL
			      || (pEl->ElTypeNumber == ord (PageBreak) + 1
				  && pEl->ElViewPSchema != VueSch)))
			  {
			     printf ("peut etre erreur VerifPave : pas trouve de marque page %s",
				     pEl->ElSructSchema->SsRule[pEl->ElTypeNumber - 1].SrName);
			     printf ("\n");
			     acreer = False;
			  }
		  }
		if (acreer)
		  {
		     /* on detruit l'i.a. sauf la racine */
		     /* pour recreer correctement la partie de l'i.a. demandee */
		     PavRacine = pElRacine->ElAbstractBox[Vue - 1];
		     if (PavRacine != NULL)
		       {
			  pPa1 = PavRacine->AbFirstEnclosed;
			  while (pPa1 != NULL)
			    {
			       TuePave (pPa1);
			       pPa1 = pPa1->AbNext;
			    }
			  PavRacine->AbDead = True;	/* pour que sa boite soit */
			  /* detruite */
			  h = 0;
			  ModifVue (frame, &h, PavRacine);
			  PavRacine->AbDead = False;
			  PavRacine->AbNew = True;
			  PavRacine->AbTruncatedHead = True;
			  PavRacine->AbTruncatedTail = True;
			  LibPavMort (PavRacine);
		       }
		     else
		       {
			  /* la racine n'a pas de pave, on le cree (sans la descendance) */
			  PavRacine = CreePaves (pElRacine, pDoc, Vue, True,
						 False, &complet);
			  pDoc->DocViewRootAb[Vue - 1] = PavRacine;
			  PavRacine->AbTruncatedHead = True;
			  PavRacine->AbTruncatedTail = True;
		       }
		  }
		/* on cree les paves de la Marque Page */
		/* par construction (cf. CreePaves), ils seront mis sous */
		/* la racine */
		if (trouve)
		  {
		     pPavPage = CreePaves (pElPage, pDoc, Vue, True, True, &complet);
		     /* on cree les paves de ses ascendants en les marquant CT et CQ */
		     pAncetre = pElPage->ElParent;
		     pFils = pElPage;
		     NumAsc = 0;
		     stop = False;
		     do
			if (pAncetre == NULL)
			   stop = True;
			else if (pAncetre->ElAbstractBox[Vue - 1] != NULL)
			   /* normalement c'est la racine */
			   stop = True;
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
			  pPrec = CreePaves (pAsc[i - 1], pDoc, Vue, True, False,
					     &complet);
			  pPrec = pAsc[i - 1]->ElAbstractBox[Vue - 1];
			  if (pPrec != NULL)
			    {
			       /* il faudra appliquer au pave ses */
			       /* regles de presentation (sera fait par CreePaves) */
			       pPrec->AbSize = -1;
			       /* on le marque coupe */
			       pPrec->AbTruncatedHead = True;
			       pPrec->AbTruncatedTail = True;
			       if (PremCreVide == NULL)
				  /* 1er pave cree' */
				 {
				    PremCreVide = pPrec;
				    /* on chaine ce pave sous le corps de page */
				    while (pPavPage != NULL && pPavPage->AbPresentationBox)
				       pPavPage = pPavPage->AbNext;
				    if (pPavPage != NULL)	/* toujours vrai ? */
				      {
					 if (pPrec->AbEnclosing->AbFirstEnclosed ==
					     pPrec)
					    pPrec->AbEnclosing->AbFirstEnclosed =
					       pPrec->AbNext;
					 if (pPrec->AbNext != NULL)
					    pPrec->AbNext->AbPrevious = NULL;
					 pPrec->AbNext = NULL;
					 pPrec->AbEnclosing = pPavPage;
					 if (pPavPage->AbFirstEnclosed == NULL)
					    pPavPage->AbFirstEnclosed = pPrec;
					 else
					   {
					      pPa1 = pPavPage->AbFirstEnclosed;
					      while (pPa1->AbNext != NULL)
						 pPa1 = pPa1->AbNext;
					      pPa1->AbNext = pPrec;		/* en queue */
					      pPrec->AbPrevious = pPa1;
					   }
				      }
				 }
			    }
			  i--;
		       }
		  }		/* fin cas creation paves de page et des ascendants */
		/* trouve est vrai */
		if (trouve || acreer)
		   /* on appelle VolAugmente pour creer l'i.a. page a page, */
		   /* appliquer les regles de presentation des paves deja crees et */
		   /* et creer l'image concrete */
		  {
		     VolAugmente (False, THOT_MAXINT, frame);
		     /* on appelle MontrerBoite pour positionner correctement l'i.a. */
		     /* dans la fenetre */
		     if (affiche && !debut && pEl->ElAbstractBox[Vue - 1] != NULL)
			/* TODO : que faire dans les autres cas */
			MontrerBoite (frame, pEl->ElAbstractBox[Vue - 1]->AbBox, 1, 0);
		     /* sauvegarde de l'image abstraite pour tests */
		     list = fopen ("/perles/roisin/debug/verifpav", "w");
		     if (list != NULL)
		       {
			  NumPav (PavRacine);
			  AffPaves (PavRacine, 2, list);
			  fclose (list);
		       }

		     if (affiche)
			AfficherVue (frame);
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
		   stop = False;
		   do
		      if (pAncetre == NULL)
			 stop = True;
		      else if (pAncetre->ElAbstractBox[Vue - 1] != NULL)
			 stop = True;
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
					stop = True;
					vueexiste = False;
				     }
				else
				   /* il devrait s'afficher dans une boite de haut ou */
				   /* de bas de page, mais il n'y a pas de page */
				  {
				     stop = True;
				     vueexiste = False;
				  }
			     }
			   else
			      pAncetre = pEl1->ElParent;
			}
		   while (!(stop));
		   if (pAsc[NumAsc - 1]->ElParent == NULL)
		      /* la racine de l'arbre n'a pas de pave dans cette vue */
		      creation = True;
		   /* c'est une creation de vue */
		   else
		      creation = False;
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
			     pPrec = CreePaves (pAsc[i - 1], pDoc, Vue, True, False, &complet);
			     if (pEl1->ElAbstractBox[Vue - 1] != NULL)
				pPrec = pEl1->ElAbstractBox[Vue - 1];
			     if (pPrec != NULL)

				/* marque sur le pave cree qu'il faudra lui appliquer ses */
				/* regles de presentation (ce sera fait par CreePaves) */
				pPrec->AbSize = -1;

			     if (pPrec != NULL)
				/* on a cree un pave */
			       {
				  DerCreVide = pPrec;
				  /* dernier pave cree' */
				  if (PremCreVide == NULL)
				     PremCreVide = pPrec;
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
				  PavRacine = pDo1->DocAssocRoot[nAssoc - 1]->ElAbstractBox[0];
				  frame = pDo1->DocAssocFrame[nAssoc - 1];
			       }
			     else
				/* vue de l'arbre principal */
			       {
				  pDo1->DocViewRootAb[Vue - 1] = PremCreVide;
				  PavRacine = pDo1->DocViewRootAb[Vue - 1];
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
			     TuePresEnglob (PavRestant, True, pDoc, &PavReaff, &volsupp, True);
			     PavReaff = NULL;
			     TuePresEnglob (PavRestant, False, pDoc, &PavReaff, &volsupp, True);
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
				  PavRacine = pDo1->DocAssocRoot[nAssoc - 1]->ElAbstractBox[0];
				  frame = pDo1->DocAssocFrame[nAssoc - 1];
				  if (frame != 0)
				    {
				       PavRacine->AbDead = True;
				       /* marque mort le pave racine */
				       /* le signale au Mediateur */
				       ModifVue (frame, &h, PavRacine);
				       PavRacine->AbDead = False;
				       /* resucite le pave racine */
				       LibPavMort (PavRacine);
				       /* libere tous les pave detruits */
				       pDo1->DocAssocModifiedAb[nAssoc - 1] = NULL;
				    }
			       }
			     else
				/* meme traitement pour une vue de l'arbre principal */
			       {
				  PavRacine = pDo1->DocViewRootAb[Vue - 1];
				  frame = pDo1->DocViewFrame[Vue - 1];
				  if (frame != 0)
				    {
				       PavRacine->AbDead = True;
				       ModifVue (frame, &h, PavRacine);
				       PavRacine->AbDead = False;
				       LibPavMort (PavRacine);
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
			     pPa1 = PavRestant;
			     pPa1->AbNew = False;
			     if (creation)
				/* a priori les paves ne sont pas complets */
#ifdef __COLPAGE__
				if (pPa1->AbLeafType == LtCompound)
#else  /* __COLPAGE__ */
				if (pPa1->AbLeafType == LtCompound
				    && !pPa1->AbInLine)
#endif /* __COLPAGE__ */
				  {
				     pPa1->AbTruncatedHead = True;
				     pPa1->AbTruncatedTail = True;
				  }
			     PavRestant = pPa1->AbEnclosing;
			     /* passe a l'englobant */
			  }
			/* applique les regles des paves nouvellement crees et cree */
			/* d'autres paves derriere */
			AjoutePaves (PavRacine, pDoc, False);
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
				  pPrec = CreePaves (pAsc[i - 1], pDoc, Vue, True, True, &complet);
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
			     /* viennent d'etre crees par AjoutePaves devant les paves */
			     /* conserves. Ces paves de presentation seront ainsi traites */
			     /* correctement lors du prochain appel de AjoutePaves. */
			     PavRestant = DerCreVide;
			     while (PavRestant != NULL)
			       {
				  pPrec = PavRestant->AbPrevious;
				  stop = False;
				  do
				     if (pPrec == NULL)
					stop = True;
				     else if (pPrec->AbElement != PavRestant->AbElement)
					stop = True;
				     else
				       {
					  pPrec->AbNew = False;
					  pPrec = pPrec->AbPrevious;
				       }
				  while (!(stop));
				  PavRestant = PavRestant->AbEnclosing;
				  /* passe a l'englobant */
			       }
			     /* cree de nouveaux paves */
			     AjoutePaves (PavRacine, pDoc, True);
			     /* marque comme nouveaux tous les paves de presentation qui */
			     /* viennent d'etre marques anciens. Ces paves de presentation */
			     /* seront ainsi traites correctement par le Mediateur. */
			     PavRestant = DerCreVide;
			     while (PavRestant != NULL)
			       {
				  pPrec = PavRestant->AbPrevious;
				  stop = False;
				  do
				     if (pPrec == NULL)
					stop = True;
				     else if (pPrec->AbElement != PavRestant->AbElement)
					stop = True;
				     else
				       {
					  pPrec->AbNew = True;
					  pPrec = pPrec->AbPrevious;
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
			     pPa1 = PavRestant;
			     pPa1->AbNew = True;
			     PavRestant = pPa1->AbEnclosing;
			     /* passe a l'englobant */
			  }
			/* indique les nouvelles modifications au Mediateur et */
			/*  lui fait reafficher toute la vue */
			if (frame != 0)
			  {
			     h = 0;
			     ModifVue (frame, &h, PavRacine);
			     if (affiche)
				AfficherVue (frame);
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
/* |    VolumeArbAbs    retourne                                        | */
/* |       VolAvant: le volume total des elements de l'arbre abstrait   | */
/* |                 qui precedent l'element auquel appartient le pave  | */
/* |                 PremPav.                                           | */
/* |       VolApres: le volume total des elements de l'arbre abstrait   | */
/* |                 qui se trouvent apres l'element auquel appartient  | */
/* |                 le pave DerPav.                                    | */
/* |       VolArbre: le volume total de l'arbre abstrait.               | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
   void                VolumeArbAbs (PtrAbstractBox PavRacine, PtrAbstractBox PremPav, PtrAbstractBox DerPav, int *VolAvant, int *VolApres, int *VolArbre)
#else  /* __STDC__ */
   void                VolumeArbAbs (PavRacine, PremPav, DerPav, VolAvant, VolApres, VolArbre)
   PtrAbstractBox             PavRacine;
   PtrAbstractBox             PremPav;
   PtrAbstractBox             DerPav;
   int                *VolAvant;
   int                *VolApres;
   int                *VolArbre;

#endif /* __STDC__ */
   {
      PtrElement          pEl;

      if (PremPav == NULL || DerPav == NULL)
	 /* les parametres d'appels sont errone's */
	{
	   *VolAvant = 0;
	   *VolApres = 0;
	   *VolArbre = 0;
	}
      else
	{
	   /* calcule d'abord le volume total de l'arbre abstrait */
	   *VolArbre = PavRacine->AbElement->ElVolume;

	   /* on calcule maintenant le volume de ce qui precede */
	   *VolAvant = 0;
	   /* si le premier pave' correspond a un element associe' affiche' */
	   /* dans un haut ou bas de page, on prend le pave' saut de page */
	   while (PremPav->AbElement->ElAssocNum !=
		  PavRacine->AbElement->ElAssocNum)
	      PremPav = PremPav->AbEnclosing;
	   pEl = PremPav->AbElement;
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
	   while (DerPav->AbElement->ElAssocNum !=
		  PavRacine->AbElement->ElAssocNum)
	      DerPav = DerPav->AbEnclosing;
	   pEl = DerPav->AbElement;
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
/* |    SauterDansVue fait afficher dans la fenetre la partie de        | */
/* |            document qui se trouve a la distance indiquee du debut  | */
/* |            de l'arbre abstrait.                                    | */
/* |            distance est un pourcentage : 0 <= distance <= 100      | */
/* |            Si distance = 0, on fait afficher le debut de l'arbre.  | */
/* |            Si distance = 100, on fait afficher la fin de l'arbre.  | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
   void                SauterDansVue (int frame, int distance)

#else  /* __STDC__ */
   void                SauterDansVue (frame, distance)
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
      PtrAbstractBox             PaveRacine, pPav;
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
			pPav = NULL;
		     else
		       {
			  pPav = pEl->ElAbstractBox[Vue - 1];
			  if (pPav == NULL)
			    {
			       VerifPave (pEl, Vue, pDoc, False, False);
			       pPav = pEl->ElAbstractBox[Vue - 1];
			    }
		       }
		     apres = True;
		     while (pEl != NULL && pPav == NULL)
			/* pEl n'a pas de pave dans le vue traitee. */
			/* On cherche la feuille la plus proche de pEl qui */
			/* soit visible dans la vue */
		       {
			  if (apres)
			     Suiv = Successeur (pEl);
			  else
			     Suiv = NULL;
			  if (Suiv != NULL)
			     /* on prend la feuille suivante */
			     if (!Suiv->ElTerminal)
				pEl = PremFeuille (Suiv);
			     else
				pEl = Suiv;
			  else
			     /* on prend la feuille precedente */
			    {
			       apres = False;
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
			       VerifPave (pEl, Vue, pDoc, False, False);
			       pPav = pEl->ElAbstractBox[Vue - 1];
			    }
		       }

		     pPav = NULL;
		     /* positionne l'image dans sa frame, en l'affichant */
		     if (distance == 0)
			/* boite racine de la vue en haut de la fenetre */
		       {
			  position = 0;
			  pPav = PaveRacine;
		       }
		     else if (distance == 100)
			/* boite racine de la vue en bas de la fenetre */
		       {
			  position = 2;
			  pPav = PaveRacine;
		       }
		     else if (pEl != NULL)
			/* boite de l'element au milieu de la fenetre */
		       {
			  if (pEl->ElAbstractBox[Vue - 1] == NULL)
			     /* cherche le premier element precedent qui a */
			     /* un pave' dans la vue */
			     pEl = ArChElVisible (PaveRacine->AbElement, pEl, Vue);
			  if (pEl != NULL)
			     pPav = pEl->ElAbstractBox[Vue - 1];
			  position = 1;
		       }
		     if (pPav != NULL)
			MontrerBoite (frame, pPav->AbBox, position, 0);

		     /* Allume la selection */
		     AllumeSelection (False, False);
		  }
	     }
	}
   }

#ifdef __COLPAGE__

/** debut ajout */
/* affichage du contexte de pPav pour debug */

#ifdef __STDC__
   void                AffPaveDebug (PtrAbstractBox pPav)

#else  /* __STDC__ */
   void                AffPaveDebug (pPav)
   PtrAbstractBox             pPav;

#endif /* __STDC__ */

   {
      PtrElement          pEl;

      if (pPav != NULL)
	{
	   pEl = pPav->AbElement;
	   printf ("contexte pPav = %x", (int) pPav, " %s",
		   pEl->ElSructSchema->SsRule[pEl->ElTypeNumber - 1].SrName);
	   printf ("pres = %d", pPav->AbPresentationBox);
	   printf ("\n");
	   printf ("pere = %x", (int) pPav->AbEnclosing);
	   if (pPav->AbEnclosing != NULL)
	      printf (" %s",
		      pPav->AbEnclosing->AbElement->ElSructSchema->SsRule[pPav->AbEnclosing->AbElement->ElTypeNumber - 1].SrName);
	   printf ("\n");
	   printf ("frere avant = %x", (int) pPav->AbPrevious);
	   if (pPav->AbPrevious != NULL)
	      printf (" %s",
		      pPav->AbPrevious->AbElement->ElSructSchema->SsRule[pPav->AbPrevious->AbElement->ElTypeNumber - 1].SrName);
	   printf ("\n");
	   printf ("frere apres  = %x", (int) pPav->AbNext);
	   if (pPav->AbNext != NULL)
	      printf (" %s",
		      pPav->AbNext->AbElement->ElSructSchema->SsRule[pPav->AbNext->AbElement->ElTypeNumber
							     - 1].SrName);
	   printf ("\n");
	   if (pPav->AbPreviousRepeated != NULL)
	      printf (" dup precedent = %x", (int) pPav->AbPreviousRepeated);
	   if (pPav->AbNextRepeated != NULL)
	      printf (" dup suivant  = %x", (int) pPav->AbNextRepeated);
	   printf ("\n");
	}
   }

   /* ------------------------------------------------------------------ */
   /*  PaveCorrect verifie les conditions d'invariance sur le pave pPav| */
   /*            et affiche un message d'erreur si une condition       | */
   /*            pas respectee. On verifie que :                       | */
   /*            1. la suite des paves de l'element pPav               | */
   /*               ne contienne pas que des paves de presentation,    | */
   /*           2. si pPav->AbPreviousRepeated != NULL                    | */
   /*               alors l'element ne pointe pas sur pPav             | */
   /*            3. si pPav->AbNextRepeated(ou Prec)  != NULL           | */
   /*               alors tous les ascendants doivent l'etre aussi     | */
   /* ------------------------------------------------------------------ */

#ifdef __STDC__
   boolean             PaveCorrect (PtrAbstractBox pPav)

#else  /* __STDC__ */
   boolean             PaveCorrect (pPav)
   PtrAbstractBox             pPav;

#endif /* __STDC__ */

   {
      PtrAbstractBox             pP, pPa1;
      PtrElement          pEl;
      boolean             correct, tousmorts;
      int                 AbDocView;

      pPa1 = pPav;
      AbDocView = pPav->AbDocView;
      /* a priori le pave est correct */
      correct = True;
      tousmorts = True;
      pEl = pPav->AbElement;

      /* premiere verification */
      /* message d'erreur si la suite des paves pointes par */
      /* pEl->ElAbstractBox ne contient que  */
      /* des paves de presentation  dont certains sont non morts */
      pP = pEl->ElAbstractBox[pPa1->AbDocView - 1];
      if (pP != NULL)		/* il existe des paves pour l'element */
	{
	   while (pP != NULL && pP->AbPresentationBox)
	     {
		if (!pP->AbDead)
		   tousmorts = False;
		if (pP->AbNext != NULL)
		   if (pP->AbNext->AbElement == pP->AbElement)
		      pP = pP->AbNext;
		   else
		      pP = NULL;
		else
		   pP = NULL;
	     }
	   if (pP == NULL && !tousmorts)
	      /* erreur dans l'image abstraite */
	     {
		correct = False;
		printf ("Erreur dans i.a : l'elem n'a que des paves pres %s",
		    pEl->ElSructSchema->SsRule[pEl->ElTypeNumber - 1].SrName);
		printf ("\n");
	     }
	}

      /* un pave de presentation ne peut pas etre duplique ?? */
      if (pPav->AbPresentationBox &&
	  (pPav->AbPreviousRepeated != NULL || pPav->AbNextRepeated != NULL))
	{
	   correct = False;
	   printf ("Erreur dans l'i.a. : pave de pres duplique %s",
		   pEl->ElSructSchema->SsRule[pEl->ElTypeNumber - 1].SrName);
	   printf ("\n");
	}
      /* deuxieme verification */
      if (pPav->AbPreviousRepeated != NULL)
	{
	   if (pEl->ElAbstractBox[AbDocView - 1] == pPav)
	      /* l'element pointe sur pPav alors qu'il a un dupPrec */
	     {
		correct = False;
		printf ("Erreur dans l'i.a.:l'elem pointe sur pPav %s",
		    pEl->ElSructSchema->SsRule[pEl->ElTypeNumber - 1].SrName);
		printf ("\n");
	     }
	}

      /* troisieme verification */
      /* si pPav a AbNextRepeated(ou Prec) != NULL alors tous ses ascendants ont */
      /* AbNextRepeated != NULL (jusqu'a corps de page) */
      if (pPav->AbNextRepeated != NULL || pPav->AbPreviousRepeated != NULL)
	{
	   pP = pPav->AbEnclosing;
	   if (pP == NULL)
	     {
		correct = False;
		printf ("Erreur dans l'i.a.: la racine a des dupliques %s",
		    pEl->ElSructSchema->SsRule[pEl->ElTypeNumber - 1].SrName);
		printf ("\n");
	     }
	   else
	     {
		while (pP != NULL && pP->AbElement->ElTypeNumber != ord (PageBreak) + 1)
		  {
		     if ((pPav->AbNextRepeated != NULL && pP->AbNextRepeated == NULL)
			 || (pPav->AbPreviousRepeated != NULL && pP->AbPreviousRepeated == NULL))
		       {
			  /*cas d'erreur */
			  correct = False;
			  printf ("Erreur dans l'i.a.: l'ascendant devrait etre dup %s",
				  pP->AbElement->ElSructSchema->SsRule[pP->AbElement->ElTypeNumber
							     - 1].SrName);
			  printf ("\n");
		       }
		     pP = pP->AbEnclosing;
		  }
		if (pP == NULL)
		   /* on n'a pas rencontre le corps de page -> erreur */
		  {
		     correct = False;
		     printf ("Erreur i.a.: dup en dehors un corps de page %s",
		     pEl->ElSructSchema->SsRule[pEl->ElTypeNumber - 1].SrName);
		     printf ("\n");
		  }
	     }
	}
      if (!correct)
	 /* affichage du contexte */
	 AffPaveDebug (pPav);

      return correct;
   }
/** fin ajout */
#endif /* __COLPAGE__ */

/* End Of Module imabs */
