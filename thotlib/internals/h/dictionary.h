/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 

#ifndef _DICTIONARY_H_
#define _DICTIONARY_H_

#include "typebase.h"

#define MAX_LANGUAGES 20
#define MAX_DICTS        2  /* Maximum number of dictionaries related to a given language */

#define MAX_PATTERN	5000  	/* nbre max de pattern dans une langue */
#define MAX_LET_PATTERN	13	/* nbre max de lettres dans un pattern +1 */
#define MAX_POINT_COUP	8

#define ISHYPHENABLE(c) ((c) == '1' || (c) =='3' || (c) == '5' || (c) == '7' || (c) == '9')

typedef int *PatternListe ;

typedef struct _PatternList *PtrPattern;

struct Pattern
{
	unsigned char	CarPattern[MAX_LET_PATTERN];
	char	PoidsPattern[MAX_LET_PATTERN];
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
  char		LangNom[MAX_NAME_LENGTH];		/* Name de la langue */
  char		LangAlphabet;		/* Alphabet associe a la langue */
  char		LangPrincipal[MAX_NAME_LENGTH];	/* Name du dictionnaire principal */
  char		LangSecondary[MAX_NAME_LENGTH];	/* Name du dictionnaire secondaire */
  Dictionary    LangDict[MAX_DICTS];    /* Pointeurs vers les dictionnaires */
  char		LangPattern[MAX_NAME_LENGTH];	/* Name de la liste de patterns d'hyphenation */
  struct PatternList	LangTabPattern;		/* Pointeur sur la liste de patterns */
};

#ifndef __CEXTRACT__
#ifdef __STDC__

extern boolean TtaLoadDocumentDictionary ( PtrDocument document, int *pDict, boolean ToCreate );
extern char *TtaGetHyphenList ( char word[30], Language languageId );
extern int *TtaGetPatternHyphenList( char word[100], Language languageId );
extern boolean TtaExistPatternList ( Language languageId );

#else /* __STDC__ */

extern boolean TtaLoadDocumentDictionary (/* PtrDocument document, int *pDict, boolean ToCreate */);
extern char *TtaGetHyphenList (/* char word[30], Language languageId */);
extern int *TtaGetPatternHyphenList(/* char word[100], Language languageId */);
extern boolean TtaExistPatternList (/* Language languageId */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */

#endif
