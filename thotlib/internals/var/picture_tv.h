/*
 * Variables des graphiques
 */

/* booleen utilise dans les drivers graphiques specifiques 	*/
EXPORT boolean ImageDriverPrinting;
EXPORT ThotGC GCpicture; /* for bitmap */
EXPORT char *FileExtension[];
EXPORT THOT_VInfo THOT_vInfo;
#ifdef WWW_XWINDOWS
EXPORT	XVisualInfo *vptr;
EXPORT	Visual *theVisual;
#endif /* WWW_XWINDOWS */
