/*
 * Copyright (c) 1996 INRIA, All rights reserved
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

#include "viewcommands_f.h"
#include "exceptions_f.h"
#include "absboxlist_f.h"
#include "structmodif_f.h"
#include "presentmenu_f.h"
#include "printmenu_f.h"
#include "structselect_f.h"
#include "selectmenu_f.h"
#include "memory_f.h"
#include "registry_f.h"
#include "docs_f.h"
#include "print_tv.h"

static char         Orientation[MAX_NAME_LENGTH];

/*----------------------------------------------------------------------
  PrintInit
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         PrintInit (int userOrientation, char *tempDir, char *dir, char *name)
#else  /* __STDC__ */
static void         PrintInit (userOrientation, tempDir, dir, name)
int                 userOrientation;
char               *tempDir;
char               *dir;
char               *name;

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
}

/*----------------------------------------------------------------------
  Print
  Interface to the Print program.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         Print (char *name, char *dir, char *thotSch, char *thotDoc, char *thotpres, char *realName, char *realDir, char *printer, int firstPage, int lastPage, int nCopies, int hShift, int vShift, int userOrientation, int reduction, int nbPagesPerSheet, int suppFrame, int manualFeed, int blackAndWhite, int repaginate, char *viewsToPrint)

#else  /* __STDC__ */
static void         Print (name, dir, thotSch, thotDoc, thotpres, realName, realDir, printer, firstPage, lastPage, nCopies, hShift, vShift, userOrientation, reduction, nbPagesPerSheet, suppFrame, manualFeed, blackAndWhite, repaginate, viewsToPrint)
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
char               *viewsToPrint;

#endif /* __STDC__ */

{
   ThotPid             pid = ThotPid_get ();

#ifndef _WINDOWS
   char                cmd[800];
   int                 res;
   char               *thotDir;
   char               *tempDir;

   thotDir = (char *) TtaGetEnvString ("THOTDIR");
   if (!thotDir)
      thotDir = ThotDir ();
   tempDir = (char *) TtaGetMemory (40);
   sprintf (tempDir, "/tmp/Thot%d", pid + numOfJobs);

   PrintInit (userOrientation, tempDir, dir, name);
   if (printer[0] != '\0')
      sprintf (cmd, "%s/print %s %s %d %d %d 0 %s \"%s\" %s %d %d %d %s %d %d %d %d %d %ld PRINTER %s &\n",
	       BinariesDirectory, name, tempDir, repaginate, firstPage, lastPage, realName, printer, PageSize, nCopies, hShift,
	       vShift, Orientation, reduction, nbPagesPerSheet, suppFrame, manualFeed, blackAndWhite, FrRef[0], viewsToPrint);
   else
      sprintf (cmd, "%s/print %s %s %d %d %d 0 %s \"%s\" %s %d %d %d %s %d %d %d %d %d %ld PRINTER %s &\n",
	       BinariesDirectory, name, tempDir, repaginate, firstPage, lastPage, realName, "lp", PageSize, nCopies, hShift,
	       vShift, Orientation, reduction, nbPagesPerSheet, suppFrame, manualFeed, blackAndWhite, FrRef[0], viewsToPrint);

   res = system (cmd);
   if (res == -1)
      TtaDisplaySimpleMessage (CONFIRM, LIB, TMSG_ERROR_PS_TRANSLATION);
#endif /* _WINDOWS */
}


/*----------------------------------------------------------------------
   PostScriptSave
   saves the PostScript version of a document into a file.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         PostScriptSave (char *name, char *dir, char *thotSch, char *thotDoc, char *thotpres, char *realName, char *realDir, char *psName, int firstPage, int lastPage, int nCopies, int hShift, int vShift, int userOrientation, int reduction, int nbPagesPerSheet, int suppFrame, int manualFeed, int blackAndWhite, int repaginate, char *viewsToPrint)

#else  /* __STDC__ */
static void         PostScriptSave (name, dir, thotSch, thotDoc, thotpres, realName, realDir, psName, firstPage, lastPage, nCopies, hShift, vShift, userOrientation, reduction, nbPagesPerSheet, suppFrame, manualFeed, blackAndWhite, repaginate, viewsToPrint)
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
   sprintf (tempDir, "/tmp/Thot%d", pid + numOfJobs);
   PrintInit (userOrientation, tempDir, dir, name);

   if (psName[0] != '\0')
      sprintf (cmd, "%s/print %s %s %d %d %d 0 %s %s %s %d %d %d %s %d %d %d %d %d %ld PSFILE %s &\n",
	       BinariesDirectory, name, tempDir, repaginate, firstPage, lastPage, realName, psName, PageSize, nCopies, hShift,
	       vShift, Orientation, reduction, nbPagesPerSheet, suppFrame, manualFeed, blackAndWhite, FrRef[0], viewsToPrint);
   else
      sprintf (cmd, "%s/print %s %s %d %d %d 0 %s %s %s %d %d %d %s %d %d %d %d %d %ld PSFILE %s &\n",
	       BinariesDirectory, name, tempDir, repaginate, firstPage, lastPage, realName, "out.ps", PageSize, nCopies, hShift,
	       vShift, Orientation, reduction, nbPagesPerSheet, suppFrame, manualFeed, blackAndWhite, FrRef[0], viewsToPrint);

   res = system (cmd);
   if (res == -1)
      TtaDisplaySimpleMessage (CONFIRM, LIB, TMSG_ERROR_PS_TRANSLATION);
}


/*----------------------------------------------------------------------
   ConnectPrint
   initializes the printing parameters.
  ----------------------------------------------------------------------*/
void ConnectPrint ()
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
        /*if (!PSdir)
           sprintf (PSdir, "/tmp/out%d.ps", numOfJobs) ;*/
        sprintf (PSdir, "%s/%s.ps", pDocPrint->DocDirectory, pDocPrint->DocDName) ;
	/*PSdir[0] = '\0';*/
	PaperPrint = TRUE;
	ManualFeed = FALSE;
	FirstPage = 0;
	LastPage = 999;
	NbCopies = 1;
	Reduction = 100;
	PagesPerSheet = 1;
	strcpy (PageSize, "A4");
     }
}

/*----------------------------------------------------------------------
   TtcPrint
   standard action for the Print menu. Calls TtaPrint to print the
   current view.
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

   strcpy (viewsToPrint, TtaGetViewName (document, view));
   strcat (viewsToPrint, " ");
   TtaPrint (document, viewsToPrint) ;
}

/*----------------------------------------------------------------------
   TtaPrint
   interface to the multiview print command.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaPrint (Document document, char* viewNames)
#else  /* __STDC__ */
void                TtaPrint (document, viewNames)
Document            document;
char               *viewNames;

#endif /* __STDC__ */
{
   PathBuffer          dirName;
   Name                docName;
   /*PathBuffer          viewsToPrint;*/
   boolean             ok;
   Name                savePres, newPres;

   pDocPrint = LoadedDocument[document - 1];
   ConnectPrint ();
   /* prepares the execution of the print command */
   strcpy (savePres, pDocPrint->DocSSchema->SsDefaultPSchema);
   ConfigGetPSchemaForPageSize (pDocPrint->DocSSchema, PageSize, newPres);
   if (newPres[0] != '\0')
      strcpy (pDocPrint->DocSSchema->SsDefaultPSchema, newPres);

   /* the print program takes care of the repagination */
   strncpy (dirName, pDocPrint->DocDirectory, MAX_PATH);
   strncpy (docName, pDocPrint->DocDName, MAX_NAME_LENGTH);

   strcpy (pDocPrint->DocDirectory, "/tmp");
   strcpy (pDocPrint->DocDName, "ThotXXXXXX");
#ifdef WWW_MSWINDOWS
   _mktemp (pDocPrint->DocDName);
#else  /* WWW_MSWINDOWS */
   mktemp (pDocPrint->DocDName);
#endif /* !WWW_MSWINDOWS */

   ok = WriteDocument (pDocPrint, 2);

   /* restores the presentation scheme */
   strcpy (pDocPrint->DocSSchema->SsDefaultPSchema, savePres);

   /* make an automatic backup */
   if (ok)
     {
	if (PaperPrint)
	   Print (pDocPrint->DocDName,
		  pDocPrint->DocDirectory,
		  pDocPrint->DocSchemasPath,
		  DocumentPath,
		  pDocPrint->DocSSchema->SsDefaultPSchema,
		  docName, dirName, pPrinter,
		  FirstPage, LastPage, NbCopies, 
		  0, 0, 0,
		  Reduction, PagesPerSheet, TRUE,
		  (int) ManualFeed, 0,
		  1,
		  viewNames);
	else if (PSdir[0] != '\0')
	   PostScriptSave (pDocPrint->DocDName,
			   pDocPrint->DocDirectory,
			   pDocPrint->DocSchemasPath,
			   DocumentPath,
			   pDocPrint->DocSSchema->SsDefaultPSchema,
			   docName, dirName, PSdir,
			   FirstPage, LastPage, NbCopies,
			   0, 0, 0,
			   Reduction, PagesPerSheet, TRUE,
			   (int) ManualFeed, 0,
			   1,
			   viewNames);
     }
   strncpy (pDocPrint->DocDirectory, dirName, MAX_PATH);
   strncpy (pDocPrint->DocDName, docName, MAX_NAME_LENGTH);
   numOfJobs++;
}


/*----------------------------------------------------------------------
  CallbackPrintmenu
  callback associated to the PrintSetup form 
  ----------------------------------------------------------------------*/
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
				      TtaSetTextForm (NumZonePrinterName, PSdir);
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
			   strncpy (PSdir, txt, MAX_PATH);
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
				 if(ThotLocalActions[T_rextprint]!=NULL)
				   (*ThotLocalActions[T_rextprint])(ref, val, txt);
				 break;
			      default:
				 break;
			   }
		     break;
		  default:
		    if(ThotLocalActions[T_rextprint]!=NULL)
		      (*ThotLocalActions[T_rextprint])(ref, val, txt);
		     break;
	       }
}

/*----------------------------------------------------------------------
   TtcPrintSetup
   default action for the PrintSetup menu. Prepares and displays
   a form.
  ----------------------------------------------------------------------*/
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

   /* Print form */
   ConnectPrint ();
   TtaNewSheet (NumFormPrint, TtaGetViewFrame (document, view), 0, 0,
		TtaGetMessage (LIB, TMSG_LIB_PRINT),
	   1, TtaGetMessage (LIB, TMSG_LIB_CONFIRM), FALSE, 2, 'L', D_DONE);
   i = 0;
   sprintf (&bufMenu[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_MANUAL_FEED));
   TtaNewToggleMenu (NumMenuOptions, NumFormPrint,
		TtaGetMessage (LIB, TMSG_OPTIONS), 1, bufMenu, NULL, FALSE);
   if (ManualFeed)
      TtaSetToggleMenu (NumMenuOptions, 0, TRUE);

   /* Paper format submenu */
   i = 0;
   sprintf (&bufMenu[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_A4));
   i += strlen (&bufMenu[i]) + 1;
   sprintf (&bufMenu[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_US));
   TtaNewSubmenu (NumMenuPaperFormat, NumFormPrint, 0,
	     TtaGetMessage (LIB, TMSG_PAPER_SIZE), 2, bufMenu, NULL, FALSE);
   if (!strcmp (PageSize, "US"))
      TtaSetMenuForm (NumMenuPaperFormat, 1);
   else
      TtaSetMenuForm (NumMenuPaperFormat, 0);

   /* Print to paper/ Print to file submenu */
   i = 0;
   sprintf (&bufMenu[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_PRINTER));
   i += strlen (&bufMenu[i]) + 1;
   sprintf (&bufMenu[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_PS_FILE));
   TtaNewSubmenu (NumMenuSupport, NumFormPrint, 0,
		  TtaGetMessage (LIB, TMSG_OUTPUT), 2, bufMenu, NULL, TRUE);
   /* zone for capturing the name of the printer */
   TtaNewTextForm (NumZonePrinterName, NumFormPrint, NULL, 30, 1, FALSE);

   /* initialization of the PaperPrint selector */
   NewPaperPrint = PaperPrint;
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

   /* activates the Print form */
   TtaShowDialogue (NumFormPrint, FALSE);

}
