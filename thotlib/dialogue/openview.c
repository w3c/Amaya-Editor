
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */
/* I. Vatton    Mai 1994 */

#include "thot_sys.h"
#include "constmenu.h"
#include "constmedia.h"
#include "typemedia.h"
#include "app.h"
#include "appdialogue.h"
#include "tree.h"
#include "libmsg.h"
#include "message.h"
#include "dialog.h"

#undef EXPORT
#define EXPORT extern
#include "edit.var"
#include "appdialogue.var"
#include "frame.var"

static PtrDocument  DocVueAOuvrir;
static int          NumeroVueAOuvrir;
static PtrElement   SousArbreVueAOuvrir;
static DocViewNumber    VueDeReference;

#include "views_f.h"
#include "appdialogue_f.h"


/* ---------------------------------------------------------------------- */
/* | returnopenview met a jour le formulaire de openview.               | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                returnopenview (int ref, int typedata, char *data)
#else  /* __STDC__ */
void                returnopenview (ref, typedata, data)
int                 ref;
int                 typedata;
char               *data;

#endif /* __STDC__ */
{
   if ((int) data > 0)
      if (DocVueAOuvrir != NULL)
	 if (DocVueAOuvrir->DocSSchema != NULL)
	   {
	      NumeroVueAOuvrir = (int) data;
	      TraiteRetMenuVues (DocVueAOuvrir, NumeroVueAOuvrir,
				 SousArbreVueAOuvrir, VueDeReference);
	   }
}


/* ---------------------------------------------------------------------- */
/* | TtcOpenView initialise le menu de openview.                        | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtcOpenView (Document document, View vue)
#else  /* __STDC__ */
void                TtcOpenView (document, vue)
Document            document;
View                vue;

#endif /* __STDC__ */
{
   int                 k, l, nbitem;
   char               *src;
   char               *dest;
   char                Buf[MAX_TXT_LEN];
   char                BufMenu[MAX_TXT_LEN];

   PtrDocument         pDoc;

   /* Connexion initiale */
   if (ThotLocalActions[T_openview] == NULL)
     {
	/* Connecte les actions liees au traitement de la commande */
	TteConnectAction (T_openview, (Proc) returnopenview);
     }

   pDoc = TabDocuments[document - 1];
   DocVueAOuvrir = pDoc;
   VueDeReference = vue;

   if (DocVueAOuvrir != NULL)
     {
	/* construit le menus des vues que l'on peut ouvrir */
	MenuVuesAOuvrir (pDoc, Buf, &nbitem);
	if (nbitem == 0)
	   TtaDisplaySimpleMessage (INFO, LIB, LIB_ALL_VIEWS_ALREADY_CREATED);
	else
	  {
	     NumeroVueAOuvrir = -1;
	     /* ajoute un 'B' au debut de chaque entree du menu */
	     dest = &BufMenu[0];
	     src = &Buf[0];
	     for (k = 1; k <= nbitem; k++)
	       {
		  strcpy (dest, "B");
		  dest++;
		  l = strlen (src);
		  strcpy (dest, src);
		  dest += l + 1;
		  src += l + 1;
	       }
	     TtaNewPopup (NumMenuVuesAOuvrir, 0, TtaGetMessage (LIB, LIB_VIEWS), nbitem,
			  BufMenu, NULL, 'L');
	     TtaShowDialogue (NumMenuVuesAOuvrir, FALSE);
	  }
     }
}


/* ---------------------------------------------------------------------- */
/* | TtcCloseView ferme une frame de document.                          | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtcCloseView (Document document, View view)
#else  /* __STDC__ */
void                TtcCloseView (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   PtrDocument         pDoc;
   int                 vue;
   boolean             assoc;

   pDoc = TabDocuments[document - 1];
   if (pDoc != NULL)
     {
	GetViewInfo (document, view, &vue, &assoc);
	FermerVueDoc (pDoc, vue, assoc);
     }
}


/* ---------------------------------------------------------------------- */
/* |    TtcSynchronizeView l'utilisateur demande le changement du mode  | */
/* |            de synchronisation de la vue indiquee du document.      | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                changesynchronize (Document document, View view)
#else  /* __STDC__ */
void                changesynchronize (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   PtrDocument         pDoc;

   if (document == 0 && view == 0)
      return;
   /* les vues d'elements associes ne sont pas concernees */
   else if (view < 100)
     {
	/* change le mode de synchronisation de la vue */
	pDoc = TabDocuments[document - 1];
	pDoc->DocView[view - 1].DvSync = !pDoc->DocView[view - 1].DvSync;
     }
}
