
#ifndef __CEXTRACT__
#ifdef __STDC__

extern int IdentDocument ( PtrDocument pDoc );
extern boolean Corr_alphabet ();
extern void InsertText();
extern void asci2code ( char * chaine );
extern void code2asci ( char * chaine );
extern void Corr_pretraitement ( PtrDico dico );
extern boolean Corr_ReloadDico ( PtrDico *pDico );
extern void Dico_Init ();
extern void TraiteExceptionCreation ( PtrElement pEl, PtrDocument pDoc );
extern void GetViewInfo(Document document, View view, int *viewnumber, boolean *assoc);
extern int GetWindowNumber ( Document document, View view );
extern void RedispNewElement ( Document document, PtrElement newElement, PtrElement sibling, boolean first, boolean creation );
extern void ChangePavModif ( PtrElement pEl, Document document, boolean newPavModif );
extern void NewSelection ( Document document, Element element, int firstCharacter, int lastCharacter );
extern void NewSelectionExtension ( Document document, Element element, int lastCharacter );
extern void UndisplayElement ( PtrElement pEl, Document document );
extern void DisplayHolophrasted ( PtrElement pEl, Document document );
extern void HideElement ( PtrElement pEl, Document document );
extern void RedispReference ( PtrElement element, Document document );
extern void RedispLeaf ( PtrElement element, Document document, int delta );
extern void RedispSplittedText ( PtrElement element, int position, Document document );
extern void RedispMergedText ( PtrElement element, Document document );
extern void UndisplayHeritAttr ( PtrElement pEl, PtrAttribute pAttr, Document document, boolean suppression );
extern void DisplayAttribute ( PtrElement pEl, PtrAttribute pAttr, Document document );
extern void UndisplayAttribute ( PtrElement pEl, PtrAttribute pAttr, Document document );
extern void RedispNewPresSpec ( Document document, PtrElement pEl, PtrPRule pRegle );
extern void RedispPresStandard ( Document document, PtrElement pEl, PRuleType typeRegleP, int vue );
extern void RedispNewGeometry ( Document document, PtrElement pEl );
extern boolean DemandeSelEnregistree ( Document document, boolean *annulation );

#else /* __STDC__ */

extern int IdentDocument (/* PtrDocument pDoc */);
extern boolean Corr_alphabet ();
extern void InsertText();
extern void asci2code (/* char * chaine */);
extern void code2asci (/* char * chaine */);
extern void Corr_pretraitement (/* PtrDico dico */);
extern boolean Corr_ReloadDico (/* PtrDico *pDico */);
extern void Dico_Init ();
extern void TraiteExceptionCreation (/* PtrElement pEl, PtrDocument pDoc */);
extern void GetViewInfo(/*Document document, View view, int *viewnumber, boolean *assoc*/);
extern int GetWindowNumber (/* Document document, View view */);
extern void RedispNewElement (/* Document document, PtrElement newElement, PtrElement sibling, boolean first, boolean creation */);
extern void ChangePavModif (/* PtrElement pEl, Document document, boolean newPavModif */);
extern void NewSelection (/* Document document, Element element, int firstCharacter, int lastCharacter */);
extern void NewSelectionExtension (/* Document document, Element element, int lastCharacter */);
extern void UndisplayElement (/* PtrElement pEl, Document document */);
extern void DisplayHolophrasted (/* PtrElement pEl, Document document */);
extern void HideElement (/* PtrElement pEl, Document document */);
extern void RedispReference (/* PtrElement element, Document document */);
extern void RedispLeaf (/* PtrElement element, Document document, int delta */);
extern void RedispSplittedText (/* PtrElement element, int position, Document document */);
extern void RedispMergedText (/* PtrElement element, Document document */);
extern void UndisplayHeritAttr (/* PtrElement pEl, PtrAttribute pAttr, Document document, boolean suppression */);
extern void DisplayAttribute (/* PtrElement pEl, PtrAttribute pAttr, Document document */);
extern void UndisplayAttribute (/* PtrElement pEl, PtrAttribute pAttr, Document document */);
extern void RedispNewPresSpec (/* Document document, PtrElement pEl, PtrPRule pRegle */);
extern void RedispPresStandard (/* Document document, PtrElement pEl, PRuleType typeRegleP, int vue */);
extern void RedispNewGeometry (/* Document document, PtrElement pEl */);
extern boolean DemandeSelEnregistree (/* Document document, boolean *annulation */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
