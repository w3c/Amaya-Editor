/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
  
/*
 * AHTMemConv.c: writes a stream to a memory structure 
 * (Adapted from libwww's HTFWrite.c module). See libwww for a more
 * complete documentation.
 *
 * Author: J. Kahan
 *
 */

#define THOT_EXPORT extern
#include "amaya.h"

/*
   **
   **              A H T    M E M   C O N V E R T E R   C L A S S
   **
 */

struct _HTStream
  {
     const HTStreamClass *isa;
     HTRequest          *request;	/* saved for callback */
  };

#include "AHTMemConv_f.h"

static int AHTMemConv_put_character ( HTStream * me,
                                              char c );
static int AHTMemConv_put_string ( HTStream * me,
                                           const char *s );
static int AHTMemConv_write ( HTStream * me,
                                      const char *s,
                                      int l );
static int AHTMemConv_flush ( HTStream * me );
static int AHTMemConv_TtaFreeMemory ( HTStream * me );
static int AHTMemConv_abort ( HTStream * me,
                                      HTList * e );

/*----------------------------------------------------------------------
  AHTMemConv_put_character
  ----------------------------------------------------------------------*/
static int         AHTMemConv_put_character (HTStream * me, char c)
{
   char                tmp[2];
   AHTReqContext      *reqcont;

   if (WWWTRACE)
      HTTrace ("AHTMemConv_put_character %c\n", c);

   reqcont = (AHTReqContext *) HTRequest_context (me->request);

   tmp[0] = c;
   tmp[1] = EOS;
   StrAllocCat (reqcont->error_stream, tmp);
   reqcont->error_stream_size += 1;
   return HT_OK;
}

/*----------------------------------------------------------------------
  AHTMemConv_put_string
  ----------------------------------------------------------------------*/
static int         AHTMemConv_put_string (HTStream * me, const char* s)
{
   AHTReqContext      *reqcont;

   reqcont = (AHTReqContext *) HTRequest_context (me->request);
   StrAllocCat (reqcont->error_stream, s);
   reqcont->error_stream_size += strlen (s);
   if (WWWTRACE)
      HTTrace ("AHTMemConv_put_string %s\n", s);
   return HT_OK;
}

/*----------------------------------------------------------------------
  AHTMemConv_write
  ----------------------------------------------------------------------*/
static int         AHTMemConv_write (HTStream * me, const char *s, int l)
{
   AHTReqContext      *reqcont;
   char               *tmp;
   int                 i;

   reqcont = (AHTReqContext *) HTRequest_context (me->request);

   tmp = (char *)TtaGetMemory (l + 1);

   if (!tmp)
     {
	fprintf (stderr, "AHTMemConv_write: out of memory\n");
	return HT_ERROR;
     }

   for (i = 0; i < l; i++)
      tmp[i] = s[i];
   tmp[i] = EOS;
   StrAllocCat (reqcont->error_stream, tmp);
   TtaFreeMemory (tmp);
   reqcont->error_stream_size += l;

   if (WWWTRACE)
      HTTrace ("AHTMemConv_write, l=%d bytes\n", l);
   return HT_OK;
}

/*----------------------------------------------------------------------
  AHTMemConv_flush
  ----------------------------------------------------------------------*/
static int         AHTMemConv_flush (HTStream * me)
{
   if (WWWTRACE)
      HTTrace ("AHTMemConv_flush\n");

   return HT_OK;
}


/*----------------------------------------------------------------------
  AHTMemConv_put_TtaFreeMemory
  ----------------------------------------------------------------------*/
static int         AHTMemConv_TtaFreeMemory (HTStream * me)
{
   if (WWWTRACE)
      HTTrace ("AHTMemConv_free\n");
   HT_FREE (me);
   return HT_OK;
}

/*----------------------------------------------------------------------
  AHTMemConv_abort
  ----------------------------------------------------------------------*/
static int         AHTMemConv_abort (HTStream * me, HTList * e)
{
   if (WWWTRACE)
      HTTrace ("AHTMemConv_abort\n");
   AHTMemConv_TtaFreeMemory (me);
   return HT_ERROR;
}


/*      AHTResponseClass stream
 */

static const HTStreamClass AHTResponseClass =
{
   "AHTResponse",
   AHTMemConv_flush,
   AHTMemConv_TtaFreeMemory,
   AHTMemConv_abort,
   AHTMemConv_put_character,
   AHTMemConv_put_string,
   AHTMemConv_write
};

/*----------------------------------------------------------------------
  AHTMemConv_new
  ----------------------------------------------------------------------*/
HTStream           *AHTMemConv_new (HTRequest * request)
{
   HTStream           *me;
   AHTReqContext      *reqcont;

   if ((me = (HTStream *) HT_CALLOC (1, sizeof (HTStream))) == NULL)
      HT_OUTOFMEM ((char*)"HTXConvert");

   me->isa = &AHTResponseClass;
   me->request = request;

   reqcont = (AHTReqContext *) HTRequest_context (request);

   if (reqcont && reqcont->error_stream)
     {
	HT_FREE (reqcont->error_stream);
	reqcont->error_stream = NULL;
	reqcont->error_stream_size = 0;
     }
   if (STREAM_TRACE)
      HTTrace ("AHTMemConv... Created\n");
   return me;
}

/*----------------------------------------------------------------------
  AHTMemConverter
  ----------------------------------------------------------------------*/
HTStream           *AHTMemConverter (HTRequest * request, void *param, HTFormat input_format, HTFormat output_format, HTStream * output_stream)
{
   return AHTMemConv_new (request);
}
