#ifndef _USTRING_H
#define _USTRING_H


#ifdef _I18N_
#else  /* _I18N_ */
#include <string.h>

typedef char           CHAR;
typedef char*          PCHAR;
typedef unsigned char  UCHAR;
typedef unsigned char* PUCHAR;
typedef char*          STRING;
typedef unsigned char* USTRING;

#endif /* _I18N_ */

#ifdef __STDC__
extern unsigned int ustrcasecmp  (STRING, const STRING);
extern STRING       ustrcat      (STRING, const STRING);
extern STRING       ustrchr      (const STRING, CHAR);
extern int          ustrcmp      (const STRING, const STRING);
extern int          ustrcoll     (const STRING, const STRING);
extern STRING       ustrcpy      (STRING, const STRING);
extern STRING       ustrdup      (const STRING);
extern unsigned int ustrlen      (const STRING);
extern unsigned int ustrncasecmp (STRING, const STRING, unsigned int);
extern STRING       ustrncat     (STRING, const STRING, unsigned int);
extern STRING       ustrncmp     (const STRING, const STRING, unsigned int);
extern STRING       ustrncpy     (STRING, const STRING, unsigned int);
extern STRING       ustrrchr     (const STRING, CHAR);
extern STRING       ustrstr      (const STRING, const STRING);
extern STRING       ustrtok      (STRING, const STRING);
#else  /* __STDC__ */
extern unsigned int ustrcasecmp  ();
extern STRING       ustrcat      ();
extern STRING       ustrchr      ();
extern int          ustrcmp      ();
extern int          ustrcoll     ();
extern STRING       ustrcpy      ();
extern STRING       ustrdup      ();
extern unsigned int ustrlen      ();
extern unsigned int ustrncasecmp ();
extern STRING       ustrncat     ();
extern STRING       ustrncmp     ();
extern STRING       ustrncpy     ();
extern STRING       ustrrchr     ();
extern STRING       ustrstr      ();
extern STRING       ustrtok      ();
#endif /* __STDC__ */

#endif /* _USTRING_H */


