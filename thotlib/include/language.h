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
 
#ifndef _LANGUAGE_H_
#define _LANGUAGE_H_

#include "typebase.h"

/* document */
typedef int         Language;
typedef int        *Dictionary;

#ifndef __CEXTRACT__
#ifdef __STDC__


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
   languageName: name of the language according RFC 1766.
   languageAlphabet: alphabet to be used for writing that language:
   `L' for ISO-Latin-1, `G' for Symbol (Greek).
   principalDictionary: name of the principal dictionary or NULL.
   secondDictionary: name of the secondary dictionary or NULL.

   Return value:
   identifier of the new language or 0 if the language cannot be added.
  ----------------------------------------------------------------------*/
extern Language     TtaNewLanguage (CHAR_T* languageName, char languageAlphabet, STRING principalDictionary, STRING secondDictionary);

/*----------------------------------------------------------------------
   TtaRemoveLanguage

   Remove a language from the Thot language table.

   Parameters:
       language: the language to be removed.
  ----------------------------------------------------------------------*/
extern void         TtaRemoveLanguage (Language language);

/*----------------------------------------------------------------------
   TtaGetLanguageIdFromName

   Available for TYPO languages.

   Returns the identifier of a language that matches a language name.

   Parameters:
   languageName: name of the language.

   Return value:
   identifier of that language or 0 if the language is unknown.
  ----------------------------------------------------------------------*/
extern Language     TtaGetLanguageIdFromName (CHAR_T* languageName);

/*----------------------------------------------------------------------
   TtaGetVarLANG

   Returns the 2 first chars of environment variable LANG or 'fr'.

   Return value:
   a string of 2 chars.
  ----------------------------------------------------------------------*/
extern CHAR_T*       TtaGetVarLANG (void);

/*----------------------------------------------------------------------
   TtaGetDefaultLanguage

   Returns the identifier of the default language.

   Return value:
   identifier of the default language.
  ----------------------------------------------------------------------*/
extern Language     TtaGetDefaultLanguage (void);

/*----------------------------------------------------------------------
   TtaGetLanguageIdFromAlphabet

   Not available for TYPO languages.

   Returns the identifier of the first language that uses a given alphabet.

   Parameters:
   languageAlphabet: the alphabet of interest (`L' = latin, `G' = greek).

   Return value:
   identifier of that language or 0 if the language is unknown.
  ----------------------------------------------------------------------*/
extern Language     TtaGetLanguageIdFromAlphabet (CHAR_T languageAlphabet);

/*----------------------------------------------------------------------
   TtaGetAlphabet

   Not available for TYPO languages.

   Returns the alphabet of a language.

   Parameters:
   languageId: name of the language.

   Return value:
   a character that identifies the alphabet ('L' = latin, 'G' = greek).
  ----------------------------------------------------------------------*/
extern char        TtaGetAlphabet (Language languageId);

/*----------------------------------------------------------------------
   TtaGetLanguageName

   Not available for TYPO languages.

   Returns the name of a given language.

   Parameters:
   languageId: identifier of the language.

   Return value:
   the name of the language.
  ----------------------------------------------------------------------*/
extern CHAR_T*       TtaGetLanguageName (Language languageId);

/*----------------------------------------------------------------------
   TtaGetLanguageCode

   Not available for TYPO languages.

   Returns the RFC-1766 code of a given language.

   Parameters:
   languageId: identifier of the language.

   Return value:
   the code of the language.
  ----------------------------------------------------------------------*/
extern CHAR_T*       TtaGetLanguageCode (Language languageId);

/*----------------------------------------------------------------------
   TtaGetNumberOfLanguages

   Not available for TYPO languages.

   Returns the current number of languages known by Thot.

   Return value:
   the current number of languages.
  ----------------------------------------------------------------------*/
extern int          TtaGetNumberOfLanguages (void);

/*----------------------------------------------------------------------
   TtaLoadLanguageDictionaries

   Loads the dictionary associated with a language, if it is not loaded yet
   and registers that a dictionary associated with this language has been loaded.

   Returns -1 if the mandatory dictionary cann't be loaded.
   0 if no dictionary has been loaded
   1 if the mandatory dictionary is loaded.

   Parameters:
   languageId: name of the concerned language.

  ----------------------------------------------------------------------*/
extern ThotBool     TtaLoadLanguageDictionaries (Language languageId);

/*----------------------------------------------------------------------
   TtaUnLoadLanguageDictionaries

   Unloads dictionaries associated with a given language.

   Parameters:
   languageId: identifier of the language.

  ----------------------------------------------------------------------*/
extern void         TtaUnLoadLanguageDictionaries (Language languageId);

/*----------------------------------------------------------------------
   TtaGetPrincipalDictionary

   Returns a pointer to the principal dictionary associated to a language.

   Return value:
   the pointer to that dictionary or NULL if there is no dictionary for
   this language.
  ----------------------------------------------------------------------*/
extern Dictionary   TtaGetPrincipalDictionary (Language languageId);

/*----------------------------------------------------------------------
   TtaGetSecondaryDictionary

   Returns a pointer to the secondary dictionary associated to a language.

   Return value:
   the pointer to that dictionary or NULL if there is no dictionary for
   this language.
  ----------------------------------------------------------------------*/
extern Dictionary   TtaGetSecondaryDictionary (Language languageId);

/*----------------------------------------------------------------------
   TtaGetPrincipalTypoDictionary

   Returns a pointer to the principal dictionary associated to a language.

   Return value:
   the pointer to that dictionary or NULL if there is no dictionary for
   this language.
  ----------------------------------------------------------------------*/
extern Dictionary   TtaGetPrincipalTypoDictionary (Language languageId);

/*----------------------------------------------------------------------
   TtaGetSecondaryTypoDictionary

   Returns a pointer to the secondary dictionary associated to a language.

   Return value:
   the pointer to that dictionary or NULL if there is no dictionary for
   this language.
  ----------------------------------------------------------------------*/
extern Dictionary   TtaGetSecondaryTypoDictionary (Language languageId);

#else  /* __STDC__ */

extern Language     TtaNewLanguage ( /* CHAR_T* languageName, char languageAlphabet, char *principalDictionary, char *secondDictionary */ );
extern void         TtaRemoveLanguage ( /* Language language */ );
extern Language     TtaGetLanguageIdFromName ( /* CHAR_T* languageName */ );
extern CHAR_T*    TtaGetVarLANG ( /* void */ );
extern Language     TtaGetDefaultLanguage ( /* void */ );
extern Language     TtaGetLanguageIdFromAlphabet ( /* char languageAlphabet */ );
extern char         TtaGetAlphabet ( /* Language languageId */ );
extern CHAR_T*      TtaGetLanguageName ( /* Language languageId */ );
extern CHAR_T*      TtaGetLanguageCode ( /* Language languageId */ );
extern int          TtaGetNumberOfLanguages ( /* void */ );
extern ThotBool     TtaLoadLanguageDictionaries ( /* Language languageId */ );
extern void         TtaUnLoadLanguageDictionaries ( /* Language languageId */ );
extern Dictionary   TtaGetPrincipalDictionary ( /* Language languageId */ );
extern Dictionary   TtaGetSecondaryDictionary ( /* Language languageId */ );
extern Dictionary   TtaGetPrincipalTypoDictionary ( /* Language languageId */ );
extern Dictionary   TtaGetSecondaryTypoDictionary ( /* Language languageId */ );

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */

#endif
