/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
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
 
#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include "libmsg.h"

#define	LIB             0	/* Thot library messages table identifier    */
#define THOT            1

#define	INFO		1
#define	FATAL		2
#define	OVERHEAD	3
#define CONFIRM		4

/*extern int THOT ; */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern int          TtaGetMessageTable (CONST char *, int);
extern char        *TtaGetMessage (int, int);
extern void         TtaDisplaySimpleMessage (int, int, int);
extern void         TtaDisplaySimpleMessageString (int, int, int, char *);
extern void         TtaDisplaySimpleMessageNum (int, int, int, int);
extern void	    TtaDisplayMessage (int msgType, char *fmt, ...);
#else  /* __STDC__ */

extern int          TtaGetMessageTable ();
extern char        *TtaGetMessage ();
extern void         TtaDisplaySimpleMessage ();
extern void         TtaDisplaySimpleMessageString ();
extern void         TtaDisplaySimpleMessageNum ();
extern void	    TtaDisplayMessage ();

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */

#endif
