/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * WIndows Dialogue API routines for Amaya
 *
 * Author: R. Guetari (W3C/INRIA)
 *
 */

#ifdef _WINDOWS

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <string.h>
#include <stdio.h>
#include "fileaccess.h"
#include "message.h"
#include "registry.h"
#include "resource.h"
#include "thot_gui.h"
#include "thot_sys.h"
#include "dialog.h"
#include "application.h"
#include "interface.h"
#include "document.h"
#include "view.h"



#define OPEN    100
#define COMPILE 101
#define QUIT    102

#define CTRL_O   15

#define NEW_LINE '\n'
#define CR         13

#define ERROR_CMD          0
#define APP                1
#define PRS                2
#define STR                3
#define TRA                4
#define SRC_DIR            5
#define DEST_DIR           6

#define NUMLINES        5000
#define FATAL_EXIT_CODE   33
#define COMP_WARNING       1
#define COMP_SUCCESS       0

#define ID_TOOLBAR         1

HWND  hWND = (HWND) 0;
HWND  StatusBar;
HWND  hWndToolBar;

int   Y = 10;
char* cmdLine;
char* SrcPath;
char* DestPath;
char* WorkPath;
char* ThotPath;
char* currentFile;
char* currentDestFile;
char* BinFiles [100];
char* TbStrings [2] = {"Open (Ctrl+O)", "Build (F7)"};

DWORD       dwStatusBarStyles = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | CCS_BOTTOM | SBARS_SIZEGRIP;

TBBUTTON tbButtons[] = {
	{0, OPEN,    TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
	{1, COMPILE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
};

TOOLINFO tbToolInfo;

#ifdef __STDC__
LRESULT CALLBACK CompilersWndProc (HWND, UINT, WPARAM, LPARAM);
#else  /* !__STDC__ */
LRESULT CALLBACK CompilersWndProc ();
#endif /* __STDC__ */

static OPENFILENAME OpenFileName;
static STRING       szFilter = "Amaya Makefiles (*.mkf)\0*.mkf\0All files (*.*)\0*.*\0";
static CHAR_T       szFileName[256];
static char         fileToOpen [256];
static int          iVscrollPos = 0, iVscrollMax, iVscrollInc; 
static HINSTANCE    g_hInstance;

#ifdef __STDC__
static void   CopyToolTipText (LPTOOLTIPTEXT lpttt)
#else  /* __STDC__ */
static void   CopyToolTipText (lpttt)
LPTOOLTIPTEXT lpttt;
#endif /* __STDC__ */
{
   int        iButton = lpttt->hdr.idFrom;
   char*      pString;
   char*      pDest = lpttt->lpszText;

   pString = TbStrings[iButton - OPEN];
   ustrcpy (pDest, pString);
}

#ifdef __STDC__
void MakeMessage (HWND hwnd, char* errorMsg, int msgType)
#else  /* !__STDC__ */
void MakeMessage (hwnd, errorMsg, msgType)
HWND  hwnd;
char* errorMsg;
int   msgType;
#endif /* __STDC__ */
{
   if (hwnd) {
      LPSTR pText = (LPSTR) malloc (strlen (errorMsg) + 3);
      if ( pText ) {
         /* Set caret to end of current text */
         int ndx = GetWindowTextLength (hwnd);
         SetFocus (hwnd);   
         SendMessage (hwnd, EM_SETSEL, (WPARAM)ndx, (LPARAM)ndx);
         /* Append text */
         sprintf( pText, "%s\r\n", errorMsg );
         SendMessage (hwnd, EM_REPLACESEL, 0, (LPARAM) ((LPSTR) pText));

         free( pText );			
	  } 
   } 
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int Copy_File (HWND hwnd, const char* src, const char* dest)
#else  /* __STDC__ */
int Copy_File (hwnd, src, dest)
HWND        hwnd;
const char* src; 
const char* dest;
#endif /* __STDC__ */
{
    FILE* srcFile;
    FILE* destFile;
    char  errorMsg [800];
	int   c;

    if ((srcFile = fopen (src, "r")) == NULL) {
       sprintf (errorMsg, "Error: Can't open file: %s", src);
       MakeMessage (hwnd, errorMsg, FATAL_EXIT_CODE);
       return FATAL_EXIT_CODE;
	}
    if ((destFile = fopen (dest, "w")) == NULL) {
       sprintf (errorMsg, "Error: Can't open file: %s", dest);
       MakeMessage (hwnd, errorMsg, FATAL_EXIT_CODE);
       return FATAL_EXIT_CODE;
	}

    while ((c = getc (srcFile)) != EOF)
          putc (c, destFile);

    fclose (srcFile);
    fclose (destFile);

	return 0;
}

#ifdef __STDC__
int Move_File (HWND hwnd, const char* src, const char* dest)
#else  /* __STDC__ */
int Move_File (hwnd, src, dest)
HWND        hwnd;
const char* src; 
const char* dest;
#endif /* __STDC__ */
{
	int ret;

    ret = Copy_File (hwnd, src, dest);
     _unlink (src);
	 return (ret);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int makeArgcArgv (HINSTANCE hInst, char*** pArgv, char* commandLine)
#else  /* __STDC__ */
int makeArgcArgv (hInst, pArgv, commandLine)
HINSTANCE hInst; 
char***   pArgv; 
char*     commandLine;
#endif /* __STDC__ */
{
    int          argc;
    static char* argv[20];
    static char  argv0[256];
    char*        ptr     = commandLine;
    char         lookFor = 0;

    enum {
         nowAt_start, 
         nowAt_text
    } nowAt;

    *pArgv = argv;
    argc   = 0;
    GetModuleFileName (hInst, argv0, sizeof (argv0));
    argv[argc++] = argv0;
    for (nowAt = nowAt_start;;) {
        if (!*ptr) 
           return (argc);
	
        if (lookFor) {
           if (*ptr == lookFor) {
	      nowAt = nowAt_start;
	      lookFor = 0;
	      *ptr = 0;   /* remove the quote */
	   } else if (nowAt == nowAt_start) {
	        argv[argc++] = ptr;
                nowAt = nowAt_text;
	   }
	   ptr++;
	   continue;
        }
        if (*ptr == ' ' || *ptr == '\t') {
           *ptr = 0;
	   ptr++;
	   nowAt = nowAt_start;
	   continue;
        }
        if ((*ptr == '\'' || *ptr == '\"' || *ptr == '`') && nowAt == nowAt_start) {
           lookFor = *ptr;
	   nowAt = nowAt_start;
	   ptr++;
	   continue;
        }
        if (nowAt == nowAt_start) {
           argv[argc++] = ptr;
	   nowAt = nowAt_text;
        }
        ptr++;
    }
}

#ifdef __STDC__
int Makefile (HWND hwnd, char* fileName)
#else  /* !__STDC__ */
int Makefile (hwnd, fileName)
HWND  hwnd;
char* fileName;
#endif /* __STDC__ */
{
    FILE* f; 
    HANDLE hLib;
    FARPROC ptrMainProc;
    char  msg [1024];
    char  seps[]   = " \t=$()\n\r";
	char  string [1024];
    char* args [100];
	char* token;
    char* pChar;
    char* ptr;
    char* SrcFileName = NULL;
    char* WorkFileName = NULL;
    char* currentFileName;
	int   command;
    int   index, i;
    int   indexBinFiles = 0;
    int   len;
    int   line = 1;
    int   result = COMP_SUCCESS;

    if (hwnd) {
       if ((f = fopen (fileName, "r")) == NULL) {
          sprintf (msg, "Cannot open file %s", fileToOpen);
          MessageBox (hwnd, msg, "Make error", MB_OK | MB_ICONWARNING);
	   } else {
              while (!feof (f)) {
                    if (currentFile) {
                       free (currentFile);
                       currentFile = (char*) 0;
					} 
                    if (currentDestFile) {
                       free (currentDestFile);
                       currentDestFile = (char*) 0;
					} 

                    /* Establish string and get the first token: */
                    string [0] = 0;
                    fgets (string, 1024, f);
					/*
                    len = strlen (string);
                    if (string[len - 2] == 13)
                       string [len - 2] = 0;
                    else if (string[len - 1] == '\n')
                         string[len - 1] = 0;
						 */
                    pChar = &string [0];
                    while (*pChar == SPACE || *pChar == TAB || *pChar == CR)
                          pChar++;
                    if (*pChar != NEW_LINE && *pChar != EOS && *pChar != CR) {
                       token = strtok (string, seps);
                       if (token != NULL) {
                          if (!strcmp (token, "APP"))
                             command = APP;
                          else if (!strcmp (token, "PRS"))
                               command = PRS;
                          else if (!strcmp (token, "STR"))
                               command = STR;
                          else if (!strcmp (token, "TRA"))
                               command = TRA;
                          else if (!strcmp (token, "SRC_DIR"))
                               command = SRC_DIR;
                          else if (!strcmp (token, "DEST_DIR"))
                               command = DEST_DIR;
                          else {
                               command = ERROR_CMD;
                               sprintf (msg, "%s (Line %3d): unknown command %s", fileToOpen, line, token);
                               MakeMessage (hwnd, msg, FATAL_EXIT_CODE);				   
						  } 
					   }
                       if (command == ERROR_CMD)
                          return FATAL_EXIT_CODE;

                       /* @@@@@ if (command != ERROR_CMD) { @@@@@ */
                          index = 0;
                          args [index] = (char*) malloc (strlen (cmdLine) + 1);
                          strcpy (args [index++], cmdLine);
                          while ((token = strtok (NULL, seps)) != NULL) {
                                /* While there are tokens in "string" */
                                ptr = strrchr(token, '.');
                                if (ptr || (token [0] != '-')) {
                                   if (!currentFile) {
                                      currentFile = (char*) malloc (strlen (token) + 1);
                                      strcpy (currentFile, token);
								   } else {
                                          currentDestFile = (char*) malloc (strlen (token) + 1);
                                          strcpy (currentDestFile, token);
								   }
								} 
                                args [index] = (char*) malloc (strlen (token) + 1);
                                strcpy (args [index++], token);
                                /* Get next token: */
                                /* token = strtok (NULL, seps); */
						  }
                          if (SrcFileName) {
							  free (SrcFileName);
							  SrcFileName = NULL;
						  }
						  if (WorkFileName) {
                              free (WorkFileName);
							  WorkFileName = NULL;
						  } 
                          switch (command) {
                                 case SRC_DIR: 
                                      if (SrcPath) {
                                         free (SrcPath);
                                         SrcPath = (char*) 0;
									  } 
                                      if (!strcmp (args [1], "THOTDIR")) {
                                         if (index > 2) {
                                            SrcPath = (char*) malloc (strlen (ThotPath) + strlen (args [2]) + 1);
                                            strcpy (SrcPath, ThotPath);
                                            strcat (SrcPath, args [2]);
										 } else {
                                                SrcPath = (char*) malloc (strlen (ThotPath) + 1);
                                                strcpy (SrcPath, ThotPath);
										 }
									  }
                                      for (i = 0; i < index; i++) {
                                          free (args [i]);
                                          args [i] = (char*) 0;
									  }
                                      break;

                                 case DEST_DIR: 
                                      if (DestPath) {
                                         free (DestPath);
                                         DestPath = (char*) 0;
									  } 
                                      if (!strcmp (args [1], "THOTDIR")) {
                                         if (index > 2) {
                                            DestPath = (char*) malloc (strlen (ThotPath) + strlen (args [2]) + 1);
                                            strcpy (DestPath, ThotPath);
                                            strcat (DestPath, args [2]);
										 } else {
                                                DestPath = (char*) malloc (strlen (ThotPath) + 1);
                                                strcpy (DestPath, ThotPath);
										 }
									  }
                                      for (i = 0; i < index; i++) {
                                          free (args [i]);
                                          args [i] = (char*) 0;
									  }
                                      break;

                                 case APP: 
                                      hLib = LoadLibrary ("app");
                                      if (!hLib)
                                         return FATAL_EXIT_CODE;
                                      ptrMainProc = GetProcAddress (hLib, "APPmain");
                                      if (!ptrMainProc) {
                                         FreeLibrary (hLib);
                                         return FATAL_EXIT_CODE;
									  }

                                      ptr = strrchr(currentFile, '.');
                                      if (ptr) {
                                         len = strlen (SrcPath);
                                         if (len > 0 && SrcPath [len - 1] == '\\') {
                                            SrcFileName = (char*) malloc (len + strlen (currentFile) + 1);
                                            sprintf (SrcFileName, "%s%s", SrcPath, currentFile);
										 } else {
                                                SrcFileName = (char*) malloc (len + strlen (currentFile) + 2);
                                                sprintf (SrcFileName, "%s\\%s", SrcPath, currentFile);
										 }
                                         len = strlen (WorkPath);
                                         if (len > 0 && WorkPath [len - 1] == '\\') {
                                            WorkFileName = (char*) malloc (len + strlen (currentFile) + 1);
                                            sprintf (WorkFileName, "%s%s", WorkPath, currentFile);
										 } else {
                                                WorkFileName = (char*) malloc (len + strlen (currentFile) + 2);
                                                sprintf (WorkFileName, "%s\\%s", WorkPath, currentFile);
										 } 
									  } else {
                                             len = strlen (SrcPath);
                                             if (len > 0 && SrcPath [len - 1] == '\\') {
                                                SrcFileName = (char*) malloc (len + strlen (currentFile) + 3);
                                                sprintf (SrcFileName, "%s%s.A", SrcPath, currentFile);
											 } else {
                                                    SrcFileName = (char*) malloc (len + strlen (currentFile) + 3);
                                                    sprintf (SrcFileName, "%s\\%s.A", SrcPath, currentFile);
											 }
                                             len = strlen (WorkPath);
                                             if (len > 0 && WorkPath [len - 1] == '\\') {
                                                WorkFileName = (char*) malloc (len + strlen (currentFile) + 3);
                                                sprintf (WorkFileName, "%s%s.A", WorkPath, currentFile);
											 } else {
                                                    WorkFileName = (char*) malloc (len + strlen (currentFile) + 4);
                                                    sprintf (WorkFileName, "%s\\%s.A", WorkPath, currentFile);
											 }  
									  } 

                                      if ((result = Copy_File (hwnd, SrcFileName, WorkFileName)) != FATAL_EXIT_CODE) {
                                         result = ptrMainProc (hwnd, StatusBar, index, args, &Y);
                                         FreeLibrary (hLib);
                                         for (i = 0; i < index; i++) {
                                             free (args [i]);
                                             args [i] = (char*) 0;
										 }
                                         if (currentFile) {
                                            free (currentFile);
                                            currentFile = (char*) 0;
										 }
                                         if (SrcFileName) {
                                            free (SrcFileName);
                                            SrcFileName = (char*) 0;
										 }
                                         if (WorkFileName) {
                                            _unlink (WorkFileName);
                                            free (WorkFileName);
                                            WorkFileName = (char*) 0;
										 }
									  }
                                      if (result == FATAL_EXIT_CODE)
                                         return result;
                                      break;

                                 case PRS: 
                                      hLib = LoadLibrary ("prs");
                                      if (!hLib)
                                         return FATAL_EXIT_CODE;
                                      ptrMainProc = GetProcAddress (hLib, "PRSmain");
                                      if (!ptrMainProc) {
                                         FreeLibrary (hLib);
                                         return FATAL_EXIT_CODE;
									  }

                                      ptr = strrchr(currentFile, '.');
                                      if (ptr) {
                                         len = strlen (SrcPath);
                                         if (len > 0 && SrcPath [len - 1] == '\\') {
                                            SrcFileName = (char*) malloc (len + strlen (currentFile) + 1);
                                            sprintf (SrcFileName, "%s%s", SrcPath, currentFile);
										 } else {
                                                SrcFileName = (char*) malloc (len + strlen (currentFile) + 2);
                                                sprintf (SrcFileName, "%s\\%s", SrcPath, currentFile);
										 }
                                         len = strlen (WorkPath);
                                         if (len > 0 && WorkPath [len - 1] == '\\') {
                                            WorkFileName = (char*) malloc (len + strlen (currentFile) + 1);
                                            sprintf (WorkFileName, "%s%s", WorkPath, currentFile);
										 } else {
                                                WorkFileName = (char*) malloc (len + strlen (currentFile) + 2);
                                                sprintf (WorkFileName, "%s\\%s", WorkPath, currentFile);
										 } 
									  } else {
                                             len = strlen (SrcPath);
                                             if (len > 0 && SrcPath [len - 1] == '\\') {
                                                SrcFileName = (char*) malloc (len + strlen (currentFile) + 3);
                                                sprintf (SrcFileName, "%s%s.P", SrcPath, currentFile);
											 } else {
                                                    SrcFileName = (char*) malloc (len + strlen (currentFile) + 4);
                                                    sprintf (SrcFileName, "%s\\%s.P", SrcPath, currentFile);
											 }
                                             len = strlen (WorkPath);
                                             if (len > 0 && WorkPath [len - 1] == '\\') {
                                                WorkFileName = (char*) malloc (len + strlen (currentFile) + 3);
                                                sprintf (WorkFileName, "%s%s.P", WorkPath, currentFile);
											 } else {
                                                    WorkFileName = (char*) malloc (len + strlen (currentFile) + 4);
                                                    sprintf (WorkFileName, "%s\\%s.P", WorkPath, currentFile);
											 }  
									  } 

                                      if (currentDestFile) {
                                         ptr = strrchr (currentDestFile, '.');
                                         if (ptr) {
                                            BinFiles [indexBinFiles] = (char*) malloc (strlen (currentDestFile) + 1);
                                            strcpy (BinFiles [indexBinFiles], currentDestFile);
										 } else {
                                                BinFiles [indexBinFiles] = (char*) malloc (strlen (currentDestFile) + 5);
                                                sprintf (BinFiles [indexBinFiles], "%s.PRS", currentDestFile);
										 }
									  } else {
                                             currentFileName = (char*) malloc (strlen (currentFile) + 1);
                                             strcpy (currentFileName, currentFile);
                                             ptr = strrchr (currentFileName, '.');
                                             if (ptr)
                                                ptr [0] = 0;
                                             BinFiles [indexBinFiles] = (char*) malloc (strlen (currentFile) + 5);
                                             sprintf (BinFiles [indexBinFiles], "%s.PRS", currentFile);
									  }

                                      if ((result = Copy_File (hwnd, SrcFileName, WorkFileName)) != FATAL_EXIT_CODE) {
                                         result = ptrMainProc (hwnd, StatusBar, index, args, &Y);
                                         FreeLibrary (hLib);
                                         for (i = 0; i < index; i++) {
                                             free (args [i]);
                                             args [i] = (char*) 0;
										 }
                                         if (currentFile) {
                                            free (currentFile);
                                            currentFile = (char*) 0;
										 }
                                         if (SrcFileName) {
                                            free (SrcFileName);
                                            SrcFileName = (char*) 0;
										 }
                                         if (WorkFileName) {
                                            _unlink (WorkFileName);
                                            free (WorkFileName);
                                            WorkFileName = (char*) 0;
										 }
									  }
                                      if (result == FATAL_EXIT_CODE)
                                         return result;
                                      len = strlen (WorkPath);
                                      if (len > 0 && WorkPath [len - 1] == '\\') {
                                         SrcFileName = (char*) malloc (len + strlen (BinFiles [indexBinFiles]) + 1);
                                         sprintf (SrcFileName, "%s%s", WorkPath, BinFiles [indexBinFiles]);
									  } else {
                                             SrcFileName = (char*) malloc (len + strlen (BinFiles [indexBinFiles]) + 2);
                                             sprintf (SrcFileName, "%s\\%s", WorkPath, BinFiles [indexBinFiles]);
									  }  
                                      len = strlen (SrcPath);
                                      if (len > 0 && SrcPath [len - 1] == '\\') {
                                          WorkFileName = (char*) malloc (len + strlen (BinFiles [indexBinFiles]) + 1);
                                          sprintf (WorkFileName, "%s%s", SrcPath, BinFiles [indexBinFiles]);
									  } else { 
                                             WorkFileName = (char*) malloc (len + strlen (BinFiles [indexBinFiles]) + 2);
                                             sprintf (WorkFileName, "%s\\%s", SrcPath, BinFiles [indexBinFiles]);
									  } 
                                      Copy_File (hwnd, SrcFileName, WorkFileName);
                                      indexBinFiles++;
                                      break;

                                 case STR: 
                                      hLib = LoadLibrary ("str");
                                      if (!hLib)
                                         return FATAL_EXIT_CODE;
                                      ptrMainProc = GetProcAddress (hLib, "STRmain");
                                      if (!ptrMainProc) {
                                         FreeLibrary (hLib);
                                         return FATAL_EXIT_CODE;
									  }

                                      ptr = strrchr(currentFile, '.');
                                      if (ptr) {
                                         len = strlen (SrcPath);
                                         if (len > 0 && SrcPath [len - 1] == '\\') {
                                            SrcFileName = (char*) malloc (len + strlen (currentFile) + 1);
                                            sprintf (SrcFileName, "%s%s", SrcPath, currentFile);
										 } else {
                                                SrcFileName = (char*) malloc (len + strlen (currentFile) + 2);
                                                sprintf (SrcFileName, "%s\\%s", SrcPath, currentFile);
										 }
                                         len = strlen (WorkPath);
                                         if (len > 0 && WorkPath [len - 1] == '\\') {
                                            WorkFileName = (char*) malloc (len + strlen (currentFile) + 1);
                                            sprintf (WorkFileName, "%s%s", WorkPath, currentFile);
										 } else {
                                                WorkFileName = (char*) malloc (len + strlen (currentFile) + 2);
                                                sprintf (WorkFileName, "%s\\%s", WorkPath, currentFile);
										 } 
									  } else {
                                             len = strlen (SrcPath);
                                             if (len > 0 && SrcPath [len - 1] == '\\') {
                                                SrcFileName = (char*) malloc (len + strlen (currentFile) + 3);
                                                sprintf (SrcFileName, "%s%s.S", SrcPath, currentFile);
											 } else {
                                                    SrcFileName = (char*) malloc (len + strlen (currentFile) + 4);
                                                    sprintf (SrcFileName, "%s\\%s.S", SrcPath, currentFile);
											 }
                                             len = strlen (WorkPath);
                                             if (len > 0 && WorkPath [len - 1] == '\\') {
                                                WorkFileName = (char*) malloc (len + strlen (currentFile) + 3);
                                                sprintf (WorkFileName, "%s%s.S", WorkPath, currentFile);
											 } else {
                                                    WorkFileName = (char*) malloc (len + strlen (currentFile) + 4);
                                                    sprintf (WorkFileName, "%s\\%s.S", WorkPath, currentFile);
											 }  
									  } 

                                      if (currentDestFile) {
                                         ptr = strrchr (currentDestFile, '.');
                                         if (ptr) {
                                            BinFiles [indexBinFiles] = (char*) malloc (strlen (currentDestFile) + 1);
                                            strcpy (BinFiles [indexBinFiles], currentDestFile);
										 } else {
                                                BinFiles [indexBinFiles] = (char*) malloc (strlen (currentDestFile) + 5);
                                                sprintf (BinFiles [indexBinFiles], "%s.STR", currentDestFile);
										 }
									  } else {
                                             currentFileName = (char*) malloc (strlen (currentFile) + 1);
                                             strcpy (currentFileName, currentFile);
                                             ptr = strrchr (currentFileName, '.');
                                             if (ptr)
                                                ptr [0] = 0;
                                             BinFiles [indexBinFiles] = (char*) malloc (strlen (currentFile) + 5);
                                             sprintf (BinFiles [indexBinFiles], "%s.STR", currentFile);
									  }

                                      if ((result = Copy_File (hwnd, SrcFileName, WorkFileName)) != FATAL_EXIT_CODE) {
                                         result = ptrMainProc (hwnd, StatusBar, index, args, &Y);
                                         FreeLibrary (hLib);

                                         for (i = 0; i < index; i++) {
                                             free (args [i]);
                                             args [i] = NULL;
										 } 
                                         if (currentFile) {
                                            free (currentFile);
                                            currentFile = NULL;
										 }
                                         if (SrcFileName) {
                                            free (SrcFileName);
                                            SrcFileName = NULL;
										 }
                                         if (WorkFileName) {
											 _unlink (WorkFileName);
                                            free (WorkFileName);
                                            WorkFileName = NULL;
										 }
									  }
                                      if (result == FATAL_EXIT_CODE)
                                         return result;
                                      len = strlen (WorkPath);
                                      if (len > 0 && WorkPath [len - 1] == '\\') {
                                         SrcFileName = (char*) malloc (len + strlen (BinFiles [indexBinFiles]) + 1);
                                         sprintf (SrcFileName, "%s%s", WorkPath, BinFiles [indexBinFiles]);
									  } else {
                                             SrcFileName = (char*) malloc (len + strlen (BinFiles [indexBinFiles]) + 2);
                                             sprintf (SrcFileName, "%s\\%s", WorkPath, BinFiles [indexBinFiles]);
									  }  
                                      len = strlen (SrcPath);
                                      if (len > 0 && SrcPath [len - 1] == '\\') {
                                          WorkFileName = (char*) malloc (len + strlen (BinFiles [indexBinFiles]) + 1);
                                          sprintf (WorkFileName, "%s%s", SrcPath, BinFiles [indexBinFiles]);
									  } else { 
                                             WorkFileName = (char*) malloc (len + strlen (BinFiles [indexBinFiles]) + 2);
                                             sprintf (WorkFileName, "%s\\%s", SrcPath, BinFiles [indexBinFiles]);
									  } 
                                      Copy_File (hwnd, SrcFileName, WorkFileName);
                                      indexBinFiles++;
                                      break;

                                 case TRA: 
                                      hLib = LoadLibrary ("tra");
                                      if (!hLib)
                                         return FATAL_EXIT_CODE;

                                      ptrMainProc = GetProcAddress (hLib, "TRAmain");
                                      if (!ptrMainProc) {
                                         FreeLibrary (hLib);
                                         return FATAL_EXIT_CODE;
									  }

                                      len = strlen (SrcPath);
                                      if (len > 0 && SrcPath [len - 1] == '\\') {
                                         SrcFileName = (char*) malloc (len + 12);
                                         sprintf (SrcFileName, "%sgreek.sgml", SrcPath);
									  } else {
                                             SrcFileName = (char*) malloc (len + 13);
                                             sprintf (SrcFileName, "%s\\greek.sgml", SrcPath);
									  }

                                      len = strlen (WorkPath);
                                      if (len > 0 && WorkPath [len - 1] == '\\') {
                                         WorkFileName = (char*) malloc (len + 12);
                                         sprintf (WorkFileName, "%sgreek.sgml", WorkPath);
									  } else {
                                             WorkFileName = (char*) malloc (len + 13);
                                             sprintf (WorkFileName, "%s\\greek.sgml", WorkPath);
									  }
                
                                      if ((result = Copy_File (hwnd, SrcFileName, WorkFileName)) != FATAL_EXIT_CODE) {
                                         if (SrcFileName) {
										     free (SrcFileName);
										     SrcFileName = NULL;
										 }
										 if (WorkFileName) {
										      free (WorkFileName);
										      WorkFileName = NULL;
										 }
                                         len = strlen (SrcPath);
                                         if (len > 0 && SrcPath [len - 1] == '\\') {
                                            SrcFileName = (char*) malloc (len + 14);
                                            sprintf (SrcFileName, "%sText_SGML.inc", SrcPath);
										 } else {
                                                SrcFileName = (char*) malloc (len + 15);
                                                sprintf (SrcFileName, "%s\\Text_SGML.inc", SrcPath);
										 }

                                         len = strlen (WorkPath);
                                         if (len > 0 && WorkPath [len - 1] == '\\') {
                                            WorkFileName = (char*) malloc (len + 14);
                                            sprintf (WorkFileName, "%sText_SGML.inc", WorkPath);
										 } else {
                                                WorkFileName = (char*) malloc (len + 15);
                                                sprintf (WorkFileName, "%s\\Text_SGML.inc", WorkPath);
										 }

                                         if ((result = Copy_File (hwnd, SrcFileName, WorkFileName)) != FATAL_EXIT_CODE) {
										    if (SrcFileName) {
										        free (SrcFileName);
										        SrcFileName = NULL;
											}
										    if (WorkFileName) {
										        free (WorkFileName);
										        WorkFileName = NULL;
											}
                                            ptr = strrchr(currentFile, '.');
                                            if (ptr) {
                                               len = strlen (SrcPath);
                                               if (len > 0 && SrcPath [len - 1] == '\\') {
                                                  SrcFileName = (char*) malloc (len + strlen (currentFile) + 1);
                                                  sprintf (SrcFileName, "%s%s", SrcPath, currentFile);
											   } else {
                                                      SrcFileName = (char*) malloc (len + strlen (currentFile) + 2);
                                                      sprintf (SrcFileName, "%s\\%s", SrcPath, currentFile);
											   } 
                                               len = strlen (WorkPath);
                                               if (len > 0 && WorkPath [len - 1] == '\\') {
                                                  WorkFileName = (char*) malloc (len + strlen (currentFile) + 1);
                                                  sprintf (WorkFileName, "%s%s", WorkPath, currentFile);
											   } else {
                                                      WorkFileName = (char*) malloc (len + strlen (currentFile) + 2);
                                                      sprintf (WorkFileName, "%s\\%s", WorkPath, currentFile);
											   } 
											} else {
												   len = strlen (SrcPath);
                                                   if (len > 0 && SrcPath [len - 1] == '\\') {
                                                      SrcFileName = (char*) malloc (len + strlen (currentFile) + 3);
                                                      sprintf (SrcFileName, "%s%s.T", SrcPath, currentFile);
												   } else {
                                                          SrcFileName = (char*) malloc (len + strlen (currentFile) + 4);
                                                          sprintf (SrcFileName, "%s\\%s.T", SrcPath, currentFile);
												   }
                                                   len = strlen (WorkPath);
                                                   if (len > 0 && WorkPath [len - 1] == '\\') {
                                                      WorkFileName = (char*) malloc (len + strlen (currentFile) + 3);
                                                      sprintf (WorkFileName, "%s%s.T", WorkPath, currentFile);
												   } else {
                                                          WorkFileName = (char*) malloc (len + strlen (currentFile) + 4);
                                                          sprintf (WorkFileName, "%s\\%s.T", WorkPath, currentFile);
												   }  
											} 

                                            if (currentDestFile) {
                                               ptr = strrchr (currentDestFile, '.');
                                               if (ptr) {
                                                  BinFiles [indexBinFiles] = (char*) malloc (strlen (currentDestFile) + 1);
                                                  strcpy (BinFiles [indexBinFiles], currentDestFile);
											   } else {
                                                      BinFiles [indexBinFiles] = (char*) malloc (strlen (currentDestFile) + 5);
                                                      sprintf (BinFiles [indexBinFiles], "%s.TRA", currentDestFile);
											   }
											} else {
                                                   currentFileName = (char*) malloc (strlen (currentFile) + 1);
                                                   strcpy (currentFileName, currentFile);
                                                   ptr = strrchr (currentFileName, '.');
                                                   if (ptr)
                                                      ptr [0] = 0;
                                                   BinFiles [indexBinFiles] = (char*) malloc (strlen (currentFile) + 5);
                                                   sprintf (BinFiles [indexBinFiles], "%s.TRA", currentFile);
											}
 
                                            if ((result = Copy_File (hwnd, SrcFileName, WorkFileName)) != FATAL_EXIT_CODE) {
                                               result = ptrMainProc (hwnd, StatusBar, index, args, &Y);
                                               FreeLibrary (hLib);
                                               for (i = 0; i < index; i++) {
                                                   free (args [i]);
                                                   args [i] = NULL;
											   }
                                               if (currentFile) {
                                                  free (currentFile);
                                                  currentFile = NULL;
											   }
                                               if (WorkFileName)
												  _unlink (WorkFileName);
											}
										 }
									  }
                                      if (SrcFileName) {
                                          free (SrcFileName);
                                          SrcFileName = NULL;
									  }
                                      if (WorkFileName) {
                                          free (WorkFileName);
                                          WorkFileName = NULL;
									  }

                                      if (result == FATAL_EXIT_CODE)
                                         return result;
                                      len = strlen (WorkPath);
                                      if (len > 0 && WorkPath [len - 1] == '\\') {
                                         SrcFileName = (char*) malloc (len + strlen (BinFiles [indexBinFiles]) + 1);
                                         sprintf (SrcFileName, "%s%s", WorkPath, BinFiles [indexBinFiles]);
									  } else {
                                             SrcFileName = (char*) malloc (len + strlen (BinFiles [indexBinFiles]) + 2);
                                             sprintf (SrcFileName, "%s\\%s", WorkPath, BinFiles [indexBinFiles]);
									  }  
                                      len = strlen (SrcPath);
                                      if (len > 0 && SrcPath [len - 1] == '\\') {
                                          WorkFileName = (char*) malloc (len + strlen (BinFiles [indexBinFiles]) + 1);
                                          sprintf (WorkFileName, "%s%s", SrcPath, BinFiles [indexBinFiles]);
									  } else { 
                                             WorkFileName = (char*) malloc (len + strlen (BinFiles [indexBinFiles]) + 2);
                                             sprintf (WorkFileName, "%s\\%s", SrcPath, BinFiles [indexBinFiles]);
									  } 
                                      Copy_File (hwnd, SrcFileName, WorkFileName);
                                      indexBinFiles++;
                                      break;

                                 default:
                                      for (i = 0; i < index; i++) {
                                          free (args [i]);
                                          args [i] = NULL;
									  }
                                      break;
						  } 
					   /* @@@@@ }  @@@@@ */
					}   
                    line++;
			  }

			  if (SrcFileName) {
			     free (SrcFileName);
				 SrcFileName = NULL;
			  }
			  for (i = 0; i < indexBinFiles; i++) {
                  len = strlen (WorkPath);
                  if (len > 0 && WorkPath [len - 1] == '\\') {
                      SrcFileName = (char*) malloc (len + strlen (BinFiles[i]) + 1);
                      sprintf (SrcFileName, "%s%s", WorkPath, BinFiles [i]);
				  } else {
                         SrcFileName = (char*) malloc (len + strlen (BinFiles [i]) + 2);
                         sprintf (SrcFileName, "%s\\%s", WorkPath, BinFiles [i]);
				  }
                  _unlink (SrcFileName);
                  free (SrcFileName);
                  SrcFileName = NULL;
			  }
              fclose (f);
	   }  
    }
    MakeMessage (hwnd, "\r\n\r\nBuild process success ...", COMP_SUCCESS);
    /* MakeMessage (hwnd, "\r\n\r\nNow you can Build Amaya ...", COMP_SUCCESS); */
    SendMessage (StatusBar, SB_SETTEXT, (WPARAM) 0, (LPARAM) "Finished");
    SendMessage (StatusBar, WM_PAINT, (WPARAM) 0, (LPARAM) 0);
    return COMP_SUCCESS;
}

#ifdef __STDC__
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
#else  /* !__STDC__ */
int WINAPI WinMain (hInstance, hPrevInstance, szCmdLine, iCmdShow)
HINSTANCE hInstance; 
HINSTANCE hPrevInstance; 
PSTR      szCmdLine; 
int       iCmdShow;
#endif /* __STDC__ */
{
     static char szAppName[] = "ThotCompilers";
     HWND        hwnd;
     MSG         msg;
     WNDCLASSEX  wndClass;
     BOOL        ok;
     int         argc;
     char**      argv;
     char*       dir_end;
     char*       BinPath;

	 g_hInstance = hInstance;

     InitCommonControls ();

     argc = makeArgcArgv (hInstance, &argv, szCmdLine);
	 cmdLine = (char*) malloc (strlen (argv[0]) + 1);
	 strcpy (cmdLine, argv [0]);
     TtaInitializeAppRegistry (cmdLine);

     BinPath = TtaGetEnvString ("PWD");
     dir_end = BinPath;

     /* go to the ending NUL */
     while (*dir_end)
           dir_end++;

     /* remove the application name */
     ok = FALSE;
     do {
        dir_end--;
        ok = (dir_end <= BinPath || *dir_end == DIR_SEP);
	 } while (!ok);

     if (*dir_end == DIR_SEP) {
        /* the name has been found */
        *dir_end = EOS;
        /* save the binary directory in BinariesDirectory */
	 }

     WorkPath = (char*) malloc (strlen (BinPath) + 7);
     sprintf (WorkPath, "%s\\amaya", BinPath);

     ThotPath = TtaGetEnvString ("THOTDIR");

     wndClass.style         = CS_HREDRAW | CS_VREDRAW ;
     wndClass.lpfnWndProc   = (WNDPROC) CompilersWndProc ;
     wndClass.cbClsExtra    = 0 ;
     wndClass.cbWndExtra    = 0 ;
     wndClass.hInstance     = hInstance ;
     wndClass.hIcon         = LoadIcon (NULL, COMP_ICON) ;
     wndClass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
     wndClass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
     wndClass.lpszMenuName  = NULL ;
     wndClass.lpszClassName = szAppName ;
     wndClass.cbSize        = sizeof(WNDCLASSEX);
     wndClass.hIconSm       = LoadIcon (hInstance, COMP_ICON) ;

     if (!RegisterClassEx (&wndClass))
        return FALSE;

     hwnd = CreateWindowEx (0, szAppName, "Thot compilers",
                            DS_MODALFRAME | WS_POPUP |
                            WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CAPTION | WS_SYSMENU,
                            0, 0,
                            600, 650,
                            NULL, NULL, hInstance, NULL) ;

    ShowWindow (hwnd, SW_SHOWNORMAL) ;
    UpdateWindow (hwnd) ;

    while (GetMessage (&msg, NULL, 0, 0)) {
          TranslateMessage (&msg) ;
          DispatchMessage (&msg) ;
	} 
    return TRUE;
}

#ifdef __STDC__
LRESULT CALLBACK CompilersWndProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK CompilersWndProc (hwnd, iMsg, wParam, lParam)
HWND   hwnd; 
UINT   iMsg; 
WPARAM wParam; 
LPARAM lParam;
#endif /* __STDC__ */
{
	 static HMENU  menuBar, popupMenu;
     static int    cxChar, cyChar, cyClient;
     int           result = COMP_SUCCESS; 
     int           status, cx, cy;
     int           cyStatus, cyTB;
	 static HWND   hEdit = NULL;
     static CHAR   szBuf[128];
     static HWND   hWndTT;
     LPTOOLTIPTEXT lpttt;
     TOOLINFO      lpToolInfo;
	 RECT          r ={0};
     
     switch (iMsg) {
            case WM_CREATE:
                 menuBar = CreateMenu ();
                 popupMenu = CreateMenu ();
                 AppendMenu (popupMenu, MF_STRING, OPEN, "&Open\tCtrl+O");
                 AppendMenu (popupMenu, MF_STRING, COMPILE, "&Build\tF7");
                 AppendMenu (popupMenu, MF_SEPARATOR, 0, NULL);
                 AppendMenu (popupMenu, MF_STRING, QUIT, "&Quit\tAlt+F4");
                 AppendMenu (menuBar, MF_POPUP, (UINT)popupMenu, "&File");
				 SetMenu (hwnd, menuBar);
                 EnableMenuItem (popupMenu, COMPILE, MFS_GRAYED);

                 hWndToolBar = CreateToolbarEx (hwnd, WS_CHILD | WS_BORDER | WS_VISIBLE | TBSTYLE_TOOLTIPS | CCS_ADJUSTABLE,
                                                ID_TOOLBAR, 2, g_hInstance, COMP_TOOLBAR, (LPCTBBUTTON)&tbButtons,
                                                2, 32, 32, 32, 32, sizeof(TBBUTTON)); 

                 if (hWndToolBar == NULL ) {
                    MessageBox (NULL, "Toolbar Bar not created!", NULL, MB_OK );
                    break;
				 } 

                 SendMessage (hWndToolBar, TB_ENABLEBUTTON, (WPARAM) COMPILE, (LPARAM) MAKELONG (FALSE, 0));

                 GetClientRect( hwnd, &r );
				 hEdit = CreateWindow( "EDIT", 
					                   "", 
									   WS_CHILD|WS_BORDER|WS_HSCROLL|WS_VSCROLL|WS_VISIBLE|
									   ES_AUTOHSCROLL|ES_AUTOVSCROLL|ES_MULTILINE|ES_READONLY,
									   0,0,
									   r.right,r.bottom,
									   hwnd,
									   NULL,
									   g_hInstance,
									   NULL );

                 StatusBar = CreateWindowEx (0L, STATUSCLASSNAME, "", WS_CHILD | WS_BORDER | WS_VISIBLE, 
                                             -100, -100, 10, 10,  hwnd, (HMENU)100, g_hInstance, NULL);
                 ShowWindow (StatusBar, SW_SHOWNORMAL);
                 UpdateWindow (StatusBar);

                 hWndTT = (HWND)SendMessage(hWndToolBar, TB_GETTOOLTIPS, 0, 0);

                 if (hWndTT) {
                    /* Fill in the TOOLINFO structure. */
                    lpToolInfo.cbSize = sizeof(lpToolInfo);
                    lpToolInfo.uFlags = TTF_IDISHWND | TTF_CENTERTIP;
                    lpToolInfo.lpszText = (LPSTR)COMP_TOOLBAR;
                    lpToolInfo.hwnd = hwnd;
                    lpToolInfo.uId = (UINT)hWndToolBar;
                    lpToolInfo.hinst = g_hInstance;
                    /* Set up tooltips for the combo box. */
                    SendMessage(hWndTT, TTM_ADDTOOL, 0, (LPARAM)(LPTOOLINFO)&lpToolInfo);
				 } else
                        MessageBox(NULL, "Could not get tooltip window handle.",NULL, MB_OK);
                 return 0;

            case WM_SIZE:
                 cx = LOWORD (lParam);
                 cy = HIWORD (lParam);
                 GetWindowRect (StatusBar, &r);
                 cyStatus = r.bottom - r.top;

                 SendMessage(hWndToolBar, TB_AUTOSIZE, 0L, 0L);

                 InvalidateRect (hWndToolBar, NULL, TRUE);
                 GetWindowRect (hWndToolBar, &r);
                 ScreenToClient (hwnd, (LPPOINT) &r.left);
                 ScreenToClient (hwnd, (LPPOINT) &r.right);
                 cyTB = r.bottom - r.top;

                 MoveWindow (StatusBar, 0, cy - cyStatus, cx, cyStatus, TRUE);

                 GetClientRect( hwnd, &r );
                 MoveWindow (hEdit, 0, cyTB - 2, r.right, r.bottom - cyStatus - cyTB + 4, TRUE);
                 return 0;

			case WM_ERASEBKGND:
				 return TRUE;

			case WM_DESTROY:
                 PostQuitMessage (0);
                 return 0;

            case WM_KEYDOWN:
                 if ((wParam == VK_F7) && fileToOpen && (fileToOpen[0] != 0)) {
                    SetCursor(LoadCursor(NULL, IDC_WAIT));
                    SetWindowText( hEdit, "" );
                    result = Makefile (hEdit, fileToOpen);
                    SetCursor(LoadCursor(NULL, IDC_ARROW));
                    if (result == FATAL_EXIT_CODE)
                       MakeMessage (hEdit, "\r\n\r\nBuild process aborted because of errors", FATAL_EXIT_CODE);
                    else 
                         MakeMessage (hEdit, "\r\n\r\nYou can build Amaya", FATAL_EXIT_CODE);
				 }
		         break;

            case WM_NOTIFY:
                 switch (((LPNMHDR) lParam)->code) {
                        case TTN_NEEDTEXT: /* Display tool tip text. */
                             lpttt = (LPTOOLTIPTEXT) lParam;
                             CopyToolTipText (lpttt);
                             break;

                        case TBN_QUERYDELETE: /* Toolbar customization -- can we delete this button? */
                             return TRUE;

                        case TBN_GETBUTTONINFO: /* The toolbar needs information about a button. */
                             return FALSE;

                        case TBN_QUERYINSERT: /* Can this button be inserted? Just say yo. */
                             return TRUE;

                        case TBN_CUSTHELP: /* Need to display custom help. */
                             MessageBox(hwnd, "This help is custom.",NULL, MB_OK);
                             break;

                        case TBN_TOOLBARCHANGE: /* Done dragging a bitmap to the toolbar. */
                             SendMessage(hWndToolBar, TB_AUTOSIZE, 0L, 0L);
                             break;

                        default: return TRUE;
				 }
                 return 0L;

            case WM_CHAR:
                    status = GetKeyState (VK_CONTROL);
                    if (HIBYTE (status)) {
                       if (wParam == CTRL_O)
                             OpenFileName.lStructSize       = sizeof (OPENFILENAME); 
                             OpenFileName.hwndOwner         = hwnd; 
                             OpenFileName.hInstance         = (HINSTANCE) GetWindowLong (hwnd, GWL_HINSTANCE); 
                             OpenFileName.lpstrFilter       = (LPSTR) szFilter; 
                             OpenFileName.lpstrCustomFilter = (LPTSTR) NULL; 
                             OpenFileName.nMaxCustFilter    = 0L; 
                             OpenFileName.nFilterIndex      = 1L; 
                             OpenFileName.lpstrFile         = (LPSTR) szFileName; 
                             OpenFileName.nMaxFile          = 256; 
                             OpenFileName.lpstrInitialDir   = NULL; 
                             OpenFileName.lpstrTitle        = TEXT ("Open a File"); 
                             OpenFileName.nFileOffset       = 0; 
                             OpenFileName.nFileExtension    = 0; 
                             OpenFileName.lpstrDefExt       = TEXT ("*.html"); 
                             OpenFileName.lCustData         = 0; 
                             OpenFileName.Flags             = OFN_SHOWHELP | OFN_HIDEREADONLY; 
  
                             if (GetOpenFileName (&OpenFileName)) {
                                strcpy (fileToOpen, OpenFileName.lpstrFile);
                                EnableMenuItem (popupMenu, COMPILE, MFS_ENABLED);
                                SendMessage (hWndToolBar, TB_ENABLEBUTTON, (WPARAM) COMPILE, (LPARAM) MAKELONG (TRUE, 0));
							 }
					}
                    break;

            case WM_COMMAND:
                 switch (LOWORD (wParam)) {
                        case OPEN: 
                             OpenFileName.lStructSize       = sizeof (OPENFILENAME); 
                             OpenFileName.hwndOwner         = hwnd; 
                             OpenFileName.hInstance         = (HINSTANCE) GetWindowLong (hwnd, GWL_HINSTANCE); 
                             OpenFileName.lpstrFilter       = (LPSTR) szFilter; 
                             OpenFileName.lpstrCustomFilter = (LPTSTR) NULL; 
                             OpenFileName.nMaxCustFilter    = 0L; 
                             OpenFileName.nFilterIndex      = 1L; 
                             OpenFileName.lpstrFile         = (LPSTR) szFileName; 
                             OpenFileName.nMaxFile          = 256; 
                             OpenFileName.lpstrInitialDir   = NULL; 
                             OpenFileName.lpstrTitle        = TEXT ("Open a File"); 
                             OpenFileName.nFileOffset       = 0; 
                             OpenFileName.nFileExtension    = 0; 
                             OpenFileName.lpstrDefExt       = TEXT ("*.html"); 
                             OpenFileName.lCustData         = 0; 
                             OpenFileName.Flags             = OFN_SHOWHELP | OFN_HIDEREADONLY; 
  
                             if (GetOpenFileName (&OpenFileName)) {
                                strcpy (fileToOpen, OpenFileName.lpstrFile);
                                EnableMenuItem (popupMenu, COMPILE, MFS_ENABLED);
                                SendMessage (hWndToolBar, TB_ENABLEBUTTON, (WPARAM) COMPILE, (LPARAM) MAKELONG (TRUE, 0));
							 }
                             break;

                        case COMPILE: 
							 SetCursor(LoadCursor(NULL, IDC_WAIT));
							 SetWindowText( hEdit, "" );
                             result = Makefile (hEdit, fileToOpen);
 							 SetCursor(LoadCursor(NULL, IDC_ARROW));
                             if (result == FATAL_EXIT_CODE)
                                MakeMessage (hEdit, "Build process aborted because of errors", FATAL_EXIT_CODE);
                             else 
                                 MakeMessage (hEdit, "\r\n\r\nYou can build Amaya", COMP_SUCCESS);
                             break;

                        case QUIT: SendMessage (hwnd, WM_CLOSE, 0, 0);
                             break;
				 }
	 }
     
     return DefWindowProc (hwnd, iMsg, wParam, lParam);
}
#endif /* _WINDOWS */
