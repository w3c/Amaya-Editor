		{ V. Quint	June 2000 }

STRUCTURE XLink;

DEFPRES XLinkP;

ATTR

   type = simple, extended, locator, arc, resource, title_, none_;

   href_ = TEXT;

   role = TEXT;

   arcrole = TEXT;

   title = TEXT;

   show = new, replace, embed, other, none_;

   actuate = onLoad, onRequest, other, none_;

   from = TEXT;

   to = TEXT;

   { annotations }
   id = TEXT; { To store the reverse link, from an annotation to a source doc }

   { generic attributes for internal use }
   Unknown_attribute = TEXT;	{ to store an unknown attribute }
   Ghost_restruct = TEXT;	{ to help structure transformation }

CONST

   C_Empty = ' ';

STRUCT
 
  XLink
     (ATTR AnnotIsHidden = Yes_;
           { AnnotIsHidden : do not show this annotation }
	   AnnotIcon = TEXT;
           AnnotIcon1 = Yes_;
           AnnotOrphIcon = Yes_;
           { the icons we can associate with an annotation }) =
	BEGIN PICTURE; END;

EXCEPT

    XLink:          Hidden;
    {PICTURE:        SelectParent;}
    AnnotIsHidden:  Invisible;
    AnnotIcon:      Invisible;
    AnnotIcon1:     Invisible;
    AnnotOrphIcon:  Invisible;
    Unknown_attribute: Invisible;

END
