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
	int			__align__;
} iMux;
typedef struct _iCv {
	struct _thread*		cvWaiters;
	struct _iMux*		mux;
} iCv;
#endif

/* From object.h */
#if !defined(__object_h)
#define	__object_h
struct _object {
	struct _dispatchTable*	dtable;
	unsigned int		size;
	iMux			mux;
	iCv			cv;
};
#endif
#ifndef KaffeObject
struct _object;
#define	KaffeObject struct _object
#endif

/* Build an object handle */
#define	HandleTo(class)					\
	typedef struct H##class {			\
		KaffeObject base;				\
		struct Class##class data[1];		\
	} H##class

/* Turn a handle into the real thing */
#define	unhand(o)	((o)->data)

/* Some internal machine object conversions to "standard" types. */
#define	Hjava_lang_Object	_object
#define	Hjava_lang_Class	_classes

#if 0
typedef struct Hjava_lang_KaffeObject HObject;
typedef struct Hjava_lang_Class Class;
#endif

/* Array types */
typedef struct { KaffeObject base; struct { jbyte body[1]; } data[1]; } HArrayOfByte;
typedef struct { KaffeObject base; struct { jchar body[1]; } data[1]; } HArrayOfChar;
typedef struct { KaffeObject base; struct { jdouble body[1]; } data[1]; } HArrayOfDouble;
typedef struct { KaffeObject base; struct { jfloat body[1]; } data[1]; } HArrayOfFloat;
typedef struct { KaffeObject base; struct { jint body[1]; } data[1]; } HArrayOfInt;
typedef struct { KaffeObject base; struct { jshort body[1]; } data[1]; } HArrayOfShort;
typedef struct { KaffeObject base; struct { jlong body[1]; } data[1]; } HArrayOfLong;
typedef struct { KaffeObject base; struct { KaffeObject* body[1]; } data[1]; } HArrayOfArray;
typedef struct { KaffeObject base; struct { KaffeObject* body[1]; } data[1]; } HArrayOfObject;

/* Get length of arrays */
#define	obj_length(_obj)	((_obj)->base.size)

/* Get the strings */
#include <java_lang_String.h>
#if 0
typedef struct Hjava_lang_String HString;
#endif

/* Various useful function prototypes */
extern char* javaString2CString(struct Hjava_lang_String*, char*, int);
extern char* makeCString(struct Hjava_lang_String*);
extern struct Hjava_lang_String* makeJavaString(char*, int);

extern jword	do_execute_java_method(void*, KaffeObject*, char*, char*, struct _methods*, int, ...);
extern jword	do_execute_java_class_method(char*, char*, char*, ...);
extern KaffeObject* execute_java_constructor(void*, char*, struct _classes*, char*, ...);

extern void	SignalError(void*, char*, char*);

extern void	classname2pathname(char*, char*);

extern KaffeObject*	AllocObject(char*);
extern KaffeObject*	AllocArray(int, int);
extern KaffeObject*	AllocObjectArray(int, char*);

extern void	addNativeMethod(char*, void*);

#endif
