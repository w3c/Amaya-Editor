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
 
#ifndef _SELECTION_H_
#define _SELECTION_H_
#include "typebase.h"
#include "tree.h"
#include "document.h"

#ifndef __CEXTRACT__
#ifdef __STDC__

extern boolean      TtaIsDocumentSelected (Document document);
extern void         TtaSelectElement (Document document, Element selectedElement);
extern void         TtaSelectString (Document document, Element textElement, int firstCharacter, int lastCharacter);
extern void         TtaExtendSelection (Document document, Element element, int lastCharacter);
extern void         TtaSelectInterval ();
extern void         TtaUnselect (Document document);
extern void         TtaSetSelectionMode (boolean withMenu);
extern void         TtaGiveFirstSelectedElement (Document document, Element * selectedElement, int *firstCharacter, int *lastCharacter);
extern void         TtaGiveNextSelectedElement (Document document, Element * selectedElement, int *firstCharacter, int *lastCharacter);
extern void         TtaGiveLastSelectedElement (Document document, Element * selectedElement, int *firstCharacter, int *lastCharacter);

#else  /* __STDC__ */

extern boolean      TtaIsDocumentSelected ( /*Document document */ );
extern void         TtaSelectElement ( /* Document document, Element selectedElement */ );
extern void         TtaSelectString ( /* Document document, Element textElement, int firstCharacter, int lastCharacter */ );
extern void         TtaExtendSelection ( /* Document document, Element element, int lastCharacter */ );
extern void         TtaSelectInterval ();
extern void         TtaUnselect ( /* Document document */ );
extern void         TtaSetSelectionMode ( /* boolean withMenu */ );
extern void         TtaGiveFirstSelectedElement ( /* Document document, Element *selectedElement, int *firstCharacter, int *lastCharacter */ );
extern void         TtaGiveNextSelectedElement ( /* Document document, Element *selectedElement, int *firstCharacter, int *lastCharacter */ );
extern void         TtaGiveLastSelectedElement ( /* Document document, Element *selectedElement, int *firstCharacter, int *lastCharacter */ );

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */

#endif
