	{ Structure Schema de structure for MathML.

	  I. Vatton			June 1997	}


STRUCTURE MathML;

DEFPRES MathMLP;

STRUCT

   MathML =	MROW;

   MROW = LIST OF (Construction = CASE OF
		  TEXT;
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
		  SYMBOL;
		  END);

END
