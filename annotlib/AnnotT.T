		{ Author: I. Vatton 	 1994-1995-1996  }

TRANSLATION Annot;

LINELENGTH 78;

CONST
   DoubleQuote  = '"';

RULES

   Annot : BEGIN
     Use HTMLTX for HTML;
   END;

   Description : BEGIN
     Remove;
   END;

END
