/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/** typecorr.h **/

#ifndef _TYPECHECKER_H_
#define _TYPECHECKER_H_

#define NbLtr     120  /* maxm number of letters in the alphabet < 256 isolatin1 */

typedef char     FileName[MAX_NAME_LENGTH];    /* filename */

typedef  char   *PtrString;   /* pointer to the word list                     */
typedef  char   *PtrCommons;  /* pointer to the list of shared prefix length  */
typedef  int    *PtrWords;    /* pointer to the list of pointers to the words */

typedef struct _WordDict *PtrDict;

/* a dictionary */
typedef struct _WordDict
{
      PtrDict     DictNext;             /* for linking to free blocks */
      PtrDocument DictDoc;              /* document which uses this dictionary */
      Name        DictName;             /* name of the dictionary */
      PtrString   DictDirectory;        /* directory of the dictionary */
      ThotBool    DictReadOnly;         /* dictionary is read only  */
      ThotBool    DictLoaded;           /* dictionary is loaded */
      ThotBool    DictModified;         /* dictionnary has been modified */
      Language    DictLanguage;         /* the language of the dictionary */
      int         DictMaxChars;    /* max characters for string and for prefix */
      int         DictMaxWords;    /* max allocated integers for pdict */
      PtrString   DictString;      /* pointer to the characters of the dictionary */
      PtrWords    DictWords;       /* pointer to the list of pointers */
                                   /*  to the words of the dictionary */
      PtrCommons  DictCommon;      /* pointer to the list of common letters */
      int         DictLengths[MAX_WORD_LEN + 1]; /* pointer to the words having the same size */
      int         DictNbWords;     /*number of words in the dictionary */
      int         DictNbChars;     /* real number of characters in string */

} WordDict;
 
#endif




 
