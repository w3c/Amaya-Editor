/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

EXPORT boolean       ImageDriverPrinting;
EXPORT ThotGC        GCpicture;	 /* for bitmap */
EXPORT char*         FileExtension[];
EXPORT THOT_VInfo    THOT_vInfo;
EXPORT int           HandlersCounter ;
EXPORT int           InlineHandlers ;
#ifdef WWW_XWINDOWS
EXPORT	XVisualInfo* vptr;
EXPORT	Visual*      theVisual;
#endif /* WWW_XWINDOWS */
