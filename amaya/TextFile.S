	  { I. Vatton	February 1999	}

STRUCTURE TextFile;

DEFPRES TextFileP;

ATTR
   Highlight = Yes_;
   IsLink = Yes_;
   IsTag = Yes_;
   IsString = Yes_;

STRUCT
   TextFile (ATTR PrintURL = Yes_; Source = Yes_) =
	AGGREGATE
	Document_URL = TEXT;
	BODY = List of (Line\240 =
				BEGIN
				TEXT;
				END);
	END;

EXCEPT
	TextFile: 	NoMove, NoResize, ImportLine;
        Document_URL:	Hidden, NoCut;
	BODY:           Hidden, NoCut;
        Language:	Invisible; 
	Highlight:	Invisible;
	IsLink:		Invisible;
	IsTag:		Invisible;
	IsString:       Invisible;
        PrintURL:       Invisible;
	Source:	        Invisible;
END
