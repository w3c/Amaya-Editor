/*  --------------------------------------------------------
 ** 
 ** File: davlib.c - WebDAV module
 **
 ** This file implements the WebDAV functions for Amaya editor.
 **
 ** Author : Manuele Kirsch Pinheiro
 ** Email: Manuele.Kirsch_Pinheiro@inrialpes.fr / manuele@inf.ufrgs.br 
 ** Project CEMT - II/UFRGS - Opera Group/INRIA
 ** Financed by CNPq(Brazil)/INRIA(France) Cooperation
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

#include <stdlib.h>
#include <stdio.h>

#define THOT_EXPORT extern

#include "davlib.h"

#include "davlib_f.h"
#include "query_f.h"
#include "init_f.h"
#include "AHTURLTools_f.h"

#include "davlibCommon_f.h"
#include "davlibRequests_f.h"
#include "davlibUI_f.h"


/* ********************************************************************* *
 *                               INIT FUNCTION                           *
 * ********************************************************************* */


/*----------------------------------------------------------------------
   InitDAV - initialization function. Inits DAVHome variable pointing to
             home dir, where the lock base will be stored.   
  ----------------------------------------------------------------------*/
void InitDAV (void) {
    char *ptr = NULL;
    BOOL modified = FALSE;
    char *fqdn = NULL;
    
    
    /* ******************** DAVHome ********************* */
    /* try to get default home (.amaya) dir from APP_HOME */
    ptr = TtaGetEnvString ("APP_HOME");
    sprintf (DAVHome,"%s%c",(ptr)?ptr:TempFileDirectory,DIR_SEP);

#ifdef DEBUG_DAV
    fprintf (stderr,"InitDAV..... DAVHome is %s\n",DAVHome);
#endif   

    
    /* **************** DAVFullHostName ***************** */
    fqdn = DAVFQDN();
    sprintf (DAVFullHostName,"%s",fqdn);
    
#ifdef DEBUG_DAV
    fprintf (stderr,"InitDAV..... DAVFullHostName is %s\n",DAVFullHostName);
#endif   

    
    /***** DAVUserURL, DAVLockScope, DAVDepth, DAVTimeout *****/
    /* getting user URL: if no DAV_USER_URL element defined in 
     * thot.rc, get the user email */
    ptr = NULL; 
    ptr = TtaGetEnvString (DAV_USER_URL);
    if (ptr && (*ptr))   
        sprintf (DAVUserURL,ptr);
    else {  
        char *email = DefaultEmail();
        sprintf (DAVUserURL,"%s%s",(email)?"mailto:":"http://", (email)?email:fqdn);

        /* save user URL in thot env */
        TtaSetEnvString (DAV_USER_URL,DAVUserURL,TRUE);
        modified = TRUE;
    }
    

    /* getting lock scope. If there is an entry DAV_LOCK_SCOPE in
     * thot.rc and it is valid, use it. Otherwise, assume "exclusive" scope */
    ptr = NULL; 
    ptr = TtaGetEnvString (DAV_LOCK_SCOPE);
    if (ptr && (*ptr) && \
       (!strcmp(ptr,"exclusive") || !strcmp (ptr,"shared")))
        sprintf (DAVLockScope,ptr);
    else { 
        sprintf (DAVLockScope,"exclusive");
        
        /*save lock scope in thot env*/
        TtaSetEnvString (DAV_LOCK_SCOPE,DAVLockScope,TRUE);
        modified = TRUE;
    }

    
    /* getting depth option. If there is an entry DAV_DEPTH and
     * it is valid, use it/ Otherwise, assuming 0 */
    ptr = NULL;  
    ptr = TtaGetEnvString (DAV_DEPTH);
    if (ptr && (*ptr) && \
       (!strcmp (ptr,"0") || !strcmp(ptr,"1") || !strcmp(ptr,"infinity")))
        sprintf (DAVDepth,ptr); 
    else {
        sprintf (DAVDepth,"0");
        
        /* save depth in thot env */
        TtaSetEnvString (DAV_DEPTH,DAVDepth,TRUE);
        modified = TRUE;
    }

    
    /* getting timeout option. If no timeout,assuming Infinite */
    ptr = NULL; 
    ptr = TtaGetEnvString (DAV_TIMEOUT);
    if (ptr && (*ptr))
        sprintf (DAVTimeout,ptr);
    else { 
        sprintf (DAVTimeout,"Infinite");

        /* save timeout in thot env */
        TtaSetEnvString (DAV_TIMEOUT,DAVTimeout,TRUE);
        modified = TRUE;
    }

    /* *********************** SAVING REGISTRY ***************** */
    if (modified) TtaSaveAppRegistry();

    
#ifdef DEBUG_DAV
    fprintf (stderr,"InitDAV..... WebDAV User's preferences are \n");
    fprintf (stderr,"\tuser url: %s\n\tlock scope: %s\n",DAVUserURL,DAVLockScope); 
    fprintf (stderr,"\tdepth: %s\n\ttimeout: %s\n",DAVDepth,DAVTimeout); 

    /*HTSetTraceMessageMask("pl");*/
#endif


    /* *********************** SETING FILTERS *************** */
    HTNet_addAfter (FilterFailedDependency_handler,"http://*", NULL, \
                                           HT_FAILED_DEPENDENCY,HT_FILTER_MIDDLE);

    HTNet_addAfter (FilterLocked_handler,"http://*", NULL, \
                                           HT_LOCKED,HT_FILTER_MIDDLE);

    HTNet_addAfter (FilterMultiStatus_handler,"http://*", NULL, \
                                           HT_MULTI_STATUS,HT_FILTER_MIDDLE);


    /* *********************** SETING MENUS ***************** */
    /* set lock indicatior toggle. initial state: FALSE */
    DAVLockIndicatorState = FALSE; 
    DAVSetLockIndicator(CurrentDocument);
    
}



/* ********************************************************************* *
 *                               MENU ACTIONS                            *
 * ********************************************************************* */


/*----------------------------------------------------------------------
   DAVLockDocument - Lock the document URL 

  ----------------------------------------------------------------------*/
void DAVLockDocument (Document document, View view)
{
    AHTDAVContext *davctx = NULL;

#ifdef DEBUG_DAV    
    fprintf (stderr,"DAVLockDocument..... Locking document %s\n",DocumentURLs[document]);
    fprintf (stderr,"DAVLockDocument..... creating the dav context object\n");
#endif
    
    /* getting DAV context object */ 
    davctx = GetLockInfo (document);
    if (!davctx) {
        DAVDisplayMessage (TtaGetMessage (AMAYA, AM_DAV_UNSUPPORTED_PROTOCOL), \
                           DocumentURLs[document]);
    }
    else {
#ifdef DEBUG_DAV
        char *url = DocumentURLs[document];
        fprintf (stderr,"\tabsolute: %s\n\trelative: %s\n",davctx->absoluteURI,davctx->relativeURI);
        fprintf (stderr,"\tdepth: %s\n",HTDAV_DepthHeader(davctx->davheaders));
        fprintf (stderr,"\ttimeout: %s\n",HTDAV_TimeoutHeader(davctx->davheaders));
        fprintf (stderr,"\turl: %s\n",url);
        fprintf (stderr,"\tLock XML body: %s\n",(davctx->xmlbody)?"YES":"NO");
#endif 

        /* executing the request */
        if (!DoLockRequest (document,davctx)) {
            DAVDisplayMessage (TtaGetMessage (AMAYA, AM_LOCK_FAILED), NULL);
        }

    }
        
}



/*----------------------------------------------------------------------
   DAVUlnlocDocument - Unlock document URL

  ----------------------------------------------------------------------*/
void DAVUnlockDocument (Document document, View view) {
    AHTDAVContext *davctx = NULL;
    
#ifdef DEBUG_DAV    
    fprintf (stderr,"DAVUnlockDocument..... Unlocking document %s\n",DocumentURLs[document]);
    fprintf (stderr,"DAVUnlockDocument..... creating the dav context object\n");
#endif
  
    davctx = GetUnlockInfo(document);
    if (davctx) {
#ifdef DEBUG_DAV
        fprintf (stderr,"DAVUnlockDocument..... DAV context object:\n");
        fprintf (stderr,"\tabsolute: %s\n\trelative: %s\n",davctx->absoluteURI,davctx->relativeURI);
        fprintf (stderr,"\tdepth: %s\n",HTDAV_DepthHeader(davctx->davheaders));
        fprintf (stderr,"\tlock-token: %s\n",HTDAV_LockTokenHeader(davctx->davheaders));
#endif 
        
        /* do the request */       
        if (!DoUnlockRequest (document,davctx)) {
            DAVDisplayMessage (TtaGetMessage (AMAYA, AM_UNLOCK_FAILED), NULL);
        }
        
    }
    else {
        /* *** Should we give to the user the option to unlock a document anyway? YES *** */
        ForceUnlockRequest (document);
    }
}



/*----------------------------------------------------------------------
   DAVProfindDocument - do a Propfind request in the document URL 
   
  ----------------------------------------------------------------------*/
void DAVProfindDocument (Document document, View view) {
    AHTDAVContext *davctx= NULL;

#ifdef DEBUG_DAV
    fprintf (stderr,"DAVPropfindDocument.... Profind document %s\n",DocumentURLs[document]);
    fprintf (stderr,"DAVPropfindDocument..... creating the dav context object\n");    
#endif    
    
    davctx = GetPropfindInfo(document);
    if (davctx) {
#ifdef DEBUG_DAV
        fprintf (stderr,"DAVPropfindDocument..... DAV context object:\n");
        fprintf (stderr,"\tabsolute: %s\n\trelative: %s\n",davctx->absoluteURI,davctx->relativeURI);
        fprintf (stderr,"\tdepth: %s\n",HTDAV_DepthHeader(davctx->davheaders));
        fprintf (stderr,"\txml body: %s\n",(davctx->xmlbody));    
#endif
        
        /* YES, show the results*/
        davctx->showIt = YES;

        if (!DoPropfindRequest (document,davctx,FilterPropfind_handler,NULL)) {
            DAVDisplayMessage (TtaGetMessage (AMAYA,AM_PROPFIND_FAILED), NULL);
        }
        
    }
    else {
#ifndef _WINDOWS            
        DAVDisplayMessage (TtaGetMessage (AMAYA, AM_DAV_UNSUPPORTED_PROTOCOL), \
                           DocumentURLs[document]);                        
#endif
        DAVLockIndicatorState = FALSE;
    }
    
    
}



/*----------------------------------------------------------------------
   DAVCopyLockInfo - do a Propfind request in the document URL 
   to discover and copy (to the local base) the lock information
  ----------------------------------------------------------------------*/
void DAVCopyLockInfo (Document document, View view) {
    AHTDAVContext *davctx= NULL;
    BOOL ok = NO;
#ifdef DEBUG_DAV
    fprintf (stderr,"DAVCopyLockInfo.... Profind document %s\n",DocumentURLs[document]);
    fprintf (stderr,"DAVCopyLockInfo..... creating the dav context object\n");    
#endif    
    
    davctx = GetPropfindInfo(document);
    if (davctx) {
#ifdef DEBUG_DAV
        fprintf (stderr,"DAVPropfindDocument..... DAV context object:\n");
        fprintf (stderr,"\tabsolute: %s\n\trelative: %s\n",davctx->absoluteURI,davctx->relativeURI);
        fprintf (stderr,"\tdepth: %s\n",HTDAV_DepthHeader(davctx->davheaders));
        fprintf (stderr,"\txml body: %s\n",(davctx->xmlbody));    
#endif
        
        /* YES, show the results*/
        davctx->showIt = YES;

        if (createPropfindBody (YES,davctx->xmlbody,DAV_XML_LEN)) {
            ok = DoPropfindRequest (document,davctx,FilterCopyLockInfo_handler,NULL);
        }
        
        if (!ok)
            DAVDisplayMessage (TtaGetMessage (AMAYA,AM_PROPFIND_FAILED), NULL);
        
    }
    else {
#ifndef _WINDOWS            
        DAVDisplayMessage (TtaGetMessage (AMAYA, AM_DAV_UNSUPPORTED_PROTOCOL), \
                           DocumentURLs[document]);                        
#endif
        DAVLockIndicatorState = FALSE;
    }
    
    
}



/*----------------------------------------------------------------------
   DAVLockIndicator: Manipulates Lock indicator
  ----------------------------------------------------------------------*/
void DAVLockIndicator (Document document, View view) {
        
    /* if lock indicator is TRUE, unlock the document
     * if it's FALSE, lock the document */
    if (DAVLockIndicatorState) {
        DAVUnlockDocument (document,view);
    }
    else  {
        DAVLockDocument (document,view);
    }

}





