#ifndef __UCONVERT_H__
#define __UCONVERT_H__
#include <stdlib.h>
#include "ustring.h"

#ifdef __STDC__
extern STRING ISO2WideChar (const char*);
extern char*  WideChar2ISO (STRING);
extern int    uctoi        (const STRING);
extern long   uctol        (const STRING);
extern CHAR_T utolower     (CHAR_T);
#else  /* !__STDC__ */
extern STRING ISO2WideChar ();
extern char*  WideChar2ISO        ();
extern int  uctoi          ();
extern long uctol          ();
extern CHAR_T utolower     ();
#endif /* !__STDC__ */

#endif /* __UCONVERT_H__ */
