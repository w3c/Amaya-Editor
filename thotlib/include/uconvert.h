#ifndef __UCONVERT_H__
#define __UCONVERT_H__
#include <stdlib.h>
#include "ustring.h"

#ifdef __STDC__
extern CHAR utolower (CHAR);
#else  /* !__STDC__ */
extern CHAR utolower ();
#endif /* __STDC__ */

#endif /* __UCONVERT_H__ */
