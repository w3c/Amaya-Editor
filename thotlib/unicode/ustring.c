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

#ifdef _I18N_
/*-------------------------------------------------------------
  ustrcasecmp: compare two strings without regard to case.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
int ustrcasecmp (STRING str1, const STRING str2)
#else  /* __STDC__ */
int ustrcasecmp (str1, str2)
const STRING str1;
const STRING str2;
#endif /* __STDC__ */
{
#   ifdef _WINDOWS
    return (int) _wcsicmp ((wchar_t*)str1, (wchar_t*)str2);
#   else  /* !_WINDOWS */
#   endif /* !_WINDOWS */
}

/*-------------------------------------------------------------
  ustrcat: appends src to the content of dest. This function 
  suposes that momery has been already allocated in the same 
  way that strcat does.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
STRING ustrcat (STRING dest, const STRING src)
#else  /* __STDC__ */
STRING ustrcat (dest, src)
STRING       dest;
const STRING src;
#endif /* __STDC__ */
{
#   ifdef _WINDOWS
    return (STRING) wcscat ((wchar_t*)dest, (wchar_t*)src);
#   else  /* !_WINDOWS */
#   endif /* !_WINDOWS */
}

/*-------------------------------------------------------------
  ustrchr: Find a character in a string.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
STRING ustrchr (const STRING src, CHAR_T c)
#else  /* __STDC__ */
STRING ustrchr (src, c)
const STRING src;
CHAR_T         c;
#endif /* __STDC__ */
{
#   ifdef _WINDOWS
    return ((STRING) wcschr ((wchar_t*)src, (wint_t)c));
#   else  /* !_WINDOWS */
#   endif /* !_WINDOWS */
}

/*-------------------------------------------------------------
  ustrcmp: compare strings.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
int ustrcmp (const STRING str1, const STRING str2)
#else  /* __STDC__ */
int ustrcmp (str1, str2)
const STRING str1;
const STRING str2;
#endif /* __STDC__ */
{
#   ifdef _WINDOWS
    return wcscmp ((wchar_t*)str1, (wchar_t*)str2);
#   else  /* !_WINDOWS */
#   endif /* !_WINDOWS */
}

/*-------------------------------------------------------------
  ustrcoll: compre strings using local-specific information.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
int ustrcoll (const STRING str1, const STRING str2)
#else  /* __STDC__ */
int ustrcoll (str1, str2)
const STRING str1;
const STRING str2;
#endif /* __STDC__ */
{
#   ifdef _WINDOWS
    return wcscoll ((wchar_t*)str1, (wchar_t*)str2);
#   else  /* !_WINDOWS */
#   endif /* !_WINDOWS */
}

/*-------------------------------------------------------------
  ustrcpy: copies src into dest. This function suposes that
  momery has been already allocated in the same way that strcpy
  does.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
STRING ustrcpy (STRING dest, const STRING src)
#else  /* __STDC__ */
STRING ustrcpy (dest, src)
STRING       dest;
const STRING src;
#endif /* __STDC__ */
{
#   ifdef _WINDOWS
    return (STRING) wcscpy ((wchar_t*)dest, (wchar_t*)src);
#   else  /* !_WINDOWS */
#   endif /* !_WINDOWS */
}

/*-------------------------------------------------------------
  iso2wc_strcpy: copies src (8-bit) into dest (16-bit). This 
  function suposes that momery has been already allocated in the 
  same way that strcpy does.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
STRING iso2wc_strcpy (STRING dest, const char* src)
#else  /* __STDC__ */
STRING iso2wc_strcpy (dest, src)
STRING      dest;
const char* src;
#endif /* __STDC__ */
{
    int i, len = strlen (src);
    for (i = 0; i < len; i++)
        dest[i] = (CHAR_T)src[i];
    dest[i] = (CHAR_T)0;
    return dest;
}

/*-------------------------------------------------------------
  wc2iso_strcpy: copies src (16-bit) into dest (8-bit). This 
  function suposes that momery has been already allocated in the 
  same way that strcpy does.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
char* wc2iso_strcpy (char* dest, const STRING src)
#else  /* __STDC__ */
char* wc2iso_strcpy (dest, src)
char*         dest;
const CHAR_T* src;
#endif /* __STDC__ */
{
    int i, len = ustrlen (src);
    for (i = 0; i < len; i++)
        dest[i] = (char)src[i];
    dest[i] = (char)0;
    return dest;
}

/*-------------------------------------------------------------
  ustrdup: duplicate strings.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
STRING ustrdup (const STRING str)
#else  /* __STDC__ */
STRING ustrdup (str)
const STRING str;
#endif /* __STDC__ */
{
#   ifdef _WINDOWS
    return (STRING) _wcsdup (str);
#   else  /* !_WINDOWS */
#   endif /* _WINDOWS */
}

/*-------------------------------------------------------------
  iso2wc_strdup: duplicate strings.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
STRING iso2wc_strdup (const char* str)
#else  /* __STDC__ */
STRING iso2wc_strdup (str)
const char* str;
#endif /* __STDC__ */
{
    STRING  res;
    CHAR_T* tmp = (CHAR_T*) malloc ((strlen (str) + 1) * sizeof (CHAR_T));
    iso2wc_strcpy (tmp, str);
#   ifdef _WINDOWS
    res = _wcsdup ((wchar_t*)tmp);
#   else  /* !_WINDOWS */
#   endif /* _WINDOWS */
    free (tmp);
    return (STRING) res;
}

/*-------------------------------------------------------------
  wc2iso_strdup: duplicate strings.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
char* wc2iso_strdup (const STRING str)
#else  /* __STDC__ */
char* wc2iso_strdup (str)
const char* str;
#endif /* __STDC__ */
{
    char*  res;
    char* tmp = (char*) malloc (ustrlen (str) + 1);
    wc2iso_strcpy (tmp, str);
#   ifdef _WINDOWS
    res = _strdup (tmp);
#   else  /* !_WINDOWS */
#   endif /* _WINDOWS */
    free (tmp);
    return res;
}


/*-------------------------------------------------------------
  ustrlen: get the length of a string.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
size_t ustrlen (const STRING str)
#else  /* __STDC__ */
size_t ustrlen (str)
const STRING str;
#endif /* __STDC__ */
{
#   ifdef _WINDOWS
    return wcslen ((wchar_t*)str);
#   else  /* !_WINDOWS */
#   endif /* !_WINDOWS */
}

/*-------------------------------------------------------------
  ustrncasecmp: compare characters of two strings without regard
                to case.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
int ustrncasecmp (STRING str1, const STRING str2, unsigned int count)
#else  /* __STDC__ */
int ustrncasecmp (str1, str2, count)
const STRING str1;
const STRING str2;
unsigned int  count;
#endif /* __STDC__ */
{
#   ifdef _WINDOWS
    return (unsigned int) _wcsnicmp ((wchar_t*)str1, (wchar_t*)str2, (size_t)count);
#   else  /* !_WINDOWS */
#   endif /* !_WINDOWS */
}

/*-------------------------------------------------------------
  ustrncat: append n characters of a string src.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
STRING ustrncat (STRING dest, const STRING src, unsigned int count)
#else  /* __STDC__ */
STRING ustrncat (dest, src, count)
STRING       dest;
const STRING src;
unsigned int  count;
#endif /* __STDC__ */
{
#   ifdef _WINDOWS 
    return (STRING) wcsncat (dest, src, (size_t)count);
#   else  /* !_WINDOWS */
#   endif /* !_WINDOWS */
}

/*-------------------------------------------------------------
  ustrncmp: compare n characters of str1 and str2.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
STRING ustrncmp (const STRING str1, const STRING str2, unsigned int count)
#else  /* __STDC__ */
STRING ustrncmp (str1, str2, count)
const STRING str1;
const STRING str2;
unsigned int  count;
#endif /* __STDC__ */
{
#   ifdef _WINDOWS 
    return (STRING) wcsncmp ((wchar_t*)str1, (wchar_t*)str2, (size_t)count);
#   else  /* !_WINDOWS */
#   endif /* !_WINDOWS */
}

/*-------------------------------------------------------------
  ustrncpy: copy n characters of one string to another.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
STRING ustrncpy (STRING dest, const STRING src, unsigned int count)
#else  /* __STDC__ */
STRING ustrncpy (dest, src, count)
STRING       dest;
const STRING src;
unsigned int  count;
#endif /* __STDC__ */
{
#   ifdef _WINDOWS 
    return (STRING) wcsncpy ((wchar_t*)dest, (wchar_t*)src, (size_t)count);
#   else  /* !_WINDOWS */
#   endif /* !_WINDOWS */
}

/*-------------------------------------------------------------
  iso2wc_strncpy: copy n characters of one string (8-bit) to 
  another (16-bit).
  -------------------------------------------------------------*/
 
#ifdef __STDC__
STRING iso2wc_strncpy (STRING dest, const char* src, unsigned int count)
#else  /* __STDC__ */
STRING iso2wc_strncpy (dest, src, count)
STRING      dest;
const char* src;
unsigned int  count;
#endif /* __STDC__ */
{
	int i, len = strlen (src);
    int cnt;
    if (len < count)
       cnt = len;
    else 
        cnt = count;
    for (i = 0; i < cnt; i++)
        dest [i] = (CHAR_T) src[i];
    dest[i] = (CHAR_T)0;
    return dest;
}

/*-------------------------------------------------------------
  ustrrchr: scan a string for the last occurrence of a character.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
STRING ustrrchr (const STRING str, CHAR_T c)
#else  /* __STDC__ */
STRING ustrrchr (str, c)
const STRING str;
CHAR_T         c;
#endif /* __STDC__ */
{
#   ifdef _WINDOWS
    return (STRING) (wcsrchr ((wchar_t*)str, (int)c));
#   else  /* !_WINDOWS */
#   endif /* !_WINDOWS */
}

/*-------------------------------------------------------------
  ustrtok: find the next token in a string.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
STRING ustrtok (STRING str, const STRING delemiter)
#else  /* __STDC__ */
STRING ustrtok (str, delemiter)
STRING       str;
const STRING delemiter;
#endif /* __STDC__ */
{
#   ifdef _WINDOWS 
    return (STRING) wcstok ((wchar_t*)str, (wchar_t*)delemiter);
#   else  /* !_WINDOWS */
#   endif /* !_WINDOWS */
}

/*-------------------------------------------------------------
  ustrstr: find a substring.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
STRING ustrstr (const STRING str, const STRING strCharSet)
#else  /* __STDC__ */
STRING ustrstr (str, strCharSet)
const STRING str;
const STRING strCharSet;
#endif /* __STDC__ */
{
#   ifdef _WINDOWS
    return (STRING) wcsstr ((wchar_t*)str, (wchar_t*)strCharSet);
#   else  /* !_WINDOWS */
#   endif /* !_WINDOWS */
}

#else  /* !_I18N_ */

/*-------------------------------------------------------------
  ustrcasecmp: compare two strings without regard to case.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
int ustrcasecmp (STRING str1, const STRING str2)
#else  /* __STDC__ */
int ustrcasecmp (str1, str2)
const STRING str1;
const STRING str2;
#endif /* __STDC__ */
{
#   ifdef _WINDOWS
    return (unsigned int) _stricmp ((char*)str1, (char*)str2);
#   else  /* !_WINDOWS */
    return (unsigned int) strcasecmp ((char*)str1, (char*)str2);
#   endif /* !_WINDOWS */
}

/*-------------------------------------------------------------
  ustrcat: appends src to the content of dest. This function 
  suposes that momery has been already allocated in the same 
  way that strcat does.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
STRING ustrcat (STRING dest, const STRING src)
#else  /* __STDC__ */
STRING ustrcat (dest, src)
STRING       dest;
const STRING src;
#endif /* __STDC__ */
{
    return (STRING) strcat ((char*)dest, (char*)src);
}

/*-------------------------------------------------------------
  ustrchr: Find a character in a string.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
STRING ustrchr (const STRING src, CHAR_T c)
#else  /* __STDC__ */
STRING ustrchr (src, c)
const STRING src;
CHAR_T         c;
#endif /* __STDC__ */
{
    return ((STRING) strchr ((char*)src, (int)c));
}

/*-------------------------------------------------------------
  ustrcmp: compare strings.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
int ustrcmp (const STRING str1, const STRING str2)
#else  /* __STDC__ */
int ustrcmp (str1, str2)
const STRING str1;
const STRING str2;
#endif /* __STDC__ */
{
    return strcmp ((char*)str1, (char*)str2);
}

/*-------------------------------------------------------------
  ustrcoll: compre strings using local-specific information.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
int ustrcoll (const STRING str1, const STRING str2)
#else  /* __STDC__ */
int ustrcoll (str1, str2)
const STRING str1;
const STRING str2;
#endif /* __STDC__ */
{
    return strcoll ((char*)str1, (char*)str2);
}

/*-------------------------------------------------------------
  ustrcpy: copies src into dest. This function suposes that
  momery has been already allocated in the same way that strcpy
  does.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
STRING ustrcpy (STRING dest, const STRING src)
#else  /* __STDC__ */
STRING ustrcpy (dest, src)
STRING       dest;
const STRING src;
#endif /* __STDC__ */
{
    return (STRING) strcpy ((char*)dest, (char*)src);
}

/*-------------------------------------------------------------
  iso2wc_strcpy: copies src into dest. This function suposes that
  momery has been already allocated in the same way that strcpy
  does.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
STRING iso2wc_strcpy (STRING dest, const char* src)
#else  /* __STDC__ */
STRING iso2wc_strcpy (dest, src)
STRING      dest;
const char* src;
#endif /* __STDC__ */
{
    return (STRING) strcpy ((char*)dest, (char*)src);
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
const CHAR_T* src;
#endif /* __STDC__ */
{
    return (char*) strcpy ((char*)dest, (char*)src);
}

/*-------------------------------------------------------------
  ustrdup: duplicate strings.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
STRING ustrdup (const STRING str)
#else  /* __STDC__ */
STRING ustrdup (str)
const STRING str;
#endif /* __STDC__ */
{
#   ifdef _WINDOWS
    return (STRING) _strdup ((char*)str);
#   else  /* !_WINDOWS */
    return (STRING) strdup ((char*)str);
#   endif /* _WINDOWS */
}

/*-------------------------------------------------------------
  iso2wc_strdup: duplicate strings.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
STRING iso2wc_strdup (const char* str)
#else  /* __STDC__ */
STRING iso2wc_strdup (str)
const char* str;
#endif /* __STDC__ */
{
#   ifdef _WINDOWS
    return (STRING) _strdup (str);
#   else  /* !_WINDOWS */
    return (STRING) strdup (str);
#   endif /* _WINDOWS */
}

/*-------------------------------------------------------------
  wc2iso_strdup: duplicate strings.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
char* wc2iso_strdup (const STRING str)
#else  /* __STDC__ */
char* wc2iso_strdup (str)
const char* str;
#endif /* __STDC__ */
{
#   ifdef _WINDOWS
    return (char*) _strdup ((char*)str);
#   else  /* !_WINDOWS */
    return (char*) strdup ((char*)str);
#   endif /* _WINDOWS */
}

/*-------------------------------------------------------------
  ustrlen: get the length of a string.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
size_t ustrlen (const STRING str)
#else  /* __STDC__ */
size_t ustrlen (str)
const STRING str;
#endif /* __STDC__ */
{
    return (size_t) strlen ((char*)str);
}

/*-------------------------------------------------------------
  ustrncasecmp: compare characters of two strings without regard
                to case.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
int ustrncasecmp (STRING str1, const STRING str2, unsigned int count)
#else  /* __STDC__ */
int ustrncasecmp (str1, str2, count)
const STRING str1;
const STRING str2;
unsigned int  count;
#endif /* __STDC__ */
{
#   ifdef _WINDOWS
    return (unsigned int) _strnicmp ((char*)str1, (char*)str2, (size_t)count);
#   else  /* !_WINDOWS */
    return (unsigned int) strncasecmp ((char*)str1, (char*)str2, (size_t)count);
#   endif /* !_WINDOWS */
}

/*-------------------------------------------------------------
  ustrncat: append n characters of a string src.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
STRING ustrncat (STRING dest, const STRING src, unsigned int count)
#else  /* __STDC__ */
STRING ustrncat (dest, src, count)
STRING       dest;
const STRING src;
unsigned int  count;
#endif /* __STDC__ */
{
    return (STRING) strncat ((char*)dest, (char*)src, (size_t)count);
}

/*-------------------------------------------------------------
  ustrncmp: compare n characters of str1 and str2.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
STRING ustrncmp (const STRING str1, const STRING str2, unsigned int count)
#else  /* __STDC__ */
STRING ustrncmp (str1, str2, count)
const STRING str1;
const STRING str2;
unsigned int  count;
#endif /* __STDC__ */
{
    return (STRING) strncmp ((char*)str1, (char*)str2, (size_t)count);
}

/*-------------------------------------------------------------
  ustrncpy: copy n characters of one string to another.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
STRING ustrncpy (STRING dest, const STRING src, unsigned int count)
#else  /* __STDC__ */
STRING ustrncpy (dest, src, count)
STRING       dest;
const STRING src;
unsigned int  count;
#endif /* __STDC__ */
{
    return (STRING) strncpy ((char*)dest, (char*)src, (size_t)count);
}

/*-------------------------------------------------------------
  iso2wc_strncpy: copy n characters of one string to another.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
STRING iso2wc_strncpy (STRING dest, const STRING src, unsigned int count)
#else  /* __STDC__ */
STRING iso2wc_strncpy (dest, src, count)
STRING       dest;
const STRING src;
unsigned int  count;
#endif /* __STDC__ */
{
    return (STRING) strncpy ((char*)dest, (char*)src, (size_t)count);
}

/*-------------------------------------------------------------
  ustrrchr: scan a string for the last occurrence of a character.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
STRING ustrrchr (const STRING str, CHAR_T c)
#else  /* __STDC__ */
STRING ustrrchr (str, c)
const STRING str;
CHAR_T         c;
#endif /* __STDC__ */
{
    return (STRING) strrchr ((char*)str, (int)c);
}

/*-------------------------------------------------------------
  ustrtok: find the next token in a string.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
STRING ustrtok (STRING str, const STRING delemiter)
#else  /* __STDC__ */
STRING ustrtok (str, delemiter)
STRING       str;
const STRING delemiter;
#endif /* __STDC__ */
{
    return (STRING) strtok ((char*)str, (char*)delemiter);
}

/*-------------------------------------------------------------
  ustrstr: find a substring.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
STRING ustrstr (const STRING str, const STRING strCharSet)
#else  /* __STDC__ */
STRING ustrstr (str, strCharSet)
const STRING str;
const STRING strCharSet;
#endif /* __STDC__ */
{
    return (STRING) strstr ((char*)str, (char*)strCharSet);
}

#endif /* _I18N_ */

/*-------------------------------------------------------------
  StringCaseCompare: compare two strings (CharUnit*) without 
  regard to case.
  -------------------------------------------------------------*/
#ifdef __STDC__
int StringCaseCompare (CharUnit* str1, const CharUnit* str2)
#else  /* !__STDC__ */
int StringCaseCompare (str1, str2)
CharUnit*       str1; 
const CharUnit* str2;
#endif /* !__STDC__ */
{
#    if defined(_WINDOWS) && defined(_I18N_)
     return (int) _wcsicmp ((wchar_t*)str1, (wchar_t*)str2);
#    else  /* (_WINDOWS) && defined(_I18N_) */
#    ifdef _WINDOWS
     return (unsigned int) _stricmp ((char*)str1, (char*)str2);
#    else  /* !_WINDOWS */
     return (unsigned int) strcasecmp ((char*)str1, (char*)str2);
#    endif /* !_WINDOWS */
#    endif /* (_WINDOWS) && defined(_I18N_) */
}

/*-------------------------------------------------------------
  StringConcat: appends src (CharUnit*) to the content of dest
  (CharUnit*). This function suposes that momery has been already 
  allocated in the same way that strcat does.
  -------------------------------------------------------------*/
#ifdef __STDC__
CharUnit* StringConcat (CharUnit* dest, const CharUnit* src)
#else  /* !__STDC__ */
CharUnit* StringConcat (dest, src)
CharUnit*       dest; 
const CharUnit* src;
#endif /* !__STDC__ */
{
#    if defined(_WINDOWS) && defined(_I18N_)
     return (CharUnit*) wcscat ((wchar_t*)dest, (wchar_t*)src);
#    else  /* (_WINDOWS) && defined(_I18N_) */
     return (CharUnit*) strcat ((char*)dest, (char*)src);
#    endif /* (_WINDOWS) && defined(_I18N_) */
}

/*-------------------------------------------------------------
  StrChr: Find a character (CharUnit) in a string (CharUnit*).
  -------------------------------------------------------------*/
#ifdef __STDC__
CharUnit* StrChr (const CharUnit* src, CharUnit c)
#else  /* !__STDC__ */
CharUnit* StrChr (src, c)
const CharUnit* src; 
CharUnit        c;
#endif /* !__STDC__ */
{
#    if defined(_WINDOWS) && defined(_I18N_)
     return ((CharUnit*) wcschr ((wchar_t*)src, (wint_t)c));
#    else  /* (_WINDOWS) && defined(_I18N_) */
     return ((CharUnit*) strchr ((char*)src, (int)c));
#    endif /* (_WINDOWS) && defined(_I18N_) */
}

/*-------------------------------------------------------------
  StringCompare: compare strings (CharUnit*).
  -------------------------------------------------------------*/
#ifdef __STDC__
int StringCompare (const CharUnit* str1, const CharUnit* str2)
#else  /* !__STDC__ */
int StringCompare (str1, str2)
const CharUnit* str1; 
const CharUnit* str2;
#endif /* !__STDC__ */
{
#    if defined(_WINDOWS) && defined(_I18N_)
     return wcscmp ((wchar_t*)str1, (wchar_t*)str2);
#    else  /* (_WINDOWS) && defined(_I18N_) */
     return strcmp ((char*)str1, (char*)str2);
#    endif /* (_WINDOWS) && defined(_I18N_) */
}

/*-------------------------------------------------------------
  StrColl: compre strings (CharUnit*) using local-specific 
  information.
  -------------------------------------------------------------*/
#ifdef __STDC__
int StrColl (const CharUnit* str1, const CharUnit* str2)
#else  /* !__STDC__ */
int ustrcoll (str1, str2)
const CharUnit* str1; 
const CharUnit* str2;
#endif /* !__STDC__ */
{
#    if defined(_WINDOWS) && defined(_I18N_)
     return wcscoll ((wchar_t*)str1, (wchar_t*)str2);
#    else  /* (_WINDOWS) && defined(_I18N_) */
     return strcoll ((char*)str1, (char*)str2);
#    endif /* (_WINDOWS) && defined(_I18N_) */
}

/*-------------------------------------------------------------
  StringCopy: copies src (CharUnit*) into dest (CharUnit*). This 
  function suposes that memory has been already allocated in the 
  same way that strcpy does.
  -------------------------------------------------------------*/
#ifdef __STDC__
CharUnit* StringCopy (CharUnit* dest, const CharUnit* src)
#else  /* !__STDC__ */
CharUnit* StringCopy (dest, src)
CharUnit*       dest; 
const CharUnit* src;
#endif /* !__STDC__ */
{
#    if defined(_WINDOWS) && defined(_I18N_)
     return (CharUnit*) wcscpy ((wchar_t*)dest, (wchar_t*)src);
#    else  /* (_WINDOWS) && defined(_I18N_) */
     return (CharUnit*) strcpy ((char*)dest, (char*)src);
#    endif /* (_WINDOWS) && defined(_I18N_) */
}

/*-------------------------------------------------------------
  StringDuplicate: duplicate strings (CharUnit*).
  -------------------------------------------------------------*/
#ifdef __STDC__
CharUnit* StringDuplicate (const CharUnit* str)
#else  /* !__STDC__ */
CharUnit* StringDuplicate (str)
const CharUnit* str;
#endif /* !__STDC__ */
{
#    if defined(_WINDOWS) && defined(_I18N_)
     return (CharUnit*) _wcsdup ((wchar_t*)str);
#    else  /* (_WINDOWS) && defined(_I18N_) */
#    ifdef _WINDOWS
     return (CharUnit*) _strdup ((char*)str);
#    else  /* !_WINDOWS */
     return (CharUnit*) strdup ((char*)str);
#    endif /* _WINDOWS */
#    endif /* (_WINDOWS) && defined(_I18N_) */
}

/*-------------------------------------------------------------
  StringLength: get the length of a string (CharUnit*).
  -------------------------------------------------------------*/
#ifdef __STDC__
size_t StringLength (const CharUnit* str)
#else  /* !__STDC__ */
size_t StringLength (str)
const CharUnit* str;
#endif /* !__STDC__ */
{
#    if defined(_WINDOWS) && defined(_I18N_)
     return wcslen ((wchar_t*)str);
#    else  /* (_WINDOWS) && defined(_I18N_) */
     return (size_t) strlen ((char*)str);
#    endif /* (_WINDOWS) && defined(_I18N_) */
}

/*-------------------------------------------------------------
  StringNCaseCompare: compare characters of two strings (CharUnit*)
  without regard to case.
  -------------------------------------------------------------*/
#ifdef __STDC__
int StringNCaseCompare (CharUnit* str1, const CharUnit* str2, unsigned int count)
#else  /* !__STDC__ */
int StringNCaseCompare (str1, str2, count)
CharUnit*       str1; 
const CharUnit* str2; 
unsigned int    count;
#endif /* !__STDC__ */
{
#    if defined(_WINDOWS) && defined(_I18N_)
     return (unsigned int) _wcsnicmp ((wchar_t*)str1, (wchar_t*)str2, (size_t)count);
#    else  /* (_WINDOWS) && defined(_I18N_) */
#    ifdef _WINDOWS
     return (unsigned int) _strnicmp ((char*)str1, (char*)str2, (size_t)count);
#    else  /* !_WINDOWS */
     return (unsigned int) strncasecmp ((char*)str1, (char*)str2, (size_t)count);
#    endif /* !_WINDOWS */
#    endif /* (_WINDOWS) && defined(_I18N_) */
}

/*-------------------------------------------------------------
  StringNConcat: append n characters of a string src (CharUnit*).
  -------------------------------------------------------------*/
#ifdef __STDC__
CharUnit* StringNConcat (CharUnit* dest, const CharUnit* src, unsigned int count)
#else  /* !__STDC__ */
CharUnit* StringNConcat (dest, src, count)
CharUnit*       dest; 
const CharUnit* src; 
unsigned int    count;
#endif /* !__STDC__ */
{
#    if defined(_WINDOWS) && defined(_I18N_)
     return (CharUnit*) wcsncat (dest, src, (size_t)count);
#    else  /* (_WINDOWS) && defined(_I18N_) */
     return (CharUnit*) strncat ((char*)dest, (char*)src, (size_t)count);
#    endif /* (_WINDOWS) && defined(_I18N_) */
}

/*-------------------------------------------------------------
  StringNCompare: compare n characters of str1 and str2 (CharUnit*).
  -------------------------------------------------------------*/
#ifdef __STDC__
CharUnit* StringNCompare (const CharUnit* str1, const CharUnit* str2, unsigned int count)
#else  /* !__STDC__ */
CharUnit* StringNCompare (str1, str2, count)
const CharUnit* str1; 
const CharUnit* str2; 
unsigned int    count;
#endif /* !__STDC__ */
{
#    if defined(_WINDOWS) && defined(_I18N_)
     return (CharUnit*) wcsncmp ((wchar_t*)str1, (wchar_t*)str2, (size_t)count);
#    else  /* (_WINDOWS) && defined(_I18N_) */
     return (CharUnit*) strncmp ((char*)str1, (char*)str2, (size_t)count);
#    endif /* (_WINDOWS) && defined(_I18N_) */
}

/*-------------------------------------------------------------
  StringNCopy: copy n characters of one string to another (CharUnit*).
  -------------------------------------------------------------*/
#ifdef __STDC__
CharUnit* StringNCopy (CharUnit* dest, const CharUnit* src, unsigned int count)
#else  /* !__STDC__ */
CharUnit* StringNCopy (dest, src, count)
CharUnit*       dest; 
const CharUnit* src; 
unsigned int    count;
#endif /* !__STDC__ */
{
#    if defined(_WINDOWS) && defined(_I18N_)
     return (CharUnit*) wcsncpy ((wchar_t*)dest, (wchar_t*)src, (size_t)count);
#    else  /* (_WINDOWS) && defined(_I18N_) */
     return (CharUnit*) strncpy ((char*)dest, (char*)src, (size_t)count);
#    endif /* (_WINDOWS) && defined(_I18N_) */
}

/*-------------------------------------------------------------
  StrRChr: scan a string (CharUnit*) for the last occurrence of 
  a character (CharUnit).
  -------------------------------------------------------------*/
#ifdef __STDC__
CharUnit* StrRChr (const CharUnit* str, CharUnit c)
#else  /* !__STDC__ */
CharUnit* StrRChr (str, c)
const CharUnit* str; 
CharUnit        c;
#endif /* !__STDC__ */
{
#    if defined(_WINDOWS) && defined(_I18N_)
     return (CharUnit*) (wcsrchr ((wchar_t*)str, (int)c));
#    else  /* (_WINDOWS) && defined(_I18N_) */
     return (CharUnit*) strrchr ((char*)str, (int)c);
#    endif /* (_WINDOWS) && defined(_I18N_) */
}

/*-------------------------------------------------------------
  StringToken: find the next token in a string (CharUnit*).
  -------------------------------------------------------------*/
#ifdef __STDC__
CharUnit* StringToken (CharUnit* str, const CharUnit* delemiter)
#else  /* !__STDC__ */
CharUnit* StringToken (str, delemiter)
CharUnit*       str; 
const CharUnit* delemiter;
#endif /* !__STDC__ */
{
#    if defined(_WINDOWS) && defined(_I18N_)
     return (CharUnit*) wcstok ((wchar_t*)str, (wchar_t*)delemiter);
#    else  /* (_WINDOWS) && defined(_I18N_) */
     return (CharUnit*) strtok ((char*)str, (char*)delemiter);
#    endif /* (_WINDOWS) && defined(_I18N_) */
}

/*-------------------------------------------------------------
  StringSubstring: find a substring (CharUnit*).
  -------------------------------------------------------------*/
#ifdef __STDC__
CharUnit* StringSubstring (const CharUnit* str, const CharUnit* strCharSet)
#else  /* !__STDC__ */
CharUnit* StringSubstring (str, strCharSet)
const CharUnit* str; 
const CharUnit* strCharSet;
#endif /* !__STDC__ */
{
#    if defined(_WINDOWS) && defined(_I18N_)
     return (CharUnit*) wcsstr ((wchar_t*)str, (wchar_t*)strCharSet);
#    else  /* (_WINDOWS) && defined(_I18N_) */
     return (CharUnit*) strstr ((char*)str, (char*)strCharSet);
#    endif /* (_WINDOWS) && defined(_I18N_) */
}














