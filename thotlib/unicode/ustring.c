/*
 *
 *  (c) COPYRIGHT INRIA, 1999-2001.
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
  char        ISOCode [50];
  CHARSET     Charset;
} CharsetCode;

static CharsetCode CharsetCodeTable[] =
{
    {"ascii",            US_ASCII},
    {"iso646-US",        US_ASCII},
    {"us-ascii",         US_ASCII},
    {"us",               US_ASCII},
    {"iso-8859-1",      ISO_8859_1},
    {"iso_8859-1:1987", ISO_8859_1},
    {"latin1",          ISO_8859_1},
    {"iso-8859-2",      ISO_8859_2},
    {"iso_8859-2:1987", ISO_8859_2},
    {"latin2",          ISO_8859_2},
    {"iso-8859-3",      ISO_8859_3},
    {"iso_8859-3:1988", ISO_8859_3},
    {"latin3",          ISO_8859_3},
    {"iso-8859-4",      ISO_8859_4},
    {"iso_8859-4:1988", ISO_8859_4},
    {"latin4",          ISO_8859_4},
    {"iso-8859-5",         ISO_8859_5},
    {"iso_8859-5:1988",    ISO_8859_5},
    {"iso-8859-6",       ISO_8859_6},
    {"iso_8859-6:1987",  ISO_8859_6},
    {"iso-8859-6",       ISO_8859_6},
    {"arabic",           ISO_8859_6},
    {"ECMA-114",         ISO_8859_6},
    {"iso-8859-7",      ISO_8859_7},
    {"iso_8859-7:1987", ISO_8859_7},
    {"greek",           ISO_8859_7},
    {"ECMA-118",        ISO_8859_7},
    {"iso-8859-8",       ISO_8859_8},
    {"iso_8859-8:1988",  ISO_8859_8},
    {"hebrew",           ISO_8859_8},
    {"iso-8859-9",      ISO_8859_9},
    {"iso_8859-9:1989", ISO_8859_9},
    {"latin5",          ISO_8859_9},

    {"UNICODE-1-1", UNICODE_1_1},
    {"csUnicode11", UNICODE_1_1},

    {"UNICODE-1-1-UTF-7", UNICODE_1_1_UTF_7},
    {"csUnicode11UTF7",   UNICODE_1_1_UTF_7},

    {"UTF-7", UTF_7},
    {"UTF-8", UTF_8},
    {"windows-1250", WINDOWS_1250},
    {"windows-1251", WINDOWS_1251},
    {"windows-1252", WINDOWS_1252},
    {"windows-1253", WINDOWS_1253},
    {"windows-1254", WINDOWS_1254},
    {"windows-1255", WINDOWS_1255},
    {"windows-1256", WINDOWS_1256},
    {"windows-1257", WINDOWS_1257},
    {"windows-1258", WINDOWS_1258},
    {"", UNDEFINED_CHARSET}
};


/*-------------------------------------------------------------
  uputchar
  -------------------------------------------------------------*/
int     uputchar (int c)
{
#ifdef _I18N_
    /* Compatibility of putwchar: ANSI, WIN NT and WIN 9x */
    return putwchar (c);
#else  /* _I18N_ */
    return putchar (c);
#endif /* _I18N_ */
}

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
  int len1 = ustrlen (str1);
  int len2 = ustrlen (str2);
  int i    = 0;

  if (len1 != len2)
    return (len1 - len2);
  while (str1[i])
    {
      if (towlower (str1[i]) != towlower (str2[i]))
	return (str1[i] - str2[i]);
      i++;
    }
  return 0;
#endif /* _WINDOWS */
#else  /* _I18N_ */
#ifdef _WINDOWS
  return (unsigned int) _stricmp ((char*)str1, (char*)str2);
#else  /* _WINDOWS */
  return (unsigned int) strcasecmp ((char*)str1, (char*)str2);
#endif /* _WINDOWS */
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
  ustrcoll: compare strings using local-specific information.
  -------------------------------------------------------------*/
int ustrcoll (const CHAR_T *str1, const CHAR_T *str2)
{
#ifdef _I18N_
  /* Compatibility of wcscoll: ANSI, WIN NT and WIN 9x */
  return wcscoll ((wchar_t*)str1, (wchar_t*)str2);
#else  /* _I18N_ */
  return strcoll ((char*)str1, (char*)str2);
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
  iso2wc_strcpy: copies src (8-bit) into dest (16-bit). This 
  function suposes that momery has been already allocated in the 
  same way that strcpy does.
  -------------------------------------------------------------*/
CHAR_T *iso2wc_strcpy (CHAR_T *dest, const char *src)
{
#ifdef _I18N_
  int len;

  len = strlen (src);
  mbstowcs (dest, src, len + 1);
  return dest;
#else  /* _I18N_ */
  return (CHAR_T *) strcpy ((char *)dest, (char *)src);
#endif /* _I18N_ */
}


/*-------------------------------------------------------------
  ustrlen: get the length of a string.
  -------------------------------------------------------------*/
size_t        ustrlen (const CHAR_T *str)
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
  int       len1 = ustrlen (str1);
  int       len2 = ustrlen (str2);
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
	      c1 = towlower (str1[ndx]);
	      c2 = towlower (str2[ndx]);
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
CHAR_T        *ustrrchr (const CHAR_T *str, CHAR_T c)
{
#ifdef _I18N_
  /* Compatibility of wcsrchr: ANSI, WIN NT and WIN 9x */
  return (CHAR_T*) (wcsrchr ((wchar_t*)str, (int)c));
#else  /* _I18N_ */
  return (CHAR_T*) strrchr ((char*)str, (int)c);
#endif /* _I18N_ */
}


/*-------------------------------------------------------------
  ustrtok: find the next token in a string.
  -------------------------------------------------------------*/
CHAR_T       *ustrtok (CHAR_T *str, const CHAR_T *delemiter)
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


/*-------------------------------------------------------------
  wc2iso_strcpy: copies src (16-bit) into dest (8-bit). This 
  function suposes that momery has been already allocated in the 
  same way that strcpy does.
  -------------------------------------------------------------*/
char *wc2iso_strcpy (char *dest, const CHAR_T *src)
{
#ifdef _I18N_
  wcstombs (dest, src, MAX_TXT_LEN);
#else  /* _I18N_ */
  strcpy (dest, src);
#endif /* _I18N_ */
  return dest;
}


/*----------------------------------------------------------------------
  TtaGetCharset gives the charset 
  ----------------------------------------------------------------------*/
CHARSET       TtaGetCharset (const CHAR_T *charsetname)
{
  int index = 0;

  if (charsetname == NULL || charsetname[0] == 0)
    return UNDEFINED_CHARSET;
  while (CharsetCodeTable[index].Charset != UNDEFINED_CHARSET)
    {
      if (!ustrcasecmp (CharsetCodeTable[index].ISOCode, charsetname))
	return CharsetCodeTable[index].Charset;
      index++;
    }
  return UNDEFINED_CHARSET;
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
	return &(CharsetCodeTable[index].ISOCode);
      index++;
    }
  return NULL;
}















