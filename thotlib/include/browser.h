/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Warning:
 * This module is part of the Thot library, which was originally
 * developed in French. That's why some comments are still in
 * French, but their translation is in progress and the full module
 * will be available in English in the next release.
 * 
 */
 
#ifndef _BROWSER_H_
#define _BROWSER_H_

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void         TtaListDirectory (char *aDirectory, int formRef, char *dirTitle, int dirRef, char *suffix, char *fileTitle, int fileRef);
extern boolean      TtaIsSuffixFileIn (char *aDirectory, char *suffix);

#else  /* __STDC__ */

extern void         TtaListDirectory ( /* char *aDirectory, int formRef, char *dirTitle, int dirRef, char *suffix, char *fileTitle, int fileRef */ );
extern boolean      TtaIsSuffixFileIn ( /*char *aDirectory, char *suffix */ );

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */

#endif
