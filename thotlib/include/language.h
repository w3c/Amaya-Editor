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
typedef unsigned char Language;
typedef int        *Dictionary;

#ifndef __CEXTRACT__
#ifdef __STDC__

extern Language     TtaNewLanguage (char *languageName, char languageAlphabet, char *principalDictionary, char *secondDictionary);
extern Language     TtaGetLanguageIdFromName (char *languageName);
extern char        *TtaGetVarLANG (void);
extern Language     TtaGetDefaultLanguage (void);
extern Language     TtaGetLanguageIdFromAlphabet (char languageAlphabet);
extern char         TtaGetAlphabet (Language languageId);
extern char        *TtaGetLanguageName (Language languageId);
extern int          TtaGetNumberOfLanguages (void);
extern boolean      TtaLoadLanguageDictionaries (Language languageId);
extern void         TtaUnLoadLanguageDictionaries (Language languageId);
extern Dictionary   TtaGetPrincipalDictionary (Language languageId);
extern Dictionary   TtaGetSecondaryDictionary (Language languageId);
extern Dictionary   TtaGetPrincipalTypoDictionary (Language languageId);
extern Dictionary   TtaGetSecondaryTypoDictionary (Language languageId);

#else  /* __STDC__ */

extern Language     TtaNewLanguage ( /* char *languageName, char languageAlphabet, char *principalDictionary, char *secondDictionary */ );
extern Language     TtaGetLanguageIdFromName ( /* char *languageName */ );
extern char        *TtaGetVarLANG ( /* void */ );
extern Language     TtaGetDefaultLanguage ( /* void */ );
extern Language     TtaGetLanguageIdFromAlphabet ( /* char languageAlphabet */ );
extern char         TtaGetAlphabet ( /* Language languageId */ );
extern char        *TtaGetLanguageName ( /* Language languageId */ );
extern int          TtaGetNumberOfLanguages ( /* void */ );
extern boolean      TtaLoadLanguageDictionaries ( /* Language languageId */ );
extern void         TtaUnLoadLanguageDictionaries ( /* Language languageId */ );
extern Dictionary   TtaGetPrincipalDictionary ( /* Language languageId */ );
extern Dictionary   TtaGetSecondaryDictionary ( /* Language languageId */ );
extern Dictionary   TtaGetPrincipalTypoDictionary ( /* Language languageId */ );
extern Dictionary   TtaGetSecondaryTypoDictionary ( /* Language languageId */ );

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */

#endif
