/*
 * Interface for the Kaffe Java interpreter.
 *    NOTE : This must NOT include the W-Windows stuff since
 *           Object and ObjectClass identifiers clashes
 *
 *  Daniel Veillard
 */

#include <stdio.h>
#include <string.h>
#include "thotlib_Interface_stubs.h"
#include "thotlib_Document_stubs.h"
#include "thotlib_Application_stubs.h"
#include "thotlib_Tree_stubs.h"
#include "thotlib_Presentation_stubs.h"
#include "thotlib_Language_stubs.h"
#include "thotlib_Registry_stubs.h"
#include "thotlib_Pivot_stubs.h"
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
static void register_stubs(void);

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
    char initClass[256];

    fprintf(stderr, "Initialize Java Runtime\n");

    /* Initialise */
    initialise();

    /* Register stubs */
    register_stubs();

    fprintf(stderr, "Java Runtime Initialized\n");

    /* Build the init class name */
    strcpy(initClass, app_name);
    strcat(initClass, "Init");

    /* Build an array of strings as the arguments */
    args = AllocObjectArray(1, "Ljava/lang/String;");

    /* Build each string and put into the array */
    str = (stringClass**)(args + 1);
    str[0] = makeJavaString(app_name, strlen(app_name));

    /* lauch the init class for the application */
    do_execute_java_class_method(initClass, "main",
                   "([Ljava/lang/String;)V", args);

    /* Start the application loop of events */
    do_execute_java_class_method("thotlib.Interface", "main",
                   "([Ljava/lang/String;)V", args);
}

/*
 * Register the thotlib stuff.
 */
static void register_stubs(void)
{
   register_thotlib_Interface_stubs();
   register_thotlib_Document_stubs();
   register_thotlib_Application_stubs();
   register_thotlib_Tree_stubs();
   register_thotlib_Presentation_stubs();
   register_thotlib_Language_stubs();
   register_thotlib_Registry_stubs();
   register_thotlib_Pivot_stubs();
}

