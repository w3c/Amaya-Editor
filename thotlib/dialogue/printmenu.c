/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
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
#include "viewcommands_f.h"
#include "writedoc_f.h"
#ifdef _WINDOWS
#include "thotprinter_f.h"
#include "wininclude.h"
#endif /* _WINDOWS */


static PathBuffer   PrintDirName;
static Name         PrintDocName;
static CHAR_T       Orientation[MAX_NAME_LENGTH];
static Func         pFuncExportPrintDoc = NULL;
static int          defPaperPrint;
static int          defManualFeed;
static int          defFirstPage;
static int          defLastPage;
static int          defNbCopies;
static int          defReduction;
static int          defPagesPerSheet;
static int          defPaginate;
static int          defPageSize;
static Name         PresSchema;


/*----------------------------------------------------------------------
  Print: interface to the Print program.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         Print (STRING name, STRING dir, STRING thotSch, STRING thotDoc, STRING realName, STRING output, int firstPage, int lastPage, int nCopies, int hShift, int vShift, int userOrientation, int reduction, int nbPagesPerSheet, int suppFrame, int manualFeed, int blackAndWhite, int repaginate, STRING viewsToPrint, STRING cssToPrint, Document document)
#else  /* __STDC__ */
static void         Print (name, dir, thotSch, thotDoc, realName, output, firstPage, lastPage, nCopies, hShift, vShift, userOrientation, reduction, nbPagesPerSheet, suppFrame, manualFeed, blackAndWhite, repaginate, viewsToPrint, cssToPrint, document)
STRING              name;
STRING              dir;
STRING              thotSch;
STRING              thotDoc;
STRING              realName;
STRING              output;
int                 firstPage;
int                 lastPage;
int                 nCopies;
int                 hShift;
int                 vShift;
int                 userOrientation;
int                 reduction;
int                 nbPagesPerSheet;
int                 suppFrame;
int                 manualFeed;
int                 blackAndWhite;
int                 repaginate;
STRING              viewsToPrint;
STRING              cssToPrint;
Document            document;
#endif /* __STDC__ */
{ 
   CHAR_T*                 ptr;
#  ifdef _WINDOWS
   static LPPRINTER_INFO_5 pInfo5;
   CHAR_T*                 printArgv [100];
   DWORD                   dwNeeded, dwReturned;
   HANDLE                  hLib;
   FARPROC                 ptrMainProc;
   int                     printArgc = 0;
#  else  /* !_WINDOWS */
   char                    cmd[1024];
   int                     res;
#  endif /* _WINDOWS */
   int                     i, j = 0;
   int                     frame;

   /* initialize the print command */
   ptr = TtaGetEnvString ("LANG");
#ifdef _WINDOWS
   printArgv[printArgc] = TtaAllocString (ustrlen (BinariesDirectory) + 7);
   ustrcpy (printArgv[printArgc], BinariesDirectory);
   ustrcat (printArgv[printArgc], WC_DIR_STR);
   ustrcat (printArgv[printArgc], TEXT("print"));
   printArgc++;   
   printArgv[printArgc] = TtaAllocString (6) ;
   ustrcpy (printArgv[printArgc], TEXT("-lang"));
   printArgc++;
   printArgv[printArgc] = TtaAllocString (ustrlen (ptr) + 1);
   ustrcpy (printArgv[printArgc], ptr);
   printArgc++;
#else /* !_WINDOWS */
   sprintf (cmd, "%s/print", BinariesDirectory);
   ustrcat (cmd, " -lang ");
   ustrcat (cmd, ptr);
#endif /* !_WINDOWS */

   if (thotSch != NULL && thotSch[0] != EOS)
     {
#ifdef _WINDOWS
       printArgv[printArgc] = TtaAllocString (5) ;
       ustrcpy (printArgv[printArgc], TEXT("-sch"));
       printArgc++;
       printArgv[printArgc] = TtaAllocString (ustrlen (thotSch) + 1);
       ustrcpy (printArgv[printArgc], thotSch);
       printArgc++;
       printArgv[printArgc] = TtaAllocString (5);
       ustrcpy (printArgv[printArgc], TEXT("-doc"));
       printArgc++;
       printArgv[printArgc] = TtaAllocString (ustrlen (thotDoc) + 1);
       ustrcpy (printArgv[printArgc], thotDoc);
       printArgc++;
#else  /* !_WINDOWS */
       ustrcat (cmd, " -sch ");
       ustrcat (cmd, thotSch);
       ustrcat (cmd, " -doc ");
       ustrcat (cmd, thotDoc);
#endif /* _WINDOWS */
     }

   /* transmit the server name */
   if (servername && servername[0] != EOS)
     { 
#ifdef _WINDOWS
       printArgv[printArgc] = TtaAllocString (9);
       ustrcpy (printArgv[printArgc], TEXT("-display"));
       printArgc++;
       printArgv[printArgc] = TtaAllocString (ustrlen (servername) + 1);
       ustrcpy (printArgv[printArgc], servername);
       printArgc++;
#else  /* _WINDOWS */
       ustrcat (cmd, " -display ");
       ustrcat (cmd, servername);
#endif /* _WINDOWS */
     }

   /* transmit the document name */
   if (realName)
     { 
#ifdef _WINDOWS
       printArgv[printArgc] = TtaAllocString (6);
       ustrcpy (printArgv[printArgc], TEXT("-name"));
       printArgc++;
       printArgv[printArgc] = TtaAllocString (ustrlen (realName) + 10);
       ustrcpy (printArgv[printArgc], realName);
       printArgc++;
#else  /* _WINDOWS */
       ustrcat (cmd, " -name ");
       ustrcat (cmd, realName);
#endif /* _WINDOWS */
     }

   /* transmit the orientation (default value is portrait) */
   if (userOrientation != 0)
     {
#ifdef _WINDOWS
       printArgv[printArgc] = TtaAllocString (11);
       ustrcpy (printArgv[printArgc], TEXT("-landscape"));
       printArgc++;
#else  /* _WINDOWS */
       ustrcat (cmd, " -landscape");
#endif /* _WINDOWS */
     }

   /* transmit the output command */
   if (PaperPrint)
     {
#ifdef _WINDOWS
       printArgv[printArgc] = TtaAllocString (5);
       ustrcpy (printArgv[printArgc], TEXT("-out"));
       printArgc++;
#else  /* _WINDOWS */
       ustrcat (cmd, " -out \"");
#endif /* _WINDOWS */
     }
   else
     {
#ifdef _WINDOWS
       printArgv[printArgc] = TtaAllocString (4);
       ustrcpy (printArgv[printArgc], TEXT("-ps"));
       printArgc++;
#else  /* _WINDOWS */
       ustrcat (cmd, " -ps \"");
#endif /* _WINDOWS */
     }

   if (output[0] != EOS)
     {
#ifdef _WINDOWS
       printArgv[printArgc] = TtaAllocString (ustrlen (output) + 1);
       ustrcpy (printArgv[printArgc], output);
       printArgc++;
#else  /* _WINDOWS */
       ustrcat (cmd, output);
       ustrcat (cmd, "\" ");
#endif /* _WINDOWS */
     }
   else
     {
#ifdef _WINDOWS
       printArgv[printArgc] = TtaAllocString (3);
       ustrcpy (printArgv[printArgc], TEXT("lp"));
       printArgc++;
#else  /* _WINDOWS */
       ustrcat (cmd, "lp");
       ustrcat (cmd, "\" ");
#endif /* _WINDOWS */
     }

   /* transmit visualization of empty boxes (default no) */
   if (suppFrame == 0)
     {
#ifdef _WINDOWS
       printArgv[printArgc] = TtaAllocString (10);
       ustrcpy (printArgv[printArgc], TEXT("-emptybox"));
       printArgc++;
#else  /* _WINDOWS */
       ustrcat (cmd, " -emptybox");
#endif /* _WINDOWS */
     }

   /* transmit black/white output (default no) */
   if (blackAndWhite != 0)
     {
#ifdef _WINDOWS
       printArgv[printArgc] = TtaAllocString (4);
       ustrcpy (printArgv[printArgc], TEXT("-bw"));
       printArgc++;
#else  /* _WINDOWS */
       ustrcat (cmd, " -bw");
#endif /* _WINDOWS */
     }

   /* transmit manualfeed (default no) */
   if (manualFeed != 0)
     {
#ifdef _WINDOWS
       printArgv[printArgc] = TtaAllocString (12);
       ustrcpy (printArgv[printArgc], TEXT("-manualfeed"));
       printArgc++;
#else  /* _WINDOWS */
       ustrcat (cmd, " -manualfeed");
#endif /* _WINDOWS */
     }

   /* transmit repaginate (default no) */
   if (repaginate != 0)
     {
#ifdef _WINDOWS
       printArgv[printArgc] = TtaAllocString (10);
       ustrcpy (printArgv[printArgc], TEXT("-paginate"));
       printArgc++;
#else  /* _WINDOWS */
       ustrcat (cmd, " -paginate");
#endif /* _WINDOWS */
     }

   /* transmit page format */
   if (ustrcmp (PageSize, TEXT("A4")))
     {
#ifdef _WINDOWS
       CHAR_T tmp [MAX_TXT_LEN];
       usprintf (tmp, TEXT("-P%s"), PageSize);
       printArgv[printArgc] = TtaAllocString (ustrlen (PageSize) + 3);
       ustrcpy (printArgv[printArgc], tmp);
       printArgc++;
#else  /* _WINDOWS */
       ustrcat (cmd, " -P");
       ustrcat (cmd, PageSize);
#endif /* _WINDOWS */
     }

   /* transmit window id */
#ifndef _WINDOWS 
   i = ustrlen (cmd);
#endif /* _WINDOWS */
   if (FrRef[0] != 0)
     {
#ifdef _WINDOWS
       printArgv[printArgc] = TtaAllocString (20);
       usprintf (printArgv[printArgc], TEXT("-w%ld"), FrRef[0]);
       printArgc++;
#else  /* _WINDOWS */
       sprintf (&cmd[i], " -w%ld", FrRef[0]);
#endif /* _WINDOWS */
     }
   else
     {
       frame = 1;
       while (frame <= MAX_FRAME && FrameTable[frame].FrDoc != document)
	 frame++;
       if (frame <= MAX_FRAME)
	 {
#ifdef _WINDOWS
	   printArgv[printArgc] = TtaAllocString (20);
	   usprintf (printArgv[printArgc], TEXT("-w%ld"), FrRef[frame]);
	   printArgc++;
#else  /* _WINDOWS */
	   sprintf (&cmd[i], " -w%ld", FrRef[frame]);
#endif /* _WINDOWS */
	 }
       else
	 {
#ifdef _WINDOWS
	   printArgv[printArgc] = TtaAllocString (20);
	   usprintf (printArgv[printArgc], TEXT("-w%ld"), FrRef[0]);
	   printArgc++;
#else  /* _WINDOWS */
	   sprintf (&cmd[i], " -w%ld", FrRef[0]);
#endif /* _WINDOWS */
	 }
     }

   /* transmit values */
   if (nbPagesPerSheet != 1)
     {
#ifdef _WINDOWS
       printArgv[printArgc] = TtaAllocString (6);
       ustrcpy (printArgv[printArgc], TEXT("-npps"));
       printArgc++;
       printArgv[printArgc] = TtaAllocString (5);
       usprintf (printArgv[printArgc], TEXT("%d"), nbPagesPerSheet);
       printArgc++;
#else  /* _WINDOWS */
       i = ustrlen (cmd);
       sprintf (&cmd[i], " -npps %d ", nbPagesPerSheet);
#endif /* _WINDOWS */
     }

   if (firstPage > 1 || lastPage < 999)
     {
#ifdef _WINDOWS
       printArgv[printArgc] = TtaAllocString (7);
       usprintf (printArgv[printArgc], TEXT("-F%d"), firstPage);
       printArgc++;
       printArgv[printArgc] = TtaAllocString (7);
       usprintf (printArgv[printArgc], TEXT("-L%d"), lastPage);
       printArgc++;
#else  /* _WINDOWS */
   i = ustrlen (cmd);
   sprintf (&cmd[i], " -F%d -L%d ", firstPage, lastPage);
#endif /* _WINDOWS */
     }

   if (nCopies > 1)
     {
#ifdef _WINDOWS
       printArgv[printArgc] = TtaAllocString (8);
       usprintf (printArgv[printArgc], TEXT("-#%d"), nCopies);
       printArgc++;
#else  /* _WINDOWS */
       i = ustrlen (cmd);
       sprintf (&cmd[i], " -#%d ", nCopies);
#endif /* _WINDOWS */
     }

   if (hShift != 0)
     {
#ifdef _WINDOWS
       printArgv[printArgc] = TtaAllocString (7);
       usprintf (printArgv[printArgc], TEXT("-H%d"), hShift);
       printArgc++;
#else  /* _WINDOWS */
       i = ustrlen (cmd);
       sprintf (&cmd[i], " -H%d ", hShift);
#endif /* _WINDOWS */
     }

   if (vShift != 0)
     {
#ifdef _WINDOWS
       printArgv[printArgc] = TtaAllocString (7);
       usprintf (printArgv[printArgc], TEXT("-V%d"), vShift);
       printArgc++;
#else  /* _WINDOWS */
       i = ustrlen (cmd);
       sprintf (&cmd[i], " -V%d ", vShift);
#endif /* _WINDOWS */
     }

   if (reduction != 100)
     {
#ifdef _WINDOWS
       printArgv[printArgc] = TtaAllocString (7);
       usprintf (printArgv[printArgc], TEXT("-%%%d"), reduction);
       printArgc++;
#else  /* _WINDOWS */
       i = ustrlen (cmd);
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
       printArgv[printArgc] = TtaWCSdup (TEXT("-v"));
       printArgc++;
       printArgv[printArgc] = TtaAllocString (50);
       j = 0;
#else  /* _WINDOWS */
       j = ustrlen (cmd);
       sprintf (&cmd[j], " -v ");
       j = ustrlen (cmd);
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
		   printArgv[printArgc] = TtaWCSdup (TEXT("-v"));
		   printArgc++;
		   printArgv[printArgc] = TtaAllocString (50);
		   j = 0;
#else  /* _WINDOWS */
		   j = ustrlen (cmd);
		   sprintf (&cmd[j], " -v ");
		   j = ustrlen (cmd);
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
       /* skip leading spaces */ 
       while (cssToPrint[i] == SPACE)
	 i++;
       /* insert the first flag */
       if (cssToPrint[i] != EOS)
	 {
	   /* insert the flag -v before each view name */
#ifdef _WINDOWS
	   printArgv[printArgc] = TtaWCSdup (TEXT("-css"));
	   printArgc++;
	   printArgv[printArgc] = TtaAllocString (50);
	   j = 0;
#else  /* _WINDOWS */
	   j = ustrlen (cmd);
           sprintf (&cmd[j], " -css ");
	   j = ustrlen (cmd);
#endif /* _WINDOWS */

	   while (cssToPrint[i] != EOS)
	     {
	       /* is it a space? */
	       if (cssToPrint[i] == SPACE)
		 {
		   i++;
		   /* skip multiple spaces */
		   while (cssToPrint[i] == SPACE)
		     i++;
		   if (cssToPrint[i] != EOS)
		     {
#ifdef _WINDOWS
		       printArgv[printArgc][j++] = EOS;
		       printArgc++;
		       printArgv[printArgc] = TtaWCSdup (TEXT("-css"));
		       printArgc++;
		       printArgv[printArgc] = TtaAllocString (50);
		       j = 0;
#else  /* _WINDOWS */
		       j = ustrlen (cmd);
		       sprintf (&cmd[j], " -css ");
		       j = ustrlen (cmd);
#endif /* _WINDOWS */
		     }
		 }
	       else
		 {
		   /* copy the character */
#ifdef _WINDOWS
		   printArgv[printArgc][j++] = cssToPrint[i];
#else /* _WINDOWS */
		   cmd[j++] = cssToPrint[i];
		   cmd[j] = EOS;
#endif /* _WINDOWS */
		   /* process next char */
		   i++;
		 }
	     }
	 }
#ifdef _WINDOWS
       printArgv[printArgc][j] = EOS;
       printArgc++;
#endif /* _WINDOWS */
   }
   /* transmit the path or source file */
#ifdef _WINDOWS 
   printArgv[printArgc] = TtaWCSdup (TEXT("-removedir"));
   printArgc++;
   printArgv[printArgc] = TtaAllocString (ustrlen (dir) + ustrlen (name) + 6);
   usprintf  (printArgv[printArgc], TEXT("%s\\%s.PIV"), dir, name);
   printArgc++;
   WIN_ReleaseDeviceContext ();

   hLib = LoadLibrary (TEXT("thotprinter"));
   if (!hLib)
      return /* FATAL_EXIT_CODE */;
   ptrMainProc = GetProcAddress (hLib, "PrintDoc");
   if (!ptrMainProc) {
      FreeLibrary (hLib);
      return /* FATAL_EXIT_CODE */;
   }

   ptrMainProc (FrRef [currentFrame], printArgc, printArgv, TtPrinterDC, TtIsTrueColor, TtWDepth, name, dir, hInstance, buttonCommand);
   FreeLibrary (hLib);

   if (!IsWindowEnabled (FrRef[currentFrame]))
      EnableWindow (FrRef[currentFrame], TRUE);
   SetFocus (FrRef[currentFrame]);
   for (i = 0; i < printArgc; i++)
       TtaFreeMemory (printArgv[i]);
   if (TtPrinterDC) {
      DeleteDC (TtPrinterDC);
      TtPrinterDC = (HDC) 0;
   }
#else /* !_WINDOWS */
   cmd[j] = EOS;
   i = ustrlen (cmd);

   sprintf (&cmd[i], " -removedir %s/%s.PIV &", dir, name);
   res = system (cmd);
   if (res == -1)
      TtaDisplaySimpleMessage (CONFIRM, LIB, TMSG_ERROR_PS_TRANSLATION);
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
   InitPrintParameters
   initializes the printing parameters.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void        InitPrintParameters (Document document)
#else  /* __STDC__ */
void        InitPrintParameters (document)
Document document;
#endif /* __STDC__ */
{
   PtrDocument pDoc;
   CHAR_T*     ptr;
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
          ustrcpy (pPrinter, TEXT(""));
       else
           ustrcpy (pPrinter, ptr);
       PSdir[0] = WC_EOS;
       PrintingDoc = 0;
       defPaperPrint = TRUE;
       defManualFeed = FALSE;
       defFirstPage = 0;
       defLastPage = 999;
       defNbCopies = 1;
       defReduction = 100;
       defPagesPerSheet = 1;
       defPageSize= PP_A4;
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
       if (defPageSize == PP_A4)
          ustrcpy(PageSize, TEXT("A4"));
       else
           ustrcpy(PageSize, TEXT("US"));
       if (pDoc != NULL)
	 {
	   if (pDoc->DocDirectory[0] == DIR_SEP)
	     usprintf (PSdir, TEXT("%s/%s.ps"), pDoc->DocDirectory, pDoc->DocDName);
	   else
	     {
	       ptr = NULL;
	       ptr = TtaGetEnvString ("APP_TMPDIR");
	       if (ptr == NULL || *ptr == WC_EOS || !TtaCheckDirectory (ptr))
		 {
		   ptr = NULL;
		   ptr = TtaGetEnvString ("TMPDIR");
		 }
	       if (ptr != NULL && TtaCheckDirectory (ptr))
		 {
		   ustrcpy(PSdir,ptr);
		   lg = ustrlen(PSdir);
		   if (PSdir[lg - 1] == DIR_SEP)
		     PSdir[--lg] = EOS;
		 }
	       else
		 {
#                  ifdef _WINDOWS
		   ustrcpy (PSdir, TEXT("C:\\TEMP"));
#                  else  /* !_WINDOWS */
		   ustrcpy (PSdir,"/tmp");
#                  endif /* !_WINDOWS */
		   lg = ustrlen (PSdir);
		 }
	       usprintf (&PSdir[lg], TEXT("/%s.ps"), pDoc->DocDName);
	     }
	 }
     }
}

/*----------------------------------------------------------------------
   TtcPrint standard handler for the Print action.  
   Calls TtaPrint to print the current view.
   ----------------------------------------------------------------------*/  
#ifdef __STDC__
void                TtcPrint (Document document, View view)
#else  /* __STDC__ */
void                TtcPrint (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   PathBuffer          viewsToPrint;

   ustrcpy (viewsToPrint, TtaGetViewName (document, view));
   ustrcat (viewsToPrint, TEXT(" "));
   TtaPrint (document, viewsToPrint, NULL);
}

/*----------------------------------------------------------------------
   TtaGetPrintNames generates and returns a directory name to store
   temporary files neede to start a print process.
   The function creates the directory if it doesn't already exist.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void             TtaGetPrintNames (STRING *printDocName, STRING *printDirName)
#else  /* __STDC__ */
void             TtaGetPrintNames (printDocName, printDirName)
STRING          *printDocName;
STRING          *printDirName;
#endif /* __STDC__ */
{
   ThotPid             pid = ThotPid_get ();
   CHAR_T*             dirString;
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
       ustrcpy (PrintDirName, dirString);
       lg = ustrlen(PrintDirName);
       if (PrintDirName[lg - 1] == DIR_SEP)
         PrintDirName[--lg] = EOS;
     }
   else
     {
       ustrcpy (PrintDirName, TtaGetEnvString ("TMPDIR"));
       lg = ustrlen (PrintDirName);
     }

   usprintf (PrintDocName, TEXT("Thot%ld"), (long) pid + numOfJobs);
   ustrcpy (&PrintDirName[lg], WC_DIR_STR);
   ustrcat (&PrintDirName[lg], PrintDocName);
   if (!TtaCheckDirectory (PrintDirName))
      TtaMakeDirectory (PrintDirName);
}

/*----------------------------------------------------------------------
   TtaPrint
   interface to the multiview print command.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaPrint (Document document, STRING viewNames, STRING cssNames)
#else  /* __STDC__ */
void                TtaPrint (document, viewNames, cssNames)
Document            document;
STRING              viewNames;
STRING              cssNames;
#endif /* __STDC__ */
{
   PtrDocument         pDoc;
   PathBuffer          dirName;
   Name                docName;
   Name                savePres, newPres;
   STRING              tmpDirName, tmpDocName;
   int                 orientation;
   ThotBool	           docReadOnly;
   ThotBool            ok;

   pDoc = LoadedDocument[document - 1];
   /* prepares the execution of the print command */
   ustrcpy (savePres, pDoc->DocSSchema->SsDefaultPSchema);
   if (PresSchema[0] != EOS)
      ustrcpy (newPres, PresSchema);
   else
     ConfigGetPSchemaForPageSize (pDoc->DocSSchema, PageSize, newPres);
     
   if (newPres[0] != EOS)
      ustrcpy (pDoc->DocSSchema->SsDefaultPSchema, newPres);
   if (ThotLocalActions[T_rextprint]!=NULL && 
      ustrcmp(pDoc->DocSSchema->SsDefaultPSchema, savePres))
     {
       TtaDisplayMessage(INFO, TtaGetMessage(LIB,TMSG_CHANGE_PSCH), newPres);
     }

   /* initialise temporary directory and temporary file names */
   TtaGetPrintNames (&tmpDocName, &tmpDirName);
   numOfJobs++;

   ustrncpy (dirName, pDoc->DocDirectory, MAX_PATH);
   ustrncpy (docName, pDoc->DocDName, MAX_NAME_LENGTH);
   if (pFuncExportPrintDoc !=NULL)
     /* a export procedure is defined */
       ok = (*pFuncExportPrintDoc)(document, PrintDocName, PrintDirName);
   else
     /* standard export */
     {
       docReadOnly = pDoc->DocReadOnly;

       ustrcpy (pDoc->DocDirectory, PrintDirName);
       ustrcpy (pDoc->DocDName, PrintDocName);
       pDoc->DocReadOnly = FALSE;

       ok = WriteDocument (pDoc, 5);

       pDoc->DocReadOnly = docReadOnly;
       ustrncpy (pDoc->DocDirectory, dirName, MAX_PATH);
       ustrncpy (pDoc->DocDName, docName, MAX_NAME_LENGTH);
     }

   /* searches the paper orientation for the presentation scheme */
   ConfigGetPresentationOption(pDoc->DocSSchema, TEXT("orientation"), Orientation);
   if (!ustrcmp (Orientation, TEXT("Landscape")))
     orientation = 1;
   else
     orientation = 0;

   /* restores the presentation scheme */
   ustrcpy (pDoc->DocSSchema->SsDefaultPSchema, savePres);

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
   ustrcpy (pDoc->DocSSchema->SsDefaultPSchema, savePres);
}

/*----------------------------------------------------------------------
  TtaSetPrintExportFunc: Sets a non-standard document export function for printing
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void TtaSetPrintExportFunc (Func exportFunc)
#else /* __STDC__ */
void TtaSetPrintExportFunc (exportFunc)
Func exportFunc;
#endif /*__STDC__ */
{
  pFuncExportPrintDoc = exportFunc;
}

/*----------------------------------------------------------------------
  TtaSetPrintParameter: Sets a print parameter
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void TtaSetPrintParameter (PrintParameter parameter, int value)
#else /* __STDC__ */
void TtaSetPrintParameter (parameter, value)
PrintParameter parameter;
int value;
#endif /*__STDC__ */
{
   if (ThotLocalActions[T_rprint] == NULL)
     /* force the initialization of printing parameters */
     InitPrintParameters (0);

   switch (parameter)
     {
     case PP_FirstPage:
       if (value <0 || value >999)
          TtaError(ERR_invalid_parameter);
       else
	 FirstPage = value;
       break;
     case PP_LastPage:
       if (value <0 || value >999)
	 TtaError(ERR_invalid_parameter);
       else
	 LastPage = value;
       break;
     case PP_Scale:
       if (value <0 || value >999)
	 TtaError(ERR_invalid_parameter);
       else
	 Reduction = value;
       break;
     case PP_NumberCopies:
       if (value <0 || value >999)
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
       if (value != 1 || value != 2 || value != 4)
	 TtaError(ERR_invalid_parameter);
       else
	 PagesPerSheet = value;
       break;
     case PP_PaperSize:
       if (value == PP_A4)
          ustrcpy (PageSize, TEXT("A4"));
       else if (value == PP_US)
            ustrcpy (PageSize, TEXT("US"));
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
#ifdef __STDC__
int TtaGetPrintParameter (PrintParameter parameter)
#else /* __STDC__ */
int TtaGetPrintParameter (parameter)
PrintParameter parameter;
int value;
#endif /*__STDC__ */
{
  switch (parameter)
    {
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
	return (PP_ON);
      break;
    case PP_PagesPerSheet:
      return (PagesPerSheet);
      break;
    case PP_PaperSize:
      if (!ustrcmp (PageSize, TEXT("A4")))
	return (PP_A4);
      else
	return (PP_US);
      break;
    case PP_Destination:
      if (PaperPrint)
	return (PP_ON);
      else
	return (PP_ON);
      break;
    default:
      TtaError(ERR_invalid_parameter);
      return (0);
    }
}


/*----------------------------------------------------------------------
  TtaSetPrintCommand sets the print command.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaSetPrintCommand (STRING command)
#else  /* __STDC__ */
void                TtaSetPrintCommand (command)
STRING command;
#endif /* __STDC__ */
{
  ustrcpy (pPrinter, command);
}


/*----------------------------------------------------------------------
  TtaGetPrintCommand returns the print command.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaGetPrintCommand (STRING command)
#else  /* __STDC__ */
void                TtaGetPrintCommand (command)
STRING command;
#endif /* __STDC__ */
{
  if (command == NULL)
    TtaError(ERR_invalid_parameter);
  else
    ustrcpy (command, pPrinter);
}


/*----------------------------------------------------------------------
  TtaSetPrintSchema fixes the printing schema.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaSetPrintSchema (CHAR_T* name)
#else  /* __STDC__ */
void                TtaSetPrintSchema (name)
CHAR_T*             name;
#endif /* __STDC__ */
{
  if (ustrlen(name) >= MAX_NAME_LENGTH)
    TtaError(ERR_invalid_parameter);
  else
    ustrcpy (PresSchema, name);
}


/*----------------------------------------------------------------------
  TtaSetPrintCommand sets the path of ps file.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaSetPsFile (STRING path)
#else  /* __STDC__ */
void                TtaSetPsFile (path)
STRING command;
#endif /* __STDC__ */
{
  ustrcpy (PSdir, path);
}


/*----------------------------------------------------------------------
  TtaGetPsFile returns the path of ps file.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaGetPsFile (STRING path)
#else  /* __STDC__ */
void                TtaGetPsFile (/*char *path*/)
STRING path;
#endif /* __STDC__ */
{
  if (path == NULL)
    TtaError(ERR_invalid_parameter);
  else
    ustrcpy (path, PSdir);
}


/*----------------------------------------------------------------------
  CallbackPrintmenu
  callback associated to the PrintSetup form 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CallbackPrintmenu (int ref, int val, STRING txt)
#else  /* __STDC__ */
void                CallbackPrintmenu (ref, val, txt)
int                 ref;
int                 val;
STRING              txt;

#endif /* __STDC__ */
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
#         ifndef _WINDOWS
		  TtaSetTextForm (NumZonePrinterName, pPrinter);
#         endif /* !_WINDOWS */
		}
	      break;
	    case 1:
	      if (NewPaperPrint)
		{
		  NewPaperPrint = FALSE;
#         ifndef _WINDOWS
		  TtaSetTextForm (NumZonePrinterName, PSdir);
#         endif /* !_WINDOWS */
		}
	      break;
	    }
	  break;
	case NumMenuPaperFormat:
	  /* page size submenu */
	  switch (val)
	    {
	    case 0:
	      ustrcpy (PageSize, TEXT("A4"));
	      break;
	    case 1:
	      ustrcpy (PageSize, TEXT("US"));
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
	    if (NewPaperPrint)
	      /* text capture zone for the printer name */
	      ustrncpy (pPrinter, txt, MAX_PATH);
	    else
	      /* text capture zone for the name of the PostScript file */
	      ustrncpy (PSdir, txt, MAX_PATH);
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
	      if (ThotLocalActions[T_rextprint]!=NULL)
		(*ThotLocalActions[T_rextprint])(ref, val, txt);
	      break;
	    default:
	      break;
	    }
	  break;
	default:
	  if (ThotLocalActions[T_rextprint]!=NULL)
	    (*ThotLocalActions[T_rextprint])(ref, val, txt);
	  break;
	}
    }
}

/*----------------------------------------------------------------------
   TtcPrintSetup
   standard handler for a PrintSetup action.
   Prepares and displays a form.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcPrintSetup (Document document, View view)
#else  /* __STDC__ */
void                TtcPrintSetup (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   int              i;
   CHAR_T             bufMenu[MAX_TXT_LEN];

   if (document == 0)
     return;

   /* Print form */
   InitPrintParameters (document);
#  ifndef _WINDOWS
   TtaNewSheet (NumFormPrint, TtaGetViewFrame (document, view), TtaGetMessage (LIB, TMSG_LIB_PRINT), 1, TtaGetMessage (LIB, TMSG_LIB_CONFIRM), FALSE, 2, 'L', D_CANCEL);

   i = 0;
   usprintf (&bufMenu[i], TEXT("B%s"), TtaGetMessage (LIB, TMSG_MANUAL_FEED));
   TtaNewToggleMenu (NumMenuOptions, NumFormPrint, TtaGetMessage (LIB, TMSG_OPTIONS), 1, bufMenu, NULL, FALSE);
   if (ManualFeed)
      TtaSetToggleMenu (NumMenuOptions, 0, TRUE);
#     endif /* _WINDOWS */

   /* Paper format submenu */
   i = 0;
   usprintf (&bufMenu[i], TEXT("B%s"), TtaGetMessage (LIB, TMSG_A4));
   i += ustrlen (&bufMenu[i]) + 1;
   usprintf (&bufMenu[i], TEXT("B%s"), TtaGetMessage (LIB, TMSG_US));
   TtaNewSubmenu (NumMenuPaperFormat, NumFormPrint, 0, TtaGetMessage (LIB, TMSG_PAPER_SIZE), 2, bufMenu, NULL, FALSE);
#  ifndef _WINDOWS
   if (!ustrcmp (PageSize, TEXT("US")))
      TtaSetMenuForm (NumMenuPaperFormat, 1);
   else
      TtaSetMenuForm (NumMenuPaperFormat, 0);
#  endif /* !_WINDOWS */

   /* Print to paper/ Print to file submenu */
   i = 0;
   usprintf (&bufMenu[i], TEXT("B%s"), TtaGetMessage (LIB, TMSG_PRINTER));
   i += ustrlen (&bufMenu[i]) + 1;
   usprintf (&bufMenu[i], TEXT("B%s"), TtaGetMessage (LIB, TMSG_PS_FILE));
   TtaNewSubmenu (NumMenuSupport, NumFormPrint, 0,
                  TtaGetMessage (LIB, TMSG_OUTPUT), 2, bufMenu, NULL, TRUE);
   /* text capture zone for the printer name */
#  ifndef _WINDOWS
   TtaNewTextForm (NumZonePrinterName, NumFormPrint, NULL, 30, 1, FALSE);
#  endif /* !_WINDOWS */

   /* initialization of the PaperPrint selector */
   NewPaperPrint = PaperPrint;
#  ifndef _WINDOWS
   if (PaperPrint) {
      TtaSetMenuForm (NumMenuSupport, 0);
      TtaSetTextForm (NumZonePrinterName, pPrinter);
   } else {
          TtaSetMenuForm (NumMenuSupport, 1);
          TtaSetTextForm (NumZonePrinterName, PSdir);
   } 

   /* activates the Print form */
   TtaShowDialogue (NumFormPrint, FALSE);
#  endif /* !_WINDOWS */
}

