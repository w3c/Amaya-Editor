#ifndef __UCONVERT_H__
#define __UCONVERT_H__
#include <stdlib.h>
#include "ustring.h"

#ifdef __STDC__
extern int    uctoi        (const STRING);
extern long   uctol        (const STRING);
extern CHAR_T utolower     (CHAR_T);
extern int    wctoi        (const CHAR_T*);
#else  /* !__STDC__ */
extern int  uctoi          ();
extern long uctol          ();
extern CHAR_T utolower     ();
extern int    wctoi        ();
#endif /* !__STDC__ */

#endif /* __UCONVERT_H__ */
