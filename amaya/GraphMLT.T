
TRANSLATION GraphML;

LINELENGTH 78;

CONST

   DoubleQuote = '"';
   CloseTagNL = '>\12';
   CloseEmptyTagNL = '/>\12';

RULES

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
	create '<line';
	create attributes;
	create CloseEmptyTagNL;
	END;

   Rectangle:
	BEGIN
	create '<rect';
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
		create '</rect>\12' after;
		END;
	END;

   Label:
	BEGIN
	create '<label';
	create attributes;
	create '>';
	indent +2;
	indent -2 after;
	create '</label>\12' after;
	END;

   MathML:
	Use MathMLT;

   RoundRect:
	BEGIN
	create '<roundrect';
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
		create '</roundrect>\12' after;
		END;
	END;

   Circle:
	BEGIN
	create '<circle';
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
		create '</circle>\12' after;
		END;
	END;

   Oval:
	BEGIN
	create '<oval';
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
		create '</oval>\12' after;
		END;
	END;

   Polyline:
	BEGIN
	create '<polyline';
	create attributes;
	create CloseEmptyTagNL;
	END;

   Polygon:
	BEGIN
	create '<polygon';
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
		create '</polygon>\12' after;
		END;
	END;

   Spline:
	BEGIN
	create '<spline';
	create attributes;
	create CloseEmptyTagNL;
	END;

   ClosedSpline:
	BEGIN
	create '<closedspline';
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
		create '</closedspline>\12' after;
		END;
	END;

   Text_:
	BEGIN
	create '<text';
	create attributes;
	create '>';
	indent +2;
	indent -2 after;
	create '</text>\12' after;
	END;

   Math:
	BEGIN
	create '<math';
	create attributes;
	create CloseTagNL;
	indent +2;
	indent -2 after;
	create '</math>\12' after;
	END;

   Group:
	BEGIN
	create '<group';
	create attributes;
	create CloseTagNL;
	indent +2;
	indent -2 after;
	create '</group>\12' after;
	END;

   GRAPHICS_UNIT:
	    Remove;

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

   linewidth:
	BEGIN
	create ' linewidth="';
	create linewidth;
	create DoubleQuote;
	END;

   linestyle_ = solid_:
	create ' linestyle="solid"';
   linestyle_ = dashed_:
	create ' linestyle="dashed"';
   linestyle_ = dotted_:
	create ' linestyle="dotted"';

   lid:
	BEGIN
	create ' lid="';
	create lid;
	create DoubleQuote;
	END;

   align = left_:
	create ' align="left"';
   align = center_:
	create ' align="center"';
   align = right_:
	create ' align="right"';

   valign = top_:
	create ' valign="top"';
   valign = middle:
	create ' valign="middle"';
   valign = bottom_:
	create ' valign="bottom"';

   arrowhead = none:
	create ' arrowhead="none"';
   arrowhead = start:
	create ' arrowhead="start"';
   arrowhead = end_:
	create ' arrowhead="end"';
   arrowhead = both:
	create ' arrowhead="both"';

   depth_:
	BEGIN
	create ' depth="';
	create depth_;
	create DoubleQuote;
	END;

   position:
	BEGIN
	create ' position="';
	create position;
	create DoubleQuote;
	END;

   width_:
	BEGIN
	create ' width="';
	create width_;
	create DoubleQuote;
	END;

   height_:
	BEGIN
	create ' height="';
	create height_;
	create DoubleQuote;
	END;

   points:
	BEGIN
	create ' points="';
	create points;
	create DoubleQuote;
	END;

   direction = left_:
	create ' direction="left"';
   direction = right_:
	create ' direction="right"';
   direction = up:
	create ' direction="up"';
   direction = down:
	create ' direction="down"';

   hspace:
	BEGIN
	create ' hspace="';
	create hspace;
	create DoubleQuote;
	END;

   vspace:
	BEGIN
	create ' vspace="';
	create vspace;
	create DoubleQuote;
	END;

END

