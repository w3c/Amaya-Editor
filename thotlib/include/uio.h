#include "ustring.h"
#include <stdio.h>
#ifdef __STDC__
extern STRING ufgets (STRING, int, FILE*);
#else  /* !__STDC__ */
extern STRING ufgets ();
#endif /* !__STDC__ */

#ifdef _I18N_

#ifdef _WINDOWS

#define ufprintf fwprintf
#define usprintf swprintf
#define usscanf  swscanf

#define uaccess  _waccess
#define ufopen   _wfopen
#define uopen    _wopen
#define uunlink  _wunlink
#define ustat    _wstat

#define umkdir   _wmkdir
#define urmdir   _wrmdir

#define ugetenv  _wgetenv
#define ugetcwd  _wgetcwd

#define urename  _wrename

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

#define ufopen   fopen

#ifdef _WINDOWS
#      define uopen   _open
#      define uunlink _unlink
#      define ustat   _stat
#      define umkdir  _mkdir
#      define urmdir  _rmdir
#      define ugetcwd _getcwd
#else  /* !_WINDOWS */
#      define uopen   open
#      define uunlink unlink
#      define ustat   stat
#      define umkdir  mkdir 
#      define urmdir  rmdir
#      define ugetcwd getcwd
#endif /* _WINDOWS*/

#define ugetenv getenv

#define urename rename

#endif /* !_I18N_ */
