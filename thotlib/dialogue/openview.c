/*
 * Copyright (c) 1996 INRIA, All rights reserved
 */

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
#include "edit_tv.h"
#include "appdialogue_tv.h"
#include "frame_tv.h"

static PtrDocument   ViewToOpenDoc;
static int           ViewToOpenNumber;
static PtrElement    ViewToOpenSubTree;
static DocViewNumber ReferenceView;

#include "views_f.h"
#include "appdialogue_f.h"
#include "viewapi_f.h"

/*----------------------------------------------------------------------
   CallbackOpenView met a jour le formulaire de openview.               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CallbackOpenView (int ref, int dataType, char *data)
#else  /* __STDC__ */
void                CallbackOpenView (ref, dataType, data)
int                 ref;
int                 dataType;
char               *data;

#endif /* __STDC__ */
{
   if ((int) data >= 0)
      if (ViewToOpenDoc != NULL)
	 if (ViewToOpenDoc->DocSSchema != NULL)
	   {
	      ViewToOpenNumber = (int) data;
	      OpenViewByMenu (ViewToOpenDoc, ViewToOpenNumber,
			      ViewToOpenSubTree, ReferenceView);
	   }
}


/*----------------------------------------------------------------------
   TtcOpenView initialise le menu de openview.                        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcOpenView (Document document, View view)
#else  /* __STDC__ */
void                TtcOpenView (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   int                 k, l, nbItems;
   char               *src;
   char               *dest;
   char                buf[MAX_TXT_LEN];
   char                bufMenu[MAX_TXT_LEN];

   PtrDocument         pDoc;

   /* Connexion initiale */
   if (ThotLocalActions[T_openview] == NULL)
     {
	/* Connecte les actions liees au traitement de la commande */
	TteConnectAction (T_openview, (Proc) CallbackOpenView);
     }

   pDoc = LoadedDocument[document - 1];
   ViewToOpenDoc = pDoc;
   ReferenceView = view;

   if (ViewToOpenDoc != NULL)
     {
	/* construit le menus des vues que l'on peut ouvrir */
	BuildViewList (pDoc, buf, &nbItems);
	if (nbItems == 0)
	   TtaDisplaySimpleMessage (INFO, LIB, TMSG_ALL_VIEWS_ALREADY_CREATED);
	else
	  {
	     ViewToOpenNumber = -1;
	     /* ajoute un 'B' au debut de chaque entree du menu */
	     dest = &bufMenu[0];
	     src = &buf[0];
	     for (k = 1; k <= nbItems; k++)
	       {
		  strcpy (dest, "B");
		  dest++;
		  l = strlen (src);
		  strcpy (dest, src);
		  dest += l + 1;
		  src += l + 1;
	       }
	     TtaNewPopup (NumMenuViewsToOpen, 0, TtaGetMessage (LIB, TMSG_VIEWS), nbItems,
			  bufMenu, NULL, 'L');
	     TtaShowDialogue (NumMenuViewsToOpen, FALSE);
	  }
     }
}


/*----------------------------------------------------------------------
   TtcCloseView ferme une frame de document.                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcCloseView (Document document, View viewIn)
#else  /* __STDC__ */
void                TtcCloseView (document, viewIn)
Document            document;
View                viewIn;

#endif /* __STDC__ */
{
   PtrDocument         pDoc;
   int                 view;
   boolean             assoc;

   pDoc = LoadedDocument[document - 1];
   if (pDoc != NULL)
     {
	GetViewInfo (document, viewIn, &view, &assoc);
	CloseView (pDoc, view, assoc);
     }
}


/*----------------------------------------------------------------------
   TtcSynchronizeView l'utilisateur demande le changement du mode  
   de synchronisation de la vue indiquee du document.      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SwitchSynchronize (Document document, View view)
#else  /* __STDC__ */
void                SwitchSynchronize (document, view)
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
	pDoc = LoadedDocument[document - 1];
	pDoc->DocView[view - 1].DvSync = !pDoc->DocView[view - 1].DvSync;
     }
}
