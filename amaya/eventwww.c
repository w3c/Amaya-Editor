/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * eventwww.c : front end to the libWWW access.
 *      This involves keeeping track of active request,
 *      and dispatching socket events.
 *      This involves queuing new request, the associated callback,
 *      removing them once completed and interfacing with the
 *      eventloop module concerning socket allocation.
 */
