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
#if defined (WILLOWS) || defined(NEW_WILLOWS)

/*
 * MS-Windows emulation on Unix.
 */
#ifndef NEW_WILLOWS
#define NEW_WILLOWS
#endif /* !NEW_WILLOWS */

#else  /* defined (WILLOWS) || defined(NEW_WILLOWS) */

/*
 * Motif/Intrinsic/X11 on Unix
 */
#ifndef WWW_XWINDOWS
#define WWW_XWINDOWS
#endif /* !WWW_XWINDOWS */

#endif /* !(defined (WILLOWS) || defined(NEW_WILLOWS)) */
#endif /* !(defined(_WINDOWS) || defined(_CONSOLE)) */

#ifdef linux
/*
 * just to imitate the Sun allocator, until Ceverything has been
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

/* preproccessor flags */
#undef NOERROR

/* type mappings */
typedef char        Boolean;	/* X11/Intrinsic.h */

#define Bool		int	/* X11/Xlib.h */
#define None		0L	/* X11/X.h */
/* #define MAX_MENU 20 */
typedef char       *caddr_t;	/* may be TCHAR for UNICODE */

#define ThotPid		int


/* added functions */
void                bzero (void *s, size_t n);
int                 _getpid (void);

/* function mappings */
#define index(str, ch) strchr(str, ch)
#define stat _stat		/* stat(a,b), struct stat and probably everything else */
#define fstat(fileno, buf) _fstat(fileno, buf)
#define getcwd(buffer, len) _getcwd(buffer, len)

#define strcasecmp(a, b) _stricmp(a, b)
#define ThotPid_get()	_getpid()

/*
 * Constants for PATHs
 */
#define DIR_SEP '\\'
#define DIR_STR "\\"
#define PATH_SEP ';'
#define PATH_STR ";"

#else  /* WWW_WINDOWS */

#ifdef NEW_WILLOWS
typedef char        Boolean;	/* X11/Intrinsic.h */

#define Bool		int	/* X11/Xlib.h */
#define None		0L	/* X11/X.h */
#endif /* NEW_WILLOWS */

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
