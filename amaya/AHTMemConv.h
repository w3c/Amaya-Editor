
/*                                  Amaya Source Code

   AHTFWRITE

 */

/*
   **      (c) COPYRIGHT 
   **      Please first read the full copyright statement in the file COPYRIGH.
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
