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

   { Presentation: all presentation constructs }
   Construct (ATTR placeholder = yes_) = CASE OF

		{ ptoken }
		  MTEXT = TEXT (ATTR entity = yes_);
		  MI (ATTR fontstyle = normal, italic;
			   IntFontstyle = IntNormal, IntItalic) = TEXT;
		  MN = TEXT;
		  MO (ATTR fence = true, false;
			   vertstretch = yes_;
			   addspace = nospace, spacebefore, spaceafter, both) =
			TEXT;
		  MS = TEXT;

		{ petoken}
		  MSPACE = CONSTANT C_Empty;

		{ plschema }
		{ pgenschema }
		  MROW = LIST OF (Construct);
		  MFRAC = BEGIN
			Numerator = Construct;
			Denominator = Construct;
			END;
		  MSQRT = BEGIN
			RootBase;
			END;
		  MROOT = BEGIN
			RootBase = Construct;
		  	Index = Construct;
			END;
		  MSTYLE = LIST OF (Construct);
		  MERROR = LIST OF (Construct);
		  MPADDED = LIST OF (Construct);
		  MPHANTOM = LIST OF (Construct);
		  MFENCED (ATTR open = TEXT; close = TEXT; separators = TEXT;
				vertstretch;) =
			BEGIN
			OpeningFence = SYMBOL with vertstretch = yes_;
			FencedExpression = LIST OF (Construct)
					         + (FencedSeparator);
			ClosingFence = SYMBOL with vertstretch = yes_;
			END;
		  { MF is for internal use. Exported as MO }
		  MF (ATTR fence; vertstretch) = TEXT with vertstretch = yes_;

		{ pscrschema }
		  MSUB (ATTR vertstretch) = BEGIN
			Base;
		  	Subscript;
			END;
		  MSUP (ATTR vertstretch) = BEGIN
			Base;
		  	Superscript;
			END;
		  MSUBSUP (ATTR vertstretch) = BEGIN
			Base = Construct;
		  	Subscript = Construct;
		  	Superscript = Construct;
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
			{ pscreschema: mprescripts, none }
			PrescriptPairs = LIST OF (PrescriptPair =
						BEGIN
						MSubscript;
						MSuperscript;
						END);
			END;

		{ ptabschema }
		  MTABLE = BEGIN
			MTable_head = LIST OF
				(MColumn_head = CONSTANT MC_Head);
			MTable_body = LIST OF
				(MTR = LIST OF (MTD));
			END;

		{ pactions }
		  MACTION = LIST OF (Construct);

		{ sep: provision for content tags }
		  SEP = CONSTANT C_Empty;

		  END;

   MTD (ATTR MRef_column = REFERENCE(MColumn_head)) = BEGIN
	  	CellWrapper = Construct;
	  	END + (MALIGNGROUP, MALIGNMARK);

   { peschema }
   MALIGNGROUP =  CONSTANT C_Empty;
   MALIGNMARK =   CONSTANT C_Empty;

   { FencedSeparator is for internal use only. It contains a separator
     in a mfenced expression, according to the separator attribute }
   FencedSeparator = TEXT;

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
