/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * searchmenu.c : diverse document search functions.
 *
 * Authors: I. Vatton (INRIA)
 *          R. Guetari (W3C/INRIA) - Windows version
 *
 */

#ifdef _WX
#include "wx/wx.h"
#endif /* _WX */

#include "thot_gui.h"
#include "thot_sys.h"
#include "dialog.h"
#include "libmsg.h"
#include "message.h"
#include "constmedia.h"
#include "typemedia.h"
#include "fileaccess.h"
#include "interface.h"
#include "appdialogue.h"

#ifdef _WINGUI
#include "resource.h"
#include "wininclude.h"
#endif /* _WINGUI */

#ifdef _WX
#include "wxinclude.h"
#include "wx/msgdlg.h" // wxMessageDialog
#include "message_wx.h"
#endif /* _WX */

#define THOT_EXPORT extern
#include "platform_tv.h"
#include "edit_tv.h"
#include "appdialogue_tv.h"
#include "frame_tv.h"

/* pointer to the search domain for the the current command */
static PtrSearchContext SearchingD = NULL;
/* document to which the CurrRef belongs */
static PtrDocument   CurrRefDoc;
static PtrDocument   DocTextOK;
/* element whose references we are looking for */
static PtrElement    CurrRefElem;
static PtrElement    ElemTextOK;
static char          Caption[200];
/* precedent searched string */
static char          pPrecedentString[MAX_LENGTH];
/* searched string */
static char          SearchedString[MAX_LENGTH];
static CHAR_T       *SString = NULL;
/* length of the searched string */
static int           SStringLen;
/* the replace string */
static char          ReplacingString[MAX_LENGTH];
static CHAR_T       *RString = NULL;
/* length of the replace string */
static int           RStringLen;
/* pointer to the current reference */
static PtrReference  CurrRef;

/* indicating whether there's a character Upper/lower case distinction */
static ThotBool      UpperLower = TRUE;
/* find and replace strings */
static ThotBool      WithReplace;
/* pointer to the external document containing the current reference */
static ThotBool      AutoReplace;
static ThotBool      StartSearch;
static ThotBool      ReplaceDone;
static ThotBool      DoReplace;
static ThotBool      TextOK = FALSE;
static ThotBool      SearchAfter;
static int           FirstCharTextOK;
static int           LastCharTextOK;

#include "absboxes_f.h"
#include "actions_f.h"
#include "appli_f.h"
#include "changeabsbox_f.h"
#include "content_f.h"
#include "createabsbox_f.h"
#include "docs_f.h"
#include "exceptions_f.h"
#include "fileaccess_f.h"
#include "memory_f.h"
#include "references_f.h"
#include "regexp_f.h"
#include "search_f.h"
#include "searchmenu_f.h"
#include "searchref_f.h"
#include "schemas_f.h"
#include "structcreation_f.h"
#include "structmodif_f.h"
#include "structschema_f.h"
#include "structselect_f.h"
#include "tree_f.h"
#include "undo_f.h"
#include "views_f.h"
#include "word_f.h"

#ifdef _WINGUI
static int          iLocation;
static int          iMode;
static ThotWindow   SearchW = NULL;
static char         textToSearch[255];
static char         newText[255];
static ThotBool     SearchEnd;

/*-----------------------------------------------------------------------
  SearchDlgProc
  ------------------------------------------------------------------------*/
LRESULT CALLBACK SearchDlgProc (ThotWindow hwnDlg, UINT msg, WPARAM wParam,
                                LPARAM lParam)
{
  switch (msg)
    {
    case WM_INITDIALOG:
      SearchW = hwnDlg;
      SetWindowText (hwnDlg, Caption);
      SetWindowText (GetDlgItem (hwnDlg, IDC_SEARCHFOR),
                     TtaGetMessage (LIB, TMSG_SEARCH_FOR));
      SetWindowText (GetDlgItem (hwnDlg, IDC_UPPERLOWER),
                     TtaGetMessage (LIB, TMSG_UPPERCASE_EQ_LOWERCASE));
      SetWindowText (GetDlgItem (hwnDlg, IDC_REPLACEGROUP),
                     TtaGetMessage (LIB, TMSG_REPLACE));
      SetWindowText (GetDlgItem (hwnDlg, IDC_REPLACEDBY),
                     TtaGetMessage (LIB, TMSG_REPLACE_BY));
      SetWindowText (GetDlgItem (hwnDlg, IDC_NOREPLACE),
                     TtaGetMessage (LIB, TMSG_NO_REPLACE));
      SetWindowText (GetDlgItem (hwnDlg, IDC_ONREQUEST),
                     TtaGetMessage (LIB, TMSG_REPLACE_ON_REQU));
      SetWindowText (GetDlgItem (hwnDlg, IDC_AUTOMATIC),
                     TtaGetMessage (LIB, TMSG_AUTO_REPLACE));
      SetWindowText (GetDlgItem (hwnDlg, IDC_WHEREGROUP),
                     TtaGetMessage (LIB, TMSG_SEARCH_WHERE));
      SetWindowText (GetDlgItem (hwnDlg, IDC_BEFORE),
                     TtaGetMessage (LIB, TMSG_BEFORE_SEL));
      SetWindowText (GetDlgItem (hwnDlg, IDC_WITHIN),
                     TtaGetMessage (LIB, TMSG_WITHIN_SEL));
      SetWindowText (GetDlgItem (hwnDlg, IDC_AFTER),
                     TtaGetMessage (LIB, TMSG_AFTER_SEL));
      SetWindowText (GetDlgItem (hwnDlg, IDC_WHOLEDOC),
                     TtaGetMessage (LIB, TMSG_IN_WHOLE_DOC));
      SetWindowText (GetDlgItem (hwnDlg, ID_CONFIRM),
                     TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
      SetWindowText (GetDlgItem (hwnDlg, ID_NOREPLACE),
                     TtaGetMessage (LIB, TMSG_DO_NOT_REPLACE));
      SetWindowText (GetDlgItem (hwnDlg, ID_DONE),
                     TtaGetMessage (LIB, TMSG_DONE));
      SetDlgItemText (hwnDlg, IDC_SEARCHEDIT, SearchedString);
      SetDlgItemText (hwnDlg, IDC_REPLACEDIT, ReplacingString);	
      if (SearchAfter)
        {
          CheckRadioButton (hwnDlg, IDC_BEFORE, IDC_WHOLEDOC, IDC_AFTER);
          iLocation = 2;
        }
      else
        {
          CheckRadioButton (hwnDlg, IDC_BEFORE, IDC_WHOLEDOC, IDC_WHOLEDOC);
          iLocation = 3;
        }
      if (AutoReplace)
        {
          iMode = 2;
          CheckRadioButton (hwnDlg, IDC_NOREPLACE, IDC_AUTOMATIC, IDC_AUTOMATIC);
        }
      else if (WithReplace)
        {
          iMode = 1;
          CheckRadioButton (hwnDlg, IDC_NOREPLACE, IDC_AUTOMATIC, IDC_ONREQUEST);
        }
      else
        {
          iMode = 0;
          CheckRadioButton (hwnDlg, IDC_NOREPLACE, IDC_AUTOMATIC, IDC_NOREPLACE);
        }

      /* initialize the ignore case button */
      CheckDlgButton (hwnDlg, IDC_UPPERLOWER, (UpperLower)
                      ? BST_CHECKED : BST_UNCHECKED);

      /* put the focus on the first item */
      SetFocus (GetDlgItem (hwnDlg, IDC_SEARCHEDIT));
      /* if we don't return false, Windows will override our SetFocus */
      return FALSE;
      break;
	    
    case WM_CLOSE:
    case WM_DESTROY:
      SearchW = NULL;
      EndDialog (hwnDlg, ID_DONE);
      break;
      
    case WM_COMMAND:
      if (HIWORD (wParam) == EN_UPDATE)
        {
          switch (LOWORD (wParam))
            {
            case IDC_REPLACEDIT:
              /* if the user types in this box, we'll turn on the
                 replace mode if it wasn't the case at this time */
              if (iMode == 0)
                {
                  iMode = 1;
                  CheckRadioButton (hwnDlg, IDC_NOREPLACE, IDC_AUTOMATIC, IDC_ONREQUEST);
                }
              break;
            }
        }
      switch (LOWORD (wParam))
        {
        case ID_CONFIRM:
          SearchEnd = FALSE;
          GetDlgItemText (hwnDlg, IDC_SEARCHEDIT, textToSearch, sizeof (textToSearch) - 1);
          GetDlgItemText (hwnDlg, IDC_REPLACEDIT, newText, sizeof (newText) - 1);
          if (newText && newText[0] != '\0' && iMode == 0)
            {
              iMode = 1;
              CheckRadioButton (hwnDlg, IDC_NOREPLACE, IDC_AUTOMATIC, IDC_ONREQUEST);
            }
	  
          if (iMode == 1 || iMode == 2) 
            ThotCallback (NumZoneTextReplace, STRING_DATA, newText);
	  
          ThotCallback (NumZoneTextSearch, STRING_DATA, textToSearch);
          ThotCallback (NumMenuReplaceMode, INTEGER_DATA, (char *) iMode);
          ThotCallback (NumMenuOrSearchText, INTEGER_DATA, (char *) iLocation);
          ThotCallback (NumFormSearchText, INTEGER_DATA, (char *) 1);
          if (!SearchEnd && iLocation == 3)
            {
              CheckRadioButton (hwnDlg, IDC_BEFORE, IDC_WHOLEDOC, IDC_AFTER);
              iLocation = 2;
            }
          break;
	  
        case ID_NOREPLACE:
          ThotCallback (NumZoneTextSearch, STRING_DATA, textToSearch);
          ThotCallback (NumMenuReplaceMode, INTEGER_DATA, (char *) 0);
          ThotCallback (NumMenuOrSearchText, INTEGER_DATA, (char *) iLocation);
          ThotCallback (NumFormSearchText, INTEGER_DATA, (char *) 1);
          if (iLocation == 3)
            {
              CheckRadioButton (hwnDlg, IDC_BEFORE, IDC_WHOLEDOC, IDC_AFTER);
              iLocation = 2;
            }
          break;
	  
        case IDCANCEL:
          SearchW = NULL;
          EndDialog (hwnDlg, ID_DONE);
          break;

        case ID_DONE:
          SearchW = NULL;
          ThotCallback (120, 1, NULL);
          EndDialog (hwnDlg, ID_DONE);
          break;
	  
        case IDC_NOREPLACE:
          iMode = 0;
          CheckRadioButton (hwnDlg, IDC_NOREPLACE, IDC_AUTOMATIC, LOWORD (wParam));
          break;
	  
        case IDC_ONREQUEST:
          iMode = 1;
          CheckRadioButton (hwnDlg, IDC_NOREPLACE, IDC_AUTOMATIC, LOWORD (wParam));
          break;
	  
        case IDC_AUTOMATIC:
          iMode = 2;
          CheckRadioButton (hwnDlg, IDC_NOREPLACE, IDC_AUTOMATIC, LOWORD (wParam));
          break;
	  
        case IDC_BEFORE:
          iLocation = 0;
          CheckRadioButton (hwnDlg, IDC_BEFORE, IDC_WHOLEDOC, LOWORD (wParam));
          break;
	  
        case IDC_WITHIN:
          iLocation = 1;
          CheckRadioButton (hwnDlg, IDC_BEFORE, IDC_WHOLEDOC, LOWORD (wParam));
          break;
	  
        case IDC_AFTER:
          iLocation = 2;
          CheckRadioButton (hwnDlg, IDC_BEFORE, IDC_WHOLEDOC, LOWORD (wParam));
          break;
	  
        case IDC_WHOLEDOC:
          iLocation = 3;
          CheckRadioButton (hwnDlg, IDC_BEFORE, IDC_WHOLEDOC, LOWORD (wParam));
          break;
	  
        case	IDC_UPPERLOWER:
          ThotCallback (NumToggleUpperEqualLower, INTEGER_DATA, (char *) 0);
          break;
        }
      break;
    default:
      return FALSE;
    }
  return TRUE;
}

/*-----------------------------------------------------------------------
  CreateSearchDlgWindow
  ------------------------------------------------------------------------*/
void CreateSearchDlgWindow (ThotWindow parent)
{  
  if (SearchW)
    SetFocus (SearchW);
  else
    DialogBox (hInstance, MAKEINTRESOURCE (SEARCHDIALOG), NULL,
               (DLGPROC) SearchDlgProc);
}
#endif /* _WINGUI */

#ifdef _GTK
/*----------------------------------------------------------------------
  InitMenuWhereToSearch 
  inits the "Where to search" submenu.
  ----------------------------------------------------------------------*/
static void InitMenuWhereToSearch (int ref)
{
  int                 i;
  char                string[200];
  
  i = 0;
  sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_BEFORE_SEL));
  i += strlen (&string[i]) + 1;
  sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_WITHIN_SEL));
  i += strlen (&string[i]) + 1;
  sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_AFTER_SEL));
  i += strlen (&string[i]) + 1;
  sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_IN_WHOLE_DOC));
  /* sous-menu Ou` rechercher element vide */
  TtaNewSubmenu (NumMenuOrSearchText, ref, 0,
                 TtaGetMessage (LIB, TMSG_SEARCH_WHERE), 4, string,
                 NULL, 0, FALSE);
  TtaSetMenuForm (NumMenuOrSearchText, 2);
}
#endif /* _GTK */


/*----------------------------------------------------------------------
  ResetSearchInDocument
  cleans up the search domain if it refers a closed document.
  ----------------------------------------------------------------------*/
void ResetSearchInDocument (PtrDocument pDoc)
{
  if (SearchingD && pDoc == SearchingD->SDocument)
    {
      SearchingD->SDocument = NULL;
#ifdef _WINGUI
      EndDialog (SearchW, ID_DONE);
      SearchW = NULL;
#endif /* _WINGUI */
#ifdef _GTK
      TtaDestroyDialogue (NumFormSearchText);
#endif /* _GTK */
    }
}

/*----------------------------------------------------------------------
  CallbackWhereToSearch
  callback handler for the "Where to search" submenu.
  ----------------------------------------------------------------------*/
void CallbackWhereToSearch (int ref, int val)
{
  /* determine le point de depart de la recherche et le point */
  /* de terminaison */
  switch (val)
    {
    case 0:
      /* ElemIsBefore la selection */
      SearchingD->SStartToEnd = FALSE;
      break;
    case 1:
      /* Dans la selection */
      StartSearch = TRUE;
      break;
    case 2:
      /* Apres la selection */
      SearchingD->SStartToEnd = TRUE;
      break;
    case 3:
      /* Dans tout le document */
      StartSearch = TRUE;
      break;
    }
  if (StartSearch)
    InitSearchDomain (val, SearchingD);
}


/*----------------------------------------------------------------------
  TtcSearchText
  launches the command for searching and/or replacing a text,
  an element type and an attribute for the pDoc document.
  ----------------------------------------------------------------------*/
void TtcSearchText (Document document, View view)
{
  PtrDocument         pDocSel;
  PtrElement          pFirstSel;
  PtrElement          pLastSel;
  PtrDocument         pDoc;
  char                string[200];
  int                 firstChar;
  int                 lastChar, i;
  ThotBool            ok;

  pDoc = LoadedDocument[document - 1];
  ok = GetCurrentSelection (&pDocSel, &pFirstSel, &pLastSel, &firstChar, &lastChar);
  if (ok)
    SearchAfter = (pDoc == pDocSel);
  else
    {
      pDocSel = pDoc;
      pFirstSel = pLastSel = pDoc->DocDocElement;
      firstChar = lastChar = 0;
      SearchAfter = FALSE;
    }

  StartSearch = TRUE;
  /* compose le titre du formulaire "Recherche dans le document..." */
  strcpy (Caption, TtaGetMessage (LIB, TMSG_SEARCH_IN));
  strcat (Caption, " ");
  strcat (Caption, pDoc->DocDName);

  /* feuille de dialogue Rechercher texte et structure */
  strcpy (string, TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
  i = strlen (TtaGetMessage (LIB, TMSG_LIB_CONFIRM)) + 1;
  strcpy (string + i, TtaGetMessage (LIB, TMSG_DO_NOT_REPLACE));

#ifdef _GTK
  TtaNewSheet (NumFormSearchText, TtaGetViewFrame (document, view), 
               Caption, 2, string, FALSE, 6, 'L', D_DONE);
  
  /* zone de saisie du texte a` rechercher */
  TtaNewTextForm (NumZoneTextSearch, NumFormSearchText,
                  TtaGetMessage (LIB, TMSG_SEARCH_FOR), 30, 1, FALSE);
  TtaSetTextForm (NumZoneTextSearch, SearchedString);
  
  /* Toggle button "UPPERCASE = lowercase" */
  i = 0;
  sprintf (&string[i], "%s%s", "B",
           TtaGetMessage (LIB, TMSG_UPPERCASE_EQ_LOWERCASE));
  TtaNewToggleMenu (NumToggleUpperEqualLower, NumFormSearchText,
                    NULL, 1, string, NULL, FALSE);
  TtaSetToggleMenu (NumToggleUpperEqualLower, 0, UpperLower);
  
  /* zone de saisie du texte de remplacement */
  TtaNewTextForm (NumZoneTextReplace, NumFormSearchText,
                  TtaGetMessage (LIB, TMSG_REPLACE_BY), 30, 1, FALSE);
  TtaSetTextForm (NumZoneTextReplace, ReplacingString);
  
  /* sous-menu mode de remplacement */
  if (!pDoc->DocReadOnly)
    {
      /* on autorise les remplacement */
      /* attache le sous-menu remplacement */
      i = 0;
      sprintf (&string[i], "%s%s", "B",
               TtaGetMessage (LIB, TMSG_NO_REPLACE));
      i += strlen (&string[i]) + 1;
      sprintf (&string[i], "%s%s", "B",
               TtaGetMessage (LIB, TMSG_REPLACE_ON_REQU));
      i += strlen (&string[i]) + 1;
      sprintf (&string[i], "%s%s", "B",
               TtaGetMessage (LIB, TMSG_AUTO_REPLACE));
      TtaNewSubmenu (NumMenuReplaceMode, NumFormSearchText, 0,
                     TtaGetMessage (LIB, TMSG_REPLACE), 3, string,
                     NULL, 0, FALSE);
      if (WithReplace)
        {
          if (AutoReplace)
            TtaSetMenuForm (NumMenuReplaceMode, 2);
          else
            TtaSetMenuForm (NumMenuReplaceMode, 1);
        }
      else
        TtaSetMenuForm (NumMenuReplaceMode, 0);
    }
  else
    TtaNewLabel (NumMenuReplaceMode, NumFormSearchText, " ");
  
  /* sous-menu Ou` rechercher */
  InitMenuWhereToSearch (NumFormSearchText);
#endif /* _GTK */
  
  WithReplace = FALSE;
  ReplaceDone = FALSE;
  AutoReplace = FALSE;
  strcpy ((char *)pPrecedentString, "");
  
  SearchLoadResources ();
  /* active le formulaire */
  if (!ok)
    /* new activation */
    InitSearchDomain (3, SearchingD);
  SearchingD->SDocument = pDoc;
  TextOK = FALSE;

#ifdef _GTK
  TtaShowDialogue (NumFormSearchText, TRUE);
  if (!ok)
    TtaSetMenuForm (NumMenuOrSearchText, 3);
#endif /* _GTK */

#ifdef _WINGUI
  SearchEnd = FALSE;
  CreateSearchDlgWindow (TtaGetViewFrame (document, view));
#endif /* _WINGUI */

#ifdef _WX
  {
    ThotBool created;
    created = CreateSearchDlgWX( NumFormSearchText,
                                 TtaGetViewFrame (document, view),
                                 Caption, SearchedString, ReplacingString,
                                 WithReplace, UpperLower, SearchAfter);
    if (created)
      TtaShowDialogue ( NumFormSearchText, TRUE );
  }
#endif /* _WX */
}

/*----------------------------------------------------------------------
  CleanSearchContext free all context related to search commands.
  ----------------------------------------------------------------------*/
void CleanSearchContext ()
{
  FreeSearchContext (&SearchingD);
  TtaFreeMemory (SString);
  SString = NULL;
  TtaFreeMemory (RString);
  RString = NULL;
}

/*----------------------------------------------------------------------
  CallbackTextReplace
  callback handler for the text Search/Replace form.
  ref: reference of the dialogue element to process.
  val: value of the dialogue element.
  ----------------------------------------------------------------------*/
void CallbackTextReplace (int ref, int val, char *txt)
{
  PtrElement          pFirstSel, pLastSel;
  PtrDocument         pDocSel, pDoc;
  int                 firstChar, lastChar, aView;
  PtrElement          pCurrEl;
  ThotBool            selectionOK;
  ThotBool            found, stop;
  ThotBool            foundString;
  ThotBool            error;

  error = FALSE;
  switch (ref)
    {
    case NumZoneTextSearch:
      /* Chaine a chercher */
      strcpy (SearchedString, txt);
      if (strcmp (SearchedString, pPrecedentString) != 0)
        {
          ReplaceDone = FALSE;
          strcpy (pPrecedentString, SearchedString);
        }
      /* convert the string if necessary */
      TtaFreeMemory (SString);
      SString = TtaConvertByteToCHAR ((unsigned char*)SearchedString, DialogCharset);
      SStringLen = ustrlen (SString);
      break;
    case NumZoneTextReplace:
      /* Chaine a remplacer */
      if (ReplacingString[0] == EOS && txt[0] != EOS)
        {
          if (!WithReplace && !SearchingD->SDocument->DocReadOnly)
            {
              WithReplace = TRUE;
              DoReplace = TRUE;
#ifdef _GTK
              TtaSetMenuForm (NumMenuReplaceMode, 1);
#endif /* _GTK */
            }
        }
      strncpy (ReplacingString, txt, MAX_LENGTH);
      ReplacingString[MAX_LENGTH-1] = EOS;
      /* convert the string if necessary */
      TtaFreeMemory (RString);
      RString = TtaConvertByteToCHAR ((unsigned char*)ReplacingString, DialogCharset);
      RStringLen = ustrlen (RString);
      /* bascule automatiquement le remplacement */
      break;
    case NumToggleUpperEqualLower:
      if (val == 0)
        /* toggle button UPPERCASE = lowercase */
        UpperLower = !UpperLower;
      break;
    case NumMenuReplaceMode:
      /* sous-menu mode de remplacement */
      if (SearchingD->SDocument == NULL ||
          SearchingD->SDocument->DocReadOnly)
        val = 0;
      switch (val)
        {
        case 0:
          /* Sans remplacement */
          WithReplace = FALSE;
          AutoReplace = FALSE;
          DoReplace = FALSE;
          break;
        case 1:
          /* Remplacement a la demande */
          WithReplace = TRUE;
          AutoReplace = FALSE;
          DoReplace = TRUE;
          break;
        case 2:
          /* Remplacement automatique */
          WithReplace = TRUE;
          AutoReplace = TRUE;
          DoReplace = TRUE;
          break;
        }
      break;
    case NumFormSearchText:
      /* Boutons de la feuille de dialogue */
      if (SearchingD->SDocument == NULL ||
          SearchingD->SDocument->DocSSchema == NULL)
        {
#ifdef _WX
#else /* _WX */
#ifdef _WINGUI
          EndDialog (SearchW, ID_DONE);
          SearchW = NULL;
#endif /* _WINGUI */
#if defined(_GTK) || defined(_WX)
          TtaDestroyDialogue (NumFormSearchText);
#endif /* _GTK || _WX */
          TtaFreeMemory (SString);
          SString = NULL;
          TtaFreeMemory (RString);
          RString = NULL;
          return;
#endif /* _WX */
        }
      if (val == 2 && WithReplace && !StartSearch)
        DoReplace = FALSE;
      else if (val == 0)
        {
#ifdef _WX
          /* the user has clicked on cancel button */
          TtaDestroyDialogue( ref );
#endif /* _WX */
          return;
        }

      GetDocAndView (ActiveFrame, &pDoc, &aView);
      selectionOK = GetCurrentSelection (&pDocSel, &pFirstSel,
                                         &pLastSel, &firstChar, &lastChar);
      if (!selectionOK || pDoc != SearchingD->SDocument)
        {
          SearchingD->SDocument = pDoc;
          if (pDocSel != pDoc)
            {
              pDocSel = pDoc;
              pFirstSel = pLastSel = pDoc->DocDocElement;
              firstChar = lastChar = 0;
              SearchAfter = FALSE;
              selectionOK = FALSE;
              StartSearch = TRUE;
            }
          else if (firstChar == 0 && lastChar == 0 && pFirstSel == pLastSel)
            {
              firstChar = 1;
              lastChar = pFirstSel->ElVolume + 1;
            }
        }
      else if (firstChar == 0 && lastChar == 0 && pFirstSel == pLastSel)
        {
          firstChar = 1;
          lastChar = pFirstSel->ElVolume + 1;
        }
      if (StartSearch)
        pCurrEl = NULL;
      else if (SearchingD->SStartToEnd)
        pCurrEl = pLastSel;
      else
        pCurrEl = pFirstSel;
       
      /* la recherche est demandee, on recupere les parametres */
      if (!error || SStringLen != 0)
        {
          found = FALSE;
          if (SStringLen)
            /* on cherche une chaine de caracteres */
            /* eventuellement, avec remplacement */
            {
              pFirstSel = pCurrEl;
              if (AutoReplace)
                OpenHistorySequence (SearchingD->SDocument, pFirstSel,
                                     pLastSel, NULL, firstChar, lastChar);
              do
                {
                  stop = TRUE;
                  /* on sortira de la boucle si on ne */
                  /* trouve pas le texte cherche' */
                  if (WithReplace && DoReplace && !StartSearch
                      && TextOK
                      && DocTextOK == SearchingD->SDocument
                      && ElemTextOK == pFirstSel
                      && FirstCharTextOK == firstChar
                      && LastCharTextOK == lastChar)
                    {
                      /* il faut faire un remplacement et on est sur le */
                      /* texte cherche' */
                      /* on ne remplace pas dans un sous-arbre en */
                      /* lecture seule */
                      if (ElementIsReadOnly (pFirstSel))
                        ;
                      else if (!pFirstSel->ElIsCopy && pFirstSel->ElText != NULL
                               && pFirstSel->ElTerminal
                               && pFirstSel->ElLeafType == LtText)
                        /* on ne remplace pas dans une copie */
                        {
                          found = TRUE;
                          if (found)
                            {
                              /* register the editing operation for Undo
                                 command) */
                              if (!AutoReplace)
                                OpenHistorySequence (SearchingD->SDocument,
                                                     pFirstSel, pFirstSel, NULL,
                                                     firstChar, firstChar + SStringLen);
                              AddEditOpInHistory (pFirstSel,
                                                  SearchingD->SDocument,
                                                  TRUE, TRUE);
                              if (!AutoReplace)
                                CloseHistorySequence (SearchingD->SDocument);
                              /* effectue le remplacement du texte */
                              ReplaceString (SearchingD->SDocument, pFirstSel,
                                             firstChar, SStringLen,
                                             RString, RStringLen,
                                             (ThotBool)(!AutoReplace));
                              ReplaceDone = TRUE;
                              StartSearch = FALSE;
                              if (AutoReplace)
                                lastChar += RStringLen - SStringLen;
                              /* met eventuellement a jour la borne de */
                              /* fin du domaine de recherche */
                              if (pFirstSel == SearchingD->SEndElement &&
                                  /* la borne est dans l'element ou` on a */
                                  /* fait le remplacement */
                                  SearchingD->SEndChar > 1)
                                /* la borne n'est pas a la fin de */
                                /* l'element, on decale la borne */
                                SearchingD->SEndChar += RStringLen - SStringLen;
                              /* recupere les parametres de la nouvelle */
                              /* chaine */
                              if (!AutoReplace)
                                selectionOK = GetCurrentSelection (&pDocSel,
                                                                   &pFirstSel, &pLastSel,
                                                                   &firstChar, &lastChar);
                            }
                        }
                    }

                  do
                    {
                      /*Recherche de la prochaine occurence du texte cherche'*/
                      if (pFirstSel == NULL)
                        {
                          /* debut de recherche */
                          if (SearchingD->SStartToEnd)
                            {
                              pFirstSel = SearchingD->SStartElement;
                              firstChar = SearchingD->SStartChar;
                              if (pFirstSel == NULL)
                                pFirstSel = SearchingD->SDocument->DocDocElement;
                            }
                          else
                            {
                              pFirstSel = SearchingD->SEndElement;
                              firstChar = SearchingD->SEndChar;
                            }
                        }
                      else if (SearchingD->SStartToEnd)
                        {
                          pFirstSel = pLastSel;
                          firstChar = lastChar;
                        }
		      
                      if (SearchingD->SStartToEnd)
                        {
                          pLastSel = SearchingD->SEndElement;
                          lastChar = SearchingD->SEndChar;
                        }
                      else
                        {
                          pLastSel = SearchingD->SStartElement;
                          lastChar = SearchingD->SStartChar;
                        }
                      found = SearchText (SearchingD->SDocument,
                                          &pFirstSel, &firstChar, &pLastSel,
                                          &lastChar, SearchingD->SStartToEnd,
                                          UpperLower, SString,
                                          SStringLen);
                      foundString = found;
                      if (found)
                        /* on a trouve' la chaine cherchee */
                        stop = FALSE;
                    }
                  while (foundString && !found);
		  
                  if (found)
                    {
                      /* on a trouve la chaine recherchee dans le bon type */
                      /* d'element et avec le bon attribut */
                      if (!AutoReplace)
                        {
                          /* selectionne la chaine trouvee */
                          SelectStringWithEvent (SearchingD->SDocument,
                                                 pFirstSel, firstChar,
                                                 lastChar);
                          /* arrete la boucle de recherche */
                          stop = TRUE;
                        }
                      TextOK = TRUE;
                      DocTextOK = SearchingD->SDocument;
                      ElemTextOK = pFirstSel;
                      FirstCharTextOK = firstChar;
                      LastCharTextOK = lastChar;
                    }
                  else
                    {
                      TextOK = FALSE;
                      stop = TRUE;
                    }
                  StartSearch = FALSE;
                }
              while (!stop);
              if (AutoReplace)
                CloseHistorySequence (SearchingD->SDocument);
            }
          if (found)
            {
              /* on a trouve' et selectionne'. */
#ifdef _GTK
              if (!AutoReplace)
                {
                  /* On prepare la recherche suivante */
                  if (SearchingD->SStartToEnd) /* After selection */
                    TtaSetMenuForm (NumMenuOrSearchText, 2);
                  else /* Before selection */
                    TtaSetMenuForm (NumMenuOrSearchText, 0);
                }
#endif /* _GTK */
              StartSearch = FALSE;
            }
          else
            {
              /* not found */
#ifdef _WINGUI
              if (!SearchEnd)
                {
                  SearchEnd = TRUE;
                  if (WithReplace && ReplaceDone)
                    {
                      if (!AutoReplace)
                        MessageBox (NULL,
                                    TtaGetMessage (LIB, TMSG_NOTHING_TO_REPLACE),
                                    Caption, MB_OK | MB_ICONEXCLAMATION);
                    }
                  else
                    MessageBox (NULL, TtaGetMessage (LIB, TMSG_NOT_FOUND),
                                Caption, MB_OK | MB_ICONEXCLAMATION);
                }
#endif /* _WINGUI */
#ifdef _GTK      
              if (WithReplace && ReplaceDone)
                {
                  if (!AutoReplace)
                    TtaDisplayMessage (CONFIRM,
                                       TtaGetMessage (LIB, TMSG_NOTHING_TO_REPLACE),
                                       NULL);
                }
              else
                TtaDisplayMessage (CONFIRM,
                                   TtaGetMessage (LIB, TMSG_NOT_FOUND),
                                   NULL);
#endif /* _GTK */
#ifdef _WX
              WX_SearchResult = 0;
              if (WithReplace && ReplaceDone)
                {
                  if (!AutoReplace)
                      WX_SearchResult = 1; // Nothing to replace
                }
              else
                WX_SearchResult = 2; // Not found
#endif /* _WX */
              StartSearch = TRUE;
            }
        }
      break;
    default:
      break;
    }
}


/*----------------------------------------------------------------------
  SearchLoadResources
  inits the variables of the search commands.
  ----------------------------------------------------------------------*/
void SearchLoadResources (void)
{
  if (ThotLocalActions[T_searchtext] == NULL)
    {
      /* Connecte les actions liees au traitement du search */
      TteConnectAction (T_searchtext, (Proc) CallbackTextReplace);
      TteConnectAction (T_locatesearch, (Proc) CallbackWhereToSearch);
      CurrRef = NULL;
      CurrRefDoc = NULL;
      CurrRefElem = NULL;
      GetSearchContext (&SearchingD);
      SearchedString[0] = EOS;
      SStringLen = 0;
      UpperLower = TRUE;
      WithReplace = FALSE;
      ReplacingString[0] = EOS;
      RStringLen = 0;
    }
}
