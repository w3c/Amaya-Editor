	{ Translation Schema for XLink.

	  V. Quint		June 2000 }


TRANSLATION XLink;

CONST
   nsdcl = ' xmlns:xlink="http://www.w3.org/1999/xlink"';

ATTRIBUTES

   type = simple:
	begin
        create nsdcl;
	create ' xlink:type="simple"';
	end;
   type = extended:
	begin
        create nsdcl;
	create ' xlink:type="extended"';
	end;
   type = locator:
	begin
        create nsdcl;
	create ' xlink:type="locator"';
	end;
   type = arc:
	begin
        create nsdcl;
	create ' xlink:type="arc"';
	end;
   type = resource:
	begin
        create nsdcl;
	create ' xlink:type="resource"';
	end;
   type = title_:
	begin
        create nsdcl;
	create ' xlink:type="title"';
	end;
   type = none_:
	begin
        create nsdcl;
	create ' xlink:type="none"';
	end;

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
