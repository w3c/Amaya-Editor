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
   {TEXT ("A"), SPACE, HTML_EL_Anchor, NULL},
   {TEXT ("ABBR"), SPACE, HTML_EL_ABBR, NULL},
   {TEXT ("ACRONYM"), SPACE, HTML_EL_ACRONYM, NULL},
   {TEXT ("ADDRESS"), SPACE, HTML_EL_Address, NULL},
   {TEXT ("APPLET"), SPACE, HTML_EL_Applet, NULL},
   {TEXT ("AREA"), TEXT ('E'), HTML_EL_AREA, NULL},
   {TEXT ("B"), SPACE, HTML_EL_Bold_text, NULL},
   {TEXT ("BASE"), TEXT ('E'), HTML_EL_BASE, NULL},
   {TEXT ("BASEFONT"), TEXT ('E'), HTML_EL_BaseFont, NULL},
   {TEXT ("BDO"), SPACE, HTML_EL_BDO, NULL},
   {TEXT ("BIG"), SPACE, HTML_EL_Big_text, NULL},
   {TEXT ("BLOCKQUOTE"), SPACE, HTML_EL_Block_Quote, NULL},
   {TEXT ("BODY"), SPACE, HTML_EL_BODY, NULL},
   {TEXT ("BR"), TEXT ('E'), HTML_EL_BR, NULL},
   {TEXT ("BUTTON"), SPACE, HTML_EL_BUTTON, NULL},
   {TEXT ("C"), SPACE, HTML_EL_TEXT_UNIT, NULL},
   {TEXT ("CAPTION"), SPACE, HTML_EL_CAPTION, NULL},
   {TEXT ("CENTER"), SPACE, HTML_EL_Center, NULL},
   {TEXT ("CITE"), SPACE, HTML_EL_Cite, NULL},
   {TEXT ("CODE"), SPACE, HTML_EL_Code, NULL},
   {TEXT ("COLGROUP"), SPACE, HTML_EL_COLGROUP, NULL},
   {TEXT ("COL"), SPACE, HTML_EL_COL, NULL},
   {TEXT ("DD"), SPACE, HTML_EL_Definition, NULL},
   {TEXT ("DEL"), SPACE, HTML_EL_DEL, NULL},
   {TEXT ("DFN"), SPACE, HTML_EL_Def, NULL},
   {TEXT ("DIR"), SPACE, HTML_EL_Directory, NULL},
   {TEXT ("DIV"), SPACE, HTML_EL_Division, NULL},
   {TEXT ("DL"), SPACE, HTML_EL_Definition_List, NULL},
   {TEXT ("DT"), SPACE, HTML_EL_Term, NULL},
   {TEXT ("EM"), SPACE, HTML_EL_Emphasis, NULL},
   {TEXT ("FIELDSET"), SPACE, HTML_EL_FIELDSET, NULL},
   {TEXT ("FONT"), SPACE, HTML_EL_Font_, NULL},
   {TEXT ("FORM"), SPACE, HTML_EL_Form, NULL},
   {TEXT ("FRAME"), TEXT ('E'), HTML_EL_FRAME, NULL},
   {TEXT ("FRAMESET"), SPACE, HTML_EL_FRAMESET, NULL},
   {TEXT ("H1"), SPACE, HTML_EL_H1, NULL},
   {TEXT ("H2"), SPACE, HTML_EL_H2, NULL},
   {TEXT ("H3"), SPACE, HTML_EL_H3, NULL},
   {TEXT ("H4"), SPACE, HTML_EL_H4, NULL},
   {TEXT ("H5"), SPACE, HTML_EL_H5, NULL},
   {TEXT ("H6"), SPACE, HTML_EL_H6, NULL},
   {TEXT ("HEAD"), SPACE, HTML_EL_HEAD, NULL},
   {TEXT ("HR"), TEXT ('E'), HTML_EL_Horizontal_Rule, NULL},
   {TEXT ("HTML"), SPACE, HTML_EL_HTML, NULL},
   {TEXT ("I"), SPACE, HTML_EL_Italic_text, NULL},
   {TEXT ("IFRAME"), SPACE, HTML_EL_IFRAME, NULL},
   {TEXT ("IMAGE"), TEXT ('E'), HTML_EL_PICTURE_UNIT, NULL},
   {TEXT ("IMG"), TEXT ('E'), HTML_EL_PICTURE_UNIT, NULL},
   {TEXT ("INPUT"), TEXT ('E'), HTML_EL_Input, NULL},
   {TEXT ("INS"), SPACE, HTML_EL_INS, NULL},
   {TEXT ("ISINDEX"), TEXT ('E'), HTML_EL_ISINDEX, NULL},
   {TEXT ("KBD"), SPACE, HTML_EL_Keyboard, NULL},
   {TEXT ("LABEL"), SPACE, HTML_EL_LABEL, NULL},
   {TEXT ("LEGEND"), SPACE, HTML_EL_LEGEND, NULL},
   {TEXT ("LI"), SPACE, HTML_EL_List_Item, NULL},
   {TEXT ("LINK"), TEXT ('E'), HTML_EL_LINK, NULL},
   {TEXT ("LISTING"), SPACE, HTML_EL_Preformatted, NULL}, /*converted to PRE */
   {TEXT ("MAP"), SPACE, HTML_EL_MAP, NULL},
   {TEXT ("MATH"), SPACE, HTML_EL_Math, NULL},
   {TEXT ("MATHDISP"), SPACE, HTML_EL_Math, NULL}, /* for compatibility with an
				     old version of MathML: WD-math-970704 */
   {TEXT ("MENU"), SPACE, HTML_EL_Menu, NULL},
   {TEXT ("META"), TEXT ('E'), HTML_EL_META, NULL},
   {TEXT ("NOFRAMES"), SPACE, HTML_EL_NOFRAMES, NULL},
   {TEXT ("NOSCRIPT"), SPACE, HTML_EL_NOSCRIPT, NULL},
   {TEXT ("OBJECT"), SPACE, HTML_EL_Object, NULL},
   {TEXT ("OL"), SPACE, HTML_EL_Numbered_List, NULL},
   {TEXT ("OPTGROUP"), SPACE, HTML_EL_OptGroup, NULL},
   {TEXT ("OPTION"), SPACE, HTML_EL_Option, NULL},
   {TEXT ("P"), SPACE, HTML_EL_Paragraph, NULL},
   {TEXT ("P*"), SPACE, HTML_EL_Pseudo_paragraph, NULL},
   {TEXT ("PARAM"), TEXT ('E'), HTML_EL_Parameter, NULL},
   {TEXT ("PLAINTEXT"), SPACE, HTML_EL_Preformatted, NULL},/* converted to PRE */
   {TEXT ("PRE"), SPACE, HTML_EL_Preformatted, NULL},
   {TEXT ("Q"), SPACE, HTML_EL_Quotation, NULL},
   {TEXT ("S"), SPACE, HTML_EL_Struck_text, NULL},
   {TEXT ("SAMP"), SPACE, HTML_EL_Sample, NULL},
   {TEXT ("SCRIPT"), SPACE, HTML_EL_SCRIPT, NULL},
   {TEXT ("SELECT"), SPACE, HTML_EL_Option_Menu, NULL},
   {TEXT ("SMALL"), SPACE, HTML_EL_Small_text, NULL},
   {TEXT ("SPAN"), SPACE, HTML_EL_Span, NULL},
   {TEXT ("STRIKE"), SPACE, HTML_EL_Struck_text, NULL},
   {TEXT ("STRONG"), SPACE, HTML_EL_Strong, NULL},
   {TEXT ("STYLE"), SPACE, HTML_EL_STYLE_, NULL},
   {TEXT ("SUB"), SPACE, HTML_EL_Subscript, NULL},
   {TEXT ("SUP"), SPACE, HTML_EL_Superscript, NULL},
   {TEXT ("TABLE"), SPACE, HTML_EL_Table, NULL},
   {TEXT ("TBODY"), SPACE, HTML_EL_tbody, NULL},
   {TEXT ("TD"), SPACE, HTML_EL_Data_cell, NULL},
   {TEXT ("TEXTAREA"), SPACE, HTML_EL_Text_Area, NULL},
   {TEXT ("TFOOT"), SPACE, HTML_EL_tfoot, NULL},
   {TEXT ("TH"), SPACE, HTML_EL_Heading_cell, NULL},
   {TEXT ("THEAD"), SPACE, HTML_EL_thead, NULL},
   {TEXT ("TITLE"), SPACE, HTML_EL_TITLE, NULL},
   {TEXT ("TR"), SPACE, HTML_EL_Table_row, NULL},
   {TEXT ("TT"), SPACE, HTML_EL_Teletype_text, NULL},
   {TEXT ("U"), SPACE, HTML_EL_Underlined_text, NULL},
   {TEXT ("UL"), SPACE, HTML_EL_Unnumbered_List, NULL},
   {TEXT ("VAR"), SPACE, HTML_EL_Variable, NULL},
   {TEXT ("XMLGRAPHICS"), SPACE, HTML_EL_XMLGraphics, NULL},
   {TEXT ("XMP"), SPACE, HTML_EL_Preformatted, NULL},  /* converted to PRE */
   {TEXT (""), SPACE, 0, NULL}	/* Last entry. Mandatory */
};
/* mapping table of HTML attributes */

static AttributeMapping HTMLAttributeMappingTable[] =
{
   /* The first entry MUST be unknown_attr */
   /* The rest of this table MUST be in alphabetical order */
   {TEXT("unknown_attr"), TEXT(""), TEXT('A'), HTML_ATTR_Invalid_attribute},

   {TEXT("ABBR"), TEXT(""), TEXT('A'), HTML_ATTR_abbr},
   {TEXT("ACCEPT"), TEXT(""), TEXT('A'), HTML_ATTR_accept},
   {TEXT("ACCEPT-CHARSET"), TEXT("FORM"), TEXT('A'), HTML_ATTR_accept_charset},
   {TEXT("ACCESSKEY"), TEXT(""), TEXT('A'), HTML_ATTR_accesskey},
   {TEXT("ACTION"), TEXT(""), TEXT('A'), HTML_ATTR_Script_URL},
   {TEXT("ALIGN"), TEXT("APPLET"), TEXT('A'), HTML_ATTR_Alignment},
   {TEXT("ALIGN"), TEXT("CAPTION"), TEXT('A'), HTML_ATTR_Position},
   {TEXT("ALIGN"), TEXT("COL"), TEXT('A'), HTML_ATTR_Cell_align},
   {TEXT("ALIGN"), TEXT("COLGROUP"), TEXT('A'), HTML_ATTR_Cell_align},
   {TEXT("ALIGN"), TEXT("DIV"), TEXT('A'), HTML_ATTR_TextAlign},
   {TEXT("ALIGN"), TEXT("H1"), TEXT('A'), HTML_ATTR_TextAlign},
   {TEXT("ALIGN"), TEXT("H2"), TEXT('A'), HTML_ATTR_TextAlign},
   {TEXT("ALIGN"), TEXT("H3"), TEXT('A'), HTML_ATTR_TextAlign},
   {TEXT("ALIGN"), TEXT("H4"), TEXT('A'), HTML_ATTR_TextAlign},
   {TEXT("ALIGN"), TEXT("H5"), TEXT('A'), HTML_ATTR_TextAlign},
   {TEXT("ALIGN"), TEXT("H6"), TEXT('A'), HTML_ATTR_TextAlign},
   {TEXT("ALIGN"), TEXT("HR"), TEXT('A'), HTML_ATTR_Align},
   {TEXT("ALIGN"), TEXT("IFRAME"), TEXT('A'), HTML_ATTR_Alignment},
   {TEXT("ALIGN"), TEXT("IMAGE"), TEXT('A'), HTML_ATTR_Alignment},
   {TEXT("ALIGN"), TEXT("IMG"), TEXT('A'), HTML_ATTR_Alignment},
   {TEXT("ALIGN"), TEXT("INPUT"), TEXT('A'), HTML_ATTR_Alignment},
   {TEXT("ALIGN"), TEXT("LEGEND"), TEXT('A'), HTML_ATTR_LAlign},
   {TEXT("ALIGN"), TEXT("OBJECT"), TEXT('A'), HTML_ATTR_Alignment},
   {TEXT("ALIGN"), TEXT("P"), TEXT('A'), HTML_ATTR_TextAlign},
   {TEXT("ALIGN"), TEXT("TABLE"), TEXT('A'), HTML_ATTR_Align},
   {TEXT("ALIGN"), TEXT("TBODY"), TEXT('A'), HTML_ATTR_Cell_align},
   {TEXT("ALIGN"), TEXT("TD"), TEXT('A'), HTML_ATTR_Cell_align},
   {TEXT("ALIGN"), TEXT("TFOOT"), TEXT('A'), HTML_ATTR_Cell_align},
   {TEXT("ALIGN"), TEXT("TH"), TEXT('A'), HTML_ATTR_Cell_align},
   {TEXT("ALIGN"), TEXT("THEAD"), TEXT('A'), HTML_ATTR_Cell_align},
   {TEXT("ALIGN"), TEXT("TR"), TEXT('A'), HTML_ATTR_Cell_align},
   {TEXT("ALINK"), TEXT("BODY"), TEXT('A'), HTML_ATTR_ActiveLinkColor},
   {TEXT("ALT"), TEXT(""), TEXT('A'), HTML_ATTR_ALT},
   {TEXT("ARCHIVE"), TEXT(""), TEXT('A'), HTML_ATTR_archive},
   {TEXT("AXIS"), TEXT(""), TEXT('A'), HTML_ATTR_axis},

   {TEXT("BACKGROUND"), TEXT(""), TEXT('A'), HTML_ATTR_background_},
   {TEXT("BGCOLOR"), TEXT(""), TEXT('A'), HTML_ATTR_BackgroundColor},
   {TEXT("BORDER"), TEXT("IMAGE"), TEXT('A'), HTML_ATTR_Img_border},
   {TEXT("BORDER"), TEXT("IMG"), TEXT('A'), HTML_ATTR_Img_border},
   {TEXT("BORDER"), TEXT("OBJECT"), TEXT('A'), HTML_ATTR_Img_border},
   {TEXT("BORDER"), TEXT("TABLE"), TEXT('A'), HTML_ATTR_Border},

   {TEXT("CELLSPACING"), TEXT(""), TEXT('A'), HTML_ATTR_cellspacing},
   {TEXT("CELLPADDING"), TEXT(""), TEXT('A'), HTML_ATTR_cellpadding},
   {TEXT("CHAR"), TEXT(""), TEXT('A'), HTML_ATTR_char},
   {TEXT("CHAROFF"), TEXT(""), TEXT('A'), HTML_ATTR_charoff},
   {TEXT("CHARSET"), TEXT(""), TEXT('A'), HTML_ATTR_charset},
   {TEXT("CHECKED"), TEXT(""), TEXT('A'), HTML_ATTR_Checked},
   {TEXT("CITE"), TEXT(""), TEXT('A'), HTML_ATTR_cite},
   {TEXT("CLASS"), TEXT(""), TEXT('A'), HTML_ATTR_Class},
   {TEXT("CLASSID"), TEXT(""), TEXT('A'), HTML_ATTR_classid},
   {TEXT("CLEAR"), TEXT("BR"), TEXT('A'), HTML_ATTR_Clear},
   {TEXT("CODE"), TEXT(""), TEXT('A'), HTML_ATTR_code},
   {TEXT("CODEBASE"), TEXT(""), TEXT('A'), HTML_ATTR_codebase},
   {TEXT("CODETYPE"), TEXT(""), TEXT('A'), HTML_ATTR_codetype},
   {TEXT("COLOR"), TEXT("BASEFONT"), TEXT('A'), HTML_ATTR_BaseFontColor},
   {TEXT("COLOR"), TEXT(""), TEXT('A'), HTML_ATTR_color},
   {TEXT("COLS"), TEXT("FRAMESET"), TEXT('A'), HTML_ATTR_ColWidth},
   {TEXT("COLS"), TEXT("TEXTAREA"), TEXT('A'), HTML_ATTR_Columns},
   {TEXT("COLSPAN"), TEXT(""), TEXT('A'), HTML_ATTR_colspan_},
   {TEXT("COMPACT"), TEXT(""), TEXT('A'), HTML_ATTR_COMPACT},
   {TEXT("CONTENT"), TEXT(""), TEXT('A'), HTML_ATTR_meta_content},
   {TEXT("COORDS"), TEXT(""), TEXT('A'), HTML_ATTR_coords},

   {TEXT("DATA"), TEXT(""), TEXT('A'), HTML_ATTR_data},
   {TEXT("DATAPAGESIZE"), TEXT("TABLE"), TEXT('A'), HTML_ATTR_datapagesize},
   {TEXT("DATETIME"), TEXT(""), TEXT('A'), HTML_ATTR_datetime},
   {TEXT("DECLARE"), TEXT("OBJECT"), TEXT('A'), HTML_ATTR_declare},
   {TEXT("DEFER"), TEXT("SCRIPT"), TEXT('A'), HTML_ATTR_defer},
   {TEXT("DIR"), TEXT(""), TEXT('A'), HTML_ATTR_dir},
   {TEXT("DISABLED"), TEXT(""), TEXT('A'), HTML_ATTR_disabled},

   {TEXT("ENCTYPE"), TEXT(""), TEXT('A'), HTML_ATTR_ENCTYPE},
   {TEXT("EVENT"), TEXT("SCRIPT"), TEXT('A'), HTML_ATTR_event},

   {TEXT("FACE"), TEXT("BASEFONT"), TEXT('A'), HTML_ATTR_BaseFontFace},
   {TEXT("FACE"), TEXT("FONT"), TEXT('A'), HTML_ATTR_face},
   {TEXT("FOR"), TEXT("LABEL"), TEXT('A'), HTML_ATTR_Associated_control},
   {TEXT("FOR"), TEXT("SCRIPT"), TEXT('A'), HTML_ATTR_for_},
   {TEXT("FRAME"), TEXT("TABLE"), TEXT('A'), HTML_ATTR_frame},
   {TEXT("FRAMEBORDER"), TEXT(""), TEXT('A'), HTML_ATTR_frameborder},

   {TEXT("HEADERS"), TEXT(""), TEXT('A'), HTML_ATTR_headers},
   {TEXT("HEIGHT"), TEXT(""), TEXT('A'), HTML_ATTR_Height_},
   {TEXT("HREF"), TEXT(""), TEXT('A'), HTML_ATTR_HREF_},
   {TEXT("HREFLANG"), TEXT(""), TEXT('A'), HTML_ATTR_hreflang},
   {TEXT("HSPACE"), TEXT(""), TEXT('A'), HTML_ATTR_hspace},
   {TEXT("HTTP-EQUIV"), TEXT(""), TEXT('A'), HTML_ATTR_http_equiv},

   {TEXT("ID"), TEXT(""), TEXT('A'), HTML_ATTR_ID},
   {TEXT("ISMAP"), TEXT(""), TEXT('A'), HTML_ATTR_ISMAP},

   {TEXT("LABEL"), TEXT(""), TEXT('A'), HTML_ATTR_label},
   {TEXT("LANG"), TEXT(""), TEXT('A'), HTML_ATTR_Langue},
   {TEXT("LANGUAGE"), TEXT("SCRIPT"), TEXT('A'), HTML_ATTR_script_language},
   {TEXT("LINK"), TEXT("BODY"), TEXT('A'), HTML_ATTR_LinkColor},
   {TEXT("LONGDESC"), TEXT(""), TEXT('A'), HTML_ATTR_longdesc},

   {TEXT("MARGINHEIGHT"), TEXT(""), TEXT('A'), HTML_ATTR_marginheight},
   {TEXT("MARGINWIDTH"), TEXT(""), TEXT('A'), HTML_ATTR_marginwidth},
   {TEXT("MAXLENGTH"), TEXT(""), TEXT('A'), HTML_ATTR_MaxLength},
   {TEXT("MEDIA"), TEXT(""), TEXT('A'), HTML_ATTR_media},
   {TEXT("METHOD"), TEXT(""), TEXT('A'), HTML_ATTR_METHOD},
   {TEXT("MODE"), TEXT(""), TEXT('A'), HTML_ATTR_mode},
   {TEXT("MULTIPLE"), TEXT(""), TEXT('A'), HTML_ATTR_Multiple},

   {TEXT("N"), TEXT(""), TEXT('C'), 0},
   {TEXT("NAME"), TEXT("APPLET"), TEXT('A'), HTML_ATTR_applet_name},
   {TEXT("NAME"), TEXT("FRAME"), TEXT('A'), HTML_ATTR_FrameName},
   {TEXT("NAME"), TEXT("IFRAME"), TEXT('A'), HTML_ATTR_FrameName},
   {TEXT("NAME"), TEXT("META"), TEXT('A'), HTML_ATTR_meta_name},
   {TEXT("NAME"), TEXT("PARAM"), TEXT('A'), HTML_ATTR_Param_name},
   {TEXT("NAME"), TEXT(""), TEXT('A'), HTML_ATTR_NAME},
   {TEXT("NOHREF"), TEXT(""), TEXT('A'), HTML_ATTR_nohref},
   {TEXT("NORESIZE"), TEXT(""), TEXT('A'), HTML_ATTR_no_resize},
   {TEXT("NOSHADE"), TEXT(""), TEXT('A'), HTML_ATTR_NoShade},
   {TEXT("NOWRAP"), TEXT(""), TEXT('A'), HTML_ATTR_No_wrap},

   {TEXT("OBJECT"), TEXT("APPLET"), TEXT('A'), HTML_ATTR_object},
   {TEXT("ONBLUR"), TEXT(""), TEXT('A'), HTML_ATTR_onblur},
   {TEXT("ONCHANGE"), TEXT(""), TEXT('A'), HTML_ATTR_onchange},
   {TEXT("ONCLICK"), TEXT(""), TEXT('A'), HTML_ATTR_onclick},
   {TEXT("ONDBLCLICK"), TEXT(""), TEXT('A'), HTML_ATTR_ondblclick},
   {TEXT("ONFOCUS"), TEXT(""), TEXT('A'), HTML_ATTR_onfocus},
   {TEXT("ONKEYDOWN"), TEXT(""), TEXT('A'), HTML_ATTR_onkeydown},
   {TEXT("ONKEYPRESS"), TEXT(""), TEXT('A'), HTML_ATTR_onkeypress},
   {TEXT("ONKEYUP"), TEXT(""), TEXT('A'), HTML_ATTR_onkeyup},
   {TEXT("ONLOAD"), TEXT(""), TEXT('A'), HTML_ATTR_onload},
   {TEXT("ONMOUSEDOWN"), TEXT(""), TEXT('A'), HTML_ATTR_onmousedown},
   {TEXT("ONMOUSEMOVE"), TEXT(""), TEXT('A'), HTML_ATTR_onmousemove},
   {TEXT("ONMOUSEOUT"), TEXT(""), TEXT('A'), HTML_ATTR_onmouseout},
   {TEXT("ONMOUSEOVER"), TEXT(""), TEXT('A'), HTML_ATTR_onmouseover},
   {TEXT("ONMOUSEUP"), TEXT(""), TEXT('A'), HTML_ATTR_onmouseup},
   {TEXT("ONRESET"), TEXT("FORM"), TEXT('A'), HTML_ATTR_onreset},
   {TEXT("ONSELECT"), TEXT(""), TEXT('A'), HTML_ATTR_onselect},
   {TEXT("ONSUBMIT"), TEXT("FORM"), TEXT('A'), HTML_ATTR_onsubmit},
   {TEXT("ONUNLOAD"), TEXT(""), TEXT('A'), HTML_ATTR_onunload},

   {TEXT("PROFILE"), TEXT("HEAD"), TEXT('A'), HTML_ATTR_profile},
   {TEXT("PROMPT"), TEXT(""), TEXT('A'), HTML_ATTR_Prompt},

   {TEXT("READONLY"), TEXT(""), TEXT('A'), HTML_ATTR_readonly},
   {TEXT("REL"), TEXT(""), TEXT('A'), HTML_ATTR_REL},
   {TEXT("REV"), TEXT(""), TEXT('A'), HTML_ATTR_REV},
   {TEXT("ROWS"), TEXT("FRAMESET"), TEXT('A'), HTML_ATTR_RowHeight},
   {TEXT("ROWS"), TEXT("TEXTAREA"), TEXT('A'), HTML_ATTR_Rows},
   {TEXT("ROWSPAN"), TEXT(""), TEXT('A'), HTML_ATTR_rowspan_},
   {TEXT("RULES"), TEXT("TABLE"), TEXT('A'), HTML_ATTR_rules_},

   {TEXT("SCHEME"), TEXT("META"), TEXT('A'), HTML_ATTR_scheme},
   {TEXT("SCOPE"), TEXT(""), TEXT('A'), HTML_ATTR_scope},
   {TEXT("SCROLLING"), TEXT(""), TEXT('A'), HTML_ATTR_scrolling},
   {TEXT("SELECTED"), TEXT(""), TEXT('A'), HTML_ATTR_Selected},
   {TEXT("SHAPE"), TEXT(""), TEXT('A'), HTML_ATTR_shape},
   {TEXT("SIZE"), TEXT("BASEFONT"), TEXT('A'), HTML_ATTR_BaseFontSize},
   {TEXT("SIZE"), TEXT("FONT"), TEXT('A'), HTML_ATTR_Font_size},
   {TEXT("SIZE"), TEXT("HR"), TEXT('A'), HTML_ATTR_Size_},
   {TEXT("SIZE"), TEXT("INPUT"), TEXT('A'), HTML_ATTR_Area_Size},
   {TEXT("SIZE"), TEXT("SELECT"), TEXT('A'), HTML_ATTR_MenuSize},
   {TEXT("SPAN"), TEXT("COL"), TEXT('A'), HTML_ATTR_span_},
   {TEXT("SPAN"), TEXT("COLGROUP"), TEXT('A'), HTML_ATTR_span_},
   {TEXT("SRC"), TEXT("FRAME"), TEXT('A'), HTML_ATTR_FrameSrc},
   {TEXT("SRC"), TEXT("IFRAME"), TEXT('A'), HTML_ATTR_FrameSrc},
   {TEXT("SRC"), TEXT("SCRIPT"), TEXT('A'), HTML_ATTR_script_src},
   {TEXT("SRC"), TEXT(""), TEXT('A'), HTML_ATTR_SRC},
   {TEXT("STANDBY"), TEXT(""), TEXT('A'), HTML_ATTR_standby},
   {TEXT("START"), TEXT(""), TEXT('A'), HTML_ATTR_Start},
   {TEXT("STYLE"), TEXT(""), TEXT('A'), HTML_ATTR_Style_},
   {TEXT("SUMMARY"), TEXT("TABLE"), TEXT('A'), HTML_ATTR_summary},

   {TEXT("TABINDEX"), TEXT(""), TEXT('A'), HTML_ATTR_tabindex},
   {TEXT("TARGET"), TEXT(""), TEXT('A'), HTML_ATTR_target_},
   {TEXT("TEXT"), TEXT(""), TEXT('A'), HTML_ATTR_TextColor},
   {TEXT("TITLE"), TEXT(""), TEXT('A'), HTML_ATTR_Title},
   {TEXT("TYPE"), TEXT("BUTTON"), TEXT('A'), HTML_ATTR_Button_type},
   {TEXT("TYPE"), TEXT("LI"), TEXT('A'), HTML_ATTR_ItemStyle},
   {TEXT("TYPE"), TEXT("LINK"), TEXT('A'), HTML_ATTR_Link_type},
   {TEXT("TYPE"), TEXT("A"), TEXT('A'), HTML_ATTR_Link_type},
   {TEXT("TYPE"), TEXT("OBJECT"), TEXT('A'), HTML_ATTR_Object_type},
   {TEXT("TYPE"), TEXT("OL"), TEXT('A'), HTML_ATTR_NumberStyle},
   {TEXT("TYPE"), TEXT("PARAM"), TEXT('A'), HTML_ATTR_Param_type},
   {TEXT("TYPE"), TEXT("SCRIPT"), TEXT('A'), HTML_ATTR_content_type},
   {TEXT("TYPE"), TEXT("STYLE"), TEXT('A'), HTML_ATTR_Notation},
   {TEXT("TYPE"), TEXT("UL"), TEXT('A'), HTML_ATTR_BulletStyle},
   {TEXT("TYPE"), TEXT(""), SPACE, DummyAttribute},

   {TEXT("USEMAP"), TEXT(""), TEXT('A'), HTML_ATTR_USEMAP},

   {TEXT("VALIGN"), TEXT("TBODY"), TEXT('A'), HTML_ATTR_Row_valign},
   {TEXT("VALIGN"), TEXT("TD"), TEXT('A'), HTML_ATTR_Cell_valign},
   {TEXT("VALIGN"), TEXT("TFOOT"), TEXT('A'), HTML_ATTR_Row_valign},
   {TEXT("VALIGN"), TEXT("TH"), TEXT('A'), HTML_ATTR_Cell_valign},
   {TEXT("VALIGN"), TEXT("THEAD"), TEXT('A'), HTML_ATTR_Row_valign},
   {TEXT("VALIGN"), TEXT("TR"), TEXT('A'), HTML_ATTR_Row_valign},
   {TEXT("VALUE"), TEXT("LI"), TEXT('A'), HTML_ATTR_ItemValue},
   {TEXT("VALUE"), TEXT("PARAM"), TEXT('A'), HTML_ATTR_Param_value},
   {TEXT("VALUE"), TEXT(""), TEXT('A'), HTML_ATTR_Value_},
   {TEXT("VALUETYPE"), TEXT("PARAM"), TEXT('A'), HTML_ATTR_valuetype},
   {TEXT("VERSION"), TEXT(""), TEXT('A'), 0},
   {TEXT("VLINK"), TEXT("BODY"), TEXT('A'), HTML_ATTR_VisitedLinkColor},
   {TEXT("VSPACE"), TEXT(""), TEXT('A'), HTML_ATTR_vspace},

   {TEXT("WIDTH"), TEXT("APPLET"), TEXT('A'), HTML_ATTR_Width__},
   {TEXT("WIDTH"), TEXT("HR"), TEXT('A'), HTML_ATTR_Width__},
   {TEXT("WIDTH"), TEXT("IFRAME"), TEXT('A'), HTML_ATTR_Width__},
   {TEXT("WIDTH"), TEXT("IMAGE"), TEXT('A'), HTML_ATTR_Width__},
   {TEXT("WIDTH"), TEXT("IMG"), TEXT('A'), HTML_ATTR_Width__},
   {TEXT("WIDTH"), TEXT("OBJECT"), TEXT('A'), HTML_ATTR_Width__},
   {TEXT("WIDTH"), TEXT("PRE"), TEXT('A'), HTML_ATTR_Width__},
   {TEXT("WIDTH"), TEXT("TABLE"), TEXT('A'), HTML_ATTR_Width__},
   {TEXT("WIDTH"), TEXT("TD"), TEXT('A'), HTML_ATTR_Width__},
   {TEXT("WIDTH"), TEXT("TH"), TEXT('A'), HTML_ATTR_Width__},
#ifdef GRAPHML
   {TEXT("WIDTH"), TEXT("XMLGRAPHICS"), TEXT('A'), HTML_ATTR_Width__},
#endif

   {TEXT("ZZGHOST"), TEXT(""), TEXT('A'), HTML_ATTR_Ghost_restruct},
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
      c = toupper (gi[0]);
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
  c = toupper (gi[0]);

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
