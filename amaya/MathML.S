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
		  MN = TEXT;
		  MI (ATTR fontslant = plain, italic) = TEXT;
		  MO (ATTR addspace = nospace, spacebefore, spaceafter, both;
		           entity = TEXT) =
			TEXT;
		  MS = TEXT;
		  MF = SYMBOL;
		  END;

EXCEPT

   MathML:	  Hidden;
   Construct:	  Hidden;
   Index:	  Hidden;
   RootBase:	  Hidden;
   Numerator:	  Hidden;
   Denominator:	  Hidden;
   Base:	  Hidden;
   Subscript:	  Hidden;
   Superscript:	  Hidden;

   addspace:	  Invisible;
   entity:	  Invisible;
END
