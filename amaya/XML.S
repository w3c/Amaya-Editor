
      { Thot structure schema for generic XML documents }

STRUCTURE XML;

DEFPRES XMLP;

ATTR

   { global attributes for all XML elements }
   xml_space = xml_space_default, xml_space_preserve;
   Highlight = Yes_;    { to show the XML element corresponding to the
			  current selection in the source view }
   id = Text;			{ id }
   class = Text;		{ class }
   PseudoClass = Text;

STRUCT

   XML
	  (ATTR RealLang = Yes_;)
	= LIST OF (XML_Element) + (xmlcomment, xmlpi, cdata);

   XML_Element = ANY;
           
   xmlcomment = LIST OF (xmlcomment_line = TEXT) - (xmlcomment);

   xmlpi = LIST OF (xmlpi_line = TEXT);
	
   doctype = LIST OF (doctype_line = TEXT);
	
   cdata = LIST OF (cdata_line = TEXT);

EXCEPT

   XML:                 MoveResize, NoMove;
   PICTURE:             NoMove, NoResize;
   TEXT:                NoMove, NoResize;
   Highlight:	        Invisible;
   class:		CssClass;
   PseudoClass:	        Invisible, CssPseudoClass;
   RealLang:	        Invisible;
   xmlpi:	        ReturnCreateNL, NoSpellCheck, NoReplicate;
   xmlcomment:	        ReturnCreateNL, NoReplicate;
   xmlcomment_line:     Hidden;
   xmlpi_line:          Hidden;
   doctype_line:        Hidden, NoCut;
   doctype:             NoCut;
   cdata_line:          Hidden;
   XML_Element:         Hidden, IsPlaceholder;
   xml_space:           SpacePreserve;
END
