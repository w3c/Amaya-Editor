
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
#include "application.h"
#include "document.h"
#include "thotfile.h"
#include "thotdir.h"

#define MAX_ARGS 20

#ifdef SONY
char               *strstr (s1, s2)
char               *s1, *s2;
{
   char               *tmp;
   int                 i, lg;
   boolean             encore;

   tmp = s1;
   lg = strlen (s2);
   encore = (tmp != NULL);
   while (encore)
     {
	tmp = index (tmp, s2[0]);
	encore = FALSE;
	if (tmp != NULL)
	  {
	     i = 1;
	     while (i < lg)
		if (tmp[i] == s2[i])
		   i++;
		else
		  {
		     encore = TRUE;
		     tmp = &tmp[1];
		     i = lg;
		  }
	  }
     }
   return (tmp);
}
#endif

#undef EXPORT
#define EXPORT extern
#include "edit_tv.h"
#include "appdialogue_tv.h"
#include "platform_tv.h"

static PathBuffer   NomDirectory;
static Name          SchStrImport;

/* static PathBuffer DirectoryDocImport; */
static Name          NomSchemaSaisi;

#include "browser_f.h"
#include "config_f.h"
#include "views_f.h"
#include "dofile_f.h"
#include "appdialogue_f.h"
#include "viewcommands_f.h"
#include "platform_f.h"
#include "opendoc_f.h"
#include "structschema_f.h"


/* ---------------------------------------------------------------------- */
/* | BuildPathDocBuffer construit une chaine des paths de documents     | */
/* |                    pour les dialogues de l'editeur Thot.                   | */
/* |                    la chaine est construite dans BufDir et les entrees sont        | */
/* |                    separees par separator, le nombre d'entrees est | */
/* |                    retourne dans nbitem                            | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                BuildPathDocBuffer (char *BufDir, char separator, int *nbitem)
#else  /* __STDC__ */
void                BuildPathDocBuffer (BufDir, separator, nbitem)
char               *BufDir;
char                separator;
int                *nbitem;

#endif /* __STDC__ */
{
   int                 i, nb;

   strncpy (BufDir, DocumentPath, MAX_PATH);

   /* remplace PATH_SEP par separator pour le formulaire de saisie des documents */
   nb = 1;
   for (i = 0; i < MAX_PATH && BufDir[i] != '\0'; i++)
      if (BufDir[i] == PATH_SEP)
	{
	   BufDir[i] = separator;
	   nb++;
	}
   *nbitem = nb;
}


/* ---------------------------------------------------------------------- */
/* |    SaisitNomSchPres        demande a l'utilisateur le schema de    | */
/* |    presentation a charger pour le schema de structure pSchStr.     | */
/* |    A l'appel, name contient le nom du schema propose' par Thot,    | */
/* |    au retour, name contient le nom entre' par l'utilisateur.       | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                SaisitNomSchPres (PtrSSchema pSchStr, Name name)

#else  /* __STDC__ */
void                SaisitNomSchPres (pSchStr, name)
PtrSSchema        pSchStr;
Name                 name;

#endif /* __STDC__ */

{

   /* Formulaire du schema de presentation */
   /* ++++++++++++++++++++++++++++++++++++ */
   TtaNewForm (NumFormPresentationSchema, 0, 0, 0,
	       TtaGetMessage (LIB, PRES), TRUE, 1, 'L', D_DONE);

   /* zone de saisie du nom du schema de presentation */
   TtaNewTextForm (NumZonePresentationSchema, NumFormPresentationSchema,
		   TtaGetMessage (LIB, PRES), 30, 1, FALSE);

   /* presentation par defaut */
   if (pSchStr->SsExtension)
      /* c'est une extension de schema, il n'y a pas de regle racine */
      TtaDisplayMessage (INFO, TtaGetMessage(LIB, ENTER_PRS_SCH),
				     pSchStr->SsName);
   else
      /* on prend le nom de la regle racine, qui est traduit dans la */
      /* langue de l'utilisateur, plutot que le nom du schema, qui n'est */
      /* pas traduit */
      TtaDisplayMessage (INFO, TtaGetMessage(LIB, ENTER_PRS_SCH),
		     pSchStr->SsRule[pSchStr->SsRootElem - 1].SrName);
   /* demande un autre nom de fichier a l'utilisateur */
   TtaSetTextForm (NumZonePresentationSchema, name);
   TtaShowDialogue (NumFormPresentationSchema, FALSE);
   /* attend la reponse de l'utilisateur */
   TtaWaitShowDialogue ();
   strncpy (name, NomSchemaSaisi, MAX_NAME_LENGTH);
   /* efface le message */

   /* detruit le formulaire */
   TtaDestroyDialogue (NumFormPresentationSchema);
}


/* ---------------------------------------------------------------------- */
/* | retpresentation met a jour le choix du schema de presentation.     | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                retpresentation (int ref, int typedata, char *data)

#else  /* __STDC__ */
void                retpresentation (ref, typedata, data)
int                 ref;
int                 typedata;
char               *data;

#endif /* __STDC__ */
{
   switch (ref)
	 {
	    case NumFormPresentationSchema:
	       /* formulaire demandant le nom du schema de presentation */

	       break;
	    case NumZonePresentationSchema:
	       /* zone de saisie du nom du schema de presentation */
	       strncpy (NomSchemaSaisi, data, MAX_NAME_LENGTH);
	       break;
	 }
}


/* ---------------------------------------------------------------------- */
/* | retimport met a jour le formulaire de import.                      | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                retimport (int ref, int typedata, char *data)

#else  /* __STDC__ */
void                retimport (ref, typedata, data)
int                 ref;
int                 typedata;
char               *data;

#endif /* __STDC__ */
{
   int                 val;

   val = (int) data;
   switch (ref)
	 {
	    case NumSelectImportClass:
	       /* conserve le nom interne du schema de structure d'importation */
	       ConfigNomInterneSSchema (data, SchStrImport, TRUE);
	       if (SchStrImport[0] == '\0')
		  /* pas de fichier .langue, on prend le nom tel quel */
		 {
		    strncpy (SchStrImport, data, MAX_NAME_LENGTH - 1);
		    SchStrImport[MAX_NAME_LENGTH - 1] = '\0';
		 }
	       break;
	    case NumFormImportClass:
	       /* retour du formulaire lui-meme */
	       if (val == 0)
		  return;	/* abandon */
	       /* le nom du fichier a importer a deja ete saisie par "Ouvrir" */
	       TraiteMenuImporter (SchStrImport, NomDirectory, DefaultDocumentName);
	       break;
	 }
}


/* ---------------------------------------------------------------------- */
/* | retopendoc met a jour le formulaire de opendoc.                    | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                retopendoc (int ref, int typedata, char *data)

#else  /* __STDC__ */
void                retopendoc (ref, typedata, data)
int                 ref;
int                 typedata;
char               *data;

#endif /* __STDC__ */
{
   int                 val;
   PathBuffer          nomdoc;
   PtrDocument         pDoc;
   int                 i;
   char                BufDir[MAX_PATH];

   val = (int) data;
   switch (ref)
	 {
	    case NumZoneDocNameToOpen:
	       if (TtaCheckDirectory (data) && data[strlen (data) - 1] != DIR_SEP)
		 {
		    strcpy (NomDirectory, data);
		    DefaultDocumentName[0] = '\0';
		 }
	       else
		 {
		    /* conserve le nom du document a ouvrir */
		    TtaExtractName (data, NomDirectory, nomdoc);
		    i = strlen (nomdoc);
		    if (i >= MAX_NAME_LENGTH)
		      {
			 i = MAX_NAME_LENGTH;	/*Longueur du nom limitee */
			 nomdoc[i] = '\0';
		      }
		    strcpy (DefaultDocumentName, nomdoc);
		 }

	       if (TtaCheckDirectory (NomDirectory))
		 {
		    /* Est-ce un nouveau directory qui contient des documents */
		    if (!TtaIsInDocumentPath (NomDirectory))
		       if (TtaIsSuffixFileIn (NomDirectory, ".PIV"))
			 {
			    /* il faut ajouter le directory au path */
			    i = strlen (DocumentPath);
			    if (i + strlen (NomDirectory) + 2 < MAX_PATH)
			      {
				 strcat (DocumentPath, PATH_STR);
				 strcat (DocumentPath, NomDirectory);
				 BuildPathDocBuffer (BufDir, '\0', &i);
				 TtaNewSelector (NumZoneDirOpenDoc, NumFormOpenDoc,
						 TtaGetMessage (LIB, DOC_DIR), i, BufDir, 9, NULL, FALSE, TRUE);

				 TtaListDirectory (NomDirectory, NumFormOpenDoc, NULL, -1,
						   ".PIV", TtaGetMessage (LIB, FILES), NumSelDoc);
			      }
			 }
		 }
	       break;
	    case NumZoneDirOpenDoc:
	       strcpy (NomDirectory, data);
	       TtaSetTextForm (NumZoneDocNameToOpen, NomDirectory);
	       TtaListDirectory (data, NumFormOpenDoc, NULL, -1,
			 ".PIV", TtaGetMessage (LIB, FILES), NumSelDoc);
	       break;
	    case NumSelDoc:
	       if (NomDirectory[0] == '\0')
		 {
		    /* compose le path complet du fichier pivot */
		    strncpy (NomDirectory, DocumentPath, MAX_PATH);
		    BuildFileName (nomdoc, "PIV", NomDirectory, data, &i);
		    TtaExtractName (nomdoc, NomDirectory, DefaultDocumentName);
		 }
	       else
		 {
		    strcpy (nomdoc, NomDirectory);
		    strcat (nomdoc, DIR_STR);
		    i = strlen (data);
		    if (i >= MAX_NAME_LENGTH)
		      {
			 /* RemoveElement le suffixe .PIV du nom de fichier */
			 if (!strcmp (&data[i - 4], ".PIV"))
			    data[i - 4] = '\0';
		      }
		    strncpy (DefaultDocumentName, data, MAX_NAME_LENGTH);
		    DefaultDocumentName[MAX_NAME_LENGTH - 1] = '\0';
		    strcat (nomdoc, DefaultDocumentName);
		 }
	       TtaSetTextForm (NumZoneDocNameToOpen, nomdoc);
	       break;
	    case NumFormOpenDoc:
	       if (val == 0)
		 {		/* abandon */
		    TtaDestroyDialogue (NumFormOpenDoc);
		    TtaDestroyDialogue (NumFormImportClass);
		    return;
		 }
	       /* le formulaire Ouvrir Document */
	       if (NomDirectory[0] == '\0')
		  /* compose le path complet du fichier pivot */
		  strncpy (NomDirectory, DocumentPath, MAX_PATH);
	       else if (TtaCheckDirectory (NomDirectory))
		  /* Est-ce un nouveau directory de documents */
		  if (!TtaIsInDocumentPath (NomDirectory))
		    {
		       /* il faut ajouter le directory au path */
		       i = strlen (DocumentPath);
		       if (i + strlen (NomDirectory) + 2 < MAX_PATH)
			 {
			    strcat (DocumentPath, PATH_STR);
			    strcat (DocumentPath, NomDirectory);
			 }
		    }

	       BuildFileName (DefaultDocumentName, "PIV", NomDirectory, nomdoc, &i);
	       /* teste si le fichier 'PIV' existe */
	       if (FileExist (nomdoc) != 0)
		  /* le fichier PIV existe, on ouvre le document */
		 {
		    /* acquiert et initialise un descripteur de document */
		    CreateDocument (&pDoc);
		    if (pDoc != NULL)
		      {
			 strcpy (pDoc->DocDirectory, NomDirectory);
			 /* charge le document */
			 LoadDocument (&pDoc, DefaultDocumentName);
		      }
		 }
	       else
		  /* Le fichier PIV n'existe pas */
		 {
		    /* cherche s'il existe un fichier de ce nom, sans extension */
		    strncpy (NomDirectory, DocumentPath, MAX_PATH);
		    BuildFileName (DefaultDocumentName, "", NomDirectory, nomdoc, &i);
		    if (FileExist (nomdoc) == 0)
		       /* le fichier n'existe pas */
		       TtaDisplayMessage (INFO, TtaGetMessage(LIB, LIB_MISSING_FILE), DefaultDocumentName);
		    else
		       /* le fichier existe ; c'est sans doute une importation */
		       /* demande le schema de structure d'importation */
		      {
			 TtaShowDialogue (NumFormImportClass, TRUE);
			 TtaWaitShowDialogue ();
		      }
		 }
	       /* indique le nom du fichier charge */
	       TtaDestroyDialogue (NumFormOpenDoc);
	       TtaDestroyDialogue (NumFormImportClass);
	       break;
	 }
}


/* ---------------------------------------------------------------------- */
/* | TtcOpenDocument initialise le changement de opendoc.                       | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtcOpenDocument (Document document, View view)

#else  /* __STDC__ */
void                TtcOpenDocument (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   char                BufMenu[MAX_TXT_LEN];
   char                BufDir[MAX_PATH];
   PathBuffer          nomdoc;
   int                 longueur, nbitem;

   if (ThotLocalActions[T_opendoc] == NULL)
     {
	/* Connecte les actions liees au traitement de la opendoc */
	TteConnectAction (T_opendoc, (Proc) retopendoc);
	TteConnectAction (T_import, (Proc) retimport);
	TteConnectAction (T_presentation, (Proc) retpresentation);
	TteConnectAction (T_presentchoice, (Proc) SaisitNomSchPres);
	TteConnectAction (T_buildpathdocbuffer, (Proc) BuildPathDocBuffer);
     }
   /* Creation du Formulaire Ouvrir */
   /* +++++++++++++++++++++++++++++ */
   TtaNewForm (NumFormOpenDoc, 0, 0, 0,
	  TtaGetMessage (LIB, OPEN_DOC), TRUE, 2, 'L', D_CANCEL);
   /* zone de saisie des dossiers documents */
   BuildPathDocBuffer (BufDir, '\0', &nbitem);
   TtaNewSelector (NumZoneDirOpenDoc, NumFormOpenDoc,
		   TtaGetMessage (LIB, DOC_DIR), nbitem, BufDir, 6, NULL, FALSE, TRUE);
   if (NomDirectory[0] == '\0' && nbitem >= 1)
      /* si pas de dossier courant, on initialise avec le premier de BufDir */
     {
	strcpy (NomDirectory, BufDir);
	strcpy (DefaultDocumentName, BufDir);
	TtaSetSelector (NumZoneDirOpenDoc, 0, NULL);
     }
   else if (NomDirectory[0] != '\0')
     {
	strcpy (nomdoc, NomDirectory);
	longueur = strlen (nomdoc);
	nomdoc[longueur] = DIR_SEP;
	nomdoc[longueur + 1] = '\0';
	strcpy (DefaultDocumentName, nomdoc);
     }
   /* liste des fichiers existants */
   TtaListDirectory (NomDirectory, NumFormOpenDoc, NULL, -1, ".PIV", TtaGetMessage (LIB, FILES), NumSelDoc);
   /* zone de saisie du nom du document a ouvrir */
   TtaNewTextForm (NumZoneDocNameToOpen, NumFormOpenDoc,
		   TtaGetMessage (LIB, DOCUMENT_NAME), 50, 1, TRUE);
   TtaSetTextForm (NumZoneDocNameToOpen, DefaultDocumentName);

   /* Formulaire Classe du document a importer */
   /* ++++++++++++++++++++++++++++++++++++++++ */
   TtaNewForm (NumFormImportClass, 0, 0, 0,
	   TtaGetMessage (LIB, IMPORT_DOC_TYPE), FALSE, 1, 'L', D_DONE);
   /* selecteur ou zone de saisie Classe du document a importer */
   nbitem = ConfigMakeImportMenu (BufMenu);
   if (nbitem == 0)
      /* pas d'import defini dans le fichier de langue, */
      /* on cree une simple zone de saisie de texte */
      TtaNewTextForm (NumSelectImportClass, NumFormImportClass,
		    TtaGetMessage (LIB, IMPORT_DOC_TYPE), 30, 1, FALSE);
   else
      /* on cree un selecteur */
     {
	if (nbitem >= 6)
	   longueur = 6;
	else
	   longueur = nbitem;
	TtaNewSelector (NumSelectImportClass, NumFormImportClass,
			TtaGetMessage (LIB, IMPORT_DOC_TYPE), nbitem, BufMenu, longueur, NULL, TRUE, FALSE);
	/* initialise le selecteur sur sa premiere entree */
	TtaSetSelector (NumSelectImportClass, 0, "");
     }
   TtaSetDialoguePosition ();
   TtaShowDialogue (NumFormOpenDoc, TRUE);
}
