/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996-2000
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

/* mapping table of Xhtml attributes */
AttributeMapping XhtmlAttributeMappingTable[] =
{
   /* The first entry MUST be unknown_attr */
   {TEXT("unknown_attr"), TEXT(""), 'A', HTML_ATTR_Invalid_attribute, L_Basic},
   {TEXT("abbr"), TEXT(""), 'A', HTML_ATTR_abbr, L_Basic},
   {TEXT("accept"), TEXT(""), 'A', HTML_ATTR_accept, L_Basic},
   {TEXT("accept-charset"), TEXT("form"), 'A', HTML_ATTR_accept_charset, L_Basic},
   {TEXT("accesskey"), TEXT(""), 'A', HTML_ATTR_accesskey, L_Basic},
   {TEXT("action"), TEXT(""), 'A', HTML_ATTR_Script_URL, L_Basic},
   {TEXT("align"), TEXT("applet"), 'A', HTML_ATTR_Alignment, L_Transitional},
   {TEXT("align"), TEXT("caption"), 'A', HTML_ATTR_Position, L_Transitional},
   {TEXT("align"), TEXT("col"), 'A', HTML_ATTR_Cell_align, L_Transitional},
   {TEXT("align"), TEXT("colgroup"), 'A', HTML_ATTR_Cell_align, L_Transitional},
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
   {TEXT("align"), TEXT("tbody"), 'A', HTML_ATTR_Cell_align, L_Transitional},
   {TEXT("align"), TEXT("td"), 'A', HTML_ATTR_Cell_align, L_Transitional},
   {TEXT("align"), TEXT("tfoot"), 'A', HTML_ATTR_Cell_align, L_Transitional},
   {TEXT("align"), TEXT("th"), 'A', HTML_ATTR_Cell_align, L_Transitional},
   {TEXT("align"), TEXT("thead"), 'A', HTML_ATTR_Cell_align, L_Transitional},
   {TEXT("align"), TEXT("tr"), 'A', HTML_ATTR_Cell_align, L_Transitional},
   {TEXT("alink"), TEXT("body"), 'A', HTML_ATTR_ActiveLinkColor, L_Transitional},
   {TEXT("alt"), TEXT(""), 'A', HTML_ATTR_ALT, L_Basic},
   {TEXT("archive"), TEXT(""), 'A', HTML_ATTR_archive, L_Basic},
   {TEXT("axis"), TEXT(""), 'A', HTML_ATTR_axis, L_Basic},

   {TEXT("background"), TEXT(""), 'A', HTML_ATTR_background_, L_Transitional},
   {TEXT("bgcolor"), TEXT(""), 'A', HTML_ATTR_BackgroundColor, L_Transitional},
   {TEXT("border"), TEXT("image"), 'A', HTML_ATTR_Img_border, L_Transitional},
   {TEXT("border"), TEXT("img"), 'A', HTML_ATTR_Img_border, L_Transitional},
   {TEXT("border"), TEXT("object"), 'A', HTML_ATTR_Img_border, L_Transitional},
   {TEXT("border"), TEXT("table"), 'A', HTML_ATTR_Border, L_Transitional},

   {TEXT("cellspacing"), TEXT(""), 'A', HTML_ATTR_cellspacing, L_Transitional},
   {TEXT("cellpadding"), TEXT(""), 'A', HTML_ATTR_cellpadding, L_Transitional},
   {TEXT("char"), TEXT(""), 'A', HTML_ATTR_char, L_Basic},
   {TEXT("charoff"), TEXT(""), 'A', HTML_ATTR_charoff, L_Basic},
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

   {TEXT("enctype"), TEXT(""), 'A', HTML_ATTR_ENCTYPE, L_Transitional},
   {TEXT("event"), TEXT("script"), 'A', HTML_ATTR_event, L_Transitional},

   {TEXT("face"), TEXT("basefont"), 'A', HTML_ATTR_BaseFontFace, L_Transitional},
   {TEXT("face"), TEXT("font"), 'A', HTML_ATTR_face, L_Transitional},
   {TEXT("for"), TEXT("label"), 'A', HTML_ATTR_Associated_control, L_Transitional},
   {TEXT("for"), TEXT("script"), 'A', HTML_ATTR_for_, L_Transitional},
   {TEXT("frame"), TEXT("table"), 'A', HTML_ATTR_frame, L_Transitional},
   {TEXT("frameborder"), TEXT(""), 'A', HTML_ATTR_frameborder, L_Transitional},

   {TEXT("headers"), TEXT(""), 'A', HTML_ATTR_headers, L_Transitional},
#ifdef GRAPHML
   {TEXT("height"), TEXT("svg"), 'A', HTML_ATTR_SvgHeight, L_Transitional},
#endif /* GRAPHML */
   {TEXT("height"), TEXT(""), 'A', HTML_ATTR_Height_, L_Basic},
   {TEXT("href"), TEXT(""), 'A', HTML_ATTR_HREF_, L_Basic},
   {TEXT("hreflang"), TEXT(""), 'A', HTML_ATTR_hreflang, L_Basic},
   {TEXT("hspace"), TEXT(""), 'A', HTML_ATTR_hspace, L_Transitional},
   {TEXT("http-equiv"), TEXT(""), 'A', HTML_ATTR_http_equiv, L_Basic},

   {TEXT("id"), TEXT(""), 'A', HTML_ATTR_ID, L_Basic},
   {TEXT("ismap"), TEXT(""), 'A', HTML_ATTR_ISMAP, L_Transitional},

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
   {TEXT("mode"), TEXT(""), 'A', HTML_ATTR_mode, L_Basic},
   {TEXT("multiple"), TEXT(""), 'A', HTML_ATTR_Multiple, L_Basic},

   {TEXT("N"), TEXT(""), 'C', 0, L_Basic},
   {TEXT("name"), TEXT("applet"), 'A', HTML_ATTR_applet_name, L_Basic},
   {TEXT("name"), TEXT("frame"), 'A', HTML_ATTR_FrameName, L_Basic},
   {TEXT("name"), TEXT("iframe"), 'A', HTML_ATTR_FrameName, L_Basic},
   {TEXT("name"), TEXT("meta"), 'A', HTML_ATTR_meta_name, L_Basic},
   {TEXT("name"), TEXT("param"), 'A', HTML_ATTR_Param_name, L_Basic},
   {TEXT("name"), TEXT(""), 'A', HTML_ATTR_NAME, L_Basic},
   {TEXT("nohref"), TEXT(""), 'A', HTML_ATTR_nohref, L_Basic},
   {TEXT("noresize"), TEXT(""), 'A', HTML_ATTR_no_resize, L_Basic},
   {TEXT("noshade"), TEXT(""), 'A', HTML_ATTR_NoShade, L_Basic},
   {TEXT("nowrap"), TEXT(""), 'A', HTML_ATTR_No_wrap, L_Basic},

   {TEXT("object"), TEXT("applet"), 'A', HTML_ATTR_object, L_Transitional},
   {TEXT("onblur"), TEXT(""), 'A', HTML_ATTR_onblur, L_Transitional},
   {TEXT("onchange"), TEXT(""), 'A', HTML_ATTR_onchange, L_Transitional},
   {TEXT("onclick"), TEXT(""), 'A', HTML_ATTR_onclick, L_Transitional},
   {TEXT("ondblclick"), TEXT(""), 'A', HTML_ATTR_ondblclick, L_Transitional},
   {TEXT("onfocus"), TEXT(""), 'A', HTML_ATTR_onfocus, L_Transitional},
   {TEXT("onkeydown"), TEXT(""), 'A', HTML_ATTR_onkeydown, L_Transitional},
   {TEXT("onkeypress"), TEXT(""), 'A', HTML_ATTR_onkeypress, L_Transitional},
   {TEXT("onkeyup"), TEXT(""), 'A', HTML_ATTR_onkeyup, L_Transitional},
   {TEXT("onload"), TEXT(""), 'A', HTML_ATTR_onload, L_Transitional},
   {TEXT("onmousedown"), TEXT(""), 'A', HTML_ATTR_onmousedown, L_Transitional},
   {TEXT("onmousemove"), TEXT(""), 'A', HTML_ATTR_onmousemove, L_Transitional},
   {TEXT("onmouseout"), TEXT(""), 'A', HTML_ATTR_onmouseout, L_Transitional},
   {TEXT("onmouseover"), TEXT(""), 'A', HTML_ATTR_onmouseover, L_Transitional},
   {TEXT("onmouseup"), TEXT(""), 'A', HTML_ATTR_onmouseup, L_Transitional},
   {TEXT("onreset"), TEXT("form"), 'A', HTML_ATTR_onreset, L_Transitional},
   {TEXT("onselect"), TEXT(""), 'A', HTML_ATTR_onselect, L_Transitional},
   {TEXT("onsubmit"), TEXT("form"), 'A', HTML_ATTR_onsubmit, L_Transitional},
   {TEXT("onunload"), TEXT(""), 'A', HTML_ATTR_onunload, L_Transitional},

   {TEXT("profile"), TEXT("head"), 'A', HTML_ATTR_profile, L_Basic},
   {TEXT("prompt"), TEXT(""), 'A', HTML_ATTR_Prompt, L_Transitional},

   {TEXT("readonly"), TEXT(""), 'A', HTML_ATTR_readonly, L_Basic},
   {TEXT("rel"), TEXT(""), 'A', HTML_ATTR_REL, L_Basic},
   {TEXT("rev"), TEXT(""), 'A', HTML_ATTR_REV, L_Basic},
   {TEXT("rows"), TEXT("frameset"), 'A', HTML_ATTR_RowHeight, L_Transitional},
   {TEXT("rows"), TEXT("textarea"), 'A', HTML_ATTR_Rows, L_Basic},
   {TEXT("rowspan"), TEXT(""), 'A', HTML_ATTR_rowspan_, L_Basic},
   {TEXT("rules"), TEXT("table"), 'A', HTML_ATTR_rules_, L_Basic},

   {TEXT("scheme"), TEXT("meta"), 'A', HTML_ATTR_scheme, L_Basic},
   {TEXT("scope"), TEXT(""), 'A', HTML_ATTR_scope, L_Basic},
   {TEXT("scrolling"), TEXT(""), 'A', HTML_ATTR_scrolling, L_Transitional},
   {TEXT("selected"), TEXT(""), 'A', HTML_ATTR_Selected, L_Transitional},
   {TEXT("shape"), TEXT(""), 'A', HTML_ATTR_shape, L_Transitional},
   {TEXT("size"), TEXT("basefont"), 'A', HTML_ATTR_BaseFontSize, L_Transitional},
   {TEXT("size"), TEXT("font"), 'A', HTML_ATTR_Font_size, L_Transitional},
   {TEXT("size"), TEXT("hr"), 'A', HTML_ATTR_Size_, L_Transitional},
   {TEXT("size"), TEXT("input"), 'A', HTML_ATTR_Area_Size, L_Transitional},
   {TEXT("size"), TEXT("select"), 'A', HTML_ATTR_MenuSize, L_Transitional},
   {TEXT("span"), TEXT("col"), 'A', HTML_ATTR_span_, L_Transitional},
   {TEXT("span"), TEXT("colgroup"), 'A', HTML_ATTR_span_, L_Transitional},
   {TEXT("src"), TEXT("frame"), 'A', HTML_ATTR_FrameSrc, L_Transitional},
   {TEXT("src"), TEXT("iframe"), 'A', HTML_ATTR_FrameSrc, L_Transitional},
   {TEXT("src"), TEXT("script"), 'A', HTML_ATTR_script_src, L_Transitional},
   {TEXT("src"), TEXT(""), 'A', HTML_ATTR_SRC, L_Basic},
   {TEXT("standby"), TEXT(""), 'A', HTML_ATTR_standby, L_Basic},
   {TEXT("start"), TEXT(""), 'A', HTML_ATTR_Start, L_Basic},
   {TEXT("style"), TEXT(""), 'A', HTML_ATTR_Style_, L_Basic},
   {TEXT("summary"), TEXT("table"), 'A', HTML_ATTR_summary, L_Basic},

   {TEXT("tabindex"), TEXT(""), 'A', HTML_ATTR_tabindex, L_Basic},
   {TEXT("target"), TEXT(""), 'A', HTML_ATTR_target_, L_Basic},
   {TEXT("text"), TEXT(""), 'A', HTML_ATTR_TextColor, L_Basic},
   {TEXT("title"), TEXT(""), 'A', HTML_ATTR_Title, L_Basic},
   {TEXT("type"), TEXT("button"), 'A', HTML_ATTR_Button_type, L_Transitional},
   {TEXT("type"), TEXT("li"), 'A', HTML_ATTR_ItemStyle, L_Basic},
   {TEXT("type"), TEXT("link"), 'A', HTML_ATTR_Link_type, L_Basic},
   {TEXT("type"), TEXT("a"), 'A', HTML_ATTR_Link_type, L_Basic},
   {TEXT("type"), TEXT("object"), 'A', HTML_ATTR_Object_type, L_Basic},
   {TEXT("type"), TEXT("ol"), 'A', HTML_ATTR_NumberStyle, L_Basic},
   {TEXT("type"), TEXT("param"), 'A', HTML_ATTR_Param_type, L_Basic},
   {TEXT("type"), TEXT("script"), 'A', HTML_ATTR_content_type, L_Transitional},
   {TEXT("type"), TEXT("style"), 'A', HTML_ATTR_Notation, L_Transitional},
   {TEXT("type"), TEXT("ul"), 'A', HTML_ATTR_BulletStyle, L_Basic},
   {TEXT("type"), TEXT(""), SPACE, DummyAttribute, L_Basic},

   {TEXT("usemap"), TEXT(""), 'A', HTML_ATTR_USEMAP, L_Basic},

   {TEXT("valign"), TEXT("tbody"), 'A', HTML_ATTR_Row_valign, L_Basic},
   {TEXT("valign"), TEXT("td"), 'A', HTML_ATTR_Cell_valign, L_Basic},
   {TEXT("valign"), TEXT("tfoot"), 'A', HTML_ATTR_Row_valign, L_Basic},
   {TEXT("valign"), TEXT("th"), 'A', HTML_ATTR_Cell_valign, L_Basic},
   {TEXT("valign"), TEXT("thead"), 'A', HTML_ATTR_Row_valign, L_Basic},
   {TEXT("valign"), TEXT("tr"), 'A', HTML_ATTR_Row_valign, L_Basic},
   {TEXT("value"), TEXT("li"), 'A', HTML_ATTR_ItemValue, L_Basic},
   {TEXT("value"), TEXT("param"), 'A', HTML_ATTR_Param_value, L_Basic},
   {TEXT("value"), TEXT(""), 'A', HTML_ATTR_Value_, L_Basic},
   {TEXT("valuetype"), TEXT("param"), 'A', HTML_ATTR_valuetype, L_Basic},
   {TEXT("version"), TEXT(""), 'A', 0, L_Basic},
   {TEXT("vlink"), TEXT("body"), 'A', HTML_ATTR_VisitedLinkColor, L_Transitional},
   {TEXT("vspace"), TEXT(""), 'A', HTML_ATTR_vspace, L_Transitional},

   {TEXT("width"), TEXT("applet"), 'A', HTML_ATTR_Width__, L_Transitional},
   {TEXT("width"), TEXT("col"), 'A', HTML_ATTR_Width__, L_Transitional},
   {TEXT("width"), TEXT("colgroup"), 'A', HTML_ATTR_Width__, L_Transitional},
   {TEXT("width"), TEXT("hr"), 'A', HTML_ATTR_Width__, L_Transitional},
   {TEXT("width"), TEXT("iframe"), 'A', HTML_ATTR_Width__, L_Transitional},
   {TEXT("width"), TEXT("image"), 'A', HTML_ATTR_Width__, L_Transitional},
   {TEXT("width"), TEXT("img"), 'A', HTML_ATTR_Width__, L_Basic},
   {TEXT("width"), TEXT("object"), 'A', HTML_ATTR_Width__, L_Basic},
   {TEXT("width"), TEXT("pre"), 'A', HTML_ATTR_Width__, L_Transitional},
#ifdef GRAPHML
   {TEXT("width"), TEXT("svg"), 'A', HTML_ATTR_SvgWidth, L_Transitional},
#endif /* GRAPHML */
   {TEXT("width"), TEXT("table"), 'A', HTML_ATTR_Width__, L_Transitional},
   {TEXT("width"), TEXT("td"), 'A', HTML_ATTR_Width__, L_Transitional},
   {TEXT("width"), TEXT("th"), 'A', HTML_ATTR_Width__, L_Transitional},
#ifdef GRAPHML
   {TEXT("width"), TEXT("xmlgraphics"), 'A', HTML_ATTR_Width__, L_Transitional},
#endif /* GRAPHML */

   {TEXT("zzghost"), TEXT(""), 'A', HTML_ATTR_Ghost_restruct, L_Basic},
   {TEXT(""), TEXT(""), EOS, 0, L_Basic}		/* Last entry. Mandatory */
};

/* define a pointer to let other parser functions access the local table */
AttributeMapping *pHTMLAttributeMapping = XhtmlAttributeMappingTable;


/*----------------------------------------------------------------------
   MapGI
   search in the mapping tables the entry for the element of
   name GI and returns the rank of that entry.
   When returning, schema contains the Thot SSchema that defines that element,
   Returns -1 and schema = NULL if not found.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 MapGI (CHAR_T* gi, SSchema *schema, Document doc)
#else
int                 MapGI (gi, schema, doc)
CHAR_T*             gi;
SSchema*            schema;
Document            doc;
#endif
{
  ElementType     elType;
  CHAR_T*         ptr; 
  CHAR_T          c;
  int             i;
  int             entry;
  ThotBool	  isHTML;

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
      c = utolower (gi[0]);
      /* look for the first concerned entry in the table */
      while (pHTMLGIMapping[i].XMLname[0] < c
	     && pHTMLGIMapping[i].XMLname[0] != EOS)
	i++;

      /* look at all entries starting with the right character */
      do
	{
	  if (ustrcasecmp (pHTMLGIMapping[i].XMLname, gi))
	    i++;
	  else
	    entry = i;
	}
      while (entry < 0 && pHTMLGIMapping[i].XMLname[0] == c);
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
void                GIType (CHAR_T* gi, ElementType *elType, Document doc)
#else
void                GIType (gi, elType, doc)
CHAR_T*             gi;
ElementType*        elType;
Document            doc;
#endif
{
  CHAR_T              c;
  CHAR_T*             ptr;
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
  c = utolower (gi[0]);

  i = 0;
  /* look for the first concerned entry in the table */
  while (pHTMLGIMapping[i].XMLname[0] < c &&
	 pHTMLGIMapping[i].XMLname[0] != EOS)
    i++;
  /* look at all entries starting with the right character */
  do
    {
      if (!ustrcasecmp (pHTMLGIMapping[i].XMLname, gi))
      {
	if (doc != 0)
        elType->ElSSchema = TtaGetSSchema (TEXT("HTML"), doc);
	elType->ElTypeNum = pHTMLGIMapping[i].ThotType;
	return;
      }
      i++;
    }
  while (pHTMLGIMapping[i].XMLname[0] == c);

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
AttributeMapping   *MapAttr (CHAR_T* Attr, SSchema *schema, int elemEntry, Document doc)
#else
AttributeMapping   *MapAttr (Attr, schema, elemEntry, doc)
CHAR_T*               Attr;
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
    if (!ustrcasecmp (XhtmlAttributeMappingTable[i].XMLattribute, Attr))
      if (XhtmlAttributeMappingTable[i].XMLelement[0] == EOS)
        {
	  entry = i;
	  *schema = TtaGetDocumentSSchema (doc);
	}
     else if (elemEntry >= 0 &&
	      !ustrcasecmp (XhtmlAttributeMappingTable[i].XMLelement, pHTMLGIMapping[elemEntry].XMLname))
       {
	 entry = i;
	 *schema = TtaGetDocumentSSchema (doc);
       }
      else
	i++;
    else
      i++;
  while (entry < 0 && XhtmlAttributeMappingTable[i].AttrOrContent != EOS);

  if (entry >= 0)
    return (&XhtmlAttributeMappingTable[entry]);
  else
    return (NULL);
}

/*----------------------------------------------------------------------
   MapHTMLAttribute search in the HTML Attribute Mapping Tables the entry
   for the attribute of name Attr and returns the corresponding Thot
   attribute type.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void           MapHTMLAttribute (CHAR_T* Attr, AttributeType *attrType, CHAR_T* elementName, Document doc)
#else
void           MapHTMLAttribute (Attr, attrType, elementName, doc)
CHAR_T*        Attr;
AttributeType* attrType;
CHAR_T*        elementName;
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
