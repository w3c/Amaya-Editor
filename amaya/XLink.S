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
	
   { generic attributes for internal use }
   Invalid_attribute = TEXT;	{ to store an unknown attribute }
   Ghost_restruct = TEXT;	{ to help structure transformation }

END