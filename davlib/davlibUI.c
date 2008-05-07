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
** Revision 1.28  2008-05-07 13:49:07  kia
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

#define WEBDAV_EXPORT extern
#include "davlib.h"
#include "davlib_f.h"
#include "davlibUI_f.h"
#include "davlibCommon_f.h"
#include "davlibRequests_f.h"

#include "init_f.h"
#include "query_f.h"
#include "MENUconf.h"
#include "MENUconf_f.h"


/* ********************************************************************* *
 *                         PRIVATE VARIABLES                             *
 * ********************************************************************* */

extern int      DAVBase;
extern Prop_DAV GProp_DAV;
#ifdef _WINGUI
#include "resource.h"
extern HINSTANCE    hInstance;
static HWND     DAVDlg;
#endif /* _WINGUI */


/*----------------------------------------------------------------------
  DAVSetLockIndicator: set the Lock indicator button.
  ---------------------------------------------------------------------- */
void DAVSetLockIndicator (Document docid) 
{
  /* updates Lock indicator*/ 
  TtaSetToggleItem (docid, DAV_VIEW,Cooperation_, BLockIndicator, DAVLockIndicatorState);  
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
void DAVPropertiesVerticalDialog (Document docid, const char *title,
    const char *rheader, const char *lheader, AwList *list) 
{
#ifdef _GTK
  char   *name, *value;
  char    label[MAX_LENGTH];
  char   *ns = NULL;
  int     i = MAX_REF+1, form = MAX_REF;
#endif /* _GTK */
  int     lines = 0;

  if (docid > 0 && list) 
    {
      lines = AwList_size(list)/2;
#ifdef _GTK
      /* Main form */
      TtaNewSheet (BaseDialog + form, 
                   TtaGetViewFrame (docid, DAV_VIEW), 
                   (title)?title:(char *)" ", 0, NULL, FALSE, 
                   (rheader && lheader)?lines+3:lines+1, 'L', D_DONE);
      
      if (rheader || lheader) 
        {
          TtaNewLabel (BaseDialog + (i++), BaseDialog + form,
                       (lheader)?lheader:(char *)" ");
          TtaNewLabel (BaseDialog + (i++), BaseDialog + form,
                       (char *)"----------------");
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
                   (char *)"              ");

      if (rheader || lheader) 
        {
          TtaNewLabel (BaseDialog + (i++), BaseDialog + form,
                       (rheader)?rheader:(char *)" ");
          TtaNewLabel (BaseDialog + (i++), BaseDialog + form,
                       (char *)"------------------------------");
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
                   (char *)"              ");
      TtaSetDialoguePosition ();
      TtaShowDialogue (BaseDialog + form, TRUE);            
#else /* _GTK */
      /* function not implemented yet */
      InitInfo ("", TtaGetMessage(LIB, TMSG_NOT_AVAILABLE));
#endif /* _GTK */
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
#ifdef _GTK
  char   *name, *value;    
  char    label[MAX_LENGTH];
  int     i = MAX_REF+1, form = MAX_REF;
#endif /* _GTK */

  if (docid > 0 && list) 
    {
#ifdef _GTK
      /* Main form */
      TtaNewSheet (BaseDialog + form, 
                   TtaGetViewFrame (docid, DAV_VIEW), 
                   (title)?title:(char *)" ", 0, NULL, TRUE, 
                   2, 'L', D_DONE);
      
      if (rheader || lheader) 
        {
          TtaNewLabel (BaseDialog + (i++), BaseDialog + form,
                       (lheader)?lheader:(char *)" ");
          TtaNewLabel (BaseDialog + (i++), BaseDialog + form,
                       (rheader)?rheader:(char *)" ");
          TtaNewLabel (BaseDialog + (i++), BaseDialog + form,
                       (char *)"----------------");
          TtaNewLabel (BaseDialog + (i++), BaseDialog + form,
                       (char *)"---------------------------");
        }

      /* names  values */ 
      while ( list && (name = (char *)AwList_next (list)) 
              && (value = (char *)AwList_next (list)) ) 
        {
          /* name  */
          sprintf (label, "%s ", name);
          TtaNewLabel (BaseDialog + (i++), BaseDialog + form, label);
          sprintf (label, "%s ", value);
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
#else /* _GTK */
      /* function not implemented yet */
      InitInfo ("", TtaGetMessage(LIB, TMSG_NOT_AVAILABLE));
#endif /* _GTK */
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
                      while ((child = AwNode_nextChild(Nactivelock)) != NULL) 
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
  
  /* ***** User's resource list - DAVResources  ***** */
  /* it can be empty, so we don't verify it */
  strcpy (DAVResources, GProp_DAV.textUserResources);
  
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

  /* ***** Awareness values - DAVAwareness, DAVAwarenessExit  **** */
  DAVAwareness = (GProp_DAV.toggleAwareness1)?YES:NO;
  DAVAwarenessExit = (GProp_DAV.toggleAwareness2)?YES:NO;

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
  /* user resource list */
  strcpy (GProp_DAV.textUserResources, DAVResources);
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

  /* awareness toggle */
  GProp_DAV.toggleAwareness1 = (DAVAwareness)?TRUE:FALSE;
  GProp_DAV.toggleAwareness2 = (DAVAwarenessExit)?TRUE:FALSE;

#ifdef _WINGUI
  SetWindowText (GetDlgItem (DAVDlg, IDC_DAVUSER), GProp_DAV.textUserReference);
  SetWindowText (GetDlgItem (DAVDlg, IDC_DAVRESOURCES), GProp_DAV.textUserResources);
  if (!strcmp (GProp_DAV.radioDepth, "0"))
    CheckRadioButton (DAVDlg, IDC_ZERO_DAVDEPTH, IDC_INFINITE_DAVDEPTH, IDC_ZERO_DAVDEPTH);
  else
    CheckRadioButton (DAVDlg, IDC_ZERO_DAVDEPTH, IDC_INFINITE_DAVDEPTH,
                      IDC_INFINITE_DAVDEPTH);
  if (!strcmp (GProp_DAV.radioLockScope, "exclusive"))
    CheckRadioButton (DAVDlg, IDC_EXCLUSIVE_DAVSCOPE, IDC_SHARED_DAVSCOPE,
                      IDC_EXCLUSIVE_DAVSCOPE);
  else
    CheckRadioButton (DAVDlg, IDC_EXCLUSIVE_DAVSCOPE, IDC_SHARED_DAVSCOPE,
                      IDC_SHARED_DAVSCOPE);
  if (!strcmp (GProp_DAV.radioTimeout, "Infinite"))
    CheckRadioButton (DAVDlg, IDC_INFINITE_DAVTIMEOUT, IDC_OTHER_DAVTIMEOUT,
                      IDC_INFINITE_DAVTIMEOUT);
  else
    CheckRadioButton (DAVDlg, IDC_INFINITE_DAVTIMEOUT, IDC_OTHER_DAVTIMEOUT,
                      IDC_OTHER_DAVTIMEOUT);

  SetDlgItemInt (DAVDlg, IDC_TIMEOUT_VALUE, GProp_DAV.numberTimeout, FALSE);

  CheckDlgButton (DAVDlg, IDC_GENERAL_DAVAWARENESS, (GProp_DAV.toggleAwareness1) 
                  ? BST_CHECKED : BST_UNCHECKED);
  CheckDlgButton (DAVDlg, IDC_EXIT_DAVAWARENESS, (GProp_DAV.toggleAwareness2) 
                  ? BST_CHECKED : BST_UNCHECKED);
#endif /* _WINGUI */
#ifdef _WX
#endif /* _WX */
#ifdef _GTK
  TtaSetTextForm (DAVBase + DAVtextUserReference,
                  GProp_DAV.textUserReference);
  TtaSetTextForm (DAVBase + DAVtextUserResources,
                  GProp_DAV.textUserResources);
  if (!strcmp (GProp_DAV.radioDepth, "infinity")) 
    TtaSetMenuForm (DAVBase + DAVradioDepth, 1);
  else 
    TtaSetMenuForm (DAVBase + DAVradioDepth, 0);
  if (!strcmp (GProp_DAV.radioLockScope, "shared")) 
    TtaSetMenuForm (DAVBase + DAVradioLockScope, 1);
  else 
    TtaSetMenuForm (DAVBase + DAVradioLockScope, 0);
  if (!strcmp (GProp_DAV.radioTimeout, "Infinite"))
    TtaSetMenuForm (DAVBase + DAVradioTimeout, 0);
  else
    TtaSetMenuForm (DAVBase + DAVradioTimeout, 1);
  TtaSetNumberForm (DAVBase + DAVnumberTimeout,
                    GProp_DAV.numberTimeout);
  TtaSetToggleMenu (DAVBase + DAVtoggleAwareness, 0,
                    GProp_DAV.toggleAwareness1);
  TtaSetToggleMenu (DAVBase + DAVtoggleAwareness, 1,
                    GProp_DAV.toggleAwareness2);
#endif /* _GTK */    
}


#ifdef _WINGUI
/*----------------------------------------------------------------------
  WIN_AnnotDlgProc
  Windows callback for the annot menu
  ----------------------------------------------------------------------*/
LRESULT CALLBACK WIN_DAVPreferencesDlg (HWND hwnDlg, UINT msg, WPARAM wParam,
                                        LPARAM lParam)
{
  switch (msg)
    {
    case WM_INITDIALOG:
      /* initialize the menu text */
      DAVDlg = hwnDlg;
      SetWindowText (GetDlgItem (hwnDlg, ID_APPLY),
                     TtaGetMessage (AMAYA, AM_APPLY_BUTTON));
      SetWindowText (GetDlgItem (hwnDlg, ID_DEFAULTS),
                     TtaGetMessage (AMAYA, AM_DEFAULT_BUTTON));	    
      SetWindowText (GetDlgItem (hwnDlg, ID_DONE),
                     TtaGetMessage (LIB, TMSG_DONE));

      SetWindowText (GetDlgItem (hwnDlg, IDC_TXT_DAVUSER),
                     TtaGetMessage (AMAYA, AM_DAV_USER_URL));
      SetWindowText (GetDlgItem (hwnDlg, IDC_TXT_DAVRESOURCES),
                     TtaGetMessage (AMAYA, AM_DAV_USER_RESOURCES));

      SetWindowText (GetDlgItem (hwnDlg, IDC_DAVDEPTH),
                     TtaGetMessage (AMAYA, AM_DAV_DEPTH));
      SetWindowText (GetDlgItem (hwnDlg, IDC_ZERO_DAVDEPTH), "0");
      SetWindowText (GetDlgItem (hwnDlg, IDC_INFINITE_DAVDEPTH),
                     TtaGetMessage (AMAYA, AM_DAV_DEPTH_INFINITY));

      SetWindowText (GetDlgItem (hwnDlg, IDC_DAVSCOPE),
                     TtaGetMessage (AMAYA, AM_DAV_LOCKSCOPE));
      SetWindowText (GetDlgItem (hwnDlg, IDC_EXCLUSIVE_DAVSCOPE),
                     TtaGetMessage (AMAYA, AM_DAV_LOCKSCOPE_EXCLUSIVE));
      SetWindowText (GetDlgItem (hwnDlg, IDC_SHARED_DAVSCOPE),
                     TtaGetMessage (AMAYA, AM_DAV_LOCKSCOPE_SHARED));

      SetWindowText (GetDlgItem (hwnDlg, IDC_DAVTIMEOUT),
                     TtaGetMessage (AMAYA, AM_DAV_TIMEOUT));
      SetWindowText (GetDlgItem (hwnDlg, IDC_INFINITE_DAVTIMEOUT),
                     TtaGetMessage (AMAYA, AM_DAV_TIMEOUT_INFINITE));
      SetWindowText (GetDlgItem (hwnDlg, IDC_OTHER_DAVTIMEOUT),
                     TtaGetMessage (AMAYA, AM_DAV_TIMEOUT_OTHER));
      SetWindowText (GetDlgItem (hwnDlg, IDC_TIMEOUT_SECOND), "");

      SetWindowText (GetDlgItem (hwnDlg, IDC_DAVAWARENESS),
                     TtaGetMessage (AMAYA, AM_DAV_AWARENESS));
      SetWindowText (GetDlgItem (hwnDlg, IDC_GENERAL_DAVAWARENESS),
                     TtaGetMessage (AMAYA, AM_DAV_AWARENESS_GENERAL));
      SetWindowText (GetDlgItem (hwnDlg, IDC_EXIT_DAVAWARENESS),
                     TtaGetMessage (AMAYA, AM_DAV_AWARENESS_ONEXIT));
      GetDAVConf();
      break;
      
    case WM_CLOSE:
    case WM_DESTROY:
      /* reset the status flag */
      DAVDlg = NULL;
      EndDialog (hwnDlg, ID_DONE);
      break;

    case WM_COMMAND:
      if (HIWORD (wParam) == EN_UPDATE)
        {
          switch (LOWORD (wParam))
            {
            case IDC_DAVUSER:
              GetDlgItemText (hwnDlg, IDC_DAVUSER, GProp_DAV.textUserReference,
                              MAX_LENGTH - 1);
              break;
            case IDC_DAVRESOURCES:
              GetDlgItemText (hwnDlg, IDC_DAVRESOURCES, GProp_DAV.textUserResources,
                              MAX_LENGTH - 1);
              break;
            case IDC_TIMEOUT_VALUE:
              GProp_DAV.numberTimeout = GetDlgItemInt (hwnDlg, IDC_TIMEOUT_VALUE,
                                                       FALSE, FALSE);
              break;
            }
        }
      switch (LOWORD (wParam))
        {
          /* toggle buttons */
        case IDC_ZERO_DAVDEPTH:
          strcpy (GProp_DAV.radioDepth, "0");
          CheckRadioButton (DAVDlg, IDC_ZERO_DAVDEPTH, IDC_INFINITE_DAVDEPTH,
                            IDC_ZERO_DAVDEPTH);
          break;
        case IDC_INFINITE_DAVDEPTH:
          strcpy (GProp_DAV.radioDepth, "infinity");
          CheckRadioButton (DAVDlg, IDC_ZERO_DAVDEPTH, IDC_INFINITE_DAVDEPTH,
                            IDC_INFINITE_DAVDEPTH);
          break;
        case IDC_EXCLUSIVE_DAVSCOPE:
          strcpy (GProp_DAV.radioLockScope, "exclusive");
          CheckRadioButton (DAVDlg, IDC_EXCLUSIVE_DAVSCOPE, IDC_SHARED_DAVSCOPE,
                            IDC_EXCLUSIVE_DAVSCOPE);
          break;
        case IDC_SHARED_DAVSCOPE:
          strcpy (GProp_DAV.radioLockScope, "shared");
          CheckRadioButton (DAVDlg, IDC_EXCLUSIVE_DAVSCOPE, IDC_SHARED_DAVSCOPE,
                            IDC_SHARED_DAVSCOPE);
          break;
        case IDC_INFINITE_DAVTIMEOUT:
          strcpy (GProp_DAV.radioTimeout, "Infinite");
          CheckRadioButton (DAVDlg, IDC_INFINITE_DAVTIMEOUT, IDC_OTHER_DAVTIMEOUT,
                            IDC_INFINITE_DAVTIMEOUT);
          break;
        case IDC_OTHER_DAVTIMEOUT:
          strcpy (GProp_DAV.radioTimeout, "Second-");
          CheckRadioButton (DAVDlg, IDC_INFINITE_DAVTIMEOUT, IDC_OTHER_DAVTIMEOUT,
                            IDC_OTHER_DAVTIMEOUT);
          break;
        case IDC_GENERAL_DAVAWARENESS:
          GProp_DAV.toggleAwareness1 = !(GProp_DAV.toggleAwareness1);
          break;
        case IDC_EXIT_DAVAWARENESS:
          GProp_DAV.toggleAwareness2 = !(GProp_DAV.toggleAwareness2);
          break;

          /* action buttons */
        case ID_APPLY:
          SetDAVConf();
          /* reset the status flag */
          EndDialog (hwnDlg, ID_DONE);
          break;
        case ID_DONE:
        case IDCANCEL:
          /* reset the status flag */
          DAVDlg = NULL;
          EndDialog (hwnDlg, ID_DONE);
          break;
        case ID_DEFAULTS:
          GetDAVConf();
          break;
        }
      break;	     
    default: return FALSE;
    }
  return TRUE;
}
#else /* _WINGUI */
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
#ifndef _WX
              TtaDestroyDialogue (DAVBase + DAVPreferencesDlg);
#endif /* _WX */
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

        case DAVtextUserResources :
          strcpy (GProp_DAV.textUserResources, data);
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
	  
        case DAVtoggleAwareness :
          switch ((long int)data) 
            {
            case 0:
              GProp_DAV.toggleAwareness1 = !GProp_DAV.toggleAwareness1;
              break;
            case 1:
              GProp_DAV.toggleAwareness2 = !GProp_DAV.toggleAwareness2;
              break;
            }
          break;
        }
    }
}
#endif /* _WINGUI */


/*----------------------------------------------------------------------
  InitDAVPreferences inits DAV preferences
  ---------------------------------------------------------------------- */
void InitDAVPreferences ()
{
#ifndef _WINGUI
  DAVBase = TtaSetCallback ((Proc)DAVPreferencesDlg_callback,
                            MAX_DAVPREF_DLG);
#endif /* _WINGUI */
}

/*----------------------------------------------------------------------
  DAVShowPreferencesDlg: shows the DAV preferences dialog
  ---------------------------------------------------------------------- */
void DAVShowPreferencesDlg (Document document)
{
#ifdef _GTK
  char buf[MAX_LENGTH];
    
  sprintf (buf,"%s%c%s%c",TtaGetMessage (AMAYA, AM_APPLY_BUTTON),EOS,
           TtaGetMessage (AMAYA, AM_DEFAULT_BUTTON),EOS);
  TtaNewSheet (DAVBase + DAVPreferencesDlg, 
               TtaGetViewFrame (document, DAV_VIEW),
               TtaGetMessage (AMAYA, AM_DAV_PREFERENCES),
               2, buf, TRUE, 3, 'L', D_DONE);
    
  /* first line */
  TtaNewTextForm (DAVBase + DAVtextUserReference,
                  DAVBase + DAVPreferencesDlg,
                  TtaGetMessage (AMAYA, AM_DAV_USER_URL),
                  40, 1, FALSE);
  TtaNewLabel (DAVBase + DAVlabelEmpty1,DAVBase + DAVPreferencesDlg, " ");
  TtaNewLabel (DAVBase + DAVlabelEmpty2,DAVBase + DAVPreferencesDlg, " ");
  
  /* second line */
  sprintf (buf, "B0%cB%s%c", EOS,TtaGetMessage (AMAYA, AM_DAV_DEPTH_INFINITY), EOS);
  TtaNewSubmenu (DAVBase + DAVradioDepth,
                 DAVBase + DAVPreferencesDlg,
                 0, TtaGetMessage (AMAYA, AM_DAV_DEPTH), 2, buf, NULL, 0, FALSE);
  sprintf (buf, "B%s%cB%s%c",TtaGetMessage (AMAYA, AM_DAV_TIMEOUT_INFINITE),EOS,
           TtaGetMessage (AMAYA, AM_DAV_TIMEOUT_OTHER), EOS);
  TtaNewSubmenu (DAVBase + DAVradioTimeout,
                 DAVBase + DAVPreferencesDlg,
                 0,TtaGetMessage (AMAYA, AM_DAV_TIMEOUT), 2, buf, NULL, 0,FALSE);
  TtaNewNumberForm (DAVBase + DAVnumberTimeout,
                    DAVBase + DAVPreferencesDlg,
                    "", 300,9999, FALSE);  
  
  /* third line */
  sprintf (buf, "B%s%cB%s%c",TtaGetMessage (AMAYA, AM_DAV_LOCKSCOPE_EXCLUSIVE), EOS, 
           TtaGetMessage (AMAYA, AM_DAV_LOCKSCOPE_SHARED), EOS);
  TtaNewSubmenu (DAVBase + DAVradioLockScope,
                 DAVBase + DAVPreferencesDlg,
                 0, TtaGetMessage (AMAYA, AM_DAV_LOCKSCOPE), 2, buf, NULL, 0, FALSE);
  TtaNewLabel (DAVBase + DAVlabelEmpty3,DAVBase + DAVPreferencesDlg, " ");
  TtaNewLabel (DAVBase + DAVlabelEmpty4,DAVBase + DAVPreferencesDlg, " ");
  
  /* fourth line */
  sprintf (buf, "B%s%cB%s%c", TtaGetMessage (AMAYA, AM_DAV_AWARENESS_GENERAL), EOS, 
           TtaGetMessage (AMAYA, AM_DAV_AWARENESS_ONEXIT), EOS);
  TtaNewToggleMenu (DAVBase + DAVtoggleAwareness,
                    DAVBase + DAVPreferencesDlg,
                    TtaGetMessage (AMAYA, AM_DAV_AWARENESS), 2, buf, NULL, TRUE);
  TtaNewLabel (DAVBase + DAVlabelEmpty5,DAVBase + DAVPreferencesDlg, "    ");
  TtaNewLabel (DAVBase + DAVlabelEmpty6,DAVBase + DAVPreferencesDlg, "    ");
  
  /* fifth line */
  TtaNewTextForm (DAVBase + DAVtextUserResources,
                  DAVBase + DAVPreferencesDlg,
                  TtaGetMessage (AMAYA, AM_DAV_USER_RESOURCES),
                  40, 1, FALSE);
  
  /* get the active values and set the dialogue variables */
  GetDAVConf();
  
  /* show the dialogue */
  TtaSetDialoguePosition ();
  TtaShowDialogue (DAVBase + DAVPreferencesDlg, TRUE);
#endif /* _GTK */
#ifdef _WINGUI
  if (!DAVDlg)
    /* only activate the menu if it isn't active already */
    DialogBox (hInstance, MAKEINTRESOURCE (DAVCONFMENU), NULL,
               (DLGPROC) WIN_DAVPreferencesDlg);
  else
    SetFocus (DAVDlg);
#endif /* _WINGUI */
#ifdef _WX
#endif /* _WX */
}
