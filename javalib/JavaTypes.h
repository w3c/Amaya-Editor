/*
 * Header describing the signatures for the conversion
 * funtions between internal types and external ones.
 *
 * Daniel Veillard 1997
 */

#ifndef _Included_Java_Type_H_
#define _Included_Java_Type_H_

typedef void (*Java2CCallback) (void *arg);

extern void addNativeMethod(char*, void*);

/*
 * Since there is no way to emulate const in java, all the signatures
 * extracted by kaffeh can't use "const" iven if the original header
 * was using const. To avoid signatures clashes between the original
 * header and the one extracted from the classe on need to disable CONST
 * when compiling Java stubs.
 */

#ifdef CONST
#undef CONST
#endif
#define CONST

#include "thot_gui.h"
#include "thot_sys.h"
#include "tree.h"
#include "attribute.h"
#include "presentation.h"
#include "language.h"

#include "native.h"

/*
 * Includes auto-generated interface Java classes.
 */

#include "java_lang_StringBuffer.h"
#include "w3c_thotlib_Document.h"
#include "w3c_thotlib_SSchema.h"
#include "w3c_thotlib_Element.h"
#include "w3c_thotlib_ElementType.h"
#include "w3c_thotlib_Attribute.h"
#include "w3c_thotlib_AttributeType.h"
#include "w3c_thotlib_PRule.h"
#include "w3c_thotlib_Language.h"
#include "w3c_thotlib_IntPtr.h"

/*
 * Includes auto-generated interface to stubs.
 */
#include "w3c_thotlib_APIApplication.h"
#include "w3c_thotlib_APIDocument.h"
#include "w3c_thotlib_APITree.h"
#include "w3c_thotlib_APIContent.h"
#include "w3c_thotlib_APIAttribute.h"
#include "w3c_thotlib_APIReference.h"
#include "w3c_thotlib_APILanguage.h"
#include "w3c_thotlib_APIPresentation.h"
#include "w3c_thotlib_APIView.h"
#include "w3c_thotlib_APISelection.h"
#include "w3c_thotlib_APIInterface.h"
#include "w3c_thotlib_APIRegistry.h"
#include "w3c_thotlib_APIDialog.h"
#include "w3c_amaya_APIAmayaMsg.h"
#include "w3c_amaya_APIJavaAmaya.h"

/*
 * The signatures of the conversion functions.
 */
#include "JavaTypes_f.h"
#include "kaffeine_f.h"

/*
 * Setup the Lock/Release macros
 */

extern void JavaThotlibLock();
extern void JavaThotlibRelease();

#define w3c_thotlib_APIApplication_LOCK() JavaThotlibLock()
#define w3c_thotlib_APIAttribute_LOCK() JavaThotlibLock()
#define w3c_thotlib_APIContent_LOCK() JavaThotlibLock()
#define w3c_thotlib_APIDocument_LOCK() JavaThotlibLock()
#define w3c_thotlib_APIDialog_LOCK() JavaThotlibLock()
#define w3c_thotlib_APIInterface_LOCK() JavaThotlibLock()
#define w3c_thotlib_APILanguage_LOCK() JavaThotlibLock()
#define w3c_thotlib_APIPresentation_LOCK() JavaThotlibLock()
#define w3c_thotlib_APIReference_LOCK() JavaThotlibLock()
#define w3c_thotlib_APIRegistry_LOCK() JavaThotlibLock()
#define w3c_thotlib_APISelection_LOCK() JavaThotlibLock()
#define w3c_thotlib_APITree_LOCK() JavaThotlibLock()
#define w3c_thotlib_APIView_LOCK() JavaThotlibLock()
#define w3c_amaya_APIJavaAmaya_LOCK() JavaThotlibLock()
#define w3c_amaya_APIAmayaMsg_LOCK() JavaThotlibLock()

#define w3c_thotlib_APIApplication_UNLOCK() JavaThotlibRelease()
#define w3c_thotlib_APIAttribute_UNLOCK() JavaThotlibRelease()
#define w3c_thotlib_APIContent_UNLOCK() JavaThotlibRelease()
#define w3c_thotlib_APIDocument_UNLOCK() JavaThotlibRelease()
#define w3c_thotlib_APIDialog_UNLOCK() JavaThotlibRelease()
#define w3c_thotlib_APIInterface_UNLOCK() JavaThotlibRelease()
#define w3c_thotlib_APILanguage_UNLOCK() JavaThotlibRelease()
#define w3c_thotlib_APIPresentation_UNLOCK() JavaThotlibRelease()
#define w3c_thotlib_APIReference_UNLOCK() JavaThotlibRelease()
#define w3c_thotlib_APIRegistry_UNLOCK() JavaThotlibRelease()
#define w3c_thotlib_APISelection_UNLOCK() JavaThotlibRelease()
#define w3c_thotlib_APITree_UNLOCK() JavaThotlibRelease()
#define w3c_thotlib_APIView_UNLOCK() JavaThotlibRelease()
#define w3c_amaya_APIJavaAmaya_UNLOCK() JavaThotlibRelease()
#define w3c_amaya_APIAmayaMsg_UNLOCK() JavaThotlibRelease()

#endif /* _Included_Java_Type_H_ */
