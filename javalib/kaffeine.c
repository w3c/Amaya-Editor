/*
 * Interface for the Kaffe Java interpreter.
 *    NOTE : This must NOT include the W-Windows stuff since
 *           Object and ObjectClass identifiers clashes
 *
 *  Daniel Veillard
 */

#include <stdio.h>
#include <string.h>
#include "StubPreamble.h"
#include "jtypes.h"
#include "native.h"

/***********************
#include "config.h"
#include "gtypes.h"
#include "access.h"
#include "object.h"
#include "constants.h"
#include "classMethod.h"
#include "baseClasses.h"
#include "thread.h"
#include "support.h"
#include "slib.h"
#include "errors.h"
#include "exception.h"
 *************************/

#define BISS_AWT "-Dawt.toolkit=biss.awt.kernel.Toolkit"

extern void initialise(void);

/*
 * This method is needed by the Kaffe interpreter.
 * What's happening when the memory is too low ?
 */

void
throwOutOfMemory ()
{
/*************************
        if (OutOfMemoryError != NULL)
                throwException(OutOfMemoryError);
 *************************/
        fprintf (stderr, "(Insufficient memory)\n");
        exit (-1);
}

/*----------------------------------------------------------------------
   InitJava

   Initialize the Java Interpreter.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                InitJava (char *app_name)
#else
void                InitJava (app_name)
char *app_name;
#endif
{
    object* args;
    stringClass** str;
    char startupClass[256];

    fprintf(stderr, "Initialize Java Runtime\n");

    /* Initialise */
    initialise();

    /* Register stubs */
    register_stubs();

    fprintf(stderr, "Java Runtime Initialized\n");

    /* Build the init class name */
    strcpy(startupClass, app_name);
    strcat(startupClass, "Init");

    /* Build an array of strings as the arguments */
    args = AllocObjectArray(4, "Ljava/lang/String;");

    /* Build each string and put into the array */
    str = (stringClass**)(args + 1);
    str[0] = makeJavaString("1", 1);
    str[1] = makeJavaString("2", 1);
    str[2] = makeJavaString("3", 1);
    str[3] = makeJavaString("4", 1);

    /* lauch the init class */
    do_execute_java_class_method(startupClass, "main",
                   "([Ljava/lang/String;)V", args);
}

void HelloWorld_tst_thread()
{
   fprintf(stderr,"tst_thread() called\n");
}

void register_stubs(void)
{
   addExternalNativeFunc("HelloWorld_tst_thread", HelloWorld_tst_thread);
}

