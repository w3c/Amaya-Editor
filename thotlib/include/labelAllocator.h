/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
#ifndef _LABELALLOCATOR_H_
#define _LABELALLOCATOR_H_

#ifdef __STDC__
typedef int (*NewLabelFunc) (Document document);
typedef int (*GetCurLabelFunc) (Document document);
typedef void (*SetCurLabelFunc) (Document document, int label);
#else /* __STDC__ */
typedef int (*NewLabelFunc) ();
typedef int (*GetCurLabelFunc) ();
typedef void (*SetCurLabelFunc) ();
#endif  /* __STDC__ */
 
/* Label allocator to overload default label allocation. */
typedef struct {
  NewLabelFunc NewLabel;
  GetCurLabelFunc GetCurLabel;
  SetCurLabelFunc SetCurLabel;
} T_LabelAllocator;

 
#ifndef __CEXTRACT__
#ifdef __STDC__

extern void         TtaSetLabelAllocator (T_LabelAllocator *labelAlloc);
 
#else  /* __STDC__ */

extern void         TtaSetLabelAllocator ( /* T_LabelAllocator *labelAlloc */ );

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
 
#endif

