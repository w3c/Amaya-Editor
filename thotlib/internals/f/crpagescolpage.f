
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern int TypeBPage ( PtrElement pEl, int VueNb, PtrPSchema *pSchPPage );
#ifdef __COLPAGE__
extern int TypeBCol(PtrElement pEl, int VueNb, PtrPSchema *pSchPPage, int *NbCol);
#endif /* __COLPAGE__ */
extern int CptPage ( PtrElement pEl, int VueNb, PtrPSchema *pSchPPage );
#ifdef __COLPAGE__
extern int NbPages(PtrAbstractBox pPav);
extern void PagePleine (DocViewNumber VueNb, PtrDocument pDoc, PtrElement pEl, boolean EnAvant);
extern PtrElement TestElHB(PtrElement pEl, DocViewNumber VueNb);
extern int CrPavHB(PtrElement pEl, PtrDocument pDoc, DocViewNumber VueNb, PtrElement pElPage, boolean EnAvant);
extern InitPageCol (PtrElement *ppEl, PtrDocument pDoc, DocViewNumber VueNb, int VueSch, boolean *Creation, boolean *complet, boolean *ApplRegles, PtrElement *pElSauv);
#endif /* __COLPAGE__ */
#ifndef __COLPAGE__
extern void ApplPage (PtrElement pEl, PtrDocument pDoc, DocViewNumber VueNb, int VueSch, int TypeP, PtrPSchema pSchPPage, PtrAbstractBox NouvPave);
#endif /* __COLPAGE__ */
#ifdef __COLPAGE__
extern PtrAbstractBox CreePageCol(PtrElement pEl, PtrAbstractBox *AdrNouvPave, PtrDocument pDoc, DocViewNumber VueNb, int VueSch, boolean EnAvant, boolean *complet,	int *lqueue, PtrPRule queuePR[MAX_QUEUE_LEN], PtrAbstractBox queuePP[MAX_QUEUE_LEN], PtrPSchema queuePS[MAX_QUEUE_LEN], PtrAttribute queuePA[MAX_QUEUE_LEN], PtrElement pElSauv);
extern PtrAbstractBox RechPavPage(PtrElement pEl, DocViewNumber VueNb, int VueSch, boolean EnAvant);
extern PtrAbstractBox RechPavPageCol(PtrElement pEl, DocViewNumber VueNb, int VueSch, boolean EnAvant);
extern PtrAbstractBox CopiePav(PtrAbstractBox pPav);
#endif /* __COLPAGE__ */

#else /* __STDC__ */

extern int TypeBPage (/* PtrElement pEl, int VueNb, PtrPSchema *pSchPPage */);
#ifdef __COLPAGE__
extern int TypeBCol(/* PtrElement pEl, int VueNb, PtrPSchema *pSchPPage, int *NbCol */);
#endif /* __COLPAGE__ */
extern int CptPage (/* PtrElement pEl, int VueNb, PtrPSchema *pSchPPage */);
#ifdef __COLPAGE__
extern int NbPages(/* PtrAbstractBox pPav */);
extern void PagePleine (/* DocViewNumber VueNb, PtrDocument pDoc, PtrElement pEl, boolean EnAvant */);
extern PtrElement TestElHB(/* PtrElement pEl, DocViewNumber VueNb */);
extern int CrPavHB(/* PtrElement pEl, PtrDocument pDoc, DocViewNumber VueNb, PtrElement pElPage, boolean EnAvant */);
extern InitPageCol (/* PtrElement *ppEl, PtrDocument pDoc, DocViewNumber VueNb, int VueSch, boolean *Creation, boolean *complet, boolean *ApplRegles, PtrElement *pElSauv */);
#endif /* __COLPAGE__ */
#ifndef __COLPAGE__
extern void ApplPage (/* PtrElement pEl, PtrDocument pDoc, DocViewNumber VueNb, int VueSch, int TypeP, PtrPSchema pSchPPage, PtrAbstractBox NouvPave */);
#endif /* __COLPAGE__ */
#ifdef __COLPAGE__
extern PtrAbstractBox CreePageCol(/* PtrElement pEl, PtrAbstractBox *AdrNouvPave, PtrDocument pDoc, DocViewNumber VueNb, int VueSch, boolean EnAvant, boolean *complet,	int *lqueue, PtrPRule queuePR[MAX_QUEUE_LEN], PtrAbstractBox queuePP[MAX_QUEUE_LEN], PtrPSchema queuePS[MAX_QUEUE_LEN], PtrAttribute queuePA[MAX_QUEUE_LEN], PtrElement pElSauv */);
extern PtrAbstractBox RechPavPage(/* PtrElement pEl, DocViewNumber VueNb, int VueSch, boolean EnAvant */);
extern PtrAbstractBox RechPavPageCol(/* PtrElement pEl, DocViewNumber VueNb, int VueSch, boolean EnAvant */);
extern PtrAbstractBox CopiePav(/* PtrAbstractBox pPav */);
#endif /* __COLPAGE__ */

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
