#include <glib.h>
#define THOT_EXPORT extern
#include "amaya.h"
#include "AHTBridge_f.h"
#include "AHTFWrite_f.h"
#include "query_f.h"
#include "answer_f.h"
#include "HTEvtLst.h"

extern ThotAppContext app_cont;
 
#define WWW_HIGH_PRIORITY (G_PRIORITY_HIGH_IDLE + 50)
#define WWW_LOW_PRIORITY G_PRIORITY_LOW
#define WWW_SCALE_PRIORITY(p) ((WWW_HIGH_PRIORITY - WWW_LOW_PRIORITY) * p \
                          / HT_PRIORITY_MAX + WWW_LOW_PRIORITY)
     
#define READ_CONDITION (G_IO_IN | G_IO_HUP | G_IO_ERR)
#define WRITE_CONDITION (G_IO_OUT | G_IO_ERR)
#define EXCEPTION_CONDITION (G_IO_PRI)
     
 typedef struct _SockEventInfo SockEventInfo;
 struct _SockEventInfo {
   SOCKET s;
   HTEventType type;
   HTEvent *event;
   guint io_tag;
   guint timer_tag;
 };
 
 typedef struct _SockInfo SockInfo;
 struct _SockInfo {
   SOCKET s;
   GIOChannel *io;
   SockEventInfo ev[HTEvent_TYPES];
 };
 
 static GHashTable *sockhash = NULL;
 
 
 static SockInfo *
 get_sock_info(SOCKET s, gboolean create)
 {
   SockInfo *info;
 
   if (!sockhash)
     sockhash = g_hash_table_new(g_direct_hash, g_direct_equal);
 
   info = g_hash_table_lookup(sockhash, GINT_TO_POINTER(s));
   if (!info && create) {
     info = g_new0(SockInfo, 1);
     info->s = s;
     info->io = g_io_channel_unix_new(s);
     info->ev[0].s = info->ev[1].s = info->ev[2].s = s;
     info->ev[0].type = HTEvent_READ;
     info->ev[1].type = HTEvent_WRITE;
     info->ev[2].type = HTEvent_OOB;
     g_hash_table_insert(sockhash, GINT_TO_POINTER(s), info);
   }
   return info;
 }
 
 static gboolean glibwww_timeout_func (gpointer data);
 static gboolean glibwww_io_func(GIOChannel *source, GIOCondition condition,
                                 gpointer data);
 
 int
 AHTEvent_register (SOCKET s, HTEventType type, HTEvent *event)
 {
   SockInfo *info;
   gint priority = G_PRIORITY_DEFAULT;
   GIOCondition condition;
 
   if (s == INVSOC || HTEvent_INDEX(type) >= HTEvent_TYPES)
     return 0;
 
   info = get_sock_info(s, TRUE);
   info->ev[HTEvent_INDEX(type)].event = event;
 
   switch (HTEvent_INDEX(type)) {
   case HTEvent_INDEX(HTEvent_READ):
     condition = READ_CONDITION;      break;
   case HTEvent_INDEX(HTEvent_WRITE):
     condition = WRITE_CONDITION;     break;
   case HTEvent_INDEX(HTEvent_OOB):
     condition = EXCEPTION_CONDITION; break;
   }
   if (event->priority != HT_PRIORITY_OFF)
     priority = WWW_SCALE_PRIORITY(event->priority);
 
   info->ev[HTEvent_INDEX(type)].io_tag =
     g_io_add_watch_full(info->io, priority, condition, glibwww_io_func,
                          &info->ev[HTEvent_INDEX(type)], NULL);
 
   if (event->millis >= 0)
     info->ev[HTEvent_INDEX(type)].timer_tag =
       g_timeout_add_full(priority, event->millis, glibwww_timeout_func,
                          &info->ev[HTEvent_INDEX(type)], NULL);
 
   return HT_OK;
 }
 
 int
 AHTEvent_unregister (SOCKET s, HTEventType type)
 {
   SockInfo *info = get_sock_info(s, FALSE);
 
   if (info) {
     if (info->ev[HTEvent_INDEX(type)].io_tag)
       g_source_remove(info->ev[HTEvent_INDEX(type)].io_tag);
     if (info->ev[HTEvent_INDEX(type)].timer_tag)
       g_source_remove(info->ev[HTEvent_INDEX(type)].timer_tag);
 
     info->ev[HTEvent_INDEX(type)].event = NULL;
     info->ev[HTEvent_INDEX(type)].io_tag = 0;
     info->ev[HTEvent_INDEX(type)].timer_tag = 0;
 
     /* clean up sock hash if needed */
     if (info->ev[0].event == NULL &&
         info->ev[1].event == NULL &&
         info->ev[2].event == NULL) {
       g_hash_table_remove(sockhash, GINT_TO_POINTER(s));
       g_io_channel_unref(info->io);
       g_free(info);
     }
     
     return HT_OK;
   }
   return HT_ERROR;
 }
 
 static gboolean
 glibwww_timeout_func (gpointer data)
 {
   SockEventInfo *info = (SockEventInfo *)data;
   HTEvent *event = info->event;
 
   (* event->cbf) (info->s, event->param, HTEvent_TIMEOUT);
   return TRUE;
 }
 
 static gboolean
 glibwww_io_func(GIOChannel *source, GIOCondition condition, gpointer data)
 {
   SockEventInfo *info = (SockEventInfo *)data;
   HTEvent *event = info->event;
 
   if (info->timer_tag)
       g_source_remove(info->timer_tag);
   if (info->event->millis >= 0) {
     gint priority = G_PRIORITY_DEFAULT;
 
     if (event->priority != HT_PRIORITY_OFF)
       priority = WWW_SCALE_PRIORITY(event->priority);
     info->timer_tag =
       g_timeout_add_full(priority, info->event->millis, glibwww_timeout_func,
                          info, NULL);
   }
 
   (* event->cbf) (info->s, event->param, info->type);
   return TRUE;
 }
 
/*--------------------------------------------------------------------
  ProcessTerminateRequest
  This function is called whenever a request has ended. If the requested
  ended normally, the function will call any callback associated to the
  request. Otherwise, it will just mark the request as over.
  -------------------------------------------------------------------*/
#ifdef __STDC__
void  ProcessTerminateRequest (HTRequest * request, HTResponse * response, void *param, int status)
#else
void ProcessTerminateRequest (request, response, param, status)
HTRequest *request;
HTResponse *response;
void *param;
int status;
#endif
{   
  AHTReqContext *me = HTRequest_context (request);

  /* choose a correct treatment in function of the request's
     being associated with an error, with an interruption, or with a
     succesful completion */

#ifdef DEBUG_LIBWWW  
  if (THD_TRACE)
    fprintf (stderr,"ProcessTerminateRequest: processing req %p, url %s, status %d\n", me, me->urlName, me->reqStatus);  
#endif /* DEBUG_LIBWWW */
  if (me->reqStatus == HT_END)
    {
      if (AmayaIsAlive ()  && me->terminate_cbf)
	(*me->terminate_cbf) (me->docid, 0, ISO2WideChar(me->urlName), ISO2WideChar(me->outputfile),
			      ISO2WideChar(me->content_type), me->context_tcbf);

    }
  else if (me->reqStatus == HT_ABORT)
    /* either the application ended or the user pressed the stop 
       button. We erase the incoming file, if it exists */
    {
      if (AmayaIsAlive () && me->terminate_cbf)
	(*me->terminate_cbf) (me->docid, -1, ISO2WideChar(me->urlName), ISO2WideChar(me->outputfile),
			      ISO2WideChar(me->content_type), me->context_tcbf);
      if (me->outputfile && me->outputfile[0] != EOS)
	{
	  TtaFileUnlink (ISO2WideChar (me->outputfile));
	  me->outputfile[0] = EOS;
	}
    }
  else if (me->reqStatus == HT_ERR)
    {
      /* there was an error */
      if (AmayaIsAlive && me->terminate_cbf)
	(*me->terminate_cbf) (me->docid, -1, ISO2WideChar(me->urlName), ISO2WideChar(me->outputfile),
			      ISO2WideChar(me->content_type), me->context_tcbf);
      
      if (me->outputfile && me->outputfile[0] != EOS)
	{
	  TtaFileUnlink (ISO2WideChar (me->outputfile));
	  me->outputfile[0] = EOS;
	}
    }

   /* we erase the context if we're dealing with an asynchronous request */
  if ((me->mode & AMAYA_ASYNC)
      || (me->mode & AMAYA_IASYNC))
    {
      Document doc = me->docid;
      me->reqStatus = HT_END;
      /*** @@@ do we need this? yes!! **/
      AHTLoadTerminate_handler (request, response, param, status);
      AHTReqContext_delete (me);
      AHTPrintPendingRequestStatus (doc, YES);
    }
}

/*----------------------------------------------------------------------
  RequestKillAllXtevents
  front-end for kill all Xt events associated with the request pointed
  to by "me".
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                RequestKillAllXtevents (AHTReqContext * me)
#else
void                RequestKillAllXtevents (me)
AHTReqContext      *me;
#endif /* __STDC__ */
{
}





