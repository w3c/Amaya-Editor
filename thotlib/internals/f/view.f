
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern int GetWindowNumber ( Document document, View view );
extern View TtaOpenMainView ( Document document, int x, int y, int w, int h );
extern View TtaOpenView ( Document document, char *viewName, int x, int y, int w, int h );
extern View TtaOpenSubView ( Document document, char *viewName, int x, int y, int w, int h, Element subtree );
extern void TtaCloseView ( Document document, View view );
extern void TtaSetPSchema ( Document document, char *presentationName );
extern void TtaSetSensibility ( Document document, View view, int value );
extern void TtaSetZoom ( Document document, View view, int value );
extern void TtaShowElement ( Document document, View view, Element element, int position );
extern int TtaGetSensibility ( Document document, View view );
extern int TtaGetZoom ( Document document, View view );
extern int TtaIsPSchemaValid ( char *structureName, char *presentationName );
extern void TtaGiveViewsToOpen ( Document document, char *buffer, int *nbViews );
extern char *TtaGetViewName ( Document document, View view );
extern View TtaGetViewFromName ( Document document, char *viewName );
extern void TtaGiveActiveView ( Document *document, int *view );
extern void RedispNewElement ( Document document, PtrElement newElement, PtrElement sibling, boolean first, boolean creation );
extern void ChangePavModif ( PtrElement pEl, Document document, boolean newPavModif );
extern void NewSelection ( Document document, Element element, int firstCharacter, int lastCharacter );
extern void NewSelectionExtension ( Document document, Element element, int lastCharacter );
extern void VerifieElementSelectionne ( PtrElement pEl, Document document );
extern void UndisplayElement ( PtrElement pEl, Document document );
extern void DisplayHolophrasted ( PtrElement pEl, Document document );
extern void HideElement ( PtrElement pEl, Document document );
extern void RedispReference ( PtrElement element, Document document );
extern void RedispLeaf ( PtrElement element, Document document, int delta );
extern void RedispSplittedText ( PtrElement element, Document document );
extern void RedispMergedText ( PtrElement element, Document document );
extern void UndisplayHeritAttr ( PtrElement pEl, PtrAttribute pAttr, Document document, boolean suppression );
extern void DisplayAttribute ( PtrElement pEl, PtrAttribute pAttr, Document document );
extern void UndisplayAttribute ( PtrElement pEl, PtrAttribute pAttr, Document document );
extern void RedispNewPresSpec ( Document document, PtrElement pEl, PtrPRule pRegle );
extern void RedispPresStandard ( Document document, PtrElement pEl, PRuleType typeRegleP, int vue );
extern void RedispNewGeometry ( Document document, PtrElement pEl );
extern void TtaSetDisplayMode ( Document document, DisplayMode newDisplayMode );
extern DisplayMode TtaGetDisplayMode ( Document document );
extern void TtaResetViewBackgroundColor ( Document document, View view );
extern void TtaSetViewBackgroundColor ( Document document, View view, int color );
extern int  TtaGetViewBackgroundColor ( Document document, View viewr );

#else /* __STDC__ */

extern int GetWindowNumber (/* Document document, View view */);
extern View TtaOpenMainView (/* Document document, int x, int y, int w, int h */);
extern View TtaOpenView (/* Document document, char *viewName, int x, int y, int w, int h */);
extern View TtaOpenSubView (/* Document document, char *viewName, int x, int y, int w, int h, Element subtree */);
extern void TtaCloseView (/* Document document, View view */);
extern void TtaSetPSchema (/* Document document, char *presentationName */);
extern void TtaSetSensibility (/* Document document, View view, int value */);
extern void TtaSetZoom (/* Document document, View view, int value */);
extern void TtaShowElement (/* Document document, View view, Element element, int position */);
extern int TtaGetSensibility (/* Document document, View view */);
extern int TtaGetZoom (/* Document document, View view */);
extern int TtaIsPSchemaValid (/* char *structureName, char *presentationName */);
extern void TtaGiveViewsToOpen (/* Document document, char *buffer, int *nbViews */);
extern char *TtaGetViewName (/* Document document, View view */);
extern View TtaGetViewFromName (/* Document document, char *viewName */);
extern void TtaGiveActiveView (/* Document *document, int *view */);
extern void RedispNewElement (/* Document document, PtrElement newElement, PtrElement sibling, boolean first, boolean creation */);
extern void ChangePavModif (/* PtrElement pEl, Document document, boolean newPavModif */);
extern void NewSelection (/* Document document, Element element, int firstCharacter, int lastCharacter */);
extern void NewSelectionExtension (/* Document document, Element element, int lastCharacter */);
extern void VerifieElementSelectionne (/* PtrElement pEl, Document document */);
extern void UndisplayElement (/* PtrElement pEl, Document document */);
extern void DisplayHolophrasted (/* PtrElement pEl, Document document */);
extern void HideElement (/* PtrElement pEl, Document document */);
extern void RedispReference (/* PtrElement element, Document document */);
extern void RedispLeaf (/* PtrElement element, Document document, int delta */);
extern void RedispSplittedText (/* PtrElement element, Document document */);
extern void RedispMergedText (/* PtrElement element, Document document */);
extern void UndisplayHeritAttr (/* PtrElement pEl, PtrAttribute pAttr, Document document, boolean suppression */);
extern void DisplayAttribute (/* PtrElement pEl, PtrAttribute pAttr, Document document */);
extern void UndisplayAttribute (/* PtrElement pEl, PtrAttribute pAttr, Document document */);
extern void RedispNewPresSpec (/* Document document, PtrElement pEl, PtrPRule pRegle */);
extern void RedispPresStandard (/* Document document, PtrElement pEl, PRuleType typeRegleP, int vue */);
extern void RedispNewGeometry (/* Document document, PtrElement pEl */);
extern void TtaSetDisplayMode (/* Document document, DisplayMode newDisplayMode */);
extern DisplayMode TtaGetDisplayMode (/* Document document */);
extern void TtaResetViewBackgroundColor (/* Document document, View view */);
extern void TtaSetViewBackgroundColor (/* Document document, View view, int color */);
extern int  TtaGetViewBackgroundColor (/* Document document, View view */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
