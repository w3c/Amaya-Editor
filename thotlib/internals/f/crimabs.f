
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern PtrPRule GetRule ( PtrPRule *pRSpecif, PtrPRule *pRDefaut, PtrElement pEl, PtrAttribute pAttr, PtrSSchema pSS );
extern PtrPRule GetRuleView ( PtrPRule *pRSpecif, PtrPRule *pRDefaut, PRuleType Typ, int Vue, PtrElement pEl, PtrAttribute pAttr, PtrSSchema pSS );
extern PtrAbstractBox initpave ( PtrElement pEl, DocViewNumber nv, int Visib );
extern void CopieConstante ( int NConst, PtrPSchema pSchP, PtrAbstractBox pPav );
extern boolean VueAssoc ( PtrElement pEl );
extern boolean VueExiste ( PtrElement pEl, PtrDocument pDoc, DocViewNumber VueNb );
extern int VueAAppliquer ( PtrElement pEl, PtrAttribute pAttr, PtrDocument pDoc, DocViewNumber VueNb );
extern void Retarde ( PtrPRule pR, PtrPSchema pSP, PtrAbstractBox pP, PtrAttribute pAttr, PtrAbstractBox pPRP );
extern void GetRet ( PtrPRule *pR, PtrPSchema *pSP, PtrAbstractBox *pP, PtrAttribute *pAttr );
extern void ApplReglesRetard ( PtrElement pEl, PtrDocument pDoc );
extern boolean CondPresentation ( PtrCondition pCond, PtrElement pEl, PtrAttribute pAttr, int Vue, PtrSSchema pSS );
extern PtrAbstractBox CrPavPres ( PtrElement pEl, PtrDocument pDoc, PtrPRule pRCre, PtrSSchema pSS, PtrAttribute pAttr, DocViewNumber VueNb, PtrPSchema pSchP, boolean DansBoiteAssoc, boolean CreateurComplet );
extern PtrPRule ReglePresAttr ( PtrAttribute pAttr, PtrElement pEl, boolean heritage, PtrAttribute pAttrComp, PtrPSchema pSchP );
extern PtrAbstractBox PaveCoupe ( PtrAbstractBox pPav, boolean Coupe, boolean Tete, PtrDocument pDoc );
extern boolean VuePleine(DocViewNumber VueNb, PtrDocument pDoc, PtrElement pEl);
extern void Attente ( PtrPRule pR, PtrAbstractBox pP, PtrPSchema pSP, PtrAttribute pA, PtrAttribute queuePA[MAX_QUEUE_LEN], PtrPSchema queuePS[MAX_QUEUE_LEN], PtrAbstractBox queuePP[MAX_QUEUE_LEN], PtrPRule queuePR[MAX_QUEUE_LEN], int *lqueue );
extern PtrAbstractBox CreePaves ( PtrElement pEl, PtrDocument pDoc, DocViewNumber VueNb, boolean EnAvant, boolean Desc, boolean *complet );

#else /* __STDC__ */

extern PtrPRule GetRule (/* PtrPRule *pRSpecif, PtrPRule *pRDefaut, PtrElement pEl, PtrAttribute pAttr, PtrSSchema pSS */);
extern PtrPRule GetRuleView (/* PtrPRule *pRSpecif, PtrPRule *pRDefaut, PRuleType Typ, int Vue, PtrElement pEl, PtrAttribute pAttr, PtrSSchema pSS */);
extern PtrAbstractBox initpave (/* PtrElement pEl, DocViewNumber nv, int Visib */);
extern void CopieConstante (/* int NConst, PtrPSchema pSchP, PtrAbstractBox pPav */);
extern boolean VueAssoc (/* PtrElement pEl */);
extern boolean VueExiste (/* PtrElement pEl, PtrDocument pDoc, DocViewNumber VueNb */);
extern int VueAAppliquer (/* PtrElement pEl, PtrAttribute pAttr, PtrDocument pDoc, DocViewNumber VueNb */);
extern void Retarde (/* PtrPRule pR, PtrPSchema pSP, PtrAbstractBox pP, PtrAttribute pAttr, PtrAbstractBox pPRP */);
extern void GetRet (/* PtrPRule *pR, PtrPSchema *pSP, PtrAbstractBox *pP, PtrAttribute *pAttr */);
extern void ApplReglesRetard (/* PtrElement pEl, PtrDocument pDoc */);
extern boolean CondPresentation (/* PtrCondition pCond, PtrElement pEl, PtrAttribute pAttr, int Vue, PtrSSchema pSS */);
extern PtrAbstractBox CrPavPres (/* PtrElement pEl, PtrDocument pDoc, PtrPRule pRCre, PtrSSchema pSS, PtrAttribute pAttr, DocViewNumber VueNb, PtrPSchema pSchP, boolean DansBoiteAssoc, boolean CreateurComplet */);
extern PtrPRule ReglePresAttr (/* PtrAttribute pAttr, PtrElement pEl, boolean heritage, PtrAttribute pAttrComp, PtrPSchema pSchP */);
extern PtrAbstractBox PaveCoupe (/* PtrAbstractBox pPav, boolean Coupe, boolean Tete, PtrDocument pDoc */);
extern boolean VuePleine( /* DocViewNumber VueNb, PtrDocument pDoc, PtrElement pEl */);
extern void Attente (/* PtrPRule pR, PtrAbstractBox pP, PtrPSchema pSP, PtrAttribute pA, PtrAttribute queuePA[MAX_QUEUE_LEN], PtrPSchema queuePS[MAX_QUEUE_LEN], PtrAbstractBox queuePP[MAX_QUEUE_LEN], PtrPRule queuePR[MAX_QUEUE_LEN], int *lqueue */);
extern PtrAbstractBox CreePaves (/* PtrElement pEl, PtrDocument pDoc, DocViewNumber VueNb, boolean EnAvant, boolean Desc, boolean *complet */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
