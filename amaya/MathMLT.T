	{ Translation Schema for MathML.

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

   MSQRT:
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
   SqrtBase:
	if empty
		begin
		create SOT;
		create 'none/>\12';
		end;

   MENCLOSE:
        begin
	create SOT;
	create 'menclose';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create SCT after;
	create 'menclose>\12' after;
  	end;

   MROOT:
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

   MFRAC:
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
   BevelledMFRAC:
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

   MFENCED:
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

   MSUBSUP:
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

   MSUB:
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

   MSUP:
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

   MUNDER:
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

   MOVER:
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

   MUNDEROVER:
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

   MMULTISCRIPTS:
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
   
   Construct:
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
   PostscriptPair:
	if empty Remove;
   PrescriptPair:
	if empty Remove;
   PrescriptPairs:
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

   MLABELEDTR:
	begin
	create SOT;
	create 'mlabeledtr';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create SCT after;
	create 'mlabeledtr>\12' after;
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
	create 'maligngroup';
	create Attributes;
	create '/>\12';
	end;

   MALIGNMARK:
	begin
	create SOT;
	create 'malignmark';
	create Attributes;
        create '/>\12';
	end;

   MF:
	begin
	create SOT;
	create 'mo';
	create Attributes;
	create '>';
	create SCT after;
	create 'mo>\12' after;
	end;

   MN:
	begin
	create SOT;
	create 'mn';
	create Attributes;
	create '>';
	create SCT after;
	create 'mn>\12' after;
	end;

   MI:
	begin
	create SOT;
	create 'mi';
	create Attributes;
	create '>';
	create SCT after;
	create 'mi>\12' after;
	end;

   MO:
	begin
	create SOT;
	create 'mo';
	create Attributes;
	create '>';
	create SCT after;
	create 'mo>\12' after;
	end;

   MS:
	begin
	create SOT;
	create 'ms';
	create Attributes;
	create '>';
	create SCT after;
	create 'ms>\12' after;
	end;

   MTEXT:
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
	create 'mspace';
        create Attributes;
        create '/>\12';
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

   MCHAR:
	begin
	create SOT;
	create 'mchar';
	create Attributes;
	create '/>';
	Remove;
	end;

   MGLYPH:
	begin
	create SOT;
	create 'mglyph';
	create Attributes;
        create '/>';
	Remove;
	end;

   MACTION:
	begin
	create SOT;
	create 'maction';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create SCT after;
	create 'maction>\12' after;
	end;

   TEXT_UNIT:
	begin
	end;

   SYMBOL_UNIT:
	begin
	end;

ATTRIBUTES

   class:
	begin
	create SAN;
	create 'class="';
	create class;
	create '"';
	end;
  
   style\240:
	begin
	create SAN;
	create 'style="';
	create style\240;
	create '"';
	end;
  
   id:
	begin
	create SAN;
	create 'id="';
	create id;
	create '"';
	end;
  
   xref:
	begin
	create SAN;
	create 'xref="';
	create xref;
	create '"';
	end;
  
   other:
	begin
	create SAN;
	create 'other="';
	create other;
	create '"';
	end;
  
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

   fontsize:
	begin
	create SAN;
	create 'fontsize="';
	create fontsize;
	create '"';	
	end;

   fontweight = normal_:
	begin
	create SAN;
	create 'fontweight="normal"';
	end;
   fontweight = bold_:
	begin
	create SAN;
	create 'fontweight="bold"';
	end;

   fontstyle = normal_:
	begin
	create SAN;
	create 'fontstyle="normal"';
	end;
   fontstyle = italic:
	begin
	create SAN;
	create 'fontstyle="italic"';
	end;

   fontfamily:
	begin
	create SAN;
	create 'fontfamily="';
	create fontfamily;
	create '"';	
	end;

   color:
	begin
	create SAN;
	create 'color="';
	create color;
	create '"';	
	end;

   form = prefix:
	begin
	create SAN;
	create 'form="prefix"';
	end;
   form = infix:
	begin
	create SAN;
	create 'form="infix"';
	end;
   form = postfix:
	begin
	create SAN;
	create 'form="postfix"';
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

   separator = true:
	begin
	create SAN;
	create 'separator="true"';
	end;
   separator = false:
	begin
	create SAN;
	create 'separator="false"';
	end;

   lspace:
	begin
	create SAN;
	create 'lspace="';
	create lspace;
	create '"';	
	end;

   rspace:
	begin
	create SAN;
	create 'rspace="';
	create rspace;
	create '"';	
	end;

   stretchy = true:
	begin
	create SAN;
	create 'stretchy="true"';
	end;
   stretchy = false:
	begin
	create SAN;
	create 'stretchy="false"';
	end;

   symmetric = true:
	begin
	create SAN;
	create 'symmetric="true"';
	end;
   symmetric = false:
	begin
	create SAN;
	create 'symmetric="false"';
	end;

   maxsize:
	begin
	create SAN;
	create 'maxsize="';
	create maxsize;
	create '"';	
	end;

   minsize:
	begin
	create SAN;
	create 'minsize="';
	create minsize;
	create '"';	
	end;

   largeop = true:
	begin
	create SAN;
	create 'largeop="true"';
	end;
   largeop = false:
	begin
	create SAN;
	create 'largeop="false"';
	end;

   movablelimits = true:
	begin
	create SAN;
	create 'movablelimits="true"';
	end;
   movablelimits = false:
	begin
	create SAN;
	create 'movablelimits="false"';
	end;

   accent = true:
	begin
	create SAN;
	create 'accent="true"';
	end;
   accent = false:
	begin
	create SAN;
	create 'accent="false"';
	end;

   width\240:
	begin
	create SAN;
	create 'width="';
	create width\240;
	create '"';
	end;

   height\240:
	begin
	create SAN;
	create 'height="';
	create height\240;
	create '"';
	end;

   depth\240:
	begin
	create SAN;
	create 'depth="';
	create depth\240;
	create '"';
	end;

   lquote:
	begin
	create SAN;
	create 'lquote="';
	create lquote;
	create '"';
	end;

   rquote:
	begin
	create SAN;
	create 'rquote="';
	create rquote;
	create '"';
	end;

   linethickness:
	begin
	create SAN;
	create 'linethickness="';
	create linethickness;
	create '"';
	end;

   numalign = left\240:
	begin
	create SAN;
	create 'numalign="left"';
	end;
   numalign = center\240:
	begin
	create SAN;
	create 'numalign="center"';
	end;
   numalign = right\240:
	begin
	create SAN;
	create 'numalign="right"';
	end;

   denomalign = left\240:
	begin
	create SAN;
	create 'denomalign="left"';
	end;
   denomalign = center\240:
	begin
	create SAN;
	create 'denomalign="center"';
	end;
   denomalign = right\240:
	begin
	create SAN;
	create 'denomalign="right"';
	end;

   bevelled = true:
	begin
	create SAN;
	create ' bevelled="true"';
	end;
   bevelled = false:
	begin
	create SAN;
	create ' bevelled="false"';
	end;

   notation = longdiv:
	begin
	create SAN;
	create ' notation="longdiv"';
	end;
   notation = actuarial:
	begin
	create SAN;
	create ' notation="actuarial"';
	end;
   notation =radical :
	begin
	create SAN;
	create ' notation="radical"';
	end;

   scriptlevel:
	begin
	create SAN;
	create 'scriptlevel="';
	create scriptlevel;
	create '"';
	end;

   displaystyle = true:
	begin
	create SAN;
	create 'displaystyle="true"';
	end;
   displaystyle = false:
	begin
	create SAN;
	create 'displaystyle="false"';
	end;

   side = left\240:
	begin
	create SAN;
	create 'side="left"';
	end;
   side = right\240:
	begin
	create SAN;
	create 'side="right"';
	end;
   side = leftoverlap:
	begin
	create SAN;
	create 'side="leftoverlap"';
	end;
   side = rightoverlap:
	begin
	create SAN;
	create 'side="rightoverlap"';
	end;

   minlabelspacing:
	begin
	create SAN;
	create 'minlabelspacing="';
	create minlabelspacing;
	create '"';
	end;

   scriptsizemultiplier:
	begin
	create SAN;
	create 'scriptsizemultiplier="';
	create scriptsizemultiplier;
	create '"';
	end;

   scriptminsize:
	begin
	create SAN;
	create 'scriptminsize="';
	create scriptminsize;
	create '"';
	end;

   background\240:
	begin
	create SAN;
	create 'background="';
	create background\240;
	create '"';
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

   subscriptshift:
	begin
	create SAN;
	create 'subscriptshift="';
	create subscriptshift;
	create '"';
	end;

   superscriptshift:
	begin
	create SAN;
	create 'superscriptshift="';
	create superscriptshift;
	create '"';
	end;

   accentunder = true:
	begin
	create SAN;
	create 'accentunder="true"';
	end;
   accentunder = false:
	begin
	create SAN;
	create 'accentunder="false"';
	end;

   align:
	begin
	create SAN;
	create 'align="';
	create align;
	create '"';
	end;

   rowalign:
	begin
	create SAN;
	create 'rowalign="';
	create rowalign;
	create '"';
	end;

   columnalign:
	begin
	create SAN;
	create 'columnalign="';
	create columnalign;
	create '"';
	end;

   groupalign:
	begin
	create SAN;
	create 'groupalign="';
	create groupalign;
	create '"';
	end;

   alignmentscope:
	begin
	create SAN;
	create 'alignmentscope="';
	create alignmentscope;
	create '"';
	end;

   columnwidth:
	begin
	create SAN;
	create 'columnwidth="';
	create columnwidth;
	create '"';
	end;

   rowspacing:
	begin
	create SAN;
	create 'rowspacing="';
	create rowspacing;
	create '"';
	end;

   columnspacing:
	begin
	create SAN;
	create 'columnspacing="';
	create columnspacing;
	create '"';
	end;

   rowlines:
	begin
	create SAN;
	create 'rowlines="';
	create rowlines;
	create '"';
	end;

   columnlines:
	begin
	create SAN;
	create 'columnlines="';
	create columnlines;
	create '"';
	end;

   frame = none\240:
	begin
	create SAN;
	create 'frame="none"';
	end;
   frame = solid\240:
	begin
	create SAN;
	create 'frame="solid"';
	end;
   frame = dashed\240:
	begin
	create SAN;
	create 'frame="dashed"';
	end;

   framespacing:
	begin
	create SAN;
	create 'framespacing="';
	create framespacing;
	create '"';
	end;

   equalrows:
	begin
	create SAN;
	create 'equalrows="';
	create equalrows;
	create '"';
	end;

   equalcolumns:
	begin
	create SAN;
	create 'equalcolumns="';
	create equalcolumns;
	create '"';
	end;

   rowspan\240:
	begin
	create SAN;
	create 'rowspan="';
	create rowspan\240;
	create '"';
	end;

   columnspan:
	begin
	create SAN;
	create 'columnspan="';
	create columnspan;
	create '"';
	end;

   edge = left\240:
	begin
	create SAN;
	create 'edge="left"';
	end;
   edge = right\240:
	begin
	create SAN;
	create 'edge="right"';
	end;

   name:
	begin
	create SAN;
	create 'name="';
	create name;
	create '"';
	end;

   alt:
	begin
	create SAN;
	create 'alt="';
	create alt;
	create '"';
	end;

   index:
	begin
	create SAN;
	create 'index="';
	create index;
	create '"';
	end;

   actiontype:
	begin
	create SAN;
	create 'actiontype="';
	create actiontype;
	create '"';
	end;

   selection:
	begin
	create SAN;
	create 'selection="';
	create selection;
	create '"';
	end;

   EntityName:
	begin
	create EntityName;
	remove;	{ do no output the element content }
	end;

#include "Text_SGML.inc"

TextTRANSLATE Grec
 
	BEGIN
	'\42' -> '&ForAll;';
	'\44' -> '&Exists;';
	'\46' -> '&amp;';
	'''' -> '&SuchThat;';
	'\74' -> '&lt;';
	
	'\100' -> '&Congruent;';
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
	'\134' -> '&Therefore;';
	'\135' -> '&rsqb;';
	'\136' -> '&DownTee;';
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
	'\250' -> '&Diamond;';
	'\251' -> '&hearts;';
	'\252' -> '&spades;';
	'\253' -> '&LeftRightArrow;';
	'\254' -> '&LeftArrow;';
	'\255' -> '&UpArrow;';
	'\256' -> '&RightArrow;';
	'\257' -> '&DownArrow;';
	'\260' -> '&deg;';
	'\261' -> '&PlusMinus;';
	'\262' -> '&rdquo;';
	'\263' -> '&GreaterEqual;';
	'\264' -> '&times;';
	'\265' -> '&Proportional;';
	'\266' -> '&PartialD;';
	'\267' -> '&bull;';
	'\270' -> '&divide;';
	'\271' -> '&NotEqual;';
	'\272' -> '&equiv;';
	'\273' -> '&ap;';
	'\274' -> '&TripleDot;';
	'\275' -> '&Verbar;';
	'\276' -> '&horbar;';
	'\277' -> '&cr;';
	'\300' -> '&aleph;';
	'\301' -> '&ifraktur;';
	'\302' -> '&rfraktur;';
	'\303' -> '&weierp;';
	'\304' -> '&CircleTimes;';
	'\305' -> '&CirclePlus;';
	'\306' -> '&empty;';
	'\307' -> '&Intersection;';
	'\310' -> '&Union;';
	'\311' -> '&Superset;';
	'\312' -> '&SupersetEqual;';
	'\313' -> '&NotSubset;';
	'\314' -> '&Subset;';
	'\315' -> '&SubsetEqual;';
	'\316' -> '&Element;';
	'\317' -> '&NotElement;';
	'\320' -> '&angle;';
	'\321' -> '&dtri;';
	'\322' -> '&regsf;';
	'\323' -> '&copysf;';
	'\324' -> '&trade;';
	'\325' -> '&PI;';
	'\326' -> '&radic;';
	'\327' -> '&CenterDot;';
	'\330' -> '&Not;';
	'\331' -> '&And;';
	'\332' -> '&Or;';
	'\333' -> '&DoubleLeftRightArrow;';
	'\334' -> '&DoubleLeftArrow;';
	'\335' -> '&DoubleUpArrow;';
	'\336' -> '&DoubleRightArrow;';
	'\337' -> '&DoubleDownArrow;';
	'\340' -> '&loz;';
	'\341' -> '&lang;';
	'\342' -> '&regssf;';
	'\343' -> '&copyssf;';
	'\344' -> '&tradessf;';
	'\345' -> '&Sum;';
	'\362' -> '&Integral;';
	END;

SYMBTRANSLATE
        BEGIN
	'r'	-> '&Sqrt;';
	'i'     -> '&Integral;';
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
