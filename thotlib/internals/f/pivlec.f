
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern PtrTextBuffer readComment ( BinFile fich, boolean effectif, boolean oldformat );
extern void ReadAttribut ( BinFile fichpiv, PtrElement pEl, PtrDocument pDoc, boolean cree, PtrAttribute *pAttrLu, PtrAttribute *pAttr );
extern void rdReglePres ( PtrDocument pDoc, BinFile fichpiv, PtrElement pEl, boolean cree, PtrPRule *pRegleLue, boolean Attache );
extern void rdNomsSchemas(BinFile fich, PtrDocument pDoc, char *marque, PtrSSchema pSCharge);
extern PtrElement Internalise ( BinFile fichpiv, PtrSSchema pSchStr, PtrDocument pDoc, char *marque, int NAssoc, boolean creepar, boolean creetout, int *typecontenu, PtrSSchema *pschcont, int *TypeLu, PtrSSchema *SchStrLu, boolean creepage, PtrElement Pere, boolean creedesc );
extern boolean ArbreCorrect ( PtrElement pEl, PtrDocument pDoc );
extern void AccouplePaires ( PtrElement pRacine );
extern int rdVersionNumber ( BinFile fich, PtrDocument pDoc );
extern void rdPivotHeader ( BinFile fich, PtrDocument pDoc, char *marque );
extern void ChargeDoc ( BinFile, PtrDocument, boolean, boolean, PtrSSchema, boolean);
extern void rdTableLangues(BinFile fich, PtrDocument pDoc, char *marque);

#else /* __STDC__ */

extern PtrTextBuffer readComment (/* BinFile fich, boolean effectif, boolean oldformat */);
extern void ReadAttribut (/* BinFile fichpiv, PtrElement pEl, PtrDocument pDoc, boolean cree, PtrAttribute *pAttrLu, PtrAttribute *pAttr */);
extern void rdReglePres (/* PtrDocument pDoc, BinFile fichpiv, PtrElement pEl, boolean cree, PtrPRule *pRegleLue, boolean Attache */);
extern void rdNomsSchemas(/*BinFile fich, PtrDocument pDoc, char *marque, PtrSSchema pSCharge*/);
extern PtrElement Internalise (/* BinFile fichpiv, PtrSSchema pSchStr, PtrDocument pDoc, char *marque, int NAssoc, boolean creepar, boolean creetout, int *typecontenu, PtrSSchema *pschcont, int *TypeLu, PtrSSchema *SchStrLu, boolean creepage, PtrElement Pere, boolean creedesc */);
extern boolean ArbreCorrect (/* PtrElement pEl, PtrDocument pDoc */);
extern void AccouplePaires (/* PtrElement pRacine */);
extern int rdVersionNumber (/* BinFile fich, PtrDocument pDoc */);
extern void rdPivotHeader (/* BinFile fich, PtrDocument pDoc, char *marque */);
extern void ChargeDoc (/* BinFile fich, PtrDocument pDoc, boolean ChargeDocExt, boolean Squelette, PtrSSchema pSCharge, boolean WithAPPEvent */);
extern void rdTableLangues(/*BinFile fich, PtrDocument pDoc, char *marque*/);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
