
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void CreateDocument ( PtrDocument *pDoc );
extern void GetSRuleFromName ( int *NRegle, PtrSSchema *pSS, Name NomType );
extern int GetTypeNumIdentity ( int nR1, PtrSSchema pS1 );
extern boolean SameSRules ( int nR1, PtrSSchema pS1, int nR2, PtrSSchema pS2 );
extern boolean EquivalentSRules ( int nR1, PtrSSchema pS1, int nR2, PtrSSchema pS2, PtrElement pEl );
extern int ListRuleOfElem ( int NRegle, PtrSSchema pSS );
extern int AggregateRuleOfElem ( int NRegle, PtrSSchema pSS );
extern boolean ExcludedType ( PtrElement pEl, int numtype, PtrSSchema pSchStr );
extern boolean AllowedIncludedElem ( PtrElement pEl, int NType, PtrSSchema pSS );
extern void ListOrAggregateRule ( PtrElement pEl, int *nR, PtrSSchema *pS );
extern RConstruct GetElementConstruct ( PtrElement El );
extern PtrElement AncestorList ( PtrElement pE );
extern boolean CanChangeNumberOfElem ( PtrElement father, int delta );
extern void SRuleForSibling ( PtrElement pEl, boolean Dev, int Distance, int *nR, PtrSSchema *pS, boolean *RegleListe, boolean *Opt );
extern void ReferredType ( PtrElement pElRef, PtrAttribute pAttrRef, PtrSSchema *pSS, int *NumType );
extern boolean CanCutElement ( PtrElement pEl, PtrDocument pDoc, PtrElement pSauve );
extern boolean AllowedSibling ( PtrElement pEl, PtrDocument pDoc, int nType, PtrSSchema pSS, boolean Avant, boolean User, boolean EnPlace );
extern boolean AllowedFirstComponent ( int nTypeAgr, PtrSSchema pSSAgr, int nTypeComp, PtrSSchema pSSComp );
extern boolean AllowedFirstChild ( PtrElement pEl, PtrDocument pDoc, int nType, PtrSSchema pSS, boolean User, boolean EnPlace );
extern void InsertChildFirst ( PtrElement Pere, PtrElement Desc, PtrElement *pFeuille );
extern PtrElement CreateDescendant ( int NRegle, PtrSSchema pSchStr, PtrDocument pDoc, PtrElement *pFeuille, int NAssoc, int TypeDesc, PtrSSchema pStrDesc );
extern SRule *ExtensionRule ( PtrSSchema pSS, int NumType, PtrSSchema pSchExt );
extern boolean ValidExtension ( PtrElement pEl, PtrSSchema *pExt );
extern PtrAttribute GetAttributeOfElement ( PtrElement pEl, PtrAttribute pAttr );
extern PtrAttribute AttributeValue ( PtrElement pEl, PtrAttribute pAttr );
extern boolean CanAssociateAttr ( PtrElement pEl, PtrAttribute pAttr, PtrAttribute pAttrNouv, boolean *obligatoire );
extern boolean CanSplitElement ( PtrElement premsel, int premcar, boolean Paragraph, PtrElement *pUp, PtrElement *pEl, PtrElement *ElemADupliquer );
#else /* __STDC__ */

extern void CreateDocument (/* PtrDocument *pDoc */);
extern void GetSRuleFromName (/* int *NRegle, PtrSSchema *pSS, Name NomType */);
extern int GetTypeNumIdentity (/* int nR1, PtrSSchema pS1 */);
extern boolean SameSRules (/* int nR1, PtrSSchema pS1, int nR2, PtrSSchema pS2 */);
extern boolean EquivalentSRules (/* int nR1, PtrSSchema pS1, int nR2, PtrSSchema pS2, PtrElement pEl */);
extern int ListRuleOfElem (/* int NRegle, PtrSSchema pSS */);
extern int AggregateRuleOfElem (/* int NRegle, PtrSSchema pSS */);
extern boolean ExcludedType (/* PtrElement pEl, int numtype, PtrSSchema pSchStr */);
extern boolean AllowedIncludedElem (/* PtrElement pEl, int NType, PtrSSchema pSS */);
extern void ListOrAggregateRule (/* PtrElement pEl, int *nR, PtrSSchema *pS */);
extern RConstruct GetElementConstruct (/* PtrElement El */);
extern PtrElement AncestorList (/* PtrElement pE */);
extern boolean CanChangeNumberOfElem (/* PtrElement father, int delta */);
extern void SRuleForSibling (/* PtrElement pEl, boolean Dev, int Distance, int *nR, PtrSSchema *pS, boolean *RegleListe, boolean *Opt */);
extern void ReferredType (/* PtrElement pElRef, PtrAttribute pAttrRef, PtrSSchema *pSS, int *NumType */);
extern boolean CanCutElement (/* PtrElement pEl, PtrDocument pDoc, PtrElement pSauve */);
extern boolean AllowedSibling (/* PtrElement pEl, PtrDocument pDoc, int nType, PtrSSchema pSS, boolean Avant, boolean User, boolean EnPlace */);
extern boolean AllowedFirstComponent (/* int nTypeAgr, PtrSSchema pSSAgr, int nTypeComp, PtrSSchema pSSComp */);
extern boolean AllowedFirstChild (/* PtrElement pEl, PtrDocument pDoc, int nType, PtrSSchema pSS, boolean User, boolean EnPlace */);
extern void InsertChildFirst (/* PtrElement Pere, PtrElement Desc, PtrElement *pFeuille */);
extern PtrElement CreateDescendant (/* int NRegle, PtrSSchema pSchStr, PtrDocument pDoc, PtrElement *pFeuille, int NAssoc, int TypeDesc, PtrSSchema pStrDesc */);
extern SRule *ExtensionRule (/* PtrSSchema pSS, int NumType, PtrSSchema pSchExt */);
extern boolean ValidExtension (/* PtrElement pEl, PtrSSchema *pExt */);
extern PtrAttribute GetAttributeOfElement (/* PtrElement pEl, PtrAttribute pAttr */);
extern PtrAttribute AttributeValue (/* PtrElement pEl, PtrAttribute pAttr */);
extern boolean CanAssociateAttr (/* PtrElement pEl, PtrAttribute pAttr, PtrAttribute pAttrNouv, boolean *obligatoire */);
extern boolean CanSplitElement (/* PtrElement premsel, int premcar, boolean Paragraph, PtrElement *pUp, PtrElement *pEl, PtrElement *ElemADupliquer */);
#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
