#ifndef __UCONVERT_H__
#define __UCONVERT_H__
#include <stdlib.h>
#include "ustring.h"

#ifndef MAX_TXT_LEN
#define MAX_TXT_LEN 1024
#endif 

#ifdef __STDC__
extern int           uctoi                               (const STRING);
extern long          uctol                               (const STRING);
extern CHAR_T        utolower                            (CHAR_T);
extern int           wctoi                               (const CHAR_T*);
extern unsigned char TtaGetISOLatin2CodeFromUnicode      (const CHAR_T);
extern unsigned char TtaGetISOLatin6CodeFromUnicode      (const CHAR_T);
extern CHAR_T        TtaGetUnicodeValueFromISOLatin2Code (const unsigned char);
extern CHAR_T        TtaGetUnicodeValueFromISOLatin6Code (const unsigned char);
#else  /* !__STDC__ */
extern int  uctoi                                        ();
extern long uctol                                        ();
extern CHAR_T utolower                                   ();
extern int    wctoi                                      ();
extern unsigned char TtaGetISOLatin2CodeFromUnicode      ();
extern unsigned char TtaGetISOLatin6CodeFromUnicode      ();
extern CHAR_T        TtaGetUnicodeValueFromISOLatin2Code ();
extern CHAR_T        TtaGetUnicodeValueFromISOLatin6Code ();
#endif /* !__STDC__ */

#endif /* __UCONVERT_H__ */
