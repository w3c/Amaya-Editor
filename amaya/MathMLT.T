	{ Translation Schema de structure for MathML.

	  I. Vatton			June 1997	}


TRANSLATION MathML;

LineLength 78;

CONST
   tagend = '>\12';
#ifdef XML
   SOT = '<m:';         { Start of opening tag }
   SCT = '</m:';        { Start of closing tag }
   SAN = ' m:';         { Start of attribute name }
#else
   SOT = '<';           { Start of opening tag }
   SCT = '</';          { Start of closing tag }
   SAN = ' ';           { Start of attribute name }
#endif
	
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
	create SOT;
	create 'mrow';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create SCT after;
	create 'mrow>\12' after;
	end;

   MROOT :
        begin
	create SOT;
	create 'mroot';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create SCT after;
	create 'mroot>\12' after;
  	end;
   RootBase:
	if empty
		begin
		create SOT;
		create 'none/>\12';
		end;
   Index:
	if empty
		begin
		create SOT;
		create 'none/>\12';
		end;

   MSQRT :
        begin
	create SOT;
	create 'msqrt';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create SCT after;
	create 'msqrt>\12' after;
  	end;

   MFRAC :
	begin
	create SOT;
	create 'mfrac';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create SCT after;
	create 'mfrac>\12' after;
	end;
   Numerator:
	if empty
		begin
		create SOT;
		create 'none/>\12';
		end;
   Denominator:
	if empty
		begin
		create SOT;
		create 'none/>\12';
		end;

   MFENCED :
        begin
	create SOT;
	create 'mfenced';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create SCT after;
	create 'mfenced>\12' after;
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
	create SOT;
	create 'msubsup';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create SCT after;
	create 'msubsup>\12' after;
	end;
   Base:
	if empty
		begin
		create SOT;
		create 'none/>\12';
		end;
   Subscript:
	if empty
		begin
		create SOT;
		create 'none/>\12';
		end;
   Superscript:
	if empty
		begin
		create SOT;
		create 'none/>\12';
		end;

   MSUB :
	begin
	create SOT;
	create 'msub';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create SCT after;
	create 'msub>\12' after;
	end;

   MSUP :
	begin
	create SOT;
	create 'msup';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create SCT after;
	create 'msup>\12' after;
	end;

   MUNDER :
	begin
	create SOT;
	create 'munder';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create SCT after;
	create 'munder>\12' after;
	end;
   UnderOverBase:
	if empty
		begin
		create SOT;
		create 'none/>\12';
		end;
   Underscript:
	if empty
		begin
		create SOT;
		create 'none/>\12';
		end;

   MOVER :
	begin
	create SOT;
	create 'mover';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create SCT after;
	create 'mover>\12' after;
	end;
   Overscript:
	if empty
		begin
		create SOT;
		create 'none/>\12';
		end;

   MUNDEROVER :
	begin
	create SOT;
	create 'munderover';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create SCT after;
	create 'munderover>\12' after;
	end;

   MMULTISCRIPTS :
	begin
	create SOT;
	create 'mmultiscripts';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create SCT after;
	create 'mmultiscripts>\12' after;
	end;
   MultiscriptBase:
	if empty
		begin
		create SOT;
		create 'none/>\12';
		end;
   
   Construct :
	begin
	if immediately within MSubscript
		begin
		create SOT;
		create 'none/>\12';
		end;
	if immediately within MSuperscript
		begin
		create SOT;
		create 'none/>\12';
		end;
	end;
   PostscriptPair :
	if empty Remove;
   PrescriptPair :
	if empty Remove;
   PrescriptPairs :
	if not empty
		begin
		create SOT;
		create 'mprescripts/>\12' before;
		end;

   MTABLE:
	begin
	create SOT;
	create 'mtable';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create SCT after;
	create 'mtable>\12' after;
	end;

   MTable_head:
	remove;

   MTR:
	begin
	create SOT;
	create 'mtr';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create SCT after;
	create 'mtr>\12' after;
	end;

   MTD:
	begin
	create SOT;
	create 'mtd';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create SCT after;
	create 'mtd>\12' after;
	end;

   MALIGNGROUP:
	begin
	create SOT;
	create 'maligngroup/>\12';
	end;

   MALIGNMARK:
	begin
	create SOT;
	create 'malignmark/>\12';
	end;

   MF :
	begin
	create SOT;
	create 'mo';
	create Attributes;
	create '>';
	create SCT after;
	create 'mo>\12' after;
	end;

   MN :
	begin
	create SOT;
	create 'mn';
	create Attributes;
	create '>';
	create SCT after;
	create 'mn>\12' after;
	end;

   MI :
	begin
	create SOT;
	create 'mi';
	create Attributes;
	create '>';
	create SCT after;
	create 'mi>\12' after;
	end;

   MO :
	begin
	create SOT;
	create 'mo';
	create Attributes;
	create '>';
	create SCT after;
	create 'mo>\12' after;
	end;

   MS :
	begin
	create SOT;
	create 'ms';
	create Attributes;
	create '>';
	create SCT after;
	create 'ms>\12' after;
	end;

   MTEXT :
	begin
	create SOT;
	create 'mtext';
	create Attributes;
	create '>';
	create SCT after;
	create 'mtext>\12' after;
	end;

   C_Empty:
	remove;

   SEP:
	begin
	create SOT;
	create 'sep/>\12';
	end;

   MSPACE:
	begin
	create SOT;
	create 'mspace/>\12';
	end;

   MSTYLE:
	begin
	create SOT;
	create 'mstyle';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create SCT after;
	create 'mstyle>\12' after;
	end;

   MERROR:
	begin
	create SOT;
	create 'merror';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create SCT after;
	create 'merror>\12' after;
	end;

   MPADDED:
	begin
	create SOT;
	create 'mpadded';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create SCT after;
	create 'mpadded>\12' after;
	end;

   MPHANTOM:
	begin
	create SOT;
	create 'mphantom';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create SCT after;
	create 'mphantom>\12' after;
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
	create SAN;
	create 'open="';
	create open;
	create '"';
	end;

   close:
	begin
	create SAN;
	create 'close="';
	create close;
	create '"';
	end;

   separators:
	begin
	create SAN;
	create 'separators="';
	create separators;
	create '"';
	end;

   fontstyle = normal:
	begin
	create SAN;
	create 'fontstyle="normal"';
	end;
   fontstyle = italic:
	begin
	create SAN;
	create 'fontstyle="italic"';
	end;
   fence = true:
	begin
	create SAN;
	create 'fence="true"';
	end;
   fence = false:
	begin
	create SAN;
	create 'fence="false"';
	end;

#include "Text_SGML.inc"

TextTRANSLATE Grec
 
	BEGIN
	'\42' -> '&forall;';
	'\44' -> '&exist;';
	'\46' -> '&amp;';
	'\74' -> '&lt;';
	
	'\100' -> '&cong;';
	'\101' -> '&Agr;';
	'\102' -> '&Bgr;';
	'\103' -> '&KHgr;';
	'\104' -> '&Delta;';
	'\105' -> '&Egr;';
	'\106' -> '&Phi;';
	'\107' -> '&Gamma;';
	'\110' -> '&EEgr;';
	'\111' -> '&Igr;';
	'\112' -> '&thetav;';
	'\113' -> '&Kgr;';
	'\114' -> '&Lambda;';
	'\115' -> '&Mgr;';
	'\116' -> '&Ngr;';
	'\117' -> '&Ogr;';
	'\120' -> '&Pi;';
	'\121' -> '&Theta;';
	'\122' -> '&Rgr;';
	'\123' -> '&Sigma;';
	'\124' -> '&Tgr;';
	'\125' -> '&Upsi;';
	'\126' -> '&sigmav;';
	'\127' -> '&Omega;';
	'\130' -> '&Xi;';
	'\131' -> '&Psi;';
	'\132' -> '&Zgr;';
	'\133' -> '&lsqb;';
	'\134' -> '&there4;';
	'\135' -> '&rsqb;';
	'\136' -> '&bottom;';
	'\137' -> '&lowbar;';
	'\141' -> '&alpha;';
	'\142' -> '&beta;';
	'\143' -> '&chi;';
	'\144' -> '&delta;';
	'\145' -> '&epsiv;';
	'\146' -> '&phi;';
	'\147' -> '&gamma;';
	'\150' -> '&eta;';
	'\151' -> '&iota;';
	'\152' -> '&phiv;';
	'\153' -> '&kappa;';
	'\154' -> '&lambda;';
	'\155' -> '&mu;';
	'\156' -> '&nu;';
	'\157' -> '&ogr;';
	'\160' -> '&pi;';
	'\161' -> '&theta;';
	'\162' -> '&rho;';
	'\163' -> '&sigma;';
	'\164' -> '&tau;';
	'\165' -> '&upsi;';
	'\166' -> '&piv;';
	'\167' -> '&omega;';
	'\170' -> '&xi;';
	'\171' -> '&psi;';
	'\172' -> '&zeta;';

	'\241' -> '&Upsi1;';

	'\243' -> '&le;';
	'\244' -> '&Sol;';
	'\245' -> '&infin;';
	'\246' -> '&florin;';
	'\247' -> '&clubs;';
	'\250' -> '&diams;';
	'\251' -> '&hearts;';
	'\252' -> '&spades;';
	'\253' -> '&lrarr;';
	'\254' -> '&LeftArrow;';
	'\255' -> '&uarr;';
	'\256' -> '&RightArrow;';
	'\257' -> '&darr;';
	'\260' -> '&deg;';
	'\261' -> '&PlusMinus;';
	'\262' -> '&rdquo;';
	'\263' -> '&ge;';
	'\264' -> '&times;';
	'\265' -> '&prop;';
	'\266' -> '&part;';
	'\267' -> '&bull;';
	'\270' -> '&divide;';
	'\271' -> '&ne;';
	'\272' -> '&equiv;';
	'\273' -> '&ap;';
	'\274' -> '&mldr;';
	'\275' -> '&Verbar;';
	'\276' -> '&horbar;';
	'\277' -> '&cr;';
	'\300' -> '&aleph;';
	'\301' -> '&ifraktur;';
	'\302' -> '&rfraktur;';
	'\303' -> '&weierp;';
	'\304' -> '&otimes;';
	'\305' -> '&oplus;';
	'\306' -> '&empty;';
	'\307' -> '&cap;';
	'\310' -> '&cup;';
	'\311' -> '&sup;';
	'\312' -> '&supe;';
	'\313' -> '&subne;';
	'\314' -> '&sub;';
	'\315' -> '&sube;';
	'\316' -> '&isin;';
	'\317' -> '&notin;';
	'\320' -> '&angle;';
	'\321' -> '&dtri;';
	'\322' -> '&regsf;';
	'\323' -> '&copysf;';
	'\324' -> '&trade;';
	'\325' -> '&PI;';
	'\326' -> '&radic;';
	'\327' -> '&middot;';
	'\330' -> '&not;';
	'\331' -> '&and;';
	'\332' -> '&or;';
	'\333' -> '&lrArr;';
	'\334' -> '&lArr;';
	'\335' -> '&uArr;';
	'\336' -> '&rArr;';
	'\337' -> '&dArr;';
	'\340' -> '&loz;';
	'\341' -> '&lang;';
	'\342' -> '&regssf;';
	'\343' -> '&copyssf;';
	'\344' -> '&tradessf;';
	'\345' -> '&sum;';
	'\362' -> '&int;';
	END;

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
