	 { L. Carcone 	 May 1999 } 

TRANSLATION XmlStr;

LINELENGTH 78;

VAR
	VarGi : Gi;

RULES

XmlStr:
	BEGIN
	Create '<';
	Create Attributes;
	Get Attribute;
	Create '>\12';
	Create '</' After;
	Create VarGi After;
	Create '>\12' After;
	END;

Tag:
	BEGIN
	Create '<';
	Create Attributes;
	Get Attribute;
	Create '>';
	Create '</' After;
	Create VarGi After;
	Create '>\12' After;
	END;

Attribute:
	BEGIN
	Create ' ';
	END;

Attribute_Name:
        BEGIN
	END;

Attribute_Value:
        BEGIN
        Create '="';
	Create '"'  After;
	END;
	
Component:
	BEGIN
	END;

ATTRIBUTES

Gi:
	BEGIN
	Create Gi Before;
	END;

END
