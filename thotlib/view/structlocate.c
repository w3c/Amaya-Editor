
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

#include "appli.f"
#include "creation.f"
#include "commun.f"
#include "dep.f"
#include "des.f"
#include "docvues.f"
#include "appexec.f"
#include "environ.f"
#include "font.f"
#include "geom.f"
#include "imabs.f"
#include "img.f"
#include "lig.f"
#include "modpres.f"
#include "sel.f"

#ifdef WWW_MSWINDOWS		/* map to MSVC library system calls */
#include <math.h>
#endif /* WWW_MSWINDOWS */

#define YFACTOR 200		/* penalisation en Y */
#define ASIZE 3			/* taille des ancres */
#define MAXLINE 500
#define MAXVERTS 100

#ifdef __STDC__
extern PtrBox     DansLaBoite (PtrAbstractBox, int, int, int, int *);

#else  /* __STDC__ */
extern PtrBox     DansLaBoite ();

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
void                StrDesBoite (PtrBox * result, PtrAbstractBox pAb, int frame, int x, int y, int *pointselect)
#else  /* __STDC__ */
void                StrDesBoite (result, pAb, frame, x, y, pointselect)
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

   /* On admet une erreur de precision de DELTA dans la designation */
   distmax = x - DELTA;
   pFrame = &FntrTable[frame - 1];
   sbox = NULL;
   pav = pAb;
   while (pav != NULL)
     {
	/* Est-ce le pave selectionne ? */
	if (pav->AbVisibility >= pFrame->FrVisibility)
	  {
	     pBox = DansLaBoite (pav, distmax, x, y, &lepoint);
	     if (pBox != NULL)
		/* Si c'est le premier pave trouve */
		if (sbox == NULL)
		  {
		     sbox = pBox;
		     *pointselect = lepoint;	/* le point selectionne */
		  }
	     /* Si le pave est sur un plan au dessus du precedent */
	     /* ou si le pave est un fils du precedent */
		else if (sbox->BxAbstractBox->AbDepth > pav->AbDepth
			 || (sbox->BxAbstractBox->AbLeafType == LtCompound && pBox->BxAbstractBox->AbLeafType != LtCompound)
			 || Parent (sbox, pBox))
		  {
		     sbox = pBox;
		     *pointselect = lepoint;
		  }
		else
		  {
		     /* Verifie que le point designe est strictement dans la boite */
		     pBox = DansLaBoite (pav, x, x, y, &lepoint);
		     testbox = DansLaBoite (sbox->BxAbstractBox, x, x, y, &lepoint);
		     if (testbox == NULL && pBox != NULL)
		       {
			  sbox = pBox;
			  *pointselect = lepoint;	/* le point selectionne */
		       }
		  }
	  }
	/* On teste un autre pave de l'arbre */
	pav = Pave_Suivant (pav);
     }
   *result = sbox;
}
