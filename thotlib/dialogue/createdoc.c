
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
#include "libmsg.h"
#include "message.h"
#include "dialog.h"
#include "document.h"

#undef EXPORT
#define EXPORT extern
#include "edit.var"
#include "appdialogue.var"
#include "environ.var"
int                 CurrentDialog;

static char         NomDocACreer[100] = "";
static char         ClasseDocACreer[100] = "";
static char         DirectoryDocACreer[MAX_PATH] = "";

#include "structure.f"
#include "browser.f"
#include "filesystem.f"
#include "appdialogue.f"
#include "docvues.f"
#include "config.f"
#include "environmenu.f"
#include "environ.f"
#include "opendoc.f"

#ifdef __STDC__
extern void         TtaExtractName (char *, char *, char *);

#else  /* __STDC__ */
extern void         TtaExtractName ();

#endif /* __STDC__ */

/* ---------------------------------------------------------------------- */
/* | retconfirm met a jour le formulaire de confirm.                    | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                retconfirm (int ref, int typedata, char *data)
#else  /* __STDC__ */
void                retconfirm (ref, typedata, data)
int                 ref;
int                 typedata;
char               *data;

#endif /* __STDC__ */

{
   PtrDocument         pDoc;

   if ((int) data == 0)
      /* reprends le dialogue courant */
      TtaShowDialogue (CurrentDialog, False);
   else
     {
	if (ref != 0)
	   if (CurrentDialog == NumFormCreerDoc)
	     {
		/* confirme la creation */
		CreateDocument (&pDoc);	/* acquiert un contexte de document */
		if (pDoc != NULL)
		   /* cree un document dans ce contexte */
		   NewDocument (&pDoc, (PtrBuffer) ClasseDocACreer,
				NomDocACreer, DirectoryDocACreer);
	     }
	   else if (CurrentDialog == NumFormSauverComme)
	      if (CurrentDialog != 0)
		{		/* confirme la sauvegarde */
		   if (ThotLocalActions[T_savedoc] != NULL)
		      (*ThotLocalActions[T_savedoc]) ();
		}

	TtaDestroyDialogue (CurrentDialog);
	TtaDestroyDialogue (NumFormConfirm);
     }
}


/* ---------------------------------------------------------------------- */
/* | retcreatedoc met a jour le formulaire de createdoc.                | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                retcreatedoc (int ref, int typedata, char *data)
#else  /* __STDC__ */
void                retcreatedoc (ref, typedata, data)
int                 ref;
int                 typedata;
char               *data;

#endif /* __STDC__ */

{
   PathBuffer          nomdoc;
   int                 i;
   char                BufDir[MAX_PATH];

   switch (ref)
	 {
	    case NumFormCreerDoc:
	       /* le formulaire "Creer un document" lui-meme */
	       if (NomDocACreer[0] == '\0')
		  /* le nom par defaut */
		  strcpy (NomDocACreer, TtaGetMessage (LIB, LIB_NO_NAME));
	       CurrentDialog = NumFormCreerDoc;
	       if (ClasseDocACreer[0] != '\0' && ((int) data) == 1)
		 {
		    /* on a tous les parametres */
		    /* ******** verifier que le document n'esixte pas deja **** */
		    strcpy (nomdoc, DirectoryDocACreer);
		    strcat (nomdoc, DIR_STR);
		    strcat (nomdoc, NomDocACreer);
		    strcat (nomdoc, ".PIV");

		    if (!TtaCheckDirectory (DirectoryDocACreer))
		       TtaDisplaySimpleMessageString (LIB, INFO, LIB_DIRECTORY_NOT_FOUND, DirectoryDocACreer);
		    else
		      {
			 if (FileExist (nomdoc))
			   {
			      /* demande confirmation */
			      sprintf (BufDir, TtaGetMessage (LIB, LIB_FILE_EXIST), nomdoc);
			      TtaNewLabel (NumLabelConfirm, NumFormConfirm, BufDir);
/*           ClasseDocACreer[0] = '\0'; */
			      TtaSetDialoguePosition ();
			      TtaShowDialogue (NumFormConfirm, False);
			   }
			 else
			    /* traite la confirmation */
			 if (ThotLocalActions[T_confirmcreate] != NULL)
			    (*ThotLocalActions[T_confirmcreate]) (NumFormConfirm, 1, (char *) 1);
		      }
		 }
	       else
		  /* annulation : on detruit les dialogues */
		 {
		    if (ThotLocalActions[T_confirmcreate] != NULL)
		       (*ThotLocalActions[T_confirmcreate]) (0, 1, (char *) 1);
		 }
	       /*ClasseDocACreer[0] = '\0'; */
	       break;
	    case NumZoneNomDocACreer:
	       /* zone de saisie du nom du document a creer */
	       if (TtaCheckDirectory (data) && data[strlen (data) - 1] != DIR_SEP)
		 {
		    strcpy (DirectoryDocACreer, data);
		    NomDocACreer[0] = '\0';
		 }
	       else
		 {
		    /* conserve le nom du document a ouvrir */
		    TtaExtractName (data, DirectoryDocACreer, nomdoc);
		    if (strlen (nomdoc) >= MAX_NAME_LENGTH)
		       nomdoc[MAX_NAME_LENGTH - 1] = '\0';	/* limite la longueur des noms */
		    strcpy (NomDocACreer, nomdoc);
		 }
	       if (TtaCheckDirectory (DirectoryDocACreer))
		 {
		    /* Est-ce un nouveau directory qui contient des documents */
		    if (!TtaIsInDocumentPath (DirectoryDocACreer))
		       if (TtaIsSuffixFileIn (DirectoryDocACreer, ".PIV"))
			 {
			    /* il faut ajouter le directory au path */
			    i = strlen (DirectoryDoc);
			    if (i + strlen (DirectoryDocACreer) + 2 < MAX_PATH)
			      {
				 strcat (DirectoryDoc, PATH_STR);
				 strcat (DirectoryDoc, DirectoryDocACreer);
				 BuildPathDocBuffer (BufDir, '\0', &i);
				 TtaNewSelector (NumZoneDirDocACreer, NumFormCreerDoc, TtaGetMessage (LIB, LIB_DOCUMENT_DIRECTORIES), i, BufDir, 9, NULL, False, True);
			      }
			 }
		 }
	       break;
	    case NumZoneDirDocACreer:
	       /* zone de saisie du directory ou le document doit etre cree */
	       strcpy (DirectoryDocACreer, data);
	       strcpy (nomdoc, DirectoryDocACreer);
	       strcat (nomdoc, DIR_STR);
	       strcat (nomdoc, NomDocACreer);
	       TtaSetTextForm (NumZoneNomDocACreer, nomdoc);
	       break;
	    case NumSelClasseDocACreer:
	       /* selecteur classe du document a creer */
	       strncpy (ClasseDocACreer, data, MAX_NAME_LENGTH);
	       break;
	 }
}


/* ---------------------------------------------------------------------- */
/* | TtcCreateDocument initialise le changement de createdoc.           | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtcCreateDocument (Document document, View view)

#else  /* __STDC__ */
void                TtcCreateDocument (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   PathBuffer          nomdoc;
   char               *ptr;
   int                 i = 0, longueur, nbitem;
   char                BufMenu[MAX_TXT_LEN];
   char                BufDir[MAX_PATH];

   if (ThotLocalActions[T_createdoc] == NULL)
     {
	/* Connecte les actions liees au traitement de la createdoc */
	TteConnectAction (T_createdoc, (Proc) retcreatedoc);
	TteConnectAction (T_confirmcreate, (Proc) retconfirm);
	TteConnectAction (T_buildpathdocbuffer, (Proc) BuildPathDocBuffer);
     }
   /* Creation du formulaire Creer document */
   /* +++++++++++++++++++++++++++++++++++++ */
   TtaNewForm (NumFormCreerDoc, 0, 0, 0,
	  TtaGetMessage (LIB, LIB_CREATE_A_DOCUMENT), True, 2, 'L', D_DONE);
   /* zone de saisie des dossiers documents */
   BuildPathDocBuffer (BufDir, '\0', &nbitem);
   TtaNewSelector (NumZoneDirDocACreer, NumFormCreerDoc,
		   TtaGetMessage (LIB, LIB_DOCUMENT_DIRECTORIES), nbitem, BufDir, 9, NULL, False, True);
   if (nbitem >= 1)
      TtaSetSelector (NumZoneDirDocACreer, 0, NULL);
   /* nom du document a creer */
   if (DirectoryDoc == NULL)
      DirectoryDocACreer[i] = '\0';
   else
     {
	ptr = strstr (DirectoryDoc, PATH_STR);
	if (ptr == NULL)
	   strcpy (DirectoryDocACreer, DirectoryDoc);
	else
	  {
	     i = (int) ptr - (int) DirectoryDoc;
	     strncpy (DirectoryDocACreer, DirectoryDoc, i);
	     DirectoryDocACreer[i] = '\0';
	  }
     }
   strcpy (NomDocACreer, TtaGetMessage (LIB, LIB_NO_NAME));
   strcpy (nomdoc, DirectoryDocACreer);
   strcat (nomdoc, DIR_STR);
   strcat (nomdoc, NomDocACreer);
   /* compose le selecteur des types de documents que l'utilisateur peut */
   /* creer */
   nbitem = ConfigMakeDocTypeMenu (BufMenu, &longueur, True);
   if (nbitem > 0)
      /* le fichier Start Up definit des classes de documents */
     {
	/* calcule la hauteur de la partie menu du selecteur */
	if (nbitem < 5)
	   longueur = nbitem;
	else
	   longueur = 5;
	/* cree le selecteur */
	TtaNewSelector (NumSelClasseDocACreer, NumFormCreerDoc,
			TtaGetMessage (LIB, LIB_DOCUMENT_TYPE), nbitem, BufMenu, longueur, NULL, True, False);
	/* initialise le selecteur sur sa premiere entree */
	TtaSetSelector (NumSelClasseDocACreer, 0, "");
     }
   else
      /* on n'a pas cree' de selecteur, on cree une zone de saisie */
      TtaNewTextForm (NumSelClasseDocACreer, NumFormCreerDoc,
		      TtaGetMessage (LIB, LIB_DOCUMENT_TYPE), 30, 1, False);

   /* zone de saisie du nom du document a creer */
   TtaNewTextForm (NumZoneNomDocACreer, NumFormCreerDoc,
		   TtaGetMessage (LIB, LIB_DOCUMENT_NAME), 50, 1, True);
   TtaSetTextForm (NumZoneNomDocACreer, nomdoc);

   /* Formulaire Confirmation creation */
   /* ++++++++++++++++++++++++++++++++ */
   strcpy (BufMenu, TtaGetMessage (LIB, LIB_RENAME));
   i = strlen (BufMenu) + 1;
   strcpy (&BufMenu[i], TtaGetMessage (LIB, LIB_CONFIRM));
   TtaNewDialogSheet (NumFormConfirm, 0, 0, 0, NULL, 2, BufMenu, False, 1, 'L', D_DONE);

/* affichage du formulaire Creer document */
   TtaSetDialoguePosition ();
   TtaShowDialogue (NumFormCreerDoc, False);
}
