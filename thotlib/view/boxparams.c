/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *  traitement des options du Mediateur.
 *
 * Author: I. Vatton (INRIA)
 *
 */
#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "appdialogue.h"

#define THOT_EXPORT extern
#include "boxes_tv.h"
#include "edit_tv.h"
#include "frame_tv.h"
#include "appdialogue_tv.h"

#include "applicationapi_f.h"
#include "boxparams_f.h"
#include "boxselection_f.h"
#include "buildboxes_f.h"
#include "editcommands_f.h"
#include "font_f.h"
#include "frame_f.h"
#include "textcommands_f.h"


/*----------------------------------------------------------------------
  InitializeFrameParams initialise le seuil de visibilite et le facteur de     
  zoom de la fenetre frame.                                       
  ----------------------------------------------------------------------*/
void InitializeFrameParams (int frame, int visibility, int zoom)
{
  ViewFrame          *pFrame;

  if (frame > 0 && frame <= MAX_FRAME)
    {
      pFrame = &ViewFrameTable[frame - 1];
      pFrame->FrVisibility = visibility;
      pFrame->FrMagnification = zoom;
    }
}


/*----------------------------------------------------------------------
  GetFrameParams retourne les valeurs courantes du seuil de visibilite et
  du facteur de zoom de la fenetre.                       
  ----------------------------------------------------------------------*/
void GetFrameParams (int frame, int *visibility, int *zoom)
{
  ViewFrame          *pFrame;

  if (frame > 0 && frame <= MAX_FRAME)
    {
      pFrame = &ViewFrameTable[frame - 1];
      *visibility = pFrame->FrVisibility;
      *zoom = pFrame->FrMagnification;
    }
}


/*----------------------------------------------------------------------
  SetFrameParams rebuilds the whole concrete image after visibility or
  zoom change.
  ----------------------------------------------------------------------*/
void SetFrameParams (int frame, int visibility, int zoom)
{
  PtrAbstractBox      pAb, pv1, pvN;
  ViewFrame          *pFrame;
  ViewSelection      *pViewSel;
  Document            document;
  int                 x, y;
  int                 c1;
  int                 cN;
  int                 h;
  ThotBool            unique;

  CloseTextInsertion ();
  /* On enregistre le seuil de visibilite et facteur de zoom de la fenetre */
  pFrame = &ViewFrameTable[frame - 1];
  pFrame->FrVisibility = visibility;
  pFrame->FrMagnification = zoom;
  document = FrameTable[frame].FrDoc;
  if (document != 0 && documentDisplayMode[document - 1] != NoComputedDisplay)
    {
      ReadyToDisplay = FALSE;
      c1 = 0;
      cN = 0;
      if (pFrame->FrAbstractBox)
        /* On sauvegarde la selection courante dans la fenetre */
        {
          pViewSel = &pFrame->FrSelectionBegin;
          if (pViewSel->VsBox)
            {
              pv1 = pViewSel->VsBox->BxAbstractBox;
              if (pViewSel->VsIndBox == 0)
                c1 = 0;
              else
                c1 = pViewSel->VsBox->BxFirstChar + pViewSel->VsIndBox;
              /* On annule le debut de selection */
              pViewSel->VsBox = NULL;
            }
          else
            pv1 = NULL;
          pViewSel = &pFrame->FrSelectionEnd;
          if (pViewSel->VsBox != NULL)
            {
              pvN = pViewSel->VsBox->BxAbstractBox;
              if (pViewSel->VsIndBox == 0)
                cN = 0;
              else
                cN = pViewSel->VsBox->BxFirstChar + pViewSel->VsIndBox;
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
          pAb->AbDead = TRUE;
          RemoveBoxes (pAb, TRUE, frame);
          ThotFreeFont (frame);
          /* On libere les polices de caracteres utilisees */
          pFrame->FrAbstractBox = NULL;
          /* Recreation de la vue */
          pAb->AbDead = FALSE;
          pAb->AbNew = TRUE;
          h = 0;
          (void) ChangeConcreteImage (frame, &h, pAb);
          /* On restaure la position de la fenetre dans le document */
          pFrame->FrXOrg = x;
          pFrame->FrYOrg = y;
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
          DefClip (frame, -1, -1, -1, -1);
          DisplayFrame (frame);
        }
      ReadyToDisplay = TRUE;
    }
}
/* End Of Module option */
