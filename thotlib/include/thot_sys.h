/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Warning:
 * This module is part of the Thot library, which was originally
 * developed in French. That's why some comments are still in
 * French, but their translation is in progress and the full module
 * will be available in English in the next release.
 * 
 */
 
/*
 * thot_sys.h : system dependancies for Thot.
 */


/* "AMAYA_RESTART" : this definition is just used to debug the Amaya restart feature
and must be removed at the end of the debug */
#define AMAYA_RESTART

#ifndef THOT_SYS_H
#define THOT_SYS_H

/* #include <setjmp.h> */
#include <signal.h>
#include <math.h>

#ifndef M_PI
  #define M_PI            3.14159265358979323846  /* pi */
  #define M_PI_2          1.57079632679489661923  /* pi/2 */
#endif
/*
  A charater that starts an entity that Amaya cannot convert.
  It should be displayed as a & and should be exported as &
  instead of &amp;
*/
#define START_ENTITY        26
/*
  A character Unicode that cannot be displayed
*/
#define UNDISPLAYED_UNICODE 27
/**********************************************************/
#if defined(_WINDOWS) || defined(_CONSOLE)
  #include <errno.h>
  #include <limits.h>
  #include <fcntl.h>
  #include <io.h>

  /* MS-Windows platform */
  #ifndef WWW_MSWINDOWS
    #define WWW_MSWINDOWS
  #endif /* !WWW_MSWINDOWS */

  #ifndef _WINDOWS
    #define _WINDOWS
  #endif /* !_WINDOWS */
#endif /* !(defined(_WINDOWS) || defined(_CONSOLE)) */
/**********************************************************/

#include "sysdep.h"
#include "ustring.h"
#include "uconvert.h"

/* If const does work, or hasn't been redefined before */
#ifndef CONST
  #define CONST const
#endif

#ifndef MAX_PATH
  #define MAX_PATH HT_MAX_PATH	/* nombre de caracteres par liste de path */
#endif

#ifndef FALSE
  #ifdef __cplusplus
    #define FALSE false
  #else /* #ifdef __cplusplus */
    #define FALSE 0
  #endif /* #ifdef __cplusplus */
#endif

#ifndef TRUE
  #ifdef __cplusplus
    #define TRUE true
  #else /* #ifdef __cplusplus */
    #define TRUE (!FALSE)
  #endif /* #ifdef __cplusplus */
#endif



#ifdef _WINDOWS
  #define open   _open
  #define stat   _stat
  #define rmdir  _rmdir

  #define WM_ENTER (WM_USER)

  #include <direct.h>
  /*------------------------------------------------------GNUC--*/
  #if defined(__GNUC__) || defined(__GNUWIN32)
    #ifndef EXTERN
      #define EXTERN extern
    #endif

    /* Constants for PATHs */
    #define DIR_SEP  '/'
    #define PATH_SEP ':'
    #define DIR_STR  "/"
    #define PATH_STR ":"

    #define WC_DIR_SEP  '/'
    #define WC_PATH_SEP ':'
    #define WC_DIR_STR  "/"
    #define WC_PATH_STR ":"
  #else /* __GNUC__ */ /*---------------------------------GNUC--*/
    /* Ugly patches to cope with Visual C++ */
    /* preproccessor flags */
    #undef NOERROR

    /* lacking types */
    typedef char       *caddr_t;	/* may be TCHAR for UNICODE */
    /* added functions */
    void bzero (void *s, size_t n);
    #if !defined(_WX) && !defined(_WINDOWS) /* SG there is warnings if I define this function */
      int _getpid (void);
    #endif /* _WX */
    #ifdef __STDC__ // allready defined if __STDC__ is not defined
      #define isascii(c) __isascii(c)
    #endif /* #ifdef __STDC__ */
    #define strcasecmp(a, b) _stricmp(a, b)
    #define index(str, ch) strchr(str, ch)
    /* function mappings */
    #define stat _stat /* stat(a,b), struct stat and probably everything else */
    #define fstat(fileno, buf) _fstat(fileno, buf)
    #define getcwd(buffer, len) _getcwd(buffer, len)
    #define access(f,m) _access((f),(m))
    #define unlink(f) _unlink((f))
    #define snprintf _snprintf

    /* Constants for PATHs */
    #define DIR_SEP  '\\'
    #define PATH_SEP ';'
    #define DIR_STR  "\\"
    #define PATH_STR ";"

    #define WC_DIR_SEP  '\\'
    #define WC_PATH_SEP ';'
    #define WC_DIR_STR  "\\"
    #define WC_PATH_STR ";"
  #endif /* ! __GNUC__ */
  /*------------------------------------------------------GNUC--*/

  /* type mappings */
  #ifdef __cplusplus
    typedef bool      Boolean;	/* X11/Intrinsic.h */
    #define HAVE_BOOLEAN
    typedef bool      ThotBool;
    #define Bool	    bool	/* X11/Xlib.h */
    #define None	    0L	/* X11/X.h */
  #else /* #ifdef __cplusplus */
    typedef BOOL        Boolean;	/* X11/Intrinsic.h */
    #define HAVE_BOOLEAN
    typedef BOOL        ThotBool;
    #define Bool	    int	/* X11/Xlib.h */
    #define None	    0L	/* X11/X.h */
  #endif /* #ifdef __cplusplus */

  #define Dimension int

  #ifndef R_OK
    #define R_OK 4
  #endif
  #ifndef W_OK
    #define W_OK 2
  #endif
  #ifndef X_OK
    #define X_OK 0
  #endif

  /* added functions */
  #define ThotPid_get()	_getpid()
  #define ThotPid		int

  #ifdef __cplusplus /* WX use C++ */
    #ifndef False
      #define False false
    #endif
    #ifndef True
      #define True true
    #endif
  #else /* #ifdef __cplusplus */
    #ifndef False
      #define False 0
    #endif
    #ifndef True
      #define True 1
    #endif
  #endif /* #ifdef __cplusplus */

#endif /* _WINDOWS */


#if defined(_UNIX)
  /* Unix definitions */
  #define None	    0L	/* X11/X.h */
#ifdef _MACOS
  // Amaya crashes on http redirect when ThotBool is not an unsigned char
  typedef  unsigned char   ThotBool;
#else /* _MACOS */
  #ifdef __cplusplus
    typedef bool ThotBool;
  #else /* #ifdef __cplusplus */
    typedef unsigned char   ThotBool;
  #endif /* #ifdef __cplusplus */
#endif /* _MACOS */
#ifndef BOOL
  typedef ThotBool BOOL;
#endif
  #define ThotPid_get()	getpid()
  #define ThotPid		pid_t

  /* Constants for PATHs */
  #define DIR_SEP  '/'
  #define DIR_STR  "/"
  #define PATH_SEP ':'
  #define PATH_STR ":"

  #define WC_DIR_SEP  '/'
  #define WC_PATH_SEP ':'
  #define WC_DIR_STR  "/"
  #define WC_PATH_STR ":"
#endif /* #if defined(_UNIX) */

#define ___TEXT___(str) L##str
#ifndef TEXT
  #define TEXT(str) ___TEXT___(str)
#endif  /* TEXT */

#define __CR__  '\r'
#define EOS     '\0'
#define EOL     '\n'
#define TAB     '\t'
#define SPACE   ' '
#define BSPACE  '\b'

#define NBSP      0xA0
#define MAX_BYTES 6

/* we encourage using "long" to store pointer values, never use "int" ! */
#if !defined(__intptr_t_defined) && !defined(_INTPTR_T_DEFINED)
#define __intptr_t_defined
typedef           long   intptr_t;
#endif

#endif /* THOT_SYS_H */
