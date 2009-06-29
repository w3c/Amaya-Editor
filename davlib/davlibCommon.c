/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 * 
 */
 
/*  --------------------------------------------------------
** 
** File: davlibCommon.c - WebDAV module _ common functions
**
** This file implements common functions for the WebDAV 
** functions implemented into Amaya editor.
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
** Revision 1.18  2009-06-29 08:28:58  vatton
** Fix Windows warnings
** Irene
**
** Revision 1.17  2009/06/10 10:57:23  vatton
** Change the management of Templates list
** + Implementation of a new WebDAV list of sites
** + Fix problems with lock/unlock status
** Irene
**
** Revision 1.16  2009/04/24 14:29:53  vatton
** Check webdav locks even if precondition is requested
** Irene
**
** Revision 1.15  2009/04/10 14:22:18  vatton
** Fix several WebDAV bugs
** Irene
**
** Revision 1.14  2008/05/13 09:30:27  kia
** More char* fixes
**
** Revision 1.13  2005/06/23 15:00:48  cvs
** fix 2 memory leak.
** (I hope it can help for MacOSX crash on http://www.w3.org/People/all ?)
** S. GULLY
**
** Revision 1.12  2005/06/07 13:37:11  gully
** code cleaning + warning fixes
** S. GULLY
**
** Revision 1.11  2003/11/19 12:33:16  gully
** Compilation fix (webdav support)
**
** S. GULLY
**
** Revision 1.10  2003/11/04 13:07:42  vatton
** Fix warnings reported by insure.
** Irene
**
** Revision 1.9  2002/07/01 10:34:16  kahan
** JK: Enabling/Disabling DAV support by means of the new DAV_Enable
** registry entry.
** Removed the DAV menu from the standard profile.
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
** Revision 1.6  2002/06/11 17:34:52  kirschpi
** improving commentaries and format (for WebDAV code).
** update DAV Resources list automatically
** Manuele
**
** Revision 1.5  2002/06/06 17:10:46  kirschpi
** Breaking the user messages in three lines
** Fixing some code format problems
** Fixing DAVLockIndicator, when Lock discovery is disabled.
** Fixing unecessary memory allocations in FilterMultiStatus_handler
** and FilterLocked_handler.
** Manuele
**
** Revision 1.4  2002/06/05 16:46:06  kirschpi
** Applying Amaya code format.
** Modifying some dialogs (looking for a better windows presentation)
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

#define WEBDAV_EXPORT extern
#include "davlib.h"

#include "davlibCommon_f.h"
#include "AHTLockBase_f.h"

#include "query_f.h"
#include "init_f.h"
#include "AHTURLTools_f.h"
#include "HTAssoc.h"
#include "davlibUI_f.h"


/* ********************************************************************* *
 *                              USEFUL FUNCTIONS                         *
 * ********************************************************************* */


/*----------------------------------------------------------------------
  Returns the local hostname with full qualify domain name
  ----------------------------------------------------------------------*/
const char * DAVFQDN (void) 
{
  HTRequest *request = HTRequest_new();    
  HTUserProfile *user = HTRequest_userProfile (request);
  const char *fqdn = (user)?HTUserProfile_fqdn(user):NULL;
    
  HTRequest_delete(request);
    
  if (!fqdn) fqdn = "localhost.localdomain";
    
  return fqdn;
}


/*----------------------------------------------------------------------
  Returns the default user email
  ----------------------------------------------------------------------*/
char * DAVDefaultEmail (void) 
{
  HTRequest *request = HTRequest_new();    
  HTUserProfile *user = HTRequest_userProfile (request);
  char *email = (user)?HTUserProfile_email(user):NULL;
  HTRequest_delete(request);
  return email;
}


/*----------------------------------------------------------------------
  Breaks a string in two segments (the second segment starts at the 
  firs with space ' ' after the middle of the string).
  Return: char * : pointer to the second segment of the string
  Note: it changes the orignal string, putting a '\0' between
  the two segments
  ----------------------------------------------------------------------*/
char * DAVBreakString (char *original) 
{
  char *ptr = NULL;
  char *next = NULL;
  char *last = NULL;
  int len = 0, y = 0;
  BOOL middle = NO;
  
  len = strlen (original)/2;
  /* less than 5 characters, ignore */
  if (len<5)
    return NULL;
    
  ptr = strchr (original, ' ');
  next = (ptr)?ptr+1:NULL;
  while (next && *next && !middle) 
    {
      last = ptr;
      y = strlen (next);
      if (y<len)
        middle = YES;
      else 
        {
          ptr = strchr (next, ' ');
          next = (ptr)?ptr+1:NULL;
        }
    }
  if (last) 
    {
      (*last) = '\0';
      last++;
    }
  return last;
}


/*----------------------------------------------------------------------
  Calculates a time_t value for the timeout string
  ----------------------------------------------------------------------*/
time_t DAVTimeoutValue (char *timeout) 
{
  time_t now,tout;
  char *ptr = NULL;
        
  time(&now);                
  if (HTStrCaseStr(timeout, (char*)"Infinite")!=NULL) tout = now;
  else if (HTStrCaseStr(timeout, (char*)"Second-")!=NULL) 
    {
      ptr = strchr(timeout,'-') + 1;
      tout = (time_t) atol(ptr);
    }
  else tout = 0;          /*a unknown timeout notation,do not consider */
  return tout;
}


/*----------------------------------------------------------------------
  Copy the file 'filename' to a string 
  ----------------------------------------------------------------------*/
char * DAVCopyFile (char * filename, int size) 
{
  char *document = NULL, realname[MAX_LENGTH];
  FILE *fp;
  int i = 0;
  char c[1];

  if (filename) 
    {
      document = (char *)TtaGetMemory (size + 1);
      NormalizeFile (filename, realname, AM_CONV_ALL);
      fp = TtaReadOpen (realname);
      if (fp)
        {
          while (fread(c,1,1,fp))
            document[i++] = c[0];
          document[i] = EOS;     
          TtaReadClose (fp);
        }
    }

  return document;
}


/*----------------------------------------------------------------------
  Verifies if a URL is a WebDAV resource used by the user.
  ----------------------------------------------------------------------*/
ThotBool DAVAllowResource (char *url)  
{
  HTList     *entries = NULL;
  char       *host, *rel, *next;
  ThotBool    match = FALSE;

  host = rel = next = NULL;
  if (!(url && *url))
    return FALSE;
  /* verifies the server list */
  match = IsPathInDAVList (url);
  /* if the url is not in DAVResources list
   * verifies if it is in the lock base
   */ 
  if (!match && separateUri (url, DAVFullHostName, &host, &rel)) 
    {
      entries = searchLockBase (host, rel);
      if (entries && !HTList_isEmpty (entries))
        match = TRUE;
    }

  /* free everything */
  if (entries)
    {
      while ((next = (char *) HTList_nextObject (entries)))
        HT_FREE (next);
      HT_FREE (entries);
    }
  if (rel)
    HT_FREE (rel);
  if (host)
    HT_FREE (host);
  return match;
}


/*----------------------------------------------------------------------
  Returns the most recent valid Lock-Token found in the lock base 
  for the given URL.

  Parameters:
  char * hostname: host (server) name
  char * relative: relative URL
  Returns:
  char * with the lock-token, or NULL if fails
  ----------------------------------------------------------------------*/
char * DAVFindLockToken (char *hostname, char *relative)
{
  char   *lock    = NULL;
  HTList *matches = NULL;
  LockLine *line  = NULL;
  LockLine *last  = NULL;
  time_t itime, tout;
  time_t last_itime;
    
  /* search matches in lock base */
  matches = processLockFile ((const char*)hostname, (const char*)relative);
  if (!matches)      /* no matches, returning NULL */
    return NULL;

  while (!HTList_isEmpty(matches) && 
         (line = (LockLine *) HTList_nextObject(matches)) !=NULL ) 
    {

      /* verify if it is an exact match. If we already found it, use 
       * the loop to free the LockLine objects too */
      if (strcasecomp (line->relativeURI,relative)!=0) 
        {
          LockLine_delete (line); 
          continue;
        }
        
      /* we don't need to verify if the lock token expired, because we 
       * can only unlock a resource after get it, consequently, after 
       * the DAVFindLock, which upadtes the base*/
      itime = strtotime (line->initialTime);    
      tout = DAVTimeoutValue (line->timeout);

        
      /* try to get the most recent token */ 
      last_itime = (last)?strtotime(last->initialTime):-1;
      if (last_itime < itime) 
        {
          if (last) LockLine_delete (last);
          last = line;
        }       
    } /* while */

  /* we found something, return the lock-token */
  if (last) 
    {
      StrAllocCopy (lock,last->lockToken);
      LockLine_delete (last); 
    }
    
#ifdef DEBUG_DAV
  fprintf (stderr,"DAVFindLockToken.... Returning %s\n",(lock)?lock:"NULL");
#endif
  HTList_delete (matches);
  return lock;
}



/*----------------------------------------------------------------------
  DAVGetLockFromTree: Get the lock information from the AwTree object.
  Parameters:
  AwTree * tree: where we should look for lock information  
  char * owner: for return purposes 
  Returns:
  LockLine *: lock information or NULL
  ----------------------------------------------------------------------*/
LockLine * DAVGetLockFromTree (AwTree * tree, char *owner) 
{
  LockLine * lock;
  AwNode * Nactivelock;
  AwNode * node;
  AwString tmp = NULL; 
  AwString pattern = NULL;
  char *Iowner, *Itimeout, *Ilocktoken, *Idepth, *Irelative;
  char *abs, *rel;
  char *s = NULL;

  lock = NULL;
  node = NULL;
  Nactivelock = NULL;
  Iowner = Itimeout = Ilocktoken = Idepth = Irelative = NULL;

  if (tree && owner) 
    {
      tmp = NULL; 
      pattern = AwString_new (15);
        
      /*look for lockdiscovery info*/
      AwString_set (pattern,"lockdiscovery");
      Nactivelock = AwTree_search (AwTree_getRoot(tree),pattern);
        
      if (Nactivelock && AwNode_howManyChildren(Nactivelock)>0) 
        {
                
          /* look for owner information */
          AwString_set (pattern,"owner");
          node = AwTree_search (Nactivelock,pattern);
          if (node) 
            {
              AwString_set (pattern,"href");
              node = AwTree_search (node,pattern);
              node = (node)?AwNode_nextChild (node):NULL;
              tmp = (node)?AwNode_getInfo (node):NULL;
              if (tmp) 
                {
                  Iowner = AwString_get(tmp);
                  AwString_delete (tmp);
                  tmp = NULL;
                }
            }
          if (Iowner && *Iowner) 
            {
              strcpy (owner, Iowner);
              HT_FREE (Iowner);
            }


          /* look for timeout information */
          AwString_set (pattern,"timeout");
          node = AwTree_search (Nactivelock,pattern);
          if (node) 
            {
              node = AwNode_nextChild (node);
              tmp = (node)?AwNode_getInfo (node):NULL;
              if (tmp) 
                {
                  Itimeout = AwString_get(tmp);
                  AwString_delete (tmp);
                  tmp = NULL;
                }
            }

            
          /* look for depth information */
          AwString_set (pattern,"depth");
          node = AwTree_search (Nactivelock,pattern);
          if (node) 
            {
              node = AwNode_nextChild (node);
              tmp = (node)?AwNode_getInfo (node):NULL;
              if (tmp) 
                {
                  Idepth = AwString_get(tmp);
                  AwString_delete (tmp);
                  tmp = NULL;
                }
            }


          /* look for locktoken information */
          AwString_set (pattern,"locktoken");
          node = AwTree_search (Nactivelock,pattern);
          if (node) 
            {
              AwString_set (pattern,"href");
              node = AwTree_search (node,pattern);
              node = (node)?AwNode_nextChild (node):NULL;
              tmp = (node)?AwNode_getInfo (node):NULL;
              if (tmp) 
                {
                  Ilocktoken = AwString_get(tmp);
                  AwString_delete (tmp);
                  tmp = NULL;  
                }

              /*put < and > in locktoken */
              if (Ilocktoken && *Ilocktoken!='<') 
                {
                  s = (char *)HT_CALLOC (strlen(Ilocktoken)+3,sizeof(char));
                  sprintf (s,"<%s>",Ilocktoken);
                  HT_FREE (Ilocktoken);
                  Ilocktoken = s;
                }
            }


          /* look for relative URL information */
          AwString_set (pattern,"href");
          node = AwTree_search (AwTree_getRoot(tree),pattern);
          node = (node)?AwNode_nextChild (node):NULL;
          tmp = (node)?AwNode_getInfo (node):NULL;
          if (tmp)
            {          
              Irelative = AwString_get(tmp);
              AwString_delete (tmp);
              tmp = NULL;
            }
          /* some servers use absolute URL in href element
           * others use the relative. Try to separate it. */
          if (Irelative) 
            {
              abs = rel = NULL;
              if (separateUri ((const char*) Irelative, 
                               (const char*)DAVFullHostName, &abs, &rel))
                {
                  HT_FREE (Irelative);
                  HT_FREE (abs);
                  Irelative = rel;
                }    
            }
           

          /* mounts LockLine object */
          lock = LockLine_newObject(Irelative, Ilocktoken, Idepth, Itimeout, time(NULL));
                    
          /* freeing everything */
          if (Irelative) HT_FREE (Irelative);
          if (Ilocktoken) HT_FREE (Ilocktoken);
          if (Idepth) HT_FREE (Idepth);
          if (Itimeout) HT_FREE (Itimeout);
                            
#ifdef DEBUG_DAV            
          fprintf (stderr,"DAVGetLockFromTree..... LockLine: %s Owner %s\n", 
                   (lock)?"OK":"NO",owner);
#endif
        }

    }

  return lock;
}
      



/* ********************************************************************* *
 *                 "If" HEADER MANIPULATION FUNCTIONS                    *
 * ********************************************************************* */


/*----------------------------------------------------------------------
 * DAVAddIfHeader - adds an If header (based on lock information found
 * in the lock base) 
 *
 * Parameters: 
 *        AHTReqContext *context: Request context object
 *        char *url: destiny URL
 * Return TRUE if a hthead is requested
 ----------------------------------------------------------------------*/
ThotBool DAVAddIfHeader (AHTReqContext *context, char *url) 
{
  AHTDAVContext *davctx = NULL;
  HTList * matches = NULL;
  char *ifHeader = NULL;
  
  if (!DAVLibEnable)
    return FALSE;

  if (context && url && (*url) && (context->request)) 
    {
      /*create DAV context*/
      davctx =  AHTDAVContext_new (url);
      if (davctx) 
        {
          ifHeader = NULL;
          matches = searchLockBase (davctx->absoluteURI, davctx->relativeURI);
#ifdef DEBUG_DAV            
          fprintf (stderr,"DAVAddIfHeader..... seaching for %s\n",url);
          fprintf (stderr,"DAVAddIfHeader..... matches? %s\n",
                   (matches && !HTList_isEmpty(matches))?"YES":"NO");
#endif         
          /*search in lock base: if found something creates the header */
          if (matches && !HTList_isEmpty(matches))
            ifHeader = mountIfHeader (matches);
          
#ifdef DEBUG_DAV
          fprintf (stderr,"DAVAddIfHeader..... if header %s\n",
                   (ifHeader)?ifHeader:"none");
#endif    
          if (ifHeader)
            {
              /*add If header */
              HTRequest_addExtraHeader (context->request, (char*)"If", ifHeader);
              return TRUE;
            }
          else
            return FALSE;
        }
    }
  return FALSE;
}


/*----------------------------------------------------------------------
 * DAVRemoveIfHeader - removes an If header from the request
 * 
 ----------------------------------------------------------------------*/
void DAVRemoveIfHeader (AHTReqContext *context) 
{
  if (!DAVLibEnable)
    return;

#ifdef DEBUG_DAV            
  fprintf (stderr,"DAVRemoveIfHeader..... Request for %s\n",context->urlName);
#endif          
  if (context) 
    {
      HTAssocList *headers = HTRequest_extraHeader (context->request);       
      if (headers)
        HTAssocList_removeObject (headers,"If");
    }
}


/* ********************************************************************* *
 *               AHTDAVContext MANIPULATION FUNCTIONS                    *
 * ********************************************************************* */


/*----------------------------------------------------------------------
  Create an AHTDAVContext object
  ----------------------------------------------------------------------*/
AHTDAVContext * AHTDAVContext_new (const char *url) 
{
  AHTDAVContext *me = NULL;

  if (!DAVLibEnable)
    return NULL;

  if (!url || !(*url)) 
    return NULL;
    
  if ( (me = (AHTDAVContext *)HT_CALLOC (1,sizeof(AHTDAVContext))) == NULL)
    outofmem ((char*)__FILE__, (char*)"AHTDAVContext_new");
    
  /* getting absolute and relative URI */
  if (!separateUri (url, DAVFullHostName,
                    &(me->absoluteURI),&(me->relativeURI))) 
    {
      HT_FREE (me);
      return NULL;
    }

#ifdef DEBUG_DAV
  fprintf (stderr,"AHTDAVContext..... Creating new AHTDAVContext object\n");    
#endif
    
  me->davheaders = HTDAVHeaders_new();
  me->output = NULL;
  me->debug = NULL;
  me->status = 0;
  me->xmlbody[0]= EOS;
  me->tree = NULL;
  me->showIt = YES;
  me->retry = NO;
  me->new_source = NULL;
  me->new_request = NULL;
  me->error_msgs = NULL;
  return me;
}



/*----------------------------------------------------------------------
  Delete an AHTDAVContext object
  ----------------------------------------------------------------------*/
void AHTDAVContext_delete (AHTDAVContext * me) 
{
  if (me) 
    {
#ifdef DEBUG_DAV
      fprintf (stderr,"AHTDAVContext..... Deleting AHTDAVContext object\n"); 
#endif
  
      /* when the request context is deleted, the output stream
       * is deleted too, so leave it to amaya normal behavior */
      /* if (me->output) HTChunk_delete (me->output); *
       * if (me->debug) HTChunk_delete (me->debug);   */
      if (me->davheaders)  HTDAVHeaders_delete (me->davheaders);
      if (me->absoluteURI) HT_FREE (me->absoluteURI);
      if (me->relativeURI) HT_FREE (me->relativeURI);
      if (me->tree) AwTree_delete (me->tree);
      if (me->error_msgs) AwList_delete (me->error_msgs);

      /* ignore new_source and new_request because they should be in use
       * when this context is deleted.*/ 
      me->new_source = NULL;
      me->new_request = NULL;
         
      HT_FREE (me);
      me = NULL;
    }
}




/* ********************************************************************* *
 *                AHTReqContext MANIPULATION FUNCTIONS                   *
 * ********************************************************************* */


/*----------------------------------------------------------------------
  Create an AHTReqContext object 

  Parameters:
  int doc : document
  char *url : destiny url name (cannot be NULL)
  AHTDAVContext *dav : WebDAV context
  HTNet *after : local after filter (position LAST)
  TTcbf *terminate : terminate callback function
  TIcbf *incremental : incremental callback function
  BOOL preemptive : should the request be preemptive?
  int mode : request mode
        
  ----------------------------------------------------------------------*/
AHTReqContext * DAVCreateDefaultContext (int doc, char *url, AHTDAVContext *dav,\
                                         HTNetAfter * after, TTcbf * terminate, \
                                         TIcbf * incremental, BOOL preemptive, int mode)  
{
  AHTReqContext *me = AHTReqContext_new (doc);
  if (me)
    {
      if (!url || !(*url))
        {
          AHTReqContext_delete (me);
          return NULL;
        }
        
      /* WebDav context */
      me->dav_context = dav;
        
      /* destiny url */
      me->urlName = (char *)TtaGetMemory (MAX_LENGTH + 2);
      strcpy (me->urlName, url);
      ChopURL (me->status_urlName,me->urlName);

      /* request mode */
      me->mode = mode;

      /* request callbacks */
      me->terminate_cbf = terminate;
      me->incremental_cbf = incremental;      
      if (me->terminate_cbf)
        me->context_tcbf = me;
      else 
        me->context_tcbf = NULL;
        
      if (me->incremental_cbf)
        me->context_icbf = me;
      else
        me->context_icbf = NULL;

      /* local after filter */
      if (after)
        HTRequest_addAfter (me->request,after,NULL,NULL,HT_ALL,HT_FILTER_LAST,NO);

      /* preemptive */
      HTRequest_setPreemptive (me->request, preemptive);

      /* initial status */
      me->reqStatus = HT_NEW;
    }

  return me;
}


/*----------------------------------------------------------------------
  Copy an AHTDAVContext object to a new object for retry purposes
  ----------------------------------------------------------------------*/
AHTReqContext * DAVCopyContext (AHTReqContext *context) 
{
  AHTReqContext *me = NULL;

  if (context) 
    {
#ifdef DEBUG_DAV
      fprintf (stderr,"Copycontext.... copying the request context\n");
#endif              
      me = DAVCreateDefaultContext (context->docid, context->urlName,NULL, NULL, 
                                    context->terminate_cbf, context->incremental_cbf,
                                    HTRequest_preemptive (context->request),
                                    context->mode);
      if (!me) 
        return NULL;

      me->method = context->method;

      me->anchor = context->anchor;
      me->dest = context->dest;

      HTRequest_setOutputFormat (me->request, HTRequest_outputFormat(context->request));
      HTRequest_setOutputStream (me->request, HTRequest_outputStream (context->request));

      me->output = context->output;
      context->output = NULL;
      me->outputfile = context->outputfile;
      context->outputfile = NULL;
      me->formdata = context->formdata;
      context->formdata = NULL;

      me->context_tcbf = context->context_tcbf;
      me->context_icbf = context->context_icbf;
        
      me->error_html = context->error_html;
    }

  return me;
}


