/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * thot_sys.h : system dependancies for Thot.
 */

#ifndef THOT_SYS_H
#define THOT_SYS_H

#include <setjmp.h>
#include <signal.h>
#include <math.h>

#include "sysdep.h"

#if defined(_WINDOWS) || defined(_CONSOLE)

/*
 * MS-Windows platform.
 */
#ifndef WWW_MSWINDOWS
#define WWW_MSWINDOWS
#endif /* !WWW_MSWINDOWS */

#else  /* defined(_WINDOWS) || defined(_CONSOLE) */
#if defined (WILLOWS) || defined(_WINDOWS)

/*
 * MS-Windows emulation on Unix.
 */
#ifndef _WINDOWS
#define _WINDOWS
#endif /* !_WINDOWS */

#else  /* defined (WILLOWS) || defined(_WINDOWS) */

/*
 * Motif/Intrinsic/X11 on Unix
 */
#ifndef WWW_XWINDOWS
#define WWW_XWINDOWS
#endif /* !WWW_XWINDOWS */

#endif /* !(defined (WILLOWS) || defined(_WINDOWS)) */
#endif /* !(defined(_WINDOWS) || defined(_CONSOLE)) */

#ifdef linux
/*
 * just to imitate the Sun allocator, until everything has been
 * cleaned with INSURE.
 */
#define malloc(s) calloc(1,s)
#endif

#ifndef MAX_PATH
#define MAX_PATH HT_MAX_PATH	/* nombre de caracteres par liste de path */
#endif


#ifndef TRUE
#define TRUE  1
#define FALSE 0
#endif /* !TRUE */

#ifdef WWW_MSWINDOWS

/* type mappings */
typedef char        Boolean;	/* X11/Intrinsic.h */
#define Bool		int	/* X11/Xlib.h */
#define None		0L	/* X11/X.h */

#ifndef __GNUC__

/*
 * Ugly patches to cope with Visual C++
 */

/* preproccessor flags */
#undef NOERROR

/* lacking types */
typedef char       *caddr_t;	/* may be TCHAR for UNICODE */

/* added functions */
void                bzero (void *s, size_t n);
int                 _getpid (void);

/* function mappings */
#define index(str, ch) strchr(str, ch)
#define stat _stat		/* stat(a,b), struct stat and probably everything else */
#define fstat(fileno, buf) _fstat(fileno, buf)
#define getcwd(buffer, len) _getcwd(buffer, len)
#define strcasecmp(a, b) _stricmp(a, b)
#define access(f,m) _access((f),(m))
#define unlink(f) _unlink((f))

#endif /* ! __GNUC__ */

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

#ifdef __GNUC__
#ifndef EXTERN
#define EXTERN extern
#endif
#endif

/*
 * Constants for PATHs
 */
#define DIR_SEP '\\'
#define DIR_STR "\\"
#define PATH_SEP ';'
#define PATH_STR ";"

#ifndef False
#define False 0
#endif
#ifndef True
#define True 1
#endif

#else  /* WWW_WINDOWS */

#ifdef _WINDOWS
typedef char        Boolean;	/* X11/Intrinsic.h */

#define Bool		int	/* X11/Xlib.h */
#define None		0L	/* X11/X.h */

#endif /* _WINDOWS */

#define ThotPid_get()	getpid()
#define ThotPid		pid_t

/*
 * Constants for PATHs
 */
#define DIR_SEP '/'
#define DIR_STR "/"
#define PATH_SEP ':'
#define PATH_STR ":"

#endif /* !WWW_WINDOWS */

#endif /* THOT_SYS_H */
