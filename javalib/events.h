/*
 * events.h : type structures for event handling when adding Timer and
 *            extra I/O channels to the basic multithreading core.
 */

#ifndef __JAVA_EVENT_H__
#define __JAVA_EVENT_H__

/*
 * An user block is a pointer to an opaque structure
 * containing whatever the user level program need.
 */

typedef void *userBlock;

/****************************************************************
 *								*
 *			Timers					*
 *								*
 ****************************************************************/

/*
 * A timer value is a struct timeval (at least on Unices).
 */

typedef struct timeval timerValue;

/*
 * A timer callback is basically a function with an userBlock
 * argument without return value
 */

typedef void (*timerCallback) (userBlock data);

/*
 * The basic structure associated to a timer is a time
 * limit and if the current time is greater the timer rings.
 * The timer also contains an user callback and an associated
 * user block.
 */

#define TIMER_UNLINKED		1
#define TIMER_FREE		2
#define TIMER_CLEAN		3
#define TIMER_ARMED		4

typedef struct timerBlock {
    int status;
    struct timerBlock *next;
    timerValue limit;
    timerCallback callback;
    userBlock data;
} timerBlock, *timerPtr;

/*
 * The public interfaces for the timers :
 *      createTimer : create a new timer block (uninitialized).
 *     destroyTimer : destroy a timer block, if needed the values
 *                    for the callback and data can be retrieved.
 *         setTimer : initialize the timer, hence allowing it to
 *                    trigger the callback code at the given time.
 *       unsetTimer : reset the timer.
 */

extern timerPtr createTimer(void);
extern void destroyTimer(timerPtr timer, timerCallback *callback,
                         userBlock *data);
extern void setTimer(timerPtr timer, timerValue *when, timerCallback callback,
                     userBlock data);
extern void unsetTimer(timerPtr timer);

/*
 * the internal interfaces to the timers :
 *        nextTimer : get the timerValue for the next timer to ring in the list.
 *       checkTimers: rings all the timers who have expired.
 */

extern timerValue *nextTimer(void);
extern void checkTimers(void);

/****************************************************************
 *								*
 *			I/O Channels				*
 *								*
 ****************************************************************/

/*
 * A channel value is an int representing the File Descriptor (at
 * least on Unices).
 */

typedef int channelValue;

/*
 * A channel callback is basically a function with a channel Value
 * and an userBlock arguments without return value
 */

typedef void (*channelCallback) (channelValue chan, userBlock data);

/*
 * A channel block contains the channel value, the direction,
 * the channel callback and the user data.
 */

#define INPUT_CHANNEL	1
#define OUTPUT_CHANNEL	2
#define FREE_CHANNEL	4

typedef struct channelBlock {
    struct channelBlock *next;
    channelValue chan;
    int direction;     /* input or output */
    channelCallback callback;
    userBlock data;
} channelBlock, *channelPtr;

/*
 * The public interfaces for the channels :
 *    registerInputChannel : register an input channel, defining the callback
 *                           and data blocks.
 *  unregisterInputChannel : remove it from the list.
 *   registerOutputChannel : register an output channel, defining the callback
 *                           and data blocks.
 * unregisterOutputChannel : remove it from the list.
 */

extern int registerInputChannel(channelValue chan, channelCallback callback,
                                 userBlock data);
extern int unregisterInputChannel(channelValue chan, channelCallback *callback,
                                   userBlock *data);
extern int registerOutputChannel(channelValue chan, channelCallback callback,
                                 userBlock data);
extern int unregisterOutputChannel(channelValue chan, channelCallback *callback,
                                   userBlock *data);

/*
 * the internal interfaces to the timers :
 *      createChannelMasks : create bitfield masks for select.
 *       checkChannelMasks : read bitfield masks from select and take the
 *			     appropriate actions.
 */

extern void createChannelMasks(int *n, fd_set *readfds, fd_set *writefds);
extern void checkChannelMasks(int n, fd_set *readfds, fd_set *writefds, int *nb);
#endif /* __JAVA_EVENT_H__ */
