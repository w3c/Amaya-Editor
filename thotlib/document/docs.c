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
   Module de gestion des documents

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
#include "application.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "appdialogue_tv.h"
#include "appevents_tv.h"

#include "absboxes_f.h"
#include "appli_f.h"
#include "appdialogue_f.h"
#include "applicationapi_f.h"
#include "attributeapi_f.h"
#include "attributes_f.h"
#include "boxselection_f.h"
#include "buildboxes_f.h"
#include "callback_f.h"
#include "callbackinit_f.h"
#include "changeabsbox_f.h"
#include "config_f.h"
#include "createabsbox_f.h"
#include "createpages_f.h"
#include "displayview_f.h"
#include "docs_f.h"
#include "documentapi_f.h"
#include "draw_f.h"
#include "edit_tv.h"
#include "fileaccess_f.h"
#include "frame_tv.h"
#include "memory_f.h"
#include "modif_tv.h"
#include "page_tv.h"
#include "platform_tv.h"
#include "print_tv.h"
#include "paginate_f.h"
#include "platform_f.h"
#include "presvariables_f.h"
#include "readpivot_f.h"
#include "readstr_f.h"
#include "references_f.h"
#include "schemas_f.h"
#include "search_f.h"
#include "searchref_f.h"
#include "select_tv.h"
#include "structcommands_f.h"
#include "structcreation_f.h"
#include "structmodif_f.h"
#include "structschema_f.h"
#include "structselect_f.h"
#include "thotmsg_f.h"
#include "tree_f.h"
#include "undo_f.h"
#include "views_f.h"
#include "viewapi_f.h"
#include "viewcommands_f.h"
#include "writepivot_f.h"

/*----------------------------------------------------------------------
   RedisplayExternalRefs cherche, pour tous les elements du document	
   pDoc qui sont designes par des references, toutes les   
   references appartenant a` d'autres documents charges et 
   fait reafficher ces references si elles sont deja       
   affichees.                                              
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         RedisplayExternalRefs (PtrDocument pDoc)
#else  /* __STDC__ */
static void         RedisplayExternalRefs (pDoc)
PtrDocument         pDoc;
#endif /* __STDC__ */
{
   PtrReferredDescr    pDescElRef;
   PtrReference        pRef;
   PtrDocument         pRefDoc;
   PtrExternalDoc      pExtDoc;
   PtrElement          pEl;

   /* parcourt les descripteurs d'elements reference's du document */
   pDescElRef = pDoc->DocReferredEl->ReNext;
   while (pDescElRef != NULL)
     {
	if (!pDescElRef->ReExternalRef)
	   if (pDescElRef->ReExtDocRef != NULL)
	      /* il y a des references a cet element depuis d'autres */
	      /* documents cherche toutes les references a cet element */
	     {
		pEl = pDescElRef->ReReferredElem;
		/* l'element reference' */
		pRef = NULL;
		pRefDoc = NULL;
		pExtDoc = NULL;
		/* on n'a pas encore trouve' de reference */
		do
		   /* cherche la reference suivant a l'element */
		  {

		     pRef = NextReferenceToEl (pEl, pDoc, FALSE, pRef,
					       &pRefDoc, &pExtDoc, TRUE);
		     if (pRef != NULL)
			/* on a trouve' une reference a cet element */
			if (pRefDoc != pDoc)
			   /* la reference trouvee n'est pas dans le meme */
			   /* document que l'element reference', on reaffiche */
			   /* tous ses paves qui copient l'element reference'. */
			  {
			     if (pRef->RdTypeRef == RefInclusion)
				/* c'est une inclusion */
				if (pRef->RdElement != NULL)
				   if (pRef->RdElement->ElSource != NULL)
				      /* c'est une inclusion avec expansion, on */
				      /* copie d'abord l'element inclus */
				      CopyIncludedElem (pRef->RdElement, pRefDoc);
			     RedispRef (pRef, NULL, pRefDoc);
			  }
		  }
		while (pRef != NULL);
	     }
	/* passe au descripteur d'element reference' suivant */
	pDescElRef = pDescElRef->ReNext;
     }
}


/*----------------------------------------------------------------------
   TtaNewDocument

   Creates the internal representation of a new document according to a given
   structure schema. No file is created immediately, but the backup files
   (.BAK and .SAV) and the document file (.PIV see TtaNewDocument) will be created
   in the first directory of the document path (see TtaSetDocumentPath).

   Parameters:
   structureSchema: name of the structure schema that defines the type of
   document to be created.
   documentName: name of the document to be created (maximum length 19
   characters). The directory name is not part of this parameter
   (see TtaSetDocumentPath).

   Return value:
   the document that has been created or 0 if the document has not
   been created.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
Document            TtaNewDocument (CHAR_T* structureSchema, CHAR_T* documentName)
#else  /* __STDC__ */
Document            TtaNewDocument (structureSchema, documentName)
CHAR_T*             structureSchema;
CHAR_T*             documentName;
#endif /* __STDC__ */
{ 
  PtrDocument         pDoc;
  Document            document;
  int                 i;
  
  UserErrorCode = 0;
  document = 0;
  pDoc = NULL;
  if (documentName[0] == WC_EOS)
    /* No name provided by the user */
    TtaError (ERR_document_name);
  else
    {
      /* initializes a document context */
      CreateDocument (&pDoc);
      if (pDoc == NULL)
	/* No free context document */
	TtaError (ERR_too_many_documents);
      else
	{	  
	  /* charge le schema de structure */
	  GetSchStruct (&pDoc->DocSSchema);
	  pDoc->DocSSchema->SsExtension = FALSE;
	  if (!ReadStructureSchema (structureSchema, pDoc->DocSSchema) ||
	      pDoc->DocSSchema->SsExtension)
	    /* failure while reading the structure schema or while loading
	       a schema extension */
	    {
	      FreeSchStruc (pDoc->DocSSchema);
	      pDoc->DocSSchema = NULL;
	      UnloadDocument (&pDoc);
	      TtaError (ERR_cannot_read_struct_schema);
	    }
	  else
	    {
	      /* The structure schema is loaded */
	      /* The structure schema is translated into the user language */
	      ConfigTranslateSSchema (pDoc->DocSSchema);
#ifndef NODISPLAY
	      InitApplicationSchema (pDoc->DocSSchema);
#endif
	      /* One create the internal representation of an empty document */
	      pDoc->DocRootElement = NewSubtree (pDoc->DocSSchema->SsRootElem,
						 pDoc->DocSSchema, pDoc, 0, TRUE, TRUE, TRUE, TRUE);
	      /* suppress excluded elements */
	      RemoveExcludedElem (&pDoc->DocRootElement, pDoc);
	      if (pDoc->DocRootElement == NULL)
		{
		  UnloadDocument (&pDoc);
		  TtaError (ERR_empty_document);
		}
	      else
		{
		  pDoc->DocRootElement->ElAccess = AccessReadWrite;
#ifndef NODISPLAY
		  /* Create required attributes by the whole created tree */
		  AttachMandatoryAttributes (pDoc->DocRootElement, pDoc);
#endif
		  /* dealing with exceptions */
		  CreateWithException (pDoc->DocRootElement, pDoc);
		  /* An attribut Language is stored in the root */
		  CheckLanguageAttr (pDoc, pDoc->DocRootElement);
		  /* The document is named */
		  ustrncpy (pDoc->DocDName, documentName, MAX_NAME_LENGTH);
		  pDoc->DocDName[MAX_NAME_LENGTH - 1] = WC_EOS;
		  /* one get an identifier to the document */
		  GetDocIdent (&pDoc->DocIdent, documentName); 
		  /* keep the actual schema path in the document context */
		  ustrncpy (pDoc->DocSchemasPath, SchemaPath, MAX_PATH);
		  /* initializes the directory of the document */
		  ustrncpy (pDoc->DocDirectory, DocumentPath, MAX_PATH);
		  /* if path, keep only the first directory */
		  i = 1;
		  while (pDoc->DocDirectory[i - 1] != WC_EOS &&
                 pDoc->DocDirectory[i - 1] != WC_PATH_SEP && i < MAX_PATH)
                i++;
          pDoc->DocDirectory[i - 1] = WC_EOS;
		  /* Read-Write document */
		  pDoc->DocReadOnly = FALSE;
		  document = IdentDocument (pDoc);
		}
	    }
	}
    }
  return document;
}


/*----------------------------------------------------------------------
   LoadDocument charge le document que contient le fichier nomme'  
   fileName dans le descripteur pointe par pDoc. Au	
   retour pDoc est NIL si le document n'a pas pu etre      
   charge.                                                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                LoadDocument (PtrDocument * pDoc, STRING fileName)
#else  /* __STDC__ */
void                LoadDocument (pDoc, fileName)
PtrDocument        *pDoc;
STRING              fileName;

#endif /* __STDC__ */
{
   PathBuffer          directoryBuffer;
   int                 i, j, len;
   ThotBool            ok;
   CHAR_T                URL_DIR_SEP;

   if (fileName && ustrchr (fileName, TEXT('/')))
	  URL_DIR_SEP = TEXT('/');
   else 
	   URL_DIR_SEP = DIR_SEP;

   CreateDocument (pDoc);
   if (pDoc != NULL)
     {
       directoryBuffer[0] = EOS;
       if (fileName != NULL)
	 /* nom de document fourni a l'appel, on le recopie dans DefaultDocumentName */
	 {
	   len = ustrlen (fileName);
	   if (len > 4)
	     if (ustrcmp (fileName + len - 4, TEXT(".PIV")) == 0)
	       fileName[len - 4] = EOS;
	   if (fileName[0] != URL_DIR_SEP)
	     {
	       if (fileName != DefaultDocumentName)
		 ustrncpy (DefaultDocumentName, fileName, MAX_NAME_LENGTH);
	       /* nom de document relatif */
	       ustrncpy ((*pDoc)->DocDName, DefaultDocumentName, MAX_NAME_LENGTH);
	       (*pDoc)->DocDName[MAX_NAME_LENGTH - 1] = EOS;
	       ustrncpy ((*pDoc)->DocIdent, DefaultDocumentName, MAX_DOC_IDENT_LEN);
	       (*pDoc)->DocIdent[MAX_DOC_IDENT_LEN - 1] = EOS;
	       if ((*pDoc)->DocDirectory[0] == EOS)
		 ustrncpy ((*pDoc)->DocDirectory, DocumentPath, MAX_PATH);
	     }
	   else
	     {
	       /* nom absolu */
	       i = 0;
	       j = 0;
	       while (fileName[i] != EOS && i < MAX_PATH - 1)
		 {
		   (*pDoc)->DocDirectory[i] = fileName[i];
		   if ((*pDoc)->DocDirectory[i] == URL_DIR_SEP)
		     j = i;
		   i++;
		 }
	       (*pDoc)->DocDirectory[j + 1] = EOS;
	       i = 0;
	       while (fileName[i] != EOS && i < MAX_NAME_LENGTH - 1)
		 {
		   DefaultDocumentName[i] = fileName[j + 1];
		   i++;
		   j++;
		 }
	       DefaultDocumentName[i] = WC_EOS;
	       ustrncpy ((*pDoc)->DocDName, DefaultDocumentName, MAX_NAME_LENGTH);
	       (*pDoc)->DocDName[MAX_NAME_LENGTH - 1] = EOS;
	       ustrncpy ((*pDoc)->DocIdent, DefaultDocumentName, MAX_DOC_IDENT_LEN);
	       (*pDoc)->DocIdent[MAX_DOC_IDENT_LEN - 1] = EOS;
	       /* sauve le path des documents avant de l'ecraser */
	       ustrncpy (directoryBuffer, DocumentPath, MAX_PATH);
	       ustrncpy (DocumentPath, (*pDoc)->DocDirectory, MAX_PATH);
	     }
	 }

       /* on ouvre le document en chargeant temporairement les documents */
       /* externes qui contiennent les elements inclus dans notre document */
       TtaDisplaySimpleMessage (INFO, LIB, TMSG_READING_DOC);
       ok = OpenDocument (DefaultDocumentName, *pDoc, TRUE, FALSE, NULL, TRUE, TRUE);
       /* restaure le path des documents s'il a ete ecrase */
       if (directoryBuffer[0] != EOS)
	 ustrncpy (DocumentPath, directoryBuffer, MAX_PATH);
       if (!ok)
	 {
	   TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_OPEN_DOC_IMP), DefaultDocumentName);
	   UnloadDocument (pDoc);
	   *pDoc = NULL;
	 }
     }

   if (*pDoc != NULL)
     {
       /* conserve le path actuel des schemas dans le contexte du document */
       ustrncpy ((*pDoc)->DocSchemasPath, SchemaPath, MAX_PATH);
       /* ouvre les vues a ouvrir */
       OpenDefaultViews (*pDoc);
       if ((*pDoc)->DocRootElement != NULL)
	 /* Pour tous les elements du document que l'on vient de */
	 /* charger qui sont designe's par des references, cherche */
	 /* toutes les references appartenant a d'autres documents */
	 /* charges et fait reafficher ces references si elles sont */
	 /* deja affichees */
	 RedisplayExternalRefs (*pDoc);
     }
}

/*----------------------------------------------------------------------
   NewDocument cree un document vide, conforme au schema de nom    
   SSchemaName, dans le descripteur pointe' par pDoc.      
   docName est le nom a donner au document                 
   directory est le directory ou il faut creer le document 
   Au retour pDoc est NIL si le document n'a pas ete cree. 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                NewDocument (PtrDocument * pDoc, CHAR_T* SSchemaName, CHAR_T* docName, PathBuffer directory)
#else  /* __STDC__ */
void                NewDocument (pDoc, SSchemaName, docName, directory)
PtrDocument        *pDoc;
CHAR_T*             SSchemaName;
CHAR_T*             docName;
PathBuffer          directory;

#endif /* __STDC__ */
{
   PtrElement          pEl;
   NotifyDialog        notifyDoc;
   CHAR_T              PSchemaName[MAX_NAME_LENGTH]; 
   CHAR_T              docNameBuffer[MAX_NAME_LENGTH]; 
   CHAR_T              docType[MAX_NAME_LENGTH];
   PathBuffer          directoryBuffer;
   PathBuffer          fileNameBuffer;

   PtrPSchema          pPSchema;
   int                 view;

   int                 i;

   if (*pDoc != NULL)
      if (SSchemaName == NULL || SSchemaName[0] == WC_EOS)
	 /* L'utilisateur n'a pas fourni de nom de schema */
	 UnloadDocument (pDoc);
      else
	{
	   ustrncpy ((*pDoc)->DocDirectory, DocumentPath, MAX_PATH);
	   /* si c'est un path, retient seulement le 1er directory */
	   i = 0;
	   while ((*pDoc)->DocDirectory[i] != EOS &&
		  (*pDoc)->DocDirectory[i] != PATH_SEP && i < MAX_PATH - 1)
	      i++;
	   (*pDoc)->DocDirectory[i] = EOS;
	   /* on suppose que le mon de schema est dans la langue de */
	   /* l'utilisateur: on le traduit en nom interne */
	   ConfigSSchemaInternalName (SSchemaName, docType, TRUE);
	   if (docType[0] == WC_EOS)
	      /* ce nom n'est pas dans le fichier langue, on le prend */
	      /* tel quel */
	      ustrncpy (docType, SSchemaName, MAX_NAME_LENGTH);
	   /* compose le nom du fichier a ouvrir avec le nom du directory */
	   /* des schemas... */
	   ustrncpy (directoryBuffer, SchemaPath, MAX_PATH);
	   MakeCompleteName (docType, TEXT("STR"), directoryBuffer, fileNameBuffer, &i);
	   /* teste si le fichier '.STR' existe */

	   if (TtaFileExist (fileNameBuffer) == 0)
	     {
		ustrncpy (fileNameBuffer, docType, MAX_NAME_LENGTH);
		ustrcat (fileNameBuffer, TEXT(".STR"));
		TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_SCHEMA_NOT_FIND), fileNameBuffer);
	     }
	   else
	     {
		/* charge le schema de structure et le schema de presentation */
		PSchemaName[0] =WC_EOS;
		/* pas de preference pour un schema de */
		/* presentation particulier */
		LoadSchemas (docType, PSchemaName, &((*pDoc)->DocSSchema), NULL, FALSE);
		if (docName[0] != WC_EOS)
		   ustrncpy (docNameBuffer, docName, MAX_NAME_LENGTH);
		else
		  {
		     ustrncpy (docNameBuffer, SSchemaName, MAX_NAME_LENGTH);
		     ustrcat (docNameBuffer, TEXT("X"));
		  }
		if ((*pDoc)->DocSSchema != NULL)
		   if ((*pDoc)->DocSSchema->SsPSchema != NULL)
		     {
			notifyDoc.event = TteDocCreate;
			notifyDoc.document = (Document) IdentDocument (*pDoc);
			notifyDoc.view = 0;
			if (!CallEventType ((NotifyEvent *) & notifyDoc, TRUE))
			  {
			     /* cree la representation interne d'un document vide */
			     (*pDoc)->DocRootElement = NewSubtree ((*pDoc)->DocSSchema->SsRootElem,
				  (*pDoc)->DocSSchema, *pDoc, 0, TRUE, TRUE,
								TRUE, TRUE);
			     /* supprime les elements exclus */
			     RemoveExcludedElem (&((*pDoc)->DocRootElement), *pDoc);
			  }
		     }
	     }
	   if ((*pDoc)->DocRootElement == NULL)
	      /* echec creation document */
	      UnloadDocument (pDoc);
	   else
	     {

		(*pDoc)->DocRootElement->ElAccess = AccessReadWrite;
		CheckLanguageAttr (*pDoc, (*pDoc)->DocRootElement);
		/* ajoute un saut de page a la fin de l'arbre principal */
		/* pour toutes les vues qui sont mises en page */
		/* schema de presentation du document */
		pPSchema = (*pDoc)->DocSSchema->SsPSchema;
		/* examine toutes les vues definies dans le schema */
		for (view = 0; view < pPSchema->PsNViews; view++)
		   if (pPSchema->PsPaginatedView[view])
		      /* cette vue est mise en page */
		      AddLastPageBreak ((*pDoc)->DocRootElement, view + 1, *pDoc, TRUE);
		/* le document appartient au directory courant */
		if (directory[0] != EOS)
		   ustrncpy (directoryBuffer, directory, MAX_PATH);
		else
		  {
		     ustrncpy (directoryBuffer, DocumentPath, MAX_PATH);
		     /* si c'est un path, retient seulement le 1er directory */
		     i = 0;
		     while (directoryBuffer[i] != EOS &&
			 directoryBuffer[i] != PATH_SEP && i < MAX_PATH - 1)
			i++;
		     directoryBuffer[i] = EOS;
		  }
		FindCompleteName (docNameBuffer, TEXT("PIV"), directoryBuffer, fileNameBuffer, &i);
		ustrncpy ((*pDoc)->DocDName, docNameBuffer, MAX_NAME_LENGTH);
		(*pDoc)->DocDName[MAX_NAME_LENGTH - 1] = EOS;
		ustrncpy ((*pDoc)->DocIdent, docNameBuffer, MAX_DOC_IDENT_LEN);
		(*pDoc)->DocIdent[MAX_DOC_IDENT_LEN - 1] = EOS;
		/* le document appartient au directory courant */
		ustrncpy ((*pDoc)->DocDirectory, directoryBuffer, MAX_PATH);
		/* conserve le path actuel des schemas dans le contexte du
		   document */
		ustrncpy ((*pDoc)->DocSchemasPath, SchemaPath, MAX_PATH);
		notifyDoc.event = TteDocCreate;
		notifyDoc.document = (Document) IdentDocument (*pDoc);
		notifyDoc.view = 0;
		CallEventType ((NotifyEvent *) & notifyDoc, FALSE);
		/* traitement des attributs requis */
		AttachMandatoryAttributes ((*pDoc)->DocRootElement, *pDoc);
		if ((*pDoc)->DocSSchema != NULL)
		   /* le document n'a pas ete ferme' pendant l'attente */
		   /* des attributs requis */
		  {
		     /* traitement des exceptions */
		     if (ThotLocalActions[T_createtable] != NULL)
			(*ThotLocalActions[T_createtable])
			   ((*pDoc)->DocRootElement, *pDoc);
		     /* ouvre les vues du document cree' */
		     OpenDefaultViews (*pDoc);
		     /* selectionne la 1ere feuille */
		     pEl = FirstLeaf ((*pDoc)->DocRootElement);
		     SelectElement (*pDoc, pEl, TRUE, TRUE);
		  }
	     }
	}
}


/*----------------------------------------------------------------------
   PaginateDocument	pagine toutes les vues du document pDoc		
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                PaginateDocument (PtrDocument pDoc)
#else  /* __STDC__ */
void                PaginateDocument (pDoc)
PtrDocument         pDoc;
#endif /* __STDC__ */
{
   AvailableView       viewList;
   int                 i, nViews, docView;
   ThotBool            found;

   nViews = BuildDocumentViewList (pDoc, viewList);
   for (i = 0; i < nViews; i++)
      if (viewList[i].VdOpen && viewList[i].VdPaginated && !viewList[i].VdNature)
	/* c'est une vue a paginer */
	{
	  if (viewList[i].VdAssoc)
	    /* c'est une vue d'elements associes, */
	    /* utilise le numero d'element assoc. */
	    docView = viewList[i].VdView;
	  else
	    {
	      /* cherche le numero de vue dans le document */
	      found = FALSE;
	      for (docView = 0; docView < MAX_VIEW_DOC && !found; docView++)
		if (pDoc->DocView[docView].DvPSchemaView == viewList[i].VdView
		    && pDoc->DocView[docView].DvSSchema == viewList[i].VdSSchema)
		  found = TRUE;
	    }
	  /* pagine la vue */
	  PaginateView (pDoc, docView , viewList[i].VdAssoc);
	}
}

/*----------------------------------------------------------------------
   UpdateIncludedElement met a` jour et reaffiche l'element pEl inclus dans  
   le document pDoc.                                       
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                UpdateIncludedElement (PtrElement pEl, PtrDocument pDoc)
#else  /* __STDC__ */
void                UpdateIncludedElement (pEl, pDoc)
PtrElement          pEl;
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   PtrElement          pChild, pNext;
   PtrTextBuffer       pBuf, pNextBuf;
   int                 view;
   ThotBool            ToCreate[MAX_VIEW_DOC];

   /* conserve la liste des vues ou l'element a des paves */
   if (!AssocView (pEl))
      for (view = 0; view < MAX_VIEW_DOC; view++)
	 ToCreate[view] = pEl->ElAbstractBox[view] != NULL;
   else
      /* vue d'elements associes */
      ToCreate[0] = pEl->ElAbstractBox[0] != NULL;
   /* detruit les paves de l'element */
   DestroyAbsBoxes (pEl, pDoc, FALSE);
   /* Update Abstract views */
   if (ThotLocalActions[T_AIupdate] != NULL)
     (*ThotLocalActions[T_AIupdate]) (pDoc);
   if (pEl->ElTerminal)
      switch (pEl->ElLeafType)
	    {
	       case LtPicture:
	       case LtText:
		  pBuf = pEl->ElText;
		  while (pBuf != NULL)
		    {
		       pNextBuf = pBuf->BuNext;
		       FreeTextBuffer (pBuf);
		       pBuf = pNextBuf;
		    }
		  pEl->ElText = NULL;
		  pEl->ElTextLength = 0;
		  break;
	       case LtPolyLine:
		  pBuf = pEl->ElPolyLineBuffer;
		  while (pBuf != NULL)
		    {
		       pNextBuf = pBuf->BuNext;
		       FreeTextBuffer (pBuf);
		       pBuf = pNextBuf;
		    }
		  pEl->ElPolyLineBuffer = NULL;
		  pEl->ElNPoints = 0;
		  pEl->ElVolume = 0;
		  pEl->ElPolyLineType = EOS;
		  break;
	       case LtSymbol:
	       case LtGraphics:
		  pEl->ElGraph = EOS;
		  break;
	       default:
		  break;
	    }
   else
     {
	pChild = pEl->ElFirstChild;
	while (pChild != NULL)
	  {
	     pNext = pChild->ElNext;
	     DeleteElement (&pChild, pDoc);
	     pChild = pNext;
	  }
     }
   /* effectue une nouvelle copie */
   CopyIncludedElem (pEl, pDoc);
   /* cree les paves de la nouvelle copie dans les vues ou il y avait */
   /* deja des paves */
   if (!AssocView (pEl))
     {
	for (view = 0; view < MAX_VIEW_DOC; view++)
	   if (ToCreate[view])
	     {
		pDoc->DocViewFreeVolume[view] = pDoc->DocViewVolume[view];
		CreateNewAbsBoxes (pEl, pDoc, view + 1);
	     }
     }
   else
      /* vue d'elements associes */
   if (ToCreate[0])
     {
	pDoc->DocAssocFreeVolume[pEl->ElAssocNum - 1] =
	   pDoc->DocAssocVolume[pEl->ElAssocNum - 1];
	CreateNewAbsBoxes (pEl, pDoc, 1);
     }
   ApplDelayedRule (pEl, pDoc);
   /* reaffiche l'element dans toutes les vues ou il existe */
   /* Update Abstract views */
   if (ThotLocalActions[T_AIupdate] != NULL)
     (*ThotLocalActions[T_AIupdate]) (pDoc);
   /* Redisplay views */
   if (ThotLocalActions[T_redisplay] != NULL)
     (*ThotLocalActions[T_redisplay]) (pDoc);
   /* Reaffiche les numeros suivants qui changent */
   UpdateNumbers (NextElement (pEl), pEl, pDoc, TRUE);
}


/*----------------------------------------------------------------------
   UpdateAllInclusions met a` jour tous les elements inclus d'un	
   		document.						
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void         UpdateAllInclusions (PtrDocument pDoc)
#else  /* __STDC__ */
void         UpdateAllInclusions (pDoc)
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   PtrReference        pRef;
   PtrReferredDescr    pRefD;
   PtrDocument         pRefDoc;
   PtrExternalDoc      pExtDoc;
   ThotBool            setSelect;

   setSelect = FALSE;
   /* parcourt la chaine des descripteurs d'elements reference's */
   pRefD = pDoc->DocReferredEl;
   if (pRefD != NULL)
      /* saute le premier descripteur bidon */
      pRefD = pRefD->ReNext;
   while (pRefD != NULL)
      /* on ne considere que les elements reference's internes au document */
     {
	if (!pRefD->ReExternalRef)
	  {
	     pRefDoc = NULL;
	     pRef = NULL;
	     pExtDoc = NULL;
	     /* cherche toutes les references a cet element qui se trouvent
	        dans un document charge' dans l'editeur */
	     do
	       {

		  pRef = NextReferenceToEl (pRefD->ReReferredElem, pDoc,
				     FALSE, pRef, &pRefDoc, &pExtDoc, TRUE);
		  if (pRef != NULL)
		     if (pRef->RdTypeRef == RefInclusion)
			/* c'est une inclusion */
			if (pRef->RdElement != NULL)
			   if (pRef->RdElement->ElSource != NULL)
			      /* c'est une inclusion avec expansion */
			      if (pRefDoc != pDoc)
				 /* l'element inclus est dans un autre doc */
				 /* on traite cette inclusion */
				{
				   /* eteint la selection si ca n'a pas deja */
				   /* ete fait */
				   if (!setSelect)
				     {
					TtaClearViewSelections ();
					setSelect = TRUE;
				     }
				   /* refait la copie de l'element inclus */
				   UpdateIncludedElement (pRef->RdElement, pRefDoc);
				}
	       }
	     while (pRef != NULL);
	  }
	/* passe au descripteur d'element reference' suivant */
	if (pRefD != NULL)
	   pRefD = pRefD->ReNext;
     }
   if (setSelect)
      /* rallume la selection */
      HighlightSelection (FALSE, TRUE);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         RemoveExtensionFromTree (PtrElement * pEl, Document document, PtrSSchema pSSExt, int *removedElements, int *removedAttributes)

#else  /* __STDC__ */
static void         RemoveExtensionFromTree (pEl, document, pSSExt, removedElements, removedAttributes)
PtrElement         *pEl;
Document            document;
PtrSSchema          pSSExt;
int                *removedElements;
int                *removedAttributes;
#endif /* __STDC__ */

{
   PtrDocument         pDoc;
   PtrElement          child, nextChild;
   PtrAttribute        attribute, nextAttribute;

   if (*pEl != NULL)
     {
	pDoc = LoadedDocument[document - 1];
	if ((*pEl)->ElStructSchema == pSSExt)
	   /* this element belongs to the extension schema to be removed */
	  {
	     RegisterExternalRef (*pEl, pDoc, FALSE);
	     RegisterDeletedReferredElem (*pEl, pDoc);
#ifndef NODISPLAY
	     UndisplayElement (*pEl, document);
#endif
	     DeleteElement (pEl, pDoc);
	     *pEl = NULL;
	     (*removedElements)++;
	  }
	else
	  {
	     /* looks for all attributes associated with that element */
	     attribute = (*pEl)->ElFirstAttr;
	     while (attribute != NULL)
	       {
		  nextAttribute = attribute->AeNext;
		  if (attribute->AeAttrSSchema == pSSExt)
		    {
		       TtaRemoveAttribute ((Element) (*pEl), (Attribute) attribute, document);
#ifndef NODISPLAY
		       UndisplayInheritedAttributes (*pEl, attribute, document, TRUE);
#endif
#ifndef NODISPLAY
		       UndisplayAttribute (*pEl, attribute, document);
#endif
		       (*removedAttributes)++;
		    }
		  attribute = nextAttribute;
	       }

	     if ((*pEl)->ElTerminal)
		child = NULL;
	     else
		child = (*pEl)->ElFirstChild;
	     while (child != NULL)
	       {
		  nextChild = child->ElNext;
		  RemoveExtensionFromTree (&child, document, pSSExt, removedElements,
					   removedAttributes);
		  child = nextChild;
	       }
	  }
     }
}

/*----------------------------------------------------------------------
   TtaRemoveSchemaExtension

   Removes a structure schema extension from a given document. Removes also from
   the document all attributes and elements defined in that structure schema
   extension.

   Parameters:
   document: the document.
   extension: the structure schema extension to be removed.

   Return parameters:
   removedElements: number of elements actually removed.
   removedAttributes: number of attributes actually removed.

  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                TtaRemoveSchemaExtension (Document document, SSchema extension, int *removedElements, int *removedAttributes)

#else  /* __STDC__ */
void                TtaRemoveSchemaExtension (document, extension, removedElements, removedAttributes)
Document            document;
SSchema             extension;
int                *removedElements;
int                *removedAttributes;

#endif /* __STDC__ */

{
   PtrSSchema          curExtension, previousSSchema;
   PtrElement          root;
   PtrDocument         pDoc;
   ThotBool            found;
   int                 assoc;

   UserErrorCode = 0;
   /* verifies the parameter document */
   if (document < 1 || document > MAX_DOCUMENTS)
	TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
	TtaError (ERR_invalid_document_parameter);
   else
      /* parameter document is correct */
     {
	pDoc = LoadedDocument[document - 1];
	/* Looks for the extension to suppress */
	previousSSchema = pDoc->DocSSchema;
	curExtension = previousSSchema->SsNextExtens;
	found = FALSE;
	while (!found && curExtension != NULL)
	   if (((PtrSSchema) extension)->SsCode == curExtension->SsCode)
	      found = TRUE;
	   else
	     {
		previousSSchema = curExtension;
		curExtension = curExtension->SsNextExtens;
	     }
	if (!found)
	  {
	     TtaError (ERR_invalid_document_parameter);
	  }
	else
	  {
	     root = pDoc->DocRootElement;
	     if (root != NULL)
		RemoveExtensionFromTree (&root, document, (PtrSSchema) extension,
					 removedElements, removedAttributes);
	     for (assoc = 0; assoc < MAX_ASSOC_DOC; assoc++)
	       {
		  root = pDoc->DocAssocRoot[assoc];
		  RemoveExtensionFromTree (&root, document, (PtrSSchema) extension,
					removedElements, removedAttributes);
		  if (root == NULL)
		     pDoc->DocAssocRoot[assoc] = NULL;
	       }
	     previousSSchema->SsNextExtens = curExtension->SsNextExtens;
	     if (curExtension->SsNextExtens != NULL)
		curExtension->SsNextExtens->SsPrevExtens = previousSSchema;
#ifndef NODISPLAY
	     FreePresentationSchema (curExtension->SsPSchema, curExtension);
#endif
	     FreeSchStruc (curExtension);
	  }
     }
}


/*----------------------------------------------------------------------
  BackupAll sauvegarde les fichiers modifies en cas de CRASH majeur
  ----------------------------------------------------------------------*/
void BackupAll()
{
  int             doc;

  ufprintf (stderr, TtaGetMessage (LIB, TMSG_DEBUG_SAV_FILES));
  /* parcourt la table des documents */
  for (doc = 0; doc < MAX_DOCUMENTS; doc++)
    if (LoadedDocument[doc] != NULL)
      /* il y a un document pour cette entree de la table */
      if (LoadedDocument[doc]->DocModified)
	if (ThotLocalActions[T_writedocument] != NULL)
	   (* ThotLocalActions[T_writedocument]) (LoadedDocument[doc], 3);
}

/*----------------------------------------------------------------------
  BackupOnFatalErrorLoadResources
  intialise la resource de sauvegarde les fichiers modifies en cas de 
  CRASH majeur
  ----------------------------------------------------------------------*/
void BackupOnFatalErrorLoadResources()
{
  if (ThotLocalActions[T_backuponfatal] == NULL)
    TteConnectAction (T_backuponfatal, (Proc) BackupAll);
}
