/*
 * Implementation for the conversion funtions between internal
 * types and external ones.
 */

#include "JavaTypes.h"

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
void JavaElement2CElement(struct Hthotlib_Element *in, Element *out)
{
    *out = (Element) unhand(in)->element;
}
