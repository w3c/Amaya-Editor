	  { I. Vatton	February 1999	}

STRUCTURE TextFile;

DEFPRES TextFileP;

ATTR
   Highlight = Yes_;

STRUCT
   TextFile (ATTR PrintURL = Yes_;) =
	AGGREGATE
	Document_URL = TEXT;
	BODY = List of (Line\240 =
				BEGIN
				TEXT;
				END);
	END;

EXCEPT
	TextFile: 	NoMove, NoResize, ImportLine;
	BODY:           Hidden;
        PrintURL:       Invisible;
	Highlight:	Invisible;
END
