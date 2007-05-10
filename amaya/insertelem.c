/*
 *
 *  COPYRIGHT INRIA and W3C, 1996-2007
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * Authors: Emilien Kia
 *
 */

#define THOT_EXPORT extern
#include "amaya.h"
#include "document.h"

#include "containers.h"
#include "Elemlist.h"

#ifdef TEMPLATES
#include "Template.h"
#include "templates.h"
#include "templates_f.h"
#include "templateDeclarations.h"

#include "templateLoad_f.h"
#include "templateDeclarations_f.h"
#include "templateInstantiate_f.h"
#include "appdialogue_wx.h"
#include "init_f.h"
#include "wxdialogapi_f.h"
#include "AHTURLTools_f.h"

#endif /* TEMPLATES */


#include "fetchXMLname_f.h"
#include "MENUconf.h"
#include "parser.h"
#include "fetchXMLname_f.h"

typedef struct _sInsertableElementList *InsertableElementList;
typedef struct _sInsertableElementList
{
  /** Current selected element.*/
  Element elem;
  /** Insertable element list.*/
  DLList list;
} sInsertableElementList;


static HashMap InsertableElementMap = NULL;


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static InsertableElementList InsertableElementList_Create(Element elem, DLList list)
{
  InsertableElementList lst = (InsertableElementList)TtaGetMemory(sizeof(sInsertableElementList));
  lst->elem = elem;
  lst->list = list;
  return lst; 
}

static void InsertableElementList_Destroy(InsertableElementList list)
{
  if (list->list)
    DLList_Destroy(list->list);
  TtaFreeMemory(list);
}

/*----------------------------------------------------------------------
  InsertableElement_Init
  Initialize the module.
  ----------------------------------------------------------------------*/
void InsertableElement_Init()
{
  if (!InsertableElementMap)
    InsertableElementMap = PointerHashMap_Create((Container_DestroyElementFunction)InsertableElementList_Destroy, 32);
}

/*----------------------------------------------------------------------
  InsertableElement_Final
  Finalize the module.
  ----------------------------------------------------------------------*/
void InsertableElement_Final()
{
  if (InsertableElementMap)
  {
    HashMap_Destroy(InsertableElementMap);
    InsertableElementMap = NULL;
  }
}


#ifdef TEMPLATES
/*----------------------------------------------------------------------
  FillUnionResolvedPossibleElement
  Fill an element list with all possible element, resolving them if union.
  @param name Element name
  @param elem Document element to attach
  @param resolvedPath Path of different succesive union name.
  @param list List to fill.
  ----------------------------------------------------------------------*/
static void FillUnionResolvedPossibleElement(XTigerTemplate t, const char* name,
                                             Element elem, const char* resolvedPath,
                                             DLList list, int level)
{
  Declaration dec = Template_GetDeclaration (t, name);
  if (dec == NULL)
    return;
    
  if (dec->declaredIn->isPredefined)
  {
//    DLList_Append(list, ElemListElement_CreateComponent(level, dec->name,
//                                                        (void*)dec, resolvedPath, elem));
  }
  else if (dec->nature==ComponentNat)
  {
    DLList_Append(list, ElemListElement_CreateComponent(level, dec->name, (void*)dec,
                                                        resolvedPath, elem));
  }
  else if (dec->nature==UnionNat)
  {
    DLList          tempList = ElemList_Create();
    ForwardIterator iter = HashMap_GetForwardIterator(dec->unionType.include);
    HashMapNode     mapnode;
    DLListNode      listnode;

    int len1 = 0 , len2 = strlen(dec->name);
    if (resolvedPath!=NULL)
      len1 = strlen(resolvedPath);
    char* newPath = (char*)TtaGetMemory(len1+len2+2);
    if (len1>0)
    {
      strcpy(newPath, resolvedPath);
      newPath[len1] = '/';
      strcpy(newPath+len1+1, dec->name);
    }
    else
    {
      strcpy(newPath, dec->name);
    }
    
    ITERATOR_FOREACH(iter, DLListNode, listnode)
      {
        FillUnionResolvedPossibleElement(t, (char*)mapnode->key, elem, newPath, tempList, level);
      }
    TtaFreeMemory(iter);
    
    iter = DLList_GetForwardIterator(tempList);
    
    
    listnode = (DLListNode) ForwardIterator_GetFirst(iter);
    for(listnode = (DLListNode) ForwardIterator_GetFirst(iter); listnode;
          listnode = (DLListNode) ForwardIterator_GetNext(iter))
      DLList_Append(list, listnode->elem);
    TtaFreeMemory(iter);

    tempList->destroyElement = NULL;
    DLList_Destroy(tempList);
    
    TtaFreeMemory(newPath);
    
    /** todo Remove excluded elements.*/
  }
  else if (dec->nature==SimpleTypeNat)
  {
    DLList_Append(list, ElemListElement_CreateBaseType(level, dec->name, resolvedPath,
                                                       elem));
    /* Do nothing. */
  }
  else
  {
    /* Search in tgt std elements. */
    int xmlType; /* See parser.h */
    for(xmlType=XHTML_TYPE; xmlType<Template_TYPE; xmlType++)
    {
      ElementType elType = {0,0};
      char*       mappedName;
      char       content;
      ThotBool    checkProfile;
      MapXMLElementType(xmlType, dec->name, &elType, &mappedName, &content,
                        &checkProfile, TtaGetDocument(elem));
      if (elType.ElTypeNum!=0)
      {
        DLList_Append(list, ElemListElement_CreateLanguageElement(level, elType,
                                                                  resolvedPath, elem));
        break;
      }
    }
  }
}

/*----------------------------------------------------------------------
  FillInsertableElementFromElemAttribute
  Fill an element list with all possible elements from an attribute list.
  ----------------------------------------------------------------------*/
static void FillInsertableElementFromElemAttribute (XTigerTemplate t,
                                                    Element elem, Element refelem,
                                                    int attrib, DLList list, int level)
{
  ElementType     type = TtaGetElementType(elem);
  AttributeType   attributeType = {type.ElSSchema, attrib};
  Attribute       att = TtaGetAttribute (elem, attributeType);
  int             size = TtaGetTextAttributeLength (att);
  char*           types = (char *) TtaGetMemory (size+1); 

  TtaGiveTextAttributeValue (att, types, &size);

  HashMap         basemap = KeywordHashMap_CreateFromList(NULL, -1, types);
  HashMap         map     = Template_ExpandHashMapTypes(t, basemap);
  ForwardIterator iter;
  HashMapNode     node;
   
  iter = HashMap_GetForwardIterator(map);
  ITERATOR_FOREACH(iter, HashMapNode, node)
    {
      FillUnionResolvedPossibleElement(t, (const char*)node->key, refelem, NULL, list, level);
    }
  HashMap_Destroy (map);
  HashMap_Destroy (basemap);
  
  TtaFreeMemory (types);
}
#endif/* TEMPLATES */

/*----------------------------------------------------------------------
  FillInsertableElemList
  Fill an element list with all insertable elements (base element or
  XTiger comonent).
  ----------------------------------------------------------------------*/
static void FillInsertableElemList (Document doc, Element elem, DLList list)
{
  ElementType      type;
  Element          parent;
#ifdef TEMPLATES
  Element          child;
  ElementType      childType;
  XTigerTemplate   t;
  ThotBool         haveAncestorBag = FALSE;
#endif/* TEMPLATES */
  int level;
  ThotBool cont;

  if (elem){
    if (doc==0)
      doc = TtaGetDocument(elem);

#ifdef TEMPLATES
    t = GetXTigerTemplate(DocumentMeta[doc]->template_url);


    if(!IsTemplateElement(elem))
      elem = GetFirstTemplateParentElement(elem);

    // Search for first xt:bag ancestor.
    parent = elem;
    while(parent!= NULL && cont)
      {
        type = TtaGetElementType(parent);
        if(type.ElTypeNum==Template_EL_bag)
          {
            haveAncestorBag = TRUE;
            cont = FALSE;
          }
        parent = GetFirstTemplateParentElement(parent);
      }

    level = 0;
    cont = TRUE;

    // Process for each ancestor.
    while(elem!=NULL && cont)
    {
      type = TtaGetElementType(elem);
      switch(type.ElTypeNum)
        {
        case Template_EL_repeat:
          child = TtaGetFirstChild(elem);
          childType = TtaGetElementType(child);
          switch(childType.ElTypeNum)
          {
            case Template_EL_useEl:
              FillInsertableElementFromElemAttribute(t, child, elem,
                                                     Template_ATTR_types, list, level);
              break;
            case Template_EL_useSimple:
              FillInsertableElementFromElemAttribute(t, child, elem,
                                                     Template_ATTR_types, list, level);
              break;
            case Template_EL_bag:
              FillInsertableElementFromElemAttribute(t, child, elem,
                                                     Template_ATTR_types, list, level);
              break;
            default:
              break;
          }
          cont = haveAncestorBag;
          break;
        case Template_EL_useEl:
          // Fill for xt:use only if have no child.
          if (TtaGetFirstChild(elem)==NULL){
            FillInsertableElementFromElemAttribute(t, elem, elem,
                                                   Template_ATTR_types, list, level);
            cont = haveAncestorBag;
          }
          break;
        case Template_EL_bag:
          FillInsertableElementFromElemAttribute(t, elem, elem,
                                                 Template_ATTR_types, list, level);
          cont = FALSE;
          break;
        }
      elem = GetFirstTemplateParentElement(elem);
      level ++;
    }
#endif/* TEMPLATES */
  }
}

/*----------------------------------------------------------------------
  InsertableElement_GetList
  Get the insertable element list for a document.
  @param doc Document
  @return The insertable element list or NULL.
  ----------------------------------------------------------------------*/
DLList InsertableElement_GetList(Document doc)
{
  InsertableElementList list;

  list = (InsertableElementList) HashMap_Get(InsertableElementMap, (void*)doc);
  if (list)
    return list->list;
  else
    return NULL;
}

/*----------------------------------------------------------------------
  InsertableElement_Update
  Update the insertable element list for a document.
  @param el Selected element, cant be NULL.
  @param document Document, can be NULL.
  @param force No dont force the refresh of the list if the element is already selected.
  @return List of insertable elements.
  ----------------------------------------------------------------------*/
DLList InsertableElement_Update(Document doc, Element el)
{
  InsertableElementList list;

  if (doc == 0)
    doc= TtaGetDocument (el);
  list = (InsertableElementList) HashMap_Get (InsertableElementMap, (void*)doc);
  if (list == NULL)
  {
    list = InsertableElementList_Create (0, DLList_Create());
    HashMap_Set (InsertableElementMap, (void*)doc, list);
  }
  
  DLList_Empty (list->list);
  FillInsertableElemList (doc, el, list->list);
  list->elem = el;

  return list->list;
}

/*----------------------------------------------------------------------
  InsertableElement_DoInsertElement
  Insert the specified element.
  @param el Element to insert (ElemListElement)
  ----------------------------------------------------------------------*/
void InsertableElement_DoInsertElement (void* el)
{
  ElemListElement elem = (ElemListElement) el;
  Element         ref = elem->refElem;
  ElementType     refType = TtaGetElementType (ref);
  Document        doc = TtaGetDocument (ref);
  Element         newEl = NULL;
  SSchema         templateSSchema;

#ifdef AMAYA_DEBUG
  printf("insert %s into %s\n", ElemListElement_GetName(elem),
         TtaGetElementTypeName (refType));
#endif /* AMAYA_DEBUG */

#ifdef TEMPLATES
  templateSSchema = TtaGetSSchema ("Template", doc);
  if (templateSSchema && refType.ElSSchema == templateSSchema)
  {
    switch(refType.ElTypeNum)
    {
      case Template_EL_repeat:
        if (elem->typeClass==DefinedComponent)
          newEl = Template_InsertRepeatChild (doc, ref,
                                              (Declaration)elem->elem.component.declaration,
                                              -1);
        break;
      case Template_EL_bag:
        newEl = Template_InsertBagChild (doc, ref,
                                         (Declaration)elem->elem.component.declaration);
        break;
      default:
        break;
    }
  }
#endif /* TEMPLATES */

  if (newEl)
    TtaSelectElement (doc, newEl);
}
