/*
 * StubPreamble.h
 *
 * Copyright (c) 1996 T. J. Wilkinson & Associates, London, UK.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * Written by Tim Wilkinson <tim@tjwassoc.demon.co.uk>, 1996.
 */

#ifndef _stubpreamble_h
#define _stubpreamble_h

#include <stdio.h>
#include <native.h>

typedef union {
	jint		i;
	jlong 		l;
	jfloat		f;
	jdouble		d;
	jref		p;
} stack_item;

#define	return_int(_r_)		(_R_)->i = (_r_)
#define	return_long(_r_)	(_R_)->l = (_r_)
#define	return_ref(_r_)		(_R_)->p = (_r_)
#define	return_float(_r_)	(_R_)->f = (_r_)
#define	return_double(_r_)	(_R_)->d = (_r_)

#endif
