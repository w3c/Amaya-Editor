#include "uconvert.h"

#ifdef _I18N_
#else  /* !_I18N_ */
#include <stdlib.h>
#include <ctype.h>
#ifdef __STDC__
CHAR utolower (CHAR c)
#else  /* !__STDC__ */
CHAR utolower (c)
CHAR c;
#endif /* __STDC__ */
{
     return ((CHAR) tolower ((int)c));
}

#endif /* _I18N_ */
