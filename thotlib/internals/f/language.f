
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void InitLanguage ( void );
extern Language TtaNewLanguage ( char *languageName, char languageAlphabet, char *principalDictionary, char *secondDictionary );
extern Language TtaGetLanguageIdFromName ( char *languageName );
extern Language TtaGetDefaultLanguage ( void );
extern Language TtaGetLanguageIdFromAlphabet ( char languageAlphabet );
extern char TtaGetAlphabet ( Language languageId );
extern char *TtaGetLanguageName ( Language languageId );
extern int TtaGetNumberOfLanguages ( void );
extern boolean GetPatternList ( Language langageId );
extern char *FoundPatternInList ( Language langageId, unsigned char substring[13] );
extern void FoundHyphenPoints ( Language langageId, char mot_a_couper[MAX_CHAR] );
extern int *TtaGetPatternHyphenList ( char word[MAX_CHAR], Language languageId );
extern boolean TtaExistPatternList ( Language languageId );

#else /* __STDC__ */

extern void InitLanguage (/* void */);
extern Language TtaNewLanguage (/* char *languageName, char languageAlphabet, char *principalDictionary, char *secondDictionary */);
extern Language TtaGetLanguageIdFromName (/* char *languageName */);
extern Language TtaGetDefaultLanguage (/* void */);
extern Language TtaGetLanguageIdFromAlphabet (/* char languageAlphabet */);
extern char TtaGetAlphabet (/* Language languageId */);
extern char *TtaGetLanguageName (/* Language languageId */);
extern int TtaGetNumberOfLanguages (/* void */);
extern boolean GetPatternList (/* Language langageId */);
extern char *FoundPatternInList (/* Language langageId, unsigned char substring[13] */);
extern void FoundHyphenPoints (/* Language langageId, char mot_a_couper[MAX_CHAR] */);
extern int *TtaGetPatternHyphenList (/* char word[MAX_CHAR], Language languageId */);
extern boolean TtaExistPatternList (/* Language languageId */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
