            { A. Tollenaere	July 2005 }

	{ Templates schema. Only implements }
	{ protected areas yet }
	
STRUCTURE Template;

DEFPRES TemplateP;

ATTR
   { No global attributes for protected areas }

   { generic attributes for internal use }
   Unknown_attribute = TEXT;	{ to store an unknown attribute }
   Ghost_restruct = TEXT;	{ to help structure transformation }

CONST

  C_Empty = ' ';

STRUCT

  Template { No attribute for the root } =
	CASE OF
		free_struct = ANY ;
		free_content = ANY ;
	END; 
	
EXCEPT

    Unknown_attribute: Invisible;
    free_struct: NoCreate;
		free_content: NoCreate;

END
