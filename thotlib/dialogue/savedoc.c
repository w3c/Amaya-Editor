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

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "edit_tv.h"
#include "appdialogue_tv.h"
#include "platform_tv.h"
#include "boxes_tv.h"
#include "frame_tv.h"

#include "actions_f.h"
#include "applicationapi_f.h"
#include "browser_f.h"
#include "callback_f.h"
#include "config_f.h"
#include "createdoc_f.h"
#include "documentapi_f.h"
#include "docs_f.h"
#include "fileaccess_f.h"
#include "font_f.h"
#include "opendoc_f.h"
#include "platform_f.h"
#include "translation_f.h"
#include "viewapi_f.h"
#include "writedoc_f.h"

extern int          CurrentDialog;
CHAR_T                DefaultFileSuffix[5];        
static PathBuffer   SaveDirectoryName;
static PathBuffer   SaveFileName;
static PathBuffer   TraductionSchemaName;
static ThotBool     SaveDocWithCopy;
static ThotBool     SaveDocWithMove;
static PtrDocument  DocumentToSave;
static int          PivotEntryNum;

#ifndef _WINDOWS
/*----------------------------------------------------------------------
   UnsetEntryMenu
   displays as non active the "ent" entry of the menu referenced by
   "ref".
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                UnsetEntryMenu (int ref, int ent)
#else  /* __STDC__ */
void                UnsetEntryMenu (ref, ent)
int                 ref;
int                 ent;

#endif /* __STDC__ */
{
   CHAR_T                fontname[100];
   CHAR_T                text[20];

   if (TtWDepth > 1)
      TtaRedrawMenuEntry (ref, ent, NULL, InactiveB_Color, 0);
   else
     {
	FontIdentifier (TEXT('L'), TEXT('T'), 2, 11, 1, text, fontname);
	TtaRedrawMenuEntry (ref, ent, fontname, -1, 0);
     }
}				/*UnsetEntryMenu */
#endif /* !_WINDOWS */

/*----------------------------------------------------------------------
   BuildSaveDocMenu
   does the File Save.
  ----------------------------------------------------------------------*/
void                BuildSaveDocMenu ()

{
   PathBuffer          outputFileName;
   int                 i;
   NotifyDialog        notifyDoc;

   /* Name du fichier a sauver */
   ustrcpy (outputFileName, SaveDirectoryName);
   ustrcat (outputFileName, DIR_STR);
   ustrcat (outputFileName, SaveFileName);
   if (TraductionSchemaName[0] == EOS)
      /* sauver en format Thot */
     {
       if (ThotLocalActions[T_setwritedirectory] != NULL &&
	   ThotLocalActions[T_writedocument] != NULL)
	 {
	   if (DocumentToSave->DocPivotVersion == -1)
	     {
	       (*ThotLocalActions [T_setwritedirectory]) (DocumentToSave,
							  SaveFileName, 
							  SaveDirectoryName, 
							  SaveDocWithCopy, 
							SaveDocWithMove);
	       (*ThotLocalActions[T_writedocument]) (DocumentToSave, 0);
	     }
	   else
	     {
	       ustrcat (outputFileName, ".PIV"); 
	       (void) StoreDocument (DocumentToSave,
				     SaveFileName, SaveDirectoryName,
				     SaveDocWithCopy, SaveDocWithMove);
	     }
	 }
     }
   else if (!ustrcmp (TraductionSchemaName, TEXT("_ThotOther_")))
     {
       if (DocumentToSave->DocPivotVersion == -1)
         {
	   ustrcat (outputFileName, ".PIV"); 
	   (void) StoreDocument (DocumentToSave,
				 SaveFileName, SaveDirectoryName,
				 SaveDocWithCopy, SaveDocWithMove);
	 }
       else if (ThotLocalActions[T_xmlparsedoc] != NULL)
	 {
	   (*ThotLocalActions [T_setwritedirectory]) (DocumentToSave,
						      SaveFileName, 
						      SaveDirectoryName, 
						      SaveDocWithCopy, 
						      SaveDocWithMove);
	   (*ThotLocalActions[T_writedocument]) (DocumentToSave, 0);
	 } 
     }
   else
      /* exporter le document */
     {
	/* envoie le message DocExport.Pre a l'application */
	notifyDoc.event = TteDocExport;
	notifyDoc.document = (Document) IdentDocument (DocumentToSave);
	notifyDoc.view = 0;
	if (!CallEventType ((NotifyEvent *) & notifyDoc, TRUE))
	   /* l'application accepte que Thot exporte le document */
	  {
	     TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_EXPORTING), DocumentToSave->DocDName);
	     FindCompleteName (SaveFileName, _EMPTYSTR_, SaveDirectoryName, outputFileName, &i);
	     ExportDocument (DocumentToSave, outputFileName, TraductionSchemaName, FALSE);
	     TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_LIB_DOC_WRITTEN), outputFileName);
	     /* envoie le message DocExport.Post a l'application */
	     notifyDoc.event = TteDocExport;
	     notifyDoc.document = (Document) IdentDocument (DocumentToSave);
	     notifyDoc.view = 0;
	     CallEventType ((NotifyEvent *) & notifyDoc, FALSE);
	  }
     }
}

#ifndef _WINDOWS
/*----------------------------------------------------------------------
   CallbackSaveDocMenu
   callback handler for the Save File menu.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CallbackSaveDocMenu (int ref, int typedata, STRING txt)

#else  /* __STDC__ */
void                CallbackSaveDocMenu (ref, typedata, txt)
int                 ref;
int                 typedata;
STRING              txt;

#endif /* __STDC__ */

{
   PathBuffer          ptTranslatedName;
   PathBuffer          BufDir;
   int                 i, nbitem;
   int                 val;
   CHAR_T                URL_DIR_SEP;

   if (typedata == STRING_DATA && txt && ustrchr (txt, TEXT('/')))
	  URL_DIR_SEP = TEXT('/');
   else 
	   URL_DIR_SEP = DIR_SEP;

   val = (int) txt;
   switch (ref)
	 {
	    case NumZoneDocNameTooSave:
	       /* zone de saisie du nom du document a creer */
	       if (TtaCheckDirectory (txt) && txt[ustrlen (txt) - 1] != URL_DIR_SEP)
		 {
		    ustrcpy (SaveDirectoryName, txt);
		    SaveFileName[0] = EOS;
		 }
	       else
		 {
		    /* conserve le nom du document a sauver */
		    TtaExtractName (txt, SaveDirectoryName, BufDir);
		    /* Remove le suffixe du nom de fichier */
		    i = ustrlen (BufDir) - 4;
		    if (!ustrcmp (&BufDir[i], DefaultFileSuffix))
		       BufDir[i] = EOS;
		    else
		       i += 4;
		    if (i >= MAX_NAME_LENGTH - 1)
		      {
			 i = MAX_NAME_LENGTH - 1;	/*Longueur du nom limitee */
			 BufDir[i] = EOS;
			 ustrcpy (ptTranslatedName, SaveDirectoryName);
			 ustrcat (ptTranslatedName, DIR_STR);
			 ustrcat (ptTranslatedName, BufDir);
			 if (TraductionSchemaName[0] == EOS)
			   ustrcat (ptTranslatedName, DefaultFileSuffix);
			 /* reinitialise la zone du nom de document */
#            ifndef _WINDOWS
			 TtaSetTextForm (NumZoneDocNameTooSave, ptTranslatedName);
#             endif /* !_WINDOWS */
		      }
		    ustrcpy (SaveFileName, BufDir);
		 }

	       if (TtaCheckDirectory (SaveDirectoryName))
		 {
		    /* Est-ce un nouveau directory qui contient des documents */
		    if (!TtaIsInDocumentPath (SaveDirectoryName))
		       if (TtaIsSuffixFileIn (SaveDirectoryName, DefaultFileSuffix))
			 {
			    /* il faut ajouter le directory au path */
			    i = ustrlen (DocumentPath);
			    if (i + ustrlen (SaveDirectoryName) + 2 < MAX_PATH)
			      {
				 ustrcat (DocumentPath, PATH_STR);
				 ustrcat (DocumentPath, SaveDirectoryName);
				 BuildPathDocBuffer (BufDir, EOS, &nbitem);
#                ifndef _WINDOWS
				 TtaNewSelector (NumZoneDirDocToSave, NumFormSaveAs, TtaGetMessage (LIB, TMSG_DOC_DIR), nbitem, BufDir, 6, NULL, FALSE, TRUE);
#                endif /* _WINDOWS */
			      }
			 }
		 }

	       break;
	    case NumZoneDirDocToSave:
	       /* zone de saisie du directory ou le document doit etre cree */
	       ustrcpy (SaveDirectoryName, txt);
	       ustrcpy (ptTranslatedName, SaveDirectoryName);
	       ustrcat (ptTranslatedName, DIR_STR);
	       ustrcat (ptTranslatedName, SaveFileName);
	       if (TraductionSchemaName[0] == EOS)
		  ustrcat (ptTranslatedName, DefaultFileSuffix);
	       /* reinitialise la zone du nom de document */
	       TtaSetTextForm (NumZoneDocNameTooSave, ptTranslatedName);
	       break;
	    case NumMenuFormatDocToSave:
	       /* sous-menu pour le choix du format de sauvegarde */
	       ustrcpy (ptTranslatedName, SaveDirectoryName);
	       ustrcat (ptTranslatedName, DIR_STR);
	       ustrcat (ptTranslatedName, SaveFileName);
	       if (val == 0)
		 {
		    /* premiere entree du menu format: format Thot */
		    TraductionSchemaName[0] = EOS;
		    ustrcat (ptTranslatedName, DefaultFileSuffix);
		    TtaRedrawMenuEntry (NumMenuCopyOrRename, 0, NULL, -1, 1);
		    TtaRedrawMenuEntry (NumMenuCopyOrRename, 1, NULL, -1, 1);
		 }
	       else if (val == PivotEntryNum)
		 {
		   ustrcpy (TraductionSchemaName, TEXT("_ThotOther_"));
		   UnsetEntryMenu (NumMenuCopyOrRename, 0);
		   UnsetEntryMenu (NumMenuCopyOrRename, 1);
		 }
	       else
		 {
		    ConfigGetExportSchemaName (val, TraductionSchemaName);
		    UnsetEntryMenu (NumMenuCopyOrRename, 0);
		    UnsetEntryMenu (NumMenuCopyOrRename, 1);
		 }
	       /* reinitialise la zone du nom de document */
	       TtaSetTextForm (NumZoneDocNameTooSave, ptTranslatedName);
	       break;
	    case NumMenuCopyOrRename:
	       /* sous-menu copier/renommer un document */
	       if (val == 0)
		  /* c'est un copy */
		 {
		    SaveDocWithCopy = TRUE;
		    SaveDocWithMove = FALSE;
		 }
	       else
		  /* c'est un move */
		 {
		    SaveDocWithCopy = FALSE;
		    SaveDocWithMove = TRUE;
		 }
	       break;
	    case NumFormSaveAs:
	       /* le formulaire Sauver Comme */
	       /* fait disparaitre la feuille de dialogue */
	       if (val == 1)
		 {
		    /* c'est une confirmation */
		    TtaUnmapDialogue (NumFormSaveAs);
		    CurrentDialog = NumFormSaveAs;
		    if (DocumentToSave != NULL)
		       if (DocumentToSave->DocSSchema != NULL)
			  /* le document a sauver n'a pas ete ferme' entre temps */
			  if (!TtaCheckDirectory (SaveDirectoryName))
			    {	/* le repertoire est invalide : affiche un message et detruit les dialogues */
			       TtaDisplayMessage (CONFIRM, TtaGetMessage (LIB, TMSG_MISSING_DIR), SaveDirectoryName);
			       if (ThotLocalActions[T_confirmcreate] != NULL)
				  (*ThotLocalActions[T_confirmcreate])
				     (0, 1, (STRING) 1);
			    }
			  else if (!ustrcmp (SaveDirectoryName, DocumentToSave->DocDirectory)
				   && !ustrcmp (SaveFileName, DocumentToSave->DocDName)
				   && TraductionSchemaName[0] == EOS)
			    {	/* traite la confirmation */
			       if (ThotLocalActions[T_confirmcreate] != NULL)
				  (*ThotLocalActions[T_confirmcreate]) (NumFormConfirm, 1, (STRING) 1);
			    }
			  else
			    {
			       ustrcpy (ptTranslatedName, SaveDirectoryName);
			       ustrcat (ptTranslatedName, DIR_STR);
			       ustrcat (ptTranslatedName, SaveFileName);
			       if (TraductionSchemaName[0] == EOS)
				 ustrcat (ptTranslatedName, DefaultFileSuffix);
			       if (TtaFileExist (ptTranslatedName))
				 {
				    /* demande confirmation */
				    usprintf (BufDir, TtaGetMessage (LIB, TMSG_FILE_EXIST), ptTranslatedName);
#                   ifndef _WINDOWS
				    TtaNewLabel (NumLabelConfirm, NumFormConfirm, BufDir);
#                   endif /* !_WINDOWS */
				    TtaShowDialogue (NumFormConfirm, FALSE);
				 }

			       else
				  /* traite la confirmation */
			       if (ThotLocalActions[T_confirmcreate] != NULL)
				  (*ThotLocalActions[T_confirmcreate]) (NumFormConfirm, 1, (STRING) 1);
			    }
		 }
	       else if (ThotLocalActions[T_confirmcreate] != NULL)
		  (*ThotLocalActions[T_confirmcreate]) (0, 1, (STRING) 1);
	       break;
	 }
}
#endif /* !_WINDOWS */

/*----------------------------------------------------------------------
   SaveDocAs
   creates, initializes and activates the "Save As" form for document
   pDoc.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         SaveDocAs (PtrDocument pDoc)
#else  /* __STDC__ */
static void         SaveDocAs (pDoc)
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   int                 nbitem;
   CHAR_T                BufMenu[MAX_TXT_LEN];
   CHAR_T                BufMenuB[MAX_TXT_LEN];
   CHAR_T                BufDir[MAX_PATH];
   STRING              src;
   STRING              dest;
   int                 i, k, l, Indx,entry;

   if (pDoc != NULL)
      if (!pDoc->DocReadOnly)
	 if (pDoc->DocSSchema != NULL)
	   {
	      DocumentToSave = pDoc;
#         ifndef _WINDOWS
	      /* cree le formaulaire Sauver comme */
	      TtaNewSheet (NumFormSaveAs, 0, TtaGetMessage (LIB, TMSG_SAVE_AS), 1, TtaGetMessage (LIB, TMSG_SAVE), TRUE, 3, 'L', D_CANCEL);
#         endif /* !_WINDOWS */

	      /* cree et */
	      /* initialise le selecteur sur aucune entree */
	      BuildPathDocBuffer (BufDir, EOS, &nbitem);
#         ifndef _WINDOWS
	      TtaNewSelector (NumZoneDirDocToSave, NumFormSaveAs, TtaGetMessage (LIB, TMSG_DOC_DIR), nbitem, BufDir, 6, NULL, FALSE, TRUE);
#         endif /* _WINDOWS */
	      entry = SearchStringInBuffer(BufDir,pDoc->DocDirectory,nbitem);
#         ifndef _WINDOWS
	      TtaSetSelector (NumZoneDirDocToSave, entry, _EMPTYSTR_);
#         endif /* !_WINDOWS */
	      /* initialise le titre du formulaire Sauver Comme */
	      ustrcpy (SaveFileName, pDoc->DocDName);
	      ustrcpy (SaveDirectoryName, pDoc->DocDirectory);
	      /* compose le menu des formats de sauvegarde applicables */
	      /* a ce document, d'apres sa classe */
	      nbitem = ConfigMakeMenuExport (pDoc->DocSSchema->SsName, BufMenu);
	      /* met le format Thot en tete */
	      BufMenuB[0] = TEXT('B');
	      ustrcpy (&BufMenuB[1], TtaGetMessage (LIB, TMSG_THOT_APP));
	      l = ustrlen (TtaGetMessage (LIB, TMSG_THOT_APP)) + 2;
	      /* ajoute 'B' au debut de chaque entree */
	      dest = &BufMenuB[l];
	      src = &BufMenu[0];
	      for (k = 1; k <= nbitem; k++)
		{
		   ustrcpy (dest, TEXT("B"));
		   dest++;
		   l = ustrlen (src);
		   ustrcpy (dest, src);
		   dest += l + 1;
		   src += l + 1;
		}
	      nbitem++;
	      if (pDoc->DocPivotVersion == -1)
		{
		  ustrcpy (dest, TEXT("B"));
		  dest++;
		  ustrcpy (dest, TEXT("Pivot"));
		  PivotEntryNum = nbitem;
		  nbitem++;
		}
	      else if (ThotLocalActions[T_xmlparsedoc] != NULL)
		/* XML extensions are loaded add xml item */
		{
		  ustrcpy (dest, TEXT("B"));
		  dest++;
		  ustrcpy (dest, XML_EXT2);
		  PivotEntryNum = nbitem;
		  nbitem++;
		}
	      TtaNewSubmenu (NumMenuFormatDocToSave, NumFormSaveAs, 0,
			     TtaGetMessage (LIB, TMSG_DOC_FORMAT), nbitem, BufMenuB, NULL, TRUE);
	      TtaSetMenuForm (NumMenuFormatDocToSave, 0);
	      /* sous-menu copier/renommer un document */
	      Indx = 0;
	      usprintf (&BufMenu[Indx], TEXT("B%s"), TtaGetMessage (LIB, TMSG_COPY));
	      Indx += ustrlen (&BufMenu[Indx]) + 1;
	      usprintf (&BufMenu[Indx], TEXT("B%s"), TtaGetMessage (LIB, TMSG_RENAME));
	      TtaNewSubmenu (NumMenuCopyOrRename, NumFormSaveAs, 0,
		   TtaGetMessage (LIB, TMSG_SAVE), 2, BufMenu, NULL, FALSE);
	      TtaSetMenuForm (NumMenuCopyOrRename, 0);
	      /* initialise le  nom de document propose */
	      ustrcpy (BufMenu, TtaGetMessage (LIB, TMSG_SAVE));
	      ustrcat (BufMenu, TEXT(" "));
	      ustrcat (BufMenu, pDoc->DocDName);
	      TtaChangeFormTitle (NumFormSaveAs, BufMenu);
	      ustrcpy (BufMenu, SaveDirectoryName);
	      ustrcat (BufMenu, DIR_STR);
	      ustrcat (BufMenu, SaveFileName);
	      if (pDoc->DocPivotVersion == -1)
		ustrcat (BufMenu, XML_EXT);
	      else
		ustrcat (BufMenu, ".PIV");
	      /* nom de document propose' */
#         ifndef _WINDOWS
	      TtaNewTextForm (NumZoneDocNameTooSave, NumFormSaveAs, TtaGetMessage (LIB, TMSG_DOCUMENT_NAME), 50, 1, TRUE);
#         endif /* !_WINDOWS */
	      TtaSetTextForm (NumZoneDocNameTooSave, BufMenu);

/*        ActiveEntree(NumMenuCopyOrRename, 0); */
/*        ActiveEntree(NumMenuCopyOrRename, 1); */
/*        TtaSetMenuForm(NumMenuCopyOrRename, 0); */
	      /* premiere entree du menu format: format Thot */
	      TraductionSchemaName[0] = EOS;
	      SaveDocWithCopy = TRUE;
	      SaveDocWithMove = FALSE;
	      /* Formulaire Confirmation creation */
	      ustrcpy (BufMenu, TtaGetMessage (LIB, TMSG_SAVE_AS));
	      i = ustrlen (BufMenu) + 1;
	      ustrcpy (&BufMenu[i], TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
#         ifndef _WINDOWS
	      TtaNewDialogSheet (NumFormConfirm,  0, NULL, 2, BufMenu, FALSE, 1, 'L');
#         endif /* !_WINDOWS */

	      /* affiche le formulaire */
	      TtaShowDialogue (NumFormSaveAs, FALSE);
	   }
}

#ifndef _WINDOWS
/*----------------------------------------------------------------------
  TtcSaveDocumentAs
  standard handler for a SaveDocumentAs action.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcSaveDocumentAs (Document document, View view)
#else  /* __STDC__ */
void                TtcSaveDocumentAs (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   PtrDocument         pDoc;
   int                 frame;

   if (document != 0)
     {
	pDoc = LoadedDocument[document - 1];
	if (pDoc != NULL)
	   /* il y a un document pour cette entree de la table */
	  {
	     if (ThotLocalActions[T_savedoc] == NULL)
	       {
		  TteConnectAction (T_savedoc, (Proc) BuildSaveDocMenu);
		  TteConnectAction (T_confirmcreate, (Proc) CallbackConfirmMenu);
		  TteConnectAction (T_rsavedoc, (Proc) CallbackSaveDocMenu);
		  TteConnectAction (T_buildpathdocbuffer, (Proc) BuildPathDocBuffer);
	       }
	     frame = GetWindowNumber (document, view);
	     if (ThotLocalActions[T_updateparagraph] != NULL)
		(*ThotLocalActions[T_updateparagraph]) (ViewFrameTable[frame - 1].FrAbstractBox, frame);
	     SaveDocAs (pDoc);
	  }
     }
}
#endif /* !_WINDOWS */

/*----------------------------------------------------------------------
  TtcSaveDocument
  standard handler for a SaveDocument action.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcSaveDocument (Document document, View view)
#else  /* __STDC__ */
void                TtcSaveDocument (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   PtrDocument         pDoc;
   int                 frame;

   if (document != 0)
     {
	pDoc = LoadedDocument[document - 1];
	if (pDoc != NULL)
	   /* il y a un document pour cette entree de la table */
	  {
	     frame = GetWindowNumber (document, view);
	     if (ThotLocalActions[T_updateparagraph] != NULL)
		(*ThotLocalActions[T_updateparagraph]) (ViewFrameTable[frame - 1].FrAbstractBox, frame);
	    SetDocumentModified (pDoc, !(* (Func)ThotLocalActions[T_writedocument]) (pDoc, 4), 0);
	  }
     }
}










