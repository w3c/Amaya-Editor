#ifndef THOT_UIO_H
#define THOT_UIO_H

#ifdef _I18N_ 
#define usprintf swprintf
#else  /* !_I18N_ */
#define usprintf sprintf
#endif /* !_I18N_ */


#ifdef _I18N_ /*****************************/

#ifdef _WINDOWS

#define ufprintf fwprintf
#define usscanf  swscanf
#define uaccess  _waccess
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
#define usscanf  sscanf
#define ugetenv  getenv
#define urename  rename

#ifdef _WINDOWS
#      define uaccess _access 
#      define uopen   _open
#      define uunlink _unlink
#      define ustat   _stat
#      define umkdir  _mkdir
#      define urmdir  _rmdir
#      define ugetcwd _getcwd
#else  /* !_WINDOWS */
#      define uaccess access 
#      define uopen   open
#      define ustat   stat
#      define uunlink unlink
#      define umkdir  mkdir 
#      define urmdir  rmdir
#      define ugetcwd getcwd
#endif /* _WINDOWS*/

#endif /* !_I18N_ ****************************/

#if defined(_WINDOWS) && defined(_I18N_)

#define cus_fprintf fwprintf
#define cus_sprintf swprintf
#define cus_sscanf  swscanf
#define cus_access  _waccess
#define cus_fopen   _wfopen
#define cus_open    _wopen
#define cus_unlink  _wunlink
#define cus_stat    _wstat
#define cus_mkdir   _wmkdir
#define cus_rmdir   _wrmdir
#define cus_getenv  _wgetenv
#define cus_getcwd  _wgetcwd
#define cus_rename  _wrename

#else  /* !(defined(_WINDOWS) && defined(_I18N_)) */

#define cus_fprintf fprintf
#define cus_sprintf sprintf
#define cus_sscanf  sscanf
#define cus_fopen   fopen
#define cus_getenv  getenv
#define cus_rename  rename

#ifdef _WINDOWS
#      define cus_access _access 
#      define cus_open   _open
#      define cus_unlink _unlink
#      define cus_stat   _stat
#      define cus_mkdir  _mkdir
#      define cus_rmdir  _rmdir
#      define cus_getcwd _getcwd
#else  /* !_WINDOWS */
#      define cus_access access 
#      define cus_open   open
#      define cus_stat   stat
#      define cus_unlink unlink
#      define cus_mkdir  mkdir 
#      define cus_rmdir  rmdir
#      define cus_getcwd getcwd
#endif /* _WINDOWS*/

#endif /* !(defined(_WINDOWS) && defined(_I18N_)) */


#endif /* THOT_UIO_H */

#ifdef __STDC__
extern CHAR_T* ufgets (CHAR_T*, int, FILE*);
extern FILE*   ufopen (const CHAR_T*, const CHAR_T*);
#else  /* !__STDC__ */
extern CHAR_T* ufgets ();
extern FILE*   ufopen ();
#endif /* !__STDC__ */
