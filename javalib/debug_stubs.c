/*
 * Debug traps on C side.
 *
 * Daniel Veillard 1997
 */

#ifdef CONST
#undef CONST
#endif
#define CONST

#include "app.h"
#include "appaction.h"
#include "memory.h"
#include "JavaTypes.h"
#include "w3c_thotlib_Action.h"
#include "debug_stubs.h"

#include "JavaTypes_f.h"

void
w3c_thotlib_Element_Debug(struct Hw3c_thotlib_Element* el)
{
}

/*
 * Java to C function Ttaxxx stub.
xxx
w3c_thotlib_Extra_Ttaxxx(struct Hw3c_thotlib_Extra* none, xxx)
{
}
 */

/*
 * Function to register all w3c_thotlib_Selection stubs.
 */
void register_debug_stubs(void)
{
	addNativeMethod("w3c_thotlib_Element_Debug",
	                w3c_thotlib_Element_Debug);

/*
	addNativeMethod("w3c_thotlib_Extra_Ttaxxx", w3c_thotlib_Extra_Ttaxxx);
 */
}

