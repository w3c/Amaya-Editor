#ifndef __UACCESS_H__
#define __UACCESS_H__

#include "ustring.h"

#ifdef __STDC__
extern int TtaIsDigit (CHAR_T);
extern int TtaIsAlpha (CHAR_T);
extern int TtaIsAlnum (CHAR_T);
#else  /* __STDC__ */
extern int TtaIsDigit ();
extern int TtaIsAlpha ();
extern int TtaIsAlnum ();
#endif /* __STDC__ */

#endif /* __UACCESS_H__ */




