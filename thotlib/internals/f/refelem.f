
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern PtrElement ReferredElement ( PtrReference pRef, DocumentIdentifier *IDoc, PtrDocument *pDoc );
extern PtrReference SearchExternalReferenceToElem ( PtrElement pEl, PtrDocument pDocEl, boolean TraiteNonCharge, PtrDocument *pDocRef, PtrExternalDoc *pDE, boolean DocExtSuivant );
extern PtrReferredDescr NewReferredElDescr ( PtrDocument pDoc );
extern void DeleteReferredElDescr ( PtrReferredDescr pDR );
extern void DeleteAllReferences ( PtrElement El );
extern void DeleteReference ( PtrReference RefPtr );
extern void CancelReference ( PtrElement pEl );
extern void CopyReference ( PtrReference RefCopie, PtrReference RefSource, PtrElement *pEl );
extern void TransferReferences ( PtrElement pElem, PtrDocument pDoc, PtrElement pEl, PtrDocument pDocSource );
extern void RegisterExternalRef ( PtrElement pEl, PtrDocument pDoc, boolean New );
extern void AddDocOfExternalRef ( PtrElement pEl, DocumentIdentifier IdentDoc, PtrDocument pDoc2 );
extern boolean SetReference ( PtrElement, PtrAttribute, PtrElement, PtrDocument, PtrDocument, boolean, boolean);
extern void CopyDescrExtDoc ( PtrElement pEl, PtrChangedReferredEl ElemRef );
extern void RegisterDeletedReferredElem ( PtrElement pEl, PtrDocument pDoc );

#else /* __STDC__ */

extern PtrElement ReferredElement (/* PtrReference pRef, DocumentIdentifier *IDoc, PtrDocument *pDoc */);
extern PtrReference SearchExternalReferenceToElem (/* PtrElement pEl, PtrDocument pDocEl, boolean TraiteNonCharge, PtrDocument *pDocRef, PtrExternalDoc *pDE, boolean DocExtSuivant */);
extern PtrReferredDescr NewReferredElDescr (/* PtrDocument pDoc */);
extern void DeleteReferredElDescr (/* PtrReferredDescr pDR */);
extern void DeleteAllReferences (/* PtrElement El */);
extern void DeleteReference (/* PtrReference RefPtr */);
extern void CancelReference (/* PtrElement pEl */);
extern void CopyReference (/* PtrReference RefCopie, PtrReference RefSource, PtrElement *pEl */);
extern void TransferReferences (/* PtrElement pElem, PtrDocument pDoc, PtrElement pEl, PtrDocument pDocSource */);
extern void RegisterExternalRef (/* PtrElement pEl, PtrDocument pDoc, boolean New */);
extern void AddDocOfExternalRef (/* PtrElement pEl, DocumentIdentifier IdentDoc, PtrDocument pDoc2 */);
extern boolean SetReference (/* PtrElement pElRef, PtrAttribute pAttrRef, PtrElement pSelEl, PtrDocument pDoc, PtrDocument pDoc2, boolean verif, boolean WithAPPEvent */);
extern void CopyDescrExtDoc (/* PtrElement pEl, PtrChangedReferredEl ElemRef */);
extern void RegisterDeletedReferredElem (/* PtrElement pEl, PtrDocument pDoc */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
