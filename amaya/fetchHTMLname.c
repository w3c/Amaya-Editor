/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *
 * fetchHTMLname
 *
 * Author: I. Vatton
 *
 */

/* This module is used by the html2thot parser and the css parser. */

#define THOT_EXPORT
#include "amaya.h"
#include "fetchHTMLname.h"
#include "parser.h"

#include "fetchHTMLname_f.h"
#include "fetchXMLname_f.h"

/* mapping table of HTML elements */
static GIMapping    HTMLGIMappingTable[] =
{
   /* This table MUST be in alphabetical order */
   {"a", SPACE, HTML_EL_Anchor, NULL},
   {"abbr", SPACE, HTML_EL_ABBR, NULL},
   {"acronym", SPACE, HTML_EL_ACRONYM, NULL},
   {"address", SPACE, HTML_EL_Address, NULL},
   {"applet", SPACE, HTML_EL_Applet, NULL},
   {"area", 'E', HTML_EL_AREA, NULL},
   {"b", SPACE, HTML_EL_Bold_text, NULL},
   {"base", 'E', HTML_EL_BASE, NULL},
   {"basefont", 'E', HTML_EL_BaseFont, NULL},
   {"bdo", SPACE, HTML_EL_BDO, NULL},
   {"big", SPACE, HTML_EL_Big_text, NULL},
   {"blockquote", SPACE, HTML_EL_Block_Quote, NULL},
   {"body", SPACE, HTML_EL_BODY, NULL},
   {"br", 'E', HTML_EL_BR, NULL},
   {"button", SPACE, HTML_EL_BUTTON, NULL},
   {"c", SPACE, HTML_EL_TEXT_UNIT, NULL},
   {"caption", SPACE, HTML_EL_CAPTION, NULL},
   {"center", SPACE, HTML_EL_Center, NULL},
   {"cite", SPACE, HTML_EL_Cite, NULL},
   {"code", SPACE, HTML_EL_Code, NULL},
   {"colgroup", SPACE, HTML_EL_COLGROUP, NULL},
   {"col", SPACE, HTML_EL_COL, NULL},
   {"dd", SPACE, HTML_EL_Definition, NULL},
   {"del", SPACE, HTML_EL_DEL, NULL},
   {"dfn", SPACE, HTML_EL_Def, NULL},
   {"dir", SPACE, HTML_EL_Directory, NULL},
   {"div", SPACE, HTML_EL_Division, NULL},
   {"dl", SPACE, HTML_EL_Definition_List, NULL},
   {"dt", SPACE, HTML_EL_Term, NULL},
   {"em", SPACE, HTML_EL_Emphasis, NULL},
   {"fieldset", SPACE, HTML_EL_FIELDSET, NULL},
   {"font", SPACE, HTML_EL_Font_, NULL},
   {"form", SPACE, HTML_EL_Form, NULL},
   {"frame", 'E', HTML_EL_FRAME, NULL},
   {"frameset", SPACE, HTML_EL_FRAMESET, NULL},
   {"h1", SPACE, HTML_EL_H1, NULL},
   {"h2", SPACE, HTML_EL_H2, NULL},
   {"h3", SPACE, HTML_EL_H3, NULL},
   {"h4", SPACE, HTML_EL_H4, NULL},
   {"h5", SPACE, HTML_EL_H5, NULL},
   {"h6", SPACE, HTML_EL_H6, NULL},
   {"head", SPACE, HTML_EL_HEAD, NULL},
   {"hr", 'E', HTML_EL_Horizontal_Rule, NULL},
   {"html", SPACE, HTML_EL_HTML, NULL},
   {"i", SPACE, HTML_EL_Italic_text, NULL},
   {"iframe", SPACE, HTML_EL_IFRAME, NULL},
   {"image", 'E', HTML_EL_PICTURE_UNIT, NULL},
   {"img", 'E', HTML_EL_PICTURE_UNIT, NULL},
   {"input", 'E', HTML_EL_Input, NULL},
   {"ins", SPACE, HTML_EL_INS, NULL},
   {"isindex", 'E', HTML_EL_ISINDEX, NULL},
   {"kbd", SPACE, HTML_EL_Keyboard, NULL},
   {"label", SPACE, HTML_EL_LABEL, NULL},
   {"legend", SPACE, HTML_EL_LEGEND, NULL},
   {"li", SPACE, HTML_EL_List_Item, NULL},
   {"link", 'E', HTML_EL_LINK, NULL},
   {"listing", SPACE, HTML_EL_Preformatted, NULL}, /*converted to PRE */
   {"map", SPACE, HTML_EL_MAP, NULL},
   {"math", SPACE, HTML_EL_Math, NULL},
   {"mathdisp", SPACE, HTML_EL_Math, NULL}, /* for compatibility with an
				     old version of MathML: WD-math-970704 */
   {"menu", SPACE, HTML_EL_Menu, NULL},
   {"meta", 'E', HTML_EL_META, NULL},
   {"noframes", SPACE, HTML_EL_NOFRAMES, NULL},
   {"noscript", SPACE, HTML_EL_NOSCRIPT, NULL},
   {"object", SPACE, HTML_EL_Object, NULL},
   {"ol", SPACE, HTML_EL_Numbered_List, NULL},
   {"optgroup", SPACE, HTML_EL_OptGroup, NULL},
   {"option", SPACE, HTML_EL_Option, NULL},
   {"p", SPACE, HTML_EL_Paragraph, NULL},
   {"p*", SPACE, HTML_EL_Pseudo_paragraph, NULL},
   {"param", 'E', HTML_EL_Parameter, NULL},
   {"plaintext", SPACE, HTML_EL_Preformatted, NULL},/* converted to PRE */
   {"pre", SPACE, HTML_EL_Preformatted, NULL},
   {"q", SPACE, HTML_EL_Quotation, NULL},
   {"s", SPACE, HTML_EL_Struck_text, NULL},
   {"samp", SPACE, HTML_EL_Sample, NULL},
   {"script", SPACE, HTML_EL_SCRIPT, NULL},
   {"select", SPACE, HTML_EL_Option_Menu, NULL},
   {"small", SPACE, HTML_EL_Small_text, NULL},
   {"span", SPACE, HTML_EL_Span, NULL},
   {"strike", SPACE, HTML_EL_Struck_text, NULL},
   {"strong", SPACE, HTML_EL_Strong, NULL},
   {"style", SPACE, HTML_EL_STYLE_, NULL},
   {"sub", SPACE, HTML_EL_Subscript, NULL},
   {"sup", SPACE, HTML_EL_Superscript, NULL},
   {"table", SPACE, HTML_EL_Table, NULL},
   {"tbody", SPACE, HTML_EL_tbody, NULL},
   {"td", SPACE, HTML_EL_Data_cell, NULL},
   {"textarea", SPACE, HTML_EL_Text_Area, NULL},
   {"tfoot", SPACE, HTML_EL_tfoot, NULL},
   {"th", SPACE, HTML_EL_Heading_cell, NULL},
   {"thead", SPACE, HTML_EL_thead, NULL},
   {"title", SPACE, HTML_EL_TITLE, NULL},
   {"tr", SPACE, HTML_EL_Table_row, NULL},
   {"tt", SPACE, HTML_EL_Teletype_text, NULL},
   {"u", SPACE, HTML_EL_Underlined_text, NULL},
   {"ul", SPACE, HTML_EL_Unnumbered_List, NULL},
   {"var", SPACE, HTML_EL_Variable, NULL},
   {"xmlgraphics", SPACE, HTML_EL_XMLGraphics, NULL},
   {"xmp", SPACE, HTML_EL_Preformatted, NULL},  /* converted to PRE */
   {"", SPACE, 0, NULL}	/* Last entry. Mandatory */
};
/* mapping table of HTML attributes */

static AttributeMapping HTMLAttributeMappingTable[] =
{
   /* The first entry MUST be unknown_attr */
   /* The rest of this table MUST be in alphabetical order */
   {"unknown_attr", "", 'A', HTML_ATTR_Invalid_attribute},

   {"abbr", "", 'A', HTML_ATTR_abbr},
   {"accept", "", 'A', HTML_ATTR_accept},
   {"accept-charset", "form", 'A', HTML_ATTR_accept_charset},
   {"accesskey", "", 'A', HTML_ATTR_accesskey},
   {"action", "", 'A', HTML_ATTR_Script_URL},
   {"align", "applet", 'A', HTML_ATTR_Alignment},
   {"align", "caption", 'A', HTML_ATTR_Position},
   {"align", "col", 'A', HTML_ATTR_Cell_align},
   {"align", "colgroup", 'A', HTML_ATTR_Cell_align},
   {"align", "div", 'A', HTML_ATTR_TextAlign},
   {"align", "h1", 'A', HTML_ATTR_TextAlign},
   {"align", "h2", 'A', HTML_ATTR_TextAlign},
   {"align", "h3", 'A', HTML_ATTR_TextAlign},
   {"align", "h4", 'A', HTML_ATTR_TextAlign},
   {"align", "h5", 'A', HTML_ATTR_TextAlign},
   {"align", "h6", 'A', HTML_ATTR_TextAlign},
   {"align", "hr", 'A', HTML_ATTR_Align},
   {"align", "iframe", 'A', HTML_ATTR_Alignment},
   {"align", "image", 'A', HTML_ATTR_Alignment},
   {"align", "img", 'A', HTML_ATTR_Alignment},
   {"align", "input", 'A', HTML_ATTR_Alignment},
   {"align", "legend", 'A', HTML_ATTR_LAlign},
   {"align", "object", 'A', HTML_ATTR_Alignment},
   {"align", "p", 'A', HTML_ATTR_TextAlign},
   {"align", "table", 'A', HTML_ATTR_Align},
   {"align", "tbody", 'A', HTML_ATTR_Cell_align},
   {"align", "td", 'A', HTML_ATTR_Cell_align},
   {"align", "tfoot", 'A', HTML_ATTR_Cell_align},
   {"align", "th", 'A', HTML_ATTR_Cell_align},
   {"align", "thead", 'A', HTML_ATTR_Cell_align},
   {"align", "tr", 'A', HTML_ATTR_Cell_align},
   {"alink", "body", 'A', HTML_ATTR_ActiveLinkColor},
   {"alt", "", 'A', HTML_ATTR_ALT},
   {"archive", "", 'A', HTML_ATTR_archive},
   {"axis", "", 'A', HTML_ATTR_axis},

   {"background", "", 'A', HTML_ATTR_background_},
   {"bgcolor", "", 'A', HTML_ATTR_BackgroundColor},
   {"border", "image", 'A', HTML_ATTR_Img_border},
   {"border", "img", 'A', HTML_ATTR_Img_border},
   {"border", "object", 'A', HTML_ATTR_Img_border},
   {"border", "table", 'A', HTML_ATTR_Border},

   {"cellspacing", "", 'A', HTML_ATTR_cellspacing},
   {"cellpadding", "", 'A', HTML_ATTR_cellpadding},
   {"char", "", 'A', HTML_ATTR_char},
   {"charoff", "", 'A', HTML_ATTR_charoff},
   {"charset", "", 'A', HTML_ATTR_charset},
   {"checked", "", 'A', HTML_ATTR_Checked},
   {"cite", "", 'A', HTML_ATTR_cite},
   {"class", "", 'A', HTML_ATTR_Class},
   {"classid", "", 'A', HTML_ATTR_classid},
   {"clear", "br", 'A', HTML_ATTR_Clear},
   {"code", "", 'A', HTML_ATTR_code},
   {"codebase", "", 'A', HTML_ATTR_codebase},
   {"codetype", "", 'A', HTML_ATTR_codetype},
   {"color", "basefont", 'A', HTML_ATTR_BaseFontColor},
   {"color", "", 'A', HTML_ATTR_color},
   {"cols", "frameset", 'A', HTML_ATTR_ColWidth},
   {"cols", "textarea", 'A', HTML_ATTR_Columns},
   {"colspan", "", 'A', HTML_ATTR_colspan_},
   {"compact", "", 'A', HTML_ATTR_COMPACT},
   {"content", "", 'A', HTML_ATTR_meta_content},
   {"coords", "", 'A', HTML_ATTR_coords},

   {"data", "", 'A', HTML_ATTR_data},
   {"datapagesize", "table", 'A', HTML_ATTR_datapagesize},
   {"datetime", "", 'A', HTML_ATTR_datetime},
   {"declare", "object", 'A', HTML_ATTR_declare},
   {"defer", "script", 'A', HTML_ATTR_defer},
   {"dir", "", 'A', HTML_ATTR_dir},
   {"disabled", "", 'A', HTML_ATTR_disabled},

   {"enctype", "", 'A', HTML_ATTR_ENCTYPE},
   {"event", "script", 'A', HTML_ATTR_event},

   {"face", "basefont", 'A', HTML_ATTR_BaseFontFace},
   {"face", "font", 'A', HTML_ATTR_face},
   {"for", "label", 'A', HTML_ATTR_Associated_control},
   {"for", "script", 'A', HTML_ATTR_for_},
   {"frame", "table", 'A', HTML_ATTR_frame},
   {"frameborder", "", 'A', HTML_ATTR_frameborder},

   {"headers", "", 'A', HTML_ATTR_headers},
   {"height", "", 'A', HTML_ATTR_Height_},
   {"href", "", 'A', HTML_ATTR_HREF_},
   {"hreflang", "", 'A', HTML_ATTR_hreflang},
   {"hspace", "", 'A', HTML_ATTR_hspace},
   {"http-equiv", "", 'A', HTML_ATTR_http_equiv},

   {"id", "", 'A', HTML_ATTR_ID},
   {"ismap", "", 'A', HTML_ATTR_ISMAP},

   {"label", "", 'A', HTML_ATTR_label},
   {"lang", "", 'A', HTML_ATTR_Langue},
   {"language", "script", 'A', HTML_ATTR_script_language},
   {"link", "body", 'A', HTML_ATTR_LinkColor},
   {"longdesc", "", 'A', HTML_ATTR_longdesc},

   {"marginheight", "", 'A', HTML_ATTR_marginheight},
   {"marginwidth", "", 'A', HTML_ATTR_marginwidth},
   {"maxlength", "", 'A', HTML_ATTR_MaxLength},
   {"media", "", 'A', HTML_ATTR_media},
   {"method", "", 'A', HTML_ATTR_METHOD},
   {"mode", "", 'A', HTML_ATTR_mode},
   {"multiple", "", 'A', HTML_ATTR_Multiple},

   {"N", "", 'C', 0},
   {"name", "applet", 'A', HTML_ATTR_applet_name},
   {"name", "frame", 'A', HTML_ATTR_FrameName},
   {"name", "iframe", 'A', HTML_ATTR_FrameName},
   {"name", "meta", 'A', HTML_ATTR_meta_name},
   {"name", "param", 'A', HTML_ATTR_Param_name},
   {"name", "", 'A', HTML_ATTR_NAME},
   {"nohref", "", 'A', HTML_ATTR_nohref},
   {"noresize", "", 'A', HTML_ATTR_no_resize},
   {"noshade", "", 'A', HTML_ATTR_NoShade},
   {"nowrap", "", 'A', HTML_ATTR_No_wrap},

   {"object", "applet", 'A', HTML_ATTR_object},
   {"onblur", "", 'A', HTML_ATTR_onblur},
   {"onchange", "", 'A', HTML_ATTR_onchange},
   {"onclick", "", 'A', HTML_ATTR_onclick},
   {"ondblclick", "", 'A', HTML_ATTR_ondblclick},
   {"onfocus", "", 'A', HTML_ATTR_onfocus},
   {"onkeydown", "", 'A', HTML_ATTR_onkeydown},
   {"onkeypress", "", 'A', HTML_ATTR_onkeypress},
   {"onkeyup", "", 'A', HTML_ATTR_onkeyup},
   {"onload", "", 'A', HTML_ATTR_onload},
   {"onmousedown", "", 'A', HTML_ATTR_onmousedown},
   {"onmousemove", "", 'A', HTML_ATTR_onmousemove},
   {"onmouseout", "", 'A', HTML_ATTR_onmouseout},
   {"onmouseover", "", 'A', HTML_ATTR_onmouseover},
   {"onmouseup", "", 'A', HTML_ATTR_onmouseup},
   {"onreset", "form", 'A', HTML_ATTR_onreset},
   {"onselect", "", 'A', HTML_ATTR_onselect},
   {"onsubmit", "form", 'A', HTML_ATTR_onsubmit},
   {"onunload", "", 'A', HTML_ATTR_onunload},

   {"profile", "head", 'A', HTML_ATTR_profile},
   {"prompt", "", 'A', HTML_ATTR_Prompt},

   {"readonly", "", 'A', HTML_ATTR_readonly},
   {"rel", "", 'A', HTML_ATTR_REL},
   {"rev", "", 'A', HTML_ATTR_REV},
   {"rows", "frameset", 'A', HTML_ATTR_RowHeight},
   {"rows", "textarea", 'A', HTML_ATTR_Rows},
   {"rowspan", "", 'A', HTML_ATTR_rowspan_},
   {"rules", "table", 'A', HTML_ATTR_rules_},

   {"scheme", "meta", 'A', HTML_ATTR_scheme},
   {"scope", "", 'A', HTML_ATTR_scope},
   {"scrolling", "", 'A', HTML_ATTR_scrolling},
   {"selected", "", 'A', HTML_ATTR_Selected},
   {"shape", "", 'A', HTML_ATTR_shape},
   {"size", "basefont", 'A', HTML_ATTR_BaseFontSize},
   {"size", "font", 'A', HTML_ATTR_Font_size},
   {"size", "hr", 'A', HTML_ATTR_Size_},
   {"size", "input", 'A', HTML_ATTR_Area_Size},
   {"size", "select", 'A', HTML_ATTR_MenuSize},
   {"span", "col", 'A', HTML_ATTR_span_},
   {"span", "colgroup", 'A', HTML_ATTR_span_},
   {"src", "frame", 'A', HTML_ATTR_FrameSrc},
   {"src", "iframe", 'A', HTML_ATTR_FrameSrc},
   {"src", "script", 'A', HTML_ATTR_script_src},
   {"src", "", 'A', HTML_ATTR_SRC},
   {"standby", "", 'A', HTML_ATTR_standby},
   {"start", "", 'A', HTML_ATTR_Start},
   {"style", "", 'A', HTML_ATTR_Style_},
   {"summary", "table", 'A', HTML_ATTR_summary},

   {"tabindex", "", 'A', HTML_ATTR_tabindex},
   {"target", "", 'A', HTML_ATTR_target_},
   {"text", "", 'A', HTML_ATTR_TextColor},
   {"title", "", 'A', HTML_ATTR_Title},
   {"type", "button", 'A', HTML_ATTR_Button_type},
   {"type", "li", 'A', HTML_ATTR_ItemStyle},
   {"type", "link", 'A', HTML_ATTR_Link_type},
   {"type", "a", 'A', HTML_ATTR_Link_type},
   {"type", "object", 'A', HTML_ATTR_Object_type},
   {"type", "ol", 'A', HTML_ATTR_NumberStyle},
   {"type", "param", 'A', HTML_ATTR_Param_type},
   {"type", "script", 'A', HTML_ATTR_content_type},
   {"type", "style", 'A', HTML_ATTR_Notation},
   {"type", "ul", 'A', HTML_ATTR_BulletStyle},
   {"type", "", SPACE, DummyAttribute},

   {"usemap", "", 'A', HTML_ATTR_USEMAP},

   {"valign", "tbody", 'A', HTML_ATTR_Row_valign},
   {"valign", "td", 'A', HTML_ATTR_Cell_valign},
   {"valign", "tfoot", 'A', HTML_ATTR_Row_valign},
   {"valign", "th", 'A', HTML_ATTR_Cell_valign},
   {"valign", "thead", 'A', HTML_ATTR_Row_valign},
   {"valign", "tr", 'A', HTML_ATTR_Row_valign},
   {"value", "li", 'A', HTML_ATTR_ItemValue},
   {"value", "param", 'A', HTML_ATTR_Param_value},
   {"value", "", 'A', HTML_ATTR_Value_},
   {"valuetype", "param", 'A', HTML_ATTR_valuetype},
   {"version", "", 'A', 0},
   {"vlink", "body", 'A', HTML_ATTR_VisitedLinkColor},
   {"vspace", "", 'A', HTML_ATTR_vspace},

   {"width", "applet", 'A', HTML_ATTR_Width__},
   {"width", "col", 'A', HTML_ATTR_Width__},
   {"width", "colgroup", 'A', HTML_ATTR_Width__},
   {"width", "hr", 'A', HTML_ATTR_Width__},
   {"width", "iframe", 'A', HTML_ATTR_Width__},
   {"width", "image", 'A', HTML_ATTR_Width__},
   {"width", "img", 'A', HTML_ATTR_Width__},
   {"width", "object", 'A', HTML_ATTR_Width__},
   {"width", "pre", 'A', HTML_ATTR_Width__},
   {"width", "table", 'A', HTML_ATTR_Width__},
   {"width", "td", 'A', HTML_ATTR_Width__},
   {"width", "th", 'A', HTML_ATTR_Width__},
#ifdef GRAPHML
   {"width", "xmlgraphics", 'A', HTML_ATTR_Width__},
#endif

   {"zzghost", "", 'A', HTML_ATTR_Ghost_restruct},
   {"", "", EOS, 0}		/* Last entry. Mandatory */
};

/* define a pointer to let other parser functions access the local table */
GIMapping    *pHTMLGIMapping = HTMLGIMappingTable;
AttributeMapping *pHTMLAttributeMapping = HTMLAttributeMappingTable;


/*----------------------------------------------------------------------
   MapGI
   search in the mapping tables the entry for the element of
   name GI and returns the rank of that entry.
   When returning, schema contains the Thot SSchema that defines that element,
   Returns -1 and schema = NULL if not found.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 MapGI (char* gi, SSchema *schema, Document doc)
#else
int                 MapGI (gi, schema, doc)
char*               gi;
SSchema*            schema;
Document            doc;
#endif
{
  ElementType     elType;
  char*           ptr; 
  char            c;
  int             i;
  int             entry;
  ThotBool	      isHTML;

  /* TODO: use NameSpaces to search in the right table */
  entry = -1;
  if (*schema == NULL)
    {
      isHTML = FALSE;
      ptr = NULL;
    }
  else
    {
      ptr = TtaGetSSchemaName (*schema);
      isHTML = !strcmp (ptr, "HTML");
    }

  i = 0;
  if (*schema == NULL || isHTML)
    {
      /*
	First convert the first char into lower case to locate
	registered tags in the HTML mapping table.
	Entries are registered in upper case and in alphabetic order.
      */

      /* TODO: define a function which works on unicode */
      c = tolower (gi[0]);
      /* look for the first concerned entry in the table */
      while (HTMLGIMappingTable[i].htmlGI[0] < c
	     && HTMLGIMappingTable[i].htmlGI[0] != EOS)
	i++;

      /* look at all entries starting with the right character */
      do
	{
	  if (strcasecmp (HTMLGIMappingTable[i].htmlGI, gi))
	    i++;
	  else
	    entry = i;
	}
      while (entry < 0 && HTMLGIMappingTable[i].htmlGI[0] == c);
    }

  if (entry < 0)
    if (*schema != NULL && isHTML)
      *schema = NULL;
    else
      /* not found. Look at the XML mapping tables */
      {
	elType.ElTypeNum = 0;
	elType.ElSSchema = *schema;

#ifndef STANDALONE
	if (!ptr || !strcmp (ptr, "MathML"))
	  MapXMLElementType (MATH_TYPE, gi, &elType, &ptr, &c, doc);
	if (elType.ElTypeNum == 0 && (!ptr || !strcmp (ptr, "GraphML")))
	  MapXMLElementType (GRAPH_TYPE, gi, &elType, &ptr, &c, doc);
#endif
	if (elType.ElTypeNum == 0)
	  {
            entry = -1;
	    elType.ElSSchema = NULL;
	    *schema = NULL;
	  }
	else
	  {
            entry = elType.ElTypeNum;
            *schema = elType.ElSSchema;
	  }
      }
  return entry;
}

/*----------------------------------------------------------------------
   GIType  search in mapping tables the Element type associated with
   a given GI Name. If not found returns zero.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GIType (char* gi, ElementType *elType, Document doc)
#else
void                GIType (gi, elType, doc)
char*               gi;
ElementType*        elType;
Document            doc;
#endif
{
  char                c;
  char*               ptr;
  int                 i;

  /* TODO: use NameSpaces to search in the right table */
  elType->ElSSchema = NULL;
  elType->ElTypeNum = 0;

  /*
    First convert the first char into lower case to locate
    registered tags in the HTML mapping table.
    Entries are registered in upper case and in alphabetic order.
  */

  /* TODO: define a function which works on unicode */
  c = tolower (gi[0]);

  i = 0;
  /* look for the first concerned entry in the table */
  while (HTMLGIMappingTable[i].htmlGI[0] < c &&
	 HTMLGIMappingTable[i].htmlGI[0] != EOS)
    i++;
  /* look at all entries starting with the right character */
  do
    {
      if (!strcasecmp (HTMLGIMappingTable[i].htmlGI, gi))
      {
	if (doc != 0)
        elType->ElSSchema = TtaGetSSchema (TEXT("HTML"), doc);
	elType->ElTypeNum = HTMLGIMappingTable[i].ThotType;
	return;
      }
      i++;
    }
  while (HTMLGIMappingTable[i].htmlGI[0] == c);

#ifndef STANDALONE
  /* if not found, look at the XML mapping tables */
  MapXMLElementType (MATH_TYPE, gi, elType, &ptr, &c, doc);
  if (elType->ElTypeNum == 0)
  MapXMLElementType (GRAPH_TYPE, gi, elType, &ptr, &c, doc);
#endif
}

/*----------------------------------------------------------------------
   MapAttr search in all AttributeMappingTables the entry for the
   attribute of name Attr and returns a pointer to that entry,
   as well as the corresponding Thot SSchema
  ----------------------------------------------------------------------*/
#ifdef __STDC__
AttributeMapping   *MapAttr (char* Attr, SSchema *schema, int elemEntry, Document doc)
#else
AttributeMapping   *MapAttr (Attr, schema, elemEntry, doc)
char*               Attr;
SSchema*            schema;
int                 elemEntry;
Document            doc;
#endif
{
  int                 i;
  int                 entry;

  entry = -1;
  *schema = NULL;
  i = 0;
  do
    if (!strcasecmp (HTMLAttributeMappingTable[i].XMLattribute, Attr))
      if (HTMLAttributeMappingTable[i].XMLelement[0] == EOS)
        {
	  entry = i;
	  *schema = TtaGetDocumentSSchema (doc);
	}
     else if (elemEntry >= 0 &&
	      !strcasecmp (HTMLAttributeMappingTable[i].XMLelement,
			    pHTMLGIMapping[elemEntry].htmlGI))
       {
	 entry = i;
	 *schema = TtaGetDocumentSSchema (doc);
       }
      else
	i++;
    else
      i++;
  while (entry < 0 && HTMLAttributeMappingTable[i].AttrOrContent != EOS);

  if (entry >= 0)
    return (&HTMLAttributeMappingTable[entry]);
  else
    return (NULL);
}

/*----------------------------------------------------------------------
   MapHTMLAttribute search in the HTML Attribute Mapping Tables the entry
   for the attribute of name Attr and returns the corresponding Thot
   attribute type.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void           MapHTMLAttribute (char* Attr, AttributeType *attrType, char* elementName, Document doc)
#else
void           MapHTMLAttribute (Attr, attrType, elementName, doc)
char*          Attr;
AttributeType* attrType;
char*          elementName;
Document       doc;
#endif
{
  SSchema	       schema = NULL;
  AttributeMapping*   tableEntry;
  int                 elemEntry;

  elemEntry = MapGI (elementName, &schema, doc);
  if (elemEntry >= 0)
    {
      tableEntry = MapAttr (Attr, &schema, elemEntry, doc);
      if (tableEntry != NULL)
	{
	  attrType->AttrTypeNum = tableEntry->ThotAttribute;
	  if (schema == NULL && doc != 0)
	    attrType->AttrSSchema = TtaGetSSchema (TEXT("HTML"), doc);
	  else	    
	    attrType->AttrSSchema = schema;
	}
      else
	{
	  attrType->AttrTypeNum = 0;
	  attrType->AttrSSchema = NULL;
	}
    }
  else
    {
      attrType->AttrTypeNum = 0;
      attrType->AttrSSchema = NULL;
    }
}
