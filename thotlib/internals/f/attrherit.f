
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void CreeTabAttrHerites ( PtrElement pEl );
extern void CreeTabAttrComparant ( PtrAttribute pAttr );
extern void TransmetValElem ( PtrElement pEl, PtrDocument pDoc, Name NomA, int RegleIncl, PtrSSchema pSchS );
extern void ApplReglesTransmit ( PtrElement pElRef, PtrDocument pDoc );
extern void PostApplReglesTransmit ( PtrElement pEltr, PtrElement pElloc, PtrDocument pDoc );

#else /* __STDC__ */

extern void CreeTabAttrHerites (/* PtrElement pEl */);
extern void CreeTabAttrComparant (/* PtrAttribute pAttr */);
extern void TransmetValElem (/* PtrElement pEl, PtrDocument pDoc, Name NomA, int RegleIncl, PtrSSchema pSchS */);
extern void ApplReglesTransmit (/* PtrElement pElRef, PtrDocument pDoc */);
extern void PostApplReglesTransmit (/* PtrElement pEltr, PtrElement pElloc, PtrDocument pDoc */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
