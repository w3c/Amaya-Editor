/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996-2001.
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

/* Mapping table of HTML attribute values */
AttrValueMapping XhtmlAttrValueMappingTable[] =
{
   {HTML_ATTR_dir, "ltr", HTML_ATTR_dir_VAL_ltr},
   {HTML_ATTR_dir, "rtl", HTML_ATTR_dir_VAL_rtl},

   {HTML_ATTR_TextAlign, "left", HTML_ATTR_TextAlign_VAL_left_},
   {HTML_ATTR_TextAlign, "center", HTML_ATTR_TextAlign_VAL_center_},
   {HTML_ATTR_TextAlign, "right", HTML_ATTR_TextAlign_VAL_right_},
   {HTML_ATTR_TextAlign, "justify", HTML_ATTR_TextAlign_VAL_justify_},

   {HTML_ATTR_Align, "left", HTML_ATTR_Align_VAL_left_},
   {HTML_ATTR_Align, "center", HTML_ATTR_Align_VAL_center_},
   {HTML_ATTR_Align, "right", HTML_ATTR_Align_VAL_right_},

   {HTML_ATTR_LAlign, "top", HTML_ATTR_LAlign_VAL_Top_},
   {HTML_ATTR_LAlign, "bottom", HTML_ATTR_LAlign_VAL_Bottom_},
   {HTML_ATTR_LAlign, "left", HTML_ATTR_LAlign_VAL_Left_},
   {HTML_ATTR_LAlign, "right", HTML_ATTR_LAlign_VAL_Right_},

   {HTML_ATTR_Clear, "left", HTML_ATTR_Clear_VAL_Left_},
   {HTML_ATTR_Clear, "right", HTML_ATTR_Clear_VAL_Right_},
   {HTML_ATTR_Clear, "all", HTML_ATTR_Clear_VAL_All_},
   {HTML_ATTR_Clear, "none", HTML_ATTR_Clear_VAL_None_},

   {HTML_ATTR_NumberStyle, "1", HTML_ATTR_NumberStyle_VAL_Arabic_},
   {HTML_ATTR_NumberStyle, "a", HTML_ATTR_NumberStyle_VAL_LowerAlpha},
   {HTML_ATTR_NumberStyle, "A", HTML_ATTR_NumberStyle_VAL_UpperAlpha},
   {HTML_ATTR_NumberStyle, "i", HTML_ATTR_NumberStyle_VAL_LowerRoman},
   {HTML_ATTR_NumberStyle, "I", HTML_ATTR_NumberStyle_VAL_UpperRoman},

   {HTML_ATTR_BulletStyle, "disc", HTML_ATTR_BulletStyle_VAL_disc},
   {HTML_ATTR_BulletStyle, "square", HTML_ATTR_BulletStyle_VAL_square},
   {HTML_ATTR_BulletStyle, "circle", HTML_ATTR_BulletStyle_VAL_circle},

   {HTML_ATTR_ItemStyle, "1", HTML_ATTR_ItemStyle_VAL_Arabic_},
   {HTML_ATTR_ItemStyle, "a", HTML_ATTR_ItemStyle_VAL_LowerAlpha},
   {HTML_ATTR_ItemStyle, "A", HTML_ATTR_ItemStyle_VAL_UpperAlpha},
   {HTML_ATTR_ItemStyle, "i", HTML_ATTR_ItemStyle_VAL_LowerRoman},
   {HTML_ATTR_ItemStyle, "I", HTML_ATTR_ItemStyle_VAL_UpperRoman},
   {HTML_ATTR_ItemStyle, "disc", HTML_ATTR_ItemStyle_VAL_disc},
   {HTML_ATTR_ItemStyle, "square", HTML_ATTR_ItemStyle_VAL_square},
   {HTML_ATTR_ItemStyle, "circle", HTML_ATTR_ItemStyle_VAL_circle},

   {HTML_ATTR_Button_type, "button", HTML_ATTR_Button_type_VAL_button},
   {HTML_ATTR_Button_type, "submit", HTML_ATTR_Button_type_VAL_submit},
   {HTML_ATTR_Button_type, "reset", HTML_ATTR_Button_type_VAL_reset},

   {HTML_ATTR_frame, "void", HTML_ATTR_frame_VAL_void},
   {HTML_ATTR_frame, "above", HTML_ATTR_frame_VAL_above},
   {HTML_ATTR_frame, "below", HTML_ATTR_frame_VAL_below},
   {HTML_ATTR_frame, "hsides", HTML_ATTR_frame_VAL_hsides},
   {HTML_ATTR_frame, "lhs", HTML_ATTR_frame_VAL_lhs},
   {HTML_ATTR_frame, "rhs", HTML_ATTR_frame_VAL_rhs},
   {HTML_ATTR_frame, "vsides", HTML_ATTR_frame_VAL_vsides},
   {HTML_ATTR_frame, "box", HTML_ATTR_frame_VAL_box},
   {HTML_ATTR_frame, "border", HTML_ATTR_frame_VAL_border},

   {HTML_ATTR_frameborder, "0", HTML_ATTR_frameborder_VAL_Border0},
   {HTML_ATTR_frameborder, "1", HTML_ATTR_frameborder_VAL_Border1},

   {HTML_ATTR_scrolling, "yes", HTML_ATTR_scrolling_VAL_Yes_},
   {HTML_ATTR_scrolling, "no", HTML_ATTR_scrolling_VAL_No_},
   {HTML_ATTR_scrolling, "auto", HTML_ATTR_scrolling_VAL_auto_},

   {HTML_ATTR_rules_, "none", HTML_ATTR_rules__VAL_none_},
   {HTML_ATTR_rules_, "groups", HTML_ATTR_rules__VAL_groups},
   {HTML_ATTR_rules_, "rows", HTML_ATTR_rules__VAL_rows},
   {HTML_ATTR_rules_, "cols", HTML_ATTR_rules__VAL_cols},
   {HTML_ATTR_rules_, "all", HTML_ATTR_rules__VAL_all},

   {HTML_ATTR_Cell_align, "left", HTML_ATTR_Cell_align_VAL_Cell_left},
   {HTML_ATTR_Cell_align, "center", HTML_ATTR_Cell_align_VAL_Cell_center},
   {HTML_ATTR_Cell_align, "right", HTML_ATTR_Cell_align_VAL_Cell_right},
   {HTML_ATTR_Cell_align, "justify", HTML_ATTR_Cell_align_VAL_Cell_justify},
   {HTML_ATTR_Cell_align, "char", HTML_ATTR_Cell_align_VAL_Cell_char},

   {HTML_ATTR_Alignment, "top", HTML_ATTR_Alignment_VAL_Top_},
   {HTML_ATTR_Alignment, "middle", HTML_ATTR_Alignment_VAL_Middle_},
   {HTML_ATTR_Alignment, "bottom", HTML_ATTR_Alignment_VAL_Bottom_},
   {HTML_ATTR_Alignment, "left", HTML_ATTR_Alignment_VAL_Left_},
   {HTML_ATTR_Alignment, "right", HTML_ATTR_Alignment_VAL_Right_},

   {HTML_ATTR_METHOD, "get", HTML_ATTR_METHOD_VAL_Get_},
   {HTML_ATTR_METHOD, "post", HTML_ATTR_METHOD_VAL_Post_},

   {HTML_ATTR_Position, "top", HTML_ATTR_Position_VAL_Position_top},
   {HTML_ATTR_Position, "bottom", HTML_ATTR_Position_VAL_Position_bottom},
   {HTML_ATTR_Position, "left", HTML_ATTR_Position_VAL_Position_left},
   {HTML_ATTR_Position, "right", HTML_ATTR_Position_VAL_Position_right},

   {HTML_ATTR_Row_valign, "top", HTML_ATTR_Row_valign_VAL_Row_top},
   {HTML_ATTR_Row_valign, "middle", HTML_ATTR_Row_valign_VAL_Row_middle},
   {HTML_ATTR_Row_valign, "bottom", HTML_ATTR_Row_valign_VAL_Row_bottom},
   {HTML_ATTR_Row_valign, "baseline", HTML_ATTR_Row_valign_VAL_Row_baseline},

   {HTML_ATTR_Cell_valign, "top", HTML_ATTR_Cell_valign_VAL_Cell_top},
   {HTML_ATTR_Cell_valign, "middle", HTML_ATTR_Cell_valign_VAL_Cell_middle},
   {HTML_ATTR_Cell_valign, "bottom", HTML_ATTR_Cell_valign_VAL_Cell_bottom},
   {HTML_ATTR_Cell_valign, "baseline", HTML_ATTR_Cell_valign_VAL_Cell_baseline},

   {HTML_ATTR_shape, "rect", HTML_ATTR_shape_VAL_rectangle},
   {HTML_ATTR_shape, "circle", HTML_ATTR_shape_VAL_circle},
   {HTML_ATTR_shape, "poly", HTML_ATTR_shape_VAL_polygon},

   {HTML_ATTR_valuetype, "data", HTML_ATTR_valuetype_VAL_data_},
   {HTML_ATTR_valuetype, "ref", HTML_ATTR_valuetype_VAL_ref},
   {HTML_ATTR_valuetype, "object", HTML_ATTR_valuetype_VAL_object_},

/* HTML attribute TYPE generates a Thot element */
   {DummyAttribute, "button", HTML_EL_Button_Input},
   {DummyAttribute, "checkbox", HTML_EL_Checkbox_Input},
   {DummyAttribute, "file", HTML_EL_File_Input},
   {DummyAttribute, "hidden", HTML_EL_Hidden_Input},
   {DummyAttribute, "image", HTML_EL_PICTURE_UNIT},
   {DummyAttribute, "password", HTML_EL_Password_Input},
   {DummyAttribute, "radio", HTML_EL_Radio_Input},
   {DummyAttribute, "reset", HTML_EL_Reset_Input},
   {DummyAttribute, "submit", HTML_EL_Submit_Input},
   {DummyAttribute, "text", HTML_EL_Text_Input},

/* The following declarations allow the parser to accept boolean attributes */
/* written "checked=CHECKED"), for instance */
   {HTML_ATTR_ISMAP, "ismap", HTML_ATTR_ISMAP_VAL_Yes_},
   {HTML_ATTR_nohref, "nohref", HTML_ATTR_nohref_VAL_Yes_},
   {HTML_ATTR_COMPACT, "compact", HTML_ATTR_COMPACT_VAL_Yes_},
   {HTML_ATTR_Multiple, "multiple", HTML_ATTR_Multiple_VAL_Yes_},
   {HTML_ATTR_Selected, "selected", HTML_ATTR_Selected_VAL_Yes_},
   {HTML_ATTR_Checked, "checked", HTML_ATTR_Checked_VAL_Yes_},
   {HTML_ATTR_No_wrap, "nowrap", HTML_ATTR_No_wrap_VAL_no_wrap},
   {HTML_ATTR_NoShade, "noshade", HTML_ATTR_NoShade_VAL_NoShade_},
   {HTML_ATTR_declare, "declare", HTML_ATTR_declare_VAL_Yes_},
   {HTML_ATTR_defer, "defer", HTML_ATTR_defer_VAL_Yes_},
   {HTML_ATTR_disabled, "disabled", HTML_ATTR_disabled_VAL_Yes_},
   {HTML_ATTR_readonly, "readonly", HTML_ATTR_readonly_VAL_Yes_},
   {HTML_ATTR_no_resize, "noresize", HTML_ATTR_no_resize_VAL_Yes_},

   /* XML attribute xml:space */
   {HTML_ATTR_xml_space, "default", HTML_ATTR_xml_space_VAL_xml_space_default},
   {HTML_ATTR_xml_space, "preserve", HTML_ATTR_xml_space_VAL_xml_space_preserve},

   {0, "", 0}			/* Last entry. Mandatory */
};

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
			   *ptrText = utolower (*ptrText);
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
   XhtmlEntityCreated
   A XTHML entity has been created by the XML parser.
  ----------------------------------------------------------------------*/
void             XhtmlEntityCreated (int         entityValue,
				     char       *entityName,
				     ThotBool    entityFound,
				     ParserData *context)

{ 
  ElementType	 elType;
  Element	 elLeaf;
  AttributeType  attrType;
  Attribute	 attr;
  Language	 lang;
  int		 len;
#define MAX_ENTITY_LENGTH 80
  char	         buffer[MAX_ENTITY_LENGTH];
  char	         bufName[MAX_ENTITY_LENGTH];
  char           msgBuffer[MAX_ENTITY_LENGTH + 50];
  
  if (entityValue <= 255 && entityFound)
    {
      /* It is an ISO latin1 character */
      buffer[0] = ((unsigned char) entityValue);
      buffer[1] = EOS;
      lang = TtaGetLanguageIdFromAlphabet('L');
      PutInXmlElement (buffer);
    }
  else
    {
      if (entityFound)
	{
	  /* try to find a fallback character */
	  GetFallbackCharacter (entityValue, buffer, &lang);
	}
      len = strlen (entityName);
      bufName[0] = (char) START_ENTITY;
      strncpy (&bufName[1], entityName, len);
      bufName[len+1] = ';';
      bufName[len+2] = EOS;
      
      /* Create a new text leaf */
      elType.ElSSchema = GetXMLSSchema (XHTML_TYPE, context->doc);
      elType.ElTypeNum = HTML_EL_TEXT_UNIT;
      elLeaf = TtaNewElement (context->doc, elType);
      XmlSetElemLineNumber (elLeaf);
      InsertXmlElement (&elLeaf);
      if (buffer[0] == '?' || !entityFound)
	{
	  /* Character not found in the fallback table or not supported */
	  /* Put the entity name into the new text leaf */
	  lang = TtaGetLanguageIdFromAlphabet('L');
	  TtaSetTextContent (elLeaf, bufName, lang, context->doc);
	  if (entityFound)
	    {
	      sprintf (msgBuffer, "XHTML entity not supported : &%s", bufName);
	      XmlParseError (errorParsing, msgBuffer, 0);
	    }
	}
      else
	{
	  /* Character found in the fallback table */
	  TtaSetTextContent (elLeaf, buffer, lang, context->doc);
	  /* Associate an attribute EntityName with the new text leaf */
	  attrType.AttrSSchema = elType.ElSSchema;
	  attrType.AttrTypeNum = HTML_ATTR_EntityName;
	  attr = TtaNewAttribute (attrType);
	  TtaAttachAttribute (elLeaf, attr, context->doc);
	  TtaSetAttributeText (attr, bufName, elLeaf, context->doc);
	}
      context->lastElement = elLeaf;
      context->lastElementClosed = TRUE;
      context->mergeText = FALSE;
      /* Make that text leaf read-only */
      TtaSetAccessRight (elLeaf, ReadOnly, context->doc);
    }
}

/*----------------------------------------------------------------------
  XhtmlElementComplete
  Complete Xhtml elements.
  Check its attributes and its contents.
  ----------------------------------------------------------------------*/
void XhtmlElementComplete (Element el, Document doc, int *error)

{
   ElementType    elType, newElType, childType;
   Element        constElem, child, desc, leaf, prev, next, last,
	          elFrames, lastFrame, lastChild, parent;
   Attribute      attr;
   AttributeType  attrType;
   Language       lang;
   char           *text;
   char           lastChar[2];
   char           *name1;
   int            length;
   SSchema        docSSchema;

   *error = 0;
   docSSchema = TtaGetDocumentSSchema (doc);

   elType = TtaGetElementType (el);
   /* is this a block-level element in a character-level element? */
   if (!IsXMLElementInline (elType) &&
       elType.ElTypeNum != HTML_EL_Comment_ &&
       elType.ElTypeNum != HTML_EL_XMLPI)
       BlockInCharLevelElem (el);

   newElType.ElSSchema = elType.ElSSchema;
   switch (elType.ElTypeNum)
     {
     case HTML_EL_Object:	/*  it's an object */
       /* create Object_Content */
       child = TtaGetFirstChild (el);
       if (child != NULL)
	   elType = TtaGetElementType (child);
	 
       /* is it the PICTURE element ? */
       if (child == NULL || elType.ElTypeNum != HTML_EL_PICTURE_UNIT)
	 {
	   desc = child;
	   /* create the PICTURE element */
	   elType.ElTypeNum = HTML_EL_PICTURE_UNIT;
	   child = TtaNewTree (doc, elType, "");
	   if (desc == NULL)
	       TtaInsertFirstChild (&child, el, doc);
	   else
	       TtaInsertSibling (child, desc, TRUE, doc);
	 }

       /* copy attribute data into SRC attribute of Object_Image */
       attrType.AttrSSchema = docSSchema;
       attrType.AttrTypeNum = HTML_ATTR_data;
       attr = TtaGetAttribute (el, attrType);
       if (attr != NULL)
	 {
	   length = TtaGetTextAttributeLength (attr);
	   if (length > 0)
	     {
	       name1 = TtaGetMemory (length + 1);
	       TtaGiveTextAttributeValue (attr, name1, &length);
	       attrType.AttrTypeNum = HTML_ATTR_SRC;
	       attr = TtaGetAttribute (child, attrType);
	       if (attr == NULL)
		 {
		   attr = TtaNewAttribute (attrType);
		   TtaAttachAttribute (child, attr, doc);
		 }
	       TtaSetAttributeText (attr, name1, child, doc);
	       TtaFreeMemory (name1);
	     }
	 }

       /* is the Object_Content element already created ? */
       desc = child;
       TtaNextSibling(&desc);
       if (desc != NULL)
	   elType = TtaGetElementType (desc);
	 
       /* is it the Object_Content element ? */
       if (desc == NULL || elType.ElTypeNum != HTML_EL_Object_Content)
	 {
	   /* create Object_Content */
	   elType.ElTypeNum = HTML_EL_Object_Content;
	   desc = TtaNewTree (doc, elType, "");
	   TtaInsertSibling (desc, child, FALSE, doc);
	   /* move previous existing children into Object_Content */
	   child = TtaGetLastChild(el);
	   while (child != desc)
	     {
	       TtaRemoveTree (child, doc);
	       TtaInsertFirstChild (&child, desc, doc);
	       child = TtaGetLastChild(el);
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
		   XmlSetElemLineNumber (elFrames);
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
       XmlSetElemLineNumber (child);
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
     case HTML_EL_SCRIPT:	/* it's a SCRIPT element */
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
       if (IsParsingCSS ())
	 {
	   text = GetStyleContents (el);
	   if (text)
	     {
	       ReadCSSRules (doc, NULL, text,
			     TtaGetElementLineNumber (el), FALSE);
	       TtaFreeMemory (text);
	     }
	   SetParsingCSS (FALSE);
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
       if (IsWithinTable ())
	   NewCell (el, doc, FALSE);
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
  int             attrKind;
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
void              CreateAttrWidthPercentPxl (char *buffer, Element el,
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
void               EndOfHTMLAttributeValue (char       *attrValue,
					    AttributeMapping *lastMappedAttr,
					    Attribute   currentAttribute,
					    Element     lastAttrElement,
					    ThotBool    UnknownAttr,
					    ParserData *context,
					    ThotBool    isXML)

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
   int             val;
   int             length;
   int             attrKind;
   ThotBool        done = FALSE;
   ThotBool        level;
   char            msgBuffer[MaxMsgLength];

   /* treatments of some particular HTML attributes */
   if (!strcmp (lastMappedAttr->XMLattribute, "style"))
     {
       TtaSetAttributeText (currentAttribute, attrValue,
			    lastAttrElement, context->doc);
       ParseHTMLSpecificStyle (context->lastElement, attrValue,
			       context->doc, 1, FALSE);
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
				 "Unknown attribute value \"%s = %s\"",
				 attrName, attrValue);
		       if (isXML)
			 XmlParseError (errorParsing, msgBuffer, 0);
		       else
			 HTMLParseError (context->doc, msgBuffer);

		       /* remove the attribute and replace it by an */
		       /* Invalid_attribute */
		       TtaRemoveAttribute (lastAttrElement,
					   currentAttribute, context->doc);
		       if (isXML)
			 MapHTMLAttribute ("unknown_attr", &attrType,
					   NULL, &level, context->doc);
		       else
			 {
			   attrType.AttrSSchema = 
			     TtaGetDocumentSSchema (context->doc);
			   attrType.AttrTypeNum =
			     pHTMLAttributeMapping[0].ThotAttribute;
			 }
		       sprintf (msgBuffer, "%s=%s", attrName, attrValue);
		       CreateHTMLAttribute (lastAttrElement, attrType,
					    msgBuffer, TRUE, context->doc,
					    &currentAttribute, &lastAttrElement);
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
		       if (attrType.AttrTypeNum == HTML_ATTR_Langue)
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
		     /* this is the content of an invalid attribute */
		     /* append it to the current Invalid_attribute */
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
      else
	 if (!strcmp (lastMappedAttr->XMLattribute, "size"))
	   {
	     TtaGiveAttributeType (currentAttribute, &attrType, &attrKind);
	     if (attrType.AttrTypeNum == HTML_ATTR_Font_size)
	         CreateAttrIntSize (attrValue, lastAttrElement, context->doc);
	   }
      else
	 if (!strcmp (lastMappedAttr->XMLattribute, "shape"))
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
	   else
	     if (!strcmp (lastMappedAttr->XMLattribute, "value"))
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

       /* Some HTML attributes are equivalent to a CSS property:      */
       /*      background     ->                   background         */
       /*      bgcolor        ->                   background         */
       /*      text           ->                   color              */
       /*      color          ->                   color              */
	     else
	       if (!strcmp (lastMappedAttr->XMLattribute, "background"))
		 {
		   if (strlen (attrValue) > MaxMsgLength - 30)
		       attrValue[MaxMsgLength - 30] = EOS;
		   sprintf (msgBuffer, "background: url(%s)", attrValue);
		   ParseHTMLSpecificStyle (context->lastElement, msgBuffer,
					   context->doc, 1, FALSE);
		 }
	       else
		 if (!strcmp (lastMappedAttr->XMLattribute, "bgcolor"))
		     HTMLSetBackgroundColor (context->doc,
					     context->lastElement, attrValue);
		 else
		   if (!strcmp (lastMappedAttr->XMLattribute, "text") ||
		       !strcmp (lastMappedAttr->XMLattribute, "color"))
		     HTMLSetForegroundColor (context->doc,
					     context->lastElement, attrValue);
     }
}

/*----------------------------------------------------------------------
   MapHTMLAttributeValue
   Search in the Attribute Value Mapping Table the entry for the attribute
   ThotAtt and its value AttrVal. Returns the corresponding Thot value.
  ----------------------------------------------------------------------*/
void    MapHTMLAttributeValue (char          *AttrVal,
			       AttributeType  attrType,
			       int           *value)
{
  int  i;

  *value = 0;
  i = 0;
  while (XhtmlAttrValueMappingTable[i].ThotAttr != attrType.AttrTypeNum &&
	 XhtmlAttrValueMappingTable[i].ThotAttr != 0)
    i++;
  if (XhtmlAttrValueMappingTable[i].ThotAttr == attrType.AttrTypeNum)
    do
      if (!strcmp (XhtmlAttrValueMappingTable[i].XMLattrValue, AttrVal))
	*value = XhtmlAttrValueMappingTable[i].ThotAttrValue;
      else 
	i++;
    while (*value <= 0 &&
	   XhtmlAttrValueMappingTable[i].ThotAttr == attrType.AttrTypeNum);
}
