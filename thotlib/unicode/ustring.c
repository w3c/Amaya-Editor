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
STRING ustrcat (src, c)
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
    return (STRING) _wcsdup ((wchar_t*)str);
#   else  /* !_WINDOWS */
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
  ustrncmp: append n characters of a string src.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
STRING ustrncmp (const STRING str1, const STRING str2, unsigned int count)
#else  /* __STDC__ */
STRING ustrncat (str1, str2, count)
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
  ustrrchr: scan a string for the last occurrence of a character.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
STRING ustrrchr (const STRING str, CHAR_T c)
#else  /* __STDC__ */
STRING ustrncat (str, c)
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
STRING ustrncat (str, delemiter)
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
STRING ustrncat (str, strCharSet)
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
STRING ustrcat (src, c)
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
  ustrlen: get the length of a string.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
size_t ustrlen (const STRING str)
#else  /* __STDC__ */
int ustrlen (str)
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
  ustrncmp: append n characters of a string src.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
STRING ustrncmp (const STRING str1, const STRING str2, unsigned int count)
#else  /* __STDC__ */
STRING ustrncat (str1, str2, count)
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
  ustrrchr: scan a string for the last occurrence of a character.
  -------------------------------------------------------------*/
 
#ifdef __STDC__
STRING ustrrchr (const STRING str, CHAR_T c)
#else  /* __STDC__ */
STRING ustrncat (str, c)
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
STRING ustrncat (str, delemiter)
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
STRING ustrncat (str, strCharSet)
const STRING str;
const STRING strCharSet;
#endif /* __STDC__ */
{
    return (STRING) strstr ((char*)str, (char*)strCharSet);
}

#endif /* _I18N_ */


















