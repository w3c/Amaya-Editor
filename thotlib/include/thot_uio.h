#ifndef THOT_UIO_H
#define THOT_UIO_H

#ifdef _I18N_ /*****************************/

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

#else  /* !_I18N_ ****************************/

#define ufprintf fprintf
#define usprintf sprintf
#define usscanf  sscanf
#define ufopen   fopen
#define ugetenv getenv
#define urename rename

#ifdef _WINDOWS
#      define uaccess  _access 
#      define uopen   _open
#      define uunlink _unlink
#      define ustat   _stat
#      define umkdir  _mkdir
#      define urmdir  _rmdir
#      define ugetcwd _getcwd
#else  /* !_WINDOWS */
#      define uaccess  access 
#      define uopen   open
#      define ustat   stat
#      define uunlink unlink
#      define umkdir  mkdir 
#      define urmdir  rmdir
#      define ugetcwd getcwd
#endif /* _WINDOWS*/

#endif /* !_I18N_ ****************************/

extern STRING ufgets (STRING , int , FILE*);

#endif /* THOT_UIO_H */