/*
 * Debug traps on C side.
 *
 * Daniel Veillard 1997
 */

#ifdef CONST
#undef CONST
#endif
#define CONST

#include "ustring.h"
#include "thot_sys.h"
#include "appstruct.h"
#include "appaction.h"
#include "memory.h"
#include "JavaTypes.h"
#include "org_w3c_thotlib_Action.h"
#include "debug_stubs.h"

#include "JavaTypes_f.h"

void
org_w3c_thotlib_Element_Debug(struct Horg_w3c_thotlib_Element* el)
{
}

/*
 * Java to C function Ttaxxx stub.
xxx
org_w3c_thotlib_Extra_Ttaxxx(struct Horg_w3c_thotlib_Extra* none, xxx)
{
}
 */

/*
 * Function to register all org_w3c_thotlib_Selection stubs.
 */
void register_debug_stubs(void)
{
	addNativeMethod("org_w3c_thotlib_Element_Debug",
	                org_w3c_thotlib_Element_Debug);

/*
	addNativeMethod("org_w3c_thotlib_Extra_Ttaxxx", org_w3c_thotlib_Extra_Ttaxxx);
 */
}

