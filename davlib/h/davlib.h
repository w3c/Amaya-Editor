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
 ** Revision 1.1  2002-05-31 10:48:46  kirschpi
 ** Added a new module for WebDAV purposes _ davlib.
 ** Some changes have been done to add this module in the following files:
 ** amaya/query.c, amaya/init.c, amaya/answer.c, amaya/libwww.h, amaya/amayamsg.h,
 ** amaya/EDITOR.A, amaya/EDITORactions.c, amaya/Makefile.libwww amaya/Makefile.in,
 ** config/amaya.profiles, tools/xmldialogues/bases/base_am_dia.xml,
 ** tools/xmldialogues/bases/base_am_dia.xml, Makefile.in, configure.in
 ** This new module is only activated when --with-dav options is used in configure.
 **
 * -------------------------------------------------------- 
 */

#ifndef DAVLIB_H
#define DAVLIB_H

#include "string.h"      	/* Thot stuff */
#include "thot_gui.h"
#include "thot_sys.h"
#include "application.h"
#include "appstruct.h"
#include "interface.h"

#include "amaya.h"       	/* amaya declarations, as DocumentURLs array
				   and include files like #include "libwww.h" */

/* header files used by AHTDAVContext struct */
#include "awtree.h"     	/* AwTree struct, for tree element in AHTDAVContext */
#include "HTChunk.h"    	/* for the output/debug elements in AHTDAVContext */
#include "HTReq.h"        	/* for oldRequest element in AHTDAVContext */
#include "HTDAV.h"      	/* WebDAV module in libwww */
#include "HTHome.h"

#include "AHTLockBase.h"	/* "base" for lock tokens */



/*----------------------------------------------------------------------
   Definitions/global variables used in WebDAV functions
  ----------------------------------------------------------------------*/

/* 
 * XML body max. length 
 */
#define DAV_XML_LEN	2048


/*
 * Default View for Cooperation_ menu
 */
#define DAV_VIEW   	1

/*
 * Erro codes
 * Note: There isn't a define HT_something = -400 or 405 in 
 * the libwww, so we added it here.
 * 
 * Default HTTP error code - 400 Bad Request
 * 405  Method Not Allowed
 */
#define DAV_BAD_REQUEST	-400
#define DAV_METHOD_NOT_ALLOWED -405

/*
 * Thot "Env" strings
 */ 
#define DAV_USER_URL    "DAV_USER_URL"   	/* owner element for lock requests */
#define DAV_LOCK_SCOPE  "DAV_LOCK_SCOPE"	/* lockscope element for lock requests*/
#define DAV_DEPTH       "DAV_DEPTH"     	/* depth header for lock requests */
#define DAV_TIMEOUT     "DAV_TIMEOUT"   	/* timeout header for lock requests */


/*
 * WebDAV User's preferences and informations
 */
char DAVDepth[LINE_MAX];                 	/* Depth to be used in WebDAV requests */
char DAVTimeout[LINE_MAX];              	/* Timeout to be used in LOCK requests */
char DAVLockScope[LINE_MAX];            	/* Lock scope (exclusive/shared) */
char DAVUserURL[LINE_MAX];              	/* user's reference */


/*
 * Full qualified host.domain name 
 */
char DAVFullHostName[LINE_MAX];              


/*
 * State of the LockIndicator toggle
 */
BOOL DAVLockIndicatorState;


/*----------------------------------------------------------------------
   Types and Structures used in WebDAV functions
  ----------------------------------------------------------------------*/

/*
 * Context for WebDAV requests
 */ 
typedef struct _AHTDAVContext {
     char *absoluteURI;    	/* absolute URI */
     char *relativeURI;     	/* relative URI */
     
     char xmlbody[DAV_XML_LEN];	/* request's xml body */
     
     HTChunk *output;      	/* output stream */
     HTChunk *debug;      	/* debug stream */
     int status;           	/* status of the request */
     
     HTParentAnchor *new_source;/* new source document for PUT and POST file */
     AHTReqContext *new_request;/* new request to be performed */
     AwList *error_msgs;       	/* error messages if the next request can't be done*/
     BOOL retry;               	/* it is a retry request*/
     
     AwTree *tree;        	/* tree for response xml body */
     BOOL showIt;            	/* should we show the request's result? */

     HTDAVHeaders *davheaders;	/* WebDAV headers */     
} AHTDAVContext;





#endif
