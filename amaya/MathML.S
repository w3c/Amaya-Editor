	{ Structure Schema de structure for MathML.

	  I. Vatton			June 1997	}


STRUCTURE MathML;

DEFPRES MathMLP;

STRUCT

   MathML =	MROW;

   MROW = LIST OF (Construct);

   Construct = CASE OF
		  MTEXT = TEXT;
		  MROW;
		  MROOT = BEGIN
			RootBase = Construct;
		  	Index = Construct;
			END;
		  MSQRT = BEGIN
			RootBase;
			END;
		  MFRAC = BEGIN
			Numerator = Construct;
			Denominator = Construct;
			END;
		  MSUBSUP = BEGIN
			Base = CASE OF
				    SYMBOL;
				    Construct;
				    END;
		  	Subscript = Construct;
		  	Superscript = Construct;
			END;
		  MSUB = BEGIN
			Base;
		  	Subscript;
			END;
		  MSUP = BEGIN
			Base;
		  	Superscript;
			END;
		  MUNDER = BEGIN
			UnderOverBase = CASE OF
				    SYMBOL;
				    Construct;
				    END;
			Underscript = Construct;
			END;
		  MOVER = BEGIN
			UnderOverBase;
			Overscript = Construct;
			END;
		  MUNDEROVER = BEGIN
			UnderOverBase;
			Underscript;
			Overscript;
			END;
		  MMULTISCRIPTS = BEGIN
			MultiscriptBase =  CASE OF
				    SYMBOL;
				    Construct;
				    END;
			PostscriptPairs = LIST OF (PostscriptPair =
						BEGIN
						MSubscript = Construct;
						MSuperscript = Construct;
						END);
			PrescriptPairs = LIST OF (PrescriptPair =
						BEGIN
						MSubscript;
						MSuperscript;
						END);
			END;
		  MN = TEXT;
		  MI (ATTR fontslant = plain, italic) = TEXT;
		  MO (ATTR addspace = nospace, spacebefore, spaceafter, both;
		           entity = TEXT) =
			TEXT;
		  MS = TEXT;
		  MF = SYMBOL;
		  END;

EXCEPT

   MathML:		Hidden;
   Construct:		Hidden;
   Index:		Hidden, NoCut;
   RootBase:		Hidden, NoCut;
   Numerator:		Hidden, NoCut;
   Denominator:	 	Hidden, NoCut;
   Base:	 	Hidden, NoCut;
   Subscript:	 	Hidden, NoCut;
   Superscript:	 	Hidden, NoCut;
   UnderOverBase:	Hidden, NoCut;
   Underscript:		Hidden, NoCut;
   Overscript:		Hidden, NoCut;
   MultiscriptBase:	Hidden, NoCut;
   PostscriptPairs:	Hidden, NoCut;
   PostscriptPair:	Hidden;
   MSubscript:		Hidden, NoCut;
   MSuperscript:	Hidden, NoCut;
   PrescriptPairs:	Hidden;
   PrescriptPair:	Hidden;

   addspace:	  Invisible;
   entity:	  Invisible;
END
