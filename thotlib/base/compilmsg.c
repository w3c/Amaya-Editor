/*
 *
 *  (c) COPYRIGHT INRIA, Grif, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * This module handles messages displayed by Thot compilers.
 *
 * Author: I. Vatton (INRIA)
 *
 */

#include "thot_sys.h"
#include "compilmsg.h"
#include "message.h"
#include "constgrm.h"
#include "typegrm.h"
#include "constmedia.h"
#include "typemedia.h"

#define THOT_EXPORT extern
#include "compil_tv.h"

int                 UserErrorCode;

/*----------------------------------------------------------------------
   DisplayConfirmMessage
   displays the given message (text).
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DisplayConfirmMessage (char *text)
#else  /* __STDC__ */
void                DisplayConfirmMessage (text)
char               *text;

#endif /* __STDC__ */
{
   fprintf (stderr, text);
}


/*----------------------------------------------------------------------
   DisplayMessage
   displays the given message (text).
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DisplayMessage (char *text, int msgType)
#else  /* __STDC__ */
void                DisplayMessage (text, msgType)
char               *text;
int                 msgType;

#endif /* __STDC__ */
{
   fprintf (stderr, text);
}

/*----------------------------------------------------------------------
   TtaError
   sets the error code
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaError (int errorCode)
#else  /* __STDC__ */
void                TtaError (errorCode)
int                 errorCode;

#endif /* __STDC__ */
{
   UserErrorCode = errorCode;
}


/*----------------------------------------------------------------------
   CompilerMessage
   displays an simple error message for a compiler.

   inputline: the imput line containing the error
   lineNum: SynInteger of that imput line in the source file.
   index: rank of first erroneous character in the input line.
   origin: the compiler that raises the error.
   level: severity level
   msgCode: SynInteger of the message to be displayed
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                CompilerMessage (int index, int origin, int level, int msgCode, unsigned char *inputline, int lineNum)
#else  /* __STDC__ */
void                CompilerMessage (index, origin, level, msgCode, inputline, lineNum)
int                 index;
int                 origin;
int                 level;
int                 msgCode;
unsigned char      *inputline;
int                 lineNum;

#endif /* __STDC__ */
{
   char                buffer[LINE_LENGTH];
   int                 i;

   TtaDisplayMessage (INFO, TtaGetMessage (COMPIL, ERR_LINE), lineNum);
   if (index != 0)
     {
	for (i = 0; i < index - 1; buffer[i++] = ' ') ;
	buffer[index - 1] = '*';
	buffer[index] = EOS;
	TtaDisplayMessage (INFO, TtaGetMessage (COMPIL, COMPIL_STRING), inputline, buffer);
     }
   TtaDisplaySimpleMessage (level, origin, msgCode);
   error = True;
}


/*----------------------------------------------------------------------
   CompilerMessageString
   displays an error message with a variable part for a compiler.

   inputline: the imput line containing the error
   lineNum: SynInteger of that imput line in the source file.
   index: rank of first erroneous character in the input line.
   origin: the compiler that raises the error.
   level: severity level
   msgCode: SynInteger of the message to be displayed
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                CompilerMessageString (int index, int origin, int level, int msgCode, char *inputline, int lineNum, char *string)
#else  /* __STDC__ */
void                CompilerMessageString (index, origin, level, msgCode, inputline, lineNum, string)
int                 index;
int                 origin;
int                 level;
int                 msgCode;
char               *inputline;
int                 lineNum;
char               *string;

#endif /* __STDC__ */

{
   char                buffer[LINE_LENGTH];
   int                 i;

   TtaDisplayMessage (INFO, TtaGetMessage (COMPIL, ERR_LINE), lineNum);
   if (index != 0)
     {
	for (i = 0; i < index - 1; buffer[i++] = ' ') ;
	buffer[index - 1] = '*';
	buffer[index] = EOS;
	TtaDisplayMessage (INFO, TtaGetMessage (COMPIL, COMPIL_STRING), inputline, buffer);
     }
   TtaDisplayMessage (level, TtaGetMessage (origin, msgCode), string);
   error = True;
}
