/*
 * JavaVMaccesses.h : header file providing encapsulation
 *                    for java internal object state
 */

#ifndef __JAVA_VM_ACCESSES__
#define __JAVA_VM_ACCESSES__
#include "JavaTypes.h"
#include "JavaTypes_f.h"

/* #define DIRECT_JAVA_VM_ACCESS */

#ifdef DIRECT_JAVA_VM_ACCESS

/*
 * When using direct access, the C code read and write directly into
 * the Kaffe VM to get and set the Java objects value.
 */
#define Set_ElementType_Ptr_sschema(value, obj) \
		StorePtrToJavaVM((value), &(unhand(obj)->sschema))
#define Get_ElementType_Ptr_sschema(obj) \
		FetchPtrFromJavaVM(&(unhand(obj)->sschema))

#define Set_ElementType_Int_type(value, obj) \
		StoreIntToJavaVM((value), &(unhand(obj)->type))
#define Get_ElementType_Int_type(obj) \
		FetchIntFromJavaVM(&(unhand(obj)->type))

#define Set_AttributeType_Ptr_sschema(value, obj) \
		StorePtrToJavaVM((value), &(unhand(obj)->sschema))
#define Get_AttributeType_Ptr_sschema(obj) \
		FetchPtrFromJavaVM(&(unhand(obj)->sschema))

#define Set_AttributeType_Int_type(value, obj) \
		StoreIntToJavaVM((value), &(unhand(obj)->type))
#define Get_AttributeType_Int_type(obj) \
		FetchIntFromJavaVM(&(unhand(obj)->type))

#define Set_Element_Ptr_element(value, obj) \
		StorePtrToJavaVM((value), &(unhand(obj)->element))
#define Get_Element_Ptr_element(obj) \
		FetchPtrFromJavaVM(&(unhand(obj)->element))

#define Set_PRule_Ptr_prule(value, obj) \
		StorePtrToJavaVM((value), &(unhand(obj)->prule))
#define Get_PRule_Ptr_prule(obj) \
		FetchPtrFromJavaVM(&(unhand(obj)->prule))

#define Set_SSchema_Ptr_sschema(value, obj) \
		StorePtrToJavaVM((value), &(unhand(obj)->sschema))
#define Get_SSchema_Ptr_sschema(obj) \
		FetchPtrFromJavaVM(&(unhand(obj)->sschema))

#define Set_Attribute_Ptr_attribute(value, obj) \
		StorePtrToJavaVM((value), &(unhand(obj)->attribute))
#define Get_Attribute_Ptr_attribute(obj) \
		FetchPtrFromJavaVM(&(unhand(obj)->attribute))

#define Set_IntPtr_Int_value(val, obj) \
		StoreIntToJavaVM((val), &(unhand(obj)->value))
#define Get_IntPtr_Int_value(obj) \
		FetchIntFromJavaVM(&(unhand(obj)->value))

#define Set_Document_Int_document(value, obj) \
		StoreIntToJavaVM((value), &(unhand(obj)->document))
#define Get_Document_Int_document(obj) \
		FetchIntFromJavaVM(&(unhand(obj)->document))

#define Set_Language_Char_lang(value, obj) \
		StoreCharToJavaVM((value), &(unhand(obj)->lang))
#define Get_Language_Char_lang(obj) \
		FetchCharFromJavaVM(&(unhand(obj)->lang))

#define Set_HTTPRequest_Ptr_callback_f(value, obj) \
		StorePtrToJavaVM((value), &(unhand(obj)->callback_f))
#define Get_HTTPRequest_Ptr_callback_f(obj) \
		FetchPtrFromJavaVM(&(unhand(obj)->callback_f))

#define Set_HTTPRequest_Ptr_callback_arg(value, obj) \
		StorePtrToJavaVM((value), &(unhand(obj)->callback_arg))
#define Get_HTTPRequest_Ptr_callback_arg(obj) \
		FetchPtrFromJavaVM(&(unhand(obj)->callback_arg))

#define Set_HTTPRequest_Int_doc(value, obj) \
		StoreIntToJavaVM((value), &(unhand(obj)->doc))
#define Get_HTTPRequest_Int_doc(obj) \
		FetchIntFromJavaVM(&(unhand(obj)->doc))

#define Set_HTTPRequest_Int_status(value, obj) \
		StoreIntToJavaVM((value), &(unhand(obj)->status))
#define Get_HTTPRequest_Int_status(obj) \
		FetchIntFromJavaVM(&(unhand(obj)->status))

#define Set_HTTPRequest_Str_urlName(value, obj) \
		StoreStrToJavaVM((value), &(unhand(obj)->urlName))
#define Get_HTTPRequest_Str_urlName(obj) \
		FetchStrFromJavaVM(&(unhand(obj)->urlName))

#define Set_HTTPRequest_Str_filename(value, obj) \
		StoreStrToJavaVM((value), &(unhand(obj)->filename))
#define Get_HTTPRequest_Str_filename(obj) \
		FetchStrFromJavaVM(&(unhand(obj)->filename))

#define Set_HTTPRequest_Str_postCmd(value, obj) \
		StoreStrToJavaVM((value), &(unhand(obj)->postCmd))
#define Get_HTTPRequest_Str_postCmd(obj) \
		FetchStrFromJavaVM(&(unhand(obj)->postCmd))

#define Set_HTTPRequest_Str_mimeType(value, obj) \
		StoreStrToJavaVM((value), &(unhand(obj)->mimeType))
#define Get_HTTPRequest_Str_mimeType(obj) \
		FetchStrFromJavaVM(&(unhand(obj)->mimeType))

#else /* ! DIRECT_JAVA_VM_ACCESS */

/*
 * The indirect access method makes a Java object method invocation
 * in order to get/set the content of Java object. This method is
 * quite slowler than direct access but far more portable.
 */

#define SET_INT_SIG "(I)V"
#define GET_INT_SIG "()I"

#if SIZEOF_VOIDP == 4
#define SET_PTR_SIG "(I)V"
#define GET_PTR_SIG "(I)I"
#define L2PTR_CVRT(x) x
#define PTR2L_CVRT(x) x
#elif SIZEOF_VOIDP == 8
#define SET_PTR_SIG "(J)V"
#define GET_PTR_SIG "(J)J"
#define L2PTR_CVRT(x) JavaLong2CPtr(x)
#define PTR2L_CVRT(x) CPtr2JavaLong(x)
#else
#endif

#define Set_ElementType_Ptr_sschema(value, obj) \
		do_execute_java_method((Hjava_lang_Object*) (obj), \
		"set_sschema", SET_PTR_SIG, 0, 0, PTR2L_CVRT(value))
#define Get_ElementType_Ptr_sschema(obj) \
		L2PTR_CVRT(do_execute_java_method((Hjava_lang_Object*) (obj), \
		"get_sschema", GET_PTR_SIG, 0, 0).j)

#define Set_ElementType_Int_type(value, obj) \
		do_execute_java_method((Hjava_lang_Object*) (obj), \
		"set_type", SET_INT_SIG, 0, 0, (jint) value)
#define Get_ElementType_Int_type(obj) \
		do_execute_java_method((Hjava_lang_Object*) (obj), \
		"get_type", GET_INT_SIG, 0, 0).i

#define Set_AttributeType_Ptr_sschema(value, obj) \
		do_execute_java_method((Hjava_lang_Object*) (obj), \
		"set_sschema", SET_PTR_SIG, 0, 0, PTR2L_CVRT(value))
#define Get_AttributeType_Ptr_sschema(obj) \
		L2PTR_CVRT(do_execute_java_method((Hjava_lang_Object*) (obj), \
		"get_sschema", GET_PTR_SIG, 0, 0).j)

#define Set_AttributeType_Int_type(value, obj) \
		do_execute_java_method((Hjava_lang_Object*) (obj), \
		"set_type", SET_INT_SIG, 0, 0, (jint) value)
#define Get_AttributeType_Int_type(obj) \
		do_execute_java_method((Hjava_lang_Object*) (obj), \
		"get_type", GET_INT_SIG, 0, 0).i

#define Set_Element_Ptr_element(value, obj) \
		do_execute_java_method((Hjava_lang_Object*) (obj), \
		"set_element", SET_PTR_SIG, 0, 0, PTR2L_CVRT(value))
#define Get_Element_Ptr_element(obj) \
		L2PTR_CVRT(do_execute_java_method((Hjava_lang_Object*) (obj), \
		"get_element", GET_PTR_SIG, 0, 0).j)

#define Set_PRule_Ptr_prule(value, obj) \
		do_execute_java_method((Hjava_lang_Object*) (obj), \
		"set_prule", SET_PTR_SIG, 0, 0, PTR2L_CVRT(value))
#define Get_PRule_Ptr_prule(obj) \
		L2PTR_CVRT(do_execute_java_method((Hjava_lang_Object*) (obj), \
		"get_prule", GET_PTR_SIG, 0, 0).j)

#define Set_SSchema_Ptr_sschema(value, obj) \
		do_execute_java_method((Hjava_lang_Object*) (obj), \
		"set_sschema", SET_PTR_SIG, 0, 0, PTR2L_CVRT(value))
#define Get_SSchema_Ptr_sschema(obj) \
		L2PTR_CVRT(do_execute_java_method((Hjava_lang_Object*) (obj), \
		"get_sschema", GET_PTR_SIG, 0, 0).j)

#define Set_Attribute_Ptr_attribute(value, obj) \
		do_execute_java_method((Hjava_lang_Object*) (obj), \
		"set_attribute", SET_PTR_SIG, 0, 0, PTR2L_CVRT(value))
#define Get_Attribute_Ptr_attribute(obj) \
		L2PTR_CVRT(do_execute_java_method((Hjava_lang_Object*) (obj), \
		"get_attribute", GET_PTR_SIG, 0, 0).j)

#define Set_IntPtr_Int_value(value, obj) \
		do_execute_java_method((Hjava_lang_Object*) (obj), \
		"set_value", SET_INT_SIG, 0, 0, (jint) value)
#define Get_IntPtr_Int_value(obj) \
		do_execute_java_method((Hjava_lang_Object*) (obj), \
		"get_value", GET_INT_SIG, 0, 0).i

#define Set_Document_Int_document(value, obj) \
		do_execute_java_method((Hjava_lang_Object*) (obj), \
		"set_document", SET_INT_SIG, 0, 0, (jint) value)
#define Get_Document_Int_document(obj) \
		do_execute_java_method((Hjava_lang_Object*) (obj), \
		"get_document", GET_INT_SIG, 0, 0).i

#define Set_Language_Char_lang(value, obj) \
		do_execute_java_method((Hjava_lang_Object*) (obj), \
		"set_lang", SET_INT_SIG, 0, 0, (jint) value)
#define Get_Language_Char_lang(obj) \
		do_execute_java_method((Hjava_lang_Object*) (obj), \
		"get_lang", GET_INT_SIG, 0, 0).c

#define Set_HTTPRequest_Ptr_callback_f(value, obj) \
		do_execute_java_method((Hjava_lang_Object*) (obj), \
		"set_callback_f", SET_PTR_SIG, 0, 0, PTR2L_CVRT(value))
#define Get_HTTPRequest_Ptr_callback_f(obj) \
		L2PTR_CVRT(do_execute_java_method((Hjava_lang_Object*) (obj), \
		"get_callback_f", GET_PTR_SIG, 0, 0).j)

#define Set_HTTPRequest_Ptr_callback_arg(value, obj) \
		do_execute_java_method((Hjava_lang_Object*) (obj), \
		"set_callback_arg", SET_PTR_SIG, 0, 0, PTR2L_CVRT(value))
#define Get_HTTPRequest_Ptr_callback_arg(obj) \
		L2PTR_CVRT(do_execute_java_method((Hjava_lang_Object*) (obj), \
		"get_callback_arg", GET_PTR_SIG, 0, 0).j)

#define Set_HTTPRequest_Int_doc(value, obj) \
		do_execute_java_method((Hjava_lang_Object*) (obj), \
		"set_doc", SET_INT_SIG, 0, 0, (jint) value)
#define Get_HTTPRequest_Int_doc(obj) \
		do_execute_java_method((Hjava_lang_Object*) (obj), \
		"get_doc", GET_INT_SIG, 0, 0).i

#define Set_HTTPRequest_Int_status(value, obj) \
		do_execute_java_method((Hjava_lang_Object*) (obj), \
		"set_status", SET_INT_SIG, 0, 0, (jint) value)
#define Get_HTTPRequest_Int_status(obj) \
		do_execute_java_method((Hjava_lang_Object*) (obj), \
		"get_status", GET_INT_SIG, 0, 0).i

#define Set_HTTPRequest_Str_filename(value, obj) \
		do_execute_java_method((Hjava_lang_Object*) (obj), \
		"set_filename", "(Ljava/lang/String;)V", 0, 0, \
		(struct Hjava_lang_String*) value)
#define Get_HTTPRequest_Str_filename(obj) \
		do_execute_java_method((Hjava_lang_Object*) (obj), \
		"get_filename", "()Ljava/lang/String;", 0, 0).l

#define Set_HTTPRequest_Str_urlName(value, obj) \
		do_execute_java_method((Hjava_lang_Object*) (obj), \
		"set_urlName", "(Ljava/lang/String;)V", 0, 0, \
		(struct Hjava_lang_String*) value)
#define Get_HTTPRequest_Str_urlName(obj) \
		do_execute_java_method((Hjava_lang_Object*) (obj), \
		"get_urlName", "()Ljava/lang/String;", 0, 0).l

#define Set_HTTPRequest_Str_mimeType(value, obj) \
		do_execute_java_method((Hjava_lang_Object*) (obj), \
		"set_mimeType", "(Ljava/lang/String;)V", 0, 0, \
		(struct Hjava_lang_String*) value)
#define Get_HTTPRequest_Str_mimeType(obj) \
		do_execute_java_method((Hjava_lang_Object*) (obj), \
		"get_mimeType", "()Ljava/lang/String;", 0, 0).l

#define Set_HTTPRequest_Str_postCmd(value, obj) \
		do_execute_java_method((Hjava_lang_Object*) (obj), \
		"set_postCmd", "(Ljava/lang/String;)V", 0, 0, \
		(struct Hjava_lang_String*) value)
#define Get_HTTPRequest_Str_postCmd(obj) \
		do_execute_java_method((Hjava_lang_Object*) (obj), \
		"get_postCmd", "()Ljava/lang/String;", 0, 0).l

#endif /* DIRECT_JAVA_VM_ACCESS */
#endif /* __JAVA_VM_ACCESSES__ */
