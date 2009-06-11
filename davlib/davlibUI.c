/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

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
** Revision 1.34  2009-06-11 15:12:32  vatton
** Keep the value of the old DAV_URLS environment variable
** + update dialogs
** Irene
**
** Revision 1.33  2009/06/10 10:57:23  vatton
** Change the management of Templates list
** + Implementation of a new WebDAV list of sites
** + Fix problems with lock/unlock status
** Irene
**
** Revision 1.32  2009/06/08 14:57:00  vatton
** Addd a new button to lock/unlock WebDAV resources
** + display only the end of the message when the status bar is too short
** Irene
**
** Revision 1.31  2009/04/23 14:51:36  vatton
** Improving the WebDAV interface
** Irene
**
** Revision 1.30  2009/02/03 15:26:19  carcone
** Change the prototype of the function 'TtaShowDialogue'
** to avoid to move a dialogue in some cases.
** Laurent
**
** Revision 1.29  2008/05/13 09:30:27  kia
** More char* fixes
**
** Revision 1.28  2008/05/07 13:49:07  kia
** char* => const char*
** (Regis patches + many many others)
**
** Revision 1.27  2006/04/06 08:38:30  vatton
** Improve the creation of in-line elements.
** Irene
**
** Revision 1.26  2006/01/06 14:01:52  vatton
** Prevent compiling troubles.
**
** Irene
**
** Revision 1.25  2005/08/31 07:30:34  vatton
** Fixing a warning.
** Irene
**
** Revision 1.24  2005/08/18 10:26:00  cvs
** Replace "intptr_t" by "long int" as "intptr_t" is not accepted by
** Visual C++
**
** Irene
**
** Revision 1.23  2005/08/18 09:35:28  vatton
** Enable by default --with-templates with WX versions
** Display attributes of read-only elements
**
** PS: Don't forget to remove your old Options and amaya/ *.o files.
** Irene
**
** Revision 1.22  2005/08/17 10:28:16  vatton
** Make Help files read-only.
**
** Irene
**
** Revision 1.21  2005/06/07 13:37:11  gully
** code cleaning + warning fixes
** S. GULLY
**
** Revision 1.20  2004/09/30 07:23:42  vatton
** New message info.
** Irene
**
** Revision 1.19  2004/09/23 16:18:44  vatton
** Reorganization of Preferences menus.
** Irene
**
** Revision 1.18  2004/09/22 16:07:08  cvs
** Win32 WebAV preferences.
** Irene
**
** Revision 1.17  2004/09/22 11:55:05  cvs
** Amaya didn't compile when the DAV option is set.
** Irene
**
** Revision 1.16  2004/09/22 09:25:20  cvs
** Work on WebDAV preferences.
** Irene
**
** Revision 1.15  2004/09/21 16:34:32  cvs
** Fixing compiling troubles.
** Irene
**
** Revision 1.14  2004/09/21 16:10:19  vatton
** Clean up MENUconf.c (not tested on Windows platforms).
** Prepare the WebDAV configuration (hidden when DAV is not active).
** Irene
**
** Revision 1.13  2004/08/24 16:09:39  cvs
** compilation fix
** S. GULLY
**
** Revision 1.12  2004/07/21 14:33:02  cvs
** New workspace to compile wxWidgets version.
** S. GULLY
**
** Revision 1.11  2003/12/16 12:10:08  cvs
** Rename flags _WINDOWS by _WINGUI
** + the flag _WINDOWS must be used only for windows specific code.
** + the flag _WINGUI must be used only for old windows gui stuff.
**
** S. GULLY
**
** Revision 1.10  2003/11/19 12:33:16  gully
** Compilation fix (webdav support)
**
** S. GULLY
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
** Revision 1.7  2002/06/12 10:29:07  kirschpi
** - adjusts in code format
** - new commentaries
** - small fixes
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

#define WEBDAV_EXPORT extern
#include "davlib.h"
#include "davlib_f.h"
#include "davlibUI_f.h"
#include "davlibCommon_f.h"
#include "davlibRequests_f.h"
#include "AHTURLTools_f.h"

#include "init_f.h"
#include "query_f.h"
#include "MENUconf.h"
#include "MENUconf_f.h"

/* ********************************************************************* *
 *                         PRIVATE VARIABLES                             *
 * ********************************************************************* */

/* Paths from which looking for templates.*/
Prop_DAV_Path *DAV_Paths = NULL;
extern int      DAVBase;
extern Prop_DAV GProp_DAV;

static AwList * GetPropfindInfoFromNode (AwNode *propnode);

/*----------------------------------------------------------------------
  DAVSetLockIndicator: set the Lock indicator button.
  ---------------------------------------------------------------------- */
void DAVSetLockIndicator (Document doc, int status) 
{
  ThotBool  val = status == 2;
  /* updates Lock indicator*/
  if (DocumentMeta[doc])
    {
      DocumentMeta[doc]->lockState = status;
      TtaSetToggleItem (doc, DAV_VIEW, Tools, TLock, val);
      TtaSetLockButton (doc, status);
    }
}


/*----------------------------------------------------------------------
  DAVDisplayMessage: display a message to user.
  ---------------------------------------------------------------------- */
void DAVDisplayMessage (const char *msg, const char *arg) 
{
  if (msg && *msg) 
    {
      if (arg && *arg) 
        {
          char label[2*MAX_LENGTH];
          sprintf (label, msg, arg);
          InitInfo (" ", label);
        }
      else 
        InitInfo (" ", msg);
    }
}


/*----------------------------------------------------------------------
  DAVDisplayMultiLineMessage: display message with 3 lines.
  ---------------------------------------------------------------------- */
void DAVDisplayMultiLineMessage (Document docid, char *line1, char *line2,
                                 char *line3) 
{
  if (line1 && line2 && line3) 
    InitConfirm3L (docid, DAV_VIEW, line1, line2, line3, FALSE);
}


/*----------------------------------------------------------------------
  DAVConfirmDialog: display a "Confirm/Cancel" dialog to user.
  ---------------------------------------------------------------------- */
BOOL DAVConfirmDialog (Document docid, char *msg1, char *msg2, char *msg3) 
{
  InitConfirm3L (docid, DAV_VIEW, msg1, msg2, msg3, TRUE);
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
static void DAVPropertiesVerticalDialog (Document docid, const char *title,
    const char *rheader, const char *lheader, AwList *list) 
{
  int     lines = 0;

  if (docid > 0 && list) 
    {
      lines = AwList_size(list)/2;
      /* function not implemented yet */
      InitInfo ("", TtaGetMessage(LIB, TMSG_NOT_AVAILABLE));
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
        {
          /* it's a WebDAV request */
          switch (context->method)
            {
              /* **** LOCK requests **** */
            case METHOD_LOCK:      
              /* Normal results */
              if (davctx->status > 0 && davctx->status != HT_MULTI_STATUS) 
                {
                  /* lock succeed */                         
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
                  DAVDisplayMessage (TtaGetMessage (AMAYA,
                                                    AM_PROPFIND_FAILED_TEMP), NULL);
                  StopAllRequests(context->docid);
                }
              else 
                /* other error codes */
                DAVDisplayMessage (TtaGetMessage (AMAYA, AM_PROPFIND_FAILED), NULL);
              break;
	      
            default:
              if (davctx->status != HT_MULTI_STATUS) 
                printf ("Request %s Completed. Status %d\n", 
                        HTMethod_name (context->method), davctx->status);
              else 
                printf ("Request %s - Multi-Status\n", HTMethod_name (context->method));
            } /* switch (context->method) */
	  
          if (status_msg) 
            TtaSetStatus (context->docid, DAV_VIEW, status_msg, NULL);
        } /* if (davctx) */
    } /* if (context) */
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
	  
          DAVPropertiesVerticalDialog (context->docid,
                                       TtaGetMessage (AMAYA, AM_DAV_PROPERTIES),
                                       context->urlName, "Document URL: ", 
                                       list);
        }        
    }
}


/*----------------------------------------------------------------------
  GetPropfindInfoFromNode: get the PROPFIND allprop request's results 
  from the node 'prop' of the propfind response tree.  
  ---------------------------------------------------------------------- */
static AwList * GetPropfindInfoFromNode (AwNode *propnode) 
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
          if (!HTStrCaseStr(name,(char*)"getetag") && !HTStrCaseStr(name,(char*)"executable") 
              && !HTStrCaseStr(name,(char*)"resourcetype") && !HTStrCaseStr(name,(char*)"source")
              && !HTStrCaseStr(name,(char*)"supportedlock") && !HTStrCaseStr(name,(char*)"status"))
            {
              /* lockdiscovery property? */
              if (HTStrCaseStr(name,(char*)"lockdiscovery")) 
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
                      while ((child = AwNode_nextChild(Nactivelock)) != NULL) 
                        {
                          AwNode_resetChildren(child);
                          info = AwNode_getInfo(child);
                          Nns = AwString_get (info);
                          AwString_delete(info);
			  
                          /* ignore locktoken */
                          if (HTStrCaseStr (Nns,(char*)"locktoken")) 
                            continue;
			  
                          /* if owner, found href child */ 
                          if (HTStrCaseStr (Nns,(char*)"owner")) 
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
  char tmp[MAX_LENGTH];
        
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
                          if (value && ptr)
                            HT_FREE (ptr);
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
              char tmp[MAX_LENGTH];
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
            DAVDisplayMessage (TtaGetMessage (AMAYA, AM_MULTI_STATUS_FAIL), 
                               "Multi-Status failure");
        }
    }
}


/*----------------------------------------------------------------------
  SetDAVConf: update the DAV preferences based on the 
  preference dialog options.         
  ---------------------------------------------------------------------- */
void SetDAVConf (void)
{
  /* ****** User reference -  DAVUserURL ***** */
  /* if there's something in textUserReference, analyse it*/    
  if (verifyString (GProp_DAV.textUserReference) > 0) 
    {
      /*remove first and last space */     
      CleanFirstLastSpace (GProp_DAV.textUserReference);
      /* copy the info */
      strcpy (DAVUserURL, GProp_DAV.textUserReference);
    }
  
  /* ***** Lock depth -  DAVDepth **** */
  /* we control the radioDepth content in DAVPreferencesDlg_callback */
  strcpy (DAVDepth, GProp_DAV.radioDepth);
  
  /* ***** Lock scope -  DAVLockScope **** */
  /* we control the radioLockScope content in DAVPreferencesDlg_callback */
  strcpy (DAVLockScope, GProp_DAV.radioLockScope);
  
  /* ***** Lock timout -  DAVTimeout **** */
  if (!strcmp(GProp_DAV.radioTimeout, "Infinite"))
    strcpy (DAVTimeout, GProp_DAV.radioTimeout);
  else
    {
      if (GProp_DAV.numberTimeout < 300) GProp_DAV.numberTimeout = 300;
      sprintf (DAVTimeout, "%s%d", GProp_DAV.radioTimeout, GProp_DAV.numberTimeout);
    }
  /* ***** save registry ***** */
  DAVSaveRegistry ();
}


/*----------------------------------------------------------------------
  GetDAVConf: set the DAV preferences dialog with the 
  active values.                     
  ---------------------------------------------------------------------- */
void GetDAVConf (void)
{
  char *ptr = NULL;

  /* user reference */
  strcpy (GProp_DAV.textUserReference, DAVUserURL);
  /* lock depth */
  strcpy (GProp_DAV.radioDepth, DAVDepth);
  /* lock scope */
  strcpy (GProp_DAV.radioLockScope, DAVLockScope);

  /* lock timeout 
   * timeout format: Infinite or Second-XXXX
   */
  ptr = strstr (DAVTimeout, "Second-");
  if (!strcmp (DAVTimeout, "Infinite") || ptr == NULL ||
      (ptr != NULL && ptr != DAVTimeout))
    {
      strcpy (GProp_DAV.radioTimeout, "Infinite");
      GProp_DAV.numberTimeout = 0; /* */
    }
  else 
    {
      strcpy (GProp_DAV.radioTimeout, "Second-");
      ptr = strchr (DAVTimeout, '-');
      if (ptr)        
        GProp_DAV.numberTimeout = atoi (++ptr);
      else 
        GProp_DAV.numberTimeout = 300; /* 300s = 5min */
    }
}


/*----------------------------------------------------------------------
  DAVPreferencesDlg_callback : callback for the DAV preferences dialog
  ---------------------------------------------------------------------- */
void DAVPreferencesDlg_callback (int ref, int typedata, char *data)
{
  if (ref == -1)
    /* removes the network conf menu */
    TtaDestroyDialogue (DAVBase + DAVPreferencesDlg);
  else 
    {       
      switch (ref - DAVBase)
        {
        case DAVPreferencesDlg:
          switch ((long int)data) 
            {
            case 1:
              SetDAVConf();
            case 0:
              break;
            case 2:
              /*reset to old values */
              GetDAVConf();
              break;
            }
          break;
	  
        case DAVtextUserReference :
          strcpy (GProp_DAV.textUserReference, data);
          break;

        case DAVradioDepth :
          switch ((long int)data) 
            {
            case 0:
              strcpy (GProp_DAV.radioDepth, "0");
              break;
            case 1:
              strcpy (GProp_DAV.radioDepth, "infinity");
              break;
            }
          break;
                   
        case DAVradioTimeout :
          switch ((long int)data) 
            {
            case 0:
              strcpy (GProp_DAV.radioTimeout, "Infinite");
              break;
            case 1:
              strcpy (GProp_DAV.radioTimeout, "Second-");
              break;
            }
          break;
	  
        case DAVnumberTimeout :
          GProp_DAV.numberTimeout = (long int)data;
          break;                   
	  
        case DAVradioLockScope :
          switch ((long int)data) 
            {
            case 0:
              strcpy (GProp_DAV.radioLockScope, "exclusive");
              break;
            case 1:
              strcpy (GProp_DAV.radioLockScope, "shared");
              break;
            }
          break;
        }
    }
}

/*----------------------------------------------------------------------
  AllocDAVPathsListElement: allocates an element for the list of DAV paths.
  path : path of the new element
  return : address of the new element
  ----------------------------------------------------------------------*/
void* AllocDAVPathsListElement (const char* path, void* prevElement)
{
  Prop_DAV_Path *element;

  element  = (Prop_DAV_Path*)TtaGetMemory (sizeof(Prop_DAV_Path));
  memset (element, 0, sizeof(Prop_DAV_Path));
  element->Path = TtaStrdup (path);
  if (prevElement)
    {
      element->NextPath = ((Prop_DAV_Path*)prevElement)->NextPath;
      ((Prop_DAV_Path*)prevElement)->NextPath = element;
    }
  return element;
}


/*----------------------------------------------------------------------
  SaveDAVPathsList: Save the list of DAV paths.
  list   : address of the list (address of the first element).
  ----------------------------------------------------------------------*/
static void SaveDAVPathsList ()
{
  const Prop_DAV_Path *element;
  char                *path, *homePath;
  unsigned char       *c;
  FILE                *file;

  path = (char *) TtaGetMemory (MAX_LENGTH);
  homePath       = TtaGetEnvString ("APP_HOME");
  sprintf (path, "%s%cdav.dat", homePath, DIR_SEP);

  file = TtaWriteOpen ((char *)path);
  c = (unsigned char*)path;
  *c = EOS;
  if (file)
    {
      element = DAV_Paths;
      while (element)
        {
          fprintf(file, "%s\n", element->Path);
          element = element->NextPath;
        }
      TtaWriteClose (file);
    }
}

/*----------------------------------------------------------------------
  AddPathInDAVList: add a path in the list of DAV paths.
  ----------------------------------------------------------------------*/
void AddPathInDAVList (const char *path)
{
  Prop_DAV_Path  *element, *prev;

  if (path == NULL || *path == EOS && IsHTTPPath (path))
    return;
  element = (Prop_DAV_Path*) TtaGetMemory (sizeof(Prop_DAV_Path));
  element->NextPath = NULL;
  element->Path = TtaStrdup (path);
  prev = DAV_Paths;
  if (prev)
    {
      while (prev->NextPath)
        prev = prev->NextPath;
      prev->NextPath = element;
    }
  else
    DAV_Paths = element;
  SaveDAVPathsList ();
}

/*----------------------------------------------------------------------
  RemovePathInDAVList: add a path in the list of DAV paths.
  ----------------------------------------------------------------------*/
void RemovePathInDAVList (const char *path)
{
  Prop_DAV_Path  *element, *prev = NULL;

  if (path == NULL || *path == EOS)
    return;
  element = DAV_Paths;
  while (element)
    {
      if (element->Path && !strcmp (path, element->Path))
        {
          if (prev)
            prev->NextPath = element->NextPath;
          else
            DAV_Paths = element->NextPath;
          TtaFreeMemory (element->Path);
          TtaFreeMemory (element);
          SaveDAVPathsList ();
          return;
        }
      prev = element;
      element = element->NextPath;
    }
  return;
}

/*----------------------------------------------------------------------
  IsPathInDAVList: look for a path in the list of DAV paths.
  ----------------------------------------------------------------------*/
ThotBool IsPathInDAVList (char *path)
{
  Prop_DAV_Path  *element;
  int             l;

  if (path == NULL || *path == EOS)
    return FALSE;
  element = DAV_Paths;
  while (element)
    {
      if (element->Path)
        {
          l = strlen (element->Path);
          if (!strncmp (element->Path, path, l))
            return TRUE;
        }
      element = element->NextPath;
    }
  return FALSE;
}

/*----------------------------------------------------------------------
  FreeDAVPathsList: Free the list of DAV paths.
  list : address of the list (address of the first element).
  ----------------------------------------------------------------------*/
void FreeDAVPathsList ()
{
  Prop_DAV_Path  *element = DAV_Paths;

  while (element)
    {
      Prop_DAV_Path* next = element->NextPath;
      TtaFreeMemory (element->Path);
      TtaFreeMemory (element);
      element = next;
    }
  DAV_Paths = NULL;
}


/*----------------------------------------------------------------------
  LoadDAVPathsList: Load the list of DAV paths.
  return : the number of readed paths.
  ----------------------------------------------------------------------*/
static int LoadDAVPathsList ()
{
  Prop_DAV_Path *element, *current = NULL;
  char          *path, *homePath, *s, *ptr;
  unsigned char *c;
  int            nb;
  FILE          *file;

  path = (char *) TtaGetMemory (MAX_LENGTH);
  // get the old list of webdav resources
  s = TtaGetEnvString ("DAV_URLS");
  if (s && s[0] != EOS)
    {
      strncpy (path, s, MAX_LENGTH - 1);
      path[MAX_LENGTH - 1] = EOS;
      ptr = strstr (s, " ");
      if (ptr)
        *ptr = EOS;
      element = (Prop_DAV_Path*) TtaGetMemory (sizeof(Prop_DAV_Path));
      element->NextPath = NULL;
      element->Path = TtaStrdup (path);
      DAV_Paths = element;
      current = element;
      TtaSetEnvString ("DAV_URLS", "", TRUE);
    }
  // open the file
  homePath       = TtaGetEnvString ("APP_HOME");
  sprintf (path, "%s%cdav.dat", homePath, DIR_SEP);
  file = TtaReadOpen ((char *)path);
  nb = 0;
  if (file)
    {
      // read the file
      c = (unsigned char*)path;
      *c = EOS;
      while (TtaReadByte (file, c))
        {
          if (*c == 13 || *c == EOL)
            *c = EOS;
          if (*c == EOS && c != (unsigned char*)path )
            {
              element = (Prop_DAV_Path*) TtaGetMemory (sizeof(Prop_DAV_Path));
              element->NextPath = NULL;
              element->Path = TtaStrdup (path);

              if ( DAV_Paths== NULL)
                DAV_Paths = element;
              else
                current->NextPath = element;
              current = element;
              nb++;

              c = (unsigned char*) path;
              *c = EOS;
            }
          else
            c++;
        }
      if (c != (unsigned char*)path && *path != EOS)
        {
          element = (Prop_DAV_Path*) TtaGetMemory (sizeof(Prop_DAV_Path));
          *(c+1) = EOS;
          element->Path =TtaStrdup (path);
          element->NextPath = NULL;

          if (DAV_Paths == NULL)
            DAV_Paths = element;
          else
            current->NextPath = element;
          nb++;
        }
      TtaReadClose (file);
    }
  TtaFreeMemory(path);
  if (s && s[0] != EOS)
    {
      SaveDAVPathsList ();
      nb++;
    }
  return nb;
}

/*----------------------------------------------------------------------
  InitDAVPreferences inits DAV preferences
  ---------------------------------------------------------------------- */
void InitDAVPreferences ()
{
  DAVBase = TtaSetCallback ((Proc)DAVPreferencesDlg_callback,
                            MAX_DAVPREF_DLG);
  LoadDAVPathsList ();
}

