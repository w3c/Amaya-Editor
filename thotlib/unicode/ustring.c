/*
 *
 *  (c) COPYRIGHT INRIA, 1999-2003
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/* Unicode Routines
 * Authors: I. Vatton (W3C/INRIA)
 *          R. Guetari (W3C/INRIA): previous version
 */

#include <stdio.h>
#include <stdlib.h>
#include "thot_sys.h"

#include "registry_f.h"
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
  char       *ISOCode;
  CHARSET     Charset;
} CharsetCode;

static CharsetCode CharsetCodeTable[] =
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
    {"", UNDEFINED_CHARSET}
};

#if defined(_I18N_)
/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
wchar_t Ttowlower(wchar_t wc)
{
  if (wc < 128) return tolower(wc);
  else return wc;
}
#endif /* _I18N_ */

#if defined(_I18N_) && !defined(_WINDOWS)
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
#endif /* _I18N_ && !_WINDOWS */


/*-------------------------------------------------------------
  ustrcasecmp: compare two strings without regard to case.
  -------------------------------------------------------------*/
int ustrcasecmp (const CHAR_T *str1, const CHAR_T *str2)
{
#ifdef _I18N_
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
#else  /* _I18N_ */
  return (unsigned int) strcasecmp ((char*)str1, (char*)str2);
#endif /* _I18N_ */
}


/*-------------------------------------------------------------
  ustrcat: appends src to the content of dest. This function 
  suposes that memory has been already allocated in the same 
  way that strcat does.
  -------------------------------------------------------------*/
CHAR_T *ustrcat (CHAR_T *dest, const CHAR_T *src)
{
#ifdef _I18N_
  /* Compatibility of wcscat: ANSI, WIN NT and WIN 9x */
  return (CHAR_T*) wcscat ((wchar_t*)dest, (wchar_t*)src);
#else  /* _I18N_ */
  return (CHAR_T*) strcat ((char*)dest, (char*)src);
#endif /* _I18N_ */
}


/*-------------------------------------------------------------
  ustrchr: Find a character in a string.
  -------------------------------------------------------------*/
CHAR_T *ustrchr (const CHAR_T *src, CHAR_T c)
{
#ifdef _I18N_
  /* Compatibility of wcschr: ANSI, WIN NT and WIN 9x */
  return ((CHAR_T*) wcschr ((wchar_t*)src, (wint_t)c));
#else  /* _I18N_ */
  return ((CHAR_T*) strchr ((char*)src, (int)c));
#endif /* _I18N_ */
}


/*-------------------------------------------------------------
  ustrcmp: compare strings.
  -------------------------------------------------------------*/
int ustrcmp (const CHAR_T *str1, const CHAR_T *str2)
{
#ifdef _I18N_
  /* Compatibility of wcscmp: ANSI, WIN NT and WIN 9x */
  return wcscmp ((wchar_t*)str1, (wchar_t*)str2);
#else  /* _I18N_ */
  return strcmp ((char*)str1, (char*)str2);
#endif /* _I18N_ */
}


/*-------------------------------------------------------------
  ustrcpy: copies src into dest. This function suposes that
  momery has been already allocated in the same way that strcpy
  does.
  -------------------------------------------------------------*/
CHAR_T *ustrcpy (CHAR_T *dest, const CHAR_T *src)
{
#ifdef _I18N_
  /* Compatibility of wcscpy: ANSI, WIN NT and WIN 9x */
  return (CHAR_T*) wcscpy ((wchar_t *)dest, (wchar_t *)src);
#else  /* _I18N_ */
  return (CHAR_T*) strcpy ((char *)dest, (char *)src);
#endif /* _I18N_ */
}


/*-------------------------------------------------------------
  ustrlen: get the length of a string.
  -------------------------------------------------------------*/
size_t ustrlen (const CHAR_T *str)
{
#ifdef _I18N_
  /* Compatibility of wcslen: ANSI, WIN NT and WIN 9x */
  return wcslen ((wchar_t*)str);
#else  /* _I18N_ */
  return (size_t) strlen ((char*)str);
#endif /* _I18N_ */
}


/*-------------------------------------------------------------
  ustrncasecmp: compare characters of two strings without regard
                to case.
  -------------------------------------------------------------*/
int ustrncasecmp (const CHAR_T *str1, const CHAR_T *str2,
		  unsigned int count)
{
#ifdef _I18N_
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
#else  /* _I18N_ */
#ifdef _WINDOWS
  return (unsigned int) _strnicmp ((char*)str1, (char*)str2, (size_t)count);
#else  /* _WINDOWS */
  return (unsigned int) strncasecmp ((char*)str1, (char*)str2, (size_t)count);
#endif /* _WINDOWS */
#endif /* _I18N_ */
}


/*-------------------------------------------------------------
  ustrncat: append n characters of a string src.
  -------------------------------------------------------------*/
CHAR_T *ustrncat (CHAR_T *dest, const CHAR_T *src, unsigned int count)
{
#ifdef _I18N_ 
  /* Compatibility of wcsncat: ANSI, WIN NT and WIN 9x */
  return (CHAR_T*) wcsncat (dest, src, (size_t)count);
#else  /* _I18N_ */
  return (CHAR_T*) strncat ((char*)dest, (char*)src, (size_t)count);
#endif /* _I18N_ */
}


/*-------------------------------------------------------------
  ustrncmp: compare n characters of str1 and str2.
  -------------------------------------------------------------*/
CHAR_T *ustrncmp (const CHAR_T *str1, const CHAR_T *str2, unsigned int count)
{
#ifdef _I18N_ 
  /* Compatibility of wcsncmp: ANSI, WIN NT and WIN 9x */
  return (CHAR_T*) wcsncmp ((wchar_t*)str1, (wchar_t*)str2, (size_t)count);
#else  /* _I18N_ */
  return (CHAR_T*) strncmp ((char*)str1, (char*)str2, (size_t)count);
#endif /* _I18N_ */
}


/*-------------------------------------------------------------
  ustrncpy: copy n characters of one string to another.
  -------------------------------------------------------------*/
CHAR_T *ustrncpy (CHAR_T *dest, const CHAR_T *src, unsigned int count)
{
#ifdef _I18N_ 
  /* Compatibility of wcsncpy: ANSI, WIN NT and WIN 9x */
  return (CHAR_T*) wcsncpy ((wchar_t*)dest, (wchar_t*)src, (size_t)count);
#else  /* _I18N_ */
  return (CHAR_T*) strncpy ((char*)dest, (char*)src, (size_t)count);
#endif /* _I18N_ */
}


/*-------------------------------------------------------------
  ustrrchr: scan a string for the last occurrence of a character.
  -------------------------------------------------------------*/
CHAR_T *ustrrchr (const CHAR_T *str, CHAR_T c)
{
#ifdef _I18N_
  /* Compatibility of wcsrchr: ANSI, WIN NT and WIN 9x */
  return (CHAR_T*) (wcsrchr ((wchar_t*)str, (wchar_t)c));
#else  /* _I18N_ */
  return (CHAR_T*) strrchr ((char*)str, (char)c);
#endif /* _I18N_ */
}


/*-------------------------------------------------------------
  ustrtok: find the next token in a string.
  -------------------------------------------------------------*/
CHAR_T *ustrtok (CHAR_T *str, const CHAR_T *delemiter)
{
#ifdef _I18N_ 
  /* Compatibility of wcstok: ANSI, WIN NT and WIN 9x */
#ifdef _WINDOWS
  return (CHAR_T*) wcstok ((wchar_t*)str, (wchar_t*)delemiter);
#else  /* _WINDOWS */
  return (CHAR_T*) wcstok ((wchar_t*)str, (wchar_t*)delemiter, NULL);
#endif /* _WINDOWS */
#else  /* _I18N_ */
  return (CHAR_T*) strtok ((char*)str, (char*)delemiter);
#endif /* _I18N_ */
}


/*-------------------------------------------------------------
  ustrstr: find a substring.
  -------------------------------------------------------------*/
CHAR_T *ustrstr (const CHAR_T *str, const CHAR_T *strCharSet)
{
#ifdef _I18N_
  /* Compatibility of wcsstr: ANSI, WIN NT and WIN 9x */
  return (CHAR_T*) wcsstr ((wchar_t*)str, (wchar_t*)strCharSet);
#else  /* _I18N_ */
  return (CHAR_T*) strstr ((char*)str, (char*)strCharSet);
#endif /* _I18N_ */
}


/*----------------------------------------------------------------------
  TtaGetCharset gives the charset 
  ----------------------------------------------------------------------*/
CHARSET TtaGetCharset (char *charsetname)
{
  int index = 0;

  if (charsetname == NULL || charsetname[0] == EOS)
    return UNDEFINED_CHARSET;
  while (CharsetCodeTable[index].Charset != UNDEFINED_CHARSET)
    {
      if (!strcasecmp (CharsetCodeTable[index].ISOCode, charsetname))
	return CharsetCodeTable[index].Charset;
      index++;
    }
  return UNDEFINED_CHARSET;
}


/*----------------------------------------------------------------------
  TtaGetDefaultCharset gives the default charset 
  ----------------------------------------------------------------------*/
CHARSET TtaGetDefaultCharset ()
{
  char     *charsetname;

  /* it should be given by the system locale */
  charsetname = TtaGetEnvString ("Default_Charset");
  if (charsetname)
    return TtaGetCharset (charsetname);
  else
    return ISO_8859_1;
}

/*----------------------------------------------------------------------
  TtaGetCharsetName gives the constant string of the charset ISO name.
  ----------------------------------------------------------------------*/
char *TtaGetCharsetName (CHARSET charset)
{
  int index = 0;

  if (UNDEFINED_CHARSET)
    return NULL;
  while (CharsetCodeTable[index].Charset != UNDEFINED_CHARSET)
    {
      if (CharsetCodeTable[index].Charset == charset)
	return (CharsetCodeTable[index].ISOCode);
      index++;
    }
  return NULL;
}















