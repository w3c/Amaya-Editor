      { Structure Schema for XTiger Templates
        Francesc Campoy Flores & I. Vatton    January 2007 }

	
STRUCTURE Template;

DEFPRES TemplateP;

ATTR
  { generic attributes for internal use }
  Unknown_attribute = TEXT;	{ to store an unknown attribute }

CONST

  C_CR = '\12';

STRUCT

  Template = CASE OF
      head;
      repeat;
      option;
      useEl;
      useSimple;
      bag;
      attribute;
    END;

  head = LIST OF ( 
    Declaration = CASE OF
      component;
      union;
      import;
    END
   );

  component ( ATTR name = TEXT; title = TEXT ) = ANY - ( head, union, import );
  union ( ATTR name; includeAt = TEXT; exclude = TEXT; title ) = CONSTANT C_CR; {It is always empty}
  import ( ATTR src = TEXT ) = CONSTANT C_CR; {It is always empty}

  folder = ANY - ( head, union, import, component );

  repeat ( ATTR ref = TEXT; minOccurs = TEXT; maxOccurs = TEXT; currentOccurs =
  	TEXT; title ) =
  	LIST OF ( Repetition =
		  CASE OF
		    useEl;
        useSimple;
	    	option;
		    repeat;
		    bag;
		    folder;
		    END);


  option ( ATTR ref )
  	= ANY - ( head, union, import, component );
  
  useEl ( ATTR ref; types = TEXT; currentType = TEXT; title )
     = ANY - ( head, union, import, component );

  useSimple ( ATTR ref; types; currentType; title )
     = ANY - ( head, union, import, component );

  bag ( ATTR ref; types ) 
     =  LIST OF (ANY) - ( head, union, import, component );

  attribute ( ATTR 
    ref_name = TEXT; 
    type = integerVal, decimal, string, listVal; 
    useAt = TEXT; 
    defaultAt = TEXT; 
    fixed = TEXT; 
    values = TEXT;
    title ) = CONSTANT C_CR; {It is always empty}

EXCEPT
  useEl:              NoReplicate, NoBreakByReturn, NoCreate;
  useSimple:          NoReplicate, NoBreakByReturn, NoCreate;
  repeat:             NoReplicate, NoBreakByReturn, NoCreate;
  option:             NoReplicate, NoBreakByReturn, NoCreate;
  bag:                NoReplicate, NoBreakByReturn, NoCreate, ParagraphBreak;
  {Hiding unkown and language attributes}
	Unknown_attribute:  Invisible;
	Language:           Invisible;
  types :             DuplicateAttr;
  currentType:        Invisible, GiveName;
  ref:                Invisible;
	
END
