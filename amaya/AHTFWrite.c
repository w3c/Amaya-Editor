/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-1999-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
  
/*
 * AHTFWrite.c:  it's a rewrite of libwww's HTFWrite.c module. It
 * provides a callback to a user defined function each time a new data
 * block is received over the network.  
 * See libwww for a more complete documentation.
 *
 * Author: J. Kahan
 *
 */

#define THOT_EXPORT extern
#include "amaya.h"

/*
   **
   **              A H T    F W R I T E R   C O N V E R T E R   C L A S S
   **
 */
struct _HTStream
  {
     const HTStreamClass *isa;
     FILE               *fp;
     BOOL                leave_open;	  /* Close file when TtaFreeMemory? */
     HTRequest          *request;	  /* saved for callback       */
     HTRequestCallback  *callback;
  };

/***
  Static function prototypes 
***/

static int AHTFWriter_flush ( HTStream *me);
static int AHTFWriter_put_string ( HTStream *me, const char *s);
static int AHTFWriter_write ( HTStream *me, const char *s, int l);
static int AHTFWriter_abort (HTStream *me, HTList *e);

/*----------------------------------------------------------------------
  AHTFWriter_put_character
  ----------------------------------------------------------------------*/
static int AHTFWriter_put_character (HTStream *me, char c)
{
  int                 status;
  AHTReqContext      *reqcont;

  if (me->fp == NULL)
    {
#ifdef DEBUG_LIBWWW
     fprintf (stderr, "ERROR:fp is NULL in AHTFWriter_new\n");
#endif
     return HT_ERROR;
   }

  /* Don't write anything if the output is stdout (used for publishing */
  if (me->fp == stdout) 
    return HT_OK;
  status = (fputc (c, me->fp) == EOF) ? HT_ERROR : HT_OK;
  if (status == HT_OK)
    status = AHTFWriter_flush (me);

  reqcont = (AHTReqContext *) HTRequest_context (me->request);
  if (reqcont && reqcont->incremental_cbf)
    /* @@ JK: http_headers isn't initalized here yet */
    (*reqcont->incremental_cbf) (reqcont->docid, 1, reqcont->urlName,
				 reqcont->outputfile,  &(reqcont->http_headers),
				 &c, 1, reqcont->context_icbf);
   return status; 
}

/*----------------------------------------------------------------------
  AHTFWriter_put_string
  ----------------------------------------------------------------------*/
static int AHTFWriter_put_string (HTStream *me, const char* s)
{
  int                 status = HT_OK;
  AHTReqContext      *reqcont;


  if (me->fp == NULL)
    {
#ifdef DEBUG_LIBWWW
      fprintf (stderr, "ERROR:fp is NULL in AHTFWriter_new\n");
#endif
     return HT_ERROR;
    }

  /* Don't write anything if the output is stdout (used for publishing */
  if (me->fp == stdout) 
    return HT_OK;
  if (*s)
    {
      status = (fputs (s, me->fp) == EOF) ? HT_ERROR : HT_OK;
      if (status == HT_OK)
	status = AHTFWriter_flush (me);

      reqcont = (AHTReqContext *) HTRequest_context (me->request);
      if (reqcont && reqcont->incremental_cbf)
	/* @@ JK: http_headers isn't initalized here yet */
	(*reqcont->incremental_cbf) (reqcont->docid, 1, reqcont->urlName,
				     reqcont->outputfile, &(reqcont->http_headers),
				     s, strlen (s), reqcont->context_icbf);
    }
  else
    status = HT_ERROR;
  /* JK: Should there we a callback to incremental too? */
  return status;
}


/*----------------------------------------------------------------------
  AHTFWriter_write
  ----------------------------------------------------------------------*/
static int AHTFWriter_write (HTStream *me, const char *s, int l)
{
  int                 status;
  AHTReqContext      *reqcont;

  if (me->fp == NULL)
    {
#ifdef DEBUG_LIBWWW
      fprintf (stderr, "ERROR:fp is NULL in AHTFWriter_new\n");
#endif
      return HT_ERROR;
    }

  /* Don't write anything if the output is stdout (used for publishing */
  if (me->fp == stdout) 
    return HT_OK;
  status = (fwrite (s, 1, l, me->fp) != (unsigned int)l) ? HT_ERROR : HT_OK;
  if (l > 1 && status == HT_OK)
    (void) AHTFWriter_flush (me);

  reqcont = (AHTReqContext *) HTRequest_context (me->request);
   if (reqcont && reqcont->incremental_cbf)
     /* @@ JK: http_headers isn't initalized here yet */
     (*reqcont->incremental_cbf) (reqcont->docid, 1, reqcont->urlName,
				  reqcont->outputfile, &(reqcont->http_headers),
				  s, l, reqcont->context_icbf);
   return status;
}

/*----------------------------------------------------------------------
  AHTFWriter_flush
  ----------------------------------------------------------------------*/
static int AHTFWriter_flush (HTStream *me)
{
   if (me->fp == NULL)
     {
#ifdef DEBUG_LIBWWW
       fprintf (stderr, "ERROR:fp is NULL in AHTFWriter_new\n");
#endif
       return HT_OK;
     }

   /* Don't write anything if the output is stdout (used for publishing */
   if (me->fp == stdout) 
     return HT_OK;
   return (fflush (me->fp) == EOF) ? HT_ERROR : HT_OK;
}


/*----------------------------------------------------------------------
  AHTFWriter_FREE
  ----------------------------------------------------------------------*/
int AHTFWriter_FREE (HTStream *me)
{
  if (me)
    {
      if (me->leave_open != YES && me->fp != stdout)
        fclose (me->fp);
      HTRequest_setOutputStream (me->request, NULL);
      HT_FREE (me);
    }
  return HT_OK;
}

/*----------------------------------------------------------------------
  AHTFWriter_abort
  ----------------------------------------------------------------------*/
static int AHTFWriter_abort (HTStream *me, HTList *e)
{

  if (STREAM_TRACE)
    HTTrace ("FileWriter.. ABORTING...\n");
  if (me)
    {
      if (me->leave_open != YES && me->fp)
	fclose (me->fp);
      HTRequest_setOutputStream (me->request, NULL);
      HT_FREE (me);
    }
  return HT_ERROR;
}

/*      AHTFWriter class stream
 */
static const HTStreamClass AHTFWriter =	/* As opposed to print etc */
{
   "FileWriter",
   AHTFWriter_flush,
   AHTFWriter_FREE,
   AHTFWriter_abort,
   AHTFWriter_put_character,
   AHTFWriter_put_string,
   AHTFWriter_write
};


/*----------------------------------------------------------------------
  AHTFWriter_new
  ----------------------------------------------------------------------*/
HTStream *AHTFWriter_new (HTRequest *request, FILE *fp, BOOL leave_open)
{
   HTStream           *me = NULL;

   if (!fp)
     {
	if (STREAM_TRACE)
	   HTTrace ("FileWriter.. Bad file descriptor\n");
	return (HTStream *) HTErrorStream ();
     }

   if ((me = (HTStream *) HT_CALLOC (1, sizeof (HTStream))) == NULL)
     HT_OUTOFMEM ((char*)"HTFWriter_new");
   me->isa = &AHTFWriter;
#ifdef DEBUG_LIBWWW
   if (fp == NULL)
     fprintf (stderr, "ERROR:fp is NULL in AHTFWriter_new\n");
#endif
   me->fp = fp;
   me->leave_open = leave_open;
   me->request = request;
   return me;
}
