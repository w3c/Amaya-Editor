/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*  --------------------------------------------------------
 ** 
 ** File: davlibRequests.c - Request functions for WebDAV module
 **
 ** This file implements the functions used for the WebDAV 
 ** requests into Amaya editor.
 **
 ** Author : Manuele Kirsch Pinheiro
 ** Email: Manuele.Kirsch_Pinheiro@inrialpes.fr / manuele@inf.ufrgs.br 
 ** Project CEMT - II/UFRGS - Opera Group/INRIA
 ** Financed by CNPq(Brazil)/INRIA(France) Cooperation
 **
 ** Date : May / 2002
 **
 ** $Id$
 ** $Date$
 ** $Log$
 ** Revision 1.21  2009-06-29 08:28:58  vatton
 ** Fix Windows warnings
 ** Irene
 **
 ** Revision 1.20  2009/06/10 10:57:23  vatton
 ** Change the management of Templates list
 ** + Implementation of a new WebDAV list of sites
 ** + Fix problems with lock/unlock status
 ** Irene
 **
 ** Revision 1.19  2009/06/08 14:57:00  vatton
 ** Addd a new button to lock/unlock WebDAV resources
 ** + display only the end of the message when the status bar is too short
 ** Irene
 **
 ** Revision 1.18  2009/04/23 14:51:36  vatton
 ** Improving the WebDAV interface
 ** Irene
 **
 ** Revision 1.17  2009/04/10 14:22:18  vatton
 ** Fix several WebDAV bugs
 ** Irene
 **
 ** Revision 1.16  2008/05/13 09:30:27  kia
 ** More char* fixes
 **
 ** Revision 1.15  2008/05/07 13:49:07  kia
 ** char* => const char*
 ** (Regis patches + many many others)
 **
 ** Revision 1.14  2005/06/21 13:29:35  gully
 ** compilation fix
 ** S. GULLY
 **
 ** Revision 1.13  2004/05/27 15:47:56  cvs
 ** Irene
 **
 ** Revision 1.12  2003/11/19 12:33:16  gully
 ** Compilation fix (webdav support)
 **
 ** S. GULLY
 **
 ** Revision 1.11  2002/06/18 08:21:48  kahan
 ** JK: removed some compiler warnings when compiling without the DAV_DEBUG flag.
 **
 ** Revision 1.10  2002/06/13 16:10:13  kirschpi
 ** New dialogue "WebDAV Preferences"
 ** Corrections due last commit by JK
 ** Manuele
 **
 ** Revision 1.9  2002/06/13 13:40:32  kahan
 ** JK: Changed MAX_LINE to DAV_MAX_LINE. MAX_LINE is a reserved macro and
 ** the code was generating a warning.
 **
 ** Revision 1.8  2002/06/12 10:29:07  kirschpi
 ** - adjusts in code format
 ** - new commentaries
 ** - small fixes
 ** Manuele
 **
 ** Revision 1.7  2002/06/11 17:34:52  kirschpi
 ** improving commentaries and format (for WebDAV code).
 ** update DAV Resources list automatically
 ** Manuele
 **
 ** Revision 1.6  2002/06/06 17:10:46  kirschpi
 ** Breaking the user messages in three lines
 ** Fixing some code format problems
 ** Fixing unecessary memory allocations in FilterMultiStatus_handler
 ** and FilterLocked_handler.
 ** Manuele
 **
 ** Revision 1.5  2002/06/05 16:46:06  kirschpi
 ** Applying Amaya code format.
 ** Modifying some dialogs (looking for a better windows presentation)
 ** Adding a DAVResource list, a list of resources (specially collections),
 ** where we should do a Lock discovery.
 ** Manuele
 **
 ** Revision 1.4  2002/06/04 14:52:42  kirschpi
 ** *** empty log message ***
 **
 ** Revision 1.3  2002/06/03 14:37:43  kirschpi
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

#define WEBDAV_EXPORT extern
#include "davlib.h"

#include "query_f.h"
#include "init_f.h"
#include "AHTURLTools_f.h"

#include "davlibRequests_f.h"
#include "davlibCommon_f.h"
#include "davlibUI_f.h"
#include "davlib_f.h"




/* ********************************************************************* *
 *                      GLOBAL FILTERS / CALLBACKS                       *
 * ********************************************************************* */


/*----------------------------------------------------------------------
  FilterFailedDependency_handler: After filter for handle 424 Failed 
                                 Dependency status code.
  ---------------------------------------------------------------------- */
int FilterFailedDependency_handler (HTRequest * request, HTResponse * response,
                   void * param, int status)  
{
    /* A 424 Failed Dependency status code may have many causes. The
     * most problably is an If header without the opaque lock of some
     * ancestral resource or with an invalid opaque lock.
     */
#ifdef DAV
    DAVDisplayMessage (TtaGetMessage (AMAYA, AM_FAILED_DEPENDENCY),NULL);
#else
    InitInfo (" ",TtaGetMessage (AMAYA, AM_FAILED_DEPENDENCY));
#endif  
    return HT_OK;
}


/* ----------------------------------------------------------------------
   DAVSetCopyRequest: set a copy of a request context, according the 
                   method needs. Used by FilterLoked_handler and 
                   FilterMultiStatus_handler.
   ---------------------------------------------------------------------- */
void DAVSetCopyRequest (AHTReqContext *new_request, AHTReqContext *context, 
                        AHTDAVContext *davctx) 
{
    char *filename = NULL;
                                   
    if (new_request && context && davctx) 
     {
#ifdef DEBUG_DAV
        fprintf (stderr,"DAVSetCopyRequest.... copying the request\n");
#endif  
        /* copy the document content for PUT and file POST */   
        if (new_request->method == METHOD_PUT || 
           (new_request->method == METHOD_POST && 
           (new_request->mode & AMAYA_FILE_POST))) 
         {
            if (context->document) 
             {
                new_request->document = TtaStrdup (context->document);
             }
            else
             {
                filename = HTAnchor_address (context->source);
                filename = strchr (filename,':');
                if (filename) 
                 {
                    filename++;
                    new_request->document = DAVCopyFile (filename,context->block_size);
                 }
             }
                        
            /* set new source anchor with the document */
            if (new_request->document) 
             {
                davctx->new_source = HTTmpAnchor (NULL);
                HTAnchor_setDocument (davctx->new_source, new_request->document);
                HTAnchor_setLength (davctx->new_source, context->block_size);
                HTAnchor_setFormat (davctx->new_source, HTAnchor_format (HTAnchor_parent\
                                                                     (context->source)));

                if (new_request->method == METHOD_PUT) 
                 {
                    if (HTAnchor_charset (HTAnchor_parent(context->source)))
                        HTAnchor_setCharset (davctx->new_source,
                                             HTAnchor_charset (HTAnchor_parent\
                                                                     (context->source)));
                 }                           
             } /* document */

         } /* PUT || POST file */        

        if (new_request->method == METHOD_PUT) 
         {
            /* default put name */
            new_request->default_put_name = TtaStrdup (context->default_put_name);
            HTRequest_setDefaultPutName (new_request->request, 
                                         new_request->default_put_name);

            /* preconditions */
            HTRequest_setPreconditions(new_request->request, 
                                       HTRequest_preconditions (context->request));

            /* request anchor */
            HTRequest_setAnchor (new_request->request, new_request->dest);
                            
         } /* PUT */
        else if (new_request->method == METHOD_POST) 
        {
            if (new_request->mode & AMAYA_FILE_POST) 
             {
                HTRequest_setEntityAnchor (new_request->request,davctx->new_source);
             }
            else if (new_request->mode & AMAYA_FORM_POST) 
             {
                HTRequest_setEntityAnchor (new_request->request,new_request->anchor);
             }

         }/* POST FORM & FILE */
                
     } /* new_request */
}



/*----------------------------------------------------------------------
  FilterLocked_handler: After filter for handle 423 Locked status code.
  
  ---------------------------------------------------------------------- */
int FilterLocked_handler (HTRequest * request, HTResponse * response,
                          void * param, int status)  
{

    AHTReqContext *context = (AHTReqContext *) HTRequest_context (request);
    AHTDAVContext *davctx  = (context)?(AHTDAVContext *)context->dav_context:NULL;
    AHTDAVContext *new_davctx = NULL;
    char *msg = NULL;
    BOOL ok = NO;
    
    if (context) 
     {
        new_davctx = GetPropfindInfo (context->docid); 
   
        if (new_davctx) 
         {             
            new_davctx->showIt = NO;
            new_davctx->retry = YES;
            
#ifdef DEBUG_DAV
            fprintf (stderr,"FilterLocked_handler.... finding lock informations \n");

            fprintf (stderr,"FilterLocked_handler.... source (%s) %s\n", 
                             HTAnchor_address (context->source),
                             (HTAnchor_document (HTAnchor_parent(context->source)))?
                             "YES":"NO");       
#endif

            /*sets xmlbody to lockdiscovery*/
            if (!createPropfindBody (YES,new_davctx->xmlbody,DAV_XML_LEN)) 
             {
                AHTDAVContext_delete (new_davctx);
                return HT_OK;
             }
            

            if (context->method != METHOD_LOCK) 
             {
                /* copy info to retry later */
                AHTReqContext *new_request = DAVCopyContext (context);
                DAVSetCopyRequest (new_request, context, new_davctx);
                new_davctx->new_request = new_request;          
             }

            /* create the message to be displayed later */
            new_davctx->error_msgs = AwList_new (2);
            if (new_davctx->error_msgs) 
             {
                msg = (char *)HT_CALLOC (DAV_LINE_MAX, sizeof(char));
                sprintf (msg, TtaGetMessage (AMAYA,AM_LOCKED), new_davctx->relativeURI);
                AwList_put (new_davctx->error_msgs,msg);
             }
            
            
            /* disable the output from this request */
            if (davctx) davctx->showIt = NO;
                
            /* find lock informations with a propfind request */                
            ok = DoPropfindRequest(context->docid,new_davctx, 
                                   (context->method == METHOD_LOCK)?
                                   FilterFindAndShowLock_handler : FilterFindAndPut_handler, NULL);

         } /* new_davctx */

        if (ok!=YES) 
         {
            DAVDisplayMessage (TtaGetMessage (AMAYA,AM_LOCKED),
                               DocumentURLs[context->docid]);
        
            /* we stop the work _ reset stop button */
            ResetStop (context->docid);
    
            return HT_OK;
         }
     } /* context */
    
    return HT_OK;
}




/*----------------------------------------------------------------------
  FilterMultiStatus_handler: After filter for handle 207 Multi-Status
                             status code.
  ---------------------------------------------------------------------- */
int FilterMultiStatus_handler (HTRequest * request, HTResponse * response,
                               void * param, int status)  
{

    AHTReqContext *context = (AHTReqContext *) HTRequest_context (request);
    AHTDAVContext *davctx  = (context)?(AHTDAVContext *)context->dav_context:NULL;
    AHTDAVContext *new_davctx  = NULL; 
    AHTReqContext *new_request = NULL;
    char *msg    = NULL;
    char *reason = NULL;
    char urlName[DAV_LINE_MAX];
    BOOL ok = NO;
    int i = 0;
    char c;
      
    if (context) 
     {
        /* 207 Multi-Status is the normal status code for PROPFIND requests*/
        if (context->method == METHOD_PROPFIND)
            return HT_OK;  
         /* 207 Multi-Status is an error code for LOCK requests */
         else if (context->method == METHOD_LOCK || context->method == METHOD_UNLOCK) 
            return HT_OK;

        new_davctx  = GetPropfindInfo (context->docid); 
        new_request = NULL;
        
        if (new_davctx) 
         {         
            new_davctx->showIt = NO;
            new_davctx->retry = YES;

#ifdef DEBUG_DAV
            fprintf (stderr,"FilterMultiStatus_handler.... finding lock info for %s\n",
                             context->urlName);
#endif
            
            /*sets xmlbody to lockdiscovery*/
            if (!createPropfindBody (YES,new_davctx->xmlbody,DAV_XML_LEN)) 
                    return HT_OK;
            
            /* copy info to retry later */
            new_request = DAVCopyContext (context);
            DAVSetCopyRequest (new_request, context, new_davctx);
            new_davctx->new_request = new_request;
            
            /* continue only if we could copy the request*/ 
            if (new_request) 
             {
                i = 0;
                c = ' ';
                
               /* change the destiny url for the propfind request.
                * Multi-Status status code may be caused by a lock in a ancestral
                * collection, when using depth = 0. Thus, we need to do a propfind 
                * in the resource ancestral collection.
                */ 
                i =  strlen (new_davctx->relativeURI) - 2;
                i = (i<0)?0:i;
                while (i>=0 && new_davctx->relativeURI[i]!='/') 
                     i--; 
                new_davctx->relativeURI[++i] = '\0';

                i =  strlen (context->urlName) - 2;
                i = (i<0)?0:i;
                while (i>=0 && context->urlName[i]!='/') 
                     i--; 
                c = context->urlName[++i];
                context->urlName[i] = '\0';

                strcpy (urlName,context->urlName);
                context->urlName[i] = c;
            
                /* disable the output from this request */
                if (davctx) davctx->showIt = NO;
        
                /* create the message to be displayed later */
                new_davctx->error_msgs = AwList_new (2);
                if (new_davctx->error_msgs) 
                 {
                    msg = (char *)HT_CALLOC (DAV_LINE_MAX, sizeof(char));
                    reason = HTResponse_reason (response);
                    sprintf (msg, TtaGetMessage (AMAYA,AM_MULTI_STATUS_FAIL), 
                                                 (reason)?reason:"Multi-Status Failure");
                    AwList_put (new_davctx->error_msgs,msg);
                 }
            
                                
                /* find lock informations with a propfind request */                    
                ok = DoPropfindRequest(context->docid,new_davctx, 
                                       FilterFindAndPut_handler, urlName);

             } /* new_context */
         } /* new_davctx */

        if (!ok) 
         {
            reason = HTResponse_reason (response);
            DAVDisplayMessage (TtaGetMessage (AMAYA,AM_MULTI_STATUS_FAIL),
                               (reason)?reason:(char *)"Multi-Status Failure");
        
            /* we stop the work _ reset stop button */
            ResetStop (context->docid);
            
            return HT_OK;
         }
        
     } /* context */
    
    return HT_OK;
}


/*----------------------------------------------------------------------
 * terminate callback - presents the request results to the user
  ----------------------------------------------------------------------*/
void DAVTerminate_callback (int doc, int status, char *urlName,char *outputfile,
			    char *proxyName, AHTHeaders *http_headers,
			    void * context)
{                                          
    AHTDAVContext *davctx = (context)? 
                            (AHTDAVContext*)((AHTReqContext*)context)->dav_context
                            :NULL;
    if (davctx) 
     {
        /*show results to the user*/
        if (davctx->showIt)
          DAVShowInfo ((AHTReqContext*)context);            

#ifdef DEBUG_DAV        
        fprintf (stderr, "DAVTerminate_callback.... starting for %s\n",  
                         (urlName)?urlName:"NULL");
        fprintf (stderr, "\tstatus %d\n\tfile: %s\n",status,
                         (outputfile)?outputfile:"NULL");
        fprintf (stderr, "\tcontext: %s\n\tdav_context: %s\n",
                         (context)?"YES":"NO", (davctx)?"YES":"NO");
       fprintf (stderr,"FilterUnlock_handler.... Open requests %d\n",Amaya->open_requests);
#endif    
        /* we stop the work _ reset stop button */
        if (Amaya->open_requests<=1) 
           ResetStop ( ((AHTReqContext*)context)->docid);

     } 
}


/* ********************************************************************* *
 *                              OTHER FILTERS                            *
 * ********************************************************************* */


/*----------------------------------------------------------------------
  FilterFindAndPut_handler: After filter for lock discovery and 
  non-dav operations (specially PUT), when the resource is locked.
  ---------------------------------------------------------------------- */
int FilterFindAndPut_handler (HTRequest * request, HTResponse * response,
                              void * param, int status)  
{

    AHTReqContext  *context  =   (AHTReqContext *)HTRequest_context(request);
    AHTDAVContext  *davctx   =   (context)?(AHTDAVContext *)context->dav_context:NULL;
    LockLine       *lockinfo =   NULL;
    BOOL            ok       =   NO;
    
    char owner[DAV_LINE_MAX];
    char label1[DAV_LINE_MAX],label2[DAV_LINE_MAX];
    char *out, *deb, *uri,*msg, *ptr ;
    
    out = deb = uri = msg = NULL;
     
    if (davctx) 
     {

        /* if we haven't the old request to retry, show message and return */    
        if (!davctx->retry || !davctx->new_request || !davctx->new_request->request) 
         {
            msg = NULL;
            if (davctx->error_msgs) 
             {
                AwList_reset(davctx->error_msgs);
                msg = (char *)AwList_next (davctx->error_msgs);
                if (msg) 
                 {
                    DAVDisplayMessage (msg,NULL);
                    return HT_OK;
                 }
             }       
            DAVDisplayMessage (TtaGetMessage (AMAYA, AM_MULTI_STATUS_FAIL), 
                               (char *)"Locked or Multi-Status Failure");
            return HT_OK;
         }
           
        
        /* get output and debug streams */
        out = (davctx->output)?HTChunk_data (davctx->output):NULL;
        deb = (davctx->debug)?HTChunk_data (davctx->debug):NULL;
        
        /* get original URI */
        uri = DocumentURLs[context->docid];

#ifdef DEBUG_DAV
        fprintf (stderr,"FilterFindAndPut_handler.... status %d\n",status);    
#endif
        
        /*set AHTReqContext object */
        context->anchor = HTRequest_anchor(request);
        
        /*set AHTDAVContext - status*/
        davctx->status = status;

        /* if succed (status 207) set AHTDAVContext - response body tree */
        if (status == HT_MULTI_STATUS && out) 
            davctx->tree = AwParser_umountMessage(out);

        /*find lock informations*/
        if (davctx->tree)
            lockinfo = DAVGetLockFromTree (davctx->tree,owner);

        /*document is locked, ask to*/
        if (lockinfo) 
         {
                    
            /*user if s/he wants to try again */            
            sprintf (label1, TtaGetMessage(AMAYA,AM_LOCKED_BY_OTHER),
                            lockinfo->relativeURI, owner, lockinfo->timeout);
            ptr = DAVBreakString (label1);
            sprintf (label2, TtaGetMessage(AMAYA,AM_LOCK_OVERWRITE));
            
            if (DAVConfirmDialog (context->docid,label1, (ptr)?ptr:(char *)" ", label2))
             {
                /* add this url to the DAV Resources list */
                if (!DAVAllowResource (DocumentURLs[context->docid])) 
                   AddPathInDAVList (context->urlName);

                /* save lock info */
                if (saveLockLine (davctx->absoluteURI, lockinfo)==YES) 
                 {
                    /* retry the request, using davctx->new_request */
                    DAVAddIfHeader (davctx->new_request,
                                    davctx->new_request->urlName);
                    
                    /* add a local filter to set save icon */
                    HTRequest_addAfter (davctx->new_request->request, FilterSave_handler, 
                                        NULL,NULL,HT_ALL,HT_FILTER_LAST,NO); 
                                    
                    /* it was a PUT request */
                    if (davctx->new_request->method == METHOD_PUT && 
                        davctx->new_source) 
                     {
                        ok = HTPutAnchor (davctx->new_source,
                                          davctx->new_request->dest,
                                          davctx->new_request->request);
                     }
                    /* POST request */
                    else if (davctx->new_request->method == METHOD_POST) 
                     {
                         if ((davctx->new_request->mode & AMAYA_FORM_POST) && 
                              davctx->new_request->formdata) 
                          {
                             HTParentAnchor *posted = NULL;
                             posted = HTPostFormAnchor (davctx->new_request->formdata,
                                                 (HTAnchor *)davctx->new_request->anchor,
                                                  davctx->new_request->request);
                             ok = (posted)?YES:NO;
                          }
                         else if ((davctx->new_request->mode & AMAYA_FORM_POST) && 
                                  davctx->new_source) 
                          {
                             ok = HTPostAnchor (davctx->new_source,
                                               (HTAnchor *)davctx->new_request->anchor,
                                                davctx->new_request->request);
                          }
                     }
                    /* GET form */
                    else if (davctx->new_request->formdata) 
                     {
                        ok = HTGetFormAnchor (davctx->new_request->formdata, 
                                             (HTAnchor *)davctx->new_request->anchor,
                                              davctx->new_request->request); 
                     }   

                    TtaHandlePendingEvents ();      
                 } /* save */
             } /*confirm dialog */             
         } /* lockinfo */
            
        /* if status != YES, continue filters */
        if (ok!=YES) 
         {
            msg = NULL;
            
            SavingDocument = 0;
            SavingObject = 0;

            /* force the document to be modified */
            TtaSetDocumentModified (context->docid);
            DocStatusUpdate (context->docid, TRUE);
                            
            /* we finished? reset stop button */
            ResetStop (context->docid);
            
            /* display the correct message */   
            if (davctx->error_msgs) 
             {
                AwList_reset(davctx->error_msgs);
                msg = (char *)AwList_next (davctx->error_msgs);
                if (msg) 
                 {
                    DAVDisplayMessage (msg,NULL);
                    return HT_OK;
                 }
             }       
            DAVDisplayMessage (TtaGetMessage (AMAYA, AM_MULTI_STATUS_FAIL), 
                               (char *)"Locked or Multi-Status Failure");
         }

     }

    return HT_OK;
}


/*----------------------------------------------------------------------
  FilterSave_handler: After filter for non-webdav requests that 
  saved the document using the filter FilterFindAndPut_handler.
  It only set the document status according the request status.
  ---------------------------------------------------------------------- */
int FilterSave_handler (HTRequest * request, HTResponse * response,
                        void * param, int status)  
{
    AHTReqContext * context = (AHTReqContext *)param;
    if (context) 
     {
        SavingDocument = 0;
        SavingObject = 0;       

         /* request succeed, document is saved */
        if (context->method == METHOD_PUT) 
         {
            if (status > 0)
                DocStatusUpdate (context->docid,FALSE);
            else
                DocStatusUpdate (context->docid,TRUE);
         }
     }

    return HT_OK;
}



/* ********************************************************************* *
 *                REDO FUNCTIONS FOR LOCK/UNLOCK REQUEST                 *
 * ********************************************************************* */


/*----------------------------------------------------------------------
   Redo a LOCK/UNLOCK request. 
   Note: It doesn't use this context, just pick up its dav_context object.
  ----------------------------------------------------------------------*/
void DAVRedo (AHTReqContext *context) 
{
    AHTDAVContext *info = (context)?(AHTDAVContext *)context->dav_context:NULL;
    
    if (info) 
     {
        if (context->method == METHOD_LOCK) 
         {
                context->dav_context = NULL;
                DoLockRequest (context->docid, info);    
         }       
        else if (context->method ==  METHOD_UNLOCK) 
         {
                context->dav_context = NULL;
                DoUnlockRequest (context->docid, info);    
         }
     }
}



/* ********************************************************************* *
 *                       FUNCTIONS FOR LOCK REQUEST                      *
 * ********************************************************************* */


/*----------------------------------------------------------------------
   Creates a XML body for LOCK requests
   
   Parameters:
           char *owner: owner element (user reference)
        char *scope: lock scope
        char *body: string where the result will be placed
        int len: lenght of the string "body"

   Returns:
        BOOL: YES if it suceed, and NO if it fails. It will fail if the
              parameter "len" indicates that "body"'s length is smaller
              than we need.
        if succed, the parameter "body" will contain the XML request body.
        
  ----------------------------------------------------------------------*/
BOOL createLockBody (char *owner, char *scope, char *body, int len) 
{
     char tmp[DAV_XML_LEN];
     char nl[3];
     int  l; 
     BOOL status = NO;
     
     sprintf (nl,"%c%c",CR,LF);            
     if (owner && *owner && scope && *scope) 
      {
        sprintf (tmp,"<?xml version=\"1.0\" encoding=\"utf-8\" ?>%s"
                     "<D:lockinfo xmlns:D=\"DAV:\">%s"
                     "<D:lockscope><D:%s/></D:lockscope>%s"
                     "<D:locktype><D:write/></D:locktype>%s"
                     "<D:owner><D:href>%s</D:href></D:owner>%s"
                     "</D:lockinfo>", nl,nl,scope,nl,nl,owner,nl);
        l = strlen (tmp);
         if (body && len > l) 
          {
             /* copy the body value */
            strncpy (body, tmp, len);
            status = YES;
          }
      }
     return status;    
}



/*----------------------------------------------------------------------
   Returns a AHTDAVContext object with the informations needed to
   perform a LOCK request. 
   
   Parameters:
        int document: document 
   Returns:
        AHTDAVContext * object if suceed, or NULL if it fails.        
  ----------------------------------------------------------------------*/
AHTDAVContext * GetLockInfo (int document) 
{
    AHTDAVContext * info = NULL; 
    char *owner, *scope;
    char *depth, *timeout;  
    
    /* getting absolute and relative URI. If NULL, URL probably has not a 
     * "http://" scheme, returning NULL */
    if (!(info = AHTDAVContext_new((const char*)DocumentURLs[document]))) 
        return NULL;
       
    /* user URL, lock scope */
    owner = DAVUserURL;
    scope = DAVLockScope;

    /* mounting LOCK XML body: without the XML body, we can't lock the
     * resource, returning NULL  */
    if (!createLockBody (owner,scope,info->xmlbody,DAV_XML_LEN)) 
     {
        AHTDAVContext_delete (info);
        return NULL;
#ifdef DEBUG_DAV              
     } 
    else 
     {
        fprintf (stderr,"OK\n**%s**\n",info->xmlbody);
#endif
     }
    
    /*depth and timeout headers*/
    depth = DAVDepth;
    timeout = DAVTimeout;
    
    /* setting DAV headers */
    HTDAV_setDepthHeader (info->davheaders,depth);
    HTDAV_setTimeoutHeader (info->davheaders,timeout);
    
    return info;
}




/*----------------------------------------------------------------------
   Executes a LOCK request for the document.
   
   Parameters:
        int doc: document to be locked
        AHTDAVContext *info: informations for the request
   Returns:
        BOOL: YES, if suceed, No, if fails
  ----------------------------------------------------------------------*/
BOOL DoLockRequest (int doc, AHTDAVContext *info) 
{
    AHTReqContext *context = NULL;
    BOOL status = NO;
    char *url = (doc>=0)?DocumentURLs[doc]:NULL;
    
    if (url && *url && info) 
     {
        /*creating request context
         * terminate handler for the request: FilterLock_handler
         * terminate callback : DAVTerminate_callback
         * incremental callback : NULL
         * preemptive : no 
         * mode : AMAYA_ASYNC | AMAYA_FLUSH_REQUEST | AMAYA_NOCACHE
         *
         * about the mode: without ASYNC, the request won't be deleted before
         * the end of the application (request will be kept in memory), 
         * and AMAYA_NOCACHE is  mandatory for DAV requests 
         * */
        context = DAVCreateDefaultContext (doc, url, info, 
                        FilterLock_handler, 
                        (TTcbf *) DAVTerminate_callback, NULL, NO, 
                        (AMAYA_ASYNC | AMAYA_FLUSH_REQUEST | AMAYA_NOCACHE));
                        
        if (context) 
         {
            HTRequest *request = context->request;

#ifdef DEBUG_DAV    
            fprintf (stderr,"DoLockRequest.... setting request\n");
#endif
            
            /*set AHTReqContext fields*/
            context->method = METHOD_LOCK;
           
            /* set status line */
            TtaSetStatus (context->docid, DAV_VIEW, 
                          TtaGetMessage (AMAYA, AM_LOCKING),
                          context->urlName);

            /* 
             * ** Testing: if we didn't set the output format, will the libwww **
             * ** recognize the xml format?                                    **
             * ** It will, but it will take long to process the request        **
             *
             * streams: output (xml) and debug (html).
             */
            
            HTRequest_setOutputFormat (request,HTAtom_for("text/xml"));
            HTRequest_setOutputStream(request, HTStreamToChunk (request, 
                                               &(info->output),0));
    
            HTRequest_setDebugFormat (request, WWW_SOURCE);
            HTRequest_setDebugStream (request, HTStreamToChunk (request,
                                               &(info->debug),0));
                        
#ifdef DEBUG_DAV    
            fprintf (stderr,"DoLockRequest.... executing the request\n");
#endif    

            /* testing if stop button will work here */
            SetStopButton (doc);
            
            /*Doing the request*/
            status = HTLOCKAbsolute(request,url,info->xmlbody,info->davheaders);
         } /* if context */
     }

    return status;
}





/*----------------------------------------------------------------------
  FilterLock_handler: After filter for LOCK request.
  
  ---------------------------------------------------------------------- */
int FilterLock_handler (HTRequest * request, HTResponse * response,
                        void * param, int status)  
{

    char *out, *deb;
    AHTReqContext  *context = (AHTReqContext *)HTRequest_context(request);
    AHTDAVContext  *davctx  = (context)?(AHTDAVContext *)context->dav_context:NULL;
    HTParentAnchor *anchor  = NULL;
    HTAssocList    *headers = NULL;
    char *format = (response)?HTAtom_name(HTResponse_format(response)):(char *)"Unknwon";

    out = deb = NULL;
    if (davctx) 
     {
        out = (davctx->output)?HTChunk_data (davctx->output):NULL;
        deb = (davctx->debug)?HTChunk_data (davctx->debug):NULL;
        
                
#ifdef DEBUG_DAV
        fprintf (stderr,"FilterLock_handler.... status %d\n",status);    
        fprintf (stderr,"FilterLock_handler.... Response format %s\n",format);
        if (out) 
            fprintf (stderr,"FilterLock_handler.... readed %s\n",out);
        else if (deb)
            fprintf (stderr,"FilterLock_handler.... readed %s\n",deb); 
        else 
            fprintf (stderr,"FilterLock_handler.... nothing readed\n");
#endif

        /*set AHTReqContext object */
        anchor = HTRequest_anchor(request);
        context->anchor = anchor;
        
        /* set AHTDAVContext - status */
        davctx->status = status;
        
        /* if succed (status 200 OK), save lock info in base */
        if (status == HT_LOADED && out) 
         {
            headers = HTAnchor_header(anchor);
            
            /* add this url to the DAV Resources list */
            if (!DAVAllowResource (DocumentURLs[context->docid])) 
              AddPathInDAVList (context->urlName);
         
            if (saveLockBase (davctx->absoluteURI,davctx->relativeURI, out, headers)!=YES) 
              DAVDisplayMessage (TtaGetMessage (AMAYA, AM_SAVE_LOCKBASE_FAILED), NULL);
            
            DAVSetLockIndicator (context->docid, 2);
         }
         else if (status == HT_MULTI_STATUS && out) 
          {
            /* set AHTDAVContext - response body tree */ 
            if (!strcasecomp(format,"text/xml")) 
                davctx->tree = AwParser_umountMessage(out);
          }   
         
        /* **** User interface is done by the DAVTerminate_handler **** */
     }
#ifdef DEBUG_DAV    
    else
       fprintf (stderr,"FilterLock_handler.... Error condition: no DAV context\n");
#endif    
   
    /* we finished? reset stop button here */
    ResetStop (context->docid);
    
    return HT_OK;
}




/*----------------------------------------------------------------------
  FilterFindAndShowLock_handler: After filter for lock discovery and 
  show that the ressource is locked. 
  ---------------------------------------------------------------------- */
int FilterFindAndShowLock_handler (HTRequest * request, HTResponse * response,
                                   void * param, int status)  
{

    AHTReqContext  *context  = (AHTReqContext *)HTRequest_context(request);
    AHTDAVContext  *davctx   = (context)?(AHTDAVContext *)context->dav_context:NULL;
    LockLine       *lockinfo = NULL;
    char owner[DAV_LINE_MAX];
    char tmp1[DAV_LINE_MAX];
    char tmp2[DAV_LINE_MAX];
    char tmp3[DAV_LINE_MAX];
    char *ptr, *out, *deb;
    
    out = deb = ptr = NULL;
    
    if (davctx) 
     {
        out = (davctx->output)?HTChunk_data (davctx->output):NULL;
        deb = (davctx->debug)?HTChunk_data (davctx->debug):NULL;

#ifdef DEBUG_DAV
        fprintf (stderr,"FilterFindAndShowLock_handler.... status %d\n",status);    
#endif
     
        /*set AHTReqContext object */
        context->anchor = HTRequest_anchor(request);
        
        /*set AHTDAVContext - status*/
        davctx->status = status;
        
        /* if succed (status 207) set AHTDAVContext - response body tree */
        if (status == HT_MULTI_STATUS && out) 
            davctx->tree = AwParser_umountMessage(out);
      
        /*find lock informations*/
        if (davctx->tree)
            lockinfo = DAVGetLockFromTree (davctx->tree,owner);

        /* show lock informations to the user */
        if (lockinfo) 
         {
            /* mounts the message for the user */
            sprintf (tmp1,TtaGetMessage(AMAYA,AM_LOCKED_BY_OTHER),
                            lockinfo->relativeURI, owner, lockinfo->timeout);
            ptr = DAVBreakString (tmp1);
            sprintf (tmp2,"%s",(ptr)?ptr:" "); 
            sprintf (tmp3,TtaGetMessage(AMAYA,AM_UNLOCK_BEFORE));     
         }
        else 
         {
            /* mounts the message for the user */
            sprintf (tmp1,TtaGetMessage(AMAYA,AM_LOCK_FAILED));
            sprintf (tmp2,TtaGetMessage(AMAYA,AM_LOCKED),context->urlName);
            ptr = DAVBreakString (tmp2);
            sprintf (tmp3,"%s",(ptr)?ptr:" "); 
         }
            
        /* display the message */
        DAVDisplayMultiLineMessage (context->docid,tmp1, tmp2, tmp3);

        /* we finished? reset stop button */
        ResetStop (context->docid);
        
        /* clear new_request from davctx */
        if (davctx->new_request)
           AHTReqContext_delete (davctx->new_request);
     }

    return HT_OK;
}






/* ********************************************************************* *
 *                      FUNCTIONS FOR UNLOCK REQUESTS                    *
 * ********************************************************************* */


/*----------------------------------------------------------------------
   Returns a AHTDAVContext object with the informations needed to
   perform a UNLOCK request. 
   
   Parameters:
        int document: document 
   Returns:
        AHTDAVContext * object if suceed, or NULL if it fails.        
  ----------------------------------------------------------------------*/
AHTDAVContext * GetUnlockInfo (int document) 
{
    AHTDAVContext *info = NULL; 
    char *lock, *depth;  
    
    /*getting absolute and relative URI. If NULL,URL has not an "http://" 
     * scheme, returning NULL  */
    if (!(info = AHTDAVContext_new((const char*)DocumentURLs[document]))) 
        return NULL; 
  
    /* get the lock-token. if there isn't a lock-token, we can't do the
     * request here, returning NULL */
    lock = DAVFindLockToken (info->absoluteURI,info->relativeURI);
    if (!lock) 
     {
        AHTDAVContext_delete (info);
        return NULL;
     }
    
    /* getting depth hearder */
    depth = DAVDepth;                             
   
    /* setting DAV headers */ 
    HTDAV_setDepthHeader (info->davheaders,depth);
    HTDAV_setLockTokenHeader (info->davheaders,lock);

    return info;
}



/*----------------------------------------------------------------------
   Executes a UNLOCK request for the document.
   
   Parameters:
        int doc: document to be locked
        AHTDAVContext *info: informations for the request
   Returns:
        BOOL: YES, if suceed, No, if fails

   Note: when doing a unlock request when exiting the application,
         the request will be killed under Windows machines, and
         under unix/linux machines, it will not, since preemptive
         is true (see DAVCreateDefaultContext parameters). 
  ----------------------------------------------------------------------*/
BOOL DoUnlockRequest (int doc, AHTDAVContext *info) 
{
    AHTReqContext *context=NULL;
    BOOL status = NO;
    char *url = (doc>=0)?DocumentURLs[doc]:NULL;
    
    if (url && *url && info) 
     {
        /* creating: request context
         * 
         * after filter : FilterUnlock_handler
         * terminate callback : DAVTerminate_callback
         * incremantal callback : null
         * preemptive : yes
         * mode : AMAYA_ASYNC | AMAYA_FLUSH_REQUEST | AMAYA_NOCACHE
         * We need AMAYA_ASYNC to delete the request object before  
         * the end of the application (without it, the request will 
         * be kept in memory), and AMAYA_NOCACHE is mandatory for 
         * DAV requests 
         * */
        context = DAVCreateDefaultContext (doc, url, info, 
                  FilterUnlock_handler, 
                  (TTcbf *)DAVTerminate_callback, NULL, YES, 
                  AMAYA_ASYNC | AMAYA_FLUSH_REQUEST | AMAYA_NOCACHE);

        if (context) 
         {
            HTRequest *request = context->request;

#ifdef DEBUG_DAV    
            fprintf (stderr,"DoUnlockRequest.... setting request\n");
#endif
    
            /* set AHTReqContext fields */
            context->method = METHOD_UNLOCK;
           
            /* set status line */
            TtaSetStatus (context->docid, DAV_VIEW, 
                          TtaGetMessage (AMAYA, AM_UNLOCKING),
                          context->urlName);

           /* 
            * ** Testing: if we didn't set the output format, will the libwww **
            * ** recognize the xml format? Yes, but the request is too slow.  **
            * 
            * streams: output (xml) and debug (html)
            */
        
            HTRequest_setOutputFormat (request,HTAtom_for("text/xml"));
            HTRequest_setOutputStream(request, HTStreamToChunk (request, 
                                               &(info->output),0)); 
    
            HTRequest_setDebugFormat (request, WWW_SOURCE);
            HTRequest_setDebugStream (request, HTStreamToChunk (request, 
                                               &(info->debug),0));

#ifdef DEBUG_DAV    
            fprintf (stderr,"DoUnlockRequest.... executing the request\n");
#endif    

            /* set stop button  _ we will work */
            SetStopButton (doc);
                    
            /* Doing the request */
            status = HTUNLOCKAbsolute (request,url,info->davheaders);
         }
     }

    return status;
}




/*----------------------------------------------------------------------
  FilterUnlock_handler: After filter for UNLOCK request.
  
  ---------------------------------------------------------------------- */
int FilterUnlock_handler (HTRequest * request, HTResponse * response,
                          void * param, int status)  
{
        
    AHTReqContext *context = (AHTReqContext *)HTRequest_context(request);
    AHTDAVContext *davctx  = (context)?(AHTDAVContext *)context->dav_context:NULL;
    LockLine *line = NULL;
    BOOL saved = YES;
    char *out, *deb, *lock, *format;
    
    out = deb = lock = format = NULL;
    
    if (davctx) 
     {
        out = (davctx->output)?HTChunk_data (davctx->output):NULL;
        deb = (davctx->debug)?HTChunk_data (davctx->debug):NULL;
#ifdef DEBUG_DAV
        fprintf (stderr,"FilterUnlock_handler.... status %d\n",status);    
        fprintf (stderr,"FilterUnlock_handler.... Response format %s\n", 
                            (response && (HTResponse_format(response)))?
                             HTAtom_name(HTResponse_format(response)):"Unknwon");
        if (out) 
            fprintf (stderr,"FilterUnlock_handler.... readed %s\n",out);
        else if (deb)
            fprintf (stderr,"FilterUnlock_handler.... readed %s\n",deb); 
        else 
            fprintf (stderr,"FilterUnlock_handler.... no content\n");
#endif
        /*set AHTReqContext object */
        context->anchor = HTRequest_anchor(request);
        /*set AHTDAVContext - status*/
        davctx->status = status;
        
        /*remove lock info from base
         * status 204 No content = sucess condition, lock info should be removed
         * status 400 Bad Request = fail condition, lock info is not valid anymore */
        if (status == HT_NO_DATA || status == DAV_BAD_REQUEST) 
         {
            lock = HTDAV_LockTokenHeader (davctx->davheaders); 
            line = LockLine_newObject (davctx->relativeURI,
                                  lock,DAVDepth,DAVTimeout,time(NULL));
            
#ifdef DEBUG_DAV
            fprintf (stderr,"FilterUnlock_handler.... LockLine object\n");
            fprintf (stderr,"\trelative: %s\n\tlocktoken: %s\n",line->relativeURI, line->lockToken);
            fprintf (stderr,"FilterUnlock_handler.... absolute %s\n",davctx->absoluteURI);
#endif            
            if (lock) HT_FREE (lock);
            
            if (removeFromBase (davctx->absoluteURI,line)!=YES) 
             {
                DAVDisplayMessage (TtaGetMessage (AMAYA, AM_REMOVE_LOCKBASE_FAILED), NULL);    
                saved = NO;
             }

            DAVSetLockIndicator (context->docid, 1);
         }
        else if (status == HT_MULTI_STATUS && out) 
         {
            /* if XML response body is available, create the tree */
            format = (response)?HTAtom_name (HTResponse_format(response)):(char *)"Unknown";
            if (format && !strcasecomp (format,"text/xml")) 
                davctx->tree = AwParser_umountMessage (out);
         }
    
        /* **** User interface is done by the DAVTerminate_handler **** */
    }
#ifdef DEBUG_DAV    
    else 
       fprintf (stderr,"FilterUnlock_handler.... Error condition: no DAV context\n");
#endif

    return HT_OK;
}




/*----------------------------------------------------------------------
  ForceUnlockRequest: Look for lock information to do the unlock request.
  
  ---------------------------------------------------------------------- */
void ForceUnlockRequest (Document document) 
{
   AHTDAVContext *davctx = NULL;

#ifdef DEBUG_DAV
   fprintf (stderr,"ForceUnlockRequest.... finding lock info for %s\n",
                   DocumentURLs[document]);
#endif
   
   /*basic info for the request*/
   davctx = GetPropfindInfo (document); 
   
   if (davctx) 
    {
      davctx->showIt = NO;
      /*sets xmlbody to lockdiscovery*/
      if (createPropfindBody (YES,davctx->xmlbody,DAV_XML_LEN)) 
          DoPropfindRequest (document,davctx,FilterFindAndUnlock_handler,NULL);
      else 
          AHTDAVContext_delete (davctx);
    }
}



/*----------------------------------------------------------------------
  FilterFindAndUnlock_handler: After filter for lock discovery and 
  unlock the ressource. 
  ---------------------------------------------------------------------- */
int FilterFindAndUnlock_handler (HTRequest * request, HTResponse * response,
                                 void * param, int status)  
{

    AHTReqContext  *context    = (AHTReqContext *)HTRequest_context(request);
    AHTDAVContext  *davctx     = (context)?(AHTDAVContext *)context->dav_context:NULL;
    AHTDAVContext  *new_davctx = NULL;
    LockLine       *lockinfo   = NULL;
    char label1[DAV_LINE_MAX],label2[DAV_LINE_MAX];
    char owner[DAV_LINE_MAX];
    char msg[DAV_LINE_MAX];
    char *out, *deb, *ptr;
    BOOL ok = NO;
    
    out = deb = NULL;
    
    if (davctx) 
     {
        out = (davctx->output)?HTChunk_data (davctx->output):NULL;
        deb = (davctx->debug)?HTChunk_data (davctx->debug):NULL;

#ifdef DEBUG_DAV
        fprintf (stderr,"FilterFindAndUnlock_handler.... status %d\n",status);    
#endif

        /*set AHTReqContext object */
        context->anchor = HTRequest_anchor(request);
        
        /*set AHTDAVContext - status*/
        davctx->status = status;
      
        /* if succed (status 207) set AHTDAVContext - response body tree */
        if (status == HT_MULTI_STATUS && out) 
            davctx->tree = AwParser_umountMessage(out);

        /*find lock informations*/
        if (davctx->tree)
            lockinfo = DAVGetLockFromTree (davctx->tree,owner);

        /*document is locked, ask to*/
        if (lockinfo && !davctx->retry) 
         {
            
            /*user if s/he wants unlock it*/        
            sprintf (label1,TtaGetMessage(AMAYA,AM_LOCKED_BY_OTHER),
                            lockinfo->relativeURI, owner, lockinfo->timeout);
            ptr = DAVBreakString (label1);
            sprintf (label2,TtaGetMessage(AMAYA,AM_LOCK_OVERWRITE));
            
            if (DAVConfirmDialog (context->docid,label1,(ptr)?ptr:(char *)" ", label2))
             {
                /* save lock info */
                if (saveLockLine (davctx->absoluteURI, lockinfo)==YES) 
                 {
                    new_davctx = GetUnlockInfo (context->docid);
                    if (new_davctx) 
                     {
                        new_davctx->retry = YES;
                        ok = DoUnlockRequest (context->docid, new_davctx);
                     } 
                 }
             }

            if (ok!=YES) 
                sprintf (msg,"%s",TtaGetMessage (AMAYA, AM_UNLOCK_FAILED));
            
         } /* if lockinfo && !davctx->retry */
        else  /* file is unlocked */
            sprintf (msg,TtaGetMessage (AMAYA, AM_UNLOCKED), davctx->relativeURI);
                
            
        /* if ok != YES, continue filters */
        if (ok!=YES) 
         {
            DAVDisplayMessage (msg,NULL);
        
            /* we stop the work _ reset stop button */
            ResetStop (context->docid);
    
            return HT_OK;
         }
        
     }

    return HT_OK;
}



/* ********************************************************************* *
 *                    FUNCTIONS FOR PROPFIND REQUEST                     *
 * ********************************************************************* */


/*----------------------------------------------------------------------
   Creates a XML body for PROPFIND requests
   
   Parameters:
        BOOL lockdiscovery: YES, if it is a lock discovery, NO, if it
                            is a allprop request.
        char *body: string where the result will be placed
        int len: lenght of the string "body"

   Returns:
        BOOL: YES if it suceed, and NO if it fails. It will fail if the
              parameter "len" indicates that "body"'s length is smaller
              than we need.
        if succed, the parameter "body" will contain the XML request body.
        
  ----------------------------------------------------------------------*/
BOOL createPropfindBody (BOOL lockdiscovery, char *body, int len) 
{
     char tmp[DAV_XML_LEN];
     char nl[3];
     int i;
     BOOL status = NO;
     
     sprintf (nl,"%c%c",CR,LF);     
            
     sprintf (tmp,"<?xml version=\"1.0\" encoding=\"utf-8\" ?>%s"
                     "<D:propfind xmlns:D=\"DAV:\">%s"
                     "%s%s"
                     "</D:propfind>", nl,nl,
                     (lockdiscovery)?"<D:prop><D:lockdiscovery/></D:prop>":
                     "<D:allprop/>",nl);
         
     if ( body!=NULL && (unsigned) len > strlen (tmp)) 
      {
         /* clean the memory */
         for (i=0;i<len;i++) body[i]='\0';
         /* copy the body value */
         sprintf (body,"%s%c",tmp,'\0');  
         status = YES;
      }
     
     return status;    
}



/*----------------------------------------------------------------------
   Returns a AHTDAVContext object with the informations needed to
   perform a PROPFIND request. 
   
   Parameters:
        int document: document 
   Returns:
        AHTDAVContext * object if suceed, or NULL if it fails.        
  ----------------------------------------------------------------------*/
AHTDAVContext * GetPropfindInfo (int document) 
{
    AHTDAVContext * info = NULL; 
    
    /*getting absolute and relative URI. if NULL, URL has not an "http://"
     * scheme, returning NULL */
    if (!(info = AHTDAVContext_new((const char*)DocumentURLs[document])))
        return NULL;

    /* mounting LOCK XML body. without the XML body, we can't do a 
     * propfind, returning NULL  */
    if (!createPropfindBody (NO,info->xmlbody,DAV_XML_LEN)) 
     {
        AHTDAVContext_delete (info); 
        return NULL;
     }
    
    /*setting DAV headers: depth 0 to get only resource's props */
    HTDAV_setDepthHeader (info->davheaders,"0");

    return info;
}



/*----------------------------------------------------------------------
   DAVLockDiscovery - do a Lock discovery in the document URL (using a
   propfind request); This request should cause the update of
   the DAVLockIndicatorState variable. 
   
  ----------------------------------------------------------------------*/
void DAVLockDiscovery (Document document)
{
   AHTDAVContext *davctx = NULL;
#ifdef DEBUG_DAV
   fprintf (stderr,"DAVLockDiscovery.... Executing a lock discovery in %s\n",
                   DocumentURLs[document]);
#endif

   if (!DAVAllowResource (DocumentURLs[document])) 
    {           
       DAVSetLockIndicator (document, 0);
       return;
    }
   
   /*basic info for the request*/
   davctx = GetPropfindInfo (document);
   if (davctx) 
    {
      davctx->showIt = NO; 
      /*sets xmlbody to lockdiscovery*/
      if (createPropfindBody (YES,davctx->xmlbody,DAV_XML_LEN)) 
          DoPropfindRequest (document,davctx,FilterFindLock_handler,NULL);      
    }
}



/*----------------------------------------------------------------------
  FilterFindLock_handler: After filter for Lock discovery.

  ---------------------------------------------------------------------- */
int FilterFindLock_handler (HTRequest * request, HTResponse * response,
                            void * param, int status)  
{

    AHTReqContext *context = (AHTReqContext *)HTRequest_context(request);
    AHTDAVContext *davctx  = (context)?(AHTDAVContext *)context->dav_context:NULL;
#ifdef DEBUG_DAV
    const char    *format  = (response)?HTAtom_name(HTResponse_format(response)):"Unknwon";
#endif /* DEBUG_DAV */
    HTList        *matches = NULL;
    char *lockdiscovery    = NULL;
    LockLine     *lockinfo = NULL;
    LockLine     *line     = NULL;
    char label1[DAV_LINE_MAX], label2[DAV_LINE_MAX];
    char owner[DAV_LINE_MAX];
    char *out, *deb, *ptr;
    BOOL saved = NO;

    out = deb = NULL;

    if (davctx) 
     {
        out = (davctx->output)?HTChunk_data (davctx->output):NULL;
        deb = (davctx->debug)?HTChunk_data (davctx->debug):NULL;

#ifdef DEBUG_DAV
        fprintf (stderr,"FilterFindLock_handler.... status %d\n",status);    
        fprintf (stderr,"FilterFindLock_handler.... Response format %s\n",format);
#endif

        /*set AHTReqContext object */
        context->anchor = HTRequest_anchor(request);
        
        /*set AHTDAVContext - status*/
        davctx->status = status; 
        /* if succed (status 207), set AHTDAVContext - response body tree */ 
        if (status == HT_MULTI_STATUS && out) 
            davctx->tree = AwParser_umountMessage(out);

        /*find lockdiscovery element*/
        if (davctx->tree)
            lockdiscovery = AwParser_searchInTree (davctx->tree, "lockdiscovery");
        
        /* there is a lockdiscovery element in XML response, 
         * the resource is locked*/  
        if (lockdiscovery) 
         { 
            matches = NULL;
            saved = NO;
            
            DAVSetLockIndicator (context->docid, 2);
#ifdef DEBUG_DAV
            fprintf (stderr,"FilterFindLock_handler.... found %s, "
                            "resource is locked\n",lockdiscovery);
#endif        
            /* try to discover who is the lock's owner */
            lockinfo = DAVGetLockFromTree (davctx->tree, owner);
            if (lockinfo && owner[0]!=EOS) 
             {
                /* if the user wants to receive awareness information,
                 * notify him/her about locked resources
                 */
                if (DAVAwareness) 
                 {
                    matches = searchLockBase (davctx->absoluteURI, davctx->relativeURI);
                
                    /* if there is no lock info in the base, the user is not 
                     * the lock owner. So, we need to notify him 
                     */
                    if (!matches) 
                     {

                        sprintf (label1,TtaGetMessage(AMAYA,AM_LOCKED_BY_OTHER),\
                                 lockinfo->relativeURI, owner, lockinfo->timeout);
                        ptr = DAVBreakString (label1);
                        sprintf (label2,TtaGetMessage(AMAYA,AM_COPY_LOCK));
            
                        if (DAVConfirmDialog (context->docid,label1, (ptr)?ptr:(char *)" ", label2)) 
                         {
                           if (!DAVAllowResource (DocumentURLs[context->docid])) 
                             /* add this url to the DAV Resources list */
                             AddPathInDAVList (context->urlName);

                            /* save lock info */
                            if (saveLockLine (davctx->absoluteURI, lockinfo)!=YES) 
                                DAVDisplayMessage (TtaGetMessage (AMAYA, AM_SAVE_LOCKBASE_FAILED), NULL);
                            else
                                saved = YES;
                         }
                     
                     }/*!matches*/
                 } /*DAVAwareness */
             } /*lockinfo*/
         }/*lockdiscovery*/
         else
          { /* the resource is unlocked */ 
            line = LockLine_newObject (davctx->relativeURI, (char*)" ", DAVDepth, DAVTimeout,time(NULL));
#ifdef DEBUG_DAV
            fprintf (stderr,"FilterFindLock_handler.... not found, " "resource is unlocked\n");
#endif            
            /* use this information to clean the lock base from 
             * expired locks */ 
            DAVSetLockIndicator (context->docid, 1);
            if (davctx->status > 0) 
             {
                removeFromBase (davctx->absoluteURI,line);
#ifdef DEBUG_DAV
                fprintf (stderr,"FilterFindLock_handler.... base cleaned\n");
#endif            
             }
         }
        /* we finished? reset stop button 
         * We need this reset here to avoid problems due the pipelining */
        ResetStop (context->docid);
    }
    
    return HT_OK;
}



/*----------------------------------------------------------------------
   Executes a PROPFIND request for the document.
   
   Parameters:
        int doc: document to be locked
        AHTDAVContext *info: informations for the request
        HTNetAfter *filter_handler: filter to handler server response
        char *urlName: if it is set, execute the request over this url
   Returns:
        BOOL: YES, if suceed, No, if fails
  ----------------------------------------------------------------------*/
BOOL DoPropfindRequest (int doc, AHTDAVContext *info, HTNetAfter * filter_handler, char *urlName) 
{
    AHTReqContext *context = NULL;
    HTRequest     *request = NULL;
    BOOL status = NO;
    char *url = (doc>=0)?DocumentURLs[doc]:NULL;
    
    if (url && *url && info) 
     {
         /*if there is urlName, use it as url */
         if (urlName)
              url = urlName;

        /* Creating request context 
         * after filter - terminate handler : filter_handler
         * terminate callback : DAVTerminate_callback
         * incremental callback : null
         * preemptive : no
         * mode : AMAYA_ASYNC | AMAYA_FLUSH_REQUEST | AMAYA_NOCACHE
         * */
        context = DAVCreateDefaultContext (doc,url, info, 
                      filter_handler,(TTcbf *)DAVTerminate_callback, NULL, NO,
                      AMAYA_ASYNC | AMAYA_FLUSH_REQUEST | AMAYA_NOCACHE);
        
        if (context) 
         {
            request = context->request;

#ifdef DEBUG_DAV    
            fprintf (stderr,"DoPropfindRequest.... setting request\n");
#endif
            /* set AHTReqContext fields */
            context->method = METHOD_PROPFIND;

            /* set status line */
            TtaSetStatus (context->docid, DAV_VIEW, 
                          TtaGetMessage (AMAYA, AM_FINDING_PROPERTIES),
                          context->urlName);

            /* a successful propfind request always returns an xml body
             * streams: output (xml) and debug (html) */
            HTRequest_setOutputFormat (request,HTAtom_for("text/xml"));
            HTRequest_setOutputStream(request, HTStreamToChunk (request, &(info->output),0));
    
            HTRequest_setDebugFormat (request, WWW_SOURCE);
            HTRequest_setDebugStream (request, HTStreamToChunk (request, &(info->debug),0));
                        
#ifdef DEBUG_DAV    
            fprintf (stderr,"DoPropfindRequest.... executing the request\n");
#endif    

            /* we start the work _ set stop button */
            SetStopButton (doc);
            
            /*Doing the request*/
            status = HTPROPFINDAbsolute(request,url,info->xmlbody,info->davheaders);
         }
     }

    return status;
}





/*----------------------------------------------------------------------
  FilterPropfind_handler: After filter for normal PROPFIND requests.
  
  ---------------------------------------------------------------------- */
int FilterPropfind_handler (HTRequest * request, HTResponse * response,
                            void * param, int status)  
{

    AHTReqContext *context = (AHTReqContext *)HTRequest_context(request);
    AHTDAVContext *davctx  = (context)?(AHTDAVContext *)context->dav_context:NULL;
#ifdef DEBUG_DAV
    const char    *format  = (response)?HTAtom_name(HTResponse_format(response)):"Unknwon";
#endif /* DEBUG_DAV*/
    char *out, *deb;

    out = deb = NULL;
    
    if (davctx) 
     {
        out = (davctx->output)?HTChunk_data (davctx->output):NULL;
        deb = (davctx->debug)?HTChunk_data (davctx->debug):NULL;

#ifdef DEBUG_DAV
        fprintf (stderr,"FilterPropfind_handler.... status %d\n",status);    
        fprintf (stderr,"FilterPropfind_handler.... Response format %s\n",format);
                             
        if (deb)
            fprintf (stderr,"FilterPropfind_handler.... readed %s\n",deb); 
        else 
            fprintf (stderr,"FilterPropfind_handler.... nothing readed\n");
#endif
        
        /*set AHTReqContext object */
        context->anchor = HTRequest_anchor(request);
        
        /* set AHTDAVContext - status */
        davctx->status = status;
         
        /* if succed (status 207), set AHTDAVContext - response body tree */
        if (status == HT_MULTI_STATUS && out) 
            davctx->tree = AwParser_umountMessage(out);
        

        /* **** User interface is done by the DAVTerminate_handler **** */
        
        /* done? reset stop button 
         * We need this reset here to avoid problems due the pipelining */
        ResetStop (context->docid);
    }
#ifdef DEBUG_DAV    
    else 
       fprintf (stderr,"FilterPropfind_handler.... Error condition: no DAV context\n");    
#endif
   
    
    return HT_OK;
}


/*----------------------------------------------------------------------
   FilterCopyLockInfo - filter to copy the lock information from a Lock 
   discovery request in the document URL
  ---------------------------------------------------------------------- */
int FilterCopyLockInfo_handler (HTRequest *request, HTResponse *response,
                                void *param, int status) 
{

    AHTReqContext *context  = (AHTReqContext *)HTRequest_context(request);
    AHTDAVContext *davctx   = (context)?(AHTDAVContext *)context->dav_context:NULL;
#ifdef DEBUG_DAV
    const char    *format   = (response)?HTAtom_name(HTResponse_format(response)):"Unknwon";
#endif /* DEBUG_DAV */
    LockLine      *lockinfo = NULL;
    char owner[DAV_LINE_MAX];
    char *out, *deb;

    out = deb = NULL;

    if (davctx) 
     {
        out = (davctx->output)?HTChunk_data (davctx->output):NULL;
        deb = (davctx->debug)?HTChunk_data (davctx->debug):NULL;

#ifdef DEBUG_DAV
        fprintf (stderr,"FilterCopyLockInfo_handler.... status %d\n",status);    
        fprintf (stderr,"FilterCopyLockInfo_handler.... Response format %s\n",format);
#endif

        /*set AHTReqContext object */
        context->anchor = HTRequest_anchor(request);
        
        /*set AHTDAVContext - status*/
        davctx->status = status; 
      
        /* if succed (status 207), set AHTDAVContext - response body tree */ 
        if (status == HT_MULTI_STATUS && out) 
            davctx->tree = AwParser_umountMessage(out);
        

        /*find lockdiscovery element*/
        if (davctx->tree)
            lockinfo = DAVGetLockFromTree (davctx->tree,owner);
        
        /* there is a lockdiscovery element in XML response, 
         * the resource is locked*/  
        if (lockinfo) 
        { 
#ifdef DEBUG_DAV
            fprintf (stderr,"FilterCopyLockInfo_handler.... found lock, "
                            "resource is locked to %s\n",owner);
#endif            
            DAVSetLockIndicator(context->docid, 2);
            
            /* add this url to the DAV Resources list */
            if (!DAVAllowResource (DocumentURLs[context->docid])) 
                AddPathInDAVList (context->urlName);
            /* save the lock info in local base */
            if (!saveLockLine (davctx->absoluteURI, lockinfo)) 
             {
                DAVDisplayMessage (TtaGetMessage (AMAYA, AM_SAVE_LOCKBASE_FAILED), NULL);
                davctx->showIt = NO;
             }
         }
        else 
         { /* the resource is unlocked */ 
#ifdef DEBUG_DAV
            fprintf (stderr,"FilterFindLock_handler.... not found, "
                            "resource is unlocked\n");
#endif      
            DAVDisplayMessage (TtaGetMessage (AMAYA, AM_UNLOCKED), davctx->relativeURI);      
            davctx->showIt = NO;
            DAVSetLockIndicator(context->docid, 1);
         }
        
        /* we finished? reset stop button 
         * We need this reset here to avoid problems due the pipelining */
        ResetStop (context->docid);
     }
    
    return HT_OK;
}






