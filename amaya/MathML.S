	{ Thot Structure Schema for MathML

	  I. Vatton		 June 1997	}


STRUCTURE MathML;

DEFPRES MathMLP;

CONST
   C_Empty = ' ';

STRUCT

   MathML =	MROW;

   MROW = LIST OF (Construct);

   Construct (ATTR placeholder = yes_) = CASE OF
		  MTEXT = TEXT (ATTR entity = yes_);
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
			Base = Construct;
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
			UnderOverBase (ATTR horizstretch = yes_) = Construct;
			Underscript (ATTR horizstretch) = Construct;
			END;
		  MOVER = BEGIN
			UnderOverBase;
			Overscript (ATTR horizstretch) = Construct;
			END;
		  MUNDEROVER = BEGIN
			UnderOverBase;
			Underscript;
			Overscript;
			END;
		  MMULTISCRIPTS = BEGIN
			MultiscriptBase =  Construct;
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
		  MO (ATTR addspace = nospace, spacebefore, spaceafter, both) =
			TEXT;
		  MS = TEXT;
		  MF = TEXT;
		  SEP = CONSTANT C_Empty;
		  MSPACE = CONSTANT C_Empty;
		  MSTYLE = LIST OF (Construct);
		  MERROR = LIST OF (Construct);
		  MPHANTOM = LIST OF (Construct);
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
   C_Empty:		Hidden, NoSelect;

   placeholder:   Invisible;
   addspace:	  Invisible;
   entity:	  Invisible;
END
