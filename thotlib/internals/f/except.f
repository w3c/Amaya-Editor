
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern boolean ExceptTypeElem ( int NumExcept, int TypeEl, PtrSSchema pSS );
extern boolean ExceptAttr ( int NumExcept, int Attr, PtrSSchema pSS );
extern int ExceptNumAttr ( int NumExcept, PtrSSchema pSS );
extern int ExceptNumType ( int NumExcept, PtrSSchema pSS );
extern boolean ExcAttrib ( int NumExcept, PtrElement pEl );
extern PtrDocument SearchFirstDocExcepted ( int NumExcept );
extern PtrElement ExceptionIsAbove ( int NumExcept, PtrElement pEl, PtrDocument pDoc );

#else /* __STDC__ */

extern boolean ExceptTypeElem (/* int NumExcept, int TypeEl, PtrSSchema pSS */);
extern boolean ExceptAttr (/* int NumExcept, int Attr, PtrSSchema pSS */);
extern int ExceptNumAttr (/* int NumExcept, PtrSSchema pSS */);
extern int ExceptNumType (/* int NumExcept, PtrSSchema pSS */);
extern boolean ExcAttrib (/* int NumExcept, PtrElement pEl */);
extern PtrDocument SearchFirstDocExcepted (/* int NumExcept */);
extern PtrElement ExceptionIsAbove (/* int NumExcept, PtrElement pEl, PtrDocument pDoc */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
