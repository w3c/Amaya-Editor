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

#include "uisoentries.h"

/* Variables recuperees de la gestion des dictionaires */
struct Langue_Ctl   LangTable[MAX_LANGUAGES];
struct Langue_Ctl   TypoLangTable[MAX_LANGUAGES];
int                 FreeEntry;
int                 FreeTypoEntry;
int		    FirstUserLang;
static CHAR_T       Langbuffer[2 * MAX_NAME_LENGTH];
static int          breakPoints[MAX_POINT_COUP];
static CHAR_T       StandardLANG[3];

#include "thotmsg_f.h"


/*	ISO 639 CODES ALPHABETIC BY LANGUAGE NAME (ENGLISH SPELLING) */

typedef UCHAR_T aLangName[16];
typedef struct _ISO639entry
  {
     aLangName	fullName;
     aLangName	code;
  }
ISO639entry;

static ISO639entry	ISO639table[] =
{
	{_ABKHAZIAN_,  _ABKHAZIAN_ISO},
	{_AFANOROMO_,    _AFANOROMO_ISO},
	{_AFAR_,         _AFAR_ISO},
	{_AFRIKAANS_,    _AFRIKAANS_ISO},
	{_ALBANIAN_,     _ALBANIAN_ISO},
	{_AMHARIC_,      _AMHARIC_ISO},
	{_ARABIC_,       _ARABIC_ISO},
	{_ARMENIAN_,     _ARMENIAN_ISO},
	{_ASSAMESE_,     _ASSAMESE_ISO},
	{_AYMARA_,       _AYMARA_ISO},
	{_AZERBAIJANI_,  _AZERBAIJANI_ISO},
	{_BASHKIR_,      _BASHKIR_ISO},
	{_BASQUE_,       _BASQUE_ISO},
	{_BENGALI_,      _BENGALI_ISO},
	{_BANGLA_,       _BANGLA_ISO},
	{_BHUTANI_,      _BHUTANI_ISO},
	{_BIHARI_,       _BIHARI_ISO},
	{_BISLAMA_,      _BISLAMA_ISO},
	{_BRETON_,       _BRETON_ISO},
	{_BULGARIAN_,    _BULGARIAN_ISO},
	{_BURMESE_,      _BURMESE_ISO},
	{_BYELORUSSIAN_, _BYELORUSSIAN_ISO},
	{_CAMBODIAN_,    _CAMBODIAN_ISO},
	{_CATALAN_,      _CATALAN_ISO},
	{_CHINESE_,      _CHINESE_ISO},
	{_CORSICAN_,     _CORSICAN_ISO},
	{_CROATIAN_,     _CROATIAN_ISO},
	{_Czech_,        _Czech_ISO},
	{_DANISH_,       _DANISH_ISO},
	{_Dutch_,        _Dutch_ISO},
	{_English_,      _English_ISO},
	{_ESPERANTO_,    _ESPERANTO_ISO},
	{_ESTONIAN_,     _ESTONIAN_ISO},
	{_FAROESE_,      _FAROESE_ISO},
	{_FIJI_,         _FIJI_ISO},
	{_Finnish_,      _Finnish_ISO},
	{_French_,       _French_ISO},
	{_FRISIAN_,      _FRISIAN_ISO},
	{_GALICIAN_,     _GALICIAN_ISO},
	{_GEORGIAN_,     _GEORGIAN_ISO},
	{_German_,       _German_ISO},
	{_Greek_,        _Greek_ISO},
	{_GREENLANDIC_,  _GREENLANDIC_ISO},
	{_GUARANI_,      _GUARANI_ISO},
	{_GUJARATI_,     _GUJARATI_ISO},
	{_HAUSA_,        _HAUSA_ISO},
	{_HEBREW_,       _HEBREW_ISO},
	{_HINDI_,        _HINDI_ISO},
	{_HUNGARIAN_,    _HUNGARIAN_ISO},
	{_ICELANDIC_,    _ICELANDIC_ISO},
	{_INDONESIAN_,   _INDONESIAN_ISO},
	{_INTERLINGUA_,  _INTERLINGUA_ISO},
	{_INTERLINGUE_,  _INTERLINGUE_ISO},
	{_INUKTITUT_,    _INUKTITUT_ISO},
	{_INUPIAK_,      _INUPIAK_ISO},
	{_IRISH_,        _IRISH_ISO},
	{_Italian_,      _Italian_ISO},
	{_JAPANESE_,     _JAPANESE_ISO},
	{_JAVANESE_,     _JAVANESE_ISO},
	{_KANNADA_,      _KANNADA_ISO},
	{_KASHMIRI_,     _KASHMIRI_ISO},
	{_KAZAKH_,       _KAZAKH_ISO},
	{_KINYARWANDA_,  _KINYARWANDA_ISO},
	{_KIRGHIZ_,      _KIRGHIZ_ISO},
	{_KURUNDI_,      _KURUNDI_ISO},
	{_KOREAN_,       _KOREAN_ISO},
	{_KURDISH_,      _KURDISH_ISO},
	{_LAOTHIAN_,     _LAOTHIAN_ISO},
	{_LATIN_,        _LATIN_ISO},
	{_LATVIAN_,      _LATVIAN_ISO},
	{_LETTISH_,      _LETTISH_ISO},
	{_LINGALA_,      _LINGALA_ISO},
	{_LITHUANIAN_,   _LITHUANIAN_ISO},
	{_MACEDONIAN_,   _MACEDONIAN_ISO},
	{_MALAGASY_,     _MALAGASY_ISO},
	{_MALAY_,        _MALAY_ISO},
	{_MALAYALAM_,    _MALAYALAM_ISO},
	{_MALTESE_,      _MALTESE_ISO},
	{_MAORI_,        _MAORI_ISO},
	{_MARATHI_,      _MARATHI_ISO},
	{_MOLDAVIAN_,    _MOLDAVIAN_ISO},
	{_MONGOLIAN_,    _MONGOLIAN_ISO},
	{_NAURU_,        _NAURU_ISO},
	{_NEPALI_,       _NEPALI_ISO},
	{_NORWEGIAN_,    _NORWEGIAN_ISO},
	{_OCCITAN_,      _OCCITAN_ISO},
	{_ORIYA_,        _ORIYA_ISO},
	{_PASHTO_,       _PASHTO_ISO},
	{_PUSHTO_,       _PUSHTO_ISO},
	{_PERSIAN_,      _PERSIAN_ISO},
	{_Polish_,       _Polish_ISO},
	{_Portuguese_,   _Portuguese_ISO},
	{_PUNJABI_,      _PUNJABI_ISO},
	{_QUECHUA_,      _QUECHUA_ISO},
	{_RHAETOROMANCE_,_RHAETOROMANCE_ISO},
	{_ROMANIAN_,     _ROMANIAN_ISO},
	{_RUSSIAN_,      _RUSSIAN_ISO},
	{_SAMOAN_,       _SAMOAN_ISO},
	{_SANGHO_,       _SANGHO_ISO},
	{_SANSKRIT_,     _SANSKRIT_ISO},
	{_SCOTSGAELIC_,  _SCOTSGAELIC_ISO},
	{_SERBIAN_,      _SERBIAN_ISO},
	{_SERBOCROATIAN_,_SERBOCROATIAN_ISO},
	{_SESOTHO_,      _SESOTHO_ISO},
	{_SETSWANA_,     _SETSWANA_ISO},
	{_SHONA_,        _SHONA_ISO},
	{_SINDHI_,       _SINDHI_ISO},
	{_SINGHALESE_,   _SINGHALESE_ISO},
	{_SISWATI_,      _SISWATI_ISO},
	{_SLOVAK_,       _SLOVAK_ISO},
	{_SLOVENIAN_,    _SLOVENIAN_ISO},
	{_SOMALI_,       _SOMALI_ISO},
	{_Spanish_,      _Spanish_ISO},
	{_SUNDANESE_,    _SUNDANESE_ISO},
	{_SWAHILI_,      _SWAHILI_ISO},
	{_Swedish_,      _Swedish_ISO},
	{_TAGALOG_,      _TAGALOG_ISO},
	{_TAJIK_,        _TAJIK_ISO},
	{_TAMIL_,        _TAMIL_ISO},
	{_TATAR_,        _TATAR_ISO},
	{_TELUGU_,       _TELUGU_ISO},
	{_THAI_,         _THAI_ISO},
	{_TIBETAN_,      _TIBETAN_ISO},
	{_TIGRINYA_,     _TIGRINYA_ISO},
	{_TONGA_,        _TONGA_ISO},
	{_TSONGA_,       _TSONGA_ISO},
	{_TURKISH_,      _TURKISH_ISO},
	{_TURKMEN_,      _TURKMEN_ISO},
	{_TWI_,          _TWI_ISO},
	{_UIGUR_,        _UIGUR_ISO},
	{_UKRAINIAN_,    _UKRAINIAN_ISO},
	{_URDU_,         _URDU_ISO},
	{_UZBEK_,        _UZBEK_ISO},
	{_VIETNAMESE_,   _VIETNAMESE_ISO},
	{_VOLAPUK_,      _VOLAPUK_ISO },
	{_WELSH_,        _WELSH_ISO},
	{_WOLOF_,        _WOLOF_ISO},
	{_XHOSA_,        _XHOSA_ISO},
	{_YIDDISH_,      _YIDDISH_ISO},
	{_YORUBA_,       _YORUBA_ISO},
	{_ZHUANG_,       _ZHUANG_ISO},
	{ _ZULU_,        _ZULU_ISO},
	{_EMPTY_,        _EMPTY_}
};

/* this table associates the ancient language names used in Thot documents
   with their standard code */
static ISO639entry	OldLangTable[] =
{
	{_American_,    _American_ISO},
	{_Deutsch_,     _Deutsch_ISO},
	{_Espanol_,     _Espanol_ISO},
	{_Francais_,    _Francais_ISO},
	{_ISOLatin1_,   _ISOLatin1_ISO},
	{_ISOLatin2_,   _ISOLatin2_ISO},
	{_Italiano_,    _Italiano_ISO},
	{_Symbol_,      _Symbol_ISO},
	{_EMPTY_,       _EMPTY_}
};


/*----------------------------------------------------------------------
   TtaGetLanguageNameFromCode
   Returns the full name of a language whose RFC-1766 code is known
  ----------------------------------------------------------------------*/
#ifdef __STDC__
STRING          TtaGetLanguageNameFromCode (STRING code)
#else  /* __STDC__ */
STRING          TtaGetLanguageNameFromCode (code)
STRING          code;
#endif /* __STDC__ */

{
   int                 i;

   Langbuffer[0] = EOS;
   for (i = 0; Langbuffer[0] == EOS; i++)
      if (ISO639table[i].code[0] == EOS)
	    break;
      else if (ustrcasecmp (code, ISO639table[i].code) == 0)
	 ustrcpy (Langbuffer, ISO639table[i].fullName);
   if (Langbuffer[0] == EOS)
      for (i = 0; Langbuffer[0] == EOS; i++)
         if (OldLangTable[i].code[0] == EOS)
	    break;
         else
            if (ustrcasecmp (code, OldLangTable[i].code) == 0)
	       ustrcpy (Langbuffer, OldLangTable[i].fullName);
   return Langbuffer;
}


/*----------------------------------------------------------------------
   TtaGetLanguageCodeFromName
   Returns the RFC-1766 code for a language whose name is known
  ----------------------------------------------------------------------*/
#ifdef __STDC__
STRING          TtaGetLanguageCodeFromName (STRING name)
#else  /* __STDC__ */
STRING          TtaGetLanguageCodeFromName (name)
STRING          name;
#endif /* __STDC__ */

{
   int                 i;

   Langbuffer[0] = EOS;
   for (i = 0; Langbuffer[0] == EOS; i++)
      if (ISO639table[i].fullName[0] == EOS)
	 break;
      else
         if (ustrcasecmp (name, ISO639table[i].fullName) == 0)
	    ustrcpy (Langbuffer, ISO639table[i].code);
   if (Langbuffer[0] == EOS)
      for (i = 0; Langbuffer[0] == EOS; i++)
         if (OldLangTable[i].fullName[0] == EOS)
	    break;
         else
            if (ustrcasecmp (name, OldLangTable[i].fullName) == 0)
	       ustrcpy (Langbuffer, OldLangTable[i].code);
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
	TypoLangTable[i].LangAlphabet = TEXT('L');
	for (j = 0; j < MAX_DICTS; j++)
	   TypoLangTable[i].LangDict[j] = NULL;
	TypoLangTable[i].LangPattern[0] = EOS;
	TypoLangTable[i].LangTabPattern.Charge = 0;
     }
   ustrcpy (TypoLangTable[0].LangNom, TEXT("F-Short"));
   TypoLangTable[0].LangAlphabet = TEXT('L');
   ustrcpy (TypoLangTable[0].LangPrincipal, TEXT("Fabbrev"));
   ustrcpy (TypoLangTable[0].LangSecondary, TEXT("Ftrabbrev"));

   ustrcpy (TypoLangTable[1].LangNom, TEXT("E-Short"));
   TypoLangTable[1].LangAlphabet = TEXT('L');
   ustrcpy (TypoLangTable[1].LangPrincipal, TEXT("Eabbrev"));
   ustrcpy (TypoLangTable[1].LangSecondary, TEXT("Etrabbrev"));

   ustrcpy (TypoLangTable[2].LangNom, TEXT("F-Acronym"));
   TypoLangTable[2].LangAlphabet = TEXT('L');
   ustrcpy (TypoLangTable[2].LangPrincipal, TEXT("Facronym"));
   ustrcpy (TypoLangTable[2].LangSecondary, TEXT("Ftracronym"));

   ustrcpy (TypoLangTable[3].LangNom, TEXT("E-Acronym"));
   TypoLangTable[3].LangAlphabet = TEXT('L');
   ustrcpy (TypoLangTable[3].LangPrincipal, TEXT("Eacronym"));
   ustrcpy (TypoLangTable[3].LangSecondary, TEXT("Etracronym"));

   ustrcpy (TypoLangTable[4].LangNom, TEXT("F-Expo1"));
   TypoLangTable[4].LangAlphabet = TEXT('L');
   ustrcpy (TypoLangTable[4].LangPrincipal, TEXT("Fexpo"));
   ustrcpy (TypoLangTable[4].LangSecondary, TEXT("Ftrexpo"));

   ustrcpy (TypoLangTable[5].LangNom, TEXT("F-Expo2"));
   TypoLangTable[5].LangAlphabet = TEXT('L');
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
	LangTable[i].LangNom[0] = EOS;
	LangTable[i].LangCode[0] = EOS;
	LangTable[i].LangAlphabet = TEXT('L');
	for (j = 0; j < MAX_DICTS; j++)
	   LangTable[i].LangDict[j] = NULL;
	LangTable[i].LangPattern[0] = EOS;
	LangTable[i].LangTabPattern.Charge = 0;
     }
   /* Loading the default system languages */
   i = 0;
   ustrcpy (LangTable[i].LangNom, TEXT("ISO_latin_1"));
   ustrcpy (LangTable[i].LangCode, "x-Latin1");
   LangTable[i].LangAlphabet = TEXT('L');
   ustrcpy (LangTable[i].LangPrincipal, TEXT("Usigle"));
   ustrcpy (LangTable[i].LangSecondary, TEXT("Uname"));

   i = 1;
   ustrcpy (LangTable[i].LangNom, TEXT("ISO_latin_2"));
   ustrcpy (LangTable[i].LangCode, TEXT("x-Latin2"));
   LangTable[i].LangAlphabet = TEXT('2');
   LangTable[i].LangPrincipal[0] = EOS;
   LangTable[i].LangSecondary[0] = EOS;

   i = 2;
   ustrcpy (LangTable[i].LangNom, TEXT("ISO_latin_9"));
   ustrcpy (LangTable[i].LangCode, "x-Latin9");
   LangTable[i].LangAlphabet = TEXT('9');
   LangTable[i].LangPrincipal[0] = EOS;
   LangTable[i].LangSecondary[0] = EOS;

   i = 3;
   ustrcpy (LangTable[i].LangNom, TEXT("Symbol"));
   ustrcpy (LangTable[i].LangCode, TEXT("x-Symbol"));
   LangTable[i].LangAlphabet = TEXT('G');
   LangTable[i].LangPrincipal[0] = EOS;
   LangTable[i].LangSecondary[0] = EOS;

   /* Loading the default user languages */
   FirstUserLang = 4;
   i = 4;
   ustrcpy (LangTable[i].LangNom, TEXT("French"));
   ustrcpy (LangTable[i].LangCode, "fr");
   LangTable[i].LangAlphabet = TEXT('L');
   ustrcpy (LangTable[i].LangPrincipal, TEXT("Fprinc"));
   ustrcpy (LangTable[i].LangSecondary, TEXT("Fperso"));
   ustrcpy (LangTable[i].LangPattern, TEXT("francais.ptn"));

   i = 5;
   ustrcpy (LangTable[i].LangNom, TEXT("English"));
   ustrcpy (LangTable[i].LangCode, "en");
   LangTable[i].LangAlphabet = TEXT('L');
   ustrcpy (LangTable[i].LangPrincipal, TEXT("Eprinc"));
   ustrcpy (LangTable[i].LangSecondary, TEXT("Eperso"));
   ustrcpy (LangTable[i].LangPattern, TEXT("english.ptn"));

   i = 6;
   ustrcpy (LangTable[i].LangNom, TEXT("American"));
   ustrcpy (LangTable[i].LangCode, "en-US");
   LangTable[i].LangAlphabet = TEXT('L');
   ustrcpy (LangTable[i].LangPrincipal, TEXT("Eprinc"));
   ustrcpy (LangTable[i].LangSecondary, TEXT("Eperso"));
   ustrcpy (LangTable[i].LangPattern, TEXT("american.ptn"));

   i = 7;
   ustrcpy (LangTable[i].LangNom, TEXT("German"));
   ustrcpy (LangTable[i].LangCode, "de");
   LangTable[i].LangAlphabet = TEXT('L');
   ustrcpy (LangTable[i].LangPrincipal, TEXT("Gprinc"));
   LangTable[i].LangSecondary[0] = EOS;
   ustrcpy (LangTable[i].LangPattern, TEXT("deutsch.ptn"));

   i = 8;
   ustrcpy (LangTable[i].LangNom, TEXT("Italian"));
   ustrcpy (LangTable[i].LangCode, "it");
   LangTable[i].LangAlphabet = TEXT('L');
   ustrcpy (LangTable[i].LangPrincipal, TEXT("Iprinc"));
   LangTable[i].LangSecondary[0] = EOS;
   ustrcpy (LangTable[i].LangPattern, TEXT("italiano.ptn"));

   i = 9;
   ustrcpy (LangTable[i].LangNom, TEXT("Spanish"));
   ustrcpy (LangTable[i].LangCode, "es");
   LangTable[i].LangAlphabet = TEXT('L');
   ustrcpy (LangTable[i].LangPrincipal, TEXT("Sprinc"));
   LangTable[i].LangSecondary[0] = EOS;
   ustrcpy (LangTable[i].LangPattern, TEXT("espanol.ptn"));

   i = 10;
   ustrcpy (LangTable[i].LangNom, TEXT("Portuguese"));
   ustrcpy (LangTable[i].LangCode, "pt");
   LangTable[i].LangAlphabet = TEXT('L');
   LangTable[i].LangPrincipal[0] = EOS;
   LangTable[i].LangSecondary[0] = EOS;
   ustrcpy (LangTable[i].LangPattern, TEXT("portug.ptn"));

   i = 11;
   ustrcpy (LangTable[i].LangNom, TEXT("Dutch"));
   ustrcpy (LangTable[i].LangCode, "nl");
   LangTable[i].LangAlphabet = TEXT('L');
   ustrcpy (LangTable[i].LangPrincipal, TEXT("Nprinc"));
   LangTable[i].LangSecondary[0] = EOS;
   ustrcpy (LangTable[i].LangPattern, TEXT("nederl.ptn"));

   i = 12;
   ustrcpy (LangTable[i].LangNom, TEXT("Swedish"));
   ustrcpy (LangTable[i].LangCode, "sv");
   LangTable[i].LangAlphabet = TEXT('L');
   ustrcpy (LangTable[i].LangPrincipal, TEXT("Wprinc"));
   LangTable[i].LangSecondary[0] = EOS;
   ustrcpy (LangTable[i].LangPattern, TEXT("swedish.ptn"));

   i = 13;
   ustrcpy (LangTable[i].LangNom, TEXT("Finnish"));
   ustrcpy (LangTable[i].LangCode, "fi");
   LangTable[i].LangAlphabet = TEXT('L');
   LangTable[i].LangPrincipal[0] = EOS;
   LangTable[i].LangSecondary[0] = EOS;
   ustrcpy (LangTable[i].LangPattern, TEXT("finish.ptn"));

   i = 14;
   ustrcpy (LangTable[i].LangNom, TEXT("Greek"));
   ustrcpy (LangTable[i].LangCode, "el");
   LangTable[i].LangAlphabet = TEXT('G');
   LangTable[i].LangPrincipal[0] = EOS;
   LangTable[i].LangSecondary[0] = EOS;

   i = 15;
   ustrcpy (LangTable[i].LangNom, TEXT("Czech"));
   ustrcpy (LangTable[i].LangCode, "cs");
   LangTable[i].LangAlphabet = TEXT('2');
   LangTable[i].LangPrincipal[0] = EOS;
   LangTable[i].LangSecondary[0] = EOS;

   i = 17;
   ustrcpy (LangTable[i].LangNom, TEXT("Polish"));
   ustrcpy (LangTable[i].LangCode, "pl");
   LangTable[i].LangAlphabet = TEXT('2');
   LangTable[i].LangPrincipal[0] = EOS;
   LangTable[i].LangSecondary[0] = EOS;

   i = 18;
   ustrcpy (LangTable[i].LangNom, TEXT("Turkish"));
   ustrcpy (LangTable[i].LangCode, "tr");
   LangTable[i].LangAlphabet = TEXT('9');
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
Language            TtaNewLanguage (STRING languageName, CHAR_T languageAlphabet, STRING principalDictionary, STRING secondDictionary)
#else  /* __STDC__ */
Language            TtaNewLanguage (languageName, languageAlphabet, principalDictionary, secondDictionary)
STRING              languageName;
CHAR_T              languageAlphabet;
STRING              principalDictionary;
STRING              secondDictionary;
#endif /* __STDC__ */

{
   int                 i;

   i = 0;
   /* Avoids error cases */
   if (languageName == NULL)
      TtaError (ERR_invalid_parameter);
   else if (languageName[0] == EOS)
      TtaError (ERR_invalid_parameter);
   else if (ustrlen (languageName) >= MAX_NAME_LENGTH)
      TtaError (ERR_string_too_long);
   else if (FreeEntry == MAX_LANGUAGES)
      TtaError (ERR_too_many_languages);
   else
     {
	if (ustrlen(languageName) == 2 ||
	    languageName[1] == TEXT('-') || languageName[2] == TEXT('-'))
	   /* it's an ISO-639 code */
	  {
	  for (i = 0; i < FreeEntry; i++)
	    if (!ustrcasecmp (languageName, LangTable[i].LangCode))
	      /* The language is already defined */
	      return i;
	  ustrcpy (LangTable[FreeEntry].LangCode, languageName);
	  ustrcpy (LangTable[FreeEntry].LangNom, TtaGetLanguageCodeFromName(languageName));
	  }
	else
	  {
	  /* Consults the languages table to see if the language exists. */
	  for (i = 0; i < FreeEntry; i++)
	    if (!ustrcasecmp (languageName, LangTable[i].LangNom))
	      /* The language is already defined */
	      return i;
	  ustrcpy (LangTable[FreeEntry].LangNom, languageName);
	  ustrcpy (LangTable[FreeEntry].LangCode, TtaGetLanguageNameFromCode (languageName));
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
Language            TtaGetLanguageIdFromName (char* languageName)
#else  /* __STDC__ */
Language            TtaGetLanguageIdFromName (languageName)
char*               languageName;
#endif /* __STDC__ */

{
   int                 i;
   ThotBool            again;
   aLangName	       code;

   i = 0;
   /* Avoids error cases */
   if (languageName == NULL)
      TtaError (ERR_invalid_parameter);
   else if (languageName[0] == EOS)
      TtaError (ERR_invalid_parameter);
   else if (ustrlen (languageName) >= MAX_NAME_LENGTH)
      TtaError (ERR_string_too_long);
   else
     {
	/* Consults the languages table to see if the language exists. */
	again = TRUE;
	while (again && i < FreeEntry)
	  {
	     if (!ustrcasecmp (languageName, LangTable[i].LangCode))
		/* The language is already defined */
		again = FALSE;
	     else
		if (!ustrcasecmp (languageName, LangTable[i].LangNom))
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
		  if (!ustrcmp (languageName, TypoLangTable[i].LangNom))
		     /* The TYPO language is already defined */
		     again = FALSE;
		  else
		     i++;
	       }
	  }
	if (again)
	  {
	  ustrcpy (code, TtaGetLanguageCodeFromName (languageName));
	  if (code[0] != EOS)
	    {
	    i = 0;
	    while (again && i < FreeEntry)
	      if (!ustrcasecmp (code, LangTable[i].LangCode))
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
char*               TtaGetVarLANG ()
{
   char*            name;

   name = TtaGetEnvString ("LANG");
   if (name == NULL)
      ustrcpy (StandardLANG, "en");
   else if (!ustrcmp (name, TEXT("C")) || !ustrcasecmp (name, TEXT("iso_8859_1")))
      ustrcpy (StandardLANG, "fr");
   else
     {
	ustrncpy (StandardLANG, name, 2);
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
   STRING           name;

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
CHAR_T              TtaGetAlphabet (Language languageId)
#else  /* __STDC__ */
CHAR_T              TtaGetAlphabet (languageId)
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
STRING              TtaGetLanguageName (Language languageId)
#else  /* __STDC__ */
STRING              TtaGetLanguageName (languageId)
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
      ustrcpy (Langbuffer, LangTable[i].LangNom);
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
STRING             TtaGetLanguageCode (Language languageId)
#else  /* __STDC__ */
STRING             TtaGetLanguageCode (languageId)
Language           languageId;
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
      ustrcpy (Langbuffer, LangTable[i].LangCode);
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
   static char*        dictPath;	/* Environment variable DICOPAR */
   STRING              ptPattern;
   FILE               *in;

   dictPath = TtaGetEnvString ("DICOPAR");
   if (dictPath == NULL)
     {
	/* The environment variable DICOPAR does not exist */
	TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_MISSING_DICOPAR), "DICOPAR");
	return (FALSE);
     }

   ustrcpy (patternFileName, dictPath);
   ustrcat (patternFileName, DIR_STR);
   lang = (int) langageId;
   ptPattern = LangTable[lang].LangPattern;
   ustrcat (patternFileName, ptPattern);
   if ((in = fopen (patternFileName, "r")) == NULL)
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
