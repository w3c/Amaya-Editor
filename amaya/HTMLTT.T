
TRANSLATION HTML;

LINELENGTH 78;

COUNTERS
  ItemCounter : Rank of List_Item;

RULES

 HTML : BEGIN
	Create '\12' After;
	END;

 HEAD:	BEGIN
	Remove;
	END;

 Document_URL:
	BEGIN
	Remove;
	END;

 TITLE:	BEGIN
	END;

 ISINDEX: BEGIN
	Remove;
	END;

 BASE:	BEGIN
	Remove;
	END;

 Styles:BEGIN
	END;

 StyleRule:
	BEGIN
	END;

 SCRIPT: BEGIN
	END;

 META: BEGIN
	Remove;
	END;

 LINK:	BEGIN
	Remove;
	END;

 BODY:	BEGIN
	END;

 H1:	BEGIN
	Create '\12\12\12';
	Create '\12\12' After;
	END;

 H2:	BEGIN
	Create '\12\12';
	Create '\12\12' After;
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
#endif
 Paragraph: BEGIN
	Create '\12' After;
	END;

 Pseudo_paragraph: BEGIN
	Create '\12' After;
	END;

 TEXT_UNIT: BEGIN
	IF *Style\240 and not Within HEAD
	    BEGIN
	    Create '<span';
	    Create Attributes;
	    Create '>';
	    Create '</span>' After;
	    END;
	IF *Class and not Within HEAD
	    BEGIN
	    Create '<span';
	    Create Attributes;
	    Create '>';
	    Create '</span>' After;
	    END;
	IF Within Text_Area
		Remove;
	IF Within Text_Input
		Remove;
	END;

 Definition_List: BEGIN
	END;

 Term:	BEGIN
	Create '\12' After;
	END;

 Definition: BEGIN
	Indent +5;
	Indent -5 After;
	END;

 Unnumbered_List: BEGIN
	if not within Unnumbered_List and not within Numbered_List and not
	   within Definition_List
	   BEGIN
	   Indent +3;
	   Indent -3 After;
	   END;
	END;

 Numbered_List:	BEGIN
	if not within Unnumbered_List and not within Numbered_List and not
	   within Definition_List
	   BEGIN
	   Indent +3;
	   Indent -3 After;
	   END;
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
	Create Attributes;
	Indent +3;
	Indent -3 after;
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
	if not within Preformatted Create '>\12';
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
	END;

 Heading_cell:
	BEGIN
	Create '<th';
	Create Attributes;
	Create '>';
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

 http_equiv: BEGIN
	END;

 meta_name: BEGIN
	END;

 meta_content: BEGIN
	END;

 REL:	BEGIN
	END;

 REV:	BEGIN
	END;

 HREF_ :
	BEGIN
	END;

 background_ :
	BEGIN
	END;

 BackgroundColor :
	BEGIN
	END;

 TextColor :
	BEGIN
	END;

 LinkColor :
	BEGIN
	END;

 VisitedLinkColor :
	BEGIN
	END;

 ActiveLinkColor :
	BEGIN
	END;

 BaseFontSize:
	BEGIN
	END;

 Font_size :
	BEGIN
	END;

 color :
	BEGIN
	END;

 Clear = Left_:
	BEGIN
	END;

 Clear = Right_:
	BEGIN
	END;
 Clear = All_:
	BEGIN
	END;
 Clear = None:
	BEGIN
	END;

 Align = left_:
	BEGIN
	END;
 Align = center_:
	BEGIN
	END;
 Align = right_:
	BEGIN
	END;

 NoShade:
	BEGIN
	END;

 Size_: BEGIN
	END;

 Width__: BEGIN
	END;

 codebase: BEGIN
	END;

 code:	BEGIN
	END;

 applet_name: BEGIN
	END;

 Param_name: BEGIN
	END;

 Param_value: BEGIN
	END;
#ifdef COUGAR
 classid: BEGIN
	END;

 data: BEGIN
	END;

 Object_type: BEGIN
	END;

 codetype: BEGIN
	END;
#endif
 Title : BEGIN
	NoTranslation;
	END;

 Class: BEGIN
	END;

 Style\240: BEGIN
	END;

 COMPACT: BEGIN
	END;

 IntItemStyle = disc:
	Create '*  ';
 IntItemStyle = square :
	Create '+  ';
 IntItemStyle = circle :
	Create 'o  ';
 IntItemStyle = Arabic_ :
	Create (Value(ItemCounter, Arabic) '. ');
 IntItemStyle = LowerAlpha :
	Create (Value(ItemCounter, Lowercase) '. ');
 IntItemStyle = UpperAlpha :
	Create (Value(ItemCounter, Uppercase) '. ');
 IntItemStyle = LowerRoman :
	Create (Value(ItemCounter, LRoman) '. ');
 IntItemStyle = UpperRoman :
	Create (Value(ItemCounter, URoman) '. ');

 Start: BEGIN
	END;

 ItemValue: BEGIN
	END;

 WidthElement: BEGIN
	END;

 Script_URL: BEGIN
	END;

 METHOD=Post_:
	Create ' method="post"';

 METHOD=Get_:
	Create ' method="get"';

 ENCTYPE: BEGIN
	END;

 NAME: BEGIN
	END;

 Multiple:
	Create ' multiple';

 MenuSize: BEGIN
	END;

 Selected:
	Create ' selected';

 Value_: BEGIN
	END;

 Rows: BEGIN
	END;

 Columns: BEGIN
	END;

 Checked = Yes_:
	Create ' checked';

 Area_Size: BEGIN
	END;

 MaxLength: BEGIN
	END;

 SRC:	BEGIN
	Create SRC;
	END;

 ALT:	BEGIN
	Create ALT;
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
	END;

 Width_: BEGIN
	END;

 Img_border: BEGIN
	END;

 hspace: BEGIN
	END;

 vspace: BEGIN
	END;

 ISMAP:
	Create ' ismap';

 USEMAP: BEGIN
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
	END;

 Border: BEGIN
	END;

 Table_align = Align_left:
	Create ' align=left';
 Table_align = Center_:
	Create ' align=center';
 Table_align = Align_right:
	Create ' align=right';

 cellspacing: BEGIN
	END;

 cellpadding: BEGIN
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
	END;

 rowspan: BEGIN
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
	END;

 Cell_height: BEGIN
	END;

 Word_wrap = No_wrap:
	Create ' nowrap';

 Invalid_attribute:
	Create Invalid_attribute;

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
