
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern PtrElement ElemRefer ( PtrReference pRef, DocumentIdentifier *IDoc, PtrDocument *pDoc );
extern PtrReference ChRefDocExt ( PtrElement pEl, PtrDocument pDocEl, boolean TraiteNonCharge, PtrDocument *pDocRef, PtrExternalDoc *pDE, boolean DocExtSuivant );
extern PtrReferredDescr NewRef ( PtrDocument pDoc );
extern void SuppDescRef ( PtrReferredDescr pDR );
extern void SuppRef ( PtrElement El );
extern void RefSupprime ( PtrReference RefPtr );
extern void RefAnnule ( PtrElement pEl );
extern void CopieRef ( PtrReference RefCopie, PtrReference RefSource, PtrElement *pEl );
extern void TransRefInclus ( PtrElement pElem, PtrDocument pDoc, PtrElement pEl, PtrDocument pDocSource );
extern void NoteRefSortantes ( PtrElement pEl, PtrDocument pDoc, boolean New );
extern void AjDocRefExt ( PtrElement pEl, DocumentIdentifier IdentDoc, PtrDocument pDoc2 );
extern boolean LieReference ( PtrElement, PtrAttribute, PtrElement, PtrDocument, PtrDocument, boolean, boolean);
extern void CopieDocExt ( PtrElement pEl, PtrChangedReferredEl ElemRef );
extern void NoteElemRefDetruits ( PtrElement pEl, PtrDocument pDoc );

#else /* __STDC__ */

extern PtrElement ElemRefer (/* PtrReference pRef, DocumentIdentifier *IDoc, PtrDocument *pDoc */);
extern PtrReference ChRefDocExt (/* PtrElement pEl, PtrDocument pDocEl, boolean TraiteNonCharge, PtrDocument *pDocRef, PtrExternalDoc *pDE, boolean DocExtSuivant */);
extern PtrReferredDescr NewRef (/* PtrDocument pDoc */);
extern void SuppDescRef (/* PtrReferredDescr pDR */);
extern void SuppRef (/* PtrElement El */);
extern void RefSupprime (/* PtrReference RefPtr */);
extern void RefAnnule (/* PtrElement pEl */);
extern void CopieRef (/* PtrReference RefCopie, PtrReference RefSource, PtrElement *pEl */);
extern void TransRefInclus (/* PtrElement pElem, PtrDocument pDoc, PtrElement pEl, PtrDocument pDocSource */);
extern void NoteRefSortantes (/* PtrElement pEl, PtrDocument pDoc, boolean New */);
extern void AjDocRefExt (/* PtrElement pEl, DocumentIdentifier IdentDoc, PtrDocument pDoc2 */);
extern boolean LieReference (/* PtrElement pElRef, PtrAttribute pAttrRef, PtrElement pSelEl, PtrDocument pDoc, PtrDocument pDoc2, boolean verif, boolean WithAPPEvent */);
extern void CopieDocExt (/* PtrElement pEl, PtrChangedReferredEl ElemRef */);
extern void NoteElemRefDetruits (/* PtrElement pEl, PtrDocument pDoc */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
