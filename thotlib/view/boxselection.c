/*
   Module de gestion des Selections.
   I. Vatton
 */

#include "thot_gui.h"
#include "thot_sys.h"
#include "functions.h"
#include "constmedia.h"
#include "typemedia.h"
#include "message.h"
#include "appdialogue.h"

#define EXPORT extern
#include "creation.var"
#include "img.var"
#include "frame.var"
#include "select.var"
#include "appdialogue.var"

#include "appli_f.h"
#include "textcommands_f.h"
#include "editcommands_f.h"
#include "structcreation_f.h"
#include "scroll_f.h"
#include "boxmoves_f.h"
#include "boxlocate_f.h"
#include "font_f.h"
#include "buildboxes_f.h"
#include "memory_f.h"
#include "boxselection_f.h"
#include "structselect_f.h"
#include "displayselect_f.h"


/* ---------------------------------------------------------------------- */
/* |    RazPavSelect parcours l'arborescence pour annuler toutes les    | */
/* |            selections de pave.                                     | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         RazPavSelect (PtrAbstractBox pAb)
#else  /* __STDC__ */
static void         RazPavSelect (pAb)
PtrAbstractBox             pAb;

#endif /* __STDC__ */
{
   PtrAbstractBox             pChildAb;
   PtrAbstractBox             pAbbox1;

   pAbbox1 = pAb;
   if (pAbbox1->AbSelected)
     {
	/* Le pave est selectionne */
	pAbbox1->AbSelected = FALSE;
     }
   else
     {
	/* Sinon on parcours le sous-arbre */
	pChildAb = pAbbox1->AbFirstEnclosed;
	while (pChildAb != NULL)
	  {
	     RazPavSelect (pChildAb);
	     pChildAb = pChildAb->AbNext;
	  }
     }
}

/* ---------------------------------------------------------------------- */
/* |    AnnuleMrq annule la selection courante dans la fenetre.         | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                AnnuleMrq (int frame)
#else  /* __STDC__ */
void                AnnuleMrq (frame)
int                 frame;

#endif /* __STDC__ */
{
   ViewFrame            *pFrame;

   if (frame > 0)
     {
	pFrame = &FntrTable[frame - 1];
	if (pFrame->FrAbstractBox != NULL)
	   RazPavSelect (pFrame->FrAbstractBox);
	pFrame->FrSelectOneBox = FALSE;
	pFrame->FrSelectionBegin.VsBox = NULL;
	pFrame->FrSelectionEnd.VsBox = NULL;
     }
}

/* ---------------------------------------------------------------------- */
/* |    SetSelect bascule la mise en e'vidence de la se'lection dans    | */
/* |            la fenetree^tre frame :                                 | */
/* |            - si Allume est Vrai et que la se'lection est eteinte,  | */
/* |            - ou si Allume est Faux et la se'lection allume'e.      | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                SetSelect (int frame, boolean allume)
#else  /* __STDC__ */
void                SetSelect (frame, allume)
int                 frame;
boolean             allume;

#endif /* __STDC__ */
{
   /* Visualisation de la selection locale */
   if (frame > 0)
     {
	/* Teste le booleen allume et l'etat de la selection */
	if (allume && !FntrTable[frame - 1].FrSelectShown)
	   VisuSel (frame, TRUE);
	else if (!allume && FntrTable[frame - 1].FrSelectShown)
	   VisuSel (frame, TRUE);
     }
}


/* ---------------------------------------------------------------------- */
/* |    ResetSelect bascule et annule la mise en evidence de la         | */
/* |            selection dans la fenetre.                              | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                ResetSelect (int frame)
#else  /* __STDC__ */
void                ResetSelect (frame)
int                 frame;

#endif /* __STDC__ */
{
   ViewFrame            *pFrame;

   if (frame > 0)
     {
	pFrame = &FntrTable[frame - 1];
	/* On eteint la selection ssi elle est allumee */
	if (pFrame->FrSelectShown)
	   VisuSel (frame, FALSE);
	else
	   MajPavSelect (frame, pFrame->FrAbstractBox, FALSE);
	pFrame->FrSelectOneBox = FALSE;
	pFrame->FrSelectionBegin.VsBox = NULL;
	pFrame->FrSelectionEnd.VsBox = NULL;
     }
}


/* ---------------------------------------------------------------------- */
/* |    RazSelect annule et bascule toutes les selections courantes     | */
/* |            visualisees.                                            | */
/* ---------------------------------------------------------------------- */
void                RazSelect ()
{
   int                 i;

   /* On annule et on bascule dans chaque frame la selection courante */
   for (i = 1; i <= MAX_FRAME; i++)
      if (FntrTable[i - 1].FrAbstractBox != NULL)
	 ResetSelect (i);
}


/* ---------------------------------------------------------------------- */
/* |    Detruit le buffer donne en parametre, met a jour les marques de selction  | */
/* |    et rend le pointeur sur le buffer precedent.                    | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
PtrTextBuffer      DestBuff (PtrTextBuffer adbuff, int frame)
#else  /* __STDC__ */
PtrTextBuffer      DestBuff (adbuff, frame)
PtrTextBuffer      adbuff;
int                 frame;

#endif /* __STDC__ */
{
   PtrTextBuffer      nbuff;
   PtrTextBuffer      pbuff;
   int                 longueur;
   ViewFrame            *pFrame;
   ViewSelection            *pViewSel;
   ViewSelection            *pMa2;

   nbuff = adbuff->BuNext;
   pbuff = adbuff->BuPrevious;
   if (pbuff != NULL)
     {
	pbuff->BuNext = nbuff;
	longueur = pbuff->BuLength;
     }
   if (nbuff != NULL)
      nbuff->BuPrevious = pbuff;

   /* Mise a jour des marques de selection courante */
   pFrame = &FntrTable[frame - 1];
   pViewSel = &pFrame->FrSelectionBegin;
   if (pViewSel->VsBuffer == adbuff)
     {
	if (pFrame->FrSelectionEnd.VsBuffer == pViewSel->VsBuffer)
	  {
	     pMa2 = &pFrame->FrSelectionEnd;
	     if (pbuff != NULL)
	       {
		  /* On deplace la selection dans les buffers */
		  pMa2->VsIndBuf += longueur;
		  pMa2->VsBuffer = pbuff;
	       }
	     else
	       {
		  pMa2->VsIndBuf = 1;
		  pMa2->VsBuffer = nbuff;
	       }
	  }
	pViewSel->VsBuffer = pbuff;

	if (pbuff != NULL)
	  {
	     /* On deplace la selection dans les buffers */
	     pViewSel->VsIndBuf += longueur;
	     pViewSel->VsBuffer = pbuff;
	  }
	else
	  {
	     pViewSel->VsIndBuf = 1;
	     pViewSel->VsBuffer = nbuff;
	  }
     }

   FreeBufTexte (adbuff);
   return pbuff;
}

/* ---------------------------------------------------------------------- */
/* |    MajMrq met a jour les marques de selection de la fenetre apres  | */
/* |            insertion ou destruction de caracteres.                 | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                MajMrq (int frame, int dx, int dblanc, int dcar)
#else  /* __STDC__ */
void                MajMrq (frame, dx, dblanc, dcar)
int                 frame;
int                 dx;
int                 dblanc;
int                 dcar;

#endif /* __STDC__ */
{
   ViewFrame            *pFrame;
   ViewSelection            *pViewSel;

   pFrame = &FntrTable[frame - 1];
   pViewSel = &pFrame->FrSelectionBegin;
   pViewSel->VsXPos += dx;
   pViewSel->VsIndBox += dcar;
   pViewSel->VsNSpaces += dblanc;
   pViewSel = &pFrame->FrSelectionEnd;
   if (pViewSel->VsBox == pFrame->FrSelectionBegin.VsBox)
     {
	pViewSel->VsXPos += dx;
	pViewSel->VsIndBox += dcar;
	pViewSel->VsNSpaces += dblanc;
     }
}


/* ---------------------------------------------------------------------- */
/* |    ReevalMrq met a jour la marque de selection connaissant la boite| */
/* |            entiere (VsBox), le buffer (VsBuffer) et l'index du       | */
/* |            caractere (VsIndBuf) marque'.                              | */
/* |            Deduit l'index caractere (VsIndBox), le nombre de blancs   | */
/* |            le precedant (VsNSpaces), la position dans la boite (VsXPos) | */
/* |            et la ligne contenant la boite (VsLine).               | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                ReevalMrq (ViewSelection * marque)
#else  /* __STDC__ */
void                ReevalMrq (marque)
ViewSelection            *marque;

#endif /* __STDC__ */
{
   PtrTextBuffer      adbuff;
   PtrTextBuffer      ibuff;
   PtrBox            pBox;
   int                 deb, x;
   int                 carbl;
   int                 max, i;
   int                 saut;
   boolean             fin;


   if (marque->VsBox->BxAbstractBox->AbLeafType == LtText)
     {
	/* On note l'index et le buffer du caractere precedant la marque */
	ibuff = marque->VsBuffer;
	pBox = marque->VsBox;
	if (marque->VsIndBuf == 1 && ibuff != pBox->BxBuffer)
	  {
	     if (ibuff->BuPrevious == NULL)
		ibuff = pBox->BxBuffer;
	     else
		ibuff = ibuff->BuPrevious;	/* En debut de buffer */
	     i = ibuff->BuLength;
	  }
	else
	   i = marque->VsIndBuf - 1;	/* En fin ou en cours de buffer */
	fin = FALSE;

	/* Est-ce une boite coupee ? */
	if (pBox->BxType == BoSplit)
	   pBox = pBox->BxNexChild;

	/* Recherche l'index du caractere et la boite de coupure */
	adbuff = pBox->BxBuffer;
	deb = 1 - pBox->BxFirstChar;
	max = pBox->BxNChars;
	/* Calcule le saut entre cette boite et la suivante pour */
	/* determiner si on peut selectionner en fin de boite */
	if (pBox->BxNexChild == NULL)
	   saut = 0;
	else
	   saut = pBox->BxNexChild->BxIndChar - pBox->BxIndChar - pBox->BxNChars;

	/* Boucle tant que le caractere designe se trouve dans */
	/* le buffer suivant ou dans la boite suivante */
	while (!fin && (adbuff != ibuff || max - deb <= i - saut))
	   if (max - deb <= i - saut)
	     {
		/* Box de coupure Suivante */
		/* Cas particulier des blancs supprimes en fin de boite */
		/* Est-ce qu'il y a une boite apres ? */
		if (pBox->BxNexChild == NULL)
		   fin = TRUE;
		/* Il existe une boite apres mais c'est une boite fantome */
		else if (pBox->BxNexChild->BxNChars == 0 && SearchLine (pBox->BxNexChild) == NULL)
		   fin = TRUE;
		else
		   fin = max - deb + saut > i && adbuff == ibuff;

		/* Est-ce que la selection est en fin de boite ? */
		if (fin)
		  {
		     saut = i - max + deb;
		     /* Position dans les blancs de fin de ligne */
		     marque->VsIndBox = pBox->BxNChars + saut;
		     marque->VsXPos = pBox->BxWidth;
		     marque->VsNSpaces = pBox->BxNSpaces + saut;
		  }
		/* Sinon on passe a la boite suivante */
		else
		  {
		     pBox = pBox->BxNexChild;
		     deb = 1 - pBox->BxFirstChar;
		     max = pBox->BxNChars;
		     adbuff = pBox->BxBuffer;
		     /* Calcule le saut entre cette boite et la suivante pour */
		     /* determiner si on peut selectionner en fin de boite */
		     if (pBox->BxNexChild == NULL)
			saut = 0;
		     else
			saut = pBox->BxNexChild->BxIndChar - pBox->BxIndChar - pBox->BxNChars;
		     /* Cas particulier du premier caractere d'une boite coupee */
		     if (ibuff == adbuff->BuPrevious)
		       {
			  marque->VsIndBox = 0;
			  marque->VsXPos = 0;
			  marque->VsNSpaces = 0;
			  fin = TRUE;
		       }
		  }
	     }
	   else
	     {
		deb += adbuff->BuLength;
		adbuff = adbuff->BuNext;
	     }

	/* On a trouve la boite de coupure */
	if (!fin)
	  {
	     marque->VsIndBox = deb + i;
	     /* Reevaluation du decalage dans la boite */
	     carbl = pBox->BxSpaceWidth;	/* 0 si on prend la largeur reelle du blanc */
	     x = pBox->BxFirstChar;	/* Index du premier caractere a traiter */
	     GiveTextParams (pBox->BxBuffer, marque->VsIndBox, pBox->BxFont, &x, &carbl);
	     marque->VsXPos = x;
	     marque->VsNSpaces = carbl;
	     /* On ajoute eventuellement les pixels repartis */
	     if (pBox->BxSpaceWidth != 0)
		if (marque->VsNSpaces < pBox->BxNPixels)
		   marque->VsXPos += marque->VsNSpaces;
		else
		   marque->VsXPos += pBox->BxNPixels;
	  }
	marque->VsBox = pBox;
     }
   marque->VsLine = SearchLine (marque->VsBox);
}


/* ---------------------------------------------------------------------- */
/* |    IndBuffer parcours les buffers de la boite de texte pour trouver| */
/* |            celui qui contient le caractere d'indice global index   | */
/* |            ainsi que son indice dans ce buffer.                    | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         IndBuffer (PtrTextBuffer * ibuff, int *index)

#else  /* __STDC__ */
static void         IndBuffer (ibuff, index)
PtrTextBuffer     *ibuff;
int                *index;

#endif /* __STDC__ */

{
   boolean             encore;
   PtrTextBuffer      pBu1;

   encore = *ibuff != NULL;
   while (encore)
     {
	pBu1 = *ibuff;
	/* Est-ce le bon buffer ? */
	if ((*ibuff)->BuLength < *index)
	   /* Non : Il faut passer au buffer suivant */
	   if (pBu1->BuNext == NULL)
	     {
		/* On arrive en fin de liste de buffers sans trouver le caractere */
		*index = pBu1->BuLength + 1;
		encore = FALSE;
	     }
	   else
	      /* On passe au buffer suivant */
	     {
		*index -= pBu1->BuLength;
		*ibuff = pBu1->BuNext;
	     }
	else
	   encore = FALSE;
     }
}


/* ---------------------------------------------------------------------- */
/* |    PoseSelect traite la selection courante sur la portion de       | */
/* |            document visualisee dans une frame du Mediateur. Le     | */
/* |            pave Pav correspond soit au debut de la selection       | */
/* |            (debut est Vrai), soit la fin de la selection (fin est  | */
/* |            vrai), soit les deux. Le parametre c1 donne l'indice    | */
/* |            du premier caractere selectionne ou 0 si tout le pave   | */
/* |            est selectionne.                                        | */
/* |            Le parametre cN donne l'indice du caractere qui suit le | */
/* |            dernier selectionne'.                                   | */
/* |            Le parametre Unique indique que la selection reelle     | */
/* |            donc visualisee porte sur un seul et unique pave.       | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                PoseSelect (int frame, PtrAbstractBox Pav, int c1, int cN, boolean Debut, boolean Fin, boolean Unique)
#else  /* __STDC__ */
void                PoseSelect (frame, Pav, c1, cN, Debut, Fin, Unique)
int                 frame;
PtrAbstractBox             Pav;
int                 c1;
int                 cN;
boolean             Debut;
boolean             Fin;
boolean             Unique;

#endif /* __STDC__ */
{
   PtrLine            adligne;
   PtrTextBuffer      adbuff;
   int                 ind, icar;
   PtrBox            pBox;
   ViewFrame            *pFrame;
   ViewSelection            *pViewSel;

   /* Verifie s'il faut reformater le dernier paragraphe edite */
   if (ThotLocalActions[T_updateparagraph] != NULL)
      (*ThotLocalActions[T_updateparagraph]) (Pav, frame);

   if (Pav != NULL && frame > 0)
     {
	pFrame = &FntrTable[frame - 1];
	if (Pav->AbBox != NULL)
	  {
	     /* On eteint la selection */
	     pBox = Pav->AbBox;
	     adligne = SearchLine (pBox);

	     /* On verifie la coherence des indices de caracteres */
	     if (Pav->AbLeafType == LtText)
		/* C'est une feuille de texte */
	       {
		  if (c1 == 0 && cN != 0)
		     c1 = 1;
		  else if (c1 != 0 && cN == 0)
		     cN = Pav->AbVolume;
		  else if (c1 == 0 && Pav->AbVolume != 0)
		    {
		       c1 = 1;	/* On selection tout le texte du pave */
		       cN = Pav->AbVolume;
		    }
	       }
	     else if (Pav->AbLeafType != LtPlyLine && Pav->AbLeafType != LtPicture)
		c1 = 0;

	     /* On memorise si la selection relle porte sur un seul pave ou non */
	     pFrame->FrSelectOneBox = Unique;
	     /* et si elle indique seulement une position */
	     pFrame->FrSelectOnePosition = SelPosition;

	     /* La selection porte sur le pave complet ou un point de controle */
	     /* de pave polyline */
	     if (c1 == 0 || Pav->AbVolume == 0 || Pav->AbLeafType == LtPlyLine || Pav->AbLeafType == LtPicture)
	       {
		  /* Est-ce une boite de texte ? */
		  if (Pav->AbLeafType == LtText)
		    {
		       ind = 1;
		       adbuff = Pav->AbText;
		    }
		  else
		    {
		       ind = 0;
		       adbuff = NULL;
		    }

		  /* On memorise les marques de selection */
		  if (Debut)
		    {
		       pViewSel = &pFrame->FrSelectionBegin;
		       pViewSel->VsBox = pBox;
		       if (Fin && Pav->AbLeafType != LtPlyLine && Pav->AbLeafType != LtPicture)
			  pViewSel->VsIndBox = 0;	/* tout selectionne */
		       else
			  pViewSel->VsIndBox = c1;
		       pViewSel->VsIndBuf = ind;
		       pViewSel->VsBuffer = adbuff;
		       pViewSel->VsLine = adligne;
		       if (Pav->AbLeafType == LtPicture && c1 > 0)
			  pViewSel->VsXPos = pBox->BxWidth;
		       else
			  pViewSel->VsXPos = 0;
		       pViewSel->VsNSpaces = 0;
		    }
		  if (Fin)
		    {
		       pViewSel = &pFrame->FrSelectionEnd;
		       pViewSel->VsBox = pBox;
		       pViewSel->VsIndBox = 0;
		       pViewSel->VsIndBuf = ind;
		       pViewSel->VsBuffer = adbuff;
		       pViewSel->VsLine = adligne;
		       if (Pav->AbLeafType == LtPicture && c1 > 0)
			  pViewSel->VsXPos = pBox->BxWidth;
		       else
			  pViewSel->VsXPos = 0;
		       pViewSel->VsNSpaces = 0;
		    }

	       }
	     /* La selection porte sur une sous-chaine */
	     else
	       {
		  /* On recherche le buffer et l'index dans ce buffer */
		  if (Debut)
		     ind = c1;
		  else
		     ind = cN;
		  adbuff = Pav->AbText;
		  if (ind > Pav->AbVolume)
		    {
		       /* En fin de boite */
		       icar = Pav->AbVolume;
		       IndBuffer (&adbuff, &ind);	/* On recherche le buffer et l'indice */
		    }
		  else
		    {
		       /* Au milieu de la boite */
		       icar = ind - 1;
		       IndBuffer (&adbuff, &ind);	/* On recherche le buffer et l'indice */
		    }

		  /* On met a jour le debut de selection */
		  if (Debut)
		    {
		       pViewSel = &pFrame->FrSelectionBegin;
		       pViewSel->VsBox = pBox;
		       pViewSel->VsIndBox = icar;
		       pViewSel->VsIndBuf = ind;
		       pViewSel->VsBuffer = adbuff;
		       ReevalMrq (&pFrame->FrSelectionBegin);
		    }
		  /* On met a jour la fin de selection */
		  if (Fin)
		    {
		       pViewSel = &pFrame->FrSelectionEnd;
		       /* Debut et Fin sur le meme caractere */
		       if (Debut && c1 >= cN)
			 {
			    pViewSel->VsBox = pFrame->FrSelectionBegin.VsBox;
			    pViewSel->VsIndBox = pFrame->FrSelectionBegin.VsIndBox;
			    pViewSel->VsLine = pFrame->FrSelectionBegin.VsLine;
			    pViewSel->VsBuffer = pFrame->FrSelectionBegin.VsBuffer;
			    pViewSel->VsIndBuf = pFrame->FrSelectionBegin.VsIndBuf;
			    pViewSel->VsXPos = pFrame->FrSelectionBegin.VsXPos;
			    pViewSel->VsNSpaces = pFrame->FrSelectionBegin.VsNSpaces;
			 }
		       else
			 {
			    /* Debut et Fin sur deux caracteres differents */
			    if (Debut)
			      {
				 adbuff = Pav->AbText;
				 ind = cN;
				 if (ind > Pav->AbVolume)
				   {
				      /* En fin de boite */
				      icar = Pav->AbVolume;
				      IndBuffer (&adbuff, &ind);	/* On recherche le buffer et l'indice */
				   }
				 else
				   {
				      /* Au milieu de la boite */
				      icar = ind - 1;
				      IndBuffer (&adbuff, &ind);	/* On recherche le buffer et l'indice */
				   }
			      }
			    pViewSel->VsBox = pBox;
			    pViewSel->VsIndBox = icar;
			    pViewSel->VsIndBuf = ind;
			    pViewSel->VsBuffer = adbuff;
			    ReevalMrq (&pFrame->FrSelectionEnd);
			 }

		       /* On recherche la position limite du caractere */
		       pBox = pViewSel->VsBox;
		       if (pBox->BxNChars == 0 && pBox->BxType == BoComplete)
			  pViewSel->VsXPos += pBox->BxWidth;
		       else if (pViewSel->VsIndBox == pBox->BxNChars)
			  pViewSel->VsXPos += 2;
		       else
			 {
			    icar = (int) (pViewSel->VsBuffer->BuContent[pViewSel->VsIndBuf - 1]);
			    if (icar == BLANC && pBox->BxSpaceWidth != 0)
			       pViewSel->VsXPos += pBox->BxSpaceWidth;
			    else
			       pViewSel->VsXPos += CarWidth (icar, pBox->BxFont);
			 }
		    }
	       }
	  }
     }
}


/* ---------------------------------------------------------------------- */
/* |    PoseMrq repe`re le pave' et e'ventuellement le caracte`re       | */
/* |    se'lectionne'. La valeur de bouton, indique s'il s'agit         | */
/* |    d'une marque initiale ou d'une extension de se'lection :        | */
/* |    - 0 s'il s'agit d'une extension de se'lection.                  | */
/* |    - 1 s'il s'agit d'un drag.                                      | */
/* |    - 2 s'il s'agit d'une marque initiale.                          | */
/* |    - 3 s'il s'agit d'un double clic.                               | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                PoseMrq (int frame, int x, int y, int bouton)
#else  /* __STDC__ */
void                PoseMrq (frame, x, y, bouton)
int                 frame;
int                 x;
int                 y;
int                 bouton;

#endif /* __STDC__ */
{
   int                 indbuff;
   PtrBox            pBox;

#ifdef IV
   PtrBox            testbox;

#endif
   PtrTextBuffer      adbuff;
   PtrAbstractBox             paved;
   int                 nbcar;
   int                 nbbl;
   ViewFrame            *pFrame;

   if (frame >= 1)
     {
	/* On recherche si une boite terminale est designee */
	pFrame = &FntrTable[frame - 1];
	x += pFrame->FrXOrg;
	y += pFrame->FrYOrg;
	paved = pFrame->FrAbstractBox;
	nbcar = 0;
	/* On recupere la boite selectionnee */
#ifdef IV
	pBox = DesBoiteTerm (frame, x, y);
	/* recupere eventuellement le point controle */
	if (pBox != NULL && pBox->BxAbstractBox != NULL
	    && pBox->BxAbstractBox->AbLeafType == LtPlyLine)
	   testbox = DansLaBoite (pBox->BxAbstractBox, x - 2, x, y, &nbcar);
#else
	if (ThotLocalActions[T_selecbox] != NULL)
	   (*ThotLocalActions[T_selecbox]) (&pBox, paved, frame, x, y, &nbcar);
#endif
	/* S'il s'agit d'une extension de la selection */
	/* il faut eviter de selectionner la boite englobante */
	if (bouton == 0 || bouton == 1)
	  {
	     if (IsParentBox (pBox, pFrame->FrSelectionBegin.VsBox))
		pBox = DesBoiteTerm (frame, x, y);
	  }
	if (pBox != NULL)
	  {
	     paved = pBox->BxAbstractBox;
	     if (paved->AbLeafType == LtText &&
		 (!paved->AbPresentationBox || paved->AbCanBeModified))
	       {
		  x -= pBox->BxXOrg;
		  DesCaractere (pBox, &adbuff, &x, &indbuff, &nbcar, &nbbl);
		  nbcar = pBox->BxIndChar + nbcar + 1;
	       }
	  }
	else
	   paved = NULL;

	EndInsert ();
	if (paved != NULL)
	   /* Initialisation de la selection */
	   if (bouton == 3)
	      SelectCour (frame, paved, nbcar, FALSE, TRUE, TRUE, FALSE);
	   else if (bouton == 2)
	      SelectCour (frame, paved, nbcar, FALSE, TRUE, FALSE, FALSE);
	/* Extension de la selection */
	   else if (bouton == 0)
	      SelectCour (frame, paved, nbcar, TRUE, TRUE, FALSE, FALSE);
	   else if (bouton == 1)
	      SelectCour (frame, paved, nbcar, TRUE, TRUE, FALSE, TRUE);
     }
}


/* ---------------------------------------------------------------------- */
/* |    PavPosFen rend la position en Y d'un pave par rapport a la      | */
/* |            frame. Si axe = 0, position du haut du pave, sinon si | */
/* |            axe = 1 du milieu sinon si axe = 2 du bas.              | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
int                 PavPosFen (PtrAbstractBox pav, int frame, int axe)
#else  /* __STDC__ */
int                 PavPosFen (pav, frame, axe)
PtrAbstractBox             pav;
int                 frame;
int                 axe;

#endif /* __STDC__ */
{
   int                 delta;
   ViewFrame            *pFrame;
   PtrBox            pBo1;

   delta = -100000;
   if (pav != NULL)
      if (pav->AbBox != NULL)
	{
	   pFrame = &FntrTable[frame - 1];
	   pBo1 = pav->AbBox;
	   delta = pBo1->BxYOrg - pFrame->FrYOrg;
	   if (axe == 1)
	      delta += (pBo1->BxHeight + 1) / 2;
	   else if (axe == 2)
	      delta += pBo1->BxHeight;
	}
   return delta;
}

/* ---------------------------------------------------------------------- */
/* |    PaveAffiche rend la valeur vrai si le pave est affiche' dans la | */
/* |            frame.                                          | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
boolean             PaveAffiche (PtrAbstractBox pav, int frame)
#else  /* __STDC__ */
boolean             PaveAffiche (pav, frame)
PtrAbstractBox             pav;
int                 frame;

#endif /* __STDC__ */
{
   int                 val1, val2;

   if (pav == NULL)
      return FALSE;
   else
      while (pav->AbBox == NULL)
	 /* On remonte au pave englobant cree */
	 if (pav->AbEnclosing == NULL)
	    return FALSE;
	 else
	    pav = pav->AbEnclosing;

   /* On regarde si le pave est affiche dans la fenetre */
   DimFenetre (frame, &val1, &val2);
   val1 = FntrTable[frame - 1].FrYOrg;
   val2 += val1;
   if (pav->AbBox->BxYOrg + pav->AbBox->BxHeight < val1
       || pav->AbBox->BxYOrg > val2)
      return FALSE;
   else
      return TRUE;
}
/* End Of Module sel */
