/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifndef _BROWSER_H_
#define _BROWSER_H_

#ifndef __CEXTRACT__

/*----------------------------------------------------------------------
   TtaExtractSuffix extract suffix from a file nane.                
  ----------------------------------------------------------------------*/
extern void TtaExtractSuffix (char *aName, char *aSuffix);

/*----------------------------------------------------------------------
  TtaStrAddSorted
  Adds new entry in table. Table is a linear array with strings
  separated by EOS. The sort is done with a strcmp. This means
  that digits are sorted alphabetically, rather than numerically.
  Parameters:
  new_entry : string to be added
  table : string linear table
  ptr_last : ptr to the last used byte in the table
  nb_entries : number of entries in the table
  ----------------------------------------------------------------------*/
extern void TtaStrAddSorted (char *new_entry, char *table, char *ptr_last, int nb_entries);

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
#endif /* __CEXTRACT__ */

#endif
