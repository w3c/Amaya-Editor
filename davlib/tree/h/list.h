/* --------------------------------------------------------
 ** Header file fron an implementation of a dynamic list. 
 **
 ** Author : Manuele Kirsch Pinheiro
 ** Project CEMT - II/UFRGS - Opera Group/INRIA
 ** Finance by CNPq(Brazil)/INRIA(France) Cooperation
 **
 ** Date : 24 / Oct / 2001
 **
 ** $Id$
 ** $Revision$
 ** --------------------------------------------------- 
 */

#include <stdlib.h>		/* necessary includes */
#include <stdio.h>

#ifndef AWLIST_H
#define AWLIST_H

#define LIST_OK 1		/* success flag */
#define LIST_FAILED -1		/* failed flag */

                       		/* some macro definitions */
#define LIST_CALLOC(count,size)	 calloc((count),(size))
#define LIST_FREE(ptr)  	free((ptr))


typedef struct _awlist {		/* list's structure */
    void **elements;		/* list's elements are pointers to void */
    int index;	   		/* index of the last element introduzed in the list */
    int next;     		/* next element to be returned */
    int size;      		/* list's size */
} AwList;



extern AwList * AwList_new (int lsize);     	/* creates a new list */
extern int AwList_delete (AwList *nlist);   	/* deletes a list */
extern int AwList_put (AwList *nlist, void *data);	/* introduce new elements to the list */
extern void * AwList_next (AwList *nlist);   	/* returns the next element in the list */
extern void AwList_reset (AwList *nlist);		/* resets the next element index in the list */
extern AwList * AwList_duplicate ( AwList * nlist );	/* duplicates the list size, keeping all its elements */
extern int AwList_size (AwList *list);   		/* returns the list's size */
#endif 
