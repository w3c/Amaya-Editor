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

  Template {Virtual} = CASE OF
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

  component (ATTR name = TEXT; title = TEXT) = ANY - (head, union, import);
  union (ATTR name; includeAt = TEXT; exclude = TEXT; title) = CONSTANT C_CR; {It is always empty}
  import (ATTR src = TEXT) = CONSTANT C_CR; {It is always empty}

  Repetition 
  	= CASE OF
  	  useEl;
  	  option;
  	  repeat;
  	  bag; 
    	folder;
    END;

  folder
  	= BEGIN
  		ANY - (head, union, import, component);
  	END;

  repeat (ATTR id = TEXT; minOccurs = TEXT; maxOccurs = TEXT; title)
  	= BEGIN
	  	LIST OF (Repetition);
  		repeatMenu;
  	END;

  option (ATTR id)
  	= BEGIN 
    	ANY - (head, union, import, component);
    	optionMenu;
  	END;
  
  useEl (ATTR id; types = TEXT; currentType = TEXT; title)
     = BEGIN
        ANY;
        useMenu;        
       END - (head, union, import, component);

  bag (ATTR id; types) 
     = BEGIN
        ANY;
        bagMenu;        
       END - (head, union, import, component);

  useMenu 		= CONSTANT C_CR; {Just a button}  
  bagMenu 		= CONSTANT C_CR; {Just a button}  
  repeatMenu 	= CONSTANT C_CR; {Just a button}  
  optionMenu 	= CONSTANT C_CR; {Just a button}  

  attribute (ATTR 
    name; 
    type = integerVal, decimal, string, listVal; 
    useAt = TEXT; 
    defaultAt = TEXT; 
    fixed = TEXT; 
    values = TEXT;
    title) = CONSTANT C_CR; {It is always empty}

EXCEPT
  {Hiding unkown and language attributes}
	Unknown_attribute: Invisible;
	Language: Invisible;
	
	{Element buttons are not editable}
	useMenu			: NoCut, NoCreate, NoMove, NoResize, NoSelect;
	bagMenu 		: NoCut, NoCreate, NoMove, NoResize, NoSelect;
	optionMenu 	: NoCut, NoCreate, NoMove, NoResize, NoSelect;
	repeatMenu 	: NoCut, NoCreate, NoMove, NoResize, NoSelect;

END
