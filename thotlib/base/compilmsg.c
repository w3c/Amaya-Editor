/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * This module handles messages displayed by Thot compilers.
 *
 * Author: I. Vatton (INRIA)
 *         R. Guetari (W3C/INRIA): Windows & Unicode.
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
#include "edit_tv.h"


/*----------------------------------------------------------------------
   DisplayConfirmMessage
   displays the given message (text).
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DisplayConfirmMessage (STRING text)
#else  /* __STDC__ */
void                DisplayConfirmMessage (text)
STRING              text;

#endif /* __STDC__ */
{
#  ifdef _WINDOWS
	if ( COMPWnd )
	{		
		LPTSTR pText = (LPTSTR) malloc (ustrlen(text) + 3);
		if ( pText )
		{
			/* Set caret to end of current text */
			int ndx = GetWindowTextLength (COMPWnd);
			SetFocus (COMPWnd);   
			SendMessage (COMPWnd, EM_SETSEL, (WPARAM)ndx, (LPARAM)ndx);
			/* Append text */
			usprintf (pText, TEXT("%s\r\n"), text);
			SendMessage (COMPWnd, EM_REPLACESEL, 0, (LPARAM) ((LPSTR) pText));

			free( pText );
		}
	}
#  else  /* _WINDOWS */
   fprintf (stderr, text);
#  endif /* _WINDOWS */
}


/*----------------------------------------------------------------------
   DisplayMessage
   displays the given message (text).
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DisplayMessage (STRING text, int msgType)
#else  /* __STDC__ */
void                DisplayMessage (text, msgType)
STRING              text;
int                 msgType;

#endif /* __STDC__ */
{
#  ifdef _WINDOWS
	if ( COMPWnd ) {
       CHAR_T pText[MAX_TXT_LEN];
       /* Set caret to end of current text */
       int ndx = GetWindowTextLength (COMPWnd);
       SetFocus (COMPWnd);   
       SendMessage (COMPWnd, EM_SETSEL, (WPARAM)ndx, (LPARAM)ndx);
       /* Append text */
       usprintf (pText, TEXT("%s\r\n"), text);
       SendMessage (COMPWnd, EM_REPLACESEL, 0, (LPARAM) ((LPTSTR) pText));
	}
	
#   else  /* _WINDOWS */
    fprintf (stderr, text);
#   endif /* _WINDOWS */
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
void                CompilerMessage (int index, int origin, int level, int msgCode, USTRING inputline, int lineNum)
#else  /* __STDC__ */
void                CompilerMessage (index, origin, level, msgCode, inputline, lineNum)
int                 index;
int                 origin;
int                 level;
int                 msgCode;
USTRING             inputline;
int                 lineNum;

#endif /* __STDC__ */
{
   CHAR_T                buffer[LINE_LENGTH];
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
void                CompilerMessageString (int index, int origin, int level, int msgCode, STRING inputline, int lineNum, STRING string)
#else  /* __STDC__ */
void                CompilerMessageString (index, origin, level, msgCode, inputline, lineNum, string)
int                 index;
int                 origin;
int                 level;
int                 msgCode;
STRING              inputline;
int                 lineNum;
STRING              string;

#endif /* __STDC__ */

{
   CHAR_T                buffer[LINE_LENGTH];
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
