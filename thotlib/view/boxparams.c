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
 *  traitement des options du Mediateur.
 *
 * Author: I. Vatton (INRIA)
 *
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "appdialogue.h"

#define THOT_EXPORT extern
#include "boxes_tv.h"
#include "edit_tv.h"
#include "frame_tv.h"

#include "applicationapi_f.h"
#include "buildboxes_f.h"
#include "textcommands_f.h"
#include "font_f.h"
#include "frame_f.h"
#include "boxselection_f.h"
#include "boxparams_f.h"


/*----------------------------------------------------------------------
   InitializeFrameParams initialise le seuil de visibilite et le facteur de     
   zoom de la fenetre frame.                                       
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                InitializeFrameParams (int frame, int Visibilite, int Zoom)

#else  /* __STDC__ */
void                InitializeFrameParams (frame, Visibilite, Zoom)
int                 frame;
int                 Visibilite;
int                 Zoom;

#endif /* __STDC__ */

{
   ViewFrame          *pFrame;

   if (frame > 0 && frame <= MAX_FRAME)
     {
	pFrame = &ViewFrameTable[frame - 1];
	pFrame->FrVisibility = Visibilite;
	pFrame->FrMagnification = Zoom;
     }
}


/*----------------------------------------------------------------------
   GetFrameParams retourne les valeurs courantes du seuil de visibilite et
   du facteur de zoom de la fenetre.                       
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GetFrameParams (int frame, int *Visibilite, int *Zoom)
#else  /* __STDC__ */
void                GetFrameParams (frame, Visibilite, Zoom)
int                 frame;
int                *Visibilite;
int                *Zoom;
#endif /* __STDC__ */
{
   ViewFrame          *pFrame;

   if (frame > 0 && frame <= MAX_FRAME)
     {
	pFrame = &ViewFrameTable[frame - 1];
	*Visibilite = pFrame->FrVisibility;
	*Zoom = pFrame->FrMagnification;
     }
}


/*----------------------------------------------------------------------
   SetFrameParams reevalue la vue designee apres decalage des tailles     
   ou/et modification du seuil de visibilite des boites.   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetFrameParams (int frame, int Visibilite, int Zoom)
#else  /* __STDC__ */
void                SetFrameParams (frame, Visibilite, Zoom)
int                 frame;
int                 Visibilite;
int                 Zoom;
#endif /* __STDC__ */
{
   PtrAbstractBox      pAb;
   PtrAbstractBox      pv1;
   PtrAbstractBox      pvN;
   ViewFrame          *pFrame;
   ViewSelection      *pViewSel;
   Document            document;
   int                 x, y;
   int                 c1;
   int                 cN;
   int                 h;
   boolean             unique;

   CloseInsertion ();
   /* On enregistre le seuil de visibilite et facteur de zoom de la fenetre */
   pFrame = &ViewFrameTable[frame - 1];
   pFrame->FrVisibility = Visibilite;
   pFrame->FrMagnification = Zoom;
   document = FrameTable[frame].FrDoc;
   if (document != 0 && documentDisplayMode[document - 1] != NoComputedDisplay)
     {
       ReadyToDisplay = FALSE;
       c1 = 0;
       cN = 0;
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
	     InsertViewSelMarks (frame, pv1, c1, cN, TRUE, TRUE, unique);
	   /* La selection porte sur plusieurs paves */
	   else
	     {
	       if (pv1 != NULL)
		 InsertViewSelMarks (frame, pv1, c1, 0, TRUE, FALSE, FALSE);
	       if (pvN != NULL)
		 InsertViewSelMarks (frame, pvN, 0, cN, FALSE, TRUE, FALSE);
	       /* On visualise la selection que l'on vient de poser */
	     }
	   SwitchSelection (frame, TRUE);
	 }
       ReadyToDisplay = TRUE;
     }
}
/* End Of Module option */
