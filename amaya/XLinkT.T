	{ Translation Schema for XLink.

	  V. Quint		June 2000 }


TRANSLATION XLink;

RULES

   XLink:
	begin
	end;

ATTRIBUTES

   type:
        create ' xmlns:xlink="http://www.w3.org/1999/xlink"';
   type = simple:
	create ' xlink:type="simple"';
   type = extended:
	create ' xlink:type="extended"';
   type = locator:
	create ' xlink:type="locator"';
   type = arc:
	create ' xlink:type="arc"';
   type = resource:
	create ' xlink:type="resource"';
   type = title_:
	create ' xlink:type="title"';
   type = none_:
	create ' xlink:type="none"';

   href_:
	begin
	create ' xlink:href="';
	create href_;
	create '"';
	end;

{****
   role = TEXT;

   title = TEXT;

   show = new, replace, embed, undefined;

   actuate = onLoad, onRequest, undefined;

   from = TEXT;

   to = TEXT;
****}

END
