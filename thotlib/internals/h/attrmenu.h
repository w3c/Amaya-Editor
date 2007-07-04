#ifndef ATTRMENU_H_
#define ATTRMENU_H_

typedef enum
{
  attr_global,
  attr_local,
  attr_event,
  attr_other
} AttributeCategory;

typedef enum
{
  attr_normal    = 0,
  attr_mandatory = 1,
  attr_readonly  = 2,
  attr_new       = 8,
}AttributeFlag;

/**
 * Element to enable attribute listings. 
 */
struct AttrListElem
{
  PtrSSchema        pSS;
  int               num;
  int               flags; /* see AttributeFlag *//* oblig */
  AttributeCategory categ;
  PtrAttribute      val;
};

typedef struct AttrListElem* PtrAttrListElem;

#define AttrListElem_GetTtAttribute(PtrAttrElem) (PtrAttrElem->pSS->SsAttribute->TtAttr[PtrAttrElem->num-1])
#define AttrListElem_GetName(PtrAttrElem) (AttrListElem_GetTtAttribute(PtrAttrElem)->AttrName)
#define AttrListElem_GetType(PtrAttrElem) (AttrListElem_GetTtAttribute(PtrAttrElem)->AttrType)

#define AttrListElem_IsMandatory(PtrAttrElem) (PtrAttrElem->flags&attr_mandatory)
#define AttrListElem_IsReadOnly(PtrAttrElem) (PtrAttrElem->flags&attr_readonly)
#define AttrListElem_IsNew(PtrAttrElem) (PtrAttrElem->flags&attr_new)

#endif /*ATTRMENU_H_*/
