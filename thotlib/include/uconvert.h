#ifndef __UCONVERT_H__
#define __UCONVERT_H__
#include <stdlib.h>
#include "ustring.h"

#ifndef MAX_TXT_LEN
#define MAX_TXT_LEN 1024
#endif 

#ifdef __STDC__
extern int           uatoi                                  (const STRING);
extern long          uatol                                  (const STRING);
extern CHAR_T        utolower                               (CHAR_T);
extern int           wctoi                                  (const CHAR_T*);
extern int           TtaWC2MB                               (CHAR_T, char*, CHARSET);
extern int           TtaMB2WC                               (char*, CHAR_T*, CHARSET);
extern int           TtaMBS2WCS                             (unsigned char**, CHAR_T**, CHARSET);
extern int           TtaWCS2MBS                             (CHAR_T**, unsigned char**, CHARSET);
extern unsigned char TtaGet_ISO_8859_2_CodeFromUnicode      (const CHAR_T);
extern unsigned char TtaGet_ISO_8859_3_CodeFromUnicode      (const CHAR_T);
extern unsigned char TtaGet_ISO_8859_4_CodeFromUnicode      (const CHAR_T);
extern unsigned char TtaGet_ISO_8859_5_CodeFromUnicode      (const CHAR_T);
extern unsigned char TtaGet_ISO_8859_6_CodeFromUnicode      (const CHAR_T);
extern unsigned char TtaGet_ISO_8859_7_CodeFromUnicode      (const CHAR_T);
extern unsigned char TtaGet_ISO_8859_8_CodeFromUnicode      (const CHAR_T);
extern unsigned char TtaGet_ISO_8859_9_CodeFromUnicode      (const CHAR_T);
extern unsigned char TtaGetWindows1250CPCodeFromUnicode     (const CHAR_T);
extern unsigned char TtaGetWindows1251CPCodeFromUnicode     (const CHAR_T);
extern unsigned char TtaGetWindows1252CPCodeFromUnicode     (const CHAR_T);
extern unsigned char TtaGetWindows1253CPCodeFromUnicode     (const CHAR_T);
extern unsigned char TtaGetWindows1254CPCodeFromUnicode     (const CHAR_T);
extern unsigned char TtaGetWindows1255CPCodeFromUnicode     (const CHAR_T);
extern unsigned char TtaGetWindows1256CPCodeFromUnicode     (const CHAR_T);
extern unsigned char TtaGetWindows1257CPCodeFromUnicode     (const CHAR_T);
extern CHAR_T        TtaGetUnicodeValueFrom_ISO_8859_2_Code (const unsigned char);
extern CHAR_T        TtaGetUnicodeValueFrom_ISO_8859_3_Code (const unsigned char);
extern CHAR_T        TtaGetUnicodeValueFrom_ISO_8859_4_Code (const unsigned char);
extern CHAR_T        TtaGetUnicodeValueFrom_ISO_8859_5_Code (const unsigned char);
extern CHAR_T        TtaGetUnicodeValueFrom_ISO_8859_6_Code (const unsigned char);
extern CHAR_T        TtaGetUnicodeValueFrom_ISO_8859_7_Code (const unsigned char);
extern CHAR_T        TtaGetUnicodeValueFrom_ISO_8859_8_Code (const unsigned char);
extern CHAR_T        TtaGetUnicodeValueFrom_ISO_8859_9_Code (const unsigned char);
extern CHAR_T        TtaGetUnicodeValueFromWindows1250CP    (const unsigned char);
extern CHAR_T        TtaGetUnicodeValueFromWindows1251CP    (const unsigned char);
extern CHAR_T        TtaGetUnicodeValueFromWindows1252CP    (const unsigned char);
extern CHAR_T        TtaGetUnicodeValueFromWindows1253CP    (const unsigned char);
extern CHAR_T        TtaGetUnicodeValueFromWindows1254CP    (const unsigned char);
extern CHAR_T        TtaGetUnicodeValueFromWindows1255CP    (const unsigned char);
extern CHAR_T        TtaGetUnicodeValueFromWindows1256CP    (const unsigned char);
extern CHAR_T        TtaGetUnicodeValueFromWindows1257CP    (const unsigned char);
#else  /* !__STDC__ */
extern int           uatoi                                  ();
extern long          uatol                                  ();
extern CHAR_T        utolower                               ();
extern int           wctoi                                  ();
extern int           TtaWC2MB                               ();
extern int           TtaMB2WC                               ();
extern int           TtaMBS2WCS                             ();
extern int           TtaWCS2MBS                             ();
extern unsigned char TtaGet_ISO_8859_2_CodeFromUnicode      ();
extern unsigned char TtaGet_ISO_8859_3_CodeFromUnicode      ();
extern unsigned char TtaGet_ISO_8859_4_CodeFromUnicode      ();
extern unsigned char TtaGet_ISO_8859_5_CodeFromUnicode      ();
extern unsigned char TtaGet_ISO_8859_6_CodeFromUnicode      ();
extern unsigned char TtaGet_ISO_8859_7_CodeFromUnicode      ();
extern unsigned char TtaGet_ISO_8859_8_CodeFromUnicode      ();
extern unsigned char TtaGet_ISO_8859_9_CodeFromUnicode      ();
extern unsigned char TtaGetWindows1250CPCodeFromUnicode     ();
extern unsigned char TtaGetWindows1251CPCodeFromUnicode     ();
extern unsigned char TtaGetWindows1252CPCodeFromUnicode     ();
extern unsigned char TtaGetWindows1253CPCodeFromUnicode     ();
extern unsigned char TtaGetWindows1254CPCodeFromUnicode     ();
extern unsigned char TtaGetWindows1255CPCodeFromUnicode     ();
extern unsigned char TtaGetWindows1256CPCodeFromUnicode     ();
extern unsigned char TtaGetWindows1257CPCodeFromUnicode     ();
extern CHAR_T        TtaGetUnicodeValueFrom_ISO_8859_2_Code ();
extern CHAR_T        TtaGetUnicodeValueFrom_ISO_8859_3_Code ();
extern CHAR_T        TtaGetUnicodeValueFrom_ISO_8859_4_Code ();
extern CHAR_T        TtaGetUnicodeValueFrom_ISO_8859_5_Code ();
extern CHAR_T        TtaGetUnicodeValueFrom_ISO_8859_6_Code ();
extern CHAR_T        TtaGetUnicodeValueFrom_ISO_8859_7_Code ();
extern CHAR_T        TtaGetUnicodeValueFrom_ISO_8859_8_Code ();
extern CHAR_T        TtaGetUnicodeValueFrom_ISO_8859_9_Code ();
extern CHAR_T        TtaGetUnicodeValueFromWindows1250CP    ();
extern CHAR_T        TtaGetUnicodeValueFromWindows1251CP    ();
extern CHAR_T        TtaGetUnicodeValueFromWindows1252CP    ();
extern CHAR_T        TtaGetUnicodeValueFromWindows1253CP    ();
extern CHAR_T        TtaGetUnicodeValueFromWindows1254CP    ();
extern CHAR_T        TtaGetUnicodeValueFromWindows1255CP    ();
extern CHAR_T        TtaGetUnicodeValueFromWindows1256CP    ();
extern CHAR_T        TtaGetUnicodeValueFromWindows1257CP    ();
#endif /* !__STDC__ */

#endif /* __UCONVERT_H__ */
