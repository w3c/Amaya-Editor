
      { Thot structure schema for simple 2D graphics }

STRUCTURE GraphML;

DEFPRES GraphMLP;

ATTR

   { global attributes for all GraphML elements }
   id = text;
   class = text;
   style\240 = text;

   { xml:link }
   link = simple, extended, locator, group, document;

   { generic attributes for Amaya and XML }
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
		      align = left_, center_, right_;
		      valign = top_, middle, bottom_;
		      depth_ = integer;
				IntLeftDistance = integer;
				IntRightDistance = integer;
				IntUpDistance = integer;
				IntDownDistance = integer)
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
		(ATTR position;
				IntPosX; IntPosY;
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
                (ATTR position;
				IntPosX; IntPosY;
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
		(ATTR position;
				IntPosX; IntPosY;
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
                (ATTR position;
				IntPosX; IntPosY;
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
            Math
                (ATTR position;
				IntPosX; IntPosY;
		      align;
		      valign;
		      depth_;
				IntLeftDistance;
				IntRightDistance;
				IntUpDistance;
				IntDownDistance)
                = MathML;
            Group
                (ATTR position;
				IntPosX; IntPosY;
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
                = LIST OF (GraphicalElement);
            END;

EXCEPT

   GraphML:		NoMove, NoResize, Hidden;
   Label:		NoMove, NoResize, NoShowBox;
   Line_:		MoveResize, HighlightChildren, NoShowBox;
   Rectangle:		MoveResize, HighlightChildren, NoShowBox;
   RoundRect:		MoveResize, HighlightChildren, NoShowBox;
   Circle:		MoveResize, HighlightChildren, NoShowBox;
   Oval:		MoveResize, HighlightChildren, NoShowBox;
   Polyline:		MoveResize, HighlightChildren, NoShowBox;
   Polygon:		MoveResize, HighlightChildren, NoShowBox;
   Spline:		MoveResize, HighlightChildren, NoShowBox;
   ClosedSpline:	MoveResize, HighlightChildren, NoShowBox;
   Text_:		MoveResize;
   Math:		MoveResize;
   Group:               MoveResize;
   PICTURE:		MoveResize;
   TEXT:		NoMove, NoResize;

   id:			CssId;
   class:		CssClass;
   Invalid_attribute:	Invisible;
   Ghost_restruct:	Invisible;
   IntPosX:		Invisible, NewHPos; 
   IntPosY:		Invisible, NewVPos;
   IntWidth:		NewWidth, Invisible;
   IntHeight:		NewHeight, Invisible;
   IntEmptyShape:	Invisible;
   IntLeftDistance:	Invisible;
   IntRightDistance:	Invisible;
   IntUpDistance:	Invisible;
   IntDownDistance:	Invisible;

END
