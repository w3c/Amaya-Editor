/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/****************************************************************/
	/*                                                              */
	/*      Ce module implemente les fonctions de l'API Thot qui    */
	/*      permettent de lire ou d'ecrire certaines parties de     */
	/*      la forme pivot des documents.                           */
	/*                                                              */
	/*      Utilise' en particulier par Thotfon, le comparateur     */
	/*      et le fusionneur de documents                           */
	/*                                                              */
	/*      V. Quint        Juin 1992                               */
	/*                                                              */
/****************************************************************/
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "storage.h"
#include "application.h"
#include "document.h"
#include "tree.h"
#include "attribute.h"
#include "presentation.h"
#include "constpiv.h"

#include "arbabs.f"
#include "thotmsg.f"
#include "structure.f"
#include "application.f"
#include "pivecr.f"
#include "pivlec.f"
#include "storage.f"
#include "appexec.f"
#include "refext.f"

#undef EXPORT
#define EXPORT
#include "edit.var"

extern int          UserErrorCode;


/* ----------------------------------------------------------------------
   TtaAllocateDocument

   Allocates a new document context.

   Parameter:
   documentName: name of the document to be created (maximum length 19
   characters). The directory name is not part of this parameter
   (see TtaSetDocumentPath).

   Return value:
   the allocated document.

   ---------------------------------------------------------------------- */
#ifdef __STDC__
Document            TtaAllocateDocument (char *documentName)

#else  /* __STDC__ */
Document            TtaAllocateDocument (documentName)
char               *documentName;

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
	strncpy (pDoc->DocDName, documentName, MAX_NAME_LENGTH);
	/* on acquiert in identificateur pour le document */
	GetDocIdent (&pDoc->DocIdent, documentName);
	/* document en lecture-ecriture */
	pDoc->DocReadOnly = False;
	doc = IdentDocument (pDoc);
     }
   return doc;
}


/* ----------------------------------------------------------------------
   TtaWritePivotHeader

   Writes the header of a pivot file.

   Parameters:
   pivotFile: the pivot file. This file must be open.
   document: the document for which the pivot file is written.

   ---------------------------------------------------------------------- */

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
   else if (TabDocuments[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parametre document correct */
      WritePivotHeader (pivotFile, TabDocuments[document - 1]);
}


/* ----------------------------------------------------------------------
   TtaReadPivotHeader

   Reads the header of a pivot file.

   Parameters:
   pivotFile: the pivot file. This file must be open.
   document: the document for which the header is read.

   Return parameter:
   nextChar: first byte that follows the header in the pivot file.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TtaReadPivotHeader (BinFile pivotFile, Document document, char *nextChar)

#else  /* __STDC__ */
void                TtaReadPivotHeader (pivotFile, document, nextChar)
BinFile             pivotFile;
Document            document;
char               *nextChar;

#endif /* __STDC__ */

{
   UserErrorCode = 0;
   /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (TabDocuments[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parametre document correct */
      rdPivotHeader (pivotFile, TabDocuments[document - 1], nextChar);
}


/* ----------------------------------------------------------------------
   TtaWriteLanguageTable

   Writes into a pivot file the names of all languages used in a document.

   Parameters:
   pivotFile: the pivot file. This file must be open and at the
   correct position.
   document: the document for which the language names must be written.

   ---------------------------------------------------------------------- */

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
   else if (TabDocuments[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parametre document correct */
      WriteTableLangues (pivotFile, TabDocuments[document - 1]);
}


/* ----------------------------------------------------------------------
   TtaReadLanguageTable

   Reads from a pivot file the names of all languages used in a document.
   The language names read are stored in the context of the document.

   Parameters:
   pivotFile: the pivot file. This file must be open and at the correct
   position.
   document: the document for which the languages are read.

   Return parameter:
   nextChar: first byte that follows the language names in the pivot file.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TtaReadLanguageTable (BinFile pivotFile, Document document, char *nextChar)

#else  /* __STDC__ */
void                TtaReadLanguageTable (pivotFile, document, nextChar)
BinFile             pivotFile;
Document            document;
char               *nextChar;

#endif /* __STDC__ */

{
   UserErrorCode = 0;
   /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (TabDocuments[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parametre document correct */
      rdTableLangues (pivotFile, TabDocuments[document - 1], nextChar);
}

/* ----------------------------------------------------------------------
   TtaWriteSchemaNames

   Writes into a pivot file the names of all structure schemas and presentation
   schemas used by a document.

   Parameters:
   pivotFile: the pivot file. This file must be open and at the
   correct position.
   document: the document.

   ---------------------------------------------------------------------- */

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
   else if (TabDocuments[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parametre document correct */
      WriteNomsSchemasDoc (pivotFile, TabDocuments[document - 1]);
}


/* ----------------------------------------------------------------------
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

   ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TtaReadSchemaNames (BinFile pivotFile, Document document, char *nextChar)

#else  /* __STDC__ */
void                TtaReadSchemaNames (pivotFile, document, nextChar)
BinFile             pivotFile;
Document            document;
char               *nextChar;

#endif /* __STDC__ */

{
   UserErrorCode = 0;
   /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (TabDocuments[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parametre document correct */
      rdNomsSchemas (pivotFile, TabDocuments[document - 1], nextChar, NULL);
}

/* ----------------------------------------------------------------------
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

   ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TtaReadAttribute (BinFile pivotFile, Element element, Document document,
				      boolean create, Attribute * attribute)

#else  /* __STDC__ */
void                TtaReadAttribute (pivotFile, element, document, create, attribute)
BinFile             pivotFile;
Element             element;
Document            document;
boolean             create;
Attribute          *attribute;

#endif /* __STDC__ */

{
   PtrAttribute         pAttr;

   UserErrorCode = 0;
   /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (TabDocuments[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parametre document correct */
      ReadAttribut (pivotFile, (PtrElement) element,
		    TabDocuments[document - 1], create,
		    (PtrAttribute *) attribute, &pAttr);
}

/* ----------------------------------------------------------------------
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

   ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TtaReadPRule (BinFile pivotFile, Element element, Document document, boolean create, PRule * pRule)

#else  /* __STDC__ */
void                TtaReadPRule (pivotFile, element, document, create, pRule)
BinFile             pivotFile;
Element             element;
Document            document;
boolean             create;
PRule              *pRule;

#endif /* __STDC__ */

{
   UserErrorCode = 0;
   /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (TabDocuments[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parametre document correct */
      rdReglePres (TabDocuments[document - 1], pivotFile,
		(PtrElement) element, create, (PtrPRule *) pRule, True);
}

/* ----------------------------------------------------------------------
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

   ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TtaReadTree (BinFile pivotFile, Element element, Document document, char *byte, Element * elementRead)

#else  /* __STDC__ */
void                TtaReadTree (pivotFile, element, document, byte, elementRead)
BinFile             pivotFile;
Element             element;
Document            document;
char               *byte;
Element            *elementRead;

#endif /* __STDC__ */

{
   int         TypeCont;
   PtrSSchema        pSchStrCont;
   int                 NumAssoc;
   PtrSSchema        pSS;
   int         TypeLu;
   PtrSSchema        SchStrLu;

   UserErrorCode = 0;
   /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (TabDocuments[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parametre document correct */
     {
	TypeCont = 0;
	pSchStrCont = NULL;
	if (element == NULL)
	  {
	     NumAssoc = 0;
	     pSS = TabDocuments[document - 1]->DocSSchema;
	  }
	else
	  {
	     NumAssoc = ((PtrElement) element)->ElAssocNum;
	     pSS = ((PtrElement) element)->ElSructSchema;
	  }
	*elementRead = (Element) Internalise (pivotFile, pSS,
				 TabDocuments[document - 1], byte, NumAssoc,
			      False, True, &TypeCont, &pSchStrCont, &TypeLu,
			       &SchStrLu, True, (PtrElement) element, True);
     }
}


/* ----------------------------------------------------------------------
   TtaWriteTree

   Writes a tree into a pivot file.

   Parameters:
   pivotFile: the pivot file. This file must be open and the tree will
   be written at the current position.
   element: root element of the tree to be written.
   document: the document to which the tree belongs.

   ---------------------------------------------------------------------- */


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
   else if (TabDocuments[document - 1] == NULL)
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
	notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElSructSchema);
	notifyEl.position = 0;
	if (!ThotSendMessage ((NotifyEvent *) & notifyEl, True))
	   /* l'application accepte que Thot sauve l'element */
	  {
	     /* Ecrit d'abord le numero de la structure generique s'il y */
	     /* a changement de schema de structure par rapport au pere */
	     if (pEl->ElParent != NULL)
		if (pEl->ElParent->ElSructSchema != pEl->ElSructSchema)
		   EcritNat (pEl->ElSructSchema, pivotFile, TabDocuments[document - 1]);
	     Externalise (pivotFile, &pEl, TabDocuments[document - 1], True);
	     /* envoie le message ElemSave.Post a l'application, si */
	     /* elle le demande */
	     notifyEl.event = TteElemSave;
	     notifyEl.document = document;
	     notifyEl.element = element;
	     notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
	     notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElSructSchema);
	     notifyEl.position = 0;
	     ThotSendMessage ((NotifyEvent *) & notifyEl, False);
	  }
     }
}


/* ----------------------------------------------------------------------
   TtaWriteElement

   Writes a single element into a pivot file. The descendants of that element
   are not written.

   Parameters:
   pivotFile: the pivot file. This file must be open and the element will
   be written at the current position.
   element: element to be written.
   document: the document to which the element belongs.

   ---------------------------------------------------------------------- */

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
   else if (TabDocuments[document - 1] == NULL)
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
	notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElSructSchema);
	notifyEl.position = 0;
	if (!ThotSendMessage ((NotifyEvent *) & notifyEl, True))
	   /* l'application accepte que Thot sauve l'element */
	  {
	     /* Ecrit d'abord le numero de la structure generique s'il y */
	     /* a changement de schema de structure par rapport au pere */
	     if (pEl->ElParent != NULL)
		if (pEl->ElParent->ElSructSchema != pEl->ElSructSchema)
		   EcritNat (pEl->ElSructSchema, pivotFile, TabDocuments[document - 1]);
	     Externalise (pivotFile, &pEl, TabDocuments[document - 1], False);
	     /* envoie le message ElemSave.Post a l'application, si */
	     /* elle le demande */
	     notifyEl.event = TteElemSave;
	     notifyEl.document = document;
	     notifyEl.element = element;
	     notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
	     notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElSructSchema);
	     notifyEl.position = 0;
	     ThotSendMessage ((NotifyEvent *) & notifyEl, False);
	  }
     }
}


/* ----------------------------------------------------------------------
   TtaReadPivotVersion

   Reads the pivot version number from a pivot file. The version number read
   is stored in the context of the document.

   Parameters:
   pivotFile: the pivot file. This file must be open and at the position
   where a version number is expected. When returning, the current
   position in the pivot file is just after the version number that
   has been read.
   document: the document for which the pivot version number is read.

   ---------------------------------------------------------------------- */

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
   else if (TabDocuments[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parametre document correct */
      UserErrorCode = rdVersionNumber (pivotFile, TabDocuments[document - 1]);
}


/* ----------------------------------------------------------------------
   TtaWritePivotVersion

   Writes the current pivot version number into a pivot file.

   Parameters:
   pivotFile: the pivot file. This file must be open and the pivot
   version number will be written at the current position.
   document: the document for which the pivot version number is written.

   ---------------------------------------------------------------------- */

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
   else if (TabDocuments[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parametre document correct */
      WriteVersionNumber (pivotFile);
}


/* ----------------------------------------------------------------------
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

   ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TtaReadLabel (BinFile pivotFile, char byte, char *labelRead)

#else  /* __STDC__ */
void                TtaReadLabel (pivotFile, byte, labelRead)
BinFile             pivotFile;
char                byte;
char               *labelRead;

#endif /* __STDC__ */

{
   UserErrorCode = 0;
   rdLabel (byte, labelRead, pivotFile);
}


/* ----------------------------------------------------------------------
   TtaWriteLabel

   Writes a label into a pivot file.

   Parameters:
   pivotFile: the pivot file. This file must be open and the label will
   be written at the current position.
   label: the label to be written.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TtaWriteLabel (BinFile pivotFile, char *label)

#else  /* __STDC__ */
void                TtaWriteLabel (pivotFile, label)
BinFile             pivotFile;
char               *label;

#endif /* __STDC__ */

{
   UserErrorCode = 0;
   PutLabel (pivotFile, label);
}


/**** les fonctions suivantes sont utilisees par le fusionneur de Anne Denys ****/
/* ----------------------------------------------------------------------
   TtaNewElementWithLabel

   Creates a new element of a given type.

   Parameters:
   document: the document for which the element is created.
   elemType: type of the element to be created.
   label: label of the element to be created. PcEmpty string if the value
   of the label is undefined.

   Return value:
   the created element.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
Element             TtaNewElementWithLabel (Document document, ElementType elemType, char *label)

#else  /* __STDC__ */
Element             TtaNewElementWithLabel (document, elemType, label)
Document            document;
ElementType         elemType;
char               *label;

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
   else if (TabDocuments[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parametre document correct */
      if (elemType.ElTypeNum < 1 ||
   elemType.ElTypeNum > ((PtrSSchema) (elemType.ElSSchema))->SsNRules)
      TtaError (ERR_invalid_element_type);
   else
     {
	element = (Element) NewSubtree (elemType.ElTypeNum, (PtrSSchema) (elemType.ElSSchema),
		    TabDocuments[document - 1], 0, False, True, True, (*label) == '\0');
	if (*label != '\0')
	   strncpy (((PtrElement) element)->ElLabel, label, MAX_LABEL_LEN);
     }
   return element;
}


/* ----------------------------------------------------------------------
   TtaGetLabelMax

   Returns the value of the highest label used in a document.

   Parameter:
   document: the document.

   Return value:
   highest label used in the document.

   ---------------------------------------------------------------------- */

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
   else if (TabDocuments[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parametre document correct */
     {
	label = GetCurrentLabel (TabDocuments[document - 1]);
     }
   return label;
}

/* ----------------------------------------------------------------------
   TtaSetLabelMax

   Sets the value of the next label to be created in a document.

   Parameters:
   document: the document.
   label: value of the next label to be created.

   ---------------------------------------------------------------------- */

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
   else if (TabDocuments[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parametre document correct */
      SetCurrentLabel (TabDocuments[document - 1], label);
}

/* fin du module */
