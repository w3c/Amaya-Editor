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
extern int          ustrcasecmp  (STRING, const STRING);
extern STRING       ustrcat      (STRING, const STRING);
extern STRING       ustrchr      (const STRING, CHAR_T);
extern int          ustrcmp      (const STRING, const STRING);
extern int          ustrcoll     (const STRING, const STRING);
extern STRING       ustrcpy      (STRING, const STRING);
extern STRING       ustrdup      (const STRING);
extern size_t       ustrlen      (const STRING);
extern int          ustrncasecmp (STRING, const STRING, unsigned int);
extern STRING       ustrncat     (STRING, const STRING, unsigned int);
extern STRING       ustrncmp     (const STRING, const STRING, unsigned int);
extern STRING       ustrncpy     (STRING, const STRING, unsigned int);
extern STRING       ustrrchr     (const STRING, CHAR_T);
extern STRING       ustrstr      (const STRING, const STRING);
extern STRING       ustrtok      (STRING, const STRING);
#else  /* __STDC__ */
extern int          ustrcasecmp  ();
extern STRING       ustrcat      ();
extern STRING       ustrchr      ();
extern int          ustrcmp      ();
extern int          ustrcoll     ();
extern STRING       ustrcpy      ();
extern STRING       ustrdup      ();
extern size_t       ustrlen      ();
extern int          ustrncasecmp ();
extern STRING       ustrncat     ();
extern STRING       ustrncmp     ();
extern STRING       ustrncpy     ();
extern STRING       ustrrchr     ();
extern STRING       ustrstr      ();
extern STRING       ustrtok      ();
#endif /* __STDC__ */

#endif /* _USTRING_H */


