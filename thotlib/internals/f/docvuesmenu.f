
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void MenuAutoSave ( PtrDocument pDoc );
extern void RetMenuAutoSave ( int val );
extern void CopyOrMoveDoc ( boolean *CopyDoc, boolean *MoveDoc );
extern void RetMenuSauverDoc ( int ref, int val, char *txt );
extern boolean fileNameMenu ( char * NomDuDocument, char * NomDuDirectory );
extern void EnleveFormSauverDocument ( PtrDocument pDoc );
extern void SauveDocComme ( PtrDocument pDoc );
extern void RetireAnnoter ( int fen );
extern void DetruitFenetre ( int nfenetre );
extern void MajMenuDocument ( PtrDocument pDoc, int vue, boolean assoc );
extern void MajMenuVues ( PtrDocument pDoc, int vue, boolean assoc );
extern void PaginerVue ( PtrDocument pDoc, int Vue, boolean Assoc );
extern void MajMenusDeVue(PtrDocument pDoc, int Vue, boolean Assoc);
extern void MajMenusVariables ( PtrDocument pDoc, int Vue, boolean Assoc );
extern void RetMenuVues ( int ref, int val );
extern void OuvreVueDoc ( PtrDocument pDoc, int VueRef, PtrElement pSArbre );
extern void FermerDocument ( PtrDocument pDoc );

#else /* __STDC__ */

extern void MenuAutoSave (/* PtrDocument pDoc */);
extern void RetMenuAutoSave (/* int val */);
extern void CopyOrMoveDoc (/* boolean *CopyDoc, boolean *MoveDoc */);
extern void RetMenuSauverDoc (/* int ref, int val, char *txt */);
extern boolean fileNameMenu (/* char * NomDuDocument, char * NomDuDirectory */);
extern void EnleveFormSauverDocument (/* PtrDocument pDoc */);
extern void SauveDocComme (/* PtrDocument pDoc */);
extern void RetireAnnoter (/* int fen */);
extern void DetruitFenetre (/* int nfenetre */);
extern void MajMenuDocument (/* PtrDocument pDoc, int vue, boolean assoc */);
extern void MajMenuVues (/* PtrDocument pDoc, int vue, boolean assoc */);
extern void PaginerVue (/* PtrDocument pDoc, int Vue, boolean Assoc */);
extern void MajMenusDeVue(/* PtrDocument pDoc, int Vue, boolean Assoc */);
extern void MajMenusVariables (/* PtrDocument pDoc, int Vue, boolean Assoc */);
extern void RetMenuVues (/* int ref, int val */);
extern void OuvreVueDoc (/* PtrDocument pDoc, int VueRef, PtrElement pSArbre */);
extern void FermerDocument (/* PtrDocument pDoc */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
