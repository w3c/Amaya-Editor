/*
 * Debug traps on C side.
 *
 * Daniel Veillard 1997
 */

#include <native.h>
#include "app.h"
#include "appaction.h"
#include "memory.h"
#include "JavaTypes.h"
#include "thotlib_Action.h"
#include "debug_stubs.h"

#include "JavaTypes_f.h"

void
thotlib_Element_Debug(struct Hthotlib_Element* el)
{
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
void register_debug_stubs(void)
{
	addNativeMethod("thotlib_Element_Debug",
	                thotlib_Element_Debug);

/*
	addNativeMethod("thotlib_Extra_Ttaxxx", thotlib_Extra_Ttaxxx);
 */
}

