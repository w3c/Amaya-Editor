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
#include "davlibUI_f.h"
#include "davlibCommon_f.h"
#include "davlibRequests_f.h"

#include "init_f.h"
#include "query_f.h"



/* ********************************************************************* *
 *                       PRESENTATION FUNCTIONS                          *
 * ********************************************************************* */


/*----------------------------------------------------------------------
  DAVSetLockIndicator: set the Lock indicator button.
  ---------------------------------------------------------------------- */
void DAVSetLockIndicator (Document docid) {
   /* updates Lock indicator*/ 
   TtaSetToggleItem (docid,DAV_VIEW,Cooperation_,\
                          BLockIndicator,DAVLockIndicatorState);  
}




/*----------------------------------------------------------------------
  DAVDisplayMessage: display a message to user.
  ---------------------------------------------------------------------- */
void DAVDisplayMessage (char *msg, char *arg) {
    if (msg && *msg) {
        if (arg && *arg) {
            char label[LINE_MAX];
            sprintf (label,msg,arg);
            InitInfo (" ",label);
        }
        else {
            InitInfo (" ",msg);
        }
    }
}



/*----------------------------------------------------------------------
  DAVConfirmDialog: display a "Confirm/Cancel" dialog to user.
  ---------------------------------------------------------------------- */
BOOL DAVConfirmDialog (Document docid, char *msg1, char *msg2, char *msg3) {

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
void DAVPropertiesVerticalDialog (Document docid, char *title, char *rheader, \
                       char *lheader, AwList *list) {
        
    int i=MAX_REF+1, form=MAX_REF, lines=0;        
    char *name, *value;
    
    if (docid>0 && list) {
        lines = AwList_size(list)/2;
             
#ifndef _WINDOWS           
        /* Main form */
        TtaNewSheet (BaseDialog + form, \
                TtaGetViewFrame (docid, DAV_VIEW), \
                (title)?title:" ", 0, NULL, FALSE, \
                (rheader && lheader)?lines+3:lines+1, 'L', D_DONE);

        if (rheader || lheader) {
            TtaNewLabel (BaseDialog + (i++), BaseDialog + form,\
                              (lheader)?lheader:" ");
            TtaNewLabel (BaseDialog + (i++), BaseDialog + form,\
                             "----------------");
        }

            /* properties: names */ 
        while ( list && (name=(char *)AwList_next(list)) \
                         && (value=(char *)AwList_next(list)) ) {
           char label[LINE_MAX];
           char *ns = NULL;
               
           /* ignore 'namespace:' in property name */
           if ((ns = strchr (name,':'))) 
               sprintf (label,"%s : ",(++ns));
           else
               sprintf (label,"%s : ",name);

           /* property name */
           TtaNewLabel (BaseDialog + (i++), BaseDialog + form, label);
        }                              
            
        AwList_reset (list);
            
        TtaNewLabel (BaseDialog + (i++), BaseDialog + form,\
                             "              ");

        if (rheader || lheader) {
            TtaNewLabel (BaseDialog + (i++), BaseDialog + form,\
                             (rheader)?rheader:" ");
            TtaNewLabel (BaseDialog + (i++), BaseDialog + form,\
                             "------------------------------");
        }
            
        /* properties: values */ 
        while ( list && (name=(char *)AwList_next(list)) \
                         && (value=(char *)AwList_next(list)) ) {
           char label[LINE_MAX];
           char *ns = NULL; 
              
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
                
        TtaNewLabel (BaseDialog + (i++), BaseDialog + form,\
                             "              ");

            
        TtaSetDialoguePosition ();
        TtaShowDialogue (BaseDialog + form, TRUE);            
#endif
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
void DAVHorizontalDialog (Document docid, char *title, char *rheader, \
                          char *lheader, AwList *list) {
        
    int i=MAX_REF+1, form=MAX_REF;
    char *name, *value;
    
    if (docid>0 && list) {
             
#ifndef _WINDOWS           
        /* Main form */
        TtaNewSheet (BaseDialog + form, \
                TtaGetViewFrame (docid, DAV_VIEW), \
                (title)?title:" ", 0, NULL, TRUE, \
                 2, 'L', D_DONE);

        if (rheader || lheader) {
            TtaNewLabel (BaseDialog + (i++), BaseDialog + form,\
                              (lheader)?lheader:" ");
            TtaNewLabel (BaseDialog + (i++), BaseDialog + form,\
                              (rheader)?rheader:" ");
            TtaNewLabel (BaseDialog + (i++), BaseDialog + form,\
                             "----------------");
            TtaNewLabel (BaseDialog + (i++), BaseDialog + form,\
                             "---------------------------");
        }

        /* names  values */ 
        while ( list && (name=(char *)AwList_next(list)) \
                         && (value=(char *)AwList_next(list)) ) {
           char label[LINE_MAX];
               
           /* name  */
           sprintf (label,"%s ",name);
           TtaNewLabel (BaseDialog + (i++), BaseDialog + form, label);
                                     
           sprintf (label, "%s ",value);
           TtaNewLabel (BaseDialog + (i++), BaseDialog + form, label);
           
           HT_FREE (name);
           HT_FREE (value);               
        }

        AwList_delete (list);
                
        TtaNewLabel (BaseDialog + (i++), BaseDialog + form,\
                             "              ");
        TtaNewLabel (BaseDialog + (i++), BaseDialog + form,\
                             "              ");

        TtaSetDialoguePosition ();
        TtaShowDialogue (BaseDialog + form, TRUE);            
#endif
     }
}




/*----------------------------------------------------------------------
   DAVShowInfo: shows the request's results for the user.
  ---------------------------------------------------------------------- */
void DAVShowInfo (AHTReqContext *context) {

#ifdef DEBUG_DAV        
    printf ("****** DAVShowInfo ****** \n");
#endif

    if (context) {           
        AHTDAVContext *davctx = (AHTDAVContext*) context->dav_context;
        char *status_msg = NULL;
        
        if (davctx) {  /* it's a WebDAV request */
            
            switch (context->method) {
                    
                /* **** LOCK requests **** */
                case METHOD_LOCK:      
                    /* Normal results */
                    if (davctx->status > 0 && davctx->status != HT_MULTI_STATUS) {
                            
                        /* lock succeed */                                               
                        DAVDisplayMessage (TtaGetMessage (AMAYA, AM_LOCK_SUCCEED),NULL);
                        
                        /*set the status line */
                        status_msg = TtaGetMessage (AMAYA, AM_LOCK_SUCCEED);

                    }
                    /* 207 Multi-Status - Error! */
                    else if (davctx->status == HT_MULTI_STATUS) {
                        DAVShowMultiStatusInfo (context);
                        /*set the status line */
                        status_msg = TtaGetMessage (AMAYA, AM_LOCK_FAILED);

                    }
                    /*405 Method not allowed*/
                    else if (davctx->status == DAV_METHOD_NOT_ALLOWED) {
                        TtaDisplayMessage (CONFIRM, \
                                           TtaGetMessage (AMAYA, AM_DAV_NOT_ALLOWED),\
                                           context->urlName);
                    }
                    /* -400 Bad Request */
                    else if (davctx->status == DAV_BAD_REQUEST) {
                        /* It may happens when user interrupts the request,
                         * and the XML body is not sent to the server */
#ifdef DEBUG_DAV                                
                        fprintf (stderr,"BAD REQUEST - XML body not sent\n");
#endif 
                        DAVDisplayMessage (TtaGetMessage (AMAYA, AM_LOCK_FAILED),NULL);
                        status_msg = TtaGetMessage (AMAYA, AM_LOCK_FAILED);
                    }                                        
                    else {  /* other error codes */    
                        DAVDisplayMessage (TtaGetMessage (AMAYA, AM_LOCK_FAILED),NULL);
                        
                        /* set the status line */
                        status_msg = TtaGetMessage (AMAYA, AM_LOCK_FAILED);
                    }
                    
                    break;
                   
                    
                /* **** UNLOCK requests **** */
                case METHOD_UNLOCK:
                    /* Normal results */
                    if (davctx->status > 0 && davctx->status != HT_MULTI_STATUS) {
                        DAVDisplayMessage (TtaGetMessage (AMAYA, AM_UNLOCK_SUCCEED), NULL);
                        
                        /*set the status line */
                        status_msg = TtaGetMessage (AMAYA, AM_UNLOCK_SUCCEED);
                        
                    }
                    /*405 Method not allowed*/
                    else if (davctx->status == DAV_METHOD_NOT_ALLOWED) {
                        DAVDisplayMessage (TtaGetMessage (AMAYA, AM_DAV_NOT_ALLOWED),\
                                           context->urlName);
                    }
                    /*-400 Bad Request */
                    else if (davctx->status == DAV_BAD_REQUEST) {
                        if (DAVConfirmDialog (context->docid,TtaGetMessage (AMAYA, AM_UNLOCK_FAILED),\
                                              TtaGetMessage (AMAYA, AM_DAV_TRY_AGAIN), " "))
                            /*DAVRedo (context);*/
                            context->dav_context = NULL;
                        else {
                            /*set the status line */
                            status_msg = TtaGetMessage (AMAYA, AM_UNLOCK_FAILED);
                        }
                    }
                    else { /* other error codes */    
                        DAVDisplayMessage (TtaGetMessage (AMAYA, AM_UNLOCK_FAILED), NULL);
                        
                        /*set the status line */
                        status_msg = TtaGetMessage (AMAYA, AM_UNLOCK_FAILED);
                    }
                    
                    break;

                    
               /* **** PROPFIND requests **** */
                case METHOD_PROPFIND:
                    /* 207 Multi-Status - normal case */
                    if (davctx->status == HT_MULTI_STATUS) {
                        DAVShowPropfindInfo(context);    
                    }
                    /*405 Method not allowed*/
                    else if (davctx->status == DAV_METHOD_NOT_ALLOWED) {
                        DAVDisplayMessage (TtaGetMessage (AMAYA, AM_DAV_NOT_ALLOWED),\
                                           context->urlName);
                    }
                    /* -400 Bad Request */
                    else if (davctx->status == DAV_BAD_REQUEST) {
                        /* It may happens when user interrupts the request,
                         * and the XML body is not sent to the server */
#ifdef DEBUG_DAV                                
                        fprintf (stderr,"BAD REQUEST - XML body not sent\n");
#endif 
                        
                        DAVDisplayMessage (TtaGetMessage (AMAYA, AM_PROPFIND_FAILED_TEMP),NULL);
                        StopAllRequests(context->docid);
                    }
                    else { /* other error codes */
                        DAVDisplayMessage (TtaGetMessage (AMAYA, AM_PROPFIND_FAILED),NULL);
                    }
                    
                    break;

                        
                default:
                    if (davctx->status != HT_MULTI_STATUS) { 
                        printf ("Request %s Completed. Status %d\n", \
                                    HTMethod_name (context->method), davctx->status);
                    }
                    else {
                        printf ("Request %s - Multi-Status\n",HTMethod_name (context->method));
                    }

                                        
            }         /* switch (context->method) */
            
            if (status_msg) {
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
void DAVShowPropfindInfo (AHTReqContext *context) {

    if (context) {           
        AHTDAVContext *davctx = (AHTDAVContext*) context->dav_context;
        
        if (davctx && davctx->tree) {
            AwList *list = NULL;
            AwNode *node = NULL;
            AwString pattern = AwString_new(20);
            AwString_set (pattern,"propstat");

            /* find propstat node */
            node = AwTree_search (AwTree_getRoot(davctx->tree),pattern);
            
            /* find prop node */
            if (node) {
                AwNode_resetChildren (node);
                node = AwNode_nextChild (node);
            }

            /* if there is prop element in the tree, we have properties
             * to show */
            if (node) {
                list = GetPropfindInfoFromNode (node);
            }
            
            if (!list) {
                char *empty = StrAllocCopy (empty," ");
                list = AwList_new (2);
                AwList_put (list, empty);
                StrAllocCopy (empty, TtaGetMessage (AMAYA, AM_PROPFIND_FAILED)); 
                AwList_put (list, empty);
            }
                            
            DAVPropertiesVerticalDialog (context->docid, \
                                TtaGetMessage (AMAYA, AM_DAV_PROPERTIES),\
                                context->urlName, "Document URL: ", \
                                list);
        }        
    }
}


/*----------------------------------------------------------------------
   GetPropfindInfoFromNode: get the PROPFIND allprop request's results 
   from the node 'prop' of the propfind response tree.  
  ---------------------------------------------------------------------- */
AwList * GetPropfindInfoFromNode (AwNode *propnode) {
    AwList *list = NULL;
    
    if (propnode && AwNode_howManyChildren (propnode)>0) {
        AwNode *child;
        AwString info;
        char *name, *value;
        list = AwList_new(5);
        
        AwNode_resetChildren (propnode);
        while ((child = AwNode_nextChild(propnode))!=NULL) {
            AwNode_resetChildren (child);
            info = AwNode_getInfo(child);
            name = AwString_get (info);
            AwString_delete(info);


            /* properties that we don't want, continue */
            if (!HTStrCaseStr(name,"getetag") && !HTStrCaseStr(name,"executable") \
                && !HTStrCaseStr(name,"resourcetype") && !HTStrCaseStr(name,"source")\
                && !HTStrCaseStr(name,"supportedlock") && !HTStrCaseStr(name,"status")) {

                /* lockdiscovery property? */
                if (HTStrCaseStr(name,"lockdiscovery")) {
                    AwNode *Nactivelock = AwNode_nextChild(child);
                    char *Nns = NULL;
                    
                    if (Nactivelock) {
                        info = AwString_new (12);
                        AwString_set (info,"activelock");
                        AwList_put (list,AwString_get(info));
                        AwString_set (info,"yes");
                        AwList_put (list,AwString_get(info));
                        AwString_delete (info);

                        AwNode_resetChildren(Nactivelock);

                        while ((child = AwNode_nextChild(Nactivelock))!=NULL) {                        
                            AwNode_resetChildren(child);
                            
                            info = AwNode_getInfo(child);
                            Nns = AwString_get (info);
                            AwString_delete(info);

                            
                            /* ignore locktoken */
                            if (HTStrCaseStr (Nns,"locktoken")) 
                               continue;

                            /* if owner, found href child */ 
                            if (HTStrCaseStr (Nns,"owner")) {
                                AwNode *href = NULL;    
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

                            if (child) {
                                info = AwNode_getInfo (child);
                                value = AwString_get (info);
                                AwString_delete(info);

#ifdef DEBUG_DAV                                
                                fprintf (stderr,"GetPropfindInfoFromNode.... adding %s , %s\n",\
                                                Nns,value);
#endif
                                AwList_put (list,Nns);
                                AwList_put (list,value);
                            }
                        }
                    } /*if Nactivelock*/
                }
                /* other properties, get the name and the value */
                else {
                    /* if the node has children, them the information
                     * is in the children */
                    while (child && AwNode_howManyChildren(child)>0 )
                        child = AwNode_nextChild (child);
                    
                    if (child) {
                        info = AwNode_getInfo (child);
                        value = AwString_get (info);
                        AwString_delete(info);

#ifdef DEBUG_DAV                                
                       fprintf (stderr,"GetPropfindInfoFromNode.... adding %s , %s\n",\
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
void DAVShowMultiStatusInfo (AHTReqContext *context) {
        
    if (context) {
        AHTDAVContext *davctx = (AHTDAVContext*) context->dav_context;
        BOOL ok = NO;
        
        if (davctx && davctx->tree) {
            AwList *list = AwList_new(5);
            AwNode *node = NULL;
            AwNode *child = NULL;
            AwNode *root = NULL;            
            AwString pattern = AwString_new(20);
            char *name, *value;
            
#ifdef DEBUG_DAV            
            fprintf (stderr,"DAVShowMultiStatusInfo....starting\n");
#endif      
            
            root = AwTree_getRoot(davctx->tree);
            AwTree_saveTree (stdout,root);

            AwNode_resetChildren (root);
            
            while ((child = AwNode_nextChild(root))!=NULL) {
                /*reseting */
                name = NULL;
                value = NULL;
                
                /* is it a response? */
                AwString_set (pattern,"response");
                if (AwString_str (AwNode_getInfo(child),pattern)>0) {
                    
                    /* lock up its children */
                    AwNode_resetChildren (child);       
                    while ((node = AwNode_nextChild(child))!=NULL) {
                            
                        /* is it a href? take the firs reference */
                        AwString_set (pattern,"href");
                        if (!name && AwString_str (AwNode_getInfo(node),pattern)>0) {
                            AwNode_resetChildren (node);
                            node = AwNode_nextChild (node);         
                            name = AwNode_getInfo (node);
                            continue;
                        }/* if href */

                        /* is it a responsedescription? */
                        AwString_set (pattern,"responsedescription");
                        if (AwString_str (AwNode_getInfo(node),pattern)>0) {
                            char *tmp = value;
                            AwNode_resetChildren (node);
                            node = AwNode_nextChild (node);         
                            value = AwNode_getInfo (node);
                            /* ignore old value */                          
                            if (value && tmp) HT_FREE (tmp);
                                    
                            continue;
                        }/* if responsedescription */

                        
                        /* lock for a status element 
                         * if we haven't a value yet */
                        AwString_set (pattern,"status");
                        node = AwTree_search (node,pattern);
                        if (!value && node) {
                            AwNode_resetChildren (node);
                            node = AwNode_nextChild (node);         
                            value = AwNode_getInfo (node);
                            if (value) {
                                /* ignore HTTP/1.1 XXX */
                                if (strlen(value)>strlen ("HTTP/1.1 XXX")) {
                                    char tmp[LINE_MAX];
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
                    if (name && value) {
#ifdef DEBUG_DAV                            
                        fprintf (stderr,"DAVShowMultiStatusInfo.... adding %s %s\n",name,value);
#endif                  
                        AwList_put (list,name);
                        AwList_put (list,value);
                        ok = YES;
                    }
                } /* if response */
            } /*while child */

            if (ok) {
                char tmp[LINE_MAX];
                sprintf (tmp,TtaGetMessage (AMAYA, AM_MULTI_STATUS_FAIL)," ");
                DAVPropertiesVerticalDialog (context->docid, " ", \
                                             "  ", tmp, list);
            }
        } /* davctx && tree */

        
        if (ok != YES) {
            if (context->method == METHOD_LOCK) 
                 DAVDisplayMessage (TtaGetMessage (AMAYA, AM_LOCK_FAILED), NULL);
            else if (context->method == METHOD_UNLOCK) 
                 DAVDisplayMessage (TtaGetMessage (AMAYA, AM_UNLOCK_FAILED), NULL);
            else {
                 DAVDisplayMessage (TtaGetMessage (AMAYA, AM_MULTI_STATUS_FAIL), \
                                                   "Multi-Status failure");
            }
        }
        
    }
}
