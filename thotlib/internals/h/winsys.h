#ifndef __WINSYS_H
#define __WINSYS_H

#if defined (WIN32)
#   define IS_WIN32 TRUE
#else
#   define IS_WIN32 FALSE
#endif

#define IS_NT      IS_WIN32 && (BOOL)(GetVersion () < 0x80000000)
#define IS_WIN32S  IS_WIN32 && (BOOL)(!(IS_NT) && (LOBYTE (LOWORD (GetVersion ())) < 4))
#define IS_WIN95   (BOOL)(!(IS_NT) && !(IS_WIN32S)) && IS_WIN32

#endif /* __WINSYS_H */