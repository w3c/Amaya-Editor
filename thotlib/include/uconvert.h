#ifndef __UCONVERT_H__
#define __UCONVERT_H__
#include <stdlib.h>
#include "ustring.h"

#ifndef MAX_TXT_LEN
#define MAX_TXT_LEN 1024
#endif 


/*----------------------------------------------------------------------
  TtaGetCharFromWC returns the ISO or Windows character code
  of the Unicode value wc.
  ----------------------------------------------------------------------*/
#ifdef _TH_
extern wchar_t TtaGetCharFromWC (wchar_t wc, CHARSET encoding);
#else /* _TH */
extern unsigned char TtaGetCharFromWC (wchar_t wc, CHARSET encoding);
#endif /* _TH_ */

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
  TtaConvertWCToByte converts the src (16-bit) into an ISO string
  (8-bit).
  The returned string should be freed by the caller.
  -------------------------------------------------------------*/
extern unsigned char *TtaConvertWCToByte (wchar_t *src, CHARSET encoding);

/*-------------------------------------------------------------
  TtaConvertByteToWC converts the src (8-bit) into a wide character
  string (16-bit).
  The returned string should be freed by the caller.
  -------------------------------------------------------------*/
extern wchar_t *TtaConvertByteToWC (unsigned char *src, CHARSET encoding);

/*-------------------------------------------------------------
  TtaConvertByteToMbs converts the src (8-bit) into a UTF-8
  string (8-bit).
  The returned string should be freed by the caller.
  -------------------------------------------------------------*/
extern unsigned char *TtaConvertByteToMbs (unsigned char *src, CHARSET encoding);

/*-------------------------------------------------------------
  TtaConvertMbsToByte converts a UTF-8 string (8-bit) into an
  Byte string (8-bit).
  The returned string should be freed by the caller.
  -------------------------------------------------------------*/
extern unsigned char *TtaConvertMbsToByte (unsigned char *src, CHARSET encoding);

/*-------------------------------------------------------------
  TtaConvertMbsToCHAR converts a UTF-8 string (8-bit) into CHAR_T
  string (8-bit or 16-bit).
  The returned string should be freed by the caller.
  -------------------------------------------------------------*/
extern CHAR_T *TtaConvertMbsToCHAR (unsigned char *src);

/*-------------------------------------------------------------
  TtaConvertCHARToByte converts the src CHAR_T* into an ISO
  string (8-bit).
  The returned string should be freed by the caller.
  -------------------------------------------------------------*/
extern unsigned char *TtaConvertCHARToByte (CHAR_T *src, CHARSET encoding);

/*-------------------------------------------------------------
  TtaConvertByteToWC converts the src (8-bit) into a CHAR_T*
  string.
  The returned string should be freed by the caller.
  -------------------------------------------------------------*/
extern CHAR_T *TtaConvertByteToCHAR (unsigned char *src, CHARSET encoding);

#endif /* __UCONVERT_H__ */
