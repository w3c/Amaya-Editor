
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void SavePath ( PtrDocument pDoc, char *savedir, char *savename );
extern void RestorePath ( PtrDocument pDoc, char *savedir, char *savename );
extern int CrFenTitre ( PtrDocument pDoc, int view, Name VdViewName, int *vol, int X, int Y, int L, int H );
extern void TraiteMenuImporter(Name SchStrImport, PathBuffer DirectoryDocImport, Name NomDocImport);
extern void TraiteCommandeLister ( PtrDocument pDoc, int nv, boolean assoc, Name NomFichierListe, int TypeFichierListe );
extern void BackupAll ( void );
extern boolean AuMoinsUnDoc ( void );
extern void FermeToutesVuesDoc ( PtrDocument pDoc );
extern void ChangerDocPres ( PtrDocument pDoc, Name NewSchPres );
extern void ChangerNatPres ( PtrDocument pDoc, PtrSSchema pSchNat, Name NewSchPres );
extern void InitMenusEditeur ( void );
extern void QuitEditeur ( void );

#else /* __STDC__ */

extern void SavePath (/* PtrDocument pDoc, char *savedir, char *savename */);
extern void RestorePath (/* PtrDocument pDoc, char *savedir, char *savename */);
extern int CrFenTitre (/* PtrDocument pDoc, int view, Name VdViewName, int *vol, int X, int Y, int L, int H */);
extern void TraiteMenuImporter(/* Name SchStrImport, PathBuffer DirectoryDocImport, Name NomDocImport */);
extern void TraiteCommandeLister (/* PtrDocument pDoc, int nv, boolean assoc, Name NomFichierListe, int TypeFichierListe */);
extern void BackupAll (/* void */);
extern boolean AuMoinsUnDoc (/* void */);
extern void FermeToutesVuesDoc (/* PtrDocument pDoc */);
extern void ChangerDocPres (/* PtrDocument pDoc, Name NewSchPres */);
extern void ChangerNatPres (/* PtrDocument pDoc, PtrSSchema pSchNat, Name NewSchPres */);
extern void InitMenusEditeur (/* void */);
extern void QuitEditeur (/* void */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
