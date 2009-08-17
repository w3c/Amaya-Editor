	{ Translation Schema for XTiger Templates
	  Francesc Campoy Flores & I. Vatton    2007 - 2008 }


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
    create SOT;
    create 'library';
    create Attributes;
    create tagend;
    indent +2;
    indent -2 after;
    create SCT after;
    create 'library>\12' after;
  end;

  DOCTYPE: BEGIN
	  NoLineBreak;
	  NoTranslation;
	  Create '\12' After;
	END;

  DOCTYPE_line:
	  if not last create '\12' after;

  head:
    begin
    create '\12';
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
    create '\12';
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
    create '\12';
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
    create '\12';
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

  useEl: 
    begin
    create SOT;
    create 'use';
    create Attributes;
    create '>';
    indent +2;
    indent -2 after;
    create SCT after;
    create 'use>' after;
  end;

  useSimple: 
    begin
    create SOT;
    create 'use';
    create Attributes;
    create '>';
    indent +2;
    indent -2 after;
    create SCT after;
    create 'use>' after;
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
  
 Comment\240: BEGIN
        NoLineBreak;
         Create '<!--';
        {NoTranslation;}
        Create '-->\12' after;
        END;

 Comment_line: BEGIN
        if not last create '\12' after;
        END;
        
 XMLPI: BEGIN
        NoLineBreak;
         Create '<?';
        {NoTranslation;}
        Create '?>\12' after;
        END;

 PI_line: BEGIN
        if not last create '\12' after;
        END;
  
ATTRIBUTES
	
  option = option_set :
    begin
      create SAN;
      create 'option="set"';
    end;
  option = option_unset :
    begin
      create SAN;
      create 'option="unset"';
    end;

  xmlid :
    begin
      create SAN;
      create 'xml:id=';
      create DoubleQuote;
      create name;
      create DoubleQuote;
    end;

  name :
    begin
      create SAN;
      create 'name=';
      create DoubleQuote;
      create name;
      create DoubleQuote;
    end;

  ref_name :
    begin
      create SAN;
      create 'name=';
      create DoubleQuote;
      create ref_name;
      create DoubleQuote;
    end;

  title :
    begin
      create SAN;
      create 'label=';
      create DoubleQuote;
      create title;
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

  exclude :
    begin
      create SAN;
      create 'exclude=';
      create DoubleQuote;
      create exclude;
      create DoubleQuote;
    end;

  src :
    begin
      create SAN;
      create 'src=';
      create DoubleQuote;
      create src;
      create DoubleQuote;
    end;
	
  minOccurs :
    begin
      create SAN;
      create 'minOccurs=';
      create DoubleQuote;
      create minOccurs;
      create DoubleQuote;
    end;

  maxOccurs :
    begin
      create SAN;
      create 'maxOccurs=';
      create DoubleQuote;
      create maxOccurs;
      create DoubleQuote;
    end;

  types :
    begin
      create SAN;
      create 'types=';
      create DoubleQuote;
      create types;
      create DoubleQuote;
    end;

  currentType :
    begin
      create SAN;
      create 'currentType=';
      create DoubleQuote;
      create currentType;
      create DoubleQuote;
    end;
	
  type :
    begin
      create SAN;
      create 'type=';
      create DoubleQuote;
      create type;
      create DoubleQuote;
    end;

  type = number:
    begin
      create SAN;
      create 'type="number"';
    end;

  type = string:
    begin
      create SAN;
      create 'type="string"';
    end;

  type = listVal:
    begin
      create SAN;
      create 'type="list"';
    end;
	
  prompt :
    begin
      create SAN;
      create 'initial="true"';
    end;

  useAt = required:
    begin
      create SAN;
      create 'use="required"';
    end;

  useAt = optional:
    begin
      create SAN;
      create 'use="optional"';
    end;

  useAt = prohibited:
    begin
      create SAN;
      create 'use="prohibited"';
    end;

  defaultAt :
    begin
      create SAN;
      create 'default=';
      create DoubleQuote;
      create defaultAt;
      create DoubleQuote;
    end;

  fixed :
    begin
      create SAN;
      create 'fixed=';
      create DoubleQuote;
      create fixed;
      create DoubleQuote;
    end;

  values :
    begin
      create SAN;
      create 'values=';
      create DoubleQuote;
      create values;
      create DoubleQuote;
    end;

  version :
    begin
      create SAN;
      create 'version=';
      create DoubleQuote;
      create version;
      create DoubleQuote;
    end;

  templateVersion :
    begin
      create SAN;
      create 'templateVersion=';
      create DoubleQuote;
      create templateVersion;
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

TextTRANSLATE
        BEGIN
        '\46' -> '&amp;';        { ampersand, U+0026 ISOnum }
        '\74' -> '&lt;';        { less-than sign, U+003C ISOnum }
        '\76' -> '&gt;';         { greater-than sign, U+003E ISOnum }
        END;

END

