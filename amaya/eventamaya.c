/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * eventamaya.c : front end for the network requests from Amaya.
 *      Request are document based, i.e. concerns a full URL
 *      transfert. This involves queuing incoming request, 
 *      especially the associated callback, moving request to
 *      active request lists, removing them once completed and
 *      executing the associated callback.
 */
