
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*
   frame.c : affichage incremental dans les frames.
   I. Vatton - Mai 87
   IV : Aout 92 coupure des mots
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "libmsg.h"
#include "message.h"

#undef EXPORT
#define EXPORT extern
#include "img.var"
#include "environ.var"

#include "displaybox_f.h"
#include "appli_f.h"
#include "windowdisplay_f.h"
#include "boxlocate_f.h"

#include "font_f.h"
#include "absboxes_f.h"
#include "displayselect_f.h"

#ifdef __STDC__
boolean             AfFinFenetre (int frame, int delta);

#else  /* __STDC__ */
boolean             AfFinFenetre ( /* frame, delta */ );

#endif /* __STDC__ */

/*debut */
/* ------------------------------------------------------------------- */
/* | On fait une conversion de coordonnees                           | */
/* ------------------------------------------------------------------- */

#ifdef __STDC__
void                GetXYOrg (int frame, int *XOrg, int *YOrg)

#else  /* __STDC__ */
void                GetXYOrg (frame, XOrg, YOrg)
int                 frame;
int                *XOrg;
int                *YOrg;

#endif /* __STDC__ */

{
   ViewFrame            *pFrame;

   pFrame = &FntrTable[frame - 1];
   *XOrg = pFrame->FrXOrg;
   *YOrg = pFrame->FrYOrg;
}
/*fin */

/* ---------------------------------------------------------------------- */
/* |    DefClip de'limite la zone de reaffichage dans la fenetre frame. | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                DefClip (int frame, int xd, int yd, int xf, int yf)

#else  /* __STDC__ */
void                DefClip (frame, xd, yd, xf, yf)
int                 frame;
int                 xd;
int                 yd;
int                 xf;
int                 yf;

#endif /* __STDC__ */

{
   int                 large, haut;
   ViewFrame            *pFrame;

   if (frame > 0 && frame <= MAX_FRAME)
     {
	pFrame = &FntrTable[frame - 1];
	/* Faut-il prendre toute la largeur de la fenetre? */
	if (xd == xf && xd == -1)
	  {
	     DimFenetre (frame, &large, &haut);
	     pFrame->FrClipXBegin = pFrame->FrXOrg;
	     pFrame->FrClipXEnd = large + pFrame->FrXOrg;
	  }
	/* On termine un reaffichage */
	else if (xd == xf && xd == 0)
	  {
	     pFrame->FrClipXBegin = 0;
	     pFrame->FrClipXEnd = 0;
	  }
	/* Faut-il initialiser la zone de reaffichage? */
	else if (pFrame->FrClipXBegin == pFrame->FrClipXEnd && pFrame->FrClipXBegin == 0)
	  {
	     pFrame->FrClipXBegin = xd;
	     pFrame->FrClipXEnd = xf;
	  }
	/* On met a jour la zone de reaffichage */
	else
	  {
	     if (pFrame->FrClipXBegin > xd)
		pFrame->FrClipXBegin = xd;
	     if (pFrame->FrClipXEnd < xf)
		pFrame->FrClipXEnd = xf;
	  }
	/* Faut-il prendre toute la hauteur de la fenetre? */
	if (yd == yf && yd == -1)
	  {
	     DimFenetre (frame, &large, &haut);
	     pFrame->FrClipYBegin = pFrame->FrYOrg;
	     pFrame->FrClipYEnd = haut + pFrame->FrYOrg;
	     /* On termine un reaffichage */
	  }
	else if (yd == yf && yd == 0)
	  {
	     pFrame->FrClipYBegin = 0;
	     pFrame->FrClipYEnd = 0;
	  }
	/* Faut-il initialiser la zone de reaffichage? */
	else if (pFrame->FrClipYBegin == pFrame->FrClipYEnd && pFrame->FrClipYBegin == 0)
	  {
	     pFrame->FrClipYBegin = yd;
	     pFrame->FrClipYEnd = yf;
	  }
	/* On met a jour la zone de reaffichage */
	else
	  {
	     if (pFrame->FrClipYBegin > yd)
		pFrame->FrClipYBegin = yd;
	     if (pFrame->FrClipYEnd < yf)
		pFrame->FrClipYEnd = yf;
	  }
     }
}				/*DefClip */


/* ---------------------------------------------------------------------- */
/* |    DefRegion de'limite la portion de frame d'indice frame que l'on | */
/* |            doit reafficher.                                        | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                DefRegion (int frame, int xd, int yd, int xf, int yf)

#else  /* __STDC__ */
void                DefRegion (frame, xd, yd, xf, yf)
int                 frame;
int                 xd;
int                 yd;
int                 xf;
int                 yf;

#endif /* __STDC__ */

{
   ViewFrame            *pFrame;


   pFrame = &FntrTable[frame - 1];
   DefClip (frame, xd + pFrame->FrXOrg, yd + pFrame->FrYOrg, xf + pFrame->FrXOrg, yf + pFrame->FrYOrg);
}

/* ajout de fonction */
/* ---------------------------------------------------------------------- */
/* |    TtaRefresh reaffiche toutes les frames de documents.            | */
/* ---------------------------------------------------------------------- */
void                TtaRefresh ()
{
   int                 frame;

   for (frame = 1; frame <= MAX_FRAME; frame++)
     {
	if (FntrTable[frame - 1].FrAbstractBox != NULL)
	  {
	     /* force le reaffichage de toute la fenetre */
	     DefClip (frame, -1, -1, -1, -1);
	     (void) AfFinFenetre (frame, 0);
	  }
     }
}

/* ---------------------------------------------------------------------- */
/* |    AfDebFenetre affiche de bas en haut d'une frame (avec           | */
/* |            eventuellement un scroll en arriere d'une amplitude     | */
/* |            delta).                                                 | */
/* |            Nettoie le rectangle avant de reafficher.               | */
/* |            Les coordonnees origine des boites sont supposees       | */
/* |            calculees.                                              | */
/* |            Rend la valeur Vrai si il a fallu ajouter des paves.    | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
boolean             AfDebFenetre (int frame, int delta)

#else  /* __STDC__ */
boolean             AfDebFenetre (frame, delta)
int                 frame;
int                 delta;

#endif /* __STDC__ */

{
   PtrBox            pBox;
   PtrBox            min, max;
   int                 y, x, vol, h, l;
   int                 haut, bas;
   int                 framexmin;
   int                 framexmax;
   int                 frameymin;
   int                 frameymax;
   boolean             audessous;
   boolean             ajoute;
   int                 plan;
   int                 plansuiv;
   PtrBox            prembox;
   ViewFrame            *pFrame;
   PtrBox            pBo1;
   PtrBox            pBo2;

   /* A priori on n'ajoute pas de nouveaux paves */
   ajoute = FALSE;
   pFrame = &FntrTable[frame - 1];
   if (pFrame->FrReady && pFrame->FrAbstractBox != NULL
       && pFrame->FrClipXBegin < pFrame->FrClipXEnd
       && pFrame->FrClipYBegin < pFrame->FrClipYEnd)
     {
	pFrame->FrYOrg -= delta;
	framexmin = pFrame->FrClipXBegin;
	framexmax = pFrame->FrClipXEnd;
	frameymin = pFrame->FrClipYBegin;
	frameymax = pFrame->FrClipYEnd;
	SetClip (frame, pFrame->FrXOrg, pFrame->FrYOrg, &framexmin, &frameymin, &framexmax, &frameymax, 1);
	DimFenetre (frame, &l, &h);
	haut = pFrame->FrYOrg;
	bas = haut + h;
	/* S'il y a quelque chose a reafficher ? */
	if (framexmin < framexmax && frameymin < frameymax)
	  {
	     /* Recherche la 1ere boite au dessus du bas de la fenetre */
	     /* La recherche commence par la derniere boite affichee */
	     pBox = pFrame->FrAbstractBox->AbBox->BxPrevious;
	     if (pBox == NULL)
		/* Document vide */
		pBox = pFrame->FrAbstractBox->AbBox;
	     audessous = TRUE;
	     min = NULL;
	     max = NULL;
	     vol = 0;
	     /* 1/2 frame au dessous du bas de la fenetre */
	     x = bas + h / 2;
	     while (audessous)
	       {
		  pBo1 = pBox;
		  if (pBo1->BxYOrg < bas)
		     audessous = FALSE;
		  else
		    {
		       if (pBo1->BxYOrg > x)
			  if (pBo1->BxType != BoPiece && pBo1->BxType != BoDotted)
			     /* Ce n'est pas une boite de coupure */
			     vol += pBo1->BxAbstractBox->AbVolume;
			  else if (pBo1->BxAbstractBox->AbBox->BxNexChild == pBox)
			     /* C'est la 1ere boite de coupure */
			     vol += pBo1->BxAbstractBox->AbVolume;
		       if (pBo1->BxPrevious == NULL)
			  audessous = FALSE;
		       pBox = pBo1->BxPrevious;
		    }
	       }

	     /* On affiche les plans dans l'orde n a 0 */
	     plan = 65536;
	     plansuiv = plan - 1;
	     prembox = pBox;
	     while (plan != plansuiv)
		/* Il y a un nouveau plan a afficher */
	       {
		  plan = plansuiv;
		  pBox = prembox;
		  /* On reparcourt toutes les boites */

		  /* Affiche toutes les boites non encore entierement affichees */
		  while (pBox != NULL)
		    {
		       pBo1 = pBox;
		       if (pBo1->BxAbstractBox->AbDepth == plan)
			  /* La boite est affichee dans le plan courant */
			 {
			    y = pBo1->BxYOrg + pBo1->BxHeight;
			    /* Il faut tenir compte du remplissage de fin de bloc */
			    x = pBo1->BxXOrg + pBo1->BxWidth + pBo1->BxEndOfBloc;
			    /* On note la derniere boite au dessus ou non visible */
			    if (y > haut && pBo1->BxYOrg < bas)
			      {
				 if (max == NULL)
				    max = pBox;
				 min = pBox;
			      }
			    if (y >= frameymin
				&& pBo1->BxYOrg <= frameymax
				&& x >= framexmin
				&& pBo1->BxXOrg <= framexmax)
			       AfBoite (pBox, frame);
			    /* On passe a la boite suivante */
			    pBox = pBo1->BxPrevious;
			 }
		       else if (pBo1->BxAbstractBox->AbDepth < plan)
			  /* On retient la plus grande valeur de plan inferieur */
			 {
			    if (plan == plansuiv)
			       plansuiv = pBo1->BxAbstractBox->AbDepth;
			    else if (pBo1->BxAbstractBox->AbDepth > plansuiv)
			       plansuiv = pBo1->BxAbstractBox->AbDepth;
			    pBox = pBo1->BxPrevious;
			 }
		       else
			  pBox = pBo1->BxPrevious;
		    }
	       }

	     /* La zone modifiee est affichee */
	     DefClip (frame, 0, 0, 0, 0);
	     ResetClip (frame);

	     /* On complete eventuellement l'image partielle */
	     pBox = pFrame->FrAbstractBox->AbBox;
	     if (!Complete && !Insert)
	       {
		  pBo1 = pBox;
		  Complete = TRUE;
		  /* On est en train de completer l'image */
		  y = haut - pBo1->BxYOrg;
		  x = h / 2;

		  /* L'image depasse d'une 1/2 frame en haut et en bas */
		  if (vol > 0 && y > x)
		     /* On calcule le volume a retirer */
		    {
		       pBox = pBo1->BxNext;
		       haut -= x;
		       y = 0;
		       while (pBox != NULL)
			 {
			    pBo2 = pBox;
			    if (pBo2->BxYOrg + pBo2->BxHeight > haut)
			       pBox = NULL;
			    else
			      {
				 pBox = pBo2->BxNext;
				 if (pBo2->BxType != BoPiece && pBo2->BxType != BoDotted)
				    y += pBo2->BxAbstractBox->AbVolume;
				 else if (pBo2->BxNexChild == NULL)
				    y += pBo2->BxAbstractBox->AbVolume;
				 /* else while */
			      }
			 }
		       pFrame->FrVolume = pFrame->FrAbstractBox->AbVolume - vol - y;
		    }

		  /* Il manque un morceau d'image concrete en haut de la fenetre */
		  else if (pFrame->FrAbstractBox->AbTruncatedHead && y < 0)
		    {
		       /* On libere des paves en bas */
		       if (vol > 0 && vol < pFrame->FrAbstractBox->AbVolume)
			 {
			    VolReduit (FALSE, vol, frame);
			    DefClip (frame, 0, 0, 0, 0);
			    /* On complete en haut -> On decale toute l'image concrete */
			 }

		       /* Volume a ajouter */
		       if (pFrame->FrAbstractBox == NULL)
			 {
			    printf ("ERR: plus de pave dans %d\n", frame);
			    vol = -pFrame->FrVolume;
			 }
		       else
			  vol = pFrame->FrVolume - pFrame->FrAbstractBox->AbVolume;

		       /* Hauteur a ajouter */
		       x -= y;
		       if (vol <= 0)
			 {
			    /* Volume de la surface manquante */
			    vol = x * l;
			    /* Evaluation en caracteres */
			    vol = VolumCar (vol);
			 }
		       if (min != NULL)
			 {
			    pBo2 = min;
			    y = pBo2->BxYOrg;
			    /* Ancienne limite de la fenetre */
			    x = y + pBo2->BxHeight;
			 }
		       VolAugmente (TRUE, vol, frame);
		       /* On a ajoute des paves */
		       ajoute = TRUE;
		       /* Il faut repositionner la fenetre dans l'image concrete */
		       if (min != NULL)
			 {
			    pBo2 = min;
			    y = -y + pBo2->BxYOrg;
			    /* y=deplacement de l'ancienne 1ere boite */
			    /* Ce qui est deja affiche depend de l'ancienne 1ere boite */
			    pFrame->FrYOrg += y;
			    /* x = limite du reaffichage apres decalage */
			    if (y > 0)
			       x = pBo2->BxYOrg + pBo2->BxHeight;
			    /* Nouvelle position limite */
			    pFrame->FrClipYEnd = x;
			 }
		       else
			  /* Il n'y a pas d'ancienne boite : */
			  /* la fenetre est cadree en haut de l'image */
			  pFrame->FrYOrg = 0;

		       /* On a fini de completer l'image */
		       Complete = FALSE;
		       audessous = AfDebFenetre (frame, 0);
		    }

		  /* Il manque un morceau d'image concrete en bas de la fenetre */
		  else if (pFrame->FrAbstractBox->AbTruncatedTail && bas > pBo1->BxYOrg + pBo1->BxHeight)
		    {
		       y = pBo1->BxYOrg + pBo1->BxHeight;
		       /* Volume de la surface manquante */
		       vol = (bas - y) * l;
		       /* Evaluation en caracteres */
		       vol = VolumCar (vol);

		       /* On nettoie le bas de la fenetre */
		       Clear (frame, l, pFrame->FrYOrg + h - y, 0, y);
		       /* Il faut eviter de boucler quand le volume n'est pas modifie */
		       y = pFrame->FrAbstractBox->AbVolume;
		       VolAugmente (FALSE, vol, frame);
		       y -= pFrame->FrAbstractBox->AbVolume;
		       /* On a fini de completer l'image */
		       Complete = FALSE;
		       if (y == 0)
			  printf ("ERR: Il n'y a rien a ajouter\n");
		       else
			  /* On n'a peut-etre pas fini de completer l'image */
			  audessous = AfFinFenetre (frame, 0);
		    }

		  /* Le volume est satisfaisant */
		  else
		     pFrame->FrVolume = pFrame->FrAbstractBox->AbVolume;
		  /* On a fini de completer l'image */
		  Complete = FALSE;
	       }
	  }
	else
	   /* La zone modifiee n'est pas visible */
	   DefClip (frame, 0, 0, 0, 0);

     }
   else if (pFrame->FrReady)
     {
	/* Il n'y avait rien a afficher */
	DefClip (frame, 0, 0, 0, 0);
	ResetClip (frame);
     }

   return ajoute;
}				/* function AfDebFenetre */


/* ---------------------------------------------------------------------- */
/* |    AjouteACreer enregistre dans adbloc la filiation des boites a`  | */
/* |            creer, en partant de la boite la plus englobante a` la  | */
/* |            boite pBox elle-meme.                                   | */
/* |            Une boite ne peut etre presente qu'une seule fois dans  | */
/* |            adbloc.                                                 | */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
void                AjouteACreer (PtrBox * acreer, PtrBox pBox, int frame)

#else  /* __STDC__ */
void                AjouteACreer (acreer, pBox, frame)
PtrBox           *acreer;
PtrBox            pBox;
int                 frame;

#endif /* __STDC__ */

{
   PtrAbstractBox             pAbbox1;
   int                 i;

   /* Recherche si un pave pere a l'indicateur PavGraphCreation */
   pAbbox1 = pBox->BxAbstractBox->AbEnclosing;
   i = 0;
   while (i == 0)
      if (pAbbox1 == NULL)
	 i = 1;
      else if (pAbbox1->AbBox->BxHorizFlex
	       || pAbbox1->AbBox->BxVertFlex)
	{
	   i = 1;
	   pAbbox1 = NULL;
	}
      else if ((pAbbox1->AbHorizPos.PosUserSpecified)
	       || (pAbbox1->AbVertPos.PosUserSpecified)
	       || (pAbbox1->AbWidth.DimUserSpecified)
	       || (pAbbox1->AbHeight.DimUserSpecified))
	 i = 1;
      else
	 pAbbox1 = pAbbox1->AbEnclosing;

   /* Il existe un pave englobant avec l'indicateur -> meme traitement */
   if (pAbbox1 != NULL)
      AjouteACreer (acreer, pAbbox1->AbBox, frame);

   /* Enregistrement de la boite a creer s'il n'y en a pas une  */
   /* englobante enregistree et si la boite a creer est visible */
   if (*acreer == NULL
       && pBox->BxAbstractBox->AbVisibility >= FntrTable[frame - 1].FrVisibility)
      *acreer = pBox;
}				/*function AjouteACreer */

/* ---------------------------------------------------------------------- */
/* |    AfFinFenetre affiche de haut en bas d'une frame (avec           | */
/* |            eventuellement un scroll en avant d'une amplitude       | */
/* |            delta).                                                 | */
/* |            Nettoie le rectangle avant de reafficher.               | */
/* |            Les coordonnees origine des boites sont supposees       | */
/* |            calculees.                                              | */
/* |            Rend la valeur Vrai si il a fallu ajouter des paves.    | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
boolean             AfFinFenetre (int frame, int delta)

#else  /* __STDC__ */
boolean             AfFinFenetre (frame, delta)
int                 frame;
int                 delta;

#endif /* __STDC__ */

{
   PtrBox            pBox;
   PtrBox            min /*, max */ ;
   int                 y, x, vol, h, l;
   int                 haut, bas;
   int                 framexmin;
   int                 framexmax;
   int                 frameymin;
   int                 frameymax;
   boolean             audessus;
   boolean             ajoute;
   int                 plan;
   int                 plansuiv;
   PtrBox            prembox;
   ViewFrame            *pFrame;
   PtrBox            pBo1;
   PtrAbstractBox             pAbbox1;
   PtrBox            pBo2;
   PtrBox            ACreer;
   int                 i;

   /* A priori on n'ajoute pas de nouveaux paves */
   ajoute = FALSE;
   /* A priori on n'a pas besoin de creer interactivement des boites */
   ACreer = NULL;

   pFrame = &FntrTable[frame - 1];
   if (pFrame->FrReady && pFrame->FrAbstractBox != NULL
       && pFrame->FrClipXBegin < pFrame->FrClipXEnd
       && pFrame->FrClipYBegin < pFrame->FrClipYEnd)
     {
	pFrame->FrYOrg += delta;
	framexmin = pFrame->FrClipXBegin;
	framexmax = pFrame->FrClipXEnd;
	frameymin = pFrame->FrClipYBegin;
	frameymax = pFrame->FrClipYEnd;
	SetClip (frame, pFrame->FrXOrg, pFrame->FrYOrg, &framexmin, &frameymin, &framexmax, &frameymax, 1);
	DimFenetre (frame, &l, &h);
	haut = pFrame->FrYOrg;
	bas = haut + h;

	/* On recherche la premiere boite visible ou en dessous */
	pBox = pFrame->FrAbstractBox->AbBox->BxNext;
	if (pBox == NULL)
	   pBox = pFrame->FrAbstractBox->AbBox;	/* Document vide */

	audessus = TRUE;
	min = NULL;
	/*max = NULL; */
	vol = 0;
	x = haut - h / 2;

	/* On affiche les plans dans l'orde n a 0 */
	plan = 65536;
	plansuiv = plan - 1;
	prembox = pBox;
	while (plan != plansuiv)
	   /* Il y a un nouveau plan a afficher */
	  {
	     plan = plansuiv;
	     pBox = prembox;
	     /* On reparcourt toute les boites */

	     /* On affiche toutes les boites non encore entierement affichees */
	     while (pBox != NULL)
	       {

		  /* Faut-il comptabiliser ce volume comme volume au dessus */
		  if (audessus)
/**485*/ if (pBox->BxYOrg + pBox->BxHeight < haut - h / 4)
		       {
/**HYP*/ if ((pBox->BxType != BoPiece && pBox->BxType != BoDotted)
	     || pBox->BxNexChild == NULL)
			     vol += pBox->BxAbstractBox->AbVolume;
		       }
		     else
			audessus = FALSE;

		  if (pBox->BxAbstractBox != NULL && pBox->BxAbstractBox->AbDepth == plan)
		    {
		       /* La boite est affichee dans le plan courant */
		       y = pBox->BxYOrg + pBox->BxHeight;
		       /* Il faut tenir compte du remplissage de fin de bloc */
		       x = pBox->BxXOrg + pBox->BxWidth + pBox->BxEndOfBloc;

		       /* On note la premiere et derniere boite visible */
		       if (y > haut && pBox->BxYOrg < bas)
			 {
			    if (min == NULL)
			       min = pBox;
			    /*max = pBox; */
			 }	/*if */

		       /* Si la boite est affichee pour la premiere fois, */
		       /* on regarde si elle ou une de ces boites parentes */
		       /* doit etre creee interactivement ? */
		       if (pBox->BxNew)
			 {
			    /* La boite n'est plus nouvelle */
			    pBox->BxNew = 0;
			    pAbbox1 = pBox->BxAbstractBox;
			    i = 0;
			    while (i == 0)
			       if (pAbbox1 == NULL)
				  i = 1;
			       else if (pAbbox1->AbWidth.DimIsPosition
					|| pAbbox1->AbHeight.DimIsPosition)
				 {
				    i = 1;
				    pAbbox1 = NULL;
				 }
			       else if ((pAbbox1->AbHorizPos.PosUserSpecified)
					|| (pAbbox1->AbVertPos.PosUserSpecified)
					|| (pAbbox1->AbWidth.DimUserSpecified)
					|| (pAbbox1->AbHeight.DimUserSpecified))
				  i = 1;
			       else
				  pAbbox1 = pAbbox1->AbEnclosing;
			 }
		       else
			  pAbbox1 = NULL;

/*** Saute la boite a creer interactivement ***/
		       if (pAbbox1 != NULL)
			  /* Enregistre la boite a creer */
			  AjouteACreer (&ACreer, pAbbox1->AbBox, frame);
		       else if (y >= frameymin
				&& pBox->BxYOrg <= frameymax
				&& x >= framexmin
				&& pBox->BxXOrg <= framexmax)
			  AfBoite (pBox, frame);
		       /* On passe a la boite suivante */
		       pBox = pBox->BxNext;
		    }
		  else if (pBox->BxAbstractBox != NULL && pBox->BxAbstractBox->AbDepth < plan)
		    {
		       /* On retient la plus grande valeur de plan inferieur */
		       if (plan == plansuiv)
			  plansuiv = pBox->BxAbstractBox->AbDepth;
		       else if (pBox->BxAbstractBox->AbDepth > plansuiv)
			  plansuiv = pBox->BxAbstractBox->AbDepth;
		       pBox = pBox->BxNext;
		    }
		  else
		     pBox = pBox->BxNext;
	       }
	  }

	/* Creation interactive des boites laissees dans le plan */
	if (ACreer != NULL)
	  {
	     ModeCreation (ACreer, frame);
	     /* Faut-il aussi traiter les boites filles ? */
	     pAbbox1 = ACreer->BxAbstractBox;
	     if (pAbbox1 != NULL)
		pAbbox1 = pAbbox1->AbFirstEnclosed;
	     while (pAbbox1 != NULL)
	       {
		  if ((pAbbox1->AbHorizPos.PosUserSpecified)
		      || (pAbbox1->AbVertPos.PosUserSpecified)
		      || (pAbbox1->AbWidth.DimUserSpecified)
		      || (pAbbox1->AbHeight.DimUserSpecified))
		    {
		       ACreer = pAbbox1->AbBox;
		       ModeCreation (ACreer, frame);
		    }
		  else
		     pAbbox1 = pAbbox1->AbNext;
	       }
	     return (FALSE);
	  }

	/* La zone modifiee est affichee */
	DefClip (frame, 0, 0, 0, 0);
	ResetClip (frame);

	/* On complete eventuellement l'image partielle */
	pBox = pFrame->FrAbstractBox->AbBox;
	if (!Complete && (!Insert || delta > 0))
	  {
	     pBo1 = pBox;
	     Complete = TRUE;
	     /* On est en train de completer l'image */
	     y = haut - pBo1->BxYOrg;

	     if (pFrame->FrAbstractBox->AbInLine)
		Complete = FALSE;

	     /* Il manque un morceau d'image concrete en haut de la fenetre */
	     else if (pFrame->FrAbstractBox->AbTruncatedHead && y < 0)
	       {
		  /* On complete en haut -> On decale toute l'image concrete */
		  haut = h / 2 - y;
		  /* Hauteur a ajouter */
		  haut = haut * l;
		  /* Volume de la surface manquante */
		  if (min != NULL)
		    {
		       pBo2 = min;
		       y = pBo2->BxYOrg;	/* ancienne position */
		       x = y + pBo2->BxHeight;
		    }
		  VolAugmente (TRUE, VolumCar (haut), frame);
		  ajoute = TRUE;
		  /* On ajoute des paves en tete */
		  /* Il faut repositionner la fenetre dans l'image concrete */
		  if (min != NULL)
		    {
		       pBo2 = min;
		       y = -y + pBo2->BxYOrg;
		       /* y=deplacement de l'ancienne 1ere boite */
		       /* Ce qui est deja affiche depend de l'ancienne 1ere boite */
		       pFrame->FrYOrg += y;
		       /* x = limite du reaffichage apres decalage */
		       if (y > 0)
			  x = pBo2->BxYOrg + pBo2->BxHeight;
		       /* Nouvelle position limite */
		       pFrame->FrClipYEnd = x;
		    }
		  audessus = AfDebFenetre (frame, 0);
	       }
	     y = pFrame->FrYOrg + h - pBo1->BxYOrg - pBo1->BxHeight;
	     x = h / 2;

	     /* L'image depasse d'une 1/2 frame en haut et en bas */
	     if (vol > 0 && -y > x)
		/* On calcule le volume a retirer */
	       {
		  pBox = pBo1->BxPrevious;
		  bas += x;
		  y = 0;
		  while (pBox != NULL)
		    {
		       pBo2 = pBox;
		       if (pBo2->BxYOrg < bas)
			  pBox = NULL;
		       else
			 {
			    pBox = pBo2->BxPrevious;
			    if (pBo2->BxType != BoPiece && pBo2->BxType != BoDotted)
			       y += pBo2->BxAbstractBox->AbVolume;
			    else if (pBo2->BxAbstractBox->AbBox->BxNexChild == pBox)
			       y += pBo2->BxAbstractBox->AbVolume;
			 }
		    }
		  pFrame->FrVolume = pFrame->FrAbstractBox->AbVolume - vol - y;
	       }

	     /* Il manque un morceau d'image concrete en bas de la fenetre */
	     else if (pFrame->FrAbstractBox->AbTruncatedTail && y > 0)
		/* On libere des paves en haut */
	       {
		  if (vol > 0 && vol < pFrame->FrAbstractBox->AbVolume)
		    {
		       if (min != NULL)
			  y = min->BxYOrg;
		       VolReduit (TRUE, vol, frame);
		       DefClip (frame, 0, 0, 0, 0);
		       /* Il faut repositionner la fenetre dans l'image concrete */
		       if (min != NULL)
			  pFrame->FrYOrg = pFrame->FrYOrg - y + min->BxYOrg;
		       /* On complete en bas */
		    }
		  if (pFrame->FrAbstractBox == NULL)
		    {
		       printf ("ERR: Plus de pave dans %d\n", frame);
		       vol = -pFrame->FrVolume;
		    }
		  else
		     vol = pFrame->FrVolume - pFrame->FrAbstractBox->AbVolume;

		  /* Volume a ajouter */
		  if (vol <= 0)
		    {
		       /* Volume de la surface manquante */
		       vol = (y + x) * l;
		       /* Evaluation en caracteres */
		       vol = VolumCar (vol);
		    }

		  y = pBo1->BxYOrg + pBo1->BxHeight;
		  /* On nettoie le bas de la fenetre */
		  Clear (frame, l, pFrame->FrYOrg + h - y, 0, y);
		  /* Il faut eviter de boucler quand le volume n'est pas modifie */
		  y = pFrame->FrAbstractBox->AbVolume;
		  VolAugmente (FALSE, vol, frame);
		  y -= pFrame->FrAbstractBox->AbVolume;
		  /* On a fini de completer l'image */
		  Complete = FALSE;
		  if (y == 0)
		     printf ("ERR: Il n'y a rien a ajouter\n");
		  else
		     /* On n'a peut-etre pas fini de completer l'image */
		     audessus = AfFinFenetre (frame, 0);
	       }

	     /* Le volume est satisfaisant */
	     else
	       {
		  /* Faut-il nettoyer le bas de la fenetre ? */
		  if (y > 0)
		     Clear (frame, l, y, 0, h - y);
		  pFrame->FrVolume = pFrame->FrAbstractBox->AbVolume;
	       }

	     /* On a fini de completer l'image */
	     Complete = FALSE;
	  }


     }
   else if (pFrame->FrReady)
     {
	/* Il n'y avait rien a afficher */
	DefClip (frame, 0, 0, 0, 0);
	ResetClip (frame);
     }
   FirstCreation = FALSE;

#ifdef NEW_WILLOWS
   WIN_ReleaseDeviceContext ();
#endif /* NEW_WILLOWS */

   return ajoute;
}				/* function AfFinFenetre */

/* ---------------------------------------------------------------------- */
/* |    AfficherVue affiche une vue de document dans la fenetre frame.  | */
/* |            Si une portion de l'image abstraite est selectionnee,   | */
/* |            l'image concrete qui lui correspond est positionnee     | */
/* |            au centre de la fenetre.                                | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                AfficherVue (int frame)

#else  /* __STDC__ */
void                AfficherVue (frame)
int                 frame;

#endif /* __STDC__ */

{
   /*boolean ajoute; */
   ViewFrame            *pFrame;
   int                 l, h;

   /* Est-ce que la vue existe et n'est pas en cours d'evaluation? */
   pFrame = &FntrTable[frame - 1];
   if (pFrame->FrAbstractBox != NULL)
     {
	/* Affichage du rectangle mis a jour */
	AfFinFenetre (frame, 0);
	/* Mise en evidence des boites en erreur */
	if (Erreur)
	  {
	     MajPavSelect (frame, pFrame->FrAbstractBox, TRUE);
	     if (HardMsgAff)
		/* Attend un moment avant d'annuler les mises en evidences */
	       {
		  TtaDisplaySimpleMessage (INFO, LIB, LIB_CONFIRM);
	       }
	     MajPavSelect (frame, pFrame->FrAbstractBox, FALSE);
	     Erreur = FALSE;
	  }

	/* Reevalue les ascenseurs */
	MajScrolls (frame);

     }
   else
     {
	/* Il faut simplement nettoyer la fenetre */
	DimFenetre (frame, &l, &h);
	Clear (frame, l, h, 0, 0);
     }
}				/* procedure AfficherVue */

/* End Of Module frame */
