/* --------------------------------------------------------
 ** Header file for an interface to string manipulation 
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
 ** Revision 1.2  2008-05-07 13:49:08  kia
 ** char* => const char*
 ** (Regis patches + many many others)
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
 ** Revision 1.4  2002/03/27 17:08:24  kirschpi
 ** Added AwString_str function (a version of strstr for AwString objects).
 **
 ** Revision 1.3  2001/10/30 10:53:31  kirschpi
 ** A new define - AwStringElement - was defined.
 ** It describe one string unit element.
 **
 ** Revision 1.2  2001/10/26 13:06:18  kirschpi
 ** Adding new RCS comments
 **
 ** Revision 1.1  2001/10/26 12:57:55  kirschpi
 ** Initial revision
 **
 ** --------------------------------------------------- 
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef AWSTRING_H
#define AWSTRING_H

#define AWSTRING_OK 1      		/* return codes */
#define AWSTRING_FAILED -1

                           		/* memory functions */
#define AWSTRING_CALLOC(count,size)	calloc((count),(size))
#define AWSTRING_FREE(ptr)	free((ptr))
		

#define AwString AwString_t		/* string definition */
#define AwStringElement AwStringElement_t
typedef char * AwString_t;
typedef char AwStringElement_t;

extern AwString AwString_new (int size);              		/* creates a new string */
extern int AwString_delete (AwString string);         		/* deletes a string */
extern int AwString_set (AwString string, const char *text);		/* sets a string's content */
extern int AwString_copy (AwString dst, AwString src);		/* copy the content of  one string to another */
extern int AwString_comp (AwString str1, AwString str2);   	/* compare the content of two strings */
extern int AwString_str (AwString str, AwString sub);     	/* searchs a substring in a string */
extern AwString AwString_cat (AwString str1, AwString str2);	/* concatenates the content of two strings */
extern int AwString_len (AwString str);                    	/* returns the string's size */
extern char * AwString_get (AwString string);           	/* copies the string content a new char array */
extern double AwString_tof (AwString string);           	/* converts the string in a double value */


#endif

