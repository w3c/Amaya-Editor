/* include/jtypes.h.  Generated automatically by configure.  */
/*
 * jtypes.h
 * Java types.
 *
 * Copyright (c) 1996 T. J. Wilkinson & Associates, London, UK.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * Written by Tim Wilkinson <tim@tjwassoc.demon.co.uk>, 1996.
 */

#ifndef __jtypes_h
#define __jtypes_h

#define	SIZEOF_INT 4
#define	SIZEOF_LONG 4
#define	SIZEOF_LONG_LONG 8
#define	SIZEOF___INT64 0
#define	SIZEOF_VOIDP 4

typedef float		jfloat;
typedef double		jdouble;
typedef	void*		jref;
typedef unsigned short	jchar;
typedef char		jbyte;
typedef short 		jshort;

#if SIZEOF_INT == 4
typedef int		jint;
#elif SIZEOF_LONG == 4
typedef long		jint;
#else
#error "sizeof(int) or sizeof(long) must be 4"
#endif

#if SIZEOF_LONG == 8
typedef long		jlong;
#elif SIZEOF_LONG_LONG == 8
typedef long long	jlong;
#elif SIZEOF___INT64 == 8
typedef __int64		jlong;
#else
#error "sizeof(long long) or sizeof(__int64) must be 8"
#endif

typedef	jchar		unicode;

/* An integer type big enough for a pointer or a 32-bit int/float. */
#if SIZEOF_VOIDP <= SIZEOF_INT
typedef jint		jword;
#elif SIZEOF_VOIDP <= SIZEOF_LONG
typedef jlong		jword;
#else
#error "both sizeof(int) and sizeof(long) < sizeof(void*)"
#endif

#endif
