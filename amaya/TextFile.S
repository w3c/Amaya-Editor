	  { I. Vatton	1999 - 2003	}

STRUCTURE TextFile;

DEFPRES TextFileP;

ATTR
  Highlight = Yes_;
  IsLink = Yes_;
  IsTag = Yes_;
  IsMarkup = Yes_;
  IsString = Yes_;
  IsComment = Yes_;

STRUCT
  TextFile (ATTR PrintURL = Yes_; Source = Yes_) =
	  AGGREGATE
	    Document_URL = TEXT;
	    BODY = List of (Line\240 = TEXT);
	  END;

EXCEPT
	TextFile: 	  NoMove, NoResize, ImportLine;
  Document_URL:	Hidden, NoSpellCheck, NoCut;
	BODY:         Hidden, NoCut, NoBreakByReturn;
  Language:	    Invisible; 
	Highlight:	  Invisible;
	IsLink:		    Invisible;
	IsTag:		    Invisible;
	IsMarkup:	    Invisible;
	IsString:     Invisible;
	IsComment:    Invisible;
  PrintURL:     Invisible;
	Source:	      Invisible;
END
