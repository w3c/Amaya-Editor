		{ Author: J. Kahan, I. Vatton 	 1999-2001  }

TRANSLATION Annot;

LINELENGTH 78;

CONST
   DoubleQuote  = '"';

RULES

   Document: BEGIN
    Create '<?xml version="1.0"';
    Create '?>\12';
   END;


   Annot : BEGIN
     Use HTMLTX for HTML;
     Use MathMLT for MathML;
     Use SVGT for SVG;
     Use XLinkT for XLink;
   END;

   Description : BEGIN
     Remove;
   END;

   Thread : BEGIN
     Remove;
   END;

END
