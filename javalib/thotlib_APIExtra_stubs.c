/*
 * Kaffe stubs generated for class thotlib_Extra.
 * This class encapsulate call to the Thot lib api which could not
 * be built automatically by javastub program.
 */

#include <native.h>
#include "JavaTypes.h"
#include "thotlib_Extra.h"

/*
 * Java to C function TtaRemoveSchemaExtension stub.
 */
void
thotlib_Extra_TtaRemoveSchemaExtension(struct Hthotlib_Extra* none, 
      jint jdocument, jint jextension)
{
    int removedElements;
    int removedAttributes;
    JavaThotlibLock();
    TtaRemoveSchemaExtension((Document) jdocument, (SSchema) jextension,
                             &removedElements, &removedAttributes);
    JavaThotlibRelease();
}

/*
 * The C Callback interface.
 */
void
thotlib_Extra_Java2CCallback(struct Hthotlib_Extra* none, jlong callback,
                             struct Hjava_lang_Object* arg)
{
    Java2CCallback callback_func = (Java2CCallback)((void *) callback);

    JavaThotlibLock();
    callback_func((void *) arg);
    JavaThotlibRelease();
}

/*
 * Call the poll loop.
 */
void
thotlib_Extra_JavaPollLoop(struct Hthotlib_Extra* none)
{
    JavaPollLoop();
}

/*
 * Flush the X-Windows stream.
 */
void
thotlib_Extra_JavaXFlush(struct Hthotlib_Extra* none)
{
#ifndef _WINDOWS
    XFlush(TtaGetCurrentDisplay());
#endif
    TtaHandlePendingEvents();
}

/*
 * Java to C function Ttaxxx stub.
xxx
thotlib_Extra_Ttaxxx(struct Hthotlib_Extra* none, xxx)
{
}
 */

/*
 * Function to register all thotlib_Selection stubs.
 */
void register_thotlib_Extra_stubs(void)
{
	addNativeMethod("thotlib_Extra_TtaRemoveSchemaExtension",
	                thotlib_Extra_TtaRemoveSchemaExtension);
	addNativeMethod("thotlib_Extra_Java2CCallback",
	                thotlib_Extra_Java2CCallback);
	addNativeMethod("thotlib_Extra_JavaPollLoop",
	                thotlib_Extra_JavaPollLoop);
	addNativeMethod("thotlib_Extra_JavaXFlush",
	                thotlib_Extra_JavaXFlush);
/*
	addNativeMethod("thotlib_Extra_Ttaxxx", thotlib_Extra_Ttaxxx);
 */
}

