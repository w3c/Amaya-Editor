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
extern int           TtaWC2MB                            (CHAR_T, char*, CHARSET);
extern int           TtaMB2WC                            (char*, CHAR_T*, CHARSET);
extern int           TtaMBS2WCS                          (unsigned char**, CHAR_T**, CHARSET);
extern int           TtaWCS2MBS                          (CHAR_T**, unsigned char**, CHARSET);
extern unsigned char TtaGetISOLatin2CodeFromUnicode      (const CHAR_T);
extern unsigned char TtaGetISOLatin6CodeFromUnicode      (const CHAR_T);
extern unsigned char TtaGetWindows1256CPCodeFromUnicode  (const CHAR_T);
extern CHAR_T        TtaGetUnicodeValueFromISOLatin2Code (const unsigned char);
extern CHAR_T        TtaGetUnicodeValueFromISOLatin6Code (const unsigned char);
extern CHAR_T        TtaGetUnicodeValueFromWindows1256CP (const unsigned char);
#else  /* !__STDC__ */
extern int           uctoi                               ();
extern long          uctol                               ();
extern CHAR_T        utolower                            ();
extern int           wctoi                               ();
extern int           TtaWC2MB                            ();
extern int           TtaMB2WC                            ();
extern int           TtaMBS2WCS                          ();
extern int           TtaWCS2MBS                          ();
extern unsigned char TtaGetISOLatin2CodeFromUnicode      ();
extern unsigned char TtaGetISOLatin6CodeFromUnicode      ();
extern unsigned char TtaGetWindows1256CPCodeFromUnicode  ();
extern CHAR_T        TtaGetUnicodeValueFromISOLatin2Code ();
extern CHAR_T        TtaGetUnicodeValueFromISOLatin6Code ();
extern CHAR_T        TtaGetUnicodeValueFromWindows1256CP ();
#endif /* !__STDC__ */

#endif /* __UCONVERT_H__ */
