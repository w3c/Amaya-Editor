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
#include "constmenu.h"
#include "constmedia.h"
#include "typemedia.h"
#include "app.h"
#include "appdialogue.h"
#include "tree.h"
#include "message.h"
#include "dialog.h"

#define MAX_ARGS 20

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "frame_tv.h"
#include "appdialogue_tv.h"

static int          Fenvisibilityview;

#include "appdialogue_f.h"
#include "actions_f.h"
#include "boxparams_f.h"

#ifdef __STDC__
extern int          GetWindowNumber (Document, View);

#else  /* __STDC__ */
extern int          GetWindowNumber ();

#endif /* __STDC__ */

/*----------------------------------------------------------------------
   changevisibilityview met a jour le formulaire de visibilite.               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                changevisibilityview (int ref, int typedata, char *data)

#else  /* __STDC__ */
void                changevisibilityview (ref, typedata, data)
int                 ref;
int                 typedata;
char               *data;

#endif /* __STDC__ */
{
   int                 valvisib;
   int                 valzoom;
   int                 bouton;
   char                chaine[100];

   bouton = (int) data;
   GetFrameParams (Fenvisibilityview, &valvisib, &valzoom);
   if (bouton == 0)
      /* Abandon du formulaire */
      Fenvisibilityview = 0;
   else if (bouton == 1 && valvisib > 0)
     {
	valvisib--;
	sprintf (chaine, "%s : %d", TtaGetMessage (LIB, TMSG_CHANGE_FILTER), 10 - valvisib);
	TtaNewLabel (NumTextVisibility, NumMenuVisibility, chaine);
	SetFrameParams (Fenvisibilityview, valvisib, valzoom);
     }
   else if (bouton == 2 && valvisib < 10)
     {
	valvisib++;
	sprintf (chaine, "%s : %d", TtaGetMessage (LIB, TMSG_CHANGE_FILTER), 10 - valvisib);
	TtaNewLabel (NumTextVisibility, NumMenuVisibility, chaine);
	SetFrameParams (Fenvisibilityview, valvisib, valzoom);
     }
   else if (bouton == 4)
     {
	sprintf (chaine, "%s : %d", TtaGetMessage (LIB, TMSG_CHANGE_FILTER), 10 - valvisib);
	TtaNewLabel (NumTextVisibility, NumMenuVisibility, chaine);
     }
   else
     {
	sprintf (chaine, "%s : %d", TtaGetMessage (LIB, TMSG_VALUE_NOT_CHANGED), 10 - valvisib);
	TtaNewLabel (NumTextVisibility, NumMenuVisibility, chaine);
     }

}


/*----------------------------------------------------------------------
   reTtcSetVisibilityView desactive le formulaire de visibilite.              
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                reTtcSetVisibilityView (int frame)

#else  /* __STDC__ */
void                reTtcSetVisibilityView (frame)
int                 frame;

#endif /* __STDC__ */
{
   if (Fenvisibilityview != 0 && Fenvisibilityview == frame)
     {
	/* Annule le formulaire de changement de visibilite */
	TtaDestroyDialogue (NumMenuVisibility);
	Fenvisibilityview = 0;
     }
}


/*----------------------------------------------------------------------
   TtcSetVisibilityView initialise le changement de visibilite.               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcSetVisibilityView (Document document, View view)

#else  /* __STDC__ */
void                TtcSetVisibilityView (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   int                 i;
   char                chaine[100];

   /* Faut-il creer le formulaire visibilite */
   if (ThotLocalActions[T_chvisibility] == NULL)
     {
	/* Connecte les actions locales liees au traitement de la visibilite */
	TteConnectAction (T_chvisibility, (Proc) changevisibilityview);
	TteConnectAction (T_rsvisibility, (Proc) reTtcSetVisibilityView);
     }
   /* Creation du formulaire */


   strcpy (chaine, TtaGetMessage (LIB, TMSG_INCREASE));
   i = strlen (chaine) + 1;
   strcpy (&chaine[i], TtaGetMessage (LIB, TMSG_DECREASE));
   TtaNewSheet (NumMenuVisibility,  0, TtaGetMessage (LIB, TMSG_VISIB),
		2, chaine, FALSE, 4, 'L', D_DONE);


   /* Affiche le nom du document concerne */
   sprintf (chaine, "%s %s", TtaGetDocumentName (document),
	    TtaGetViewName (document, view));
   Fenvisibilityview = GetWindowNumber (document, view);
   TtaNewLabel (NumDocVisibility, NumMenuVisibility, chaine);
   /* Initialisation du reste du formulaire */
   changevisibilityview (NumMenuVisibility, INTEGER_DATA, (char *) 4);
   TtaShowDialogue (NumMenuVisibility, TRUE);
}
