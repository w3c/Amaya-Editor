/*
 * Copyright (c) 1996 INRIA, All rights reserved
 */

/*----------------------------------------------------------------------
   
   Application Program Interface                     
   --->Language managment                                          
  ----------------------------------------------------------------------*/

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "libmsg.h"
#include "message.h"
#include "language.h"
#include "application.h"
#include "dictionary.h"

/* Variables recuperees de la gestion des dictionaires */
struct Langue_Ctl   LangTable[MAX_LANGUAGES];
struct Langue_Ctl   TypoLangTable[MAX_LANGUAGES];
int                 FreeEntry;
int                 FreeTypoEntry;
static char         Langbuffer[2 * MAX_NAME_LENGTH];
static int          breakPoints[MAX_POINT_COUP];
static char         StandardLANG[3];

#include "thotmsg_f.h"

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         InitTypoLanguage ()
#else  /* __STDC__ */
static void         InitTypoLanguage ()
#endif				/* __STDC__ */

{
   int                 i, j;

   /* Initialisation du reste de la table */
   for (i = 0; i < MAX_LANGUAGES; i++)
     {
	TypoLangTable[i].LangNom[0] = '\0';
	TypoLangTable[i].LangAlphabet = 'L';
	for (j = 0; j < MAX_DICTS; j++)
	   TypoLangTable[i].LangDict[j] = NULL;
	TypoLangTable[i].LangPattern[0] = '\0';
	TypoLangTable[i].LangTabPattern.Charge = 0;
     }
   strcpy (TypoLangTable[0].LangNom, "F-Short");
   TypoLangTable[0].LangAlphabet = 'L';
   strcpy (TypoLangTable[0].LangPrincipal, "Fabbrev");
   strcpy (TypoLangTable[0].LangSecondary, "Ftrabbrev");

   strcpy (TypoLangTable[1].LangNom, "E-Short");
   TypoLangTable[1].LangAlphabet = 'L';
   strcpy (TypoLangTable[1].LangPrincipal, "Eabbrev");
   strcpy (TypoLangTable[1].LangSecondary, "Etrabbrev");

   strcpy (TypoLangTable[2].LangNom, "F-Acronym");
   TypoLangTable[2].LangAlphabet = 'L';
   strcpy (TypoLangTable[2].LangPrincipal, "Facronym");
   strcpy (TypoLangTable[2].LangSecondary, "Ftracronym");

   strcpy (TypoLangTable[3].LangNom, "E-Acronym");
   TypoLangTable[3].LangAlphabet = 'L';
   strcpy (TypoLangTable[3].LangPrincipal, "Eacronym");
   strcpy (TypoLangTable[3].LangSecondary, "Etracronym");

   strcpy (TypoLangTable[4].LangNom, "F-Expo1");
   TypoLangTable[4].LangAlphabet = 'L';
   strcpy (TypoLangTable[4].LangPrincipal, "Fexpo");
   strcpy (TypoLangTable[4].LangSecondary, "Ftrexpo");

   strcpy (TypoLangTable[5].LangNom, "F-Expo2");
   TypoLangTable[5].LangAlphabet = 'L';
   strcpy (TypoLangTable[5].LangPrincipal, "Fexpolex");
/*  strcpy(TypoLangTable[5].LangSecondary, "Fexpo2");
 */

   FreeTypoEntry = 6;

}				

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                InitLanguage ()

#else  /* __STDC__ */
void                InitLanguage ()
#endif				/* __STDC__ */

{
   int                 i, j;

   /* Initialization of the remaining of the table */
   for (i = 0; i < MAX_LANGUAGES; i++)
     {
	LangTable[i].LangNom[0] = '\0';
	LangTable[i].LangAlphabet = 'L';
	for (j = 0; j < MAX_DICTS; j++)
	   LangTable[i].LangDict[j] = NULL;
	LangTable[i].LangPattern[0] = '\0';
	LangTable[i].LangTabPattern.Charge = 0;
     }
   /* Loading the default languages */
   strcpy (LangTable[0].LangNom, "ISO_latin_1");
   LangTable[0].LangAlphabet = 'L';
   strcpy (LangTable[0].LangPrincipal, "Usigle");
   strcpy (LangTable[0].LangSecondary, "Uname");

   strcpy (LangTable[1].LangNom, "Fran\347ais");
   LangTable[1].LangAlphabet = 'L';
   strcpy (LangTable[1].LangPrincipal, "Fprinc");
   strcpy (LangTable[1].LangSecondary, "Fperso");
   strcpy (LangTable[1].LangPattern, "francais.ptn");

   strcpy (LangTable[2].LangNom, "English");
   LangTable[2].LangAlphabet = 'L';
   strcpy (LangTable[2].LangPrincipal, "Eprinc");
   strcpy (LangTable[2].LangSecondary, "Eperso");
   strcpy (LangTable[2].LangPattern, "english.ptn");

   strcpy (LangTable[3].LangNom, "American");
   LangTable[3].LangAlphabet = 'L';
   strcpy (LangTable[3].LangPrincipal, "Eprinc");
   strcpy (LangTable[3].LangSecondary, "Eperso");
   strcpy (LangTable[3].LangPattern, "american.ptn");

   strcpy (LangTable[4].LangNom, "Deutsch");
   LangTable[4].LangAlphabet = 'L';
   strcpy (LangTable[4].LangPrincipal, "Gprinc");
   LangTable[4].LangSecondary[0] = '\0';
   strcpy (LangTable[4].LangPattern, "deutsch.ptn");

   strcpy (LangTable[5].LangNom, "Italiano");
   LangTable[5].LangAlphabet = 'L';
   strcpy (LangTable[5].LangPrincipal, "Iprinc");
   LangTable[5].LangSecondary[0] = '\0';
   strcpy (LangTable[5].LangPattern, "italiano.ptn");

   strcpy (LangTable[6].LangNom, "Espa\361ol");
   LangTable[6].LangAlphabet = 'L';
   strcpy (LangTable[6].LangPrincipal, "Sprinc");
   LangTable[6].LangSecondary[0] = '\0';
   strcpy (LangTable[6].LangPattern, "espanol.ptn");

   strcpy (LangTable[7].LangNom, "Portuguese");
   LangTable[7].LangAlphabet = 'L';
   LangTable[7].LangPrincipal[0] = '\0';
   LangTable[7].LangSecondary[0] = '\0';
   strcpy (LangTable[7].LangPattern, "portug.ptn");

   strcpy (LangTable[8].LangNom, "Dutch");
   LangTable[8].LangAlphabet = 'L';
   strcpy (LangTable[8].LangPrincipal, "Nprinc");
   LangTable[8].LangSecondary[0] = '\0';
   strcpy (LangTable[8].LangPattern, "nederl.ptn");

   strcpy (LangTable[9].LangNom, "Swedish");
   LangTable[9].LangAlphabet = 'L';
   strcpy (LangTable[9].LangPrincipal, "Wprinc");
   LangTable[9].LangSecondary[0] = '\0';
   strcpy (LangTable[9].LangPattern, "swedish.ptn");

   strcpy (LangTable[10].LangNom, "Finnish");
   LangTable[10].LangAlphabet = 'L';
   LangTable[10].LangPrincipal[0] = '\0';
   LangTable[10].LangSecondary[0] = '\0';
   strcpy (LangTable[10].LangPattern, "finish.ptn");

   strcpy (LangTable[11].LangNom, "Symbol");
   LangTable[11].LangAlphabet = 'G';
   LangTable[11].LangPrincipal[0] = '\0';
   LangTable[11].LangSecondary[0] = '\0';

   strcpy (LangTable[12].LangNom, "Greek");
   LangTable[12].LangAlphabet = 'G';
   LangTable[12].LangPrincipal[0] = '\0';
   LangTable[12].LangSecondary[0] = '\0';

   FreeEntry = 13;

   InitTypoLanguage ();
}




/*----------------------------------------------------------------------
   TtaNewLanguage

   Not available for TYPO languages.

   Declares a new language, its alphabet and optionally the names of the
   principal ans secondary dictionaries. All languages used in a Thot
   document must be explicitely declared, except for predefined languages.
   Thot registers all declared languages and allocates a different
   identifier to each of them.
   This function does not load dictionaries but loads their name.
   If principalDictionary and/or secondDictionary are not NULL values,
   dictionaries are loaded when necessary from files
   $DICOPAR/principalDictionary and/or $DICOPAR/secondDictionary.
   If an application redeclares an existing language, this new declaration
   has no effect.

   Parameters:
   languageName: name of the language.
   languageAlphabet: alphabet to be used for writing that language:
   `L' for ISO-Latin-1, `G' for Symbol (Greek).
   principalDictionary: name of the principal dictionary or NULL.
   secondDictionary: name of the secondary dictionary or NULL.

   Return value:
   identifier of the new language or 0 if the language cannot be added.

  ----------------------------------------------------------------------*/

#ifdef __STDC__
Language            TtaNewLanguage (char *languageName, char languageAlphabet, char *principalDictionary, char *secondDictionary)

#else  /* __STDC__ */
Language            TtaNewLanguage (languageName, languageAlphabet, principalDictionary, secondDictionary)
char               *languageName;
char                languageAlphabet;
char               *principalDictionary;
char               *secondDictionary;

#endif /* __STDC__ */

{
   int                 i;

   i = 0;
   /* Avoids error cases */
   if (languageName == NULL
       || (languageAlphabet != 'L' && languageAlphabet != 'G'))
      TtaError (ERR_invalid_parameter);
   else if (languageName[0] == '\0')
      TtaError (ERR_invalid_parameter);
   else if (strlen (languageName) >= MAX_NAME_LENGTH)
      TtaError (ERR_string_too_long);
   else if (FreeEntry == MAX_LANGUAGES)
      TtaError (ERR_too_many_languages);
   else
     {
	/* Consults the languages table to see if the language exists. */
	for (i = 0; i < FreeEntry; i++)
	   if (!strcmp (languageName, LangTable[i].LangNom))
	      /* The language is already defined */
	      return i;

	/* Saves the new language */
	i = FreeEntry;
	strcpy (LangTable[i].LangNom, languageName);
	LangTable[i].LangAlphabet = languageAlphabet;
	if (principalDictionary != NULL)
	  {
	     strncpy (LangTable[i].LangPrincipal, principalDictionary, MAX_NAME_LENGTH);
	     LangTable[i].LangPrincipal[MAX_NAME_LENGTH - 1] = '\0';
	  }
	else
	   LangTable[i].LangPrincipal[0] = '\0';
	if (secondDictionary != NULL)
	  {
	     strncpy (LangTable[i].LangSecondary, secondDictionary, MAX_NAME_LENGTH);
	     LangTable[i].LangSecondary[MAX_NAME_LENGTH - 1] = '\0';
	  }
	else
	   LangTable[i].LangSecondary[0] = '\0';
	FreeEntry++;
     }
   return i;
}				/*TtaNewLanguage */


/*----------------------------------------------------------------------
   TtaGetLanguageIdFromName

   Available for TYPO languages.

   Returns the identifier of a language that matches a language name.

   Parameters:
   languageName: name of the language.

   Return value:
   identifier of that language or 0 if the language is unknown.

  ----------------------------------------------------------------------*/

#ifdef __STDC__
Language            TtaGetLanguageIdFromName (char *languageName)

#else  /* __STDC__ */
Language            TtaGetLanguageIdFromName (languageName)
char               *languageName;

#endif /* __STDC__ */

{
   int                 i;
   boolean             again;

   i = 0;
   /* Avoids error cases */
   if (languageName == NULL)
      TtaError (ERR_invalid_parameter);
   else if (languageName[0] == '\0')
      TtaError (ERR_invalid_parameter);
   else if (strlen (languageName) >= MAX_NAME_LENGTH)
      TtaError (ERR_string_too_long);
   else
     {
	/* Consults the languages table to see if the language exists. */
	again = TRUE;
	while (again && i < FreeEntry)
	  {
	     if (!strcmp (languageName, LangTable[i].LangNom))
	        /* The language is already defined */
		again = FALSE;
	     else
		i++;
	  }
	if (again)
	  {
	     /* The language dose not exist, saerch intoTYPO languages */
	     i = 0;
	     while (again && i < FreeTypoEntry)
	       {
		  if (!strcmp (languageName, TypoLangTable[i].LangNom))
		     /* The TYPO language is already defined */
		     again = FALSE;
		  else
		     i++;
	       }
	  }			/* end of if (again) */

	if (again)
	  {
	     /* The language does not exist */
	     i = 0;
	     TtaError (ERR_language_not_found);
	  }
     }
   /* returned value */
   return (Language) i;
}				/*TtaGetLanguageIdFromName */

/*----------------------------------------------------------------------
   TtaGetVarLANG

   Returns the 2 first chars of environment variable LANG or 'fr'.

   Return value:
   a string of 2 chars.

  ----------------------------------------------------------------------*/
char               *TtaGetVarLANG ()
{
   char               *name;

   name = TtaGetEnvString ("LANG");
   if (name == NULL)
      strcpy (StandardLANG, "fr");
   else if (!strcmp (name, "C") || !strcmp (name, "iso_8859_1"))
      strcpy (StandardLANG, "fr");
   else
     {
	strncpy (StandardLANG, name, 2);
	StandardLANG[2] = '\0';
     }
   return (StandardLANG);
}

/*----------------------------------------------------------------------
   TtaGetDefaultLanguage

   Returns the identifier of the default language.

   Return value:
   identifier of the default language.

  ----------------------------------------------------------------------*/
Language            TtaGetDefaultLanguage ()
{
   char               *name;
   char                language[MAX_NAME_LENGTH];

   name = TtaGetVarLANG ();
   if (!strncmp (name, "fr", 2))
      strcpy (language, "Fran\347ais");
   else if (!strncmp (name, "en", 2))
      strcpy (language, "English");
   else if (!strncmp (name, "es", 2))
      strcpy (language, "Espa\361ol");
   else if (!strncmp (name, "it", 2))
      strcpy (language, "Italiano");
   else if (!strncmp (name, "de", 2))
      strcpy (language, "Deutsch");
   else if (!strncmp (name, "sv", 2))
      strcpy (language, "Swedish");
   else if (!strncmp (name, "pt", 2))
      strcpy (language, "Portuguese");
   else
      strcpy (language, "English");
   return TtaGetLanguageIdFromName (language);
}

/*----------------------------------------------------------------------
   TtaGetLanguageIdFromAlphabet

   Not available for TYPO languages.

   Returns the identifier of the first language that uses a given alphabet.

   Parameters:
   languageAlphabet: the alphabet of interest (`L' = latin, `G' = greek).

   Return value:
   identifier of that language or 0 if the language is unknown.

  ----------------------------------------------------------------------*/

#ifdef __STDC__
Language            TtaGetLanguageIdFromAlphabet (char languageAlphabet)

#else  /* __STDC__ */
Language            TtaGetLanguageIdFromAlphabet (languageAlphabet)
char                languageAlphabet;

#endif /* __STDC__ */

{
   int                 i;
   boolean             again;

   i = 0;
   /* Avoids error cases */
   if (languageAlphabet != 'L' && languageAlphabet != 'G')
      TtaError (ERR_invalid_parameter);
   else
     {
	/* Consults the languages table to see if the language exists. */
	again = TRUE;
	while (again && i < FreeEntry)
	  {
	     if (languageAlphabet == LangTable[i].LangAlphabet)
		/* The language is already defined */
		again = FALSE;
	     else
		i++;
	  }

	if (again)
	  {
	     /* Tha language does not exist */
	     i = 0;
	     TtaError (ERR_language_not_found);
	  }
     }
   return (Language) i;
}				/*TtaGetLanguageIdFromAlphabet */


/*----------------------------------------------------------------------
   TtaGetAlphabet

   Not available for TYPO languages.

   Returns the alphabet of a language.

   Parameters:
   languageId: name of the language.

   Return value:
   a character that identifies the alphabet ('L' = latin, 'G' = greek).

  ----------------------------------------------------------------------*/

#ifdef __STDC__
char                TtaGetAlphabet (Language languageId)

#else  /* __STDC__ */
char                TtaGetAlphabet (languageId)
Language            languageId;

#endif /* __STDC__ */

{
   int                 i;

   i = (int) languageId;
   /* Verification of the parameter */
   if (i >= FreeEntry)
     {
	TtaError (ERR_language_not_found);
	return ('\0');
     }
   return LangTable[i].LangAlphabet;
}				/*TtaGetAlphabet */


/*----------------------------------------------------------------------
   TtaGetLanguageName

   Not available for TYPO languages.

   Returns the name of a given language.

   Parameters:
   languageId: identifier of the language.

   Return value:
   the name of the language.

  ----------------------------------------------------------------------*/

#ifdef __STDC__
char               *TtaGetLanguageName (Language languageId)

#else  /* __STDC__ */
char               *TtaGetLanguageName (languageId)
Language            languageId;

#endif /* __STDC__ */

{
   int                 i;

   i = (int) languageId;
   if (i >= FreeEntry)
     {
	TtaError (ERR_language_not_found);
	Langbuffer[0] = '\0';
     }
   else
      strcpy (Langbuffer, LangTable[i].LangNom);
   return Langbuffer;
}				/*TtaGetLanguageName */


/*----------------------------------------------------------------------
   TtaGetNumberOfLanguages

   Not available for TYPO languages.

   Returns the current number of languages known by Thot.

   Return value:
   the current number of languages.

  ----------------------------------------------------------------------*/

#ifdef __STDC__
int                 TtaGetNumberOfLanguages ()

#else  /* __STDC__ */
int                 TtaGetNumberOfLanguages ()
#endif				/* __STDC__ */

{
   return FreeEntry;
}


/* 
 * GetPatternList: Read the pattern file of the language and creates the appropriate structure.
 * return 0 if problem else returns 1 
 */
#ifdef __STDC__
boolean             GetPatternList (Language langageId)
#else  /* __STDC__ */
boolean             GetPatternList (langageId)
Language            langageId;

#endif /* __STDC__ */
{
   int                 lang;
   int                 currentIndex;
   int                 previousLength;
   int                 i, lg;
   unsigned char       patternGot[MAX_LET_PATTERN];
   char                weightGot[MAX_LET_PATTERN + 1];
   char                patternFileName[MAX_CHAR];
   static char        *dictPath;	/* Environment variable DICOPAR */
   char               *ptPattern;
   FILE               *in;

   dictPath = TtaGetEnvString ("DICOPAR");
   if (dictPath == NULL)
     {
	/* The environment variable DICOPAR does not exist */
	TtaDisplayMessage (INFO, TtaGetMessage(LIB, MISSING_DICOPAR), "DICOPAR");
	return (FALSE);
     }

   strcpy (patternFileName, dictPath);
   strcat (patternFileName, DIR_STR);
   lang = (int) langageId;
   ptPattern = LangTable[lang].LangPattern;
   strcat (patternFileName, ptPattern);
   if ((in = fopen (patternFileName, "r")) == NULL)
     {
	TtaDisplayMessage (INFO, TtaGetMessage(LIB, HYPHEN_FILE_NOT_OPEN), LangTable[lang].LangPattern);
	return (FALSE);
     }
   TtaDisplayMessage (INFO, TtaGetMessage(LIB, HYPHEN_FILE_OPEN), ptPattern);
   currentIndex = 0;
   previousLength = 0;
   i = 0;

   while ((fscanf (in, "%s %s", (char *) patternGot, weightGot)) != EOF)
     {
	i++;
	lg = strlen (patternGot);
	if (lg != previousLength)
	  {
	     previousLength = lg;
	     currentIndex++;
	     LangTable[lang].LangTabPattern.ind_pattern[previousLength] = i;
	  }
	strcpy (LangTable[lang].LangTabPattern.liste_pattern[i].CarPattern, patternGot);
	strcpy (LangTable[lang].LangTabPattern.liste_pattern[i].PoidsPattern, weightGot);
     }
   LangTable[lang].LangTabPattern.NbPatt = i;
   LangTable[lang].LangTabPattern.Charge = 1;
   fclose (in);
   return (TRUE);
}				/* GetPatternList */


/* 
 * FoundPatternInList regarde: verifies if a string belongs to the pattern list.
 * if true, it returns 1 else 0 
 */
#ifdef __STDC
char               *FoundPatternInList (Language langageId, unsigned char substring[MAX_LET_PATTERN])
#else  /* __STDC__ */
char               *FoundPatternInList (langageId, substring)
Language            langageId;
unsigned char       substring[MAX_LET_PATTERN];

#endif /* __STDC__ */
{
   int                 language;
   int                 lgstring;
   int                 i;
   struct PatternList *ptrTabPattern;

   language = (int) langageId;
   lgstring = strlen (substring);
   ptrTabPattern = &LangTable[language].LangTabPattern;
   i = ptrTabPattern->ind_pattern[lgstring];
   if (i == 0)
      return (0);

   while ((strlen (ptrTabPattern->liste_pattern[i].CarPattern) <= lgstring)
	  && (i <= ptrTabPattern->NbPatt))
     {
	if (!strcmp (ptrTabPattern->liste_pattern[i].CarPattern, substring))
	   return (ptrTabPattern->liste_pattern[i].PoidsPattern);
	i++;
     }
   return (0);
}


  /* 
   * FoundHyphenPoints: apply Liang algo. on a word and returns the 
   * hypen points.
   */
#ifdef __STDC__
void                FoundHyphenPoints (Language langageId, char wordToCut[MAX_CHAR])
#else  /* __STDC__ */
void                FoundHyphenPoints (langageId, wordToCut)
Language            langageId;
char                wordToCut[MAX_CHAR];

#endif /* __STDC__ */
{
   int                 lang;
   unsigned char       wordToTreat[MAX_CHAR];	/* "." + wordToCut + "." */
   unsigned char       subword[MAX_CHAR];
   int                 tab_weight[MAX_CHAR];
   char               *weight_subword;
   int                 wordLength;
   int                 size_subword;
   int                 currentPosition;
   int                 i, j;

   lang = (int) langageId;
   wordLength = strlen (wordToCut) + 2;
   if (wordLength > MAX_CHAR)
     {
	TtaDisplayMessage (INFO, TtaGetMessage(LIB, HYPHEN_WORD_TOO_LONG), wordToCut);
	return;
     }
   for (i = 0; i < MAX_CHAR; i++)
      tab_weight[i] = 0;
   strcpy (wordToTreat, ".");
   strcat (wordToTreat, wordToCut);
   strcat (wordToTreat, ".");
   size_subword = 1;
   while ((size_subword <= wordLength) && (size_subword <= MAX_LET_PATTERN))
     {
	currentPosition = 0;
	while ((currentPosition + size_subword) <= wordLength)
	  {
	     j = 0;
	     for (i = currentPosition; j < size_subword; i++)
		subword[j++] = wordToTreat[i];
	     subword[j] = 0;
	     if ((weight_subword = FoundPatternInList (lang, subword)))
	       {
		  for (j = 0; j <= size_subword; j++)
		     if (weight_subword[j] > tab_weight[currentPosition + j])
			tab_weight[currentPosition + j] = weight_subword[j];
	       }
	     currentPosition++;
	  }
	size_subword++;
     }
   j = 0;
   for (i = 3; i < (wordLength - 2); i++)
      if (ISHYPHENABLE (tab_weight[i]))
	 breakPoints[j++] = i - 1;
   breakPoints[j] = 0;
}


/* TtaGetPatternHyphenList 
   returns a pointer on the list of values representing the hyphen points
   or NULL 
 */
#ifdef __STDC__
int                *TtaGetPatternHyphenList (char word[MAX_CHAR], Language languageId)
#else  /* __STDC__ */
int                *TtaGetPatternHyphenList (word, languageId)
char                word[MAX_CHAR];
Language            languageId;

#endif /* __STDC__ */
{
   int                 language;
   int                 i;

   language = (int) languageId;
   if (word[0] == '\0')
      return (NULL);
   if (strlen (word) < 2)
      return (NULL);
   if (LangTable[language].LangPattern[0] == '\0')
      /* Language without a pattern */
      return (NULL);

   if (!LangTable[language].LangTabPattern.Charge)
      /* patterns not loaded by the language */
      if (!GetPatternList (languageId))
	 return (NULL);

   for (i = 0; i < MAX_POINT_COUP; i++)
      breakPoints[i] = 0;

   FoundHyphenPoints (languageId, word);
   return (breakPoints);

}

/* 
 * TtaExistPatternList verifies if a list of patterns is defined
 * for a given language
 */
#ifdef __STDC__
boolean             TtaExistPatternList (Language languageId)
#else  /* __STDC__ */
boolean             TtaExistPatternList (languageId)
Language            languageId;

#endif /* __STDC__ */
{
   int                 language;

   language = (int) languageId;
   if (LangTable[language].LangPattern[0] != '\0')
      return TRUE;
   else
      return FALSE;
}
