
      { Thot structure schema for Scalable Vector Graphics }

STRUCTURE GraphML;

DEFPRES GraphMLP;

ATTR

   { global attributes for all GraphML elements }
   id = text;
   class = text;
   style_ = text;
   xml_space = xml_space_default, xml_space_preserve;

   { global attributes for internal processing }
   Invalid_attribute = TEXT;
   Ghost_restruct = TEXT;
   Namespace = TEXT;        { for children of element foreignObject }
   IntEmptyShape = yes_, no_;

STRUCT

   GraphML
	(ATTR viewBox = text;
	      x = text;
              y = text;
	      !height_ = Text;
	      !width_ = Text)
        = LIST OF (GraphicsElement) + (XMLcomment)
          with height_ ?= '6cm', width_ ?= '10cm';

   g
        (ATTR fill = text;
	      stroke = text;
	      stroke_width = text;
	      transform = TEXT;
              linestyle_ = solid_, dashed_, dotted_)
        = LIST OF (GraphicsElement);

   { defs }
   { path }
	
   Spline
        (ATTR width_;
              height_;
	      transform;
              points = text;
              arrowhead = none_, start, end_, both;
              linestyle_)
        = GRAPHICS;
	
   ClosedSpline
        (ATTR width_;
              height_;
	      transform;
              points;
              linestyle_)
        = GRAPHICS;

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
	      stroke_width;
              linestyle_)
        = AGGREGATE
	      desc; title; metadata;
	      GRAPHICS;
	      END;
	
   circle
        (ATTR transform;
	      cx = text;
	      cy = text;
              r = text;
              linestyle_)
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
              linestyle_)
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
              linestyle_;
              arrowhead)
        = AGGREGATE
	      desc; title; metadata;
	      GRAPHICS;
	      END;
	
   polyline
        (ATTR width_;
              height_;
	      transform;
              points;
              arrowhead;
              linestyle_)
        = AGGREGATE
	      desc; title; metadata;
	      GRAPHICS;
	      END;
	
   polygon
        (ATTR width_;
              height_;
	      transform;
              points;
              linestyle_)
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
	      dx = TEXT;
	      dy = TEXT)
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
   { use }
   
   image
	(ATTR xlink_href = Text;
	      transform;
	      x; 
	      y;
              width_;
              height_)
        = AGGREGATE
              desc; title; metadata;
              PICTURE;
	      END;

   { symbol }
   { marker }
   
   desc = TEXT;
	
   title = TEXT;
	
   { clipPath }
   { mask }
   { linearGradient }
   { radialGradient }
   { stop }
   { pattern }
   { a }
   { view }
   { animate }
   { set }
   { animateMotion }
   { animateColor }
   { animateTransform }
   { script }
   { style }
   { color-profile }
   { color-profile-src }
   { font-face }
   { font-face-src }
   { font-face-uri }
   { font-face-format }
   { font-face-name }
   { definition-src }
   { cursor }
   { switch }

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
	      GraphML; g; Spline; ClosedSpline; rect; circle; ellipse; line_;
	      polyline; polygon; text_; image; desc; title;
	      foreignObject; metadata; XLink;
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
   
   XMLcomment = LIST OF (XMLcomment_line = TEXT);

EXCEPT

   GraphML:             IsDraw, MoveResize, NoMove;
   g:                   NoMove, NoResize, NoCreate;
   Spline:              MoveResize, HighlightChildren, NoShowBox, NoCreate;
   ClosedSpline:        MoveResize, HighlightChildren, NoShowBox, NoCreate;
   rect:                MoveResize, HighlightChildren, NoShowBox, NoCreate;
   circle:              MoveResize, HighlightChildren, NoShowBox, NoCreate;
   ellipse:             MoveResize, HighlightChildren, NoShowBox, NoCreate;
   line_:               MoveResize, HighlightChildren, NoShowBox, NoCreate;
   polyline:            MoveResize, HighlightChildren, NoShowBox, NoCreate;
   polygon:             MoveResize, HighlightChildren, NoShowBox, NoCreate;
   image:	        MoveResize, HighlightChildren, NoShowBox, NoCreate;
   text_:               MoveResize, NoResize, ReturnCreateWithin, NoCreate;
   foreignObject:       MoveResize, NoCreate;
   PICTURE:             NoMove, NoResize;
   TEXT:                NoMove, NoResize;

   id:                  CssId;
   class:               CssClass;
   Invalid_attribute:   Invisible;
   Ghost_restruct:      Invisible;
   Namespace:	        Invisible;
   IntEmptyShape:       Invisible;

END



