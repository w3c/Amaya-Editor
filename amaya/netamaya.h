/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifndef __AMAYA_NET_H__
#define __AMAYA_NET_H__

/*
 * netamaya.h : defines the types and constants used by the Amaya
 *      application for Network requests.
 */

/*
 * An AmayaNetStatus indicate the result of a transfert.
 *   A completer !!!
 */

typedef enum {
    AMAYA_NET_STATUS_FAILED = -1,
    AMAYA_NET_STATUS_OK = 0
} AmayaNetStatus;

/*
 * An AmayaNetCallbackBlock is the block of information given
 * back in the net callback function once a request is completed.
 */

typedef struct AmayaNetCallbackBlock {
    int				doc;	/* associated document */
    char	     url[MAX_LENGTH];	/* the URL name    */
    char		       *outputfile;/* result file     */
    void		       *data;	/* data block      */
    size_t			size;	/* data block size */
} AmayaNetCallbackBlock, *AmayaNetCallbackBlockPtr;

/*
 * An AmayaNetCall is a function called by Amaya for starting a
 * network transfert.
 */

typedef void (*AmayaNetCall) (AmayaNetCallbackBlockPtr blk,
                                  int request_status);

/*
 * An AmayaNetCallback is the net callback function called
 * once a request is completed.
 */

typedef void (*AmayaNetCallback) (AmayaNetCallbackBlockPtr blk,
                                  int request_status);

#endif /* __AMAYA_NET_H__ */

