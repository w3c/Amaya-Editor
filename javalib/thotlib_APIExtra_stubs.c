/*
 * Kaffe stubs generated for class thotlib_Extra.
 * This class encapsulate call to the Thot lib api which could not
 * be built automatically by javastub program.
 */

#include <native.h>
#include "app.h"
#include "appaction.h"
#include "memory.h"
#include "application.h"
#include "JavaTypes.h"
#include "thotlib_Action.h"
#include "thotlib_Extra.h"
#include "amaya_HTTPRequest.h"

#include "JavaTypes_f.h"

/*
 * Callback for amaya/HTTPRequest
 */
void
amaya_HTTPRequest_Callback(struct Hamaya_HTTPRequest* none,
                           struct Hamaya_HTTPRequest* arg)
{
    Java2CCallback callback_func = (Java2CCallback)
                        FetchPtrFromJavaVM(&(unhand(arg)->callback));

    JavaThotlibLock();
    callback_func((void *) arg);
    JavaThotlibRelease();
}

/*
 * The C Callback interface.
 */
void
thotlib_Extra_Java2CCallback(struct Hthotlib_Extra* none,
                             struct Hjava_lang_Object* arg, jlong callback)
{
    Java2CCallback callback_func = (Java2CCallback)
                        JavaLong2CPtr(callback);

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
    JavaThotlibLock();
    JavaPollLoop();
    JavaThotlibRelease();
}

/*
 * Stop a poll loop.
 */
void
thotlib_Extra_JavaStopPoll(struct Hthotlib_Extra* none)
{
    JavaStopPoll();
}

/*
 * Call a Java Applet entry point.
 */
jint
thotlib_Extra_JavaStartApplet(struct Hthotlib_Extra* none,
                              struct Hjava_lang_String* class, 
			      struct Hjava_lang_String* signature,
			      jint doc,
			      HArrayOfObject* args)
{
    jword res;
    char classname[300];
    char sig[300];

    javaString2CString(class, classname, sizeof(classname));
    javaString2CString(signature, sig, sizeof(sig));

    if (!strncmp(sig, "(I[Ljava/lang/String;)", 22))
	res = do_execute_java_class_method(classname, "main", sig, doc, args);
    else if (!strncmp(sig, "(I)", 3))
	res = do_execute_java_class_method(classname, "main", sig, doc);
    else
	res = do_execute_java_class_method(classname, "main", sig, args);
        

    return(res);
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
    JavaThotlibLock();
    TtaHandlePendingEvents();
    JavaThotlibRelease();
}

/*
 * The User Action' callback stuff.
 */

static int
thotlib_Extra_JavaActionEventCallback(void *arg, NotifyEvent *event)
{
    jword res = 0;

    JavaThotlibRelease();
    if ((arg == NULL) || (event == NULL)) return(0);
    switch(event->event) {
	case TteAttrMenu:
	case TteAttrCreate:
	case TteAttrModify:
	case TteAttrRead:
	case TteAttrSave:
	case TteAttrExport:
	case TteAttrDelete: {
	    NotifyAttribute *ev = (NotifyAttribute *) event;
	    res = do_execute_java_method(NULL, (Hjava_lang_Object*) arg,
		     "callbackAttribute", "(IIJJJI)I", NULL, FALSE,
		     ev->event, ev->document,
		     CPtr2JavaLong(ev->element),
		     CPtr2JavaLong(ev->attribute),
		     CPtr2JavaLong(ev->attributeType.AttrSSchema),
		     ev->attributeType.AttrTypeNum);
	    break;
	}
	case TteElemMenu:
	case TteElemNew:
	case TteElemRead:
	case TteElemSave:
	case TteElemExport:
	case TteElemDelete:
	case TteElemSelect:
	case TteElemExtendSelect:
	case TteElemActivate:
	case TteElemInclude:
	case TteElemCopy:
	case TteElemChange:
	case TteElemMove:
	case TteElemMouseOver:
	case TteElemMouseOut: {
	    NotifyElement *ev = (NotifyElement *) event;
	    res = do_execute_java_method(NULL, (Hjava_lang_Object*) arg,
		     "callbackElement", "(IIJJII)I", NULL, FALSE,
		     ev->event, ev->document,
		     CPtr2JavaLong(ev->element),
		     CPtr2JavaLong(ev->elementType.ElSSchema),
		     ev->elementType.ElTypeNum,
		     ev->position);
	    break;
	}
	case TteElemPaste:
	case TteElemGraphModify: {
	    NotifyOnValue *ev = (NotifyOnValue *) event;
	    res = do_execute_java_method(NULL, (Hjava_lang_Object*) arg,
		     "callbackValue", "(IIJJI)I", NULL, FALSE,
		     ev->event, ev->document,
		     CPtr2JavaLong(ev->element),
		     CPtr2JavaLong(ev->target),
		     ev->value);
	    break;
	}
	case TteElemSetReference:
	case TteElemTextModify: {
	    NotifyOnTarget *ev = (NotifyOnTarget *) event;
	    res = do_execute_java_method(NULL, (Hjava_lang_Object*) arg,
		     "callbackTarget", "(IIJJI)I", NULL, FALSE,
		     ev->event, ev->document,
		     CPtr2JavaLong(ev->element),
		     CPtr2JavaLong(ev->target),
		     ev->targetdocument);
	    break;
	}
	case TteElemTransform: {
	    NotifyOnElementType *ev = (NotifyOnElementType *) event;
	    res = do_execute_java_method(NULL, (Hjava_lang_Object*) arg,
		     "callbackElementType", "(IIJJIJI)I", NULL, FALSE,
		     ev->event, ev->document,
		     CPtr2JavaLong(ev->element),
		     CPtr2JavaLong(ev->elementType.ElSSchema),
		     ev->elementType.ElTypeNum,
		     CPtr2JavaLong(ev->targetElementType.ElSSchema),
		     ev->targetElementType.ElTypeNum);
	    break;
	}
	case TtePRuleCreate:
	case TtePRuleModify:
	case TtePRuleDelete: {
	    NotifyPresentation *ev = (NotifyPresentation *) event;
	    res = do_execute_java_method(NULL, (Hjava_lang_Object*) arg,
		     "callbackPresentation", "(IIJJI)I", NULL, FALSE,
		     ev->event, ev->document,
		     CPtr2JavaLong(ev->element),
		     CPtr2JavaLong(ev->pRule),
		     ev->pRuleType);
	    break;
	}
	case TteDocOpen:
	case TteDocCreate:
	case TteDocClose:
	case TteDocSave:
	case TteDocExport:
	case TteViewOpen:
	case TteViewClose: {
	    NotifyDialog *ev = (NotifyDialog *) event;
	    res = do_execute_java_method(NULL, (Hjava_lang_Object*) arg,
		     "callbackDialog", "(III)I", NULL, FALSE,
		     ev->event, ev->document, ev->view);
	    break;
	}
	case TteViewScroll:
	case TteViewResize: {
	    NotifyWindow *ev = (NotifyWindow *) event;
	    res = do_execute_java_method(NULL, (Hjava_lang_Object*) arg,
		     "callbackWindow", "(IIIII)I", NULL, FALSE,
		     ev->event, ev->document, ev->view,
		     ev->verticalValue, ev->horizontalValue);
	    break;
	}
	case TteInit:
	case TteExit: {
	    NotifyEvent *ev = (NotifyEvent *) event;
	    res = do_execute_java_method(NULL, (Hjava_lang_Object*) arg,
		     "callbackNotify", "(I)I", NULL, FALSE,
		     ev->event);
	    break;
	}
	case TteNull:
	case TteElemFetchInclude:
	case TteDocTmpOpen:
	    res = 0;
	    break;
    }
    JavaThotlibLock();
    return((int) res);
}

static int
thotlib_Extra_JavaActionMenuCallback(void *arg, int doc, int view)
{
    jword res;

    JavaThotlibRelease();
    res = do_execute_java_method(NULL, (Hjava_lang_Object*) arg,
		     "callbackMenu", "(II)I", NULL, FALSE, doc, view);
    JavaThotlibLock();
    return((int) res);
}

/*
 * The User Action' registering stuff.
 */

void
thotlib_Extra_JavaRegisterAction(struct Hthotlib_Extra* none,
                                 struct Hthotlib_Action* handler,
				 struct Hjava_lang_String* actionName)
{
    char actionname[300];

    if ((handler == NULL) || (actionName == NULL)) return;
    javaString2CString(actionName, actionname, sizeof(actionname));

    JavaThotlibLock();
    TteAddUserAction (TtaStrdup(actionname),
                      thotlib_Extra_JavaActionEventCallback,
		      (void *) handler);
    JavaThotlibRelease();
}

void
thotlib_Extra_JavaRegisterMenuAction(struct Hthotlib_Extra* none,
                                 struct Hthotlib_Action* handler,
				 struct Hjava_lang_String* actionName)
{
    char actionname[300];

    if ((handler == NULL) || (actionName == NULL)) return;
    javaString2CString(actionName, actionname, sizeof(actionname));

    JavaThotlibLock();
    TteAddUserMenuAction (TtaStrdup(actionname),
                          thotlib_Extra_JavaActionMenuCallback,
                          (void *) handler);
    JavaThotlibRelease();
}

/*
 * The User Action' deregistration stuff.
 */

void
thotlib_Extra_JavaUnregisterAction(struct Hthotlib_Extra* none,
				 struct Hjava_lang_String* actionName)
{
    char actionname[300];

    if (actionName == NULL) return;
    javaString2CString(actionName, actionname, sizeof(actionname));

    JavaThotlibLock();
    TteAddUserAction (TtaStrdup(actionname), NULL, NULL);
    JavaThotlibRelease();
}

void
thotlib_Extra_JavaUnregisterMenuAction(struct Hthotlib_Extra* none,
				 struct Hjava_lang_String* actionName)
{
    char actionname[300];

    if (actionName == NULL) return;
    javaString2CString(actionName, actionname, sizeof(actionname));

    JavaThotlibLock();
    TteAddUserMenuAction (TtaStrdup(actionname), NULL, NULL);
    JavaThotlibRelease();
}

/*
 * Adding an action dynamically, both for the the Editor or for
 * a specific DTD given it's name.
 */

void
thotlib_Extra_AddEditorActionEvent(struct Hthotlib_Extra* none,
       struct Hjava_lang_String* actionName, jint eventType,
       jint typeId, jint /* bool */ pre) {
    char actionname[300];
    boolean is_pre;
    PtrEventsSet EditorEvents;

    if (actionName == NULL) return;
    if (pre != 0) is_pre = TRUE;
    else is_pre = FALSE;
    
    EditorEvents = TteGetEventsSet("EDITOR");
    if (EditorEvents == NULL) return;

    javaString2CString(actionName, actionname, sizeof(actionname));

    JavaThotlibLock();
    TteAddActionEvent(EditorEvents, (int) typeId, (APPevent) eventType,
                      is_pre, actionname);
    JavaThotlibRelease();
}

void
thotlib_Extra_AddSSchemaActionEvent(struct Hthotlib_Extra* none,
       struct Hjava_lang_String* DTDName,
       struct Hjava_lang_String* actionName, jint eventType,
       jint typeId, jint /* bool */ pre) {
    char actionname[300];
    char dtdname[300];
    boolean is_pre;
    PtrEventsSet EditorEvents;

    if (actionName == NULL) return;
    if (DTDName == NULL) return;
    if (pre != 0) is_pre = TRUE;
    else is_pre = FALSE;
    
    javaString2CString(DTDName, dtdname, sizeof(dtdname));
    EditorEvents = TteGetEventsSet(dtdname);
    if (EditorEvents == NULL) return;

    javaString2CString(actionName, actionname, sizeof(actionname));

    JavaThotlibLock();
    TteAddActionEvent(EditorEvents, (int) typeId, (APPevent) eventType,
                      is_pre, actionname);
    JavaThotlibRelease();
}

void
thotlib_Extra_TtaGetElementType(struct Hthotlib_Extra* none,
		struct Hthotlib_ElementType* elType, jlong el) {
    ElementType et;

    JavaThotlibLock();
    et = TtaGetElementType((Element) JavaLong2CPtr(el));
    JavaThotlibRelease();
    CElementType2JavaElementType(et, elType);
}

struct Hjava_lang_String*
thotlib_Extra_TtaGetElementTypeName(struct Hthotlib_Extra* none,
		jlong sschema, jint type) {
    char *res;
    ElementType et;

    et.ElSSchema = (SSchema) JavaLong2CPtr(sschema);
    et.ElTypeNum = (int) type;
    JavaThotlibLock();
    res = TtaGetElementTypeName(et);
    JavaThotlibRelease();
    if (res == NULL)
        return(NULL);
    else
        return(makeJavaString(res, strlen(res)));
}

jlong
thotlib_Extra_TtaNewAttribute(struct Hthotlib_Extra* none,
		struct Hthotlib_AttributeType* atType) {
    AttributeType att;
    Attribute at;

    att.AttrSSchema = (SSchema) FetchPtrFromJavaVM(&(unhand(atType)->sschema));
    att.AttrTypeNum = FetchIntFromJavaVM(&(unhand(atType)->type));
    JavaThotlibLock();
    at = TtaNewAttribute(att);
    JavaThotlibRelease();
    return(CPtr2JavaLong(at));
}

jlong
thotlib_Extra_TtaGetAttribute(struct Hthotlib_Extra* none,
		jlong element, struct Hthotlib_AttributeType* atType) {
    AttributeType att;
    Attribute at;
    Element el = JavaLong2CPtr(element);

    att.AttrSSchema = (SSchema) FetchPtrFromJavaVM(&(unhand(atType)->sschema));
    att.AttrTypeNum = FetchIntFromJavaVM(&(unhand(atType)->type));
    JavaThotlibLock();
    at = TtaGetAttribute(el, att);
    JavaThotlibRelease();
    return(CPtr2JavaLong(at));
}

void
thotlib_Extra_TtaSearchAttribute(struct Hthotlib_Extra* none,
		struct Hthotlib_AttributeType* atType, jint scope,
		jlong element, struct Hthotlib_Element* el,
		struct Hthotlib_Attribute* at) {
    AttributeType att;
    Attribute lat;
    Element lel;
    Element elem;

    att.AttrSSchema = (SSchema) FetchPtrFromJavaVM(&(unhand(atType)->sschema));
    att.AttrTypeNum = FetchIntFromJavaVM(&(unhand(atType)->type));
    elem = (Element) JavaLong2CPtr(element);
    JavaThotlibLock();
    TtaSearchAttribute(att, (SearchDomain) scope, elem, &lel, &lat);
    JavaThotlibRelease();
    StorePtrToJavaVM(lat, &(unhand(at)->attribute));
    StorePtrToJavaVM(el, &(unhand(el)->element));
}

jlong
thotlib_Extra_TtaSearchTypedElement(struct Hthotlib_Extra* none,
		struct Hthotlib_ElementType* elType, jint scope,
		struct Hthotlib_Element* el) {
    ElementType lelt;
    Element lel;

    lelt.ElSSchema = (SSchema) FetchPtrFromJavaVM(&(unhand(elType)->sschema));
    lelt.ElTypeNum = FetchIntFromJavaVM(&(unhand(elType)->type));
    lel = (Element) FetchPtrFromJavaVM(&(unhand(el)->element));
    JavaThotlibLock();
    lel = TtaSearchTypedElement(lelt, (SearchDomain) scope, lel);
    JavaThotlibRelease();
    return(CPtr2JavaLong(lel));
}

jlong
thotlib_Extra_TtaNewElement(struct Hthotlib_Extra* none,
		jint document, struct Hthotlib_ElementType* elType) {
    ElementType elt;
    Element el;

    elt.ElSSchema = (SSchema) FetchPtrFromJavaVM(&(unhand(elType)->sschema));
    elt.ElTypeNum = FetchIntFromJavaVM(&(unhand(elType)->type));
    JavaThotlibLock();
    el = TtaNewElement((Document) document, elt);
    JavaThotlibRelease();
    return(CPtr2JavaLong(el));
}

jlong
thotlib_Extra_TtaNewTree(struct Hthotlib_Extra* none,
		jint document, struct Hthotlib_ElementType* elType,
		struct Hjava_lang_String* jlabel) {
    ElementType elt;
    Element el;
    char label[1024];
    char *label_ptr = &label[0];

    if (jlabel != NULL)
        javaString2CString(jlabel, label_ptr, sizeof(label));
    else
        label_ptr = NULL;

    elt.ElSSchema = (SSchema) FetchPtrFromJavaVM(&(unhand(elType)->sschema));
    elt.ElTypeNum = FetchIntFromJavaVM(&(unhand(elType)->type));
    JavaThotlibLock();
    el = TtaNewTree((Document) document, elt, label_ptr);
    JavaThotlibRelease();
    return(CPtr2JavaLong(el));
}

jlong
thotlib_Extra_TtaCreateDescent(struct Hthotlib_Extra* none,
		jint document, jlong element,
		struct Hthotlib_ElementType* elType) {
    ElementType elt;
    Element el = (Element) JavaLong2CPtr(element);

    elt.ElSSchema = (SSchema) FetchPtrFromJavaVM(&(unhand(elType)->sschema));
    elt.ElTypeNum = FetchIntFromJavaVM(&(unhand(elType)->type));
    JavaThotlibLock();
    el = TtaCreateDescent((Document) document, el, elt);
    JavaThotlibRelease();
    return(CPtr2JavaLong(el));
}

jlong
thotlib_Extra_TtaCreateDescentWithContent(struct Hthotlib_Extra* none,
		jint document, jlong element,
		struct Hthotlib_ElementType* elType) {
    ElementType elt;
    Element el = (Element) JavaLong2CPtr(element);

    elt.ElSSchema = (SSchema) FetchPtrFromJavaVM(&(unhand(elType)->sschema));
    elt.ElTypeNum = FetchIntFromJavaVM(&(unhand(elType)->type));
    JavaThotlibLock();
    el = TtaCreateDescentWithContent((Document) document, el, elt);
    JavaThotlibRelease();
    return(CPtr2JavaLong(el));
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
	addNativeMethod("thotlib_Extra_Java2CCallback",
	                thotlib_Extra_Java2CCallback);
	addNativeMethod("thotlib_Extra_JavaPollLoop",
	                thotlib_Extra_JavaPollLoop);
	addNativeMethod("thotlib_Extra_JavaStopPoll",
	                thotlib_Extra_JavaStopPoll);
	addNativeMethod("thotlib_Extra_JavaXFlush",
	                thotlib_Extra_JavaXFlush);
	addNativeMethod("thotlib_Extra_JavaStartApplet",
	                thotlib_Extra_JavaStartApplet);
	addNativeMethod("thotlib_Extra_JavaRegisterAction",
	                thotlib_Extra_JavaRegisterAction);
	addNativeMethod("thotlib_Extra_JavaRegisterMenuAction",
	                thotlib_Extra_JavaRegisterMenuAction);
	addNativeMethod("thotlib_Extra_JavaUnregisterAction",
	                thotlib_Extra_JavaUnregisterAction);
	addNativeMethod("thotlib_Extra_JavaUnregisterMenuAction",
	                thotlib_Extra_JavaUnregisterMenuAction);
	addNativeMethod("thotlib_Extra_AddEditorActionEvent",
	                thotlib_Extra_AddEditorActionEvent);
	addNativeMethod("thotlib_Extra_AddSSchemaActionEvent",
	                thotlib_Extra_AddSSchemaActionEvent);
        addNativeMethod("thotlib_Extra_TtaGetElementType",
	                thotlib_Extra_TtaGetElementType);
        addNativeMethod("thotlib_Extra_TtaGetElementTypeName",
	                thotlib_Extra_TtaGetElementTypeName);
        addNativeMethod("thotlib_Extra_TtaNewAttribute",
	                thotlib_Extra_TtaNewAttribute);
        addNativeMethod("thotlib_Extra_TtaSearchTypedElement",
	                thotlib_Extra_TtaSearchTypedElement);
        addNativeMethod("thotlib_Extra_TtaGetAttribute",
	                thotlib_Extra_TtaGetAttribute);
        addNativeMethod("thotlib_Extra_TtaSearchAttribute",
	                thotlib_Extra_TtaSearchAttribute);
        addNativeMethod("thotlib_Extra_TtaNewElement",
	                thotlib_Extra_TtaNewElement);
        addNativeMethod("thotlib_Extra_TtaNewTree",
	                thotlib_Extra_TtaNewTree);
        addNativeMethod("thotlib_Extra_TtaCreateDescent",
	                thotlib_Extra_TtaCreateDescent);
        addNativeMethod("thotlib_Extra_TtaCreateDescentWithContent",
	                thotlib_Extra_TtaCreateDescentWithContent);

        addNativeMethod("amaya_HTTPRequest_Callback",
	                amaya_HTTPRequest_Callback);

/*
	addNativeMethod("thotlib_Extra_Ttaxxx", thotlib_Extra_Ttaxxx);
 */
}

