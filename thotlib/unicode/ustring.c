/*
 *
 *  (c) COPYRIGHT INRIA, 1999.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/* Unicode Routines
 * Author: R. Guetari (W3C/INRIA)
 */

#include <stdio.h>
#include <stdlib.h>
#include "thot_sys.h"

#include "ustring_f.h"
 
unsigned long offset[6] = {
         0x00000000UL,
         0x00003080UL,
         0x000E2080UL,
         0x03C82080UL,
         0xFA082080UL,
         0x82082080UL
};

typedef struct {
        CHAR_T  ISOCode [50];
        CHARSET Charset;
} CharsetCode;

static CharsetCode CharsetCodeTable[] =
{
    {TEXT("ANSI_X3.4-1968"),   US_ASCII},
    {TEXT("iso-ir-6"),         US_ASCII},
    {TEXT("ANSI_X3.4-1986"),   US_ASCII},
    {TEXT("ISO_646.irv:1991"), US_ASCII},
    {TEXT("ASCII"),            US_ASCII},
    {TEXT("ISO646-US"),        US_ASCII},
    {TEXT("US-ASCII"),         US_ASCII},
    {TEXT("us"),               US_ASCII},
    {TEXT("IBM367"),           US_ASCII},
    {TEXT("cp367"),            US_ASCII},
    {TEXT("csASCII"),          US_ASCII},

	{TEXT("ISO_8859-1:1987"), ISO_8859_1},
	{TEXT("iso-ir-100"),      ISO_8859_1},
	{TEXT("ISO_8859-1"),      ISO_8859_1},
	{TEXT("ISO-8859-1"),      ISO_8859_1},
	{TEXT("latin1"),          ISO_8859_1},
	{TEXT("l1"),              ISO_8859_1},
	{TEXT("IBM819"),          ISO_8859_1},
	{TEXT("CP819"),           ISO_8859_1},
	{TEXT("csISOLatin1"),     ISO_8859_1},

	{TEXT("ISO_8859-2:1987"), ISO_8859_2},
	{TEXT("iso-ir-101"),      ISO_8859_2},
	{TEXT("ISO_8859-2"),      ISO_8859_2},
	{TEXT("ISO-8859-2"),      ISO_8859_2},
	{TEXT("latin2"),          ISO_8859_2},
	{TEXT("l2"),              ISO_8859_2},
	{TEXT("csISOLatin2"),     ISO_8859_2},

	{TEXT("ISO_8859-3:1988"), ISO_8859_3},
	{TEXT("iso-ir-109"),      ISO_8859_3},
	{TEXT("ISO_8859-3"),      ISO_8859_3},
	{TEXT("ISO-8859-3"),      ISO_8859_3},
	{TEXT("latin3"),          ISO_8859_3},
	{TEXT("l3"),              ISO_8859_3},
	{TEXT("csISOLatin3"),     ISO_8859_3},

	{TEXT("ISO_8859-4:1988"), ISO_8859_4},
	{TEXT("iso-ir-110"),      ISO_8859_4},
	{TEXT("ISO_8859-4"),      ISO_8859_4},
	{TEXT("ISO-8859-4"),      ISO_8859_4},
	{TEXT("latin4"),          ISO_8859_4},
	{TEXT("l4"),              ISO_8859_4},
	{TEXT("csISOLatin4"),     ISO_8859_4},

	{TEXT("ISO_8859-5:1988"),    ISO_8859_5},
	{TEXT("iso-ir-144"),         ISO_8859_5},
	{TEXT("ISO_8859-5"),         ISO_8859_5},
	{TEXT("ISO-8859-5"),         ISO_8859_5},
	{TEXT("cyrillic"),           ISO_8859_5},
	{TEXT("csISOLatinCyrillic"), ISO_8859_5},

	{TEXT("ISO_8859-6:1987"),  ISO_8859_6},
	{TEXT("iso-ir-127"),       ISO_8859_6},
	{TEXT("ISO_8859-6"),       ISO_8859_6},
	{TEXT("ISO-8859-6"),       ISO_8859_6},
	{TEXT("ECMA-114"),         ISO_8859_6},
	{TEXT("ASMO-708"),         ISO_8859_6},
	{TEXT("arabic"),           ISO_8859_6},
	{TEXT("csISOLatinArabic"), ISO_8859_6},

	{TEXT("ISO_8859-6-E"), ISO_8859_6_E},
	{TEXT("csISO88596E"),  ISO_8859_6_E},

	{TEXT("ISO_8859-6-I"), ISO_8859_6_I},
	{TEXT("csISO88596I"),  ISO_8859_6_I},

	{TEXT("ISO_8859-7:1987"), ISO_8859_7},
	{TEXT("iso-ir-126"),      ISO_8859_7},
	{TEXT("ISO_8859-7"),      ISO_8859_7},
	{TEXT("ISO-8859-7"),      ISO_8859_7},
	{TEXT("ELOT_928"),        ISO_8859_7},
	{TEXT("ECMA-118"),        ISO_8859_7},
	{TEXT("greek"),           ISO_8859_7},
	{TEXT("greek8"),          ISO_8859_7},
	{TEXT("csISOLatinGreek"), ISO_8859_7},

	{TEXT("ISO_8859-8:1988"),  ISO_8859_8},
	{TEXT("iso-ir-138"),       ISO_8859_8},
	{TEXT("ISO_8859-8"),       ISO_8859_8},
	{TEXT("ISO-8859-8"),       ISO_8859_8},
	{TEXT("hebrew"),           ISO_8859_8},
	{TEXT("csISOLatinHebrew"), ISO_8859_8},

	{TEXT("ISO_8859-8-E"), ISO_8859_8_E},
	{TEXT("csISO88598E"),  ISO_8859_8_E},

	{TEXT("ISO_8859-8-I"), ISO_8859_8_I},
	{TEXT("csISO88598I"),  ISO_8859_8_I},

	{TEXT("ISO_8859-9:1989"), ISO_8859_9},
	{TEXT("iso-ir-148"),      ISO_8859_9},
	{TEXT("ISO_8859-9"),      ISO_8859_9},
	{TEXT("ISO-8859-9"),      ISO_8859_9},
	{TEXT("latin5"),          ISO_8859_9},
	{TEXT("l5"),              ISO_8859_9},
	{TEXT("csISOLatin5"),     ISO_8859_9},

	{TEXT("latin6"),           ISO_8859_10},
	{TEXT("iso-ir-157"),       ISO_8859_10},
	{TEXT("l6"),               ISO_8859_10},
	{TEXT("ISO_8859-10:1992"), ISO_8859_10},
	{TEXT("csISOLatin6"),      ISO_8859_10},

	{TEXT("ISO_8859-15"), ISO_8859_15},

	{TEXT("ISO_8859-supp"), ISO_8859_supp},
	{TEXT("iso-ir-154"),    ISO_8859_supp},
	{TEXT("latin1-2-5"),    ISO_8859_supp},
	{TEXT("csISO8859Supp"), ISO_8859_supp},


	{TEXT("UNICODE-1-1"), UNICODE_1_1},
	{TEXT("csUnicode11"), UNICODE_1_1},

	{TEXT("UNICODE-1-1-UTF-7"), UNICODE_1_1_UTF_7},
	{TEXT("csUnicode11UTF7"),   UNICODE_1_1_UTF_7},

	{TEXT("UTF-7"), UTF_7},

	{TEXT("UTF-8"), UTF_8},

	{TEXT("windows-1250"), WINDOWS_1250},

	{TEXT("windows-1251"), WINDOWS_1251},

	{TEXT("windows-1252"), WINDOWS_1252},

	{TEXT("windows-1253"), WINDOWS_1253},

	{TEXT("windows-1254"), WINDOWS_1254},

	{TEXT("windows-1255"), WINDOWS_1255},

	{TEXT("windows-1256"), WINDOWS_1256},

	{TEXT("windows-1257"), WINDOWS_1257},

	{TEXT("windows-1258"), WINDOWS_1258},

	{TEXT(""), -1}
};

CHARSET  CharEncoding = UTF_8;
ThotBool charset_undefined = FALSE;

/*-------------------------------------------------------------
  uputchar
  -------------------------------------------------------------*/
#ifdef __STDC__
int uputchar (int c)
#else  /* !__STDC__ */
int uputchar (c)
int c;
#endif /* !__STDC__ */
{
#   ifdef _I18N_
    /* Compatibility of putwchar: ANSI, WIN NT and WIN 9x */
    return putwchar (c);
#   else  /* !_I18N_ */
    return putchar (c);
#   endif /* !_I18N_ */
}

/*-------------------------------------------------------------
  ustrcasecmp: compare two strings without regard to case.
  -------------------------------------------------------------*/
#ifdef __STDC__
int ustrcasecmp (const CHAR_T* str1, const CHAR_T* str2)
#else  /* __STDC__ */
int ustrcasecmp (str1, str2)
const CHAR_T* str1;
const CHAR_T* str2;
#endif /* __STDC__ */
{
#   ifdef _I18N_
#   ifdef _WINDOWS
    /* Compatibility of _wcsicmp: WIN NT WIN 9x */
    return (int) _wcsicmp ((wchar_t*)str1, (wchar_t*)str2);
#   else  /* !_WINDOWS */
    int len1 = ustrlen (str1);
    int len2 = ustrlen (str2);
    int i    = 0;

    if (len1 != len2)
       return (len1 - len2);

    while (str1[i]) {
          if (towlower (str1[i]) != towlower (str2[i]))
             return (str1[i] - str2[i]);
          i++;
	}
    return 0;
#   endif /* !_WINDOWS */
#   else  /* !_I18N_ */
#   ifdef _WINDOWS
    return (unsigned int) _stricmp ((char*)str1, (char*)str2);
#   else  /* !_WINDOWS */
    return (unsigned int) strcasecmp ((char*)str1, (char*)str2);
#   endif /* !_WINDOWS */
#   endif /* !_I18N_ */
}


/*-------------------------------------------------------------
  ustrcat: appends src to the content of dest. This function 
  suposes that momery has been already allocated in the same 
  way that strcat does.
  -------------------------------------------------------------*/
#ifdef __STDC__
CHAR_T* ustrcat (CHAR_T* dest, const CHAR_T* src)
#else  /* __STDC__ */
CHAR_T* ustrcat (dest, src)
CHAR_T*       dest;
const CHAR_T* src;
#endif /* __STDC__ */
{
#   ifdef _I18N_
    /* Compatibility of wcscat: ANSI, WIN NT and WIN 9x */
    return (CHAR_T*) wcscat ((wchar_t*)dest, (wchar_t*)src);
#   else  /* !_I18N_ */
    return (CHAR_T*) strcat ((char*)dest, (char*)src);
#   endif /* !_I18N_ */
}


/*-------------------------------------------------------------
  ustrchr: Find a character in a string.
  -------------------------------------------------------------*/
#ifdef __STDC__
CHAR_T* ustrchr (const CHAR_T* src, CHAR_T c)
#else  /* __STDC__ */
CHAR_T* ustrchr (src, c)
const CHAR_T*  src;
CHAR_T         c;
#endif /* __STDC__ */
{
#   ifdef _I18N_
    /* Compatibility of wcschr: ANSI, WIN NT and WIN 9x */
    return ((CHAR_T*) wcschr ((wchar_t*)src, (wint_t)c));
#   else  /* !_I18N_ */
    return ((CHAR_T*) strchr ((char*)src, (int)c));
#   endif /* !_I18N_ */
}


/*-------------------------------------------------------------
  ustrcmp: compare strings.
  -------------------------------------------------------------*/
#ifdef __STDC__
int ustrcmp (const CHAR_T* str1, const CHAR_T* str2)
#else  /* __STDC__ */
int ustrcmp (str1, str2)
const CHAR_T* str1;
const CHAR_T* str2;
#endif /* __STDC__ */
{
#   ifdef _I18N_
    /* Compatibility of wcscmp: ANSI, WIN NT and WIN 9x */
    return wcscmp ((wchar_t*)str1, (wchar_t*)str2);
#   else  /* !_I18N_ */
    return strcmp ((char*)str1, (char*)str2);
#   endif /* !_I18N_ */
}


/*-------------------------------------------------------------
  ustrcoll: compre strings using local-specific information.
  -------------------------------------------------------------*/
#ifdef __STDC__
int ustrcoll (const CHAR_T* str1, const CHAR_T* str2)
#else  /* __STDC__ */
int ustrcoll (str1, str2)
const CHAR_T* str1;
const CHAR_T* str2;
#endif /* __STDC__ */
{
#   ifdef _I18N_
    /* Compatibility of wcscoll: ANSI, WIN NT and WIN 9x */
    return wcscoll ((wchar_t*)str1, (wchar_t*)str2);
#   else  /* !_I18N_ */
    return strcoll ((char*)str1, (char*)str2);
#   endif /* !_I18N_ */
}


/*-------------------------------------------------------------
  ustrcpy: copies src into dest. This function suposes that
  momery has been already allocated in the same way that strcpy
  does.
  -------------------------------------------------------------*/
#ifdef __STDC__
CHAR_T* ustrcpy (CHAR_T* dest, const CHAR_T* src)
#else  /* __STDC__ */
CHAR_T* ustrcpy (dest, src)
CHAR_T*       dest;
const CHAR_T* src;
#endif /* __STDC__ */
{
#   ifdef _I18N_
    /* Compatibility of wcscpy: ANSI, WIN NT and WIN 9x */
    return (CHAR_T*) wcscpy ((wchar_t*)dest, (wchar_t*)src);
#   else  /* !_I18N_ */
    return (CHAR_T*) strcpy ((char*)dest, (char*)src);
#   endif /* !_I18N_ */
}


/*-------------------------------------------------------------
  ustrdup: duplicate strings.
  -------------------------------------------------------------*/
#ifdef __STDC__
CHAR_T* ustrdup (const CHAR_T* str)
#else  /* __STDC__ */
CHAR_T* ustrdup (str)
const CHAR_T* str;
#endif /* __STDC__ */
{
#   ifdef _I18N_
#   ifdef _WINDOWS
    /* Compatibility of _wcsdup: WIN NT and WIN 9x */
    return (CHAR_T*) _wcsdup (str);
#   else  /* !_WINDOWS */
    int     len = ustrlen (str);
    CHAR_T* res = (CHAR_T*) malloc (len * sizeof (CHAR_T));
    if (res)
       ustrcpy (res, str);
    return res;
#   endif /* _WINDOWS */
#   else  /* !_I18N_ */
#   ifdef _WINDOWS
    return (CHAR_T*) _strdup ((char*)str);
#   else  /* !_WINDOWS */
    return (CHAR_T*) strdup ((char*)str);
#   endif /* _WINDOWS */
#   endif /* !_I18N_ */
}


/*-------------------------------------------------------------
  iso2wc_strcpy: copies src (8-bit) into dest (16-bit). This 
  function suposes that momery has been already allocated in the 
  same way that strcpy does.
  -------------------------------------------------------------*/
#ifdef __STDC__
CHAR_T* iso2wc_strcpy (CHAR_T* dest, const char* src)
#else  /* __STDC__ */
CHAR_T* iso2wc_strcpy (dest, src)
CHAR_T*      dest;
const char* src;
#endif /* __STDC__ */
{
#   ifdef _I18N_
    int len = strlen (src);
	/*
    for (i = 0; i < len; i++)
        dest[i] = (CHAR_T)src[i];
    dest[i] = (CHAR_T)0;
	*/
    mbstowcs (dest, src, len + 1);
    return dest;
#   else  /* !_I18N_ */
    return (CHAR_T*) strcpy ((char*)dest, (char*)src);
#   endif /* !_I18N_ */
}


/*-------------------------------------------------------------
  ustrlen: get the length of a string.
  -------------------------------------------------------------*/
#ifdef __STDC__
size_t ustrlen (const CHAR_T* str)
#else  /* __STDC__ */
size_t ustrlen (str)
const CHAR_T* str;
#endif /* __STDC__ */
{
#   ifdef _I18N_
    /* Compatibility of wcslen: ANSI, WIN NT and WIN 9x */
    return wcslen ((wchar_t*)str);
#   else  /* !_I18N_ */
    return (size_t) strlen ((char*)str);
#   endif /* !_I18N_ */
}


/*-------------------------------------------------------------
  ustrncasecmp: compare characters of two strings without regard
                to case.
  -------------------------------------------------------------*/
#ifdef __STDC__
int ustrncasecmp (const CHAR_T* str1, const CHAR_T* str2, unsigned int count)
#else  /* __STDC__ */
int ustrncasecmp (str1, str2, count)
const CHAR_T* str1;
const CHAR_T* str2;
unsigned int  count;
#endif /* __STDC__ */
{
#   ifdef _I18N_
#   ifdef _WINDOWS
    /* Compatibility of _wcsnicmp: WIN NT and WIN 9x */
    return _wcsnicmp ((wchar_t*)str1, (wchar_t*)str2, (size_t)count);
#   else  /* !_WINDOWS */
    int len1 = ustrlen (str1);
    int len2 = ustrlen (str2);
    int len;
    int ndx;

    if (count < len1 && count < len2)
       len = count;
    else if (len1 < count && len2 < count)
		len = (len1 < len2) ? len1 : len2;
    else if (len1 < count)
         len = len1;
    else
         len = len2;

    for (ndx = 0; ndx < len; ndx++) {
        CHAR_T c1 = towlower (str1[ndx]);
        CHAR_T c2 = towlower (str2[ndx]);
        if (c1 != c2)
           return c1 - c2;
	}
    return 0;
#   endif /* !_WINDOWS */
#   else  /* !_I18N_ */
#   ifdef _WINDOWS
    return (unsigned int) _strnicmp ((char*)str1, (char*)str2, (size_t)count);
#   else  /* !_WINDOWS */
    return (unsigned int) strncasecmp ((char*)str1, (char*)str2, (size_t)count);
#   endif /* !_WINDOWS */
#   endif /* !_I18N_ */
}


/*-------------------------------------------------------------
  ustrncat: append n characters of a string src.
  -------------------------------------------------------------*/
#ifdef __STDC__
CHAR_T* ustrncat (CHAR_T* dest, const CHAR_T* src, unsigned int count)
#else  /* __STDC__ */
CHAR_T* ustrncat (dest, src, count)
CHAR_T*       dest;
const CHAR_T* src;
unsigned int  count;
#endif /* __STDC__ */
{
#   ifdef _I18N_ 
    /* Compatibility of wcsncat: ANSI, WIN NT and WIN 9x */
    return (CHAR_T*) wcsncat (dest, src, (size_t)count);
#   else  /* !_I18N_ */
    return (CHAR_T*) strncat ((char*)dest, (char*)src, (size_t)count);
#   endif /* !_I18N_ */
}


/*-------------------------------------------------------------
  ustrncmp: compare n characters of str1 and str2.
  -------------------------------------------------------------*/
#ifdef __STDC__
CHAR_T* ustrncmp (const CHAR_T* str1, const CHAR_T* str2, unsigned int count)
#else  /* __STDC__ */
CHAR_T* ustrncmp (str1, str2, count)
const CHAR_T* str1;
const CHAR_T* str2;
unsigned int  count;
#endif /* __STDC__ */
{
#   ifdef _I18N_ 
    /* Compatibility of wcsncmp: ANSI, WIN NT and WIN 9x */
    return (CHAR_T*) wcsncmp ((wchar_t*)str1, (wchar_t*)str2, (size_t)count);
#   else  /* !_I18N_ */
    return (CHAR_T*) strncmp ((char*)str1, (char*)str2, (size_t)count);
#   endif /* !_I18N_ */
}


/*-------------------------------------------------------------
  ustrncpy: copy n characters of one string to another.
  -------------------------------------------------------------*/
#ifdef __STDC__
CHAR_T* ustrncpy (CHAR_T* dest, const CHAR_T* src, unsigned int count)
#else  /* __STDC__ */
CHAR_T* ustrncpy (dest, src, count)
CHAR_T*       dest;
const CHAR_T* src;
unsigned int  count;
#endif /* __STDC__ */
{
#   ifdef _I18N_ 
    /* Compatibility of wcsncpy: ANSI, WIN NT and WIN 9x */
    return (CHAR_T*) wcsncpy ((wchar_t*)dest, (wchar_t*)src, (size_t)count);
#   else  /* !_I18N_ */
    return (CHAR_T*) strncpy ((char*)dest, (char*)src, (size_t)count);
#   endif /* !_I18N_ */
}


/*-------------------------------------------------------------
  ustrrchr: scan a string for the last occurrence of a character.
  -------------------------------------------------------------*/
#ifdef __STDC__
CHAR_T* ustrrchr (const CHAR_T* str, CHAR_T c)
#else  /* __STDC__ */
CHAR_T* ustrrchr (str, c)
const CHAR_T* str;
CHAR_T         c;
#endif /* __STDC__ */
{
#   ifdef _I18N_
    /* Compatibility of wcsrchr: ANSI, WIN NT and WIN 9x */
    return (CHAR_T*) (wcsrchr ((wchar_t*)str, (int)c));
#   else  /* !_I18N_ */
    return (CHAR_T*) strrchr ((char*)str, (int)c);
#   endif /* !_I18N_ */
}


/*-------------------------------------------------------------
  ustrtok: find the next token in a string.
  -------------------------------------------------------------*/
#ifdef __STDC__
CHAR_T* ustrtok (CHAR_T* str, const CHAR_T* delemiter)
#else  /* __STDC__ */
CHAR_T* ustrtok (str, delemiter)
CHAR_T*       str;
const CHAR_T* delemiter;
#endif /* __STDC__ */
{
#   ifdef _I18N_ 
    /* Compatibility of wcstok: ANSI, WIN NT and WIN 9x */
#   ifdef _WINDOWS
    return (CHAR_T*) wcstok ((wchar_t*)str, (wchar_t*)delemiter);
#   else  /* !_WINDOWS */
    return (CHAR_T*) wcstok ((wchar_t*)str, (wchar_t*)delemiter, NULL);
#   endif /* !_WINDOWS */
#   else  /* !_I18N_ */
    return (CHAR_T*) strtok ((char*)str, (char*)delemiter);
#   endif /* !_I18N_ */
}


/*-------------------------------------------------------------
  ustrstr: find a substring.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
CHAR_T* ustrstr (const CHAR_T* str, const CHAR_T* strCharSet)
#else  /* __STDC__ */
CHAR_T* ustrstr (str, strCharSet)
const CHAR_T* str;
const CHAR_T* strCharSet;
#endif /* __STDC__ */
{
#   ifdef _I18N_
    /* Compatibility of wcsstr: ANSI, WIN NT and WIN 9x */
    return (CHAR_T*) wcsstr ((wchar_t*)str, (wchar_t*)strCharSet);
#   else  /* !_I18N_ */
    return (CHAR_T*) strstr ((char*)str, (char*)strCharSet);
#   endif /* !_I18N_ */
}


/*-------------------------------------------------------------
  wc2iso_strcasecmp: compare CHAR_T* string to a char* string.
  -------------------------------------------------------------*/
#ifdef __STDC__
int wc2iso_strcasecmp (const CHAR_T* str1, const char* str2)
#else  /* __STDC__ */
int wc2iso_strcasecmp (str1, str2)
const CHAR_T* str1;
const char*   str2;
#endif /* __STDC__ */
{
    int       diff;
    CHAR_T* wc_str2 = (CHAR_T*) malloc ((strlen (str2) + 1) * sizeof (CHAR_T));

    iso2wc_strcpy (wc_str2, str2);
    diff = ustrcasecmp ((CHAR_T*)str1, (CHAR_T*)wc_str2);

    free (wc_str2);
    return diff;
}


/*-------------------------------------------------------------
  iso2wc_strcasecmp: compare char* string to a CHAR_T* string.
  -------------------------------------------------------------*/
#ifdef __STDC__
int iso2wc_strcasecmp (const char* str1, const CHAR_T* str2)
#else  /* __STDC__ */
int iso2wc_strcasecmp (str1, str2)
const CHAR_T* str1;
const char*   str2;
#endif /* __STDC__ */
{
    return wc2iso_strcasecmp (str2, str1);
}


/*-------------------------------------------------------------
  wc2iso_strcpy: copies src (16-bit) into dest (8-bit). This 
  function suposes that momery has been already allocated in the 
  same way that strcpy does.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
char* wc2iso_strcpy (char* dest, const CHAR_T* src)
#else  /* __STDC__ */
char* wc2iso_strcpy (dest, src)
char*         dest;
const CHAR_T*  src;
#endif /* __STDC__ */
{
    /* 
	int i, len = ustrlen (src);
    for (i = 0; i < len; i++)
        dest[i] = (char)src[i];
    dest[i] = (char)0;
	*/
#   ifdef _I18N_
    wcstombs (dest, src, MAX_TXT_LEN);
#   else  /* !_I18N_ */
    strcpy (dest, src);
#   endif /* !_I18N_ */
    return dest;
}


/*-------------------------------------------------------------
  wc2iso_strncpy: copies src (16-bit) into dest (8-bit). This 
  function suposes that momery has been already allocated in the 
  same way that strcpy does.
  -------------------------------------------------------------*/
#ifdef __STDC__
char* wc2iso_strncpy (char* dest, const CHAR_T* src, int count)
#else  /* __STDC__ */
char* wc2iso_strncpy (dest, src, count)
char*         dest;
const CHAR_T*  src;
int           count;
#endif /* __STDC__ */
{
    int i, len = ustrlen (src);

    if (len <= count)
       return wc2iso_strcpy (dest, src);

    for (i = 0; i < count; i++)
        dest[i] = (char)src[i];
    dest[i] = (char)0;
    return dest;
}


/*-------------------------------------------------------------
  wc2iso_strcmp: compare a CHAR_T* string to a char* string.
  The first arg (str1) must be CHAR_T*
  The second arg mest be char*
  -------------------------------------------------------------*/
#ifdef __STDC__
int wc2iso_strcmp (CHAR_T* str1, const char* str2)
#else  /* __STDC__ */
int wc2iso_strcmp (str1, str2)
const CHAR_T* str1;
const char*   str2;
#endif /* __STDC__ */
{
    int       diff;
    CHAR_T* cus_str2 = (CHAR_T*) malloc ((strlen (str2) + 1) * sizeof (CHAR_T));

    iso2wc_strcpy (cus_str2, str2);
    diff = ustrcmp (str1, cus_str2);

    free (cus_str2);
    return diff;
}


/*----------------------------------------------------------------------
  TtaGetCharset: 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
CHARSET TtaGetCharset (const CHAR_T* charsetname)
#else  /* !__STDC__ */
CHARSET TtaGetCharset (charsetname)
const CHAR_T* charsetname;
#endif /* !__STDC__ */
{
    int index = 0;

    if (charsetname == NULL || charsetname[0] == 0)
       return UNDEFINED_CHARSET;

    while (CharsetCodeTable[index].ISOCode != NULL && CharsetCodeTable[index].ISOCode[0]) {
          if (!ustrcasecmp (CharsetCodeTable[index].ISOCode, charsetname))
             return CharsetCodeTable[index].Charset;
          index++;
	}

    return UNDEFINED_CHARSET;
}


#ifdef _I18N_
/*----------------------------------------------------------------------
  TtaGetNextWideCharFromMultibyteString: Looks for the next Wide character 
  value in a multibyte character string.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int TtaGetNextWideCharFromMultibyteString (CHAR_T* car, unsigned char** txt, CHARSET encoding)
#else  /* !__STDC__ */
int TtaGetNextWideCharFromMultibyteString (car, txt, encoding)
CHAR_T*        car;
unsigned char* txt;
CHARSET        encoding;
#endif /* !__STDC__ */
{
    int            nbBytesToRead;
    unsigned char* start = *txt;
    CHAR_T         res;

    switch (encoding) {
           case ISO_8859_1: 
                nbBytesToRead = 1;
                *car = (CHAR_T) *start++;
                break;

           case ISO_8859_2:
                nbBytesToRead = 1;
                *car = TtaGetUnicodeValueFromISOLatin2Code (*start);
                start++;
                break;

           case ISO_8859_3:
                nbBytesToRead = 1;
                *car = TtaGetUnicodeValueFromISOLatin3Code (*start);
                start++;
                break;

           case ISO_8859_4:
                nbBytesToRead = 1;
                *car = TtaGetUnicodeValueFromISOLatin4Code (*start);
                start++;
                break;

           case ISO_8859_5:
                nbBytesToRead = 1;
                *car = TtaGetUnicodeValueFromISOLatin5Code (*start);
                start++;
                break;

           case ISO_8859_6:
                nbBytesToRead = 1;
                *car = TtaGetUnicodeValueFromISOLatin6Code (*start);
                start++;
                break;

           case ISO_8859_7:
                nbBytesToRead = 1;
                *car = TtaGetUnicodeValueFromISOLatin7Code (*start);
                start++;
                break;

           case ISO_8859_8:
                nbBytesToRead = 1;
                *car = TtaGetUnicodeValueFromISOLatin8Code (*start);
                start++;
                break;

           case ISO_8859_9:
                nbBytesToRead = 1;
                *car = TtaGetUnicodeValueFromISOLatin9Code (*start);
                start++;
                break;

           case WINDOWS_1250:
                nbBytesToRead = 1;
                *car = TtaGetUnicodeValueFromWindows1250CP (*start);
                start++;
                break;

           case WINDOWS_1251:
                nbBytesToRead = 1;
                *car = TtaGetUnicodeValueFromWindows1251CP (*start);
                start++;
                break;

           case WINDOWS_1252:
                nbBytesToRead = 1;
                *car = TtaGetUnicodeValueFromWindows1252CP (*start);
                start++;
                break;

           case WINDOWS_1253:
                nbBytesToRead = 1;
                *car = TtaGetUnicodeValueFromWindows1253CP (*start);
                start++;
                break;

           case WINDOWS_1254:
                nbBytesToRead = 1;
                *car = TtaGetUnicodeValueFromWindows1254CP (*start);
                start++;
                break;

           case WINDOWS_1255:
                nbBytesToRead = 1;
                *car = TtaGetUnicodeValueFromWindows1255CP (*start);
                start++;
                break;

           case WINDOWS_1256:
                nbBytesToRead = 1;
                *car = TtaGetUnicodeValueFromWindows1256CP (*start);
                start++;
                break;

           case WINDOWS_1257:
                nbBytesToRead = 1;
                *car = TtaGetUnicodeValueFromWindows1257CP (*start);
                start++;
                break;

           case UTF_8:
                if (*start < 0xC0)
                   nbBytesToRead = 1;
                else if (*start < 0xE0)
                     nbBytesToRead = 2;
                else if (*start < 0xF0)
                     nbBytesToRead = 3;
                else if (*start < 0xF8)
                     nbBytesToRead = 4;
                else if (*start < 0xFC)
                     nbBytesToRead = 5;
                else if (*start <= 0xFF)
                     nbBytesToRead = 6;
            
                res = 0;

                /* See how many bytes to read to build a wide character */
                switch (nbBytesToRead) {        /** WARNING: There is not break statement between cases */
                       case 6: res += *start++;
                               res <<= 6;

                       case 5: res += *start++;
                               res <<= 6;
            
                       case 4: res += *start++;
                               res <<= 6;

                       case 3: res += *start++;
                               res <<= 6;

                       case 2: res += *start++;
                               res <<= 6;
            
                       case 1: res += *start++;
				}
                res -= offset[nbBytesToRead - 1];

                if (res <= 0xFFFF)
                   *car = res;
                else 
                    *car = TEXT('?');    
                break;
	}

    return nbBytesToRead;
}
#endif /* _I18N_ */













