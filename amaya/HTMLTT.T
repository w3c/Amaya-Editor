
TRANSLATION HTML;

LINELENGTH 78;

COUNTERS
  ItemCounter : Rank of List_Item;
  AnchorCounter : Set 1 on BODY Add 1 on Anchor;

CONST
  TmpFileName = 'test';
  TmpFileExt = '.tmp';
  TmpFileDir = '/tmp/';
  DoubleQuote  = '"';

VAR
  OUT_AnchorCounter : Value(AnchorCounter);
  TmpFile : TmpFileDir TmpFileName TmpFileExt;

RULES

 HTML : BEGIN
	Create '\12' After;
	END;

 HEAD:	BEGIN
	Create in TmpFile '\12' After;
	END;

 Document_URL:
	BEGIN
	Create In TmpFile 'The URL of the document is:  ';
	Create Attributes;
	Create In TmpFile '\12' After;
	{Waiting for a change of HTML.S}
	Remove;
	END;

 TITLE:	BEGIN
	Create '\12\12' After;
	END;

 ISINDEX: BEGIN
	Remove;
	END;

 BASE:	BEGIN
        Create IN TmpFile 'The base of the document is: ';
        Create Attributes;
        Create IN TmpFile '\12' After;
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
	Create 'List of References\12\12' After;
	Include '/tmp/test.tmp' After;
	END;

 H1:	BEGIN
	Create '\12';
	Indent -3;
	Indent +3 After;
	if not within Preformatted Create '\12' After;
	END;

 H2:	BEGIN
	Create '\12';
	if not within Preformatted Create '\12' After;
	END;

 H3:	BEGIN
	Create '\12';
	Indent +3;
	Indent -3 After;
	if not within Preformatted Create '\12' After;
	END;

 H4:	BEGIN
	Create '\12';
	Indent +6;
	Indent -6 After;
	if not within Preformatted Create '\12' After;
	END;

 H5:	BEGIN
	Create '\12';
	Indent +9;
	Indent -9 After;
	if not within Preformatted Create '\12' After;
	END;

 H6:	BEGIN
	Create '\12';
	Indent +12;
	Indent -12 After;
	if not within Preformatted Create '\12' After;
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
	Create DoubleQuote; 
        Create DoubleQuote After;
	END;

 Font_: BEGIN
	Remove;
	END;

 Applet: BEGIN
	Create Attributes;
	Remove;
	Create '\12' After;
	END;

 Parameter: BEGIN
	Remove;
	END;

#ifdef COUGAR
 Object: BEGIN
	Create Attributes;
	Remove;
	Create '\12' After;	
	END;
#endif
 Paragraph: BEGIN
	Create '\12';
	Create '\12' After;
	END;

 Pseudo_paragraph: BEGIN
	Create '\12';
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
	Indent +3;
	Indent -3 After;
	END;

 Term:	BEGIN
	Create '\12' After;
	END;

 Definition: BEGIN
	Indent +5;
	Indent -5 After;
	Create '\12' After;
	END;

 Unnumbered_List: BEGIN
	Create '\12';	
        Indent +3;	
	Indent -3 After;
	END;

 Numbered_List:	BEGIN
	Create '\12';	
        Indent +3;	
	Indent -3 After;
	END;

 Address: BEGIN
	Create Attributes;
	Create '\12' After;
	END;

 Menu:	BEGIN
	Create '\12';
	Create '\12\12'  After;
	END;

 Directory: BEGIN
	Create '\12';
	END;

 List_Item: BEGIN
	Create Attributes;
	Indent +3;
	Indent -3 after;
	Create '\12' After;
	END;

 Preformatted: BEGIN
	NoLineBreak;
	Create Attributes;
	Create '\12' After;
	END;

 Horizontal_Rule: BEGIN
	Create '-----------------------------\12';	
	Remove;
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
		Create Attributes;
		END;
	if NOT within Paragraph AND NOT Immediately within Object BEGIN
		END;
#else
	Create Attributes;
#endif
	END;

 Anchor: BEGIN
	Create IN TmpFile '[';
	Create IN TmpFile  OUT_AnchorCounter;
	Create IN TmpFile  '] ';
	Create Attributes;
	Create IN TmpFile '\12' After;
#ifdef ANCHOR_PREFIX
	Create '[' ;
	Create OUT_AnchorCounter;
	Create ']';
#else
	Create '[' After;
	Create OUT_AnchorCounter After;
	Create ']' After;
#endif
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
	Create '\12';
	Indent +3;
	Create Attributes;
	Indent -3 After;
	Create '\12' After;
	END;

 CAPTION:
	BEGIN
	Indent -3;
	Indent +3 After;
	Create '\12' after;
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
	END;
#endif

 Table_body:
	Get tfoot;

 Table_row:
	BEGIN
	END;

 Data_cell:
	BEGIN
	Create '\12' After;
	END;

 Heading_cell:
	BEGIN
	Create '\12' After;
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
        Create IN TmpFile HREF_;
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
	BEGIN
	END;

 METHOD=Get_:
	BEGIN
	END;

 ENCTYPE: BEGIN
	END;

 NAME: BEGIN
	END;

 Multiple:
	BEGIN
	END;

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
	BEGIN
	END;
 Alignment = Middle_:
	BEGIN
	END;
 Alignment = Bottom_:
	BEGIN
	END;
 Alignment = Left_:
	BEGIN
	END;
 Alignment = Right_:
	BEGIN
	END;

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
	BEGIN
	END;

 USEMAP: BEGIN
	END;

 nohref:
	BEGIN
	END;

 shape = rectangle:
	BEGIN
	END;
 shape = circle:
	BEGIN
	END;
 shape = polygon:
	BEGIN
	END;

 coords: BEGIN
	END;

 Border: BEGIN
	END;

 Table_align = Align_left:
		BEGIN
		END;
 Table_align = Center_:
		BEGIN	
		END;	
 Table_align = Align_right:
		BEGIN
		END;
 cellspacing: BEGIN
	END;

 cellpadding:
	 BEGIN
	END;

 Position = Position_top:
		BEGIN
		END;
 Position = Position_bottom:
		BEGIN
		END;

 Row_align = Row_left:
		BEGIN
		END;
 Row_align = Row_center:
		BEGIN
		END;
 Row_align = Row_right:
		BEGIN
		END;
 Row_valign = Row_top:
		BEGIN
		END;
 Row_valign = Row_middle:
		BEGIN
		END;
 Row_valign = Row_bottom:
		BEGIN
		END;

 colspan: BEGIN
	END;

 rowspan: BEGIN
	END;

 Cell_align = Cell_left:
		BEGIN
		END;
 Cell_align = Cell_center:
		BEGIN
		END;
 Cell_align = Cell_right:
		BEGIN
		END;

 Cell_valign = Cell_top:
		BEGIN
		END;
 Cell_valign = Cell_middle:
		BEGIN
		END;
 Cell_valign = Cell_bottom:
		BEGIN
		END;

 Cell_width: BEGIN
	END;

 Cell_height: BEGIN
	END;

 Word_wrap = No_wrap:
		BEGIN
		END;

 Invalid_attribute:
		BEGIN
		END;

TextTRANSLATE
	BEGIN
	'\240' -> ' '; { &nbsp; }
	'\267' -> '-'; { &middot; }
	'\212' -> '\12'; { &CTLrc; }	
	END;

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













