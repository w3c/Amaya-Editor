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
/* #include "compilers_f.h" */

#define OPEN    100
#define COMPILE 101
#define QUIT    102

#if defined(_I18N_) || defined(__JIS__)
#define NEW_LINE L'\n'
static STRING szFilter = L"Amaya Makefiles (*.mkf)\0*.mkf\0All files (*.*)\0*.*\0";
#else
#define NEW_LINE '\n'
static STRING szFilter = "Amaya Makefiles (*.mkf)\0*.mkf\0All files (*.*)\0*.*\0";
#endif

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
STRING cmdLine;
STRING SrcPath;
STRING DestPath;
STRING WorkPath;
STRING ThotPath;
STRING currentFile;
STRING currentDestFile;
STRING BinFiles [100];

#ifdef __STDC__
extern STRING TtaAllocString (unsigned int);
extern void   TtaFreeMemory (void*);
#else  /* !__STDC__ */
extern STRING TtaAllocString ();
extern void   TtaFreeMemory ();
#endif /* !__STDC__ */

#ifdef __STDC__
LRESULT CALLBACK CompilersWndProc (HWND, UINT, WPARAM, LPARAM);
#else  /* !__STDC__ */
LRESULT CALLBACK CompilersWndProc ();
#endif /* __STDC__ */

static OPENFILENAME OpenFileName;
static CHAR_T       szFileName[256];
static CHAR_T       fileToOpen [256];
static int          iVscrollPos = 0, iVscrollMax, iVscrollInc; 

#if 0
#ifdef __STDC__
int WINAPI DllMain (HINSTANCE hInstance, DWORD fdwReason, PVOID pvReserved) 
#else  /* __STDC__ */
int WINAPI DllMain (hInstance, fdwReason, pvReserved) 
HINSTANCE hInstance; 
DWORD     fdwReason; 
PVOID     pvReserved;
#endif /* __STDC__ */
{
    return TRUE;
}
#endif /* 0000000 */

#ifdef __STDC__
void MakeMessage (HWND hwnd, STRING errorMsg, int msgType)
#else  /* !__STDC__ */
void MakeMessage (hwnd, errorMsg, msgType)
HWND   hwnd;
STRING errorMsg;
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
      /* hFont = CreateFont (16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, 
                          OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, 
                          DEFAULT_PITCH | FF_DONTCARE, "Times New Roman"); */
      hFont = CreateFont (16, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, 
                          OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, 
                          DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"));

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
      if (!TextOut (hDC, 5, Y, errorMsg, ustrlen (errorMsg)))
         MessageBox (NULL, TEXT("Error Writing text"), TEXT("Thot Compilers"), MB_OK);

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
    CHAR_T errorMsg [800];
	int   c;

    if ((srcFile = fopen (src, "r")) == NULL) {
       usprintf (errorMsg, TEXT("Error: Can't open file: %s"), ISO2WideChar (src));
       MakeMessage (hwnd, errorMsg, FATAL_EXIT_CODE);
       return FATAL_EXIT_CODE;
	}
    if ((destFile = fopen (dest, "w")) == NULL) {
       usprintf (errorMsg, TEXT("Error: Can't open file: %s"), ISO2WideChar (dest));
       MakeMessage (hwnd, errorMsg, FATAL_EXIT_CODE);
       return FATAL_EXIT_CODE;
	}

    while ((c = getc (srcFile)) != EOF)
          putc (c, destFile);

    fclose (srcFile);
    fclose (destFile);
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
    static CHAR_T argv0[256];
    char*         ptr     = commandLine;
    char         lookFor = 0;

    enum {
         nowAt_start, 
         nowAt_text
    } nowAt;

    *pArgv = argv;
    argc   = 0;
    GetModuleFileName (hInst, argv0, sizeof (argv0));
    argv[argc++] = WideChar2ISO (argv0);
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
    CHAR_T  msg [1024];
    char    seps[]   = " \t=$()\n\r";
	char    string [1024];
    STRING  args [100];
	char*   token;
    char*   pChar;
    char*   ptr;
    STRING  SrcFileName;
    STRING  WorkFileName;
    STRING  currentFileName;
	int     command;
    int     index, i;
    int     indexBinFiles = 0;
    int     len;
    int     line = 1;
    int     result = COMP_SUCCESS;

    if (hwnd) {
       if ((f = fopen (fileName, "r")) == NULL) {
          usprintf (msg, TEXT("Cannot open file %s"), fileToOpen);
          MessageBox (hwnd, msg, TEXT("Make error"), MB_OK | MB_ICONWARNING);
	   } else {
              while (!feof (f)) {
                    if (currentFile) {
                       TtaFreeMemory (currentFile);
                       currentFile = (STRING) 0;
					} 
                    if (currentDestFile) {
                       TtaFreeMemory (currentDestFile);
                       currentDestFile = (STRING) 0;
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
                               usprintf (msg, TEXT("Line %d: unknown command %s"), line, token);
                               MakeMessage (hwnd, msg, FATAL_EXIT_CODE);				   
						  } 
					   }  
                       if (command != ERROR_CMD) {
                          index = 0;
                          args [index] = TtaAllocString (ustrlen (cmdLine) + 1);
                          ustrcpy (args [index++], cmdLine);
                          while ((token = strtok (NULL, seps)) != NULL) {
                                /* While there are tokens in "string" */
                                ptr = strrchr(token, '.');
                                if (ptr || (token [0] != '-')) {
                                   if (!currentFile) {
                                      currentFile = TtaAllocString (strlen (token) + 1);
                                      ustrcpy (currentFile, ISO2WideChar (token));
								   } else {
                                          currentDestFile = TtaAllocString (strlen (token) + 1);
                                          ustrcpy (currentDestFile, ISO2WideChar (token));
								   }
								} 
                                args [index] = TtaAllocString (strlen (token) + 1);
                                ustrcpy (args [index++], ISO2WideChar (token));
                                /* Get next token: */
                                /* token = strtok (NULL, seps); */
						  }
                          switch (command) {
                                 case SRC_DIR: 
                                      if (SrcPath) {
                                         TtaFreeMemory (SrcPath);
                                         SrcPath = (STRING) 0;
									  } 
                                      if (!ustrcmp (args [1], TEXT("THOTDIR"))) {
                                         if (index > 2) {
                                            SrcPath = TtaAllocString (ustrlen (ThotPath) + ustrlen (args [2]) + 1);
                                            ustrcpy (SrcPath, ThotPath);
                                            ustrcat (SrcPath, args [2]);
										 } else {
                                                SrcPath = TtaAllocString (ustrlen (ThotPath) + 1);
                                                ustrcpy (SrcPath, ThotPath);
										 }
									  }
                                      for (i = 0; i < index; i++) {
                                          TtaFreeMemory (args [i]);
                                          args [i] = (STRING) 0;
									  }
                                      break;

                                 case DEST_DIR: 
                                      if (DestPath) {
                                         TtaFreeMemory (DestPath);
                                         DestPath = (STRING) 0;
									  } 
                                      if (!ustrcmp (args [1], TEXT("THOTDIR"))) {
                                         if (index > 2) {
                                            DestPath = TtaAllocString (ustrlen (ThotPath) + ustrlen (args [2]) + 1);
                                            ustrcpy (DestPath, ThotPath);
                                            ustrcat (DestPath, args [2]);
										 } else {
                                                DestPath = TtaAllocString (ustrlen (ThotPath) + 1);
                                                ustrcpy (DestPath, ThotPath);
										 }
									  }
                                      for (i = 0; i < index; i++) {
                                          TtaFreeMemory (args [i]);
                                          args [i] = (STRING) 0;
									  }
                                      break;

                                 case APP: 
                                      hLib = LoadLibrary (TEXT("app"));
                                      if (!hLib)
                                         return FATAL_EXIT_CODE;
                                      ptrMainProc = GetProcAddress (hLib, "APPmain");
                                      if (!ptrMainProc) {
                                         FreeLibrary (hLib);
                                         return FATAL_EXIT_CODE;
									  }

                                      ptr = WideChar2ISO (ustrrchr(currentFile, '.'));
                                      if (ptr) {
                                         len = ustrlen (SrcPath);
                                         if (SrcPath [len - 1] == '\\') {
                                            SrcFileName = TtaAllocString (len + ustrlen (currentFile) + 1);
                                            usprintf (SrcFileName, TEXT("%s%s"), SrcPath, currentFile);
										 } else {
                                                SrcFileName = TtaAllocString (len + ustrlen (currentFile) + 2);
                                                usprintf (SrcFileName, TEXT("%s\\%s"), SrcPath, currentFile);
										 }
                                         len = ustrlen (WorkPath);
                                         if (WorkPath [len - 1] == '\\') {
                                            WorkFileName = TtaAllocString (len + ustrlen (currentFile) + 1);
                                            usprintf (WorkFileName, TEXT("%s%s"), WorkPath, currentFile);
										 } else {
                                                WorkFileName = TtaAllocString (len + ustrlen (currentFile) + 2);
                                                usprintf (WorkFileName, TEXT("%s\\%s"), WorkPath, currentFile);
										 } 
									  } else {
                                             len = ustrlen (SrcPath);
                                             if (SrcPath [len - 1] == '\\') {
                                                SrcFileName = TtaAllocString (len + ustrlen (currentFile) + 3);
                                                usprintf (SrcFileName, TEXT("%s%s.A"), SrcPath, currentFile);
											 } else {
                                                    SrcFileName = TtaAllocString (len + ustrlen (currentFile) + 3);
                                                    usprintf (SrcFileName, TEXT("%s\\%s.A"), SrcPath, currentFile);
											 }
                                             len = ustrlen (WorkPath);
                                             if (WorkPath [len - 1] == '\\') {
                                                WorkFileName = TtaAllocString (len + ustrlen (currentFile) + 3);
                                                usprintf (WorkFileName, TEXT("%s%s.A"), WorkPath, currentFile);
											 } else {
                                                    WorkFileName = TtaAllocString (len + ustrlen (currentFile) + 4);
                                                    usprintf (WorkFileName, TEXT("%s\\%s.A"), WorkPath, currentFile);
											 }  
									  } 

                                      Copy_File (hwnd, WideChar2ISO (SrcFileName), WideChar2ISO (WorkFileName));

                                      /* result = APPmain (hwnd, index, args, &Y); */
                                      result = ptrMainProc (hwnd, index, args, &Y);
                                      FreeLibrary (hLib);
                                      for (i = 0; i < index; i++) {
                                          TtaFreeMemory (args [i]);
                                          args [i] = (STRING) 0;
									  }
                                      if (currentFile) {
                                         TtaFreeMemory (currentFile);
                                         currentFile = (STRING) 0;
									  }
                                      if (SrcFileName) {
                                         TtaFreeMemory (SrcFileName);
                                         SrcFileName = (STRING) 0;
									  }
                                      if (WorkFileName) {
                                         uunlink (WorkFileName);
                                         TtaFreeMemory (WorkFileName);
                                         WorkFileName = (STRING) 0;
									  }
                                      if (result == FATAL_EXIT_CODE)
                                         return result;
                                      break;

                                 case PRS: 
                                      hLib = LoadLibrary (TEXT("prs"));
                                      if (!hLib)
                                         return FATAL_EXIT_CODE;
                                      ptrMainProc = GetProcAddress (hLib, "PRSmain");
                                      if (!ptrMainProc) {
                                         FreeLibrary (hLib);
                                         return FATAL_EXIT_CODE;
									  }

                                      ptr = WideChar2ISO (ustrrchr(currentFile, '.'));
                                      if (ptr) {
                                         len = ustrlen (SrcPath);
                                         if (SrcPath [len - 1] == '\\') {
                                            SrcFileName = TtaAllocString (len + ustrlen (currentFile) + 1);
                                            usprintf (SrcFileName, TEXT("%s%s"), SrcPath, currentFile);
										 } else {
                                                SrcFileName = TtaAllocString (len + ustrlen (currentFile) + 2);
                                                usprintf (SrcFileName, TEXT("%s\\%s"), SrcPath, currentFile);
										 }
                                         len = ustrlen (WorkPath);
                                         if (WorkPath [len - 1] == '\\') {
                                            WorkFileName = TtaAllocString (len + ustrlen (currentFile) + 1);
                                            usprintf (WorkFileName, TEXT("%s%s"), WorkPath, currentFile);
										 } else {
                                                WorkFileName = TtaAllocString (len + ustrlen (currentFile) + 2);
                                                usprintf (WorkFileName, TEXT("%s\\%s"), WorkPath, currentFile);
										 } 
									  } else {
                                             len = ustrlen (SrcPath);
                                             if (SrcPath [len - 1] == '\\') {
                                                SrcFileName = TtaAllocString (len + ustrlen (currentFile) + 3);
                                                usprintf (SrcFileName, TEXT("%s%s.P"), SrcPath, currentFile);
											 } else {
                                                    SrcFileName = TtaAllocString (len + ustrlen (currentFile) + 4);
                                                    usprintf (SrcFileName, TEXT("%s\\%s.P"), SrcPath, currentFile);
											 }
                                             len = ustrlen (WorkPath);
                                             if (WorkPath [len - 1] == '\\') {
                                                WorkFileName = TtaAllocString (len + ustrlen (currentFile) + 3);
                                                usprintf (WorkFileName, TEXT("%s%s.P"), WorkPath, currentFile);
											 } else {
                                                    WorkFileName = TtaAllocString (len + ustrlen (currentFile) + 4);
                                                    usprintf (WorkFileName, TEXT("%s\\%s.P"), WorkPath, currentFile);
											 }  
									  } 

                                      if (currentDestFile) {
                                         ptr = WideChar2ISO (ustrrchr (currentDestFile, '.'));
                                         if (ptr) {
                                            BinFiles [indexBinFiles] = TtaAllocString (ustrlen (currentDestFile) + 1);
                                            ustrcpy (BinFiles [indexBinFiles], currentDestFile);
										 } else {
                                                BinFiles [indexBinFiles] = TtaAllocString (ustrlen (currentDestFile) + 4);
                                                usprintf (BinFiles [indexBinFiles], TEXT("%s.PRS"), currentDestFile);
										 }
									  } else {
                                             currentFileName = TtaAllocString (ustrlen (currentFile) + 1);
                                             ustrcpy (currentFileName, currentFile);
                                             ptr = WideChar2ISO (ustrrchr (currentFileName, '.'));
                                             if (ptr)
                                                ptr [0] = 0;
                                             BinFiles [indexBinFiles] = TtaAllocString (ustrlen (currentFile) + 4);
                                             usprintf (BinFiles [indexBinFiles], TEXT("%s.PRS"), currentFile);
									  }

                                      Copy_File (hwnd, WideChar2ISO (SrcFileName), WideChar2ISO (WorkFileName));

                                      result = ptrMainProc (hwnd, index, args, &Y);
                                      FreeLibrary (hLib);
                                      /* result = PRSmain (hwnd, index, args, &Y); */
                                      for (i = 0; i < index; i++) {
                                          TtaFreeMemory (args [i]);
                                          args [i] = (STRING) 0;
									  }
                                      if (currentFile) {
                                         TtaFreeMemory (currentFile);
                                         currentFile = (STRING) 0;
									  }
                                      if (SrcFileName) {
                                         TtaFreeMemory (SrcFileName);
                                         SrcFileName = (STRING) 0;
									  }
                                      if (WorkFileName) {
                                         uunlink (WorkFileName);
                                         TtaFreeMemory (WorkFileName);
                                         WorkFileName = (STRING) 0;
									  }

                                      if (result == FATAL_EXIT_CODE)
                                         return result;
                                      len = ustrlen (WorkPath);
                                      if (WorkPath [len - 1] == '\\') {
                                         SrcFileName = TtaAllocString (len + ustrlen (BinFiles [indexBinFiles]) + 1);
                                         usprintf (SrcFileName, TEXT("%s%s"), WorkPath, BinFiles [indexBinFiles]);
									  } else {
                                             SrcFileName = TtaAllocString (len + ustrlen (BinFiles [indexBinFiles]) + 2);
                                             usprintf (SrcFileName, TEXT("%s\\%s"), WorkPath, BinFiles [indexBinFiles]);
									  }  
                                      len = ustrlen (SrcPath);
                                      if (SrcPath [len - 1] == '\\') {
                                          WorkFileName = TtaAllocString (len + ustrlen (BinFiles [indexBinFiles]) + 1);
                                          usprintf (WorkFileName, TEXT("%s%s"), SrcPath, BinFiles [indexBinFiles]);
									  } else { 
                                             WorkFileName = TtaAllocString (len + ustrlen (BinFiles [indexBinFiles]) + 2);
                                             usprintf (WorkFileName, TEXT("%s\\%s"), SrcPath, BinFiles [indexBinFiles]);
									  } 
                                      Copy_File (hwnd, WideChar2ISO (SrcFileName), WideChar2ISO (WorkFileName));
                                      indexBinFiles++;
                                      break;

                                 case STR: 
                                      hLib = LoadLibrary (TEXT("str"));
                                      if (!hLib)
                                         return FATAL_EXIT_CODE;
                                      ptrMainProc = GetProcAddress (hLib, "STRmain");
                                      if (!ptrMainProc) {
                                         FreeLibrary (hLib);
                                         return FATAL_EXIT_CODE;
									  }

                                      ptr = WideChar2ISO (ustrrchr(currentFile, '.'));
                                      if (ptr) {
                                         len = ustrlen (SrcPath);
                                         if (SrcPath [len - 1] == '\\') {
                                            SrcFileName = TtaAllocString (len + ustrlen (currentFile) + 1);
                                            usprintf (SrcFileName, TEXT("%s%s"), SrcPath, currentFile);
										 } else {
                                                SrcFileName = TtaAllocString (len + ustrlen (currentFile) + 2);
                                                usprintf (SrcFileName, TEXT("%s\\%s"), SrcPath, currentFile);
										 }
                                         len = ustrlen (WorkPath);
                                         if (WorkPath [len - 1] == '\\') {
                                            WorkFileName = TtaAllocString (len + ustrlen (currentFile) + 1);
                                            usprintf (WorkFileName, TEXT("%s%s"), WorkPath, currentFile);
										 } else {
                                                WorkFileName = TtaAllocString (len + ustrlen (currentFile) + 2);
                                                usprintf (WorkFileName, TEXT("%s\\%s"), WorkPath, currentFile);
										 } 
									  } else {
                                             len = ustrlen (SrcPath);
                                             if (SrcPath [len - 1] == '\\') {
                                                SrcFileName = TtaAllocString (len + ustrlen (currentFile) + 3);
                                                usprintf (SrcFileName, TEXT("%s%s.S"), SrcPath, currentFile);
											 } else {
                                                    SrcFileName = TtaAllocString (len + ustrlen (currentFile) + 4);
                                                    usprintf (SrcFileName, TEXT("%s\\%s.S"), SrcPath, currentFile);
											 }
                                             len = ustrlen (WorkPath);
                                             if (WorkPath [len - 1] == '\\') {
                                                WorkFileName = TtaAllocString (len + ustrlen (currentFile) + 3);
                                                usprintf (WorkFileName, TEXT("%s%s.S"), WorkPath, currentFile);
											 } else {
                                                    WorkFileName = TtaAllocString (len + ustrlen (currentFile) + 4);
                                                    usprintf (WorkFileName, TEXT("%s\\%s.S"), WorkPath, currentFile);
											 }  
									  } 

                                      if (currentDestFile) {
                                         ptr = WideChar2ISO (ustrrchr (currentDestFile, '.'));
                                         if (ptr) {
                                            BinFiles [indexBinFiles] = TtaAllocString (ustrlen (currentDestFile) + 1);
                                            ustrcpy (BinFiles [indexBinFiles], currentDestFile);
										 } else {
                                                BinFiles [indexBinFiles] = TtaAllocString (ustrlen (currentDestFile) + 4);
                                                usprintf (BinFiles [indexBinFiles], TEXT("%s.STR"), currentDestFile);
										 }
									  } else {
                                             currentFileName = TtaAllocString (ustrlen (currentFile) + 1);
                                             ustrcpy (currentFileName, currentFile);
                                             ptr = WideChar2ISO (ustrrchr (currentFileName, '.'));
                                             if (ptr)
                                                ptr [0] = 0;
                                             BinFiles [indexBinFiles] = TtaAllocString (ustrlen (currentFile) + 4);
                                             usprintf (BinFiles [indexBinFiles], TEXT("%s.STR"), currentFile);
									  }

                                      Copy_File (hwnd, WideChar2ISO (SrcFileName), WideChar2ISO (WorkFileName));

                                      result = ptrMainProc (hwnd, index, args, &Y);
                                      FreeLibrary (hLib);
                                      /* result = STRmain (hwnd, index, args, &Y); */
                                      for (i = 0; i < index; i++) {
                                          TtaFreeMemory (args [i]);
                                          args [i] = (STRING) 0;
									  }
                                      if (currentFile) {
                                         TtaFreeMemory (currentFile);
                                         currentFile = (STRING) 0;
									  }
                                      if (SrcFileName) {
                                         TtaFreeMemory (currentFile);
                                         currentFile = (STRING) 0;
									  }
                                      if (WorkFileName) {
                                         TtaFreeMemory (currentFile);
                                         currentFile = (STRING) 0;
									  }
                                      uunlink (WorkFileName);
                                      if (result == FATAL_EXIT_CODE)
                                         return result;
                                      len = ustrlen (WorkPath);
                                      if (WorkPath [len - 1] == '\\') {
                                         SrcFileName = TtaAllocString (len + ustrlen (BinFiles [indexBinFiles]) + 1);
                                         usprintf (SrcFileName, TEXT("%s%s"), WorkPath, BinFiles [indexBinFiles]);
									  } else {
                                             SrcFileName = TtaAllocString (len + ustrlen (BinFiles [indexBinFiles]) + 2);
                                             usprintf (SrcFileName, TEXT("%s\\%s"), WorkPath, BinFiles [indexBinFiles]);
									  }  
                                      len = ustrlen (SrcPath);
                                      if (SrcPath [len - 1] == '\\') {
                                          WorkFileName = TtaAllocString (len + ustrlen (BinFiles [indexBinFiles]) + 1);
                                          usprintf (WorkFileName, TEXT("%s%s"), SrcPath, BinFiles [indexBinFiles]);
									  } else { 
                                             WorkFileName = TtaAllocString (len + ustrlen (BinFiles [indexBinFiles]) + 2);
                                             usprintf (WorkFileName, TEXT("%s\\%s"), SrcPath, BinFiles [indexBinFiles]);
									  } 
                                      Copy_File (hwnd, WideChar2ISO (SrcFileName), WideChar2ISO (WorkFileName));
                                      indexBinFiles++;
                                      break;

                                 case TRA: 
                                      hLib = LoadLibrary (TEXT("tra"));
                                      if (!hLib)
                                         return FATAL_EXIT_CODE;

                                      ptrMainProc = GetProcAddress (hLib, "TRAmain");
                                      if (!ptrMainProc) {
                                         FreeLibrary (hLib);
                                         return FATAL_EXIT_CODE;
									  }

                                      len = ustrlen (SrcPath);
                                      if (SrcPath [len - 1] == '\\') {
                                         SrcFileName = TtaAllocString (len + 12);
                                         usprintf (SrcFileName, TEXT("%sgreek.sgml"), SrcPath);
									  } else {
                                             SrcFileName = TtaAllocString (len + 13);
                                             usprintf (SrcFileName, TEXT("%s\\greek.sgml"), SrcPath);
									  }

                                      len = ustrlen (WorkPath);
                                      if (WorkPath [len - 1] == '\\') {
                                         WorkFileName = TtaAllocString (len + 12);
                                         usprintf (WorkFileName, TEXT("%sgreek.sgml"), WorkPath);
									  } else {
                                             WorkFileName = TtaAllocString (len + 13);
                                             usprintf (WorkFileName, TEXT("%s\\greek.sgml"), WorkPath);
									  }

                                      Copy_File (hwnd, WideChar2ISO (SrcFileName), WideChar2ISO (WorkFileName));

                                      len = ustrlen (SrcPath);
                                      if (SrcPath [len - 1] == '\\') {
                                         SrcFileName = TtaAllocString (len + 14);
                                         usprintf (SrcFileName, TEXT("%sText_SGML.inc"), SrcPath);
									  } else {
                                             SrcFileName = TtaAllocString (len + 15);
                                             usprintf (SrcFileName, TEXT("%s\\Text_SGML.inc"), SrcPath);
									  }

                                      len = ustrlen (WorkPath);
                                      if (WorkPath [len - 1] == '\\') {
                                         WorkFileName = TtaAllocString (len + 14);
                                         usprintf (WorkFileName, TEXT("%sText_SGML.inc"), WorkPath);
									  } else {
                                             WorkFileName = TtaAllocString (len + 15);
                                             usprintf (WorkFileName, TEXT("%s\\Text_SGML.inc"), WorkPath);
									  }

                                      Copy_File (hwnd, WideChar2ISO (SrcFileName), WideChar2ISO (WorkFileName));

                                      ptr = WideChar2ISO (ustrrchr(currentFile, '.'));
                                      if (ptr) {
                                         len = ustrlen (SrcPath);
                                         if (SrcPath [len - 1] == '\\') {
                                            SrcFileName = TtaAllocString (len + ustrlen (currentFile) + 1);
                                            usprintf (SrcFileName, TEXT("%s%s"), SrcPath, currentFile);
										 } else {
                                                SrcFileName = TtaAllocString (len + ustrlen (currentFile) + 2);
                                                usprintf (SrcFileName, TEXT("%s\\%s"), SrcPath, currentFile);
										 }
                                         len = ustrlen (WorkPath);
                                         if (WorkPath [len - 1] == '\\') {
                                            WorkFileName = TtaAllocString (len + ustrlen (currentFile) + 1);
                                            usprintf (WorkFileName, TEXT("%s%s"), WorkPath, currentFile);
										 } else {
                                                WorkFileName = TtaAllocString (len + ustrlen (currentFile) + 2);
                                                usprintf (WorkFileName, TEXT("%s\\%s"), WorkPath, currentFile);
										 } 
									  } else {
                                             if (SrcPath [len - 1] == '\\') {
                                                SrcFileName = TtaAllocString (len + ustrlen (currentFile) + 3);
                                                usprintf (SrcFileName, TEXT("%s%s.T"), SrcPath, currentFile);
											 } else {
                                                    SrcFileName = TtaAllocString (len + ustrlen (currentFile) + 4);
                                                    usprintf (SrcFileName, TEXT("%s\\%s.T"), SrcPath, currentFile);
											 }
                                             len = ustrlen (WorkPath);
                                             if (WorkPath [len - 1] == '\\') {
                                                WorkFileName = TtaAllocString (len + ustrlen (currentFile) + 3);
                                                usprintf (WorkFileName, TEXT("%s%s.T"), WorkPath, currentFile);
											 } else {
                                                    WorkFileName = TtaAllocString (len + ustrlen (currentFile) + 4);
                                                    usprintf (WorkFileName, TEXT("%s\\%s.T"), WorkPath, currentFile);
											 }  
									  } 

                                      if (currentDestFile) {
                                         ptr = WideChar2ISO (ustrrchr (currentDestFile, '.'));
                                         if (ptr) {
                                            BinFiles [indexBinFiles] = TtaAllocString (ustrlen (currentDestFile) + 1);
                                            ustrcpy (BinFiles [indexBinFiles], currentDestFile);
										 } else {
                                                BinFiles [indexBinFiles] = TtaAllocString (ustrlen (currentDestFile) + 4);
                                                usprintf (BinFiles [indexBinFiles], TEXT("%s.TRA"), currentDestFile);
										 }
									  } else {
                                             currentFileName = TtaAllocString (ustrlen (currentFile) + 1);
                                             ustrcpy (currentFileName, currentFile);
                                             ptr = WideChar2ISO (ustrrchr (currentFileName, '.'));
                                             if (ptr)
                                                ptr [0] = 0;
                                             BinFiles [indexBinFiles] = TtaAllocString (ustrlen (currentFile) + 4);
                                             usprintf (BinFiles [indexBinFiles], TEXT("%s.TRA"), currentFile);
									  }

                                      Copy_File (hwnd, WideChar2ISO (SrcFileName), WideChar2ISO (WorkFileName));

                                      result = ptrMainProc (hwnd, index, args, &Y);
                                      FreeLibrary (hLib);
                                      /* result = TRAmain (hwnd, index, args, &Y); */
                                      for (i = 0; i < index; i++) {
                                          TtaFreeMemory (args [i]);
                                          args [i] = (STRING) 0;
									  }
                                      if (currentFile) {
                                         TtaFreeMemory (currentFile);
                                         currentFile = (STRING) 0;
									  }
                                      if (SrcFileName) {
                                         TtaFreeMemory (currentFile);
                                         currentFile = (STRING) 0;
									  }
                                      if (WorkFileName) {
                                         TtaFreeMemory (currentFile);
                                         currentFile = (STRING) 0;
									  }
                                      uunlink (WorkFileName);
                                      if (result == FATAL_EXIT_CODE)
                                         return result;
                                      len = ustrlen (WorkPath);
                                      if (WorkPath [len - 1] == '\\') {
                                         SrcFileName = TtaAllocString (len + ustrlen (BinFiles [indexBinFiles]) + 1);
                                         usprintf (SrcFileName, TEXT("%s%s"), WorkPath, BinFiles [indexBinFiles]);
									  } else {
                                             SrcFileName = TtaAllocString (len + ustrlen (BinFiles [indexBinFiles]) + 2);
                                             usprintf (SrcFileName, TEXT("%s\\%s"), WorkPath, BinFiles [indexBinFiles]);
									  }  
                                      len = ustrlen (SrcPath);
                                      if (SrcPath [len - 1] == '\\') {
                                          WorkFileName = TtaAllocString (len + ustrlen (BinFiles [indexBinFiles]) + 1);
                                          usprintf (WorkFileName, TEXT("%s%s"), SrcPath, BinFiles [indexBinFiles]);
									  } else { 
                                             WorkFileName = TtaAllocString (len + ustrlen (BinFiles [indexBinFiles]) + 2);
                                             usprintf (WorkFileName, TEXT("%s\\%s"), SrcPath, BinFiles [indexBinFiles]);
									  } 
                                      Copy_File (hwnd, WideChar2ISO (SrcFileName), WideChar2ISO (WorkFileName));
                                      indexBinFiles++;
                                      break;

                                 default:
                                      for (i = 0; i < index; i++) {
                                          TtaFreeMemory (args [i]);
                                          args [i] = (STRING) 0;
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
                     SrcFileName = (STRING) 0;
				  }*/
                  len = ustrlen (WorkPath);
                  if (WorkPath [len - 1] == '\\') {
                      SrcFileName = TtaAllocString (len + ustrlen (BinFiles[i]) + 1);
                      usprintf (SrcFileName, TEXT("%s%s"), WorkPath, BinFiles [i]);
				  } else {
                         SrcFileName = TtaAllocString (len + ustrlen (BinFiles [i]) + 2);
                         usprintf (SrcFileName, TEXT("%s\\%s"), WorkPath, BinFiles [i]);
				  }
                  uunlink (SrcFileName);
			  }
              fclose (f);
	   }  
    }
    MakeMessage (hwnd, TEXT("Build process success ..."), COMP_SUCCESS);
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
#    if defined(_I18N_) || defined(__JIS__)
     static CHAR_T szAppName[] = L"ThotCompilers";
#    else /* defined(_I18N_) || defined(__JIS__) */
     static CHAR_T szAppName[] = "ThotCompilers";
#    endif /* defined(_I18N_) || defined(__JIS__) */
     HWND        hwnd;
     MSG         msg;
     WNDCLASSEX  wndClass;
     BOOL        ok;
     int         argc;
     char**      argv;
     STRING      dir_end;
     STRING      BinPath;

     argc = makeArgcArgv (hInstance, &argv, szCmdLine);
	 cmdLine = TtaAllocString (strlen (argv[0]) + 1);
	 ustrcpy (cmdLine, ISO2WideChar (argv [0]));
     TtaInitializeAppRegistry (cmdLine);

     BinPath = TtaGetEnvString (TEXT("PWD"));
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

     WorkPath = TtaAllocString (ustrlen (BinPath) + 7);
     usprintf (WorkPath, TEXT("%s\\amaya"), BinPath);

     ThotPath = TtaGetEnvString (TEXT("THOTDIR"));

     wndClass.style         = CS_HREDRAW | CS_VREDRAW ;
     wndClass.lpfnWndProc   = (WNDPROC) CompilersWndProc ;
     wndClass.cbClsExtra    = 0 ;
     wndClass.cbWndExtra    = 0 ;
     wndClass.hInstance     = hInstance ;
     wndClass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
     wndClass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
     wndClass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
     wndClass.lpszMenuName  = NULL ;
     wndClass.lpszClassName = szAppName ;
     wndClass.cbSize        = sizeof(WNDCLASSEX);
     wndClass.hIconSm       = LoadIcon (hInstance, IDI_APPLICATION) ;

     if (!RegisterClassEx (&wndClass))
        return FALSE;

     hwnd = CreateWindowEx (WS_EX_STATICEDGE | WS_EX_OVERLAPPEDWINDOW | WS_EX_DLGMODALFRAME, szAppName, TEXT("Thot compilers"),
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
                 AppendMenu (popupMenu, MF_STRING, OPEN, TEXT("Open"));
                 AppendMenu (popupMenu, MF_STRING, COMPILE, TEXT("Build"));
                 AppendMenu (popupMenu, MF_SEPARATOR, 0, NULL);
                 AppendMenu (popupMenu, MF_STRING, QUIT, TEXT("Quit"));
                 AppendMenu (menuBar, MF_POPUP, (UINT)popupMenu, TEXT("File"));
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
                             OpenFileName.lpstrTitle        = TEXT ("Open a File"); 
                             OpenFileName.nFileOffset       = 0; 
                             OpenFileName.nFileExtension    = 0; 
                             OpenFileName.lpstrDefExt       = TEXT ("*.html"); 
                             OpenFileName.lCustData         = 0; 
                             OpenFileName.Flags             = OFN_SHOWHELP | OFN_HIDEREADONLY; 
  
                             if (GetOpenFileName (&OpenFileName)) {
                                ustrcpy (fileToOpen, OpenFileName.lpstrFile);
                                EnableMenuItem (popupMenu, COMPILE, MFS_ENABLED);
							 }
                             break;

                        case COMPILE: 
                             result = Makefile (hwnd, WideChar2ISO (fileToOpen));
                             if (result == FATAL_EXIT_CODE)
                                MessageBox (hwnd, TEXT("Build process aborted because of errors"), TEXT("Thot compilers"), MB_OK | MB_ICONERROR);
                
                             else 
                                 MessageBox (hwnd, TEXT("You can now build the Amaya application"), TEXT("Thot compilers"), MB_OK | MB_ICONINFORMATION);
                             break;

                        case QUIT: SendMessage (hwnd, WM_CLOSE, 0, 0);
                             break;
				 }
	 }
     
     return DefWindowProc (hwnd, iMsg, wParam, lParam);
}
#endif /* _WINDOWS */