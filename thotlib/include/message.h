
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

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

extern int          TtaGetMessageTable (char *, int);
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
