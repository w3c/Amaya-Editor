/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/** typecorr.h **/

#ifndef _TYPECHECKER_H_
#define _TYPECHECKER_H_

#define NbLtr     120  /* maxm number of letters in the alphabet  < 256 isolatin1  */

typedef char    FileName[MAX_NAME_LENGTH];    /* filename */

typedef  char *PtrChaine;   /* pointer to the word list */
typedef  char *PtrCommuns;  /* pointer to the list of shared prefix length */
typedef int *PtrMots;   /* pointer to the list of pointers to the words */

typedef struct _Dictionnaire *PtrDict;

/* a dictionary */
typedef struct _Dictionnaire
{
      PtrDict     DictSuivant;              /* for linking to free blocks */
      PtrDocument DictDoc;                  /* document which uses this dictionary */
      Name         DictNom;                 /* name of the dictionary */
      Buffer      DictDirectory;            /* directory of the dictionary */
      boolean     DictReadOnly;             /* dictionary is read only  */
      boolean     DictCharge;               /* dictionary is loaded */
      boolean     DictModifie;              /* dictionnary has been modified */
      Language    DictLangue;               /* the language of the dictionary */
       int MAXcars;       /* max characters for string and for prefix */
       int MAXmots;       /* max allocated integers for pdict */
      PtrChaine   chaine;       /* pointer to the characters of the dictionary */
      PtrMots     pdico;                /* pointer to the list of pointers */
                                       /*  to the words of the dictionary */
      PtrCommuns  commun;     /* pointer to the list of common letters */
      int         plgdico[MAX_WORD_LEN];     /* pointer to the words having the same size */
      int         nbmots;     /*number of words in the dictionary */
      int         nbcars;     /* real number of characters in string */

}    Dictionnaire;
 
#endif




 
