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

#ifdef _I18N_
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
#   ifdef _WINDOWS
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
#   ifdef _WINDOWS
    return (CHAR_T*) wcscat ((wchar_t*)dest, (wchar_t*)src);
#   else  /* !_WINDOWS */
#   endif /* !_WINDOWS */
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
#   ifdef _WINDOWS
    return ((CHAR_T*) wcschr ((wchar_t*)src, (wint_t)c));
#   else  /* !_WINDOWS */
#   endif /* !_WINDOWS */
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
#   ifdef _WINDOWS
    return wcscmp ((wchar_t*)str1, (wchar_t*)str2);
#   else  /* !_WINDOWS */
#   endif /* !_WINDOWS */
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
CHAR_T* ustrcpy (CHAR_T* dest, const CHAR_T* src)
#else  /* __STDC__ */
CHAR_T* ustrcpy (dest, src)
CHAR_T*       dest;
const CHAR_T* src;
#endif /* __STDC__ */
{
#   ifdef _WINDOWS
    return (CHAR_T*) wcscpy ((wchar_t*)dest, (wchar_t*)src);
#   else  /* !_WINDOWS */
#   endif /* !_WINDOWS */
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
    int i, len = strlen (src);
    for (i = 0; i < len; i++)
        dest[i] = (CHAR_T)src[i];
    dest[i] = (CHAR_T)0;
    return dest;
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
#   ifdef _WINDOWS
    return (CHAR_T*) _wcsdup (str);
#   else  /* !_WINDOWS */
#   endif /* _WINDOWS */
}

/*-------------------------------------------------------------
  iso2wc_strdup: duplicate strings.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
CHAR_T* iso2wc_strdup (const char* str)
#else  /* __STDC__ */
CHAR_T* iso2wc_strdup (str)
const char* str;
#endif /* __STDC__ */
{
    CHAR_T*  res;
    CHAR_T* tmp = (CHAR_T*) malloc ((strlen (str) + 1) * sizeof (CHAR_T));
    iso2wc_strcpy (tmp, str);
#   ifdef _WINDOWS
    res = _wcsdup ((wchar_t*)tmp);
#   else  /* !_WINDOWS */
#   endif /* _WINDOWS */
    free (tmp);
    return (CHAR_T*) res;
}

/*-------------------------------------------------------------
  wc2iso_strdup: duplicate strings.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
char* wc2iso_strdup (const CHAR_T* str)
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
size_t ustrlen (const CHAR_T* str)
#else  /* __STDC__ */
size_t ustrlen (str)
const CHAR_T* str;
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
int ustrncasecmp (const CHAR_T* str1, const CHAR_T* str2, unsigned int count)
#else  /* __STDC__ */
int ustrncasecmp (str1, str2, count)
const CHAR_T* str1;
const CHAR_T* str2;
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
CHAR_T* ustrncat (CHAR_T* dest, const CHAR_T* src, unsigned int count)
#else  /* __STDC__ */
CHAR_T* ustrncat (dest, src, count)
CHAR_T*       dest;
const CHAR_T* src;
unsigned int  count;
#endif /* __STDC__ */
{
#   ifdef _WINDOWS 
    return (CHAR_T*) wcsncat (dest, src, (size_t)count);
#   else  /* !_WINDOWS */
#   endif /* !_WINDOWS */
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
#   ifdef _WINDOWS 
    return (CHAR_T*) wcsncmp ((wchar_t*)str1, (wchar_t*)str2, (size_t)count);
#   else  /* !_WINDOWS */
#   endif /* !_WINDOWS */
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
#   ifdef _WINDOWS 
    return (CHAR_T*) wcsncpy ((wchar_t*)dest, (wchar_t*)src, (size_t)count);
#   else  /* !_WINDOWS */
#   endif /* !_WINDOWS */
}

/*-------------------------------------------------------------
  iso2wc_strncpy: copy n characters of one string (8-bit) to 
  another (16-bit).
  -------------------------------------------------------------*/
 
#ifdef __STDC__
CHAR_T* iso2wc_strncpy (CHAR_T* dest, const char* src, unsigned int count)
#else  /* __STDC__ */
CHAR_T* iso2wc_strncpy (dest, src, count)
CHAR_T*      dest;
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
CHAR_T* ustrrchr (const CHAR_T* str, CHAR_T c)
#else  /* __STDC__ */
CHAR_T* ustrrchr (str, c)
const CHAR_T* str;
CHAR_T         c;
#endif /* __STDC__ */
{
#   ifdef _WINDOWS
    return (CHAR_T*) (wcsrchr ((wchar_t*)str, (int)c));
#   else  /* !_WINDOWS */
#   endif /* !_WINDOWS */
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
#   ifdef _WINDOWS 
    return (CHAR_T*) wcstok ((wchar_t*)str, (wchar_t*)delemiter);
#   else  /* !_WINDOWS */
#   endif /* !_WINDOWS */
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
#   ifdef _WINDOWS
    return (CHAR_T*) wcsstr ((wchar_t*)str, (wchar_t*)strCharSet);
#   else  /* !_WINDOWS */
#   endif /* !_WINDOWS */
}

#else  /* !_I18N_ */

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
CHAR_T* ustrcat (CHAR_T* dest, const CHAR_T* src)
#else  /* __STDC__ */
CHAR_T* ustrcat (dest, src)
CHAR_T*       dest;
const CHAR_T* src;
#endif /* __STDC__ */
{
    return (CHAR_T*) strcat ((char*)dest, (char*)src);
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
    return ((CHAR_T*) strchr ((char*)src, (int)c));
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
    return strcmp ((char*)str1, (char*)str2);
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
    return strcoll ((char*)str1, (char*)str2);
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
    return (CHAR_T*) strcpy ((char*)dest, (char*)src);
}

/*-------------------------------------------------------------
  iso2wc_strcpy: copies src into dest. This function suposes that
  momery has been already allocated in the same way that strcpy
  does.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
CHAR_T* iso2wc_strcpy (CHAR_T* dest, const char* src)
#else  /* __STDC__ */
CHAR_T* iso2wc_strcpy (dest, src)
CHAR_T*      dest;
const char* src;
#endif /* __STDC__ */
{
    return (CHAR_T*) strcpy ((char*)dest, (char*)src);
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
#   ifdef _WINDOWS
    return (CHAR_T*) _strdup ((char*)str);
#   else  /* !_WINDOWS */
    return (CHAR_T*) strdup ((char*)str);
#   endif /* _WINDOWS */
}

/*-------------------------------------------------------------
  iso2wc_strdup: duplicate strings.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
CHAR_T* iso2wc_strdup (const char* str)
#else  /* __STDC__ */
CHAR_T* iso2wc_strdup (str)
const char* str;
#endif /* __STDC__ */
{
#   ifdef _WINDOWS
    return (CHAR_T*) _strdup (str);
#   else  /* !_WINDOWS */
    return (CHAR_T*) strdup (str);
#   endif /* _WINDOWS */
}

/*-------------------------------------------------------------
  wc2iso_strdup: duplicate strings.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
char* wc2iso_strdup (const CHAR_T* str)
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
size_t ustrlen (const CHAR_T* str)
#else  /* __STDC__ */
size_t ustrlen (str)
const CHAR_T* str;
#endif /* __STDC__ */
{
    return (size_t) strlen ((char*)str);
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
CHAR_T* ustrncat (CHAR_T* dest, const CHAR_T* src, unsigned int count)
#else  /* __STDC__ */
CHAR_T* ustrncat (dest, src, count)
CHAR_T*       dest;
const CHAR_T* src;
unsigned int  count;
#endif /* __STDC__ */
{
    return (CHAR_T*) strncat ((char*)dest, (char*)src, (size_t)count);
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
    return (CHAR_T*) strncmp ((char*)str1, (char*)str2, (size_t)count);
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
    return (CHAR_T*) strncpy ((char*)dest, (char*)src, (size_t)count);
}

/*-------------------------------------------------------------
  iso2wc_strncpy: copy n characters of one string to another.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
CHAR_T* iso2wc_strncpy (CHAR_T* dest, const char* src, unsigned int count)
#else  /* __STDC__ */
CHAR_T* iso2wc_strncpy (dest, src, count)
CHAR_T*       dest;
const char*  src;
unsigned int  count;
#endif /* __STDC__ */
{
    return (CHAR_T*) strncpy ((char*)dest, (char*)src, (size_t)count);
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
    return (CHAR_T*) strrchr ((char*)str, (int)c);
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
    return (CHAR_T*) strtok ((char*)str, (char*)delemiter);
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
    return (CHAR_T*) strstr ((char*)str, (char*)strCharSet);
}

#endif /* _I18N_ */

/*-------------------------------------------------------------
  StringCaseCompare: compare two strings (CharUnit*) without 
  regard to case.
  -------------------------------------------------------------*/
#ifdef __STDC__
int StringCaseCompare (const CharUnit* str1, const CharUnit* str2)
#else  /* !__STDC__ */
int StringCaseCompare (str1, str2)
const CharUnit* str1; 
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
     if (str == (CharUnit*)0)
        return NULL;
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
int StringNCaseCompare (const CharUnit* str1, const CharUnit* str2, unsigned int count)
#else  /* !__STDC__ */
int StringNCaseCompare (str1, str2, count)
const CharUnit* str1; 
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

/*-------------------------------------------------------------
  cus2iso_strcasecmp: compare CharUnit* string to a char* string.
  -------------------------------------------------------------*/
#ifdef __STDC__
int cus2iso_strcasecmp (const CharUnit* str1, const char* str2)
#else  /* __STDC__ */
int cus2iso_strcasecmp (str1, str2)
const CharUnit* str1;
const char*     str2;
#endif /* __STDC__ */
{
    int       diff;
    CharUnit* cus_str2 = (CharUnit*) malloc ((strlen (str2) + 1) * sizeof (CharUnit));

    iso2cus_strcpy (cus_str2, str2);
    diff = StringCaseCompare (str1, cus_str2);

    free (cus_str2);
    return diff;
}

/*-------------------------------------------------------------
  iso2cus_strcasecmp: compare char* string to a CharUnit* string.
  -------------------------------------------------------------*/
#ifdef __STDC__
int iso2cus_strcasecmp (const char* str1, const CharUnit* str2)
#else  /* __STDC__ */
int iso2cus_strcasecmp (str1, str2)
const CharUnit* str1;
const char*     str2;
#endif /* __STDC__ */
{
    return cus2iso_strcasecmp (str2, str1);
}

/*-------------------------------------------------------------
  cus2iso_strcmp: compare a CharUnit* string to a char* string.
  The first arg (str1) must be CharUnit*
  The second arg mest be char*
  -------------------------------------------------------------*/
 
#ifdef __STDC__
int cus2iso_strcmp (const CharUnit* str1, const char* str2)
#else  /* __STDC__ */
int cus2iso_strcmp (str1, str2)
const CharUnit* str1;
const char*     str2;
#endif /* __STDC__ */
{
    int       diff;
    CharUnit* cus_str2 = (CharUnit*) malloc ((strlen (str2) + 1) * sizeof (CharUnit));

    iso2cus_strcpy (cus_str2, str2);
    diff = StringCompare (str1, cus_str2);

    free (cus_str2);
    return diff;
}

/*-------------------------------------------------------------
  iso2cus_strcmp: compares a char* string to a CharUnit* string.
  The first arg (str1) must be char*
  The second arg mest be CharUnit*
  -------------------------------------------------------------*/
 
#ifdef __STDC__
int iso2cus_strcmp (const char* str1, const CharUnit* str2)
#else  /* __STDC__ */
int iso2cus_strcmp (str1, str2)
const char*     str1;
const CharUnit* str2;
#endif /* __STDC__ */
{
    return cus2iso_strcmp (str2, str1);
}

/*-------------------------------------------------------------
  iso2cus_strcpy: copies src (8-bit) into dest (CharUnit*). This 
  function suposes that momery has been already allocated in the 
  same way that strcpy does.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
CharUnit* iso2cus_strcpy (CharUnit* dest, const char* src)
#else  /* __STDC__ */
CharUnit* iso2cus_strcpy (dest, src)
CharUnit* dest;
const char* src;
#endif /* __STDC__ */
{
    int i, len = strlen (src);
    for (i = 0; i < len; i++)
        dest[i] = (CharUnit)src[i];
    dest[i] = (CharUnit)0;
    return dest;
}

/*-------------------------------------------------------------
  cus2iso_strcpy: copies src (CharUnit*) into dest (8-bit). This 
  function suposes that momery has been already allocated in the 
  same way that strcpy does.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
char* cus2iso_strcpy (char* dest, const CharUnit* src)
#else  /* __STDC__ */
char* cus2iso_strcpy (dest, src)
char*           dest;
const CharUnit* src;
#endif /* __STDC__ */
{
    int i, len = StringLength (src);
    for (i = 0; i < len; i++)
        dest[i] = (char)src[i];
    dest[i] = (char)0;
    return dest;
}

/*-------------------------------------------------------------
  iso2cus_strncpy: copy n characters of a char* string to 
  a CharUnit* string.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
CharUnit* iso2cus_strncpy (CharUnit* dest, const char* src, unsigned int count)
#else  /* __STDC__ */
CharUnit* iso2cus_strncpy (dest, src, count)
CharUnit*   dest;
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
        dest[i] = (CharUnit) src[i];
    dest[i] = (CharUnit)0;
    return dest;
}

/*-------------------------------------------------------------
  cus2iso_strncpy: copy n characters of a char* string to 
  a CharUnit* string.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
char* cus2iso_strncpy (char* dest, const CharUnit* src, unsigned int count)
#else  /* __STDC__ */
char* cus2iso_strncpy (dest, src, count)
char*           dest;
const CharUnit* src;
unsigned int  count;
#endif /* __STDC__ */
{
	int i, len = StringLength (src);
    int cnt;
    if (len < count)
       cnt = len;
    else 
        cnt = count;

    for (i = 0; i < cnt; i++)
        dest[i] = (char) src[i];
    dest[i] = (char)0;
    return dest;
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
    int i, len = ustrlen (src);
    for (i = 0; i < len; i++)
        dest[i] = (char)src[i];
    dest[i] = (char)0;
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
  wc2cus_strcpy: copies src (CHAR_T*) into dest (CharUnit*). This 
  function suposes that momery has been already allocated in the 
  same way that strcpy does.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
CharUnit* wc2cus_strcpy (CharUnit* dest, const CHAR_T* src)
#else  /* __STDC__ */
CharUnit* wc2cus_strcpy (dest, src)
CharUnit*     dest;
const CHAR_T*  src;
#endif /* __STDC__ */
{
    int i, len = ustrlen ((CHAR_T*)src);

    for (i = 0; i < len; i++)
        dest[i] = (CharUnit)src[i];
    dest[i] = (CharUnit)0;
    return dest;
}

/*-------------------------------------------------------------
  cus2wc_strcpy: copies src (CharUnit*) into dest (CHAR_T*). This 
  function suposes that momery has been already allocated in the 
  same way that strcpy does.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
CHAR_T* cus2wc_strcpy (CHAR_T* dest, const CharUnit* src)
#else  /* __STDC__ */
CHAR_T* cus2wc_strcpy (dest, src)
CHAR_T*         dest;
const CharUnit* src;
#endif /* __STDC__ */
{
    int i, len = StringLength (src);

    for (i = 0; i < len; i++)
        dest[i] = (CHAR_T)src[i];
    dest[i] = (CHAR_T)0;
    return dest;
}

/*-------------------------------------------------------------
  wc2iso_strcmp: compare a CharUnit* string to a char* string.
  The first arg (str1) must be CharUnit*
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















