
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

   SVG
	(ATTR viewBox = text;
	      x = text;
              y = text;
	      height_ = text;
	      width_ = text;
	      Charset = text)
        = LIST OF (GraphicsElement) + (XMLcomment, XMLPI, Unknown_namespace);

   g
        (ATTR fill = text;
	      stroke = text;
	      stroke_width = text;
	      transform = text)
        = LIST OF (GraphicsElement);

   defs
        (ATTR requiredFeatures = text;
	      requiredExtensions = text;
	      systemLanguage = text;
	      externalResourcesRequired = false, true;
	      fill;
	      stroke;
	      stroke_width;
	      transform;
	      { graphicsElementEvents })
        = LIST OF (GraphicsElement);

   path
	(ATTR externalResourcesRequired; 
	      fill;
              stroke;
	      stroke_width;
              transform;
	      d = text;
	      pathLength = text)
        = AGGREGATE
	      desc; title; metadata;
	      GRAPHICS;
	      END;

   rect
        (ATTR transform;
	      x;
              y;
              width_;
              height_;
	      rx = text;
	      ry = text;
	      fill;
              stroke;
	      stroke_width)
        = AGGREGATE
	      desc; title; metadata;
	      GRAPHICS;
	      END;
	
   circle
        (ATTR transform;
	      cx = text;
	      cy = text;
              r = text;
	      fill;
              stroke;
	      stroke_width)
        = AGGREGATE
	      desc; title; metadata;
	      GRAPHICS;
	      END;
	
   ellipse
        (ATTR transform;
	      cx;
	      cy;
	      rx;
	      ry;
	      fill;
              stroke;
	      stroke_width)
        = AGGREGATE
	      desc; title; metadata;
	      GRAPHICS;
	      END;
	
   line_
        (ATTR transform;
	      x1 = text;
	      y1 = text;
              x2 = text;
	      y2 = text;
	      fill;
              stroke;
	      stroke_width)
        = AGGREGATE
	      desc; title; metadata;
	      GRAPHICS;
	      END;
	
   polyline
        (ATTR width_;
              height_;
	      transform;
              points = text;
	      fill;
              stroke;
	      stroke_width)
        = AGGREGATE
	      desc; title; metadata;
	      GRAPHICS;
	      END;
	
   polygon
        (ATTR width_;
              height_;
	      transform;
              points;
	      fill;
              stroke;
	      stroke_width)
        = AGGREGATE
	      desc; title; metadata;
	      GRAPHICS;
	      END;

   text_
        (ATTR transform;
	      x;
	      y)
         = LIST OF (TextComponent);

   tspan
	(ATTR x;
              y;
	      dx = text;
	      dy = text)
	= LIST OF (SpanElement);

   { tref }
   { textPath }
   { altGlyph }
   { altGlyphDef }
   { glyphSub }
   { font }
   { glyph }
   { missing-glyph }
   { hkern }
   { vkern }

   use_
        (ATTR xlink_href = text;
	      requiredFeatures; requiredExtensions; systemLanguage;
	      externalResourcesRequired;
	      fill;
	      stroke;
	      stroke_width;
	      transform;
	      { graphicsElementEvents }
	      x; 
	      y;
              width_;
              height_)
        = AGGREGATE
              desc; title; metadata;
	      END;
	
   image
	(ATTR xlink_href;
	      transform;
	      x; 
	      y;
              width_;
              height_)
        = AGGREGATE
              desc; ? title; ? metadata;
              PICTURE;
              SVG_Image;
	      END;

   symbol_
	(ATTR externalResourcesRequired;
	      fill;
	      stroke;
	      stroke_width;
	      viewBox;
	      preserveAspectRatio = text; 
	      { graphicsElementEvents })
        = LIST OF (GraphicsElement);
	      
   { marker }
   
   desc = TEXT;
	
   title = TEXT;
	
   { clipPath }
   { mask }
   { linearGradient }
   { radialGradient }
   { stop }
   { pattern }

   a
        (ATTR xlink_href;
	      requiredFeatures; requiredExtensions; systemLanguage;
	      externalResourcesRequired;
	      fill;
              stroke;
	      stroke_width;
	      transform;
	      { graphicsElementEvents }
	      target_ = text)
	= LIST OF (CASE OF
		      GraphicsElement; TextComponent;
	              END);

   { view }
   { animate }
   { set }
   { animateMotion }
   { animateColor }
   { animateTransform }

   script
	(ATTR externalResourcesRequired;
	      type = text)
	= TEXT;

   style__
	(ATTR type;
	      media = text;
	      title_ = text)
	= TEXT;

   { color-profile }
   { color-profile-src }
   { font-face }
   { font-face-src }
   { font-face-uri }
   { font-face-format }
   { font-face-name }
   { definition-src }
   { cursor }
	
   switch
	(ATTR requiredFeatures; requiredExtensions; systemLanguage;
	      externalResourcesRequired;
	      { PresentationAttributes-All }
	      transform;
	      { graphicsElementEvents })
	= LIST OF (CASE OF
		      GraphicsElement; foreignObject;
	              END);

   foreignObject
        (ATTR transform;
	      x; 
              y;
              width_;
              height_; 
              fill;
              stroke)
	= BEGIN CASE OF
	      HTML; MathML;
	      END;
          END;
	              
   metadata = TEXT;

   { filter }
   { ... }

   GraphicsElement
	= CASE OF
	      desc; title; metadata;
	      defs;
	      path; text_; rect; circle; ellipse; line_; polyline; polygon;
	      use_; image; SVG; g; switch; a;
	      script; style__; symbol_;
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

   SpanElement
	= CASE OF
	      TEXT;
	      tspan; 
	      desc;
	      title;
	      metadata;
	      END;
	
   SVG_Image = SVG;
   
   XMLcomment = LIST OF (XMLcomment_line = TEXT);

   XMLPI = LIST OF (XMLPI_line = TEXT);

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
   script:	        NoMove, NoResize, NoShowBox, NoCreate;
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

END



