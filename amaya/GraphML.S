
      { Thot structure schema for simple 2D graphics }

STRUCTURE GraphML;

DEFPRES GraphMLP;

ATTR
   lid = text;
   align = left_, center_, right_;
   valign = top_, middle, bottom_;
   Invalid_attribute = TEXT;
   Ghost_restruct = TEXT;

STRUCT

   GraphML = LIST OF (GraphicalElement) + (XMLcomment);

   XMLcomment = LIST OF (XMLcomment_line = TEXT);

   GraphicalElement = CASE OF
            Line_
                (ATTR points = text;
				IntPosX = integer; IntPosY = integer;
				IntWidth = integer; IntHeight = integer;
		      linewidth = integer;
		      linestyle_ = solid_, dashed_, dotted_;
                      arrowhead = none, start, end_, both;
		      depth_ = integer)
                = GRAPHICS;
            Rectangle
                (ATTR position = text;
				IntPosX; IntPosY;
                      width_ = text;
				IntWidth;
                      height_ = text;
				IntHeight;
		      linewidth;
		      linestyle_;
				IntEmptyShape = yes_, no_;
		      depth_)
                = BEGIN
                  Label = HTML;
                  GRAPHICS;
                  END;
	    RoundRect
		(ATTR position;
				IntPosX; IntPosY;
		      width_;
				IntWidth;
		      height_;
				IntHeight;
		      linewidth;
		      linestyle_;
				IntEmptyShape;
		      depth_)
		= BEGIN
		  Label;
		  GRAPHICS;
		  END;
            Circle
                (ATTR position;
				IntPosX; IntPosY;
		      width_;
				IntWidth;
		      linewidth;
		      linestyle_;
				IntEmptyShape;
		      depth_)
                = BEGIN
                  Label;
		  GRAPHICS;
                  END;
            Oval
		(ATTR position;
				IntPosX; IntPosY;
		      width_;
				IntWidth;
		      height_;
				IntHeight;
		      linewidth;
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
				IntWidth;
		      height_;
				IntHeight;
		      points;
		      arrowhead;
		      linewidth;
		      linestyle_;
		      depth_)
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
		      depth_)
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
		      depth_)
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
		      depth_)
                = BEGIN
                  Label;
		  GRAPHICS;
                  END;
            Text_
                (ATTR position;
				IntPosX; IntPosY;
		      width_;
				IntWidth;
		      depth_)
                = HTML;
            Math
                (ATTR position;
				IntPosX; IntPosY;
		      depth_)
                = MathML;
            Group
                (ATTR position;
				IntPosX; IntPosY;
                      direction = left_, right_, up, down;
                      hspace = text;
                      vspace = text;
				IntLeftDistance = integer;
				IntRightDistance = integer;
				IntUpDistance = integer;
				IntDownDistance = integer;
		      linewidth;
		      linestyle_;
		      depth_)
                = LIST OF (GraphicalElement);
            END;

EXCEPT

   GraphML:		MoveResize, Hidden;
   Label:		NoMove, Noresize;
   Line_:		HighlightChildren;
   Rectangle:		HighlightChildren;
   RoundRect:		HighlightChildren;
   Circle:		HighlightChildren;
   Oval:		HighlightChildren;
   Polyline:		HighlightChildren;
   Polygon:		HighlightChildren;
   Spline:		HighlightChildren;
   ClosedSpline:	HighlightChildren;

   Invalid_attribute:	Invisible;
   Ghost_restruct:	Invisible;
   IntPosX:		Invisible, NewHPos; 
   IntPosY:		Invisible, NewVPos;
   IntWidth:		Invisible, NewWidth;
   IntHeight:		Invisible, NewHeight;
   IntEmptyShape:	Invisible;
   IntLeftDistance:	Invisible;
   IntRightDistance:	Invisible;
   IntUpDistance:	Invisible;
   IntDownDistance:	Invisible;

END
