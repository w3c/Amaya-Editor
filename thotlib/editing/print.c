/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * This module saves Thot documents in PostScript
 *
 * Authors: I. Vatton (INRIA)
 *          C. Roisin (INRIA) - Pagination at printing time
 *          R. Guetari (INRIA) - Printing routines for Windows.
 *                               Integration of PostScript prologue.
 * 13/08/02: A small patch which changes the generation of Postscript DSC
 *           page comments to prevent an extraneous one at the end of the document,
 *           which could confuse programs such as gv (by Brian Campbell).
 *
 */
#ifdef _WX
#include "wx/wx.h"
#endif

#include "thot_gui.h"
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
#include "nodialog.h"
#ifdef _WINGUI
#include "resource.h"
#include "wininclude.h"
#endif /* _WINGUI */

#define MAX_VOLUME        10000	/* volume maximum d'une page, en octets */
#define DEF_TOP_MARGIN    57	/* marge de haut de page par defaut, en points */
#define DEF_LEFT_MARGIN   57	/* marge de gauche par defaut, en points */
#define DEF_PAGE_HEIGHT   800	/* hauteur de page par defaut, en points */
#define DEF_PAGE_WIDTH    482	/* largeur de page par defaut, en points */

#define THOT_EXPORT 
#include "dialogapi_tv.h" // just because some modules import this file
#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "boxes_tv.h"
#include "font_tv.h"
#include "platform_tv.h"
#include "thotcolor.h"
#include "thotcolor_tv.h"

#ifdef _WINDOWS
#include "thotprinter_f.h"
#endif /* _WINDOWS */
#include "select_tv.h"
#include "page_tv.h"
#include "modif_tv.h"
#include "edit_tv.h"
#include "creation_tv.h"
#include "frame_tv.h"
#include "platform_tv.h"
#include "appdialogue_tv.h"
#include "units_tv.h"

int          PRINT;	/* Identification des messages */
ThotFont     PostscriptFont;
int          ColorPs;

static PtrDocument  TheDoc;	/* le document en cours de traitement */
static PathBuffer   DocumentDir;   /* le directory d'origine du document */
static int          NumberOfPages;
static char         tempDir [MAX_PATH];
static ThotBool     removeDirectory;

/* table des vues a imprimer */
#define MAX_PRINTED_VIEWS MAX_VIEW_DOC
#define MAX_CSS           10
static Name         PrintViewName[MAX_PRINTED_VIEWS];
static char        *CSSName[MAX_CSS];
static char         CSSOrigin[MAX_CSS];
static int          TopMargin;
static int          LeftMargin;
static ThotBool     CleanTopOfPageElement; /* premiere page imprimee pour le
                                              pagination-impression */
static DocViewNumber CurrentView;	/* numero de la vue traitee */
static int           CurrentFrame;	/* No frame contenant la vue traitee */
static char         *printer;
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
#include "schemas_f.h"
#include "structlist_f.h"
#include "structschema_f.h"
#include "tree_f.h"
#include "glwindowdisplay.h"

static int          manualFeed;
static char         PageSize [3];
static int          BlackAndWhite;
static int          HorizShift;
static int          VertShift;
static int          Zoom;
static const char  *Orientation;
static int          NPagesPerSheet;
static int          NoEmpyBox;
static int          Repaginate;
static int          FirstPrinted;
static int          LastPrinted;
/* TODO : cette ligne provoque une erreur de compilation
 * extern int          errno;*/

#ifdef _WX
#include <locale.h>
#include "AmayaPrintNotify.h"
#include "wxAmayaTimer.h"
/* TODO : rendre le code plus propre car FrRef n'a rien a voir avec un file descriptor !!!
 * pourtant on l'utilise comme tel dans le print :( */
ThotWindow         FrRef[MAX_FRAME + 2];
AmayaPrintNotify * g_p_print_dialog = NULL;
wxAmayaTimer *     g_p_dialog_timer = NULL;
static int         pg_counter = 0;
#endif /* _WX */

#ifdef _GTK
GtkWidget       *window;
GtkWidget       *pbar;
/* Printed page counter  */
static int       pg_counter = 0;
#endif /* _GTK */
/* permits to cancel current printing */
static ThotBool  button_quit = FALSE;
static ThotBool  DoAbort = FALSE;

#ifdef _WINGUI
HBITMAP          WIN_LastBitmap = 0;
static HWND      GHwnAbort;
static HINSTANCE hCurrentInstance;
static int       pg_counter;

/*----------------------------------------------------------------------
  WinErrorBox :  Pops-up a message box when an MS-Window error      
  occured.                                                    
  ----------------------------------------------------------------------*/
void WinErrorBox (HWND hWnd, char *source)
{
  char                str[200];

  sprintf (str, "Error - Source: %s", source);
  MessageBox (hWnd, str, "Amaya", MB_OK);
}

/* ----------------------------------------------------------------------
   ----------------------------------------------------------------------*/
ThotBool WINAPI DllMain (HINSTANCE hInstance, DWORD fdwReason, LPVOID pvReserved) 
{
  return TRUE;
}

/* ---------------------------------------------------------------------- *
 *  FUNCTION:    AbortDlgProc (standard dialog procedure INPUTS/RETURNS)  *
 *  COMMENTS:    Handles "Abort" dialog messages                          *
 * ---------------------------------------------------------------------- */
LRESULT CALLBACK AbortDlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg)
    {
    case WM_INITDIALOG:
      GHwnAbort = hwnd;
      /* show the little icon on the top left corner of the window */
      EnableMenuItem (GetSystemMenu (hwnd, FALSE), SC_CLOSE, MF_GRAYED);
      ShowWindow (hwnd, SW_NORMAL);
      SetFocus (hwnd);
      UpdateWindow (hwnd);
      return TRUE;
      break;

    case WM_COMMAND:
      switch (LOWORD (wParam))
        {
        case IDCANCEL:
          DoAbort = TRUE;
          DestroyWindow (hwnd);
          return TRUE;
        }
      break;
    }
  return FALSE;
}

/* ---------------------------------------------------------------------- *
 *  FUNCTION:    AbortProc                                                *
 *  COMMENTS:    Standard printing abort proc                             *
 * ---------------------------------------------------------------------- */
BOOL CALLBACK AbortProc (HDC hdc, int error)
{
  MSG msg;

  while (!DoAbort && PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
    if ( !IsDialogMessage (GHwnAbort, &msg) )
      {
        TranslateMessage (&msg);
        DispatchMessage (&msg);
      }

  /* the DoAbort function is TRUE (return is FALSE)
     if the user has canceled the print operation */
  return !DoAbort;
}

/* ---------------------------------------------------------------------- *
 *  FUNCTION: InitPrinting(HDC hDC, HWND hWnd, HANDLE hInst, LPSTR msg)   *
 *  PURPOSE : Makes preliminary driver calls to set up print job.         *
 *  RETURNS : TRUE  - if successful.                                      *
 *            FALSE - otherwise.                                          *
 * ---------------------------------------------------------------------- */
BOOL PASCAL InitPrinting(HDC hDC, HWND hWnd, HANDLE hInst, LPSTR msg)
{
  DOCINFO         DocInfo;
  int             xRes, yRes, xSize, ySize;
  RECT            rect;

  DoAbort    = FALSE;     /* user hasn't aborted */
  pg_counter = 0;         /* number of pages we have printed */
  if (!(GHwnAbort = CreateDialog (hInst, "Printinprogress", WIN_Main_Wd,
                                  (DLGPROC) AbortDlgProc)))
    /*MessageBox (WIN_Main_Wd, msg, "InitPrinting", MB_OK)*/;
 
  SetAbortProc (TtPrinterDC, AbortProc);
  memset(&DocInfo, 0, sizeof(DOCINFO));
  DocInfo.cbSize      = sizeof(DOCINFO);
  DocInfo.lpszDocName = (LPTSTR) msg;
  DocInfo.lpszOutput  = NULL;
  xSize = GetDeviceCaps (TtPrinterDC, HORZRES);
  ySize = GetDeviceCaps (TtPrinterDC, VERTRES);
  xRes  = GetDeviceCaps (TtPrinterDC, LOGPIXELSX);
  yRes  = GetDeviceCaps (TtPrinterDC, LOGPIXELSY);
    
  /* Fix bounding rectangle for the picture .. */
  rect.top    = 0;
  rect.left   = 0;
  rect.bottom = ySize;
  rect.right  = xSize;
  /* ... and inform the driver */
  Escape (TtPrinterDC, SET_BOUNDS, sizeof (RECT), (LPSTR)&rect, NULL);    

  if (StartDoc (hDC, &DocInfo) <= 0)
    return FALSE;
  
#ifdef _GLPRINT
  SetupPixelFormatPrintGL (TtPrinterDC, 1);
#endif /* _GLPRINT */
  /* might want to call the abort proc here to allow the user to
   * abort just before printing begins */
  return TRUE;
}

/*----------------------------------------------------------------------
  WIN_GetDeviceContext :  select a Device Context for a given       
  thot window.                                                
  ----------------------------------------------------------------------*/
void WIN_GetDeviceContext (int frame)
{
  TtDisplay = GetDC (NULL);
}

/*----------------------------------------------------------------------
  WIN_ReleaseDeviceContext :  unselect the Device Context           
  ----------------------------------------------------------------------*/
void WIN_ReleaseDeviceContext (void)
{
  /* release the previous Device Context. */
  if (TtDisplay != NULL)
    if (!ReleaseDC (NULL, TtDisplay))
      WinErrorBox (NULL, "PRINT: WIN_ReleaseDeviceContext");
  TtDisplay = NULL;
}
#endif /* _WINGUI */

/*----------------------------------------------------------------------
  psBoundingBox output the %%BoundingBox macro for Postscript.
  ----------------------------------------------------------------------*/
static void psBoundingBox (int frame, int width, int height)
{
  FILE               *fout;

  fout = (FILE *) FrRef[frame];
  /* Since the origin of Postscript coordinates is the lower-left    */
  /* corner of the page, that an A4 page is 2970 mm (i.e 2970*72/254 */
  /* = 841 pts) and that Thot adds a 50 pts margin on top and height */
  /* of the output image, here is the correct values :               */

  fprintf (fout, "%%%%BoundingBox: %d %d %d %d\n",
           50, 791 - height, 50 + width, 791);
}


/*----------------------------------------------------------------------
  PrintPageFooter
  ----------------------------------------------------------------------*/
static void PrintPageFooter (FILE *fout, int frame, PtrAbstractBox pPage,
                             ThotBool last)
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
  /* print all boxes of the current page */
  while (pBox && IsParentBox (pPage->AbBox, pBox))
    {
      if (last && pBox->BxNChars == 1 && pBox->BxBuffer &&
          pBox->BxBuffer->BuContent[0] == '1')
        // avoid to display the page number when only one page is printed
        ;
      else if (pBox->BxYOrg < h)
        DisplayBox (pBox, frame, 0, 32000, 0, h, NULL, FALSE);
      pBox = pBox->BxNext;
    }
}

/*----------------------------------------------------------------------
  PrintPageHeader displays the content of the page header and cleans
  the page element contents except the break line.
  ----------------------------------------------------------------------*/
static void PrintPageHeader (FILE *fout, int frame, PtrAbstractBox pPage, int org)
{
  PtrAbstractBox    pAb;
  PtrBox            pBox;
  ViewFrame        *pFrame;
  int               framexmin, framexmax;
  int               y, h;


  /* Set the top margin to the page header position on the paper */
  FrameTable[frame].FrTopMargin = PixelValue (TopMargin, UnPoint, NULL, 0);
  /* define the clipping to display the page header */
  pFrame = &ViewFrameTable[frame - 1];
  framexmin = pFrame->FrClipXBegin = 0;
  framexmax = pFrame->FrClipXEnd = PixelValue (32000, UnPoint, NULL, 0);
  pFrame->FrClipYBegin = 0;
  /* pFrame->FrYOrg = PixelValue (org, UnPoint, pFrame->FrAbstractBox, 0); */
  pFrame->FrYOrg = org;
  pFrame->FrClipYEnd = PixelValue (32000, UnPoint, NULL, 0);
  y = 0;
  h = 32000;
  /* set the clipping to the frame size before generating postscript */
  DefineClipping (frame, pFrame->FrXOrg, pFrame->FrYOrg, &framexmin,
                  &y, &framexmax, &h, 1);
  /* Look for the first terminal box of the current page */
  pAb = pPage->AbFirstEnclosed;
  while (pAb && pAb->AbFirstEnclosed)
    pAb = pAb->AbFirstEnclosed;
  pBox = pAb->AbBox;
  /* print all boxes of the current page*/
  while (pBox && IsParentBox (pPage->AbBox, pBox))
    {
      DisplayBox (pBox, frame, framexmin, framexmax, y, h, NULL, FALSE);
      pBox = pBox->BxNext;
    }

  /* Set the top margin to the page body position on the paper */
  FrameTable[frame].FrTopMargin += pPage->AbBox->BxHeight;
  /* Remove all boxes within the page element except the line */
  pAb = pPage->AbFirstEnclosed;
  while (pAb)
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
  NotePageNumber produces a Postscript DSC page number comment.
  ----------------------------------------------------------------------*/
static void NotePageNumber (FILE *fout)
{
  NumberOfPages++;
#ifndef _WINDOWS
  fprintf (fout, "%%%%Page: %d %d\n", NumberOfPages, NumberOfPages);
  fflush (fout);
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
  DrawPage
  Store the width and height of the current page, then generate a
  showpage is needed and increment the number of pages.
  ----------------------------------------------------------------------*/
static void DrawPage (FILE *fout, int pagenum, int width, int height)
{
#ifdef _WINDOWS
  if (TtPrinterDC)
    EndPage (TtPrinterDC);
#else /* _WINDOWS */
  fprintf (fout, "%d %d %d nwpage\n", pagenum - 1, width, height);
  fflush (fout);
  /* Enforce loading the font when starting a new page */
  PostscriptFont = NULL;
  ColorPs = -1;
#endif /* _WINDOWS */  
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void usage (char *processName) 
{
#ifndef _WINDOWS
  fprintf (stderr, "\n\nusage: %s [-lang value] <file name>\n", processName);
  fprintf (stderr, "       -sch <schema directories> -doc <doc directories>\n");
  fprintf (stderr, "       -ps <psfile> | -out <printer>\n");
  fprintf (stderr, "       [-v <view name> [...]]\n");
  fprintf (stderr, "       [-portrait | -landscape]\n");
  fprintf (stderr, "       [-display <display>]\n");
  fprintf (stderr, "       [-cssa <author CSS file name> [...]]\n");
  fprintf (stderr, "       [-cssu <user CSS file name>]\n");
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
#ifdef _GTK
  gtk_exit (1);
#endif /* _GTK */	
#endif /* _WINDOWS */  
  exit (1);
}

/*----------------------------------------------------------------------
  GetCurrentSelection                                             
  ----------------------------------------------------------------------*/
ThotBool GetCurrentSelection (PtrDocument *pDoc, PtrElement *firstEl,
                              PtrElement *lastEl, int *firstChar, int *lastChar)
{
  *pDoc = NULL;
  return FALSE;
}


/*----------------------------------------------------------------------
  Si l'entree existe :                                             
  Ferme la fenetre, detruit le fichier et libere l'entree.      
  Libere toutes les boites allouees a la fenetre.                   
  ----------------------------------------------------------------------*/
void DestroyFrame (int frame)
{
  ClearConcreteImage (frame);
  ThotFreeFont (frame);	/* On libere les polices de caracteres utilisees */
}

/*----------------------------------------------------------------------
  FirstFrame cree et initialise la premiere frame.          	
  ----------------------------------------------------------------------*/
static void FirstFrame (char *server)
{
  int                 i;

  /* Initialisation de la table des frames */
  for (i = 0; i <= MAX_FRAME; i++)
    FrRef[i] = 0;
   
  DefaultBColor = 0;
  DefaultFColor = 1;
  InitDocColors ("thot");
#ifdef _GTK
  TtDisplay = XOpenDisplay (server);
#ifndef _GL
  /* initilize the imlib */
  gdk_imlib_init();
  gtk_widget_push_visual(gdk_imlib_get_visual());
  gtk_widget_push_colormap(gdk_imlib_get_colormap());
#endif /* _GL */
#endif /* _GTK */
}

/*----------------------------------------------------------------------
  DefineClipping  limite la zone de reaffichage sur la fenetre frame et
  recalcule ses limites sur l'image concrete.
  Dans le cas du print, c'est exactement la hauteur de page.
  ----------------------------------------------------------------------*/
void DefineClipping (int frame, int orgx, int orgy, int *xd, int *yd,
                     int *xf, int *yf, int raz)
{
  FrameTable[frame].FrHeight = *yf;
}

/*----------------------------------------------------------------------
  RemoveClipping annule le rectangle de clipping de la fenetre frame.  
  ----------------------------------------------------------------------*/
void RemoveClipping (int frame)
{   
  FrameTable[frame].FrWidth = 32000;
  FrameTable[frame].FrHeight = PixelValue (1000, UnPixel, NULL, 0);
}


/*----------------------------------------------------------------------
  GetSizesFrame retourne les dimensions de la fenetre d'indice frame.
  ----------------------------------------------------------------------*/
void GetSizesFrame (int frame, int *width, int *height)
{
  *width = FrameTable[frame].FrWidth;
  *height = FrameTable[frame].FrHeight;
}


/*----------------------------------------------------------------------
  TtaGetThotWindow recupere le numero de la fenetre.              
  ----------------------------------------------------------------------*/
ThotWindow TtaGetThotWindow (int frame)
{
  return FrRef[frame];
}

/*----------------------------------------------------------------------
  GetDocAndView    Retourne le pointeur sur le document (pDoc) et le    
  numero de vue (view) dans ce document, correspondant a    
  frame de numero frame.
  Procedure differente de GetDocAndView de docvues.c          
  ----------------------------------------------------------------------*/
void GetDocAndView (int frame, PtrDocument *pDoc, int *view)
{
  *pDoc = TheDoc;
  *view = CurrentView;
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void InitTable (int i, PtrDocument pDoc)
{
  ViewFrame          *pFrame;
	
	/* initialize visibility and zoom for the window */
  /* cf. procedure InitializeFrameParams */
  pFrame = &ViewFrameTable[i - 1];
  pFrame->FrVisibility = 5;	/* visibilite mise a 5 */
  pFrame->FrMagnification = 0;	/* zoom a 0 */
  
  /* initialize frames tabe because it's used by display functions */
  FrameTable[i].FrDoc = IdentDocument (pDoc);
  FrameTable[i].FrView = i;
  RemoveClipping(i);

#ifdef _WX
  /* initialize a virtual frame because it's tested into display function to know if it's necessary to display */
  FrameTable[i].WdFrame = (AmayaFrame*)1;
#endif /* _WX */
}

/*----------------------------------------------------------------------
  OpenPSFile opens the printing file and write the PS prologue.
  ----------------------------------------------------------------------*/
static int OpenPSFile (PtrDocument pDoc, int *volume)
{
#ifndef _WINDOWS
  FILE               *PSfile;
  char                tmp[MAX_PATH];
  char                fileName[256];
  int                 len;
#endif /* _WINDOWS */
  int                 i;

  /* Est-ce la premiere creation de frame ? */
  i = 1;
  while (FrRef[i] != 0)
    i++;

#ifndef _WINDOWS
  if (i == 1)
    {
      /* On construit le nom du fichier PostScript */
      strcpy (tmp, DocumentPath);
      /* On cherche le directory ou existe le .PIV */
      MakeCompleteName (pDoc->DocDName, "PIV", tmp, fileName, &len);
      /* generate the full name directory/name.ps */
      FindCompleteName (pDoc->DocDName, "ps", tmp, fileName, &len);
      if ((PSfile = TtaWriteOpen (fileName)) == NULL)
        TtaDisplayMessage (FATAL, TtaGetMessage (LIB, TMSG_CANNOT_CREATE_PS), fileName);
      else
        {
          fflush (PSfile);
          FrRef[i] = (ThotWindow) PSfile;
          fprintf (PSfile, "%%!PS-Adobe-3.0\n");
          fprintf (PSfile, "%%%%Creator: Thotlib\n");
          fprintf (PSfile, "%%%% Delete the last nwpage line command for an encapsulated PostScript\n");
          fprintf (PSfile, "%%%%Pages: (atend)\n");
          fprintf (PSfile, "%%%%EndComments\n\n");
	  
          fprintf (PSfile, "%%%%BeginProlog\n");
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
          fprintf (PSfile, "/setstyle %% style setstyle\n");
          fprintf (PSfile, "{ dup 4 le\n");
          fprintf (PSfile, "  { dup 3 eq\n");
          fprintf (PSfile, "    {pop [3] 0 setdash}\n");
          fprintf (PSfile, "    {pop [6 3] 0 setdash}\n");
          fprintf (PSfile, "    ifelse}\n");
          fprintf (PSfile, "  {pop [ ] 0 setdash}\n");
          fprintf (PSfile, "  ifelse\n");
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
          fprintf (PSfile, "   5 setstyle\n");
          fprintf (PSfile, "   0.75 setlinewidth stroke grestore\n");
          fprintf (PSfile, " } bind def\n");
          fprintf (PSfile, "/Seg	%% (x y)* dash width nb Segments\n");
          fprintf (PSfile, "{ [/nb /ep /style] {exch def} forall\n");
          fprintf (PSfile, "   newpath\n");
          fprintf (PSfile, "   moveto\n");
          fprintf (PSfile, "   2 1 nb {pop lineto} for\n");
          fprintf (PSfile, "   ep 0 ne {0 setlinejoin 0 setlinecap\n");
          fprintf (PSfile, "   ep 1 gt {ep 0.8 mul} {ep 0.4 mul} ifelse\n");
          fprintf (PSfile, "   setlinewidth \n");
          fprintf (PSfile, "   style setstyle\n");
          fprintf (PSfile, "   stroke} if\n");
          fprintf (PSfile, "} bind def\n");
          fprintf (PSfile, "/Poly      %% rgb fill (x y)* dash width nb Polygone\n");
          fprintf (PSfile, "{ [/nb /ep /style] {exch def} forall\n");
          fprintf (PSfile, "   newpath\n");
          fprintf (PSfile, "   moveto\n");
          fprintf (PSfile, "   2 1 nb {pop lineto} for\n");
          fprintf (PSfile, "   closepath\n");
          fprintf (PSfile, "   fillbox\n");
          fprintf (PSfile, "   ep 0 ne {0 setlinejoin 0 setlinecap\n");
          fprintf (PSfile, "   ep 1 gt {ep 0.8 mul} {ep 0.4 mul} ifelse\n");
          fprintf (PSfile, "   setlinewidth \n");
          fprintf (PSfile, "   style setstyle \n");
          fprintf (PSfile, "   stroke} if\n");
          fprintf (PSfile, "} bind def\n");
          fprintf (PSfile, "/Curv	%% (x y)* dash width nb Curves\n");
          fprintf (PSfile, "{ [/nb /ep /style] {exch def} forall\n");
          fprintf (PSfile, "   newpath\n");
          fprintf (PSfile, "   moveto\n");
          fprintf (PSfile, "   2 1 nb {pop curveto} for\n");
          fprintf (PSfile, "   0 setlinejoin 0 setlinecap\n");
          fprintf (PSfile, "   ep 1 gt {ep 0.8 mul} {ep 0.4 mul} ifelse\n");
          fprintf (PSfile, "   setlinewidth \n");
          fprintf (PSfile, "   style setstyle\n");
          fprintf (PSfile, "   ep 0 gt {stroke} if\n");
          fprintf (PSfile, "} bind def\n");
          fprintf (PSfile, "/Splin	%% rgb fill (x y)* dash width nb Spline\n");
          fprintf (PSfile, "{ [/nb /ep /style] {exch def} forall\n");
          fprintf (PSfile, "   newpath\n");
          fprintf (PSfile, "   moveto\n");
          fprintf (PSfile, "   2 1 nb {pop curveto} for\n");
          fprintf (PSfile, "   closepath\n");
          fprintf (PSfile, "   fillbox\n");
          fprintf (PSfile, "   0 setlinejoin 0 setlinecap\n");
          fprintf (PSfile, "   ep 1 gt {ep 0.8 mul} {ep 0.4 mul} ifelse\n");
          fprintf (PSfile, "   setlinewidth \n");
          fprintf (PSfile, "   style setstyle\n");
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
          fprintf (PSfile, "   thick 1 gt {thick 0.8 mul} {thick 0.4 mul} ifelse\n");
          fprintf (PSfile, "   setlinewidth \n");
          fprintf (PSfile, "   savem setmatrix\n");
          fprintf (PSfile, "   dash 0 setstyle\n");
          fprintf (PSfile, "   thick 0 gt {stroke} if\n");
          fprintf (PSfile, "   end\n");
          fprintf (PSfile, " } bind def\n");
          fprintf (PSfile, "/oval	%% oval fill style ep x1 y1 x2 y2 rx ry\n");
          fprintf (PSfile, "{\n");
          fprintf (PSfile, "  10 dict begin\n");
          fprintf (PSfile, "  /ry exch def\n");
          fprintf (PSfile, "  /rx exch def\n");
          fprintf (PSfile, "  ry rx div /scalef exch def\n");
          fprintf (PSfile, "  ry add scalef div /y2 exch def\n");
          fprintf (PSfile, "  rx sub /x2 exch def\n");
          fprintf (PSfile, "  ry sub scalef div /y1 exch def\n");
          fprintf (PSfile, "  rx add /x1 exch def\n");
          fprintf (PSfile, "  /ep exch def /dash exch def /fil exch def\n");
          fprintf (PSfile, "  gsave 1 scalef scale\n");
          fprintf (PSfile, "  x2 y1 moveto\n");
          fprintf (PSfile, "  currentpoint newpath rx 0 90 arc\n");
          fprintf (PSfile, "  x1 y1 rx 90 180 arc\n");
          fprintf (PSfile, "  x1 y2 rx 180 270 arc\n");
          fprintf (PSfile, "  x2 y2 rx 270 360 arc closepath\n");
          fprintf (PSfile, "  fil fillbox\n");
          fprintf (PSfile, "  ep 1 gt {ep 0.8 mul} {ep 0.4 mul} ifelse\n");
          fprintf (PSfile, "  setlinewidth\n");
          fprintf (PSfile, "  dash setstyle\n");
          fprintf (PSfile, "  ep 0 gt {stroke} if\n");
          fprintf (PSfile, "  grestore\n");
          fprintf (PSfile, "  end\n");
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
          fprintf (PSfile, "  ep 1 gt {ep 0.8 mul} {ep 0.4 mul} ifelse\n");
          fprintf (PSfile, "  setlinewidth \n");
          fprintf (PSfile, "  dash setstyle\n");
          fprintf (PSfile, "  ep 0 gt {stroke} if\n");
          fprintf (PSfile, "  end\n");
          fprintf (PSfile, " } bind def\n");
          fprintf (PSfile, "/cer  %% Cercle ep x y R\n");
          fprintf (PSfile, "{\n");
          fprintf (PSfile, "  2 index 2 index moveto \n");
          fprintf (PSfile, "  newpath 0 360 arc\n");
          fprintf (PSfile, "  /ep exch def /dash exch def\n");
          fprintf (PSfile, "  fillbox\n");
          fprintf (PSfile, "  ep 1 gt {ep 0.8 mul} {ep 0.4 mul} ifelse\n");
          fprintf (PSfile, "  setlinewidth \n");
          fprintf (PSfile, "  dash setstyle\n");
          fprintf (PSfile, "  ep 0 gt {stroke} if\n");
          fprintf (PSfile, "} bind def\n\n");

          fprintf (PSfile, "/ellipse { %% style ep x y A B\n");
          fprintf (PSfile, "  1 index div /scalef exch def /bigaxis exch def\n");
          fprintf (PSfile, "  moveto\n");
          fprintf (PSfile, "  gsave\n");
          fprintf (PSfile, "  1 scalef scale\n");
          fprintf (PSfile, "  currentpoint newpath bigaxis 0 360 arc closepath\n");
          fprintf (PSfile, "  /ep exch def /dash exch def\n");
          fprintf (PSfile, "  fillbox\n");
          fprintf (PSfile, "  ep 1 gt {ep 0.8 mul} {ep 0.4 mul} ifelse\n");
          fprintf (PSfile, "  setlinewidth \n");
          fprintf (PSfile, "  dash setstyle\n");
          fprintf (PSfile, "  ep 0 gt {stroke} if\n");
          fprintf (PSfile, "  grestore\n");
          fprintf (PSfile, "} bind def\n\n");

#ifdef _GL
          fprintf (PSfile,
                   /*   "/GL 64 dict def GL begin\n" */
                   "1 setlinecap 1 setlinejoin\n"
                   "/BD { bind def } bind def\n"
                   "/C  { setrgbcolor } BD\n"
                   "/G  { setgray } BD\n"
                   "/W  { setlinewidth } BD\n"
                   "/FC { findfont exch scalefont setfont } BD\n"
                   "/S  { FC moveto show } BD\n"
                   "/P  { newpath 0.0 360.0 arc closepath fill } BD\n"
                   "/L  { newpath moveto lineto stroke } BD\n"
                   "/SL { C moveto C lineto stroke } BD\n"
                   "/T  { newpath moveto lineto lineto closepath fill } BD\n");

          /* Flat-shaded triangle with middle color:
             x3 y3 r3 g3 b3 x2 y2 r2 g2 b2 x1 y1 r1 g1 b1 Tm 
             This is the postscript function that will reproduce the opengl rendering,
             triangle by triangle
          */
          fprintf (PSfile,
                   /* stack : x3 y3 r3 g3 b3 x2 y2 r2 g2 b2 x1 y1 r1 g1 b1 */
                   "/Tm { 3 -1 roll 8 -1 roll 13 -1 roll add add 3 div\n" /* r = (r1+r2+r3)/3 */
                   /* stack : x3 y3 g3 b3 x2 y2 g2 b2 x1 y1 g1 b1 r */
                   "      3 -1 roll 7 -1 roll 11 -1 roll add add 3 div\n" /* g = (g1+g2+g3)/3 */
                   /* stack : x3 y3 b3 x2 y2 b2 x1 y1 b1 r g b */
                   "      3 -1 roll 6 -1 roll 9 -1 roll add add 3 div" /* b = (b1+b2+b3)/3 */
                   /* stack : x3 y3 x2 y2 x1 y1 r g b */
                   " C T } BD\n");
#endif /* _GL */

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
          fprintf (PSfile, "} bind def\n");
	  
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
          fprintf (PSfile, "/t/u/v/w/x/y/z/braceleft/bar/braceright/asciitilde/.notdef/epsilon\n");
          fprintf (PSfile, "/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef\n");
          fprintf (PSfile, "/.notdef/oe/OE/.notdef/.notdef/.notdef/.notdef/dotlessi/grave\n");
          fprintf (PSfile, "/acute/circumflex/tilde/macron/breve/dotaccent/dieresis/.notdef/ring\n");
          fprintf (PSfile, "/cedilla/.notdef/hungarumlaut/ogonek/caron/space/exclamdown/cent\n");
          fprintf (PSfile, "/sterling/currency/yen/brokenbar/section/dieresis/copyright/ordfeminine\n");
          fprintf (PSfile, "/guillemotleft/logicalnot/hyphen/registered/macron/degree/plusminus\n");
          fprintf (PSfile, "/twosuperior/threesuperior/acute/mu/paragraph/periodcentered/cedilla\n");
          fprintf (PSfile, "/onesuperior/ordmasculine/guillemotright/onequarter/onehalf/threequarters\n");
          fprintf (PSfile, "/questiondown/Agrave/Aacute/Acircumflex/Atilde/Adieresis/Aring/AE\n");
          fprintf (PSfile, "/Ccedilla/Egrave/Eacute/Ecircumflex/Edieresis/Igrave/Iacute/Icircumflex\n");
          fprintf (PSfile, "/Idieresis/Eth/Ntilde/Ograve/Oacute/Ocircumflex/Otilde/Odieresis\n");
          fprintf (PSfile, "/multiply/Oslash/Ugrave/Uacute/Ucircumflex/Udieresis/Yacute/Thorn\n");
          fprintf (PSfile, "/germandbls/agrave/aacute/acircumflex/atilde/adieresis/aring/ae\n");
          fprintf (PSfile, "/ccedilla/egrave/eacute/ecircumflex/edieresis/igrave/iacute/icircumflex\n");
          fprintf (PSfile, "/idieresis/eth/ntilde/ograve/oacute/ocircumflex/otilde/odieresis/divide\n");
          fprintf (PSfile, "/oslash/ugrave/uacute/ucircumflex/udieresis/yacute/thorn/ydieresis]\n");
          fprintf (PSfile, "def %%ISOLatin1Encoding\n");
          fprintf (PSfile, "} ifelse\n\n");

          fprintf (PSfile, "/ReEncode	%% NewFont Font ReEncode\n");
          fprintf (PSfile, "  { findfont	%% load desired font\n");
          fprintf (PSfile, "    dup length dict /newfont exch def	%% allocate new fontdict\n");
          fprintf (PSfile, "    dup\n");
          fprintf (PSfile, "    { exch dup dup dup dup /FID ne exch /Encoding ne and exch /FontBBox ne and exch /FontMatrix ne and\n");
          fprintf (PSfile, "	{ exch newfont 3 1 roll put }\n");
          fprintf (PSfile, "	{ pop pop }\n");
          fprintf (PSfile, "	ifelse }\n");
          fprintf (PSfile, "    forall	%% copy fontdict\n\n");
	  
          fprintf (PSfile, "    dup /FontBBox get 4 array copy newfont /FontBBox 3 -1 roll put\n");
          fprintf (PSfile, "    /FontMatrix get 6 array copy newfont /FontMatrix 3 -1 roll put\n\n");
	  
          fprintf (PSfile, "    newfont /Encoding ISOLatin1Encoding 256 array copy put	%%put new encoding vector\n");
	  
          fprintf (PSfile, "    dup newfont /FontName 3 -1 roll put	%%put a new Fontname\n");
          fprintf (PSfile, "    newfont definefont pop	%%define a new font\n");
          fprintf (PSfile, "  } def\n\n");
	  
          fprintf (PSfile, "/ReEncodeOblique	%% NewFont Font ReEncodeOblique\n");
          fprintf (PSfile, "  { findfont	%% load desired font\n");
          fprintf (PSfile, "    dup length dict /newfont exch def	%% allocate new fontdict\n");
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

          fprintf (PSfile, "/nwpage		%% new page\n");
          fprintf (PSfile, " { /pagecounter pagecounter 1 add def\n");
          fprintf (PSfile, "   pagesizeok { pagenumberok { showpage }\n");
          fprintf (PSfile, "			         {erasepage } ifelse } \n");
          fprintf (PSfile, "	          { erasepage } ifelse\n");
          fprintf (PSfile, "   grestore\n");
          fprintf (PSfile, "   VectMatrice pagecounter 4 mod get setmatrix \n");
          fprintf (PSfile, "   gsave UserMatrice concat } def\n\n");
	  
          fprintf (PSfile, "/ppf 1 def\n");
          fprintf (PSfile, "/showpage { pagecounter ppf mod 0 eq {systemdict /showpage get exec} if } bind def\n");
          fprintf (PSfile, "/ejectpage { pagecounter ppf mod 0 ne {systemdict /showpage get exec} if } bind def\n\n");
	  
          fprintf (PSfile, "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n\n");

          fprintf (PSfile, "/MatriceDict 9 dict def\n");
          fprintf (PSfile, "MatriceDict begin\n\n");
          fprintf (PSfile, " /MatPortrait matrix def\n");
          fprintf (PSfile, " /MatLandscape [0 1 -1 0 0 0] def\n\n");
          fprintf (PSfile, " /defmat { \n");
          fprintf (PSfile, "    matrix concatmatrix matrix concatmatrix matrix currentmatrix matrix concatmatrix } bind def\n\n");

          fprintf (PSfile, "    /defdict_portrait {	%% dict defdict\n");
          fprintf (PSfile, "    /Portrait 3 dict def \n");
          fprintf (PSfile, "	Portrait begin\n");
          fprintf (PSfile, "	  /1ppf 2 dict def 1ppf begin\n");
          fprintf (PSfile, "	        /VectMatrice [\n");
          fprintf (PSfile, "	            MatPortrait MatScale1 [1 0 0 1 0 PHeight] defmat\n");
          fprintf (PSfile, "	        dup dup dup ] def\n");
          fprintf (PSfile, "	        /ppf 1 def\n");
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

          fprintf (PSfile, "%%%%EndProlog\n");
          fprintf (PSfile, "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%DEMARRAGE%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n\n");

	  
          fprintf (PSfile, "/ThotDict 100 dict def\n");
          fprintf (PSfile, "ThotDict begin\n\n");
          fprintf (PSfile, "statusdict begin\n");

          if (manualFeed == 0)
            {
              if (!strcmp (PageSize, "A3"))
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
          fprintf (PSfile, "/page_size (%s) def\n", PageSize);
          fprintf (PSfile, "/orientation (%s) def\n", Orientation);
          fprintf (PSfile, "/nb_ppf (%dppf) def\n", NPagesPerSheet);
          fprintf (PSfile, "/suptrame %d def\n", NoEmpyBox);
          fprintf (PSfile, "/evenodd 0 def\n");
          fprintf (PSfile, "/HPrinterOff 0 def\n");
          fprintf (PSfile, "/VPrinterOff 0 def\n");
          fprintf (PSfile, "/user_orientation 0 def  \n");
          fprintf (PSfile, "/UserMatrice \n");
          fprintf (PSfile, "[reduction 100 div 0 0 2 index 0 0] \n");
          fprintf (PSfile, "[1 0 0 1 decalageH HPrinterOff add 72 mul 25.4 div decalageV VPrinterOff add 72 mul 25.4 div neg] matrix concatmatrix\n");
          fprintf (PSfile, "[1 0 0 1 0 0]\n");
          fprintf (PSfile, "matrix concatmatrix def\n");
          fprintf (PSfile, "MatriceDict begin\n");
          fprintf (PSfile, "page_size cvlit load	 		%% get the page_size dict\n");
          fprintf (PSfile, "begin defdict_portrait defdict_paysage	%% and init it\n");
          fprintf (PSfile, "currentdict orientation cvlit get		%% get the orientation dict\n");
          fprintf (PSfile, "nb_ppf cvlit get				%% get the nb_ppf dict\n");
          fprintf (PSfile, " begin \n");
          fprintf (PSfile, "  gsave VectMatrice 0 get setmatrix		%% init CTM\n");
          fprintf (PSfile, "  gsave UserMatrice concat\n");
          fprintf (PSfile, "  %%100 dict begin				%% working dict\n");


          NumberOfPages = 0;
          fflush (PSfile);
        }
    }
  else
    {
      PSfile = (FILE *) FrRef[1];
      FrRef[i] = (ThotWindow) PSfile;
    }
#endif /* _WINDOWS */

  InitTable (i, pDoc);
  *volume = 16000;
  return (i);
}

/*----------------------------------------------------------------------
  ClosePSFile ferme le fichier PostScript.                        
  ----------------------------------------------------------------------*/
static void ClosePSFile (int frame)
{
  FILE               *PSfile;

  PSfile = (FILE *) FrRef[frame];
  /* Est-ce la fenetre principale ? */
  if (frame == 1 && PSfile)
    {
      /* Oui -> on ferme le fichier */
      fprintf (PSfile, "grestore\n");
      fprintf (PSfile, "end end end %%close all open dict\n");
      fprintf (PSfile, "%%%%Trailer\n");
      fprintf (PSfile, "%%%%Pages: %d\n%%%%EOF\n", NumberOfPages);
      TtaReadClose (PSfile);
    }
  FrRef[frame] = 0;
  /* Libere toutes les boites allouees */
  ClearConcreteImage (frame);
  /* On libere les polices de caracteres utilisees */
  ThotFreeFont (frame);
}


/*----------------------------------------------------------------------
  ClipOnPage defines the drawing limits.
  ----------------------------------------------------------------------*/
static void ClipOnPage (int frame, int org, int width, int height)
{
  int                 y, h, framexmin, framexmax;
  ViewFrame          *pFrame;

  if (height != 0)
    {
      pFrame = &ViewFrameTable[frame - 1];
      /* On convertit suivant l'unite donnee */
      pFrame->FrClipXBegin = 0;
      pFrame->FrClipXEnd = width;
      pFrame->FrClipYBegin = org;
      pFrame->FrYOrg = org;
      pFrame->FrClipYEnd = org + height;
      y = org;
      h = height;
      /* set the clipping to the frame size before generating postscript */
      DefineClipping (frame, pFrame->FrXOrg, pFrame->FrYOrg, &framexmin,
                      &y, &framexmax, &h, 1);
    }
}

/*----------------------------------------------------------------------
  SetMargins met a jour PageHeight et PageFooterHeight ainsi que  
  les marges du haut et du cote gauche, selon le type de page        
  auquel appartient l'element Marque Page pointe par pPageEl.        
  Cette procedure utilise PageHeaderFooter (commune a print et page)         
  ----------------------------------------------------------------------*/
static void SetMargins (PtrElement pPageEl, PtrDocument pDoc,
                        PtrAbstractBox rootAbsBox)
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
  boxNum = 0;
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
      if (rootAbsBox)
        {
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
    }

  else if (pPageEl->ElPageType == PgBegin && pPageEl->ElParent != NULL)
    /* Nouveau type de page, on determine la hauteur des pages */
    {
      schView = TheDoc->DocView[CurrentView - 1].DvPSchemaView;
      if (Repaginate)
        {
          /* on recupere la boite page sans mettre a jour la hauteur */
          /* de page (car c'est fait par le paginateur) */
          /* l'element englobant porte-t-il une regle page ? */
          pPRule = GetPageRule (pPageEl->ElParent, pDoc, schView, &pPSchema);
          if (pPRule != NULL)	/* on a trouve la regle page */
            boxNum = pPRule->PrPresBox[0];
        }
      else
        /* appel a PageHeaderFooter qui met a jour les variables PageHeight*/
        /* et PageFooterHeight */
        PageHeaderFooter (pPageEl, TheDoc, schView, &boxNum, &pPSchema);
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
      if (rootAbsBox)
        {
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
static PtrAbstractBox NextPage (PtrAbstractBox pAb)
{
  /* cherche la derniere feuille dans la marque de page du debut */
  while (pAb->AbFirstEnclosed != NULL)
    {
      pAb = pAb->AbFirstEnclosed;
      while (pAb->AbNext != NULL)
        pAb = pAb->AbNext;
    }
  /* cherche la marque de page suivante */
  pAb = AbsBoxFromElOrPres (pAb, FALSE, FALSE, PageBreak + 1, NULL, NULL);
  return (pAb);
}

/*----------------------------------------------------------------------
  PrintView calcule l'image imprimable de la vue traitee.   
  ----------------------------------------------------------------------*/
static void PrintView (PtrDocument pDoc)
{
  PtrElement          pEl;
  PtrPSchema          pPS;
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
  pBox = NULL;
  /* cree le debut de l'image du document (le volume libre a ete
     initialise' apres la creation de la fenetre) */
  pEl = pDoc->DocDocElement;
  pViewD = &pDoc->DocView[CurrentView - 1];
  pPS = PresentationSchema (pViewD->DvSSchema, pDoc);
  strncpy (viewName, pPS->PsView[pViewD->DvPSchemaView-1],
           MAX_NAME_LENGTH);
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
    pAb = AbsBoxFromElOrPres (pAb, FALSE, FALSE, PageBreak + 1, NULL, NULL);
   
  if (pAb != NULL)
    if (pAb->AbElement->ElTypeNumber == PageBreak + 1)
      /* on a trouve' une marque de page, on determine la hauteur de ce */
      /* type de page */
      {
        SetMargins (pAb->AbElement, pDoc, rootAbsBox);
        pPageAb = pAb;
      }
   
  if (pPageAb == NULL)
    /* document non pagine */
    {
      /* generation de la bounding box d'un grand graphique */
      if (pNextPageAb == NULL && rootAbsBox->AbBox != NULL)
        {
          pBox = rootAbsBox->AbBox;
          psBoundingBox (CurrentFrame, pBox->BxWidth + pBox->BxXOrg,
                         pBox->BxHeight + pBox->BxYOrg);
        }
      SetMargins (NULL, NULL, NULL);
      /* Document sans marques de pages */
      /* probablement un graphique: il ne faut pas clipper */
      /* control the Abort printing button */
      if (DoAbort)
        return;
#ifdef _WINGUI
      if (TtPrinterDC)
        {
          if ((StartPage (TtPrinterDC)) <= 0)
            WinErrorBox (NULL, "PrintView (1)");
        }
      else
#endif /* _WINGUI */
        PSfile = (FILE *) FrRef[CurrentFrame];
      NotePageNumber (PSfile);
      DisplayFrame (CurrentFrame);
      DrawPage (PSfile, 1,
                pBox->BxWidth, PageHeight);
    }
  else
    {
      /* traite une page apres l'autre */
      do
        {
#ifdef _WINGUI
          /* control the Abort printing button */
          AbortProc (TtPrinterDC, 0);
#endif /* _WINGUI */
          if (DoAbort)
            return;

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
                      pDoc->DocViewFreeVolume[CurrentView - 1] = volume;
                      prevVol = rootAbsBox->AbVolume;
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
          if (PrintOnePage (pDoc, pPageAb, pNextPageAb, rootAbsBox, clipOrg, FALSE))
            /* la nouvelle marque de page devient la page courante */
            pPageAb = pNextPageAb;
          else
            pPageAb = NULL;
        }
      while (pPageAb != NULL);
    }
}


/*----------------------------------------------------------------------
  PrintDocument							
  ----------------------------------------------------------------------*/
static int PrintDocument (PtrDocument pDoc, int viewsCounter)
{
  PtrSSchema          pSS;
  PtrPSchema          pPSchema;
  DocViewNumber       docView;
  int                 schView, v, firstFrame;
  ThotBool            found, withPages;

  TopMargin = 0;
  LeftMargin = 0;
  PageHeight = 0;
  PageFooterHeight = 0;
  CurrentView = 0;
  CurrentFrame = 0;
  firstFrame = 0;
  withPages = FALSE;

#ifdef _WINGUI
  if (TtPrinterDC)
    InitPrinting (TtPrinterDC, WIN_Main_Wd, hCurrentInstance, "Doc");
#endif /* _WINGUI */

  /* imprime l'une apres l'autre les vues a imprimer indiquees dans */
  /* les parametres d'appel du programme print */
  for (v = 0; v < viewsCounter; v++)
    {
#if defined(_GTK) || defined(_WX)
      if (DoAbort)
        return 1;
#endif /* _GTK || _WX */
      CurrentView = 0;
      withPages = FALSE;
      /* cherche si la vue est une vue de l'arbre principal */
      found = FALSE;
      pSS = pDoc->DocSSchema;
      /* on examine le schema principal du document et ses extensions */
      do
        {
          pPSchema = PresentationSchema (pSS, pDoc);
          for (schView = 0; schView < pPSchema->PsNViews && !found;)
            if (!strcmp (PrintViewName[v], pPSchema->PsView[schView]))
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
          for (docView = 0; docView < MAX_VIEW_DOC && CurrentView == 0;
               docView++)
            if (pDoc->DocView[docView].DvPSchemaView == 0)
              /* on prend ce descripteur libre */
              {
                pDoc->DocView[docView].DvSSchema = pSS;
                pDoc->DocView[docView].DvPSchemaView = schView + 1;
                pDoc->DocView[docView].DvSync = FALSE;
                pDoc->DocView[docView].DvFirstGuestView = NULL;
                /* create the guest view list for that view */
                CreateGuestViewList (pDoc, docView+1);
                CurrentView = docView + 1;
              }
        }

      /* on ne traite pas, pour l'instant, les vues des natures */
      if (CurrentView > 0)
        /* il y a une vue a imprimer */
        /* demande la creation d'une frame pour la vue a traiter */
        {
          CurrentFrame = OpenPSFile (pDoc, &pDoc->DocViewVolume[CurrentView - 1]);
          if (CurrentFrame != 0)
            {
              /* creation frame reussie */
              /* si c'est la premiere frame, on garde son numero */
              if (firstFrame == 0)
                firstFrame = CurrentFrame;
              /* initialise la fenetre, et notamment le volume de l'image abstraite a creer et le nombre de pages creees a 0 */
              pDoc->DocViewFrame[CurrentView - 1] = CurrentFrame;
              /* DocVueCreee[CurrentView]:= true; */
              pDoc->DocViewFreeVolume[CurrentView - 1] = 1000;

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
                  PaginateView (pDoc, CurrentView);
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

#ifdef _WINGUI
  if (TtPrinterDC)
    {
      if (DoAbort)
        AbortDoc (TtPrinterDC);
      else 
        {
          /* remove the Abort window */
          DestroyWindow (GHwnAbort);
          /* end the document */
          if ((EndDoc (TtPrinterDC)) <= 0)
            WinErrorBox (NULL, "PrintDocument (2)");    
        }
      GHwnAbort = NULL;
      return 0;
    }
#endif /* _WINGUI */ 
#ifndef _WINDOWS
  if (firstFrame != 0)
    {
      ClosePSFile (firstFrame);
      return (0); /** The .ps file was generated **/
    }
  else
    return (-1); /** The .ps file was not generated for any raison **/
#endif /* _WINDOWS */
  return 0;
}

#ifdef _WX
/*----------------------------------------------------------------------
  MyPrintDlgTimerProcWX : Call when the timer occurs
  ----------------------------------------------------------------------*/
static void MyPrintDlgTimerProcWX( void * data)
{
  delete g_p_dialog_timer;
  g_p_dialog_timer = NULL;
}

/*----------------------------------------------------------------------
  Callback that cancel printing
  ----------------------------------------------------------------------*/
void wx_print_cancel()
{
  if (button_quit)
    {
      wxExit();
    }
  else
    DoAbort = TRUE;
}

/*----------------------------------------------------------------------
  Create and display dialog box that permits cancel and viewing page number
  ----------------------------------------------------------------------*/
void wx_print_dialog ()
{
  g_p_print_dialog = new AmayaPrintNotify();
  g_p_print_dialog->Show();
}
#endif /* _WX */

/*----------------------------------------------------------------------
  PrintOnePage    imprime l'image de la page de pave pPageAb a    
  pNextPageAb du document pDoc                       
  L'image a ete calculee avant l'appel (par Page) et il n'y a rien dans
  l'image avant pPageAb sauf dans si pPageAb se trouve dans un element
  BoTable.
  ----------------------------------------------------------------------*/
ThotBool PrintOnePage (PtrDocument pDoc, PtrAbstractBox pPageAb,
                       PtrAbstractBox pNextPageAb, PtrAbstractBox rootAbsBox,
                       int clipOrg, ThotBool last)
{
  PtrAbstractBox      pAb, pSpaceAb;
  PtrBox              box;
  AbPosition         *pPos;
  FILE               *PSfile;
  int                 pageHeight, nextPageBreak, nChars;
  int                 h;
  ThotBool            stop, emptyImage;
#if defined(_GTK) || defined(_WX)
  char                title_label[50];
#endif /* _GTK || _WX */

  /* control the Abort printing button */
  if (DoAbort)
    return (FALSE);

#ifdef _GTK
  pg_counter++;
  sprintf(title_label, "%s [ %i ]", TtaGetMessage(LIB, TMSG_LIB_PRINT), pg_counter);
  gtk_window_set_title (GTK_WINDOW (window),title_label);			 
  while (gtk_events_pending())
    gtk_main_iteration();
#endif /* _GTK */

#ifdef _WX
  pg_counter++;
  sprintf(title_label, "%s [ %i ]", TtaGetMessage(LIB, TMSG_LIB_PRINTING_IN_PROGRESS), pg_counter);
  g_p_print_dialog->SetMessage( title_label );
  
  g_p_dialog_timer = new wxAmayaTimer( MyPrintDlgTimerProcWX, NULL );
  /* start a one shot timer */
  g_p_dialog_timer->Start( 500, wxTIMER_ONE_SHOT );
  while (g_p_dialog_timer)
    wxSafeYield( g_p_print_dialog );
#endif /* _WX */

#ifdef _WINGUI
  if (TtPrinterDC)
    {
      if ((StartPage (TtPrinterDC)) <= 0)
        WinErrorBox (NULL, "PrintOnePage (1)");
      /* control the Abort printing button */
      AbortProc (TtPrinterDC, 0);
      if (DoAbort)
        return (FALSE);
      /* update and display the number of pages we have printed */
      {
        char print_msg [20];
        pg_counter++;
        sprintf (print_msg, "Page: %d", pg_counter);
        SetWindowText (GetDlgItem (GHwnAbort, IDC_PAGENO), print_msg);
      }
    }
  else
#endif /* _WINGUI */
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
      SetMargins (pPageAb->AbElement, pDoc, rootAbsBox);
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
              SetPageHeight (pAb, &pageHeight, &nextPageBreak, &nChars);
            }
        }

      /* reste-t-il autre chose a afficher qu'un filet de saut
         de page a la fin de la vue ? */
      emptyImage = FALSE;
      if (!rootAbsBox->AbTruncatedTail)
        /* c'est la fin de la vue */
        {
          /* cherche le premier pave feuille de l'image */
          pAb = AbsBoxFromElOrPres (rootAbsBox, FALSE, FALSE, PageBreak + 1,
				    NULL, NULL);
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
          NotePageNumber (PSfile);
          /* display the content of the page header and delete its contents
             except the break line */
          PrintPageHeader (PSfile, CurrentFrame, pPageAb, clipOrg);
          /* Register the translation between box positions in the Concrete
             Image and box positions in the paper page and the height of
             the page body to avoid text overlaping the page footer */
          if (pNextPageAb)
            {
              box = pNextPageAb->AbBox;
              ClipOnPage (CurrentFrame, clipOrg,
                          FrameTable[CurrentFrame].FrWidth,
                          box->BxYOrg - pPageAb->AbBox->BxYOrg - pPageAb->AbBox->BxHeight);
            }
          else
            ClipOnPage (CurrentFrame, clipOrg, 32000, PageHeight);
          DisplayFrame (CurrentFrame);
          if (pNextPageAb)
            PrintPageFooter (PSfile, CurrentFrame, pNextPageAb, last);
          DrawPage (PSfile, pPageAb->AbElement->ElPageNumber,
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
  return (TRUE);
}


/*----------------------------------------------------------------------
  DisplayConfirmMessage
  displays the given message (text).
  ----------------------------------------------------------------------*/
void DisplayConfirmMessage (const char *text)
{
#ifdef _GTK
  gtk_window_set_title (GTK_WINDOW (window),text); 
  button_quit = TRUE;
#endif /* _GTK */

#ifdef _WX
  g_p_print_dialog->SetMessage( text );
  button_quit = TRUE;
#endif /* _WX */
}

/*----------------------------------------------------------------------
  DisplayMessage
  displays the given message (text).
  ----------------------------------------------------------------------*/
void DisplayMessage (char *text, int msgType)
{
  if (msgType == FATAL)
    {
#ifdef _GTK
      gtk_window_set_title (GTK_WINDOW (window),text);
      button_quit = TRUE;
#endif /* _GTK */
#ifdef _WX
      g_p_print_dialog->SetMessage( text );
      button_quit = TRUE;
#endif /* _WX */
      printf ("Remove directory \n");
      /* if the request comes from the Thotlib we have to remove the directory */
      if (removeDirectory)
        {
          if ((unlink (tempDir)) == -1)
            fprintf (stderr, "Cannot remove directory %s\n", tempDir);
        }
#ifdef _GTK 
      gtk_main_iteration_do (TRUE);
#endif /* _GTK */
#if _WINGUI
      exit (1);
#endif /* _WINGUI */
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void TtaError (int errorCode)
{
  UserErrorCode = errorCode;
}


#ifdef _GTK
/*----------------------------------------------------------------------
  Callback that cancel printing
  ----------------------------------------------------------------------*/
void set_cancel (GtkWidget *widget, void *nothing)
{
  if (button_quit)
    {
      gtk_main_quit();
      gtk_exit (1);
    }
  else
    DoAbort = TRUE;
}

/*----------------------------------------------------------------------
  Create and display dialog box that permits cancel and viewing page number
  ----------------------------------------------------------------------*/
void gtk_print_dialog ()
{
  GtkWidget *button;
  GtkWidget *vbox;
  char title_label[50];
  
  /* create a new window */
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_policy (GTK_WINDOW (window), TRUE, TRUE, TRUE);
  gtk_window_set_default_size(GTK_WINDOW (window), 200, 60);

  gtk_container_set_border_width (GTK_CONTAINER (window), 0);
  sprintf(title_label, "%s [    ]", TtaGetMessage(LIB, TMSG_LIB_PRINT)); 
  gtk_window_set_title (GTK_WINDOW (window), title_label);

  vbox = gtk_vbox_new (TRUE, 5);
  gtk_container_set_border_width (GTK_CONTAINER (vbox), 10);
  gtk_container_add (GTK_CONTAINER (window), vbox);
  gtk_widget_show(vbox);

  /* Creates a new button with the label "Printing". */
  button = gtk_button_new_with_label (TtaGetMessage(LIB, TMSG_CANCEL));
  /* This will cause the window to be destroyed by calling
     gtk_widget_destroy(window) when "clicked".  The destroy
     signal could come from here, or the window manager. */
  gtk_signal_connect_object (GTK_OBJECT (button), 
                             "clicked",
                             GTK_SIGNAL_FUNC (set_cancel), 
                             NULL);
  /* This makes it so the button is the default. */
  GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
  gtk_container_add (GTK_CONTAINER (vbox), button);
  gtk_widget_show_now (button);
  gtk_widget_show(vbox);
  /* and the window */
  gtk_widget_show (window);
  /* Force display */
  while (gtk_events_pending ())
    gtk_main_iteration ();
}
#endif /* _GTK */	    

#ifdef _WINGUI_WX
#include "commdlg.h"
static PRINTDLG     Pdlg;
static ThotWindow   PrintForm = NULL;
static ThotBool     LpInitialized = FALSE;

/*----------------------------------------------------------------------
  GetPrinterDC()
  Call the Windows print dialogue and returns TRUE if the printer is
  available. Reuses the previous defined printer when the parameter 
  reuse is TRUE.
  Returns the orientation (portrait, landscape), and the paper
  format (A4, US). 
  ----------------------------------------------------------------------*/
ThotBool GetPrinterDC (ThotBool reuse)
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
      Pdlg.hInstance   = (HINSTANCE) NULL;
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
          TtPrinterDC = CreateDC ((LPCSTR)lpDriverName, (LPCSTR)lpDeviceName, (LPCSTR)lpPortName, lpDevMode);
          if (lpDevMode->dmOrientation == DMORIENT_LANDSCAPE)
            /* landscape */
            Orientation = "Landscape";
          else
            /* portrait */
            Orientation = "Portrait";
          if (lpDevMode->dmPaperSize == DMPAPER_A4)
            /* A4 */
            strcpy (PageSize, "A4");

          else
            /* US */
            strcpy (PageSize, "US");

          GlobalUnlock (Pdlg.hDevMode);
          return TRUE;
        }
    }
  Pdlg.hwndOwner   = FrRef[0];
  if (PrintDlg (&Pdlg))
    {
      if (Pdlg.hDevMode)
        {
          lpDevMode = (LPDEVMODE) GlobalLock (Pdlg.hDevMode);
          if (!lpDevMode)
            return FALSE;
          if (lpDevMode->dmOrientation == DMORIENT_LANDSCAPE)
            /* landscape */
            Orientation = "Landscape";
          else
            /* portrait */
            Orientation = "Portrait";

          if (lpDevMode->dmPaperSize == DMPAPER_A4)
            /* A4 */
            strcpy (PageSize, "A4");

          else
            /* US */
            strcpy (PageSize, "US");

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
#endif /* _WINGUI_WX */

#ifdef _GLPRINT
/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
int makeArgcArgv (HINSTANCE hInst, char*** pArgv, char* cmdLine)
{ 
  int          argc;
  static char *argv[40];
  static char  argv0[1024];
  static char  commandLine [1024];
  char        *ptr;
  char         lookFor = 0;
  enum {
    nowAt_start, 
    nowAt_text
  } nowAt;

  strcpy (commandLine, cmdLine);
  ptr = commandLine;
  *pArgv = argv;
  argc = 0;
  GetModuleFileName (hInst, (LPTSTR)argv0, sizeof (argv0));
  argv[argc++] = argv0;
  for (nowAt = nowAt_start;;)
    {
      if (!*ptr) 
        return (argc);
      if (lookFor)
        {
          if (*ptr == lookFor)
            {
              nowAt = nowAt_start;
              lookFor = 0;
              *ptr = 0;   /* remove the quote */
            }
          else if (nowAt == nowAt_start)
            {
              argv[argc++] = ptr;
              nowAt = nowAt_text;
            }
          ptr++;
          continue;
        }
      if (*ptr == SPACE || *ptr == TAB)
        {
          *ptr = 0;
          ptr++;
          nowAt = nowAt_start;
          continue;
        }
      if ((*ptr == '\'' || *ptr == '\"' || *ptr == '`') && nowAt == nowAt_start)
        {
          lookFor = *ptr;
          nowAt = nowAt_start;
          ptr++;
          continue;
        }
      if (nowAt == nowAt_start)
        {
          argv[argc++] = ptr;
          nowAt = nowAt_text;
        }
      ptr++;
    }
}
#endif /*_GLPRINT*/

#ifdef _WINDOWS
/*----------------------------------------------------------------------
  DeleteAFile                                                         
  ----------------------------------------------------------------------*/
static void DeleteAFile (char *fileName)
{
  if (TtaFileExist (fileName))
#ifndef _WX
    DeleteFile (fileName);
#else /* _WX */
  return;/* TODO */
#endif /* _WX */
}
#endif /* _WINDOWS */

/*----------------------------------------------------------------------
  Main program                                                           
  ----------------------------------------------------------------------*/
#ifdef _WINDOWS
#ifdef _WX
/* TODO: integrer ceci avec l appel de la dll dans printmenu.c */
void PrintDoc (HWND hWnd, int argc, char **argv, HDC PrinterDC,
               ThotBool isTrueColors, int depth, char *tmpDocName,
               char *tmpDir, HINSTANCE hInst, ThotBool specificPrinter)
#else /* _WX */
     DLLEXPORT void PrintDoc (HWND hWnd, int argc, char **argv, HDC PrinterDC,

                              ThotBool isTrueColors, int depth, char *tmpDocName,

                              char *tmpDir, HINSTANCE hInst, ThotBool specificPrinter)

#endif /* _WX */
#else  /* _WINDOWS */
#ifdef _WX
     int amaya_main (int argc, char **argv)
#else /* _WX */
     int main (int argc, char **argv)
#endif /* _WX */
#endif /* _WINDOWS */
{
#ifdef _WINDOWS
  char             *fileName;
#endif /* _WINDOWS */
  char             *realName = NULL, *server = NULL, *pChar = NULL, *ptr;
  const char       *destination = NULL;
  char              option [100];
  char              name [MAX_PATH];             
  char              cmd[800];
  int               i, l;
  int               argCounter;
  int               viewsCounter = 0;
  int               cssCounter = 0;
  int               index;
  int               length;
  int               NCopies;
  ThotBool          realNameFound = FALSE;
  ThotBool          viewFound = FALSE;
  ThotBool          done;

#if defined(_WX)
  /* In order to get a "." even in a localised unix (ie: french becomes ",") */
  setlocale (LC_NUMERIC, "C");
#endif /* _WX */
  ShowReturn = 0;
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
  strcpy (PageSize, "A4");
  Orientation    = "Portrait";
  PostscriptFont = NULL;
  ColorPs = -1;
  Printing = TRUE;
  InitLanguage ();

  /* Initialisation de la gestion memoire */
  InitKernelMemory ();
  InitEditorMemory ();
  ShowOnePage = TRUE;

  if (argc < 4)
    usage (argv[0]);
  TtaInitializeAppRegistry (argv[0]);
  argCounter = 1;

  /* if present the argument -lang should be the first */
  if (!strcmp (argv[argCounter], "-lang"))
    {
      argCounter++;
      TtaSetEnvString ("LANG", argv[argCounter++], TRUE);
    }
  /* Initialise la table des messages d'erreurs */
  TtaGetMessageTable ("libdialogue", TMSG_LIB_MSG_MAX);
#ifndef _WINDOWS
  if (!strcmp (argv[argCounter], "--help"))
    usage (argv[0]);
#endif /* _WINDOWS */

#ifdef _WX
  /* create the print dialog befor the opengl initialisation because the virtual gl canvas is a child of this dialog */
  wx_print_dialog ();
#endif /* _WX */
#ifdef _GL
  /* init an offscreen rendering context 
     in order to use OpenGL drawing results 
     as a base for printing. */
  GetGLContext ();
#endif /* _GL */
#ifdef _WINGUI_WX
  if (!GetPrinterDC (TRUE))
    return;
#endif /* _WINGUI_WX */

  while (argCounter < argc)
    {
      /* Parsing the command line */
      if (argv[argCounter][0] == '-')
        {
          /* the argument is a parameter */
          if (!strcmp (argv[argCounter], "-display"))
            {
              /* The display is distant */
              argCounter++;
              server = (char *)TtaGetMemory (strlen (argv[argCounter]) + 1);
              strcpy (server, argv[argCounter++]);
            }
          else if (!strcmp (argv[argCounter], "-name"))
            {
              realNameFound = TRUE;
              argCounter++;
              realName = (char *)TtaGetMemory (strlen (argv[argCounter]) + 1);
              strcpy (realName, argv[argCounter++]);
            }
          else if (!strcmp (argv[argCounter], "-ps"))
            {
              /* The destination is postscript file */
              destination = "PSFILE";
              argCounter++;
              printer = (char *)TtaGetMemory (strlen (argv[argCounter]) + 1);
              strcpy (printer, argv[argCounter++]);
            }
          else if (!strcmp (argv[argCounter], "-out"))
            {
              /* The destination is a printer */
              destination = "PRINTER";
              argCounter++;
              l = strlen (argv[argCounter]);
              if (argv[argCounter][0] == '"' &&
                  argv[argCounter][l - 1] != '"')
                {
                  /* the printer name includes spaces */
                  i = 0;
                  length = -1; /* skip the first '"' */
                  /* get the printer name length */
                  while (argv[argCounter + i][l - 1] != '"')
                    {
                      i++;
                      length = length + l + 1; /* +1 for the space */
                      l = strlen (argv[argCounter + i]);
                    }
                  /* store the printer name */
                  length += l;
                  printer = (char *)TtaGetMemory (length + 1);
                  strcpy (printer, &argv[argCounter++][1]);
                  for (l = 1; l <= i; l++)
                    {
                      strcat (printer, " ");
                      strcat (printer, argv[argCounter++]);
                    }
                  /* remove the last '"' */
                  printer[length-1] = EOS;
                }
              else
                {
                  printer = (char *)TtaGetMemory (l + 1);
                  strcpy (printer, argv[argCounter++]);
                }
            }
          else if (!strcmp (argv[argCounter], "-v"))
            {
              /* At least one view must be given in the command line */
              viewFound = TRUE;
              argCounter++;
              strcpy (PrintViewName [viewsCounter++], argv[argCounter++]);
            }
          else if (!strcmp (argv[argCounter], "-cssa"))
            {
              /* CSS files given in the command line */
              argCounter++;
              CSSOrigin[cssCounter] = 'a';
              /* suppress quotes if necessary */
              l = strlen (argv[argCounter]);
              if (argv[argCounter][l - 1] == '"')
                argv[argCounter][l - 1] = EOS;
              if (argv[argCounter][0] == '"')
                CSSName[cssCounter] = TtaStrdup (&(argv[argCounter][1]));
              else
                CSSName[cssCounter] = TtaStrdup (argv[argCounter]);
              argCounter++;
              cssCounter++;
            }
          else if (!strcmp (argv[argCounter], "-cssu"))
            {
              /* CSS files given in the command line */
              argCounter++;
              CSSOrigin[cssCounter] = 'u';
              /* suppress quotes if necessary */
              l = strlen (argv[argCounter]);
              if (argv[argCounter][l - 1] == '"')
                argv[argCounter][l - 1] = EOS;
              if (argv[argCounter][0] == '"')
                CSSName[cssCounter] = TtaStrdup (&(argv[argCounter][1]));
              else
                CSSName[cssCounter] = TtaStrdup (argv[argCounter]);
              argCounter++;
              cssCounter++;
            }
          else if (!strcmp (argv[argCounter], "-npps"))
            {
              argCounter++;
              NPagesPerSheet = atoi (argv[argCounter++]);
            }
          else if (!strcmp (argv[argCounter], "-bw"))
            {
              argCounter++;
              BlackAndWhite = 1;
            }
          else if (!strcmp (argv[argCounter], "-manualfeed"))
            {
              argCounter++;
              manualFeed = 1;
            }
          else if (!strcmp (argv[argCounter], "-emptybox"))
            {
              argCounter++;
              NoEmpyBox = 0;
            }
          else if (!strcmp (argv[argCounter], "-paginate"))
            {
              argCounter++;
              Repaginate = 1;
            }
          else if (!strcmp (argv[argCounter], "-landscape"))
            {
#ifndef _WINGUI_WX
              Orientation = "Landscape";
#endif /* _WINGUI_WX */
              argCounter++;
            }
          else if (!strcmp (argv[argCounter], "-removedir"))
            {
              removeDirectory = TRUE;
              argCounter++;
            }
          else if (!strcmp (argv[argCounter], "-portrait"))
            /* Orientation is already set to Portrait value */ 
            argCounter++;
          else if (!strcmp (argv[argCounter], "-sch"))
            {
              /* flag for schema directories */
              argCounter++;
              strcpy (SchemaPath, argv[argCounter++]);
            }
          else if (!strcmp (argv[argCounter], "-doc"))
            {
              /* flag for document directories */
              argCounter++;
              strcpy (DocumentDir, argv[argCounter++]);
            }
          else
            {
              index = 0;
              pChar = &argv[argCounter][2];
              while ((option[index++] = *pChar++));
              option [index] = EOS;
#ifndef _WINGUI_WX
              switch (argv[argCounter][1])
                {
                case 'F': FirstPrinted = atoi (option);
                  break;
                case 'L': LastPrinted = atoi (option);
                  break;
                case 'P': strcpy (PageSize, option);
                  break;
                case '#': NCopies = atoi (option);
                  break;
                case 'H': HorizShift = atoi (option);
                  break;
                case 'V': VertShift = atoi (option);
                  break;
                case '%': Zoom = atoi (option);
                  break;
                case 'w': thotWindow = (ThotWindow) atoi (option);
                  break;
                default:
                  ;
                }
#endif /* _WINGUI_WX */
              argCounter++;
            }
        }
      else
        {
          /* the argument is the filename */
              /* suppress quotes if necessary */
              l = strlen (argv[argCounter]);
              if (argv[argCounter][l - 1] == '"')
                argv[argCounter][l - 1] = EOS;
              if (argv[argCounter][0] == '"')
                ptr = &(argv[argCounter][1]);
              else
                ptr = argv[argCounter];
          /* does it exist ?? */
          if (TtaFileExist (ptr))
            /* Yes, it does, split the string into two parts: directory
               and filename */
            TtaExtractName (ptr, tempDir, name);
          argCounter++;
        }
    }
  
  /* At least one view is mandatory */
  if (!viewFound)
    strcpy (PrintViewName[0], "Formatted_view");
  
  length = strlen (name);
  if (!realNameFound)
    {
      realName = (char *)TtaGetMemory (length + 1);
      strcpy (realName, name);
    }

  done   = FALSE;
  index  = 0;

  /* The following loop removes the suffix from the filename (name) */
  while ((index < length) && !done)
    {
      if (name[index] == '.')
        {
          name[index] = EOS;
          done = TRUE;
        }
      else
        index++;
    }

  FirstFrame (server);
#ifdef _WINDOWS
#ifndef _WX
  if (PrinterDC)
    TtPrinterDC = PrinterDC;

  TtIsTrueColor = isTrueColors;
  TtWDepth = depth;
  TtWPrinterDepth = GetDeviceCaps (TtPrinterDC, PLANES);
  hCurrentInstance = hInst;
  TtDisplay = GetDC (NULL);
  ScreenDPI = 80 /*GetDeviceCaps (TtDisplay, LOGPIXELSY)*/;
  DOT_PER_INCH = ScreenDPI;
  ReleaseDC (NULL, TtDisplay);
  TtDisplay = NULL;
  PrinterDPI = GetDeviceCaps (TtPrinterDC, LOGPIXELSY);
  if (!specificPrinter && TtPrinterDC == NULL)
    DOT_PER_INCH = 72;
  else 
    DOT_PER_INCH = PrinterDPI;
  WIN_Main_Wd = hWnd;
#else /* _WX */
  /* TODO: ecrire le code d impression avec WX */
#endif /* _WX */
#else /* _WINDOWS */
#ifndef _WX
  DOT_PER_INCH = 90;
#else /* _WX */
  DOT_PER_INCH = 72;
#endif /* _WX */
#endif /* _WINDOWS */
  
  /* Initialisation des polices de caracteres */
  InitDialogueFonts ("thot");
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

  /* Initialise Picture Drivers for printing */
  InitPictureHandlers (TRUE);

  /* initialise un contexte de document */
  i = 0;
  CreateDocument (&TheDoc, &i);

  /* load the document */
  if (TheDoc != NULL)
    {
      /* add its directory into the DocumentPath */
      l = strlen (DocumentDir);
      if (l == 0)
        strcpy (DocumentPath, tempDir);
      else
        sprintf (DocumentPath, "%s%c%s", tempDir, PATH_SEP, DocumentDir);
      
      if (!OpenDocument (name, TheDoc, FALSE))
        TheDoc = NULL;
    }
  if (TheDoc != NULL)
    {
      /* the document is loaded */
      /* load CSS files and apply CSS rules */
      for (i = 0; i < cssCounter; i++)
        LoadStyleSheet (CSSName[i], 1, NULL, NULL, NULL, CSS_ALL,
                        CSSOrigin[i] == 'u');
      
      if (TypeHasException (ExcNoPaginate, TheDoc->DocSSchema->SsRootElem,
                            TheDoc->DocSSchema))
        /* Don't paginate a document with the exception NoPaginate */
        Repaginate = 0;
      
      /* Start the printing process */
      if (PrintDocument (TheDoc, viewsCounter) == 0)
        {
          if (!strcmp (destination, "PSFILE"))
            {
              sprintf (cmd, "%s%c%s.ps", tempDir, DIR_SEP, name);
#ifdef _WINDOWS
              CopyFile (cmd, printer, FALSE);
#else  /* _WINDOWS */
	      TtaFileCopy (cmd, printer);
	      TtaFileUnlink (cmd);
#endif  /* _WINDOWS */         
            }
#ifndef _WINDOWS
          else
            {
              sprintf (cmd, "%s \"%s/%s.ps\"", printer, tempDir, name);
              system (cmd);
            }
#endif  /* _WINDOWS */    
        }
    }
   
  /* if the request comes from the Thotlib we have to remove the directory */
  if (removeDirectory)
    {
#ifdef _WINDOWS
      if (!strcmp (destination, "PSFILE"))
        DeleteAFile (cmd);
      else
        {
          sprintf (name, "%s\\%s.PIV", tmpDir, tmpDocName); 
          DeleteAFile (name);
          if (tmpDir)
            {
              length = strlen (tmpDir);
              /* remove CSS files from the temporary directory */
              for (i = 0; i < cssCounter; i++)
                {
                  if (CSSName[i] && TtaFileExist (CSSName[i]) &&
                      strncasecmp(CSSName[i], tmpDir, length) == 0)
                    DeleteAFile (CSSName[i]);
                  TtaFreeMemory (CSSName[i]);
                  CSSName[i] = NULL;
                }
              /* remove the structure and presentation schemas from the
                 temporary directory */
              for (i = 0; i < TheDoc->DocNNatures; i++)
                {
                  if (TheDoc->DocNatureName[i])
                    {
                      fileName = (char *)TtaGetMemory (length +
                                                       strlen(TheDoc->DocNatureName[i]) + 6);
                      sprintf (fileName, "%s\\%s.STR", tmpDir, TheDoc->DocNatureName[i]);
                      if (TtaFileExist (fileName))
                        DeleteAFile (fileName);
                      TtaFreeMemory (fileName);
                    } 
                  if (TheDoc->DocNaturePresName[i])
                    {
                      fileName = (char *)TtaGetMemory (length +
                                                       strlen(TheDoc->DocNaturePresName[i]) + 6);
                      sprintf (fileName, "%s\\%s.PRS", tmpDir, TheDoc->DocNaturePresName[i]);
                      DeleteAFile (fileName);
                      TtaFreeMemory (fileName);
                    } 
                }
              if (rmdir (tempDir))
		WinErrorBox (NULL, "PrintDoc (4)");
            }
        }
#else  /* _WINDOWS */
      sprintf (cmd, "/bin/rm -rf \"%s\"\n", tempDir);
      system (cmd);
#endif  /* _WINDOWS */
    }
  TtaFreeMemory (realName);
#ifdef _WX
  wxExit();
#endif /* _WX */
#ifdef _WINDOWS
  return;
#endif /* _WINDOWS */
}
