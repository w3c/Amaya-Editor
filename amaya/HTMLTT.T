TRANSLATION HTML;

LINELENGTH 78;

COUNTERS
  ItemCounter : Rank of List_Item;
  LOG_AnchorCounter : Set 1 on BODY Add 1 on Anchor;
 
CONST
  TmpFileExt = '.tmp';
  TmpFileName = 'test';
  TmpFileDir = '/tmp/';

VAR
  OUT_AnchorCounter : Value(LOG_AnchorCounter);
  OUT_URL  : HREF_;	
  TmpFile : TmpFileDir TmpFileName TmpFileExt;
#
# Have to do this until include can read the value of a variable
# It should rather be:
#  TmpFile : TmpFileDir FileName TmpFileExt;
#


RULES

 HTML : BEGIN
	END;

 HEAD:	BEGIN
	Remove;
	END;

 Document_URL:
	BEGIN
	Create Attributes;
	END;

 TITLE:	BEGIN
	END;

 ISINDEX: BEGIN
	Remove;
	END;

 BASE:	BEGIN
	Create Attributes;
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
	Indent +3;
	Indent -3 After;
	Create '\12\12' After;
	Create '-------------------------\12' After;
	Create 'Document URL: http://www.w3.org/...\12\12' After;
	Include '/home/kahan/tmp/t' After;
	END;

 H1:	BEGIN
	Indent -3;
	Indent +3 After;
	if not within Preformatted Create '\12\12' After;
	END;

 H2:	BEGIN
	if not within Preformatted Create '\12\12' After;
	END;

 H3:	BEGIN
	Indent +3;
	Indent -3 After;
	if not within Preformatted Create '\12\12' After;
	END;

 H4:	BEGIN
	Indent +6;
	Indent -6 After;
	if not within Preformatted Create '\12\12' After;
	END;

 H5:	BEGIN
	Indent +9;
	Indent -9 After;
	if not within Preformatted Create '\12\12' After;
	END;

 H6:	BEGIN
	Indent +12;
	Indent -12 After;
	if not within Preformatted Create '\12\12' After;
	END;

 BaseFont:
	BEGIN
	Remove;
	END;

 BR:	BEGIN
	Create '\12';
	Remove;
	END;
 Italic_text:
	BEGIN
	END;
 Bold_text:
	BEGIN
	END;
 Teletype_text:
	BEGIN
	END;
 Underlined_text:
	BEGIN
	END;
 Struck_text:
	BEGIN
	END;
 Big_text:
	BEGIN
	END;
 Small_text:
	BEGIN
	END;
 Subscript:
	BEGIN
	END;
 Superscript:
	BEGIN
	END;

 Emphasis:
	BEGIN
	END;
 Strong:
	BEGIN
	END;
 Def:
	BEGIN
	END;
 Code:
	BEGIN
	END;
 Sample:
	BEGIN
	END;
 Keyboard:
	BEGIN
	END;
 Variable:
	BEGIN
	END;
 Cite:
	BEGIN
	END;

 Font_: BEGIN
	Remove;
	END;

 Applet: BEGIN
	END;

 Parameter: BEGIN
	Remove;
	END;

#ifdef COUGAR
Object: BEGIN
	Create '\12';
	Create '\12' After;	
	END;
#endif
 Paragraph: BEGIN
	Create '\12\12' After;
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
	Create '\12';
	Create '\12' After;
	END;

 Term:	BEGIN
	Create '\12\12' After;
	END;

 Definition: BEGIN
	Indent +5;
	Indent -5 After;
	END;

 Unnumbered_List: BEGIN
	if not within Unnumbered_List and not within Numbered_List and not
	   within Definition_List
	   BEGIN
	   Create '\12';
	   Indent +3;
	   Indent -3 After;
	   END;
	END;

 Numbered_List:	BEGIN
	if not within Unnumbered_List and not within Numbered_List and not
	   within Definition_List
	   BEGIN
	   Create '\12';
	   Indent +3;
	   Indent -3 After;
	   END;
	END;

 Address: BEGIN
	Create '\12';
	Create '\12' After;
	END;

 Menu:	BEGIN
	Create '\12' After;
	END;

 Directory: BEGIN
	Create '\12' After;
	END;

 List_Item: BEGIN
	Create Attributes;
	Indent +3;
	Indent -3 after;
	END;

 Preformatted: BEGIN
	NoLineBreak;
	Create '\12' After;
	END;

 Horizontal_Rule: BEGIN
	Create '-----------------------------\12';
	END;

 Form: BEGIN
	Create '\12\12' After;
	END;

 Option_Menu: BEGIN
	Create '\12';
	Create Attributes;
	Create '\12' After;
	END;

 Option: BEGIN
	Create Attributes;
	Create '\12' After;
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
	Remove;
	END;

 Reset_Input: BEGIN
	Create Attributes;
	Create '\12' After;
	Remove;
	END;

 Submit_Input: BEGIN
	Create Attributes;
	Create '\12' After;
	END;

 Block_Quote: BEGIN
	Create '\12';
	Indent +5;
	Indent -5 After;
	Create '\12' After;
	END;

 PICTURE_UNIT: BEGIN
#ifdef COUGAR
	if Immediately within Object
		Remove;
	if NOT Immediately within Object BEGIN
		END;
	if NOT within Paragraph AND NOT Immediately within Object
		Create '\12' After;
#else
	Create Attributes;
	if NOT within Paragraph
		Create '\12' After;
#endif
	END;

 Anchor: BEGIN
	Create IN TmpFile '[' After;
	Create IN TmpFile  OUT_AnchorCounter After;
	Create IN TmpFile  '] ' After;
	Create IN TmpFile  OUT_URL After;
	Create IN TmpFile '\12' After;
	Create '[' After;
	Create OUT_AnchorCounter After;
	Create ']' After;
	END;

 MAP:
	BEGIN
	Remove;
	END;

 AREA:
	BEGIN
	Remove;
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
	Create '</caption>\12' After;
	END;

 Table_head:
	Remove;

#ifdef COUGAR
 thead:
	BEGIN
	Create '\12' After;
	END;

 tfoot:
	BEGIN
	Create '\12' After;
	END;
 tbody:
	BEGIN
	Create '\12' After;
	END;
#endif

 Table_body:
	Get tfoot;

 Table_row:
	BEGIN
	Remove;
	END;

 Data_cell:
	BEGIN
	Create '\12' After;
	END;

 Heading_cell:
	BEGIN
	CREATE '\12' After;
	END;

 Table_foot:
	BEGIN
	Remove;
	END;

 Division: BEGIN
	Create '\12' After;
	END;

 Center: BEGIN
	Create '\12';
# Center text?
	Create '\12' After;
	END;

 Invalid_element:
	BEGIN
	Remove;
	END;

 Comment\240: BEGIN
	Remove;
	END;

 Comment_line: BEGIN
	Remove;
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
	Remove;
#	Create HREF_;
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
	Remove;
	END;

 Param_name: BEGIN
	Remove;
	END;

 Param_value: BEGIN
	Remove;
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
	Remove;
	END;

 ALT:	BEGIN
	Create '[';
	Create ALT;
	Create ']' After;
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






