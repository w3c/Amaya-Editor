
      { Thot structure schema for simple 2D graphics }

STRUCTURE GraphML;

DEFPRES GraphMLP;

ATTR

   { global attributes for all GraphML elements }
   id = text;
   class = text;
   style\240 = text;

   { global attributes for Amaya }
   Invalid_attribute = TEXT;
   Ghost_restruct = TEXT;
   Namespace = TEXT;        { for children of element foreignObject }

STRUCT

   GraphML
	    (ATTR !height_ = Text; !width_ = Text)
                {  height           width }
	        = LIST OF (GraphicsElement) + (XMLcomment)
	          with height_ ?= '6cm', width_ ?= '10cm';

   XMLcomment = LIST OF (XMLcomment_line = TEXT);

   GraphicsElement = CASE OF
	    desc = TEXT;                             { NEW }
	    title = TEXT;                            { NEW }
	    metadata = TEXT;                         { NEW }
            Line_
                (ATTR points = text;
                                IntPosX = integer; IntPosY = integer;
                                IntWidth = integer; IntHeight = integer;
                      linestyle_ = solid_, dashed_, dotted_;
                      arrowhead = none_, start, end_, both;
                      depth_ = integer)
                = GRAPHICS;
            Rectangle
                (ATTR position = text;               { OBSOLETE }
                                IntPosX; IntPosY;    { OBSOLETE }
                      x = text;                      { NEW }
                      y = text;                      { NEW }
                      width_;
                      height_;
	              fill = text;                   { NEW }
	              stroke = text;                 { NEW }
	              stroke_width = text;           { NEW }
                      linestyle_;
                                IntEmptyShape = yes_, no_;
                      depth_)
                = BEGIN
                  Label = HTML;
                  GRAPHICS;
                  END;
            RoundRect
                (ATTR position;                      { OBSOLETE }
                                IntPosX; IntPosY;    { OBSOLETE }
	              x; y;                          { NEW }
                      width_;
                      height_;
                      linestyle_;
                                IntEmptyShape;
                      depth_)
                = BEGIN
                  Label;
                  GRAPHICS;
                  END;
            Circle
                (ATTR position;                      { OBSOLETE }
                                IntPosX; IntPosY;    { OBSOLETE }
                      width_;
                      linestyle_;
                                IntEmptyShape;
                      depth_)
                = BEGIN
                  Label;
                  GRAPHICS;
                  END;
            Oval
                (ATTR position;                      { OBSOLETE }
                                IntPosX; IntPosY;    { OBSOLETE }
                      width_;
                      height_;
                      linestyle_;
                                IntEmptyShape;
                      depth_)
                = BEGIN
                  Label;
                  GRAPHICS;
                  END;
            Polyline
                (ATTR position;
                                IntPosX; IntPosY;
                      width_;
                      height_;
                      points;
                      arrowhead;
                      linestyle_;
                      depth_)
                = GRAPHICS;
            Polygon
                (ATTR position;
                                IntPosX; IntPosY;
                      width_;
                      height_;
                      points;
                      linestyle_;
                                IntEmptyShape;
                      depth_)
                = BEGIN
                  Label;
                  GRAPHICS;
                  END;
            Spline
                (ATTR position;
                                IntPosX; IntPosY;
                      width_;
                      height_;
                      points;
                      arrowhead;
                      linestyle_;
                      depth_)
                = GRAPHICS;
            ClosedSpline
                (ATTR position;
                                IntPosX; IntPosY;
                      width_;
                      height_;
                      points;
                      linestyle_;
                                IntEmptyShape;
                      depth_)
                = BEGIN
                  Label;
                  GRAPHICS;
                  END;

            text_
                (ATTR x;
		      y;
                      width_;
                      depth_)
                = LIST OF (TextComponent);

	    foreignObject			{ NEW }
	        (ATTR x; 
	              y;
                      width_;
                      height_; 
	              fill;
	              stroke)
		= BEGIN CASE OF HTML; MathML; END; END;
	              
            g
                (ATTR position;                      { OBSOLETE }
                                IntPosX; IntPosY;    { OBSOLETE }
	              fill;
	              stroke;
	              stroke_width;
                      linestyle_;
                      depth_;
                                ShowGroup = Yes_)
                = LIST OF (GraphicsElement);
            END;
	
   TextComponent = CASE OF
	    TEXT; 
	    desc;
	    title;
	    metadata;
	    tspan
		(ATTR x;
	              y;
		      dx = TEXT;
		      dy = TEXT)
		= LIST OF (SpanElement);
	    END	;

   SpanElement = CASE OF
	    TEXT;
	    desc;
	    title;
	    metadata;
	    tspan; 
	    END;
   
EXCEPT

   GraphML:             IsDraw;
   Label:               NoMove, NoResize, NoShowBox;
   Line_:               MoveResize, HighlightChildren, NoShowBox;
   Rectangle:           MoveResize, HighlightChildren, NoShowBox;
   RoundRect:           MoveResize, HighlightChildren, NoShowBox;
   Circle:              MoveResize, HighlightChildren, NoShowBox;
   Oval:                MoveResize, HighlightChildren, NoShowBox;
   Polyline:            MoveResize, HighlightChildren, NoShowBox;
   Polygon:             MoveResize, HighlightChildren, NoShowBox;
   Spline:              MoveResize, HighlightChildren, NoShowBox;
   ClosedSpline:        MoveResize, HighlightChildren, NoShowBox;
   text_:               MoveResize;
   foreignObject:       MoveResize;
   g:                   MoveResize;
   PICTURE:             MoveResize;
   TEXT:                NoMove, NoResize;

   id:                  CssId;
   class:               CssClass;
   Invalid_attribute:   Invisible;
   Ghost_restruct:      Invisible;
   IntPosX:             Invisible, NewHPos; 
   IntPosY:             Invisible, NewVPos;
   IntEmptyShape:       Invisible;

END
