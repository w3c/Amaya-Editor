{HTMLTT -> HTML to Text translator rules}
{(c) COPYRIGHT MIT and INRIA, 1996. }
{Please first read the full copyright statement in file COPYRIGHT.}
{ Authors: J. Kahan/V. Quint 1996 }

TRANSLATION HTML;

LINELENGTH 78;

COUNTERS
  ItemCounter : Rank of List_Item;
  H2Counter;
  H3Counter;
  H4Counter;
  H5Counter;
  H6Counter;
  AnchorCounter;

CONST
  DoubleQuote  = '"';
  DashLine = '--------------------------------------------------------------';
VAR
  OUT_AnchorCounter : '[' Value(AnchorCounter) ']';
  TmpFile : '/tmp/amaya.tmp';

RULES

 HTML : BEGIN
	Create '\12' After;
	END;

 HEAD:	BEGIN
	Create In TmpFile '\12' After;
	END;

 Document_URL:
	BEGIN
	NoLineBreak;
	Create In TmpFile 'Document''s URL:  ';
	Create In TmpFile Content;
	Create In TmpFile '\12' After;
	Remove;
	END;

 TITLE:	BEGIN
	Create 'Document''s Title: ';
	Create '\12\12' After;
	END;

 ISINDEX: BEGIN
	Remove;
	END;

 BASE:	BEGIN
	NoLineBreak;
	Create IN TmpFile 'Document''s BASE: ';
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
	Set H2Counter 0;
	Set H3Counter 0;
	Set H4Counter 0;
	Set H5Counter 0;
	Set H6Counter 0;
	Set AnchorCounter 0;
	Indent +4;
	Indent -4 After;
	{ --- print the List of References ---}
	Create '\12\12' After;
	Create (DashLine) After;
	Create '\12' After;
	Create 'List of References\12\12' After;
	Include '/tmp/amaya.tmp' After;
	RemoveFile TmpFile After;
	END;

 H1:	BEGIN
	Set H2Counter 0;
	Set H3Counter 0;
	Set H4Counter 0;
	Set H5Counter 0;
	Set H6Counter 0;
	Indent -4;
	Indent +4 After;
	Create '\12\12' After;
	END;

 H2:	BEGIN
	Add H2Counter 1;
	Set H3Counter 0;
	Set H4Counter 0;
	Set H5Counter 0;
	Set H6Counter 0;
	Indent -2;
	Indent +2 After;
	if SectionNumbering
	   Create (Value(H2Counter, Arabic) '  ');
	Create '\12\12' After;
	END;

 H3:	BEGIN
	Add H3Counter 1;
	Set H4Counter 0;
	Set H5Counter 0;
	Set H6Counter 0;
	if SectionNumbering
	   Create (Value(H2Counter, Arabic) '.'
		   Value(H3Counter, Arabic) '  ');
	Create '\12\12' After;
	END;

 H4:	BEGIN
	Add H4Counter 1;
	Set H5Counter 0;
	Set H6Counter 0;
	Indent +2;
	Indent -2 After;
	if SectionNumbering
	   Create (Value(H2Counter, Arabic) '.'
		   Value(H3Counter, Arabic) '.'
		   Value(H4Counter, Arabic) '  ');
	Create '\12\12' After;
	END;

 H5:	BEGIN
	Add H5Counter 1;
	Set H6Counter 0;
	Indent +4;
	Indent -4 After;
	if SectionNumbering
	   Create (Value(H2Counter, Arabic) '.'
		   Value(H3Counter, Arabic) '.'
		   Value(H4Counter, Arabic) '.'
		   Value(H5Counter, Arabic) '  ');
	Create '\12\12' After;
	END;

 H6:	BEGIN
	Add H6Counter 1;
	Indent +6;
	Indent -6 After;
	if SectionNumbering
	   Create (Value(H2Counter, Arabic) '.'
		   Value(H3Counter, Arabic) '.'
		   Value(H4Counter, Arabic) '.'
		   Value(H5Counter, Arabic) '.'
		   Value(H6Counter, Arabic) '  ');

	Create '\12\12' After;
	END;

 BaseFont:
	BEGIN
	Remove;
	END;

 BR:	BEGIN
	If Within 1 BR
	   Remove;
	If Last
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
	END;

 Applet: BEGIN
	Create Attributes;
	Remove;
	END;

 Parameter: BEGIN
	Remove;
	END;

#ifdef COUGAR
 Object: BEGIN
	Create Attributes;
	Remove;
	END;
#endif
#ifdef MATHML
 Math:  BEGIN
        Create '[math]';
	Remove;
	END;
 MathDisp: BEGIN
        Create '\12[math]\12';
        Remove;
        END;
#endif
 Paragraph:
	BEGIN
	Create '\12' After;
	if not within List_Item and not within Definition
  	    Create '\12' After;
	END;

 Pseudo_paragraph:
	BEGIN
	Create '\12' After;
	if not within List_Item and not within Definition
	    Create '\12' After;
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
	Create '\12' After;
	END;

 Term:	BEGIN
 	Create '\12' After;
	END;

 Definition: BEGIN
	Indent +5;
	Indent -5 After;
	END;

 Unnumbered_List: BEGIN
	Indent +3;
	Indent -3 After;
	Create '\12' After;
	END;

 Numbered_List:	BEGIN
	Indent +3;
	Indent -3 After;
	Create '\12' After;
	END;

 Address: BEGIN
	Create Attributes;
	Create '\12\12' After;
	END;

 Menu:	BEGIN
	Indent +3;
	Indent -3 After;
	END;

 Directory: BEGIN
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
	Create '\12\12' After;
	END;

 Horizontal_Rule: BEGIN
	Create (DashLine) After;
	Create '\12\12' After;
	Remove;
	END;

 Form: BEGIN
	END;

 Option_Menu: BEGIN
	END;

 Option: BEGIN
	If Selected = Yes_
	   Create Content;
	Remove;
	END;

 Toggle_Menu: BEGIN
	END;

 Toggle_Item: BEGIN
	If NOT Selected
	   Remove;
	Create Content;
	Remove;
	END;

 Radio_Menu: BEGIN
	END;

 Radio_Item: BEGIN
       If NOT Selected
	   Remove;
	Create Content;
	Remove;
	END;

 Text_Input_Line: BEGIN
	If Empty
	   Remove;
	END;

 Command_Line: BEGIN
	END;

 Text_Area: BEGIN
	Create Content;
	Remove;
	END;

 Checkbox_Input: BEGIN
	If Checked = Yes_
	   Create '[x] ';
	If Checked = No_
	   Create '[ ] ';
	END;

 Radio_Input: BEGIN
	If Checked =  Yes_
	   Create '(x)';
	If Checked = No_
	   Create '( )';
	Remove;
	END;

 Text_Input: BEGIN
	Create Content;
	Remove;
	END;

 Hidden_Input: BEGIN
	Remove;
	END;

 Password_Input: BEGIN
	Remove;
	END;

 File_Input: BEGIN
	Create Content;
	Remove;
	END;

 Reset_Input: BEGIN
	Create '<<';
	Create Value_;
	Create '>>';
	If Last
  	   Create '\12';
	Remove;
	END;

 Submit_Input: BEGIN
	Create '<<';
	Create Value_;
	Create '>>';
	If Last
  	   Create '\12';
	Remove;

	END;

 Block_Quote: BEGIN
	Indent +5;
	Indent -5 After;
	END;

 PICTURE_UNIT: BEGIN
#ifdef COUGAR
	If Immediately within Object
		Remove;
	If NOT Immediately within Object BEGIN
		Create Attributes;
		END;
#else
	Create Attributes;
#endif
	If NOT ALT 
	  Create '[Image]';
	END;


 Anchor: BEGIN
	If HREF_ AND NOT InternalLink
	  BEGIN
	    Add AnchorCounter 1;
	    Create In TmpFile  OUT_AnchorCounter;
	    Create In TmpFile ' ';
 	    Create Attributes;
	    Create IN TmpFile '\12' After;
#ifdef ANCHOR_PREFIX
	    Create OUT_AnchorCounter;
	    Create Content;
	    Remove;
#else
	    Create OUT_AnchorCounter After;
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
	Indent +3;
	Create Attributes;
	Indent -3 After;
	END;

 CAPTION:
	BEGIN
	Indent -3;
	Indent +3 After;
	END;

 Table_head:
	Remove;

#ifdef COUGAR
 thead:
	BEGIN
	END;

 tfoot:
	BEGIN
	END;
 tbody:
	BEGIN
	END;
#endif

 Table_body:
	BEGIN
	END;

 Table_row:
	If Empty
	   Remove;

 Data_cell:
	If Empty
	   Remove;

 Heading_cell:
	If Empty
	   Remove;
 
 Table_foot:
	Remove;

 Division: 
	If Empty
	   Remove;

 Center:
	If Empty
	   Remove;

 Invalid_element:
	If Error_type = BadPosition
	   Remove;

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

 Link_type: BEGIN
	END;

 codetype: BEGIN
	END;
#endif
 Title : BEGIN
	NoTranslation;
	END;

 Class: BEGIN
	If Empty
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

 Selected: BEGIN
	END;

 Value_: BEGIN
	END;

 Rows: BEGIN
	END;

 Columns: BEGIN
	END;

 Checked = Yes_: BEGIN
	END;

 Area_Size: BEGIN
	END;

 MaxLength: BEGIN
	END;

 SRC:	BEGIN
	Remove;
	END;

 ALT:	BEGIN
	Create ALT;
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

#ifdef MATHML
 mode = inline_math:
	BEGIN
	END;
 mode = display:
	BEGIN
	END;
#endif

 Border: BEGIN
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

 colspan_: BEGIN
	END;

 rowspan_: BEGIN
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

 Cell_height: BEGIN
	END;

 No_wrap = no_wrap:
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

