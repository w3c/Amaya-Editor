#ifndef _USTRING_H
#define _USTRING_H

#include <string.h>

#ifdef _WINDOWS
#include <windows.h>
#include <wchar.h>
#endif /* _WINDOWS */

#ifdef _WINDOWS
#define strncasecmp _strnicmp
#endif /* _WINDOWS */

#ifdef _I18N_

typedef wchar_t  CHAR_T;
typedef wchar_t* PCHAR_T;
typedef wchar_t  UCHAR_T;
typedef wchar_t* PUCHAR_T;
typedef wchar_t* STRING;
typedef wchar_t* USTRING;

#else  /* _I18N_ */

typedef char            CHAR_T;
typedef char*           PCHAR_T;
typedef unsigned char   UCHAR_T;
typedef unsigned char*  PUCHAR_T;
typedef char*           STRING;
typedef unsigned char*  USTRING;

#endif /* _I18N_ */

#if defined(_WINDOWS) && defined(_I18N_)
/* typedef unsigned short CharUnit; */
typedef char CharUnit;
#else  /* !(defined(_WINDOWS) && defined(_I18N_)) */
typedef char CharUnit;
#endif /* defined(_WINDOWS) && defined(_I18N_) */


#ifdef __STDC__
extern int          ustrcasecmp        (const STRING, const STRING);
extern int          cus2iso_strcasecmp (const CharUnit*, const char*);
extern int          iso2cus_strcasecmp (const char*, const CharUnit*);
extern CHAR_T*      ustrcat            (CHAR_T*, const CHAR_T*);
extern CHAR_T*      ustrchr            (const CHAR_T*, CHAR_T);
extern int          ustrcmp            (const CHAR_T*, const CHAR_T*);
extern int          ustrcoll           (const STRING, const STRING);
extern CHAR_T*      ustrcpy            (CHAR_T*, const CHAR_T*);
extern STRING       ustrdup            (const STRING);
extern size_t       ustrlen            (const STRING);
extern int          ustrncasecmp       (const STRING, const STRING, unsigned int);
extern STRING       ustrncat           (STRING, const STRING, unsigned int);
extern CHAR_T*      ustrncmp           (const CHAR_T*, const CHAR_T*, unsigned int);
extern STRING       ustrncpy           (STRING, const STRING, unsigned int);
extern STRING       ustrrchr           (const STRING, CHAR_T);
extern STRING       ustrstr            (const STRING, const STRING);
extern STRING       ustrtok            (STRING, const STRING);
extern int          StringCaseCompare  (const CharUnit*, const CharUnit*);
extern CharUnit*    StringConcat       (CharUnit*, const CharUnit*);
extern CharUnit*    StrChr             (const CharUnit*, CharUnit);
extern int          StringCompare      (const CharUnit*, const CharUnit*);
extern int          StrColl            (const CharUnit*, const CharUnit*);
extern CharUnit*    StringCopy         (CharUnit*, const CharUnit*);
extern CharUnit*    StringDuplicate    (const CharUnit*);
extern size_t       StringLength       (const CharUnit*);
extern int          StringNCaseCompare (const CharUnit*, const CharUnit*, unsigned int);
extern CharUnit*    StringNConcat      (CharUnit*, const CharUnit*, unsigned int);
extern CharUnit*    StringNCompare     (const CharUnit*, const CharUnit*, unsigned int);
extern CharUnit*    StringNCopy        (CharUnit*, const CharUnit*, unsigned int);
extern CharUnit*    StrRChr            (const CharUnit*, CharUnit);
extern CharUnit*    StringToken        (CharUnit*, const CharUnit*);
extern CharUnit*    StringSubstring    (const CharUnit*, const CharUnit*);
extern CharUnit*    iso2cus_strcpy     (CharUnit*, const char*);
extern int          iso2cus_strcmp     (const char*, const CharUnit*);
extern char*        cus2iso_strcpy     (char*, const CharUnit*);
extern char*        cus2iso_strncpy    (char*, const CharUnit*, unsigned int);
extern CharUnit*    iso2cus_strncpy    (CharUnit*, const char*, unsigned int);
extern char*        wc2iso_strncpy     (char*, const STRING, int);
extern int          wc2iso_strcasecmp  (const CHAR_T*, const char*);
extern int          iso2wc_strcasecmp  (const char*, const CHAR_T*);
extern int          wc2iso_strcmp      (CHAR_T*, const char*);
extern STRING       iso2wc_strcpy      (STRING dest, const char* src);
extern CharUnit*    wc2cus_strcpy      (CharUnit*, const CHAR_T*);
extern CHAR_T*      cus2wc_strcpy      (CHAR_T*, const CharUnit*);
extern char*        wc2iso_strcpy      (char* dest, const STRING src);
extern STRING       iso2wc_strncpy     (STRING dest, const char* src, unsigned int count);

#else  /* __STDC__ */
extern int          ustrcasecmp        ();
extern int          cus2iso_strcasecmp ();
extern int          iso2cus_strcasecmp ();
extern CHAR_T*      ustrcat            ();
extern CHAR_T*      ustrchr            ();
extern int          ustrcmp            ();
extern int          ustrcoll           ();
extern CHAR_T*      ustrcpy            ();
extern STRING       ustrdup            ();
extern size_t       ustrlen            ();
extern int          ustrncasecmp       ();
extern STRING       ustrncat           ();
extern CHAR_T*      ustrncmp           ();
extern STRING       ustrncpy           ();
extern STRING       ustrrchr           ();
extern STRING       ustrstr            ();
extern STRING       ustrtok            ();
extern int          StringCaseCompare  ();
extern CharUnit*    StringConcat       ();
extern CharUnit*    StrChr             ();
extern int          StringCompare      ();
extern int          StrColl            ();
extern CharUnit*    StringCopy         ();
extern CharUnit*    StringDuplicate    ();
extern size_t       StringLength       ();
extern int          StringNCaseCompare ();
extern CharUnit*    StringNConcat      ();
extern CharUnit*    StringNCompare     ();
extern CharUnit*    StringNCopy        ();
extern CharUnit*    StrRChr            ();
extern CharUnit*    StringToken        ();
extern CharUnit*    StringSubstring    ();
extern CharUnit*    iso2cus_strcpy     ();
extern int          iso2cus_strcmp     ();
extern char*        cus2iso_strcpy     ();
extern char*        cus2iso_strncpy    ();
extern CharUnit*    iso2cus_strncpy    ();
extern char*        wc2iso_strncpy     ();
extern int          wc2iso_strcasecmp  ();
extern int          iso2wc_strcasecmp  ();
extern int          wc2iso_strcmp      ();
extern STRING       iso2wc_strcpy      ();
extern CharUnit*    wc2cus_strcpy      ();
extern CHAR_T*      cus2wc_strcpy      ();
extern char*        wc2iso_strcpy      ();
extern STRING       iso2wc_strncpy     ();
#endif /* __STDC__ */

#endif /* _USTRING_H */


