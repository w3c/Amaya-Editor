
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern int TypeBPage ( PtrElement pEl, int VueNb, PtrPSchema *pSchPPage );
extern int CptPage ( PtrElement pEl, int VueNb, PtrPSchema *pSchPPage );
extern void ApplPage ( PtrElement pEl, PtrDocument pDoc, DocViewNumber VueNb, int VueSch, int TypeP, PtrPSchema pSchPPage, PtrAbstractBox NouvPave );
#ifdef __COLPAGE__
extern int TypeBCol(PtrElement pEl, int VueNb, PtrPSchema *pSchPPage, int *NbCol);
extern int NbPages(PtrAbstractBox pPav);
extern void PagePleine (DocViewNumber VueNb, PtrDocument pDoc, PtrElement pEl, boolean EnAvant);
extern PtrElement TestElHB(PtrElement pEl, DocViewNumber VueNb);
extern void CrPavHB(PtrElement pEl, PtrDocument pDoc, DocViewNumber VueNb, PtrElement pElPage, boolean EnAvant);
extern void InitPageCol (PtrElement *ppEl, PtrDocument pDoc, DocViewNumber VueNb, int VueSch, boolean EnAvant, boolean *Creation, boolean *complet, boolean *ApplRegles, PtrElement *pElSauv);
extern void CreePageCol (PtrElement pEl, PtrAbstractBox *AdrNouvPave, PtrDocument pDoc, DocViewNumber VueNb, int VueSch, boolean EnAvant, boolean *complet,	int *lqueue, PtrPRule queuePR[MAX_QUEUE_LEN], PtrAbstractBox queuePP[MAX_QUEUE_LEN], PtrPSchema queuePS[MAX_QUEUE_LEN], PtrAttribute queuePA[MAX_QUEUE_LEN], PtrElement pElSauv,  PtrPSchema  pSchPPage, int TypeP, boolean *arret);
extern PtrAbstractBox RechPavPage(PtrElement pEl, DocViewNumber VueNb, int VueSch, boolean EnAvant);
#endif /* __COLPAGE__ */

#else /* __STDC__ */

extern int TypeBPage (/* PtrElement pEl, int VueNb, PtrPSchema *pSchPPage */);
extern int CptPage (/* PtrElement pEl, int VueNb, PtrPSchema *pSchPPage */);
extern void ApplPage (/* PtrElement pEl, PtrDocument pDoc, DocViewNumber VueNb, int VueSch, int TypeP, PtrPSchema pSchPPage, PtrAbstractBox NouvPave */);
#ifdef __COLPAGE__
extern int TypeBCol(/* PtrElement pEl, int VueNb, PtrPSchema *pSchPPage, int *NbCol */);
extern int NbPages(/* PtrAbstractBox pPav */);
extern void PagePleine (/* DocViewNumber VueNb, PtrDocument pDoc, PtrElement pEl, boolean EnAvant */);
extern PtrElement TestElHB ( /* PtrElement pEl, DocViewNumber VueNb */);
extern void CrPavHB(/* PtrElement pEl, PtrDocument pDoc, DocViewNumber VueNb, PtrElement pElPage, boolean EnAvant */);
extern void InitPageCol (/* PtrElement *ppEl, PtrDocument pDoc, DocViewNumber VueNb, int VueSch, boolean EnAvant, boolean *Creation, boolean *complet, boolean *ApplRegles, PtrElement *pElSauv */);
extern void CreePageCol (/* PtrElement pEl, PtrAbstractBox *AdrNouvPave, PtrDocument pDoc, DocViewNumber VueNb, int VueSch, boolean EnAvant, boolean *complet,	int *lqueue, PtrPRule queuePR[MAX_QUEUE_LEN], PtrAbstractBox queuePP[MAX_QUEUE_LEN], PtrPSchema queuePS[MAX_QUEUE_LEN], PtrAttribute queuePA[MAX_QUEUE_LEN], PtrElement pElSauv, PtrPSchema  pSchPPage, int TypeP, boolean *arret */);
extern PtrAbstractBox RechPavPage(/* PtrElement pEl, DocViewNumber VueNb, int VueSch, boolean EnAvant */);
#endif /* __COLPAGE__ */

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
