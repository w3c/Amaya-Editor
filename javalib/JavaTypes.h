/*
 * Header describing the signatures for the conversion
 * funtions between internal types and external ones.
 */

#ifndef _Included_Java_Type_H_
#define _Included_Java_Type_H_

#include "native.h"

typedef (*Java2CCallback) (void *arg);

extern void addNativeMethod(char*, void*);

#include "thot_gui.h"
#include "thot_sys.h"
#include "tree.h"
#include "attribute.h"
#include "presentation.h"
#include "language.h"

/*
 * Includes auto-generated interface Java classes.
 */

#include "java_lang_StringBuffer.h"
#include "thotlib_Document.h"
#include "thotlib_SSchema.h"
#include "thotlib_Element.h"
#include "thotlib_ElementType.h"
#include "thotlib_Attribute.h"
#include "thotlib_AttributeType.h"
#include "thotlib_PRule.h"
#include "thotlib_Language.h"
#include "thotlib_IntPtr.h"

/*
 * Includes auto-generated interface to stubs.
 */
#include "thotlib_APIApplication.h"
#include "thotlib_APIDocument.h"
#include "thotlib_APITree.h"
#include "thotlib_APIContent.h"
#include "thotlib_APIAttribute.h"
#include "thotlib_APIReference.h"
#include "thotlib_APILanguage.h"
#include "thotlib_APIPresentation.h"
#include "thotlib_APIView.h"
#include "thotlib_APISelection.h"
#include "thotlib_APIInterface.h"
#include "thotlib_APIRegistry.h"
#include "thotlib_APIDialog.h"
#include "amaya_APIAmayaMsg.h"
#include "amaya_APIJavaAmaya.h"

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

#define thotlib_APIApplication_LOCK() JavaThotlibLock()
#define thotlib_APIAttribute_LOCK() JavaThotlibLock()
#define thotlib_APIContent_LOCK() JavaThotlibLock()
#define thotlib_APIDialog_LOCK() JavaThotlibLock()
#define thotlib_APIInterface_LOCK() JavaThotlibLock()
#define thotlib_APILanguage_LOCK() JavaThotlibLock()
#define thotlib_APIPresentation_LOCK() JavaThotlibLock()
#define thotlib_APIReference_LOCK() JavaThotlibLock()
#define thotlib_APIRegistry_LOCK() JavaThotlibLock()
#define thotlib_APISelection_LOCK() JavaThotlibLock()
#define thotlib_APITree_LOCK() JavaThotlibLock()
#define thotlib_APIView_LOCK() JavaThotlibLock()
#define amaya_APIJavaAmaya_LOCK() JavaThotlibLock()
#define amaya_APIAmayaMsg_LOCK() JavaThotlibLock()

#define thotlib_APIApplication_UNLOCK() JavaThotlibRelease()
#define thotlib_APIAttribute_UNLOCK() JavaThotlibRelease()
#define thotlib_APIContent_UNLOCK() JavaThotlibRelease()
#define thotlib_APIDialog_UNLOCK() JavaThotlibRelease()
#define thotlib_APIInterface_UNLOCK() JavaThotlibRelease()
#define thotlib_APILanguage_UNLOCK() JavaThotlibRelease()
#define thotlib_APIPresentation_UNLOCK() JavaThotlibRelease()
#define thotlib_APIReference_UNLOCK() JavaThotlibRelease()
#define thotlib_APIRegistry_UNLOCK() JavaThotlibRelease()
#define thotlib_APISelection_UNLOCK() JavaThotlibRelease()
#define thotlib_APITree_UNLOCK() JavaThotlibRelease()
#define thotlib_APIView_UNLOCK() JavaThotlibRelease()
#define amaya_APIJavaAmaya_UNLOCK() JavaThotlibRelease()
#define amaya_APIAmayaMsg_UNLOCK() JavaThotlibRelease()

#endif /* _Included_Java_Type_H_ */
