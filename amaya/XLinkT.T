	{ Translation Schema for XLink.

	  V. Quint		June 2000 }


TRANSLATION XLink;

CONST
   nsdcl = ' xmlns:xlink="http://www.w3.org/1999/xlink"';

RULES

   XLink: begin
        remove;
        end;

ATTRIBUTES

   type = simple:
	begin
        if XLink begin
	   remove;
	   end;
	if NOT XLink
	   create ' xlink:type="simple"';
        end;

   type = extended:
	begin
        if XLink begin
           remove;
           end;
        if NOT XLink
	   create ' xlink:type="extended"';
	end;

   type = locator:
	begin
        if XLink begin
           remove;
           end;
        if NOT XLink
	   create ' xlink:type="locator"';
	end;

   type = arc:
	begin
        if XLink begin
           remove;
           end;
        if NOT XLink
	   create ' xlink:type="arc"';
	end;

   type = resource:
	begin
        if XLink begin
           remove;
           end;
        if NOT XLink
	   create ' xlink:type="resource"';
	end;

   type = title_:
	begin
        if XLink begin
           remove;
           end;
        if NOT XLink
	   create ' xlink:type="title"';
	end;

   type = none_:
	begin
        if XLink begin
           remove;
           end;
        if NOT XLink
	   create ' xlink:type="none"';
	end;

   href_:
	begin
        if XLink begin
           remove;
           end;
        if NOT XLink begin
           create nsdcl;
	   create ' xlink:href="';
	   create href_;
	   create '"';
	   end;
	end;

      AnnotIsHidden:
        begin
	  remove;
	end;

      AnnotIcon1:
        begin
	  remove;
	end;

      AnnotOrphIcon:
        begin
	  remove;
	end;

      id:
	begin
 	  remove;
	end;

      Unknown_attribute:
	begin
        if XLink begin
           remove;
           end;
        if NOT XLink begin
	   create ' xlink:';
	   create Unknown_attribute;
	   end;
	end;

{****
   role = TEXT;

   arcrole = TEXT;

   title = TEXT;

   show = new, replace, embed, other, none_;

   actuate = onLoad, onRequest, other, none_;

   from = TEXT;

   to = TEXT;
****}

END

