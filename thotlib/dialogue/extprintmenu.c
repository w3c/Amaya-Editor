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
#undef EXPORT
#define EXPORT extern
#include "print_tv.h"
#include "edit_tv.h"
#include "appdialogue_tv.h"
#include "printmenu_f.h"

static int NewFirstPage;
static int NewLastPage;
static int NewNbCopies;
static int NewReduction;
static int NewPagesPerSheet;
/*----------------------------------------------------------------------
   CallbackExtPrintmenu analyse les retours des extensions du formulaire d'impression. 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CallbackExtPrintmenu (int ref, int val, char *txt)
#else  /* __STDC__ */
void                CallbackExtPrintmenu (ref, val, txt)
int                 ref;
int                 val;
char               *txt;

#endif /* __STDC__ */
{
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
    case NumFormPrint:
      FirstPage = NewFirstPage;
      LastPage = NewLastPage;
      NbCopies = NewNbCopies;
      Reduction = NewReduction;
      PagesPerSheet = NewPagesPerSheet;
      break;
    default:
      break;

    }
}
/*----------------------------------------------------------------------
   TtcExtPrintSetup construit les catalogues qui seront utilises      
   par l'editeur pour le formulaire d'impression etendu.          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcExtPrintSetup (Document document, View view)
#else  /* __STDC__ */
void                TtcExtPrintSetup (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   int                 i;
   char                BufMenu[MAX_TXT_LEN];


   pDocPrint = LoadedDocument[document - 1];
   ConnectPrint ();
   NewFirstPage = FirstPage;
   NewLastPage = LastPage;
   NewNbCopies = NbCopies;
   NewReduction = Reduction;
   NewPagesPerSheet = PagesPerSheet;
 
   if (ThotLocalActions[T_rextprint] == NULL)
     /* Connecte les actions liees au traitement de la split */
     TteConnectAction(T_rextprint, (Proc)CallbackExtPrintmenu);
   
   /* formulaire Imprimer */
   TtaNewSheet (NumFormPrint, TtaGetViewFrame (document, view), 0, 0,
		TtaGetMessage (LIB, TMSG_LIB_PRINT),
	   1, TtaGetMessage (LIB, TMSG_LIB_CONFIRM), FALSE, 4, 'L', D_CANCEL);

   /* premiere colonne */
 
  /* zone de saisie premiere page */
   TtaNewNumberForm(NumZoneFirstPage, NumFormPrint,
                   TtaGetMessage(LIB, TMSG_FIRST_PAGE), 0, 999, FALSE);
   TtaSetNumberForm(NumZoneFirstPage, FirstPage);

    /* label vide */
   TtaNewLabel(NumMenuOrientation,NumFormPrint," ");

   /* zone de saisie nombre d'exemplaires */
   TtaNewNumberForm(NumZoneNbOfCopies, NumFormPrint,
                   TtaGetMessage(LIB, TMSG_NB_COPIES), 1, 100, FALSE);
   TtaSetNumberForm(NumZoneNbOfCopies, NbCopies);

   /* zone de saisie du taux d'agrandissement/reduction */
   TtaNewNumberForm(NumZoneReduction, NumFormPrint,
                   TtaGetMessage(LIB, TMSG_REDUCTION), 5, 300, FALSE);
   TtaSetNumberForm(NumZoneReduction, Reduction);

  
   /* deuxieme colonne */

   /* zone de saisie premiere page */
   TtaNewNumberForm(NumZoneLastPage, NumFormPrint,
                   TtaGetMessage(LIB, TMSG_LAST_PAGE), 0, 999, FALSE);
   TtaSetNumberForm(NumZoneLastPage, LastPage);

   /* label vide */
   TtaNewLabel(NumMenuViewsToPrint,NumFormPrint," ");

   /* sous-menu nombre de pages par feuille */
   i = 0;
   sprintf(&BufMenu[i],"%s%s","B", TtaGetMessage(LIB, TMSG_1_PAGE_SHEET));
   i += strlen(&BufMenu[i]) + 1;
   sprintf(&BufMenu[i],"%s%s","B", TtaGetMessage(LIB, TMSG_2_PAGE_SHEET));
   i += strlen(&BufMenu[i]) + 1;
   sprintf(&BufMenu[i],"%s%s","B", TtaGetMessage(LIB, TMSG_4_PAGE_SHEET));
   TtaNewSubmenu(NumMenuNbPagesPerSheet, NumFormPrint, 0,
		 TtaGetMessage(LIB, TMSG_NB_PAGE_SHEET), 
		 3, BufMenu, NULL, False);
   if (PagesPerSheet == 1)
     TtaSetMenuForm(NumMenuNbPagesPerSheet, 0);
   else if (PagesPerSheet == 2)
     TtaSetMenuForm(NumMenuNbPagesPerSheet, 1);
   else
     TtaSetMenuForm(NumMenuNbPagesPerSheet, 2);

   /* label vide */
   TtaNewLabel(NumMenuReducedFormat,NumFormPrint," ");

   /* troisieme colonne */
 
   /* sous-menu imprimer papier / sauver PostScript */
   i = 0;
   sprintf (&BufMenu[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_PRINTER));
   i += strlen (&BufMenu[i]) + 1;
   sprintf (&BufMenu[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_PS_FILE));
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
   sprintf (&BufMenu[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_A4));
   i += strlen (&BufMenu[i]) + 1;
   sprintf (&BufMenu[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_US));
   TtaNewSubmenu (NumMenuPaperFormat, NumFormPrint, 0,
	     TtaGetMessage (LIB, TMSG_PAPER_SIZE), 2, BufMenu, NULL, FALSE);
   if (!strcmp (PageSize, "US"))
      TtaSetMenuForm (NumMenuPaperFormat, 1);
   else
      TtaSetMenuForm (NumMenuPaperFormat, 0);

   /* sous menu options */
   i = 0;
   sprintf (&BufMenu[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_MANUAL_FEED));
   TtaNewToggleMenu (NumMenuOptions, NumFormPrint,
		TtaGetMessage (LIB, TMSG_OPTIONS), 1, BufMenu, NULL, FALSE);
   if (ManualFeed)
      TtaSetToggleMenu (NumMenuOptions, 0, TRUE);

   /* active le formulaire "Imprimer" */
   TtaShowDialogue (NumFormPrint, FALSE);

}

