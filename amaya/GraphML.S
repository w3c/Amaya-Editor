
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
                      linewidth = integer;
                      linestyle_ = solid_, dashed_, dotted_;
                      arrowhead = none_, start, end_, both;
                      align = left_, center_, right_;
                      valign = top_, middle, bottom_;
                      depth_ = integer;
                                IntLeftDistance = integer;
                                IntRightDistance = integer;
                                IntUpDistance = integer;
                                IntDownDistance = integer)
                = GRAPHICS;
            Rectangle
                (ATTR position = text;               { OBSOLETE }
                                IntPosX; IntPosY;    { OBSOLETE }
                      x = text;                      { NEW }
                      y = text;                      { NEW }
                      width_;
                                IntWidth;            { OBSOLETE }
                      height_;
                                IntHeight;           { OBSOLETE }
	              fill = text;                   { NEW }
	              stroke = text;                 { NEW }
	              stroke_width = text;           { NEW }
                      linewidth;                     { OBSOLETE }
                      linestyle_;
                                IntEmptyShape = yes_, no_;
                      align;
                      valign;
                      depth_;
                                IntLeftDistance;
                                IntRightDistance;
                                IntUpDistance;
                                IntDownDistance)
                = BEGIN
                  Label = HTML;
                  GRAPHICS;
                  END;
            RoundRect
                (ATTR position;                      { OBSOLETE }
                                IntPosX; IntPosY;    { OBSOLETE }
	              x; y;                          { NEW }
                      width_;
                                IntWidth;
                      height_;
                                IntHeight;
                      linewidth;
                      linestyle_;
                                IntEmptyShape;
                      align;
                      valign;
                      depth_;
                                IntLeftDistance;
                                IntRightDistance;
                                IntUpDistance;
                                IntDownDistance)
                = BEGIN
                  Label;
                  GRAPHICS;
                  END;
            Circle
                (ATTR position;                      { OBSOLETE }
                                IntPosX; IntPosY;    { OBSOLETE }
                      width_;
                                IntWidth;
                      linewidth;
                      linestyle_;
                                IntEmptyShape;
                      align;
                      valign;
                      depth_;
                                IntLeftDistance;
                                IntRightDistance;
                                IntUpDistance;
                                IntDownDistance)
                = BEGIN
                  Label;
                  GRAPHICS;
                  END;
            Oval
                (ATTR position;                      { OBSOLETE }
                                IntPosX; IntPosY;    { OBSOLETE }
                      width_;
                                IntWidth;
                      height_;
                                IntHeight;
                      linewidth;
                      linestyle_;
                                IntEmptyShape;
                      align;
                      valign;
                      depth_;
                                IntLeftDistance;
                                IntRightDistance;
                                IntUpDistance;
                                IntDownDistance)
                = BEGIN
                  Label;
                  GRAPHICS;
                  END;
            Polyline
                (ATTR position;
                                IntPosX; IntPosY;
                      width_;
                                IntWidth;
                      height_;
                                IntHeight;
                      points;
                      arrowhead;
                      linewidth;
                      linestyle_;
                      align;
                      valign;
                      depth_;
                                IntLeftDistance;
                                IntRightDistance;
                                IntUpDistance;
                                IntDownDistance)
                = GRAPHICS;
            Polygon
                (ATTR position;
                                IntPosX; IntPosY;
                      width_;
                                IntWidth;
                      height_;
                                IntHeight;
                      points;
                      linewidth;
                      linestyle_;
                                IntEmptyShape;
                      align;
                      valign;
                      depth_;
                                IntLeftDistance;
                                IntRightDistance;
                                IntUpDistance;
                                IntDownDistance)
                = BEGIN
                  Label;
                  GRAPHICS;
                  END;
            Spline
                (ATTR position;
                                IntPosX; IntPosY;
                      width_;
                                IntWidth;
                      height_;
                                IntHeight;
                      points;
                      arrowhead;
                      linewidth;
                      linestyle_;
                      align;
                      valign;
                      depth_;
                                IntLeftDistance;
                                IntRightDistance;
                                IntUpDistance;
                                IntDownDistance)
                = GRAPHICS;
            ClosedSpline
                (ATTR position;
                                IntPosX; IntPosY;
                      width_;
                                IntWidth;
                      height_;
                                IntHeight;
                      points;
                      linewidth;
                      linestyle_;
                                IntEmptyShape;
                      align;
                      valign;
                      depth_;
                                IntLeftDistance;
                                IntRightDistance;
                                IntUpDistance;
                                IntDownDistance)
                = BEGIN
                  Label;
                  GRAPHICS;
                  END;
            Text_
                (ATTR position;                      { OBSOLETE }
                                IntPosX; IntPosY;    { OBSOLETE }
	              x; y;                          { NEW }
                      width_;
                                IntWidth;
                      align;
                      valign;
                      depth_;
                                IntLeftDistance;
                                IntRightDistance;
                                IntUpDistance;
                                IntDownDistance)
                = HTML;

	    foreignObject			{ NEW }
	        (ATTR x; 
	              y;
                      width_;
                      height_; 
	              fill;
	              stroke)
		= BEGIN CASE OF HTML; MathML; END; END;
	              
            Group
                (ATTR position;                      { OBSOLETE }
                                IntPosX; IntPosY;    { OBSOLETE }
                      direction = left_, right_, up, down;
                      hspace = text;
                      vspace = text;
                      linewidth;
                      linestyle_;
                      align;
                      valign;
                      depth_;
                                IntLeftDistance;
                                IntRightDistance;
                                IntUpDistance;
                                IntDownDistance;
                                ShowGroup = Yes_)
                = LIST OF (GraphicsElement);
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
   Text_:               MoveResize;
   foreignObject:       MoveResize;
   Group:               MoveResize;
   PICTURE:             MoveResize;
   TEXT:                NoMove, NoResize;

   id:                  CssId;
   class:               CssClass;
   Invalid_attribute:   Invisible;
   Ghost_restruct:      Invisible;
   IntPosX:             Invisible, NewHPos; 
   IntPosY:             Invisible, NewVPos;
   IntWidth:            NewWidth, Invisible;
   IntHeight:           NewHeight, Invisible;
   IntEmptyShape:       Invisible;
   IntLeftDistance:     Invisible;
   IntRightDistance:    Invisible;
   IntUpDistance:       Invisible;
   IntDownDistance:     Invisible;

END
