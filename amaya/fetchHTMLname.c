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
   {TEXT ("a"), SPACE, HTML_EL_Anchor, NULL},
   {TEXT ("abbr"), SPACE, HTML_EL_ABBR, NULL},
   {TEXT ("acronym"), SPACE, HTML_EL_ACRONYM, NULL},
   {TEXT ("address"), SPACE, HTML_EL_Address, NULL},
   {TEXT ("applet"), SPACE, HTML_EL_Applet, NULL},
   {TEXT ("area"), TEXT ('E'), HTML_EL_AREA, NULL},
   {TEXT ("b"), SPACE, HTML_EL_Bold_text, NULL},
   {TEXT ("base"), TEXT ('E'), HTML_EL_BASE, NULL},
   {TEXT ("basefont"), TEXT ('E'), HTML_EL_BaseFont, NULL},
   {TEXT ("bdo"), SPACE, HTML_EL_BDO, NULL},
   {TEXT ("big"), SPACE, HTML_EL_Big_text, NULL},
   {TEXT ("blockquote"), SPACE, HTML_EL_Block_Quote, NULL},
   {TEXT ("body"), SPACE, HTML_EL_BODY, NULL},
   {TEXT ("br"), TEXT ('E'), HTML_EL_BR, NULL},
   {TEXT ("button"), SPACE, HTML_EL_BUTTON, NULL},
   {TEXT ("c"), SPACE, HTML_EL_TEXT_UNIT, NULL},
   {TEXT ("caption"), SPACE, HTML_EL_CAPTION, NULL},
   {TEXT ("center"), SPACE, HTML_EL_Center, NULL},
   {TEXT ("cite"), SPACE, HTML_EL_Cite, NULL},
   {TEXT ("code"), SPACE, HTML_EL_Code, NULL},
   {TEXT ("colgroup"), SPACE, HTML_EL_COLGROUP, NULL},
   {TEXT ("col"), SPACE, HTML_EL_COL, NULL},
   {TEXT ("dd"), SPACE, HTML_EL_Definition, NULL},
   {TEXT ("del"), SPACE, HTML_EL_DEL, NULL},
   {TEXT ("dfn"), SPACE, HTML_EL_Def, NULL},
   {TEXT ("dir"), SPACE, HTML_EL_Directory, NULL},
   {TEXT ("div"), SPACE, HTML_EL_Division, NULL},
   {TEXT ("dl"), SPACE, HTML_EL_Definition_List, NULL},
   {TEXT ("dt"), SPACE, HTML_EL_Term, NULL},
   {TEXT ("em"), SPACE, HTML_EL_Emphasis, NULL},
   {TEXT ("fieldset"), SPACE, HTML_EL_FIELDSET, NULL},
   {TEXT ("font"), SPACE, HTML_EL_Font_, NULL},
   {TEXT ("form"), SPACE, HTML_EL_Form, NULL},
   {TEXT ("frame"), TEXT ('E'), HTML_EL_FRAME, NULL},
   {TEXT ("frameset"), SPACE, HTML_EL_FRAMESET, NULL},
   {TEXT ("h1"), SPACE, HTML_EL_H1, NULL},
   {TEXT ("h2"), SPACE, HTML_EL_H2, NULL},
   {TEXT ("h3"), SPACE, HTML_EL_H3, NULL},
   {TEXT ("h4"), SPACE, HTML_EL_H4, NULL},
   {TEXT ("h5"), SPACE, HTML_EL_H5, NULL},
   {TEXT ("h6"), SPACE, HTML_EL_H6, NULL},
   {TEXT ("head"), SPACE, HTML_EL_HEAD, NULL},
   {TEXT ("hr"), TEXT ('E'), HTML_EL_Horizontal_Rule, NULL},
   {TEXT ("html"), SPACE, HTML_EL_HTML, NULL},
   {TEXT ("i"), SPACE, HTML_EL_Italic_text, NULL},
   {TEXT ("iframe"), SPACE, HTML_EL_IFRAME, NULL},
   {TEXT ("image"), TEXT ('E'), HTML_EL_PICTURE_UNIT, NULL},
   {TEXT ("img"), TEXT ('E'), HTML_EL_PICTURE_UNIT, NULL},
   {TEXT ("input"), TEXT ('E'), HTML_EL_Input, NULL},
   {TEXT ("ins"), SPACE, HTML_EL_INS, NULL},
   {TEXT ("isindex"), TEXT ('E'), HTML_EL_ISINDEX, NULL},
   {TEXT ("kbd"), SPACE, HTML_EL_Keyboard, NULL},
   {TEXT ("label"), SPACE, HTML_EL_LABEL, NULL},
   {TEXT ("legend"), SPACE, HTML_EL_LEGEND, NULL},
   {TEXT ("li"), SPACE, HTML_EL_List_Item, NULL},
   {TEXT ("link"), TEXT ('E'), HTML_EL_LINK, NULL},
   {TEXT ("listing"), SPACE, HTML_EL_Preformatted, NULL}, /*converted to PRE */
   {TEXT ("map"), SPACE, HTML_EL_MAP, NULL},
   {TEXT ("math"), SPACE, HTML_EL_Math, NULL},
   {TEXT ("mathdisp"), SPACE, HTML_EL_Math, NULL}, /* for compatibility with an
				     old version of MathML: WD-math-970704 */
   {TEXT ("menu"), SPACE, HTML_EL_Menu, NULL},
   {TEXT ("meta"), TEXT ('E'), HTML_EL_META, NULL},
   {TEXT ("noframes"), SPACE, HTML_EL_NOFRAMES, NULL},
   {TEXT ("noscript"), SPACE, HTML_EL_NOSCRIPT, NULL},
   {TEXT ("object"), SPACE, HTML_EL_Object, NULL},
   {TEXT ("ol"), SPACE, HTML_EL_Numbered_List, NULL},
   {TEXT ("optgroup"), SPACE, HTML_EL_OptGroup, NULL},
   {TEXT ("option"), SPACE, HTML_EL_Option, NULL},
   {TEXT ("p"), SPACE, HTML_EL_Paragraph, NULL},
   {TEXT ("p*"), SPACE, HTML_EL_Pseudo_paragraph, NULL},
   {TEXT ("param"), TEXT ('E'), HTML_EL_Parameter, NULL},
   {TEXT ("plaintext"), SPACE, HTML_EL_Preformatted, NULL},/* converted to PRE */
   {TEXT ("pre"), SPACE, HTML_EL_Preformatted, NULL},
   {TEXT ("q"), SPACE, HTML_EL_Quotation, NULL},
   {TEXT ("s"), SPACE, HTML_EL_Struck_text, NULL},
   {TEXT ("samp"), SPACE, HTML_EL_Sample, NULL},
   {TEXT ("script"), SPACE, HTML_EL_SCRIPT, NULL},
   {TEXT ("select"), SPACE, HTML_EL_Option_Menu, NULL},
   {TEXT ("small"), SPACE, HTML_EL_Small_text, NULL},
   {TEXT ("span"), SPACE, HTML_EL_Span, NULL},
   {TEXT ("strike"), SPACE, HTML_EL_Struck_text, NULL},
   {TEXT ("strong"), SPACE, HTML_EL_Strong, NULL},
   {TEXT ("style"), SPACE, HTML_EL_STYLE_, NULL},
   {TEXT ("sub"), SPACE, HTML_EL_Subscript, NULL},
   {TEXT ("sup"), SPACE, HTML_EL_Superscript, NULL},
   {TEXT ("table"), SPACE, HTML_EL_Table, NULL},
   {TEXT ("tbody"), SPACE, HTML_EL_tbody, NULL},
   {TEXT ("td"), SPACE, HTML_EL_Data_cell, NULL},
   {TEXT ("textarea"), SPACE, HTML_EL_Text_Area, NULL},
   {TEXT ("tfoot"), SPACE, HTML_EL_tfoot, NULL},
   {TEXT ("th"), SPACE, HTML_EL_Heading_cell, NULL},
   {TEXT ("thead"), SPACE, HTML_EL_thead, NULL},
   {TEXT ("title"), SPACE, HTML_EL_TITLE, NULL},
   {TEXT ("tr"), SPACE, HTML_EL_Table_row, NULL},
   {TEXT ("tt"), SPACE, HTML_EL_Teletype_text, NULL},
   {TEXT ("u"), SPACE, HTML_EL_Underlined_text, NULL},
   {TEXT ("ul"), SPACE, HTML_EL_Unnumbered_List, NULL},
   {TEXT ("var"), SPACE, HTML_EL_Variable, NULL},
   {TEXT ("xmlgraphics"), SPACE, HTML_EL_XMLGraphics, NULL},
   {TEXT ("xmp"), SPACE, HTML_EL_Preformatted, NULL},  /* converted to PRE */
   {TEXT (""), SPACE, 0, NULL}	/* Last entry. Mandatory */
};
/* mapping table of HTML attributes */

static AttributeMapping HTMLAttributeMappingTable[] =
{
   /* The first entry MUST be unknown_attr */
   /* The rest of this table MUST be in alphabetical order */
   {TEXT("unknown_attr"), TEXT(""), TEXT('A'), HTML_ATTR_Invalid_attribute},

   {TEXT("abbr"), TEXT(""), TEXT('A'), HTML_ATTR_abbr},
   {TEXT("accept"), TEXT(""), TEXT('A'), HTML_ATTR_accept},
   {TEXT("accept-charset"), TEXT("form"), TEXT('A'), HTML_ATTR_accept_charset},
   {TEXT("accesskey"), TEXT(""), TEXT('A'), HTML_ATTR_accesskey},
   {TEXT("action"), TEXT(""), TEXT('A'), HTML_ATTR_Script_URL},
   {TEXT("align"), TEXT("applet"), TEXT('A'), HTML_ATTR_Alignment},
   {TEXT("align"), TEXT("caption"), TEXT('A'), HTML_ATTR_Position},
   {TEXT("align"), TEXT("col"), TEXT('A'), HTML_ATTR_Cell_align},
   {TEXT("align"), TEXT("colgroup"), TEXT('A'), HTML_ATTR_Cell_align},
   {TEXT("align"), TEXT("div"), TEXT('A'), HTML_ATTR_TextAlign},
   {TEXT("align"), TEXT("h1"), TEXT('A'), HTML_ATTR_TextAlign},
   {TEXT("align"), TEXT("h2"), TEXT('A'), HTML_ATTR_TextAlign},
   {TEXT("align"), TEXT("h3"), TEXT('A'), HTML_ATTR_TextAlign},
   {TEXT("align"), TEXT("h4"), TEXT('A'), HTML_ATTR_TextAlign},
   {TEXT("align"), TEXT("h5"), TEXT('A'), HTML_ATTR_TextAlign},
   {TEXT("align"), TEXT("h6"), TEXT('A'), HTML_ATTR_TextAlign},
   {TEXT("align"), TEXT("hr"), TEXT('A'), HTML_ATTR_Align},
   {TEXT("align"), TEXT("iframe"), TEXT('A'), HTML_ATTR_Alignment},
   {TEXT("align"), TEXT("image"), TEXT('A'), HTML_ATTR_Alignment},
   {TEXT("align"), TEXT("img"), TEXT('A'), HTML_ATTR_Alignment},
   {TEXT("align"), TEXT("input"), TEXT('A'), HTML_ATTR_Alignment},
   {TEXT("align"), TEXT("legend"), TEXT('A'), HTML_ATTR_LAlign},
   {TEXT("align"), TEXT("object"), TEXT('A'), HTML_ATTR_Alignment},
   {TEXT("align"), TEXT("p"), TEXT('A'), HTML_ATTR_TextAlign},
   {TEXT("align"), TEXT("table"), TEXT('A'), HTML_ATTR_Align},
   {TEXT("align"), TEXT("tbody"), TEXT('A'), HTML_ATTR_Cell_align},
   {TEXT("align"), TEXT("td"), TEXT('A'), HTML_ATTR_Cell_align},
   {TEXT("align"), TEXT("tfoot"), TEXT('A'), HTML_ATTR_Cell_align},
   {TEXT("align"), TEXT("th"), TEXT('A'), HTML_ATTR_Cell_align},
   {TEXT("align"), TEXT("thead"), TEXT('A'), HTML_ATTR_Cell_align},
   {TEXT("align"), TEXT("tr"), TEXT('A'), HTML_ATTR_Cell_align},
   {TEXT("alink"), TEXT("body"), TEXT('A'), HTML_ATTR_ActiveLinkColor},
   {TEXT("alt"), TEXT(""), TEXT('A'), HTML_ATTR_ALT},
   {TEXT("archive"), TEXT(""), TEXT('A'), HTML_ATTR_archive},
   {TEXT("axis"), TEXT(""), TEXT('A'), HTML_ATTR_axis},

   {TEXT("background"), TEXT(""), TEXT('A'), HTML_ATTR_background_},
   {TEXT("bgcolor"), TEXT(""), TEXT('A'), HTML_ATTR_BackgroundColor},
   {TEXT("border"), TEXT("image"), TEXT('A'), HTML_ATTR_Img_border},
   {TEXT("border"), TEXT("img"), TEXT('A'), HTML_ATTR_Img_border},
   {TEXT("border"), TEXT("object"), TEXT('A'), HTML_ATTR_Img_border},
   {TEXT("border"), TEXT("table"), TEXT('A'), HTML_ATTR_Border},

   {TEXT("cellspacing"), TEXT(""), TEXT('A'), HTML_ATTR_cellspacing},
   {TEXT("cellpadding"), TEXT(""), TEXT('A'), HTML_ATTR_cellpadding},
   {TEXT("char"), TEXT(""), TEXT('A'), HTML_ATTR_char},
   {TEXT("charoff"), TEXT(""), TEXT('A'), HTML_ATTR_charoff},
   {TEXT("charset"), TEXT(""), TEXT('A'), HTML_ATTR_charset},
   {TEXT("checked"), TEXT(""), TEXT('A'), HTML_ATTR_Checked},
   {TEXT("cite"), TEXT(""), TEXT('A'), HTML_ATTR_cite},
   {TEXT("class"), TEXT(""), TEXT('A'), HTML_ATTR_Class},
   {TEXT("classid"), TEXT(""), TEXT('A'), HTML_ATTR_classid},
   {TEXT("clear"), TEXT("br"), TEXT('A'), HTML_ATTR_Clear},
   {TEXT("code"), TEXT(""), TEXT('A'), HTML_ATTR_code},
   {TEXT("codebase"), TEXT(""), TEXT('A'), HTML_ATTR_codebase},
   {TEXT("codetype"), TEXT(""), TEXT('A'), HTML_ATTR_codetype},
   {TEXT("color"), TEXT("basefont"), TEXT('A'), HTML_ATTR_BaseFontColor},
   {TEXT("color"), TEXT(""), TEXT('A'), HTML_ATTR_color},
   {TEXT("cols"), TEXT("frameset"), TEXT('A'), HTML_ATTR_ColWidth},
   {TEXT("cols"), TEXT("textarea"), TEXT('A'), HTML_ATTR_Columns},
   {TEXT("colspan"), TEXT(""), TEXT('A'), HTML_ATTR_colspan_},
   {TEXT("compact"), TEXT(""), TEXT('A'), HTML_ATTR_COMPACT},
   {TEXT("content"), TEXT(""), TEXT('A'), HTML_ATTR_meta_content},
   {TEXT("coords"), TEXT(""), TEXT('A'), HTML_ATTR_coords},

   {TEXT("data"), TEXT(""), TEXT('A'), HTML_ATTR_data},
   {TEXT("datapagesize"), TEXT("table"), TEXT('A'), HTML_ATTR_datapagesize},
   {TEXT("datetime"), TEXT(""), TEXT('A'), HTML_ATTR_datetime},
   {TEXT("declare"), TEXT("object"), TEXT('A'), HTML_ATTR_declare},
   {TEXT("defer"), TEXT("script"), TEXT('A'), HTML_ATTR_defer},
   {TEXT("dir"), TEXT(""), TEXT('A'), HTML_ATTR_dir},
   {TEXT("disabled"), TEXT(""), TEXT('A'), HTML_ATTR_disabled},

   {TEXT("enctype"), TEXT(""), TEXT('A'), HTML_ATTR_ENCTYPE},
   {TEXT("event"), TEXT("script"), TEXT('A'), HTML_ATTR_event},

   {TEXT("face"), TEXT("basefont"), TEXT('A'), HTML_ATTR_BaseFontFace},
   {TEXT("face"), TEXT("font"), TEXT('A'), HTML_ATTR_face},
   {TEXT("for"), TEXT("label"), TEXT('A'), HTML_ATTR_Associated_control},
   {TEXT("for"), TEXT("script"), TEXT('A'), HTML_ATTR_for_},
   {TEXT("frame"), TEXT("table"), TEXT('A'), HTML_ATTR_frame},
   {TEXT("frameborder"), TEXT(""), TEXT('A'), HTML_ATTR_frameborder},

   {TEXT("headers"), TEXT(""), TEXT('A'), HTML_ATTR_headers},
   {TEXT("height"), TEXT(""), TEXT('A'), HTML_ATTR_Height_},
   {TEXT("href"), TEXT(""), TEXT('A'), HTML_ATTR_HREF_},
   {TEXT("hreflang"), TEXT(""), TEXT('A'), HTML_ATTR_hreflang},
   {TEXT("hspace"), TEXT(""), TEXT('A'), HTML_ATTR_hspace},
   {TEXT("http-equiv"), TEXT(""), TEXT('A'), HTML_ATTR_http_equiv},

   {TEXT("id"), TEXT(""), TEXT('A'), HTML_ATTR_ID},
   {TEXT("ismap"), TEXT(""), TEXT('A'), HTML_ATTR_ISMAP},

   {TEXT("label"), TEXT(""), TEXT('A'), HTML_ATTR_label},
   {TEXT("lang"), TEXT(""), TEXT('A'), HTML_ATTR_Langue},
   {TEXT("language"), TEXT("script"), TEXT('A'), HTML_ATTR_script_language},
   {TEXT("link"), TEXT("body"), TEXT('A'), HTML_ATTR_LinkColor},
   {TEXT("longdesc"), TEXT(""), TEXT('A'), HTML_ATTR_longdesc},

   {TEXT("marginheight"), TEXT(""), TEXT('A'), HTML_ATTR_marginheight},
   {TEXT("marginwidth"), TEXT(""), TEXT('A'), HTML_ATTR_marginwidth},
   {TEXT("maxlength"), TEXT(""), TEXT('A'), HTML_ATTR_MaxLength},
   {TEXT("media"), TEXT(""), TEXT('A'), HTML_ATTR_media},
   {TEXT("method"), TEXT(""), TEXT('A'), HTML_ATTR_METHOD},
   {TEXT("mode"), TEXT(""), TEXT('A'), HTML_ATTR_mode},
   {TEXT("multiple"), TEXT(""), TEXT('A'), HTML_ATTR_Multiple},

   {TEXT("N"), TEXT(""), TEXT('C'), 0},
   {TEXT("name"), TEXT("applet"), TEXT('A'), HTML_ATTR_applet_name},
   {TEXT("name"), TEXT("frame"), TEXT('A'), HTML_ATTR_FrameName},
   {TEXT("name"), TEXT("iframe"), TEXT('A'), HTML_ATTR_FrameName},
   {TEXT("name"), TEXT("meta"), TEXT('A'), HTML_ATTR_meta_name},
   {TEXT("name"), TEXT("param"), TEXT('A'), HTML_ATTR_Param_name},
   {TEXT("name"), TEXT(""), TEXT('A'), HTML_ATTR_NAME},
   {TEXT("nohref"), TEXT(""), TEXT('A'), HTML_ATTR_nohref},
   {TEXT("noresize"), TEXT(""), TEXT('A'), HTML_ATTR_no_resize},
   {TEXT("noshade"), TEXT(""), TEXT('A'), HTML_ATTR_NoShade},
   {TEXT("nowrap"), TEXT(""), TEXT('A'), HTML_ATTR_No_wrap},

   {TEXT("object"), TEXT("applet"), TEXT('A'), HTML_ATTR_object},
   {TEXT("onblur"), TEXT(""), TEXT('A'), HTML_ATTR_onblur},
   {TEXT("onchange"), TEXT(""), TEXT('A'), HTML_ATTR_onchange},
   {TEXT("onclick"), TEXT(""), TEXT('A'), HTML_ATTR_onclick},
   {TEXT("ondblclick"), TEXT(""), TEXT('A'), HTML_ATTR_ondblclick},
   {TEXT("onfocus"), TEXT(""), TEXT('A'), HTML_ATTR_onfocus},
   {TEXT("onkeydown"), TEXT(""), TEXT('A'), HTML_ATTR_onkeydown},
   {TEXT("onkeypress"), TEXT(""), TEXT('A'), HTML_ATTR_onkeypress},
   {TEXT("onkeyup"), TEXT(""), TEXT('A'), HTML_ATTR_onkeyup},
   {TEXT("onload"), TEXT(""), TEXT('A'), HTML_ATTR_onload},
   {TEXT("onmousedown"), TEXT(""), TEXT('A'), HTML_ATTR_onmousedown},
   {TEXT("onmousemove"), TEXT(""), TEXT('A'), HTML_ATTR_onmousemove},
   {TEXT("onmouseout"), TEXT(""), TEXT('A'), HTML_ATTR_onmouseout},
   {TEXT("onmouseover"), TEXT(""), TEXT('A'), HTML_ATTR_onmouseover},
   {TEXT("onmouseup"), TEXT(""), TEXT('A'), HTML_ATTR_onmouseup},
   {TEXT("onreset"), TEXT("form"), TEXT('A'), HTML_ATTR_onreset},
   {TEXT("onselect"), TEXT(""), TEXT('A'), HTML_ATTR_onselect},
   {TEXT("onsubmit"), TEXT("form"), TEXT('A'), HTML_ATTR_onsubmit},
   {TEXT("onunload"), TEXT(""), TEXT('A'), HTML_ATTR_onunload},

   {TEXT("profile"), TEXT("head"), TEXT('A'), HTML_ATTR_profile},
   {TEXT("prompt"), TEXT(""), TEXT('A'), HTML_ATTR_Prompt},

   {TEXT("readonly"), TEXT(""), TEXT('A'), HTML_ATTR_readonly},
   {TEXT("rel"), TEXT(""), TEXT('A'), HTML_ATTR_REL},
   {TEXT("rev"), TEXT(""), TEXT('A'), HTML_ATTR_REV},
   {TEXT("rows"), TEXT("frameset"), TEXT('A'), HTML_ATTR_RowHeight},
   {TEXT("rows"), TEXT("textarea"), TEXT('A'), HTML_ATTR_Rows},
   {TEXT("rowspan"), TEXT(""), TEXT('A'), HTML_ATTR_rowspan_},
   {TEXT("rules"), TEXT("table"), TEXT('A'), HTML_ATTR_rules_},

   {TEXT("scheme"), TEXT("meta"), TEXT('A'), HTML_ATTR_scheme},
   {TEXT("scope"), TEXT(""), TEXT('A'), HTML_ATTR_scope},
   {TEXT("scrolling"), TEXT(""), TEXT('A'), HTML_ATTR_scrolling},
   {TEXT("selected"), TEXT(""), TEXT('A'), HTML_ATTR_Selected},
   {TEXT("shape"), TEXT(""), TEXT('A'), HTML_ATTR_shape},
   {TEXT("size"), TEXT("basefont"), TEXT('A'), HTML_ATTR_BaseFontSize},
   {TEXT("size"), TEXT("font"), TEXT('A'), HTML_ATTR_Font_size},
   {TEXT("size"), TEXT("hr"), TEXT('A'), HTML_ATTR_Size_},
   {TEXT("size"), TEXT("input"), TEXT('A'), HTML_ATTR_Area_Size},
   {TEXT("size"), TEXT("select"), TEXT('A'), HTML_ATTR_MenuSize},
   {TEXT("span"), TEXT("col"), TEXT('A'), HTML_ATTR_span_},
   {TEXT("span"), TEXT("colgroup"), TEXT('A'), HTML_ATTR_span_},
   {TEXT("src"), TEXT("frame"), TEXT('A'), HTML_ATTR_FrameSrc},
   {TEXT("src"), TEXT("iframe"), TEXT('A'), HTML_ATTR_FrameSrc},
   {TEXT("src"), TEXT("script"), TEXT('A'), HTML_ATTR_script_src},
   {TEXT("src"), TEXT(""), TEXT('A'), HTML_ATTR_SRC},
   {TEXT("standby"), TEXT(""), TEXT('A'), HTML_ATTR_standby},
   {TEXT("start"), TEXT(""), TEXT('A'), HTML_ATTR_Start},
   {TEXT("style"), TEXT(""), TEXT('A'), HTML_ATTR_Style_},
   {TEXT("summary"), TEXT("table"), TEXT('A'), HTML_ATTR_summary},

   {TEXT("tabindex"), TEXT(""), TEXT('A'), HTML_ATTR_tabindex},
   {TEXT("target"), TEXT(""), TEXT('A'), HTML_ATTR_target_},
   {TEXT("text"), TEXT(""), TEXT('A'), HTML_ATTR_TextColor},
   {TEXT("title"), TEXT(""), TEXT('A'), HTML_ATTR_Title},
   {TEXT("type"), TEXT("button"), TEXT('A'), HTML_ATTR_Button_type},
   {TEXT("type"), TEXT("li"), TEXT('A'), HTML_ATTR_ItemStyle},
   {TEXT("type"), TEXT("link"), TEXT('A'), HTML_ATTR_Link_type},
   {TEXT("type"), TEXT("a"), TEXT('A'), HTML_ATTR_Link_type},
   {TEXT("type"), TEXT("object"), TEXT('A'), HTML_ATTR_Object_type},
   {TEXT("type"), TEXT("ol"), TEXT('A'), HTML_ATTR_NumberStyle},
   {TEXT("type"), TEXT("param"), TEXT('A'), HTML_ATTR_Param_type},
   {TEXT("type"), TEXT("script"), TEXT('A'), HTML_ATTR_content_type},
   {TEXT("type"), TEXT("style"), TEXT('A'), HTML_ATTR_Notation},
   {TEXT("type"), TEXT("ul"), TEXT('A'), HTML_ATTR_BulletStyle},
   {TEXT("type"), TEXT(""), SPACE, DummyAttribute},

   {TEXT("usemap"), TEXT(""), TEXT('A'), HTML_ATTR_USEMAP},

   {TEXT("valign"), TEXT("tbody"), TEXT('A'), HTML_ATTR_Row_valign},
   {TEXT("valign"), TEXT("td"), TEXT('A'), HTML_ATTR_Cell_valign},
   {TEXT("valign"), TEXT("tfoot"), TEXT('A'), HTML_ATTR_Row_valign},
   {TEXT("valign"), TEXT("th"), TEXT('A'), HTML_ATTR_Cell_valign},
   {TEXT("valign"), TEXT("thead"), TEXT('A'), HTML_ATTR_Row_valign},
   {TEXT("valign"), TEXT("tr"), TEXT('A'), HTML_ATTR_Row_valign},
   {TEXT("value"), TEXT("li"), TEXT('A'), HTML_ATTR_ItemValue},
   {TEXT("value"), TEXT("param"), TEXT('A'), HTML_ATTR_Param_value},
   {TEXT("value"), TEXT(""), TEXT('A'), HTML_ATTR_Value_},
   {TEXT("valuetype"), TEXT("param"), TEXT('A'), HTML_ATTR_valuetype},
   {TEXT("version"), TEXT(""), TEXT('A'), 0},
   {TEXT("vlink"), TEXT("body"), TEXT('A'), HTML_ATTR_VisitedLinkColor},
   {TEXT("vspace"), TEXT(""), TEXT('A'), HTML_ATTR_vspace},

   {TEXT("width"), TEXT("applet"), TEXT('A'), HTML_ATTR_Width__},
   {TEXT("width"), TEXT("hr"), TEXT('A'), HTML_ATTR_Width__},
   {TEXT("width"), TEXT("iframe"), TEXT('A'), HTML_ATTR_Width__},
   {TEXT("width"), TEXT("image"), TEXT('A'), HTML_ATTR_Width__},
   {TEXT("width"), TEXT("img"), TEXT('A'), HTML_ATTR_Width__},
   {TEXT("width"), TEXT("object"), TEXT('A'), HTML_ATTR_Width__},
   {TEXT("width"), TEXT("pre"), TEXT('A'), HTML_ATTR_Width__},
   {TEXT("width"), TEXT("table"), TEXT('A'), HTML_ATTR_Width__},
   {TEXT("width"), TEXT("td"), TEXT('A'), HTML_ATTR_Width__},
   {TEXT("width"), TEXT("th"), TEXT('A'), HTML_ATTR_Width__},
#ifdef GRAPHML
   {TEXT("width"), TEXT("xmlgraphics"), TEXT('A'), HTML_ATTR_Width__},
#endif

   {TEXT("zzghost"), TEXT(""), TEXT('A'), HTML_ATTR_Ghost_restruct},
   {TEXT(""), TEXT(""), EOS, 0}		/* Last entry. Mandatory */
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
int                 MapGI (STRING gi, SSchema *schema, Document doc)
#else
int                 MapGI (gi, schema, doc)
STRING              gi;
SSchema		   *schema;
Document            doc;
#endif
{
  ElementType	      elType;
  STRING              ptr; 
  CHAR_T              c;
  int                 i;
  int                 entry;
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
      isHTML = !ustrcmp (ptr, TEXT("HTML"));
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
	  if (ustrcasecmp (HTMLGIMappingTable[i].htmlGI, gi))
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
	if (!ptr || !ustrcmp (ptr, TEXT("MathML")))
	  MapXMLElementType (MATH_TYPE, gi, &elType, &ptr, &c, doc);
	if (elType.ElTypeNum == 0 && (!ptr || !ustrcmp (ptr, TEXT("GraphML"))))
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
void                GIType (STRING gi, ElementType *elType, Document doc)
#else
void                GIType (gi, elType, doc)
STRING              gi;
ElementType        *elType;
Document	    doc;
#endif
{
  CHAR_T              c;
  STRING              ptr;
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
      if (!ustrcasecmp (HTMLGIMappingTable[i].htmlGI, gi))
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
AttributeMapping   *MapAttr (STRING Attr, SSchema *schema, int elemEntry, Document doc)
#else
AttributeMapping   *MapAttr (Attr, schema, elemEntry, doc)
STRING              Attr;
SSchema            *schema;
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
    if (!ustrcasecmp (HTMLAttributeMappingTable[i].XMLattribute, Attr))
      if (HTMLAttributeMappingTable[i].XMLelement[0] == EOS)
        {
	  entry = i;
	  *schema = TtaGetDocumentSSchema (doc);
	}
     else if (elemEntry >= 0 &&
	      !ustrcasecmp (HTMLAttributeMappingTable[i].XMLelement,
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
void           MapHTMLAttribute (STRING Attr, AttributeType *attrType, STRING elementName, Document doc)
#else
void           MapHTMLAttribute (Attr, attrType, elementName, doc)
STRING         Attr;
AttributeType *attrType;
STRING         elementName;
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
