/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifndef FETCHHTMLNAME_H 
#define FETCHHTMLNAME_H

#include "parser.h"
#define MaxGIlength 14
#define DummyAttribute 500

typedef UCHAR_T GI[MaxGIlength];


typedef struct _ClosedElement *PtrClosedElement;
typedef struct _ClosedElement
  {				        /* an element closed by a start tag */
     int                 tagNum;	/* rank (in GIMappingTable) of closed
					    element */
     PtrClosedElement    nextClosedElem;/* next element closed by the same
					    start tag */
  }
ClosedElement;

typedef struct _GIMapping
  {                                     /* mapping of a HTML element */
     GI                  htmlGI;        /* name of the HTML element */
     CHAR_T              htmlContents;  /* info about the contents of the HTML element:
                                           'E'=empty,  space=some contents */
     int                 ThotType;      /* type of the Thot element or attribute */
     PtrClosedElement    firstClosedElem;/* first element closed by the start
					    tag htmlGI */
  }
GIMapping;

/* define a pointer to let parser functions access the HTML tables */
extern GIMapping        *pHTMLGIMapping;
extern AttributeMapping *pHTMLAttributeMapping;

#endif /* FETCHHTMLNAME_H */
