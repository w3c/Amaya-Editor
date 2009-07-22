	{ Thot Structure Schema for MathML

	  I. Vatton, V. Quint		 June 1997 }


STRUCTURE MathML;

DEFPRES MathMLP;

ATTR

   { att-global }
   class = TEXT;
   PseudoClass = Text;
   style\240 = TEXT;	{ style }
   id = TEXT;
   xref = TEXT;
   other = TEXT;
   xmlid = Text;		{ xml:id }
   xml_space = xml_space_default, xml_space_preserve;

   { generic attributes for internal use }
   Highlight = Yes_;              { to show the MathML element corresponding to
	                            the current selection in the source view }
   Unknown_attribute = TEXT;	  { to store an unknown attribute }
   EntityName = TEXT;		  { the content of the Thot element is the name
				    of an entity }
   Ghost_restruct = TEXT;	  { to help structure transformation }
   IntDisplaystyle = true, false; { for internal use }
   IntHidden = Yes_;              { for internal use: hide unselected
	                            subexpressions in maction element }
CONST
   C_Empty = ' ';
   C_Space = ' '; 
   MC_Head = ' ';

STRUCT

   MathML					{ the top-level math element }
	  (ATTR macros = TEXT;
	        display_ = block_, inline_;
	        overflow = scroll, elide, truncate, scale_;
	        altimg = TEXT;
	        alttext = TEXT;
	        RealLang = Yes_;
		Charset = TEXT;
		IntSelected = Yes_)
	  = LIST OF (Construct) + (XMLcomment, XMLPI, CDATA, Unknown_namespace);

   DOCTYPE = LIST OF (DOCTYPE_line = TEXT);

   Unknown_namespace = TEXT;

   XMLcomment = LIST OF (XMLcomment_line = TEXT) - (XMLcomment);

   XMLPI = LIST OF (XMLPI_line = TEXT) ; 

   CDATA = LIST OF (CDATA_line = TEXT);

   { Presentation: all presentation constructs }
   Construct (ATTR IntPlaceholder = yes_) = CASE OF

      { ptoken }
      MTEXT
	  (ATTR fontsize = TEXT;                        { deprecated }
		fontweight = normal_, bold_;            { deprecated }
		fontstyle = normal_, italic;            { deprecated }
		fontfamily = TEXT;                      { deprecated }
		color = TEXT;                           { deprecated }
	        mathvariant = normal_, bold_, italic, bold_italic, 
	                      double_struck, bold_fraktur, script_,
	                      bold_script, fraktur_, sans_serif,
	                      bold_sans_serif, sans_serif_italic,
	                      sans_serif_bold_italic, monospace;
	        mathsize = TEXT;
	        mathcolor = TEXT;
	        mathbackground = TEXT;
		IntFontstyle = IntNormal, IntItalic;	{ for internal use }
		IntParseMe = yes_)                      { for internal use }
	  = LIST OF (MathMLCharacters);

      MI
	  (ATTR fontsize; fontweight; fontstyle; fontfamily; color;{deprecated}
	        mathvariant; mathsize; mathcolor; mathbackground;
		IntFontstyle)
	  = LIST OF (MathMLCharacters);

      MN
	  (ATTR fontsize; fontweight; fontstyle; fontfamily; color;{deprecated}
	        mathvariant; mathsize; mathcolor; mathbackground;
		IntFontstyle)
	  = LIST OF (MathMLCharacters);

      MO
	  (ATTR fontsize; fontweight; fontstyle; fontfamily; color;{deprecated}
	        mathvariant; mathsize; mathcolor; mathbackground;
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
		IntAddSpace = nospace, spacebefore, spaceafter, both_;
	        IntLargeOp = yes_)
	  = LIST OF (MathMLCharacters);

      MS
	  (ATTR fontsize; fontweight; fontstyle; fontfamily; color;{deprecated}
	        mathvariant; mathsize; mathcolor; mathbackground;
		lquote = TEXT;
		rquote = TEXT; 
		IntFontstyle)
	  = LIST OF (MathMLCharacters);

      { petoken }
      MSPACE
	  (ATTR width\240 = TEXT;
	        height\240 = TEXT;
	        depth\240 = TEXT;
	        linebreak_ = auto_, newline, indentingnewline, nobreak_,
	                        goodbreak, badbreak)
	  = CONSTANT C_Space;

      { plschema }
      { pgenschema }
      MROW
	  = LIST OF (Construct);

      MFRAC
	  (ATTR linethickness = TEXT;
		numalign = left\240, center\240, right\240;
		denomalign = left\240, center\240, right\240;
		bevelled = true, false)
	  = BEGIN
	    Numerator = LIST OF (Construct) with IntDisplaystyle = false;
	    Denominator = LIST OF (Construct) with IntDisplaystyle = false;
	    END; 

      BevelledMFRAC
	  (ATTR linethickness; numalign; denomalign; bevelled)
	  = BEGIN
	    Numerator;
	    Denominator;
	    END	;

      MSQRT
	  = BEGIN
	    SqrtBase = LIST OF (Construct);
	    END;

      MENCLOSE
	  (ATTR notation = longdiv, actuarial, radical, box, roundedbox,
                     circle_, left_, right_, top_, bottom_,
                     updiagonalstrike, downdiagonalstrike, verticalstrike,
                     horizontalstrike, madruwb)
	  = LIST OF (Construct);

      MROOT
	  = BEGIN
	    RootBase = LIST OF (Construct);
	    Index = LIST OF (Construct) with IntDisplaystyle = false;
	    END;

      MSTYLE
	  (ATTR fontsize; fontweight; fontstyle; fontfamily; color;{deprecated}
	        mathvariant; mathsize; mathcolor; mathbackground;
		form; fence; separator; lspace; rspace; stretchy; symmetric;
		maxsize; minsize; largeop; movablelimits;
		accent;
	        lquote; rquote; linethickness;
		scriptlevel = TEXT;
		scriptsizemultiplier = TEXT;
		scriptminsize = TEXT;
		background\240 = TEXT;
		open = TEXT;
		close = TEXT;
		separators = TEXT;
		subscriptshift = TEXT;
		superscriptshift = TEXT;
		accentunder = true, false;
		align = top_, bottom_, center, baseline, axis;
		rowalign = TEXT;
		columnalign = TEXT;
		columnwidth = TEXT;
		groupalign = TEXT;
		alignmentscope = TEXT;
		rowspacing = TEXT;
		columnspacing = TEXT;
		rowlines = TEXT; 
		columnlines = TEXT;
		frame = none\240, solid\240, dashed\240;
		framespacing = TEXT;
		equalrows = true, false;
		equalcolumns = true, false;
		displaystyle = true, false;
		rowspan\240 = INTEGER; 
		columnspan = INTEGER;
		edge = left\240, right\240;
		actiontype = TEXT;
	        selection = INTEGER;
		veryverythinmathspace = TEXT;
		verythinmathspace = TEXT;
		thinmathspace = TEXT;
		mediummathspace = TEXT;
		thickmathspace = TEXT;
		verythickmathspace = TEXT;
		veryverythickmathspace = TEXT;
		IntFontstyle)				{ for internal use }
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
	  (ATTR open;
	        close;
	        separators;
		IntVertStretch)			{ for internal use }
	  = BEGIN
	    OpeningFence (ATTR IntVertStretch; IntAddSpace)
                             = SYMBOL with IntVertStretch = yes_;
	    FencedExpression = LIST OF (Construct) + (FencedSeparator);
	    ClosingFence (ATTR IntVertStretch; IntAddSpace)
	                     = SYMBOL with IntVertStretch = yes_;
	    END;

      { MF is for internal use. Exported as MO }
      MF
	  (ATTR fontsize; fontweight; fontstyle; fontfamily; color;{deprecated}
		form; fence; separator; lspace;	rspace;	stretchy;
		symmetric; maxsize; minsize; largeop; movablelimits;
		accent;
		IntVertStretch)			{ for internal use }
	  = TEXT with IntVertStretch = yes_;

      { pscrschema }
      MSUB
	  (ATTR subscriptshift; 
		IntVertStretch)			{ for internal use }
	  = BEGIN
	    Base;
	    Subscript;
	    END;

      MSUP
	  (ATTR superscriptshift;
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
	    Base = LIST OF (Construct);
	    Subscript = LIST OF (Construct) with IntDisplaystyle = false;
	    Superscript = LIST OF (Construct) with IntDisplaystyle = false;
	    END;

      MUNDER
	  (ATTR accentunder;
		IntMovelimits = yes_;	        { for internal use }
		IntVertStretch)			{ for internal use }
	  = BEGIN
	    UnderOverBase
		(ATTR IntHorizStretch = yes_)	{ for internal use }
		= LIST OF (Construct);
	    Underscript
		(ATTR IntHorizStretch)		{ for internal use }
		= LIST OF (Construct) with IntDisplaystyle = false;
	    END;

      MOVER
	  (ATTR accent;
		IntMovelimits;	                { for internal use }
		IntVertStretch)			{ for internal use }
	  = BEGIN
	    UnderOverBase;
	    Overscript
		(ATTR IntHorizStretch)		{ for internal use }
		= LIST OF (Construct) with IntDisplaystyle = false;
	    END;

      MUNDEROVER
	  (ATTR accent;
		accentunder;
		IntMovelimits;	                { for internal use }
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
	    MultiscriptBase =  LIST OF (Construct);
	    PostscriptPairs = LIST OF (PostscriptPair =
					BEGIN
					MSubscript = LIST OF (Construct);
					MSuperscript = LIST OF (Construct);
					END) with IntDisplaystyle = false;
	    { pscreschema: mprescripts, none }
	    PrescriptPairs = LIST OF (PrescriptPair =
					BEGIN
					MSubscript;
					MSuperscript;
					END) with IntDisplaystyle = false;
	    END;

      { ptabschema }
      MTABLE
	  (ATTR align;
		        {*** add rownumber ***}
		rowalign;
		columnalign;
		groupalign;
		alignmentscope;
		columnwidth;
		width\240;
		rowspacing;
		columnspacing;
		rowlines; 
		columnlines; 
		frame;
		framespacing;
		equalrows;
		equalcolumns;
		displaystyle;
	        side = left\240, right\240, leftoverlap, rightoverlap;
		minlabelspacing = TEXT)
	  = BEGIN
	    MTable_head = LIST OF (MColumn_head = CONSTANT MC_Head);
	    MTable_body = LIST OF (TableRow = CASE OF
		MTR (ATTR rowalign_mtr = top_, bottom_, center, baseline, axis;
		          IntRowAlign = IntTop, IntBottom, IntCenter,
	                                IntBaseline, IntAxis; 
		          columnalign;
		          groupalign) =
	             LIST OF (MTD); 
		MLABELEDTR (ATTR rowalign_mtr; IntRowAlign;
		                 columnalign;
		                 groupalign) =
	             LIST OF (MTD) + (RowLabel, LabelCell);
		END); 
	    END with IntDisplaystyle = false;

      MGLYPH
	  (ATTR alt = TEXT;
	        fontfamily;
	        index = INTEGER)
	  = CONSTANT C_Empty;

      { pactions }
      MACTION
	  (ATTR actiontype;
	        selection)
	  = LIST OF (Construct);

      SEMANTICS  { this element is part of the content markup }
	  (ATTR definitionURL = TEXT;
	        encoding = TEXT)
	  = LIST OF (SemanticsContent = CASE OF
		Construct;
		ANNOTATION (ATTR encoding) = TEXT;
		ANNOTATION_XML (ATTR encoding) = Any;
		END); 
	
      XLink;     { for annotations }

		 { Construct1 is used as a placeholder after a msubsup,
		   msup, msub, munderover, munder or mover that stretches
		   vertically. This allows P rules to play correctly. }
	         { This definition must be the same as Construct. }
      Construct1
	  = CASE OF
	        MTEXT; MI; MN; MO; MS; MSPACE; MROW; MFRAC; BevelledMFRAC;
	        MSQRT; MENCLOSE; MROOT; MSTYLE; MERROR; MPADDED; MPHANTOM;
	        MFENCED; MF; MSUB; MSUP; MSUBSUP; MUNDER; MOVER; MUNDEROVER;
	        MMULTISCRIPTS; MTABLE; MGLYPH; MACTION; XLink;
	        END;

      END;	 { End of Construct definition }

   RowLabel = BEGIN Construct; END;     { a label within a mlabeledtr }
   LabelCell = LIST OF (Construct);	{ a MTD element used as a label in
                                          a mlabeledtr element }
   MTD
	  (ATTR rowalign_mtr; IntRowAlign;
		columnalign_mtd = left\240, center\240, right\240;
		IntColAlign = IntLeft, IntCenter, IntRight; 
		groupalign;
		rowspan\240;
		columnspan;
		MRef_column = REFERENCE(MColumn_head);   { for internal use }
                MColExt = REFERENCE(MColumn_head);       { for internal use }
                MRowExt = REFERENCE(TableRow);           { for internal use } 
	        MLineBelow = solid_, dashed_;            { for internal use }
	        MLineOnTheRight = solid_, dashed_;       { for internal use } 
	        MLineBelowExt = solid_, dashed_;         { for internal use }
	        MLineOnTheRightExt = solid_, dashed_)    { for internal use }
	  = BEGIN
	    CellWrapper = LIST OF (Construct);
	    END + (MALIGNGROUP, MALIGNMARK);
	
   { peschema }
   MALIGNMARK
	  (ATTR edge)
	  = CONSTANT C_Empty;

   MALIGNGROUP
	  (ATTR groupalign_malgr = left\240, center\240, right\240,
	                           decimalpoint)
	  = CONSTANT C_Empty;

   { FencedSeparator is for internal use only. It contains a separator
     in a mfenced expression, according to the separator attribute }
   FencedSeparator (ATTR IntVertStretch; IntAddSpace) = TEXT;

   { MathMLCharacters }
   MathMLCharacters = CASE OF
	     TEXT;
	     MGLYPH;
	     END;
	
EXCEPT

   MathML:		NoMove, NoResize;
   XMLcomment_line:	Hidden, MarkupPreserve;
   XMLPI_line:  	Hidden, MarkupPreserve;
   DOCTYPE:		NoCut, NoSpellCheck, NotAnElementNode;
   DOCTYPE_line:  	Hidden, NoCut;
   CDATA_line:          Hidden, MarkupPreserve;
   XMLPI:	        ReturnCreateNL, NoSpellCheck, NoReplicate, NotAnElementNode;
   XMLcomment:	        ReturnCreateNL, NoReplicate, NotAnElementNode;	
   Unknown_namespace:   NoCreate;
   Construct:		Hidden;
   Construct1:		Hidden;
   Numerator:		Hidden;
   Denominator:	 	Hidden;
   RootBase:		Hidden, NoCut;
   SqrtBase:		Hidden, NoCut;
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
   MTABLE:		IsTable, NoBreakByReturn;
   MTable_head:		NoCut, Hidden, NoSelect, NoBreakByReturn;
   MColumn_head:	IsColHead, NoCut, Hidden, NoSelect, NoBreakByReturn;
   MC_Head:		Hidden, NoSelect;
   MTable_body:		Hidden, NoBreakByReturn;
   TableRow:	        IsRow;
   MTR:			IsRow, NoBreakByReturn;
   MLABELEDTR:          IsRow, NoBreakByReturn;
   RowLabel:	        IsCell, Hidden, NoMove, NoResize, NoBreakByReturn,
	                NoCreate;
   MTD:			IsCell, NoMove, NoResize, NoBreakByReturn;
   CellWrapper:		NoCut, Hidden, NoBreakByReturn;
   FencedSeparator:	Hidden, NoCut, NoCreate;
   C_Empty:		Hidden, NoSelect;
   C_Space:		Hidden, NoSelect;

   id:			CssId; {Default id attribute}
   class:		CssClass, DuplicateAttr;
   PseudoClass:	        Invisible, CssPseudoClass;
   { attributes for internal use }
   RealLang:	        Invisible;
   Charset:	        Invisible;   
   Highlight:	        Invisible;
   Unknown_attribute:	Invisible;
   EntityName:	        Invisible; 
   Ghost_restruct:	Invisible;
   IntDisplaystyle:	Invisible;
   IntHidden:	        Invisible;
   IntPlaceholder:   	Invisible;
   IntFontstyle:	Invisible;
   IntParseMe:	        Invisible;
   IntAddSpace:	  	Invisible;
   IntLargeOp:	        Invisible;
   IntMovelimits:	Invisible;
   IntHorizStretch:  	Invisible;
   IntVertStretch:  	Invisible;
   IntRowAlign:	        Invisible;
   IntColAlign:	        Invisible;
   IntSelected:		Invisible;
   rowspan\240:	        RowSpan;
   columnspan:          ColSpan;
   MRef_column:		ColRef, Invisible;
   MColExt:             Invisible;
   MRowExt:             Invisible;
   MLineBelow:	        Invisible;
   MLineOnTheRight:	Invisible;
   MLineBelowExt:	Invisible;
   MLineOnTheRightExt:	Invisible;
   xml_space:           SpacePreserve;
END
