
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
       END + (XMLcomment, XMLPI, CDATA);
	
   XML_Gen = LIST OF (XML_Elements);

   XML_Elements = CASE OF
		TEXT; 
	      END;
           
   XMLcomment = LIST OF (XMLcomment_line = TEXT);

   XMLPI = LIST OF (XMLPI_line = TEXT);
	
   DOCTYPE = LIST OF (DOCTYPE_line = TEXT);
	
   CDATA = LIST OF (CDATA_line = TEXT);

EXCEPT

   XML:                 MoveResize, NoShowBox, NoMove;
   PICTURE:             NoMove, NoResize;
   TEXT:                NoMove, NoResize;
   Highlight:	        Invisible;
   class:		CssClass;
   PseudoClass:	        Invisible, CssPseudoClass;
   XMLcomment_line:     Hidden;
   XMLPI_line:          Hidden;
   DOCTYPE_line:        Hidden, NoCut;
   DOCTYPE:             NoCut;
   CDATA_line:          Hidden;

END