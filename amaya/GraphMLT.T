
TRANSLATION GraphML;

LINELENGTH 78;

CONST

#ifdef XML
   SOT = '<g:';		{ Start of opening tag }
   SCT = '</g:';	{ Start of closing tag }
   SAN = ' g:';		{ Start of attribute name }
#else
   SOT = '<';		{ Start of opening tag }
   SCT = '</';		{ Start of closing tag }
   SAN = ' ';		{ Start of attribute name }
#endif
   DoubleQuote = '"';
   CloseTagNL = '>\12';
   CloseEmptyTagNL = '/>\12';

RULES

   GraphML:
	BEGIN
	Create '<svg xmlns="http://www.w3.org/2000/svg"';
	Create Attributes;
	Create '>\12';
	indent +2;
	indent -2 after;
	Create '</svg>\12' After;
	END;

   g:
	BEGIN
	create SOT;
	create 'g';
	create attributes;
	create CloseTagNL;
	indent +2;
	indent -2 after;
	create SCT after;
	create 'g>\12' after;
	END;

   Spline:
	BEGIN
	create SOT;
	create 'spline';
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
		create 'spline>\12' after;
		END;
	END;

   ClosedSpline:
	BEGIN
	create SOT;
	create 'closedspline';
	indent +14;
	create attributes;
	if IntEmptyShape
		create CloseEmptyTagNL;
	indent -14;
	if not IntEmptyShape
		BEGIN
		create CloseTagNL;
		indent +2;
		indent -2 after;
		create SCT after;
		create 'closedspline>\12' after;
		END;
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

   text_:
	BEGIN
	create SOT;
	create 'text';
	create attributes;
	create '>';
	indent +2;
	indent -2 after;
	create SCT after;
	create 'text>\12' after;
	END;

   tspan:
	BEGIN
	create SOT;
	create 'tspan';
	create attributes;
	create '>';
	indent +2;
	indent -2 after;
	create SCT after;
	create 'tspan>\12' after;
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

   XMLcomment:
	BEGIN
	NoLineBreak;
        Create '<!--';
	NoTranslation;
        Create '-->\12' after;
        END;

   XMLcomment_line:
	if not last create '\12' after;

   HTML:
	Use HTMLT;

   MathML:
	Use MathMLT;

   TEXT_UNIT:
        if immediately within text_
	    create '\12' after;

   GRAPHICS_UNIT:
	Remove;

   PICTURE_UNIT:
	Remove;

ATTRIBUTES

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
 
   Namespace:
	begin
        create SAN;
        create 'xmlns="';
        create Namespace;
        create DoubleQuote;
	end;

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

   height_:
	BEGIN
	create SAN;
	create 'height="';
	create height_;
	create DoubleQuote;
	END;

   width_:
	BEGIN
	create SAN;
	create 'width="';
	create width_;
	create DoubleQuote;
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

   transform:
	BEGIN
	create SAN;
	create 'transform="';
	create transform;
	create DoubleQuote;
	END;

   linestyle_ = solid_:
	BEGIN
	create SAN;
	create 'linestyle="solid"';
	END;
   linestyle_ = dashed_:
	BEGIN
	create SAN;
	create 'linestyle="dashed"';
	END;
   linestyle_ = dotted_:
	BEGIN
	create SAN;
	create 'linestyle="dotted"';
	END;

   points:
	BEGIN
	create SAN;
	create 'points="';
	create points;
	create DoubleQuote;
	END;

   arrowhead = none_:
	BEGIN
	create SAN;
	create 'arrowhead="none"';
	END;
   arrowhead = start:
	BEGIN
	create SAN;
	create 'arrowhead="start"';
	END;
   arrowhead = end_:
	BEGIN
	create SAN;
	create 'arrowhead="end"';
	END;
   arrowhead = both:
	BEGIN
	create SAN;
	create 'arrowhead="both"';
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

   xlink_href:
	BEGIN
	create SAN;
	create 'xlink:href="';
	create xlink_href;
	create DoubleQuote;
	END;

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

END
