/*
 * Implementation for the conversion funtions between internal
 * types and external ones.
 */

#include "JavaTypes.h"

/*
 * convert 2 * 32 structs <=> Java long
 */
void Javalong2CElementType(jlong in, ElementType *out)
{
    memcpy(out, &in, sizeof(ElementType));
}
void Javalong2CAttributeType(jlong in, AttributeType *out)
{
    memcpy(out, &in, sizeof(AttributeType));
}
void CElementType2Javalong(ElementType in, jlong *out)
{
    memcpy(out, &in, sizeof(jlong));
}
void CAttributeType2Javalong(AttributeType in, jlong *out)
{
    memcpy(out, &in, sizeof(jlong));
}

/*
 * C object <=> Java object content
 */
void JavaElement2CElement(struct Hthotlib_Element *in, Element *out)
{
    *out = (Element) unhand(in)->element;
}

/*
 * C object pointer <=> pointers to Java object content
 */
void JavaElement2CElementPtr(struct Hthotlib_Element* in, Element **out)
{
    *out = (Element *) &(unhand(in)->element);
}
void CElementPtr2JavaElement(Element *in, struct Hthotlib_Element** out)
{
}

void JavaElementType2CElementTypePtr(struct Hthotlib_ElementType* in, ElementType **out)
{
    *out = (ElementType *) &(unhand(in)->elementType);
}
void CElementTypePtr2JavaElementType(ElementType *in, struct Hthotlib_ElementType** out)
{
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
    *out = (SSchema *) &(unhand(in)->sschema);
}
void CSSchemaPtr2JavaSSchema(SSchema *in, struct Hthotlib_SSchema** out)
{
}

void JavaAttribute2CAttributePtr(struct Hthotlib_Attribute* in, Attribute **out)
{
    *out = (Attribute *) &(unhand(in)->attribute);
}
void CAttributePtr2JavaAttribute(Attribute *in, struct Hthotlib_Attribute** out)
{
}

void JavaPRule2CPRulePtr(struct Hthotlib_PRule* in, PRule **out)
{
    *out = (PRule *) &(unhand(in)->prule);
}
void CPRulePtr2JavaPRule(PRule *in, struct Hthotlib_PRule** out)
{
}

void JavaAttributeType2CAttributeTypePtr(struct Hthotlib_AttributeType* in, AttributeType **out)
{
    *out = (AttributeType *) &(unhand(in)->attributeType);
}
void CAttributeTypePtr2JavaAttributeType(AttributeType *in, struct Hthotlib_AttributeType** out)
{
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

    /*
     * Second, dereference it to get the pointer to the actual data
     * Now pray that we won't overflow the memory allocated !
     */
    *out = (char *) &(unhand(buffer));
}

void CcharPtr2JavaStringBuffer(char *in, struct Hjava_lang_StringBuffer** out)
{
}

/*
 * C pointer <=> Java long
 */

void CPixmap2Javalong(Pixmap in, jlong *out)
{
    *out = (jlong) in;
}
void Javalong2CPixmap(jlong in, Pixmap *out)
{
#if SIZEOF_VOIDP == SIZEOF_INT
    unsigned int temp;
    temp = (unsigned int) in;
    *out = (void *) temp;
#elif SIZEOF_VOIDP == SIZEOF_LONG
    *out = (void *) (unsigned long) in;
#elif SIZEOF_VOIDP == SIZEOF_LONG_LONG
    *out = (void *) (unsigned long long) in;
#elif SIZEOF_VOIDP == SIZEOF___INT64
    *out = (void *) (unsigned __int64) in;
#endif
}

void CElement2Javalong(Element in, jlong *out)
{
    *out = (jlong) in;
}
void Javalong2CElement(jlong in, Element *out)
{
#if SIZEOF_VOIDP == SIZEOF_INT
    unsigned int temp;
    temp = (unsigned int) in;
    *out = (void *) temp;
#elif SIZEOF_VOIDP == SIZEOF_LONG
    *out = (void *) (unsigned long) in;
#elif SIZEOF_VOIDP == SIZEOF_LONG_LONG
    *out = (void *) (unsigned long long) in;
#elif SIZEOF_VOIDP == SIZEOF___INT64
    *out = (void *) (unsigned __int64) in;
#endif
}

void CSSchema2Javalong(SSchema in, jlong *out)
{
    *out = (jlong) in;
}
void Javalong2CSSchema(jlong in, SSchema *out)
{
#if SIZEOF_VOIDP == SIZEOF_INT
    unsigned int temp;
    temp = (unsigned int) in;
    *out = (void *) temp;
#elif SIZEOF_VOIDP == SIZEOF_LONG
    *out = (void *) (unsigned long) in;
#elif SIZEOF_VOIDP == SIZEOF_LONG_LONG
    *out = (void *) (unsigned long long) in;
#elif SIZEOF_VOIDP == SIZEOF___INT64
    *out = (void *) (unsigned __int64) in;
#endif
}


void CAttribute2Javalong(Attribute in, jlong *out)
{
    *out = (jlong) in;
}
void Javalong2CAttribute(jlong in, Attribute *out)
{
#if SIZEOF_VOIDP == SIZEOF_INT
    unsigned int temp;
    temp = (unsigned int) in;
    *out = (void *) temp;
#elif SIZEOF_VOIDP == SIZEOF_LONG
    *out = (void *) (unsigned long) in;
#elif SIZEOF_VOIDP == SIZEOF_LONG_LONG
    *out = (void *) (unsigned long long) in;
#elif SIZEOF_VOIDP == SIZEOF___INT64
    *out = (void *) (unsigned __int64) in;
#endif
}


void CPRule2Javalong(PRule in, jlong *out)
{
    *out = (jlong) in;
}
void Javalong2CPRule(jlong in, PRule *out)
{
#if SIZEOF_VOIDP == SIZEOF_INT
    unsigned int temp;
    temp = (unsigned int) in;
    *out = (void *) temp;
#elif SIZEOF_VOIDP == SIZEOF_LONG
    *out = (void *) (unsigned long) in;
#elif SIZEOF_VOIDP == SIZEOF_LONG_LONG
    *out = (void *) (unsigned long long) in;
#elif SIZEOF_VOIDP == SIZEOF___INT64
    *out = (void *) (unsigned __int64) in;
#endif
}


void CThotWidget2Javalong(ThotWidget in, jlong *out)
{
    *out = (jlong) in;
}
void Javalong2CThotWidget(jlong in, ThotWidget *out)
{
#if SIZEOF_VOIDP == SIZEOF_INT
    unsigned int temp;
    temp = (unsigned int) in;
    *out = (void *) temp;
#elif SIZEOF_VOIDP == SIZEOF_LONG
    *out = (void *) (unsigned long) in;
#elif SIZEOF_VOIDP == SIZEOF_LONG_LONG
    *out = (void *) (unsigned long long) in;
#elif SIZEOF_VOIDP == SIZEOF___INT64
    *out = (void *) (unsigned __int64) in;
#endif
}

