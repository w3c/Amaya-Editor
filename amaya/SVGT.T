
TRANSLATION SVG;

LINELENGTH 78;

BUFFERS
   ElemPrefixBuffer (variable);
   AttrPrefixBuffer (variable);

CONST
   DoubleQuote = '"';
   CloseTagNL = '>\12';
   CloseEmptyTagNL = '/>\12';

VAR
   SOT : '<' ElemPrefixBuffer;	{ Start of opening tag }
   SCT : '</' ElemPrefixBuffer;	{ Start of closing tag }
   SAN : ' ' AttrPrefixBuffer;  { Start of attribute name }
   SUA : '';                    { Start of unknown attribute }

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
	create SOT;
	create 'svg';
        indent +5;
	Create Attributes;
	Create CloseTagNL;
	indent -3;
	indent -2 after;
	create SCT after;
	create 'svg>\12' after;
	END;

   g:
	BEGIN
	if within use_
           Ignore;
	create SOT;
	create 'g';
	indent +2;
	create attributes;
	create CloseTagNL;
	indent -2 after;
	create SCT after;
	create 'g>\12' after;
	END;

   defs:
	BEGIN
	create SOT;
	create 'defs';
	indent +6;
	create attributes;
	create CloseTagNL;
	indent -4;
	indent -2 after;
	create SCT after;
	create 'defs>\12' after;
	END;

   desc:
	BEGIN
	create SOT;
	create 'desc';
	indent +6;
	create attributes;
	create '>';
	indent -4;
	indent -2 after;
	create SCT after;
	create 'desc>\12' after;
	END;

   title:
	BEGIN
	create SOT;
	create 'title';
	indent +7;
	create attributes;
	create '>';
	indent -5;
	indent -2 after;
	create SCT after;
	create 'title>\12' after;
	END;

   symbol_:
	BEGIN
	if within use_
           Ignore;
	create SOT;
	create 'symbol';
	indent +8;
	create attributes;
	create CloseTagNL;
	indent -6;
	indent -2 after;
	create SCT after;
	create 'symbol>\12' after;
	END;

   use_:
	BEGIN
	create SOT;
	create 'use';
	indent +5;
	create attributes;
	if IntEmptyShape
		create CloseEmptyTagNL;
	indent -5;
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
	if within use_
           Ignore;
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
	if within use_
           Ignore;
	create SOT;
	create 'switch';
	indent +8;
	create attributes;
	create CloseTagNL;
	indent -6;
	indent -2 after;
	create SCT after;
	create 'switch>\12' after;
	END;

   style__:
	BEGIN
	create SOT;
	create 'style';
	indent +7;
	create attributes;
	create CloseTagNL;
	indent -5;
	indent -2 after;
	create SCT after;
	create 'style>\12' after;
	END;

   path:
	BEGIN
	if within use_
           Ignore;
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

   rect:
	BEGIN
	if within use_
           Ignore;
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
	if within use_
           Ignore;
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
	if within use_
           Ignore;
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
	if within use_
           Ignore;
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
	if within use_
           Ignore;
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
	if within use_
           Ignore;
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
	if within use_
           Ignore;
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
	if within use_
           Ignore;
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

   tref:
	BEGIN
	if within use_
           Ignore;
	create SOT;
	create 'tref';
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
		create 'tref>\12' after;
		END;
	END;

   clipPath:
	BEGIN
	create SOT;
	create 'clipPath';
	indent +10;
	create attributes;
	create CloseTagNL;
	indent -8;
	indent -2 after;
	create SCT after;
	create 'clipPath>\12' after;
	END;

   a:
	BEGIN
	if within use_
           Ignore;
	create SOT;
	create 'a';
	indent +3;
	create attributes;
	create CloseTagNL;
	indent -1;
	indent -2 after;
	create SCT after;
	create 'a>\12' after;
	END;

   script_:
	BEGIN
	if within use_
           Ignore;
	create SOT;
	create 'script';
	indent +8;
	create attributes;
	create CloseTagNL;
	indent -6;
	indent -2 after;
	create SCT after;
	create 'script>\12' after;
	END;
 
   animate:
	BEGIN
	if within use_
           Ignore;
	create SOT;
	create 'animate';
 	indent +9;
	create attributes;
	if empty
		create CloseEmptyTagNL;
        indent -9;
	if not empty
		BEGIN
		create CloseTagNL;
		indent +2;
		indent -2 after;
		create SCT after;
		create 'animate>\12' after;
		END;
	END;

   set_:
	BEGIN
	if within use_
           Ignore;
	create SOT;
	create 'set';
	indent +5;
	create attributes;
	if empty
		create CloseEmptyTagNL;
        indent -5;
	if not empty
		BEGIN
		create CloseTagNL;
		indent +2;
		indent -2 after;
		create SCT after;
		create 'set>\12' after;		
		END;
	END;

   animateMotion:
	BEGIN
	if within use_
           Ignore;
	create SOT;
	create 'animateMotion';
        indent +15;
	create attributes;
	if empty
		create CloseEmptyTagNL;
        indent -15;
	if not empty
		BEGIN
		create CloseTagNL;
		indent +2;
		indent -2 after;
		create SCT after;
		create 'animateMotion>\12' after;
		END;	
	END;

   mpath:
	BEGIN
	if within use_
           Ignore;
	create SOT;
	create 'mpath';
	indent +7;
	create attributes;
	if empty
		create CloseEmptyTagNL;
        indent -7;
	if not empty
		BEGIN
		create CloseTagNL;
		indent +2;
		indent -2 after;
		create SCT after;
		create 'mpath>\12' after;
		END;	
	END;

   animateColor:
	BEGIN
	if within use_
           Ignore;
	create SOT;
	create 'animateColor';
 	indent +14;
	create attributes;
	if empty
		create CloseEmptyTagNL;
        indent -14;
	if not empty
		BEGIN
		create CloseTagNL;
		indent +2;
		indent -2 after;
		create SCT after;
		create 'animateColor>\12' after;
		END;	
	END;

    linearGradient:
	BEGIN
	if within use_
           Ignore;
	create SOT;
	create 'linearGradient';
	indent +16;
	create attributes;
	if empty
		create CloseEmptyTagNL;
        indent -16;
	if not empty
		BEGIN
		create CloseTagNL;
		indent +2;
		indent -2 after;
		create SCT after;
		create 'linearGradient>\14' after;
		END;
	END;

    stop:
	BEGIN
	if within use_
           Ignore;
	create SOT;
	create 'stop';
	indent +6;
	create attributes;
	if empty
		create CloseEmptyTagNL;
        indent -6;
	if not empty
		BEGIN
		create CloseTagNL;
		indent +2;
		indent -2 after;
		create SCT after;
		create 'stop>\4' after;
		END;
	END;

   animateTransform:
	BEGIN
	if within use_
           Ignore;
	create SOT;
	create 'animateTransform';
	indent +18;
	create attributes;
	if empty
		create CloseEmptyTagNL;
        indent -18;
	if not empty
		BEGIN
		create CloseTagNL;
		indent +2;
		indent -2 after;
		create SCT after;
		create 'animateTransform>\12' after;
		END;
	END;

   metadata:
	BEGIN
	create SOT;
	create 'metadata';
	indent +10;
	create attributes;
	create '>';
	indent -8;
	indent -2 after;
	create SCT after;
	create 'metadata>\12' after;
	END;

   foreignObject:
	BEGIN
	if within use_
           Ignore;
	create SOT;
	create 'foreignObject';
	indent +15;
	create attributes;
	create CloseTagNL;
	indent -13;
	indent -2 after;
	create SCT after;
	create 'foreignObject>\12' after;
	END;

   DOCTYPE: BEGIN
	NoLineBreak;
	NoTranslation;
	Create '\12' After;
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
	create '>';
	END;

   TEXT_UNIT:
	BEGIN
	if within use_
           Ignore;
	if within tref and
           not within desc and not within metadata and not within title
            Ignore;
        if immediately within text_
	    create '\12' after;
	END;

   GRAPHICS_UNIT:
	Remove;

   PICTURE_UNIT:
	Remove;

   SVG_Image: BEGIN
	Remove;
	END;

   Timeline_cross:
	BEGIN
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
	{
        create 'xmlns="';
        create Namespace;
        create DoubleQuote;
        }
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

   baseProfile:
	BEGIN
	create SAN;
	create 'baseProfile="';
	create baseProfile;
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

   text_anchor = start:
	BEGIN
	create SAN;
	create 'text-anchor="start"';
	END;
   text_anchor = middle:
	BEGIN
	create SAN;
	create 'text-anchor="middle"';
	END;
   text_anchor = end__:
	BEGIN
	create SAN;
	create 'text-anchor="end"';
	END;
   text_anchor = inherit:
	BEGIN
	create SAN;
	create 'text-anchor="inherit"';
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

   version:
	BEGIN
	create SAN;
	create 'version="';
	create version;
	create DoubleQuote;
	END;

   viewBox:
	BEGIN
	create SAN;
	create 'viewBox="';
	create viewBox;
	create DoubleQuote;
	END;

   x:
	if not Timeline_cross 
	BEGIN
	create SAN;
	create 'x="';
	create x;
	create DoubleQuote;
	END;

   y:
    if not Timeline_cross 
	BEGIN
	create SAN;
	create 'y="';
	create y;
	create DoubleQuote;
	END;

   width_:
	if not Timeline_cross 
	BEGIN
	create SAN;
	create 'width="';
	create width_;
	create DoubleQuote;
	END;

   height_:
	if not Timeline_cross 
	BEGIN
	create SAN;
	create 'height="';
	create height_;
	create DoubleQuote;
	END;
	
   opacity_:
	BEGIN
	create SAN;
	create 'opacity="';
	create opacity_;
	create DoubleQuote;
	END;

   stroke_opacity:
	BEGIN
	create SAN;
	create 'stroke-opacity="';
	create stroke_opacity;
	create DoubleQuote;
	END;

   fill_opacity:
	BEGIN
	create SAN;
	create 'fill-opacity="';
	create fill_opacity;
	create DoubleQuote;
	END;

   stop_color:
	BEGIN
	create SAN;
	create 'stop-color="';
	create stop_color;
	create DoubleQuote;
	END;

   gradientUnits:
	BEGIN
	create SAN;
	create 'gradientUnits="';
	create gradientUnits;
	create DoubleQuote;
	END;

   offset:
	BEGIN
	create SAN;
	create 'offset="';
	create offset;
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
	create ' xlink:href="';
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

  writing_mode = lr_tb:
	BEGIN
	create SAN;
	create 'writing-mode="lr-tb"';
	END;
  writing_mode = rl_tb:
	BEGIN
	create SAN;
	create 'writing-mode="rl-tb"';
	END;
  writing_mode = tb_rl:
	BEGIN
	create SAN;
	create 'writing-mode="tb-rl"';
	END;
  writing_mode = lr:
	BEGIN
	create SAN;
	create 'writing-mode="lr"';
	END;
  writing_mode = rl:
	BEGIN
	create SAN;
	create 'writing-mode="rl"';
	END;
  writing_mode = tb:
	BEGIN
	create SAN;
	create 'writing-mode="tb"';
	END;
  writing_mode = inherit:
	BEGIN
	create SAN;
	create 'writing-mode="inherit"';
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

   baseline_shift:
	BEGIN
	create SAN;
	create 'baseline-shift="';
	create baseline_shift;
	create DoubleQuote;
	END;

   clipPathUnits = userSpaceOnUse:
	BEGIN
	create SAN;
	create 'clipPathUnits="userSpaceOnUse"';
	END;

   clipPathUnits = objectBoundingBox:
	BEGIN
	create SAN;
	create 'clipPathUnits="objectBoundingBox"';
	END;

   target_:
	BEGIN
	create SAN;
	create 'target="';
	create target_;
	create DoubleQuote;
	END;

  onbegin:
	BEGIN
	create SAN;
	create 'onbegin="';
	create onbegin;
	create DoubleQuote;
	END;

  onend:
	BEGIN
	create SAN;
	create 'onend="';
	create onend;
	create DoubleQuote;
	END;

  onrepeat:
	BEGIN
	create SAN;
	create 'onrepeat="';
	create onrepeat;
	create DoubleQuote;
	END;

  attributeName_:
	BEGIN
	create SAN;
	create 'attributeName="';
	create attributeName_;
	create DoubleQuote;
	END;

  attributeType:
	BEGIN
	create SAN;
	create 'attributeType="';
	create attributeType;
	create DoubleQuote;
	END;

  begin_:
	BEGIN
	create SAN;
	create 'begin="';
	create begin_;
	create DoubleQuote;
	END;

  dur:
	BEGIN
	create SAN;
	create 'dur="';
	create dur;
	create DoubleQuote;
	END;

  end_:
	BEGIN
	create SAN;
	create 'end="';
	create end_;
	create DoubleQuote;
	END;

  min_:
	BEGIN
	create SAN;
	create 'min="';
	create min_;
	create DoubleQuote;
	END;

  max_:
	BEGIN
	create SAN;
	create 'max="';
	create max_;
	create DoubleQuote;
	END;

  restart = always:
	BEGIN
	create SAN;
	create 'restart="always"';
	END;
  restart = never:
	BEGIN
	create SAN;
	create 'restart="never"';
	END;
  restart = whenNotActive:
	BEGIN
	create SAN;
	create 'restart="whenNotActive"';
	END;

  repeatCount:
	BEGIN
	create SAN;
	create 'repeatCount="';
	create repeatCount;
	create DoubleQuote;
	END;

  repeatDur:
	BEGIN
	create SAN;
	create 'repeatDur="';
	create repeatDur;
	create DoubleQuote;
	END;

  fill_ = remove_:
	BEGIN
	create SAN;
	create 'fill="remove"';
	END;
  fill_ = freeze:
	BEGIN
	create SAN;
	create 'fill="freeze"';
	END;

  calcMode = discrete:
	BEGIN
	create SAN;
	create 'calcMode="discrete"';
	END;
  calcMode = linear:
	BEGIN
	create SAN;
	create 'calcMode="linear"';
	END;
  calcMode = paced:
	BEGIN
	create SAN;
	create 'calcMode="paced"';
	END;
  calcMode = spline:
	BEGIN
	create SAN;
	create 'calcMode="spline"';
	END;

  values:
	BEGIN
	create SAN;
	create 'values="';
	create values;
	create DoubleQuote;
	END;

  keyTimes:
	BEGIN
	create SAN;
	create 'keyTimes="';
	create keyTimes;
	create DoubleQuote;
	END;

  keySplines:
	BEGIN
	create SAN;
	create 'keySplines="';
	create keySplines;
	create DoubleQuote;
	END;

  from:
	BEGIN
	create SAN;
	create 'from="';
	create from;
	create DoubleQuote;
	END;

  to_:
	BEGIN
	create SAN;
	create 'to="';
	create to_;
	create DoubleQuote;
	END;

  by:
	BEGIN
	create SAN;
	create 'by="';
	create by;
	create DoubleQuote;
	END;

  additive = replace:
	BEGIN
	create SAN;
	create 'additive="replace"';
	END;
  additive = sum:
	BEGIN
	create SAN;
	create 'additive="sum"';
	END;

  accumulate = none_:
	BEGIN
	create SAN;
	create 'accumulate="none"';
	END;
  accumulate = sum:
	BEGIN
	create SAN;
	create 'accumulate="sum"';
	END;

  path_:
	BEGIN
	create SAN;
	create 'path="';
	create path_;
	create DoubleQuote;
	END;

  keyPoints:
	BEGIN
	create SAN;
	create 'keyPoints="';
	create keyPoints;
	create DoubleQuote;
	END;

  rotate:
	BEGIN
	create SAN;
	create 'rotate="';
	create rotate;
	create DoubleQuote;
	END;

  origin:
	BEGIN
	create SAN;
	create 'origin="';
	create origin;
	create DoubleQuote;
	END;

  type_ = translate:
	BEGIN
	create SAN;
	create 'type="translate"';
	END;
  type_ = scale:
	BEGIN
	create SAN;
	create 'type="scale"';
	END;
  type_ = rotate_:
	BEGIN
	create SAN;
	create 'type="rotate"';
	END;
  type_ = skewX:
	BEGIN
	create SAN;
	create 'type="skewX"';
	END;
  type_ = skewY:
	BEGIN
	create SAN;
	create 'type="skewY"';
	END;

   Unknown_attribute:
	BEGIN
	create SUA;
	Create Unknown_attribute;
	END;	

TextTRANSLATE
	BEGIN
	'\46' -> '&amp;';	{ ampersand, U+0026 ISOnum }
	'\74' -> '&lt;';	{ less-than sign, U+003C ISOnum }
	'\76' -> '&gt;'; 	{ greater-than sign, U+003E ISOnum }
	END;

END
