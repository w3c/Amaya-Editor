
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void ModPresentStandard ( PtrDocument pDoc, int Vue, boolean Assoc );
extern void RetMenuPresentStandard ( int ref, int val );
extern void RetMenuPresent ( int ref, int val, char *txt );
extern void ModPresentCaracteres ( PtrDocument pDoc, int Vue, boolean Assoc );
extern void ModPresentGraphiques ( PtrDocument pDoc, int Vue, boolean Assoc );
extern void ModPresentCouleurs ( PtrDocument pDoc, int Vue, boolean Assoc );
extern void ModPresentFormat ( PtrDocument pDoc, int Vue, boolean Assoc );
extern void EnleveFormPresentation ( PtrDocument pDoc );

#else /* __STDC__ */

extern void ModPresentStandard (/* PtrDocument pDoc, int Vue, boolean Assoc */);
extern void RetMenuPresentStandard (/* int ref, int val */);
extern void RetMenuPresent (/* int ref, int val, char *txt */);
extern void ModPresentCaracteres (/* PtrDocument pDoc, int Vue, boolean Assoc */);
extern void ModPresentGraphiques (/* PtrDocument pDoc, int Vue, boolean Assoc */);
extern void ModPresentCouleurs (/* PtrDocument pDoc, int Vue, boolean Assoc */);
extern void ModPresentFormat (/* PtrDocument pDoc, int Vue, boolean Assoc */);
extern void EnleveFormPresentation (/* PtrDocument pDoc */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
