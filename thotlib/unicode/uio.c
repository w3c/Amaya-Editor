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

#if defined(_I18N_) ||defined(__JIS__)

#ifdef __STDC__
STRING ufgets (STRING string, int n, FILE *stream)
#else  /* !__STDC__ */
STRING ufgets (string, n, stream)
STRING string; 
int    n; 
FILE*  stream;
#endif /* __STDC__ */
{
    char str [1024];
	int  len;
    if (fgets (str, n, stream)) {
#      ifdef _WINDOWS
       len = MultiByteToWideChar (CP_ACP, 0, str, -1, string, n);
	   string [min (n, len)] = L'\0';
#      else  /* !_WINDOWS */
       len = mbstowcs (string, str, n);
#      endif /* _WINDOWS */
	   return string;
	}
	return (STRING) 0;
}

#else  /* !_I18N_ */

#ifdef __STDC__
STRING ufgets (STRING string, int n, FILE *stream)
#else  /* !__STDC__ */
STRING ufgets (string, n, stream)
STRING string; 
int    n; 
FILE*  stream;
#endif /* __STDC__ */
{
    return (STRING) fgets ((char*) string, n, stream);
}

#endif /* !_I18N_ */
