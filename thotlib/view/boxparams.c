
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*
   option.c : traitement des options du Mediateur.
   Major Changes:
   I. Vatton    Octobre 86
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"

#define EXPORT extern
#include "img.var"

#include "buildboxes_f.h"
#include "textcommands_f.h"
#include "font_f.h"
#include "boxselection_f.h"
#include "boxparams_f.h"

#ifdef __STDC__
extern void         DefClip (int, int, int, int, int);
extern void         EndInsert (void);
extern void         DisplayFrame (int);

#else
extern void         DefClip ();
extern void         EndInsert ();
extern void         DisplayFrame ();

#endif

/* ---------------------------------------------------------------------- */
/* |    InitVisu initialise le seuil de visibilite et le facteur de     | */
/* |            zoom de la fenetre frame.                                       | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                InitVisu (int frame, int Visibilite, int Zoom)

#else  /* __STDC__ */
void                InitVisu (frame, Visibilite, Zoom)
int                 frame;
int                 Visibilite;
int                 Zoom;

#endif /* __STDC__ */

{
   ViewFrame            *pFrame;

   if (frame > 0 && frame <= MAX_FRAME)
     {
	pFrame = &FntrTable[frame - 1];
	pFrame->FrVisibility = Visibilite;
	pFrame->FrMagnification = Zoom;
     }
}


/* ---------------------------------------------------------------------- */
/* |    GetVisu retourne les valeurs courantes du seuil de visibilite et| */
/* |            du facteur de zoom de la fenetre.                       | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                GetVisu (int frame, int *Visibilite, int *Zoom)

#else  /* __STDC__ */
void                GetVisu (frame, Visibilite, Zoom)
int                 frame;
int                *Visibilite;
int                *Zoom;

#endif /* __STDC__ */

{
   ViewFrame            *pFrame;

   if (frame > 0 && frame <= MAX_FRAME)
     {
	pFrame = &FntrTable[frame - 1];
	*Visibilite = pFrame->FrVisibility;
	*Zoom = pFrame->FrMagnification;
     }
}


/* ---------------------------------------------------------------------- */
/* |    ModVisu reevalue la vue designee apres decalage des tailles     | */
/* |            ou/et modification du seuil de visibilite des boites.   | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                ModVisu (int frame, int Visibilite, int Zoom)

#else  /* __STDC__ */
void                ModVisu (frame, Visibilite, Zoom)
int                 frame;
int                 Visibilite;
int                 Zoom;

#endif /* __STDC__ */

{
   int                 c1;
   int                 cN;
   int                 h;
   PtrAbstractBox             pAb;
   PtrAbstractBox             pv1;
   PtrAbstractBox             pvN;
   int                 x, y;
   boolean             unique;

   /* boolean         retour; */
   ViewFrame            *pFrame;
   ViewSelection            *pViewSel;

   EndInsert ();
   c1 = 0;
   cN = 0;
   /* On enregistre le seuil de visibilite et facteur de zoom de la fenetre */
   pFrame = &FntrTable[frame - 1];
   pFrame->FrVisibility = Visibilite;
   pFrame->FrMagnification = Zoom;
   EvalAffich = FALSE;
   if (pFrame->FrAbstractBox != NULL)
      /* On sauvegarde la selection courante dans la fenetre */
     {
	pViewSel = &pFrame->FrSelectionBegin;
	if (pViewSel->VsBox != NULL)
	  {
	     pv1 = pViewSel->VsBox->BxAbstractBox;
	     if (pViewSel->VsIndBuf == 0)
		c1 = 0;
	     else
		c1 = pViewSel->VsBox->BxIndChar + pViewSel->VsIndBox + 1;
	     /* On annule le debut de selection */
	     pViewSel->VsBox = NULL;
	  }
	else
	   pv1 = NULL;
	pViewSel = &pFrame->FrSelectionEnd;
	if (pViewSel->VsBox != NULL)
	  {
	     pvN = pViewSel->VsBox->BxAbstractBox;
	     if (pViewSel->VsIndBuf == 0)
		cN = 0;
	     else
		cN = pViewSel->VsBox->BxIndChar + pViewSel->VsIndBox + 1;
	     /* On annule la fin de selection */
	     pViewSel->VsBox = NULL;
	  }
	else
	   pvN = NULL;
	unique = pFrame->FrSelectOneBox;
	/* On libere de la hierarchie avant recreation */
	pAb = pFrame->FrAbstractBox;
	/* On sauvegarde la position de la fenetre dans le document */
	x = pFrame->FrXOrg;
	y = pFrame->FrYOrg;
	RemoveBoxes (pAb, TRUE, frame);
	ThotFreeFont (frame);
	/* On libere les polices de caracteres utilisees */
	pFrame->FrAbstractBox = NULL;
	/* Recreation de la vue */
	h = 0;
	(void) ChangeConcreteImage (frame, &h, pAb);
	/* On restaure la position de la fenetre dans le document */
	pFrame->FrXOrg = x;
	pFrame->FrYOrg = y;
	DefClip (frame, -1, -1, -1, -1);
	DisplayFrame (frame);
	/* On restaure la selection courante dans la fenetre */
	if (unique)
	   PoseSelect (frame, pv1, c1, cN, TRUE, TRUE, unique);
	/* La selection porte sur plusieurs paves */
	else
	  {
	     if (pv1 != NULL)
		PoseSelect (frame, pv1, c1, 0, TRUE, FALSE, FALSE);
	     if (pvN != NULL)
		PoseSelect (frame, pvN, 0, cN, FALSE, TRUE, FALSE);
	     /* On visualise la selection que l'on vient de poser */
	  }
	SetSelect (frame, TRUE);
     }
   EvalAffich = TRUE;
}
/* End Of Module option */
