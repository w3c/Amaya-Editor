	{ Structure Schema de structure for MathML.

	  I. Vatton			June 1997	}


STRUCTURE MathML;

DEFPRES MathMLP;

STRUCT

   MathML =	MROW;

   MROW = LIST OF (Construction = CASE OF
		  MTEXT = TEXT;
		  MROOT =
			BEGIN
		  	Root_index = MN;
			Root_exp = MROW;
			END;
		  MSQRT =
			BEGIN
			Root_exp;
			END;
		  MFRAC =
			BEGIN
			Numerator = MROW;
			Denominator = MROW;
			END;
		  MSUBSUP =
			BEGIN
			Base = CASE OF
				    SYMBOL;
				    MROW;
				    END;
		  	Sub_exp = MROW;
		  	Sup_exp = MROW;
			END;
		  MSUB =
			BEGIN
			Base;
		  	Sub_exp;
			END;
		  MSUP =
			BEGIN
			Base;
		  	Sup_exp;
			END;
		  MFENCE = SYMBOL;
		  MN = TEXT;
		  MI = TEXT;
		  MO = TEXT;
		  MS = TEXT;
		  SYMBOL;
		  END);

EXCEPT

   MathML:	  Hidden;
   Construction:  Hidden;
   Root_index:	  Hidden;
   Root_exp:	  Hidden;
   Numerator:	  Hidden;
   Denominator:	  Hidden;
   Base:	  Hidden;
   Sub_exp:	  Hidden;
   Sup_exp:	  Hidden;

END
