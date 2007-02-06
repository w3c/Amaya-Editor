/*
 *
 *  COPYRIGHT INRIA and W3C, 1996-2007
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *  FILE  : templateDeclarations.c
 *  DESC  : Declaration structures and creators for XTiger types.
 *  AUTHOR: Francesc Campoy Flores
 */

#ifdef TEMPLATES
#include "thot_sys.h"
#include "tree.h"
#include "document.h"
#include "containers.h"
#include "insertelem_f.h"

#include "Template.h"
#include "templates.h"
#include "mydictionary_f.h"
#include "templateDeclarations_f.h"
#include "templateUtils_f.h"
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

DicDictionary Templates_Dic = NULL;
#endif /* TEMPLATES */

/*----------------------------------------------------------------------
  Creates a new template with its dictionaries
  ----------------------------------------------------------------------*/
XTigerTemplate NewXTigerTemplate (const char *templatePath, const ThotBool addPredefined)
{	
#ifdef TEMPLATES
	XTigerTemplate t = (XTigerTemplate)TtaGetMemory (sizeof (_XTigerTemplate));
	
  t->name = TtaStrdup(templatePath);
  t->version = NULL;
  t->templateVersion = NULL;
	t->isLibrary = FALSE;
	t->libraries = Dictionary_Create ();
	t->elements = Dictionary_Create ();
	t->simpleTypes	= Dictionary_Create ();
	t->components = Dictionary_Create ();
	t->unions = Dictionary_Create ();
	t->doc = -1;
  t->users = 0;
  t->isPredefined = FALSE;

	if (addPredefined)
		AddLibraryDeclarations (t,(XTigerTemplate)Dictionary_Get (Templates_Dic, PREDEFINED_LIB));	

	Dictionary_Add (Templates_Dic, templatePath, t);

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
  XTigerTemplate t;

	t = (XTigerTemplate)NewXTigerTemplate (templatePath, addPredefined);
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
  Declaration dec;

  if (name == NULL || t == NULL)
		return NULL;
	dec = (Declaration) TtaGetMemory (sizeof (_Declaration));
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
	AddDeclaration (t, dec);
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Returns a library with the predefined types
  ----------------------------------------------------------------------*/
XTigerTemplate CreatePredefinedTypesLibrary ()
{
#ifdef TEMPLATES
	XTigerTemplate lib = NewXTigerLibrary (PREDEFINED_LIB, FALSE);
	lib->isLibrary = true;

	NewSimpleType (lib, TYPE_NUMBER,  XTNumber);
	NewSimpleType (lib, TYPE_BOOLEAN, XTBoolean);
	NewSimpleType (lib, TYPE_STRING,  XTString);
	NewUnion (lib, UNION_ANYCOMPONENT, NULL, NULL);
	NewUnion (lib, UNION_ANYSIMPLE, NULL, NULL);
	NewUnion (lib, UNION_ANYELEMENT, NULL, NULL);
	NewUnion (lib, UNION_ANY, Dictionary_CreateFromList (UNION_ANY_DEFINITION), NULL);
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
	Templates_Dic = Dictionary_Create ();
	Dictionary_Add (Templates_Dic, PREDEFINED_LIB, CreatePredefinedTypesLibrary ());	
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Releasing the template environment
  ----------------------------------------------------------------------*/
void FreeTemplateEnvironment ()
{
#ifdef TEMPLATES
  XTigerTemplate t;
  if (Templates_Dic)
  {
    for (Dictionary_First (Templates_Dic);!Dictionary_IsDone (Templates_Dic);Dictionary_Next (Templates_Dic))
      {
        t = (XTigerTemplate)Dictionary_CurrentElement (Templates_Dic);
        TtaCloseDocument (t->doc);
        FreeXTigerTemplate(t);
      }
  }
      
#endif
}

/*----------------------------------------------------------------------
  Adds a new declaration or redefines an existing one
  ----------------------------------------------------------------------*/
void AddDeclaration (XTigerTemplate t, Declaration dec)
{
#ifdef TEMPLATES
  if (!t)
    return;

	Declaration old = Template_GetDeclaration (t, dec->name);
	if (old==NULL) //New type, not a redefinition
    {
      switch (dec->nature)
        {
        case SimpleTypeNat:
          Dictionary_Add (t->simpleTypes, dec->name, dec);
          break;
        case XmlElementNat:
          Dictionary_Add (t->elements, dec->name, dec);
          break;
        case ComponentNat:
          Dictionary_Add (t->components, dec->name, dec);
          break;
        case UnionNat:
          Dictionary_Add (t->unions, dec->name, dec);
          break;
        default:
          //Impossible
          break;
        }
    }
	else //A redefinition. Using the old memory zone to keep consistent pointers
    {
      //TODO CopyDeclarationInto (dec, old);
      TtaFreeMemory (dec);
    }
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void NewComponent (XTigerTemplate t, const char *name, const Element el)
{
#ifdef TEMPLATES
  if (!t)
    return;

	Declaration dec = NewDeclaration (t, name, ComponentNat);
  dec->componentType.content = TtaCopyTree (el, TtaGetDocument (el),
                                            TtaGetDocument (el), el);
	AddDeclaration (t, dec);
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void NewUnion (const XTigerTemplate t, const char *name,
               DicDictionary include, DicDictionary exclude)
{
#ifdef TEMPLATES
  if (!t)
    return;

	Declaration dec = NewDeclaration (t, name, UnionNat);
	Declaration aux;
	
	dec->unionType.include  = Dictionary_Create ();
	dec->unionType.exclude  = Dictionary_Create ();
  dec->unionType.expanded = NULL;

	//We initialize include
	if (include)
    {
      for (Dictionary_First (include); !Dictionary_IsDone (include); Dictionary_Next (include))
        {
          aux = Template_GetDeclaration (t, Dictionary_CurrentKey (include));
          if (aux == NULL) //Unknown type > a new XML element
            NewElement (t, Dictionary_CurrentKey (include));
          Dictionary_Add (dec->unionType.include, aux->name, aux);
        }
      Dictionary_Clean (include);
    }
	
	//We initialize exclude
	if (exclude)
    {
      for (Dictionary_First(exclude); !Dictionary_IsDone (exclude); Dictionary_Next (exclude))
        {
          aux = Template_GetDeclaration (t, Dictionary_CurrentKey (exclude));
          if (aux == NULL) //Unknown type > a new XML element
            NewElement (t, Dictionary_CurrentKey (exclude));
          Dictionary_Add (dec->unionType.exclude, aux->name, aux);
        }
      Dictionary_Clean (exclude);
    }

	AddDeclaration (t, dec);
#endif /* TEMPLATES */
}



/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void NewElement (const XTigerTemplate t, const char *name)
{
#ifdef TEMPLATES
  if (!t)
    return;

	Declaration dec = NewDeclaration (t, name, XmlElementNat);
  dec->elementType.name = TtaStrdup(name);
	AddDeclaration (t, dec);
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  FreeDeclaration
  Remove declaration from its template dictionary and free it.
  ----------------------------------------------------------------------*/
void FreeDeclaration (Declaration dec)
{
#ifdef TEMPLATES

  /* Free its content. */
  if (dec->nature==XmlElementNat)
  {
    TtaFreeMemory(dec->elementType.name);
    dec->elementType.name = NULL;
  }
  else if (dec->nature==ComponentNat)
  {
    TtaDeleteTree(dec->componentType.content, TtaGetDocument(dec->componentType.content));
    dec->componentType.content = NULL;
  }
  else if (dec->nature==UnionNat && !dec->declaredIn->isPredefined)
  {
    Dictionary_Clean(dec->unionType.include);
    dec->unionType.include = NULL;
    Dictionary_Clean(dec->unionType.exclude);
    dec->unionType.exclude = NULL;
    Dictionary_Clean(dec->unionType.expanded);
    dec->unionType.expanded = NULL;
    
  }
  
	TtaFreeMemory (dec->name);
  dec->name = NULL;
	TtaFreeMemory (dec);
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Template_GetDeclaration
  Find a declaration in a specified template and return it.
  @param t Template in which search the declaration
  @param name Declaration name to find.
  ----------------------------------------------------------------------*/
Declaration Template_GetDeclaration (const XTigerTemplate t, const char *name)
{
#ifdef TEMPLATES
  if (!t)
    return NULL;

	Declaration dec = (Declaration)Dictionary_Get (t->simpleTypes, name);	
	if (dec) return dec;
	dec = (Declaration)Dictionary_Get (t->components, name);
	if (dec) return dec;
	dec = (Declaration)Dictionary_Get (t->elements, name);
	if (dec) return dec;
	dec = (Declaration)Dictionary_Get (t->unions, name);
	return dec;
#else
	return NULL;
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Template_GetSimpleTypeDeclaration
  Find a declaration of a simple type in a specified template and return it.
  @param t Template in which search the declaration
  @param name Declaration name to find.
  ----------------------------------------------------------------------*/
Declaration Template_GetSimpleTypeDeclaration (const XTigerTemplate t, const char *name)
{
#ifdef TEMPLATES
  if(t)
     return (Declaration)Dictionary_Get (t->simpleTypes, name); 
  else
#endif /* TEMPLATES */
    return NULL;
}

/*----------------------------------------------------------------------
  Free all the space used by a template (also its dictionaries)
  ----------------------------------------------------------------------*/
void FreeXTigerTemplate (XTigerTemplate t)
{	
#ifdef TEMPLATES
	DicDictionary  dic;
	Declaration    dec;

  if (!t)
    return;

  //Cleaning the unions
  dic = t->unions;
  for (Dictionary_First (dic); !Dictionary_IsDone (dic); Dictionary_Next (dic))
    {
      dec = (Declaration)Dictionary_CurrentElement (dic);
      //Deleting only types defined by the template (not the imported ones)
      if (dec->declaredIn == t)
        FreeDeclaration(dec);
    }
  Dictionary_Clean (dic);


  /* Cleanning library dependancies. */
	dic = t->libraries;
	for (Dictionary_First (dic); !Dictionary_IsDone (dic); Dictionary_Next (dic))
    RemoveUser ((XTigerTemplate)Dictionary_CurrentElement (dic));
	Dictionary_Clean (dic);

	//Cleaning the components
	dic = t->components;
	for (Dictionary_First (dic); !Dictionary_IsDone (dic); Dictionary_Next (dic))
    {
      dec = (Declaration)Dictionary_CurrentElement (dic);
      //Deleting only types defined by the template (not the imported ones)
      if (dec->declaredIn == t)
        FreeDeclaration(dec);
    }
	Dictionary_Clean (dic);

	//Cleaning the elements
	dic = t->elements;
	for (Dictionary_First (dic); !Dictionary_IsDone (dic); Dictionary_Next (dic))
    {
      dec = (Declaration)Dictionary_CurrentElement (dic);
      //Deleting only types defined by the template (not the imported ones)
      if (dec->declaredIn == t)
        FreeDeclaration(dec);
    }
	Dictionary_Clean (dic);

	//Cleaning the simple types
	dic = t->simpleTypes;
	for (Dictionary_First (dic); !Dictionary_IsDone (dic); Dictionary_Next (dic))
    {
      dec = (Declaration)Dictionary_CurrentElement (dic);
      //Deleting only types defined by the template (not the imported ones)
      if (dec->declaredIn == t)
        FreeDeclaration(dec);
    }
	Dictionary_Clean (dic);

  //Freeing the document
  FreeDocumentResource (t->doc);
  TtcCloseDocument (t->doc, 0);

  //Removing the template of the dictionary
  Dictionary_RemoveElement (Templates_Dic, t);

  //Freeing the template
  TtaFreeMemory(t->name);
	TtaFreeMemory (t);
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Imports all declarations in a library lib to a template t
  ----------------------------------------------------------------------*/
void AddLibraryDeclarations (XTigerTemplate t, XTigerTemplate lib)
{	
#ifdef TEMPLATES
  if (!t)
    return;

	DicDictionary from = lib->elements;	
	DicDictionary to = t->elements;
	
	for (Dictionary_First (from); !Dictionary_IsDone (from); Dictionary_Next (from))
		Dictionary_Add (to, Dictionary_CurrentKey (from), Dictionary_CurrentElement (from));
	
	from = lib->components;	
	to = t->components;
	
	for (Dictionary_First (from); !Dictionary_IsDone (from); Dictionary_Next (from))
		Dictionary_Add (to, Dictionary_CurrentKey (from), Dictionary_CurrentElement (from));
	
	from = lib->unions;	
	to = t->unions;
	
	for (Dictionary_First (from); !Dictionary_IsDone (from); Dictionary_Next (from))
		Dictionary_Add (to, Dictionary_CurrentKey (from), Dictionary_CurrentElement (from));
	
	from = lib->simpleTypes;	
	to = t->simpleTypes;
	
	for (Dictionary_First (from); !Dictionary_IsDone (from); Dictionary_Next (from))
		Dictionary_Add (to, Dictionary_CurrentKey (from), Dictionary_CurrentElement (from));
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void PrintUnion (Declaration dec, int indent, XTigerTemplate t, FILE *file)
{	
#ifdef TEMPLATES
	DicDictionary  dic;
	Declaration    aux;
	char*		   indentation;
	int 		   i=0;
  
  if (!t)
    return;
	
	indentation = (char*) TtaGetMemory (indent*sizeof (char)+1);
	for (i = 0; i < indent; i++)
		indentation [i] = TAB;
	indentation [indent] = EOS;
	
	
	dic = dec->unionType.include;
	if (!Dictionary_IsEmpty (dic))
    {
      fprintf (file, "\n%sINCLUDE",indentation);
		
      for (Dictionary_First (dic);!Dictionary_IsDone (dic);Dictionary_Next (dic))
        {
          aux = (Declaration) Dictionary_CurrentElement (dic);
          switch (aux->nature)
            {
            case SimpleTypeNat:
            case XmlElementNat:
            case ComponentNat:
              fprintf (file, "\n%s+ %s ",indentation,aux->name);
              if (aux->declaredIn!=t)
                fprintf (file, "*");
              break;
            case UnionNat:
              fprintf (file, "\n%s+ %s ",indentation,aux->name);
              if (aux->declaredIn!=t)
                fprintf (file, "*");
              PrintUnion (aux, indent+1, t, file);
            default:
              //impossible
              break;
            }
        }
    }
	
	dic = dec->unionType.exclude;
	if (!Dictionary_IsEmpty (dic))
    {
      fprintf (file, "\n%sEXCLUDE",indentation);
		
      for (Dictionary_First (dic);!Dictionary_IsDone (dic);Dictionary_Next (dic))
        {
          aux = (Declaration) Dictionary_CurrentElement (dic);
          switch (aux->nature)
            {
            case SimpleTypeNat:
            case XmlElementNat:
            case ComponentNat:			
              fprintf (file, "\n%s- %s ",indentation,aux->name);
              if (aux->declaredIn!=t)
                fprintf (file, "*");
              break;
            case UnionNat:
              fprintf (file, "\n%s- %s ",indentation,aux->name);
              if (aux->declaredIn!=t)
                fprintf (file, "*");
              PrintUnion (aux, indent+1, t, file);
            default:
              //impossible
              break;
            }
        }
    }
	
	TtaFreeMemory (indentation);
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void PrintDeclarations (XTigerTemplate t, FILE *file)
{
#ifdef TEMPLATES
	DicDictionary  aux;
	Declaration    dec;
	
  if (!t)
    return;
  
	fprintf (file, "SIMPLE TYPES\n");
	fprintf (file, "------------");
	aux = t->simpleTypes;
	for (Dictionary_First (aux);!Dictionary_IsDone (aux);Dictionary_Next (aux))
    {
      dec = (Declaration) Dictionary_CurrentElement (aux);
      fprintf (file, "\n%s ",dec->name);
      if (dec->declaredIn!=t)
        fprintf (file, "*");
    }
	
	aux = t->elements;
	if (!Dictionary_IsEmpty (aux))
    {
      fprintf (file, "\n\nXML ELEMENTS\n");
      fprintf (file, "------------");	
      for (Dictionary_First (aux);!Dictionary_IsDone (aux);Dictionary_Next (aux))
        {
          dec = (Declaration) Dictionary_CurrentElement (aux);
          fprintf (file,"\n%s ",dec->name);
          if (dec->declaredIn!=t)
            fprintf (file, "*");
        }
    }
	
	aux = t->components;
	if (!Dictionary_IsEmpty (aux))
    {
      fprintf (file, "\n\nCOMPONENTS\n");
      fprintf (file, "----------");	
      for (Dictionary_First (aux);!Dictionary_IsDone (aux);Dictionary_Next (aux))
        {
          dec = (Declaration) Dictionary_CurrentElement (aux);
          fprintf (file,"\n%s ",dec->name);
          if (dec->declaredIn!=t)
            fprintf (file, "*");
          fprintf (file,"\n********************\n");
          TtaListAbstractTree (dec->componentType.content, file);
          fprintf (file,"********************\n");
        }
    }
	
	aux = t->unions;
	if (!Dictionary_IsEmpty (aux))
    {
      fprintf (file, "\n\nUNIONS\n");
      fprintf (file, "------");
      for (Dictionary_First (aux);!Dictionary_IsDone (aux);Dictionary_Next (aux))
        {
          dec = (Declaration) Dictionary_CurrentElement (aux);
          fprintf (file,"\n%s ",dec->name);
          if (dec->declaredIn!=t)
            fprintf (file, "*");
          PrintUnion (dec, 1, t, file);
        }
    }
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void DumpDeclarations (XTigerTemplate t)
{
#ifdef TEMPLATES
	char localname[MAX_LENGTH];
	FILE *file;

  if (!t)
    return;

  strcpy (localname, TempFileDirectory);
  strcat (localname, DIR_STR);
  strcat (localname, "templateDecl.debug");
  file = TtaWriteOpen (localname);
		
	PrintDeclarations(t, file);

	TtaWriteClose (file);
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  RedefineSpecialUnions: Redefines predefined unions: any, anySimple, etc 
  ----------------------------------------------------------------------*/
void RedefineSpecialUnions (XTigerTemplate t)
{
#ifdef TEMPLATES
  Declaration un;
  DicDictionary dic;

  if (!t)
    return;

  //We get the old definition to modify it 
  un = Template_GetDeclaration (t,UNION_ANYSIMPLE);
  //TODO: Check that it is actually the good declaration and not a newer one
  un->unionType.include = t->simpleTypes;
  un->unionType.exclude = Dictionary_Create ();
  
  un = Template_GetDeclaration (t,UNION_ANYCOMPONENT);
  //TODO: Check that it is actually the good declaration and not a newer one
  un->unionType.include = t->components;
  un->unionType.exclude = Dictionary_Create ();

  un = Template_GetDeclaration (t,UNION_ANYELEMENT);
  //TODO: Check that it is actually the good declaration and not a newer one
  un->unionType.include = t->elements;
  un->unionType.exclude = Dictionary_Create ();
  
  un = Template_GetDeclaration (t,UNION_ANY);
  //TODO: Check that it is actually the good declaration and not a newer one
  dic = t->unions;
  for (Dictionary_First (dic);!Dictionary_IsDone (dic);Dictionary_Next (dic))
    {
      Dictionary_Add (un->unionType.include, Dictionary_CurrentKey (dic), Dictionary_CurrentElement (dic));
    }

  //No recursive inclusion
  Dictionary_Remove (un->unionType.include, UNION_ANY);
  un->unionType.exclude = Dictionary_Create ();

#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
DicDictionary GetComponents (XTigerTemplate t)
{
#ifdef TEMPLATES
  if (t)
    return t->components;
  else
#endif /* TEMPLATES */
    return NULL;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
Element GetComponentContent (Declaration d)
{
#ifdef TEMPLATES
  if (d->nature == ComponentNat)
    return d->componentType.content;
  else
#endif /* TEMPLATES */
    return NULL;
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
Document GetTemplateDocument (XTigerTemplate t)
{
#ifdef TEMPLATES
  if (t)
    return t->doc;
  else
#endif /* TEMPLATES */
    return 0;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void SetTemplateDocument (XTigerTemplate t, Document doc)
{
#ifdef TEMPLATES
  if (t)
    t->doc = doc;
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void AddUser (XTigerTemplate t)
{
#ifdef TEMPLATES
  if (t)
    t->users++;
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void RemoveUser (XTigerTemplate t)
{
#ifdef TEMPLATES
  if (t)
  {
    t->users--;
    if (t->users == 0 && !t->isPredefined)
      FreeXTigerTemplate (t);
  }  
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Template_ExpandUnion
  Expand union definition if not already done.
  All included union are expanded and excluded elements are removed.
  @param t Template which embed the union
  @param decl Declaration of the union to expand.
  @return The expanded dict.
  ----------------------------------------------------------------------*/
DicDictionary Template_ExpandUnion(XTigerTemplate t, Declaration decl)
{
#ifdef TEMPLATES
  if (t)
  {
    if (decl->unionType.expanded==NULL)
    {
      DicDictionary  expanded = Dictionary_Create();
      Record rec;
      
      /* Insert all included descendants.*/
      rec = decl->unionType.include->first;
      while(rec)
      {
        Declaration child = (Declaration) rec->element;
        if (child->nature==UnionNat)
        {
          DicDictionary children = Template_ExpandUnion(t, child);
          Record recChildren = children->first;
          while(recChildren)
          {
            Declaration granchild = (Declaration) recChildren->element;
            if (!Dictionary_Get(expanded, granchild->name))
            {
              Dictionary_Add(expanded, granchild->name, (DicElement)granchild);
            }          
            recChildren = recChildren->next;
          }
          
        }
        else
        {
          if (!Dictionary_Get(expanded, child->name))
          {
            Dictionary_Add(expanded, child->name, (DicElement)child);
          }
        }
        rec = rec->next;
      }
      
      /* Remove all excluded descendants. */
      rec = decl->unionType.exclude->first;
      while(rec)
      {
        Declaration child = (Declaration) rec->element;
        if (child)
          Dictionary_Remove(expanded, child->name);
        rec = rec->next;
      }
      decl->unionType.expanded = expanded;
    }
    return decl->unionType.expanded;
  }
  else
#endif /* TEMPLATES */
    return NULL;
}

/*----------------------------------------------------------------------
  Template_ExpandTypes
  Expand a type list with resolving unions.
  @param t Template
  @param types String in which look for types.
  @return The resolved type string.
  ----------------------------------------------------------------------*/
char* Template_ExpandTypes (XTigerTemplate t, char* types)
{
#ifdef TEMPLATES
  if (t)
  {
    DicDictionary dico = Dictionary_Create();
    Record rec;
    Declaration   decl;
    int   len  = strlen(types);
    char* type = (char*)TtaGetMemory(len+1);
    int   pos = 0,
          cur = 0;
    char* result;
    int   resLen;
  
    /* Fill a dict with all finded declarations */
    while(pos<=len)
    {
      if (types[pos]==' ' || pos==len)
      {
        if (cur>0)
        {
          type[cur] = 0;
          decl = Template_GetDeclaration(t, type);
          if (decl)
          {
            if (decl->nature==UnionNat)
            {
              DicDictionary unionDecl = Template_ExpandUnion(t, decl);
              Record recChildren = unionDecl->first;
              while(recChildren)
              {
                if (!Dictionary_Get(dico, recChildren->key))
                {
                  Dictionary_Add(dico, recChildren->key, (DicElement) recChildren->element);
                }
                recChildren = recChildren->next;
              }
            }
            else
            {
              if (!Dictionary_Get(dico, type))
              {
                Dictionary_Add(dico, type, (DicElement) decl);
              }
            }
          }
        }
        cur = 0;
      }
      else
      {
        type[cur++] = types[pos];
      }
      pos++;
    }
    
    /* Fill a string with results.*/
    resLen = 0;
    rec = dico->first;
    while(rec)
    {
      resLen += strlen(((Declaration)rec->element)->name) + 1;
      rec = rec-> next;
    }
    result = (char*) TtaGetMemory(resLen+1);
    pos = 0;
    rec = dico->first;
    while(rec)
    {
      strcpy(result+pos, ((Declaration)rec->element)->name);
      pos += strlen(((Declaration)rec->element)->name);
      result[pos] = ' ';
      pos++;
      rec = rec-> next;
    }
    result[pos] = 0;
    
    Dictionary_Clean(dico);
    return result;
  }
  else
#endif /* TEMPLATES */
    return NULL;
}



/**----------------------------------------------------------------------
  Template_CanInsertElementInBag
  Test if an element can be insert in a bag
  ----------------------------------------------------------------------*/
ThotBool Template_CanInsertElementInBag (Document doc, ElementType type, char* bagTypes)
{
  ThotBool res = FALSE;
#ifdef TEMPLATES
  XTigerTemplate  t;
  char* types;
  char* elTypeName;
  
  if (TtaIsLeaf (type))
    // accept all basic elements ????
    return TRUE;
  t = (XTigerTemplate) Dictionary_Get (Templates_Dic, DocumentMeta[doc]->template_url);
  if (t)
  {
    types = Template_ExpandTypes(t, bagTypes);
    elTypeName = TtaGetElementTypeName(type);
    if (strstr(types, elTypeName))
      // Test for components or direct element types
      res = TRUE;
    else
      {
      }
    TtaFreeMemory(types);
  }
#endif /* TEMPLATES */
  return res;
}

/**----------------------------------------------------------------------
  Template_CanInsertElementInBagElement
  Test if an element can be insert in a bag
  ----------------------------------------------------------------------*/
ThotBool Template_CanInsertElementInBag (Document doc, ElementType type, Element bag)
{
  ThotBool res = FALSE;
#ifdef TEMPLATES
  XTigerTemplate  t;
  char* types, *bagTypes;
  char* elTypeName;
  
  t = (XTigerTemplate) Dictionary_Get (Templates_Dic, DocumentMeta[doc]->template_url);
  if (t && bag)
  {
    bagTypes = GetAttributeStringValueFromNum(bag, Template_ATTR_types, NULL);
    types = Template_ExpandTypes(t, bagTypes);
    elTypeName = TtaGetElementTypeName(type);
    if (strstr(types, elTypeName)) // Test for components or direct element types
      res = TRUE;
    else
    {
    }
    TtaFreeMemory(bagTypes);
    TtaFreeMemory(types);
  }
#endif /* TEMPLATES */
  return res;
}



/**----------------------------------------------------------------------
  Template_CanInsertElementInUse
  Test if an element can be insert in a use child element.
  @param type Type of element to insert.
  @param useType Type of use into which insert element.
  @param parent Parent of the new element.
  @param position Position where insert element.
  ----------------------------------------------------------------------*/
ThotBool Template_CanInsertElementInUse (Document doc, ElementType type, char* useType, Element parent, int position)
{
#ifdef TEMPLATES
  XTigerTemplate  t;
  Element         elem;
  t = (XTigerTemplate) Dictionary_Get (Templates_Dic, DocumentMeta[doc]->template_url);
  if (t && useType)
  {
    // Allow only simple type element.
    if(Template_GetSimpleTypeDeclaration(t, useType))
    {
      if(position==0)
        return TtaCanInsertFirstChild(type, parent, doc);
      else
      {
        for(elem = TtaGetFirstChild(parent); position>0 && elem; position--, TtaNextSibling(&elem));
        if(elem)
        {
          return TtaCanInsertSibling(type, elem, FALSE, doc);
        }
      }       
    }
  }
#endif /* TEMPLATES */
  return FALSE;
}

