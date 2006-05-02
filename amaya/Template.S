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

  component (ATTR name = TEXT) = ANY - (head, union, import);
  union (ATTR name; includeAt = TEXT; exclude = TEXT) = CONSTANT C_CR; {It is always empty}
  import (ATTR src = TEXT) = CONSTANT C_CR; {It is always empty}

  Model = CASE OF
      component;
      useEl;
      bag;
      ANY;
      END - (head, union, import, repeat, option);

  repeat (ATTR minOcc = INTEGER; maxOcc = INTEGER) = BEGIN 
    RepeatContent = CASE OF
      Model;
      Repetitions = LIST OF (useEl); 
    END;
  END;

  option = BEGIN 
    Model;
  END;
  
  useEl (ATTR id = TEXT; types = TEXT; currentType = TEXT)
     = BEGIN
        useMenu;
        ANY;
       END - (head, union, import, component);

  useMenu = CONSTANT C_CR; {Just a button}  

  bag (ATTR id; types) = ANY - (head, union, import, component);

  attribute (ATTR 
    name; 
    type = TEXT; 
    useAt = TEXT; 
    valueAt = TEXT; 
    fixed = true, false) = CONSTANT C_CR; {It is always empty}

EXCEPT
  {Hiding unkown and language attributes}
	Unknown_attribute: Invisible;
	Language: Invisible;
	
	{The use button is not editable}
	useMenu : NoCut, NoCreate, NoMove, NoResize, NoSelect;

END
