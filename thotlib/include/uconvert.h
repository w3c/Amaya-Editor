#ifndef __UCONVERT_H__
#define __UCONVERT_H__
#include <stdlib.h>
#include "ustring.h"

#ifdef __STDC__
extern CHAR_T utolower (CHAR_T);
#else  /* !__STDC__ */
extern CHAR_T utolower ();
#endif /* __STDC__ */

#endif /* __UCONVERT_H__ */
