	{ Structure Schema de structure for MathML.

	  I. Vatton			June 1997	}


STRUCTURE MathML;

DEFPRES MathMLP;

STRUCT

   MathML =	MROW;
   Component = CASE OF
		  Construction;
		  MROW;
		  END;
   MROW = LIST OF (Construction);

   Construction = CASE OF
		  MTEXT = TEXT;
		  MROOT = BEGIN
		  	Root_index = MN;
			Root_exp = Component;
			END;
		  MSQRT = BEGIN
			Root_exp;
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
		  	Sub_exp = Component;
		  	Sup_exp = Component;
			END;
		  MSUB = BEGIN
			Base;
		  	Sub_exp;
			END;
		  MSUP = BEGIN
			Base;
		  	Sup_exp;
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
   Root_index:	  Hidden;
   Root_exp:	  Hidden;
   Numerator:	  Hidden;
   Denominator:	  Hidden;
   Base:	  Hidden;
   Sub_exp:	  Hidden;
   Sup_exp:	  Hidden;
   Block_exp:	  Hidden;
END
