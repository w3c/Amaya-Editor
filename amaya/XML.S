
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

   XML =
       BEGIN
	LIST OF (XML_Gen); 
       END + (xmlcomment, xmlpi, cdata);
	
   XML_Gen = LIST OF (XML_Elements);

   XML_Elements = CASE OF
		TEXT; 
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
   xmlcomment_line:     Hidden;
   xmlpi_line:          Hidden;
   doctype_line:        Hidden, NoCut;
   doctype:             NoCut;
   cdata_line:          Hidden;

END