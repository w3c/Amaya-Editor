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
 
/*                                                              */
/*      Ce module implemente les fonctions de l'API Thot qui    */
/*      permettent de lire ou d'ecrire certaines parties de     */
/*      la forme pivot des documents.                           */
/*                                                              */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "fileaccess.h"
#include "application.h"
#include "document.h"
#include "tree.h"
#include "attribute.h"
#include "presentation.h"
#include "constpiv.h"
#include "labelAllocator.h"

#undef THOT_EXPORT
#define THOT_EXPORT
#include "edit_tv.h"

#include "applicationapi_f.h"
#include "callback_f.h"
#include "externalref_f.h"
#include "fileaccess_f.h"
#include "labelalloc_f.h"
#include "readpivot_f.h"
#include "structschema_f.h"
#include "thotmsg_f.h"
#include "tree_f.h"
#include "writepivot_f.h"


/*----------------------------------------------------------------------
   TtaAllocateDocument

   Allocates a new document context.

   Parameter:
   documentName: name of the document to be created (maximum length
                 19 characters). The directory name is not part of
                 this parameter (see TtaSetDocumentPath).
   documentIdentifier: internal/external document identifier.
   documentSchemasPath: Path des schemas du document.

   Return value:
   the allocated document.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
Document            TtaAllocateDocument (STRING documentName,
                                         STRING documentIdentifier,
                                         STRING documentSchemasPath)

#else  /* __STDC__ */
Document            TtaAllocateDocument (documentName,
                                         documentIdentifier,
                                         documentSchemasPath)
STRING              documentName;
STRING              documentIdentifier;
STRING              documentSchemasPath;

#endif /* __STDC__ */

{
   Document            doc;
   PtrDocument         pDoc;

   UserErrorCode = 0;
   pDoc = NULL;
   doc = 0;
   CreateDocument (&pDoc);
   if (pDoc == NULL)
      TtaError (ERR_too_many_documents);
   else
     {
	pDoc->DocSSchema = NULL;
	pDoc->DocRootElement = NULL;
	pDoc->DocLabels = NULL;
	/* on donne son nom au document */
	ustrncpy (pDoc->DocDName, documentName, MAX_NAME_LENGTH);
	pDoc->DocDName[MAX_NAME_LENGTH - 1] = EOS;
	/* on acquiert un identificateur pour le document */
	GetDocIdent (&pDoc->DocIdent, documentIdentifier);
        /* on stocke le path de schemas du document */
        ustrncpy (pDoc->DocSchemasPath,
                 documentSchemasPath,
                 MAX_PATH);
	/* document en lecture-ecriture */
	pDoc->DocReadOnly = FALSE;
	doc = IdentDocument (pDoc);
     }
   return doc;
}


/*----------------------------------------------------------------------
   TtaWritePivotHeader

   Writes the header of a pivot file.

   Parameters:
   pivotFile: the pivot file. This file must be open.
   document: the document for which the pivot file is written.

  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                TtaWritePivotHeader (BinFile pivotFile, Document document)

#else  /* __STDC__ */
void                TtaWritePivotHeader (pivotFile, document)
BinFile             pivotFile;
Document            document;

#endif /* __STDC__ */

{
   UserErrorCode = 0;
   /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parametre document correct */
      WritePivotHeader (pivotFile, LoadedDocument[document - 1]);
}


/*----------------------------------------------------------------------
   TtaReadPivotHeader

   Reads the header of a pivot file.

   Parameters:
   pivotFile: the pivot file. This file must be open.
   document: the document for which the header is read.

   Return parameter:
   nextChar: first byte that follows the header in the pivot file.

  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                TtaReadPivotHeader (BinFile pivotFile, Document document, PCHAR_T nextChar)

#else  /* __STDC__ */
void                TtaReadPivotHeader (pivotFile, document, nextChar)
BinFile             pivotFile;
Document            document;
PCHAR_T               nextChar;

#endif /* __STDC__ */

{
   UserErrorCode = 0;
   /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parametre document correct */
      ReadPivotHeader (pivotFile, LoadedDocument[document - 1], nextChar);
}


/*----------------------------------------------------------------------
   TtaWriteLanguageTable

   Writes into a pivot file the names of all languages used in a document.

   Parameters:
   pivotFile: the pivot file. This file must be open and at the
   correct position.
   document: the document for which the language names must be written.

  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                TtaWriteLanguageTable (BinFile pivotFile, Document document)

#else  /* __STDC__ */
void                TtaWriteLanguageTable (pivotFile, document)
BinFile             pivotFile;
Document            document;

#endif /* __STDC__ */

{
   UserErrorCode = 0;
   /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parametre document correct */
      WriteTableLangues (pivotFile, LoadedDocument[document - 1]);
}


/*----------------------------------------------------------------------
   TtaReadLanguageTable

   Reads from a pivot file the names of all languages used in a document.
   The language names read are stored in the context of the document.

   Parameters:
   pivotFile: the pivot file. This file must be open and at the correct
   position.
   document: the document for which the languages are read.

   Return parameter:
   nextChar: first byte that follows the language names in the pivot file.

  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                TtaReadLanguageTable (BinFile pivotFile, Document document, PCHAR_T nextChar)

#else  /* __STDC__ */
void                TtaReadLanguageTable (pivotFile, document, nextChar)
BinFile             pivotFile;
Document            document;
PCHAR_T               nextChar;

#endif /* __STDC__ */

{
   UserErrorCode = 0;
   /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parametre document correct */
      ReadLanguageTablePiv (pivotFile, LoadedDocument[document - 1], nextChar);
}

/*----------------------------------------------------------------------
   TtaWriteSchemaNames

   Writes into a pivot file the names of all structure schemas and presentation
   schemas used by a document.

   Parameters:
   pivotFile: the pivot file. This file must be open and at the
   correct position.
   document: the document.

  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                TtaWriteSchemaNames (BinFile pivotFile, Document document)

#else  /* __STDC__ */
void                TtaWriteSchemaNames (pivotFile, document)
BinFile             pivotFile;
Document            document;

#endif /* __STDC__ */

{
   UserErrorCode = 0;
   /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parametre document correct */
      WriteNomsSchemasDoc (pivotFile, LoadedDocument[document - 1]);
}


/*----------------------------------------------------------------------
   TtaReadSchemaNames

   Reads from a pivot file the names of all structure schemas and presentation
   schemas used by a document. The names read are stored in the context of the
   document.

   Parameters:
   pivotFile: the pivot file. This file must be open and at the correct
   position.
   document: the document.

   Return parameter:
   nextChar: first byte that follows the schema names in the pivot file.

  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                TtaReadSchemaNames (BinFile pivotFile, Document document, PCHAR_T nextChar, void (*withThisPSchema) (Document document, STRING natSchema, STRING presentSchema))

#else  /* __STDC__ */
void                TtaReadSchemaNames (pivotFile, document, nextChar, withThisPSchema)
BinFile             pivotFile;
Document            document;
PCHAR_T               nextChar;
void (*withThisPSchema) ();

#endif /* __STDC__ */

{
   UserErrorCode = 0;
   /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parametre document correct */
      ReadSchemaNamesPiv (pivotFile, LoadedDocument[document - 1], nextChar, NULL, withThisPSchema);
}

/*----------------------------------------------------------------------
   TtaReadAttribute

   Reads an attribute from a pivot file.
   ATTENTION: TtaReadAttribute uses the nature table of the document.

   Parameters:
   pivotFile: the pivot file. This file must be open and the current position
   must be after the MAttribute byte introducing the attribute to be
   read. When returning, the current position is the next byte that
   follows the attribute read, and the attribute is associated with
   the element.
   element: the element for which the attribute is read.
   document: the document corresponding to the pivot file.
   create: 1 means that an attribute must be created, 0 means that the
   attribute must just be skipped in the pivot file.

   Return parameter:
   attribute: the attribute that has been read if create = 1.

  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                TtaReadAttribute (BinFile pivotFile, Element element, Document document,
				      ThotBool create, Attribute * attribute)

#else  /* __STDC__ */
void                TtaReadAttribute (pivotFile, element, document, create, attribute)
BinFile             pivotFile;
Element             element;
Document            document;
ThotBool            create;
Attribute          *attribute;

#endif /* __STDC__ */

{
   PtrAttribute        pAttr;

   UserErrorCode = 0;
   /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parametre document correct */
      ReadAttributePiv (pivotFile, (PtrElement) element,
			LoadedDocument[document - 1], create,
			(PtrAttribute *) attribute, &pAttr);
}

/*----------------------------------------------------------------------
   TtaReadPRule

   Reads a presentation rule from a pivot file.

   Parameters:
   pivotFile: the pivot file. This file must be open and the current position
   must be after the C_PIV_PRESENT byte introducing the presentation
   rule to be read. When returning, the current position is the next
   byte that follows the presentation rule read.
   element: the element for which the presentation rule is read.
   create: 1 means that a presentation rule must be created and associated
   with the element, 0 means that the presentation rule must just be
   skipped in the pivot file.

   Return parameter:
   pRule: the presentation rule that has been read if create = 1.

  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                TtaReadPRule (BinFile pivotFile, Element element, Document document, ThotBool create, PRule * pRule)

#else  /* __STDC__ */
void                TtaReadPRule (pivotFile, element, document, create, pRule)
BinFile             pivotFile;
Element             element;
Document            document;
ThotBool            create;
PRule              *pRule;

#endif /* __STDC__ */

{
   UserErrorCode = 0;
   /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parametre document correct */
      ReadPRulePiv (LoadedDocument[document - 1], pivotFile,
		    (PtrElement) element, create, (PtrPRule *) pRule, TRUE);
}

/*----------------------------------------------------------------------
   TtaReadTree

   Reads a tree from a pivot file.

   Parameters:
   pivotFile: the pivot file. This file must be open and the current position
   must be after the C_PIV_TYPE or MClass byte introducing the tree
   to be read. When returning, the current position is the next
   byte that follows the tree read.
   element: parent element of the tree to be read.
   document: the document corresponding to the pivot file.
   byte: the C_PIV_TYPE or MClass byte introducing the tree to be read.

   Return parameters:
   byte: the next byte that follows the tree read.
   elementRead: the root of the tree that has been read.

  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                TtaReadTree (BinFile pivotFile, SSchema pSchema, Element element, Document document, PCHAR_T byte, Element * elementRead)

#else  /* __STDC__ */
void                TtaReadTree (pivotFile, pSchema, element, document, byte, elementRead)
BinFile             pivotFile;
SSchema          pSchema;
Element             element;
Document            document;
PCHAR_T               byte;
Element            *elementRead;

#endif /* __STDC__ */

{
   int                 TypeCont;
   PtrSSchema          pSchStrCont;
   int                 NumAssoc;
   PtrSSchema          pSS;
   int                 TypeLu;
   PtrSSchema          SchStrLu;

   UserErrorCode = 0;
   /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parametre document correct */
     {
	TypeCont = 0;
	pSchStrCont = NULL;
        if (pSchema != NULL)
          {
             NumAssoc = 0;
             pSS = (PtrSSchema) pSchema;
          }
	else if (element == NULL)
	  {
	     NumAssoc = 0;
	     pSS = LoadedDocument[document - 1]->DocSSchema;
	  }
	else
	  {
	     NumAssoc = ((PtrElement) element)->ElAssocNum;
	     pSS = ((PtrElement) element)->ElStructSchema;
	  }
	*elementRead = (Element) ReadTreePiv (pivotFile, pSS,
			       LoadedDocument[document - 1], byte, NumAssoc,
			      FALSE, TRUE, &TypeCont, &pSchStrCont, &TypeLu,
			       &SchStrLu, TRUE, (PtrElement) element, TRUE);
	if (*elementRead != NULL)
         SendEventAttrRead ((PtrElement)(*elementRead), 
                            LoadedDocument[document - 1]);
     }
}


/*----------------------------------------------------------------------
   TtaWriteTree

   Writes a tree into a pivot file.

   Parameters:
   pivotFile: the pivot file. This file must be open and the tree will
   be written at the current position.
   element: root element of the tree to be written.
   document: the document to which the tree belongs.

  ----------------------------------------------------------------------*/


#ifdef __STDC__
void                TtaWriteTree (BinFile pivotFile, Element element, Document document)

#else  /* __STDC__ */
void                TtaWriteTree (pivotFile, element, document)
BinFile             pivotFile;
Element             element;
Document            document;

#endif /* __STDC__ */

{
   PtrElement          pEl;
   NotifyElement       notifyEl;

   UserErrorCode = 0;
   if (element == NULL)
      TtaError (ERR_invalid_parameter);
   else
      /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parametre document correct */
     {
	pEl = (PtrElement) element;
	/* envoie le message ElemSave.Pre a l'application, si */
	/* elle le demande */
	notifyEl.event = TteElemSave;
	notifyEl.document = document;
	notifyEl.element = element;
	notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
	notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
	notifyEl.position = 0;
	if (!CallEventType ((NotifyEvent *) & notifyEl, TRUE))
	   /* l'application accepte que Thot sauve l'element */
	  {
	     /* Ecrit d'abord le numero de la structure generique s'il y */
	     /* a changement de schema de structure par rapport au pere */
	     if (pEl->ElParent != NULL)
		if (pEl->ElParent->ElStructSchema != pEl->ElStructSchema)
		   EcritNat (pEl->ElStructSchema, pivotFile, LoadedDocument[document - 1]);
	     Externalise (pivotFile, &pEl, LoadedDocument[document - 1], TRUE);
	     /* envoie le message ElemSave.Post a l'application, si */
	     /* elle le demande */
	     notifyEl.event = TteElemSave;
	     notifyEl.document = document;
	     notifyEl.element = element;
	     notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
	     notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
	     notifyEl.position = 0;
	     CallEventType ((NotifyEvent *) & notifyEl, FALSE);
	  }
     }
}


/*----------------------------------------------------------------------
   TtaWriteElement

   Writes a single element into a pivot file. The descendants of that element
   are not written.

   Parameters:
   pivotFile: the pivot file. This file must be open and the element will
   be written at the current position.
   element: element to be written.
   document: the document to which the element belongs.

  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                TtaWriteElement (BinFile pivotFile, Element element, Document document)

#else  /* __STDC__ */
void                TtaWriteElement (pivotFile, element, document)
BinFile             pivotFile;
Element             element;
Document            document;

#endif /* __STDC__ */

{
   PtrElement          pEl;
   NotifyElement       notifyEl;

   UserErrorCode = 0;
   if (element == NULL)
      TtaError (ERR_invalid_parameter);
   else
      /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parametre document correct */
     {
	pEl = (PtrElement) element;
	/* envoie le message ElemSave.Pre a l'application, si */
	/* elle le demande */
	notifyEl.event = TteElemSave;
	notifyEl.document = document;
	notifyEl.element = element;
	notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
	notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
	notifyEl.position = 0;
	if (!CallEventType ((NotifyEvent *) & notifyEl, TRUE))
	   /* l'application accepte que Thot sauve l'element */
	  {
	     /* Ecrit d'abord le numero de la structure generique s'il y */
	     /* a changement de schema de structure par rapport au pere */
	     if (pEl->ElParent != NULL)
		if (pEl->ElParent->ElStructSchema != pEl->ElStructSchema)
		   EcritNat (pEl->ElStructSchema, pivotFile, LoadedDocument[document - 1]);
	     Externalise (pivotFile, &pEl, LoadedDocument[document - 1], FALSE);
	     /* envoie le message ElemSave.Post a l'application, si */
	     /* elle le demande */
	     notifyEl.event = TteElemSave;
	     notifyEl.document = document;
	     notifyEl.element = element;
	     notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
	     notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
	     notifyEl.position = 0;
	     CallEventType ((NotifyEvent *) & notifyEl, FALSE);
	  }
     }
}


/*----------------------------------------------------------------------
   TtaReadPivotVersion

   Reads the pivot version number from a pivot file. The version number read
   is stored in the context of the document.

   Parameters:
   pivotFile: the pivot file. This file must be open and at the position
   where a version number is expected. When returning, the current
   position in the pivot file is just after the version number that
   has been read.
   document: the document for which the pivot version number is read.

  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                TtaReadPivotVersion (BinFile pivotFile, Document document)

#else  /* __STDC__ */
void                TtaReadPivotVersion (pivotFile, document)
BinFile             pivotFile;
Document            document;

#endif /* __STDC__ */

{
   UserErrorCode = 0;
   /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parametre document correct */
      UserErrorCode = ReadVersionNumberPiv (pivotFile, LoadedDocument[document - 1]);
}


/*----------------------------------------------------------------------
   TtaWritePivotVersion

   Writes the current pivot version number into a pivot file.

   Parameters:
   pivotFile: the pivot file. This file must be open and the pivot
   version number will be written at the current position.
   document: the document for which the pivot version number is written.

  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                TtaWritePivotVersion (BinFile pivotFile, Document document)

#else  /* __STDC__ */
void                TtaWritePivotVersion (pivotFile, document)
BinFile             pivotFile;
Document            document;

#endif /* __STDC__ */

{
   UserErrorCode = 0;
   /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parametre document correct */
      WriteVersionNumber (pivotFile);
}


/*----------------------------------------------------------------------
   TtaReadLabel

   Reads a label from a pivot file.

   Parameters:
   pivotFile: the pivot file. This file must be open and the current
   position must be just after a C_PIV_SHORT_LABEL, MLongLabel or MNameLabel
   byte. When returning, the current position is just after the
   label that has been read.
   byte: the C_PIV_SHORT_LABEL, MLongLabel or MNameLabel byte that precedes the label
   to be read.

   Return parameter:
   labelRead: the label that has been read.

  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                TtaReadLabel (BinFile pivotFile, CHAR_T byte, STRING labelRead)

#else  /* __STDC__ */
void                TtaReadLabel (pivotFile, byte, labelRead)
BinFile             pivotFile;
CHAR_T                byte;
STRING              labelRead;

#endif /* __STDC__ */

{
   UserErrorCode = 0;
   ReadLabel (byte, labelRead, pivotFile);
}


/*----------------------------------------------------------------------
   TtaWriteLabel

   Writes a label into a pivot file.

   Parameters:
   pivotFile: the pivot file. This file must be open and the label will
   be written at the current position.
   label: the label to be written.

  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                TtaWriteLabel (BinFile pivotFile, STRING label)

#else  /* __STDC__ */
void                TtaWriteLabel (pivotFile, label)
BinFile             pivotFile;
STRING              label;

#endif /* __STDC__ */

{
   UserErrorCode = 0;
   PutLabel (pivotFile, label);
}


/*----------------------------------------------------------------------
   TtaNewElementWithLabel

   Creates a new element of a given type.

   Parameters:
   document: the document for which the element is created.
   elemType: type of the element to be created.
   label: label of the element to be created. PcEmpty string if the value
   of the label is undefined.

   Return value:
   the created element.

  ----------------------------------------------------------------------*/

#ifdef __STDC__
Element             TtaNewElementWithLabel (Document document, ElementType elemType, STRING label)

#else  /* __STDC__ */
Element             TtaNewElementWithLabel (document, elemType, label)
Document            document;
ElementType         elemType;
STRING              label;

#endif /* __STDC__ */

{
   Element             element;

   UserErrorCode = 0;
   element = NULL;
   if (elemType.ElSSchema == NULL)
      TtaError (ERR_invalid_parameter);
   else
      /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parametre document correct */
      if (elemType.ElTypeNum < 1 ||
	  elemType.ElTypeNum > ((PtrSSchema) (elemType.ElSSchema))->SsNRules)
      TtaError (ERR_invalid_element_type);
   else
     {
	element = (Element) NewSubtree (elemType.ElTypeNum, (PtrSSchema) (elemType.ElSSchema),
					LoadedDocument[document - 1], 0, FALSE, TRUE, TRUE, (ThotBool)(*label == EOS));
	if (*label != EOS)
	   ustrncpy (((PtrElement) element)->ElLabel, label, MAX_LABEL_LEN);
     }
   return element;
}


/*----------------------------------------------------------------------
   TtaGetLabelMax

   Returns the value of the highest label used in a document.

   Parameter:
   document: the document.

   Return value:
   highest label used in the document.

  ----------------------------------------------------------------------*/

#ifdef __STDC__
int                 TtaGetLabelMax (Document document)

#else  /* __STDC__ */
int                 TtaGetLabelMax (document)
Document            document;

#endif /* __STDC__ */

{
   int                 label;

   UserErrorCode = 0;
   label = 0;
   /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parametre document correct */
     {
	label = GetCurrentLabel (LoadedDocument[document - 1]);
     }
   return label;
}

/*----------------------------------------------------------------------
   TtaSetLabelMax

   Sets the value of the next label to be created in a document.

   Parameters:
   document: the document.
   label: value of the next label to be created.

  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                TtaSetLabelMax (Document document, int label)

#else  /* __STDC__ */
void                TtaSetLabelMax (document, label)
Document            document;
int                 label;

#endif /* __STDC__ */

{
   UserErrorCode = 0;
   /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parametre document correct */
      SetCurrentLabel (LoadedDocument[document - 1], label);
}
