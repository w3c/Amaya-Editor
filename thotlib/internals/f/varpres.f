
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern int MinMaxComptVal ( int NCompt, PtrSSchema pSS, PtrPSchema pSchP, PtrElement pElNum, int Vue, boolean Maximum );
extern int ComptVal ( int NCompt, PtrSSchema pSS, PtrPSchema pSchP, PtrElement pElNum, int Vue );
extern boolean PavPresentModifiable ( PtrAbstractBox pPav );
extern boolean NouvVariable ( int NVar, PtrSSchema pSS, PtrPSchema pSchP, PtrAbstractBox pPav, PtrDocument pDoc );

#else /* __STDC__ */

extern int MinMaxComptVal (/* int NCompt, PtrSSchema pSS, PtrPSchema pSchP, PtrElement pElNum, int Vue, boolean Maximum */);
extern int ComptVal (/* int NCompt, PtrSSchema pSS, PtrPSchema pSchP, PtrElement pElNum, int Vue */);
extern boolean PavPresentModifiable (/* PtrAbstractBox pPav */);
extern boolean NouvVariable (/* int NVar, PtrSSchema pSS, PtrPSchema pSchP, PtrAbstractBox pPav, PtrDocument pDoc */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
