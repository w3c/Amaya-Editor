/* --------------------------------------------------------
 ** Header file for awareness protocol parser functions.
 ** This functions are implemented in awparser.c.
 **
 ** Author : Manuele Kirsch Pinheiro
 ** Project CEMT - II/UFRGS - Opera Group/INRIA
 ** Finance by CNPq(Brazil)/INRIA(France) Cooperation
 **
 ** Date : 02 / Nov / 2001
 **
 ** $Id$ 
 ** $Date$
 ** $Author$
 ** $Revision$
 ** $Log$
 ** Revision 1.3  2008-05-13 09:30:27  kia
 ** More char* fixes
 **
 ** Revision 1.2  2002/06/03 14:37:44  kirschpi
 ** The name of some public functions have been changed to avoid conflic with
 ** other libraries.
 **
 ** Revision 1.1  2002/05/31 10:48:46  kirschpi
 ** Added a new module for WebDAV purposes _ davlib.
 ** Some changes have been done to add this module in the following files:
 ** amaya/query.c, amaya/init.c, amaya/answer.c, amaya/libwww.h, amaya/amayamsg.h,
 ** amaya/EDITOR.A, amaya/EDITORactions.c, amaya/Makefile.libwww amaya/Makefile.in,
 ** config/amaya.profiles, tools/xmldialogues/bases/base_am_dia.xml,
 ** tools/xmldialogues/bases/base_am_dia.xml, Makefile.in, configure.in
 ** This new module is only activated when --with-dav options is used in configure.
 **
 ** Revision 1.2  2002/03/28 16:20:32  kirschpi
 ** New function AwParser_searchInTree that searchs in the
 ** tree the value of an given element (thinking in XML
 ** elements, the value will be its child).
 **
 ** Revision 1.1  2001/11/02 16:32:17  kirschpi
 ** Initial revision
 **
 ** 
 * --------------------------------------------------------
 */

#ifndef AWPARSER_H
#define AWPARSER_H

#include <ctype.h>

#include "xmlparse.h"
#include "awtree.h"

#define NUM_OF_NODES 3			/* estimative - number of children nodes per node */
#define XML_ELEMENT  1			/* identifier for nodes which contain XML elements */
#define TEXT_ELEMENT 2			/* identifier for nodes which contain text elements */

#ifndef MESSAGE_SIZE
#define MESSAGE_SIZE 2048		/* message protocol max size */
#endif


extern int verifyString (char *data);                                            	/* returns the number of non-space characters*/
extern AwNode *AwParser_makeNode (char *data, char **attr,int len, int type);    	/* creates a new node */
extern void AwParser_textElement (void *userData,const XML_Char *s,int len);          	/* parser text elements */
extern void AwParser_startElement(void *userData, const char *name, const char **attr);	/* parser start elements */
extern void AwParser_endElement(void *userData, const char *name);                     	/* parser end elements */
extern void AwParser_freeTree (AwTree *tree);                                         	/* deletes the tree and all its nodes */
extern void AwParser_freeNode (AwNode *n);                                             	/* deletes the node and its children */
extern AwString AwParser_mountMessage (AwTree *tree);                                 	/* mounts a protocol message */
extern AwString AwParser_mountElement (AwNode *node);                                  	/* mounts a xml string with the node data */
extern AwTree *AwParser_umountMessage (AwString message);                             	/* "umounts" a awareness protocol message */
extern char * AwParser_searchInTree (AwTree *tree, const char *elem);                            	/* searches the value of an element in a XML tree */

#endif
