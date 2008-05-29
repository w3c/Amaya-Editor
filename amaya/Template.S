      { Structure Schema for XTiger Templates
        Francesc Campoy Flores & I. Vatton    January 2007 }

	
STRUCTURE Template;

DEFPRES TemplateP;

ATTR
  { generic attributes for internal use }
  Unknown_attribute = TEXT;	{ to store an unknown attribute }
  xmlid = Text;		{ id }

CONST

  C_CR = '\12';

STRUCT

  Template ( ATTR version = TEXT; templateVersion = TEXT ) = LIST OF (TemplateObject);

  TemplateObject = CASE OF
      head;
      repeat;
      option;
      useEl;
      useSimple;
      bag;
      attribute;
    END + (Comment\240, XMLPI);

  head ( ATTR version; templateVersion ) = 
    LIST OF ( Declaration = CASE OF
      component;
      union;
      import;
    END
   );

  Comment\240 = LIST OF (Comment_line = TEXT) - (Comment\240);
  
  XMLPI (ATTR is_css = Text) = LIST OF (PI_line = TEXT);

  component ( ATTR name = TEXT ) = ANY - ( head, union, import );
  union ( ATTR name; includeAt = TEXT; exclude = TEXT ) = CONSTANT C_CR; {It is always empty}
  import ( ATTR src = TEXT ) = CONSTANT C_CR; {It is always empty}

  repeat ( ATTR title = TEXT; minOccurs = TEXT; maxOccurs = TEXT) =
  	LIST OF ( Repetition =
		  CASE OF
		    useEl;
        useSimple;
	    	option;
		    repeat;
		    bag;
		    END);


  option ( ATTR title )
  	= ANY - ( head, union, import, component );
  
  useEl ( ATTR title; types = TEXT; currentType = TEXT;  prompt = Yes_ )
     = ANY - ( head, union, import, component );

  useSimple ( ATTR title; types; prompt)
     = ANY - ( head, union, import, component );

  bag ( ATTR title; types ) 
     =  LIST OF (ANY) - ( head, union, import, component );

  attribute ( ATTR 
    ref_name = TEXT; 
    type = number, string, listVal; 
    useAt = required, optional, prohibited;
    defaultAt = TEXT; 
    fixed = TEXT; 
    values = TEXT;
    ) = CONSTANT C_CR; {It is always empty}

EXCEPT
  XMLPI:              ReturnCreateNL, NoSpellCheck, NoReplicate, NotAnElementNode;
  PI_line:            Hidden, MarkupPreserve;
  Comment\240:        ReturnCreateNL, NoSpellCheck, NoReplicate, NotAnElementNode;
  Comment_line:       Hidden, MarkupPreserve;
  useEl:              NoReplicate, NoBreakByReturn;
  useSimple:          NoReplicate, NoBreakByReturn;
  repeat:             NoReplicate, NoBreakByReturn, NoCreate;
  option:             NoReplicate, NoBreakByReturn, NoCreate;
  bag:                NoReplicate, NoBreakByReturn, NoCreate, ParagraphBreak;
  {Hiding unkown and language attributes}
	Unknown_attribute:  Invisible;
	Language:           Invisible;
  types :             GiveTypes, DuplicateAttr;
  currentType:        Invisible;
  title:              GiveName;
  ref_name:		        GiveName;
  name:			          GiveName;
  prompt:             Invisible;
  xmlid:		          CssId;
	
END
