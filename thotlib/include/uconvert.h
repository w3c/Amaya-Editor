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
  TtaGetCharFromWC returns the ISO or Windows character code
  of the Unicode value wc.
  ----------------------------------------------------------------------*/
extern unsigned char TtaGetCharFromWC (wchar_t wc, CHARSET encoding);

/*----------------------------------------------------------------------
  TtaGetWCFromChar returns the Unicode value of the corresponding
  ISO or Windows character code c.
  ----------------------------------------------------------------------*/
extern wchar_t TtaGetWCFromChar (unsigned char c, CHARSET encoding);

/*----------------------------------------------------------------------
  TtaMBstringT0WC converts a multibyte string into a wide character.
  Returns the number of bytes in the multibyte character or -1
  The pointer to the source multibyte string is updated.
  ----------------------------------------------------------------------*/
extern int TtaMBstringToWC (unsigned char **src, wchar_t *dest);

/*----------------------------------------------------------------------
  TtaWCToMBs converts a wide character into a multibyte string according to
  the charset.                                                                
  Return the number of bytes in the multibyte character or -1
  The pointer to the dest multibyte string is updated.
  ----------------------------------------------------------------------*/
extern int TtaWCToMBstring (wchar_t src, unsigned char **dest);

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
  TtaConvertWCToIso converts the src (16-bit) into an ISO string
  (8-bit).
  The returned string should be freed by the caller.
  -------------------------------------------------------------*/
extern char *TtaConvertWCToIso (wchar_t *src, CHARSET encoding);

/*-------------------------------------------------------------
  TtaConvertIsoToWC converts the src (8-bit) into a wide character
  string (16-bit).
  The returned string should be freed by the caller.
  -------------------------------------------------------------*/
extern wchar_t *TtaConvertIsoToWC (unsigned char *src, CHARSET encoding);

/*-------------------------------------------------------------
  TtaConvertIsoToMbs converts the src (8-bit) into a UTF-8
  string (8-bit).
  The returned string should be freed by the caller.
  -------------------------------------------------------------*/
extern unsigned char *TtaConvertIsoToMbs (unsigned char *src, CHARSET encoding);

/*-------------------------------------------------------------
  TtaConvertIsoToMbs converts the src (8-bit) into a UTF-8
  string (8-bit).
  The returned string should be freed by the caller.
  -------------------------------------------------------------*/
extern unsigned char *TtaConverMbsToIso (unsigned char *src, CHARSET encoding);

/*-------------------------------------------------------------
  TtaConvertCHARToIso converts the src CHAR_T* into an ISO
  string (8-bit).
  The returned string should be freed by the caller.
  -------------------------------------------------------------*/
extern char *TtaConvertCHARToIso (CHAR_T *src, CHARSET encoding);

/*-------------------------------------------------------------
  TtaConvertIsoToWC converts the src (8-bit) into a CHAR_T*
  string.
  The returned string should be freed by the caller.
  -------------------------------------------------------------*/
extern CHAR_T *TtaConvertIsoToCHAR (unsigned char *src, CHARSET encoding);

#endif /* __UCONVERT_H__ */
