		{ Author: I. Vatton 	 1994-1995-1996  }

{ If variable XML is set, the document is saved in the XHTML syntax.
  Otherwise, it is saved in plain HTML }

TRANSLATION HTML;

LINELENGTH 78;

CONST
   DoubleQuote  = '"';

RULES

 HTML : BEGIN
#ifdef XML
	Use MathMLT for MathML;
	Use GraphMLT for GraphML;
	Create '<?xml version="1.0" ?>\12';
	if not Namespaces and HtmlDTD = Frameset
	   BEGIN
	   Create '<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Frameset//EN"\12';
	   Create '                      "http://www.w3.org/TR/1999/PR-xhtml1-19991210/DTD/xhtml1-frameset.dtd">\12';
	   END;
        if not Namespaces and not HtmlDTD = Frameset
	   BEGIN
	   Create '<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"\12';
	   Create '                      "http://www.w3.org/TR/1999/PR-xhtml1-19991210/DTD/xhtml1-transitional.dtd">\12';
	   END;	   
	Create '<html xmlns="http://www.w3.org/1999/xhtml"';
	{*** Create Namespaces; ***}
#else
	Use MathMLT for MathML;
	Use GraphMLT for GraphML;
	if HtmlDTD = Frameset
	   BEGIN
	   Create '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Frameset//EN"\12';
	   Create '                      "http://www.w3.org/TR/REC-html40/frameset.dtd">\12';
	   END;
        if not HtmlDTD = Frameset
	   BEGIN
	   Create '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN"\12';
	   Create '                      "http://www.w3.org/TR/REC-html40/loose.dtd">\12';
	   END;
	Create '<html';
#endif
	Create Attributes;
	Create '>\12';
	Create '</html>\12' After;
	END;

 HEAD:	BEGIN
	Create '<head' Before;
	Create Attributes Before;
	Create '>\12' Before;
	Indent +2 Before;
	Indent -2 After;
	Create '</head>\12\12' After;
	END;

 Document_URL:
	BEGIN
	Remove;
	END;

 TITLE:	BEGIN
	Create '<title';
	Create Attributes;
	Create '>';
	Create '</title>\12' After;
	END;

 ISINDEX: BEGIN
#ifdef XML
	{ elements TITLE and BASE must be generated as the first chidren
	  of the HEAD element }
	Get TITLE Before;
	Get BASE Before;
#endif
	Create '<isindex';
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

 BASE:	BEGIN
#ifdef XML
	{ elements TITLE and BASE must be generated as the first chidren
	  of the HEAD element }
	Get TITLE Before;
#endif
	Create '<base ';
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
	Get BASE Before;
#endif
	Create '<style';
	IF Notation
	   BEGIN
	   Create ' type="';
	   Create Notation;
	   Create DoubleQuote;
	   END;
	Create Attributes;
	Create '>';
	NoLineBreak;
	NoTranslation;
	Indent Suspend;
	Indent Resume After;
	Create '</style>\12' After;
	END;

 SCRIPT: BEGIN
#ifdef XML
	{ elements TITLE and BASE must be generated as the first chidren
	  of the HEAD element }
	Get TITLE Before;
	Get BASE Before;
#endif
	Create '<script';
	Create Attributes;
	Create '>\12';
	NoLineBreak;
	NoTranslation;
	Indent Suspend;
	Indent Resume After;
	Create '</script>\12' After;
	END;

 META: BEGIN
#ifdef XML
	{ elements TITLE and BASE must be generated as the first chidren
	  of the HEAD element }
	Get TITLE Before;
	Get BASE Before;
#endif
	Create '<meta' Before;
	Create Attributes Before;
	Remove;
#ifdef XML
	Create ' />\12' After;
#else
	Create '>\12' After;
#endif
	END;

 LINK:	BEGIN
#ifdef XML
	{ elements TITLE and BASE must be generated as the first chidren
	  of the HEAD element }
	Get TITLE Before;
	Get BASE Before;
#endif
	Create '<link';
	Remove;
	Create Attributes;
#ifdef XML
	Create ' />\12';
#else
	Create '>\12';
#endif
	END;

 BODY:	BEGIN
	Create '<body';
	Create Attributes;
	Create '>\12';
	Create '</body>\12' After;
	END;

 H1:	BEGIN
	if not within Preformatted and not first Create '\12';
	Create '<h1';
	Create Attributes;
	Create '>';
	Create '</h1>' After;
	if not within Preformatted Create '\12' After;
	END;

 H2:	BEGIN
	if not within Preformatted and not first Create '\12';
	Create '<h2';
	Create Attributes;
	Create '>';
	Create '</h2>' After;
	if not within Preformatted Create '\12' After;
	END;

 H3:	BEGIN
	if not within Preformatted and not first Create '\12';
	Create '<h3';
	Create Attributes;
	Create '>';
	Create '</h3>' After;
	if not within Preformatted Create '\12' After;
	END;

 H4:	BEGIN
	if not within Preformatted and not first Create '\12';
	Create '<h4';
	Create Attributes;
	Create '>';
	Create '</h4>' After;
	if not within Preformatted Create '\12' After;
	END;

 H5:	BEGIN
	if not within Preformatted and not first Create '\12';
	Create '<h5';
	Create Attributes;
	Create '>';
	Create '</h5>' After;
	if not within Preformatted Create '\12' After;
	END;

 H6:	BEGIN
	if not within Preformatted and not first Create '\12';
	Create '<h6';
	Create Attributes;
	Create '>';
	Create '</h6>' After;
	if not within Preformatted Create '\12' After;
	END;

 BaseFont:
	BEGIN
	Create '<basefont';
	Remove;
	Create Attributes;
#ifdef XML
	Create ' />\12';
#else
	Create '>\12';
#endif
	END;

 BR:	BEGIN
	Create '<br';
	Remove;
	Create Attributes;
#ifdef XML
	Create ' />\12';
#else
	Create '>\12';
#endif
	END;

 Italic_text:
	BEGIN
	Create '<i';
	Create Attributes;
	Create '>';
	Create '</i>' After;
	END;

 Bold_text:
	BEGIN
	Create '<b';
	Create Attributes;
	Create '>';
	Create '</b>' After;
	END;
 Teletype_text:
	BEGIN
	Create '<tt';
	Create Attributes;
	Create '>';
	Create '</tt>' After;
	END;
 Underlined_text:
	BEGIN
	Create '<u';
	Create Attributes;
	Create '>';
	Create '</u>' After;
	END;
 Struck_text:
	BEGIN
	Create '<strike';
	Create Attributes;
	Create '>';
	Create '</strike>' After;
	END;
 Big_text:
	BEGIN
	Create '<big';
	Create Attributes;
	Create '>';
	Create '</big>' After;
	END;
 Small_text:
	BEGIN
	Create '<small';
	Create Attributes;
	Create '>';
	Create '</small>' After;
	END;
 Subscript:
	BEGIN
	Create '<sub';
	Create Attributes;
	Create '>';
	Create '</sub>' After;
	END;
 Superscript:
	BEGIN
	Create '<sup';
	Create Attributes;
	Create '>';
	Create '</sup>' After;
	END;

 Emphasis:
	BEGIN
	Create '<em';
	Create Attributes;
	Create '>';
	Create '</em>' After;
	END;
 Strong:
	BEGIN
	Create '<strong';
	Create Attributes;
	Create '>';
	Create '</strong>' After;
	END;
 Def:
	BEGIN
	Create '<dfn';
	Create Attributes;
	Create '>';
	Create '</dfn>' After;
	END;
 Code:
	BEGIN
	Create '<code';
	Create Attributes;
	Create '>';
	Create '</code>' After;
	END;
 Sample:
	BEGIN
	Create '<samp';
	Create Attributes;
	Create '>';
	Create '</samp>' After;
	END;
 Keyboard:
	BEGIN
	Create '<kbd';
	Create Attributes;
	Create '>';
	Create '</kbd>' After;
	END;
 Variable:
	BEGIN
	Create '<var';
	Create Attributes;
	Create '>';
	Create '</var>' After;
	END;
 Cite:
	BEGIN
	Create '<cite';
	Create Attributes;
	Create '>';
	Create '</cite>' After;
	END;
 ABBR:
	BEGIN
	Create '<abbr';
	Create Attributes;
	Create '>';
	Create '</abbr>' After;
	END;
 ACRONYM:
	BEGIN
	Create '<acronym';
	Create Attributes;
	Create '>';
	Create '</acronym>' After;
	END;

 Font_: BEGIN
	Create '<font';
	Create Attributes;
	Create '>';
	Create '</font>' After;
	END;

 Applet: BEGIN
	Create '<applet';
	Create Attributes;
	Create '>\12';
	Create '</applet>\12' After;	
	END;

 Parameter: BEGIN
	Create '<param';
	Create Attributes;
	Remove;
#ifdef XML
	Create ' />\12';
#else
	Create '>\12';
#endif
	END;

 Object: BEGIN
	Create '<object';
	Create Attributes;
	Create '>\12';
	Create '</object>\12' After;	
	END;

 INS:	BEGIN
	Create '<ins';
	Create Attributes;
	Create '>';
	Create '</ins>' After;
	END;

 DEL:	BEGIN
	Create '<del';
	Create Attributes;
	Create '>';
	Create '</del>' After;
	END;

 Span:	BEGIN
	Create '<span';
	Create Attributes;
	Create '>';
	Create '</span>' After;
	END;

 Quotation: BEGIN
	Create '<q';
	Create Attributes;
	Create '>';
	Create '</q>' After;
	END;

 BDO: BEGIN
	Create '<bdo';
	Create Attributes;
	Create '>';
	Create '</bdo>' After;
	END;

 Math: BEGIN
#ifdef XML
	Create '<math xmlns="http://www.w3.org/1998/Math/MathML"';
#else
	Create '<math';
#endif
	Create Attributes;
	Create '>\12';
	indent +2;
	indent -2 after;
	Create '</math>\12' After;
	END;

 XMLGraphics: BEGIN
#ifdef XML
	Create '<xmlgraphics xmlns="http://www.w3.org/Graphics/SVG/Amaya2D"';
#else
	Create '<xmlgraphics';
#endif
	Create Attributes;
	Create '>\12';
	indent +2;
	indent -2 after;
	Create '</xmlgraphics>\12' After;
	END;

 Paragraph: BEGIN
	if not within List_Item and not within Definition and not first
		Create '\12';
	Create '<p';
	Create Attributes;
	Create '>';
	Create '</p>\12' After;
	END;

 Pseudo_paragraph: BEGIN
	if not last Create '\12' After;
	END;

 TEXT_UNIT: BEGIN
	IF Within Text_Input
		Remove;
	END;

 Definition_List: BEGIN
	Create '<dl';
	Create Attributes;
	Create '>\12';
	Indent +2;
	Indent -2 After;
	Create '</dl>\12' After;
	END;

 Term:	BEGIN
	Create '<dt';
	Create Attributes;
	Create '>';
	Create '</dt>\12' After;
	END;

 Definition: BEGIN
	Indent +2;
	Create '<dd';
	Create Attributes;
	Create '>';
	Indent +2;
	Indent -2 After;
	Create '</dd>\12' After;
	Indent -2 After;
	END;

 Unnumbered_List: BEGIN
	Create '<ul';
	Create Attributes;
	Create '>\12';
	Indent +2;
	Indent -2 After;
	Create '</ul>\12' After;
	END;

 Numbered_List:	BEGIN
	Create '<ol';
	Create Attributes;
	Create '>\12';
	Indent +2;
	Indent -2 After;
	Create '</ol>\12' After;
	END;

 Address: BEGIN
	Create '<address';
	Create Attributes;
	Create '>\12';
	Indent +2;
	Indent -2 After;
	Create '\12</address>\12' After;
	END;

 Menu:	BEGIN
	Create '<menu';
	Create Attributes;
	Create '>\12';
	Indent +2;
	Indent -2 After;
	Create '</menu>\12' After;
	END;

 Directory: BEGIN
	Create '<dir';
	Create Attributes;
	Create '>\12';
	Indent +2;
	Indent -2 After;
	Create '</dir>\12' After;
	END;

 List_Item: BEGIN
	Create '<li';
	Create Attributes;
	Create '>';
	Indent +2;
	Indent -2 After;
	Create '</li>\12' After;
	END;

 Preformatted: BEGIN
	NoLineBreak;
	Create '<pre';
	Create Attributes;
	Create '>';
	Indent Suspend;
        Indent Resume After;
	Create '</pre>\12' After;
	END;

 Horizontal_Rule: BEGIN
	Create '<hr';
	Create Attributes;
	Remove;
#ifdef XML
	Create ' />\12';
#else
	Create '>\12';
#endif
	if not within Preformatted Create '\12';
	END;

 Form: BEGIN
	Create '\12<form';
	Create Attributes;
	Create '>\12';
	Indent +2;
	Indent -2 After;
	Create '</form>\12' After;
	END;

 Option_Menu: BEGIN
	Create '\12<select';
	Create Attributes;
	Create '>';
	Indent +2;
	Indent -2 After;
	Create '\12</select>\12' After;
	END;

 Option: BEGIN
	Create '\12<option';
	Create Attributes;
	Create '>';
	Create '</option>' After;
	END;

 OptGroup: BEGIN
	Create '\12<optgroup';
	Create Attributes;
	Create '>';
	Indent +2;
	Indent -2 After;
	Create '\12</optgroup>' After;
	END;

 Text_Area: BEGIN
	Create '<textarea';
	Create Attributes;
	Create '>';
	Create '</textarea>' After;
	END;

 FIELDSET: BEGIN
	Create '<fieldset';
	Create Attributes;
	Create '>\12';
        Indent +2;
        Indent -2 After;
	Create '</fieldset>\12' After;
	END;

 LEGEND: BEGIN
	Create '<legend';
	Create Attributes;
	Create '>';
	Create '</legend>' After;
	END;

 LABEL: BEGIN
	Create '<label';
	Create Attributes;
	Create '>';
	Create '</label>' After;
	END;

 BUTTON: BEGIN
	Create '<button';
	Create Attributes;
	Create '>';
	Create '</button>' After;
	END;

 Checkbox_Input: BEGIN
	Create '<input type="checkbox"';
	Create Attributes;
#ifdef XML
	Create ' />';
#else
	Create '>';
#endif
	Remove;
	END;

 Radio_Input: BEGIN
	Create '<input type="radio"';
	Create Attributes;
#ifdef XML
	Create ' />';
#else
	Create '>';
#endif
	Remove;
	END;

 Text_Input: BEGIN
	Create '<input type="text"';
	Create Attributes;
#ifdef XML
	Create ' />';
#else
	Create '>';
#endif
	Remove;
	END;

 Hidden_Input: BEGIN
	Create '<input type="hidden"';
	Create Attributes;
#ifdef XML
	Create ' />';
#else
	Create '>';
#endif
	Remove;
	END;

 Password_Input: BEGIN
	Create '<input type="password"';
	Create Attributes;
#ifdef XML
	Create ' />';
#else
	Create '>';
#endif
	Remove;
	END;

 File_Input: BEGIN
	Create '<input type="file"';
	Create Attributes;
#ifdef XML
	Create ' />';
#else
	Create '>';
#endif
	Remove;
	END;

 Reset_Input: BEGIN
	Create '\12<input type="reset"';
	Create Attributes;
#ifdef XML
	Create ' />';
#else
	Create '>';
#endif
	Remove;
	END;

 Submit_Input: BEGIN
	Create '\12<input type="submit"';
	Create Attributes;
#ifdef XML
	Create ' />';
#else
	Create '>';
#endif
	Remove;
	END;

 Button_Input: BEGIN
	Create '\12<input type="button"';
	Create Attributes;
#ifdef XML
	Create ' />';
#else
	Create '>';
#endif
	Remove;
	END;

 Block_Quote: BEGIN
	Create '\12<blockquote';
	Create Attributes;
	Create '>\12';
	Indent +2;
	Indent -2 After;
	Create '</blockquote>\12' After;
	END;

 PICTURE_UNIT: BEGIN
	if IsInput BEGIN
		Create '<input type=image';
		Create Attributes;
#ifdef XML
		Create ' />\12';
#else
		Create '>';
#endif
		END;
	if Immediately within Object
		Remove;
	if NOT Immediately within Object AND NOT IsInput BEGIN
		Create '<img';
		Create Attributes;
#ifdef XML
		Create ' />\12';
#else
		Create '>';
#endif
		END;
	if NOT within Paragraph AND NOT Immediately within Object
		Create '\12' After;
	END;

 Anchor: BEGIN
	if Annotation BEGIN
		Remove;
		END;
	if NOT Annotation BEGIN
		Create '<a';
		Create Attributes;
		Create '>';
		Create '</a>' After;
		END;
	END;

 MAP:
	BEGIN
	Create '<map';
	Create Attributes;
	Create '>\12';
	Indent +2;
	Indent -2 After;
	Create '</map>\12' After;
	END;

 AREA:
	BEGIN
	Create '<area';
	Remove;
	Create Attributes;
#ifdef XML
	Create ' />\12';
#else
	Create '>\12';
#endif
	END;

{ ---- Tables ---- }

 Table:
	BEGIN
	Create '\12<table';
	Create Attributes;
	Create '>\12';
	Indent +2;
	Indent -2 after;
	Create '</table>\12' after;
	END;

 CAPTION:
	BEGIN
	Create '<caption';
	Create Attributes;
	Create '>';
	Create '</caption>\12' after;
	END;

 COLGROUP:
	BEGIN
	Create '<colgroup';
	Create Attributes;
	Create '>';
	Indent +2;
	Indent -2 after;
	Create '</colgroup>\12' after;
	END;

 COL:
	BEGIN
	Create '<col';
	Create Attributes;
	Create '>';
	END;

 Table_head:
	Remove;

 thead:
	BEGIN
	Create '<thead';
	Create Attributes;
	Create '>\12';
	Indent +2;
	Indent -2 after;
	Create '</thead>\12' after;
	END;

 tfoot:
	BEGIN
	Create '<tfoot';
	Create Attributes;
	Create '>\12';
	Indent +2;
	Indent -2 after;
	Create '</tfoot>\12' after;
	END;

 tbody:
	BEGIN
	Create '<tbody';
	Create Attributes;
	Create '>\12';
	Indent +2;
	Indent -2 after;
	Create '</tbody>\12' after;
	END;

 Table_body:
	Get tfoot;

 Table_row:
	BEGIN
	Create '<tr';
	Create Attributes;
	Create '>\12';
	Indent +2;
	Indent -2 after;
	Create '</tr>\12' after;
	END;

 Data_cell:
	BEGIN
	Create '<td';
	Create Attributes;
	Create '>';
	Indent +2;
	Indent -2 after;
	Create '</td>\12' after;
	END;

 Heading_cell:
	BEGIN
	Create '<th';
	Create Attributes;
	Create '>';
	Indent +2;
	Indent -2 after;
	Create '</th>\12' after;
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
	Create '\12<center';
	Create Attributes;
	Create '>\12';
	Create '</center>\12' After;
	END;

 NOSCRIPT: BEGIN
	Create '\12<noscript';
	Create Attributes;
	Create '>\12';
	Create '</noscript>\12' After;
	END;

 Invalid_element:
	BEGIN
#ifdef XML
	Remove;
#else
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
#endif
	END;

 Comment\240: BEGIN
	NoLineBreak;
 	Create '<!--';
	NoTranslation;
	Create '-->\12' after;
	END;

 Comment_line: BEGIN
	if not last create '\12' after;
	END;

 FRAMESET: BEGIN
	Create '<frameset';
	Create Attributes;
	Create '>\12';
        indent +2;
        indent -2 after;
	Create '</frameset>\12' After;
	END;

 Frames: BEGIN
	END;

 NOFRAMES: BEGIN
	Create '<noframes';
	Create Attributes;
	Create '>\12';
        indent +2;
        indent -2 after;
	Create '</noframes>\12' After;
	END;

 FRAME: BEGIN
	Create '<frame';
	Remove;
	Create Attributes;
#ifdef XML
	Create ' />\12';
#else
	Create '>\12';
#endif
	END;

 IFRAME: BEGIN
	Create '<iframe';
	Create Attributes;
	Create '>\12';
	Indent +2;
	Indent -2;
	Create '</iframe>\12' After;	
	END;

ATTRIBUTES

 profile: BEGIN
	Create ' profile="';
	Create profile;
	Create DoubleQuote;
	END;

 Langue:
	BEGIN
	IF not HTML
	    BEGIN
#ifdef XML
	    Create ' xml:lang="';
	    Create Langue;
	    Create DoubleQuote;
#endif
	    Create ' lang="';
	    Create Langue;
	    Create DoubleQuote;
	    END;
	IF HTML and RealLang
	    BEGIN
#ifdef XML
	    Create ' xml:lang="';
	    Create Langue;
	    Create DoubleQuote;
#endif
	    Create ' lang="';
	    Create Langue;
	    Create DoubleQuote;
	    END;
	END;

 content_type: BEGIN
	Create ' type="';
	Create content_type;
	Create DoubleQuote;
	END;

 script_language: BEGIN
	Create ' language="';
	Create script_language;
	Create DoubleQuote;
	END;

 script_src: BEGIN
	Create ' src="';
	Create script_src;
	Create DoubleQuote;
	END;

 defer: BEGIN
	Create ' defer="defer"';
	END;

 event: BEGIN
	Create ' event="';
	Create event;
	Create DoubleQuote;
	END;

 for_: BEGIN
	Create ' for="';
	Create for_;
	Create DoubleQuote;
	END;

 http_equiv: BEGIN
	Create ' http-equiv="';
	Create http_equiv;
	Create DoubleQuote;
	END;

 meta_name: BEGIN
	Create ' name="';
	Create meta_name;
	Create DoubleQuote;
	END;

 meta_content: BEGIN
	{ quotes are generated by Amaya when saving the document }
	Create ' content=';
	Create Translated meta_content;
	END;

 scheme: BEGIN
	Create ' scheme="';
	Create scheme;
	Create DoubleQuote;
	END;

 charset: BEGIN
	Create ' charset="';
	Create charset;
	Create DoubleQuote;
	END;

 REL:	BEGIN
	Create ' rel="';
	Create REL;
	Create DoubleQuote;
	END;

 REV:	BEGIN
	Create ' rev="';
	Create REV;
	Create DoubleQuote;
	END;

 HREF_ :
	BEGIN
	If Annotation BEGIN
		Remove;
		END;
	If NOT Annotation BEGIN
		Create ' href="';
		Create Translated HREF_;
		Create DoubleQuote;
		END;
	END;

 hreflang :
	BEGIN
	Create ' hreflang="';
	Create hreflang;
	Create DoubleQuote;
	END;

 media :
	BEGIN
	Create ' media="';
	Create media;
	Create DoubleQuote;
	END;

 target_ :
	BEGIN
	Create ' target="';
	Create target_;
	Create DoubleQuote;
	END;

 cite:
	BEGIN
	Create ' cite="';
	Create cite;
	Create DoubleQuote;
	END;

 datetime:
	BEGIN
	Create ' datetime="';
	Create datetime;
	Create DoubleQuote;
	END;

 background_ :
	BEGIN
	Create ' background="';
	Create background_;
	Create DoubleQuote;
	END;

 BackgroundColor :
	BEGIN
	Create ' bgcolor="';
	Create BackgroundColor;
	Create DoubleQuote;
	END;

 TextColor :
	BEGIN
	Create ' text="';
	Create TextColor;
	Create DoubleQuote;
	END;

 LinkColor :
	BEGIN
	Create ' link="';
	Create LinkColor;
	Create DoubleQuote;
	END;

 VisitedLinkColor :
	BEGIN
	Create ' vlink="';
	Create VisitedLinkColor;
	Create DoubleQuote;
	END;

 ActiveLinkColor :
	BEGIN
	Create ' alink="';
	Create ActiveLinkColor;
	Create DoubleQuote;
	END;

 BaseFontSize:
	BEGIN
	Create ' size="';
	Create BaseFontSize;
	Create DoubleQuote;
	END;

 BaseFontColor:
	BEGIN
	Create ' color="';
	Create BaseFontColor;
	Create DoubleQuote;
	END;

 BaseFontFace:
	BEGIN
	Create ' face="';
	Create BaseFontFace;
	Create DoubleQuote;
	END;

 Font_size :
	BEGIN
	Create ' size="';
	Create Font_size;
	Create DoubleQuote;
	END;

 color :
	BEGIN
	Create ' color="';
	Create color;
	Create DoubleQuote;
	END;

 face :
	BEGIN
	Create ' face="';
	Create face;
	Create DoubleQuote;
	END;

 Clear = Left_:
	Create ' clear="left"';
 Clear = Right_:
	Create ' clear="right"';
 Clear = All_:
	Create ' clear="all"';
 Clear = None:
	Create ' clear="none"';

 Align = left_:
	Create ' align="left"';
 Align = center_:
	Create ' align="center"';
 Align = right_:
	Create ' align="right"';

 TextAlign = left_:
	Create ' align="left"';
 TextAlign = center_:
	Create ' align="center"';
 TextAlign = right_:
	Create ' align="right"';
 TextAlign = justify_:
	Create ' align="justify"';

 NoShade:
	Create ' noshade="noshade"';

 Size_: BEGIN
	Create ' size="';
	Create Size_;
	Create DoubleQuote;
	END;

 Width__: BEGIN
	Create ' width="';
	Create Width__;
	Create DoubleQuote;
	END;

 declare:
	Create ' declare="declare"';

 codebase: BEGIN
	Create ' codebase="';
	Create codebase;
	Create DoubleQuote;
	END;

 code:	BEGIN
	Create ' code="';
	Create code;
	Create DoubleQuote;
	END;

 object: BEGIN
	Create ' object="';
	Create object;
	Create DoubleQuote;
	END;

 applet_name: BEGIN
	Create ' name="';
	Create applet_name;
	Create DoubleQuote;
	END;

 Param_name: BEGIN
	Create ' name="';
	Create Param_name;
	Create DoubleQuote;
	END;

 Param_value: BEGIN
	Create ' value="';
	Create Param_value;
	Create DoubleQuote;
	END;

 valuetype = data_:
	Create ' valuetype="data"';
 valuetype = ref:
	Create ' valuetype="ref"';
 valuetype = object_:
	Create ' valuetype="object"';

 Param_type: BEGIN
	Create ' type="';
	Create Param_type;
	Create DoubleQuote;
	END;

 classid: BEGIN
	Create ' classid="';
	Create classid;
	Create DoubleQuote;
	END;

 data: BEGIN
	Create ' data="';
	Create data;
	Create DoubleQuote;
	END;

 Object_type: BEGIN
	Create ' type="';
	Create Object_type;
	Create DoubleQuote;
	END;

 Link_type: BEGIN
	Create ' type="';
	Create Link_type;
	Create DoubleQuote;
	END;

 codetype: BEGIN
	Create ' codetype="';
	Create codetype;
	Create DoubleQuote;
	END;

 archive: BEGIN
	Create ' archive="';
	Create archive;
	Create DoubleQuote;
	END;

 standby: BEGIN
	Create ' standby="';
	Create standby;
	Create DoubleQuote;
	END;

 Title : BEGIN
	Create ' title="';
	Create Translated Title;
	Create DoubleQuote;
	END;

 accesskey: BEGIN
	Create ' accesskey="';
	Create accesskey;
	Create DoubleQuote;
	END;

 tabindex: BEGIN
	Create ' tabindex="';
	Create tabindex;
	Create DoubleQuote;
	END;

 Associated_control: BEGIN
	Create ' for="';
	Create Associated_control;
	Create DoubleQuote;
	END;

 Button_type = button:
	Create ' type="button"';
 Button_type = submit:
	Create ' type="submit"';
 Button_type = reset:
	Create ' type="reset"';

 Class: BEGIN
	Create ' class="';
	Create Class;
	Create DoubleQuote;
	END;

 Style\240: BEGIN
	IF not Within HEAD
	    BEGIN
	    { quotes are generated by Amaya when saving the document }
	    Create ' style=';
	    Create Style\240;
	    END;
	END;

 ID:	BEGIN
	Create ' id="';
	Create ID;
	Create DoubleQuote;
	END;

 dir = ltr :
	Create ' dir="ltr"';
 dir = rtl :
	Create ' dir="rtl"';

 onclick: BEGIN
	Create ' onclick="';
	Create onclick;
	Create DoubleQuote;
	END;

 ondblclick: BEGIN
	Create ' ondblclick="';
	Create ondblclick;
	Create DoubleQuote;
	END;

 onmousedown: BEGIN
	Create ' onmousedown="';
	Create onmousedown;
	Create DoubleQuote;
	END;

 onmouseup: BEGIN
	Create ' onmouseup="';
	Create onmouseup;
	Create DoubleQuote;
	END;

 onmouseover: BEGIN
	Create ' onmouseover="';
	Create onmouseover;
	Create DoubleQuote;
	END;

 onmousemove: BEGIN
	Create ' onmousemove="';
	Create onmousemove;
	Create DoubleQuote;
	END;

 onmouseout: BEGIN
	Create ' onmouseout="';
	Create onmouseout;
	Create DoubleQuote;
	END;

 onkeypress: BEGIN
	Create ' onkeypress="';
	Create onkeypress;
	Create DoubleQuote;
	END;

 onkeydown: BEGIN
	Create ' onkeydown="';
	Create onkeydown;
	Create DoubleQuote;
	END;

 onkeyup: BEGIN
	Create ' onkeyup="';
	Create onkeyup;
	Create DoubleQuote;
	END;

 onload: BEGIN
	Create ' onload="';
	Create onload;
	Create DoubleQuote;
	END;

 onunload: BEGIN
	Create ' onunload="';
	Create onunload;
	Create DoubleQuote;
	END;

 onfocus: BEGIN
	Create ' onfocus="';
	Create onfocus;
	Create DoubleQuote;
	END;

 onblur: BEGIN
	Create ' onblur="';
	Create onblur;
	Create DoubleQuote;
	END;

 onsubmit: BEGIN
	Create ' onsubmit="';
	Create onsubmit;
	Create DoubleQuote;
	END;

 onreset: BEGIN
	Create ' onreset="';
	Create onreset;
	Create DoubleQuote;
	END;

 onselect: BEGIN
	Create ' onselect="';
	Create onselect;
	Create DoubleQuote;
	END;

 onchange: BEGIN
	Create ' onchange="';
	Create onchange;
	Create DoubleQuote;
	END;

 BulletStyle = disc :
	Create ' type="disc"';
 BulletStyle = square :
	Create ' type="square"';
 BulletStyle = circle :
	Create ' type="circle"';

 COMPACT:
	Create ' compact="compact"';

 NumberStyle = Arabic_ :
	Create ' type="1"';
 NumberStyle = LowerAlpha :
	Create ' type="a"';
 NumberStyle = UpperAlpha :
	Create ' type="A"';
 NumberStyle = LowerRoman :
	Create ' type="i"';
 NumberStyle = UpperRoman :
	Create ' type="I"';

 Start > 1: BEGIN
	Create ' start="';
	Create Start;
	Create DoubleQuote;
	END;

 ItemStyle = disc :
	Create ' type="disc"';
 ItemStyle = square :
	Create ' type="square"';
 ItemStyle = circle :
	Create ' type="circle"';
 ItemStyle = Arabic_ :
	Create ' type="1"';
 ItemStyle = LowerAlpha :
	Create ' type="a"';
 ItemStyle = UpperAlpha :
	Create ' type="A"';
 ItemStyle = LowerRoman :
	Create ' type="i"';
 ItemStyle = UpperRoman :
	Create ' type="I"';

 ItemValue: BEGIN
	Create ' value="';
	Create ItemValue;
	Create DoubleQuote;
	END;

 Script_URL: BEGIN
	Create ' action="';
	Create Script_URL;
	Create DoubleQuote;
	END;

 METHOD=Post_:
	Create ' method="post"';

 METHOD=Get_:
	Create ' method="get"';

 ENCTYPE: BEGIN
	Create ' enctype="';
	Create ENCTYPE;
	Create DoubleQuote;
	END;

 accept_charset: BEGIN
	Create ' accept-charset="';
	Create accept_charset;
	Create DoubleQuote;
	END;

 NAME: BEGIN
	if Anchor and Annotation BEGIN
		Remove;
		END;
	if NOT Annotation BEGIN
		Create ' name="';
		Create NAME;
		Create DoubleQuote;
                END;
#ifdef XML
	if Anchor and not ID and NOT Annotation BEGIN
       		Create ' id="';
    		Create NAME;
       		Create DoubleQuote;
	       	END;
#endif
	END;

 Multiple:
	Create ' multiple="multiple"';

 MenuSize: BEGIN
	Create ' size="';
	Create MenuSize;
	Create DoubleQuote;
	END;

 Selected:
	Create ' selected="selected"';

 Value_: BEGIN
	Create ' value="';
	Create Value_;
	Create DoubleQuote;
	END;

 Rows: BEGIN
	Create ' rows="';
	Create Rows;
	Create DoubleQuote;
	END;

 Columns: BEGIN
	Create ' cols="';
	Create Columns;
	Create DoubleQuote;
	END;

 Checked = Yes_:
	Create ' checked="checked"';

 disabled:
	Create ' disabled="disabled"';

 readonly:
	Create ' readonly="readonly"';

 label: BEGIN
	Create ' label="';
	Create label;
	Create DoubleQuote;
	END;

 accept: BEGIN
	Create ' accept="';
	Create accept;
	Create DoubleQuote;
	END;

 LAlign = Top_:
	Create ' align="top"';
 LAlign = Bottom_:
	Create ' align="bottom"';
 LAlign = Left_:
	Create ' align="left"';
 LAlign = Right_:
	Create ' align="right"';

 Area_Size: BEGIN
	Create ' size="';
	Create Area_Size;
	Create DoubleQuote;
	END;

 MaxLength: BEGIN
	Create ' maxlength="';
	Create MaxLength;
	Create DoubleQuote;
	END;

 SRC:	BEGIN
	if not Immediately within Object
	   BEGIN
	   Create ' src="';
	   Create SRC;
	   Create DoubleQuote;
	   END;
	END;

 ALT:	BEGIN
	if not Immediately within Object
	   BEGIN
	   Create ' alt="';
	   Create Translated ALT;
	   Create DoubleQuote;
	   END;
	END;

 longdesc: BEGIN
	Create ' longdesc="';
	Create longdesc;
	Create DoubleQuote;
	END;

 Alignment = Top_:
	Create ' align="top"';
 Alignment = Middle_:
	Create ' align="middle"';
 Alignment = Bottom_:
	Create ' align="bottom"';
 Alignment = Left_:
	Create ' align="left"';
 Alignment = Right_:
	Create ' align="right"';

 Height_: BEGIN
	Create ' height="';
	Create Height_;
	Create DoubleQuote;
	END;

 Img_border: BEGIN
	Create ' border="';
	Create Img_border;
	Create DoubleQuote;
	END;

 hspace: BEGIN
	Create ' hspace="';
	Create hspace;
	Create DoubleQuote;
	END;

 vspace: BEGIN
	Create ' vspace="';
	Create vspace;
	Create DoubleQuote;
	END;

 ISMAP:
	Create ' ismap="ismap"';

 USEMAP: BEGIN
	Create ' usemap="';
	Create USEMAP;
	Create DoubleQuote;
	END;

 nohref:
	Create ' nohref="nohref"';

 shape = rectangle:
	Create ' shape="rect"';
 shape = circle:
	Create ' shape="circle"';
 shape = polygon:
	Create ' shape="poly"';

 coords: BEGIN
	Create ' coords="';
	Create coords;
	Create DoubleQuote;
	END;

 mode = inline_math:
	Create ' mode="inline"';
 mode = display:
	Create ' mode="display"';

 summary: BEGIN
	Create ' summary="';
	Create Translated summary;
	Create DoubleQuote;
	END;

 frame = void:
	Create ' frame="void"';
 frame = above:
	Create ' frame="above"';
 frame = below:
	Create ' frame="below"';
 frame = hsides:
	Create ' frame="hsides"';
 frame = lhs:
	Create ' frame="lhs"';
 frame = rhs:
	Create ' frame="rhs"';
 frame = vsides:
	Create ' frame="vsides"';
 frame = box:
	Create ' frame="box"';
 frame = border:
	Create ' frame="border"';
 
 rules_ = none:
	Create ' rules="none"';
 rules_ = groups:
	Create ' rules="groups"';
 rules_ = rows:
	Create ' rules="rows"';
 rules_ = cols:
	Create ' rules="cols"';
 rules_ = all:
	Create ' rules="all"';

 Border: BEGIN
	Create ' border="';
	Create Border;
	Create DoubleQuote;
	END;

 cellspacing: BEGIN
	Create ' cellspacing="';
	Create cellspacing;
	Create DoubleQuote;
	END;

 cellpadding: BEGIN
	Create ' cellpadding="';
	Create cellpadding;
	Create DoubleQuote;
	END;

 datapagesize: BEGIN
	Create ' datapagesize="';
	Create datapagesize;
	Create DoubleQuote;
	END;

 Position = Position_top:
	 Create ' align="top"';
 Position = Position_bottom:
	 Create ' align="bottom"';
 Position = Position_left:
	 Create ' align="left"';
 Position = Position_right:
	 Create ' align="right"';

 Row_valign = Row_top:
	 Create ' valign="top"';
 Row_valign = Row_middle:
	 Create ' valign="middle"';
 Row_valign = Row_bottom:
	 Create ' valign="bottom"';
 Row_valign = Row_baseline:
	 Create ' valign="baseline"';

 abbr: BEGIN
	Create ' abbr="';
	Create Translated abbr;
	Create DoubleQuote;
	END;

 axis: BEGIN
	Create ' axis="';
	Create axis;
	Create DoubleQuote;
	END;

 headers: BEGIN
	Create ' headers="';
	Create headers;
	Create DoubleQuote;
	END;

 scope: BEGIN
	Create ' scope="';
	Create scope;
	Create DoubleQuote;
	END;

 span_: BEGIN
	Create ' span="';
	Create span_;
	Create DoubleQuote;
	END;

 rowspan_: BEGIN
	Create ' rowspan="';
	Create rowspan_;
	Create DoubleQuote;
	END;

 colspan_: BEGIN
	Create ' colspan="';
	Create colspan_;
	Create DoubleQuote;
	END;

 Cell_align = Cell_left:
	Create ' align="left"';
 Cell_align = Cell_center:
	Create ' align="center"';
 Cell_align = Cell_right:
	Create ' align="right"';
 Cell_align = Cell_justify:
	Create ' align="justify"';
 Cell_align = Cell_char:
	Create ' align="char"';

 char: BEGIN
	Create ' char="';
	Create char;
	Create DoubleQuote;
	END;

 charoff: BEGIN
	Create ' charoff="';
	Create charoff;
	Create DoubleQuote;
	END;
 
 Cell_valign = Cell_top:
	Create ' valign="top"';
 Cell_valign = Cell_middle:
	Create ' valign="middle"';
 Cell_valign = Cell_bottom:
	Create ' valign="bottom"';
 Cell_valign = Cell_baseline:
	Create ' valign="baseline"';

 No_wrap = no_wrap:
	Create ' nowrap="nowrap"';

 RowHeight: BEGIN
	Create ' rows="';
	Create RowHeight;
	Create DoubleQuote;
	END;

 ColWidth: BEGIN
	Create ' cols="';
	Create ColWidth;
	Create DoubleQuote;
	END;

 FrameName: BEGIN
	Create ' name="';
	Create FrameName;
	Create DoubleQuote;
	END;

 FrameSrc: BEGIN
	Create ' src="';
	Create FrameSrc;
	Create DoubleQuote;
	END;

 frameborder = Border1:
	Create ' frameborder="1"';
 frameborder = Border0:
	Create ' frameborder="0"';

 marginwidth: BEGIN
	Create ' marginwidth="';
	Create marginwidth;
	Create DoubleQuote;
	END;

 marginheight: BEGIN
	Create ' marginheight="';
	Create marginheight;
	Create DoubleQuote;
	END;

 no_resize: BEGIN
	Create ' noresize="noresize"';
	END;

 scrolling = Yes_:
	Create ' scrolling="yes"';
 scrolling = No_:
	Create ' scrolling="no"';
 scrolling = auto:
	Create ' scrolling="auto"';

 Invalid_attribute:
	BEGIN
#ifndef XML
	Create Invalid_attribute;
#endif
	END;

 IntEntity:
	BEGIN
	NoTranslation;	{ do not translate & into &amp; }
	END;

TextTRANSLATE
	BEGIN
{ 	'\42' -> '&quot;'; }	{ quotation mark = APL quote, U+0022 ISOnum }
	'\46' -> '&amp;';	{ ampersand, U+0026 ISOnum }
	'\74' -> '&lt;';	{ less-than sign, U+003C ISOnum }
{	'\76' -> '&gt;'; }	{ greater-than sign, U+003E ISOnum }
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
