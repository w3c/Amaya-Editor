/*
 * kaffeine.h : interface for the low level integration of Kaffe
 *              runtime and the embedding application.
 */

#ifndef __KAFFEINE_H__
#define __KAFFEINE_H__

#include "JavaTypes.h"

/*
 * Kaffe runtime accesses not published in native.h
 */
extern void initialiseKaffe(void);
extern int threadedFileDescriptor(int fd);
extern void yieldThread();
extern int blockOnFile(int fd, int op);
extern int (*select_call)(int, fd_set*, fd_set*, fd_set*, struct timeval*);
extern int blockInts;
extern void yieldThread();
extern void reschedule();
extern void sleepThread(jlong time);

#endif /* __KAFFEINE_H__ */

