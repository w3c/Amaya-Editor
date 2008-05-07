/* --------------------------------------------------------
 ** An interface for string manipulation. 
 **
 ** Author : Manuele Kirsch Pinheiro
 ** Project CEMT - II/UFRGS - Opera Group/INRIA
 ** Finance by CNPq(Brazil)/INRIA(France) Cooperation
 **
 ** Date : 25 / Oct / 2001
 **
 ** $Id$
 ** $Date$
 ** $Author$
 ** $Revision$
 ** $Log$
 ** Revision 1.3  2008-05-07 13:49:08  kia
 ** char* => const char*
 ** (Regis patches + many many others)
 **
 ** Revision 1.2  2002/06/12 10:30:06  kirschpi
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
 ** Revision 1.6  2002/03/27 17:07:55  kirschpi
 ** Added AwString_str function (a version of strstr for AwString objects).
 **
 ** Revision 1.5  2001/11/06 14:07:59  kirschpi
 ** Functions new, set, get and cat have been changed
 ** to prevent garbage in the string.
 **
 ** Revision 1.4  2001/11/06 10:00:02  kirschpi
 ** "i" variable - unused - removed from set function.
 **
 ** Revision 1.3  2001/10/30 10:51:17  kirschpi
 ** Corrections in new function have been made.
 ** The calloc parameters weren't approprieted.
 **
 ** Revision 1.2  2001/10/26 14:24:37  kirschpi
 ** AwString_new function have beem corrected. The loca variable
 ** "nstr" is now assigned as NULL in its declaration.
 **
 ** Revision 1.1  2001/10/26 13:11:03  kirschpi
 ** Initial revision
 **
 ** --------------------------------------------------- 
 */

#include "awstring.h"

/* --------------------------------------------------------
 * this function creates a new string with the initial 
 * size indicated by the parameter "size".
 *
 * Parameters : 
 *      int size : string initial size
 * Return :
 *      AwString : the new string created (or NULL if
 *                 it's not possible to create it)
 * ----------------------------------------------------                    
 */
extern AwString AwString_new (int size) 
{
    AwString nstr=NULL;
#ifdef DEBUG            
         fprintf (stderr,"DEBUG : alloc size %d x %d ",size,sizeof(AwStringElement));
#endif              
    nstr = (AwString) AWSTRING_CALLOC (size+1,sizeof(AwStringElement));

#ifdef DEBUG            
    fprintf (stderr,"OK\n");
#endif              
    return nstr;
}


/* --------------------------------------------------------
 * this function deletes the string indicated by the
 * parameter "string".
 *
 * Paramaters :
 *      AwString string : string to be deleted
 * Returns :
 *      int : AWSTRING_OK (normally 1), if the operation succeed
 *            AWSTRING_FAILED, if not.
 *      
 * ---------------------------------------------------
 */ 
extern int AwString_delete (AwString string) 
{
    if (string == NULL) 
        return AWSTRING_FAILED;
    
    AWSTRING_FREE (string);
    return AWSTRING_OK;
}

/* --------------------------------------------------------
 * this function set the content of a string, based on the 
 * content of a array of char.
 *
 * Parameters :
 *      AwString string : string that will be set
 *      char *text : content for the string 
 * Returns :
 *      int : AWSTRING_OK (normally 1), if the operation succeed
 *            AWSTRING_FAILED, if not.
 * Note :
 *      String should be greater than text (at least, the
 *      text size + 1)
 * ------------------------------------------------------
 */ 
extern int AwString_set (AwString string, const char *text) 
{
    int size;
    int ret = AWSTRING_FAILED;
    char *s;

    if (string != NULL && text != NULL) 
     {
#ifdef DEBUG            
         fprintf (stderr,"DEBUG : setting string as %s\n",text);
#endif              
        size = strlen (text);
        if (size>0) 
         {
            s = strncpy ((char *) string,text,size); 
            s[size]='\0'; 
            if (s == (char *) string)
                    ret = AWSTRING_OK;
         }                
     }
    return ret;
}



/* ----------------------------------------------------
 * this function returns a new pointer to a char
 * array with the same value of  string (it is not
 * a pointer to the string itself, but to a new area 
 * wih the same char equivalent content).
 * 
 * Parameters :
 *      AwString : the string to be copied
 * Return :
 *      char *: pointer to the new char array 
 *              (NULL if the operation failed)
 * --------------------------------------------------   
 */
extern char * AwString_get ( AwString string ) 
{
    char *narray;
    int size;

    if (string != NULL) 
     {
        size = AwString_len (string);
        narray = (char *) AWSTRING_CALLOC (size+1,sizeof(char *));
        if (narray!=NULL) 
         {
            strcpy (narray,(char *) string);
            narray[size]='\0';
            return (narray);
         }       
     }
    return NULL;
}



/* --------------------------------------------------------
 * this function copy the content of a string to another. 
 * Parameters :
 *      AwString dst : destiny string 
 *      AwString src : source string 
 * Returns :
 *      int : AWSTRING_OK (normally 1), if the operation succeed
 *            AWSTRING_FAILED, if not.
 * ------------------------------------------------------
 */ 
extern int AwString_copy (AwString dst, AwString src) 
{
    char *s;
    
    if (dst!=NULL && src!=NULL) 
     {
        s = strcpy ( (char *) dst, (char *)src);
        if (s == (char *) dst )
            return AWSTRING_OK; 
     }
    return AWSTRING_FAILED;
}

/* --------------------------------------------------------
 * this function compares the content of two strings.
 * Parameters :
 *      AwString str1 : first string to be compared
 *      AwString str2 : second string to be compared
 * Returns :
 *      int : AWSTRING_OK (normally 1), if the strings matches
 *            AWSTRING_FAILED, if not.
 * ------------------------------------------------------
 */ 
extern int AwString_comp (AwString str1, AwString str2) 
{
    int match;
    
    if (str1!=NULL && str2!=NULL) 
     {
        match = strcmp ((char *) str1, (char *) str2);
        if (match == 0)
            return AWSTRING_OK; 
     }
    return AWSTRING_FAILED;
}

/* --------------------------------------------------------
 * this function searchs a substring in a string.
 * Parameters :
 *      AwString str : string 
 *      AwString sub : substring
 * Returns :
 *      int : AWSTRING_OK (normally 1), if the strings matches
 *            AWSTRING_FAILED, if not.
 * ------------------------------------------------------
 */ 
extern int AwString_str (AwString str, AwString sub) 
{
    char * match = NULL;
    
    if (str!=NULL && sub!=NULL) 
     {
        match = strstr ((char *) str, (char *) sub);
        if (match != NULL)
            return AWSTRING_OK; 
     }
    return AWSTRING_FAILED;
}



/* --------------------------------------------------------
 * this function concatenates the content of two strings.
 * Parameters :
 *      AwString str1 : first string to be concatenated
 *      AwString str2 : second string to be concatenated
 * Returns :
 *      AwString : the new string produced
 *                 (NULL if an error is produced)
 * The strings "str1" and "str2" will not be changed!
 * ------------------------------------------------------
 */ 
extern AwString AwString_cat (AwString str1, AwString str2) 
{
    AwString nstr=NULL;
    int size = 0;

    if (str1!=NULL && str2!=NULL) 
     {
        size = strlen ((char *) str1);
        size += strlen ((char *) str2); 
        if (size > 0) 
         {
            nstr = AwString_new (size+1);
            if (nstr!=NULL) 
             {
                strcat ((char *) nstr, (char *) str1);      
                strcat ((char *) nstr, (char *) str2);
                nstr[size]='\0';
             }
         }
     }
    return nstr;
}



/* --------------------------------------------------------
 * this function return the lenghr of the string (number
 * of characters).
 * Parameters :
 *      AwString str : string to be "counted"
 * Returns :
 *     int : lenght of the string 
 *           ( < 0 if an error is produced )
 * ------------------------------------------------------
 */
extern int AwString_len (AwString str) 
{
    int size=AWSTRING_FAILED;

    if (str!=NULL) 
    {
        size = strlen ((char *) str);
    }
    return size;
}

/* ----------------------------------------------------
 * this function converts the string into a double 
 * value. 
 * 
 * Parameters :
 *      AwString : the string to be converted
 * Return :
 *      int : double value of the string
 * --------------------------------------------------   
 */
extern double AwString_tof (AwString string) 
{
    double d = 0;
    if (string!=NULL) 
        d = atof ((char *) string);
    return d;
}
