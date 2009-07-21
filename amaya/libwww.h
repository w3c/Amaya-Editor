/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifndef AMAYA_LIBWWW_H
#define AMAYA_LIBWWW_H

/***********
Things to put in sysdep.h???
**********/
/*typedef u_long SockOps; */
#ifndef _WINSOCKAPI_
#define FD_READ         0x01
#define FD_WRITE        0x02
#define FD_OOB          0x04
#define FD_ACCEPT       0x08
#define FD_CONNECT      0x10
#define FD_CLOSE        0x20
#endif /* _WINSOCKAPI_ */
typedef unsigned long ms_t;
/************/

/* For windows plataforms, to guarantee that 
 * HT_DAV option for the libwww WebDAV module
 * will be defined.  */
#ifdef DAV
#ifndef HT_DAV
#define HT_DAV
#endif /* HT_DAV */
#endif /* DAV */

#include "wwwsys.h"
#include "WWWLib.h"
#include "AHTInit.h"
#include "WWWApp.h"
#include "WWWHTTP.h"
#include "HTReqMan.h"
#include "HTReq.h"
#include "HTAncMan.h"
#include "HTAccess.h"
#include "HTAlert.h"
#include "HTNetMan.h"
#include "HTBInit.h"
#include "WWWHTTP.h"		/* HTTP access module */
#include "HTProxy.h"
#include "HTHost.h"
#ifdef SSL
#include "WWWSSL.h"	
#endif /* SSL */

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
     int                 open_requests;	/* number of open requests */
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
     HT_CACHE = 32,
     HT_ERR = 64          /* an error happened during the request */
  }
AHTReqStatus;

typedef struct _AHTReqContext
  {
    HTRequest          *request;	/* Pointer to the associated request object     */
    HTParentAnchor     *anchor;
    HTMethod            method;	        /* What method are we invoking                  */
    int                 docid;	        /* docid to which this request belongs          */
    AHTReqStatus        reqStatus;	/* status of the request                        */
    HTEventType         read_type;	/* The type operation which must be used during
					** an Xt read callback */
    char               *default_put_name;  /* @@what's the def name? */
    ThotBool            put_redirection; /* a PUT operation was redirected */

    HTEventType         write_type;	/* The type operation which must be used during
					** an Xt write callback */
    
    HTEventType         except_type;	/* The type operation which must be used during
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
    
    char*            outputfile;	/* file to receive incoming data         */
    FILE               *output;	        /* file pointer to outputfile            */
    int                mode;	        /* Mode of request: SYNC/ASYNC/IASYNC/FORM POST/FORM GET   */
    char*            urlName;	/* url to retrieve/or that was retrieved */
    char             status_urlName [MAX_LENGTH]; /* url name to be displayed on the status bar */
    TIcbf              *incremental_cbf;	/* For IASYNC mode, @ of callback function */
    /* It'll be called each time a new data package */
    /* is received                                  */
    void               *context_icbf;	/* Context for the above cbf                  */
    TTcbf              *terminate_cbf;		/* optional CBF which will be invoked after  */
    /* a file has been received                  */
    void               *context_tcbf;	/* Context for the above cbf                 */
    
    /*                 the following structure holds a copy of the
		       HTTP headers that the application is interested in */
    AHTHeaders         http_headers;

    /* The following elements are used for the PUT and POST */
    HTAnchor           *source;
    HTAnchor           *dest;	/* Destination for PUT etc.              */
    unsigned long       block_size;	 /* size in bytes of the file to put      */
    int                 put_counter;	 /* number of bytes already put           */
    HTAssocList        *formdata;        /* ptr to a struct containing the formdata used with POST */

    char*               error_stream;      /* pointer to an error message associated with the
					      request */
    int                 error_stream_size; /* size of the above message */
    ThotBool            error_html;        /* If TRUE, means the applications wants to display
					      error_stream. If false, error_stream is not 
					      displayed at all */
    char               *document;

#ifdef DAV
    /* WebDAV request context object - it will be a AHTDAVContext object */
    /* MKP: I declared was a void * to avoid circular references between
     *      libwww.h and davlib.h */
    void      *dav_context;
#endif /* DAV */    
    char               *refdocUrl;        /* the doc number for the reference document */
  }
AHTReqContext;

/*
** the HTTP error messages
*/
/* an interpreted version of the error */
THOT_EXPORT char      AmayaLastHTTPErrorMsg [2*MAX_LENGTH];
THOT_EXPORT HTList      *conv;	/* List of global converters */
THOT_EXPORT AmayaContext *Amaya;	/* Amaya's request global context    */
#endif /* AMAYA_LIBWWW_H */

