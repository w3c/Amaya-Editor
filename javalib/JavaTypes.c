/*
 * Implementation for the conversion funtions between internal
 * types and external ones.
 *
 * Daniel Veillard 1997
 */

#include "JavaTypes.h"
#include "JavaTypes_f.h"
#include "JavaVMaccesses.h"

#define DEBUG_ARCH

#ifdef DIRECT_JAVA_VM_ACCESS
/*
 * Test how a pointer is stored internally in the Kaffe V.M. inside
 * a jlong.
 * On some architectures, the way jlong are stored is different from
 * the native one as implemented by the compiler.
 */

static int jlong_storage_swapped = -1;
typedef union tst_jlong { struct ints { jint low; jint high; } ints;
                          jlong val; } tst_jlong;

static void check_jlong_storage(void)
{
    struct Horg_w3c_thotlib_Element *object;
    jlong  test_value = CPtr2JavaLong((int *) 0xdeadbeef);
    jlong  fetch_value;

    /*
     * Initialize a Java Element object with test_value.
     */
    object = (struct Horg_w3c_thotlib_Element *) execute_java_constructor(0,
                "org.w3c.thotlib.Element", 0, "(J)V", test_value);
    fetch_value = unhand(object)->element;

    /*
     * Check how the Java long is stored in the Java V.M.
     */
    if (fetch_value == test_value) {
        jlong_storage_swapped = 0;
#ifdef DEBUG_ARCH
	fprintf(stderr,"jlong_storage_swapped : no\n");
#endif
    } else {
        union tst_jlong tst_orig;
        union tst_jlong tst_val;

	tst_orig.val = test_value;
	tst_val.val = fetch_value;

	if ((tst_orig.ints.low == tst_val.ints.high) &&
	    (tst_orig.ints.high == tst_val.ints.low)) {
#ifdef DEBUG_ARCH
	    fprintf(stderr,"jlong_storage_swapped : yes\n");
#endif
	    jlong_storage_swapped = 1;
	} else {
	    fprintf(stderr,"jlong_storage_swapped : couldn't guess storage\n");
	    exit(1);
	}
    }
}

jlong FetchJLongFromJavaVM(jlong *address)
{
    jlong value = *address;

    if (jlong_storage_swapped) {
        /* Swap the upper and lower 32 bits parts of the java long */
        union tst_jlong val;
	jint tmp;

	val.val = value;
        tmp = val.ints.high;
	val.ints.high = val.ints.low;
	val.ints.low = tmp;
	value = val.val;
#ifdef DEBUG_ARCH
	fprintf(stderr,"FetchJLongFromJavaVM : 0x%08X%08X swapped\n",
	        val.ints.high, val.ints.low);
#endif
    }
    return(value);
}

void StoreJLongInJavaVM(jlong value, jlong *address)
{
    if (jlong_storage_swapped) {
        /* Swap the upper and lower 32 bits parts of the java long */
        union tst_jlong val;
	jint tmp;

	val.val = value;
        tmp = val.ints.high;
	val.ints.high = val.ints.low;
	val.ints.low = tmp;
	value = val.val;
#ifdef DEBUG_ARCH
	fprintf(stderr,"StoreJLongInJavaVM : 0x%08X%08X swapped\n",
	        val.ints.high, val.ints.low);
#endif
    }
    *address = value;
}

void *FetchPtrFromJavaVM(jlong *address)
{
    jlong value = *address;

    if (jlong_storage_swapped) {
        /* Swap the upper and lower 32 bits parts of the java long */
        union tst_jlong val;
	jint tmp;

	val.val = value;
        tmp = val.ints.high;
	val.ints.high = val.ints.low;
	val.ints.low = tmp;
	value = val.val;
#ifdef DEBUG_ARCH
	fprintf(stderr,"FetchPtrFromJavaVM : 0x%08X%08X swapped\n",
	        val.ints.high, val.ints.low);
#endif
    }
    return(JavaLong2CPtr(value));
}

void StorePtrToJavaVM(void *ptr, jlong *address)
{
    jlong value = CPtr2JavaLong(ptr);

    if (jlong_storage_swapped) {
        /* Swap the upper and lower 32 bits parts of the java long */
	union tst_jlong val;
	jint tmp;

	val.val = value;
        tmp = val.ints.high;
	val.ints.high = val.ints.low;
	val.ints.low = tmp;
	value = val.val;
#ifdef DEBUG_ARCH
	fprintf(stderr,"StorePtrToJavaVM : 0x%08X%08X swapped\n",
	        val.ints.high, val.ints.low);
#endif
    }
    *address = value;
}

jint FetchIntFromJavaVM(jint *address)
{
    jint value = *address;

    return(value);
}

void StoreIntToJavaVM(jint value, jint *address)
{
    *address = value;
}

char FetchCharFromJavaVM(jint *address)
{
    char value = (char) *address;

    return(value);
}

void StoreCharToJavaVM(char value, jint *address)
{
    *address = (jint) value;
}

struct Hjava_lang_String* FetchStrFromJavaVM(struct Hjava_lang_String** address)
{
    struct Hjava_lang_String* value = (struct Hjava_lang_String*) *address;

    return(value);
}

void StoreStrToJavaVM(struct Hjava_lang_String* value, struct Hjava_lang_String** address)
{
    *address = value;
}

/*
 * Problem when storing pointers in Java long and passing them
 * by address :
 *
 * a Java long is a 64 bit entity by definition,
 * a C pointer may be 64 or 32 bits. In the lastest case,
 * depending on whether the architecture is LSBF or MSBF,
 * casting a jlong pointer directly to an int pointer may
 * or may not be a good idea, let's assume the value is
 * 0xAABBCCDD :
 *
 *    pointer to the Java jlong
 *       |    --------------> increasing mem addresses
 *       V
 * LSBF [AA,BB,CC,DD,00,00,00,00]
 *       ^
 *       |
 *       pointer to the 32 bits value
 *
 * MSBF [00,00,00,00,DD,CC,BB,AA]
 *                   ^
 *                   |
 *                   pointer to the 32 bits value
 */
static int int_ptr_need_shift = -1;

static void do_ptr_need_shift(void) {
    jlong value = 0xAABBCCDD;
    jlong *jptr = &value;
    int   *iptr;

    iptr = (int   *) jptr;
    if (*iptr == 0xAABBCCDD) {
#ifdef DEBUG_ARCH
	fprintf(stderr,"do_ptr_need_shift() : no\n");
#endif
        int_ptr_need_shift = 0;
	return;
    }
    iptr++;
    if (*iptr == 0xAABBCCDD) {
#ifdef DEBUG_ARCH
	fprintf(stderr,"do_ptr_need_shift() : yes\n");
#endif
        int_ptr_need_shift = 1;
	return;
    }
    fprintf(stderr,"do_ptr_need_shift() : memory storage error !\n");
    exit(1);
}

void *JavaLongPtr2CIntPtr(jlong *in)
{
    int *res = (int *) in;

    if (int_ptr_need_shift == 1) {
        res++;
#ifdef DEBUG_ARCH
	fprintf(stderr, "JavaLongPtr2CIntPtr(0x%X%08X) = 0x%X%08X\n",
            (unsigned int) (((jlong) in) >> 32),
	    (unsigned int) (((jlong) in) & 0xFFFFFFFFL),
            (unsigned int) (((jlong) res) >> 32),
	    (unsigned int) (((jlong) res) & 0xFFFFFFFFL));
#endif
    }
    return (res);
}

/*
 * Another problem when creating Java internal (Unicode) strings:
 *
 * On an LSBF machine, the unicode string for an ASCII string is
 *     0, S, 0, t, 0, r, 0, i, 0, n, 0, g, 0, 0
 * on an MSBF this is :
 *     S, 0, t, 0, r, 0, i, 0, n, 0, g, 0, 0, 0
 * We need to check it since we are building strings directly in
 * Kaffe memory.
 */

static int lsbf_architecture = -1;
typedef union tst_lsbf { char str[2]; unsigned short int2; } tst_lsbf;

static void is_lsbf_architecture(void) {
    static union tst_lsbf test;

    test.int2 = 32;
    if (test.str[0] == ' ') {
#ifdef DEBUG_ARCH
	fprintf(stderr,"is_lsbf_architecture() : yes\n");
#endif
        lsbf_architecture = 1;
	return;
    } else if (test.str[1] == ' ') {
#ifdef DEBUG_ARCH
	fprintf(stderr,"is_lsbf_architecture() : no\n");
#endif
        lsbf_architecture = 0;
	return;
    }
    fprintf(stderr,"is_lsbf_architecture() : memory storage error !\n");
    exit(1);
}
#endif /* DIRECT_JAVA_VM_ACCESS */

void initJavaTypes() {
#ifdef DIRECT_JAVA_VM_ACCESS
    is_lsbf_architecture();
    do_ptr_need_shift();
    check_jlong_storage();
#endif /* DIRECT_JAVA_VM_ACCESS */
}

/*
 * General conversion routines between java types and C types.
 */

void *JavaLong2CPtr(jlong in)
{
    /*
     * a Java long is a 64 bit entity by definition,
     * a C pointer may be 32 or 64 bits.
     */
#if SIZEOF_INT == SIZEOF_VOIDP
    unsigned int tmp = (unsigned int) in;
#elif SIZEOF_LONG == SIZEOF_VOIDP
    unsigned long tmp = (unsigned long) in;
#elif SIZEOF_LONG_LONG == SIZEOF_VOIDP
    unsigned long long tmp = (unsigned long long) in;
#elif SIZEOF___INT64 == SIZEOF_VOIDP
    unsigned int64 tmp = (unsigned int64) in;
#else
#error "couldn't find an appropriate type to hold pointers"
#endif
    return ((void *) tmp);
}

jlong CPtr2JavaLong(void *in)
{
    /*
     * a Java long is a 64 bit entity by definition,
     * a C pointer may be 32 or 64 bits.
     */
#if SIZEOF_INT == SIZEOF_VOIDP
    unsigned int tmp = (unsigned int) in;
#elif SIZEOF_LONG == SIZEOF_VOIDP
    unsigned long tmp = (unsigned long) in;
#elif SIZEOF_LONG_LONG == SIZEOF_VOIDP
    unsigned long long tmp = (unsigned long long) in;
#elif SIZEOF___INT64 == SIZEOF_VOIDP
    unsigned int64 tmp = (unsigned int64) in;
#else
#error "couldn't find an appropriate type to hold pointers"
#endif
    return ((jlong) tmp);
}

/*
 * C object <=> Java object content
 */
void JavaElement2CElement(struct Horg_w3c_thotlib_Element *in, Element *out)
{
    *out = (Element) Get_Element_Ptr_element(in);
}

/*
 * C object pointer <=> pointers to Java object content
 */
void JavaElement2CElementPtr(struct Horg_w3c_thotlib_Element* in, Element **out)
{
    *out = (Element *) malloc(sizeof(Element));
    *(*out) = (Element) Get_Element_Ptr_element(in);
}
void CElementPtr2JavaElement(Element *in, struct Horg_w3c_thotlib_Element** out)
{
    Set_Element_Ptr_element(*(in), *out);
    free(in);
}

void JavaElementType2CElementTypePtr(struct Horg_w3c_thotlib_ElementType* in, ElementType **out)
{
    *out = (ElementType *) malloc(sizeof(ElementType));
    (*out)->ElSSchema = (SSchema) Get_ElementType_Ptr_sschema(in);
    (*out)->ElTypeNum = Get_ElementType_Int_type(in);
}
void CElementTypePtr2JavaElementType(ElementType *in, struct Horg_w3c_thotlib_ElementType** out)
{
    Set_ElementType_Ptr_sschema(in->ElSSchema, *out);
    Set_ElementType_Int_type(in->ElTypeNum, *out);
    free(in);
}
void CElementType2JavaElementType(ElementType in, struct Horg_w3c_thotlib_ElementType* out)
{
    Set_ElementType_Ptr_sschema(in.ElSSchema, out);
    Set_ElementType_Int_type(in.ElTypeNum, out);
}


void JavaDocument2CDocumentPtr(struct Horg_w3c_thotlib_Document* in, Document **out)
{
    *out = (Document *) malloc(sizeof(Document));
    *(*out) = (Document) Get_Document_Int_document(in);
}
void CDocumentPtr2JavaDocument(Document *in, struct Horg_w3c_thotlib_Document** out)
{
    Set_Document_Int_document(*(in), *out);
    free(in);
}

void JavaSSchema2CSSchemaPtr(struct Horg_w3c_thotlib_SSchema* in, SSchema **out)
{
    *out = (SSchema *) malloc(sizeof(SSchema));
    *(*out) = (SSchema) Get_SSchema_Ptr_sschema(in);
}
void CSSchemaPtr2JavaSSchema(SSchema *in, struct Horg_w3c_thotlib_SSchema** out)
{
    Set_SSchema_Ptr_sschema(*(in), *out);
    free(in);
}

void JavaAttribute2CAttributePtr(struct Horg_w3c_thotlib_Attribute* in, Attribute **out)
{
    *out = (Attribute *) malloc(sizeof(Attribute));
    *(*out) = (Attribute) Get_Attribute_Ptr_attribute(in);
}
void CAttributePtr2JavaAttribute(Attribute *in, struct Horg_w3c_thotlib_Attribute** out)
{
    Set_Attribute_Ptr_attribute(*(in), *out);
    free(in);
}

void JavaPRule2CPRulePtr(struct Horg_w3c_thotlib_PRule* in, PRule **out)
{
    *out = (PRule *) malloc(sizeof(PRule));
    **out = (PRule) Get_PRule_Ptr_prule(in);
}
void CPRulePtr2JavaPRule(PRule *in, struct Horg_w3c_thotlib_PRule** out)
{
    Set_PRule_Ptr_prule(*in, *out);
    free(in);
}

void JavaAttributeType2CAttributeTypePtr(struct Horg_w3c_thotlib_AttributeType* in, AttributeType **out)
{
    *out = (AttributeType *) malloc(sizeof(AttributeType));
    (*out)->AttrSSchema = (SSchema) Get_AttributeType_Ptr_sschema(in);
    (*out)->AttrTypeNum = Get_AttributeType_Int_type(in);
}
void CAttributeTypePtr2JavaAttributeType(AttributeType *in, struct Horg_w3c_thotlib_AttributeType** out)
{
    Set_AttributeType_Ptr_sschema(in->AttrSSchema, *out);
    Set_AttributeType_Int_type(in->AttrTypeNum, *out);
    free(in);
}
void CAttributeType2JavaAttributeType(AttributeType in, struct Horg_w3c_thotlib_AttributeType* out)
{
    Set_AttributeType_Ptr_sschema(in.AttrSSchema, out);
    Set_AttributeType_Int_type(in.AttrTypeNum, out);
}

void JavaIntPtr2CintPtr(struct Horg_w3c_thotlib_IntPtr* in, int **out)
{
    *out = (int *) malloc(sizeof(int));
    **out = (int) Get_IntPtr_Int_value(in);
}
void CintPtr2JavaIntPtr(int *in, struct Horg_w3c_thotlib_IntPtr **out)
{
    Set_IntPtr_Int_value(*in, *out);
    free(in);
}

void JavaLanguage2CLanguagePtr(struct Horg_w3c_thotlib_Language* in, Language **out)
{
    *out = (Language *) malloc(sizeof(Language));
    *(*out) = (char) Get_Language_Char_lang(in);
}
void CLanguagePtr2JavaLanguage(Language *in, struct Horg_w3c_thotlib_Language** out)
{
    Set_Language_Char_lang(*(in), *out);
    free(in);
}

/*
 * C pointer <=> Java long
 */

void CPixmap2Javalong(Pixmap in, jlong *out)
{
    *out = CPtr2JavaLong((void *) in);
}
void Javalong2CPixmap(jlong in, Pixmap *out)
{
    *out = (Pixmap) JavaLong2CPtr(in);
}

void CThotIcon2Javalong(ThotIcon in, jlong *out)
{
    *out = CPtr2JavaLong((void *) in);
}
void Javalong2CThotIcon(jlong in, ThotIcon *out)
{
    *out = (ThotIcon) JavaLong2CPtr(in);
}

void CElement2Javalong(Element in, jlong *out)
{
    *out = CPtr2JavaLong(in);
}
void Javalong2CElement(jlong in, Element *out)
{
    *out = JavaLong2CPtr(in);
}

void CSSchema2Javalong(SSchema in, jlong *out)
{
    *out = CPtr2JavaLong(in);
}
void Javalong2CSSchema(jlong in, SSchema *out)
{
    *out = JavaLong2CPtr(in);
}

void CAttribute2Javalong(Attribute in, jlong *out)
{
    *out = CPtr2JavaLong(in);
}
void Javalong2CAttribute(jlong in, Attribute *out)
{
    *out = JavaLong2CPtr(in);
}

void CPRule2Javalong(PRule in, jlong *out)
{
    *out = CPtr2JavaLong(in);
}
void Javalong2CPRule(jlong in, PRule *out)
{
    *out = JavaLong2CPtr(in);
}

void CThotWidget2Javalong(ThotWidget in, jlong *out)
{
    *out = CPtr2JavaLong(in);
}
void Javalong2CThotWidget(jlong in, ThotWidget *out)
{
    *out = JavaLong2CPtr(in);
}

/****************************************************************
 *								*
 *		String exchanges stubs				*
 *								*
 ****************************************************************/

#ifdef DIRECT_JAVA_VM_ACCESS
/*
 * This is the trickiest stub:
 *   On entry we get a StringBuffer and pass it as a char *
 */
void JavaStringBuffer2CcharPtr(struct Hjava_lang_StringBuffer* in, char **out)
{
    /* First, get the Java char[] associated to the StringBuffer */
    HArrayOfChar *buffer = unhand(in)->value;

    if (buffer->length < 30) {
        fprintf(stderr,
	  "WARNING : JavaStringBuffer2CcharPtr, using StringBuffer of size %d\n",
	  buffer->length);
    }

    /*
     * Second, dereference it to get the pointer to the actual data
     * Now pray that we won't overflow the memory allocated !
     */
    *out = (char *) &(unhand(buffer));
}

void CcharPtr2JavaStringBuffer(char *in, struct Hjava_lang_StringBuffer** out)
{
    /*
     * One need now to convert the internal StringBuffer representation
     * from a C string to a Java internal one.
     */
    HArrayOfChar *buffer = unhand(*out)->value;
    int buf_size = buffer->length;
    char *str = (char *) &(unhand(buffer));
    int str_size = strlen(str);
    char *tmp, *src, *dst;

    if (lsbf_architecture < 0) is_lsbf_architecture();
    if (buf_size <= 1) return;
    if (str_size == 0) {
        str[0] = '\0';
        str[1] = '\0';
	unhand(*out)->count = 0;
	return;
    }
    src = tmp = strdup(str);
    dst = str;
    while ((*src != '\0') && ((dst - str) < buf_size - 2)) {
        if (lsbf_architecture) {
	    *dst++ = *src++;
	    *dst++ = '\0';
	} else {
	    *dst++ = '\0';
	    *dst++ = *src++;
	}
    }
    unhand(*out)->count = (src - tmp);
    *dst++ = '\0';
    *dst++ = '\0';
    free(tmp);
}

#else /* ! DIRECT_JAVA_VM_ACCESS */

/*
 * This is the trickiest stub:
 *   On entry we get a StringBuffer and pass it as a char *
 */
void JavaStringBuffer2CcharPtr(struct Hjava_lang_StringBuffer* in, char **out)
{
    char *buffer;
    int capacity;
    struct Hjava_lang_String *string;
    
    capacity = do_execute_java_method((Hjava_lang_Object*) (in),
		        "capacity", "()I", 0, 0).i;
    if (capacity < MAX_PATH) capacity = MAX_PATH;
    *out = buffer = (char *) malloc(capacity);
    if (buffer == NULL) return;
    buffer[0] = '\0';
    string = (struct Hjava_lang_String *)
             do_execute_java_method((Hjava_lang_Object*) (in),
			"toString", "()Ljava/lang/String;", 0, 0).l;
    if (string == NULL) return;
    javaString2CString(string, buffer, capacity);
}

void CcharPtr2JavaStringBuffer(char *in, struct Hjava_lang_StringBuffer** out)
{
    int len = strlen(in);
    struct Hjava_lang_String *string;

    do_execute_java_method((Hjava_lang_Object*) (*out),
                         "ensureCapacity", "(I)V", 0, 0, (jint) len);
    string = makeJavaString(in, len);
    if (string == NULL) return;
    do_execute_java_method((Hjava_lang_Object*) (*out),
	 "insert", "(ILjava/lang/String;)Ljava/lang/StringBuffer;", 0, 0,
			 0, string);
    free(in);
}

#endif /* DIRECT_JAVA_VM_ACCESS */

/* assuming STRING is a simple char* */
void JavaStringBuffer2CSTRING(struct Hjava_lang_StringBuffer* in, char **out)
{
    JavaStringBuffer2CcharPtr(in, out);
}
void CSTRING2JavaStringBuffer(char *in, struct Hjava_lang_StringBuffer** out)
{
    CcharPtr2JavaStringBuffer(in, out);
}
