/*
 * Implementation for the conversion funtions between internal
 * types and external ones.
 */

#include "JavaTypes.h"
#include "JavaTypes_f.h"

/* #define DEBUG_ARCH */

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
 * Broblem when storing pointers in Java long and passing them
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

void do_ptr_need_shift(void) {
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

int *JavaLongPtr2CIntPtr(jlong *in)
{
    int *res = (int *) in;

    if (int_ptr_need_shift < 0) do_ptr_need_shift();

    if (int_ptr_need_shift) res++;
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
typedef union tst_lsbf { char str[2]; unsigned short int2; };
void is_lsbf_architecture(void) {
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

/*
 * C object <=> Java object content
 */
void JavaElement2CElement(struct Hthotlib_Element *in, Element *out)
{
    *out = (Element) JavaLong2CPtr(unhand(in)->element);
}

/*
 * C object pointer <=> pointers to Java object content
 */
void JavaElement2CElementPtr(struct Hthotlib_Element* in, Element **out)
{
    *out = (Element *) JavaLongPtr2CIntPtr(&(unhand(in)->element));
}
void CElementPtr2JavaElement(Element *in, struct Hthotlib_Element** out)
{
}

void JavaElementType2CElementTypePtr(struct Hthotlib_ElementType* in, ElementType **out)
{
    *out = (ElementType *) malloc(sizeof(ElementType));
    (*out)->ElSSchema = (SSchema) JavaLong2CPtr(unhand(in)->sschema);
    (*out)->ElTypeNum = (int) (unhand(in)->type);
}
void CElementTypePtr2JavaElementType(ElementType *in, struct Hthotlib_ElementType** out)
{
    unhand(*out)->sschema = CPtr2JavaLong(in->ElSSchema);
    unhand(*out)->type = (jint) in->ElTypeNum;
    free(in);
}
void CElementType2JavaElementType(ElementType in, struct Hthotlib_ElementType* out)
{
    unhand(out)->sschema = CPtr2JavaLong(in.ElSSchema);
    unhand(out)->type = (jint) in.ElTypeNum;
}


void JavaDocument2CDocumentPtr(struct Hthotlib_Document* in, Document **out)
{
    *out = (Document *) &(unhand(in)->document);
}
void CDocumentPtr2JavaDocument(Document *in, struct Hthotlib_Document** out)
{
}

void JavaSSchema2CSSchemaPtr(struct Hthotlib_SSchema* in, SSchema **out)
{
    *out = (SSchema *) JavaLongPtr2CIntPtr(&(unhand(in)->sschema));
}
void CSSchemaPtr2JavaSSchema(SSchema *in, struct Hthotlib_SSchema** out)
{
}

void JavaAttribute2CAttributePtr(struct Hthotlib_Attribute* in, Attribute **out)
{
    *out = (Attribute *) JavaLongPtr2CIntPtr(&(unhand(in)->attribute));
}
void CAttributePtr2JavaAttribute(Attribute *in, struct Hthotlib_Attribute** out)
{
}

void JavaPRule2CPRulePtr(struct Hthotlib_PRule* in, PRule **out)
{
    *out = (PRule *) JavaLongPtr2CIntPtr(&(unhand(in)->prule));
}
void CPRulePtr2JavaPRule(PRule *in, struct Hthotlib_PRule** out)
{
}

void JavaAttributeType2CAttributeTypePtr(struct Hthotlib_AttributeType* in, AttributeType **out)
{
    *out = (AttributeType *) malloc(sizeof(AttributeType));
    (*out)->AttrSSchema = (SSchema) JavaLong2CPtr(unhand(in)->sschema);
    (*out)->AttrTypeNum = (int) (unhand(in)->type);
}
void CAttributeTypePtr2JavaAttributeType(AttributeType *in, struct Hthotlib_AttributeType** out)
{
    unhand(*out)->sschema = CPtr2JavaLong(in->AttrSSchema);
    unhand(*out)->type = (jint) in->AttrTypeNum;
    free(in);
}

void JavaIntPtr2CintPtr(struct Hthotlib_IntPtr* in, int **out)
{
    *out = (int *) &(unhand(in)->value);
}
void CintPtr2JavaIntPtr(int *in, struct Hthotlib_IntPtr** out)
{
}

void JavaLanguage2CLanguagePtr(struct Hthotlib_Language* in, Language **out)
{
    *out = (Language *) &(unhand(in)->lang);
}
void CLanguagePtr2JavaLanguage(Language *in, struct Hthotlib_Language** out)
{
}

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

