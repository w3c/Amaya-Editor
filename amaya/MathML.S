	{ Thot Structure Schema for MathML

	  I. Vatton		 June 1997	}


STRUCTURE MathML;

DEFPRES MathMLP;

ATTR
   Invalid_attribute = TEXT;
   Ghost_restruct = TEXT;

CONST
   C_Empty = ' ';

STRUCT

   MathML =	MROW;

   MROW = LIST OF (Construct);

   FencedSeparator = TEXT;

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
		  MFENCED (ATTR open = TEXT; close = TEXT; separators = TEXT;
				vertstretch = yes_) =
			BEGIN
			OpeningFence = SYMBOL with vertstretch = yes_;
			FencedExpression = LIST OF (Construct)
					         + (FencedSeparator);
			ClosingFence = SYMBOL with vertstretch = yes_;
			END;
		  MSUBSUP (ATTR vertstretch) = BEGIN
			Base = Construct;
		  	Subscript = Construct;
		  	Superscript = Construct;
			END;
		  MSUB (ATTR vertstretch) = BEGIN
			Base;
		  	Subscript;
			END;
		  MSUP (ATTR vertstretch) = BEGIN
			Base;
		  	Superscript;
			END;
		  MUNDER (ATTR vertstretch) = BEGIN
			UnderOverBase (ATTR horizstretch = yes_) = Construct;
			Underscript (ATTR horizstretch) = Construct;
			END;
		  MOVER (ATTR vertstretch) = BEGIN
			UnderOverBase;
			Overscript (ATTR horizstretch) = Construct;
			END;
		  MUNDEROVER (ATTR vertstretch) = BEGIN
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
		  MI (ATTR fontslant = plain, italic;
			   IntFontslant = IntPlain, IntItalic) = TEXT;
		  MN = TEXT;
		  MO (ATTR fence = true, false;
			   vertstretch;
			   addspace = nospace, spacebefore, spaceafter, both) =
			TEXT;
		  MF (ATTR fence; vertstretch) = TEXT with vertstretch = yes_;
		  MSPACE = CONSTANT C_Empty;
		  MS = TEXT;
		  SEP = CONSTANT C_Empty;
		  MSTYLE = LIST OF (Construct);
		  MERROR = LIST OF (Construct);
		  MPADDED = LIST OF (Construct);
		  MPHANTOM = LIST OF (Construct);
		  END;

EXCEPT

   MathML:		Hidden;
   Construct:		Hidden;
   Index:		Hidden;
   RootBase:		Hidden, NoCut;
   Numerator:		Hidden;
   Denominator:	 	Hidden;
   FencedExpression:	Hidden, NoCut, NoCreate;
   FencedSeparator:	Hidden, NoCut, NoCreate;
   OpeningFence:	Hidden, NoCut, NoCreate;
   ClosingFence:	Hidden, NoCut, NoCreate;
   Base:	 	Hidden, NoCut;
   Subscript:	 	Hidden;
   Superscript:	 	Hidden;
   UnderOverBase:	Hidden, NoCut;
   Underscript:		Hidden;
   Overscript:		Hidden;
   MultiscriptBase:	Hidden, NoCut;
   PostscriptPairs:	Hidden, NoCut;
   PostscriptPair:	Hidden;
   MSubscript:		Hidden, NoCut;
   MSuperscript:	Hidden, NoCut;
   PrescriptPairs:	Hidden;
   PrescriptPair:	Hidden;
   C_Empty:		Hidden, NoSelect;

   Invalid_attribute:	Invisible;
   Ghost_restruct:	Invisible;
   placeholder:   	Invisible;
   IntFontslant:	Invisible;
   addspace:	  	Invisible;
   entity:	  	Invisible;
   horizstretch:  	Invisible;
   vertstretch:	  	Invisible;
END
