/*                                                                 Public QUERY
   OBJECT PUBLIC
   PUBLIC DECLARATION OF QUERY MODULE

 */
/*
   **  Copyright (c) 1994-1995 Inria/CNRS  All rights reserved. 
   **      Please first read the full copyright statement in the file COPYRIGH.
 */
/*
   Module comment
 */
#ifndef QUERY_H
#define QUERY_H



/* AHTLibWWW includes */

#include "AHTCommon.h"

/**** Global variables ****/

extern HTList      *conv;	/* List of global converters */

extern AmayaContext *Amaya;	/* Amaya's global context */


/**** Function prototypes ****/

#ifdef __STDC__
extern void         QueryInit ();

#else
extern void         QueryInit ();

#endif
/*__STDC__*/
