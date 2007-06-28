#ifndef ATTRMENU_H_
#define ATTRMENU_H_

typedef enum{
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
  PtrSSchema        pSS;
  int               num;
  ThotBool          oblig;
  AttributeCategory categ;
  PtrAttribute      val;
};

typedef struct AttrListElem* PtrAttrListElem;

#define AttrListElem_GetTtAttribute(PtrAttrElem) (PtrAttrElem->pSS->SsAttribute->TtAttr[PtrAttrElem->num-1])
#define AttrListElem_GetName(PtrAttrElem) (AttrListElem_GetTtAttribute(PtrAttrElem)->AttrName)
#define AttrListElem_GetType(PtrAttrElem) (AttrListElem_GetTtAttribute(PtrAttrElem)->AttrType)


#endif /*ATTRMENU_H_*/
