		{ Author: J. Kahan, I. Vatton 	 1999-2003  }

TRANSLATION Annot;

LINELENGTH 78;

CONST
   DoubleQuote  = '"';

RULES

   Document: BEGIN
     { Add the XML and encoding info }
     if XmlBody BEGIN
        Create '<?xml version="1.0"';
     END;
     if XmlBody AND Charset BEGIN
        Create ' encoding="';
        Create Charset; 
        Create '"';
     END;
     if XmlBody BEGIN
        Create '?>\12';
     END;
   END;

   Annot : BEGIN
     Use HTMLTX for HTML;
     Use MathMLT for MathML;
     Use SVGT for SVG;
     Use XLinkT for XLink;
     Use XMLT for XML;
     Use TextFileT for TextFile;
   END;

   Description : BEGIN
     Remove;
   END;

   Thread : BEGIN
     Remove;
   END;

END
