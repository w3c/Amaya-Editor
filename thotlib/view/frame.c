
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

#include "aff.f"
#include "appli.f"
#include "es.f"
#include "des.f"

#include "font.f"
#include "imabs.f"
#include "visu.f"

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
   ViewFrame            *pFe1;

   pFe1 = &FntrTable[frame - 1];
   *XOrg = pFe1->FrXOrg;
   *YOrg = pFe1->FrYOrg;
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
   ViewFrame            *pFe1;

   if (frame > 0 && frame <= MAX_FRAME)
     {
	pFe1 = &FntrTable[frame - 1];
	/* Faut-il prendre toute la largeur de la fenetre? */
	if (xd == xf && xd == -1)
	  {
	     DimFenetre (frame, &large, &haut);
	     pFe1->FrClipXBegin = pFe1->FrXOrg;
	     pFe1->FrClipXEnd = large + pFe1->FrXOrg;
	  }
	/* On termine un reaffichage */
	else if (xd == xf && xd == 0)
	  {
	     pFe1->FrClipXBegin = 0;
	     pFe1->FrClipXEnd = 0;
	  }
	/* Faut-il initialiser la zone de reaffichage? */
	else if (pFe1->FrClipXBegin == pFe1->FrClipXEnd && pFe1->FrClipXBegin == 0)
	  {
	     pFe1->FrClipXBegin = xd;
	     pFe1->FrClipXEnd = xf;
	  }
	/* On met a jour la zone de reaffichage */
	else
	  {
	     if (pFe1->FrClipXBegin > xd)
		pFe1->FrClipXBegin = xd;
	     if (pFe1->FrClipXEnd < xf)
		pFe1->FrClipXEnd = xf;
	  }
	/* Faut-il prendre toute la hauteur de la fenetre? */
	if (yd == yf && yd == -1)
	  {
	     DimFenetre (frame, &large, &haut);
	     pFe1->FrClipYBegin = pFe1->FrYOrg;
	     pFe1->FrClipYEnd = haut + pFe1->FrYOrg;
	     /* On termine un reaffichage */
	  }
	else if (yd == yf && yd == 0)
	  {
	     pFe1->FrClipYBegin = 0;
	     pFe1->FrClipYEnd = 0;
	  }
	/* Faut-il initialiser la zone de reaffichage? */
	else if (pFe1->FrClipYBegin == pFe1->FrClipYEnd && pFe1->FrClipYBegin == 0)
	  {
	     pFe1->FrClipYBegin = yd;
	     pFe1->FrClipYEnd = yf;
	  }
	/* On met a jour la zone de reaffichage */
	else
	  {
	     if (pFe1->FrClipYBegin > yd)
		pFe1->FrClipYBegin = yd;
	     if (pFe1->FrClipYEnd < yf)
		pFe1->FrClipYEnd = yf;
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
   ViewFrame            *pFe1;


   pFe1 = &FntrTable[frame - 1];
   DefClip (frame, xd + pFe1->FrXOrg, yd + pFe1->FrYOrg, xf + pFe1->FrXOrg, yf + pFe1->FrYOrg);
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
   PtrBox            ibox;
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
   ViewFrame            *pFe1;
   PtrBox            pBo1;
   PtrBox            pBo2;

   /* A priori on n'ajoute pas de nouveaux paves */
   ajoute = False;
   pFe1 = &FntrTable[frame - 1];
   if (pFe1->FrReady && pFe1->FrAbstractBox != NULL
       && pFe1->FrClipXBegin < pFe1->FrClipXEnd
       && pFe1->FrClipYBegin < pFe1->FrClipYEnd)
     {
	pFe1->FrYOrg -= delta;
	framexmin = pFe1->FrClipXBegin;
	framexmax = pFe1->FrClipXEnd;
	frameymin = pFe1->FrClipYBegin;
	frameymax = pFe1->FrClipYEnd;
	SetClip (frame, pFe1->FrXOrg, pFe1->FrYOrg, &framexmin, &frameymin, &framexmax, &frameymax, 1);
	DimFenetre (frame, &l, &h);
	haut = pFe1->FrYOrg;
	bas = haut + h;
	/* S'il y a quelque chose a reafficher ? */
	if (framexmin < framexmax && frameymin < frameymax)
	  {
	     /* Recherche la 1ere boite au dessus du bas de la fenetre */
	     /* La recherche commence par la derniere boite affichee */
	     ibox = pFe1->FrAbstractBox->AbBox->BxPrevious;
	     if (ibox == NULL)
		/* Document vide */
		ibox = pFe1->FrAbstractBox->AbBox;
	     audessous = True;
	     min = NULL;
	     max = NULL;
	     vol = 0;
	     /* 1/2 frame au dessous du bas de la fenetre */
	     x = bas + h / 2;
	     while (audessous)
	       {
		  pBo1 = ibox;
		  if (pBo1->BxYOrg < bas)
		     audessous = False;
		  else
		    {
		       if (pBo1->BxYOrg > x)
			  if (pBo1->BxType != BoPiece && pBo1->BxType != BoDotted)
			     /* Ce n'est pas une boite de coupure */
			     vol += pBo1->BxAbstractBox->AbVolume;
			  else if (pBo1->BxAbstractBox->AbBox->BxNexChild == ibox)
			     /* C'est la 1ere boite de coupure */
			     vol += pBo1->BxAbstractBox->AbVolume;
		       if (pBo1->BxPrevious == NULL)
			  audessous = False;
		       ibox = pBo1->BxPrevious;
		    }
	       }

	     /* On affiche les plans dans l'orde n a 0 */
	     plan = 65536;
	     plansuiv = plan - 1;
	     prembox = ibox;
	     while (plan != plansuiv)
		/* Il y a un nouveau plan a afficher */
	       {
		  plan = plansuiv;
		  ibox = prembox;
		  /* On reparcourt toutes les boites */

		  /* Affiche toutes les boites non encore entierement affichees */
		  while (ibox != NULL)
		    {
		       pBo1 = ibox;
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
				    max = ibox;
				 min = ibox;
			      }
			    if (y >= frameymin
				&& pBo1->BxYOrg <= frameymax
				&& x >= framexmin
				&& pBo1->BxXOrg <= framexmax)
			       AfBoite (ibox, frame);
			    /* On passe a la boite suivante */
			    ibox = pBo1->BxPrevious;
			 }
		       else if (pBo1->BxAbstractBox->AbDepth < plan)
			  /* On retient la plus grande valeur de plan inferieur */
			 {
			    if (plan == plansuiv)
			       plansuiv = pBo1->BxAbstractBox->AbDepth;
			    else if (pBo1->BxAbstractBox->AbDepth > plansuiv)
			       plansuiv = pBo1->BxAbstractBox->AbDepth;
			    ibox = pBo1->BxPrevious;
			 }
		       else
			  ibox = pBo1->BxPrevious;
		    }
	       }

	     /* La zone modifiee est affichee */
	     DefClip (frame, 0, 0, 0, 0);
	     ResetClip (frame);

	     /* On complete eventuellement l'image partielle */
	     ibox = pFe1->FrAbstractBox->AbBox;
	     if (!Complete && !Insert)
	       {
		  pBo1 = ibox;
		  Complete = True;
		  /* On est en train de completer l'image */
		  y = haut - pBo1->BxYOrg;
		  x = h / 2;

		  /* L'image depasse d'une 1/2 frame en haut et en bas */
		  if (vol > 0 && y > x)
		     /* On calcule le volume a retirer */
		    {
		       ibox = pBo1->BxNext;
		       haut -= x;
		       y = 0;
		       while (ibox != NULL)
			 {
			    pBo2 = ibox;
			    if (pBo2->BxYOrg + pBo2->BxHeight > haut)
			       ibox = NULL;
			    else
			      {
				 ibox = pBo2->BxNext;
				 if (pBo2->BxType != BoPiece && pBo2->BxType != BoDotted)
				    y += pBo2->BxAbstractBox->AbVolume;
				 else if (pBo2->BxNexChild == NULL)
				    y += pBo2->BxAbstractBox->AbVolume;
				 /* else while */
			      }
			 }
		       pFe1->FrVolume = pFe1->FrAbstractBox->AbVolume - vol - y;
		    }

		  /* Il manque un morceau d'image concrete en haut de la fenetre */
		  else if (pFe1->FrAbstractBox->AbTruncatedHead && y < 0)
		    {
		       /* On libere des paves en bas */
		       if (vol > 0 && vol < pFe1->FrAbstractBox->AbVolume)
			 {
			    VolReduit (False, vol, frame);
			    DefClip (frame, 0, 0, 0, 0);
			    /* On complete en haut -> On decale toute l'image concrete */
			 }

		       /* Volume a ajouter */
		       if (pFe1->FrAbstractBox == NULL)
			 {
			    printf ("ERR: plus de pave dans %d\n", frame);
			    vol = -pFe1->FrVolume;
			 }
		       else
			  vol = pFe1->FrVolume - pFe1->FrAbstractBox->AbVolume;

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
		       VolAugmente (True, vol, frame);
		       /* On a ajoute des paves */
		       ajoute = True;
		       /* Il faut repositionner la fenetre dans l'image concrete */
		       if (min != NULL)
			 {
			    pBo2 = min;
			    y = -y + pBo2->BxYOrg;
			    /* y=deplacement de l'ancienne 1ere boite */
			    /* Ce qui est deja affiche depend de l'ancienne 1ere boite */
			    pFe1->FrYOrg += y;
			    /* x = limite du reaffichage apres decalage */
			    if (y > 0)
			       x = pBo2->BxYOrg + pBo2->BxHeight;
			    /* Nouvelle position limite */
			    pFe1->FrClipYEnd = x;
			 }
		       else
			  /* Il n'y a pas d'ancienne boite : */
			  /* la fenetre est cadree en haut de l'image */
			  pFe1->FrYOrg = 0;

		       /* On a fini de completer l'image */
		       Complete = False;
		       audessous = AfDebFenetre (frame, 0);
		    }

		  /* Il manque un morceau d'image concrete en bas de la fenetre */
		  else if (pFe1->FrAbstractBox->AbTruncatedTail && bas > pBo1->BxYOrg + pBo1->BxHeight)
		    {
		       y = pBo1->BxYOrg + pBo1->BxHeight;
		       /* Volume de la surface manquante */
		       vol = (bas - y) * l;
		       /* Evaluation en caracteres */
		       vol = VolumCar (vol);

		       /* On nettoie le bas de la fenetre */
		       Clear (frame, l, pFe1->FrYOrg + h - y, 0, y);
		       /* Il faut eviter de boucler quand le volume n'est pas modifie */
		       y = pFe1->FrAbstractBox->AbVolume;
		       VolAugmente (False, vol, frame);
		       y -= pFe1->FrAbstractBox->AbVolume;
		       /* On a fini de completer l'image */
		       Complete = False;
		       if (y == 0)
			  printf ("ERR: Il n'y a rien a ajouter\n");
		       else
			  /* On n'a peut-etre pas fini de completer l'image */
			  audessous = AfFinFenetre (frame, 0);
		    }

		  /* Le volume est satisfaisant */
		  else
		     pFe1->FrVolume = pFe1->FrAbstractBox->AbVolume;
		  /* On a fini de completer l'image */
		  Complete = False;
	       }
	  }
	else
	   /* La zone modifiee n'est pas visible */
	   DefClip (frame, 0, 0, 0, 0);

     }
   else if (pFe1->FrReady)
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
/* |            boite ibox elle-meme.                                   | */
/* |            Une boite ne peut etre presente qu'une seule fois dans  | */
/* |            adbloc.                                                 | */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
void                AjouteACreer (PtrBox * acreer, PtrBox ibox, int frame)

#else  /* __STDC__ */
void                AjouteACreer (acreer, ibox, frame)
PtrBox           *acreer;
PtrBox            ibox;
int                 frame;

#endif /* __STDC__ */

{
   PtrAbstractBox             pPa1;
   int                 i;

   /* Recherche si un pave pere a l'indicateur PavGraphCreation */
   pPa1 = ibox->BxAbstractBox->AbEnclosing;
   i = 0;
   while (i == 0)
      if (pPa1 == NULL)
	 i = 1;
      else if (pPa1->AbBox->BxHorizFlex
	       || pPa1->AbBox->BxVertFlex)
	{
	   i = 1;
	   pPa1 = NULL;
	}
      else if ((pPa1->AbHorizPos.PosUserSpecified)
	       || (pPa1->AbVertPos.PosUserSpecified)
	       || (pPa1->AbWidth.DimUserSpecified)
	       || (pPa1->AbHeight.DimUserSpecified))
	 i = 1;
      else
	 pPa1 = pPa1->AbEnclosing;

   /* Il existe un pave englobant avec l'indicateur -> meme traitement */
   if (pPa1 != NULL)
      AjouteACreer (acreer, pPa1->AbBox, frame);

   /* Enregistrement de la boite a creer s'il n'y en a pas une  */
   /* englobante enregistree et si la boite a creer est visible */
   if (*acreer == NULL
       && ibox->BxAbstractBox->AbVisibility >= FntrTable[frame - 1].FrVisibility)
      *acreer = ibox;
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
   PtrBox            ibox;
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
   ViewFrame            *pFe1;
   PtrBox            pBo1;
   PtrAbstractBox             pPa1;
   PtrBox            pBo2;
   PtrBox            ACreer;
   int                 i;

   /* A priori on n'ajoute pas de nouveaux paves */
   ajoute = False;
   /* A priori on n'a pas besoin de creer interactivement des boites */
   ACreer = NULL;

   pFe1 = &FntrTable[frame - 1];
   if (pFe1->FrReady && pFe1->FrAbstractBox != NULL
       && pFe1->FrClipXBegin < pFe1->FrClipXEnd
       && pFe1->FrClipYBegin < pFe1->FrClipYEnd)
     {
	pFe1->FrYOrg += delta;
	framexmin = pFe1->FrClipXBegin;
	framexmax = pFe1->FrClipXEnd;
	frameymin = pFe1->FrClipYBegin;
	frameymax = pFe1->FrClipYEnd;
	SetClip (frame, pFe1->FrXOrg, pFe1->FrYOrg, &framexmin, &frameymin, &framexmax, &frameymax, 1);
	DimFenetre (frame, &l, &h);
	haut = pFe1->FrYOrg;
	bas = haut + h;

	/* On recherche la premiere boite visible ou en dessous */
	ibox = pFe1->FrAbstractBox->AbBox->BxNext;
	if (ibox == NULL)
	   ibox = pFe1->FrAbstractBox->AbBox;	/* Document vide */

	audessus = True;
	min = NULL;
	/*max = NULL; */
	vol = 0;
	x = haut - h / 2;

	/* On affiche les plans dans l'orde n a 0 */
	plan = 65536;
	plansuiv = plan - 1;
	prembox = ibox;
	while (plan != plansuiv)
	   /* Il y a un nouveau plan a afficher */
	  {
	     plan = plansuiv;
	     ibox = prembox;
	     /* On reparcourt toute les boites */

	     /* On affiche toutes les boites non encore entierement affichees */
	     while (ibox != NULL)
	       {

		  /* Faut-il comptabiliser ce volume comme volume au dessus */
		  if (audessus)
/**485*/ if (ibox->BxYOrg + ibox->BxHeight < haut - h / 4)
		       {
/**HYP*/ if ((ibox->BxType != BoPiece && ibox->BxType != BoDotted)
	     || ibox->BxNexChild == NULL)
			     vol += ibox->BxAbstractBox->AbVolume;
		       }
		     else
			audessus = False;

		  if (ibox->BxAbstractBox != NULL && ibox->BxAbstractBox->AbDepth == plan)
		    {
		       /* La boite est affichee dans le plan courant */
		       y = ibox->BxYOrg + ibox->BxHeight;
		       /* Il faut tenir compte du remplissage de fin de bloc */
		       x = ibox->BxXOrg + ibox->BxWidth + ibox->BxEndOfBloc;

		       /* On note la premiere et derniere boite visible */
		       if (y > haut && ibox->BxYOrg < bas)
			 {
			    if (min == NULL)
			       min = ibox;
			    /*max = ibox; */
			 }	/*if */

		       /* Si la boite est affichee pour la premiere fois, */
		       /* on regarde si elle ou une de ces boites parentes */
		       /* doit etre creee interactivement ? */
		       if (ibox->BxNew)
			 {
			    /* La boite n'est plus nouvelle */
			    ibox->BxNew = 0;
			    pPa1 = ibox->BxAbstractBox;
			    i = 0;
			    while (i == 0)
			       if (pPa1 == NULL)
				  i = 1;
			       else if (pPa1->AbWidth.DimIsPosition
					|| pPa1->AbHeight.DimIsPosition)
				 {
				    i = 1;
				    pPa1 = NULL;
				 }
			       else if ((pPa1->AbHorizPos.PosUserSpecified)
					|| (pPa1->AbVertPos.PosUserSpecified)
					|| (pPa1->AbWidth.DimUserSpecified)
					|| (pPa1->AbHeight.DimUserSpecified))
				  i = 1;
			       else
				  pPa1 = pPa1->AbEnclosing;
			 }
		       else
			  pPa1 = NULL;

/*** Saute la boite a creer interactivement ***/
		       if (pPa1 != NULL)
			  /* Enregistre la boite a creer */
			  AjouteACreer (&ACreer, pPa1->AbBox, frame);
		       else if (y >= frameymin
				&& ibox->BxYOrg <= frameymax
				&& x >= framexmin
				&& ibox->BxXOrg <= framexmax)
			  AfBoite (ibox, frame);
		       /* On passe a la boite suivante */
		       ibox = ibox->BxNext;
		    }
		  else if (ibox->BxAbstractBox != NULL && ibox->BxAbstractBox->AbDepth < plan)
		    {
		       /* On retient la plus grande valeur de plan inferieur */
		       if (plan == plansuiv)
			  plansuiv = ibox->BxAbstractBox->AbDepth;
		       else if (ibox->BxAbstractBox->AbDepth > plansuiv)
			  plansuiv = ibox->BxAbstractBox->AbDepth;
		       ibox = ibox->BxNext;
		    }
		  else
		     ibox = ibox->BxNext;
	       }
	  }

	/* Creation interactive des boites laissees dans le plan */
	if (ACreer != NULL)
	  {
	     ModeCreation (ACreer, frame);
	     /* Faut-il aussi traiter les boites filles ? */
	     pPa1 = ACreer->BxAbstractBox;
	     if (pPa1 != NULL)
		pPa1 = pPa1->AbFirstEnclosed;
	     while (pPa1 != NULL)
	       {
		  if ((pPa1->AbHorizPos.PosUserSpecified)
		      || (pPa1->AbVertPos.PosUserSpecified)
		      || (pPa1->AbWidth.DimUserSpecified)
		      || (pPa1->AbHeight.DimUserSpecified))
		    {
		       ACreer = pPa1->AbBox;
		       ModeCreation (ACreer, frame);
		    }
		  else
		     pPa1 = pPa1->AbNext;
	       }
	     return (False);
	  }

	/* La zone modifiee est affichee */
	DefClip (frame, 0, 0, 0, 0);
	ResetClip (frame);

	/* On complete eventuellement l'image partielle */
	ibox = pFe1->FrAbstractBox->AbBox;
	if (!Complete && (!Insert || delta > 0))
	  {
	     pBo1 = ibox;
	     Complete = True;
	     /* On est en train de completer l'image */
	     y = haut - pBo1->BxYOrg;

	     if (pFe1->FrAbstractBox->AbInLine)
		Complete = False;

	     /* Il manque un morceau d'image concrete en haut de la fenetre */
	     else if (pFe1->FrAbstractBox->AbTruncatedHead && y < 0)
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
		  VolAugmente (True, VolumCar (haut), frame);
		  ajoute = True;
		  /* On ajoute des paves en tete */
		  /* Il faut repositionner la fenetre dans l'image concrete */
		  if (min != NULL)
		    {
		       pBo2 = min;
		       y = -y + pBo2->BxYOrg;
		       /* y=deplacement de l'ancienne 1ere boite */
		       /* Ce qui est deja affiche depend de l'ancienne 1ere boite */
		       pFe1->FrYOrg += y;
		       /* x = limite du reaffichage apres decalage */
		       if (y > 0)
			  x = pBo2->BxYOrg + pBo2->BxHeight;
		       /* Nouvelle position limite */
		       pFe1->FrClipYEnd = x;
		    }
		  audessus = AfDebFenetre (frame, 0);
	       }
	     y = pFe1->FrYOrg + h - pBo1->BxYOrg - pBo1->BxHeight;
	     x = h / 2;

	     /* L'image depasse d'une 1/2 frame en haut et en bas */
	     if (vol > 0 && -y > x)
		/* On calcule le volume a retirer */
	       {
		  ibox = pBo1->BxPrevious;
		  bas += x;
		  y = 0;
		  while (ibox != NULL)
		    {
		       pBo2 = ibox;
		       if (pBo2->BxYOrg < bas)
			  ibox = NULL;
		       else
			 {
			    ibox = pBo2->BxPrevious;
			    if (pBo2->BxType != BoPiece && pBo2->BxType != BoDotted)
			       y += pBo2->BxAbstractBox->AbVolume;
			    else if (pBo2->BxAbstractBox->AbBox->BxNexChild == ibox)
			       y += pBo2->BxAbstractBox->AbVolume;
			 }
		    }
		  pFe1->FrVolume = pFe1->FrAbstractBox->AbVolume - vol - y;
	       }

	     /* Il manque un morceau d'image concrete en bas de la fenetre */
	     else if (pFe1->FrAbstractBox->AbTruncatedTail && y > 0)
		/* On libere des paves en haut */
	       {
		  if (vol > 0 && vol < pFe1->FrAbstractBox->AbVolume)
		    {
		       if (min != NULL)
			  y = min->BxYOrg;
		       VolReduit (True, vol, frame);
		       DefClip (frame, 0, 0, 0, 0);
		       /* Il faut repositionner la fenetre dans l'image concrete */
		       if (min != NULL)
			  pFe1->FrYOrg = pFe1->FrYOrg - y + min->BxYOrg;
		       /* On complete en bas */
		    }
		  if (pFe1->FrAbstractBox == NULL)
		    {
		       printf ("ERR: Plus de pave dans %d\n", frame);
		       vol = -pFe1->FrVolume;
		    }
		  else
		     vol = pFe1->FrVolume - pFe1->FrAbstractBox->AbVolume;

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
		  Clear (frame, l, pFe1->FrYOrg + h - y, 0, y);
		  /* Il faut eviter de boucler quand le volume n'est pas modifie */
		  y = pFe1->FrAbstractBox->AbVolume;
		  VolAugmente (False, vol, frame);
		  y -= pFe1->FrAbstractBox->AbVolume;
		  /* On a fini de completer l'image */
		  Complete = False;
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
		  pFe1->FrVolume = pFe1->FrAbstractBox->AbVolume;
	       }

	     /* On a fini de completer l'image */
	     Complete = False;
	  }


     }
   else if (pFe1->FrReady)
     {
	/* Il n'y avait rien a afficher */
	DefClip (frame, 0, 0, 0, 0);
	ResetClip (frame);
     }
   FirstCreation = False;

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
   ViewFrame            *pFe1;
   int                 l, h;

   /* Est-ce que la vue existe et n'est pas en cours d'evaluation? */
   pFe1 = &FntrTable[frame - 1];
   if (pFe1->FrAbstractBox != NULL)
     {
	/* Affichage du rectangle mis a jour */
	AfFinFenetre (frame, 0);
	/* Mise en evidence des boites en erreur */
	if (Erreur)
	  {
	     MajPavSelect (frame, pFe1->FrAbstractBox, True);
	     if (HardMsgAff)
		/* Attend un moment avant d'annuler les mises en evidences */
	       {
		  TtaDisplaySimpleMessage (LIB, INFO, LIB_CONFIRM);
	       }
	     MajPavSelect (frame, pFe1->FrAbstractBox, False);
	     Erreur = False;
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
