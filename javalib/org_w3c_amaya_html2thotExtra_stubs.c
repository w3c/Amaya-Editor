/*
 * Kaffe stubs generated for class org_w3c_amaya_html2thotExtra.
 * This class encapsulate call to the Thot lib api which could not
 * be built automatically by javastub program.
 */

#include "JavaTypes.h"
#include "org_w3c_amaya_html2thotExtra_stubs.h"
#include "html2thot_f.h"

#ifndef org_w3c_amaya_html2thotExtra_LOCK
#define org_w3c_amaya_html2thotExtra_LOCK() fprintf(stderr,"org_w3c_amaya_html2thotExtra_L
OCK undefined");
#endif /* org_w3c_amaya_html2thotExtra_LOCK */
#ifndef org_w3c_amaya_html2thotExtra_UNLOCK
#define org_w3c_amaya_html2thotExtra_UNLOCK() fprintf(stderr,"org_w3c_amaya_html2thotExtra
_UNLOCK undefined");
#endif /* org_w3c_amaya_html2thotExtra_UNLOCK */

/*
 * Java to C function GIType stub.
 */
void
org_w3c_amaya_html2thotExtra_GIType(struct Hjava_lang_String* gi,
			       struct Horg_w3c_thotlib_ElementType* elType,
			       jint doc)
{
    ElementType et;
    char cgi[1024];

    javaString2CString(gi, cgi, sizeof(cgi));
    JavaThotlibLock();
    GIType(cgi, &et, (Document) doc);
    JavaThotlibRelease();
    CElementType2JavaElementType(et, elType);
}


/*
 * Java to C function MapHTMLAttribute stub.
 */
void
org_w3c_amaya_html2thotExtra_MapHTMLAttribute(struct Hjava_lang_String* attr,
			       struct Horg_w3c_thotlib_AttributeType* attrType,
			       struct Hjava_lang_String* elementName,
			       jint doc)
{
    AttributeType at;
    char cattr[1024];
    char cgi[1024];

    javaString2CString(attr, cattr, sizeof(cattr));
    javaString2CString(elementName, cgi, sizeof(cgi));
    JavaThotlibLock();
    MapHTMLAttribute(cattr, &at, cgi, (Document) doc);
    JavaThotlibRelease();
    CAttributeType2JavaAttributeType(at, attrType);
}




/*
 * the following was originally automatically generated
 */

/*
 * Java to C function SkipSep stub.
 */
struct Hjava_lang_String*
org_w3c_amaya_html2thotExtra_SkipSep(struct Hjava_lang_String* jptr)
{
	char *res;
	char ptr[1024];
	char *ptr_ptr = &ptr[0];

	if (jptr != NULL)
	  javaString2CString(jptr, ptr_ptr, sizeof(ptr));
	else
	  ptr_ptr = NULL;

	org_w3c_amaya_html2thotExtra_LOCK();

	res = SkipSep((char *) ptr_ptr);

	org_w3c_amaya_html2thotExtra_UNLOCK();


	if (res == NULL)
		return(NULL);
	else;
		return(makeJavaString(res, strlen(res)));
}

/*
 * Java to C function SkipInt stub.
 */
struct Hjava_lang_String*
org_w3c_amaya_html2thotExtra_SkipInt(struct Hjava_lang_String* jptr)
{
	char *res;
	char ptr[1024];
	char *ptr_ptr = &ptr[0];

	if (jptr != NULL)
	  javaString2CString(jptr, ptr_ptr, sizeof(ptr));
	else
	  ptr_ptr = NULL;

	org_w3c_amaya_html2thotExtra_LOCK();

	res = SkipInt((char *) ptr_ptr);

	org_w3c_amaya_html2thotExtra_UNLOCK();


	if (res == NULL)
		return(NULL);
	else;
		return(makeJavaString(res, strlen(res)));
}

/*
 * Java to C function GITagName stub.
 */
struct Hjava_lang_String*
org_w3c_amaya_html2thotExtra_GITagName(jlong jelem)
{
	char *res;
	Element elem;

	/* convert arg jlong jelem to Element elem */
	Javalong2CElement(jelem,&elem);

	org_w3c_amaya_html2thotExtra_LOCK();

	res = GITagName((Element ) elem);

	org_w3c_amaya_html2thotExtra_UNLOCK();


	if (res == NULL)
		return(NULL);
	else;
		return(makeJavaString(res, strlen(res)));
}

/*
 * Java to C function MapThotAttr stub.
 */
jint
org_w3c_amaya_html2thotExtra_MapThotAttr(struct Hjava_lang_String* jAttr, struct Hjava_lang_String* jtag)
{
	int res;
	char Attr[1024];
	char *Attr_ptr = &Attr[0];
	char tag[1024];
	char *tag_ptr = &tag[0];

	if (jAttr != NULL)
	  javaString2CString(jAttr, Attr_ptr, sizeof(Attr));
	else
	  Attr_ptr = NULL;
	if (jtag != NULL)
	  javaString2CString(jtag, tag_ptr, sizeof(tag));
	else
	  tag_ptr = NULL;

	org_w3c_amaya_html2thotExtra_LOCK();

	res = MapThotAttr((char *) Attr_ptr, (char *) tag_ptr);

	org_w3c_amaya_html2thotExtra_UNLOCK();


	return((jint) res);
}

/*
 * Java to C function MapAttrValue stub.
 */
jint
org_w3c_amaya_html2thotExtra_MapAttrValue(jint ThotAtt, struct Hjava_lang_String* jAttrVal)
{
	int res;
	char AttrVal[1024];
	char *AttrVal_ptr = &AttrVal[0];

	if (jAttrVal != NULL)
	  javaString2CString(jAttrVal, AttrVal_ptr, sizeof(AttrVal));
	else
	  AttrVal_ptr = NULL;

	org_w3c_amaya_html2thotExtra_LOCK();

	res = MapAttrValue((int ) ThotAtt, (char *) AttrVal_ptr);

	org_w3c_amaya_html2thotExtra_UNLOCK();


	return((jint) res);
}

/*
 * Java to C function CreateAttrWidthPercentPxl stub.
 */
void
org_w3c_amaya_html2thotExtra_CreateAttrWidthPercentPxl(struct Hjava_lang_String* jbuffer, jlong jel, jint doc, jint oldWidth)
{
	char buffer[1024];
	char *buffer_ptr = &buffer[0];
	Element el;

	if (jbuffer != NULL)
	  javaString2CString(jbuffer, buffer_ptr, sizeof(buffer));
	else
	  buffer_ptr = NULL;
	/* convert arg jlong jel to Element el */
	Javalong2CElement(jel,&el);

	org_w3c_amaya_html2thotExtra_LOCK();

	CreateAttrWidthPercentPxl((char *) buffer_ptr, (Element ) el, (Document ) doc, (int ) oldWidth);

	org_w3c_amaya_html2thotExtra_UNLOCK();

}

/*
 * Java to C function CreateAttrIntSize stub.
 */
void
org_w3c_amaya_html2thotExtra_CreateAttrIntSize(struct Hjava_lang_String* jbuffer, jlong jel, jint doc)
{
	char buffer[1024];
	char *buffer_ptr = &buffer[0];
	Element el;

	if (jbuffer != NULL)
	  javaString2CString(jbuffer, buffer_ptr, sizeof(buffer));
	else
	  buffer_ptr = NULL;
	/* convert arg jlong jel to Element el */
	Javalong2CElement(jel,&el);

	org_w3c_amaya_html2thotExtra_LOCK();

	CreateAttrIntSize((char *) buffer_ptr, (Element ) el, (Document ) doc);

	org_w3c_amaya_html2thotExtra_UNLOCK();

}

/*
 * Java to C function CheckAbstractTree stub.
 */
void
org_w3c_amaya_html2thotExtra_CheckAbstractTree(struct Hjava_lang_String* jpathURL)
{
	char pathURL[1024];
	char *pathURL_ptr = &pathURL[0];

	if (jpathURL != NULL)
	  javaString2CString(jpathURL, pathURL_ptr, sizeof(pathURL));
	else
	  pathURL_ptr = NULL;

	org_w3c_amaya_html2thotExtra_LOCK();

	CheckAbstractTree((char *) pathURL_ptr);

	org_w3c_amaya_html2thotExtra_UNLOCK();

}

/*
 * Java to C function ParseIncludedHTML stub.
 */
void
org_w3c_amaya_html2thotExtra_ParseIncludedHTML(jlong jelem, struct Hjava_lang_String* jclosingTag)
{
	Element elem;
	char closingTag[1024];
	char *closingTag_ptr = &closingTag[0];

	/* convert arg jlong jelem to Element elem */
	Javalong2CElement(jelem,&elem);
	if (jclosingTag != NULL)
	  javaString2CString(jclosingTag, closingTag_ptr, sizeof(closingTag));
	else
	  closingTag_ptr = NULL;

	org_w3c_amaya_html2thotExtra_LOCK();

	ParseIncludedHTML((Element ) elem, (char *) closingTag_ptr);

	org_w3c_amaya_html2thotExtra_UNLOCK();

}

/*
 * Java to C function ParseSubTree stub.
 */
void
org_w3c_amaya_html2thotExtra_ParseSubTree(struct Hjava_lang_String* jHTMLbuf, jlong jlastelem, jbool isclosed, jint doc)
{
	char HTMLbuf[1024];
	char *HTMLbuf_ptr = &HTMLbuf[0];
	Element lastelem;

	if (jHTMLbuf != NULL)
	  javaString2CString(jHTMLbuf, HTMLbuf_ptr, sizeof(HTMLbuf));
	else
	  HTMLbuf_ptr = NULL;
	/* convert arg jlong jlastelem to Element lastelem */
	Javalong2CElement(jlastelem,&lastelem);

	org_w3c_amaya_html2thotExtra_LOCK();

	ParseSubTree((char *) HTMLbuf_ptr, (Element ) lastelem, (boolean ) isclosed, (Document ) doc);

	org_w3c_amaya_html2thotExtra_UNLOCK();

}

/*
 * Java to C function StartParser stub.
 */
void
org_w3c_amaya_html2thotExtra_StartParser(jint doc, struct Hjava_lang_String* jhtmlFileName, struct Hjava_lang_String* jdocumentName, struct Hjava_lang_String* jdocumentDirectory, struct Hjava_lang_String* jpathURL, jbool PlainText)
{
	char htmlFileName[1024];
	char *htmlFileName_ptr = &htmlFileName[0];
	char documentName[1024];
	char *documentName_ptr = &documentName[0];
	char documentDirectory[1024];
	char *documentDirectory_ptr = &documentDirectory[0];
	char pathURL[1024];
	char *pathURL_ptr = &pathURL[0];

	if (jhtmlFileName != NULL)
	  javaString2CString(jhtmlFileName, htmlFileName_ptr, sizeof(htmlFileName));
	else
	  htmlFileName_ptr = NULL;
	if (jdocumentName != NULL)
	  javaString2CString(jdocumentName, documentName_ptr, sizeof(documentName));
	else
	  documentName_ptr = NULL;
	if (jdocumentDirectory != NULL)
	  javaString2CString(jdocumentDirectory, documentDirectory_ptr, sizeof(documentDirectory));
	else
	  documentDirectory_ptr = NULL;
	if (jpathURL != NULL)
	  javaString2CString(jpathURL, pathURL_ptr, sizeof(pathURL));
	else
	  pathURL_ptr = NULL;

	org_w3c_amaya_html2thotExtra_LOCK();

	StartParser((Document ) doc, (char *) htmlFileName_ptr, (char *) documentName_ptr, (char *) documentDirectory_ptr, (char *) pathURL_ptr, (boolean ) PlainText);

	org_w3c_amaya_html2thotExtra_UNLOCK();

}

/*
 * Function to register all org_w3c_amaya_html2thotExtra stubs.
 */
void register_org_w3c_amaya_html2thotExtra_stubs(void)
{
	addNativeMethod("org_w3c_amaya_html2thotExtra_GIType", org_w3c_amaya_html2thotExtra_GIType);
	addNativeMethod("org_w3c_amaya_html2thotExtra_MapHTMLAttribute", org_w3c_amaya_html2thotExtra_MapHTMLAttribute);

	addNativeMethod("org_w3c_amaya_html2thotExtra_SkipSep", org_w3c_amaya_html2thotExtra_SkipSep);
	addNativeMethod("org_w3c_amaya_html2thotExtra_SkipInt", org_w3c_amaya_html2thotExtra_SkipInt);
	addNativeMethod("org_w3c_amaya_html2thotExtra_GITagName", org_w3c_amaya_html2thotExtra_GITagName);
	addNativeMethod("org_w3c_amaya_html2thotExtra_MapThotAttr", org_w3c_amaya_html2thotExtra_MapThotAttr);
	addNativeMethod("org_w3c_amaya_html2thotExtra_MapAttrValue", org_w3c_amaya_html2thotExtra_MapAttrValue);
	addNativeMethod("org_w3c_amaya_html2thotExtra_CreateAttrWidthPercentPxl", org_w3c_amaya_html2thotExtra_CreateAttrWidthPercentPxl);
	addNativeMethod("org_w3c_amaya_html2thotExtra_CreateAttrIntSize", org_w3c_amaya_html2thotExtra_CreateAttrIntSize);
	addNativeMethod("org_w3c_amaya_html2thotExtra_CheckAbstractTree", org_w3c_amaya_html2thotExtra_CheckAbstractTree);
	addNativeMethod("org_w3c_amaya_html2thotExtra_ParseIncludedHTML", org_w3c_amaya_html2thotExtra_ParseIncludedHTML);
	addNativeMethod("org_w3c_amaya_html2thotExtra_ParseSubTree", org_w3c_amaya_html2thotExtra_ParseSubTree);
	addNativeMethod("org_w3c_amaya_html2thotExtra_StartParser", org_w3c_amaya_html2thotExtra_StartParser);
}

