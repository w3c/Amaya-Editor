/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifndef __AMAYA_EVENT_H__
#define __AMAYA_EVENT_H__

/*
 * eventamaya.h : defines the structures and constants for the
 *      front end for the network requests from Amaya.
 */

typedef enum {
    AMAYA_NET_STATE_NONE,
    AMAYA_NET_STATE_QUEUED,
    AMAYA_NET_STATE_STARTED,
    AMAYA_NET_STATE_FAILED
} AmayaNetRequestStatus;

typedef enum {
    AMAYA_NET_TYPE_NONE,
    AMAYA_NET_TYPE_GET,
    AMAYA_NET_TYPE_PUT,
    AMAYA_NET_TYPE_FORM_POST,
    AMAYA_NET_TYPE_FORM_GET
} AmayaNetRequestType;

typedef enum {
    AMAYA_NET_MODE_NONE,
    AMAYA_NET_MODE_ASYNC,
    AMAYA_NET_MODE_IASYNC
} AmayaNetRequestMode;

typedef struct AmayaNetRequest {
    struct AmayaNetRequest     *next;   /* next request in queue */
    struct AmayaNetRequest     *prev;   /* prev request in queue */
    AmayaNetRequestStatus	state;  /* current state       */
    AmayaNetRequestType		type;	/* kind of request     */
    AmayaNetRequestMode		mode;	/* transfert mode      */
    AmayaNetCallback		callback;/* the callback       */
    AmayaNetCallbackBlock	blk;	/* the callback block  */
} AmayaNetRequest, *AmayaNetRequestPtr;

#endif /* __AMAYA_EVENT_H__ */
