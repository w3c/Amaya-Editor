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


/*----------------------------------------------------------------------
   TtaListDirectory
   reads the content of a directory.
   aDirectory specifies the directory to read  (!=/afs). 
   dirTitle gives the title of the directory selector.
   formRef is the reference to the form we are using.
   dirRef is the reference to the directory selector.
   suffix gives the sort suffix for the files
   fileTitle gives the title of the files selector
   fileRef is the reference of the files selector
   If dirRef or fileRef have a negative values, the corresponding selector
   won't be created.
   If aDirectory doesn't exist, the selectors will be empty.
  ----------------------------------------------------------------------*/
extern void         TtaListDirectory (STRING aDirectory, int formRef, STRING dirTitle, int dirRef, STRING suffix, STRING fileTitle, int fileRef);

/*----------------------------------------------------------------------
   TtaIsSuffixFileIn
   returns TRUE if the directory contains any file with the requested
   suffix.
  ----------------------------------------------------------------------*/
extern ThotBool     TtaIsSuffixFileIn (char* aDirectory, char *suffix);

#else  /* __STDC__ */

extern void         TtaListDirectory ( /* char *aDirectory, int formRef, char *dirTitle, int dirRef, char *suffix, char *fileTitle, int fileRef */ );
extern ThotBool     TtaIsSuffixFileIn ( /*char *aDirectory, char *suffix */ );

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */

#endif
