/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
#define	MAXCBUFSIZ	16384	/* 1024 x 16 */

typedef struct _CBuf *PtrCBuf;
typedef struct _CBuf {
			     int  CBufIndx;
			     char CBufData[MAXCBUFSIZ];
			   } CBuf ;

