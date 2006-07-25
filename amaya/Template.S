      { Structure Schema for XTiger Templates
	  Francesc Campoy Flores     31/03/2006 }

	
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

  folder = BEGIN
  		ANY - ( head, union, import, component );
  	END;

  repeat ( ATTR id = TEXT; minOccurs = TEXT; maxOccurs = TEXT; currentOccurs = TEXT; title )
  	= LIST OF ( Repetition 
		= CASE OF
		useEl;
		option;
		repeat;
		bag; 
		folder;
		END
	);


  option ( ATTR id )
  	= BEGIN 
    	ANY - ( head, union, import, component );
  	END;
  
  useEl ( ATTR id; types = TEXT; currentType = TEXT; title )
     = BEGIN
        ANY;
       END - ( head, union, import, component );

  bag ( ATTR id; types ) 
     = BEGIN
        ANY;
       END - ( head, union, import, component );

  attribute ( ATTR 
    name; 
    type = integerVal, decimal, string, listVal; 
    useAt = TEXT; 
    defaultAt = TEXT; 
    fixed = TEXT; 
    values = TEXT;
    title ) = CONSTANT C_CR; {It is always empty}

EXCEPT
    {Hiding unkown and language attributes}
	Unknown_attribute	: Invisible;
	Language			: Invisible;
	
END
