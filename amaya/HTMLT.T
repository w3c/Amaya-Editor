		{ Author: I. Vatton 	 1994-1995-1996  }

TRANSLATION HTML;

LINELENGTH 78;

CONST
   DoubleQuote  = '"';

RULES

 HTML : BEGIN
	Create '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2//EN">\12';
	Create '<html';
	Create Attributes;
	Create '>\12';
	Create '</html>\12' After;
	END;

 HEAD:	BEGIN
	Create '<head';
	Create Attributes;
	Create '>\12';
	Create '</head>\12' After;
	END;

 Document_URL:
	BEGIN
	Remove;
	END;

 TITLE:	BEGIN
	Create '<title';
	Create Attributes;
	Create '>\12';
	Create '</title>\12' After;
	END;

 ISINDEX: BEGIN
	Create '<isindex';
	IF Prompt
	   BEGIN
	   Create ' prompt="';
	   Create Prompt;
	   Create DoubleQuote;
	   END;
	Create Attributes;
	Remove;
	Create '>\12';
	END;

 BASE:	BEGIN
	Create '<base ';
	Create Attributes;
	Remove;
	Create '>\12';
	END;

 Styles:BEGIN
	Create '<style';
	IF Notation
	   BEGIN
	   Create ' type=';
	   Create Notation;
	   END;
	Create Attributes;
	Create '>\12';
	Create '</style>\12' After;
	END;

 StyleRule:
	BEGIN
	IF Selector
	   BEGIN
	   Create Selector;
	   Create ' { ';
	   Create ' }\12' After;
	   END;
	END;

 SCRIPT: BEGIN
	Create '<script';
	Create Attributes;
	Create '>\12' After;
	END;

 META: BEGIN
	Create '<meta';
	Remove;
	Create Attributes;
	Create '>\12' After;
	END;

 LINK:	BEGIN
	Create '<link';
	Remove;
	Create Attributes;
	Create '>\12' After;
	END;

 BODY:	BEGIN
	Create '<body';
	Create Attributes;
	Create '>\12';
	Create '</body>\12' After;
	END;

 H1:	BEGIN
	Create '<h1';
	Create Attributes;
	Create '>';
	Create '</h1>' After;
	if not within Preformatted Create '\12\12' After;
	END;

 H2:	BEGIN
	Create '<h2';
	Create Attributes;
	Create '>';
	Create '</h2>' After;
	if not within Preformatted Create '\12\12' After;
	END;

 H3:	BEGIN
	Create '<h3';
	Create Attributes;
	Create '>';
	Create '</h3>' After;
	if not within Preformatted Create '\12\12' After;
	END;

 H4:	BEGIN
	Create '<h4';
	Create Attributes;
	Create '>';
	Create '</h4>' After;
	if not within Preformatted Create '\12\12' After;
	END;

 H5:	BEGIN
	Create '<h5';
	Create Attributes;
	Create '>';
	Create '</h5>' After;
	if not within Preformatted Create '\12' After;
	END;

 H6:	BEGIN
	Create '<h6';
	Create Attributes;
	Create '>';
	Create '</h6>' After;
	if not within Preformatted Create '\12' After;
	END;

 BaseFont:
	BEGIN
	Create '<basefont';
	Remove;
	Create Attributes;
	Create '>\12';
	END;

 BR:	BEGIN
	Create '<br';
	Create Attributes;
	Create '>\12';
	Remove;
	END;
 Italic_text:
	BEGIN
	Create '<i';
	Create Attributes;
	Create '>';
	Create '</i>' After;
	END;
 Bold_text:
	BEGIN
	Create '<b';
	Create Attributes;
	Create '>';
	Create '</b>' After;
	END;
 Teletype_text:
	BEGIN
	Create '<tt';
	Create Attributes;
	Create '>';
	Create '</tt>' After;
	END;
 Underlined_text:
	BEGIN
	Create '<u';
	Create Attributes;
	Create '>';
	Create '</u>' After;
	END;
 Struck_text:
	BEGIN
	Create '<strike';
	Create Attributes;
	Create '>';
	Create '</strike>' After;
	END;
 Big_text:
	BEGIN
	Create '<big';
	Create Attributes;
	Create '>';
	Create '</big>' After;
	END;
 Small_text:
	BEGIN
	Create '<small';
	Create Attributes;
	Create '>';
	Create '</small>' After;
	END;
 Subscript:
	BEGIN
	Create '<sub';
	Create Attributes;
	Create '>';
	Create '</sub>' After;
	END;
 Superscript:
	BEGIN
	Create '<sup';
	Create Attributes;
	Create '>';
	Create '</sup>' After;
	END;

 Emphasis:
	BEGIN
	Create '<em';
	Create Attributes;
	Create '>';
	Create '</em>' After;
	END;
 Strong:
	BEGIN
	Create '<strong';
	Create Attributes;
	Create '>';
	Create '</strong>' After;
	END;
 Def:
	BEGIN
	Create '<dfn';
	Create Attributes;
	Create '>';
	Create '</dfn>' After;
	END;
 Code:
	BEGIN
	Create '<code';
	Create Attributes;
	Create '>';
	Create '</code>' After;
	END;
 Sample:
	BEGIN
	Create '<samp';
	Create Attributes;
	Create '>';
	Create '</samp>' After;
	END;
 Keyboard:
	BEGIN
	Create '<kbd';
	Create Attributes;
	Create '>';
	Create '</kbd>' After;
	END;
 Variable:
	BEGIN
	Create '<var';
	Create Attributes;
	Create '>';
	Create '</var>' After;
	END;
 Cite:
	BEGIN
	Create '<cite';
	Create Attributes;
	Create '>';
	Create '</cite>' After;
	END;

 Font_: BEGIN
	Create '<font';
	Create Attributes;
	Create '>';
	Create '</font>' After;
	END;

 Applet: BEGIN
	Create '<applet';
	Create Attributes;
	Create '>\12';
	Create '</applet>\12' After;	
	END;

 Parameter: BEGIN
	Create '<param';
	Create Attributes;
	Remove;
	Create '>\12';
	END;

#ifdef COUGAR
 Object: BEGIN
	Create '<object';
	Create Attributes;
	Create '>\12';
	Create '</object>\12' After;	
	END;

 Span:	BEGIN
	Create '<span';
	Create Attributes;
	Create '>';
	Create '</span>' After;
	END;
#endif
 Paragraph: BEGIN
	Create '<p';
	Create Attributes;
	Create '>\12';
	Create '</p>' After;
	Create '\12' After;
	END;

 Pseudo_paragraph: BEGIN
	Create '\12';
	Create '\12' After;
	END;

 TEXT_UNIT: BEGIN
	IF Within Text_Area
		Remove;
	IF Within Text_Input
		Remove;
	END;

 Definition_List: BEGIN
	Create '<dl';
	Create Attributes;
	Create '>\12';
	Create '</dl>\12' After;
	END;

 Term:	BEGIN
	Create '<dt';
	Create Attributes;
	Create '>';
	Create '</dt>\12' After;
	END;

 Definition: BEGIN
	Create '<dd';
	Create Attributes;
	Create '>';
	Create '</dd>\12' After;
	END;

 Unnumbered_List: BEGIN
	Create '<ul';
	Create Attributes;
	Create '>\12';
	Create '</ul>\12' After;
	END;

 Numbered_List:	BEGIN
	Create '<ol';
	Create Attributes;
	Create '>\12';
	Create '</ol>\12' After;
	END;

 Address: BEGIN
	Create '<address';
	Create Attributes;
	Create '>\12';
	Create '</address>\12' After;
	END;

 Menu:	BEGIN
	Create '<menu';
	Create Attributes;
	Create '>\12';
	Create '</menu>\12' After;
	END;

 Directory: BEGIN
	Create '<dir';
	Create Attributes;
	Create '>\12';
	Create '</dir>\12' After;
	END;

 List_Item: BEGIN
	Create '<li';
	Create Attributes;
	Create '>';
	END;

 Preformatted: BEGIN
	NoLineBreak;
	Create '<pre';
	Create Attributes;
	Create '>';
	Create '</pre>\12' After;
	END;

 Horizontal_Rule: BEGIN
	Create '<hr';
	Create Attributes;
	Remove;
	Create '>';
	if not within Preformatted Create '\12';
	END;

 Form: BEGIN
	Create '<form';
	Create Attributes;
	Create '>\12';
	Create '</form>\12' After;
	END;

 Option_Menu: BEGIN
	Create '\12<select';
	Create Attributes;
	Create '>';
	Create '\12</select>\12' After;
	END;

 Option: BEGIN
	Create '\12<option';
	Create Attributes;
	Create '>';
	END;

 Toggle_Menu: BEGIN
	Create '<!--toggle_menu-->\12<pre>\12';
	Create '</pre>\12' After;
	END;

 Toggle_Item: BEGIN
	Create '\12' After;
	END;

 Radio_Menu: BEGIN
	Create '<!--radio_menu-->\12<pre>\12';
	Create '</pre>\12' After;
	END;

 Radio_Item: BEGIN
	Create '\12' After;
	END;

 Text_Input_Line: BEGIN
	Create '<!--text_input_line-->\12<p>\12';
	Create '</p>\12' After;
	END;

 Command_Line: BEGIN
	Create '<!--command_line-->\12<p>\12';
	Create '</p>\12' After;
	END;

 Text_Area: BEGIN
	Create '<textarea';
	Create Attributes;
	Create '>';
	Create '</textarea>';
	END;

 Checkbox_Input: BEGIN
	Create '<input type=checkbox';
	Create Attributes;
	Create '>';
	Remove;
	END;

 Radio_Input: BEGIN
	Create '<input type=radio';
	Create Attributes;
	Create '>';
	Remove;
	END;

 Text_Input: BEGIN
	Create '<input type=text';
	Create Attributes;
	Create '>';
	Remove;
	END;

 Hidden_Input: BEGIN
	Create '<input type=hidden';
	Create Attributes;
	Create '>';
	Remove;
	END;

 Password_Input: BEGIN
	Create '<input type=password';
	Create Attributes;
	Create '>';
	Remove;
	END;

 File_Input: BEGIN
	Create '<input type=file';
	Create Attributes;
	Create '>';
	Remove;
	END;

 Reset_Input: BEGIN
	Create '\12<input type=reset';
	Create Attributes;
	Create '>';
	Remove;
	END;

 Submit_Input: BEGIN
	Create '\12<input type=submit';
	Create Attributes;
	Create '>';
	Remove;
	END;

 Block_Quote: BEGIN
	Create '<blockquote';
	Create Attributes;
	Create '>\12';
	Create '</blockquote>\12' After;
	END;

 PICTURE_UNIT: BEGIN
#ifdef COUGAR
	if Immediately within Object
		Remove;
	if NOT Immediately within Object BEGIN
		Create '<img';
		Create Attributes;
		Create '>' After;
		END;
	if NOT within Paragraph AND NOT Immediately within Object
		Create '\12' After;
#else
	Create '<img';
	Create Attributes;
	Create '>' After;
	if NOT within Paragraph
		Create '\12' After;
#endif
	END;

 Anchor: BEGIN
	Create '<a';
	Create Attributes;
	Create '>';
	Create '</a>' After;
	END;

 MAP:
	BEGIN
	Create '<map';
	Create Attributes;
	Create '>\12';
	Create '</map>\12' after;
	END;

 AREA:
	BEGIN
	Create '<area';
	Create Attributes;
	Remove;
	Create '>\12';
	END;

{ ---- Tables ---- }

 Table:
	BEGIN
	Create '\12<table';
	Create Attributes;
	Create '>\12';
	Create '</table>\12' after;
	END;

 CAPTION:
	BEGIN
	Create '<caption';
	Create Attributes;
	Create '>';
	Create '</caption>\12' after;
	END;

 Table_head:
	Remove;

#ifdef COUGAR
 thead:
	BEGIN
	Create '<thead';
	Create Attributes;
	Create '>\12';
	Create '</thead>\12' after;
	END;

 tfoot:
	BEGIN
	Create '<tfoot';
	Create Attributes;
	Create '>\12';
	Create '</tfoot>\12' after;
	END;
 tbody:
	BEGIN
	Create '<tbody';
	Create Attributes;
	Create '>\12';
	Create '</tbody>\12' after;
	END;
#endif

 Table_body:
	Get tfoot;

 Table_row:
	BEGIN
	Create '<tr';
	Create Attributes;
	Create '>\12';
	Create '</tr>\12' after;
	END;

 Data_cell:
	BEGIN
	Create '<td';
	Create Attributes;
	Create '>';
	Create '</td>\12' after;
	END;

 Heading_cell:
	BEGIN
	Create '<th';
	Create Attributes;
	Create '>';
	Create '</th>\12' after;
	END;

 Table_foot:
	BEGIN
	Remove;
	END;

 Division: BEGIN
	Create '\12<div';
	Create Attributes;
	Create '>\12';
	Create '</div>\12' After;
	END;

 Center: BEGIN
	Create '\12<center';
	Create Attributes;
	Create '>\12';
	Create '</center>\12' After;
	END;

 Invalid_element:
	BEGIN
	if Error_type = BadPosition
	   Remove;
	if not Error_type = BadPosition
           BEGIN
	   NoTranslation;
	   Create Content;
	   Remove;
	   Create Attributes;
	   Create '>';
	   END;
	END;

 Comment\240: BEGIN
	NoLineBreak;
 	Create '<!--';
	Create '-->\12' after;
	END;

 Comment_line: BEGIN
	if not last create '\12' after;
	END;

ATTRIBUTES

#ifdef COUGAR
 Langue:
	IF not HTML
	    BEGIN
	    Create ' lang="';
	    Create Langue;
	    Create DoubleQuote;
	    END;
#endif

 http_equiv: BEGIN
	Create ' http-equiv="';
	Create http_equiv;
	Create DoubleQuote;
	END;

 meta_name: BEGIN
	Create ' name="';
	Create meta_name;
	Create DoubleQuote;
	END;

 meta_content: BEGIN
	Create ' content="';
	Create meta_content;
	Create DoubleQuote;
	END;

 REL:	BEGIN
	Create ' rel="';
	Create REL;
	Create DoubleQuote;
	END;

 REV:	BEGIN
	Create ' rev="';
	Create REV;
	Create DoubleQuote;
	END;

 HREF_ :
	BEGIN
	NoTranslation;
	Create ' href="';
	Create HREF_;
	Create DoubleQuote;
	END;

 background_ :
	BEGIN
	Create ' background="';
	Create background_;
	Create DoubleQuote;
	END;

 BackgroundColor :
	BEGIN
	Create ' bgcolor="';
	Create BackgroundColor;
	Create DoubleQuote;
	END;

 TextColor :
	BEGIN
	Create ' text="';
	Create TextColor;
	Create DoubleQuote;
	END;

 LinkColor :
	BEGIN
	Create ' link="';
	Create LinkColor;
	Create DoubleQuote;
	END;

 VisitedLinkColor :
	BEGIN
	Create ' vlink="';
	Create VisitedLinkColor;
	Create DoubleQuote;
	END;

 ActiveLinkColor :
	BEGIN
	Create ' alink="';
	Create ActiveLinkColor;
	Create DoubleQuote;
	END;

 BaseFontSize:
	BEGIN
	Create ' size="';
	Create BaseFontSize;
	Create DoubleQuote;
	END;

 Font_size :
	BEGIN
	Create ' size="';
	Create Font_size;
	Create DoubleQuote;
	END;

 color :
	BEGIN
	Create ' color="';
	Create color;
	Create DoubleQuote;
	END;

 Clear = Left_:
	Create ' clear=left';
 Clear = Right_:
	Create ' clear=right';
 Clear = All_:
	Create ' clear=all';
 Clear = None:
	Create ' clear=none';

 Align = left_:
	Create ' align=left';
 Align = center_:
	Create ' align=center';
 Align = right_:
	Create ' align=right';

 NoShade:
	Create ' noshade';

 Size_: BEGIN
	Create ' size="';
	Create Size_;
	Create DoubleQuote;
	END;

 Width__: BEGIN
	Create ' width="';
	Create Width__;
	Create DoubleQuote;
	END;

 codebase: BEGIN
	NoTranslation;
	Create ' codebase="';
	Create codebase;
	Create DoubleQuote;
	END;

 code:	BEGIN
	Create ' code="';
	Create code;
	Create DoubleQuote;
	END;

 applet_name: BEGIN
	Create ' name="';
	Create applet_name;
	Create DoubleQuote;
	END;

 Param_name: BEGIN
	Create ' name="';
	Create Param_name;
	Create DoubleQuote;
	END;

 Param_value: BEGIN
	Create ' value="';
	Create Param_value;
	Create DoubleQuote;
	END;
#ifdef COUGAR
 classid: BEGIN
	Create ' classid="';
	Create classid;
	Create DoubleQuote;
	END;

 data: BEGIN
	Create ' data="';
	Create data;
	Create DoubleQuote;
	END;

 Object_type: BEGIN
	Create ' type="';
	Create Object_type;
	Create DoubleQuote;
	END;

 codetype: BEGIN
	Create ' codetype="';
	Create codetype;
	Create DoubleQuote;
	END;
#endif
 Title : BEGIN
	NoTranslation;
	Create ' title="';
	Create Title;
	Create DoubleQuote;
	END;

 Class: BEGIN
	Create ' class="';
	Create Class;
	Create DoubleQuote;
	END;

 Style\240:
	BEGIN
	IF not Within HEAD
	    BEGIN
	    Create ' style="';
	    Create Style\240;
	    Create DoubleQuote;
	    END;
	END;

#ifdef COUGAR
 ID:	BEGIN
	Create ' id="';
	Create ID;
	Create DoubleQuote;
	END;
#endif

 BulletStyle = disc :
	Create ' type=disc';
 BulletStyle = square :
	Create ' type=square';
 BulletStyle = circle :
	Create ' type=circle';

 COMPACT:
	Create ' compact';

 NumberStyle = Arabic_ :
	Create ' type=1';
 NumberStyle = LowerAlpha :
	Create ' type=a';
 NumberStyle = UpperAlpha :
	Create ' type=A';
 NumberStyle = LowerRoman :
	Create ' type=i';
 NumberStyle = UpperRoman :
	Create ' type=I';

 Start > 1: BEGIN
	Create ' start="';
	Create Start;
	Create DoubleQuote;
	END;

 ItemStyle = disc :
	Create ' type=disc';
 ItemStyle = square :
	Create ' type=square';
 ItemStyle = circle :
	Create ' type=circle';
 ItemStyle = Arabic_ :
	Create ' type=1';
 ItemStyle = LowerAlpha :
	Create ' type=a';
 ItemStyle = UpperAlpha :
	Create ' type=A';
 ItemStyle = LowerRoman :
	Create ' type=i';
 ItemStyle = UpperRoman :
	Create ' type=I';

 ItemValue: BEGIN
	Create ' value="';
	Create ItemValue;
	Create DoubleQuote;
	END;

 WidthElement: BEGIN
	Create ' width="';
	Create WidthElement;
	Create DoubleQuote;
	END;

 Script_URL: BEGIN
	Create ' action="';
	Create Script_URL;
	Create DoubleQuote;
	END;

 METHOD=Post_:
	Create ' method="post"';

 METHOD=Get_:
	Create ' method="get"';

 ENCTYPE: BEGIN
	Create ' enctype="';
	Create ENCTYPE;
	Create DoubleQuote;
	END;

 NAME: BEGIN
	NoTranslation;
	Create ' name="';
	Create NAME;
	Create DoubleQuote;
	END;

 Multiple:
	Create ' multiple';

 MenuSize: BEGIN
	Create ' size="';
	Create MenuSize;
	Create DoubleQuote;
	END;

 Selected:
	Create ' selected';

 Value_: BEGIN
	Create ' value="';
	Create Value_;
	Create DoubleQuote;
	END;

 Rows: BEGIN
	Create ' rows="';
	Create Rows;
	Create DoubleQuote;
	END;

 Columns: BEGIN
	Create ' cols="';
	Create Columns;
	Create DoubleQuote;
	END;

 Checked = Yes_:
	Create ' checked';

 Area_Size: BEGIN
	Create ' size="';
	Create Area_Size;
	Create DoubleQuote;
	END;

 MaxLength: BEGIN
	Create ' maxlength="';
	Create MaxLength;
	Create DoubleQuote;
	END;

 SRC:	BEGIN
	Create ' src="';
	Create SRC;
	Create DoubleQuote;
	END;

 ALT:	BEGIN
	Create ' alt="';
	Create ALT;
	Create DoubleQuote;
	END;

 Alignment = Top_:
	Create ' align=top';
 Alignment = Middle_:
	Create ' align=middle';
 Alignment = Bottom_:
	Create ' align=bottom';
 Alignment = Left_:
	Create ' align=left';
 Alignment = Right_:
	Create ' align=right';

 Height_: BEGIN
	Create ' height="';
	Create Height_;
	Create DoubleQuote;
	END;

 Width_: BEGIN
	Create ' width="';
	Create Width_;
	Create DoubleQuote;
	END;

 Img_border: BEGIN
	Create ' border="';
	Create Img_border;
	Create DoubleQuote;
	END;

 hspace: BEGIN
	Create ' hspace="';
	Create hspace;
	Create DoubleQuote;
	END;

 vspace: BEGIN
	Create ' vspace="';
	Create vspace;
	Create DoubleQuote;
	END;

 ISMAP:
	Create ' ismap';

 USEMAP: BEGIN
	NoTranslation;
	Create ' usemap="';
	Create USEMAP;
	Create DoubleQuote;
	END;

 nohref:
	Create ' nohref';

 shape = rectangle:
	Create ' shape=rect';
 shape = circle:
	Create ' shape=circle';
 shape = polygon:
	Create ' shape=poly';

 coords: BEGIN
	Create ' coords="';
	Create coords;
	Create DoubleQuote;
	END;

 Border: BEGIN
	Create ' border="';
	Create Border;
	Create DoubleQuote;
	END;

 Table_align = Align_left:
	Create ' align=left';
 Table_align = Center_:
	Create ' align=center';
 Table_align = Align_right:
	Create ' align=right';

 cellspacing: BEGIN
	Create ' cellspacing="';
	Create cellspacing;
	Create DoubleQuote;
	END;

 cellpadding: BEGIN
	Create ' cellpadding="';
	Create cellpadding;
	Create DoubleQuote;
	END;

 Position = Position_top:
	 Create ' align=top';
 Position = Position_bottom:
	 Create ' align=bottom';

 Row_align = Row_left:
	 Create ' align=left';
 Row_align = Row_center:
	 Create ' align=center';
 Row_align = Row_right:
	 Create ' align=right';
 Row_valign = Row_top:
	 Create ' valign=top';
 Row_valign = Row_middle:
	 Create ' valign=middle';
 Row_valign = Row_bottom:
	 Create ' valign=bottom';

 colspan: BEGIN
	Create ' colspan="';
	Create colspan;
	Create DoubleQuote;
	END;

 rowspan: BEGIN
	Create ' rowspan="';
	Create rowspan;
	Create DoubleQuote;
	END;

 Cell_align = Cell_left:
	Create ' align=left';
 Cell_align = Cell_center:
	Create ' align=center';
 Cell_align = Cell_right:
	Create ' align=right';

 Cell_valign = Cell_top:
	Create ' valign=top';
 Cell_valign = Cell_middle:
	Create ' valign=middle';
 Cell_valign = Cell_bottom:
	Create ' valign=bottom';

 Cell_width: BEGIN
	Create ' width="';
	Create Cell_width;
	Create DoubleQuote;
	END;

 Cell_height: BEGIN
	Create ' height="';
	Create Cell_height;
	Create DoubleQuote;
	END;

 No_wrap = no_wrap:
	Create ' nowrap';

 Invalid_attribute:
	Create Invalid_attribute;

#include "Text_SGML.inc"

GraphTRANSLATE
	BEGIN
	'c' -> 'circle';
	'Q' -> 'circle';
	'R' -> 'rect';
	'C' -> 'rect';
	' ' -> 'rect';
	'P' -> 'rect';
	'p' -> 'polygon';
	'B' -> 'polygon';
	END;

END
