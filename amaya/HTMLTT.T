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
	Create In TmpFile Content;
	Create In TmpFile '\12' After;
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
	Remove;
	END;

 StyleRule:
	BEGIN
	Remove;
	END;

 SCRIPT: BEGIN
	Remove;
	END;

 META: BEGIN
	Remove;
	END;

 LINK:	BEGIN
	Remove;
	END;


 BODY:	BEGIN
	Indent +4;
	Indent -4 After;
	Create '\12\12' After;
	Create '-------------------------\12' After;
	Create 'List of References\12\12' After;
	Include '/tmp/test.tmp' After;
	END;

 H1:	BEGIN
	Indent -4;
	if not within Preformatted Create '\12';
	Indent +4 After;
	if not within Preformatted Create '\12\12' After;
	END;

 H2:	BEGIN
	Indent -2;
	if not within Preformatted Create '\12';
	Indent +2 After;
	if not within Preformatted Create '\12\12' After;
	END;

 H3:	BEGIN
	if not within Preformatted Create '\12';
	if not within Preformatted Create '\12\12' After;
	END;

 H4:	BEGIN
	Indent +2;
	if not within Preformatted Create '\12';
	Indent -2 After;
	if not within Preformatted Create '\12\12' After;
	END;

 H5:	BEGIN
	Indent +4;
	if not within Preformatted Create '\12';
	Indent -4 After;
	if not within Preformatted Create '\12\12' After;
	END;

 H6:	BEGIN
	Indent +6;
	if not within Preformatted Create '\12';
	Indent -6 After;
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
	If NOT within Paragraph AND NOT within Pseudo_paragraph
       	   Create '\12\12' After;
	END;

 TEXT_UNIT: BEGIN
	IF Within Text_Area
		Remove;
	IF Within Text_Input
		Remove;
        IF Within HEAD and not Within TITLE
	        Remove;
        IF Within STYLE 
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
	Indent +3;
	Indent -3 After;
	END;

 Directory: BEGIN
	Create '\12';
	Indent +3;
	Indent -3 After;
	END;

 List_Item: BEGIN
	Create Attributes;
	Indent +3;
	Indent -3 after;
	END;

 Preformatted: BEGIN
	NoLineBreak;
	Create Attributes;
	Create '\12' After;
	END;

 Horizontal_Rule: BEGIN
	If NOT within Paragraph AND NOT within  Pseudo_paragraph 
           Create '\12';
	Create '-----------------------------\12';	
	Remove;
	END;

 Form: BEGIN
	Create '\12';
	Create '\12' After;
	END;

 Option_Menu: BEGIN
	Remove;
	END;

 Option: BEGIN
	Remove;
	END;

 Toggle_Menu: BEGIN
	Remove;
	END;

 Toggle_Item: BEGIN
	Remove;
	END;

 Radio_Menu: BEGIN
	Remove;
	END;

 Radio_Item: BEGIN
	Remove;
	END;

 Text_Input_Line: BEGIN
	Remove;
	END;

 Command_Line: BEGIN
	Remove;
	END;

 Text_Area: BEGIN
	Remove;
	END;

 Checkbox_Input: BEGIN
	Remove;
	END;

 Radio_Input: BEGIN
	Remove;
	END;

 Text_Input: BEGIN
	Remove;
	END;

 Hidden_Input: BEGIN
	Remove;
	END;

 Password_Input: BEGIN
	Remove;
	END;

 File_Input: BEGIN
	Remove;
	END;

 Reset_Input: BEGIN
	Remove;
	END;

 Submit_Input: BEGIN
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
#else
	Create Attributes;
#endif
	if NOT ALT 
	  Create '[Image]';
	END;


 Anchor: BEGIN
	if HREF_ AND NOT InternalLink
	  BEGIN
  	    Create In TmpFile '[';
	    Create In TmpFile  OUT_AnchorCounter;
	    Create In TmpFile  '] ';
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
	Create '\12';
	END;

 Center: BEGIN
	Create '\12';

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
	Remove;
	END;

 meta_name: BEGIN
	Remove;
	END;

 meta_content: BEGIN
	Remove;
	END;

 REL:	BEGIN
	Remove;
	END;

 REV:	BEGIN
	Remove;
	END;

 HREF_ :
	BEGIN
	If NOT Within LINK AND NOT InternalLink
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
	Remove;
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

