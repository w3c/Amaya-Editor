
      { Thot structure schema for Scalable Vector Graphics }

STRUCTURE SVG;

DEFPRES SVGP;

ATTR

   { global attributes for all SVG elements }
   id = text;
   class = text;
   PseudoClass = Text;
   style_ = text;
   xml_space = xml_space_default, xml_space_preserve;
   { xml_base = Text; }
	
   { global attributes for internal processing }
   Unknown_attribute = text;
   Ghost_restruct = text;
   Highlight = Yes_;        { to show the SVG element corresponding to the
			      current selection in the source view }
   Namespace = text;        { for children of element foreignObject }
   IntEmptyShape = yes_, no_;

STRUCT

{ Structure Module }

   SVG
	(ATTR requiredFeatures = text;
	      requiredExtensions = text;
	      systemLanguage = text;
	      externalResourcesRequired = false, true;
	      fill = text;
	      stroke = text;
	      stroke_width = text;
	      font_family = text;
	      font_size = text;
	      font_style = normal_, italic, oblique_, inherit;
	      font_variant = normal_, small_caps, inherit;
	      font_weight = normal_, bold_, bolder, lighter, w100, w200, w300,
			    w400, w500, w600, w700, w800, w900, inherit;
	      direction_ = ltr_, rtl_, inherit; 
	      text_decoration = text;
	      unicode_bidi = normal_, embed_, bidi_override, inherit;
	      viewBox = text;
	      x = text;
              y = text;
	      width_ = text;
	      height_ = text;
	      RealLang = Yes_;
	      Charset = text)
        = LIST OF (GraphicsElement) + (XMLcomment, XMLPI, CDATA,
	                               Unknown_namespace);

   desc = TEXT;
	
   title = TEXT;
	
   metadata = TEXT;

   g
        (ATTR requiredFeatures; requiredExtensions; systemLanguage;
	      externalResourcesRequired; 
	      fill; stroke; stroke_width;
	      font_family; font_size; font_style; font_variant; font_weight;
	      direction_; text_decoration; unicode_bidi;
	      transform = text)
        = LIST OF (GraphicsElement);

   defs
        (ATTR requiredFeatures; requiredExtensions; systemLanguage;
	      externalResourcesRequired;
	      fill; stroke; stroke_width;
	      font_family; font_size; font_style; font_variant; font_weight;
	      direction_; text_decoration; unicode_bidi;
	      transform;
	      { graphicsElementEvents })
        = LIST OF (GraphicsElement);

   symbol_  { not in SVG Tiny }
	(ATTR externalResourcesRequired;
	      fill; stroke; stroke_width;
	      font_family; font_size; font_style; font_variant; font_weight;
	      direction_; text_decoration; unicode_bidi;
	      viewBox;
	      preserveAspectRatio = text; 
	      { graphicsElementEvents })
        = LIST OF (GraphicsElement);

   use_  { not in SVG Tiny }
        (ATTR xlink_href = text;
	      requiredFeatures; requiredExtensions; systemLanguage;
	      externalResourcesRequired;
	      fill; stroke; stroke_width;
	      font_family; font_size; font_style; font_variant; font_weight;
	      direction_; text_decoration; unicode_bidi;
	      transform;
	      { graphicsElementEvents }
	      x; 
	      y;
              width_;
              height_)
        = AGGREGATE
              desc; title; metadata;
	      END;
	
{ Images }

   image
	(ATTR xlink_href;
	      requiredFeatures; requiredExtensions; systemLanguage;
	      externalResourcesRequired; 
	      transform;
	      x; 
	      y;
              width_;
              height_)
        = AGGREGATE
              desc; ? title; ? metadata;
              PICTURE;
              SVG_Image = SVG;
	      END;

{ Conditional Processing }

   switch
	(ATTR requiredFeatures; requiredExtensions; systemLanguage;
	      externalResourcesRequired;
	      { PresentationAttributes-All }
	      fill; stroke; stroke_width;
	      font_family; font_size; font_style; font_variant; font_weight;
	      direction_; text_decoration; unicode_bidi;
	      transform;
	      { graphicsElementEvents })
	= LIST OF (CASE OF
		      GraphicsElement; foreignObject;
	              END);

{ Styling }

   style__  { not in SVG Tiny }
	(ATTR type = text;
	      media = text;
	      title_ = text)
	= TEXT;

{ Shapes }

   rect
        (ATTR requiredFeatures; requiredExtensions; systemLanguage;
	      externalResourcesRequired; 
	      fill; stroke; stroke_width;
	      transform;
	      x;
              y;
              width_;
              height_;
	      rx = text;
	      ry = text)
        = AGGREGATE
	      desc; title; metadata;
	      GRAPHICS;
	      END;
	
   circle
        (ATTR requiredFeatures; requiredExtensions; systemLanguage;
	      externalResourcesRequired; 
	      fill; stroke; stroke_width;
	      transform;
	      cx = text;
	      cy = text;
              r = text)
        = AGGREGATE
	      desc; title; metadata;
	      GRAPHICS;
	      END;
	
   line_
        (ATTR requiredFeatures; requiredExtensions; systemLanguage;
	      externalResourcesRequired; 
	      fill; stroke; stroke_width;
	      transform;
	      x1 = text;
	      y1 = text;
              x2 = text;
	      y2 = text)
        = AGGREGATE
	      desc; title; metadata;
	      GRAPHICS;
	      END;
	
   ellipse
        (ATTR requiredFeatures; requiredExtensions; systemLanguage;
	      externalResourcesRequired; 
	      fill; stroke; stroke_width;
	      transform;
	      cx;
	      cy;
	      rx;
	      ry)
        = AGGREGATE
	      desc; title; metadata;
	      GRAPHICS;
	      END;
	
   polyline
        (ATTR requiredFeatures; requiredExtensions; systemLanguage;
	      externalResourcesRequired; 
	      fill; stroke; stroke_width;
	      transform;
              points = text)
        = AGGREGATE
	      desc; title; metadata;
	      GRAPHICS;
	      END;
	
   polygon
        (ATTR requiredFeatures; requiredExtensions; systemLanguage;
	      externalResourcesRequired; 
	      fill; stroke; stroke_width;
	      transform;
              points)
        = AGGREGATE
	      desc; title; metadata;
	      GRAPHICS;
	      END;

   path
	(ATTR requiredFeatures; requiredExtensions; systemLanguage;
	      externalResourcesRequired; 
	      fill; stroke; stroke_width;
              transform;
	      d = text;
	      pathLength = text)
        = AGGREGATE
	      desc; title; metadata;
	      GRAPHICS;
	      END;

{ Text }
	
   text_
        (ATTR requiredFeatures; requiredExtensions; systemLanguage;
	      externalResourcesRequired; 
	      fill; stroke; stroke_width;
	      font_family; font_size; font_style; font_variant; font_weight;
	      direction_; text_decoration; unicode_bidi;
	      transform;
	      x;
	      y;
	      dx = text;
	      dy = text)
         = LIST OF (TextComponent);

   tspan  { not in SVG Tiny }
	(ATTR requiredFeatures; requiredExtensions; systemLanguage;
	      externalResourcesRequired; 
	      fill; stroke; stroke_width;
	      font_family; font_size; font_style; font_variant; font_weight;
	      direction_; text_decoration; unicode_bidi;
	      x;
              y;
	      dx;
	      dy)
	= LIST OF (SpanElement = CASE OF
		      TEXT;
		      tspan; 
		      desc;
		      title;
		      metadata;
		      END);
	
   { tref }
   { textPath }
   { altGlyph }

{ Filters }

   { filter }
   { feBlend }
   { feFlood }
   { feColorMatrix }
   { feComponentTransfer }
   { feComposite }
   { feConvolveMatrix }
   { feDiffuseLighting }
   { feDisplacementMap }
   { feGaussianBlur }
   { feImage }
   { feMerge }
   { feMorphology }
   { feOffset }
   { feSpecularLighting }
   { feTile }
   { feTurbulence }

{ Interactivity }

   { cursor }

{ Linking }

   a
        (ATTR xlink_href;
	      requiredFeatures; requiredExtensions; systemLanguage;
	      externalResourcesRequired;
	      fill; stroke; stroke_width;
	      font_family; font_size; font_style; font_variant; font_weight;
	      direction_; text_decoration; unicode_bidi;
	      transform;
	      { graphicsElementEvents }
	      target_ = text)
	= LIST OF (CASE OF
		      GraphicsElement; TextComponent;
	              END);

{ Scripting }

   script_  { not in SVG Tiny }
	(ATTR externalResourcesRequired;
	      type)
	= TEXT;

{ Animation }

   { animate }
   { set }
   { animateMotion }
   { animateTransform }
   { animateColor }
   { mpath }

{ Fonts }

   { font }
   { font-face }
   { glyph }
   { missing-glyph }
   { hkern }
   { vkern }

{ Extensibility }

   foreignObject  { not in SVG Tiny }
        (ATTR requiredFeatures; requiredExtensions; systemLanguage;
	      externalResourcesRequired; 
	      fill; stroke; stroke_width;
	      font_family; font_size; font_style; font_variant; font_weight;
	      direction_; text_decoration; unicode_bidi;
	      transform;
	      x; 
              y;
              width_;
              height_)
	= BEGIN CASE OF
	      HTML; MathML;
	      END;
          END;

{ Clipping }

   { clipPath }

{ Masking }

   { mask }

{ Markers }

   { marker }

{ Gradients }

   { linearGradient }
   { radialGradient }
   { stop }

{ Patterns }

   { pattern }

{ Views }

   { view }

{ The following elements from SVG 1.0 does not appear in WD-SVG11-20011030 }

   { altGlyphDef }
   { altGlyphItem }
   { glyphRef }
   { color-profile }
   { font-face-src }
   { font-face-uri }
   { font-face-format }
   { font-face-name }
   { definition-src }
   { cursor }

{ The following elements are not defined in the SVG DTD }

   GraphicsElement
	= CASE OF
	      desc; title; metadata;
	      defs;
	      path; text_; rect; circle; ellipse; line_; polyline; polygon;
	      use_; image; SVG; g; switch; a;
	      script_; style__; symbol_;
	      XLink;
	      END;

   TextComponent
	= CASE OF
	      tspan;	{ must be the first option, to allow the Return key
			  to create tspan elements }
	      TEXT; 
	      desc;
	      title;
	      metadata;
	      END;

   DOCTYPE = LIST OF (DOCTYPE_line = TEXT);	
	
   XMLcomment = LIST OF (XMLcomment_line = TEXT);

   XMLPI = LIST OF (XMLPI_line = TEXT);

   CDATA = LIST OF (CDATA_line = TEXT);

   Unknown_namespace = TEXT;

EXCEPT

   SVG:                 IsDraw, MoveResize, NoMove;
   g:                   NoMove, NoResize, HighlightChildren, NoShowBox,
                        NoCreate;
   defs:                NoMove, NoResize, NoShowBox, NoCreate;
   rect:                MoveResize, HighlightChildren, NoShowBox, NoCreate;
   circle:              MoveResize, HighlightChildren, NoShowBox, NoCreate;
   ellipse:             MoveResize, HighlightChildren, NoShowBox, NoCreate;
   line_:               MoveResize, HighlightChildren, NoShowBox, NoCreate;
   polyline:            MoveResize, HighlightChildren, NoShowBox, NoCreate;
   polygon:             MoveResize, HighlightChildren, NoShowBox, NoCreate;
   path:	        MoveResize, HighlightChildren, NoShowBox, NoCreate;
   text_:               MoveResize, NoResize, NoShowBox, ReturnCreateWithin,
	                NoCreate;
   tspan:	        NoShowBox; 
   use_:	        NoMove, NoResize, HighlightChildren, NoShowBox,
	                NoCreate;
   image:	        MoveResize, HighlightChildren, NoShowBox;
   symbol_:	        NoMove, NoResize,  HighlightChildren,NoShowBox,
                        NoCreate;
   a:	                NoMove, NoResize, HighlightChildren, NoShowBox,
                        NoCreate;
   script_:	        NoMove, NoResize, NoShowBox, NoCreate;
   style__:             NoMove, NoResize, NoShowBox, NoCreate;
   switch:              NoMove, NoResize, NoShowBox, NoCreate;
   foreignObject:       MoveResize, HighlightChildren, NoCreate;
   SVG_Image:           Hidden, SelectParent;
   GRAPHICS:	        SelectParent;
   PICTURE:             NoMove, NoResize, SelectParent;
   TEXT:                NoMove, NoResize;
   Unknown_namespace:   NoCreate;

   id:                  CssId;
   class:               CssClass;
   PseudoClass:	        Invisible, CssPseudoClass;
   Unknown_attribute:   Invisible;
   Highlight:	        Invisible; 
   Ghost_restruct:      Invisible;
   Namespace:	        Invisible;
   IntEmptyShape:       Invisible;
   RealLang:	        Invisible;
   Charset:	        Invisible;
   XMLcomment_line:     Hidden;
   XMLPI_line:          Hidden;
   DOCTYPE:             NoCut;
   DOCTYPE_line:        Hidden, NoCut;
   CDATA_line:          Hidden;

END
