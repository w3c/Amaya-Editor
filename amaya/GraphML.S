
      { Thot structure schema for Scalable Vector Graphics }

STRUCTURE GraphML;

DEFPRES GraphMLP;

ATTR

   { global attributes for all GraphML elements }
   id = text;
   class = text;
   style_ = text;

   { global attributes for internal processing }
   Invalid_attribute = TEXT;
   Ghost_restruct = TEXT;
   Namespace = TEXT;        { for children of element foreignObject }
   IntEmptyShape = yes_, no_;

STRUCT

   GraphML
	(ATTR !height_ = Text;
	      !width_ = Text)
        = LIST OF (GraphicsElement) + (XMLcomment)
          with height_ ?= '6cm', width_ ?= '10cm';

   g
        (ATTR fill = text;
	      stroke = text;
	      stroke_width = text;
	      transform = TEXT;
              linestyle_ = solid_, dashed_, dotted_;
              depth_ = integer;
              ShowGroup = Yes_)
        = LIST OF (GraphicsElement);

   { defs }
   { path }
	
   Spline
        (ATTR width_;
              height_;
	      transform;
              points = text;
              arrowhead = none_, start, end_, both;
              linestyle_;
              depth_)
        = GRAPHICS;
	
   ClosedSpline
        (ATTR width_;
              height_;
	      transform;
              points;
              linestyle_;
              depth_)
        = GRAPHICS;

   rect
        (ATTR transform;
	      x = text;
              y = text;
              width_;
              height_;
	      rx = text;
	      ry = text;
	      fill;
              stroke;
	      stroke_width;
              linestyle_;
              depth_)
        = AGGREGATE
	      desc; title; metadata;
	      GRAPHICS;
	      END;
	
   circle
        (ATTR transform;
	      cx = text;
	      cy = text;
              r = text;
              linestyle_;
              depth_)
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
              linestyle_;
              depth_)
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
              arrowhead;
              depth_)
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
              linestyle_;
              depth_)
        = AGGREGATE
	      desc; title; metadata;
	      GRAPHICS;
	      END;
	
   polygon
        (ATTR width_;
              height_;
	      transform;
              points;
              linestyle_;
              depth_)
        = AGGREGATE
	      desc; title; metadata;
	      GRAPHICS;
	      END;
	
   text_
        (ATTR transform;
	      x;
	      y;
              width_;
              depth_)
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
	      END; END;
	              
   metadata = TEXT;

   { filter }
   { ... }

   GraphicsElement
	= CASE OF
	      desc; title; metadata; g; Spline; ClosedSpline; rect; circle;
	      ellipse; line_; polyline; polygon; text_; foreignObject; image;
	      END;
	
   TextComponent
	= CASE OF
	      TEXT; 
	      desc;
	      title;
	      metadata;
	      tspan;
	      END;

   SpanElement
	= CASE OF
	      TEXT;
	      desc;
	      title;
	      metadata;
	      tspan; 
	      END;
   
   XMLcomment = LIST OF (XMLcomment_line = TEXT);

EXCEPT

   GraphML:             IsDraw;
   g:                   MoveResize;
   Spline:              MoveResize, HighlightChildren, NoShowBox;
   ClosedSpline:        MoveResize, HighlightChildren, NoShowBox;
   rect:                MoveResize, HighlightChildren, NoShowBox;
   circle:              MoveResize, HighlightChildren, NoShowBox;
   ellipse:             MoveResize, HighlightChildren, NoShowBox;
   line_:               MoveResize, HighlightChildren, NoShowBox;
   polyline:            MoveResize, HighlightChildren, NoShowBox;
   polygon:             MoveResize, HighlightChildren, NoShowBox;
   text_:               MoveResize;
   foreignObject:       MoveResize;
   PICTURE:             MoveResize;
   TEXT:                NoMove, NoResize;

   id:                  CssId;
   class:               CssClass;
   Invalid_attribute:   Invisible;
   Ghost_restruct:      Invisible;
   Namespace:	        Invisible;
   IntEmptyShape:       Invisible;

END
