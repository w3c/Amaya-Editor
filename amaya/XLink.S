		{ V. Quint	June 2000 }

STRUCTURE XLink;

DEFPRES XLinkP;

ATTR

   type = simple, extended, locator, arc, resource, title_, none_;

   href_ = TEXT;

   role = TEXT;

   title = TEXT;

   show = new, replace, embed, undefined;

   actuate = onLoad, onRequest, undefined;

   from = TEXT;

   to = TEXT;

   { annotations }
   id = TEXT; { To store the reverse link, from an annotation to a source doc }

   { generic attributes for internal use }
   Invalid_attribute = TEXT;	{ to store an unknown attribute }
   Ghost_restruct = TEXT;	{ to help structure transformation }

CONST

   C_Empty = ' ';

STRUCT
 
  XLink
     (ATTR AnnotIsHidden = Yes_;
           { AnnotIsHidden : do not show this annotation }
           AnnotIcon1 = Yes_;
           AnnotIcon2 = Yes_;
           { the icons we can associate with an annotation }) =
	CONSTANT C_Empty;

EXCEPT

    XLink:          Hidden;
    AnnotIsHidden:  Invisible;
    AnnotIcon1:     Invisible;
    AnnotIcon2:     Invisible;

END
