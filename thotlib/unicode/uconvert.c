#include "uconvert.h"

#ifdef _I18N_
#else  /* !_I18N_ */
#include <stdlib.h>
#include <ctype.h>
#ifdef __STDC__
CHAR_T utolower (CHAR_T c)
#else  /* !__STDC__ */
CHAR_T utolower (c)
CHAR_T c;
#endif /* __STDC__ */
{
     return ((CHAR_T) tolower ((int)c));
}

#endif /* _I18N_ */
