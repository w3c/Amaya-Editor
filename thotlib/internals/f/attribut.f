
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern PtrAttribute MetAttribut ( int NumExcept, PtrElement pEl, PtrElement pElRef, PtrDocument pDoc );
extern void ReaffAttribut ( PtrAttribute pAttr, PtrElement pEl, PtrDocument pDoc );
extern void CoupeSelection ( PtrDocument SelDoc, PtrElement *PremSel, PtrElement *DerSel, int *premcar, int *dercar );
extern boolean MemesAttributs ( PtrElement pEl1, PtrElement pEl2 );
extern void ApplReglesAttr ( PtrElement pEl, PtrDocument pDoc, PtrAttribute pAttr, boolean herit );
extern void ApplReglesPresHeriteesAttr ( PtrElement pEl, PtrDocument pDoc, PtrAttribute pAttr );
extern void ApplReglesPresCompareesAttr ( PtrElement pEl, PtrDocument pDoc, PtrAttribute pAttr );
extern void ChaineLib ( PtrElement pL, PtrElement *PremLib );
extern void ChngLanguage ( PtrDocument pDoc, PtrElement pEl, Language langue, boolean force );
extern void AttrInitCompteur ( PtrElement pEl, PtrAttribute pAttr, PtrDocument pDoc );
extern void SupprPresAttr ( PtrElement pEl, PtrDocument pDoc, PtrAttribute pAttrSuppr, boolean herit, PtrAttribute pAttrComp );
extern void SupprPresHeriteesAttr ( PtrElement pEl, PtrDocument pDoc, PtrAttribute pAttr );
extern void SupprPresCompareeAttr ( PtrElement pEl, PtrDocument pDoc, PtrAttribute pAttr );
extern PtrAttribute AjouteAttribut ( PtrElement pEl, PtrAttribute pAttrNouv );
extern void ApplAttrToSel ( PtrAttribute pAttrNouv, int dercar, int premcar, PtrElement DerSel, PtrElement PremSel, PtrDocument SelDoc );
extern void AttrRequis ( PtrElement pEl, PtrDocument pDoc );
extern PtrAttribute AttrExceptElem ( PtrElement pEl, int NumExcept );
extern void ChngValAttr ( PtrElement pEl, PtrDocument pDoc, int NouvVal, int NumExcept );
extern void PutAttributs(PtrElement pEl, PtrDocument pDoc, PtrAttribute pNewAttr);

#else /* __STDC__ */

extern PtrAttribute MetAttribut (/* int NumExcept, PtrElement pEl, PtrElement pElRef, PtrDocument pDoc */);
extern void ReaffAttribut (/* PtrAttribute pAttr, PtrElement pEl, PtrDocument pDoc */);
extern void CoupeSelection (/* PtrDocument SelDoc, PtrElement *PremSel, PtrElement *DerSel, int *premcar, int *dercar */);
extern boolean MemesAttributs (/* PtrElement pEl1, PtrElement pEl2 */);
extern void ApplReglesAttr (/* PtrElement pEl, PtrDocument pDoc, PtrAttribute pAttr, boolean herit */);
extern void ApplReglesPresHeriteesAttr (/* PtrElement pEl, PtrDocument pDoc, PtrAttribute pAttr */);
extern void ApplReglesPresCompareesAttr (/* PtrElement pEl, PtrDocument pDoc, PtrAttribute pAttr */);
extern void ChaineLib (/* PtrElement pL, PtrElement *PremLib */);
extern void ChngLanguage (/* PtrDocument pDoc, PtrElement pEl, Language langue, boolean force */);
extern void AttrInitCompteur (/* PtrElement pEl, PtrAttribute pAttr, PtrDocument pDoc */);
extern void SupprPresAttr (/* PtrElement pEl, PtrDocument pDoc, PtrAttribute pAttrSuppr, boolean herit, PtrAttribute pAttrComp */);
extern void SupprPresHeriteesAttr (/* PtrElement pEl, PtrDocument pDoc, PtrAttribute pAttr */);
extern void SupprPresCompareeAttr (/* PtrElement pEl, PtrDocument pDoc, PtrAttribute pAttr */);
extern PtrAttribute AjouteAttribut (/* PtrElement pEl, PtrAttribute pAttrNouv */);
extern void ApplAttrToSel (/* PtrAttribute pAttrNouv, int dercar, int premcar, PtrElement DerSel, PtrElement PremSel, PtrDocument SelDoc */);
extern void AttrRequis (/* PtrElement pEl, PtrDocument pDoc */);
extern PtrAttribute AttrExceptElem (/* PtrElement pEl, int NumExcept */);
extern void ChngValAttr (/* PtrElement pEl, PtrDocument pDoc, int NouvVal, int NumExcept */);
extern void PutAttributs(/*PtrElement pEl, PtrDocument pDoc, PtrAttribute pNewAttr*/);
#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
