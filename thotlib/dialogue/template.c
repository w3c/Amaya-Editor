
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */
/* I. Vatton    Mai 1994 */

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmenu.h"
#include "app.h"
#include "constmedia.h"
#include "typemedia.h"
#include "appdialogue.h"
#include "tree.h"
#include "message.h"
#include "dialog.h"

#include "appdialogue_f.h"
#include "callback_f.h"
#include "viewcommands_f.h"


/* ---------------------------------------------------------------------- */
/* | retXXX met a jour le formulaire de XXX.                    | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                retXXX (int ref, int typedata, char *data)

#else  /* __STDC__ */
void                retXXX (ref, typedata, data)
int                 ref;
int                 typedata;
char               *data;

#endif /* __STDC__ */
{
}


/* ---------------------------------------------------------------------- */
/* | XXX initialise le changement de XXX.                       | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                XXX (Document document, View view)

#else  /* __STDC__ */
void                XXX (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   int                 i;
   char                chaine[100];

   /* Faut-il creer le formulaire XXX */
   if (ThotLocalActions[G_chXXX] == NULL)
     {
	/* Connecte les actions liees au traitement de la XXX */
	TteConnectAction (G_chXXX, (Proc) retXXX);
     }

   TtaShowDialogue (NumMenuXXX, TRUE);
}
