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

#define _MIN_ ((a), (b)) (a) < (b) ? (a) : (b)

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
    unsigned char byte;
    CHAR_T        wChar;
    char          mbcstr[MAX_BYTES];
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
             if (byte >= 0x80) {
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
#   else  /* !_I18N_ */
    return (CHAR_T*) fgets ((char*) string, n, stream);
#   endif /* !_I18N_ */
}
