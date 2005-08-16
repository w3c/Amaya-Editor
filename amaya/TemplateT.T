	{ Translation Schema for Template.

	  V. Quint		June 2000 }


TRANSLATION Template;

LINELENGTH 78;

BUFFERS
   ElemPrefixBuffer (variable);
   AttrPrefixBuffer (variable);

CONST
   DoubleQuote = '"';
   CloseTagNL = '>\12';
   CloseEmptyTagNL = '/>\12';
   tagend = '>\12';   

VAR
   SOT : '<' ElemPrefixBuffer;	{ Start of opening tag }
   SCT : '</' ElemPrefixBuffer;	{ Start of closing tag }
   SAN : ' ' AttrPrefixBuffer;  { Start of attribute name }
   SUA : '';                    { Start of unknown attribute }

RULES
   Template: begin
  	Use HTMLTX for HTML;
    Use XLinkT for XLink;
  	Use SVGT for SVG;
    Use MathMLT for MathML;
   end;

  free_struct:
    begin
    create SOT;
    create 'free_struct';
    create Attributes;
    create tagend;
    indent +2;
    indent -2 after;
    create SCT after;
    create 'free_struct>\12' after;
  end;

  free_content:
    begin
    create SOT;
    create 'free_struct';
    create Attributes;
    create tagend;
    indent +2;
    indent -2 after;
    create SCT after;
    create 'free_struct>\12' after;
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

