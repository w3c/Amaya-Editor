/*
 * native.h
 * Native method support.
 *
 * Copyright (c) 1996 T. J. Wilkinson & Associates, London, UK.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * Written by Tim Wilkinson <tim@tjwassoc.demon.co.uk>, 1996.
 */

#ifndef __native_h
#define __native_h

#include <jtypes.h>

struct _methods;
struct _classes;
struct _thread;

/* From locks.h */
#if !defined(__locks_h)
#define	__locks_h
typedef struct _iMux {
	struct _thread*		holder;
	int			count;
	struct _thread*		muxWaiters;
} iMux;
typedef struct _iCv {
	struct _thread*		cvWaiters;
	struct _iMux*		mux;
} iCv;
#endif

/* From object.h */
#ifndef __object_h
typedef struct Hjava_lang_Object {
	struct _dispatchTable*	dtable;
	iMux			mux;
	iCv			cv;
} Hjava_lang_Object;
#endif

/* Build an object handle */
#define	HandleTo(class)					\
	typedef struct H##class {			\
		Hjava_lang_Object base;			\
		struct Class##class data[1];		\
	} H##class

/* Turn a handle into the real thing */
#define	unhand(o)	((o)->data)

/* Some internal machine object conversions to "standard" types. */
typedef	struct Hjava_lang_Class*	HClass;

/* Compare struct Array in object.h */
#define ARRAY_FIELDS \
	unsigned int		length;\
	double			align[0]

/* Array types */
typedef struct { Hjava_lang_Object base; ARRAY_FIELDS; struct { jbyte body[1]; } data[1]; } HArrayOfByte;
typedef struct { Hjava_lang_Object base; ARRAY_FIELDS; struct { jchar body[1]; } data[1]; } HArrayOfChar;
typedef struct { Hjava_lang_Object base; ARRAY_FIELDS; struct { jdouble body[1]; } data[1]; } HArrayOfDouble;
typedef struct { Hjava_lang_Object base; ARRAY_FIELDS; struct { jfloat body[1]; } data[1]; } HArrayOfFloat;
typedef struct { Hjava_lang_Object base; ARRAY_FIELDS; struct { jint body[1]; } data[1]; } HArrayOfInt;
typedef struct { Hjava_lang_Object base; ARRAY_FIELDS; struct { jshort body[1]; } data[1]; } HArrayOfShort;
typedef struct { Hjava_lang_Object base; ARRAY_FIELDS; struct { jlong body[1]; } data[1]; } HArrayOfLong;
typedef struct { Hjava_lang_Object base; ARRAY_FIELDS; struct { Hjava_lang_Object* body[1]; } data[1]; } HArrayOfArray;
typedef struct { Hjava_lang_Object base; ARRAY_FIELDS; struct { Hjava_lang_Object* body[1]; } data[1]; } HArrayOfObject;

/* Get length of arrays */
#define	obj_length(_obj)	((_obj)->length)

/* Get the strings */
#ifndef __baseclasses_h
#include <java_lang_String.h>
#endif

/* Various useful function prototypes */
extern char*	javaString2CString(Hjava_lang_String*, char*, int);
extern char*	makeCString(Hjava_lang_String*);
extern Hjava_lang_String* makeJavaString(char*, int);

extern jword	do_execute_java_method(void*, Hjava_lang_Object*, char*, char*, struct _methods*, int, ...);
extern jword	do_execute_java_class_method(char*, char*, char*, ...);
extern Hjava_lang_Object* execute_java_constructor(void*, char*, struct Hjava_lang_Class*, char*, ...);

extern void	SignalError(void*, char*, char*);

extern void	classname2pathname(char*, char*);

extern Hjava_lang_Object*	AllocObject(char*);
extern Hjava_lang_Object*	AllocArray(int, int);
extern Hjava_lang_Object*	AllocObjectArray(int, char*);

extern void	addNativeMethod(char*, void*);

#endif
