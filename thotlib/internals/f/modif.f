/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void InitSave ( void );
extern boolean DansTampon ( PtrElement pEl );
extern boolean BreakElement ( PtrElement ElemADupliquer, PtrElement ElemCoupure, int carcoupure, boolean Paragraph);
extern void ReaffPaveCopie ( PtrElement pEl, PtrDocument pDoc, boolean reaff );
extern boolean EstNouveau ( PtrElement pE );
extern boolean DansNouveaux ( PtrElement pE );
extern void PurgeRef ( PtrElement pRac, PtrDocument pDoc );
extern void PresAttrRef ( PtrElement pRac, PtrDocument pDoc );
extern void CalculeIdentPaire ( PtrElement pRac, PtrDocument pDoc );
extern void ReaffRef ( PtrElement pElR, PtrDocument *docsel, boolean reaff );
extern void RetablitPaires ( PtrElement *pEl, PtrElement pSuiv, PtrElement pPrec, PtrElement pPere, PtrDocument pDoc );
extern void Holophraster ( void );
extern void DeHolophraste ( PtrElement pEl, PtrDocument pDoc );
extern void NewTextLanguage ( PtrAbstractBox pav, int indexc, Language langue );
extern boolean	 CompleteElement(PtrElement pEl, PtrDocument pDoc);
extern void FusEtSel ( PtrDocument SelDoc, PtrElement PremSel, PtrElement DerSel, int premcar, int dercar );
#else /* __STDC__ */

extern void InitSave (/* void */);
extern boolean DansTampon (/* PtrElement pEl */);
extern boolean BreakElement (/* PtrElement ElemADupliquer, PtrElement ElemCoupure, int carcoupure, boolean Paragraph */);
extern void ReaffPaveCopie (/* PtrElement pEl, PtrDocument pDoc, boolean reaff */);
extern boolean EstNouveau (/* PtrElement pE */);
extern boolean DansNouveaux (/* PtrElement pE */);
extern void PurgeRef (/* PtrElement pRac, PtrDocument pDoc */);
extern void PresAttrRef (/* PtrElement pRac, PtrDocument pDoc */);
extern void CalculeIdentPaire (/* PtrElement pRac, PtrDocument pDoc */);
extern void ReaffRef (/* PtrElement pElR, PtrDocument *docsel, boolean reaff */);
extern void RetablitPaires (/* PtrElement *pEl, PtrElement pSuiv, PtrElement pPrec, PtrElement pPere, PtrDocument pDoc */);
extern void Holophraster (/* void */);
extern void DeHolophraste (/* PtrElement pEl, PtrDocument pDoc */);
extern void NewTextLanguage (/* PtrAbstractBox pav, int indexc, Language langue */);
extern boolean	 CompleteElement(/*PtrElement pEl, PtrDocument pDoc*/);
extern void FusEtSel (/* PtrDocument SelDoc, PtrElement PremSel, PtrElement DerSel, int premcar, int dercar */);
#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
