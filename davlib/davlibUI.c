/*  --------------------------------------------------------
 ** 
 ** File: davlib.c - WebDAV module _ user interface functions
 **
 ** This file implements the user interface for the WebDAV 
 ** functions into Amaya editor.
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
 ** Revision 1.9  2002-06-13 16:10:13  kirschpi
 ** New dialogue "WebDAV Preferences"
 ** Corrections due last commit by JK
 ** Manuele
 **
 ** Revision 1.8  2002/06/13 13:40:32  kahan
 ** JK: Changed MAX_LINE to DAV_MAX_LINE. MAX_LINE is a reserved macro and
 ** the code was generating a warning.
 **
 ** Revision 1.7  2002/06/12 10:29:07  kirschpi
 ** - adjusts in code format
 ** - new commentaries
 ** - small fixes
 ** Manuele
 **
 ** Revision 1.6  2002/06/06 17:10:46  kirschpi
 ** Breaking the user messages in three lines
 ** Fixing some code format problems
 ** Fixing DAVLockIndicator, when Lock discovery is disabled.
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
 ** Revision 1.4  2002/06/04 10:05:14  cvs
 ** JK: Removed a windows warning
 **
 ** Revision 1.3  2002/06/04 08:15:43  cvs
 ** JK: removing Windows compiler warnings
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

#include "davlib.h"
#include "davlib_f.h"
#include "davlibUI_f.h"
#include "davlibCommon_f.h"
#include "davlibRequests_f.h"

#include "init_f.h"
#include "query_f.h"
#include "MENUconf_f.h"


/* ********************************************************************* *
 *                         PRIVATE VARIABLES                             *
 * ********************************************************************* */

static char textUserReference[DAV_LINE_MAX];
static char textUserResources[DAV_LINE_MAX];
static char radioDepth[DAV_LINE_MAX];
static char radioTimeout[DAV_LINE_MAX];
static int  numberTimeout;
static char radioLockScope[DAV_LINE_MAX];
static ThotBool toggleAwareness1;
static ThotBool toggleAwareness2;
static int  DAVPreferencesBase;



/* ********************************************************************* *
 *                       PRESENTATION FUNCTIONS                          *
 * ********************************************************************* */


/*----------------------------------------------------------------------
  DAVSetLockIndicator: set the Lock indicator button.
  ---------------------------------------------------------------------- */
void DAVSetLockIndicator (Document docid) 
{
   /* updates Lock indicator*/ 
   TtaSetToggleItem (docid,DAV_VIEW,Cooperation_,
                          BLockIndicator,DAVLockIndicatorState);  
}




/*----------------------------------------------------------------------
  DAVDisplayMessage: display a message to user.
  ---------------------------------------------------------------------- */
void DAVDisplayMessage (char *msg, char *arg) 
{
    if (msg && *msg) 
     {
        if (arg && *arg) 
         {
            char label[DAV_LINE_MAX];
            sprintf (label,msg,arg);
            InitInfo (" ",label);
         }
        else 
         {
            InitInfo (" ",msg);
         }
     }
}


/*----------------------------------------------------------------------
  DAVDisplayMultiLineMessage: display message with 3 lines.
  ---------------------------------------------------------------------- */
void DAVDisplayMultiLineMessage (Document docid, char *line1, char *line2, char *line3) 
{
    if (line1 && line2 && line3) 
        InitConfirm3L (docid, DAV_VIEW, line1,line2, line3, FALSE);
}


/*----------------------------------------------------------------------
  DAVConfirmDialog: display a "Confirm/Cancel" dialog to user.
  ---------------------------------------------------------------------- */
BOOL DAVConfirmDialog (Document docid, char *msg1, char *msg2, char *msg3) 
{

        InitConfirm3L (docid, DAV_VIEW, msg1,msg2, msg3, TRUE);
        if (UserAnswer) 
            return YES;
        else
            return NO;        
}



/*----------------------------------------------------------------------
  DAVPropertiesVerticalDialog: display a "Confirm" vertical 
                               dialog for XML DAV properties.

  Parameters : 
        Document docid : document
        char * title : window title
        char * rheader : a header message for the rigth column
        char * lheader : a header message for the left column
        AwList * list : a list with name and value strings

  THE LIST AND ITS COMPONENTES ARE DESTROYED INSIDE THIS FUNCTION!!!!
  ---------------------------------------------------------------------- */
void DAVPropertiesVerticalDialog (Document docid, char *title, char *rheader, 
                                  char *lheader, AwList *list) 
{
        
    int i=MAX_REF+1, form=MAX_REF, lines=0;
	
#ifndef _WINDOWS
    char *name, *value;
    
    char label[DAV_LINE_MAX];
    char *ns = NULL;
#endif /* ! _WINDOWS */

    if (docid>0 && list) 
     {
        lines = AwList_size(list)/2;
             
#ifndef _WINDOWS           
        /* Main form */
        TtaNewSheet (BaseDialog + form, 
                     TtaGetViewFrame (docid, DAV_VIEW), 
                     (title)?title:" ", 0, NULL, FALSE, 
                     (rheader && lheader)?lines+3:lines+1, 'L', D_DONE);

        if (rheader || lheader) 
         {
            TtaNewLabel (BaseDialog + (i++), BaseDialog + form,
                         (lheader)?lheader:" ");
            TtaNewLabel (BaseDialog + (i++), BaseDialog + form,
                         "----------------");
         }

        /* properties: names */ 
        while ( list && (name=(char *)AwList_next(list)) 
                     && (value=(char *)AwList_next(list)) ) 
         {
               
           /* ignore 'namespace:' in property name */
           if ((ns = strchr (name,':'))) 
               sprintf (label,"%s : ",(++ns));
           else
               sprintf (label,"%s : ",name);

           /* property name */
           TtaNewLabel (BaseDialog + (i++), BaseDialog + form, label);
         }                              
            
        AwList_reset (list);
            
        TtaNewLabel (BaseDialog + (i++), BaseDialog + form,
                     "              ");

        if (rheader || lheader) 
         {
            TtaNewLabel (BaseDialog + (i++), BaseDialog + form,
                         (rheader)?rheader:" ");
            TtaNewLabel (BaseDialog + (i++), BaseDialog + form,
                         "------------------------------");
         }
            
        /* properties: values */ 
        while ( list && (name=(char *)AwList_next(list)) 
                     && (value=(char *)AwList_next(list)) ) 
         {
            ns = NULL; 
              
            /* for some properties, the value has namespace. ignore it */ 
            if ( ( HTStrCaseStr (name,"lockscope") ||
                  HTStrCaseStr (name,"locktype")) && \
                (ns = strchr (value,':'))!=NULL ) 
                sprintf (label, "%s ",(++ns));
            else 
                sprintf (label, "%s ",value);
               

            /* property value */
            TtaNewLabel (BaseDialog + (i++), BaseDialog + form, label);
         
            HT_FREE (name);
            HT_FREE (value);               
         }

        AwList_delete (list);
                
        TtaNewLabel (BaseDialog + (i++), BaseDialog + form,
                     "              ");

            
        TtaSetDialoguePosition ();
        TtaShowDialogue (BaseDialog + form, TRUE);            
#endif /* ! _WINDOWS */
      }
}





/*----------------------------------------------------------------------
  DAVHorizontalDialog: display a "Confirm" horizontal dialog

  Parameters : 
        Document docid : document
        char * title : window title
        char * rheader : a header message for the rigth column
        char * lheader : a header message for the left column
        AwList * list : a list with name and value strings

  THE LIST AND ITS COMPONENTES ARE DESTROYED INSIDE THIS FUNCTION!!!!
  ---------------------------------------------------------------------- */
void DAVHorizontalDialog (Document docid, char *title, char *rheader, 
                          char *lheader, AwList *list) 
{
        
    int i=MAX_REF+1, form=MAX_REF;
#ifndef _WINDOWS
    char *name, *value;    
    char label[DAV_LINE_MAX];
#endif /* _WINDOWS */

    if (docid>0 && list) 
     {
             
#ifndef _WINDOWS           
        /* Main form */
        TtaNewSheet (BaseDialog + form, 
                     TtaGetViewFrame (docid, DAV_VIEW), 
                     (title)?title:" ", 0, NULL, TRUE, 
                     2, 'L', D_DONE);

        if (rheader || lheader) 
         {
            TtaNewLabel (BaseDialog + (i++), BaseDialog + form,
                         (lheader)?lheader:" ");
            TtaNewLabel (BaseDialog + (i++), BaseDialog + form,
                         (rheader)?rheader:" ");
            TtaNewLabel (BaseDialog + (i++), BaseDialog + form,
                         "----------------");
            TtaNewLabel (BaseDialog + (i++), BaseDialog + form,
                         "---------------------------");
         }

        /* names  values */ 
        while ( list && (name=(char *)AwList_next(list)) 
                     && (value=(char *)AwList_next(list)) ) 
         {
               
           /* name  */
           sprintf (label,"%s ",name);
           TtaNewLabel (BaseDialog + (i++), BaseDialog + form, label);
                                     
           sprintf (label, "%s ",value);
           TtaNewLabel (BaseDialog + (i++), BaseDialog + form, label);
           
           HT_FREE (name);
           HT_FREE (value);               
         }

        AwList_delete (list);
                
        TtaNewLabel (BaseDialog + (i++), BaseDialog + form,
                     "              ");
        TtaNewLabel (BaseDialog + (i++), BaseDialog + form,
                     "              ");

        TtaSetDialoguePosition ();
        TtaShowDialogue (BaseDialog + form, TRUE);   
#endif
      }
}




/*----------------------------------------------------------------------
   DAVShowInfo: shows the request's results for the user.
  ---------------------------------------------------------------------- */
void DAVShowInfo (AHTReqContext *context) 
{
    AHTDAVContext *davctx = NULL;
    char *status_msg = NULL;
    
#ifdef DEBUG_DAV        
    printf ("****** DAVShowInfo ****** \n");
#endif

    if (context) 
     { 
        davctx = (AHTDAVContext*) context->dav_context;
        
        if (davctx) 
         {  /* it's a WebDAV request */
            
            switch (context->method) 
             {
                    
                /* **** LOCK requests **** */
                case METHOD_LOCK:      
                    /* Normal results */
                    if (davctx->status > 0 && davctx->status != HT_MULTI_STATUS) 
                     {                            
                        /* lock succeed */                                               
                        DAVDisplayMessage (TtaGetMessage (AMAYA, AM_LOCK_SUCCEED),NULL);
                        
                        /*set the status line */
                        status_msg = TtaGetMessage (AMAYA, AM_LOCK_SUCCEED);

                     }
                     /* 207 Multi-Status - Error! */
                    else if (davctx->status == HT_MULTI_STATUS) 
                     {
                        DAVShowMultiStatusInfo (context);
                        /*set the status line */
                        status_msg = TtaGetMessage (AMAYA, AM_LOCK_FAILED);

                     }
                    /*405 Method not allowed*/
                    else if (davctx->status == DAV_METHOD_NOT_ALLOWED) 
                     {
                        DAVDisplayMessage (TtaGetMessage (AMAYA, AM_DAV_NOT_ALLOWED),
                                           context->urlName);
                     }
                    /* -400 Bad Request */
                    else if (davctx->status == DAV_BAD_REQUEST) 
                     {
                        /* It may happens when user interrupts the request,
                         * and the XML body is not sent to the server */
#ifdef DEBUG_DAV                                
                        fprintf (stderr,"BAD REQUEST - XML body not sent\n");
#endif 
                        DAVDisplayMessage (TtaGetMessage (AMAYA, AM_LOCK_FAILED),NULL);
                        status_msg = TtaGetMessage (AMAYA, AM_LOCK_FAILED);
                     }                                        
                    else 
                     {  /* other error codes */    
                        DAVDisplayMessage (TtaGetMessage (AMAYA, AM_LOCK_FAILED),NULL);
                        
                        /* set the status line */
                        status_msg = TtaGetMessage (AMAYA, AM_LOCK_FAILED);
                     }
                    
                    break;
                   
                    
                /* **** UNLOCK requests **** */
                case METHOD_UNLOCK:
                    /* Normal results */
                    if (davctx->status > 0 && davctx->status != HT_MULTI_STATUS) 
                     {
                        DAVDisplayMessage (TtaGetMessage (AMAYA, AM_UNLOCK_SUCCEED), NULL);
                        
                        /*set the status line */
                        status_msg = TtaGetMessage (AMAYA, AM_UNLOCK_SUCCEED);
                        
                     }
                    /*405 Method not allowed*/
                    else if (davctx->status == DAV_METHOD_NOT_ALLOWED) 
                     {
                        DAVDisplayMessage (TtaGetMessage (AMAYA, AM_DAV_NOT_ALLOWED),
                                           context->urlName);
                     }
                    /*-400 Bad Request */
                    else if (davctx->status == DAV_BAD_REQUEST) 
                     {
                        DAVDisplayMessage (TtaGetMessage (AMAYA, AM_UNLOCK_FAILED),NULL); 

                        /*set the status line */
                        status_msg = TtaGetMessage (AMAYA, AM_UNLOCK_FAILED);
                     }
                    else 
                     { /* other error codes */    
                        DAVDisplayMessage (TtaGetMessage (AMAYA, AM_UNLOCK_FAILED), NULL);
                        
                        /*set the status line */
                        status_msg = TtaGetMessage (AMAYA, AM_UNLOCK_FAILED);
                     }
                    
                    break;

                    
               /* **** PROPFIND requests **** */
                case METHOD_PROPFIND:
                    /* 207 Multi-Status - normal case */
                    if (davctx->status == HT_MULTI_STATUS) 
                     {
                        DAVShowPropfindInfo(context);    
                     }
                    /*405 Method not allowed*/
                    else if (davctx->status == DAV_METHOD_NOT_ALLOWED) 
                     {
                        DAVDisplayMessage (TtaGetMessage (AMAYA, AM_DAV_NOT_ALLOWED),
                                           context->urlName);
                     }
                    /* -400 Bad Request */
                    else if (davctx->status == DAV_BAD_REQUEST) 
                     {
                        /* It may happens when user interrupts the request,
                         * and the XML body is not sent to the server */
#ifdef DEBUG_DAV                                
                        fprintf (stderr,"BAD REQUEST - XML body not sent\n");
#endif 
                        
                        DAVDisplayMessage (TtaGetMessage (AMAYA, AM_PROPFIND_FAILED_TEMP),NULL);
                        StopAllRequests(context->docid);
                     }
                    else 
                     { /* other error codes */
                        DAVDisplayMessage (TtaGetMessage (AMAYA, AM_PROPFIND_FAILED),NULL);
                     }
                    
                    break;

                        
                default:
                    if (davctx->status != HT_MULTI_STATUS) 
                     {
                        printf ("Request %s Completed. Status %d\n", 
                                 HTMethod_name (context->method), davctx->status);
                     }
                    else 
                     {
                        printf ("Request %s - Multi-Status\n",HTMethod_name (context->method));
                     }
                                        
             } /* switch (context->method) */
            
            if (status_msg) 
             {
                TtaSetStatus (context->docid, DAV_VIEW, status_msg, NULL);
             }
         } /* if (davctx) */
     } /* if (context) */


    /* updates Lock indicator*/ 
    DAVSetLockIndicator (context->docid); 
}


/*----------------------------------------------------------------------
   DAVShowPropfindInfo: shows the PROPFIND allprop request's results for 
   the user.
  ---------------------------------------------------------------------- */
void DAVShowPropfindInfo (AHTReqContext *context) 
{
    AwList *list = NULL;
    AwNode *node = NULL;
    AwString pattern = NULL;
    char *empty = NULL;

    if (context) 
     {
        AHTDAVContext *davctx = (AHTDAVContext*) context->dav_context;
        
        if (davctx && davctx->tree) 
         {
            pattern = AwString_new(20);
            AwString_set (pattern,"propstat");

            /* find propstat node */
            node = AwTree_search (AwTree_getRoot (davctx->tree),pattern);
            
            /* find prop node */
            if (node) 
             {
                AwNode_resetChildren (node);
                node = AwNode_nextChild (node);
             }

            /* if there is prop element in the tree, we have properties
             * to show */
            if (node) 
             {
                list = GetPropfindInfoFromNode (node);
             }
            
            if (!list) 
             {
                empty = StrAllocCopy (empty," ");
                list = AwList_new (2);
                AwList_put (list, empty);
                StrAllocCopy (empty, TtaGetMessage (AMAYA, AM_PROPFIND_FAILED)); 
                AwList_put (list, empty);
             }
                            
            DAVPropertiesVerticalDialog (context->docid, TtaGetMessage (AMAYA, AM_DAV_PROPERTIES),
                                         context->urlName, "Document URL: ", 
                                         list);
         }        
     }
}


/*----------------------------------------------------------------------
   GetPropfindInfoFromNode: get the PROPFIND allprop request's results 
   from the node 'prop' of the propfind response tree.  
  ---------------------------------------------------------------------- */
AwList * GetPropfindInfoFromNode (AwNode *propnode) 
{
    AwList *list = NULL;
    AwNode *child = NULL;
    AwNode *href = NULL;    
    AwNode *Nactivelock = NULL;
    AwString info;
    char *name, *value;
    char *Nns = NULL;
    
    if (propnode && AwNode_howManyChildren (propnode)>0) 
     {
        list = AwList_new(5);
        
        AwNode_resetChildren (propnode);
        while ((child = AwNode_nextChild(propnode))!=NULL) 
         {
            AwNode_resetChildren (child);
            info = AwNode_getInfo(child);
            name = AwString_get (info);
            AwString_delete(info);


            /* properties that we don't want, continue */
            if (!HTStrCaseStr(name,"getetag") && !HTStrCaseStr(name,"executable") 
                && !HTStrCaseStr(name,"resourcetype") && !HTStrCaseStr(name,"source")
                && !HTStrCaseStr(name,"supportedlock") && !HTStrCaseStr(name,"status"))
             {

                /* lockdiscovery property? */
                if (HTStrCaseStr(name,"lockdiscovery")) 
                 {
                    Nactivelock = AwNode_nextChild(child);
                    Nns = NULL;
                    
                    if (Nactivelock) 
                     {
                        info = AwString_new (12);
                        AwString_set (info,"activelock");
                        AwList_put (list,AwString_get(info));
                        AwString_set (info,"yes");
                        AwList_put (list,AwString_get(info));
                        AwString_delete (info);

                        AwNode_resetChildren(Nactivelock);

                        while ((child = AwNode_nextChild(Nactivelock))!=NULL) 
                         {
                            AwNode_resetChildren(child);
                            
                            info = AwNode_getInfo(child);
                            Nns = AwString_get (info);
                            AwString_delete(info);

                            
                            /* ignore locktoken */
                            if (HTStrCaseStr (Nns,"locktoken")) 
                               continue;

                            /* if owner, found href child */ 
                            if (HTStrCaseStr (Nns,"owner")) 
                             {
                                href = NULL;    
                                info = AwString_new (5);
                                AwString_set (info,"href");
                                href = AwTree_search (child, info);
                                if (href) child = href;
                                AwNode_resetChildren(child);
                                AwString_delete (info);
                             }

                            /* get the information */
                            child = AwNode_nextChild(child);
                            AwNode_resetChildren(child);

                            if (child) 
                             {
                                info = AwNode_getInfo (child);
                                value = AwString_get (info);
                                AwString_delete(info);

#ifdef DEBUG_DAV                                
                                fprintf (stderr,"GetPropfindInfoFromNode.... adding %s , %s\n",
                                                Nns,value);
#endif
                                AwList_put (list,Nns);
                                AwList_put (list,value);
                             } /* if (child) */
                         } /* while */
                     } /*if Nactivelock*/
                 } /* if lockdiscovery */                
                /* other properties, get the name and the value */
                else 
                 {
                    /* if the node has children, them the information
                     * is in the children */
                    while (child && AwNode_howManyChildren(child)>0 )
                        child = AwNode_nextChild (child);
                    
                    if (child) 
                     {
                        info = AwNode_getInfo (child);
                        value = AwString_get (info);
                        AwString_delete(info);

#ifdef DEBUG_DAV                                
                       fprintf (stderr,"GetPropfindInfoFromNode.... adding %s , %s\n",
                                       name,value);
#endif
                        
                        AwList_put (list,name);
                        AwList_put (list,value);
                     }
                 }                
             }

         } /* while */        
     }
    return list;
}


/*----------------------------------------------------------------------
   DAVShowMultiStatusInfo: show some informations (href and           
                           response description) from a 207 Multi-Status
                           response.
  ---------------------------------------------------------------------- */
void DAVShowMultiStatusInfo (AHTReqContext *context) 
{
    AHTDAVContext *davctx = NULL;
    BOOL ok = NO;
    AwList *list = NULL;
    AwNode *node = NULL;
    AwNode *child = NULL;
    AwNode *root = NULL;            
    AwString pattern = NULL;
    char *name, *value, *ptr;
    char tmp[DAV_LINE_MAX];
        
    if (context) 
     {
        davctx = (AHTDAVContext*) context->dav_context;
        ok = NO;
        
        if (davctx && davctx->tree) 
         {
            list = AwList_new(5);
            node = NULL;
            child = NULL;
            root = NULL;            
            pattern = AwString_new(20);
            name = value = NULL;
            
#ifdef DEBUG_DAV            
            fprintf (stderr,"DAVShowMultiStatusInfo....starting\n");
#endif      
            
            root = AwTree_getRoot(davctx->tree);
            AwTree_saveTree (stdout,root);

            AwNode_resetChildren (root);
            
            while ((child = AwNode_nextChild(root))!=NULL)
             {
                /*reseting */
                name = NULL;
                value = NULL;
                
                /* is it a response? */
                AwString_set (pattern,"response");
                if (AwString_str (AwNode_getInfo(child),pattern)>0) 
                 {
                    
                    /* lock up its children */
                    AwNode_resetChildren (child);       
                    while ((node = AwNode_nextChild(child))!=NULL) 
                     {
                            
                        /* is it a href? take the firs reference */
                        AwString_set (pattern,"href");
                        if (!name && AwString_str (AwNode_getInfo(node),pattern)>0) 
                         {
                            AwNode_resetChildren (node);
                            node = AwNode_nextChild (node);         
                            name = AwNode_getInfo (node);
                            continue;
                         }/* if href */

                        /* is it a responsedescription? */
                        AwString_set (pattern,"responsedescription");
                        if (AwString_str (AwNode_getInfo(node),pattern)>0) 
                         {
                            ptr = value;
                            AwNode_resetChildren (node);
                            node = AwNode_nextChild (node);         
                            value = AwNode_getInfo (node);
                            
                            /* ignore old value */                          
                            if (value && ptr) HT_FREE (ptr);
                                    
                            continue;
                         }/* if responsedescription */

                        
                        /* lock for a status element 
                         * if we haven't a value yet */
                        AwString_set (pattern,"status");
                        node = AwTree_search (node,pattern);
                        if (!value && node) 
                         {
                            AwNode_resetChildren (node);
                            node = AwNode_nextChild (node);         
                            value = AwNode_getInfo (node);
                            if (value) 
                             {
                                /* ignore HTTP/1.1 XXX */
                                if (strlen(value)>strlen ("HTTP/1.1 XXX")) 
                                 {
                                    sprintf (tmp,"%s", &value[strlen ("HTTP/1.1 XXX")+1]);
                                    HT_FREE (value);               
                                    value = NULL;
                                    StrAllocCopy (value,tmp);
                                 }
                             }
                            continue;
                         }
                        
                     } /* while node */
                    
                    /* put in the list */                   
                    if (name && value) 
                     {
#ifdef DEBUG_DAV                            
                        fprintf (stderr,"DAVShowMultiStatusInfo.... adding %s %s\n",name,value);
#endif                  
                        AwList_put (list,name);
                        AwList_put (list,value);
                        ok = YES;
                     }
                 } /* if response */
             } /*while child */

            if (ok) 
             {
                char tmp[DAV_LINE_MAX];
                sprintf (tmp,TtaGetMessage (AMAYA, AM_MULTI_STATUS_FAIL)," ");
                DAVPropertiesVerticalDialog (context->docid, " ", "  ", tmp, list);
             }
         } /* davctx && tree */

        
        if (ok != YES) 
         {
            if (context->method == METHOD_LOCK) 
                 DAVDisplayMessage (TtaGetMessage (AMAYA, AM_LOCK_FAILED), NULL);
            else if (context->method == METHOD_UNLOCK) 
                 DAVDisplayMessage (TtaGetMessage (AMAYA, AM_UNLOCK_FAILED), NULL);
            else 
             {
                 DAVDisplayMessage (TtaGetMessage (AMAYA, AM_MULTI_STATUS_FAIL), 
                                    "Multi-Status failure");
             }
         }
        
     }
}




/*----------------------------------------------------------------------
  DAVSetPreferences: update the DAV preferences based on the 
                     preference dialog options.         
  ---------------------------------------------------------------------- */
void DAVSetPreferences (void)
{
#ifndef _WINDOWS

    /* ****** User reference -  DAVUserURL ***** */
    /* if there's something in textUserReference, analise it*/    
    if (verifyString(textUserReference)>0) 
     {
        /*remove first and last space */     
        CleanFirstLastSpace (textUserReference);
        /* copy the info */
        strcpy (DAVUserURL,textUserReference);
     }

    /* ***** User's resource list - DAVResources  ***** */
    /* it can be empty, so we don't verify it */
    strcpy (DAVResources, textUserResources);

    /* ***** Lock depth -  DAVDepth **** */
    /* we control the radioDepth content in DAVShowPreferencesDlg_callback */
    strcpy (DAVDepth,radioDepth);

    /* ***** Lock scope -  DAVLockScope **** */
    /* we control the radioLockScope content in DAVShowPreferencesDlg_callback */
    strcpy (DAVLockScope,radioLockScope);

    
    /* ***** Lock timout -  DAVTimeout **** */
    if (!strcmp(radioTimeout,"Infinite"))
       strcpy (DAVTimeout,radioTimeout);
    else
     {
       if (numberTimeout<300) numberTimeout = 300;
       sprintf (DAVTimeout,"%s%d",radioTimeout,numberTimeout);
     }
     

    /* ***** Awareness values - DAVAwareness, DAVAwarenessExit  **** */
    DAVAwareness = (toggleAwareness1)?YES:NO;
    DAVAwarenessExit= (toggleAwareness2)?YES:NO;
    

    /* ***** save registry ***** */
    DAVSaveRegistry ();
#endif /* _WINDOWS */    
}
  


/*----------------------------------------------------------------------
  DAVGetPreferences: set the DAV preferences dialog with the 
                     active values.                     
  ---------------------------------------------------------------------- */
void DAVGetPreferences (void)
{
#ifndef _WINDOWS
   char *ptr = NULL;

   /* user reference */
   strcpy (textUserReference, DAVUserURL);
   TtaSetTextForm (DAVPreferencesBase + DAVtextUserReference,textUserReference);

   /* user resource list */
   strcpy (textUserResources, DAVResources);
   TtaSetTextForm (DAVPreferencesBase + DAVtextUserResources,textUserResources);

   /* lock depth */
   strcpy (radioDepth,DAVDepth);
   if (!strcmp(radioDepth,"infinity")) 
       TtaSetMenuForm (DAVPreferencesBase + DAVradioDepth,1);
   else 
       TtaSetMenuForm (DAVPreferencesBase + DAVradioDepth,0);

   /* lock scope */
   strcpy (radioLockScope, DAVLockScope);
   if (!strcmp(radioLockScope,"shared")) 
       TtaSetMenuForm (DAVPreferencesBase + DAVradioLockScope,1);
   else 
       TtaSetMenuForm (DAVPreferencesBase + DAVradioLockScope,0);


   /* lock timeout 
    * timeout format: Infinite or Second-XXXX
    */
   ptr = strstr (DAVTimeout,"Second-");
   if (!strcmp(DAVTimeout,"Infinite") || ptr==NULL || (ptr!=NULL && ptr!=DAVTimeout)) 
    {
       TtaSetMenuForm (DAVPreferencesBase + DAVradioTimeout,0);
       strcpy (radioTimeout,"Infinite");
    }
   else 
    {
       strcpy (radioTimeout,"Second-");
       TtaSetMenuForm (DAVPreferencesBase + DAVradioTimeout,1);
       
       ptr = strchr (DAVTimeout,'-');
       if (ptr)        
           numberTimeout = atoi (++ptr);
       else 
           numberTimeout = 300; /* 300s = 5min */

       TtaSetNumberForm (DAVPreferencesBase + DAVnumberTimeout, numberTimeout);
    }


   /* awareness toggle */
   toggleAwareness1 = (DAVAwareness)?TRUE:FALSE;
   toggleAwareness2 = (DAVAwarenessExit)?TRUE:FALSE;
   TtaSetToggleMenu (DAVPreferencesBase + DAVtoggleAwareness,0,toggleAwareness1);
   TtaSetToggleMenu (DAVPreferencesBase + DAVtoggleAwareness,1,toggleAwareness2);
   
#endif /* _WINDOWS */    

}




/*----------------------------------------------------------------------
  DAVShowPreferencesDlg_callback : callback for the DAV preferences dialog
  ---------------------------------------------------------------------- */
void DAVShowPreferencesDlg_callback (int ref, int typedata, char *data)
{
    
#ifndef _WINDOWS
    if (ref == -1)
     {
      /* removes the network conf menu */
      TtaDestroyDialogue (DAVPreferencesBase + DAVPreferencesDlg);
     }
    else 
     {       
       switch (ref - DAVPreferencesBase)
        {
           case DAVPreferencesDlg:
                   switch ((int)data) 
                    {
                       case 0:     
                              TtaDestroyDialogue (DAVPreferencesBase + DAVPreferencesDlg);
                              break;
                       case 1:
                              DAVSetPreferences();
                              TtaDestroyDialogue (DAVPreferencesBase + DAVPreferencesDlg);
                              break;

                       case 2:
                              /*reset to old values */
                              DAVGetPreferences();
                              break;
       
                    }
                   break;
                        
           case DAVtextUserReference :
                   strcpy (textUserReference, data);
                   break;

           case DAVtextUserResources :
                   strcpy (textUserResources, data);
                   break;

           case DAVradioDepth :
                   switch ((int)data) 
                    {
                       case 0:
                               strcpy (radioDepth,"0");
                               break;
                       case 1:
                               strcpy (radioDepth,"infinity");
                               break;
                    }
                   break;
                   
           case DAVradioTimeout :
                   switch ((int)data) 
                    {
                       case 0:
                               strcpy (radioTimeout,"Infinite");
                               break;
                       case 1:
                               strcpy (radioTimeout,"Second-");
                               break;
                    }
                   break;
                   
           case DAVnumberTimeout :
                   numberTimeout = (int)data;
                   break;                   
                   
           case DAVradioLockScope :
                   switch ((int)data) 
                    {
                       case 0:
                               strcpy (radioLockScope,"exclusive");
                               break;
                       case 1:
                               strcpy (radioLockScope,"shared");
                               break;
                    }
                   break;
                   
           case DAVtoggleAwareness :
                   switch ((int)data) 
                    {
                       case 0:
                               toggleAwareness1 = !toggleAwareness1;
                               break;
                       case 1:
                               toggleAwareness2 = !toggleAwareness2;
                               break;
                    }
                   break;
        }
     }
       
#endif /* _WINDOWS */
        
}




/*----------------------------------------------------------------------
  DAVShowPreferencesDlg: shows the DAV preferences dialog
  ---------------------------------------------------------------------- */
void DAVShowPreferencesDlg (Document document)
{
#ifndef _WINDOWS
        
    char buf[DAV_LINE_MAX];
    
    DAVPreferencesBase = TtaSetCallback (DAVShowPreferencesDlg_callback,MAX_DAVPREF_DLG);

    sprintf (buf,"%s%c%s%c",TtaGetMessage (AMAYA, AM_APPLY_BUTTON),EOS,
                            TtaGetMessage (AMAYA, AM_DEFAULT_BUTTON),EOS);


    TtaNewSheet (DAVPreferencesBase + DAVPreferencesDlg, 
		TtaGetViewFrame (document, DAV_VIEW),
		TtaGetMessage (AMAYA, AM_DAV_PREFERENCES),
		2, buf, TRUE, 3, 'L', D_DONE);
    
 
    /* first line */
    TtaNewTextForm (DAVPreferencesBase + DAVtextUserReference,
		   DAVPreferencesBase + DAVPreferencesDlg,
		   TtaGetMessage (AMAYA, AM_DAV_USER_URL),
		   40, 1, FALSE);
    TtaNewLabel (DAVPreferencesBase + DAVlabelEmpty1,DAVPreferencesBase + DAVPreferencesDlg, " ");
    TtaNewLabel (DAVPreferencesBase + DAVlabelEmpty2,DAVPreferencesBase + DAVPreferencesDlg, " ");


    /* second line */
    sprintf (buf, "B0%cB%s%c", EOS,TtaGetMessage (AMAYA, AM_DAV_DEPTH_INFINITY), EOS);
    TtaNewSubmenu (DAVPreferencesBase + DAVradioDepth,
		  DAVPreferencesBase + DAVPreferencesDlg,
		  0, TtaGetMessage (AMAYA, AM_DAV_DEPTH), 2, buf, NULL, FALSE);               

    sprintf (buf, "B%s%cB%s%c",TtaGetMessage (AMAYA, AM_DAV_TIMEOUT_INFINITE),EOS,
                               TtaGetMessage (AMAYA, AM_DAV_TIMEOUT_OTHER), EOS);
    TtaNewSubmenu (DAVPreferencesBase + DAVradioTimeout,
		  DAVPreferencesBase + DAVPreferencesDlg,
		  0,TtaGetMessage (AMAYA, AM_DAV_TIMEOUT), 2, buf, NULL, FALSE);               

    TtaNewNumberForm (DAVPreferencesBase + DAVnumberTimeout,
		      DAVPreferencesBase + DAVPreferencesDlg,
                      TtaGetMessage (AMAYA, AM_DAV_TIMEOUT_SECONDS), 300,9999, FALSE);  
    
            
    /* third line */
    sprintf (buf, "B%s%cB%s%c",TtaGetMessage (AMAYA, AM_DAV_LOCKSCOPE_EXCLUSIVE), EOS, 
                               TtaGetMessage (AMAYA, AM_DAV_LOCKSCOPE_SHARED), EOS);
    TtaNewSubmenu (DAVPreferencesBase + DAVradioLockScope,
		   DAVPreferencesBase + DAVPreferencesDlg,
		   0, TtaGetMessage (AMAYA, AM_DAV_LOCKSCOPE), 2, buf, NULL, FALSE);               

    TtaNewLabel (DAVPreferencesBase + DAVlabelEmpty3,DAVPreferencesBase + DAVPreferencesDlg, " ");
    TtaNewLabel (DAVPreferencesBase + DAVlabelEmpty4,DAVPreferencesBase + DAVPreferencesDlg, " ");


    /* fourth line */
    sprintf (buf, "B%s%cB%s%c", TtaGetMessage (AMAYA, AM_DAV_AWARENESS_GENERAL), EOS, 
                               TtaGetMessage (AMAYA, AM_DAV_AWARENESS_ONEXIT), EOS);
    TtaNewToggleMenu (DAVPreferencesBase + DAVtoggleAwareness,
		      DAVPreferencesBase + DAVPreferencesDlg,
		      TtaGetMessage (AMAYA, AM_DAV_AWARENESS), 2, buf, NULL, TRUE);
    
    TtaNewLabel (DAVPreferencesBase + DAVlabelEmpty5,DAVPreferencesBase + DAVPreferencesDlg, "    ");
    TtaNewLabel (DAVPreferencesBase + DAVlabelEmpty6,DAVPreferencesBase + DAVPreferencesDlg, "    ");
    

    /* fifth line */
    TtaNewTextForm (DAVPreferencesBase + DAVtextUserResources,
		   DAVPreferencesBase + DAVPreferencesDlg,
		   TtaGetMessage (AMAYA, AM_DAV_USER_RESOURCES),
		   40, 1, FALSE);


    /* get the active values and set the dialogue variables */
    DAVGetPreferences();
    
    /* show the dialogue */
    TtaSetDialoguePosition ();
    TtaShowDialogue (DAVPreferencesBase + DAVPreferencesDlg, TRUE);

#endif /* !_WINDOWS */
        
}




