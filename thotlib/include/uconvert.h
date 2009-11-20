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
  TtaMBstringToWCS converts a UTF-8 multibyte string src into a Unicode
  wide character.
  The src parameter points to a multibyte string pointer.
  Return:
  The number of bytes consummed in the multibyte character string or -1
  The pointer to the source multibyte string is updated.
  The new wide character is inserted at the dest address.
  ----------------------------------------------------------------------*/
extern int TtaMBstringToWC (const unsigned char **src, wchar_t *dest);

/*----------------------------------------------------------------------
  TtaWCToMBstring converts a Unicode wide character into a UTF-8
  multibyte string.
  The dest parameter points to a multibyte string pointer.
  Return:
  The number of consummed bytes in the multibyte character or -1
  The pointer to the dest multibyte string is updated.
  ----------------------------------------------------------------------*/
extern int TtaWCToMBstring (wchar_t src, unsigned char **dest);

/*----------------------------------------------------------------------
  TtaGetNextWCFromString: Looks for the next wide character 
  value in a multibyte character string.
  The car parameter points to a wide character string.
  The txt parameter points to a character string pointer.
  Return:
  The number of bytes in the multibyte character or -1
  The new wide character is inserted at the car address.
  ----------------------------------------------------------------------*/
extern int TtaGetNextWCFromString (wchar_t *car, unsigned char **txt, CHARSET encoding);

/*----------------------------------------------------------------------
  TtaGetNumberOfBytesToRead: 
  Returns the number of bytes to read
  ----------------------------------------------------------------------*/
extern int TtaGetNumberOfBytesToRead (unsigned char **txt, CHARSET encoding);

/*-------------------------------------------------------------
  TtaConvertWCToByte converts the whole src (16-bit) string into
  an ISO string (8-bit).
  Return:
  The returned string should be freed by the caller.
  -------------------------------------------------------------*/
extern unsigned char *TtaConvertWCToByte (wchar_t *src, CHARSET encoding);

/*-------------------------------------------------------------
  TtaConvertByteToWC converts the whole src (1 byte or 2 byte character
  for JIS) string into a wide character (16-bit) string.
  Return:
  The returned wide character string should be freed by the caller.
  -------------------------------------------------------------*/
extern wchar_t *TtaConvertByteToWC (const unsigned char *src, CHARSET encoding);

/*-------------------------------------------------------------
  TtaConvertByteToMbs converts the whole src (1 or 2 bytes) string into a UTF-8
  string (1 byte).
  Return:
  The returned string should be freed by the caller.
  -------------------------------------------------------------*/
extern unsigned char *TtaConvertByteToMbs (unsigned char *src, CHARSET encoding);


/*----------------------------------------------------------------------
  TtaConvertByteToMbsWithCheck converts the whole src (1 or 2 bytes) string
  into a UTF-8 string (1 byte).
  Return:
  The number of characters consummed in the source string.
  The returned string should be freed by the caller.
  ----------------------------------------------------------------------*/
extern unsigned char *TtaConvertByteToMbsWithCheck (const unsigned char *src,
                                                    CHARSET encoding, int *length);

/*-------------------------------------------------------------
  TtaConvertMbsToByte converts the whole UTF-8 string (8-bit)
  src into a byte string (8-bit).
  Return:
  The returned string should be freed by the caller.
  -------------------------------------------------------------*/
extern unsigned char *TtaConvertMbsToByte (const unsigned char *src, CHARSET encoding);

/*-------------------------------------------------------------
  TtaConvertMbsToCHAR converts the whole UTF-8 string (8-bit) src
  into a CHAR_T string (8-bit or 16-bit).
  Return:
  The returned string should be freed by the caller.
  -------------------------------------------------------------*/
extern CHAR_T *TtaConvertMbsToCHAR (const unsigned char *src);

/*-------------------------------------------------------------
  TtaConvertCHARToByte converts the whole src (16-bit) string into
  an ISO string (8-bit).
  Return:
  The returned string should be freed by the caller.
  -------------------------------------------------------------*/
extern unsigned char *TtaConvertCHARToByte (CHAR_T *src, CHARSET encoding);

/*-------------------------------------------------------------
  TtaConvertByteToCHAR converts the whole src (8-bit) into
  a wide character (16-bit) string.
  Return:
  The returned string should be freed by the caller.
  -------------------------------------------------------------*/
extern CHAR_T *TtaConvertByteToCHAR (const unsigned char *src, CHARSET encoding);

#endif /* __UCONVERT_H__ */
