/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * eventloop.c : contains the core of the event loop for amaya.
 *      This contols all the asynchronous handling for fetching
 *      documents using the libWWW as well as the X-Windows 
 *      event loop.
 *      This module contains all the routines needing access to
 *      the automaton contol variables, especially the libWWW
 *      callback for Progress, Terminate, Redirect and Authentification.
 */

