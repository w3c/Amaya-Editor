/* --------------------------------------------------------
 ** A node implementation for use in a tree structure
 **
 ** Author : Manuele Kirsch Pinheiro
 ** Project CEMT - II/UFRGS - Opera Group/INRIA
 ** Finance by CNPq(Brazil)/INRIA(France) Cooperation
 **
 ** Date : 30 / Oct / 2001
 **
 ** $Id$
 ** $Date$
 ** $Author$
 ** $Revision$
 ** $Log$
 ** Revision 1.2  2002-06-12 10:30:06  kirschpi
 ** - adjusts in code format
 ** Manuele
 **
 ** Revision 1.1  2002/05/31 10:48:47  kirschpi
 ** Added a new module for WebDAV purposes _ davlib.
 ** Some changes have been done to add this module in the following files:
 ** amaya/query.c, amaya/init.c, amaya/answer.c, amaya/libwww.h, amaya/amayamsg.h,
 ** amaya/EDITOR.A, amaya/EDITORactions.c, amaya/Makefile.libwww amaya/Makefile.in,
 ** config/amaya.profiles, tools/xmldialogues/bases/base_am_dia.xml,
 ** tools/xmldialogues/bases/base_am_dia.xml, Makefile.in, configure.in
 ** This new module is only activated when --with-dav options is used in configure.
 **
 ** Revision 1.4  2002/03/28 10:57:03  kirschpi
 ** New functions AwNode_howManyAttributes and AwNode_howManyChildren
 ** to indicate the number of attributes and children in the node.
 **
 ** Revision 1.3  2001/11/01 09:48:02  kirschpi
 ** Twi functions (getType and setType) have been added
 ** to manipulate the node type identifier.
 **
 ** Revision 1.2  2001/10/31 16:48:21  kirschpi
 ** Some corrections have been made in the treatment
 ** of the "level" variable in the node.
 **
 ** Revision 1.1  2001/10/31 09:39:28  kirschpi
 ** Initial revision
 **
 **
 ** --------------------------------------------------------
 */

#include "awtree.h"


/* --------------------------------------------------------
 * this function creates a new node, with the 
 * size indicated by the parameters.
 *
 * Parameters :
 *      int nchildren : estimate number of children
 *      int nattrib : estimate number of attributes
 *      int infosize : information string size
 * Return :
 *      AwNode : the created node
 *               (NULL if an error is produced)
 * --------------------------------------------------------
 */
extern AwNode* AwNode_new (int nchildren, int nattrib, int infosize) 
{
     AwNode * node = NULL;

     node = (AwNode *) AW_CALLOC (1,sizeof(AwNode));
#ifdef DEBUG    
     fprintf (stderr,"\nPASS node Calloc done \n");
#endif 
     if (node!=NULL) 
      {
         node->level = 0;
         node->type = 0;
         node->father = NULL;    
         node->infosize = infosize;
         
         node->children = AwList_new (nchildren);
         node->attributes = AwList_new (nattrib);
         node->info = AwString_new (infosize);
         
         if (node->children == NULL || node->attributes == NULL || node->info == NULL) 
          {
#ifdef DEBUG    
             fprintf (stderr,"Memory Allocations FAILED \n");
#endif
             AW_FREE (node);
             node=NULL;
          }
      }
     return node;
}


/* --------------------------------------------------------
 * this function removes node indicated by the parameter
 * "node".
 *
 * Parameters :
 *      AwNode * node : node to be removed
 * Return :
 *      int : AW_OK if the operation succeed,
 *              AW_FAILD if not
 * Nore :
 *      This function does NOT removes the node children
 *      and attributes, neither the node father.
 * --------------------------------------------------------
 */
extern int AwNode_delete (AwNode *node) 
{
    int ret=AW_OK;
    
    if (node!=NULL) 
     {       
        ret = AwList_delete (node->children);
        ret *= AwList_delete (node->attributes);
        ret *= AwString_delete (node->info);
        AW_FREE(node);
#ifdef DEBUG    
        printf ("PASS node removed\n");
#endif
     }   
    else ret=AW_FAILED;
    return ret;
}



/* --------------------------------------------------------
 * this function sets the information string for the node 
 * indicated by the parameter "node".
 *
 * Parameters :
 *      AwNode * node : node to be setted
 *      AwString info : information string
 * Return :
 *      int : AW_OK if the operation succeed,
 *              AW_FAILD if not
 * Note :
 *      This function does a COPY of the information
 *      string passed. It does not make a reference to it.
 * --------------------------------------------------------
 */
extern int AwNode_setInfo (AwNode *node, AwString info) 
{
    AwString str;
    int isize=0;
    
    if (node!=NULL && info!=NULL) 
     {
            isize = AwString_len(info);
            
        if (node->infosize < isize) 
         {   /* test the string size */
            printf ("NOTE string space (%d) is not enough (%d) \n",node->infosize,isize);
            str = AwString_new (isize+1);       /* alloc a new string with the necessary space */       
            if (str==NULL) 
             {
#ifdef DEBUG
                fprintf (stderr,"Not enough memory\n");
#endif
                return AW_FAILED;
             }
            AwString_delete (node->info);
            node->info = str;
         }

        return AwString_copy (node->info,info);                 /* copy the information */
     }
    
    return AW_FAILED;
}


/* --------------------------------------------------------
 * this function gets the information string for the node 
 * indicated by the parameter "node".
 *
 * Parameters :
 *      AwNode * node : node to be setted
 * Return :
 *      AwString : node's information string
 *              (NULL if some error is produced)
 * Note :
 *      This function does a COPY of the information
 *      string passed. It does not make a reference to it.
 * --------------------------------------------------------
 */ 
extern AwString AwNode_getInfo (AwNode *node) 
{
    AwString str=NULL;
    
    if (node!=NULL) 
     {
        str = AwString_new (AwString_len(node->info)+1);
        if (str!=NULL) 
         {
            if (AwString_copy (str,node->info) < 0) 
             {
               AwString_delete (str);
               str = NULL;
             }   
         }
     }
    
    return str; 
}



/* --------------------------------------------------------
 * this function sets the node level in the tree.
 *
 * Parameters :
 *      AwNode * node : node to be setted
 *      int level : node level
 * Return :
 *      int : AW_OK if the operation succeed,
 *              AW_FAILD if not
 * Note :
 *      You really should know very well what you are
 *      doing...
 * --------------------------------------------------------
 */
extern int AwNode_setLevel (AwNode *node, int level) 
{
    if (node!=NULL) 
     {
        node->level = level;
        return  AW_OK;
     }       
    return AW_FAILED;
}


/* --------------------------------------------------------
 * this function returns the node level in the tree.
 *
 * Parameters :
 *      AwNode * node : node to be setted
 * Return :
 *      int : node level 
 *              AW_FAILD if some problem occurs
 * --------------------------------------------------------
 */
extern int AwNode_getLevel (AwNode *node) 
{
    if (node!=NULL) 
     {
        return node->level;     
     }
    return AW_FAILED;

}



/* --------------------------------------------------------
 * this function sets the node type
 *
 * Parameters :
 *      AwNode * node : node to be setted
 *      int level : node type
 * Return :
 *      int : AW_OK if the operation succeed,
 *              AW_FAILD if not
 * --------------------------------------------------------
 */
extern int AwNode_setType (AwNode *node, int type) 
{
    if (node!=NULL) 
     {
        node->type = type;
        return  AW_OK;
     }       
    return AW_FAILED;
}



/* --------------------------------------------------------
 * this function returns the node type.
 *
 * Parameters :
 *      AwNode * node : node to be setted
 * Return :
 *      int : node type 
 *              AW_FAILED if some problem occurs
 * --------------------------------------------------------
 */
extern int AwNode_getType (AwNode *node) 
{
    if (node!=NULL) 
     {
        return node->type;      
     }
    return AW_FAILED;

}




/* --------------------------------------------------------
 * this function sets the node "father" of the node 
 * indicated by the parameter "node".
 *
 * Parameters :
 *      AwNode * node : node to be setted
 *      AwNode * father : node father
 * Return :
 *      int : AW_OK if the operation succeed,
 *              AW_FAILD if not
 * Note :
 *      This function does a reference to the "father" node
 *      "father" may be NULL
 *      You should really know what you are doing...
 * --------------------------------------------------------
 */
extern int AwNode_setFather (AwNode *node, AwNode *father) 
{
    if (node!=NULL) 
     {
         node->father = father;
         return AW_OK;   
     }   
    return AW_FAILED;
}

/* --------------------------------------------------------
 * this function returns the father node of the node
 * indicated by the parameter "node".
 *
 * Parameters :
 *      AwNode * node : node to be setted
 * Return :
 *      AwNode * : node father
 * Note :
 *      This function does a reference to the "father" node
 *      "father" may be NULL
 * --------------------------------------------------------
 */
extern AwNode * AwNode_getFather (AwNode *node) 
{
     AwNode *father=NULL;
     if (node!=NULL) 
             father=node->father;
     return father;
}






/* --------------------------------------------------------
 * this function introduces a new child node in the 
 * parameter "node".
 *
 * Parameters :
 *      AwNode * node : node where the child will be 
 *                      introduced
 *      AwNode * child : child node to be introduced
 * Return :
 *      int : AW_OK if the operation succeed,
 *              AW_FAILD if not
 * Note :
 *      This function does a reference to the node child.
 *      It does NOT copy the node, but it DOES set the
 *      child's father and level information.
 * --------------------------------------------------------
 */
extern int AwNode_putChild (AwNode *node, AwNode *child)  
{
    if (node!=NULL && child!=NULL) 
     {
        if ( AwList_put (node->children, (void *)child) > 0) 
         {  
            /* child->level++; */          
            child->father = node;
            child->level = node->level+1;
#ifdef DEBUG
            fprintf (stderr,"Putting child %s in %s\n",child->info,node->info);
#endif      
            return AW_OK;
         }
     }
    return AW_FAILED;
}


/* --------------------------------------------------------
 * this function returns the next child in the node 
 * children list.
 *
 * Parameters :
 *      AwNode * node : node 
 * Return :
 *      AwString : next node child 
 *              (NULL if some error is produced or there is
 *              no more child in the list)
 * Note :
 *      This function does a reference to the node child.
 *      It does NOT copy the node.
 * --------------------------------------------------------
 */ 
extern AwNode * AwNode_nextChild (AwNode *node) 
{
    AwNode *child=NULL; 
    if (node!=NULL) 
     {
        child = (AwNode *) AwList_next(node->children);
     }       
    return child;
}



/* --------------------------------------------------------
 * this function resets the children list, so the next  
 * call to nextChild will return the first child in the
 * children list.
 *
 * Parameters :
 *      AwNode * node : node 
 * --------------------------------------------------------
 */ 
extern void AwNode_resetChildren (AwNode *node) 
{
    if (node!=NULL) 
     {
        AwList_reset(node->children);
     }
}


/* --------------------------------------------------------
 * this function introduces a new attribute pair in the 
 * parameter "node".
 *
 * Parameters :
 *      AwNode * node : node where the child will be 
 *                      introduced
 *      AwNode * pair : node attribute to be introduced
 * Return :
 *      int : AW_OK if the operation succeed,
 *              AW_FAILD if not
 * Note :
 *      This function does a reference to the pair.
 *      It does NOT copy the attribute pair.
 * --------------------------------------------------------
 */
extern int AwNode_putAttribute (AwNode *node, AwPair *pair) 
{

    if (node!=NULL && pair!=NULL) 
     {
        return AwList_put (node->attributes, (void *)pair );    
     }
    return AW_FAILED;
        
}


/* --------------------------------------------------------
 * this function returns the next attribute in the node 
 * attributes list.
 *
 * Parameters :
 *      AwNode * node : node 
 * Return :
 *      AwString : next node attribute 
 *              (NULL if some error is produced or there is
 *              no more attributes in the list)
 * Note :
 *      This function does a reference to the attribute.
 *      It does NOT copy the attribute pair.
 * --------------------------------------------------------
 */ 
extern AwPair * AwNode_nextAttribute (AwNode *node) 
{
    AwPair *pair=NULL;  
    if (node!=NULL) 
     {
        pair = (AwPair *) AwList_next(node->attributes);
     }       
    return pair;
}


/* --------------------------------------------------------
 * this function resets the attributes list, so the next  
 * call to nextattribute will return the first attributes 
 * in the attributes list.
 *
 * Parameters :
 *      AwNode * node : node 
 * --------------------------------------------------------
 */ 
extern void AwNode_resetAttributes (AwNode *node) 
{
    if (node!=NULL) 
     {
        AwList_reset(node->attributes);
     }
}


/* ------------------------------------------------------
 * this function returns the number of attributes defined
 * for the node.
 * 
 * Returns : 
 *    int       the number of attributes or -1 if failed
 * ------------------------------------------------------
 */ 
extern int AwNode_howManyAttributes (AwNode *node) 
{
    if (node != NULL) 
        return (AwList_size(node->attributes));
    else 
        return (-1);
}


/* ------------------------------------------------------
 * this function returns the number of childred defined
 * for the node.
 * 
 * Returns : 
 *    int       the number of children or -1 if failed
 * ------------------------------------------------------
 */ 
extern int AwNode_howManyChildren (AwNode *node) 
{
    if (node != NULL) 
        return (AwList_size(node->children));
    else 
        return (-1);
}




