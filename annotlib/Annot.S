		{ Author: J. Kahan 	 1999-2003  }

STRUCTURE Annot;

DEFPRES AnnotP;

CONST

   C_Empty = ' ';

STRUCT

  { An annotation is made of a description, an HTML body, and, optionally
    a thread }
	
  Annot
     (ATTR  XmlBody = Yes_; Charset = Text) = 
          BEGIN
	  Description;
	  Body = CASE OF      { the annotation body can be of any type }
                     HTML;
		     MathML;
		     SVG;
		     XML;
		     TextFile;
                   END;
	  ? Thread;
	  END;

  Description = AGGREGATE
	        ATitle = Text;
		Author = CONSTANT C_Empty;
		CreatorEmail = CONSTANT C_Empty;
		CreatorGivenName = CONSTANT C_Empty;
		CreatorFamilyName = CONSTANT C_Empty;
		SourceDoc (ATTR HREF_=Text) = CONSTANT C_Empty;
		RDFtype (ATTR isReply=YES_,NO_) = CONSTANT C_Empty with isReply ? =  NO_; 
		AnnotCDate = CONSTANT C_Empty;
		AnnotMDate = CONSTANT C_Empty;
	        END;

  { ======= THREADS ====== }

  {a thread is made of one or more thread items}
  Thread  = LIST OF (Thread_item); 

  { a thread item contains a description and may contain other thread items }
  Thread_item (ATTR  HREF_; Annot_HREF_ = Text; Orphan_item = Yes_) =                   { link to the annotation }
		{ HREF_ = body URL;  Annot_HREF_ = Annot Meta URL; Orphan_item  = no parent t }
                BEGIN
	        TI_desc;
	        ? TI_content = LIST OF (Thread_item);
	        END;

  TI_desc (ATTR Selected_ = Yes_;) { user is viewing this thread item }
       = BEGIN
         TI_Title    = TEXT;
         TI_Date     = TEXT;
	 TI_Author   = TEXT;
	 TI_Type     = TEXT;
         END;

EXCEPT 
        { Annotation }
	Annot:		NoMove, NoResize;

        { metadata }
	Description:	NoCut;
	ATitle:         NoCut;
        Author:	        NoCut;
        CreatorEmail:   NoCut;
        CreatorFamilyName: NoCut;
        CreatorGivenName: NoCut;
	SourceDoc:      NoCut;
	RDFtype:        NoCut;
	isReply:	Invisible;
        AnnotCDate:     NoCut;
        AnnotMDate:     NoCut;

	{ body }
	Body:		NoCut;
	C_Empty:	NoCut;

	{ threads }
	Thread:		NoCut;
	Thread_item:    NoCut;
	TI_desc:        NoCut;
	TI_content:	NoCut;
        Annot_HREF_:	Invisible; 
	Orphan_item:	Invisible;
END

