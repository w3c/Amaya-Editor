
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
#include "thotfile.h"
#include "thotdir.h"

#undef EXPORT
#define EXPORT extern
#include "edit.var"
#include "appdialogue.var"
#include "environ.var"
int                 CurrentDialog;

static char         NomDocACreer[100] = "";
static char         ClasseDocACreer[100] = "";
static char         DirectoryDocACreer[MAX_PATH] = "";

#include "structschema_f.h"
#include "browser_f.h"
#include "platform_f.h"
#include "appdialogue_f.h"
#include "views_f.h"
#include "config_f.h"
#include "opendoc_f.h"

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
      TtaShowDialogue (CurrentDialog, FALSE);
   else
     {
	if (ref != 0)
	   if (CurrentDialog == NumFormCreateDoc)
	     {
		/* confirme la creation */
		CreateDocument (&pDoc);	/* acquiert un contexte de document */
		if (pDoc != NULL)
		   /* cree un document dans ce contexte */
		   NewDocument (&pDoc, (PtrBuffer) ClasseDocACreer,
				NomDocACreer, DirectoryDocACreer);
	     }
	   else if (CurrentDialog == NumFormSaveAs)
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
	    case NumFormCreateDoc:
	       /* le formulaire "Creer un document" lui-meme */
	       if (NomDocACreer[0] == '\0')
		  /* le nom par defaut */
		  strcpy (NomDocACreer, TtaGetMessage (LIB, NO_NAME));
	       CurrentDialog = NumFormCreateDoc;
	       if (ClasseDocACreer[0] != '\0' && ((int) data) == 1)
		 {
		    /* on a tous les parametres */
		    /* ******** verifier que le document n'esixte pas deja **** */
		    strcpy (nomdoc, DirectoryDocACreer);
		    strcat (nomdoc, DIR_STR);
		    strcat (nomdoc, NomDocACreer);
		    strcat (nomdoc, ".PIV");

		    if (!TtaCheckDirectory (DirectoryDocACreer))
		       TtaDisplayMessage (INFO, TtaGetMessage(LIB, MISSING_DIR), DirectoryDocACreer);
		    else
		      {
			 if (FileExist (nomdoc))
			   {
			      /* demande confirmation */
			      sprintf (BufDir, TtaGetMessage (LIB, FILE_EXIST), nomdoc);
			      TtaNewLabel (NumLabelConfirm, NumFormConfirm, BufDir);
/*           ClasseDocACreer[0] = '\0'; */
			      TtaSetDialoguePosition ();
			      TtaShowDialogue (NumFormConfirm, FALSE);
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
	    case NumZoneDocNameToCreate:
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
			    i = strlen (DocumentPath);
			    if (i + strlen (DirectoryDocACreer) + 2 < MAX_PATH)
			      {
				 strcat (DocumentPath, PATH_STR);
				 strcat (DocumentPath, DirectoryDocACreer);
				 BuildPathDocBuffer (BufDir, '\0', &i);
				 TtaNewSelector (NumZoneDocDirToCreate, NumFormCreateDoc, TtaGetMessage (LIB, DOC_DIR), i, BufDir, 9, NULL, FALSE, TRUE);
			      }
			 }
		 }
	       break;
	    case NumZoneDocDirToCreate:
	       /* zone de saisie du directory ou le document doit etre cree */
	       strcpy (DirectoryDocACreer, data);
	       strcpy (nomdoc, DirectoryDocACreer);
	       strcat (nomdoc, DIR_STR);
	       strcat (nomdoc, NomDocACreer);
	       TtaSetTextForm (NumZoneDocNameToCreate, nomdoc);
	       break;
	    case NumSelDocClassToCreate:
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
   TtaNewForm (NumFormCreateDoc, 0, 0, 0,
	  TtaGetMessage (LIB, CREATE_DOC), TRUE, 2, 'L', D_DONE);
   /* zone de saisie des dossiers documents */
   BuildPathDocBuffer (BufDir, '\0', &nbitem);
   TtaNewSelector (NumZoneDocDirToCreate, NumFormCreateDoc,
		   TtaGetMessage (LIB, DOC_DIR), nbitem, BufDir, 9, NULL, FALSE, TRUE);
   if (nbitem >= 1)
      TtaSetSelector (NumZoneDocDirToCreate, 0, NULL);
   /* nom du document a creer */
   if (DocumentPath == NULL)
      DirectoryDocACreer[i] = '\0';
   else
     {
	ptr = strstr (DocumentPath, PATH_STR);
	if (ptr == NULL)
	   strcpy (DirectoryDocACreer, DocumentPath);
	else
	  {
	     i = (int) ptr - (int) DocumentPath;
	     strncpy (DirectoryDocACreer, DocumentPath, i);
	     DirectoryDocACreer[i] = '\0';
	  }
     }
   strcpy (NomDocACreer, TtaGetMessage (LIB, NO_NAME));
   strcpy (nomdoc, DirectoryDocACreer);
   strcat (nomdoc, DIR_STR);
   strcat (nomdoc, NomDocACreer);
   /* compose le selecteur des types de documents que l'utilisateur peut */
   /* creer */
   nbitem = ConfigMakeDocTypeMenu (BufMenu, &longueur, TRUE);
   if (nbitem > 0)
      /* le fichier Start Up definit des classes de documents */
     {
	/* calcule la hauteur de la partie menu du selecteur */
	if (nbitem < 5)
	   longueur = nbitem;
	else
	   longueur = 5;
	/* cree le selecteur */
	TtaNewSelector (NumSelDocClassToCreate, NumFormCreateDoc,
			TtaGetMessage (LIB, DOC_TYPE), nbitem, BufMenu, longueur, NULL, TRUE, FALSE);
	/* initialise le selecteur sur sa premiere entree */
	TtaSetSelector (NumSelDocClassToCreate, 0, "");
     }
   else
      /* on n'a pas cree' de selecteur, on cree une zone de saisie */
      TtaNewTextForm (NumSelDocClassToCreate, NumFormCreateDoc,
		      TtaGetMessage (LIB, DOC_TYPE), 30, 1, FALSE);

   /* zone de saisie du nom du document a creer */
   TtaNewTextForm (NumZoneDocNameToCreate, NumFormCreateDoc,
		   TtaGetMessage (LIB, DOCUMENT_NAME), 50, 1, TRUE);
   TtaSetTextForm (NumZoneDocNameToCreate, nomdoc);

   /* Formulaire Confirmation creation */
   /* ++++++++++++++++++++++++++++++++ */
   strcpy (BufMenu, TtaGetMessage (LIB, RENAME));
   i = strlen (BufMenu) + 1;
   strcpy (&BufMenu[i], TtaGetMessage (LIB, LIB_CONFIRM));
   TtaNewDialogSheet (NumFormConfirm, 0, 0, 0, NULL, 2, BufMenu, FALSE, 1, 'L', D_DONE);

/* affichage du formulaire Creer document */
   TtaSetDialoguePosition ();
   TtaShowDialogue (NumFormCreateDoc, FALSE);
}
