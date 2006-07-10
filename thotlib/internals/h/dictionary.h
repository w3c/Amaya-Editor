/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2005
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
 

#ifndef _DICTIONARY_H_
#define _DICTIONARY_H_

#include "typebase.h"

#define MAX_LANGUAGES   22	/* Maximum number of languages handled
				   simultaneously */
#define MAX_DICTS       2	/* Maximum number of dictionaries related to a
				   given language */

#define MAX_PATTERN	5000  	/* Maximum number of hyphenation patterns in
				   a language */
#define MAX_LET_PATTERN	20	/* Maximum number of characters in a
				   hyphenation pattern +1 */
#define MAX_POINT_COUP	8	/* Maximum number of hyphenation positions */

#define ISHYPHENABLE(c) ((c) == '1' || (c) =='3' || (c) == '5' || (c) == '7' || (c) == '9')

typedef int *PatternListe ;

typedef struct _PatternList *PtrPattern;

struct Pattern
{
  unsigned char CarPattern[MAX_LET_PATTERN];
  char          PoidsPattern[MAX_LET_PATTERN];
};

struct PatternList
{
  int	Charge; 	/* booleen si patterns de langue charges */
  int	NbPatt;		/* Nbre effectif de pattern */
  int	ind_pattern[MAX_LET_PATTERN];
                                /* indice de patterns de meme taille */
  struct Pattern	liste_pattern[MAX_PATTERN];
                                /* Tableau des patterns et poids */
};

struct Langue_Ctl
{
  char        LangName[MAX_NAME_LENGTH];      /* Nom de la langue                     */
  char        LangCode[MAX_NAME_LENGTH];      /* Code de la langue (RFC1766)          */
  char        LangScript;                     /* Script associe a la langue           */
  char        LangPrincipal[MAX_NAME_LENGTH]; /* Name du dictionnaire principal       */
  char        LangSecondary[MAX_NAME_LENGTH]; /* Name du dictionnaire secondaire      */
  Dictionary  LangDict[MAX_DICTS];            /* Pointeurs vers les dictionnaires     */
  char        LangPattern[MAX_NAME_LENGTH];   /* Name de la liste de patterns d'hyphenation */
  struct PatternList LangTabPattern;          /* Pointeur sur la liste de patterns   */
};

#endif
