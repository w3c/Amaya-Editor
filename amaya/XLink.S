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

STRUCT

   {  The next definition is just to make the str compiler happy.
      No XLink element is supposed to be ever created }
   XLink = TEXT;

END