#ifdef _I18N_

#ifdef _WINDOWS

#define ufprintf fwprintf
#define usprintf swprintf
#define usscanf  swscanf

#define uaccess  _waccess
#define ufgets   fgetws
#define ufopen   _wfopen
#define uunlink  _wunlink

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

#define ufgets   fgets 
#define ufopen   fopen

#ifdef _WINDOWS
#      define uunlink _unlink
#      define umkdir  _mkdir
#      define urmdir  _rmdir
#      define ugetcwd _getcwd
#else  /* !_WINDOWS */
#      define uunlink unlink
#      define umkdir  mkdir 
#      define urmdir  rmdir
#      define ugetcwd getcwd
#endif /* _WINDOWS*/

#define ugetenv getenv

#define urename rename

#endif /* !_I18N_ */