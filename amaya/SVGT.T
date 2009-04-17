
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

   SVG:
	BEGIN
        if immediately within use_
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
	if immediately within use_
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
	if immediately within use_
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
	if empty
		create CloseEmptyTagNL;
	indent -5;
	if not empty
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
	if immediately within use_
           Ignore;
	create SOT;
	create 'image';
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
		create 'image>\12' after;
		END;
	END;

   SVG_Image: BEGIN
	Remove;
	END;

   switch:
	BEGIN
	if immediately within use_
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
	if immediately within use_
           Ignore;
	create SOT;
	create 'path';
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
		create 'path>\12' after;
		END;
	END;

   rect:
	BEGIN
	if immediately within use_
           Ignore;
	create SOT;
	create 'rect';
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
		create 'rect>\12' after;
		END;
	END;

   circle_:
	BEGIN
	if immediately within use_
           Ignore;
	create SOT;
	create 'circle';
	indent +8;
	create attributes;
	if empty
		create CloseEmptyTagNL;
	indent -8;
	if not empty
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
	if immediately within use_
           Ignore;
	create SOT;
	create 'ellipse';
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
		create 'ellipse>\12' after;
		END;
	END;

   line_:
	BEGIN
	if immediately within use_
           Ignore;
	create SOT;
	create 'line';
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
		create 'line>\12' after;
		END;
	END;

   polyline:
	BEGIN
	if immediately within use_
           Ignore;
	create SOT;
	create 'polyline';
	indent +10;
	create attributes;
	if empty
		create CloseEmptyTagNL;
	indent -10;
	if not empty
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
	if immediately within use_
           Ignore;
	create SOT;
	create 'polygon';
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
		create 'polygon>\12' after;
		END;
	END;

   text_:
	BEGIN
	if immediately within use_
           Ignore;
	create SOT;
	create 'text';
	indent Suspend;
	create attributes;
	create '>';
	indent Resume after;
	create SCT after;
	create 'text>\12' after;
	END;

   tspan:
	BEGIN
	if immediately within use_
           Ignore;
	create SOT;
	create 'tspan';
	create attributes;
	create '>';
	create SCT after;
	create 'tspan>\12' after;
	END;

   tref:
	BEGIN
	if immediately within use_
           Ignore;
	create SOT;
	create 'tref';
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
		create 'tref>\12' after;
		END;
	END;

   textPath:
	BEGIN
	if immediately within use_
           Ignore;
	create SOT;
	create 'textPath';
	indent +10;
	create attributes;
	if empty
		create CloseEmptyTagNL;
        indent -10;
	if not empty
		BEGIN
		create CloseTagNL;
		indent +2;
		indent -2 after;
		create SCT after;
		create 'textPath>\12' after;
		END;
	END;

   altGlyph:
	BEGIN
	if immediately within use_
           Ignore;
	create SOT;
	create 'altGlyph';
	indent +10;
	create attributes;
	if empty
		create CloseEmptyTagNL;
        indent -10;
	if not empty
		BEGIN
		create CloseTagNL;
		indent +2;
		indent -2 after;
		create SCT after;
		create 'altGlyph>\12' after;
		END;
	END;

   altGlyphDef:
	BEGIN
	if immediately within use_
           Ignore;
	create SOT;
	create 'altGlyphDef';
	indent +13;
	create attributes;
	if empty
		create CloseEmptyTagNL;
        indent -13;
	if not empty
		BEGIN
		create CloseTagNL;
		indent +2;
		indent -2 after;
		create SCT after;
		create 'altGlyphDef>\12' after;
		END;
	END;

   altGlyphItem:
	BEGIN
	if immediately within use_
           Ignore;
	create SOT;
	create 'altGlyphItem';
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
		create 'altGlyphItem>\12' after;
		END;
	END;

   glyphRef:
	BEGIN
	if immediately within use_
           Ignore;
	create SOT;
	create 'glyphRef';
	indent +10;
	create attributes;
	if empty
		create CloseEmptyTagNL;
        indent -10;
	if not empty
		BEGIN
		create CloseTagNL;
		indent +2;
		indent -2 after;
		create SCT after;
		create 'glyphRef>\12' after;
		END;
	END;

   marker:
	BEGIN
	if immediately within use_
           Ignore;
	if transclusion
           Ignore;
	create SOT;
	create 'marker';
	indent +8;
	create attributes;
	if empty
		create CloseEmptyTagNL;
        indent -8;
	if not empty
		BEGIN
		create CloseTagNL;
		indent +2;
		indent -2 after;
		create SCT after;
		create 'marker>\12' after;
		END;
	END;

   color_profile:
	BEGIN
	if immediately within use_
           Ignore;
	create SOT;
	create 'color-profile';
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
		create 'color-profile>\12' after;
		END;
	END;

   linearGradient:
	BEGIN
	if immediately within use_
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
		create 'linearGradient>\12' after;
		END;
	END;

    radialGradient:
	BEGIN
	if immediately within use_
           Ignore;
	create SOT;
	create 'radialGradient';
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
		create 'radialGradient>\12' after;
		END;
	END;

    stop:
	BEGIN
	if immediately within use_
           Ignore;
	if IsCopy
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

   pattern:
	BEGIN
	if immediately within use_
           Ignore;
	create SOT;
	create 'pattern';
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
		create 'pattern>\12' after;
		END;
	END;

   clipPath:
	BEGIN
	if immediately within use_
           Ignore;
	create SOT;
	create 'clipPath';
	indent +10;
	create attributes;
	if empty
		create CloseEmptyTagNL;
	indent -10;
	if not empty
		BEGIN
		create CloseTagNL;
		indent +2;
		indent -2 after;
		create SCT after;
		create 'clipPath>\12' after;
		END;
	END;

   mask:
	BEGIN
	if immediately within use_
           Ignore;
	create SOT;
	create 'mask';
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
		create 'mask>\12' after;
		END;
	END;

   filter:
	BEGIN
	if immediately within use_
           Ignore;
	create SOT;
	create 'filter';
	indent +8;
	create attributes;
	if empty
		create CloseEmptyTagNL;
        indent -8;
	if not empty
		BEGIN
		create CloseTagNL;
		indent +2;
		indent -2 after;
		create SCT after;
		create 'filter>\12' after;
		END;
	END;

   feDistantLight:
	BEGIN
	if immediately within use_
           Ignore;
	create SOT;
	create 'feDistantLight';
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
		create 'feDistantLight>\12' after;
		END;
	END;

   fePointLight:
	BEGIN
	if immediately within use_
           Ignore;
	create SOT;
	create 'fePointLight';
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
		create 'fePointLight>\12' after;
		END;
	END;

   feSpotLight:
	BEGIN
	if immediately within use_
           Ignore;
	create SOT;
	create 'feSpotLight';
	indent +13;
	create attributes;
	if empty
		create CloseEmptyTagNL;
        indent -13;
	if not empty
		BEGIN
		create CloseTagNL;
		indent +2;
		indent -2 after;
		create SCT after;
		create 'feSpotLight>\12' after;
		END;
	END;

   feBlend:
	BEGIN
	if immediately within use_
           Ignore;
	create SOT;
	create 'feBlend';
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
		create 'feBlend>\12' after;
		END;
	END;

   feColorMatrix:
	BEGIN
	if immediately within use_
           Ignore;
	create SOT;
	create 'feColorMatrix';
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
		create 'feColorMatrix>\12' after;
		END;
	END;

   feComponentTransfer:
	BEGIN
	if immediately within use_
           Ignore;
	create SOT;
	create 'feComponentTransfer';
	indent +21;
	create attributes;
	if empty
		create CloseEmptyTagNL;
        indent -21;
	if not empty
		BEGIN
		create CloseTagNL;
		indent +2;
		indent -2 after;
		create SCT after;
		create 'feComponentTransfer>\12' after;
		END;
	END;

   feFuncR:
	BEGIN
	if immediately within use_
           Ignore;
	create SOT;
	create 'feFuncR';
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
		create 'feFuncR>\12' after;
		END;
	END;

   feFuncG:
	BEGIN
	if immediately within use_
           Ignore;
	create SOT;
	create 'feFuncG';
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
		create 'feFuncG>\12' after;
		END;
	END;

   feFuncB:
	BEGIN
	if immediately within use_
           Ignore;
	create SOT;
	create 'feFuncB';
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
		create 'feFuncB>\12' after;
		END;
	END;

   feFuncA:
	BEGIN
	if immediately within use_
           Ignore;
	create SOT;
	create 'feFuncA';
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
		create 'feFuncA>\12' after;
		END;
	END;

   feComposite:
	BEGIN
	if immediately within use_
           Ignore;
	create SOT;
	create 'feComposite';
	indent +13;
	create attributes;
	if empty
		create CloseEmptyTagNL;
        indent -13;
	if not empty
		BEGIN
		create CloseTagNL;
		indent +2;
		indent -2 after;
		create SCT after;
		create 'feComposite>\12' after;
		END;
	END;

   feConvolveMatrix:
	BEGIN
	if immediately within use_
           Ignore;
	create SOT;
	create 'feConvolveMatrix';
	indent +17;
	create attributes;
	if empty
		create CloseEmptyTagNL;
        indent -17;
	if not empty
		BEGIN
		create CloseTagNL;
		indent +2;
		indent -2 after;
		create SCT after;
		create 'feConvolveMatrix>\12' after;
		END;
	END;

   feDiffuseLighting:
	BEGIN
	if immediately within use_
           Ignore;
	create SOT;
	create 'feDiffuseLighting';
	indent +19;
	create attributes;
	if empty
		create CloseEmptyTagNL;
        indent -19;
	if not empty
		BEGIN
		create CloseTagNL;
		indent +2;
		indent -2 after;
		create SCT after;
		create 'feDiffuseLighting>\12' after;
		END;
	END;

   feDisplacementMap:
	BEGIN
	if immediately within use_
           Ignore;
	create SOT;
	create 'feDisplacementMap';
	indent +19;
	create attributes;
	if empty
		create CloseEmptyTagNL;
        indent -19;
	if not empty
		BEGIN
		create CloseTagNL;
		indent +2;
		indent -2 after;
		create SCT after;
		create 'feDisplacementMap>\12' after;
		END;
	END;

   feFlood:
	BEGIN
	if immediately within use_
           Ignore;
	create SOT;
	create 'feFlood';
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
		create 'feFlood>\12' after;
		END;
	END;

   feGaussianBlur:
	BEGIN
	if immediately within use_
           Ignore;
	create SOT;
	create 'feGaussianBlur';
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
		create 'feGaussianBlur>\12' after;
		END;
	END;

   feImage:
	BEGIN
	if immediately within use_
           Ignore;
	create SOT;
	create 'feImage';
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
		create 'feImage>\12' after;
		END;
	END;

   feMerge:
	BEGIN
	if immediately within use_
           Ignore;
	create SOT;
	create 'feMerge';
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
		create 'feMerge>\12' after;
		END;
	END;

   feMergeNode:
	BEGIN
	if immediately within use_
           Ignore;
	create SOT;
	create 'feMergeNode';
	indent +13;
	create attributes;
	if empty
		create CloseEmptyTagNL;
        indent -13;
	if not empty
		BEGIN
		create CloseTagNL;
		indent +2;
		indent -2 after;
		create SCT after;
		create 'feMergeNode>\12' after;
		END;
	END;

   feMorphology:
	BEGIN
	if immediately within use_
           Ignore;
	create SOT;
	create 'feMorphology';
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
		create 'feMorphology>\12' after;
		END;
	END;

   feOffset:
	BEGIN
	if immediately within use_
           Ignore;
	create SOT;
	create 'feOffset';
	indent +10;
	create attributes;
	if empty
		create CloseEmptyTagNL;
        indent -10;
	if not empty
		BEGIN
		create CloseTagNL;
		indent +2;
		indent -2 after;
		create SCT after;
		create 'feOffset>\12' after;
		END;
	END;

   feSpecularLighting:
	BEGIN
	if immediately within use_
           Ignore;
	create SOT;
	create 'feSpecularLighting';
	indent +20;
	create attributes;
	if empty
		create CloseEmptyTagNL;
        indent -20;
	if not empty
		BEGIN
		create CloseTagNL;
		indent +2;
		indent -2 after;
		create SCT after;
		create 'feSpecularLighting>\12' after;
		END;
	END;

   feTile:
	BEGIN
	if immediately within use_
           Ignore;
	create SOT;
	create 'feTile';
	indent +8;
	create attributes;
	if empty
		create CloseEmptyTagNL;
        indent -8;
	if not empty
		BEGIN
		create CloseTagNL;
		indent +2;
		indent -2 after;
		create SCT after;
		create 'feTile>\12' after;
		END;
	END;

   feTurbulence:
	BEGIN
	if immediately within use_
           Ignore;
	create SOT;
	create 'feTurbulence';
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
		create 'feTurbulence>\12' after;
		END;
	END;

   cursor:
	BEGIN
	if immediately within use_
           Ignore;
	create SOT;
	create 'cursor';
	indent +8;
	create attributes;
	if empty
		create CloseEmptyTagNL;
        indent -8;
	if not empty
		BEGIN
		create CloseTagNL;
		indent +2;
		indent -2 after;
		create SCT after;
		create 'cursor>\12' after;
		END;
	END;

   a:
	BEGIN
	if immediately within use_
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

   view:
	BEGIN
	if immediately within use_
           Ignore;
	create SOT;
	create 'view';
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
		create 'view>\12' after;
		END;
	END;

   script_:
	BEGIN
	if immediately within use_
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
	if immediately within use_
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

   animateTransform:
	BEGIN
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

   font_:
	BEGIN
	create SOT;
	create 'font';
	indent +6;
	create attributes;
	create '>';
	indent -4;
	indent -2 after;
	create SCT after;
	create 'font>\12' after;
	END;

   glyph:
	BEGIN
	create SOT;
	create 'glyph';
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
		create 'glyph>\12' after;		
		END;
	END;

   missing_glyph:
	BEGIN
	create SOT;
	create 'missing-glyph';
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
		create 'missing-glyph>\12' after;		
		END;
	END;

   hkern:
	BEGIN
	create SOT;
	create 'hkern';
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
		create 'hkern>\12' after;		
		END;
	END;

   vkern:
	BEGIN
	create SOT;
	create 'vkern';
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
		create 'vkern>\12' after;		
		END;
	END;

   font_face:
	BEGIN
	create SOT;
	create 'font-face';
	indent +5;
	create attributes;
	create CloseTagNL;
	indent -3;
	indent -2 after;
	create SCT after;
	create 'font-face>\12' after;		
	END;

   font_face_src:
	BEGIN
	create SOT;
	create 'font-face-src';
	indent +15;
	create attributes;
	create '>';
	indent -13;
	indent -2 after;
	create SCT after;
	create 'font-face-src>\12' after;
	END;

   font_face_uri:
	BEGIN
	create SOT;
	create 'font-face-uri';
	indent +15;
	create attributes;
	create '>';
	indent -13;
	indent -2 after;
	create SCT after;
	create 'font-face-uri>\12' after;
	END;

   font_face_format:
	BEGIN
	create SOT;
	create 'font-face-format';
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
		create 'font-face-format>\12' after;		
		END;
	END;

   font_face_name:
	BEGIN
	create SOT;
	create 'font-face-name';
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
		create 'font-face-name>\12' after;		
		END;
	END;

   definition_src:
	BEGIN
	create SOT;
	create 'definition-src';
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
		create 'definition-src>\12' after;		
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
	if immediately within use_
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

   Timeline_cross:
	BEGIN
	Remove;
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
	{NoTranslation;}
        Create '-->\12' after;
        END;

   XMLcomment_line:
	if not last create '\12' after;

   XMLPI:
	BEGIN
	NoLineBreak;
        Create '<?';
	{NoTranslation;}
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
	if immediately within use_
           Ignore;
	if within tref and
           not within desc and not within metadata and not within title
            Ignore;
 { if immediately within text_
	    create '\12' after;}
	END;

   GRAPHICS_UNIT:
	Remove;

   PICTURE_UNIT:
	Remove;


ATTRIBUTES

   Language:
	begin
	if not TEXT_UNIT and not Root
	    begin
	    Create ' xml:lang="';
	    Create Language;
	    Create DoubleQuote;
	    end;
	if Root and RealLang
	    begin
	    Create ' xml:lang="';
	    Create Language;
	    Create DoubleQuote;
	    end;
	end;

   id:
        BEGIN
        create SAN;
        create 'id="';
        create id;
        create DoubleQuote;
        END;

   xmlid:
	BEGIN
        create SAN;
	Create 'xml:id="';
	Create xmlid;
	Create DoubleQuote;
	END;

   xml_base:
	BEGIN
	create SAN;
	create 'xml:base="';
	create xml_base;
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

   Unknown_attribute:
	BEGIN
	create SUA;
	Create Unknown_attribute;
	END;	

   Namespace:
	begin
	{*****
        create 'xmlns="';
        create Namespace;
        create DoubleQuote;
        *****}
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

   baseProfile:
	BEGIN
	create SAN;
	create 'baseProfile="';
	create baseProfile;
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

   color:
        begin
        create SAN;
        create 'color="';
        create color;
        create DoubleQuote;
        end;

   color_interpolation = auto:
	BEGIN
	create SAN;
	create 'color-interpolation=="auto"';
	END;
   color_interpolation = sRGB:
	BEGIN
	create SAN;
	create 'color-interpolation=="sRGB"';
	END;
   color_interpolation = linearRGB:
	BEGIN
	create SAN;
	create 'color-interpolation=="linearRGB"';
	END;
   color_interpolation = inherit:
	BEGIN
	create SAN;
	create 'color-interpolation="inherit"';
	END;

   color_rendering:
        begin
        create SAN;
        create 'color-rendering="';
        create color_rendering;
        create DoubleQuote;
        end;
 
   enable_background:
        begin
        create SAN;
        create 'enable-background="';
        create enable_background;
        create DoubleQuote;
        end;
 
   flood_color:
        begin
        create SAN;
        create 'flood-color="';
        create flood_color;
        create DoubleQuote;
        end;
 
   flood_opacity:
        begin
        create SAN;
        create 'flood-opacity="';
        create flood_opacity;
        create DoubleQuote;
        end;
 
   fill:
	BEGIN
	create SAN;
	create 'fill="';
	create fill;
	create DoubleQuote;
	END;

   fill_opacity:
	BEGIN
	create SAN;
	create 'fill-opacity="';
	create fill_opacity;
	create DoubleQuote;
	END;

   fill_rule = nonzero:
	BEGIN
	create SAN;
	create 'fill-rule="nonzero"';
	END;
   fill_rule = evenodd:
	BEGIN
	create SAN;
	create 'fill-rule="evenodd"';
	END;
   fill_rule = inherit:
	BEGIN
	create SAN;
	create 'fill-rule="inherit"';
	END;

   stroke:
	BEGIN
	create SAN;
	create 'stroke="';
	create stroke;
	create DoubleQuote;
	END;

   stroke_dasharray:
        begin
        create SAN;
        create 'stroke-dasharray="';
        create stroke_dasharray;
        create DoubleQuote;
        end;
 
   stroke_dashoffset:
        begin
        create SAN;
        create 'stroke-dashoffset="';
        create stroke_dashoffset;
        create DoubleQuote;
        end;
 
   stroke_linecap = butt:
	BEGIN
	create SAN;
	create 'stroke-linecap="butt"';
	END;
   stroke_linecap = round:
	BEGIN
	create SAN;
	create 'stroke-linecap="round"';
	END;
   stroke_linecap = square:
	BEGIN
	create SAN;
	create 'stroke-linecap="square"';
	END;
   stroke_linecap = inherit:
	BEGIN
	create SAN;
	create 'stroke-linecap="inherit"';
	END;

   stroke_linejoin = miter:
	BEGIN
	create SAN;
	create 'stroke-linejoin="miter"';
	END;
   stroke_linejoin = round:
	BEGIN
	create SAN;
	create 'stroke-linejoin="round"';
	END;
   stroke_linejoin = bevel:
	BEGIN
	create SAN;
	create 'stroke-linejoin="bevel"';
	END;
   stroke_linejoin = inherit:
	BEGIN
	create SAN;
	create 'stroke-linejoin="inherit"';
	END;

   stroke_miterlimit:
        begin
        create SAN;
        create 'stroke-miterlimit="';
        create stroke_miterlimit;
        create DoubleQuote;
        end;
 
   stroke_opacity:
	BEGIN
	create SAN;
	create 'stroke-opacity="';
	create stroke_opacity;
	create DoubleQuote;
	END;

   stroke_width:
	BEGIN
	create SAN;
	create 'stroke-width="';
	create stroke_width;
	create DoubleQuote;
	END;

   color_interpolation_filters = auto:
	BEGIN
	create SAN;
	create 'color-interpolation-filters="auto"';
	END;
   color_interpolation_filters = sRGB:
	BEGIN
	create SAN;
	create 'color-interpolation-filters="sRGB"';
	END;
   color_interpolation_filters = linearRGB:
	BEGIN
	create SAN;
	create 'color-interpolation-filters="linearRGB"';
	END;
   color_interpolation_filters = inherit:
	BEGIN
	create SAN;
	create 'color-interpolation-filters="inherit"';
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

   font_size_adjust:
        begin
        create SAN;
        create 'font-size-adjust="';
        create font_size_adjust;
        create DoubleQuote;
        end;
 
   font_stretch = normal_:
	BEGIN
	create SAN;
	create 'font-stretch="normal"';
	END;
   font_stretch = wider:
	BEGIN
	create SAN;
	create 'font-stretch="wider"';
	END;
   font_stretch = narrower:
	BEGIN
	create SAN;
	create 'font-stretch="narrower"';
	END;
   font_stretch = ultra_condensed:
	BEGIN
	create SAN;
	create 'font-stretch="ultra-condensed"';
	END;
   font_stretch = extra_condensed:
	BEGIN
	create SAN;
	create 'font-stretch="extra-condensed"';
	END;
   font_stretch = condensed:
	BEGIN
	create SAN;
	create 'font-stretch="condensed"';
	END;
   font_stretch = semi_condensed:
	BEGIN
	create SAN;
	create 'font-stretch="semi-condensed"';
	END;
   font_stretch = semi_expanded:
	BEGIN
	create SAN;
	create 'font-stretch="semi-expanded"';
	END;
   font_stretch = expanded:
	BEGIN
	create SAN;
	create 'font-stretch="expanded"';
	END;
   font_stretch = extra_expanded:
	BEGIN
	create SAN;
	create 'font-stretch="extra-expanded"';
	END;
   font_stretch = ultra_expanded:
	BEGIN
	create SAN;
	create 'font-stretch="ultra-expanded"';
	END;
   font_stretch = inherit:
	BEGIN
	create SAN;
	create 'font-stretch="inherit"';
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

   stop_color:
	BEGIN
	create SAN;
	create 'stop-color="';
	create stop_color;
	create DoubleQuote;
	END;

   stop_opacity:
	BEGIN
	create SAN;
	create 'stop-opacity="';
	create stop_opacity;
	create DoubleQuote;
	END;

   clip_path:
        begin
        create SAN;
        create 'clip-path="';
        create clip_path;
        create DoubleQuote;
        end;

   clip_rule = nonzero:
	BEGIN
	create SAN;
	create 'clip-rule="nonzero"';
	END;
   clip_rule = evenodd:
	BEGIN
	create SAN;
	create 'clip-rule="evenodd"';
	END;
   clip_rule = inherit:
	BEGIN
	create SAN;
	create 'clip-rule="inherit"';
	END;

   cursor_:
	BEGIN
	create SAN;
	create 'cursor="';
	create cursor_;
	create DoubleQuote;
	END;

   display_ = inline:
	BEGIN
	create SAN;
	create 'display="inline"';
	END;
   display_ = block:
	BEGIN
	create SAN;
	create 'display="block"';
	END;
   display_ = list_item:
	BEGIN
	create SAN;
	create 'display="list-item"';
	END;
   display_ = run_in:
	BEGIN
	create SAN;
	create 'display="run-in"';
	END;
   display_ = compact:
	BEGIN
	create SAN;
	create 'display="compact"';
	END;
   display_ = marker_:
	BEGIN
	create SAN;
	create 'display="marker"';
	END;
   display_ = table:
	BEGIN
	create SAN;
	create 'display="table"';
	END;
   display_ = inline_table:
	BEGIN
	create SAN;
	create 'display="inline-table"';
	END;
   display_ = table_row_group:
	BEGIN
	create SAN;
	create 'display="table-row-group"';
	END;
   display_ = table_header_group:
	BEGIN
	create SAN;
	create 'display="table-header-group"';
	END;
   display_ = table_footer_group:
	BEGIN
	create SAN;
	create 'display="table-footer-group"';
	END;
   display_ = table_row:
	BEGIN
	create SAN;
	create 'display="table-row"';
	END;
   display_ = table_column_group:
	BEGIN
	create SAN;
	create 'display="table-column-group"';
	END;
   display_ = table_column:
	BEGIN
	create SAN;
	create 'display="table-column"';
	END;
   display_ = table_cell:
	BEGIN
	create SAN;
	create 'display="table-cell"';
	END;
   display_ = table_caption:
	BEGIN
	create SAN;
	create 'display="table-caption"';
	END;
   display_ = none:
	BEGIN
	create SAN;
	create 'display="none"';
	END;
   display_ = inherit:
	BEGIN
	create SAN;
	create 'display="inherit"';
	END;

   filter_:
	BEGIN
	create SAN;
	create 'filter="';
	create filter_;
	create DoubleQuote;
	END;

   image_rendering = auto:
	BEGIN
	create SAN;
	create 'image-rendering="auto"';
	END;
   image_rendering = optimizeSpeed:
	BEGIN
	create SAN;
	create 'image-rendering="optimizeSpeed"';
	END;
   image_rendering = optimizeQuality:
	BEGIN
	create SAN;
	create 'image-rendering="optimizeQuality"';
	END;
   image_rendering = inherit:
	BEGIN
	create SAN;
	create 'image-rendering="inherit"';
	END;

   mask_:
	BEGIN
	create SAN;
	create 'mask="';
	create mask_;
	create DoubleQuote;
	END;

   opacity_:
	BEGIN
	create SAN;
	create 'opacity="';
	create opacity_;
	create DoubleQuote;
	END;

   pointer_events = visiblePainted:
	BEGIN
	create SAN;
	create 'pointer-events="visiblePainted"';
	END;
   pointer_events = visibleFill:
	BEGIN
	create SAN;
	create 'pointer-events="visibleFill"';
	END;
   pointer_events = visibleStroke:
	BEGIN
	create SAN;
	create 'pointer-events="visibleStroke"';
	END;
   pointer_events = visible:
	BEGIN
	create SAN;
	create 'pointer-events="visible"';
	END;
   pointer_events = painted:
	BEGIN
	create SAN;
	create 'pointer-events="painted"';
	END;
   pointer_events = fill__:
	BEGIN
	create SAN;
	create 'pointer-events="fill"';
	END;
   pointer_events = stroke_:
	BEGIN
	create SAN;
	create 'pointer-events="stroke"';
	END;
   pointer_events = all:
	BEGIN
	create SAN;
	create 'pointer-events="all"';
	END;
   pointer_events = none:
	BEGIN
	create SAN;
	create 'pointer-events="none"';
	END;
   pointer_events = inherit:
	BEGIN
	create SAN;
	create 'pointer-events="inherit"';
	END;

   shape_rendering = auto:
	BEGIN
	create SAN;
	create 'shape-rendering="auto"';
	END;
   shape_rendering = optimizeSpeed:
	BEGIN
	create SAN;
	create 'shape-rendering="optimizeSpeed"';
	END;
   shape_rendering = crispEdges:
	BEGIN
	create SAN;
	create 'shape-rendering="crispEdges"';
	END;
   shape_rendering = geometricPrecision:
	BEGIN
	create SAN;
	create 'shape-rendering="geometricPrecision"';
	END;
   shape_rendering = inherit:
	BEGIN
	create SAN;
	create 'shape-rendering="inherit"';
	END;

   text_rendering = auto:
	BEGIN
	create SAN;
	create 'text-rendering="auto"';
	END;
   text_rendering = optimizeSpeed:
	BEGIN
	create SAN;
	create 'text-rendering="optimizeSpeed"';
	END;
   text_rendering = optimizeLegibility:
	BEGIN
	create SAN;
	create 'text-rendering="optimizeLegibility"';
	END;
   text_rendering = geometricPrecision:
	BEGIN
	create SAN;
	create 'text-rendering="geometricPrecision"';
	END;
   text_rendering = inherit:
	BEGIN
	create SAN;
	create 'text-rendering="inherit"';
	END;

   visibility_ = visible:
	BEGIN
	create SAN;
	create 'visibility="visible"';
	END;
   visibility_ = hidden_:
	BEGIN
	create SAN;
	create 'visibility="hidden"';
	END;
   visibility_ = inherit:
	BEGIN
	create SAN;
	create 'visibility="inherit"';
	END;

   color_profile_:
	BEGIN
	create SAN;
	create 'color-profile="';
	create color_profile_;
	create DoubleQuote;
	END;

   lighting_color:
	BEGIN
	create SAN;
	create 'lighting-color="';
	create lighting_color;
	create DoubleQuote;
	END;

   marker_start:
	BEGIN
	create SAN;
	create 'marker-start="';
	create marker_start;
	create DoubleQuote;
	END;

   marker_mid:
	BEGIN
	create SAN;
	create 'marker-mid="';
	create marker_mid;
	create DoubleQuote;
	END;

   marker_end:
	BEGIN
	create SAN;
	create 'marker-end="';
	create marker_end;
	create DoubleQuote;
	END;

   alignment_baseline = baseline:
	BEGIN
	create SAN;
	create 'alignment-baseline="baseline"';
	END;
   alignment_baseline = top:
	BEGIN
	create SAN;
	create 'alignment-baseline="top"';
	END;
   alignment_baseline = before_edge:
	BEGIN
	create SAN;
	create 'alignment-baseline="before-edge"';
	END;
   alignment_baseline = text_top:
	BEGIN
	create SAN;
	create 'alignment-baseline="text-top"';
	END;
   alignment_baseline = text_before_edge:
	BEGIN
	create SAN;
	create 'alignment-baseline="text-before-edge"';
	END;
   alignment_baseline = middle:
	BEGIN
	create SAN;
	create 'alignment-baseline="middle"';
	END;
   alignment_baseline = bottom:
	BEGIN
	create SAN;
	create 'alignment-baseline="bottom"';
	END;
   alignment_baseline = after_edge:
	BEGIN
	create SAN;
	create 'alignment-baseline="after-edge"';
	END;
   alignment_baseline = text_bottom:
	BEGIN
	create SAN;
	create 'alignment-baseline="text-bottom"';
	END;
   alignment_baseline = text_after_edge:
	BEGIN
	create SAN;
	create 'alignment-baseline="text-after-edge"';
	END;
   alignment_baseline = ideographic_:
	BEGIN
	create SAN;
	create 'alignment-baseline="ideographic"';
	END;
   alignment_baseline = lower:
	BEGIN
	create SAN;
	create 'alignment-baseline="lower"';
	END;
   alignment_baseline = hanging_:
	BEGIN
	create SAN;
	create 'alignment-baseline="hanging"';
	END;
   alignment_baseline = mathematical_:
	BEGIN
	create SAN;
	create 'alignment-baseline="mathematical"';
	END;
   alignment_baseline = inherit:
	BEGIN
	create SAN;
	create 'alignment-baseline="inherit"';
	END;

   baseline_shift:
	BEGIN
	create SAN;
	create 'baseline-shift="';
	create baseline_shift;
	create DoubleQuote;
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

   dominant_baseline = auto:
	BEGIN
	create SAN;
	create 'dominant-baseline="auto"';
	END;
   dominant_baseline = autosense_script:
	BEGIN
	create SAN;
	create 'dominant-baseline="autosense-script"';
	END;
   dominant_baseline = no_change:
	BEGIN
	create SAN;
	create 'dominant-baseline="no-change"';
	END;
   dominant_baseline = reset:
	BEGIN
	create SAN;
	create 'dominant-baseline="reset"';
	END;
   dominant_baseline = ideographic_:
	BEGIN
	create SAN;
	create 'dominant-baseline="ideographic"';
	END;
   dominant_baseline = lower:
	BEGIN
	create SAN;
	create 'dominant-baseline="lower"';
	END;
   dominant_baseline = hanging_:
	BEGIN
	create SAN;
	create 'dominant-baseline="hanging"';
	END;
   dominant_baseline = mathematical_:
	BEGIN
	create SAN;
	create 'dominant-baseline="mathematical"';
	END;
   dominant_baseline = inherit:
	BEGIN
	create SAN;
	create 'dominant-baseline="inherit"';
	END;

   glyph_orientation_horizontal:
	BEGIN
	create SAN;
	create 'glyph-orientation-horizontal="';
	create glyph_orientation_horizontal;
	create DoubleQuote;
	END;

   glyph_orientation_vertical:
	BEGIN
	create SAN;
	create 'glyph-orientation-vertical="';
	create glyph_orientation_vertical;
	create DoubleQuote;
	END;

   kerning:
	BEGIN
	create SAN;
	create 'kerning="';
	create kerning;
	create DoubleQuote;
	END;

   letter_spacing:
	BEGIN
	create SAN;
	create 'letter-spacing="';
	create letter_spacing;
	create DoubleQuote;
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

   word_spacing:
	BEGIN
	create SAN;
	create 'word-spacing="';
	create word_spacing;
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

   clip:
	BEGIN
	create SAN;
	create 'clip="';
	create clip;
	create DoubleQuote;
	END;

  overflow  = visible:
	BEGIN
	create SAN;
	create 'overflow="visible"';
	END;
  overflow  = hidden_:
	BEGIN
	create SAN;
	create 'overflow="hidden"';
	END;
  overflow  = scroll:
	BEGIN
	create SAN;
	create 'overflow="scroll"';
	END;
  overflow  = auto:
	BEGIN
	create SAN;
	create 'overflow="auto"';
	END;
  overflow  = inherit:
	BEGIN
	create SAN;
	create 'overflow="inherit"';
	END;

   viewBox:
	BEGIN
	create SAN;
	create 'viewBox="';
	create viewBox;
	create DoubleQuote;
	END;

   preserveAspectRatio:
	BEGIN
	create SAN;
	create 'preserveAspectRatio="';
	create preserveAspectRatio;
	create DoubleQuote;
	END;

   zoomAndPan = disable:
	BEGIN
	create SAN;
	create 'zoomAndPan="disable"';
	END;
   zoomAndPan = magnify:
	BEGIN
	create SAN;
	create 'zoomAndPan="magnify"';
	END;

   onfocusin:
	BEGIN
	create SAN;
	create 'onfocusin="';
	create onfocusin;
	create DoubleQuote;
	END;

   onfocusout:
	BEGIN
	create SAN;
	create 'onfocusout="';
	create onfocusout;
	create DoubleQuote;
	END;

   onactivate:
	BEGIN
	create SAN;
	create 'onactivate="';
	create onactivate;
	create DoubleQuote;
	END;

   onclick:
	BEGIN
	create SAN;
	create 'onclick="';
	create onclick;
	create DoubleQuote;
	END;

   onmousedown:
	BEGIN
	create SAN;
	create 'onmousedown="';
	create onmousedown;
	create DoubleQuote;
	END;

   onmouseup:
	BEGIN
	create SAN;
	create 'onmouseup="';
	create onmouseup;
	create DoubleQuote;
	END;

   onmouseover:
	BEGIN
	create SAN;
	create 'onmouseover="';
	create onmouseover;
	create DoubleQuote;
	END;

   onmousemove:
	BEGIN
	create SAN;
	create 'onmousemove="';
	create onmousemove;
	create DoubleQuote;
	END;

   onmouseout:
	BEGIN
	create SAN;
	create 'onmouseout="';
	create onmouseout;
	create DoubleQuote;
	END;

   onload:
	BEGIN
	create SAN;
	create 'onload="';
	create onload;
	create DoubleQuote;
	END;

   onunload:
	BEGIN
	create SAN;
	create 'onunload="';
	create onunload;
	create DoubleQuote;
	END;

   onabort:
	BEGIN
	create SAN;
	create 'onabort="';
	create onabort;
	create DoubleQuote;
	END;

   onerror:
	BEGIN
	create SAN;
	create 'onerror="';
	create onerror;
	create DoubleQuote;
	END;

   onresize:
	BEGIN
	create SAN;
	create 'onresize="';
	create onresize;
	create DoubleQuote;
	END;

   onscroll:
	BEGIN
	create SAN;
	create 'onscroll="';
	create onscroll;
	create DoubleQuote;
	END;

   onzoom:
	BEGIN
	create SAN;
	create 'onzoom="';
	create onzoom;
	create DoubleQuote;
	END;

   version:
	BEGIN
	create SAN;
	create 'version="';
	create version;
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
	
   contentScriptType:
	BEGIN
	create SAN;
	create 'contentScriptType="';
	create contentScriptType;
	create DoubleQuote;
	END;

   contentStyleType:
	BEGIN
	create SAN;
	create 'contentStyleType="';
	create contentStyleType;
	create DoubleQuote;
	END;

   transform:
	BEGIN
	create SAN;
	create 'transform="';
	create transform;
	create DoubleQuote;
	END;

   content_:
	BEGIN
	create SAN;
	create 'content="';
	create content_;
	create DoubleQuote;
	END;

   xlink_type = simple:
	BEGIN
	create SAN;
	create 'xlink:type="simple"';
	END;

   xlink_role:
	BEGIN
	create 'xlink:role="';
	create xlink_role;
	create DoubleQuote;
	END;

   xlink_arcrole:
	BEGIN
	create 'xlink:arcrole="';
	create xlink_arcrole;
	create DoubleQuote;
	END;

   xlink_title:
	BEGIN
	create 'xlink:title="';
	create xlink_title;
	create DoubleQuote;
	END;

   xlink_show = other:
	BEGIN
	create SAN;
	create 'xlink:show="other"';
	END;
   xlink_show = embed:
	BEGIN
	create SAN;
	create 'xlink:show="embed"';
	END;

   xlink_actuate = onLoad:
	BEGIN
	create SAN;
	create 'xlink:actuate="onLoad"';
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

   rotate:
	BEGIN
	create SAN;
	create 'rotate="';
	create rotate;
	create DoubleQuote;
	END;

   textLength:
	BEGIN
	create SAN;
	create 'textLength="';
	create textLength;
	create DoubleQuote;
	END;

   lengthAdjust = spacing_:
	BEGIN
	create SAN;
	create 'lengthAdjust="spacing"';
	END;
   lengthAdjust = spacingAndGlyphs:
	BEGIN
	create SAN;
	create 'lengthAdjust="spacingAndGlyphs"';
	END;

   startOffset:
	BEGIN
	create SAN;
	create 'startOffset="';
	create startOffset;
	create DoubleQuote;
	END;

   method = align:
	BEGIN
	create SAN;
	create 'method="align"';
	END;
   method = stretch:
	BEGIN
	create SAN;
	create 'method="stretch"';
	END;

   spacing = auto:
	BEGIN
	create SAN;
	create 'spacing="auto"';
	END;
   spacing = exact:
	BEGIN
	create SAN;
	create 'spacing="exact"';
	END;

   glyphRef_:
	BEGIN
	create SAN;
	create 'glyphRef="';
	create glyphRef_;
	create DoubleQuote;
	END;

   format:
	BEGIN
	create SAN;
	create 'format="';
	create format;
	create DoubleQuote;
	END;

   refX:
	BEGIN
	create SAN;
	create 'refX="';
	create refX;
	create DoubleQuote;
	END;

   refY:
	BEGIN
	create SAN;
	create 'refY="';
	create refY;
	create DoubleQuote;
	END;

   markerUnits = strokeWidth:
	BEGIN
	create SAN;
	create 'markerUnits="strokeWidth"';
	END;
   markerUnits = userSpaceOnUse:
	BEGIN
	create SAN;
	create 'markerUnits="userSpaceOnUse"';
	END;

   markerWidth:
	BEGIN
	create SAN;
	create 'markerWidth="';
	create markerWidth;
	create DoubleQuote;
	END;

   markerHeight:
	BEGIN
	create SAN;
	create 'markerHeight="';
	create markerHeight;
	create DoubleQuote;
	END;

   orient:
	BEGIN
	create SAN;
	create 'orient="';
	create orient;
	create DoubleQuote;
	END;

   local:
	BEGIN
	create SAN;
	create 'local="';
	create local;
	create DoubleQuote;
	END;

   name:
	BEGIN
	create SAN;
	create 'name="';
	create name;
	create DoubleQuote;
	END;

   rendering_intent = auto:
	BEGIN
	create SAN;
	create 'rendering-intent="auto"';
	END;
   rendering_intent = perceptual:
	BEGIN
	create SAN;
	create 'rendering-intent="perceptual"';
	END;
   rendering_intent = relative_colorimetric:
	BEGIN
	create SAN;
	create 'rendering-intent="relative-colorimetric"';
	END;
   rendering_intent = saturation:
	BEGIN
	create SAN;
	create 'rendering-intent="saturation"';
	END;
   rendering_intent = absolute_colorimetric:
	BEGIN
	create SAN;
	create 'rendering-intent="absolute-colorimetric"';
	END;

   gradientUnits = userSpaceOnUse:
	BEGIN
	create SAN;
	create 'gradientUnits="userSpaceOnUse"';
	END;
   gradientUnits = objectBoundingBox:
	BEGIN
	create SAN;
	create 'gradientUnits="objectBoundingBox"';
	END;

   gradientTransform:
	BEGIN
	create SAN;
	create 'gradientTransform="';
	create gradientTransform;
	create DoubleQuote;
	END;

   spreadMethod = pad:
	BEGIN
	create SAN;
	create 'spreadMethod="pad"';
	END;
   spreadMethod = reflect:
	BEGIN
	create SAN;
	create 'spreadMethod="reflect"';
	END;
   spreadMethod = repeat:
	BEGIN
	create SAN;
	create 'spreadMethod="repeat"';
	END;

   fx:
	BEGIN
	create SAN;
	create 'fx="';
	create fx;
	create DoubleQuote;
	END;

   fy:
	BEGIN
	create SAN;
	create 'fy="';
	create fy;
	create DoubleQuote;
	END;

   offset:
	BEGIN
	create SAN;
	create 'offset="';
	create offset;
	create DoubleQuote;
	END;

   patternUnits = userSpaceOnUse:
	BEGIN
	create SAN;
	create 'patternUnits="userSpaceOnUse"';
	END;
   patternUnits = objectBoundingBox:
	BEGIN
	create SAN;
	create 'patternUnits="objectBoundingBox"';
	END;

   patternContentUnits = userSpaceOnUse:
	BEGIN
	create SAN;
	create 'patternContentUnits="userSpaceOnUse"';
	END;
   patternContentUnits = objectBoundingBox:
	BEGIN
	create SAN;
	create 'patternContentUnits="objectBoundingBox"';
	END;

   patternTransform:
	BEGIN
	create SAN;
	create 'patternTransform="';
	create patternTransform;
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

   maskUnits = userSpaceOnUse:
	BEGIN
	create SAN;
	create 'maskUnits="userSpaceOnUse"';
	END;
   maskUnits = objectBoundingBox:
	BEGIN
	create SAN;
	create 'maskUnits="objectBoundingBox"';
	END;

   maskContentUnits = userSpaceOnUse:
	BEGIN
	create SAN;
	create 'maskContentUnits="userSpaceOnUse"';
	END;
   maskContentUnits = objectBoundingBox:
	BEGIN
	create SAN;
	create 'maskContentUnits="objectBoundingBox"';
	END;

  filterUnits = userSpaceOnUse:
	BEGIN
	create SAN;
	create 'filterUnits="userSpaceOnUse"';
	END;
  filterUnits = objectBoundingBox:
	BEGIN
	create SAN;
	create 'filterUnits="objectBoundingBox"';
	END;

  primitiveUnits = userSpaceOnUse:
	BEGIN
	create SAN;
	create 'primitiveUnits="userSpaceOnUse"';
	END;
  primitiveUnits = objectBoundingBox:
	BEGIN
	create SAN;
	create 'primitiveUnits="objectBoundingBox"';
	END;

  filterRes:
	BEGIN
	create SAN;
	create 'filterRes="';
	create filterRes;
	create DoubleQuote;
	END;

  azimuth:
	BEGIN
	create SAN;
	create 'azimuth="';
	create azimuth;
	create DoubleQuote;
	END;

  elevation:
	BEGIN
	create SAN;
	create 'elevation="';
	create elevation;
	create DoubleQuote;
	END;

  z:
	BEGIN
	create SAN;
	create 'z="';
	create z;
	create DoubleQuote;
	END;

  pointsAtX:
	BEGIN
	create SAN;
	create 'pointsAtX="';
	create pointsAtX;
	create DoubleQuote;
	END;

  pointsAtY:
	BEGIN
	create SAN;
	create 'pointsAtY="';
	create pointsAtY;
	create DoubleQuote;
	END;

  pointsAtZ:
	BEGIN
	create SAN;
	create 'pointsAtZ="';
	create pointsAtZ;
	create DoubleQuote;
	END;

  specularExponent:
	BEGIN
	create SAN;
	create 'specularExponent="';
	create specularExponent;
	create DoubleQuote;
	END;

  limitingConeAngle:
	BEGIN
	create SAN;
	create 'limitingConeAngle="';
	create limitingConeAngle;
	create DoubleQuote;
	END;

  result:
	BEGIN
	create SAN;
	create 'result="';
	create result;
	create DoubleQuote;
	END;

  in_:
	BEGIN
	create SAN;
	create 'in="';
	create in_;
	create DoubleQuote;
	END;

  in2:
	BEGIN
	create SAN;
	create 'in2="';
	create in2;
	create DoubleQuote;
	END;

  mode = normal_:
	BEGIN
	create SAN;
	create 'mode="normal"';
	END;
  mode = multiply:
	BEGIN
	create SAN;
	create 'mode="multiply"';
	END;
  mode = screen:
	BEGIN
	create SAN;
	create 'mode="screen"';
	END;
  mode = darken:
	BEGIN
	create SAN;
	create 'mode="darken"';
	END;
  mode = lighten:
	BEGIN
	create SAN;
	create 'mode="lighten"';
	END;

  type__ = matrix:
	BEGIN
	create SAN;
	create 'type="matrix"';
	END;
  type__ = saturate:
	BEGIN
	create SAN;
	create 'type="saturate"';
	END;
  type__ = hueRotate:
	BEGIN
	create SAN;
	create 'type="hueRotate"';
	END;
  type__ = luminanceToAlpha:
	BEGIN
	create SAN;
	create 'type="luminanceToAlpha"';
	END;

  values:
	BEGIN
	create SAN;
	create 'values="';
	create values;
	create DoubleQuote;
	END;

  type___ = identity:
	BEGIN
	create SAN;
	create 'type="identity"';
	END;
  type___ = table:
	BEGIN
	create SAN;
	create 'type="table"';
	END;
  type___ = discrete:
	BEGIN
	create SAN;
	create 'type="discrete"';
	END;
  type___ = linear:
	BEGIN
	create SAN;
	create 'type="linear"';
	END;
  type___ = gamma:
	BEGIN
	create SAN;
	create 'type="gamma"';
	END;

  tableValues:
	BEGIN
	create SAN;
	create 'tableValues="';
	create tableValues;
	create DoubleQuote;
	END;

  slope_:
	BEGIN
	create SAN;
	create 'slope="';
	create slope_;
	create DoubleQuote;
	END;

  intercept:
	BEGIN
	create SAN;
	create 'intercept="';
	create intercept;
	create DoubleQuote;
	END;

  amplitude:
	BEGIN
	create SAN;
	create 'amplitude="';
	create amplitude;
	create DoubleQuote;
	END;

  exponent:
	BEGIN
	create SAN;
	create 'exponent="';
	create exponent;
	create DoubleQuote;
	END;

  offset_:
	BEGIN
	create SAN;
	create 'offset="';
	create offset_;
	create DoubleQuote;
	END;

  operator = over:
	BEGIN
	create SAN;
	create 'operator="over"';
	END;
  operator = in__:
	BEGIN
	create SAN;
	create 'operator="in"';
	END;
  operator = out:
	BEGIN
	create SAN;
	create 'operator="out"';
	END;
  operator = atop:
	BEGIN
	create SAN;
	create 'operator="atop"';
	END;
  operator = xor:
	BEGIN
	create SAN;
	create 'operator="xor"';
	END;
  operator = arithmetic:
	BEGIN
	create SAN;
	create 'operator="arithmetic"';
	END;

  k1:
	BEGIN
	create SAN;
	create 'k1="';
	create k1;
	create DoubleQuote;
	END;

  k2:
	BEGIN
	create SAN;
	create 'k2="';
	create k2;
	create DoubleQuote;
	END;

  k3:
	BEGIN
	create SAN;
	create 'k3="';
	create k3;
	create DoubleQuote;
	END;

  k4:
	BEGIN
	create SAN;
	create 'k4="';
	create k4;
	create DoubleQuote;
	END;

  order:
	BEGIN
	create SAN;
	create 'order="';
	create order;
	create DoubleQuote;
	END;

  kernelMatrix:
	BEGIN
	create SAN;
	create 'kernelMatrix="';
	create kernelMatrix;
	create DoubleQuote;
	END;

  divisor:
	BEGIN
	create SAN;
	create 'divisor="';
	create divisor;
	create DoubleQuote;
	END;

  bias:
	BEGIN
	create SAN;
	create 'bias="';
	create bias;
	create DoubleQuote;
	END;

  targetX:
	BEGIN
	create SAN;
	create 'targetX="';
	create targetX;
	create DoubleQuote;
	END;

  targetY:
	BEGIN
	create SAN;
	create 'targetY="';
	create targetY;
	create DoubleQuote;
	END;

  edgeMode = duplicate:
	BEGIN
	create SAN;
	create 'edgeMode="duplicate"';
	END;
  edgeMode = wrap:
	BEGIN
	create SAN;
	create 'edgeMode="wrap"';
	END;
  edgeMode = none:
	BEGIN
	create SAN;
	create 'edgeMode="none"';
	END;

  kernelUnitLength:
	BEGIN
	create SAN;
	create 'kernelUnitLength="';
	create kernelUnitLength;
	create DoubleQuote;
	END;

  preserveAlpha:
	BEGIN
	create SAN;
	create 'preserveAlpha="';
	create preserveAlpha;
	create DoubleQuote;
	END;

  surfaceScale:
	BEGIN
	create SAN;
	create 'surfaceScale="';
	create surfaceScale;
	create DoubleQuote;
	END;

  diffuseConstant:
	BEGIN
	create SAN;
	create 'diffuseConstant="';
	create diffuseConstant;
	create DoubleQuote;
	END;

  scale_:
	BEGIN
	create SAN;
	create 'scale="';
	create scale_;
	create DoubleQuote;
	END;

  xChannelSelector = R:
	BEGIN
	create SAN;
	create 'xChannelSelector="R"';
	END;
  xChannelSelector = G:
	BEGIN
	create SAN;
	create 'xChannelSelector="G"';
	END;
  xChannelSelector = B:
	BEGIN
	create SAN;
	create 'xChannelSelector="B"';
	END;
  xChannelSelector = A:
	BEGIN
	create SAN;
	create 'xChannelSelector="A"';
	END;

  yChannelSelector = R:
	BEGIN
	create SAN;
	create 'yChannelSelector="R"';
	END;
  yChannelSelector = G:
	BEGIN
	create SAN;
	create 'yChannelSelector="G"';
	END;
  yChannelSelector = B:
	BEGIN
	create SAN;
	create 'yChannelSelector="B"';
	END;
  yChannelSelector = A:
	BEGIN
	create SAN;
	create 'yChannelSelector="A"';
	END;

  stdDeviation:
	BEGIN
	create SAN;
	create 'stdDeviation="';
	create stdDeviation;
	create DoubleQuote;
	END;

  operator_ = erode:
	BEGIN
	create SAN;
	create 'operator="erode"';
	END;
  operator_ = dilate:
	BEGIN
	create SAN;
	create 'operator="dilate"';
	END;

  radius:
	BEGIN
	create SAN;
	create 'radius="';
	create radius;
	create DoubleQuote;
	END;

  specularConstant:
	BEGIN
	create SAN;
	create 'specularConstant="';
	create specularConstant;
	create DoubleQuote;
	END;

  baseFrequency:
	BEGIN
	create SAN;
	create 'baseFrequency="';
	create baseFrequency;
	create DoubleQuote;
	END;

  numOctaves:
	BEGIN
	create SAN;
	create 'numOctaves="';
	create numOctaves;
	create DoubleQuote;
	END;

  seed:
	BEGIN
	create SAN;
	create 'seed="';
	create seed;
	create DoubleQuote;
	END;

  stitchTiles = stitch:
	BEGIN
	create SAN;
	create 'stitchTiles="stitch"';
	END;
  stitchTiles = noStitch:
	BEGIN
	create SAN;
	create 'stitchTiles="noStitch"';
	END;

  type____ = fractalNoise:
	BEGIN
	create SAN;
	create 'type="fractalNoise"';
	END;
  type____ = turbulence:
	BEGIN
	create SAN;
	create 'type="turbulence"';
	END;

   target_:
	BEGIN
	create SAN;
	create 'target="';
	create target_;
	create DoubleQuote;
	END;

  viewTarget:
	BEGIN
	create SAN;
	create 'viewTarget="';
	create viewTarget;
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

  horiz_origin_x:
	BEGIN
	create SAN;
	create 'horiz-origin-x="';
	create horiz_origin_x;
	create DoubleQuote;
	END;

  horiz_origin_y:
	BEGIN
	create SAN;
	create 'horiz-origin-y="';
	create horiz_origin_y;
	create DoubleQuote;
	END;

  horiz_adv_x:
	BEGIN
	create SAN;
	create 'horiz-adv-x="';
	create horiz_adv_x;
	create DoubleQuote;
	END;

  vert_origin_x:
	BEGIN
	create SAN;
	create 'vert-origin-x="';
	create vert_origin_x;
	create DoubleQuote;
	END;

  vert_origin_y:
	BEGIN
	create SAN;
	create 'vert-origin-y="';
	create vert_origin_y;
	create DoubleQuote;
	END;

  vert_adv_y:
	BEGIN
	create SAN;
	create 'vert-adv-y="';
	create vert_adv_y;
	create DoubleQuote;
	END;

  unicode:
	BEGIN
	create SAN;
	create 'unicode="';
	create unicode;
	create DoubleQuote;
	END;

  glyph_name:
	BEGIN
	create SAN;
	create 'glyph-name="';
	create glyph_name;
	create DoubleQuote;
	END;

  orientation:
	BEGIN
	create SAN;
	create 'orientation="';
	create orientation;
	create DoubleQuote;
	END;

  arabic_form:
	BEGIN
	create SAN;
	create 'arabic-form="';
	create arabic_form;
	create DoubleQuote;
	END;

  lang:
	BEGIN
	create SAN;
	create 'lang="';
	create lang;
	create DoubleQuote;
	END;

  u1:
	BEGIN
	create SAN;
	create 'u1="';
	create u1;
	create DoubleQuote;
	END;

  g1:
	BEGIN
	create SAN;
	create 'g1="';
	create g1;
	create DoubleQuote;
	END;

  u2:
	BEGIN
	create SAN;
	create 'u2="';
	create u2;
	create DoubleQuote;
	END;

  g2:
	BEGIN
	create SAN;
	create 'g2="';
	create g2;
	create DoubleQuote;
	END;

  k:
	BEGIN
	create SAN;
	create 'k="';
	create k;
	create DoubleQuote;
	END;

  font_stretch_:
	BEGIN
	create SAN;
	create 'font-stretch="';
	create font_stretch_;
	create DoubleQuote;
	END;

  unicode_range:
	BEGIN
	create SAN;
	create 'unicode-range="';
	create unicode_range;
	create DoubleQuote;
	END;

  units_per_em:
	BEGIN
	create SAN;
	create 'units-per-em="';
	create units_per_em;
	create DoubleQuote;
	END;

  panose_1:
	BEGIN
	create SAN;
	create 'panose-1="';
	create panose_1;
	create DoubleQuote;
	END;

  stemv:
	BEGIN
	create SAN;
	create 'panose_1="';
	create panose_1;
	create DoubleQuote;
	END;

  stemh:
	BEGIN
	create SAN;
	create 'stemh="';
	create stemh;
	create DoubleQuote;
	END;

  slope:
	BEGIN
	create SAN;
	create 'slope="';
	create slope;
	create DoubleQuote;
	END;

  cap_height:
	BEGIN
	create SAN;
	create 'cap-height="';
	create cap_height;
	create DoubleQuote;
	END;

  x_height:
	BEGIN
	create SAN;
	create 'x-height="';
	create x_height;
	create DoubleQuote;
	END;

  accent_height:
	BEGIN
	create SAN;
	create 'accent-height="';
	create accent_height;
	create DoubleQuote;
	END;

  ascent:
	BEGIN
	create SAN;
	create 'ascent="';
	create ascent;
	create DoubleQuote;
	END;

  descent:
	BEGIN
	create SAN;
	create 'descent="';
	create descent;
	create DoubleQuote;
	END;

  widths:
	BEGIN
	create SAN;
	create 'widths="';
	create widths;
	create DoubleQuote;
	END;

  bbox:
	BEGIN
	create SAN;
	create 'bbox="';
	create bbox;
	create DoubleQuote;
	END;

  ideographic:
	BEGIN
	create SAN;
	create 'ideographic="';
	create ideographic;
	create DoubleQuote;
	END;

  alphabetic:
	BEGIN
	create SAN;
	create 'alphabetic="';
	create alphabetic;
	create DoubleQuote;
	END;

  mathematical:
	BEGIN
	create SAN;
	create 'mathematical="';
	create mathematical;
	create DoubleQuote;
	END;

  hanging:
	BEGIN
	create SAN;
	create 'hanging="';
	create hanging;
	create DoubleQuote;
	END;

  v_ideographic:
	BEGIN
	create SAN;
	create 'v-ideographic="';
	create v_ideographic;
	create DoubleQuote;
	END;

  v_alphabetic:
	BEGIN
	create SAN;
	create 'v-alphabetic="';
	create v_alphabetic;
	create DoubleQuote;
	END;

  v_mathematical:
	BEGIN
	create SAN;
	create 'v-mathematical="';
	create v_mathematical;
	create DoubleQuote;
	END;

  v_hanging:
	BEGIN
	create SAN;
	create 'v-hanging="';
	create v_hanging;
	create DoubleQuote;
	END;

  underline_position:
	BEGIN
	create SAN;
	create 'underline-position="';
	create underline_position;
	create DoubleQuote;
	END;

  underline_thickness:
	BEGIN
	create SAN;
	create 'underline-thickness="';
	create underline_thickness;
	create DoubleQuote;
	END;

  strikethrough_position:
	BEGIN
	create SAN;
	create 'strikethrough-position="';
	create strikethrough_position;
	create DoubleQuote;
	END;

  strikethrough_thickness:
	BEGIN
	create SAN;
	create 'strikethrough-thickness="';
	create strikethrough_thickness;
	create DoubleQuote;
	END;

  overline_position:
	BEGIN
	create SAN;
	create 'overline-position="';
	create overline_position;
	create DoubleQuote;
	END;

  overline_thickness:
	BEGIN
	create SAN;
	create 'overline-thickness="';
	create overline_thickness;
	create DoubleQuote;
	END;

  string:
	BEGIN
	create SAN;
	create 'string="';
	create string;
	create DoubleQuote;
	END;

TextTRANSLATE
	BEGIN
	'\46' -> '&amp;';	{ ampersand, U+0026 ISOnum }
	'\74' -> '&lt;';	{ less-than sign, U+003C ISOnum }
	'\76' -> '&gt;'; 	{ greater-than sign, U+003E ISOnum }
	END;

END
