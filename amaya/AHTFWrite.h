
/*                                  Amaya Source Code

                  AHTFWRITE

*/

/*
**      (c) COPYRIGHT 
**      Please first read the full copyright statement in the file COPYRIGH.
*/

/*

This module provides 

This module is implemented by .c, and it is a part of the
AMAYA Source Code.

*/

#ifndef AHTFWRITE_H
#define AHTFWRITE_H

/*                  
   WRTING TO A FILE USING ANSI C

 */

#include "amaya.h"

#ifdef __STDC__
extern HTStream *AHTFWriter_new(HTRequest * request, FILE * fp,
                                  BOOL leave_open);
#else
extern HTStream *AHTFWriter_new(/*HTRequest * request, FILE * fp,
                                  BOOL leave_open*/);
#endif /* __STDC__ */


#endif /* AHFTWRITE_H */








