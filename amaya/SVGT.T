
TRANSLATION SVG;

LINELENGTH 78;

CONST

#ifdef XML
   SOT = '<g:';		{ Start of opening tag }
   SCT = '</g:';	{ Start of closing tag }
   SAN = ' g:';		{ Start of attribute name }
   SUA = 'g:';          { Start of unknown attribute }
#else
   SOT = '<';		{ Start of opening tag }
   SCT = '</';		{ Start of closing tag }
   SAN = ' ';		{ Start of attribute name }
   SUA = '';            { Start of unknown attribute }
#endif
   DoubleQuote = '"';
   CloseTagNL = '>\12';
   CloseEmptyTagNL = '/>\12';

RULES

   Document:
	BEGIN
        if Charset
           Create '<?xml version="1.0"';
        if Charset and not Charset = 'unknown'
           BEGIN
           Create ' encoding="';
	   Create Charset;
	   Create '"';
           END;
        if Charset
           Create '?>\12';
	END;

   SVG:
	BEGIN
        if within use_
	   Ignore;
	Use HTMLTX for HTML;
        Use MathMLT for MathML;
        Use XLinkT for XLink;
	Create '<svg xmlns="http://www.w3.org/2000/svg"';
	Create Attributes;
	Create '>\12';
	indent +2;
	indent -2 after;
	Create '</svg>\12' After;
	END;

   desc:
	BEGIN
	create SOT;
	create 'desc';
	create attributes;
	create '>';
	indent +2;
	indent -2 after;
	create SCT after;
	create 'desc>\12' after;
	END;

   title:
	BEGIN
	create SOT;
	create 'title';
	create attributes;
	create '>';
	indent +2;
	indent -2 after;
	create SCT after;
	create 'title>\12' after;
	END;

   metadata:
	BEGIN
	create SOT;
	create 'metadata';
	create attributes;
	create '>';
	indent +2;
	indent -2 after;
	create SCT after;
	create 'metadata>\12' after;
	END;

   g:
	BEGIN
	if within use_
           Ignore;
	create SOT;
	create 'g';
	create attributes;
	create CloseTagNL;
	indent +2;
	indent -2 after;
	create SCT after;
	create 'g>\12' after;
	END;

   defs:
	BEGIN
	create SOT;
	create 'defs';
	create attributes;
	create CloseTagNL;
	indent +2;
	indent -2 after;
	create SCT after;
	create 'defs>\12' after;
	END;

   symbol_:
	BEGIN
	if within use_
           Ignore;
	create SOT;
	create 'symbol';
	create attributes;
	create CloseTagNL;
	indent +2;
	indent -2 after;
	create SCT after;
	create 'symbol>\12' after;
	END;

   use_:
	BEGIN
	create SOT;
	create 'use';
	create attributes;
	if IntEmptyShape
		create CloseEmptyTagNL;
	if not IntEmptyShape
		BEGIN
		create CloseTagNL;
		indent +2;
		indent -2 after;
		create SCT after;
		create 'use>\12' after;
		END;
	END;

   image:
	BEGIN
	create SOT;
	create 'image';
	indent +7;
	create attributes;
	if IntEmptyShape
		create CloseEmptyTagNL;
	indent -7;	
	if not IntEmptyShape
		BEGIN
		create CloseTagNL;
		indent +2;
		indent -2 after;
		create SCT after;
		create 'image>\12' after;
		END;
	END;

   switch:
	BEGIN
	create SOT;
	create 'switch';
	create attributes;
	create CloseTagNL;
	indent +2;
	indent -2 after;
	create SCT after;
	create 'switch>\12' after;
	END;

   style__:
	BEGIN
	create SOT;
	create 'style';
	create attributes;
	create '><![CDATA[\12';
	indent +2;
	indent -2 after;
        create '\12]]>' after;
	create SCT after;
	create 'style>\12' after;
	END;

   rect:
	BEGIN
	create SOT;
	create 'rect';
	indent +6;
	create attributes;
	if IntEmptyShape
		create CloseEmptyTagNL;
	indent -6;
	if not IntEmptyShape
		BEGIN
		create CloseTagNL;
		indent +2;
		indent -2 after;
		create SCT after;
		create 'rect>\12' after;
		END;
	END;

   circle:
	BEGIN
	create SOT;
	create 'circle';
	indent +8;
	create attributes;
	if IntEmptyShape
		create CloseEmptyTagNL;
	indent -8;
	if not IntEmptyShape
		BEGIN
		create CloseTagNL;
		indent +2;
		indent -2 after;
		create SCT after;
		create 'circle>\12' after;
		END;
	END;

   line_:
	BEGIN
	create SOT;
	create 'line';
	indent +6;
	create attributes;
	if IntEmptyShape
		create CloseEmptyTagNL;
	indent -6;
	if not IntEmptyShape
		BEGIN
		create CloseTagNL;
		indent +2;
		indent -2 after;
		create SCT after;
		create 'line>\12' after;
		END;
	END;

   ellipse:
	BEGIN
	create SOT;
	create 'ellipse';
	indent +6;
	create attributes;
	if IntEmptyShape
		create CloseEmptyTagNL;
	indent -6;
	if not IntEmptyShape
		BEGIN
		create CloseTagNL;
		indent +2;
		indent -2 after;
		create SCT after;
		create 'ellipse>\12' after;
		END;
	END;

   polyline:
	BEGIN
	create SOT;
	create 'polyline';
	indent +10;
	create attributes;
	if IntEmptyShape
		create CloseEmptyTagNL;
	indent -10;
	if not IntEmptyShape
		BEGIN
		create CloseTagNL;
		indent +2;
		indent -2 after;
		create SCT after;
		create 'polyline>\12' after;
		END;
	END;

   polygon:
	BEGIN
	create SOT;
	create 'polygon';
	indent +9;
	create attributes;
	if IntEmptyShape
		create CloseEmptyTagNL;
	indent -9;
	if not IntEmptyShape
		BEGIN
		create CloseTagNL;
		indent +2;
		indent -2 after;
		create SCT after;
		create 'polygon>\12' after;
		END;
	END;

   path:
	BEGIN
	create SOT;
	create 'path';
	indent +6;
	create attributes;
	if IntEmptyShape
		create CloseEmptyTagNL;
	indent -6;
	if not IntEmptyShape
		BEGIN
		create CloseTagNL;
		indent +2;
		indent -2 after;
		create SCT after;
		create 'path>\12' after;
		END;
	END;

   text_:
	BEGIN
	create SOT;
	create 'text';
	indent +6;
	create attributes;
	create '>';
	indent -4;
	indent -2 after;
	create SCT after;
	create 'text>\12' after;
	END;

   tspan:
	BEGIN
	create SOT;
	create 'tspan';
	indent +7;
	create attributes;
	create '>';
	indent -5;
	indent -2 after;
	create SCT after;
	create 'tspan>\12' after;
	END;

   a:
	BEGIN
	create SOT;
	create 'a';
	create attributes;
	create CloseTagNL;
	indent +2;
	indent -2 after;
	create SCT after;
	create 'a>\12' after;
	END;

   script_:
	BEGIN
	create SOT;
	create 'script';
	create attributes;
	create '><![CDATA[\12';
	indent +2;
	indent -2 after;
        create '\12]]>' after;
	create SCT after;
	create 'script>\12' after;
	END;

   foreignObject:
	BEGIN
	create SOT;
	create 'foreignObject';
	create attributes;
	create CloseTagNL;
	indent +2;
	indent -2 after;
	create SCT after;
	create 'foreignObject>\12' after;
	END;

   DOCTYPE: BEGIN
	NoLineBreak;
	Create '<!DOCTYPE ';
	NoTranslation;
	Create '>\12' After;
	END;

   DOCTYPE_line:
	if not last create '\12' after;

   XMLcomment:
	BEGIN
	NoLineBreak;
        Create '<!--';
	NoTranslation;
        Create '-->\12' after;
        END;

   XMLcomment_line:
	if not last create '\12' after;

   XMLPI:
	BEGIN
	NoLineBreak;
        Create '<?';
	NoTranslation;
        Create '?>\12' after;
        END;

   XMLPI_line:
	if not last create '\12' after;

   CDATA: BEGIN
	NoLineBreak;
	Create '<![CDATA[';
	NoTranslation;
	Create ']]>\12' After;
	END;

   CDATA_line:
	if not last create '\12' after;

   Unknown_namespace:
	BEGIN
	NoTranslation;
	Create '\12';
        Create Content;
	Remove;
	Create Attributes;
	Create '>';
	END;

   TEXT_UNIT:
        if immediately within text_
	    create '\12' after;

   GRAPHICS_UNIT:
	Remove;

   PICTURE_UNIT:
	Remove;

   SVG_Image: BEGIN
	Remove;
	END;

ATTRIBUTES

 Language:
	begin
	if not TEXT_UNIT and not SVG
	    begin
	    Create ' xml:lang="';
	    Create Language;
	    Create DoubleQuote;
	    end;
	if SVG and RealLang
	    begin
	    Create ' xml:lang="';
	    Create Language;
	    Create DoubleQuote;
	    end;
	end;

   id:
        begin
        create SAN;
        create 'id="';
        create id;
        create DoubleQuote;
        end;
 
   class:
        begin
        create SAN;
        create 'class="';
        create class;
        create DoubleQuote;
        end;
 
   style_:
        begin
        create SAN;
        create 'style="';
        create style_;
        create DoubleQuote;
        end;
 
   xml_space = xml_space_default:
	BEGIN
	create SAN;
	create 'xml:space="default"';
	END;
   xml_space = xml_space_preserve:
	BEGIN
	create SAN;
	create 'xml:space="preserve"';
	END;

   Namespace:
	begin
        create SAN;
        create 'xmlns="';
        create Namespace;
        create DoubleQuote;
	end;

   requiredFeatures:
	BEGIN
	create SAN;
	create 'requiredFeatures="';
	create requiredFeatures;
	create DoubleQuote;
	END;

   requiredExtensions:
	BEGIN
	create SAN;
	create 'requiredExtensions="';
	create requiredExtensions;
	create DoubleQuote;
	END;

   systemLanguage:
	BEGIN
	create SAN;
	create 'systemLanguage="';
	create systemLanguage;
	create DoubleQuote;
	END;

   externalResourcesRequired = false:
	BEGIN
	create SAN;
	create 'externalResourcesRequired="false"';
	END;
   externalResourcesRequired = true:
	BEGIN
	create SAN;
	create 'externalResourcesRequired="true"';
	END;

   fill:
	BEGIN
	create SAN;
	create 'fill="';
	create fill;
	create DoubleQuote;
	END;

   stroke:
	BEGIN
	create SAN;
	create 'stroke="';
	create stroke;
	create DoubleQuote;
	END;

   stroke_width:
	BEGIN
	create SAN;
	create 'stroke-width="';
	create stroke_width;
	create DoubleQuote;
	END;

   font_family:
	BEGIN
	create SAN;
	create 'font-family="';
	create font_family;
	create DoubleQuote;
	END;

   font_size:
	BEGIN
	create SAN;
	create 'font-size="';
	create font_size;
	create DoubleQuote;
	END;

   font_style = normal_:
	BEGIN
	create SAN;
	create 'font-style="normal"';
	END;
   font_style = italic:
	BEGIN
	create SAN;
	create 'font-style="italic"';
	END;
   font_style = oblique_:
	BEGIN
	create SAN;
	create 'font-style="oblique"';
	END;
   font_style = inherit:
	BEGIN
	create SAN;
	create 'font-style="inherit"';
	END;

   font_variant = normal_:
	BEGIN
	create SAN;
	create 'font-variant="normal"';
	END;
   font_variant = small_caps:
	BEGIN
	create SAN;
	create 'font-variant="small-caps"';
	END;
   font_variant = inherit:
	BEGIN
	create SAN;
	create 'font-variant="inherit"';
	END;

   font_weight = normal_:
	BEGIN
	create SAN;
	create 'font-weight="normal"';
	END;
   font_weight = bold_:
	BEGIN
	create SAN;
	create 'font-weight="bold"';
	END;
   font_weight = bolder:
	BEGIN
	create SAN;
	create 'font-weight="bolder"';
	END;
   font_weight = lighter:
	BEGIN
	create SAN;
	create 'font-weight="lighter"';
	END;
   font_weight = w100:
	BEGIN
	create SAN;
	create 'font-weight="100"';
	END;
   font_weight = w200:
	BEGIN
	create SAN;
	create 'font-weight="200"';
	END;
   font_weight = w300:
	BEGIN
	create SAN;
	create 'font-weight="300"';
	END;
   font_weight = w400:
	BEGIN
	create SAN;
	create 'font-weight="400"';
	END;
   font_weight = w500:
	BEGIN
	create SAN;
	create 'font-weight="500"';
	END;
   font_weight = w600:
	BEGIN
	create SAN;
	create 'font-weight="600"';
	END;
   font_weight = w700:
	BEGIN
	create SAN;
	create 'font-weight="700"';
	END;
   font_weight = w800:
	BEGIN
	create SAN;
	create 'font-weight="800"';
	END;
   font_weight = w900:
	BEGIN
	create SAN;
	create 'font-weight="900"';
	END;
   font_weight = inherit:
	BEGIN
	create SAN;
	create 'font-weight="inherit"';
	END;

   direction_ = ltr_:
	BEGIN
	create SAN;
	create 'direction="ltr"';
	END;
   direction_ = rtl_:
	BEGIN
	create SAN;
	create 'direction="rtl"';
	END;
   direction_ = inherit:
	BEGIN
	create SAN;
	create 'direction="inherit"';
	END;

   text_decoration:
	BEGIN
	create SAN;
	create 'text-decoration="';
	create text_decoration;
	create DoubleQuote;
	END;

   unicode_bidi = normal_:
	BEGIN
	create SAN;
	create 'unicode-bidi="normal"';
	END;
   unicode_bidi = embed_:
	BEGIN
	create SAN;
	create 'unicode-bidi="embed"';
	END;
   unicode_bidi = bidi_override:
	BEGIN
	create SAN;
	create 'unicode-bidi="bidi-override"';
	END;
   unicode_bidi = inherit:
	BEGIN
	create SAN;
	create 'unicode-bidi="inherit"';
	END;

   viewBox:
	BEGIN
	create SAN;
	create 'viewBox="';
	create viewBox;
	create DoubleQuote;
	END;

   x:
	BEGIN
	create SAN;
	create 'x="';
	create x;
	create DoubleQuote;
	END;

   y:
	BEGIN
	create SAN;
	create 'y="';
	create y;
	create DoubleQuote;
	END;

   width_:
	BEGIN
	create SAN;
	create 'width="';
	create width_;
	create DoubleQuote;
	END;

   height_:
	BEGIN
	create SAN;
	create 'height="';
	create height_;
	create DoubleQuote;
	END;

   transform:
	BEGIN
	create SAN;
	create 'transform="';
	create transform;
	create DoubleQuote;
	END;

   preserveAspectRatio:
	BEGIN
	create SAN;
	create 'preserveAspectRatio="';
	create preserveAspectRatio;
	create DoubleQuote;
	END;

   xlink_href:
	BEGIN
	create SAN;
	create 'xlink:href="';
	create xlink_href;
	create DoubleQuote;
	END;

   type:
	BEGIN
	create SAN;
	create 'type="';
	create type;
	create DoubleQuote;
	END;

   media:
	BEGIN
	create SAN;
	create 'media="';
	create media;
	create DoubleQuote;
	END;

   title_:
	BEGIN
	create SAN;
	create 'title="';
	create title_;
	create DoubleQuote;
	END;

   rx:
	BEGIN
	create SAN;
	create 'rx="';
	create rx;
	create DoubleQuote;
	END;

   ry:
	BEGIN
	create SAN;
	create 'ry="';
	create ry;
	create DoubleQuote;
	END;

   cx:
	BEGIN
	create SAN;
	create 'cx="';
	create cx;
	create DoubleQuote;
	END;

   cy:
	BEGIN
	create SAN;
	create 'cy="';
	create cy;
	create DoubleQuote;
	END;

   r:
	BEGIN
	create SAN;
	create 'r="';
	create r;
	create DoubleQuote;
	END;

   x1:
	BEGIN
	create SAN;
	create 'x1="';
	create x1;
	create DoubleQuote;
	END;

   y1:
	BEGIN
	create SAN;
	create 'y1="';
	create y1;
	create DoubleQuote;
	END;

   x2:
	BEGIN
	create SAN;
	create 'x2="';
	create x2;
	create DoubleQuote;
	END;

   y2:
	BEGIN
	create SAN;
	create 'y2="';
	create y2;
	create DoubleQuote;
	END;

   points:
	BEGIN
	create SAN;
	create 'points="';
	create points;
	create DoubleQuote;
	END;

   d:
	BEGIN
	create SAN;
	create 'd="';
	create d;
	create DoubleQuote;
	END;

   pathLength:
	BEGIN
	create SAN;
	create 'pathLength="';
	create pathLength;
	create DoubleQuote;
	END;

   dx:
	BEGIN
	create SAN;
	create 'dx="';
	create dx;
	create DoubleQuote;
	END;

   dy:
	BEGIN
	create SAN;
	create 'dy="';
	create dy;
	create DoubleQuote;
	END;

   target_:
	BEGIN
	create SAN;
	create 'target="';
	create target_;
	create DoubleQuote;
	END;

   Unknown_attribute:
	BEGIN
	create SUA;
	Create Unknown_attribute;
	END;	

TextTRANSLATE
	BEGIN
{ 	'\42' -> '&quot;'; }	{ quotation mark = APL quote, U+0022 ISOnum }
	'\46' -> '&amp;';	{ ampersand, U+0026 ISOnum }
	'\74' -> '&lt;';	{ less-than sign, U+003C ISOnum }
	'\76' -> '&gt;'; 	{ greater-than sign, U+003E ISOnum }
	END;

TextTRANSLATE Grec
 
	BEGIN
	'\42' -> '&ForAll;';
	'\44' -> '&Exists;';
	'\46' -> '&amp;';
	'''' -> '&SuchThat;';
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
        '\242' -> '&prime;';
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
	'\262' -> '&Prime;';
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
	'\325' -> '&Pi;';
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
        '\361' -> '&rang;';
	'\362' -> '&Integral;';
	END;

END
