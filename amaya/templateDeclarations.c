/*
 *
 *  COPYRIGHT INRIA and W3C, 1996-2008
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
#include "templates_f.h"
#include "message.h"

#define UNION_ANY            "any"
#define UNION_ANYCOMPONENT   "anyComponent"
#define UNION_ANYSIMPLE      "anySimple"
#define UNION_ANYELEMENT     "anyElement"
#define UNION_ANY_DEFINITION "anyComponent anySimple anyElement"

#define UNION_APP_HTML       "appHTML"

#define HTML_LIBRARY          "-HTML-" 

const char* XTigerHTMLUnions[] = 
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


const char* XTigerHTMLElements = "em strong cite dfn code var samp kbd abbr acronym ins del "
                    "i b tt u s big small sub sup q bdo "
                    "input option optgroup select button label "
                    "a font span img br object "
                    "p h1 h2 h3 h4 h5 h6 textarea ol ul li dd dl dt "
                    "address fieldset legend ins del div "
                    "table thead tbody tfoot caption tr th td";
                    


#define TYPE_NUMBER          "number"
#define TYPE_STRING          "string"
#define TYPE_BOOLEAN         "boolean"

//The predefined library id
#define PREDEFINED_LIB "-Predefined-"

HashMap Templates_Map = NULL;
#endif /* TEMPLATES */


/*----------------------------------------------------------------------
  Initializing the template environment
  ----------------------------------------------------------------------*/
void InitializeTemplateEnvironment ()
{
#ifdef TEMPLATES
  Templates_Map = StringHashMap_Create((Container_DestroyElementFunction) 
                                                    Template_Destroy, TRUE, -1);
  HashMap_Set (Templates_Map, (void*)TtaStrdup(PREDEFINED_LIB),
               CreatePredefinedTypesLibrary ());
  HashMap_Set (Templates_Map, (void*)TtaStrdup(HTML_LIBRARY), CreateHTMLLibrary ());
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
  t->uri = TtaStrdup (templatePath);
  t->libraries = StringHashMap_Create(NULL, FALSE, -1);

  t->simpleTypes = SearchSet_Create((Container_DestroyElementFunction)Declaration_Destroy,
                                    (Container_CompareFunction)Declaration_Compare,
                                    (Container_CompareFunction)Declaration_CompareToString);
  t->elements = SearchSet_Create((Container_DestroyElementFunction)Declaration_Destroy,
                                 (Container_CompareFunction)Declaration_Compare,
                                 (Container_CompareFunction)Declaration_CompareToString);
  t->components = SearchSet_Create((Container_DestroyElementFunction)Declaration_Destroy,
                                   (Container_CompareFunction)Declaration_Compare,
                                   (Container_CompareFunction)Declaration_CompareToString);
  t->unions = SearchSet_Create((Container_DestroyElementFunction)Declaration_Destroy,
                               (Container_CompareFunction)Declaration_Compare,
                               (Container_CompareFunction)Declaration_CompareToString);
  t->unknowns = SearchSet_Create((Container_DestroyElementFunction)Declaration_Destroy,
                                 (Container_CompareFunction)Declaration_Compare,
                                 (Container_CompareFunction)Declaration_CompareToString);
  t->doc = -1;
  t->ref = 0;
  t->state = 0;
  t->errorList = SList_Create();
  t->errorList->destroyElement = (Container_DestroyElementFunction)TtaFreeMemory;
  HashMap_Set (Templates_Map, TtaStrdup(t->uri), t);
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
  t->state |= templLibrary;
  return t;
#else
  return NULL;
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Look for a XTiger library
  Create it if not found.
  ----------------------------------------------------------------------*/
XTigerTemplate LookForXTigerLibrary (const char *templatePath)
{ 
#ifdef TEMPLATES
  XTigerTemplate t = NULL;
  
  if (Templates_Map == NULL)
    InitializeTemplateEnvironment ();
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
  Look for a XTiger template
  Create it if not found.
  ----------------------------------------------------------------------*/
XTigerTemplate LookForXTigerTemplate (const char *templatePath)
{ 
#ifdef TEMPLATES
  XTigerTemplate t = NULL;
  
  if (Templates_Map == NULL)
    InitializeTemplateEnvironment ();
  t = (XTigerTemplate) HashMap_Get(Templates_Map, (void*)templatePath);
  if (t == NULL)
    t = NewXTigerTemplate (templatePath);
  return t;
#else /* TEMPLATES */
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
  Get the XTiger document template or template instance
  ----------------------------------------------------------------------*/
XTigerTemplate GetXTigerDocTemplate (Document doc)
{
#ifdef TEMPLATES
  HashMapNode     node;
  ForwardIterator iter;
  XTigerTemplate  t = NULL, res = NULL;

  if (Templates_Map == NULL)
    InitializeTemplateEnvironment ();

  if (doc == 0)
    return NULL;
  
  iter = HashMap_GetForwardIterator(Templates_Map);
  ITERATOR_FOREACH(iter, HashMapNode, node)
    {
      t = (XTigerTemplate)node->elem;
      if (t && t->doc == doc)
        {
          res = t;
          break;
        }
    }
  TtaFreeMemory(iter);
  return res;
#else
  return NULL;
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Update the XTigerTemplatePath
  ----------------------------------------------------------------------*/
ThotBool NewXTigerTemplatePath (Document doc, const char *templatePath)
{
#ifdef TEMPLATES
  XTigerTemplate  t = NULL;
 
  t = GetXTigerDocTemplate (doc);
  if (t && templatePath)
    {
      HashMap_DestroyElement (Templates_Map, t->uri);
      TtaFreeMemory (t->uri);
      t->uri = TtaStrdup (templatePath);
      HashMap_Set (Templates_Map, TtaStrdup(t->uri), t);
      return TRUE;
    }
#endif /* TEMPLATES */
  return FALSE;
}

/*----------------------------------------------------------------------
  Close a XTiger template or template instance
  ----------------------------------------------------------------------*/
void Template_Close (XTigerTemplate t)
{
#ifdef TEMPLATES
  if (t && (t->uri || t->base_uri))
    {
	  if (Template_IsInstance (t))
        HashMap_DestroyElement (Templates_Map, t->uri);
	  else
	  {
        Template_Clear (t);
        TtaFreeMemory(t->uri);
        TtaFreeMemory (t->base_uri);
        TtaFreeMemory(t->version);
        TtaFreeMemory(t->templateVersion);
        t->uri = NULL;
        t->base_uri = NULL; // the uri was freed
        t->version = NULL;
        t->templateVersion = NULL;
        t->ref = 0;
	  }
    }
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
  if (t)
  {
	Template_AddLibraryDeclarations (t, (XTigerTemplate)HashMap_Get(Templates_Map,
                                   (void*)PREDEFINED_LIB));
	if (!Template_IsLibrary(t))
	  {
		if (DocumentTypes[t->doc] == docHTML)
			Template_AddLibraryDeclarations (t, (XTigerTemplate)HashMap_Get(Templates_Map,
															  (void*)HTML_LIBRARY));  
	  }
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
  lib->state |= templLibrary|templPredefined;

  Template_DeclareNewSimpleType (lib, TYPE_NUMBER,  XTNumber);
  Template_DeclareNewSimpleType (lib, TYPE_BOOLEAN, XTBoolean);
  Template_DeclareNewSimpleType (lib, TYPE_STRING,  XTString);
  
  Template_DeclareNewUnion (lib, UNION_ANYCOMPONENT, NULL, NULL);
  Template_DeclareNewUnion (lib, UNION_ANYSIMPLE, NULL, NULL);
  Template_DeclareNewUnion (lib, UNION_ANYELEMENT, NULL, NULL);
  Template_DeclareNewUnion (lib, UNION_ANY, UNION_ANY_DEFINITION, NULL);
  
  
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
  StringSetNode    node;
  StringSet        set;
  XTigerTemplate lib = NewXTigerLibrary (HTML_LIBRARY);

  lib->state |= templLibrary|templPredefined;

  // Add elements
  set  = StringSet_CreateFromString(XTigerHTMLElements, " ");
  iter = StringSet_GetForwardIterator(set);
  ITERATOR_FOREACH(iter, StringSetNode, node)
    Template_DeclareNewElement(lib, (const char*) node->elem);
  TtaFreeMemory(iter);
  StringSet_Destroy(set);
  
  // Add predefined unions
  i=0;
  while (XTigerHTMLUnions[i] != NULL)
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
  dec->blockLevel = 0;
  return dec;
#else
  return NULL;
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Clone a declaration.
  ----------------------------------------------------------------------*/
Declaration Declaration_Clone (Declaration dec)
{
#ifdef TEMPLATES
  Declaration newdec = Declaration_Create (dec->declaredIn, dec->name, dec->nature);
  newdec->blockLevel = dec->blockLevel;
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
      newdec->unionType.includeStr = TtaStrdup(dec->unionType.includeStr);
      newdec->unionType.excludeStr = TtaStrdup(dec->unionType.excludeStr);
      newdec->unionType.include  = NULL;
      newdec->unionType.exclude  = NULL;
      newdec->unionType.expanded = NULL;
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
  Element          el;

  if (dec->nature == XmlElementNat)
    {
      TtaFreeMemory (dec->elementType.name);
      dec->elementType.name = NULL;
    }
  else if (dec->nature == ComponentNat)
    {
      el = dec->componentType.content;
      TtaDeleteTree (el, 0);
      dec->componentType.content = NULL;
    }
  else if (dec->nature==UnionNat)
    {
    SearchSet_Destroy (dec->unionType.include);
    dec->unionType.include = NULL;
    SearchSet_Destroy (dec->unionType.exclude);
    dec->unionType.exclude = NULL;
    if (dec->unionType.expanded)
      {
        SearchSet_Destroy (dec->unionType.expanded);
        dec->unionType.expanded = NULL;
      }
    if (dec->unionType.includeStr)
      TtaFreeMemory(dec->unionType.includeStr);
    dec->unionType.includeStr = NULL;
    if (dec->unionType.excludeStr)
      TtaFreeMemory(dec->unionType.excludeStr);
    dec->unionType.excludeStr = NULL;
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
      if (dec->nature==XmlElementNat)
        {
        GIType (dec->name, type, dec->usedIn->doc);
        return TRUE;
        }
      else if (dec->nature==ComponentNat)
        {
        *type = TtaGetElementType(TtaGetFirstChild(dec->componentType.content));
        return TRUE;
        }
    }
#endif /* TEMPLATES */
  return FALSE;
}

/*----------------------------------------------------------------------
  Declaration_Compare
  Compare declarations to sort them (just compare declaration names)
  ----------------------------------------------------------------------*/
int Declaration_Compare (Declaration dec1, Declaration dec2)
{
#ifdef TEMPLATES
  if (dec1 && dec2)
    return strcmp(dec1->name, dec2->name);
  else if (dec1)
    return 1;
  else if (dec2)
    return -1;
  else
#endif /* TEMPLATES */
  return 0;
}

/*----------------------------------------------------------------------
  Declaration_CompareToString
  Compare declaration to a string to sort them (just compare declaration names)
  ----------------------------------------------------------------------*/
int Declaration_CompareToString (Declaration dec, const char* name)
{
#ifdef TEMPLATES
  if (dec && name)
    return strcmp(dec->name, name);
  else if (dec)
    return 1;
  else if (name)
    return -1;
  else
#endif /* TEMPLATES */
  return 0;
}


/*----------------------------------------------------------------------
  Declaration_CalcBlockLevel
  Calculate if the declaration's element is block level or inlined.
  ----------------------------------------------------------------------*/
void Declaration_CalcBlockLevel (XTigerTemplate t, Declaration dec)
{
#ifdef TEMPLATES
  Element         elem, child;
  ElementType     elType;
  SearchSet       set;
  Declaration     sub_dcl;
  ForwardIterator iter;
  SearchSetNode   node;
  char           *s, *name;

  if (dec && dec->blockLevel == 0)
    {
      //not already calculated
      switch (dec->nature)
        {
        case SimpleTypeNat:
          /* Simple types are always inline.*/
          dec->blockLevel = 1;
          break;
        case XmlElementNat:
          /* XmlElement : test with html2thot::IsBlockElement.*/
          GIType (dec->name, &elType, dec->usedIn->doc);
          if (TtaIsLeaf (elType))
            dec->blockLevel = 1;
          else if (IsCharacterLevelType (elType))
            dec->blockLevel = 1;
          else
            dec->blockLevel = 2;
          break;
        case ComponentNat:
          dec->blockLevel = 1;
          if (dec->componentType.content)
            {
              elem = TtaGetFirstChild (dec->componentType.content);                  
              while (elem)
                {
                  elType = TtaGetElementType (elem);
                  s = TtaGetSSchemaName (elType.ElSSchema);
                  child = elem;
                  while (s &&
                         (!strcmp (s, "Template") &&
                          elType.ElTypeNum == Template_EL_repeat) ||
                         (!strcmp (s, "HTML") &&
                          elType.ElTypeNum == HTML_EL_Pseudo_paragraph))
                    {
                      // ignore repeat and pseudo paragraphs
                      child =  TtaGetFirstChild (child);
                      elType = TtaGetElementType (child);
                      s = TtaGetSSchemaName (elType.ElSSchema);
                    }
                  if (TtaIsLeaf (elType))
                    dec->blockLevel = 1;
                  else if (!strcmp (s, "Template"))
                    {
                      // check the blocklevel of the component
                      name = GetUsedTypeName (elem);
                      sub_dcl = Template_GetDeclaration (t, name);
                      TtaFreeMemory(name);
                      if (sub_dcl && sub_dcl->blockLevel == 0)
                        Declaration_CalcBlockLevel (t, sub_dcl);
                      if (sub_dcl)
                        {
                          dec->blockLevel = sub_dcl->blockLevel;
                          if (dec->blockLevel == 2)
                            break;
                        }
                    }
                  else if (!IsCharacterLevelType (elType))
                    {
                      dec->blockLevel = 2;
                      break;
                    }
                  TtaNextSibling(&elem);
                }
            }
          break;
        case UnionNat:
          if (dec->name[0] == 'a' && dec->name[1] == 'n' && dec->name[2] == 'y')
            {
              if (!strcmp(dec->name, UNION_ANY))
                {
                  dec->blockLevel = 2;
                  break;
                }
              else if (!strcmp(dec->name, UNION_ANYSIMPLE))
                {
                  dec->blockLevel = 1;
                  break;
                }
              else if (!strcmp(dec->name, UNION_ANYELEMENT))
                {
                  dec->blockLevel = 2;
                  break;
                }
              else if (!strcmp(dec->name, UNION_ANYCOMPONENT))
                {
                  iter = SearchSet_GetForwardIterator(dec->usedIn->components);
                  ITERATOR_FOREACH(iter, SearchSetNode, node)
                    {
                      sub_dcl = (Declaration)node->elem;
                      if (sub_dcl && sub_dcl->blockLevel == 0)
                        Declaration_CalcBlockLevel (t, sub_dcl);
                      if (sub_dcl)
                        {
                          dec->blockLevel = sub_dcl->blockLevel;
                          if (sub_dcl->blockLevel == 2)
                            break;
                        }
                    }
                  TtaFreeMemory(iter);
                  break;
                }
            }
          set = Template_ExpandUnion (dec->usedIn, dec);
          iter = SearchSet_GetForwardIterator (set);
          ITERATOR_FOREACH (iter, SearchSetNode, node)
            {
              sub_dcl = (Declaration)node->elem;
              if (sub_dcl && sub_dcl->blockLevel == 0)
                Declaration_CalcBlockLevel (t, sub_dcl);
              if (sub_dcl)
                {
                  dec->blockLevel = sub_dcl->blockLevel;
                  if (sub_dcl->blockLevel == 2)
                    break;
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
  SearchSetNode   node;
  Declaration     dec;
  ElementType     elType;

  // Simple types.
  iter = SearchSet_GetForwardIterator(t->simpleTypes);
  ITERATOR_FOREACH(iter, SearchSetNode, node)
    {
      dec = (Declaration) node->elem;
      if (dec)
        dec->blockLevel = 1; // Always inline;
    }
  TtaFreeMemory(iter);

  // XML elements.
  iter = SearchSet_GetForwardIterator(t->elements);
  ITERATOR_FOREACH(iter, SearchSetNode, node)
    {
      dec = (Declaration) node->elem;
      if (dec)
        {
          GIType (dec->name, &elType, t->doc);
          if (TtaIsLeaf (elType))
            dec->blockLevel = 1;
          else if (IsCharacterLevelType (elType))
            dec->blockLevel = 1;
          else
            dec->blockLevel = 2;
        }
    }
  TtaFreeMemory(iter);
  
  // Components
  iter = SearchSet_GetForwardIterator(t->components);
  ITERATOR_FOREACH(iter, SearchSetNode, node)
    {
      dec = (Declaration) node->elem;
      if (dec)
          Declaration_CalcBlockLevel(t, dec);
    }
  TtaFreeMemory(iter);

  // Unions
  iter = SearchSet_GetForwardIterator(t->unions);
  ITERATOR_FOREACH(iter, SearchSetNode, node)
    {
      dec = (Declaration) node->elem;
      if (dec)
          Declaration_CalcBlockLevel(t, dec);
    }
  TtaFreeMemory(iter);
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Retrieve block-level declarations
  ----------------------------------------------------------------------*/
char* Template_GetBlockLevelDeclarations (XTigerTemplate t, ThotBool addAny)
{
#ifdef TEMPLATES
  ForwardIterator iter;
  SearchSetNode   node;
  Declaration     decl;
  char*           str;
  
  if(t)
    {
      str = (char*)TtaGetMemory(MAX_LENGTH);
      str[0] = EOS;
      
      if(addAny)
        {
          strcat(str, UNION_ANY);
          strcat(str, " ");
          strcat(str, UNION_ANYCOMPONENT);
          strcat(str, " ");
//          strcat(str, UNION_ANYSIMPLE);
//          strcat(str, " ");
          strcat(str, UNION_ANYELEMENT);
          strcat(str, " ");
        }
    
      // Components
      iter = SearchSet_GetForwardIterator(t->components);
      ITERATOR_FOREACH(iter, SearchSetNode, node)
        {
          decl = (Declaration) node->elem;
          if (decl && decl->blockLevel == 2)
            {
              strcat(str, decl->name);
              strcat(str, " ");
            }
        }
      TtaFreeMemory(iter);
      
      // Union
      iter = SearchSet_GetForwardIterator(t->unions);
      ITERATOR_FOREACH(iter, SearchSetNode, node)
        {
          decl = (Declaration) node->elem;
          if (decl && decl->blockLevel == 2)
            {
              if(strcmp(decl->name, UNION_ANY) && 
                  strcmp(decl->name, UNION_ANYCOMPONENT) &&
                  strcmp(decl->name, UNION_ANYELEMENT) )
                {
                  strcat(str, decl->name);
                  strcat(str, " ");
                }
            }
        }
      TtaFreeMemory(iter);      

      // XML elements
      iter = SearchSet_GetForwardIterator(t->elements);
      ITERATOR_FOREACH(iter, SearchSetNode, node)
        {
          decl = (Declaration) node->elem;
          if (decl && decl->blockLevel == 2)
            {
              strcat(str, decl->name);
              strcat(str, " ");
            }
        }
      TtaFreeMemory(iter);      
      
      return str;
    }
#endif /* TEMPLATES */
  return NULL;
}

/*----------------------------------------------------------------------
  Retrieve inline-level declarations
  ----------------------------------------------------------------------*/
char* Template_GetInlineLevelDeclarations (XTigerTemplate t, ThotBool addAny,
                                           ThotBool addSimple)
{
#ifdef TEMPLATES
  ForwardIterator iter;
  SearchSetNode   node;
  Declaration     decl;
  char*           str;
  
  if(t)
    {
      str = (char*)TtaGetMemory(MAX_LENGTH);
      str[0] = EOS;
      
      if(addAny)
        {
          strcat(str, UNION_ANY);
          strcat(str, " ");
          strcat(str, UNION_ANYCOMPONENT);
          strcat(str, " ");
          strcat(str, UNION_ANYSIMPLE);
          strcat(str, " ");
          strcat(str, UNION_ANYELEMENT);
          strcat(str, " ");
        }

      if(addSimple)
        {
          strcat(str, TYPE_STRING);
          strcat(str, " ");
        }
      
      // Components
      iter = SearchSet_GetForwardIterator(t->components);
      ITERATOR_FOREACH(iter, SearchSetNode, node)
        {
          decl = (Declaration) node->elem;
          if (decl && decl->blockLevel == 1)
            {
              strcat(str, decl->name);
              strcat(str, " ");
            }
        }
      TtaFreeMemory(iter);
      
      // Union
      iter = SearchSet_GetForwardIterator(t->unions);
      ITERATOR_FOREACH(iter, SearchSetNode, node)
        {
          decl = (Declaration) node->elem;
          if (decl && decl->blockLevel == 1)
            {
              if (strcmp(decl->name, UNION_ANY) && 
                  strcmp(decl->name, UNION_ANYCOMPONENT) &&
                  strcmp(decl->name, UNION_ANYSIMPLE) &&
                  strcmp(decl->name, UNION_ANYELEMENT) )
                {
                  strcat(str, decl->name);
                  strcat(str, " ");
                }
            }
        }
      TtaFreeMemory(iter);      

      // XML elements
      iter = SearchSet_GetForwardIterator(t->elements);
      ITERATOR_FOREACH(iter, SearchSetNode, node)
        {
          decl = (Declaration) node->elem;
          if (decl && decl->blockLevel == 1)
            {
              strcat(str, decl->name);
              strcat(str, " ");
            }
        }
      TtaFreeMemory(iter);      
      
      return str;
    }
#endif /* TEMPLATES */
  return NULL;  
}

/*----------------------------------------------------------------------
  Retrieve all declarations
  ----------------------------------------------------------------------*/
char *Template_GetAllDeclarations (XTigerTemplate t, ThotBool addAnys,
                                   ThotBool addAny, ThotBool addSimple)
{
#ifdef TEMPLATES
  ForwardIterator iter;
  SearchSetNode   node;
  Declaration     decl;
  char*           str;
  
  if(t)
    {
      str = (char*)TtaGetMemory(MAX_LENGTH);
      str[0] = EOS;
      
      if(addAnys)
        {
          if(addAny)
            {
              strcat(str, UNION_ANY);
              strcat(str, " ");
            }
          strcat(str, UNION_ANYCOMPONENT);
          strcat(str, " ");
          strcat(str, UNION_ANYSIMPLE);
          strcat(str, " ");
          strcat(str, UNION_ANYELEMENT);
          strcat(str, " ");
        }

      if(addSimple)
        {
          strcat(str, TYPE_STRING);
          strcat(str, " ");
        }
      
      // Components
      iter = SearchSet_GetForwardIterator(t->components);
      ITERATOR_FOREACH(iter, SearchSetNode, node)
        {
          decl = (Declaration) node->elem;
          strcat(str, decl->name);
          strcat(str, " ");
        }
      TtaFreeMemory(iter);
      
      // Union
      iter = SearchSet_GetForwardIterator(t->unions);
      ITERATOR_FOREACH(iter, SearchSetNode, node)
        {
          decl = (Declaration) node->elem;
          if(strcmp(decl->name, UNION_ANY) && 
              strcmp(decl->name, UNION_ANYCOMPONENT) &&
              strcmp(decl->name, UNION_ANYSIMPLE) &&
              strcmp(decl->name, UNION_ANYELEMENT) )
            {
              strcat(str, decl->name);
              strcat(str, " ");
            }
        }
      TtaFreeMemory(iter);      

      // XML elements
      iter = SearchSet_GetForwardIterator(t->elements);
      ITERATOR_FOREACH(iter, SearchSetNode, node)
        {
          decl = (Declaration) node->elem;
          strcat(str, decl->name);
          strcat(str, " ");
        }
      TtaFreeMemory(iter);      
      
      return str;
    }
#endif /* TEMPLATES */
  return NULL;  
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
  The parameter level gives the default level.
  ----------------------------------------------------------------------*/
Declaration Template_DeclareNewComponent (XTigerTemplate t, const char *name,
                                          Element el, int level)
{
#ifdef TEMPLATES
  if (!t)
    return NULL;

  Declaration dec = Declaration_Create (t, name, ComponentNat);
  dec->componentType.content = el;
  dec->blockLevel = level;
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

  Declaration dec = Declaration_Create (t, name, XmlElementNat);
  dec->elementType.name = TtaStrdup(name);
  Template_AddDeclaration (t, dec);
  return dec;
#else /* TEMPLATES */
  return NULL;
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Add a declaration to a template for a new XML element.
  ----------------------------------------------------------------------*/
Declaration Template_DeclareNewUnknown (const XTigerTemplate t, const char *name)
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
  
  dec->unionType.includeStr = TtaStrdup(include);
  dec->unionType.excludeStr = TtaStrdup(exclude);
  
  //SearchSet_Create(NULL, (Container_CompareFunction)Declaration_Compare,
  //                       (Container_CompareFunction)Declaration_CompareToString);
  dec->unionType.include  = NULL;// KeywordHashMap_CreateFromList(NULL, -1, include);
  dec->unionType.exclude  = NULL;// KeywordHashMap_CreateFromList(NULL, -1, exclude);
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
  if (old == NULL) //New type, not a redefinition
    {
      switch (dec->nature)
        {
        case SimpleTypeNat:
          SearchSet_Insert(t->simpleTypes, dec);
          break;
        case XmlElementNat:
          SearchSet_Insert (t->elements, dec);
          break;
        case ComponentNat:
          SearchSet_Insert (t->components, dec);
          break;
        case UnionNat:
          SearchSet_Insert (t->unions, dec);
          break;
        default:
          SearchSet_Insert (t->unknowns, dec);
          break;
        }
        dec->usedIn = t;
    }
  else //A redefinition. Using the old memory zone to keep consistent pointers
    TtaFreeMemory (dec);
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Remove unknown declaration.
  Usefull to replace it with a known declaration.
  ----------------------------------------------------------------------*/
void Template_RemoveUnknownDeclaration (XTigerTemplate t, Declaration dec)
{
#ifdef TEMPLATES
  if (!t || !dec || dec->nature != UnknownNat)
    return;
  SearchSet_DestroyElement (t->unknowns, SearchSet_Find(t->unknowns, dec));
#endif /* TEMPLATES */
}


/*----------------------------------------------------------------------
  Move unknown declarations to XmlElementNat
  ----------------------------------------------------------------------*/
void Template_MoveUnknownDeclarationToXmlElement (XTigerTemplate t)
{
#ifdef TEMPLATES
  ForwardIterator iter;
  SearchSetNode   node;
  Declaration     decl, old;

  if (!t)
    return;

  iter = SearchSet_GetForwardIterator(t->unknowns);
  ITERATOR_FOREACH(iter, SearchSetNode, node)
    {
      old = (Declaration) node->elem;
      if (old)
        {
          decl = Declaration_Create (t, old->name, XmlElementNat);
          decl->elementType.name = TtaStrdup(old->name);
          SearchSet_Insert(t->elements,  decl);
        }
    }
  TtaFreeMemory(iter);
  SearchSet_Empty (t->unknowns);
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

  Declaration dec = (Declaration)SearchSet_SearchElement (t->simpleTypes, (void*)name, NULL);	
  if (dec) return dec;
  dec = (Declaration)SearchSet_SearchElement (t->components, (void*)name, NULL);
  if (dec) return dec;
  dec = (Declaration)SearchSet_SearchElement (t->elements, (void*)name, NULL);
  if (dec) return dec;
  dec = (Declaration)SearchSet_SearchElement (t->unions, (void*)name, NULL);
  if (dec) return dec;
  dec = (Declaration)SearchSet_SearchElement (t->unknowns, (void*)name, NULL);
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
     return (Declaration)SearchSet_SearchElement (t->simpleTypes, (void*)name, NULL); 
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
     return (Declaration)SearchSet_SearchElement (t->components, (void*)name, NULL); 
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
     return (Declaration)SearchSet_SearchElement (t->elements, (void*)name, NULL); 
  else
#endif /* TEMPLATES */
    return NULL;
}

/*----------------------------------------------------------------------
  Template_GetUnionDeclaration
  Find a declaration of an union in a specified template and return it.
  \param t Template in which search the declaration
  \param name Declaration name to find.
  ----------------------------------------------------------------------*/
Declaration Template_GetUnionDeclaration (const XTigerTemplate t, const char *name)
{
#ifdef TEMPLATES
  if (t)
     return (Declaration)SearchSet_SearchElement (t->unions, (void*)name, NULL); 
  else
#endif /* TEMPLATES */
    return NULL;
}


/*----------------------------------------------------------------------
  Re-initialize the XTigerTemplate structure
  ----------------------------------------------------------------------*/
void Template_Clear (XTigerTemplate t)
{
#ifdef TEMPLATES
  if (t)
    {
      SearchSet_Empty(t->unions);
      SearchSet_Empty(t->components);
      SearchSet_Empty(t->elements);
      SearchSet_Empty(t->simpleTypes);
      SearchSet_Empty(t->unknowns);
    }
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Free all the space used by a template
  ----------------------------------------------------------------------*/
void Template_Destroy (XTigerTemplate t)
{	
#ifdef TEMPLATES
  ForwardIterator iter;
  HashMapNode     node;
  if (t)
    {
      if (t->base_uri)
        {
          Template_RemoveReference (GetXTigerTemplate(t->base_uri));
          TtaFreeMemory (t->base_uri);
          t->base_uri = NULL;
        }
      /* Cleanning library dependancies. */
      iter = HashMap_GetForwardIterator(t->libraries);
      ITERATOR_FOREACH(iter, HashMapNode, node)
        {
          Template_RemoveReference((XTigerTemplate)node->elem);
        }
      HashMap_Destroy(t->libraries);
      t->libraries = NULL;

      //Cleaning the unions
      SearchSet_Destroy(t->unions);
      t->unions = NULL;
    
      //Cleaning the components
      SearchSet_Destroy(t->components);
      t->components = NULL;
    
      //Cleaning the elements
      SearchSet_Destroy(t->elements);
      t->elements = NULL;
    
      //Cleaning the simple types
      SearchSet_Destroy(t->simpleTypes);
      t->simpleTypes = NULL;
    
      //Cleaning the unknown types
      SearchSet_Destroy(t->unknowns);
      t->unknowns = NULL;
    
      //Clean error list
      SList_Destroy(t->errorList);
      t->errorList = NULL;
    
      //Freeing the document
      if (t->doc > 0)
        {
          TtaRemoveDocumentReference(t->doc);
          t->doc = 0;
        }
    
      //Freeing the template
      TtaFreeMemory(t->uri);
      t->uri = NULL;
      TtaFreeMemory(t->version);
      t->version = NULL;
      TtaFreeMemory(t->templateVersion);
      t->templateVersion = NULL;
      TtaFreeMemory (t);
    }
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Fill declaration contents if not already done.
  Usefull when some declarations have been reported from libraries.
  ----------------------------------------------------------------------*/
static void Template_FillDeclarationContent(XTigerTemplate t, Declaration decl)
{
#ifdef TEMPLATES
  Element          el;
  Declaration      otherDecl;
  ForwardIterator  iter;
//  SearchSetNode    node;
  StringSet        set;
  StringSetNode    node;
  
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
          set =  StringSet_CreateFromString(decl->unionType.includeStr, " ");
          iter = StringSet_GetForwardIterator(set);
          
          decl->unionType.include = SearchSet_Create(NULL, (Container_CompareFunction)Declaration_Compare,
            (Container_CompareFunction)Declaration_CompareToString);
              
          ITERATOR_FOREACH(iter, StringSetNode, node)
            {
              SearchSet_Insert(decl->unionType.include, Template_GetDeclaration(decl->usedIn, (char*)((Declaration)node)->name));
            }
          TtaFreeMemory(iter);
          StringSet_Destroy(set);
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
  SearchSetNode      node;
  
  if (t)
    {
      /* Fill components.*/
      iter = SearchSet_GetForwardIterator(t->components);
      ITERATOR_FOREACH(iter, SearchSetNode, node)
        {
          if (node->elem)
            Template_FillDeclarationContent(t, (Declaration)node->elem);
        }
      TtaFreeMemory(iter);

      /* Fill unions.*/
      iter = SearchSet_GetForwardIterator(t->unions);
      ITERATOR_FOREACH(iter, SearchSetNode, node)
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
static void CopyDeclarationSetElements(SearchSet src, SearchSet dst, XTigerTemplate t)
{
  ForwardIterator  iter;
  ContainerElement old;
  SearchSetNode    node, newnode;
  Declaration      newdecl;
  
  iter = SearchSet_GetForwardIterator(src);
  ITERATOR_FOREACH(iter, SearchSetNode, node)
    {
      newdecl = Declaration_Clone((Declaration)node->elem);
      newdecl->usedIn = t;
      newnode = SearchSet_Find(dst, node->elem);
      if (newnode == NULL)
        SearchSet_Insert(dst, newdecl);
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

  CopyDeclarationSetElements(lib->simpleTypes, t->simpleTypes, t);
  CopyDeclarationSetElements(lib->elements, t->elements, t);
  CopyDeclarationSetElements(lib->components, t->components, t);
  CopyDeclarationSetElements(lib->unions, t->unions, t);
  CopyDeclarationSetElements(lib->unknowns, t->unknowns, t);

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
  
  if (t)
    {
#ifdef _WINDOWS
	  _vsnprintf (buffer, MAX_LENGTH, format, ap);
#else /* _WINDOWS */
      vsnprintf(buffer, MAX_LENGTH, format, ap);
#endif /* _WINDOWS */
      SList_Append(t->errorList, TtaStrdup(buffer));
    }
  va_end(ap);
}

/*----------------------------------------------------------------------
 * Show template errors if any
  ----------------------------------------------------------------------*/
void Template_ShowErrors(XTigerTemplate t)
{
  if (t && !SList_IsEmpty(t->errorList))
    {
      ShowNonSelListDlgWX(NULL, t->uri,
          TtaGetMessage (AMAYA, AM_TEMPLATE_HAS_ERROR),
          TtaGetMessage (AMAYA, AM_CLOSE), t->errorList);
    }
}

/*----------------------------------------------------------------------
 * Test if template has error(s)
  ----------------------------------------------------------------------*/
ThotBool Template_HasErrors(XTigerTemplate t)
{
  return t==NULL || (t!=NULL && !SList_IsEmpty(t->errorList));
}





/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void Template_PrintUnion (Declaration dec, int indent, XTigerTemplate t, FILE *file)
{	
#ifdef TEMPLATES
  ForwardIterator iter;
  SearchSetNode   node;
  Declaration     aux;
  char*           indentation;
  int             i=0;
  
  if (!t || !dec || (dec && dec->nature!=UnionNat))
    return;

  indentation = (char*) TtaGetMemory (indent*sizeof (char)+1);
  for (i = 0; i < indent; i++)
    indentation [i] = TAB;
  indentation [indent] = EOS;

  if (!SearchSet_IsEmpty (dec->unionType.include))
    {
      fprintf (file, "\n%sINCLUDE",indentation);

      iter = SearchSet_GetForwardIterator (dec->unionType.include);
      ITERATOR_FOREACH (iter, SearchSetNode, node)
        {
          aux = (Declaration) node->elem;
          if (aux!=NULL)
            {
              switch (aux->nature)
                {
                case SimpleTypeNat:
                case XmlElementNat:
                case ComponentNat:
                  fprintf (file, "\n%s+ %s ",indentation,aux->name);
                  if (aux->blockLevel == 2)
                    fprintf (file, " block");
                  else
                    fprintf (file, " inline");
                  if (aux->declaredIn != t)
                    fprintf (file, " (declared in %s)", aux->declaredIn->uri);
                  break;
                case UnionNat:
                  fprintf (file, "\n%s+ %s ",indentation,aux->name);
                  if (aux->blockLevel == 2)
                    fprintf (file, " block");
                  else
                    fprintf (file, " inline");
                  if (aux->declaredIn != t)
                    fprintf (file, " (declared in %s)", aux->declaredIn->uri);
                  Template_PrintUnion (aux, indent+1, t, file);
                default:
                  //impossible
                  break;
                }
            }
        }
      TtaFreeMemory(iter);
    }

  if (!SearchSet_IsEmpty(dec->unionType.exclude))
    {
      fprintf (file, "\n%sEXCLUDE",indentation);
    
      iter = SearchSet_GetForwardIterator(dec->unionType.exclude);
      ITERATOR_FOREACH(iter, SearchSetNode, node)
        {
          aux = (Declaration) node->elem;
          if (aux!=NULL)
            {
            switch (aux->nature)
              {
              case SimpleTypeNat:
              case XmlElementNat:
              case ComponentNat:
                fprintf (file, "\n%s+ %s ",indentation,aux->name);
                if (aux->blockLevel == 2)
                  fprintf (file, " block");
                else
                  fprintf (file, " inline");
                if (aux->declaredIn != t)
                  fprintf (file, " (declared in %s)", aux->declaredIn->uri);
                break;
              case UnionNat:
                fprintf (file, "\n%s+ %s ",indentation,aux->name);
                if (aux->blockLevel == 2)
                  fprintf (file, " block");
                else
                  fprintf (file, " inline");
                if (aux->declaredIn != t)
                  fprintf (file, " (declared in %s)", aux->declaredIn->uri);
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
#ifdef TEMPLATE_DEBUG
#ifdef TEMPLATES
  ForwardIterator iter;
  SearchSetNode     node;
  Declaration     dec;

  if (!t)
    return;
  
  /* Simple types : */

  if (!SearchSet_IsEmpty(t->simpleTypes))
    {
      fprintf (file, "\n\nSIMPLE TYPES\n");
      fprintf (file, "------------");
      iter = SearchSet_GetForwardIterator(t->simpleTypes);
      ITERATOR_FOREACH(iter, SearchSetNode, node)
        {
          dec = (Declaration) node->elem;
          fprintf (file, "\n(%p) %s ", dec, dec->name);
          if (dec->blockLevel == 2)
            fprintf (file, " block");
          else
            fprintf (file, " inline");
          if (dec->declaredIn != t && dec->declaredIn->uri)
            fprintf (file, " (declared in %s)", dec->declaredIn->uri);
        }
      TtaFreeMemory(iter);  
    }

  /* XML elements : */
  if (!SearchSet_IsEmpty(t->elements))
    {
      fprintf (file, "\n\nXML ELEMENTS\n");
      fprintf (file, "------------");
      iter = SearchSet_GetForwardIterator(t->elements);
      ITERATOR_FOREACH(iter, SearchSetNode, node)
        {
          dec = (Declaration) node->elem;
          fprintf (file, "\n(%p) %s ", dec, dec->name);
          if (dec->blockLevel == 2)
            fprintf (file, " block");
          else
            fprintf (file, " inline");
          if (dec->declaredIn != t && dec->declaredIn->uri)
            fprintf (file, " (declared in %s)", dec->declaredIn->uri);
        }
      TtaFreeMemory(iter);  
    }

  /* Components : */
  if (!SearchSet_IsEmpty(t->components))
    {
      fprintf (file, "\n\nCOMPONENTS\n");
      fprintf (file, "------------");
      iter = SearchSet_GetForwardIterator(t->components);
      ITERATOR_FOREACH(iter, SearchSetNode, node)
        {
          dec = (Declaration) node->elem;
          fprintf (file, "\n(%p) %s ", dec, dec->name);
          if (dec->blockLevel == 2)
            fprintf (file, " block");
          else
            fprintf (file, " inline");
          if (dec->declaredIn != t && dec->declaredIn->uri)
            fprintf (file, " (declared in %s)", dec->declaredIn->uri);
          FPrintElement(file, dec->componentType.content, 1);
        }
      TtaFreeMemory(iter);  
    }

  /* Unions : */
  if (!SearchSet_IsEmpty(t->unions))
    {
      fprintf (file, "\n\nUNIONS\n");
      fprintf (file, "------------");
      iter = SearchSet_GetForwardIterator(t->unions);
      ITERATOR_FOREACH(iter, SearchSetNode, node)
        {
          dec = (Declaration) node->elem;
          fprintf (file, "\n(%p) %s ", dec, dec->name);
          if (dec->blockLevel == 2)
            fprintf (file, " block");
          else
            fprintf (file, " inline");
          if (dec->declaredIn != t && dec->declaredIn->uri)
            fprintf (file, " (declared in %s)", dec->declaredIn->uri);
          Template_PrintUnion (dec, 1, t, file);
        }
      TtaFreeMemory(iter);  
    }
  
  /* Unknowns : */
  if (!SearchSet_IsEmpty (t->unknowns))
    {
      fprintf (file, "\n\nUNKNWONS\n");
      fprintf (file, "------------");
      iter = SearchSet_GetForwardIterator(t->unknowns);
      ITERATOR_FOREACH(iter, SearchSetNode, node)
        {
          dec = (Declaration) node->elem;
          fprintf (file, "\n(%p) %s ", dec, dec->name);
        }
      TtaFreeMemory(iter);  
    }
#endif /* TEMPLATES */
#endif /* TEMPLATE_DEBUG */
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void DumpAllDeclarations()
{
#ifdef TEMPLATE_DEBUG
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
        fprintf(file, "## %s (doc %d used %d times)\n", t->uri, t->doc, t->ref);
        fprintf(file, "################################################################################\n");
        PrintDeclarations(t, file);
        fprintf(file, "\n################################################################################\n");
        fprintf(file, "################################################################################\n\n");
      }
    }
  TtaFreeMemory(iter);
  
  TtaWriteClose (file);
#endif /* TEMPLATES */
#endif /* TEMPLATE_DEBUG */
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void DumpDeclarations (XTigerTemplate t)
{
#ifdef TEMPLATE_DEBUG
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
#endif /* TEMPLATE_DEBUG */
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void DumpTemplateReferences ()
{
#ifdef TEMPLATE_DEBUG
#ifdef TEMPLATES
  XTigerTemplate  t;
  ForwardIterator iter;
  HashMapNode     node;

  iter = HashMap_GetForwardIterator(Templates_Map);
  ITERATOR_FOREACH(iter, HashMapNode, node)
    {
      t = (XTigerTemplate) node->elem;
      if (t)
      {
        printf("(%0d) %s %d\n", t->doc, t->uri, t->ref);
      }
    }
  TtaFreeMemory(iter);
#endif /* TEMPLATES */
#endif /* TEMPLATE_DEBUG */
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
SearchSet GetComponents (XTigerTemplate t)
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
 * Doc can be an instance or a template.
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
void Template_AddReference (XTigerTemplate t)
{
#ifdef TEMPLATES
  if (t)
    t->ref += 1;
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void Template_RemoveReference (XTigerTemplate t)
{
#ifdef TEMPLATES
  if (t && t->ref > 0)
  {
    t->ref -= 1;
    if (t->ref == 0 && !Template_IsPredefined(t))
      Template_Close (t);
  }  
#endif /* TEMPLATES */
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
ThotBool Template_IsPredefined(XTigerTemplate t)
{
#ifdef TEMPLATES
  if (t)
    return (t->state & templPredefined) != 0;
#endif /* TEMPLATES */
  return FALSE;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
ThotBool Template_IsLibrary (XTigerTemplate t)
{
#ifdef TEMPLATES
  if (t)
    return (t->state & templLibraryFlag) != 0;
#endif /* TEMPLATES */
  return FALSE;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
ThotBool Template_IsTemplate(XTigerTemplate t)
{
#ifdef TEMPLATES
  if (t)
    return (t->state & templTemplate) != 0;
#endif /* TEMPLATES */
  return FALSE;
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
ThotBool Template_IsInstance(XTigerTemplate t)
{
#ifdef TEMPLATES
  if (t)
    return (t->state & templInstance) != 0;
#endif /* TEMPLATES */
  return FALSE;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
ThotBool Template_IsLoaded (XTigerTemplate t)
{
#ifdef TEMPLATES
  if (t)
    return (t->state & templloaded) != 0;
#endif /* TEMPLATES */
  return FALSE;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
ThotBool Template_IsInternal (XTigerTemplate t)
{
#ifdef TEMPLATES
  if (t)
    return (t->state & templInternal) != 0;
#endif /* TEMPLATES */
  return FALSE;
}

/*----------------------------------------------------------------------
 * Template_GetDeclarationSetFromNames
 * Create a set of Declarations from a string containing declaration names.
  ----------------------------------------------------------------------*/
SearchSet Template_GetDeclarationSetFromNames(XTigerTemplate t,
                                              const char* names, ThotBool expand)
{
  SearchSet       set = NULL, expandset = NULL;
  StringSet       nameset = NULL;
  StringSetNode   node;
  ForwardIterator iter;
  const char*     name;
  Declaration     dec;
#ifdef TEMPLATES
  if (t && names)
    {
      set = SearchSet_Create(NULL, (Container_CompareFunction)Declaration_Compare,
                             (Container_CompareFunction)Declaration_CompareToString);
      nameset = StringSet_CreateFromString(names, " ");
      iter    = StringSet_GetForwardIterator(nameset);
      ITERATOR_FOREACH(iter, StringSetNode, node)
        {
          name = (const char*)node->elem;
          if (name && name[0]!=EOS)
            {
              dec = Template_GetDeclaration(t, name);
              if (dec)
                  SearchSet_Insert(set, dec);
            }
        }
      TtaFreeMemory(iter);
      StringSet_Destroy(nameset);
      
      // Expand it if any
      if (expand)
        {
          expandset = Template_ExpandTypeSet (t, set);
          SearchSet_Destroy(set);
          return expandset;
        }
    }
#endif /* TEMPLATES */
  return set;
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
SearchSet Template_ExpandUnion(XTigerTemplate t, Declaration decl)
{
#ifdef TEMPLATES
  Declaration child;
  if (t && decl && decl->nature==UnionNat)
  {
    if (decl->unionType.expanded == NULL)
    {
      ForwardIterator iter;
      SearchSetNode   node;
      SearchSet       expanded = SearchSet_Create(NULL, 
                                  (Container_CompareFunction)Declaration_Compare,
                                  (Container_CompareFunction)Declaration_CompareToString);
      
      if (!strcmp(decl->name, UNION_ANY))
        {
          // Add only element and component declaration
          iter = SearchSet_GetForwardIterator(t->elements);
          ITERATOR_FOREACH(iter, SearchSetNode, node)
            SearchSet_Insert (expanded, node->elem);
          TtaFreeMemory(iter);
          iter = SearchSet_GetForwardIterator(t->components);
          ITERATOR_FOREACH(iter, SearchSetNode, node)
            SearchSet_Insert (expanded, node->elem);
          TtaFreeMemory(iter);
          
        }
      if (!strcmp(decl->name, UNION_ANYELEMENT))
        {
          iter = SearchSet_GetForwardIterator(t->elements);
          ITERATOR_FOREACH(iter, SearchSetNode, node)
            SearchSet_Insert (expanded, node->elem);
          TtaFreeMemory(iter);
        }
      else
        {
          iter = SearchSet_GetForwardIterator(decl->unionType.include);
          /* For each element in include set */
          ITERATOR_FOREACH(iter, SearchSetNode, node)
            {
              /* Fill the decl if not already done.*/
              child = (Declaration) node->elem;
              if (child)
                {
                  /* If element is union, expand it and add content in expanded map.*/
                  if (child->nature==UnionNat)
                    {
                      SearchSet       children  = Template_ExpandUnion(t, child);
                      ForwardIterator childIter = SearchSet_GetForwardIterator(children);
                      SearchSetNode   childNode;
                      ITERATOR_FOREACH(childIter, SearchSetNode, childNode)
                        {
                          Declaration granchild = (Declaration) childNode->elem;
                          SearchSet_Insert(expanded, granchild);
                        }
                      TtaFreeMemory(childIter);
                    }
                  else
                    {
                      /* Add it to expanded map.*/
                      SearchSet_Insert(expanded, child);
                    }
                }
            }
          TtaFreeMemory(iter);
          
          /* Remove all excluded descendants. */
          iter = SearchSet_GetForwardIterator(decl->unionType.exclude);
          /* For each element in exclude map */
          ITERATOR_FOREACH(iter, SearchSetNode, node)
            {
              child = (Declaration) node->elem;      
              if (child)
                SearchSet_RemoveElement(expanded, SearchSet_Find(expanded, child));
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
  Template_ExpandTypeSet
  Expand a type list with resolving unions.
  anySimple is not expanded.
  \param t Template
  \param types String in which look for types.
  \return The resolved type string.
  ----------------------------------------------------------------------*/
SearchSet Template_ExpandTypeSet (XTigerTemplate t, SearchSet types)
{
#ifdef TEMPLATES
  SearchSet       set = NULL;
  ForwardIterator iter;
  SearchSetNode   node;
  ForwardIterator iterbase;
  SearchSetNode   nodebase;
  Declaration     decl;

  if (t)
  {
    /* Set to store expanded types. */
    set = SearchSet_Create(NULL, 
                        (Container_CompareFunction)Declaration_Compare,
                        (Container_CompareFunction)Declaration_CompareToString);

    /* Fill map with expanded result from basemap.*/
    iterbase = SearchSet_GetForwardIterator (types);
    ITERATOR_FOREACH (iterbase, SearchSetNode, nodebase)
      {
        decl = (Declaration) nodebase->elem;
        if (decl)
          {
            if (decl->nature == UnionNat)
              {
                /* Expand a list element. */
                SearchSet unionDecl = Template_ExpandUnion (t, decl);
                if (unionDecl)
                  {
                    iter = SearchSet_GetForwardIterator (unionDecl);
                    ITERATOR_FOREACH (iter, SearchSetNode, node)
                      {
                        /* For each expanded element, add it to the final map.*/
                        SearchSet_Insert(set, node->elem);
                      }
                    TtaFreeMemory (iter);
                  }
              }
            else
              /* Add it without expansion.*/
              SearchSet_Insert (set, decl);
          }
      }
    TtaFreeMemory(iterbase);
    return set;
  }
  else
#endif /* TEMPLATES */
    return NULL;
}

/*----------------------------------------------------------------------
  Template_FilterInsertableElement
  Remove all elements which cannot be inserted at the specified place.
  \param t Template
  \param set Set of types to filter.  
  \param refelem If not null, return only element which can
           be inserted to the refelem
  \param insertafter if true, test for insertion after the refelem else
           test for insert it as first child.
  \note The map parameter can be affected.
  ----------------------------------------------------------------------*/
void Template_FilterInsertableElement (XTigerTemplate t, SearchSet set,
                                          Element refelem, ThotBool insertafter)
{
#ifdef TEMPLATES
  SearchSet       newset;
  ElementType     type;
  ForwardIterator iter;
  SearchSetNode   node;
  Declaration     dec;
  ThotBool        res;

  if (t && set && refelem)
    {
      newset = SearchSet_Create(NULL, (Container_CompareFunction)Declaration_Compare,
                                      (Container_CompareFunction)Declaration_CompareToString);
      iter = SearchSet_GetForwardIterator(set);
      ITERATOR_FOREACH(iter, SearchSetNode, node)
        {
          if (node->elem)
            {
              dec = (Declaration) node->elem;
              if (dec)
                {
                  if (dec->nature != ComponentNat &&
                      Declaration_GetElementType(dec, &type))
                    {
                      if (insertafter)
                        res = TtaCanInsertSibling(type, refelem, FALSE, t->doc);
                      else
                        res = TtaCanInsertFirstChild(type, refelem, t->doc);
                      if (res)
                        SearchSet_Insert(newset, node->elem);
                    }
                  else
                    SearchSet_Insert(newset, node->elem);
                }
            }
        }
      TtaFreeMemory (iter);
      SearchSet_Swap(set, newset);
      SearchSet_Destroy(newset);   
    }
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Template_UserCompareDeclaration
  Sort a list of Declaration to be user friendly
  First components and then XmlElements and sorted alphabeticaly.
  ----------------------------------------------------------------------*/
static int Template_UserCompareDeclaration(Declaration dec1 ,Declaration dec2)
{
#ifdef TEMPLATES
  if (dec1->nature == dec2->nature)
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
char* Template_ExpandTypes (XTigerTemplate t, const char* types,
                                          Element refelem, ThotBool insertafter)
{
#ifdef TEMPLATES
  if (t)
  {
    /* Set of type names to expand. */
    SearchSet       set;
    Declaration     dec;
    DLList          list = NULL;
    ForwardIterator iter;
    SearchSetNode   node;
    DLListNode      listnode;
    int             pos = 0;
    char            result[MAX_LENGTH];

    /* Fill a string with results.*/
    set = Template_GetDeclarationSetFromNames(t, types, true);
    if (set)
      {
        if (refelem)
          Template_FilterInsertableElement(t, set, refelem, insertafter);
        
        list = DLList_Create();
        iter = SearchSet_GetForwardIterator(set);
        ITERATOR_FOREACH(iter, SearchSetNode, node)
            DLList_Append(list, node->elem);
        TtaFreeMemory (iter);
        
        // Sort the list.
        DLList_Sort(list, (Container_CompareFunction)Template_UserCompareDeclaration);
        
        pos = 0;
        iter = DLList_GetForwardIterator(list);
        ITERATOR_FOREACH (iter, DLListNode, listnode)
          {
            dec = (Declaration)listnode->elem;
            strcpy (result + pos, (char*)dec->name);
            pos += strlen((char*) dec->name);
            result[pos] = ' ';
            pos++;
          }

        TtaFreeMemory (iter);
        result[pos] = 0;
      }
    SearchSet_Destroy (set);
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
  SearchSetNode   node;
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
        
        iter = SearchSet_GetForwardIterator(decl->unionType.include);
        ITERATOR_FOREACH(iter, SearchSetNode, node)
            can = Template_IsElementTypeAllowed(type, (Declaration)node->elem);
        if (can)
        {
          iter = SearchSet_GetForwardIterator(decl->unionType.exclude);
          ITERATOR_FOREACH(iter, SearchSetNode, node)
            {
              if (Template_IsElementTypeAllowed(type, (Declaration)node->elem))
              {
                can = FALSE;
                break;
              }
            }
        }    
        TtaFreeMemory(iter);
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
  ThotBool        can;
  Declaration     declType;
  ForwardIterator iter;
  SearchSetNode   node;

  if (decl)
  {
    switch(decl->nature)
    {
      case UnionNat:
        if (!strcmp(decl->name, UNION_ANY)) /* Allow all. */
          return TRUE;

        if (!strcmp(decl->name, UNION_ANYELEMENT)) /* Allow all elements. */
        {
          declType = (Declaration)SearchSet_SearchElement (decl->declaredIn->elements, (void*)type, NULL);
          return declType!=NULL;
        }
        if (!strcmp(decl->name, UNION_ANYCOMPONENT)) /* Allow all components. */
        {
          declType = (Declaration)SearchSet_SearchElement (decl->declaredIn->components, (void*)type, NULL);
          return declType!=NULL;
        }
        if (!strcmp(decl->name, UNION_ANYSIMPLE)) /* Allow all components. */
        {
          declType = (Declaration)SearchSet_SearchElement (decl->declaredIn->simpleTypes, (void*)type, NULL);
          return declType!=NULL;
        }

        can = FALSE;

        iter = SearchSet_GetForwardIterator(decl->unionType.include);
        ITERATOR_FOREACH(iter, SearchSetNode, node)
          {
            can = Template_IsTypeAllowed(type, (Declaration)node->elem);
          }
        
        if (can)
          {
            iter = SearchSet_GetForwardIterator(decl->unionType.exclude);
            ITERATOR_FOREACH(iter, SearchSetNode, node)
              {
                if (Template_IsTypeAllowed(type, (Declaration)node->elem))
                  {
                    can = FALSE;
                    break;
                  }
              }
          }        
        TtaFreeMemory(iter);
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
  StringSet       set;
  ForwardIterator iter;
  StringSetNode   node;
  Declaration     decl;
  
  t = GetXTigerDocTemplate(doc);
  if (t)
    {
      set  = StringSet_CreateFromString(bagTypes, " ");
      iter = StringSet_GetForwardIterator(set);
      ITERATOR_FOREACH(iter, StringSetNode, node)
        {
          decl = Template_GetDeclaration(t, (const char*)node->elem);
          if (Template_IsElementTypeAllowed(type, decl))
            {
              res = TRUE;
              break;
            }
        }
      TtaFreeMemory(iter);
      StringSet_Destroy(set);
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
  StringSet       set;
  ForwardIterator iter;
  StringSetNode   node;
    
  t = GetXTigerDocTemplate(doc);
  if (t)
    {
      set  = StringSet_CreateFromString(bagTypes, " ");
      iter = StringSet_GetForwardIterator(set);
      ITERATOR_FOREACH(iter, StringSetNode, node)
        {
          decl = Template_GetDeclaration(t, (const char*)node->elem);
          if (Template_IsTypeAllowed(type, decl))
            {
              res = TRUE;
              break;
            }
        }
      TtaFreeMemory(iter);
      StringSet_Destroy(set);
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
  
  t = GetXTigerDocTemplate(doc);
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
ThotBool Template_CanInsertTypeInBagElement (Document doc, const char* type, Element bag)
{
  ThotBool res = FALSE;
#ifdef TEMPLATES
  XTigerTemplate  t;
  char           *bagTypes,  *listtypes = NULL;
  
  t = GetXTigerDocTemplate(doc);
  if (t && bag)
  {
    bagTypes = GetAttributeStringValueFromNum (bag, Template_ATTR_types, NULL);
    if (bagTypes)
      {
        listtypes = Template_ExpandTypes (t, bagTypes, NULL, FALSE);
        res = Template_CanInsertTypeInBag (doc, type, listtypes);
        TtaFreeMemory (listtypes);
        TtaFreeMemory (bagTypes);
      }
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
ThotBool Template_CanInsertElementInUse (Document doc, ElementType type,
                                         char* useType, Element parent, int position)
{
#ifdef TEMPLATES
  XTigerTemplate  t;
  Element         elem;
  t = GetXTigerDocTemplate(doc);
  if (t && useType)
    {
      // Allow only simple type element.
      if (Template_GetSimpleTypeDeclaration (t, useType))
        {
          if (position == 0)
            return TtaCanInsertFirstChild(type, parent, doc);
          else
            {
              for (elem = TtaGetFirstChild(parent); position>0 &&
                     elem; position--, TtaNextSibling(&elem));
              if (elem)
                return TtaCanInsertSibling (type, elem, FALSE, doc);
            }       
        }
    }
#endif /* TEMPLATES */
  return FALSE;
}


/**----------------------------------------------------------------------
  Template_IsUsedComponentInSubtree
  Test if a component is used by xt:use or xt:union.
  Use it to test if a xt:component or a xt:use is used in a template.
  // Param validity must be tested by caller.
  ----------------------------------------------------------------------*/
ThotBool Template_IsUsedComponentInSubtree (XTigerTemplate t, Document doc,
                                           Element elem, const char* name)
{
#ifdef TEMPLATES
  ElementType  elType;
  SSchema      schema;
  Declaration  decl;
  char        *elName, *types;
  SearchSet    set;
  ThotBool     res;
  Element      child;
  
#ifdef TEMPLATE_DEBUG
  if(t && doc && elem && name && name[0]!=EOS)
#endif /* TEMPLATE_DEBUG */
    {
      elType = TtaGetElementType(elem);
      if (elType.ElTypeNum==Template_EL_union)
        {
          schema = TtaGetSSchema ("Template", doc);
          if(elType.ElSSchema == schema)
            {
              elName = GetAttributeStringValueFromNum(elem, Template_ATTR_name, NULL);
              decl = Template_GetUnionDeclaration(t, elName);
              TtaFreeMemory(elName);
              set = Template_ExpandUnion(t, decl);
              return SearchSet_Search(set, (void*) name, NULL)!=NULL;
            }
        }
      else if(elType.ElTypeNum==Template_EL_useEl ||
              elType.ElTypeNum==Template_EL_useSimple)
        {
          schema = TtaGetSSchema ("Template", doc);
          if(elType.ElSSchema == schema)
            {
              types = GetAttributeStringValueFromNum(elem, Template_ATTR_types, NULL);
              set = Template_GetDeclarationSetFromNames(t, types, TRUE);
              res = SearchSet_Search(set, (void*) name, NULL)!=NULL;
              SearchSet_Destroy(set);
              TtaFreeMemory(types);
              if (res)
                return TRUE;
            }          
        }
      
      child = TtaGetFirstChild(elem);
      while(child)
        {
          if (Template_IsUsedComponentInSubtree (t, doc, child, name))
            return TRUE;
          TtaNextSibling(&child);
        }

    }
#endif /* TEMPLATES */
  return FALSE;
}

/**----------------------------------------------------------------------
  Template_IsUsedComponent
  Test if a component is used by xt:use or xt:union.
  Use it to test if a xt:component or a xt:use is used in a template.
  ----------------------------------------------------------------------*/
ThotBool Template_IsUsedComponent (XTigerTemplate t, Document doc, const char* name)
{
#ifdef TEMPLATES
  Element elem;
  if(t && doc&& name && name[0]!=EOS)
    {
      elem = TtaGetMainRoot(doc);
      if(elem)
        return Template_IsUsedComponentInSubtree(t, doc, elem, name);
    }
#endif /* TEMPLATES */
  return FALSE;
}
