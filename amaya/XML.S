
      { Thot structure schema for Scalable Vector Graphics }

STRUCTURE XML;

DEFPRES XMLP;

ATTR

   { global attributes for all XML elements }
   xml_space = xml_space_default, xml_space_preserve;

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

END