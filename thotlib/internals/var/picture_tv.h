/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

THOT_EXPORT boolean       ImageDriverPrinting;
THOT_EXPORT ThotGC        GCpicture;	 /* for bitmap */
THOT_EXPORT ThotGC        GCimage;	 /* for pixmap */
THOT_EXPORT char*         FileExtension[];
THOT_EXPORT THOT_VInfo    THOT_vInfo;
THOT_EXPORT int           HandlersCounter;
THOT_EXPORT int           InlineHandlers;
#ifndef _WINDOWS
THOT_EXPORT	XVisualInfo* vptr;
THOT_EXPORT	Visual*      theVisual;
#else  /* _WINDOWS */
THOT_EXPORT	void*        theVisual;
THOT_EXPORT char         LostPicturePath [512];
#endif /* _WINDOWS */
