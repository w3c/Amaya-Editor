
      { Thot structure schema for Scalable Vector Graphics }

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
	= BEGIN
	  LIST OF (XML_Elements); 
          END + (xmlcomment, xmlpi, cdata);

   XML_Elements = CASE OF
		TEXT; 
		XML_Embed = NATURE;
	      END;
           
   xmlcomment = LIST OF (xmlcomment_line = TEXT);

   xmlpi = LIST OF (xmlpi_line = TEXT);
	
   doctype = LIST OF (doctype_line = TEXT);
	
   cdata = LIST OF (cdata_line = TEXT);

EXCEPT

   XML:                 MoveResize, NoShowBox, NoMove;
   PICTURE:             NoMove, NoResize;
   TEXT:                NoMove, NoResize;
   Highlight:	        Invisible;
   class:		CssClass;
   PseudoClass:	        Invisible, CssPseudoClass;
   RealLang:	        Invisible;
   xmlcomment_line:     Hidden, NoSpellCheck;
   xmlpi_line:          Hidden, NoSpellCheck;
   doctype_line:        Hidden, NoSpellCheck, NoCut;
   doctype:             NoCut;
   cdata_line:          Hidden;
   XML_Embed:           Hidden, NoSelect;
   XML_Elements:        Hidden, NoSpellCheck;

END