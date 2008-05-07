/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

 
#ifndef _LANGUAGE_H_
#define _LANGUAGE_H_

#include "typebase.h"


#define Latin_Script    0
#define Greek_Script    1
#define Arabic_Script   2
#define Hebrew_Script   3
#define Cyrillic_Script 4
#define CJK_Script      5
#define FirstUserLang   6

/* document */
typedef int         Language;
typedef int        *Dictionary;

#ifndef __CEXTRACT__


/*----------------------------------------------------------------------
   TtaNewLanguage

   Declares a new language, its script and optionally the names of the
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
   languageScript: script to be used for writing that language:
   `L' for ISO-Latin-1, `G' for Symbol (Greek).
   principalDictionary: name of the principal dictionary or NULL.
   secondDictionary: name of the secondary dictionary or NULL.
   Return value:
   identifier of the new language or 0 if the language cannot be added.
  ----------------------------------------------------------------------*/
extern Language TtaNewLanguage (char *languageName, char languageScript,
				char *principalDictionary, char *secondDictionary);

/*----------------------------------------------------------------------
   TtaRemoveLanguage

   Remove a language from the Thot language table.
   language: the language to be removed.
  ----------------------------------------------------------------------*/
extern void TtaRemoveLanguage (Language language);

/*----------------------------------------------------------------------
   TtaGetLanguageIdFromName

   Available for TYPO languages.
   Returns the identifier of a language that matches a language name.
   languageName: name of the language.
   Return value:
   identifier of that language, 0 if the language is not supported, 
   -1 for an unknown language.
  ----------------------------------------------------------------------*/
extern Language TtaGetLanguageIdFromName (const char *languageName);

/*----------------------------------------------------------------------
   TtaGetVarLANG

   Returns the 2 first chars of environment variable LANG or 'fr'.
   Return value:
   a string of 2 chars.
  ----------------------------------------------------------------------*/
extern char *TtaGetVarLANG (void);

/*----------------------------------------------------------------------
   TtaGetISO639Code
   Returns the language code in the ISO639 table
  ----------------------------------------------------------------------*/
extern const char *TtaGetISO639Code (int code);

/*----------------------------------------------------------------------
   TtaGetDefaultLanguage

   Returns the identifier of the default language.
   Return value:
   identifier of the default language.
  ----------------------------------------------------------------------*/
extern Language TtaGetDefaultLanguage (void);

/*----------------------------------------------------------------------
   TtaGetLanguageIdFromScript

   Returns the identifier of the first language that uses a given script.
   Parameters:
   languageScript: the script of interest (`L' = latin, `G' = greek).
   Return value:
   identifier of that language or 0 if the language is unknown.
  ----------------------------------------------------------------------*/
extern Language TtaGetLanguageIdFromScript (char languageScript);


/*----------------------------------------------------------------------
   TtaGetCharacterScript

   Returns the script of the wide character.
   Return value:
   A character that identifies the script
   'L' = Latin, 'G' = Greek 'A' = Arabic, etc.
  ----------------------------------------------------------------------*/
extern char TtaGetCharacterScript (wchar_t c);

/*----------------------------------------------------------------------
   TtaGetScript

   Returns the script of a language.
   languageId: name of the language.
   Return value:
   a character that identifies the script ('L' = latin, 'G' = greek).
  ----------------------------------------------------------------------*/
extern char TtaGetScript (Language languageId);

/*----------------------------------------------------------------------
   TtaGetLanguageName

   Returns the name of a given language.
   languageId: identifier of the language.
   Return value:
   the name of the language.
  ----------------------------------------------------------------------*/
extern char *TtaGetLanguageName (Language languageId);

/*----------------------------------------------------------------------
   TtaGetProposal

   Returns the list of proposals (needed by wx)
  ----------------------------------------------------------------------*/
extern void TtaGetProposal (char **proposal, int i);

/*----------------------------------------------------------------------
   TtaGetChkrLanguageName

  Returns the name of the checker language (needed by wx)
   Return value:
   the name of the language.
  ----------------------------------------------------------------------*/
extern void TtaGetChkrLanguageName (char **lang);

/*----------------------------------------------------------------------
   TtaGetLanguageCode

   Not available for TYPO languages.
   Returns the RFC-1766 code of a given language.
   languageId: identifier of the language.
   Return value:
   the code of the language.
  ----------------------------------------------------------------------*/
extern char *TtaGetLanguageCode (Language languageId);

/*----------------------------------------------------------------------
   TtaGetNumberOfLanguages

   Returns the current number of languages known by Thot.
   Return value:
   the current number of languages.
  ----------------------------------------------------------------------*/
extern int TtaGetNumberOfLanguages (void);

/*----------------------------------------------------------------------
   TtaLoadLanguageDictionaries

   Loads the dictionary associated with a language, if it is not loaded yet
   and registers that a dictionary associated with this language has been loaded.
   Returns -1 if the mandatory dictionary cann't be loaded.
   0 if no dictionary has been loaded
   1 if the mandatory dictionary is loaded.
   languageId: name of the concerned language.
  ----------------------------------------------------------------------*/
extern ThotBool TtaLoadLanguageDictionaries (Language languageId);

/*----------------------------------------------------------------------
   TtaUnLoadLanguageDictionaries

   Unloads dictionaries associated with a given language.
   Parameters:
   languageId: identifier of the language.
  ----------------------------------------------------------------------*/
extern void TtaUnLoadLanguageDictionaries (Language languageId);

/*----------------------------------------------------------------------
   TtaGetPrincipalDictionary

   Returns a pointer to the principal dictionary associated to a language.
   Return value:
   the pointer to that dictionary or NULL if there is no dictionary for
   this language.
  ----------------------------------------------------------------------*/
extern Dictionary TtaGetPrincipalDictionary (Language languageId);

/*----------------------------------------------------------------------
   TtaGetSecondaryDictionary

   Returns a pointer to the secondary dictionary associated to a language.
   Return value:
   the pointer to that dictionary or NULL if there is no dictionary for
   this language.
  ----------------------------------------------------------------------*/
extern Dictionary TtaGetSecondaryDictionary (Language languageId);

#endif /* __CEXTRACT__ */

#endif
