#ifndef AHTCOMMON_H
#define AHTCOMMON_H

/*
   ** Include Files
 */

#define NO 0
#define YES 1

/* Amaya */
#include "amaya.h"

typedef struct _AHTDocIdStatus
  {
     int                 docid;	/* a docid */
     int                 counter;	/* number of open requests associated with
					   docid */
  }
AHTDocId_Status;


typedef struct __AmayaContext
  {

     HTList             *reqlist;	/* List of current requests */
     HTList             *docid_status;	/* Status for each active docid */
     int                 open_requests;		/* number of open requests */

  }
AmayaContext;


/* The possible request modes */

#define AMAYA_SYNC      1	/*0x000001 */
#define AMAYA_ISYNC     2	/*0x000010 */
#define AMAYA_ASYNC     4	/*0x000100 */
#define AMAYA_IASYNC    8	/*0x001000 */
#define AMAYA_FORM_POST 16	/*0x010000 */
#define AMAYA_FORM_GET  32	/*0x100000 */


typedef enum _AHTReqStatus
  {
     HT_NEW = 0,
     HT_NEW_PENDING = 1,
     HT_WAITING = 2,
     HT_BUSY = 4,
     HT_END = 8,
     HT_ABORT = 16,
     HT_ERR = 32
  }
AHTReqStatus;

typedef void        TIcbf (void *request_context,
			   const char *data_block,
			   int data_block_size,
			   int request_status);

typedef void        TTcbf (void *request_context,
			   int request_status);


typedef struct _AHTReqContext
  {
     HTRequest          *request;	/* Pointer to the associated request object */
     HTParentAnchor     *anchor;

     HTMethod            method;	/* What method are we envoking */
     int                 docid;	/* docid to which this request belongs */

     AHTReqStatus        reqStatus;	/* status of the request */

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

     char               *outputfile;	/* file to receive incoming data */
     FILE               *output;	/* file pointer to outputfile */
     int                 mode;	/* Mode of request: SYNC/ASYNC/IASYNC */
     char               *urlName;	/* url to retrieve/or that was retrieved */

     TIcbf              *incremental_cbf;	/* For IASYNC mode, @ of callback function.
						   It'll be called each time a new data package
						   is received */
     void               *context_icbf;	/* Context for the above cbf */

     TTcbf              *terminate_cbf;		/* An optional CBF which will be invoked
						   after a file has been received */
     void               *context_tcbf;	/* Context for the above cbf */

     /* The following elements are used for the PUT and POST */

     HTParentAnchor     *dest;	/* Destination for PUT etc. */
     unsigned long       block_size;	/* size in bytes of the file to put */
     int                 put_counter;	/* number of bytes already put */
     char               *mem_ptr;	/* ptr to a struct in mem which 
					   contains a copy of the file to put
					 */

/* For debugging */

     int                *s;	/* socket number */
     char               *error_stream;
     int                 error_stream_size;
     BOOL                error_html;

  }
AHTReqContext;


/*
   ** Function prototypes
 */

#endif /*AHTCOMMON_H */
