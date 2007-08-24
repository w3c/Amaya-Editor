/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
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

extern int      TtaGetMessageTable (CONST char *msgName, int msgNumber);
extern char    *TtaGetMessage (int, int);
extern void     TtaDisplaySimpleMessage (int, int, int);
extern void     TtaDisplaySimpleMessageString (int, int, int, char *);
extern void     TtaDisplaySimpleMessageNum (int, int, int, int);
extern void	TtaDisplayMessage (int msgType, const char *fmt, ...);

#endif /* __CEXTRACT__ */

#endif
