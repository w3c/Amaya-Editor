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
#include "thotmsg_f.h"
#include "viewcommands_f.h"
#include "exceptions_f.h"
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
static Func         pFuncExportPrintDoc = NULL;
static int          defPaperPrint = TRUE;
static int          defManualFeed = FALSE;
static int          defFirstPage = 0;
static int          defLastPage = 999;
static int          defNbCopies = 1;
static int          defReduction = 100;
static int          defPagesPerSheet = 1;
static int          defPageSize= PP_A4;

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
#ifndef _WINDOWS
   char                cmd[800];
   int                 res;

   if(userOrientation == 0)
      strcpy (Orientation, "Portrait");
   else
      strcpy (Orientation, "Landscape");

   if (printer[0] != '\0')
      sprintf (cmd, "%s/print %s %s %d %d %d %s \"%s\" %s %d %d %d %s %d %d %d %d %d %ld PRINTER %s &\n",
	       BinariesDirectory, name, dir, repaginate, firstPage, lastPage, realName, printer, PageSize, nCopies, hShift,
	       vShift, Orientation, reduction, nbPagesPerSheet, suppFrame, manualFeed, blackAndWhite, FrRef[0], viewsToPrint);
   else
      sprintf (cmd, "%s/print %s %s %d %d %d %s \"%s\" %s %d %d %d %s %d %d %d %d %d %ld PRINTER %s &\n",
	       BinariesDirectory, name, dir, repaginate, firstPage, lastPage, realName, "lp", PageSize, nCopies, hShift,
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
   
   
   if(userOrientation == 0)
      strcpy (Orientation, "Portrait");
   else
      strcpy (Orientation, "Landscape");

   if (psName[0] != '\0')
      sprintf (cmd, "%s/print %s %s %d %d %d %s %s %s %d %d %d %s %d %d %d %d %d %ld PSFILE %s &\n",
	       BinariesDirectory, name, dir, repaginate, firstPage, lastPage, realName, psName, PageSize, nCopies, hShift,
	       vShift, Orientation, reduction, nbPagesPerSheet, suppFrame, manualFeed, blackAndWhite, FrRef[0], viewsToPrint);
   else
      sprintf (cmd, "%s/print %s %s %d %d %d %s %s %s %d %d %d %s %d %d %d %d %d %ld PSFILE %s &\n",
	       BinariesDirectory, name, dir, repaginate, firstPage, lastPage, realName, "out.ps", PageSize, nCopies, hShift,
	       vShift, Orientation, reduction, nbPagesPerSheet, suppFrame, manualFeed, blackAndWhite, FrRef[0], viewsToPrint);

   res = system (cmd);
   if (res == -1)
      TtaDisplaySimpleMessage (CONFIRM, LIB, TMSG_ERROR_PS_TRANSLATION);
}


/*----------------------------------------------------------------------
   InitPrintParameters
   initializes the printing parameters.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void        InitPrintParameters (PtrDocument pDoc)
#else  /* __STDC__ */
void        InitPrintParameters (pDoc)
PtrDocument pDoc;
#endif /* __STDC__ */
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
       PSdir[0] = '\0';
       pDocPrint = NULL;
       defPaperPrint = TRUE;
       defManualFeed = FALSE;
       defFirstPage = 0;
       defLastPage = 999;
       defNbCopies = 1;
       defReduction = 100;
       defPagesPerSheet = 1;
       defPageSize= PP_A4;
     }

   if (pDoc != pDocPrint)
     {
       /* we are changing the current printed document */
       pDocPrint = pDoc;
       PaperPrint = defPaperPrint;
       ManualFeed = defManualFeed ;
       FirstPage = defFirstPage ;
       LastPage = defLastPage ;
       NbCopies = defNbCopies ;
       Reduction = defReduction ;
       PagesPerSheet = defPagesPerSheet ;
       if (defPageSize == PP_A4)
         strcpy(PageSize,"A4");
       else
         strcpy(PageSize,"US");
       if (pDocPrint->DocDirectory[0] == DIR_SEP)
	 sprintf (PSdir, "%s/%s.ps", pDocPrint->DocDirectory, pDocPrint->DocDName);
       else
	 sprintf (PSdir, "/tmp/%s.ps", pDocPrint->DocDName);
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
   PtrDocument         pDoc;
   PathBuffer          dirName,tmpDirName;
   Name                docName,tmpDocName;
   boolean	       docReadOnly;
   /*PathBuffer          viewsToPrint;*/
   boolean             ok;
   Name                savePres, newPres;
   int                 orientation;
   ThotPid             pid = ThotPid_get ();
   char                cmd[100];

   pDoc = LoadedDocument[document - 1];
   InitPrintParameters (pDoc);
   /* prepares the execution of the print command */
   strcpy (savePres, pDoc->DocSSchema->SsDefaultPSchema);
   ConfigGetPSchemaForPageSize (pDoc->DocSSchema, PageSize, newPres);
   if (newPres[0] != '\0')
      strcpy (pDoc->DocSSchema->SsDefaultPSchema, newPres);
   if (ThotLocalActions[T_rextprint]!=NULL && 
       strcmp(pDoc->DocSSchema->SsDefaultPSchema,savePres))
     {
       TtaDisplayMessage(INFO,TtaGetMessage(LIB,TMSG_CHANGE_PSCH),newPres);
     }
   sprintf (tmpDirName, "/tmp/Thot%d", pid + numOfJobs);
   sprintf (cmd, "/bin/mkdir %s\n", tmpDirName);
   system (cmd);
   sprintf (cmd, "chmod +rwx '%s'\n", tmpDirName);
   system (cmd);
   numOfJobs++;
   strcpy(tmpDocName,"ThotXXXXXX");
#ifdef WWW_MSWINDOWS
   _mktemp (tmpDocName);
#else  /* WWW_MSWINDOWS */
   mktemp (tmpDocName);
#endif /* !WWW_MSWINDOWS */
   if (pFuncExportPrintDoc !=NULL)
     /* a export procedure is defined */
       ok = (*pFuncExportPrintDoc)(document, tmpDocName, tmpDirName);
   else
     /* standard export */
     {
       strncpy (dirName, pDoc->DocDirectory, MAX_PATH);
       strncpy (docName, pDoc->DocDName, MAX_NAME_LENGTH);
       docReadOnly = pDoc->DocReadOnly;

       strcpy (pDoc->DocDirectory, tmpDirName);
       strcpy (pDoc->DocDName, tmpDocName);
       pDoc->DocReadOnly = FALSE;

       ok = WriteDocument (pDoc, 5);

       pDoc->DocReadOnly = docReadOnly;
       strncpy (pDoc->DocDirectory, dirName, MAX_PATH);
       strncpy (pDoc->DocDName, docName, MAX_NAME_LENGTH);
     }

   /* searches the paper orientation for the presentation scheme */
   ConfigGetPresentationOption(pDoc->DocSSchema,"orientation",Orientation);
   if (!strcmp (Orientation,"Landscape"))
     orientation = 1;
   else
     orientation = 0;

   /* restores the presentation scheme */
   strcpy (pDoc->DocSSchema->SsDefaultPSchema, savePres);

   /* make an automatic backup */
   if (ok)
     {
	if (PaperPrint)
	   Print (tmpDocName,
		  tmpDirName,
		  pDoc->DocSchemasPath,
		  DocumentPath,
		  pDoc->DocSSchema->SsDefaultPSchema,
		  docName, dirName, pPrinter,
		  FirstPage, LastPage, NbCopies, 
		  0, 0, orientation,
		  Reduction, PagesPerSheet, TRUE,
		  (int) ManualFeed, 0,
		  1,
		  viewNames);
	else if (PSdir[0] != '\0')
	   PostScriptSave (tmpDocName,
			   tmpDirName,
			   pDoc->DocSchemasPath,
			   DocumentPath,
			   pDoc->DocSSchema->SsDefaultPSchema,
			   docName, dirName, PSdir,
			   FirstPage, LastPage, NbCopies,
			   0, 0, orientation,
			   Reduction, PagesPerSheet, TRUE,
			   (int) ManualFeed, 0,
			   1,
			   viewNames);
     }
   /* restores the presentation scheme */
   strcpy (pDoc->DocSSchema->SsDefaultPSchema, savePres);
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
   InitPrintParameters ((PtrDocument)NULL);
   switch (parameter)
     {
     case PP_FirstPage:
       if (value <0 || value >999)
          TtaError(ERR_invalid_parameter);
       else
        { 
          defFirstPage = value;
          FirstPage = value;
        }
       break;
     case PP_LastPage:
       if (value <0 || value >999)
          TtaError(ERR_invalid_parameter);
       else
        {
          defLastPage = value;
          LastPage = value;
        }
       break;
     case PP_Scale:
       if (value <0 || value >999)
          TtaError(ERR_invalid_parameter);
       else
        {
          defReduction = value;
          Reduction = value;
        }
       break;
     case PP_NumberCopies:
       if (value <0 || value >999)
          TtaError(ERR_invalid_parameter);
       else
        {
          defNbCopies = value;
          NbCopies = value;
        }
       break;
     case PP_ManualFeed:
       if (value != PP_ON || value!= PP_OFF )
          TtaError(ERR_invalid_parameter);
       else
        {
          if(value == PP_ON)
            {
              defManualFeed = TRUE;
              ManualFeed = TRUE;
            }
          else
            {
              defManualFeed = FALSE;
              ManualFeed = FALSE;
            }
        }
       break;
     case PP_PagesPerSheet:
       if (value != 1 || value != 2 || value != 4)
          TtaError(ERR_invalid_parameter);
       else
        {
          defPagesPerSheet = value;
          PagesPerSheet = value;
        }
       break;
     case PP_PaperSize:
       if (value != PP_A4 || value!= PP_US )
          TtaError(ERR_invalid_parameter);
       else
        {
          defPageSize = value;
          if(value == PP_A4)
            {
              strcpy (PageSize, "A4");
            }
          else
            {
              strcpy (PageSize, "US");
            }
        }
       break;
     case PP_Destination:
       if (value != PP_PRINTER || value!= PP_PS)
          TtaError(ERR_invalid_parameter);
       else
        {
          if(value == PP_PRINTER)
            {
              defPaperPrint = TRUE;
              PaperPrint = TRUE;
            }
          else
            {
              defPaperPrint = FALSE;
              PaperPrint = FALSE;
            }
        }
       break;
      default:
       TtaError(ERR_invalid_parameter);
     }
  }
/*----------------------------------------------------------------------
  TtaSetPrintCommand
  sets the print command.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaSetPrintCommand(char *command)
#else  /* __STDC__ */
void                TtaSetPrintCommand(/*char *command*/)
char *command;
#endif /* __STDC__ */
{
  strcpy (pPrinter, command);
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
		     /* Manual feed option */
		     ManualFeed = !ManualFeed;
		     break;
		  case NumZonePrinterName:
		     if (txt[0] != '\0')
			if (NewPaperPrint)
			   /* text capture zone for the printer name */
			   strncpy (pPrinter, txt, MAX_NAME_LENGTH);
			else
			   /* text capture zone for the name of the PostScript file */
			   strncpy (PSdir, txt, MAX_PATH);
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
   PtrDocument         pDoc;
   int                 i;
   char                bufMenu[MAX_TXT_LEN];

   pDoc = LoadedDocument[document - 1];

   /* Print form */
   InitPrintParameters (pDoc);
   TtaNewSheet (NumFormPrint, TtaGetViewFrame (document, view), 0, 0,
		TtaGetMessage (LIB, TMSG_LIB_PRINT),
	   1, TtaGetMessage (LIB, TMSG_LIB_CONFIRM), FALSE, 2, 'L', D_CANCEL);
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
   /* text capture zone for the printer name */
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
