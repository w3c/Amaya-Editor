#include "uaccess.h"

#ifdef _I18N_
#else  /* !_I18N_ */

#ifdef __STDC__
CHAR ugetcharnum (const STRING str, const int index)
#else  /* __STDC__ */
CHAR ugetcharnum (str, index)
const STRING str;
const int    index;
#endif /* __STDC__ */
{
   return ((CHAR) ((char*)str)[index]);
}

#ifdef __STDC__
void usetcharnumval (STRING str, const int index, const CHAR c)
#else  /* !__STDC__ */
void usetcharnumval (str, index, c)
STRING    str; 
const int index; 
const CHAR c;
#endif /* __STDC__ */
{
   ((char*) str) [index] = (char) c;
}

#endif /* _I18N_ */
