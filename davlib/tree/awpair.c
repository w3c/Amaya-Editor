/* --------------------------------------------------------
 ** A name/value pair implementation for use as a tree's 
 ** node attribute.
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
 ** Revision 1.4  2001/10/30 10:52:32  kirschpi
 ** Corrections in new function have been made.
 ** The calloc parameters weren't approprieted.
 **
 ** Revision 1.3  2001/10/29 11:01:17  kirschpi
 ** New comment about the delete function added.
 **
 ** Revision 1.2  2001/10/26 16:10:26  kirschpi
 ** Only first comments were changed
 **
 ** Revision 1.1  2001/10/26 16:03:31  kirschpi
 ** Initial revision
 **
 ** --------------------------------------------------------
 */

#include "awtree.h"


/* --------------------------------------------------------
 * this function creates a new pair name/value, with the 
 * size indicated by the parameters.
 *
 * Parameters :
 *      int namesize : name's size
 *      int valuesize : value's size
 * Return :
 *      AwPair : the created pair
 *               (NULL if an error is produced)
 * --------------------------------------------------------
 */
extern AwPair * AwPair_new (int namesize, int valuesize) 
{
    AwPair * pair = NULL;
    
    pair = (AwPair *) AW_CALLOC (1,sizeof(AwPair));
    if (pair!=NULL) 
     {
        pair->name = AwString_new (namesize);
        pair->value = AwString_new (valuesize);
        if (pair->name == NULL || pair->value == NULL) 
         {
            AW_FREE (pair);
            pair=NULL;
         }
     }
    return pair;
}


/* --------------------------------------------------------
 * this function deletes a pair name/value.
 *
 * Parameter : 
 *      AwPair pair : pair to be removed
 * Return :
 *      int : AW_OK (normally 1) if the operation succeed
 *            AW_FAILED (normally -1) if not
 * Note :
 *      this function also deletes the name and value 
 *      strings used in this pair.
 * --------------------------------------------------------
 */
extern int AwPair_delete (AwPair *pair) 
{
    int ret=AW_OK;
    if (pair!=NULL) 
     {
        ret *= AwString_delete (pair->name);
        ret *= AwString_delete (pair->value);
        AW_FREE(pair);
     } 
    else ret = -1;
    return ret;
}

/* --------------------------------------------------------
 * this function set the name attribute of the pair 
 * indicated by the parameter "pair".
 *
 * Parameter : 
 *      AwPair pair : pair to be removed
 *      AwString name : pair's name
 * Return :
 *      int : AW_OK (normally 1) if the operation succeed
 *            AW_FAILED (normally -1) if not
 * --------------------------------------------------------
 */
extern int AwPair_setName (AwPair *pair, AwString name) 
{
    AwString str;
    
    if (pair!=NULL && name != NULL) 
     {
        if ( AwString_len(pair->name) < AwString_len (name)) 
         {
            str = AwString_new (AwString_len(name));
            if (str == NULL)
                return AW_FAILED;
            AwString_delete (pair->name);
            pair->name = str;
         } 
        if (AwString_copy (pair->name,name) > 0)
                return AW_OK;
     }
    return AW_FAILED;
}


/* --------------------------------------------------------
 * this function returns the name attribute of the pair 
 * indicated by the parameter "pair".
 *
 * Parameter : 
 *      AwPair pair : pair to be removed
 * Return :
 *      AwString : name attribute for the indicated pair
 *                 (NULL if some error is produced)
 * Note :
 *      It returns a reference to the name - Pay Attention!
 * --------------------------------------------------------
 */
extern AwString AwPair_getName (AwPair *pair) 
{
    AwString str=NULL;    
    if (pair!=NULL) 
     {
        str = pair->name;
     }
    return str;
}


/* --------------------------------------------------------
 * this function set the value attribute of the pair 
 * indicated by the parameter "pair".
 *
 * Parameter : 
 *      AwPair pair : pair to be removed
 *      AwString value : pair's value
 * Return :
 *      int : AW_OK (normally 1) if the operation succeed
 *            AW_FAILED (normally -1) if not
 * --------------------------------------------------------
 */
extern int AwPair_setValue (AwPair *pair, AwString value) 
{
    AwString str;
    
    if (pair!=NULL && value != NULL) 
     {
        if ( AwString_len(pair->value) < AwString_len (value)) 
         {
            str = AwString_new (AwString_len(value));
            if (str == NULL)
                return AW_FAILED;
            AwString_delete (pair->value);
            pair->value = str;
         } 
        if (AwString_copy (pair->value,value) > 0)
                return AW_OK;
     }
    return AW_FAILED;
}



/* --------------------------------------------------------
 * this function returns the value attribute of the pair 
 * indicated by the parameter "pair".
 *
 * Parameter : 
 *      AwPair pair : pair to be removed
 * Return :
 *      AwString : the value attribute for the indicated pair
 *                 (NULL if some error is produced)
 * Note :
 *      It returns a reference to the value - Pay Attention!
 * --------------------------------------------------------
 */
extern AwString AwPair_getValue (AwPair *pair) 
{
    AwString str=NULL;    
    if (pair!=NULL) 
     {
        str = pair->value;
     }
    return str;
}


