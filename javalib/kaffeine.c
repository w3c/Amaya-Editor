/*
 * Interface for the Kaffe Java interpreter.
 *    NOTE : This must NOT include the W-Windows stuff since
 *           Object and ObjectClass identifiers clashes
 *
 *  Daniel Veillard
 */

#include <stdio.h>
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

/*
 * This method is needed by the Kaffe interpreter.
 * What's happening when the memory is too low ?
 */

void
throwOutOfMemory ()
{
        if (OutOfMemoryError != NULL)
                throwException(OutOfMemoryError);
        fprintf (stderr, "(Insufficient memory)\n");
        exit (-1);
}

/*----------------------------------------------------------------------
   InitJava

   Initialize the Java Interpreter.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                InitJava (void)
#else
void                InitJava ()
#endif
{
    fprintf(stderr, "Initialize Java Runtime\n");

    /* Initialise */
    initialise();

    fprintf(stderr, "Java Runtime Initialized\n");
}

