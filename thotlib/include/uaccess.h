#ifndef __UACCESS_H__
#define __UACCESS_H__
#include "ustring.h"

#ifdef __STDC__
extern CHAR ugetcharnum    (const STRING, const int);
extern void usetcharnumval (STRING, const int, const CHAR);
#else  /* __STDC__ */
extern CHAR ugetcharnum    ();
extern void usetcharnumval ();
#endif /* __STDC__ */

#endif /* __UACCESS_H__ */




