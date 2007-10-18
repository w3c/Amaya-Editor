/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2005
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
#include "thot_gui.h"
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
void DisplayConfirmMessage (const char *text)
{
#ifdef _WINGUI
  int ndx;

  if ( COMPWnd)
    {		
      LPTSTR pText = (LPTSTR) malloc (strlen(text) + 3);
      if ( pText)
	{
	  /* Set caret to end of current text */
	  ndx = GetWindowTextLength (COMPWnd);
	  SetFocus (COMPWnd);   
	  SendMessage (COMPWnd, EM_SETSEL, (WPARAM)ndx, (LPARAM)ndx);
	  /* Append text */
	  sprintf (pText, "%s\r\n", text);
	  SendMessage (COMPWnd, EM_REPLACESEL, 0, (LPARAM) ((LPSTR) pText));
	  free( pText );
	}
    }
#else  /* _WINGUI */
   fprintf (stderr, text);
#endif /* _WINGUI */
}


/*----------------------------------------------------------------------
   DisplayMessage
   displays the given message (text).
  ----------------------------------------------------------------------*/
void DisplayMessage (char *text, int msgType)
{
#ifdef _WINGUI
  char          pText[MAX_TXT_LEN];
  int           ndx;

  if ( COMPWnd )
    {
      /* Set caret to end of current text */
      ndx = GetWindowTextLength (COMPWnd);
      SetFocus (COMPWnd);   
      SendMessage (COMPWnd, EM_SETSEL, (WPARAM)ndx, (LPARAM)ndx);
      /* Append text */
      sprintf (pText, "%s\r\n", text);
      SendMessage (COMPWnd, EM_REPLACESEL, 0, (LPARAM) ((LPTSTR) pText));
    }
#else  /* _WINGUI */
    fprintf (stderr, "%s\n", text);
#endif /* _WINGUI */
}

/*----------------------------------------------------------------------
   TtaError
   sets the error code
  ----------------------------------------------------------------------*/
void TtaError (int errorCode)
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
void CompilerMessage (int index, int origin, int level, int msgCode,
		      unsigned char *inputline, int lineNum)
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
void CompilerMessageString (int index, int origin, int level, int msgCode,
			    char *inputline, int lineNum, char *string)
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
