/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * User interface for spell checking
 *
 * Authors: I. Vatton, H. Richy (IRISA)
 *          R. Guetari (W3C/INRIA) - Unicode and Windows version
 *
 */

#include "thot_gui.h"
#include "thot_sys.h"

#include "corrmsg.h"
#include "libmsg.h"
#include "message.h"
#include "constmedia.h"
#include "constmenu.h"
#include "typemedia.h"
#include "constcorr.h"
#include "typecorr.h"
#include "language.h"
#include "dialog.h"
#include "corrmenu.h"
#include "appdialogue.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "edit_tv.h"
#include "appdialogue_tv.h"
#ifdef _WINDOWS
#include "wininclude.h"
#include "resource.h"
#endif /* _WINDOWS */
#undef THOT_EXPORT
#define THOT_EXPORT
#include "spell_tv.h"

/* les variables locales */
static int          OldNC;
static int          SpellingBase;
static ThotBool     FirstStep;
static ThotBool     ToReplace;
static CHAR_T       CorrectWord[MAX_WORD_LEN];
static CHAR_T       CurrentWord[MAX_WORD_LEN];
static PtrDocument  pDocSel;
/* static STRING       SpecialChars; */

/* les variables importees de l'editeur */

/* procedures importees de l'editeur */
#include "spellchecker_f.h"
#include "dictionary_f.h"
#include "word_f.h"
#include "appli_f.h"
#include "actions_f.h"
#include "memory_f.h"
#include "views_f.h"
#include "savedoc_f.h"
#include "structselect_f.h"
#include "message_f.h"

#ifdef _WINDOWS
#define IDC_WORDBUTTON    20000
#define IDC_LANGEDIT      20002
static ThotWindow   SpellChecker = NULL;
static ThotWindow   hwnListWords;
static ThotWindow   hwndCurrentWord;
static ThotWindow   hwndLanguage;
static CHAR_T       currentWord [MAX_WORD_LEN];
static ThotWindow   wordButton;
static CHAR_T       currentLabel [100];
static CHAR_T       currentRejectedchars [100]
#endif /* _WINDOWS */


/*----------------------------------------------------------------------
   RemoveSpellForm fait disparaitre les formulaires correction    
   s'ils concernent le document pDoc et s'ils sont a l'ecran.      
  ----------------------------------------------------------------------*/
static void         RemoveSpellForm ()
{
  if (ChkrRange != NULL)
    {
      FreeSearchContext (&ChkrRange);
      TtaDestroyDialogue (SpellingBase + ChkrFormCorrect);
    }
}

/*----------------------------------------------------------------------
   DisplayWords prepare le selecteur                             
  ----------------------------------------------------------------------*/
#ifndef _WINDOWS
#ifdef __STDC__
static void         DisplayWords (void)
#else
static void         DisplayWords ()
#endif
{
   int                 i, indx, length;
   STRING              entry;
   CHAR_T                BufMenu[MAX_TXT_LEN];

   /* recopie les propositions */
   indx = 0;
   /* commencer a 1 parce qu'en 0 il y a CurrentWord */
   for (i = 1; (i <= NC && ustrcmp (ChkrCorrection[i], "$") != 0); i++)
     {
	entry = ChkrCorrection[i];
	length = ustrlen (entry) + 1;
	if (length + indx < MAX_TXT_LEN)
	  {
	     ustrcpy ((BufMenu) + indx, entry);
	     indx += length;
	  }
     }
   /* creer le selecteur dans la feuille de dialogue */
   entry = ChkrCorrection[0];
   /* remplir le formulaire CORRIGER */
   /* attention i = nbpropositions + 1 (le mot errone) */
   TtaNewSelector (SpellingBase + ChkrSelectProp,
		   SpellingBase + ChkrFormCorrect,
		   TtaGetMessage (CORR, Correct), i - 1,
		   ((i < 2) ? "" : BufMenu), 3, entry, TRUE, FALSE);
   /* selectionner la proposition 0 dans le selecteur - si elle existe */
   if (ustrcmp (ChkrCorrection[1], "$") != 0)
      TtaSetSelector (SpellingBase + ChkrSelectProp, -1, ChkrCorrection[1]);
   else
      TtaSetSelector (SpellingBase + ChkrSelectProp, -1, "");

   /* le formulaire est maintenant pret a etre affiche' */
   OldNC = NC;
}
#endif /* !_WINDOWS */

#ifdef _WINDOWS
#ifdef __STDC__
void WIN_DisplayWords (void)
#else  /* __STDC__ */
void WIN_DisplayWords ()
#endif /* __STDC__ */
{
  int i;
  
  SetWindowText (wordButton, ChkrCorrection[0]);
  SendMessage (hwnListWords, LB_RESETCONTENT, 0, 0);
  if ((ustrcmp (ChkrCorrection[1], TEXT("$"))) == 0)
    {
      currentWord [0] = EOS;
      SetWindowText (hwndCurrentWord, TEXT(""));
      SendMessage (hwnListWords, LB_ADDSTRING, 0, (LPARAM) (TEXT("")));  
    }
  else
    {
      usprintf (currentWord, TEXT("%s"), ChkrCorrection[1]);
      SetWindowText (hwndCurrentWord, ChkrCorrection[1]);
      for (i = 1; (i <= NC && ustrcmp (ChkrCorrection[i], TEXT("$")) != 0);
	   i++)
	{
	  SendMessage (hwnListWords, LB_INSERTSTRING, i - 1,
		       (LPARAM) ((LPCTSTR)ChkrCorrection[i]));  
	}
    }
  OldNC = NC;
}
#endif /* _WINDOWS */

#ifdef _WINDOWS
/*-----------------------------------------------------------------------
 SpellCheckDlgProc
 Winbdows callback
 ------------------------------------------------------------------------*/
#ifdef __STDC__
static LRESULT CALLBACK SpellCheckDlgProc (ThotWindow hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
static LRESULT CALLBACK SpellCheckDlgProc (hwnDlg, msg, wParam, lParam)
ThotWindow   hwndParent;
UINT   msg;
WPARAM wParam;
LPARAM lParam;
#endif /* __STDC__ */
{
  ThotBool ok;	  
  int  val;

  static int  iLocation;
  static int  iIgnore;

  switch (msg)
    {
    case WM_INITDIALOG:
      SpellChecker = hwnDlg;
      hwndLanguage = GetDlgItem (hwnDlg, IDC_LANG);
      wordButton = GetDlgItem (hwnDlg, IDC_CURWORD);
      SetWindowText (GetDlgItem (hwnDlg, IDC_LABEL), currentLabel);
      SetWindowText (GetDlgItem (hwnDlg, IDC_BEFORE), TtaGetMessage (LIB, TMSG_BEFORE_SEL));
      SetWindowText (GetDlgItem (hwnDlg, IDC_WITHIN), TtaGetMessage (LIB, TMSG_WITHIN_SEL));
      SetWindowText (GetDlgItem (hwnDlg, IDC_AFTER), TtaGetMessage (LIB, TMSG_AFTER_SEL));
      SetWindowText (GetDlgItem (hwnDlg, IDC_WHOLEDOC), TtaGetMessage (LIB, TMSG_IN_WHOLE_DOC));
      SetWindowText (GetDlgItem (hwnDlg, IDC_NBPROPOSALS), TtaGetMessage (CORR, Number_Propositions));
      
      SetWindowText (GetDlgItem (hwnDlg, ID_SKIPNEXT), TtaGetMessage (CORR, Pass_Without));
      SetWindowText (GetDlgItem (hwnDlg, ID_SKIPDIC), TtaGetMessage (CORR, Pass_With));
      SetWindowText (GetDlgItem (hwnDlg, ID_REPLACENEXT), TtaGetMessage (CORR, Replace_Without));
      SetWindowText (GetDlgItem (hwnDlg, ID_REPLACEDIC), TtaGetMessage(CORR, Replace_With));
      
      SetWindowText (GetDlgItem (hwnDlg, IDC_IGNORE1), TtaGetMessage (CORR, Capitals));
      SetWindowText (GetDlgItem (hwnDlg, IDC_IGNORE2), TtaGetMessage (CORR, Arabics));
      SetWindowText (GetDlgItem (hwnDlg, IDC_IGNORE3), TtaGetMessage (CORR, Romans));
      SetWindowText (GetDlgItem (hwnDlg, IDC_IGNORE4), TtaGetMessage (CORR, Specials));
      
      SetWindowText (GetDlgItem (hwnDlg, IDC_CHECKGROUP), TtaGetMessage (CORR, What));
      
      SetWindowText (GetDlgItem (hwnDlg, IDC_IGNOREGROUP), TtaGetMessage (CORR, Ignore));
      
      SetWindowText (GetDlgItem (hwnDlg, ID_DONE), TtaGetMessage (LIB, TMSG_DONE));
      
      hwnListWords = CreateWindow (TEXT("listbox"), NULL, WS_CHILD | WS_VISIBLE | LBS_STANDARD,
				   13, 72, 150, 70, hwnDlg, (HMENU) 1, 
				   (HINSTANCE) GetWindowLong (hwnDlg, GWL_HINSTANCE), NULL);
      
      hwndCurrentWord = CreateWindow (TEXT("EDIT"), NULL, WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER,
				      13, 146, 150, 20, hwnDlg, (HMENU) IDC_LANGEDIT, 
				      (HINSTANCE) GetWindowLong (hwnDlg, GWL_HINSTANCE), NULL);
      
      
      CheckRadioButton (hwnDlg, IDC_BEFORE, IDC_WHOLEDOC, IDC_AFTER);
      CheckDlgButton (hwnDlg, IDC_IGNORE1, BST_CHECKED);
      CheckDlgButton (hwnDlg, IDC_IGNORE2, BST_CHECKED);
      CheckDlgButton (hwnDlg, IDC_IGNORE3, BST_CHECKED);
      CheckDlgButton (hwnDlg, IDC_IGNORE4, BST_CHECKED);
      
      SetDlgItemInt (hwnDlg, IDC_EDITPROPOSALS, 3, FALSE);
      SetDlgItemText (hwnDlg, IDC_EDITIGNORE, currentRejectedchars);
      iLocation = 2;
      SetWindowText (hwndCurrentWord, TEXT(""));
      WIN_DisplayWords ();
      break;
      
    case WM_CLOSE:
    case WM_DESTROY:
      SpellChecker = NULL;
      hwndLanguage = NULL;
      EndDialog (hwnDlg, ID_DONE);
      break;      
      
    case WM_COMMAND:
      if (LOWORD (wParam) == 1 && HIWORD (wParam) == LBN_SELCHANGE) {
	itemIndex = SendMessage (hwnListWords, LB_GETCURSEL, 0, 0);
	itemIndex = SendMessage (hwnListWords, LB_GETTEXT, itemIndex, (LPARAM) currentWord);
	SetDlgItemText (hwnDlg, IDC_LANGEDIT, currentWord);
      } else if (LOWORD (wParam) == 1 && HIWORD (wParam) == LBN_DBLCLK) {
	if (LB_ERR == (itemIndex = SendMessage (hwnListWords, LB_GETCURSEL, 0, 0L)))
	  break;
	itemIndex = SendMessage (hwnListWords, LB_GETTEXT, itemIndex, (LPARAM) currentWord);
	SetDlgItemText (hwnDlg, IDC_LANGEDIT, currentWord);
	ThotCallback (SpellingBase + ChkrSelectProp, STRING_DATA, currentWord);
	ThotCallback (SpellingBase + ChkrMenuOR, INTEGER_DATA, (CHAR_T*) iLocation);
	ThotCallback (SpellingBase + ChkrFormCorrect, INTEGER_DATA, (CHAR_T*) 3);
	if (iLocation == 3) {
	  CheckRadioButton (hwnDlg, IDC_BEFORE, IDC_WHOLEDOC, IDC_AFTER);
	  iLocation = 2;
	}
	return 0;
      } 
      if (HIWORD (wParam) == EN_UPDATE)
	{
	  if (LOWORD (wParam) == IDC_EDITPROPOSALS)
	    {
	      val = GetDlgItemInt (hwnDlg, IDC_EDITPROPOSALS, &ok, TRUE);
	      if (ok)
		ThotCallback (SpellingBase + ChkrCaptureNC, INTEGER_DATA, (CHAR_T*) val);
	    }
	  else if (LOWORD (wParam) == IDC_EDITIGNORE)
	    {
	      GetDlgItemText (hwnDlg, IDC_EDITIGNORE, currentRejectedchars, sizeof (currentRejectedchars) + 1);
	      ThotCallback (SpellingBase + ChkrSpecial, STRING_DATA, currentRejectedchars);
	    }
	  else if (LOWORD (wParam) == IDC_LANGEDIT) 
	    GetDlgItemText (hwnDlg, IDC_LANGEDIT, currentWord, sizeof (currentWord) + 1);
	}
      
      switch (LOWORD (wParam))
	{
	case IDC_BEFORE:
	  iLocation = 0;
	  break;
	  
	case IDC_WITHIN:
	  iLocation = 1;
	  break;
	  
	case IDC_AFTER:
	  iLocation = 2;
	  break;
	  
	case IDC_WHOLEDOC:
	  iLocation = 3;
	  break;
	  
	case IDC_IGNORE1:
	  ThotCallback (SpellingBase + ChkrMenuIgnore, INTEGER_DATA, (CHAR_T*) 0);
	  break;
	  
	case IDC_IGNORE2:
	  ThotCallback (SpellingBase + ChkrMenuIgnore, INTEGER_DATA, (CHAR_T*) 1);
	  break;
	  
	case IDC_IGNORE3: 
	  ThotCallback (SpellingBase + ChkrMenuIgnore, INTEGER_DATA, (CHAR_T*) 2);
	  break;
	  
	case IDC_IGNORE4:
	  ThotCallback (SpellingBase + ChkrMenuIgnore, INTEGER_DATA, (CHAR_T*) 3);
	  break;
	  
	case ID_SKIPNEXT:
	  ThotCallback (SpellingBase + ChkrSelectProp, STRING_DATA, currentWord);
	  ThotCallback (SpellingBase + ChkrMenuOR, INTEGER_DATA, (CHAR_T*) iLocation);
	  ThotCallback (SpellingBase + ChkrFormCorrect, INTEGER_DATA, (CHAR_T*) 1);
	  if (iLocation == 3) {
	    CheckRadioButton (hwnDlg, IDC_BEFORE, IDC_WHOLEDOC, IDC_AFTER);
	    iLocation = 2;
	  }
	  break;
	  
	case ID_SKIPDIC:
	  ThotCallback (SpellingBase + ChkrSelectProp, STRING_DATA, currentWord);
	  ThotCallback (SpellingBase + ChkrMenuOR, INTEGER_DATA, (CHAR_T*) iLocation);
	  ThotCallback (SpellingBase + ChkrFormCorrect, INTEGER_DATA, (CHAR_T*) 2);
	  if (iLocation == 3)
	    {
	      CheckRadioButton (hwnDlg, IDC_BEFORE, IDC_WHOLEDOC, IDC_AFTER);
	      iLocation = 2;
	    }
	  break;
	  
	case ID_REPLACENEXT:
	  ThotCallback (SpellingBase + ChkrSelectProp, STRING_DATA, currentWord);
	  ThotCallback (SpellingBase + ChkrMenuOR, INTEGER_DATA, (CHAR_T*) iLocation);
	  ThotCallback (SpellingBase + ChkrFormCorrect, INTEGER_DATA, (CHAR_T*) 3);
	  if (iLocation == 3)
	    {
	      CheckRadioButton (hwnDlg, IDC_BEFORE, IDC_WHOLEDOC, IDC_AFTER);
	      iLocation = 2;
	    }
	  break;
	  
	case ID_REPLACEDIC:
	  ThotCallback (SpellingBase + ChkrSelectProp, STRING_DATA, currentWord);
	  ThotCallback (SpellingBase + ChkrMenuOR, INTEGER_DATA, (CHAR_T*) iLocation);
	  ThotCallback (SpellingBase + ChkrFormCorrect, INTEGER_DATA, (CHAR_T*) 4);
	  if (iLocation == 3)
	    {
	      CheckRadioButton (hwnDlg, IDC_BEFORE, IDC_WHOLEDOC, IDC_AFTER);
	      iLocation = 2;
	    }
	  break;
	  
	case IDC_WORDBUTTON:
	  GetWindowText (wordButton, currentWord, MAX_WORD_LEN);
	  SetWindowText (hwndCurrentWord, currentWord);
	  break;
	  
	case ID_DONE:
	  SpellChecker = NULL;
      hwndLanguage = NULL;
	  EndDialog (hwnDlg, ID_DONE);
	  break;	  
	}
      break;
    default: return FALSE;
    }
  return TRUE;
}

/*-----------------------------------------------------------------------
 CreateSpellCheckDlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
static void CreateSpellCheckDlgWindow (ThotWindow parent, STRING label, STRING rejectedChars,
								int spellingBase, int chkrSelectProp, int chkrMenuOR, 
							    int chkrFormCorrect, int chkrMenuIgnore, int chkrCaptureNC, int chkrSpecial)
#else  /* !__STDC__ */
static void CreateSpellCheckDlgWindow (parent, label, rejectedChars, spellingBase, 
								chkrSelectProp, chkrMenuOR, chkrFormCorrect, chkrMenuIgnore, chkrCaptureNC, chkrSpecial)
ThotWindow  parent;
STRING label;
STRING rejectedChars;
int   spellingBase;
int   chkrSelectProp;
int   chkrMenuOR;
int   chkrFormCorrect;
int   chkrMenuIgnore;
int   chkrCaptureNC;
int   chkrSpecial;
#endif /* __STDC__ */
{  
#if 0
  SpellingBase    = spellingBase;
  ChkrSelectProp  = chkrSelectProp;
  ChkrMenuOR      = chkrMenuOR;
  ChkrFormCorrect = chkrFormCorrect;
  ChkrMenuIgnore  = chkrMenuIgnore;
  ChkrCaptureNC   = chkrCaptureNC;
  ChkrSpecial     = chkrSpecial;
#endif

  ustrcpy (currentLabel, label);
  ustrcpy (currentRejectedchars, rejectedChars); 
  /* to have the same behavior as under Unix, we need to destroy the
     dialog if it already existed */
  if (SpellChecker) 
    {
      EndDialog (SpellChecker, ID_DONE);
      SpellChecker = NULL;
      hwndLanguage = NULL;
    }
  DialogBox (hInstance, MAKEINTRESOURCE (SPELLCHECKDIALOG), NULL, (DLGPROC) SpellCheckDlgProc);
}
#endif /* _WINDOWS */

/*----------------------------------------------------------------------
  TtcSpellCheck  active le formulaire de correction                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcSpellCheck (Document doc, View view)
#else  /* __STDC__ */
void                TtcSpellCheck (doc, view)
Document            doc;
view                view;
#endif /* __STDC__ */
{
   PtrDocument         document;
   int                 i;
#ifndef _WINDOWS
   PtrElement          pEl1, pElN;
   int                 c1, cN;
   int                 indx;
   CHAR_T                BufMenu[MAX_TXT_LEN];
   ThotBool            ok;
#endif /* !_WINDOWS */

   /* SpecialChars = "@#$&+~"; */

   /* Document concerne */
   document = LoadedDocument[doc - 1];
   if (document == NULL)
      return;
   SpellCheckLoadResources ();

#ifndef _WINDOWS 
   if (ChkrRange != NULL)
      RemoveSpellForm ();
   TtaDestroyDialogue (SpellingBase + ChkrFormCorrect);
   /* creer la feuille de dialogue de CORRECTION */
   /* attache'e au bouton Confirmer du formulaire (1) CORRIGER */
   indx = 0;		   /* tous les boutons du dialogue de correction */
   ustrcpy (&BufMenu[indx], TtaGetMessage (CORR, Pass_Without));
   indx += ustrlen(&BufMenu[indx]) + 1;
   ustrcpy(&BufMenu[indx], TtaGetMessage(CORR, Pass_With));
   indx += ustrlen (&BufMenu[indx]) + 1;
   ustrcpy (&BufMenu[indx], TtaGetMessage (CORR, Replace_Without));
   indx += ustrlen(&BufMenu[indx]) + 1;
   ustrcpy(&BufMenu[indx], TtaGetMessage(CORR, Replace_With));
   /* ne pas afficher cette feuille maintenant */
   TtaNewSheet (SpellingBase + ChkrFormCorrect, TtaGetViewFrame (doc, view), 
		TtaGetMessage (CORR, Correct), 4, BufMenu, FALSE, 4, TEXT('L'),
		D_DONE);

   /* initialise le champ langue de correction courante */
   TtaNewLabel (SpellingBase + ChkrLabelLanguage,
		SpellingBase + ChkrFormCorrect, " ");
#endif /* !_WINDOWS */

   /* Afficher une liste de mots EMPTY */
   ustrcpy (ChkrCorrection[0], TEXT(" "));
   for (i = 1; i <= NC; i++)
      ustrcpy (ChkrCorrection[i], TEXT("$"));

#ifndef _WINDOWS 
   DisplayWords ();

   /* creer le sous-menu OU dans la feuille OPTIONS */
   indx = 0;			/* Ou commencer la correction ? */
   sprintf (&BufMenu[indx], "B%s", TtaGetMessage (LIB, TMSG_BEFORE_SEL));
   indx += ustrlen (&BufMenu[indx]) + 1;
   sprintf (&BufMenu[indx], "B%s", TtaGetMessage (LIB, TMSG_WITHIN_SEL));
   indx += ustrlen (&BufMenu[indx]) + 1;
   sprintf (&BufMenu[indx], "B%s", TtaGetMessage (LIB, TMSG_AFTER_SEL));
   indx += ustrlen (&BufMenu[indx]) + 1;
   sprintf (&BufMenu[indx], "B%s", TtaGetMessage (LIB, TMSG_IN_WHOLE_DOC));
   TtaNewSubmenu (SpellingBase + ChkrMenuOR, SpellingBase + ChkrFormCorrect, 0,
		  TtaGetMessage (CORR, What), 4, BufMenu, NULL, FALSE);
   TtaSetMenuForm (SpellingBase + ChkrMenuOR, 2);     /* apres la selection */
   /* Initialiser le formulaire CORRIGER */
   /* Document selectionne */
   ok = GetCurrentSelection (&pDocSel, &pEl1, &pElN, &c1, &cN);
   if (!ok || pDocSel != document)
     {
	/* pas de selection dans le document d'ou vient la commande */
	UnsetEntryMenu (SpellingBase + ChkrMenuOR, 0);
	UnsetEntryMenu (SpellingBase + ChkrMenuOR, 1);
	UnsetEntryMenu (SpellingBase + ChkrMenuOR, 2);
	TtaSetMenuForm (SpellingBase + ChkrMenuOR, 3);
     }
   else
     {

	TtaRedrawMenuEntry (SpellingBase + ChkrMenuOR, 0, NULL, -1, 1);
	TtaRedrawMenuEntry (SpellingBase + ChkrMenuOR, 1, NULL, -1, 1);
	TtaRedrawMenuEntry (SpellingBase + ChkrMenuOR, 2, NULL, -1, 1);
     }

   /* initialise le champ termine de correction courante */
   TtaNewLabel (SpellingBase + ChkrLabelNotFound, SpellingBase + ChkrFormCorrect, " ");

   /* creer la forme de modification d'un nombre NC 1-10 */
   TtaNewNumberForm (SpellingBase + ChkrCaptureNC, SpellingBase + ChkrFormCorrect,
     TtaGetMessage (CORR, Number_Propositions), 1, MAX_PROPOSAL_CHKR, TRUE);
   TtaSetNumberForm (SpellingBase + ChkrCaptureNC, 3);

   /* sous-menu Mots a ignorer */
   indx = 0;
   sprintf (&BufMenu[indx], "B%s", TtaGetMessage (CORR, Capitals));
   indx += ustrlen (&BufMenu[indx]) + 1;
   sprintf (&BufMenu[indx], "B%s", TtaGetMessage (CORR, Arabics));
   indx += ustrlen (&BufMenu[indx]) + 1;
   sprintf (&BufMenu[indx], "B%s", TtaGetMessage (CORR, Romans));
   indx += ustrlen (&BufMenu[indx]) + 1;
   sprintf (&BufMenu[indx], "B%s", TtaGetMessage (CORR, Specials));
   TtaNewToggleMenu (SpellingBase + ChkrMenuIgnore,
		     SpellingBase + ChkrFormCorrect,
		     TtaGetMessage (CORR, Ignore),
		     4, BufMenu, NULL, TRUE);

   /* liste des caracteres speciaux dans le formulaire OPTIONS */
   TtaNewTextForm (SpellingBase + ChkrSpecial, SpellingBase + ChkrFormCorrect,
		   NULL, 20, 1, TRUE);
   TtaSetTextForm (SpellingBase + ChkrSpecial, TEXT("@#$&+~"));
#endif /* _WINDOWS */

   /* ne pas ignorer les mots en capitale, chiffres romains */
   /* ou contenant des chiffres arabes,  certains car. speciaux */
   ustrncpy (RejectedChar, TEXT("@#$&+~"), MAX_REJECTED_CHARS);
   /* liste par defaut */

   IgnoreUppercase = TRUE;
   IgnoreArabic = TRUE;
   IgnoreRoman = TRUE;
   IgnoreSpecial = TRUE;

#ifndef _WINDOWS 
   /* Selectionne les types de mots a ne ignorer par defaut */
   TtaSetToggleMenu (SpellingBase + ChkrMenuIgnore, 0, IgnoreUppercase);
   TtaSetToggleMenu (SpellingBase + ChkrMenuIgnore, 1, IgnoreArabic);
   TtaSetToggleMenu (SpellingBase + ChkrMenuIgnore, 2, IgnoreRoman);
   TtaSetToggleMenu (SpellingBase + ChkrMenuIgnore, 3, IgnoreSpecial);

   /* affiche une liste de car. speciaux */
   TtaSetTextForm (SpellingBase + ChkrSpecial, RejectedChar);

   /* Et enfin, afficher le formulaire de CORRECTION */
   TtaShowDialogue (SpellingBase + ChkrFormCorrect, TRUE);
#endif /* _WINDOWS */

   /* Indique que c'est une nouvelle correction qui debute */
   FirstStep = TRUE;

   ChkrRange = NULL;		/* pas de contexte de correction alloue */
   /* On alloue le contexte */
   GetSearchContext (&ChkrRange);
   ChkrRange->SDocument = document;
   /* ne cree pas inutilement le dictionnaire fichier */
   TtaLoadDocumentDictionary (document, (int*) &ChkrFileDict, FALSE);

#ifdef _WINDOWS 
   CreateSpellCheckDlgWindow (TtaGetViewFrame (doc, view),
			      TtaGetMessage (CORR, Correct), RejectedChar,
			      SpellingBase, ChkrSelectProp, ChkrMenuOR, 
			      ChkrFormCorrect, ChkrMenuIgnore, ChkrCaptureNC,
			      ChkrSpecial);

#endif /* _WINDOWS */
}


/*----------------------------------------------------------------------
   SetProposals                                                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         SetProposals (Language language)
#else  /* __STDC__ */
static void         SetProposals (language)
Language            language;
#endif /* __STDC__ */
{
   CHAR_T             Lab[200];

   /* calculer les propositions de correction du mot courant */
   GiveProposal (language, ChkrFileDict);
   /* afficher les mots proposes contenus dans ChkrErrWord */
#ifndef _WINDOWS
   DisplayWords ();
#else  /* _WINDOWS */
   WIN_DisplayWords ();
#endif /* _WINDOWS */
   /* recopier la premiere proposition dans CorrectWord */
   if (ustrcmp (ChkrCorrection[1], TEXT("$")) != 0)
      ustrcpy (CorrectWord, ChkrCorrection[1]);
   else
      CorrectWord[0] = EOS;

   /* afficher la langue de correction courante */
   usprintf (Lab, TEXT("%s: %s"), TtaGetMessage (LIB, TMSG_LANGUAGE),
	     TtaGetLanguageName (ChkrLanguage));
#ifdef _WINDOWS
   SetWindowText (hwndLanguage, Lab);
#else  /* !_WINDOWS */
   TtaNewLabel (SpellingBase + ChkrLabelLanguage,
		SpellingBase + ChkrFormCorrect, Lab);
#endif /* _WINDOWS */
}


/*----------------------------------------------------------------------
   StartSpellChecker retourne 0 si c'est OK et -1 en cas d'erreur          
  ----------------------------------------------------------------------*/
static ThotBool     StartSpellChecker ()
{
   ThotBool            ok;

   /* preparer le lancement de la premiere correction */
   ok = TRUE;
   CorrectWord[0] = EOS;
   CurrentWord[0] = EOS;
   ToReplace = FALSE;
   /* Rechercher la premiere erreur */
   if (FirstStep)
     {
	FirstStep = FALSE;
	ChkrElement = NULL;
	ChkrIndChar = 0;
     }

   /* en tenant compte des options choisies (par defaut) */
   NextSpellingError (CurrentWord, ChkrFileDict);
   if (CurrentWord[0] != EOS)
      /* calculer la 1ere liste des propositions ds selecteur */
      SetProposals (ChkrLanguage);
   else
     {
       /* Correction TERMINEE */
#ifdef _WINDOWS
       MessageBox (NULL, TtaGetMessage (LIB, TMSG_NOT_FOUND), \
		   TEXT("Spell checking"), MB_OK | MB_ICONINFORMATION);
#else  /* _WINDOWS */
       TtaDisplaySimpleMessage (INFO, CORR, END_CHECK);
       /* message 'Pas trouve' dans le formulaire */
       TtaNewLabel (SpellingBase + ChkrLabelNotFound,
		    SpellingBase + ChkrFormCorrect,
		    TtaGetMessage (LIB, TMSG_NOT_FOUND));
#endif /* _WINDOWS */
       FirstStep = TRUE;
       ok = FALSE;
     }
   return (ok);
}


/*----------------------------------------------------------------------
   ApplyCommand traite l'action de correction demandee.             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ApplyCommand (int val)

#else  /* __STDC__ */
static void         ApplyCommand (val)
int                 val;

#endif /* __STDC__ */
{
   ThotBool            change;
   PtrDocument         document;

   if (ChkrRange == NULL)
      /* Le correcteur n'est pas lance */
      return;

   /* Teste l'etat de la nouvelle selection */
   document = ChkrRange->SDocument;
   change = CheckChangeSelection ();

   if (document != ChkrRange->SDocument || val == 0)
     {
       /* ABANDON de la corrrection */
       RemoveSpellForm ();
     }
   else
     {
       change = TRUE;		/* On passe a priori au mot suivant */
       switch (val)
	 {
	 case 1:
	   /* Passer sans maj du dictionnaire */
	   break;
	 case 2:
	   /* Passer apres maj du dictionnaire */
	   /* mettre CurrentWord dans le dictionnaire */
	   if (ChkrElement != NULL)	/* Sauf au 1er lancement */
	     if (CorrectWord[0] != EOS &&
		 (ustrcmp (CorrectWord, ChkrCorrection[1]) != 0))
	       {
		 /* ajouter le mot corroge dans le dictionaire */
		 if (!CheckWord (CorrectWord, ChkrLanguage, ChkrFileDict))
		   AddWord (CorrectWord, &ChkrFileDict);
	       }
	     else
	       {
		 /* ajouter le mot courant dans le dictionnaire */
		 if (!CheckWord (CurrentWord, ChkrLanguage, ChkrFileDict))
		   AddWord (CurrentWord, &ChkrFileDict);
	       }
	   break;
	 case 3:
	   /* ToReplace */
	   if (ToReplace)	/* CorrectWord est rempli */
	     {		/* et ce n'est pas 1er lancement */
	       if (ChkrElement != NULL && CurrentWord[0] != EOS)
		 WordReplace (CurrentWord, CorrectWord);
	       ToReplace = FALSE;
	     }
	   break;
	 case 4:
	   /* ToReplace et maj dictionnaire */
	   if (ToReplace)	/* CorrectWord est rempli */
	     {		/* et ce n'est pas 1er lancement */
	       if (ChkrElement != NULL && CurrentWord[0] != EOS)
		 {
		   WordReplace (CurrentWord, CorrectWord);
		   /* mettre CorrectWord dans le dictionnaire */
		   if (!CheckWord (CorrectWord, ChkrLanguage, ChkrFileDict))
		     /* mettre ce nouveau mot dans le dictionnaire */
		     AddWord (CorrectWord, &ChkrFileDict);
		 }
	       ToReplace = FALSE;
	     }
	   break;
	 }	
       
       if (change)
	 /* Lancement de la recherche d'erreur suivante */
	 StartSpellChecker ();
     }
}


/*----------------------------------------------------------------------
   CallbackCorrector                                                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CallbackCorrector (int ref, int dataType, STRING data)
#else  /* __STDC__ */
void                CallbackCorrector (ref, dataType, data)
int                 ref;
int                 dataType;
STRING              data;
#endif /* __STDC__ */
{
  PtrElement          pEl1, pElN;
  int                 c1, cN;

  if (ref == -1)
    {
      /* detruit le dialogue du correcteur */
      RemoveSpellForm ();
    }
  else
    switch (ref - SpellingBase)
      {
      case ChkrMenuIgnore:
	switch ((int) data)
	  {
	  case 0:
	    /* ignorer les mots ecrits en majuscule */
	    IgnoreUppercase = !IgnoreUppercase;
	    break;
	  case 1:
	    /* ignorer les mots contenant un chiffre arabe */
	    IgnoreArabic = !IgnoreArabic;
	    break;
	  case 2:
	    /* ignorer les chiffres romains */
	    IgnoreRoman = !IgnoreRoman;
	    break;
	  case 3:
	    /* ignorer les mots contenant certains car speciaux */
	    IgnoreSpecial = !IgnoreSpecial;
	    break;
	  }	/* end of switch */
	break;
      case ChkrSpecial:
	/* recopier la liste des car. speciaux dans RejectedChar[] */
	ustrncpy (RejectedChar, data, MAX_REJECTED_CHARS);
	/* bascule automatiquement l'indicateur IgnoreSpecial */
	if (!IgnoreSpecial)
	  {
	    IgnoreSpecial = TRUE;
#ifndef _WINDOWS 
	    TtaSetToggleMenu (SpellingBase + ChkrMenuIgnore, 3, IgnoreSpecial);
#endif /* _WINDOWS */
	  }
	break;
      case ChkrCaptureNC:
	/* modification de Nb de corrections a proposer */
	NC = (int) data;
	if (NC > OldNC && ChkrElement != NULL && CurrentWord[0] != EOS)
	  SetProposals (ChkrLanguage);
	else
#ifndef _WINDOWS 
	  DisplayWords ();
#else  /* _WINDOWS */
	WIN_DisplayWords ();
#endif /* _WINDOWS */
	break;
      case ChkrMenuOR:
	/* definition du sens de correction OU? */
	switch ((int) data)
	  {
	  case 0:	/* ElemIsBefore la selection */
	    ChkrRange->SStartToEnd = FALSE;
	    break;
	  case 1:	/* Dans la selection */
	    FirstStep = TRUE;
	    break;
	  case 2:	/* Apres la selection */
	    ChkrRange->SStartToEnd = TRUE;
	    break;
	  case 3:	/* Dans tout le document */
	    FirstStep = TRUE;
	    break;
	  }
	/* La premiere fois on initialise le domaine de recherche */
	if (FirstStep)
	  {
	    InitSearchDomain ((int) data, ChkrRange);
	    /* On prepare la recheche suivante */
#ifndef _WINDOWS
	    if (ChkrRange->SStartToEnd)
	      TtaSetMenuForm (SpellingBase + ChkrMenuOR, 2);
	    else
	      TtaSetMenuForm (SpellingBase + ChkrMenuOR, 0);
#endif /* !_WINDOWS */
	  }
	else if (ChkrRange != NULL)
	  /* Est-ce que le document vient de recevoir la selection */
	  if (pDocSel != ChkrRange->SDocument)
	    {
	      /* Est-ce encore vrai */
	      GetCurrentSelection (&pDocSel, &pEl1, &pElN, &c1, &cN);
#ifndef _WINDOWS
	      if (pDocSel == ChkrRange->SDocument) {
		/* Il faut reactiver les entree */
		TtaRedrawMenuEntry (SpellingBase + ChkrMenuOR, 0, NULL, -1, 1);
		TtaRedrawMenuEntry (SpellingBase + ChkrMenuOR, 1, NULL, -1, 1);
		TtaRedrawMenuEntry (SpellingBase + ChkrMenuOR, 2, NULL, -1, 1);
	      } 
#endif /* !_WINDOWS */
	    }
	break;
      case ChkrSelectProp:
	/* retour du selecteur de propositions */
	/* recopier le choix dans CorrectWord */
	ustrcpy (CorrectWord, data);
	if (CorrectWord[0] != EOS
	    && CurrentWord != EOS
	    && ustrcmp (CorrectWord, CurrentWord) != 0)
	  ToReplace = TRUE;
	break;
      case ChkrFormCorrect:
	/* retour de la feuille de dialogue CORRECTION */
	/* effectuer l'action demandee */
	ApplyCommand ((int) data);
	break;
      }
}


/*----------------------------------------------------------------------
   SpellCheckLoadResources charge les ressouces du correcteur       
   orthographique                                         
  ----------------------------------------------------------------------*/
void                SpellCheckLoadResources ()
{
   if (ThotLocalActions[T_corrector] == NULL)
     {
	TteConnectAction (T_rscorrector, (Proc) CallbackCorrector);
	TteConnectAction (T_corrector, (Proc) TtcSpellCheck);
	CORR = TtaGetMessageTable (TEXT("corrdialogue"), MSG_MAX_CHECK);
	SpellingBase = TtaSetCallback (CallbackCorrector, ChkrMaxDialogue);
	/* Initialisation des variables globales */
	ParametrizeChecker ();
	ChkrFileDict = NULL;
	ChkrRange = NULL;
     }
}


