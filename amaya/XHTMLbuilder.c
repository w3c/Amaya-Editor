/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2008
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
#include "EDITstyle_f.h"
#include "fetchXMLname_f.h"
#include "fetchHTMLname_f.h"
#include "html2thot_f.h"
#include "HTMLactions_f.h"
#include "HTMLedit_f.h"
#include "HTMLform_f.h"
#include "HTMLimage_f.h"
#include "HTMLtable_f.h"
#include "HTMLimage_f.h"
#include "init_f.h"
#include "UIcss_f.h"
#include "styleparser_f.h"
#include "XHTMLbuilder_f.h"
#include "Xml2thot_f.h"
#ifdef TEMPLATES
#include "Template.h"
#endif /* TEMPLATES */
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
    HTML_EL_Data_cell, HTML_EL_Heading_cell, HTML_EL_INS, HTML_EL_DEL,
    0};

/* Define a pointer to let parser functions access the HTML entity table */
extern XmlEntity *pXhtmlEntityTable;

/* maximum size of error messages */
#define MaxMsgLength 200

/*----------------------------------------------------------------------
  ParseCharsetAndContentType:
  Parses the element HTTP-EQUIV and looks for the charset value.
  ----------------------------------------------------------------------*/
void ParseCharsetAndContentType (Element el, Document doc) 

{
  AttributeType attrType;
  Attribute     attr;
  ElementType   elType;
  CHARSET       charset;
  char         *text, *text2, *ptrText, *str;
  char          charsetname[MAX_LENGTH];
  int           length;
  int           pos, index = 0;

  charset = TtaGetDocumentCharset (doc);
  if (charset != UNDEFINED_CHARSET &&
      DocumentMeta[doc] && DocumentMeta[doc]->content_type)
    return;

  elType = TtaGetElementType (el);
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = HTML_ATTR_http_equiv;
  attr = TtaGetAttribute (el, attrType);
  if (attr != NULL)
    {
      /* There is a HTTP-EQUIV attribute */
      length = TtaGetTextAttributeLength (attr);
      if (length > 0)
        {
          text = (char *)TtaGetMemory (length + 1);
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
                      text2 = (char *)TtaGetMemory (length + 1);
                      TtaGiveTextAttributeValue (attr, text2, &length);
                      ptrText = text2;
                      while (*ptrText)
                        {
                          *ptrText = tolower (*ptrText);
                          ptrText++;
                        }

                      if (!DocumentMeta[doc])
                        DocumentMeta[doc] = DocumentMetaDataAlloc ();
                      if (DocumentMeta[doc]->content_type == NULL)
                        {
			   
                          if (!strncmp (text2, "text/html", 9))
                            DocumentMeta[doc]->content_type = TtaStrdup ("text/html");
                          else
                            DocumentMeta[doc]->content_type = TtaStrdup (AM_XHTML_MIME_TYPE);
                        }

                      if (charset == UNDEFINED_CHARSET)
                        {
                          /* the charset is not already defined by the http header */
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
                                TtaSetDocumentCharset (doc, charset, FALSE);
                            }
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
ThotBool XhtmlCannotContainText (ElementType elType)
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
  CheckNamespace
  If attribute attrNum is not present on element el, generate a
  parsing error message.
  ----------------------------------------------------------------------*/
static void CheckNamespace (Element el, Document doc)
{
  char           msgBuffer[MaxMsgLength];
  int            lineNum;

  if (DocumentMeta[doc] && DocumentMeta[doc]->xmlformat &&
      TtaGiveNamespaceDeclaration (doc, el) == NULL)
    {
      sprintf (msgBuffer, "Mandatory namespace for %s will be added when saving",
               TtaGetElementTypeName(TtaGetElementType(el)));
      lineNum = TtaGetElementLineNumber(el);
      XmlParseError (warningMessage, (unsigned char*)msgBuffer, lineNum);
      TtaSetANamespaceDeclaration (doc, el, NULL, XHTML_URI);
    }
}

/*----------------------------------------------------------------------
  CheckMandatoryAttribute
  If attribute attrNum is not present on element el, generate a
  parsing error message.
  ----------------------------------------------------------------------*/
void CheckMandatoryAttribute (Element el, Document doc, int attrNum)
{
  ElementType    elType, childType;
  Element        child;
  Attribute      attr;
  AttributeType  attrType;
  int            lineNum;
  const char    *name;
  char           msgBuffer[MaxMsgLength];

  elType = TtaGetElementType (el);
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = attrNum;
  attr = TtaGetAttribute (el, attrType);
  if (attr == NULL)
    {
#ifdef TEMPLATES
      child = TtaGetFirstChild (el);
      if (child)
        {
          childType = TtaGetElementType (child);
          if (childType.ElTypeNum == Template_EL_attribute &&
              !strcmp(TtaGetSSchemaName(childType.ElSSchema) , "Template"))
            return;
        }
#endif /* TEMPLATES */
      name = GetXMLAttributeName (attrType, elType, doc);
      if (name)
        {
          sprintf (msgBuffer, "Missing mandatory attribute %s for element %s",
                   name, TtaGetElementTypeName(elType));
          lineNum = TtaGetElementLineNumber(el);
          if (DocumentMeta[doc] && DocumentMeta[doc]->xmlformat)
            XmlParseError (errorParsing, (unsigned char *)msgBuffer, lineNum);
          else
            HTMLParseError (doc, msgBuffer, lineNum);
        }
    }
}


/*----------------------------------------------------------------------
  AddRowsColumns
  Add default rows and columns attributes to a TEXTAREA element.
  ----------------------------------------------------------------------*/
void AddRowsColumns (Element el, Document doc)
{
  ElementType    elType;
  Attribute      attr;
  AttributeType  attrType;

  /* Add defaults rows and columns to display the textarea */
  elType = TtaGetElementType (el);
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = HTML_ATTR_Rows;
  attr = TtaGetAttribute (el, attrType);
  if (attr == NULL)
    {
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (el, attr, doc);
      TtaSetAttributeValue (attr, 4, el, doc);
    }
  attrType.AttrTypeNum = HTML_ATTR_Columns;
  attr = TtaGetAttribute (el, attrType);
  if (attr == NULL)
    {
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (el, attr, doc);
      TtaSetAttributeValue (attr, 20, el, doc);
    }
}

/*----------------------------------------------------------------------
  XhtmlElementComplete
  Complete Xhtml elements.
  Check its attributes and its contents.
  ----------------------------------------------------------------------*/
void XhtmlElementComplete (ParserData *context, Element el, int *error)
{
  Document       doc;   
  ElementType    elType, newElType, childType, parentType;
  Element        child, desc, leaf, prev, next, last;
  Element        elFrames, lastFrame, lastChild, parent, picture, content, legend;
  Attribute      attr;
  AttributeType  attrType;
  SSchema        htmlSchema;
  Language       lang;
  char           *text;
  char           lastChar[2];
  char           *name1, *data;
  char           msgBuffer[MaxMsgLength];
  int            typenum, length;
  ThotBool       isImage, isInline, clean;

  *error = 0;
  doc = context->doc;
  elType = TtaGetElementType (el);
  htmlSchema = elType.ElSSchema;
  childType.ElSSchema = NULL;
  isInline = IsXMLElementInline (elType, doc);
  newElType.ElSSchema = elType.ElSSchema;
  if (!IsCharacterLevelType (elType) &&
      elType.ElTypeNum != HTML_EL_IFRAME &&
      elType.ElTypeNum != HTML_EL_map &&
      elType.ElTypeNum != HTML_EL_MAP &&
      elType.ElTypeNum != HTML_EL_SCRIPT_ &&
      elType.ElTypeNum != HTML_EL_Comment_ &&
      elType.ElTypeNum != HTML_EL_XMLPI)
    {
      // check the validity of the XHTML structure
      parent = TtaGetParent (el);
      parentType = TtaGetElementType (parent);
      if (parentType.ElSSchema == elType.ElSSchema &&
          (parentType.ElTypeNum == HTML_EL_Paragraph ||
           parentType.ElTypeNum == HTML_EL_H1 ||
           parentType.ElTypeNum == HTML_EL_H2 ||
           parentType.ElTypeNum == HTML_EL_H3 ||
           parentType.ElTypeNum == HTML_EL_H4 ||
           parentType.ElTypeNum == HTML_EL_H5 ||
           parentType.ElTypeNum == HTML_EL_H6 ||
           parentType.ElTypeNum == HTML_EL_Preformatted))
        {
          name1 = TtaStrdup ( TtaGetElementTypeName(elType));
          if (name1)
            {
              sprintf (msgBuffer, "A <%s> element cannot be a child of <%s> element",
                       name1, TtaGetElementTypeName(parentType));
              XmlParseError (errorParsing,  (unsigned char *)msgBuffer,
                             TtaGetElementLineNumber(el));
              TtaFreeMemory (name1);
            }
          name1 = NULL;
        }
    }
  if (elType.ElTypeNum == HTML_EL_ins ||
      elType.ElTypeNum == HTML_EL_del)
    {
      child = TtaGetFirstChild (el);
      if (IsBlockElement (child))
        {
          // change the element type
          if (elType.ElTypeNum == HTML_EL_ins)
            TtaChangeTypeOfElement (el, doc, HTML_EL_INS);
          else
            TtaChangeTypeOfElement (el, doc, HTML_EL_DEL);
          isInline = FALSE;
        }
    }
  if (elType.ElTypeNum == HTML_EL_Paragraph ||
      elType.ElTypeNum == HTML_EL_Address ||
      elType.ElTypeNum == HTML_EL_H1 ||
      elType.ElTypeNum == HTML_EL_H2 ||
      elType.ElTypeNum == HTML_EL_H3 ||
      elType.ElTypeNum == HTML_EL_H4 ||
      elType.ElTypeNum == HTML_EL_H5 ||
      elType.ElTypeNum == HTML_EL_H6 ||
      elType.ElTypeNum == HTML_EL_Preformatted ||
      elType.ElTypeNum == HTML_EL_Term ||
      elType.ElTypeNum == HTML_EL_LEGEND ||
      elType.ElTypeNum == HTML_EL_CAPTION ||
      elType.ElTypeNum == HTML_EL_rb ||
      elType.ElTypeNum == HTML_EL_rt ||
      (isInline &&  !TtaIsLeaf (elType) &&
       elType.ElTypeNum != HTML_EL_Text_Area))
    /* It's an element that is supposed to contain at least a Basic_Elem.
       If it is empty, insert a Basic_Elem to allow the user to put the
       selection within this element */
    /* Don't do it for a Text_Area, as an Inserted_Text element has to be
       created (see below) */
    {
      child = TtaGetFirstChild (el);
      if (child == NULL)
        /* it's an empty inline element */
        /* insert a Basic_Elem element in the element */
        {
          newElType.ElTypeNum = HTML_EL_Basic_Elem;
          child = TtaNewTree (doc, newElType, "");
          TtaInsertFirstChild (&child, el, doc);
        }
    }
  if (!isInline)
    /* It's a block-level element. Is it within a character-level element? */
    if (elType.ElTypeNum != HTML_EL_Comment_ &&
        elType.ElTypeNum != HTML_EL_ASP_element &&
        elType.ElTypeNum != HTML_EL_XMLPI &&
        elType.ElTypeNum != HTML_EL_SCRIPT_)
      BlockInCharLevelElem (el);

  typenum = elType.ElTypeNum;
  switch (typenum)
    {
    case HTML_EL_HTML:
      CheckNamespace (el, doc);
      break;

    case HTML_EL_PICTURE_UNIT:
      /* Check the mandatory SRC attribute */
      CheckMandatoryAttribute (el, doc, HTML_ATTR_SRC);
      break;

    case HTML_EL_Object:	/* it's an object */
      data = NULL;
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
              name1 = (char *)TtaGetMemory (length + 1);
              TtaGiveTextAttributeValue (attr, name1, &length);
              if (!strcmp (name1, AM_MATHML_MIME_TYPE) ||
                  !strcmp (name1, "application/postscript") ||
                  !strcmp (name1, "image/x-bitmap") ||
                  !strcmp (name1, "image/x-xpixmap") ||
                  !strcmp (name1, "image/gif") ||
                  !strcmp (name1, "image/jpeg") ||
                  !strcmp (name1, "image/png") ||
                  !strcmp (name1, "image/svg") ||
                  !strcmp (name1, AM_SVG_MIME_TYPE) ||
                  !strcmp (name1, AM_XHTML_MIME_TYPE) ||
                  !strcmp (name1, "text/html") ||
                  !strcmp (name1, "text/htm") ||
                  !strcmp (name1, AM_GENERIC_XML_MIME_TYPE))
                isImage = TRUE;
              TtaFreeMemory (name1);
            }
        }

      attrType.AttrTypeNum = HTML_ATTR_data;
      attr = TtaGetAttribute (el, attrType);
      if (attr)
        /* the object has a data attribute */
        {
          length = TtaGetTextAttributeLength (attr);
          if (length > 0)
            {
              data = (char *)TtaGetMemory (length + 1);
              TtaGiveTextAttributeValue (attr, data, &length);
              if (!isImage && length >= 5)
                if (!strcmp (&data[length-4], ".mml") ||
                    !strcmp (&data[length-4], ".gif") ||
                    !strcmp (&data[length-4], ".jpg") ||
                    !strcmp (&data[length-5], ".jpeg") ||
                    !strcmp (&data[length-4], ".png") ||
                    !strcmp (&data[length-4], ".svg") ||
                    !strcmp (&data[length-5], ".svgz") ||
                    !strcmp (&data[length-4], ".htm") ||
                    !strcmp (&data[length-5], ".html") ||
                    !strcmp (&data[length-4], ".xml"))
                  isImage = TRUE;
            }
        }
      else
        {
          attrType.AttrTypeNum = HTML_ATTR_classid;
          attr = TtaGetAttribute (el, attrType);
          if (attr)
            /* the object has a data attribute */
            {
              length = TtaGetTextAttributeLength (attr);
              if (length > 0)
                {
                  data = (char *)TtaGetMemory (length + 1);
                  TtaGiveTextAttributeValue (attr, data, &length);
                  if (!isImage && length >= 5)
                    if (!strcmp (&data[length-4], ".mml") ||
                        !strcmp (&data[length-4], ".gif") ||
                        !strcmp (&data[length-4], ".jpg") ||
                        !strcmp (&data[length-5], ".jpeg") ||
                        !strcmp (&data[length-4], ".png") ||
                        !strcmp (&data[length-4], ".svg") ||
                        !strcmp (&data[length-5], ".svgz") ||
                        !strcmp (&data[length-4], ".htm") ||
                        !strcmp (&data[length-5], ".html") ||
                        !strcmp (&data[length-4], ".xml"))
                      isImage = TRUE;
                }
            }
        }
      picture = NULL;     /* no PICTURE element yet */
      child = TtaGetFirstChild (el);
      if (isImage)
        {
          /* the object represents an image. We need a PICTURE element as
             child of the object to hold the image */
          elType.ElTypeNum = HTML_EL_PICTURE_UNIT;
          picture = TtaNewTree (doc, elType, "");
          if (child)
            TtaInsertSibling (picture, child, TRUE, doc);
          else
            TtaInsertFirstChild (&picture, el, doc);
          /* copy attribute data of the object into the SRC attribute of
             the PICTURE element */
          if (data)
            /* the object has a data attribute */
            {
              // remove extra spaces
              clean = FALSE;
              while (length > 0 && data[length-1] == SPACE)
                {
                  data[length-1] = EOS;
                  length--;
                  clean = TRUE;
                }
              if (clean)
                TtaSetAttributeText (attr, data, el, doc);
              // copy the attribute in the picture element
              attrType.AttrTypeNum = HTML_ATTR_SRC;
              attr = TtaGetAttribute (picture, attrType);
              if (attr == NULL)
                {
                  attr = TtaNewAttribute (attrType);
                  TtaAttachAttribute (picture, attr, doc);

                }
              TtaSetAttributeText (attr, data, picture, doc);
              }
          attrType.AttrTypeNum = HTML_ATTR_Height_;
          attr = TtaGetAttribute (el, attrType);
          if (attr)
            /* the Object has a height attribute. Applies it to the
               picture element */
            {
              length = TtaGetTextAttributeLength (attr);
              if (length > 0)
                {
                  text = (char *)TtaGetMemory (length + 1);
                  TtaGiveTextAttributeValue (attr, text, &length);
                  /* create the corresponding attribute IntHeightPercent or */
                  /* IntHeightPxl */
                  CreateAttrHeightPercentPxl (text, el, doc, -1);
                  TtaFreeMemory (text);
                }
            }
          attrType.AttrTypeNum = HTML_ATTR_Width__;
          attr = TtaGetAttribute (el, attrType);
          if (attr)
            /* the Object has a width attribute. Applies it to the
               picture element */
            {
              length = TtaGetTextAttributeLength (attr);
              if (length > 0)
                {
                  text = (char *)TtaGetMemory (length + 1);
                  TtaGiveTextAttributeValue (attr, text, &length);
                  /* create the corresponding attribute IntWidthPercent or */
                  /* IntWidthPxl */
                  CreateAttrWidthPercentPxl (text, el, doc, -1);
                  TtaFreeMemory (text);
                }
            }
        }

      /* is the Object_Content element already created ? */
      if (child)
        /* the object element has at least 1 child element */
        {
          /* put an attribute NoObjects on the Object element: this attribute
             will be removed when and if the actual object is loaded (see
             module HTMLimage.c */
          attrType.AttrTypeNum = HTML_ATTR_NoObjects;
          attr = TtaGetAttribute (el, attrType);
          if (!attr)
            {
              attr = TtaNewAttribute (attrType);
              TtaSetAttributeValue (attr, 1, el, doc);
              TtaAttachAttribute (el, attr, doc);
            }

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
        }
      TtaFreeMemory (data);
      break;

    case HTML_EL_Image_Input:
    case HTML_EL_IMG:
      /* Check the mandatory ALT attribute */
      CheckMandatoryAttribute (el, doc, HTML_ATTR_ALT);
      /* Check the mandatory SRC attribute */
      CheckMandatoryAttribute (el, doc, HTML_ATTR_SRC);
      /* We need a PICTURE element as child to hold the image */
      picture = NULL;
      for (child = TtaGetFirstChild (el); child && !picture;
           TtaNextSibling (&child))
        {
          childType = TtaGetElementType (child);
          if (childType.ElTypeNum == HTML_EL_PICTURE_UNIT &&
              childType.ElSSchema == elType.ElSSchema)
            // the picture is already created
            picture = child;
        }
      if (picture == NULL)
        {
          childType.ElTypeNum = HTML_EL_PICTURE_UNIT;
          picture = TtaNewTree (doc, childType, "");
          if (child)
            TtaInsertSibling (picture, child, TRUE, doc);
          else
            TtaInsertFirstChild (&picture, el, doc);
        }
      attrType.AttrSSchema = elType.ElSSchema;
      attrType.AttrTypeNum = HTML_ATTR_SRC;
      attr = TtaGetAttribute (el, attrType);
      if (attr)
        /* the img has a src attribute */
        {
          length = TtaGetTextAttributeLength (attr);
          data = (char *)TtaGetMemory (length + 1);
          TtaGiveTextAttributeValue (attr, data, &length);
          if (data)
            {
              // remove extra spaces
              clean = FALSE;
              while (length > 0 && data[length-1] == SPACE)
                {
                  data[length-1] = EOS;
                  length--;
                  clean = TRUE;
                }
              if (clean)
                TtaSetAttributeText (attr, data, el, doc);
              // copy the attribute in the picture element
              attr = TtaGetAttribute (picture, attrType);
              if (attr == NULL)
                {
                  attr = TtaNewAttribute (attrType);
                  TtaAttachAttribute (picture, attr, doc);
                }
              TtaSetAttributeText (attr, data, picture, doc);
              TtaFreeMemory (data);
            }
        }
      if (typenum == HTML_EL_IMG)
        {
          attrType.AttrTypeNum = HTML_ATTR_Height_;
          attr = TtaGetAttribute (el, attrType);
          if (attr)
            /* the img has a height attribute. Applies it to the
               picture element */
            {
              length = TtaGetTextAttributeLength (attr);
              if (length > 0)
                {
                  text = (char *)TtaGetMemory (length + 1);
                  TtaGiveTextAttributeValue (attr, text, &length);
                  /* create the corresponding attribute IntHeightPercent or */
                  /* IntHeightPxl */
                  CreateAttrHeightPercentPxl (text, el, doc, -1);
                  TtaFreeMemory (text);
                }
            }
          attrType.AttrTypeNum = HTML_ATTR_Width__;
          attr = TtaGetAttribute (el, attrType);
          if (attr)
            /* the img has a width attribute. Applies it to the
               picture element */
            {
              length = TtaGetTextAttributeLength (attr);
              if (length > 0)
                {
                  text = (char *)TtaGetMemory (length + 1);
                  TtaGiveTextAttributeValue (attr, text, &length);
                  /* create the corresponding attribute IntWidthPercent or */
                  /* IntWidthPxl */
                  CreateAttrWidthPercentPxl (text, el, doc, -1);
                  TtaFreeMemory (text);
                }
            }
         }
      break;

    case HTML_EL_Parameter:
      /* Check the mandatory name attribute */
      CheckMandatoryAttribute (el, doc, HTML_ATTR_Param_name);
      break;


    case HTML_EL_IFRAME:	  /* it's an iframe */
      child = TtaGetFirstChild (el);
      /* is the Iframe_Content element already created ? */
      if (child)
        /* the iframe element has at least 1 child element */
        {
          content = NULL;
          desc = child;
          elType = TtaGetElementType (desc);
          if (elType.ElTypeNum != HTML_EL_Iframe_Content)
            {
              TtaNextSibling(&desc);
              if (desc)
                elType = TtaGetElementType (desc);
            }
          /* is it the Iframe_Content element ? */
          if (elType.ElTypeNum == HTML_EL_Iframe_Content)
            content = desc;
          else
            {
              /* create an Iframe_Content element */
              elType.ElTypeNum = HTML_EL_Iframe_Content;
              content = TtaNewElement (doc, elType);
              TtaInsertSibling (content, child, TRUE, doc);
              /* move previous existing children into Iframe_Content */
              child = TtaGetLastChild(el);
              while (child != content)
                {
                  TtaRemoveTree (child, doc);
                  TtaInsertFirstChild (&child, content, doc);
                  child = TtaGetLastChild(el);
                }
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
              elType.ElTypeNum == HTML_EL_Comment_||
              elType.ElTypeNum == HTML_EL_ASP_element)
            {
              /* create the Frames element if it does not exist */
              if (elFrames == NULL)
                {
                  newElType.ElSSchema = htmlSchema;
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
       
    case HTML_EL_Form:
      /* Check the mandatory action attribute */
      CheckMandatoryAttribute (el, doc, HTML_ATTR_Script_URL);
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
      /* set default size */
      attrType.AttrSSchema = elType.ElSSchema;
      attrType.AttrTypeNum = HTML_ATTR_IntAreaSize;
      attr = TtaGetAttribute (el, attrType);
      if (!attr)
        CreateAttrIntAreaSize (20, el, doc);
      /* get element Inserted_Text */
      child = TtaGetFirstChild (el);
      if (child != NULL)
        {
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
                          text = (char *)TtaGetMemory (length + 1);
                          TtaGiveTextAttributeValue (attr, text, &length);
                          TtaSetTextContent (leaf, (unsigned char *)text, 
                                             TtaGetDefaultLanguage (), doc);
                          TtaFreeMemory (text);
                        }
                    }
                }
            }
        }
      break;
       
    case HTML_EL_META:
      ParseCharsetAndContentType (el, doc);
      /* Check the mandatory CONTENT attribute */
      CheckMandatoryAttribute (el, doc, HTML_ATTR_meta_content);
      break;

    case HTML_EL_BASE:
      /* Check the mandatory HREF attribute */
      CheckMandatoryAttribute (el, doc, HTML_ATTR_HREF_);
      break;

    case HTML_EL_BaseFont:
      /* Check the mandatory size attribute */
      CheckMandatoryAttribute (el, doc, HTML_ATTR_BaseFontSize);
      break;

    case HTML_EL_BDO:
      /* Check the mandatory DIR attribute */
      CheckMandatoryAttribute (el, doc, HTML_ATTR_dir);
      break;

    case HTML_EL_STYLE_:	/* it's a STYLE element */
    case HTML_EL_SCRIPT_:	/* it's a SCRIPT element */
    case HTML_EL_Preformatted:	/* it's a PRE */
      if (elType.ElTypeNum == HTML_EL_SCRIPT_)
        if (DocumentMeta[doc]->xmlformat)
          SetParsingScript (FALSE);
        else
          SetHtmlParsingScript (FALSE);

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
                  TtaGiveSubString (leaf, (unsigned char *)lastChar, length, 1);
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

      if (elType.ElTypeNum == HTML_EL_STYLE_)
        /* Check the mandatory TYPE attribute */
        CheckMandatoryAttribute (el, doc, HTML_ATTR_Notation);
      else if (elType.ElTypeNum == HTML_EL_SCRIPT_)
        /* Check the mandatory TYPE attribute */
        CheckMandatoryAttribute (el, doc, HTML_ATTR_content_type);

      if (DocumentMeta[doc] && DocumentMeta[doc]->xmlformat)
        {
          if (IsXmlParsingCSS ())
            {
              text = GetStyleContents (el);
              if (text)
                {
                  ReadCSSRules (doc, NULL, text, NULL,
                                TtaGetElementLineNumber (el), FALSE, el);
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
                  ReadCSSRules (doc, NULL, text, NULL,
                                TtaGetElementLineNumber (el), FALSE, el);
                  TtaFreeMemory (text);
                }
              SetHtmlParsingCSS (FALSE);
            }
        }
      /* and continue as if it were a Preformatted or a Script */
      break;
       
    case HTML_EL_Text_Area:	/* it's a Text_Area */
      if (DocumentMeta[doc]->xmlformat)
        SetParsingTextArea (FALSE);
      else
        SetHtmlParsingTextArea (FALSE);
      child = TtaGetFirstChild (el);
      if (child == NULL)
        /* it's an empty Text_Area */
        /* insert a Inserted_Text element and a child Basic_Elem in the
           Text_Area element */
        {
          newElType.ElTypeNum = HTML_EL_Inserted_Text;
          child = TtaNewTree (doc, newElType, "");
          TtaInsertFirstChild (&child, el, doc);
        }
      else
        {
          /* save the text into Default_Value attribute */
          attrType.AttrSSchema = htmlSchema;
          attrType.AttrTypeNum = HTML_ATTR_Default_Value;
          if (TtaGetAttribute (el, attrType) == NULL)
            /* attribute Default_Value is missing */
            {
              desc = TtaGetFirstChild (child);
              if (desc)
                {
                  length = TtaGetTextLength (desc);
                  if (length > 0)
                    {
                      length++;
                      attr = TtaNewAttribute (attrType);
                      TtaAttachAttribute (el, attr, doc);
                      text = (char *)TtaGetMemory (length);
                      TtaGiveTextContent (desc, (unsigned char *)text, &length, &lang);
                      TtaSetAttributeText (attr, text, el, doc);
                      TtaFreeMemory (text);
                    }
                }
            }
        }
      /* Check the mandatory rows attribute */
      CheckMandatoryAttribute (el, doc, HTML_ATTR_Rows);
      /* Check the mandatory columns attribute */
      CheckMandatoryAttribute (el, doc, HTML_ATTR_Columns);
      /* Add default rows and columns attributes */
      AddRowsColumns (el, doc);
      break;

    case HTML_EL_Radio_Input:
    case HTML_EL_Checkbox_Input:
      /* put an attribute Checked if it is missing */
      attrType.AttrSSchema = htmlSchema;
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

    case HTML_EL_OptGroup:
      /* Check the mandatory label attribute */
      CheckMandatoryAttribute (el, doc, HTML_ATTR_label);
      break;

    case HTML_EL_MAP:
    case HTML_EL_map:
      /* Check the mandatory attributes */
      if (DocumentMeta[doc] && DocumentMeta[doc]->xmlformat)
        /* it's XHTML. Check attribute id */
        CheckMandatoryAttribute (el, doc, HTML_ATTR_ID);
      else
        /* it's a HTML document. Check attribute name */
        CheckMandatoryAttribute (el, doc, HTML_ATTR_NAME);
      break;

    case HTML_EL_AREA:
      /* Check the mandatory alt attribute */
      CheckMandatoryAttribute (el, doc, HTML_ATTR_ALT);
      break;
       
    case HTML_EL_LINK:
      CheckCSSLink (el, doc, htmlSchema);
      CheckIconLink (el, doc, htmlSchema);
      break;
       
    case HTML_EL_List_Item:
    case HTML_EL_Definition:
      /* insert a pseudo paragraph into empty list items */
      child = TtaGetFirstChild (el);
      if (child == NULL)
        {
          elType.ElTypeNum = HTML_EL_Pseudo_paragraph;
          child = TtaNewTree (doc, elType, "");
          if (child != NULL)
            TtaInsertFirstChild (&child, el, doc);
        }
      break;

    case HTML_EL_Data_cell:
    case HTML_EL_Heading_cell:
      /* insert an Element into empty table cell */
      child = TtaGetFirstChild (el);
      if (child == NULL)
        {
          elType.ElTypeNum = HTML_EL_Element;
          child = TtaNewElement (doc, elType);
          if (child)
            TtaInsertFirstChild (&child, el, doc);
        }
      /* detect whether we are parsing a whole table or just a cell */
      if (DocumentMeta[doc]->xmlformat)
        {
          if (IsWithinXmlTable ())
            NewCell (el, doc, FALSE, FALSE, FALSE);
        }
      else
        {
          if (IsWithinHtmlTable ())
            NewCell (el, doc, FALSE, FALSE, FALSE);
        }
      break;

    case HTML_EL_Table_:
      CheckTable (el, doc);
      SubWithinTable ();
      break;

    case HTML_EL_COL:
    case HTML_EL_COLGROUP:
      /* create a C_Empty child */
      if (!TtaGetFirstChild (el))
        {
          elType.ElTypeNum = HTML_EL_C_Empty;
          child = TtaNewElement (doc, elType);
          if (child)
            TtaInsertFirstChild (&child, el, doc);
        }
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
            TtaChangeElementType (parent, HTML_EL_complex_ruby);
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
                  TtaChangeElementType (el, HTML_EL_rtc2);
                  prev = NULL;
                }
            }
        }
      while (prev);
      break;

    case HTML_EL_FIELDSET:	  /* it's a fieldset */
      childType.ElTypeNum = 0;
      child = TtaGetFirstChild (el);
      if (!child)
        /* empty fieldset. Create a legend and a Fieldset_Content */
        {
          elType.ElTypeNum = HTML_EL_LEGEND;
          legend = TtaNewTree (doc, elType, "");
          TtaInsertFirstChild (&legend, el, doc);
          elType.ElTypeNum = HTML_EL_Fieldset_Content;
          content = TtaNewTree (doc, elType, "");
          TtaInsertSibling (content, legend, FALSE, doc);
        }
      else
        /* is the legend element already created ? */
        {
          legend = NULL;
          desc = child;
          elType = TtaGetElementType (desc);
          while (desc && elType.ElTypeNum != HTML_EL_LEGEND)
            {
              TtaNextSibling(&desc);
              if (desc)
                elType = TtaGetElementType (desc);
            }
          /* is it the legend element ? */
          if (elType.ElTypeNum == HTML_EL_LEGEND)
            legend = desc;
          else
            {
              /* create a legend element */
              elType.ElTypeNum = HTML_EL_LEGEND;
              legend = TtaNewTree (doc, elType, "");
              TtaInsertFirstChild (&legend, el, doc);
            }
          
          /* is the Fieldset_Content element already created ? */
          content = NULL;
          desc = child;
          elType = TtaGetElementType (desc);
          while (desc && elType.ElTypeNum != HTML_EL_Fieldset_Content)
            {
              TtaNextSibling(&desc);
              if (desc)
                elType = TtaGetElementType (desc);
            }
          /* is it the Fieldset_Content element ? */
          if (elType.ElTypeNum == HTML_EL_Fieldset_Content)
            content = desc;
          else
            {
              /* create a Fieldset_Content element */
              elType.ElTypeNum = HTML_EL_Fieldset_Content;
              content = TtaNewTree (doc, elType, "");
              TtaInsertSibling (content, legend, FALSE, doc);
              desc = TtaGetFirstChild (content);
              /* move previous existing children into the Fieldset_Content */
              child = TtaGetLastChild(el);
              while (child != content)
                {
                  elType = TtaGetElementType (child);
                  TtaRemoveTree (child, doc);
                  TtaInsertFirstChild (&child, content, doc);
                  if (desc)
                    {
                      TtaDeleteTree (desc, doc);
                      desc = NULL;
                    }
                  child = TtaGetLastChild(el);
                }
            }
        }
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
        TtaSetTextContent (el, (unsigned char *)ChrString,
                           context->language, context->doc);
      else
        TtaAppendTextContent (el, (unsigned char *)ChrString, context->doc);
    }
  return el;
}

/*----------------------------------------------------------------------
  UnknownXhtmlNameSpace
  The element doesn't belong to a supported namespace
  ----------------------------------------------------------------------*/
void UnknownXhtmlNameSpace (ParserData *context, Element *unknownEl,
                            char* content)
{
  ElementType     elType;
  Element         elText;

  /* Create a new Invalid_element */
  elType.ElSSchema = GetXMLSSchema (XHTML_TYPE, context->doc);
  elType.ElTypeNum = HTML_EL_XHTML_Unknown_namespace;
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
      TtaSetTextContent (elText, (unsigned char *)content, context->language, context->doc);
      TtaSetAccessRight (elText, ReadOnly, context->doc);
    }
}

/*----------------------------------------------------------------------
  CreateHTMLAttribute
  create an attribute of type attrType for the element el.
  ----------------------------------------------------------------------*/
void CreateHTMLAttribute (Element       el,
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
          buffer = (char *)TtaGetMemory (length + 1);
          TtaGiveTextAttributeValue (attr, buffer, &length);
          strcat (buffer, " ");
          strcat (buffer, text);
          TtaSetAttributeText (attr, buffer, el, doc);
          TtaFreeMemory (buffer);
        }
    }
}


/*----------------------------------------------------------------------
  XhtmlTypeAttrValue 
  Value val has been read for the HTML attribute TYPE.
  Create a child for the current Thot element INPUT accordingly.
  ----------------------------------------------------------------------*/
static void XhtmlTypeAttrValue (char *val,
                                Attribute currentAttribute,
                                Element lastAttrElement,
                                ParserData *context, ThotBool isXML)
{
  ElementType     elType;
  Element         newChild;
  AttributeType   attrType;
  Attribute       attr;
  char            msgBuffer[MaxMsgLength];
  int             value, nb;
  ThotBool        level;

  /* Look in the dummy section of the attribute value table */
  attrType.AttrTypeNum = DummyAttribute;
  MapHTMLAttributeValue (val, &attrType, &value);
  elType = TtaGetElementType (context->lastElement);
  if (value <= 0)
    /* invalid value for the type attribute of an input element */
    {
      snprintf (msgBuffer, MaxMsgLength,  "Unknown attribute value \"type=%50s\"", val);
      if (isXML)
        XmlParseError (errorParsing, (unsigned char *)msgBuffer, 0);
      else
        HTMLParseError (context->doc, msgBuffer, 0);
      MapHTMLAttribute ("unknown_attr", &attrType, NULL,
                        &level, context->doc);
      sprintf (msgBuffer, "type=%s", val);
      CreateHTMLAttribute (context->lastElement, attrType, msgBuffer, TRUE,
                           context->doc, &currentAttribute, &lastAttrElement);
    }
  else
    /* value is the Thot type of the element to be created for this value of
       the TYPE attribute */
    {
      if (elType.ElTypeNum != HTML_EL_Input)
        {
          sprintf (msgBuffer, "Duplicate attribute \"type = %s\"", val);
          if (isXML)
            XmlParseError (errorParsing, (unsigned char *)msgBuffer, 0);
          else
            HTMLParseError (context->doc, msgBuffer, 0);
        }
      else
        {
          elType.ElTypeNum = value;
          newChild = TtaNewTree (context->doc, elType, "");
          
          nb = TtaGetElementLineNumber (context->lastElement);
          TtaSetElementLineNumber (newChild, nb);
          TtaInsertFirstChild (&newChild, context->lastElement, context->doc);
          /* add the attribute type */
          attrType.AttrSSchema = elType.ElSSchema;
          attrType.AttrTypeNum = HTML_ATTR_type;
          attr = TtaGetAttribute (newChild, attrType);
          if (attr == NULL)
            {
              attr = TtaNewAttribute (attrType);
              TtaAttachAttribute (newChild, attr, context->doc);
            }
        }
    }
}

/*----------------------------------------------------------------------
  CreateAttrWidthPercentPxl
  an HTML attribute "width" has been created for a Table, an image,
  an Object of a HR.
  Create the corresponding attribute IntWidthPercent, IntWidthPxl or
  IntWidthRelative for element el.
  oldWidth is -1 or the old image width.
  ----------------------------------------------------------------------*/
void CreateAttrWidthPercentPxl (const char *buffer, Element el,
                                Document doc, int oldWidth)
{
  AttributeType   attrTypePxl, attrTypePercent, attrTypeRelative;
  Attribute       attrOld, attrNew;
  int             length, val;
  char            msgBuffer[MaxMsgLength];
  ElementType	    elType, childType;
  Element         origEl, child;
  int             w, h;
  ThotBool        isImage, isSVG = FALSE;

  if (buffer == NULL || buffer[0] == EOS)
    return;
  origEl = el;
  elType = TtaGetElementType (el);
  isImage = (elType.ElTypeNum == HTML_EL_PICTURE_UNIT ||
             elType.ElTypeNum == HTML_EL_Data_cell ||
             elType.ElTypeNum == HTML_EL_Heading_cell ||
             elType.ElTypeNum == HTML_EL_Object ||
             elType.ElTypeNum == HTML_EL_IMG ||
             elType.ElTypeNum == HTML_EL_Image_Input);

  if (elType.ElTypeNum == HTML_EL_Object)
    /* the width attribute is attached to an Object element */
    {
      child = TtaGetFirstChild (el);
      if (child)
        {
          childType = TtaGetElementType (child);
          if (childType.ElTypeNum == HTML_EL_PICTURE_UNIT)
            /* the Object element is of type image. apply the width
               attribute to the actual image element */
            el = child;
#ifdef _SVG
          else
            {
              el = child;
              child = TtaGetFirstChild (el);
              if (child)
                {
                  childType = TtaGetElementType (child);
                  isSVG = (!strcmp (TtaGetSSchemaName (childType.ElSSchema), "SVG") &&
                      childType.ElTypeNum == SVG_EL_SVG);
                }
            }
#endif /* _SVG */
        }
    }

  /* remove trailing spaces */
  length = strlen (buffer) - 1;
  while (length > 0 && buffer[length] <= SPACE)
    length--;
  attrTypePxl.AttrSSchema = elType.ElSSchema;
  attrTypePercent.AttrSSchema = elType.ElSSchema;
  attrTypeRelative.AttrSSchema = elType.ElSSchema;
  attrTypePxl.AttrTypeNum = HTML_ATTR_IntWidthPxl;
  attrTypePercent.AttrTypeNum = HTML_ATTR_IntWidthPercent;
  attrTypeRelative.AttrTypeNum = HTML_ATTR_IntWidthRelative;
  do
    {
      /* is the last character a '%' ? */
      if (buffer[length] == '%')
        /* percentage */
        {
          /* remove IntWidthPxl or IntWidthRelative */
          attrOld = TtaGetAttribute (el, attrTypePxl);
          if (!attrOld)
            attrOld = TtaGetAttribute (el, attrTypeRelative);
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
      /* is the last character a '*' ? */
      else if (buffer[length] == '*')
        /* relative width */
        {
          /* remove IntWidthPxl or IntWidthPercent */
          attrOld = TtaGetAttribute (el, attrTypePxl);
          if (!attrOld)
            attrOld = TtaGetAttribute (el, attrTypePercent);
          /* update IntWidthRelative */
          attrNew = TtaGetAttribute (el, attrTypeRelative);
          if (attrNew == NULL)
            {
              attrNew = TtaNewAttribute (attrTypeRelative);
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
        /* width in pixels */
        {
          /* remove IntWidthPercent or IntWidthRelative */
          attrOld = TtaGetAttribute (el, attrTypePercent);
          if (!attrOld)
            attrOld = TtaGetAttribute (el, attrTypeRelative);
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

      if (attrOld)
        TtaRemoveAttribute (el, attrOld, doc);
      if (sscanf (buffer, "%d", &val))
        TtaSetAttributeValue (attrNew, val, el, doc);
      else
        /* its not a number. Delete attribute and send an error message */
        {
          TtaRemoveAttribute (el, attrNew, doc);
// Replaced by sprintf format
//          if (strlen (buffer) > MaxMsgLength - 30)
//            buffer[MaxMsgLength - 30] = EOS;
          sprintf (msgBuffer, "Invalid attribute value \"%*s\"", 30, buffer);
          HTMLParseError (doc, msgBuffer, 0);
        }

      if (el != origEl)
        // apply the attribute to the object itself
        el = origEl;
      else
        el = NULL;
    }
  while (el);

  if (isImage)
    UpdateImageMap (origEl, doc, oldWidth, -1);
  if (isSVG && oldWidth != -1)
    {
      // force the redisplay of the SVG element
      el = TtaGetParent (child);
      if (el)
        {
          TtaRemoveTree (child, doc);
          TtaInsertFirstChild (&child, el, doc);
        }
    }
}

/*----------------------------------------------------------------------
  CreateAttrHeightPercentPxl
  an HTML attribute "height" has been created for a Table, an image,
  an Object or a HR.
  Create the corresponding attribute IntHeightPercent or IntHeightPxl.
  oldHeight is -1 or the old image height.
  ----------------------------------------------------------------------*/
void CreateAttrHeightPercentPxl (const char *buffer, Element el,
                                 Document doc, int oldHeight)
{
  AttributeType   attrTypePxl, attrTypePercent;
  Attribute       attrOld, attrNew;
  int             length, val;
  char            msgBuffer[MaxMsgLength];
  ElementType	    elType, childType;
  Element         origEl, child;
  int             w, h;
  ThotBool        isImage, isSVG = FALSE;

  origEl = el;
  elType = TtaGetElementType (el);
  isImage = (elType.ElTypeNum == HTML_EL_PICTURE_UNIT ||
             elType.ElTypeNum == HTML_EL_Data_cell ||
             elType.ElTypeNum == HTML_EL_Heading_cell ||
             elType.ElTypeNum == HTML_EL_Object ||
             elType.ElTypeNum == HTML_EL_IMG);

  if (elType.ElTypeNum == HTML_EL_Object)
    /* the height attribute is attached to an Object element */
    {
      child = TtaGetFirstChild (el);
      if (!child)
        return;
      else
        {
          childType = TtaGetElementType (child);
          if (childType.ElTypeNum == HTML_EL_PICTURE_UNIT)
            /* the Object element is of type image. apply the width
               attribute to the actual image element */
            el = child;
#ifdef _SVG
          else
            {
              el = child;
              child = TtaGetFirstChild (el);
              if (child)
                {
                  childType = TtaGetElementType (child);
                  isSVG = (!strcmp (TtaGetSSchemaName (childType.ElSSchema), "SVG") &&
                      childType.ElTypeNum == SVG_EL_SVG);
                }
            }
#endif /* _SVG */
        }
    }

  /* remove trailing spaces */
  length = strlen (buffer) - 1;
  while (length > 0 && buffer[length] <= SPACE)
    length--;
  attrTypePxl.AttrSSchema = elType.ElSSchema;
  attrTypePercent.AttrSSchema = elType.ElSSchema;
  attrTypePxl.AttrTypeNum = HTML_ATTR_IntHeightPxl;
  attrTypePercent.AttrTypeNum = HTML_ATTR_IntHeightPercent;
  do
    {
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

      if (attrOld)
        TtaRemoveAttribute (el, attrOld, doc);
      if (sscanf (buffer, "%d", &val))
        TtaSetAttributeValue (attrNew, val, el, doc);
      else
        /* its not a number. Delete attribute and send an error message */
        {
          TtaRemoveAttribute (el, attrNew, doc);
// Replaced by sprintf format
//          if (strlen (buffer) > MaxMsgLength - 30)
//            buffer[MaxMsgLength - 30] = EOS;
          sprintf (msgBuffer, "Invalid attribute value \"%*s\"", 30, buffer);
          HTMLParseError (doc, msgBuffer, 0);
        }

      if (el != origEl)
        // apply the attribute to the object itself
        el = origEl;
      else
        el = NULL;
    }
  while (el);

  if (isImage)
    UpdateImageMap (origEl, doc, oldHeight, -1);
  if (isSVG && oldHeight != -1)
    {
      // force the redisplay of the SVG element
      el = TtaGetParent (child);
      if (el)
        {
          TtaRemoveTree (child, doc);
          TtaInsertFirstChild (&child, el, doc);
        }
    }
}

/*----------------------------------------------------------------------
  CreateAttrIntAreaSize
  an HTML attribute "size" has been created or modified for a input element.
  Create or update the corresponding attribute IntAreaSize.
  ----------------------------------------------------------------------*/
void CreateAttrIntAreaSize (int value, Element el, Document doc)
{
  AttributeType   attrType;
  Attribute       attr;
  ElementType	  elType;

  elType = TtaGetElementType (el);
  attrType.AttrSSchema = elType.ElSSchema;
  if (elType.ElTypeNum == HTML_EL_Image_Input)
    {
      attrType.AttrTypeNum = HTML_ATTR_IntWidthPxl;
      attr = TtaGetAttribute (el, attrType);
      if (!attr)
        {
          attr = TtaNewAttribute (attrType);
          TtaAttachAttribute (el, attr, doc);
        }
      /* the presentation rule associated with attribute IntWidthPxl */
      TtaSetAttributeValue (attr, value, el, doc);
    }
  else
    {
      attrType.AttrTypeNum = HTML_ATTR_IntAreaSize;
      attr = TtaGetAttribute (el, attrType);
      if (!attr)
        {
          attr = TtaNewAttribute (attrType);
          TtaAttachAttribute (el, attr, doc);
        }
      /* the presentation rule associated with attribute IntAreaSize expresses
         the element width in "em". Convert the value into em */
      TtaSetAttributeValue (attr, (int) (value * 0.40), el, doc);
    }
}

/*----------------------------------------------------------------------
  CreateAttrIntSize
  an HTML attribute "size" has been created for a Font element.
  Create the corresponding internal attribute.
  ----------------------------------------------------------------------*/
void CreateAttrIntSize (char *buffer, Element el, Document doc)

{
  ElementType    elType;
  AttributeType  attrType;
  int            val, ind, factor, delta;
  Attribute      attr;
  char         msgBuffer[MaxMsgLength];

  /* is the first character a '+' or a '-' ? */
  elType = TtaGetElementType (el);
  ind = 0;
  factor = 1;
  delta = 0;
  if (buffer[0] == '+')
    {
      attrType.AttrTypeNum = HTML_ATTR_IntSizeIncr;
      ind++;
      factor = 1;
    }
  else if (buffer[0] == '-')
    {
      attrType.AttrTypeNum = HTML_ATTR_IntSizeDecr;
      ind++;
      factor = 1;
    }
  else
    {
      attrType.AttrTypeNum = HTML_ATTR_IntSizeRel;
      delta = 1;
    }
  attrType.AttrSSchema = elType.ElSSchema;
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
      HTMLParseError (doc, msgBuffer, 0);
    }
}
/*----------------------------------------------------------------------
  EndOfHTMLAttributeValue
  Filling of an HTML attribute value
  ----------------------------------------------------------------------*/
void EndOfHTMLAttributeValue (char *attrValue, AttributeMapping *lastMappedAttr,
                              Attribute currentAttribute, Element lastAttrElement,
                              ThotBool UnknownAttr, ParserData *context,
                              ThotBool isXML)
{
  AttributeType   attrType, attrType1;
  Attribute       attr;
  ElementType	  elType;
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
  ThotBool            loadcss;

  /* treatments of some particular HTML attributes */
  if (!strcmp (lastMappedAttr->XMLattribute, "style"))
    {
      TtaSetAttributeText (currentAttribute, attrValue,
                           lastAttrElement, context->doc);
      /* check if we have to load CSS */
      TtaGetEnvBoolean ("LOAD_CSS", &loadcss);
      if (loadcss)
        ParseHTMLSpecificStyle (context->lastElement, attrValue,
                                context->doc, 1000, FALSE);
      done = TRUE;
    }
  else
    {
      if (!strcmp (lastMappedAttr->XMLattribute, "link"))
        HTMLSetAlinkColor (context->doc, context->lastElement, attrValue);
      else if (!strcmp (lastMappedAttr->XMLattribute, "alink"))
        HTMLSetAactiveColor (context->doc, context->lastElement, attrValue);
      else if (!strcmp (lastMappedAttr->XMLattribute, "vlink"))
        HTMLSetAvisitedColor (context->doc, context->lastElement, attrValue);
    }

  if (!done)
    {
      elType = TtaGetElementType (lastAttrElement);
      val = 0;
      translation = lastMappedAttr->AttrOrContent;
      switch (translation)
        {
        case 'C':	/* Content */
          child = PutInContent (attrValue, context);
          if (child != NULL)
            TtaAppendTextContent (child, (unsigned char *)"\" ", context->doc);
          break;
        case 'A':
          if (currentAttribute != NULL)
            {
              TtaGiveAttributeType (currentAttribute, &attrType, &attrKind);
              switch (attrKind)
                {
                case 0:	/* enumerate */
                  if (isXML)
                    MapHTMLAttributeValue (attrValue, &attrType, &val);
                  else
                    val = MapAttrValue (lastMappedAttr->ThotAttribute,
                                        attrValue);
                  if (val < 0)
                    {
                      TtaGiveAttributeType (currentAttribute,
                                            &attrType, &attrKind);
                      attrName = TtaGetAttributeName (attrType);
                      snprintf (msgBuffer, MaxMsgLength,
                               "Invalid attribute value \"%s = %50s\"",
                               attrName, attrValue);
                      if (isXML)
                        XmlParseError (errorParsing, (unsigned char *)msgBuffer, 0);
                      else
                        /* we are parsing an HTML file, not an XHTML file */
                        {
                          /* generate an error message in the log */
                          HTMLParseError (context->doc, msgBuffer, 0);
                          /* special case for value POLYGON of attribute 
                             shape (AREA element) */
                          if (attrType.AttrTypeNum == HTML_ATTR_shape &&
                              strcasecmp (attrValue, "POLYGON") == 0)
                            {
                              val = HTML_ATTR_shape_VAL_polygon;
                              /* interpret it as if it were "poly" */
                              TtaSetAttributeValue (currentAttribute, val,
                                                    lastAttrElement, context->doc);
                            }
                          else
                            /* remove the attribute and replace it by an */
                            /* Invalid_attribute (not for XHTML) */
                            {
                              TtaRemoveAttribute (lastAttrElement,
                                                  currentAttribute, context->doc);
                              attrType.AttrSSchema = elType.ElSSchema;
                              attrType.AttrTypeNum =
                                pHTMLAttributeMapping[0].ThotAttribute;
                              sprintf (msgBuffer, "%s=%s", attrName,attrValue);
                              CreateHTMLAttribute (lastAttrElement, attrType,
                                                   msgBuffer, TRUE, context->doc,
                                                   &currentAttribute, &lastAttrElement);
                            }
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
                      snprintf (msgBuffer, MaxMsgLength,
                               "Unknown attribute value \"%s\"",
                               attrValue);
                      if (isXML)
                        XmlParseError (errorParsing, (unsigned char *)msgBuffer, 0);
                      else
                        HTMLParseError (context->doc, msgBuffer, 0);
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
                          if (lang < 0)
                            {
                              snprintf (msgBuffer, MaxMsgLength,
                                       "warning - unsupported language: %50s",
                                       attrValue);
                              if (isXML)
                                XmlParseError (warningMessage, (unsigned char *)msgBuffer, 0);
                              else
                                HTMLParseError (context->doc, msgBuffer, 0);
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
                              attrType1.AttrSSchema = elType.ElSSchema;
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
                      else if (attrType.AttrTypeNum == HTML_ATTR_ID ||
                               attrType.AttrTypeNum == HTML_ATTR_NAME)
                        CheckUniqueName (lastAttrElement, context->doc,
                                         currentAttribute, attrType);
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
                          buffer = (char *)TtaGetMemory (length + 1);
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
          XhtmlTypeAttrValue (attrValue, currentAttribute,
                              lastAttrElement, context,isXML );
          break;  
        default:
          break;
        }

      if (lastMappedAttr->ThotAttribute == HTML_ATTR_Width__)
        /* HTML attribute "width" */
        {
          /* if it's an Object element, wait until all attributes are handled,
             especially the data attribute that may generate the image to
             which the width has to be applied */
          if (elType.ElTypeNum != HTML_EL_Object)
            /* create the corresponding attribute IntWidthPercent or */
            /* IntWidthPxl */
            CreateAttrWidthPercentPxl (attrValue, lastAttrElement,
                                       context->doc, -1);
        }
      else if (lastMappedAttr->ThotAttribute == HTML_ATTR_Height_)
        /* HTML attribute "height" */
        {
          /* if it's an Object element, wait until all attributes are handled,
             especially the data attribute that may generate the image to
             which the height has to be applied */
          if (elType.ElTypeNum != HTML_EL_Object)
            /* create the corresponding attribute IntHeightPercent or */
            /* IntHeightPxl */
            CreateAttrHeightPercentPxl (attrValue, lastAttrElement,
                                        context->doc, -1);
        }
      else if (lastMappedAttr->ThotAttribute == HTML_ATTR_Area_Size)
        /* HTML attribute "size" for an element "input" */
        CreateAttrIntAreaSize (val, lastAttrElement, context->doc);
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
              HTMLSetBackgroundImage (context->doc, context->lastElement,
                                      REPEAT, 2000, attrValue, FALSE);
            }
          else if (!strcmp (lastMappedAttr->XMLattribute, "bgcolor"))
            HTMLSetBackgroundColor (context->doc, context->lastElement,
                                    2000, attrValue);
          else if (!strcmp (lastMappedAttr->XMLattribute, "text") ||
                   !strcmp (lastMappedAttr->XMLattribute, "color"))
            HTMLSetForegroundColor (context->doc, context->lastElement,
                                    2000, attrValue);
        }
    }
}

/*----------------------------------------------------------------------
  MapHTMLAttributeValue
  Search in the Attribute Value Mapping Table the entry for the attribute
  ThotAtt and its value attVal. Returns the corresponding Thot value.
  ----------------------------------------------------------------------*/
void MapHTMLAttributeValue (char *attVal, const AttributeType *attrType,
                            int *value)
{
  MapXMLAttributeValue (XHTML_TYPE, attVal, attrType, value);
}
