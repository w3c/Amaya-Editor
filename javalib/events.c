/*
 * events.c : code for event handling when adding Timer and
 *            extra I/O channels to the basic multithreading core.
 *
 * Daniel Veillard : 1997
 *
 *  Compile with -DSTANDALONE to generate a standalone test executable.
 */

#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <malloc.h>
#include "events.h"

#include "events_f.h"

#ifdef STANDALONE
#define DEBUG_TIMERS
#define DEBUG_TIMERS_LISTS
#define DEBUG_CHANNELS
#define DEBUG_CHANNELS_LISTS
#endif

/* DEBUG_TIMERS : enable TIMER normal debugging */
/* DEBUG_TIMERS_LISTS : enable TIMER full debugging */
/* DEBUG_CHANNELS : enable CHANNEL normal debugging */
/* DEBUG_CHANNELS_LISTS : enable CHANNEL full debugging */

/****************************************************************
 *								*
 *			Timers					*
 *								*
 ****************************************************************/

static timerPtr timerActiveQueue = NULL; /* waiting for alarm */
static timerPtr timerCleanQueue = NULL;  /* no alarm defined  */
static timerPtr timerFreeQueue = NULL;   /* unallocated queue */

static timerValue currentTime;           /* approx value      */

#define TIMER_LT(t1, t2) (((t1).tv_sec < (t2).tv_sec) ||	\
    (((t1).tv_sec == (t2).tv_sec) && ((t1).tv_usec < (t2).tv_usec)))

#define TIMER_NORMALIZE(t)					\
    if ((t).tv_usec >= 1000000) {				\
        (t).tv_sec += (t).tv_usec / 1000000;			\
	(t).tv_usec %= 1000000;					\
    }

#define GET_CURRENT_TIME() gettimeofday(&currentTime, NULL)

/*
 * printTimer: print the current status of a timer.
 */

static void printTimer(timerPtr timer) {
    switch (timer->status) {
        case TIMER_UNLINKED:
	    printf("unlinked ,"); break;
        case TIMER_FREE:
	    printf("free ,"); break;
        case TIMER_CLEAN:
	    printf("desactivated ,"); break;
        case TIMER_ARMED:
	    printf("armed ,"); break;
	default :
	    printf("invalid ,"); break;
    }
    if ((timer->limit.tv_sec == 0) &&
        (timer->limit.tv_sec == 0))
	printf("0x%X : %d.%d, disabled\n", (unsigned) timer,
	       timer->limit.tv_sec, timer->limit.tv_usec);
    else if (TIMER_LT(timer->limit, currentTime)) 
	printf("0x%X : %d.%d, expired\n", (unsigned) timer,
	       timer->limit.tv_sec, timer->limit.tv_usec);
    else
	printf("0x%X : %d.%d\n", (unsigned) timer,
	       timer->limit.tv_sec, timer->limit.tv_usec);
}

/*
 * printTimers: print a timer list
 */

static void printTimers(timerPtr timer) {
    while (timer != NULL) {
        printTimer(timer);
	timer = timer->next;
    }
}

/*
 * showTimers: show the full current state of timers
 */

void showTimers(void) {
    timerPtr freed = timerFreeQueue;
    int nbFreed = 0;

    GET_CURRENT_TIME();
    printf("TIMERS : at %d.%d\n", currentTime.tv_sec, currentTime.tv_usec);
    printf("  ENABLED TIMERS:\n");
    printTimers(timerActiveQueue);
    printf("  DISABLED TIMERS:\n");
    printTimers(timerCleanQueue);

    while (freed != NULL) {
        nbFreed++;
	freed = freed->next;
    }
    printf(" %d FREE TIMERS\n", nbFreed);
}

/*
 * unlinkTimer : remove a timer from it's list.
 *               the list arg can be specified, if known
 *
 * Would benefit from a doubly linked list structure !!!
 */
static void unlinkTimer(timerPtr timer) {
    timerPtr prev, *list;

    if (timer == NULL) return;
    switch (timer->status) {
        case TIMER_FREE:
	    list = &timerFreeQueue; break;
        case TIMER_CLEAN:
	    list = &timerCleanQueue; break;
        case TIMER_ARMED:
	    list = &timerActiveQueue; break;
        case TIMER_UNLINKED:
	default:
	    list = NULL; break;
    }
    if (list == NULL) {
        if (timer == timerActiveQueue) {
	    timerActiveQueue = timer->next;
	    goto found;
	}
	prev = timerActiveQueue;
	while (prev != NULL) {
	    if (prev->next == timer) {
		prev->next = timer->next;
		goto found;
	    }
	    prev = prev->next;
	}
        if (timer == timerCleanQueue) {
	    timerCleanQueue = timer->next;
	    goto found;
	}
	prev = timerCleanQueue;
	while (prev != NULL) {
	    if (prev->next == timer) {
		prev->next = timer->next;
		goto found;
	    }
	    prev = prev->next;
	}
        if (timer == timerFreeQueue) {
	    timerFreeQueue = timer->next;
	    goto found;
	}
	prev = timerFreeQueue;
	while (prev != NULL) {
	    if (prev->next == timer) {
		prev->next = timer->next;
		goto found;
	    }
	    prev = prev->next;
	}
    } else {
        if (*list == timer) {
	    *list = timer->next;
	    goto found;
	} else {
	    prev = *list;
	    while (prev != NULL) {
	        if (prev->next == timer) {
		    prev->next = timer->next;
		    goto found;
		}
		prev = prev->next;
	    }
	}
    }
#ifdef DEBUG_TIMERS
    fprintf(stderr,"unlinkTimer : not found\n");
#endif
    return;

found :
    timer->next = NULL;
    timer->status = TIMER_UNLINKED;
}

/*
 * createTimer : allocate a new timer block (or pick it from the free list).
 *               Install it in the timerCleanQueue and returns it.
 */
timerPtr createTimer(void) {
    timerPtr ret = NULL;

#ifdef DEBUG_TIMERS
    printf("\ncreateTimer : ");
#endif

    GET_CURRENT_TIME();
    if (timerFreeQueue == NULL) {
        ret = (timerPtr) malloc(sizeof(timerBlock));
	ret->next = NULL;
    } else {
        ret = timerFreeQueue;
	timerFreeQueue = ret->next;
	ret->next = NULL;
    }
    ret->limit.tv_sec = 0;
    ret->limit.tv_usec = 0;
    ret->callback = NULL;
    ret->data = NULL;
    ret->status = TIMER_CLEAN;
    ret->next = timerCleanQueue;
    timerCleanQueue = ret;

#ifdef DEBUG_TIMERS
    printTimer(ret);
#endif
#ifdef DEBUG_TIMERS_LISTS
    showTimers();
#endif
    return(ret);
}

/*
 * destroyTimer : remove a given timer from it's queue and install it in the
 *                timerCleanQueue
 */
void destroyTimer(timerPtr timer, timerCallback *callback,
                         userBlock *data) {
    if (timer == NULL) {
        fprintf(stderr, "destroyTimer(NULL, ...) !\n");
	return;
    }

#ifdef DEBUG_TIMERS
    printf("\ndestroyTimer : ");
    printTimer(timer);
#endif

    timer->limit.tv_sec = 0;
    timer->limit.tv_usec = 0;
    if (callback != NULL) *callback = timer->callback;
    if (data != NULL) *data = timer->data;
    timer->callback = NULL;
    timer->data = NULL;
    unlinkTimer(timer);
    timer->status = TIMER_FREE;
    timer->next = timerFreeQueue;
    timerFreeQueue = timer;

#ifdef DEBUG_TIMERS_LISTS
    showTimers();
#endif
}

/*
 * setTimer : initialize a timer and move it to the timerActiveQueue.
 */
void setTimer(timerPtr timer, timerValue *when, timerCallback callback,
                     userBlock data) {
    timerPtr prev;

    if (timer == NULL) {
        fprintf(stderr, "setTimer(NULL, ...) !\n");
	return;
    }

#ifdef DEBUG_TIMERS
    printf("\nsetTimer : ");
    printTimer(timer);
#endif

    unlinkTimer(timer);

    timer->limit.tv_sec = when->tv_sec;
    timer->limit.tv_usec = when->tv_usec;
    timer->callback = callback;
    timer->data = data;
    timer->status = TIMER_ARMED;

    TIMER_NORMALIZE(timer->limit);

    /*
     * insert in increasing value order.
     */
    if (timerActiveQueue == NULL) {
	timer->next = NULL;
	timerActiveQueue = timer;
    } else {
        if (TIMER_LT(timer->limit, timerActiveQueue->limit)) {
	    timer->next = timerActiveQueue;
	    timerActiveQueue = timer;
	} else {
	    prev = timerActiveQueue;
	    while ((prev->next != NULL) &&
	           (TIMER_LT(prev->next->limit, timer->limit)))
		   prev = prev->next;
            timer->next = prev->next;
	    prev->next = timer;
	}
    }

#ifdef DEBUG_TIMERS_LISTS
    showTimers();
#endif
}

/*
 * setTimer : reset a timer and move it to the timerCleanQueue.
 */
void unsetTimer(timerPtr timer) {
    unlinkTimer(timer);

    timer->limit.tv_sec = 0;
    timer->limit.tv_usec = 0;

    timer->status = TIMER_CLEAN;
    timer->next = timerCleanQueue;
    timerCleanQueue = timer;
}

/*
 * nextTimer : get the timer delay for the next timer to ring in the list.
 *
 * Doesn't seems thread safe, void nextTimer(timerValue *time) better ?
 */

timerValue *nextTimer(void) {
    static timerValue nextTimeout;

    if (timerActiveQueue == NULL) return(NULL);
    GET_CURRENT_TIME();
    nextTimeout.tv_sec = timerActiveQueue->limit.tv_sec;
    nextTimeout.tv_usec = timerActiveQueue->limit.tv_usec;
    nextTimeout.tv_sec -= currentTime.tv_sec;
    if (nextTimeout.tv_usec < currentTime.tv_usec) {
        nextTimeout.tv_sec--;
	nextTimeout.tv_usec += 1000000;
    }
    nextTimeout.tv_usec -= currentTime.tv_usec;
    if (nextTimeout.tv_sec < 0) return(NULL);
    if (nextTimeout.tv_usec < 0) return(NULL);
    return(&nextTimeout);
}

/*
 * checkTimers: rings all the timers who have expired.
 */
void checkTimers(void) {
    timerPtr timer = timerActiveQueue;

    GET_CURRENT_TIME();
    while (timer != NULL) {
        if (TIMER_LT(timer->limit, currentTime)) {
	    /*
	     * this timer has expired, remove it from the list,
	     * and call the callback.
	     */
	    timerActiveQueue = timer->next;
	    timer->status = TIMER_UNLINKED;

#ifdef DEBUG_TIMERS
            printf("\nRinging timer : ");
	    printTimer(timer);
#endif
            if (timer->callback) {
	        timer->callback(timer->data);
	    }

	    /*
	     * Take care, the timer may have been rearmed or destoyed
	     * by the callback ! Otherwise add it to the clean queue.
	     */
	    if (timer->status == TIMER_UNLINKED) {
		timer->status = TIMER_CLEAN;
	        timer->next = timerCleanQueue;
	        timerCleanQueue = timer;
	    }
	} else
	    break; /* Nice to have them sorted ! */
	timer = timerActiveQueue;
    }

#ifdef DEBUG_TIMERS_LISTS
    showTimers();
#endif
}

/****************************************************************
 *								*
 *			I/O Channels				*
 *								*
 ****************************************************************/

static channelPtr inputChannelList = NULL;
static channelPtr outputChannelList = NULL;
static channelPtr freeChannelList = NULL;

/*
 * printChannel : prints the content of a channel.
 */
static void printChannel(channelPtr channel) {
    if (channel == NULL)  {
        printf("null\n");
	return;
    }
    printf("%d, ", channel->chan);
    switch (channel->direction) {
        case INPUT_CHANNEL : 
	    printf("incoming "); break;
        case OUTPUT_CHANNEL : 
	    printf("outcoming "); break;
        case FREE_CHANNEL : 
	    printf("freed "); break;
        default :
	    printf("invalid "); break;
    }

    /*
     * gathering statistics on the # of incoming and outcoming
     * packets seems a good idea ...
     */
    printf("\n");
}

/*
 * printChannels : prints the content of a channel list.
 */
static void printChannels(channelPtr list) {
    while (list != NULL) {
        printChannel(list);
	list = list->next;
    }
}

/*
 * showChannels : display the current status of all channels.
 */
void showChannels(void) {
    channelPtr freed = freeChannelList;
    int nbFreed = 0;

    printf("INPUT CHANNELS :\n");
    printChannels(inputChannelList);
    printf("OUTPUT CHANNELS :\n");
    printChannels(outputChannelList);

    while (freed != NULL) {
        nbFreed++;
	freed = freed->next;
    }
    printf(" %d FREE CHANNELS\n", nbFreed);
}

/*
 * searchChannel : search a channel in a list.
 */

static channelPtr searchChannel(int chan, channelPtr list) {
    channelPtr ret = list;

    while ((ret != NULL) && (ret->chan != chan)) ret = ret->next;
    return(ret);
}

/*
 * unlinkChannel : unlink a channel from a list, returns the block pointer.
 */

static channelPtr unlinkChannel(int chan, channelPtr *list) {
    channelPtr ret, prev;

    if (*list == NULL) {
        printf("unlinkChannel %d : not found !\n", chan);
	return(NULL);
    }
    if ((*list)->chan == chan) {
        ret = *list;
	*list = ret->next;
	ret->next = NULL;
	return(ret);
    }

    prev = *list;
    while (prev->next != NULL) {
	if (prev->next->chan == chan) {
	    ret = prev->next;
	    prev->next = ret->next;
	    ret->next = NULL;
	    return(ret);
	}
	/* if (prev->next->chan > chan) break; Increasing order */
        prev = prev->next;
    }

#ifdef DEBUG_CHANNELS
    printf("unlinkChannel %d : not found !\n", chan);
#endif
    return(NULL);
}

/*
 * registerInputChannel : allocate a new channel block (or pick it from the
 *                        free list). Initialize it and add it to the
 *                        inputChannelList, if not already present.
 */
int registerInputChannel(channelValue chan, channelCallback callback,
                          userBlock data) {
    channelPtr ret = NULL, prev;
    int found = 0;

#ifdef DEBUG_CHANNELS
    printf("\nregisterInputChannel : ");
#endif

    if ((chan < 0) || (chan >= 1024)) {
        printf("registerInputChannel : invalid channel #%d\n", chan);
	return(0);
    }

    ret = searchChannel(chan, inputChannelList);
    if (ret == NULL) {
	if (freeChannelList == NULL) {
	    ret = (channelPtr) malloc(sizeof(channelBlock));
	    ret->next = NULL;
	} else {
	    ret = freeChannelList;
	    freeChannelList = ret->next;
	    ret->next = NULL;
	}
    } else
        found = 1;

    ret->chan = chan;
    ret->direction = INPUT_CHANNEL;
    ret->callback = callback;
    ret->data = data;

    if (!found) {
	/*
	 * insert in increasing value order of chan.
	 */
	if (inputChannelList == NULL) {
	    ret->next = NULL;
	    inputChannelList = ret;
	} else {
	    if (ret->chan < inputChannelList->chan) {
		ret->next = inputChannelList;
		inputChannelList = ret;
	    } else {
		prev = inputChannelList;
		while ((prev->next != NULL) &&
		       (prev->next->chan < ret->chan))
		       prev = prev->next;
		ret->next = prev->next;
		prev->next = ret;
	    }
	}
    }

#ifdef DEBUG_CHANNELS
    printChannel(ret);
#endif
#ifdef DEBUG_CHANNELS_LISTS
    showChannels();
#endif

    return(1);
}

/*
 * unregisterInputChannel : search and clean an existing block, and
 *                          add it to the freeChannelList.
 */
int unregisterInputChannel(channelValue chan, channelCallback *callback,
                            userBlock *data) {
    channelPtr channel;

#ifdef DEBUG_CHANNELS
    printf("\nunregisterInputChannel : ");
#endif

    if ((chan < 0) || (chan >= 1024)) {
        printf("unregisterInputChannel : invalid channel #%d\n", chan);
        if (callback != NULL) *callback = NULL;
        if (data != NULL) *data = NULL;
	return(0);
    }

    channel = unlinkChannel(chan, &inputChannelList);
    if (channel == NULL) {
        if (callback != NULL) *callback = NULL;
        if (data != NULL) *data = NULL;
	return(0);
    }

#ifdef DEBUG_CHANNELS
    printChannel(channel);
#endif

    if (callback != NULL) *callback = channel->callback;
    if (data != NULL) *data = channel->data;
    channel->direction = FREE_CHANNEL;
    channel->next = freeChannelList;
    freeChannelList = channel;

#ifdef DEBUG_CHANNELS_LISTS
    showChannels();
#endif

    return(1);
}

/*
 * registerOutputChannel : allocate a new channel block (or pick it from the
 *                         free list). Initialize it and add it to the
 *                         outputChannelList.
 */
int registerOutputChannel(channelValue chan, channelCallback callback,
                           userBlock data) {
    channelPtr ret = NULL, prev;
    int found = 0;

#ifdef DEBUG_CHANNELS
    printf("\nregisterOutputChannel : ");
#endif

    if ((chan < 0) || (chan >= 1024)) {
        printf("registerOutputChannel : invalid channel #%d\n", chan);
	return(0);
    }

    ret = searchChannel(chan, outputChannelList);
    if (ret == NULL) {
	if (freeChannelList == NULL) {
	    ret = (channelPtr) malloc(sizeof(channelBlock));
	    ret->next = NULL;
	} else {
	    ret = freeChannelList;
	    freeChannelList = ret->next;
	    ret->next = NULL;
	}
    } else
        found = 1;

    ret->chan = chan;
    ret->direction = INPUT_CHANNEL;
    ret->callback = callback;
    ret->data = data;

    if (!found) {
	/*
	 * insert in increasing value order of chan.
	 */
	if (outputChannelList == NULL) {
	    ret->next = NULL;
	    outputChannelList = ret;
	} else {
	    if (ret->chan < outputChannelList->chan) {
		ret->next = outputChannelList;
		outputChannelList = ret;
	    } else {
		prev = outputChannelList;
		while ((prev->next != NULL) &&
		       (prev->next->chan < ret->chan))
		       prev = prev->next;
		ret->next = prev->next;
		prev->next = ret;
	    }
	}
    }

#ifdef DEBUG_CHANNELS
    printChannel(ret);
#endif
#ifdef DEBUG_CHANNELS_LISTS
    showChannels();
#endif

    return(1);
}

/*
 * unregisterOutputChannel : search and clean an existing block, and
 *                           add it to the freeChannelList.
 */
int unregisterOutputChannel(channelValue chan, channelCallback *callback,
                             userBlock *data) {
    channelPtr channel;

#ifdef DEBUG_CHANNELS
    printf("\nunregisterOutputChannel : ");
#endif

    if ((chan < 0) || (chan >= 1024)) {
        printf("unregisterOutputChannel : invalid channel #%d\n", chan);
        if (callback != NULL) *callback = NULL;
        if (data != NULL) *data = NULL;
	return(0);
    }

    channel = unlinkChannel(chan, &outputChannelList);
    if (channel == NULL) {
        if (callback != NULL) *callback = NULL;
        if (data != NULL) *data = NULL;
	return(0);
    }

#ifdef DEBUG_CHANNELS
    printChannel(channel);
#endif

    if (callback != NULL) *callback = channel->callback;
    if (data != NULL) *data = channel->data;
    channel->direction = FREE_CHANNEL;
    channel->next = freeChannelList;
    freeChannelList = channel;

#ifdef DEBUG_CHANNELS_LISTS
    showChannels();
#endif

    return(1);
}

/***
 *  The two next functions are especially made to help integrate with
 *  select(2) system call.
 *
 *  SELECT(2)           Linux Programmer's Manual           SELECT(2)
 *
 *  NAME
 *	   select,  FD_CLR,  FD_ISSET,  FD_SET, FD_ZERO - synchronous
 *	   I/O multiplexing
 *
 *  SYNOPSIS
 *	   #include <sys/time.h>
 *	   #include <sys/types.h>
 *	   #include <unistd.h>
 *
 *	   int  select(int  n,  fd_set  *readfds,  fd_set  *writefds,
 *	   fd_set *exceptfds, struct timeval *timeout);
 *
 *	   FD_CLR(int fd, fd_set *set);
 *	   FD_ISSET(int fd, fd_set *set);
 *	   FD_SET(int fd, fd_set *set);
 *	   FD_ZERO(fd_set *set);
 *
 */

/*
 * createChannelMasks : create bitfield masks for select.
 *                      both input and output channel lists are scanned
 *                      and the readfds and writefds are updated accordingly.
 *   n is incremented to represent the highest index in one of the
 *   bitfields.
 */
void createChannelMasks(int *n, fd_set *readfds, fd_set *writefds) {
    channelPtr channel;

    if (readfds != NULL) {
        channel = inputChannelList;
        while (channel != NULL) {
	    FD_SET(channel->chan, readfds);
	    if (channel->chan >= *n) *n = channel->chan + 1;
	    channel = channel->next;
        }
    }
    if (writefds != NULL) {
        channel = outputChannelList;
        while (channel != NULL) {
	    FD_SET(channel->chan, writefds);
	    if (channel->chan >= *n) *n = channel->chan + 1;
	    channel = channel->next;
        }
    }
}

/*
 * checkChannelMasks : check bitfield masks from select.
 *                     both input and output channel lists are scanned
 *                     if the corresponding bits are sets in the masks
 *                     the callback is called. nb values is decremented
 *                     and the bit is reset to zero in the mask.
 *   n is the highest index in one of the bitfields.
 *   nb is the number of bits sets.
 */
void checkChannelMasks(int n, fd_set *readfds, fd_set *writefds, int *nb) {
    channelPtr channel, next;

    if (*nb <= 0) return;
    if (readfds != NULL) {
        channel = inputChannelList;
        while (channel != NULL) {
	    next = channel->next; /* in case the callback does unregister */

	    if (channel->chan > n) break; /* sorted by increasing order */
	    if (FD_ISSET(channel->chan, readfds)) {
	        FD_CLR(channel->chan, readfds);

#ifdef DEBUG_CHANNELS
                printf("Read I/O on fd %d\n", channel->chan);
#endif

	        /*
		 * An I/O is ready for this channel.
		 */
		(*nb)--;
		if (channel->callback != NULL)
		    channel->callback(channel->chan, channel->data);
		if (*nb <= 0) return;
	    }
	    channel = next;
        }
    }
    if (writefds != NULL) {
        channel = outputChannelList;
        while (channel != NULL) {
	    next = channel->next; /* in case the callback does unregister */

	    if (channel->chan > n) break; /* sorted by increasing order */
	    if (FD_ISSET(channel->chan, writefds)) {
	        FD_CLR(channel->chan, writefds);

#ifdef DEBUG_CHANNELS
                printf("Write I/O on fd %d\n", channel->chan);
#endif

	        /*
		 * An I/O is ready for this channel.
		 */
		(*nb)--;
		if (channel->callback != NULL)
		    channel->callback(channel->chan, channel->data);
		if (*nb <= 0) return;
	    }
	    channel = next;
        }
    }
}

#ifdef STANDALONE

/****************************************************************
 *								*
 *		Standalone Testing Code				*
 *								*
 ****************************************************************/

void print_fds(int n, fd_set *fds) {
    int i;

    for (i = 0;i < n;i++) {
        if (FD_ISSET(i, fds)) printf("1");
	else printf("0");
    }
    printf(" ");
}

void message(char *msg) {
    printf(msg);
}

void do_io(int chan, char *msg) {
    printf(msg, chan);
}

int main(int argc, char **argv) {
    timerPtr timer3;
    timerPtr timer1;
    timerPtr timer4;
    timerValue limit;
    fd_set read_fds;
    fd_set write_fds;
    int n = 2;
    int r = 6;
    
    printf("#\n# Timers Tests\n#\n");
    gettimeofday(&limit, NULL);

    printf("Creating timers\n\n");
    timer3 = createTimer();
    limit.tv_sec += 3;
    setTimer(timer3, &limit, (timerCallback) message,
           "\n *** Timer 3 expired : OK ***\n\n");
    timer1 = createTimer();
    limit.tv_sec -= 2;
    setTimer(timer1, &limit, (timerCallback) message,
           "\n *** Timer 1 expired : OK ***\n\n");
    timer4 = createTimer();
    limit.tv_sec += 3;
    setTimer(timer4, &limit, (timerCallback) message,
           "\n *** Timer 4 expired : OK ***\n\n");
    
    while (timerActiveQueue != NULL) {
         printf("Sleeping\n\n");
         sleep(1);

         printf("Checking timers\n\n");
	 checkTimers();
    }

    destroyTimer(timer1, NULL, NULL);
    destroyTimer(timer3, NULL, NULL);
    destroyTimer(timer4, NULL, NULL);

    printf("#\n# Timers Tests finished\n#\n");
    printf("#\n# Channels Tests\n#\n");

    registerInputChannel(3, (channelCallback) do_io,
           "\n *** Simulated read on channel %d : Ok ***\n\n");
    registerInputChannel(5, (channelCallback) do_io,
           "\n *** Simulated read on channel %d : Ok ***\n\n");
    registerOutputChannel(5, (channelCallback) do_io,
           "\n *** Simulated write on channel %d : Ok ***\n\n");
    registerOutputChannel(4, (channelCallback) do_io,
           "\n *** Simulated write on channel %d : Ok ***\n\n");

    /*
     * test for bmap filling/scanning.
     */
    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);
    FD_SET(0, &read_fds);
    FD_SET(1, &write_fds);
    printf("Initial I/O bitmaps : ");
    print_fds(n, &read_fds);
    print_fds(n, &write_fds);
    printf("\n");

    createChannelMasks(&n, &read_fds, &write_fds);
    printf("I/O bitmaps after createChannelMasks : ");
    print_fds(n, &read_fds);
    print_fds(n, &write_fds);
    printf("\n");

    printf("\nCalling checkChannelMasks\n");
    r = 6;
    checkChannelMasks(n, &read_fds, &write_fds, &r);

    printf("4 I/O should have been scheduled, and 2 remaining : %d\n", r);
    printf("I/O bitmaps after checkChannelMasks : ");
    print_fds(n, &read_fds);
    print_fds(n, &write_fds);
    printf("\n");

    unregisterInputChannel(5, NULL, NULL);
    unregisterInputChannel(3, NULL, NULL);
    unregisterOutputChannel(5, NULL, NULL);
    unregisterOutputChannel(4, NULL, NULL);

    printf("#\n# Channels Tests finished\n#\n");
    return(0);
}

#endif /* STANDALONE */

