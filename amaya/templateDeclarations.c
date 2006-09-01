/*
 *  FILE   : templateDeclarations.c
 *  DESC   : Declaration structures and creators for XTiger types.
 *  AUTHOR : Francesc Campoy Flores
 */

#ifdef TEMPLATES

#include "templates.h"

#include "mydictionary_f.h"
#include "templateDeclarations_f.h"
#include "HTMLactions_f.h"

#define UNION_ANY            "any"
#define UNION_ANYCOMPONENT   "anyComponent"
#define UNION_ANYSIMPLE      "anySimple"
#define UNION_ANYELEMENT     "anyElement"
#define UNION_ANY_DEFINITION "anyComponent anySimple anyElement"

#define TYPE_NUMBER          "number"
#define TYPE_STRING          "string"
#define TYPE_BOOLEAN         "boolean"

//The predefined library id
#define PREDEFINED_LIB "-Predefined-"

DicDictionary templates = NULL;

#endif /* TEMPLATES */

/*----------------------------------------------------------------------
  Creates a new template with its dictionaries
  ----------------------------------------------------------------------*/
XTigerTemplate NewXTigerTemplate (const char *templatePath, const ThotBool addPredefined)
{	
#ifdef TEMPLATES
	XTigerTemplate t = (XTigerTemplate)TtaGetMemory (sizeof(_XTigerTemplate));
	
	t->isLibrary    = FALSE;
	t->libraries    = CreateDictionary();
	t->elements		  = CreateDictionary();
	t->simpleTypes	= CreateDictionary();
	t->components	  = CreateDictionary();
	t->unions		    = CreateDictionary();
	t->doc          = -1;
  t->users        = 0;
  t->isPredefined = FALSE;

	if (addPredefined)
		AddLibraryDeclarations (t,(XTigerTemplate)Get (templates, PREDEFINED_LIB));	

	Add (templates, templatePath, t);

	return t;
#else
	return NULL;
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Creates a new library with its dictionaries
  ----------------------------------------------------------------------*/
XTigerTemplate NewXTigerLibrary (const char *templatePath, const ThotBool addPredefined)
{	
#ifdef TEMPLATES
	XTigerTemplate t = (XTigerTemplate)NewXTigerTemplate(templatePath, addPredefined);
	t->isLibrary = TRUE;

	return t;
#else
	return NULL;
#endif /* TEMPLATES */
}


/*----------------------------------------------------------------------
Creates a new declaration. t and name must be not NULL or this function
will return NULL
  ----------------------------------------------------------------------*/
static Declaration NewDeclaration (const XTigerTemplate t, const char *name,
                                   TypeNature xtype)
{
#ifdef TEMPLATES
    if(name==NULL || t==NULL)
		return NULL;
	Declaration dec = (Declaration) TtaGetMemory(sizeof(_Declaration));
	dec->declaredIn = t;
	dec->name = TtaStrdup (name);
	dec->nature = xtype;
	return dec;
#else
	return NULL;
#endif /* TEMPLATES */
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void NewSimpleType (XTigerTemplate t, const char *name, SimpleTypeType xtype)
{
#ifdef TEMPLATES
	Declaration dec = NewDeclaration (t, name, SimpleTypeNat);	
	dec->simpleType.type = xtype;
	AddDeclaration(t, dec);
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Returns a library with the predefined types
  ----------------------------------------------------------------------*/
XTigerTemplate CreatePredefinedTypesLibrary ()
{
#ifdef TEMPLATES
	XTigerTemplate lib = NewXTigerLibrary(PREDEFINED_LIB, FALSE);
	lib->isLibrary = true;

	NewSimpleType(lib, TYPE_NUMBER,  XTNumber);
	NewSimpleType(lib, TYPE_BOOLEAN, XTBoolean);
	NewSimpleType(lib, TYPE_STRING,  XTString);

	NewUnion(lib, UNION_ANYCOMPONENT, NULL, NULL);
	NewUnion(lib, UNION_ANYSIMPLE, NULL, NULL);
	NewUnion(lib, UNION_ANYELEMENT, NULL, NULL);
  
	NewUnion(lib, UNION_ANY, CreateDictionaryFromList(UNION_ANY_DEFINITION), NULL);

  lib->isPredefined = TRUE;

	return lib;
#else
	return NULL;
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Initializing the template environment
  ----------------------------------------------------------------------*/
void InitializeTemplateEnvironment ()
{
#ifdef TEMPLATES
	templates = CreateDictionary();
	Add (templates, PREDEFINED_LIB, CreatePredefinedTypesLibrary());	
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Releasing the template environment
  ----------------------------------------------------------------------*/

void FreeTemplateEnvironment()
{
#ifdef TEMPLATES
  XTigerTemplate t;

  if (templates)
    for(First(templates);!IsDone(templates);Next(templates))
	{
      t = (XTigerTemplate)CurrentElement(templates);
      TtaCloseDocument(t->doc);
    }
#endif
}

/*----------------------------------------------------------------------
  Adds a new declaration or redefines an existing one
  ----------------------------------------------------------------------*/
void AddDeclaration (XTigerTemplate t, Declaration dec)
{
#ifdef TEMPLATES
	Declaration old = GetDeclaration(t, dec->name);
	
	if(old==NULL) //New type, not a redefinition
	{
		switch(dec->nature)
		{
		case SimpleTypeNat :
			Add(t->simpleTypes, dec->name, dec);
			break;
		case XmlElementNat :
			Add(t->elements, dec->name, dec);
			break;
		case ComponentNat :
			Add(t->components, dec->name, dec);
			break;
		case UnionNat :
			Add(t->unions, dec->name, dec);
			break;
		default :
			//Impossible
			break;
		}
	}
	else //A redefinition. Using the old memory zone to keep consistent pointers
	{
		//TODO CopyDeclarationInto(dec, old);
		TtaFreeMemory(dec);
	}
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void NewComponent (XTigerTemplate t, const char *name, const Element el)
{
#ifdef TEMPLATES
	/*if(t->doc < 0)
		doc = CreateDocumentOfType(el);*/

	Declaration dec = NewDeclaration (t, name, ComponentNat);
	dec->componentType.content = el;
// TODO	dec->componentType.content = TtaCopyTree(el, TtaGetDocument(el), ;
	AddDeclaration(t, dec);
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void NewUnion (const XTigerTemplate t, const char *name,
                       DicDictionary include, DicDictionary exclude)
{
#ifdef TEMPLATES
	Declaration dec = NewDeclaration (t, name, UnionNat);
	Declaration aux;
	
	dec->unionType.include = CreateDictionary();
	dec->unionType.exclude = CreateDictionary();

	//We initialize include
	if(include != NULL)
	{
		for (First(include); !IsDone(include); Next(include)) {
			
			aux = GetDeclaration(t, CurrentKey(include));
			
			if(aux==NULL) //Unknown type > a new XML element
				NewElement(t, CurrentKey(include));
			
			Add(dec->unionType.include, aux->name, aux);
		}
	}
	
	//We initialize exclude
	if(exclude != NULL)
	{
		for (First(exclude); !IsDone(exclude); Next(exclude)) {
			
			aux = GetDeclaration(t, CurrentKey(exclude));
			
			if(aux==NULL) //Unknown type > a new XML element
				NewElement(t, CurrentKey(exclude));
			
			Add(dec->unionType.exclude, aux->name, aux);
		}
	}

	AddDeclaration(t, dec);
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void NewElement (const XTigerTemplate t, const char *name)
{
#ifdef TEMPLATES
	Declaration dec = NewDeclaration (t, name, XmlElementNat);
	AddDeclaration(t, dec);
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void FreeDeclaration (Declaration dec)
{
#ifdef TEMPLATES
	//TODO : remove it from its dictionary
	//TODO : clean dictionaries etc.
	TtaFreeMemory(dec->name);
	TtaFreeMemory(dec);
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
Declaration GetDeclaration(const XTigerTemplate t, const char *name)
{
#ifdef TEMPLATES
	Declaration dec = (Declaration)Get(t->simpleTypes, name);	
	if(dec) return dec;
	dec = (Declaration)Get(t->components, name);
	if(dec) return dec;
	dec = (Declaration)Get(t->elements, name);
	if(dec) return dec;
	dec = (Declaration)Get(t->unions, name);
	return dec;
#else
	return NULL;
#endif /* TEMPLATES */
}


/*----------------------------------------------------------------------
  Free all the space used by a template (also its dictionaries)
  ----------------------------------------------------------------------*/
void FreeXTigerTemplate (XTigerTemplate t)
{	
#ifdef TEMPLATES
	DicDictionary  dic;
	Declaration    dec;

	dic = t->libraries;

	for (First(dic); !IsDone(dic); Next(dic))
    RemoveUser((XTigerTemplate)CurrentElement(dic));

	CleanDictionary (dic);

	//Cleaning the components
	dic = t->components;
	for (First(dic); !IsDone(dic); Next(dic))
    {
      dec = (Declaration)CurrentElement (dic);
	  //Deleting only types defined by the template (not the imported ones)
      if (dec->declaredIn == t)
        TtaFreeMemory (dec);
    }
	CleanDictionary (dic);

	//Cleaning the elements
	dic = t->elements;
	for (First(dic); !IsDone(dic); Next(dic))
    {
      dec = (Declaration)CurrentElement (dic);
	  //Deleting only types defined by the template (not the imported ones)
      if (dec->declaredIn == t)
        TtaFreeMemory (dec);
    }
	CleanDictionary (dic);

	//Cleaning the simple types
	dic = t->simpleTypes;
	for (First(dic); !IsDone(dic); Next(dic))
    {
      dec = (Declaration)CurrentElement (dic);
	  //Deleting only types defined by the template (not the imported ones)
      if (dec->declaredIn == t)
        TtaFreeMemory (dec);
    }
	CleanDictionary (dic);

	//Cleaning the unions
	dic = t->unions;
	for (First(dic); !IsDone(dic); Next(dic))
    {
      dec = (Declaration)CurrentElement (dic);
	  //Deleting only types defined by the template (not the imported ones)
      if (dec->declaredIn == t)
        TtaFreeMemory (dec);
    }
	CleanDictionary (dic);

  //Freeing the document
  FreeDocumentResource(t->doc);
  TtcCloseDocument(t->doc, 0);

  //Removing the template of the dictionary
  RemoveElement(templates, t);

  //Freeing the template
	TtaFreeMemory (t);
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
Imports all declarations in a library lib to a template t
----------------------------------------------------------------------*/
void AddLibraryDeclarations (XTigerTemplate t, XTigerTemplate lib)
{	
#ifdef TEMPLATES
	DicDictionary from = lib->elements;	
	DicDictionary to = t->elements;
	
	for (First(from); !IsDone(from); Next(from))
		Add (to, CurrentKey(from), CurrentElement(from));
	
	from = lib->components;	
	to = t->components;
	
	for (First(from); !IsDone(from); Next(from))
		Add (to, CurrentKey(from), CurrentElement(from));
	
	from = lib->unions;	
	to = t->unions;
	
	for (First(from); !IsDone(from); Next(from))
		Add (to, CurrentKey(from), CurrentElement(from));
	
	from = lib->simpleTypes;	
	to = t->simpleTypes;
	
	for (First(from); !IsDone(from); Next(from))
		Add (to, CurrentKey(from), CurrentElement(from));
#endif /* TEMPLATES */
}

void PrintUnion (Declaration dec, int indent, XTigerTemplate t, FILE *file)
{	
#ifdef TEMPLATES
	DicDictionary  dic;
	Declaration    aux;
	char*		   indentation;
	int 		   i=0;
	
	indentation = (char*) TtaGetMemory(indent*sizeof(char)+1);
	for(i=0;i<indent;i++)
		indentation [i] = '\t';
	indentation [indent] = '\0';
	
	
	dic = dec->unionType.include;
	if(!IsEmpty(dic))
	{
		fprintf(file, "\n%sINCLUDE",indentation);
		
		for(First(dic);!IsDone(dic);Next(dic))
		{
			aux = (Declaration) CurrentElement(dic);
			switch(aux->nature)
			{
			case SimpleTypeNat :
			case XmlElementNat :
			case ComponentNat :
				fprintf(file, "\n%s+ %s ",indentation,aux->name);
				if(aux->declaredIn!=t)
					fprintf(file, "*");
				break;
			case UnionNat :
				fprintf(file, "\n%s+ %s ",indentation,aux->name);
				if(aux->declaredIn!=t)
					fprintf(file, "*");
				PrintUnion(aux, indent+1, t, file);
			default :
				//impossible
				break;
			}
		}
	}
	
	dic = dec->unionType.exclude;
	if(!IsEmpty(dic))
	{
		fprintf(file, "\n%sEXCLUDE",indentation);
		
		for(First(dic);!IsDone(dic);Next(dic))
		{
			aux = (Declaration) CurrentElement(dic);
			switch(aux->nature)
			{
			case SimpleTypeNat :
			case XmlElementNat :
			case ComponentNat :			
				fprintf(file, "\n%s- %s ",indentation,aux->name);
				if(aux->declaredIn!=t)
					fprintf(file, "*");
				break;
			case UnionNat :
				fprintf(file, "\n%s- %s ",indentation,aux->name);
				if(aux->declaredIn!=t)
					fprintf(file, "*");
				PrintUnion(aux, indent+1, t, file);
			default :
				//impossible
				break;
			}
		}
	}
	
	TtaFreeMemory(indentation);
#endif /* TEMPLATES */
}

void PrintDeclarations (XTigerTemplate t, FILE *file)
{
#ifdef TEMPLATES
	DicDictionary  aux;
	Declaration    dec;
	
	fprintf(file, "SIMPLE TYPES\n");
	fprintf(file, "------------");
	aux = t->simpleTypes;
	for(First(aux);!IsDone(aux);Next(aux))
	{
		dec = (Declaration) CurrentElement(aux);
		fprintf(file, "\n%s ",dec->name);
		if(dec->declaredIn!=t)
			fprintf(file, "*");
	}
	
	aux = t->elements;
	if(!IsEmpty(aux))
	{
		fprintf(file, "\n\nXML ELEMENTS\n");
		fprintf(file, "------------");	
		for(First(aux);!IsDone(aux);Next(aux))
		{
			dec = (Declaration) CurrentElement(aux);
			fprintf(file,"\n%s ",dec->name);
			if(dec->declaredIn!=t)
				fprintf(file, "*");
		}
	}
	
	aux = t->components;
	if(!IsEmpty(aux))
	{
		fprintf(file, "\n\nCOMPONENTS\n");
		fprintf(file, "----------");	
		for(First(aux);!IsDone(aux);Next(aux))
		{
			dec = (Declaration) CurrentElement(aux);
			fprintf(file,"\n%s ",dec->name);
			if(dec->declaredIn!=t)
				fprintf(file, "*");
			fprintf(file,"\n********************\n");
			TtaListAbstractTree (dec->componentType.content, file);
			fprintf(file,"********************\n");
		}
	}
	
	aux = t->unions;
	if(!IsEmpty(aux))
	{
		fprintf(file, "\n\nUNIONS\n");
		fprintf(file, "------");
		for(First(aux);!IsDone(aux);Next(aux))
		{
			dec = (Declaration) CurrentElement(aux);
			fprintf(file,"\n%s ",dec->name);
			if(dec->declaredIn!=t)
				fprintf(file, "*");
			PrintUnion(dec, 1, t, file);
		}
	}
#endif /* TEMPLATES */
}

void DumpDeclarations(XTigerTemplate t)
{
#ifdef TEMPLATES
	char localname[MAX_LENGTH];
	FILE *file;

    strcpy (localname, TempFileDirectory);
    strcat (localname, DIR_STR);
    strcat (localname, "templateDecl.debug");
    file = TtaWriteOpen (localname);
		
	PrintDeclarations(t, file);

	TtaWriteClose (file);
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
RedefineSpecialUnions: Redefines predefined unions : any, anySimple, etc 
----------------------------------------------------------------------*/
void RedefineSpecialUnions(XTigerTemplate t)
{
#ifdef TEMPLATES
  Declaration un;
  DicDictionary dic;

  //We get the old definition to modify it 
  un = GetDeclaration(t,UNION_ANYSIMPLE);
  //TODO : Check that it is actually the good declaration and not a newer one
  un->unionType.include = t->simpleTypes;
  un->unionType.exclude = CreateDictionary();
  
  un = GetDeclaration(t,UNION_ANYCOMPONENT);
  //TODO : Check that it is actually the good declaration and not a newer one
  un->unionType.include = t->components;
  un->unionType.exclude = CreateDictionary();

  un = GetDeclaration(t,UNION_ANYELEMENT);
  //TODO : Check that it is actually the good declaration and not a newer one
  un->unionType.include = t->elements;
  un->unionType.exclude = CreateDictionary();
  
  un = GetDeclaration(t,UNION_ANY);
  //TODO : Check that it is actually the good declaration and not a newer one
  dic = t->unions;
  for(First(dic);!IsDone(dic);Next(dic))
    {
      Add(un->unionType.include, CurrentKey(dic), CurrentElement(dic));
    }

  //No recursive inclusion
  Remove(un->unionType.include, UNION_ANY);
  un->unionType.exclude = CreateDictionary();

#endif /* TEMPLATES */
}

DicDictionary GetComponents(XTigerTemplate t)
{
#ifdef TEMPLATES
  return t->components;
#else
  return NULL;
#endif /* TEMPLATES */
}

Element GetComponentContent(Declaration d)
{
#ifdef TEMPLATES
  if(d->nature == ComponentNat)
    return d->componentType.content;
  else
#endif /* TEMPLATES */
    return NULL;
}


Document GetTemplateDocument(XTigerTemplate t)
{
#ifdef TEMPLATES
  return t->doc;
#else
  return NULL;
#endif /* TEMPLATES */
}

void SetTemplateDocument(XTigerTemplate t, Document doc)
{
#ifdef TEMPLATES
  t->doc = doc;
#endif /* TEMPLATES */
}

void AddUser(XTigerTemplate t)
{
#ifdef TEMPLATES
  t->users++;
#endif /* TEMPLATES */
}

void RemoveUser(XTigerTemplate t)
{
#ifdef TEMPLATES
  t->users--;
  if(t->users == 0 && !t->isPredefined)
    FreeXTigerTemplate(t);  
#endif /* TEMPLATES */
}

