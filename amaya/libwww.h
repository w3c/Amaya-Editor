/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifndef AMAYA_LIBWWW_H
#define AMAYA_LIBWWW_H

#include "WWWLib.h"
#include "WWWApp.h"
#include "WWWHTTP.h"
#include "WWWInit.h"
#include "HTReqMan.h"
#include "HTReq.h"
#include "HTAncMan.h"
#include "HTAccess.h"
#include "HTEvntrg.h"
#include "HTAlert.h"
#include "HTNetMan.h"
#include "HTBInit.h"
#include "WWWHTTP.h"		/* HTTP access module */
#include "HTProxy.h"

typedef struct _AHTDocIdStatus
  {
     int                 docid;	/* a docid */
     int                 counter;	/* number of open requests associated with dicid */
  }
AHTDocId_Status;


typedef struct __AmayaContext
  {
     HTList             *reqlist;	/* List of current requests */
     HTList             *docid_status;	/* Status for each active docid */
     int                 open_requests;		/* number of open requests */
  }
AmayaContext;

/* the possible states for a request */

typedef enum _AHTReqStatus
  {
     HT_NEW = 0,          /* new request */
     HT_NEW_PENDING = 1,  /* new request, waiting for a socket */
     HT_WAITING = 2,      /* active request, waiting for socket events */
     HT_BUSY = 4,         /* the request is currently being processed */
     HT_END = 8,          /* the request has ended */
     HT_ABORT = 16,       /* user aborted the request */
     HT_ERR = 32          /* an error happened during the request */
  }
AHTReqStatus;

#ifdef _WINDOWS
typedef int XtInputId;
#endif 

/* The structure used for requests */

typedef void        TIcbf (void *request_context, const char *data_block, int data_block_size, int request_status);

typedef void        TTcbf (void *request_context, int request_status);

typedef struct _AHTReqContext
  {
     HTRequest          *request;	/* Pointer to the associated request object     */
     HTParentAnchor     *anchor;
     HTMethod            method;	/* What method are we envoking                  */
     int                 docid;	        /* docid to which this request belongs          */
     AHTReqStatus        reqStatus;	/* status of the request                        */
     SockOps             read_ops;	/* The ops operation which must be used during
					   ** an Xt read callback */

     SockOps             write_ops;	/* The ops operation which must be used during
					   ** an Xt write callback */

     SockOps             except_ops;	/* The ops operation which must be used during
					   ** an Xt exception callback */

#ifdef WWW_XWINDOWS
     XtInputId           read_xtinput_id;	/* The read xt event id assocciated with
						   the request */
     XtInputId           write_xtinput_id;	/* The write xt event id assocciated with
						   the request */
     XtInputId           except_xtinput_id;	/* The except xt event id assocciated with
						   the request */
#endif				/* WWW_XWINDOWS */

    /*** Experimental ****/
     SOCKET             read_sock;              /* read socket associated with the request */
     SOCKET             write_sock;             /* write socket associated with the request */
     SOCKET             except_sock;            /* except socket associated with the request */
    /*** End of experimental stuff ****/

     char               *outputfile;	/* file to receive incoming data         */
     FILE               *output;	/* file pointer to outputfile            */
     int                 mode;	/* Mode of request: SYNC/ASYNC/IASYNC/FORM POST/FORM GET   */
     char               *urlName;	/* url to retrieve/or that was retrieved */
     char               status_urlName [MAX_LENGTH]; /* url name to be displayed on the status bar */
     TIcbf              *incremental_cbf;	/* For IASYNC mode, @ of callback function */
     /* It'll be called each time a new data package */
     /* is received                                  */
     void               *context_icbf;	/* Context for the above cbf                  */
     TTcbf              *terminate_cbf;		/* optional CBF which will be invoked after  */
     /* a file has been received                  */
     void               *context_tcbf;	/* Context for the above cbf                 */

     /* The following elements are used for the PUT and POST */

     HTParentAnchor     *dest;	/* Destination for PUT etc.              */
     unsigned long       block_size;	/* size in bytes of the file to put      */
     int                 put_counter;	/* number of bytes already put           */
     char               *mem_ptr;	/* ptr to a struct in mem which contains a copy */
     /* of the file to put                           */
     char               *error_stream;        /* pointer to an error message associated with the
						 request */
     int                 error_stream_size;   /* size of the above message */
     boolean                error_html;       /* If TRUE, means the applications wants to display
						 error_stream. If false, error_stream is not 
						 displayed at all */
  }
AHTReqContext;

THOT_EXPORT char AmayaLastHTTPErrorMsg [];
THOT_EXPORT HTList      *conv;	/* List of global converters */
THOT_EXPORT AmayaContext *Amaya;	/* Amaya's request global context    */

#endif /* AMAYA_LIBWWW_H */
