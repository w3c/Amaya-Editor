
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

   timing_text
        (ATTR fill; stroke; 
	      font_family; font_size;
	      x;
	      y)
         = LIST OF (TEXT);


   GraphicsElement
	= CASE OF
         rect_period; rect_interface; text_; timing_text; tline; image_slider; 
   	     image_collapse; image_keypos; Timeline; groupe_abstrait;
		  END;
	     

EXCEPT

   Document:			NoSelect;
   Timeline:            IsDraw, MoveResize, NoMove, NoCut, NoSelect;
   groupe_abstrait:     NoMove, NoResize, HighlightChildren, NoShowBox, NoCreate, NoSelect, NoCut;
   rect_period:         MoveResize, NoShowBox, NoCreate, NoVMove, NoVresize;
   rect_interface:      NoMove, NoResize, NoShowBox, NoCreate, NoCut;
   tline:               NoMove, NoResize, NoShowBox, NoCreate, NoCut;
   text_:               NoMove, NoResize, NoShowBox, ReturnCreateWithin, NoCreate, NoCut, NoSelect;
   timing_text:         NoMove, NoResize, ReturnCreateWithin, NoCreate, NoCut;
   image_slider:  	    MoveResize,  NoResize, NoShowBox, NoVMove, NoCut, NoSelect;
   image_collapse:      NoMove, NoResize, NoShowBox, NoCut;
   image_keypos:        NoMove, NoResize, NoShowBox, NoCut; 
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
