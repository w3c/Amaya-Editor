/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

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
 ** Revision 1.21  2009-06-10 10:57:23  vatton
 ** Change the management of Templates list
 ** + Implementation of a new WebDAV list of sites
 ** + Fix problems with lock/unlock status
 ** Irene
 **
 ** Revision 1.20  2009/06/08 14:57:00  vatton
 ** Addd a new button to lock/unlock WebDAV resources
 ** + display only the end of the message when the status bar is too short
 ** Irene
 **
 ** Revision 1.19  2009/04/23 14:51:36  vatton
 ** Improving the WebDAV interface
 ** Irene
 **
 ** Revision 1.18  2008/05/07 13:49:07  kia
 ** char* => const char*
 ** (Regis patches + many many others)
 **
 ** Revision 1.17  2005/06/23 15:00:48  cvs
 ** fix 2 memory leak.
 ** (I hope it can help for MacOSX crash on http://www.w3.org/People/all ?)
 ** S. GULLY
 **
 ** Revision 1.16  2005/06/21 16:02:06  cvs
 ** Bug fix: a double free in davlib code caused the
 ** wx version built with dav option to crash.
 ** LC + SG
 **
 ** Revision 1.15  2005/06/01 13:48:48  cvs
 ** Fixing some memory leaks.
 ** IV + SG
 **
 ** Revision 1.14  2005/02/04 12:18:24  vatton
 ** Improve the transformation into a table
 ** + typo in the WebDAV documentation.
 ** Irene
 **
 ** Revision 1.13  2003/12/16 12:10:08  cvs
 ** Rename flags _WINDOWS by _WINGUI
 ** + the flag _WINDOWS must be used only for windows specific code.
 ** + the flag _WINGUI must be used only for old windows gui stuff.
 **
 ** S. GULLY
 **
 ** Revision 1.12  2003/11/19 12:33:16  gully
 ** Compilation fix (webdav support)
 **
 ** S. GULLY
 **
 ** Revision 1.11  2003/05/19 10:24:54  vatton
 ** Internationalization of the MakeID dialogue.
 ** Irene
 **
 ** Revision 1.10  2002/07/01 10:34:16  kahan
 ** JK: Enabling/Disabling DAV support by means of the new DAV_Enable
 ** registry entry.
 ** Removed the DAV menu from the standard profile.
 **
 ** Revision 1.9  2002/06/13 16:10:13  kirschpi
 ** New dialogue "WebDAV Preferences"
 ** Corrections due last commit by JK
 ** Manuele
 **
 ** Revision 1.8  2002/06/13 13:40:32  kahan
 ** JK: Changed MAX_LINE to DAV_MAX_LINE. MAX_LINE is a reserved macro and
 ** the code was generating a warning.
 **
 ** Revision 1.7  2002/06/11 17:34:52  kirschpi
 ** improving commentaries and format (for WebDAV code).
 ** update DAV Resources list automatically
 ** Manuele
 **
 ** Revision 1.6  2002/06/06 17:10:45  kirschpi
 ** Breaking the user messages in three lines
 ** Fixing some code format problems
 ** Fixing unecessary memory allocations in FilterMultiStatus_handler
 ** and FilterLocked_handler.
 ** Manuele
 **
 ** Revision 1.5  2002/06/05 16:46:06  kirschpi
 ** Applying Amaya code format.
 ** Modifying some dialogs (looking for a better windows presentation)
 **
 ** Revision 1.4  2002/06/04 14:51:41  kirschpi
 ** Fixing bug on InitDAV.
 ** Manuele
 **
 ** Revision 1.3  2002/06/03 14:37:42  kirschpi
 ** The name of some public functions have been changed to avoid conflic with
 ** other libraries.
 **
 ** Revision 1.2  2002/05/31 17:59:19  kirschpi
 ** Functions to give to user some informations about active locks
 ** (a basic awareness support) when the user load or exit a document.
 **
 * -------------------------------------------------------- 
 */

#include <stdlib.h>
#include <stdio.h>

#define THOT_EXPORT extern

#define WEBDAV_EXPORT

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
void InitDAV (void) 
{
    char *ptr = NULL;
    BOOL modified = FALSE;
    const char *fqdn = NULL;
    char *email = NULL;

    /* DAV support is enabled */
    /*TtaGetEnvBoolean ("DAV_ENABLE", &DAVLibEnable);
    if (!DAVLibEnable)
    return;*/
    DAVLibEnable = TRUE;
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
    else 
     {  
        email = DAVDefaultEmail();
        sprintf (DAVUserURL,"%s%s",(email)?"mailto:":"http://", (email)?email:fqdn);

        /* save user URL in thot env */
        TtaSetEnvString (DAV_USER_URL,DAVUserURL,TRUE);
        modified = TRUE;
     }
    
    /* getting lock scope. If there is an entry DAV_LOCK_SCOPE in
     * thot.rc and it is valid, use it. Otherwise, assume "exclusive" scope */
    ptr = NULL; 
    ptr = TtaGetEnvString (DAV_LOCK_SCOPE);
    if (ptr && (*ptr) && 
       (!strcmp(ptr,"exclusive") || !strcmp (ptr,"shared")))
        sprintf (DAVLockScope,ptr);
    else 
     { 
        sprintf (DAVLockScope,"exclusive");
        
        /*save lock scope in thot env*/
        TtaSetEnvString (DAV_LOCK_SCOPE,DAVLockScope,TRUE);
        modified = TRUE;
     }

    
    /* getting depth option. If there is an entry DAV_DEPTH and
     * it is valid, use it/ Otherwise, assuming 0 
     * Note: for LOCK request, only the values 0 and infinity are accepted*/
    ptr = NULL;  
    ptr = TtaGetEnvString (DAV_DEPTH);
    if (ptr && (*ptr) &&\
       (!strcmp (ptr,"0") || !strcmp(ptr,"infinity")))
        sprintf (DAVDepth,ptr); 
    else 
     {
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
    else
     { 
        sprintf (DAVTimeout,"Infinite");

        /* save timeout in thot env */
        TtaSetEnvString (DAV_TIMEOUT,DAVTimeout,TRUE);
        modified = TRUE;
     }

    
    /************* DAVAwareness, DAVAwarenessOnExit ************ */
    DAVAwareness = TRUE;
    DAVAwarenessExit = TRUE;
    
    /* *********************** SAVING REGISTRY ***************** */
    if (modified)
      TtaSaveAppRegistry();
    
#ifdef DEBUG_DAV
    fprintf (stderr,"InitDAV..... WebDAV User's preferences are \n");
    fprintf (stderr,"\tuser url: %s\n\tlock scope: %s\n",DAVUserURL,DAVLockScope); 
    fprintf (stderr,"\tdepth: %s\n\ttimeout: %s\n",DAVDepth,DAVTimeout); 

    /*HTSetTraceMessageMask("pl");*/
#endif


    /* *********************** SETING FILTERS *************** */
    HTNet_addAfter (FilterFailedDependency_handler,"http://*", NULL, 
                                           HT_FAILED_DEPENDENCY,HT_FILTER_MIDDLE);

    HTNet_addAfter (FilterLocked_handler,"http://*", NULL, 
                                           HT_LOCKED,HT_FILTER_MIDDLE);

    HTNet_addAfter (FilterMultiStatus_handler,"http://*", NULL, 
                                           HT_MULTI_STATUS,HT_FILTER_MIDDLE);
}


/* ********************************************************************* *
 *                      CLOSE DOCUMENT FUNCTION                          *
 * ********************************************************************* */


/*----------------------------------------------------------------------
   DAVFreeLock - ask to user if he/she wants to free pending locks 
                 when closing the session.   
   
  NEED: deal with last document when exiting the application.
   
        Now, when exiting the application, if the document is locked
        by the user (the lock information must be in the local base),
        this function will ask whether the user wants to unlock it.
        If user agrees, an UNLOCK request will be sent. But, under
        Windows machines, this request will be killed when the application
        exit, and no unlock will be done.
                      
  ----------------------------------------------------------------------*/
void DAVFreeLock (Document docid) 
{
    BOOL ok = NO;
    char *lockinfo;
    char *relURI, *absURI, *ptr;
    char label1[DAV_LINE_MAX], label2[DAV_LINE_MAX];

    if (!DAVLibEnable)
      return;

    lockinfo = relURI = absURI = ptr = NULL;
    
    /* if user doesn't want awareness info, neither
     * awareness info on exit, return
     */ 
    if (!(DAVAwareness && DAVAwarenessExit))
        return;
    
#ifdef DEBUG_DAV
    fprintf (stderr,"Closing document %s\n",DocumentURLs[docid]);
#endif

    /* separing URI into hostname and relative parts */
    ok = separateUri (DocumentURLs[docid], DAVFullHostName, &absURI, &relURI);
    
    if (ok && absURI && relURI) 
     {
        /* if there is a lock info in the local base,
         * user has a lock for this resource.
         */
        lockinfo = DAVFindLockToken (absURI, relURI);
        if (lockinfo && *lockinfo) 
         {
            sprintf (label1,TtaGetMessage(AMAYA,AM_LOCKED),DocumentURLs[docid]);
            ptr = DAVBreakString (label1);
            sprintf (label2,TtaGetMessage(AMAYA,AM_UNLOCK_DOCUMENT));
            
            if (DAVConfirmDialog (docid,label1,(ptr)?ptr:(char *)" ", label2)) 
             {
                AHTDAVContext *new_davctx = GetUnlockInfo (docid);
                if (new_davctx) 
                 {
                    new_davctx->showIt = NO;    
                    DoUnlockRequest (docid, new_davctx);                    
                 }    
             }
         } /* lockinfo */
     }
}



/* ********************************************************************* *
 *                           REGISTRY OPERATION                          *
 * ********************************************************************* */


/*----------------------------------------------------------------------
   DAVSaveRegistry - save DAV information in the registry

  ----------------------------------------------------------------------*/
void DAVSaveRegistry (void)
{
  if (!DAVLibEnable)
    return;

    /***** DAVUserURL, DAVLockScope, DAVDepth, DAVTimeout *****/
    TtaSetEnvString (DAV_USER_URL,DAVUserURL,TRUE);
    TtaSetEnvString (DAV_LOCK_SCOPE,DAVLockScope,TRUE);
    TtaSetEnvString (DAV_DEPTH,DAVDepth,TRUE);
    TtaSetEnvString (DAV_TIMEOUT,DAVTimeout,TRUE);
    /* *********************** SAVING REGISTRY ***************** */
    TtaSaveAppRegistry();
    
#ifdef DEBUG_DAV
    fprintf (stderr,"DAVSaveRegistry..... WebDAV User's preferences are \n");
    fprintf (stderr,"\tuser url: %s\n\tlock scope: %s\n",DAVUserURL,DAVLockScope); 
    fprintf (stderr,"\tdepth: %s\n\ttimeout: %s\n",DAVDepth,DAVTimeout); 
#endif
}


/* ********************************************************************* *
 *                               MENU ACTIONS                            *
 * ********************************************************************* */


/*----------------------------------------------------------------------
   DAVLockDocument - Lock the document URL 
  ----------------------------------------------------------------------*/
void DAVLockDocument (Document doc, View view)
{
  AHTDAVContext *davctx = NULL;
  char          *url = DocumentURLs[doc];

  if (!DAVLibEnable || url == NULL)
    return;
  if (DocumentMeta[doc] == NULL || DocumentMeta[doc]->lockState == 0)
    // not a webDAV resource
    return;

#ifdef DEBUG_DAV    
  fprintf (stderr,"DAVLockDocument..... Locking document %s\n", url);
  fprintf (stderr,"DAVLockDocument..... creating the dav context object\n");
#endif
    
  /* getting DAV context object */ 
  davctx = GetLockInfo (doc);
  if (!davctx) 
    {
      DAVDisplayMessage (TtaGetMessage (AMAYA, AM_DAV_UNSUPPORTED_PROTOCOL), url);
      DAVSetLockIndicator (doc, 0);
    }
  else 
    {
#ifdef DEBUG_DAV
      fprintf (stderr,"\tabsolute: %s\n\trelative: %s\n",davctx->absoluteURI,davctx->relativeURI);
      fprintf (stderr,"\tdepth: %s\n",HTDAV_DepthHeader(davctx->davheaders));
      fprintf (stderr,"\ttimeout: %s\n",HTDAV_TimeoutHeader(davctx->davheaders));
      fprintf (stderr,"\turl: %s\n",url);
      fprintf (stderr,"\tLock XML body: %s\n",(davctx->xmlbody)?"YES":"NO");
#endif 
      /* executing the request */
      if (!DoLockRequest (doc, davctx)) 
        DAVDisplayMessage (TtaGetMessage (AMAYA, AM_LOCK_FAILED), NULL);
      else
        DAVSetLockIndicator (doc, 2);
    }
}


/*----------------------------------------------------------------------
   DAVUlnlocDocument - Unlock document URL

  ----------------------------------------------------------------------*/
void DAVUnlockDocument (Document doc, View view) 
{
  AHTDAVContext *davctx = NULL;
  char          *url = DocumentURLs[doc];

  if (!DAVLibEnable || url == NULL)
    return;
  if (DocumentMeta[doc] == NULL || DocumentMeta[doc]->lockState == 0)
    // not a webDAV resource
    return;

#ifdef DEBUG_DAV    
  fprintf (stderr,"DAVUnlockDocument..... Unlocking document %s\n", url);
  fprintf (stderr,"DAVUnlockDocument..... creating the dav context object\n");
#endif
  davctx = GetUnlockInfo (doc);
  if (davctx) 
    {
#ifdef DEBUG_DAV
      fprintf (stderr,"DAVUnlockDocument..... DAV context object:\n");
      fprintf (stderr,"\tabsolute: %s\n\trelative: %s\n", davctx->absoluteURI, davctx->relativeURI);
      fprintf (stderr,"\tdepth: %s\n", HTDAV_DepthHeader(davctx->davheaders));
      fprintf (stderr,"\tlock-token: %s\n", HTDAV_LockTokenHeader(davctx->davheaders));
#endif 
      /* do the request */       
      if (!DoUnlockRequest (doc, davctx)) 
        DAVDisplayMessage (TtaGetMessage (AMAYA, AM_UNLOCK_FAILED), NULL);
      else
        DAVSetLockIndicator (doc, 1);
    }
  else 
    /* *** Should we give to the user the option to unlock a document anyway? YES *** */
    ForceUnlockRequest (doc);
}


/*----------------------------------------------------------------------
   DAVCopyLockInfo - do a Propfind request in the document URL 
   to discover and copy (to the local base) the lock information
  ----------------------------------------------------------------------*/
void DAVCopyLockInfo (Document doc, View view) 
{
    AHTDAVContext *davctx= NULL;
    BOOL ok = NO;

    if (!DAVLibEnable)
      return;

#ifdef DEBUG_DAV
    fprintf (stderr,"DAVCopyLockInfo.... Profind document %s\n",DocumentURLs[doc]);
    fprintf (stderr,"DAVCopyLockInfo..... creating the dav context object\n");    
#endif    
    
    davctx = GetPropfindInfo(doc);
    if (davctx) 
     {
#ifdef DEBUG_DAV
        fprintf (stderr,"DAVPropfindDocument..... DAV context object:\n");
        fprintf (stderr,"\tabsolute: %s\n\trelative: %s\n", davctx->absoluteURI, davctx->relativeURI);
        fprintf (stderr,"\tdepth: %s\n", HTDAV_DepthHeader(davctx->davheaders));
        fprintf (stderr,"\txml body: %s\n", (davctx->xmlbody));    
#endif
        /* YES, show the results*/
        davctx->showIt = YES;
        if (createPropfindBody (YES, davctx->xmlbody, DAV_XML_LEN)) 
            ok = DoPropfindRequest (doc,davctx, FilterCopyLockInfo_handler, NULL);
        if (!ok)
            DAVDisplayMessage (TtaGetMessage (AMAYA, AM_PROPFIND_FAILED), NULL);
     }
    else
      DAVSetLockIndicator (doc, 1);
}


/*----------------------------------------------------------------------
   DAVLockIndicator: Manipulates Lock indicator
  ----------------------------------------------------------------------*/
void DAVLockIndicator (Document doc, View view) 
{
    /* if lock indicator is TRUE, unlock the document
     * if it's FALSE, lock the document */
  if (DocumentMeta[doc])
    {
      if (DocumentMeta[doc]->lockState == 2) 
        DAVUnlockDocument (doc, view);    
      else if (DocumentMeta[doc]->lockState == 1)
        DAVLockDocument (doc, view);    
    }
}
