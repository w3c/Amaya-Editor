#define	MAXCBUFSIZ	16384	/* 1024 x 16 */

typedef struct _CBuf *PtrCBuf;
typedef struct _CBuf {
			     int  CBufIndx;
			     char CBufData[MAXCBUFSIZ];
			   } CBuf ;

