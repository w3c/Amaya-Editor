	{ Translation Schema for Template.

	  V. Quint		June 2000 }


TRANSLATION Template;

BUFFERS
   AttrPrefixBuffer (variable);

VAR
   SAN : ' ' AttrPrefixBuffer;  { Start of attribute name }

RULES

   Template: begin
        end;

ATTRIBUTES

      Unknown_attribute:
	begin
        if Template begin
           remove;
           end;
        if NOT Template begin
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

