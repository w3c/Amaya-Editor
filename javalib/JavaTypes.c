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
void JavaElementType2CElementTypePtr(struct Hthotlib_ElementType* in, ElementType **out)
{
    *out = (ElementType *) &(unhand(in)->elementType);
}
void JavaDocument2CDocumentPtr(struct Hthotlib_Document* in, Document **out)
{
    *out = (Document *) &(unhand(in)->document);
}
void JavaSSchema2CSSchemaPtr(struct Hthotlib_SSchema* in, SSchema **out)
{
    *out = (SSchema *) &(unhand(in)->sschema);
}
void JavaAttribute2CAttributePtr(struct Hthotlib_Attribute* in, Attribute **out)
{
    *out = (Attribute *) &(unhand(in)->attribute);
}
