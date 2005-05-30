/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
#ifndef _LABELALLOCATOR_H_
#define _LABELALLOCATOR_H_

typedef int (*NewLabelFunc) (Document document);
typedef int (*GetCurLabelFunc) (Document document);
typedef void (*SetCurLabelFunc) (Document document, int label);

/* Label allocator to overload default label allocation. */
typedef struct {
  NewLabelFunc NewLabel;
  GetCurLabelFunc GetCurLabel;
  SetCurLabelFunc SetCurLabel;
} T_LabelAllocator;

 
#ifndef __CEXTRACT__
extern void         TtaSetLabelAllocator (T_LabelAllocator *labelAlloc);
#endif /* __CEXTRACT__ */
 
#endif

