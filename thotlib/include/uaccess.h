#ifndef __UACCESS_H__
#define __UACCESS_H__
#include "ustring.h"

#ifdef __STDC__
extern CHAR_T ugetcharnum    (const STRING, const int);
extern void usetcharnumval (STRING, const int, const CHAR_T);
#else  /* __STDC__ */
extern CHAR_T ugetcharnum    ();
extern void usetcharnumval ();
#endif /* __STDC__ */

#endif /* __UACCESS_H__ */




