	{ Structure Schema de structure for MathML.

	  I. Vatton			June 1997	}


STRUCTURE MathML;

DEFPRES MathMLP;

STRUCT

   MathML =	MROW;

   MROW = LIST OF (Construction);

   Component = CASE OF
		  Construction;
		  MROW;
		  END;

   Construction = CASE OF
		  MTEXT = TEXT;
		  MROOT = BEGIN
		  	Index = Component;
			RootBase = Component;
			END;
		  MSQRT = BEGIN
			RootBase;
			END;
		  MFRAC = BEGIN
			Numerator = Component;
			Denominator = Component;
			END;
		  MSUBSUP = BEGIN
			Base = CASE OF
				    SYMBOL;
				    Component;
				    END;
		  	Subscript = Component;
		  	Superscript = Component;
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
				    Component;
				    END;
			Underscript = Component;
			END;
		  MOVER = BEGIN
			UnderOverBase;
			Overscript = Component;
			END;
		  MUNDEROVER = BEGIN
			UnderOverBase;
			Underscript;
			Overscript;
			END;
		  Block = BEGIN
		  	MF = SYMBOL;
			Block_exp = Component;
			MF;
			END;
		  MN = BEGIN TEXT; END;
		  MI (ATTR fontslant = plain, italic) = BEGIN TEXT; END;
		  MO = BEGIN TEXT; END;
		  MS = BEGIN TEXT; END;
		  END;

EXCEPT

   MathML:	  Hidden;
   Component:     Hidden;
   Construction:  Hidden;
   Index:	  Hidden;
   RootBase:	  Hidden;
   Numerator:	  Hidden;
   Denominator:	  Hidden;
   Base:	  Hidden;
   Subscript:	  Hidden;
   Superscript:	  Hidden;
   Block_exp:	  Hidden;
END
