#ifndef _USTRING_H
#define _USTRING_H

#include <string.h>

#ifdef _WINDOWS
#include <windows.h>
#include <wchar.h>
#endif /* _WINDOWS */

#ifdef _I18N_

typedef unsigned short  CHAR_T;
typedef unsigned short* PCHAR_T;
typedef unsigned short  UCHAR_T;
typedef unsigned short* PUCHAR_T;
typedef unsigned short* STRING;
typedef unsigned short* USTRING;

#else  /* _I18N_ */

typedef char            CHAR_T;
typedef char*           PCHAR_T;
typedef unsigned char   UCHAR_T;
typedef unsigned char*  PUCHAR_T;
typedef char*           STRING;
typedef unsigned char*  USTRING;

#endif /* _I18N_ */

#if defined(_WINDOWS) && defined(_I18N_)
typedef unsigned short CharUnit;
#else  /* !(defined(_WINDOWS) && defined(_I18N_)) */
typedef char CharUnit;
#endif /* defined(_WINDOWS) && defined(_I18N_) */


#ifdef __STDC__
extern int          ustrcasecmp        (STRING, const STRING);
extern int          cus2iso_strcasecmp (CharUnit*, const char*);
extern int          iso2cus_strcasecmp (char*, const CharUnit*);
extern STRING       ustrcat            (STRING, const STRING);
extern STRING       ustrchr            (const STRING, CHAR_T);
extern int          ustrcmp            (const STRING, const STRING);
extern int          ustrcoll           (const STRING, const STRING);
extern STRING       ustrcpy            (STRING, const STRING);
extern STRING       ustrdup            (const STRING);
extern size_t       ustrlen            (const STRING);
extern int          ustrncasecmp       (STRING, const STRING, unsigned int);
extern STRING       ustrncat           (STRING, const STRING, unsigned int);
extern STRING       ustrncmp           (const STRING, const STRING, unsigned int);
extern STRING       ustrncpy           (STRING, const STRING, unsigned int);
extern STRING       ustrrchr           (const STRING, CHAR_T);
extern STRING       ustrstr            (const STRING, const STRING);
extern STRING       ustrtok            (STRING, const STRING);
extern int          StringCaseCompare  (CharUnit*, const CharUnit*);
extern CharUnit*    StringConcat       (CharUnit*, const CharUnit*);
extern CharUnit*    StrChr             (const CharUnit*, CharUnit);
extern int          StringCompare      (const CharUnit*, const CharUnit*);
extern int          StrColl            (const CharUnit*, const CharUnit*);
extern CharUnit*    StringCopy         (CharUnit*, const CharUnit*);
extern CharUnit*    StringDuplicate    (const CharUnit*);
extern size_t       StringLength       (const CharUnit*);
extern int          StringNCaseCompare (CharUnit*, const CharUnit*, unsigned int);
extern CharUnit*    StringNConcat      (CharUnit*, const CharUnit*, unsigned int);
extern CharUnit*    StringNCompare     (const CharUnit*, const CharUnit*, unsigned int);
extern CharUnit*    StringNCopy        (CharUnit*, const CharUnit*, unsigned int);
extern CharUnit*    StrRChr            (const CharUnit*, CharUnit);
extern CharUnit*    StringToken        (CharUnit*, const CharUnit*);
extern CharUnit*    StringSubstring    (const CharUnit*, const CharUnit*);
#else  /* __STDC__ */
extern int          ustrcasecmp        ();
extern int          cus2iso_strcasecmp ();
extern int          iso2cus_strcasecmp ();
extern STRING       ustrcat            ();
extern STRING       ustrchr            ();
extern int          ustrcmp            ();
extern int          ustrcoll           ();
extern STRING       ustrcpy            ();
extern STRING       ustrdup            ();
extern size_t       ustrlen            ();
extern int          ustrncasecmp       ();
extern STRING       ustrncat           ();
extern STRING       ustrncmp           ();
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
#endif /* __STDC__ */

#endif /* _USTRING_H */


