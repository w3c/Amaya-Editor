	 { I. Vatton 	 February 1999 } 

TRANSLATION TextFile;

LINELENGTH 0;

RULES

 TextFile: BEGIN
	END;

 Document_URL: BEGIN
        Remove;
	END;

 Line\240: BEGIN
	Create '\12' After;
	END;

TextTRANSLATE
	BEGIN
	'\212' -> '\12'; { &CTLrc; }	
	END;
END
