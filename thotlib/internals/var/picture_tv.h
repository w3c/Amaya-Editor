/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

THOT_EXPORT ThotGC          GCpicture;	 /* for bitmap */
THOT_EXPORT ThotGC          GCimage;	 /* for pixmap */
THOT_EXPORT char*           FileExtension[];
THOT_EXPORT int             HandlersCounter;
THOT_EXPORT int             InlineHandlers;
#ifndef _WINDOWS
THOT_EXPORT Visual         *theVisual;
#else  /* _WINDOWS */
THOT_EXPORT void           *theVisual;
THOT_EXPORT char            LostPicturePath [512];
#endif /* _WINDOWS */
THOT_EXPORT PictureHandler  PictureHandlerTable[MAX_PICT_FORMATS];
THOT_EXPORT int             PictureIdType[MAX_PICT_FORMATS];
THOT_EXPORT int             PictureMenuType[MAX_PICT_FORMATS];
THOT_EXPORT int             InlineHandlers;
THOT_EXPORT int             HandlersCounter;
THOT_EXPORT int             currentExtraHandler;
THOT_EXPORT ThotGC          GCpicture;
THOT_EXPORT Pixmap          EpsfPictureLogo;

/* bg colors used by Windows */#ifdef _WINDOWS
THOT_EXPORT int             bgRed;
THOT_EXPORT int             bgGreen;
THOT_EXPORT int             bgBlue;
THOT_EXPORT ThotBool        bgAlpha;
THOT_EXPORT HBITMAP         alphamap;
#endif /* _WINDOWS */
