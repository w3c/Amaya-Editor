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

#if 0
#define	GC_INCREMENTAL
#endif

#include <jtypes.h>

/*
 * Some systems need specific exports for funcions to be shared.
 */
#if defined(__BORLANDC__)
#define	EXPORT(t)	t __export
#define	IMPORT(t)	extern t __import
#elif defined(__MSC)
#define	EXPORT(t)	t __export
#define	IMPORT(t)	extern __declspec(dllimport) t
#else
#define	EXPORT(t)	t
#define	IMPORT(t)	extern t
#endif

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

/* From gc.h */
#if !defined(__gc_h)
#define	__gc_h

#if defined(GC_INCREMENTAL)
typedef struct _gcHead {
	int			colour;
	int			size;
	struct _gcHead*		nextGrey;
	struct _gcHead*		nextRoot;
} gcHead;
#else
typedef struct _gcHead {
	int			idx;
} gcHead;
#endif

#endif

/* From object.h */
#if !defined(__object_h)
#define	__object_h
struct _object {
	gcHead			gc;
	struct _dispatchTable*	dtable;
	unsigned int		size;
	iMux			mux;
	iCv			cv;
};
#endif
#ifndef object
#define object struct _object
#endif

/* From baseClasses.h */
struct _stringClass {
	object		head;
	object*		value;
	int		offset;
	int		count;
};
#ifndef stringClass
#define stringClass struct _stringClass
#endif

/* Build an object handle */
#define	HandleTo(class)					\
	struct H##class {				\
		object base;				\
		struct Class##class data[1];		\
	}

/* Turn a handle into the real thing */
#define	unhand(o)	((o)->data)

/* Some internal machine object conversions to "standard" types. */
#define	Hjava_lang_Object	_object
#define	Hjava_lang_String	_stringClass
#define	Hjava_lang_Class	_classes

/* Array types */
typedef struct { object base; struct { jbyte body[1]; } data[1]; } HArrayOfByte;
typedef struct { object base; struct { jchar body[1]; } data[1]; } HArrayOfChar;
typedef struct { object base; struct { jdouble body[1]; } data[1]; } HArrayOfDouble;
typedef struct { object base; struct { jfloat body[1]; } data[1]; } HArrayOfFloat;
typedef struct { object base; struct { jint body[1]; } data[1]; } HArrayOfInt;
typedef struct { object base; struct { jshort body[1]; } data[1]; } HArrayOfShort;
typedef struct { object base; struct { jlong body[1]; } data[1]; } HArrayOfLong;
typedef struct { object base; struct { object* body[1]; } data[1]; } HArrayOfArray;
typedef struct { object base; struct { object* body[1]; } data[1]; } HArrayOfObject;

/* Get length of arrays */
#define	obj_length(_obj)	((_obj)->base.size)

/* Various useful function prototypes */
extern char* javaString2CString(struct Hjava_lang_String*, char*, int);
extern char* makeCString(struct Hjava_lang_String*);
extern struct Hjava_lang_String* makeJavaString(char*, int);

extern jword	do_execute_java_method(void*, object*, char*, char*, struct _methods*, int, ...);
extern jword	do_execute_java_class_method(char*, char*, char*, ...);
extern object* execute_java_constructor(void*, char*, struct _classes*, char*, ...);

extern void	SignalError(void*, char*, char*);

extern void	classname2pathname(char*, char*);

extern object* AllocObject(char*);
extern object* AllocArray(int, int);
extern object* AllocObjectArray(int, char*);

extern void	addExternalNativeFunc(char*, void*);
#endif
