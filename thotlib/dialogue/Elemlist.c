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

#include "thot_sys.h"
#include "tree.h"
#include "document.h"
#include "application.h"

#include "Elemlist.h"
#include "Elemlist_f.h"


/*----------------------------------------------------------------------
  Create a new list of element.
  ----------------------------------------------------------------------*/
DLList ElemList_Create()
{
  DLList list = DLList_Create();
  list->destroyElement = (Container_DestroyElementFunction)ElemListElement_Destroy;
  return list;
}

/*----------------------------------------------------------------------
  Create a new list element from a language element.
  @param level Level of insertion.
  @param type Type of element.
  @param comment Comment
  @param refElem Linked element.
  ----------------------------------------------------------------------*/
ElemListElement ElemListElement_CreateLanguageElement(int level, ElementType type,
                                                      const char* comment, Element refElem)
{
  ElemListElement elem = (ElemListElement)TtaGetMemory (sizeof(sElemListElement));
  elem->typeClass = LanguageElement;
  elem->elem.element.type = type;
  elem->elem.element.name = NULL;
  if (comment)
    elem->comment = TtaStrdup(comment);
  else
    elem->comment = NULL;
  elem->refElem = refElem;
  return elem;
}

/*----------------------------------------------------------------------
  Create a new list element from a base type.
  @param level Level of insertion.
  @param name Base type name.
  @param comment Comment
  @param refElem Linked element.
  ----------------------------------------------------------------------*/
ElemListElement ElemListElement_CreateBaseType(int level, const char* name,
                                               const char* comment, Element refElem)
{
  ElemListElement elem = (ElemListElement)TtaGetMemory (sizeof(sElemListElement));
  elem->typeClass = BaseType;
  elem->elem.baseTypeName = TtaStrdup(name);
  if (comment)
    elem->comment = TtaStrdup(comment);
  else
    elem->comment = NULL;
  elem->refElem = refElem;
  return elem;
}

/*----------------------------------------------------------------------
  Create a new list element from a language element.
  @param level Level of insertion.
  @param name Name of the component.
  @param declaration Declaration of the component.
  @param comment Comment
  @param refElem Linked element.
  ----------------------------------------------------------------------*/
ElemListElement ElemListElement_CreateComponent(int level, const char* name,
                                                const void* declaration,
                                                const char* comment, Element refElem)
{
  ElemListElement elem = (ElemListElement)TtaGetMemory (sizeof(sElemListElement));
  memset (elem, 0, sizeof(sElemListElement));
  elem->typeClass = DefinedComponent;
  elem->elem.component.name = name;
  elem->elem.component.declaration = declaration;
  if (comment)
    elem->comment = TtaStrdup(comment);
  else
    elem->comment = NULL;
  elem->refElem = refElem;
  return elem;
}


/*----------------------------------------------------------------------
  Destroy a list element
  @param elem Address of the element to destroy.
  ----------------------------------------------------------------------*/
void ElemListElement_Destroy(ElemListElement elem)
{
  if (elem->typeClass==BaseType)
    TtaFreeMemory(elem->elem.baseTypeName);
  else if (elem->typeClass == LanguageElement && elem->elem.element.name)
    TtaFreeMemory(elem->elem.element.name);

  if (elem->comment)
    TtaFreeMemory(elem->comment);
  TtaFreeMemory(elem);
}

/*----------------------------------------------------------------------
  Return the name of the element.
  ----------------------------------------------------------------------*/
const char* ElemListElement_GetName(ElemListElement elem)
{
  if (elem->typeClass == LanguageElement && elem->elem.element.name == NULL)
     elem->elem.element.name = TtaStrdup(TtaGetElementTypeName(elem->elem.element.type));
 
  switch(elem->typeClass)
    {
    case LanguageElement:
      return elem->elem.element.name;
    case BaseType:
      return elem->elem.baseTypeName;
    case DefinedComponent:
      return elem->elem.component.name;
    default:
      return NULL;
    }
}

/*----------------------------------------------------------------------
  Compare two elements.
  ----------------------------------------------------------------------*/
int ElemListElement_Compare(ElemListElement elem1, ElemListElement elem2)
{
  if (elem1->level!=elem2->level)
    return elem2->level - elem1->level;
  return strcmp(ElemListElement_GetName(elem1), ElemListElement_GetName(elem2)); 
}

