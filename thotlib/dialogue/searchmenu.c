/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2001
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * searchmenu.c : diverse document search functions.
 *
 * Authors: I. Vatton (INRIA)
 *          R. Guetari (W3C/INRIA) - Unicode and Windows version
 *
 */

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
#ifdef _WINDOWS
#include "resource.h"
#include "wininclude.h"
#endif /* _WINDOWS */

#define THOT_EXPORT extern
#include "platform_tv.h"
#include "edit_tv.h"
#include "appdialogue_tv.h"

/* pointer to the search domain for the the current command */
static PtrSearchContext SearchingD = NULL;
/* document where we are searching a page */
static PtrDocument  SearchedPageDoc;
/* document to which the CurrRef belongs */
static PtrDocument  CurrRefDoc;
/* document to which belongs the element we are looking for */
static PtrDocument  CurrRefElemDoc;
static PtrDocument  DocTextOK;

/* root of the tree where we are searching a page */
static PtrElement   SearchedPageRoot;
/* element whose references we are looking for */
static PtrElement   CurrRefElem;
static PtrElement   ElemTextOK;

/* document view for which we are searching a page */
static int          ViewSearchedPageDoc;
/* presentation scheme view for which we are searching a page */
static int          SearchedPageSchView;
/* number of the searched page */
static int          SearchedPageNumber;
/* precedent searched string */
static char           pPrecedentString[THOT_MAX_CHAR];
/* searched string */
static char           pSearchedString[THOT_MAX_CHAR];
/* length of the searched string */
static int          SearchedStringLen;
/* the replace string */
static char           pReplaceString[THOT_MAX_CHAR];
/* length of the replace string */
static int          ReplaceStringLen;
/* pointer to the current reference */
static PtrReference CurrRef;

/* indicating whether there's a character Upper/lower case distinction */
static ThotBool     UpperLower = TRUE;
/* find and replace strings */
static ThotBool     WithReplace;
/* pointer to the external document containing the current reference */
static PtrExternalDoc pExtCurrDoc;
static ThotBool     SearchReferenceEnd;
static ThotBool     AutoReplace;
static ThotBool     StartSearch;
static ThotBool     ReplaceDone;
static ThotBool     DoReplace;
static ThotBool     TextOK = FALSE;
static ThotBool     SearchAfter;
static int          FirstCharTextOK;
static int          LastCharTextOK;
static int          ReturnValueSelectReferMenu;

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
#include "viewcommands_f.h"
#include "views_f.h"
#include "word_f.h"

#ifdef _WINDOWS
static int          iLocation;
static int          iMode;
static ThotWindow   SearchW = NULL;
static char         textToSearch [255];
static char         newText [255];
static char         msgCaption [200];
static ThotBool     searchEnd;

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
      SetWindowText (hwnDlg, msgCaption);
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
      SetDlgItemText (hwnDlg, IDC_SEARCHEDIT, pSearchedString);
      SetDlgItemText (hwnDlg, IDC_REPLACEDIT, pReplaceString);	
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
	  searchEnd = FALSE;
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
	  if (!searchEnd && iLocation == 3)
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
void        CreateSearchDlgWindow (ThotWindow parent)
{  
  if (SearchW)
    SetFocus (SearchW);
  else
    DialogBox (hInstance, MAKEINTRESOURCE (SEARCHDIALOG), NULL, (DLGPROC) SearchDlgProc);
}
#else /* _WINDOWS */
/*----------------------------------------------------------------------
  InitMenuWhereToSearch 
  inits the "Where to search" submenu.
  ----------------------------------------------------------------------*/
static void         InitMenuWhereToSearch (int ref)
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
		 NULL, FALSE);
  TtaSetMenuForm (NumMenuOrSearchText, 2);
}


/*----------------------------------------------------------------------
  ActivateMenuWhereToSearch
  activates the "Where to search" submenu.
  ----------------------------------------------------------------------*/
static void         ActivateMenuWhereToSearch ()
{
   TtaRedrawMenuEntry (NumMenuOrSearchText, 0, NULL, -1, 1);
   TtaRedrawMenuEntry (NumMenuOrSearchText, 1, NULL, -1, 1);
   TtaRedrawMenuEntry (NumMenuOrSearchText, 2, NULL, -1, 1);
}
#endif /* !_WINDOWS */


/*----------------------------------------------------------------------
  ResetSearchInDocument
  cleans up the search domain if it refers a closed document.
  ----------------------------------------------------------------------*/
void         ResetSearchInDocument (PtrDocument pDoc)
{
  if (SearchingD && pDoc == SearchingD->SDocument)
    {
      SearchingD->SDocument = NULL;
#ifdef _WINDOWS
      EndDialog (SearchW, ID_DONE);
      SearchW = NULL;
#else /* _WINDOWS */
      TtaDestroyDialogue (NumFormSearchText);
#endif /* _WINDOWS */
    }
}

/*----------------------------------------------------------------------
  CallbackWhereToSearch
  callback handler for the "Where to search" submenu.
  ----------------------------------------------------------------------*/
void                CallbackWhereToSearch (int ref, int val)
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
  SearchEmptyElem
  searches for the first empty element in the domain specified by 
  pContent, starting from (and including) pCurrEl.
  If it found such an element, it selections this element and returns
  TRUE.
  ----------------------------------------------------------------------*/
static ThotBool SearchEmptyElem (PtrElement pCurrEl, PtrSearchContext pContext)
{
   PtrElement          pEl;
   PtrElement          pParent;
   PtrElement          pNextEl;
   int                 i;
   ThotBool            found;
   ThotBool            ok;

   found = FALSE;
   ok = FALSE;
   if (pCurrEl == NULL)
      /* debut de recherche */
      if (pContext->SStartToEnd)
	{
	   pEl = pContext->SStartElement;
	   if (pEl == NULL)
	      pEl = pContext->SDocument->DocDocElement;
	}
      else
	 pEl = pContext->SEndElement;
   else
      pEl = pCurrEl;
   while (pEl != NULL && !found)
     {
	if (pContext->SStartToEnd)
	   /* Recherche en avant */
	   pEl = FwdSearchRefOrEmptyElem (pEl, 1);
	else
	   /* Recherche en arriere */
	   pEl = BackSearchRefOrEmptyElem (pEl, 1);
	if (pEl != NULL)
	   /* on a trouve' un element vide, on verifie que cet element ne */
	   /* fait pas partie d'une inclusion et n'est pas cache' a */
	   /* l'utilisateur */
	  {
	     pParent = pEl;
	     while (pParent->ElParent != NULL && pParent->ElSource == NULL)
		pParent = pParent->ElParent;
	     if (pParent->ElSource == NULL)
		/* on n'est pas dans une inclusion */
		if (!ElementIsHidden (pEl))
		   /* l'element n'est pas cache' a l'utilisateur */
		   found = TRUE;
	  }
     }

   if (pEl != NULL && found)
      /* on a trouve' */
     {
	/* l'element trouve' est pointe' par pEl */
	if (pContext->SStartToEnd)
	  {
	     if (pContext->SEndElement != NULL)
		/* il faut s'arreter avant l'extremite' du document */
		if (pEl != pContext->SEndElement)
		   /*l'element trouve' n'est pas l'element ou il faut s'arreter */
		   if (ElemIsBefore (pContext->SEndElement, pEl))
		      /* l'element trouve' est apres l'element de fin, on */
		      /* fait comme si on n'avait pas trouve' */
		      pEl = NULL;
	  }
	else if (pContext->SStartElement != NULL)
	   /* il faut s'arreter avant l'extremite' du document */
	   if (pEl != pContext->SStartElement)
	      /*l'element trouve' n'est pas l'element ou il faut s'arreter */
	      if (ElemIsBefore (pEl, pContext->SStartElement))
		 /* l'element trouve' est avant le debut du domaine, on */
		 /* fait comme si on n'avait pas trouve' */
		 pEl = NULL;
	if (pEl != NULL)
	   SelectElementWithEvent (pContext->SDocument, pEl, TRUE, FALSE);
     }
   ok = (pEl != NULL);
   if (!found)
      if (pContext->SWholeDocument)
	 /* il faut rechercher dans tout le document */
	 /* cherche l'arbre a traiter apres celui ou` on n'a pas trouve' */
	 if (NextTree (&pNextEl, &i, pContext))
	    /* il y a un autre arbre a traiter, on continue avec le */
	    /* debut de ce nouvel arbre */
	    ok = SearchEmptyElem (pNextEl, pContext);
#ifndef _WINDOWS
   if (ok) /* On prepare la recherche suivante */
     {
      if (SearchingD->SStartToEnd)
         TtaSetMenuForm (NumMenuOrSearchText, 2);	/* After selection */
      else
           TtaSetMenuForm (NumMenuOrSearchText, 0);	/* Before selection */
     }
#endif /* !_WINDOWS */
    return ok;
}


/*----------------------------------------------------------------------
  SearchEmptyRefer
  searches for the first empty reference in the domain specified by 
  pContent, starting from (and including) pCurrEl.
  If it found such an element, it selections this element and returns
  TRUE.
  ----------------------------------------------------------------------*/
static ThotBool SearchEmptyRefer (PtrElement pCurrEl, PtrSearchContext pContext)
{
   PtrElement          pEl;
   PtrElement          pParent;
   PtrElement          pNextEl;
   int                 i;
   ThotBool            found;
   ThotBool            ok;

   found = FALSE;
   ok = FALSE;
   if (pCurrEl == NULL)
      /* debut de recherche */
      if (pContext->SStartToEnd)
	{
	   pEl = pContext->SStartElement;
	   if (pEl == NULL)
	      pEl = pContext->SDocument->DocDocElement;
	}
      else
	 pEl = pContext->SEndElement;
   else
      pEl = pCurrEl;
   while (!found && pEl != NULL)
     {
	if (pContext->SStartToEnd)
	   /* Recherche en avant */
	   pEl = FwdSearchRefOrEmptyElem (pEl, 2);
	else
	   /* Recherche en arriere */
	   pEl = BackSearchRefOrEmptyElem (pEl, 2);
	if (pEl != NULL)
	   /* on a trouve' une reference vide, on verifie que cette */
	   /* reference ne fait pas partie d'une inclusion ou d'un arbre */
	   /* cache' a l'utilisateur */
	  {
	     pParent = pEl;
	     while (pParent->ElParent != NULL && pParent->ElSource == NULL)
		pParent = pParent->ElParent;
	     if (pParent->ElSource == NULL)
		/* on n'est pas dans une inclusion */
		if (!ElementIsHidden (pEl))
		  {
		   /* l'element n'est pas cache' a l'utilisateur */
		   if (pEl->ElReference == NULL)
		      /* la reference est vide */
		      found = TRUE;
		   else
		      /* la reference n'est pas vide */
		   if (pEl->ElReference->RdReferred == NULL)
		      /* la reference est vide */
		      found = TRUE;
		   else if (pEl->ElReference->RdInternalRef)
		      if (!pEl->ElReference->RdReferred->ReExternalRef)
			 if (IsASavedElement (pEl->ElReference->RdReferred->ReReferredElem))
			    /* l'element reference' est dans le buffer des */
			    /* elements coupe's */
			    found = TRUE;
		  }
	  }
     }
   if (pEl != NULL && found)
      /* on a trouve' */
     {
	/* l'element trouve' est pointe' par pEl */
	if (pContext->SStartToEnd)
	  {
	     if (pContext->SEndElement != NULL)
		/* il faut s'arreter avant l'extremite' du document */
		if (pEl != pContext->SEndElement)
		   /*l'element trouve' n'est pas l'element ou il faut s'arreter */
		   if (ElemIsBefore (pContext->SEndElement, pEl))
		      /* l'element trouve' est apres l'element de fin, on */
		      /* fait comme si on n'avait pas trouve' */
		      pEl = NULL;
	  }
	else if (pContext->SStartElement != NULL)
	   /* il faut s'arreter avant l'extremite' du document */
	   if (pEl != pContext->SStartElement)
	      /*l'element trouve' n'est pas l'element ou il faut s'arreter */
	      if (ElemIsBefore (pEl, pContext->SStartElement))
		 /* l'element trouve' est avant le debut du domaine, on */
		 /* fait comme si on n'avait pas trouve' */
		 pEl = NULL;
	if (pEl != NULL)
	   SelectElementWithEvent (pContext->SDocument, pEl, TRUE, FALSE);
     }
   ok = (pEl != NULL);
   if (!found)
      if (pContext->SWholeDocument)
	 /* il faut rechercher dans tout le document */
	 /* cherche l'arbre a traiter apres celui ou` on n'a pas trouve' */
	 if (NextTree (&pNextEl, &i, pContext))
	    /* il y a un autre arbre a traiter, on continue avec le */
	    /* debut de ce nouvel arbre */
	    ok = SearchEmptyRefer (pNextEl, pContext);
#ifndef _WINDOWS
   if (ok) /* On prepare la recherche suivante */
     {
      if (SearchingD->SStartToEnd) /* After selection */
         TtaSetMenuForm (NumMenuOrSearchText, 2);
      else /* Before selection */
           TtaSetMenuForm (NumMenuOrSearchText, 0);
     }
#endif /* !_WINDOWS */
   return ok;
}


/*----------------------------------------------------------------------
  CallbackSearchEmptyEl
  callback handler for the "Search Empty Elements" form.
  ref: reference of the dialogue element to process.
  val: value of the dialogue element.
  ----------------------------------------------------------------------*/
void                CallbackSearchEmptyEl (int ref, int val)
{
  ThotBool            ok;
  PtrElement          pCurrEl;
  PtrDocument         pDocSel;
  PtrElement          pFirstSel;
  PtrElement          pLastSel;
  int                 firstChar;
  int                 lastChar;

  if (SearchingD->SDocument != NULL)
    if (SearchingD->SDocument->DocSSchema != NULL)
      /* le document concerne' est toujours la */
      /* feuille de dialogue Rechercher element vide */
      if (val == 1)
	/* lancer la recherche */
	{
	  if (StartSearch)
	    {
	      pCurrEl = NULL;
	      StartSearch = FALSE;
	    }
	  else
	    {
	      ok = GetCurrentSelection (&pDocSel, &pFirstSel, &pLastSel, &firstChar, &lastChar);
	      if (!ok || pDocSel != SearchingD->SDocument)
		/* la selection a change' de document, on refuse */
		{
		  StartSearch = TRUE;
		  return;
		}
	      else if (SearchingD->SStartToEnd)
		pCurrEl = pLastSel;
	      else
		pCurrEl = pFirstSel;
	    }
	  ok = SearchEmptyElem (pCurrEl, SearchingD);

	  if (ok)
	    {
	      /* on a trouve' et selectionne' */
	      StartSearch = FALSE;
#ifndef _WINDOWS
	      /* on reactive les entrees du sous-menu "Ou chercher" */
	      ActivateMenuWhereToSearch ();
	      /* efface le message "Pas trouve'" dans le formulaire */
	      TtaNewLabel (NumLabelEmptyElemNotFound, NumFormSearchEmptyElement, " ");
#endif /* !_WINDOWS */
	    }
	  else
	    /* on n'a pas trouve' */
	    {
	      /* message 'Pas trouve' dans le formulaire */
#ifndef _WINDOWS 
	      TtaNewLabel (NumLabelEmptyElemNotFound, NumFormSearchEmptyElement, TtaGetMessage (LIB, TMSG_NOT_FOUND));
#endif /* !_WINDOWS */
	      StartSearch = TRUE;
	    }
	}
}


/*----------------------------------------------------------------------
  TtcSearchEmptyElement
  launches the search empty elements command for the pDoc document.
  ----------------------------------------------------------------------*/
void                TtcSearchEmptyElement (Document document, View view)
{
   ThotBool            ok;
   PtrDocument         pDocSel;
   PtrElement          pFirstSel;
   PtrElement          pLastSel;
   int                 firstChar;
   int                 lastChar;
   char                  buffTitle[200];
   PtrDocument         pDoc;

   pDoc = LoadedDocument[document - 1];
   SearchLoadResources ();

   ok = GetCurrentSelection (&pDocSel, &pFirstSel, &pLastSel, &firstChar, &lastChar);
   if (!ok)
     {
	pDocSel = pDoc;
	pFirstSel = pDoc->DocDocElement;
     }
   if (pDocSel != pDoc)
      /* pas de selection dans le document d'ou vient la commande */
      return;

   /* fait disparaitre les autres formulaires de recherche qui sont affiches */
   TtaDestroyDialogue (NumFormSearchEmptyReference);
   TtaDestroyDialogue (NumFormSearchText);

   StartSearch = TRUE;
   /* feuille de dialogue Rechercher element vide */
   /* compose le titre "Recherche dans le document..." */
   strcpy (buffTitle, TtaGetMessage (LIB, TMSG_SEARCH_IN));
   strcat (buffTitle, pDoc->DocDName);
#ifndef _WINDOWS
   TtaNewSheet (NumFormSearchEmptyElement, TtaGetViewFrame (document, view),
		buffTitle, 1, TtaGetMessage (LIB, TMSG_LIB_CONFIRM), FALSE,
		3, 'L', D_DONE);
   /* label indiquant la recherche d'elements vides */
   TtaNewLabel (NumLabelDocSearcheEmptyElement, NumFormSearchEmptyElement,
		TtaGetMessage (LIB, TMSG_SEARCH_EMPTY_EL)); 
   InitMenuWhereToSearch (NumFormSearchEmptyElement);

   /* le message "Pas trouve'" */
   TtaNewLabel (NumLabelEmptyElemNotFound, NumFormSearchEmptyElement,
		" ");
   /* active le formulaire */
   TtaShowDialogue (NumFormSearchEmptyElement, TRUE);
#endif /* !_WINDOWS */
   if (!ok)
     {
	InitSearchDomain (3, SearchingD);
#ifndef _WINDOWS
	TtaSetMenuForm (NumMenuOrSearchText, 3);
#endif /* !_WINDOWS */
     }
   SearchingD->SDocument = pDoc;
}


/*----------------------------------------------------------------------
  CallbackSearchEmptyref
  callback handler for the Search Empty reference menu.
  ref: reference of the dialogue element to process.
  val: value of the dialogue element.
  ----------------------------------------------------------------------*/
void                CallbackSearchEmptyref (int ref, int val)
{
   ThotBool            ok;
   PtrElement          pCurrEl;
   PtrElement          pFirstSel;
   PtrElement          pLastSel;
   PtrDocument         pDocSel;
   int                 firstChar;
   int                 lastChar;

   if (SearchingD->SDocument != NULL)
      if (SearchingD->SDocument->DocSSchema != NULL)
	{
	   /* feuille de dialogue Rechercher reference vide */
	   if (val == 1)
	     {
		/* lancer la recherche */
		if (StartSearch)
		  {
		     pCurrEl = NULL;
		     StartSearch = FALSE;
		  }
		else
		  {
		     ok = GetCurrentSelection (&pDocSel, &pFirstSel, &pLastSel, &firstChar, &lastChar);
		     if (!ok || pDocSel != SearchingD->SDocument)
			/* la selection a change' de document, on refuse */
		       {
			  StartSearch = TRUE;
			  return;
		       }
		     else if (SearchingD->SStartToEnd)
			pCurrEl = pLastSel;
		     else
			pCurrEl = pFirstSel;
		  }
		ok = SearchEmptyRefer (pCurrEl, SearchingD);
		if (ok)
		  {
		     StartSearch = FALSE;
		     /* on a trouve' et selectionne'. On reactive les */
		     /* entrees du sous-menu "Ou chercher" */
#ifndef _WINDOWS
		     ActivateMenuWhereToSearch ();
		     /* efface le message 'Pas trouve' dans la feuille de */
		     /* saisie */
		     TtaNewLabel (NumLabelEmptyRefereneceNotFound,
				  NumFormSearchEmptyReference, " ");
#endif /* !_WINDOWS */
		  }
		else
		   /* on n'a pas trouve' */
		  {
#ifndef _WINDOWS
		     /* message 'Pas trouve' dans la feuille de saisie */
		     TtaNewLabel (NumLabelEmptyRefereneceNotFound,
				  NumFormSearchEmptyReference,
				  TtaGetMessage (LIB, TMSG_NOT_FOUND));
#endif /* !_WINDOWS */
		     StartSearch = TRUE;
		  }
	     }
	   else
	      TtaDestroyDialogue (NumFormSearchEmptyReference);
	}
}


/*----------------------------------------------------------------------
  TtcSearchEmptyReference
  launches the search empty references command for the pDoc document.
  ----------------------------------------------------------------------*/
void                TtcSearchEmptyReference (Document document, View view)
{
   ThotBool            ok;
   PtrDocument         pDocSel;
   PtrElement          pFirstSel;
   PtrElement          pLastSel;
   int                 firstChar;
   int                 lastChar;
   char                  buffTitle[200];
   PtrDocument         pDoc;

   pDoc = LoadedDocument[document - 1];
   SearchLoadResources ();

   ok = GetCurrentSelection (&pDocSel, &pFirstSel, &pLastSel, &firstChar, &lastChar);
   if (!ok)
     {
	pDocSel = pDoc;
	pFirstSel = pDoc->DocDocElement;
     }

   if (pDocSel != pDoc)
      /* pas de selection dans le document d'ou vient la commande */
      return;

   /* fait disparaitre les autres formulaires de recherche qui sont affiches */
   TtaDestroyDialogue (NumFormSearchEmptyElement);
   TtaDestroyDialogue (NumFormSearchText);

   StartSearch = TRUE;
   /* compose le titre "Recherche dans le document..." */
   strcpy (buffTitle, TtaGetMessage (LIB, TMSG_SEARCH_IN));
   strcat (buffTitle, pDoc->DocDName);
   /* feuille de dialogue Rechercher reference vide */
#ifndef _WINDOWS
   TtaNewSheet (NumFormSearchEmptyReference, TtaGetViewFrame (document, view),
		buffTitle, 1, TtaGetMessage (LIB, TMSG_LIB_CONFIRM), FALSE,
		3, 'L', D_DONE);

   /* label indiquant la recherche de references vides */
   TtaNewLabel (NumLabelDocSearchEmptyRef, NumFormSearchEmptyReference,
		TtaGetMessage (LIB, TMSG_SEARCH_EMPTY_REF));
   InitMenuWhereToSearch (NumFormSearchEmptyReference);

   /* le message "Pas trouve'" */
   TtaNewLabel (NumLabelEmptyRefereneceNotFound, NumFormSearchEmptyReference,
		" ");
   /* active le formulaire */
   TtaShowDialogue (NumFormSearchEmptyReference, TRUE);
#endif /* !_WINDOWS */
   if (!ok)
     {
	InitSearchDomain (3, SearchingD);
#ifndef _WINDOWS 
	TtaSetMenuForm (NumMenuOrSearchText, 3);
#endif /* !_WINDOWS */
     }
   SearchingD->SDocument = pDoc;
}


/*----------------------------------------------------------------------
  CallbackReferenceMenu
  ----------------------------------------------------------------------*/
void                CallbackReferenceMenu (int val)
{
   ReturnValueSelectReferMenu = val;
}

/*----------------------------------------------------------------------
  BuildReferenceMenu
  ----------------------------------------------------------------------*/
void   BuildReferenceMenu (char *bufMenu, int nbEntries, int *selEntry)
{
   char                bufMenuB[MAX_TXT_LEN];
   char               *src;
   char               *dest;
   int                 k, l;

   ReturnValueSelectReferMenu = 0;
   /* ajoute 'B' au debut de chaque entree */
   dest = &bufMenuB[0];
   src = &bufMenu[0];
   for (k = 1; k <= nbEntries; k++)
     {
	strcpy (dest, "B");
	dest++;
	l = strlen (src);
	strcpy (dest, src);
	dest += l + 1;
	src += l + 1;
     }
   TtaNewPopup (NumMenuReferenceChoice, 0, TtaGetMessage (LIB, TMSG_LINK), nbEntries, bufMenuB, NULL, 'L');
#ifndef _WINDOWS
   TtaSetDialoguePosition ();
   TtaShowDialogue (NumMenuReferenceChoice, FALSE);
   /* attend que l'utilisateur ait repondu au menu et que le */
   /* mediateur ait appele' CallbackReferenceMenu */
   TtaWaitShowDialogue ();
#endif /* !_WINDOWS */
   *selEntry = ReturnValueSelectReferMenu;
}

/*----------------------------------------------------------------------
  SearchAReference
  ----------------------------------------------------------------------*/
static void         SearchAReference (ThotBool docExtNext)
{
   char                  msg[100];

   /* chercher une reference */
   FindReference (&CurrRef, &CurrRefDoc, &CurrRefElem,
		  &CurrRefElemDoc, &pExtCurrDoc, docExtNext);
   if (CurrRef != NULL)
      /* on a trouve', on efface le message qui traine */
      strcpy (msg, " ");
   else if (pExtCurrDoc != NULL)
      /* references dans un document non charge' */
     {
	strcpy (msg, TtaGetMessage (LIB, TMSG_REF_IN_DOC));
	GetDocName (pExtCurrDoc->EdDocIdent, msg + strlen (msg));
     }
   else
     {
	/* on n'a trouve' aucune reference */
	SearchReferenceEnd = TRUE;
	strcpy (msg, TtaGetMessage (LIB, TMSG_NOT_FOUND));
     }
#ifndef _WINDOWS
   TtaNewLabel (NumLabelReferenceNotFound, NumFormSearchReference, msg);
#endif /* !_WINDOWS */
}


/*----------------------------------------------------------------------
  BuildSearchReferences
  launches the command to search the references for the selected
  element of pDoc.
  ----------------------------------------------------------------------*/
void                BuildSearchReferences (PtrDocument pDoc)
{
   PtrElement          pEl;
   PtrElement          pFirstSel;
   PtrElement          pLastSel;
   PtrDocument         pDocSel;
   int                 firstChar;
   int                 lastChar;
   char                  bufText[200];
   ThotBool            found;

   if (GetCurrentSelection (&pDocSel, &pFirstSel, &pLastSel, &firstChar, &lastChar))
     {
	/* initialise les variables de recherche des references */
	CurrRef = NULL;
	CurrRefElem = NULL;
	pExtCurrDoc = NULL;
	/* cherche le premier ascendant reference' de la selection */
	pEl = pFirstSel;
	found = FALSE;
	do
	  {
	     if (pEl->ElReferredDescr != NULL)
		/* l'element a un descripteur d'element reference' */
		if (pEl->ElReferredDescr->ReFirstReference != NULL ||
		    pEl->ElReferredDescr->ReExtDocRef != NULL)
		   /* l'element est effectivement reference' */
		   found = TRUE;
	     if (!found)
		/* l'element n'est pas reference', on passe au pere */
		pEl = pEl->ElParent;
	  }
	while (!found && pEl != NULL);
	if (found)
	   /* il y a effectivement un ascendant reference' */
	  {
	     SearchReferenceEnd = FALSE;
	     /* initialise le label du formulaire en y mettant le type de */
	     /* l'element reference' */
	     strcpy (bufText, TtaGetMessage (LIB, TMSG_SEARCH_REF_TO_EL));
	     strcat (bufText, " ");
	     strcat (bufText, pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrName);
#ifndef _WINDOWS
	     TtaNewLabel (NumLabelSearchReference, NumFormSearchReference,
			  bufText);
	     TtaNewLabel (NumLabelReferenceNotFound, NumFormSearchReference,
			  " ");
	     /* active le formulaire de recherche */
	     TtaShowDialogue (NumFormSearchReference, FALSE);
#endif /* !_WINDOWS */
	  }
     }
}

/*----------------------------------------------------------------------
  TtcSearchReference
  ----------------------------------------------------------------------*/
void                TtcSearchReference (Document document, View view)
{
   PtrDocument         pDoc;
   char                  BufMenu[100];
   int                 i;

   pDoc = LoadedDocument[document - 1];
   SearchLoadResources ();

   if (pDoc != NULL)
     {
	/* feuille de dialogue pour la recherche des references a` un element */
	strcpy (BufMenu, TtaGetMessage (LIB, TMSG_SEARCH));
	i = strlen (TtaGetMessage (LIB, TMSG_SEARCH)) + 1;
	strcpy (BufMenu + i, TtaGetMessage (LIB, TMSG_OPEN));
#ifndef _WINDOWS
	TtaNewSheet (NumFormSearchReference, TtaGetViewFrame (document, view),
		     TtaGetMessage (LIB, TMSG_SEARCH_REF), 2, BufMenu, TRUE,
		     1, 'L', D_DONE);
	/* label indiquant le type d'element dont on cherche les references */
	TtaNewLabel (NumLabelSearchReference, NumFormSearchReference,
		     TtaGetMessage (LIB, TMSG_SEARCH_REF_TO_EL));

	/* label "Pas trouve'" pour Recherche des references a` un element */
	TtaNewLabel (NumLabelReferenceNotFound, NumFormSearchReference,
		     " ");
#endif /* !_WINDOWS */
	BuildSearchReferences (pDoc);
     }
}


/*----------------------------------------------------------------------
  CallbackReferenceTo
  callback handler for the "Search reference to' dialogue.
  ----------------------------------------------------------------------*/
void                CallbackReferenceTo (int ref, int data)
{
   PtrDocument         pDoc;

   if (ref == NumFormSearchReference && !SearchReferenceEnd)
      switch (data)
	    {
	       case 1:
		  if (CurrRefDoc != NULL)
		     if (CurrRefDoc->DocSSchema == NULL)
			/* le document ou se trouvait la reference courante a */
			/* ete ferme'. On repart du debut */
		       {
			  CurrRef = NULL;
			  CurrRefElem = NULL;
			  pExtCurrDoc = NULL;
		       }
		  SearchAReference (TRUE);
		  break;

	       case 2:
		  /* charger le document contenant la reference cherchee */
		  if (CurrRef == NULL && pExtCurrDoc != NULL)
		     /* il s'agit bien d'une reference dans un document
			externe */
		     /* non charge' */
		     if (!DocIdentIsNull (pExtCurrDoc->EdDocIdent))
		       {
			 /* charge le document */
			 LoadDocument (&pDoc, pExtCurrDoc->EdDocIdent);
			 if (pDoc != NULL)
			   {
			     /* efface le label */
#ifndef _WINDOWS
			     TtaNewLabel (NumLabelReferenceNotFound,
					  NumFormSearchReference, " ");
#endif /* !_WINDOWS */
			     SearchAReference (FALSE);
			   }
		       }
		  break;
	       default:
		  TtaDestroyDialogue (ref);
		  break;
	    }
}


/*----------------------------------------------------------------------
  TtcSearchText
  launches the command for searching and/or replacing a text,
  an element type and an attribute for the pDoc document.
  ----------------------------------------------------------------------*/
void                TtcSearchText (Document document, View view)
{
  PtrDocument         pDocSel;
  PtrElement          pFirstSel;
  PtrElement          pLastSel;
  PtrDocument         pDoc;
  char                bufTitle[200], string[200];
  int                 firstChar;
  int                 lastChar, i;
  ThotBool            ok;

  pDoc = LoadedDocument[document - 1];
  ok = GetCurrentSelection (&pDocSel, &pFirstSel, &pLastSel, &firstChar, &lastChar);
  if (ok && pDoc != pDocSel)
    SearchAfter = FALSE;
  if (!ok)
    {
      pDocSel = pDoc;
      pFirstSel = pLastSel = pDoc->DocDocElement;
      firstChar = lastChar = 0;
    }

  /* fait disparaitre les autres formulaires de recherche qui sont affiches */
#ifndef _WINDOWS
  TtaDestroyDialogue (NumFormSearchEmptyElement);
  TtaDestroyDialogue (NumFormSearchEmptyReference);
#endif /* _WINDOWS */
  StartSearch = TRUE;
  
  /* compose le titre du formulaire "Recherche dans le document..." */
  strcpy (bufTitle, TtaGetMessage (LIB, TMSG_SEARCH_IN));
  strcat (bufTitle, " ");
  strcat (bufTitle, pDoc->DocDName);
#ifdef _WINDOWS
  strcpy (msgCaption, bufTitle);
#endif /* _WINDOWS */
  /* feuille de dialogue Rechercher texte et structure */
  strcpy (string, TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
  i = strlen (TtaGetMessage (LIB, TMSG_LIB_CONFIRM)) + 1;
  strcpy (string + i, TtaGetMessage (LIB, TMSG_DO_NOT_REPLACE));
#ifndef _WINDOWS
  TtaNewSheet (NumFormSearchText, TtaGetViewFrame (document, view), 
	       bufTitle, 2, string, FALSE, 6, 'L', D_DONE);
  
  /* zone de saisie du texte a` rechercher */
  TtaNewTextForm (NumZoneTextSearch, NumFormSearchText,
		  TtaGetMessage (LIB, TMSG_SEARCH_FOR), 30, 1, FALSE);
  TtaSetTextForm (NumZoneTextSearch, pSearchedString);
  
  /* Toggle button "UPPERCASE = lowercase" */
  i = 0;
  sprintf (&string[i], "%s%s", "B",
	   TtaGetMessage (LIB, TMSG_UPPERCASE_EQ_LOWERCASE));
  TtaNewToggleMenu (NumToggleUpperEqualLower, NumFormSearchText,
		    NULL, 1, string, NULL, FALSE);
  TtaSetToggleMenu (NumToggleUpperEqualLower, 0, UpperLower);
  
  /* zone de saisie du texte de remplacement */
  TtaNewTextForm (NumZoneTextReplace, NumFormSearchText,
		  TtaGetMessage (LIB, TMSG_REPLACE_BY), 30, 1, TRUE);
  TtaSetTextForm (NumZoneTextReplace, pReplaceString);
  
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
		     NULL, FALSE);
      if (WithReplace) {
	if (AutoReplace)
          TtaSetMenuForm (NumMenuReplaceMode, 2);
	else
	  TtaSetMenuForm (NumMenuReplaceMode, 1);
      } else
	TtaSetMenuForm (NumMenuReplaceMode, 0);
    }
  else
    TtaNewLabel (NumMenuReplaceMode, NumFormSearchText, " ");
  
  /* sous-menu Ou` rechercher */
  InitMenuWhereToSearch (NumFormSearchText);
#endif /* _WINDOWS */
  
  WithReplace = FALSE;
  ReplaceDone = FALSE;
  AutoReplace = FALSE;
  strcpy (pPrecedentString, "");
  
#ifdef _WINDOWS
  SearchAfter = ok;
#else /* _WINDOWS */
  /* efface le label "References dans le document X" */
  TtaNewLabel (NumLabelAttributeValue, NumFormSearchText, " ");
#endif /* _WINDOWS */
  SearchLoadResources ();
  /* complete la feuille de dialogue avec les menus de recherche de types */
  /* d'element et d'attributs si la ressource de recherche avec structure */
  /* est chargee */
  if (ThotLocalActions[T_strsearchconstmenu] != NULL)
    (*ThotLocalActions[T_strsearchconstmenu]) (pDoc);
  
  /* active le formulaire */
  if (!ok)
    /* new activation */
    InitSearchDomain (3, SearchingD);
  SearchingD->SDocument = pDoc;
  TextOK = FALSE;
#ifndef _WINDOWS 
  TtaShowDialogue (NumFormSearchText, TRUE);
  if (!ok)
    TtaSetMenuForm (NumMenuOrSearchText, 3);
#else  /* _WINDOWS */
  searchEnd = FALSE;
  CreateSearchDlgWindow (TtaGetViewFrame (document, view));
#endif /* _WINDOWS */
}


/*----------------------------------------------------------------------
  CallbackTextReplace
  callback handler for the text Search/Replace form.
  ref: reference of the dialogue element to process.
  val: value of the dialogue element.
  ----------------------------------------------------------------------*/
void CallbackTextReplace (int ref, int val, char *txt)
{
  PtrElement          pFirstSel;
  PtrElement          pLastSel;
  PtrDocument         pDocSel;
  int                 firstChar;
  int                 lastChar;
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
      strcpy (pSearchedString, txt);
      if (strcmp (pSearchedString, pPrecedentString) != 0)
	{
	  ReplaceDone = FALSE;
	  strcpy (pPrecedentString, pSearchedString);
	}
      SearchedStringLen = strlen (pSearchedString);
      break;
    case NumZoneTextReplace:
      /* Chaine a remplacer */
      strcpy (pReplaceString, txt);
      ReplaceStringLen = strlen (pReplaceString);
      /* bascule automatiquement le remplacement */
      if (!WithReplace && !SearchingD->SDocument->DocReadOnly)
	{
	  WithReplace = TRUE;
	  DoReplace = TRUE;
#ifndef _WINDOWS
	  TtaSetMenuForm (NumMenuReplaceMode, 1);
#endif /* !_WINDOWS */
	}
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
#ifndef _WINDOWS
      TtaNewLabel (NumLabelAttributeValue, NumFormSearchText, " ");
#endif /* !_WINDOWS */
      if (SearchingD->SDocument == NULL)
	{
	  TtaDestroyDialogue (NumFormSearchText);
	  return;
	}
      if (SearchingD->SDocument->DocSSchema == NULL)
	return;
      if (val == 2 && WithReplace && !StartSearch)
	DoReplace = FALSE;
      else if (val == 0)
	/* Abandon de la recherche */
	return;

      selectionOK = GetCurrentSelection (&pDocSel, &pFirstSel,
					 &pLastSel, &firstChar, &lastChar);
      if (!selectionOK || pDocSel != SearchingD->SDocument)
	{
	  pFirstSel = NULL;
	  pLastSel = NULL;
	  firstChar = 0;
	  lastChar = 0;
	  selectionOK = FALSE;
	  StartSearch = TRUE;
	}

      if (StartSearch)
	pCurrEl = NULL;
      else if (SearchingD->SStartToEnd)
	pCurrEl = pLastSel;
      else
	pCurrEl = pFirstSel;
       
       /* la recherche est demandee, on recupere les parametres */
      if (ThotLocalActions[T_strsearchgetparams] != NULL)
	(*ThotLocalActions[T_strsearchgetparams]) (&error, SearchingD);
      if (!error || SearchedStringLen != 0)
	{
	  found = FALSE;
	  if (SearchedStringLen == 0)
	    {
	      if (ThotLocalActions[T_strsearchonly] != NULL)
		(*ThotLocalActions[T_strsearchonly]) (pCurrEl, SearchingD,
						      &found);
	    }
	  else
	    /* on cherche une chaine de caracteres */
	    /* eventuellement, avec remplacement */
	    {
	      pFirstSel = pCurrEl;
	      if (AutoReplace)
		OpenHistorySequence (SearchingD->SDocument, pFirstSel,
				     pLastSel, firstChar, lastChar);
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
#ifndef _WINDOWS
		      TtaNewLabel (NumLabelAttributeValue, NumFormSearchText,
				   TtaGetMessage (LIB, TMSG_EL_RO))
#endif /* !_WINDOWS */
			;
		    else if (!pFirstSel->ElIsCopy && pFirstSel->ElText != NULL
			     && pFirstSel->ElTerminal
			     && pFirstSel->ElLeafType == LtText)
		      /* on ne remplace pas dans une copie */
		      {
			found = TRUE;
			if (ThotLocalActions[T_strsearcheletattr] != NULL)
			  (*ThotLocalActions[T_strsearcheletattr]) (pFirstSel,
								    &found);
			if (found)
			  {
			    /* register the editing operation for Undo
			       command) */
			    if (!AutoReplace)
			      OpenHistorySequence (SearchingD->SDocument,
					       pFirstSel, pFirstSel, firstChar,
					       firstChar+SearchedStringLen-1);
			    AddEditOpInHistory (pFirstSel,
						SearchingD->SDocument,
						TRUE, TRUE);
			    if (!AutoReplace)
			      CloseHistorySequence (SearchingD->SDocument);
			    /* effectue le remplacement du texte */
			    ReplaceString (SearchingD->SDocument, pFirstSel,
					   firstChar, SearchedStringLen,
					   pReplaceString, ReplaceStringLen,
					   (ThotBool)(!AutoReplace));
			    ReplaceDone = TRUE;
			    StartSearch = FALSE;
			    /* met eventuellement a jour la borne de */
			    /* fin du domaine de recherche */
			    if (pFirstSel == SearchingD->SEndElement)
			      /* la borne est dans l'element ou` on a */
			      /* fait le remplacement */
			      if (SearchingD->SEndChar != 0)
				/* la borne n'est pas a la fin de */
				/* l'element, on decale la borne */
				SearchingD->SEndChar +=
                                          ReplaceStringLen - SearchedStringLen;
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
					  UpperLower, pSearchedString,
					  SearchedStringLen);
		      if (found)
			lastChar--;
		      
		      foundString = found;
		      if (found)
			/* on a trouve' la chaine cherchee */
			{
			  stop = FALSE;
			  if (ThotLocalActions[T_strsearcheletattr] != NULL)
			    (*ThotLocalActions[T_strsearcheletattr])(pFirstSel,
								     &found);
			}
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
			  lastChar++;
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
		      if (SearchingD->SWholeDocument)
			/* il faut rechercher dans tout le document */
			/* cherche l'arbre a traiter apres celui ou` on */
			/* n'a pas trouve' */
			if (NextTree (&pFirstSel, &firstChar, SearchingD))
			  {
			    stop = FALSE;
			    pLastSel = pFirstSel;
			    lastChar = 0;
			  }
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
#ifndef _WINDOWS
	     if (!AutoReplace)
	       {
		 /* On prepare la recherche suivante */
		 if (SearchingD->SStartToEnd) /* After selection */
		   TtaSetMenuForm (NumMenuOrSearchText, 2);
		 else /* Before selection */
		   TtaSetMenuForm (NumMenuOrSearchText, 0);
	       }
#endif /* !_WINDOWS */
	     StartSearch = FALSE;
	     if (ThotLocalActions[T_strsearchshowvalattr] != NULL)
	       (*ThotLocalActions[T_strsearchshowvalattr]) ();
	    }
	  else
	    /* on n'a pas trouve' */
	    {
	      if (WithReplace && ReplaceDone)
		{
		  if (!AutoReplace)
		    {
#ifdef _WINDOWS
		      if (!searchEnd)
			{
			  searchEnd = TRUE;
			  MessageBox (NULL,
				 TtaGetMessage (LIB, TMSG_NOTHING_TO_REPLACE),
                                 msgCaption, MB_OK | MB_ICONEXCLAMATION);
			}
#else /* !_WINDOWS */
		      /* message "Plus de remplacement" */
		      TtaNewLabel (NumLabelAttributeValue, NumFormSearchText,
				   TtaGetMessage (LIB, TMSG_NOTHING_TO_REPLACE));
#endif /* !_WINDOWS */
		    }
		}
	      else
		/* message "Pas trouve'" */
#ifdef _WINDOWS
		if (!searchEnd)
		  {
		    searchEnd = TRUE;
		    MessageBox (NULL, TtaGetMessage (LIB, TMSG_NOT_FOUND),
				msgCaption, MB_OK | MB_ICONEXCLAMATION);
		  }
#else  /* !_WINDOWS */
	      TtaNewLabel (NumLabelAttributeValue, NumFormSearchText,
			   TtaGetMessage (LIB, TMSG_NOT_FOUND));
#endif /* !_WINDOWS */
	      StartSearch = TRUE;
	    }
	}
      break;
    default:
      if (ThotLocalActions[T_strsearchretmenu] != NULL)
	(*ThotLocalActions[T_strsearchretmenu]) (ref, val, txt, SearchingD);
      break;
    }
}

/*----------------------------------------------------------------------
  BuildGoToPageMenu
  handles the Goto Page number command.
  ----------------------------------------------------------------------*/
void BuildGoToPageMenu (PtrDocument pDoc, int docView, int schView,
			ThotBool assoc)
{
   char                  buffTitle[200];

   if (ThotLocalActions[T_searchpage] == NULL)
      TteConnectAction (T_searchpage, (Proc) CallbackGoToPageMenu);

   /* garde le pointeur sur le document concerne' par la commande */
   SearchedPageDoc = pDoc;
   /* garde le  numero de vue (dans le document) de la vue concernee */
   if (assoc)
      ViewSearchedPageDoc = 1;
   else
      ViewSearchedPageDoc = docView;
   /* garde la racine de l'arbre ou on va chercher une page, ainsi que */
   /* le numero (dans le schema de presentation) de la vue concernee */
   if (assoc)
      /* c'est une vue d'elements associes */
     {
	/* les elements associes n'ont qu'une vue, la vue 1 */
	SearchedPageSchView = 1;
	SearchedPageRoot = pDoc->DocAssocRoot[docView - 1];
     }
   else
      /* c'est une vue de l'arbre principal */
     {
	/* cherche le numero de vue dans le schema de presentation */
	/* applique' au document */
	SearchedPageSchView = AppliedView (pDoc->DocDocElement, NULL, pDoc,
					   docView);
	SearchedPageRoot = pDoc->DocDocElement;
     }
   /* compose le titre "Recherche dans le document..." */
   strcpy (buffTitle, TtaGetMessage (LIB, TMSG_SEARCH_IN));
   strcat (buffTitle, pDoc->DocDName);
#ifndef _WINDOWS 
   /* cree formulaire de saisie du numero de la page cherchee */
   TtaNewSheet (NumFormSearchPage,  0, buffTitle, 1,
		TtaGetMessage (LIB, TMSG_LIB_CONFIRM), TRUE, 1, 'L',
		D_CANCEL);

   /* cree zone de saisie du numero de la page cherchee */
   TtaNewNumberForm (NumZoneSearchPage, NumFormSearchPage, TtaGetMessage (LIB, TMSG_GOTO_PAGE), 0, 9999, FALSE);
   /* affiche le formulaire */
   TtaShowDialogue (NumFormSearchPage, FALSE);
#endif /* !_WINDOWS */
}

/*----------------------------------------------------------------------
  CallbackGoToPageMenu
  callback handler for the GotoPage menu.
  ----------------------------------------------------------------------*/
void CallbackGoToPageMenu (int ref, int val)
{
   PtrElement          pPage;

   if (SearchedPageDoc != NULL)
      if (SearchedPageDoc->DocSSchema != NULL)
	 /* le document concerne' est toujours la */
	 switch (ref)
	       {
		  case NumZoneSearchPage:
		     /* zone de saisie du numero de la page cherchee */
		     SearchedPageNumber = val;
		     break;
		  case NumFormSearchPage:
		     /* formulaire de saisie du numero de la page cherchee */
		     /* cherche la page */
		     pPage = SearchPageBreak (SearchedPageRoot,
					      SearchedPageSchView,
					      SearchedPageNumber, FALSE);
		     /* fait afficher la page trouvee en haut de sa frame */
		     ScrollPageToTop (pPage, ViewSearchedPageDoc,
				      SearchedPageDoc);
		     TtaDestroyDialogue (NumFormSearchPage);
		     break;
	       }
}

/*----------------------------------------------------------------------
  SearchLoadResources
  inits the variables of the search commands.
  ----------------------------------------------------------------------*/
void                SearchLoadResources (void)
{
   if (ThotLocalActions[T_searchtext] == NULL)
     {
	/* Connecte les actions liees au traitement du search */
	TteConnectAction (T_searchtext, (Proc) CallbackTextReplace);
	TteConnectAction (T_locatesearch, (Proc) CallbackWhereToSearch);
        
	TteConnectAction (T_searchemptyref, (Proc) CallbackSearchEmptyref);
	TteConnectAction (T_searchemptyelt, (Proc) CallbackSearchEmptyEl);
	TteConnectAction (T_searchrefto, (Proc) CallbackReferenceTo);
	CurrRef = NULL;
	CurrRefDoc = NULL;
	CurrRefElem = NULL;
	pExtCurrDoc = NULL;
	GetSearchContext (&SearchingD);
	pSearchedString[0] = EOS;
	SearchedStringLen = 0;
	UpperLower = TRUE;
	WithReplace = FALSE;
	pReplaceString[0] = EOS;
	ReplaceStringLen = 0;
     }
}
