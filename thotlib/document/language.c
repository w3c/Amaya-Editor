/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*----------------------------------------------------------------------
   
   Application Program Interface                     
   Language management                                          
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
int                 FirstUserLang;
static CHAR_T       Langbuffer[2 * MAX_NAME_LENGTH];
static char         CodeBuffer[2 * MAX_NAME_LENGTH];
static int          breakPoints[MAX_POINT_COUP];
static CHAR_T       StandardLANG[3];

#include "thotmsg_f.h"
#include "ustring_f.h"

/*	ISO 639 CODES ALPHABETIC BY LANGUAGE NAME (ENGLISH SPELLING) */

typedef unsigned char aLangName[MAX_LENGTH];
typedef struct _ISO639entry
  {
     aLangName	fullName;
     aLangName	code;
  }
ISO639entry;

static ISO639entry	ISO639table[] =
{
	{"ABKHAZIAN",      "AB"},
	{"AFAN (OROMO)",   "OM"},
	{"AFAR",           "AA"},
	{"AFRIKAANS",      "AF"},
	{"ALBANIAN",       "SQ"},
	{"AMHARIC",        "AM"},
	{"ARABIC",         "AR"},
	{"ARMENIAN",       "HY"},
	{"ASSAMESE",       "AS"},
	{"AYMARA",         "AY"},
	{"AZERBAIJANI",    "AZ"},
	{"BASHKIR",        "BA"},
	{"BASQUE",         "EU"},
	{"BENGALI",        "BN"},
	{"BANGLA",         "BN"},
	{"BHUTANI",        "DZ"},
	{"BIHARI",         "BH"},
	{"BISLAMA",        "BI"},
	{"BRETON",         "BR"},
	{"BULGARIAN",      "BG"},
	{"BURMESE",        "MY"},
	{"BYELORUSSIAN",   "BE"},
	{"CAMBODIAN",      "KM"},
	{"CATALAN",        "CA"},
	{"CHINESE",        "ZH"},
	{"CORSICAN",       "CO"},
	{"CROATIAN",       "HR"},
	{"Czech",          "CS"},
	{"DANISH",         "DA"},
	{"Dutch",          "NL"},
	{"English",        "EN"},
	{"ESPERANTO",      "EO"},
	{"ESTONIAN",       "ET"},
	{"FAROESE",        "FO"},
	{"FIJI",           "FJ"},
	{"Finnish",        "FI"},
	{"French",         "FR"},
	{"FRISIAN",        "FY"},
	{"GALICIAN",       "GL"},
	{"GEORGIAN",       "KA"},
	{"German",         "DE"},
	{"Greek",          "EL"},
	{"GREENLANDIC",    "KL"},
	{"GUARANI",        "GN"},
	{"GUJARATI",       "GU"},
	{"HAUSA",          "HA"},
	{"HEBREW",         "HE"},
	{"HINDI",          "HI"},
	{"HUNGARIAN",      "HU"},
	{"ICELANDIC",      "IS"},
	{"INDONESIAN",     "ID"},
	{"INTERLINGUA",    "IA"},
	{"INTERLINGUE",    "IE"},
	{"INUKTITUT",      "IU"},
	{"INUPIAK",        "IK"},
	{"IRISH",          "GA"},
	{"Italian",        "IT"},
	{"JAPANESE",       "JA"},
	{"JAVANESE",       "JV"},
	{"KANNADA",        "KN"},
	{"KASHMIRI",       "KS"},
	{"KAZAKH",         "KK"},
	{"KINYARWANDA",    "RW"},
	{"KIRGHIZ",        "KY"},
	{"KURUNDI",        "RN"},
	{"KOREAN",         "KO"},
	{"KURDISH",        "KU"},
	{"LAOTHIAN",       "LO"},
	{"LATIN",          "LA"},
	{"LATVIAN",        "LV"},
	{"LETTISH",        "LV"},
	{"LINGALA",        "LN"},
	{"LITHUANIAN",     "LT"},
	{"MACEDONIAN",     "MK"},
	{"MALAGASY",       "MG"},
	{"MALAY",          "MS"},
	{"MALAYALAM",      "ML"},
	{"MALTESE",        "MT"},
	{"MAORI",          "MI"},
	{"MARATHI",        "MR"},
	{"MOLDAVIAN",      "MO"},
	{"MONGOLIAN",      "MN"},
	{"NAURU",          "NA"},
	{"NEPALI",         "NE"},
	{"NORWEGIAN",      "NO"},
	{"OCCITAN",        "OC"},
	{"ORIYA",          "OR"},
	{"PASHTO",         "PS"},
	{"PUSHTO",         "PS"},
	{"PERSIAN",        "FA"},
	{"Polish",         "PL"},
	{"Portuguese",     "PT"},
	{"PUNJABI",        "PA"},
	{"QUECHUA",        "QU"},
	{"RHAETO-ROMANCE", "RM"},
	{"ROMANIAN",       "RO"},
	{"RUSSIAN",        "RU"},
	{"SAMOAN",         "SM"},
	{"SANGHO",         "SG"},
	{"SANSKRIT",       "SA"},
	{"SCOTS GAELIC",   "GD"},
	{"SERBIAN",        "SR"},
	{"SERBO-CROATIAN", "SH"},
	{"SESOTHO",        "ST"},
	{"SETSWANA",       "TN"},
	{"SHONA",          "SN"},
	{"SINDHI",         "SD"},
	{"SINGHALESE",     "SI"},
	{"SISWATI",        "SS"},
	{"SLOVAK",         "SK"},
	{"SLOVENIAN",      "SL"},
	{"SOMALI",         "SO"},
	{"Spanish",        "ES"},
	{"SUNDANESE",      "SU"},
	{"SWAHILI",        "SW"},
	{"Swedish",        "SV"},
	{"TAGALOG",        "TL"},
	{"TAJIK",          "TG"},
	{"TAMIL",          "TA"},
	{"TATAR",          "TT"},
	{"TELUGU",         "TE"},
	{"THAI",           "TH"},
	{"TIBETAN",        "BO"},
	{"TIGRINYA",       "TI"},
	{"TONGA",          "TO"},
	{"TSONGA",         "TS"},
	{"TURKISH",        "TR"},
	{"TURKMEN",        "TK"},
	{"TWI",            "TW"},
	{"UIGUR",          "UG"},
	{"UKRAINIAN",      "UK"},
	{"URDU",           "UR"},
	{"UZBEK",          "UZ"},
	{"VIETNAMESE",     "VI"},
	{"VOLAPUK",        "VO"},
	{"WELSH",          "CY"},
	{"WOLOF",          "WO"},
	{"XHOSA",          "XH"},
	{"YIDDISH",        "YI"},
	{"YORUBA",         "YO"},
	{"ZHUANG",         "ZA"},
	{"ZULU",           "ZU"},
	{"",               ""}
};

/* this table associates the ancient language names used in Thot documents
   with their standard code */
static ISO639entry	OldLangTable[] =
{
	{"American",    "EN-US"},
	{"Deutsch",     "DE"},
	{"Espa\361ol",  "ES"},
	{"Fran\347ais", "FR"},
	{"ISO_latin_1", "x-Latin1"},
	{"ISO_latin_2", "x-Latin2"},
	{"Italiano",    "IT"},
	{"Symbol",      "x-Symbol"},
	{"",            ""}
};


/*----------------------------------------------------------------------
   TtaGetLanguageNameFromCode
   Returns the full name of a language whose RFC-1766 code is known
  ----------------------------------------------------------------------*/
#ifdef __STDC__
CHAR_T*        TtaGetLanguageNameFromCode (char* code)
#else  /* __STDC__ */
CHAR_T*        TtaGetLanguageNameFromCode (code)
char*          code;
#endif /* __STDC__ */

{
   int                 i;

   Langbuffer[0] = WC_EOS;
   for (i = 0; Langbuffer[0] == EOS; i++)
      if (ISO639table[i].code[0] == EOS)
	    break;
      else if (strcasecmp (code, ISO639table[i].code) == 0)
           iso2wc_strcpy (Langbuffer, ISO639table[i].fullName);
   if (Langbuffer[0] == WC_EOS)
      for (i = 0; Langbuffer[0] == WC_EOS; i++)
         if (OldLangTable[i].code[0] == EOS)
	    break;
         else
            if (strcasecmp (code, OldLangTable[i].code) == 0)
               iso2wc_strcpy (Langbuffer, OldLangTable[i].fullName);
   return Langbuffer;
}


/*----------------------------------------------------------------------
   TtaGetLanguageCodeFromName
   Returns the RFC-1766 code for a language whose name is known
  ----------------------------------------------------------------------*/
#ifdef __STDC__
char*          TtaGetLanguageCodeFromName (CHAR_T* name)
#else  /* __STDC__ */
char*          TtaGetLanguageCodeFromName (name)
CHAR_T*        name;
#endif /* __STDC__ */

{
   int                 i;
   char                langName[MAX_LENGTH];

   wc2iso_strcpy (langName, name);
   CodeBuffer[0] = EOS;
   for (i = 0; CodeBuffer[0] == EOS; i++)
      if (ISO639table[i].fullName[0] == EOS)
	 break;
      else
         if (strcasecmp (langName, ISO639table[i].fullName) == 0)
            strcpy (CodeBuffer, ISO639table[i].code);
   if (CodeBuffer[0] == EOS)
      for (i = 0; CodeBuffer[0] == EOS; i++)
         if (OldLangTable[i].fullName[0] == EOS)
 	    break;
         else
            if (strcasecmp (langName, OldLangTable[i].fullName) == 0)
               strcpy (CodeBuffer, OldLangTable[i].code);
   return CodeBuffer;
}

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
	TypoLangTable[i].LangName[0] = WC_EOS;
	TypoLangTable[i].LangCode[0] = EOS;
	TypoLangTable[i].LangAlphabet = 'L';
	for (j = 0; j < MAX_DICTS; j++)
	   TypoLangTable[i].LangDict[j] = NULL;
	TypoLangTable[i].LangPattern[0] = WC_EOS;
	TypoLangTable[i].LangTabPattern.Charge = 0;
     }
   ustrcpy (TypoLangTable[0].LangName, TEXT("F-Short"));
   TypoLangTable[0].LangAlphabet = 'L';
   ustrcpy (TypoLangTable[0].LangPrincipal, TEXT("Fabbrev"));
   ustrcpy (TypoLangTable[0].LangSecondary, TEXT("Ftrabbrev"));

   ustrcpy (TypoLangTable[1].LangName, TEXT("E-Short"));
   TypoLangTable[1].LangAlphabet = 'L';
   ustrcpy (TypoLangTable[1].LangPrincipal, TEXT("Eabbrev"));
   ustrcpy (TypoLangTable[1].LangSecondary, TEXT("Etrabbrev"));

   ustrcpy (TypoLangTable[2].LangName, TEXT("F-Acronym"));
   TypoLangTable[2].LangAlphabet = 'L';
   ustrcpy (TypoLangTable[2].LangPrincipal, TEXT("Facronym"));
   ustrcpy (TypoLangTable[2].LangSecondary, TEXT("Ftracronym"));

   ustrcpy (TypoLangTable[3].LangName, TEXT("E-Acronym"));
   TypoLangTable[3].LangAlphabet = 'L';
   ustrcpy (TypoLangTable[3].LangPrincipal, TEXT("Eacronym"));
   ustrcpy (TypoLangTable[3].LangSecondary, TEXT("Etracronym"));

   ustrcpy (TypoLangTable[4].LangName, TEXT("F-Expo1"));
   TypoLangTable[4].LangAlphabet = 'L';
   ustrcpy (TypoLangTable[4].LangPrincipal, TEXT("Fexpo"));
   ustrcpy (TypoLangTable[4].LangSecondary, TEXT("Ftrexpo"));

   ustrcpy (TypoLangTable[5].LangName, TEXT("F-Expo2"));
   TypoLangTable[5].LangAlphabet = 'L';
   ustrcpy (TypoLangTable[5].LangPrincipal, TEXT("Fexpolex"));
/*  ustrcpy(TypoLangTable[5].LangSecondary, "Fexpo2");
 */

   FreeTypoEntry = 6;

}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                InitLanguage ()
#else  /* __STDC__ */
void                InitLanguage ()
#endif /* __STDC__ */
{
   int                 i, j;

   /* Initialization of remaining entries of the language table */
   for (i = 0; i < MAX_LANGUAGES; i++)
     {
	LangTable[i].LangName[0] = WC_EOS;
	LangTable[i].LangCode[0] = EOS;
	LangTable[i].LangAlphabet = 'L';
	for (j = 0; j < MAX_DICTS; j++)
	   LangTable[i].LangDict[j] = NULL;
	LangTable[i].LangPattern[0] = WC_EOS;
	LangTable[i].LangTabPattern.Charge = 0;
     }
   /* Loading the default system languages */
   i = 0;
   ustrcpy (LangTable[i].LangName, TEXT("ISO_latin_1"));
   strcpy (LangTable[i].LangCode, "x-Latin1");
   LangTable[i].LangAlphabet = 'L';
   ustrcpy (LangTable[i].LangPrincipal, TEXT("Usigle"));
   ustrcpy (LangTable[i].LangSecondary, TEXT("Uname"));

   i = 1;
   ustrcpy (LangTable[i].LangName, TEXT("ISO_latin_2"));
   strcpy (LangTable[i].LangCode, "x-Latin2");
   LangTable[i].LangAlphabet = '2';
   LangTable[i].LangPrincipal[0] = EOS;
   LangTable[i].LangSecondary[0] = EOS;

   i = 2;
   ustrcpy (LangTable[i].LangName, TEXT("ISO_latin_9"));
   strcpy (LangTable[i].LangCode, "x-Latin9");
   LangTable[i].LangAlphabet = '9';
   LangTable[i].LangPrincipal[0] = EOS;
   LangTable[i].LangSecondary[0] = EOS;

   i = 3;
   ustrcpy (LangTable[i].LangName, TEXT("Symbol"));
   strcpy (LangTable[i].LangCode, "x-Symbol");
   LangTable[i].LangAlphabet = 'G';
   LangTable[i].LangPrincipal[0] = EOS;
   LangTable[i].LangSecondary[0] = EOS;

   /* Loading the default user languages */
   FirstUserLang = 4;
   i = 4;
   ustrcpy (LangTable[i].LangName, TEXT("French"));
   strcpy (LangTable[i].LangCode, "fr");
   LangTable[i].LangAlphabet = 'L';
   ustrcpy (LangTable[i].LangPrincipal, TEXT("Fprinc"));
   ustrcpy (LangTable[i].LangSecondary, TEXT("Fperso"));
   ustrcpy (LangTable[i].LangPattern, TEXT("francais.ptn"));

   i = 5;
   ustrcpy (LangTable[i].LangName, TEXT("English"));
   strcpy (LangTable[i].LangCode, "en");
   LangTable[i].LangAlphabet = 'L';
   ustrcpy (LangTable[i].LangPrincipal, TEXT("Eprinc"));
   ustrcpy (LangTable[i].LangSecondary, TEXT("Eperso"));
   ustrcpy (LangTable[i].LangPattern, TEXT("english.ptn"));

   i = 6;
   ustrcpy (LangTable[i].LangName, TEXT("American"));
   strcpy (LangTable[i].LangCode, "en-US");
   LangTable[i].LangAlphabet = 'L';
   ustrcpy (LangTable[i].LangPrincipal, TEXT("Eprinc"));
   ustrcpy (LangTable[i].LangSecondary, TEXT("Eperso"));
   ustrcpy (LangTable[i].LangPattern, TEXT("american.ptn"));

   i = 7;
   ustrcpy (LangTable[i].LangName, TEXT("German"));
   strcpy (LangTable[i].LangCode, "de");
   LangTable[i].LangAlphabet = 'L';
   ustrcpy (LangTable[i].LangPrincipal, TEXT("Gprinc"));
   LangTable[i].LangSecondary[0] = EOS;
   ustrcpy (LangTable[i].LangPattern, TEXT("deutsch.ptn"));

   i = 8;
   ustrcpy (LangTable[i].LangName, TEXT("Italian"));
   strcpy (LangTable[i].LangCode, "it");
   LangTable[i].LangAlphabet = 'L';
   ustrcpy (LangTable[i].LangPrincipal, TEXT("Iprinc"));
   LangTable[i].LangSecondary[0] = EOS;
   ustrcpy (LangTable[i].LangPattern, TEXT("italiano.ptn"));

   i = 9;
   ustrcpy (LangTable[i].LangName, TEXT("Spanish"));
   strcpy (LangTable[i].LangCode, "es");
   LangTable[i].LangAlphabet = 'L';
   ustrcpy (LangTable[i].LangPrincipal, TEXT("Sprinc"));
   LangTable[i].LangSecondary[0] = EOS;
   ustrcpy (LangTable[i].LangPattern, TEXT("espanol.ptn"));

   i = 10;
   ustrcpy (LangTable[i].LangName, TEXT("Portuguese"));
   strcpy (LangTable[i].LangCode, "pt");
   LangTable[i].LangAlphabet = 'L';
   LangTable[i].LangPrincipal[0] = EOS;
   LangTable[i].LangSecondary[0] = EOS;
   ustrcpy (LangTable[i].LangPattern, TEXT("portug.ptn"));

   i = 11;
   ustrcpy (LangTable[i].LangName, TEXT("Dutch"));
   strcpy (LangTable[i].LangCode, "nl");
   LangTable[i].LangAlphabet = 'L';
   ustrcpy (LangTable[i].LangPrincipal, TEXT("Nprinc"));
   LangTable[i].LangSecondary[0] = EOS;
   ustrcpy (LangTable[i].LangPattern, TEXT("nederl.ptn"));

   i = 12;
   ustrcpy (LangTable[i].LangName, TEXT("Swedish"));
   strcpy (LangTable[i].LangCode, "sv");
   LangTable[i].LangAlphabet = 'L';
   ustrcpy (LangTable[i].LangPrincipal, TEXT("Wprinc"));
   LangTable[i].LangSecondary[0] = EOS;
   ustrcpy (LangTable[i].LangPattern, TEXT("swedish.ptn"));

   i = 13;
   ustrcpy (LangTable[i].LangName, TEXT("Finnish"));
   strcpy (LangTable[i].LangCode, "fi");
   LangTable[i].LangAlphabet = 'L';
   LangTable[i].LangPrincipal[0] = EOS;
   LangTable[i].LangSecondary[0] = EOS;
   ustrcpy (LangTable[i].LangPattern, TEXT("finish.ptn"));

   i = 14;
   ustrcpy (LangTable[i].LangName, TEXT("Greek"));
   strcpy (LangTable[i].LangCode, "el");
   LangTable[i].LangAlphabet = 'G';
   LangTable[i].LangPrincipal[0] = EOS;
   LangTable[i].LangSecondary[0] = EOS;

   i = 15;
   ustrcpy (LangTable[i].LangName, TEXT("Czech"));
   strcpy (LangTable[i].LangCode, "cs");
   LangTable[i].LangAlphabet = '2';
   LangTable[i].LangPrincipal[0] = EOS;
   LangTable[i].LangSecondary[0] = EOS;

   i = 17;
   ustrcpy (LangTable[i].LangName, TEXT("Polish"));
   strcpy (LangTable[i].LangCode, "pl");
   LangTable[i].LangAlphabet = '2';
   LangTable[i].LangPrincipal[0] = EOS;
   LangTable[i].LangSecondary[0] = EOS;

   i = 18;
   ustrcpy (LangTable[i].LangName, TEXT("Turkish"));
   strcpy (LangTable[i].LangCode, "tr");
   LangTable[i].LangAlphabet = '9';
   LangTable[i].LangPrincipal[0] = EOS;
   LangTable[i].LangSecondary[0] = EOS;
   FreeEntry = 19;
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
   languageName: name of the language according RFC 1766.
   languageAlphabet: alphabet to be used for writing that language:
   `L' for ISO-Latin-1, `G' for Symbol (Greek).
   principalDictionary: name of the principal dictionary or NULL.
   secondDictionary: name of the secondary dictionary or NULL.

   Return value:
   identifier of the new language or 0 if the language cannot be added.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
Language            TtaNewLanguage (CHAR_T* languageName, char languageAlphabet, STRING principalDictionary, STRING secondDictionary)
#else  /* __STDC__ */
Language            TtaNewLanguage (languageName, languageAlphabet, principalDictionary, secondDictionary)
CHAR_T*             languageName;
char                languageAlphabet;
STRING              principalDictionary;
STRING              secondDictionary;
#endif /* __STDC__ */

{
   int                 i;

   i = 0;
   /* Avoids error cases */
   if (languageName == NULL)
      TtaError (ERR_invalid_parameter);
   else if (languageName[0] == WC_EOS)
      TtaError (ERR_invalid_parameter);
   else if (ustrlen (languageName) >= MAX_NAME_LENGTH)
      TtaError (ERR_string_too_long);
   else if (FreeEntry == MAX_LANGUAGES)
      TtaError (ERR_too_many_languages);
   else
     {
	if (ustrlen (languageName) == 2 || languageName[1] == TEXT('-') || languageName[2] == TEXT('-'))
	   /* it's an ISO-639 code */
	  {
	  for (i = 0; i < FreeEntry; i++)
	    if (!wc2iso_strcasecmp (languageName, LangTable[i].LangCode))
	      /* The language is already defined */
	      return i;
	  ustrcpy (LangTable[FreeEntry].LangName, languageName);
	  strcpy (LangTable[FreeEntry].LangCode, TtaGetLanguageCodeFromName (languageName));
	  }
	else
	  {
	  /* Consults the languages table to see if the language exists. */
	  for (i = 0; i < FreeEntry; i++)
	    if (!ustrcasecmp (languageName, LangTable[i].LangName))
	      /* The language is already defined */
	      return i;
	  ustrcpy (LangTable[FreeEntry].LangName, languageName);
	  strcpy (LangTable[FreeEntry].LangCode, TtaGetLanguageCodeFromName (languageName));
	  }

	/* Saves the new language */
	i = FreeEntry;
	LangTable[i].LangAlphabet = languageAlphabet;
	if (principalDictionary != NULL)
	  {
	     ustrncpy (LangTable[i].LangPrincipal, principalDictionary, MAX_NAME_LENGTH);
	     LangTable[i].LangPrincipal[MAX_NAME_LENGTH - 1] = EOS;
	  }
	else
	   LangTable[i].LangPrincipal[0] = EOS;
	if (secondDictionary != NULL)
	  {
	     ustrncpy (LangTable[i].LangSecondary, secondDictionary, MAX_NAME_LENGTH);
	     LangTable[i].LangSecondary[MAX_NAME_LENGTH - 1] = EOS;
	  }
	else
	   LangTable[i].LangSecondary[0] = EOS;
	FreeEntry++;
     }
   return i;
}


/*----------------------------------------------------------------------
   TtaRemoveLanguage

   Remove a language from the Thot language table.

   Parameters:
       language: the language to be removed.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaRemoveLanguage (Language language)
#else  /* __STDC__ */
void                TtaRemoveLanguage (language)
   Language language;
#endif /* __STDC__ */

{
   int                 i;

   i = (int) language;
   if (i >= FreeEntry || i < 0)
      TtaError (ERR_language_not_found);
   else
      {
      LangTable[i].LangName[0] = EOS;
      LangTable[i].LangCode[0] = EOS;
      /* don't erase LangAlphabet */
      LangTable[i].LangPrincipal[0] = WC_EOS;
      LangTable[i].LangSecondary[0] = WC_EOS;
      LangTable[i].LangPattern[0]   = WC_EOS;
      }
}

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
Language            TtaGetLanguageIdFromName (CHAR_T* languageName)
#else  /* __STDC__ */
Language            TtaGetLanguageIdFromName (languageName)
CHAR_T*             languageName;
#endif /* __STDC__ */

{ 
   int                 i;
   ThotBool            again;
   char                code[MAX_LENGTH];

   i = 0;
   /* Avoids error cases */
   if (languageName == NULL)
      TtaError (ERR_invalid_parameter);
   else if (languageName[0] == WC_EOS)
        TtaError (ERR_invalid_parameter);
   else if (ustrlen (languageName) >= MAX_NAME_LENGTH)
        TtaError (ERR_string_too_long);
   else
      {
          /* Consults the languages table to see if the language exists. */
          again = TRUE;
          while (again && i < FreeEntry)
               {
                   if (!wc2iso_strcasecmp (languageName, LangTable[i].LangCode))
                      /* The language is already defined */
                      again = FALSE;
                   else if (!ustrcasecmp (languageName, LangTable[i].LangName))
                        again = FALSE;
                   else
                        i++;
               }
          if (again)
             {
                /* The language dose not exist, search in TYPO languages */
                i = 0;
                while (again && i < FreeTypoEntry)
                      {
                          if (!ustrcmp (languageName, TypoLangTable[i].LangName))
                             /* The TYPO language is already defined */
                             again = FALSE;
                          else
                              i++;
                      }
             }
          if (again)
             {
                 strcpy (code, TtaGetLanguageCodeFromName (languageName));
                 if (code[0] != EOS)
                    {
                       i = 0;
                       while (again && i < FreeEntry)
                             if (!strcasecmp (code, LangTable[i].LangCode))
                                /* The language is already defined */
                                again = FALSE;
                             else
                                  i++;
                    }
             }
          if (again)
             {
                /* The language does not exist */
                i = 0;
                TtaError (ERR_language_not_found);
             }
      }
      /* returned value */
      return (Language) i;
}

/*----------------------------------------------------------------------
   TtaGetVarLANG

   Returns the 2 first chars of environment variable LANG or 'fr'.

   Return value:
   a string of 2 chars.
  ----------------------------------------------------------------------*/
CHAR_T* TtaGetVarLANG ()
{ 
   CHAR_T* langEVar = TtaGetEnvString ("LANG");

   if (langEVar == NULL)
      ustrcpy (StandardLANG, TEXT("en"));
   else if (!ustrcmp (langEVar, TEXT("C")) || !ustrcasecmp (langEVar, TEXT("iso_8859_1")))
        ustrcpy (StandardLANG, TEXT("fr"));
   else
     {
         ustrncpy (StandardLANG, langEVar, 2);
         StandardLANG[2] = WC_EOS;
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
   return TtaGetLanguageIdFromName (TtaGetVarLANG ());
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
Language            TtaGetLanguageIdFromAlphabet (CHAR_T languageAlphabet)
#else  /* __STDC__ */
Language            TtaGetLanguageIdFromAlphabet (languageAlphabet)
CHAR_T              languageAlphabet;
#endif /* __STDC__ */
{
   int                 i;
   ThotBool            again;

   i = 0;
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
       /* The language does not exist */
       i = 0;
       TtaError (ERR_language_not_found);
     }
   return (Language) i;
}


/*----------------------------------------------------------------------
   TtaGetAlphabet

   Not available for TYPO languages. Returns the alphabet of a language.

   Parameters:
   languageId: name of the language.

   Return value:
   a character that identifies the alphabet ('L' = latin, 'G' = greek).
  ----------------------------------------------------------------------*/
#ifdef __STDC__
char              TtaGetAlphabet (Language languageId)
#else  /* __STDC__ */
char              TtaGetAlphabet (languageId)
Language          languageId;
#endif /* __STDC__ */
{
   int                 i;

   i = (int) languageId;
   /* Verification of the parameter */
   if (i >= FreeEntry || i < 0)
     {
	TtaError (ERR_language_not_found);
	return (EOS);
     }
   return LangTable[i].LangAlphabet;
}


/*----------------------------------------------------------------------
   TtaGetLanguageName

   Not available for TYPO languages. Returns the name of a given language.

   Parameters:
   languageId: identifier of the language.

   Return value:
   the name of the language.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
CHAR_T*            TtaGetLanguageName (Language languageId)
#else  /* __STDC__ */
CHAR_T*            TtaGetLanguageName (languageId)
Language           languageId;
#endif /* __STDC__ */
{
   int                 i;

   i = (int) languageId;
   if (i >= FreeEntry || i < 0)
     {
	TtaError (ERR_language_not_found);
	Langbuffer[0] = WC_EOS;
     }
   else
      ustrcpy (Langbuffer, LangTable[i].LangName);
   return Langbuffer;
}


/*----------------------------------------------------------------------
   TtaGetLanguageCode

   Not available for TYPO languages.

   Returns the RFC-1766 code of a given language.

   Parameters:
   languageId: identifier of the language.

   Return value:
   the code of the language.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
CHAR_T*            TtaGetLanguageCode (Language languageId)
#else  /* __STDC__ */
CHAR_T*            TtaGetLanguageCode (languageId)
Language           languageId;
#endif /* __STDC__ */

{
   int                 i;

   i = (int) languageId;
   if (i >= FreeEntry || i < 0)
     {
	TtaError (ERR_language_not_found);
	Langbuffer[0] = WC_EOS;
     }
   else
      iso2wc_strcpy (Langbuffer, LangTable[i].LangCode);
   return Langbuffer;
}


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


/*----------------------------------------------------------------------
   TtaGetFirstUserLanguage

   Not available for TYPO languages.

   Returns the first user language known by Thot.

   Return value:
   the first user language number.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 TtaGetFirstUserLanguage ()
#else  /* __STDC__ */
int                 TtaGetFirstUserLanguage ()
#endif				/* __STDC__ */
{
   return FirstUserLang;
}


/*----------------------------------------------------------------------
  GetPatternList: Read the pattern file of the language and creates the
  appropriate structure.
  Return 0 if problem else returns 1 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            GetPatternList (Language langageId)
#else  /* __STDC__ */
ThotBool            GetPatternList (langageId)
Language            langageId;

#endif /* __STDC__ */
{
   int                 lang;
   int                 currentIndex;
   int                 previousLength;
   int                 i, lg;
   UCHAR_T             patternGot[MAX_LET_PATTERN];
   CHAR_T              weightGot[MAX_LET_PATTERN + 1];
   CHAR_T              patternFileName[THOT_MAX_CHAR];
   static CHAR_T*      dictPath;	/* Environment variable DICOPAR */
   CHAR_T*             ptPattern;
   FILE               *in;

   dictPath = TtaGetEnvString ("DICOPAR");
   if (dictPath == NULL)
     {
	/* The environment variable DICOPAR does not exist */
	TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_MISSING_DICOPAR), "DICOPAR");
	return (FALSE);
     }

   ustrcpy (patternFileName, dictPath);
   ustrcat (patternFileName, WC_DIR_STR);
   lang = (int) langageId;
   ptPattern = LangTable[lang].LangPattern;
   ustrcat (patternFileName, ptPattern);
   if ((in = ufopen (patternFileName, TEXT("r"))) == NULL)
     {
	TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_HYPHEN_FILE_NOT_OPEN), LangTable[lang].LangPattern);
	return (FALSE);
     }
   TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_HYPHEN_FILE_OPEN), ptPattern);
   currentIndex = 0;
   previousLength = 0;
   i = 0;

   while ((fscanf (in, "%s %s", patternGot, weightGot)) != EOF)
     {
	i++;
	lg = ustrlen (patternGot);
	if (lg != previousLength)
	  {
	     previousLength = lg;
	     currentIndex++;
	     LangTable[lang].LangTabPattern.ind_pattern[previousLength] = i;
	  }
	ustrcpy (LangTable[lang].LangTabPattern.liste_pattern[i].CarPattern, patternGot);
	ustrcpy (LangTable[lang].LangTabPattern.liste_pattern[i].PoidsPattern, weightGot);
     }
   LangTable[lang].LangTabPattern.NbPatt = i;
   LangTable[lang].LangTabPattern.Charge = 1;
   fclose (in);
   return (TRUE);
}


/*----------------------------------------------------------------------
  FoundPatternInList verifies if a string belongs to the pattern list.
  if true, it returns 1 else 0 
  ----------------------------------------------------------------------*/
#ifdef __STDC
static STRING       FoundPatternInList (Language langageId, UCHAR_T substring[MAX_LET_PATTERN])
#else  /* __STDC__ */
static STRING       FoundPatternInList (langageId, substring)
Language            langageId;
UCHAR_T             substring[MAX_LET_PATTERN];

#endif /* __STDC__ */
{
   int                 language;
   int                 lgstring;
   int                 i, max;
   struct PatternList *ptrTabPattern;

   language = (int) langageId;
   lgstring = ustrlen (substring);
   if (lgstring >= MAX_LET_PATTERN)
     return (NULL);
   else
     {
       ptrTabPattern = &LangTable[language].LangTabPattern;
       i = ptrTabPattern->ind_pattern[lgstring];
       if (i == 0)
	 return (NULL);
       else
	 {
	   /* search last index */
	   max = lgstring + 1;
	   if (max == MAX_LET_PATTERN)
	     max = ptrTabPattern->NbPatt;
	   else
	     {
	       max = ptrTabPattern->ind_pattern[max];
	       if (max == 0)
		 max = ptrTabPattern->NbPatt;
	     }
	   while (i < max)
	     {
	       if (!ustrcmp (ptrTabPattern->liste_pattern[i].CarPattern, substring))
		 return (ptrTabPattern->liste_pattern[i].PoidsPattern);
	       i++;
	     }
	   return (NULL);
	 }
     }
}


/*----------------------------------------------------------------------
   * FoundHyphenPoints: apply Liang algo. on a word and returns the 
   * hypen points.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         FoundHyphenPoints (Language langageId, CHAR_T wordToCut[THOT_MAX_CHAR])
#else  /* __STDC__ */
static void         FoundHyphenPoints (langageId, wordToCut)
Language            langageId;
CHAR_T              wordToCut[THOT_MAX_CHAR];

#endif /* __STDC__ */
{
   int                 lang;
   UCHAR_T             wordToTreat[THOT_MAX_CHAR];	/* "." + wordToCut + "." */
   UCHAR_T             subword[THOT_MAX_CHAR];
   int                 tab_weight[THOT_MAX_CHAR];
   STRING              weight_subword;
   int                 wordLength;
   int                 size_subword;
   int                 currentPosition;
   int                 i, j;

   lang = (int) langageId;
   wordLength = ustrlen (wordToCut) + 2;
   if (wordLength > THOT_MAX_CHAR)
     {
	TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_HYPHEN_WORD_TOO_LONG), wordToCut);
	return;
     }
   for (i = 0; i < THOT_MAX_CHAR; i++)
      tab_weight[i] = 0;
   ustrcpy (wordToTreat, TEXT("."));
   ustrcat (wordToTreat, wordToCut);
   ustrcat (wordToTreat, TEXT("."));
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


/*----------------------------------------------------------------------
 TtaGetPatternHyphenList 
   returns a pointer on the list of values representing the hyphen points
   or NULL 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                *TtaGetPatternHyphenList (CHAR_T word[THOT_MAX_CHAR], Language languageId)
#else  /* __STDC__ */
int                *TtaGetPatternHyphenList (word, languageId)
CHAR_T              word[THOT_MAX_CHAR];
Language            languageId;

#endif /* __STDC__ */
{
   int                 language;
   int                 i;

   language = (int) languageId;
   if (word[0] == EOS)
      return (NULL);
   if (ustrlen (word) < 2)
      return (NULL);
   if (LangTable[language].LangPattern[0] == EOS)
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

/*----------------------------------------------------------------------
 * TtaExistPatternList verifies if a list of patterns is defined
 * for a given language
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            TtaExistPatternList (Language languageId)
#else  /* __STDC__ */
ThotBool            TtaExistPatternList (languageId)
Language            languageId;

#endif /* __STDC__ */
{
   int                 language;

   language = (int) languageId;
   if (LangTable[language].LangPattern[0] != EOS)
      return TRUE;
   else
      return FALSE;
}
