
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern PtrDocument DocumentOfElement ( PtrElement pEl );
extern void ProtectElement ( PtrElement pE );
extern PtrElement GetOtherPairedElement ( PtrElement pEl );
extern boolean ElementIsReadOnly ( PtrElement pEl );
extern boolean ElementIsHidden ( PtrElement pEl );
extern PtrElement FwdSearchElemByTypeName ( PtrElement pEl, char *nomType );
extern PtrElement BackSearchElemByTypeName ( PtrElement pEl, char *nomType );
extern PtrElement FwdSearchRefOrEmptyElem ( PtrElement pEl, int Cible );
extern PtrElement BackSearchRefOrEmptyElem ( PtrElement pEl, boolean Cible );
extern void InsertElemAfterLastSibling ( PtrElement ancien, PtrElement nouveau );
extern boolean ElemIsWithinSubtree ( PtrElement pEl, PtrElement pRac );
extern void  AttachRequiredAttributes(PtrElement pEl, SRule *pRe1, PtrSSchema pSS, boolean AvecAttributs, PtrDocument pDoc);
extern boolean EquivalentType ( PtrElement pE, int Typ, PtrSSchema pStr );
extern boolean ElemIsBefore ( PtrElement pEl1, PtrElement pEl2 );
extern boolean ElemIsAnAncestor ( PtrElement p1, PtrElement p2 );
extern PtrElement CommonAncestor ( PtrElement p1, PtrElement p2 );
extern PtrElement FirstLeaf ( PtrElement pEl );
extern PtrElement LastLeaf ( PtrElement pRacine );
extern PtrElement GetTypedAncestor ( PtrElement pElDep, int TypeEl, PtrSSchema pS );
extern PtrElement FwdSearchElem2Types ( PtrElement pEl, int Typ1, int Typ2, PtrSSchema pStr1, PtrSSchema pStr2 );
extern PtrElement FwdSearchTypedElem ( PtrElement pEl, int Typ, PtrSSchema pStr );
extern PtrElement BackSearchElem2Types ( PtrElement pEl, int Typ1, int Typ2, PtrSSchema pStr1, PtrSSchema pStr2 );
extern PtrElement BackSearchTypedElem ( PtrElement pEl, int Typ, PtrSSchema pStr );
extern PtrElement BackSearchVisibleElem ( PtrElement RlRoot, PtrElement pEl, int Vue );
extern PtrElement FwdSearchAttribute ( PtrElement pEl, int Att, int Val, char *ValTxt, PtrSSchema pStr );
extern PtrElement BackSearchAttribute ( PtrElement pEl, int Att, int Val, char *ValTxt, PtrSSchema pStr );
extern void FwdSkipPageBreak ( PtrElement *p );
extern void SkipPageBreakBegin ( PtrElement *p );
extern void BackSkipPageBreak ( PtrElement *p );
extern PtrElement NextElement ( PtrElement pEl );
extern PtrElement PreviousLeaf ( PtrElement pEl );
extern PtrElement NextLeaf ( PtrElement pEl );
extern void InsertElementBefore ( PtrElement ancien, PtrElement nouveau );
extern void InsertElementAfter ( PtrElement ancien, PtrElement nouveau );
extern void InsertFirstChild ( PtrElement ancien, PtrElement nouveau );
extern void InsertElemInChoice ( PtrElement pEl, PtrElement *p, boolean enplace );
extern int NewLabel ( PtrDocument pDoc );
extern int GetCurrentLabel ( PtrDocument pDoc );
extern void SetCurrentLabel ( PtrDocument pDoc, int label );
extern PtrElement NewSubtree ( int TypeElem, PtrSSchema Str, PtrDocument pDoc, int NAssoc, boolean Desc, boolean RlRoot, boolean AvecAttributs, boolean AvecLabel );
extern void RemoveExcludedElem ( PtrElement *pEl );
extern void RemoveElement ( PtrElement El );
extern void RemoveAttribute ( PtrElement pEl, PtrAttribute pAttr );
extern void DeleteAttribute ( PtrElement pEl, PtrAttribute pAttr );
extern void DeleteElement ( PtrElement *El );
extern PtrElement CopyTree ( PtrElement Source, PtrDocument DocSource, int NAssoc, PtrSSchema pSchS, PtrDocument DocCopie, PtrElement pPere, boolean VerifAttr, boolean PartageRef );
extern void CopyIncludedElem ( PtrElement pEl, PtrDocument pDoc );
extern PtrElement ReplicateEleme ( PtrElement El, PtrDocument pDoc );
extern PtrAttribute GetTypedAttrForElem ( PtrElement pEl, int AeAttrNum, PtrSSchema ElAttrSch );
extern PtrAttribute GetTypedAttrAncestor ( PtrElement pEl, int AeAttrNum, PtrSSchema ElAttrSch, PtrElement *pElAttr );
extern void CheckLanguageAttr ( PtrDocument pDoc, PtrElement pEl );

#else /* __STDC__ */

extern PtrDocument DocumentOfElement (/* PtrElement pEl */);
extern void ProtectElement (/* PtrElement pE */);
extern PtrElement GetOtherPairedElement (/* PtrElement pEl */);
extern boolean ElementIsReadOnly (/* PtrElement pEl */);
extern boolean ElementIsHidden (/* PtrElement pEl */);
extern PtrElement FwdSearchElemByTypeName (/* PtrElement pEl, char *nomType */);
extern PtrElement BackSearchElemByTypeName (/* PtrElement pEl, char *nomType */);
extern PtrElement FwdSearchRefOrEmptyElem (/* PtrElement pEl, int Cible */);
extern PtrElement BackSearchRefOrEmptyElem (/* PtrElement pEl, boolean Cible */);
extern void InsertElemAfterLastSibling (/* PtrElement ancien, PtrElement nouveau */);
extern boolean ElemIsWithinSubtree (/* PtrElement pEl, PtrElement pRac */);
extern void  AttachRequiredAttributes(/*PtrElement pEl, SRule *pRe1, PtrSSchema pSS, boolean AvecAttributs, PtrDocument pDoc*/);
extern boolean EquivalentType (/* PtrElement pE, int Typ, PtrSSchema pStr */);
extern boolean ElemIsBefore (/* PtrElement pEl1, PtrElement pEl2 */);
extern boolean ElemIsAnAncestor (/* PtrElement p1, PtrElement p2 */);
extern PtrElement CommonAncestor (/* PtrElement p1, PtrElement p2 */);
extern PtrElement FirstLeaf (/* PtrElement pEl */);
extern PtrElement LastLeaf (/* PtrElement pRacine */);
extern PtrElement GetTypedAncestor (/* PtrElement pElDep, int TypeEl, PtrSSchema pS */);
extern PtrElement FwdSearchElem2Types (/* PtrElement pEl, int Typ1, int Typ2, PtrSSchema pStr1, PtrSSchema pStr2 */);
extern PtrElement FwdSearchTypedElem (/* PtrElement pEl, int Typ, PtrSSchema pStr */);
extern PtrElement BackSearchElem2Types (/* PtrElement pEl, int Typ1, int Typ2, PtrSSchema pStr1, PtrSSchema pStr2 */);
extern PtrElement BackSearchTypedElem (/* PtrElement pEl, int Typ, PtrSSchema pStr */);
extern PtrElement BackSearchVisibleElem (/* PtrElement RlRoot, PtrElement pEl, int Vue */);
extern PtrElement FwdSearchAttribute (/* PtrElement pEl, int Att, int Val, char *ValTxt, PtrSSchema pStr */);
extern PtrElement BackSearchAttribute (/* PtrElement pEl, int Att, int Val, char *ValTxt, PtrSSchema pStr */);
extern void FwdSkipPageBreak (/* PtrElement *p */);
extern void SkipPageBreakBegin (/* PtrElement *p */);
extern void BackSkipPageBreak (/* PtrElement *p */);
extern PtrElement NextElement (/* PtrElement pEl */);
extern PtrElement PreviousLeaf (/* PtrElement pEl */);
extern PtrElement NextLeaf (/* PtrElement pEl */);
extern void InsertElementBefore (/* PtrElement ancien, PtrElement nouveau */);
extern void InsertElementAfter (/* PtrElement ancien, PtrElement nouveau */);
extern void InsertFirstChild (/* PtrElement ancien, PtrElement nouveau */);
extern void InsertElemInChoice (/* PtrElement pEl, PtrElement *p, boolean enplace */);
extern int NewLabel (/* PtrDocument pDoc */);
extern int GetCurrentLabel (/* PtrDocument pDoc */);
extern void SetCurrentLabel (/* PtrDocument pDoc, int label */);
extern PtrElement NewSubtree (/* int TypeElem, PtrSSchema Str, PtrDocument pDoc, int NAssoc, boolean Desc, boolean RlRoot, boolean AvecAttributs, boolean AvecLabel */);
extern void RemoveExcludedElem (/* PtrElement *pEl */);
extern void RemoveElement (/* PtrElement El */);
extern void RemoveAttribute (/* PtrElement pEl, PtrAttribute pAttr */);
extern void DeleteAttribute (/* PtrElement pEl, PtrAttribute pAttr */);
extern void DeleteElement (/* PtrElement *El */);
extern PtrElement CopyTree (/* PtrElement Source, PtrDocument DocSource, int NAssoc, PtrSSchema pSchS, PtrDocument DocCopie, PtrElement pPere, boolean VerifAttr, boolean PartageRef */);
extern void CopyIncludedElem (/* PtrElement pEl, PtrDocument pDoc */);
extern PtrElement ReplicateEleme (/* PtrElement El, PtrDocument pDoc */);
extern PtrAttribute GetTypedAttrForElem (/* PtrElement pEl, int AeAttrNum, PtrSSchema ElAttrSch */);
extern PtrAttribute GetTypedAttrAncestor (/* PtrElement pEl, int AeAttrNum, PtrSSchema ElAttrSch, PtrElement *pElAttr */);
extern void CheckLanguageAttr (/* PtrDocument pDoc, PtrElement pEl */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
