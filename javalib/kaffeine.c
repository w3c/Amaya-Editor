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

#include "thotlib_APIApplication_stubs.h"
#include "thotlib_APIDocument_stubs.h"
#include "thotlib_APITree_stubs.h"
#include "thotlib_APIContent_stubs.h"
#include "thotlib_APIAttribute_stubs.h"
#include "thotlib_APIReference_stubs.h"
#include "thotlib_APILanguage_stubs.h"
#include "thotlib_APIPresentation_stubs.h"
#include "thotlib_APIView_stubs.h"
#include "thotlib_APISelection_stubs.h"
#include "thotlib_APIInterface_stubs.h"
#include "thotlib_APIRegistry_stubs.h"
#include "thotlib_APIDialog_stubs.h"

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
   register_thotlib_APIApplication_stubs();
   register_thotlib_APIDocument_stubs();
   register_thotlib_APITree_stubs();
   register_thotlib_APIContent_stubs();
   register_thotlib_APIAttribute_stubs();
   register_thotlib_APIReference_stubs();
   register_thotlib_APILanguage_stubs();
   register_thotlib_APIPresentation_stubs();
   register_thotlib_APIView_stubs();
   register_thotlib_APISelection_stubs();
   register_thotlib_APIInterface_stubs();
   register_thotlib_APIRegistry_stubs();
}

