	 { I. Vatton 	 February 1999 } 

TRANSLATION TextFile;

LINELENGTH 200;

RULES

 TextFile: BEGIN
	END;

 Document_URL: BEGIN
        Remove;
	END;

 Line_: BEGIN
	Create '\12';
	END;

TextTRANSLATE
	BEGIN
	'\212' -> '\12'; { &CTLrc; }	
	END;
END
