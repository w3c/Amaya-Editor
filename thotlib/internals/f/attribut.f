
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern PtrAttribute AttachAttrByExceptNum ( int NumExcept, PtrElement pEl, PtrElement pElRef, PtrDocument pDoc );
extern void RedisplayAttribute ( PtrAttribute pAttr, PtrElement pEl, PtrDocument pDoc );
extern void CutSelection ( PtrDocument SelDoc, PtrElement *PremSel, PtrElement *DerSel, int *premcar, int *dercar );
extern boolean MemesAttributs ( PtrElement pEl1, PtrElement pEl2 );
extern void ApplyAttrPRulesToElem ( PtrElement pEl, PtrDocument pDoc, PtrAttribute pAttr, boolean herit );
extern void ApplyAttrPRulesToSubtree ( PtrElement pEl, PtrDocument pDoc, PtrAttribute pAttr );
extern void ApplyAttrPRules ( PtrElement pEl, PtrDocument pDoc, PtrAttribute pAttr );
extern void KeepFreeElements ( PtrElement pL, PtrElement *PremLib );
extern void ChangeLanguage ( PtrDocument pDoc, PtrElement pEl, Language langue, boolean force );
extern void UpdateCountersByAttr ( PtrElement pEl, PtrAttribute pAttr, PtrDocument pDoc );
extern void RemoveAttrPresentation ( PtrElement pEl, PtrDocument pDoc, PtrAttribute pAttrSuppr, boolean herit, PtrAttribute pAttrComp );
extern void RemoveInheritedAttrPresent ( PtrElement pEl, PtrDocument pDoc, PtrAttribute pAttr );
extern void RemoveComparAttrPresent ( PtrElement pEl, PtrDocument pDoc, PtrAttribute pAttr );
extern PtrAttribute AddAttrToElem ( PtrElement pEl, PtrAttribute pAttrNouv );
extern void AttachAttrToRange ( PtrAttribute pAttrNouv, int dercar, int premcar, PtrElement DerSel, PtrElement PremSel, PtrDocument SelDoc );
extern void AttachMandatoryAttributes ( PtrElement pEl, PtrDocument pDoc );
extern PtrAttribute GetAttrByExceptNum ( PtrElement pEl, int NumExcept );
extern void SetAttrValueByExceptNum ( PtrElement pEl, PtrDocument pDoc, int NouvVal, int NumExcept );
extern void AttachAttrWithValue(PtrElement pEl, PtrDocument pDoc, PtrAttribute pNewAttr);

#else /* __STDC__ */

extern PtrAttribute AttachAttrByExceptNum (/* int NumExcept, PtrElement pEl, PtrElement pElRef, PtrDocument pDoc */);
extern void RedisplayAttribute (/* PtrAttribute pAttr, PtrElement pEl, PtrDocument pDoc */);
extern void CutSelection (/* PtrDocument SelDoc, PtrElement *PremSel, PtrElement *DerSel, int *premcar, int *dercar */);
extern boolean MemesAttributs (/* PtrElement pEl1, PtrElement pEl2 */);
extern void ApplyAttrPRulesToElem (/* PtrElement pEl, PtrDocument pDoc, PtrAttribute pAttr, boolean herit */);
extern void ApplyAttrPRulesToSubtree (/* PtrElement pEl, PtrDocument pDoc, PtrAttribute pAttr */);
extern void ApplyAttrPRules (/* PtrElement pEl, PtrDocument pDoc, PtrAttribute pAttr */);
extern void KeepFreeElements (/* PtrElement pL, PtrElement *PremLib */);
extern void ChangeLanguage (/* PtrDocument pDoc, PtrElement pEl, Language langue, boolean force */);
extern void UpdateCountersByAttr (/* PtrElement pEl, PtrAttribute pAttr, PtrDocument pDoc */);
extern void RemoveAttrPresentation (/* PtrElement pEl, PtrDocument pDoc, PtrAttribute pAttrSuppr, boolean herit, PtrAttribute pAttrComp */);
extern void RemoveInheritedAttrPresent (/* PtrElement pEl, PtrDocument pDoc, PtrAttribute pAttr */);
extern void RemoveComparAttrPresent (/* PtrElement pEl, PtrDocument pDoc, PtrAttribute pAttr */);
extern PtrAttribute AddAttrToElem (/* PtrElement pEl, PtrAttribute pAttrNouv */);
extern void AttachAttrToRange (/* PtrAttribute pAttrNouv, int dercar, int premcar, PtrElement DerSel, PtrElement PremSel, PtrDocument SelDoc */);
extern void AttachMandatoryAttributes (/* PtrElement pEl, PtrDocument pDoc */);
extern PtrAttribute GetAttrByExceptNum (/* PtrElement pEl, int NumExcept */);
extern void SetAttrValueByExceptNum (/* PtrElement pEl, PtrDocument pDoc, int NouvVal, int NumExcept */);
extern void AttachAttrWithValue(/*PtrElement pEl, PtrDocument pDoc, PtrAttribute pNewAttr*/);
#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
