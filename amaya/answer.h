
/*                                  Amaya Source Code

   answer.h

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

#ifndef ANSWER_H
#define ANSWER_H

/*                  
   WRTING TO A FILE USING ANSI C

 */

/*      AHTError_MemPrint (hacked from HTError_print)
   **      -------------
 */
#ifdef __STDC__
extern void         AHTError_MemPrint (HTRequest * request);

#else  /* __STDC__ */
extern void         AHTERROR_MemPrint ( /*HTRequest *request */ );

#endif /* __STDC__ */

#endif /* ANSWER_H */
