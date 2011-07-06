/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *
 * Languages and Scripts
 *
 * Author: I. Vatton(W3C/INRIA)
 */
#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "libmsg.h"
#include "message.h"
#include "language.h"
#include "application.h"
#include "dictionary.h"

struct Langue_Ctl   LangTable[MAX_LANGUAGES];
static char         Langbuffer[2 * MAX_NAME_LENGTH];
static char         Langbuffer1[2 * MAX_NAME_LENGTH];
static char         Langbuffer2[2 * MAX_NAME_LENGTH];
static char         CodeBuffer[2 * MAX_NAME_LENGTH];
static int          breakPoints[MAX_POINT_COUP];
static char        *dictPath;	/* Environment variable DICOPAR */
int                 FreeEntry;

#include "thotmsg_f.h"

/*	ISO 639 CODES ALPHABETIC BY LANGUAGE NAME (ENGLISH SPELLING) */
typedef unsigned char aLangName[MAX_LENGTH];
typedef struct _ISO639entry
  {
    aLangName	fullName;
    aLangName	code;
  }
ISO639entry;
// Table used to interpret wxLanguage
static ISO639entry	ISO639table[] =
{
	{"Abkhazian",      "ab"},
	{"Afan",           "om"},
	{"Afar",           "aa"},
	{"Afrikaans",      "af"},
	{"Albanian",       "sq"},
	{"Amharic",        "am"},
	{"Arabic",         "ar"},
	{"Arabic",         "ar"},
	{"Arabic",         "ar"},
	{"Arabic",         "ar"},
	{"Arabic",         "ar"},
	{"Arabic",         "ar"},
	{"Arabic",         "ar"},
	{"Arabic",         "ar"},
	{"Arabic",         "ar"},
	{"Arabic",         "ar"},
	{"Arabic",         "ar"},
	{"Arabic",         "ar"},
	{"Arabic",         "ar"},
	{"Arabic",         "ar"},
	{"Arabic",         "ar"},
	{"Arabic",         "ar"},
	{"Arabic",         "ar"},
	{"Arabic",         "ar"},
	{"Armenian",       "hy"},
	{"Assamese",       "as"},
	{"Aymara",         "ay"},
	{"Azerbaijani",    "az"},
	{"Azerbaijani",    "az"},
	{"Azerbaijani",    "az"},
	{"Bashkir",        "ba"},
	{"Basque",         "eu"},
	{"Belarussian",    "be"},
	{"Bengali",        "bn"},
	{"Bangla",         "bn"},
	{"Bhutani",        "dz"},
	{"Bihari",         "bh"},
	{"Bislama",        "bi"},
	{"Breton",         "br"},
	{"Bulgarian",      "bg"},
	{"Burmese",        "my"},
	{"Cambodian",      "km"},
	{"Catalan",        "ca"},
	{"Chinese",        "zh"},
	{"Chinese",        "zh"},
	{"Chinese",        "zh"},
	{"Chinese",        "zh"},
	{"Chinese",        "zh"},
	{"Chinese",        "zh"},
	{"Chinese",        "zh"},
	{"Corsican",       "co"},
	{"Croatian",       "hr"},
	{"Czech",          "cs"},
	{"Danish",         "da"},
	{"Dutch",          "nl"},
	{"Dutch",          "nl"},
	{"English",        "en"},
	{"English",        "en"},
	{"English",        "en"},
	{"English",        "en"},
	{"English",        "en"},
	{"English",        "en"},
	{"English",        "en"},
	{"English",        "en"},
	{"English",        "en"},
	{"English",        "en"},
	{"English",        "en"},
	{"English",        "en"},
	{"English",        "en"},
	{"English",        "en"},
	{"English",        "en"},
	{"English",        "en"},
	{"Esperanto",      "eo"},
	{"Estonian",       "et"},
	{"Faroese",        "fo"},
	{"Fiji",           "fj"},
	{"Finnish",        "fi"},
	{"French",         "fr"},
	{"French",         "fr"},
	{"French",         "fr"},
	{"French",         "fr"},
	{"French",         "fr"},
	{"French",         "fr"},
	{"Frisian",        "fy"},
	{"Galician",       "gl"},
	{"Georgian",       "ka"},
	{"German",         "de"},
	{"German",         "de"},
	{"German",         "de"},
	{"German",         "de"},
	{"German",         "de"},
	{"German",         "de"},
	{"Greek",          "el"},
	{"Greenlandic",    "kl"},
	{"Guarani",        "gn"},
	{"Gujarati",       "gu"},
	{"Hausa",          "ha"},
	{"Hebrew",         "he"},
	{"Hindi",          "hi"},
	{"Hungarian",      "hu"},
	{"Icelandic",      "is"},
	{"Indonesian",     "id"},
	{"Interlingua",    "ia"},
	{"Interlingue",    "ie"},
	{"Inuktitut",      "iu"},
	{"Inupiak",        "ik"},
	{"Irish",          "ga"},
	{"Italian",        "it"},
	{"Italian",        "it"},
	{"Japanese",       "ja"},
	{"Javanese",       "jv"},
	{"Kannada",        "kn"},
	{"Kashmiri",       "ks"},
	{"Kazakh",         "kk"},
	{"Kazakh",         "kk"},
	{"Kazakh",         "kk"},
	{"Kinyarwanda",    "rw"},
	{"Kirghiz",        "ky"},
	{"Kurundi",        "rn"},
	{"Konkani",        "ko"},
	{"Korean",         "ko"},
	{"Kurdish",        "ku"},
	{"Laothian",       "lo"},
	{"Latin",          "la"},
	{"Latvian",        "lv"},
	{"Lingala",        "ln"},
	{"Lithuanian",     "lt"},
	{"Luxembourg",     "lb"},
	{"Macedonian",     "mk"},
	{"Malagasy",       "mg"},
	{"Malay",          "ms"},
	{"Malayalam",      "ml"},
	{"Malay",          "ms"},
	{"Malay",          "ms"},
	{"Maltese",        "mt"},
	{"Maori",          "mi"},
	{"Marathi",        "mr"},
	{"Moldavian",      "mo"},
	{"Mongolian",      "mn"},
	{"Nauru",          "na"},
	{"Nepali",         "ne"},
	{"Norwegian",      "no"},
	{"Norwegian",      "no"},
	{"Occitan",        "oc"},
	{"Oriya",          "or"},
	{"Oromo",          "or"},
	{"Pashto",         "ps"},
	{"Polish",         "pl"},
	{"Portuguese",     "pt"},
	{"Portuguese",     "pt"},
	{"Punjabi",        "pa"},
	{"Quechua",        "qu"},
	{"Rhaeto-romance", "rm"},
	{"Romanian",       "ro"},
	{"Russian",        "ru"},
	{"Russian",        "ru"},
	{"Samoan",         "sm"},
	{"Sangho",         "sg"},
	{"Sanskrit",       "sa"},
	{"Scots gaelic",   "gd"},
	{"Serbian",        "sr"},
	{"Serbian",        "sr"},
	{"Serbian",        "sr"},
	{"Serbo-croatian", "sh"},
	{"Sesotho",        "st"},
	{"Setswana",       "tn"},
	{"Shona",          "sn"},
	{"Sindhi",         "sd"},
	{"Singhalese",     "si"},
	{"Siswati",        "ss"},
	{"Slovak",         "sk"},
	{"Slovenian",      "sl"},
	{"Somali",         "so"},
	{"Spanish",        "es"},
	{"Spanish",        "es"},
	{"Spanish",        "es"},
	{"Spanish",        "es"},
	{"Spanish",        "es"},
	{"Spanish",        "es"},
	{"Spanish",        "es"},
	{"Spanish",        "es"},
	{"Spanish",        "es"},
	{"Spanish",        "es"},
	{"Spanish",        "es"},
	{"Spanish",        "es"},
	{"Spanish",        "es"},
	{"Spanish",        "es"},
	{"Spanish",        "es"},
	{"Spanish",        "es"},
	{"Spanish",        "es"},
	{"Spanish",        "es"},
	{"Spanish",        "es"},
	{"Spanish",        "es"},
	{"Spanish",        "es"},
	{"Sundanese",      "su"},
	{"Swahili",        "sw"},
	{"Swedish",        "sv"},
	{"Swedish",        "sv"},
	{"Tagalog",        "tl"},
	{"Tajik",          "tg"},
	{"Tamil",          "ta"},
	{"Tatar",          "tt"},
	{"Telugu",         "te"},
	{"Thai",           "th"},
	{"Tibetan",        "bo"},
	{"Tigrinya",       "ti"},
	{"Tonga",          "to"},
	{"Tsonga",         "ts"},
	{"Turkish",        "tr"},
	{"Turkmen",        "tk"},
	{"Twi",            "tw"},
	{"Uigur",          "ug"},
	{"Ukrainian",      "uk"},
	{"Urdu",           "ur"},
	{"Urdu",           "ur"},
	{"Urdu",           "ur"},
	{"Uzbek",          "uz"},
	{"Uzbek",          "uz"},
	{"Uzbek",          "uz"},
	{"Vietnamese",     "vi"},
	{"Volapuk",        "vo"},
	{"Welsh",          "cy"},
	{"Wolof",          "wo"},
	{"Xhosa",          "xh"},
	{"Yiddish",        "yi"},
	{"Yoruba",         "yo"},
	{"Zhuang",         "za"},
	{"Zulu",           "zu"},
	{"",               ""}
};


/*----------------------------------------------------------------------
   TtaGetLanguageNameFromCode
   Returns the full name of a language whose RFC-1766 code is known
  ----------------------------------------------------------------------*/
char *TtaGetLanguageNameFromCode (const char *code)
{
  const char zh[] = "zh";
  const char* localcode = code;
  int        i;
  

  Langbuffer[0] = EOS;
  if (!strcmp (code, "cn") || !strcmp (code, "tw"))
    // a patch to support traditional and simplified chinese
    localcode = zh;
  
  for (i = 0; Langbuffer[0] == EOS && ISO639table[i].code[0] != EOS; i++)
    {
      if (!strncasecmp (localcode, (const char *)ISO639table[i].code, 2))
        strcpy (Langbuffer, (const char *)ISO639table[i].fullName);
    }
  return Langbuffer;
}

/*----------------------------------------------------------------------
   TtaGetISO639Code
   Returns the language code in the ISO639 table
  ----------------------------------------------------------------------*/
const char *TtaGetISO639Code (int val)
{
  int                 max;

  max = sizeof (ISO639table) / sizeof(ISO639entry);
  if (val < 2 || val >= max)
    return "en";
  else if (ISO639table[val].code[0] != EOS)
    return (char *)(ISO639table[val].code);
  else
    return "en";
}


/*----------------------------------------------------------------------
   GetListOfLanguages
   Stores in the buffer the maximum entries of the languages declared in
   RFC-1766 and returns into nbItem the number of languages stored.
   The parameter length gives the size of the buffer.
   The parameter languageCode gives the code of the requested language.
   The parameter defItem returns the entry of the requested language.
   If defItem is positif the function returns the full name of the Language.
  ----------------------------------------------------------------------*/
char *GetListOfLanguages (char *buffer, int length, char *languageCode,
			 int *nbItem, int *defItem)
{
  int                 i, l;

  *nbItem = 0;
  *defItem = -1;
  for (i = 0; ISO639table[i].code[0] != EOS; i++)
    {
      if (i == 0 ||
          strcmp ((char *)ISO639table[i].code, (char *)ISO639table[i-1].code))
        {
      l = strlen ((const char *)ISO639table[i].fullName) + 1;
      if (l > 1 && l < length)
        {
          if (*defItem < 0 && languageCode && languageCode[0] != EOS &&
              strcasecmp ((const char *)ISO639table[i].code, languageCode) == 0)
            /* position of the selected language in the list */
            *defItem = i;
          (*nbItem)++;
          strcpy (buffer, (const char *)ISO639table[i].fullName);
          buffer = buffer + l;
          length -= l;
        }
      else
        break;
        }
    }
  if (*defItem >= 0)
    return (char *)ISO639table[*defItem].fullName;
  else
    return (char *)NULL;
}

/*----------------------------------------------------------------------
   TtaGetLanguageCodeFromName
   Returns the RFC-1766 code for a language whose name is known
  ----------------------------------------------------------------------*/
char *TtaGetLanguageCodeFromName (const char *name)
{
  int                 i;

  CodeBuffer[0] = EOS;
  for (i = 0; CodeBuffer[0] == EOS && ISO639table[i].fullName[0] != EOS; i++)
    {
      if (!strcasecmp (name, (const char *)ISO639table[i].fullName) ||
	  !strcasecmp (name, (const char *)ISO639table[i].code))
	strcpy (CodeBuffer, (const char *)ISO639table[i].code);
    }
  return CodeBuffer;
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void InitLanguage ()
{
   int                 i, j;

   /* Initialization of remaining entries of the language table */
   for (i = 0; i < MAX_LANGUAGES; i++)
     {
	LangTable[i].LangName[0] = EOS;
	LangTable[i].LangCode[0] = EOS;
	LangTable[i].LangScript = 'L';
	for (j = 0; j < MAX_DICTS; j++)
	   LangTable[i].LangDict[j] = NULL;
	LangTable[i].LangPattern[0] = EOS;
	LangTable[i].LangTabPattern.Charge = 0;
     }
   /*
     Loading the default user languages:
     Values less than FirstUserLang are scripts (Latin, Greek, Arabic, Hebrew)
   */

   i = 0;
   strcpy (LangTable[i].LangName, "Arabic");
   strcpy (LangTable[i].LangCode, "ar");
   LangTable[i].LangScript = 'A';
   LangTable[i].LangPrincipal[0] = EOS;
   LangTable[i].LangSecondary[0] = EOS;

   i = 1;
   strcpy (LangTable[i].LangName, "Chinese");
   strcpy (LangTable[i].LangCode, "zh");
   LangTable[i].LangScript = 'C';
   LangTable[i].LangPrincipal[0] = EOS;
   LangTable[i].LangSecondary[0] = EOS;
   FreeEntry = 19;

   i = 2;
   strcpy (LangTable[i].LangName, "Czech");
   strcpy (LangTable[i].LangCode, "cs");
   LangTable[i].LangScript = '2';
   LangTable[i].LangPrincipal[0] = EOS;
   LangTable[i].LangSecondary[0] = EOS;

   i = 3;
   strcpy (LangTable[i].LangName, "Dutch");
   strcpy (LangTable[i].LangCode, "nl");
   LangTable[i].LangScript = 'L';
   strcpy (LangTable[i].LangPrincipal, "Nprinc");
   LangTable[i].LangSecondary[0] = EOS;
   strcpy (LangTable[i].LangPattern, "nederl.ptn");

   i = 4;
   strcpy (LangTable[i].LangName, "English");
   strcpy (LangTable[i].LangCode, "en");
   LangTable[i].LangScript = 'L';
   strcpy (LangTable[i].LangPrincipal, "Eprinc");
   strcpy (LangTable[i].LangSecondary, "Eperso");
   strcpy (LangTable[i].LangPattern, "english.ptn");

   i = 5;
   strcpy (LangTable[i].LangName, "Finnish");
   strcpy (LangTable[i].LangCode, "fi");
   LangTable[i].LangScript = 'L';
   LangTable[i].LangPrincipal[0] = EOS;
   LangTable[i].LangSecondary[0] = EOS;
   strcpy (LangTable[i].LangPattern, "finish.ptn");

   i = 6;
   strcpy (LangTable[i].LangName, "French");
   strcpy (LangTable[i].LangCode, "fr");
   LangTable[i].LangScript = 'L';
   strcpy (LangTable[i].LangPrincipal, "Fprinc");
   strcpy (LangTable[i].LangSecondary, "Fperso");
   strcpy (LangTable[i].LangPattern, "francais.ptn");

   i = 7;
   strcpy (LangTable[i].LangName, "German");
   strcpy (LangTable[i].LangCode, "de");
   LangTable[i].LangScript = 'L';
   strcpy (LangTable[i].LangPrincipal, "Gprinc");
   LangTable[i].LangSecondary[0] = EOS;
   strcpy (LangTable[i].LangPattern, "deutsch.ptn");

   i = 8;
   strcpy (LangTable[i].LangName, "Greek");
   strcpy (LangTable[i].LangCode, "el");
   LangTable[i].LangScript = 'G';
   LangTable[i].LangPrincipal[0] = EOS;
   LangTable[i].LangSecondary[0] = EOS;

   i = 9;
   strcpy (LangTable[i].LangName, "Hebrew");
   strcpy (LangTable[i].LangCode, "he");
   LangTable[i].LangScript = 'H';
   LangTable[i].LangPrincipal[0] = EOS;
   LangTable[i].LangSecondary[0] = EOS;

   i = 10;
   strcpy (LangTable[i].LangName, "Icelandic");
   strcpy (LangTable[i].LangCode, "is");
   LangTable[i].LangScript = 'L';
   LangTable[i].LangPrincipal[0] = EOS;
   LangTable[i].LangSecondary[0] = EOS;

   i = 11;
   strcpy (LangTable[i].LangName, "Italian");
   strcpy (LangTable[i].LangCode, "it");
   LangTable[i].LangScript = 'L';
   strcpy (LangTable[i].LangPrincipal, "Iprinc");
   LangTable[i].LangSecondary[0] = EOS;
   strcpy (LangTable[i].LangPattern, "italiano.ptn");

   i = 12;
   strcpy (LangTable[i].LangName, "Japanese");
   strcpy (LangTable[i].LangCode, "ja");
   LangTable[i].LangScript = 'Z';
   LangTable[i].LangPrincipal[0] = EOS;
   LangTable[i].LangSecondary[0] = EOS;

   i = 13;
   strcpy (LangTable[i].LangName, "Polish");
   strcpy (LangTable[i].LangCode, "pl");
   LangTable[i].LangScript = '2';
   LangTable[i].LangPrincipal[0] = EOS;
   LangTable[i].LangSecondary[0] = EOS;

   i = 14;
   strcpy (LangTable[i].LangName, "Portuguese");
   strcpy (LangTable[i].LangCode, "pt");
   LangTable[i].LangScript = 'L';
   LangTable[i].LangPrincipal[0] = EOS;
   LangTable[i].LangSecondary[0] = EOS;
   strcpy (LangTable[i].LangPattern, "portug.ptn");

   i = 15;
   strcpy (LangTable[i].LangName, "Spanish");
   strcpy (LangTable[i].LangCode, "es");
   LangTable[i].LangScript = 'L';
   strcpy (LangTable[i].LangPrincipal, "Sprinc");
   LangTable[i].LangSecondary[0] = EOS;
   strcpy (LangTable[i].LangPattern, "espanol.ptn");

   i = 16;
   strcpy (LangTable[i].LangName, "Swedish");
   strcpy (LangTable[i].LangCode, "sv");
   LangTable[i].LangScript = 'L';
   strcpy (LangTable[i].LangPrincipal, "Wprinc");
   LangTable[i].LangSecondary[0] = EOS;
   strcpy (LangTable[i].LangPattern, "swedish.ptn");

   i = 17;
   strcpy (LangTable[i].LangName, "Turkish");
   strcpy (LangTable[i].LangCode, "tr");
   LangTable[i].LangScript = '9';
   LangTable[i].LangPrincipal[0] = EOS;
   LangTable[i].LangSecondary[0] = EOS;

   i = 18;
   strcpy (LangTable[i].LangName, "Hungarian");
   strcpy (LangTable[i].LangCode, "hu");
   LangTable[i].LangScript = '2';
   LangTable[i].LangPrincipal[0] = EOS;
   LangTable[i].LangSecondary[0] = EOS;
}


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
Language TtaNewLanguage (char *languageName, char languageScript,
			 char *principalDictionary, char *secondDictionary)
{
  int                 i;

  i = 0;
  /* Avoids error cases */
  if (languageName == NULL)
    TtaError (ERR_invalid_parameter);
  else if (languageName[0] == EOS)
    TtaError (ERR_invalid_parameter);
  else if (strlen (languageName) >= MAX_NAME_LENGTH)
    TtaError (ERR_string_too_long);
  else if (FreeEntry == MAX_LANGUAGES)
    TtaError (ERR_too_many_languages);
  else
    {
      if (strlen (languageName) == 2 ||
	  languageName[1] == '-' || languageName[2] == '-')
	/* it's an ISO-639 code */
	{
	  for (i = 0; i < FreeEntry; i++)
	    if (!strcasecmp (languageName, LangTable[i].LangCode))
	      /* The language is already defined */
	      return i;
	  strcpy (LangTable[FreeEntry].LangName, languageName);
	  strcpy (LangTable[FreeEntry].LangCode,
		  TtaGetLanguageCodeFromName (languageName));
	}
      else
	{
	  /* Consults the languages table to see if the language exists. */
	  for (i = 0; i < FreeEntry; i++)
	    if (!strcasecmp (languageName, LangTable[i].LangName))
	      /* The language is already defined */
	      return i;
	  strcpy (LangTable[FreeEntry].LangName, languageName);
	  strcpy (LangTable[FreeEntry].LangCode, TtaGetLanguageCodeFromName (languageName));
	}
      
      /* Saves the new language */
      i = FreeEntry;
      LangTable[i].LangScript = languageScript;
      if (principalDictionary != NULL)
	{
	  strncpy (LangTable[i].LangPrincipal, principalDictionary, MAX_NAME_LENGTH);
	  LangTable[i].LangPrincipal[MAX_NAME_LENGTH - 1] = EOS;
	}
      else
	LangTable[i].LangPrincipal[0] = EOS;
      if (secondDictionary != NULL)
	{
	  strncpy (LangTable[i].LangSecondary, secondDictionary, MAX_NAME_LENGTH);
	  LangTable[i].LangSecondary[MAX_NAME_LENGTH - 1] = EOS;
	}
      else
	LangTable[i].LangSecondary[0] = EOS;
      FreeEntry++;
    }
  return (Language) (i + FirstUserLang);
}


/*----------------------------------------------------------------------
   TtaRemoveLanguage

   Remove a language from the Thot language table.
   Parameters:
       language: the language to be removed.
  ----------------------------------------------------------------------*/
void TtaRemoveLanguage (Language language)
{
  int                 i;

  i = (int) language - FirstUserLang;
  if (i >= FreeEntry || i < 0)
    TtaError (ERR_language_not_found);
  else
    {
      LangTable[i].LangName[0] = EOS;
      LangTable[i].LangCode[0] = EOS;
      /* don't erase LangScript */
      LangTable[i].LangPrincipal[0] = EOS;
      LangTable[i].LangSecondary[0] = EOS;
      LangTable[i].LangPattern[0]   = EOS;
    }
}

/*----------------------------------------------------------------------
   TtaGetLanguageIdFromName

   Available for TYPO languages.
   Returns the identifier of a language that matches a language name.
   name: name of the language.
   Return value:
   identifier of that language, 0 if the language is not supported, 
   -1 for an unknown language.
  ----------------------------------------------------------------------*/
Language TtaGetLanguageIdFromName (const char *name)
{ 
  char                code[MAX_LENGTH], *ptr;
  int                 i;
  ThotBool            again;

  i = 0;
  /* Avoids error cases */
  if (name == NULL)
    TtaError (ERR_invalid_parameter);
  else if (name[0] == EOS)
    TtaError (ERR_invalid_parameter);
  else if (strlen (name) >= MAX_NAME_LENGTH)
    TtaError (ERR_string_too_long);
  else
    {
      /* Checks the languages table to see if the language exists */
      again = TRUE;
      while (again && i < FreeEntry)
	{
	  if (!strcasecmp (name, LangTable[i].LangCode))
	    /* The language is already defined */
	    again = FALSE;
	  else if (!strcasecmp (name, LangTable[i].LangName))
	    again = FALSE;
	  else if (name[2] == '-' &&
		   !strncasecmp (name, LangTable[i].LangCode, 2))
	    again = FALSE;
	  else
	    i++;
	}

      if (again)
	{
	  strcpy (code, TtaGetLanguageCodeFromName (name));
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
	  ptr = TtaGetLanguageNameFromCode (name);
	  if (*ptr == EOS)
	    return -1;
	  else
	    return Latin_Script;
	}
    }
  /* returned value */
  return (Language) (i + FirstUserLang);
}

/*----------------------------------------------------------------------
   TtaGetVarLANG

   Returns the 2 first chars of environment variable LANG or 'fr'.
   Return value:
   a string of 2 chars.
  ----------------------------------------------------------------------*/
char *TtaGetVarLANG ()
{
  return TtaGetEnvString ("LANG");
}

/*----------------------------------------------------------------------
   TtaGetDefaultLanguage

   Returns the identifier of the default language.
   Return value:
   identifier of the default language.
  ----------------------------------------------------------------------*/
Language TtaGetDefaultLanguage ()
{
   return TtaGetLanguageIdFromName (TtaGetVarLANG ());
}


/*----------------------------------------------------------------------
   TtaGetLanguageIdFromScript

   Returns the identifier of the first language that uses a given script
   `L' -> ISO_latin_1, `G' -> Symbol.
   Parameters:
   languageScript: the script of interest.
   Return value:
   identifier of that language or 0 if the language is unknown.
  ----------------------------------------------------------------------*/
Language TtaGetLanguageIdFromScript (char languageScript)
{
  /* No user language declared */
  switch (languageScript)
    {
    case 'G':
      return Greek_Script;
    case 'A':
      return Arabic_Script;
    case 'H':
      return Hebrew_Script;
    case 'C':
      return Cyrillic_Script;
    case 'Z':
      return CJK_Script;
    default:
      return Latin_Script;
    }
}


/*----------------------------------------------------------------------
   TtaGetCharacterScript

   Returns the script of the wide character.
   Return value:
   A character that identifies the script
   'L' = Latin, 'G' = Greek 'A' = Arabic, etc.
  ----------------------------------------------------------------------*/
char TtaGetCharacterScript (wchar_t c)
{

  if (c == SPACE || c == 0x9A /* nbsp */ || c == 0x200D || c == 0x202C)
  return ' ';
    else if (c == 0x25 || c == 0xFF05 /* % */ ||
	     c == 0x2B || c == 0xFF0B  /* + */ ||
	     c == 0x2D || c == 0xFF0D  /* - */)
      return ' ';
  else if (c == 0x2F || c == 0xFF0F)
    return ' '; /* a slash */
  else if ((c >= 0x30 && c <= 0x39) || (c >= 0xFF10 && c < 0xFF1A))
    return 'D'; /* a digit */
  else if (c <= 0x2F || c == 0x3A || c == 0x3D || (c >= 0xFF01 && c < 0xFF20))
    return ' '; /* a punctuation */
  else if (c >= 0x370 && c < 0x400)
    return 'L'; /* Greek */
  else if (c >= 0x2AF && c < 0x45F)
    return 'L'; /* Cyrillic */
  else if (c >= 0x45F && c < 0x600)
    return 'H'; /* Hebrew */
  else if ((c >= 0x600 && c < 0x6B0 )|| c == 0x200F || c == 0x202B || c == 0x202E)
    return 'A'; /* Arabic */
  else if (c >= 0x2E80 && c < 0xA000)
    return 'Z'; /* CJK */
  else if (c >= 0xFF65 && c < 0xFFDD)
    return 'Z'; /* CJK */
  else
    return 'L'; /* Latin */
}

/*----------------------------------------------------------------------
   TtaGetScript

   Returns the script of a language languageId.
   Return value:
   A character that identifies the script
   'L' = Latin, 'G' = Greek 'A' = Arabic, etc.
  ----------------------------------------------------------------------*/
char TtaGetScript (Language languageId)
{
   int                 i;

   i = (int) languageId - FirstUserLang;
   if (i < 0)
     {
      switch (languageId)
	{
	case Greek_Script:
	  return 'G';
	case  Arabic_Script:
	  return 'A';
	case  Hebrew_Script:
	  return 'H';
	case Cyrillic_Script:
	  return 'C';
	case CJK_Script:
	  return 'Z';
	default:
	  return 'L';
	}
      return languageId;
     }
   else if (i >= FreeEntry)
     /* undeclared language */
     return 'L';
   return LangTable[i].LangScript;
}


/*----------------------------------------------------------------------
  TtaGetLanguageName

  Returns the name of a given language.
  languageId: identifier of the language.
  Return value:
  the name of the language.
  ----------------------------------------------------------------------*/
char *TtaGetLanguageName (Language languageId)
{
   int                 i;

   i = (int) languageId - FirstUserLang;
   if (i < 0 || i >= FreeEntry)
     /* undeclared language */
     Langbuffer1[0] = EOS;
   else
      strcpy (Langbuffer1, LangTable[i].LangName);
   return Langbuffer1;
}


/*----------------------------------------------------------------------
   TtaGetLanguageCode

   Not available for TYPO languages.
   Returns the RFC-1766 code of a given language.
   languageId: identifier of the language.
   Return value:
   the code of the language.
  ----------------------------------------------------------------------*/
char *TtaGetLanguageCode (Language languageId)
{
  int                 i;

  i = (int) languageId - FirstUserLang;
   if (i < 0 || i >= FreeEntry)
     /* undeclared language */
     Langbuffer2[0] = EOS;
  else
    strcpy (Langbuffer2, LangTable[i].LangCode);
  return Langbuffer2;
}


/*----------------------------------------------------------------------
   TtaGetNumberOfLanguages
   Returns the current number of languages known by Thot.
   Return value:
   the current number of languages.
  ----------------------------------------------------------------------*/
int TtaGetNumberOfLanguages ()
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
int TtaGetFirstUserLanguage ()
{
  return FirstUserLang;
}


/*----------------------------------------------------------------------
  GetPatternList: Read the pattern file of the language and creates the
  appropriate structure.
  Return 0 if problem else returns 1 
  ----------------------------------------------------------------------*/
ThotBool GetPatternList (Language langId)
{
   FILE               *in;
   unsigned char       patternGot[MAX_LET_PATTERN];
   char                weightGot[MAX_LET_PATTERN + 1];
   char                patternFileName[THOT_MAX_CHAR];
   char               *ptPattern;
   int                 lang;
   int                 currentIndex;
   int                 previousLength;
   int                 i, lg;

   dictPath = TtaGetEnvString ("DICOPAR");
   if (dictPath == NULL)
     /* The environment variable DICOPAR does not exist */
     return (FALSE);

   strcpy (patternFileName, dictPath);
   strcat (patternFileName, DIR_STR);
   lang = (int) langId - FirstUserLang;
   ptPattern = LangTable[lang].LangPattern;
   strcat (patternFileName, ptPattern);
   if ((in = TtaReadOpen (patternFileName)) == NULL)
     return (FALSE);

   currentIndex = 0;
   previousLength = 0;
   i = 0;
   while ((fscanf (in, "%s %s", patternGot, weightGot)) != EOF)
     {
	i++;
	lg = strlen ((const char *)patternGot);
	if (lg != previousLength)
	  {
	     previousLength = lg;
	     currentIndex++;
	     LangTable[lang].LangTabPattern.ind_pattern[previousLength] = i;
	  }
	strcpy ((char *)LangTable[lang].LangTabPattern.liste_pattern[i].CarPattern, (const char *)patternGot);
	strcpy ((char *)LangTable[lang].LangTabPattern.liste_pattern[i].PoidsPattern, weightGot);
     }
   LangTable[lang].LangTabPattern.NbPatt = i;
   LangTable[lang].LangTabPattern.Charge = 1;
   TtaReadClose (in);
   return (TRUE);
}


/*----------------------------------------------------------------------
  FoundPatternInList verifies if a string belongs to the pattern list.
  if true, it returns 1 else 0 
  ----------------------------------------------------------------------*/
static char *FoundPatternInList (Language langId,
				 unsigned char substring[MAX_LET_PATTERN])
{
  int                 lgstring;
  int                 i, max;
  struct PatternList *ptrTabPattern;

  lgstring = strlen ((const char *)substring);
  if (langId < 0 || lgstring >= MAX_LET_PATTERN)
    return (NULL);
  else
    {
      ptrTabPattern = &LangTable[langId].LangTabPattern;
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
	      if (!strcmp ((const char *)ptrTabPattern->liste_pattern[i].CarPattern, (const char *)substring))
		return (ptrTabPattern->liste_pattern[i].PoidsPattern);
	      i++;
	    }
	  return (NULL);
	}
    }
}


/*----------------------------------------------------------------------
  FoundHyphenPoints: apply Liang algo. on a word and returns the 
  hypen points.
  ----------------------------------------------------------------------*/
static void FoundHyphenPoints (Language langId, char wordToCut[THOT_MAX_CHAR])
{
   int                 lang;
   unsigned char       wordToTreat[THOT_MAX_CHAR];	/* "." + wordToCut + "." */
   unsigned char       subword[THOT_MAX_CHAR];
   char               *weight_subword;
   int                 tab_weight[THOT_MAX_CHAR];
   int                 wordLength;
   int                 size_subword;
   int                 currentPosition;
   int                 i, j;

   lang = (int) langId - FirstUserLang;
   wordLength = strlen (wordToCut) + 2;
   if (lang < 0 || wordLength > THOT_MAX_CHAR)
     return;

   for (i = 0; i < THOT_MAX_CHAR; i++)
      tab_weight[i] = 0;
   strcpy ((char *)wordToTreat, ".");
   strcat ((char *)wordToTreat, wordToCut);
   strcat ((char *)wordToTreat, ".");
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
int *TtaGetPatternHyphenList (char *word, Language languageId)
{
   int                 language;
   int                 i;

   language = (int) languageId - FirstUserLang;
   if (language < 0 || word[0] == EOS)
      return (NULL);
   if (strlen (word) < 2)
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
  TtaExistPatternList verifies if a list of patterns is defined
  for a given language
  ----------------------------------------------------------------------*/
ThotBool TtaExistPatternList (Language languageId)
{
  int                 language;
  
  language = (int) languageId - FirstUserLang;
  if (language < 0)
    return FALSE;
  else if (LangTable[language].LangPattern[0] != EOS)
    return TRUE;
  else
    return FALSE;
}
