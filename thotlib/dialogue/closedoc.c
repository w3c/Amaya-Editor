
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */
/* I. Vatton    Mai 1994 */

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

#undef EXPORT
#define EXPORT extern
#include "edit.var"
#include "frame.var"
#include "appdialogue.var"

static boolean      AbandonFermer;
static boolean      SauverAvantFermer;

#include "views_f.h"
#include "appdialogue_f.h"

#ifdef __STDC__
extern void         FermerDocument (PtrDocument);

#else  /* __STDC__ */
extern void         FermerDocument ();

#endif /* __STDC__ */

/* ---------------------------------------------------------------------- */
/* |    RetMenuFermer   traite les retours du formulaire "Fermer"       | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                RetMenuFermer (int ref, int typedata, char *data)
#else  /* __STDC__ */
void                RetMenuFermer (ref, typedata, data)
int                 ref;
int                 typedata;
char               *data;

#endif /* __STDC__ */
{
   switch ((int) data)
	 {
	    case 0:
	       /* abandon du formulaire */
	       break;
	    case 1:
	       /* sauver avant de fermer */
	       AbandonFermer = FALSE;
	       SauverAvantFermer = TRUE;
	       break;
	    case 2:
	       /* fermer sans sauver */
	       AbandonFermer = FALSE;
	       break;
	 }
   TtaDestroyDialogue (NumFormClose);
}

/* ---------------------------------------------------------------------- */
/* |    ConfirmeFerme demande a` l'utilisateur s'il veut sauver puis    | */
/* |            fermer le document dont le contexte est pointe' par     | */
/* |            pDoc.                                                   | */
/* |            Retourne un booleen indiquant si la fermeture du        | */
/* |            document doit avoir lieu.                               | */
/* |            Au retour, Sauver indique si la sauvegarde est demandee.| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                ConfirmeFerme (PtrDocument pDoc, Document document, View view, boolean * confirme, boolean * sauver)
#else  /* __STDC__ */
void                ConfirmeFerme (pDoc, document, view, confirme, sauver)
PtrDocument         pDoc;
Document            document;
View                view;
boolean            *confirme;
boolean            *sauver;

#endif /* __STDC__ */
{
   char                buftext[300];
   char                bufbutton[300];
   int                 i;

   AbandonFermer = TRUE;
   SauverAvantFermer = FALSE;
   /* le document a ete modifie', on propose de le sauver */
   /* initialise le label du formulaire "Fermer" en y mettant le nom */
   /* du document */
   strcpy (buftext, TtaGetMessage (LIB, SAVE_DOC));
   strcat (buftext, " ");
   strcat (buftext, pDoc->DocDName);
   strcat (buftext, " ");
   strcat (buftext, TtaGetMessage (LIB, BEFORE_CLOSING));

   /* Feuille de dialogue Fermer */
   /* ++++++++++++++++++++++++++ */
   strcpy (bufbutton, TtaGetMessage (LIB, SAVE_DOC));
   i = strlen (TtaGetMessage (LIB, SAVE_DOC)) + 1;
   strcpy (bufbutton + i, TtaGetMessage (LIB, CLOSE_DON_T_SAVE));
   TtaNewSheet (NumFormClose, TtaGetViewFrame (document, view), 0, 0,
		TtaGetMessage (LIB, CLOSE_DOC), 2, bufbutton, TRUE, 1, 'L', D_CANCEL);
   /* label indiquant le nom du document a sauver avant de fermer */
   TtaNewLabel (NumLabelSaveBeforeClosing, NumFormClose, buftext);
   /* active le formulaire "Fermer" */
   TtaShowDialogue (NumFormClose, FALSE);
   /* attend le retour de ce formulaire (traite' par RetMenuFermer) */
   TtaWaitShowDialogue ();
   *sauver = SauverAvantFermer;
   *confirme = !AbandonFermer;
}


/* ---------------------------------------------------------------------- */
/* | FermeUnDocument ferme un document.                                 | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
boolean             FermeUnDocument (Document document, Document docform, View viewform)
#else  /* __STDC__ */
boolean             FermeUnDocument (document, docform, viewform)
Document            document;
View                view;

#endif /* __STDC__ */
{
   PtrDocument         pDoc;
   boolean             sauver, ok;

   ok = TRUE;
   if (document != 0)
     {
	pDoc = LoadedDocument[document - 1];
	if (pDoc != NULL)
	   /* il y a un document pour cette entree de la table */
	  {
	     /* si le document a ete modifie', demande a l'utilisateur s'il */
	     /* veut le sauver. L'utilisateur peut aussi annuler la commande. */
	     if (!pDoc->DocReadOnly && pDoc->DocModified)
	       {
		  /* Faut-il creer le formulaire TtcCloseDocument */
		  if (ThotLocalActions[T_confirmclose] == NULL)
		    {
		       /* Connecte le traitement de la TtcCloseDocument */
		       TteConnectAction (T_confirmclose, (Proc) ConfirmeFerme);
		       TteConnectAction (T_rconfirmclose, (Proc) RetMenuFermer);
		    }
		  ConfirmeFerme (pDoc, docform, viewform, &ok, &sauver);
		  if (ok)
		     /* pas d'annulation */
		    {
		       if (sauver)
			  /* l'utilisateur demande a sauver le document */
			  ok = SauveDocument (pDoc, 4);
		       if (ok)
			  /* tout va bien, on ferme ce document */
			  FermerDocument (pDoc);
		    }
	       }
	     else
		FermerDocument (pDoc);
	  }
     }
   return (ok);
}


/* ---------------------------------------------------------------------- */
/* | TtcCloseDocument initialise la fermeture de document.                      | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtcCloseDocument (Document document, View view)
#else  /* __STDC__ */
void                TtcCloseDocument (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   FermeUnDocument (document, document, view);
}
