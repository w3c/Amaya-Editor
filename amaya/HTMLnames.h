/* Mapping table of XHTML elements */
#include "HTML.h"
static ElemMapping    XHTMLElemMappingTable[] =
{
   /* This table MUST be in alphabetical order */
   {TEXT("XMLPI"), SPACE, HTML_EL_XMLPI, L_Other, FALSE},
   {TEXT("XMLPI_line"), SPACE, HTML_EL_PI_line, L_Other, FALSE},
   {TEXT("XMLcomment"), SPACE, HTML_EL_Comment_, L_Other, FALSE},
   {TEXT("XMLcomment_line"), SPACE, HTML_EL_Comment_line, L_Other, FALSE},
   {TEXT("a"), SPACE, HTML_EL_Anchor, L_Basic, TRUE},
   {TEXT("abbr"), SPACE, HTML_EL_ABBR, L_Basic, TRUE},
   {TEXT("acronym"), SPACE, HTML_EL_ACRONYM, L_Basic, TRUE},
   {TEXT("address"), SPACE, HTML_EL_Address, L_Basic, FALSE},
   {TEXT("applet"), SPACE, HTML_EL_Applet, L_Transitional, FALSE},
   {TEXT("area"), 'E', HTML_EL_AREA, L_Strict, FALSE},
   {TEXT("b"), SPACE, HTML_EL_Bold_text, L_Transitional, TRUE},
   {TEXT("base"), 'E', HTML_EL_BASE, L_Basic, FALSE},
   {TEXT("basefont"), 'E', HTML_EL_BaseFont, L_Transitional, FALSE},
   {TEXT("bdo"), SPACE, HTML_EL_BDO, L_Strict, TRUE},
   {TEXT("big"), SPACE, HTML_EL_Big_text, L_Transitional, TRUE},
   {TEXT("blockquote"), SPACE, HTML_EL_Block_Quote, L_Basic, FALSE},
   {TEXT("body"), SPACE, HTML_EL_BODY, L_Basic, FALSE},
   {TEXT("br"), 'E', HTML_EL_BR, L_Basic, TRUE},
   {TEXT("button"), SPACE, HTML_EL_BUTTON, L_Strict, TRUE},
   {TEXT("c"), SPACE, HTML_EL_TEXT_UNIT, L_Other, TRUE}, /* used by the trans.c */
   {TEXT("caption"), SPACE, HTML_EL_CAPTION, L_Basic, FALSE},
   {TEXT("center"), SPACE, HTML_EL_Center, L_Transitional, FALSE},
   {TEXT("cite"), SPACE, HTML_EL_Cite, L_Basic, TRUE},
   {TEXT("code"), SPACE, HTML_EL_Code, L_Basic, TRUE},
   {TEXT("colgroup"), SPACE, HTML_EL_COLGROUP, L_Strict, FALSE},
   {TEXT("col"), SPACE, HTML_EL_COL, L_Strict, FALSE},
   {TEXT("dd"), SPACE, HTML_EL_Definition, L_Basic, FALSE},
   {TEXT("del"), SPACE, HTML_EL_DEL, L_Strict, TRUE},
   {TEXT("dfn"), SPACE, HTML_EL_Def, L_Basic, TRUE},
   {TEXT("dir"), SPACE, HTML_EL_Directory, L_Strict, FALSE},
   {TEXT("div"), SPACE, HTML_EL_Division, L_Basic, FALSE},
   {TEXT("dl"), SPACE, HTML_EL_Definition_List, L_Basic, FALSE},
   {TEXT("dt"), SPACE, HTML_EL_Term, L_Basic, FALSE},
   {TEXT("em"), SPACE, HTML_EL_Emphasis, L_Basic, TRUE},
   {TEXT("fieldset"), SPACE, HTML_EL_FIELDSET, L_Strict, FALSE},
   {TEXT("font"), SPACE, HTML_EL_Font_, L_Transitional, TRUE},
   {TEXT("form"), SPACE, HTML_EL_Form, L_Basic, FALSE},
   {TEXT("frame"), 'E', HTML_EL_FRAME, L_Transitional, FALSE},
   {TEXT("frameset"), SPACE, HTML_EL_FRAMESET, L_Transitional, FALSE},
   {TEXT("h1"), SPACE, HTML_EL_H1, L_Basic, FALSE},
   {TEXT("h2"), SPACE, HTML_EL_H2, L_Basic, FALSE},
   {TEXT("h3"), SPACE, HTML_EL_H3, L_Basic, FALSE},
   {TEXT("h4"), SPACE, HTML_EL_H4, L_Basic, FALSE},
   {TEXT("h5"), SPACE, HTML_EL_H5, L_Basic, FALSE},
   {TEXT("h6"), SPACE, HTML_EL_H6, L_Basic, FALSE},
   {TEXT("head"), SPACE, HTML_EL_HEAD, L_Basic, FALSE},
   {TEXT("hr"), 'E', HTML_EL_Horizontal_Rule, L_Strict, FALSE},
   {TEXT("html"), SPACE, HTML_EL_HTML, L_Basic, FALSE},
   {TEXT("i"), SPACE, HTML_EL_Italic_text, L_Strict, TRUE},
   {TEXT("iframe"), SPACE, HTML_EL_IFRAME, L_Transitional, FALSE},
   {TEXT("image"), 'E', HTML_EL_PICTURE_UNIT, L_Strict, TRUE},
   {TEXT("img"), 'E', HTML_EL_PICTURE_UNIT, L_Basic, TRUE},
   {TEXT("input"), 'E', HTML_EL_Input, L_Basic, TRUE},
   {TEXT("input"), 'E', HTML_EL_Text_Input, L_Basic, TRUE},
   {TEXT("input"), 'E', HTML_EL_Password_Input, L_Basic, TRUE},
   {TEXT("input"), 'E', HTML_EL_File_Input, L_Basic, TRUE},
   {TEXT("input"), 'E', HTML_EL_Checkbox_Input, L_Basic, FALSE},
   {TEXT("input"), 'E', HTML_EL_Radio_Input, L_Basic, FALSE},
   {TEXT("input"), 'E', HTML_EL_Submit_Input, L_Basic, FALSE},
   {TEXT("input"), 'E', HTML_EL_Reset_Input, L_Basic, FALSE},
   {TEXT("input"), 'E', HTML_EL_Button_Input, L_Basic, TRUE},
   {TEXT("input"), 'E', HTML_EL_Hidden_Input, L_Basic, FALSE},
   {TEXT("ins"), SPACE, HTML_EL_INS, L_Strict, TRUE},
   {TEXT("isindex"), 'E', HTML_EL_ISINDEX, L_Transitional, FALSE},
   {TEXT("kbd"), SPACE, HTML_EL_Keyboard, L_Basic, TRUE},
   {TEXT("label"), SPACE, HTML_EL_LABEL, L_Basic, TRUE},
   {TEXT("legend"), SPACE, HTML_EL_LEGEND, L_Strict, FALSE},
   {TEXT("li"), SPACE, HTML_EL_List_Item, L_Basic, FALSE},
   {TEXT("link"), 'E', HTML_EL_LINK, L_Basic, FALSE},
   {TEXT("listing"), SPACE, HTML_EL_Preformatted, L_Transitional, FALSE}, /*converted to PRE */
   {TEXT("map"), SPACE, HTML_EL_MAP, L_Strict, FALSE},
   {TEXT("menu"), SPACE, HTML_EL_Menu, L_Transitional, FALSE},
   {TEXT("meta"), 'E', HTML_EL_META, L_Basic, FALSE},
   {TEXT("noframes"), SPACE, HTML_EL_NOFRAMES, L_Transitional, FALSE},
   {TEXT("noscript"), SPACE, HTML_EL_NOSCRIPT, L_Strict, FALSE},
   {TEXT("object"), SPACE, HTML_EL_Object, L_Basic, FALSE},
   {TEXT("ol"), SPACE, HTML_EL_Numbered_List, L_Basic, FALSE},
   {TEXT("optgroup"), SPACE, HTML_EL_OptGroup, L_Strict, TRUE},
   {TEXT("option"), SPACE, HTML_EL_Option, L_Basic, TRUE},
   {TEXT("p"), SPACE, HTML_EL_Paragraph, L_Basic, FALSE},
   {TEXT("p*"), SPACE, HTML_EL_Pseudo_paragraph, L_Basic, FALSE}, /* pseudo-paragraph */
   {TEXT("param"), 'E', HTML_EL_Parameter, L_Basic, FALSE},
   {TEXT("plaintext"), SPACE, HTML_EL_Preformatted, L_Transitional, FALSE},/* converted to PRE */
   {TEXT("pre"), SPACE, HTML_EL_Preformatted, L_Basic, FALSE},
   {TEXT("q"), SPACE, HTML_EL_Quotation, L_Basic, TRUE},
   {TEXT("s"), SPACE, HTML_EL_Struck_text, L_Strict, TRUE},
   {TEXT("samp"), SPACE, HTML_EL_Sample, L_Basic, TRUE},
   {TEXT("script"), SPACE, HTML_EL_SCRIPT, L_Strict, FALSE},
   {TEXT("select"), SPACE, HTML_EL_Option_Menu, L_Basic, TRUE},
   {TEXT("small"), SPACE, HTML_EL_Small_text, L_Strict, TRUE},
   {TEXT("span"), SPACE, HTML_EL_Span, L_Basic, TRUE},
   {TEXT("strike"), SPACE, HTML_EL_Struck_text, L_Transitional, TRUE},
   {TEXT("strong"), SPACE, HTML_EL_Strong, L_Basic, TRUE},
   {TEXT("style"), SPACE, HTML_EL_STYLE_, L_Strict, FALSE},
   {TEXT("sub"), SPACE, HTML_EL_Subscript, L_Strict, TRUE},
   {TEXT("sup"), SPACE, HTML_EL_Superscript, L_Strict, TRUE},
   {TEXT("symb"), SPACE, HTML_EL_SYMBOL_UNIT, L_Other, TRUE},
   {TEXT("table"), SPACE, HTML_EL_Table, L_Basic, FALSE},
   {TEXT("tbody"), SPACE, HTML_EL_tbody, L_Basic, FALSE},
   {TEXT("td"), SPACE, HTML_EL_Data_cell, L_Basic, FALSE},
   {TEXT("textarea"), SPACE, HTML_EL_Text_Area, L_Basic, TRUE},
   {TEXT("tfoot"), SPACE, HTML_EL_tfoot, L_Strict, FALSE},
   {TEXT("th"), SPACE, HTML_EL_Heading_cell, L_Basic, FALSE},
   {TEXT("thead"), SPACE, HTML_EL_thead, L_Strict, FALSE},
   {TEXT("title"), SPACE, HTML_EL_TITLE, L_Basic, FALSE},
   {TEXT("tr"), SPACE, HTML_EL_Table_row, L_Basic, FALSE},
   {TEXT("tt"), SPACE, HTML_EL_Teletype_text, L_Strict, TRUE},
   {TEXT("u"), SPACE, HTML_EL_Underlined_text, L_Strict, TRUE},
   {TEXT("ul"), SPACE, HTML_EL_Unnumbered_List, L_Basic, FALSE},
   {TEXT("var"), SPACE, HTML_EL_Variable, L_Basic, TRUE},
   {TEXT("xmp"), SPACE, HTML_EL_Preformatted, L_Transitional, FALSE},  /* converted to PRE */
   {TEXT(""), SPACE, 0, L_Basic, FALSE}	  /* Last entry. Mandatory */
};

/* Mapping table of Xhtml attributes */
AttributeMapping XHTMLAttributeMappingTable[] =
{
   /* The first entry MUST be unknown_attr */
   /* The rest of this table MUST be in alphabetical order */
   {TEXT("unknown_attr"), TEXT(""), 'A', HTML_ATTR_Invalid_attribute, L_Transitional},

   {TEXT("abbr"), TEXT(""), 'A', HTML_ATTR_abbr, L_Basic},
   {TEXT("accept"), TEXT(""), 'A', HTML_ATTR_accept, L_Basic},
   {TEXT("accept-charset"), TEXT("form"), 'A', HTML_ATTR_accept_charset, L_Basic},
   {TEXT("accesskey"), TEXT(""), 'A', HTML_ATTR_accesskey, L_Basic},
   {TEXT("action"), TEXT(""), 'A', HTML_ATTR_Script_URL, L_Basic},
   {TEXT("align"), TEXT("applet"), 'A', HTML_ATTR_Alignment, L_Transitional},
   {TEXT("align"), TEXT("caption"), 'A', HTML_ATTR_Position, L_Strict},
   {TEXT("align"), TEXT("col"), 'A', HTML_ATTR_Cell_align, L_Strict},
   {TEXT("align"), TEXT("colgroup"), 'A', HTML_ATTR_Cell_align, L_Strict},
   {TEXT("align"), TEXT("div"), 'A', HTML_ATTR_TextAlign, L_Transitional},
   {TEXT("align"), TEXT("h1"), 'A', HTML_ATTR_TextAlign, L_Transitional},
   {TEXT("align"), TEXT("h2"), 'A', HTML_ATTR_TextAlign, L_Transitional},
   {TEXT("align"), TEXT("h3"), 'A', HTML_ATTR_TextAlign, L_Transitional},
   {TEXT("align"), TEXT("h4"), 'A', HTML_ATTR_TextAlign, L_Transitional},
   {TEXT("align"), TEXT("h5"), 'A', HTML_ATTR_TextAlign, L_Transitional},
   {TEXT("align"), TEXT("h6"), 'A', HTML_ATTR_TextAlign, L_Transitional},
   {TEXT("align"), TEXT("hr"), 'A', HTML_ATTR_Align, L_Transitional},
   {TEXT("align"), TEXT("iframe"), 'A', HTML_ATTR_Alignment, L_Transitional},
   {TEXT("align"), TEXT("image"), 'A', HTML_ATTR_Alignment, L_Transitional},
   {TEXT("align"), TEXT("img"), 'A', HTML_ATTR_Alignment, L_Transitional},
   {TEXT("align"), TEXT("input"), 'A', HTML_ATTR_Alignment, L_Transitional},
   {TEXT("align"), TEXT("legend"), 'A', HTML_ATTR_LAlign, L_Transitional},
   {TEXT("align"), TEXT("object"), 'A', HTML_ATTR_Alignment, L_Transitional},
   {TEXT("align"), TEXT("p"), 'A', HTML_ATTR_TextAlign, L_Transitional},
   {TEXT("align"), TEXT("table"), 'A', HTML_ATTR_Align, L_Transitional},
   {TEXT("align"), TEXT("tbody"), 'A', HTML_ATTR_Cell_align, L_Strict},
   {TEXT("align"), TEXT("td"), 'A', HTML_ATTR_Cell_align, L_Strict},
   {TEXT("align"), TEXT("tfoot"), 'A', HTML_ATTR_Cell_align, L_Strict},
   {TEXT("align"), TEXT("th"), 'A', HTML_ATTR_Cell_align, L_Strict},
   {TEXT("align"), TEXT("thead"), 'A', HTML_ATTR_Cell_align, L_Strict},
   {TEXT("align"), TEXT("tr"), 'A', HTML_ATTR_Cell_align, L_Strict},
   {TEXT("alink"), TEXT("body"), 'A', HTML_ATTR_ActiveLinkColor, L_Transitional},
   {TEXT("alt"), TEXT(""), 'A', HTML_ATTR_ALT, L_Basic},
   {TEXT("archive"), TEXT(""), 'A', HTML_ATTR_archive, L_Basic},
   {TEXT("axis"), TEXT(""), 'A', HTML_ATTR_axis, L_Basic},

   {TEXT("background"), TEXT(""), 'A', HTML_ATTR_background_, L_Transitional},
   {TEXT("bgcolor"), TEXT(""), 'A', HTML_ATTR_BackgroundColor, L_Transitional},
   {TEXT("border"), TEXT("image"), 'A', HTML_ATTR_Img_border, L_Transitional},
   {TEXT("border"), TEXT("img"), 'A', HTML_ATTR_Img_border, L_Transitional},
   {TEXT("border"), TEXT("object"), 'A', HTML_ATTR_Img_border, L_Transitional},
   {TEXT("border"), TEXT("table"), 'A', HTML_ATTR_Border, L_Strict},

   {TEXT("cellspacing"), TEXT(""), 'A', HTML_ATTR_cellspacing, L_Strict},
   {TEXT("cellpadding"), TEXT(""), 'A', HTML_ATTR_cellpadding, L_Strict},
   {TEXT("char"), TEXT(""), 'A', HTML_ATTR_char, L_Transitional},
   {TEXT("charoff"), TEXT(""), 'A', HTML_ATTR_charoff, L_Transitional},
   {TEXT("charset"), TEXT(""), 'A', HTML_ATTR_charset, L_Basic},
   {TEXT("checked"), TEXT(""), 'A', HTML_ATTR_Checked, L_Basic},
   {TEXT("cite"), TEXT(""), 'A', HTML_ATTR_cite, L_Basic},
   {TEXT("class"), TEXT(""), 'A', HTML_ATTR_Class, L_Basic},
   {TEXT("classid"), TEXT(""), 'A', HTML_ATTR_classid, L_Basic},
   {TEXT("clear"), TEXT("br"), 'A', HTML_ATTR_Clear, L_Transitional},
   {TEXT("code"), TEXT(""), 'A', HTML_ATTR_code, L_Basic},
   {TEXT("codebase"), TEXT(""), 'A', HTML_ATTR_codebase, L_Basic},
   {TEXT("codetype"), TEXT(""), 'A', HTML_ATTR_codetype, L_Basic},
   {TEXT("color"), TEXT("basefont"), 'A', HTML_ATTR_BaseFontColor, L_Transitional},
   {TEXT("color"), TEXT(""), 'A', HTML_ATTR_color, L_Transitional},
   {TEXT("cols"), TEXT("frameset"), 'A', HTML_ATTR_ColWidth, L_Transitional},
   {TEXT("cols"), TEXT("textarea"), 'A', HTML_ATTR_Columns, L_Basic},
   {TEXT("colspan"), TEXT(""), 'A', HTML_ATTR_colspan_, L_Basic},
   {TEXT("compact"), TEXT(""), 'A', HTML_ATTR_COMPACT, L_Transitional},
   {TEXT("content"), TEXT(""), 'A', HTML_ATTR_meta_content, L_Basic},
   {TEXT("coords"), TEXT(""), 'A', HTML_ATTR_coords, L_Basic},

   {TEXT("data"), TEXT(""), 'A', HTML_ATTR_data, L_Basic},
   {TEXT("datapagesize"), TEXT("table"), 'A', HTML_ATTR_datapagesize, L_Basic},
   {TEXT("datetime"), TEXT(""), 'A', HTML_ATTR_datetime, L_Basic},
   {TEXT("declare"), TEXT("object"), 'A', HTML_ATTR_declare, L_Basic},
   {TEXT("defer"), TEXT("script"), 'A', HTML_ATTR_defer, L_Basic},
   {TEXT("dir"), TEXT(""), 'A', HTML_ATTR_dir, L_Basic},
   {TEXT("disabled"), TEXT(""), 'A', HTML_ATTR_disabled, L_Basic},

   {TEXT("enctype"), TEXT(""), 'A', HTML_ATTR_ENCTYPE, L_Strict},
   {TEXT("event"), TEXT("script"), 'A', HTML_ATTR_event, L_Strict},

   {TEXT("face"), TEXT("basefont"), 'A', HTML_ATTR_BaseFontFace, L_Transitional},
   {TEXT("face"), TEXT("font"), 'A', HTML_ATTR_face, L_Transitional},
   {TEXT("for"), TEXT("label"), 'A', HTML_ATTR_Associated_control, L_Strict},
   {TEXT("for"), TEXT("script"), 'A', HTML_ATTR_for_, L_Strict},
   {TEXT("frame"), TEXT("table"), 'A', HTML_ATTR_frame, L_Strict},
   {TEXT("frameborder"), TEXT(""), 'A', HTML_ATTR_frameborder, L_Transitional},

   {TEXT("headers"), TEXT(""), 'A', HTML_ATTR_headers, L_Transitional},
   {TEXT("height"), TEXT("img"), 'A', HTML_ATTR_Height_, L_Basic},
   {TEXT("height"), TEXT("object"), 'A', HTML_ATTR_Height_, L_Basic},
   {TEXT("height"), TEXT(""), 'A', HTML_ATTR_Height_, L_Transitional},
   {TEXT("href"), TEXT(""), 'A', HTML_ATTR_HREF_, L_Basic},
   {TEXT("hreflang"), TEXT(""), 'A', HTML_ATTR_hreflang, L_Basic},
   {TEXT("hspace"), TEXT(""), 'A', HTML_ATTR_hspace, L_Transitional},
   {TEXT("http-equiv"), TEXT(""), 'A', HTML_ATTR_http_equiv, L_Basic},

   {TEXT("id"), TEXT(""), 'A', HTML_ATTR_ID, L_Basic},
   {TEXT("ismap"), TEXT(""), 'A', HTML_ATTR_ISMAP, L_Strict},

   {TEXT("label"), TEXT(""), 'A', HTML_ATTR_label, L_Basic},
   {TEXT("lang"), TEXT(""), 'A', HTML_ATTR_Langue, L_Basic},
   {TEXT("language"), TEXT("script"), 'A', HTML_ATTR_script_language, L_Basic},
   {TEXT("link"), TEXT("body"), 'A', HTML_ATTR_LinkColor, L_Basic},
   {TEXT("longdesc"), TEXT(""), 'A', HTML_ATTR_longdesc, L_Basic},

   {TEXT("marginheight"), TEXT(""), 'A', HTML_ATTR_marginheight, L_Transitional},
   {TEXT("marginwidth"), TEXT(""), 'A', HTML_ATTR_marginwidth, L_Transitional},
   {TEXT("maxlength"), TEXT(""), 'A', HTML_ATTR_MaxLength, L_Basic},
   {TEXT("media"), TEXT(""), 'A', HTML_ATTR_media, L_Basic},
   {TEXT("method"), TEXT(""), 'A', HTML_ATTR_METHOD, L_Basic},
   {TEXT("multiple"), TEXT(""), 'A', HTML_ATTR_Multiple, L_Basic},

   {TEXT("name"), TEXT("applet"), 'A', HTML_ATTR_applet_name, L_Transitional},
   {TEXT("name"), TEXT("frame"), 'A', HTML_ATTR_FrameName, L_Transitional},
   {TEXT("name"), TEXT("iframe"), 'A', HTML_ATTR_FrameName, L_Transitional},
   {TEXT("name"), TEXT("meta"), 'A', HTML_ATTR_meta_name, L_Basic},
   {TEXT("name"), TEXT("param"), 'A', HTML_ATTR_Param_name, L_Basic},
   {TEXT("name"), TEXT(""), 'A', HTML_ATTR_NAME, L_Basic},
   {TEXT("nohref"), TEXT(""), 'A', HTML_ATTR_nohref, L_Basic},
   {TEXT("noresize"), TEXT(""), 'A', HTML_ATTR_no_resize, L_Basic},
   {TEXT("noshade"), TEXT(""), 'A', HTML_ATTR_NoShade, L_Basic},
   {TEXT("nowrap"), TEXT(""), 'A', HTML_ATTR_No_wrap, L_Transitional},

   {TEXT("object"), TEXT("applet"), 'A', HTML_ATTR_object, L_Transitional},
   {TEXT("onblur"), TEXT(""), 'A', HTML_ATTR_onblur, L_Strict},
   {TEXT("onchange"), TEXT(""), 'A', HTML_ATTR_onchange, L_Strict},
   {TEXT("onclick"), TEXT(""), 'A', HTML_ATTR_onclick, L_Strict},
   {TEXT("ondblclick"), TEXT(""), 'A', HTML_ATTR_ondblclick, L_Strict},
   {TEXT("onfocus"), TEXT(""), 'A', HTML_ATTR_onfocus, L_Strict},
   {TEXT("onkeydown"), TEXT(""), 'A', HTML_ATTR_onkeydown, L_Strict},
   {TEXT("onkeypress"), TEXT(""), 'A', HTML_ATTR_onkeypress, L_Strict},
   {TEXT("onkeyup"), TEXT(""), 'A', HTML_ATTR_onkeyup, L_Strict},
   {TEXT("onload"), TEXT(""), 'A', HTML_ATTR_onload, L_Strict},
   {TEXT("onmousedown"), TEXT(""), 'A', HTML_ATTR_onmousedown, L_Strict},
   {TEXT("onmousemove"), TEXT(""), 'A', HTML_ATTR_onmousemove, L_Strict},
   {TEXT("onmouseout"), TEXT(""), 'A', HTML_ATTR_onmouseout, L_Strict},
   {TEXT("onmouseover"), TEXT(""), 'A', HTML_ATTR_onmouseover, L_Strict},
   {TEXT("onmouseup"), TEXT(""), 'A', HTML_ATTR_onmouseup, L_Strict},
   {TEXT("onreset"), TEXT("form"), 'A', HTML_ATTR_onreset, L_Strict},
   {TEXT("onselect"), TEXT(""), 'A', HTML_ATTR_onselect, L_Strict},
   {TEXT("onsubmit"), TEXT("form"), 'A', HTML_ATTR_onsubmit, L_Strict},
   {TEXT("onunload"), TEXT(""), 'A', HTML_ATTR_onunload, L_Strict},

   {TEXT("profile"), TEXT("head"), 'A', HTML_ATTR_profile, L_Basic},
   {TEXT("prompt"), TEXT(""), 'A', HTML_ATTR_Prompt, L_Strict},

   {TEXT("readonly"), TEXT(""), 'A', HTML_ATTR_readonly, L_Basic},
   {TEXT("rel"), TEXT(""), 'A', HTML_ATTR_REL, L_Basic},
   {TEXT("rev"), TEXT(""), 'A', HTML_ATTR_REV, L_Basic},
   {TEXT("rows"), TEXT("frameset"), 'A', HTML_ATTR_RowHeight, L_Transitional},
   {TEXT("rows"), TEXT("textarea"), 'A', HTML_ATTR_Rows, L_Basic},
   {TEXT("rowspan"), TEXT(""), 'A', HTML_ATTR_rowspan_, L_Basic},
   {TEXT("rules"), TEXT("table"), 'A', HTML_ATTR_rules_, L_Strict},

   {TEXT("scheme"), TEXT("meta"), 'A', HTML_ATTR_scheme, L_Basic},
   {TEXT("scope"), TEXT(""), 'A', HTML_ATTR_scope, L_Basic},
   {TEXT("scrolling"), TEXT(""), 'A', HTML_ATTR_scrolling, L_Transitional},
   {TEXT("selected"), TEXT(""), 'A', HTML_ATTR_Selected, L_Basic},
   {TEXT("shape"), TEXT(""), 'A', HTML_ATTR_shape, L_Strict},
   {TEXT("size"), TEXT("basefont"), 'A', HTML_ATTR_BaseFontSize, L_Transitional},
   {TEXT("size"), TEXT("font"), 'A', HTML_ATTR_Font_size, L_Transitional},
   {TEXT("size"), TEXT("hr"), 'A', HTML_ATTR_Size_, L_Transitional},
   {TEXT("size"), TEXT("input"), 'A', HTML_ATTR_Area_Size, L_Strict},
   {TEXT("size"), TEXT("select"), 'A', HTML_ATTR_MenuSize, L_Strict},
   {TEXT("span"), TEXT("col"), 'A', HTML_ATTR_span_, L_Strict},
   {TEXT("span"), TEXT("colgroup"), 'A', HTML_ATTR_span_, L_Strict},
   {TEXT("src"), TEXT("frame"), 'A', HTML_ATTR_FrameSrc, L_Transitional},
   {TEXT("src"), TEXT("iframe"), 'A', HTML_ATTR_FrameSrc, L_Transitional},
   {TEXT("src"), TEXT("script"), 'A', HTML_ATTR_script_src, L_Strict},
   {TEXT("src"), TEXT(""), 'A', HTML_ATTR_SRC, L_Basic},
   {TEXT("standby"), TEXT(""), 'A', HTML_ATTR_standby, L_Basic},
   {TEXT("start"), TEXT(""), 'A', HTML_ATTR_Start, L_Basic},
   {TEXT("style"), TEXT(""), 'A', HTML_ATTR_Style_, L_Basic},
   {TEXT("summary"), TEXT("table"), 'A', HTML_ATTR_summary, L_Basic},

   {TEXT("tabindex"), TEXT(""), 'A', HTML_ATTR_tabindex, L_Basic},
   {TEXT("target"), TEXT(""), 'A', HTML_ATTR_target_, L_Basic},
   {TEXT("text"), TEXT(""), 'A', HTML_ATTR_TextColor, L_Basic},
   {TEXT("title"), TEXT(""), 'A', HTML_ATTR_Title, L_Basic},
   {TEXT("type"), TEXT("button"), 'A', HTML_ATTR_Button_type, L_Strict},
   {TEXT("type"), TEXT("li"), 'A', HTML_ATTR_ItemStyle, L_Basic},
   {TEXT("type"), TEXT("link"), 'A', HTML_ATTR_Link_type, L_Basic},
   {TEXT("type"), TEXT("a"), 'A', HTML_ATTR_Link_type, L_Basic},
   {TEXT("type"), TEXT("object"), 'A', HTML_ATTR_Object_type, L_Basic},
   {TEXT("type"), TEXT("ol"), 'A', HTML_ATTR_NumberStyle, L_Basic},
   {TEXT("type"), TEXT("param"), 'A', HTML_ATTR_Param_type, L_Basic},
   {TEXT("type"), TEXT("script"), 'A', HTML_ATTR_content_type, L_Strict},
   {TEXT("type"), TEXT("style"), 'A', HTML_ATTR_Notation, L_Strict},
   {TEXT("type"), TEXT("ul"), 'A', HTML_ATTR_BulletStyle, L_Basic},
   {TEXT("type"), TEXT(""), SPACE, DummyAttribute, L_Basic},

   {TEXT("usemap"), TEXT(""), 'A', HTML_ATTR_USEMAP, L_Basic},

   {TEXT("valign"), TEXT("tbody"), 'A', HTML_ATTR_Row_valign, L_Strict},
   {TEXT("valign"), TEXT("td"), 'A', HTML_ATTR_Cell_valign, L_Strict},
   {TEXT("valign"), TEXT("tfoot"), 'A', HTML_ATTR_Row_valign, L_Strict},
   {TEXT("valign"), TEXT("th"), 'A', HTML_ATTR_Cell_valign, L_Strict},
   {TEXT("valign"), TEXT("thead"), 'A', HTML_ATTR_Row_valign, L_Strict},
   {TEXT("valign"), TEXT("tr"), 'A', HTML_ATTR_Row_valign, L_Strict},
   {TEXT("value"), TEXT("li"), 'A', HTML_ATTR_ItemValue, L_Basic},
   {TEXT("value"), TEXT("param"), 'A', HTML_ATTR_Param_value, L_Basic},
   {TEXT("value"), TEXT(""), 'A', HTML_ATTR_Value_, L_Basic},
   {TEXT("valuetype"), TEXT("param"), 'A', HTML_ATTR_valuetype, L_Basic},
   {TEXT("version"), TEXT(""), 'A', 0, L_Basic},
   {TEXT("vlink"), TEXT("body"), 'A', HTML_ATTR_VisitedLinkColor, L_Transitional},
   {TEXT("vspace"), TEXT(""), 'A', HTML_ATTR_vspace, L_Transitional},

   {TEXT("width"), TEXT("applet"), 'A', HTML_ATTR_Width__, L_Transitional},
   {TEXT("width"), TEXT("col"), 'A', HTML_ATTR_Width__, L_Strict},
   {TEXT("width"), TEXT("colgroup"), 'A', HTML_ATTR_Width__, L_Strict},
   {TEXT("width"), TEXT("hr"), 'A', HTML_ATTR_Width__, L_Transitional},
   {TEXT("width"), TEXT("iframe"), 'A', HTML_ATTR_Width__, L_Transitional},
   {TEXT("width"), TEXT("image"), 'A', HTML_ATTR_Width__, L_Transitional},
   {TEXT("width"), TEXT("img"), 'A', HTML_ATTR_Width__, L_Basic},
   {TEXT("width"), TEXT("object"), 'A', HTML_ATTR_Width__, L_Basic},
   {TEXT("width"), TEXT("pre"), 'A', HTML_ATTR_Width__, L_Transitional},
   {TEXT("width"), TEXT("table"), 'A', HTML_ATTR_Width__, L_Strict},
   {TEXT("width"), TEXT("td"), 'A', HTML_ATTR_Width__, L_Transitional},
   {TEXT("width"), TEXT("th"), 'A', HTML_ATTR_Width__, L_Transitional},

   {TEXT("xml:space"), TEXT(""), 'A', HTML_ATTR_xml_space, L_Basic},

   {TEXT("zzghost"), TEXT(""), 'A', HTML_ATTR_Ghost_restruct, L_Basic},
   {TEXT(""), TEXT(""), EOS, 0, L_Other}		/* Last entry. Mandatory */
};

/* Mapping table of Xhtml entities */
XmlEntity XhtmlEntityTable[] =
{
/* This table MUST be in alphabetical order */
{TEXT("AElig"), 198, TEXT(' ')},   /* latin capital letter AE = */ 
                                   /* latin capital ligature AE, U+00C6 ISOlat1 */
{TEXT("Aacute"), 193, TEXT(' ')},  /* latin capital letter A with acute, U+00C1 ISOlat1 */
{TEXT("Acirc"), 194, TEXT(' ')},   /* latin capital letter A with circumflex, U+00C2 ISOlat1 */
{TEXT("Agrave"), 192, TEXT(' ')},  /* latin capital letter A with grave = */
                                   /* latin capital letter A grave, U+00C0 ISOlat1 */
{TEXT("Alpha"), 913, TEXT(' ')},   /* greek capital letter alpha, U+0391 */
{TEXT("Aring"), 197, TEXT(' ')},   /* latin capital letter A with ring above = */ 
                                   /* latin capital letter A ring, U+00C5 ISOlat1 */
{TEXT("Atilde"), 195, TEXT(' ')},  /* latin capital letter A with tilde, U+00C3 ISOlat1 */
{TEXT("Auml"), 196, TEXT(' ')},	   /* latin capital letter A with diaeresis, U+00C4 ISOlat1 */
{TEXT("Beta"), 914, TEXT(' ')},	   /* greek capital letter beta, U+0392 */
{TEXT("Ccedil"), 199, TEXT(' ')},  /* latin capital letter C with cedilla, U+00C7 ISOlat1 */
{TEXT("Chi"), 935, TEXT(' ')},	   /* greek capital letter chi, U+03A7 */
{TEXT("Dagger"), 8225, TEXT(' ')}, /* double dagger, U+2021 ISOpub */
{TEXT("Delta"), 916, TEXT(' ')},   /* greek capital letter delta, U+0394 ISOgrk3 */
{TEXT("ETH"), 208, TEXT(' ')},	   /* latin capital letter ETH, U+00D0 ISOlat1 */
{TEXT("Eacute"), 201, TEXT(' ')},  /* latin capital letter E with acute, U+00C9 ISOlat1 */
{TEXT("Ecirc"), 202, TEXT(' ')},   /* latin capital letter E with circumflex, U+00CA ISOlat1 */
{TEXT("Egrave"), 200, TEXT(' ')},  /* latin capital letter E with grave, U+00C8 ISOlat1 */
{TEXT("Epsilon"), 917, TEXT(' ')}, /* greek capital letter epsilon, U+0395 */
{TEXT("Eta"), 919, TEXT(' ')},	   /* greek capital letter eta, U+0397 */
{TEXT("Euml"), 203, TEXT(' ')},	   /* latin capital letter E with diaeresis, U+00CB ISOlat1 */
{TEXT("Gamma"), 915, TEXT(' ')},   /* greek capital letter gamma, U+0393 ISOgrk3 */
{TEXT("Iacute"), 205, TEXT(' ')},  /* latin capital letter I with acute, U+00CD ISOlat1 */
{TEXT("Icirc"), 206, TEXT(' ')},   /* latin capital letter I with circumflex, U+00CE ISOlat1 */
{TEXT("Igrave"), 204, TEXT(' ')},  /* latin capital letter I with grave, U+00CC ISOlat1 */
{TEXT("Iota"), 921, TEXT(' ')},    /* greek capital letter iota, U+0399 */
{TEXT("Iuml"), 207, TEXT(' ')},	   /* latin capital letter I with diaeresis, U+00CF ISOlat1 */
{TEXT("Kappa"), 922, TEXT(' ')},   /* greek capital letter kappa, U+039A */
{TEXT("Lambda"), 923, TEXT(' ')},  /* greek capital letter lambda, U+039B ISOgrk3 */
{TEXT("Mu"), 924, TEXT(' ')},	   /* greek capital letter mu, U+039C */
{TEXT("Ntilde"), 209, TEXT(' ')},  /* latin capital letter N with tilde, U+00D1 ISOlat1 */
{TEXT("Nu"), 925, TEXT(' ')},      /* greek capital letter nu, U+039D */
{TEXT("OElig"), 338, TEXT(' ')},   /* latin capital ligature OE, U+0152 ISOlat2 */
{TEXT("Oacute"), 211, TEXT(' ')},  /* latin capital letter O with acute, U+00D3 ISOlat1 */
{TEXT("Ocirc"), 212, TEXT(' ')},   /* latin capital letter O with circumflex, U+00D4 ISOlat1 */
{TEXT("Ograve"), 210, TEXT(' ')},  /* latin capital letter O with grave, U+00D2 ISOlat1 */
{TEXT("Omega"), 937, TEXT(' ')},   /* greek capital letter omega, U+03A9 ISOgrk3 */
{TEXT("Omicron"), 927, TEXT(' ')}, /* greek capital letter omicron, U+039F */
{TEXT("Oslash"), 216, TEXT(' ')},  /* latin capital letter O with stroke = */ 
                                   /* latin capital letter O slash, U+00D8 ISOlat1 */
{TEXT("Otilde"), 213, TEXT(' ')},  /* latin capital letter O with tilde, U+00D5 ISOlat1 */
{TEXT("Ouml"), 214, TEXT(' ')},    /* latin capital letter O with diaeresis, U+00D6 ISOlat1 */
{TEXT("Phi"), 934, TEXT(' ')},     /* greek capital letter phi, U+03A6 ISOgrk3 */
{TEXT("Pi"), 928, TEXT(' ')},      /* greek capital letter pi, U+03A0 ISOgrk3 */
{TEXT("Prime"), 8243, TEXT(' ')},  /* double prime = seconds = inches, U+2033 ISOtech */
{TEXT("Psi"), 936, TEXT(' ')},     /* greek capital letter psi, U+03A8 ISOgrk3 */
{TEXT("Rho"), 929, TEXT(' ')},     /* greek capital letter rho, U+03A1 */
{TEXT("Scaron"), 352, TEXT(' ')},  /* latin capital letter S with caron, U+0160 ISOlat2 */
{TEXT("Sigma"), 931, TEXT(' ')},   /* greek capital letter sigma, U+03A3 ISOgrk3 */
{TEXT("THORN"), 222, TEXT(' ')},   /* latin capital letter THORN, U+00DE ISOlat1 */
{TEXT("Tau"), 932, TEXT(' ')},     /* greek capital letter tau, U+03A4 */
{TEXT("Theta"), 920, TEXT(' ')},   /* greek capital letter theta, U+0398 ISOgrk3 */
{TEXT("Uacute"), 218, TEXT(' ')},  /* latin capital letter U with acute, U+00DA ISOlat1 */
{TEXT("Ucirc"), 219, TEXT(' ')},   /* latin capital letter U with circumflex, U+00DB ISOlat1 */
{TEXT("Ugrave"), 217, TEXT(' ')},  /* latin capital letter U with grave, U+00D9 ISOlat1 */
{TEXT("Upsilon"), 933, TEXT(' ')}, /* greek capital letter upsilon, U+03A5 ISOgrk3 */
{TEXT("Uuml"), 220, TEXT(' ')},    /* latin capital letter U with diaeresis, U+00DC ISOlat1 */
{TEXT("Xi"), 926, TEXT(' ')},      /* greek capital letter xi, U+039E ISOgrk3 */
{TEXT("Yacute"), 221, TEXT(' ')},  /* latin capital letter Y with acute, U+00DD ISOlat1 */
{TEXT("Yuml"), 376, TEXT(' ')},    /* latin capital letter Y with diaeresis, U+0178 ISOlat2 */
{TEXT("Zeta"), 918, TEXT(' ')},    /* greek capital letter zeta, U+0396 */
{TEXT("aacute"), 225, TEXT(' ')},  /* latin small letter a with acute, U+00E1 ISOlat1 */
{TEXT("acirc"), 226, TEXT(' ')},   /* latin small letter a with circumflex, U+00E2 ISOlat1 */
{TEXT("acute"), 180, TEXT(' ')},   /* acute accent = spacing acute, U+00B4 ISOdia */
{TEXT("aelig"), 230, TEXT(' ')},   /* latin small letter ae = */
                                   /* latin small ligature ae, U+00E6 ISOlat1 */
{TEXT("agrave"), 224, TEXT(' ')},  /* latin small letter a with grave = */
                                   /* latin small letter a grave, U+00E0 ISOlat1 */
{TEXT("alefsym"), 8501, TEXT(' ')}, /* alef symbol = first transfinite cardinal, U+2135 NEW */
{TEXT("alpha"), 945, TEXT(' ')},   /* greek small letter alpha, U+03B1 ISOgrk3 */
{TEXT("amp"), 38, TEXT(' ')},      /* ampersand, U+0026 ISOnum */
{TEXT("and"), 8743, TEXT(' ')},    /* logical and = wedge, U+2227 ISOtech */
{TEXT("ang"), 8736, TEXT(' ')},    /* angle, U+2220 ISOamso */
{TEXT("aring"), 229, TEXT(' ')},   /* latin small letter a with ring above = */
                                   /* latin small letter a ring, U+00E5 ISOlat1 */
{TEXT("asymp"), 8776, TEXT(' ')},  /* almost equal to = asymptotic to, U+2248 ISOamsr */
{TEXT("atilde"), 227, TEXT(' ')},  /* latin small letter a with tilde, U+00E3 ISOlat1 */
{TEXT("auml"), 228, TEXT(' ')},    /* latin small letter a with diaeresis, U+00E4 ISOlat1 */
{TEXT("bdquo"), 8222},	           /* double low-9 quotation mark, U+201E NEW */
{TEXT("beta"), 946, TEXT(' ')},    /* greek small letter beta, U+03B2 ISOgrk3 */
{TEXT("brvbar"), 166, TEXT(' ')},  /* broken bar = broken vertical bar, U+00A6 ISOnum */
{TEXT("bull"), 8226, TEXT(' ')},   /* bullet = black small circle, U+2022 ISOpub */
{TEXT("cap"), 8745, TEXT(' ')},    /* intersection = cap, U+2229 ISOtech */
{TEXT("ccedil"), 231, TEXT(' ')},  /* latin small letter c with cedilla, U+00E7 ISOlat1 */
{TEXT("cedil"), 184, TEXT(' ')},   /* cedilla = spacing cedilla, U+00B8 ISOdia */
{TEXT("cent"), 162, TEXT(' ')},    /* cent sign, U+00A2 ISOnum */
{TEXT("chi"), 967, TEXT(' ')},     /* greek small letter chi, U+03C7 ISOgrk3 */
{TEXT("circ"), 710, TEXT(' ')},    /* modifier letter circumflex accent, U+02C6 ISOpub */
{TEXT("clubs"), 9827, TEXT(' ')},  /* black club suit = shamrock, U+2663 ISOpub */
{TEXT("cong"), 8773, TEXT(' ')},   /* approximately equal to, U+2245 ISOtech */
{TEXT("copy"), 169, TEXT(' ')},    /* copyright sign, U+00A9 ISOnum */
{TEXT("crarr"), 8629, TEXT(' ')},  /* downwards arrow with corner leftwards = */
                                   /* carriage return, U+21B5 NEW */
{TEXT("cup"), 8746, TEXT(' ')},    /* union = cup, U+222A ISOtech */
{TEXT("curren"), 164, TEXT(' ')},  /* currency sign, U+00A4 ISOnum */
{TEXT("dArr"), 8659, TEXT(' ')},   /* downwards double arrow, U+21D3 ISOamsa */
{TEXT("dagger"), 8224, TEXT(' ')}, /* dagger, U+2020 ISOpub */
{TEXT("darr"), 8595, TEXT(' ')},   /* downwards arrow, U+2193 ISOnum */
{TEXT("deg"), 176, TEXT(' ')},     /* degree sign, U+00B0 ISOnum */
{TEXT("delta"), 948, TEXT(' ')},   /* greek small letter delta, U+03B4 ISOgrk3 */
{TEXT("diams"), 9830, TEXT(' ')},  /* black diamond suit, U+2666 ISOpub */
{TEXT("divide"), 247, TEXT(' ')},  /* division sign, U+00F7 ISOnum */
{TEXT("eacute"), 233, TEXT(' ')},  /* latin small letter e with acute, U+00E9 ISOlat1 */
{TEXT("ecirc"), 234, TEXT(' ')},   /* latin small letter e with circumflex, U+00EA ISOlat1 */
{TEXT("egrave"), 232, TEXT(' ')},  /* latin small letter e with grave, U+00E8 ISOlat1 */
{TEXT("empty"), 8709, TEXT(' ')},  /* empty set = null set = diameter, U+2205 ISOamso */
{TEXT("emsp"), 8195, TEXT(' ')},   /* em space, U+2003 ISOpub */
{TEXT("ensp"), 8194, TEXT(' ')},   /* en space, U+2002 ISOpub */
{TEXT("epsilon"), 949, TEXT(' ')}, /* greek small letter epsilon, U+03B5 ISOgrk3 */
{TEXT("equiv"), 8801, TEXT(' ')},  /* identical to, U+2261 ISOtech */
{TEXT("eta"), 951, TEXT(' ')},     /* greek small letter eta, U+03B7 ISOgrk3 */
{TEXT("eth"), 240, TEXT(' ')},     /* latin small letter eth, U+00F0 ISOlat1 */
{TEXT("euml"), 235, TEXT(' ')},    /* latin small letter e with diaeresis, U+00EB ISOlat1 */
{TEXT("euro"), 8364, TEXT(' ')},   /* euro sign, U+20AC NEW */
{TEXT("exist"), 8707, TEXT(' ')},  /* there exists, U+2203 ISOtech */
{TEXT("fnof"), 402, TEXT(' ')},    /* latin small f with hook = function = */
                                   /* florin, U+0192 ISOtech */
{TEXT("forall"), 8704, TEXT(' ')}, /* for all, U+2200 ISOtech */
{TEXT("frac12"), 189, TEXT(' ')},  /* vulgar fraction one half = */
                                   /*fraction one half, U+00BD ISOnum */
{TEXT("frac14"), 188, TEXT(' ')},  /* vulgar fraction one quarter = */
                                   /* fraction one quarter, U+00BC ISOnum */
{TEXT("frac34"), 190, TEXT(' ')},  /* vulgar fraction three quarters = */
                                   /* fraction three quarters, U+00BE ISOnum */
{TEXT("frasl"), 8260, TEXT(' ')},  /* fraction slash, U+2044 NEW */
{TEXT("gamma"), 947, TEXT(' ')},   /* greek small letter gamma, U+03B3 ISOgrk3 */
{TEXT("ge"), 8805, TEXT(' ')},     /* greater-than or equal to, U+2265 ISOtech */
{TEXT("gt"), 62, TEXT(' ')},       /* greater-than sign, U+003E ISOnum */
{TEXT("hArr"), 8660, TEXT(' ')},   /* left right double arrow, U+21D4 ISOamsa */
{TEXT("harr"), 8596, TEXT(' ')},   /* left right arrow, U+2194 ISOamsa */
{TEXT("hearts"), 9829, TEXT(' ')}, /* black heart suit = valentine, U+2665 ISOpub */
{TEXT("hellip"), 8230, TEXT(' ')}, /* horizontal ellipsis = three dot leader, U+2026 ISOpub */
{TEXT("hyphen"), 173, TEXT(' ')},  /* hyphen = discretionary hyphen, U+00AD ISOnum */
{TEXT("iacute"), 237, TEXT(' ')},  /* latin small letter i with acute, U+00ED ISOlat1 */
{TEXT("icirc"), 238, TEXT(' ')},   /* latin small letter i with circumflex, U+00EE ISOlat1 */
{TEXT("iexcl"), 161, TEXT(' ')},   /* inverted exclamation mark, U+00A1 ISOnum */
{TEXT("igrave"), 236, TEXT(' ')},  /* latin small letter i with grave, U+00EC ISOlat1 */
{TEXT("image"), 8465, TEXT(' ')},  /* blackletter capital I = imaginary part, U+2111 ISOamso */
{TEXT("infin"), 8734, TEXT(' ')},  /* infinity, U+221E ISOtech */
{TEXT("int"), 8747, TEXT(' ')},    /* integral, U+222B ISOtech */
{TEXT("iota"), 953, TEXT(' ')},    /* greek small letter iota, U+03B9 ISOgrk3 */
{TEXT("iquest"), 191, TEXT(' ')},  /* inverted question mark = */
                                   /* turned question mark, U+00BF ISOnum */
{TEXT("isin"), 8712, TEXT(' ')},   /* element of, U+2208 ISOtech */
{TEXT("iuml"), 239, TEXT(' ')},    /* latin small letter i with diaeresis, U+00EF ISOlat1 */
{TEXT("kappa"), 954, TEXT(' ')},   /* greek small letter kappa, U+03BA ISOgrk3 */
{TEXT("lArr"), 8656, TEXT(' ')},   /* leftwards double arrow, U+21D0 ISOtech */
{TEXT("lambda"), 955, TEXT(' ')},  /* greek small letter lambda, U+03BB ISOgrk3 */
{TEXT("lang"), 9001, TEXT(' ')},   /* left-pointing angle bracket = bra, U+2329 ISOtech */
{TEXT("laquo"), 171, TEXT(' ')},   /* left-pointing double angle quotation mark = */
                                   /* left pointing guillemet, U+00AB ISOnum */
{TEXT("larr"), 8592, TEXT(' ')},   /* leftwards arrow, U+2190 ISOnum */
{TEXT("lceil"), 8968, TEXT(' ')},  /* left ceiling = apl upstile, U+2308 ISOamsc */
{TEXT("ldquo"), 8220, TEXT(' ')},  /* left double quotation mark, U+201C ISOnum */
{TEXT("le"), 8804, TEXT(' ')},     /* less-than or equal to, U+2264 ISOtech */
{TEXT("lfloor"), 8970, TEXT(' ')}, /* left floor = apl downstile, U+230A ISOamsc */
{TEXT("lowast"), 8727, TEXT(' ')}, /* asterisk operator, U+2217 ISOtech */
{TEXT("loz"), 9674, TEXT(' ')},    /* lozenge, U+25CA ISOpub */
{TEXT("lrm"), 8206, TEXT(' ')},    /* left-to-right mark, U+200E NEW RFC 2070 */
{TEXT("lsaquo"), 8249, TEXT(' ')}, /* single left-pointing angle quotation mark, */
                                   /* U+2039 ISO proposed */
{TEXT("lsquo"), 8216, TEXT(' ')},  /* left single quotation mark, U+2018 ISOnum */
{TEXT("lt"), 60, TEXT(' ')},       /* less-than sign, U+003C ISOnum */
{TEXT("macr"), 175, TEXT(' ')},    /* macron = spacing macron = overline = APL overbar, */
                                   /* U+00AF ISOdia */
{TEXT("mdash"), 8212, TEXT(' ')},  /* em dash, U+2014 ISOpub */
{TEXT("micro"), 181, TEXT(' ')},   /* micro sign, U+00B5 ISOnum */
{TEXT("middot"), 183, TEXT(' ')},  /* middle dot = Georgian comma = */
                                   /* Greek middle dot, U+00B7 ISOnum */
{TEXT("minus"), 8722, TEXT(' ')},  /* minus sign, U+2212 ISOtech */
{TEXT("mu"), 956, TEXT(' ')},      /* greek small letter mu, U+03BC ISOgrk3 */
{TEXT("nabla"), 8711, TEXT(' ')},  /* nabla = backward difference, U+2207 ISOtech */
{TEXT("nbsp"), 160, TEXT(' ')},    /* no-break space = non-breaking space, U+00A0 ISOnum */
{TEXT("ndash"), 8211, TEXT(' ')},  /* en dash, U+2013 ISOpub */
{TEXT("ne"), 8800, TEXT(' ')},     /* not equal to, U+2260 ISOtech */
{TEXT("ni"), 8715, TEXT(' ')},     /* contains as member, U+220B ISOtech */
{TEXT("not"), 172, TEXT(' ')},     /* not sign, U+00AC ISOnum */
{TEXT("notin"), 8713, TEXT(' ')},  /* not an element of, U+2209 ISOtech */
{TEXT("nsub"), 8836, TEXT(' ')},   /* not a subset of, U+2284 ISOamsn */
{TEXT("ntilde"), 241, TEXT(' ')},  /* latin small letter n with tilde, U+00F1 ISOlat1 */
{TEXT("nu"), 957, TEXT(' ')},      /* greek small letter nu, U+03BD ISOgrk3 */
{TEXT("oacute"), 243, TEXT(' ')},  /* latin small letter o with acute, U+00F3 ISOlat1 */
{TEXT("ocirc"), 244, TEXT(' ')},   /* latin small letter o with circumflex, U+00F4 ISOlat1 */
{TEXT("oelig"), 339, TEXT(' ')},   /* latin small ligature oe, U+0153 ISOlat2 */
{TEXT("ograve"), 242, TEXT(' ')},  /* latin small letter o with grave, U+00F2 ISOlat1 */
{TEXT("oline"), 8254, TEXT(' ')},  /* overline = spacing overscore, U+203E NEW */
{TEXT("omega"), 969, TEXT(' ')},   /* greek small letter omega, U+03C9 ISOgrk3 */
{TEXT("omicron"), 959, TEXT(' ')}, /* greek small letter omicron, U+03BF NEW */
{TEXT("oplus"), 8853, TEXT(' ')},  /* circled plus = direct sum, U+2295 ISOamsb */
{TEXT("or"), 8744, TEXT(' ')},     /* logical or = vee, U+2228 ISOtech */
{TEXT("ordf"), 170, TEXT(' ')},    /* feminine ordinal indicator, U+00AA ISOnum */
{TEXT("ordm"), 186, TEXT(' ')},    /* masculine ordinal indicator, U+00BA ISOnum */
{TEXT("oslash"), 248, TEXT(' ')},  /* latin small letter o with stroke, = */
                                   /* latin small letter o slash, U+00F8 ISOlat1 */
{TEXT("otilde"), 245, TEXT(' ')},  /* latin small letter o with tilde, U+00F5 ISOlat1 */
{TEXT("otimes"), 8855, TEXT(' ')}, /* circled times = vector product, U+2297 ISOamsb */
{TEXT("ouml"), 246, TEXT(' ')},    /* latin small letter o with diaeresis, U+00F6 ISOlat1 */
{TEXT("para"), 182, TEXT(' ')},    /* pilcrow sign = paragraph sign, U+00B6 ISOnum */
{TEXT("part"), 8706, TEXT(' ')},   /* partial differential, U+2202 ISOtech */
{TEXT("permil"), 8240, TEXT(' ')}, /* per mille sign, U+2030 ISOtech */
{TEXT("perp"), 8869, TEXT(' ')},   /* up tack = orthogonal to = perpendicular, U+22A5 ISOtech */
{TEXT("phi"), 966, TEXT(' ')},     /* greek small letter phi, U+03C6 ISOgrk3 */
{TEXT("pi"), 960, TEXT(' ')},      /* greek small letter pi, U+03C0 ISOgrk3 */
{TEXT("piv"), 982, TEXT(' ')},     /* greek pi symbol, U+03D6 ISOgrk3 */
{TEXT("plusmn"), 177, TEXT(' ')},  /* plus-minus sign = plus-or-minus sign, U+00B1 ISOnum */
{TEXT("pound"), 163, TEXT(' ')},   /* pound sign, U+00A3 ISOnum */
{TEXT("prime"), 8242, TEXT(' ')},  /* prime = minutes = feet, U+2032 ISOtech */
{TEXT("prod"), 8719, TEXT(' ')},   /* n-ary product = product sign, U+220F ISOamsb */
{TEXT("prop"), 8733, TEXT(' ')},   /* proportional to, U+221D ISOtech */
{TEXT("psi"), 968, TEXT(' ')},     /* greek small letter psi, U+03C8 ISOgrk3 */
{TEXT("quot"), 34, TEXT(' ')},     /* quotation mark = APL quote, U+0022 ISOnum */
{TEXT("rArr"), 8658, TEXT(' ')},   /* rightwards double arrow, U+21D2 ISOtech */
{TEXT("radic"), 8730, TEXT(' ')},  /* square root = radical sign, U+221A ISOtech */
{TEXT("rang"), 9002, TEXT(' ')},   /* right-pointing angle bracket = ket, U+232A ISOtech */
{TEXT("raquo"), 187, TEXT(' ')},   /* right-pointing double angle quotation mark = */
                                   /* right pointing guillemet, U+00BB ISOnum */
{TEXT("rarr"), 8594, TEXT(' ')},   /* rightwards arrow, U+2192 ISOnum */
{TEXT("rceil"), 8969, TEXT(' ')},  /* right ceiling, U+2309 ISOamsc */
{TEXT("rdquo"), 8221, TEXT(' ')},  /* right double quotation mark, U+201D ISOnum */
{TEXT("real"), 8476, TEXT(' ')},   /* blackletter capital R = real part symbol, U+211C ISOamso */
{TEXT("reg"), 174, TEXT(' ')},     /* registered sign = registered trade mark sign, */
                                   /* U+00AE ISOnum */
{TEXT("rfloor"), 8971, TEXT(' ')}, /* right floor, U+230B ISOamsc */
{TEXT("rho"), 961, TEXT(' ')},     /* greek small letter rho, U+03C1 ISOgrk3 */
{TEXT("rlm"), 8207, TEXT(' ')},    /* right-to-left mark, U+200F NEW RFC 2070 */
{TEXT("rsaquo"), 8250, TEXT(' ')}, /* single right-pointing angle quotation mark, */
                                   /* U+203A ISO proposed */
{TEXT("rsquo"), 8217, TEXT(' ')},  /* right single quotation mark, U+2019 ISOnum */
{TEXT("sbquo"), 8218, TEXT(' ')},  /* single low-9 quotation mark, U+201A NEW */
{TEXT("scaron"), 353, TEXT(' ')},  /* latin small letter s with caron, U+0161 ISOlat2 */
{TEXT("sdot"), 8901, TEXT(' ')},   /* dot operator, U+22C5 ISOamsb */
{TEXT("sect"), 167, TEXT(' ')},    /* section sign, U+00A7 ISOnum */
{TEXT("shy"), 173, TEXT(' ')},     /* soft hyphen = discretionary hyphen, U+00AD ISOnum */
{TEXT("sigma"), 963, TEXT(' ')},   /* greek small letter sigma, U+03C3 ISOgrk3 */
{TEXT("sigmaf"), 962, TEXT(' ')},  /* greek small letter final sigma, U+03C2 ISOgrk3 */
{TEXT("sim"), 8764, TEXT(' ')},    /* tilde operator = varies with = similar to, U+223C ISOtech */
{TEXT("spades"), 9824, TEXT(' ')}, /* black spade suit, U+2660 ISOpub */
{TEXT("sub"), 8834, TEXT(' ')},    /* subset of, U+2282 ISOtech */
{TEXT("sube"), 8838, TEXT(' ')},   /* subset of or equal to, U+2286 ISOtech */
{TEXT("sum"), 8721, TEXT(' ')},    /* n-ary sumation, U+2211 ISOamsb */
{TEXT("sup"), 8835, TEXT(' ')},    /* superset of, U+2283 ISOtech */
{TEXT("sup1"), 185, TEXT(' ')},    /* superscript one = superscript digit one, U+00B9 ISOnum */
{TEXT("sup2"), 178, TEXT(' ')},    /* superscript two = superscript digit two = squared, */
                                   /* U+00B2 ISOnum */
{TEXT("sup3"), 179, TEXT(' ')},    /* superscript three = superscript digit three = cubed, */
                                   /* U+00B3 ISOnum */
{TEXT("supe"), 8839, TEXT(' ')},   /* superset of or equal to, U+2287 ISOtech */
{TEXT("szlig"), 223, TEXT(' ')},   /* latin small letter sharp s = ess-zed, U+00DF ISOlat1 */
{TEXT("tau"), 964, TEXT(' ')},     /* greek small letter tau, U+03C4 ISOgrk3 */
{TEXT("there4"), 8756, TEXT(' ')}, /* therefore, U+2234 ISOtech */
{TEXT("theta"), 952, TEXT(' ')},   /* greek small letter theta, U+03B8 ISOgrk3 */
{TEXT("thetasym"), 977, TEXT(' ')}, /* greek small letter theta symbol, U+03D1 NEW */
{TEXT("thinsp"), 8201, TEXT(' ')}, /* thin space, U+2009 ISOpub */
{TEXT("thorn"), 254, TEXT(' ')},   /* latin small letter thorn with, U+00FE ISOlat1 */
{TEXT("tilde"), 732, TEXT(' ')},   /* small tilde, U+02DC ISOdia */
{TEXT("times"), 215, TEXT(' ')},   /* multiplication sign, U+00D7 ISOnum */
{TEXT("trade"), 8482, TEXT(' ')},  /* trade mark sign, U+2122 ISOnum */
{TEXT("uArr"), 8657, TEXT(' ')},   /* upwards double arrow, U+21D1 ISOamsa */
{TEXT("uacute"), 250, TEXT(' ')},  /* latin small letter u with acute, U+00FA ISOlat1 */
{TEXT("uarr"), 8593, TEXT(' ')},   /* upwards arrow, U+2191 ISOnum*/
{TEXT("ucirc"), 251, TEXT(' ')},   /* latin small letter u with circumflex, U+00FB ISOlat1 */
{TEXT("ugrave"), 249, TEXT(' ')},  /* latin small letter u with grave, U+00F9 ISOlat1 */
{TEXT("uml"), 168, TEXT(' ')},     /* diaeresis = spacing diaeresis, U+00A8 ISOdia */
{TEXT("upsih"), 978, TEXT(' ')},   /* greek upsilon with hook symbol, U+03D2 NEW */
{TEXT("upsilon"), 965, TEXT(' ')}, /* greek small letter upsilon, U+03C5 ISOgrk3 */
{TEXT("uuml"), 252, TEXT(' ')},    /* latin small letter u with diaeresis, U+00FC ISOlat1 */
{TEXT("weierp"), 8472, TEXT(' ')}, /* script capital P = power set = Weierstrass p, */
                                   /* U+2118 ISOamso */
{TEXT("xi"), 958, TEXT(' ')},      /* greek small letter xi, U+03BE ISOgrk3 */
{TEXT("yacute"), 253, TEXT(' ')},  /* latin small letter y with acute, U+00FD ISOlat1 */
{TEXT("yen"), 165, TEXT(' ')},     /* yen sign = yuan sign, U+00A5 ISOnum */
{TEXT("yuml"), 255, TEXT(' ')},    /* latin small letter y with diaeresis, U+00FF ISOlat1 */
{TEXT("zeta"), 950, TEXT(' ')},    /* greek small letter zeta, U+03B6 ISOgrk3 */
{TEXT("zwj"), 8205, TEXT(' ')},    /* zero width joiner, U+200D NEW RFC 2070 */
{TEXT("zwnj"), 8204, TEXT(' ')},   /* zero width non-joiner, U+200C NEW RFC 2070 */
{TEXT("zzzz"), 0, TEXT(' ')}	   /* this last entry is required */
};
