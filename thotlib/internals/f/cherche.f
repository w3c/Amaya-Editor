
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern boolean TextOk ( PtrElement premsel, int premcar, PtrElement dersel, int dercar, boolean MajEgalMin, char pChaineCherchee[MAX_CHAR], int LgChaineCh );
extern boolean ChTexte ( PtrDocument pDoc, PtrElement *pElDebut, int *NumCarDebut, PtrElement *pElFin, int *NumCarFin, boolean EnAvant, boolean MajEgalMin, char pChaineCherchee[MAX_CHAR], int LgChaineCh );
extern void ChNatures ( PtrSSchema pSchStr, PtrSSchema TablePtrNature[10], int *LgTableNat, boolean SansDouble );
extern PtrReference ReferSuivante ( PtrElement pEl, PtrDocument pDocEl, boolean TraiteNonCharge, PtrReference pRefPrec, PtrDocument *pDocRef, PtrExternalDoc *pDE, boolean DocExtSuivant );
extern void ChUneRef ( PtrReference *ReferCour, PtrDocument *DocDeReferCour, PtrElement *ElemRefCour, PtrDocument *DocDeElemRefCour, PtrExternalDoc *pDocExtCour, boolean DocExtSuivant );
extern void ChElemRefer ( void );
extern void RemplaceTexte(PtrDocument docsel, PtrElement pEl, int icar, int LgChaineCh, char pChaineRemplace[MAX_CHAR], int LgChaineRempl, boolean Affiche);
extern PtrElement ElemSuiv ( PtrElement pEl );
extern PtrElement CherchePage ( PtrElement from, int vue, int depl, boolean relatif );
extern void PageHautFenetre ( PtrElement PP, int VueDoc, PtrDocument pDoc );
extern void MoveToPage ( int fen, int func );

#else /* __STDC__ */

extern boolean TextOk (/* PtrElement premsel, int premcar, PtrElement dersel, int dercar, boolean MajEgalMin, char pChaineCherchee[MAX_CHAR], int LgChaineCh */);
extern boolean ChTexte (/* PtrDocument pDoc, PtrElement *pElDebut, int *NumCarDebut, PtrElement *pElFin, int *NumCarFin, boolean EnAvant, boolean MajEgalMin, char pChaineCherchee[MAX_CHAR], int LgChaineCh */);
extern void ChNatures (/* PtrSSchema pSchStr, PtrSSchema TablePtrNature[10], int *LgTableNat, boolean SansDouble */);
extern PtrReference ReferSuivante (/* PtrElement pEl, PtrDocument pDocEl, boolean TraiteNonCharge, PtrReference pRefPrec, PtrDocument *pDocRef, PtrExternalDoc *pDE, boolean DocExtSuivant */);
extern void ChUneRef (/* PtrReference *ReferCour, PtrDocument *DocDeReferCour, PtrElement *ElemRefCour, PtrDocument *DocDeElemRefCour, PtrExternalDoc *pDocExtCour, boolean DocExtSuivant */);
extern void ChElemRefer (/* void */);
extern void RemplaceTexte(/*PtrDocument docsel, PtrElement pEl, int icar, int LgChaineCh, char pChaineRemplace[MAX_CHAR], int LgChaineRempl, boolean Affiche*/);
extern PtrElement ElemSuiv (/* PtrElement pEl */);
extern PtrElement CherchePage (/* PtrElement from, int vue, int depl, boolean relatif */);
extern void PageHautFenetre (/* PtrElement PP, int VueDoc, PtrDocument pDoc */);
extern void MoveToPage (/* int fen, int func */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
