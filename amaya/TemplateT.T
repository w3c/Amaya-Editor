	{ Translation Schema for XTiger Templates
	  Francesc Campoy Flores	 31/03/2006 }


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

  head:
    begin
    create SOT;
    create 'head';
    create Attributes;
    create tagend;
    indent +2;
    indent -2 after;
    create SCT after;
    create 'head>\12' after;
  end;

  component:
    begin
    create SOT;
    create 'component';
    create Attributes;
    create tagend;
    indent +2;
    indent -2 after;
    create SCT after;
    create 'component>\12' after;
  end;

  union:
    begin
    create SOT;
    create 'union';
    create Attributes;
    create tagend;
    indent +2;
    indent -2 after;
    create SCT after;
    create 'union>\12' after;
  end;

  import:
    begin
    create SOT;
    create 'import';
    create Attributes;
    create CloseEmptyTagNL;
  end;

  repeat:
    begin
    create SOT;
    create 'repeat';
    create Attributes;
    create tagend;
    indent +2;
    indent -2 after;
    create SCT after;
    create 'repeat>\12' after;
  end;

  option:
    begin
    create SOT;
    create 'option';
    create Attributes;
    create tagend;
    indent +2;
    indent -2 after;
    create SCT after;
    create 'repeat>\12' after;
  end;

  useEl: 
    begin
    create SOT;
    create 'use';
    create Attributes;
    create tagend;
    indent +2;
    indent -2 after;
    create SCT after;
    create 'use>\12' after;
  end;

  bag:
    begin
    create SOT;
    create 'bag';
    create Attributes;
    create tagend;
    indent +2;
     indent -2 after;
    create SCT after;
    create 'bag>\12' after;
  end;

  attribute:
    begin
    create SOT;
    create 'attribute';
    create Attributes;
    create CloseEmptyTagNL;
  end;

ATTRIBUTES

  name:
    begin
      create SAN;
      create 'name=';
      create DoubleQuote;
      create name;
      create DoubleQuote;
    end;

  includeAt :
    begin
      create SAN;
      create 'include=';
      create DoubleQuote;
      create includeAt;
      create DoubleQuote;
    end;

  exclude:
    begin
      create SAN;
      create 'exclude=';
      create DoubleQuote;
      create exclude;
      create DoubleQuote;
    end;

  src:
    begin
      create SAN;
      create 'src=';
      create DoubleQuote;
      create src;
      create DoubleQuote;
    end;

  minOcc:
    begin
      create SAN;
      create 'minOccurs=';
      create DoubleQuote;
      create minOcc;
      create DoubleQuote;
    end;

  maxOcc:
    begin
      create SAN;
      create 'maxOccurs=';
      create DoubleQuote;
      create maxOcc;
      create DoubleQuote;
    end;

  id:
    begin
      create SAN;
      create 'id=';
      create DoubleQuote;
      create id;
      create DoubleQuote;
    end;

  types:
    begin
      create SAN;
      create 'types=';
      create DoubleQuote;
      create types;
      create DoubleQuote;
    end;

  currentType:
    begin
      create SAN;
      create 'currentType=';
      create DoubleQuote;
      create currentType;
      create DoubleQuote;
    end;

  type:
    begin
      create SAN;
      create 'type=';
      create DoubleQuote;
      create type;
      create DoubleQuote;
    end;

  useAt:
    begin
      create SAN;
      create 'use=';
      create DoubleQuote;
      create useAt;
      create DoubleQuote;
    end;

  valueAt:
    begin
      create SAN;
      create 'default=';
      create DoubleQuote;
      create valueAt;
      create DoubleQuote;
    end;

  fixed:
    begin
      create SAN;
      create 'fixed=';
      create DoubleQuote;
      create fixed;
      create DoubleQuote;
    end;

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

END

