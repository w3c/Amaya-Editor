            { A. Tollenaere	July 2005 }

	{ Templates schema. Only implements }
	{ protected areas yet }
	
STRUCTURE Template;

DEFPRES TemplateP;

ATTR
   { No global attributes for protected areas }

   xmlid = Text;		{ xml:id }

   { generic attributes for internal use }
   Unknown_attribute = TEXT;	{ to store an unknown attribute }

CONST

  C_Empty = ' ';

STRUCT

  Template { The root has no attribute } =
	CASE OF
    FREE_STRUCT = ANY ;
		FREE_CONTENT = ANY ;
	END; 
	
EXCEPT

    Unknown_attribute: Invisible;
    FREE_STRUCT: NoCreate;
		FREE_CONTENT: NoCreate;

END
