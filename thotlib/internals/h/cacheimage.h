
typedef struct _CacheElem *PtrCacheElem;

/* fonctions d'acces au cache */
void InitCacheMem();
void CachePutImage(/* pEl, vue, imdesc */);
PtrCacheElem  CacheIsOk(/* pEl, vue, imdesc, wif, hif */);
Pixmap CacheGetImage(/* pCache  */);
void CacheRemoveImage(/* pEl */);

