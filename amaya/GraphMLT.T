
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

   Line_:
	BEGIN
	create SOT;
	create 'line';
	create attributes;
	create CloseEmptyTagNL;
	END;

   Rectangle:
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

   Label:
	BEGIN
	create SOT;
	create 'label';
	create attributes;
	create '>';
	indent +2;
	indent -2 after;
	create SCT after;
	create 'label>\12' after;
	END;

   MathML:
	Use MathMLT;

   RoundRect:
	BEGIN
	create SOT;
	create 'roundrect';
	indent +11;
	create attributes;
	if IntEmptyShape
		create CloseEmptyTagNL;
	indent -11;
	if not IntEmptyShape
		BEGIN
		create CloseTagNL;
		indent +2;
		indent -2 after;
		create SCT after;
		create 'roundrect>\12' after;
		END;
	END;

   Circle:
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

   Oval:
	BEGIN
	create SOT;
	create 'oval';
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
		create 'oval>\12' after;
		END;
	END;

   Polyline:
	BEGIN
	create SOT;
	create 'polyline';
	create attributes;
	create CloseEmptyTagNL;
	END;

   Polygon:
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

   Spline:
	BEGIN
	create SOT;
	create 'spline';
	create attributes;
	create CloseEmptyTagNL;
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

   Text_:
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

   Math:
	BEGIN
	create SOT;
	create 'math';
	create attributes;
	create CloseTagNL;
	indent +2;
	indent -2 after;
	create SCT after;
	create 'math>\12' after;
	END;

   Group:
	BEGIN
	create SOT;
	create 'group';
	create attributes;
	create CloseTagNL;
	indent +2;
	indent -2 after;
	create SCT after;
	create 'group>\12' after;
	END;

   GRAPHICS_UNIT:
	    Remove;

ATTRIBUTES

   class:
        begin
        create SAN;
        create 'class="';
        create class;
        create DoubleQuote;
        end;
 
   style\240:
        begin
        create SAN;
        create 'style="';
        create style\240;
        create DoubleQuote;
        end;
 
   id:
        begin
        create SAN;
        create 'id="';
        create id;
        create DoubleQuote;
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

   linewidth:
	BEGIN
	create SAN;
	create 'linewidth="';
	create linewidth;
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

   align = left_:
	BEGIN
	create SAN;
	create 'align="left"';
	END;
   align = center_:
	BEGIN
	create SAN;
	create 'align="center"';
	END;
   align = right_:
	BEGIN
	create SAN;
	create 'align="right"';
	END;

   valign = top_:
	BEGIN
	create SAN;
	create 'valign="top"';
	END;
   valign = middle:
	BEGIN
	create SAN;
	create 'valign="middle"';
	END;
   valign = bottom_:
	BEGIN
	create SAN;
	create 'valign="bottom"';
	END;

   arrowhead = none:
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

   depth_:
	BEGIN
	create SAN;
	create 'depth="';
	create depth_;
	create DoubleQuote;
	END;

   position:
	BEGIN
	create SAN;
	create 'position="';
	create position;
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

   points:
	BEGIN
	create SAN;
	create 'points="';
	create points;
	create DoubleQuote;
	END;

   direction = left_:
	BEGIN
	create SAN;
	create 'direction="left"';
	END;
   direction = right_:
	BEGIN
	create SAN;
	create 'direction="right"';
	END;
   direction = up:
	BEGIN
	create SAN;
	create 'direction="up"';
	END;
   direction = down:
	BEGIN
	create SAN;
	create 'direction="down"';
	END;

   hspace:
	BEGIN
	create SAN;
	create 'hspace="';
	create hspace;
	create DoubleQuote;
	END;

   vspace:
	BEGIN
	create SAN;
	create 'vspace="';
	create vspace;
	create DoubleQuote;
	END;

END

