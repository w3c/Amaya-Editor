/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Warning:
 * This module is part of the Thot library, which was originally
 * developed in French. That's why some comments are still in
 * French, but their translation is in progress and the full module
 * will be available in English in the next release.
 * 
 */
 
/*
 * This module saves into PostScript format thot documents
 *
 * Authors: I. Vatton (INRIA)
 *          C. Roisin (INRIA) - Pagination at printing time
 *          R. Guetari (INRIA) - Unicode - Printing routines for Windows.
 *                               Integration of PostScript prologue. 
 *
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "picture.h"
#include "fileaccess.h"
#include "frame.h"
#include "printmsg.h"
#include "libmsg.h"
#include "message.h"
#include "appdialogue.h"
#include "dictionary.h"
#include "thotcolor.h"

#ifdef _WINDOWS
#define DLLEXPORT __declspec (dllexport)
#include "thotprinter_f.h"
#endif /* _WINDOWS */

#define MAX_VOLUME        10000	/* volume maximum d'une page, en octets */
#define DEF_TOP_MARGIN    57	/* marge de haut de page par defaut, en points */
#define DEF_LEFT_MARGIN   57	/* marge de gauche par defaut, en points */
#define DEF_PAGE_HEIGHT   800	/* hauteur de page par defaut, en points */
#define DEF_PAGE_WIDTH    482	/* largeur de page par defaut, en points */

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "boxes_tv.h"
#include "font_tv.h"
#include "platform_tv.h"
#include "thotcolor.h"
#include "thotcolor_tv.h"
#undef THOT_EXPORT
#define THOT_EXPORT
#include "select_tv.h"
#include "page_tv.h"
#include "modif_tv.h"
#include "edit_tv.h"
#include "creation_tv.h"
#include "frame_tv.h"
#include "platform_tv.h"
#include "appdialogue_tv.h"
#include "units_tv.h"

extern RGBstruct RGB_colors[];

int          PRINT;	/* Identification des messages */
ptrfont      PoscriptFont;
int          ColorPs;
int          LastPageNumber, LastPageWidth, LastPageHeight;

static PtrDocument  TheDoc;	/* le document en cours de traitement */
static PathBuffer   DocumentDir;   /* le directory d'origine du document */
static int          NumberOfPages;
static CHAR_T       tempDir [MAX_PATH];
static ThotBool     removeDirectory;

/* table des vues a imprimer */
#define MAX_PRINTED_VIEWS MAX_VIEW_DOC+MAX_ASSOC_DOC
#define MAX_CSS           10
static Name         PrintViewName[MAX_PRINTED_VIEWS];
static Name         CSSName[MAX_CSS];
static int          TopMargin;
static int          LeftMargin;
static ThotBool     CleanTopOfPageElement; /* premiere page imprimee pour le
					     pagination-impression */
static DocViewNumber CurrentView;	/* numero de la vue traitee */
static int           CurAssocNum;	/* No d'element associe de la vue traitee */
static int           CurrentFrame;	/* No frame contenant la vue traitee */
static STRING        printer;
static ThotWindow    thotWindow;

#include "absboxes_f.h"
#include "actions_f.h"
#include "applicationapi_f.h"
#include "attrpresent_f.h"
#include "boxmoves_f.h"
#include "boxpositions_f.h"
#include "buildboxes_f.h"
#include "changeabsbox_f.h"
#include "config_f.h"
#include "context_f.h"
#include "createabsbox_f.h"
#include "dictionary_f.h"
#include "displaybox_f.h"
#include "exceptions_f.h"
#include "fileaccess_f.h"
#include "frame_f.h"
#include "font_f.h"
#include "frame_f.h"
#include "inites_f.h"
#include "language_f.h"
#include "memory_f.h"
#include "nodialog_f.h"
#include "paginate_f.h"
#include "pagecommands_f.h"
#include "picture_f.h"
#include "print_f.h"
#include "psdisplay_f.h"
#include "readpivot_f.h"
#include "registry_f.h"
#include "structlist_f.h"
#include "structschema_f.h"
#include "table2_f.h"
#include "tree_f.h"

#ifdef _WINDOWS 
#include "wininclude.h"
#endif /* _WINDOWS */

static int          manualFeed;
static CHAR_T       pageSize [3];
static int          BlackAndWhite;
static int          HorizShift;
static int          VertShift;
static int          Zoom;
static STRING       Orientation;
static int          NPagesPerSheet;
static int          NoEmpyBox;
static int          Repaginate;
static int          FirstPrinted;
static int          LastPrinted;

#ifdef _WINDOWS
#define PRINTPROGRESSDLG 389
ThotWindow       WIN_curWin = (ThotWindow)(-1);
ThotWindow       WIN_Main_Wd = (ThotWindow)(-1);
HBITMAP          WIN_LastBitmap = 0;

BOOL             bError;
BOOL             gbAbort;
HWND             hDlgPrint     = NULL;
HWND             ghwndMain     = NULL;
HWND             currentWindow;
HWND             ghwndAbort;
static HINSTANCE hCurrentInstance ;

/*----------------------------------------------------------------------
   WinErrorBox :  Pops-up a message box when an MS-Window error      
   occured.                                                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void WinErrorBox (HWND hWnd, STRING source)
#else  /* !__STDC__ */
void WinErrorBox (hWnd, source)
HWND hWnd;
STRING source;
#endif /* __STDC__ */
{
   CHAR_T                str[200];

   sprintf (str, "Error - Source: %s", source);
   MessageBox (hWnd, str, TEXT("Amaya"), MB_OK);

}

/* ----------------------------------------------------------------------
   ----------------------------------------------------------------------*/
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

/* ---------------------------------------------------------------------- *
 *                                                                        *
 *  FUNCTION:    AbortDlgProc (standard dialog procedure INPUTS/RETURNS)  *
 *                                                                        *
 *  COMMENTS:    Handles "Abort" dialog messages                          *
 *                                                                        *
 * ---------------------------------------------------------------------- */
#ifdef __STDC__
LRESULT CALLBACK AbortDlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
#else  /* __STDC__ */
LRESULT CALLBACK AbortDlgProc (hwnd, msg, wParam, lParam)
HWND   hwnd; 
UINT   msg; 
WPARAM wParam; 
LPARAM lParam;
#endif /* __STDC __ */
{
   switch (msg) {
          case WM_INITDIALOG: ghwndAbort = hwnd;
                              EnableMenuItem (GetSystemMenu (hwnd, FALSE), SC_CLOSE, MF_GRAYED);
                              break;

          case WM_COMMAND:
               switch (LOWORD (wParam)) {
                       case IDCANCEL: gbAbort = TRUE;
                                      AbortDoc (TtPrinterDC);
                                      EnableWindow  (ghwndMain, TRUE);
                                      DestroyWindow (hwnd);
                                      return TRUE;
			   }
               break;
   }
   return 0;
}

/* ---------------------------------------------------------------------- *
 *                                                                        *
 *  FUNCTION:    AbortProc                                                *
 *                                                                        *
 *  COMMENTS:    Standard printing abort proc                             *
 *                                                                        *
 * ---------------------------------------------------------------------- */
#ifdef __STDC__
BOOL CALLBACK AbortProc (HDC hdc, int error)
#else  /* __STDC__ */
BOOL CALLBACK AbortProc (hdc, error)
HDC hdc; 
int error;
#endif /* __STDC__ */
{
   MSG msg;

   while (!gbAbort && PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
         if (!ghwndAbort || !IsDialogMessage (ghwndAbort, &msg)) {
            TranslateMessage (&msg);
            DispatchMessage (&msg);
		 }

   return !gbAbort;
}

/* ---------------------------------------------------------------------- *
 *                                                                        *
 *  FUNCTION: InitPrinting(HDC hDC, HWND hWnd, HANDLE hInst, LPSTR msg)   *
 *                                                                        *
 *  PURPOSE : Makes preliminary driver calls to set up print job.         *
 *                                                                        *
 *  RETURNS : TRUE  - if successful.                                      *
 *            FALSE - otherwise.                                          *
 *                                                                        *
 * ---------------------------------------------------------------------- */
#ifdef __STDC__
BOOL PASCAL InitPrinting(HDC hDC, HWND hWnd, HANDLE hInst, LPSTR msg)
#else /* !__STDC__ */
BOOL PASCAL InitPrinting(hDC, hWnd, hInst, msg)
HDC    hDC; 
HWND   hWnd; 
HANDLE hInst; 
LPSTR  msg;
#endif /* __STDC__ */
{
    DOCINFO         DocInfo;
    STRING suffix;

    bError     = FALSE;     /* no errors yet */
    gbAbort    = FALSE;     /* user hasn't aborted */

#  ifdef _WINDOWS
   suffix = TtaGetVarLANG ();

   if (!ustrncasecmp (suffix, "fr", 2))
      app_lang = FR_LANG;
   else if (!ustrncasecmp (suffix, "en", 2))
      app_lang = EN_LANG;
   else if (!ustrncasecmp (suffix, "de", 2))
      app_lang = DE_LANG;
#  endif /* _WINDOWS */

	switch (app_lang) {
           case FR_LANG:
               if (!(ghwndAbort = CreateDialog (hInst, (LPCTSTR) "FR_Printinprogress", ghwndMain, (DLGPROC) AbortDlgProc)))
                  WinErrorBox (ghwndMain, "InitPrinting: FR_LANG");
               break;

		   case EN_LANG:
                if (!(ghwndAbort = CreateDialog (hInst, (LPCTSTR) "EN_Printinprogress", ghwndMain, (DLGPROC) AbortDlgProc)))
                   WinErrorBox (ghwndMain, "InitPrinting: EN_LANG");
                break;

		   case DE_LANG:
                if (!(ghwndAbort = CreateDialog (hInst, (LPCTSTR) "DE_Printinprogress", ghwndMain, (DLGPROC) AbortDlgProc)))
                   WinErrorBox (ghwndMain, "InitPrinting: DE_LANG");
				break;
	} 

    EnableWindow (ghwndMain, FALSE);
    SetAbortProc (TtPrinterDC, AbortProc);

    memset(&DocInfo, 0, sizeof(DOCINFO));
    DocInfo.cbSize      = sizeof(DOCINFO);
    DocInfo.lpszDocName = (LPTSTR) msg;
    DocInfo.lpszOutput  = NULL;

    if (StartDoc (hDC, &DocInfo) <= 0) {
        bError = TRUE;
        return FALSE;
    }

    /* might want to call the abort proc here to allow the user to
     * abort just before printing begins */
    return TRUE;
}

/*----------------------------------------------------------------------
   WIN_GetDeviceContext :  select a Device Context for a given       
   thot window.                                                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void WIN_GetDeviceContext (int frame)
#else  /* !__STDC__ */
void WIN_GetDeviceContext (frame)
int frame;
#endif /* __STDC__ */
{
  WIN_curWin = NULL;
  TtDisplay = GetDC (WIN_curWin);
}

/*----------------------------------------------------------------------
   WIN_ReleaseDeviceContext :  unselect the Device Context           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void WIN_ReleaseDeviceContext (void)
#else  /* !__STDC__ */
void WIN_ReleaseDeviceContext ()
#endif /* __STDC__ */
{
  /* release the previous Device Context. */
  /* if ((TtDisplay != 0) && (WIN_curWin != (ThotWindow) (-1))) */
  if (TtDisplay != 0)
    if (!ReleaseDC (WIN_curWin, TtDisplay))
      WinErrorBox (NULL, "PRINT: WIN_ReleaseDeviceContext");
  
  WIN_curWin = (ThotWindow) (-1);
  TtDisplay = 0;
}
#endif /* _WINDOWS */

/*----------------------------------------------------------------------
   PrintPageFooter
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         PrintPageFooter (FILE * fout, int frame, PtrAbstractBox pPage)
#else  /* __STDC__ */
static void         PrintPageFooter (fout, frame, pPage)
FILE               *fout;
int                 frame;
PtrAbstractBox      pPage;
#endif /* __STDC__ */
{
  PtrAbstractBox    pAb;
  PtrBox            pBox;
  ViewFrame        *pFrame;
  int               h;


  /* Look for the break line position */
  pAb = pPage->AbFirstEnclosed;
  while (pAb != NULL && pAb->AbPresentationBox)
    pAb = pAb->AbNext;
  h = pAb->AbBox->BxYOrg;

  /* define the clipping to display the page header */
  pFrame = &ViewFrameTable[frame - 1];
  pFrame->FrClipXBegin = 0;
  pFrame->FrClipXEnd = PixelValue (32000, UnPoint, NULL, 0);
  pFrame->FrClipYBegin = 0;
  pFrame->FrClipYEnd = PixelValue (32000, UnPoint, NULL, 0);
  pFrame->FrYOrg = pPage->AbBox->BxYOrg;
  /* Set the top margin to the page footer position on the paper */
  FrameTable[frame].FrTopMargin = PixelValue (TopMargin, UnPoint, NULL, 0) + PageHeight + PageFooterHeight - h  + pPage->AbBox->BxYOrg;

  /*y = 0;
  h = 32000; */
  /* set the clipping to the frame size before generating postscript (RedrawFrameBottom) */
  /* DefineClipping (frame, pFrame->FrXOrg, pFrame->FrYOrg, &framexmin, &y, &framexmax, &h, 1); */

  /* Look for the first terminal box of the current page */
  pAb = pPage->AbFirstEnclosed;
  while (pAb != NULL && pAb->AbFirstEnclosed != NULL)
      pAb = pAb->AbFirstEnclosed;
  pBox = pAb->AbBox;

  pBox = pAb->AbBox;
  /* print all boxes of the current page */
  while (pBox != NULL && IsParentBox (pPage->AbBox, pBox))
    {
      if (pBox->BxYOrg < h)
	DisplayBox (pBox, frame);
      pBox = pBox->BxNext;
    }
}

/*----------------------------------------------------------------------
   PrintPageHeader displays the content of the page header and cleans
   the page element contents except the break line.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         PrintPageHeader (FILE * fout, int frame, PtrAbstractBox pPage, int org)
#else  /* __STDC__ */
static void         PrintPageHeader (fout, frame, pPage, org)
FILE               *fout;
int                 frame;
PtrAbstractBox      pPage;
int                 org;
#endif /* __STDC__ */
{
  PtrAbstractBox    pAb;
  PtrBox            pBox;
  ViewFrame        *pFrame;
  int              framexmin, framexmax;
  int               y, h;


  /* Set the top margin to the page header position on the paper */
  FrameTable[frame].FrTopMargin = PixelValue (TopMargin, UnPoint, NULL, 0);
  /* define the clipping to display the page header */
  pFrame = &ViewFrameTable[frame - 1];
  pFrame->FrClipXBegin = 0;
  pFrame->FrClipXEnd = PixelValue (32000, UnPoint, NULL, 0);
  pFrame->FrClipYBegin = 0;
  /* pFrame->FrYOrg = PixelValue (org, UnPoint, pFrame->FrAbstractBox, 0); */
  pFrame->FrYOrg = org;
  pFrame->FrClipYEnd = PixelValue (32000, UnPoint, NULL, 0);
  y = 0;
  h = 32000;
  /* set the clipping to the frame size before generating postscript (RedrawFrameBottom) */
  DefineClipping (frame, pFrame->FrXOrg, pFrame->FrYOrg, &framexmin, &y, &framexmax, &h, 1);

  /* Look for the first terminal box of the current page */
  pAb = pPage->AbFirstEnclosed;
  while (pAb != NULL && pAb->AbFirstEnclosed != NULL)
      pAb = pAb->AbFirstEnclosed;
  pBox = pAb->AbBox;

  /* print all boxes of the current page*/
  while (pBox != NULL && IsParentBox (pPage->AbBox, pBox))
    {
      DisplayBox (pBox, frame);
      pBox = pBox->BxNext;
    }

  /* Set the top margin to the page body position on the paper */
  FrameTable[frame].FrTopMargin = PixelValue (TopMargin, UnPoint, NULL, 0) + pPage->AbBox->BxHeight;

  /* Remove all boxes within the page element except the line */
  pAb = pPage->AbFirstEnclosed;
  while (pAb != NULL)
    {
      if (pAb->AbPresentationBox)
	{
	  /* Note: le filet n'est pas un pave de presentation, alors que */
	  /* toutes les autres boites de bas de page sont des paves de */
	  /* presentation */
	  SetDeadAbsBox (pAb);
	}
      pAb = pAb->AbNext;
    }
  /* signale les paves morts au Mediateur */
  h = 0;
  ChangeConcreteImage (frame, &h, pPage);
}

/*----------------------------------------------------------------------
   DrawPage check whether a showpage is needed.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         DrawPage (FILE * fout)
#else  /* __STDC__ */
static void         DrawPage (fout)
FILE               *fout;
#endif /* __STDC__ */
{
  NumberOfPages++;
# ifdef _WINDOWS 
  if (TtPrinterDC) {
    EndPage (TtPrinterDC);
  } else {
    fprintf (fout, "%d %d %d nwpage\n%%%%Page: %d %d\n", LastPageNumber, LastPageWidth, LastPageHeight, NumberOfPages, NumberOfPages);
    fflush (fout);
    /* Enforce loading the font when starting a new page */
    PoscriptFont = NULL;
    ColorPs = -1;
  }
# else  /* _WINDOWS */
  fprintf (fout, "%d %d %d nwpage\n%%%%Page: %d %d\n", LastPageNumber, LastPageWidth, LastPageHeight, NumberOfPages, NumberOfPages);
  fflush (fout);
  /* Enforce loading the font when starting a new page */
  PoscriptFont = NULL;
  ColorPs = -1;
# endif /* _WINDOWS */
}

#ifndef _WINDOWS
/*----------------------------------------------------------------------
 * XWindowError is the X-Windows non-fatal errors handler.
 ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          XWindowError (Display * dpy, XErrorEvent * err)
#else  /* __STDC__ */
static int          XWindowError (dpy, err)
Display            *dpy;
XErrorEvent        *err;

#endif /* __STDC__ */
{
   CHAR_T                msg[200];

   XGetErrorText (dpy, err->error_code, msg, 200);
   return (0);
}

/*----------------------------------------------------------------------
 * XWindowFatalError is the X-Windows fatal errors handler.
 ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          XWindowFatalError (Display * dpy)
#else  /* __STDC__ */
static int          XWindowFatalError (dpy)
Display            *dpy;
#endif /* __STDC__ */
{
   extern int          errno;

   if (errno != EPIPE)
      TtaDisplayMessage (FATAL, TtaGetMessage (LIB, TMSG_LIB_X11_ERR), DisplayString (dpy));
   else
      TtaDisplayMessage (FATAL, TtaGetMessage (LIB, TMSG_LIB_X11_ERR), DisplayString (dpy));
   return (0);
}
#endif /* _WINDOWS */


#ifdef __STDC__
static void usage (STRING processName) 
#else  /* __STDC__ */
static void usage (processName)
STRING processName;
#endif /* __STDC__ */ 
{
       fprintf (stderr, "\n\nusage: %s [-lang value] <file name>\n", processName);
       fprintf (stderr, "       -v <view name> [-v <view name> [...]]\n");
       fprintf (stderr, "       -ps <psfile> | -out <printer>\n");
       fprintf (stderr, "       [-portrait | -landscape]\n");
       fprintf (stderr, "       [-display <display>]\n");
       fprintf (stderr, "       [-css <file name> [...]]\n");
       fprintf (stderr, "       [-name <document name>]\n");
       fprintf (stderr, "       [-npps <number of pages per sheet>]\n");
       fprintf (stderr, "       [-bw]\t\t /* for black & white output */\n");
       fprintf (stderr, "       [-paginate]\t /* to repaginate */\n");
       fprintf (stderr, "       [-manualfeed]\t /* for manualfeed */\n");
       fprintf (stderr, "       [-Fn1]\t\t /* n1: number of first page to print */\n");
       fprintf (stderr, "       [-Ln2]\t\t /* n2: number of last page to print */\n");
       fprintf (stderr, "       [-#n]\t\t /* n: number of copies to print */\n");
       fprintf (stderr, "       [-Hn]\t\t /* n: left margin */\n");
       fprintf (stderr, "       [-Vn]\t\t /* n: top margin */\n");
       fprintf (stderr, "       [-%%n]\t\t /* n: zoom in percent */\n");
       fprintf (stderr, "       [-emptybox]\t /* to print empty boxes */\n");
       fprintf (stderr, "       [-wn]\t\t /* n: window number */\n");
       fprintf (stderr, "       [-removedir]\t /* remove directory after printing */\n\n\n");
       exit (1);
}

/*----------------------------------------------------------------------
   NextReferenceToEl                                               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PtrReference        NextReferenceToEl (PtrElement pEl, PtrDocument pDoc, ThotBool processNotLoaded, PtrReference pPrevRef, PtrDocument * pDocRef, PtrExternalDoc * pExtDoc, ThotBool nextExtDoc)

#else  /* __STDC__ */
PtrReference        NextReferenceToEl (pEl, pDoc, processNotLoaded, pPrevRef, pDocRef, pExtDoc, nextExtDoc)
PtrElement          pEl;
PtrDocument         pDoc;
ThotBool            processNotLoaded;
PtrReference        pPrevRef;
PtrDocument        *pDocRef;
PtrExternalDoc     *pExtDoc;
ThotBool            nextExtDoc;

#endif /* __STDC__ */

{
   PtrReference        pRef;

   if (pPrevRef != NULL)
      pRef = pPrevRef->RdNext;
   else
      /* premier appel de la fonction */
     {
	*pExtDoc = NULL;
	pRef = NULL;
	if (pEl != NULL)
	   if (pEl->ElReferredDescr != NULL)
	     {
		pRef = pEl->ElReferredDescr->ReFirstReference;
		*pDocRef = pDoc;
	     }
     }
   /* on ne cherche pas dans les documents externes */
   return pRef;
}

/*----------------------------------------------------------------------
   GetCurrentSelection                                             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            GetCurrentSelection (PtrDocument * pDoc, PtrElement * firstEl, PtrElement * lastEl, int *firstChar, int *lastChar)
#else  /* __STDC__ */
ThotBool            GetCurrentSelection (pDoc, firstEl, lastEl, firstChar, lastChar)
PtrDocument        *pDoc;
PtrElement         *firstEl;
PtrElement         *lastEl;
int                *firstChar;
int                *lastChar;
#endif /* __STDC__ */
{
   *pDoc = NULL;
   return FALSE;
}


/*----------------------------------------------------------------------
   Si l'entree existe :                                             
   Ferme la fenetre, detruit le fichier et libere l'entree.      
   Libere toutes les boites allouees a la fenetre.                   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DestroyFrame (int frame)
#else  /* __STDC__ */
void                DestroyFrame (frame)
int                 frame;

#endif /* __STDC__ */
{
  ClearConcreteImage (frame);
  ThotFreeFont (frame);	/* On libere les polices de caracteres utilisees */
}

/*----------------------------------------------------------------------
   FirstFrame cree et initialise la premiere frame.          	
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         FirstFrame (STRING server)
#else  /* __STDC__ */
static void         FirstFrame (server)
STRING server;
#endif /* __STDC__ */
{
   int                 i;

   /* Initialisation de la table des frames */
   for (i = 0; i <= MAX_FRAME; i++)
	FrRef[i] = 0;
   /* Ouverture du serveur X-ThotWindow */
      /*Connexion au serveur X impossible */
#  ifndef _WINDOWS
   TtDisplay = XOpenDisplay (server);
   if (!TtDisplay)
      TtaDisplaySimpleMessage (FATAL, LIB, TMSG_UNABLE_TO_CONNECT_TO_X);
   XSetErrorHandler (XWindowError);
   XSetIOErrorHandler (XWindowFatalError);
   TtScreen = DefaultScreen (TtDisplay);
   TtRootWindow = RootWindow (TtDisplay, TtScreen);
   TtWDepth = DefaultDepth (TtDisplay, TtScreen);
   TtCmap = XDefaultColormap (TtDisplay, TtScreen);
   Black_Color = BlackPixel (TtDisplay, TtScreen);
   White_Color = WhitePixel (TtDisplay, TtScreen);
#  endif /* ! _WINDOWS */
   DefaultBColor = 0;
   DefaultFColor = 1;
   InitDocColors (_THOTElement_);
   /* Initialisation des polices de caracteres */
#  ifdef _WINDOWS 
   WIN_InitDialogueFonts (TtPrinterDC, _THOTElement_);
#  else  /* _WINDOWS */
   InitDialogueFonts ("thot");
#  endif /* _WINDOWS */
}

/*----------------------------------------------------------------------------*/
/* DefineClipping  limite la zone de reaffichage sur la fenetre frame et      */
/*                 recalcule ses limites sur l'image concrete.                */
/*                 Dans le cas du print, c'est exactement la hauteur de page. */
/*  --------------------------------------------------------------------------*/
#ifdef __STDC__
void                DefineClipping (int frame, int orgx, int orgy, int *xd, int *yd, int *xf, int *yf, int raz)

#else  /* __STDC__ */
void                DefineClipping (frame, orgx, orgy, xd, yd, xf, yf, raz)
int                 frame;
int                 orgx;
int                 orgy;
int                *xd;
int                *yd;
int                *xf;
int                *yf;
int                 raz;
#endif /* __STDC__ */
{
   FrameTable[frame].FrHeight = *yf;
}

/*----------------------------------------------------------------------
   RemoveClipping annule le rectangle de clipping de la fenetre frame.  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                RemoveClipping (int frame)
#else  /* __STDC__ */
void                RemoveClipping (frame)
int                 frame;

#endif /* __STDC__ */

{   
   FrameTable[frame].FrWidth = 32000;
   FrameTable[frame].FrHeight = PixelValue (1000, UnPixel, NULL, 0);

}


/*----------------------------------------------------------------------
   GetSizesFrame retourne les dimensions de la fenetre d'indice frame.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GetSizesFrame (int frame, int *width, int *height)
#else  /* __STDC__ */
void                GetSizesFrame (frame, width, height)
int                 frame;
int                *width;
int                *height;

#endif /* __STDC__ */
{
   *width = FrameTable[frame].FrWidth;
   *height = FrameTable[frame].FrHeight;
}


/*----------------------------------------------------------------------
   TtaGetThotWindow recupere le numero de la fenetre.              
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotWindow          TtaGetThotWindow (int frame)

#else  /* __STDC__ */
ThotWindow          TtaGetThotWindow (frame)
int                 frame;
#endif /* __STDC__ */
{
   return FrRef[frame];
}

/*----------------------------------------------------------------------
   GetDocAndView    Retourne le pointeur sur le document (pDoc) et le    
   numero de vue (view) dans ce document, correspondant a    
   frame de numero frame. Si c'est une frame d'elements   
   associes, rend assoc=vrai et view=numero d'el. associe    
   sinon rend assoc faux.                                  
   Procedure differente de GetDocAndView de docvues.c          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GetDocAndView (int frame, PtrDocument * pDoc, int *view, ThotBool * assoc)
#else  /* __STDC__ */
void                GetDocAndView (frame, pDoc, view, assoc)
int                 frame;
PtrDocument        *pDoc;
int                *view;
ThotBool           *assoc;
#endif /* __STDC__ */
{
   *pDoc = TheDoc;
   if (CurAssocNum > 0)
     {
	*view = CurAssocNum;
	*assoc = TRUE;
     }
   else
     {
	*view = CurrentView;
	*assoc = FALSE;
     }
}


/*----------------------------------------------------------------------
   OpenPSFile opens the printing file and write the PS prologue.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          OpenPSFile (PtrDocument pDoc, int *volume)
#else  /* __STDC__ */
static int          OpenPSFile (pDoc, volume)
PtrDocument         pDoc;
int                *volume;
#endif /* __STDC__ */
{
  FILE               *PSfile;
  CHAR_T              tmp[MAX_PATH];
  CHAR_T              fileName[256];
  ViewFrame          *pFrame;
  long                i;
  int                 len;

  /* Est-ce la premiere creation de frame ? */
  i = 1;
  while (FrRef[i] != 0)
    i++;

#ifdef _WINDOWS
  if (TtPrinterDC) {
    FrRef[i] = (ThotWindow)i;
    /* initialiser visibilite et zoom de la fenetre */
    /* cf. procedure InitializeFrameParams */
    pFrame = &ViewFrameTable[i - 1];
    pFrame->FrVisibility = 5;	/* visibilite mise a 5 */
    pFrame->FrMagnification = 0;	/* zoom a 0 */
    
    /* On initialise la table des frames  (attention MYSTERES)*/
    FrameTable[i].FrDoc = IdentDocument (pDoc);
    RemoveClipping(i);
    *volume = 16000;
    return (i);
  }
#endif /* _WINDOWS */

  if (i == 1)
    {
      /* On construit le nom du fichier PostScript */
      ustrcpy (tmp, DocumentPath);
      /* On cherche le directory ou existe le .PIV */
      MakeCompleteName (pDoc->DocDName, PIV_EXT2, tmp, fileName, &len);
      /* On construit le nom complet avec ce directory */
      FindCompleteName (pDoc->DocDName, Ps_EXT2, tmp, fileName, &len);	/* ps au lieu de PIV */
      if ((PSfile = ufopen (fileName, _WriteMODE_)) == NULL)
	TtaDisplayMessage (FATAL, TtaGetMessage (LIB, TMSG_CANNOT_CREATE_PS), fileName);
      else
	{
	  fflush (PSfile);
	  FrRef[i] = (ThotWindow) PSfile;
	  fprintf (PSfile, "%%!PS-Adobe-2.0\n");
	  fprintf (PSfile, "%%%%Creator: Thot\n");
	  fprintf (PSfile, "%%%% Delete the last nwpage line command for an encapsulated PostScript\n");
	  fprintf (PSfile, "%%%%CreationDate: Sat Nov  2 12:03:40 MET 1996\n");
	  
	  fprintf (PSfile, "%%%%PaginateView: (atend)\n");
	  fprintf (PSfile, "%%%%EndComments\n\n");
	  
	  fprintf (PSfile, "/ThotDict 100 dict def\n");
	  fprintf (PSfile, "ThotDict begin\n\n");
	  
	  fprintf (PSfile, "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%Fonctions generales%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
	  fprintf (PSfile, "/setpatterndict 18 dict def\n");
	  fprintf (PSfile, "setpatterndict begin\n");
	  fprintf (PSfile, "/bitison\n");
	  fprintf (PSfile, "{ /ybit exch def\n");
	  fprintf (PSfile, "  /xbit exch def\n");
	  fprintf (PSfile, "  /bytevalue bstring ybit bwidth mul xbit 8 idiv add get def\n");
	  fprintf (PSfile, "  /PicMask 1 7 xbit 8 mod sub bitshift def\n");
	  fprintf (PSfile, "  bytevalue PicMask and 0 ne\n");
	  fprintf (PSfile, "}def\n");
	  fprintf (PSfile, "end\n");
	  fprintf (PSfile, "/bpspotf\n");
	  fprintf (PSfile, "{setpatterndict begin\n");
	  fprintf (PSfile, "  /y exch def\n");
	  fprintf (PSfile, "  /x exch def\n");
	  fprintf (PSfile, "  /xind x 1 add 2 div bpside mul cvi def\n");
	  fprintf (PSfile, "  /yind y 1 add 2 div bpside mul cvi def\n");
	  fprintf (PSfile, "  xind yind bitison\n");
	  fprintf (PSfile, "   {/onbits onbits 1 add def 1}\n");
	  fprintf (PSfile, "   {/offbits offbits 1 add def 0}\n");
	  fprintf (PSfile, "  ifelse\n");
	  fprintf (PSfile, " end\n");
	  fprintf (PSfile, "}def\n");
	  fprintf (PSfile, "/setpattern\n");
	  fprintf (PSfile, "{setpatterndict begin\n");
	  fprintf (PSfile, "  /cellsz exch def\n");
	  fprintf (PSfile, "  /bwidth exch def\n");
	  fprintf (PSfile, "  /bpside exch def\n");
	  fprintf (PSfile, "  /bstring exch def\n");
	  fprintf (PSfile, "  /onbits 0 def\n");
	  fprintf (PSfile, "  /offbits 0 def\n");
	  fprintf (PSfile, "  cellsz 0 /bpspotf load setscreen\n");
	  fprintf (PSfile, "  {} settransfer\n");
	  fprintf (PSfile, "  offbits offbits onbits add div setgray\n");
	  fprintf (PSfile, " end\n");
	  fprintf (PSfile, "}def\n");
	  fprintf (PSfile, "/fillbox	%% (red green blue / pattern) code fillbox\n");
	  fprintf (PSfile, "{ /code exch def\n");
	  fprintf (PSfile, "  code 0 ge\n");
	  fprintf (PSfile, "  {code 0 gt\n");
	  fprintf (PSfile, "   {code 8 eq \n");
	  fprintf (PSfile, "    {gsave 8 1 72 300 32 div div setpattern fill grestore} %%new pattern\n");
	  fprintf (PSfile, "    {gsave 1 code 10 div sub setgray fill grestore} %%gray level\n");
	  fprintf (PSfile, "    ifelse\n");
	  fprintf (PSfile, "   }if\n");
	  fprintf (PSfile, "  }\n");
	  fprintf (PSfile, "  {gsave setrgbcolor fill grestore} %%lower\n");
	  fprintf (PSfile, "  ifelse\n");
	  fprintf (PSfile, "} bind def\n\n");
	  
	  fprintf (PSfile, "/sf	%% font val =->\n");
	  fprintf (PSfile, " { exch cvn findfont exch dup 10 eq {pop 9.8} if scalefont setfont } bind def\n");
	  fprintf (PSfile, "/s	%% string larg x y =->\n");
	  fprintf (PSfile, " { moveto \n");
	  fprintf (PSfile, "   1 index stringwidth pop sub\n");
	  fprintf (PSfile, "   1 index length div 0 3 2 roll ashow } bind def\n");
	  fprintf (PSfile, "/c	%% y x string =->\n");
	  fprintf (PSfile, " { dup 4 1 roll stringwidth pop 2 div sub exch moveto show\n");
	  fprintf (PSfile, " } bind def\n");
	  fprintf (PSfile, "/j  %% string nbl larg x y =->\n");
	  fprintf (PSfile, " { moveto\n");
	  fprintf (PSfile, "   2 index\n");
	  fprintf (PSfile, "   stringwidth pop sub dup\n");
	  fprintf (PSfile, "   0 ge { exch div 0 32 4 3 roll widthshow }\n");
	  fprintf (PSfile, "	{ 2 index length div exch pop\n");
	  fprintf (PSfile, "	  0 3 2 roll ashow }\n");
	  fprintf (PSfile, "	ifelse\n");
	  fprintf (PSfile, " } bind def\n");
	  fprintf (PSfile, "/r	%% Radical\n");
	  fprintf (PSfile, " { gsave newpath moveto gsave -1 -1 rlineto 0.75 setlinewidth\n");
	  fprintf (PSfile, "   stroke grestore\n");
	  fprintf (PSfile, "   lineto 1.5 setlinewidth %%1 setlinecap\n");
	  fprintf (PSfile, "   gsave stroke grestore\n");
	  fprintf (PSfile, "   lineto lineto 0 setlinejoin 2 setmiterlimit\n");
	  fprintf (PSfile, "   0.75 setlinewidth stroke grestore\n");
	  fprintf (PSfile, " } bind def\n");
	  fprintf (PSfile, "/Seg	%% (x y)* dash width nb Segments\n");
	  fprintf (PSfile, "{ [/nb /ep /style] {exch def} forall\n");
	  fprintf (PSfile, "   newpath\n");
	  fprintf (PSfile, "   moveto\n");
	  fprintf (PSfile, "   2 1 nb {pop lineto} for\n");
	  fprintf (PSfile, "   0 setlinejoin ep 0.4 mul setlinewidth \n");
	  fprintf (PSfile, "   style dup 0 gt\n");
	  fprintf (PSfile, "   {5 mul [ exch 3 ] 0 setdash}\n");
	  fprintf (PSfile, "   {pop [ ] 0 setdash}\n");
	  fprintf (PSfile, "   ifelse\n");
	  fprintf (PSfile, "   ep 0 gt {stroke} if\n");
	  fprintf (PSfile, "} bind def\n");
	  fprintf (PSfile, "/Poly	%% rgb fill (x y)* dash width nb Polygone\n");
	  fprintf (PSfile, "{ [/nb /ep /style] {exch def} forall\n");
	  fprintf (PSfile, "   newpath\n");
	  fprintf (PSfile, "   moveto\n");
	  fprintf (PSfile, "   2 1 nb {pop lineto} for\n");
	  fprintf (PSfile, "   closepath\n");
	  fprintf (PSfile, "   fillbox\n");
	  fprintf (PSfile, "   0 setlinejoin ep 0.4 mul setlinewidth \n");
	  fprintf (PSfile, "   style dup 0 gt\n");
	  fprintf (PSfile, "   {5 mul [ exch 3 ] 0 setdash}\n");
	  fprintf (PSfile, "   {pop [ ] 0 setdash}\n");
	  fprintf (PSfile, "   ifelse\n");
	  fprintf (PSfile, "   ep 0 gt {stroke} if\n");
	  fprintf (PSfile, "} bind def\n");
	  fprintf (PSfile, "/Curv	%% (x y)* dash width nb Curves\n");
	  fprintf (PSfile, "{ [/nb /ep /style] {exch def} forall\n");
	  fprintf (PSfile, "   newpath\n");
	  fprintf (PSfile, "   moveto\n");
	  fprintf (PSfile, "   2 1 nb {pop curveto} for\n");
	  fprintf (PSfile, "   0 setlinejoin ep 0.4 mul setlinewidth \n");
	  fprintf (PSfile, "   style dup 0 gt\n");
	  fprintf (PSfile, "   {5 mul [ exch 3 ] 0 setdash}\n");
	  fprintf (PSfile, "   {pop [ ] 0 setdash}\n");
	  fprintf (PSfile, "   ifelse\n");
	  fprintf (PSfile, "   ep 0 gt {stroke} if\n");
	  fprintf (PSfile, "} bind def\n");
	  fprintf (PSfile, "/Splin	%% rgb fill (x y)* dash width nb Spline\n");
	  fprintf (PSfile, "{ [/nb /ep /style] {exch def} forall\n");
	  fprintf (PSfile, "   newpath\n");
	  fprintf (PSfile, "   moveto\n");
	  fprintf (PSfile, "   2 1 nb {pop curveto} for\n");
	  fprintf (PSfile, "   closepath\n");
	  fprintf (PSfile, "   fillbox\n");
	  fprintf (PSfile, "   0 setlinejoin ep 0.4 mul setlinewidth \n");
	  fprintf (PSfile, "   style dup 0 gt\n");
	  fprintf (PSfile, "   {5 mul [ exch 3 ] 0 setdash}\n");
	  fprintf (PSfile, "   {pop [ ] 0 setdash}\n");
	  fprintf (PSfile, "   ifelse\n");
	  fprintf (PSfile, "   ep 0 gt {stroke} if\n");
	  fprintf (PSfile, "} bind def\n");
	  fprintf (PSfile, "/s3	%% x yb yt h c1 c2 c3 =->\n");
	  fprintf (PSfile, " { [/c3 /c2 /c1 /h /yt /yb /x] {exch def} forall\n");
	  fprintf (PSfile, "   c1 stringwidth pop 2 div x exch sub /x exch def\n");
	  fprintf (PSfile, "   x yt moveto c1 show\n");
	  fprintf (PSfile, "   x yb moveto c3 show\n");
	  fprintf (PSfile, "   { yt h sub /yt exch def\n");
	  fprintf (PSfile, "     yt yb sub h gt\n");
	  fprintf (PSfile, "     { yb h add /yb exch def\n");
	  fprintf (PSfile, "       x yt moveto c2 show x yb moveto c2 show\n");
	  fprintf (PSfile, "     }\n");
	  fprintf (PSfile, "     { yt yb gt\n");
	  fprintf (PSfile, "       { yt yb h 0.75 mul add le { yt h 4 div add /yt exch def } if\n");
	  fprintf (PSfile, "         x yt moveto c2 show\n");
	  fprintf (PSfile, "       } if\n");
	  fprintf (PSfile, "       exit\n");
	  fprintf (PSfile, "     } ifelse\n");
	  fprintf (PSfile, "   } loop\n");
	  fprintf (PSfile, " } bind def\n");
	  fprintf (PSfile, "/s4	%% x yb yt h c1 c2 c3 c4 =->\n");
	  fprintf (PSfile, " { 10 dict begin \n");
	  fprintf (PSfile, "   [/c4 /c3 /c2 /c1 /h /yt /yb /x] {exch def} forall\n");
	  fprintf (PSfile, "   c1 stringwidth pop 2 div x exch sub /x exch def\n");
	  fprintf (PSfile, "   x yt moveto c1 show\n");
	  fprintf (PSfile, "   x yb moveto c3 show\n");
	  fprintf (PSfile, "   yt yt yb sub 2 div sub /ym exch def\n");
	  fprintf (PSfile, " x ym moveto c2 show\n");
	  fprintf (PSfile, "   /ymb ym def\n");
	  fprintf (PSfile, "   { yt h sub /yt exch def\n");
	  fprintf (PSfile, "     yt ym sub h gt\n");
	  fprintf (PSfile, "     { ym h add /ym exch def\n");
	  fprintf (PSfile, "       x yt moveto c4 show x ym moveto c4 show\n");
	  fprintf (PSfile, "       ymb h sub /ymb exch def yb h add /yb exch def\n");
	  fprintf (PSfile, "       x ymb moveto c4 show x yb moveto c4 show\n");
	  fprintf (PSfile, "     }\n");
	  fprintf (PSfile, "     { ymb h sub /ymb exch def\n");
	  fprintf (PSfile, "       yt ym gt\n");
	  fprintf (PSfile, "       { yt ym h 0.5 mul add le\n");
	  fprintf (PSfile, "         { yt h 0.75 mul add /yt exch def\n");
	  fprintf (PSfile, "           ymb h 0.25 mul add /ymb exch def\n");
	  fprintf (PSfile, "         } if\n");
	  fprintf (PSfile, "         x yt moveto c4 show x ymb moveto c4 show\n");
	  fprintf (PSfile, "       } if\n");
	  fprintf (PSfile, "       exit\n");
	  fprintf (PSfile, "     } ifelse\n");
	  fprintf (PSfile, "   } loop end\n");
	  fprintf (PSfile, " } bind def\n");
	  fprintf (PSfile, "/arr	%% xQueue yQueue xTail yTail th headth headlg Arrow\n");
	  fprintf (PSfile, " { 16 dict begin\n");
	  fprintf (PSfile, "   /mtrx matrix def\n");
	  fprintf (PSfile, "   /hlg exch def\n");
	  fprintf (PSfile, "   /hthick exch 3 div def\n");
	  fprintf (PSfile, "   /thick exch def\n");
	  fprintf (PSfile, "   /tipy exch def /tipx exch def\n");
	  fprintf (PSfile, "   /taily exch def /tailx exch def\n");
	  fprintf (PSfile, "   /dash exch def\n");
	  fprintf (PSfile, "   /dx tipx tailx sub def\n");
	  fprintf (PSfile, "   /dy tipy taily sub def\n");
	  fprintf (PSfile, "   /arlg dx dx mul dy dy mul add sqrt def\n");
	  fprintf (PSfile, "   dx 0 eq dy 0 eq and \n");
	  fprintf (PSfile, "   { /angle 0 def }\n");
	  fprintf (PSfile, "   { /angle dy dx atan def }\n");
	  fprintf (PSfile, "   ifelse\n");
	  fprintf (PSfile, "   /base arlg hlg sub def\n");
	  fprintf (PSfile, "   /savem mtrx currentmatrix def\n");
	  fprintf (PSfile, "   tailx taily translate\n");
	  fprintf (PSfile, "   angle rotate\n");
	  fprintf (PSfile, "   newpath\n");
	  fprintf (PSfile, "   base hthick neg moveto\n");
	  fprintf (PSfile, "   arlg 0 lineto base hthick lineto\n");
	  fprintf (PSfile, "   closepath\n");
	  fprintf (PSfile, "   fill\n");
	  fprintf (PSfile, "   newpath\n");
	  fprintf (PSfile, "   0 0 moveto base 0 moveto\n");
	  fprintf (PSfile, "   thick 0.4 mul setlinewidth\n");
	  fprintf (PSfile, "   savem setmatrix\n");
	  fprintf (PSfile, "   dash 0 gt \n");
	  fprintf (PSfile, "   { dash 5 mul /dash def\n");
	  fprintf (PSfile, "     [dash 3] 0 setdash\n");
	  fprintf (PSfile, "   }\n");
	  fprintf (PSfile, "   { [] 0 setdash }\n");
	  fprintf (PSfile, "   ifelse\n");
	  fprintf (PSfile, "   thick 0 gt {stroke} if\n");
	  fprintf (PSfile, "   end\n");
	  fprintf (PSfile, " } bind def\n");
	  fprintf (PSfile, "/ov	%% Ovale\n");
	  fprintf (PSfile, "{\n");
	  fprintf (PSfile, "  8 dict begin\n");
	  fprintf (PSfile, "  /ray exch def\n");
	  fprintf (PSfile, "  newpath moveto\n");
	  fprintf (PSfile, "  /y1 exch def\n");
	  fprintf (PSfile, "  /x1 exch def\n");
	  fprintf (PSfile, "  4 {\n");
	  fprintf (PSfile, "    /y2 exch def /x2 exch def\n");
	  fprintf (PSfile, "    x1 y1 x2 y2 ray arcto pop pop pop pop\n");
	  fprintf (PSfile, "    /y1 y2 def /x1 x2 def\n");
	  fprintf (PSfile, "  } repeat closepath\n");
	  fprintf (PSfile, "  /ep exch def /dash exch def\n");
	  fprintf (PSfile, "  fillbox\n");
	  fprintf (PSfile, "  ep 0.4 mul setlinewidth \n");
	  fprintf (PSfile, "  dash dup 0 gt\n");
	  fprintf (PSfile, "  { 5 mul [ exch 3 ] 0 setdash }\n");
	  fprintf (PSfile, "  {  pop [ ] 0 setdash }\n");
	  fprintf (PSfile, "  ifelse\n");
	  fprintf (PSfile, "  ep 0 gt {stroke} if\n");
	  fprintf (PSfile, "  end\n");
	  fprintf (PSfile, " } bind def\n");
	  fprintf (PSfile, "/cer  %% Cercle ep x y R\n");
	  fprintf (PSfile, "{\n");
	  fprintf (PSfile, "  2 index 2 index moveto \n");
	  fprintf (PSfile, "  newpath 0 360 arc\n");
	  fprintf (PSfile, "  /ep exch def /dash exch def\n");
	  fprintf (PSfile, "  fillbox\n");
	  fprintf (PSfile, "  ep 0.4 mul setlinewidth \n");
	  fprintf (PSfile, "  dash dup 0 gt\n");
	  fprintf (PSfile, "  { 5 mul [ exch 3 ] 0 setdash }\n");
	  fprintf (PSfile, "  {  pop [ ] 0 setdash }\n");
	  fprintf (PSfile, "  ifelse\n");
	  fprintf (PSfile, "  ep 0 gt {stroke} if\n");
	  fprintf (PSfile, "} bind def\n\n");

	  fprintf (PSfile, "/ellipse { %% ep x y A B\n");
	  fprintf (PSfile, "  1 index div /scalef exch def /bigaxis exch def\n");
	  fprintf (PSfile, "  moveto\n");
	  fprintf (PSfile, "  gsave\n");
	  fprintf (PSfile, "  1 scalef scale\n");
	  fprintf (PSfile, "  currentpoint newpath bigaxis 0 360 arc closepath\n");
	  fprintf (PSfile, "  /ep exch def /dash exch def\n");
	  fprintf (PSfile, "  fillbox\n");
	  fprintf (PSfile, "  ep 0.4 mul setlinewidth \n");
	  fprintf (PSfile, "  dash dup 0 gt\n");
	  fprintf (PSfile, "  { 5 mul [ exch 3 ] 0 setdash }\n");
	  fprintf (PSfile, "  {  pop [ ] 0 setdash }\n");
	  fprintf (PSfile, "  ifelse\n");
	  fprintf (PSfile, "  ep 0 gt {stroke} if\n");
	  fprintf (PSfile, "  grestore\n");
	  fprintf (PSfile, "} bind def\n\n");

	  fprintf (PSfile, "/DumpImage { %%%% Wim Him Wdr Hdr DumpImage\n");
	  fprintf (PSfile, "       /Hdr exch def /Wdr exch def /Him exch def /Wim exch def\n");
	  fprintf (PSfile, "       Wdr Hdr scale\n");
	  fprintf (PSfile, "       /picstr  Wim 7 add 8 idiv  string def\n");
	  fprintf (PSfile, "       Wim Him 1 [ Wim 0 0 Him neg 0 Him ]\n");
	  fprintf (PSfile, "          { currentfile picstr readhexstring pop } image\n");
	  fprintf (PSfile, " } bind def\n\n");

	  fprintf (PSfile, "/DumpImage2 { %%%% Wim Him Wdr Hdr DumpImage2\n");
	  fprintf (PSfile, "       /Hdr exch def /Wdr exch def /Him exch def /Wim exch def\n");
	  fprintf (PSfile, "       /picstr  Wim 3 mul string def\n");
	  fprintf (PSfile, "     	Wdr Hdr scale\n");
	  fprintf (PSfile, "        Wim Him 8 \n");
	  fprintf (PSfile, "	[ Wim 0 0 Him neg 0 Him ]\n");
	  fprintf (PSfile, "          { currentfile picstr readhexstring pop } \n");
	  fprintf (PSfile, "	false 3\n");
	  fprintf (PSfile, "	colorimage\n");
	  fprintf (PSfile, " } bind def\n\n");
	  
	  fprintf (PSfile, "%%%% define \'colorimage\' if it isn\'t defined\n");
	  fprintf (PSfile, "%%%%   (\'colortogray\' and \'mergeprocs\' come from xwd2ps\n");
	  fprintf (PSfile, "%%%%     via xgrab)\n");
	  fprintf (PSfile, "/colorimage where   %%%% do we know about \'colorimage\'?\n");
	  fprintf (PSfile, "  { pop }           %%%% yes: pop off the \'dict\' returned\n");
	  fprintf (PSfile, "  {                 %%%% no:  define one\n");
	  fprintf (PSfile, "    /colortogray {  %%%% define an RGB->I function\n");
	  fprintf (PSfile, "      /rgbdata exch store    %%%% call input \'rgbdata\'\n");
	  fprintf (PSfile, "      rgbdata length 3 idiv\n");
	  fprintf (PSfile, "      /npixls exch store\n");
	  fprintf (PSfile, "      /rgbindx 0 store\n");
	  fprintf (PSfile, "      /grays npixls string store  %%%% str to hold the result\n");
	  fprintf (PSfile, "      0 1 npixls 1 sub {\n");
	  fprintf (PSfile, "        grays exch\n");
	  fprintf (PSfile, "        rgbdata rgbindx       get 20 mul    %%%% Red\n");
	  fprintf (PSfile, "        rgbdata rgbindx 1 add get 32 mul    %%%% Green\n");
	  fprintf (PSfile, "        rgbdata rgbindx 2 add get 12 mul    %%%% Blue\n");
	  fprintf (PSfile, "        add add 64 idiv      %%%% I = .5G + .31R + .18B\n");
	  fprintf (PSfile, "        put\n");
	  fprintf (PSfile, "        /rgbindx rgbindx 3 add store\n");
	  fprintf (PSfile, "      } for\n");
	  fprintf (PSfile, "      grays\n");
	  fprintf (PSfile, "    } bind def\n\n");
	  
	  fprintf (PSfile, "    %%%% Utility procedure for colorimage operator.\n");
	  fprintf (PSfile, "    %%%% This procedure takes two procedures off the\n");
	  fprintf (PSfile, "    %%%% stack and merges them into a single procedure.\n\n");
	  
	  fprintf (PSfile, "    /mergeprocs { %%%% def\n");
	  fprintf (PSfile, "      dup length\n");
	  fprintf (PSfile, "      3 -1 roll\n");
	  fprintf (PSfile, "      dup\n");
	  fprintf (PSfile, "      length\n");
	  fprintf (PSfile, "      dup\n");
	  fprintf (PSfile, "      5 1 roll\n");
	  fprintf (PSfile, "      3 -1 roll\n");
	  fprintf (PSfile, "      add\n");
	  fprintf (PSfile, "      array cvx\n");
	  fprintf (PSfile, "      dup\n");
	  fprintf (PSfile, "      3 -1 roll\n");
	  fprintf (PSfile, "      0 exch\n");
	  fprintf (PSfile, "      putinterval\n");
	  fprintf (PSfile, "      dup\n");
	  fprintf (PSfile, "      4 2 roll\n");
	  fprintf (PSfile, "      putinterval\n");
	  fprintf (PSfile, "    } bind def\n\n");
	  
	  fprintf (PSfile, "    /colorimage { %%%% def\n");
	  fprintf (PSfile, "      pop pop     %%%% remove \'false 3\' operands\n");
	  fprintf (PSfile, "      {colortogray} mergeprocs\n");
	  fprintf (PSfile, "      image\n");
	  fprintf (PSfile, "    } bind def\n");
	  fprintf (PSfile, "  } ifelse          %%%% end of \'false\' case\n\n");
	  
	  fprintf (PSfile, "/Pes	%% x y width =->\n");
	  fprintf (PSfile, " { 5 dict begin\n");
	  fprintf (PSfile, "   [/xf /y /x] {exch def} forall\n");
	  fprintf (PSfile, "   /st (.) def\n");
	  fprintf (PSfile, "   /lg st stringwidth pop 3 mul def\n");
	  fprintf (PSfile, "   /xf xf x add def\n");
	  fprintf (PSfile, "   { xf x gt\n");
	  fprintf (PSfile, "     { xf y moveto\n");
	  fprintf (PSfile, "       st show\n");
	  fprintf (PSfile, "       /xf xf lg sub def\n");
	  fprintf (PSfile, "     }\n");
	  fprintf (PSfile, "     { exit }\n");
	  fprintf (PSfile, "     ifelse\n");
	  fprintf (PSfile, "   } loop end\n");
	  fprintf (PSfile, " } bind def\n");
	  fprintf (PSfile, "/gr	%%%% framework for graphics\n");
	  fprintf (PSfile, " { newpath moveto lineto lineto lineto closepath\n");
	  fprintf (PSfile, "   0.125 mul 1 exch sub setgray fill 0 setgray\n");
	  fprintf (PSfile, " } bind def\n\n");
	  
	  fprintf (PSfile, "/trm    %%%% framework for empty boxes\n");
	  fprintf (PSfile, "{ suptrame 0 eq {gr} {pop pop pop pop pop pop pop pop pop} ifelse\n");
	  fprintf (PSfile, "} bind def\n\n");
	  
	  fprintf (PSfile, "/BEGINEPSFILE {%%def\n");
	  fprintf (PSfile, "  /EPSFsave save def\n");
	  fprintf (PSfile, "  0 setgray 0 setlinecap 1 setlinewidth 0 setlinejoin 10 setmiterlimit [] 0 setdash\n");
	  fprintf (PSfile, "  newpath\n");
	  fprintf (PSfile, "  /showpage {} def\n");
	  fprintf (PSfile, "} bind def\n\n");
	  
	  fprintf (PSfile, "/ENDEPSFILE {%%def\n");
	  fprintf (PSfile, "  EPSFsave restore\n");
	  fprintf (PSfile, "} bind def\n\n");
	  
	  fprintf (PSfile, "%%%%%%%%%%%%%%%%%%%%%%%%%%%% Encoding of font dictionaries %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n\n");
	  
	  fprintf (PSfile, "/ISOLatin1Encoding where { pop } { /ISOLatin1Encoding\n");
	  fprintf (PSfile, "[/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef\n");
	  fprintf (PSfile, "/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef\n");
	  fprintf (PSfile, "/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef\n");
	  fprintf (PSfile, "/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/space\n");
	  fprintf (PSfile, "/exclam/quotedbl/numbersign/dollar/percent/ampersand/quoteright\n");
	  fprintf (PSfile, "/parenleft/parenright/asterisk/plus/comma/minus/period/slash/zero/one\n");
	  fprintf (PSfile, "/two/three/four/five/six/seven/eight/nine/colon/semicolon/less/equal\n");
	  fprintf (PSfile, "/greater/question/at/A/B/C/D/E/F/G/H/I/J/K/L/M/N/O/P/Q/R/S\n");
	  fprintf (PSfile, "/T/U/V/W/X/Y/Z/bracketleft/backslash/bracketright/asciicircum\n");
	  fprintf (PSfile, "/underscore/quoteleft/a/b/c/d/e/f/g/h/i/j/k/l/m/n/o/p/q/r/s\n");
	  fprintf (PSfile, "/t/u/v/w/x/y/z/braceleft/bar/braceright/asciitilde/.notdef/.notdef\n");
	  fprintf (PSfile, "/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef\n");
	  fprintf (PSfile, "/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/dotlessi/grave\n");
	  fprintf (PSfile, "/acute/circumflex/tilde/macron/breve/dotaccent/dieresis/.notdef/ring\n");
	  fprintf (PSfile, "/cedilla/.notdef/hungarumlaut/ogonek/caron/space/exclamdown/cent\n");
	  fprintf (PSfile, "/sterling/currency/yen/brokenbar/section/dieresis/copyright/ordfeminine\n");
	  fprintf (PSfile, "/guillemotleft/logicalnot/hyphen/registered/macron/degree/plusminus\n");
	  fprintf (PSfile, "/twosuperior/threesuperior/acute/mu/paragraph/periodcentered/cedilla\n");
	  fprintf (PSfile, "/onesuperior/ordmasculine/guillemotright/onequarter/onehalf/threequarters\n");
	  fprintf (PSfile, "/questiondown/Agrave/Aacute/Acircumflex/Atilde/Adieresis/Aring/AE\n");
	  fprintf (PSfile, "/Ccedilla/Egrave/Eacute/Ecircumflex/Edieresis/Igrave/Iacute/Icircumflex\n");
	  fprintf (PSfile, "/Idieresis/Eth/Ntilde/Ograve/Oacute/Ocircumflex/Otilde/Odieresis\n");
	  fprintf (PSfile, "/OE/Oslash/Ugrave/Uacute/Ucircumflex/Udieresis/Yacute/Thorn\n");
	  fprintf (PSfile, "/germandbls/agrave/aacute/acircumflex/atilde/adieresis/aring/ae\n");
	  fprintf (PSfile, "/ccedilla/egrave/eacute/ecircumflex/edieresis/igrave/iacute/icircumflex\n");
	  fprintf (PSfile, "/idieresis/eth/ntilde/ograve/oacute/ocircumflex/otilde/odieresis/oe\n");
	  fprintf (PSfile, "/oslash/ugrave/uacute/ucircumflex/udieresis/yacute/thorn/ydieresis]\n");
	  fprintf (PSfile, "def %%ISOLatin1Encoding\n");
	  fprintf (PSfile, "} ifelse\n\n");

	  fprintf (PSfile, "/ReEncode	%% NewFont Font ReEncode\n");
	  fprintf (PSfile, "  { findfont	%% load desired font\n");
	  fprintf (PSfile, "    dup maxlength dict /newfont exch def	%% allocate new fontdict\n");
	  fprintf (PSfile, "    dup\n");
	  fprintf (PSfile, "    { exch dup dup dup dup /FID ne exch /Encoding ne and exch /FontBBox ne and exch /FontMatrix ne and\n");
	  fprintf (PSfile, "	{ exch newfont 3 1 roll put }\n");
	  fprintf (PSfile, "	{ pop pop }\n");
	  fprintf (PSfile, "	ifelse }\n");
	  fprintf (PSfile, "    forall	%% copy fontdict\n\n");
	  
	  fprintf (PSfile, "    dup /FontBBox get 4 array copy newfont /FontBBox 3 -1 roll put\n");
	  fprintf (PSfile, "    /FontMatrix get 6 array copy newfont /FontMatrix 3 -1 roll put\n\n");
	  
	  fprintf (PSfile, "    newfont /Encoding ISOLatin1Encoding 256 array copy put	%%put new encoding vector\n");
	  fprintf (PSfile, "    newfont /Encoding get dup dup dup				%%add /**OE*/\n");
	  fprintf (PSfile, "			 8#230 /divide put			%%add /**OE*/\n");
	  fprintf (PSfile, "			 8#231 /multiply put			%%add /**OE*/\n");
	  fprintf (PSfile, "			 8#367 /oe put				%%add /**OE*/\n");
	  fprintf (PSfile, "			 8#327 /OE put				%%add /**OE*/\n\n");
	  
	  fprintf (PSfile, "    dup newfont /FontName 3 -1 roll put	%%put a new Fontname\n");
	  fprintf (PSfile, "    newfont definefont pop	%%define a new font\n");
	  fprintf (PSfile, "  } def\n\n");
	  
	  fprintf (PSfile, "/ReEncodeOblique	%% NewFont Font ReEncodeOblique\n");
	  fprintf (PSfile, "  { findfont	%% load desired font\n");
	  fprintf (PSfile, "    dup maxlength dict /newfont exch def	%% allocate new fontdict\n");
	  fprintf (PSfile, "    dup\n");
	  fprintf (PSfile, "    { exch dup dup dup dup /FID ne exch /Encoding ne and exch /FontBBox ne and exch /FontMatrix ne and\n");
	  fprintf (PSfile, "	{ exch newfont 3 1 roll put }\n");
	  fprintf (PSfile, "	{ pop pop }\n");
	  fprintf (PSfile, "	ifelse }\n");
	  fprintf (PSfile, "    forall	%% copy fontdict\n\n");
	  
	  fprintf (PSfile, "    dup /FontBBox get 4 array copy newfont /FontBBox 3 -1 roll put\n");
	  fprintf (PSfile, "    /FontMatrix get \n");
	  fprintf (PSfile, "    [1 0 15 sin 1 0 0 ] exch 6 array concatmatrix\n");
	  fprintf (PSfile, "    newfont /FontMatrix 3 -1 roll put\n\n");
	  
	  fprintf (PSfile, "    newfont /Encoding ISOLatin1Encoding 256 array copy put	%%put new encoding vector\n");
	  fprintf (PSfile, "    newfont /Encoding get dup dup dup				%%add /**OE*/\n");
	  fprintf (PSfile, "			 8#230 /divide put			%%add /**OE*/\n");
	  fprintf (PSfile, "			 8#231 /multiply put			%%add /**OE*/\n");
	  fprintf (PSfile, "			 8#367 /oe put				%%add /**OE*/\n");
	  fprintf (PSfile, "			 8#327 /OE put				%%add /**OE*/\n\n");
	  
	  fprintf (PSfile, "    dup newfont /FontName 3 -1 roll put	%%put a new Fontname\n");
	  fprintf (PSfile, "    newfont definefont pop	%%define a new font\n");
	  fprintf (PSfile, "  } def\n\n");
	  
	  fprintf (PSfile, "/lhr /Helvetica 		ReEncode\n");
	  fprintf (PSfile, "/lhb /Helvetica-Bold  	ReEncode\n");
	  fprintf (PSfile, "/lho /Helvetica-Oblique  	ReEncode\n");
	  fprintf (PSfile, "/lhq /Helvetica-BoldOblique ReEncode\n\n");
	  
	  fprintf (PSfile, "/lhr (lhr) def\n");
	  fprintf (PSfile, "/lhb (lhb) def\n");
	  fprintf (PSfile, "/lhi (lho) def\n");
	  fprintf (PSfile, "/lho (lho) def    %% lhi for helvetica italic ( = oblique )\n");
	  fprintf (PSfile, "/lhq (lhq) def\n");
	  fprintf (PSfile, "/lhg (lhq) def    %% lhg for helvetica bold italic ( = bold oblique )\n\n");
	  
	  fprintf (PSfile, "/ltr /Times-Roman 		ReEncode\n");
	  fprintf (PSfile, "/ltb /Times-Bold 		ReEncode\n");
	  fprintf (PSfile, "/lti /Times-Italic 		ReEncode\n");
	  fprintf (PSfile, "/ltg /Times-BoldItalic 	ReEncode\n");
	  fprintf (PSfile, "/lto /Times-Roman 		ReEncodeOblique\n");
	  fprintf (PSfile, "/ltq /Times-Bold 		ReEncodeOblique\n\n");
	  
	  fprintf (PSfile, "/ltr (ltr) def\n");
	  fprintf (PSfile, "/ltb (ltb) def\n");
	  fprintf (PSfile, "/lti (lti) def\n");
	  fprintf (PSfile, "/lto (lto) def\n");
	  fprintf (PSfile, "/ltg (ltg) def\n");
	  fprintf (PSfile, "/ltq (ltq) def\n\n");

	  fprintf (PSfile, "/lcr /Courier 		ReEncode\n");
	  fprintf (PSfile, "/lcb /Courier-Bold 		ReEncode\n");
	  fprintf (PSfile, "/lco /Courier-Oblique	ReEncode\n");
	  fprintf (PSfile, "/lcq /Courier-BoldOblique 	ReEncode\n\n");
	  
	  fprintf (PSfile, "/lcr (lcr) def\n");
	  fprintf (PSfile, "/lcb (lcb) def\n");
	  fprintf (PSfile, "/lco (lco) def\n");
	  fprintf (PSfile, "/lci (lco) def    %% lci for courier italic ( = oblique )\n");
	  fprintf (PSfile, "/lcq (lcq) def\n");
	  fprintf (PSfile, "/lcg (lcq) def    %% lcq for courier bold italic ( = bold oblique )\n\n");
	  
	  fprintf (PSfile, "/ggr (Symbol) def\n\n");
	  
	  fprintf (PSfile, "%%%%%%%%%%%%%%%%%%%%%% Loading font commands %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n\n");
	  
	  fprintf (PSfile, "/pagecounter 0 def\n\n");
	  
	  fprintf (PSfile, "/pagenumberok 		%% int pagenumberok ThotBool\n");
	  fprintf (PSfile, " { evenodd 0 eq 	%% evenodd = 0 -> all pages\n");
	  fprintf (PSfile, "	{ pop true }\n");
	  fprintf (PSfile, "	{ evenodd 1 eq 	%% evenodd = 1 -> odd pages\n");
	  fprintf (PSfile, "		{ 2 mod 1 eq { true }	%% counter value is odd\n");
	  fprintf (PSfile, "			     { false }\n");
	  fprintf (PSfile, "			     ifelse }\n");
	  fprintf (PSfile, "	 	{ 2 mod 0 eq { true }	%% evenodd = 2 -> even pages and the counter value is even\n");
	  fprintf (PSfile, "			     { false }\n");
	  fprintf (PSfile, "			     ifelse }\n");
	  fprintf (PSfile, "		ifelse }\n");
	  fprintf (PSfile, "	ifelse } bind def\n\n");
	  
	  fprintf (PSfile, "/pagesizeok\n");
	  fprintf (PSfile, " { pop pop true } def\n\n");

	  fprintf (PSfile, "/frstpage	%% first page\n");
	  fprintf (PSfile, " { } bind def\n\n");
	  
	  fprintf (PSfile, "/nwpage		%% new page\n");
	  fprintf (PSfile, " { /pagecounter pagecounter 1 add def\n");
	  fprintf (PSfile, "   pagesizeok { pagenumberok { showpage }\n");
	  fprintf (PSfile, "			     {erasepage } ifelse } \n");
	  fprintf (PSfile, "	      { erasepage } ifelse\n");
	  fprintf (PSfile, "   grestore\n");
	  fprintf (PSfile, "   VectMatrice pagecounter 4 mod get setmatrix \n");
	  fprintf (PSfile, "   gsave UserMatrice concat } def\n\n");
	  
	  fprintf (PSfile, "/showpage { pagecounter ppf mod 0 eq {systemdict /showpage get exec} if } bind def\n");
	  fprintf (PSfile, "/ejectpage { pagecounter ppf mod 0 ne {systemdict /showpage get exec} if } bind def\n\n");
	  
	  fprintf (PSfile, "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n\n");

	  fprintf (PSfile, "/MatriceDict 9 dict def\n");
	  fprintf (PSfile, "MatriceDict begin\n\n");

	  fprintf (PSfile, "    /MatPortrait matrix def\n");
	  fprintf (PSfile, "    /MatLandscape [0 1 -1 0 0 0] def\n\n");

	  fprintf (PSfile, "    /defmat { \n");
	  fprintf (PSfile, "	matrix concatmatrix matrix concatmatrix matrix currentmatrix matrix concatmatrix } bind def\n\n");

	  fprintf (PSfile, "    /defdict_portrait {	%% dict defdict\n");
	  fprintf (PSfile, "	/Portrait 3 dict def \n");
	  fprintf (PSfile, "	Portrait begin\n");
	  fprintf (PSfile, "	    /1ppf 2 dict def 1ppf begin\n");
	  fprintf (PSfile, "		/VectMatrice [\n");
	  fprintf (PSfile, "	    	    MatPortrait MatScale1 [1 0 0 1 0 PHeight] defmat\n");
	  fprintf (PSfile, "		    dup dup dup ] def\n");
	  fprintf (PSfile, "		/ppf 1 def\n");
	  fprintf (PSfile, "	    end %% 1ppf dict\n");
	  fprintf (PSfile, "	    /2ppf 2 dict def 2ppf begin\n");
	  fprintf (PSfile, "		/VectMatrice [\n");
	  fprintf (PSfile, "	    	    MatLandscape MatScale2 [1 0 0 1 0 0] defmat\n");
	  fprintf (PSfile, "		    MatLandscape MatScale2 [1 0 0 1 0 PHeight 2 div] defmat\n");
	  fprintf (PSfile, "	    	    2 copy ] def\n");
	  fprintf (PSfile, "		/ppf 2 def\n");
	  fprintf (PSfile, "	    end %% 2ppf dict\n");
	  fprintf (PSfile, "	    /4ppf 2 dict def 4ppf begin\n");
	  fprintf (PSfile, "		/VectMatrice [\n");
	  fprintf (PSfile, "	    	    MatPortrait MatScale4 [1 0 0 1 0 PHeight] defmat\n");
	  fprintf (PSfile, "		    MatPortrait MatScale4 [1 0 0 1 PWidth 2 div PHeight] defmat\n");
	  fprintf (PSfile, "	    	    MatPortrait MatScale4 [1 0 0 1 0 PHeight 2 div] defmat\n");
	  fprintf (PSfile, "		    MatPortrait MatScale4 [1 0 0 1 PWidth 2 div PHeight 2 div] defmat\n");
	  fprintf (PSfile, "		    ] def\n");
	  fprintf (PSfile, "		/ppf 4 def\n");
	  fprintf (PSfile, "	    end %% 4ppf dict\n");
	  fprintf (PSfile, "	end %% Portrait dict\n");
	  fprintf (PSfile, "    } bind def %% end defdict_portrait\n\n");
	  
	  fprintf (PSfile, "    /defdict_paysage {	%% defdict\n");
	  fprintf (PSfile, "	/Landscape 3 dict def\n");
	  fprintf (PSfile, "	Landscape begin\n");
	  fprintf (PSfile, "	    /1ppf 2 dict def 1ppf begin\n");
	  fprintf (PSfile, "		/VectMatrice [\n");
	  fprintf (PSfile, "	    	    MatLandscape MatScale1 [1 0 0 1 0 842 PHeight sub ] defmat\n");
	  fprintf (PSfile, "		    dup dup dup ] def\n");
	  fprintf (PSfile, "		/ppf 1 def\n");
	  fprintf (PSfile, "	    end %% 1ppf dict\n");
	  fprintf (PSfile, "	    /2ppf 2 dict def 2ppf begin\n");
	  fprintf (PSfile, "		/VectMatrice [\n");
	  fprintf (PSfile, "	    	    MatPortrait MatScale2 [1 0 0 1 0 PHeight] defmat\n");
	  fprintf (PSfile, "		     MatPortrait MatScale2 [1 0 0 1 0 420] defmat\n");
	  fprintf (PSfile, "	    	    2 copy ] def\n");
	  fprintf (PSfile, "		/ppf 2 def\n");
	  fprintf (PSfile, "	    end %% 2ppf dict\n");
	  fprintf (PSfile, "	    /4ppf 2 dict def 4ppf begin\n");
	  fprintf (PSfile, "		/VectMatrice [\n");
	  fprintf (PSfile, "	    	    MatLandscape MatScale4 [1 0 0 1 0 0] defmat\n");
	  fprintf (PSfile, "		    MatLandscape MatScale4 [1 0 0 1 0 PHeight 2 div] defmat\n");
	  fprintf (PSfile, " 		    MatLandscape MatScale4 [1 0 0 1 PWidth 2 div 0] defmat\n");
	  fprintf (PSfile, "		    MatLandscape MatScale4 [1 0 0 1 PWidth 2 div PHeight 2 div] defmat\n");
	  fprintf (PSfile, "		    ] def\n");
	  fprintf (PSfile, "		/ppf 4 def\n");
	  fprintf (PSfile, "	    end %% 4ppf dict\n");
	  fprintf (PSfile, "	end %% Landscape dict\n");
	  fprintf (PSfile, "    } bind def %% end defdict_paysage\n\n");
	  
	  fprintf (PSfile, "    /A4 8 dict def\n");
	  fprintf (PSfile, "    A4 begin\n");
	  fprintf (PSfile, "    	/MatScale1 matrix def\n");
	  fprintf (PSfile, "    	/MatScale2 [1 2 sqrt div 0 0 2 index 0 0] def\n");
	  fprintf (PSfile, "    	/MatScale4 [0.5 0 0 0.5 0 0] def\n");
	  fprintf (PSfile, "	/PHeight 842 def\n");
	  fprintf (PSfile, "	/PWidth 596 def\n");
	  fprintf (PSfile, "	end\n\n");
	  
	  fprintf (PSfile, "    /US 8 dict def\n");
	  fprintf (PSfile, "    US begin\n");
	  fprintf (PSfile, "    	/MatScale1 [1 0 0 1 0 -50.01] def\n");
	  fprintf (PSfile, "    	/MatScale2 [1 3 sqrt div 0 0 2 index 0 0] def\n");
	  fprintf (PSfile, "    	/MatScale4 [0.45 0 0 0.45 0 0] def\n");
	  fprintf (PSfile, "	/PHeight 792 def\n");
	  fprintf (PSfile, "	/PWidth 612 def\n");
	  fprintf (PSfile, "	end\n\n");
	  
	  fprintf (PSfile, "    /A3 8 dict def\n");
	  fprintf (PSfile, "    A3 begin\n");
	  fprintf (PSfile, "    	/MatScale1 matrix def\n");
	  fprintf (PSfile, "    	/MatScale2 [1 2 sqrt div 0 0 2 index 0 0] def\n");
	  fprintf (PSfile, "    	/MatScale4 [0.5 0 0 0.5 0 0] def\n");
	  fprintf (PSfile, "	/PHeight 842 def\n");
	  fprintf (PSfile, "	/PWidth 596 def\n");
	  fprintf (PSfile, "	end\n\n");
	  
	  fprintf (PSfile, "    /A5 8 dict def\n");
	  fprintf (PSfile, "    A5 begin\n");
	  fprintf (PSfile, "    	/MatScale1 matrix def\n");
	  fprintf (PSfile, "    	/MatScale2 matrix def\n");
	  fprintf (PSfile, "    	/MatScale4 [1 2 sqrt div 0 0 2 index 0 0] def\n");
	  fprintf (PSfile, "	/PHeight 842 def\n");
	  fprintf (PSfile, "	/PWidth 596 def\n");
	  fprintf (PSfile, "	end\n\n");

	  fprintf (PSfile, "end %% Matrice dict\n\n");

	  fprintf (PSfile, "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%DEMARRAGE%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n\n");

	  fprintf (PSfile, "/InitThot {\n");
	  fprintf (PSfile, "    /UserMatrice \n");
	  fprintf (PSfile, "	[reduction 100 div 0 0 2 index 0 0] \n");
	  fprintf (PSfile, "	[1 0 0 1 decalageH HPrinterOff add 72 mul 25.4 div decalageV VPrinterOff add 72 mul 25.4 div neg] matrix concatmatrix\n");
	  fprintf (PSfile, "        [1 0 0 1 0 0]\n");
	  fprintf (PSfile, "	matrix concatmatrix def\n");
	  fprintf (PSfile, "    MatriceDict begin\n");
	  fprintf (PSfile, "    page_size cvlit load	 		%% get the page_size dict\n");
	  fprintf (PSfile, "    begin defdict_portrait defdict_paysage	%% and init it\n");
	  fprintf (PSfile, "    currentdict orientation cvlit get		%% get the orientation dict\n");
	  fprintf (PSfile, "    nb_ppf cvlit get				%% get the nb_ppf dict\n");
	  fprintf (PSfile, "    begin \n");
	  fprintf (PSfile, "    VectMatrice 0 get setmatrix			%% init CTM\n");
	  fprintf (PSfile, "    gsave UserMatrice concat\n");
	  fprintf (PSfile, "    100 dict begin				%% working dict\n");
	  fprintf (PSfile, "    } bind def\n\n");
	  
	  fprintf (PSfile, "/EndThot {\n");
	  fprintf (PSfile, "   ejectpage\n");
	  fprintf (PSfile, "   end end end end end} bind def	%%close all open dict\n\n");

	  fprintf (PSfile, "%%%%EndProlog\n");
	  fprintf (PSfile, "/user_orientation 0 def  \n");

	  fprintf (PSfile, "statusdict begin\n");

	  if (manualFeed == 0)
	    {
	      if (!ustrcmp (pageSize, _A3PaperFormat_))
		fprintf (PSfile, "a3tray\n");
	    }
	  else
	    {
	      fprintf (PSfile, "/manualfeed true def\n");
	    }
	  if (BlackAndWhite != 0)
	    fprintf (PSfile, "1 setprocesscolors\n");
	  fprintf (PSfile, "end\n");
	  fprintf (PSfile, "/decalageH %d def /decalageV %d def\n", HorizShift, VertShift);
	  fprintf (PSfile, "/reduction %d def\n", Zoom);
	  fprintf (PSfile, "/page_size (%s) def\n", pageSize);
	  fprintf (PSfile, "/orientation (%s) def\n", Orientation);
	  fprintf (PSfile, "/nb_ppf (%dppf) def\n", NPagesPerSheet);
	  fprintf (PSfile, "/suptrame %d def\n", NoEmpyBox);
	  fprintf (PSfile, "/evenodd 0 def\n");
	  fprintf (PSfile, "/HPrinterOff 0 def\n");
	  fprintf (PSfile, "/VPrinterOff 0 def\n");
	  fprintf (PSfile, "InitThot\n");
	  fprintf (PSfile, "frstpage\n%%%%Page: cover 1\n");
	  NumberOfPages = 1;
	  fflush (PSfile);
	}
    }
  else
    {
      PSfile = (FILE *) FrRef[1];
      FrRef[i] = (ThotWindow) PSfile;
    }

  /* initialize visibility and zoom for the window */
  /* cf. procedure InitializeFrameParams */
  pFrame = &ViewFrameTable[i - 1];
  pFrame->FrVisibility = 5;	/* visibilite mise a 5 */
   pFrame->FrMagnification = 0;	/* zoom a 0 */

   /* initialize frames tabe because it's used by display functions */
   FrameTable[i].FrDoc = IdentDocument (pDoc);
   RemoveClipping(i);
   *volume = 16000;
   return (i);
}

/*----------------------------------------------------------------------
   ClosePSFile ferme le fichier PostScript.                        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ClosePSFile (int frame)
#else  /* __STDC__ */
static void         ClosePSFile (frame)
int                 frame;
#endif /* __STDC__ */
{
   FILE               *PSfile;

   PSfile = (FILE *) FrRef[frame];
   /* Faut-il ajouter un showpage ? */
   /*DrawPage (PSfile);*/

   /* Est-ce la fenetre principale ? */
   if (frame == 1)
     {
	/* Oui -> on ferme le fichier */
	fprintf (PSfile, "%%%%%%%%Trailer\nEndThot\n%%%%%%%%PaginateView: %d\n", NumberOfPages);

	fclose (PSfile);
     }
   FrRef[frame] = 0;
   /* Libere toutes les boites allouees */
   ClearConcreteImage (frame);
   /* On libere les polices de caracteres utilisees */
   ThotFreeFont (frame);
}


/*----------------------------------------------------------------------
   GivePageHeight force la limite d'affichage dans la fenetre frame a`   
   la hauteur d'une page de texte.                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         GivePageHeight (int frame, int org, int height)
#else  /* __STDC__ */
static void         GivePageHeight (frame, org, height)
int                 frame;
int                 org;
int                 height;

#endif /* __STDC__ */
{
   int                 y, h, framexmin, framexmax;
   ViewFrame          *pFrame;

   if (height != 0)
     {
	pFrame = &ViewFrameTable[frame - 1];
	/* On convertit suivant l'unite donnee */
	/* y = PixelValue (org, UnPoint, pFrame->FrAbstractBox, 0);
	h = PixelValue (height, UnPoint, pFrame->FrAbstractBox, 0); */
	y = org;
	h = y + height;
	pFrame->FrClipXBegin = 0;
	pFrame->FrClipXEnd = 32000;
	pFrame->FrClipYBegin = y;
	pFrame->FrYOrg = y;
	pFrame->FrClipYEnd = h;
    /* set the clipping to the frame size before generating postscript (RedrawFrameBottom) */
	DefineClipping (frame, pFrame->FrXOrg, pFrame->FrYOrg, &framexmin, &y, &framexmax, &h, 1);
     }
}

/*----------------------------------------------------------------------
   SetMargins met a jour PageHeight et PageFooterHeight ainsi que  
   les marges du haut et du cote gauche, selon le type de page        
   auquel appartient l'element Marque Page pointe par pPageEl.        
   Cette procedure utilise PageHeaderFooter (commune a print et page)         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         SetMargins (PtrElement pPageEl, PtrAbstractBox rootAbsBox)
#else  /* __STDC__ */
static void         SetMargins (pPageEl, rootAbsBox)
PtrElement          pPageEl;
PtrAbstractBox      rootAbsBox;
#endif /* __STDC__ */
{
   PtrPRule            pPRule;
   PtrPSchema          pPSchema;
   AbDimension        *pDim;
   AbPosition         *pPos;
   int                 boxNum, pageHeight, schView, NewLeftMargin, NewTopMargin;
   ThotBool            change;

   /* a priori, les nouvelles marges sont les memes que les anciennes */
   NewTopMargin = TopMargin;
   change = FALSE;
   NewLeftMargin = LeftMargin;

   if (pPageEl == NULL)
      /* pas de page definie, on prend les marges et les dimensions */
      /* par defaut */
     {
	PageFooterHeight = 0;
	/****** NewLeftMargin = PixelValue (DEF_LEFT_MARGIN, UnPoint, NULL, 0);
	NewTopMargin = PixelValue (DEF_TOP_MARGIN, UnPoint, NULL, 0); ******/
	NewLeftMargin = DEF_LEFT_MARGIN;
	NewTopMargin = DEF_TOP_MARGIN;
	PageHeight = DEF_PAGE_HEIGHT;
	/* verifie que le pave racine a une largeur fixe */
	pDim = &rootAbsBox->AbWidth;
	if (pDim->DimIsPosition || pDim->DimValue < 0 ||
	    pDim->DimAbRef != NULL || pDim->DimUnit == UnPercent)
	   /* ce n'est pas une largeur fixe, on impose la largeur par */
	   /* defaut */
	  {
	     pDim->DimIsPosition = FALSE;
	     pDim->DimValue = DEF_PAGE_WIDTH;
	     pDim->DimAbRef = NULL;
	     pDim->DimUnit = UnPoint;
	     rootAbsBox->AbWidthChange = TRUE;
	     change = TRUE;
	  }
     }

   else if (pPageEl->ElPageType == PgBegin && pPageEl->ElParent != NULL)
      /* Nouveau type de page, on determine la hauteur des pages */
     {
	if (CurAssocNum > 0)
	   schView = 1;
	else
	   schView = TheDoc->DocView[CurrentView - 1].DvPSchemaView;
	if (Repaginate)
	  {
	     /* on recupere la boite page sans mettre a jour la hauteur */
	     /* de page (car c'est fait par le paginateur) */
	     /* l'element englobant porte-t-il une regle page ? */
	     pPRule = GetPageRule (pPageEl->ElParent, schView, &pPSchema);
	     if (pPRule != NULL)	/* on a trouve la regle page */
		boxNum = pPRule->PrPresBox[0];
	  }
	else
	   /* appel a PageHeaderFooter qui met a jour les variables PageHeight */
	   /* et PageFooterHeight */
	   PageHeaderFooter (pPageEl, schView, &boxNum, &pPSchema);
	/* mise a jour de NewTopMargin et NewLeftMargin */
	if (boxNum != 0 && pPSchema != NULL)
	  {
	     /* cherche la regle de position verticale */
	     pPRule = GetPRulePage (PtVertPos, boxNum, pPSchema);
	     if (pPRule != NULL)
		/* on a trouve' la regle de position verticale, elle donne */
		/* la nouvelle marge de haut de page */
		NewTopMargin = pPRule->PrPosRule.PoDistance;
	     /* cherche la regle de position horizontale */
	     pPRule = GetPRulePage (PtHorizPos, boxNum, pPSchema);
	     if (pPRule != NULL)
		/* on a trouve' la regle de position horizontale, elle */
		/* donne la nouvelle marge de gauche de la page. */
		NewLeftMargin = pPRule->PrPosRule.PoDistance;
	  }
     }

      /* le cadrage vertical dans la feuille de papier change */
      /* on conserve la nouvelle marge */
      /* Si on est en mode repagination, il faut repositionner le pave racine            en fonction de la marge */
      /* il faut le faire aussi en mode non repagination (cf. UnSetTopMargin) */
        TopMargin = NewTopMargin;
	/* on modifie la position verticale du pave racine */

   if (NewLeftMargin != LeftMargin)
      /* le cadrage horizontal dans la feuille de papier change */
      /* on conserve la nouvelle marge */
     {
	LeftMargin = NewLeftMargin;
	/* on modifie la position horizontale du pave racine */
	pPos = &rootAbsBox->AbHorizPos;
	pPos->PosAbRef = NULL;
	pPos->PosDistance = LeftMargin;
	pPos->PosEdge = Left;
	pPos->PosRefEdge = Left;
	pPos->PosUnit = UnPoint;
	pPos->PosUserSpecified = FALSE;
	rootAbsBox->AbHorizPosChange = TRUE;
	change = TRUE;
     }
   if (change)
      /* on signale le changement de position ou de dimension du pave */
      /* racine au Mediateur */
     {
	/* on ne s'occupe pas des hauteurs de page */
	pageHeight = 0;
	change = ChangeConcreteImage (CurrentFrame, &pageHeight, rootAbsBox);
     }
}

/*----------------------------------------------------------------------
   NextPage    On cherche dans l'image abstraite     		
   le prochain pave d'une marque de page qui ne soit pas un        
   rappel de page. pAb pointe sur le pave d'une marque de page.   	
   Retourne un pointeur sur le pave trouve ou NULL si pas trouve    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static PtrAbstractBox NextPage (PtrAbstractBox pAb)
#else  /* __STDC__ */
static PtrAbstractBox NextPage (pAb)
PtrAbstractBox      pAb;

#endif /* __STDC__ */
{
   /* cherche la derniere feuille dans la marque de page du debut */
   while (pAb->AbFirstEnclosed != NULL)
     {
	pAb = pAb->AbFirstEnclosed;
	while (pAb->AbNext != NULL)
	   pAb = pAb->AbNext;
     }
   /* cherche la marque de page suivante */
   pAb = AbsBoxFromElOrPres (pAb, FALSE, PageBreak + 1, NULL, NULL);
   return (pAb);
}

/*----------------------------------------------------------------------
   PrintView calcule l'image imprimable de la vue traitee.   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         PrintView (PtrDocument pDoc)
#else  /* __STDC__ */
static void         PrintView (pDoc)
PtrDocument         pDoc;
#endif /* __STDC__ */
{
   PtrElement          pEl;
   PtrAbstractBox      rootAbsBox;
   PtrAbstractBox      pAb, pPageAb, pNextPageAb, pHeaderAb;
   DocViewDescr       *pViewD;
   PtrBox              pBox;
   Name                viewName;
   FILE               *PSfile;
   int                 volume, prevVol, h, clipOrg;
   ThotBool            full;

   PageHeight = 0;
   clipOrg = 0;
   /* on ne construit pas les pages (on respecte les marques de page qui existent) */
   pPageAb = NULL;
   pNextPageAb = NULL;
   pHeaderAb = NULL;
   /* cree le debut de l'image du document (le volume libre a ete
      initialise' apres la creation de la fenetre) */
   if (CurAssocNum > 0)
     /* on traite une vue d'elements associes */
     {
       pEl = pDoc->DocAssocRoot[CurAssocNum - 1];
       ustrncpy (viewName, pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrName,
		MAX_NAME_LENGTH);
       rootAbsBox = pDoc->DocAssocRoot[CurAssocNum - 1]->ElAbstractBox[0];
       /* les numeros de pages a imprimer ne sont significatifs que pour la 
	  vue principale de l'arbre principal */
       FirstPrinted = -9999;
       LastPrinted = 9999;
       pAb = AbsBoxesCreate (pEl, pDoc, 1, TRUE, TRUE, &full);
     }
   else
     {
       pEl = pDoc->DocRootElement;
       pViewD = &pDoc->DocView[CurrentView - 1];
       ustrncpy (viewName, pViewD->DvSSchema->SsPSchema->PsView[pViewD->DvPSchemaView - 1], MAX_NAME_LENGTH);
       pDoc->DocViewRootAb[CurrentView - 1] = AbsBoxesCreate (pEl, pDoc,
			 CurrentView, TRUE, TRUE, &full);
       rootAbsBox = pEl->ElAbstractBox[CurrentView - 1];
       /* les numeros de pages a imprimer ne sont significatifs que pour la
	  vue principale de l'arbre principal */
       if (pViewD->DvPSchemaView != 1)
	 {
	   FirstPrinted = -9999;
	   LastPrinted = 9999;
	 }
     }

   /* demande le calcul de l'image */
   h = 0;
   ChangeConcreteImage (CurrentFrame, &h, rootAbsBox);
   volume = rootAbsBox->AbVolume;
   
   /* cherche le premier element page ou la premiere feuille */
   pAb = rootAbsBox;
   while (pAb->AbFirstEnclosed != NULL && pAb->AbElement->ElTypeNumber != PageBreak + 1)
     pAb = pAb->AbFirstEnclosed;
   
   if (pAb->AbElement->ElTypeNumber != PageBreak + 1)
     /* le document ne commence pas par une marque de page, cherche la */
     /* premiere marque de page qui suit */
     pAb = AbsBoxFromElOrPres (pAb, FALSE, PageBreak + 1, NULL, NULL);
   
   if (pAb != NULL)
     if (pAb->AbElement->ElTypeNumber == PageBreak + 1)
       /* on a trouve' une marque de page, on determine la hauteur de ce */
       /* type de page */
       {
	 SetMargins (pAb->AbElement, rootAbsBox);
	 pPageAb = pAb;
       }
   
   if (pPageAb == NULL)
     /* document non pagine */
     {
       /* generation de la bounding box d'un grand graphique */
       if (pNextPageAb == NULL && rootAbsBox->AbBox != NULL)
	 {
	   pBox = rootAbsBox->AbBox;
	   psBoundingBox (CurrentFrame, pBox->BxWidth + pBox->BxXOrg, pBox->BxHeight + pBox->BxYOrg);
	 }
       SetMargins (NULL, NULL);
       /* Document sans marques de pages */
       /* probablement un graphique: il ne faut pas clipper */
#      ifdef _WINDOWS
       if (gbAbort)
	 return;
       if (TtPrinterDC)
	 {
	   if ((StartPage (TtPrinterDC)) <= 0)
	     WinErrorBox (NULL, "PrintView (1)");
	 }
       else
#      endif /* _WINDOWS */
	 PSfile = (FILE *) FrRef[CurrentFrame];
       DisplayFrame (CurrentFrame);
       DrawPage (PSfile);
     }
   else
     {
       /* traite une page apres l'autre */
       do
	 {
	   /* cherche la premiere marque de la page suivante, en ignorant */
	   /* les rappels de page. */
	   pNextPageAb = NextPage (pPageAb);
	   if (pNextPageAb == NULL)
	     /* on n'a pas trouve' de marque de page suivante, on complete */
	     /* l'image abstraite jusqu'a ce qu'elle contienne une marque de */
	     /* page suivante ou qu'on soit arrive' a la fin de la vue ou que */
	     /* le volume de l'image soit trop important */
	     {
	       while (pNextPageAb == NULL && rootAbsBox->AbTruncatedTail &&
		      rootAbsBox->AbVolume < MAX_VOLUME)
		 /* on ajoute au moins 100 caracteres a l'image */
		 {
		   if (volume < 100)
		     volume = 100;
		   /* indique le volume qui peut etre cree */
		   do
		     {
		       if (CurAssocNum > 0)
			 {
			   pDoc->DocAssocFreeVolume[CurAssocNum - 1] = volume;
			   prevVol = rootAbsBox->AbVolume;
			 }
		       else
			 {
			   pDoc->DocViewFreeVolume[CurrentView - 1] = volume;
			   prevVol = rootAbsBox->AbVolume;
			 }
		       /* demande la creation de paves supplementaires */
		       AddAbsBoxes (rootAbsBox, pDoc, FALSE);
		       if (rootAbsBox->AbVolume <= prevVol)
			 /* rien n'a ete cree, augmente le volume de ce qui */
			 /* peut etre cree' */
			 volume = 2 * volume;
		     }
		   while (rootAbsBox->AbVolume <= prevVol && rootAbsBox->AbTruncatedTail);
		   /* on a effectivement cree de nouveaux paves, on cherche */
		   /* si on a cree' une marque de la page suivante */
		   if (pPageAb != NULL)
		     pNextPageAb = NextPage (pPageAb);
		 }

	       /* on demande le calcul de l'image */
	       h = 0;
	       ChangeConcreteImage (CurrentFrame, &h, rootAbsBox);
	     }

	   /* Clean up the top of each page */
	   CleanTopOfPageElement = TRUE;
	   PrintOnePage (pDoc, pPageAb, pNextPageAb, rootAbsBox, clipOrg);
	   /* la nouvelle marque de page devient la page courante */
	   pPageAb = pNextPageAb;
	 }
       while (pPageAb != NULL);
     }
}


/*----------------------------------------------------------------------
   PrintDocument							
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int         PrintDocument (PtrDocument pDoc, int viewsCounter)
#else  /* __STDC__ */
static int         PrintDocument (pDoc, viewsCounter)
PtrDocument        pDoc;
int                viewsCounter;
#endif /* __STDC__ */
{
  PtrSSchema          pSS;
  PtrPSchema          pPSchema;
  DocViewNumber       docView;
  int                 schView, rule, v, assocNum, firstFrame;
  ThotBool            present, found, withPages;
#  ifdef _WINDOWS
  /* static DOCINFO docInfo = {sizeof (DOCINFO), TEXT("Amaya"), NULL}; */
  int    i;
  int    xRes, yRes, xSize, ySize;
  RECT   Rect;
#  endif /* _WINDOWS */

  TopMargin = 0;
  LeftMargin = 0;
  PageHeight = 0;
  PageFooterHeight = 0;
  CurrentView = 0;
  CurAssocNum = 0;
  CurrentFrame = 0;
  firstFrame = 0;
  withPages = FALSE;

#  ifdef _WINDOWS
  if (TtPrinterDC) {
    for (i = 0; i < MAX_COLOR; i++) 
      Pix_Color[i] = RGB (RGB_Table[i].red, RGB_Table[i].green, RGB_Table[i].blue);
    
    xSize = GetDeviceCaps (TtPrinterDC, HORZRES);
    ySize = GetDeviceCaps (TtPrinterDC, VERTRES);
    xRes  = GetDeviceCaps (TtPrinterDC, LOGPIXELSX);
    yRes  = GetDeviceCaps (TtPrinterDC, LOGPIXELSY);
    
    /* Fix bounding rectangle for the picture .. */
    Rect.top    = 0;
    Rect.left   = 0;
    Rect.bottom = ySize;
    Rect.right  = xSize;

    /* ... and inform the driver */
    Escape (TtPrinterDC, SET_BOUNDS, sizeof (RECT), (LPSTR)&Rect, NULL);
    
    if (!InitPrinting (TtPrinterDC, ghwndMain, hCurrentInstance, NULL))
      WinErrorBox (NULL, "PrintDocument (1)");
  }
   #  endif /* _WINDOWS */

  /* imprime l'une apres l'autre les vues a imprimer indiquees dans */
  /* les parametres d'appel du programme print */
  for (v = 0; v < viewsCounter; v++)
    {
      CurrentView = 0;
      CurAssocNum = 0;
      withPages = FALSE;
      /* cherche si la vue est une vue de l'arbre principal */
      found = FALSE;
      pSS = pDoc->DocSSchema;
      /* on examine le schema principal du document et ses extensions */
      do
	{
	  pPSchema = pSS->SsPSchema;
	  for (schView = 0; schView < pPSchema->PsNViews && !found;)
	    if (!ustrcmp (PrintViewName[v], pPSchema->PsView[schView]))
	      found = TRUE;
	    else schView++ ;
	  if (!found)
	    pSS = pSS->SsNextExtens;
	}
      while (pSS != NULL && !found);

      if (found)
	{
	  /* on memorise le fait que c'est une vue avec ou sans pages */
	  withPages = pPSchema->PsPaginatedView[schView];
	  /* c'est une vue de l'arbre principal, on cherche un */
	  /* descripteur de vue libre */
	  for (docView = 0; docView < MAX_VIEW_DOC && CurrentView == 0; docView++)
	    if (pDoc->DocView[docView].DvPSchemaView == 0)
	      /* on prend ce descripteur libre */
	      {
		pDoc->DocView[docView].DvPSchemaView = schView + 1;
		pDoc->DocView[docView].DvSSchema = pSS;
		CurrentView = docView + 1;
	      }
	}
      else
	/* ce n'est pas une vue de l'arbre principal */
	/* cherche si c'est une vue d'elements associes */
	{
	  pSS = pDoc->DocSSchema;
	  /* des elements associes sont utilises pour definir les vues associees */
	  /* cherche dans le schema du document et ses extensions */
	  do
	    {
	      if (pSS->SsFirstDynNature == 0)
		/* rule: derniere regle qui pourrait etre une liste */
		/* d'elem. assoc. */
		rule = pSS->SsNRules;
	      else
		rule = pSS->SsFirstDynNature - 1;
	      /* boucle sur les regles a la recherche de ce nom */
	      do
		if (ustrcmp (pSS->SsRule[rule - 1].SrName, PrintViewName[v]) == 0)
		  found = TRUE;
		else
		  rule--;
	      while (rule > 0 && !found);
	      if (!found)
		pSS = pSS->SsNextExtens;
	    }
	  while (pSS != NULL && !found);

	  if (found)
	    /* c'est une vue d'elements associes */
	    /* cherche si les elements associes existent */
	    {
	      present = FALSE;
	      assocNum = 0;
	      do
		{
		  if (pDoc->DocAssocRoot[assocNum] != NULL)
		    if (pDoc->DocAssocRoot[assocNum]->ElTypeNumber == rule &&
			pDoc->DocAssocRoot[assocNum]->ElStructSchema == pSS)
		      present = TRUE;
		  assocNum++;
		}
	      while (!present && assocNum < MAX_ASSOC_DOC);

	      if (present)
		{
		  CurAssocNum = assocNum;
		  /* les elements associes n'ont qu'une vue, la vue 1 */
		  CurrentView = 1;
		  /* on memorise le fait que c'est une vue avec ou
		     sans pages */
		  withPages = pSS->SsPSchema->PsPaginatedView[CurAssocNum - 1];
		}
	    }
	}

      /* on ne traite pas, pour l'instant, les vues des natures */
      if (CurrentView > 0 || CurAssocNum > 0)
	/* il y a une vue a imprimer */
	/* demande la creation d'une frame pour la vue a traiter */
	{
	  if (CurAssocNum > 0)
            CurrentFrame = OpenPSFile (pDoc, &pDoc->DocAssocVolume[CurAssocNum - 1]);
	  else
	    CurrentFrame = OpenPSFile (pDoc, &pDoc->DocViewVolume[CurrentView - 1]);
	  if (CurrentFrame != 0)
	    {
	      /* creation frame reussie */
	      /* si c'est la premiere frame, on garde son numero */
	      if (firstFrame == 0)
		firstFrame = CurrentFrame;
	      /* initialise la fenetre, et notamment le volume de l'image abstraite a creer et le nombre de pages creees a 0 */
	      if (CurAssocNum > 0)
		{
		  pDoc->DocAssocFrame[CurAssocNum - 1] = CurrentFrame;
		  pDoc->DocAssocFreeVolume[CurAssocNum - 1] = 1000;
		}
	      else
		{
		  pDoc->DocViewFrame[CurrentView - 1] = CurrentFrame;
		  /* DocVueCreee[CurrentView]:= true; */
		  pDoc->DocViewFreeVolume[CurrentView - 1] = 1000;
		}

	      /* page dimensions and margings are unknown, set default values
		 TopMargin = 57;
		 LeftMargin = 57;
	      */
	      TopMargin = 0;
	      LeftMargin = 0;
	      PageHeight = 0;
	      PageFooterHeight = 0;
	      CleanTopOfPageElement = True;
	      if (Repaginate && withPages)
		{
		  /* on pagine le document et on imprime au fur et */
		  /* mesure les pages creees : PaginateView appelle la */
		  /* procedure PrintOnePage definie ci-dessous */
		  if (CurAssocNum > 0)
		    PaginateView (pDoc, CurAssocNum, TRUE);
		  else
		    PaginateView (pDoc, CurrentView, FALSE);
		}
	      else
		/* imprime la vue */
		PrintView (pDoc);
	      /* on ferme la fenetre, sauf si c'est la premiere creee. */
	      if (CurrentFrame != firstFrame)
		ClosePSFile (CurrentFrame);
	      CurrentFrame = 0;
	    }
	}
    }

#  ifdef _WINDOWS
  if (TtPrinterDC) {
    if ((EndDoc (TtPrinterDC)) <= 0)
      WinErrorBox (NULL, "PrintDocument (2)");
    
    DeleteDC (TtPrinterDC);
    TtPrinterDC = NULL;
    
    if (!gbAbort) {
      EnableWindow  (ghwndMain, TRUE);
      DestroyWindow (ghwndAbort);
    }
    return 0;
  } else {
    if (firstFrame != 0) {
      ClosePSFile (firstFrame);
      return (0); /** The .ps file was generated **/
    } else {
      TtaDisplayMessage (FATAL, TtaGetMessage (LIB, TMSG_MISSING_VIEW));
      return (-1); /** The .ps file was not generated for any raison **/
    }
  }
#  else  /* _WINDOWS */
  if (firstFrame != 0)
    {
      ClosePSFile (firstFrame);
      return (0); /** The .ps file was generated **/
    }
  else
    {
      TtaDisplayMessage (FATAL, TtaGetMessage (LIB, TMSG_MISSING_VIEW));
      return (-1); /** The .ps file was not generated for any raison **/
    }
#  endif /* _WINDOWS */
}


/*----------------------------------------------------------------------
  PrintOnePage    imprime l'image de la page de pave pPageAb a    
  pNextPageAb du document pDoc                       
  Si assoc est vrai, c'est la vue d'elements associes de numero view qui
  doit etre traitee.
  L'image a ete calculee avant l'appel (par Page) et il n'y a rien dans
  l'image avant pPageAb sauf dans si pPageAb se trouve dans un element
  BoTable.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                PrintOnePage (PtrDocument pDoc, PtrAbstractBox pPageAb, PtrAbstractBox pNextPageAb, PtrAbstractBox rootAbsBox, int clipOrg)
#else  /* __STDC__ */
void                PrintOnePage (pDoc, pPageAb, pNextPageAb, rootAbsBox, clipOrg)
PtrDocument         pDoc;
PtrAbstractBox      pPageAb;
PtrAbstractBox      pNextPageAb;
PtrAbstractBox      rootAbsBox;
int                 clipOrg;
#endif /* __STDC__ */
{
  ThotBool            stop, emptyImage;
  PtrAbstractBox      pAb, pSpaceAb;
  int                 pageHeight, nextPageBreak, nChars;
  int                 h;
  AbPosition         *pPos;
  FILE               *PSfile;

#ifdef PRINT_DEBUG
FILE     *list;
CHAR_T      localname[50];
static int       n = 1;

   sprintf (localname, "/home/stephane/.amaya/printpave%d.debug", n);
   n++;
   list = fopen (localname, "w");
   NumberAbsBoxes (rootAbsBox);
   ListAbsBoxes(rootAbsBox, 1, list);
   fclose (list);
#endif

# ifdef _WINDOWS
  if (gbAbort)
    return;
  if (TtPrinterDC)
    {
      if ((StartPage (TtPrinterDC)) <= 0)
        WinErrorBox (NULL, "PrintOnePage (1)");
    }
  else
# endif /* _WINDOWS */
    PSfile = (FILE *) FrRef[CurrentFrame];

  if (pPageAb != NULL &&
      pPageAb->AbElement->ElPageNumber >= FirstPrinted &&
      pPageAb->AbElement->ElPageNumber <= LastPrinted)
    {
      if (CleanTopOfPageElement)
	/* cas ou on imprime la premiere page */
	/* il faut rendre le premier filet invisible */
	{
	  if (pPageAb->AbFirstEnclosed != NULL &&
	      !pPageAb->AbFirstEnclosed->AbPresentationBox)
	    /* rend le filet invisible */
	    {
	      pPageAb->AbFirstEnclosed->AbShape = SPACE;
	      pPageAb->AbFirstEnclosed->AbRealShape = SPACE;
	    }
	  KillAbsBoxBeforePage (pPageAb, CurrentFrame, pDoc, CurrentView, &clipOrg);
	  CleanTopOfPageElement = False;
	}

      /* met a jour les marges du haut et du cote gauche (mais pas PageHeight */
      /* car elle a ete calculee par la pagination */
      /* le pave racine est decale en fonction de la valeur des marges */
      SetMargins (pPageAb->AbElement, rootAbsBox);
      /* calcule a priori la position verticale (par rapport au bord */
      /* superieur de la feuille de papier) au-dela de laquelle rien ne */
      /* sera imprime' */
      nextPageBreak = PixelValue (TopMargin, UnPoint, NULL, 0) + PageHeight + PageFooterHeight;
      /* valeur utilisee par SetPageHeight si pas de nouvelle marque page */
      /* (cas de la fin du document) */
      /* pas (encore) de pave espace insere' */
      pSpaceAb = NULL;
      
      if (pNextPageAb != NULL)
	/* il y a une nouvelle marque de page */
	/* cherche le pave du filet saut de page dans la marque de page */
	{
	  pAb = pNextPageAb->AbFirstEnclosed;
	  stop = FALSE;
	  do
	    if (pAb == NULL)
	      stop = TRUE;
	    else if (!pAb->AbPresentationBox)
	      /* Note: le filet n'est pas un pave de presentation, */
	      /* alors que toutes les autres boites de bas de page sont */
	      /* des paves de presentation */
	      {
		stop = TRUE;	/* rend le filet invisible */
		pAb->AbShape = SPACE;
		pAb->AbRealShape = SPACE;
	      }
	    else
	      pAb = pAb->AbNext;
	  while (!stop);

	  if (pAb != NULL)
	    /* demande au Mediateur la position du saut de page */
	    {
	      nextPageBreak = 0;
	      SetPageHeight (pAb, TRUE, &pageHeight, &nextPageBreak, &nChars);
	    }
	}

      /* reste-t-il autre chose a afficher qu'un filet de saut
	 de page a la fin de la vue ? */
      emptyImage = FALSE;
      if (!rootAbsBox->AbTruncatedTail)
	/* c'est la fin de la vue */
	{
	  /* cherche le premier pave feuille de l'image */
	  pAb = AbsBoxFromElOrPres (rootAbsBox, FALSE, PageBreak + 1, NULL, NULL);
	  if (pAb->AbElement->ElTypeNumber == PageBreak + 1)
	    /* le premier pave feuille est une marque de page. Est-il */
	    /* suivi d'un autre pave ? */
	    {
	      while (pAb->AbNext == NULL && pAb->AbEnclosing != NULL)
		pAb = pAb->AbEnclosing;
	      /* l'image est vide si ni la marque de page ni aucun de ses */
	      /* paves englobants n'a de successeur */
	      emptyImage = pAb->AbNext == NULL;
	      /* fait imprimer la page */
	    }
	}

      if (!emptyImage)
	  /* indique au Mediateur la hauteur de la page, pour qu'il */
	  /* n'imprime pas ce qui se trouve au-dela de cette limite */
	  {
	    /* display the content of the page header and delete its contents
	       except the break line */
	    PrintPageHeader (PSfile, CurrentFrame, pPageAb, clipOrg);
	    /* Register the translation between box positions in the Concrete
	       Image and box positions in the paper page and the height of
	       the page body to avoid text overlaping the page footer */
	    if (pNextPageAb)
	      GivePageHeight (CurrentFrame, clipOrg, pNextPageAb->AbBox->BxYOrg - pPageAb->AbBox->BxYOrg - pPageAb->AbBox->BxHeight);
	    else
	      GivePageHeight (CurrentFrame, clipOrg, PageHeight);
	    DisplayFrame (CurrentFrame);
	    if (pNextPageAb)
	      PrintPageFooter (PSfile, CurrentFrame, pNextPageAb);
	    DrawPage (PSfile);
	    /* annule le volume du pave espace insere' en bas de page */
	    StorePageInfo (pPageAb->AbElement->ElPageNumber,
			   pPageAb->AbBox->BxWidth, PageHeight);
	  }

      /* repositionne le pave racine en haut de l'image pour le calcul de la */
      /* page suivante dans le paginateur */
      /* le pave racine avait ete decale par SetMargins */
      pPos = &rootAbsBox->AbVertPos;
      if (pPos->PosAbRef != NULL && pPos->PosDistance != 0 &&
	  pPos->PosEdge != Top && pPos->PosRefEdge != Top)
	{
	  pPos->PosAbRef = NULL;
	  pPos->PosDistance = 0;
	  pPos->PosEdge = Top;
	  pPos->PosRefEdge = Top;
	  pPos->PosUnit = UnPoint;
	  pPos->PosUserSpecified = FALSE;
	  rootAbsBox->AbVertPosChange = TRUE;
	  /* annule le volume du pave espace insere' en bas de page */
	  if (pSpaceAb != NULL)
	    pSpaceAb->AbVolume = 0;
	  /* on ne s'occupe pas des hauteurs de page */
	  h = 0;
	  ChangeConcreteImage (CurrentFrame, &h, rootAbsBox);
	}
    }
}

/*----------------------------------------------------------------------
   ClientSend
   Send a message to the editor.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ClientSend (ThotWindow clientWindow, STRING name, int messageID)
#else  /* __STDC__ */
static void         ClientSend (clientWindow, name, messageID)
ThotWindow          clientWindow;
STRING              name;
int                 messageID;

#endif /* __STDC__ */
{
#ifndef _WINDOWS
   Atom                atom;
   XClientMessageEvent event;

   if (clientWindow == 0)
     fprintf (stderr, TtaGetMessage (LIB, messageID), name);
   else
     {
       event.type = ClientMessage;
       event.display = TtDisplay;
       atom = XInternAtom (TtDisplay, "THOT_MESSAGES", FALSE);
       event.message_type = atom;
       event.send_event = TRUE;
       event.window = clientWindow;
       event.format = 32;
       atom = XInternAtom (TtDisplay, name, FALSE);
       event.data.l[0] = atom;
       event.data.l[1] = messageID;
       XSendEvent (TtDisplay, clientWindow, TRUE, NoEventMask, (ThotEvent *) & event);
       XSync (TtDisplay, FALSE);
     }
#endif /* ! _WINDOWS */
}

/*----------------------------------------------------------------------
   DisplayConfirmMessage
   displays the given message (text).
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DisplayConfirmMessage (STRING text)
#else  /* __STDC__ */
void                DisplayConfirmMessage (text)
STRING              text;

#endif /* __STDC__ */
{
  ClientSend (thotWindow, text, TMSG_LIB_STRING);
}

/*----------------------------------------------------------------------
   DisplayMessage
   displays the given message (text).
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DisplayMessage (STRING text, int msgType)
#else  /* __STDC__ */
void                DisplayMessage (text, msgType)
STRING              text;
int                 msgType;

#endif /* __STDC__ */
{
# ifndef _WINDOWS
  CHAR_T              cmd[800];
# endif /* _WINDOWS */

  if (msgType == FATAL)
    {
      ClientSend (thotWindow, text, TMSG_LIB_STRING);
      /* if the request comes from the Thotlib we have to remove the directory */
      if (removeDirectory)
	{
#     ifdef _WINDOWS
      if ((uunlink (tempDir)) == -1)
         fprintf (stderr, "Cannot remove directory %s\n", tempDir);
#     else  /* !_WINDOWS */
	  sprintf (cmd, "/bin/rm -rf %s\n", tempDir);
	  system (cmd);
#     endif /* _WINDOWS */
	}
      exit (1);
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaError (int errorCode)
#else  /* __STDC__ */
void                TtaError (errorCode)
int                 errorCode;
#endif /* __STDC__ */
{
   UserErrorCode = errorCode;
}


/*----------------------------------------------------------------------
   LoadReferedDocuments    charge tous les documents reference's   
   par le document pointe' par pDoc.			
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         LoadReferedDocuments (PtrDocument pDoc)
#else  /* __STDC__ */
static void         LoadReferedDocuments (pDoc)
PtrDocument         pDoc;
#endif /* __STDC__ */
{
   PtrReferredDescr    pRefD;
   PtrDocument         pDocRef;
   int                 doc;
   ThotBool            found;

   pRefD = pDoc->DocReferredEl;
   if (pRefD != NULL)
      pRefD = pRefD->ReNext;
   /* cherche tous les descripteurs d'elements reference's externes */
   while (pRefD != NULL)
     {
	if (pRefD->ReExternalRef)
	   /* c'est un descripteur d'element reference' externe */
	  {
	     /* le document contenant cet element externe est-il charge'? */
	     found = FALSE;
	     for (doc = 0; doc < MAX_DOCUMENTS && !found; doc++)
		if (LoadedDocument[doc] != NULL)
		   if (SameDocIdent (LoadedDocument[doc]->DocIdent, pRefD->ReExtDocument))
		      found = TRUE;
	     if (!found)
		/* le document reference' n'est pas charge', on le charge */
	       {
		  CreateDocument (&pDocRef);
		  if (pDocRef != NULL)
		    {
		       CopyDocIdent (&pDocRef->DocIdent, pRefD->ReExtDocument);
		       OpenDocument (_EMPTYSTR_, pDocRef, TRUE, FALSE, NULL, FALSE, FALSE);
		    }
	       }
	  }
	pRefD = pRefD->ReNext;
     }
}

		    
/*----------------------------------------------------------------------
   Main program                                                           
  ----------------------------------------------------------------------*/
#ifdef _WINDOWS
#ifdef __STDC__
void PrintDoc (HWND hWnd, int argc, STRING* argv, HDC PrinterDC, BOOL isTrueColors, int depth, STRING tmpDocName, STRING tmpDir, HINSTANCE hInst, BOOL buttonCmd)
#else  /* !__STDC__ */
void PrintDoc (hWnd, argc, argc, PrinterDC, isTrueColors, depth, tmpDocName, tmpDir, hInstance, buttonCmd)
HWND      hWnd;
int       argc;
STRING*    argv;
HDC       PrinterDC;
BOOL      isTrueColors;
int       depth;
STRING     tmpDocName; 
STRING     tmpDir;
HINSTANCE hInst;
BOOL      buttonCmd;
#endif /* __STDC__ */
#else  /* !_WINDOWS */
#ifdef __STDC__
int                 main (int argc, char **argv)
#else  /* __STDC__ */
int                 main (argc, argv)
int                 argc;
char              **argv;

#endif /* __STDC__ */
#endif /* _WINDOWS */
{
  STRING              realName;
  STRING              server = (STRING) NULL;
  STRING              pChar;
  STRING              destination = (STRING) NULL;
  CHAR_T              option [100];
  CHAR_T              name [MAX_PATH];             
  CHAR_T              cmd[800];
  CHAR_T              tempFile [MAX_PATH];
  int                 i, l;
  int                 argCounter;
  int                 viewsCounter = 0;
  int                 cssCounter = 0;
  int                 index;
  int                 length;
  int                 NCopies = 1;
  ThotBool            realNameFound = FALSE;
  ThotBool            viewFound = FALSE;
  ThotBool            done;
# ifndef _WINDOWS
  int                 result;
#endif /* _WINDOWS */

#ifdef _WINDOWS 
  TtPrinterDC      = PrinterDC;
  TtIsTrueColor    = isTrueColors;
  TtWDepth         = depth;
  TtWPrinterDepth  = GetDeviceCaps (TtPrinterDC, PLANES);
  hCurrentInstance = hInst;
  WIN_GetDeviceContext (-1);
  ScreenDPI        = GetDeviceCaps (TtDisplay, LOGPIXELSY);
  if (buttonCmd == FALSE && TtPrinterDC == 0)
     DOT_PER_INCHE = 72;
  else 
      DOT_PER_INCHE    = ScreenDPI;
  WIN_ReleaseDeviceContext ();
  PrinterDPI       = GetDeviceCaps (TtPrinterDC, LOGPIXELSY);
  ghwndMain        = hWnd;
  buttonCommand    = buttonCmd;
#endif /* _WINDOWS */

  thotWindow       = 0;
  removeDirectory = FALSE;
  Repaginate     = 0;
  NPagesPerSheet = 1;
  BlackAndWhite  = 0;
  manualFeed     = 0;
  NoEmpyBox      = 1;
  Repaginate     = 0;
  FirstPrinted   = 0;
  LastPrinted    = 999;
  NCopies        = 1;
  HorizShift     = 0;
  VertShift      = 0;
  Zoom           = 100;
  ustrcpy (pageSize, _A4PaperFormat_);
  Orientation    = TEXT("Portrait");
  PoscriptFont = NULL;
  ColorPs = -1;

  ShowSpace = 1;  /* Restitution des espaces */
  InitLanguage ();
  Dict_Init ();

  /* Initialisation de la gestion memoire */
  InitKernelMemory ();
  InitEditorMemory ();

  if (argc < 4)
    usage (argv[0]);

  TtaInitializeAppRegistry (argv[0]);
  argCounter = 1;

  /* if present the argument -lang should be the first */
  if (!ustrcmp (argv[argCounter], TEXT("-lang")))
    {
      argCounter++;
      TtaSetEnvString ("LANG", argv[argCounter++], TRUE);
    }
  /* Initialise la table des messages d'erreurs */
  TtaGetMessageTable (TEXT("libdialogue"), TMSG_LIB_MSG_MAX);

  while (argCounter < argc)
    {
      /* Parsing the command line */
      if (argv[argCounter][0] == TEXT('-'))
	{
	  /* the argument is a parameter */
	  if (!ustrcmp (argv[argCounter], TEXT("-display")))
	    {
	      /* The display is distant */
	      argCounter++;
	      server = TtaAllocString (ustrlen (argv[argCounter]) + 1);
	      ustrcpy (server, argv[argCounter++]);
	    }
	  else if (!ustrcmp (argv[argCounter], TEXT("-name")))
	    {
	      realNameFound = TRUE;
	      argCounter++;
	      realName = TtaAllocString (ustrlen (argv[argCounter]) + 1);
	      ustrcpy (realName, argv[argCounter++]);
	    }
	  else if (!ustrcmp (argv[argCounter], TEXT("-ps")))
	    {
	      /* The destination is postscript file */
	      destination = TEXT("PSFILE");
	      argCounter++;
	      printer = TtaAllocString (ustrlen (argv[argCounter]) + 1);
	      ustrcpy (printer, argv[argCounter++]);
	    }
	  else if (!ustrcmp (argv[argCounter], TEXT("-out")))
	    {
	      /* The destination is a printer */
	      destination = TEXT("PRINTER");
	      argCounter++;
	      printer = TtaAllocString (ustrlen (argv[argCounter]) + 1);
	      ustrcpy (printer, argv[argCounter++]);
	    }
	  else if (!ustrcmp (argv[argCounter], TEXT("-v")))
	    {
	      /* At least one view must be given in the command line */
	      viewFound = TRUE;
	      argCounter++;
	      ustrcpy (PrintViewName [viewsCounter++], argv[argCounter++]);
	    }
	  else if (!ustrcmp (argv[argCounter], TEXT("-css")))
	    {
                  /* CSS files given in the command line */
                  argCounter++;
                  ustrcpy (CSSName [cssCounter++], argv[argCounter++]);
           }
	  else if (!ustrcmp (argv[argCounter], TEXT("-npps")))
	    {
	      argCounter++;
	      NPagesPerSheet = uctoi (argv[argCounter++]);
	    }
	  else if (!ustrcmp (argv[argCounter], TEXT("-bw")))
	    {
	      argCounter++;
	      BlackAndWhite = 1;
	    }
	  else if (!ustrcmp (argv[argCounter], TEXT("-manualfeed")))
	    {
	      argCounter++;
	      manualFeed = 1;
	    }
	  else if (!ustrcmp (argv[argCounter], TEXT("-emptybox")))
	    {
	      argCounter++;
	      NoEmpyBox = 0;
	    }
	  else if (!ustrcmp (argv[argCounter], TEXT("-paginate")))
	    {
	      argCounter++;
	      Repaginate = 1;
	    }
	  else if (!ustrcmp (argv[argCounter], TEXT("-landscape")))
	    {
	      Orientation = TEXT("Landscape");
	      argCounter++;
	    }
	  else if (!ustrcmp (argv[argCounter], TEXT("-removedir")))
	    {
	      removeDirectory = TRUE;
	      argCounter++;
	    }
	  else if (!ustrcmp (argv[argCounter], TEXT("-portrait")))
	    /* Orientation is already set to Portrait value */ 
	    argCounter++;
	  else if (!ustrcmp (argv[argCounter], TEXT("-sch")))
	    {
	      /* flag for schema directories */
	      argCounter++;
	      ustrcpy (SchemaPath, argv[argCounter++]);
	    }
	  else if (!ustrcmp (argv[argCounter], TEXT("-doc")))
	    {
	      /* flag for document directories */
	      argCounter++;
	      ustrcpy (DocumentDir, argv[argCounter++]);
	    }
	  else
	    {
	      index = 0;
	      pChar = &argv[argCounter][2];
	      while ((option[index++] = *pChar++));
	      option [index] = EOS;
	      switch (argv[argCounter][1])
		{
		case TEXT('F'): FirstPrinted = uctoi (option);
		  argCounter++;
		  break;
		case TEXT('L'): LastPrinted = uctoi (option);
		  argCounter++;
		  break;
		case TEXT('P'): ustrcpy (pageSize, option);
		  argCounter++;
		  break;
		case TEXT('#'): NCopies = uctoi (option);
		  argCounter++;
		  break;
		case TEXT('H'): HorizShift = uctoi (option);
		  argCounter++;
		  break;
		case TEXT('V'): VertShift = uctoi (option);
		  argCounter++;
		  break;
		case TEXT('%'): Zoom = uctoi (option);
		  argCounter++;
		  break;
		case TEXT('w'): thotWindow = (ThotWindow) uctoi (option);
		  argCounter++;
		  break;
		default:
		  ;
                }
	    }
        }
      else
	{
	  /* the argument is the filename */
	  if (TtaFileExist (argv[argCounter]))
	    {
	      /* does it exist ?? */
	      TtaExtractName (argv[argCounter], tempDir, name); /* Yes, it does, split the string into two parts: directory and filename */  
	      argCounter++;
	    }
	  else
	    /* The file does not exist */
	    TtaDisplayMessage (FATAL, TtaGetMessage (LIB, TMSG_LIB_MISSING_FILE), argv[argCounter]);
        }
    }
  
  /* At least one view is mandatory */
# ifndef _WINDOWS 
  if (!viewFound)
    usage (argv[0]);
# endif /* _WINDOWS */
  
  length = ustrlen (name);
  if (!realNameFound)
    {
      realName = TtaAllocString (length + 1);
      ustrcpy (realName, name);
    }

  done   = FALSE;
  index  = 0;

  /* The following loop removes the suffix from the filename (name) */
   while ((index < length) && !done)
     {
       if (name [index] == TEXT('.'))
	 {
	   name [index] = EOS;
	   done = TRUE;
	 }
       else
	 index++;
     }

   FirstFrame (server);
   /* Initialise the color table */
   NColors = MAX_COLOR;
   RGB_Table = RGB_colors;
   Color_Table = Name_colors;

   /* initialise the list of loaded document */
   for (i = 0; i < MAX_DOCUMENTS; i++)
     LoadedDocument[i] = NULL;

   /* Initialise the table of default actions */
   for (i = 0; i < MAX_LOCAL_ACTIONS; i++)
     ThotLocalActions[i] = NULL;

   /* Initialise the list of table editing actions */
   TableHLoadResources (); 
   Table2LoadResources (); 

   /* Initialise Picture Drivers for printing */
   InitPictureHandlers (TRUE);

   /* initialise un contexte de document */
   CreateDocument (&TheDoc);

   /* load the document */
   if (TheDoc != NULL)
     {
       /* add its directory into the DocumentPath */
       l = ustrlen (DocumentDir);
       if (l == 0)
	 ustrcpy (DocumentPath, tempDir);
       else
	 usprintf (DocumentPath, TEXT("%s%c%s"), tempDir, PATH_SEP, DocumentDir);
       
       if (!OpenDocument (name, TheDoc, TRUE, FALSE, NULL, FALSE, FALSE))
	 TheDoc = NULL;
     }
   if (TheDoc != NULL)
     {
       /* the document is loaded */
       /* load CSS files and apply CSS rules */
       for (i = 0; i < cssCounter; i++)
	 LoadStyleSheet (CSSName[i], 1, NULL, NULL);
       
       /* load all referred document before printing */
       LoadReferedDocuments (TheDoc);
       
       if (TypeHasException (ExcNoPaginate, TheDoc->DocSSchema->SsRootElem,
			     TheDoc->DocSSchema))
	 /* Don't paginate a document with the exception NoPaginate */
	 Repaginate = 0;

       /* Start the printing process */
       if (PrintDocument (TheDoc, viewsCounter) == 0)
	 {
	   if (!ustrcmp (destination, TEXT("PSFILE")))
	     {
#ifdef _WINDOWS 
	       usprintf (cmd, TEXT("%s%c%s.ps"), tempDir, DIR_SEP, name);
	       CopyFile (cmd, printer, FALSE);
#else  /* !_WINDOWS */
	       sprintf (cmd, "/bin/mv %s%c%s.ps %s\n", tempDir, DIR_SEP, name, printer);
	       result = system (cmd);
	       if (result != 0)
	         ClientSend (thotWindow, printer, TMSG_CANNOT_CREATE_PS);
	       else
	         ClientSend (thotWindow, realName, TMSG_DOC_PRINTED);
#endif /* _WINDOWS */
	     }
	   else
	     {
#ifndef _WINDOWS
	       if (NCopies > 1)
		 sprintf (cmd, "%s -#%d -T%s %s/%s.ps\n", printer, NCopies, realName, tempDir, name);
	       else
		 sprintf (cmd, "%s %s/%s.ps\n", printer, tempDir, name);
	       
	       result = system (cmd);
	       if (result != 0)
		 ClientSend (thotWindow, cmd, TMSG_UNKNOWN_PRINTER);
	       else
		 ClientSend (thotWindow, realName, TMSG_DOC_PRINTED);
#endif /* _WINDOWS */
	     }
	 }
       else
	 {
           usprintf(tempFile, TEXT("%s/%s.ps"), tempDir, name);
	   ClientSend (thotWindow, tempFile, TMSG_CANNOT_CREATE_PS);
	 }
     }
   
   /* if the request comes from the Thotlib we have to remove the directory */
   if (removeDirectory)
    {
#ifdef _WINDOWS
      int i;
      if (!ustrcmp (destination, TEXT("PSFILE")) && !DeleteFile (cmd))
         WinErrorBox (NULL, "PrintDoc (1)");
      else {
           int cssNDX;
           STRING pivDoc = TtaAllocString (ustrlen (tmpDocName) + ustrlen (tmpDir) + 6);
           usprintf (pivDoc, TEXT("%s\\%s.PIV"), tmpDir, tmpDocName); 
           if (!DeleteFile (pivDoc))
              WinErrorBox (NULL, "PrintDoc (2)");
           for (cssNDX = 0; cssNDX < MAX_CSS; cssNDX++)
               if (CSSName[cssNDX] && CSSName[cssNDX][0] != EOS && TtaFileExist (CSSName[cssNDX]))
                  if (!DeleteFile (CSSName[cssNDX]))
                     WinErrorBox (NULL, "PrintDoc (3)"); 
           if (urmdir (tempDir))
              WinErrorBox (NULL, "PrintDoc (4)");
      }
#else  /* _WINDOWS */
      sprintf (cmd, "/bin/rm -rf %s\n", tempDir);
      system (cmd);
#endif /* _WINDOWS */
    }
   TtaFreeMemory (realName);
#  ifndef _WINDOWS
   exit (0);
#  endif /* !_WINDOWS */
}
