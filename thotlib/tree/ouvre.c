/*
   ouvre.c : gestion de l'ouverture et de la fermeture d'un document.
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "storage.h"

#define EXPORT extern
#include "edit.var"
#include "environ.var"

#include "tree_f.h"
#include "dofile_f.h"
#include "fileaccess_f.h"
#include "ouvre_f.h"
#include "memory_f.h"
#include "readpivot_f.h"
#include "externalref_f.h"

/* ---------------------------------------------------------------------- */
/* |	FreeUnusedReferredElemDesc					| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         FreeUnusedReferredElemDesc (PtrDocument pDoc)

#else  /* __STDC__ */
static void         FreeUnusedReferredElemDesc (pDoc)
PtrDocument         pDoc;
#endif /* __STDC__ */

{
   PtrReferredDescr    pRefD, pPrevRefD;

   pRefD = pDoc->DocReferredEl;
   do
      /* parcourt la chaine des descripteurs de reference du document */
     {
	/* (Le premier descripteur de la chaine est bidon) */
	pRefD = pRefD->ReNext;
	if (pRefD != NULL)
	   if (!pRefD->ReExternalRef)
	      if (pRefD->ReExtDocRef == NULL)
		 if (pRefD->ReFirstReference == NULL)
		   {
		      if (pRefD->ReReferredElem != NULL)
			 pRefD->ReReferredElem->ElReferredDescr = NULL;
		      pRefD->ReReferredElem = NULL;
		      pPrevRefD = pRefD->RePrevious;
		      pPrevRefD->ReNext = pRefD->ReNext;
		      if (pRefD->ReNext != NULL)
			 pRefD->ReNext->RePrevious = pPrevRefD;
		      FreeDescReference (pRefD);
		      pRefD = pPrevRefD;
		   }
     }
   while (pRefD != NULL);
}

/* ---------------------------------------------------------------------- */
/* |    GetPtrDocument cherche si le document d'identificateur docIdent est	| */
/* |	charge'.							| */
/* |            Retourne un pointeur sur son contexte, ou NULL s'il	| */
/* |            n'est pas charge'.                                      | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
PtrDocument         GetPtrDocument (DocumentIdentifier docIdent)

#else  /* __STDC__ */
PtrDocument         GetPtrDocument (docIdent)
DocumentIdentifier     docIdent;

#endif /* __STDC__ */

{
   int                 doc;
   PtrDocument	       pDoc;

   pDoc = NULL;
   for (doc = 0; doc < MAX_DOCUMENTS; doc++)
      if (TabDocuments[doc] != NULL)
	 if (MemeIdentDoc (TabDocuments[doc]->DocIdent, docIdent))
	    pDoc = TabDocuments[doc];
   return pDoc;
}

/* ---------------------------------------------------------------------- */
/* |    OpenDocument ouvre le fichier document de nom docName et le charge  | */
/* |            dans pDoc. loadIncludedDoc indique s'il faut charger ou | */
/* |            non les documents externes dont des parties sont        | */
/* |            incluses dans le document a` ouvrir. Retourne faux si   | */
/* |            le document n'a pas pu etre charge'. Charge le          | */
/* |            squelette si skeleton est TRUE. Ne pas charger de       | */
/* |            schema de structure et utiliser pSS si pSS <> NULL.     | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
boolean             OpenDocument (Name docName, PtrDocument pDoc, boolean loadIncludedDoc, boolean skeleton, PtrSSchema pSS, boolean withAppEvent)

#else  /* __STDC__ */
boolean             OpenDocument (docName, pDoc, loadIncludedDoc, skeleton, pSS, withAppEvent)
Name                 docName;
PtrDocument         pDoc;
boolean             loadIncludedDoc;
boolean             skeleton;
PtrSSchema        pSS;
boolean             withAppEvent;

#endif /* __STDC__ */

{
   boolean             ret;
   int                 i;
   PathBuffer          directoryName;
   PtrChangedReferredEl      pChngRef;
   FILE             *pivotFile;
   char                text[MAX_TXT_LEN];

   ret = FALSE;
   if (pDoc != NULL)
     {
	if (!IdentDocNul (pDoc->DocIdent))
	   /* l'identificateur du document est connu, on accede au document par */
	   /* cet identificateur */
	  {
	     /* on n'a pas d'autre outil de stockage des documents que le SGF UNIX */
	     /* On confond identificateur et nom de document */
	     strncpy (pDoc->DocDName, pDoc->DocIdent, MAX_NAME_LENGTH);
	  }
	else
	   /* le document n'a pas d'identificateur, on l'accede par son nom */
	  {
	     strncpy (pDoc->DocDName, docName, MAX_NAME_LENGTH);
	     /* on n'a pas d'autre outil de stockage des documents que le SGF UNIX */
	     /* On confond identificateur et nom de document */
	     strncpy (pDoc->DocIdent, docName, MAX_DOC_IDENT_LEN);
	  }
	if (pDoc->DocDName[0] > ' ')
	   /* nom de document non vide */
	  {
	     /* compose le nom du fichier a ouvrir avec le nom du directory */
	     /* des documents... */
	     if (pDoc->DocDirectory[0] == '\0')
		strncpy (directoryName, DirectoryDoc, MAX_PATH);
	     else
		strncpy (directoryName, pDoc->DocDirectory, MAX_PATH);
	     BuildFileName (pDoc->DocDName, "PIV", directoryName, text, &i);
	     /* ouvre le fichier 'PIV' */
	     pivotFile = BIOreadOpen (text);
	     if (pivotFile != 0)
		/* le fichier existe */
		/* internalise le fichier pivot sans charger les documents */
		/* externes qui contiennent des elements inclus. */
	       {
		  /* le document appartient au directory courant */
		  strncpy (pDoc->DocDirectory, directoryName, MAX_PATH);
		  ChargeDoc (pivotFile, pDoc, loadIncludedDoc, skeleton, pSS, withAppEvent);
		  BIOreadClose (pivotFile);
		  if (pDoc->DocRootElement != NULL)
		     /* le document lu n'est pas vide */
		    {
		       /* le nom de fichier devient le nom du document */
		       ret = TRUE;
		       /* lit le fichier des references externes s'il existe */
		       /* dans le meme directory que le fichier .PIV */
		       DoFileName (pDoc->DocDName, "EXT", directoryName, text, &i);
		       pivotFile = BIOreadOpen (text);
		       if (pivotFile != 0)
			 {
			    ChargeExt (pivotFile, pDoc, NULL, FALSE);
			    BIOreadClose (pivotFile);
			 }
		       /* lit le fichier de mise a jour des references sortantes */
		       /* s'il existe dans le meme directory que le fichier .PIV */
		       DoFileName (pDoc->DocDName, "REF", directoryName, text, &i);
		       pivotFile = BIOreadOpen (text);
		       if (pivotFile != 0)
			 {
			    ChargeRef (pivotFile, &pChngRef);
			    BIOreadClose (pivotFile);
			    /* traite les mises a jour de */
			    /* references sortantes */
			    if (pChngRef != NULL)
			       MiseAJourRef (pChngRef, pDoc);
			 }
		       /* libere les descripteurs d'element reference' inutilise's */
		       FreeUnusedReferredElemDesc (pDoc);
		    }
	       }
	  }
     }
   return ret;
}


/* ---------------------------------------------------------------------- */
/* |    DeleteAllTrees supprime les arbres abstraits d'un document et de tous | */
/* |            ses elements associes et parametres. Les schemas de     | */
/* |            structure et de presentation utilises par le document   | */
/* |            ne sont pas liberes...                                  | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                DeleteAllTrees (PtrDocument pDoc)

#else  /* __STDC__ */
void                DeleteAllTrees (pDoc)
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   int                 i;
   PtrOutReference      pOutRef, pNextOutRef;
   PtrChangedReferredEl      pChnRef, pNextChnRef;
   PtrTextBuffer      pBuf, pNextBuf;
   PtrExternalDoc       pExtDoc, pNextExtDoc;

   if (pDoc != NULL)
     {
	/* libere les buffers de commentaire */
	pBuf = pDoc->DocComment;
	while (pBuf != NULL)
	  {
	     pNextBuf = pBuf->BuNext;
	     FreeBufTexte (pBuf);
	     pBuf = pNextBuf;
	  }
	pDoc->DocComment = NULL;
	/* libere tout l'arbre du document et ses descripteurs de reference */
	DeleteElement (&pDoc->DocRootElement);
	/* libere les elements associes */
	for (i = 1; i <= MAX_ASSOC_DOC; i++)
	   if (pDoc->DocAssocRoot[i - 1] != NULL)
	      DeleteElement (&pDoc->DocAssocRoot[i - 1]);
	/* libere les parametres */
	for (i = 1; i <= MAX_PARAM_DOC; i++)
	   if (pDoc->DocParameters[i - 1] != NULL)
	      DeleteElement (&pDoc->DocParameters[i - 1]);
	/* libere le 1er descripteur de reference (bidon) */
	FreeDescReference (pDoc->DocReferredEl);
	pDoc->DocReferredEl = NULL;
	/* libere les descripteurs de references sortantes creees */
	pOutRef = pDoc->DocNewOutRef;
	while (pOutRef != NULL)
	  {
	     pNextOutRef = pOutRef->OrNext;
	     FreeRefSortante (pOutRef);
	     pOutRef = pNextOutRef;
	  }
	/* libere les descripteurs de references sortantes detruites */
	pOutRef = pDoc->DocDeadOutRef;
	while (pOutRef != NULL)
	  {
	     pNextOutRef = pOutRef->OrNext;
	     FreeRefSortante (pOutRef);
	     pOutRef = pNextOutRef;
	  }
	/* libere les decripteurs d'element reference's change's */
	pChnRef = pDoc->DocChangedReferredEl;

	while (pChnRef != NULL)
	  {
	     pNextChnRef = pChnRef->CrNext;
	     pExtDoc = pChnRef->CrReferringDoc;
	     pChnRef->CrReferringDoc = NULL;
	     while (pExtDoc != NULL)
	       {
		  pNextExtDoc = pExtDoc->EdNext;
		  FreeDocExterne (pExtDoc);
		  pExtDoc = pNextExtDoc;
	       }
	     FreeElemRefChng (pChnRef);
	     pChnRef = pNextChnRef;
	  }

     }
}

