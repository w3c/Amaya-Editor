/* --------------------------------------------------------
 ** An implementation of a dynamic list. 
 **
 ** Author : Manuele Kirsch Pinheiro
 ** Project CEMT - II/UFRGS - Opera Group/INRIA
 ** Finance by CNPq(Brazil)/INRIA(France) Cooperation
 **
 ** Date : 24 / Oct / 2001
 **
 ** $Id$
 ** $Revision$
 ** 
 ** --------------------------------------------------- 
 */

#include "list.h"

/* ----------------------------------------------------
 * this function creates a new list, with the initial
 * size indicated by "lsize" parameter.
 * 
 * Parameters :
 *      int lsize : initial size for the list
 * Return :
 *      AwList * : a pointer to the new created list
 * --------------------------------------------------   
 */
extern AwList * AwList_new (int lsize) 
{
    AwList* nlist=NULL;

    nlist = (AwList *) LIST_CALLOC(1,sizeof (AwList));
    if (nlist != NULL) 
     {
        nlist->elements = (void **) LIST_CALLOC(lsize,sizeof(void *));
        if (nlist->elements == NULL) 
         {
            LIST_FREE (nlist);
            return NULL;
         }
        else
         {
            nlist->size = lsize;
            nlist->index=0;
            nlist->next=0;
         }
     }
    return nlist;
}


/* ----------------------------------------------------
 * this function deletes the listindicated by the 
 * parameter "nlist".
 * 
 * Parameters :
 *      AwList *nlist : the list to be removed
 * Return :
 *      int : LIST_OK (normally 1), if the operation 
 *                    was succeed
 *            LIST_FAILED (normally -1), if the 
 *                    operation failed        
 * --------------------------------------------------   
 */
extern int AwList_delete (AwList *nlist) 
{
        
    if (nlist!= NULL) 
     {
        LIST_FREE(nlist->elements);
        LIST_FREE(nlist);
        return LIST_OK;
     }       
    return LIST_FAILED;
}       


/* ----------------------------------------------------
 * this function introduce new elements in the list
 * indicated by the parameter "nlist".
 * 
 * Parameters :
 *      AwList *nlist : the list to be removed
 *      void *data : pointer to the new element in the list
 * Return :
 *      int : LIST_OK (normally 1), if the operation 
 *                    was succeed
 *            LIST_FAILED (normally -1), if the 
 *                    operation failed        
 * --------------------------------------------------   
 */
extern int AwList_put (AwList *nlist, void *data) 
{
    AwList *dlist = NULL;
    
    if (nlist != NULL) 
     {
        if (nlist->index < nlist->size) 
            nlist->elements[nlist->index++] = data;
        else 
         {
            /* duplicate */
            dlist = AwList_duplicate (nlist);
            if (dlist==NULL)
                return LIST_FAILED;
            else 
             {
                *nlist = *dlist;
                AwList_put (nlist,data);
             } 
                
         }       
        return LIST_OK;
     }         
    return LIST_FAILED;
}

/* ----------------------------------------------------
 * this function the next element in the list
 * indicated by the parameter "nlist".
 * 
 * Parameters :
 *      AwList *nlist : the list to be removed
 * Return :
 *      void *data : pointer to the element in the list
 * --------------------------------------------------   
 */
extern void * AwList_next (AwList *nlist) 
{
    if (nlist!=NULL) 
     {
        if (nlist->next < nlist->size) 
                return nlist->elements[nlist->next++];
     }
    return NULL; 
}

/* ----------------------------------------------------
 * this function the reset the next element index in 
 * in the list indicated by the parameter "nlist". So
 * the next call for the function "AwList_next" will 
 * return the first element from the list.
 * 
 * Parameters :
 *      AwList *nlist : the list to be removed
 * --------------------------------------------------   
 */
extern void AwList_reset (AwList *nlist) 
{
    if (nlist!=NULL) 
     {
        nlist->next = 0;
     }   
}


/* ----------------------------------------------------
 * this function duplicates the list's size, keeping 
 * all original list's elements.
 * 
 * Parameters :
 *      AwList *nlist : the list to be removed
 * Return :
 *      AwList *: pointer to the new resized list
 * --------------------------------------------------   
 */
extern AwList * AwList_duplicate ( AwList * nlist ) 
{
    AwList *dlist = NULL;
    int i;
    
    /* creates a new list */
    if (nlist != NULL) 
     {
        dlist = AwList_new (nlist->size*2);
        /* copy all elements and informations to the new list */
        if (dlist != NULL) 
         {
            dlist->next = nlist->next;
            dlist->index = nlist->index;
            for (i=0; i<nlist->index;i++)
                    dlist->elements[i] = nlist->elements[i];
         }
     }
    return dlist;
}



/* ----------------------------------------------------
 * this function returns the list's size (that is, the
 * position of it's last element.
 *
 * Returns: int  the list size or -1 if failed
 * ----------------------------------------------------
 */
extern int AwList_size (AwList *list) 
{
    if (list != NULL) return list->index;
    else return (-1);
}


