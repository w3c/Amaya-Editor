		{ Author: J. Kahan 	 2003  }

STRUCTURE Topics;

DEFPRES TopicsP;



STRUCT

  { A bookmark collection is a set of bookmarks and topics. }

  { ======= TOPICS ====== }

  Topics
     (ATTR  XmlBody = Yes_; Charset = Text) = 
	 LIST OF (Item);

  { a topic item is made of a title, a reference to the model, and an optional list
    of Items }
  
  Topic_item (ATTR Model_HREF_ = Text; Closed_ = Yes; Open_ = Yes_;  Selected_ = Yes_;) = 
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
	Topics:		NoResize, NoMove, NoResize, NoCut;
 
	{ topics }
	Topic_title:	NoCut, NoSelect, Hidden;
	Topic_content:	NoSelect, Hidden;

        { Bookmarks }
	Bookmark_item:  Nocut;
	Bookmark_title: NoCut, NoSelect, Hidden;
	Selected_:	Invisible;
END
