/* ----------------------------------------------------------- 
** Functions to manipulate the local WebDAV locks base.
**
** Author : Manuele Kirsch Pinheiro 
** Email: Manuele.Kirsch_Pinheiro@inrialpes.fr / manuele@inf.ufrgs.br
** Project CEMT - II/UFRGS - Opera Group/INRIA
** Financed by CNPq(Brazil)/INRIA(France) Cooperation
**
**
** ----------------------------------------------------------- 
*/

#ifndef AHTLOCKBASE_H
#define AHTLOCKBASE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "wwwsys.h"

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

#include "HTList.h"
#include "HTAssoc.h"
#include "HTMemory.h"
#include "HTString.h"
#include "HTWWWStr.h"

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

   
#include "awparser.h"

/* thotlib header for TtaFileUnlink, a portable version
 * of unlink */
#include "sysdep.h"

#define  BREAK_CHAR    '\t'
#define  DAV_LINE_MAX  1024

#ifndef _WINGUI
#define PORT_CHAR      ':'
#else
#define PORT_CHAR      '%' 
#endif /* _WINGUI */


/*----------------------------------------------------------------------
   Static (global) variable used to create the lock base
  ----------------------------------------------------------------------*/

PUBLIC char     	DAVHome[DAV_LINE_MAX];


/* ------------------------------------------------------------------ 
 * This function will that date like this "Fri Mar 29 18:36:47 2002" 
 * and convert it in a time_t.
 * Note: this fate format is the output from ctime, and we hope
 *       that month names will be in english
 * ------------------------------------------------------------------       
 */
PUBLIC time_t strtotime (char *time);


/* -------------------------------------------------------------
 * This function creates a new date string with the format
 * "Fri Mar 29 18:36:47 2002" from the time_t parameter.
 * -------------------------------------------------------------
 */ 
PUBLIC char * timetostr (time_t t);



/* ----------------------------------------------------------- 
 * Informations about a lock found in any line of 'lock 
 * base' files. These informations concern about WebDAV locks 
 * demanded by the user. 
 * ----------------------------------------------------------- */
typedef struct _LockLine {
    char * relativeURI;
    char * lockToken;
    char   depth;
    char * timeout;
    char * initialTime;
} LockLine;


/* ----------------------------------------------------------- 
 * Create a LockLine object with the informations found in
 * line string. This string should have the 'lock base' format.
 * 
 * That is :
 * LINE	::= URI SEP LOCK SEP DEPTH SEP TIME SEP ITIME
 * URI	::= relative-uri 	;example: /foo/bar/file.html or /foo
 * SEP 	::= \t          	;separator - a tab char
 * LOCK	::= lock-token  	;as defined in RFC2518
 * TIME ::= timeout     	;as defined in RFC2518
 * ITIME::= initial-time	
 *
 * Returns : a new LockLine * object, if succeed, NULL if don't.
 *            
 * ----------------------------------------------------------- */
PUBLIC LockLine * LockLine_new (const char * aline);


/* ----------------------------------------------------------- 
 * Creates a new LockLine object. It's an alternative function
 * to create these objects.
 *
 * Parameters :
 *     char * relativeURI :  relative URI
 *     char * locktoken   :  lock-token header
 *     char * depth       :  depth value
 *     char * timeout     :  timeout value
 *     time_t itime       :  initial time value
 * ----------------------------------------------------------- */
PUBLIC LockLine * LockLine_newObject (char * relativeURI, char * locktoken, \
                                      char * depth, char * timeout, time_t itime);


/* ----------------------------------------------------------- 
 * Deletes a LockLine object. 
 * ----------------------------------------------------------- */
PUBLIC BOOL LockLine_delete (LockLine *me);


/* ----------------------------------------------------------- 
 * Creates a string to be used in an If header.
 * Format: "<http://filename/relative-uri> (lock-token)"
 * ----------------------------------------------------------- */
PUBLIC char * makeIfItem (const char * filename, char * relUri, char *lockToken); 


/* ----------------------------------------------------------- 
 * Gets a line from the file pointed by 'fp' parameter.
 * Returns: a new LockLine * object or NULL, if it reach EOF
 *          with no more data. 
 * ----------------------------------------------------------- */
PUBLIC LockLine * LockLine_readline (FILE *fp);



/* ----------------------------------------------------------- 
 * Writes in file a new line with the informations in LockLine 
 * object.
 * Return: BOOL YES if it succeed, NO, if it doesn't. 
 * ----------------------------------------------------------- */
PUBLIC BOOL LockLine_writeline (FILE *fp, LockLine *line);


/* ----------------------------------------------------------- 
 * Process a file from 'lock base'. It searchs locks that 
 * should affect the relative URI in 'reqUri' parameter and
 * returns a list of LockLine objects with the lines that
 * matches in filename. 
 *
 * Returns : an HTList with LockLine objects. This list may be 
 *           empty or NULL.
 *
 * Note: the caller must delete all LockLine objects after
 *       use them. 
 *
 * ----------------------------------------------------------- */
PUBLIC HTList * processLockFile (const char * filename, const char * reqUri);


/* -----------------------------------------------------------
 * this functions searchs the locks in "filename" that matches
 * or affect an operation over the resource "reqUri". It test
 * the lock timeout in the file, and only returns matches that
 * are still valids. 
 * 
 * Returns: a list of "tagged lists" to be used in a If header
 * (format "<http://filename/uri> (<lock-token>)" ).
 * ----------------------------------------------------------- */
PUBLIC HTList * searchLockBase ( char * filename,  char * reqUri);


/* -----------------------------------------------------------
 * This function creates the content for a If header based in 
 * a HTlist of "tagged list" string, like the list returned by 
 * the function searchLockBase.
 * ----------------------------------------------------------- */
PUBLIC char * mountIfHeader (HTList *if_list);

/* ---------------------------------------------------------------------------
 * This function separates the host name and the relative part
 * from a URL.
 *
 * Parameters:
 *     const char *  URI : request URI 
 *     const char *  localFQDN : local hostname with full qualify domain name
 *     char ** hostname : char pointer for return purposes
 *     char ** relative : char pointer for return purposes
 * Returns:
 *     BOOL :  YES if it succeed, NO if fails.
 *     char ** hostname : if succeed, this pointer should contain the complete
 *                        hostname (with domain name) of the host in URI
 *     char ** relative : if succeed, this pointer should contain the relative
 *                        URI used in URI      
 * -------------------------------------------------------------------------- */

PUBLIC BOOL separateUri (const char *URI, const char *localFQDN, \
                         char ** hostname, char **relative);

/* ---------------------------------------------------------------------------
 * This function process the lock information in the parameters and returns
 * a new LockLine object with these informations.  
 * 
 * Returns: LockLine * object or NULL, if failed. 
 *  -------------------------------------------------------------------------- */
PUBLIC LockLine *processLockInfo (char *relative, AwTree *xmlbody, HTAssocList *headers);


/* ---------------------------------------------------------------------------
 * This function saves the LockLine object in LockBase
 *
 * Parameters:
 * 	char * absolute: the request hostname with domain name and port number
 * 	LockLine * lockinfo: informatio to be saved
 *
 * Returns: BOOL YESS if it succeed, NO if it doesn't.
 *  -------------------------------------------------------------------------- */
PUBLIC BOOL saveLockLine (char *absolute, LockLine *lockinfo);


/* ---------------------------------------------------------------------------
 * This function saves the lock information present in xmlbody in the LockBase
 *
 * Parameters:
 *     char * absolute: the request hostname with domain name and port number
 *     char * relative: the relative URI (collections should terminate
 *                            with a slash "/")
 *     char * xmlbody: XML response body from lock request
 *     HTAssocList * headers: response headers
 *     
 * Returns: BOOL YES if it suceed, NO if it doesn't.
 *  -------------------------------------------------------------------------- */
PUBLIC BOOL saveLockBase (char *absolute, char *relative, char *xmlbody, HTAssocList *headers);


/* ---------------------------------------------------------------------------
 * This function removes the lock information in LockLine object from the base.
 * Returns: BOOL YES it it succed, NO if it doesn't.
 * --------------------------------------------------------------------------- */
PUBLIC BOOL removeFromBase (char *filename, LockLine *line);



#endif
