#include <stdio.h>
#include "thot_sys.h"

#ifdef _I18N_

#ifdef _WINDOWS

#define ufprintf fwprintf
#define usprintf swprintf
#define usscanf  swscanf

#define uaccess  _waccess
#define ufgets   fgetws
#define ufopen   _wfopen
#define uunlink  _wunlink

#else  /* !_WINDOWS */
#endif /* _WINDOWS */

#else  /* !_I18N_ */

#define ufprintf fprintf
#define usprintf sprintf
#define usscanf  sscanf

#ifdef _WINDOWS
#      define uaccess  _access 
#else  /* !_WINDOWS */
#      define uaccess  access 
#endif /* _WINDOWS */

#define ufgets   fgets 
#define ufopen   fopen

#ifdef _WINDOWS
#      define uunlink _unlink
#else  /* !_WINDOWS */
#      define uunlink unlink
#endif /* _WINDOWS*/

#endif /* !_I18N_ */