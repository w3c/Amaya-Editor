		{ Author: I. Vatton 	 1994-2007  }

{ If variable XML is set, the document is saved in the XHTML syntax.
  Otherwise, it is saved in plain HTML }

TRANSLATION HTML;

LINELENGTH 78;

BUFFERS
   ElemPrefixBuffer (variable);
   AttrPrefixBuffer (variable);

CONST
   DoubleQuote  = '"';

VAR
   SOT : '<' ElemPrefixBuffer;        { Start of opening tag }
   SCT : '</' ElemPrefixBuffer;        { Start of closing tag }
   SAN : ' ' AttrPrefixBuffer;  { Start of attribute name }

RULES

 HTML : BEGIN
        Use MathMLT for MathML;
        Use SVGT for SVG;
        Use XLinkT for XLink;
        Use TemplateT for Template;
        Create PI Before;
        create SOT;
        Create 'html';
        Create Attributes;
        Create '>\12';
        create SCT after;
        Create 'html>\12' After;
        END;

 HEAD:        BEGIN
        create SOT;
        Create 'head' Before;
        Create Attributes Before;
        Create '>\12' Before;
        Indent +2 Before;
        Indent -2 After;
        create SCT after;
        Create 'head>\12\12' After;
        END;

 Document_URL:
        BEGIN
        Remove;
        END;

 TITLE:        BEGIN
        create SOT;
        Create 'title';
        Create Attributes;
        Create '>';
        create SCT after;
        Create 'title>\12' After;
        END;

 ISINDEX: BEGIN
#ifdef XML
        { elements TITLE and BASE must be generated as the first chidren
          of the HEAD element }
        Get TITLE Before;
#endif
        create SOT;
        Create 'isindex';
        IF Prompt
           BEGIN
           Create ' prompt="';
           Create Prompt;
           Create DoubleQuote;
           END;
        Create Attributes;
        Remove;
#ifdef XML
        Create ' />\12';
#else
        Create '>\12';
#endif
        END;

 BASE:        BEGIN
#ifdef XML
        { elements TITLE and BASE must be generated as the first chidren
          of the HEAD element }
        Get TITLE Before;
#endif
        create SOT;
        Create 'base ';
        Create Attributes;
        Remove;
#ifdef XML
        Create ' />\12';
#else
        Create '>\12';
#endif
        END;

 STYLE_: BEGIN
#ifdef XML
        { elements TITLE and BASE must be generated as the first chidren
          of the HEAD element }
        Get TITLE Before;
#endif
        create SOT;
        Create 'style';
        IF Notation
           BEGIN
           Create ' type="';
           Create Notation;
           Create DoubleQuote;
           END;
        Create Attributes;
        Create '>\12';
        NoLineBreak;
        NoTranslation;
        Indent Suspend;
        Indent Resume After;
        create SCT after;
        Create 'style>\12' After;
        END;

 SCRIPT_: BEGIN
#ifdef XML
        { elements TITLE and BASE must be generated as the first chidren
          of the HEAD element }
        Get TITLE Before;
#endif
        create SOT;
        Create 'script';
        Create Attributes;
        Create '>\12';
        NoLineBreak;
        NoTranslation;
        Indent Suspend;
        Indent Resume After;
        create SCT after;
        Create 'script>\12' After;
        END;

 META: BEGIN
        create SOT;
        Create 'meta' Before;
        Create Attributes Before;
        Remove;
#ifdef XML
        Create ' />\12' After;
#else
        Create '>\12' After;
#endif
        END;

 LINK:        BEGIN
#ifdef XML
        { elements TITLE and BASE must be generated before }
        Get TITLE Before;
#endif
        create SOT;
        Create 'link';
        Remove;
        Create Attributes;
#ifdef XML
        Create ' />\12';
#else
        Create '>\12';
#endif
        END;

 BODY:        BEGIN
        create SOT;
        Create 'body';
        Create Attributes;
        Create '>\12';
        create SCT after;
        Create 'body>\12' After;
        END;

 H1:        BEGIN
        if not within Preformatted and not first Create '\12';
        create SOT;
        Create 'h1';
        Create Attributes;
        Create '>';
        create SCT after;
        Create 'h1>' After;
        if not within Preformatted Create '\12' After;
        END;

 H2:        BEGIN
        if not within Preformatted and not first Create '\12';
        create SOT;
        Create 'h2';
        Create Attributes;
        Create '>';
        create SCT after;
        Create 'h2>' After;
        if not within Preformatted Create '\12' After;
        END;

 H3:        BEGIN
        if not within Preformatted and not first Create '\12';
        create SOT;
        Create 'h3';
        Create Attributes;
        Create '>';
        create SCT after;
        Create 'h3>' After;
        if not within Preformatted Create '\12' After;
        END;

 H4:        BEGIN
        if not within Preformatted and not first Create '\12';
        create SOT;
        Create 'h4';
        Create Attributes;
        Create '>';
        create SCT after;
        Create 'h4>' After;
        if not within Preformatted Create '\12' After;
        END;

 H5:        BEGIN
        if not within Preformatted and not first Create '\12';
        create SOT;
        Create 'h5';
        Create Attributes;
        Create '>';
        create SCT after;
        Create 'h5>' After;
        if not within Preformatted Create '\12' After;
        END;

 H6:        BEGIN
        if not within Preformatted and not first Create '\12';
        create SOT;
        Create 'h6';
        Create Attributes;
        Create '>';
        create SCT after;
        Create 'h6>' After;
        if not within Preformatted Create '\12' After;
        END;

 BaseFont:
        BEGIN
        create SOT;
        Create 'basefont';
        Remove;
        Create Attributes;
#ifdef XML
        Create ' />\12';
#else
        Create '>\12';
#endif
        END;

 BR:        BEGIN
        create SOT;
        Create 'br';
        Remove;
        Create Attributes;
        if not within Preformatted
#ifdef XML
           Create ' />\12';
#else
           Create '>\12';
#endif
        if within Preformatted
#ifdef XML
           Create ' />';
#else
           Create '>';
#endif
        END;

 Italic_text:
        BEGIN
        create SOT;
        Create 'i';
        Create Attributes;
        Create '>';
        create SCT after;
        Create 'i>' After;
        END;

 Bold_text:
        BEGIN
        create SOT;
        Create 'b';
        Create Attributes;
        Create '>';
        create SCT after;
        Create 'b>' After;
        END;
 Teletype_text:
        BEGIN
        create SOT;
        Create 'tt';
        Create Attributes;
        Create '>';
        create SCT after;
        Create 'tt>' After;
        END;
 Underlined_text:
        BEGIN
        create SOT;
        Create 'u';
        Create Attributes;
        Create '>';
        create SCT after;
        Create 'u>' After;
        END;
 Struck_text:
        BEGIN
        create SOT;
        Create 'strike';
        Create Attributes;
        Create '>';
        create SCT after;
        Create 'strike>' After;
        END;
 Big_text:
        BEGIN
        create SOT;
        Create 'big';
        Create Attributes;
        Create '>';
        create SCT after;
        Create 'big>' After;
        END;
 Small_text:
        BEGIN
        create SOT;
        Create 'small';
        Create Attributes;
        Create '>';
        create SCT after;
        Create 'small>' After;
        END;
 Subscript:
        BEGIN
        create SOT;
        Create 'sub';
        Create Attributes;
        Create '>';
        create SCT after;
        Create 'sub>' After;
        END;
 Superscript:
        BEGIN
        create SOT;
        Create 'sup';
        Create Attributes;
        Create '>';
        create SCT after;
        Create 'sup>' After;
        END;

 Emphasis:
        BEGIN
        create SOT;
        Create 'em';
        Create Attributes;
        Create '>';
        create SCT after;
        Create 'em>' After;
        END;
 Strong:
        BEGIN
        create SOT;
        Create 'strong';
        Create Attributes;
        Create '>';
        create SCT after;
        Create 'strong>' After;
        END;
 Def:
        BEGIN
        create SOT;
        Create 'dfn';
        Create Attributes;
        Create '>';
        create SCT after;
        Create 'dfn>' After;
        END;
 Code:
        BEGIN
        create SOT;
        Create 'code';
        Create Attributes;
        Create '>';
        create SCT after;
        Create 'code>' After;
        END;
 Sample:
        BEGIN
        create SOT;
        Create 'samp';
        Create Attributes;
        Create '>';
        create SCT after;
        Create 'samp>' After;
        END;
 Keyboard:
        BEGIN
        create SOT;
        Create 'kbd';
        Create Attributes;
        Create '>';
        create SCT after;
        Create 'kbd>' After;
        END;
 Variable_:
        BEGIN
        create SOT;
        Create 'var';
        Create Attributes;
        Create '>';
        create SCT after;
        Create 'var>' After;
        END;
 Cite:
        BEGIN
        create SOT;
        Create 'cite';
        Create Attributes;
        Create '>';
        create SCT after;
        Create 'cite>' After;
        END;
 ABBR:
        BEGIN
        create SOT;
        Create 'abbr';
        Create Attributes;
        Create '>';
        create SCT after;
        Create 'abbr>' After;
        END;
 ACRONYM:
        BEGIN
        create SOT;
        Create 'acronym';
        Create Attributes;
        Create '>';
        create SCT after;
        Create 'acronym>' After;
        END;

 Font_: BEGIN
        create SOT;
        Create 'font';
        Create Attributes;
        Create '>';
        create SCT after;
        Create 'font>' After;
        END;

 Applet: BEGIN
        create SOT;
        Create 'applet';
        Create Attributes;
        Create '>\12';
        Indent +2;
        Indent -2 after;
        Create '\12' after;
        create SCT after;
        Create 'applet>\12' after;        
        END;

 Parameter: BEGIN
        create SOT;
        Create 'param';
        Create Attributes;
        Remove;
#ifdef XML
        Create ' />\12';
#else
        Create '>\12';
#endif
        END;

 Object: BEGIN
        create SOT;
        Create 'object';
        Create Attributes;
        Create '>\12';
        Indent +2;
        Indent -2 After;
        create SCT after;
        Create 'object>' After;        
        END;

 IMG: BEGIN
        create SOT;
        Create 'img';
        Create Attributes;
        if empty
#ifdef XML
                Create ' />';
#else
                Create '>';
#endif
        if not empty
                BEGIN
        Create '>';
        Indent +2;
        Indent -2 After;
        create SCT after;
        Create 'img>' After;
                END;
        END;

 Image_Input: BEGIN
        create SOT;
        Create 'input';
        Create Attributes;
        if empty
          Create ' />';
        if not empty
                BEGIN
        Create '>';
        Indent +2;
        Indent -2 After;
        create SCT after;
        Create 'img>' After;
                END;
        END;

 INS:        BEGIN
        create SOT;
        Create 'ins';
        Create Attributes;
        Create '>';
        create SCT after;
        Create 'ins>' After;
        END;

 DEL:        BEGIN
        create SOT;
        Create 'del';
        Create Attributes;
        Create '>';
        create SCT after;
        Create 'del>' After;
        END;

 ins:        BEGIN
        create SOT;
        Create 'ins';
        Create Attributes;
        Create '>';
        create SCT after;
        Create 'ins>' After;
        END;

 del:        BEGIN
        create SOT;
        Create 'del';
        Create Attributes;
        Create '>';
        create SCT after;
        Create 'del>' After;
        END;

 Span:        BEGIN
        create SOT;
        Create 'span';
        Create Attributes;
        Create '>';
        create SCT after;
        Create 'span>' After;
        END;

 Quotation: BEGIN
        create SOT;
        Create 'q';
        Create Attributes;
        Create '>';
        create SCT after;
        Create 'q>' After;
        END;

 BDO: BEGIN
        create SOT;
        Create 'bdo';
        Create Attributes;
        Create '>';
        create SCT after;
        Create 'bdo>' After;
        END;

 Paragraph: BEGIN
        if not within List_Item and not within Definition and not first
                Create '\12';
        create SOT;
        Create 'p';
        Create Attributes;
        Create '>';
        create SCT after;
        Create 'p>\12' After;
        END;

 Pseudo_paragraph: BEGIN
        if not last Create '\12' After;
        END;

 TEXT_UNIT: BEGIN
        IF Within Text_Input
                Remove;
        END;

 Definition_List: BEGIN
        create SOT;
        Create 'dl';
        Create Attributes;
        Create '>\12';
        Indent +2;
        Indent -2 After;
        create SCT after;
        Create 'dl>\12' After;
        END;

 Term:        BEGIN
        create SOT;
        Create 'dt';
        Create Attributes;
        Create '>';
        create SCT after;
        Create 'dt>\12' After;
        END;

 Definition: BEGIN
        Indent +2;
        create SOT;
        Create 'dd';
        Create Attributes;
        Create '>';
        Indent +2;
        Indent -2 After;
        create SCT after;
        Create 'dd>\12' After;
        Indent -2 After;
        END;

 Unnumbered_List: BEGIN
        create SOT;
        Create 'ul';
        Create Attributes;
        Create '>\12';
        Indent +2;
        Indent -2 After;
        create SCT after;
        Create 'ul>\12' After;
        END;

 Numbered_List:        BEGIN
        create SOT;
        Create 'ol';
        Create Attributes;
        Create '>\12';
        Indent +2;
        Indent -2 After;
        create SCT after;
        Create 'ol>\12' After;
        END;

 Address: BEGIN
        create SOT;
        Create 'address';
        Create Attributes;
        Create '>\12';
        Indent +2;
        Indent -2 After;
        Create '\12' After;
        create SCT after;
        Create 'address>\12' After;
        END;

 Menu:        BEGIN
        create SOT;
        Create 'menu';
        Create Attributes;
        Create '>\12';
        Indent +2;
        Indent -2 After;
        create SCT after;
        Create 'menu>\12' After;
        END;

 Directory: BEGIN
        create SOT;
        Create 'dir';
        Create Attributes;
        Create '>\12';
        Indent +2;
        Indent -2 After;
        create SCT after;
        Create 'dir>\12' After;
        END;

 List_Item: BEGIN
        create SOT;
        Create 'li';
        Create Attributes;
        Create '>';
        Indent +2;
        Indent -2 After;
        create SCT after;
        Create 'li>\12' After;
        END;

 Preformatted: BEGIN
        NoLineBreak;
        create SOT;
        Create 'pre';
        Create Attributes;
        Create '>';
        Indent Suspend;
        Indent Resume After;
        create SCT after;
        Create 'pre>\12' After;
        END;

 Horizontal_Rule: BEGIN
        create SOT;
        Create 'hr';
        Create Attributes;
        Remove;
#ifdef XML
        Create ' />\12';
#else
        Create '>\12';
#endif
        END;

 Form: BEGIN
        Create '\12';
        create SOT;
        Create 'form';
        Create Attributes;
        Create '>\12';
        Indent +2;
        Indent -2 After;
        create SCT after;
        Create 'form>\12' After;
        END;

 Option_Menu: BEGIN
        Create '\12';
        create SOT;
        Create 'select';
        Create Attributes;
        Create '>';
        Indent +2;
        Indent -2 After;
        Create '\12' after;
        create SCT after;
        Create 'select>\12' After;
        END;

 Option: BEGIN
        Create '\12';
        create SOT;
        Create 'option';
        Create Attributes;
        Create '>';
        create SCT after;
        Create 'option>' After;
        END;

 OptGroup: BEGIN
        Create '\12';
        create SOT;
        Create 'optgroup';
        Create Attributes;
        Create '>';
        Indent +2;
        Indent -2 After;
        Create '\12' after;
        create SCT after;
        Create 'optgroup>' After;
        END;

 Text_Area: BEGIN
        create SOT;
        Create 'textarea';
        Create Attributes;
        Create '>';
        NoLineBreak;
        create SCT after;
        Create 'textarea>' After;
        END;

 FIELDSET: BEGIN
        create SOT;
        Create 'fieldset';
        Create Attributes;
        Create '>\12';
        Indent +2;
        Indent -2 After;
        create SCT after;
        Create 'fieldset>\12' After;
        END;

 LEGEND: BEGIN
        create SOT;
        Create 'legend';
        Create Attributes;
        Create '>';
        create SCT after;
        Create 'legend>\12' After;
        END;

 LABEL: BEGIN
        create SOT;
        Create 'label';
        Create Attributes;
        Create '>';
        create SCT after;
        Create 'label>' After;
        END;

 BUTTON_: BEGIN
        create SOT;
        Create 'button';
        Create Attributes;
        Create '>';
        create SCT after;
        Create 'button>' After;
        END;

 Checkbox_Input: BEGIN
        create SOT;
        Create 'input';
        Create Attributes;
        Remove;
#ifdef XML
        Create ' />';
#else
        Create '>';
#endif
        END;

 Radio_Input: BEGIN
        create SOT;
        Create 'input';
        Create Attributes;
        Remove;
#ifdef XML
        Create ' />';
#else
        Create '>';
#endif
        END;

 Text_Input: BEGIN
        create SOT;
        Create 'input';
        Create Attributes;
        Remove;
#ifdef XML
        Create ' />';
#else
        Create '>';
#endif
        END;

 Hidden_Input: BEGIN
        create SOT;
        Create 'input';
        Create Attributes;
        Remove;
#ifdef XML
        Create ' />';
#else
        Create '>';
#endif
        END;

 Password_Input: BEGIN
        create SOT;
        Create 'input';
        Create Attributes;
        Remove;
#ifdef XML
        Create ' />';
#else
        Create '>';
#endif
        END;

 File_Input: BEGIN
        create SOT;
        Create 'input';
        Create Attributes;
        Remove;
#ifdef XML
        Create ' />';
#else
        Create '>';
#endif
        END;

 Reset_Input: BEGIN
        Create '\12';
        create SOT;
        Create 'input';
        Create Attributes;
        Remove;
#ifdef XML
        Create ' />';
#else
        Create '>';
#endif
        END;

 Submit_Input: BEGIN
        Create '\12';
        create SOT;
        Create 'input';
        Create Attributes;
        Remove;
#ifdef XML
        Create ' />';
#else
        Create '>';
#endif
        END;

 Button_Input: BEGIN
        Create '\12';
        create SOT;
        Create 'input';
        Create Attributes;
        Remove;
#ifdef XML
        Create ' />';
#else
        Create '>';
#endif
        END;

 Block_Quote: BEGIN
        Create '\12';
        create SOT;
        Create 'blockquote';
        Create Attributes;
        Create '>\12';
        Indent +2;
        Indent -2 After;
        create SCT after;
        Create 'blockquote>\12' After;
        END;

 PICTURE_UNIT: BEGIN
        Remove;
        END;

 External_Object: BEGIN
        Remove;
        END;

 External_Object_Content: BEGIN
        Remove;
        END;

 Embed_: BEGIN
        if NOT Immediately within Object BEGIN
          create SOT;
                Create 'embed';
                Create Attributes;
                Create ' />';
                END;
        END;

 Embed_Content: BEGIN
        Remove;
        END;

 Anchor: BEGIN
         create SOT;
         Create 'a';
         Create Attributes;
         Create '>';
           create SCT after;
         Create 'a>' After;
         END;

 MAP:
        BEGIN
        create SOT;
        Create 'map';
        Create Attributes;
        Create '>\12';
        Indent +2;
        Indent -2 After;
          create SCT after;
        Create 'map>\12' After;
        END;

 map:
        BEGIN
        create SOT;
        Create 'map';
        Create Attributes;
        Create '>\12';
        Indent +2;
        Indent -2 After;
          create SCT after;
        Create 'map>\12' After;
        END;

 AREA:
        BEGIN
        create SOT;
        Create 'area';
        Create Attributes;
        Remove;
#ifdef XML
        Create ' />\12';
#else
        Create '>\12';
#endif
        END;

{ ---- Tables ---- }

 Table_:
        BEGIN
        Create '\12';
        create SOT;
        Create 'table';
        Create Attributes;
        Create '>\12';
        Indent +2;
        Indent -2 after;
          create SCT after;
        Create 'table>\12' after;
        END;

 CAPTION:
        BEGIN
        create SOT;
        Create 'caption';
        Create Attributes;
        Create '>';
          create SCT after;
        Create 'caption>\12' after;
        END;

 COLGROUP:
        BEGIN
        create SOT;
        Create 'colgroup';
        Create Attributes;
        Create '>';
        Indent +2;
        Indent -2 after;
          create SCT after;
        Create 'colgroup>\12' after;
        END;

 COL:
        BEGIN
        create SOT;
        Create 'col';
        Create Attributes;
        Remove;
#ifdef XML
        Create ' />\12';
#else
        Create '>\12';
#endif
        END;

 Table_head:
        Remove;

 thead:
        BEGIN
        create SOT;
        Create 'thead';
        Create Attributes;
        Create '>\12';
        Indent +2;
        Indent -2 after;
          create SCT after;
        Create 'thead>\12' after;
        END;

 tfoot:
        BEGIN
        create SOT;
        Create 'tfoot';
        Create Attributes;
        Create '>\12';
        Indent +2;
        Indent -2 after;
          create SCT after;
        Create 'tfoot>\12' after;
        END;

 tbody:
        BEGIN
        create SOT;
        Create 'tbody';
        Create Attributes;
        Create '>\12';
        Indent +2;
        Indent -2 after;
          create SCT after;
        Create 'tbody>\12' after;
        END;

 Table_body:
        Get tfoot;

 Table_row:
        BEGIN
        create SOT;
        Create 'tr';
        Create Attributes;
        Create '>\12';
        Indent +2;
        Indent -2 after;
          create SCT after;
        Create 'tr>\12' after;
        END;

 Data_cell:
        BEGIN
        create SOT;
        Create 'td';
        Create Attributes;
        Create '>';
        Indent +2;
        Indent -2 after;
          create SCT after;
        Create 'td>\12' after;
        END;

 Heading_cell:
        BEGIN
        create SOT;
        Create 'th';
        Create Attributes;
        Create '>';
        Indent +2;
        Indent -2 after;
          create SCT after;
        Create 'th>\12' after;
        END;

 Division: BEGIN
        Create '\12';
        create SOT;
        Create 'div';
        Create Attributes;
        Create '>\12';
          create SCT after;
        Create 'div>\12' After;
        END;

 Center: BEGIN
        Create '\12';
        create SOT;
        Create 'center';
        Create Attributes;
        Create '>\12';
          create SCT after;
        Create 'center>\12' After;
        END;

 NOSCRIPT: BEGIN
        Create '\12';
        create SOT;
        Create 'noscript';
        Create Attributes;
        Create '>\12';
          create SCT after;
        Create 'noscript>\12' After;
        END;

 Invalid_element:
        BEGIN
        Remove;
        END;

 Unknown_namespace:
        BEGIN
           NoTranslation;
        END;

 XHTML_Unknown_namespace:
        BEGIN
           NoTranslation;
           Create '\12';
           Create Content;
           Remove;
           Create Attributes;
           Create '>';
        END;

 Comment\240: BEGIN
        Create '<!--';
        NoLineBreak;
        {NoTranslation;}
        Create '-->' after;
	if not within ParagEquiv and not within SCRIPT_
           Create '\12' after;
        END;

 Comment_line: BEGIN
        if not last create '\12' after;
        END;

 ASP_element: BEGIN
        Create '<%';
        NoLineBreak;
        {NoTranslation;}
        Create '>\12' after;
        END;

 ASP_line: BEGIN
        if not last create '\12' after;
        END;

 XMLPI: BEGIN
        Create '<?';
        NoLineBreak;
        {NoTranslation;}
        IF Within Paragraph
           Create '?>' after;
        IF Within List_Item
           Create '?>' after;
        IF Within TITLE
           Create '?>' after;
        IF not Within Paragraph and not Within List_Item and not Within TITLE
           Create '?>\12' after;
        END;

 PI_line: BEGIN
        if not last create '\12' after;
        END;

 DOCTYPE: BEGIN
        NoLineBreak;
        NoTranslation;
        Create '\12' after;
        END;

 DOCTYPE_line:
        if not last create '\12' after;

 CDATA: BEGIN
        Create '<![CDATA[';
        NoLineBreak;
        NoTranslation;
        Create ']]>' After;
        END;

 CDATA_line:
        if not last create '\12' after;

 FRAMESET: BEGIN
        create SOT;
        Create 'frameset';
        Create Attributes;
        Create '>\12';
        indent +2;
        indent -2 after;
        create SCT after;
        Create 'frameset>\12' After;
        END;

 Frames: BEGIN
        END;

 NOFRAMES: BEGIN
        create SOT;
        Create 'noframes';
        Create Attributes;
        Create '>\12';
        indent +2;
        indent -2 after;
        create SCT after;
        Create 'noframes>\12' After;
        END;

 FRAME: BEGIN
        create SOT;
        Create 'frame';
        Create Attributes;
        Remove;
#ifdef XML
        Create ' />\12';
#else
        Create '>\12';
#endif
        END;

 IFRAME: BEGIN
        create SOT;
        Create 'iframe';
        Create Attributes;
        Create '>\12';
        Indent +2;
        Indent -2;
        create SCT after;
        Create 'iframe>\12' After;        
        END;

 Iframe_Src_Content: BEGIN
        Remove;
        END;

 ruby: BEGIN
        create SOT;
        Create 'ruby';
        Create Attributes;
        Create '>';
        create SCT after;
        Create 'ruby>' After;        
        END;

 simple_ruby: BEGIN
        create SOT;
        Create 'ruby';
        Create Attributes;
        Create '>';
        create SCT after;
        Create 'ruby>' After;        
        END;

 complex_ruby: BEGIN
        create SOT;
        Create 'ruby';
        Create Attributes;
        Create '>';
        create SCT after;
        Create 'ruby>' After;        
        END;

 rb:    BEGIN
        create SOT;
        Create 'rb';
        Create Attributes;
        Create '>';
        create SCT after;
        Create 'rb>' After;        
        END;

 rt:        BEGIN
        create SOT;
        Create 'rt';
        Create Attributes;
        Create '>';
        create SCT after;
        Create 'rt>' After;        
        END;

 rp:        BEGIN
        create SOT;
        Create 'rp';
        Create Attributes;
        Create '>';
        create SCT after;
        Create 'rp>' After;        
        END;

 rbc:        BEGIN
        create SOT;
        Create 'rbc';
        Create Attributes;
        Create '>';
        create SCT after;
        Create 'rbc>' After;        
        END;

 rtc1:        BEGIN
        create SOT;
        Create 'rtc';
        Create Attributes;
        Create '>';
        create SCT after;
        Create 'rtc>' After;        
        END;

 rtc2:        BEGIN
        create SOT;
        Create 'rtc';
        Create Attributes;
        Create '>';
        create SCT after;
        Create 'rtc>' After;        
        END;

ATTRIBUTES

 profile: BEGIN
  Create SAN;
        Create 'profile="';
        Create profile;
        Create DoubleQuote;
        END;

 Language:
        BEGIN
        IF not Root
            BEGIN
#ifdef XML
            Create ' xml:lang="';
            Create Language;
            Create DoubleQuote;
#endif
#ifndef XHTML11
            Create ' lang="';
            Create Language;
            Create DoubleQuote;
#endif
            END;
        IF Root and RealLang
            BEGIN
#ifdef XML
            Create ' xml:lang="';
            Create Language;
            Create DoubleQuote;
#endif
#ifndef XHTML11
            Create ' lang="';
            Create Language;
            Create DoubleQuote;
#endif
            END;
        END;

 content_type: BEGIN
  Create SAN;
        Create 'type="';
        Create content_type;
        Create DoubleQuote;
        END;

 script_src: BEGIN
  Create SAN;
        Create 'src="';
        Create script_src;
        Create DoubleQuote;
        END;

 script_language: BEGIN
        Create ' language="';
        Create script_language;
        Create DoubleQuote;
        END;

 defer: BEGIN
  Create SAN;
        Create 'defer="defer"';
        END;

 event: BEGIN
  Create SAN;
        Create 'event="';
        Create event;
        Create DoubleQuote;
        END;

 for_: BEGIN
  Create SAN;
        Create 'for="';
        Create for_;
        Create DoubleQuote;
        END;

 http_equiv: BEGIN
  Create SAN;
        Create 'http-equiv="';
        Create http_equiv;
        Create DoubleQuote;
        END;

 meta_name: BEGIN
  Create SAN;
        Create 'name="';
        Create meta_name;
        Create DoubleQuote;
        END;

 meta_content: BEGIN
  Create SAN;
        Create 'content="';
        Create Translated meta_content;
        Create DoubleQuote;
        END;

 scheme: BEGIN
  Create SAN;
        Create 'scheme="';
        Create scheme;
        Create DoubleQuote;
        END;

 charset: BEGIN
  Create SAN;
        Create 'charset="';
        Create charset;
        Create DoubleQuote;
        END;

 REL:        BEGIN
  Create SAN;
        Create 'rel="';
        Create REL;
        Create DoubleQuote;
        END;

 REV:        BEGIN
  Create SAN;
        Create 'rev="';
        Create REV;
        Create DoubleQuote;
        END;

 HREF_ :
        BEGIN
  Create SAN;
        Create 'href="';
        Create Translated HREF_;
        Create DoubleQuote;
        END;

 hreflang :
        BEGIN
  Create SAN;
        Create 'hreflang="';
        Create hreflang;
        Create DoubleQuote;
        END;

 media :
        BEGIN
  Create SAN;
        Create 'media="';
        Create media;
        Create DoubleQuote;
        END;

 target_ :
        BEGIN
  Create SAN;
        Create 'target="';
        Create Translated target_;
        Create DoubleQuote;
        END;

 cite:
        BEGIN
  Create SAN;
        Create 'cite="';
        Create Translated cite;
        Create DoubleQuote;
        END;

 datetime:
        BEGIN
  Create SAN;
        Create 'datetime="';
        Create datetime;
        Create DoubleQuote;
        END;

 background_ :
        BEGIN
  Create SAN;
        Create 'background="';
        Create Translated background_;
        Create DoubleQuote;
        END;

 BackgroundColor :
        BEGIN
  Create SAN;
        Create 'bgcolor="';
        Create BackgroundColor;
        Create DoubleQuote;
        END;

 TextColor :
        BEGIN
  Create SAN;
        Create 'text="';
        Create TextColor;
        Create DoubleQuote;
        END;

 LinkColor :
        BEGIN
  Create SAN;
        Create 'link="';
        Create LinkColor;
        Create DoubleQuote;
        END;

 VisitedLinkColor :
        BEGIN
  Create SAN;
        Create 'vlink="';
        Create VisitedLinkColor;
        Create DoubleQuote;
        END;

 ActiveLinkColor :
        BEGIN
  Create SAN;
        Create 'alink="';
        Create ActiveLinkColor;
        Create DoubleQuote;
        END;

 BaseFontSize:
        BEGIN
  create SAN;
        Create 'size="';
        Create BaseFontSize;
        Create DoubleQuote;
        END;

 BaseFontColor:
        BEGIN
  Create SAN;
        Create 'color="';
        Create BaseFontColor;
        Create DoubleQuote;
        END;

 BaseFontFace:
        BEGIN
  Create SAN;
        Create 'face="';
        Create BaseFontFace;
        Create DoubleQuote;
        END;

 Font_size :
        BEGIN
  Create SAN;
        Create 'size="';
        Create Font_size;
        Create DoubleQuote;
        END;

 color :
        BEGIN
  Create SAN;
        Create 'color="';
        Create color;
        Create DoubleQuote;
        END;

 face :
        BEGIN
  Create SAN;
        Create 'face="';
        Create face;
        Create DoubleQuote;
        END;

 Clear_ = Left_:
        BEGIN
  Create SAN;
        Create 'clear="left"';
        END;
 Clear_ = Right_:
        BEGIN
  Create SAN;
        Create 'clear="right"';
        END;
 Clear_ = All_:
        BEGIN
  Create SAN;
        Create 'clear="all"';
        END;
 Clear_ = None_:
        BEGIN
  Create SAN;
        Create 'clear="none"';
        END;

 Align = left_:
        BEGIN
  Create SAN;
        Create 'align="left"';
        END;
 Align = center_:
        BEGIN
  Create SAN;
        Create 'align="center"';
        END;
 Align = right_:
        BEGIN
  Create SAN;
        Create 'align="right"';
        END;

 TextAlign = left_:
        BEGIN
  Create SAN;
        Create 'align="left"';
        END;
 TextAlign = center_:
        BEGIN
  Create SAN;
        Create 'align="center"';
        END;
 TextAlign = right_:
        BEGIN
  create SAN;
        Create 'align="right"';
        END;
 TextAlign = justify_:
        BEGIN
  Create SAN;
        Create 'align="justify"';
        END;

 NoShade:
        BEGIN
  create SAN;
        Create 'noshade="noshade"';
        END;

 Size_: BEGIN
  Create SAN;
        Create 'size="';
        Create Size_;
        Create DoubleQuote;
        END;

 Width__: BEGIN
  create SAN;
        Create 'width="';
        Create Width__;
        Create DoubleQuote;
        END;

 declare:
        BEGIN
  create SAN;
        Create 'declare="declare"';
        END;

 codebase: BEGIN
  Create SAN;
        Create 'codebase="';
        Create codebase;
        Create DoubleQuote;
        END;

 code:        BEGIN
  Create SAN;
        Create 'code="';
        Create code;
        Create DoubleQuote;
        END;

 object: BEGIN
  Create SAN;
        Create 'object="';
        Create object;
        Create DoubleQuote;
        END;

 applet_name: BEGIN
  Create SAN;
        Create 'name="';
        Create applet_name;
        Create DoubleQuote;
        END;

 Param_name: BEGIN
  Create SAN;
        Create 'name="';
        Create Param_name;
        Create DoubleQuote;
        END;

 Param_value: BEGIN
  Create SAN;
        Create 'value="';
        Create Param_value;
        Create DoubleQuote;
        END;

 valuetype = data_:
        BEGIN
  Create SAN;
        Create 'valuetype="data"';
        END;
 valuetype = ref:
        BEGIN
  Create SAN;
        Create 'valuetype="ref"';
        END;
 valuetype = object_:
        BEGIN
  Create SAN;
        Create 'valuetype="object"';
        END;

 Param_type: BEGIN
  Create SAN;
        Create 'type="';
        Create Param_type;
        Create DoubleQuote;
        END;

 classid: BEGIN
  Create SAN;
        Create 'classid="';
        Create classid;
        Create DoubleQuote;
        END;

 Object_type: BEGIN
  Create SAN;
        Create 'type="';
        Create Object_type;
        Create DoubleQuote;
        END;

 Embed_type: BEGIN
  Create SAN;
        Create 'type="';
        Create Embed_type;
        Create DoubleQuote;
        END;

 pluginspage: BEGIN
  Create SAN;
        Create 'pluginspage="';
        Create pluginspage;
        Create DoubleQuote;
        END;

 pluginurl: BEGIN
  Create SAN;
        Create 'pluginurl="';
        Create pluginurl;
        Create DoubleQuote;
        END;

 Link_type: BEGIN
  Create SAN;
        Create 'type="';
        Create Link_type;
        Create DoubleQuote;
        END;

 codetype: BEGIN
  Create SAN;
        Create 'codetype="';
        Create codetype;
        Create DoubleQuote;
        END;

 archive: BEGIN
  Create SAN;
        Create 'archive="';
        Create archive;
        Create DoubleQuote;
        END;

 standby: BEGIN
  Create SAN;
        Create 'standby="';
        Create standby;
        Create DoubleQuote;
        END;

 Title : BEGIN
  Create SAN;
        Create 'title="';
        Create Translated Title;
        Create DoubleQuote;
        END;

 accesskey: BEGIN
  Create SAN;
        Create 'accesskey="';
        Create accesskey;
        Create DoubleQuote;
        END;

 tabindex: BEGIN
  Create SAN;
        Create 'tabindex="';
        Create tabindex;
        Create DoubleQuote;
        END;

 Associated_control: BEGIN
  Create SAN;
        Create 'for="';
        Create Associated_control;
        Create DoubleQuote;
        END;

 Button_type = button:
        BEGIN
  Create SAN;
        Create 'type="button"';
        END;
 Button_type = submit:
        BEGIN
  Create SAN;
        Create 'type="submit"';
        END;
 Button_type = reset:
        BEGIN
  Create SAN;
        Create 'type="reset"';
        END;

 Class: BEGIN
  Create SAN;
        Create 'class="';
        Create Class;
        Create DoubleQuote;
        END;

 Style\240: BEGIN
        IF not Within HEAD
            BEGIN
      Create SAN;
            Create 'style="';
            Create Style\240;
            Create DoubleQuote;
            END;
        END;

 ID:        BEGIN
  Create SAN;
        Create 'id="';
        Create ID;
        Create DoubleQuote;
        END;

 dir = ltr_ :
        if not Document
            BEGIN
      Create SAN;
            Create 'dir="ltr"';
            END;
 dir = rtl_ :
        if not Document
            BEGIN
      Create SAN;
            Create 'dir="rtl"';
            END;

 onclick: BEGIN
  Create SAN;
        Create 'onclick="';
        Create onclick;
        Create DoubleQuote;
        END;

 ondblclick: BEGIN
  Create SAN;
        Create 'ondblclick="';
        Create ondblclick;
        Create DoubleQuote;
        END;

 onmousedown: BEGIN
  Create SAN;
        Create 'onmousedown="';
        Create onmousedown;
        Create DoubleQuote;
        END;

 onmouseup: BEGIN
  Create SAN;
        Create 'onmouseup="';
        Create onmouseup;
        Create DoubleQuote;
        END;

 onmouseover: BEGIN
  Create SAN;
        Create 'onmouseover="';
        Create onmouseover;
        Create DoubleQuote;
        END;

 onmousemove: BEGIN
  Create SAN;
        Create 'onmousemove="';
        Create onmousemove;
        Create DoubleQuote;
        END;

 onmouseout: BEGIN
  Create SAN;
        Create 'onmouseout="';
        Create onmouseout;
        Create DoubleQuote;
        END;

 onkeypress: BEGIN
  Create SAN;
        Create 'onkeypress="';
        Create onkeypress;
        Create DoubleQuote;
        END;

 onkeydown: BEGIN
  Create SAN;
        Create 'onkeydown="';
        Create onkeydown;
        Create DoubleQuote;
        END;

 onkeyup: BEGIN
  Create SAN;
        Create 'onkeyup="';
        Create onkeyup;
        Create DoubleQuote;
        END;

 onload: BEGIN
  Create SAN;
        Create 'onload="';
        Create onload;
        Create DoubleQuote;
        END;

 onunload: BEGIN
  Create SAN;
        Create 'onunload="';
        Create onunload;
        Create DoubleQuote;
        END;

 onfocus: BEGIN
  Create SAN;
        Create 'onfocus="';
        Create onfocus;
        Create DoubleQuote;
        END;

 onblur: BEGIN
  Create SAN;
        Create 'onblur="';
        Create onblur;
        Create DoubleQuote;
        END;

 onsubmit: BEGIN
  Create SAN;
        Create 'onsubmit="';
        Create onsubmit;
        Create DoubleQuote;
        END;

 onreset: BEGIN
  Create SAN;
        Create 'onreset="';
        Create onreset;
        Create DoubleQuote;
        END;

 onselect: BEGIN
  Create SAN;
        Create 'onselect="';
        Create onselect;
        Create DoubleQuote;
        END;

 onchange: BEGIN
  Create SAN;
        Create 'onchange="';
        Create onchange;
        Create DoubleQuote;
        END;

 BulletStyle = disc_ :
        BEGIN
  Create SAN;
        Create 'type="disc"';
        END;
 BulletStyle = square_ :
        BEGIN
  Create SAN;
        Create 'type="square"';
        END;
 BulletStyle = circle_ :
        BEGIN
  Create SAN;
        Create 'type="circle"';
        END;

 COMPACT:
        BEGIN
  Create SAN;
        Create 'compact="compact"';
        END;

 NumberStyle = Arabic_ :
        BEGIN
  Create SAN;
        Create 'type="1"';
        END;
 NumberStyle = LowerAlpha :
        BEGIN
  Create SAN;
        Create 'type="a"';
        END;
 NumberStyle = UpperAlpha :
        BEGIN
  Create SAN;
        Create 'type="A"';
        END;
 NumberStyle = LowerRoman_ :
        BEGIN
  Create SAN;
        Create 'type="i"';
        END;
 NumberStyle = UpperRoman_ :
        BEGIN
  Create SAN;
        Create 'type="I"';
        END;

 Start > 1: BEGIN
  Create SAN;
        Create 'start="';
        Create Start;
        Create DoubleQuote;
        END;

 ItemStyle = disc_ :
        BEGIN
  Create SAN;
        Create 'type="disc"';
        END;
 ItemStyle = square_ :
        BEGIN
  Create SAN;
        Create 'type="square"';
        END;
 ItemStyle = circle_ :
        BEGIN
  Create SAN;
        Create 'type="circle"';
        END;
 ItemStyle = Arabic_ :
        BEGIN
  Create SAN;
        Create 'type="1"';
        END;
 ItemStyle = LowerAlpha :
        BEGIN
  Create SAN;
        Create 'type="a"';
        END;
 ItemStyle = UpperAlpha :
        BEGIN
  Create SAN;
        Create 'type="A"';
        END;
 ItemStyle = LowerRoman_ :
        BEGIN
  Create SAN;
        Create 'type="i"';
        END;
 ItemStyle = UpperRoman_ :
        BEGIN
  Create SAN;
        Create 'type="I"';
        END;

 ItemValue: BEGIN
  Create SAN;
        Create 'value="';
        Create ItemValue;
        Create DoubleQuote;
        END;

 Script_URL: BEGIN
  Create SAN;
        Create 'action="';
        Create Script_URL;
        Create DoubleQuote;
        END;

 METHOD=Post_:
        BEGIN
  Create SAN;
        Create 'method="post"';
        END;

 METHOD=Get_:
        BEGIN
  Create SAN;
        Create 'method="get"';
        END;

 ENCTYPE: BEGIN
  Create SAN;
        Create 'enctype="';
        Create ENCTYPE;
        Create DoubleQuote;
        END;

 accept_charset: BEGIN
  Create SAN;
        Create 'accept-charset="';
        Create accept_charset;
        Create DoubleQuote;
        END;

 NAME: BEGIN
  Create SAN;
        Create 'name="';
  Create NAME;
  Create DoubleQuote;
#ifdef XHTML11
        if map and not ID BEGIN
          Create SAN;
                       Create 'id="';
                    Create NAME;
                       Create DoubleQuote;
                       END;
        if MAP and not ID BEGIN
          Create SAN;
                       Create 'id="';
                    Create NAME;
                       Create DoubleQuote;
                       END;
        if Anchor and not ID BEGIN
          Create SAN;
                       Create 'id="';
                    Create NAME;
                       Create DoubleQuote;
                       END;
#endif
        END;

 Multiple: BEGIN
  Create SAN;
        Create 'multiple="multiple"';
        END;

 MenuSize: BEGIN
  Create SAN;
        Create 'size="';
        Create MenuSize;
        Create DoubleQuote;
        END;

 Selected: BEGIN
  Create SAN;
        Create 'selected="selected"';
        END;

 Value_: BEGIN
  Create SAN;
        Create 'value="';
        Create Value_;
        Create DoubleQuote;
        END;

 Rows: BEGIN
  Create SAN;
        Create 'rows="';
        Create Rows;
        Create DoubleQuote;
        END;

 Columns: BEGIN
  Create SAN;
        Create 'cols="';
        Create Columns;
        Create DoubleQuote;
        END;

 Checked = Yes_: BEGIN
  Create SAN;
        Create 'checked="checked"';
        END;

 disabled: BEGIN
  Create SAN;
        Create 'disabled="disabled"';
        END;

 readonly: BEGIN
  Create SAN;
        Create 'readonly="readonly"';
        END;

 label: BEGIN
  Create SAN;
        Create 'label="';
        Create label;
        Create DoubleQuote;
        END;

 accept: BEGIN
  Create SAN;
        Create 'accept="';
        Create accept;
        Create DoubleQuote;
        END;

 LAlign = Top_: BEGIN
  Create SAN;
        Create 'align="top"';
        END;
 LAlign = Bottom_: BEGIN
  Create SAN;
        Create 'align="bottom"';
        END;
 LAlign = Left_: BEGIN
  Create SAN;
        Create 'align="left"';
        END;
 LAlign = Right_: BEGIN
  Create SAN;
        Create 'align="right"';
        END;

 Area_Size: BEGIN
  Create SAN;
        Create 'size="';
        Create Area_Size;
        Create DoubleQuote;
        END;

 MaxLength: BEGIN
  Create SAN;
        Create 'maxlength="';
        Create MaxLength;
        Create DoubleQuote;
        END;

 data:        BEGIN
     create SAN;
           Create 'data="';
           Create Translated data;
           Create DoubleQuote;
        END;

 SRC:        BEGIN
        if not Immediately within Object and not Immediately within IMG
     and not Immediately within Image_Input
           BEGIN
     Create SAN;
           Create 'src="';
           Create Translated SRC;
           Create DoubleQuote;
           END;
        END;
 
 type:        BEGIN
        if not Immediately within Object
           BEGIN
     create SAN;
           Create 'type="';
           Create Translated type;
           Create DoubleQuote;
           END;
        END;
 
 ALT:        BEGIN
        if not Immediately within Object
           BEGIN
     create SAN;
           Create 'alt="';
           Create Translated ALT;
           Create DoubleQuote;
           END;
        END;

 longdesc: BEGIN
  Create SAN;
        Create 'longdesc="';
        Create longdesc;
        Create DoubleQuote;
        END;

 Alignment = Top_: BEGIN
  Create SAN;
        Create 'align="top"';
        END;
 Alignment = Middle_: BEGIN
  Create SAN;
        Create 'align="middle"';
        END;
 Alignment = Bottom_: BEGIN
  Create SAN;
        Create 'align="bottom"';
        END;
 Alignment = Left_: BEGIN
  Create SAN;
        Create 'align="left"';
        END;
 Alignment = Right_: BEGIN
  Create SAN;
        Create 'align="right"';
        END;

 Height_: BEGIN
  Create SAN;
        Create 'height="';
        Create Height_;
        Create DoubleQuote;
        END;

 Img_border: BEGIN
  Create SAN;
        Create 'border="';
        Create Img_border;
        Create DoubleQuote;
        END;

 hspace: BEGIN
  Create SAN;
        Create 'hspace="';
        Create hspace;
        Create DoubleQuote;
        END;

 vspace: BEGIN
  Create SAN;
        Create 'vspace="';
        Create vspace;
        Create DoubleQuote;
        END;

 ISAMAP: BEGIN
  Create SAN;
        Create 'ismap="ismap"';
        END;

 USEMAP: BEGIN
  Create SAN;
        Create 'usemap="';
        Create USEMAP;
        Create DoubleQuote;
        END;

 nohref: BEGIN
  Create SAN;
        Create 'nohref="nohref"';
        END;

 shape = rectangle: BEGIN
  Create SAN;
        Create 'shape="rect"';
        END;
 shape = circle: BEGIN
  Create SAN;
        Create 'shape="circle"';
        END;
 shape = polygon: BEGIN
  Create SAN;
        Create 'shape="poly"';
        END;

 coords: BEGIN
  Create SAN;
        Create 'coords="';
        Create coords;
        Create DoubleQuote;
        END;

 summary: BEGIN
  Create SAN;
        Create 'summary="';
        Create Translated summary;
        Create DoubleQuote;
        END;

 frame = void: BEGIN
  Create SAN;
        Create 'frame="void"';
        END;
 frame = above: BEGIN
  Create SAN;
        Create 'frame="above"';
        END;
 frame = below: BEGIN
  Create SAN;
        Create 'frame="below"';
        END;
 frame = hsides: BEGIN
  Create SAN;
        Create 'frame="hsides"';
        END;
 frame = lhs: BEGIN
  Create SAN;
        Create 'frame="lhs"';
        END;
 frame = rhs: BEGIN
  Create SAN;
        Create 'frame="rhs"';
        END;
 frame = vsides: BEGIN
  Create SAN;
        Create 'frame="vsides"';
        END;
 frame = box: BEGIN
  Create SAN;
        Create 'frame="box"';
        END;
 frame = border: BEGIN
  Create SAN;
        Create 'frame="border"';
         END;

 rules_ = none_: BEGIN
  Create SAN;
        Create 'rules="none"';
         END;
 rules_ = groups: BEGIN
  Create SAN;
        Create 'rules="groups"';
         END;
 rules_ = rows: BEGIN
  Create SAN;
        Create 'rules="rows"';
         END;
 rules_ = cols: BEGIN
  Create SAN;
        Create 'rules="cols"';
         END;
 rules_ = all: BEGIN
  Create SAN;
        Create 'rules="all"';
         END;

 Border: BEGIN
  Create SAN;
        Create 'border="';
        Create Border;
        Create DoubleQuote;
        END;

 cellspacing: BEGIN
  Create SAN;
        Create 'cellspacing="';
        Create cellspacing;
        Create DoubleQuote;
        END;

 cellpadding: BEGIN
  Create SAN;
        Create 'cellpadding="';
        Create cellpadding;
        Create DoubleQuote;
        END;

 datapagesize: BEGIN
  Create SAN;
        Create 'datapagesize="';
        Create datapagesize;
        Create DoubleQuote;
        END;

 Position = Position_top: BEGIN
   Create SAN;
         Create 'align="top"';
        END;
 Position = Position_bottom: BEGIN
   Create SAN;
         Create 'align="bottom"';
        END;
 Position = Position_left: BEGIN
   Create SAN;
         Create 'align="left"';
        END;
 Position = Position_right: BEGIN
   Create SAN;
         Create 'align="right"';
        END;

 Row_valign = Row_top: BEGIN
   Create SAN;
         Create 'valign="top"';
        END;
 Row_valign = Row_middle: BEGIN
   Create SAN;
         Create 'valign="middle"';
        END;
 Row_valign = Row_bottom: BEGIN
   Create SAN;
         Create 'valign="bottom"';
        END;
 Row_valign = Row_baseline: BEGIN
   Create SAN;
         Create 'valign="baseline"';
        END;

 abbr: BEGIN
  Create SAN;
        Create 'abbr="';
        Create Translated abbr;
        Create DoubleQuote;
        END;

 axis: BEGIN
  Create SAN;
        Create 'axis="';
        Create axis;
        Create DoubleQuote;
        END;

 headers: BEGIN
  Create SAN;
        Create 'headers="';
        Create headers;
        Create DoubleQuote;
        END;

 scope: BEGIN
  Create SAN;
        Create 'scope="';
        Create scope;
        Create DoubleQuote;
        END;

 span_: BEGIN
  Create SAN;
        Create 'span="';
        Create span_;
        Create DoubleQuote;
        END;

 rowspan_: BEGIN
  Create SAN;
        Create 'rowspan="';
        Create rowspan_;
        Create DoubleQuote;
        END;

 colspan_: BEGIN
  Create SAN;
        Create 'colspan="';
        Create colspan_;
        Create DoubleQuote;
        END;

 Cell_align = Cell_left: BEGIN
  Create SAN;
        Create 'align="left"';
        END;
 Cell_align = Cell_center: BEGIN
  Create SAN;
        Create 'align="center"';
        END;
 Cell_align = Cell_right: BEGIN
  Create SAN;
        Create 'align="right"';
        END;
 Cell_align = Cell_justify: BEGIN
  Create SAN;
        Create 'align="justify"';
        END;
 Cell_align = Cell_char: BEGIN
  Create SAN;
        Create 'align="char"';
        END;

 char: BEGIN
  Create SAN;
        Create 'char="';
        Create char;
        Create DoubleQuote;
        END;

 charoff: BEGIN
  Create SAN;
        Create 'charoff="';
        Create charoff;
        Create DoubleQuote;
        END;
 
 Cell_valign = Cell_top: BEGIN
  Create SAN;
        Create 'valign="top"';
        END;
 Cell_valign = Cell_middle: BEGIN
  Create SAN;
        Create 'valign="middle"';
        END;
 Cell_valign = Cell_bottom: BEGIN
  Create SAN;
        Create 'valign="bottom"';
        END;
 Cell_valign = Cell_baseline: BEGIN
  Create SAN;
        Create 'valign="baseline"';
        END;

 No_wrap = no_wrap: BEGIN
  Create SAN;
        Create 'nowrap="nowrap"';
        END;

 RowHeight: BEGIN
  Create SAN;
        Create 'rows="';
        Create RowHeight;
        Create DoubleQuote;
        END;

 ColWidth: BEGIN
  Create SAN;
        Create 'cols="';
        Create ColWidth;
        Create DoubleQuote;
        END;

 FrameSrc: BEGIN
  Create SAN;
        Create 'src="';
        Create FrameSrc;
        Create DoubleQuote;
        END;

 frameborder = Border1: BEGIN
  Create SAN;
        Create 'frameborder="1"';
        END;
 frameborder = Border0: BEGIN
  Create SAN;
        Create 'frameborder="0"';
        END;

 marginwidth: BEGIN
  Create SAN;
        Create 'marginwidth="';
        Create marginwidth;
        Create DoubleQuote;
        END;

 marginheight: BEGIN
  Create SAN;
        Create 'marginheight="';
        Create marginheight;
        Create DoubleQuote;
        END;

 no_resize: BEGIN
  Create SAN;
        Create 'noresize="noresize"';
        END;

 scrolling = Yes_: BEGIN
  Create SAN;
        Create 'scrolling="yes"';
        END;
 scrolling = No_: BEGIN
  Create SAN;
        Create 'scrolling="no"';
        END;
 scrolling = auto_: BEGIN
  Create SAN;
        Create 'scrolling="auto"';
        END;

 rbspan: BEGIN
  Create SAN;
        Create 'rbspan="';
        Create rbspan;
        Create DoubleQuote;
        END;

 EmbedHidden: BEGIN
  Create SAN;
        Create 'hidden="';
        Create EmbedHidden;
        Create DoubleQuote;
        END;

 EmbedName: BEGIN
  Create SAN;
        Create 'name="';
        Create EmbedName;
        Create DoubleQuote;
        END;

 about: BEGIN
  Create SAN;
	Create 'about="';
	Create about;
	Create DoubleQuote;
	END;

 property: BEGIN
  Create SAN;
	Create 'property="';
	Create property;
	Create DoubleQuote;
	END;

 resource: BEGIN
  Create SAN;
	Create 'resource="';
	Create resource;
	Create DoubleQuote;
	END;

 datatype: BEGIN
  Create SAN;
	Create 'datatype="';
	Create datatype;
	Create DoubleQuote;
	END;

 typeof: BEGIN
  Create SAN;
	Create 'typeof="';
	Create typeof;
	Create DoubleQuote;
	END;

 Invalid_attribute:
        BEGIN
#ifndef XML
        {Create Invalid_attribute;}
#endif
        END;

 Unknown_attribute:
        BEGIN
        Create Unknown_attribute;
        END;

 IntEntity:
        BEGIN
        NoTranslation;        { do not translate & into &amp; }
        END;

 EntityName:
        BEGIN
        Create EntityName;
        Remove;        { do no output the element content }
        END;

 xmlid:        BEGIN
  Create SAN;
        Create 'xml:id="';
        Create xmlid;
        Create DoubleQuote;
        END;

 xml_space = xml_space_default:
         Create ' xml:space="default"';
 xml_space = xml_space_preserve:
         Create ' xml:space="preserve"';

 is_css:
        BEGIN
        NoTranslation;
        END;

TextTRANSLATE
        BEGIN
        '\46' -> '&amp;';        { ampersand, U+0026 ISOnum }
        '\74' -> '&lt;';        { less-than sign, U+003C ISOnum }
        '\76' -> '&gt;';         { greater-than sign, U+003E ISOnum }
{        '\240' -> '&nbsp;';     no-break space = non-breaking space, U+00A0 ISOnum 
        '\260' -> '&deg;';         degree sign, U+00B0 ISOnum }
        END;

#include "greek.sgml"

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
