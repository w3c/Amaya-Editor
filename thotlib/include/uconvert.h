#ifndef __UCONVERT_H__
#define __UCONVERT_H__
#include <stdlib.h>
#include "ustring.h"

#ifndef MAX_TXT_LEN
#define MAX_TXT_LEN 1024
#endif 

#ifdef __STDC__
extern int uatoi (const STRING string);
extern long uatol (const STRING string);
extern CHAR_T utolower (CHAR_T c);
extern unsigned char TtaGetCharFromUnicode (const wchar_t wc, CHARSET encoding);
extern wchar_t TtaGetUnicodeFromChar (const unsigned char c, CHARSET encoding);
extern int TtaWC2MB (wchar_t wchar, char* mbchar, CHARSET encoding);
extern int TtaGetNextWideCharFromMultibyteString (wchar_t *car, unsigned char **txt, CHARSET encoding);

#else  /* !__STDC__ */

extern int uatoi (/* const STRING string */);
extern long uatol (/* const STRING string */);
extern CHAR_T utolower (/* CHAR_T c */);
extern unsigned char TtaGetCharFromUnicode (/* const wchar_t wc, CHARSET encoding */);
extern wchar_t TtaGetUnicodeFromChar (/* const unsigned char c, CHARSET encoding */);
extern int TtaGetNextWideCharFromMultibyteString (/*wchar_t *car, unsigned char **txt, CHARSET encoding*/);

#endif /* !__STDC__ */
#endif /* __UCONVERT_H__ */
