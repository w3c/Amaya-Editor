/* java_lang_Object.h
 * Java's base class - the Object.
 *
 * Copyright (c) 1997 T. J. Wilkinson & Associates, London, UK.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * Written by Tim Wilkinson <tim@tjwassoc.co.uk>
 */

#ifndef __java_lang_object_h
#define __java_lang_object_h

struct _dispatchTable;

typedef struct Hjava_lang_Object {
	struct _dispatchTable*  dtable;
	/* Data follows on immediately */
} Hjava_lang_Object;

#endif
