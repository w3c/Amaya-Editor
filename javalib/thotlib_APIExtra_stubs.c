/*
 * Kaffe stubs generated for class thotlib_Extra.
 * This class encapsulate call to the Thot lib api which could not
 * be built automatically by javastub program.
 */

#include <native.h>
#include "app.h"
#include "appaction.h"
#include "memory.h"
#include "JavaTypes.h"
#include "thotlib_Action.h"
#include "thotlib_Extra.h"

/*
 * The C Callback interface.
 */
void
thotlib_Extra_Java2CCallback(struct Hthotlib_Extra* none,
                             struct Hjava_lang_Object* arg, jlong callback)
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

    if (!strncmp(sig, "(I)", 3))
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
    jword res;

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
		     "callbackAttribute", "(IIJJJ)I", NULL, FALSE,
		     ev->event, ev->document, ev->element,
		     ev->attribute, ev->attributeType);
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
	case TteElemMove: {
	    NotifyElement *ev = (NotifyElement *) event;
	    res = do_execute_java_method(NULL, (Hjava_lang_Object*) arg,
		     "callbackElement", "(IIJJI)I", NULL, FALSE,
		     ev->event, ev->document, ev->element,
		     ev->elementType, ev->position);
	    break;
	}
	case TteElemPaste:
	case TteElemGraphModify: {
	    NotifyOnValue *ev = (NotifyOnValue *) event;
	    res = do_execute_java_method(NULL, (Hjava_lang_Object*) arg,
		     "callbackValue", "(IIJJI)I", NULL, FALSE,
		     ev->event, ev->document, ev->element,
		     ev->target, ev->value);
	    break;
	}
	case TteElemSetReference:
	case TteElemTextModify: {
	    NotifyOnTarget *ev = (NotifyOnTarget *) event;
	    res = do_execute_java_method(NULL, (Hjava_lang_Object*) arg,
		     "callbackTarget", "(IIJJI)I", NULL, FALSE,
		     ev->event, ev->document, ev->element,
		     ev->target, ev->targetdocument);
	    break;
	}
	case TteElemTransform: {
	    NotifyOnElementType *ev = (NotifyOnElementType *) event;
	    res = do_execute_java_method(NULL, (Hjava_lang_Object*) arg,
		     "callbackElementType", "(IIJJI)I", NULL, FALSE,
		     ev->event, ev->document, ev->element,
		     ev->elementType, ev->targetElementType);
	    break;
	}
	case TtePRuleCreate:
	case TtePRuleModify:
	case TtePRuleDelete: {
	    NotifyPresentation *ev = (NotifyPresentation *) event;
	    res = do_execute_java_method(NULL, (Hjava_lang_Object*) arg,
		     "callbackPresentation", "(IIJJI)I", NULL, FALSE,
		     ev->event, ev->document, ev->element,
		     ev->pRule, ev->pRuleType);
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
    TteAddUserAction (TtaStrdup(actionname), thotlib_Extra_JavaActionEventCallback,
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
    TteAddUserMenuAction (TtaStrdup(actionname), thotlib_Extra_JavaActionMenuCallback,
                      (void *) handler);
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
	addNativeMethod("thotlib_Extra_AddEditorActionEvent",
	                thotlib_Extra_AddEditorActionEvent);
	addNativeMethod("thotlib_Extra_AddSSchemaActionEvent",
	                thotlib_Extra_AddSSchemaActionEvent);
/*
	addNativeMethod("thotlib_Extra_Ttaxxx", thotlib_Extra_Ttaxxx);
 */
}

