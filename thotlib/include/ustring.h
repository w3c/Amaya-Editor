#ifndef _USTRING_H
#define _USTRING_H

#include <string.h>
#ifdef _WINGUI
#include <windows.h>
#endif /* _WINGUI */

#ifdef _WINDOWS
#define strncasecmp _strnicmp
#endif /* _WINDOWS */

typedef enum {
   UNDEFINED_CHARSET,
   US_ASCII,
   ISO_8859_1,
   ISO_8859_2, 
   ISO_8859_3, 
   ISO_8859_4, 
   ISO_8859_5,
   ISO_8859_6, 
   ISO_8859_7, 
   ISO_8859_8,
   ISO_8859_9, 
   ISO_8859_10,
   ISO_8859_13,
   ISO_8859_15,
   UNICODE_1_1,
   UNICODE_1_1_UTF_7,
   UTF_7,
   UTF_8,
   UTF_16,
   WINDOWS_1250,
   WINDOWS_1251,
   WINDOWS_1252,
   WINDOWS_1253,
   WINDOWS_1254,
   WINDOWS_1255,
   WINDOWS_1256,
   WINDOWS_1257,
   WINDOWS_1258,
   ISO_SYMBOL,
   KOI8_R,
   MAC_OS_ROMAN,
   ISO_2022_JP,
   EUC_JP,
   SHIFT_JIS,
   JIS_X_0201_ROMAN,
   JIS_X_0201_KANA,
   JIS_X_0208,
   JIS_X_0212,
   GB_2312,
   KSC_5601,
   UNSUPPORTED_CHARSET
} CHARSET;

#include <wchar.h>
typedef wchar_t  CHAR_T;
typedef wchar_t *STRING;

/*----------------------------------------------------------------------
  TtaGetCharset gives the charset 
  ----------------------------------------------------------------------*/
extern CHARSET TtaGetCharset (const char *charsetname);

/*----------------------------------------------------------------------
  TtaGetLocaleCharset returns the user system charset
  ----------------------------------------------------------------------*/
extern CHARSET TtaGetLocaleCharset ();

/*----------------------------------------------------------------------
  TtaGetDefaultCharset gives the default charset 
  ----------------------------------------------------------------------*/
extern CHARSET TtaGetDefaultCharset ();

/*----------------------------------------------------------------------
  TtaGetCharsetName gives the constant string of the charset ISO name.
  ----------------------------------------------------------------------*/
extern const char *TtaGetCharsetName (CHARSET charset);

extern int ustrcasecmp (const CHAR_T *, const CHAR_T *);
extern CHAR_T *ustrcat (CHAR_T *, const CHAR_T *);
extern CHAR_T *ustrchr (const CHAR_T *, CHAR_T);
extern int ustrcmp (const CHAR_T *, const CHAR_T *);
extern CHAR_T *ustrcpy (CHAR_T *, const CHAR_T *);
extern CHAR_T *ustrdup (const CHAR_T *);
extern size_t ustrlen (const CHAR_T *);
extern int ustrncasecmp (const CHAR_T *, const CHAR_T *, unsigned int);
extern CHAR_T *ustrncat (CHAR_T *, const CHAR_T *, unsigned int);
extern CHAR_T *ustrncmp (const CHAR_T *, const CHAR_T *, unsigned int);
extern CHAR_T *ustrncpy (CHAR_T *, const CHAR_T *, unsigned int);
extern CHAR_T *ustrrchr (const CHAR_T *, CHAR_T);
extern CHAR_T *ustrstr (const CHAR_T *, const CHAR_T *);
extern CHAR_T *ustrtok (CHAR_T *, const CHAR_T *);

#endif /* _USTRING_H */


