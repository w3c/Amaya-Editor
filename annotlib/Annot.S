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
		Author = CONSTANT C_Empty;
		SourceDoc (ATTR HREF_=Text) = CONSTANT C_Empty;
		RDFtype (ATTR Resource=Text) = CONSTANT C_Empty; 
		AnnotCDate =  CONSTANT  C_Empty;
		AnnotMDate = CONSTANT C_Empty;
	        END;

EXCEPT
	Annot:		NoMove, NoResize;
	Description:	NoCut;
        Author:	        NoCut;
	SourceDoc:      NoCut;
	RDFtype:        NoCut;
        AnnotCDate:     NoCut;
        AnnotMDate:     NoCut;
	Body:		NoCut;
	C_Empty:	NoCut;
END



