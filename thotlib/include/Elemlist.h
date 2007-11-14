#ifndef ELEMLIST_H_
#define ELEMLIST_H_
 
#define THOT_EXPORT extern

#include "document.h"
#include "containers.h"

typedef enum
{
  /** XTiger component. */
  DefinedComponent,
  /** Base language element. */
  LanguageElement,
  /** Base type (string, number ...).*/
  BaseType
}ElementListElementTypeClass;

/**
 * A list of element.
 */
typedef struct _sElemListElement *ElemListElement;
typedef struct _sElemListElement{
  /** Level of insertion.
   * Smaller is more prioritary.*/
  int         level;

  /** "Class" of element type.*/
  ElementListElementTypeClass typeClass;
  
  union
  {
    /** Element type in case of LanguageElement class.*/
    struct
    {
      ElementType type;
      char* name;
    }element;
    
    /** Base type name.*/
    char* baseTypeName;
    
    /** Component. */
    struct {
      const char* name; /* Ref to the name.*/
      const void* declaration; /* ref to the XTiger Declaration of the component.*/
    }component;
  }elem;
  
  /** Comment.*/
  char*       comment;
  /** Linked element.
   * Element to which apply (insert/change type) the new element.*/
  Element     refElem;
} sElemListElement;

typedef void (*ElemListElement_DoInsertElementFunction)(ElemListElement);

#ifndef __CEXTRACT__
extern DLList ElemList_Create();

extern ElemListElement ElemListElement_CreateLanguageElement(int level, ElementType type, const char* comment, Element refElem);
extern ElemListElement ElemListElement_CreateBaseType(int level, const char* name, const char* comment, Element refElem);
extern ElemListElement ElemListElement_CreateComponent(int level, const char* name, const void* declaration, const char* comment, Element refElem);
extern const char*     ElemListElement_GetName(ElemListElement elem);
extern void ElemListElement_Destroy(ElemListElement elem);
extern int ElemListElement_Compare(ElemListElement elem1, ElemListElement elem2);
#endif /*__CEXTRACT__*/
 
#endif /*ELEMLIST_H_*/
