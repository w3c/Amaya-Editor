		{ Author: J. Kahan 	 2003  }

STRUCTURE Topics;

DEFPRES TopicsP;

ATTR
 
  { global attributes for all Topic elements}

STRUCT

  { A bookmark collection is a set of bookmarks and topics. }

  { ======= TOPICS ====== }

  Topics
     (ATTR  XmlBody = Yes_; Charset = Text) = 
          BEGIN
	  Topic_item;
	  END;

  { a topic item is made of a title, a reference to the model, and an optional list
    of Items }
  
  Topic_item (ATTR Model_HREF_ = Text; Closed_ = Yes; Open_ = Yes_;) = 
                BEGIN
	        Topic_title = Text;
		? Topic_content = LIST OF (Item);
	        END;

  { an Item can be either a Topic_item or a Bookmark_item }
  Item =        CASE OF
		Topic_item;
		Bookmark_item;
	        END;

  { a Bookmark has a pointer to the bookmarked document and a title}
  Bookmark_item (ATTR  Model_HREF_; HREF_ = Text;) =
                BEGIN
		Bookmark_title = Text;
		END;

EXCEPT 
        { Bookmarks }
	Topics:		NoMove, NoResize;

	{ topics }
	Topic_item:     NoCut;


END
