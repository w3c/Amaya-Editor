/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
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
