
      { Thot structure schema for Timeline documents
	  
		Pierre Geneves	June 2002}

STRUCTURE Timeline;

DEFPRES TimelineP;

ATTR

   { global attributes for all elements }
   id = text;
   	
   { global attributes for internal processing }
   Unknown_attribute = text;
   Ghost_restruct = text;
   Highlight = Yes_;        { to show the element corresponding to the current selection in the source view }
   Namespace = text;
   IntEmptyShape = yes_, no_;

STRUCT

{ Document Structure }

   Timeline
   (ATTR 
    fill = text;
	stroke = text;
    font_family = text;
	font_size = text;
    x = text;
    y = text;
	width_ = text;
	height_ = text)
     = LIST OF (GraphicsElement);


   groupe_abstrait 
     (ATTR transform = text) 
        = LIST OF (GraphicsElement);

	
{ Images }

 image_slider
	(ATTR xlink_href = text;
	      x; 
	      y)
        = AGGREGATE
              PICTURE;
              Timeline_Image = Timeline;
	      END;

  image_collapse
	(ATTR xlink_href;
	      x; 
	      y;)
        = AGGREGATE
              PICTURE;
              Timeline_Image;
	      END;


   image_keypos
	(ATTR xlink_href;
	      x; 
	      y;)
        = AGGREGATE
              PICTURE;
              Timeline_Image;
	      END;


	
{ Basic Shapes }

   rect_period
        (ATTR fill; stroke;
	      x;
          y;
          width_;
          height_)
        = BEGIN
		  GRAPHICS;
		  END;
	      
	   

   rect_interface
        (ATTR fill; stroke; 
	      x;
          y;
          width_;
          height_)
        = BEGIN
		  GRAPHICS;
		  END;
	      
	   
	

   tline
        (ATTR fill; stroke; 
	      x1 = text;
	      y1 = text;
          x2 = text;
	      y2 = text)
        = BEGIN
		  GRAPHICS;
		  END;
	      
	      
	
{ Text }
	
   text_
        (ATTR fill; stroke; 
	      font_family; font_size;
	      x;
	      y)
         = LIST OF (TEXT);


   GraphicsElement
	= CASE OF
         rect_period; rect_interface; text_; tline; image_slider; 
   	     image_collapse; image_keypos; Timeline; groupe_abstrait;
		  END;
	     

EXCEPT

   Timeline:                 IsDraw, MoveResize, NoMove;
   groupe_abstrait:     NoMove, NoResize, HighlightChildren, NoShowBox, NoCreate;
   rect_period:         MoveResize, HighlightChildren, NoShowBox, NoCreate, NoVMove, NoVresize;
   rect_interface:      NoMove,NoResize, HighlightChildren, NoShowBox, NoCreate;
   tline:               NoMove, NoResize, HighlightChildren, NoShowBox, NoCreate;
   text_:               NoMove, NoResize, NoShowBox, ReturnCreateWithin,
	                NoCreate;
   image_slider:	MoveResize, HighlightChildren, NoShowBox, NoVMove, NoResize;
   image_collapse:  NoMove, NoResize, NoShowBox;
   image_keypos:    NoMove, NoResize, NoShowBox; 
   Timeline_Image:           Hidden, SelectParent;
   GRAPHICS:	        SelectParent;
   PICTURE:             NoMove, NoResize, SelectParent;
   TEXT:                NoMove, NoResize;

   id:                  CssId;
   Unknown_attribute:   Invisible;
   Highlight:	        Invisible; 
   Ghost_restruct:      Invisible;
   Namespace:	        Invisible;
   IntEmptyShape:       Invisible;
 

END
