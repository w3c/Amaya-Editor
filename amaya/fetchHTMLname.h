/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifndef FETCHHTMLNAME_H 
#define FETCHHTMLNAME_H

#include "parser.h"

typedef struct _ClosedElement *PtrClosedElement;
typedef struct _ClosedElement
  {				        /* an element closed by a start tag */
     int                 tagNum;	/* rank (in GIMappingTable) of closed
					   element */
     PtrClosedElement    nextClosedElem;/* next element closed by the same
					   start tag */
  }
ClosedElement;

/* define a pointer to let parser functions access the HTML tables */
extern ElemMapping        *pHTMLGIMapping;
extern AttributeMapping *pHTMLAttributeMapping;
#endif /* FETCHHTMLNAME_H */
