
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void TtaSelectElement ( Document document, Element selectedElement );
extern void TtaSelectString ( Document document, Element textElement, int firstCharacter, int lastCharacter );
extern void TtaExtendSelection ( Document document, Element element, int lastCharacter );
extern void TtaUnselect ( Document document );
extern void TtaGiveFirstSelectedElement ( Document document, Element *selectedElement, int *firstCharacter, int *lastCharacter );
extern void TtaGiveNextSelectedElement ( Document document, Element *selectedElement, int *firstCharacter, int *lastCharacter );
extern void TtaGiveLastSelectedElement ( Document document, Element *selectedElement, int *firstCharacter, int *lastCharacter );

#else /* __STDC__ */

extern void TtaSelectElement (/* Document document, Element selectedElement */);
extern void TtaSelectString (/* Document document, Element textElement, int firstCharacter, int lastCharacter */);
extern void TtaExtendSelection (/* Document document, Element element, int lastCharacter */);
extern void TtaUnselect (/* Document document */);
extern void TtaGiveFirstSelectedElement (/* Document document, Element *selectedElement, int *firstCharacter, int *lastCharacter */);
extern void TtaGiveNextSelectedElement (/* Document document, Element *selectedElement, int *firstCharacter, int *lastCharacter */);
extern void TtaGiveLastSelectedElement (/* Document document, Element *selectedElement, int *firstCharacter, int *lastCharacter */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
