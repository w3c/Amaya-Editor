/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/* instancied in picture.c */
extern ThotGC          GCpicture;	 /* for bitmap */
THOT_EXPORT ThotGC     GCimage;	 	/* for pixmap */
//extern char           *FileExtension[]; 
extern int             HandlersCounter;
extern int             InlineHandlers;

#ifndef _WINGUI
THOT_EXPORT ThotVisual     *theVisual;
#else  /* _WINGUI */
THOT_EXPORT void           *theVisual;
#endif /* _WINGUI */

#if defined _GTK || defined _WINGUI || defined(_GL)
THOT_EXPORT char            LostPicturePath [512];
#endif 

THOT_EXPORT PictureHandler  PictureHandlerTable[MAX_PICTURE_DRIVER];
THOT_EXPORT int             PictureIdType[MAX_PICTURE_DRIVER];
THOT_EXPORT int             InlineHandlers;
THOT_EXPORT int             HandlersCounter;
THOT_EXPORT int             currentExtraHandler;
THOT_EXPORT ThotGC          GCpicture;
THOT_EXPORT ThotPixmap      EpsfPictureLogo;

/* bg colors used by Windows */
#ifdef _WINGUI
THOT_EXPORT ThotBool        bgAlpha;
THOT_EXPORT HBITMAP         alphamap;
#endif /* _WINGUI */
