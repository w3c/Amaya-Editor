/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996-2001.
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

extern int          TtaGetMessageTable (CONST CHAR_T* msgName, int msgNumber);
extern STRING       TtaGetMessage (int, int);
extern void         TtaDisplaySimpleMessage (int, int, int);
extern void         TtaDisplaySimpleMessageString (int, int, int, STRING);
extern void         TtaDisplaySimpleMessageNum (int, int, int, int);
extern void	        TtaDisplayMessage (int msgType, STRING fmt, ...);

#endif /* __CEXTRACT__ */

#endif
