/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996-2001.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifndef _BROWSER_H_
#define _BROWSER_H_

#ifndef __CEXTRACT__

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
extern void TtaListDirectory (char *aDirectory, int formRef, char *dirTitle,
			      int dirRef, char *suffix, char *fileTitle, int fileRef);

/*----------------------------------------------------------------------
   TtaIsSuffixFileIn
   returns TRUE if the directory contains any file with the requested
   suffix.
  ----------------------------------------------------------------------*/
extern ThotBool TtaIsSuffixFileIn (char *aDirectory, char *suffix);

#endif /* __CEXTRACT__ */

#endif
