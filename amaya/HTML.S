		{ I. Vatton 	 November 1994  }
STRUCTURE HTML;

DEFPRES HTMLP;
 
ATTR
   ID = TEXT;
   Class = text;
   PseudoClass = text;
   Style\240 = TEXT;
   Invalid_attribute = TEXT;
   Ghost_restruct = TEXT;

CONST
   C_Empty = ' ';
   C_Foot = ' ';
   C_Head = ' ';
   C_BR   = '\212';

STRUCT
  HTML (ATTR SectionNumbering = Yes_; ShowAreas = Yes_) =
	BEGIN
	HEAD;
	BODY;
#ifdef R_HTML
	END + (Invalid_element, Comment\240, Entity);
#else
	END + (Invalid_element, Comment\240);
#endif

  BODY (ATTR background_ = Text; BackgroundColor = Text; TextColor = Text;
	LinkColor = Text; VisitedLinkColor = Text; ActiveLinkColor = Text)
	= LIST OF (Element);

  Invalid_element (ATTR Error_type = UnknownTag, BadPosition) = TEXT;
  Comment\240 = LIST OF (Comment_line = TEXT);

  HEAD = AGGREGATE
	  Document_URL = TEXT;
	  TITLE = TEXT;
	? ISINDEX (ATTR Prompt = TEXT) = CONSTANT C_Empty;
	? BASE;
	? Styles (ATTR Notation = TEXT) = LIST OF (StyleRule);
	? Scripts = LIST OF (SCRIPT = TEXT);
	? Metas = LIST OF (META);
	? Links = LIST OF (LINK);
	END;
  META (ATTR http_equiv = Text; !meta_name = Text; !meta_content = Text)
	 = CONSTANT C_Empty;
  LINK (ATTR !HREF_ = Text; REL = Text; REV = Text; Title = Text)
	 = CONSTANT C_Empty;

  StyleRule (ATTR Selector = TEXT) = TEXT;

  BASE (ATTR !HREF_) = CONSTANT C_Empty;

#ifdef R_HTML
  Entity (ATTR entity_name = TEXT; HREF_) = CONSTANT C_Empty;
#endif

  Element = CASE OF
	Paragraph;
	Pseudo_paragraph;
	Heading;
	List_;
	Horizontal_Rule;
	Block_Quote = LIST OF (Element);
	Preformatted;
	Address = LIST OF (AddressContent);
	Division;
	Center;
	Applet;
#ifdef COUGAR
	Object;
#endif
	Form;
	Table;
	PICTURE;
	MAP;
	ISINDEX;
	END;

  Horizontal_Rule (ATTR Align = left_, center_, right_;
			NoShade = NoShade_; Size_ = Integer;
			Width__ = Text; IntWidthPercent = Integer;
			IntWidthPxl = Integer) =
	CONSTANT C_Empty;

  AddressContent = CASE OF
	Basic_Elem;
	Paragraph;
	Pseudo_paragraph;
	END;

  Basic_Set = LIST OF (Basic_Elem);

  Heading = CASE OF
	H1 (ATTR Align) = Basic_Set;
	H2 (ATTR Align) = Basic_Set;
	H3 (ATTR Align) = Basic_Set;
	H4 (ATTR Align) = Basic_Set;
	H5 (ATTR Align) = Basic_Set;
	H6 (ATTR Align) = Basic_Set;
	END;

  Paragraph (ATTR Align) = Basic_Set;
  Pseudo_paragraph = Basic_Set;
  Preformatted (ATTR WidthElement = integer) = Basic_Set
	     - (PICTURE, Big_text, Small_text, Subscript, Superscript, Font_);

  Anchor (ATTR HREF_; NAME = Text; REL; REV; Title;
	       InternalLink = Reference(Anchor))
	= Basic_Set;

  Basic_Elem = CASE OF
        TEXT;
	PICTURE (ATTR Alignment = Top_, Middle_, Bottom_, Left_, Right_;
		 Height_ = Integer; Width_ = Integer; hspace = Integer;
		 vspace = Integer; Img_border = Integer;
		 ISMAP = Yes_; USEMAP = Text; SRC = Text; ALT = Text);
	Anchor;
	Font;
	Phrase;
	Font_;
	BaseFont;
	Applet;
#ifdef COUGAR
	Object;
#endif
	SCRIPT;
	MAP;
	BR (ATTR Clear = Left_, Right_, All_, None) = CONSTANT C_BR;
	Span = Basic_Set;
	END;

  BaseFont (ATTR BaseFontSize = Integer) = CONSTANT C_Empty;

  Font = CASE OF
	Italic_text = Basic_Set;
	Bold_text = Basic_Set;
	Teletype_text = Basic_Set;
	Underlined_text = Basic_Set;
	Struck_text = Basic_Set;
	Big_text = Basic_Set;
	Small_text = Basic_Set;
	Subscript = Basic_Set;
	Superscript = Basic_Set;
	END;

  Phrase = CASE OF
	Emphasis = Basic_Set;
	Strong = Basic_Set;
	Def = Basic_Set;
	Code = Basic_Set;
	Sample = Basic_Set;
	Keyboard = Basic_Set;
	Variable = Basic_Set;
	Cite = Basic_Set;
	END;

  Font_ (ATTR Font_size = Text; IntSizeIncr = Integer; IntSizeDecr = Integer;
	      IntSizeRel = Integer; color = Text) = Basic_Set;

  Applet (ATTR codebase = TEXT; !code = TEXT; applet_name = TEXT;
	       alt = TEXT; Alignment; Height_; Width_; hspace; vspace) =
	LIST OF (Applet_Content = CASE OF
				  Parameter;
				  Basic_Set;
				  END);
  Parameter (ATTR Param_name = TEXT; Param_value = TEXT) =  CONSTANT C_Empty;

#ifdef COUGAR
  Object (ATTR classid = TEXT; codebase; data = TEXT; Object_type = TEXT;
	  codetype = TEXT; Alignment; !Height_; !Width_; hspace; vspace) =
	BEGIN
	PICTURE;
	Object_Content = LIST OF (CASE OF Element; Parameter; END);
	END with Height_ ?= 400, Width_ ?= 400;
#endif

  Block = CASE OF
	Paragraph;
	Pseudo_paragraph;
	List_;
	Block_Quote;
	Preformatted;
	Division;
	Center = LIST OF (Element);
	Form;
	END;

  Division (ATTR Align) = LIST OF (Element);

  List_ = CASE OF
	Unnumbered_List (ATTR BulletStyle = disc, circle, square;
			      COMPACT = Yes_) =
			LIST OF (List_Item);
	Numbered_List  (ATTR NumberStyle = Arabic_, LowerAlpha, UpperAlpha,
			    LowerRoman, UpperRoman; Start = Integer; COMPACT) =
			LIST OF (List_Item) with Start ?= 1;
	Directory (ATTR COMPACT) =
			LIST OF (List_Item);
	Menu (ATTR COMPACT) =
			LIST OF (List_Item);
	Definition_List = LIST OF (Definition_Item);
	END;

  List_Item (ATTR ItemStyle = disc, circle, square, Arabic_, LowerAlpha,
		  UpperAlpha, LowerRoman, UpperRoman;
		  IntItemStyle = disc, circle, square, Arabic_, LowerAlpha,
		  UpperAlpha, LowerRoman, UpperRoman;
		  ItemValue = Integer) =
	LIST OF (Block);

  Definition_Item (ATTR COMPACT) = BEGIN
	Term_List = LIST OF (Term = Basic_Set);
	? Definition = LIST OF (Block);
	END;

  Form (ATTR !Script_URL = Text; METHOD = Get_, Post_; ENCTYPE = Text) =
	LIST OF (Form_Element) + (Input) with METHOD ?= Get_;

  Form_Element = CASE OF
	Paragraph;
	Pseudo_paragraph;
	Toggle_Menu;
	Radio_Menu;
	Option_Menu;
	Text_Input_Line;
	Command_Line;
	Text_Area (ATTR !NAME; Rows = Integer; Columns = Integer;
			Default_Value = Text) =
		Text_With_Frame with Rows ?= 4, Columns ?= 20;
	Heading;
	List_;
	Horizontal_Rule;
	Block_Quote;
	Preformatted;
	Division;
	Center;
	ISINDEX;
	END;

  Option_Menu (ATTR !NAME; Multiple = Yes_; MenuSize = Integer) =
	LIST OF (Option) - (Input);
  Option (ATTR Selected = Yes_; DefaultSelected = Yes_; !Value_ = Text) =
	TEXT;

  Toggle_Menu = LIST OF (Toggle_Item) - (Input);
  Toggle_Item = BEGIN
	Checkbox_Input (ATTR !NAME; 
			     Checked = Yes_, No_; DefaultChecked = Yes_, No_;
			     Value_) =
		CONSTANT C_Empty with Checked ?= No_;
	TEXT;
	END;

  Radio_Menu (ATTR !NAME) = LIST OF (Radio_Item) - (Input);
  Radio_Item = BEGIN
	Radio_Input (ATTR !NAME; Checked; DefaultChecked; !Value_) =
		CONSTANT C_Empty with Checked ?= No_, NAME ?= 'radio';
	TEXT;
	END;

  Text_Input_Line = BEGIN
	TEXT;
	Text_Input (ATTR !NAME; Area_Size = Integer; MaxLength = Integer; Value_) =
		Text_With_Frame with Area_Size ?= 20;
	END - (Input);

  Command_Line = BEGIN
	Reset_Input (ATTR NAME; Value_) = BEGIN
		Frame;
		END with NAME ?= 'Reset', Value_ ?= 'Reset';
	Space = CONSTANT C_Empty;
	Submit_Input (ATTR NAME; Value_) = BEGIN
		Frame;
		END with Value_ ?= 'Submit';
	END - (Input);

  Input = CASE OF
	Checkbox_Input;
	Radio_Input;
	Text_Input;
	Hidden_Input (ATTR !NAME; Value_) = CONSTANT C_Empty;
	Password_Input (ATTR !NAME; Area_Size; MaxLength) = Text_With_Frame;
	File_Input (ATTR !NAME) = CONSTANT C_Empty;
	Reset_Input;
	Submit_Input;
	END;

  Text_With_Frame = BEGIN
	Inserted_Text = TEXT;
	Frame = CONSTANT C_Empty;
	END;

  Table (ATTR Border = Integer;
	      Table_align = Align_left, Center_, Align_right;
	      Width__; IntWidthPercent; IntWidthPxl;
	      cellspacing = Integer; cellpadding = Integer) =
	BEGIN
	CAPTION (ATTR Position = Position_top, Position_bottom) = Basic_Set;
	Table_head = LIST OF (Column_head);
	? thead = LIST OF (Table_row);
	Table_body = LIST OF (tbody = LIST OF (Table_row));
	? tfoot = LIST OF (Table_row);
	Table_foot = CONSTANT C_Foot;
	END;
  Column_head (ATTR Col_width_percent = INTEGER; Col_width_pxl = INTEGER;
		    Col_width_delta = INTEGER; IntMaxVol = Integer) =
	CONSTANT C_Head with Col_width_percent ?= 100;
  Table_row (ATTR Row_align = Row_left, Row_center, Row_right;
		  Row_valign = Row_top, Row_middle, Row_bottom, Row_baseline) =
	LIST OF (Table_cell);
  Table_cell = CASE OF
	Data_cell(ATTR Ref_column = REFERENCE(Column_head);
                 ColExt = REFERENCE(Column_head);
                 RowExt = REFERENCE(Table_row);
                 colspan = INTEGER;
                 rowspan = INTEGER;
                 Cell_align = Cell_left, Cell_center, Cell_right;
                 Cell_valign = Cell_top, Cell_middle, Cell_bottom;
		 Cell_width = Text;
		 Cell_height = Integer;
                 No_wrap = no_wrap) = LIST OF (Element);
	Heading_cell(ATTR Ref_column; ColExt; RowExt; colspan; rowspan;
		 Cell_align; Cell_valign; Cell_width; Cell_height; No_wrap) =
		 LIST OF (Element);
	END;
  Table_cell_ghost = CONSTANT C_Empty;

  MAP (ATTR NAME; Ref_IMG = REFERENCE(Any)) = LIST OF (AREA);
  AREA (ATTR shape = rectangle, circle, polygon, default_;
	coords = Text; HREF_; nohref = Yes_; ALT;
	x_coord = Integer; y_coord = Integer; width\240 = Integer;
        height\240 = Integer; AreaRef_IMG = REFERENCE(Any)) = GRAPHICS
	with x_coord ?= 0, y_coord ?= 0, width\240 ?= 25, height\240 ?= 10;

EXCEPT
	HTML:           NoMove, NoResize;
	BODY:		NoCut;
	HEAD:		NoCut;
	TITLE:		NoCut;
	Document_URL:	Hidden, NoSpellCheck;
	C_Empty:	Hidden, NoSelect;
	C_BR:		NoSelect;
	Scripts:	Hidden, NoSelect;
	Metas:		Hidden, NoSelect;
	Links:		Hidden, NoSelect;
	Frame:		Hidden;
	Inserted_Text:	Hidden, NoSelect;
	Basic_Set:	Hidden, NoSelect;
	Term_List:	Hidden, NoSelect;
	Definition_Item: Hidden;
	Comment_line:	Hidden;
	GRAPHICS:	NoMove, NoResize;
	PICTURE:	MoveResize;
	Invalid_element: NoCreate;
	Pseudo_paragraph:	ParagraphBreak, Hidden;
	Paragraph:	ParagraphBreak;
	H1:		ParagraphBreak;
	H2:		ParagraphBreak;
	H3:		ParagraphBreak;
	H4:		ParagraphBreak;
	H5:		ParagraphBreak;
	H6:		ParagraphBreak;
	Address:	ParagraphBreak;
	Term:		ParagraphBreak;
	MAP:		IsDraw;
	AREA:		MoveResize;
	Preformatted:	ReturnCreateNL;

#ifndef COUGAR
	Langue:         Invisible;
#endif
	InternalLink:	Invisible;
	WidthElement:	NewWidth;
	IntWidthPercent:Invisible;
	IntWidthPxl:	Invisible;
	SRC:		Invisible;
	IntItemStyle:	Invisible;
	IntSizeIncr:	Invisible;
	IntSizeDecr:	Invisible;
	IntSizeRel:	Invisible;
	IntMaxVol:	Invisible;
	x_coord:	Invisible, NewHPos;
	y_coord:	Invisible, NewVPos;
	width\240:	Invisible, NewWidth;
	height\240:	Invisible, NewHeight;
	Ref_IMG:	Invisible;
	AreaRef_IMG:	Invisible;
	Invalid_attribute: Invisible;
	PseudoClass:	Invisible;
	Default_Value:	Invisible;
	DefaultSelected: Invisible;
	DefaultChecked:	Invisible;
	SectionNumbering:Invisible;
	ShowAreas:	Invisible;

	Table:		PageBreak;
	Column_head:	NoCut, Hidden, NoSelect;
	Table_head:	NoCut, Hidden, NoSelect,
			PageBreakRepetition;
	C_Head:		Hidden, NoSelect;
	Table_foot:	NoCut, Hidden, PageBreakRepBefore;
	C_Foot:		Hidden, NoSelect;
	Table_body:	PageBreakAllowed, Hidden;
	Table_row:	PageBreakPlace;
	Col_width_percent:	Invisible;
	Col_width_pxl:	Invisible;
	Col_width_delta:	Invisible;
	ColExt:		Invisible;
	RowExt:		Invisible;
	Ref_column:	Invisible;
	Table_cell_ghost: Hidden, NoCreate, NoCut, NoSelect;
#ifdef COUGAR
	Object_Content: Hidden, NoCreate, NoCut, NoSelect;
#endif
#ifdef R_HTML
	Entity: Hidden, NoCreate, NoCut;
#endif
	Ghost_restruct:	Invisible;
END
