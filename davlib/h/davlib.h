/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*  --------------------------------------------------------
 ** 
 ** File: davlib.h - WebDAV module
 **
 ** Header file for WebDAV module. It contains defines,
 ** typedefs and structs used by WebDAV functions (implemented
 ** at davlib.c).
 **
 ** Author : Manuele Kirsch Pinheiro (Manuele.Kirsch_Pinheiro@inrialpes.fr)
 ** Project CEMT - II/UFRGS - Opera Group/INRIA
 **    CNPq(Brazil)/INRIA(France) Cooperation
 **
 ** Date : April / 2002
 **
 ** $Id$
 ** $Date$
 ** $Log$
 ** Revision 1.11  2009-06-10 10:57:23  vatton
 ** Change the management of Templates list
 ** + Implementation of a new WebDAV list of sites
 ** + Fix problems with lock/unlock status
 ** Irene
 **
 ** Revision 1.10  2009/04/23 14:51:36  vatton
 ** Improving the WebDAV interface
 ** Irene
 **
 ** Revision 1.9  2005/06/07 13:37:11  gully
 ** code cleaning + warning fixes
 ** S. GULLY
 **
 ** Revision 1.8  2003/11/19 12:33:16  gully
 ** Compilation fix (webdav support)
 **
 ** S. GULLY
 **
 ** Revision 1.7  2003/05/19 11:39:47  vatton
 ** Moving the variable DAVLibEnable.
 ** Irene
 **
 ** Revision 1.6  2002/07/01 10:34:16  kahan
 ** JK: Enabling/Disabling DAV support by means of the new DAV_Enable
 ** registry entry.
 ** Removed the DAV menu from the standard profile.
 **
 ** Revision 1.5  2002/06/13 16:11:16  kirschpi
 ** New dialogue "WebDAV Preferences"
 ** Corrections due last commit by JK
 ** Manuele
 **
 ** Revision 1.4  2002/06/13 13:40:33  kahan
 ** JK: Changed MAX_LINE to DAV_MAX_LINE. MAX_LINE is a reserved macro and
 ** the code was generating a warning.
 **
 ** Revision 1.3  2002/06/05 16:46:52  kirschpi
 ** Adding to the registry a DAVResource list, a list of resources
 ** (specially collections), where we should do a Lock discovery.
 ** Manuele
 **
 ** Revision 1.2  2002/05/31 18:01:09  kirschpi
 ** Functions to give to user some informations about active locks
 ** (a basic awareness support) when the user load or exit a document.
 **
 * -------------------------------------------------------- 
 */

#ifndef DAVLIB_H
#define DAVLIB_H

#include "string.h"             /* Thot stuff */
#include "thot_gui.h"
#include "thot_sys.h"
#include "application.h"
#include "appstruct.h"
#include "interface.h"

#include "amaya.h"              /* amaya declarations, as DocumentURLs array
                                   and include files like #include "libwww.h" */

/* header files used by AHTDAVContext struct */
#include "awtree.h"             /* AwTree struct, for tree element in AHTDAVContext */

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */
   
#include "HTChunk.h"            /* for the output/debug elements in AHTDAVContext */
#include "HTReq.h"              /* for oldRequest element in AHTDAVContext */
#include "HTDAV.h"              /* WebDAV module in libwww */
#include "HTHome.h"

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#undef PUBLIC
#define PUBLIC extern
#include "AHTLockBase.h"        /* "base" for lock tokens */
#undef PUBLIC
#define PUBLIC



/*----------------------------------------------------------------------
   Definitions/global variables used in WebDAV functions
  ----------------------------------------------------------------------*/

/* 
 * XML body max. length 
 */
#define DAV_XML_LEN     2048


/*
 * Default View for Cooperation_ menu
 */
#define DAV_VIEW        1

/*
 * Erro codes
 * Note: There isn't a define HT_something = -400 or 405 in 
 * the libwww, so we added it here.
 * 
 * Default HTTP error code - 400 Bad Request
 * 405  Method Not Allowed
 */
#define DAV_BAD_REQUEST -400
#define DAV_METHOD_NOT_ALLOWED -405


/*
 * Thot "Env" strings
 */ 
#define DAV_USER_URL    "DAV_USER_URL"          /* owner element for lock requests */
#define DAV_LOCK_SCOPE  "DAV_LOCK_SCOPE"        /* lockscope element for lock requests*/
#define DAV_DEPTH       "DAV_DEPTH"             /* depth header for lock requests */
#define DAV_TIMEOUT     "DAV_TIMEOUT"           /* timeout header for lock requests */
#define DAV_AWARENESS   "DAV_AWARENESS"         /* if the user wants some awareness information */
#define DAV_AWARENESS_EXIT   "DAV_AWARENESS_ONEXIT"     /* if the user wants information about */
                                                        /* his/her locks, when leaving a page */
#define DAV_URLS        "DAV_URLS"              /* URLs of the resources normally used by the user */ 


/*
 * WebDAV User's preferences and informations
 */
WEBDAV_EXPORT char DAVDepth[DAV_LINE_MAX];                        /* Depth to be used in WebDAV requests */
WEBDAV_EXPORT char DAVTimeout[DAV_LINE_MAX];                      /* Timeout to be used in LOCK requests */
WEBDAV_EXPORT char DAVLockScope[DAV_LINE_MAX];                    /* Lock scope (exclusive/shared) */
WEBDAV_EXPORT char DAVUserURL[DAV_LINE_MAX];                      /* user's reference */
WEBDAV_EXPORT BOOL DAVAwareness;                              /* if user wants general awareness info */
WEBDAV_EXPORT BOOL DAVAwarenessExit;                          /* if user wants awareness information */
                                                /* about his/her locks, when exiting a resource */  


/*
 * Full qualified host.domain name 
 */
WEBDAV_EXPORT char DAVFullHostName[DAV_LINE_MAX];              

/*----------------------------------------------------------------------
   Types and Structures used in WebDAV functions
  ----------------------------------------------------------------------*/
/*
 * Context for WebDAV requests
 */ 
typedef struct _AHTDAVContext {
     char *absoluteURI;         /* absolute URI */
     char *relativeURI;         /* relative URI */
     
     char xmlbody[DAV_XML_LEN]; /* request's xml body */
     
     HTChunk *output;           /* output stream */
     HTChunk *debug;            /* debug stream */
     int status;                /* status of the request */
     
     HTParentAnchor *new_source;/* new source document for PUT and POST file */
     AHTReqContext *new_request;/* new request to be performed */
     AwList *error_msgs;        /* error messages if the next request can't be done*/
     BOOL retry;                /* it is a retry request*/
     
     AwTree *tree;              /* tree for response xml body */
     BOOL showIt;               /* should we show the request's result? */

     HTDAVHeaders *davheaders;  /* WebDAV headers */     
} AHTDAVContext;




/*----------------------------------------------------------------------
   Definitions for the WebDAV preferences dialogue
  ----------------------------------------------------------------------*/
#define DAVPreferencesDlg       1
#define DAVtextUserReference    2
#define DAVtextUserResources    3
#define DAVradioDepth           4
#define DAVradioTimeout         5
#define DAVnumberTimeout        6
#define DAVradioLockScope       7
#define DAVtoggleAwareness      8
#define DAVlabelEmpty1          9
#define DAVlabelEmpty2         10 
#define DAVlabelEmpty3         11 
#define DAVlabelEmpty4         12 
#define DAVlabelEmpty5         13 
#define DAVlabelEmpty6         14 
#define MAX_DAVPREF_DLG        15
#endif
