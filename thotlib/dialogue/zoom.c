/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
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
 *
 * Author: I. Vatton (INRIA)
 *
 */

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "constmenu.h"
#include "appstruct.h"
#include "appdialogue.h"
#include "tree.h"
#include "message.h"
#include "dialog.h"

#define MAX_ARGS 20

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "frame_tv.h"
#include "appdialogue_tv.h"

static int          Fenzoomview;

#include "actions_f.h"
#include "appdialogue_f.h"
#include "applicationapi_f.h"
#include "boxparams_f.h"
#include "viewapi_f.h"


/*----------------------------------------------------------------------
   changezoomview met a jour le formulaire de zoom.                   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                changezoomview (int ref, int typedata, STRING data)

#else  /* __STDC__ */
void                changezoomview (ref, typedata, data)
int                 ref;
int                 typedata;
STRING               data;

#endif /* __STDC__ */
{
   int                 valvisib;
   int                 valzoom;
   int                 bouton;
   CHAR_T              chaine[100];

   bouton = (int) data;
   GetFrameParams (Fenzoomview, &valvisib, &valzoom);
   if (bouton == 0)
      /* Abandon du formulaire */
      Fenzoomview = 0;
   else if (bouton == 1 && valzoom < 10)
     {
	/* Augmente le zoom */
	valzoom++;
	usprintf (chaine, TEXT("%s : %d"), TtaGetMessage (LIB, TMSG_CHANGE_ZOOM), valzoom);
	TtaNewLabel (NumTextZoom, NumMenuZoom, chaine);
	SetFrameParams (Fenzoomview, valvisib, valzoom);
     }
   else if (bouton == 2 && valzoom > -10)
     {
	/* Diminue le zoom */
	valzoom--;
	usprintf (chaine, TEXT("%s : %d"), TtaGetMessage (LIB, TMSG_CHANGE_ZOOM), valzoom);
	TtaNewLabel (NumTextZoom, NumMenuZoom, chaine);
	SetFrameParams (Fenzoomview, valvisib, valzoom);
     }
   else if (bouton == 4)
     {
	usprintf (chaine, TEXT("%s : %d"), TtaGetMessage (LIB, TMSG_CHANGE_ZOOM), valzoom);
	TtaNewLabel (NumTextZoom, NumMenuZoom, chaine);
     }
   else
     {
	usprintf (chaine, TEXT("%s : %d"), TtaGetMessage (LIB, TMSG_VALUE_NOT_CHANGED), valzoom);
	TtaNewLabel (NumTextZoom, NumMenuZoom, chaine);
     }
}


/*----------------------------------------------------------------------
   reTtcSetZoomView desactive le formulaire de zoom.                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                reTtcSetZoomView (int frame)

#else  /* __STDC__ */
void                reTtcSetZoomView (frame)
int                 frame;

#endif /* __STDC__ */
{

   if (Fenzoomview != 0 && Fenzoomview == frame)
     {
	/* Annule le formulaire de zoom */
	TtaDestroyDialogue (NumMenuZoom);
	Fenzoomview = 0;
     }
}


/*----------------------------------------------------------------------
   TtcSetZoomView initialise le changement de Zoom.                   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcSetZoomView (Document document, View view)

#else  /* __STDC__ */
void                TtcSetZoomView (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   int                 i;
   CHAR_T                chaine[100];

   /* Faut-il creer le formulaire Zoom */
   if (ThotLocalActions[T_chzoom] == NULL)
     {
	/* Connecte les actions liees au traitement de la Zoom */
	TteConnectAction (T_chzoom, (Proc) changezoomview);
	TteConnectAction (T_rszoom, (Proc) reTtcSetZoomView);
     }

   /* Creation du formulaire */
   ustrcpy (chaine, TtaGetMessage (LIB, TMSG_INCREASE));
   i = ustrlen (chaine) + 1;
   ustrcpy (&chaine[i], TtaGetMessage (LIB, TMSG_DECREASE));
   TtaNewSheet (NumMenuZoom,  0, TtaGetMessage (LIB, TMSG_LIB_ZOOM),
		2, chaine, FALSE, 4, TEXT('L'), D_DONE);

   /* Affiche le nom du document concerne */
   usprintf (chaine, TEXT("%s %s"), TtaGetDocumentName (document),
	    TtaGetViewName (document, view));
   Fenzoomview = GetWindowNumber (document, view);
   TtaNewLabel (NumDocZoom, NumMenuZoom, chaine);
   /* Initialisation du formulaire affiche */
   changezoomview (NumMenuZoom, INTEGER_DATA, (STRING) 4);
   TtaShowDialogue (NumMenuZoom, TRUE);
}
