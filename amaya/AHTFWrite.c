/***
 *** Copyright (c) 1996 INRIA, All rights reserved
 ***/

/*                                                          HTFWrite.c
 *    FILE WRITER
 *
 *      (c) COPYRIGHT MIT 1995.
 *      Please first read the full copyright statement in the file COPYRIGH.
 *
 *      This version of the stream object just writes to a C file.
 *      The file is assumed open and left open.
 *
 *      Bugs:
 *              strings written must be less than buffer size.
 *
 *      History:
 *         HFN: wrote it
 *         HWL: converted the caching scheme to be hierachical by taking
 *              AL code from Deamon
 *         HFN: moved cache code to HTCache module
 *
 */

#include "amaya.h"

struct _HTStream
  {
     const HTStreamClass *isa;
     FILE               *fp;
     BOOL                leave_open;	/* Close file when HT_FREE? */
     char               *end_command;	/* Command to execute       */
     BOOL                remove_on_close;	/* Remove file?             */
     char               *filename;	/* Name of file             */
     HTRequest          *request;	/* saved for callback       */
     HTRequestCallback  *callback;
  };


/*----------------------------------------------------------------------
   SOCKET WRITER STREAM			     
  ----------------------------------------------------------------------*/

#ifdef __STDC__
PRIVATE int         AHTFWriter_flush (HTStream * me)
#else  /* __STDC__ */
PRIVATE int         AHTFWriter_flush (me)
HTStream           *me
#endif				/* __STDC__ */
{
   return (fflush (me->fp) == EOF) ? HT_ERROR : HT_OK;
}

#ifdef __STDC__
PRIVATE int         AHTFWriter_put_character (HTStream * me, char c)
#else				/* __STDC__ */
PRIVATE int         AHTFWriter_put_character (me, c)
Stream             *me;
char                c;

#endif /* __STDC__ */
{
   int                 status;
   AHTReqContext      *reqcont;

   reqcont = (AHTReqContext *) HTRequest_context (me->request);
   status = (fputc (c, me->fp) == EOF) ? HT_ERROR : HT_OK;

   if (status == HT_OK)
      status = AHTFWriter_flush (me);
   if (reqcont && reqcont->incremental_cbf)
      (*reqcont->incremental_cbf) (reqcont, &c, 1, status);
   return status;
}

#ifdef __STDC__
PRIVATE int         AHTFWriter_put_string (HTStream * me, const char *s)
#else  /* __STDC__ */
PRIVATE int         AHTFWriter_put_string (me, s)
HTStream           *me;
const char         *s;

#endif /* __STDC__ */
{
   int                 status;
   AHTReqContext      *reqcont;

   reqcont = (AHTReqContext *) HTRequest_context (me->request);
   if (*s)
     {
	status = (fputs (s, me->fp) == EOF) ? HT_ERROR : HT_OK;
	if (status == HT_OK)
	   status = AHTFWriter_flush (me);
     }

   if (reqcont && reqcont->incremental_cbf)
      (*reqcont->incremental_cbf) (reqcont, s, strlen (s), status);
   return status;
}


#ifdef __STDC__
PRIVATE int         AHTFWriter_write (HTStream * me, const char *s, int l)
#else  /* __STDC__ */
PRIVATE int         AHTFWriter_write (me, s, l)
HTStream           *me;
const char         *s;
int                 l;

#endif /* __STDC__ */
{
   int                 status;
   AHTReqContext      *reqcont;

   reqcont = (AHTReqContext *) HTRequest_context (me->request);
   status = (fwrite (s, 1, l, me->fp) != l) ? HT_ERROR : HT_OK;
   if (l > 1 && status == HT_OK)
      (void) AHTFWriter_flush (me);

   if (reqcont && reqcont->incremental_cbf)
      (*reqcont->incremental_cbf) (reqcont, s, l, status);

   return status;
}

#ifdef __STDC__
PRIVATE int         AHTFWriter_HT_FREE (HTStream * me)
#else  /* __STDC__ */
PRIVATE int         AHTFWriter_HT_FREE (me)
HTStream           *me;

#endif /* __STDC__ */
{
   if (me)
     {
	if (me->leave_open != YES)
	   fclose (me->fp);
#ifdef HAVE_SYSTEM
	if (me->end_command)
	   system (me->end_command);	/* SECURITY HOLE!!! */
#endif
	if (me->remove_on_close)
	   REMOVE (me->filename);
	if (me->callback)
	   (*me->callback) (me->request, me->filename);
	HT_FREE (me->end_command);
	HT_FREE (me->filename);
	HT_FREE (me);

	/* JK debugging */
	me = (HTStream *) NULL;

     }
   return HT_OK;
}

#ifdef __STDC__
PRIVATE int         AHTFWriter_abort (HTStream * me, HTList * e)
#else  /* __STDC__ */
PRIVATE int         AHTFWriter_abort (me, e)
HTStream           *me;
HTList             *e;

#endif /* __STDC__ */
{
   if (STREAM_TRACE)
      HTTrace ("FileWriter.. ABORTING...\n");
   if (me)
     {
	if (me->leave_open != YES)
	   fclose (me->fp);
	if (me->remove_on_close)
	   REMOVE (me->filename);
	HT_FREE (me->end_command);
	HT_FREE (me->filename);
	HT_FREE (me);

	/* JK debugging */
	me = (HTStream *) NULL;
     }
   return HT_ERROR;
}

PRIVATE const HTStreamClass AHTFWriter =	/* As opposed to print etc */
{
   "FileWriter",
   AHTFWriter_flush,
   AHTFWriter_HT_FREE,
   AHTFWriter_abort,
   AHTFWriter_put_character,
   AHTFWriter_put_string,
   AHTFWriter_write
};

#ifdef __STDC__
HTStream           *AHTFWriter_new (HTRequest * request, FILE * fp, BOOL leave_open)
#else  /* __STDC__ */
HTStream           *AHTFWriter_new (request, fp, leave_open)
HTRequest          *request;
FILE               *fp;
BOOL                leave_open;

#endif /* __STDC__ */
{
   HTStream           *me = NULL;

   if (!fp)
     {
	if (STREAM_TRACE)
	   HTTrace ("FileWriter.. Bad file descriptor\n");
	return (HTStream *) HTErrorStream ();
     }
   if ((me = (HTStream *) HT_CALLOC (1, sizeof (HTStream))) == NULL)
      HT_OUTOFMEM ("HTFWriter_new");
   me->isa = &AHTFWriter;
   me->fp = fp;
   me->leave_open = leave_open;
   me->request = request;
   return me;
}
