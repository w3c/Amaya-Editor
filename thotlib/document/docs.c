/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
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
#include "writepivot_f.h"

/*----------------------------------------------------------------------
  TtaGetNextDocumentIndex

  Return value:
  the document that will be created or 0 by the next TtaInitDocument.
  ----------------------------------------------------------------------*/
Document TtaGetNextDocumentIndex ()
{
  Document        doc;

  /* look for the next free document */
  doc = 0;
  while (doc < MAX_DOCUMENTS && LoadedDocument[doc] != NULL)
    doc++;
  if (doc >= MAX_DOCUMENTS)
    {
      return 0;
    }
  else
    return doc + 1;
}

/*----------------------------------------------------------------------
  TtaInitDocument

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
  document: the requested document or 0.
  nspace: namespace used for the XML document

  Return value:
  the document that has been created or 0 if the document has not
  been created.
  ----------------------------------------------------------------------*/
Document TtaInitDocument (const char *structureSchema, const char *documentName,
                          const char *nspace, Document document)
{
  PtrDocument         pDoc;
  PtrElement          pEl;
  int                 i;
  
  UserErrorCode = 0;
  pDoc = NULL;
  if (documentName[0] == EOS)
    /* No name provided by the user */
    TtaError (ERR_document_name);
  else
    {
      /* initializes a document context */
      CreateDocument (&pDoc, &document);
      if (pDoc == NULL)
        /* No free context document */
        TtaError (ERR_too_many_documents);
      else
        {	  
          /* charge le schema de structure */
          if (structureSchema && !strcmp (structureSchema, "XML"))
            pDoc->DocSSchema = LoadStructureSchema (nspace, structureSchema, pDoc);
          else
            pDoc->DocSSchema = LoadStructureSchema (NULL, structureSchema, pDoc);
          if (pDoc->DocSSchema == NULL || pDoc->DocSSchema->SsExtension)
            /* failure while reading the structure schema or while loading
               a schema extension */
            {
              if (pDoc->DocSSchema)
                {
                  ReleaseStructureSchema (pDoc->DocSSchema, pDoc);
                  pDoc->DocSSchema = NULL;
                }
              UnloadDocument (&pDoc);
              TtaError (ERR_cannot_read_struct_schema);
            }
          else
            {
              /* The structure schema is loaded */
              if (strcmp (structureSchema, "XML") == 0)
                pDoc->DocSSchema->SsIsXml = TRUE;
#ifndef NODISPLAY
              InitApplicationSchema (pDoc->DocSSchema);
#endif
              /* One create the internal representation of an empty document */
              pDoc->DocDocElement = NewSubtree (pDoc->DocSSchema->SsDocument,
                                                pDoc->DocSSchema, pDoc,
                                                FALSE, TRUE, TRUE, TRUE);
              pEl = NewSubtree (pDoc->DocSSchema->SsRootElem, pDoc->DocSSchema,
                                pDoc, TRUE, TRUE, TRUE, TRUE);
              /* suppress excluded elements */
              if (pEl)
                RemoveExcludedElem (&pEl, pDoc);
              if (pEl == NULL)
                {
                  DeleteElement (&pDoc->DocDocElement, pDoc);
                  UnloadDocument (&pDoc);
                  TtaError (ERR_empty_document);
                }
              else
                {
                  InsertFirstChild (pDoc->DocDocElement, pEl);
                  pDoc->DocDocElement->ElAccess = ReadWrite;
#ifndef NODISPLAY
                  /* Create required attributes for the whole created tree */
                  AttachMandatoryAttributes (pDoc->DocDocElement, pDoc);
#endif
                  /* An attribut Language is stored in the root */
                  CheckLanguageAttr (pDoc, pEl);
                  /* The document is named */
                  strncpy (pDoc->DocDName, documentName, MAX_NAME_LENGTH);
                  pDoc->DocDName[MAX_NAME_LENGTH - 1] = EOS;
                  /* keep the actual schema path in the document context */
                  strncpy (pDoc->DocSchemasPath, SchemaPath, MAX_PATH);
                  /* initializes the directory of the document */
                  strncpy (pDoc->DocDirectory, DocumentPath, MAX_PATH);
                  /* if path, keep only the first directory */
                  i = 1;
                  while (pDoc->DocDirectory[i - 1] != EOS &&
                         pDoc->DocDirectory[i - 1] != PATH_SEP &&
                         i < MAX_PATH)
                    i++;
                  pDoc->DocDirectory[i - 1] = EOS;
                  /* Read-Write document */
                  pDoc->DocReadOnly = FALSE;
                }
            }
        }
    }
  return document;
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
Document TtaNewDocument (const char *structureSchema, const char *documentName)
{
  return TtaInitDocument (structureSchema, documentName, structureSchema, 0);
}

/*----------------------------------------------------------------------
  NewDocument cree un document vide, conforme au schema de nom    
  SSchemaName, dans le descripteur pointe' par pDoc.      
  docName est le nom a donner au document                 
  directory est le directory ou il faut creer le document 
  Au retour pDoc est NIL si le document n'a pas ete cree. 
  ----------------------------------------------------------------------*/
void NewDocument (PtrDocument *pDoc, char *SSchemaName, char *docName,
                  PathBuffer directory)
{
  PtrElement          pEl;
  NotifyDialog        notifyDoc;
  char                PSchemaName[MAX_NAME_LENGTH]; 
  char                docNameBuffer[MAX_NAME_LENGTH]; 
  char                docType[MAX_NAME_LENGTH];
  PathBuffer          directoryBuffer;
  PathBuffer          fileNameBuffer;
  PtrPSchema          pPSchema;
  int                 view;
  int                 i;

  if (*pDoc != NULL)
    {
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
          /*ConfigSSchemaInternalName (SSchemaName, docType, TRUE);
            if (docType[0] == EOS)*/
          /* ce nom n'est pas dans le fichier langue, on le prend */
          /* tel quel */
          strncpy (docType, SSchemaName, MAX_NAME_LENGTH);
          /* compose le nom du fichier a ouvrir avec le nom du directory */
          /* des schemas... */
          strncpy (directoryBuffer, SchemaPath, MAX_PATH);
          MakeCompleteName (docType, "STR", directoryBuffer,
                            fileNameBuffer, &i);
          pEl = NULL;
          /* teste si le fichier '.STR' existe */
          if (!TtaFileExist (fileNameBuffer))
            {
              strncpy (fileNameBuffer, docType, MAX_NAME_LENGTH);
              strcat (fileNameBuffer, ".STR");
            }
          else
            {
              /* charge le schema de structure et le schema de presentation */
              PSchemaName[0] =EOS;
              /* pas de preference pour un schema de */
              /* presentation particulier */
              LoadSchemas (docType, PSchemaName, &((*pDoc)->DocSSchema), *pDoc,
                           NULL, FALSE);
              if (docName[0] != EOS)
                strncpy (docNameBuffer, docName, MAX_NAME_LENGTH);
              else
                {
                  strncpy (docNameBuffer, SSchemaName, MAX_NAME_LENGTH);
                  strcat (docNameBuffer, "X");
                }
              if ((*pDoc)->DocSSchema != NULL)
                if (PresentationSchema ((*pDoc)->DocSSchema, *pDoc) != NULL)
                  {
                    notifyDoc.event = TteDocCreate;
                    notifyDoc.document = (Document) IdentDocument (*pDoc);
                    notifyDoc.view = 0;
                    if (!CallEventType ((NotifyEvent *) & notifyDoc, TRUE))
                      {
                        /* cree la representation interne d'un document vide */
                        (*pDoc)->DocDocElement = NewSubtree ((*pDoc)->DocSSchema->SsDocument, (*pDoc)->DocSSchema, *pDoc, FALSE, TRUE, TRUE, TRUE);
                        pEl = NewSubtree ((*pDoc)->DocSSchema->SsRootElem,
                                          (*pDoc)->DocSSchema, *pDoc, TRUE, TRUE, TRUE, TRUE);
                        /* suppress excluded elements  */
                        RemoveExcludedElem (&pEl, *pDoc);
                        if (pEl)
                          InsertFirstChild ((*pDoc)->DocDocElement, pEl);
                      }
                  }
            }
          if (pEl == NULL)
            /* echec creation document */
            {
              if ((*pDoc)->DocDocElement)
                DeleteElement (&((*pDoc)->DocDocElement), *pDoc);
              UnloadDocument (pDoc);
            }
          else
            {
              (*pDoc)->DocDocElement->ElAccess = ReadWrite;
              CheckLanguageAttr (*pDoc, pEl);
              /* ajoute un saut de page a la fin de l'arbre principal */
              /* pour toutes les vues qui sont mises en page */
              /* schema de presentation du document */
              pPSchema = PresentationSchema ((*pDoc)->DocSSchema, *pDoc);
              /* examine toutes les vues definies dans le schema */
              for (view = 0; view < pPSchema->PsNViews; view++)
                if (pPSchema->PsPaginatedView[view])
                  /* cette vue est mise en page */
                  AddLastPageBreak ((*pDoc)->DocDocElement, view + 1, *pDoc,
                                    TRUE);
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
              FindCompleteName (docNameBuffer, "PIV", directoryBuffer,
                                fileNameBuffer, &i);
              strncpy ((*pDoc)->DocDName, docNameBuffer, MAX_NAME_LENGTH);
              (*pDoc)->DocDName[MAX_NAME_LENGTH - 1] = EOS;
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
              AttachMandatoryAttributes (pEl, *pDoc);
              if ((*pDoc)->DocSSchema != NULL)
                /* le document n'a pas ete ferme' pendant l'attente */
                /* des attributs requis */
                {
                  /* ouvre les vues du document cree' */
                  OpenDefaultViews (*pDoc);
                  /* selectionne la 1ere feuille du document */
                  pEl = FwdSearchTypedElem ((*pDoc)->DocDocElement,
                                            (*pDoc)->DocSSchema->SsRootElem,
                                            (*pDoc)->DocSSchema, NULL);
                  if (pEl)
                    {
                      pEl = FirstLeaf (pEl);
                      SelectElement (*pDoc, pEl, TRUE, TRUE, TRUE);
                    }
                }
            }
        }
    }
}

/*----------------------------------------------------------------------
  PaginateDocument	pagine toutes les vues du document pDoc		
  ----------------------------------------------------------------------*/
void                PaginateDocument (PtrDocument pDoc)
{
  AvailableView       viewList;
  int                 i, nViews, docView;
  ThotBool            found;

  nViews = BuildDocumentViewList (pDoc, viewList);
  for (i = 0; i < nViews; i++)
    if (viewList[i].VdOpen && viewList[i].VdPaginated &&
        !viewList[i].VdNature)
      /* c'est une vue a paginer */
      {
        /* cherche le numero de vue dans le document */
        found = FALSE;
        for (docView = 0; docView < MAX_VIEW_DOC && !found; docView++)
          if (pDoc->DocView[docView].DvPSchemaView == viewList[i].VdView &&
              pDoc->DocView[docView].DvSSchema == viewList[i].VdSSchema)
            found = TRUE;
        /* pagine la vue */
        PaginateView (pDoc, docView);
      }
}

/*----------------------------------------------------------------------
  UpdateIncludedElement met a` jour et reaffiche l'element pEl inclus dans  
  le document pDoc.                                       
  ----------------------------------------------------------------------*/
void UpdateIncludedElement (PtrElement pEl, PtrDocument pDoc)
{
  PtrElement          pChild, pNext;
  PtrTextBuffer       pBuf, pNextBuf;
  PtrPathSeg          pPa, pNextPa;
  int                 view;
  ThotBool            ToCreate[MAX_VIEW_DOC];

  /* conserve la liste des vues ou l'element a des paves */
  for (view = 0; view < MAX_VIEW_DOC; view++)
    ToCreate[view] = pEl->ElAbstractBox[view] != NULL;
  /* detruit les paves de l'element */
  DestroyAbsBoxes (pEl, pDoc, FALSE);
  /* Update Abstract views */
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
      case LtPath:
        pPa = pEl->ElFirstPathSeg;
        while (pPa)
          {
            pNextPa = pPa->PaNext;
            FreePathSeg (pPa);
            pPa = pNextPa;
          }
        pEl->ElFirstPathSeg = NULL;
        pEl->ElVolume = 0;
        break;
      case LtSymbol:
      case LtGraphics:
        pEl->ElGraph = EOS;
        pEl->ElWideChar = 0;
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
  for (view = 0; view < MAX_VIEW_DOC; view++)
    if (ToCreate[view])
      {
        pDoc->DocViewFreeVolume[view] = pDoc->DocViewVolume[view];
        CreateNewAbsBoxes (pEl, pDoc, view + 1);
      }
  ApplDelayedRule (pEl, pDoc);
  /* reaffiche l'element dans toutes les vues ou il existe */
  /* Update Abstract views */
  AbstractImageUpdated (pDoc);
  /* Redisplay views */
  RedisplayDocViews (pDoc);
  /* Reaffiche les numeros suivants qui changent */
  UpdateNumbers (NextElement (pEl), pEl, pDoc, TRUE);
}

/*----------------------------------------------------------------------
  UpdateAllInclusions updates all inclusion elements of a document
  ----------------------------------------------------------------------*/
void UpdateAllInclusions (PtrDocument pDoc)
{
  PtrReference        pRef;
  PtrReferredDescr    pRefD;
  ThotBool            updated = FALSE;

  /* check all reference descriptors */
  pRefD = pDoc->DocReferredEl;
  if (pRefD)
    /* skip the first empty descriptor */
    pRefD = pRefD->ReNext;
  while (pRefD)
    /* referred element are within the document */
    {
      pRef = NULL;
      /* look for referred elements from external documents */
      do
        {
          pRef = NextReferenceToEl (pRefD->ReReferredElem, pDoc, pRef);
          if (pRef && pRef->RdTypeRef == RefInclusion &&
              pRef->RdElement && pRef->RdElement->ElSource)
            /* inclusion with expansion */
            {
              /* located in another document: switch off the selection */
              /* get a new copy */
              updated = TRUE;
              UpdateIncludedElement (pRef->RdElement, pDoc);
            }
        }
      while (pRef);

      /* next descriptor */
      if (pRefD != NULL)
        pRefD = pRefD->ReNext;
    }

  if (updated && pDoc == SelectedDocument)
    /* switch on the selection */
    HighlightSelection (FALSE, TRUE);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void RemoveExtensionFromTree (PtrElement * pEl, Document document,
                                     PtrSSchema pSSExt, int *removedElements,
                                     int *removedAttributes)
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

  Removes a structure schema extension from a given document.
  Removes also from the document all attributes and elements defined in
  that structure schema extension.
  Parameters:
  document: the document.
  extension: the structure schema extension to be removed.
  Return parameters:
  removedElements: number of elements actually removed.
  removedAttributes: number of attributes actually removed.
  ----------------------------------------------------------------------*/
void TtaRemoveSchemaExtension (Document document, SSchema extension,
                               int *removedElements, int *removedAttributes)
{
  PtrSSchema          curExtension, previousSSchema;
  PtrElement          root;
  PtrDocument         pDoc;
  ThotBool            found;
#ifndef NODISPLAY
  PtrPSchema          pPS;
#endif

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
        if (!strcmp (((PtrSSchema) extension)->SsName, curExtension->SsName))
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
          root = pDoc->DocDocElement;
          if (root != NULL)
            RemoveExtensionFromTree (&root, document, (PtrSSchema) extension,
                                     removedElements, removedAttributes);
          previousSSchema->SsNextExtens = curExtension->SsNextExtens;
          if (curExtension->SsNextExtens != NULL)
            curExtension->SsNextExtens->SsPrevExtens = previousSSchema;
#ifndef NODISPLAY
          pPS = PresentationSchema (curExtension, pDoc);
          FreePresentationSchema (pPS, curExtension, pDoc);
#endif
          ReleaseStructureSchema (curExtension, pDoc);
        }
    }
}

/*----------------------------------------------------------------------
  BackupAll sauvegarde les fichiers modifies en cas de CRASH majeur
  ----------------------------------------------------------------------*/
void BackupAll()
{
  int             doc;

  fprintf (stderr, TtaGetMessage (LIB, TMSG_DEBUG_SAV_FILES));
  /* parcourt la table des documents */
  for (doc = 0; doc < MAX_DOCUMENTS; doc++)
    if (LoadedDocument[doc] != NULL)
      /* il y a un document pour cette entree de la table */
      if (LoadedDocument[doc]->DocModified)
        if (ThotLocalActions[T_writedocument] != NULL)
          (*(Proc2)ThotLocalActions[T_writedocument]) (
                                                       (void *)LoadedDocument[doc],
                                                       (void *)3);
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
