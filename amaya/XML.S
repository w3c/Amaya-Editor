
      { Thot structure schema for Scalable Vector Graphics }

STRUCTURE XML;

DEFPRES XMLP;

ATTR

   { global attributes for all XML elements }
   id = text;
   class = text;
   style_ = text;
   xml_space = xml_space_default, xml_space_preserve;

   { global attributes for internal processing }
   Invalid_attribute = text;
   Ghost_restruct = text;
   Highlight = Yes_;        { to show the SVG element corresponding to the
			      current selection in the source view }
   Namespace = text;        { for children of element foreignObject }

STRUCT

   XML
       = LIST OF (XML_Gen) + (XMLcomment, XMLPI);
	
   XML_Gen = LIST OF (XML_Elements);

   XML_Elements = CASE OF
		TEXT; 
	      END;
           
   XMLcomment = LIST OF (XMLcomment_line = TEXT);

   XMLPI = LIST OF (XMLPI_line = TEXT);

EXCEPT

   XML:                 MoveResize, NoShowBox, NoMove;
   PICTURE:             NoMove, NoResize;
   TEXT:                NoMove, NoResize;

   id:                  CssId;
   class:               CssClass;
   Invalid_attribute:   Invisible;
   Highlight:	        Invisible; 
   Ghost_restruct:      Invisible;
   Namespace:	        Invisible;

END



