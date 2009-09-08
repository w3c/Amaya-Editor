/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2005
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
#ifdef _WX
#include "wx/wx.h"
#endif /* _WX */

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
#include "frame.h"

#ifdef _WINGUI
#include "wininclude.h"
#include "resource.h"
#endif /* _WINGUI */

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "edit_tv.h"
#include "appdialogue_tv.h"
#include "frame_tv.h"
#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "spell_tv.h"

#ifdef _WX
#include "wxinclude.h"
#include "wx/msgdlg.h" // wxMessageDialog
#include "message_wx.h"
#endif /* _WX */

/* les variables locales */
static int          SpellingBase = 0;
static ThotBool     FirstStep;
static ThotBool     ToReplace;
static char         CorrectWord[MAX_WORD_LEN];
static char         BadWord[MAX_WORD_LEN];
static PtrDocument  pDocSel;

/* procedures importees de l'editeur */
#include "actions_f.h"
#include "appli_f.h"
#include "checkermenu_f.h"
#include "dictionary_f.h"
#include "font_f.h"
#include "memory_f.h"
#include "message_f.h"
#include "spellchecker_f.h"
#include "structselect_f.h"
#include "tree_f.h"
#include "views_f.h"
#include "word_f.h"

#ifdef _WINGUI
#define IDC_WORDBUTTON    20000
#define IDC_LANGEDIT      20002
static ThotWindow   SpellChecker = NULL;
static ThotWindow   WordList;
static ThotWindow   EditWord;
static ThotWindow   hwndLanguage;
static char         currentWord [MAX_WORD_LEN];
static ThotWindow   wordButton;
static UINT         itemIndex;
static int          iLocation;
static int          iIgnore;
#endif /* _WINGUI */

#ifdef _WX
#include "appdialogue_wx.h"
#endif /* _WX */

/*----------------------------------------------------------------------
  RemoveSpellForm fait disparaitre les formulaires correction    
  s'ils concernent le document pDoc et s'ils sont a l'ecran.      
  ----------------------------------------------------------------------*/
static void RemoveSpellForm ()
{
  if (ChkrRange != NULL)
    {
      FreeSearchContext (&ChkrRange);
#ifdef _WINGUI
      EndDialog (SpellChecker, ID_DONE);
      hwndLanguage = NULL;
      SpellChecker = NULL;
#else /* _WINGUI */
      TtaDestroyDialogue (SpellingBase + ChkrFormCorrect);
#endif /* _WINGUI */
    }
}

/*----------------------------------------------------------------------
  DisplayWords prepare le selecteur                             
  ----------------------------------------------------------------------*/
static void DisplayWords (void)
{
#ifdef _WINGUI
  int i;
  
  SetWindowText (wordButton, ChkrCorrection[0]);
  SendMessage (WordList, LB_RESETCONTENT, 0, 0);
  if ((strcmp (ChkrCorrection[1], "$")) == 0)
    {
      currentWord [0] = EOS;
      SetWindowText (EditWord, "");
      SendMessage (WordList, LB_ADDSTRING, 0, (LPARAM) (""));  
    }
  else
    {
      sprintf (currentWord, "%s", ChkrCorrection[1]);
      SetWindowText (EditWord, ChkrCorrection[1]);
      for (i = 1; (i <= NB_PROPOSALS && strcmp (ChkrCorrection[i], "$") != 0);
           i++)
        {
          SendMessage (WordList, LB_INSERTSTRING, i - 1,
                       (LPARAM) ((LPCTSTR)ChkrCorrection[i]));  
        }
    }
#endif /* _WINGUI */
}


#ifdef _WINGUI
/*-----------------------------------------------------------------------
  SpellCheckDlgProc
  ------------------------------------------------------------------------*/
static LRESULT CALLBACK SpellCheckDlgProc (ThotWindow hwnDlg, UINT msg,
                                           WPARAM wParam, LPARAM lParam)
{
  char currentRejectedchars [MAX_REJECTED_CHARS];

  switch (msg)
    {
    case WM_INITDIALOG:
      SpellChecker = hwnDlg;
      hwndLanguage = GetDlgItem (hwnDlg, IDC_LANG);
      wordButton = GetDlgItem (hwnDlg, IDC_CURWORD);
      SetWindowText (hwnDlg, TtaGetMessage (LIB, TMSG_Correct));
      SetWindowText (GetDlgItem (hwnDlg, IDC_LABEL), TtaGetMessage (LIB, TMSG_Correct));
      SetWindowText (GetDlgItem (hwnDlg, IDC_BEFORE), TtaGetMessage (LIB, TMSG_BEFORE_SEL));
      SetWindowText (GetDlgItem (hwnDlg, IDC_WITHIN), TtaGetMessage (LIB, TMSG_WITHIN_SEL));
      SetWindowText (GetDlgItem (hwnDlg, IDC_AFTER), TtaGetMessage (LIB, TMSG_AFTER_SEL));
      SetWindowText (GetDlgItem (hwnDlg, IDC_WHOLEDOC), TtaGetMessage (LIB, TMSG_IN_WHOLE_DOC));
      SetWindowText (GetDlgItem (hwnDlg, ID_SKIPNEXT), TtaGetMessage (LIB, TMSG_Pass_Without));
      SetWindowText (GetDlgItem (hwnDlg, ID_SKIPDIC), TtaGetMessage (LIB, TMSG_Pass_With));
      SetWindowText (GetDlgItem (hwnDlg, ID_REPLACENEXT), TtaGetMessage (LIB, TMSG_Replace_Without));
      SetWindowText (GetDlgItem (hwnDlg, ID_REPLACEDIC), TtaGetMessage(LIB, TMSG_Replace_With));
      
      SetWindowText (GetDlgItem (hwnDlg, IDC_IGNORE1), TtaGetMessage (LIB, TMSG_Capitals));
      SetWindowText (GetDlgItem (hwnDlg, IDC_IGNORE2), TtaGetMessage (LIB, TMSG_Arabics));
      SetWindowText (GetDlgItem (hwnDlg, IDC_IGNORE3), TtaGetMessage (LIB, TMSG_Romans));
      SetWindowText (GetDlgItem (hwnDlg, IDC_IGNORE4), TtaGetMessage (LIB, TMSG_Specials));
      
      SetWindowText (GetDlgItem (hwnDlg, IDC_CHECKGROUP), TtaGetMessage (LIB, TMSG_What));
      
      SetWindowText (GetDlgItem (hwnDlg, IDC_IGNOREGROUP), TtaGetMessage (LIB, TMSG_Ignore));
      
      SetWindowText (GetDlgItem (hwnDlg, ID_DONE), TtaGetMessage (LIB, TMSG_DONE));
      WordList = GetDlgItem (hwnDlg, IDC_WORDLIST);
      WIN_SetDialogfont(WordList);
      /*BuildWordList ();*/
      EditWord = GetDlgItem (hwnDlg, IDC_WORDEDIT);
      WIN_SetDialogfont(EditWord);
       
      CheckRadioButton (hwnDlg, IDC_BEFORE, IDC_WHOLEDOC, IDC_AFTER);
      CheckDlgButton (hwnDlg, IDC_IGNORE1, BST_CHECKED);
      CheckDlgButton (hwnDlg, IDC_IGNORE2, BST_CHECKED);
      CheckDlgButton (hwnDlg, IDC_IGNORE3, BST_CHECKED);
      CheckDlgButton (hwnDlg, IDC_IGNORE4, BST_CHECKED);
      
      SetDlgItemText (hwnDlg, IDC_EDITIGNORE, RejectedChar);
      iLocation = 2;
      SetWindowText (EditWord, "");
      DisplayWords ();
      break;
      
    case WM_CLOSE:
    case WM_DESTROY:
      SpellChecker = NULL;
      hwndLanguage = NULL;
      EndDialog (hwnDlg, ID_DONE);
      break;      
      
    case WM_COMMAND:
      switch (LOWORD (wParam))
        {
        case IDC_EDITIGNORE:
          GetDlgItemText (hwnDlg, IDC_EDITIGNORE, currentRejectedchars, sizeof (currentRejectedchars) + 1);
          ThotCallback (SpellingBase + ChkrSpecial, STRING_DATA, currentRejectedchars);
          break;
        case IDC_WORDLIST:
          itemIndex = SendMessage (WordList, LB_GETCURSEL, 0, 0);
          itemIndex = SendMessage (WordList, LB_GETTEXT, itemIndex,
                                   (LPARAM) currentWord);
          SetDlgItemText (hwnDlg, IDC_WORDEDIT, currentWord);
          if (HIWORD (wParam) == LBN_DBLCLK)
            {
              /* double click activate the replace */
              ThotCallback (SpellingBase + ChkrSelectProp, STRING_DATA, currentWord);
              ThotCallback (SpellingBase + ChkrMenuOR, INTEGER_DATA, (char*) iLocation);
              ThotCallback (SpellingBase + ChkrFormCorrect, INTEGER_DATA, (char*) 3);
              if (iLocation == 3)
                {
                  CheckRadioButton (hwnDlg, IDC_BEFORE, IDC_WHOLEDOC, IDC_AFTER);
                  iLocation = 2;
                }
              return 0;
            }
          break;
        case IDC_WORDEDIT:
          GetDlgItemText (hwnDlg, IDC_WORDEDIT, currentWord, sizeof (currentWord) + 1);
          break;
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
          ThotCallback (SpellingBase + ChkrMenuIgnore, INTEGER_DATA, (char*) 0);
          break;
	  
        case IDC_IGNORE2:
          ThotCallback (SpellingBase + ChkrMenuIgnore, INTEGER_DATA, (char*) 1);
          break;
	  
        case IDC_IGNORE3: 
          ThotCallback (SpellingBase + ChkrMenuIgnore, INTEGER_DATA, (char*) 2);
          break;
	  
        case IDC_IGNORE4:
          ThotCallback (SpellingBase + ChkrMenuIgnore, INTEGER_DATA, (char*) 3);
          break;
	  
        case ID_SKIPNEXT:
          ThotCallback (SpellingBase + ChkrSelectProp, STRING_DATA, currentWord);
          ThotCallback (SpellingBase + ChkrMenuOR, INTEGER_DATA, (char*) iLocation);
          ThotCallback (SpellingBase + ChkrFormCorrect, INTEGER_DATA, (char*) 1);
          if (iLocation == 3)
            {
              CheckRadioButton (hwnDlg, IDC_BEFORE, IDC_WHOLEDOC, IDC_AFTER);
              iLocation = 2;
            }
          break;
	  
        case ID_SKIPDIC:
          ThotCallback (SpellingBase + ChkrSelectProp, STRING_DATA, currentWord);
          ThotCallback (SpellingBase + ChkrMenuOR, INTEGER_DATA, (char*) iLocation);
          ThotCallback (SpellingBase + ChkrFormCorrect, INTEGER_DATA, (char*) 2);
          if (iLocation == 3)
            {
              CheckRadioButton (hwnDlg, IDC_BEFORE, IDC_WHOLEDOC, IDC_AFTER);
              iLocation = 2;
            }
          break;
	  
        case ID_REPLACENEXT:
          ThotCallback (SpellingBase + ChkrSelectProp, STRING_DATA, currentWord);
          ThotCallback (SpellingBase + ChkrMenuOR, INTEGER_DATA, (char*) iLocation);
          ThotCallback (SpellingBase + ChkrFormCorrect, INTEGER_DATA, (char*) 3);
          if (iLocation == 3)
            {
              CheckRadioButton (hwnDlg, IDC_BEFORE, IDC_WHOLEDOC, IDC_AFTER);
              iLocation = 2;
            }
          break;
	  
        case ID_REPLACEDIC:
          ThotCallback (SpellingBase + ChkrSelectProp, STRING_DATA, currentWord);
          ThotCallback (SpellingBase + ChkrMenuOR, INTEGER_DATA, (char*) iLocation);
          ThotCallback (SpellingBase + ChkrFormCorrect, INTEGER_DATA, (char*) 4);
          if (iLocation == 3)
            {
              CheckRadioButton (hwnDlg, IDC_BEFORE, IDC_WHOLEDOC, IDC_AFTER);
              iLocation = 2;
            }
          break;
	  
        case IDC_WORDBUTTON:
          GetWindowText (wordButton, currentWord, MAX_WORD_LEN);
          SetWindowText (EditWord, currentWord);
          break;
	  
        case IDCANCEL:
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
#endif /* _WINGUI */

/*----------------------------------------------------------------------
  TtcSpellCheck  active le formulaire de correction                
  ----------------------------------------------------------------------*/
void TtcSpellCheck (Document doc, View view)
{
  PtrDocument         document;
  PtrElement          pEl1, pElN;
  int                 c1, cN;
  int                 i;
  ThotBool            ok;
#ifdef _WX
  ThotBool             created;
#endif /* _WX */

  /* Document concerne */
  document = LoadedDocument[doc - 1];
  if (document == NULL)
    return;
  SpellCheckLoadResources ();

  /* Afficher une liste de mots EMPTY */
  strcpy (ChkrCorrection[0], " ");
  for (i = 1; i <= NB_PROPOSALS; i++)
    strcpy (ChkrCorrection[i], "$");

  /* ne pas ignorer les mots en capitale, chiffres romains */
  /* ou contenant des chiffres arabes,  certains car. speciaux */
  strncpy (RejectedChar, "@#$&+~", MAX_REJECTED_CHARS);
  /* liste par defaut */
  IgnoreUppercase = TRUE;
  IgnoreArabic = TRUE;
  IgnoreRoman = TRUE;
  IgnoreSpecial = TRUE;
  /* Document selectionne */
  ok = GetCurrentSelection (&pDocSel, &pEl1, &pElN, &c1, &cN);


  /* Indique que c'est une nouvelle correction qui debute */
  FirstStep = TRUE;
  ChkrRange = NULL;		/* pas de contexte de correction alloue */
  /* On alloue le contexte */
  GetSearchContext (&ChkrRange);
  ChkrRange->SDocument = document;
  /* ne cree pas inutilement le dictionnaire fichier */
  TtaLoadDocumentDictionary (document, (int*) &ChkrFileDict, FALSE);

#ifdef _WINGUI  
  /* to have the same behavior as under Unix, we need to destroy the
     dialog if it already existed */
  if (SpellChecker) 
    {
      EndDialog (SpellChecker, ID_DONE);
      SpellChecker = NULL;
      hwndLanguage = NULL;
    }
  DialogBox (hInstance, MAKEINTRESOURCE (SPELLCHECKDIALOG), NULL,
             (DLGPROC) SpellCheckDlgProc);
#endif /* _WINGUI */
#ifdef _WX
  if (!ok || pDocSel != document)
    i = 1;
  else
    i = 2;
  created = CreateSpellCheckDlgWX (SpellingBase + ChkrFormCorrect,
                                   SpellingBase, TtaGetViewFrame (doc, view), i);
  CallbackChecker (SpellingBase + ChkrMenuOR, INTEGER_DATA, (char *)i);
  if (created)
    TtaShowDialogue (SpellingBase + ChkrFormCorrect, FALSE, TRUE);
#endif /* _WX */
}

/*----------------------------------------------------------------------
  ResetCheckInDocument
  cleans up the checker domain if it refers a closed document.
  ----------------------------------------------------------------------*/
void ResetCheckInDocument (PtrDocument pDoc)
{
  if (ChkrRange && pDoc == ChkrRange->SDocument)
    RemoveSpellForm ();
}


/*----------------------------------------------------------------------
  SetProposals                                                 
  ----------------------------------------------------------------------*/
static void SetProposals (Language language)
{
  char             Lab[200];

  /* calculer les propositions de correction du mot courant */
  GiveProposal (language, ChkrFileDict);
  /* afficher les mots proposes contenus dans ChkrErrWord */
  DisplayWords ();
  /* recopier la premiere proposition dans CorrectWord */
  if (!strcmp (ChkrCorrection[1], "$"))
    strcpy (CorrectWord, ChkrCorrection[1]);
  else
    CorrectWord[0] = EOS;

  /* afficher la langue de correction courante */
  sprintf (Lab, "%s: %s", TtaGetMessage (LIB, TMSG_LANGUAGE),
           TtaGetLanguageName (ChkrLanguage));
#ifdef _WINGUI
  SetWindowText (hwndLanguage, Lab);
#endif /* _WINGUI */
#ifdef _GTK
  TtaNewLabel (SpellingBase + ChkrLabelLanguage,
               SpellingBase + ChkrFormCorrect, Lab);
#endif /* _GTK */
}

/*----------------------------------------------------------------------
  TtaGetProposal
  Returns a proposal from the list of proposals (needed by wx)
  ----------------------------------------------------------------------*/
void TtaGetProposal (char **proposal, int i)
{
  if (strcmp (ChkrCorrection[i], "$"))
    *proposal = ChkrCorrection[i];
  else
    proposal[0] = EOS;
}

/*----------------------------------------------------------------------
  TtaGetChkrLanguageName
  Returns the name of the checker language (needed by wx)
  Return value:
  the name of the language.
  ----------------------------------------------------------------------*/
void TtaGetChkrLanguageName (char **lang)
{
  *lang = TtaGetLanguageName (ChkrLanguage);
}

/*----------------------------------------------------------------------
  StartSpellChecker retourne 0 si c'est OK et -1 en cas d'erreur          
  ----------------------------------------------------------------------*/
static ThotBool StartSpellChecker ()
{
  CHARSET             defaultCharset;
  unsigned char      *word;
  ThotBool            ok;

  /* preparer le lancement de la premiere correction */
  ok = TRUE;
  CorrectWord[0] = EOS;
  BadWord[0] = EOS;
  strcpy (ChkrCorrection[0], " ");
  ToReplace = FALSE;
  /* Rechercher la premiere erreur */
  if (FirstStep)
    {
      FirstStep = FALSE;
      ChkrElement = NULL;
      ChkrIndChar = 1;
      /*InitSearchDomain ((int) data, ChkrRange);*/
    }

  /* en tenant compte des options choisies (par defaut) */
  NextSpellingError (ChkrFileDict);
  defaultCharset = TtaGetDefaultCharset ();
  /* convert the string into the dialog encoding if necessary */
  if (defaultCharset == UTF_8)
    {
      word = TtaConvertByteToMbs ((unsigned char *)ChkrErrWord,
                                  ISO_8859_1);
      strcpy (BadWord, (char *)word);
      TtaFreeMemory (word);
    }
  else
    strcpy (BadWord, ChkrErrWord);
  if (BadWord[0] != EOS)
    /* calculer la 1ere liste des propositions ds selecteur */
    SetProposals (ChkrLanguage);
  else
    {
      /* Correction TERMINEE */
#ifdef _WINGUI
      MessageBox (NULL, TtaGetMessage (LIB, TMSG_END_CHECK), \
                  "Spell checking", MB_OK | MB_ICONINFORMATION);
#endif /* _WINGUI */
#ifdef _GTK
      TtaDisplayMessage (CONFIRM, TtaGetMessage (LIB, TMSG_END_CHECK), NULL);
#endif /* _GTK */
      ok = FALSE;
    }
  return (ok);
}


/*----------------------------------------------------------------------
  ApplyCommand traite l'action de correction demandee.             
  ----------------------------------------------------------------------*/
static void ApplyCommand (int val)
{
  PtrDocument         document;
  CHARSET             defaultCharset;
  unsigned char      *word;
  ThotBool            change;

  if (ChkrRange == NULL)
    /* Le correcteur n'est pas lance */
    return;

  /* Teste l'etat de la nouvelle selection */
  document = ChkrRange->SDocument;
  change = CheckChangeSelection ();

  if (document != ChkrRange->SDocument || val == 0)
    /* ABANDON de la corrrection */
    RemoveSpellForm ();
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
          /* mettre BadWord dans le dictionnaire */
          if (ChkrElement != NULL)
            {
              /* Sauf au 1er lancement */
              defaultCharset = TtaGetDefaultCharset ();
              if (CorrectWord[0] != EOS &&
                  strcmp (CorrectWord, ChkrCorrection[1]))
                {
                  /* ajouter le mot corrige dans le dictionaire */
                  if (defaultCharset == UTF_8)
                    word = TtaConvertMbsToByte ((unsigned char *)CorrectWord,
                                                ISO_8859_1);
                  else
                    word = (unsigned char *)CorrectWord;
                  if (!CheckWord (word, ChkrLanguage, ChkrFileDict))
                    AddWord (word, &ChkrFileDict);
                }
              else
                {
                  /* ajouter le mot courant dans le dictionnaire */
                  word = (unsigned char *)ChkrErrWord;
                  if (!CheckWord (word, ChkrLanguage, ChkrFileDict))
                    AddWord (word, &ChkrFileDict);
                }
            }
          break;
        case 3:
          /* ToReplace */
          if (ToReplace)	/* CorrectWord est rempli */
            {		/* et ce n'est pas 1er lancement */
              if (ChkrElement && BadWord[0] != EOS &&
                  !document->DocReadOnly && !ElementIsReadOnly(ChkrElement))
                WordReplace ((unsigned char*)BadWord,
                             (unsigned char*)CorrectWord);
              ToReplace = FALSE;
            }
          break;
        case 4:
          /* ToReplace et maj dictionnaire */
          if (ToReplace)	/* CorrectWord est rempli */
            {		/* et ce n'est pas 1er lancement */
              if (ChkrElement && BadWord[0] != EOS &&
                  !document->DocReadOnly && !ElementIsReadOnly(ChkrElement))
                {
                  WordReplace ((unsigned char*)BadWord,
                               (unsigned char*)CorrectWord);
                  /* mettre CorrectWord dans le dictionnaire */
                  defaultCharset = TtaGetDefaultCharset ();
                  if (defaultCharset == UTF_8)
                    word = TtaConvertMbsToByte ((unsigned char *)CorrectWord,
                                                ISO_8859_1);
                  else
                    word = (unsigned char *)CorrectWord;
                  if (!CheckWord (word, ChkrLanguage, ChkrFileDict))
                    /* mettre ce nouveau mot dans le dictionnaire */
                    AddWord (word, &ChkrFileDict);
                  if (defaultCharset == UTF_8)
                    TtaFreeMemory (word);
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
  CallbackChecker
  ----------------------------------------------------------------------*/
void CallbackChecker (int ref, int dataType, char *data)
{
  PtrElement          pEl1, pElN;
  int                 c1, cN;

  if (ref == -1)
    /* detruit le dialogue du correcteur */
    RemoveSpellForm ();
  else
    switch (ref - SpellingBase)
      {
      case ChkrMenuIgnore:
        switch ((long int) data)
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
        strncpy (RejectedChar, data, MAX_REJECTED_CHARS);
        /* bascule automatiquement l'indicateur IgnoreSpecial */
        if (!IgnoreSpecial)
          {
            IgnoreSpecial = TRUE;
#ifdef _GTK
            TtaSetToggleMenu (SpellingBase + ChkrMenuIgnore, 3, IgnoreSpecial);
#endif /* _GTK */
          }
        break;
      case ChkrMenuOR:
        /* definition du sens de correction OU? */
        switch ((long int) data)
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
            InitSearchDomain ((long int) data, ChkrRange);
            /* On prepare la recheche suivante */
#ifdef _GTK
            if (ChkrRange->SStartToEnd)
              TtaSetMenuForm (SpellingBase + ChkrMenuOR, 2);
            else
              TtaSetMenuForm (SpellingBase + ChkrMenuOR, 0);
#endif /* GTK */
          }
        else if (ChkrRange != NULL)
          /* Est-ce que le document vient de recevoir la selection */
          if (pDocSel != ChkrRange->SDocument)
            {
              /* Est-ce encore vrai */
              GetCurrentSelection (&pDocSel, &pEl1, &pElN, &c1, &cN);
#ifdef _GTK
              if (pDocSel == ChkrRange->SDocument)
                {
                  /* Il faut reactiver les entree */
                  TtaRedrawMenuEntry (SpellingBase + ChkrMenuOR,
                                      0, NULL, (ThotColor)-1, 1);
                  TtaRedrawMenuEntry (SpellingBase + ChkrMenuOR,
                                      1, NULL, (ThotColor)-1, 1);
                  TtaRedrawMenuEntry (SpellingBase + ChkrMenuOR,
                                      2, NULL, (ThotColor)-1, 1);
                } 
#endif /* GTK */
            }
        break;
      case ChkrSelectProp:
        /* retour du selecteur de propositions */
        /* recopier le choix dans CorrectWord */
        strcpy (CorrectWord, data);
        if (CorrectWord[0] != EOS && BadWord != EOS
            && strcmp (CorrectWord, BadWord) != 0)
          ToReplace = TRUE;
        break;
      case ChkrFormCorrect:
        /* retour de la feuille de dialogue CORRECTION */
        /* effectuer l'action demandee */
        ApplyCommand ((long int) data);
        break;
      }
}


/*----------------------------------------------------------------------
  SpellCheckLoadResources loads resouces of the spellchecker
  ----------------------------------------------------------------------*/
void SpellCheckLoadResources ()
{
  if (SpellingBase == 0)
    {
      SpellingBase = TtaSetCallback ((Proc)CallbackChecker, ChkrMaxDialogue);
      /* Initialisation des variables globales */
      ParametrizeChecker ();
      ChkrFileDict = NULL;
      ChkrRange = NULL;
    }
}

