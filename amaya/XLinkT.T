	{ Translation Schema for XLink.

	  V. Quint		June 2000 }


TRANSLATION XLink;

CONST
   nsdcl = ' xmlns:xlink="http://www.w3.org/1999/xlink"';

ATTRIBUTES

   type = simple:
	begin
        if XLink begin
	   remove;
	   end;
	if NOT XLink begin
           create nsdcl;
	   create ' xlink:type="simple"';
	   end;
        end;

   type = extended:
	begin
        if XLink begin
           remove;
           end;
        if NOT XLink begin
           create nsdcl;
	   create ' xlink:type="extended"';
           end;
	end;

   type = locator:
	begin
        if XLink begin
           remove;
           end;
        if NOT XLink begin
           create nsdcl;
	   create ' xlink:type="locator"';
           end;
	end;

   type = arc:
	begin
        if XLink begin
           remove;
           end;
        if NOT XLink begin
           create nsdcl;
	   create ' xlink:type="arc"';
           end;
	end;

   type = resource:
	begin
        if XLink begin
           remove;
           end;
        if NOT XLink begin
           create nsdcl;
	   create ' xlink:type="resource"';
           end;
	end;

   type = title_:
	begin
        if XLink begin
           remove;
           end;
        if NOT XLink begin
           create nsdcl;
	   create ' xlink:type="title"';
           end;
	end;

   type = none_:
	begin
        if XLink begin
           remove;
           end;
        if NOT XLink begin
           create nsdcl;
	   create ' xlink:type="none"';
	   end;
	end;

   href_:
	begin
        if XLink begin
           remove;
           end;
        if NOT XLink begin
	   create ' xlink:href="';
	   create href_;
	   create '"';
	   end;
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

