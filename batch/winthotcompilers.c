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
#include <string.h>
#include <stdio.h>
#include "fileaccess.h"
#include "message.h"
#include "registry.h"
#include "resource.h"
/* #include "compilers_f.h" */

#define OPEN    100
#define COMPILE 101
#define QUIT    102

#define NEW_LINE '\n'
static char* szFilter = "Amaya Makefiles (*.mkf)\0*.mkf\0All files (*.*)\0*.*\0";

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

HWND  hWND = (HWND) 0;
int   Y = 10;
char* cmdLine;
char* SrcPath;
char* DestPath;
char* WorkPath;
char* ThotPath;
char* currentFile;
char* currentDestFile;
char* BinFiles [100];

#ifdef __STDC__
LRESULT CALLBACK CompilersWndProc (HWND, UINT, WPARAM, LPARAM);
extern char*     TtaGetMemory     (int);
extern void      TtaFreeMemory    (void*);
#else  /* !__STDC__ */
LRESULT CALLBACK CompilersWndProc ();
extern char*     TtaGetMemory     ();
extern void      TtaFreeMemory    ();
#endif /* __STDC__ */

static OPENFILENAME OpenFileName;
static char       szFileName[256];
static char       fileToOpen [256];
static int          iVscrollPos = 0, iVscrollMax, iVscrollInc; 


#ifdef __STDC__
void MakeMessage (HWND hwnd, char* errorMsg, int msgType)
#else  /* !__STDC__ */
void MakeMessage (hwnd, errorMsg, msgType)
HWND   hwnd;
char* errorMsg;
int   msgType;
#endif /* __STDC__ */
{
   TEXTMETRIC  textMetric;
   COLORREF    oldColor;
   HFONT       hFont, hOldFont;
   HDC         hDC;
   int         cxChar, cyChar;

   if (hwnd) {
      hDC = GetDC (hwnd);

      hFont = CreateFont (16, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, 
                          OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, 
                          DEFAULT_PITCH | FF_DONTCARE, "Arial");

      hOldFont = SelectObject (hDC, hFont);

      GetTextMetrics (hDC, &textMetric);
      cxChar = textMetric.tmAveCharWidth;
      cyChar = textMetric.tmHeight + textMetric.tmExternalLeading + 1;

      if (Y >= 550) {
         ScrollWindow (hwnd, 0, -cyChar, NULL, NULL);
         UpdateWindow (hwnd);
      } else
           Y += cyChar;

      switch (msgType) {
              case FATAL_EXIT_CODE:
                   oldColor = SetTextColor (hDC, RGB (255, 0, 0));
                   break;

              case COMP_WARNING:
                   oldColor = SetTextColor (hDC, RGB (0, 0, 255));
                   break;

              case COMP_SUCCESS:
                   oldColor = SetTextColor (hDC, RGB (0, 0, 0));
                   break;
	  }
      if (!TextOut (hDC, 5, Y, errorMsg, strlen (errorMsg)))
         MessageBox (NULL, "Error Writing text", "Thot Compilers", MB_OK);

      SetTextColor (hDC, oldColor);
      SelectObject (hDC, hOldFont);
	  DeleteObject (hFont);
      ReleaseDC (hWND, hDC);
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
    FILE*  srcFile;
    FILE*  destFile;
    char   errorMsg [800];
	int    c;

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
    return COMP_SUCCESS;
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
    int           argc;
    static char*  argv[20];
    static char   argv0[256];
    char*         ptr     = commandLine;
    char          lookFor = 0;

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
    FILE*   f; 
    HANDLE  hLib;
    FARPROC ptrMainProc;
    char    msg [1024];
    char    seps[]   = " \t=$()\n\r";
	char    string [1024];
    char*   args [100];
	char*   token;
    char*   pChar;
    char*   ptr;
    char*   SrcFileName;
    char*   WorkFileName;
    char*   currentFileName;
	int     command;
    int     index, i;
    int     indexBinFiles = 0;
    int     len;
    int     line = 1;
    int     result = COMP_SUCCESS;

    if (hwnd) {
       if ((f = fopen (fileName, "r")) == NULL) {
          usprintf (msg, "Cannot open file %s", fileToOpen);
          MessageBox (hwnd, msg, "Make error", MB_OK | MB_ICONWARNING);
	   } else {
              while (!feof (f)) {
                    if (currentFile) {
                       TtaFreeMemory (currentFile);
                       currentFile = (char*) 0;
					} 
                    if (currentDestFile) {
                       TtaFreeMemory (currentDestFile);
                       currentDestFile = (char*) 0;
					} 

                    /* Establish string and get the first token: */
                    string [0] = 0;
                    fgets (string, 1024, f);

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
                               usprintf (msg, "Line %d: unknown command %s", line, token);
                               MakeMessage (hwnd, msg, FATAL_EXIT_CODE);				   
						  } 
					   }  
                       if (command != ERROR_CMD) {
                          index = 0;
                          args [index] = TtaGetMemory (strlen (cmdLine) + 1);
                          strcpy (args [index++], cmdLine);
                          while ((token = strtok (NULL, seps)) != NULL) {
                                /* While there are tokens in "string" */
                                ptr = strrchr(token, '.');
                                if (ptr || (token [0] != '-')) {
                                   if (!currentFile) {
                                      currentFile = TtaGetMemory (strlen (token) + 1);
                                      strcpy (currentFile, token);
								   } else {
                                          currentDestFile = TtaGetMemory (strlen (token) + 1);
                                          strcpy (currentDestFile, token);
								   }
								} 
                                args [index] = TtaGetMemory (strlen (token) + 1);
                                strcpy (args [index++], token);
                                /* Get next token: */
                                /* token = strtok (NULL, seps); */
						  }
                          switch (command) {
                                 case SRC_DIR: 
                                      if (SrcPath) {
                                         TtaFreeMemory (SrcPath);
                                         SrcPath = (char*) 0;
									  } 
                                      if (!strcmp (args [1], "THOTDIR")) {
                                         if (index > 2) {
                                            SrcPath = TtaGetMemory (strlen (ThotPath) + strlen (args [2]) + 1);
                                            strcpy (SrcPath, ThotPath);
                                            strcat (SrcPath, args [2]);
										 } else {
                                                SrcPath = TtaGetMemory (strlen (ThotPath) + 1);
                                                strcpy (SrcPath, ThotPath);
										 }
									  }
                                      for (i = 0; i < index; i++) {
                                          TtaFreeMemory (args [i]);
                                          args [i] = (char*) 0;
									  }
                                      break;

                                 case DEST_DIR: 
                                      if (DestPath) {
                                         TtaFreeMemory (DestPath);
                                         DestPath = (char*) 0;
									  } 
                                      if (!strcmp (args [1], "THOTDIR")) {
                                         if (index > 2) {
                                            DestPath = TtaGetMemory (strlen (ThotPath) + strlen (args [2]) + 1);
                                            strcpy (DestPath, ThotPath);
                                            strcat (DestPath, args [2]);
										 } else {
                                                DestPath = TtaGetMemory (strlen (ThotPath) + 1);
                                                strcpy (DestPath, ThotPath);
										 }
									  }
                                      for (i = 0; i < index; i++) {
                                          TtaFreeMemory (args [i]);
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
                                         if (SrcPath [len - 1] == '\\') {
                                            SrcFileName = TtaGetMemory (len + strlen (currentFile) + 1);
                                            usprintf (SrcFileName, "%s%s", SrcPath, currentFile);
										 } else {
                                                SrcFileName = TtaGetMemory (len + strlen (currentFile) + 2);
                                                usprintf (SrcFileName, "%s\\%s", SrcPath, currentFile);
										 }
                                         len = strlen (WorkPath);
                                         if (WorkPath [len - 1] == '\\') {
                                            WorkFileName = TtaGetMemory (len + strlen (currentFile) + 1);
                                            usprintf (WorkFileName, "%s%s", WorkPath, currentFile);
										 } else {
                                                WorkFileName = TtaGetMemory (len + strlen (currentFile) + 2);
                                                usprintf (WorkFileName, "%s\\%s", WorkPath, currentFile);
										 } 
									  } else {
                                             len = strlen (SrcPath);
                                             if (SrcPath [len - 1] == '\\') {
                                                SrcFileName = TtaGetMemory (len + strlen (currentFile) + 3);
                                                usprintf (SrcFileName, "%s%s.A", SrcPath, currentFile);
											 } else {
                                                    SrcFileName = TtaGetMemory (len + strlen (currentFile) + 3);
                                                    usprintf (SrcFileName, "%s\\%s.A", SrcPath, currentFile);
											 }
                                             len = strlen (WorkPath);
                                             if (WorkPath [len - 1] == '\\') {
                                                WorkFileName = TtaGetMemory (len + strlen (currentFile) + 3);
                                                usprintf (WorkFileName, "%s%s.A", WorkPath, currentFile);
											 } else {
                                                    WorkFileName = TtaGetMemory (len + strlen (currentFile) + 4);
                                                    usprintf (WorkFileName, "%s\\%s.A", WorkPath, currentFile);
											 }  
									  } 

                                      Copy_File (hwnd, SrcFileName, WorkFileName);

                                      result = ptrMainProc (hwnd, index, args, &Y);
                                      FreeLibrary (hLib);
                                      for (i = 0; i < index; i++) {
                                          TtaFreeMemory (args [i]);
                                          args [i] = (char*) 0;
									  }
                                      if (currentFile) {
                                         TtaFreeMemory (currentFile);
                                         currentFile = (char*) 0;
									  }
                                      if (SrcFileName) {
                                         TtaFreeMemory (SrcFileName);
                                         SrcFileName = (char*) 0;
									  }
                                      if (WorkFileName) {
                                         uunlink (WorkFileName);
                                         TtaFreeMemory (WorkFileName);
                                         WorkFileName = (char*) 0;
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
                                         if (SrcPath [len - 1] == '\\') {
                                            SrcFileName = TtaGetMemory (len + strlen (currentFile) + 1);
                                            usprintf (SrcFileName, "%s%s", SrcPath, currentFile);
										 } else {
                                                SrcFileName = TtaGetMemory (len + strlen (currentFile) + 2);
                                                usprintf (SrcFileName, "%s\\%s", SrcPath, currentFile);
										 }
                                         len = strlen (WorkPath);
                                         if (WorkPath [len - 1] == '\\') {
                                            WorkFileName = TtaGetMemory (len + strlen (currentFile) + 1);
                                            usprintf (WorkFileName, "%s%s", WorkPath, currentFile);
										 } else {
                                                WorkFileName = TtaGetMemory (len + strlen (currentFile) + 2);
                                                usprintf (WorkFileName, "%s\\%s", WorkPath, currentFile);
										 } 
									  } else {
                                             len = strlen (SrcPath);
                                             if (SrcPath [len - 1] == '\\') {
                                                SrcFileName = TtaGetMemory (len + strlen (currentFile) + 3);
                                                usprintf (SrcFileName, "%s%s.P", SrcPath, currentFile);
											 } else {
                                                    SrcFileName = TtaGetMemory (len + strlen (currentFile) + 4);
                                                    usprintf (SrcFileName, "%s\\%s.P", SrcPath, currentFile);
											 }
                                             len = strlen (WorkPath);
                                             if (WorkPath [len - 1] == '\\') {
                                                WorkFileName = TtaGetMemory (len + strlen (currentFile) + 3);
                                                usprintf (WorkFileName, "%s%s.P", WorkPath, currentFile);
											 } else {
                                                    WorkFileName = TtaGetMemory (len + strlen (currentFile) + 4);
                                                    usprintf (WorkFileName, "%s\\%s.P", WorkPath, currentFile);
											 }  
									  } 

                                      if (currentDestFile) {
                                         ptr = strrchr (currentDestFile, '.');
                                         if (ptr) {
                                            BinFiles [indexBinFiles] = TtaGetMemory (strlen (currentDestFile) + 1);
                                            strcpy (BinFiles [indexBinFiles], currentDestFile);
										 } else {
                                                BinFiles [indexBinFiles] = TtaGetMemory (strlen (currentDestFile) + 4);
                                                usprintf (BinFiles [indexBinFiles], "%s.PRS", currentDestFile);
										 }
									  } else {
                                             currentFileName = TtaGetMemory (strlen (currentFile) + 1);
                                             strcpy (currentFileName, currentFile);
                                             ptr = strrchr (currentFileName, '.');
                                             if (ptr)
                                                ptr [0] = 0;
                                             BinFiles [indexBinFiles] = TtaGetMemory (strlen (currentFile) + 4);
                                             usprintf (BinFiles [indexBinFiles], "%s.PRS", currentFile);
									  }

                                      Copy_File (hwnd, SrcFileName, WorkFileName);

                                      result = ptrMainProc (hwnd, index, args, &Y);
                                      FreeLibrary (hLib);
                                      for (i = 0; i < index; i++) {
                                          TtaFreeMemory (args [i]);
                                          args [i] = (char*) 0;
									  }
                                      if (currentFile) {
                                         TtaFreeMemory (currentFile);
                                         currentFile = (char*) 0;
									  }
                                      if (SrcFileName) {
                                         TtaFreeMemory (SrcFileName);
                                         SrcFileName = (char*) 0;
									  }
                                      if (WorkFileName) {
                                         uunlink (WorkFileName);
                                         TtaFreeMemory (WorkFileName);
                                         WorkFileName = (char*) 0;
									  }

                                      if (result == FATAL_EXIT_CODE)
                                         return result;
                                      len = strlen (WorkPath);
                                      if (WorkPath [len - 1] == '\\') {
                                         SrcFileName = TtaGetMemory (len + strlen (BinFiles [indexBinFiles]) + 1);
                                         usprintf (SrcFileName, "%s%s", WorkPath, BinFiles [indexBinFiles]);
									  } else {
                                             SrcFileName = TtaGetMemory (len + strlen (BinFiles [indexBinFiles]) + 2);
                                             usprintf (SrcFileName, "%s\\%s", WorkPath, BinFiles [indexBinFiles]);
									  }  
                                      len = strlen (SrcPath);
                                      if (SrcPath [len - 1] == '\\') {
                                          WorkFileName = TtaGetMemory (len + strlen (BinFiles [indexBinFiles]) + 1);
                                          usprintf (WorkFileName, "%s%s", SrcPath, BinFiles [indexBinFiles]);
									  } else { 
                                             WorkFileName = TtaGetMemory (len + strlen (BinFiles [indexBinFiles]) + 2);
                                             usprintf (WorkFileName, "%s\\%s", SrcPath, BinFiles [indexBinFiles]);
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

                                      ptr = strrchr (currentFile, '.');
                                      if (ptr) {
                                         len = strlen (SrcPath);
                                         if (SrcPath [len - 1] == '\\') {
                                            SrcFileName = TtaGetMemory (len + strlen (currentFile) + 1);
                                            usprintf (SrcFileName, "%s%s", SrcPath, currentFile);
										 } else {
                                                SrcFileName = TtaGetMemory (len + strlen (currentFile) + 2);
                                                usprintf (SrcFileName, "%s\\%s", SrcPath, currentFile);
										 }
                                         len = strlen (WorkPath);
                                         if (WorkPath [len - 1] == '\\') {
                                            WorkFileName = TtaGetMemory (len + strlen (currentFile) + 1);
                                            usprintf (WorkFileName, "%s%s", WorkPath, currentFile);
										 } else {
                                                WorkFileName = TtaGetMemory (len + strlen (currentFile) + 2);
                                                usprintf (WorkFileName, "%s\\%s", WorkPath, currentFile);
										 } 
									  } else {
                                             len = strlen (SrcPath);
                                             if (SrcPath [len - 1] == '\\') {
                                                SrcFileName = TtaGetMemory (len + strlen (currentFile) + 3);
                                                usprintf (SrcFileName, "%s%s.S", SrcPath, currentFile);
											 } else {
                                                    SrcFileName = TtaGetMemory (len + strlen (currentFile) + 4);
                                                    usprintf (SrcFileName, "%s\\%s.S", SrcPath, currentFile);
											 }
                                             len = strlen (WorkPath);
                                             if (WorkPath [len - 1] == '\\') {
                                                WorkFileName = TtaGetMemory (len + strlen (currentFile) + 3);
                                                usprintf (WorkFileName, "%s%s.S", WorkPath, currentFile);
											 } else {
                                                    WorkFileName = TtaGetMemory (len + strlen (currentFile) + 4);
                                                    usprintf (WorkFileName, "%s\\%s.S", WorkPath, currentFile);
											 }  
									  } 

                                      if (currentDestFile) {
                                         ptr = strrchr (currentDestFile, '.');
                                         if (ptr) {
                                            BinFiles [indexBinFiles] = TtaGetMemory (strlen (currentDestFile) + 1);
                                            strcpy (BinFiles [indexBinFiles], currentDestFile);
										 } else {
                                                BinFiles [indexBinFiles] = TtaGetMemory (strlen (currentDestFile) + 4);
                                                usprintf (BinFiles [indexBinFiles], "%s.STR", currentDestFile);
										 }
									  } else {
                                             currentFileName = TtaGetMemory (strlen (currentFile) + 1);
                                             strcpy (currentFileName, currentFile);
                                             ptr = strrchr (currentFileName, '.');
                                             if (ptr)
                                                ptr [0] = 0;
                                             BinFiles [indexBinFiles] = TtaGetMemory (strlen (currentFile) + 4);
                                             usprintf (BinFiles [indexBinFiles], "%s.STR", currentFile);
									  }

                                      Copy_File (hwnd, SrcFileName, WorkFileName);

                                      result = ptrMainProc (hwnd, index, args, &Y);
                                      FreeLibrary (hLib);
                                      /* result = STRmain (hwnd, index, args, &Y); */
                                      for (i = 0; i < index; i++) {
                                          TtaFreeMemory (args [i]);
                                          args [i] = (char*) 0;
									  }
                                      if (currentFile) {
                                         TtaFreeMemory (currentFile);
                                         currentFile = (char*) 0;
									  }
                                      if (SrcFileName) {
                                         TtaFreeMemory (currentFile);
                                         currentFile = (char*) 0;
									  }
                                      if (WorkFileName) {
                                         TtaFreeMemory (currentFile);
                                         currentFile = (char*) 0;
									  }
                                      uunlink (WorkFileName);
                                      if (result == FATAL_EXIT_CODE)
                                         return result;
                                      len = strlen (WorkPath);
                                      if (WorkPath [len - 1] == '\\') {
                                         SrcFileName = TtaGetMemory (len + strlen (BinFiles [indexBinFiles]) + 1);
                                         usprintf (SrcFileName, "%s%s", WorkPath, BinFiles [indexBinFiles]);
									  } else {
                                             SrcFileName = TtaGetMemory (len + strlen (BinFiles [indexBinFiles]) + 2);
                                             usprintf (SrcFileName, "%s\\%s", WorkPath, BinFiles [indexBinFiles]);
									  }  
                                      len = strlen (SrcPath);
                                      if (SrcPath [len - 1] == '\\') {
                                          WorkFileName = TtaGetMemory (len + strlen (BinFiles [indexBinFiles]) + 1);
                                          usprintf (WorkFileName, "%s%s", SrcPath, BinFiles [indexBinFiles]);
									  } else { 
                                             WorkFileName = TtaGetMemory (len + strlen (BinFiles [indexBinFiles]) + 2);
                                             usprintf (WorkFileName, "%s\\%s", SrcPath, BinFiles [indexBinFiles]);
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
                                      if (SrcPath [len - 1] == '\\') {
                                         SrcFileName = TtaGetMemory (len + 12);
                                         usprintf (SrcFileName, "%sgreek.sgml", SrcPath);
									  } else {
                                             SrcFileName = TtaGetMemory (len + 13);
                                             usprintf (SrcFileName, "%s\\greek.sgml", SrcPath);
									  }

                                      len = strlen (WorkPath);
                                      if (WorkPath [len - 1] == '\\') {
                                         WorkFileName = TtaGetMemory (len + 12);
                                         usprintf (WorkFileName, "%sgreek.sgml", WorkPath);
									  } else {
                                             WorkFileName = TtaGetMemory (len + 13);
                                             usprintf (WorkFileName, "%s\\greek.sgml", WorkPath);
									  }

                                      Copy_File (hwnd, SrcFileName, WorkFileName);

                                      len = strlen (SrcPath);
                                      if (SrcPath [len - 1] == '\\') {
                                         SrcFileName = TtaGetMemory (len + 14);
                                         usprintf (SrcFileName, "%sText_SGML.inc", SrcPath);
									  } else {
                                             SrcFileName = TtaGetMemory (len + 15);
                                             usprintf (SrcFileName, "%s\\Text_SGML.inc", SrcPath);
									  }

                                      len = strlen (WorkPath);
                                      if (WorkPath [len - 1] == '\\') {
                                         WorkFileName = TtaGetMemory (len + 14);
                                         usprintf (WorkFileName, "%sText_SGML.inc", WorkPath);
									  } else {
                                             WorkFileName = TtaGetMemory (len + 15);
                                             usprintf (WorkFileName, "%s\\Text_SGML.inc", WorkPath);
									  }

                                      Copy_File (hwnd, SrcFileName, WorkFileName);

                                      ptr = strrchr (currentFile, '.');
                                      if (ptr) {
                                         len = strlen (SrcPath);
                                         if (SrcPath [len - 1] == '\\') {
                                            SrcFileName = TtaGetMemory (len + strlen (currentFile) + 1);
                                            usprintf (SrcFileName, "%s%s", SrcPath, currentFile);
										 } else {
                                                SrcFileName = TtaGetMemory (len + strlen (currentFile) + 2);
                                                usprintf (SrcFileName, "%s\\%s", SrcPath, currentFile);
										 }
                                         len = strlen (WorkPath);
                                         if (WorkPath [len - 1] == '\\') {
                                            WorkFileName = TtaGetMemory (len + strlen (currentFile) + 1);
                                            usprintf (WorkFileName, "%s%s", WorkPath, currentFile);
										 } else {
                                                WorkFileName = TtaGetMemory (len + strlen (currentFile) + 2);
                                                usprintf (WorkFileName, "%s\\%s", WorkPath, currentFile);
										 } 
									  } else {
                                             if (SrcPath [len - 1] == '\\') {
                                                SrcFileName = TtaGetMemory (len + strlen (currentFile) + 3);
                                                usprintf (SrcFileName, "%s%s.T", SrcPath, currentFile);
											 } else {
                                                    SrcFileName = TtaGetMemory (len + strlen (currentFile) + 4);
                                                    usprintf (SrcFileName, "%s\\%s.T", SrcPath, currentFile);
											 }
                                             len = strlen (WorkPath);
                                             if (WorkPath [len - 1] == '\\') {
                                                WorkFileName = TtaGetMemory (len + strlen (currentFile) + 3);
                                                usprintf (WorkFileName, "%s%s.T", WorkPath, currentFile);
											 } else {
                                                    WorkFileName = TtaGetMemory (len + strlen (currentFile) + 4);
                                                    usprintf (WorkFileName, "%s\\%s.T", WorkPath, currentFile);
											 }  
									  } 

                                      if (currentDestFile) {
                                         ptr = strrchr (currentDestFile, '.');
                                         if (ptr) {
                                            BinFiles [indexBinFiles] = TtaGetMemory (strlen (currentDestFile) + 1);
                                            strcpy (BinFiles [indexBinFiles], currentDestFile);
										 } else {
                                                BinFiles [indexBinFiles] = TtaGetMemory (strlen (currentDestFile) + 4);
                                                usprintf (BinFiles [indexBinFiles], "%s.TRA", currentDestFile);
										 }
									  } else {
                                             currentFileName = TtaGetMemory (strlen (currentFile) + 1);
                                             strcpy (currentFileName, currentFile);
                                             ptr = strrchr (currentFileName, '.');
                                             if (ptr)
                                                ptr [0] = 0;
                                             BinFiles [indexBinFiles] = TtaGetMemory (strlen (currentFile) + 4);
                                             usprintf (BinFiles [indexBinFiles], "%s.TRA", currentFile);
									  }

                                      Copy_File (hwnd, SrcFileName, WorkFileName);

                                      result = ptrMainProc (hwnd, index, args, &Y);
                                      FreeLibrary (hLib);
                                      /* result = TRAmain (hwnd, index, args, &Y); */
                                      for (i = 0; i < index; i++) {
                                          TtaFreeMemory (args [i]);
                                          args [i] = (char*) 0;
									  }
                                      if (currentFile) {
                                         TtaFreeMemory (currentFile);
                                         currentFile = (char*) 0;
									  }
                                      if (SrcFileName) {
                                         TtaFreeMemory (currentFile);
                                         currentFile = (char*) 0;
									  }
                                      if (WorkFileName) {
                                         TtaFreeMemory (currentFile);
                                         currentFile = (char*) 0;
									  }
                                      uunlink (WorkFileName);
                                      if (result == FATAL_EXIT_CODE)
                                         return result;
                                      len = strlen (WorkPath);
                                      if (WorkPath [len - 1] == '\\') {
                                         SrcFileName = TtaGetMemory (len + strlen (BinFiles [indexBinFiles]) + 1);
                                         usprintf (SrcFileName, "%s%s", WorkPath, BinFiles [indexBinFiles]);
									  } else {
                                             SrcFileName = TtaGetMemory (len + strlen (BinFiles [indexBinFiles]) + 2);
                                             usprintf (SrcFileName, "%s\\%s", WorkPath, BinFiles [indexBinFiles]);
									  }  
                                      len = strlen (SrcPath);
                                      if (SrcPath [len - 1] == '\\') {
                                          WorkFileName = TtaGetMemory (len + strlen (BinFiles [indexBinFiles]) + 1);
                                          usprintf (WorkFileName, "%s%s", SrcPath, BinFiles [indexBinFiles]);
									  } else { 
                                             WorkFileName = TtaGetMemory (len + strlen (BinFiles [indexBinFiles]) + 2);
                                             usprintf (WorkFileName, "%s\\%s", SrcPath, BinFiles [indexBinFiles]);
									  } 
                                      Copy_File (hwnd, SrcFileName, WorkFileName);
                                      indexBinFiles++;
                                      break;

                                 default:
                                      for (i = 0; i < index; i++) {
                                          TtaFreeMemory (args [i]);
                                          args [i] = (char*) 0;
									  }
                                      break;
						  } 
					   } 
					}   
                    line++;
			  } 
			  for (i = 0; i < indexBinFiles; i++) {
                  /* if (SrcFileName) {
                     TtaFreeMemory (SrcFileName);
                     SrcFileName = (char*) 0;
				  }*/
                  len = strlen (WorkPath);
                  if (WorkPath [len - 1] == '\\') {
                      SrcFileName = TtaGetMemory (len + strlen (BinFiles[i]) + 1);
                      usprintf (SrcFileName, "%s%s", WorkPath, BinFiles [i]);
				  } else {
                         SrcFileName = TtaGetMemory (len + strlen (BinFiles [i]) + 2);
                         usprintf (SrcFileName, "%s\\%s", WorkPath, BinFiles [i]);
				  }
                  uunlink (SrcFileName);
			  }
              fclose (f);
	   }  
    }
    MakeMessage (hwnd, "Build process success ...", COMP_SUCCESS);
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

     argc = makeArgcArgv (hInstance, &argv, szCmdLine);
	 cmdLine = TtaGetMemory (strlen (argv[0]) + 1);
	 strcpy (cmdLine, argv [0]);
     TtaInitializeAppRegistry (cmdLine);

     BinPath = TtaGetEnvString ("PWD");
     dir_end = BinPath;

     /* go to the ending NULL */
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

     WorkPath = TtaGetMemory (strlen (BinPath) + 7);
     usprintf (WorkPath, "%s\\amaya", BinPath);

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

     hwnd = CreateWindowEx (WS_EX_STATICEDGE | WS_EX_OVERLAPPEDWINDOW | WS_EX_DLGMODALFRAME, szAppName, "Thot compilers",
                            DS_MODALFRAME | WS_POPUP | WS_VSCROLL |
                            WS_VISIBLE | WS_CAPTION | WS_SYSMENU,
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
	 static HMENU menuBar, popupMenu;
     static int   cxChar, cyChar, cyClient;
	 TEXTMETRIC   tm;
     HDC          hDC;
     int          result = COMP_SUCCESS;
     
     switch (iMsg) {
            case WM_CREATE:
                 hDC = GetDC (hwnd);
                 GetTextMetrics (hDC, &tm);
                 cxChar = tm.tmAveCharWidth;
                 cyChar = tm.tmHeight + tm.tmExternalLeading;
                 ReleaseDC (hwnd, hDC);
                 SetScrollRange (hwnd, SB_VERT, 0, NUMLINES, FALSE);
                 SetScrollPos (hwnd, SB_VERT, iVscrollPos,TRUE);
                 menuBar = CreateMenu ();
                 popupMenu = CreateMenu ();
                 AppendMenu (popupMenu, MF_STRING, OPEN, "Open");
                 AppendMenu (popupMenu, MF_STRING, COMPILE, "Build	F7");
                 AppendMenu (popupMenu, MF_SEPARATOR, 0, NULL);
                 AppendMenu (popupMenu, MF_STRING, QUIT, "Quit");
                 AppendMenu (menuBar, MF_POPUP, (UINT)popupMenu, "File");
				 SetMenu (hwnd, menuBar);
                 EnableMenuItem (popupMenu, COMPILE, MFS_GRAYED);
                 ShowScrollBar (hwnd, SB_VERT, TRUE);
                 return 0;

            case WM_SIZE:
                 cyClient = HIWORD (lParam);
                 iVscrollMax = max (0, NUMLINES + 2 - cyClient / cyChar);
                 SetScrollRange (hwnd, SB_VERT, 0, iVscrollMax, FALSE);
                 SetScrollPos (hwnd, SB_VERT, iVscrollPos, TRUE);
                 return 0;

            case WM_DESTROY:
                 PostQuitMessage (0);
                 return 0;

            case WM_VSCROLL:
                 switch (LOWORD (wParam)) {
                        case SB_TOP:
                             iVscrollPos = 0;
                             break;

                        case SB_BOTTOM:
                             iVscrollPos = iVscrollMax;
                             break;

                        case SB_LINEUP:
                             iVscrollInc = -1;
                             break;   

                        case SB_LINEDOWN:
                             iVscrollInc = 1;
                             break;   

                        case SB_PAGEUP:
                             iVscrollInc = min (-1, cyClient / cyChar);
                             break;   

                        case SB_PAGEDOWN:
                             iVscrollInc = max (1, cyClient / cyChar);
                             break; 
							 
                        case SB_THUMBPOSITION:
                             iVscrollInc = HIWORD (wParam) - iVscrollPos;
                             break;

                        default: 
                             iVscrollInc = 0;
                             break;
				 } 

                 iVscrollInc = max (-iVscrollPos, min (iVscrollInc, iVscrollMax - iVscrollPos));
                 if (iVscrollInc != 0) {
                    ScrollWindow (hwnd, 0, -cyChar * iVscrollInc, NULL, NULL);
                    SetScrollPos (hwnd, SB_VERT, iVscrollPos, TRUE);
                    UpdateWindow (hwnd);
				 }
                 break;

            case WM_KEYDOWN:
				if ((wParam == VK_F7) && (fileToOpen) && (fileToOpen [0] != 0)) {
                   result = Makefile (hwnd, fileToOpen);
                   if (result == FATAL_EXIT_CODE)
                      MessageBox (hwnd, "Build process aborted because of errors", "Thot compilers", MB_OK | MB_ICONERROR);
                  else 
                      MessageBox (hwnd, "You can now build the Amaya application", "Thot compilers", MB_OK | MB_ICONINFORMATION);
				}
                break;

            case WM_COMMAND:
                 switch (LOWORD (wParam)) {
                        case OPEN: 
                             OpenFileName.lStructSize       = sizeof (OPENFILENAME); 
                             OpenFileName.hwndOwner         = hwnd; 
                             OpenFileName.hInstance         = (HINSTANCE) GetWindowLong (hwnd, GWL_HINSTANCE); 
                             OpenFileName.lpstrFilter       = szFilter; 
                             OpenFileName.lpstrCustomFilter = (LPTSTR) NULL; 
                             OpenFileName.nMaxCustFilter    = 0L; 
                             OpenFileName.nFilterIndex      = 1L; 
                             OpenFileName.lpstrFile         = szFileName; 
                             OpenFileName.nMaxFile          = 256; 
                             OpenFileName.lpstrInitialDir   = NULL; 
                             OpenFileName.lpstrTitle        = "Open a File"; 
                             OpenFileName.nFileOffset       = 0; 
                             OpenFileName.nFileExtension    = 0; 
                             OpenFileName.lpstrDefExt       = "*.html"; 
                             OpenFileName.lCustData         = 0; 
                             OpenFileName.Flags             = OFN_SHOWHELP | OFN_HIDEREADONLY; 
  
                             if (GetOpenFileName (&OpenFileName)) {
                                strcpy (fileToOpen, OpenFileName.lpstrFile);
                                EnableMenuItem (popupMenu, COMPILE, MFS_ENABLED);
							 }
                             break;

                        case COMPILE: 
                             result = Makefile (hwnd, fileToOpen);
                             if (result == FATAL_EXIT_CODE)
                                MessageBox (hwnd, "Build process aborted because of errors", "Thot compilers", MB_OK | MB_ICONERROR);
                
                             else 
                                 MessageBox (hwnd, "You can now build the Amaya application", "Thot compilers", MB_OK | MB_ICONINFORMATION);
                             break;

                        case QUIT: SendMessage (hwnd, WM_CLOSE, 0, 0);
                             break;
				 }
	 }
     
     return DefWindowProc (hwnd, iMsg, wParam, lParam);
}
#endif /* _WINDOWS */