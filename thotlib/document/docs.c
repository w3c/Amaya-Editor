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
   LoadDocument charge le document que contient le fichier nomme'  
   fileName dans le descripteur pointe par pDoc. Au	
   retour pDoc est NIL si le document n'a pas pu etre      
   charge.                                                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                LoadDocument (PtrDocument * pDoc, char *fileName)
#else  /* __STDC__ */
void                LoadDocument (pDoc, fileName)
PtrDocument        *pDoc;
char               *fileName;

#endif /* __STDC__ */
{
   PathBuffer          directoryBuffer;
#ifdef __COLPAGE__
   FILE               *list;
   PtrPSchema          pPagePsch;
#endif /* __COLPAGE__ */
   int                 i, j, len;
   boolean             ok;
   char                URL_DIR_SEP;

   if (fileName && strchr (fileName, '/'))
	  URL_DIR_SEP = '/';
   else 
	   URL_DIR_SEP = DIR_SEP;

   CreateDocument (pDoc);
   if (pDoc != NULL)
     {
       directoryBuffer[0] = EOS;
       if (fileName != NULL)
	 /* nom de document fourni a l'appel, on le recopie dans DefaultDocumentName */
	 {
	   len = strlen (fileName);
	   if (len > 4)
	     if (strcmp (fileName + len - 4, ".PIV") == 0)
	       fileName[len - 4] = EOS;
	   if (fileName[0] != URL_DIR_SEP)
	     {
	       if (fileName != DefaultDocumentName)
		 strncpy (DefaultDocumentName, fileName, MAX_NAME_LENGTH);
	       /* nom de document relatif */
	       strncpy ((*pDoc)->DocDName, DefaultDocumentName, MAX_NAME_LENGTH);
	       strncpy ((*pDoc)->DocIdent, DefaultDocumentName, MAX_DOC_IDENT_LEN);
	       if ((*pDoc)->DocDirectory[0] == EOS)
		 strncpy ((*pDoc)->DocDirectory, DocumentPath, MAX_PATH);
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
	       DefaultDocumentName[i] = EOS;
	       strncpy ((*pDoc)->DocDName, DefaultDocumentName, MAX_NAME_LENGTH);
	       strncpy ((*pDoc)->DocIdent, DefaultDocumentName, MAX_DOC_IDENT_LEN);
	       /* sauve le path des documents avant de l'ecraser */
	       strncpy (directoryBuffer, DocumentPath, MAX_PATH);
	       strncpy (DocumentPath, (*pDoc)->DocDirectory, MAX_PATH);
	     }
	 }

       /* on ouvre le document en chargeant temporairement les documents */
       /* externes qui contiennent les elements inclus dans notre document */
       TtaDisplaySimpleMessage (INFO, LIB, TMSG_READING_DOC);
       ok = OpenDocument (DefaultDocumentName, *pDoc, TRUE, FALSE, NULL, TRUE, TRUE);
       /* restaure le path des documents s'il a ete ecrase */
       if (directoryBuffer[0] != EOS)
	 strncpy (DocumentPath, directoryBuffer, MAX_PATH);
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
       strncpy ((*pDoc)->DocSchemasPath, SchemaPath, MAX_PATH);
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
void                NewDocument (PtrDocument * pDoc, PtrBuffer SSchemaName, Name docName, PathBuffer directory)
#else  /* __STDC__ */
void                NewDocument (pDoc, SSchemaName, docName, directory)
PtrDocument        *pDoc;
PtrBuffer           SSchemaName;
Name                docName;
PathBuffer          directory;

#endif /* __STDC__ */
{
   PtrElement          pEl;
   NotifyDialog        notifyDoc;
   Name                PSchemaName, docNameBuffer, docType;
   PathBuffer          directoryBuffer;
   PathBuffer          fileNameBuffer;

#ifdef __COLPAGE__
   PtrPSchema          pPagePsch;

#else  /* __COLPAGE__ */
   PtrPSchema          pPSchema;
   int                 view;

#endif /* __COLPAGE__ */
   int                 i;

   if (*pDoc != NULL)
      if (SSchemaName == NULL || SSchemaName[0] == EOS)
	 /* L'utilisateur n'a pas fourni de nom de schema */
	 UnloadDocument (pDoc);
      else
	{
	   strncpy ((*pDoc)->DocDirectory, DocumentPath, MAX_PATH);
	   /* si c'est un path, retient seulement le 1er directory */
	   i = 0;
	   while ((*pDoc)->DocDirectory[i] != EOS &&
		  (*pDoc)->DocDirectory[i] != PATH_SEP && i < MAX_PATH - 1)
	      i++;
	   (*pDoc)->DocDirectory[i] = EOS;
	   /* on suppose que le mon de schema est dans la langue de */
	   /* l'utilisateur: on le traduit en nom interne */
	   ConfigSSchemaInternalName ((char *) SSchemaName, docType, TRUE);
	   if (docType[0] == EOS)
	      /* ce nom n'est pas dans le fichier langue, on le prend */
	      /* tel quel */
	      strncpy (docType, (char *) SSchemaName, MAX_NAME_LENGTH);
	   /* compose le nom du fichier a ouvrir avec le nom du directory */
	   /* des schemas... */
	   strncpy (directoryBuffer, SchemaPath, MAX_PATH);
	   MakeCompleteName (docType, "STR", directoryBuffer, fileNameBuffer, &i);
	   /* teste si le fichier '.STR' existe */

	   if (TtaFileExist (fileNameBuffer) == 0)
	     {
		strncpy (fileNameBuffer, docType, MAX_NAME_LENGTH);
		strcat (fileNameBuffer, ".STR");
		TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_SCHEMA_NOT_FIND), fileNameBuffer);
	     }
	   else
	     {
		/* charge le schema de structure et le schema de presentation */
		PSchemaName[0] = EOS;
		/* pas de preference pour un schema de */
		/* presentation particulier */
		LoadSchemas (docType, PSchemaName, &((*pDoc)->DocSSchema), NULL, FALSE);
		if (docName[0] != EOS)
		   strncpy (docNameBuffer, docName, MAX_NAME_LENGTH);
		else
		  {
		     strncpy (docNameBuffer, (char *) SSchemaName, MAX_NAME_LENGTH);
		     strcat (docNameBuffer, "X");
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
			     RemoveExcludedElem (&((*pDoc)->DocRootElement));
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
#ifdef __COLPAGE__
		/* il n'est plus necessaire d'ajouter un saut de page */
		/* a la fin de l'arbre principal */
#else  /* __COLPAGE__ */
		/* ajoute un saut de page a la fin de l'arbre principal */
		/* pour toutes les vues qui sont mises en page */
		/* schema de presentation du document */
		pPSchema = (*pDoc)->DocSSchema->SsPSchema;
		/* examine toutes les vues definies dans le schema */
		for (view = 0; view < pPSchema->PsNViews; view++)
		   if (pPSchema->PsPaginatedView[view])
		      /* cette vue est mise en page */
		      AddLastPageBreak ((*pDoc)->DocRootElement, view + 1, *pDoc, TRUE);
#endif /* __COLPAGE__ */
		/* le document appartient au directory courant */
		if (directory[0] != EOS)
		   strncpy (directoryBuffer, directory, MAX_PATH);
		else
		  {
		     strncpy (directoryBuffer, DocumentPath, MAX_PATH);
		     /* si c'est un path, retient seulement le 1er directory */
		     i = 0;
		     while (directoryBuffer[i] != EOS &&
			 directoryBuffer[i] != PATH_SEP && i < MAX_PATH - 1)
			i++;
		     directoryBuffer[i] = EOS;
		  }
		FindCompleteName (docNameBuffer, "PIV", directoryBuffer, fileNameBuffer, &i);
		strncpy ((*pDoc)->DocDName, docNameBuffer, MAX_NAME_LENGTH);
		strncpy ((*pDoc)->DocIdent, docNameBuffer, MAX_NAME_LENGTH);
		/* le document appartient au directory courant */
		strncpy ((*pDoc)->DocDirectory, directoryBuffer, MAX_PATH);
		/* conserve le path actuel des schemas dans le contexte du
		   document */
		strncpy ((*pDoc)->DocSchemasPath, SchemaPath, MAX_PATH);
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
   SetAbsBoxAccessMode met a` jour le mode d'acces accessMode sur  
   le pave pAb et tous ses descendants.                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         SetAbsBoxAccessMode (PtrAbstractBox pAb, int accessMode)
#else  /* __STDC__ */
static void         SetAbsBoxAccessMode (pAb, accessMode)
PtrAbstractBox      pAb;
int                 accessMode;

#endif /* __STDC__ */
{
   PtrAbstractBox      pAbChild;

   if (accessMode == 0)
      /* read only */
     {
	pAb->AbCanBeModified = FALSE;
	pAb->AbReadOnly = TRUE;
	pAb->AbChange = TRUE;
     }
   else
      /* read write */
      /* on laisse en read only si l'element est en read only */
   if (!ElementIsReadOnly (pAb->AbElement))
     {
	if (!pAb->AbPresentationBox)
	   /* ce n'est pas un pave de presentation, il est donc modifiable */
	   pAb->AbCanBeModified = TRUE;
	else if (PresAbsBoxUserEditable (pAb))
	   pAb->AbCanBeModified = TRUE;
	pAb->AbReadOnly = FALSE;
	pAb->AbChange = TRUE;
     }
   /* on passe aux fils */
   pAbChild = pAb->AbFirstEnclosed;
   while (pAbChild != NULL)
     {
	SetAbsBoxAccessMode (pAbChild, accessMode);
	pAbChild = pAbChild->AbNext;
     }
}

/*----------------------------------------------------------------------
   SetAccessMode met a` jour le mode d'acces sur tout les pave's   
   de tous les elements de toutes les vues du document pDoc. 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetAccessMode (PtrDocument pDoc, int accessMode)
#else  /* __STDC__ */
void                SetAccessMode (pDoc, accessMode)
PtrDocument         pDoc;
int                 accessMode;

#endif /* __STDC__ */
{
   int                 view, assoc;

   /* met a jour les vues de l'arbre principal */
   for (view = 0; view < MAX_VIEW_DOC; view++)
      if (pDoc->DocView[view].DvPSchemaView > 0)
	{
	   SetAbsBoxAccessMode (pDoc->DocRootElement->ElAbstractBox[view], accessMode);
	   pDoc->DocViewModifiedAb[view] = pDoc->DocRootElement->ElAbstractBox[view];
	}
   /* met a jour les vues des elements associes */
   for (assoc = 0; assoc < MAX_ASSOC_DOC; assoc++)
      if (pDoc->DocAssocFrame[assoc] > 0)
	{
	   SetAbsBoxAccessMode (pDoc->DocAssocRoot[assoc]->ElAbstractBox[0], accessMode);
	   pDoc->DocAssocModifiedAb[assoc] = pDoc->DocAssocRoot[assoc]->ElAbstractBox[0];
	}
   /* reaffiche toutes les vues */
   AbstractImageUpdated (pDoc);
   RedisplayDocViews (pDoc);
}


/*----------------------------------------------------------------------
   UnloadTree frees the document tree of pDoc				
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                UnloadTree (Document document)
#else  /* __STDC__ */
void                UnloadTree (document)
Document            document;
#endif /* __STDC__ */
{
  PtrDocument      pDoc;

  pDoc = LoadedDocument[document - 1];
   if (pDoc != NULL)
     {
       /* enleve la selection de ce document */
       ResetSelection (pDoc);
       /* libere le contenu du buffer s'il s'agit d'une partie de ce docum. */
       if (DocOfSavedElements == pDoc)
	 FreeSavedElements ();
       /* libere tous les arbres abstraits */
       DeleteAllTrees (pDoc);
     }
}


/*----------------------------------------------------------------------
   UnloadDocument libere le document pDoc				
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                UnloadDocument (PtrDocument * pDoc)
#else  /* __STDC__ */
void                UnloadDocument (pDoc)
PtrDocument        *pDoc;
#endif /* __STDC__ */
{
   int                 d;

   if (*pDoc != NULL)
      /* cherche dans la table le descripteur de document a liberer */
     {
	d = 0;
	while (LoadedDocument[d] != *pDoc && d < MAX_DOCUMENTS - 1)
	   d++;
	if (LoadedDocument[d] == *pDoc)
	  {
	     /* libere les schemas */
	     FreeDocumentSchemas (*pDoc);
	     FreeDocument (LoadedDocument[d]);
	     LoadedDocument[d] = NULL;
             /* annuler le pointeur sur le doc a imprimer */
             if(PrintingDoc == d+1)
               PrintingDoc = 0;
	     *pDoc = NULL;
             /* libere les contextes inutilises */
	     FreeAll ();
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
   boolean             found;

   nViews = BuildDocumentViewList (pDoc, viewList);
   for (i = 0; i < nViews; i++)
      if (viewList[i].VdOpen)
	 if (viewList[i].VdPaginated)
	    if (!viewList[i].VdNature)
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
			 if (pDoc->DocView[docView].DvPSchemaView ==
			     viewList[i].VdView
			     && pDoc->DocView[docView].DvSSchema ==
			     viewList[i].VdSSchema)
			    found = TRUE;
		   }
		 /* pagine la vue */
		 PaginateView (pDoc, docView , viewList[i].VdAssoc);
	      }
}

/*----------------------------------------------------------------------
   DocumentModified positionne le flag modification d'un document a` TRUE.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DocumentModified (PtrDocument pDoc, PtrElement pEl)
#else  /* __STDC__ */
void                DocumentModified (pDoc, pEl)
PtrDocument         pDoc;
PtrElement          pEl;

#endif /* __STDC__ */
{
   if (pDoc != NULL)
     {
	pDoc->DocModified = TRUE;
	pDoc->DocNTypedChars += 10;
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
   boolean             ToCreate[MAX_VIEW_DOC];

   /* conserve la liste des vues ou l'element a des paves */
   if (!AssocView (pEl))
      for (view = 0; view < MAX_VIEW_DOC; view++)
	 ToCreate[view] = pEl->ElAbstractBox[view] != NULL;
   else
      /* vue d'elements associes */
      ToCreate[0] = pEl->ElAbstractBox[0] != NULL;
   /* detruit les paves de l'element */
   DestroyAbsBoxes (pEl, pDoc, FALSE);
   AbstractImageUpdated (pDoc);
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
	     DeleteElement (&pChild);
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
   AbstractImageUpdated (pDoc);
   RedisplayDocViews (pDoc);
   /* Reaffiche les numeros suivants qui changent */
   UpdateNumbers (NextElement (pEl), pEl, pDoc, TRUE);
}


/*----------------------------------------------------------------------
   UpdateAllInclusions met a` jour tous les elements inclus d'un	
   		document.						
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         UpdateAllInclusions (PtrDocument pDoc)
#else  /* __STDC__ */
static void         UpdateAllInclusions (pDoc)
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   PtrReference        pRef;
   PtrReferredDescr    pRefD;
   PtrDocument         pRefDoc;
   PtrExternalDoc      pExtDoc;
   boolean             setSelect;

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
      HighlightSelection (FALSE);
}


/********** TODO revoir tout ce qui suit ***********/

/*----------------------------------------------------------------------
   simpleSave sauve un document sous forme pivot dans un fichier   
   dont le nom est donne par name, et ne fait rien d'autre.
   Rend false si l'ecriture n'a pu se faire.               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean      simpleSave (PtrDocument pDoc, char *name, boolean withEvent)
#else  /* __STDC__ */
static boolean      simpleSave (pDoc, name, withEvent)
PtrDocument         pDoc;
char               *name;
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
static boolean      saveWithExtension (PtrDocument pDoc, char *extension)
#else  /* __STDC__ */
static boolean      saveWithExtension (pDoc, extension)
PtrDocument         pDoc;
char               *extension;

#endif /* __STDC__ */
{
   char                buf[MAX_TXT_LEN];
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
   char                buf[MAX_TXT_LEN];
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
	if (strcmp (docName, pDoc->DocDName) != 0)
	   sameFile = FALSE;
	if (strcmp (dirName, pDoc->DocDirectory) != 0)
	   sameFile = FALSE;

	/* construit le nom complet de l'ancien fichier de sauvegarde */
	FindCompleteName (pDoc->DocDName, "BAK", pDoc->DocDirectory, bakName, &i);
	strncpy (oldDir, pDoc->DocDirectory, MAX_PATH);
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
		  i = rename (pivName, backName);
	       }
	     /* 2- faire mv du .Tmp sur le .PIV */
	     FindCompleteName (docName, "PIV", dirName, pivName, &i);
	     i = rename (tempName, pivName);
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
		       if (strcmp (dirName, oldDir) != 0 &&
			   strcmp (docName, pDoc->DocDName) == 0)
			  /* changement de directory sans changement de nom */
			  if (move)
			    {
			       /* deplacer le fichier .EXT dans le nouveau directory */
			       FindCompleteName (pDoc->DocDName, "EXT", oldDir, buf, &i);
			       FindCompleteName (pDoc->DocDName, "EXT", dirName, pivName, &i);
			       rename (buf, pivName);
			       /* detruire l'ancien fichier PIV */
			       FindCompleteName (pDoc->DocDName, "PIV", oldDir, buf, &i);
			       TtaFileUnlink (buf);
			    }

		       if (strcmp (docName, pDoc->DocDName) != 0)
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
				 rename (buf, pivName);
				 /* detruit l'ancien fichier .PIV */
				 FindCompleteName (pDoc->DocDName, "PIV", oldDir, buf,
						   &i);
				 TtaFileUnlink (buf);
			      }
			 }
		       strncpy (pDoc->DocDName, docName, MAX_NAME_LENGTH);
		       strncpy (pDoc->DocIdent, docName, MAX_DOC_IDENT_LEN);
		       strncpy (pDoc->DocDirectory, dirName, MAX_PATH);
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
   interactiveSave sauve un document sous forme pivot en proposant 
   un menu a` l'utilisateur (si 'ask'). Rend false si      
   l'ecriture n'a pu se faire.                             
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static boolean      interactiveSave (PtrDocument pDoc, boolean ask)

#else  /* __STDC__ */
static boolean      interactiveSave (pDoc, ask)
PtrDocument         pDoc;
boolean             ask;

#endif /* __STDC__ */

{
   Name                docName;
   char                directory[MAX_PATH];
   boolean             ok;
   boolean             status;

   status = FALSE;
   if (pDoc->DocReadOnly)
      /* on ne sauve pas les documents qui sont en lecture seule */
      TtaDisplaySimpleMessage (INFO, LIB, TMSG_RO_DOC_FORBIDDEN);
   else if (pDoc->DocSSchema == NULL)
      TtaDisplaySimpleMessage (INFO, LIB, TMSG_EMPTY_DOC_NOT_WRITTEN);
   else
     {
	strncpy (docName, pDoc->DocDName, MAX_NAME_LENGTH);
	/* on prend le directory ou le document a ete lu */
	strncpy (directory, pDoc->DocDirectory, MAX_PATH);
	/* recherche le nom du fichier en proposant le nom courant */
	ok = !ask;
	if (ok && !pDoc->DocReadOnly)
	   status = StoreDocument (pDoc, docName, directory, FALSE, FALSE);
     }
   if (status && ask)
     {
	pDoc->DocModified = FALSE;
	pDoc->DocNTypedChars = 0;
     }
   return status;
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
		     ok = interactiveSave (pDoc, TRUE);
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
		     ok = interactiveSave (pDoc, FALSE);
		     break;
		  case 5:
		     ok = saveWithExtension (pDoc, "PIV");
		     break;
	       }
   return ok;
}


/*----------------------------------------------------------------------
  BackupAll sauvegarde les fichiers modifies en cas de CRASH majeur
  ----------------------------------------------------------------------*/
void BackupAll()
{
  int             doc;

  /* parcourt la table des documents */
  for (doc = 0; doc < MAX_DOCUMENTS; doc++)
    if (LoadedDocument[doc] != NULL)
      /* il y a un document pour cette entree de la table */
      if (LoadedDocument[doc]->DocModified)
	WriteDocument(LoadedDocument[doc], 3);
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
