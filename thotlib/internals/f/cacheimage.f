
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void InitCacheMem ( void );
extern void CachePutImage ( PtrElement pEl, int vue, PictInfo imdesc );
extern PtrCacheElem CacheIsOk ( PtrElement pEl, int vue, PictInfo imdesc, int wif, int hif );
extern Pixmap CacheGetImage ( PtrCacheElem pCache );
extern void CacheRemoveImage ( PtrElement pEl );

#else /* __STDC__ */

extern void InitCacheMem (/* void */);
extern void CachePutImage (/* PtrElement pEl, int vue, PictInfo imdesc */);
extern PtrCacheElem CacheIsOk (/* PtrElement pEl, int vue, PictInfo imdesc, int wif, int hif */);
extern Pixmap CacheGetImage (/* PtrCacheElem pCache */);
extern void CacheRemoveImage (/* PtrElement pEl */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
