/*
 * Kaffe stubs generated for class w3c_thotlib_Extra.
 * This class encapsulate call to the Thot lib api which could not
 * be built automatically by javastub program.
 */

#ifdef CONST
#undef CONST
#endif
#define CONST

#include "app.h"
#include "appaction.h"
#include "memory.h"
#include "application.h"
#include "JavaTypes.h"
#include "w3c_thotlib_Action.h"
#include "w3c_thotlib_Extra.h"
#include "w3c_thotlib_ButtonCallback.h"
#include "w3c_amaya_HTTPRequest.h"

#include "JavaTypes_f.h"
#include "JavaVMaccesses.h"

/*
 * Callback for amaya/HTTPRequest
 */
extern void
w3c_amaya_HTTPRequest_Callback(struct Hw3c_amaya_HTTPRequest* req,
                           jlong callback_f, jlong callback_arg);

/*
 * The C Callback interface.
 */
void
w3c_thotlib_Extra_Java2CCallback( /* struct Hw3c_thotlib_Extra* none, */
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
w3c_thotlib_Extra_JavaPollLoop( /* struct Hw3c_thotlib_Extra* none */)
{
    JavaThotlibLock();
    JavaPollLoop();
    JavaThotlibRelease();
}

/*
 * Stop a poll loop.
 */
void
w3c_thotlib_Extra_JavaStopPoll( /* struct Hw3c_thotlib_Extra* none */)
{
    JavaStopPoll();
}

/*
 * Call a Java Applet entry point.
 */
jint
w3c_thotlib_Extra_JavaStartApplet( /* struct Hw3c_thotlib_Extra* none, */
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
w3c_thotlib_Extra_JavaXFlush( /* struct Hw3c_thotlib_Extra* none */)
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
w3c_thotlib_Extra_JavaActionEventCallback(void *arg, NotifyEvent *event)
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
w3c_thotlib_Extra_JavaActionMenuCallback(void *arg, int doc, int view)
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
w3c_thotlib_Extra_JavaRegisterAction( /* struct Hw3c_thotlib_Extra* none, */
                                 struct Hw3c_thotlib_Action* handler,
				 struct Hjava_lang_String* actionName)
{
    char actionname[300];

    if ((handler == NULL) || (actionName == NULL)) return;
    javaString2CString(actionName, actionname, sizeof(actionname));

    JavaThotlibLock();
    TteAddUserAction (TtaStrdup(actionname),
                      w3c_thotlib_Extra_JavaActionEventCallback,
		      (void *) handler);
    JavaThotlibRelease();
}

void
w3c_thotlib_Extra_JavaRegisterMenuAction( /* struct Hw3c_thotlib_Extra* none, */
                                 struct Hw3c_thotlib_Action* handler,
				 struct Hjava_lang_String* actionName)
{
    char actionname[300];

    if ((handler == NULL) || (actionName == NULL)) return;
    javaString2CString(actionName, actionname, sizeof(actionname));

    JavaThotlibLock();
    TteAddUserMenuAction (TtaStrdup(actionname),
                          w3c_thotlib_Extra_JavaActionMenuCallback,
                          (void *) handler);
    JavaThotlibRelease();
}

/*
 * The User Action' deregistration stuff.
 */

void
w3c_thotlib_Extra_JavaUnregisterAction( /* struct Hw3c_thotlib_Extra* none, */
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
w3c_thotlib_Extra_JavaUnregisterMenuAction( /* struct Hw3c_thotlib_Extra* none, */
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
w3c_thotlib_Extra_AddEditorActionEvent( /* struct Hw3c_thotlib_Extra* none, */
       struct Hjava_lang_String* actionName, jint eventType,
       jint typeId, jbool pre) {
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
w3c_thotlib_Extra_AddSSchemaActionEvent( /* struct Hw3c_thotlib_Extra* none, */
       struct Hjava_lang_String* DTDName,
       struct Hjava_lang_String* actionName, jint eventType,
       jint typeId, jbool pre) {
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
w3c_thotlib_Extra_TtaGetElementType( /* struct Hw3c_thotlib_Extra* none, */
		struct Hw3c_thotlib_ElementType* elType, jlong el) {
    ElementType et;

    JavaThotlibLock();
    et = TtaGetElementType((Element) JavaLong2CPtr(el));
    JavaThotlibRelease();
    CElementType2JavaElementType(et, elType);
}

struct Hjava_lang_String*
w3c_thotlib_Extra_TtaGetElementTypeName( /* struct Hw3c_thotlib_Extra* none, */
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
w3c_thotlib_Extra_TtaNewAttribute( /* struct Hw3c_thotlib_Extra* none, */
		struct Hw3c_thotlib_AttributeType* atType) {
    AttributeType att;
    Attribute at;

    att.AttrSSchema = (SSchema) Get_AttributeType_Ptr_sschema(atType);
    att.AttrTypeNum = Get_AttributeType_Int_type(atType);
    JavaThotlibLock();
    at = TtaNewAttribute(att);
    JavaThotlibRelease();
    return(CPtr2JavaLong(at));
}

jlong
w3c_thotlib_Extra_TtaGetAttribute( /* struct Hw3c_thotlib_Extra* none, */
		jlong element, struct Hw3c_thotlib_AttributeType* atType) {
    AttributeType att;
    Attribute at;
    Element el = JavaLong2CPtr(element);

    att.AttrSSchema = (SSchema) Get_AttributeType_Ptr_sschema(atType);
    att.AttrTypeNum = Get_AttributeType_Int_type(atType);
    JavaThotlibLock();
    at = TtaGetAttribute(el, att);
    JavaThotlibRelease();
    return(CPtr2JavaLong(at));
}

void
w3c_thotlib_Extra_TtaSearchAttribute( /* struct Hw3c_thotlib_Extra* none, */
		struct Hw3c_thotlib_AttributeType* atType, jint scope,
		jlong element, struct Hw3c_thotlib_Element* el,
		struct Hw3c_thotlib_Attribute* at) {
    AttributeType att;
    Attribute lat;
    Element lel;
    Element elem;

    att.AttrSSchema = (SSchema) Get_AttributeType_Ptr_sschema(atType);
    att.AttrTypeNum = Get_AttributeType_Int_type(atType);
    elem = (Element) JavaLong2CPtr(element);
    JavaThotlibLock();
    TtaSearchAttribute(att, (SearchDomain) scope, elem, &lel, &lat);
    JavaThotlibRelease();
    Set_Attribute_Ptr_attribute(lat, at);
    Set_Element_Ptr_element(lel, el);
}

jlong
w3c_thotlib_Extra_TtaSearchTypedElement( /* struct Hw3c_thotlib_Extra* none, */
		struct Hw3c_thotlib_ElementType* elType, jint scope,
		struct Hw3c_thotlib_Element* el) {
    ElementType lelt;
    Element lel;

    lelt.ElSSchema = (SSchema) Get_ElementType_Ptr_sschema(elType);
    lelt.ElTypeNum = Get_ElementType_Int_type(elType);
    lel = (Element) Get_Element_Ptr_element(el);
    JavaThotlibLock();
    lel = TtaSearchTypedElement(lelt, (SearchDomain) scope, lel);
    JavaThotlibRelease();
    return(CPtr2JavaLong(lel));
}

jlong
w3c_thotlib_Extra_TtaNewElement( /* struct Hw3c_thotlib_Extra* none, */
		jint document, struct Hw3c_thotlib_ElementType* elType) {
    ElementType elt;
    Element el;

    elt.ElSSchema = (SSchema) Get_ElementType_Ptr_sschema(elType);
    elt.ElTypeNum = Get_ElementType_Int_type(elType);
    JavaThotlibLock();
    el = TtaNewElement((Document) document, elt);
    JavaThotlibRelease();
    return(CPtr2JavaLong(el));
}

jlong
w3c_thotlib_Extra_TtaNewTree( /* struct Hw3c_thotlib_Extra* none, */
		jint document, struct Hw3c_thotlib_ElementType* elType,
		struct Hjava_lang_String* jlabel) {
    ElementType elt;
    Element el;
    char label[1024];
    char *label_ptr = &label[0];

    if (jlabel != NULL)
        javaString2CString(jlabel, label_ptr, sizeof(label));
    else
        label_ptr = NULL;

    elt.ElSSchema = (SSchema) Get_ElementType_Ptr_sschema(elType);
    elt.ElTypeNum = Get_ElementType_Int_type(elType);
    JavaThotlibLock();
    el = TtaNewTree((Document) document, elt, label_ptr);
    JavaThotlibRelease();
    return(CPtr2JavaLong(el));
}

jlong
w3c_thotlib_Extra_TtaCreateDescent( /* struct Hw3c_thotlib_Extra* none, */
		jint document, jlong element,
		struct Hw3c_thotlib_ElementType* elType) {
    ElementType elt;
    Element el = (Element) JavaLong2CPtr(element);

    elt.ElSSchema = (SSchema) Get_ElementType_Ptr_sschema(elType);
    elt.ElTypeNum = Get_ElementType_Int_type(elType);
    JavaThotlibLock();
    el = TtaCreateDescent((Document) document, el, elt);
    JavaThotlibRelease();
    return(CPtr2JavaLong(el));
}

jlong
w3c_thotlib_Extra_TtaCreateDescentWithContent( /* struct Hw3c_thotlib_Extra* none, */
		jint document, jlong element,
		struct Hw3c_thotlib_ElementType* elType) {
    ElementType elt;
    Element el = (Element) JavaLong2CPtr(element);

    elt.ElSSchema = (SSchema) Get_ElementType_Ptr_sschema(elType);
    elt.ElTypeNum = Get_ElementType_Int_type(elType);
    JavaThotlibLock();
    el = TtaCreateDescentWithContent((Document) document, el, elt);
    JavaThotlibRelease();
    return(CPtr2JavaLong(el));
}

/****************************************************************
 *								*
 * 	Handling of support for new Button from Java		*
 *								*
 ****************************************************************/
#define MAX_JAVA_BUTTONS 5
#define MAX_BUTTONS	 30

typedef void (*ButtonCCallback)(Document document, View view);

static int JavaButtonsInitialized = 0;
static int nbJavaButtons = 0;
static struct Hw3c_thotlib_ButtonCallback 
                       *ButtonJavaCallbacks[MAX_JAVA_BUTTONS];
static ButtonCCallback *ButtonCCallbacks[MAX_JAVA_BUTTONS];
static char            *ButtonImageName[MAX_JAVA_BUTTONS];

/*
 * This register a Java ButtonCallback object.
 */

int registerJavaButtonCallback(Hw3c_thotlib_ButtonCallback *callback) {
    int i;

    /* first find whether the Java callback is registered */
    for (i=0;i < nbJavaButtons;i++)
        if (ButtonJavaCallbacks[i] == callback) return(i);

    /* check for overflow */
    if (nbJavaButtons >= MAX_JAVA_BUTTONS) {
        fprintf(stderr, "MAX_JAVA_BUTTONS %d overflow\n", MAX_JAVA_BUTTONS);
	return(-1);
    }

    /*
     * Reference the Java object from the C space and ensure
     * persistancy of the Java object .
     */
    i = nbJavaButtons++;
    ButtonJavaCallbacks[i] = callback;
    do_execute_java_class_method("w3c.thotlib.Application",
	    "makePersistant", "(Ljava/lang/Object;)V", callback);
    return(i);
}

/*
 * This check for the existence of a button 
 */

int hasJavaButton(Document document, int callback) {
    int i;

    /* Now, check whether this document has a button with this callback */
    for (i = 0;i < MAX_BUTTONS;i++)
        if (TtaGetButtonCallback(document, 1, i) == ButtonCCallbacks[callback])
	    return(i);
    return(-1);
}

#define NewButtonCCallback(no)					\
static void ButtonCCallback##no( Document document, View view) {\
    ButtonCCallbackDispatcher(no, document, view);}

/*
 * This dispatcher is called each time a Java button is pressed.
 */
void ButtonCCallbackDispatcher(int no, Document document, View view) {
    if ((no < 0) || (no >= MAX_JAVA_BUTTONS)) return;
    if (ButtonJavaCallbacks[no] == NULL) return;

    do_execute_java_method(0, ButtonJavaCallbacks[no],
                           "callback", "(II)V", 0, 0,
			   document, view);
}

NewButtonCCallback(0)
NewButtonCCallback(1)
NewButtonCCallback(2)
NewButtonCCallback(3)
NewButtonCCallback(4)

/*
 * initialize the button handler arrays.
 */
void initializeJavaButtons(void) {
    int i;

    if (JavaButtonsInitialized) return;
    for (i = 0;i < MAX_JAVA_BUTTONS;i++) {
        ButtonJavaCallbacks[i] = 0;
        ButtonImageName[i] = NULL;
    }
    ButtonCCallbacks[0] = ButtonCCallback0;
    ButtonCCallbacks[1] = ButtonCCallback1;
    ButtonCCallbacks[2] = ButtonCCallback2;
    ButtonCCallbacks[3] = ButtonCCallback3;
    ButtonCCallbacks[4] = ButtonCCallback4;
    JavaButtonsInitialized = 1;
}
 
/*
 * This method install a new button asked by Java code.
 */
jint
w3c_thotlib_Extra_TtaNewButton(jint document,
                 struct Hw3c_thotlib_ButtonCallback *jcallback,
		 struct Hjava_lang_String* jimgname,
		 struct Hjava_lang_String* jinfos)
{
    char infos[300] = "";
    char imgname[300];
    Pixmap pixmap;
    int callback;
    int button;

    if (!JavaButtonsInitialized) initializeJavaButtons();

    callback = registerJavaButtonCallback(jcallback);
    if (callback < 0) return(-1);

    /* is the document ok ? */
    if ((document <= 0) || (document > 10)) document = 1;

    /* has this document already this new button */
    button = hasJavaButton(document, callback);
    if (button >= 0) return(button);

    /* convert the string(s) to C format */
    if (jinfos != NULL)
	javaString2CString(jinfos, infos, sizeof(infos));
    javaString2CString(jimgname, imgname, sizeof(imgname));

    /* Search the pixmap */
    pixmap = TtaGetImage(imgname);
    if (pixmap == NULL) {
        fprintf(stderr, "TtaNewButton : unregistered image name %s\n",
	        imgname);
	return(-1);
    }

    /* Register the callback and add the button */
    if (infos[0] == '\0')
	button = TtaAddButton (document, 1, pixmap,
	                       ButtonCCallbacks[callback], NULL);
    else	
	button = TtaAddButton (document, 1, pixmap,
			       ButtonCCallbacks[callback], infos);

    return(button);
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
void register_w3c_thotlib_Extra_stubs(void)
{
	addNativeMethod("w3c_thotlib_Extra_Java2CCallback",
	                w3c_thotlib_Extra_Java2CCallback);
	addNativeMethod("w3c_thotlib_Extra_JavaPollLoop",
	                w3c_thotlib_Extra_JavaPollLoop);
	addNativeMethod("w3c_thotlib_Extra_JavaStopPoll",
	                w3c_thotlib_Extra_JavaStopPoll);
	addNativeMethod("w3c_thotlib_Extra_JavaXFlush",
	                w3c_thotlib_Extra_JavaXFlush);
	addNativeMethod("w3c_thotlib_Extra_JavaStartApplet",
	                w3c_thotlib_Extra_JavaStartApplet);
	addNativeMethod("w3c_thotlib_Extra_JavaRegisterAction",
	                w3c_thotlib_Extra_JavaRegisterAction);
	addNativeMethod("w3c_thotlib_Extra_JavaRegisterMenuAction",
	                w3c_thotlib_Extra_JavaRegisterMenuAction);
	addNativeMethod("w3c_thotlib_Extra_JavaUnregisterAction",
	                w3c_thotlib_Extra_JavaUnregisterAction);
	addNativeMethod("w3c_thotlib_Extra_JavaUnregisterMenuAction",
	                w3c_thotlib_Extra_JavaUnregisterMenuAction);
	addNativeMethod("w3c_thotlib_Extra_AddEditorActionEvent",
	                w3c_thotlib_Extra_AddEditorActionEvent);
	addNativeMethod("w3c_thotlib_Extra_AddSSchemaActionEvent",
	                w3c_thotlib_Extra_AddSSchemaActionEvent);
        addNativeMethod("w3c_thotlib_Extra_TtaGetElementType",
	                w3c_thotlib_Extra_TtaGetElementType);
        addNativeMethod("w3c_thotlib_Extra_TtaGetElementTypeName",
	                w3c_thotlib_Extra_TtaGetElementTypeName);
        addNativeMethod("w3c_thotlib_Extra_TtaNewAttribute",
	                w3c_thotlib_Extra_TtaNewAttribute);
        addNativeMethod("w3c_thotlib_Extra_TtaSearchTypedElement",
	                w3c_thotlib_Extra_TtaSearchTypedElement);
        addNativeMethod("w3c_thotlib_Extra_TtaGetAttribute",
	                w3c_thotlib_Extra_TtaGetAttribute);
        addNativeMethod("w3c_thotlib_Extra_TtaSearchAttribute",
	                w3c_thotlib_Extra_TtaSearchAttribute);
        addNativeMethod("w3c_thotlib_Extra_TtaNewElement",
	                w3c_thotlib_Extra_TtaNewElement);
        addNativeMethod("w3c_thotlib_Extra_TtaNewTree",
	                w3c_thotlib_Extra_TtaNewTree);
        addNativeMethod("w3c_thotlib_Extra_TtaCreateDescent",
	                w3c_thotlib_Extra_TtaCreateDescent);
        addNativeMethod("w3c_thotlib_Extra_TtaCreateDescentWithContent",
	                w3c_thotlib_Extra_TtaCreateDescentWithContent);
        addNativeMethod("w3c_thotlib_Extra_TtaNewButton",
	                w3c_thotlib_Extra_TtaNewButton);

        addNativeMethod("w3c_amaya_HTTPRequest_Callback",
	                w3c_amaya_HTTPRequest_Callback);

/*
	addNativeMethod("w3c_thotlib_Extra_Ttaxxx", w3c_thotlib_Extra_Ttaxxx);
 */
}

