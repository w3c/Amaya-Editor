        { This schema represents the structure of an XML document

	  S.Bonhomme      Dec 1997        }

STRUCTURE XmlStr;

DEFPRES XmlStrP;

STRUCT
  
   XmlStr (Attr !Gi =  TEXT;  Validity = Valid, Ambiguous, Unvalid) =
        Tag;

   Component = 
         CASE OF
         Tag;
         Pc_Data = TEXT;
         END;
   
   Tag (Attr !Gi;  Validity) =
         BEGIN
	?Attributes = LIST OF (Attribute);
	Contents = LIST OF (Component); 
         END;

   Attribute = 
         BEGIN
         Attribute_Name = TEXT;
        ?Attribute_Value = TEXT;
	 END;

EXCEPT
   Component : Hidden;
   Contents  : Hidden;
   Attributes: Hidden;
END
