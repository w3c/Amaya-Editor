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
#include "Elemlist.h"
#include "insertelem_f.h"

#include "Template.h"
#include "templates.h"
#include "templateDeclarations_f.h"
#include "templateUtils_f.h"
#include "HTMLactions_f.h"
#include "fetchHTMLname_f.h"
#include "html2thot_f.h"
#include "wxdialogapi_f.h"
#include "message.h"

#define UNION_ANY            "any"
#define UNION_ANYCOMPONENT   "anyComponent"
#define UNION_ANYSIMPLE      "anySimple"
#define UNION_ANYELEMENT     "anyElement"
#define UNION_ANY_DEFINITION "anyComponent anySimple anyElement"

#define UNION_APP_HTML       "appHTML"

#define HTML_LIBRARY          "-HTML-" 

char* XTigerHTMLUnions[] = 
{
  "appHTMLInfoTypes", "em strong cite dfn code var samp kbd abbr acronym ins del",
  "appHTMLCharTypes", "i b tt u s big small sub sup q bdo",
  "appHTMLForm", "input option optgroup select button label",
  "appHTMLInlines", "appHTMLInfoTypes appHTMLCharTypes appHTMLForm a font span img br object",
  "appHTMLBlocks", "p h1 h2 h3 h4 h5 h6 textarea ol ul li dd dl dt address fieldset legend ins del div",
  "appHTMLTable", "table thead tbody tfoot caption tr th td",
  UNION_APP_HTML , "appHTMLInlines appHTMLBlocks appHTMLTable",
  NULL, NULL
};


char* XTigerHTMLElements = "em strong cite dfn code var samp kbd abbr acronym ins del "
                    "i b tt u s big small sub sup q bdo "
                    "input option optgroup select button label "
                    "a font span img br object "
                    "p h1 h2 h3 h4 h5 h6 textarea ol ul li dd dl dt "
                    "address fieldset legend ins del div"
                    "table thead tbody tfoot caption tr th td";
                    


#define TYPE_NUMBER          "number"
#define TYPE_STRING          "string"
#define TYPE_BOOLEAN         "boolean"

//The predefined library id
#define PREDEFINED_LIB "-Predefined-"

HashMap Templates_Map = NULL;
#endif /* TEMPLATES */


/* Forward declaration : */
static void Template_Destroy (XTigerTemplate t);


/*----------------------------------------------------------------------
  Initializing the template environment
  ----------------------------------------------------------------------*/
void InitializeTemplateEnvironment ()
{
#ifdef TEMPLATES
  Templates_Map = StringHashMap_Create((Container_DestroyElementFunction) 
                                                    Template_Destroy, TRUE, -1);
  HashMap_Set (Templates_Map, (void*)PREDEFINED_LIB, CreatePredefinedTypesLibrary ());
  HashMap_Set (Templates_Map, (void*)HTML_LIBRARY, CreateHTMLLibrary ());
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Releasing the template environment
  ----------------------------------------------------------------------*/
void FreeTemplateEnvironment ()
{
#ifdef TEMPLATES
  if (Templates_Map)
    HashMap_Destroy(Templates_Map);
  Templates_Map = NULL;
#endif
}


/*----------------------------------------------------------------------
  Creates a new template with its dictionaries
  ----------------------------------------------------------------------*/
XTigerTemplate NewXTigerTemplate (const char *templatePath)
{
#ifdef TEMPLATES
  XTigerTemplate t = (XTigerTemplate)TtaGetMemory (sizeof (_XTigerTemplate));

  memset (t, 0 ,sizeof (_XTigerTemplate));
  t->name = TtaStrdup(templatePath);
  t->libraries   = StringHashMap_Create(NULL, FALSE, -1);
  t->elements    = KeywordHashMap_Create((Container_DestroyElementFunction)
                                                Declaration_Destroy, TRUE, -1);
  t->simpleTypes = KeywordHashMap_Create((Container_DestroyElementFunction)
                                                Declaration_Destroy, TRUE, -1);
  t->components  = KeywordHashMap_Create((Container_DestroyElementFunction)
                                                Declaration_Destroy, TRUE, -1);
  t->unions      = KeywordHashMap_Create((Container_DestroyElementFunction)
                                                Declaration_Destroy, TRUE, -1);
  t->unknowns    = KeywordHashMap_Create((Container_DestroyElementFunction)
                                                Declaration_Destroy, TRUE, -1);
  t->doc = -1;
  t->users = 0;
  t->isPredefined = FALSE;
  
  t->errorList = DLList_Create();
  t->errorList->destroyElement = (Container_DestroyElementFunction)TtaFreeMemory;

  HashMap_Set (Templates_Map, t->name, t);
  return t;
#else
  return NULL;
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Creates a new library with its dictionaries
  ----------------------------------------------------------------------*/
XTigerTemplate NewXTigerLibrary (const char *templatePath)
{	
#ifdef TEMPLATES
  XTigerTemplate t;
  t = (XTigerTemplate)NewXTigerTemplate (templatePath);
  t->isLibrary = TRUE;
  return t;
#else
  return NULL;
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Look for a XTiger library
  ----------------------------------------------------------------------*/
XTigerTemplate LookForXTigerLibrary (const char *templatePath)
{ 
#ifdef TEMPLATES
  XTigerTemplate t = NULL;
  
  t = (XTigerTemplate) HashMap_Get(Templates_Map, (void*)templatePath);
  if (!t)
  {
    t = NewXTigerLibrary(templatePath);
  }
  return t;
#else
  return NULL;
#endif /* TEMPLATES */
}


/*----------------------------------------------------------------------
  Look for a registered XTigerTemplate
  ----------------------------------------------------------------------*/
XTigerTemplate GetXTigerTemplate (const char *templatePath)
{
#ifdef TEMPLATES
  if (templatePath == NULL)
    return NULL;
  if (Templates_Map == NULL)
    InitializeTemplateEnvironment ();
  return (XTigerTemplate) HashMap_Get(Templates_Map, (void*)templatePath);
#else
  return NULL;
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Get the XTiger document template 
  ----------------------------------------------------------------------*/
XTigerTemplate GetXTigerDocTemplate (Document doc)
{
#ifdef TEMPLATES
  return GetXTigerTemplate(DocumentMeta[doc]->template_url);
#else
  return NULL;
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Template_AddStandardDependancies
  Load dependant standard libraries for a template.
  @param t Template.
  ----------------------------------------------------------------------*/
void Template_AddStandardDependancies(XTigerTemplate t)
{
#ifdef TEMPLATES
  if(t && !t->isLibrary)
  {
      if(DocumentTypes[t->doc]==docHTML)
      {
        Template_AddLibraryDeclarations (t,(XTigerTemplate)HashMap_Get(Templates_Map,
                                                          (void*)HTML_LIBRARY));  
      }
    Template_AddLibraryDeclarations (t,(XTigerTemplate)HashMap_Get(Templates_Map,
                                                      (void*)PREDEFINED_LIB));  
  }      

#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Returns a library with the predefined types
  ----------------------------------------------------------------------*/
XTigerTemplate CreatePredefinedTypesLibrary ()
{
#ifdef TEMPLATES
  XTigerTemplate lib = NewXTigerLibrary (PREDEFINED_LIB);
  lib->isLibrary = true;

  Template_DeclareNewSimpleType (lib, TYPE_NUMBER,  XTNumber);
  Template_DeclareNewSimpleType (lib, TYPE_BOOLEAN, XTBoolean);
  Template_DeclareNewSimpleType (lib, TYPE_STRING,  XTString);
  
  Template_DeclareNewUnion (lib, UNION_ANYCOMPONENT, NULL, NULL);
  Template_DeclareNewUnion (lib, UNION_ANYSIMPLE, NULL, NULL);
  Template_DeclareNewUnion (lib, UNION_ANYELEMENT, NULL, NULL);
  Template_DeclareNewUnion (lib, UNION_ANY, UNION_ANY_DEFINITION, NULL);
  
  lib->isPredefined = TRUE;
  return lib;
#else
  return NULL;
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Returns a library with HTML predefined types
  ----------------------------------------------------------------------*/
XTigerTemplate CreateHTMLLibrary ()
{
#ifdef TEMPLATES
  int              i;
  ForwardIterator  iter;
  HashMapNode      node;
  HashMap          map;
  XTigerTemplate lib = NewXTigerLibrary (HTML_LIBRARY);

  lib->isLibrary = true;
  lib->isPredefined = TRUE;

  // Add elements
  map = KeywordHashMap_CreateFromList(NULL, -1, XTigerHTMLElements);
  iter = HashMap_GetForwardIterator(map);
  ITERATOR_FOREACH(iter, HashMapNode, node)
    Template_DeclareNewElement(lib, (const char*) node->key);
  TtaFreeMemory(iter);
  HashMap_Destroy(map);
  
  // Add predefined unions
  i=0;
  while (XTigerHTMLUnions[i]!=NULL)
    {
      Template_DeclareNewUnion (lib, XTigerHTMLUnions[i], XTigerHTMLUnions[i+1], NULL);
      
      i+=2;
    }
  
  return lib;
#else
  return NULL;
#endif /* TEMPLATES */
}


/*----------------------------------------------------------------------
  Creates a new declaration. t and name must be not NULL or this function
  will return NULL
  \param t Template where is declared the declaration.
  \param name Name of the declaration.
  \param xtype Type of the declaration.
  ----------------------------------------------------------------------*/
static Declaration Declaration_Create(const XTigerTemplate t, const char *name,
                                   TypeNature xtype)
{
#ifdef TEMPLATES
  Declaration dec;

  if (name == NULL || t == NULL)
    return NULL;
  dec = (Declaration) TtaGetMemory (sizeof (_Declaration));
  memset (dec, 0, sizeof (_Declaration));
  dec->declaredIn = t;
  dec->usedIn     = t;
  dec->name = TtaStrdup (name);
  dec->nature = xtype;
  dec->blockLevel = FALSE;
  return dec;
#else
  return NULL;
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Clone a declaration.
  \note If the decl is an union, only union hashmap key are copied.
  ----------------------------------------------------------------------*/
Declaration Declaration_Clone (Declaration dec)
{
#ifdef TEMPLATES
  Declaration newdec = Declaration_Create (dec->declaredIn, dec->name, dec->nature);
//  Element         el;
  ForwardIterator iter;
  HashMapNode     node;
  
  switch (dec->nature)
    {
    case SimpleTypeNat:
      newdec->simpleType.type = dec->simpleType.type;
      break;
    case XmlElementNat:
      newdec->elementType.name = TtaStrdup(dec->elementType.name);
      break;
    case ComponentNat:
      newdec->componentType.content = NULL;
      break;
    case UnionNat:
      newdec->unionType.include = KeywordHashMap_CreateFromList(NULL, -1, NULL);
      newdec->unionType.exclude = KeywordHashMap_CreateFromList(NULL, -1, NULL);
      newdec->unionType.expanded = NULL;
      iter = HashMap_GetForwardIterator(dec->unionType.include);
      ITERATOR_FOREACH(iter, HashMapNode, node)
        {
          HashMap_Set(newdec->unionType.include, TtaStrdup((const char*)node->key), NULL);
        }
      TtaFreeMemory(iter);
      iter = HashMap_GetForwardIterator(dec->unionType.exclude);
      ITERATOR_FOREACH(iter, HashMapNode, node)
        {
          HashMap_Set(newdec->unionType.exclude, TtaStrdup((const char*)node->key), NULL);
        }
      TtaFreeMemory(iter);
      break;
    case UnknownNat:
      /* Do nothing. */
      break;
    }
  
  return newdec;
#else /* TEMPLATES */
  return NULL;
#endif /* TEMPLATES */
}


/*----------------------------------------------------------------------
  Declaration_Destroy
  Remove declaration from its template dictionary and free it.
  ----------------------------------------------------------------------*/
void Declaration_Destroy (Declaration dec)
{
#ifdef TEMPLATES
  Document doc;

  if (dec->nature == XmlElementNat)
  {
    TtaFreeMemory (dec->elementType.name);
    dec->elementType.name = NULL;
  }
  else if (dec->nature == ComponentNat)
  {
    doc = TtaGetDocument(dec->componentType.content);
    TtaDeleteTree (dec->componentType.content, doc);
    dec->componentType.content = NULL;
  }
  else if (dec->nature==UnionNat)
  {
    HashMap_Destroy (dec->unionType.include);
    dec->unionType.include = NULL;
    HashMap_Destroy (dec->unionType.exclude);
    dec->unionType.exclude = NULL;
    if (dec->unionType.expanded)
      {
        HashMap_Destroy (dec->unionType.expanded);
        dec->unionType.expanded = NULL;
      }
  }
  /* Do nothing for UnknownNat */
  
  TtaFreeMemory (dec->name);
  dec->name = NULL;
  TtaFreeMemory (dec);
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Declaration_GetElementType
  Intend to return the element type of the declaration.
  If dec is
    - element   : return the element type.
    - component : return the element type of the first top element.
    - union     : return nothing revelant {0,0}.
    - simple    : return nothing revelant {0,0}.
  ----------------------------------------------------------------------*/
ThotBool Declaration_GetElementType (Declaration dec, ElementType *type)
{
  type->ElSSchema = 0;
  type->ElTypeNum = 0;
#ifdef TEMPLATES
  if (dec)
    {
      if(dec->nature==XmlElementNat)
      {
        GIType (dec->name, type, dec->usedIn->doc);
        return TRUE;
      }
      else if(dec->nature==ComponentNat)
      {
        *type = TtaGetElementType(TtaGetFirstChild(dec->componentType.content));
        return TRUE;
      }
    }
#endif /* TEMPLATES */
  return FALSE;
}


/*----------------------------------------------------------------------
  Declaration_CalcBlockLevel
  Calculate if the declaration's element is block level or inlined.
  ----------------------------------------------------------------------*/
void Declaration_CalcBlockLevel (Declaration dec)
{
#ifdef TEMPLATES
  Element         elem;
  ElementType     type;
  HashMap         map;
  Declaration     unionDecl;
  ForwardIterator iter;
  HashMapNode     node;

  if (dec)
    {
      switch(dec->nature)
        {
        case SimpleTypeNat:
          /* Simple types are always inline.*/
          dec->blockLevel = FALSE;
          break;
        case XmlElementNat:
          /* XmlElement : test with html2thot::IsBlockElement.*/
          GIType (dec->name, &type, dec->usedIn->doc);
          dec->blockLevel = !IsCharacterLevelType (type);
          break;
        case ComponentNat:
          dec->blockLevel = FALSE;
          if (dec->componentType.content)
            {
              elem = TtaGetFirstChild(dec->componentType.content);                  
              while (elem)
                {
                  if (!IsCharacterLevelType(TtaGetElementType(elem)))
                    {
                      dec->blockLevel = TRUE;
                      break;
                    }
                  TtaNextSibling(&elem);
                }
            }
          break;
        case UnionNat:
          if (dec->name[0]=='a' && dec->name[1]=='n' && dec->name[2]=='y')
            {
              if (!strcmp(dec->name, UNION_ANY))
                {
                  dec->blockLevel = TRUE;
                  break;
                }
              else if (!strcmp(dec->name, UNION_ANYSIMPLE))
                {
                  dec->blockLevel = FALSE;
                  break;
                }
              else if (!strcmp(dec->name, UNION_ANYELEMENT))
                {
                  dec->blockLevel = TRUE;
                  break;
                }
              else if (!strcmp(dec->name, UNION_ANYCOMPONENT))
                {
                  iter = HashMap_GetForwardIterator(dec->usedIn->components);
                  ITERATOR_FOREACH(iter, HashMapNode, node)
                    {
                      unionDecl = (Declaration)node->elem;
                      if (!unionDecl)
                        unionDecl = Template_GetDeclaration(dec->usedIn, (char*)node->key);
                      if (unionDecl)
                        {
                          Declaration_CalcBlockLevel(unionDecl);
                          if (unionDecl->blockLevel)
                            {
                              dec->blockLevel = TRUE;
                              break;
                            }
                        }
                    }
                  TtaFreeMemory(iter);
                  break;
                }
            }
          map = Template_ExpandUnion(dec->usedIn, dec);
          iter = HashMap_GetForwardIterator(map);
          ITERATOR_FOREACH(iter, HashMapNode, node)
            {
              unionDecl = (Declaration)node->elem;
              if (!unionDecl)
                unionDecl = Template_GetDeclaration(dec->usedIn, (char*)node->key);
              if (unionDecl)
                {
                  Declaration_CalcBlockLevel(unionDecl);
                  if (unionDecl->blockLevel)
                    dec->blockLevel = TRUE;
                }
            }
          TtaFreeMemory(iter);
          break;
        case UnknownNat:
          break; /* Not used. */
        }
    }
#endif /* TEMPLATES */
}


/*----------------------------------------------------------------------
 * Calculate block-level for all declaration of a template. 
  ----------------------------------------------------------------------*/
void Template_CalcBlockLevel (XTigerTemplate t)
{
#ifdef TEMPLATES
  ForwardIterator iter;
  HashMapNode     node;
  Declaration     decl;
  ElementType     type;

  // Simple types.
  iter = HashMap_GetForwardIterator(t->simpleTypes);
  ITERATOR_FOREACH(iter, HashMapNode, node)
    {
      decl = (Declaration) node->elem;
      if (decl)
        decl->blockLevel = FALSE; // Always inline;
    }
  TtaFreeMemory(iter);

  // XML elements.
  iter = HashMap_GetForwardIterator(t->elements);
  ITERATOR_FOREACH(iter, HashMapNode, node)
    {
      decl = (Declaration) node->elem;
      if (decl)
        {
          GIType (decl->name, &type, t->doc);
          decl->blockLevel = !IsCharacterLevelType(type);
        }
    }
  TtaFreeMemory(iter);
  
  // Components
  iter = HashMap_GetForwardIterator(t->components);
  ITERATOR_FOREACH(iter, HashMapNode, node)
    {
      decl = (Declaration) node->elem;
      if (decl)
          Declaration_CalcBlockLevel(decl);
    }
  TtaFreeMemory(iter);

  // Unions
  iter = HashMap_GetForwardIterator(t->unions);
  ITERATOR_FOREACH(iter, HashMapNode, node)
    {
      decl = (Declaration) node->elem;
      if (decl)
          Declaration_CalcBlockLevel(decl);
    }
  TtaFreeMemory(iter);
  
#endif
}


/*----------------------------------------------------------------------
  Add a declaration to a template for a new simple type.
  ----------------------------------------------------------------------*/
Declaration Template_DeclareNewSimpleType (XTigerTemplate t, const char *name,
                                                          SimpleTypeType xtype)
{
#ifdef TEMPLATES
  Declaration dec = Declaration_Create (t, name, SimpleTypeNat);	
  dec->simpleType.type = xtype;
  Template_AddDeclaration(t, dec);
  return dec;
#else /* TEMPLATES */
  return NULL;
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Add a declaration to a template for a new component.
  ----------------------------------------------------------------------*/
Declaration Template_DeclareNewComponent (XTigerTemplate t, const char *name,
                                                           const Element el)
{
#ifdef TEMPLATES
  if (!t)
    return NULL;

  Declaration dec = Declaration_Create (t, name, ComponentNat);
  dec->componentType.content = TtaCopyTree (el, TtaGetDocument (el),
                                            TtaGetDocument (el), el);
  Template_AddDeclaration (t, dec);
  return dec;
#else /* TEMPLATES */
  return NULL;
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Add a declaration to a template for a new XML element.
  ----------------------------------------------------------------------*/
Declaration Template_DeclareNewElement (const XTigerTemplate t, const char *name)
{
#ifdef TEMPLATES
  if (!t)
    return NULL;

  Declaration dec = Declaration_Create (t, name, UnknownNat);
  dec->elementType.name = TtaStrdup(name);
  Template_AddDeclaration (t, dec);
  return dec;
#else /* TEMPLATES */
  return NULL;
#endif /* TEMPLATES */
}



/*----------------------------------------------------------------------
  Add a declaration to a template for a new union.
  \note The created union has just declaration names (hashmap keys),
  the declarations must be filled after all loading process.
  \param include Space-separated decl list string to include.  
  \param exclude Space-separated decl list string to exclude.  
  ----------------------------------------------------------------------*/
Declaration Template_DeclareNewUnion (XTigerTemplate t, const char *name,
                                      const char* include, const char* exclude)
{
#ifdef TEMPLATES
  if (!t)
    return NULL;

  Declaration dec = Declaration_Create (t, name, UnionNat);
  
  dec->unionType.include  = KeywordHashMap_CreateFromList(NULL, -1, include);
  dec->unionType.exclude  = KeywordHashMap_CreateFromList(NULL, -1, exclude);
  dec->unionType.expanded = NULL;

  Template_AddDeclaration (t, dec);
  return dec;
#else /* TEMPLATES */
  return NULL;
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Adds a new declaration or redefines an existing one
  ----------------------------------------------------------------------*/
void Template_AddDeclaration (XTigerTemplate t, Declaration dec)
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
          HashMap_Set (t->simpleTypes, dec->name, dec);
          break;
        case XmlElementNat:
          HashMap_Set (t->elements, dec->name, dec);
          break;
        case ComponentNat:
          HashMap_Set (t->components, dec->name, dec);
          break;
        case UnionNat:
          HashMap_Set (t->unions, dec->name, dec);
          break;
        default:
          HashMap_Set (t->unknowns, dec->name, dec);
          break;
        }
        dec->usedIn = t;
    }
  else //A redefinition. Using the old memory zone to keep consistent pointers
    {
      TtaFreeMemory (dec);
    }
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Remove unknown declaration.
  Usefull to replace it with a known declaration.
  ----------------------------------------------------------------------*/
void Template_RemoveUnknownDeclaration (XTigerTemplate t, Declaration dec)
{
#ifdef TEMPLATES
  if (!t || !dec || dec->nature!=UnknownNat)
    return;

  HashMap_DestroyElement (t->unknowns, dec->name);
#endif /* TEMPLATES */
}


/*----------------------------------------------------------------------
  Move unknown declarations to XmlElementNat
  ----------------------------------------------------------------------*/
void Template_MoveUnknownDeclarationToXmlElement (XTigerTemplate t)
{
#ifdef TEMPLATES
  ForwardIterator iter;
  HashMapNode     node;
  Declaration     decl, old;

  if (!t)
    return;

  iter = HashMap_GetForwardIterator(t->unknowns);
  ITERATOR_FOREACH(iter, HashMapNode, node)
    {
      old = (Declaration) node->elem;
      if (old)
        {
          decl = Declaration_Create (t, old->name, UnknownNat);
          decl->elementType.name = TtaStrdup(old->name);
          HashMap_Set (t->elements, decl->name, decl);
        }
    }
  TtaFreeMemory(iter);
  HashMap_Empty (t->unknowns);
#endif /* TEMPLATES */
}



/*----------------------------------------------------------------------
  Template_GetDeclaration
  Find a declaration in a specified template and return it.
  \param t Template in which search the declaration
  \param name Declaration name to find.
  ----------------------------------------------------------------------*/
Declaration Template_GetDeclaration (const XTigerTemplate t, const char *name)
{
#ifdef TEMPLATES
  if (!t)
    return NULL;

  Declaration dec = (Declaration)HashMap_Get (t->simpleTypes, (void*)name);	
  if (dec) return dec;
  dec = (Declaration)HashMap_Get (t->components, (void*)name);
  if (dec) return dec;
  dec = (Declaration)HashMap_Get (t->elements, (void*)name);
  if (dec) return dec;
  dec = (Declaration)HashMap_Get (t->unions, (void*)name);
  if (dec) return dec;
  dec = (Declaration)HashMap_Get (t->unknowns, (void*)name);
  return dec;
#else
  return NULL;
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Template_GetSimpleTypeDeclaration
  Find a declaration of a simple type in a specified template and return it.
  \param t Template in which search the declaration
  \param name Declaration name to find.
  ----------------------------------------------------------------------*/
Declaration Template_GetSimpleTypeDeclaration (const XTigerTemplate t, const char *name)
{
#ifdef TEMPLATES
  if (t)
     return (Declaration)HashMap_Get (t->simpleTypes, (void*)name); 
  else
#endif /* TEMPLATES */
    return NULL;
}

/*----------------------------------------------------------------------
  Template_GetComponentDeclaration
  Find a declaration of a component in a specified template and return it.
  \param t Template in which search the declaration
  \param name Declaration name to find.
  ----------------------------------------------------------------------*/
Declaration Template_GetComponentDeclaration (const XTigerTemplate t, const char *name)
{
#ifdef TEMPLATES
  if (t)
     return (Declaration)HashMap_Get (t->components, (void*)name); 
  else
#endif /* TEMPLATES */
    return NULL;
}

/*----------------------------------------------------------------------
  Template_GetElementDeclaration
  Find a declaration of an element in a specified template and return it.
  \param t Template in which search the declaration
  \param name Declaration name to find.
  ----------------------------------------------------------------------*/
Declaration Template_GetElementDeclaration (const XTigerTemplate t, const char *name)
{
#ifdef TEMPLATES
  if (t)
     return (Declaration)HashMap_Get (t->elements, (void*)name); 
  else
#endif /* TEMPLATES */
    return NULL;
}



/*----------------------------------------------------------------------
  Close a template and free it.
  Revove it from the global template map.
  ----------------------------------------------------------------------*/
void Template_Close(XTigerTemplate t)
{
#ifdef TEMPLATES
  if (t)
  {
    HashMap_DestroyElement(Templates_Map, t->name);
  }
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Free all the space used by a template
  ----------------------------------------------------------------------*/
static void Template_Destroy (XTigerTemplate t)
{	
#ifdef TEMPLATES
  if (!t)
    return;
    
  //Cleaning the unions
  HashMap_Destroy(t->unions);
  t->unions = NULL;

  /* Cleanning library dependancies. */
  HashMap_Destroy(t->libraries);
  t->libraries = NULL;

  //Cleaning the components
  HashMap_Destroy(t->components);
  t->components = NULL;

  //Cleaning the elements
  HashMap_Destroy(t->elements);
  t->elements = NULL;

  //Cleaning the simple types
  HashMap_Destroy(t->simpleTypes);
  t->simpleTypes = NULL;

  //Cleaning the unknown types
  HashMap_Destroy(t->unknowns);
  t->unknowns = NULL;

  //Clean error list
  DLList_Destroy(t->errorList);
  t->errorList = NULL;
  
  //Freeing the document
  if (t->doc>0)
    {
      FreeDocumentResource (t->doc);
      TtcCloseDocument (t->doc, 0);
    }

  //Freeing the template
  TtaFreeMemory(t->version);
  TtaFreeMemory(t->templateVersion);
  TtaFreeMemory(t->name);
  TtaFreeMemory (t);
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Fill declaration contents if not already done.
  Usefull when some declarations have been reported from libraries.
  ----------------------------------------------------------------------*/
static void Template_FillDeclarationContent(XTigerTemplate t, Declaration decl)
{
#ifdef TEMPLATES
  Element el;
  Declaration otherDecl;
  ForwardIterator  iter;
  HashMapNode      node;
  
  if (t && decl)
    {
      switch(decl->nature)
        {
        case ComponentNat:
            otherDecl = Template_GetComponentDeclaration(decl->declaredIn, decl->name);
            if (otherDecl && otherDecl->nature==ComponentNat)
              {
                el =  otherDecl->componentType.content;
                decl->componentType.content = TtaCopyTree (el, decl->declaredIn->doc,
                                              decl->usedIn->doc, NULL);
              }
          break;
        case UnionNat:
          iter = HashMap_GetForwardIterator(decl->unionType.include);
          ITERATOR_FOREACH(iter, HashMapNode, node)
            {
              node->elem = Template_GetDeclaration(decl->usedIn, (char*)node->key);
            }
          TtaFreeMemory(iter);
          iter = HashMap_GetForwardIterator(decl->unionType.exclude);
          ITERATOR_FOREACH(iter, HashMapNode, node)
            {
              node->elem = Template_GetDeclaration(decl->usedIn, (char*)node->key);
            }
          TtaFreeMemory(iter);
          iter = HashMap_GetForwardIterator(decl->unionType.exclude);
          ITERATOR_FOREACH(iter, HashMapNode, node)
            {
              node->elem = Template_GetDeclaration(decl->usedIn, (char*)node->key);
            }
          TtaFreeMemory(iter);
          break;
        default:
          break;
        }
    }
#endif /* TEMPLATES */  
}


/*----------------------------------------------------------------------
  Fill declaration contents if not already done.
  Usefull when some declarations have been reported from libraries.
  Only component and union must be filled.
  ----------------------------------------------------------------------*/
void Template_FillDeclarations(XTigerTemplate t)
{
#ifdef TEMPLATES
  ForwardIterator  iter;
  HashMapNode      node;
  
  if (t)
    {
      /* Fill components.*/
      iter = HashMap_GetForwardIterator(t->components);
      ITERATOR_FOREACH(iter, HashMapNode, node)
        {
          if (node->elem)
            Template_FillDeclarationContent(t, (Declaration)node->elem);
        }
      TtaFreeMemory(iter);

      /* Fill unions.*/
      iter = HashMap_GetForwardIterator(t->unions);
      ITERATOR_FOREACH(iter, HashMapNode, node)
        {
          if (node->elem)
            Template_FillDeclarationContent(t, (Declaration)node->elem);
        }
      TtaFreeMemory(iter);
    }
#endif /* TEMPLATES */
}


/*----------------------------------------------------------------------
  Copy elements of src to dst.
  If elements are override, they are destroyed.
  The src and dst ContainerElement must be ''Declaration''.
  ----------------------------------------------------------------------*/
static void CopyDeclarationHashMapElements(HashMap src, HashMap dst, XTigerTemplate t)
{
  ForwardIterator  iter;
  ContainerElement old;
  HashMapNode      node, newnode;
  Declaration      newdecl;
  
  iter = HashMap_GetForwardIterator(src);
  ITERATOR_FOREACH(iter, HashMapNode, node)
    {
      newdecl = Declaration_Clone((Declaration)node->elem);
      newdecl->usedIn = t;
      newnode = HashMap_Find(dst, node->key);
      if (newnode==NULL)
      {
        HashMap_Set(dst, TtaStrdup((const char*)node->key), newdecl);
      }
      else
      {
        old = newnode->elem;
        newnode->elem = newdecl;
        /** TODO Destroy old ''Declaration'' element. */
      }
    }
  TtaFreeMemory(iter);
}

/*----------------------------------------------------------------------
  Imports all declarations in a library lib to a template t
  ----------------------------------------------------------------------*/
void Template_AddLibraryDeclarations (XTigerTemplate t, XTigerTemplate lib)
{
#ifdef TEMPLATES
  if (!t || !lib)
    return;

  CopyDeclarationHashMapElements(lib->simpleTypes, t->simpleTypes, t);
  CopyDeclarationHashMapElements(lib->elements, t->elements, t);
  CopyDeclarationHashMapElements(lib->components, t->components, t);
  CopyDeclarationHashMapElements(lib->unions, t->unions, t);
  // Do not copy unknown elements

#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
 * Add an error to the template list error.
  ----------------------------------------------------------------------*/
void Template_AddError(XTigerTemplate t, const char* format, ...)
{
  va_list ap;
  va_start(ap, format);
  char buffer[MAX_LENGTH];
  
  if(t)
    {
#ifdef _WINDOWS
	  _vsnprintf (buffer, MAX_LENGTH, format, ap);
#else /* _WINDOWS */
      vsnprintf(buffer, MAX_LENGTH, format, ap);
#endif /* _WINDOWS */
      DLList_Append(t->errorList, TtaStrdup(buffer));
    }
  va_end(ap);
}

/*----------------------------------------------------------------------
 * Show template errors if any
  ----------------------------------------------------------------------*/
void Template_ShowErrors(XTigerTemplate t)
{
  if(t && !DLList_IsEmpty(t->errorList))
    {
      ShowNonSelListDlgWX(NULL, t->name,
          TtaGetMessage (AMAYA, AM_TEMPLATE_HAS_ERROR),
          TtaGetMessage (AMAYA, AM_CLOSE), t->errorList);
    }
}

/*----------------------------------------------------------------------
 * Test if template has error(s)
  ----------------------------------------------------------------------*/
ThotBool Template_HasErrors(XTigerTemplate t)
{
  return t==NULL || (t!=NULL && !DLList_IsEmpty(t->errorList));
}





/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void Template_PrintUnion (Declaration dec, int indent, XTigerTemplate t, FILE *file)
{	
#ifdef TEMPLATES
  ForwardIterator iter;
  HashMapNode     node;
  Declaration     aux;
  char*           indentation;
  int             i=0;
  
  if (!t || !dec || (dec && dec->nature!=UnionNat))
    return;

  indentation = (char*) TtaGetMemory (indent*sizeof (char)+1);
  for (i = 0; i < indent; i++)
    indentation [i] = TAB;
  indentation [indent] = EOS;

  if (!HashMap_IsEmpty (dec->unionType.include))
    {
      fprintf (file, "\n%sINCLUDE",indentation);

      iter = HashMap_GetForwardIterator (dec->unionType.include);
      ITERATOR_FOREACH (iter, HashMapNode, node)
        {
          aux = (Declaration) node->elem;
          if (aux==NULL)
            aux = Template_GetDeclaration(t, (const char*)node->key);
          if (aux!=NULL)
            {
              switch (aux->nature)
                {
                case SimpleTypeNat:
                case XmlElementNat:
                case ComponentNat:
                  fprintf (file, "\n%s+ %s ",indentation,aux->name);
                  if (aux->blockLevel)
                    fprintf (file, " block");
                  else
                    fprintf (file, " inline");
                  if (aux->declaredIn!=t)
                    fprintf (file, " (declared in %s)", aux->declaredIn->name);
                  break;
                case UnionNat:
                  fprintf (file, "\n%s+ %s ",indentation,aux->name);
                  if (aux->blockLevel)
                    fprintf (file, " block");
                  else
                    fprintf (file, " inline");
                  if (aux->declaredIn!=t)
                    fprintf (file, " (declared in %s)", aux->declaredIn->name);
                  Template_PrintUnion (aux, indent+1, t, file);
                default:
                  //impossible
                  break;
                }
            }
        }
      TtaFreeMemory(iter);
    }

  if (!HashMap_IsEmpty(dec->unionType.exclude))
    {
      fprintf (file, "\n%sEXCLUDE",indentation);
    
      iter = HashMap_GetForwardIterator(dec->unionType.exclude);
      ITERATOR_FOREACH(iter, HashMapNode, node)
        {
          aux = (Declaration) node->elem;
          if (aux==NULL)
            aux = Template_GetDeclaration(t, (const char*) node->key);
          if (aux!=NULL)
            {
            switch (aux->nature)
              {
              case SimpleTypeNat:
              case XmlElementNat:
              case ComponentNat:
                fprintf (file, "\n%s+ %s ",indentation,aux->name);
                if (aux->blockLevel)
                  fprintf (file, " block");
                else
                  fprintf (file, " inline");
                if (aux->declaredIn!=t)
                  fprintf (file, " (declared in %s)", aux->declaredIn->name);
                break;
              case UnionNat:
                fprintf (file, "\n%s+ %s ",indentation,aux->name);
                if (aux->blockLevel)
                  fprintf (file, " block");
                else
                  fprintf (file, " inline");
                if (aux->declaredIn!=t)
                  fprintf (file, " (declared in %s)", aux->declaredIn->name);
                Template_PrintUnion (aux, indent+1, t, file);
              default:
                //impossible
                break;
              }
            }
        }
      TtaFreeMemory(iter);
    }

  TtaFreeMemory (indentation);
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void FPrintElement(FILE *file, Element elem, int dec)
{
  int i;
  Element child;
  for(i=0; i<dec; i++)
    fprintf(file, "  ");
  fprintf(file, "%s\n", TtaGetElementTypeName(TtaGetElementType(elem)));
  child = TtaGetFirstChild(elem);
  while(child)
  {
    FPrintElement(file, child, dec+1);
    TtaNextSibling(&child);
  }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void PrintElement(Element elem, int dec)
{
  int i;
  Element child;
  for(i=0; i<dec; i++)
    printf("  ");
  printf("%s\n", TtaGetElementTypeName(TtaGetElementType(elem)));
  child = TtaGetFirstChild(elem);
  while(child)
  {
    PrintElement(child, dec+1);
    TtaNextSibling(&child);
  }
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void PrintDeclarations (XTigerTemplate t, FILE *file)
{
#ifdef TEMPLATES
  ForwardIterator iter;
  HashMapNode     node;
  Declaration     dec;

  if (!t)
    return;
  
  /* Simple types : */

  if (!HashMap_IsEmpty(t->simpleTypes))
    {
      fprintf (file, "\n\nSIMPLE TYPES\n");
      fprintf (file, "------------");
      iter = HashMap_GetForwardIterator(t->simpleTypes);
      ITERATOR_FOREACH(iter, HashMapNode, node)
        {
          dec = (Declaration) node->elem;
          fprintf (file, "\n(%p) %s ", dec, dec->name);
          if (dec->blockLevel)
            fprintf (file, " block");
          else
            fprintf (file, " inline");
          if (dec->declaredIn!=t)
            fprintf (file, " (declared in %s)", dec->declaredIn->name);
        }
      TtaFreeMemory(iter);  
    }

  /* XML elements : */
  if (!HashMap_IsEmpty(t->elements))
    {
      fprintf (file, "\n\nXML ELEMENTS\n");
      fprintf (file, "------------");
      iter = HashMap_GetForwardIterator(t->elements);
      ITERATOR_FOREACH(iter, HashMapNode, node)
        {
          dec = (Declaration) node->elem;
          fprintf (file, "\n(%p) %s ", dec, dec->name);
          if (dec->blockLevel)
            fprintf (file, " block");
          else
            fprintf (file, " inline");
          if (dec->declaredIn!=t)
            fprintf (file, " (declared in %s)", dec->declaredIn->name);
        }
      TtaFreeMemory(iter);  
    }

  /* Components : */
  if (!HashMap_IsEmpty(t->components))
    {
      fprintf (file, "\n\nCOMPONENTS\n");
      fprintf (file, "------------");
      iter = HashMap_GetForwardIterator(t->components);
      ITERATOR_FOREACH(iter, HashMapNode, node)
        {
          dec = (Declaration) node->elem;
          fprintf (file, "\n(%p) %s ", dec, dec->name);
          if (dec->blockLevel)
            fprintf (file, " block");
          else
            fprintf (file, " inline");
          if (dec->declaredIn!=t)
            fprintf (file, " (declared in %s)", dec->declaredIn->name);
          FPrintElement(file, dec->componentType.content, 1);
        }
      TtaFreeMemory(iter);  
    }

  /* Unions : */
  if (!HashMap_IsEmpty(t->unions))
    {
      fprintf (file, "\n\nUNIONS\n");
      fprintf (file, "------------");
      iter = HashMap_GetForwardIterator(t->unions);
      ITERATOR_FOREACH(iter, HashMapNode, node)
        {
          dec = (Declaration) node->elem;
          fprintf (file, "\n(%p) %s ", dec, dec->name);
          if (dec->blockLevel)
            fprintf (file, " block");
          else
            fprintf (file, " inline");
          if (dec->declaredIn!=t)
            fprintf (file, " (declared in %s)", dec->declaredIn->name);
          Template_PrintUnion (dec, 1, t, file);
        }
      TtaFreeMemory(iter);  
    }
  
  /* Unknowns : */
  if (!HashMap_IsEmpty(t->unknowns))
    {
      fprintf (file, "\n\nUNKNWONS\n");
      fprintf (file, "------------");
      iter = HashMap_GetForwardIterator(t->unknowns);
      ITERATOR_FOREACH(iter, HashMapNode, node)
        {
          dec = (Declaration) node->elem;
          fprintf (file, "\n(%p) %s ", dec, dec->name);
        }
      TtaFreeMemory(iter);  
    }
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void DumpAllDeclarations()
{
#ifdef TEMPLATES
  char localname[MAX_LENGTH];
  FILE *file;

  XTigerTemplate  t;
  ForwardIterator iter;
  HashMapNode     node;
  
  strcpy (localname, TempFileDirectory);
  strcat (localname, DIR_STR);
  strcat (localname, "templateAllDecl.debug");
  file = TtaWriteOpen (localname);

  iter = HashMap_GetForwardIterator(Templates_Map);
  ITERATOR_FOREACH(iter, HashMapNode, node)
    {
      t = (XTigerTemplate) node->elem;
      if (t)
      {
        fprintf(file, "################################################################################\n");
        fprintf(file, "## Template declaration for \"%s\" (%d) :\n", t->name, t->doc);
        fprintf(file, "################################################################################\n");
        PrintDeclarations(t, file);
        fprintf(file, "\n################################################################################\n");
        fprintf(file, "################################################################################\n\n");
      }
    }
  TtaFreeMemory(iter);
  
  TtaWriteClose (file);
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
  ----------------------------------------------------------------------*/
HashMap GetComponents (XTigerTemplate t)
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
      Template_Close (t);
  }  
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Template_ExpandUnion
  Expand union definition if not already done.
  TODO anyElement and anySimple are not expanded.
  All included union are expanded and excluded elements are removed.
  \param t Template which embed the union
  \param decl Declaration of the union to expand.
  \return The expanded dict.
  ----------------------------------------------------------------------*/
HashMap Template_ExpandUnion(XTigerTemplate t, Declaration decl)
{
#ifdef TEMPLATES
  Declaration child;
  if (t && decl && decl->nature==UnionNat)
  {
    if (decl->unionType.expanded == NULL)
    {
      ForwardIterator iter;
      HashMapNode     node;
      HashMap         expanded = KeywordHashMap_Create(NULL, FALSE, -1);
      
      if(!strcmp(decl->name, UNION_ANYELEMENT))
        {
          iter = HashMap_GetForwardIterator(t->elements);
          ITERATOR_FOREACH(iter, HashMapNode, node)
            HashMap_Set (expanded, TtaStrdup((char*)node->key), node->elem);
          TtaFreeMemory(iter);        
        }
      else
        {
          iter = HashMap_GetForwardIterator(decl->unionType.include);
          /* For each element in include map */
          ITERATOR_FOREACH(iter, HashMapNode, node)
            {
              /* Fill the decl if not already done.*/
              if (node->elem==NULL)
                node->elem = Template_GetDeclaration(t, (const char*)node->key);
              child = (Declaration) node->elem;
              if(child)
                {
                  /* If element is union, expand it and add content in expanded map.*/
                  if (child->nature==UnionNat)
                    {
                      HashMap         children  = Template_ExpandUnion(t, child);
                      ForwardIterator childIter = HashMap_GetForwardIterator(children);
                      HashMapNode     childNode;
                      ITERATOR_FOREACH(childIter, HashMapNode, childNode)
                        {
                          Declaration granchild = (Declaration) childNode->elem;
                          if (!HashMap_Get(expanded, granchild->name))
                            HashMap_Set(expanded, granchild->name, granchild);
                        }
                      TtaFreeMemory(childIter);
                    }
                  else
                    {
                      /* Add it to expanded map.*/
                      if (!HashMap_Get(expanded, child->name))
                        HashMap_Set(expanded, child->name, child);
                    }
                }
            }
          TtaFreeMemory(iter);
          
          /* Remove all excluded descendants. */
          iter = HashMap_GetForwardIterator(decl->unionType.exclude);
          /* For each element in exclude map */
          ITERATOR_FOREACH(iter, HashMapNode, node)
            {
              child = (Declaration) node->elem;      
              if (child)
                HashMap_Remove(expanded, child->name);
            }
          TtaFreeMemory(iter);
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
  Template_ExpandHashMapTypes
  Expand a type list with resolving unions.
  anySimple is not expanded.
  \param t Template
  \param types String in which look for types.
  \return The resolved type string.
  ----------------------------------------------------------------------*/
HashMap Template_ExpandHashMapTypes (XTigerTemplate t, HashMap types)
{
#ifdef TEMPLATES
  if (t)
  {
    /* Map to store expanded types. */
    HashMap     map     = KeywordHashMap_Create (NULL, TRUE, -1);
    ForwardIterator iter;
    HashMapNode     node;
    ForwardIterator iterbase;
    HashMapNode     nodebase;
    Declaration decl;

    /* Fill map with expanded result from basemap.*/
    iterbase = HashMap_GetForwardIterator (types);
    ITERATOR_FOREACH (iterbase, HashMapNode, nodebase)
      {
        decl = (Declaration) nodebase->elem;
        if (!decl)
          decl = Template_GetDeclaration (t, (char*) nodebase->key);
        if (decl)
          {
            if (decl->nature == UnionNat)
              {
                /* Expand a list element. */
                HashMap unionDecl = Template_ExpandUnion (t, decl);
                if (unionDecl)
                  {
                    iter = HashMap_GetForwardIterator (unionDecl);
                    ITERATOR_FOREACH (iter, HashMapNode, node)
                      {
                        /* For each expanded element, add it to the final map.*/
                        HashMap_Set(map, TtaStrdup((char*)node->key), node->elem);
                      }
                    TtaFreeMemory (iter);
                  }
              }
            else
              /* Add it without expansion.*/
              HashMap_Set (map, TtaStrdup(decl->name), decl);
          }
      }
    TtaFreeMemory(iterbase);
    return map;
  }
  else
#endif /* TEMPLATES */
    return NULL;
}

/*----------------------------------------------------------------------
  Template_FilterInsertableElement
  Remove all elements which cannot be inserted at the specified place.
  \param t Template
  \param map Map of types to filter.  
  \param refelem If not null, return only element which can
           be inserted to the refelem
  \param insertafter if true, test for insertion after the refelem else
           test for insert it as first child.
  \note The map parameter can be affected.
  ----------------------------------------------------------------------*/
void Template_FilterInsertableElement (XTigerTemplate t, HashMap map,
                                          Element refelem, ThotBool insertafter)
{
#ifdef TEMPLATES
  HashMap         newmap;
  ElementType     type;
  ForwardIterator iter;
  HashMapNode     node;
  Declaration     dec;
  ThotBool        res;

  if (t && map && refelem)
    {
      newmap = KeywordHashMap_Create(NULL, TRUE, -1);
      iter = HashMap_GetForwardIterator(map);
      ITERATOR_FOREACH(iter, HashMapNode, node)
        {
          if (!node->elem)
            node->elem = Template_GetDeclaration(t, (char*)node->key);
          if (node->elem)
            {
              dec = (Declaration) node->elem;
              if (Declaration_GetElementType(dec, &type))
                {
                  if (insertafter)
                    res = TtaCanInsertSibling(type, refelem, FALSE, t->doc);
                  else
                    res = TtaCanInsertFirstChild(type, refelem, t->doc);
                  if(res)
                    HashMap_Set(newmap, TtaStrdup((char*)node->key), node->elem);
                }
            }
        }
      TtaFreeMemory (iter);    
      HashMap_SwapContents(map, newmap);
      HashMap_Destroy(newmap);   
    }
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Template_SortDeclarationMap
  Sort a list of KeywordHashMap<Declaration> to be user friendly
  First components and then XmlElements and sorted alphabeticaly.
  ----------------------------------------------------------------------*/
static int Template_SortDeclarationMap(HashMapNode elem1 ,HashMapNode elem2)
{
#ifdef TEMPLATES
  Declaration dec1 = (Declaration)elem1->elem,
              dec2 = (Declaration)elem2->elem;
  if(dec1->nature == dec2->nature)
    return strcmp((char*)dec1->name,(char*)dec2->name);
  else
    return ((int)dec1->nature) - ((int)dec2->nature);
         
#else  /* TEMPLATES */
  return 0;
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Template_ExpandTypes
  Expand a type list with resolving unions.
  anySimple is not expanded.
  \param t Template
  \param types String in which look for types.
  \param refelem If not null, return only element which can
           be inserted to the refelem
  \param insertafter if true, test for insertion after the refelem else
           test for insert it as first child.  
  \return The resolved type string.
  ----------------------------------------------------------------------*/
char* Template_ExpandTypes (XTigerTemplate t, char* types,
                                          Element refelem, ThotBool insertafter)
{
#ifdef TEMPLATES
  if (t)
  {
    /* Map of types to expand. */
    HashMap         basemap = KeywordHashMap_CreateFromList (NULL, -1, types);
    /* Map to store expanded types. */
    HashMap         map     = NULL;
    DLList          list    = NULL;
    ForwardIterator iter;
    HashMapNode     node;
    DLListNode      listnode;
    int             pos = 0;
    char            result[MAX_LENGTH];

    /* Fill map with expanded result from basemap.*/
    map = Template_ExpandHashMapTypes(t, basemap);
    
    /* Fill a string with results.*/
    if (map)
      {
        if (refelem)
          Template_FilterInsertableElement(t, map, refelem, insertafter);

        list = DLList_Create(); // List to store HashMapNodes to sort them.
        iter = HashMap_GetForwardIterator(map);
        ITERATOR_FOREACH(iter, HashMapNode, node)
            DLList_Append(list, node);
        TtaFreeMemory (iter);
        // Sort the list.
        DLList_Sort(list, (Container_CompareFunction)Template_SortDeclarationMap);

        pos = 0;
        iter = DLList_GetForwardIterator(list);
        ITERATOR_FOREACH (iter, DLListNode, listnode)
          {
            node = (HashMapNode)listnode->elem;
            strcpy (result + pos, (char*)node->key);
            pos += strlen((char*) node->key);
            result[pos] = ' ';
            pos++;
          }

        TtaFreeMemory (iter);
        result[pos] = 0;
      }
    HashMap_Destroy (map);
    HashMap_Destroy (basemap);
    DLList_Destroy  (list);
    return TtaStrdup(result);
  }
  else
#endif /* TEMPLATES */
    return NULL;
}


/**----------------------------------------------------------------------
  Template_IsElementTypeAllowed
  Test if an element is allowed by a declaration.
  \param type Type of element to search.
  \param decl Declaration where search element type.
  \return TRUE if type is allowed and alse if not.
  ----------------------------------------------------------------------*/
ThotBool Template_IsElementTypeAllowed(ElementType type, Declaration decl)
{
#ifdef TEMPLATES
  ThotBool        can;
  ForwardIterator iter;
  HashMapNode     node;
  char            *name;

  if (decl)
  {
    switch(decl->nature)
    {
      case XmlElementNat:
        name = TtaGetElementTypeName(type);
        return !strcmp(name, decl->name);
      case UnionNat:
        if (!strcmp(decl->name, UNION_ANYELEMENT)) /* Allow all elements. */
          return TRUE;
        if (!strcmp(decl->name, UNION_ANY)) /* Allow all. */
          return TRUE;

        can = FALSE;
        
        iter = HashMap_GetForwardIterator(decl->unionType.include);
        ITERATOR_FOREACH(iter, HashMapNode, node)
            can = Template_IsElementTypeAllowed(type, (Declaration)node->elem);
        TtaFreeMemory(iter);

        if (can)
        {
          iter = HashMap_GetForwardIterator(decl->unionType.exclude);
          ITERATOR_FOREACH(iter, HashMapNode, node)
            {
              if (Template_IsElementTypeAllowed(type, (Declaration)node->elem))
              {
                can = FALSE;
                break;
              }
            }
          TtaFreeMemory(iter);
        }    
        return can;
      default:
        break;
    }
  }
#endif /* TEMPLATES */
  return FALSE;
}

/**----------------------------------------------------------------------
  Template_IsTypeAllowed
  Test if a type is allowed by a declaration.
  \param type Type name of element to search.
  \param decl Declaration where search element type.
  \return TRUE if type is allowed and alse if not.
  ----------------------------------------------------------------------*/
ThotBool Template_IsTypeAllowed(const char* type, Declaration decl)
{
#ifdef TEMPLATES
  ThotBool      can;
  Declaration   declType;
  ForwardIterator iter;
  HashMapNode     node;

  if (decl)
  {
    switch(decl->nature)
    {
      case UnionNat:
        if (!strcmp(decl->name, UNION_ANY)) /* Allow all. */
          return TRUE;

        if (!strcmp(decl->name, UNION_ANYELEMENT)) /* Allow all elements. */
        {
          declType = (Declaration)HashMap_Get (decl->declaredIn->elements, (void*)type);
          return declType!=NULL;
        }
        if (!strcmp(decl->name, UNION_ANYCOMPONENT)) /* Allow all components. */
        {
          declType = (Declaration)HashMap_Get (decl->declaredIn->components, (void*)type);
          return declType!=NULL;
        }
        if (!strcmp(decl->name, UNION_ANYSIMPLE)) /* Allow all components. */
        {
          declType = (Declaration)HashMap_Get (decl->declaredIn->simpleTypes, (void*)type);
          return declType!=NULL;
        }

        can = FALSE;

        iter = HashMap_GetForwardIterator(decl->unionType.include);
        ITERATOR_FOREACH(iter, HashMapNode, node)
          {
            can = Template_IsTypeAllowed(type, (Declaration)node->elem);
          }
        TtaFreeMemory(iter);
        
        if (can)
          {
            iter = HashMap_GetForwardIterator(decl->unionType.exclude);
            ITERATOR_FOREACH(iter, HashMapNode, node)
              {
                if (Template_IsTypeAllowed(type, (Declaration)node->elem))
                  {
                    can = FALSE;
                    break;
                  }
              }
            TtaFreeMemory(iter);
          }        
        return can;
      default:
        return !strcmp(type, decl->name);
    }
  }
#endif /* TEMPLATES */
  return FALSE;
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
  HashMap         map;
  ForwardIterator iter;
  HashMapNode     node;
  Declaration     decl;
  
  t = (XTigerTemplate) HashMap_Get (Templates_Map, DocumentMeta[doc]->template_url);
  if (t)
    {
      map = KeywordHashMap_CreateFromList(NULL, -1, bagTypes);
      iter = HashMap_GetForwardIterator(map);
      ITERATOR_FOREACH(iter, HashMapNode, node)
        {
          decl = Template_GetDeclaration(t, (const char*)node->key);
          if (Template_IsElementTypeAllowed(type, decl))
            {
              res = TRUE;
              break;
            }
        }
      TtaFreeMemory(iter);
      HashMap_Destroy(map);
    }
#endif /* TEMPLATES */
  return res;
}

/**----------------------------------------------------------------------
  Template_CanInsertTypeInBag
  Test if something  can be insert in a bag
  ----------------------------------------------------------------------*/
ThotBool Template_CanInsertTypeInBag (Document doc, const char* type, char* bagTypes)
{
  ThotBool res = FALSE;
#ifdef TEMPLATES
  XTigerTemplate  t;
  Declaration     decl;
  HashMap         map;
  ForwardIterator iter;
  HashMapNode     node;
    
  t = (XTigerTemplate) HashMap_Get (Templates_Map, DocumentMeta[doc]->template_url);
  if (t)
    {
      map = KeywordHashMap_CreateFromList(NULL, -1, bagTypes);
      iter = HashMap_GetForwardIterator(map);
      ITERATOR_FOREACH(iter, HashMapNode, node)
        {
          decl = Template_GetDeclaration(t, (const char*)node->key);
          if (Template_IsTypeAllowed(type, decl))
            {
              res = TRUE;
              break;
            }
        }
      TtaFreeMemory(iter);
      HashMap_Destroy(map);
    }
#endif /* TEMPLATES */
  return res;
}


/**----------------------------------------------------------------------
  Template_CanInsertElementInBagElement
  Test if an element can be insert in a bag
  ----------------------------------------------------------------------*/
ThotBool Template_CanInsertElementInBagElement (Document doc, ElementType type, Element bag)
{
  ThotBool res = FALSE;
#ifdef TEMPLATES
  XTigerTemplate  t;
  char *bagTypes;
  
  t = (XTigerTemplate) HashMap_Get (Templates_Map, DocumentMeta[doc]->template_url);
  if (t && bag)
  {
    bagTypes = GetAttributeStringValueFromNum(bag, Template_ATTR_types, NULL);
    res = Template_CanInsertElementInBag(doc, type, bagTypes);
    TtaFreeMemory(bagTypes);
  }
#endif /* TEMPLATES */
  return res;
}

/**----------------------------------------------------------------------
  Template_CanInsertTypeInBagElement
  Test if something can be insert in a bag
  ----------------------------------------------------------------------*/
ThotBool Template_CanInsertElementInBagElement (Document doc, const char* type, Element bag)
{
  ThotBool res = FALSE;
#ifdef TEMPLATES
  XTigerTemplate  t;
  char *bagTypes;
  
  t = (XTigerTemplate) HashMap_Get (Templates_Map, DocumentMeta[doc]->template_url);
  if (t && bag)
  {
    bagTypes = GetAttributeStringValueFromNum(bag, Template_ATTR_types, NULL);
    res = Template_CanInsertTypeInBag(doc, type, bagTypes);
    TtaFreeMemory(bagTypes);
  }
#endif /* TEMPLATES */
  return res;
}


/**----------------------------------------------------------------------
  Template_CanInsertElementInUse
  Test if an element can be insert in a use child element.
  \param type Type of element to insert.
  \param useType Type of use into which insert element.
  \param parent Parent of the new element.
  \param position Position where insert element.
  ----------------------------------------------------------------------*/
ThotBool Template_CanInsertElementInUse (Document doc, ElementType type, char* useType, Element parent, int position)
{
#ifdef TEMPLATES
  XTigerTemplate  t;
  Element         elem;
  t = (XTigerTemplate) HashMap_Get (Templates_Map, DocumentMeta[doc]->template_url);
  if (t && useType)
  {
    // Allow only simple type element.
    if (Template_GetSimpleTypeDeclaration(t, useType))
    {
      if (position==0)
        return TtaCanInsertFirstChild(type, parent, doc);
      else
      {
        for(elem = TtaGetFirstChild(parent); position>0 && elem; position--, TtaNextSibling(&elem));
        if (elem)
        {
          return TtaCanInsertSibling(type, elem, FALSE, doc);
        }
      }       
    }
  }
#endif /* TEMPLATES */
  return FALSE;
}

