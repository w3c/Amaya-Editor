
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void CreateInheritedAttrTable ( PtrElement pEl );
extern void CreateComparAttrTable ( PtrAttribute pAttr );
extern void TransmitElementContent ( PtrElement pEl, PtrDocument pDoc, Name NomA, int RegleIncl, PtrSSchema pSchS );
extern void ApplyTransmitRules ( PtrElement pElRef, PtrDocument pDoc );
extern void RepApplyTransmitRules ( PtrElement pEltr, PtrElement pElloc, PtrDocument pDoc );

#else /* __STDC__ */

extern void CreateInheritedAttrTable (/* PtrElement pEl */);
extern void CreateComparAttrTable (/* PtrAttribute pAttr */);
extern void TransmitElementContent (/* PtrElement pEl, PtrDocument pDoc, Name NomA, int RegleIncl, PtrSSchema pSchS */);
extern void ApplyTransmitRules (/* PtrElement pElRef, PtrDocument pDoc */);
extern void RepApplyTransmitRules (/* PtrElement pEltr, PtrElement pElloc, PtrDocument pDoc */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
