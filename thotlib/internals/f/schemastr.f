
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void InitNatures ( void );
extern void LoadNat ( PtrSSchema PSchStr, Name NomSchPrs, int r );
extern void ajouteregle_CreeNature ( int *ret, PtrSSchema *pSS );
extern int CreeNature ( Name NomSchStr, Name NomSchPrs, PtrSSchema pSS );
extern void LoadSchemas ( Name NomSchStr, Name NomSchPrs, PtrSSchema *pSS, PtrSSchema pSCharge, boolean Extension );
extern boolean LibNature ( PtrSSchema pSPere, PtrSSchema pSS );
extern void LibSchemas ( PtrDocument pDoc );

#else /* __STDC__ */

extern void InitNatures (/* void */);
extern void LoadNat (/* PtrSSchema PSchStr, Name NomSchPrs, int r */);
extern void ajouteregle_CreeNature (/* int *ret, PtrSSchema *pSS */);
extern int CreeNature (/* Name NomSchStr, Name NomSchPrs, PtrSSchema pSS */);
extern void LoadSchemas (/* Name NomSchStr, Name NomSchPrs, PtrSSchema *pSS, PtrSSchema pSCharge, boolean Extension */);
extern boolean LibNature (/* PtrSSchema pSPere, PtrSSchema pSS */);
extern void LibSchemas (/* PtrDocument pDoc */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
