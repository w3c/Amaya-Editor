 
/* -- Copyright (c) 1990 - 1995 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern PtrPRule ReglePage( PtrElement pEl, int Vue, PtrPSchema *pSchP );
extern PtrPRule RegleType_HautPage( PRuleType TypeR, int b, PtrPSchema pSchP );
#ifdef __COLPAGE__
extern PtrPRule    ReglePageDebut(PtrElement pElPage, PtrPSchema *pSchP);
extern boolean TueAvantPage(PtrAbstractBox pPage, int fenetre, PtrDocument pDoc, int VueNb);

#else /* __COLPAGE__ */
extern void HautPage( PtrElement pElPage, int Vue, int *b, PtrPSchema *pSchP );
extern void DansPage ( PtrAbstractBox pPav );
extern void TuePavesAuDessus ( PtrAbstractBox pP, int limite, int VueNb, PtrDocument pDoc, PtrAbstractBox *PavReaff );
extern boolean TueAvantPage( PtrAbstractBox pPage, int fenetre, PtrDocument pDoc, int VueNb );
#endif /* __COLPAGE__ */

#else /* __STDC__ */

extern PtrPRule    ReglePage(/* PtrElement pEl, int Vue, PtrPSchema *pSchP */);
extern PtrPRule    RegleType_HautPage(/* PRuleType TypeR, int b, PtrPSchema pSchP */);
#ifdef __COLPAGE__
extern PtrPRule    ReglePageDebut(/* PtrElement pElPage, PtrPSchema *pSchP */);
extern boolean TueAvantPage(/* PtrAbstractBox pPage, int fenetre, PtrDocument pDoc, int VueNb */);
#else
extern void HautPage(/* PtrElement pElPage, int Vue, int *b, PtrPSchema *pSchP */);
extern void DansPage (/* PtrAbstractBox pPav */);
extern void TuePavesAuDessus (/* PtrAbstractBox pP, int limite, int VueNb, PtrDocument pDoc, PtrAbstractBox *PavReaff */);
extern boolean TueAvantPage(/* PtrAbstractBox pPage, int fenetre, PtrDocument pDoc, int VueNb */);
#endif /* __COLPAGE__ */


#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
