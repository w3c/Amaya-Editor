/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996-2002
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * XHTMLbuilder.c
 * Builds the corresponding abstract tree for a Thot document of type HTML.
 *
 * Authors: L. Carcone
 *          V. Quint 
 */

#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"
#include "parser.h"
#include "HTML.h"
#include "fetchHTMLname.h"

#include "css_f.h"
#include "fetchXMLname_f.h"
#include "fetchHTMLname_f.h"
#include "html2thot_f.h"
#include "HTMLactions_f.h"
#include "HTMLedit_f.h"
#include "HTMLform_f.h"
#include "HTMLimage_f.h"
#include "HTMLtable_f.h"
#include "HTMLimage_f.h"
#include "UIcss_f.h"
#include "styleparser_f.h"
#include "XHTMLbuilder_f.h"
#include "Xml2thot_f.h"

/* maximum length of a Thot structure schema name */
#define MAX_SS_NAME_LENGTH 32

#define MaxMsgLength 200

/* Elements that cannot contain text as immediate children.
   When some text is present in the HTML file it must be 
   surrounded by a Pseudo_paragraph element */
static int          NoTextChild[] =
{
   HTML_EL_Document, HTML_EL_HTML, HTML_EL_HEAD, HTML_EL_BODY,
   HTML_EL_Definition_List, HTML_EL_Block_Quote, HTML_EL_Directory,
   HTML_EL_Form, HTML_EL_Menu, HTML_EL_FIELDSET,
   HTML_EL_Numbered_List, HTML_EL_Option_Menu,
   HTML_EL_Unnumbered_List, HTML_EL_Definition, HTML_EL_List_Item,
   HTML_EL_MAP, HTML_EL_map, HTML_EL_Applet,
   HTML_EL_Object, HTML_EL_IFRAME, HTML_EL_NOFRAMES,
   HTML_EL_Division, HTML_EL_Center, HTML_EL_NOSCRIPT,
   HTML_EL_Data_cell, HTML_EL_Heading_cell,
   0};

/* Define a pointer to let parser functions access the HTML entity table */
extern XmlEntity *pXhtmlEntityTable;

/* maximum size of error messages */
#define MaxMsgLength 200

/*----------------------------------------------------------------------
  ParseCharset:
  Parses the element HTTP-EQUIV and looks for the charset value.
  ----------------------------------------------------------------------*/
void             ParseCharset (Element el, Document doc) 

{
   AttributeType attrType;
   Attribute     attr;
   SSchema       docSSchema;
   CHARSET       charset;
   char         *text, *text2, *ptrText, *str;
   char          charsetname[MAX_LENGTH];
   int           length;
   int           pos, index = 0;

   charset = TtaGetDocumentCharset (doc);
   if (charset != UNDEFINED_CHARSET)
     /* the charset was already defined by the http header */
     return;

   docSSchema = TtaGetDocumentSSchema (doc);
   attrType.AttrSSchema = docSSchema;
   attrType.AttrTypeNum = HTML_ATTR_http_equiv;
   attr = TtaGetAttribute (el, attrType);
   if (attr != NULL)
     {
       /* There is a HTTP-EQUIV attribute */
       length = TtaGetTextAttributeLength (attr);
       if (length > 0)
	 {
	   text = TtaGetMemory (length + 1);
	   TtaGiveTextAttributeValue (attr, text, &length);
	   if (!strcasecmp (text, "content-type"))
	     {
	       attrType.AttrTypeNum = HTML_ATTR_meta_content;
	       attr = TtaGetAttribute (el, attrType);
	       if (attr != NULL)
		 {
		   length = TtaGetTextAttributeLength (attr);
		   if (length > 0)
		     {
		       text2 = TtaGetMemory (length + 1);
		       TtaGiveTextAttributeValue (attr, text2, &length);
		       ptrText = text2;
		       while (*ptrText)
			 {
			   *ptrText = tolower (*ptrText);
			   ptrText++;
			 }
		       
		       str = strstr (text2, "charset=");
		       if (str)
			 {
			   pos = str - text2 + 8;
			   while (text2[pos] != SPACE &&
				  text2[pos] != TAB && text2[pos] != EOS)
			     charsetname[index++] = text2[pos++];
			   charsetname[index] = EOS;
			   charset = TtaGetCharset (charsetname);
			   if (charset != UNDEFINED_CHARSET)
			     TtaSetDocumentCharset (doc, charset);
			 }
		       TtaFreeMemory (text2);
		     }       
		 } 
	     }
	   TtaFreeMemory (text);
	 }
     }
}

/*----------------------------------------------------------------------
   XhtmlCannotContainText 
   Return TRUE if element el is a block element.
  ----------------------------------------------------------------------*/
ThotBool      XhtmlCannotContainText (ElementType elType)

{
   int        i;
   ThotBool   ret;

   if (strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML"))
      /* not an HTML element */
      ret = TRUE;
   else
     {
       ret = FALSE;
       i = 0;
       while (NoTextChild[i] > 0 && NoTextChild[i] != elType.ElTypeNum)
           i++;
       if (NoTextChild[i] == elType.ElTypeNum)
           ret = TRUE;
     }
   return ret;
}

/*----------------------------------------------------------------------
  XhtmlElementComplete
  Complete Xhtml elements.
  Check its attributes and its contents.
  ----------------------------------------------------------------------*/
void       XhtmlElementComplete (Element el, Document doc, int *error)

{
   ElementType    elType, newElType, childType;
   Element        constElem, child, desc, leaf, prev, next, last,
	          elFrames, lastFrame, lastChild, parent, picture, content;
   Attribute      attr;
   AttributeType  attrType;
   Language       lang;
   char           *text;
   char           lastChar[2];
   char           *name1;
   int            length;
   SSchema        docSSchema;
   ThotBool       isImage;
   PresentationValue    pval;
   PresentationContext  ctxt;

   *error = 0;
   docSSchema = TtaGetDocumentSSchema (doc);

   elType = TtaGetElementType (el);
   /* is this a block-level element in a character-level element? */
   if (!IsXMLElementInline (elType, doc) &&
       elType.ElTypeNum != HTML_EL_Comment_ &&
       elType.ElTypeNum != HTML_EL_XMLPI)
       BlockInCharLevelElem (el);

   newElType.ElSSchema = elType.ElSSchema;
   switch (elType.ElTypeNum)
     {
     case HTML_EL_Object:	/* it's an object */
       isImage = FALSE;
       /* is there a type attribute on the object element? */
       attrType.AttrSSchema = elType.ElSSchema;
       attrType.AttrTypeNum = HTML_ATTR_Object_type;
       attr = TtaGetAttribute (el, attrType);
       if (attr)
	 /* there is a type attribute. Get its value to see if the object
	    represents an image */
	 {
	   length = TtaGetTextAttributeLength (attr);
	   if (length > 0)
	     {
	       name1 = TtaGetMemory (length + 1);
	       TtaGiveTextAttributeValue (attr, name1, &length);
	       if (!strcmp (name1, "application/mathml+xml") ||
		   !strcmp (name1, "application/postscript") ||
		   !strcmp (name1, "image/x-bitmap") ||
		   !strcmp (name1, "image/x-xpixmap") ||
		   !strcmp (name1, "image/gif") ||
		   !strcmp (name1, "image/jpeg") ||
		   !strcmp (name1, "image/png") ||
		   !strcmp (name1, "image/svg"))
		 isImage = TRUE;
	       TtaFreeMemory (name1);
	     }
	 }
       
       picture = NULL;     /* no PICTURE element yet */
       child = TtaGetFirstChild (el);
       if (isImage)
	 /* the object represents an image. We need a PICTURE element as
	    child of the object to hold the image */
	 {
	   if (child)
	     {
	       elType = TtaGetElementType (child);
	       if (elType.ElTypeNum == HTML_EL_PICTURE_UNIT)
		 /* there is already a PICTURE element */
		 picture = child;
	     }
	   /* if the object element has no PICTURE element as first child
	      create one */
	   if (!picture)
	     {
	       elType.ElTypeNum = HTML_EL_PICTURE_UNIT;
	       picture = TtaNewTree (doc, elType, "");
	       if (child)
		 TtaInsertSibling (picture, child, TRUE, doc);
	       else
		 TtaInsertFirstChild (&picture, el, doc);
	       child = picture;
	     }
	   /* copy attribute data of the object into the SRC attribute of
	      the PICTURE element */
	   attrType.AttrSSchema = elType.ElSSchema;
	   attrType.AttrTypeNum = HTML_ATTR_data;
	   attr = TtaGetAttribute (el, attrType);
	   if (attr)
	     /* the object has a data attribute */
	     {
	       length = TtaGetTextAttributeLength (attr);
	       if (length > 0)
		 {
		   name1 = TtaGetMemory (length + 1);
		   TtaGiveTextAttributeValue (attr, name1, &length);
		   attrType.AttrTypeNum = HTML_ATTR_SRC;
		   attr = TtaGetAttribute (picture, attrType);
		   if (attr == NULL)
		     {
		       attr = TtaNewAttribute (attrType);
		       TtaAttachAttribute (picture, attr, doc);
		     }
		   TtaSetAttributeText (attr, name1, picture, doc);
		   TtaFreeMemory (name1);
		 }
	     }
	 }
       /* is the Object_Content element already created ? */
       if (child)
	 /* the object element has at least 1 child element */
	 {
	   content = NULL;
	   desc = child;
	   elType = TtaGetElementType (desc);
	   if (elType.ElTypeNum != HTML_EL_Object_Content)
	     {
	       TtaNextSibling(&desc);
	       if (desc)
		 elType = TtaGetElementType (desc);
	     }
	   /* is it the Object_Content element ? */
	   if (elType.ElTypeNum == HTML_EL_Object_Content)
	     content = desc;
	   else
	     {
	       /* create an Object_Content element */
	       elType.ElTypeNum = HTML_EL_Object_Content;
	       content = TtaNewElement (doc, elType);
	       if (picture)
		 TtaInsertSibling (content, picture, FALSE, doc);
	       else
		 TtaInsertSibling (content, child, TRUE, doc);
	       /* move previous existing children into Object_Content */
	       child = TtaGetLastChild(el);
	       while (child != content)
		 {
		   TtaRemoveTree (child, doc);
		   TtaInsertFirstChild (&child, content, doc);
		   child = TtaGetLastChild(el);
		 }
	     }
	   if (picture && content)
	     /* there is a picture element. The Object_Content must not be
		displayed in the main view */
	     {
	       ctxt = TtaGetSpecificStyleContext (doc);
	       /* the presentation rule to be set is not a CSS rule */
	       ctxt->cssSpecificity = 0;
	       ctxt->destroy = FALSE;
	       pval.typed_data.unit = STYLE_UNIT_PX;
	       pval.typed_data.value = 0;
	       pval.typed_data.real = FALSE;
	       TtaSetStylePresentation (PRVisibility, content, NULL, ctxt, pval);
	     }
	 }
       break;

     case HTML_EL_Unnumbered_List:
     case HTML_EL_Numbered_List:
     case HTML_EL_Menu:
     case HTML_EL_Directory:
       /* It's a List element. It should only have List_Item children.
	  If it has List element chidren, move these List elements
	  within their previous List_Item sibling.  This is to fix
	  a bug in document generated by Mozilla. */
       prev = NULL;
       next = NULL;
       child = TtaGetFirstChild (el);
       while (child != NULL)
	 {
	   next = child;
	   TtaNextSibling (&next);
	   elType = TtaGetElementType (child);
	   if (elType.ElTypeNum == HTML_EL_Unnumbered_List ||
	       elType.ElTypeNum == HTML_EL_Numbered_List ||
	       elType.ElTypeNum == HTML_EL_Menu ||
	       elType.ElTypeNum == HTML_EL_Directory)
	     /* this list element is a child of another list element */
	     if (prev)
	       {
		 elType = TtaGetElementType (prev);
		 if (elType.ElTypeNum == HTML_EL_List_Item)
		   {
		     /* get the last child of the previous List_Item */
		     desc = TtaGetFirstChild (prev);
		     last = NULL;
		     while (desc)
		       {
			 last = desc;
			 TtaNextSibling (&desc);
		       }
		     /* move the list element after the last child of the
			previous List_Item */
		     TtaRemoveTree (child, doc);
		     if (last)
		       TtaInsertSibling (child, last, FALSE, doc);
		     else
		       TtaInsertFirstChild (&child, prev, doc);
		     child = prev;
		   }
	       }
	   prev = child;
	   child = next;
	 }
       break;
       
     case HTML_EL_FRAMESET:
       /* The FRAMESET element is now complete.  Gather all its FRAMESET
	  and FRAME children and wrap them up in a Frames element */
       elFrames = NULL; lastFrame = NULL;
       lastChild = NULL;
       child = TtaGetFirstChild (el);
       while (child != NULL)
	 {
	   next = child;
	   TtaNextSibling (&next);
	   elType = TtaGetElementType (child);
	   if (elType.ElTypeNum == HTML_EL_FRAMESET ||
	       elType.ElTypeNum == HTML_EL_FRAME ||
	       elType.ElTypeNum == HTML_EL_Comment_)
	     {
	       /* create the Frames element if it does not exist */
	       if (elFrames == NULL)
		 {
		   newElType.ElSSchema = docSSchema;
		   newElType.ElTypeNum = HTML_EL_Frames;
		   elFrames = TtaNewElement (doc, newElType);
		   if (DocumentMeta[doc]->xmlformat)
		     XmlSetElemLineNumber (elFrames);
		   else
		     SetHtmlElemLineNumber (elFrames);
		   TtaInsertSibling (elFrames, child, TRUE, doc);
		 }
	       /* move the element as the last child of the Frames element */
	       TtaRemoveTree (child, doc);
	       if (lastFrame == NULL)
		 TtaInsertFirstChild (&child, elFrames, doc);
	       else
		 TtaInsertSibling (child, lastFrame, FALSE, doc);
	       lastFrame = child;
	     }
	   child = next;
	 }
       break;
       
     case HTML_EL_Input:	/* it's an INPUT without any TYPE attribute */
       /* Create a child of type Text_Input */
       elType.ElTypeNum = HTML_EL_Text_Input;
       child = TtaNewTree (doc, elType, "");
       if (DocumentMeta[doc]->xmlformat)
	 XmlSetElemLineNumber (child);
       else
	 SetHtmlElemLineNumber (child);
       TtaInsertFirstChild (&child, el, doc);
       /* now, process it like a Text_Input element */

     case HTML_EL_Text_Input:
     case HTML_EL_Password_Input:
     case HTML_EL_File_Input:
       /* get element Inserted_Text */
       child = TtaGetFirstChild (el);
       if (child != NULL)
	 {
	   attrType.AttrSSchema = docSSchema;
	   attrType.AttrTypeNum = HTML_ATTR_Value_;
	   attr = TtaGetAttribute (el, attrType);
	   if (attr != NULL)
	     {
	       /* copy the value of attribute "value" into the first text
		  leaf of element */
	       length = TtaGetTextAttributeLength (attr);
	       if (length > 0)
		 {
		   /* get the text leaf */
		   leaf = TtaGetFirstChild (child);
		   if (leaf != NULL)
		     {
		       childType = TtaGetElementType (leaf);
		       if (childType.ElTypeNum == HTML_EL_TEXT_UNIT)
			 {
			   /* copy attribute value into the text leaf */
			   text = TtaGetMemory (length + 1);
			   TtaGiveTextAttributeValue (attr, text, &length);
			   TtaSetTextContent (leaf, text, 
					      TtaGetDefaultLanguage (), doc);
			   TtaFreeMemory (text);
			 }
		     }
		 }
	     }
	 }
       break;
       
     case HTML_EL_META:
       ParseCharset (el, doc);
       break;

     case HTML_EL_STYLE_:	/* it's a STYLE element */
     case HTML_EL_SCRIPT_:	/* it's a SCRIPT element */
     case HTML_EL_Preformatted:	/* it's a PRE */
       /* if the last line of the Preformatted is empty, remove it */
       leaf = XmlLastLeafInElement (el);
       if (leaf != NULL)
	 {
	   elType = TtaGetElementType (leaf);
	   if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
	     /* the last leaf is a TEXT element */
	     {
	       length = TtaGetTextLength (leaf);
	       if (length > 0)
		 {
		   TtaGiveSubString (leaf, lastChar, length, 1);
		   if (lastChar[0] == EOL)
		     /* last character is new line, delete it */
		     {
		       if (length == 1)
			 /* empty TEXT element */
			 TtaDeleteTree (leaf, doc);
		       else
			 /* remove the last character */
			 TtaDeleteTextContent (leaf, length, 1, doc);
		     }
		 }
	     }
	 }
       if (DocumentMeta[doc]->xmlformat)
	 {
	   if (IsXmlParsingCSS ())
	     {
	       text = GetStyleContents (el);
	       if (text)
		 {
		   ReadCSSRules (doc, NULL, text,
				 TtaGetElementLineNumber (el), FALSE);
		   TtaFreeMemory (text);
		 }
	       SetXmlParsingCSS (FALSE);
	     }
	 }
       else
	 {
	   if (IsHtmlParsingCSS ())
	     {
	       text = GetStyleContents (el);
	       if (text)
		 {
		   ReadCSSRules (doc, NULL, text,
				 TtaGetElementLineNumber (el), FALSE);
		   TtaFreeMemory (text);
		 }
	       SetHtmlParsingCSS (FALSE);
	     }
	 }
       /* and continue as if it were a Preformatted or a Script */
       break;
       
     case HTML_EL_Text_Area:	/* it's a Text_Area */
       SetParsingTextArea (FALSE);
       child = TtaGetFirstChild (el);
       if (child == NULL)
	 /* it's an empty Text_Area */
	 /* insert a Inserted_Text element in the element */
	 {
	   newElType.ElTypeNum = HTML_EL_Inserted_Text;
	   child = TtaNewTree (doc, newElType, "");
	   TtaInsertFirstChild (&child, el, doc);
	 }
       else
	 {
	   /* save the text into Default_Value attribute */
	   attrType.AttrSSchema = docSSchema;
	   attrType.AttrTypeNum = HTML_ATTR_Default_Value;
	   if (TtaGetAttribute (el, attrType) == NULL)
	     /* attribute Default_Value is missing */
	     {
	       attr = TtaNewAttribute (attrType);
	       TtaAttachAttribute (el, attr, doc);
	       desc = TtaGetFirstChild (child);
	       length = TtaGetTextLength (desc) + 1;
	       text = TtaGetMemory (length);
	       TtaGiveTextContent (desc, text, &length, &lang);
	       TtaSetAttributeText (attr, text, el, doc);
	       TtaFreeMemory (text);
	     }
	 }
       /* insert a Frame element */
       newElType.ElTypeNum = HTML_EL_Frame;
       constElem = TtaNewTree (doc, newElType, "");
       TtaInsertSibling (constElem, child, FALSE, doc);
       break;
       
     case HTML_EL_Radio_Input:
     case HTML_EL_Checkbox_Input:
       /* put an attribute Checked if it is missing */
       attrType.AttrSSchema = docSSchema;
       attrType.AttrTypeNum = HTML_ATTR_Checked;
       if (TtaGetAttribute (el, attrType) == NULL)
	 /* attribute Checked is missing */
	 {
	   attr = TtaNewAttribute (attrType);
	   TtaAttachAttribute (el, attr, doc);
	   TtaSetAttributeValue (attr, HTML_ATTR_Checked_VAL_No_, el, doc);
	 }
       break;
       
     case HTML_EL_Option_Menu:
       /* Check that at least one option has a SELECTED attribute */
       OnlyOneOptionSelected (el, doc, TRUE);
       break;

     case HTML_EL_PICTURE_UNIT:
       break;
       
     case HTML_EL_LINK:
       CheckCSSLink (el, doc, docSSchema);
       break;
       
     case HTML_EL_Data_cell:
     case HTML_EL_Heading_cell:
       /* insert a pseudo paragraph into empty cells */
       child = TtaGetFirstChild (el);
       if (child == NULL)
	 {
	   elType.ElTypeNum = HTML_EL_Pseudo_paragraph;
	   child = TtaNewTree (doc, elType, "");
	   if (child != NULL)
	       TtaInsertFirstChild (&child, el, doc);
	 }
       
       /* detect whether we're parsing a whole table or just a cell */
       if (DocumentMeta[doc]->xmlformat)
	 {
	   if (IsWithinXmlTable ())
	     NewCell (el, doc, FALSE);
	 }
       else
	 {
	   if (IsWithinHtmlTable ())
	     NewCell (el, doc, FALSE);
	 }
       break;
       
     case HTML_EL_Table:
       CheckTable (el, doc);
       SubWithinTable ();
       break;
       
     case HTML_EL_TITLE:
       /* show the TITLE in the main window */
       UpdateTitle (el, doc);
       break;

     case HTML_EL_rbc:
       /* an rbc element has been read. Its parent should be a complex_ruby.
	  Change the type of the parent, as simple_ruby are created by
	  default */
       parent = TtaGetParent (el);
       if (parent)
	 {
	   newElType = TtaGetElementType (parent);
	   if (newElType.ElSSchema == elType.ElSSchema &&
	       newElType.ElTypeNum == HTML_EL_simple_ruby)
	      ChangeElementType (parent, HTML_EL_complex_ruby);
	 }
       break;

     case HTML_EL_rtc1:
       /* an rtc element has been parsed. If it has already a rtc1 sibling,
	  change its type to rtc2 */
       prev = el;
       do
	 {
	   TtaPreviousSibling(&prev);
	   if (prev)
	     {
	       newElType = TtaGetElementType (prev);
	       if (newElType.ElSSchema == elType.ElSSchema &&
		   newElType.ElTypeNum == HTML_EL_rtc1)
		 {
		   ChangeElementType (el, HTML_EL_rtc2);
		   prev = NULL;
		 }
	     }
	 }
       while (prev);
       break;

     default:
       break;
     }
}

/*----------------------------------------------------------------------
   PutInContent    
   Put the string ChrString in the leaf of current element.
  ----------------------------------------------------------------------*/
Element         PutInContent (char *ChrString, ParserData *context)

{
   Element      el, child;
   ElementType  elType;
   int          length;

   el = NULL;
   if (context->lastElement != NULL)
     {
	/* search first leaf of current element */
	el = context->lastElement;
	do
	  {
	     child = TtaGetFirstChild (el);
	     if (child != NULL)
		el = child;
	  }
	while (child != NULL);
	elType = TtaGetElementType (el);
	length = 0;
	if (elType.ElTypeNum == 1)
	   length = TtaGetTextLength (el);
	if (length == 0)
	   TtaSetTextContent (el, ChrString, context->language, context->doc);
	else
	   TtaAppendTextContent (el, ChrString, context->doc);
     }
   return el;
}

/*----------------------------------------------------------------------
   UnknownXhtmlNameSpace
   The element doesn't belong to a supported namespace
  ----------------------------------------------------------------------*/
void               UnknownXhtmlNameSpace (ParserData *context,
					  Element *unknownEl,
					  char* content)
{
   ElementType     elType;
   Element         elText;

   /* Create a new Invalid_element */
   elType.ElSSchema = GetXMLSSchema (XHTML_TYPE, context->doc);
   elType.ElTypeNum = HTML_EL_Unknown_namespace;
   *unknownEl = TtaNewElement (context->doc, elType);
   if (*unknownEl != NULL)
     {
       XmlSetElemLineNumber (*unknownEl);
       InsertXmlElement (unknownEl);
       context->lastElementClosed = TRUE;
       elType.ElTypeNum = HTML_EL_TEXT_UNIT;
       elText = TtaNewElement (context->doc, elType);
       XmlSetElemLineNumber (elText);
       TtaInsertFirstChild (&elText, *unknownEl, context->doc);
       TtaSetTextContent (elText, content, context->language, context->doc);
       TtaSetAccessRight (elText, ReadOnly, context->doc);
     }
}

/*----------------------------------------------------------------------
   CreateHTMLAttribute
   create an attribute of type attrType for the element el.
  ----------------------------------------------------------------------*/
void           CreateHTMLAttribute (Element       el,
				    AttributeType attrType,
				    char*         text,
				    ThotBool      isInvalid,
				    Document      doc,
				    Attribute    *lastAttribute,
				    Element      *lastAttrElement)
     
{
   int         attrKind;
   int         length;
   char       *buffer;
   Attribute   attr, oldAttr;

   if (attrType.AttrTypeNum != 0)
     {
       oldAttr = TtaGetAttribute (el, attrType);
       if (oldAttr != NULL)
	 /* this attribute already exists */
	 attr = oldAttr;
       else
	 /* create a new attribute and attach it to the element */
	 {
	   attr = TtaNewAttribute (attrType);
	   TtaAttachAttribute (el, attr, doc);
	 }
       *lastAttribute = attr;
       *lastAttrElement = el;

       TtaGiveAttributeType (attr, &attrType, &attrKind);
       if (attrKind == 0)	/* enumerate */
	 TtaSetAttributeValue (attr, 1, el, doc);

       /* attribute BORDER without any value (ThotBool attribute) is */
       /* considered as BORDER=1 */
       if (attrType.AttrTypeNum == HTML_ATTR_Border)
	 TtaSetAttributeValue (attr, 1, el, doc);

       if (isInvalid)
	 /* Copy the name of the invalid attribute as the content */
	 /* of the Invalid_attribute attribute. */
	 {
	   length = strlen (text) + 2;
	   length += TtaGetTextAttributeLength (attr);
	   buffer = TtaGetMemory (length + 1);
	   TtaGiveTextAttributeValue (attr, buffer, &length);
	   strcat (buffer, " ");
	   strcat (buffer, text);
	   TtaSetAttributeText (attr, buffer, el, doc);
	   TtaFreeMemory (buffer);
	 }
     }
}

/*----------------------------------------------------------------------
   HTMLTypeAttrValue
   Value val has been read for the HTML attribute TYPE.
   Create a child for the current Thot element INPUT accordingly.
  ----------------------------------------------------------------------*/
void               HTMLTypeAttrValue (char       *val,
				      Attribute   lastAttribute,
				      Element     lastAttrElement,
				      ParserData *context)

{
  ElementType      elType;
  Element          newChild;
  AttributeType    attrType;
  Attribute        attr;
  char             msgBuffer[MaxMsgLength];
  int              value;
  int              numberOfLinesRead;

  value = MapAttrValue (DummyAttribute, val);
  if (value < 0)
    {
      if (strlen (val) > MaxMsgLength - 40)
         val[MaxMsgLength - 40] = EOS;
      sprintf (msgBuffer, "Unknown attribute value \"type = %s\"", val);
      HTMLParseError (context->doc, msgBuffer);
      attrType.AttrSSchema = TtaGetDocumentSSchema (context->doc);
      attrType.AttrTypeNum = pHTMLAttributeMapping[0].ThotAttribute;
      sprintf (msgBuffer, "type=%s", val);
      CreateHTMLAttribute (context->lastElement, attrType, msgBuffer, TRUE,
			   context->doc, &lastAttribute, &lastAttrElement);
    }
  else
    {
      elType = TtaGetElementType (context->lastElement);
      if (elType.ElTypeNum != HTML_EL_Input)
	{
	  if (strlen (val) > MaxMsgLength - 40)
	    val[MaxMsgLength - 40] = EOS;
	  sprintf (msgBuffer, "Duplicate attribute \"type = %s\"", val);
	}
      else
	{
	  elType.ElSSchema = TtaGetDocumentSSchema (context->doc);
	  elType.ElTypeNum = value;
	  newChild = TtaNewTree (context->doc, elType, "");
	  numberOfLinesRead = 0;
	  TtaSetElementLineNumber (newChild, numberOfLinesRead);
	  TtaInsertFirstChild (&newChild, context->lastElement, context->doc);
	  if (value == HTML_EL_PICTURE_UNIT)
	    {
	      /* add the attribute IsInput to input pictures */
	      attrType.AttrSSchema = elType.ElSSchema;
	      attrType.AttrTypeNum = HTML_ATTR_IsInput;
	      attr = TtaNewAttribute (attrType);
	      TtaAttachAttribute (newChild, attr, context->doc);
	    }
	}
    }
}

/*----------------------------------------------------------------------
   XhtmlTypeAttrValue 
   Value val has been read for the HTML attribute TYPE.
   Create a child for the current Thot element INPUT accordingly.
  ----------------------------------------------------------------------*/
void              XhtmlTypeAttrValue (char       *val,
				      Attribute   currentAttribute,
				      Element     lastAttrElement,
				      ParserData *context)

{
  ElementType     elType;
  Element         newChild;
  AttributeType   attrType;
  Attribute       attr;
  char            msgBuffer[MaxMsgLength];
  int             value;
  ThotBool        level;

  attrType.AttrTypeNum = DummyAttribute;
  MapHTMLAttributeValue (val, attrType, &value);
  if (value < 0)
    {
      sprintf (msgBuffer, "Unknown attribute value \"type=%s\"", val);
      XmlParseError (errorParsing, msgBuffer, 0);
      MapHTMLAttribute ("unknown_attr", &attrType, NULL,
			&level, context->doc);
      sprintf (msgBuffer, "type=%s", val);
      CreateHTMLAttribute (context->lastElement, attrType, msgBuffer, TRUE,
			   context->doc, &currentAttribute, &lastAttrElement);
    }
  else
    {
      elType = TtaGetElementType (context->lastElement);
      if (elType.ElTypeNum != HTML_EL_Input)
	{
	  sprintf (msgBuffer, "Duplicate attribute \"type = %s\"", val);
	  XmlParseError (errorParsing, msgBuffer, 0);
	}
      else
	{
	  elType.ElTypeNum = value;
	  newChild = TtaNewTree (context->doc, elType, "");
	  XmlSetElemLineNumber (newChild);
	  TtaInsertFirstChild (&newChild, context->lastElement, context->doc);
	  if (value == HTML_EL_PICTURE_UNIT)
	    {
	      /* add the attribute IsInput to input pictures */
	      attrType.AttrSSchema = elType.ElSSchema;
	      attrType.AttrTypeNum = HTML_ATTR_IsInput;
	      attr = TtaNewAttribute (attrType);
	      TtaAttachAttribute (newChild, attr, context->doc);
	    }
	}
    }
}

/*----------------------------------------------------------------------
   CreateAttrWidthPercentPxl
   an HTML attribute "width" has been created for a Table of a HR.
   Create the corresponding attribute IntWidthPercent or IntWidthPxl.
   oldWidth is -1 or the old image width.
  ----------------------------------------------------------------------*/
void CreateAttrWidthPercentPxl (char *buffer, Element el,
				Document doc, int oldWidth)
{
  AttributeType   attrTypePxl, attrTypePercent;
  Attribute       attrOld, attrNew;
  int             length, val;
  char            msgBuffer[MaxMsgLength];
  ElementType	  elType;
  int             w, h;
  ThotBool        isImage;

  elType = TtaGetElementType (el);
  isImage = (elType.ElTypeNum == HTML_EL_PICTURE_UNIT ||
	     elType.ElTypeNum == HTML_EL_Data_cell ||
	     elType.ElTypeNum == HTML_EL_Heading_cell);

  /* remove trailing spaces */
  length = strlen (buffer) - 1;
  while (length > 0 && buffer[length] <= SPACE)
    length--;
  attrTypePxl.AttrSSchema = TtaGetDocumentSSchema (doc);
  attrTypePercent.AttrSSchema = TtaGetDocumentSSchema (doc);
  attrTypePxl.AttrTypeNum = HTML_ATTR_IntWidthPxl;
  attrTypePercent.AttrTypeNum = HTML_ATTR_IntWidthPercent;
  /* is the last character a '%' ? */
  if (buffer[length] == '%')
    {
      /* remove IntWidthPxl */
      attrOld = TtaGetAttribute (el, attrTypePxl);
      /* update IntWidthPercent */
      attrNew = TtaGetAttribute (el, attrTypePercent);
      if (attrNew == NULL)
	{
	  attrNew = TtaNewAttribute (attrTypePercent);
	  TtaAttachAttribute (el, attrNew, doc);
	}
      else if (isImage && oldWidth == -1)
	{
	  if (attrOld == NULL)
	    oldWidth = TtaGetAttributeValue (attrNew);
	  else
	    oldWidth = TtaGetAttributeValue (attrOld);
	}
    }
  else
    {
      /* remove IntWidthPercent */
      attrOld = TtaGetAttribute (el, attrTypePercent);
      /* update IntWidthPxl */
      attrNew = TtaGetAttribute (el, attrTypePxl);
      if (attrNew == NULL)
	{
	  attrNew = TtaNewAttribute (attrTypePxl);
	  TtaAttachAttribute (el, attrNew, doc);
	}
      else if (isImage && oldWidth == -1)
	{
	  TtaGiveBoxSize (el, doc, 1, UnPixel, &w, &h);
	  if (attrOld == NULL)
	    oldWidth = w * TtaGetAttributeValue (attrNew) / 100;
	  else
	    oldWidth = w * TtaGetAttributeValue (attrOld) / 100;	  
	}
    }

  if (attrOld != NULL)
    TtaRemoveAttribute (el, attrOld, doc);
  if (sscanf (buffer, "%d", &val))
    TtaSetAttributeValue (attrNew, val, el, doc);
  else
    /* its not a number. Delete attribute and send an error message */
    {
    TtaRemoveAttribute (el, attrNew, doc);
    if (strlen (buffer) > MaxMsgLength - 30)
        buffer[MaxMsgLength - 30] = EOS;
    sprintf (msgBuffer, "Invalid attribute value \"%s\"", buffer);
    HTMLParseError (doc, msgBuffer);
    }
  if (isImage)
    UpdateImageMap (el, doc, oldWidth, -1);
}

/*----------------------------------------------------------------------
   CreateAttrHeightPercentPxl
   an HTML attribute "width" has been created for a Table of a HR.
   Create the corresponding attribute IntHeightPercent or IntHeightPxl.
   oldHeight is -1 or the old image width.
  ----------------------------------------------------------------------*/
void CreateAttrHeightPercentPxl (char *buffer, Element el,
				Document doc, int oldHeight)
{
  AttributeType   attrTypePxl, attrTypePercent;
  Attribute       attrOld, attrNew;
  int             length, val;
  char            msgBuffer[MaxMsgLength];
  ElementType	  elType;
  int             w, h;
  ThotBool        isImage;

  elType = TtaGetElementType (el);
  isImage = (elType.ElTypeNum == HTML_EL_PICTURE_UNIT ||
	     elType.ElTypeNum == HTML_EL_Data_cell ||
	     elType.ElTypeNum == HTML_EL_Heading_cell);

  /* remove trailing spaces */
  length = strlen (buffer) - 1;
  while (length > 0 && buffer[length] <= SPACE)
    length--;
  attrTypePxl.AttrSSchema = TtaGetDocumentSSchema (doc);
  attrTypePercent.AttrSSchema = TtaGetDocumentSSchema (doc);
  attrTypePxl.AttrTypeNum = HTML_ATTR_IntHeightPxl;
  attrTypePercent.AttrTypeNum = HTML_ATTR_IntHeightPercent;
  /* is the last character a '%' ? */
  if (buffer[length] == '%')
    {
      /* remove IntHeightPxl */
      attrOld = TtaGetAttribute (el, attrTypePxl);
      /* update IntHeightPercent */
      attrNew = TtaGetAttribute (el, attrTypePercent);
      if (attrNew == NULL)
	{
	  attrNew = TtaNewAttribute (attrTypePercent);
	  TtaAttachAttribute (el, attrNew, doc);
	}
      else if (isImage && oldHeight == -1)
	{
	  if (attrOld == NULL)
	    oldHeight = TtaGetAttributeValue (attrNew);
	  else
	    oldHeight = TtaGetAttributeValue (attrOld);
	}
    }
  else
    {
      /* remove IntHeightPercent */
      attrOld = TtaGetAttribute (el, attrTypePercent);
      /* update IntHeightPxl */
      attrNew = TtaGetAttribute (el, attrTypePxl);
      if (attrNew == NULL)
	{
	  attrNew = TtaNewAttribute (attrTypePxl);
	  TtaAttachAttribute (el, attrNew, doc);
	}
      else if (isImage && oldHeight == -1)
	{
	  TtaGiveBoxSize (el, doc, 1, UnPixel, &w, &h);
	  if (attrOld == NULL)
	    oldHeight = w * TtaGetAttributeValue (attrNew) / 100;
	  else
	    oldHeight = w * TtaGetAttributeValue (attrOld) / 100;	  
	}
    }

  if (attrOld != NULL)
    TtaRemoveAttribute (el, attrOld, doc);
  if (sscanf (buffer, "%d", &val))
    TtaSetAttributeValue (attrNew, val, el, doc);
  else
    /* its not a number. Delete attribute and send an error message */
    {
    TtaRemoveAttribute (el, attrNew, doc);
    if (strlen (buffer) > MaxMsgLength - 30)
        buffer[MaxMsgLength - 30] = EOS;
    sprintf (msgBuffer, "Invalid attribute value \"%s\"", buffer);
    HTMLParseError (doc, msgBuffer);
    }
  if (isImage)
    UpdateImageMap (el, doc, oldHeight, -1);
}

/*----------------------------------------------------------------------
   CreateAttrIntSize
   an HTML attribute "size" has been created for a Font element.
   Create the corresponding internal attribute.
  ----------------------------------------------------------------------*/
void              CreateAttrIntSize (char *buffer, Element el, Document doc)

{
   AttributeType  attrType;
   int            val, ind, factor, delta;
   Attribute      attr;
   char         msgBuffer[MaxMsgLength];

   /* is the first character a '+' or a '-' ? */
   ind = 0;
   factor = 1;
   delta = 0;
   if (buffer[0] == '+')
     {
       attrType.AttrTypeNum = HTML_ATTR_IntSizeIncr;
       ind++;
       factor = 2;
     }
   else if (buffer[0] == '-')
     {
       attrType.AttrTypeNum = HTML_ATTR_IntSizeDecr;
       ind++;
       factor = 2;
     }
   else
     {
       attrType.AttrTypeNum = HTML_ATTR_IntSizeRel;
       delta = 1;
     }
   attrType.AttrSSchema = TtaGetDocumentSSchema (doc);
   attr = TtaGetAttribute (el, attrType);
   if (sscanf (&buffer[ind], "%d", &val))
     {
       val = val * factor + delta;
       if (attr == NULL)
	 {
	   /* this attribute doesn't exist, create it */
	   attr = TtaNewAttribute (attrType);
	   TtaAttachAttribute (el, attr, doc);
	 }
       TtaSetAttributeValue (attr, val, el, doc);
     }
   else
     /* its not a number. Delete attribute and send an error message */
     {
       if (attr)
         TtaRemoveAttribute (el, attr, doc);
       if (strlen (buffer) > MaxMsgLength - 30)
         buffer[MaxMsgLength - 30] = EOS;
       sprintf (msgBuffer, "Invalid attribute value \"%s\"", buffer);
       HTMLParseError (doc, msgBuffer);
     }
}
/*----------------------------------------------------------------------
   EndOfHTMLAttributeValue
   Filling of an HTML attribute value
  ----------------------------------------------------------------------*/
void EndOfHTMLAttributeValue (char *attrValue,
			      AttributeMapping *lastMappedAttr,
			      Attribute currentAttribute,
			      Element lastAttrElement,
			      ThotBool UnknownAttr,
			      ParserData *context,
			      ThotBool isXML)
{
  AttributeType   attrType, attrType1;
  Attribute       attr;
  ElementType	   elType;
  Element         child, root;
  Language        lang;
  char            translation;
  char            shape;
  char           *buffer;
  char           *attrName;
  char            msgBuffer[MaxMsgLength];
  int             val;
  int             length;
  int             attrKind;
  ThotBool        done = FALSE;

  /* treatments of some particular HTML attributes */
  if (!strcmp (lastMappedAttr->XMLattribute, "style"))
    {
      TtaSetAttributeText (currentAttribute, attrValue,
			   lastAttrElement, context->doc);
      ParseHTMLSpecificStyle (context->lastElement, attrValue,
			      context->doc, 100, FALSE);
      done = TRUE;
    }
  else
    {
      if (!strcmp (lastMappedAttr->XMLattribute, "link"))
	HTMLSetAlinkColor (context->doc, attrValue);
      else if (!strcmp (lastMappedAttr->XMLattribute, "alink"))
	HTMLSetAactiveColor (context->doc, attrValue);
      else if (!strcmp (lastMappedAttr->XMLattribute, "vlink"))
	HTMLSetAvisitedColor (context->doc, attrValue);
    }

  if (!done)
    {
      val = 0;
      translation = lastMappedAttr->AttrOrContent;
      switch (translation)
	{
	case 'C':	/* Content */
	  child = PutInContent (attrValue, context);
	  if (child != NULL)
	    TtaAppendTextContent (child, "\" ", context->doc);
	  break;
	  
	case 'A':
	  if (currentAttribute != NULL)
	    {
	      TtaGiveAttributeType (currentAttribute, &attrType, &attrKind);
	      switch (attrKind)
		{
		case 0:	/* enumerate */
		  if (isXML)
		    MapHTMLAttributeValue (attrValue, attrType, &val);
		  else
		    val = MapAttrValue (lastMappedAttr->ThotAttribute,
					attrValue);
		  if (val < 0)
		    {
		      TtaGiveAttributeType (currentAttribute,
					    &attrType, &attrKind);
		      attrName = TtaGetAttributeName (attrType);
		      sprintf (msgBuffer,
			       "Invalid attribute value \"%s = %s\"",
			       attrName, attrValue);
		      if (isXML)
			XmlParseError (errorParsing, msgBuffer, 0);
		      else
			{
			  HTMLParseError (context->doc, msgBuffer);
		      /* remove the attribute and replace it by an */
		      /* Invalid_attribute (not for XHTML) */
			  TtaRemoveAttribute (lastAttrElement,
					      currentAttribute, context->doc);
			  attrType.AttrSSchema = 
			    TtaGetDocumentSSchema (context->doc);
			  attrType.AttrTypeNum =
			    pHTMLAttributeMapping[0].ThotAttribute;
			  sprintf (msgBuffer, "%s=%s", attrName, attrValue);
			  CreateHTMLAttribute (lastAttrElement, attrType,
					       msgBuffer, TRUE, context->doc,
					       &currentAttribute,
					       &lastAttrElement);
			}
		    }
		  else
		    TtaSetAttributeValue (currentAttribute, val,
					  lastAttrElement, context->doc);
		  break;
		case 1:	/* integer */
		  if (attrType.AttrTypeNum == HTML_ATTR_Border &&
		      !strcasecmp (attrValue, "border"))
		    {
		      /* border="border" for a table */
		      val = 1;
		      TtaSetAttributeValue (currentAttribute, val,
					    lastAttrElement, context->doc);
		    }
		  else if (sscanf (attrValue, "%d", &val))
		    TtaSetAttributeValue (currentAttribute, val,
					  lastAttrElement, context->doc);
		  else
		    {
		      TtaRemoveAttribute (lastAttrElement, currentAttribute,
					  context->doc);
		      sprintf (msgBuffer,
			       "Unknown attribute value \"%s\"",
			       attrValue);
		      if (isXML)
			XmlParseError (errorParsing, msgBuffer, 0);
		      else
			HTMLParseError (context->doc, msgBuffer);
		    }
		  break;
		case 2:	/* text */
		  if (!UnknownAttr)
		    {
		      TtaSetAttributeText (currentAttribute, attrValue,
					   lastAttrElement, context->doc);
		      if (attrType.AttrTypeNum == HTML_ATTR_Language)
			{
			  /* it's a LANG attribute value */
			  lang = TtaGetLanguageIdFromName (attrValue);
			  if (lang == 0)
			    {
			      sprintf (msgBuffer,
				       "warning - unsupported language: %s",
				       attrValue);
			      if (isXML)
				XmlParseError (errorParsing, msgBuffer, 0);
			      else
				HTMLParseError (context->doc, msgBuffer);
			    }
			  else
			    {
			      /* change current language */
			      context->language = lang;
			      if (isXML)
				SetLanguagInXmlStack (lang);
			      else
				SetLanguagInHTMLStack (lang);
			    }
			  root = TtaGetRootElement (context->doc);
			  if (lastAttrElement == root)
			    /* it's a LANG attribute on the root element */
			    /* set the RealLang attribute */
			    {
			      attrType1.AttrSSchema = TtaGetDocumentSSchema (context->doc);
			      attrType1.AttrTypeNum = HTML_ATTR_RealLang;
			      /* this attribute could be already present,
				 (lang and xml:lang attributes) */
			      if (!TtaGetAttribute (lastAttrElement,
						    attrType1))
				/* it's not present. Add it */
				{
				  attr = TtaNewAttribute (attrType1);
				  TtaAttachAttribute (lastAttrElement,
						      attr, context->doc);
				  TtaSetAttributeValue (attr,
							HTML_ATTR_RealLang_VAL_Yes_,
							lastAttrElement,
							context->doc);
				}
			    }
			}
		      else if (attrType.AttrTypeNum == HTML_ATTR_accesskey)
			TtaAddAccessKey (context->doc, (unsigned int)attrValue[0],
					 lastAttrElement);
		    }
		  else
		    {
		      /* this is the content of an invalid attribute */
		      /* append it to the current Invalid_attribute */
		      if (!isXML)
			{
			  length = strlen (attrValue) + 2;
			  length += TtaGetTextAttributeLength (currentAttribute);
			  buffer = TtaGetMemory (length + 1);
			  TtaGiveTextAttributeValue (currentAttribute,
						     buffer, &length);
			  strcat (buffer, "=");
			  strcat (buffer, attrValue);
			  TtaSetAttributeText (currentAttribute, buffer,
					       lastAttrElement, context->doc);
			  TtaFreeMemory (buffer);
			}
		    }
		  break;
		case 3:	/* reference */
		  break;
		}
	    }
	  break;
	  
	case SPACE:
	  if (isXML)
	    XhtmlTypeAttrValue (attrValue, currentAttribute,
				lastAttrElement, context);
	  else
	    HTMLTypeAttrValue (attrValue, currentAttribute,
			       lastAttrElement, context);
	  break;
	  
	 default:
	   break;
	}

      if (lastMappedAttr->ThotAttribute == HTML_ATTR_Width__)
	/* HTML attribute "width" for a table or a hr */
	/* create the corresponding attribute IntWidthPercent or */
	/* IntWidthPxl */
	CreateAttrWidthPercentPxl (attrValue, lastAttrElement, context->doc, -1);
      else if (lastMappedAttr->ThotAttribute == HTML_ATTR_Height_)
	/* HTML attribute "width" for a table or a hr */
	/* create the corresponding attribute IntHeightPercent or */
	/* IntHeightPxl */
	CreateAttrHeightPercentPxl (attrValue, lastAttrElement, context->doc, -1);
      else if (!strcmp (lastMappedAttr->XMLattribute, "size"))
	{
	  TtaGiveAttributeType (currentAttribute, &attrType, &attrKind);
	  if (attrType.AttrTypeNum == HTML_ATTR_Font_size)
	    CreateAttrIntSize (attrValue, lastAttrElement, context->doc);
	}
      else if (!strcmp (lastMappedAttr->XMLattribute, "shape"))
	{
	  child = TtaGetFirstChild (lastAttrElement);
	  if (child != NULL)
	    {
	      switch (val)
		{
		case HTML_ATTR_shape_VAL_rectangle:
		  shape = 'R';
		  break;
		case HTML_ATTR_shape_VAL_circle:
		  shape = 'a';
		  break;
		case HTML_ATTR_shape_VAL_polygon:
		  shape = 'p';
		  break;
		default:
		  shape = SPACE;
		  break;
		}
	      TtaSetGraphicsShape (child, shape, context->doc);
	    }
	}
      else if (!strcmp (lastMappedAttr->XMLattribute, "value"))
	{
	  elType = TtaGetElementType (lastAttrElement);
	  if (elType.ElTypeNum == HTML_EL_Text_Input ||
	      elType.ElTypeNum == HTML_EL_Password_Input ||
	      elType.ElTypeNum == HTML_EL_File_Input ||
	      elType.ElTypeNum == HTML_EL_Input)
	    /* create a Default_Value attribute with the same content */
	    {
	      attrType1.AttrSSchema = attrType.AttrSSchema;
	      attrType1.AttrTypeNum = HTML_ATTR_Default_Value;
	      attr = TtaNewAttribute (attrType1);
	      TtaAttachAttribute (lastAttrElement, attr, context->doc);
	      TtaSetAttributeText (attr, attrValue,
				   lastAttrElement, context->doc);
	    }
	}
      else
	{
	  /* Some HTML attributes are equivalent to a CSS property:      */
	  /*      background     ->                   background         */
	  /*      bgcolor        ->                   background         */
	  /*      text           ->                   color              */
	  /*      color          ->                   color              */
	  if (!strcmp (lastMappedAttr->XMLattribute, "background"))
	    {
	      if (strlen (attrValue) > MaxMsgLength - 30)
		attrValue[MaxMsgLength - 30] = EOS;
	      HTMLSetBackgroundImage (context->doc,
				      context->lastElement, STYLE_REPEAT, attrValue);
	    }
	  else if (!strcmp (lastMappedAttr->XMLattribute, "bgcolor"))
	    HTMLSetBackgroundColor (context->doc,
				    context->lastElement, attrValue);
	  else if (!strcmp (lastMappedAttr->XMLattribute, "text") ||
		   !strcmp (lastMappedAttr->XMLattribute, "color"))
	    HTMLSetForegroundColor (context->doc,
				    context->lastElement, attrValue);
	}
    }
}

/*----------------------------------------------------------------------
   MapHTMLAttributeValue
   Search in the Attribute Value Mapping Table the entry for the attribute
   ThotAtt and its value attVal. Returns the corresponding Thot value.
  ----------------------------------------------------------------------*/
void MapHTMLAttributeValue (char *attVal, AttributeType attrType, int *value)
{
  MapXMLAttributeValue (XHTML_TYPE, attVal, attrType, value);
}
