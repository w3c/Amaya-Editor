/*
 * Kaffe stubs generated for class thotlib_Extra.
 * This class encapsulate call to the Thot lib api which could not
 * be built automatically by javastub program.
 */

#include <native.h>
#include "JavaTypes.h"
#include "thotlib_Extra.h"

/*
 * Java to C function Ttaxxx stub.
 */
void
thotlib_Extra_TtaRemoveSchemaExtension(struct Hthotlib_Extra* none, 
      jint jdocument, jint jextension)
{
    int removedElements;
    int removedAttributes;
    TtaRemoveSchemaExtension((Document) jdocument, (SSchema) jextension,
                             &removedElements, &removedAttributes);
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
	addNativeMethod("thotlib_Extra_TtaRemoveSchemaExtension", thotlib_Extra_TtaRemoveSchemaExtension);
/*
	addNativeMethod("thotlib_Extra_Ttaxxx", thotlib_Extra_Ttaxxx);
 */
}

