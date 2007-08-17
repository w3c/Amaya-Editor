#ifndef ATTRMENU_H_
#define ATTRMENU_H_

typedef enum
{
  attr_global,
  attr_local,
  attr_event,
  attr_other
} AttributeCategory;


/**
 * Element to enable attribute listings. 
 */
struct AttrListElem
{
  PtrSSchema           pSS;
  int                  num;
  AttributeCategory    categ;
  PtrAttribute         val;
  TypeRestriction      restr; /* restriction rescription.*/
};

typedef struct AttrListElem* PtrAttrListElem;

#define AttrListElem_GetTtAttribute(PtrAttrElem) (PtrAttrElem->pSS->SsAttribute->TtAttr[PtrAttrElem->num-1])
#define AttrListElem_GetName(PtrAttrElem) (AttrListElem_GetTtAttribute(PtrAttrElem)->AttrName)
#define AttrListElem_GetType(PtrAttrElem) (AttrListElem_GetTtAttribute(PtrAttrElem)->AttrType)

#define AttrListElem_IsMandatory(PtrAttrElem) (PtrAttrElem->restr.RestrFlags&attr_mandatory)
#define AttrListElem_IsReadOnly(PtrAttrElem) (PtrAttrElem->restr.RestrFlags&attr_readonly)
#define AttrListElem_IsEnum(PtrAttrElem) (PtrAttrElem->restr.RestrFlags&attr_enum)
#define AttrListElem_IsNew(PtrAttrElem) (PtrAttrElem->restr.RestrFlags&attr_new)

#endif /*ATTRMENU_H_*/
