/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2003
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * Messages and printing management.
 */

#include "thot_sys.h"
#include "libmsg.h"
#include "message.h"
#include "constmenu.h"
#include "constmedia.h"
#include "typemedia.h"
#include "language.h"
#include "constmenu.h"
#include "document.h"
#include "dialog.h"
#include "appdialogue.h"
#include "frame.h"
#include "fileaccess.h"
#include "application.h"
#include "print.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "platform_tv.h"
#include "edit_tv.h"
#include "appdialogue_tv.h"
#include "frame_tv.h"
#include "print_tv.h"

#include "actions_f.h"
#include "appli_f.h"
#include "applicationapi_f.h"
#include "attrmenu_f.h"
#include "browser_f.h"
#include "config_f.h"
#include "creationmenu_f.h"
#include "dialogapi_f.h"
#include "docs_f.h"
#include "exceptions_f.h"
#include "memory_f.h"
#include "presentmenu_f.h"
#include "printmenu_f.h"
#include "registry_f.h"
#include "search_f.h"
#include "searchmenu_f.h"
#include "selectmenu_f.h"
#include "structcreation_f.h"
#include "structmodif_f.h"
#include "structselect_f.h"
#include "textcommands_f.h"
#include "thotmsg_f.h"
#include "tree_f.h"
#include "views_f.h"
#include "viewapi_f.h"
#include "writedoc_f.h"
#ifdef _WINDOWS
#include "thotprinter_f.h"
#include "wininclude.h"
static PRINTDLG     Pdlg;
static ThotBool     LpInitialized = FALSE;
#endif /* _WINDOWS */


static PathBuffer   PrintDirName;
static Name         PrintDocName;
static char       Orientation[MAX_NAME_LENGTH];
static Func         pFuncExportPrintDoc = NULL;
static int          defPaperPrint;
static int          defManualFeed;
static int          defFirstPage;
static int          defLastPage;
static int          defNbCopies;
static int          defReduction;
static int          defPagesPerSheet;
static int          defPaginate;
static Name         PresSchema;


/*----------------------------------------------------------------------
  Print: interface to the Print program.
  ----------------------------------------------------------------------*/
static void Print (char *name, char *dir, char *thotSch, char *thotDoc,
		   char *realName, char *output, int firstPage, int lastPage,
		   int nCopies, int hShift, int vShift, int userOrientation,
		   int reduction, int nbPagesPerSheet, int suppFrame,
		   int manualFeed, int blackAndWhite, int repaginate,
		   char *viewsToPrint, char *cssToPrint, Document document)
{
   char                   *ptr;

#ifdef _WINDOWS_DLL
   HINSTANCE               hLib;
   /* FARPROC                 ptrMainProc; */
   typedef void (*MYPROC)(HWND, int, char **, HDC, ThotBool,
			      int, char *, char *, HINSTANCE, ThotBool);
   MYPROC  ptrMainProc;

   char                    tmp[MAX_TXT_LEN];
   char                   *printArgv [100];
   int                     printArgc = 0;
#else /* _WINDOWS_DLL */
   char                    cmd[1024];
#ifndef _WINDOWS
   int                     res;
#endif /*_WINDOWS*/
#endif /* _WINDOWS_DLL */
   int                     i, j = 0;
   int                     frame;

   /* initialize the print command */
   ptr = TtaGetEnvString ("LANG");
#ifdef _WINDOWS_DLL
   printArgv[printArgc] = TtaGetMemory (strlen (BinariesDirectory) + 7);
   strcpy (printArgv[printArgc], BinariesDirectory);
   strcat (printArgv[printArgc], DIR_STR);
   strcat (printArgv[printArgc], "print");
   printArgc++;   
   printArgv[printArgc] = TtaGetMemory (6) ;
   strcpy (printArgv[printArgc], "-lang");
   printArgc++;
   printArgv[printArgc] = TtaGetMemory (strlen (ptr) + 1);
   strcpy (printArgv[printArgc], ptr);
   printArgc++;
#else /* !_WINDOWS_DLL */
#ifdef _WINDOWS
   strcpy (cmd, "thotprinter.exe"); 
#else
   sprintf (cmd, "%s/print", BinariesDirectory); 
#endif /* _WINDOWS */
   strcat (cmd, " -lang ");
   strcat (cmd, ptr);
#endif /* !_WINDOWS_DLL */

   if (thotSch != NULL && thotSch[0] != EOS)
     {
#ifdef _WINDOWS_DLL
       printArgv[printArgc] = TtaGetMemory (5) ;
       strcpy (printArgv[printArgc], "-sch");
       printArgc++;
       printArgv[printArgc] = TtaGetMemory (strlen (thotSch) + 1);
       strcpy (printArgv[printArgc], thotSch);
       printArgc++;
       printArgv[printArgc] = TtaGetMemory (5);
       strcpy (printArgv[printArgc], "-doc");
       printArgc++;
       printArgv[printArgc] = TtaGetMemory (strlen (thotDoc) + 1);
       strcpy (printArgv[printArgc], thotDoc);
       printArgc++;
#else /* !_WINDOWS */
       strcat (cmd, " -sch ");
       strcat (cmd, thotSch);
       strcat (cmd, " -doc ");
       strcat (cmd, thotDoc);
#endif /* _WINDOWS */
     }

   /* transmit the server name */
   if (servername && servername[0] != EOS)
     { 
#ifdef _WINDOWS_DLL
       printArgv[printArgc] = TtaGetMemory (9);
       strcpy (printArgv[printArgc], "-display");
       printArgc++;
       printArgv[printArgc] = TtaGetMemory (strlen (servername) + 1);
       strcpy (printArgv[printArgc], servername);
       printArgc++;
#else /* !_WINDOWS */
       strcat (cmd, " -display ");
       strcat (cmd, servername);
#endif /* _WINDOWS */
     }

   /* transmit the document name */
   if (realName)
     { 
#ifdef _WINDOWS_DLL
       printArgv[printArgc] = TtaGetMemory (6);
       strcpy (printArgv[printArgc], "-name");
       printArgc++;
       printArgv[printArgc] = TtaGetMemory (strlen (realName) + 10);
       strcpy (printArgv[printArgc], realName);
       printArgc++;
#else /* _WINDOWS_DLL */
       strcat (cmd, " -name ");
       strcat (cmd, realName);
#endif /* _WINDOWS */
     }

   /* transmit the orientation (default value is portrait) */
   if (userOrientation != 0)
     {
#ifdef _WINDOWS_DLL
       printArgv[printArgc] = TtaGetMemory (11);
       strcpy (printArgv[printArgc], "-landscape");
       printArgc++;
#else /* _WINDOWS_DLL */
       strcat (cmd, " -landscape");
#endif /* _WINDOWS */
     }

   /* transmit the output command */
   if (PaperPrint)
     {
#ifdef _WINDOWS_DLL
       printArgv[printArgc] = TtaGetMemory (5);
       strcpy (printArgv[printArgc], "-out");
       printArgc++;
#else /* _WINDOWS_DLL */
       strcat (cmd, " -out \"");
#endif /* _WINDOWS */
     }
   else
     {
#ifdef _WINDOWS_DLL
       printArgv[printArgc] = TtaGetMemory (4);
       strcpy (printArgv[printArgc], "-ps");
       printArgc++;
#else /* _WINDOWS_DLL */
       strcat (cmd, " -ps \"");
#endif /* _WINDOWS */
     }

   if (output[0] != EOS)
     {
#ifdef _WINDOWS_DLL
       printArgv[printArgc] = TtaGetMemory (strlen (output) + 1);
       strcpy (printArgv[printArgc], output);
       printArgc++;
#else /* _WINDOWS_DLL */
       strcat (cmd, output);
       strcat (cmd, "\" ");
#endif /* _WINDOWS */
     }
   else
     {
#ifdef _WINDOWS_DLL
       printArgv[printArgc] = TtaGetMemory (3);
       strcpy (printArgv[printArgc], "lp");
       printArgc++;
#else /* _WINDOWS_DLL */
       strcat (cmd, "lp");
       strcat (cmd, "\" ");
#endif /* _WINDOWS */
     }

   /* transmit visualization of empty boxes (default no) */
   if (suppFrame == 0)
     {
#ifdef _WINDOWS_DLL
       printArgv[printArgc] = TtaGetMemory (10);
       strcpy (printArgv[printArgc], "-emptybox");
       printArgc++;
#else /* _WINDOWS_DLL */
       strcat (cmd, " -emptybox");
#endif /* _WINDOWS_DLL */;
     }

   /* transmit black/white output (default no) */
   if (blackAndWhite != 0)
     {
#ifdef _WINDOWS_DLL
       printArgv[printArgc] = TtaGetMemory (4);
       strcpy (printArgv[printArgc], "-bw");
       printArgc++;
#else /* _WINDOWS_DLL */
       strcat (cmd, " -bw");
#endif /* _WINDOWS_DLL */
     }

   /* transmit manualfeed (default no) */
   if (manualFeed != 0)
     {
#ifdef _WINDOWS_DLL
       printArgv[printArgc] = TtaGetMemory (12);
       strcpy (printArgv[printArgc], "-manualfeed");
       printArgc++;
#else /* _WINDOWS_DLL */
       strcat (cmd, " -manualfeed");
#endif /* _WINDOWS_DLL  */
     }

   /* transmit repaginate (default no) */
   if (repaginate != 0)
     {
#ifdef _WINDOWS_DLL
       printArgv[printArgc] = TtaGetMemory (10);
       strcpy (printArgv[printArgc], "-paginate");
       printArgc++;
#else /* _WINDOWS_DLL */
       strcat (cmd, " -paginate");
#endif /* _WINDOWS_DLL */
     }

   /* transmit page format */
   if (strcmp (PageSize, "A4"))
     {
#ifdef _WINDOWS_DLL
       sprintf (tmp, "-P%s", PageSize);
       printArgv[printArgc] = TtaGetMemory (strlen (PageSize) + 3);
       strcpy (printArgv[printArgc], tmp);
       printArgc++;
#else /* _WINDOWS_DLL */
       strcat (cmd, " -P");
       strcat (cmd, PageSize);
#endif /* _WINDOWS */
     }

   /* transmit window id */

#ifndef _WINDOWS_DLL
   i = strlen (cmd);
#endif /*_WINDOWS_DLL*/

   if (FrRef[0] != 0)
     {
#ifdef _WINDOWS_DLL
       printArgv[printArgc] = TtaGetMemory (20);
       sprintf (printArgv[printArgc], "-w%ld", FrRef[0]);
       printArgc++;
#else /* _WINDOWS_DLL */
       sprintf (&cmd[i], " -w%u", (unsigned int) FrRef[0]);
#endif /* _WINDOWS */
     }
   else
     {
       frame = 1;
       while (frame <= MAX_FRAME && FrameTable[frame].FrDoc != document)
	 frame++;
       if (frame <= MAX_FRAME)
	 {
#ifdef _WINDOWS_DLL
	   printArgv[printArgc] = TtaGetMemory (20);
	   sprintf (printArgv[printArgc], "-w%ld", FrRef[frame]);
	   printArgc++;
#else /* _WINDOWS_DLL */
	   sprintf (&cmd[i], " -w%u", (unsigned int) FrRef[frame]);
#endif /* _WINDOWS */
	 }
       else
	 {
#ifdef _WINDOWS_DLL
	   printArgv[printArgc] = TtaGetMemory (20);
	   sprintf (printArgv[printArgc], "-w%ld", FrRef[0]);
	   printArgc++;
#else /* _WINDOWS_DLL */
	   sprintf (&cmd[i], " -w%u", (unsigned int) FrRef[0]);
#endif /* _WINDOWS */
	 }
     }

   /* transmit values */
   if (nbPagesPerSheet != 1)
     {
#ifdef _WINDOWS_DLL
       printArgv[printArgc] = TtaGetMemory (6);
       strcpy (printArgv[printArgc], "-npps");
       printArgc++;
       printArgv[printArgc] = TtaGetMemory (5);
       sprintf (printArgv[printArgc], "%d", nbPagesPerSheet);
       printArgc++;
#else /* _WINDOWS_DLL */
       i = strlen (cmd);
       sprintf (&cmd[i], " -npps %d ", nbPagesPerSheet);
#endif /* _WINDOWS_DLL */
     }

   if (firstPage > 1 || lastPage < 999)
     {
#ifdef _WINDOWS_DLL
       printArgv[printArgc] = TtaGetMemory (7);
       sprintf (printArgv[printArgc], "-F%d", firstPage);
       printArgc++;
       printArgv[printArgc] = TtaGetMemory (7);
       sprintf (printArgv[printArgc], "-L%d", lastPage);
       printArgc++;
#else /* _WINDOWS */
   i = strlen (cmd);
   sprintf (&cmd[i], " -F%d -L%d ", firstPage, lastPage);
#endif /* _WINDOWS */
     }

   if (nCopies > 1)
     {
#ifdef _WINDOWS_DLL
       printArgv[printArgc] = TtaGetMemory (8);
       sprintf (printArgv[printArgc], "-#%d", nCopies);
       printArgc++;
#else /* _WINDOWS */
       i = strlen (cmd);
       sprintf (&cmd[i], " -#%d ", nCopies);
#endif /* _WINDOWS */
     }

   if (hShift != 0)
     {
#ifdef _WINDOWS_DLL
       printArgv[printArgc] = TtaGetMemory (7);
       sprintf (printArgv[printArgc], "-H%d", hShift);
       printArgc++;
#else /* _WINDOWS */
       i = strlen (cmd);
       sprintf (&cmd[i], " -H%d ", hShift);
#endif /* _WINDOWS */
     }

   if (vShift != 0)
     {
#ifdef _WINDOWS_DLL
       printArgv[printArgc] = TtaGetMemory (7);
       sprintf (printArgv[printArgc], "-V%d", vShift);
       printArgc++;
#else /* _WINDOWS */
       i = strlen (cmd);
       sprintf (&cmd[i], " -V%d ", vShift);
#endif /* _WINDOWS */
     }

   if (reduction != 100)
     {
#ifdef _WINDOWS_DLL
       printArgv[printArgc] = TtaGetMemory (7);
       sprintf (printArgv[printArgc], "-%%%d", reduction);
       printArgc++;
#else /* _WINDOWS */
       i = strlen (cmd);
       sprintf (&cmd[i], " -%%%d ", reduction);
#endif /* _WINDOWS */
     }

   /* transmit all view names */
   i = 0;
   /* skip leading spaces */ 
   while (viewsToPrint[i] == SPACE)
     i++;
   /* insert the first flag */
   if (viewsToPrint[i] != EOS)
     {
       /* insert the flag -v before each view name */
#ifdef _WINDOWS_DLL
       printArgv[printArgc] = TtaStrdup ("-v");
       printArgc++;
       printArgv[printArgc] = TtaGetMemory (50);
       j = 0;
#else /* _WINDOWS */
       j = strlen (cmd);
       sprintf (&cmd[j], " -v ");
       j = strlen (cmd);
#endif /* _WINDOWS */

       while (viewsToPrint[i] != EOS)
	 {
	   /* is it a space? */
	   if (viewsToPrint[i] == SPACE)
	     {
	       i++;
	       /* skip multiple spaces */
	       while (viewsToPrint[i] == SPACE)
		 i++;
	       if (viewsToPrint[i] != EOS)
		 {
		   /* insert the flag -v before each view name */
#ifdef _WINDOWS_DLL
		   printArgv[printArgc][j++] = EOS;
		   printArgc++;
		   printArgv[printArgc] = TtaStrdup ("-v");
		   printArgc++;
		   printArgv[printArgc] = TtaGetMemory (50);
		   j = 0;
#else /* _WINDOWS */
		   j = strlen (cmd);
		   sprintf (&cmd[j], " -v ");
		   j = strlen (cmd);
#endif /* _WINDOWS_DLL */
		 }
	     }
	   else
	     {
	       /* copy the character */
#ifdef _WINDOWS_DLL
	       printArgv[printArgc][j++] = viewsToPrint[i];
#else /* _WINDOWS */
	       cmd[j++] = viewsToPrint[i];
	       cmd[j] = EOS;
#endif /* _WINDOWS */
	       /* process next char */
	       i++;
	     }
	 }
#ifdef _WINDOWS_DLL
       printArgv[printArgc][j] = EOS;
       printArgc++;
#endif /* _WINDOWS */
     }

   /* transmit css files */
   if (cssToPrint != NULL && cssToPrint[0] != EOS)
     {
       i = 0;
       while (cssToPrint[i] != EOS)
	 {
	   /* skip leading spaces */ 
	   while (cssToPrint[i] == SPACE)
	     i++;
	   if (cssToPrint[i] != 'a' && cssToPrint[i] != 'u')
	     /* ERROR */
	     cssToPrint[i] = EOS;
	   else
	     {
	       /* insert the flag -cssa or -cssu before each stylesheet name */
#ifdef _WINDOWS_DLL
	       if (cssToPrint[i] == 'a')
		 /* it's an author stylesheet */
		 printArgv[printArgc] = TtaStrdup ("-cssa");
	       else if (cssToPrint[i] == 'u')
		 /* it's an user stylesheet */
		 printArgv[printArgc] = TtaStrdup ("-cssu");
	       printArgc++;
#else /* _WINDOWSDLL */
	       j = strlen (cmd);
	       if (cssToPrint[i] == 'a')
		 sprintf (&cmd[j], " -cssa ");
	       else
		 sprintf (&cmd[j], " -cssu ");
	       j = strlen (cmd);
#endif /* _WINDOWS_DLL */
	       /* skip the flag "a" or "u" */
	       i++;
	     }
	   /* skip spaces after the flag */
	   while (cssToPrint[i] == SPACE && cssToPrint[i] != EOS)
	     i++;
           if (cssToPrint[i] != EOS)
	     /* there is a file name after the flag */
	     {
#ifdef _WINDOWS_DLL
	       printArgv[printArgc] = TtaGetMemory (50);
		   j = 0;
#endif /* _WINDOWS_DLL */
	       while (cssToPrint[i] != SPACE && cssToPrint[i] != EOS)
		 {
		   /* copy the character */
#ifdef _WINDOWS_DLL
		   printArgv[printArgc][j++] = cssToPrint[i];
		   printArgv[printArgc][j] = EOS;
#else /* _WINDOWSDLL */
		   cmd[j++] = cssToPrint[i];
		   cmd[j] = EOS;
#endif /* _WINDOWS */
		   /* process next char */
		   i++;
		 }
#ifdef _WINDOWS_DLL
	       printArgc++;
#endif /* _WINDOWS */
	     }
	 }
     }
   /* transmit the path or source file */
#ifdef _WINDOWS
#ifndef _WINDOWS_DLL
   {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

   cmd[j] = EOS;
   i = strlen (cmd);
   sprintf (&cmd[i], " -removedir %s\\%s.PIV", dir, name);

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

	CreateProcess ( NULL,   /* No module name (use command line). */
        cmd,               /* Command line. */
        NULL,             /* Process handle not inheritable. */
        NULL,             /* Thread handle not inheritable. */
        FALSE,            /* Set handle inheritance to FALSE. */
        0,                /* No creation flags. */
        NULL,             /* Use parent's environment block. */
        NULL,             /** Use parent's starting directory. */
        &si,              /* Pointer to STARTUPINFO structure.*/
        &pi );            /* Pointer to PROCESS_INFORMATION structure.*/
	/*do we have to wait until process die ?*/
	/* WaitForInputIdle()*/ 
}
#else /*_WINDOWSDLL*/
   printArgv[printArgc] = TtaStrdup ("-removedir");
   printArgc++;
   printArgv[printArgc] = TtaGetMemory (strlen (dir) + strlen (name) + 6);
   sprintf  (printArgv[printArgc], "%s\\%s.PIV", dir, name);
   printArgc++;
   /*WIN_ReleaseDeviceContext ();*/
   hLib = LoadLibrary ("thotprinter");
   if (!hLib)
      return /* FATAL_EXIT_CODE */;
   ptrMainProc = (MYPROC) GetProcAddress (hLib, "PrintDoc");
   if (!ptrMainProc)
     {
       FreeLibrary (hLib);
       return /* FATAL_EXIT_CODE */;
     }
   
   EnableWindow  (FrRef[frame], FALSE);

   if (TtPrinterDC)
   (ptrMainProc) (FrRef[frame], printArgc, printArgv,
		TtPrinterDC, TtIsTrueColor, 
		TtWDepth, name, dir, hInstance, buttonCommand);

   FreeLibrary (hLib);

   EnableWindow (FrRef[frame], TRUE);
   SetFocus (FrRef[frame]);
   for (i = 0; i < printArgc; i++)
       TtaFreeMemory (printArgv[i]);
   if (TtPrinterDC)
   { 
       DeleteDC (TtPrinterDC);
       TtPrinterDC = (HDC) 0;
     }
#endif /*_WINDOWSDLL*/
#else /* !_WINDOWS */
   cmd[j] = EOS;
   i = strlen (cmd);

   sprintf (&cmd[i], " -removedir %s/%s.PIV &", dir, name);

#ifdef _PCLDEBUG
    printf ("\n/usr/bin/ddd bin/%s\n", cmd); 
   /* res = system (cmd);  */
#else /* _PCLDEBUG */
   res = system (cmd); 
#endif /* _PCLDEBUG */
    if (res == -1) 
       TtaDisplaySimpleMessage (CONFIRM, LIB, TMSG_ERROR_PS_TRANSLATION); 

#endif /* _WINDOWS */
 
}

/*----------------------------------------------------------------------
   InitPrintParameters
   initializes the printing parameters.
  ----------------------------------------------------------------------*/
void        InitPrintParameters (Document document)
{
   PtrDocument pDoc;
   char       *ptr;
   int         lg;

   if (document == 0)
     pDoc = 0;
   else
     pDoc = LoadedDocument[document - 1];

   if (ThotLocalActions[T_rprint] == NULL)
     {
       /* Connect printing actions */
       TteConnectAction (T_rprint, (Proc) CallbackPrintmenu);
       /* read DEFAULTPRINTER variable */
       ptr = TtaGetEnvString ("THOTPRINT");
       if (ptr == NULL)
          strcpy (pPrinter, "");
       else
           strcpy (pPrinter, ptr);
       PSdir[0] = EOS;
       PrintingDoc = 0;
       defPaperPrint = TRUE;
       defManualFeed = FALSE;
       defFirstPage = 0;
       defLastPage = 999;
       defNbCopies = 1;
       defReduction = 100;
       defPagesPerSheet = 1;
       ptr = TtaGetEnvString ("PAPER");
       if (ptr == NULL)
          strcpy(PageSize, "A4");
       else
           strcpy(PageSize, ptr);
       defPaginate = TRUE;
       PresSchema[0] = EOS;
     }

   if (document != PrintingDoc || document == 0)
     {
       /* we are changing the current printed document */
       PrintingDoc = document;
       PaperPrint = defPaperPrint;
       ManualFeed = defManualFeed;
       FirstPage = defFirstPage;
       LastPage = defLastPage;
       NbCopies = defNbCopies;
       Reduction = defReduction;
       PagesPerSheet = defPagesPerSheet;
       Paginate = defPaginate;
       if (pDoc != NULL)
	 {
	   if (pDoc->DocDirectory[0] == DIR_SEP)
	     sprintf (PSdir, "%s/%s.ps", pDoc->DocDirectory, pDoc->DocDName);
	   else
	     {
	       ptr = NULL;
	       ptr = TtaGetEnvString ("APP_TMPDIR");
	       if (ptr == NULL || *ptr == EOS || !TtaCheckDirectory (ptr))
		 {
		   ptr = NULL;
		   ptr = TtaGetEnvString ("TMPDIR");
		 }
	       if (ptr != NULL && TtaCheckDirectory (ptr))
		 {
		   strcpy(PSdir,ptr);
		   lg = strlen(PSdir);
		   if (PSdir[lg - 1] == DIR_SEP)
		     PSdir[--lg] = EOS;
		 }
	       else
		 {
#ifdef _WINDOWS
		   strcpy (PSdir, "C:\\TEMP");
#else  /* !_WINDOWS */
		   strcpy (PSdir,"/tmp");
#endif /* !_WINDOWS */
		   lg = strlen (PSdir);
		 }
	       sprintf (&PSdir[lg], "/%s.ps", pDoc->DocDName);
	     }
	 }
     }
}

/*----------------------------------------------------------------------
   TtcPrint standard handler for the Print action.  
   Calls TtaPrint to print the current view.
   ---------------------------------------------------------------------*/
void TtcPrint (Document document, View view)
{
   PathBuffer          viewsToPrint;

   strcpy (viewsToPrint, TtaGetViewName (document, view));
   strcat (viewsToPrint, " ");
   TtaPrint (document, viewsToPrint, NULL);
}

/*----------------------------------------------------------------------
   TtaGetPrintNames generates and returns a directory name to store
   temporary files neede to start a print process.
   The function creates the directory if it doesn't already exist.
  ----------------------------------------------------------------------*/
void TtaGetPrintNames (char **printDocName, char **printDirName)
{
   ThotPid             pid = ThotPid_get ();
   char               *dirString;
   int                 lg;

   *printDocName = PrintDocName;
   PrintDocName[0] = EOS;
   *printDirName = PrintDirName;
   PrintDirName[0] = EOS;

   /* get the tmp directory from the registry */
   dirString = TtaGetEnvString("TMPDIR");
   if (dirString != NULL) 
     { 
       if (!TtaCheckDirectory (dirString))
	 TtaMakeDirectory (dirString);
       strcpy (PrintDirName, dirString);
       lg = strlen(PrintDirName);
       if (PrintDirName[lg - 1] == DIR_SEP)
         PrintDirName[--lg] = EOS;
     }
   else
     {
       strcpy (PrintDirName, TtaGetEnvString ("TMPDIR"));
       lg = strlen (PrintDirName);
     }

   sprintf (PrintDocName, "Thot%ld", (long) pid + numOfJobs);
   strcpy (&PrintDirName[lg], DIR_STR);
   strcat (&PrintDirName[lg], PrintDocName);
   if (!TtaCheckDirectory (PrintDirName))
      TtaMakeDirectory (PrintDirName);
}

#ifdef _WINDOWS

/*----------------------------------------------------------------------
  TtaGetPrinterDC()
  Call the Windows print dialogue and returns TRUE is the printer is
  available. Reuses the previous defined printer when the parameter 
  reuse is TRUE.
  Returns the orientation (0 = portrait, 1 = landscape), and the paper
  format (0 = A4, 1 = US). 
  ----------------------------------------------------------------------*/
ThotBool TtaGetPrinterDC (ThotBool reuse, int *orientation, int *paper)
{
  LPDEVNAMES  lpDevNames;
  LPDEVMODE   lpDevMode;
  LPSTR       lpDriverName, lpDeviceName, lpPortName;

  /* Display the PRINT dialog box. */
  if (!LpInitialized)
    {
      /* initialize the pinter context */
      memset(&Pdlg, 0, sizeof(PRINTDLG));
      Pdlg.lStructSize = sizeof(PRINTDLG);
      Pdlg.nCopies = 1;
      Pdlg.Flags       = PD_RETURNDC;
      Pdlg.hInstance   = (HANDLE) NULL;
      LpInitialized = TRUE;
    }
  else if (reuse && Pdlg.hDevNames)
    {
      lpDevNames = (LPDEVNAMES) GlobalLock (Pdlg.hDevNames);
      lpDriverName = (LPSTR) lpDevNames + lpDevNames->wDriverOffset;
      lpDeviceName = (LPSTR) lpDevNames + lpDevNames->wDeviceOffset;
      lpPortName = (LPSTR) lpDevNames + lpDevNames->wOutputOffset;
      GlobalUnlock (Pdlg.hDevNames);
      if (Pdlg.hDevMode)
	{
	  lpDevMode = (LPDEVMODE) GlobalLock (Pdlg.hDevMode);
	  if (!lpDevMode)
		  return FALSE;
		  TtPrinterDC = CreateDC (lpDriverName, lpDeviceName, lpPortName, lpDevMode);
	  if (lpDevMode->dmOrientation == DMORIENT_LANDSCAPE)
	    /* landscape */
	    *orientation = 1;
	  else
	    /* portrait */
	    *orientation = 0;
	  if (lpDevMode->dmPaperSize == DMPAPER_A4)
	    /* A4 */
	    *paper = 0;
	  else
	    /* US */
	    *paper = 1;
	  GlobalUnlock (Pdlg.hDevMode);
	  return TRUE;
	}
    }

  Pdlg.hwndOwner   = GetCurrentWindow ();
  if (PrintDlg (&Pdlg))
    {
      if (Pdlg.hDevMode)
	{
	  lpDevMode = (LPDEVMODE) GlobalLock (Pdlg.hDevMode);
	  if (!lpDevMode)
		 return FALSE;
	  if (lpDevMode->dmOrientation == DMORIENT_LANDSCAPE)
	    /* landscape */
	    *orientation = 1;
	  else
	    /* portrait */
	    *orientation = 0;
	  if (lpDevMode->dmPaperSize == DMPAPER_A4)
	    /* A4 */
	    *paper = 0;
	  else
	    /* US */
	    *paper = 1;
	  GlobalUnlock (Pdlg.hDevMode);
	}
      TtPrinterDC = Pdlg.hDC;
      return TRUE;
    }
  else
    {
      TtPrinterDC = NULL;
      return FALSE;
    }
}
#endif /* _WINDOWS */


/*----------------------------------------------------------------------
   TtaPrint
   interface to the multiview print command.
  ----------------------------------------------------------------------*/
void TtaPrint (Document document, char *viewNames, char *cssNames)
{
   PtrDocument         pDoc;
   PathBuffer          dirName;
   Name                docName;
   Name                savePres, newPres;
   char               *tmpDirName, *tmpDocName;
   int                 orientation;
   ThotBool	       docReadOnly;
   ThotBool            ok;

   pDoc = LoadedDocument[document - 1];
   /* prepares the execution of the print command */
   strcpy (savePres, pDoc->DocSSchema->SsDefaultPSchema);
   if (PresSchema[0] != EOS)
     strcpy (newPres, PresSchema);
   else
     ConfigGetPSchemaForPageSize (pDoc->DocSSchema, PageSize, newPres);
     
   if (newPres[0] != EOS)
     strcpy (pDoc->DocSSchema->SsDefaultPSchema, newPres);

   /* initialise temporary directory and temporary file names */
   TtaGetPrintNames (&tmpDocName, &tmpDirName);
   numOfJobs++;

   strncpy (dirName, pDoc->DocDirectory, MAX_PATH);
   strncpy (docName, pDoc->DocDName, MAX_NAME_LENGTH);
   if (pFuncExportPrintDoc !=NULL)
     /* a export procedure is defined */
       ok = (*pFuncExportPrintDoc)(document, PrintDocName, PrintDirName);
   else
     /* standard export */
     {
       docReadOnly = pDoc->DocReadOnly;

       strcpy (pDoc->DocDirectory, PrintDirName);
       strcpy (pDoc->DocDName, PrintDocName);
       pDoc->DocReadOnly = FALSE;

       ok = WriteDocument (pDoc, 5);

       pDoc->DocReadOnly = docReadOnly;
       strncpy (pDoc->DocDirectory, dirName, MAX_PATH);
       strncpy (pDoc->DocDName, docName, MAX_NAME_LENGTH);
     }

   /* searches the paper orientation for the presentation scheme */
   if (!strcmp (Orientation, "Landscape"))
     orientation = 1;
   else
     orientation = 0;

   /* restores the presentation scheme */
   strcpy (pDoc->DocSSchema->SsDefaultPSchema, savePres);

   /* make an automatic backup */
   if (ok)
     { 
       if (PaperPrint)
	 Print (PrintDocName,
		PrintDirName,
		pDoc->DocSchemasPath,
		DocumentPath,
		docName, pPrinter,
		FirstPage, LastPage, NbCopies, 
		0, 0, orientation,
		Reduction, PagesPerSheet, TRUE,
		(int) ManualFeed, 0,
		Paginate,
		viewNames,
		cssNames,
		document);
       else if (PSdir[0] != EOS)
	 Print (PrintDocName,
		PrintDirName,
		pDoc->DocSchemasPath,
		DocumentPath,
		docName, PSdir,
		FirstPage, LastPage, NbCopies,
		0, 0, orientation,
		Reduction, PagesPerSheet, TRUE,
		(int) ManualFeed, 0,
		Paginate,
		viewNames,
		cssNames,
		document);
     }
   /* restores the presentation scheme */
   strcpy (pDoc->DocSSchema->SsDefaultPSchema, savePres);
}

/*----------------------------------------------------------------------
  TtaSetPrintExportFunc: Sets a non-standard document export function for printing
  ----------------------------------------------------------------------*/
void TtaSetPrintExportFunc (Func exportFunc)
{
  pFuncExportPrintDoc = exportFunc;
}

/*----------------------------------------------------------------------
  TtaSetPrintParameter: Sets a print parameter
  ----------------------------------------------------------------------*/
void TtaSetPrintParameter (PrintParameter parameter, int value)
{
  if (ThotLocalActions[T_rprint] == NULL)
    /* force the initialization of printing parameters */
    InitPrintParameters (0);

  switch (parameter)
    {
    case PP_Orientation:
      if (value == PP_Landscape)
	strcpy (Orientation, "Landscape");
      else
	strcpy (Orientation, "Portrait");
      break;
    case PP_FirstPage:
      if (value  <0 || value > 999)
	TtaError(ERR_invalid_parameter);
      else
	FirstPage = value;
      break;
    case PP_LastPage:
      if (value < 0 || value > 999)
	TtaError(ERR_invalid_parameter);
      else
	LastPage = value;
      break;
    case PP_Scale:
      if (value < 0 || value > 999)
	TtaError(ERR_invalid_parameter);
      else
	Reduction = value;
      break;
    case PP_NumberCopies:
      if (value < 0 || value > 999)
	TtaError(ERR_invalid_parameter);
      else
	NbCopies = value;
      break;
    case PP_Paginate:
      if (value == PP_ON)
	Paginate = TRUE;
      else if (value == PP_OFF)
	Paginate = FALSE;
      else
	TtaError(ERR_invalid_parameter);
      break;
    case PP_ManualFeed:
      if (value == PP_ON)
	ManualFeed = TRUE;
      else if (value == PP_OFF)
	ManualFeed = FALSE;
      else
	TtaError(ERR_invalid_parameter);
      break;
    case PP_PagesPerSheet:
      if (value != 1 && value != 2 && value != 4)
	TtaError(ERR_invalid_parameter);
      else
	PagesPerSheet = value;
      break;
    case PP_PaperSize:
      if (value == PP_A4)
	{
	  if (strcmp (PageSize, "A4"))
	    {
	      strcpy (PageSize, "A4");
	      TtaSetEnvString ("PAPER", PageSize, TRUE);
	      TtaSaveAppRegistry ();
	    }
	}
      else if (value == PP_US)
	{
	  if (strcmp (PageSize, "US"))
	    {
	      strcpy (PageSize, "US");
	      TtaSetEnvString ("PAPER", PageSize, TRUE);
	      TtaSaveAppRegistry ();
	    }
	}
      else
	TtaError(ERR_invalid_parameter);
      break;
    case PP_Destination:
      if (value == PP_PRINTER)
	PaperPrint = TRUE;
      else if (value == PP_PS)
	PaperPrint = FALSE;
      else
	TtaError(ERR_invalid_parameter);
      break;
    default:
      TtaError(ERR_invalid_parameter);
    }
}


/*----------------------------------------------------------------------
  TtaGetPrintParameter: returns a print parameter
  ----------------------------------------------------------------------*/
int TtaGetPrintParameter (PrintParameter parameter)
{
  char    *ptr;

  switch (parameter)
    {
    case PP_Orientation:
      if (!strcmp (Orientation, "Landscape"))
	return (PP_Landscape);
      else
	return (PP_Portrait);
      break;
    case PP_FirstPage:
      return (FirstPage);
      break;
    case PP_LastPage:
      return (LastPage);
      break;
    case PP_Scale:
      return (Reduction);
      break;
    case PP_NumberCopies:
      return (NbCopies);
      break;
    case PP_Paginate:
      if (Paginate)
	return (PP_ON);
      else
	return (PP_ON);
      break;
    case PP_ManualFeed:
      if (ManualFeed)
	return (PP_ON);
      else
	return (PP_OFF);
      break;
    case PP_PagesPerSheet:
      return (PagesPerSheet);
      break;
    case PP_PaperSize:
      ptr = TtaGetEnvString ("PAPER");
      if (ptr && strcmp (ptr, "A4"))
	{
	  strcpy (PageSize, ptr);
	  return (PP_US);
	}
      else
	{
	  strcpy (PageSize, "A4");
	  return (PP_A4);
	}
      break;
    case PP_Destination:
      if (PaperPrint)
	return (PP_PRINTER);
      else
	return (PP_PS);
      break;
    default:
      TtaError(ERR_invalid_parameter);
      return (0);
    }
}


/*----------------------------------------------------------------------
  TtaSetPrintCommand sets the print command.
  ----------------------------------------------------------------------*/
void                TtaSetPrintCommand (char *command)
{
  strcpy (pPrinter, command);
}


/*----------------------------------------------------------------------
  TtaGetPrintCommand returns the print command.
  ----------------------------------------------------------------------*/
void                TtaGetPrintCommand (char *command)
{
  if (command == NULL)
    TtaError(ERR_invalid_parameter);
  else
    strcpy (command, pPrinter);
}


/*----------------------------------------------------------------------
  TtaSetPrintSchema fixes the printing schema.
  ----------------------------------------------------------------------*/
void                TtaSetPrintSchema (char *name)
{
  if (strlen(name) >= MAX_NAME_LENGTH)
    TtaError(ERR_invalid_parameter);
  else
    strcpy (PresSchema, name);
}


/*----------------------------------------------------------------------
  TtaSetPrintCommand sets the path of ps file.
  ----------------------------------------------------------------------*/
void                TtaSetPsFile (char *path)
{
  strcpy (PSdir, path);
}


/*----------------------------------------------------------------------
  TtaGetPsFile returns the path of ps file.
  ----------------------------------------------------------------------*/
void                TtaGetPsFile (char *path)
{
  if (path == NULL)
    TtaError(ERR_invalid_parameter);
  else
    strcpy (path, PSdir);
}


/*----------------------------------------------------------------------
  CallbackPrintmenu
  callback associated to the PrintSetup form 
  ----------------------------------------------------------------------*/
void                CallbackPrintmenu (int ref, int val, char *txt)
{
  PtrDocument         pDoc;

  if (PrintingDoc != 0)
    {
      pDoc = LoadedDocument[PrintingDoc - 1];

    if (pDoc->DocSSchema != NULL)
      /* the document to be printed still exists */
      switch (ref)
	{
	case NumMenuSupport:
	  /* paper print/save PostScript submenu */
	  switch (val)
	    {
	    case 0:
	      if (!NewPaperPrint)
		{
		  NewPaperPrint = TRUE;
#ifndef _WINDOWS
		  TtaSetTextForm (NumZonePrinterName, pPrinter);
#endif /* !_WINDOWS */
		}
	      break;
	    case 1:
	      if (NewPaperPrint)
		{
		  NewPaperPrint = FALSE;
#ifndef _WINDOWS
		  TtaSetTextForm (NumZonePrinterName, PSdir);
#endif /* !_WINDOWS */
		}
	      break;
	    }
	  break;
	case NumMenuPaperFormat:
	  /* page size submenu */
	  switch (val)
	    {
	    case 0:
	      strcpy (PageSize, "A4");
	      break;
	    case 1:
	      strcpy (PageSize, "US");
	      break;
	    }
	  break;
	case NumMenuOptions:
	  switch (val)
	    {
	    case 0:
	      /* Manual feed option */
	      ManualFeed = !ManualFeed;
	      break;
	    case 1:
	      /* Repagination option */
	      Paginate = !Paginate;
	      break;
	    }
	  break;
	case NumZonePrinterName:
	  if (txt[0] != EOS)
	    {
	    if (NewPaperPrint)
	      /* text capture zone for the printer name */
	      strncpy (pPrinter, txt, MAX_PATH);
	    else
	      /* text capture zone for the name of the PostScript file */
	      strncpy (PSdir, txt, MAX_PATH);
	    }
	  break;
	case NumFormPrint:
	  /* Print form option */
	  TtaDestroyDialogue (NumFormPrint);
	  switch (val)
	    {
	    case 1:
	      /* confirms the paper print option */
	      /* the other options are not taken into account without this
		 confirmation */
	      PaperPrint = NewPaperPrint;
	      break;
	    default:
	      break;
	    }
	  break;
	default:
	  break;
	}
    }
}

/*----------------------------------------------------------------------
   TtcPrintSetup
   standard handler for a PrintSetup action.
   Prepares and displays a form.
  ----------------------------------------------------------------------*/
void                TtcPrintSetup (Document document, View view)
{
   int              i;
   char             bufMenu[MAX_TXT_LEN];

   if (document == 0)
     return;

   /* Print form */
   InitPrintParameters (document);
#ifndef _WINDOWS
   TtaNewSheet (NumFormPrint, TtaGetViewFrame (document, view), TtaGetMessage (LIB, TMSG_LIB_PRINT), 1, TtaGetMessage (LIB, TMSG_LIB_CONFIRM), FALSE, 2, 'L', D_CANCEL);

   i = 0;
   sprintf (&bufMenu[i], "B%s", TtaGetMessage (LIB, TMSG_MANUAL_FEED));
   TtaNewToggleMenu (NumMenuOptions, NumFormPrint, TtaGetMessage (LIB, TMSG_OPTIONS), 1, bufMenu, NULL, FALSE);
   if (ManualFeed)
      TtaSetToggleMenu (NumMenuOptions, 0, TRUE);
#endif /* _WINDOWS */

   /* Paper format submenu */
   i = 0;
   sprintf (&bufMenu[i], "B%s", TtaGetMessage (LIB, TMSG_A4));
   i += strlen (&bufMenu[i]) + 1;
   sprintf (&bufMenu[i], "B%s", TtaGetMessage (LIB, TMSG_US));
   TtaNewSubmenu (NumMenuPaperFormat, NumFormPrint, 0, TtaGetMessage (LIB, TMSG_PAPER_SIZE), 2, bufMenu, NULL, FALSE);
#ifndef _WINDOWS
   if (!strcmp (PageSize, "US"))
      TtaSetMenuForm (NumMenuPaperFormat, 1);
   else
      TtaSetMenuForm (NumMenuPaperFormat, 0);
#endif /* !_WINDOWS */

   /* Print to paper/ Print to file submenu */
   i = 0;
   sprintf (&bufMenu[i], "B%s", TtaGetMessage (LIB, TMSG_PRINTER));
   i += strlen (&bufMenu[i]) + 1;
   sprintf (&bufMenu[i], "B%s", TtaGetMessage (LIB, TMSG_PS_FILE));
   TtaNewSubmenu (NumMenuSupport, NumFormPrint, 0,
                  TtaGetMessage (LIB, TMSG_OUTPUT), 2, bufMenu, NULL, TRUE);
   /* initialization of the PaperPrint selector */
   NewPaperPrint = PaperPrint;
#ifndef _WINDOWS
   if (PaperPrint)
     {
       TtaSetMenuForm (NumMenuSupport, 0);
       TtaSetTextForm (NumZonePrinterName, pPrinter);
     }
   else
     {
       TtaSetMenuForm (NumMenuSupport, 1);
       TtaSetTextForm (NumZonePrinterName, PSdir);
     }
   /* text capture zone for the printer name */
   TtaNewTextForm (NumZonePrinterName, NumFormPrint, NULL, 30, 1, FALSE);

   /* activates the Print form */
   TtaShowDialogue (NumFormPrint, FALSE);
#endif /* !_WINDOWS */
}

