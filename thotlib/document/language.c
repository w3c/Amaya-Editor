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
int		    FirstUserLang;
static char         Langbuffer[2 * MAX_NAME_LENGTH];
static int          breakPoints[MAX_POINT_COUP];
static char         StandardLANG[3];

#include "thotmsg_f.h"


/*	ISO 639 CODES ALPHABETIC BY LANGUAGE NAME (ENGLISH SPELLING) */

typedef unsigned char aLangName[16];
typedef struct _ISO639entry
  {
     aLangName	fullName;
     aLangName	code;
  }
ISO639entry;

static ISO639entry	ISO639table[] =
{
   {"ABKHAZIAN",	"AB"},
   {"AFAN (OROMO)",	"OM"},
   {"AFAR",		"AA"},
   {"AFRIKAANS",	"AF"},
   {"ALBANIAN",		"SQ"},
   {"AMHARIC",		"AM"},
   {"ARABIC",		"AR"},
   {"ARMENIAN",		"HY"},
   {"ASSAMESE",		"AS"},
   {"AYMARA",		"AY"},
   {"AZERBAIJANI",	"AZ"},
   {"BASHKIR",		"BA"},
   {"BASQUE",		"EU"},
   {"BENGALI",		"BN"},
   {"BANGLA",		"BN"},
   {"BHUTANI",		"DZ"},
   {"BIHARI",		"BH"},
   {"BISLAMA",		"BI"},
   {"BRETON",		"BR"},
   {"BULGARIAN",	"BG"},
   {"BURMESE",		"MY"},
   {"BYELORUSSIAN",	"BE"},
   {"CAMBODIAN",	"KM"},
   {"CATALAN",		"CA"},
   {"CHINESE",		"ZH"},
   {"CORSICAN",		"CO"},
   {"CROATIAN",		"HR"},
   {"Czech",		"CS"},
   {"DANISH",		"DA"},
   {"Dutch",		"NL"},
   {"English",		"EN"},
   {"ESPERANTO",	"EO"},
   {"ESTONIAN",		"ET"},
   {"FAROESE",		"FO"},
   {"FIJI",		"FJ"},
   {"Finnish",		"FI"},
   {"French",		"FR"},
   {"FRISIAN",		"FY"},
   {"GALICIAN",		"GL"},
   {"GEORGIAN",		"KA"},
   {"German",		"DE"},
   {"Greek",		"EL"},
   {"GREENLANDIC",	"KL"},
   {"GUARANI",		"GN"},
   {"GUJARATI",		"GU"},
   {"HAUSA",		"HA"},
   {"HEBREW",		"HE"},
   {"HINDI",		"HI"},
   {"HUNGARIAN",	"HU"},
   {"ICELANDIC",	"IS"},
   {"INDONESIAN",	"ID"},
   {"INTERLINGUA",	"IA"},
   {"INTERLINGUE",	"IE"},
   {"INUKTITUT",	"IU"},
   {"INUPIAK",		"IK"},
   {"IRISH",		"GA"},
   {"Italian",		"IT"},
   {"JAPANESE",		"JA"},
   {"JAVANESE",		"JV"},
   {"KANNADA",		"KN"},
   {"KASHMIRI",		"KS"},
   {"KAZAKH",		"KK"},
   {"KINYARWANDA",	"RW"},
   {"KIRGHIZ",		"KY"},
   {"KURUNDI",		"RN"},
   {"KOREAN",		"KO"},
   {"KURDISH",		"KU"},
   {"LAOTHIAN",		"LO"},
   {"LATIN",		"LA"},
   {"LATVIAN",		"LV"},
   {"LETTISH",		"LV"},
   {"LINGALA",		"LN"},
   {"LITHUANIAN",	"LT"},
   {"MACEDONIAN",	"MK"},
   {"MALAGASY",		"MG"},
   {"MALAY",		"MS"},
   {"MALAYALAM",	"ML"},
   {"MALTESE",		"MT"},
   {"MAORI",		"MI"},
   {"MARATHI",		"MR"},
   {"MOLDAVIAN",	"MO"},
   {"MONGOLIAN",	"MN"},
   {"NAURU",		"NA"},
   {"NEPALI",		"NE"},
   {"NORWEGIAN",	"NO"},
   {"OCCITAN",		"OC"},
   {"ORIYA",		"OR"},
   {"PASHTO",		"PS"},
   {"PUSHTO",		"PS"},
   {"PERSIAN",		"FA"},
   {"Polish",		"PL"},
   {"Portuguese",	"PT"},
   {"PUNJABI",		"PA"},
   {"QUECHUA",		"QU"},
   {"RHAETO-ROMANCE",	"RM"},
   {"ROMANIAN",		"RO"},
   {"RUSSIAN",		"RU"},
   {"SAMOAN",		"SM"},
   {"SANGHO",		"SG"},
   {"SANSKRIT",		"SA"},
   {"SCOTS GAELIC",	"GD"},
   {"SERBIAN",		"SR"},
   {"SERBO-CROATIAN",	"SH"},
   {"SESOTHO",		"ST"},
   {"SETSWANA",		"TN"},
   {"SHONA",		"SN"},
   {"SINDHI",		"SD"},
   {"SINGHALESE",	"SI"},
   {"SISWATI",		"SS"},
   {"SLOVAK",		"SK"},
   {"SLOVENIAN",	"SL"},
   {"SOMALI",		"SO"},
   {"Spanish",		"ES"},
   {"SUNDANESE",	"SU"},
   {"SWAHILI",		"SW"},
   {"Swedish",		"SV"},
   {"TAGALOG",		"TL"},
   {"TAJIK",		"TG"},
   {"TAMIL",		"TA"},
   {"TATAR",		"TT"},
   {"TELUGU",		"TE"},
   {"THAI",		"TH"},
   {"TIBETAN",		"BO"},
   {"TIGRINYA",		"TI"},
   {"TONGA",		"TO"},
   {"TSONGA",		"TS"},
   {"TURKISH",		"TR"},
   {"TURKMEN",		"TK"},
   {"TWI",		"TW"},
   {"UIGUR",		"UG"},
   {"UKRAINIAN",	"UK"},
   {"URDU",		"UR"},
   {"UZBEK",		"UZ"},
   {"VIETNAMESE",	"VI"},
   {"VOLAPUK",		"VO"},
   {"WELSH",		"CY"},
   {"WOLOF",		"WO"},
   {"XHOSA",		"XH"},
   {"YIDDISH",		"YI"},
   {"YORUBA",		"YO"},
   {"ZHUANG",		"ZA"},
   {"ZULU",		"ZU"},
   {"",			""}
};

/* this table associates the ancient language names used in Thot documents
   with their standard code */
static ISO639entry	OldLangTable[] =
{
   {"American",		"EN-US"},
   {"Deutsch",		"DE"},
   {"Espa\361ol",	"ES"},
   {"Fran\347ais",	"FR"},
   {"ISO_latin_1",	"x-Latin1"},
   {"ISO_latin_2",	"x-Latin2"},
   {"Italiano",		"IT"},
   {"Symbol",		"x-Symbol"},
   {"",			""}
};


/*----------------------------------------------------------------------
   TtaGetLanguageNameFromCode
   Returns the full name of a language whose RFC-1766 code is known
  ----------------------------------------------------------------------*/
#ifdef __STDC__
char            *TtaGetLanguageNameFromCode (char *code)
#else  /* __STDC__ */
char            *TtaGetLanguageNameFromCode (code)
char               *code;
#endif /* __STDC__ */

{
   int                 i;

   Langbuffer[0] = EOS;
   for (i = 0; Langbuffer[0] == EOS; i++)
      if (ISO639table[i].code[0] == EOS)
	    break;
      else if (strcasecmp (code, ISO639table[i].code) == 0)
	 strcpy (Langbuffer, ISO639table[i].fullName);
   if (Langbuffer[0] == EOS)
      for (i = 0; Langbuffer[0] == EOS; i++)
         if (OldLangTable[i].code[0] == EOS)
	    break;
         else
            if (strcasecmp (code, OldLangTable[i].code) == 0)
	       strcpy (Langbuffer, OldLangTable[i].fullName);
   return Langbuffer;
}


/*----------------------------------------------------------------------
   TtaGetLanguageCodeFromName
   Returns the RFC-1766 code for a language whose name is known
  ----------------------------------------------------------------------*/
#ifdef __STDC__
char            *TtaGetLanguageCodeFromName (char *name)
#else  /* __STDC__ */
char            *TtaGetLanguageCodeFromName (name)
char               *name;
#endif /* __STDC__ */

{
   int                 i;

   Langbuffer[0] = EOS;
   for (i = 0; Langbuffer[0] == EOS; i++)
      if (ISO639table[i].fullName[0] == EOS)
	 break;
      else
         if (strcasecmp (name, ISO639table[i].fullName) == 0)
	    strcpy (Langbuffer, ISO639table[i].code);
   if (Langbuffer[0] == EOS)
      for (i = 0; Langbuffer[0] == EOS; i++)
         if (OldLangTable[i].fullName[0] == EOS)
	    break;
         else
            if (strcasecmp (name, OldLangTable[i].fullName) == 0)
	       strcpy (Langbuffer, OldLangTable[i].code);
   return Langbuffer;
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
	TypoLangTable[i].LangNom[0] = EOS;
	TypoLangTable[i].LangCode[0] = EOS;
	TypoLangTable[i].LangAlphabet = 'L';
	for (j = 0; j < MAX_DICTS; j++)
	   TypoLangTable[i].LangDict[j] = NULL;
	TypoLangTable[i].LangPattern[0] = EOS;
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

   /* Initialization of remaining entries of the language table */
   for (i = 0; i < MAX_LANGUAGES; i++)
     {
	LangTable[i].LangNom[0] = EOS;
	LangTable[i].LangCode[0] = EOS;
	LangTable[i].LangAlphabet = 'L';
	for (j = 0; j < MAX_DICTS; j++)
	   LangTable[i].LangDict[j] = NULL;
	LangTable[i].LangPattern[0] = EOS;
	LangTable[i].LangTabPattern.Charge = 0;
     }
   /* Loading the default system languages */
   strcpy (LangTable[0].LangNom, "ISO_latin_1");
   strcpy (LangTable[0].LangCode, "x-Latin1");
   LangTable[0].LangAlphabet = 'L';
   strcpy (LangTable[0].LangPrincipal, "Usigle");
   strcpy (LangTable[0].LangSecondary, "Uname");

   strcpy (LangTable[1].LangNom, "ISO_latin_2");
   strcpy (LangTable[1].LangCode, "x-Latin2");
   LangTable[1].LangAlphabet = 'E';
   LangTable[1].LangPrincipal[0] = EOS;
   LangTable[1].LangSecondary[0] = EOS;

   strcpy (LangTable[2].LangNom, "Symbol");
   strcpy (LangTable[2].LangCode, "x-Symbol");
   LangTable[2].LangAlphabet = 'G';
   LangTable[2].LangPrincipal[0] = EOS;
   LangTable[2].LangSecondary[0] = EOS;
   /* Loading the default user languages */
   strcpy (LangTable[3].LangNom, "French");
   strcpy (LangTable[3].LangCode, "fr");
   LangTable[3].LangAlphabet = 'L';
   strcpy (LangTable[3].LangPrincipal, "Fprinc");
   strcpy (LangTable[3].LangSecondary, "Fperso");
   strcpy (LangTable[3].LangPattern, "francais.ptn");

   strcpy (LangTable[4].LangNom, "English");
   strcpy (LangTable[4].LangCode, "en");
   LangTable[4].LangAlphabet = 'L';
   strcpy (LangTable[4].LangPrincipal, "Eprinc");
   strcpy (LangTable[4].LangSecondary, "Eperso");
   strcpy (LangTable[4].LangPattern, "english.ptn");

   strcpy (LangTable[5].LangNom, "American");
   strcpy (LangTable[5].LangCode, "en-US");
   LangTable[5].LangAlphabet = 'L';
   strcpy (LangTable[5].LangPrincipal, "Eprinc");
   strcpy (LangTable[5].LangSecondary, "Eperso");
   strcpy (LangTable[5].LangPattern, "american.ptn");

   strcpy (LangTable[6].LangNom, "German");
   strcpy (LangTable[6].LangCode, "de");
   LangTable[6].LangAlphabet = 'L';
   strcpy (LangTable[6].LangPrincipal, "Gprinc");
   LangTable[6].LangSecondary[0] = EOS;
   strcpy (LangTable[6].LangPattern, "deutsch.ptn");

   strcpy (LangTable[7].LangNom, "Italian");
   strcpy (LangTable[7].LangCode, "it");
   LangTable[7].LangAlphabet = 'L';
   strcpy (LangTable[7].LangPrincipal, "Iprinc");
   LangTable[7].LangSecondary[0] = EOS;
   strcpy (LangTable[7].LangPattern, "italiano.ptn");

   strcpy (LangTable[8].LangNom, "Spanish");
   strcpy (LangTable[8].LangCode, "es");
   LangTable[8].LangAlphabet = 'L';
   strcpy (LangTable[8].LangPrincipal, "Sprinc");
   LangTable[8].LangSecondary[0] = EOS;
   strcpy (LangTable[8].LangPattern, "espanol.ptn");

   strcpy (LangTable[9].LangNom, "Portuguese");
   strcpy (LangTable[9].LangCode, "pt");
   LangTable[9].LangAlphabet = 'L';
   LangTable[9].LangPrincipal[0] = EOS;
   LangTable[9].LangSecondary[0] = EOS;
   strcpy (LangTable[9].LangPattern, "portug.ptn");

   strcpy (LangTable[10].LangNom, "Dutch");
   strcpy (LangTable[10].LangCode, "nl");
   LangTable[10].LangAlphabet = 'L';
   strcpy (LangTable[10].LangPrincipal, "Nprinc");
   LangTable[10].LangSecondary[0] = EOS;
   strcpy (LangTable[10].LangPattern, "nederl.ptn");

   strcpy (LangTable[11].LangNom, "Swedish");
   strcpy (LangTable[11].LangCode, "sv");
   LangTable[11].LangAlphabet = 'L';
   strcpy (LangTable[11].LangPrincipal, "Wprinc");
   LangTable[11].LangSecondary[0] = EOS;
   strcpy (LangTable[11].LangPattern, "swedish.ptn");

   strcpy (LangTable[12].LangNom, "Finnish");
   strcpy (LangTable[12].LangCode, "fi");
   LangTable[12].LangAlphabet = 'L';
   LangTable[12].LangPrincipal[0] = EOS;
   LangTable[12].LangSecondary[0] = EOS;
   strcpy (LangTable[12].LangPattern, "finish.ptn");

   strcpy (LangTable[13].LangNom, "Greek");
   strcpy (LangTable[13].LangCode, "el");
   LangTable[13].LangAlphabet = 'G';
   LangTable[13].LangPrincipal[0] = EOS;
   LangTable[13].LangSecondary[0] = EOS;

   strcpy (LangTable[14].LangNom, "Czech");
   strcpy (LangTable[14].LangCode, "cs");
   LangTable[14].LangAlphabet = 'E';
   LangTable[14].LangPrincipal[0] = EOS;
   LangTable[14].LangSecondary[0] = EOS;

   strcpy (LangTable[15].LangNom, "Polish");
   strcpy (LangTable[15].LangCode, "pl");
   LangTable[15].LangAlphabet = 'E';
   LangTable[15].LangPrincipal[0] = EOS;
   LangTable[15].LangSecondary[0] = EOS;

   FreeEntry = 16;
   FirstUserLang = 3;

   for (i = 1; i < FreeEntry; i++)
       strcpy (LangTable[i].LangNom, TtaGetLanguageNameFromCode(LangTable[i].LangCode));

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
   else if (languageName[0] == EOS)
      TtaError (ERR_invalid_parameter);
   else if (strlen (languageName) >= MAX_NAME_LENGTH)
      TtaError (ERR_string_too_long);
   else if (FreeEntry == MAX_LANGUAGES)
      TtaError (ERR_too_many_languages);
   else
     {
	if (strlen(languageName) == 2 ||
	    languageName[1] == '-' || languageName[2] == '-')
	   /* it's an ISO-639 code */
	  {
	  for (i = 0; i < FreeEntry; i++)
	    if (!strcasecmp (languageName, LangTable[i].LangCode))
	      /* The language is already defined */
	      return i;
	  strcpy (LangTable[FreeEntry].LangCode, languageName);
	  strcpy (LangTable[FreeEntry].LangNom, TtaGetLanguageCodeFromName(languageName));
	  }
	else
	  {
	  /* Consults the languages table to see if the language exists. */
	  for (i = 0; i < FreeEntry; i++)
	    if (!strcasecmp (languageName, LangTable[i].LangNom))
	      /* The language is already defined */
	      return i;
	  strcpy (LangTable[FreeEntry].LangNom, languageName);
	  strcpy (LangTable[FreeEntry].LangCode, TtaGetLanguageNameFromCode (languageName));
	  }

	/* Saves the new language */
	i = FreeEntry;
	LangTable[i].LangAlphabet = languageAlphabet;
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
      LangTable[i].LangNom[0] = EOS;
      LangTable[i].LangCode[0] = EOS;
      /* don't erase LangAlphabet */
      LangTable[i].LangPrincipal[0] = EOS;
      LangTable[i].LangSecondary[0] = EOS;
      LangTable[i].LangPattern[0] = EOS;
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
Language            TtaGetLanguageIdFromName (char *languageName)
#else  /* __STDC__ */
Language            TtaGetLanguageIdFromName (languageName)
char               *languageName;
#endif /* __STDC__ */

{
   int                 i;
   boolean             again;
   aLangName	       code;

   i = 0;
   /* Avoids error cases */
   if (languageName == NULL)
      TtaError (ERR_invalid_parameter);
   else if (languageName[0] == EOS)
      TtaError (ERR_invalid_parameter);
   else if (strlen (languageName) >= MAX_NAME_LENGTH)
      TtaError (ERR_string_too_long);
   else
     {
	/* Consults the languages table to see if the language exists. */
	again = TRUE;
	while (again && i < FreeEntry)
	  {
	     if (!strcasecmp (languageName, LangTable[i].LangCode))
		/* The language is already defined */
		again = FALSE;
	     else
		if (!strcasecmp (languageName, LangTable[i].LangNom))
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
		  if (!strcmp (languageName, TypoLangTable[i].LangNom))
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
char               *TtaGetVarLANG ()
{
   char               *name;

   name = TtaGetEnvString ("LANG");
   if (name == NULL)
      strcpy (StandardLANG, "en");
   else if (!strcmp (name, "C") || !strcasecmp (name, "iso_8859_1"))
      strcpy (StandardLANG, "fr");
   else
     {
	strncpy (StandardLANG, name, 2);
	StandardLANG[2] = EOS;
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

   name = TtaGetVarLANG ();
   return TtaGetLanguageIdFromName (name);
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
   if (languageAlphabet != 'L' && languageAlphabet != 'G' && languageAlphabet != 'E')
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
	     /* The language does not exist */
	     i = 0;
	     TtaError (ERR_language_not_found);
	  }
     }
   return (Language) i;
}


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
   if (i >= FreeEntry || i < 0)
     {
	TtaError (ERR_language_not_found);
	return (EOS);
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
   if (i >= FreeEntry || i < 0)
     {
	TtaError (ERR_language_not_found);
	Langbuffer[0] = EOS;
     }
   else
      strcpy (Langbuffer, LangTable[i].LangNom);
   return Langbuffer;
}				/*TtaGetLanguageName */


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
char               *TtaGetLanguageCode (Language languageId)

#else  /* __STDC__ */
char               *TtaGetLanguageCode (languageId)
Language            languageId;

#endif /* __STDC__ */

{
   int                 i;

   i = (int) languageId;
   if (i >= FreeEntry || i < 0)
     {
	TtaError (ERR_language_not_found);
	Langbuffer[0] = EOS;
     }
   else
      strcpy (Langbuffer, LangTable[i].LangCode);
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
   char                patternFileName[THOT_MAX_CHAR];
   static char        *dictPath;	/* Environment variable DICOPAR */
   char               *ptPattern;
   FILE               *in;

   dictPath = TtaGetEnvString ("DICOPAR");
   if (dictPath == NULL)
     {
	/* The environment variable DICOPAR does not exist */
	TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_MISSING_DICOPAR), "DICOPAR");
	return (FALSE);
     }

   strcpy (patternFileName, dictPath);
   strcat (patternFileName, DIR_STR);
   lang = (int) langageId;
   ptPattern = LangTable[lang].LangPattern;
   strcat (patternFileName, ptPattern);
   if ((in = fopen (patternFileName, "r")) == NULL)
     {
	TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_HYPHEN_FILE_NOT_OPEN), LangTable[lang].LangPattern);
	return (FALSE);
     }
   TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_HYPHEN_FILE_OPEN), ptPattern);
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
}


/*----------------------------------------------------------------------
  FoundPatternInList verifies if a string belongs to the pattern list.
  if true, it returns 1 else 0 
  ----------------------------------------------------------------------*/
#ifdef __STDC
static char        *FoundPatternInList (Language langageId, unsigned char substring[MAX_LET_PATTERN])
#else  /* __STDC__ */
static char        *FoundPatternInList (langageId, substring)
Language            langageId;
unsigned char       substring[MAX_LET_PATTERN];

#endif /* __STDC__ */
{
   int                 language;
   int                 lgstring;
   int                 i, max;
   struct PatternList *ptrTabPattern;

   language = (int) langageId;
   lgstring = strlen (substring);
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
	       if (!strcmp (ptrTabPattern->liste_pattern[i].CarPattern, substring))
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
static void         FoundHyphenPoints (Language langageId, char wordToCut[THOT_MAX_CHAR])
#else  /* __STDC__ */
static void         FoundHyphenPoints (langageId, wordToCut)
Language            langageId;
char                wordToCut[THOT_MAX_CHAR];

#endif /* __STDC__ */
{
   int                 lang;
   unsigned char       wordToTreat[THOT_MAX_CHAR];	/* "." + wordToCut + "." */
   unsigned char       subword[THOT_MAX_CHAR];
   int                 tab_weight[THOT_MAX_CHAR];
   char               *weight_subword;
   int                 wordLength;
   int                 size_subword;
   int                 currentPosition;
   int                 i, j;

   lang = (int) langageId;
   wordLength = strlen (wordToCut) + 2;
   if (wordLength > THOT_MAX_CHAR)
     {
	TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_HYPHEN_WORD_TOO_LONG), wordToCut);
	return;
     }
   for (i = 0; i < THOT_MAX_CHAR; i++)
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


/*----------------------------------------------------------------------
 TtaGetPatternHyphenList 
   returns a pointer on the list of values representing the hyphen points
   or NULL 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                *TtaGetPatternHyphenList (char word[THOT_MAX_CHAR], Language languageId)
#else  /* __STDC__ */
int                *TtaGetPatternHyphenList (word, languageId)
char                word[THOT_MAX_CHAR];
Language            languageId;

#endif /* __STDC__ */
{
   int                 language;
   int                 i;

   language = (int) languageId;
   if (word[0] == EOS)
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
 * TtaExistPatternList verifies if a list of patterns is defined
 * for a given language
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             TtaExistPatternList (Language languageId)
#else  /* __STDC__ */
boolean             TtaExistPatternList (languageId)
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
