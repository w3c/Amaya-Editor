#ifndef __UCONVERT_H__
#define __UCONVERT_H__
#include <stdlib.h>
#include "ustring.h"

#ifdef __STDC__
#ifdef _I18N_
extern long uctoi (const STRING);
#else  /* !_I18N_ */
extern int  uctoi (const STRING);
#endif /* !_I18N_ */
extern CHAR_T utolower (CHAR_T);
#else  /* !__STDC__ */
#ifdef _I18N_
extern long uctoi ();
#else  /* !_I18N_ */
extern int  uctoi ();
#endif /* !_I18N_ */
extern CHAR_T utolower ();
#endif /* !__STDC__ */

#endif /* __UCONVERT_H__ */
