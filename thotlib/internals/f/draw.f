
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern boolean DrawEtendSelection ( PtrElement pEl, PtrDocument pDoc );
extern void DrawAjAttr ( PtrAttribute *pAttr, PtrElement pEl );
extern void DrawSupprAttr ( PtrAttribute pAttr, PtrElement pEl );

#else /* __STDC__ */

extern boolean DrawEtendSelection (/* PtrElement pEl, PtrDocument pDoc */);
extern boolean TraiteAjAttr_DrawAjAttr (/* Ensemble Except, PtrElement *pEl, PtrAttribute *pAttr */);
extern void DrawAjAttr (/* PtrAttribute *pAttr, PtrElement pEl */);
extern void DrawSupprAttr (/* PtrAttribute pAttr, PtrElement pEl */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
