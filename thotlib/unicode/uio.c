/*
 *
 *  (c) COPYRIGHT INRIA, 1999.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/* Unicode Routines
 * Author: R. Guetari (W3C/INRIA)
 */

#include <stdio.h>
#include "thot_sys.h"
#include "winsys.h"

#define _MIN_ ((a), (b)) (a) < (b) ? (a) : (b)
#define MAX_LEN 1024

#ifdef __STDC__
CHAR_T* ufgets (CHAR_T* string, int n, FILE *stream)
#else  /* !__STDC__ */
CHAR_T* ufgets (string, n, stream)
CHAR_T* string; 
int    n; 
FILE*  stream;
#endif /* __STDC__ */
{
#   ifdef _I18N_
#   if 0
    unsigned char byte;
    CHAR_T        wChar;
    char          mbcstr[MAX_BYTES] = "\0";
    int           nbWChar = 0;
    int           nbBytes;
    ThotBool      done = FALSE;

    while (nbWChar < n && !done) {
          nbBytes = 1; 
          if ((byte = fgetc (stream)) == 0 || byte == EOF || byte == EOL) {
             string [nbWChar] = WC_EOS;
             done = TRUE;
          }
          if (!done) {
             mbcstr[0] = byte;
             if (isleadbyte (byte)) {
                if ((byte = fgetc (stream)) == 0 || byte == EOF) {
                   string [nbWChar] = WC_EOS;
                   done = TRUE;
				}
                mbcstr [1] = byte;
                nbBytes = 2;
			 }
             mbtowc (&wChar, mbcstr, nbBytes);
             string[nbWChar++] = wChar;
		  }
	}
    if (nbWChar == 0)
       return (CHAR_T*) 0;
    return string;
#   endif /*** 000 ***/
    char str[2048];
    fgets (str, 2048, stream);
    mbstowcs (string, str, 2048);
    return string;
#   else  /* !_I18N_ */
    return (CHAR_T*) fgets ((char*) string, n, stream);
#   endif /* !_I18N_ */
}

#ifdef __STDC__
FILE* ufopen (const CHAR_T* name, const CHAR_T* mode)
#else  /* !__STDC__ */
FILE* ufopen (name, mode)
const CHAR_T* name;
const CHAR_T* mode;
#endif /* !__STDC__ */
{
#  ifdef _I18N_
#  ifdef _WINDOWS
   if (IS_NT)
      return _wfopen (name, mode);
   else
#  endif /* _WINDOWS */ 
   {
        char mbName[MAX_LEN];
        char mbMode[MAX_LEN];

        wcstombs (mbName, name, MAX_LEN);
        wcstombs (mbMode, mode, MAX_LEN);

        return fopen (mbName, mbMode);
   }
#  else  /* !_I18N_ */
   return fopen (name, mode);
#  endif /* !_I18N_ */
}
