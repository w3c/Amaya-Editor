
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */


#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "application.h"
#include "document.h"
#include "storage.h"
#include "constpiv.h"
#include "typecorr.h"
#include "appdialogue.h"

#ifdef NODISPLAY
/*** pour la bibliotheque ThotKernel, on definit les variables SelPremier
     et SelDernier, qui sont utilisees a la fin de la procedure MergeTextElements  ***/
#undef EXPORT
#define EXPORT
#include "select.var"
#endif

#undef EXPORT
#define EXPORT extern
#include "edit.var"
#include "environ.var"
#ifndef NODISPLAY
#include "modif.var"
#endif
#include "appdialogue.var"

#include "arbabs.f"
#include "attribut.f"
#include "commun.f"
#include "config.f"
#include "docvues.f"
#include "docvuesmenu.f"
#include "dofile.f"
#include "draw.f"
#include "environmenu.f"
#include "exportm.f"
#include "memory.f"
#include "ouvre.f"
#include "pivecr.f"
#include "pivlec.f"
#include "rdschstr.f"
#include "refelem.f"
#include "refext.f"
#include "schemas.f"
#include "storage.f"
#include "structure.f"
#include "thotmsg.f"

extern int          UserErrorCode;
static Name          nameBuffer;

#ifdef __STDC__
extern int          RemoveFile (char *);

#else  /* __STDC__ */
extern int          RemoveFile ();

#endif /* __STDC__ */

#ifdef __STDC__
void                TraiteExceptionCreation (PtrElement pEl, PtrDocument pDoc)

#else  /* __STDC__ */
void                TraiteExceptionCreation (pEl, pDoc)
PtrElement          pEl;
PtrDocument         pDoc;

#endif /* __STDC__ */
{
        /* si creation d'une table */
    if (ThotLocalActions[T_Tableau_Creation]!= NULL)
       (*ThotLocalActions[T_Tableau_Creation])(pEl, pDoc);
}


/* ---------------------------------------------------------------------- 
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
   ---------------------------------------------------------------------- */
#ifdef __STDC__
Document            TtaNewDocument (char *structureSchema, char *documentName)

#else  /* __STDC__ */
Document            TtaNewDocument (structureSchema, documentName)
char               *structureSchema;
char               *documentName;

#endif /* __STDC__ */

{
   PtrDocument         pDoc;
   Document            document;
   int                 i;

   UserErrorCode = 0;
   /* a priori ca va se passer mal */
   document = 0;
   pDoc = NULL;
   if (documentName[0] == '\0')
      /* l'utilisateur n'a pas fourni de nom */
     {
	TtaError (ERR_document_name);
     }
   else
     {
	/* initialise un contexte de document */
	CreateDocument (&pDoc);
	if (pDoc == NULL)
	   /* plus de contexte de document libre */
	  {
	     TtaError (ERR_too_many_documents);
	  }
	else
	  {
	     /* charge le schema de structure */
	     GetSchStruct (&pDoc->DocSSchema);
	     pDoc->DocSSchema->SsExtension = False;
	     if (!RdSchStruct (structureSchema, pDoc->DocSSchema) ||
		 pDoc->DocSSchema->SsExtension)
		/* echec a la lecture du schema de structure ou chargement */
		/* d'une extension de schema */
	       {
		  FreeSStruct (pDoc->DocSSchema);
		  pDoc->DocSSchema = NULL;
		  LibDocument (&pDoc);
		  TtaError (ERR_cannot_read_struct_schema);
	       }
	     else
	       {
		  /* le schema de structure est charge' */
		  /* on traduit le schema de structure dans la langue de */
		  /* l'utilisateur */
		  ConfigTranslateSSchema (pDoc->DocSSchema);
#ifndef NODISPLAY
		  InitSchAppli (pDoc->DocSSchema);
#endif
		  /* on cree la representation interne d'un document vide. */
		  pDoc->DocRootElement = NewSubtree (pDoc->DocSSchema->SsRootElem,
		    pDoc->DocSSchema, pDoc, 0, True, True, True, True);
		  /* supprime les elements exclus (au sens SGML) */
		  RemoveExcludedElem (&pDoc->DocRootElement);
		  if (pDoc->DocRootElement == NULL)
		    {
		       LibDocument (&pDoc);
		       TtaError (ERR_empty_document);
		    }
		  else
		    {
		       pDoc->DocRootElement->ElAccess = AccessReadWrite;
#ifndef NODISPLAY
		       /* cree les attributs requis de tout l'arbre cree' */
		       AttachMandatoryAttributes (pDoc->DocRootElement, pDoc);
#endif
		       /* traitement des exceptions */
		       TraiteExceptionCreation (pDoc->DocRootElement, pDoc);
		       /* on met un attribut Langue sur la racine */
		       CheckLanguageAttr (pDoc, pDoc->DocRootElement);
		       /* on donne son nom au document */
		       strncpy (pDoc->DocDName, documentName, MAX_NAME_LENGTH);
		       /* on acquiert in identificateur pour le document */
		       GetDocIdent (&pDoc->DocIdent, documentName);
		       /* conserve le path actuel des schemas dans le contexte */
		       /* du document */
		       strncpy (pDoc->DocSchemasPath, DirectorySchemas, MAX_PATH);
		       /* initialise le directory du document */
		       strncpy (pDoc->DocDirectory, DirectoryDoc, MAX_PATH);
		       /* si c'est un path, retient seulement le 1er directory */
		       i = 1;
		       while (pDoc->DocDirectory[i - 1] != '\0' &&
		       pDoc->DocDirectory[i - 1] != PATH_SEP && i < MAX_PATH)
			  i++;
		       pDoc->DocDirectory[i - 1] = '\0';
		       /* document en lecture-ecriture */
		       pDoc->DocReadOnly = False;
		       document = IdentDocument (pDoc);
		    }
	       }
	  }
     }
   return document;

}

/* ----------------------------------------------------------------------
   TtaOpenDocument

   Opens an existing document for subsequent operations.

   Parameters:
   documentName: name of the file containing the document to be open
   (maximum length 19 characters). The directory name is not part of
   this parameter (see TtaSetDocumentPath).
   accessMode: 0 = read only, 1 = read-write.

   Return value:
   the opened document, or 0 if the document cannot be open.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
Document            TtaOpenDocument (char *documentName, int accessMode)

#else  /* __STDC__ */
Document            TtaOpenDocument (documentName, accessMode)
char               *documentName;
int                 accessMode;

#endif /* __STDC__ */

{
   PtrDocument         pDoc;
   Document            document;
   int                 lg;
   boolean             ok;

   UserErrorCode = 0;
   /* a priori, on va pas y arriver */
   document = 0;
   /* initialise un contexte de document */
   CreateDocument (&pDoc);
   if (pDoc == NULL)
      /* trop de documents deja ouverts */
     {
	TtaError (ERR_too_many_documents);
     }
   else
     {
	lg = strlen (documentName);
	if (lg >= MAX_NAME_LENGTH)
	   TtaError (ERR_string_too_long);
	else
	  {
	     strncpy (pDoc->DocDName, documentName, MAX_NAME_LENGTH);
	     /* supprime le suffixe .PIV s'il est present */
	     if (lg > 4)
		if (strcmp (&(pDoc->DocDName[lg - 4]), ".PIV") == 0)
		   pDoc->DocDName[lg - 4] = '\0';
	     GetDocIdent (&pDoc->DocIdent, pDoc->DocDName);
	     strncpy (pDoc->DocDirectory, DirectoryDoc, MAX_PATH);
	     ok = OuvreDoc (pDoc->DocDName, pDoc, True, False, NULL, False);
	     if (!ok)
		/* echec d'acces a l'objet pivot */
	       {
		  LibDocument (&pDoc);
		  TtaError (ERR_cannot_open_pivot_file);
	       }
	     else
	       {
		  /* conserve le path actuel des schemas dans le contexte du doc. */
		  strncpy (pDoc->DocSchemasPath, DirectorySchemas, MAX_PATH);
		  document = IdentDocument (pDoc);
		  pDoc->DocReadOnly = (accessMode == 0);
	       }
	  }
     }
   return document;
}


/* ----------------------------------------------------------------------
   TtaSaveDocument

   Saves a document into a file in Thot format. The document is not closed
   by the function and can still be accessed by the application program.

   Parameters:
   document: the document to be saved.
   documentName: name of the file in which the document must be saved
   (maximum length 19 characters). The directory name is not part of
   this parameter (see TtaSetDocumentPath).
   If the documentName is not the same as the one used when opening
   (see TtaOpenDocument) or creating (see TtaNewDocument) the document,
   a new file is created and the file with the old name is unchanged,
   i. e. a new version is created. If necessary, the old file can be
   removed by the function TtaRemoveDocument.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TtaSaveDocument (Document document, char *documentName)

#else  /* __STDC__ */
void                TtaSaveDocument (document, documentName)
Document            document;
char               *documentName;

#endif /* __STDC__ */

{
   PtrDocument         pDoc;
   BinFile             fichpivot;
   char                path[250];
   int                 i;

   UserErrorCode = 0;
   /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else if (TabDocuments[document - 1] == NULL)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else
      /* parametre document correct */
     {
	pDoc = TabDocuments[document - 1];
	if (pDoc->DocReadOnly)
	  {
	     TtaError (ERR_read_only_document);
	  }
	else
	  {
	     /* compose le nom de fichier */
	     DoFileName (documentName, "PIV", pDoc->DocDirectory, path, &i);
	     fichpivot = BIOwriteOpen (path);
	     if (fichpivot == 0)
	       {
		  TtaError (ERR_cannot_open_pivot_file);
	       }
	     else
	       {
		  /* ecrit le document dans ce fichier sous la forme pivot */
		  SauveDoc (fichpivot, pDoc);
		  BIOwriteClose (fichpivot);
		  /* modifie les fichiers .EXT des documents nouvellement */
		  /* reference's ou qui ne sont plus reference's par */
		  /* notre document */
		  UpdateExt (pDoc);
		  /* modifie les fichiers .REF des documents qui */
		  /* referencent des elements qui ne sont plus dans notre */
		  /* document et met a jour le fichier .EXT de notre */
		  /* document */
		  UpdateRef (pDoc);
		  if (strcmp (documentName, pDoc->DocDName) != 0)
		     /* on a sauve' le document avec un nouveau nom */
		    {
		       /* l'application veut creer une copie du document. */
		       /* on fait apparaitre le document copie dans les */
		       /* fichiers .EXT des documents reference's */
		       ChangeNomExt (pDoc, documentName, True);
		       /* met le nouveau nom dans le descripteur du document */
		       strncpy (pDoc->DocDName, documentName, MAX_NAME_LENGTH);
		       strncpy (pDoc->DocIdent, documentName, MAX_DOC_IDENT_LEN);
#ifndef NODISPLAY
		       /* change le titre des frames */
		       changenomdoc (pDoc, documentName);
#endif
		    }
	       }
	  }
     }
}

/* ----------------------------------------------------------------------
   TtaExportDocument

   Saves a whole document into a file in a particular format. The output
   format is specified by a translation schema. The document is not closed
   by the function and it can still be accessed by the application program.

   Parameters:
   document: the document to be exported.
   fileName: name of the file in which the document must be saved,
   including the directory name.
   TSchemaName: name of the translation schema to be used. The directory
   name must not be specified in parameter TSchemaName. See
   function TtaSetSchemaPath.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TtaExportDocument (Document document, char *fileName, char *TSchemaName)

#else  /* __STDC__ */
void                TtaExportDocument (document, fileName, TSchemaName)
Document            document;
char               *fileName;
char               *TSchemaName;

#endif /* __STDC__ */

{
   UserErrorCode = 0;
   /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else if (TabDocuments[document - 1] == NULL)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else
      /* parametre document correct */
     {
	ExportDocument (TabDocuments[document - 1], fileName, TSchemaName);
     }
}

/* ----------------------------------------------------------------------
   TtaCloseDocument

   Closes a document that is no longer needed and releases all ressources
   allocated to the document. This function does not save the document.

   Parameter:
   document: the document to be closed.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TtaCloseDocument (Document document)

#else  /* __STDC__ */
void                TtaCloseDocument (document)
Document            document;

#endif /* __STDC__ */

{
#ifndef NODISPLAY
   int                 nv, numassoc;
   PtrDocument         pDoc;

#endif

   UserErrorCode = 0;
   /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else if (TabDocuments[document - 1] == NULL)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else
      /* parametre document correct */
     {
#ifndef NODISPLAY
	/* on ferme toutes les vues ouvertes du document */
	pDoc = TabDocuments[document - 1];
	/* on ferme d'abord les vues de l'arbre principal */
	for (nv = 1; nv <= MAX_VIEW_DOC; nv++)
	   if (pDoc->DocView[nv - 1].DvPSchemaView != 0)
	     {
		DetruitFenetre (pDoc->DocViewFrame[nv - 1]);
		detruit (pDoc, nv, False, False);
	     }
	/* on ferme ensuite les frames des elements associes */
	for (numassoc = 1; numassoc <= MAX_ASSOC_DOC; numassoc++)
	   if (pDoc->DocAssocFrame[numassoc - 1] != 0)
	     {
		DetruitFenetre (pDoc->DocAssocFrame[numassoc - 1]);
		detruit (pDoc, numassoc, True, False);
	     }
#endif
	LibDocument (&TabDocuments[document - 1]);
     }
}


/* ----------------------------------------------------------------------
   TtaRemoveDocument

   Closes a document, releases all ressources allocated to that document,
   removes all files related to the document and updates all links connecting
   the removed document with other documents.

   Parameter:
   document: the document to be removed.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TtaRemoveDocument (Document document)

#else  /* __STDC__ */
void                TtaRemoveDocument (document)
Document            document;

#endif /* __STDC__ */

{
#ifndef NODISPLAY
   int                 nv, numassoc;

#endif
   PtrDocument         pDoc;
   int                 i;
   PathBuffer          DirectoryOrig;
   char                texte[MAX_TXT_LEN];

   UserErrorCode = 0;
   /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else if (TabDocuments[document - 1] == NULL)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else
      /* parametre document correct */
     {
	pDoc = TabDocuments[document - 1];
	/* Note d'abord dans le contexte du document tous les liens de */
	/* reference externe */
	/* traite l'arbre principal du document */
	RegisterExternalRef (pDoc->DocRootElement, pDoc, False);
	RegisterDeletedReferredElem (pDoc->DocRootElement, pDoc);
	/* traite les arbres d'elements associes */
	for (i = 1; i <= MAX_ASSOC_DOC; i++)
	   if (pDoc->DocAssocRoot[i - 1] != NULL)
	     {
		RegisterExternalRef (pDoc->DocAssocRoot[i - 1], pDoc, False);
		RegisterDeletedReferredElem (pDoc->DocAssocRoot[i - 1], pDoc);
	     }
	/* traite les parametres */
	for (i = 1; i <= MAX_PARAM_DOC; i++)
	   if (pDoc->DocParameters[i - 1] != NULL)
	     {
		RegisterExternalRef (pDoc->DocParameters[i - 1], pDoc, False);
		RegisterDeletedReferredElem (pDoc->DocParameters[i - 1], pDoc);
	     }
	/* modifie les fichiers .EXT des documents qui etaient */
	/* reference's par le document detruit */
	UpdateExt (pDoc);
	/* modifie les fichiers .REF des documents qui referencent des */
	/* elements du document detruit */
	UpdateRef (pDoc);
	/* detruit les fichiers .PIV, .EXT, .REF et .BAK du document */
	strncpy (DirectoryOrig, pDoc->DocDirectory, MAX_PATH);
	DoFileName (pDoc->DocDName, "PIV", DirectoryOrig, texte, &i);
	RemoveFile (texte);
	strncpy (DirectoryOrig, pDoc->DocDirectory, MAX_PATH);
	DoFileName (pDoc->DocDName, "EXT", DirectoryOrig, texte, &i);
	RemoveFile (texte);
	strncpy (DirectoryOrig, pDoc->DocDirectory, MAX_PATH);
	DoFileName (pDoc->DocDName, "REF", DirectoryOrig, texte, &i);
	RemoveFile (texte);
	strncpy (DirectoryOrig, pDoc->DocDirectory, MAX_PATH);
	DoFileName (pDoc->DocDName, "BAK", DirectoryOrig, texte, &i);
#ifndef NODISPLAY
	/* on ferme toutes les vues ouvertes du document */
	/* on ferme d'abord les vues de l'arbre principal */
	for (nv = 1; nv <= MAX_VIEW_DOC; nv++)
	   if (pDoc->DocView[nv - 1].DvPSchemaView != 0)
	     {
		DetruitFenetre (pDoc->DocViewFrame[nv - 1]);
		detruit (pDoc, nv, False, False);
	     }
	/* on ferme ensuite les frames des elements associes */
	for (numassoc = 1; numassoc <= MAX_ASSOC_DOC; numassoc++)
	   if (pDoc->DocAssocFrame[numassoc - 1] != 0)
	     {
		DetruitFenetre (pDoc->DocAssocFrame[numassoc - 1]);
		detruit (pDoc, numassoc, True, False);
	     }
#endif
	LibDocument (&TabDocuments[document - 1]);
     }
}


/* ----------------------------------------------------------------------
   TtaSetDocumentPath

   Sets a new list of document directories. This list replaces the existing one.
   It is used for searching a document when it is open either by the user or
   by the application program (see TtaOpenDocument).
   The first directory in the list is used when a new document is created
   (see TtaNewDocument).

   Parameter:
   path: the directory list, where directory names are separated by
   the character PATH_SEP.

   ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtaSetDocumentPath (char *path)
#else  /* __STDC__ */
void                TtaSetDocumentPath (path)
char               *path;

#endif /* __STDC__ */
{
   UserErrorCode = 0;
   if (strlen (path) >= MAX_PATH)
      TtaError (ERR_string_too_long);
   else
     {
	strcpy (DirectoryDoc, path);
     }
}


/* ----------------------------------------------------------------------
   TtaIsInDocumentPath

   returns True if the directory is in the list of document directories.
   Parameter:
   aDirectory: the new directory name.

   ---------------------------------------------------------------------- */
#ifdef __STDC__
boolean             TtaIsInDocumentPath (char *aDirectory)

#else  /* __STDC__ */
boolean             TtaIsInDocumentPath (aDirectory)
char               *aDirectory;

#endif /* __STDC__ */

{
   int                 i;
   char               *ptr;

   /* Regarde si ce directory est deja dans la liste */
   ptr = strstr (DirectoryDoc, aDirectory);
   i = strlen (aDirectory);
   while (ptr != NULL && ptr[i] != PATH_SEP && ptr[i] != '\0')
     {
	/* on a trouve une sous-chaine */
	ptr = strstr (ptr, PATH_STR);
	if (ptr != NULL)
	   ptr = strstr (ptr, aDirectory);
     }
   return (ptr != NULL);
}


/* ----------------------------------------------------------------------
   TtaAppendDocumentPath

   Appends a new directory in the list of document directories if this directory
   is not already in the list and if the directory exists.

   Parameter:
   aDirectory: the new directory name.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TtaAppendDocumentPath (char *aDirectory)

#else  /* __STDC__ */
void                TtaAppendDocumentPath (aDirectory)
char               *aDirectory;

#endif /* __STDC__ */

{
   int                 i;
   int                 lg;

   UserErrorCode = 0;
   lg = strlen (aDirectory);

   if (lg >= MAX_PATH)
      TtaError (ERR_string_too_long);
   else if (!TtaCheckDirectory (aDirectory))
      TtaError (ERR_invalid_parameter);
   else if (!TtaIsInDocumentPath (aDirectory))
     {
	/* add the directory in the path */
	i = strlen (DirectoryDoc);
	if (i + lg + 2 >= MAX_PATH)
	   TtaError (ERR_string_too_long);
	else
	  {
	     if (i > 0)
		strcat (DirectoryDoc, PATH_STR);
	     strcat (DirectoryDoc, aDirectory);
	  }
     }
}

/* ----------------------------------------------------------------------
   TtaSetSchemaPath

   Sets a new list of schema directories. This list replaces the existing one.
   It is used for searching schemas.

   Parameter:
   path: the directory list, where directory names are separated by
   the character PATH_SEP.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TtaSetSchemaPath (char *path)

#else  /* __STDC__ */
void                TtaSetSchemaPath (path)
char               *path;

#endif /* __STDC__ */

{
   UserErrorCode = 0;
   if (strlen (path) >= MAX_PATH)
      TtaError (ERR_string_too_long);
   else
      strcpy (DirectorySchemas, path);
}

/* ----------------------------------------------------------------------
   TtaNewNature

   Adds a new nature in a structure schema and returns the structure schema
   of the new nature. If the nature already exists in that structure schema,
   the function simply returns the structure schema of that nature.

   Parameters:
   schema: the structure schema to which the nature is added.
   natureName: name of the nature to be added in the structure schema.
   presentationName: name of the presentation schema to be associated with
   the extension schema. If presentationName is an empty string, the
   default presentation schema is associated. If the nature already
   exists, presentationName is ignored.

   Return value:
   the structure schema of the new nature; NULL if the structure schema
   has not been loaded.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
SSchema             TtaNewNature (SSchema schema, char *natureName, char *presentationName)

#else  /* __STDC__ */
SSchema             TtaNewNature (schema, natureName, presentationName)
SSchema             schema;
char               *natureName;
char               *presentationName;

#endif /* __STDC__ */

{
   int         regleNature;
   PtrSSchema        natureSchema;

   UserErrorCode = 0;
   natureSchema = NULL;
   if (schema == NULL || natureName[0] == '\0')
     {
	TtaError (ERR_invalid_parameter);
     }
   else
     {
	regleNature = CreeNature (natureName, presentationName,
				  (PtrSSchema) schema);
	if (regleNature == 0)
	  {
	     TtaError (ERR_invalid_parameter);
	  }
	else
	   natureSchema = ((PtrSSchema) schema)->SsRule[regleNature - 1].SrSSchemaNat;
     }
   return ((SSchema) natureSchema);
}


/* ----------------------------------------------------------------------
   TtaNewSchemaExtension

   Loads a structure schema extension and associates it with
   a given document.

   Parameters:
   document: the document whose structure schema must be extended.
   extensionName: name of the extension schema.
   presentationName: name of the presentation schema to be associated with
   the extension schema. If presentationName is an empty string, the
   default presentation schema is associated.

   Return value:
   the extension schema, NULL if the extension schema has not been loaded.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
SSchema             TtaNewSchemaExtension (Document document, char *extensionName, char *presentationName)

#else  /* __STDC__ */
SSchema             TtaNewSchemaExtension (document, extensionName, presentationName)
Document            document;
char               *extensionName;
char               *presentationName;

#endif /* __STDC__ */

{
   PtrSSchema        extension;

   UserErrorCode = 0;
   /* verifie le parametre document */
   extension = NULL;
   if (document < 1 || document > MAX_DOCUMENTS)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else if (TabDocuments[document - 1] == NULL)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else
      /* parametre document correct */
     {
	extension = LoadExtension (extensionName, presentationName,
				   TabDocuments[document - 1]);
	if (extension == NULL)
	  {
	     TtaError (ERR_cannot_read_struct_schema);
	  }
     }
   return ((SSchema) extension);
}

#ifdef __STDC__
static void         RemoveExtensionFromTree (PtrElement * pEl, Document document, PtrSSchema pSSExt, int *removedElements, int *removedAttributes)

#else  /* __STDC__ */
static void         RemoveExtensionFromTree (pEl, document, pSSExt, removedElements, removedAttributes)
PtrElement         *pEl;
Document            document;
PtrSSchema        pSSExt;
int                *removedElements;
int                *removedAttributes;

#endif /* __STDC__ */

{
   PtrDocument         pDoc;
   PtrElement          child, nextChild;
   PtrAttribute         attribute, nextAttribute;

   if (*pEl != NULL)
     {
	pDoc = TabDocuments[document - 1];
	if ((*pEl)->ElSructSchema == pSSExt)
	   /* this element belongs to the extension schema to be removed */
	  {
	     RegisterExternalRef (*pEl, pDoc, False);
	     RegisterDeletedReferredElem (*pEl, pDoc);
#ifndef NODISPLAY
	     UndisplayElement (*pEl, document);
#endif
	     DeleteElement (pEl);
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
		       UndisplayHeritAttr (*pEl, attribute, document, True);
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


/* ----------------------------------------------------------------------
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

   ---------------------------------------------------------------------- */

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
   PtrSSchema        curExtension, previousSSchema;
   PtrElement          root;
   PtrDocument         pDoc;
   boolean             found;
   int                 assoc;

   UserErrorCode = 0;
   /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else if (TabDocuments[document - 1] == NULL)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else
      /* parametre document correct */
     {
	pDoc = TabDocuments[document - 1];
	/* cherche l'extension a retirer */
	previousSSchema = pDoc->DocSSchema;
	curExtension = previousSSchema->SsNextExtens;
	found = False;
	while (!found && curExtension != NULL)
	   if (((PtrSSchema) extension)->SsCode == curExtension->SsCode)
	      found = True;
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
	     SupprSchPrs (curExtension->SsPSchema, curExtension);
#endif
	     FreeSStruct (curExtension);
	  }
     }
}


/* ----------------------------------------------------------------------
   TtaSetPSchema

   Sets or changes the main presentation schema of a document. The document
   must be open, but no view must be open for that document.

   Parameters:
   document: the document.
   presentationName: Name of the presentation schema to be associated
   with the document.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TtaSetPSchema (Document document, char *presentationName)

#else  /* __STDC__ */
void                TtaSetPSchema (document, presentationName)
Document            document;
char               *presentationName;

#endif /* __STDC__ */

{
   PtrDocument         pDoc;

#ifndef NODISPLAY
   int                 Vue;
   int                 Assoc;
   boolean             ok;

#endif

   UserErrorCode = 0;
   /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else if (TabDocuments[document - 1] == NULL)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else
      /* parametre document correct */
     {
	pDoc = TabDocuments[document - 1];
#ifdef NODISPLAY
	if (pDoc->DocSSchema != NULL)
	   strncpy (pDoc->DocSSchema->SsDefaultPSchema, presentationName,
		    MAX_NAME_LENGTH - 1);
#else
	/* verifie qu'aucune vue n'est ouverte */
	ok = True;
	for (Vue = 1; Vue <= MAX_VIEW_DOC && ok; Vue++)
	   if (pDoc->DocView[Vue - 1].DvPSchemaView != 0)
	      ok = False;
	if (ok)
	   for (Assoc = 1; Assoc <= MAX_ASSOC_DOC && ok; Assoc++)
	      if (pDoc->DocAssocFrame[Assoc - 1] != 0)
		 ok = False;
	if (!ok)
	  {
	     TtaError (ERR_there_are_open_views);
	  }
	else
	   /* aucune vue n'est ouverte */
	  {
	     if (pDoc->DocSSchema->SsPSchema != NULL)
		/* il y a deja un schema de presentation */
	       {
		  /* on le libere */
		  SupprSchPrs (pDoc->DocSSchema->SsPSchema, pDoc->DocSSchema);
		  pDoc->DocSSchema->SsPSchema = NULL;
	       }
	     /* charge le schema de presentation */
	     if (pDoc->DocSSchema->SsExtension)
		/*pour eviter que RdSchPres recharge le schema de structure */
		pDoc->DocSSchema->SsRootElem = 1;
	     pDoc->DocSSchema->SsPSchema = LdSchPres (presentationName,
							 pDoc->DocSSchema);
	     if (pDoc->DocSSchema->SsPSchema == NULL)
		/* echec chargement schema */
	       {
		  TtaError (ERR_cannot_load_pschema);
	       }
	  }
#endif
     }
}


/* ----------------------------------------------------------------------
   TtaSetDocumentDirectory

   Sets the directory to which the document is supposed to be saved.

   Parameters:
   document: the document whose directory is set.
   directory: new document directory.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TtaSetDocumentDirectory (Document document, char *directory)

#else  /* __STDC__ */
void                TtaSetDocumentDirectory (document, directory)
Document            document;
char               *directory;

#endif /* __STDC__ */

{
   UserErrorCode = 0;
   /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else if (TabDocuments[document - 1] == NULL)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else
      /* parametre document correct */
     {
	if (strlen (directory) >= MAX_PATH)
	   TtaError (ERR_buffer_too_small);
	strcpy (TabDocuments[document - 1]->DocDirectory, directory);
     }
}


/* ----------------------------------------------------------------------
   TtaSetDocumentName

   Sets or changes the name of a document. The document must be loaded.

   Parameters:
   document: the document whose name is set.
   documentName: new document name. This is only the name, without any
   suffix, without directory name. See function TtaSetDocumentDirectory
   for changing the directory of a document.
   The name must not exceed 31 characters.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TtaSetDocumentName (Document document, char *documentName)

#else  /* __STDC__ */
void                TtaSetDocumentName (document, documentName)
Document            document;
char               *documentName;

#endif /* __STDC__ */

{
   UserErrorCode = 0;
   /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else if (TabDocuments[document - 1] == NULL)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else
      /* parametre document correct */
     {
	if (strlen (documentName) >= MAX_NAME_LENGTH)
	   TtaError (ERR_buffer_too_small);
	else
	  {
#ifndef NODISPLAY
	     changenomdoc (TabDocuments[document - 1], documentName);
#else
	     strncpy (TabDocuments[document - 1]->DocDName, documentName, MAX_NAME_LENGTH);
	     strncpy (TabDocuments[document - 1]->DocIdent, documentName, MAX_DOC_IDENT_LEN);
#endif
	  }
     }
}


/* ----------------------------------------------------------------------
   TtaSetDocumentAccessMode

   Sets the access mode for a document.

   Parameters:
   document: the document whose access mode is changed.
   accessMode: 0 = read only, 1 = read-write.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TtaSetDocumentAccessMode (Document document, int accessMode)

#else  /* __STDC__ */
void                TtaSetDocumentAccessMode (document, accessMode)
Document            document;
int                 accessMode;

#endif /* __STDC__ */

{
   UserErrorCode = 0;
   /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else if (TabDocuments[document - 1] == NULL)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else
      /* parametre document correct */
     {
	TabDocuments[document - 1]->DocReadOnly = (accessMode == 0);
#ifndef NODISPLAY
	MajAccessMode (TabDocuments[document - 1], accessMode);
#endif
     }
}


/* ----------------------------------------------------------------------
   TtaSetDocumentBackUpInterval

   Sets the backup interval for a document.

   Parameters:
   document: the document whose backup interval is changed.
   interval:
   0 : the backup mechanism must be disabled
   positive integer : number of characters typed which triggers
   automatic save of the document into a .BAK file.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TtaSetDocumentBackUpInterval (Document document, int interval)

#else  /* __STDC__ */
void                TtaSetDocumentBackUpInterval (document, interval)
Document            document;
int                 interval;

#endif /* __STDC__ */

{
   UserErrorCode = 0;
   /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else if (TabDocuments[document - 1] == NULL)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else
      /* parametre document correct */
   if (interval < 0)
     {
	TtaError (ERR_invalid_parameter);
     }
   else
      TabDocuments[document - 1]->DocBackUpInterval = interval;
}

/* ----------------------------------------------------------------------
   TtaSetNotificationMode

   Sets the ECF notification mode for a document.
   Parameters:
   document: the document whose notification mode is changed.
   notificationMode: 0 = only roots of created and deleted subtrees must
   be notified, 1 = all elements of created and deleted subtrees must
   be notified.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TtaSetNotificationMode (Document document, int notificationMode)

#else  /* __STDC__ */
void                TtaSetNotificationMode (document, notificationMode)
Document            document;
int                 notificationMode;

#endif /* __STDC__ */

{
   UserErrorCode = 0;
   /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else if (TabDocuments[document - 1] == NULL)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else
      /* parametre document correct */
      TabDocuments[document - 1]->DocNotifyAll = (notificationMode != 0);
}


/* ----------------------------------------------------------------------
   TtaSetDocumentModified

   Notifies the tool kit that a document has been modified by the application.
   As a consequence, the user will be asked to save the document when closing it.

   Parameter:
   document: the document.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TtaSetDocumentModified (Document document)

#else  /* __STDC__ */
void                TtaSetDocumentModified (document)
Document            document;

#endif /* __STDC__ */

{
   UserErrorCode = 0;
   /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else if (TabDocuments[document - 1] == NULL)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else
      /* parametre document correct */
      TabDocuments[document - 1]->DocModified = True;
}

/* ----------------------------------------------------------------------
   TtaSetDocumentUnmodified

   Notifies the tool kit that a document must be considered as not modified
   by the application or by the user. As a consequence, if no further modification
   is made to that document, the user will not be asked to save the document
   when closing it.

   Parameter:
   document: the document.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TtaSetDocumentUnmodified (Document document)

#else  /* __STDC__ */
void                TtaSetDocumentUnmodified (document)
Document            document;

#endif /* __STDC__ */

{
   UserErrorCode = 0;
   /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else if (TabDocuments[document - 1] == NULL)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else
      /* parametre document correct */
      TabDocuments[document - 1]->DocModified = False;
}

/* ----------------------------------------------------------------------
   TtaGetDocumentName

   Returns the name of a document.

   Parameter:
   document: the document whose name is asked.

   Return value:
   name of that document.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
char               *TtaGetDocumentName (Document document)

#else  /* __STDC__ */
char               *TtaGetDocumentName (document)
Document            document;

#endif /* __STDC__ */

{
   UserErrorCode = 0;
   nameBuffer[0] = '\0';
   /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else if (TabDocuments[document - 1] == NULL)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else
      /* parametre document correct */
     {
	strcpy (nameBuffer, TabDocuments[document - 1]->DocDName);
     }
   return nameBuffer;
}


/* ----------------------------------------------------------------------
   TtaGetDocumentFromName

   Returns the document having a given name.

   Parameter:
   documentName: the document name.

   Return value:
   the document having that name.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
Document            TtaGetDocumentFromName (char *documentName)

#else  /* __STDC__ */
Document            TtaGetDocumentFromName (documentName)
char               *documentName;

#endif /* __STDC__ */

{
   int                 document;
   boolean             found;

   UserErrorCode = 0;
   document = 1;
   found = False;
   while (!found && document < MAX_DOCUMENTS)
     {
	if (TabDocuments[document - 1] != NULL)
	   if (strcmp (documentName, TabDocuments[document - 1]->DocDName) == 0)
	      found = True;
	if (!found)
	   document++;
     }
   if (!found)
      document = 0;
   return (Document) document;
}


/* ----------------------------------------------------------------------
   TtaGetDocumentDirectory

   Returns the directory to which the document is supposed to be saved.

   Parameters:
   document: the document whose directory is asked.
   buffer: a buffer provided by the caller.
   bufferLength: the length of that buffer.

   Return parameter:
   buffer: the document directory.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TtaGetDocumentDirectory (Document document, char *buffer, int bufferLength)

#else  /* __STDC__ */
void                TtaGetDocumentDirectory (document, buffer, bufferLength)
Document            document;
char               *buffer;
int                 bufferLength;

#endif /* __STDC__ */

{
   UserErrorCode = 0;
   nameBuffer[0] = '\0';
   /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else if (TabDocuments[document - 1] == NULL)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else
      /* parametre document correct */
     {
	if (strlen (TabDocuments[document - 1]->DocDirectory) >= bufferLength)
	   TtaError (ERR_buffer_too_small);
	strncpy (buffer, TabDocuments[document - 1]->DocDirectory, bufferLength - 1);
     }
}

/* ----------------------------------------------------------------------
   TtaGetDocumentSSchema

   Returns the main structure schema of a document.

   Parameter:
   document: the document for which the structure schema is asked.

   Return value:
   the structure schema of that document.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
SSchema             TtaGetDocumentSSchema (Document document)

#else  /* __STDC__ */
SSchema             TtaGetDocumentSSchema (document)
Document            document;

#endif /* __STDC__ */

{
   SSchema             schema;

   UserErrorCode = 0;
   /* verifie le parametre document */
   schema = NULL;
   if (document < 1 || document > MAX_DOCUMENTS)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else if (TabDocuments[document - 1] == NULL)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else
      /* parametre document correct */
     {
	schema = (SSchema) TabDocuments[document - 1]->DocSSchema;
     }
   return schema;
}

/* ----------------------------------------------------------------------
   TtaGetSSchemaName

   Returns the name of a structure schema.

   Parameter:
   schema: the structure schema of interest.

   Return value:
   name of that structure schema.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
char               *TtaGetSSchemaName (SSchema schema)

#else  /* __STDC__ */
char               *TtaGetSSchemaName (schema)
SSchema             schema;

#endif /* __STDC__ */

{
   UserErrorCode = 0;
   if (schema == NULL)
     {
	nameBuffer[0] = '\0';
	TtaError (ERR_invalid_parameter);
     }
   else
     {
	strcpy (nameBuffer, ((PtrSSchema) schema)->SsName);
     }
   return nameBuffer;
}

/* ----------------------------------------------------------------------
   TtaGetPSchemaName

   Returns the name of the presentation schema currently associated
   with a given structure schema.

   Parameter:
   schema: the structure schema of interest.

   Return value:
   name of the associated presentation schema.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
char               *TtaGetPSchemaName (SSchema schema)

#else  /* __STDC__ */
char               *TtaGetPSchemaName (schema)
SSchema             schema;

#endif /* __STDC__ */

{
   UserErrorCode = 0;
   if (schema == NULL)
     {
	nameBuffer[0] = '\0';
	TtaError (ERR_invalid_parameter);
     }
   else
     {
	strcpy (nameBuffer, ((PtrSSchema) schema)->SsDefaultPSchema);
     }
   return nameBuffer;
}

	/*
	   ChSchStruct cherche recursivement parmi les schemas de nature et
	   d'extension utilise's par pSS, celui qui porte le nom name
	   et retourne un pointeur sur ce schema ou NULL si pas trouve'.
	 */
#ifdef __STDC__
static SSchema      ChSchStruct (PtrSSchema pSS, char *name)

#else  /* __STDC__ */
static SSchema      ChSchStruct (pSS, name)
PtrSSchema        pSS;
char               *name;

#endif /* __STDC__ */

{
   int         nRegle;
   SSchema             retour;

   retour = NULL;
   if (pSS != NULL)
      if (strcmp (name, pSS->SsName) == 0)
	 /* c'est le schema lui-meme */
	 retour = (SSchema) pSS;
      else
	{
	   /* cherche les regles nature du schema */
	   for (nRegle = MAX_BASIC_TYPE - 1; retour == NULL && nRegle < pSS->SsNRules; nRegle++)
	      if (pSS->SsRule[nRegle].SrConstruct == CsNatureSchema)
		 /* une nature, cherche dans son schema de structure */
		 retour = ChSchStruct (pSS->SsRule[nRegle].SrSSchemaNat, name);
	   /* si on n'a pas trouve', on cherche dans les extensions du schema */
	   if (retour == NULL)
	      retour = ChSchStruct (pSS->SsNextExtens, name);
	}
   return retour;
}

/* ----------------------------------------------------------------------
   TtaGetSSchema

   Returns a structure schema whose name is known and that is used in a
   given document.

   Parameter:
   name: the name of the structure schema of interest.
   document: the document that uses this structure schema.

   Return value:
   the structure schema having this name, or NULL if this structure
   schema is not loaded or not used by the document.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
SSchema             TtaGetSSchema (char *name, Document document)

#else  /* __STDC__ */
SSchema             TtaGetSSchema (name, document)
char               *name;
Document            document;

#endif /* __STDC__ */

{
   SSchema             schema;

   UserErrorCode = 0;
   schema = NULL;
   if (name == NULL || name[0] == '\0')
     {
	TtaError (ERR_invalid_parameter);
     }
   else
      /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else if (TabDocuments[document - 1] == NULL)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else
      /* parametre document correct */
      /* on cherche a partir du schema principal du document */
      schema = ChSchStruct (TabDocuments[document - 1]->DocSSchema, name);
   return schema;
}


/* ----------------------------------------------------------------------
   TtaSameSSchemas

   Compares two structure schemas.

   Parameters:
   schema1: first structure schema.
   schema2: second structure schema.

   Return value:
   0 if both schemas are different, 1 if they are identical.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
int                 TtaSameSSchemas (SSchema schema1, SSchema schema2)

#else  /* __STDC__ */
int                 TtaSameSSchemas (schema1, schema2)
SSchema             schema1;
SSchema             schema2;

#endif /* __STDC__ */

{
   int                 result;

   UserErrorCode = 0;
   result = 0;
   if (schema1 == NULL || schema2 == NULL)
     {
	TtaError (ERR_invalid_parameter);
     }
   else if (((PtrSSchema) schema1)->SsCode ==
	    ((PtrSSchema) schema2)->SsCode)
      result = 1;
   return result;
}

/* ----------------------------------------------------------------------
   TtaGiveSchemasOfDocument

   Returns the names of the main structure schema and presentation schema
   associated with a given document. The document does not need to be open
   and the schemas are not loaded by this function.

   Parameters:
   documentName: Name of the document to be checked (maximum length
   19 characters).
   structureName: buffer.
   presentationName: buffer.

   Return value:
   structureName: Name of the document structure schema.
   presentationName: Name of the document presentation schema.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TtaGiveSchemasOfDocument (char *documentName, char *structureName, char *presentationName)

#else  /* __STDC__ */
void                TtaGiveSchemasOfDocument (documentName, structureName, presentationName)
char               *documentName;
char               *structureName;
char               *presentationName;

#endif /* __STDC__ */

{
   PathBuffer          DirBuffer;
   BinFile             file;
   char                texte[MAX_TXT_LEN];
   int                 i;
   boolean             error;
   char                carlu;
   LabelString         lab;
   int                 currentVersion = 0;

   UserErrorCode = 0;
   structureName[0] = '\0';
   presentationName[0] = '\0';
   /* compose le nom du fichier a ouvrir avec le nom du directory */
   /* des documents... */
   strncpy (DirBuffer, DirectoryDoc, MAX_PATH);
   BuildFileName (documentName, "PIV", DirBuffer, texte, &i);
   /* teste si le fichier existe */
   file = BIOreadOpen (texte);
   if (file == 0)
      /* fichier document inaccessible */
     {
	TtaError (ERR_cannot_open_pivot_file);
     }
   else
      /* lit le debut du fichier document */
     {
	error = False;
	/* lit le numero de version s'il est present */
	if (!BIOreadByte (file, &carlu))
	   error = True;
	if (carlu == (char) C_PIV_VERSION)
	  {
	     if (!BIOreadByte (file, &carlu))
		error = True;
	     if (!BIOreadByte (file, &carlu))
		error = True;
	     else
		currentVersion = (int) carlu;
	     if (!BIOreadByte (file, &carlu))
		error = True;
	  }
	/* lit le label max. du document s'il est present */
	if (!error && (carlu == (char) C_PIV_SHORT_LABEL || carlu == (char) C_PIV_LONG_LABEL ||
		       carlu == (char) C_PIV_LABEL))
	  {
	     rdLabel (carlu, lab, file);
	     if (!BIOreadByte (file, &carlu))
		error = True;
	  }

	if (currentVersion >= 4)
	  {
	     /* lit la table des langues utilisees par le document */
	     while (carlu == (char) C_PIV_LANG && !error)
	       {
		  do
		     if (!BIOreadByte (file, &carlu))
			error = True;
		  while (!(error || carlu == '\0')) ;
		  if (carlu != '\0')
		     error = True;
		  else
		     /* lit l'octet suivant le nom de langue */
		  if (!BIOreadByte (file, &carlu))
		     error = True;
	       }
	  }

	/* lit le commentaire du document s'il est present */
	if (!error && (carlu == (char) C_PIV_COMMENT || carlu == (char) C_PIV_OLD_COMMENT))
	  {
	     /* lit l'octet suivant le commentaire */
	     if (!BIOreadByte (file, &carlu))
		error = True;
	  }
	/* Lit le nom du schema de structure */
	/* qui est en tete du fichier pivot */
	if (!error && carlu != (char) C_PIV_NATURE)
	   error = True;
	if (!error)
	  {
	     i = 0;
	     do
		if (!BIOreadByte (file, &structureName[i++]))
		   error = True;
	     while (!(error || structureName[i - 1] == '\0' || i == MAX_NAME_LENGTH)) ;
	     if (structureName[i - 1] != '\0')
		error = True;
	     else
	       {
		  if (currentVersion >= 4)
		     /* Lit le code du schema de structure */
		     if (!BIOreadShort (file, &i))
			error = True;
		  if (!error)
		    {
		       /* Lit le nom du schema de presentation associe' */
		       i = 0;
		       do
			  if (!BIOreadByte (file, &presentationName[i++]))
			     error = True;
		       while (!(error || presentationName[i - 1] == '\0' || i == MAX_NAME_LENGTH)) ;
		    }
	       }
	  }
	BIOreadClose (file);
     }
}

/* ----------------------------------------------------------------------
   TtaNextSchemaExtension

   Returns a structure schema extension associated with a given document.

   Parameters:
   document: the document of interest.
   extension: a schema extension of that document. NULL for accessing
   the first schema extension.
   Return parameter:
   extension: the schema extension that follows or the first schema
   extension of the document if parameter extension is NULL.
   NULL if there is no more schema extension.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TtaNextSchemaExtension (Document document, SSchema * extension)

#else  /* __STDC__ */
void                TtaNextSchemaExtension (document, extension)
Document            document;
SSchema            *extension;

#endif /* __STDC__ */

{
   PtrSSchema        nextExtension;

   UserErrorCode = 0;
   /* verifie le parametre document */
   nextExtension = NULL;
   if (document < 1 || document > MAX_DOCUMENTS)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else if (TabDocuments[document - 1] == NULL)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else
      /* parametre document correct */
     {
	if (*extension == NULL)
	   nextExtension = TabDocuments[document - 1]->DocSSchema->SsNextExtens;
	else if (!(((PtrSSchema) (*extension))->SsExtension))
	   /* ce n'est pas un schema d'extension */
	  {
	     TtaError (ERR_invalid_parameter);
	  }
	else
	   nextExtension = ((PtrSSchema) (*extension))->SsNextExtens;
     }
   *extension = (SSchema) nextExtension;
}

/* ----------------------------------------------------------------------
   TtaNextNature

   Returns the structure schema of a nature used in a given document.

   Parameters:
   document: the document of interest.
   nature: the structure schema of a nature for that document. NULL for
   accessing the first nature.
   Return parameter:
   nature: the structure schema of the next nature, or the structure schema
   of the first nature of the document if parameter nature was NULL
   when calling.
   NULL if there is no more nature for the document.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TtaNextNature (Document document, SSchema * nature)

#else  /* __STDC__ */
void                TtaNextNature (document, nature)
Document            document;
SSchema            *nature;

#endif /* __STDC__ */

{
   PtrSSchema        nextNature;
   int                 n;
   PtrDocument         pDoc;
   boolean             found;

   UserErrorCode = 0;
   /* verifie le parametre document */
   nextNature = NULL;
   if (document < 1 || document > MAX_DOCUMENTS)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else if (TabDocuments[document - 1] == NULL)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else
      /* parametre document correct */
     {
	pDoc = TabDocuments[document - 1];
	if (*nature == NULL)
	   /* premiere demande, construit la table des natures du document */
	  {
	     BuildDocNatureTable (pDoc);
	     if (pDoc->DocNNatures > 1)
		/* on retournera la premiere entree de la table */
		n = 1;
	     else
		/* table des natures vide */
		n = 0;
	  }
	else
	   /* la table a deja ete construite */
	  {
	     /* on cherche l'entree courante dans la table */
	     n = 1;
	     found = False;
	     while ((n < pDoc->DocNNatures) && !found)
	       {
		  if (pDoc->DocNatureSSchema[n] == (PtrSSchema) (*nature))
		     found = True;
		  n++;
	       }
	     if (!found)
		n = 0;
	  }
	if (n > 0)
	  {
	     /* on saute les extensions de schemas */
	     found = False;
	     while ((n < pDoc->DocNNatures) && !found)
		if (!pDoc->DocNatureSSchema[n]->SsExtension)
		   found = True;
		else
		   n++;
	     if (found)
		nextNature = pDoc->DocNatureSSchema[n];
	  }
     }
   *nature = (SSchema) nextNature;
}

/* ----------------------------------------------------------------------
   TtaIsDocumentModified

   Indicates whether a document has been modified by the user or not.
   Modifications made by the application program are not considered,
   except when explicitely notified by TtaDocumentModified.

   Parameter:
   document: the document.

   Return value:
   1 if the document has been modified by the user since it has been saved,
   loaded or created, 0 if it has not been modified.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
int                 TtaIsDocumentModified (Document document)

#else  /* __STDC__ */
int                 TtaIsDocumentModified (document)
Document            document;

#endif /* __STDC__ */

{
   int                 modified;

   UserErrorCode = 0;
   /* verifie le parametre document */
   modified = 0;
   if (document < 1 || document > MAX_DOCUMENTS)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else if (TabDocuments[document - 1] == NULL)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else
      /* parametre document correct */
   if (TabDocuments[document - 1]->DocModified)
      modified = 1;
   return modified;
}

/* ----------------------------------------------------------------------
   TtaGetDocumentAccessMode

   Returns the access mode for a document.

   Parameter:
   document: the document whose access mode is asked.
   Return value:
   0 if access mode is read only, 1 if access mode is read-write.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
int                 TtaGetDocumentAccessMode (Document document)

#else  /* __STDC__ */
int                 TtaGetDocumentAccessMode (document)
Document            document;

#endif /* __STDC__ */

{
   int                 result;

   UserErrorCode = 0;
   result = 1;
   /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else if (TabDocuments[document - 1] == NULL)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else
      /* parametre document correct */
   if (TabDocuments[document - 1]->DocReadOnly)
      result = 0;
   else
      result = 1;
   return result;
}


/* ----------------------------------------------------------------------
   TtaGetDocumentBackUpInterval

   Returns backup interval for a document.

   Parameters:
   document: the document whose backup interval is asked.
   Return value:
   0 : the backup mechanism is disabled
   positive integer : number of typed characters which trigger an autamatic
   save of the document into a .BAK file.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
int                 TtaGetDocumentBackUpInterval (Document document)

#else  /* __STDC__ */
int                 TtaGetDocumentBackUpInterval (document)
Document            document;

#endif /* __STDC__ */

{
   int                 result;

   UserErrorCode = 0;
   result = 0;
   /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else if (TabDocuments[document - 1] == NULL)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else
      /* parametre document correct */
      result = TabDocuments[document - 1]->DocBackUpInterval;
   return result;
}


/* ----------------------------------------------------------------------
   TtaGetNotificationMode

   Returns the ECF notification mode for a document.

   Parameters:
   document: the document whose notification mode is asked.
   Return value:
   0 = if only roots of created and deleted subtrees must be notified,
   1 = all elements of created and deleted subtrees must be notified.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
int                 TtaGetNotificationMode (Document document)

#else  /* __STDC__ */
int                 TtaGetNotificationMode (document)
Document            document;

#endif /* __STDC__ */

{
   int                 result;

   UserErrorCode = 0;
   result = 0;
   /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else if (TabDocuments[document - 1] == NULL)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else
      /* parametre document correct */
   if (TabDocuments[document - 1]->DocNotifyAll)
      result = 1;
   else
      result = 0;
   return result;
}


/* ----------------------------------------------------------------------
   TtaGetDocumentPath

   Returns the current list of the directories used when a document is open
   (see TtaOpenDocument).

   Parameters:
   buffer: a buffer provided by the caller.
   bufferLength: the length of that buffer.

   Return parameter:
   buffer: the list of directories. Directory names are separated by
   the character PATH_SEP.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TtaGetDocumentPath (char *buffer, int bufferLength)

#else  /* __STDC__ */
void                TtaGetDocumentPath (buffer, bufferLength)
char               *buffer;
int                 bufferLength;

#endif /* __STDC__ */

{

   UserErrorCode = 0;
   if (strlen (DirectoryDoc) >= bufferLength)
      TtaError (ERR_buffer_too_small);
   strncpy (buffer, DirectoryDoc, bufferLength - 1);
}

/* ----------------------------------------------------------------------
   TtaGetSchemaPath

   Returns the current list of directories used for accessing schemas.

   Parameters:
   buffer: a buffer provided by the caller.
   bufferLength: the length of that buffer.

   Return parameter:
   buffer: the list of directories. Directory names are separated by
   the character PATH_SEP.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TtaGetSchemaPath (char *buffer, int bufferLength)

#else  /* __STDC__ */
void                TtaGetSchemaPath (buffer, bufferLength)
char               *buffer;
int                 bufferLength;

#endif /* __STDC__ */

{

   UserErrorCode = 0;
   if (strlen (DirectorySchemas) >= bufferLength)
      TtaError (ERR_buffer_too_small);
   strncpy (buffer, DirectorySchemas, bufferLength - 1);
}

#ifndef NODISPLAY
/* ----------------------------------------------------------------------
   TtaGetDocumentOfSavedElements

   Returns the document for which the last Copy or Cut command has been
   issued.

   Parameters:
   no parameter.

   Return value:
   the document for which the last Copy or Cut command has been issued.
   0 if the clipboard is empty.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
Document            TtaGetDocumentOfSavedElements ()

#else  /* __STDC__ */
Document            TtaGetDocumentOfSavedElements ()

#endif				/* __STDC__ */

{
   UserErrorCode = 0;
   if (DocDeSauve == NULL)
      return 0;
   else
      return IdentDocument (DocDeSauve);
}
#endif

/*
   DocToPtr retourne le PtrDocument correspondant a un Document
 */

#ifdef __STDC__
PtrDocument         DocToPtr (Document document)

#else  /* __STDC__ */
PtrDocument         DocToPtr (document)
Document            document;

#endif /* __STDC__ */

{
   return TabDocuments[document - 1];
}

/* end of module */
