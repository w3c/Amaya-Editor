/*
 *
 *  (c) COPYRIGHT INRIA, 1999.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/* Unicode Routines
 * Author: R. Guetari (W3C/INRIA)
 */

#include "uaccess.h"

#ifdef _I18N_
#else  /* !_I18N_ */

#ifdef __STDC__
CHAR_T ugetcharnum (const STRING str, const int index)
#else  /* __STDC__ */
CHAR_T ugetcharnum (str, index)
const STRING str;
const int    index;
#endif /* __STDC__ */
{
   return ((CHAR_T) ((char*)str)[index]);
}

#ifdef __STDC__
void usetcharnumval (STRING str, const int index, const CHAR_T c)
#else  /* !__STDC__ */
void usetcharnumval (str, index, c)
STRING    str; 
const int index; 
const CHAR_T c;
#endif /* __STDC__ */
{
   ((char*) str) [index] = (char) c;
}

#endif /* _I18N_ */
