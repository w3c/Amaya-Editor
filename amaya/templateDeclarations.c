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
	char *booleanName = strdup("boolean");
	char *stringName  = strdup("string" );

	Add ( lib->declaredTypes, integerName, NewSimpleType(lib, integerName, XTIGER_NUMBER ));
	Add ( lib->declaredTypes, stringName,  NewSimpleType(lib, stringName,  XTIGER_STRING  ));
	Add ( lib->declaredTypes, booleanName, NewSimpleType(lib, booleanName, XTIGER_BOOLEAN ));

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
void FreeDeclaration (Declaration dec)
{
	//TODO
	TtaFreeMemory(dec->name);
	TtaFreeMemory(dec);
}


/*----------------------------------------------------------------------
  Creates a new template with its dictionaries
  ----------------------------------------------------------------------*/
XTigerTemplate NewXTigerTemplate (ThotBool addPredefined)
{	
	XTigerTemplate t = (XTigerTemplate)TtaGetMemory (sizeof(_XTigerTemplate));
	
	t->isLibrary     = false;
	t->libraries     = CreateDictionary();
	t->declaredTypes = CreateDictionary();
	if (addPredefined)
		AddLibraryDeclarations (t,(XTigerTemplate)Get (templates, PREDEFINED_LIB));

	return t;
}

/*----------------------------------------------------------------------
  Free all the space used by a template (also its dictionaries)
  ----------------------------------------------------------------------*/
void FreeXTigerTemplate (XTigerTemplate t)
{	
	DicDictionary  dic = t->libraries;
  XTigerTemplate lib;
	Declaration    dec;

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
	dic = t->declaredTypes;

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
