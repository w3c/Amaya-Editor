	{ Thot Structure Schema for MathML

	  I. Vatton		 June 1997	}


STRUCTURE MathML;

DEFPRES MathMLP;

ATTR
   { generic attributes for Amaya and XML }
   Invalid_attribute = TEXT;
   Ghost_restruct = TEXT;
   link = simple, extended, locator, group, document;	{ xml:link }

CONST
   C_Empty = ' ';
   MC_Head = ' ';

STRUCT

   MathML = MROW + (XMLcomment);

   XMLcomment = LIST OF (XMLcomment_line = TEXT);

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
		  MTABLE = BEGIN
			MTable_head = LIST OF
				(MColumn_head = CONSTANT MC_Head);
			MTable_body = LIST OF
				(MTR = LIST OF (MTD));
			END;
		  MI (ATTR fontstyle = normal, italic;
			   IntFontstyle = IntNormal, IntItalic) = TEXT;
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

   MTD (ATTR MRef_column = REFERENCE(MColumn_head)) = BEGIN
		  CellWrapper = Construct;
		  END + (MALIGNGROUP, MALIGNMARK);

   MALIGNGROUP =  CONSTANT C_Empty;
   MALIGNMARK =   CONSTANT C_Empty;

EXCEPT

   MathML:		Hidden, NoMove, NoResize;
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
   MTABLE:		IsTable;
   MTable_head:		NoCut, Hidden, NoSelect;
   MColumn_head:	IsColHead, NoCut, Hidden, NoSelect;
   MC_Head:		Hidden, NoSelect;
   MTable_body:		Hidden;
   MTR:			IsRow;
   MTD:			IsCell, NoMove, NoResize;
   CellWrapper:		NoCut, Hidden;
   C_Empty:		Hidden, NoSelect;

   Invalid_attribute:	Invisible;
   Ghost_restruct:	Invisible;
   placeholder:   	Invisible;
   IntFontstyle:	Invisible;
   addspace:	  	Invisible;
   entity:	  	Invisible;
   horizstretch:  	Invisible;
   vertstretch:	  	Invisible;
   MRef_column:		ColRef, Invisible;
END
