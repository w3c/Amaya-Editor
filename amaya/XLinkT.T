	{ Translation Schema for XLink.

	  V. Quint		June 2000 }


TRANSLATION XLink;

BUFFERS
   AttrPrefixBuffer (variable);

VAR
   SAN : ' ' AttrPrefixBuffer;  { Start of attribute name }

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
	if NOT XLink begin
           create SAN;
	   create 'type="simple"';
	   end;
        end;

   type = extended:
	begin
        if XLink begin
           remove;
           end;
        if NOT XLink begin
           create SAN;
	   create 'type="extended"';
	   end;
	end;

   type = locator:
	begin
        if XLink begin
           remove;
           end;
        if NOT XLink begin
           create SAN;
	   create 'type="locator"';
	   end;
	end;

   type = arc:
	begin
        if XLink begin
           remove;
           end;
        if NOT XLink begin
           create SAN;
	   create 'type="arc"';
	   end;
	end;

   type = resource:
	begin
        if XLink begin
           remove;
           end;
        if NOT XLink begin
           create SAN;
	   create 'type="resource"';
	   end;
	end;

   type = title_:
	begin
        if XLink begin
           remove;
           end;
        if NOT XLink begin
           create SAN;
	   create 'type="title"';
	   end;
	end;

   type = none_:
	begin
        if XLink begin
           remove;
           end;
        if NOT XLink begin
           create SAN;
	   create 'type="none"';
	   end;
	end;

   href_:
	begin
        if XLink begin
           remove;
           end;
        if NOT XLink begin
           create SAN;
	   create 'href="';
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
           create SAN;
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

