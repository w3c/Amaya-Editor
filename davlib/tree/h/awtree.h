/* --------------------------------------------------------
 ** Header file for a simple tree implementatio.
 **
 ** Author : Manuele Kirsch Pinheiro
 ** Project CEMT - II/UFRGS - Opera Group/INRIA
 ** Finance by CNPq(Brazil)/INRIA(France) Cooperation
 **
 ** Date : 26 / Oct / 2001
 **
 ** $Id$
 ** $Date$
 ** $Author$
 ** $Revision$
 ** $Log$
 ** Revision 1.2  2002-06-17 10:04:48  cheyroul
 ** Win32 : calloc and free bad re-prototyping correction
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
 ** Revision 1.8  2002/03/28 11:00:57  kirschpi
 ** New functions added. For the AwNode structure, there
 ** are two new functions: AwNode_howManyAttributes and
 ** AwNode_howManyChildren, to indicate the number of
 ** attributes and children in the node. For the AwTree
 ** structure, there are also two functions: AwTree_saveTree,
 ** to save the tree in a file, and AwTree_search, to search
 ** a node by a pattern information.
 **
 ** Revision 1.7  2001/11/02 13:46:00  kirschpi
 ** Functions to manipulate the node type have been introduced.
 **
 ** Revision 1.6  2001/11/01 09:47:20  kirschpi
 ** Type identifier has been added in AwNode structure.
 **
 ** Revision 1.5  2001/10/31 16:49:02  kirschpi
 ** Functions to manipulate the tree implementation.
 **
 ** Revision 1.4  2001/10/31 09:40:31  kirschpi
 ** All functions for node manipulation have been added.
 ** This functions are implemented in awnode.c
 **
 ** Revision 1.3  2001/10/30 10:54:41  kirschpi
 ** New functions, about node implementation, have been added.
 **
 ** Revision 1.2  2001/10/26 16:07:44  kirschpi
 ** The initial version already include functions for
 ** the node's attributes (name/value pairs) manipulation.
 ** Small comments included.
 **
 ** Revision 1.1  2001/10/26 16:04:23  kirschpi
 ** Initial revision
 **
 * --------------------------------------------------------
 */

#include <stdlib.h>             /* necessary includes */
#include <stdio.h>

#ifndef AWTREE_H
#define AWTREE_H

#include "list.h"
#include "awstring.h"


#define AW_OK 1     		/* macros with return codes values */
#define AW_FAILED -1

				/* macros para alocacao de memoria */
#define AW_CALLOC(count,size)   calloc((count),(size))
#define AW_FREE(ptr)  free((ptr))


typedef struct _AwPair {		/* name/value pair used for attibutes purposes */
    AwString name;
    AwString value;
} AwPair;



typedef struct _AwNode {		/* a tree node */
    AwString info;      			/* node's information */
    int infosize;       			/* node's string information allocated size */
    AwList *children;     			/* node's children */
    AwList *attributes;   			/* node's attributes */
    int  level;         			/* node's level in the tree */
    int  type;             			/* node's type identifier */
    struct _AwNode *father;    			/* node's antecessor (father) */
} AwNode;


typedef struct _AwTree {    		/* a tree structure */
    int depth;           			/* tree's last node depth */
    int maxdepth;         			/* tree's maximum depth */
    AwNode * root;        			/* tree's root node */
    AwNode * last;       			/* last node introduced in the tree */
} AwTree; 



					/* pair manipulation functions - implemented at awpair.c */
extern AwPair* AwPair_new (int namesize, int valuesize);	/* creates a new name/value pair */
extern int AwPair_delete (AwPair *pair);              	 	/* deletes a name/value pair pointer */
extern int AwPair_setName (AwPair *pair, AwString name); 	/* sets the name attribute from the pair  */
extern AwString AwPair_getName (AwPair *pair);          	/* gets the name attribute from the pair */
extern int AwPair_setValue (AwPair *pair, AwString value);	/* sets the value attribute from the pair */
extern AwString AwPair_getValue (AwPair *pair);           	/* gets the value attribute from the pair */

					/* node manipulation functions - implemented at awnode.c */
extern AwNode* AwNode_new (int nchildren, int nattrib, int infosize);  	/* creates a new node */
extern int AwNode_delete (AwNode *node);                               	/* removes the node pointer */
extern int AwNode_putChild (AwNode *node, AwNode *child);           	/* inserts a new child node */
extern AwNode * AwNode_nextChild (AwNode *node);                     	/* returns the next child */
extern void AwNode_resetChildren (AwNode *node);                  	/* resets the next child element index */
extern int AwNode_putAttribute (AwNode *node, AwPair *pair);     	/* inserts a new attribute in the node */
extern AwPair * AwNode_nextAttribute (AwNode *node);                	/* returns the next node attribute */
extern void AwNode_resetAttributes (AwNode *node);                	/* resets the next attribute element index */
extern int AwNode_setInfo (AwNode *node, AwString info);             	/* sets the node information string */
extern AwString AwNode_getInfo (AwNode *node);                    	/* returns the node information string */
extern int AwNode_setLevel (AwNode *node, int level);              	/* sets the node level */
extern int AwNode_getLevel (AwNode *node);                       	/* returns the node level */
extern int AwNode_setType (AwNode *node, int type);              	/* sets the node type */
extern int AwNode_getType (AwNode *node);                       	/* returns the node type */
extern int AwNode_setFather (AwNode *node, AwNode *father);        	/* set the node's father */
extern AwNode * AwNode_getFather (AwNode *node);                	/* get a pointer to the node's father */

extern int AwNode_howManyAttributes (AwNode *node);             	/* gets the number of attributes */
extern int AwNode_howManyChildren (AwNode *node);               	/* gets the number of children */


						/* tree manipulation functions - implemented at awtree.c */              
extern AwTree * AwTree_new ();                          	/* creates a new empty tree */  
extern int AwTree_delete (AwTree * tree);               	/* removes a tree pointer */
extern int AwTree_setRoot (AwTree *tree, AwNode * node);   	/* sets the tree root node */
extern AwNode * AwTree_getRoot (AwTree *tree);          	/* returns the tree root node */
extern int AwTree_setLast (AwTree *tree, AwNode * node);	/* sets the last node in the tree */
extern AwNode * AwTree_getLast (AwTree *tree);          	/* returns the last introduced node in the tree */
extern int AwTree_putNewNode (AwTree *tree, AwNode *node);	/* introduces a new node in the tree */
extern int AwTree_up (AwTree *tree);                           	/* goes up in the tree */
extern int AwTree_down (AwTree *tree);                         	/* goes down in the three */
extern int AwTree_getDepth (AwTree *tree);               	/* returns the last node depth value */
extern int AwTree_getMaxDepth (AwTree *tree);           	/* return the tree max depth value */
extern AwNode * AwTree_search (AwNode *inode, AwString pattern);/* searchs a node by its information */
extern void AwTree_saveTree (FILE *out, AwNode *n);     	/* saves in file the informations in tree */

#endif
 
