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
#include "appstruct.h"
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
STRING              strstr (s1, s2)
STRING              s1, s2;
{
   STRING              tmp;
   int                 i, length;
   ThotBool            continue;

   tmp = s1;
   length = ustrlen (s2);
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
static int          NbDocSuffix = 1;
static CHAR_T       tabDocSuffix [10][10] = {".PIV", _EMPTYSTR_, _EMPTYSTR_, _EMPTYSTR_, _EMPTYSTR_, _EMPTYSTR_, _EMPTYSTR_, _EMPTYSTR_, _EMPTYSTR_, _EMPTYSTR_};
static CHAR_T       docSuffix [5];
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
void                BuildPathDocBuffer (STRING bufDir, CHAR_T separator, int *nbItems)
#else  /* __STDC__ */
void                BuildPathDocBuffer (bufDir, separator, nbItems)
STRING              bufDir;
CHAR_T              separator;
int                *nbItems;

#endif /* __STDC__ */
{
   int                 i, nb;

   ustrncpy (bufDir, DocumentPath, MAX_PATH);

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
int SearchStringInBuffer(char* buffer, PathBuffer s,int nbStr)
#else  /* __STDC__ */
int  SearchStringInBuffer(buffer, s, nbStr)
char*      buffer;
PathBuffer s;
int        nbStr;
#endif /* __STDC__ */
{
  int occ;
  STRING pBuf;
  ThotBool found = FALSE;

  occ=0;
  pBuf = buffer;
  while (!found && occ < nbStr)
    {
      if(!ustrcmp(pBuf,s))
	found = TRUE;
      else
	{
	  pBuf = pBuf + ustrlen(pBuf) + 1;
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
#  ifndef _WINDOWS
   /* Formulaire du schema de presentation */
   TtaNewForm (NumFormPresentationSchema, 0, TtaGetMessage (LIB, TMSG_PRES), TRUE, 1, 'L', D_DONE);

   /* zone de saisie du nom du schema de presentation */
   TtaNewTextForm (NumZonePresentationSchema, NumFormPresentationSchema, TtaGetMessage (LIB, TMSG_PRES), 30, 1, FALSE);
#  endif /* !_WINDOWS */
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
   ustrncpy (name, NewSchemaName, MAX_NAME_LENGTH);
   /* efface le message */

   /* detruit le formulaire */
   TtaDestroyDialogue (NumFormPresentationSchema);
}


/*----------------------------------------------------------------------
   CallbackSchPresNameMenu
   updates the presentation scheme choice.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CallbackSchPresNameMenu (int ref, int typedata, STRING data)

#else  /* __STDC__ */
void                CallbackSchPresNameMenu (ref, typedata, data)
int                 ref;
int                 typedata;
STRING              data;

#endif /* __STDC__ */
{
   switch (ref)
	 {
	    case NumFormPresentationSchema:
	       /* formulaire demandant le nom du schema de presentation */

	       break;
	    case NumZonePresentationSchema:
	       /* zone de saisie du nom du schema de presentation */
	       ustrncpy (NewSchemaName, data, MAX_NAME_LENGTH);
	       break;
	 }
}

/*----------------------------------------------------------------------
BuildImportForm : cree Le formulaire d'importation de documents.
  ----------------------------------------------------------------------*/
static void BuildImportForm()
{
   int nbItems,length;
   CHAR_T bufMenu[MAX_TXT_LEN];
 
#  ifndef _WINDOWS
   /* Formulaire Classe du document a importer */
   TtaNewForm (NumFormImportClass, 0, TtaGetMessage (LIB, TMSG_IMPORT_DOC_TYPE), TRUE, 1, 'L', D_DONE);
   /* selecteur ou zone de saisie Classe du document a importer */
#  endif /* !_WINDOWS */
   nbItems = ConfigMakeImportMenu (bufMenu);
   if (nbItems == 0)
      /* pas d'import defini dans le fichier de langue, */
      /* on cree une simple zone de saisie de texte */
#     ifndef _WINDOWS
      TtaNewTextForm (NumSelectImportClass, NumFormImportClass, TtaGetMessage (LIB, TMSG_IMPORT_DOC_TYPE), 30, 1, FALSE)
#     endif /* !_WINDOWS */
	  ;
   else {
        /* on cree un selecteur */
        if (nbItems >= 6)
           length = 6;
        else
             length = nbItems;
#       ifndef _WINDOWS
	    TtaNewSelector (NumSelectImportClass, NumFormImportClass, TtaGetMessage (LIB, TMSG_IMPORT_DOC_TYPE), nbItems, bufMenu, length, NULL, TRUE, FALSE);
	    /* initialise le selecteur sur sa premiere entree */
	    TtaSetSelector (NumSelectImportClass, 0, _EMPTYSTR_);
#       endif /* !_WINDOWS */
   } 
}

/*----------------------------------------------------------------------
   CallbackImportMenu
   updates the ImportMenu form.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CallbackImportMenu (int ref, int typedata, STRING data)

#else  /* __STDC__ */
void                CallbackImportMenu (ref, typedata, data)
int                 ref;
int                 typedata;
STRING              data;

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
		    ustrncpy (SchStrImport, data, MAX_NAME_LENGTH - 1);
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
void                CallbackOpenDocMenu (int ref, int typedata, STRING data)

#else  /* __STDC__ */
void                CallbackOpenDocMenu (ref, typedata, data)
int                 ref;
int                 typedata;
STRING              data;

#endif /* __STDC__ */
{
   int                 val;
   PathBuffer          docName;
   PtrDocument         pDoc;
   int                 i;
   CHAR_T              bufDir[MAX_PATH];
   CHAR_T              URL_DIR_SEP;

   if (typedata == STRING_DATA && data && ustrchr (data, TEXT('/')))
     URL_DIR_SEP = TEXT('/');
   else 
     URL_DIR_SEP = DIR_SEP;

   val = (int) data;
   switch (ref)
	 {
	    case NumToggleDocTypeToOpen:
               {
                  ustrcpy (docSuffix, tabDocSuffix[(int)data]);
		  if (TtaCheckDirectory (DirectoryName))
                     TtaListDirectory (DirectoryName, NumFormOpenDoc, NULL, -1,
                                       docSuffix, TtaGetMessage (LIB, TMSG_FILES), NumSelDoc);
                }
		break;

	    case NumZoneDocNameToOpen:
	       if (TtaCheckDirectory (data) && data[ustrlen (data) - 1] != URL_DIR_SEP)
		 {
		    ustrcpy (DirectoryName, data);
		    DefaultDocumentName[0] = EOS;
		 }
	       else
		 {
		    /* conserve le nom du document a ouvrir */
		    TtaExtractName (data, DirectoryName, docName);
		    i = ustrlen (docName);
		    if (i >= MAX_NAME_LENGTH)
		      {
			 i = MAX_NAME_LENGTH;	/*Longueur du nom limitee */
			 docName[i] = EOS;
		      }
		    ustrcpy (DefaultDocumentName, docName);
		 }

	       if (TtaCheckDirectory (DirectoryName))
		 {
		    /* Est-ce un nouveau directory qui contient des documents */
		    if (!TtaIsInDocumentPath (DirectoryName))
		       if (TtaIsSuffixFileIn (DirectoryName, docSuffix))
			 {
			    /* il faut ajouter le directory au path */
			    i = ustrlen (DocumentPath);
			    if (i + ustrlen (DirectoryName) + 2 < MAX_PATH)
			      {
				 ustrcat (DocumentPath, PATH_STR);
				 ustrcat (DocumentPath, DirectoryName);
				 BuildPathDocBuffer (bufDir, EOS, &i);
#                ifndef _WINDOWS
				 TtaNewSelector (NumZoneDirOpenDoc, NumFormOpenDoc, TtaGetMessage (LIB, TMSG_DOC_DIR), i, bufDir, 9, NULL, FALSE, TRUE);
#                endif /* !_WINDOWS */

				 TtaListDirectory (DirectoryName, NumFormOpenDoc, NULL, -1,
						   docSuffix, TtaGetMessage (LIB, TMSG_FILES), NumSelDoc);
			      }
			 }
		 }
	       break;
	    case NumZoneDirOpenDoc:
	       ustrcpy (DirectoryName, data);
	       TtaSetTextForm (NumZoneDocNameToOpen, DirectoryName);
	       TtaListDirectory (data, NumFormOpenDoc, NULL, -1,
			docSuffix, TtaGetMessage (LIB, TMSG_FILES), NumSelDoc);
	       break;
	    case NumSelDoc:
	       if (DirectoryName[0] == EOS)
		 {
		    /* compose le path complet du fichier pivot */
		    ustrncpy (DirectoryName, DocumentPath, MAX_PATH);
		    MakeCompleteName (docName, &docSuffix[1], DirectoryName, data, &i);
		    TtaExtractName (docName, DirectoryName, DefaultDocumentName);
		 }
	       else
		 {
		    ustrcpy (docName, DirectoryName);
		    ustrcat (docName, DIR_STR);
		    i = ustrlen (data);
		    if (i >= MAX_NAME_LENGTH)
		      {
			 /* RemoveElement le suffixe du nom de fichier */
			 if (!ustrcmp (&data[i - 4], docSuffix))
			    data[i - 4] = EOS;
		      }
		    ustrncpy (DefaultDocumentName, data, MAX_NAME_LENGTH);
		    DefaultDocumentName[MAX_NAME_LENGTH - 1] = EOS;
		    ustrcat (docName, DefaultDocumentName);
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
		  ustrncpy (DirectoryName, DocumentPath, MAX_PATH);
	       else if (TtaCheckDirectory (DirectoryName))
		  /* Est-ce un nouveau directory de documents */
		  if (!TtaIsInDocumentPath (DirectoryName))
		    {
		       /* il faut ajouter le directory au path */
		       i = ustrlen (DocumentPath);
		       if (i + ustrlen (DirectoryName) + 2 < MAX_PATH)
			 {
			    ustrcat (DocumentPath, PATH_STR);
			    ustrcat (DocumentPath, DirectoryName);
			 }
		    }

	       MakeCompleteName (DefaultDocumentName, &docSuffix[1], DirectoryName, docName, &i);
	       /* teste si le fichier 'suffix' existe */
	       if (TtaFileExist (docName) != 0)
		  /* le fichier existe, on ouvre le document */
		 {
		   /* charge le document */
		   if ((!ustrcmp (docSuffix, XML_EXT)) && 
		       ThotLocalActions[T_xmlparsedoc] != NULL)
                      LoadXmlDocument (&pDoc, docName);
                   else
		      LoadDocument (&pDoc, docName);
		   if (pDoc != NULL)
		     ustrcpy (pDoc->DocDirectory, DirectoryName);
		 }
	       else
		  /* Le fichier n'existe pas */
		 {
		    /* cherche s'il existe un fichier de ce nom, sans extension */
		    ustrncpy (DirectoryName, DocumentPath, MAX_PATH);
		    MakeCompleteName (DefaultDocumentName, _EMPTYSTR_, DirectoryName, docName, &i);
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
   CHAR_T                bufDir[MAX_PATH];
   PathBuffer          docName;
   int                 length, nbItems, entry;
   CHAR_T                URL_DIR_SEP;
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
   if (ThotLocalActions[T_xmlparsedoc] != NULL && NbDocSuffix <= 1)
    {
      ustrcpy (tabDocSuffix[NbDocSuffix++], XML_EXT);
    }

   /* Creation du Formulaire Ouvrir */
   parentWidget = TtaGetViewFrame (document, view);
#  ifndef _WINDOWS
   TtaNewForm (NumFormOpenDoc, parentWidget, TtaGetMessage (LIB, TMSG_OPEN_DOC), TRUE, 2, 'L', D_CANCEL);
#  endif /* !_WINDOWS */
   /* zone de saisie des dossiers documents */
   BuildPathDocBuffer (bufDir, EOS, &nbItems);
#  ifndef _WINDOWS
   TtaNewSelector (NumZoneDirOpenDoc, NumFormOpenDoc, TtaGetMessage (LIB, TMSG_DOC_DIR), nbItems, bufDir, 6, NULL, FALSE, TRUE);
#  endif /* !_WINDOWS */
   if (DirectoryName[0] == EOS && nbItems >= 1)
      /* si pas de dossier courant, on initialise avec le premier de bufDir */
     {
	ustrcpy (DirectoryName, bufDir);
	ustrcpy (DefaultDocumentName, bufDir);
#   ifndef _WINDOWS
	TtaSetSelector (NumZoneDirOpenDoc, 0, NULL);
#   endif /* !_WINDOWS */
     }
   else if (DirectoryName[0] != EOS)
     {
       if (ustrchr (DirectoryName, TEXT('/')))
	 URL_DIR_SEP = TEXT('/');
       else 
	 URL_DIR_SEP = DIR_SEP;

       entry = SearchStringInBuffer(bufDir, DirectoryName, nbItems);
#      ifndef _WINDOWS
       if (entry != -1)
          TtaSetSelector (NumZoneDirOpenDoc, entry, NULL);
#      endif /* !_WINDOWS */
       ustrcpy (docName, DirectoryName);
       length = ustrlen (docName);
       docName[length] = URL_DIR_SEP;
       docName[length + 1] = EOS;
       ustrcpy (DefaultDocumentName, docName);
     }
   /* liste des fichiers existants */
   ustrcpy (docSuffix, tabDocSuffix[0]);
   TtaListDirectory (DirectoryName, NumFormOpenDoc, NULL, -1, docSuffix,
                     TtaGetMessage (LIB, TMSG_FILES), NumSelDoc);

   /* zone de saisie du nom du document a ouvrir */
   if (NbDocSuffix > 1)
      length = 30;
   else
      length = 50;
#  ifndef _WINDOWS
   TtaNewTextForm (NumZoneDocNameToOpen, NumFormOpenDoc, TtaGetMessage (LIB, TMSG_DOCUMENT_NAME), length, 1, TRUE);
#  endif /* !_WINDOWS */
   TtaSetTextForm (NumZoneDocNameToOpen, DefaultDocumentName);

   if (NbDocSuffix > 1)
     {
   	/* select menu du type de fichier a ouvrir */
   	length = 0;
   	for (entry=0; entry<NbDocSuffix; entry++)
     	  {
            usprintf (&bufDir[length], TEXT("T%s"), tabDocSuffix[entry]+1);
            length += ustrlen (&bufDir[length])+1;
          }
        TtaNewSubmenu (NumToggleDocTypeToOpen, NumFormOpenDoc, 0, TtaGetMessage (LIB, TMSG_DOCUMENT_FORMAT),
                          NbDocSuffix, bufDir, NULL, TRUE);
        TtaSetMenuForm (NumToggleDocTypeToOpen, 0);
      }
#  ifndef _WINDOWS
   TtaSetDialoguePosition ();
#  endif /* !_WINDOWS */
   TtaShowDialogue (NumFormOpenDoc, TRUE);
}








