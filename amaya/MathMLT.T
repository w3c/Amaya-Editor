	{ Translation Schema de structure for MathML.

	  I. Vatton			June 1997	}


TRANSLATION MathML;

LineLength 78;

CONST
   tagend = '>\12';
	
RULES

   MathML:
	begin
	end;

   XMLcomment:
        BEGIN
        NoLineBreak;
        Create '<!--';
	NoTranslation;
        Create '-->\12' after;
        END;

   XMLcomment_line:
	if not last create '\12' after;

   MROW:
	begin
	create '<mrow';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create '</mrow>\12' after;
	end;

   MROOT :
        begin
	create '<mroot';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create '</mroot>\12' after;
  	end;
   RootBase:
	if empty create '<none/>\12';
   Index:
	if empty create '<none/>\12';

   MSQRT :
        begin
	create '<msqrt';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create '</msqrt>\12' after;
  	end;

   MFRAC :
	begin
	create '<mfrac';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create '</mfrac>\12' after;
	end;
   Numerator:
	if empty create '<none/>\12';
   Denominator:
	if empty create '<none/>\12';

   MFENCED :
        begin
	create '<mfenced';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create '</mfenced>\12' after;
  	end;
   FencedExpression:
	begin
	end;
   FencedSeparator:
	Remove;
   OpeningFence:
	Remove;
   ClosingFence:
	Remove;

   MSUBSUP :
	begin
	create '<msubsup';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create '</msubsup>\12' after;
	end;
   Base:
	if empty create '<none/>\12';
   Subscript:
	if empty create '<none/>\12';
   Superscript:
	if empty create '<none/>\12';

   MSUB :
	begin
	create '<msub';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create '</msub>\12' after;
	end;

   MSUP :
	begin
	create '<msup';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create '</msup>\12' after;
	end;

   MUNDER :
	begin
	create '<munder';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create '</munder>\12' after;
	end;
   UnderOverBase:
	if empty create '<none/>\12';
   Underscript:
	if empty create '<none/>\12';

   MOVER :
	begin
	create '<mover';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create '</mover>\12' after;
	end;
   Overscript:
	if empty create '<none/>\12';

   MUNDEROVER :
	begin
	create '<munderover';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create '</munderover>\12' after;
	end;

   MMULTISCRIPTS :
	begin
	create '<mmultiscripts';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create '</mmultiscripts>\12' after;
	end;
   MultiscriptBase:
	if empty create '<none/>\12';
   
   Construct :
	begin
	if immediately within MSubscript
	   create '<none/>\12';
	if immediately within MSuperscript
	   create '<none/>\12';
	end;
   PostscriptPair :
	if empty Remove;
   PrescriptPair :
	if empty Remove;
   PrescriptPairs :
	if not empty
	   create '<mprescripts/>\12' before;

   MTABLE:
	begin
	create '<mtable';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create '</mtable>\12' after;
	end;

   MTable_head:
	remove;

   MTR:
	begin
	create '<mtr';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create '</mtr>\12' after;
	end;

   MTD:
	begin
	create '<mtd';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create '</mtd>\12' after;
	end;

   MALIGNGROUP:
	create '<maligngroup/>\12';

   MALIGNMARK:
	create '<malignmark/>\12';

   MF :
	begin
	create '<mo';
	create Attributes;
	create '>';
	create '</mo>\12' after;
	end;

   MN :
	begin
	create '<mn';
	create Attributes;
	create '>';
	create '</mn>\12' after;
	end;

   MI :
	begin
	create '<mi';
	create Attributes;
	create '>';
	create '</mi>\12' after;
	end;

   MO :
	begin
	create '<mo';
	create Attributes;
	create '>';
	create '</mo>\12' after;
	end;

   MS :
	begin
	create '<ms';
	create Attributes;
	create '>';
	create '</ms>\12' after;
	end;

   MTEXT :
	begin
	create '<mtext';
	create Attributes;
	create '>';
	create '</mtext>\12' after;
	end;

   C_Empty:
	remove;

   SEP:
	create '<sep/>\12';

   MSPACE:
	create '<mspace/>\12';

   MSTYLE:
	begin
	create '<mstyle';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create '</mstyle>\12' after;
	end;

   MERROR:
	begin
	create '<merror';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create '</merror>\12' after;
	end;

   MPADDED:
	begin
	create '<mpadded';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create '</mpadded>\12' after;
	end;

   MPHANTOM:
	begin
	create '<mphantom';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create '</mphantom>\12' after;
	end;

   TEXT_UNIT :
	begin
	end;

   SYMBOL_UNIT :
	begin
	end;

ATTRIBUTES

  link = simple:
	create ' xml:link="simple"';
  link = extended:
	create ' xml:link="extended"';
  link = locator:
	create ' xml:link="locator"';
  link = group:
	create ' xml:link="group"';
  link = document:
	create ' xml:link="document"';

   entity:
	begin
	notranslation;	{ do not translate & into &amp; }
	end;
   open:
	begin
	create ' open="';
	create open;
	create '"';
	end;
   close:
	begin
	create ' close="';
	create close;
	create '"';
	end;
   separators:
	begin
	create ' separators="';
	create separators;
	create '"';
	end;
   fontstyle = normal:
	create ' fontstyle="normal"';
   fontstyle = italic:
	create ' fontstyle="italic"';
   fence = true:
	create ' fence="true"';
   fence = false:
	create ' fence="false"';

#include "Text_SGML.inc"
#include "greek.sgml"
SYMBTRANSLATE
        BEGIN
	'r'	-> '&Sqrt;';
	'i'     -> '&int;';
	'c'     -> '&ContourIntegral;';
	'd'	-> '&Integral;&Integral;';
	't'	-> '&Integral;&Integral;&Integral;';
        'S'     -> '&Sum;';
        'P'     -> '&Product;';
        'U'     -> '&Union;';
        'I'     -> '&Intersection;';
	'>'	-> '&RightArrow;';
	'<'	-> '&LeftArrow;';
	'^'	-> '&UpArrow;';
	'V'	-> '&DownArrow;';
        END;

END
