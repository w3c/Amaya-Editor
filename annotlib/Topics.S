		{ Author: J. Kahan @W3C 2003-2004  }

STRUCTURE Topics;

DEFPRES TopicsP;

CONST

   C_Empty = ' ';

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
		? Topic_content = LIST OF (Item) + (SeeAlso_content);
	        END;

  { an Item can be either a Topic_item a Bookmark_item, or a Separator_item }
  Item =        CASE OF
		Topic_item;
		Bookmark_item;
		Separator_item;
	        END;

  { a Bookmark has a pointer to the bookmarked document and a title}
  Bookmark_item (ATTR  Model_HREF_; HREF_ = Text;) =
                BEGIN
		Bookmark_title = Text;
		? SeeAlso_content;
		END;

  { a Separator is an empty element}
  Separator_item (ATTR  Model_HREF_; ) = CONSTANT C_Empty;

  SeeAlso_content = LIST OF (SeeAlso_item);

  { a SeeAlso item points to related information; it has a pointer to the
   related document  and a title}
  SeeAlso_item (ATTR  Model_HREF_; HREF_;) =
                BEGIN
		SeeAlso_title = Text;
		END;

EXCEPT 
	Topics:		NoResize, NoMove, NoResize, NoCut;
 
	{ topics }
	Topic_title:	NoCut, NoSelect, Hidden;
	Topic_content:	NoSelect, Hidden;
	Selected_:	Invisible;

        { Bookmarks }
	Bookmark_item:  Nocut;
	Bookmark_title: NoCut, NoSelect, Hidden;


	{ SeeAlso }
	SeeAlso_item:  Nocut, NoSelect;
	SeeAlso_title: NoCut, NoSelect, Hidden;
	SeeAlso_content: NoSelect, Hidden;

       { Separator }
       C_Empty:	       NoCut, Hidden;
END
