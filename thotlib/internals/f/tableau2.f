
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__
extern void Tableau_ApplRegleAttribut ( PtrElement pEl, PtrAttribute pAttr, PtrDocument pDoc, boolean *ApplAttr );
extern void Tableau_PaveRef ( PtrAbstractBox pPav, PosRule *Posit, PtrPRule pRegle, PtrAbstractBox pPavBas );
extern void Tableau_Cond_Dernier ( PtrElement pEl, boolean *PcLast );
extern void TableauDebordeVertical ( PtrPRule pRegle, PtrAbstractBox pPav );
extern void HauteurFilets ( PtrElement pBasTableau, PtrDocument pDoc );
extern void Exc_Page_Break_Inserer ( PtrElement pElPage, PtrDocument pDoc, int VueNb, boolean *coupe );
extern void Exc_Page_Break_Supprime ( PtrElement pElPage, PtrDocument pDoc );
extern void Exc_Page_Break_Detruit_Pave ( PtrElement pElPage, PtrDocument pDoc, int VueNb );

#else /* __STDC__ */

extern void Tableau_ApplRegleAttribut (/* PtrElement pEl, PtrAttribute pAttr, PtrDocument pDoc, boolean *ApplAttr */);
extern void Tableau_PaveRef (/* PtrAbstractBox pPav, PosRule *Posit, PtrPRule pRegle, PtrAbstractBox pPavBas */);
extern void Tableau_Cond_Dernier (/* PtrElement pEl, boolean *PcLast */);
extern void TableauDebordeVertical (/* PtrPRule pRegle, PtrAbstractBox pPav */);
extern void HauteurFilets (/* PtrElement pBasTableau, PtrDocument pDoc */);
extern void Exc_Page_Break_Inserer (/* PtrElement pElPage, PtrDocument pDoc, int VueNb, boolean *coupe */);
extern void Exc_Page_Break_Supprime (/* PtrElement pElPage, PtrDocument pDoc */);
extern void Exc_Page_Break_Detruit_Pave (/* PtrElement pElPage, PtrDocument pDoc, int VueNb */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
