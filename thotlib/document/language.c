
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*=======================================================================*/
/*|                                                                     | */
/*|     Thot Toolkit: Application Program Interface                     | */
/*|     --->Language managment                                          | */
/*|                                                                     | */
/*|                     I. Vatton       January 93                      | */
/*|                                                                     | */
/*=======================================================================*/

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
static int          Points_de_coupure[MAX_POINT_COUP];
static char         StandardLANG[3];

#include "thotmsg_f.h"

/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
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
	for (j = 0; j < MAX_DICOS; j++)
	   TypoLangTable[i].LangDico[j] = NULL;
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

}				/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                InitLanguage ()

#else  /* __STDC__ */
void                InitLanguage ()
#endif				/* __STDC__ */

{
   int                 i, j;

   /* Initialisation du reste de la table */
   for (i = 0; i < MAX_LANGUAGES; i++)
     {
	LangTable[i].LangNom[0] = '\0';
	LangTable[i].LangAlphabet = 'L';
	for (j = 0; j < MAX_DICOS; j++)
	   LangTable[i].LangDico[j] = NULL;
	LangTable[i].LangPattern[0] = '\0';
	LangTable[i].LangTabPattern.Charge = 0;
     }
   /* Chargement des langues par defaut */
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




/* ----------------------------------------------------------------------
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

   ---------------------------------------------------------------------- */

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
   /* Elimine les cas d'erreur */
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
	/* Consulte la table des langues pour voir si la langue existe deja */
	for (i = 0; i < FreeEntry; i++)
	   if (!strcmp (languageName, LangTable[i].LangNom))
	      /* La langue est deja declaree */
	      return i;

	/* Enregistre la nouvelle langue */
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


/* ----------------------------------------------------------------------
   TtaGetLanguageIdFromName

   Available for TYPO languages.

   Returns the identifier of a language that matches a language name.

   Parameters:
   languageName: name of the language.

   Return value:
   identifier of that language or 0 if the language is unknown.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
Language            TtaGetLanguageIdFromName (char *languageName)

#else  /* __STDC__ */
Language            TtaGetLanguageIdFromName (languageName)
char               *languageName;

#endif /* __STDC__ */

{
   int                 i;
   boolean             encore;

   i = 0;
   /* Elimine les cas d'erreur */
   if (languageName == NULL)
      TtaError (ERR_invalid_parameter);
   else if (languageName[0] == '\0')
      TtaError (ERR_invalid_parameter);
   else if (strlen (languageName) >= MAX_NAME_LENGTH)
      TtaError (ERR_string_too_long);
   else
     {
	/* Consulte la table des langues pour voir si la langue existe deja */
	encore = TRUE;
	while (encore && i < FreeEntry)
	  {
	     if (!strcmp (languageName, LangTable[i].LangNom))
		/* La langue est deja declaree */
		encore = FALSE;
	     else
		i++;
	  }
	if (encore)
	  {
	     /* On n'a pas trouve la langue, chercher dans les TYPO langues */
	     i = 0;
	     while (encore && i < FreeTypoEntry)
	       {
		  if (!strcmp (languageName, TypoLangTable[i].LangNom))
		     /* La langue TYPO est deja declaree */
		     encore = FALSE;
		  else
		     i++;
	       }
	  }			/* end of if (encore) */

	if (encore)
	  {
	     /* On n'a pas trouve la langue */
	     i = 0;
	     TtaError (ERR_language_not_found);
	  }
     }
   /* Valeur retournee */
   return (Language) i;
}				/*TtaGetLanguageIdFromName */

/* ----------------------------------------------------------------------
   TtaGetVarLANG

   Returns the 2 first chars of environment variable LANG or 'fr'.

   Return value:
   a string of 2 chars.

   ---------------------------------------------------------------------- */
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

/* ----------------------------------------------------------------------
   TtaGetDefaultLanguage

   Returns the identifier of the default language.

   Return value:
   identifier of the default language.

   ---------------------------------------------------------------------- */
Language            TtaGetDefaultLanguage ()
{
   char               *name;
   char                langue[MAX_NAME_LENGTH];

   name = TtaGetVarLANG ();
   if (!strncmp (name, "fr", 2))
      strcpy (langue, "Fran\347ais");
   else if (!strncmp (name, "en", 2))
      strcpy (langue, "English");
   else if (!strncmp (name, "es", 2))
      strcpy (langue, "Espa\361ol");
   else if (!strncmp (name, "it", 2))
      strcpy (langue, "Italiano");
   else if (!strncmp (name, "de", 2))
      strcpy (langue, "Deutsch");
   else if (!strncmp (name, "sv", 2))
      strcpy (langue, "Swedish");
   else if (!strncmp (name, "pt", 2))
      strcpy (langue, "Portuguese");
   else
      strcpy (langue, "English");
   return TtaGetLanguageIdFromName (langue);
}

/* ----------------------------------------------------------------------
   TtaGetLanguageIdFromAlphabet

   Not available for TYPO languages.

   Returns the identifier of the first language that uses a given alphabet.

   Parameters:
   languageAlphabet: the alphabet of interest (`L' = latin, `G' = greek).

   Return value:
   identifier of that language or 0 if the language is unknown.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
Language            TtaGetLanguageIdFromAlphabet (char languageAlphabet)

#else  /* __STDC__ */
Language            TtaGetLanguageIdFromAlphabet (languageAlphabet)
char                languageAlphabet;

#endif /* __STDC__ */

{
   int                 i;
   boolean             encore;

   i = 0;
   /* Elimine les cas d'erreur */
   if (languageAlphabet != 'L' && languageAlphabet != 'G')
      TtaError (ERR_invalid_parameter);
   else
     {
	/* Consulte la table des langues pour voir si la langue existe deja */
	encore = TRUE;
	while (encore && i < FreeEntry)
	  {
	     if (languageAlphabet == LangTable[i].LangAlphabet)
		/* La langue est deja declaree */
		encore = FALSE;
	     else
		i++;
	  }

	if (encore)
	  {
	     /* On n'a pas trouve la langue */
	     i = 0;
	     TtaError (ERR_language_not_found);
	  }
     }
   return (Language) i;
}				/*TtaGetLanguageIdFromAlphabet */


/* ----------------------------------------------------------------------
   TtaGetAlphabet

   Not available for TYPO languages.

   Returns the alphabet of a language.

   Parameters:
   languageId: name of the language.

   Return value:
   a character that identifies the alphabet ('L' = latin, 'G' = greek).

   ---------------------------------------------------------------------- */

#ifdef __STDC__
char                TtaGetAlphabet (Language languageId)

#else  /* __STDC__ */
char                TtaGetAlphabet (languageId)
Language            languageId;

#endif /* __STDC__ */

{
   int                 i;

   i = (int) languageId;
   /* Verification du parametre */
   if (i >= FreeEntry)
     {
	TtaError (ERR_language_not_found);
	return ('\0');
     }
   return LangTable[i].LangAlphabet;
}				/*TtaGetAlphabet */


/* ----------------------------------------------------------------------
   TtaGetLanguageName

   Not available for TYPO languages.

   Returns the name of a given language.

   Parameters:
   languageId: identifier of the language.

   Return value:
   the name of the language.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
char               *TtaGetLanguageName (Language languageId)

#else  /* __STDC__ */
char               *TtaGetLanguageName (languageId)
Language            languageId;

#endif /* __STDC__ */

{
   int                 i;

   i = (int) languageId;
   /* Verification du parametre */
   if (i >= FreeEntry)
     {
	TtaError (ERR_language_not_found);
	Langbuffer[0] = '\0';
     }
   else
      strcpy (Langbuffer, LangTable[i].LangNom);
   return Langbuffer;
}				/*TtaGetLanguageName */


/* ----------------------------------------------------------------------
   TtaGetNumberOfLanguages

   Not available for TYPO languages.

   Returns the current number of languages known by Thot.

   Return value:
   the current number of languages.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
int                 TtaGetNumberOfLanguages ()

#else  /* __STDC__ */
int                 TtaGetNumberOfLanguages ()
#endif				/* __STDC__ */

{
   return FreeEntry;
}


/* GetPatternList lit le fichier de pattern de la langue et
   creer la stracture appropriee.
   rend 0 si probleme, 1 sinon */
#ifdef __STDC__
boolean             GetPatternList (Language langageId)
#else  /* __STDC__ */
boolean             GetPatternList (langageId)
Language            langageId;

#endif /* __STDC__ */
{
   int                 lang;
   int                 indice_courant;
   int                 taille_prec;
   int                 i, lg;
   unsigned char       pattern_lu[MAX_LET_PATTERN];
   char                poids_lu[MAX_LET_PATTERN + 1];
   char                nom_fichier_pattern[MAX_CHAR];
   static char        *dicopath;	/* variable d'environnement DICOPAR */
   char               *ptPattern;
   FILE               *in;

   dicopath = TtaGetEnvString ("DICOPAR");
   if (dicopath == NULL)
     {
	/* la variable d'environnement DICOPAR n'existe pas */
	TtaDisplayMessage (INFO, TtaGetMessage(LIB, LIB_NO_DICOPAR), "DICOPAR");
	return (FALSE);
     }

   strcpy (nom_fichier_pattern, dicopath);
   strcat (nom_fichier_pattern, DIR_STR);
   lang = (int) langageId;
   ptPattern = LangTable[lang].LangPattern;
   strcat (nom_fichier_pattern, ptPattern);
   if ((in = fopen (nom_fichier_pattern, "r")) == NULL)
     {
	TtaDisplayMessage (INFO, TtaGetMessage(LIB, LIB_HYPHEN_FILE_NOT_OPEN), LangTable[lang].LangPattern);
	return (FALSE);
     }
   TtaDisplayMessage (INFO, TtaGetMessage(LIB, LIB_HYPHEN_FILE_OPEN), ptPattern);
   indice_courant = 0;
   taille_prec = 0;
   i = 0;

   while ((fscanf (in, "%s %s", (char *) pattern_lu, poids_lu)) != EOF)
     {
	i++;
	lg = strlen (pattern_lu);
	if (lg != taille_prec)
	  {
	     taille_prec = lg;
	     indice_courant++;
	     LangTable[lang].LangTabPattern.ind_pattern[taille_prec] = i;
	  }
	strcpy (LangTable[lang].LangTabPattern.liste_pattern[i].CarPattern, pattern_lu);
	strcpy (LangTable[lang].LangTabPattern.liste_pattern[i].PoidsPattern, poids_lu);
     }
   LangTable[lang].LangTabPattern.NbPatt = i;
   LangTable[lang].LangTabPattern.Charge = 1;
   fclose (in);
   return (TRUE);
}				/* GetPatternList */


/* FoundPatternInList regarde si un chaine de caractere
   est dans la liste de pattern. Si oui rends 1 sinon 0 */
#ifdef __STDC
char               *FoundPatternInList (Language langageId, unsigned char substring[MAX_LET_PATTERN])
#else  /* __STDC__ */
char               *FoundPatternInList (langageId, substring)
Language            langageId;
unsigned char       substring[MAX_LET_PATTERN];

#endif /* __STDC__ */
{
   int                 langue;
   int                 lgstring;
   int                 i;
   struct PatternList *ptrTabPattern;

   langue = (int) langageId;
   lgstring = strlen (substring);
   ptrTabPattern = &LangTable[langue].LangTabPattern;
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


  /* FoundHyphenPoints applique l'algo de Liang sur un mot et rend
     la liste des points de cesures
   */
#ifdef __STDC__
void                FoundHyphenPoints (Language langageId, char mot_a_couper[MAX_CHAR])
#else  /* __STDC__ */
void                FoundHyphenPoints (langageId, mot_a_couper)
Language            langageId;
char                mot_a_couper[MAX_CHAR];

#endif /* __STDC__ */
{
   int                 lang;
   unsigned char       mot_a_traiter[MAX_CHAR];	/* "." + mot_a_couper + "." */
   unsigned char       subword[MAX_CHAR];
   int                 tab_weight[MAX_CHAR];
   char               *poids_subword;
   int                 lgmot;
   int                 size_subword;
   int                 pos_courante;
   int                 i, j;

   lang = (int) langageId;
   lgmot = strlen (mot_a_couper) + 2;
   if (lgmot > MAX_CHAR)
     {
	TtaDisplayMessage (INFO, TtaGetMessage(LIB, LIB_HYPHEN_WORD_TOO_LONG), mot_a_couper);
	return;
     }
   for (i = 0; i < MAX_CHAR; i++)
      tab_weight[i] = 0;
   strcpy (mot_a_traiter, ".");
   strcat (mot_a_traiter, mot_a_couper);
   strcat (mot_a_traiter, ".");
   size_subword = 1;
   while ((size_subword <= lgmot) && (size_subword <= MAX_LET_PATTERN))
     {
	pos_courante = 0;
	while ((pos_courante + size_subword) <= lgmot)
	  {
	     j = 0;
	     for (i = pos_courante; j < size_subword; i++)
		subword[j++] = mot_a_traiter[i];
	     subword[j] = 0;
	     if ((poids_subword = FoundPatternInList (lang, subword)))
	       {
		  for (j = 0; j <= size_subword; j++)
		     if (poids_subword[j] > tab_weight[pos_courante + j])
			tab_weight[pos_courante + j] = poids_subword[j];
	       }
	     pos_courante++;
	  }
	size_subword++;
     }
   j = 0;
   for (i = 3; i < (lgmot - 2); i++)
      if (ISHYPHENABLE (tab_weight[i]))
	 Points_de_coupure[j++] = i - 1;
   Points_de_coupure[j] = 0;
}


/* TtaGetPatternHyphenList --------------------------------------------
   retourne un pointeur sur une liste de valeurs representant les
   points de cesure possibles ou NULL s'il n'y en a pas              */
#ifdef __STDC__
int                *TtaGetPatternHyphenList (char word[MAX_CHAR], Language languageId)
#else  /* __STDC__ */
int                *TtaGetPatternHyphenList (word, languageId)
char                word[MAX_CHAR];
Language            languageId;

#endif /* __STDC__ */
{
   int                 langue;
   int                 i;

   langue = (int) languageId;
   if (word[0] == '\0')
      return (NULL);
   if (strlen (word) < 2)
      return (NULL);
   if (LangTable[langue].LangPattern[0] == '\0')
      /* Langue sans pattern */
      return (NULL);

   if (!LangTable[langue].LangTabPattern.Charge)
      /* patterns non charges pour la langue */
      if (!GetPatternList (languageId))
	 return (NULL);

   for (i = 0; i < MAX_POINT_COUP; i++)
      Points_de_coupure[i] = 0;

   FoundHyphenPoints (languageId, word);
   return (Points_de_coupure);

}

/* TtaExistPatternList regarde si une liste de patterns est
   definie pour une langue donnee.                         */
#ifdef __STDC__
boolean             TtaExistPatternList (Language languageId)
#else  /* __STDC__ */
boolean             TtaExistPatternList (languageId)
Language            languageId;

#endif /* __STDC__ */
{
   int                 langue;

   langue = (int) languageId;
   if (LangTable[langue].LangPattern[0] != '\0')
      return TRUE;
   else
      return FALSE;
}

/* end of module */
