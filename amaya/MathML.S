	{ Thot Structure Schema for MathML

	  I. Vatton, V. Quint		 June 1997 }


STRUCTURE MathML;

DEFPRES MathMLP;

ATTR

   { att-globalatts }
   class = TEXT;
   style\240 = TEXT;	{ style }
   id = TEXT;
   other = TEXT;

   { xml:link }
   link = simple, extended, locator, group, document;

   { generic attributes for internal use }
   Invalid_attribute = TEXT;	{ to store an unknown attribute }
   EntityName = TEXT;		{ the content of the element is the name of
				  an entity }
   Ghost_restruct = TEXT;	{ to help structure transformation }

CONST
   C_Empty = ' ';
   MC_Head = ' ';

STRUCT

   MathML = MROW + (XMLcomment);

   XMLcomment = LIST OF (XMLcomment_line = TEXT);

   { Presentation: all presentation constructs }
   Construct (ATTR IntPlaceholder = yes_) = CASE OF

      { ptoken }
      MTEXT
	  (ATTR fontsize = TEXT;
		fontweight = normal_, bold_;
		fontstyle = normal_, italic;
		IntFontstyle = IntNormal, IntItalic;	{ for internal use }
		fontfamily = TEXT;
		color = TEXT)
	  = TEXT;

      MI
	  (ATTR fontsize; fontweight; fontstyle; fontfamily; color;
		IntFontstyle)
	  = TEXT;

      MN
	  (ATTR fontsize; fontweight; fontstyle; fontfamily; color;
		IntFontstyle)
	  = TEXT;

      MO
	  (ATTR fontsize; fontweight; fontstyle; fontfamily; color;
		form = prefix, infix, postfix;
		fence = true, false;
		separator = true, false;
		lspace = TEXT;
		rspace = TEXT;
		stretchy = true, false;
		symmetric = true, false;
		maxsize = TEXT;
		minsize = TEXT;
		largeop = true, false;
		movablelimits = true, false;
		accent = true, false;
		IntFontstyle;				{ for internal use }
		IntVertStretch = yes_;			{ for internal use }
		IntAddSpace = nospace, spacebefore, spaceafter, both)
	  = TEXT;

      MS
	  (ATTR fontsize; fontweight; fontstyle; fontfamily; color;
		lquote = TEXT;
		rquote = TEXT; 
		IntFontstyle)
	  = TEXT;

      { petoken}
      MSPACE
	  (ATTR width\240 = TEXT;
	        height\240 = TEXT;
	        depth\240 = TEXT)
	  = CONSTANT C_Empty;

      { plschema }
      { pgenschema }
      MROW
	  = LIST OF (Construct);

      MFRAC
	  (ATTR linethickness = TEXT)
	  = BEGIN
	    Numerator = Construct;
	    Denominator = Construct;
	    END	;

      MSQRT
	  = BEGIN
	    RootBase = Construct;
	    END;

      MROOT
	  = BEGIN
	    RootBase;
	    Index = Construct;
	    END;

      MSTYLE
	  (ATTR fontsize; fontweight; fontstyle; fontfamily; color;
		form; fence; separator; lspace; rspace; stretchy; symmetric;
		maxsize; minsize; largeop; movablelimits; accent;
		lquote; rquote; linethickness;
		scriptlevel = TEXT;
		scriptsizemultiplier = TEXT;
		scriptminsize = TEXT;
		background\240 = TEXT;
{****	In the current version, Thot only allows 25 attributes to be declared
	for an element type. So, the following attributes cannot be declared.
		open = TEXT;
		close = TEXT;
		separators = TEXT;
		subscriptshift = TEXT;
		superscriptshift = TEXT;
		accentunder = true, false;
		align = TEXT;
		rowalign = TEXT;
		columnalign = TEXT;
		groupalign = TEXT;
		alignmentscope = TEXT;
		rowspacing = TEXT;
		columnspacing = TEXT;
		rowlines = TEXT; 
		columnlines = TEXT;
		frame = none, solid\240, dashed\240;
		framespacing = TEXT;
		equalrows = TEXT;
		equalcolumns = TEXT;
		displaystyle = true, false;
		rowspan = TEXT;
		columnspan = TEXT;
		edge = left\240, right\240;
		actiontype = TEXT;
	        selection = INTEGER;
*****}		IntFontstyle)				{ for internal use }
	  = LIST OF (Construct);

      MERROR
	  = LIST OF (Construct);

      MPADDED
	  (ATTR width\240; height\240; depth\240;
		lspace)
	  = LIST OF (Construct);

      MPHANTOM
	  = LIST OF (Construct);

      MFENCED
	  (ATTR open = TEXT;
	        close = TEXT;
	        separators = TEXT;
		IntVertStretch)			{ for internal use }
	  = BEGIN
	    OpeningFence = SYMBOL with IntVertStretch = yes_;
	    FencedExpression = LIST OF (Construct) + (FencedSeparator);
	    ClosingFence = SYMBOL with IntVertStretch = yes_;
	    END;

      { MF is for internal use. Exported as MO }
      MF
	  (ATTR fence;
		IntVertStretch)			{ for internal use }
	  = TEXT with IntVertStretch = yes_;

      { pscrschema }
      MSUB
	  (ATTR subscriptshift = TEXT; 
		IntVertStretch)			{ for internal use }
	  = BEGIN
	    Base;
	    Subscript;
	    END;

      MSUP
	  (ATTR superscriptshift = TEXT;
		IntVertStretch)			{ for internal use }
	  = BEGIN
	    Base;
	    Superscript;
	    END;

      MSUBSUP
	  (ATTR subscriptshift;
		superscriptshift;
		IntVertStretch)			{ for internal use }
	  = BEGIN
	    Base = Construct;
	    Subscript = Construct;
	    Superscript = Construct;
	    END;

      MUNDER
	  (ATTR accentunder = true, false; 
		IntVertStretch)			{ for internal use }
	  = BEGIN
	    UnderOverBase
		(ATTR IntHorizStretch = yes_)	{ for internal use }
		= Construct;
	    Underscript
		(ATTR IntHorizStretch)		{ for internal use }
		= Construct;
	    END;

      MOVER
	  (ATTR accent;
		IntVertStretch)			{ for internal use }
	  = BEGIN
	    UnderOverBase;
	    Overscript
		(ATTR IntHorizStretch)		{ for internal use }
		= Construct;
	    END;

      MUNDEROVER
	  (ATTR accent;
		accentunder;
		IntVertStretch)			{ for internal use }
	  = BEGIN
	    UnderOverBase;
	    Underscript;
	    Overscript;
	    END;

      MMULTISCRIPTS
	  (ATTR subscriptshift;
		superscriptshift)
	  = BEGIN
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
      MTABLE
	  (ATTR align = TEXT;
		rowalign = TEXT;
		columnalign = TEXT;
		groupalign = TEXT;
		alignmentscope = TEXT;
		rowspacing = TEXT;
		columnspacing = TEXT;
		rowlines = TEXT	; 
		columnlines = TEXT; 
		frame = none, solid\240, dashed\240;
		framespacing = TEXT;
		equalrows = TEXT;
		equalcolumns = TEXT;
		displaystyle = true, false;
	    )
	  = BEGIN
	    MTable_head = LIST OF (MColumn_head = CONSTANT MC_Head);
	    MTable_body = LIST OF (MTR); 
	    END;

      { pactions }
      MACTION
	  (ATTR actiontype = TEXT;
	        selection = INTEGER)
	  = LIST OF (Construct);

      { sep: provision for content tags }
      SEP = CONSTANT C_Empty;

      END;	 { End of Construct definition }

   MTR
	  (ATTR rowalign;
		columnalign;
		groupalign;
	    )
	  = LIST OF (MTD);
   MTD
	  (ATTR rowalign;
		columnalign;
		groupalign;
		rowspan\240 = TEXT;
		columnspan = TEXT;
		MRef_column = REFERENCE(MColumn_head))	{ for internal use }
	  = BEGIN
	    CellWrapper = Construct;
	    END + (MALIGNGROUP, MALIGNMARK);

   { peschema }
   MALIGNMARK
	  (ATTR edge = left\240, right\240)
	  = CONSTANT C_Empty;

   MALIGNGROUP
	  (ATTR groupalign) 
	  = CONSTANT C_Empty;

   { FencedSeparator is for internal use only. It contains a separator
     in a mfenced expression, according to the separator attribute }
   FencedSeparator = TEXT;

EXCEPT

   MathML:		Hidden, NoMove, NoResize;
   XMLcomment_line:	Hidden;
   Construct:		Hidden;
   Numerator:		Hidden;
   Denominator:	 	Hidden;
   RootBase:		Hidden, NoCut;
   Index:		Hidden;
   OpeningFence:	Hidden, NoCut, NoCreate;
   FencedExpression:	Hidden, NoCut, NoCreate;
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
   FencedSeparator:	Hidden, NoCut, NoCreate;
   C_Empty:		Hidden, NoSelect;

   id:			CssId;
   class:		CssClass;
   { attributes for internal use }
   Invalid_attribute:	Invisible;
   Ghost_restruct:	Invisible;
   IntPlaceholder:   	Invisible;
   IntFontstyle:	Invisible;
   IntAddSpace:	  	Invisible;
   IntHorizStretch:  	Invisible;
   IntVertStretch:  	Invisible;
   MRef_column:		ColRef, Invisible;

END
