/*
 *
 *  (c) COPYRIGHT INRIA, 1999-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/* Unicode Routines
 * Authors: I. Vatton (W3C/INRIA)
 *          R. Guetari (W3C/INRIA): previous version
 */

#include <stdio.h>
#ifndef _WINDOWS
#include <langinfo.h>
#endif /* _WINDOWS */
#include <stdlib.h>
#include "thot_sys.h"
#include "fileaccess.h"

#include "platform_f.h"
#include "registry_f.h"
#include "ustring_f.h"

static CHARSET LocaleSystemCharset = UNSUPPORTED_CHARSET;

unsigned long offset[6] = {
  0x00000000UL,
  0x00003080UL,
  0x000E2080UL,
  0x03C82080UL,
  0xFA082080UL,
  0x82082080UL
};

typedef struct {
  const char *ISOCode;
  CHARSET     Charset;
} CharsetCode;

static const CharsetCode CharsetCodeTable[] =
{
    {"us-ascii",         US_ASCII},
    {"ascii",            US_ASCII},
    {"iso646-US",        US_ASCII},
    {"us",               US_ASCII},
    {"iso-8859-1",       ISO_8859_1},
    {"iso_8859-1:1987",  ISO_8859_1},
    {"latin1",           ISO_8859_1},
    {"iso-8859-2",       ISO_8859_2},
    {"iso_8859-2:1987",  ISO_8859_2},
    {"latin2",           ISO_8859_2},
    {"iso-8859-3",       ISO_8859_3},
    {"iso_8859-3:1988",  ISO_8859_3},
    {"latin3",           ISO_8859_3},
    {"iso-8859-4",       ISO_8859_4},
    {"iso_8859-4:1988",  ISO_8859_4},
    {"latin4",           ISO_8859_4},
    {"iso-8859-5",       ISO_8859_5},
    {"iso_8859-5:1988",  ISO_8859_5},
    {"iso-8859-6",       ISO_8859_6},
    {"iso_8859-6:1987",  ISO_8859_6},
    {"iso-8859-6",       ISO_8859_6},
    {"arabic",           ISO_8859_6},
    {"ECMA-114",         ISO_8859_6},
    {"iso-8859-7",       ISO_8859_7},
    {"iso_8859-7:1987",  ISO_8859_7},
    {"greek",            ISO_8859_7},
    {"ECMA-118",         ISO_8859_7},
    {"iso-8859-8",       ISO_8859_8},
    {"iso_8859-8:1988",  ISO_8859_8},
    {"hebrew",           ISO_8859_8},
    {"iso-8859-9",       ISO_8859_9},
    {"iso_8859-9:1989",  ISO_8859_9},
    {"latin5",           ISO_8859_9},
    {"iso-8859-10",      ISO_8859_10},
    {"iso-8859-15",      ISO_8859_15},

    {"UNICODE-1-1",      UNICODE_1_1},
    {"csUnicode11",      UNICODE_1_1},

    {"UNICODE-1-1-UTF-7", UNICODE_1_1_UTF_7},
    {"csUnicode11UTF7",   UNICODE_1_1_UTF_7},

    {"UTF-7",            UTF_7},
    {"UTF-8",            UTF_8},
    {"windows-1250",     WINDOWS_1250},
    {"windows-1251",     WINDOWS_1251},
    {"windows-1252",     WINDOWS_1252},
    {"windows-1253",     WINDOWS_1253},
    {"windows-1254",     WINDOWS_1254},
    {"windows-1255",     WINDOWS_1255},
    {"windows-1256",     WINDOWS_1256},
    {"windows-1257",     WINDOWS_1257},
    {"windows-1258",     WINDOWS_1258},

    {"koi8-r",           KOI8_R},
    {"macintosh",        MAC_OS_ROMAN},
    {"iso-2022-jp",      ISO_2022_JP},
    {"euc-jp",           EUC_JP},
    {"shift_jis",        SHIFT_JIS},
    {"shift-jis",        SHIFT_JIS},
    {"x-sjis",           SHIFT_JIS},
    {"gb2312",           GB_2312},
    {"gb-2312",          GB_2312},
    {"", UNSUPPORTED_CHARSET}
};

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
wchar_t Ttowlower(wchar_t wc)
{
  if (wc < 128) return tolower(wc);
  else return wc;
}

#if defined(_UNIX) && !defined(_AIX)
/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
wchar_t *wcstok(wchar_t *s, const wchar_t *delim, wchar_t **last)
{
  wchar_t      *spanp;
  int           c, sc;
  wchar_t      *tok;

  if (s == NULL && (s = *last) == NULL)
    return NULL;

  /*
   * Skip (span) leading delimiters (s += strspn(s, delim), sort of).
   */
 cont:
  c = *s++;
  for (spanp = (wchar_t *)delim; (sc = *spanp++) != 0; )
    {
      if (c == sc)
	goto cont;
    }
  
  if (c == 0)		/* no non-delimiter characters */
    {
      *last = NULL;
      return NULL;
    }
  tok = s - 1;
  
  /*
   * Scan token (scan for delimiters: s += strcspn(s, delim), sort of).
   * Note that delim must have one NUL; we stop if we see that, too.
   */
  for (;;)
    {
      c = *s++;
      spanp = (wchar_t *)delim;
      do
	{
	  if ((sc = *spanp++) == c)
	    {
	      if (c == 0)
		{
		  s = NULL;
		}
	      else
		{
		  wchar_t *w = s - 1;
		  *w = '\0';
		}
	      *last = s;
	      return tok;
	    }
	}
      while (sc != 0);
    }
  /* NOTREACHED */
}
#endif /* !_WINDOWS && !_AIX*/


/*-------------------------------------------------------------
  ustrcasecmp: compare two strings without regard to case.
  -------------------------------------------------------------*/
int ustrcasecmp (const CHAR_T *str1, const CHAR_T *str2)
{
#ifdef _WINDOWS
  /* Compatibility of _wcsicmp: WIN NT WIN 9x */
  return (int) _wcsicmp ((wchar_t*)str1, (wchar_t*)str2);
#else  /* _WINDOWS */
  int     i = 0;
  wchar_t ch1, ch2;

  while (str1[i] != EOS && str2[i] != EOS)
    {
      ch1 = Ttowlower(str1[i]);
      ch2 = Ttowlower(str2[i]);
      if (ch1 != ch2)
	return (ch1 - ch2);
      i++;
    }
  return (str1[i] - str2[i]);
#endif /* _WINDOWS */
}


/*-------------------------------------------------------------
  ustrcat: appends src to the content of dest. This function 
  suposes that memory has been already allocated in the same 
  way that strcat does.
  -------------------------------------------------------------*/
CHAR_T *ustrcat (CHAR_T *dest, const CHAR_T *src)
{
  /* Compatibility of wcscat: ANSI, WIN NT and WIN 9x */
  return (CHAR_T*) wcscat ((wchar_t*)dest, (wchar_t*)src);
}


/*-------------------------------------------------------------
  ustrchr: Find a character in a string.
  -------------------------------------------------------------*/
CHAR_T *ustrchr (const CHAR_T *src, CHAR_T c)
{
  /* Compatibility of wcschr: ANSI, WIN NT and WIN 9x */
  return ((CHAR_T*) wcschr ((wchar_t*)src, (wint_t)c));
}


/*-------------------------------------------------------------
  ustrcmp: compare strings.
  -------------------------------------------------------------*/
int ustrcmp (const CHAR_T *str1, const CHAR_T *str2)
{
  /* Compatibility of wcscmp: ANSI, WIN NT and WIN 9x */
  return wcscmp ((wchar_t*)str1, (wchar_t*)str2);
}


/*-------------------------------------------------------------
  ustrcpy: copies src into dest. This function suposes that
  momery has been already allocated in the same way that strcpy
  does.
  -------------------------------------------------------------*/
CHAR_T *ustrcpy (CHAR_T *dest, const CHAR_T *src)
{
  /* Compatibility of wcscpy: ANSI, WIN NT and WIN 9x */
  return (CHAR_T*) wcscpy ((wchar_t *)dest, (wchar_t *)src);
}


/*-------------------------------------------------------------
  ustrlen: get the length of a string.
  -------------------------------------------------------------*/
size_t ustrlen (const CHAR_T *str)
{
  /* Compatibility of wcslen: ANSI, WIN NT and WIN 9x */
  return wcslen ((wchar_t*)str);
}


/*-------------------------------------------------------------
  ustrncasecmp: compare characters of two strings without regard
                to case.
  -------------------------------------------------------------*/
int ustrncasecmp (const CHAR_T *str1, const CHAR_T *str2,
		  unsigned int count)
{
#ifdef _WINDOWS
  /* Compatibility of _wcsnicmp: WIN NT and WIN 9x */
  return _wcsnicmp ((wchar_t *)str1, (wchar_t *)str2, (size_t)count);
#else  /* _WINDOWS */
  char      c1, c2;
  unsigned int       len1 = ustrlen (str1);
  unsigned int       len2 = ustrlen (str2);
  int       len, ndx;

  if (count < len1 && count < len2)
    len = count;
  else if (len1 < count && len2 < count)
    len = (len1 < len2) ? len1 : len2;
  else if (len1 < count)
    len = len1;
  else
    len = len2;

  for (ndx = 0; ndx < len; ndx++)
    {
      if (str1[ndx] != str2[ndx])
	{
	  if (str1[ndx] < 255 && str2[ndx] < 255)
	    {
	      c1 = Ttowlower (str1[ndx]);
	      c2 = Ttowlower (str2[ndx]);
	      if (c1 != c2)
		return c1 - c2;
	    }
	  else
	    return (int) str1[ndx] - (int) str2[ndx];
	}
    }
  return 0;
#endif /* _WINDOWS */
}


/*-------------------------------------------------------------
  ustrncat: append n characters of a string src.
  -------------------------------------------------------------*/
CHAR_T *ustrncat (CHAR_T *dest, const CHAR_T *src, unsigned int count)
{
  /* Compatibility of wcsncat: ANSI, WIN NT and WIN 9x */
  return (CHAR_T*) wcsncat (dest, src, (size_t)count);
}


/*-------------------------------------------------------------
  ustrncmp: compare n characters of str1 and str2.
  -------------------------------------------------------------*/
CHAR_T *ustrncmp (const CHAR_T *str1, const CHAR_T *str2, unsigned int count)
{
  /* Compatibility of wcsncmp: ANSI, WIN NT and WIN 9x */
  return (CHAR_T*) wcsncmp ((wchar_t*)str1, (wchar_t*)str2, (size_t)count);
}


/*-------------------------------------------------------------
  ustrncpy: copy n characters of one string to another.
  -------------------------------------------------------------*/
CHAR_T *ustrncpy (CHAR_T *dest, const CHAR_T *src, unsigned int count)
{
  /* Compatibility of wcsncpy: ANSI, WIN NT and WIN 9x */
  return (CHAR_T*) wcsncpy ((wchar_t*)dest, (wchar_t*)src, (size_t)count);
}


/*-------------------------------------------------------------
  ustrrchr: scan a string for the last occurrence of a character.
  -------------------------------------------------------------*/
CHAR_T *ustrrchr (const CHAR_T *str, CHAR_T c)
{
  /* Compatibility of wcsrchr: ANSI, WIN NT and WIN 9x */
  return (CHAR_T*) (wcsrchr ((wchar_t*)str, (wchar_t)c));
}


/*-------------------------------------------------------------
  ustrtok: find the next token in a string.
  -------------------------------------------------------------*/
CHAR_T *ustrtok (CHAR_T *str, const CHAR_T *delemiter)
{
  /* Compatibility of wcstok: ANSI, WIN NT and WIN 9x */
#if defined(_WINDOWS) || defined(_AIX)
  return (CHAR_T*) wcstok ((wchar_t*)str, (wchar_t*)delemiter);
#else  /* defined(_WINDOWS) || defined(_AIX) */
  return (CHAR_T*) wcstok ((wchar_t*)str, (wchar_t*)delemiter, NULL);
#endif /* defined(_WINDOWS) || defined(_AIX) */
}


/*-------------------------------------------------------------
  ustrstr: find a substring.
  -------------------------------------------------------------*/
CHAR_T *ustrstr (const CHAR_T *str, const CHAR_T *strCharSet)
{
  /* Compatibility of wcsstr: ANSI, WIN NT and WIN 9x */
  return (CHAR_T*) wcsstr ((wchar_t*)str, (wchar_t*)strCharSet);
}


/*----------------------------------------------------------------------
  TtaGetCharset gives the charset 
  ----------------------------------------------------------------------*/
CHARSET TtaGetCharset (const char *charsetname)
{
  int index = 0;

  if (charsetname == NULL || charsetname[0] == EOS)
    return UNDEFINED_CHARSET;
  while (CharsetCodeTable[index].Charset != UNSUPPORTED_CHARSET)
    {
      if (!strcasecmp (CharsetCodeTable[index].ISOCode, charsetname))
	return CharsetCodeTable[index].Charset;
      index++;
    }
  return UNSUPPORTED_CHARSET;
}


/*----------------------------------------------------------------------
  TtaGetLocaleCharset returns the user system charset
  ----------------------------------------------------------------------*/
CHARSET TtaGetLocaleCharset ()
{
#ifdef _WINDOWS
  UINT cp;
   if (LocaleSystemCharset == UNSUPPORTED_CHARSET)
    {
      cp = GetOEMCP ();
      switch (cp)
	{
	case 437: /* Latin 1 */
	case 850:
	case 860:
	case 863:
	case 861:
	  LocaleSystemCharset = WINDOWS_1252;
	  break;
	case 708:
	case 709:
	case 710:
	case 720: /* Arabic */
	case 864:
	  LocaleSystemCharset = WINDOWS_1256;
	  break;
	case 737: /* Greek */
	case 869:
	  LocaleSystemCharset = WINDOWS_1253;
	  break;
	case 775: /* Baltic */
	case 852:
	case 865:
	  LocaleSystemCharset = WINDOWS_1257;
	  break;
	case 855: /* Cyrillic */
	case 866:
	  LocaleSystemCharset = WINDOWS_1251;
	  break;
	case 857: /* Turkish */
	  LocaleSystemCharset = WINDOWS_1254;
	  break;
	case 862: /* Hebrew */
	  LocaleSystemCharset = WINDOWS_1255;
	  break;
	default:
	  LocaleSystemCharset = WINDOWS_1252;
	}
   }
#else /* _WINDOWS */
#if !defined(_MACOS) && !defined(_FREEBSD)
  if (LocaleSystemCharset == UNSUPPORTED_CHARSET)
    {
      char *buffer;
      buffer = nl_langinfo(_NL_MESSAGES_CODESET);
      if (buffer != NULL)
        LocaleSystemCharset = TtaGetCharset(buffer);
    }
#endif /* _MACOS && _FREEBSD */
  if ((LocaleSystemCharset == UNSUPPORTED_CHARSET) ||
      (LocaleSystemCharset == UNDEFINED_CHARSET))
#if defined (_MACOS) && defined (_WX) 
    /* default macosx charset is utf-8 */
    LocaleSystemCharset = UTF_8;
#else /* _MACOS */
    /* default unix charset is iso-latin-1 */
    LocaleSystemCharset = ISO_8859_1;
#endif /* _MACOSX */
#endif /* _WINDOWS */
  return LocaleSystemCharset;
}


/*----------------------------------------------------------------------
  TtaGetDefaultCharset gives the default charset 
  ----------------------------------------------------------------------*/
CHARSET TtaGetDefaultCharset ()
{
#ifdef _WX
  return UTF_8;
#else /* _WX */
#ifdef _MACOS
    /* default macosx dialog charset is iso_8859_1 */
  return ISO_8859_1;
#else /* _MACOS */
  return TtaGetLocaleCharset ();
#endif /* _MACOSX */
#endif /* _WX */
}

/*----------------------------------------------------------------------
  TtaGetCharsetName gives the constant string of the charset ISO name.
  ----------------------------------------------------------------------*/
const char *TtaGetCharsetName (CHARSET charset)
{
  int index = 0;

  if (charset == UNDEFINED_CHARSET)
    return NULL;
  while (CharsetCodeTable[index].Charset != UNSUPPORTED_CHARSET)
    {
      if (CharsetCodeTable[index].Charset == charset)
	return (CharsetCodeTable[index].ISOCode);
      index++;
    }
  return NULL;
}


