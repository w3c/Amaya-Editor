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

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmenu.h"
#include "libmsg.h"
#include "message.h"
#include "constmedia.h"
#include "typemedia.h"
#include "fileaccess.h"
#include "appdialogue.h"
#include "fileaccess.h"
#include "thotdir.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "platform_tv.h"
#include "print_tv.h"
#include "modif_tv.h"
#include "page_tv.h"
#include "select_tv.h"
#include "edit_tv.h"
#include "frame_tv.h"
#include "appdialogue_tv.h"

#include "absboxes_f.h"
#include "appli_f.h"
#include "appdialogue_f.h"
#include "applicationapi_f.h"
#include "attributes_f.h"
#include "boxselection_f.h"
#include "buildboxes_f.h"
#include "callback_f.h"
#include "changeabsbox_f.h"
#include "config_f.h"
#include "createabsbox_f.h"
#include "createpages_f.h"
#include "docs_f.h"
#include "draw_f.h"
#include "fileaccess_f.h"
#include "memory_f.h"
#include "paginate_f.h"
#include "platform_f.h"
#include "presvariables_f.h"
#include "readpivot_f.h"
#include "references_f.h"
#include "search_f.h"
#include "searchref_f.h"
#include "structschema_f.h"
#include "structmodif_f.h"
#include "structcommands_f.h"
#include "structcreation_f.h"
#include "structselect_f.h"
#include "tree_f.h"
#include "views_f.h"
#include "viewapi_f.h"
#include "viewcommands_f.h"
#include "schemas_f.h"
#include "writepivot_f.h"

static PathBuffer   SaveDirectoryName;
static PathBuffer   SaveFileName;
static boolean      SaveDocWithCopy;
static boolean      SaveDocWithMove;
static PtrDocument  DocumentToSave;
extern CHAR_T         DefaultFileSuffix[5];        


/*----------------------------------------------------------------------
   simpleSave sauve un document sous forme pivot dans un fichier   
   dont le nom est donne par name, et ne fait rien d'autre.
   Rend false si l'ecriture n'a pu se faire.               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean      simpleSave (PtrDocument pDoc, STRING name, boolean withEvent)
#else  /* __STDC__ */
static boolean      simpleSave (pDoc, name, withEvent)
PtrDocument         pDoc;
STRING              name;
boolean             withEvent;
#endif /* __STDC__ */
{
   BinFile             pivotFile;
   NotifyDialog        notifyDoc;
   boolean             ok;

   if (!pDoc->DocReadOnly)
     {
	pivotFile = TtaWriteOpen (name);
	if (pivotFile == 0)
	   return FALSE;
	else
	  {
	     if (withEvent)
	       {
		  /* envoie l'evenement DocSave.Pre a l'application */
		  notifyDoc.event = TteDocSave;
		  notifyDoc.document = (Document) IdentDocument (pDoc);
		  notifyDoc.view = 0;
		  ok = !CallEventType ((NotifyEvent *) & notifyDoc, TRUE);
	       }
	     else
		ok = TRUE;
	     if (ok)
		/* l'application laisse Thot effectuer la sauvegarde */
	       {
		  /* ecrit le document dans ce fichier sous la forme pivot */
		  SauveDoc (pivotFile, pDoc);
		  TtaWriteClose (pivotFile);
		  if (withEvent)
		    {
		       /* envoie l'evenement DocSave.Post a l'application */
		       notifyDoc.event = TteDocSave;
		       notifyDoc.document = (Document) IdentDocument (pDoc);
		       notifyDoc.view = 0;
		       CallEventType ((NotifyEvent *) & notifyDoc, FALSE);
		    }
	       }
	     return TRUE;
	  }
     }
   return FALSE;
}


/*----------------------------------------------------------------------
   saveWithExtension sauve un document sous forme pivot en         
   concatenant l'extension au nom stocke' dans le document.
   Envoie un message et rend false si l'ecriture n'a pu se 
   faire.                                                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean      saveWithExtension (PtrDocument pDoc, STRING extension)
#else  /* __STDC__ */
static boolean      saveWithExtension (pDoc, extension)
PtrDocument         pDoc;
STRING              extension;

#endif /* __STDC__ */
{
   CHAR_T                buf[MAX_TXT_LEN];
   int                 i;

   if (pDoc == NULL)
      return FALSE;
   FindCompleteName (pDoc->DocDName, extension, pDoc->DocDirectory, buf, &i);
   if (simpleSave (pDoc, buf, FALSE))
     {
	UpdateAllInclusions (pDoc);
	return TRUE;
     }
   else
     {
	TtaDisplayMessage (CONFIRM, TtaGetMessage (LIB, TMSG_WRITING_IMP), buf);
	return FALSE;
     }
}

/*----------------------------------------------------------------------
   StoreDocument       sauve le document pDoc dans un fichier
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             StoreDocument (PtrDocument pDoc, Name docName, PathBuffer dirName, boolean copy, boolean move)
#else  /* __STDC__ */
boolean             StoreDocument (pDoc, docName, dirName, copy, move)
PtrDocument         pDoc;
Name                docName;
PathBuffer          dirName;
boolean             copy;
boolean             move;

#endif /* __STDC__ */
{
   PathBuffer          bakName, pivName, tempName, backName, oldDir;
   NotifyDialog        notifyDoc;
   CHAR_T                buf[MAX_TXT_LEN];
   int                 i;
   boolean             sameFile, status, ok;

   CloseInsertion ();
   notifyDoc.event = TteDocSave;
   notifyDoc.document = (Document) IdentDocument (pDoc);
   notifyDoc.view = 0;
   if (CallEventType ((NotifyEvent *) & notifyDoc, TRUE))
      /* l'application a pris la sauvegarde en charge */
      status = TRUE;
   else
     {
	status = TRUE;
	sameFile = TRUE;
	if (ustrcmp (docName, pDoc->DocDName) != 0)
	   sameFile = FALSE;
	if (ustrcmp (dirName, pDoc->DocDirectory) != 0)
	   sameFile = FALSE;

	/* construit le nom complet de l'ancien fichier de sauvegarde */
	FindCompleteName (pDoc->DocDName, "BAK", pDoc->DocDirectory, bakName, &i);
	ustrncpy (oldDir, pDoc->DocDirectory, MAX_PATH);
	/*     SECURITE:                                         */
	/*     on ecrit sur un fichier nomme' X.Tmp et non pas   */
	/*     directement X.PIV ...                             */
	/*     On fait ensuite des renommages                    */
	FindCompleteName (docName, "PIV", dirName, buf, &i);
	/* on teste d'abord le droit d'ecriture sur le .PIV */
	ok = FileWriteAccess (buf) == 0;
	if (ok)
	  {
	     FindCompleteName (docName, "Tmp", dirName, tempName, &i);
	     /* on teste le droit d'ecriture sur le .Tmp */
	     ok = FileWriteAccess (tempName) == 0;
	     if (ok)
	       {
		  TtaDisplaySimpleMessage (INFO, LIB, TMSG_WRITING);
		  ok = simpleSave (pDoc, tempName, FALSE);
	       }
	     if (ok)
		UpdateAllInclusions (pDoc);
	  }
	if (!ok)
	  {
	     /* on indique un nom connu de l'utilisateur... */
	     FindCompleteName (docName, "PIV", dirName, buf, &i);
	     TtaDisplayMessage (CONFIRM, TtaGetMessage (LIB, TMSG_WRITING_IMP),
				buf);
	     status = FALSE;
	  }
	else
	  {
	     /* 1- faire mv .PIV sur .OLD sauf si c'est une copie */
	     /* Le nom et le directory du document peuvent avoir change'. */
	     /* le fichier .OLD reste dans l'ancien directory, avec */
	     /* l'ancien nom */
	     FindCompleteName (pDoc->DocDName, "PIV", oldDir, pivName, &i);
	     if (!copy)
	       {
		  FindCompleteName (pDoc->DocDName, "OLD", oldDir, backName, &i);
		  i = urename (pivName, backName);
	       }
	     /* 2- faire mv du .Tmp sur le .PIV */
	     FindCompleteName (docName, "PIV", dirName, pivName, &i);
	     i = urename (tempName, pivName);
	     if (i >= 0)
		/* >> tout s'est bien passe' << */
		/* detruit l'ancienne sauvegarde */
	       {
		  TtaFileUnlink (bakName);
		  TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_LIB_DOC_WRITTEN),
				     pivName);
		  /* c'est trop tot pour perdre l'ancien nom du fichier et son */
		  /* directory d'origine. */
		  pDoc->DocModified = FALSE;
		  pDoc->DocNTypedChars = 0;

		  /* modifie les fichiers .EXT des documents nouvellement */
		  /* reference's ou qui ne sont plus reference's par */
		  /* notre document */
		  UpdateExt (pDoc);
		  /* modifie les fichiers .REF des documents qui */
		  /* referencent des elements qui ne sont plus dans notre */
		  /* document et met a jour le fichier .EXT de notre */
		  /* document */
		  UpdateRef (pDoc);
		  /* detruit le fichier .REF du document sauve' */
		  FindCompleteName (pDoc->DocDName, "REF", oldDir, buf, &i);
		  TtaFileUnlink (buf);
		  if (!sameFile)
		    {
		       if (ustrcmp (dirName, oldDir) != 0 &&
			   ustrcmp (docName, pDoc->DocDName) == 0)
			  /* changement de directory sans changement de nom */
			  if (move)
			    {
			       /* deplacer le fichier .EXT dans le nouveau directory */
			       FindCompleteName (pDoc->DocDName, "EXT", oldDir, buf, &i);
			       FindCompleteName (pDoc->DocDName, "EXT", dirName, pivName, &i);
			       urename (buf, pivName);
			       /* detruire l'ancien fichier PIV */
			       FindCompleteName (pDoc->DocDName, "PIV", oldDir, buf, &i);
			       TtaFileUnlink (buf);
			    }

		       if (ustrcmp (docName, pDoc->DocDName) != 0)
			 {
			    /* il y a effectivement changement de nom */
			    if (copy)
			       /* l'utilisateur veut creer une copie du document. */
			       /* on fait apparaitre le document copie dans les */
			       /* fichiers .EXT des documents reference's */
			       ChangeNomExt (pDoc, docName, TRUE);
			    if (move)
			      {
				 /* il s'agit d'un changement de nom du document */
				 /* change le nom du document dans les fichiers */
				 /* .EXT de tous les documents reference's */
				 ChangeNomExt (pDoc, docName, FALSE);
				 /* indique le changement de nom a tous les */
				 /* documents qui referencent ce document */
				 ChangeNomRef (pDoc, docName);
				 /* renomme le fichier .EXT du document qui change */
				 /* de nom */
				 FindCompleteName (pDoc->DocDName, "EXT", oldDir, buf,
						   &i);
				 FindCompleteName (docName, "EXT", dirName,
						   pivName, &i);
				 urename (buf, pivName);
				 /* detruit l'ancien fichier .PIV */
				 FindCompleteName (pDoc->DocDName, "PIV", oldDir, buf,
						   &i);
				 TtaFileUnlink (buf);
			      }
			 }
		       ustrncpy (pDoc->DocDName, docName, MAX_NAME_LENGTH);
		       ustrncpy (pDoc->DocIdent, docName, MAX_DOC_IDENT_LEN);
		       ustrncpy (pDoc->DocDirectory, dirName, MAX_PATH);
		       ChangeDocumentName (pDoc, docName);
		    }
	       }
	     notifyDoc.event = TteDocSave;
	     notifyDoc.document = (Document) IdentDocument (pDoc);
	     notifyDoc.view = 0;
	     CallEventType ((NotifyEvent *) & notifyDoc, FALSE);
	  }
     }
   return status;
}

/*----------------------------------------------------------------------
   interactiveSave sauve un document sous forme pivot avec evenements et
   info utilisateur
   Rend false si l'ecriture n'a pu se faire.                             
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static boolean      interactiveSave (PtrDocument pDoc)

#else  /* __STDC__ */
static boolean      interactiveSave (pDoc)
PtrDocument         pDoc;
#endif /* __STDC__ */

{
   boolean             status;

   status = FALSE;
   if (pDoc->DocReadOnly)
      /* on ne sauve pas les documents qui sont en lecture seule */
      TtaDisplaySimpleMessage (INFO, LIB, TMSG_RO_DOC_FORBIDDEN);
   else if (pDoc->DocSSchema == NULL)
      TtaDisplaySimpleMessage (INFO, LIB, TMSG_EMPTY_DOC_NOT_WRITTEN);
   else
     status = StoreDocument (pDoc, SaveFileName, SaveDirectoryName,SaveDocWithCopy, SaveDocWithMove);
    
   if (status)
     {
	pDoc->DocModified = FALSE;
	pDoc->DocNTypedChars = 0;
     }
   return status;
}

/*----------------------------------------------------------------------
  SetWriteDirectory sets the directory, filename, if the next write 
  is with copy and with move
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void              SetWriteDirectory (PtrDocument pDoc, PathBuffer fileName, PathBuffer directoryName, boolean withCopy, boolean withMove)
#else  /* __STDC__ */
void              SetWriteDirectory (pDoc, fileName, directoryName, withCopy, withMove)
PtrDocument    pDoc;
PathBuffer     fileName;
PathBuffer     directoryName;
boolean        withCopy;
boolean        withMove;
#endif /* __STDC__ */
{
  ustrcpy (SaveFileName, fileName);
  ustrcat (SaveFileName, ".PIV");
  ustrcpy (SaveDirectoryName, directoryName);
  SaveDocWithCopy = withCopy;
  SaveDocWithMove = withMove;
  DocumentToSave = pDoc;
}

/*----------------------------------------------------------------------
   WriteDocument sauve sous forme pivot le document pointe' par    
   pDoc. Retourne Vrai si le document a pu etre sauve,     
   Faux si echec.                                          
   - mode = 0 : demander le nom de fichier a` l'utilisateur
   - mode = 1 : fichier de sauvegarde automatique (.BAK)   
   - mode = 2 : fichier scratch (pas de message)           
   - mode = 3 : fichier de sauvegarde urgente (.SAV)       
   - mode = 4 : sauve sans demander de nom.                
   - mode = 5 : sauve sans demander de nom et sans message.                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             WriteDocument (PtrDocument pDoc, int mode)

#else  /* __STDC__ */
boolean             WriteDocument (pDoc, mode)
PtrDocument         pDoc;
int                 mode;

#endif /* __STDC__ */

{
   boolean             ok;

   ok = FALSE;
   if (pDoc != NULL)
     if (mode >= 0 && mode <= 5)
       switch (mode)
	 {
	 case 0:
	   if (DocumentToSave == pDoc)
	     ok = interactiveSave (pDoc);
	   break;
	 case 1:
	   ok = saveWithExtension (pDoc, "BAK");
	   if (ok)
	     TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_LIB_DOC_WRITTEN), pDoc->DocDName);
	   break;
	 case 2:
	   ok = saveWithExtension (pDoc, "BAK");
	   break;
	 case 3:
	   ok = saveWithExtension (pDoc, "SAV");
	   break;
	 case 4:
	   SetWriteDirectory (pDoc, pDoc->DocDName, pDoc->DocDirectory, FALSE, FALSE);
	   ok = interactiveSave (pDoc);
	   break;
	 case 5:
	   ok = saveWithExtension (pDoc, "PIV");
	   break;
	 }
   return ok;
}

/*----------------------------------------------------------------------
  PivotLoadResources
  intialises the pivot format save resource
  ----------------------------------------------------------------------*/
void PivotLoadResources()
{
  if (ThotLocalActions[T_writedocument] == NULL)
    {
      TteConnectAction (T_writedocument, (Proc) WriteDocument);  
      TteConnectAction (T_setwritedirectory, (Proc) SetWriteDirectory);
    }
  ustrcpy (DefaultFileSuffix, ".PIV");
}

