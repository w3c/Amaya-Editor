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
#include "fileaccess.h"
#include "thotdir.h"
#include "fileaccess.h"

#define MAX_ARGS 20

#ifdef SONY
/*----------------------------------------------------------------------
   strstr
  ----------------------------------------------------------------------*/
char               *strstr (s1, s2)
char               *s1, *s2;
{
   char               *tmp;
   int                 i, length;
   boolean             continue;

   tmp = s1;
   length = strlen (s2);
   continue = (tmp != NULL);
   while (continue)
     {
	tmp = index (tmp, s2[0]);
	continue = FALSE;
	if (tmp != NULL)
	  {
	     i = 1;
	     while (i < length)
		if (tmp[i] == s2[i])
		   i++;
		else
		  {
		     continue = TRUE;
		     tmp = &tmp[1];
		     i = length;
		  }
	  }
     }
   return (tmp);
}
#endif

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "edit_tv.h"
#include "appdialogue_tv.h"
#include "platform_tv.h"

static PathBuffer   DirectoryName;
static Name         SchStrImport;

/* static PathBuffer DirectoryDocImport; */
static Name         NewSchemaName;

#include "browser_f.h"
#include "config_f.h"
#include "views_f.h"

#include "appdialogue_f.h"
#include "actions_f.h"
#include "viewcommands_f.h"
#include "platform_f.h"
#include "opendoc_f.h"
#include "structschema_f.h"
#include "fileaccess_f.h"
#include "docs_f.h"

/*----------------------------------------------------------------------
   BuildPathDocBuffer
   builds a document path for the Thot editor dialogues.
   The path is built in bufDir and the entries are separated by
   separator. 
   nbItems returns the number of entries.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                BuildPathDocBuffer (char *bufDir, char separator, int *nbItems)
#else  /* __STDC__ */
void                BuildPathDocBuffer (bufDir, separator, nbItems)
char               *bufDir;
char                separator;
int                *nbItems;

#endif /* __STDC__ */
{
   int                 i, nb;

   strncpy (bufDir, DocumentPath, MAX_PATH);

   /* remplace PATH_SEP par separator pour le formulaire de saisie des documents */
   nb = 1;
   for (i = 0; i < MAX_PATH && bufDir[i] != EOS; i++)
      if (bufDir[i] == PATH_SEP)
	{
	   bufDir[i] = separator;
	   nb++;
	}
   *nbItems = nb;
}

/*----------------------------------------------------------------------
   SearchStringInBuffer
   searches the string s in the menu buffer. 
   The buffer contains nbStr string separated by an EOS character.
   Returns the rank of s in buffer or -1 if s doesn't occur.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int SearchStringInBuffer(char *buffer, char *s,int nbStr)
#else  /* __STDC__ */
int  SearchStringInBuffer(buffer, s, nbStr)
char *buffer;
char *s;
int nbStr;
#endif /* __STDC__ */
{
  int occ;
  char *pBuf;
  boolean found = FALSE;

  occ=0;
  pBuf = buffer;
  while (!found && occ < nbStr)
    {
      if(!strcmp(pBuf,s))
	found = TRUE;
      else
	{
	  pBuf = pBuf + strlen(pBuf) + 1;
	  occ++;
	}
    }
  if(found)
    return occ;
  else
    return -1;
}
    


/*----------------------------------------------------------------------
   BuildSchPresNameMenu
   queries the user for the presentation scheme to be loaded by
   the structure scheme pSchStr.
   When the function is called, name contains the schema name proposed
   by Thot.
   When it returns, name contains the name entered by the user.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                BuildSchPresNameMenu (PtrSSchema pSchStr, Name name)

#else  /* __STDC__ */
void                BuildSchPresNameMenu (pSchStr, name)
PtrSSchema          pSchStr;
Name                name;

#endif /* __STDC__ */

{

   /* Formulaire du schema de presentation */
   TtaNewForm (NumFormPresentationSchema,  0,
	       TtaGetMessage (LIB, TMSG_PRES), TRUE, 1, 'L', D_DONE);

   /* zone de saisie du nom du schema de presentation */
   TtaNewTextForm (NumZonePresentationSchema, NumFormPresentationSchema,
		   TtaGetMessage (LIB, TMSG_PRES), 30, 1, FALSE);

   /* presentation par defaut */
   if (pSchStr->SsExtension)
      /* c'est une extension de schema, il n'y a pas de regle racine */
      TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_ENTER_PRS_SCH),
			 pSchStr->SsName);
   else
      /* on prend le nom de la regle racine, qui est traduit dans la */
      /* langue de l'utilisateur, plutot que le nom du schema, qui n'est */
      /* pas traduit */
      TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_ENTER_PRS_SCH),
			 pSchStr->SsRule[pSchStr->SsRootElem - 1].SrName);
   /* demande un autre nom de fichier a l'utilisateur */
   TtaSetTextForm (NumZonePresentationSchema, name);
   TtaShowDialogue (NumFormPresentationSchema, FALSE);
   /* attend la reponse de l'utilisateur */
   TtaWaitShowDialogue ();
   strncpy (name, NewSchemaName, MAX_NAME_LENGTH);
   /* efface le message */

   /* detruit le formulaire */
   TtaDestroyDialogue (NumFormPresentationSchema);
}


/*----------------------------------------------------------------------
   CallbackSchPresNameMenu
   updates the presentation scheme choice.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CallbackSchPresNameMenu (int ref, int typedata, char *data)

#else  /* __STDC__ */
void                CallbackSchPresNameMenu (ref, typedata, data)
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
	       strncpy (NewSchemaName, data, MAX_NAME_LENGTH);
	       break;
	 }
}
/*----------------------------------------------------------------------
BuildImportForm : cree Le formulaire d'importation de documents.
  ----------------------------------------------------------------------*/
static void BuildImportForm()
{
  int nbItems,length;
  char bufMenu[MAX_TXT_LEN];
 
 /* Formulaire Classe du document a importer */
   TtaNewForm (NumFormImportClass,  0,
	  TtaGetMessage (LIB, TMSG_IMPORT_DOC_TYPE), TRUE, 1, 'L', D_DONE);
   /* selecteur ou zone de saisie Classe du document a importer */
   nbItems = ConfigMakeImportMenu (bufMenu);
   if (nbItems == 0)
      /* pas d'import defini dans le fichier de langue, */
      /* on cree une simple zone de saisie de texte */
      TtaNewTextForm (NumSelectImportClass, NumFormImportClass,
		   TtaGetMessage (LIB, TMSG_IMPORT_DOC_TYPE), 30, 1, FALSE);
   else
      /* on cree un selecteur */
     {
	if (nbItems >= 6)
	   length = 6;
	else
	   length = nbItems;
	TtaNewSelector (NumSelectImportClass, NumFormImportClass,
			TtaGetMessage (LIB, TMSG_IMPORT_DOC_TYPE), nbItems, bufMenu, length, NULL, TRUE, FALSE);
	/* initialise le selecteur sur sa premiere entree */
	TtaSetSelector (NumSelectImportClass, 0, "");
     }
}
/*----------------------------------------------------------------------
   CallbackImportMenu
   updates the ImportMenu form.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CallbackImportMenu (int ref, int typedata, char *data)

#else  /* __STDC__ */
void                CallbackImportMenu (ref, typedata, data)
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
	       ConfigSSchemaInternalName (data, SchStrImport, TRUE);
	       if (SchStrImport[0] == EOS)
		  /* pas de fichier .langue, on prend le nom tel quel */
		 {
		    strncpy (SchStrImport, data, MAX_NAME_LENGTH - 1);
		    SchStrImport[MAX_NAME_LENGTH - 1] = EOS;
		 }
	       break;
	    case NumFormImportClass:
	       /* retour du formulaire lui-meme */
	       if (val == 0)
		  return;	/* abandon */
	       /* le nom du fichier a importer a deja ete saisie par "Ouvrir" */
	       ImportDocument (SchStrImport, DirectoryName, DefaultDocumentName);
	       break;
	 }
}


/*----------------------------------------------------------------------
   CallbackOpenDocMenu
   updates the OpenDoc form.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CallbackOpenDocMenu (int ref, int typedata, char *data)

#else  /* __STDC__ */
void                CallbackOpenDocMenu (ref, typedata, data)
int                 ref;
int                 typedata;
char               *data;

#endif /* __STDC__ */
{
   int                 val;
   PathBuffer          docName;
   PtrDocument         pDoc;
   int                 i;
   char                bufDir[MAX_PATH];
   char                URL_DIR_SEP;

   if (typedata == STRING_DATA && data && strchr (data, '/'))
     URL_DIR_SEP = '/';
   else 
     URL_DIR_SEP = DIR_SEP;

   val = (int) data;
   switch (ref)
	 {
	    case NumZoneDocNameToOpen:
	       if (TtaCheckDirectory (data) && data[strlen (data) - 1] != URL_DIR_SEP)
		 {
		    strcpy (DirectoryName, data);
		    DefaultDocumentName[0] = EOS;
		 }
	       else
		 {
		    /* conserve le nom du document a ouvrir */
		    TtaExtractName (data, DirectoryName, docName);
		    i = strlen (docName);
		    if (i >= MAX_NAME_LENGTH)
		      {
			 i = MAX_NAME_LENGTH;	/*Longueur du nom limitee */
			 docName[i] = EOS;
		      }
		    strcpy (DefaultDocumentName, docName);
		 }

	       if (TtaCheckDirectory (DirectoryName))
		 {
		    /* Est-ce un nouveau directory qui contient des documents */
		    if (!TtaIsInDocumentPath (DirectoryName))
		       if (TtaIsSuffixFileIn (DirectoryName, ".PIV"))
			 {
			    /* il faut ajouter le directory au path */
			    i = strlen (DocumentPath);
			    if (i + strlen (DirectoryName) + 2 < MAX_PATH)
			      {
				 strcat (DocumentPath, PATH_STR);
				 strcat (DocumentPath, DirectoryName);
				 BuildPathDocBuffer (bufDir, EOS, &i);
				 TtaNewSelector (NumZoneDirOpenDoc, NumFormOpenDoc,
						 TtaGetMessage (LIB, TMSG_DOC_DIR), i, bufDir, 9, NULL, FALSE, TRUE);

				 TtaListDirectory (DirectoryName, NumFormOpenDoc, NULL, -1,
						   ".PIV", TtaGetMessage (LIB, TMSG_FILES), NumSelDoc);
			      }
			 }
		 }
	       break;
	    case NumZoneDirOpenDoc:
	       strcpy (DirectoryName, data);
	       TtaSetTextForm (NumZoneDocNameToOpen, DirectoryName);
	       TtaListDirectory (data, NumFormOpenDoc, NULL, -1,
			".PIV", TtaGetMessage (LIB, TMSG_FILES), NumSelDoc);
	       break;
	    case NumSelDoc:
	       if (DirectoryName[0] == EOS)
		 {
		    /* compose le path complet du fichier pivot */
		    strncpy (DirectoryName, DocumentPath, MAX_PATH);
		    MakeCompleteName (docName, "PIV", DirectoryName, data, &i);
		    TtaExtractName (docName, DirectoryName, DefaultDocumentName);
		 }
	       else
		 {
		    strcpy (docName, DirectoryName);
		    strcat (docName, DIR_STR);
		    i = strlen (data);
		    if (i >= MAX_NAME_LENGTH)
		      {
			 /* RemoveElement le suffixe .PIV du nom de fichier */
			 if (!strcmp (&data[i - 4], ".PIV"))
			    data[i - 4] = EOS;
		      }
		    strncpy (DefaultDocumentName, data, MAX_NAME_LENGTH);
		    DefaultDocumentName[MAX_NAME_LENGTH - 1] = EOS;
		    strcat (docName, DefaultDocumentName);
		 }
	       TtaSetTextForm (NumZoneDocNameToOpen, docName);
	       break;
	    case NumFormOpenDoc:
	       if (val == 0)
		 {		/* abandon */
		    TtaDestroyDialogue (NumFormOpenDoc);
		    return;
		 }
	       /* le formulaire Ouvrir Document */
	       if (DirectoryName[0] == EOS)
		  /* compose le path complet du fichier pivot */
		  strncpy (DirectoryName, DocumentPath, MAX_PATH);
	       else if (TtaCheckDirectory (DirectoryName))
		  /* Est-ce un nouveau directory de documents */
		  if (!TtaIsInDocumentPath (DirectoryName))
		    {
		       /* il faut ajouter le directory au path */
		       i = strlen (DocumentPath);
		       if (i + strlen (DirectoryName) + 2 < MAX_PATH)
			 {
			    strcat (DocumentPath, PATH_STR);
			    strcat (DocumentPath, DirectoryName);
			 }
		    }

	       MakeCompleteName (DefaultDocumentName, "PIV", DirectoryName, docName, &i);
	       /* teste si le fichier 'PIV' existe */
	       if (TtaFileExist (docName) != 0)
		  /* le fichier PIV existe, on ouvre le document */
		 {
		   /* charge le document */
		   LoadDocument (&pDoc, docName);
		   if (pDoc != NULL)
		     strcpy (pDoc->DocDirectory, DirectoryName);
		 }
	       else
		  /* Le fichier PIV n'existe pas */
		 {
		    /* cherche s'il existe un fichier de ce nom, sans extension */
		    strncpy (DirectoryName, DocumentPath, MAX_PATH);
		    MakeCompleteName (DefaultDocumentName, "", DirectoryName, docName, &i);
		    if (TtaFileExist (docName) == 0)
		       /* le fichier n'existe pas */
		       TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_LIB_MISSING_FILE), DefaultDocumentName);
		    else
		       /* le fichier existe ; c'est sans doute une importation */
		       /* demande le schema de structure d'importation */
		      {
			BuildImportForm();
			TtaShowDialogue (NumFormImportClass, FALSE);
			TtaWaitShowDialogue ();
			TtaDestroyDialogue (NumFormImportClass);
		      }
		 }
	       /* indique le nom du fichier charge */
	       TtaDestroyDialogue (NumFormOpenDoc);
	    
	       break;
	 }
}

/*----------------------------------------------------------------------
   TtcOpenDocument
   initializes the OpenDoc form.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcOpenDocument (Document document, View view)

#else  /* __STDC__ */
void                TtcOpenDocument (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   char                bufDir[MAX_PATH];
   PathBuffer          docName;
   int                 length, nbItems, entry;
   char                URL_DIR_SEP;
   ThotWidget	       parentWidget;
   if (ThotLocalActions[T_opendoc] == NULL)
     {
	/* Connecte les actions liees au traitement de la opendoc */
	TteConnectAction (T_opendoc, (Proc) CallbackOpenDocMenu);
	TteConnectAction (T_import, (Proc) CallbackImportMenu);
	TteConnectAction (T_presentation, (Proc) CallbackSchPresNameMenu);
	TteConnectAction (T_presentchoice, (Proc) BuildSchPresNameMenu);
	TteConnectAction (T_buildpathdocbuffer, (Proc) BuildPathDocBuffer);
     }

   /* Creation du Formulaire Ouvrir */
   parentWidget = TtaGetViewFrame (document, view);
   TtaNewForm (NumFormOpenDoc,  parentWidget,
	       TtaGetMessage (LIB, TMSG_OPEN_DOC), TRUE, 2, 'L', D_CANCEL);
   /* zone de saisie des dossiers documents */
   BuildPathDocBuffer (bufDir, EOS, &nbItems);
   TtaNewSelector (NumZoneDirOpenDoc, NumFormOpenDoc,
   TtaGetMessage (LIB, TMSG_DOC_DIR), nbItems, bufDir, 6, NULL, FALSE, TRUE);
   if (DirectoryName[0] == EOS && nbItems >= 1)
      /* si pas de dossier courant, on initialise avec le premier de bufDir */
     {
	strcpy (DirectoryName, bufDir);
	strcpy (DefaultDocumentName, bufDir);
	TtaSetSelector (NumZoneDirOpenDoc, 0, NULL);
     }
   else if (DirectoryName[0] != EOS)
     {
       if (strchr (DirectoryName, '/'))
	 URL_DIR_SEP = '/';
       else 
	 URL_DIR_SEP = DIR_SEP;

       entry = SearchStringInBuffer(bufDir, DirectoryName, nbItems);
       if(entry != -1)
	 TtaSetSelector (NumZoneDirOpenDoc,entry,NULL);
       strcpy (docName, DirectoryName);
       length = strlen (docName);
       docName[length] = URL_DIR_SEP;
       docName[length + 1] = EOS;
       strcpy (DefaultDocumentName, docName);
     }
   /* liste des fichiers existants */
   TtaListDirectory (DirectoryName, NumFormOpenDoc, NULL, -1, ".PIV", TtaGetMessage (LIB, TMSG_FILES), NumSelDoc);
   /* zone de saisie du nom du document a ouvrir */
   TtaNewTextForm (NumZoneDocNameToOpen, NumFormOpenDoc,
		   TtaGetMessage (LIB, TMSG_DOCUMENT_NAME), 50, 1, TRUE);
   TtaSetTextForm (NumZoneDocNameToOpen, DefaultDocumentName);

  
   TtaSetDialoguePosition ();
   TtaShowDialogue (NumFormOpenDoc, TRUE);
}








