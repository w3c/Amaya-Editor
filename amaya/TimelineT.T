
TRANSLATION Timeline;

LINELENGTH 78;

CONST

   SOT = '<';		{ Start of opening tag }
   SCT = '</';		{ Start of closing tag }
   SAN = ' ';		{ Start of attribute name }
   SUA = '';        { Start of unknown attribute }

   DoubleQuote = '"';
   CloseTagNL = '>\12';
   CloseEmptyTagNL = '/>\12';

RULES



   Timeline:
	BEGIN
	Use HTMLTX for HTML;
        Use MathMLT for MathML;
        Use XLinkT for XLink;
	Create '<svg xmlns="http://www.w3.org/2000/svg"';
        indent +5;
	Create Attributes;
	Create CloseTagNL;
	indent -3;
	indent -2 after;
	Create '</svg>\12' After;
	END;


  abstract_group:
	BEGIN
	create SOT;
	create 'g';
	indent +2;
	create attributes;
	create CloseTagNL;
	indent -2 after;
	create SCT after;
	create 'g>\12' after;
	END;
  
   image_slider:
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

   image_collapse:
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
  

 image_help:
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
 
   col_period:
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


  exp_period:
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

   rect_interface:
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


  rect_id:
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


 
   tline:
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

 text_id:
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

   timing_text:
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

   
   TEXT_UNIT:
        if immediately within text_
	    create '\12' after;

   GRAPHICS_UNIT:
	Remove;

   PICTURE_UNIT:
	Remove;

   Timeline_Image: BEGIN
	Remove;
	END;


ATTRIBUTES


   id:
        begin
        create SAN;
        create 'id="';
        create id;
        create DoubleQuote;
        end;

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

   xlink_href:
	BEGIN
	create SAN;
	create 'xlink:href="';
	create xlink_href;
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


TextTRANSLATE
	BEGIN
	'\46' -> '&amp;';	{ ampersand, U+0026 ISOnum }
	'\74' -> '&lt;';	{ less-than sign, U+003C ISOnum }
	'\76' -> '&gt;'; 	{ greater-than sign, U+003E ISOnum }
	END;
END
