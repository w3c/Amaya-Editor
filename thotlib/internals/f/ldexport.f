
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern int IdentDocument ( PtrDocument pDoc );
extern boolean ThotSendMessage ( int *notifyEvent, boolean pre );
extern boolean LibNature ( PtrSSchema pSPere, PtrSSchema pSS );
extern void LibSchemas ( PtrDocument pDoc );
extern int Tableau_OrphanCell ( PtrElement pCell, PtrElement pSauve );
extern int PtEnPixel ( int valeur, int horiz );
extern int PixelEnPt ( int valeur, int horiz );
extern void DrawSupprAttr ( PtrAttribute pAttr, PtrElement pEl );
extern PtrPSchema RdSchPres ( Name fname, PtrSSchema SS );
extern void LibPavElem ( PtrElement pEl );
extern PtrPRule ReglePEl ( PtrElement pEl, PtrPSchema *pSPR, PtrSSchema *pSSR, int NumPres, PtrPSchema pSchP, int Vue, PRuleType TRegle, boolean Pag, boolean attr, PtrAttribute *pAttr );
extern boolean FormeComplete ( void );
extern void ChangeModeSynchro ( boolean Sync );
extern void InitNatures ( void );
extern void LoadNat ( PtrSSchema PSchStr, Name NomSchPrs, int r );
extern void ajouteregle_CreeNature ( int *ret, PtrSSchema *pSS );
extern int CreeNature ( Name NomSchStr, Name NomSchPrs, PtrSSchema pSS );
extern void LoadSchemas ( Name NomSchStr, Name NomSchPrs, PtrSSchema *pSS, PtrSSchema pSCharge, boolean Extension );
extern PtrSSchema LoadExtension ( Name NomSchStr, Name NomSchPrs, PtrDocument pDoc );
extern int main ( int argc, char **argv );

#else /* __STDC__ */

extern int IdentDocument (/* PtrDocument pDoc */);
extern boolean ThotSendMessage (/* int *notifyEvent, boolean pre */);
extern boolean LibNature (/* PtrSSchema pSPere, PtrSSchema pSS */);
extern void LibSchemas (/* PtrDocument pDoc */);
extern int Tableau_OrphanCell (/* PtrElement pCell, PtrElement pSauve */);
extern int PtEnPixel (/* int valeur, int horiz */);
extern int PixelEnPt (/* int valeur, int horiz */);
extern void DrawSupprAttr (/* PtrAttribute pAttr, PtrElement pEl */);
extern PtrPSchema RdSchPres (/* Name fname, PtrSSchema SS */);
extern void LibPavElem (/* PtrElement pEl */);
extern PtrPRule ReglePEl (/* PtrElement pEl, PtrPSchema *pSPR, PtrSSchema *pSSR, int NumPres, PtrPSchema pSchP, int Vue, PRuleType TRegle, boolean Pag, boolean attr, PtrAttribute *pAttr */);
extern boolean FormeComplete (/* void */);
extern void ChangeModeSynchro (/* boolean Sync */);
extern void InitNatures (/* void */);
extern void LoadNat (/* PtrSSchema PSchStr, Name NomSchPrs, int r */);
extern void ajouteregle_CreeNature (/* int *ret, PtrSSchema *pSS */);
extern int CreeNature (/* Name NomSchStr, Name NomSchPrs, PtrSSchema pSS */);
extern void LoadSchemas (/* Name NomSchStr, Name NomSchPrs, PtrSSchema *pSS, PtrSSchema pSCharge, boolean Extension */);
extern PtrSSchema LoadExtension (/* Name NomSchStr, Name NomSchPrs, PtrDocument pDoc */);
extern int main (/* int argc, char **argv */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
