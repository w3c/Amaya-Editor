
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern boolean TypeHasException ( int NumExcept, int TypeEl, PtrSSchema pSS );
extern boolean AttrHasException ( int NumExcept, int Attr, PtrSSchema pSS );
extern int GetAttrWithException ( int NumExcept, PtrSSchema pSS );
extern int GetElemWithException ( int NumExcept, PtrSSchema pSS );
extern boolean ExcAttrib ( int NumExcept, PtrElement pEl );
extern PtrDocument SearchFirstDocExcepted ( int NumExcept );
extern PtrElement ExceptionIsAbove ( int NumExcept, PtrElement pEl, PtrDocument pDoc );

#else /* __STDC__ */

extern boolean TypeHasException (/* int NumExcept, int TypeEl, PtrSSchema pSS */);
extern boolean AttrHasException (/* int NumExcept, int Attr, PtrSSchema pSS */);
extern int GetAttrWithException (/* int NumExcept, PtrSSchema pSS */);
extern int GetElemWithException (/* int NumExcept, PtrSSchema pSS */);
extern boolean ExcAttrib (/* int NumExcept, PtrElement pEl */);
extern PtrDocument SearchFirstDocExcepted (/* int NumExcept */);
extern PtrElement ExceptionIsAbove (/* int NumExcept, PtrElement pEl, PtrDocument pDoc */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
