
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void CreeDocument ( PtrDocument *pDoc );
extern void ChRegle ( int *NRegle, PtrSSchema *pSS, Name NomType );
extern int TypeIdentite ( int nR1, PtrSSchema pS1 );
extern boolean RegleStrEgale ( int nR1, PtrSSchema pS1, int nR2, PtrSSchema pS2 );
extern boolean Equivalent ( int nR1, PtrSSchema pS1, int nR2, PtrSSchema pS2, PtrElement pEl );
extern int RegleListeDeElem ( int NRegle, PtrSSchema pSS );
extern int RegleAgregatDeElem ( int NRegle, PtrSSchema pSS );
extern boolean TypeExclus ( PtrElement pEl, int numtype, PtrSSchema pSchStr );
extern boolean InclusSGMLpossible ( PtrElement pEl, int NType, PtrSSchema pSS );
extern void RegleSuite ( PtrElement pEl, int *nR, PtrSSchema *pS );
extern RConstruct ElConstruct ( PtrElement El );
extern PtrElement ListeAieule ( PtrElement pE );
extern boolean testListLength ( PtrElement father, int delta );
extern void RegleVoisin ( PtrElement pEl, boolean Dev, int Distance, int *nR, PtrSSchema *pS, boolean *RegleListe, boolean *Opt );
extern void TypePointe ( PtrElement pElRef, PtrAttribute pAttrRef, PtrSSchema *pSS, int *NumType );
extern boolean CanCut ( PtrElement pEl, PtrDocument pDoc, PtrElement pSauve );
extern boolean VoisinPossible ( PtrElement pEl, PtrDocument pDoc, int nType, PtrSSchema pSS, boolean Avant, boolean User, boolean EnPlace );
extern boolean PremierDansAgregat ( int nTypeAgr, PtrSSchema pSSAgr, int nTypeComp, PtrSSchema pSSComp );
extern boolean PremierFilsPossible ( PtrElement pEl, PtrDocument pDoc, int nType, PtrSSchema pSS, boolean User, boolean EnPlace );
extern void InsereDesc ( PtrElement Pere, PtrElement Desc, PtrElement *pFeuille );
extern PtrElement Descendance ( int NRegle, PtrSSchema pSchStr, PtrDocument pDoc, PtrElement *pFeuille, int NAssoc, int TypeDesc, PtrSSchema pStrDesc );
extern SRule *RegleExtens ( PtrSSchema pSS, int NumType, PtrSSchema pSchExt );
extern boolean ExtensionValide ( PtrElement pEl, PtrSSchema *pExt );
extern PtrAttribute Attr ( PtrElement pEl, PtrAttribute pAttr );
extern PtrAttribute ValAttr ( PtrElement pEl, PtrAttribute pAttr );
extern boolean ApplicationPossible ( PtrElement pEl, PtrAttribute pAttr, PtrAttribute pAttrNouv, boolean *obligatoire );
extern boolean CanSplitElement ( PtrElement premsel, int premcar, boolean Paragraph, PtrElement *pUp, PtrElement *pEl, PtrElement *ElemADupliquer );
#else /* __STDC__ */

extern void CreeDocument (/* PtrDocument *pDoc */);
extern void ChRegle (/* int *NRegle, PtrSSchema *pSS, Name NomType */);
extern int TypeIdentite (/* int nR1, PtrSSchema pS1 */);
extern boolean RegleStrEgale (/* int nR1, PtrSSchema pS1, int nR2, PtrSSchema pS2 */);
extern boolean Equivalent (/* int nR1, PtrSSchema pS1, int nR2, PtrSSchema pS2, PtrElement pEl */);
extern int RegleListeDeElem (/* int NRegle, PtrSSchema pSS */);
extern int RegleAgregatDeElem (/* int NRegle, PtrSSchema pSS */);
extern boolean TypeExclus (/* PtrElement pEl, int numtype, PtrSSchema pSchStr */);
extern boolean InclusSGMLpossible (/* PtrElement pEl, int NType, PtrSSchema pSS */);
extern void RegleSuite (/* PtrElement pEl, int *nR, PtrSSchema *pS */);
extern RConstruct ElConstruct (/* PtrElement El */);
extern PtrElement ListeAieule (/* PtrElement pE */);
extern boolean testListLength (/* PtrElement father, int delta */);
extern void RegleVoisin (/* PtrElement pEl, boolean Dev, int Distance, int *nR, PtrSSchema *pS, boolean *RegleListe, boolean *Opt */);
extern void TypePointe (/* PtrElement pElRef, PtrAttribute pAttrRef, PtrSSchema *pSS, int *NumType */);
extern boolean CanCut (/* PtrElement pEl, PtrDocument pDoc, PtrElement pSauve */);
extern boolean VoisinPossible (/* PtrElement pEl, PtrDocument pDoc, int nType, PtrSSchema pSS, boolean Avant, boolean User, boolean EnPlace */);
extern boolean PremierDansAgregat (/* int nTypeAgr, PtrSSchema pSSAgr, int nTypeComp, PtrSSchema pSSComp */);
extern boolean PremierFilsPossible (/* PtrElement pEl, PtrDocument pDoc, int nType, PtrSSchema pSS, boolean User, boolean EnPlace */);
extern void InsereDesc (/* PtrElement Pere, PtrElement Desc, PtrElement *pFeuille */);
extern PtrElement Descendance (/* int NRegle, PtrSSchema pSchStr, PtrDocument pDoc, PtrElement *pFeuille, int NAssoc, int TypeDesc, PtrSSchema pStrDesc */);
extern SRule *RegleExtens (/* PtrSSchema pSS, int NumType, PtrSSchema pSchExt */);
extern boolean ExtensionValide (/* PtrElement pEl, PtrSSchema *pExt */);
extern PtrAttribute Attr (/* PtrElement pEl, PtrAttribute pAttr */);
extern PtrAttribute ValAttr (/* PtrElement pEl, PtrAttribute pAttr */);
extern boolean ApplicationPossible (/* PtrElement pEl, PtrAttribute pAttr, PtrAttribute pAttrNouv, boolean *obligatoire */);
extern boolean CanSplitElement (/* PtrElement premsel, int premcar, boolean Paragraph, PtrElement *pUp, PtrElement *pEl, PtrElement *ElemADupliquer */);
#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
