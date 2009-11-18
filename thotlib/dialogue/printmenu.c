/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * Messages and printing management.
 */

#if defined(_WX) && defined(_WINDOWS)
  #include "wx/wx.h"
  #include "wx/dynlib.h"
#endif /* _WX  && _WINDOWS */

#include "thot_gui.h"
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
#include "browser_f.h"
#include "config_f.h"
#include "creationmenu_f.h"
#include "dialogapi_f.h"
#include "docs_f.h"
#include "exceptions_f.h"
#include "memory_f.h"
#include "fileaccess_f.h"
#include "presentmenu_f.h"
#include "printmenu_f.h"
#include "registry_f.h"
#include "schemas_f.h"
#include "search_f.h"
#include "searchmenu_f.h"
#include "structcreation_f.h"
#include "structmodif_f.h"
#include "structselect_f.h"
#include "textcommands_f.h"
#include "thotmsg_f.h"
#include "tree_f.h"
#include "views_f.h"
#include "viewapi_f.h"
#include "writedoc_f.h"
#include "writeprs_f.h"
#include "writestr_f.h"

#if defined(_WX) && defined(_WINDOWS)
  #include "AmayaWindow.h"
  #include "appdialogue_wx.h"
#endif /* _WX  && _WINDOWS */

#ifdef _WINDOWS
#include "thotprinter_f.h"
#include "wininclude.h"
static PRINTDLG     Pdlg;
static ThotBool     LpInitialized = FALSE;
#endif /* _WINDOWS */

static ThotBool     PInitialized = FALSE;
static PathBuffer   PrintDirName;
static Name         PrintDocName;
static char         Orientation[MAX_NAME_LENGTH];
static Func         pFuncExportPrintDoc = NULL;
static ThotBool     defPaperPrint;
static ThotBool     defManualFeed;
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
#ifdef _WINDOWS
  HINSTANCE               hLib = NULL;
   typedef void (*MYPROC)(HWND, int, char **, HDC, ThotBool,
			      int, char *, char *, HINSTANCE, ThotBool);
   MYPROC                  ptrMainProc;
   char                    tmp[MAX_TXT_LEN];
   char                   *printArgv [100];
   int                     printArgc = 0;
#else /* _WINDOWS */
   char                    cmd[1024];
   int                     res;
#endif /* _WINDOWS */
   int                     i, lg, j = 0;
   int                     frame;

#ifdef _WX
   name         = TtaGetRealFileName(name);
   dir          = TtaGetRealFileName(dir);
   thotSch      = TtaGetRealFileName(thotSch);
   thotDoc      = TtaGetRealFileName(thotDoc);
   realName     = TtaGetRealFileName(realName);
   output       = TtaGetRealFileName(output);
   cssToPrint   = TtaGetRealFileName(cssToPrint);
#endif /* _WX */

   /* initialize the print command */
   ptr = TtaGetEnvString ("LANG");
#ifdef _WINDOWS
   for (i = 0; i < 100; i++)
     printArgv[i] = NULL;
   printArgv[printArgc] = (char *)TtaGetMemory (strlen (BinariesDirectory) + 7);
   strcpy (printArgv[printArgc], BinariesDirectory);
   strcat (printArgv[printArgc], DIR_STR);
   strcat (printArgv[printArgc], "print");
   printArgc++;   
   printArgv[printArgc] = (char *)TtaGetMemory (6) ;
   strcpy (printArgv[printArgc], "-lang");
   printArgc++;
   printArgv[printArgc] = (char *)TtaGetMemory (strlen (ptr) + 1);
   strcpy (printArgv[printArgc], ptr);
   printArgc++;
#else /* _WINDOWS */
   sprintf (cmd, "%s/print", BinariesDirectory); 
   strcat (cmd, " -lang ");
   strcat (cmd, ptr);
#endif /* _WINDOWS */

   if (thotSch != NULL && thotSch[0] != EOS)
     {
#ifdef _WINDOWS
       printArgv[printArgc] = (char *)TtaGetMemory (5) ;
       strcpy (printArgv[printArgc], "-sch");
       printArgc++;
       printArgv[printArgc] = (char *)TtaGetMemory (strlen (thotSch) + 1);
       strcpy (printArgv[printArgc], thotSch);
       printArgc++;
       printArgv[printArgc] = (char *)TtaGetMemory (5);
       strcpy (printArgv[printArgc], "-doc");
       printArgc++;
       printArgv[printArgc] = (char *)TtaGetMemory (strlen (thotDoc) + 1);
       strcpy (printArgv[printArgc], thotDoc);
       printArgc++;
#else /* _WINDOWS */
       strcat (cmd, " -sch ");
       strcat (cmd, thotSch);
       strcat (cmd, " -doc ");
       strcat (cmd, thotDoc);
#endif /* _WINDOWS */
     }

   /* transmit the server name */
   if (servername && servername[0] != EOS)
     {
#ifdef _WINDOWS
       printArgv[printArgc] = (char *)TtaGetMemory (9);
       strcpy (printArgv[printArgc], "-display");
       printArgc++;
       printArgv[printArgc] = (char *)TtaGetMemory (strlen (servername) + 1);
       strcpy (printArgv[printArgc], servername);
       printArgc++;
#else /* _WINDOWS */
       strcat (cmd, " -display ");
       strcat (cmd, servername);
#endif /* _WINDOWS */
     }

   /* transmit the document name */
   if (realName)
     {
       /* cut the name at the first & character */
       ptr = strstr (realName, "&");
       if (ptr)
	 *ptr = EOS;
#ifdef _WINDOWS
       printArgv[printArgc] = (char *)TtaGetMemory (6);
       strcpy (printArgv[printArgc], "-name");
       printArgc++;
       printArgv[printArgc] = (char *)TtaGetMemory (strlen (realName) + 10);
       strcpy (printArgv[printArgc], realName);
       printArgc++;
#else /* _WINDOWS */
       strcat (cmd, " -name ");
       strcat (cmd, realName);
#endif /* _WINDOWS */
       /* restore the name */
       if (ptr)
	 *ptr = '&';
     }

   /* transmit the orientation (default value is portrait) */
   if (userOrientation != 0)
     {
#ifdef _WINDOWS
       printArgv[printArgc] = (char *)TtaGetMemory (11);
       strcpy (printArgv[printArgc], "-landscape");
       printArgc++;
#else /* _WINDOWS */
       strcat (cmd, " -landscape");
#endif /* _WINDOWS */
     }

   /* transmit the output command */
   if (PaperPrint)
     {
#ifdef _WINDOWS
       printArgv[printArgc] = (char *)TtaGetMemory (5);
       strcpy (printArgv[printArgc], "-out");
       printArgc++;
#else /* _WINDOWS */
       strcat (cmd, " -out \"");
#endif /* _WINDOWS */
     }
   else
     {
#ifdef _WINDOWS
       printArgv[printArgc] = (char *)TtaGetMemory (4);
       strcpy (printArgv[printArgc], "-ps");
       printArgc++;
#else /* _WINDOWS */
       strcat (cmd, " -ps \"");
#endif /* _WINDOWS */
     }

   if (output[0] != EOS)
     {
#ifdef _WINDOWS
       printArgv[printArgc] = (char *)TtaGetMemory (strlen (output) + 1);
       strcpy (printArgv[printArgc], output);
       printArgc++;
#else /* _WINDOWS */
       strcat (cmd, output);
       strcat (cmd, "\" ");
#endif /* _WINDOWS */
     }
   else
     {
#ifdef _WINDOWS
       printArgv[printArgc] = (char *)TtaGetMemory (3);
       strcpy (printArgv[printArgc], "lp");
       printArgc++;
#else /* _WINDOWS */
       strcat (cmd, "lp");
       strcat (cmd, "\" ");
#endif /* _WINDOWS */
     }

   /* transmit visualization of empty boxes (default no) */
   if (suppFrame == 0)
     {
#ifdef _WINDOWS
       printArgv[printArgc] = (char *)TtaGetMemory (10);
       strcpy (printArgv[printArgc], "-emptybox");
       printArgc++;
#else /* _WINDOWS */
       strcat (cmd, " -emptybox");
#endif /* _WINDOWS */
     }

   /* transmit black/white output (default no) */
   if (blackAndWhite != 0)
     {
#ifdef _WINDOWS
       printArgv[printArgc] = (char *)TtaGetMemory (4);
       strcpy (printArgv[printArgc], "-bw");
       printArgc++;
#else /* _WINDOWS */
       strcat (cmd, " -bw");
#endif /* _WINDOWS */
     }

   /* transmit manualfeed (default no) */
   if (manualFeed != 0)
     {
#ifdef _WINDOWS
       printArgv[printArgc] = (char *)TtaGetMemory (12);
       strcpy (printArgv[printArgc], "-manualfeed");
       printArgc++;
#else /* _WINDOWS */
       strcat (cmd, " -manualfeed");
#endif /* _WINDOWS */
     }

   /* transmit repaginate (default no) */
   if (repaginate != 0)
     {
#ifdef _WINDOWS
       printArgv[printArgc] = (char *)TtaGetMemory (10);
       strcpy (printArgv[printArgc], "-paginate");
       printArgc++;
#else /* _WINDOWS */
       strcat (cmd, " -paginate");
#endif /* _WINDOWS */
     }

   /* transmit page format */
   if (strcmp (PageSize, "A4"))
     {
#ifdef _WINDOWS
       sprintf (tmp, "-P%s", PageSize);
       printArgv[printArgc] = (char *)TtaGetMemory (strlen (PageSize) + 3);
       strcpy (printArgv[printArgc], tmp);
       printArgc++;
#else /* _WINDOWS */
       strcat (cmd, " -P");
       strcat (cmd, PageSize);
#endif /* _WINDOWS */
     }

   /* transmit window id */
#ifndef _WINDOWS
   i = strlen (cmd);
#endif /*_WINDOWS */

   frame = 1;
   while (frame <= MAX_FRAME && FrameTable[frame].FrDoc != document)
     frame++;
   if (frame <= MAX_FRAME)
     {
#ifdef _WINDOWS
       printArgv[printArgc] = (char *)TtaGetMemory (20);
#ifndef _WX
       sprintf (printArgv[printArgc], "-w%ld", FrRef[frame]);
#else /* _WX */
	   sprintf (printArgv[printArgc], "-w%ld", FrameTable[0].WdFrame);
#endif /* _WX */
       printArgc++;
#else /* _WINDOWS */
#ifndef _WX
       sprintf (&cmd[i], " -w%lu", (intptr_t) FrRef[0]);
#else /* _WX */
       sprintf (&cmd[i], " -w%lu", (long unsigned int)(intptr_t) FrameTable[0].WdFrame);
#endif /* _WX */
#endif /* _WINDOWS */
     }

   /* transmit values */
   if (nbPagesPerSheet != 1)
     {
#ifdef _WINDOWS
       printArgv[printArgc] = (char *)TtaGetMemory (6);
       strcpy (printArgv[printArgc], "-npps");
       printArgc++;
       printArgv[printArgc] = (char *)TtaGetMemory (5);
       sprintf (printArgv[printArgc], "%d", nbPagesPerSheet);
       printArgc++;
#else /* _WINDOWS */
       i = strlen (cmd);
       sprintf (&cmd[i], " -npps %d ", nbPagesPerSheet);
#endif /* _WINDOWS */
     }

   if (firstPage > 1 || lastPage < 999)
     {
#ifdef _WINDOWS
       printArgv[printArgc] = (char *)TtaGetMemory (7);
       sprintf (printArgv[printArgc], "-F%d", firstPage);
       printArgc++;
       printArgv[printArgc] = (char *)TtaGetMemory (7);
       sprintf (printArgv[printArgc], "-L%d", lastPage);
       printArgc++;
#else /* _WINDOWS */
   i = strlen (cmd);
   sprintf (&cmd[i], " -F%d -L%d ", firstPage, lastPage);
#endif /* _WINDOWS */
     }

   if (nCopies > 1)
     {
#ifdef _WINDOWS
       printArgv[printArgc] = (char *)TtaGetMemory (8);
       sprintf (printArgv[printArgc], "-#%d", nCopies);
       printArgc++;
#else /* _WINDOWS */
       i = strlen (cmd);
       sprintf (&cmd[i], " -#%d ", nCopies);
#endif /* _WINDOWS */
     }

   if (hShift != 0)
     {
#ifdef _WINDOWS
       printArgv[printArgc] = (char *)TtaGetMemory (7);
       sprintf (printArgv[printArgc], "-H%d", hShift);
       printArgc++;
#else /* _WINDOWS */
       i = strlen (cmd);
       sprintf (&cmd[i], " -H%d ", hShift);
#endif /* _WINDOWS */
     }

   if (vShift != 0)
     {
#ifdef _WINDOWS
       printArgv[printArgc] = (char *)TtaGetMemory (7);
       sprintf (printArgv[printArgc], "-V%d", vShift);
       printArgc++;
#else /* _WINDOWS */
       i = strlen (cmd);
       sprintf (&cmd[i], " -V%d ", vShift);
#endif /* _WINDOWS */
     }

   if (reduction != 100)
     {
#ifdef _WINDOWS
       printArgv[printArgc] = (char *)TtaGetMemory (7);
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
#ifdef _WINDOWS
       printArgv[printArgc] = TtaStrdup ("-v");
       printArgc++;
       printArgv[printArgc] = (char *)TtaGetMemory (50);
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
#ifdef _WINDOWS
		   printArgv[printArgc][j++] = EOS;
		   printArgc++;
		   printArgv[printArgc] = TtaStrdup ("-v");
		   printArgc++;
		   printArgv[printArgc] = (char *)TtaGetMemory (50);
		   j = 0;
#else /* _WINDOWS */
		   j = strlen (cmd);
		   sprintf (&cmd[j], " -v ");
		   j = strlen (cmd);
#endif /* _WINDOWS */
		 }
	     }
	   else
	     {
	       /* copy the character */
#ifdef _WINDOWS
	       printArgv[printArgc][j++] = viewsToPrint[i];
#else /* _WINDOWS */
	       cmd[j++] = viewsToPrint[i];
	       cmd[j] = EOS;
#endif /* _WINDOWS */
	       /* process next char */
	       i++;
	     }
	 }
#ifdef _WINDOWS
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
	   if (cssToPrint[i] == '"')
	     i++;
	   if (cssToPrint[i] != 'a' && cssToPrint[i] != 'u')
	     /* ERROR */
	     cssToPrint[i] = EOS;
	   else
	     {
	       /* insert the flag -cssa or -cssu before each stylesheet name */
#ifdef _WINDOWS
	       if (cssToPrint[i] == 'a')
		 /* it's an author stylesheet */
		 printArgv[printArgc] = TtaStrdup ("-cssa");
	       else if (cssToPrint[i] == 'u')
		 /* it's an user stylesheet */
		 printArgv[printArgc] = TtaStrdup ("-cssu");
	       printArgc++;
#else /* _WINDOWS */
	       j = strlen (cmd);
	       if (cssToPrint[i] == 'a')
		 sprintf (&cmd[j], " -cssa ");
	       else
		 sprintf (&cmd[j], " -cssu ");
	       j = strlen (cmd);
#endif /* _WINDOWS */
	       /* skip the flag "a" or "u" */
	       i++;
	     }
	   /* skip spaces after the flag */
       if (cssToPrint[i] == '"')
	   {
	     lg = 1;
	     while (cssToPrint[i+lg] != '"' && cssToPrint[i+lg] != EOS)
	       lg++;
	     if (cssToPrint[i+lg] != EOS)
	       /* there is a file name after the flag */
	       {
		 lg++;
#ifdef _WINDOWS
		 printArgv[printArgc] = (char *)TtaGetMemory (lg + 1);
		 j = 0;
		 while (j < lg)
		   printArgv[printArgc][j++] = cssToPrint[i++];
		 printArgv[printArgc][j] =EOS;
		 /* process next char */
		 printArgc++;
#else /* _WINDOWS */
		 lg += j;
		 while (j < lg)
		   cmd[j++] = cssToPrint[i++];
		 cmd[j] = EOS;
#endif /* _WINDOWS */
	       }
	   }
	 }
     }
   /* transmit the path or source file */
#ifdef _WINDOWS
   printArgv[printArgc] = TtaStrdup ("-removedir");
   printArgc++;
   printArgv[printArgc] = (char *)TtaGetMemory (strlen (dir) + strlen (name) + 6);
   sprintf  (printArgv[printArgc], "%s\\%s.PIV", dir, name);
   printArgc++;
#ifdef _WX
   wxDynamicLibrary dyn_lib;
   if (dyn_lib.Load(_T("thotprinter.dll"), wxDL_DEFAULT))
   {
	  ptrMainProc = (MYPROC) dyn_lib.GetSymbol (_T("PrintDoc"));
     if (ptrMainProc)
	 {   
       AmayaWindow * p_window = TtaGetActiveWindow();
	   HWND win_handle = (HWND)p_window->GetHandle();
	   (ptrMainProc) (win_handle, printArgc, printArgv,
		  NULL, TtIsTrueColor, 
          TtWDepth, name, dir, wxGetInstance(), TRUE);
        dyn_lib.Unload();
     }
   }
#else /* _WX */
   hLib = LoadLibrary ("thotprinter");
   if (hLib)
   {
     ptrMainProc = (MYPROC) GetProcAddress (hLib, "PrintDoc");
     if (ptrMainProc)
	 {
       EnableWindow  (FrRef[frame], FALSE);
       if (TtPrinterDC)
         (ptrMainProc) (FrRef[frame], printArgc, printArgv,
		    TtPrinterDC, TtIsTrueColor, 
		    TtWDepth, name, dir, hInstance, buttonCommand);

       EnableWindow (FrRef[frame], TRUE);
       SetFocus (FrRef[frame]);
       if (TtPrinterDC)
	   {
         DeleteDC (TtPrinterDC);
         TtPrinterDC = (HDC) 0;
	   }
	 }
     FreeLibrary (hLib);
   }
#endif /* _WX */
   for (i = 0; i < printArgc; i++)
       TtaFreeMemory (printArgv[i]);
#else /*_WINDOWS*/
   cmd[j] = EOS;
   i = strlen (cmd);
   sprintf (&cmd[i], " -removedir \"%s/%s.PIV\" &", dir, name);
#ifdef _PCLDEBUG
   printf ("\n/usr/bin/ddd bin/%s\n", cmd); 
#else /* _PCLDEBUG */
   res = system (cmd);
#endif /* _PCLDEBUG */
   if (res == -1) 
     TtaDisplaySimpleMessage (CONFIRM, LIB, TMSG_ERROR_PS_TRANSLATION);
#endif /*_WINDOWS*/

#ifdef _WX
   TtaFreeMemory(name);
   TtaFreeMemory(dir);
   TtaFreeMemory(thotSch);
   TtaFreeMemory(thotDoc);
   TtaFreeMemory(realName);
   TtaFreeMemory(output);
   TtaFreeMemory(cssToPrint);
#endif /* _WX */
}

/*----------------------------------------------------------------------
   InitPrintParameters
   initializes the printing parameters.
  ----------------------------------------------------------------------*/
void InitPrintParameters (Document document)
{
   PtrDocument pDoc;
   char       *ptr;
   int         lg;

   if (document == 0)
     pDoc = 0;
   else
     pDoc = LoadedDocument[document - 1];

   /* Connect printing actions */
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
	       if (ptr && TtaCheckDirectory (ptr))
		 {
		   strcpy (PSdir,ptr);
		   lg = strlen(PSdir);
		   if (PSdir[lg - 1] == DIR_SEP)
		     PSdir[--lg] = EOS;
		 }
	       else
		 {
		   lg = 0;
		   PSdir[0] = EOS;
		 }
	       sprintf (&PSdir[lg], "/%s.ps", pDoc->DocDName);
	     }
	 }
     }
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
   dirString = TtaGetEnvString ("APP_TMPDIR");
   if (dirString) 
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
       PrintDirName[0] = EOS;
       lg = 0;
     }

   strcpy (PrintDocName, "Thot_Print");
   strcpy (&PrintDirName[lg], DIR_STR);
   strcat (&PrintDirName[lg], PrintDocName);
   if (!TtaCheckDirectory (PrintDirName))
      TtaMakeDirectory (PrintDirName);
}

#ifdef _WINGUI

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
#endif /* _WINGUI */


/*----------------------------------------------------------------------
   TtaPrint
   interface to the multiview print command.
  ----------------------------------------------------------------------*/
void TtaPrint (Document document, char *viewNames, char *cssNames)
{
   PtrDocument         pDoc;
   PathBuffer          dirName;
   Name                docName;
   Name                schName;
   char               *newPres, *savePres;
   PtrPSchema          pPSch;
   char               *tmpDirName, *tmpDocName;
   char                fileName[MAX_TXT_LEN];
   int                 orientation, i, k;
   ThotBool	       docReadOnly;
   ThotBool            ok, firstGenericXML;

   pDoc = LoadedDocument[document - 1];
   /* prepares the execution of the print command */
   newPres = NULL;
   savePres = NULL;
   if (PresSchema[0] != EOS)
     newPres = TtaStrdup (PresSchema);
   else
     {
       ConfigGetPSchemaForPageSize (pDoc->DocSSchema, PageSize, schName);
       if (schName[0] != EOS)
	 newPres = TtaStrdup (schName);
     }
     
   if (newPres)
     {
       savePres = pDoc->DocSSchema->SsDefaultPSchema;
       pDoc->DocSSchema->SsDefaultPSchema = newPres;
     }

   /* initialise temporary directory and temporary file names */
   TtaGetPrintNames (&tmpDocName, &tmpDirName);
   numOfJobs++;

   strncpy (dirName, pDoc->DocDirectory, MAX_PATH);
   strncpy (docName, pDoc->DocDName, MAX_NAME_LENGTH);
   if (pFuncExportPrintDoc !=NULL)
     /* a export procedure is defined */
       ok = (*(Func3)pFuncExportPrintDoc)(
	   	(void *)document,
		(void *)PrintDocName,
		(void *)PrintDirName);
   else
     /* standard export */
     {
       strcpy (pDoc->DocDirectory, PrintDirName);
       strcpy (pDoc->DocDName, PrintDocName);

       /* save the schemas that have been created dynamically for
	  generic XML namespaces */
       BuildDocNatureTable (pDoc);
       firstGenericXML = TRUE;
       for (i = 0; i < pDoc->DocNNatures; i++)
	 {
	   if (pDoc->DocNatureSSchema[i]->SsIsXml)
	     /* this is a generic XML schema. Save it in a temp file */
	     {
	       if (firstGenericXML)
		 {
		   k = strlen (pDoc->DocSchemasPath);
		   pDoc->DocSchemasPath[k] = PATH_SEP;
		   pDoc->DocSchemasPath[k + 1] = EOS;
		   strcat (pDoc->DocSchemasPath, pDoc->DocDirectory);
		   firstGenericXML = FALSE;
		 }
	       FindCompleteName (pDoc->DocNatureName[i], "STR",
				 pDoc->DocDirectory, fileName, &k);
	       WriteStructureSchema (fileName , pDoc->DocNatureSSchema[i],
	                             pDoc->DocNatureSSchema[i]->SsCode);
	       FindCompleteName (pDoc->DocNaturePresName[i], "PRS",
				 pDoc->DocDirectory, fileName, &k);
	       pPSch = PresentationSchema (pDoc->DocNatureSSchema[i], pDoc);
	       WritePresentationSchema (fileName, pPSch,
					pDoc->DocNatureSSchema[i]);
	     }
	 }

       /* write the pivot representation of the document */
       docReadOnly = pDoc->DocReadOnly;
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
   if (newPres)
     {
       pDoc->DocSSchema->SsDefaultPSchema = savePres;
       TtaFreeMemory (newPres);
     }
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
  if (!PInitialized)
    {
      /* force the initialization of printing parameters */
      InitPrintParameters (0);
      PInitialized = TRUE;
    }

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
void TtaSetPrintCommand (const char *command)
{
  strcpy (pPrinter, command);
}


/*----------------------------------------------------------------------
  TtaSetPrintSchema fixes the printing schema.
  ----------------------------------------------------------------------*/
void TtaSetPrintSchema (const char *name)
{
  if (strlen(name) >= MAX_NAME_LENGTH)
    TtaError(ERR_invalid_parameter);
  else
    strcpy (PresSchema, name);
}


/*----------------------------------------------------------------------
  TtaSetPrintCommand sets the path of ps file.
  ----------------------------------------------------------------------*/
void TtaSetPsFile (const char *path)
{
  strcpy (PSdir, path);
}

