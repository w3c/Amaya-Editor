		{ I. Vatton 	 November 1994  }
STRUCTURE HTML;

DEFPRES HTMLP;

ATTR
			{ coreattrs }
   ID = TEXT;			{ id }
   Class = TEXT;		{ class }
   PseudoClass = TEXT;
   Style\240 = TEXT;		{ style }
   Title = TEXT;		{ title }
			{ i18n }
   dir = ltr, rtl;		{ dir }
			{ events }
   onclick = TEXT;		{ onclick }
   ondblclick = TEXT;		{ ondblclick }
   onmousedown = TEXT;		{ onmousedown }
   onmouseup = TEXT;		{ onmouseup }
   onmouseover = TEXT;		{ onmouseover }
   onmousemove = TEXT;		{ onmousemove }
   onmouseout = TEXT;		{ onmouseout }
   onkeypress = TEXT;		{ onkeypress }
   onkeydown = TEXT;		{ onkeydown }
   onkeyup = TEXT;		{ onkeyup }

   Invalid_attribute = TEXT;
   Ghost_restruct = TEXT;

CONST

   C_Empty = ' ';
   C_Foot = ' ';
   C_Head = ' ';
   C_BR   = '\212';

STRUCT

  HTML (ATTR SectionNumbering = Yes_; ShowAreas = Yes_) =	{ HTML }
	BEGIN
	HEAD;
	BODY;
	END + (Invalid_element, Comment\240);

#ifdef FRAMES
  FRAMESET (ATTR RowHeight = TEXT; ColWidth = TEXT) =		{ FRAMESET }
	       { rows		   cols }
	AGGREGATE
	Frames = LIST [1, *] OF (CASE OF FRAMESET; FRAME; END);
      ?	NOFRAMES = BODY;					{ NOFRAMES }
	END;

  FRAME (ATTR FrameName = TEXT; FrameSrc = TEXT) =		{ FRAME }
	    { name		src }
	CONSTANT C_Empty;
#endif

  BODY								{ BODY }
	(ATTR background_ = Text; BackgroundColor = Text; TextColor = Text;
	   { background          bgcolor                 text }
	     LinkColor = Text; VisitedLinkColor = Text; ActiveLinkColor = Text)
           { link              vlink                    alink }
	= LIST OF (Element) + (INS, DEL);

  Invalid_element (ATTR Error_type = UnknownTag, BadPosition) = TEXT;
  Comment\240 = LIST OF (Comment_line = TEXT);

  HEAD = AGGREGATE						{ HEAD }
	  Document_URL = TEXT;
	  TITLE = TEXT;						{ TITLE }
	? ISINDEX (ATTR Prompt = TEXT) = CONSTANT C_Empty;	{ ISINDEX }
		      { prompt }
	? BASE;							{ BASE }
	? Styles						{ STYLE }
		(ATTR Notation = TEXT) = TEXT;
		    { type }
	? Scripts = LIST OF (SCRIPT);
	? Metas = LIST OF (META);
	? Links = LIST OF (LINK);
	END;

  SCRIPT = TEXT;						{ SCRIPT }

  META								{ META }
	(ATTR http_equiv = Text; !meta_name = Text; !meta_content = Text)
	    { http-equic          name               content }
	 = CONSTANT C_Empty;

  LINK (ATTR HREF_ = Text;					{ LINK }
	   { href }
	     Link_type = TEXT;
	   { type }
	     REL = Text; REV = Text)
	   { rel         rev }
	 = CONSTANT C_Empty;

  BASE (ATTR !HREF_) = CONSTANT C_Empty;			{ BASE }
	    { href }

  Element = CASE OF						{ block }
	Paragraph;						{ P }
	Pseudo_paragraph;
	Heading;						{ heading }
	List_;							{ list + DL }
	Preformatted;						{ PRE preformatted }
	Division;						{ DIV }
	Center;							{ CENTER }
	NOSCRIPT = LIST OF (Element);				{ NOSCRIPT }
	Block_Quote (ATTR cite = TEXT;				{ BLOCKQUOTE }
			{ cite }
	        InternalLink = Reference(Any); ExternalLink = Yes_) =
		LIST OF (Element);
	Form;							{ FORM }
	ISINDEX;						{ ISINDEX }
	Horizontal_Rule (ATTR Align = left_, center_, right_;	{ HR }
			    { align   left   center   right }
			NoShade = NoShade_; Size_ = Integer;
		      { noshade             size }
			Width__ = Text;
		      { width }
			IntWidthPercent = Integer;
			IntWidthPxl = Integer) =
		CONSTANT C_Empty;
	Table;							{ TABLE }
	Address = LIST OF (AddressContent);			{ ADDRESS }
	PICTURE							{ IMG }
		(ATTR Alignment = Top_, Middle_, Bottom_, Left_, Right_;
		{     align       top   middle   bottom   left   right }
		  Height_ = Integer; Width__; IntWidthPercent; IntWidthPxl;
		{ height             width }
		  hspace = Integer; vspace = Integer; Img_border = Integer;
		{ hspace            vspace            border }
		  ISMAP = Yes_; USEMAP = Text; SRC = Text; ALT = Text);
		{ ismap        usemap         src         alt }
	Object;							{ OBJECT }
	Applet;							{ APPLET }
#ifdef MATHML
	MathDisp (ATTR mode = display, inline_math) = MathML;
#endif
#ifdef GRAPHML
        XMLGraphics (ATTR Height_; Width__;
			IntWidthPercent;
			IntWidthPxl)
	 = GraphML
	   with Height_ ?= 200, Width__ ?= '100%', IntWidthPercent ?= 100;
#endif
	MAP;							{ MAP }
	END;

#ifdef GRAPHML
	HTMLfragment = LIST OF (Element);
#endif

  AddressContent = CASE OF
	Basic_Elem;
	Paragraph;		{ P }
	Pseudo_paragraph;
	END;

  Basic_Set = LIST OF (Basic_Elem);

  Paragraph							{ P }
	(ATTR TextAlign = left_, center_, right_, justify_) = Basic_Set;
	    { align	  left   center   right   justify }
  Pseudo_paragraph = Basic_Set;

  Heading = CASE OF						{ heading }
	H1 (ATTR TextAlign) = Basic_Set;			{ H1 }
	       { align }
	H2 (ATTR TextAlign) = Basic_Set;			{ H2 }
	H3 (ATTR TextAlign) = Basic_Set;			{ H3 }
	H4 (ATTR TextAlign) = Basic_Set;			{ H4 }
	H5 (ATTR TextAlign) = Basic_Set;			{ H5 }
	H6 (ATTR TextAlign) = Basic_Set;			{ H6 }
	END;

  Preformatted (ATTR Width__; IntWidthPercent; IntWidthPxl)	{ PRE }
		   { width }
		= Basic_Set
	     - (PICTURE, Object, Applet,
		Big_text, Small_text, Subscript, Superscript,
		Font_, BaseFont);

  { A }
  Anchor (ATTR HREF_; NAME = Text; REL; REV;
	     { href   name         rel  rev }
	       InternalLink; ExternalLink)
	= Basic_Set;

  Basic_Elem = CASE OF					{ special }
        TEXT;
	Font;
	Phrase;
	Anchor;							{ A }
	PICTURE;						{ IMG }
	Applet;							{ APPLET }
	Object;							{ OBJECT }
	Font_;							{ FONT }
	BaseFont;						{ BASEFONT }
	BR (ATTR Clear = Left_, Right_, All_, None) =		{ BR }
	       { clear   left   right   all   none }
		 CONSTANT C_BR;
	SCRIPT;							{ SCRIPT }
	MAP;							{ MAP }
	Quotation (ATTR cite; InternalLink; ExternalLink) =	{ Q }
		      { cite }
		 Basic_Set;

	Subscript = Basic_Set;					{ SUB }
	Superscript = Basic_Set;				{ SUP }
	Span = Basic_Set;					{ SPAN }
	BDO = Basic_Set;					{ BDO }
	IFRAME;							{ IFRAME }
#ifdef MATHML
	Math (ATTR mode) = MathML;
#endif
	END;

  BaseFont							{ BASEFONT }
	(ATTR BaseFontSize = Integer; BaseFontColor = Text;
	    { size                    color }
	      BaseFontFace = Text) =
            { face }
		CONSTANT C_Empty;

  Font = CASE OF		{ fontstyle }
	Teletype_text = Basic_Set;	{ TT }
	Italic_text = Basic_Set;	{ I }
	Bold_text = Basic_Set;		{ B }
	Underlined_text = Basic_Set;	{ U }
	Struck_text = Basic_Set;	{ S or STRIKE }
	Big_text = Basic_Set;		{ BIG }
	Small_text = Basic_Set;		{ SMALL }
	END;

  Phrase = CASE OF		{ phrase }
	Emphasis = Basic_Set;		{ EM }
	Strong = Basic_Set;		{ STRONG }
	Def = Basic_Set;		{ DFN }
	Code = Basic_Set;		{ CODE }
	Sample = Basic_Set;		{ SAMP }
	Keyboard = Basic_Set;		{ KBD }
	Variable = Basic_Set;		{ VAR }
	Cite = Basic_Set;		{ CITE }
	ABBR = Basic_Set;		{ ABBR }
        ACRONYM = Basic_Set;		{ ACRONYM }
	END;

  Font_ (ATTR Font_size = Text; color = Text; face = Text;	{ FONT }
	    { size              color	      face }
	      IntSizeIncr = Integer; IntSizeDecr = Integer;
	      IntSizeRel = Integer) = Basic_Set;

  Applet (ATTR codebase = TEXT; !code = TEXT;			{ APPLET }
	     { codebase          code }
	       alt = TEXT; applet_name = TEXT;
	     { alt         name }
	       Width__; Height_;
	     { width    height }
	       IntWidthPercent; IntWidthPxl;
	       Alignment; hspace; vspace) =
	     { align      hspace  vspace }
	LIST OF (Applet_Content = CASE OF
				  Parameter;			{ PARAM }
				  Basic_Set;
				  END);
  Parameter (ATTR Param_name = TEXT; Param_value = TEXT) =	{ PARAM }
		{ name               value }
	CONSTANT C_Empty;

  Object							{ OBJECT }
	(ATTR classid = TEXT; codebase; data = TEXT; Object_type = TEXT;
	    { classid         codebase  data         type }
	  codetype = TEXT; Alignment; !Height_; !Width__; IntWidthPercent;
        { codetype         align       height    width }
	  IntWidthPxl; hspace; vspace;
        {              hspace  vspace }
	  Img_border; USEMAP; ALT) =
	{ border      usemap  alt }
	BEGIN
	PICTURE;
	Object_Content = LIST OF (CASE OF Element; Parameter; END);
						 { PARAM }
	END with Height_ ?= 400, Width__ ?= '400', IntWidthPxl ?= 400;

  INS (ATTR cite; datetime = TEXT) = Basic_Set;			{ INS }
  DEL (ATTR cite; datetime) = Basic_Set;			{ DEL }

  Block = CASE OF						{ block }
	Paragraph;						{ P }
	Pseudo_paragraph;
	Heading;						{ heading }
	List_;							{ list + DL }
	Block_Quote;						{ BLOCKQUOTE }
	Preformatted;						{ PRE }
	Division;						{ DIV }
	Center = LIST OF (Element);				{ CENTER }
	Form;
	END;

  Division (ATTR TextAlign) = LIST OF (Element);		{ DIV }

  List_ = CASE OF						{ list }
	Unnumbered_List			 			{ UL }
		(ATTR BulletStyle = disc, circle, square;
		    { type          disc  circle  square }
		      COMPACT = Yes_) =
		    { compact }
			LIST OF (List_Item);
	Numbered_List						{ OL }
		(ATTR NumberStyle = Arabic_, LowerAlpha, UpperAlpha,
		    { type          1        a           A }
				    LowerRoman, UpperRoman;
				  { i           I }
		      Start = Integer; COMPACT) =
		    { start            compact }
			LIST OF (List_Item) with Start ?= 1;
	Directory (ATTR COMPACT) =				{ DIR }
			LIST OF (List_Item);
	Menu (ATTR COMPACT) =					{ MENU }
			LIST OF (List_Item);
	Definition_List (ATTR COMPACT) =			{ DL }
			LIST OF (Definition_Item);
	END;

  List_Item (ATTR ItemStyle = disc, circle, square,		{ LI }
		{ type        disc  circle  square }
		  Arabic_, LowerAlpha, UpperAlpha, LowerRoman, UpperRoman;
		{ 1        a           A           i           I }
		  IntItemStyle = disc, circle, square,
		  Arabic_, LowerAlpha, UpperAlpha, LowerRoman, UpperRoman;
		  ItemValue = Integer) =
		{ value }
	LIST OF (Block);

  Definition_Item = BEGIN
	Term_List = LIST OF (Term = Basic_Set);			{ DT }
	? Definition = LIST OF (Block);				{ DD }
	END;

  Form (ATTR !Script_URL = Text; METHOD = Get_, Post_;		{ FORM }
	    { action             method }
	     ENCTYPE = Text) =
	   { enctype }
	LIST OF (Form_Element) + (Input) with METHOD ?= Get_;

  Form_Element = CASE OF
	Paragraph;						{ P }
	Pseudo_paragraph;
	Option_Menu;						{ SELECT }
	Text_Area 						{ TEXTAREA }
		(ATTR !NAME; Rows = Integer; Columns = Integer;
		     { name  rows            cols }
		       Default_Value = Text) =
		Text_With_Frame with Rows ?= 4, Columns ?= 20;
	Heading;						{ heading }
	List_;							{ list + DL }
	Horizontal_Rule;					{ HR }
	Block_Quote;						{ BLOCKQUOTE }
	Preformatted;						{ PRE }
	Division;						{ DIV }
	Table;							{ TABLE }
	Center;							{ CENTER }
	ISINDEX;						{ ISINDEX }
	END;

  Option_Menu (ATTR !NAME; Multiple = Yes_; MenuSize = Integer) =    { SELECT }
		   { name  multiple         size }
	LIST OF (Option) - (Input);
  Option							     { OPTION }
	(ATTR Selected = Yes_; DefaultSelected = Yes_; !Value_ = Text) =
	    { selected                                  value }
	TEXT;

  Input = CASE OF					{ formctrl }
	Checkbox_Input					{ INPUT / CHECKBOX }
		(ATTR !NAME; { name }
		      Checked = Yes_, No_; DefaultChecked = Yes_, No_;
		    { checked }
		      Value_ { value } ) =
		CONSTANT C_Empty with Checked ?= No_;
	Radio_Input					{ INPUT / RADIO }
		(ATTR !NAME; Checked; DefaultChecked; !Value_) =
		     { name  checked                   value }
		CONSTANT C_Empty with Checked ?= No_, NAME ?= 'radio';
	Text_Input					{ INPUT / TEXT }
		(ATTR !NAME; Area_Size = Integer;
		     { name  size }
		      MaxLength = Integer; Value_) =
		     { maxlength           value }
		Text_With_Frame with Area_Size ?= 20;
	Hidden_Input					{ INPUT / HIDDEN }
		(ATTR !NAME; Value_) = CONSTANT C_Empty;
	Password_Input					{ INPUT / PASSWORD }
		(ATTR !NAME; Area_Size; MaxLength) = Text_With_Frame;
	File_Input					{ INPUT / FILE }
		(ATTR !NAME) = CONSTANT C_Empty;
	Reset_Input (ATTR NAME; Value_) = BEGIN		{ INPUT / RESET }
		Frame;
		END with NAME ?= 'Reset', Value_ ?= 'Reset';
	Submit_Input (ATTR NAME; Value_) = BEGIN	{ INPUT / SUBMIT }
		Frame;
		END with Value_ ?= 'Submit';
	Option_Menu;					{ SELECT }
	Text_Area;					{ TEXTAREA }
{****
        LABEL
        BUTTON
****}
	END;

  Text_With_Frame = BEGIN
	Inserted_Text = TEXT;
	Frame = CONSTANT C_Empty;
	END;

  Table (ATTR Border = Integer;					{ TABLE }
	    { border }
	      Align; BackgroundColor;
	    { align  bgcolor }
	      Width__; IntWidthPxl; IntWidthPercent;
	    { width }
	      cellspacing = Integer; cellpadding = Integer) =
	    { cellspacing	     cellpadding }
	BEGIN
	CAPTION							{ CAPTION }
	      (ATTR Position = Position_top, Position_bottom) =
		  { align      top           bottom }
		Basic_Set;
	Table_head = LIST OF (Column_head);
	? thead = LIST OF (Table_row);				{ THEAD }
	Table_body = LIST OF (
		tbody = LIST OF (Table_row));			{ TBODY }
	? tfoot = LIST OF (Table_row);				{ TFOOT }
	Table_foot = CONSTANT C_Foot;
	END;
  Column_head (ATTR IntWidthPercent; IntWidthPxl) =
	CONSTANT C_Head;
  Table_row							{ TR }
	(ATTR Row_align = Row_left, Row_center, Row_right;
	    { align       left      center      right }
	      Row_valign = Row_top, Row_middle, Row_bottom, Row_baseline;
	    { valign       top      middle      bottom      baseline }
	      BackgroundColor) =
	    { bgcolor }
	LIST OF (Table_cell);
  Table_cell = CASE OF
	Data_cell(ATTR Ref_column = REFERENCE(Column_head);	{ TD }
                 ColExt = REFERENCE(Column_head);
                 RowExt = REFERENCE(Table_row);
		 colspan_ = INTEGER;
		{ colspan }
                 rowspan_ = INTEGER;
		{ rowspan }
                 Cell_align = Cell_left, Cell_center, Cell_right;
		{ align       left       center       right }
                 Cell_valign = Cell_top, Cell_middle, Cell_bottom;
		{ valign       top       middle       bottom }
		 BackgroundColor;
		{ bgcolor }
		 Width__; IntWidthPxl; IntWidthPercent;
		{ width }
		 Height_;
		{ height }
                 No_wrap = no_wrap) = LIST OF (Element);
		{ nowrap }
	Heading_cell						{ TH }
		(ATTR Ref_column; ColExt; RowExt;
		 colspan_; rowspan_;
		 Cell_align; Cell_valign; BackgroundColor;
		 Width__; IntWidthPxl; IntWidthPercent;
		 Height_; No_wrap) = LIST OF (Element);
	END;

  MAP (ATTR NAME; Ref_IMG = REFERENCE(Any)) = LIST OF (AREA);	{ MAP }
	  { name }
  AREA (ATTR shape = rectangle, circle, polygon, default_;	{ AREA }
	   { shape   rect       cirecle poly     default }
	coords = Text; HREF_; nohref = Yes_; ALT;
	{ coords       href   nohref         alt }
	x_coord = Integer; y_coord = Integer; IntWidthPxl;
        height\240 = Integer; AreaRef_IMG = REFERENCE(Any)) = GRAPHICS
	with x_coord ?= 0, y_coord ?= 0, IntWidthPxl ?= 25, height\240 ?= 10;

  FRAMESET =							{ FRAMESET }
	AGGREGATE
	LIST OF (FrameElem);
	NOFRAMES = LIST OF (Element);				{ NOFRAMES }
	END;

  FRAME = CONSTANT C_Empty;					{ FRAME }

  FrameElem = CASE OF
	FRAMESET;
	FRAME;
	END;

  IFRAME = LIST OF (Element);					{ IFRAME }

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
	TEXT:		NoMove, NoResize;
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
#ifdef GRAPHML
	HTMLfragment:	NoMove, NoResize, Hidden, NoSelect;
	XMLGraphics:    MoveResize, NoMove;
#endif

	ExternalLink:	Invisible;
	InternalLink:	Invisible;
	IntWidthPercent:NewPercentWidth, Invisible;
	IntWidthPxl:	NewWidth, Invisible;
	SRC:		Invisible;
	IntItemStyle:	Invisible;
	IntSizeIncr:	Invisible;
	IntSizeDecr:	Invisible;
	IntSizeRel:	Invisible;
	x_coord:	Invisible, NewHPos;
	y_coord:	Invisible, NewVPos;
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
	shape:		Invisible;
	Data_cell:	IsCell, NoMove, NoResize;
	Heading_cell:	IsCell, NoMove, NoResize;
	Table:		IsTable, PageBreak;
	Column_head:	IsColHead, NoCut, Hidden, NoSelect;
	Table_head:	NoCut, Hidden, NoSelect;
	C_Head:		Hidden, NoSelect;
	Table_foot:	NoCut, Hidden, PageBreakRepBefore;
	C_Foot:		Hidden, NoSelect;
	Table_body:	PageBreakAllowed, Hidden;
	Table_row:	IsRow, PageBreakPlace;
	ColExt:		Invisible;
	RowExt:		Invisible;
	Ref_column:	ColRef, Invisible;
	colspan_:	ColSpan;
	rowspan_:	RowSpan;
	Object_Content: Hidden, NoCreate, NoCut, NoSelect;
	Ghost_restruct:	Invisible;
END
