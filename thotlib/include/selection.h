#ifndef _SELECTION_H_
#define _SELECTION_H_

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
