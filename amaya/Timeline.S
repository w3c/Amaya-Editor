
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


   abstract_group
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


   image_toolbar
	(ATTR xlink_href;
	      x; 
	      y;)
        = AGGREGATE
              PICTURE;
              Timeline_Image;
	      END;

	 image_color
	(ATTR xlink_href;
	      x; 
	      y;)
        = AGGREGATE
              PICTURE;
              Timeline_Image;
	      END;

	 image_motion
	(ATTR xlink_href;
	      x; 
	      y;)
        = AGGREGATE
              PICTURE;
              Timeline_Image;
	      END;

	image_interp
	(ATTR xlink_href;
	      x; 
	      y;)
        = AGGREGATE
              PICTURE;
              Timeline_Image;
	      END;


	image_help
	(ATTR xlink_href;
	      x; 
	      y;)
        = AGGREGATE
              PICTURE;
              Timeline_Image;
	      END;

   exp_period
        (ATTR fill; stroke;
	      x;
          y;
          width_;
          height_)
        = BEGIN
		  GRAPHICS;
		  END;
	      
	col_period
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

	rect_id
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

   text_id
        (ATTR fill; stroke; 
	      font_family; font_size;)
         = LIST OF (TEXT);

   timing_text
        (ATTR fill; stroke; 
	      font_family; font_size;
	      x;
	      y)
         = LIST OF (TEXT);


   GraphicsElement
	= CASE OF
         exp_period; col_period; rect_interface; rect_id; text_; text_id; timing_text; tline; image_slider; 
   	     image_collapse; image_help; image_motion; image_color; image_toolbar; image_interp; Timeline; abstract_group;
		  END;
	     

EXCEPT

   Document:			NoSelect;
   Timeline:            IsDraw, MoveResize, NoMove, NoCut, NoSelect;
   abstract_group:      NoMove, NoResize, HighlightChildren, NoShowBox, NoCreate, NoSelect, NoCut;
   col_period:          MoveResize, NoShowBox, NoCreate, NoVMove, NoResize;
   exp_period:          MoveResize, NoShowBox, NoCreate, NoVMove, NoVresize;
   rect_interface:      NoMove, NoResize, NoShowBox, NoCreate, NoCut;
   rect_id:				NoMove, NoResize, NoShowBox, NoCreate, NoCut;
   tline:               NoMove, NoResize, NoShowBox, NoCreate, NoCut;
   text_:               NoMove, NoResize, NoShowBox, ReturnCreateWithin, NoCreate, NoCut, NoSelect;
   timing_text:         NoMove, NoResize, ReturnCreateWithin, NoCreate, NoCut;
   text_id:             NoMove, NoResize, ReturnCreateWithin, NoCreate, NoCut;
   image_slider:  	    MoveResize,  NoResize, NoShowBox, NoVMove, NoCut, NoSelect;
   image_collapse:      NoMove, NoResize, NoShowBox, NoCut;
   image_color:         NoMove, NoResize, NoShowBox, NoCut;
   image_motion:        NoMove, NoResize, NoShowBox, NoCut; 
   image_toolbar:       NoMove, NoResize, NoShowBox, NoCut; 
   image_help:        NoMove, NoResize, NoShowBox, NoCut; 
   image_interp:        NoMove, NoResize, NoShowBox, NoCut; 
   Timeline_Image:      Hidden, SelectParent, NoCut;
   GRAPHICS:	        SelectParent, NoCut;
   PICTURE:             NoMove, NoResize, SelectParent, NoCut;
   TEXT:                NoMove, NoResize, NoCut;

   id:                  CssId;
   Unknown_attribute:   Invisible;
   Highlight:	        Invisible; 
   Ghost_restruct:      Invisible;
   Namespace:	        Invisible;
   IntEmptyShape:       Invisible;
 

END
