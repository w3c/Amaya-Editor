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
		SourceDoc = Text;
		RDFtype (ATTR Resource=Text) =  CONSTANT  C_Empty;
		AnnotDate = Text;
	        END;

EXCEPT
	Annot:		NoMove, NoResize;
	Description:	NoCut, NoSelect;
	Body:		NoCut;
END


