		{ I. Vatton 	 November 1994  }

STRUCTURE HTML;

DEFPRES HTMLP;

ATTR
			{ coreattrs }
   ID = Text;			{ id }
   Class = Text;		{ class }
   PseudoClass = Text;
   Style\240 = Text;		{ style }
   Title = Text;		{ title }
			{ i18n }
   dir = ltr, rtl;		{ dir }
			{ events }
   onclick = Text;		{ onclick }
   ondblclick = Text;		{ ondblclick }
   onmousedown = Text;		{ onmousedown }
   onmouseup = Text;		{ onmouseup }
   onmouseover = Text;		{ onmouseover }
   onmousemove = Text;		{ onmousemove }
   onmouseout = Text;		{ onmouseout }
   onkeypress = Text;		{ onkeypress }
   onkeydown = Text;		{ onkeydown }
   onkeyup = Text;		{ onkeyup }

   Invalid_attribute = Text;
   Ghost_restruct = Text;
   Highlight = Yes_;
   IntEntity = Yes_; 

CONST

   C_Empty = ' ';
   C_Foot = ' ';
   C_Head = ' ';
   C_BR   = '\12';

STRUCT

  HTML								{ HTML }
     (ATTR SectionNumbering = Yes_; ShowAreas = Yes_;
	   PrintURL = Yes_; HtmlDTD = Transitional, Frameset;
	   Namespaces = Text; RealLang = Yes_;) =
	BEGIN
	HEAD;
      ? BODY;	{ only one of BODY, FRAMESET is allowed }
      ? FRAMESET;
	END + (Invalid_element, Comment\240);

  BODY								{ BODY }
     (ATTR onload = Text; onunload = Text;
	 { onload         onunload }
	   background_ = Text; BackgroundColor = Text;
	 { background          bgcolor }
	   TextColor = Text; LinkColor = Text;
         { text              link }
	   VisitedLinkColor = Text; ActiveLinkColor = Text) =
         { vlink                    alink }
	LIST OF (Element) + (INS, DEL);

  Invalid_element (ATTR Error_type = UnknownTag, BadPosition) = TEXT;
  Comment\240 = LIST OF (Comment_line = TEXT);

  Head_misc =
	CASE OF
	STYLE_;
	SCRIPT;
	META;
	LINK;
	END;
  HEAD								{ HEAD }
     (ATTR profile = Text) =
	AGGREGATE
	Document_URL = TEXT - (Head_misc);
	TITLE = TEXT - (Head_misc);				{ TITLE }
      ? ISINDEX (ATTR Prompt = Text) = CONSTANT C_Empty;	{ ISINDEX }
		    { prompt }
      ? BASE;							{ BASE }
	END + (Object, Head_misc);

  STYLE_							{ STYLE }
     (ATTR Notation = Text; media = Text) =
	 { type             media }
	 TEXT;

  SCRIPT							{ SCRIPT }
     (ATTR charset = Text; !content_type = Text;
	 { charset          type }
	   script_language = Text; script_src = Text;
	 { language                src }
	   defer = Yes_; event = Text; for_ = Text) =
	 { defer         event         for }
	TEXT;

  META								{ META }
     (ATTR http_equiv = Text;
	 { http-equiv }
	   meta_name = Text; meta_content = Text;
         { name              content }
	   scheme = Text) =
	 { scheme }
	CONSTANT C_Empty;

  LINK								{ LINK }
     (ATTR charset; HREF_ = Text; hreflang = Text;
	 { charset  href          hreflang }
	   Link_type = Text; REL = Text; REV = Text;
	 { type              rel         rev }
	   media; target_ = Text) =
	 { media  target }
	CONSTANT C_Empty;

  BASE								{ BASE }
     (ATTR !HREF_; target_) =
	  { href   target }
	CONSTANT C_Empty;

  Element = CASE OF						{ block }
	Paragraph;						{ P }
	Pseudo_paragraph;
	Heading;						{ heading }
	List_;							{ list + DL }
	Preformatted;						{ PRE preformatted }
	Division;						{ DIV }
	Center;							{ CENTER }
	NOSCRIPT = LIST OF (Element);				{ NOSCRIPT }
	Block_Quote;						{ BLOCKQUOTE }
	Form;							{ FORM }
	ISINDEX;						{ ISINDEX }
	Horizontal_Rule;					{ HR }
	Table;							{ TABLE }
	Address = LIST OF (AddressContent);			{ ADDRESS }
	PICTURE							{ IMG }
	    (ATTR SRC = Text; ALT = Text; longdesc = Text;
		{ src         alt         longdesc }
		  Height_ = Integer; Width__ = Text;
		{ height             width }
		  IntWidthPercent = Integer; IntWidthPxl = Integer;
		  USEMAP = Text; ISMAP = Yes_;
		{ usemap         ismap }
		 IsInput = Yes_;    NAME = Text;
		{ input type=IMAGE  name }
		  Alignment = Top_, Middle_, Bottom_, Left_, Right_;
		{ align       top   middle   bottom   left   right }
		  Img_border = Integer;
		{ border }
		  hspace = Integer; vspace = Integer);
		{ hspace            vspace }
	Object;							{ OBJECT }
	Applet;							{ APPLET }
	Math							{ MATH }
	    (ATTR mode = display, inline_math) =
		MathML;
        XMLGraphics (ATTR Height_; Width__;
			IntWidthPercent;
			IntWidthPxl)
	 = GraphML with Height_ ?= 200, Width__ ?= '100%',
			IntWidthPercent ?= 100;
	MAP;							{ MAP }
	END;

	HTMLfragment = LIST OF (Element);

  Block_Quote							{ BLOCKQUOTE }
     (ATTR cite = Text;
	 { cite }
	   InternalLink = Reference(Any); ExternalLink = Yes_) =
	LIST OF (Element);

  Horizontal_Rule						{ HR }
     (ATTR Align = left_, center_, right_;
	 { align   left   center   right }
	   NoShade = NoShade_; Size_ = Integer; Width__;
	 { noshade             size             width }
	   IntWidthPercent; IntWidthPxl) =
	CONSTANT C_Empty;

  Basic_Set = LIST OF (Basic_Elem);

  AddressContent = CASE OF
	Basic_Elem;
	Paragraph;		{ P }
	Pseudo_paragraph;
	END;

  Paragraph							{ P }
     (ATTR TextAlign = left_, center_, right_, justify_) =
	 { align       left   center   right   justify }
	Basic_Set;

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

  Preformatted							{ PRE }
     (ATTR Width__; IntWidthPercent; IntWidthPxl) =
	 { width }
	Basic_Set
	     - (PICTURE, Object, Applet,
		Big_text, Small_text, Subscript, Superscript,
		Font_, BaseFont);

  Anchor							{ A }
     (ATTR charset; Link_type; NAME;
	 { charset  type       name }
	   HREF_; hreflang; target_;
	 { href   hreflang  target}
	   REL; REV; accesskey = Text;
	 { rel  rev  accesskey }
	   shape = rectangle, circle, polygon, default_;
	 { shape   rect       cirecle poly     default }
	   coords = Text; tabindex = Integer;
	 { coords         tabindex }
	   onfocus = Text; onblur = Text;
	 { onfocus         onblur }
	   InternalLink; ExternalLink;
	   Annotation = Text;
         { Annotation}) =
	Basic_Set;

  Basic_Elem = CASE OF						{ special }
        TEXT;
	Font;							{ fontstyle }
	Phrase;							{ phrase }
	Anchor;							{ A }
	PICTURE;						{ IMG }
	Applet;							{ APPLET }
	Font_;							{ FONT }
	BaseFont;						{ BASEFONT }
	BR;							{ BR }
	SCRIPT;							{ SCRIPT }
	MAP;							{ MAP }
	Quotation;						{ Q }
	Subscript = Basic_Set;					{ SUB }
	Superscript = Basic_Set;				{ SUP }
	Span = Basic_Set;					{ SPAN }
	BDO = Basic_Set;					{ BDO }
	Object;							{ OBJECT }
	IFRAME;							{ IFRAME }
	Math;							{ MATH }
	END;

  BaseFont							{ BASEFONT }
     (ATTR BaseFontSize = Integer; BaseFontColor = Text;
	 { size                    color }
	   BaseFontFace = Text) =
         { face }
	CONSTANT C_Empty;

  BR								{ BR }
     (ATTR Clear = Left_, Right_, All_, None) =
	 { clear   left   right   all   none }
	CONSTANT C_BR;

  Quotation							{ Q }
     (ATTR cite; InternalLink; ExternalLink) =
	 { cite }
	Basic_Set;

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

  Font_								{ FONT }
     (ATTR Font_size = Text; color = Text; face = Text;
	 { size              color	      face }
	   IntSizeIncr = Integer; IntSizeDecr = Integer;
	   IntSizeRel = Integer) =
	Basic_Set;

  Applet							{ APPLET }
     (ATTR codebase = Text; archive = Text; code = Text;
	 { codebase         archive         code }
	   object = Text; alt = Text; applet_name = Text;
	 { object         alt         name }
	   Width__; Height_; IntWidthPercent; IntWidthPxl;
	 { width    height }
	   Alignment; hspace; vspace) =
	 { align      hspace  vspace }
	LIST OF (Applet_Content = CASE OF
				  Parameter;			{ PARAM }
				  Basic_Set;
				  END);
  Parameter							{ PARAM }
     (ATTR Param_name = Text; Param_value = Text;
	 { name               value }
	   valuetype = data_, ref, object_;
	 { valuetype   data   ref  object }
	   Param_type = Text) =
	 { type }
	CONSTANT C_Empty;

  Object							{ OBJECT }
     (ATTR declare = Yes_; classid = Text; codebase;
	 { declare         classid         codebase }
	   data = Text; Object_type = Text;
	 { data         type }
	   codetype = Text; archive; standby = Text;
         { codetype         archive  standby }
	   !Height_; !Width__; IntWidthPercent; IntWidthPxl; 
	 {  height    width }
	   USEMAP; NAME; tabindex; Alignment; Img_border;
	 { usemap  name  tabindex  align      border }
	   hspace; vspace) =
         { hspace  vspace }
	BEGIN
	PICTURE;
	Object_Content = LIST OF (CASE OF Element; Parameter; END);
						 { PARAM }
	END with Height_ ?= 400, Width__ ?= '400',
		 IntWidthPxl ?= 400;

  INS								{ INS }
     (ATTR cite; datetime = Text; InternalLink; ExternalLink) =
	 { cite  datetime }
	Basic_Set;

  DEL								{ DEL }
     (ATTR cite; datetime; InternalLink; ExternalLink) =
	 { cite  datetime }
	Basic_Set;

  Block = CASE OF						{ block }
	Paragraph;						{ P }
	Pseudo_paragraph;
	Heading;						{ heading }
	List_;							{ list + DL }
	Block_Quote;						{ BLOCKQUOTE }
	Preformatted;						{ PRE }
	Division;						{ DIV }
	Center;							{ CENTER }
	Form;
	Horizontal_Rule;					{ HR }
	Table;							{ TABLE }
	Address;						{ ADDRESS }
	END;

  Division							{ DIV }
     (ATTR TextAlign) =
	 { align }
	LIST OF (Element);

  Center =							{ CENTER }
	LIST OF (Element);

  List_ = CASE OF						{ list }
	Unnumbered_List;		 			{ UL }
	Numbered_List;						{ OL }
	Directory;						{ DIR }
	Menu;							{ MENU }
	Definition_List;					{ DL }
	END;

  Unnumbered_List			 			{ UL }
     (ATTR BulletStyle = disc, circle, square;
	 { type          disc  circle  square }
	   COMPACT = Yes_) =
	 { compact }
	LIST OF (List_Item);

  Numbered_List							{ OL }
     (ATTR NumberStyle = Arabic_, LowerAlpha, UpperAlpha,
	 { type          1        a           A }
	   LowerRoman, UpperRoman;
	 { i           I }
	   Start = Integer; COMPACT) =
	 { start            compact }
	LIST OF (List_Item) with Start ?= 1;

  Directory							{ DIR }
     (ATTR COMPACT) =
	 { compact }
	LIST OF (List_Item);

  Menu								{ MENU }
     (ATTR COMPACT) =
	 { compact }
	LIST OF (List_Item);

  Definition_List						{ DL }
     (ATTR COMPACT) =
	 { compact }
	LIST OF (Definition_Item);

  List_Item							{ LI }
     (ATTR ItemStyle = disc, circle, square, Arabic_,
	 { type        disc  circle  square  1 }
	   LowerAlpha, UpperAlpha, LowerRoman, UpperRoman;
	 { a           A           i           I }
	   IntItemStyle = disc, circle, square, Arabic_,
	   LowerAlpha, UpperAlpha, LowerRoman, UpperRoman;
	   ItemValue = Integer) =
	 { value }
	LIST OF (Block);

  Definition_Item = BEGIN
	Term_List = LIST OF (Term = Basic_Set);			{ DT }
      ? Definition = LIST OF (Block);				{ DD }
	END;

  Form								{ FORM }
     (ATTR !Script_URL = Text; METHOD = Get_, Post_;
	 {  action             method }
	   ENCTYPE = Text; onsubmit = Text; onreset = Text;
	 { enctype         onsubmit         onreset }
	   target_; accept_charset = Text) =
	 { target   accept-charset }
	LIST OF (Form_Element) + (Input) with METHOD ?= Get_;

  Form_Element = CASE OF
	Paragraph;						{ P }
	Pseudo_paragraph;
	FIELDSET;						{ FIELDSET }
	Option_Menu;						{ SELECT }
	Text_Area; 						{ TEXTAREA }
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

  Option_Menu   						{ SELECT }
     (ATTR !NAME; MenuSize = Integer; Multiple = Yes_;
	 {  name  size                multiple }
	    disabled = Yes_; tabindex; onfocus; onblur;
	  { disabled         tabindex  onfocus  onblur}
	    onchange = Text) =
	  { onchange }
	LIST OF (Option_item = CASE OF Option; OptGroup; END) - (Input);

  Option							{ OPTION }
     (ATTR Selected = Yes_; DefaultSelected = Yes_;
	 { selected }
	   disabled; label = Text; !Value_ = Text) =
         { disabled  label          value }
	TEXT;

  OptGroup							{ OPTGROUP }
     (ATTR disabled; !label) =
	 { disabled   label }
	LIST OF (Option);

  Text_Area							{ TEXTAREA }
     (ATTR !NAME; Rows = Integer; Columns = Integer;
	 {  name  rows            cols }
	   disabled; readonly = Yes_; tabindex; accesskey;
	 { disabled  readonly         tabindex  accesskey  }
	   onfocus; onblur; onselect = Text; onchange;
	 { onfocus  onblur  onselect         onchange }
	   Default_Value = Text) =
	Text_With_Frame - (Input) with Rows ?= 4, Columns ?= 20;

  FIELDSET =							{ FIELDSET }
	BEGIN
	LEGEND;
	Basic_Set;
	END;

  LEGEND							{ LEGEND }
     (ATTR accesskey; LAlign = Top_, Bottom_, Left_, Right_) =
	 { accesskey  align    top   bottom   left   right }
	Basic_Set;

  Input = CASE OF					{ formctrl }
	Text_Input					{ INPUT / TEXT }
	   (ATTR !NAME; Value_; disabled; readonly;
	        { name  value   disabled  readonly }
		  Area_Size = Integer; MaxLength = Integer;
		{ size                 maxlength }
		  tabindex; accesskey;
		{ tabindex  accesskey }
		  onfocus; onblur; onselect; onchange;
		{ onfocus  onblur  onselect  onchange }
		  Default_Value) =
		Text_With_Frame - (Input) with Area_Size ?= 20;
	Password_Input					{ INPUT / PASSWORD }
	   (ATTR !NAME; Value_; disabled; readonly;
		{ name  value   disabled  readonly }
		  Area_Size; MaxLength;
		{ size       maxlength }
		  tabindex; accesskey;
		{ tabindex  accesskey }
		  onfocus; onblur; onselect; onchange;
		{ onfocus  onblur  onselect  onchange }
		  Default_Value) =
		Text_With_Frame  - (Input) with Area_Size ?= 20;
	File_Input					{ INPUT / FILE }
	   (ATTR !NAME; Value_; disabled; readonly;
		{ name  value   disabled  readonly }
		  Area_Size; MaxLength;
		{ size       maxlength }
		  tabindex; accesskey;
		{ tabindex  accesskey }
		  onfocus; onblur; accept = Text;
		{ onfocus  onblur }
		  Default_Value) =
		Text_With_Frame - (Input) with Area_Size ?= 20;
	Checkbox_Input					{ INPUT / CHECKBOX }
	   (ATTR !NAME; Value_; Checked = Yes_, No_;
		{ name  value   checked }
		  disabled; readonly;
		{ disabled  readonly }
		  tabindex; accesskey; onfocus; onblur;
		{ tabindex  accesskey  onfocus  onblur }
		  DefaultChecked = Yes_, No_) =
		CONSTANT C_Empty with Checked ?= No_;
	Radio_Input					{ INPUT / RADIO }
	   (ATTR !NAME; !Value_; Checked; disabled;
		{ name  value    checked   disabled }
		  readonly; tabindex; accesskey;
		{ readonly  tabindex  accesskey }
		  onfocus; onblur;
		{ onfocus  onblur }
		  DefaultChecked) =
		CONSTANT C_Empty with Checked ?= No_,
				      NAME ?= 'radio';
	Submit_Input					{ INPUT / SUBMIT }
	   (ATTR  NAME; Value_; disabled; readonly;
		{ name  value   disabled; readonly }
		  tabindex; accesskey;
		{ tabindex  accesskey }
		  onfocus; onblur) =
		{ onfocus  onblur }
		BEGIN
		Frame;
		END with Value_ ?= 'Submit';
	Reset_Input					{ INPUT / RESET }
	   (ATTR  NAME; Value_; disabled; readonly;
		{ name  value   disabled  readonly }
		  tabindex; accesskey;
		{ tabindex  accesskey }
		  onfocus; onblur) =
		{ onfocus  onblur }
		BEGIN
		Frame;
		END  - (Input) with NAME ?= 'Reset',
				    Value_ ?= 'Reset';
        Button_Input					{ INPUT / BUTTON }
	   (ATTR !NAME; Value_; disabled; readonly;
		{ name  value   disabled  readonly }
		  tabindex; accesskey;
		{ tabindex  accesskey }
		  onfocus; onblur) =
		{ onfocus  onblur }
		BEGIN
		Frame;
		END;

        BUTTON						{ BUTTON }
	   (ATTR !NAME; Value_;
		{ name  value }
		  Button_type = button, submit, reset;
		{ type          button  submit  reset }
		  disabled; tabindex; accesskey;
		{ disabled  tabindex  accesskey }
		  onfocus; onblur) =
		{ onfocus  onblur }
		Basic_Set - (Anchor, Input, Form, FIELDSET, IFRAME);
        LABEL						{ LABEL }
	   (ATTR  Associated_control = TEXT;
		{ for }
		  accesskey; onfocus; onblur) =
		{ accesskey  onfocus  onblur }
		Basic_Set;
	Hidden_Input					{ INPUT / HIDDEN }
	   (ATTR !NAME; Value_; disabled;
		{ name  value   disabled }
		  tabindex; accesskey;
		{ tabindex  accesskey }
		  onfocus; onblur) =
		{ onfocus  onblur }
		CONSTANT C_Empty;
	Option_Menu;					{ SELECT }
	Text_Area;					{ TEXTAREA }
	END;

  Text_With_Frame = BEGIN
	Inserted_Text = TEXT;
	Frame = CONSTANT C_Empty;
	END;

  Table_row							{ TR }
     (ATTR Cell_align = Cell_left, Cell_center, Cell_right,
	 { align        left       center       right }
			Cell_justify, Cell_char;
	 {		justify       char }
	   char = Text; charoff = Text;
	 { char	        charoff }
	   Row_valign = Row_top, Row_middle, Row_bottom, Row_baseline;
	 { valign       top      middle      bottom      baseline }
	   BackgroundColor; height\240 = Integer) =
	 { bgcolor          Special attribute to control row span }
	LIST OF (Table_cell);

  Table								{ TABLE }
     (ATTR summary = Text; Width__; Border = Integer;
	 { summary         width    border }
	   frame = void, above, below, hsides, lhs, rhs,
	 { frame   void  above  below  hsides  lhs  rhs  }
		   vsides, box, border;
	 {	   vsides  box  border }
	   rules_ = none, groups, rows, cols, all;
	 { rules    none  groups  rows  cols  all }
	   cellspacing = Integer; cellpadding = Integer;
	 { cellspacing	          cellpadding }
	   Align; BackgroundColor;
	 { align  bgcolor }
	   datapagesize = Text; IntWidthPxl; IntWidthPercent) =
	 { datapagesize }
	BEGIN
	CAPTION;						{ CAPTION }
	Cols = LIST OF (Col_elem = CASE OF
				COL;				{ COL }
				COLGROUP;			{ COLGROUP }
				END);
	Table_head = LIST OF (Column_head);
      ? thead;							{ THEAD }
	Table_body = LIST OF (tbody);
      ? tfoot;							{ TFOOT }
	Table_foot = CONSTANT C_Foot;
	END;

  CAPTION							{ CAPTION }
     (ATTR Position = Position_top, Position_bottom,
	 { align      top           bottom }
	   Position_left, Position_right) =
	 { left           right }
	Basic_Set;

  Column_head (ATTR IntWidthPercent; IntWidthPxl) =
	CONSTANT C_Head;

  thead								{ THEAD }
     (ATTR Cell_align; char; charoff; Row_valign) =
	LIST OF (Table_row);

  tbody								 { TBODY }
     (ATTR Cell_align; char; charoff; Row_valign) =
	LIST OF (Table_row);

  tfoot								{ TFOOT }
     (ATTR Cell_align; char; charoff; Row_valign) =
	LIST OF (Table_row);

  Table_cell = CASE OF
	Data_cell;						{ TD }
	Heading_cell;						{ TH }
	END;

  Data_cell							{ TD }
     (ATTR Ref_column = REFERENCE(Column_head);
           ColExt = REFERENCE(Column_head);
           RowExt = REFERENCE(Table_row);
	   abbr = Text; axis = Text; headers = Text; scope = Text;
	 { abbr         axis         headers         scope }
           rowspan_ = INTEGER; colspan_ = INTEGER;
	 { rowspan             colspan }
           Cell_align; char; charoff;
	 { align       char  charoff }
           Cell_valign = Cell_top, Cell_middle, Cell_bottom, Cell_baseline;
	 { valign        top       middle       bottom       baseline }
           No_wrap = no_wrap; BackgroundColor;
	 { nowrap             bgcolor }
	   Width__; Height_; IntWidthPxl; IntWidthPercent) =
	 { width    height }
	LIST OF (Element);

  Heading_cell							{ TH }
     (ATTR Ref_column; ColExt; RowExt;
	   abbr; axis; headers; scope;
	 { abbr  axis  headers  scope }
	   rowspan_; colspan_;
	 { rowspan   colspan }
	   Cell_align; char; charoff; Cell_valign;
	 { align       char  charoff  valign }
	   No_wrap; BackgroundColor;
	 { nowrap   bgcolor }
	   Width__; Height_; IntWidthPxl; IntWidthPercent) =
	 { width    height }
	LIST OF (Element);

  COLGROUP							{ COLGROUP }
    (ATTR Cell_align; Cell_valign; Width__; span_ = Integer)
	{ align       valign       width    span }
	 = LIST OF (COL);

  COL								{ COL }
    (ATTR Cell_align; Cell_valign; Width__; span_)
	{ align       valign       width    span }
	 = CONSTANT C_Empty;

  MAP								{ MAP }
     (ATTR NAME; Ref_IMG = REFERENCE(Any)) =
	 { name }
	LIST OF (AREA);

  AREA								{ AREA }
     (ATTR shape; coords; HREF_; target_; nohref = Yes_; ALT;
	 { shape  coords  href   target   nohref         alt }
	   tabindex; accesskey; onfocus; onblur;
	 { tabindex  accesskey  onfocus  onblur }
	   x_coord = Integer; y_coord = Integer; IntWidthPxl;
           height\240; AreaRef_IMG = REFERENCE(Any)) =
	GRAPHICS with x_coord ?= 0, y_coord ?= 0,
		      IntWidthPxl ?= 25, height\240 ?= 10;

  FRAMESET							{ FRAMESET }
     (ATTR RowHeight = Text; ColWidth = Text;
	 { rows              cols }
	   onload; onunload) =
	 { onload  onunload }
	AGGREGATE
	Frames = LIST [1 .. *] OF (FrameElem = CASE OF
					FRAME;
					FRAMESET;
					END - (NOFRAMES));
      ? NOFRAMES = BODY + (INS, DEL);				{ NOFRAMES }
	END;

  FRAME								{ FRAME }
     (ATTR longdesc; FrameName = Text; FrameSrc = Text;
	 { longdesc  name	       src }
	   frameborder = Border1, Border0;
	 { frameborder   1        0 }
	   marginwidth = Integer; marginheight = Integer;
	 { marginwidth            marginheight }
	   no_resize = Yes_; scrolling = Yes_, No_, auto) =
	 { noresize          scrolling   yes   no   auto }
	CONSTANT C_Empty;

  IFRAME							{ IFRAME }
     (ATTR longdesc; FrameName; FrameSrc; frameborder;
	 { longdesc  name       src       frameborder }
	   marginwidth; marginheight; scrolling;
	 { marginwidth  marginheight  scrolling }
	   Alignment; Width__; Height_) =
	 { align      width    height }
	LIST OF (Element);

  { AnyLink is an alias for all elements with an attribute
    of type URI.  Only elements to be shown in the Links view
    are taken into account }
  AnyLink = CASE OF
	Anchor;		{ HREF_ }
	Block_Quote;	{ cite }
	Quotation;	{ cite }
	INS;		{ cite }
	DEL;		{ cite }
	END;

EXCEPT
	HTML:           NoMove, NoResize;
	BODY:		NoCut, CssBackground;
	HEAD:		NoCut;
	TITLE:		NoCut;
	Document_URL:	Hidden, NoSpellCheck;
	C_Empty:	Hidden;
	C_BR:		NoSelect;
	Head_misc:	Hidden;
	Frames:		Hidden;
	Frame:		Hidden, NoCut;
	FRAMESET:	NoCreate;	{ prevent a Return at the end of the
					  document from creating a FRAMESET }
	Inserted_Text:	Hidden, NoCut;
	Basic_Set:	Hidden;
	Term_List:	Hidden;
	Definition_Item:Hidden;
	Comment_line:	Hidden;
	GRAPHICS:	NoMove, NoResize;
	TEXT:		NoMove, NoResize;
	PICTURE:	MoveResize;
	Invalid_element:NoCreate;
	Pseudo_paragraph: ParagraphBreak, Hidden;
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
	Password_Input: Shadow;
	Preformatted:	ReturnCreateNL;
	STYLE_:		ReturnCreateNL;
	SCRIPT:		ReturnCreateNL;
	Text_Area:	ReturnCreateNL;
	HTMLfragment:	NoMove, NoResize, Hidden;
	XMLGraphics:    MoveResize, NoMove;

	ExternalLink:	Invisible;
	InternalLink:	Invisible;
	Annotation:     Invisible;
	IntWidthPercent:NewPercentWidth, Invisible;
	IntWidthPxl:	NewWidth, Invisible;
	IntItemStyle:	Invisible;
	IntSizeIncr:	Invisible;
	IntSizeDecr:	Invisible;
	IntSizeRel:	Invisible;
	IsInput:	Invisible;
	x_coord:	Invisible, NewHPos;
	y_coord:	Invisible, NewVPos;
	height\240:	Invisible, NewHeight;
	Ref_IMG:	Invisible;
	AreaRef_IMG:	Invisible;
	Invalid_attribute: Invisible;
	Highlight:	Invisible;
	PseudoClass:	Invisible, CssPseudoClass;
	Default_Value:	Invisible;
	DefaultSelected:Invisible;
	DefaultChecked:	Invisible;
	SectionNumbering:Invisible;
	ShowAreas:	Invisible;
	PrintURL:       Invisible;
	Namespaces:	Invisible;
	RealLang:	Invisible;
	HtmlDTD:	Invisible;
	shape:		Invisible;
	Data_cell:	IsCell, NoMove, NoResize;
	Heading_cell:	IsCell, NoMove, NoResize;
	Table:		IsTable, PageBreak;
	Column_head:	IsColHead, NoCut, Hidden;
	Table_head:	NoCut, Hidden, NoSelect;
	C_Head:		Hidden, NoSelect;
	Table_foot:	NoCut, Hidden, NoSelect, PageBreakRepBefore;
	C_Foot:		Hidden, NoSelect;
	Cols:		Hidden, NoSelect;
	Table_body:	PageBreakAllowed, Hidden;
	Table_row:	IsRow, PageBreakPlace;
	ColExt:		Invisible;
	RowExt:		Invisible;
	Ref_column:	ColRef, Invisible;
	colspan_:	ColSpan;
	rowspan_:	RowSpan;
	Object_Content: Hidden, NoCreate, NoCut;
	Ghost_restruct:	Invisible;
        IntEntity:	Invisible; 
	onclick:	EventAttr;
	ondblclick:	EventAttr;
	onmousedown:	EventAttr;
	onmouseup:	EventAttr;
	onmouseover:	EventAttr;
	onmousemove:	EventAttr;
	onmouseout:	EventAttr;
	onkeypress:	EventAttr;
	onkeydown:	EventAttr;
	onkeyup:	EventAttr;
	ID :		CssId;
	Class:		CssClass;

END
