/* --------------------------------------------------------
 ** A tree implementation.
 ** All functions use pre-fixed (depth first) algorithms.
 **
 ** Author : Manuele Kirsch Pinheiro
 ** Project CEMT - II/UFRGS - Opera Group/INRIA
 ** Finance by CNPq(Brazil)/INRIA(France) Cooperation
 **
 ** Date : 31 / Oct / 2001
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
 ** Revision 1.3  2002/03/28 10:58:28  kirschpi
 ** New functions: AwTree_saveTree to save in file the tree information
 ** and AwTree_search, to search a node by a given pattern.
 **
 ** Revision 1.2  2001/11/01 13:06:46  kirschpi
 ** Function putChild has been changed - it now
 ** set the root node if it's NULL.
 **
 ** Revision 1.1  2001/10/31 16:50:54  kirschpi
 ** Initial revision
 **
 **
 ** --------------------------------------------------------
 */

#include "awtree.h"


/* --------------------------------------------------------
 * this function creates a new tree. 
 *
 * Parameters :
 * Return :
 *      AwTree * : the created tree
 *               (NULL if an error is produced)
 * --------------------------------------------------------
 */
extern AwTree * AwTree_new () 
{
    AwTree * tree=NULL;

    tree = (AwTree *) AW_CALLOC (1,sizeof(AwTree));
    if (tree!=NULL) 
     {
        tree->depth = 0;
        tree->maxdepth = 0;
        tree->root = NULL;
        tree->last = NULL;
     }
    return tree;
}


/* --------------------------------------------------------
 * this function removes a tree. 
 *
 * Parameters :
 *      AwTree *tree : the tree to be removed
 * Return :
 *      int : AW_OK (normally 1) if the operation succeed
 *              AW_FAILED (normally -1) if not
 * Note :
 *      This function does NOT remove the nodes in the
 *      tree (neither the root node).
 * --------------------------------------------------------
 */
extern int AwTree_delete (AwTree * tree) 
{
    if (tree!=NULL) 
     {
        AW_FREE(tree);
        return AW_OK;
     }
    return AW_FAILED;
}


/* --------------------------------------------------------
 * this function sets the root node of the tree. 
 *
 * Parameters :
 *      AwTree * tree : the tree 
 *      AwNode * node : the root node
 * Return :
 *      int : AW_OK (normally 1) if the operation succeed
 *              AW_FAILED (normally -1) if not
 * Note :
 *      This function does NOT copy the node. It only
 *      does a reference to the node.
 *      The root node may be NULL.
 * --------------------------------------------------------
 */
extern int AwTree_setRoot (AwTree *tree, AwNode * node) 
{
    if (tree!=NULL) 
     {
        tree->root = node;
        return AW_OK;   
     }
    return AW_FAILED;
}


/* --------------------------------------------------------
 * this function returns the root node of the tree. 
 *
 * Parameters :
 *      AwTree * tree : the tree
 * Return :
 *      AwNode * : the root node
 *              
 * Note :
 *      This function does NOT copy the node. It only
 *      does a reference to the node.
 *      The root node may be NULL.
 * --------------------------------------------------------
 */
extern AwNode * AwTree_getRoot (AwTree *tree) 
{
    AwNode *root = NULL;
    if (tree!=NULL)
        root = tree->root;
    return root;
}



/* --------------------------------------------------------
 * this function sets the last node of the tree. This node
 * is used by the functions putNewNode, up and down.
 * 
 * Parameters :
 *      AwTree * tree : the tree 
 *      AwNode * node : the last node
 * Return :
 *      int : AW_OK (normally 1) if the operation succeed
 *              AW_FAILED (normally -1) if not
 * Note :
 *      This function does NOT copy the node. It only
 *      does a reference to the node.
 *      The last node must NOT be NULL.
 * --------------------------------------------------------
 */
extern int AwTree_setLast (AwTree *tree, AwNode * node) 
{
    if (tree!=NULL && node!=NULL) 
     {
        tree->last = node;
        return AW_OK;
     }
    return AW_FAILED;
}


/* --------------------------------------------------------
 * this function returns the last node of the tree. 
 *
 * Parameters :
 *      AwTree * tree : the tree
 * Return :
 *      AwNode * : the last node
 *              
 * Note :
 *      This function does NOT copy the node. It only
 *      does a reference to the node.
 *      The root node may be NULL if the tree is a 
 *      empty tree.
 * --------------------------------------------------------
 */
extern AwNode * AwTree_getLast (AwTree *tree) 
{
    AwNode *last = NULL;
    if (tree!=NULL)
        last = tree->last;
    return last;
}


/* --------------------------------------------------------
 * this function introduces a new node in the tree. This 
 * node is introduced as a child node of the tree's last 
 * node.
 * 
 * Parameters :
 *      AwTree * tree : the tree 
 *      AwNode * node : the node to be added
 * Return :
 *      int : AW_OK (normally 1) if the operation succeed
 *              AW_FAILED (normally -1) if not
 * Note :
 *      This function does NOT copy the node. It only
 *      does a reference to the node.
 *      The node must NOT be NULL.
 * --------------------------------------------------------
 */
extern int AwTree_putNewNode (AwTree *tree, AwNode *node) 
{
    if (tree!=NULL && node!=NULL) 
     {
        if (tree->last != NULL) 
         {      /* tree is not empty */
            if (AwNode_putChild (tree->last,node) < 0)
                return AW_FAILED;
#ifdef DEBUG
            fprintf (stderr,"new node introduced %s\n",AwNode_getInfo(node));
#endif  
         }

        if (tree->root == NULL) 
         {
#ifdef DEBUG            
            fprintf (stderr,"root node setted\n");
#endif  
            tree->root = node;
         }    
        
        tree->last = node;
        /* tree->depth++; */
        tree->depth = AwNode_getLevel(node);
        if (tree->maxdepth < tree->depth)
            tree->maxdepth = tree->depth;
#ifdef DEBUG
        fprintf (stderr,"Last node setted to %s (depth %d)\n",AwNode_getInfo(tree->last),tree->depth);
#endif  
        return AW_OK;
     }
    return AW_FAILED;
}


/* --------------------------------------------------------
 * this function goes up in the tree. It makes the last
 * node point to its father.
 * 
 * Parameters :
 *      AwTree * tree : the tree 
 * Return :
 *      int : AW_OK (normally 1) if the operation succeed
 *              AW_FAILED (normally -1) if not
 * Note : 
 *      if the tree is empty or the last node is the 
 *      root node, then the function will failed.
 *      If this function failed, the value of last remains
 *      unchanged.
 * --------------------------------------------------------
 */
extern int AwTree_up (AwTree *tree) 
{
    if (tree!=NULL) 
     {
        if (AwNode_getFather(tree->last)!=NULL) 
         {
            tree->last = AwNode_getFather(tree->last);
            tree->depth--;
            return AW_OK;
         }
     }
    return AW_FAILED;
}


/* --------------------------------------------------------
 * this function goes down in the tree. It makes the last
 * node point to its next child.
 * 
 * Parameters :
 *      AwTree * tree : the tree 
 * Return :
 *      int : AW_OK (normally 1) if the operation succeed
 *              AW_FAILED (normally -1) if not
 * Note : 
 *      if the tree is empty or the last node was no more
 *      children, then the function will failed.
 *      
 *      Calls to AwNode_nextChild and AwNode_resetChildren
 *      in the last node WILL change the behavior of this
 *      function. Example, if you call AwNode_resetChildren
 *      in the last node just before to call this function,
 *      the new last node will be the first node child.
 *
 *      If this function failed, the value of last remains
 *      unchanged.
 * --------------------------------------------------------
 */
extern int AwTree_down (AwTree *tree) 
{
    AwNode *node=NULL;
    if (tree!=NULL) 
     {
        node = AwNode_nextChild(tree->last);
        if (node!=NULL) 
         {
            tree->last = node;
            tree->depth++;
            return AW_OK;
         }
     }
    return AW_FAILED;
}


/* --------------------------------------------------------
 * this function returns the last node depth in the tree. 
 * 
 * Parameters :
 *      AwTree * tree : the tree 
 * Return :
 *      int : last node depth value
 *              AW_FAILED (normally -1) in case of problems 
 * --------------------------------------------------------
 */
extern int AwTree_getDepth (AwTree *tree) 
{
    if (tree!=NULL)
        return tree->depth;
    return AW_FAILED;
}


/* --------------------------------------------------------
 * this function returns the max depth in the tree. 
 * 
 * Parameters :
 *      AwTree * tree : the tree 
 * Return :
 *      int : max depth value
 *              AW_FAILED (normally -1) in case of problems 
 * --------------------------------------------------------
 */
extern int AwTree_getMaxDepth (AwTree *tree) 
{
    if (tree!=NULL)
        return tree->maxdepth;
    return AW_FAILED;
}


/* --------------------------------------------------------
 * This function searchs in the tree (or a subtree), a node 
 * by its information. 
 * Parameter: AwNode *inode - initial node (the search will 
 *                             begin here). Usually, it is 
 *                             the root node. 
 *            AwString pattern - pattern to be found
 * --------------------------------------------------------        
 */ 
extern AwNode * AwTree_search (AwNode *inode, AwString pattern) 
{
    AwNode * found = NULL;

    if (inode && pattern) 
     {
        /* is inode the right node? */
        AwString info = AwNode_getInfo(inode);   
        if (AwString_str (AwNode_getInfo(inode),pattern)==AWSTRING_OK) 
            found = inode;
        AwString_delete (info);
        
        /* search inode's children */
        if (found==NULL) 
         {
            AwNode *child;
            AwNode_resetChildren (inode);
            child = AwNode_nextChild(inode);
            while (child!=NULL && found==NULL) 
             {
                found = AwTree_search (child,pattern);
                child = AwNode_nextChild(inode);
             }
            AwNode_resetChildren (inode);
         }
     }
    
    return found;
}


/* --------------------------------------------------------
 * This function saves in file the informations in tree or
 * subtree indicated by the node.
 * 
 * Parameters:
 *     AwNode * n : "root" node from the tree (or subtree)
 *                  that should be save
 *     File * out : file where the information should be 
 *                  save
 * 
 * --------------------------------------------------------
 */
extern void AwTree_saveTree (FILE *out, AwNode *n) 
{
    AwNode *child;
    AwPair *atr;
    int i;

    if (n==NULL) return; 
 
    /* node's info */
    for (i = 0; i < AwNode_getLevel(n); i++)  
        fprintf (out,"\t");
    fprintf (out,"%c %s\n",(AwNode_howManyChildren(n)>0)?'+':' ', \
                            AwNode_getInfo(n));
    
    /* node's attributes */
    AwNode_resetAttributes(n);
    atr = AwNode_nextAttribute(n);
    while (atr!=NULL) 
     {
        for (i = 0; i < AwNode_getLevel(n); i++)  
            fprintf (out,"\t");
        fprintf (out,"%s=\'%s\'\n",AwPair_getName(atr),AwPair_getValue(atr));
        atr = AwNode_nextAttribute(n);
     }
    
    
    /* node's children */
    fflush (out);
    AwNode_resetChildren (n);
    child = AwNode_nextChild(n);
    while (child!=NULL) 
     {
        AwTree_saveTree(out,child);
        child = AwNode_nextChild(n);
     }

    /* closing node's element */
    if (AwNode_howManyChildren(n)>0) 
     {
        for (i = 0; i < AwNode_getLevel(n); i++)  
            fprintf (out,"\t");
        fprintf (out,"- %s\n",AwNode_getInfo(n));
     }
        
}



