/*
 * JavaX11Interf.h : global variables exported by the X-Windows
 *                   kaffe interface.
 */

#ifndef __JAVA_X11_INTERF_H__
#define __JAVA_X11_INTERF_H__

extern int x_window_socket;

/*
 * If DoJavaSelectPoll is selected, any JavaSelectCall on the
 * x-window socket will fail with value -1 if BreakJavaSelectPoll
 * is non zero. This is needed to do multi filedescriptor polling
 * without breaking the Kaffe threading model.
 */

extern int DoJavaSelectPoll;
extern int BreakJavaSelectPoll;

extern int ThotlibLockValue;
extern int XWindowSocketLockValue;
extern int XWindowSocketWaitValue;

#endif /* __JAVA_X11_INTERF_H__ */

