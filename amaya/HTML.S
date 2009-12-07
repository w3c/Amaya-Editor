		{ . Vatton 	 Nonember 1994-2002  }

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
   dir = ltr_, rtl_;		{ dir }
		 	{ RDFa }
   about = Text;		{ about }
   property = Text;		{ property }
   resource = Text;		{ resource }
   datatype = Text;		{ datatype }
   typeof = Text;		{ typeof }
   REL = Text;			{ rel }
   REV = Text;			{ rev }
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
   Unknown_attribute = Text;
   Ghost_restruct = Text;
   Highlight = Yes_;    { to show the HTML element corresponding to the
			  current selection in the source view }
   IntEntity = Yes_;	{ do not translate & into &amp; in output file }
   EntityName = Text;   { the content of the element is the name of an entity }
   xmlid = Text;		{ xml:id }
   xml_space = xml_space_default, xml_space_preserve; { xml:space }
   NoImages = Yes_;
   NoObjects = Yes_;

CONST

   C_Empty = ' ';
   C_Head = ' ';
   C_BR   = '\12';

STRUCT

  HTML								{ HTML }
     (ATTR ShowAreas = Yes_;
	   PrintURL = Yes_; PI = Text;
	   Namespaces = Text; RealLang = Yes_; Charset = Text) =
        BEGIN
        HEAD;
      ? BODY;        { only one of BODY, document type }
      ? FRAMESET;
        END + (Invalid_element, ASP_element, Unknown_namespace, XHTML_Unknown_namespace, Comment\240, XMLPI, CDATA);
        
  DOCTYPE = LIST OF (DOCTYPE_line = TEXT);

  Invalid_element = TEXT;
  ASP_element = LIST OF (ASP_line = TEXT);
  Unknown_namespace = TEXT;

  XHTML_Unknown_namespace = TEXT;

  Comment\240 = LIST OF (Comment_line = TEXT) - (Comment\240);

  XMLPI (ATTR is_css = Text) = LIST OF (PI_line = TEXT);
        
  CDATA = LIST OF (CDATA_line = TEXT);

  HEAD                                                                { HEAD }
     (ATTR profile = Text) =
        AGGREGATE
        Document_URL = TEXT - (STYLE_, SCRIPT_, META, LINK);
        TITLE = TEXT - (STYLE_, SCRIPT_, META, LINK);                { TITLE }
      ? ISINDEX (ATTR Prompt = Text) = CONSTANT C_Empty;        { ISINDEX }
                    { prompt }
      ? BASE;                                                        { BASE }
        END + (Object, STYLE_, SCRIPT_, META, LINK);

  STYLE_                                                        { STYLE }
     (ATTR !Notation = Text; media = Text) =
         {  type             media }
         TEXT - (STYLE_, SCRIPT_, META, LINK);

  SCRIPT_                                                        { SCRIPT }
     (ATTR charset = Text; !content_type = Text;
         { charset          type }
           script_language = Text; script_src = Text;
         { language                src }
           defer = Yes_; event = Text; for_ = Text) =
         { defer         event         for }
        TEXT - (STYLE_, SCRIPT_, META, LINK) with content_type ?= 'text/javascript';

  META                                                                { META }
     (ATTR http_equiv = Text;
         { http-equiv }
           meta_name = Text; meta_content = Text;
         { name               content }
           scheme = Text) =
         { scheme }
        CONSTANT C_Empty;

  LINK                                                                { LINK }
     (ATTR charset; HREF_ = Text; hreflang = Text;
         { charset  href          hreflang }
           Link_type = Text;
         { type }
           media; target_ = Text) =
         { media  target }
        CONSTANT C_Empty;

  BASE                                                                { BASE }
     (ATTR !HREF_; target_) =
          { href   target }
        CONSTANT C_Empty;

        IMG                                                        { IMG }
            (ATTR !SRC = Text; !ALT = Text; longdesc = Text;
                {  src          alt         longdesc }
                  Height_ = Text; Width__ = Text;
                { height          width }
                  IntWidthPercent = Integer; IntWidthPxl = Integer;
                  IntHeightPercent = Integer; IntHeightPxl = Integer;
                  USEMAP = Text; ISAMAP = Yes_;
                { usemap         ismap }
                  Alignment = Top_, Middle_, Bottom_, Left_, Right_;
                { align       top   middle   bottom   left   right }
                  Img_border = Integer;
                { border }
                  hspace = Integer; vspace = Integer) =
                { hspace            vspace }
    BEGIN
            PICTURE (ATTR SRC);
    END;

  BODY                                                                { BODY }
     (ATTR onload = Text; onunload = Text;
         { onload         onunload }
           background_ = Text; BackgroundColor = Text;
         { background          bgcolor }
           TextColor = Text; LinkColor = Text;
         { text              link }
           VisitedLinkColor = Text; ActiveLinkColor = Text) =
         { vlink                    alink }
        LIST OF (Element);

  Element = CASE OF                                                { block }
        Paragraph;                                                { P }
        Pseudo_paragraph;
        Heading;                                                { heading }
        List_;                                                        { list + DL }
        Preformatted;                                                { PRE preformatted }
        Division;                                                { DIV }
        Center;                                                        { CENTER }
        SCRIPT_;                                                { SCRIPT }
        NOSCRIPT = LIST OF (Element) - (SCRIPT_);                { NOSCRIPT }
        Block_Quote;                                                { BLOCKQUOTE }
        Form;                                                        { FORM }
        ISINDEX;                                                { ISINDEX }
        Horizontal_Rule;                                        { HR }
        Table_;                                                        { TABLE }
        FIELDSET;                                                { FIELDSET }
        Address;                                                { ADDRESS }
        IMG;                                                          { IMG }
        INS;
        DEL;
        Object;                                                        { OBJECT }
        Applet;                                                        { APPLET }
        MathML;                                                  { MATH }
        SVG;                                  { SVG }
        XLink;                                                        { Annotations }
        External_Object                  { IMG type=svg}
            (ATTR SRC;
                { src }
                  Height_; Width__;
                { height   width }
                  IntWidthPercent; IntWidthPxl;
                  IntHeightPercent; IntHeightPxl;
                  USEMAP; ISAMAP;
                { usemap  ismap }
                  NAME = Text;
                { name }
                  Alignment;
                { align   top   middle   bottom   left   right }
                  Img_border;
                { border }
                  hspace; vspace) =
                { hspace  vspace }
                 BEGIN
                 External_Object_Content;
                 END;
        Embed_                                                { EMBED }
            (ATTR SRC;
                  Alignment;
                { align   top   bottom   left   right }
                  Height_; Width__;
                { height   width }
                  EmbedHidden = Yes_, No_;
                { hidden        yes   no }
                  EmbedName = Text;
                { name }
                  hspace; vspace;
                { hspace  vspace }
                  Embed_type = Text;
                { type }
                  pluginspage = Text;
                  pluginurl = Text; 
                ) =        
                BEGIN
                Embed_Content;
                END;
        Template;           { XTiger }
        END;

        HTMLfragment = LIST OF (Element);
        External_Object_Content = NATURE;
        Embed_Content = NATURE;

  Block_Quote                                                        { BLOCKQUOTE }
     (ATTR cite = Text;
         { cite }
           InternalLink = Reference(Any); ExternalLink = Yes_) =
        LIST OF (Element);

  Horizontal_Rule                                                { HR }
     (ATTR Align = left_, center_, right_;
         { align   left   center   right }
           NoShade = NoShade_; Size_ = Integer; Width__;
         { noshade             size             width }
           IntWidthPercent; IntWidthPxl) =
        CONSTANT C_Empty;

  Basic_Set = LIST OF (Basic_Elem);

  { Paragraph must be the first element defined as a Basic_Set, to make
    sure that the editor will create a Paragraph when the user wants to
    create a Basic_Elem }
  Paragraph                                                        { P }
     (ATTR TextAlign = left_, center_, right_, justify_) =
         { align       left   center   right   justify }
        Basic_Set;

  Address = Basic_Set;

  Pseudo_paragraph = Basic_Set;

  Heading = CASE OF                                                { heading }
        H1 (ATTR TextAlign) = Basic_Set;                        { H1 }
               { align }
        H2 (ATTR TextAlign) = Basic_Set;                        { H2 }
        H3 (ATTR TextAlign) = Basic_Set;                        { H3 }
        H4 (ATTR TextAlign) = Basic_Set;                        { H4 }
        H5 (ATTR TextAlign) = Basic_Set;                        { H5 }
        H6 (ATTR TextAlign) = Basic_Set;                        { H6 }
        END;

  Preformatted                                                        { PRE }
     (ATTR Width__; IntWidthPercent; IntWidthPxl) =
         { width }
        Basic_Set
             - (IMG, Object, Applet,
                Big_text, Small_text, Subscript, Superscript,
                Font_, BaseFont, External_Object);

  Anchor                                                        { A }
     (ATTR charset; Link_type; NAME;
         { charset  type       name }
           HREF_; hreflang; target_;
         { href   hreflang  target}
           accesskey = Text;
         { accesskey }
           shape = rectangle, circle, polygon, default_;
         { shape   rect       cirecle poly     default }
           coords = Text; tabindex = Integer;
         { coords         tabindex }
           onfocus = Text; onblur = Text;
         { onfocus         onblur }
           InternalLink; ExternalLink;
         { InternalLink  ExternalLink }) =
        Basic_Set;

  Basic_Elem = CASE OF                                                { special }
        TEXT;
        Font;                                                        { fontstyle }
        Phrase;                                                        { phrase }
        Form_Element;                                           { formctrl }
        Anchor;                                                        { A }
        IMG;                                                { IMG }
        Applet;                                                        { APPLET }
        Font_;                                                        { FONT }
        BaseFont;                                                { BASEFONT }
        BR;                                                        { BR }
        SCRIPT_;                                                { SCRIPT }
        Quotation;                                                { Q }
        Subscript = Basic_Set;                                        { SUB }
        Superscript = Basic_Set;                                { SUP }
        Span = Basic_Set;                                        { SPAN }
        map; MAP;                                                { MAP }
        BDO (ATTR !dir) = Basic_Set;                                { BDO }
        Object;                                                        { OBJECT }
        Embed_;                                                        { EMBED }
        IFRAME;                                                        { IFRAME }
        ruby;                                                   { ruby }
        MathML;                                                        { MATH }
        XLink;                                                        { Annotations }
        Template;           { XTiger }
        External_Object;                                        { External Object }
        END;

  BaseFont                                                        { BASEFONT }
     (ATTR !BaseFontSize = Integer; BaseFontColor = Text;
         {  size                    color }
           BaseFontFace = Text) =
         { face }
        CONSTANT C_Empty;

  BR                                                                { BR }
     (ATTR Clear_ = Left_, Right_, All_, None_) =
         { clear    left   right   all   none }
        CONSTANT C_BR;

  Quotation                                                        { Q }
     (ATTR cite; InternalLink; ExternalLink) =
         { cite }
        Basic_Set;

  Font = CASE OF                { fontstyle }
        Teletype_text = Basic_Set;        { TT }
        Italic_text = Basic_Set;        { I }
        Bold_text = Basic_Set;                { B }
        Underlined_text = Basic_Set;        { U }
        Struck_text = Basic_Set;        { S or STRIKE }
        Big_text = Basic_Set;                { BIG }
        Small_text = Basic_Set;                { SMALL }
        END;

  Phrase = CASE OF                { phrase }
        Emphasis = Basic_Set;                { EM }
        Strong = Basic_Set;                { STRONG }
        Def = Basic_Set;                { DFN }
        Code = Basic_Set;                { CODE }
        Sample = Basic_Set;                { SAMP }
        Keyboard = Basic_Set;                { KBD }
        Variable_ = Basic_Set;                { VAR }
        Cite = Basic_Set;                { CITE }
        ABBR = Basic_Set;                { ABBR }
        ACRONYM = Basic_Set;                { ACRONYM }
        ins;                             { INS }
        del;                             { DEL }
        END;

  Font_                                                                { FONT }
     (ATTR Font_size = Text; color = Text; face = Text;
         { size              color              face }
           IntSizeIncr = Integer; IntSizeDecr = Integer;
           IntSizeRel = Integer) =
        Basic_Set;

  Applet                                                        { APPLET }
     (ATTR codebase = Text; archive = Text; code = Text;
         { codebase         archive         code }
           object = Text; alt = Text; applet_name = Text;
         { object         alt         name }
           !Width__; IntWidthPercent; IntWidthPxl;
         {  width }
           !Height_; IntHeightPercent; IntHeightPxl;
         {  height }
           Alignment; hspace; vspace) =
         { align      hspace  vspace }
        LIST OF (Applet_Content = CASE OF
                                  Parameter;                        { PARAM }
                                  Basic_Set;
                                  END);
  Parameter                                                        { PARAM }
     (ATTR !Param_name = Text; Param_value = Text;
         { name               value }
           valuetype = data_, ref, object_;
         { valuetype   data   ref  object }
           Param_type = Text) =
         { type }
        CONSTANT C_Empty;

  Object                                                        { OBJECT }
     (ATTR declare = declare_; classid = Text; codebase;
         { declare            classid         codebase }
           data = Text; Object_type = Text;
         { data         type }
           codetype = Text; archive; standby = Text;
         { codetype         archive  standby }
           Height_; Width__;
           IntWidthPercent; IntWidthPxl; 
           IntHeightPercent; IntHeightPxl;
         {  height    width }
           USEMAP; NAME; tabindex; Alignment; Img_border;
         { usemap  name  tabindex  align      border }
           hspace; vspace) =
         { hspace  vspace }
        BEGIN
        PICTURE;
        Object_Content = LIST OF (ElemOrParam = CASE OF
                                    Element; Parameter; END);   { PARAM }
        END;

  INS                                                                { INS }
     (ATTR cite; datetime = Text; InternalLink; ExternalLink) =
         { cite  datetime }
        LIST OF (Element);

  DEL                                                                { DEL }
     (ATTR cite; datetime; InternalLink; ExternalLink) =
         { cite  datetime }
        LIST OF (Element);

  ins                                                                { INS }
     (ATTR cite; datetime; InternalLink; ExternalLink) =
         { cite  datetime }
        Basic_Set;

  del                                                                { DEL }
     (ATTR cite; datetime; InternalLink; ExternalLink) =
         { cite  datetime }
        Basic_Set;

  Block = CASE OF {Block}
        Paragraph;
        Pseudo_paragraph;
        Heading;
        List_;
        Preformatted;
        Division;
        Center;
        Block_Quote;
        Form;
        Horizontal_Rule;
        Table_;
        Address;
        IMG;
        Object;
        Applet;
        MathML;
        SVG;
        XLink;
        External_Object;
        Embed_;
        INS;
        DEL;
        END;

  Division                                                        { DIV }
     (ATTR TextAlign) =
         { align }
        LIST OF (Element);

  Center =                                                        { CENTER }
        LIST OF (Element);

  List_ = CASE OF                                                { list }
        Unnumbered_List;                                         { UL }
        Numbered_List;                                                { OL }
        Directory;                                                { DIR }
        Menu;                                                        { MENU }
        Definition_List;                                        { DL }
        END;

  Unnumbered_List                                                 { UL }
     (ATTR BulletStyle = disc_, circle_, square_;
         { type          disc   circle   square }
           COMPACT = Yes_) =
         { compact }
        LIST OF (List_Item);

  Numbered_List                                                        { OL }
     (ATTR NumberStyle = Arabic_, LowerAlpha, UpperAlpha,
         { type          1        a           A }
           LowerRoman_, UpperRoman_;
         { i            I }
           Start = Integer; COMPACT) =
         { start            compact }
        LIST OF (List_Item);

  Directory                                                        { DIR }
     (ATTR COMPACT) =
         { compact }
        LIST OF (List_Item);

  Menu                                                                { MENU }
     (ATTR COMPACT) =
         { compact }
        LIST OF (List_Item);

  List_Item                                                        { LI }
     (ATTR ItemStyle = disc_, circle_, square_, Arabic_,
         { type        disc   circle   square   1 }
           LowerAlpha, UpperAlpha, LowerRoman_, UpperRoman_;
         { a           A           i            I }
           ItemValue = Integer) =
         { value }
        LIST OF (Block);

  Definition_List                                                { DL }
     (ATTR COMPACT) =
         { compact }
        LIST OF (Definition_Item);

  Definition_Item = BEGIN
        Term_List = LIST OF (Term = Basic_Set);                        { DT }
      ? Definitions = LIST OF (Definition = LIST OF (Block));        { DD }
        END;

  Form                                                                { FORM }
     (ATTR !Script_URL = Text; METHOD = Get_, Post_;
         {  action             method }
           ENCTYPE = Text; onsubmit = Text; onreset = Text;
         { enctype         onsubmit         onreset }
           target_; accept_charset = Text) =
         { target   accept-charset }
        LIST OF (Element) + (Input) - (Form);

  Form_Element = CASE OF
        Input;                                                  { INPUT }
        Option_Menu;                                                { SELECT }
        Text_Area;                                                 { TEXTAREA }
        LABEL;                                                  { LABEL }
        BUTTON_;                                                { BUTTON }
        END;

  Option_Menu                                                   { SELECT }
     (ATTR  NAME; MenuSize = Integer; Multiple = Yes_;
         {  name  size                multiple }
            disabled = Yes_; tabindex; onfocus; onblur;
          { disabled         tabindex  onfocus  onblur}
            onchange = Text) =
          { onchange }
        LIST OF (Option_item = CASE OF Option; OptGroup; END) - (Input);

  Option                                                        { OPTION }
     (ATTR Selected = Yes_; DefaultSelected = Yes_; ShowMe = Yes_;
         { selected }
           disabled; label = Text; Value_ = Text) =
         { disabled  label         value }
        TEXT;

  OptGroup                                                        { OPTGROUP }
     (ATTR disabled; !label) =
         { disabled   label }
        LIST OF (Option);

  Text_Area                                                        { TEXTAREA }
     (ATTR NAME; !Rows = Integer; !Columns = Integer;
         { name   rows             cols }
           disabled; readonly = Yes_; tabindex; accesskey;
         { disabled  readonly         tabindex  accesskey  }
           onfocus; onblur; onselect = Text; onchange;
         { onfocus  onblur  onselect         onchange }
           Default_Value = Text) =
        Input_Text - (Input) {with Rows ?= 4, Columns ?= 20};

  FIELDSET =                                                        { FIELDSET }
        BEGIN
        LEGEND;
        Fieldset_Content = LIST OF (Element);
        END;

  LEGEND                                                        { LEGEND }
     (ATTR accesskey; LAlign = Top_, Bottom_, Left_, Right_) =
         { accesskey  align    top   bottom   left   right }
        Basic_Set;

  Input = CASE OF                                        { formctrl }
    Text_Input                                        { INPUT / TEXT }
           (ATTR  type = Text;
            NAME; Value_; disabled; readonly;
          { type name  value   disabled  readonly }
                  Area_Size = Integer; MaxLength = Integer;
                { size                 maxlength }
                  IntAreaSize = Integer;
                  tabindex; accesskey;
                { tabindex  accesskey }
                  onfocus; onblur; onselect; onchange;
                { onfocus  onblur  onselect  onchange }
                  Default_Value) =
                Input_Text - (Input) with type = 'text';

    Password_Input                                        { INPUT / PASSWORD }
           (ATTR  type; NAME; Value_; disabled; readonly; ALT;
                { type name  value   disabled  readonly alt}
                  Area_Size; IntAreaSize; MaxLength;
                { size                    maxlength }
                  tabindex; accesskey;
                { tabindex  accesskey }
                  onfocus; onblur; onselect; onchange;
                { onfocus  onblur  onselect  onchange }
                  Default_Value) =
                Input_Text  - (Input) with type = 'password';

    File_Input                                        { INPUT / FILE }
           (ATTR  type; NAME; Value_; disabled; readonly; ALT;
                { type name  value   disabled  readonly alt }
                  Area_Size; IntAreaSize; MaxLength;
                { size                    maxlength }
                  tabindex; accesskey;
                { tabindex  accesskey }
                  onfocus; onblur; accept = Text;
                { onfocus  onblur }
                  Default_Value) =
                Input_Text - (Input) with type = 'file';

    Checkbox_Input                                        { INPUT / CHECKBOX }
           (ATTR  type; NAME; Value_; Checked = Yes_, No_; ALT;
                { type name  value   checked alt }
                  disabled; readonly;
                { disabled  readonly }
                  tabindex; accesskey; onfocus; onblur;
                { tabindex  accesskey  onfocus  onblur }
                  DefaultChecked = Yes_, No_) =
                CONSTANT C_Empty with Checked ?= No_, type = 'checkbox';

    Image_Input                                        { INPUT / IMAGE }
           (ATTR  type; NAME; Value_; !SRC; ALT;
                { type name  value src alt }
                  Area_Size; IntWidthPxl;
                { size }
                  USEMAP; ISAMAP;
                { usemap  ismap }
                  disabled; readonly; tabindex; accesskey;
                { disabled  readonly  tabindex  accesskey }
                  onfocus; onblur;) =
                { onfocus  onblur }
                BEGIN
                PICTURE;
                END with NAME ?= 'radio', type = 'image';

    Radio_Input                                        { INPUT / RADIO }
           (ATTR  type; NAME; Value_; Checked; ALT;
                { type name  value   checked  disabled alt }
                  disabled; readonly; tabindex; accesskey;
                { disabled  readonly  tabindex  accesskey }
                  onfocus; onblur;
                { onfocus  onblur }
                  DefaultChecked) =
                CONSTANT C_Empty with Checked ?= No_, NAME ?= 'radio', type = 'radio';

    Submit_Input                                        { INPUT / SUBMIT }
           (ATTR  type; NAME; Value_; disabled; readonly; ALT;
                { type name  value   disabled; readonly alt }
                  tabindex; accesskey;
                { tabindex  accesskey }
                  onfocus; onblur) =
                { onfocus  onblur }
    BEGIN
      CONSTANT C_Empty;
    END with Value_ ?= 'Submit', type = 'submit';

    Reset_Input                                        { INPUT / RESET }
           (ATTR  type; NAME; Value_; disabled; readonly; ALT;
                { type name  value   disabled  readonly alt }
                  tabindex; accesskey;
                { tabindex  accesskey }
                  onfocus; onblur) =
                { onfocus  onblur }
                BEGIN
                CONSTANT C_Empty;
                END  - (Input) with NAME ?= 'Reset', Value_ ?= 'Reset', type = 'reset';

    Button_Input                                        { INPUT / BUTTON }
           (ATTR  type; NAME; Value_; disabled; readonly; ALT;
                { type name  value   disabled  readonly alt }
                  tabindex; accesskey;
                { tabindex  accesskey }
                  onfocus; onblur) =
                { onfocus  onblur }
                BEGIN
                CONSTANT C_Empty;
                END with type = 'button';

    Hidden_Input                                        { INPUT / HIDDEN }
           (ATTR  type; NAME; Value_; disabled; ALT;
                { type name  value   disabled alt }
                  tabindex; accesskey;
                { tabindex  accesskey }
                  onfocus; onblur) =
                { onfocus  onblur }
                CONSTANT C_Empty with type = 'hidden';

    BUTTON_                                                { BUTTON }
           (ATTR  NAME; Value_;
                { name  value }
                  Button_type = button, submit, reset;
                { button  submit  reset }
                  disabled; tabindex; accesskey;
                { disabled  tabindex  accesskey }
                  onfocus; onblur) =
                { onfocus  onblur }
                Basic_Set - (Anchor, Input, Form, FIELDSET, IFRAME);
    LABEL                                                { LABEL }
           (ATTR  Associated_control = TEXT;
                { for }
                  accesskey; onfocus; onblur) =
                { accesskey  onfocus  onblur }
                Basic_Set;

    Option_Menu;                                        { SELECT }
    Text_Area;                                        { TEXTAREA }
    END;

  Input_Text = BEGIN
        Inserted_Text = TEXT;
        END;

  Table_                                                        { TABLE }
     (ATTR summary = Text; Width__; Border = Integer;
         { summary         width    border }
           frame = void, above, below, hsides, lhs, rhs,
         { frame   void  above  below  hsides  lhs  rhs  }
                   vsides, box, border;
         {         vsides  box  border }
           rules_ = none_, groups, rows, cols, all;
         { rules    none   groups  rows  cols  all }
           cellspacing = Integer; cellpadding = Integer;
         { cellspacing                  cellpadding }
           Align; BackgroundColor;
         { align  bgcolor }
           datapagesize = Text; IntWidthPxl; IntWidthPercent) =
         { datapagesize }
        BEGIN
      ? CAPTION;                                                { CAPTION }
      ? ColStruct = CASE OF
            Cols = LIST OF (COL);                               { COL }
            Colgroups = LIST OF (COLGROUP);                     { COLGROUP }
            END;
        Table_head = LIST OF (Column_head);
	Table_content =
	    BEGIN
          ? thead;                                               { THEAD }
            Table_body = LIST OF (tbody);
          ? tfoot;                                               { TFOOT }
	    END;
        END;

  CAPTION                                                        { CAPTION }
     (ATTR Position = Position_top, Position_bottom,
         { align      top           bottom }
           Position_left, Position_right) =
         { left           right }
        Basic_Set;

  COLGROUP                                                        { COLGROUP }
    (ATTR Cell_align = Cell_left, Cell_center, Cell_right,
        { align        left       center       right }
                       Cell_justify, Cell_char;
        {              justify       char }
          Cell_valign = Cell_top, Cell_middle, Cell_bottom, Cell_baseline;
        { valign        top       middle       bottom       baseline }
          Width__; span_ = Integer)
        { width    span }
         = LIST [0..*] OF (COL);

  COL                                                                { COL }
    (ATTR Cell_align; Cell_valign; Width__; span_)
        { align       valign       width    span }
         = CONSTANT C_Empty;

  ColColgroup = CASE OF { alias for reference  Ref_ColColgroup }
        COL;
        COLGROUP;
        END;

  Column_head (ATTR IntWidthPercent; IntWidthPxl;
               IntWidthRelative = Integer;
               { IntWidthForced indicates that IntWidthPercent,
                 IntWidthPxl or IntWidthRelative comes from a
                 COL or COLGROUP element }
               IntWidthForced = IntWidthForced_;
               Ref_ColColgroup = REFERENCE(ColColgroup)) =
        CONSTANT C_Head;

  thead                                                                { THEAD }
     (ATTR Cell_align; char = Text; charoff = Text; 
           Row_valign = Row_top, Row_middle, Row_bottom, Row_baseline) =
         { valign       top      middle      bottom      baseline }
        LIST OF (Table_row);

  tbody                                                                 { TBODY }
     (ATTR Cell_align; char; charoff; Row_valign) =
        LIST OF (Table_row);

  tfoot                                                                { TFOOT }
     (ATTR Cell_align; char; charoff; Row_valign) =
        LIST OF (Table_row);

  Table_row                                                        { TR }
     (ATTR Cell_align; char; charoff; Row_valign;
         { align       char  charoff  valign }
           BackgroundColor; IntHeightPxl) =
         { bgcolor          Special attribute to control row span }
        LIST OF (Table_cell);

  Table_cell = CASE OF
        Data_cell;                                                { TD }
        Heading_cell;                                                { TH }
        END;

  Data_cell                                                        { TD }
     (ATTR Ref_column = REFERENCE(Column_head);
           ColExt = REFERENCE(Column_head);
           RowExt = REFERENCE(Table_row);
           abbr = Text; axis = Text; headers = Text; scope = Text;
         { abbr         axis         headers         scope }
           rowspan_ = INTEGER; colspan_ = INTEGER;
         { rowspan             colspan }
           Cell_align; char; charoff; Cell_valign;
         { align       char  charoff  valign }
	   IntCellAlign = IntCellLeft, IntCellCenter, IntCellRight,
	                  IntCellJustify, IntCellChar;
           No_wrap = no_wrap; BackgroundColor;
         { nowrap             bgcolor }
           Width__; IntWidthPxl; IntWidthPercent;
           Height_;  IntHeightPercent; IntHeightPxl;) =
         { width    height }
        LIST OF (Element);

  Heading_cell                                                        { TH }
     (ATTR Ref_column; ColExt; RowExt;
           abbr; axis; headers; scope;
         { abbr  axis  headers  scope }
           rowspan_; colspan_;
         { rowspan   colspan }
           Cell_align; char; charoff; Cell_valign;
	   IntCellAlign;
         { align       char  charoff  valign }
           No_wrap; BackgroundColor;
         { nowrap   bgcolor }
           Width__; IntWidthPxl; IntWidthPercent;
           Height_;  IntHeightPercent; IntHeightPxl) =
         { width    height }
        LIST OF (Element);

  MAP                                                                { MAP }
     (ATTR NAME; Ref_IMG = REFERENCE(Any)) =
         { name linked to an image}
        LIST OF (ElemOrArea = CASE OF Element; AREA; END);

  map                                                                { MAP }
     (ATTR NAME) =
         { name }
        LIST OF (Element);

  AREA                                                                { AREA }
     (ATTR shape; coords; HREF_; target_; nohref = Yes_; !ALT;
         { shape  coords  href   target   nohref          alt }
           tabindex; accesskey; onfocus; onblur;
         { tabindex  accesskey  onfocus  onblur }
           x_coord = Integer; y_coord = Integer; IntWidthPxl;
           IntHeightPxl; AreaRef_IMG = REFERENCE(Any)) =
        GRAPHICS with x_coord ?= 0, y_coord ?= 0,
                      IntWidthPxl ?= 25, IntHeightPxl ?= 10;

  FRAMESET                                                        { FRAMESET }
     (ATTR RowHeight = Text; ColWidth = Text;
         { rows              cols }
           onload; onunload) =
         { onload  onunload }
        AGGREGATE
        Frames = LIST [1 .. *] OF (FrameElem = CASE OF
                                        FRAME;
                                        FRAMESET;
                                        END - (NOFRAMES));
      ? NOFRAMES = BODY;                                               { NOFRAMES }
        END;

  FRAME                                                                { FRAME }
     (ATTR longdesc; NAME; FrameSrc = Text;
         { longdesc  name  src }
           frameborder = Border1, Border0;
         { frameborder   1        0 }
           marginwidth = Integer; marginheight = Integer;
         { marginwidth            marginheight }
           no_resize = Yes_; scrolling = Yes_, No_, auto_) =
         { noresize          scrolling   yes   no   auto }
        CONSTANT C_Empty;

  IFRAME                                                        { IFRAME }
     (ATTR longdesc; NAME; FrameSrc; frameborder;
         { longdesc  name  src       frameborder }
           marginwidth; marginheight; scrolling;
         { marginwidth  marginheight  scrolling }
           Alignment; Width__; Height_) =
         { align      width    height }
        BEGIN
        Iframe_Src_Content= NATURE;
        Iframe_Content = LIST OF (Element);                     { flow }
        END;

  ruby =                                                        { ruby }
        CASE OF
        simple_ruby;
        complex_ruby;
        END;

  simple_ruby =                                                 { ruby }
        BEGIN
        rb = Basic_Set - (simple_ruby, complex_ruby);
        RtOrRtWithPar = CASE OF
              rt (ATTR rbspan = Integer) =
                  Basic_Set - (simple_ruby, complex_ruby);
              RtWithPar = BEGIN
                  rp = TEXT;
                  rt;
                  rp;
                  END;
              END;
        END;

  complex_ruby =                                               { ruby }
        BEGIN
        rbc = LIST OF (rb);
        rtc1 = LIST OF (rt);
      ? rtc2 = LIST OF (rt);
        END;        
        
  { AnyLink is an alias for all elements with an attribute
    of type URI.  Only elements to be shown in the Links view
    are taken into account }
  AnyLink = CASE OF
        Anchor;             { HREF_ }
        Block_Quote;        { cite }
        Quotation;          { cite }
        ins;                { cite }
        del;                { cite }
        END;

  { ParagEquiv is an alias for elements that are usually formatted like
	paragraphs }
  ParagEquiv = CASE OF
	Paragraph; Pseudo_paragraph;
	H1; H2; H3; H4; H5; H6;
	Term;
	Address;
	Preformatted;
	CAPTION;
	END;

EXCEPT
        HTML:           NoMove, NoResize;
        BODY:           NoCut, CssBackground, NoMove, NoResize,
                        SetWindowBackground, NoBreakByReturn;
        HEAD:           NoCut;
        TITLE:          NoCut, NoSpellCheck;
        Document_URL:   Hidden, NoSpellCheck, NoCut;
        C_Empty:        Hidden, NoSpellCheck, NoSelect;
        C_BR:           SelectParent;
        Frames:         Hidden;
        FRAMESET:       NoCreate;        { prevent a Return at the end of the
                                          document from creating a FRAMESET }
        Inserted_Text:  Hidden, NoCut, NoSpellCheck, CheckAllChars;
        Basic_Set:      Hidden;
        Definition_Item:Hidden;
        Term_List:      Hidden, CanCut;
        Definitions:    Hidden;
        XMLPI:          ReturnCreateNL, NoSpellCheck, NoReplicate, NotAnElementNode;
        Comment\240:    ReturnCreateNL, NoSpellCheck, NoReplicate, NotAnElementNode;
        Comment_line:   Hidden, MarkupPreserve;
        ASP_element:    ReturnCreateNL, NoSpellCheck, NoReplicate, NotAnElementNode;
        ASP_line:   Hidden, MarkupPreserve;
        PI_line:        Hidden, MarkupPreserve;
        DOCTYPE:        NoCut, NoSpellCheck, NotAnElementNode;
        DOCTYPE_line:   Hidden, NoCut;
        CDATA_line:     Hidden, MarkupPreserve;
        GRAPHICS:       NoMove, NoResize, SelectParent;
        TEXT:           NoMove, NoResize;
        PICTURE:        MoveResize;
        Invalid_element:NoCreate;
        Unknown_namespace:NoCreate;
        XHTML_Unknown_namespace:NoCreate;
        BR:             IsBreak;
        Pseudo_paragraph: ParagraphBreak, Hidden;
        Basic_Elem:     Hidden;
        Paragraph:      ParagraphBreak;
        H1:             ParagraphBreak;
        H2:             ParagraphBreak;
        H3:             ParagraphBreak;
        H4:             ParagraphBreak;
        H5:             ParagraphBreak;
        H6:             ParagraphBreak;
        Address:        ParagraphBreak;
        Term:           ParagraphBreak;
        List_Item:      ListItemBreak;
        Definition:      ListItemBreak;
        MAP:            IsDraw, IsMap, NoReplicate;
        IMG:            IsImg, NoReplicate;
        AREA:           MoveResize, ClickableSurface, HighlightChildren;
        Anchor:         ClickableSurface;
        Horizontal_Rule: ClickableSurface;
        Form:           NoReplicate;
        Image_Input:    IsImg, NoReplicate;
        Password_Input: Shadow, NoReplicate;
        Text_Input:     NoReplicate;
        File_Input:     NoReplicate;
        Checkbox_Input: NoReplicate;
        Radio_Input:    NoReplicate;
        Submit_Input:   NoReplicate;
        Reset_Input:    NoReplicate;
        Hidden_Input:   NoReplicate;
        Button_Input:   NoReplicate;
        BUTTON_:        NoReplicate;
        Preformatted:   ReturnCreateNL, NoReplicate;
        STYLE_:         ReturnCreateNL, NoSpellCheck, NoReplicate;
        SCRIPT_:        ReturnCreateNL, NoReplicate, MarkupPreserve;
        Text_Area:      ReturnCreateNL, NoReplicate;
        HTMLfragment:   NoMove, NoResize, Hidden, NoBreakByReturn;
        Division:       NoMove, NoResize, NoBreakByReturn;
        INS:            NoReplicate;
        DEL:            NoReplicate;
        ins:            NoReplicate;
        del:            NoReplicate;
        ExternalLink:   Invisible;
        InternalLink:   Invisible;
        IntWidthPercent:NewPercentWidth, Invisible;
        IntWidthPxl:    NewWidth, Invisible;
        IntWidthRelative:NewWidth, Invisible;
        IntWidthForced: Invisible;
        IntHeightPercent:Invisible;
        IntHeightPxl:   NewHeight, Invisible;
        IntAreaSize:    Invisible;
        IntSizeIncr:    Invisible;
        IntSizeDecr:    Invisible;
        IntSizeRel:     Invisible;
        IntCellAlign:	Invisible;
        x_coord:        Invisible, NewHPos;
        y_coord:        Invisible, NewVPos;
        Ref_IMG:        Invisible;
        AreaRef_IMG:    Invisible;
        Invalid_attribute: Invisible;
        Unknown_attribute: Invisible;
        Highlight:      Invisible;
        PseudoClass:    Invisible, CssPseudoClass;
        Default_Value:  Invisible;
        DefaultSelected:Invisible;
        ShowMe:         Invisible;
        DefaultChecked: Invisible;
        ShowAreas:      Invisible;
        PrintURL:       Invisible;
        NoImages:       Invisible;
        NoObjects:      Invisible;
        Namespaces:     Invisible;
        RealLang:       Invisible;
        Charset:        Invisible;
        PI:             Invisible;
        shape:          Invisible;
        Data_cell:      IsCell, NoMove, NoResize, NoBreakByReturn;
        Heading_cell:   IsCell, NoMove, NoResize, NoBreakByReturn;
        Table_cell:     IsCell, NoMove, NoResize, NoBreakByReturn;
        Table_:         IsTable, PageBreak, NoReplicate, NoBreakByReturn;
        CAPTION:        IsCaption;
        Column_head:    IsColHead, NoCut, Hidden, NoBreakByReturn;
        Table_head:     NoCut, Hidden, NoSelect, NoBreakByReturn;
        C_Head:         Hidden, NoSelect;
        ColStruct:      Hidden, NoSelect, NoCreate, NoBreakByReturn;
        Cols:           Hidden, NoSelect, NoCreate, NoBreakByReturn;
        Colgroups:      Hidden, NoSelect, NoCreate, NoBreakByReturn;
        Table_content:	PageBreakAllowed, Hidden, NoBreakByReturn;
        Table_body:     PageBreakAllowed, Hidden, NoBreakByReturn;
        thead:          NoBreakByReturn; 
        tbody:          NoBreakByReturn;
        tfoot:          NoBreakByReturn;
        Table_row:      IsRow, PageBreakPlace, NoBreakByReturn;
        rbc:            Hidden;
        rtc1:           Hidden;
        rtc2:           Hidden;
        ColExt:         Invisible;
        RowExt:         Invisible;
        Ref_column:     ColRef, Invisible;
        Ref_ColColgroup:ColColRef;
        colspan_:       ColSpan;
        rowspan_:       RowSpan;
        Ghost_restruct: Invisible;
        IntEntity:      Invisible;
        EntityName:     Invisible;
        onclick:        EventAttr;
        ondblclick:     EventAttr;
        onmousedown:    EventAttr;
        onmouseup:      EventAttr;
        onmouseover:    EventAttr;
        onmousemove:    EventAttr;
        onmouseout:     EventAttr;
        onkeypress:     EventAttr;
        onkeydown:      EventAttr;
        onkeyup:        EventAttr;
        onload:         EventAttr;
        onunload:       EventAttr;
        onfocus:        EventAttr;
        onblur:         EventAttr;
        onsubmit:       EventAttr;
        onreset:        EventAttr;
        onchange:       EventAttr;
        onselect:       EventAttr;
        ID :            CssId; {Default id attribute}
        Class:          CssClass, DuplicateAttr;
        External_Object: Hidden;
        External_Object_Content: Hidden, NoSpellCheck, NewRoot;
        Embed_:         NoBreakByReturn, NoReplicate, NoSpellCheck;
        Embed_Content:  Hidden, NoSelect, NewRoot;
        Object:         NoBreakByReturn, NoReplicate, IsImg;
        Object_Content: Hidden, NoSelect, NoCreate, NoCut, NoBreakByReturn;
        Applet:         NoBreakByReturn, NoReplicate;
        Applet_Content: Hidden, NoSelect, NoCreate, NoCut, NoBreakByReturn;
        IFRAME:         NoBreakByReturn, NoReplicate, NoSpellCheck;
        Iframe_Content: Hidden, NoSelect, NoCut, NoBreakByReturn;
        Iframe_Src_Content: Hidden, NoCut, NoSelect, NewRoot;
        Fieldset_Content: Hidden, NoSelect, NoCut, NoBreakByReturn;
        is_css:         Invisible;
        Start:          StartCounter;
        ItemValue:      SetCounter;
        xml_space:      SpacePreserve;
END
