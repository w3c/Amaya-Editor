STRUCTURE Annot;

DEFPRES AnnotP;

CONST

   C_Empty = ' ';

STRUCT

  Annot = BEGIN
	  Description;
	  Body = HTML;
	  END;

  Description = AGGREGATE
		Author = Text;
		RDFtype (ATTR resource=Text) =  CONSTANT  C_Empty;
		Date = Text;
	        END;