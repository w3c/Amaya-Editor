/* java_lang_String.h
 * Java's Strings.
 *
 * Copyright (c) 1997 T. J. Wilkinson & Associates, London, UK.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * Written by Tim Wilkinson <tim@tjwassoc.demon.co.uk>
 */

#ifndef __java_lang_string_h
#define __java_lang_string_h

/* From baseClasses.h */
struct _stringClass {
	object base;
	struct {
		HArrayOfChar*	value;
		jint		offset;
		jint		count;
	} data[1];
};
#ifndef stringClass
#define stringClass struct _stringClass
#endif
#define	Hjava_lang_String	_stringClass

/* Get length of string */
#define	javaStringLength(_str)	(unhand(_str)->count)

#endif
