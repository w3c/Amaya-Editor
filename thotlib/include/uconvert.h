#ifndef __UCONVERT_H__
#define __UCONVERT_H__
#include <stdlib.h>
#include "ustring.h"

#ifndef MAX_TXT_LEN
#define MAX_TXT_LEN 1024
#endif 

extern int uatoi (const STRING string);
extern long uatol (const STRING string);
extern CHAR_T utolower (CHAR_T c);

/*----------------------------------------------------------------------
  TtaGetCharFromUnicode: returns the char code in the corresponding encoding
  of  the Unicode value wc.
  ----------------------------------------------------------------------*/
extern unsigned char TtaGetCharFromUnicode (const wchar_t wc, CHARSET encoding);

/*----------------------------------------------------------------------
  TtaGetUnicodeFromChar: return the Unicode val corresponding
  to the ISO Latin 2 code c.
  ----------------------------------------------------------------------*/
extern wchar_t TtaGetUnicodeFromChar (const unsigned char c, CHARSET encoding);

/*----------------------------------------------------------------------
  TtaMBS2WCS converts a multibyte string into a wide character string
  according to the charset.
  Returns the number of bytes in the multibyte character or -1
  ----------------------------------------------------------------------*/
extern int TtaMBS2WCS (unsigned char** src, wchar_t** target, CHARSET encoding);

/*----------------------------------------------------------------------
  TtaWCS2MBS:
  converts a wide character string into a multibyte string according to
  the charset.                                                                
  Return value: -1 if fails
  Number of bytes in the multibyte character
  ----------------------------------------------------------------------*/
extern int TtaWCS2MBS (wchar_t** src, unsigned char** target, CHARSET encoding );

/*----------------------------------------------------------------------
  TtaGetNextWideCharFromMultibyteString: Looks for the next Wide character 
  value in a multibyte character string.
  ----------------------------------------------------------------------*/
extern int TtaGetNextWideCharFromMultibyteString (wchar_t *car, unsigned char **txt, CHARSET encoding);

/*----------------------------------------------------------------------
  TtaGetNumberOfBytesToRead: 
  Returns the number of bytes to read
  ----------------------------------------------------------------------*/
extern int TtaGetNumberOfBytesToRead (unsigned char **txt, CHARSET encoding);

/*-------------------------------------------------------------
  TtaCopyWC2Iso copies src (16-bit) into dest (8-bit). This 
  function suposes that enough memory has been already allocated.
  Return the encoding detected.
  -------------------------------------------------------------*/
extern void TtaCopyWC2Iso (unsigned char *dest, CHAR_T *src, CHARSET encoding);

/*-------------------------------------------------------------
  TtaCopyIso2WC copies src (8-bit) into dest (16-bits). This 
  function suposes that enough memory has been already allocated.
  -------------------------------------------------------------*/
extern void TtaCopyIso2WC (CHAR_T *dest, unsigned char *src, CHARSET encoding);

#endif /* __UCONVERT_H__ */
