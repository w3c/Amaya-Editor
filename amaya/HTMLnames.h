/* Mapping table of XHTML elements */
#include "HTML.h"
static ElemMapping    XHTMLElemMappingTable[] =
{
   /* This table MUST be in alphabetical order */
 
   {"XMLPI", SPACE, HTML_EL_XMLPI, L_Other, FALSE},
   {"XMLPI_line", SPACE, HTML_EL_PI_line, L_Other, FALSE},
   {"XMLcomment", SPACE, HTML_EL_Comment_, L_Other, FALSE},
   {"XMLcomment_line", SPACE, HTML_EL_Comment_line, L_Other, FALSE},
   {"a", SPACE, HTML_EL_Anchor, L_Basic, TRUE},
   {"abbr", SPACE, HTML_EL_ABBR, L_Basic, TRUE},
   {"acronym", SPACE, HTML_EL_ACRONYM, L_Basic, TRUE},
   {"address", SPACE, HTML_EL_Address, L_Basic, FALSE},
   {"applet", SPACE, HTML_EL_Applet, L_Transitional, FALSE},
   {"area", 'E', HTML_EL_AREA, L_Strict, FALSE},
   {"b", SPACE, HTML_EL_Bold_text, L_Transitional, TRUE},
   {"base", 'E', HTML_EL_BASE, L_Basic, FALSE},
   {"basefont", 'E', HTML_EL_BaseFont, L_Transitional, FALSE},
   {"bdo", SPACE, HTML_EL_BDO, L_Strict, TRUE},
   {"big", SPACE, HTML_EL_Big_text, L_Transitional, TRUE},
   {"blockquote", SPACE, HTML_EL_Block_Quote, L_Basic, FALSE},
   {"body", SPACE, HTML_EL_BODY, L_Basic, FALSE},
   {"br", 'E', HTML_EL_BR, L_Basic, TRUE},
   {"button", SPACE, HTML_EL_BUTTON, L_Strict, TRUE},
   {"c", SPACE, HTML_EL_TEXT_UNIT, L_Other, TRUE}, /* used by the trans.c */
   {"caption", SPACE, HTML_EL_CAPTION, L_Basic, FALSE},
   {"center", SPACE, HTML_EL_Center, L_Transitional, FALSE},
   {"cite", SPACE, HTML_EL_Cite, L_Basic, TRUE},
   {"code", SPACE, HTML_EL_Code, L_Basic, TRUE},
   {"colgroup", SPACE, HTML_EL_COLGROUP, L_Strict, FALSE},
   {"col", SPACE, HTML_EL_COL, L_Strict, FALSE},
   {"dd", SPACE, HTML_EL_Definition, L_Basic, FALSE},
   {"del", SPACE, HTML_EL_DEL, L_Strict, TRUE},
   {"dfn", SPACE, HTML_EL_Def, L_Basic, TRUE},
   {"dir", SPACE, HTML_EL_Directory, L_Strict, FALSE},
   {"div", SPACE, HTML_EL_Division, L_Basic, FALSE},
   {"dl", SPACE, HTML_EL_Definition_List, L_Basic, FALSE},
   {"dt", SPACE, HTML_EL_Term, L_Basic, FALSE},
   {"em", SPACE, HTML_EL_Emphasis, L_Basic, TRUE},
   {"fieldset", SPACE, HTML_EL_FIELDSET, L_Strict, FALSE},
   {"font", SPACE, HTML_EL_Font_, L_Transitional, TRUE},
   {"form", SPACE, HTML_EL_Form, L_Basic, FALSE},
   {"frame", 'E', HTML_EL_FRAME, L_Transitional, FALSE},
   {"frameset", SPACE, HTML_EL_FRAMESET, L_Transitional, FALSE},
   {"h1", SPACE, HTML_EL_H1, L_Basic, FALSE},
   {"h2", SPACE, HTML_EL_H2, L_Basic, FALSE},
   {"h3", SPACE, HTML_EL_H3, L_Basic, FALSE},
   {"h4", SPACE, HTML_EL_H4, L_Basic, FALSE},
   {"h5", SPACE, HTML_EL_H5, L_Basic, FALSE},
   {"h6", SPACE, HTML_EL_H6, L_Basic, FALSE},
   {"head", SPACE, HTML_EL_HEAD, L_Basic, FALSE},
   {"hr", 'E', HTML_EL_Horizontal_Rule, L_Strict, FALSE},
   {"html", SPACE, HTML_EL_HTML, L_Basic, FALSE},
   {"i", SPACE, HTML_EL_Italic_text, L_Strict, TRUE},
   {"iframe", SPACE, HTML_EL_IFRAME, L_Transitional, FALSE},
   {"image", 'E', HTML_EL_PICTURE_UNIT, L_Strict, TRUE},
   {"img", 'E', HTML_EL_PICTURE_UNIT, L_Basic, TRUE},
   {"input", 'E', HTML_EL_Input, L_Basic, TRUE},
   {"input", 'E', HTML_EL_Text_Input, L_Basic, TRUE},
   {"input", 'E', HTML_EL_Password_Input, L_Basic, TRUE},
   {"input", 'E', HTML_EL_File_Input, L_Basic, TRUE},
   {"input", 'E', HTML_EL_Checkbox_Input, L_Basic, FALSE},
   {"input", 'E', HTML_EL_Radio_Input, L_Basic, FALSE},
   {"input", 'E', HTML_EL_Submit_Input, L_Basic, FALSE},
   {"input", 'E', HTML_EL_Reset_Input, L_Basic, FALSE},
   {"input", 'E', HTML_EL_Button_Input, L_Basic, TRUE},
   {"input", 'E', HTML_EL_Hidden_Input, L_Basic, FALSE},
   {"ins", SPACE, HTML_EL_INS, L_Strict, TRUE},
   {"isindex", 'E', HTML_EL_ISINDEX, L_Transitional, FALSE},
   {"kbd", SPACE, HTML_EL_Keyboard, L_Basic, TRUE},
   {"label", SPACE, HTML_EL_LABEL, L_Basic, TRUE},
   {"legend", SPACE, HTML_EL_LEGEND, L_Strict, FALSE},
   {"li", SPACE, HTML_EL_List_Item, L_Basic, FALSE},
   {"link", 'E', HTML_EL_LINK, L_Basic, FALSE},
   {"listing", SPACE, HTML_EL_Preformatted, L_Transitional, FALSE}, /*converted to PRE */
   {"map", SPACE, HTML_EL_MAP, L_Strict, FALSE},
   {"menu", SPACE, HTML_EL_Menu, L_Transitional, FALSE},
   {"meta", 'E', HTML_EL_META, L_Basic, FALSE},
   {"noframes", SPACE, HTML_EL_NOFRAMES, L_Transitional, FALSE},
   {"noscript", SPACE, HTML_EL_NOSCRIPT, L_Strict, FALSE},
   {"object", SPACE, HTML_EL_Object, L_Basic, FALSE},
   {"ol", SPACE, HTML_EL_Numbered_List, L_Basic, FALSE},
   {"optgroup", SPACE, HTML_EL_OptGroup, L_Strict, TRUE},
   {"option", SPACE, HTML_EL_Option, L_Basic, TRUE},
   {"p", SPACE, HTML_EL_Paragraph, L_Basic, FALSE},
   {"p*", SPACE, HTML_EL_Pseudo_paragraph, L_Basic, FALSE}, /* pseudo-paragraph */
   {"param", 'E', HTML_EL_Parameter, L_Basic, FALSE},
   {"plaintext", SPACE, HTML_EL_Preformatted, L_Transitional, FALSE},/* converted to PRE */
   {"pre", SPACE, HTML_EL_Preformatted, L_Basic, FALSE},
   {"q", SPACE, HTML_EL_Quotation, L_Basic, TRUE},
   {"s", SPACE, HTML_EL_Struck_text, L_Strict, TRUE},
   {"samp", SPACE, HTML_EL_Sample, L_Basic, TRUE},
   {"script", SPACE, HTML_EL_SCRIPT, L_Strict, FALSE},
   {"select", SPACE, HTML_EL_Option_Menu, L_Basic, TRUE},
   {"small", SPACE, HTML_EL_Small_text, L_Strict, TRUE},
   {"span", SPACE, HTML_EL_Span, L_Basic, TRUE},
   {"strike", SPACE, HTML_EL_Struck_text, L_Transitional, TRUE},
   {"strong", SPACE, HTML_EL_Strong, L_Basic, TRUE},
   {"style", SPACE, HTML_EL_STYLE_, L_Strict, FALSE},
   {"sub", SPACE, HTML_EL_Subscript, L_Strict, TRUE},
   {"sup", SPACE, HTML_EL_Superscript, L_Strict, TRUE},
   {"symb", SPACE, HTML_EL_SYMBOL_UNIT, L_Other, TRUE},
   {"table", SPACE, HTML_EL_Table, L_Basic, FALSE},
   {"tbody", SPACE, HTML_EL_tbody, L_Basic, FALSE},
   {"td", SPACE, HTML_EL_Data_cell, L_Basic, FALSE},
   {"textarea", SPACE, HTML_EL_Text_Area, L_Basic, TRUE},
   {"tfoot", SPACE, HTML_EL_tfoot, L_Strict, FALSE},
   {"th", SPACE, HTML_EL_Heading_cell, L_Basic, FALSE},
   {"thead", SPACE, HTML_EL_thead, L_Strict, FALSE},
   {"title", SPACE, HTML_EL_TITLE, L_Basic, FALSE},
   {"tr", SPACE, HTML_EL_Table_row, L_Basic, FALSE},
   {"tt", SPACE, HTML_EL_Teletype_text, L_Strict, TRUE},
   {"u", SPACE, HTML_EL_Underlined_text, L_Strict, TRUE},
   {"ul", SPACE, HTML_EL_Unnumbered_List, L_Basic, FALSE},
   {"var", SPACE, HTML_EL_Variable, L_Basic, TRUE},
   {"xmp", SPACE, HTML_EL_Preformatted, L_Transitional, FALSE},  /* converted to PRE */
   {"", SPACE, 0, L_Basic, FALSE}	  /* Last entry. Mandatory */
};

/* Mapping table of Xhtml attributes */
AttributeMapping XHTMLAttributeMappingTable[] =
{
   /* The first entry MUST be unknown_attr */
   /* The rest of this table MUST be in alphabetical order */
   {"unknown_attr", "", 'A', HTML_ATTR_Invalid_attribute, L_Transitional},

   {"abbr", "", 'A', HTML_ATTR_abbr, L_Basic},
   {"accept", "", 'A', HTML_ATTR_accept, L_Basic},
   {"accept-charset", "form", 'A', HTML_ATTR_accept_charset, L_Basic},
   {"accesskey", "", 'A', HTML_ATTR_accesskey, L_Basic},
   {"action", "", 'A', HTML_ATTR_Script_URL, L_Basic},
   {"align", "applet", 'A', HTML_ATTR_Alignment, L_Transitional},
   {"align", "caption", 'A', HTML_ATTR_Position, L_Strict},
   {"align", "col", 'A', HTML_ATTR_Cell_align, L_Strict},
   {"align", "colgroup", 'A', HTML_ATTR_Cell_align, L_Strict},
   {"align", "div", 'A', HTML_ATTR_TextAlign, L_Transitional},
   {"align", "h1", 'A', HTML_ATTR_TextAlign, L_Transitional},
   {"align", "h2", 'A', HTML_ATTR_TextAlign, L_Transitional},
   {"align", "h3", 'A', HTML_ATTR_TextAlign, L_Transitional},
   {"align", "h4", 'A', HTML_ATTR_TextAlign, L_Transitional},
   {"align", "h5", 'A', HTML_ATTR_TextAlign, L_Transitional},
   {"align", "h6", 'A', HTML_ATTR_TextAlign, L_Transitional},
   {"align", "hr", 'A', HTML_ATTR_Align, L_Transitional},
   {"align", "iframe", 'A', HTML_ATTR_Alignment, L_Transitional},
   {"align", "image", 'A', HTML_ATTR_Alignment, L_Transitional},
   {"align", "img", 'A', HTML_ATTR_Alignment, L_Transitional},
   {"align", "input", 'A', HTML_ATTR_Alignment, L_Transitional},
   {"align", "legend", 'A', HTML_ATTR_LAlign, L_Transitional},
   {"align", "object", 'A', HTML_ATTR_Alignment, L_Transitional},
   {"align", "p", 'A', HTML_ATTR_TextAlign, L_Transitional},
   {"align", "table", 'A', HTML_ATTR_Align, L_Transitional},
   {"align", "tbody", 'A', HTML_ATTR_Cell_align, L_Strict},
   {"align", "td", 'A', HTML_ATTR_Cell_align, L_Strict},
   {"align", "tfoot", 'A', HTML_ATTR_Cell_align, L_Strict},
   {"align", "th", 'A', HTML_ATTR_Cell_align, L_Strict},
   {"align", "thead", 'A', HTML_ATTR_Cell_align, L_Strict},
   {"align", "tr", 'A', HTML_ATTR_Cell_align, L_Strict},
   {"alink", "body", 'A', HTML_ATTR_ActiveLinkColor, L_Transitional},
   {"alt", "", 'A', HTML_ATTR_ALT, L_Basic},
   {"archive", "", 'A', HTML_ATTR_archive, L_Basic},
   {"axis", "", 'A', HTML_ATTR_axis, L_Basic},

   {"background", "", 'A', HTML_ATTR_background_, L_Transitional},
   {"bgcolor", "", 'A', HTML_ATTR_BackgroundColor, L_Transitional},
   {"border", "image", 'A', HTML_ATTR_Img_border, L_Transitional},
   {"border", "img", 'A', HTML_ATTR_Img_border, L_Transitional},
   {"border", "object", 'A', HTML_ATTR_Img_border, L_Transitional},
   {"border", "table", 'A', HTML_ATTR_Border, L_Strict},

   {"cellspacing", "", 'A', HTML_ATTR_cellspacing, L_Strict},
   {"cellpadding", "", 'A', HTML_ATTR_cellpadding, L_Strict},
   {"char", "", 'A', HTML_ATTR_char, L_Transitional},
   {"charoff", "", 'A', HTML_ATTR_charoff, L_Transitional},
   {"charset", "", 'A', HTML_ATTR_charset, L_Basic},
   {"checked", "", 'A', HTML_ATTR_Checked, L_Basic},
   {"cite", "", 'A', HTML_ATTR_cite, L_Basic},
   {"class", "", 'A', HTML_ATTR_Class, L_Basic},
   {"classid", "", 'A', HTML_ATTR_classid, L_Basic},
   {"clear", "br", 'A', HTML_ATTR_Clear, L_Transitional},
   {"code", "", 'A', HTML_ATTR_code, L_Basic},
   {"codebase", "", 'A', HTML_ATTR_codebase, L_Basic},
   {"codetype", "", 'A', HTML_ATTR_codetype, L_Basic},
   {"color", "basefont", 'A', HTML_ATTR_BaseFontColor, L_Transitional},
   {"color", "", 'A', HTML_ATTR_color, L_Transitional},
   {"cols", "frameset", 'A', HTML_ATTR_ColWidth, L_Transitional},
   {"cols", "textarea", 'A', HTML_ATTR_Columns, L_Basic},
   {"colspan", "", 'A', HTML_ATTR_colspan_, L_Basic},
   {"compact", "", 'A', HTML_ATTR_COMPACT, L_Transitional},
   {"content", "", 'A', HTML_ATTR_meta_content, L_Basic},
   {"coords", "", 'A', HTML_ATTR_coords, L_Basic},

   {"data", "", 'A', HTML_ATTR_data, L_Basic},
   {"datapagesize", "table", 'A', HTML_ATTR_datapagesize, L_Basic},
   {"datetime", "", 'A', HTML_ATTR_datetime, L_Basic},
   {"declare", "object", 'A', HTML_ATTR_declare, L_Basic},
   {"defer", "script", 'A', HTML_ATTR_defer, L_Basic},
   {"dir", "", 'A', HTML_ATTR_dir, L_Basic},
   {"disabled", "", 'A', HTML_ATTR_disabled, L_Basic},

   {"enctype", "", 'A', HTML_ATTR_ENCTYPE, L_Strict},
   {"event", "script", 'A', HTML_ATTR_event, L_Strict},

   {"face", "basefont", 'A', HTML_ATTR_BaseFontFace, L_Transitional},
   {"face", "font", 'A', HTML_ATTR_face, L_Transitional},
   {"for", "label", 'A', HTML_ATTR_Associated_control, L_Strict},
   {"for", "script", 'A', HTML_ATTR_for_, L_Strict},
   {"frame", "table", 'A', HTML_ATTR_frame, L_Strict},
   {"frameborder", "", 'A', HTML_ATTR_frameborder, L_Transitional},

   {"headers", "", 'A', HTML_ATTR_headers, L_Transitional},
   {"height", "img", 'A', HTML_ATTR_Height_, L_Basic},
   {"height", "object", 'A', HTML_ATTR_Height_, L_Basic},
   {"height", "", 'A', HTML_ATTR_Height_, L_Transitional},
   {"href", "", 'A', HTML_ATTR_HREF_, L_Basic},
   {"hreflang", "", 'A', HTML_ATTR_hreflang, L_Basic},
   {"hspace", "", 'A', HTML_ATTR_hspace, L_Transitional},
   {"http-equiv", "", 'A', HTML_ATTR_http_equiv, L_Basic},

   {"id", "", 'A', HTML_ATTR_ID, L_Basic},
   {"ismap", "", 'A', HTML_ATTR_ISMAP, L_Strict},

   {"label", "", 'A', HTML_ATTR_label, L_Basic},
   {"lang", "", 'A', HTML_ATTR_Langue, L_Basic},
   {"language", "script", 'A', HTML_ATTR_script_language, L_Basic},
   {"link", "body", 'A', HTML_ATTR_LinkColor, L_Basic},
   {"longdesc", "", 'A', HTML_ATTR_longdesc, L_Basic},

   {"marginheight", "", 'A', HTML_ATTR_marginheight, L_Transitional},
   {"marginwidth", "", 'A', HTML_ATTR_marginwidth, L_Transitional},
   {"maxlength", "", 'A', HTML_ATTR_MaxLength, L_Basic},
   {"media", "", 'A', HTML_ATTR_media, L_Basic},
   {"method", "", 'A', HTML_ATTR_METHOD, L_Basic},
   {"multiple", "", 'A', HTML_ATTR_Multiple, L_Basic},

   {"name", "applet", 'A', HTML_ATTR_applet_name, L_Transitional},
   {"name", "frame", 'A', HTML_ATTR_FrameName, L_Transitional},
   {"name", "iframe", 'A', HTML_ATTR_FrameName, L_Transitional},
   {"name", "meta", 'A', HTML_ATTR_meta_name, L_Basic},
   {"name", "param", 'A', HTML_ATTR_Param_name, L_Basic},
   {"name", "", 'A', HTML_ATTR_NAME, L_Basic},
   {"nohref", "", 'A', HTML_ATTR_nohref, L_Basic},
   {"noresize", "", 'A', HTML_ATTR_no_resize, L_Basic},
   {"noshade", "", 'A', HTML_ATTR_NoShade, L_Basic},
   {"nowrap", "", 'A', HTML_ATTR_No_wrap, L_Transitional},

   {"object", "applet", 'A', HTML_ATTR_object, L_Transitional},
   {"onblur", "", 'A', HTML_ATTR_onblur, L_Strict},
   {"onchange", "", 'A', HTML_ATTR_onchange, L_Strict},
   {"onclick", "", 'A', HTML_ATTR_onclick, L_Strict},
   {"ondblclick", "", 'A', HTML_ATTR_ondblclick, L_Strict},
   {"onfocus", "", 'A', HTML_ATTR_onfocus, L_Strict},
   {"onkeydown", "", 'A', HTML_ATTR_onkeydown, L_Strict},
   {"onkeypress", "", 'A', HTML_ATTR_onkeypress, L_Strict},
   {"onkeyup", "", 'A', HTML_ATTR_onkeyup, L_Strict},
   {"onload", "", 'A', HTML_ATTR_onload, L_Strict},
   {"onmousedown", "", 'A', HTML_ATTR_onmousedown, L_Strict},
   {"onmousemove", "", 'A', HTML_ATTR_onmousemove, L_Strict},
   {"onmouseout", "", 'A', HTML_ATTR_onmouseout, L_Strict},
   {"onmouseover", "", 'A', HTML_ATTR_onmouseover, L_Strict},
   {"onmouseup", "", 'A', HTML_ATTR_onmouseup, L_Strict},
   {"onreset", "form", 'A', HTML_ATTR_onreset, L_Strict},
   {"onselect", "", 'A', HTML_ATTR_onselect, L_Strict},
   {"onsubmit", "form", 'A', HTML_ATTR_onsubmit, L_Strict},
   {"onunload", "", 'A', HTML_ATTR_onunload, L_Strict},

   {"profile", "head", 'A', HTML_ATTR_profile, L_Basic},
   {"prompt", "", 'A', HTML_ATTR_Prompt, L_Strict},

   {"readonly", "", 'A', HTML_ATTR_readonly, L_Basic},
   {"rel", "", 'A', HTML_ATTR_REL, L_Basic},
   {"rev", "", 'A', HTML_ATTR_REV, L_Basic},
   {"rows", "frameset", 'A', HTML_ATTR_RowHeight, L_Transitional},
   {"rows", "textarea", 'A', HTML_ATTR_Rows, L_Basic},
   {"rowspan", "", 'A', HTML_ATTR_rowspan_, L_Basic},
   {"rules", "table", 'A', HTML_ATTR_rules_, L_Strict},

   {"scheme", "meta", 'A', HTML_ATTR_scheme, L_Basic},
   {"scope", "", 'A', HTML_ATTR_scope, L_Basic},
   {"scrolling", "", 'A', HTML_ATTR_scrolling, L_Transitional},
   {"selected", "", 'A', HTML_ATTR_Selected, L_Basic},
   {"shape", "", 'A', HTML_ATTR_shape, L_Strict},
   {"size", "basefont", 'A', HTML_ATTR_BaseFontSize, L_Transitional},
   {"size", "font", 'A', HTML_ATTR_Font_size, L_Transitional},
   {"size", "hr", 'A', HTML_ATTR_Size_, L_Transitional},
   {"size", "input", 'A', HTML_ATTR_Area_Size, L_Strict},
   {"size", "select", 'A', HTML_ATTR_MenuSize, L_Strict},
   {"span", "col", 'A', HTML_ATTR_span_, L_Strict},
   {"span", "colgroup", 'A', HTML_ATTR_span_, L_Strict},
   {"src", "frame", 'A', HTML_ATTR_FrameSrc, L_Transitional},
   {"src", "iframe", 'A', HTML_ATTR_FrameSrc, L_Transitional},
   {"src", "script", 'A', HTML_ATTR_script_src, L_Strict},
   {"src", "", 'A', HTML_ATTR_SRC, L_Basic},
   {"standby", "", 'A', HTML_ATTR_standby, L_Basic},
   {"start", "", 'A', HTML_ATTR_Start, L_Basic},
   {"style", "", 'A', HTML_ATTR_Style_, L_Basic},
   {"summary", "table", 'A', HTML_ATTR_summary, L_Basic},

   {"tabindex", "", 'A', HTML_ATTR_tabindex, L_Basic},
   {"target", "", 'A', HTML_ATTR_target_, L_Basic},
   {"text", "", 'A', HTML_ATTR_TextColor, L_Basic},
   {"title", "", 'A', HTML_ATTR_Title, L_Basic},
   {"type", "button", 'A', HTML_ATTR_Button_type, L_Strict},
   {"type", "li", 'A', HTML_ATTR_ItemStyle, L_Basic},
   {"type", "link", 'A', HTML_ATTR_Link_type, L_Basic},
   {"type", "a", 'A', HTML_ATTR_Link_type, L_Basic},
   {"type", "object", 'A', HTML_ATTR_Object_type, L_Basic},
   {"type", "ol", 'A', HTML_ATTR_NumberStyle, L_Basic},
   {"type", "param", 'A', HTML_ATTR_Param_type, L_Basic},
   {"type", "script", 'A', HTML_ATTR_content_type, L_Strict},
   {"type", "style", 'A', HTML_ATTR_Notation, L_Strict},
   {"type", "ul", 'A', HTML_ATTR_BulletStyle, L_Basic},
   {"type", "", SPACE, DummyAttribute, L_Basic},

   {"usemap", "", 'A', HTML_ATTR_USEMAP, L_Basic},

   {"valign", "tbody", 'A', HTML_ATTR_Row_valign, L_Strict},
   {"valign", "td", 'A', HTML_ATTR_Cell_valign, L_Strict},
   {"valign", "tfoot", 'A', HTML_ATTR_Row_valign, L_Strict},
   {"valign", "th", 'A', HTML_ATTR_Cell_valign, L_Strict},
   {"valign", "thead", 'A', HTML_ATTR_Row_valign, L_Strict},
   {"valign", "tr", 'A', HTML_ATTR_Row_valign, L_Strict},
   {"value", "li", 'A', HTML_ATTR_ItemValue, L_Basic},
   {"value", "param", 'A', HTML_ATTR_Param_value, L_Basic},
   {"value", "", 'A', HTML_ATTR_Value_, L_Basic},
   {"valuetype", "param", 'A', HTML_ATTR_valuetype, L_Basic},
   {"version", "", 'A', 0, L_Basic},
   {"vlink", "body", 'A', HTML_ATTR_VisitedLinkColor, L_Transitional},
   {"vspace", "", 'A', HTML_ATTR_vspace, L_Transitional},

   {"width", "applet", 'A', HTML_ATTR_Width__, L_Transitional},
   {"width", "col", 'A', HTML_ATTR_Width__, L_Strict},
   {"width", "colgroup", 'A', HTML_ATTR_Width__, L_Strict},
   {"width", "hr", 'A', HTML_ATTR_Width__, L_Transitional},
   {"width", "iframe", 'A', HTML_ATTR_Width__, L_Transitional},
   {"width", "image", 'A', HTML_ATTR_Width__, L_Transitional},
   {"width", "img", 'A', HTML_ATTR_Width__, L_Basic},
   {"width", "object", 'A', HTML_ATTR_Width__, L_Basic},
   {"width", "pre", 'A', HTML_ATTR_Width__, L_Transitional},
   {"width", "table", 'A', HTML_ATTR_Width__, L_Strict},
   {"width", "td", 'A', HTML_ATTR_Width__, L_Transitional},
   {"width", "th", 'A', HTML_ATTR_Width__, L_Transitional},

   {"xml:space", "", 'A', HTML_ATTR_xml_space, L_Basic},

   {"zzghost", "", 'A', HTML_ATTR_Ghost_restruct, L_Basic},
   {"", "", EOS, 0, L_Other}		/* Last entry. Mandatory */
};

/* Mapping table of Xhtml entities */
XmlEntity XhtmlEntityTable[] =
{
/* This table MUST be in alphabetical order */
{"AElig", 198},   /* latin capital letter AE = */ 
                        /* latin capital ligature AE, U+00C6 ISOlat1 */
{"Aacute", 193},  /* latin capital letter A with acute, U+00C1 ISOlat1 */
{"Acirc", 194},   /* latin capital letter A with circumflex, U+00C2 ISOlat1 */
{"Agrave", 192},  /* latin capital letter A with grave = */
                        /* latin capital letter A grave, U+00C0 ISOlat1 */
{"Alpha", 913},   /* greek capital letter alpha, U+0391 */
{"Aring", 197},   /* latin capital letter A with ring above = */ 
                        /* latin capital letter A ring, U+00C5 ISOlat1 */
{"Atilde", 195},  /* latin capital letter A with tilde, U+00C3 ISOlat1 */
{"Auml", 196},    /* latin capital letter A with diaeresis, U+00C4 ISOlat1 */
{"Beta", 914},    /* greek capital letter beta, U+0392 */
{"Ccedil", 199},  /* latin capital letter C with cedilla, U+00C7 ISOlat1 */
{"Chi", 935},     /* greek capital letter chi, U+03A7 */
{"Dagger", 8225}, /* double dagger, U+2021 ISOpub */
{"Delta", 916},   /* greek capital letter delta, U+0394 ISOgrk3 */
{"ETH", 208},	/* latin capital letter ETH, U+00D0 ISOlat1 */
{"Eacute", 201},  /* latin capital letter E with acute, U+00C9 ISOlat1 */
{"Ecirc", 202},   /* latin capital letter E with circumflex, U+00CA ISOlat1 */
{"Egrave", 200},  /* latin capital letter E with grave, U+00C8 ISOlat1 */
{"Epsilon", 917}, /* greek capital letter epsilon, U+0395 */
{"Eta", 919},     /* greek capital letter eta, U+0397 */
{"Euml", 203},    /* latin capital letter E with diaeresis, U+00CB ISOlat1 */
{"Gamma", 915},   /* greek capital letter gamma, U+0393 ISOgrk3 */
{"Iacute", 205},  /* latin capital letter I with acute, U+00CD ISOlat1 */
{"Icirc", 206},   /* latin capital letter I with circumflex, U+00CE ISOlat1 */
{"Igrave", 204},  /* latin capital letter I with grave, U+00CC ISOlat1 */
{"Iota", 921},    /* greek capital letter iota, U+0399 */
{"Iuml", 207},    /* latin capital letter I with diaeresis, U+00CF ISOlat1 */
{"Kappa", 922},   /* greek capital letter kappa, U+039A */
{"Lambda", 923},  /* greek capital letter lambda, U+039B ISOgrk3 */
{"Mu", 924},      /* greek capital letter mu, U+039C */
{"Ntilde", 209},  /* latin capital letter N with tilde, U+00D1 ISOlat1 */
{"Nu", 925},      /* greek capital letter nu, U+039D */
{"OElig", 338},   /* latin capital ligature OE, U+0152 ISOlat2 */
{"Oacute", 211},  /* latin capital letter O with acute, U+00D3 ISOlat1 */
{"Ocirc", 212},   /* latin capital letter O with circumflex, U+00D4 ISOlat1 */
{"Ograve", 210},  /* latin capital letter O with grave, U+00D2 ISOlat1 */
{"Omega", 937},   /* greek capital letter omega, U+03A9 ISOgrk3 */
{"Omicron", 927}, /* greek capital letter omicron, U+039F */
{"Oslash", 216},  /* latin capital letter O with stroke = */ 
                        /* latin capital letter O slash, U+00D8 ISOlat1 */
{"Otilde", 213},  /* latin capital letter O with tilde, U+00D5 ISOlat1 */
{"Ouml", 214},    /* latin capital letter O with diaeresis, U+00D6 ISOlat1 */
{"Phi", 934},     /* greek capital letter phi, U+03A6 ISOgrk3 */
{"Pi", 928},      /* greek capital letter pi, U+03A0 ISOgrk3 */
{"Prime", 8243},  /* double prime = seconds = inches, U+2033 ISOtech */
{"Psi", 936},     /* greek capital letter psi, U+03A8 ISOgrk3 */
{"Rho", 929},     /* greek capital letter rho, U+03A1 */
{"Scaron", 352},  /* latin capital letter S with caron, U+0160 ISOlat2 */
{"Sigma", 931},   /* greek capital letter sigma, U+03A3 ISOgrk3 */
{"THORN", 222},   /* latin capital letter THORN, U+00DE ISOlat1 */
{"Tau", 932},     /* greek capital letter tau, U+03A4 */
{"Theta", 920},   /* greek capital letter theta, U+0398 ISOgrk3 */
{"Uacute", 218},  /* latin capital letter U with acute, U+00DA ISOlat1 */
{"Ucirc", 219},   /* latin capital letter U with circumflex, U+00DB ISOlat1 */
{"Ugrave", 217},  /* latin capital letter U with grave, U+00D9 ISOlat1 */
{"Upsilon", 933}, /* greek capital letter upsilon, U+03A5 ISOgrk3 */
{"Uuml", 220},    /* latin capital letter U with diaeresis, U+00DC ISOlat1 */
{"Xi", 926},      /* greek capital letter xi, U+039E ISOgrk3 */
{"Yacute", 221},  /* latin capital letter Y with acute, U+00DD ISOlat1 */
{"Yuml", 376},    /* latin capital letter Y with diaeresis, U+0178 ISOlat2 */
{"Zeta", 918},    /* greek capital letter zeta, U+0396 */
{"aacute", 225},  /* latin small letter a with acute, U+00E1 ISOlat1 */
{"acirc", 226},   /* latin small letter a with circumflex, U+00E2 ISOlat1 */
{"acute", 180},   /* acute accent = spacing acute, U+00B4 ISOdia */
{"aelig", 230},   /* latin small letter ae = */
                        /* latin small ligature ae, U+00E6 ISOlat1 */
{"agrave", 224},  /* latin small letter a with grave = */
                        /* latin small letter a grave, U+00E0 ISOlat1 */
{"alefsym", 8501},/* alef symbol = first transfinite cardinal, U+2135 NEW */
{"alpha", 945},   /* greek small letter alpha, U+03B1 ISOgrk3 */
{"amp", 38},      /* ampersand, U+0026 ISOnum */
{"and", 8743},    /* logical and = wedge, U+2227 ISOtech */
{"ang", 8736},    /* angle, U+2220 ISOamso */
{"aring", 229},   /* latin small letter a with ring above = */
                        /* latin small letter a ring, U+00E5 ISOlat1 */
{"asymp", 8776},  /* almost equal to = asymptotic to, U+2248 ISOamsr */
{"atilde", 227},  /* latin small letter a with tilde, U+00E3 ISOlat1 */
{"auml", 228},    /* latin small letter a with diaeresis, U+00E4 ISOlat1 */
{"bdquo", 8222},	/* double low-9 quotation mark, U+201E NEW */
{"beta", 946},    /* greek small letter beta, U+03B2 ISOgrk3 */
{"brvbar", 166},  /* broken bar = broken vertical bar, U+00A6 ISOnum */
{"bull", 8226},   /* bullet = black small circle, U+2022 ISOpub */
{"cap", 8745},    /* intersection = cap, U+2229 ISOtech */
{"ccedil", 231},  /* latin small letter c with cedilla, U+00E7 ISOlat1 */
{"cedil", 184},   /* cedilla = spacing cedilla, U+00B8 ISOdia */
{"cent", 162},    /* cent sign, U+00A2 ISOnum */
{"chi", 967},     /* greek small letter chi, U+03C7 ISOgrk3 */
{"circ", 710},    /* modifier letter circumflex accent, U+02C6 ISOpub */
{"clubs", 9827},  /* black club suit = shamrock, U+2663 ISOpub */
{"cong", 8773},   /* approximately equal to, U+2245 ISOtech */
{"copy", 169},    /* copyright sign, U+00A9 ISOnum */
{"crarr", 8629},  /* downwards arrow with corner leftwards = */
                        /* carriage return, U+21B5 NEW */
{"cup", 8746},    /* union cup, U+222A ISOtech */
{"curren", 164},  /* currency sign, U+00A4 ISOnum */
{"dArr", 8659},   /* downwards double arrow, U+21D3 ISOamsa */
{"dagger", 8224}, /* dagger, U+2020 ISOpub */
{"darr", 8595},   /* downwards arrow, U+2193 ISOnum */
{"deg", 176},     /* degree sign, U+00B0 ISOnum */
{"delta", 948},   /* greek small letter delta, U+03B4 ISOgrk3 */
{"diams", 9830},  /* black diamond suit, U+2666 ISOpub */
{"divide", 247},  /* division sign, U+00F7 ISOnum */
{"eacute", 233},  /* latin small letter e with acute, U+00E9 ISOlat1 */
{"ecirc", 234},   /* latin small letter e with circumflex, U+00EA ISOlat1 */
{"egrave", 232},  /* latin small letter e with grave, U+00E8 ISOlat1 */
{"empty", 8709},  /* empty set = null set = diameter, U+2205 ISOamso */
{"emsp", 8195},   /* em space, U+2003 ISOpub */
{"ensp", 8194},   /* en space, U+2002 ISOpub */
{"epsilon", 949}, /* greek small letter epsilon, U+03B5 ISOgrk3 */
{"equiv", 8801},  /* identical to, U+2261 ISOtech */
{"eta", 951},     /* greek small letter eta, U+03B7 ISOgrk3 */
{"eth", 240},     /* latin small letter eth, U+00F0 ISOlat1 */
{"euml", 235},    /* latin small letter e with diaeresis, U+00EB ISOlat1 */
{"euro", 8364},   /* euro sign, U+20AC NEW */
{"exist", 8707},  /* there exists, U+2203 ISOtech */
{"fnof", 402},    /* latin small f with hook = function = */
                        /* florin, U+0192 ISOtech */
{"forall", 8704}, /* for all, U+2200 ISOtech */
{"frac12", 189},  /* vulgar fraction one half = */
                        /*fraction one half, U+00BD ISOnum */
{"frac14", 188},  /* vulgar fraction one quarter = */
                        /* fraction one quarter, U+00BC ISOnum */
{"frac34", 190},  /* vulgar fraction three quarters = */
                        /* fraction three quarters, U+00BE ISOnum */
{"frasl", 8260},  /* fraction slash, U+2044 NEW */
{"gamma", 947},   /* greek small letter gamma, U+03B3 ISOgrk3 */
{"ge", 8805},     /* greater-than or equal to, U+2265 ISOtech */
{"gt", 62},       /* greater-than sign, U+003E ISOnum */
{"hArr", 8660},   /* left right double arrow, U+21D4 ISOamsa */
{"harr", 8596},   /* left right arrow, U+2194 ISOamsa */
{"hearts", 9829}, /* black heart suit = valentine, U+2665 ISOpub */
{"hellip", 8230}, /* horizontal ellipsis = three dot leader, U+2026 ISOpub */
{"hyphen", 173},  /* hyphen = discretionary hyphen, U+00AD ISOnum */
{"iacute", 237},  /* latin small letter i with acute, U+00ED ISOlat1 */
{"icirc", 238},   /* latin small letter i with circumflex, U+00EE ISOlat1 */
{"iexcl", 161},   /* inverted exclamation mark, U+00A1 ISOnum */
{"igrave", 236},  /* latin small letter i with grave, U+00EC ISOlat1 */
{"image", 8465},  /* blackletter capital I = imaginary part, U+2111 ISOamso */
{"infin", 8734},  /* infinity, U+221E ISOtech */
{"int", 8747},    /* integral, U+222B ISOtech */
{"iota", 953},    /* greek small letter iota, U+03B9 ISOgrk3 */
{"iquest", 191},  /* inverted question mark = */
                        /* turned question mark, U+00BF ISOnum */
{"isin", 8712},   /* element of, U+2208 ISOtech */
{"iuml", 239},    /* latin small letter i with diaeresis, U+00EF ISOlat1 */
{"kappa", 954},   /* greek small letter kappa, U+03BA ISOgrk3 */
{"lArr", 8656},   /* leftwards double arrow, U+21D0 ISOtech */
{"lambda", 955},  /* greek small letter lambda, U+03BB ISOgrk3 */
{"lang", 9001},   /* left-pointing angle bracket = bra, U+2329 ISOtech */
{"laquo", 171},   /* left-pointing double angle quotation mark = */
                        /* left pointing guillemet, U+00AB ISOnum */
{"larr", 8592},   /* leftwards arrow, U+2190 ISOnum */
{"lceil", 8968},  /* left ceiling = apl upstile, U+2308 ISOamsc */
{"ldquo", 8220},  /* left double quotation mark, U+201C ISOnum */
{"le", 8804},     /* less-than or equal to, U+2264 ISOtech */
{"lfloor", 8970}, /* left floor = apl downstile, U+230A ISOamsc */
{"lowast", 8727}, /* asterisk operator, U+2217 ISOtech */
{"loz", 9674},    /* lozenge, U+25CA ISOpub */
{"lrm", 8206},    /* left-to-right mark, U+200E NEW RFC 2070 */
{"lsaquo", 8249}, /* single left-pointing angle quotation mark, */
                        /* U+2039 ISO proposed */
{"lsquo", 8216},  /* left single quotation mark, U+2018 ISOnum */
{"lt", 60},       /* less-than sign, U+003C ISOnum */
{"macr", 175},    /* macron = spacing macron = overline = APL overbar, */
                        /* U+00AF ISOdia */
{"mdash", 8212},  /* em dash, U+2014 ISOpub */
{"micro", 181},   /* micro sign, U+00B5 ISOnum */
{"middot", 183},  /* middle dot = Georgian comma = */
                        /* Greek middle dot, U+00B7 ISOnum */
{"minus", 8722},  /* minus sign, U+2212 ISOtech */
{"mu", 956},      /* greek small letter mu, U+03BC ISOgrk3 */
{"nabla", 8711},  /* nabla = backward difference, U+2207 ISOtech */
{"nbsp", 160},    /* no-break space = non-breaking space, U+00A0 ISOnum */
{"ndash", 8211},  /* en dash, U+2013 ISOpub */
{"ne", 8800},     /* not equal to, U+2260 ISOtech */
{"ni", 8715},     /* contains as member, U+220B ISOtech */
{"not", 172},     /* not sign, U+00AC ISOnum */
{"notin", 8713},  /* not an element of, U+2209 ISOtech */
{"nsub", 8836},   /* not a subset of, U+2284 ISOamsn */
{"ntilde", 241},  /* latin small letter n with tilde, U+00F1 ISOlat1 */
{"nu", 957},      /* greek small letter nu, U+03BD ISOgrk3 */
{"oacute", 243},  /* latin small letter o with acute, U+00F3 ISOlat1 */
{"ocirc", 244},   /* latin small letter o with circumflex, U+00F4 ISOlat1 */
{"oelig", 339},   /* latin small ligature oe, U+0153 ISOlat2 */
{"ograve", 242},  /* latin small letter o with grave, U+00F2 ISOlat1 */
{"oline", 8254},  /* overline = spacing overscore, U+203E NEW */
{"omega", 969},   /* greek small letter omega, U+03C9 ISOgrk3 */
{"omicron", 959}, /* greek small letter omicron, U+03BF NEW */
{"oplus", 8853},  /* circled plus = direct sum, U+2295 ISOamsb */
{"or", 8744},     /* logical or = vee, U+2228 ISOtech */
{"ordf", 170},    /* feminine ordinal indicator, U+00AA ISOnum */
{"ordm", 186},    /* masculine ordinal indicator, U+00BA ISOnum */
{"oslash", 248},  /* latin small letter o with stroke, = */
                        /* latin small letter o slash, U+00F8 ISOlat1 */
{"otilde", 245},  /* latin small letter o with tilde, U+00F5 ISOlat1 */
{"otimes", 8855}, /* circled times = vector product, U+2297 ISOamsb */
{"ouml", 246},    /* latin small letter o with diaeresis, U+00F6 ISOlat1 */
{"para", 182},    /* pilcrow sign = paragraph sign, U+00B6 ISOnum */
{"part", 8706},   /* partial differential, U+2202 ISOtech */
{"permil", 8240}, /* per mille sign, U+2030 ISOtech */
{"perp", 8869},   /* up tack = orthogonal to = perpendicular, U+22A5 ISOtech */
{"phi", 966},     /* greek small letter phi, U+03C6 ISOgrk3 */
{"pi", 960},      /* greek small letter pi, U+03C0 ISOgrk3 */
{"piv", 982},     /* greek pi symbol, U+03D6 ISOgrk3 */
{"plusmn", 177},  /* plus-minus sign = plus-or-minus sign, U+00B1 ISOnum */
{"pound", 163},   /* pound sign, U+00A3 ISOnum */
{"prime", 8242},  /* prime = minutes = feet, U+2032 ISOtech */
{"prod", 8719},   /* n-ary product = product sign, U+220F ISOamsb */
{"prop", 8733},   /* proportional to, U+221D ISOtech */
{"psi", 968},     /* greek small letter psi, U+03C8 ISOgrk3 */
{"quot", 34},     /* quotation mark = APL quote, U+0022 ISOnum */
{"rArr", 8658},   /* rightwards double arrow, U+21D2 ISOtech */
{"radic", 8730},  /* square root = radical sign, U+221A ISOtech */
{"rang", 9002},   /* right-pointing angle bracket = ket, U+232A ISOtech */
{"raquo", 187},   /* right-pointing double angle quotation mark = */
                        /* right pointing guillemet, U+00BB ISOnum */
{"rarr", 8594},   /* rightwards arrow, U+2192 ISOnum */
{"rceil", 8969},  /* right ceiling, U+2309 ISOamsc */
{"rdquo", 8221},  /* right double quotation mark, U+201D ISOnum */
{"real", 8476},   /* blackletter capital R = real part symbol, U+211C ISOamso */
{"reg", 174},     /* registered sign = registered trade mark sign, */
                        /* U+00AE ISOnum */
{"rfloor", 8971}, /* right floor, U+230B ISOamsc */
{"rho", 961},     /* greek small letter rho, U+03C1 ISOgrk3 */
{"rlm", 8207},    /* right-to-left mark, U+200F NEW RFC 2070 */
{"rsaquo", 8250}, /* single right-pointing angle quotation mark, */
                        /* U+203A ISO proposed */
{"rsquo", 8217},  /* right single quotation mark, U+2019 ISOnum */
{"sbquo", 8218},  /* single low-9 quotation mark, U+201A NEW */
{"scaron", 353},  /* latin small letter s with caron, U+0161 ISOlat2 */
{"sdot", 8901},   /* dot operator, U+22C5 ISOamsb */
{"sect", 167},    /* section sign, U+00A7 ISOnum */
{"shy", 173},     /* soft hyphen = discretionary hyphen, U+00AD ISOnum */
{"sigma", 963},   /* greek small letter sigma, U+03C3 ISOgrk3 */
{"sigmaf", 962},  /* greek small letter final sigma, U+03C2 ISOgrk3 */
{"sim", 8764},    /* tilde operator = varies with = similar to, U+223C ISOtech */
{"spades", 9824}, /* black spade suit, U+2660 ISOpub */
{"sub", 8834},    /* subset of, U+2282 ISOtech */
{"sube", 8838},   /* subset of or equal to, U+2286 ISOtech */
{"sum", 8721},    /* n-ary sumation, U+2211 ISOamsb */
{"sup", 8835},    /* superset of, U+2283 ISOtech */
{"sup1", 185},    /* superscript one = superscript digit one, U+00B9 ISOnum */
{"sup2", 178},    /* superscript two = superscript digit two = squared, */
                        /* U+00B2 ISOnum */
{"sup3", 179},    /* superscript three = superscript digit three = cubed, */
                        /* U+00B3 ISOnum */
{"supe", 8839},   /* superset of or equal to, U+2287 ISOtech */
{"szlig", 223},   /* latin small letter sharp s = ess-zed, U+00DF ISOlat1 */
{"tau", 964},     /* greek small letter tau, U+03C4 ISOgrk3 */
{"there4", 8756}, /* therefore, U+2234 ISOtech */
{"theta", 952},   /* greek small letter theta, U+03B8 ISOgrk3 */
{"thetasym", 977},/* greek small letter theta symbol, U+03D1 NEW */
{"thinsp", 8201}, /* thin space, U+2009 ISOpub */
{"thorn", 254},   /* latin small letter thorn with, U+00FE ISOlat1 */
{"tilde", 732},   /* small tilde, U+02DC ISOdia */
{"times", 215},   /* multiplication sign, U+00D7 ISOnum */
{"trade", 8482},  /* trade mark sign, U+2122 ISOnum */
{"uArr", 8657},   /* upwards double arrow, U+21D1 ISOamsa */
{"uacute", 250},  /* latin small letter u with acute, U+00FA ISOlat1 */
{"uarr", 8593},   /* upwards arrow, U+2191 ISOnum*/
{"ucirc", 251},   /* latin small letter u with circumflex, U+00FB ISOlat1 */
{"ugrave", 249}, /* latin small letter u with grave, U+00F9 ISOlat1 */
{"uml", 168},     /* diaeresis = spacing diaeresis, U+00A8 ISOdia */
{"upsih", 978},   /* greek upsilon with hook symbol, U+03D2 NEW */
{"upsilon", 965}, /* greek small letter upsilon, U+03C5 ISOgrk3 */
{"uuml", 252},    /* latin small letter u with diaeresis, U+00FC ISOlat1 */
{"weierp", 8472}, /* script capital P = power set = Weierstrass p, */
                        /* U+2118 ISOamso */
{"xi", 958},      /* greek small letter xi, U+03BE ISOgrk3 */
{"yacute", 253},  /* latin small letter y with acute, U+00FD ISOlat1 */
{"yen", 165},     /* yen sign = yuan sign, U+00A5 ISOnum */
{"yuml", 255},    /* latin small letter y with diaeresis, U+00FF ISOlat1 */
{"zeta", 950},    /* greek small letter zeta, U+03B6 ISOgrk3 */
{"zwj", 8205},    /* zero width joiner, U+200D NEW RFC 2070 */
{"zwnj", 8204},   /* zero width non-joiner, U+200C NEW RFC 2070 */
{"zzzz", 0}	/* this last entry is required */
};
