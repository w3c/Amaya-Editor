/*
   gestion des messages et menus de l'impression.

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
#include "thotfile.h"

#undef EXPORT
#define EXPORT extern
#include "platform_tv.h"
#include "edit_tv.h"
#include "appdialogue_tv.h"
#include "frame_tv.h"

#include "appli_f.h"
#include "applicationapi_f.h"
#include "attrmenu_f.h"
#include "tree_f.h"
#include "browser_f.h"
#include "search_f.h"
#include "searchmenu_f.h"
#include "textcommands_f.h"
#include "config_f.h"
#include "structcreation_f.h"
#include "creationmenu_f.h"
#include "dialogapi_f.h"
#include "views_f.h"
#include "viewapi_f.h"
#include "viewapi_f.h"
#include "dofile_f.h"
#include "viewcommands_f.h"
#include "exceptions_f.h"
#include "absboxlist_f.h"
#include "structmodif_f.h"
#include "presentmenu_f.h"
#include "printmenu_f.h"
#include "structselect_f.h"
#include "selectmenu_f.h"

static PathBuffer   psdir;
static boolean      PaperPrint;
static boolean      ManualFeed;
static boolean      NewPaperPrint;
static char	    pPrinter[MAX_NAME_LENGTH];
static PtrDocument  pDocPrint;
static char	    PageSize[MAX_NAME_LENGTH];
static char	    Orientation[MAX_NAME_LENGTH];

/* ---------------------------------------------------------------------- */

/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         initImpression (int userOrientation, char *thotDir, char *tempDir, char *dir, char *name, char *realName,
				    char *printer, int pid, long thotWin, char *thotSch, char *thotDoc, char *printProcessing)
#else  /* __STDC__ */
static void         initImpression (userOrientation, thotDir, tempDir, dir, name, realName, printer, pid, thotWin, thotSch, thotDoc, printProcessing)
int                 userOrientation;
char               *thotDir;
char               *tempDir;
char               *dir;
char               *name;
char               *realName;
char               *printer;
int                 pid;
long                thotWin;
char               *thotSch;
char               *thotDoc;
char               *printProcessing;

#endif /* __STDC__ */
{
   char                cmd[800];
   char               *bakName;
   char               *pivName;

   if (userOrientation == 0)
      strcpy (Orientation, "Portrait");
   else
      strcpy (Orientation, "Landscape");

   sprintf (cmd, "/bin/mkdir %s\n", tempDir);
   system (cmd);
   sprintf (cmd, "chmod +rwx '%s'\n", tempDir);
   system (cmd);

   bakName = (char *) TtaGetMemory (strlen (name) + 5);
   sprintf (bakName, "%s.BAK", name);
   pivName = (char *) TtaGetMemory (strlen (name) + 5);
   sprintf (pivName, "%s.PIV", name);
   sprintf (cmd, "/bin/mv '%s'/'%s' '%s'/'%s'\n", dir, bakName, tempDir, pivName);
   system (cmd);
   sprintf (cmd, "printProcessing=%s\n", printProcessing);
   system (cmd);
   sprintf (cmd, "export printProcessing\n");
   system (cmd);
   sprintf (cmd, "realName=%s\n", realName);
   system (cmd);
   sprintf (cmd, "export realName\n");
   system (cmd);
   /*  sprintf (cmd, "printer_or_psname=%s\n", printer) ;
      system (cmd) ;
      sprintf (cmd, "export printer_or_psname\n") ;
      system (cmd) ; */
   sprintf (cmd, "thotpid=%d\n", pid);
   system (cmd);
   sprintf (cmd, "export pid\n");
   system (cmd);
   sprintf (cmd, "thotwindow=%ld\n", thotWin);
   system (cmd);
   sprintf (cmd, "export thotwindow\n");
   system (cmd);
   sprintf (cmd, "BIN=%s/bin\n", thotDir);
   system (cmd);
   sprintf (cmd, "export BIN\n");
   system (cmd);
   sprintf (cmd, "THOTDIR=%s\n", thotDir);
   system (cmd);
   sprintf (cmd, "export THOTDIR\n");
   system (cmd);
   sprintf (cmd, "THOTSCH=%s\n", thotSch);
   system (cmd);
   sprintf (cmd, "export THOTSCH\n");
   system (cmd);
   sprintf (cmd, "THOTDOC=%s:%s\n", tempDir, thotDoc);
   system (cmd);
   sprintf (cmd, "export THOTDOC\n");
   system (cmd);
}

/* ---------------------------------------------------------------------- */
/* |    Print effectue le lancement du shell pour l'impression.      | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void          Print (char *name, char *dir, char *thotSch, char *thotDoc, char *thotpres, char *realName, char *realDir, char *printer, int firstPage, int lastPage, int nbCopies, int hShift, int vShift, int userOrientation, int reduction, int nbPagesPerSheet, int suppFrame, int manualFeed, int blackAndWhite, int repaginate, char *viewsToPrint)

#else  /* __STDC__ */
static void        Print (name, dir, thotSch, thotDoc, thotpres, realName, realDir, printer, firstPage, lastPage, nbCopies, hShift, vShift, userOrientation, reduction, nbPagesPerSheet, suppFrame, manualFeed, blackAndWhite, repaginate, viewsToPrint)
char               *name;
char               *dir;
char               *thotSch;
char               *thotDoc;
char               *thotpres;
char               *realName;
char               *realDir;
char               *printer;
int                 firstPage;
int                 lastPage;
int                 nbCopies;
int                 hShift;
int                 vShift;
int                 userOrientation;
int                 reduction;
int                 nbPagesPerSheet;
int                 suppFrame;
int                 manualFeed;
int                 blackAndWhite;
int                 repaginate;
char               *viewsToPrint;

#endif /* __STDC__ */

{
   ThotPid             pid = ThotPid_get ();

#ifndef NEW_WILLOWS
   char                cmd[800];
   int                 res;
   char               *thotDir;
   char               *tempDir;

   thotDir = (char *) TtaGetEnvString ("THOTDIR");
   if (!thotDir)
	thotDir = ThotDir ();
   tempDir = (char *) TtaGetMemory (40);
   sprintf (tempDir, "/tmp/Thot%d", pid);

   initImpression (userOrientation, thotDir, tempDir, dir, name, realName, printer, pid, FrRef[0], thotSch, thotDoc, "PRINT");
   if (printer[0] != '\0')
      sprintf (cmd, "%s/print %s %s %d %d %d 0 %s %s \"%s\" %s %d %d %d %s %d %d %d %d %d %ld Print &\n",
	       BinariesDirectory, name, tempDir, repaginate, firstPage, lastPage, viewsToPrint, realName, printer, PageSize, nbCopies,
	       hShift, vShift, Orientation, reduction, nbPagesPerSheet, suppFrame, manualFeed, blackAndWhite, FrRef[0]);
   else
      sprintf (cmd, "%s/print %s %s %d %d %d 0 %s %s %s %s %d %d %d %s %d %d %d %d %d %ld Print &\n",
	       BinariesDirectory, name, tempDir, repaginate, firstPage, lastPage, viewsToPrint, realName, "lp", PageSize, nbCopies,
	       hShift, vShift, Orientation, reduction, nbPagesPerSheet, suppFrame, manualFeed, blackAndWhite, FrRef[0]);

   res = system (cmd);
   if (res == -1)
      TtaDisplaySimpleMessage (CONFIRM, LIB, ERROR_PS_TRANSLATION);
#endif /* NEW_WILLOWS */
}


/* ---------------------------------------------------------------------- */
/* |    SauverPS effectue le lancement du shell pour sauvegarde PS.     | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void          SauverPS (char *name, char *dir, char *thotSch, char *thotDoc, char *thotpres, char *realName, char *realDir, char *psName, int firstPage, int lastPage, int nbCopies, int hShift, int vShift, int userOrientation, int reduction, int nbPagesPerSheet, int suppFrame, int manualFeed, int blackAndWhite, int repaginate, char *viewsToPrint)

#else  /* __STDC__ */
static void         SauverPS (name, dir, thotSch, thotDoc, thotpres, realName, realDir, psName, firstPage, lastPage, nbCopies, hShift, vShift, userOrientation, reduction, nbPagesPerSheet, suppFrame, manualFeed, blackAndWhite, repaginate, viewsToPrint)
char               *name;
char               *dir;
char               *thotSch;
char               *thotDoc;
char               *thotpres;
char               *realName;
char               *realDir;
char               *psName;
int                 firstPage;
int                 lastPage;
int                 nbCopies;
int                 hShift;
int                 vShift;
int                 userOrientation;
int                 reduction;
int                 nbPagesPerSheet;
int                 suppFrame;
int                 manualFeed;
int                 blackAndWhite;
int                 repaginate;
char               *viewsToPrint;

#endif /* __STDC__ */

{
   char                cmd[800];
   int                 res;
   char               *thotDir;

   char               *tempDir;
   ThotPid             pid = ThotPid_get ();

   thotDir = TtaGetEnvString ("THOTDIR");
   if (!thotDir)
     {
	thotDir = ThotDir ();
     }
   tempDir = (char *) TtaGetMemory (40);
   sprintf (tempDir, "/tmp/Thot%d", pid);
   initImpression (userOrientation, thotDir, tempDir, dir, name, realName, psName, pid, FrRef[0], thotSch, thotDoc, "SAVEPS");

   if (psName[0] != '\0')
      sprintf (cmd, "%s/print %s %s %d %d %d 0 %s %s %s %s %d %d %d %s %d %d %d %d %d %ld Sauver &\n",
	       BinariesDirectory, name, tempDir, repaginate, firstPage, lastPage, viewsToPrint, realName, psName, PageSize, nbCopies,
	       hShift, vShift, Orientation, reduction, nbPagesPerSheet, suppFrame, manualFeed, blackAndWhite, FrRef[0]);
   else
      sprintf (cmd, "%s/print %s %s %d %d %d 0 %s %s %s %s %d %d %d %s %d %d %d %d %d %ld Sauver &\n",
	       BinariesDirectory, name, tempDir, repaginate, firstPage, lastPage, viewsToPrint, realName, "out.ps", PageSize, nbCopies,
	       hShift, vShift, Orientation, reduction, nbPagesPerSheet, suppFrame, manualFeed, blackAndWhite, FrRef[0]);

   res = system (cmd);
   if (res == -1)
      TtaDisplaySimpleMessage (CONFIRM, LIB, ERROR_PS_TRANSLATION);
}


/* ---------------------------------------------------------------------- */
/* |  ConnectPrint initialise les valeurs du formulaire d'impression.   | */
/* ---------------------------------------------------------------------- */
static void         ConnectPrint ()
{
   char               *ptr;

   if (ThotLocalActions[T_rprint] == NULL)
     {
	/* Connecte les actions liees au traitement du print */
	TteConnectAction (T_rprint, (Proc) CallbackPrintmenu);
	/* read DEFAULTPRINTER shell variable */
	ptr = TtaGetEnvString ("THOTPRINT");
	if (ptr == NULL)
	   strcpy (pPrinter, "");
	else
	   strcpy (pPrinter, ptr);
	psdir[0] = '\0';
	PaperPrint = TRUE;
	ManualFeed = FALSE;
	strcpy (PageSize, "A4");
     }
}


/* ---------------------------------------------------------------------- */
/* |  TraiteMenuImprimer traite les retours du formulaire d'impression. | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtcPrint (Document document, View view)
#else  /* __STDC__ */
void                TtcPrint (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   PathBuffer          dirName;
   Name                docName;
   PathBuffer          viewsToPrint;
   boolean             ok;
   Name                savePres, newPres;

   pDocPrint = LoadedDocument[document - 1];
   ConnectPrint ();
   /* prepare le lancement de l'impression */
   strcpy (savePres, pDocPrint->DocSSchema->SsDefaultPSchema);
   ConfigGetPSchemaForPageSize (pDocPrint->DocSSchema, PageSize, newPres);
   if (newPres[0] != '\0')
      strcpy (pDocPrint->DocSSchema->SsDefaultPSchema, newPres);

   /* la repagination se fait dans le print */
   SavePath (pDocPrint, dirName, docName);

   strcpy (pDocPrint->DocDirectory, "/tmp");
   strcpy (pDocPrint->DocDName, "ThotXXXXXX");
#ifdef WWW_MSWINDOWS
   _mktemp (pDocPrint->DocDName);
#else  /* WWW_MSWINDOWS */
   mktemp (pDocPrint->DocDName);
#endif /* !WWW_MSWINDOWS */

   ok = WriteDocument (pDocPrint, 2);

   /* restaure le schema de presentation */
   strcpy (pDocPrint->DocSSchema->SsDefaultPSchema, savePres);

   /* on fait une sauvegarde automatique */
   if (ok)
     {
	strcpy (viewsToPrint, TtaGetViewName (document, view));
	strcat (viewsToPrint, " ");
	if (PaperPrint)
	   Print (pDocPrint->DocDName,
		     pDocPrint->DocDirectory,
		     pDocPrint->DocSchemasPath,
		     DocumentPath,
		     pDocPrint->DocSSchema->SsDefaultPSchema,
		     docName, dirName, pPrinter,
		     1, 999, 1, 0, 0, 0,
		     100, 1, TRUE,
		     (int) ManualFeed, 0,
		     1,
		     viewsToPrint);
	else if (psdir[0] != '\0')
	   SauverPS (pDocPrint->DocDName,
		     pDocPrint->DocDirectory,
		     pDocPrint->DocSchemasPath,
		     DocumentPath,
		     pDocPrint->DocSSchema->SsDefaultPSchema,
		     docName, dirName, psdir,
		     1, 999, 1, 0, 0, 0,
		     100, 1, TRUE,
		     (int) ManualFeed, 0,
		     1,
		     viewsToPrint);
     }
   RestorePath (pDocPrint, dirName, docName);
}


/* ---------------------------------------------------------------------- */
/* |    CallbackPrintmenu analyse les retours du formulaire d'impression. | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                CallbackPrintmenu (int ref, int val, char *txt)
#else  /* __STDC__ */
void                CallbackPrintmenu (ref, val, txt)
int                 ref;
int                 val;
char               *txt;

#endif /* __STDC__ */
{
   if (pDocPrint != NULL)
      if (pDocPrint->DocSSchema != NULL)
	 /* le document a imprimer existe toujours */
	 switch (ref)
	       {
		  case NumMenuSupport:
		     /* sous-menu imprimer papier / sauver PostScript */
		     switch (val)
			   {
			      case 0:
				 if (!NewPaperPrint)
				   {
				      NewPaperPrint = TRUE;
				      TtaSetTextForm (NumZonePrinterName, pPrinter);
				   }
				 break;
			      case 1:
				 if (NewPaperPrint)
				   {
				      NewPaperPrint = FALSE;
				      TtaSetTextForm (NumZonePrinterName, psdir);
				   }
				 break;
			   }
		     break;
		  case NumMenuPaperFormat:
		     /* sous-menu format papier */
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
		     /* choix multiple Options */
		     ManualFeed = !ManualFeed;
		     break;
		  case NumZonePrinterName:
		     if (txt[0] != '\0')
			if (NewPaperPrint)
			   /* zone de saisie du nom de l'imprimante */
			   strncpy (pPrinter, txt, MAX_NAME_LENGTH);
			else
			   /* zone de saisie du nom du fichier PostScript */
			   strncpy (psdir, txt, MAX_PATH);
		     break;
		  case NumFormPrint:
		     /* formulaire Imprimer */
		     TtaDestroyDialogue (NumFormPrint);
		     switch (val)
			   {
			      case 1:
				 /* confirme l'option Imprimer papier */
				 /* les autres options ne sont prises en compte que sur confirmation */
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

/* ---------------------------------------------------------------------- */
/* |    TtcPrintSetup construit les catalogues qui seront utilises      | */
/* |            par l'editeur pour le formulaire d'impression.          | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtcPrintSetup (Document document, View view)
#else  /* __STDC__ */
void                TtcPrintSetup (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   int                 i;
   char                bufMenu[MAX_TXT_LEN];

   pDocPrint = LoadedDocument[document - 1];

   /* formulaire Imprimer */
   ConnectPrint ();
   TtaNewSheet (NumFormPrint, TtaGetViewFrame (document, view), 0, 0,
		TtaGetMessage (LIB, LIB_PRINT),
		1, TtaGetMessage (LIB, LIB_CONFIRM), FALSE, 2, 'L', D_DONE);
   i = 0;
   sprintf (&bufMenu[i], "%s%s", "B", TtaGetMessage (LIB, MANUAL_FEED));
   TtaNewToggleMenu (NumMenuOptions, NumFormPrint,
		 TtaGetMessage (LIB, OPTIONS), 1, bufMenu, NULL, FALSE);
   if (ManualFeed)
      TtaSetToggleMenu (NumMenuOptions, 0, TRUE);

   /* sous-menu format papier */
   i = 0;
   sprintf (&bufMenu[i], "%s%s", "B", TtaGetMessage (LIB, A4));
   i += strlen (&bufMenu[i]) + 1;
   sprintf (&bufMenu[i], "%s%s", "B", TtaGetMessage (LIB, US));
   TtaNewSubmenu (NumMenuPaperFormat, NumFormPrint, 0,
	    TtaGetMessage (LIB, PAPER_SIZE), 2, bufMenu, NULL, FALSE);
   if (!strcmp (PageSize, "US"))
      TtaSetMenuForm (NumMenuPaperFormat, 1);
   else
      TtaSetMenuForm (NumMenuPaperFormat, 0);

   /* sous-menu imprimer papier / sauver PostScript */
   i = 0;
   sprintf (&bufMenu[i], "%s%s", "B", TtaGetMessage (LIB, PRINTER));
   i += strlen (&bufMenu[i]) + 1;
   sprintf (&bufMenu[i], "%s%s", "B", TtaGetMessage (LIB, PS_FILE));
   TtaNewSubmenu (NumMenuSupport, NumFormPrint, 0,
		  TtaGetMessage (LIB, OUTPUT), 2, bufMenu, NULL, TRUE);
   /* zone de saisie du nom de l'imprimante */
   TtaNewTextForm (NumZonePrinterName, NumFormPrint, NULL, 30, 1, FALSE);

   /* initialisation du selecteur PaperPrint */
   NewPaperPrint = PaperPrint;
   if (PaperPrint)
     {
	TtaSetMenuForm (NumMenuSupport, 0);
	TtaSetTextForm (NumZonePrinterName, pPrinter);
     }
   else
     {
	TtaSetMenuForm (NumMenuSupport, 1);
	TtaSetTextForm (NumZonePrinterName, psdir);
     }

   /* active le formulaire "Imprimer" */
   TtaShowDialogue (NumFormPrint, FALSE);

}
