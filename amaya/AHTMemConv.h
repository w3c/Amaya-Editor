/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*                                  Amaya Source Code

   AHTFWRITE

 */
/*

   This module provides 

   This module is implemented by AHTFMemConv.c, and it is a part of the
   AMAYA Source Code.

 */

#ifndef AHTMEMCONVERT_H
#define AHTMEMCONVERT_H

/*                  

 */

#include "amaya.h"


#ifdef __STDC__
extern HTStream    *AHTMemConverter (HTRequest * request,
				     void *param,
				     HTFormat input_format,
				     HTFormat output_format,
				     HTStream * output_stream);

#else
extern HTStream    *AHTMemConverter (	/*HTRequest *     request,
					   void *          param,
					   HTFormat        input_format,
					   HTFormat        output_format,
					   HTStream *      output_stream */ );

#endif

#endif /* AHTMEMCONVERT_H  */
