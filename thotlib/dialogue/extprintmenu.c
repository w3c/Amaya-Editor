/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2001
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
#include "view.h"
#include "print.h"
#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "print_tv.h"
#include "edit_tv.h"
#include "appdialogue_tv.h"

#include "actions_f.h"
#include "printmenu_f.h"
#include "schemas_f.h"
#include "ustring_f.h"
#include "views_f.h"

static int NewFirstPage;
static int NewLastPage;
static int NewNbCopies;
static int NewReduction;
static int NewPagesPerSheet;
static int		NbVuesImprimables;
static int		NbPrintViews;
static AvailableView	LesVuesImprimables;
static int		EntreesMenuVuesAImprimer[MAX_FRAME];
static Document		docPrint;

/* ----------------------------------------------------------------------
   ComposePrintMenu
   Compose le menu Imprimer pour le document pDoc
  ---------------------------------------------------------------------- */
static void ComposePrintMenu (PtrDocument pDoc, CHAR_T* buffer, int *nbEntry)
{
   int                 i, v;
   int                 nbentrees;
   int                 lgmenu, lgentree;
   PtrPSchema          pPSch;
   PrintedView         *pVueImp;
   ThotBool            trouve;

   /* compose le menu des vues a imprimer */
   /* construit d'abord la liste de toutes les vues possibles pour */
   /* ce document */
   NbVuesImprimables = BuildDocumentViewList (pDoc, LesVuesImprimables);
   /* et indique qu'aucune de ces vues n'est dans le menu */
   for (i = 0; i < NbVuesImprimables; i++)
      LesVuesImprimables[i].VdOpen = False;
   /* initialise le menu (vide) */
   nbentrees = 0;
   buffer[0] = WC_EOS;
   lgmenu = 0;
   /* met en tete du menu les vues indiquees dans l'instruction */
   /* PRINT du schema de presentation du document */
   pPSch = PresentationSchema (pDoc->DocSSchema, pDoc);
   for (v = 1; v <= pPSch->PsNPrintedViews; v++)
     {
       pVueImp = &pPSch->PsPrintedView[v - 1];
       /* cherche cette vue dans la liste des vues possibles */
       i = 0;
       trouve = False;
       while (i < NbVuesImprimables && !trouve)
	 {
	   i++;
	   /* c'est une vue de l'arbre principal */
	   trouve = LesVuesImprimables[i - 1].VdView == pVueImp->VpNumber &&
	            !ustrcmp (LesVuesImprimables[i - 1].VdSSchema->SsName,
			      pDoc->DocSSchema->SsName);
	 }
       if (trouve)
	 {
	   /* met le nom de la vue dans le menu */
	   lgentree = ustrlen (LesVuesImprimables[i - 1].VdViewName) + 1;
	   if (lgmenu + lgentree < MAX_TXT_LEN)
	     {
	       buffer[lgmenu] = 'B';
	       lgmenu++;
	       ustrcpy (buffer + lgmenu, LesVuesImprimables[i - 1].VdViewName);
	       lgmenu += lgentree;
	       if (!LesVuesImprimables[i - 1].VdPaginated)
		 /* vue sans pages, on met une etoile a la fin du nom */
		 {
		   buffer[lgmenu - 1] = '*';
		   buffer[lgmenu] = EOS;
		   lgmenu++;
		 }
	       EntreesMenuVuesAImprimer[nbentrees] = i;
	       nbentrees++;
	     }
	   /* indique que la vue est dans le menu */
	   LesVuesImprimables[i - 1].VdOpen = TRUE;
	 }
     }
   /* met ensuite dans le menu les autres vues */
   for (i = 1; i <= NbVuesImprimables; i++)
     {
	if (!LesVuesImprimables[i - 1].VdOpen)
	   /* cette vue n'est pas encore dans le menu, on la met */
	   /* sauf si c'est une vue de nature : print ne sait pas */
	   /* (pas encore) imprimer les vues de natures */
	   if (!LesVuesImprimables[i - 1].VdNature)
	     {
		lgentree = ustrlen (LesVuesImprimables[i - 1].VdViewName) + 1;
		if (lgmenu + lgentree < MAX_TXT_LEN)
		  {
		     buffer[lgmenu] = 'B';
		     lgmenu++;
		     ustrcpy (buffer + lgmenu,
			      LesVuesImprimables[i - 1].VdViewName);
		     lgmenu += lgentree;
		     if (!LesVuesImprimables[i - 1].VdPaginated)
			/* vue sans pages, on met une etoile a la fin du nom */
		       {
			  buffer[lgmenu - 1] = '*';
			  buffer[lgmenu] = WC_EOS;
			  lgmenu++;
		       }
		     EntreesMenuVuesAImprimer[nbentrees] = i;
		     nbentrees++;
		  }
	     }
     }
   if((nbentrees > 0) && !LesVuesImprimables[0].VdOpen)
     LesVuesImprimables[0].VdOpen = TRUE;
   *nbEntry = nbentrees;
}

/*----------------------------------------------------------------------
  CallbackExtPrintmenu
  analyse les retours des extensions du formulaire d'impression. 
  ----------------------------------------------------------------------*/
void                CallbackExtPrintmenu (int ref, int val, STRING txt)
{
  int      i;
  CHAR_T   BufMenu[MAX_TXT_LEN];
  ThotBool okprint;

  switch (ref)
    {
    case NumZoneFirstPage:
      NewFirstPage=val;
      break;
    case NumZoneNbOfCopies:
      NewNbCopies=val;
      break;
    case NumZoneReduction:
      if (NewReduction != val)
	NewReduction = val;
      break;
    case NumZoneLastPage:
      NewLastPage = val;
      break;
    case NumMenuNbPagesPerSheet:
      switch (val)
	{
	case 0:
	  NewPagesPerSheet  = 1;
	  break;
	case 1:
	  NewPagesPerSheet  = 2;
	  break;
	case 2:
	  NewPagesPerSheet = 4;
	  break;
	}
     break;
    case NumMenuViewsToPrint:
      LesVuesImprimables[EntreesMenuVuesAImprimer[val] - 1].VdOpen =
	        !LesVuesImprimables[EntreesMenuVuesAImprimer[val] - 1].VdOpen;
      break;
    case NumFormPrint:
      FirstPage = NewFirstPage;
      LastPage = NewLastPage;
      NbCopies = NewNbCopies;
      Reduction = NewReduction;
      PagesPerSheet = NewPagesPerSheet;
      okprint = FALSE;
      BufMenu[0] = EOS;
      for (i=0;i<NbPrintViews;i++)
	{ 
	  if( LesVuesImprimables[EntreesMenuVuesAImprimer[i]-1].VdOpen )
	    {
	      okprint=TRUE;
	      ustrcat (BufMenu, LesVuesImprimables[EntreesMenuVuesAImprimer[i]-1].VdViewName);
	      ustrcat (BufMenu, " ");
	    }
	}
      if(okprint)
	{
	  i = ustrlen (BufMenu);
	  BufMenu[i - 1] = WC_EOS;
	  TtaPrint (docPrint, BufMenu, NULL);
	}
      break;

    default:
      break;

    } 
}
 
/*----------------------------------------------------------------------
  TtcSetupAndPrint
  Complete dialogue sheet for print setup with option to print.
  ----------------------------------------------------------------------*/
void                TtcSetupAndPrint (Document document, View view)
{
   PtrDocument         pDoc;
   int                 i;
   CHAR_T              BufMenu[MAX_TXT_LEN];

   docPrint = document;
   pDoc = LoadedDocument[document - 1];
   InitPrintParameters (document);
   NewFirstPage = FirstPage;
   NewLastPage = LastPage;
   NewNbCopies = NbCopies;
   NewReduction = Reduction;
   NewPagesPerSheet = PagesPerSheet;
 
   if (ThotLocalActions[T_rextprint] == NULL)
     /* Connecte les actions liees au traitement de la split */
     TteConnectAction(T_rextprint, (Proc)CallbackExtPrintmenu);
   
   /* formulaire Imprimer */
   TtaNewSheet (NumFormPrint, TtaGetViewFrame (document, view), 
		TtaGetMessage (LIB, TMSG_LIB_PRINT),
	   1, TtaGetMessage (LIB, TMSG_LIB_PRINT), FALSE, 3, 'L', D_CANCEL);

   /* premiere colonne */
   /* zone de saisie premiere page */
   TtaNewNumberForm(NumZoneFirstPage, NumFormPrint,
                   TtaGetMessage(LIB, TMSG_FIRST_PAGE), 0, 999, FALSE);
   TtaSetNumberForm(NumZoneFirstPage, FirstPage);

   /* zone de saisie nombre d'exemplaires */
   TtaNewNumberForm(NumZoneNbOfCopies, NumFormPrint,
                   TtaGetMessage(LIB, TMSG_NB_COPIES), 1, 100, FALSE);
   TtaSetNumberForm(NumZoneNbOfCopies, NbCopies);
   /* sous menu options */
   i = 0;
   usprintf (&BufMenu[i], "B%s", TtaGetMessage (LIB, TMSG_MANUAL_FEED));
   i = ustrlen(BufMenu) +1;
   usprintf (&BufMenu[i], "B%s", TtaGetMessage (LIB, TMSG_PAGINATE));
   TtaNewToggleMenu (NumMenuOptions, NumFormPrint,
		TtaGetMessage (LIB, TMSG_OPTIONS), 2, BufMenu, NULL, FALSE);
   if (ManualFeed)
      TtaSetToggleMenu (NumMenuOptions, 0, TRUE);
   if (Paginate)
      TtaSetToggleMenu (NumMenuOptions, 1, TRUE);

     /* deuxieme colonne */
   /* zone de saisie derniere page */
   TtaNewNumberForm (NumZoneLastPage, NumFormPrint,
		     TtaGetMessage (LIB, TMSG_LAST_PAGE), 0, 999, FALSE);
   TtaSetNumberForm (NumZoneLastPage, LastPage);

   /* zone de saisie des vues a imprimer */
   ComposePrintMenu (pDoc, BufMenu, &NbPrintViews);
   TtaNewToggleMenu (NumMenuViewsToPrint,
		     NumFormPrint,
		     TtaGetMessage(LIB, TMSG_VIEWS_TO_PRINT),
		     NbPrintViews,BufMenu,NULL,TRUE);
   for (i=0; i<NbPrintViews; i++)
     if (LesVuesImprimables[EntreesMenuVuesAImprimer[i]-1].VdOpen)
       TtaSetToggleMenu (NumMenuViewsToPrint, i, 1);

   /* label vide */
   TtaNewLabel(NumEmptyLabel1,NumFormPrint, " ");

   /* troisieme colonne */
   /* zone de saisie du taux d'agrandissement/reduction */
   TtaNewNumberForm (NumZoneReduction, NumFormPrint,
		     TtaGetMessage(LIB, TMSG_REDUCTION), 10, 300, FALSE);
   TtaSetNumberForm (NumZoneReduction, Reduction);
   /* sous-menu nombre de pages par feuille */
   i = 0;
   usprintf (&BufMenu[i], "B%s", TtaGetMessage (LIB, TMSG_1_PAGE_SHEET));
   i += ustrlen(&BufMenu[i]) + 1;
   usprintf (&BufMenu[i], "B%s", TtaGetMessage (LIB, TMSG_2_PAGE_SHEET));
   i += ustrlen(&BufMenu[i]) + 1;
   usprintf (&BufMenu[i], "B%s", TtaGetMessage (LIB, TMSG_4_PAGE_SHEET));
   TtaNewSubmenu (NumMenuNbPagesPerSheet, NumFormPrint, 0,
		 TtaGetMessage (LIB, TMSG_NB_PAGE_SHEET), 
		 3, BufMenu, NULL, False);
   if (PagesPerSheet == 1)
     TtaSetMenuForm (NumMenuNbPagesPerSheet, 0);
   else if (PagesPerSheet == 2)
     TtaSetMenuForm (NumMenuNbPagesPerSheet, 1);
   else
     TtaSetMenuForm (NumMenuNbPagesPerSheet, 2);
   /* label vide */
   TtaNewLabel (NumEmptyLabel2,NumFormPrint, " ");

   /* quatrieme colonne */ 
   /* sous-menu imprimer papier / sauver PostScript */
   i = 0;
   usprintf (&BufMenu[i], "B%s", TtaGetMessage (LIB, TMSG_PRINTER));
   i += ustrlen (&BufMenu[i]) + 1;
   usprintf (&BufMenu[i], "B%s", TtaGetMessage (LIB, TMSG_PS_FILE));
   TtaNewSubmenu (NumMenuSupport, NumFormPrint, 0,
		  TtaGetMessage (LIB, TMSG_OUTPUT), 2, BufMenu, NULL, TRUE);
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
	TtaSetTextForm (NumZonePrinterName, PSdir);
     }
   /* sous-menu format papier */
   i = 0;
   usprintf (&BufMenu[i], "B%s", TtaGetMessage (LIB, TMSG_A4));
   i += ustrlen (&BufMenu[i]) + 1;
   usprintf (&BufMenu[i], "%s", TtaGetMessage (LIB, TMSG_US));
   TtaNewSubmenu (NumMenuPaperFormat, NumFormPrint, 0,
	     TtaGetMessage (LIB, TMSG_PAPER_SIZE), 2, BufMenu, NULL, FALSE);
   if (!ustrcmp (PageSize, "US"))
      TtaSetMenuForm (NumMenuPaperFormat, 1);
   else
      TtaSetMenuForm (NumMenuPaperFormat, 0);
   /* active le formulaire "Imprimer" */
   TtaShowDialogue (NumFormPrint, FALSE);
}
