
/* 
   des.c : gestion des designations de boites.
   I. Vatton - Mars 85
   IV : Juin 93 polylines
 */

#include "libmsg.h"
#include "thot_sys.h"
#include "functions.h"
#include "constmedia.h"
#include "typemedia.h"
#include "message.h"
#include "appdialogue.h"

#define EXPORT extern
#include "img.var"
#include "frame.var"
#include "environ.var"
#include "appdialogue.var"

#include "appli_f.h"
#include "structcreation_f.h"
#include "boxmoves_f.h"
#include "boxlocate_f.h"
#include "views_f.h"
#include "callback_f.h"
#include "font_f.h"
#include "geom_f.h"
#include "absboxes_f.h"
#include "buildboxes_f.h"
#include "buildlines_f.h"
#include "changepresent_f.h"
#include "boxselection_f.h"

#ifdef WWW_MSWINDOWS		/* map to MSVC library system calls */
#include <math.h>
#endif /* WWW_MSWINDOWS */

#define YFACTOR 200		/* penalisation en Y */
#define ASIZE 3			/* taille des ancres */
#define MAXLINE 500
#define MAXVERTS 100

#ifdef __STDC__
extern PtrBox     DansLaBoite (PtrAbstractBox, int, int, int, int *);
extern int          DistGraphique (int, int, PtrBox, int);
extern int          DistBox (int, int, int, int, int, int);

#else
extern PtrBox     DansLaBoite ();
extern int          DistGraphique ();
extern int          DistBox ();

#endif /* __STDC__ */

/* ---------------------------------------------------------------------- */
/* |    DesBoite recherche recursivement le pave qui englobe le point   | */
/* |            designe' par x,y.                                       | */
/* |            La fonction regarde toute l'arborescence des paves      | */
/* |            pour trouver le premier pave de plus petite profondeur  | */
/* |            couvrant le point designe.                              | */
/* |            Si un pave et son fils repondent a la condition, c'est  | */
/* |            le pave fils qui l'emporte.                             | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                DesBoite (PtrBox * result, PtrAbstractBox pAb, int frame, int x, int y, int *pointselect)
#else  /* __STDC__ */
void                DesBoite (result, pAb, frame, x, y, pointselect)
PtrBox           *result;
PtrAbstractBox             pAb;
int                 frame;
int                 x;
int                 y;
int                *pointselect;

#endif /* __STDC__ */
{
   PtrAbstractBox             pav;
   PtrBox            sbox, pBox;
   PtrBox            testbox;
   int                 distmax;
   int                 lepoint;
   ViewFrame            *pFrame;
   int                 d;

   pBox = NULL;
   sbox = NULL;
   distmax = 2000;		/* au-dela, on n'accepte pas la selection */
   pFrame = &FntrTable[frame - 1];

   if (pFrame->FrAbstractBox != NULL)
      pBox = pFrame->FrAbstractBox->AbBox;

   if (pBox != NULL)
     {
	pBox = pBox->BxNext;
	while (pBox != NULL)
	  {
	     pav = pBox->BxAbstractBox;
	     lepoint = 0;
	     if (pav->AbVisibility >= pFrame->FrVisibility)
	       {
		  if (pav->AbPresentationBox || pav->AbLeafType == LtGraphics || pav->AbLeafType == LtPlyLine)
		    {
		       testbox = DansLaBoite (pav, x, x, y, &lepoint);
		       if (testbox == NULL)
			  d = distmax + 1;
		       else
			  d = 0;
		    }
		  else if (pav->AbLeafType == LtSymbol && pav->AbShape == 'r')
		     /* glitch pour le symbole racine */
		     d = DistGraphique (x, y, pBox, 1);
		  else if (pav->AbLeafType == LtText
			   || pav->AbLeafType == LtSymbol
			   || pav->AbLeafType == LtPicture
		     /* ou une boite composee vide */
		   || (pav->AbLeafType == LtCompound && pav->AbVolume == 0))
		    {
		       if (pav->AbLeafType == LtPicture)
			 {
			    /* detecte si on selectionne la droite de l'image */
			    d = pBox->BxXOrg + (pBox->BxWidth / 2);
			    if (x > d)
			       lepoint = 1;
			    d = DistBox (x, y, pBox->BxXOrg, pBox->BxYOrg,
					 pBox->BxWidth, pBox->BxHeight);
			 }
		       else
			  d = DistBox (x, y, pBox->BxXOrg, pBox->BxYOrg,
				       pBox->BxWidth, pBox->BxHeight);
		    }
		  else
		     d = distmax + 1;

		  /* Prend l'element le plus proche */
		  if (d < distmax)
		    {
		       distmax = d;
		       sbox = pBox;
		       *pointselect = lepoint;	/* le point selectionne */
		    }
		  else if (d == distmax)
		    {
		       /* Si c'est la premiere boite trouvee */
		       if (sbox == NULL)
			 {
			    distmax = d;
			    sbox = pBox;
			    *pointselect = lepoint;	/* le point selectionne */
			 }
		       /* Si la boite est sur un plan au dessus de la precedente */
		       else if (sbox->BxAbstractBox->AbDepth > pBox->BxAbstractBox->AbDepth)
			 {
			    distmax = d;
			    sbox = pBox;
			    *pointselect = lepoint;	/* le point selectionne */
			 }
		    }
	       }
	     pBox = pBox->BxNext;
	  }
     }
   *result = sbox;
}
