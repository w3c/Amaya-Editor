
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void ChargeExt ( BinFile fich, PtrDocument pDocu, PtrReferredDescr *Ancre, boolean LabelSeul );
extern void ChargeRef ( BinFile fich, PtrChangedReferredEl *Ancre );
extern void MiseAJourRef ( PtrChangedReferredEl Ancre, PtrDocument pDoc );
extern void LabelIntToString(int num, LabelString strn);
extern void rdLabel(char c, LabelString lab, BinFile fich);

#else /* __STDC__ */

extern void ChargeExt (/* BinFile fich, PtrDocument pDocu, PtrReferredDescr *Ancre, boolean LabelSeul */);
extern void ChargeRef (/* BinFile fich, PtrChangedReferredEl *Ancre */);
extern void MiseAJourRef (/* PtrChangedReferredEl Ancre, PtrDocument pDoc */);
extern void LabelIntToString(/*int num, LabelString strn*/);
extern void rdLabel(/*char c, LabelString lab, BinFile fich*/);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
