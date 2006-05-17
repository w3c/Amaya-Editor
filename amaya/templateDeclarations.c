/*
 *  FILE   : mydictionary.c
 *  AUTHOR : Francesc Campoy Flores
 */

#ifdef TEMPLATES
#include "templateDeclarations.h"

DicDictionary templates = NULL;

/*----------------------------------------------------------------------
  Returns a library with the predefined types
  ----------------------------------------------------------------------*/
XTigerTemplate CreatePredefinedTypesLibrary ()
{
	XTigerTemplate lib = NewXTigerTemplate(false);
	lib->isLibrary = true;

	char *integerName = strdup("integer");
	char *decimalName = strdup("decimal");
	char *booleanName = strdup("boolean");
	char *stringName  = strdup("string" );

	Add ( lib->simpleTypes, integerName, NewSimpleType(lib, integerName, XTIGER_INTEGER ));
	Add ( lib->simpleTypes, decimalName, NewSimpleType(lib, decimalName, XTIGER_DECIMAL ));
	Add ( lib->simpleTypes, stringName,  NewSimpleType(lib, stringName,  XTIGER_STRING  ));
	Add ( lib->simpleTypes, booleanName, NewSimpleType(lib, booleanName, XTIGER_BOOLEAN ));

	//TODO : Add the any unions
	return lib;
}

/*----------------------------------------------------------------------
  Initializing the template environment
  ----------------------------------------------------------------------*/
DicDictionary InitializeTemplateEnvironment ()
{
	DicDictionary dic = CreateDictionary();
	Add (dic, PREDEFINED_LIB, CreatePredefinedTypesLibrary());	
	return dic;
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static Declaration NewDeclaration (const XTigerTemplate t, const char *name,
                                   TypeNature xtype)
{
  Declaration dec = (Declaration) TtaGetMemory(sizeof(_Declaration));
	dec->declaredIn = t;
	dec->name = TtaStrdup (name);
	dec->nature = xtype;
	return dec;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
Declaration NewSimpleType (const XTigerTemplate t, const char *name,
                           TypeNature xtype)
{
	Declaration dec = NewDeclaration (t, name, SIMPLE_TYPE);	
	dec->simpleType.type = xtype;
	return dec;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
Declaration NewComponent (const XTigerTemplate t, const char *name,
                           const Element el)
{
	Declaration dec = NewDeclaration (t, name, COMPONENT);
	dec->componentType.content = el;
	return dec;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
Declaration NewUnion (const XTigerTemplate t, const char *name,
                       DicDictionary include, DicDictionary exclude)
{
	Declaration dec = NewDeclaration (t, name, UNION);
	dec->unionType.include = include;
	dec->unionType.exclude = exclude;
	return dec;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
Declaration NewElement (const XTigerTemplate t, const char *name)
{
	Declaration dec = NewDeclaration (t, name, XMLELEMENT);
	return dec;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void FreeDeclaration (Declaration dec)
{
	//TODO
	TtaFreeMemory(dec->name);
	TtaFreeMemory(dec);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
Declaration GetDeclaration(const XTigerTemplate t, const char *name)
{
	Declaration dec = (Declaration)Get(t->simpleTypes, name);	
	if(dec) return dec;
	dec = (Declaration)Get(t->components, name);
	if(dec) return dec;
	dec = (Declaration)Get(t->elements, name);
	if(dec) return dec;
	dec = (Declaration)Get(t->unions, name);
	return dec;
}


/*----------------------------------------------------------------------
  Creates a new template with its dictionaries
  ----------------------------------------------------------------------*/
XTigerTemplate NewXTigerTemplate (ThotBool addPredefined)
{	
	XTigerTemplate t = (XTigerTemplate)TtaGetMemory (sizeof(_XTigerTemplate));
	
	t->isLibrary    = false;
//	t->libraries    = CreateDictionary();
	t->elements		= CreateDictionary();
	t->simpleTypes	= CreateDictionary();
	t->components	= CreateDictionary();
	t->unions		= CreateDictionary();
	if (addPredefined)
		AddLibraryDeclarations (t,(XTigerTemplate)Get (templates, PREDEFINED_LIB));

	return t;
}

/*----------------------------------------------------------------------
  Free all the space used by a template (also its dictionaries)
  ----------------------------------------------------------------------*/
void FreeXTigerTemplate (XTigerTemplate t)
{	
	DicDictionary  dic;
	Declaration    dec;
/*
    XTigerTemplate lib;
	dic = t->libraries;
	//TODO : Clean only unused libraries (not the predefined) and elements declared by t
	for (First(dic); !IsDone(dic); Next(dic))
    {
      lib = (XTigerTemplate)CurrentElement (dic);
      lib->documentUsingMe--;
      if (lib->documentUsingMe == 0) //If nobody uses the library
        if (strcmp(CurrentKey(dic), PREDEFINED_LIB) != 0) //And it's not the predefined library
          FreeXTigerTemplate (lib);
    }

	CleanDictionary (dic);
*/
	//Cleaning the components
	dic = t->components;
	for (First(dic); !IsDone(dic); Next(dic))
    {
      dec = (Declaration)CurrentElement (dic);
      if (dec->declaredIn == t)
        TtaFreeMemory (dec);
    }
	CleanDictionary (dic);

	//Cleaning the elements
	dic = t->elements;
	for (First(dic); !IsDone(dic); Next(dic))
    {
      dec = (Declaration)CurrentElement (dic);
      if (dec->declaredIn == t)
        TtaFreeMemory (dec);
    }
	CleanDictionary (dic);

	//Cleaning the simple types
	dic = t->simpleTypes;
	for (First(dic); !IsDone(dic); Next(dic))
    {
      dec = (Declaration)CurrentElement (dic);
      if (dec->declaredIn == t)
        TtaFreeMemory (dec);
    }
	CleanDictionary (dic);

	//Cleaning the unions
	dic = t->unions;
	for (First(dic); !IsDone(dic); Next(dic))
    {
      dec = (Declaration)CurrentElement (dic);
      if (dec->declaredIn == t)
        TtaFreeMemory (dec);
    }
	CleanDictionary (dic);

	TtaFreeMemory (t);
}
#endif /* TEMPLATES */
