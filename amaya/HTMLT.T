		{ Author: I. Vatton 	 1994-1995-1996  }

TRANSLATION HTML;

LINELENGTH 78;

CONST
   QuoteF  = '"';
   C_Style = ' style="';

RULES

 HTML : BEGIN
	Create '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2 Final//EN">\12';
	Create '<HTML>\12';
	Create '</HTML>\12' After;
	END;

 HEAD:	BEGIN
	Create '<HEAD>\12';
	Create '</HEAD>\12' After;
	END;

 Document_URL:
	BEGIN
	Remove;
	END;

 TITLE:	BEGIN
	Create '<TITLE>';
	Create '</TITLE>\12' After;
	END;

 ISINDEX: BEGIN
	Create '<ISINDEX';
	IF Prompt
	   BEGIN
	   Create ' prompt="';
	   Create Prompt;
	   Create QuoteF;
	   END;
	Create Attributes;
	Remove;
	Create '>\12';
	END;

 BASE:	BEGIN
	Create '<BASE ';
	Create Attributes;
	Remove;
	Create '>\12';
	END;

 Styles:BEGIN
	Create '<STYLE';
	IF Notation
	   BEGIN
	   Create ' type=';
	   Create Notation;
	   END;
	Create Attributes;
	Create '>\12';
	Create '</STYLE>\12' After;
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
	Create '<SCRIPT';
	Create Attributes;
	Create '>\12' After;
	END;

 META: BEGIN
	Create '<META';
	Remove;
	Create Attributes;
	Create '>\12' After;
	END;

 LINK:	BEGIN
	Create '<LINK';
	Remove;
	Create Attributes;
	Create '>\12' After;
	END;

 BODY:	BEGIN
	Create '<BODY';
	Create Attributes;
	Create '>\12';
	Create '</BODY>\12' After;
	END;

 H1:	BEGIN
	Create '<H1';
	Create Attributes;
	Create '>';
	Create '</H1>\12\12' After;
	END;

 H2:	BEGIN
	Create '<H2';
	Create Attributes;
	Create '>';
	Create '</H2>\12\12' After;
	END;

 H3:	BEGIN
	Create '<H3';
	Create Attributes;
	Create '>';
	Create '</H3>\12\12' After;
	END;

 H4:	BEGIN
	Create '<H4';
	Create Attributes;
	Create '>';
	Create '</H4>\12\12' After;
	END;

 H5:	BEGIN
	Create '<H5';
	Create Attributes;
	Create '>';
	Create '</H5>\12' After;
	END;

 H6:	BEGIN
	Create '<H6';
	Create Attributes;
	Create '>';
	Create '</H6>\12' After;
	END;

 BaseFont:
	BEGIN
	Create '<BASEFONT';
	Remove;
	Create Attributes;
	Create '>\12';
	END;

 BR:	BEGIN
	Create '<BR';
	Create Attributes;
	Create '>';
	Remove;
	END;
 Italic_text:
	BEGIN
	Create '<I';
	Create Attributes;
	Create '>';
	Create '</I>' After;
	END;
 Bold_text:
	BEGIN
	Create '<B';
	Create Attributes;
	Create '>';
	Create '</B>' After;
	END;
 Teletype_text:
	BEGIN
	Create '<TT';
	Create Attributes;
	Create '>';
	Create '</TT>' After;
	END;
 Underlined_text:
	BEGIN
	Create '<U';
	Create Attributes;
	Create '>';
	Create '</U>' After;
	END;
 Struck_text:
	BEGIN
	Create '<STRIKE';
	Create Attributes;
	Create '>';
	Create '</STRIKE>' After;
	END;
 Big_text:
	BEGIN
	Create '<BIG';
	Create Attributes;
	Create '>';
	Create '</BIG>' After;
	END;
 Small_text:
	BEGIN
	Create '<SMALL';
	Create Attributes;
	Create '>';
	Create '</SMALL>' After;
	END;
 Subscript:
	BEGIN
	Create '<SUB';
	Create Attributes;
	Create '>';
	Create '</SUB>' After;
	END;
 Superscript:
	BEGIN
	Create '<SUP';
	Create Attributes;
	Create '>';
	Create '</SUP>' After;
	END;

 Emphasis:
	BEGIN
	Create '<EM';
	Create Attributes;
	Create '>';
	Create '</EM>' After;
	END;
 Strong:
	BEGIN
	Create '<STRONG';
	Create Attributes;
	Create '>';
	Create '</STRONG>' After;
	END;
 Def:
	BEGIN
	Create '<DFN';
	Create Attributes;
	Create '>';
	Create '</DFN>' After;
	END;
 Code:
	BEGIN
	Create '<CODE';
	Create Attributes;
	Create '>';
	Create '</CODE>' After;
	END;
 Sample:
	BEGIN
	Create '<SAMP';
	Create Attributes;
	Create '>';
	Create '</SAMP>' After;
	END;
 Keyboard:
	BEGIN
	Create '<KBD';
	Create Attributes;
	Create '>';
	Create '</KBD>' After;
	END;
 Variable:
	BEGIN
	Create '<VAR';
	Create Attributes;
	Create '>';
	Create '</VAR>' After;
	END;
 Cite:
	BEGIN
	Create '<CITE';
	Create Attributes;
	Create '>';
	Create '</CITE>' After;
	END;

 Font_: BEGIN
	Create '<FONT';
	Create Attributes;
	Create '>';
	Create '</FONT>' After;
	END;

 Applet: BEGIN
	Create '<APPLET';
	Create Attributes;
	Create '>\12';
	Create '</APPLET>\12' After;	
	END;

 Parameter: BEGIN
	Create '<PARAM';
	Create Attributes;
	Remove;
	Create '>\12';
	END;

#ifdef COUGAR
 Object: BEGIN
	Create '<OBJECT';
	Create Attributes;
	Create '>\12';
	Create '</OBJECT>\12' After;	
	END;
#endif
 Paragraph: BEGIN
	if not firstwithin List_Item and not firstwithin Definition
	   BEGIN
	   Create '<P';
	   Create Attributes;
	   Create '>\12';
	   Create '</P>' After;
	   END;
	Create '\12' After;
	END;

 TEXT_UNIT: BEGIN
	IF *Style\240 and not Within HEAD
	    BEGIN
	    Create '<SPAN';
	    Create Attributes;
	    Create '>';
	    Create '</SPAN>' After;
	    END;
	IF *Class and not Within HEAD
	    BEGIN
	    Create '<SPAN';
	    Create Attributes;
	    Create '>';
	    Create '</SPAN>' After;
	    END;
	IF Within Text_Area
		Remove;
	IF Within Text_Input
		Remove;
	END;

 Definition_List: BEGIN
	Create '<DL';
	Create Attributes;
	Create '>\12';
	Create '</DL>\12' After;
	END;

 Term:	BEGIN
	Create '<DT';
	Create Attributes;
	Create '>';
	Create '</DT>\12' After;
	END;

 Definition: BEGIN
	Create '<DD';
	Create Attributes;
	Create '>';
	Create '</DD>\12' After;
	END;

 Unnumbered_List: BEGIN
	Create '<UL';
	Create Attributes;
	Create '>\12';
	Create '</UL>\12' After;
	END;

 Numbered_List:	BEGIN
	Create '<OL';
	Create Attributes;
	Create '>\12';
	Create '</OL>\12' After;
	END;

 Address: BEGIN
	Create '<ADDRESS';
	Create Attributes;
	Create '>\12';
	Create '</ADDRESS>\12' After;
	END;

 Menu:	BEGIN
	Create '<MENU';
	Create Attributes;
	Create '>\12';
	Create '</MENU>\12' After;
	END;

 Directory: BEGIN
	Create '<DIR';
	Create Attributes;
	Create '>\12';
	Create '</DIR>\12' After;
	END;

 List_Item: BEGIN
	Create '<LI';
	Create Attributes;
	Create '>';
	END;

 Preformatted: BEGIN
	NoLineBreak;
	Create '<PRE';
	Create Attributes;
	Create '>';
	Create '\12</PRE>\12' After;
	END;

 Pre_Line : BEGIN
	Create '\12';
	END;

 Horizontal_Rule: BEGIN
	Create '<HR';
	Create Attributes;
	Remove;
	Create '>\12';
	END;

 Form: BEGIN
	Create '<FORM';
	Create Attributes;
	Create '>\12';
	Create '</FORM>\12' After;
	END;

 Option_Menu: BEGIN
	Create '\12<SELECT';
	Create Attributes;
	Create '>';
	Create '\12</SELECT>\12' After;
	END;

 Option: BEGIN
	Create '\12<OPTION';
	Create Attributes;
	Create '>';
	END;

 Toggle_Menu: BEGIN
	Create '<!--Toggle_Menu-->\12<PRE>\12';
	Create '</PRE>\12' After;
	END;

 Toggle_Item: BEGIN
	Create '\12' After;
	END;

 Radio_Menu: BEGIN
	Create '<!--Radio_Menu-->\12<PRE>\12';
	Create '</PRE>\12' After;
	END;

 Radio_Item: BEGIN
	Create '\12' After;
	END;

 Text_Input_Line: BEGIN
	Create '<!--Text_Input_Line-->\12<P>\12';
	Create '</P>\12' After;
	END;

 Command_Line: BEGIN
	Create '<!--Command_Line-->\12<P>\12';
	Create '</P>\12' After;
	END;

 Text_Area: BEGIN
	Create '<TEXTAREA';
	Create Attributes;
	Create '>';
	Create '</TEXTAREA>';
	END;

 Checkbox_Input: BEGIN
	Create '<INPUT type=CHECKBOX';
	Create Attributes;
	Create '>';
	Remove;
	END;

 Radio_Input: BEGIN
	Create '<INPUT type=RADIO';
	Create Attributes;
	Create '>';
	Remove;
	END;

 Text_Input: BEGIN
	Create '<INPUT type=TEXT';
	Create Attributes;
	Create '>';
	Remove;
	END;

 Hidden_Input: BEGIN
	Create '<INPUT type=HIDDEN';
	Create Attributes;
	Create '>';
	Remove;
	END;

 Password_Input: BEGIN
	Create '<INPUT type=PASSWORD';
	Create Attributes;
	Create '>';
	Remove;
	END;

 File_Input: BEGIN
	Create '<INPUT type=FILE';
	Create Attributes;
	Create '>';
	Remove;
	END;

 Reset_Input: BEGIN
	Create '\12<INPUT type=RESET';
	Create Attributes;
	Create '>';
	Remove;
	END;

 Submit_Input: BEGIN
	Create '\12<INPUT type=SUBMIT';
	Create Attributes;
	Create '>';
	Remove;
	END;

 Block_Quote: BEGIN
	Create '<BLOCKQUOTE';
	Create Attributes;
	Create '>\12';
	Create '</BLOCKQUOTE>\12' After;
	END;

 PICTURE_UNIT: BEGIN
#ifdef COUGAR
	if Immediately within Object
		Remove;
	if NOT Immediately within Object BEGIN
		Create '<IMG';
		Create Attributes;
		Create '>' After;
		END;
	if NOT within Paragraph AND NOT Immediately within Object
		Create '\12' After;
#else
	Create '<IMG';
	Create Attributes;
	Create '>' After;
	if NOT within Paragraph
		Create '\12' After;
#endif
	END;

 Anchor: BEGIN
	Create '<A';
	Create Attributes;
	Create '>';
	Create '</A>' After;
	END;

 MAP:
	BEGIN
	Create '<MAP';
	Create Attributes;
	Create '>\12';
	Create '</MAP>\12' after;
	END;

 AREA:
	BEGIN
	Create '<AREA';
	Create Attributes;
	Remove;
	Create '>\12';
	END;

{ ---- Tables ---- }

 Table:
	BEGIN
	Create '\12<TABLE';
	Create Attributes;
	Create '>\12';
	Create '</TABLE>\12' after;
	END;

 CAPTION:
	BEGIN
	Create '<CAPTION';
	Create Attributes;
	Create '>';
	Create '</CAPTION>\12' after;
	END;

 Table_head:
	Remove;

{ provision for COUGAR DTD
 thead:
	BEGIN
	Create '<THEAD';
	Create Attributes;
	Create '>\12';
	Create '</THEAD>\12' after;
	END;

 tfoot:
	BEGIN
	Create '<TFOOT';
	Create Attributes;
	Create '>\12';
	Create '</TFOOT>\12' after;
	END;
 tbody:
	BEGIN
	Create '<TBODY';
	Create Attributes;
	Create '>\12';
	Create '</TBODY>\12' after;
	END;
}

 Table_body:
	Get tfoot;

 Table_row:
	BEGIN
	Create '  <TR';
	Create Attributes;
	Create '>\12';
	Create '  </TR>\12' after;
	END;

 Data_cell:
	BEGIN
	Create '<TD';
	Create Attributes;
	Create '>';
	END;

 Heading_cell:
	BEGIN
	Create '<TH';
	Create Attributes;
	Create '>';
	END;

 Table_foot:
	BEGIN
	Remove;
	END;

 Division: BEGIN
	Create '\12<DIV';
	Create Attributes;
	Create '>\12';
	Create '</DIV>\12' After;
	END;

 Center: BEGIN
	Create '\12<CENTER';
	Create Attributes;
	Create '>\12';
	Create '</CENTER>\12' After;
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
	Create ' http-equiv="';
	Create http_equiv;
	Create QuoteF;
	END;

 meta_name: BEGIN
	Create ' name="';
	Create meta_name;
	Create QuoteF;
	END;

 meta_content: BEGIN
	Create ' content="';
	Create meta_content;
	Create QuoteF;
	END;

 REL:	BEGIN
	Create ' rel="';
	Create REL;
	Create QuoteF;
	END;

 REV:	BEGIN
	Create ' rev="';
	Create REV;
	Create QuoteF;
	END;

 HREF_ :
	BEGIN
	NoTranslation;
	Create ' href="';
	Create HREF_;
	Create QuoteF;
	END;

 background_ :
	BEGIN
	Create ' background="';
	Create background_;
	Create QuoteF;
	END;

 BackgroundColor :
	BEGIN
	Create ' bgcolor="';
	Create BackgroundColor;
	Create QuoteF;
	END;

 TextColor :
	BEGIN
	Create ' text="';
	Create TextColor;
	Create QuoteF;
	END;

 LinkColor :
	BEGIN
	Create ' link="';
	Create LinkColor;
	Create QuoteF;
	END;

 VisitedLinkColor :
	BEGIN
	Create ' vlink="';
	Create VisitedLinkColor;
	Create QuoteF;
	END;

 ActiveLinkColor :
	BEGIN
	Create ' alink="';
	Create ActiveLinkColor;
	Create QuoteF;
	END;

 BaseFontSize:
	BEGIN
	Create ' size=';
	Create BaseFontSize;
	END;

 Font_size :
	BEGIN
	Create ' size="';
	Create Font_size;
	Create QuoteF;
	END;

 color :
	BEGIN
	Create ' color="';
	Create color;
	Create QuoteF;
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
	Create QuoteF;
	END;

 Width__: BEGIN
	Create ' width="';
	Create Width__;
	Create QuoteF;
	END;

 codebase: BEGIN
	NoTranslation;
	Create ' codebase="';
	Create codebase;
	Create QuoteF;
	END;

 code:	BEGIN
	Create ' code="';
	Create code;
	Create QuoteF;
	END;

 applet_name: BEGIN
	Create ' name="';
	Create applet_name;
	Create QuoteF;
	END;

 Param_name: BEGIN
	Create ' name="';
	Create Param_name;
	Create QuoteF;
	END;

 Param_value: BEGIN
	Create ' value="';
	Create Param_value;
	Create QuoteF;
	END;
#ifdef COUGAR
 classid: BEGIN
	Create ' classid="';
	Create classid;
	Create QuoteF;
	END;

 data: BEGIN
	Create ' data="';
	Create data;
	Create QuoteF;
	END;

 Object_type: BEGIN
	Create ' type="';
	Create Object_type;
	Create QuoteF;
	END;

 codetype: BEGIN
	Create ' codetype="';
	Create codetype;
	Create QuoteF;
	END;
#endif
 Title : BEGIN
	NoTranslation;
	Create ' title="';
	Create Title;
	Create QuoteF;
	END;

 Class: BEGIN
	IF Within TEXT_UNIT
	    BEGIN
	    Create '<SPAN class="';
	    Create Class;
	    Create '">';
	    Create '</SPAN>' After;
	    END;
	IF NOT Within TEXT_UNIT
	    BEGIN
	    Create ' class="';
	    Create Class;
	    Create QuoteF;
	    END;
	END;

 Style\240:
	BEGIN
	IF Within TEXT_UNIT and not Within HEAD 
	    BEGIN
	    Create '<SPAN ';
	    Create C_Style;
	    Create Style\240;
	    Create QuoteF;
	    Create '>';
	    Create '</SPAN>' After;
	    END;
	IF not Within TEXT_UNIT and not Within HEAD
	    BEGIN
	    Create C_Style;
	    Create Style\240;
	    Create QuoteF;
	    END;
	END;

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
	Create QuoteF;
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
	Create QuoteF;
	END;

 WidthElement: BEGIN
	Create ' width="';
	Create WidthElement;
	Create QuoteF;
	END;

 Script_URL: BEGIN
	Create ' action="';
	Create Script_URL;
	Create QuoteF;
	END;

 METHOD=Post_:
	Create ' method="Post"';

 METHOD=Get_:
	Create ' method="Get"';

 ENCTYPE: BEGIN
	Create ' enctype="';
	Create ENCTYPE;
	Create QuoteF;
	END;

 NAME: BEGIN
	NoTranslation;
	Create ' name="';
	Create NAME;
	Create QuoteF;
	END;

 Multiple:
	Create ' multiple';

 MenuSize: BEGIN
	Create ' size=';
	Create MenuSize;
	END;

 Selected:
	Create ' selected';

 Value_: BEGIN
	Create ' value="';
	Create Default_Value;
	Create QuoteF;
	END;

 Rows: BEGIN
	Create ' rows=';
	Create Rows;
	END;

 Columns: BEGIN
	Create ' cols=';
	Create Columns;
	END;

 Checked = Yes_:
	Create ' checked';

 Area_Size: BEGIN
	Create ' size=';
	Create Area_Size;
	END;

 MaxLength: BEGIN
	Create ' maxlength=';
	Create MaxLength;
	END;

 SRC:	BEGIN
	Create ' src="';
	Create SRC;
	Create QuoteF;
	END;

 ALT:	BEGIN
	Create ' alt="';
	Create ALT;
	Create QuoteF;
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
	Create QuoteF;
	END;

 Width_: BEGIN
	Create ' width="';
	Create Width_;
	Create QuoteF;
	END;

 Img_border: BEGIN
	Create ' border="';
	Create Img_border;
	Create QuoteF;
	END;

 hspace: BEGIN
	Create ' hspace="';
	Create hspace;
	Create QuoteF;
	END;

 vspace: BEGIN
	Create ' vspace="';
	Create vspace;
	Create QuoteF;
	END;

 ISMAP:
	Create ' ismap';

 USEMAP: BEGIN
	NoTranslation;
	Create ' usemap="';
	Create USEMAP;
	Create QuoteF;
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
	Create QuoteF;
	END;

 Border: BEGIN
	Create ' BORDER=';
	Create Border;
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
	Create QuoteF;
	END;

 cellpadding: BEGIN
	Create ' cellpadding="';
	Create cellpadding;
	Create QuoteF;
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
	Create ' colspan=';
	Create colspan;
	END;

 rowspan: BEGIN
	Create ' rowspan=';
	Create rowspan;
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
	Create ' width=';
	Create Cell_width;
	END;

 Cell_height: BEGIN
	Create ' height=';
	Create Cell_height;
	END;

 Word_wrap = No_wrap:
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
