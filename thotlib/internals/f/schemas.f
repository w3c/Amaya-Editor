
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void InitNatures ( void );
extern PtrPSchema LdSchPres ( Name fname, PtrSSchema SS );
extern void LibListeRegles_SupprSchPrs ( PtrPRule *arp );
extern void SupprSchPrs ( PtrPSchema pSch, PtrSSchema pSS );
extern void LoadNat ( PtrSSchema PSchStr, Name NomSchPrs, int r );
extern void ajouteregle_CreeNature ( int *ret, PtrSSchema *pSS );
extern int CreeNature ( Name NomSchStr, Name NomSchPrs, PtrSSchema pSS );
extern void LoadSchemas ( Name NomSchStr, Name NomSchPrs, PtrSSchema *pSS, PtrSSchema pSCharge, boolean Extension );
extern PtrSSchema	LoadExtension(Name NomSchStr, Name NomSchPrs, PtrDocument pDoc);
extern boolean LibNature ( PtrSSchema pSPere, PtrSSchema pSS );
extern void LibSchemas ( PtrDocument pDoc );
extern void InitSchAppli(PtrSSchema scheme);

#else /* __STDC__ */

extern void InitNatures (/* void */);
extern PtrPSchema LdSchPres (/* Name fname, PtrSSchema SS */);
extern void LibListeRegles_SupprSchPrs (/* PtrPRule *arp */);
extern void SupprSchPrs (/* PtrPSchema pSch, PtrSSchema pSS */);
extern void LoadNat (/* PtrSSchema PSchStr, Name NomSchPrs, int r */);
extern void ajouteregle_CreeNature (/* int *ret, PtrSSchema *pSS */);
extern int CreeNature (/* Name NomSchStr, Name NomSchPrs, PtrSSchema pSS */);
extern void LoadSchemas (/* Name NomSchStr, Name NomSchPrs, PtrSSchema *pSS, PtrSSchema pSCharge, boolean Extension */);
extern PtrSSchema	LoadExtension(/*NomSchStr, NomSchPrs, pDoc*/);
extern boolean LibNature (/* PtrSSchema pSPere, PtrSSchema pSS */);
extern void LibSchemas (/* PtrDocument pDoc */);
extern void InitSchAppli(/*PtrSSchema scheme*/);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
