/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
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
static ThotBool      WithReplace = FALSE;
/* pointer to the external document containing the current reference */
static ThotBool      AutoReplace = FALSE;
static ThotBool      StartSearch = TRUE;
static ThotBool      ReplaceDone;
static ThotBool      DoReplace = FALSE;
static ThotBool      TextOK = FALSE;
static ThotBool      SearchAfter = TRUE;
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


/*----------------------------------------------------------------------
  ResetSearchInDocument
  cleans up the search domain if it refers a closed document.
  ----------------------------------------------------------------------*/
void ResetSearchInDocument (PtrDocument pDoc)
{
  if (SearchingD && pDoc == SearchingD->SDocument)
    SearchingD->SDocument = NULL;
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
      DoReplace = FALSE;
      AutoReplace = FALSE;
      ReplacingString[0] = EOS;
      RStringLen = 0;
    }
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
  ThotBool            ok, created;

  pDoc = LoadedDocument[document - 1];
  if (SearchingD && SearchingD->SDocument == pDoc)
    {
      TtcNextSearchReplace (document, view);
      return;
    }

  SearchLoadResources ();
  ok = GetCurrentSelection (&pDocSel, &pFirstSel, &pLastSel, &firstChar, &lastChar);
  if (!ok)
    {
      pDocSel = pDoc;
      pFirstSel = pLastSel = pDoc->DocDocElement;
      firstChar = lastChar = 0;
      ok = GetCurrentSelection (&pDocSel, &pFirstSel, &pLastSel, &firstChar, &lastChar);
      if (!ok)
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
  ReplaceDone = FALSE;
  strcpy ((char *)pPrecedentString, ""); 
  /* active le formulaire */
  if (SearchAfter)
    /* new activation */
    InitSearchDomain (2, SearchingD);
  else
    /* new activation */
    InitSearchDomain (3, SearchingD);
  SearchingD->SDocument = pDoc;
  TextOK = FALSE;
  created = CreateSearchDlgWX( NumFormSearchText,
                               TtaGetViewFrame (document, view),
                               Caption, SearchedString, ReplacingString,
                               WithReplace, UpperLower, SearchAfter);
  if (created)
    TtaShowDialogue ( NumFormSearchText, TRUE, TRUE);
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
  TtcCloseSearchReplace simulate the confirm button
  ----------------------------------------------------------------------*/
void TtcCloseSearchReplace (Document document, View view)
{
  PtrDocument         pDoc;

  pDoc = LoadedDocument[document - 1];
  if (SearchingD && SearchingD->SDocument == pDoc)
    {
      CallbackTextReplace (NumFormSearchText, 0, NULL);
      StartSearch = FALSE;
    }
}

/*----------------------------------------------------------------------
  TtcNextSearchReplace simulate the confirm button
  ----------------------------------------------------------------------*/
void TtcNextSearchReplace (Document document, View view)
{
  PtrDocument         pDoc;

  pDoc = LoadedDocument[document - 1];
  if (SearchingD && SearchingD->SDocument == pDoc)
    {
      CallbackTextReplace (NumFormSearchText, 1, NULL);
      StartSearch = FALSE;
    }
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
      if (val == 2 && WithReplace && !StartSearch)
        DoReplace = FALSE;
      else if (val == 0)
        {
          /* the user has clicked on cancel button */
          TtaDestroyDialogue( ref );
          SearchingD->SDocument = NULL;
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
          else
            {
              if (SearchingD->SStartToEnd)
                {
                  SearchingD->SEndElement = pLastSel;
                  SearchingD->SEndChar = lastChar;
                }
              else
                {
                  SearchingD->SStartElement = pLastSel;
                  SearchingD->SStartChar = lastChar;
                }
            }
          if (firstChar == 0 && lastChar == 0 && pFirstSel == pLastSel)
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
      if (StartSearch && SearchingD->SWholeDocument)
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
            /* on a trouve' et selectionne'. */
            StartSearch = FALSE;
          else
            {
              /* not found */
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

