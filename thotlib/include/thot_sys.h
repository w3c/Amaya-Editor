/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
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

#include <setjmp.h>
#include <signal.h>
#include <math.h>
#define M_PI            3.14159265358979323846  /* pi */
#define M_PI_2          1.57079632679489661923  /* pi/2 */
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
#include "thot_uio.h"

/* If const does work, or hasn't been redefined before */
#ifndef CONST
#define CONST const
#endif

#ifndef MAX_PATH
#define MAX_PATH HT_MAX_PATH	/* nombre de caracteres par liste de path */
#endif

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE  (!FALSE)
#endif

/********************************************************WINDOWS**/
#ifdef _WINDOWS

#define WM_ENTER (WM_USER)

#include <direct.h>
/*------------------------------------------------------GNUC--*/
#if defined(__GNUC__) || defined(__GNUWIN32)
#ifndef EXTERN
#define EXTERN extern
#endif

/* Constants for PATHs */
#define DIR_SEP  TEXT('/')
#define PATH_SEP TEXT(':')
#define DIR_STR  TEXT("/")
#define PATH_STR TEXT(":")
#else /* __GNUC__ */ /*---------------------------------GNUC--*/
/* Ugly patches to cope with Visual C++ */
/* preproccessor flags */
#undef NOERROR

/* lacking types */
typedef char       *caddr_t;	/* may be TCHAR for UNICODE */
/* added functions */
void                bzero (void *s, size_t n);
int                 _getpid (void);
#define strcasecmp(a, b) _stricmp(a, b)
#define index(str, ch) strchr(str, ch)
/* function mappings */
#define stat _stat /* stat(a,b), struct stat and probably everything else */
#define fstat(fileno, buf) _fstat(fileno, buf)
#define getcwd(buffer, len) _getcwd(buffer, len)
#define access(f,m) _access((f),(m))
#define unlink(f) _unlink((f))

/* Constants for PATHs */
#define DIR_SEP  '\\'
#define PATH_SEP ';'
#define DIR_STR  "\\"
#define PATH_STR ";"
#endif /* ! __GNUC__ */
/*------------------------------------------------------GNUC--*/

/* type mappings */
typedef BOOL        Boolean;	/* X11/Intrinsic.h */
#define HAVE_BOOLEAN
typedef BOOL        ThotBool;
#define Bool	    int	/* X11/Xlib.h */
#define None	    0L	/* X11/X.h */
#define Bool	    int	/* X11/Xlib.h */

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

#ifndef False
#define False 0
#endif
#ifndef True
#define True 1
#endif

#define Dimension int

#else /* _WINDOWS *//***********************************WINDOWS**/
/* Unix definitions */
typedef unsigned char   ThotBool;
#define ThotPid_get()	getpid()
#define ThotPid		pid_t

/* Constants for PATHs */
#define DIR_SEP  TEXT('/')
#define DIR_STR  TEXT("/")
#define PATH_SEP TEXT(':')
#define PATH_STR TEXT(":")

#endif /* _WINDOWS */
/********************************************************WINDOWS**/
#if defined(_I18N_) || defined (__JIS__)
#   define ___TEXT___(str) L##str
#else /* !defined(_I18N_) && !defined (__JIS__) */
#     define ___TEXT___(str) str
#endif /* defined(_I18N_) || defined (__JIS__) */

#ifndef TEXT
#define TEXT(str) ___TEXT___(str)
#endif  /* TEXT */

#define _EMPTYSTR_ TEXT("")
#define __CR__  TEXT('\r')
#define EOS     TEXT('\0')
#define EOL     TEXT('\n')
#define TAB     TEXT('\t')
#define SPACE   TEXT(' ')
#define BSPACE  TEXT('\b')

#include "unicodeconsts.h"

#endif /* THOT_SYS_H */
