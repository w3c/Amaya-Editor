
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

{ Document Structure }

   SVG
	(ATTR requiredFeatures = text;
	      requiredExtensions = text;
	      systemLanguage = text;
	      baseProfile = text;
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
	      text_anchor = start, middle, end__, inherit;
	      text_decoration = text;
	      unicode_bidi = normal_, embed_, bidi_override, inherit;
	      viewBox = text;
	      version = text;
	      x = text;
              y = text; 
	      width_ = text;
	      height_ = text;
	      opacity = text; 
	      RealLang = Yes_;
	      Charset = text)
        = LIST OF (GraphicsElement) + (XMLcomment, XMLPI, CDATA,
	                               Unknown_namespace);

   g
        (ATTR requiredFeatures; requiredExtensions; systemLanguage;
	      externalResourcesRequired; 
	      fill; stroke; stroke_width;
	      opacity; 
	      font_family; font_size; font_style; font_variant; font_weight;
	      direction_; text_anchor; text_decoration; unicode_bidi;
	      transform = text)
        = LIST OF (GraphicsElement);

   defs
        (ATTR requiredFeatures; requiredExtensions; systemLanguage;
	      externalResourcesRequired;
	      fill; stroke; stroke_width;
	      font_family; font_size; font_style; font_variant; font_weight;
	      direction_; text_anchor; text_decoration; unicode_bidi;
	      transform;
	      { graphicsElementEvents })
        = LIST OF (GraphicsElement);

   desc = TEXT;
	
   title = TEXT;
	
   symbol_  { not in SVG Tiny }
	(ATTR externalResourcesRequired;
	      fill; stroke; stroke_width;
	      font_family; font_size; font_style; font_variant; font_weight;
	      direction_; text_anchor; text_decoration; unicode_bidi;
	      viewBox;
	      preserveAspectRatio = text; 
	      { graphicsElementEvents })
        = LIST OF (GraphicsElement);

   use_  { not in SVG Tiny }
        (ATTR xlink_href = text;
	      requiredFeatures; requiredExtensions; systemLanguage;
	      externalResourcesRequired;
	      opacity; fill; stroke; stroke_width;
	      font_family; font_size; font_style; font_variant; font_weight;
	      direction_; text_anchor; text_decoration; unicode_bidi;
	      transform;
	      { graphicsElementEvents }
	      x; 
	      y;
              width_;
              height_)
        = AGGREGATE
              desc; title; metadata;
	      LIST OF (Anim);
	      END;
	
{ Images }

   image
	(ATTR xlink_href;
	      requiredFeatures; requiredExtensions; systemLanguage;
	      externalResourcesRequired; 
	      transform;
	      opacity; 
	      x; 
	      y;
              width_;
              height_)		
        = AGGREGATE
              desc; ? title; ? metadata;
	      LIST OF (Anim);
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
	      direction_; text_anchor; text_decoration; unicode_bidi;
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

{ Paths }

   path
	(ATTR requiredFeatures; requiredExtensions; systemLanguage;
	      externalResourcesRequired; 
	      opacity; 
	      fill_opacity=text; 
	      stroke_opacity=text; 
	      fill; stroke; stroke_width;
              transform;
	      d = text;
	      pathLength = text)
        = AGGREGATE
	      desc; title; metadata;
	      LIST OF (Anim);
	      GRAPHICS;
	      END;
	
{ Basic Shapes }

   rect
        (ATTR requiredFeatures; requiredExtensions; systemLanguage;
	      externalResourcesRequired; 
	      fill; stroke; stroke_width;
	      transform;
	      x;
              y;
              width_;
              height_;
	      opacity; fill_opacity; stroke_opacity;
	      rx = text;
	      ry = text)
        = AGGREGATE
	      desc; title; metadata;
	      LIST OF (Anim);
	      GRAPHICS;
	      END;
	
   circle
        (ATTR requiredFeatures; requiredExtensions; systemLanguage;
	      externalResourcesRequired; 
	      fill; stroke; stroke_width;
	      transform;
	      cx = text;
	      cy = text;
              r = text;
	      opacity; fill_opacity; stroke_opacity)
        = AGGREGATE
	      desc; title; metadata;
	      LIST OF (Anim);
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
	      ry;
	      opacity; fill_opacity; stroke_opacity)
        = AGGREGATE
	      desc; title; metadata;
	      LIST OF (Anim);
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
	      y2 = text;
	      opacity; fill_opacity; stroke_opacity)
        = AGGREGATE
	      desc; title; metadata;
	      LIST OF (Anim);
	      GRAPHICS;
	      END;
	
   polyline
        (ATTR requiredFeatures; requiredExtensions; systemLanguage;
	      externalResourcesRequired; 
	      fill; stroke; stroke_width;
	      transform;
              points = text;
	      opacity; fill_opacity; stroke_opacity)
        = AGGREGATE
	      desc; title; metadata;
	      LIST OF (Anim);
	      GRAPHICS;
	      END;
	
   polygon
        (ATTR requiredFeatures; requiredExtensions; systemLanguage;
	      externalResourcesRequired; 
	      fill; stroke; stroke_width;
	      transform;
              points;
	      opacity; fill_opacity; stroke_opacity)
        = AGGREGATE
	      desc; title; metadata;
	      LIST OF (Anim);
	      GRAPHICS;
	      END;

{ Text }
	
   text_
        (ATTR requiredFeatures; requiredExtensions; systemLanguage;
	      externalResourcesRequired; 
	      fill; stroke; stroke_width;
	      font_family; font_size; font_style; font_variant; font_weight;
	      direction_; text_anchor; text_decoration; unicode_bidi;
	      transform;
	      writing_mode = lr_tb, rl_tb, tb_rl, lr, rl, tb, inherit;
	      x;
	      y;
	      dx = text;
	      dy = text;
	      opacity)
        = LIST OF (CASE OF
		   tspan;	{ must be the first option, to allow the Return key
				  to create tspan elements }
		   TEXT; desc; title; metadata;
	           tref; a;
	           animate; set_; animateMotion; animateColor; animateTransform;
		   END);

   tspan  { not in SVG Tiny }
	(ATTR requiredFeatures; requiredExtensions; systemLanguage;
	      externalResourcesRequired; 
	      fill; stroke; stroke_width;
	      font_family; font_size; font_style; font_variant; font_weight;
	      baseline_shift = text;
	      direction_; text_anchor; text_decoration; unicode_bidi;
	      writing_mode;
	      x;
              y;
	      dx;
	      dy;
	      opacity)
	= LIST OF (SpanElement = CASE OF
		      TEXT;
		      desc; title; metadata;
		      tspan; tref;
		      a;
		      animate; set_; animateColor;
		      END);
   tref
	(ATTR xlink_href;
	      requiredFeatures; requiredExtensions; systemLanguage;
	      externalResourcesRequired; 
	      fill; stroke; stroke_width;
	      font_family; font_size; font_style; font_variant; font_weight;
	      baseline_shift;
	      direction_; text_anchor; text_decoration; unicode_bidi;
	      writing_mode;
	      x;
              y;
	      dx;
	      dy;
	      opacity)
	= LIST OF (CASE OF
		   desc; title; metadata;
	           animate; set_; animateColor;
		   END);

   { textPath }
   { altGlyph }
   { altGlyphDef }
   { altGlyphItem }
   { glyphRef }

{ Marker Symbols }

   { marker }

{ Color }

   { color-profile }

{ Gradients and Patterns }

   linearGradient
	(ATTR requiredFeatures; requiredExtensions; systemLanguage;
	      externalResourcesRequired;
	      x1; y1; x1; y2;	      
	      gradientTransform = text;
	      gradientUnits = text;
              xlink_href;)
	= AGGREGATE
	      desc; title; metadata;
	 LIST OF 
	        (CASE OF
	           path; rect; circle; ellipse; line_; polyline; polygon;
	           END);
		GRAPHICS;
	  END;

   stop
         (ATTR requiredFeatures; requiredExtensions; systemLanguage;
	      externalResourcesRequired;
	      offset=text;
	      style=text; 
	      stop_color=text;)
	 = AGGREGATE
	      desc; title; metadata;
	 LIST OF 
	        (CASE OF
		   linearGradient;
	           END);
	  END;
	

   { radialGradient }
   { pattern }

{ Clipping, Masking and Compositing }

   clipPath
        (ATTR requiredFeatures; requiredExtensions; systemLanguage;
	      externalResourcesRequired;
	      fill; stroke; stroke_width;
	      font_family; font_size; font_style; font_variant; font_weight;
	      direction_; text_anchor; text_decoration; unicode_bidi;
	      transform;
	      clipPathUnits = userSpaceOnUse, objectBoundingBox;)
	= AGGREGATE
	    desc; title; metadata;
	    LIST OF
	      (CASE OF
	         path; text_; rect; circle; ellipse; line_; polyline; polygon;
	         use_; animate; set_; animateMotion; animateColor;
	         animateTransform;
	       END);
	  END;

   { mask }

{ Filter Effects }

   { filter }
   { feDistantLight }
   { fePointLight }
   { feSpotLight }
   { feBlend }
   { feColorMatrix }
   { feComponentTransfer }
   { feFuncR }
   { feFuncG }
   { feFuncB }
   { feFuncA }
   { feComposite }
   { feConvolveMatrix }
   { feDiffuseLighting }
   { feDisplacementMap }
   { feFlood }
   { feGaussianBlur }
   { feImage }
   { feMerge }
   { feMergeNode }
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
	      direction_; text_anchor; text_decoration; unicode_bidi;
	      transform;
	      { graphicsElementEvents }
	      target_ = text)
	= LIST OF (CASE OF
		   TEXT; 
		   GraphicsElement;
	           END);
   { view }

{ Scripting }

   script_  { not in SVG Tiny }
	(ATTR externalResourcesRequired;
	      type)
	= TEXT;

{ Animation }

   animate
	(ATTR requiredFeatures; requiredExtensions; systemLanguage; {testAttrs}
	      externalResourcesRequired;
	      onbegin = text;  {animationEvents}
	      onend = text;
	      onrepeat = text;
	      {animElementAttrs - see XLink.S}
              xlink_href;
              attributeName_ = text;  {animAttributeAttrs}
	      attributeType = text;
	      begin_ = text;  {animTimingAttrs}
	      dur = text;
	      end_ = text;
	      min_ = text;
	      max_ = text;
	      restart = always, never, whenNotActive;
	      repeatCount = text;
	      repeatDur = text;
	      fill_ = remove_, freeze;
              calcMode = discrete, linear, paced, spline;  {animValueAttrs}
	      values = text;
	      keyTimes = text;
	      keySplines = text;
	      from = text;
	      to_ = text;
	      by = text;
              additive = replace, sum;  {animAdditionAttrs}
	      accumulate = none_, sum;)
	= AGGREGATE
	      desc; title; metadata;
	  END;

   set_
	(ATTR requiredFeatures; requiredExtensions; systemLanguage; {testAttrs}
	      externalResourcesRequired;
	      onbegin; onend; onrepeat; {animationEvents}
	      xlink_href; {animElementAttrs - see XLink.S}
              attributeName_; attributeType; {animAttributeAttrs}
	      begin_; dur; end_; min_; max_; restart; repeatCount; repeatDur; fill_;
	                                                      {animTimingAttrs}
	      to_;)
	= AGGREGATE
	      desc; title; metadata;
	  END;

   animateMotion
	(ATTR requiredFeatures; requiredExtensions; systemLanguage; {testAttrs}
	      externalResourcesRequired;
	      onbegin; onend; onrepeat; {animationEvents}
	      xlink_href; {animElementAttrs - see XLink.S}
              attributeName_; attributeType; {animAttributeAttrs}
	      begin_; dur; end_; min_; max_; restart; repeatCount; repeatDur; fill_;
	                                                      {animTimingAttrs}
	      calcMode; values; keyTimes; keySplines; from; to_; by;
              additive; accumulate; {animAdditionAttrs}
	      path_ = text;
              keyPoints = text;
              rotate = text;
              origin = text;)
	= AGGREGATE
	      desc; title; metadata; ? mpath;
	  END;

   mpath
	(ATTR
	   xlink_href;  {xlinkRefAttrs - see XLink.S}
	   externalResourcesRequired;)
	= AGGREGATE
	      desc; title; metadata;
	  END;

   animateColor
	(ATTR requiredFeatures; requiredExtensions; systemLanguage; {testAttrs}
	      externalResourcesRequired;
	      onbegin; onend; onrepeat; {animationEvents}
	      xlink_href; {animElementAttrs - see XLink.S}
              attributeName_; attributeType; {animAttributeAttrs}
	      begin_; dur; end_; min_; max_; restart; repeatCount; repeatDur; fill_;
	                                                      {animTimingAttrs}
              calcMode; values; keyTimes; keySplines; from; to_; by;
	                                                       {animValueAttrs}
              additive; accumulate;) {animAdditionAttrs}
	= AGGREGATE
	      desc; title; metadata;
	  END;
	
   animateTransform
	(ATTR requiredFeatures; requiredExtensions; systemLanguage; {testAttrs}
	      externalResourcesRequired;
	      onbegin; onend; onrepeat; {animationEvents}
	      xlink_href; {animElementAttrs - see XLink.S}
              attributeName_; attributeType; {animAttributeAttrs}
	      begin_; dur; end_; min_; max_; restart; repeatCount; repeatDur; fill_;
	                                                      {animTimingAttrs}
              calcMode; values; keyTimes; keySplines; from; to_; by;
	                                                       {animValueAttrs}
              additive; accumulate;  {animAdditionAttrs}
	      type_ = translate, scale, rotate_, skewX, skewY;)
	= AGGREGATE
	      desc; title; metadata;
	  END;

   Anim = CASE OF animate; set_; animateMotion; animateColor; animateTransform;
	  END;
{ Fonts }

   { font }
   { glyph }
   { missing-glyph }
   { hkern }
   { vkern }
   { font-face }
   { font-face-src }
   { font-face-uri }
   { font-face-format }
   { font-face-name }
   { definition-src }
   { cursor }

{ Metadata }
	
   metadata = TEXT;

{ Extensibility }

   foreignObject  { not in SVG Tiny }
        (ATTR requiredFeatures; requiredExtensions; systemLanguage;
	      externalResourcesRequired; 
	      opacity; fill_opacity; stroke_opacity; fill; stroke; stroke_width;
	      font_family; font_size; font_style; font_variant; font_weight;
	      direction_; text_anchor; text_decoration; unicode_bidi;
	      transform;
	      x; 
              y;
              width_;
              height_)
	= BEGIN CASE OF
	      HTML; MathML;
	      END;
          END;

{ The following elements are not defined in the SVG DTD }


  Timeline_cross
	(ATTR x; 
	      y;
	opacity; 
          width_;
          height_)
        = PICTURE;

   GraphicsElement
	= CASE OF
	      desc; title; metadata; defs;
	      path; text_; rect; circle; ellipse; line_; polyline; polygon;
	      use_; image; Timeline_cross; SVG; g; switch; a;
	      script_; style__; symbol_; clipPath; Anim;
	      animate; set_; animateMotion; animateColor; animateTransform;
	      linearGradient; stop; 
	      XLink;
	      END;

   DOCTYPE = LIST OF (DOCTYPE_line = TEXT);	
	
   XMLcomment = LIST OF (XMLcomment_line = TEXT);

   XMLPI = LIST OF (XMLPI_line = TEXT);

   CDATA = LIST OF (CDATA_line = TEXT);

   Unknown_namespace = TEXT;

EXCEPT

   SVG:                 IsDraw, MoveResize, NoMove;
   g:                   NoMove, NoResize, HighlightChildren, NoShowBox,
                        NoCreate, IsGroup;
   defs:                NoMove, NoResize, NoShowBox, NoCreate;
   linearGradient:      NoMove, NoResize, NoShowBox, NoCreate;
   stop:	        NoMove, NoResize, NoShowBox, NoCreate;
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
   switch:              NoMove, NoResize, NoShowBox, NoCreate, HighlightChildren;
   foreignObject:       MoveResize, HighlightChildren, NoCreate;
   SVG_Image:           Hidden, SelectParent;
   GRAPHICS:	        SelectParent;
   PICTURE:             NoMove, NoResize, SelectParent;
   TEXT:                NoMove, NoResize;
   Unknown_namespace:   NoCreate;
   DOCTYPE:             NoCut;
   DOCTYPE_line:        Hidden, NoSpellCheck, NoCut;
   XMLcomment_line:     Hidden, NoSpellCheck;
   XMLPI_line:          Hidden, NoSpellCheck;
   CDATA_line:          Hidden;
   XMLPI:	        ReturnCreateNL, NoReplicate;
   XMLcomment:	        ReturnCreateNL, NoReplicate;
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
   Timeline_cross:      MoveResize, NoResize, HighlightChildren, NoShowBox, NoCut;

END
