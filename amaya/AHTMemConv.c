/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
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

#ifndef AMAYA_JAVA

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

#ifdef __STDC__
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
#else 
static int AHTMemConv_put_character (/* HTStream * me,
                                                char c */);
static int AHTMemConv_put_string (/* HTStream * me,
                                             const char *s */);
static int AHTMemConv_write (/* HTStream * me,
                                        const char *s,
                                        int l */);
static int AHTMemConv_flush (/* HTStream * me */);
static int AHTMemConv_TtaFreeMemory (/* HTStream * me */);
static int AHTMemConv_abort (/* HTStream * me,
                                       HTList * e */);
#endif

/*----------------------------------------------------------------------
  AHTMemConv_put_character
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int         AHTMemConv_put_character (HTStream * me, char c)
#else  /* __STDC__ */
static int         AHTMemConv_put_character (me, c)
HTStream           *me;
char                c;

#endif /* __STDC__ */
{
   char*              tmp = "";
   AHTReqContext      *reqcont;

   if (WWWTRACE)
      HTTrace ("AHTMemConv_put_character %c\n", c);

   reqcont = (AHTReqContext *) HTRequest_context (me->request);

   tmp[0] = c;
   StrAllocCat (reqcont->error_stream, tmp);
   reqcont->error_stream_size += 1;
   return HT_OK;
}

/*----------------------------------------------------------------------
  AHTMemConv_put_string
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int         AHTMemConv_put_string (HTStream * me, const char* s)
#else  /* __STDC__ */
static int         AHTMemConv_put_string (me, s)
HTStream           *me;
const char*        s;

#endif /* __STDC__ */
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
#ifdef __STDC__
static int         AHTMemConv_write (HTStream * me, const char *s, int l)
#else  /* __STDC__ */
static int         AHTMemConv_write (me, s, l)
HTStream           *me;
const char         *s;
int                 l;

#endif /* __STDC__ */
{
   AHTReqContext      *reqcont;
   char               *tmp;
   int                 i;

   reqcont = (AHTReqContext *) HTRequest_context (me->request);

   tmp = TtaGetMemory (l + 1);

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
#ifdef __STDC__
static int         AHTMemConv_flush (HTStream * me)
#else  /* __STDC__ */
static int         AHTMemConv_flush (me)
HTStream           *me;

#endif /* __STDC__ */
{
   if (WWWTRACE)
      HTTrace ("AHTMemConv_flush\n");

   return HT_OK;
}


/*----------------------------------------------------------------------
  AHTMemConv_put_TtaFreeMemory
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int         AHTMemConv_TtaFreeMemory (HTStream * me)
#else  /* __STDC__ */
static int         AHTMemConv_TtaFreeMemory (me)
HTStream           *me;

#endif /* __STDC__ */
{
   if (WWWTRACE)
      HTTrace ("AHTMemConv_free\n");
   HT_FREE (me);
   return HT_OK;
}

/*----------------------------------------------------------------------
  AHTMemConv_abort
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int         AHTMemConv_abort (HTStream * me, HTList * e)
#else  /* __STDC__ */
static int         AHTMemConv_abort (me, e)
HTStream           *me;
HTList             *e;

#endif /* __STDC__ */
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
#ifdef __STDC__
HTStream           *AHTMemConv_new (HTRequest * request)
#else  /* __STDC__ */
HTStream           *AHTMemConv_new (request)
HTRequest          *request;

#endif /* __STDC__ */
{
   HTStream           *me;
   AHTReqContext      *reqcont;

   if ((me = (HTStream *) HT_CALLOC (1, sizeof (HTStream))) == NULL)
      HT_OUTOFMEM ("HTXConvert");

   me->isa = &AHTResponseClass;
   me->request = request;

   reqcont = (AHTReqContext *) HTRequest_context (request);

   if (reqcont->error_stream)
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
#ifdef __STDC__
HTStream           *AHTMemConverter (HTRequest * request, void *param, HTFormat input_format, HTFormat output_format, HTStream * output_stream)
#else  /* __STDC__ */
HTStream           *AHTMemConverter (request, param, input_format, output_format, output_stream)
HTRequest          *request;
void               *param;
HTFormat            input_format;
HTFormat            output_format;
HTStream           *output_stream;

#endif /* __STDC__ */
{
   return AHTMemConv_new (request);
}


/*
  End of Module AHTMemConv.c
*/
#endif /* ! AMAYA_JAVA */



