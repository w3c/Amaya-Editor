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
  TtaGetCharFromUnicode returns the ISO or Windows character code
  of the Unicode value wc.
  ----------------------------------------------------------------------*/
extern unsigned char TtaGetCharFromUnicode (const wchar_t wc, CHARSET encoding);

/*----------------------------------------------------------------------
  TtaGetUnicodeFromChar returns the Unicode value of the corresponding
  ISO or Windows character code c.
  ----------------------------------------------------------------------*/
extern wchar_t TtaGetUnicodeFromChar (const unsigned char c, CHARSET encoding);

/*----------------------------------------------------------------------
  TtaMBstring2WCS converts a multibyte string into a wide character.
  Returns the number of bytes in the multibyte character or -1
  The pointer to the source multibyte string is updated.
  ----------------------------------------------------------------------*/
extern int TtaMBstring2WC (unsigned char **src, wchar_t *dest);

/*----------------------------------------------------------------------
  TtaWCS2MBs converts a wide character into a multibyte string according to
  the charset.                                                                
  Return the number of bytes in the multibyte character or -1
  The pointer to the dest multibyte string is updated.
  ----------------------------------------------------------------------*/
extern int TtaWC2MBstring (wchar_t src, unsigned char **dest);

/*----------------------------------------------------------------------
  TtaGetNextWCFromString: Looks for the next Wide character 
  value in a multibyte character string.
  ----------------------------------------------------------------------*/
extern int TtaGetNextWCFromString (wchar_t *car, unsigned char **txt, CHARSET encoding);

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
