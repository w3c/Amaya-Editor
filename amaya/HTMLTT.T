{   HTML to Text translator rules              }
{   (c) COPYRIGHT MIT and INRIA, 2004.         }
{                                              }
{    Authors: J. Kahan, V. Quint               }

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
  TmpFile : 'amaya_XX.tmp';

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

 STYLE_: BEGIN
	Remove;
	END;

 SCRIPT_: BEGIN
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
	Include 'amaya_XX.tmp' After;
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
	Create '\12\12' After;
	END;

 H3:	BEGIN
	Add H3Counter 1;
	Set H4Counter 0;
	Set H5Counter 0;
	Set H6Counter 0;
	Create '\12\12' After;
	END;

 H4:	BEGIN
	Add H4Counter 1;
	Set H5Counter 0;
	Set H6Counter 0;
	Indent +2;
	Indent -2 After;
	Create '\12\12' After;
	END;

 H5:	BEGIN
	Add H5Counter 1;
	Set H6Counter 0;
	Indent +4;
	Indent -4 After;
	Create '\12\12' After;
	END;

 H6:	BEGIN
	Add H6Counter 1;
	Indent +6;
	Indent -6 After;
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
 Variable_:
	BEGIN
	END;
 Cite:
	BEGIN
	Create DoubleQuote; 
	Create DoubleQuote After;
	END;
 ABBR:
	BEGIN
	END;
 ACRONYM:
	BEGIN
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

 Object: BEGIN
	Create Attributes;
	Remove;
	END;

 Quotation:
	BEGIN
	Create DoubleQuote; 
	Create DoubleQuote After;
	END;

 INS:	BEGIN
	END;

 DEL:	BEGIN
	END;

 Span:	BEGIN
	END;

 BDO:	BEGIN
	END;

 MathML:  BEGIN
        Create '[math]';
	Remove;
	END;

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
	IF Within STYLE_ 
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
	if not within List_
	   Create '\12' After;
	END;

 Numbered_List:	BEGIN
	Indent +3;
	Indent -3 After;
	if not within List_
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
	if within 1 Unnumbered_List and not immediately within Numbered_List
	   Create '*  ';
	if within 2 Unnumbered_List and not immediately within Numbered_List
	   Create 'o  ';
	if within > 2 Unnumbered_List and not immediately within Numbered_List
	   Create '+  ';
  if immediately within Numbered_List
	   Create (Value(ItemCounter, Arabic) '. ');
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

 OptGroup: BEGIN
	END;

 Text_Area: BEGIN
	Create Content;
	Remove;
	END;

 FIELDSET: BEGIN
	END;

 LEGEND: BEGIN
	END;

 LABEL: BEGIN
	END;

 BUTTON_: BEGIN
	Create '[' before;
	Create ']' after;
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

 Button_Input: BEGIN
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
	Remove;
	END;

 IMG: BEGIN
	Create Attributes;
	If NOT ALT 
	  Create '[Image]';
	END;

 Image_Input: BEGIN
	Create Attributes;
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

 Table_:
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

 thead:
	BEGIN
	END;

 tfoot:
	BEGIN
	END;
 tbody:
	BEGIN
	END;

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
 
 Division: 
	If Empty
	   Remove;

 Center:
	If Empty
	   Remove;

 NOSCRIPT:
	If Empty
	   Remove;

 Invalid_element:
	Remove;

 Comment\240:
	Remove;

 Comment_line:
	Remove;

 XMLPI:
	Remove;

 PI_line:
	Remove;

 DOCTYPE:
	Remove;

 DOCTYPE_line:
	Remove;

 CDATA:
	Remove;

 CDATA_line:
	Remove;

 FRAMESET: BEGIN
	END;

 Frames: BEGIN
	END;

 NOFRAMES: BEGIN
	END;

 FRAME: BEGIN
	END;

 IFRAME: BEGIN
	Create '\12';
	Create '\12' After;
	END;

ATTRIBUTES

 profile: BEGIN
	END;

 content_type: BEGIN
	END;

 script_src: BEGIN
	END;

 script_language: BEGIN
        END;

 defer: BEGIN
	END;

 event: BEGIN
	END;

 for_: BEGIN
	END;
 
 http_equiv: BEGIN
	Remove;
	END;

 meta_name: BEGIN
	Remove;
	END;

 meta_content: BEGIN
	Remove;
	END;

 scheme: BEGIN
	END;

 charset: BEGIN
	Remove;
	END;

 hreflang: BEGIN
	Remove;
	END;

 media: BEGIN
	Remove;
	END;

 target_: BEGIN
	Remove;
	END;

 REL:	BEGIN
	Remove;
	END;

 REV:	BEGIN
	Remove;
	END;

 HREF_:
	BEGIN
	If NOT Within LINK AND NOT InternalLink
	   BEGIN
	   Create IN TmpFile HREF_;
	   END;
	END;

 cite: BEGIN
	If NOT InternalLink
	  BEGIN
	    Add AnchorCounter 1;
	    Create In TmpFile  OUT_AnchorCounter;
	    Create In TmpFile ' ';
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

 datetime:
	BEGIN
	END;

 background_:
	BEGIN
	END;

 BackgroundColor:
	BEGIN
	END;

 TextColor:
	BEGIN
	END;

 LinkColor:
	BEGIN
	END;

 VisitedLinkColor:
	BEGIN
	END;

 ActiveLinkColor:
	BEGIN
	END;

 BaseFontSize:
	BEGIN
	END;

 BaseFontColor:
	BEGIN
	END;

 BaseFontFace:
	BEGIN
	END;

 Font_size:
	BEGIN
	END;

 color:
	BEGIN
	END;

 face:
	BEGIN
	END;

 Clear_ = Left_:
	BEGIN
	END;

 Clear_ = Right_: 
	BEGIN
	END;
 Clear_ = All_:
	BEGIN
	END;
 Clear_ = None_:
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

 TextAlign = left_:
	BEGIN
	END;
 TextAlign = center_:
	BEGIN
	END;
 TextAlign = right_:
	BEGIN
	END;
 TextAlign = justify_:
	BEGIN
	END;

 NoShade:
	BEGIN
	END;

 Size_: BEGIN
	END;

 Width__: BEGIN
	END;

 declare: BEGIN
	END;

 codebase: BEGIN
	END;

 code:	BEGIN
	END;

 object: BEGIN
	END;

 applet_name: BEGIN
	END;

 Param_name: BEGIN
	END;

 Param_value: BEGIN
	END;

 valuetype: BEGIN
	END;

 Param_type: BEGIN
	END;

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

 archive: BEGIN
	END;

 standby: BEGIN
	END;

 Title : BEGIN
	NoTranslation;
	END;

 accesskey: BEGIN
	Remove;
	END;

 tabindex: BEGIN
	Remove;
	END;

 Associated_control: BEGIN
	END;

 Button_type: BEGIN
	END;

 Class: BEGIN
	If Empty
	   Remove; 
	END;

 Style\240: BEGIN
	END;

 onclick: BEGIN
        END;
 
 ondblclick: BEGIN
        END;
 
 onmousedown: BEGIN
        END;
 
 onmouseup: BEGIN
        END;
 
 onmouseover: BEGIN
        END;
 
 onmousemove: BEGIN
        END;
 
 onmouseout: BEGIN
        END;
 
 onkeypress: BEGIN
        END;
 
 onkeydown: BEGIN
        END;
 
 onkeyup: BEGIN
        END;
 
 onload: BEGIN
        END;
 
 onunload: BEGIN
        END;
 
 onfocus: BEGIN
        END;
 
 onblur: BEGIN
        END;

 onsubmit: BEGIN
	END;

 onreset: BEGIN
	END;
 
 onselect: BEGIN
	END;
 
 onchange: BEGIN
	END;
 
 COMPACT: BEGIN
	END;

 Start: BEGIN
	END;

 ItemStyle: BEGIN
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

 accept_charset: BEGIN
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

 disabled: BEGIN
	END;

 readonly: BEGIN
	END;

 label:	BEGIN
	END;

 accept: BEGIN
	END;

 LAlign = Top_: BEGIN
	END;
 LAlign = Bottom_: BEGIN
	END;
 LAlign = Left_: BEGIN
	END;
 LAlign = Right_: BEGIN
	END;

 Area_Size: BEGIN
	END;

 MaxLength: BEGIN
	END;

 SRC:	BEGIN
	END;

 type:	BEGIN
	END;

 ALT:	BEGIN
	Create ALT;
	END;

 longdesc: BEGIN
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

 ISAMAP:
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

 summary: BEGIN
	END;

 frame = void: BEGIN
	END;
 frame = above: BEGIN
	END;
 frame = below: BEGIN
	END;
 frame = hsides: BEGIN
	END;
 frame = lhs: BEGIN
	END;
 frame = rhs: BEGIN
	END;
 frame = vsides: BEGIN
	END;
 frame = box: BEGIN
	END;
 frame = border: BEGIN
	END;
 
 rules_ = none_: BEGIN
	END;
 rules_ = groups: BEGIN
	END;
 rules_ = rows: BEGIN
	END;
 rules_ = cols: BEGIN
	END;
 rules_ = all: BEGIN
	END;

 Border: BEGIN
	END;

 cellspacing: BEGIN
	END;

 cellpadding:
	 BEGIN
	END;

 datapagesize: BEGIN
	END;

 Position = Position_top:
		BEGIN
		END;
 Position = Position_bottom:
		BEGIN
		END;
 Position = Position_left:
		BEGIN
		END;
 Position = Position_right:
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
 Row_valign = Row_baseline:
		BEGIN
		END;

 abbr: BEGIN
	END;

 axis: BEGIN
	END;

 headers: BEGIN
	END;

 scope: BEGIN
	END;

 rowspan_: BEGIN
	END;

 colspan_: BEGIN
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
 Cell_align = Cell_justify:
		BEGIN
		END;
 Cell_align = Cell_char:
		BEGIN
		END;

 char:	BEGIN
	END;

 charoff: BEGIN
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
 Cell_valign = Cell_baseline:
		BEGIN
		END;

 No_wrap = no_wrap:
		BEGIN
		END;

 RowHeight:
		BEGIN
		END;
 ColWidth:
		BEGIN
		END;
 FrameSrc:
		BEGIN
		END;
 frameborder = Border1:
		BEGIN
		END;
 frameborder = Border0:
		BEGIN
		END;
 marginwidth:
		BEGIN
		END;
 marginheight:
		BEGIN
		END;
 no_resize:
		BEGIN
		END;
 scrolling = Yes_:
		BEGIN
		END;
 scrolling = No_:
		BEGIN
		END;
 scrolling = auto_:
		BEGIN
		END;

 Invalid_attribute:
		BEGIN
		END;

 Unknown_attribute:
		BEGIN
		END;

 xml_space = xml_space_default:
		BEGIN
		END;

 xml_space = xml_space_preserve:
		BEGIN
		END;

 is_css:
		BEGIN
		END;

 about:
		BEGIN
		END;

 property:
		BEGIN
		END;

 resource:
		BEGIN
		END;

 datatype:
		BEGIN
		END;

 typeof:
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

