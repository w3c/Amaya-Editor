/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
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
 
#ifndef _PIVOT_H_
#define _PIVOT_H_
#include "thot_sys.h"
#include "fileaccess.h"
#include "presentation.h"
#include "attribute.h"

#ifndef __CEXTRACT__
#ifdef __STDC__

extern Document     TtaAllocateDocument (char *documentName, char *documentIdentifier, char *documentSchemasPath);
extern void         TtaWritePivotHeader (BinFile pivotFile, Document document);
extern void         TtaReadPivotHeader (BinFile pivotFile, Document document, char *nextChar);
extern void         TtaWriteLanguageTable (BinFile pivotFile, Document document);
extern void         TtaReadLanguageTable (BinFile pivotFile, Document document, char *nextChar);
extern void         TtaWriteSchemaNames (BinFile pivotFile, Document document);
extern void         TtaReadSchemaNames (BinFile pivotFile, Document document, char *nextChar, void (*withThisPSchema) (Document document, CHAR_T *natSchema, CHAR_T *presentSchema));
extern void         TtaReadAttribute (BinFile pivotFile, Document document, ThotBool create, Attribute * attribute);
extern void         TtaReadPRule (BinFile pivotFile, Element element, Document document, ThotBool create, PRule * pRule);
extern void         TtaReadTree (BinFile pivotFile, SSchema pSchema, Element element, Document document, char *byte, Element * elementRead);
extern void         TtaWriteTree (BinFile pivotFile, Element element, Document document);
extern void         TtaWriteElement (BinFile pivotFile, Element element, Document document);
extern void         TtaReadPivotVersion (BinFile pivotFile, Document document);
extern void         TtaWritePivotVersion (BinFile pivotFile, Document document);
extern void         TtaReadLabel (BinFile pivotFile, char byte, char *labelRead);
extern void         TtaWriteLabel (BinFile pivotFile, char *label);
extern Element      TtaNewElementWithLabel (Document document, ElementType elemType, char *label);
extern int          TtaGetLabelMax (Document document);
extern void         TtaSetLabelMax (Document document, int label);

#else  /* __STDC__ */

extern Document     TtaAllocateDocument ( /* char *documentName, char *documentIdentifier, char *documentSchemasPath */ );
extern void         TtaWritePivotHeader ( /* BinFile pivotFile, Document document */ );
extern void         TtaReadPivotHeader ( /* BinFile pivotFile, Document document, char *nextChar */ );
extern void         TtaWriteLanguageTable ( /* BinFile pivotFile, Document document */ );
extern void         TtaReadLanguageTable ( /* BinFile pivotFile, Document document, char *nextChar */ );
extern void         TtaWriteSchemaNames ( /* Document document, BinFile pivotFile, Document document */ );
extern void         TtaReadSchemaNames ( /* BinFile pivotFile, Document document, char *nextChar, void (*withThisPSchema) (CHAR_T *natSchema, CHAR_T *presentSchema) */ );
extern void         TtaReadAttribute ( /* BinFile pivotFile, Document document, ThotBool create, Attribute *attribute */ );
extern void         TtaReadPRule ( /* BinFile pivotFile, Element element, Document document, ThotBool create, PRule *pRule */ );
extern void         TtaReadTree ( /* BinFile pivotFile,  SSchema pSchema, Element element, Document document, char *byte, Element *elementRead */ );
extern void         TtaWriteTree ( /* BinFile pivotFile, Element element, Document document */ );
extern void         TtaWriteElement ( /* BinFile pivotFile, Element element, Document document */ );
extern void         TtaReadPivotVersion ( /* BinFile pivotFile, Document document */ );
extern void         TtaWritePivotVersion ( /* BinFile pivotFile, Document document */ );
extern void         TtaReadLabel ( /* BinFile pivotFile, char byte, char *labelRead */ );
extern void         TtaWriteLabel ( /* BinFile pivotFile, char *label */ );
extern Element      TtaNewElementWithLabel ( /* Document document, ElementType elemType, char *label */ );
extern int          TtaGetLabelMax ( /* Document document */ );
extern void         TtaSetLabelMax ( /* Document document, int label */ );

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */

#endif
