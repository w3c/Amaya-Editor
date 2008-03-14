/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2007
 *
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *
 * libmanag.c
 *
 * This module contains functions to handle Amaya SVG Library
 * Authors :
 *         E. Bonnet (INRIA) - GTK combo box & svglib
 */

/* header file */
#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"
#include "trans.h"
#include "view.h"
#include "content.h"
#include "init_f.h"
#include "zlib.h"
#include "profiles.h"
#include "SVG.h"
#include "MENUconf.h"

#include "fileaccess.h"
#include "AHTURLTools_f.h"
#include "css_f.h"
#include "EDITORactions_f.h"
#include "EDITimage_f.h"
#include "EDITstyle_f.h"
#include "fetchXMLname_f.h"
#include "XLinkedit_f.h"

#include "HTMLactions_f.h"
#include "HTMLbook_f.h"
#include "HTMLedit_f.h"
#include "HTMLhistory_f.h"
#include "HTMLimage_f.h"
#include "HTMLsave_f.h"
#include "HTMLpresentation_f.h"
#include "HTMLtable_f.h"
#include "html2thot_f.h"
#include "Mathedit_f.h"
#include "SVGedit_f.h"
#include "SVGbuilder_f.h"
#include "MENUconf_f.h"
#include "query_f.h"
#include "styleparser_f.h"
#include "trans_f.h"
#include "transparse_f.h"
#include "UIcss_f.h"


/* Useful headers */
#include "libmanag.h" 
#include "libmanag_f.h"

#ifdef _SVG
/* Variable to handle Use Library Interface (context of png selection)*/
Document	LibraryDocDocument = 0;
static Element	LibraryDocElement = NULL;
static char     LibURL[MAX_LENGTH];

/* Structure List URI-Title */
typedef struct _Library_URITitle
{
  char          *URI;
  char          *Title;
  int            indice;
  ThotBool       customLibrary;
  struct         _Library_URITitle *next;
} ListUriTitle;
/* List header */
static ListUriTitle *HeaderListUriTitle = NULL;

/* variable to handle SVGLibrary dialogue */
static char SaveLibraryTitleSelection[MAX_LENGTH];
static char NewLibraryTitle[MAX_LENGTH];
static char LastURLCatalogue[MAX_LENGTH];

/* box size and position values */
static int x_box = 10000;
static int y_box = 10000;
static int width_box = 0;
static int height_box = 0;
#endif /* _SVG */

#ifdef _WINDOWS
	#include <commctrl.h>
#endif /* _WINDOWS */

#ifdef _SVG
/*----------------------------------------------------------------------
  AddLibraryDataIntoStructure
  Add a new structure url title into a dynamic liste :
  the Library Manager Structure List
  parameters: url and title
  persLib : if persLib is FALSE the Library identified by URL is delivered 
  with Amaya distribution, else if it's a custom Library.
  ----------------------------------------------------------------------*/
static void AddLibraryDataIntoStructure (ThotBool persLib, char *url, char *title)
{
/* Utiliser un tableau plut�t qu'une liste chain�e */
/* ensuite modifier test sur les lignes du tableau avec un indice (Cf. FrameTable)*/
/*!!!!!!modifier le nom des variables */
  ListUriTitle     *listCur, *listNext, *listNew; /* *curCel, *nextCel, *newCel*/
  char             *path, *Title;
  int               index = 1;

  path = (char *) TtaGetMemory (strlen (url) + 1);
  strcpy (path, url);
  Title = CreateUniqueLibraryTitle (title);
  if (HeaderListUriTitle)
    {
      /* list exist */
      listNext = HeaderListUriTitle;
      while (listNext)
	{
	  listCur = listNext;
	  /* A EFFECTUER test pour savoir si l'url est deja placer dans la liste */
	  /* il n'est pas necessaire d'ajouter deux fois la m�me librairie dans la liste!!!*/
	  listNext = listNext->next;
	  index++;
	}
      listNew = (ListUriTitle *) TtaGetMemory (sizeof (ListUriTitle));
      listNew->next = NULL;
      listNew->URI = path;
      listNew->Title = Title;
      listNew->indice = index;
      if (persLib)
	listNew->customLibrary = TRUE;
      else
	listNew->customLibrary = FALSE;
      listCur->next = listNew;
    }
  else
    {
      /* list didn't exist, create it */
      HeaderListUriTitle = (ListUriTitle *) TtaGetMemory (sizeof (ListUriTitle));
      HeaderListUriTitle->next = NULL;
      HeaderListUriTitle->URI = path;
      HeaderListUriTitle->Title = Title;
      HeaderListUriTitle->indice = index;
      if (persLib)
	HeaderListUriTitle->customLibrary = TRUE;
      else
	HeaderListUriTitle->customLibrary = FALSE;
    }
}
#endif /* _SVG */

#ifdef _WINGUI
/*-----------------------------------------------------------------------
 PasteLibraryModelDlgProc
 ------------------------------------------------------------------------*/
LRESULT CALLBACK PasteLibraryModelDlgProc (ThotWindow hwnDlg, UINT msg,
					   WPARAM wParam, LPARAM lParam)
{
#ifdef _SVG
    switch (msg)
      {
      case WM_INITDIALOG:
	/*SVGLibHwnd = hwnDlg;*/
	SetWindowText (hwnDlg, TtaGetMessage (AMAYA, AM_SVGLIB_DIALOG1));
	SetWindowText (GetDlgItem (hwnDlg, ID_COPYSVGLIB),
		TtaGetMessage (AMAYA, AM_SVGLIB_COPY_SELECTION));
	SetWindowText (GetDlgItem (hwnDlg, ID_REFERSVGLIB), 
		TtaGetMessage (AMAYA, AM_SVGLIB_REF_SELECTION));
	SetWindowText (GetDlgItem (hwnDlg, IDCANCEL), TtaGetMessage (LIB, TMSG_CANCEL));
	break;
      case WM_COMMAND:
	switch (LOWORD (wParam))
	  {
	  case ID_COPYSVGLIB:
	    EndDialog (hwnDlg, ID_COPYSVGLIB);
	    ThotCallback (BaseLibrary + FormLibrary, INTEGER_DATA, (char*) 1);
	    break;
	  case ID_REFERSVGLIB:
	    EndDialog (hwnDlg, ID_REFERSVGLIB);
	    ThotCallback (BaseLibrary + FormLibrary, INTEGER_DATA, (char*) 2);
	    break;

	  case IDCANCEL:
	    ThotCallback (BaseLibrary + FormLibrary, INTEGER_DATA, (char*) 0);
	    EndDialog (hwnDlg, IDCANCEL);
	    break;
	  }
	break;
      default:
	return FALSE;
      }
#endif /* _SVG */
    return TRUE;
}

/*-----------------------------------------------------------------------
 AddNewModelIntoLibraryDlgProc
 ------------------------------------------------------------------------*/
LRESULT CALLBACK AddNewModelIntoLibraryDlgProc (ThotWindow hwnDlg, UINT msg,
						WPARAM wParam, LPARAM lParam)
{
#ifdef _SVG
int    iListIndex = 0;
int    iCurrItem = -1;
int    length = 0;
HWND   hwndLibraryListCtrl; /* handle the library title list */

 hwndLibraryListCtrl = GetDlgItem (hwnDlg, IDC_LIBRARYLIST);
 switch (msg)
   {
   case WM_INITDIALOG:
     SetWindowText (hwnDlg, TtaGetMessage (AMAYA, AM_SVGLIB_ADD_SVG_MODEL_DIALOG));
     SetWindowText (GetDlgItem (hwnDlg, IDC_LIBRARYTITLE),
		    TtaGetMessage (AMAYA, AM_SVGLIB_CATALOGUE_TITLE));
     SetWindowText (GetDlgItem (hwnDlg, ID_CONFIRM),
		    TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
     SetWindowText (GetDlgItem (hwnDlg, ID_CREATELIBRARY), 
		    TtaGetMessage (AMAYA, AM_SVGLIB_CREATE_NEW_CATALOGUE));
     SetWindowText (GetDlgItem (hwnDlg, IDCANCEL), TtaGetMessage (LIB, TMSG_CANCEL));

     /* Reset library list content */
     SendDlgItemMessage (hwnDlg, IDC_LIBRARYLIST, LB_RESETCONTENT, 0, 0);
     /* Set library list variable to handle text */
     SendDlgItemMessage (hwnDlg, IDC_LIBRARYLIST, LBS_HASSTRINGS, 0, 0);

     /*SendMessage (hwndLibraryListCtrl, LBS_HASSTRING, 0, 0);*/
     for (iListIndex = 0; iListIndex < nbTitle; ++iListIndex)
       {
	 /* Insert code here to retrieve the name of each library title, and then 
	    put the library title in Title. */
	 /* Add the title string to the list control. */
	 SendDlgItemMessage (hwnDlg, IDC_LIBRARYLIST, LB_INSERTSTRING/*LB_ADDSTRING*/, iListIndex, 
			     (LPARAM) &listTitle[length]);
	 length += strlen (&listTitle[length]) + 1;
         /*Set a 32-bit value, (LPARAM) iListIndex, that is associated
           with the newly added item in the list control.*/
	 /*SendMessage (hwndLibraryListCtrl, LB_SETITEMDATA,      
	   (WPARAM) iCurrItem, (LPARAM) iListIndex);*/
       }

     /* Select the first library title in the list control. */
     SendDlgItemMessage (hwnDlg, IDC_LIBRARYLIST, LB_SETCURSEL, 0, 0);
     return TRUE;
     break;
   case WM_COMMAND:
     switch (LOWORD (wParam))
       {
       case ID_CONFIRM:
	 /* Retrieve the index of the currently selected library.*/
	 if ((iCurrItem = SendDlgItemMessage (hwnDlg, IDC_LIBRARYLIST, LB_GETCURSEL, 
					      0, 0)) != LB_ERR)
	   {
	     /*length = SendDlgItemMessage (hwnDlg, IDC_LIBRARYLIST, LB_GETTEXTLEN, 
	       iCurrItem, 0);*/
	     length = SendDlgItemMessage (hwnDlg, IDC_LIBRARYLIST, LB_GETTEXT,
					  iCurrItem, (LPARAM) szBuffer);
	     ThotCallback (BaseLibrary + SVGLibCatalogueTitle, STRING_DATA, (char *) szBuffer);
	     ThotCallback (BaseLibrary + AddSVGModel, INTEGER_DATA, (char *) 1);
	     EndDialog (hwnDlg, 0);
	   }
	 break;

       case ID_CREATELIBRARY:
	 ThotCallback (BaseLibrary + AddSVGModel, INTEGER_DATA, (char *) 2);
	 break;
	 
       case IDCANCEL:
	 ThotCallback (BaseLibrary + FormLibrary, INTEGER_DATA, (char*) 0);
	 EndDialog (hwnDlg, IDCANCEL);
	 break;
       }	
     break;
   default:
     return FALSE;
   }
#endif /* _SVG */
 return TRUE;
}

/*-----------------------------------------------------------------------
 NewLibraryDlgProc
 ------------------------------------------------------------------------*/
LRESULT CALLBACK NewLibraryDlgProc (ThotWindow hwnDlg, UINT msg,
				    WPARAM wParam, LPARAM lParam)
{
#ifdef _SVG
   AddNewModelHwnd = hwnDlg;
   switch (msg)
      {
      case WM_INITDIALOG:
	SetWindowText (hwnDlg, TtaGetMessage (AMAYA, AM_SVGLIB_CREATE_NEW_CATALOGUE_DIALOG));
	SetWindowText (GetDlgItem (hwnDlg, ID_LIBRARYURL),
		TtaGetMessage (AMAYA, AM_SVGLIB_CATALOGUE_URL));
	SetWindowText (GetDlgItem (hwnDlg, ID_LIBRARYTITLE), 
		TtaGetMessage (AMAYA, AM_SVGLIB_CATALOGUE_TITLE));
	SetWindowText (GetDlgItem (hwnDlg, ID_CONFIRM), 
		TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
	SetWindowText (GetDlgItem (hwnDlg, ID_CLEAR), 
		TtaGetMessage (AMAYA, AM_CLEAR));
	SetWindowText (GetDlgItem (hwnDlg, ID_BROWSELIBRARY), 
		TtaGetMessage (AMAYA, AM_SVGLIB_ADD_NEW_CATALOGUE));
	SetWindowText (GetDlgItem (hwnDlg, IDCANCEL), TtaGetMessage (LIB, TMSG_CANCEL));
	return TRUE;
	break;
      case WM_COMMAND:
	switch (LOWORD (wParam))
	  {
	  case ID_CONFIRM:
		EndDialog (hwnDlg, ID_CONFIRM);
		break;

	case ID_BROWSELIBRARY:
	  OpenFileName.lStructSize       = sizeof (OPENFILENAME);
	  OpenFileName.hwndOwner         = hwnDlg;
	  OpenFileName.hInstance         = hInstance;
	  OpenFileName.lpstrFilter       = (LPTSTR) szFilter;
	  OpenFileName.lpstrCustomFilter = (LPTSTR) NULL;
	  OpenFileName.nMaxCustFilter    = 0L;
	  OpenFileName.nFilterIndex      = 1L;
	  OpenFileName.lpstrFile         = (LPTSTR) szFileName;
	  OpenFileName.nMaxFile          = 256;
	  OpenFileName.lpstrInitialDir   = NULL;
	  OpenFileName.lpstrTitle        = "Select";
	  OpenFileName.nFileOffset       = 0;
	  OpenFileName.nFileExtension    = 0;
	  OpenFileName.lpstrDefExt       = "*.lhtml";
	  OpenFileName.lCustData         = 0;
	  OpenFileName.Flags             = OFN_SHOWHELP | OFN_HIDEREADONLY;

	  if (GetOpenFileName (&OpenFileName))
		  strcpy (UrlToOpen, OpenFileName.lpstrFile);

	  SetDlgItemText (hwnDlg, ID_GETLIBRARYURL, UrlToOpen);

	  if (UrlToOpen[0] != 0)
	    {
	      ThotCallback (BaseLibrary + SVGLibFileBrowserText, STRING_DATA, UrlToOpen);
	      ThotCallback (BaseLibrary + SVGLibFileBrowser, INTEGER_DATA, (char*) 1);
	    }
	  break;

	  case IDCANCEL:
	    EndDialog (hwnDlg, IDCANCEL);
	    break;
	  }
	break;
      default:
	return FALSE;
      }
#endif /* _SVG */
   return TRUE;
}

/*-----------------------------------------------------------------------
 CreatePasteLibraryModelDlgWindow
 ------------------------------------------------------------------------*/
void CreatePasteLibraryModelDlgWindow (ThotWindow parent)
{
#ifdef _SVG
  DialogBox (hInstance, MAKEINTRESOURCE (PASTELIBRARYDIALOG), parent,
	     (DLGPROC) PasteLibraryModelDlgProc);
#endif _SVG
}

/*-----------------------------------------------------------------------
 CreateAddNewModelIntoLibraryDlgWindow
 ------------------------------------------------------------------------*/
void CreateAddNewModelIntoLibraryDlgWindow (ThotWindow parent, int nbr_Title, char *list_Title)
{
#ifdef _SVG
  nbTitle = nbr_Title;
  memcpy (listTitle, list_Title, MAX_LENGTH);

  DialogBox (hInstance, MAKEINTRESOURCE (ADDNEWMODELINTOLIBRARYDIALOG), parent,
	     (DLGPROC) AddNewModelIntoLibraryDlgProc);
#endif _SVG
}

/*-----------------------------------------------------------------------
 CreateNewLibraryDlgWindow
 ------------------------------------------------------------------------*/
void CreateNewLibraryDlgWindow (ThotWindow parent, int doc_type)
{
#ifdef _SVG
  UrlToOpen[0] = EOS;

  if (doc_type == docLibrary)
    szFilter = APPLIBRARYNAMEFILTER;
  else 
    szFilter = APPALLFILESFILTER;

	DialogBox (hInstance, MAKEINTRESOURCE (CREATELIBRARYDIALOG), parent,
	     (DLGPROC) NewLibraryDlgProc);
#endif /* _SVG */
}

#endif /* _WINGUI */

/*----------------------------------------------------------------------
  IsCurrentSelectionSVG
  Check if the current selection is only svg
  ----------------------------------------------------------------------*/
ThotBool IsCurrentSelectionSVG ()
{
  ThotBool     IsSVG = FALSE;
#ifdef _SVG
  Document     selDoc;
  Element      firstSelEl, lastSelEl, currentEl;
  ElementType  elTypeSel;
  int          firstChar, lastChar, i;
  int          stop = 0;

  selDoc = TtaGetSelectedDocument();
  if (selDoc == 0 ||
      (DocumentTypes[selDoc] == docLibrary ||
       DocumentTypes[selDoc] == docMath ||
       DocumentTypes[selDoc] == docText ||
       DocumentTypes[selDoc] == docCSS ||
       DocumentTypes[selDoc] == docSource ||
       DocumentTypes[selDoc] == docAnnot))
    /* there is a bad selection. Nothing to do */
    IsSVG = FALSE;
  if (TtaIsSelectionEmpty())
    IsSVG = FALSE;

  /* check the current selection */
  TtaGiveFirstSelectedElement (selDoc, &firstSelEl, &firstChar, &i);
  TtaGiveLastSelectedElement (selDoc, &lastSelEl, &i, &lastChar);

  elTypeSel = TtaGetElementType (firstSelEl);
  if (!strcmp (TtaGetSSchemaName (elTypeSel.ElSSchema), "SVG") &&
      (firstSelEl == lastSelEl))
    {
      IsSVG = TRUE;
    }
  else if (!strcmp (TtaGetSSchemaName (elTypeSel.ElSSchema), "SVG"))
    {
      currentEl = firstSelEl;
      while (currentEl && (stop == 0))
	{
	  elTypeSel = TtaGetElementType (currentEl);
	  if (strcmp (TtaGetSSchemaName (elTypeSel.ElSSchema), "SVG"))
	    {
	      stop = 1;
	      IsSVG = FALSE;
	    }
	  if (currentEl == lastSelEl)
	    currentEl = NULL;
	  else
	    TtaGiveNextSelectedElement (selDoc, &currentEl, &i, &i);
	}
      if (stop == 0)
	IsSVG = TRUE;
    }
  else
    IsSVG = FALSE;
#endif /* _SVG */
  return IsSVG;
}


/*----------------------------------------------------------------------
  IsCurrentSelectionContainsUseElement
  Check if the current selection contains use element
  You must use IsCurrentSelectionSVG before
  ----------------------------------------------------------------------*/
ThotBool IsCurrentSelectionContainsUseElement()
{
  ThotBool      containUseEl = FALSE;
#ifdef _SVG
  Document      selDoc;
  Element       firstSelEl, lastSelEl, currentEl, elFound;
  ElementType   elTypeSearch;
  int           firstChar, lastChar, i;
  int           stop = 0;

  selDoc = TtaGetSelectedDocument ();
  /* get the current selection */
  TtaGiveFirstSelectedElement (selDoc, &firstSelEl, &firstChar, &i);
  TtaGiveLastSelectedElement (selDoc, &lastSelEl, &i, &lastChar);

  elTypeSearch = TtaGetElementType (firstSelEl);
  elTypeSearch.ElTypeNum = SVG_EL_use_;

  currentEl = firstSelEl;
  while (currentEl && (stop == 0))
    {
      elFound = TtaSearchTypedElement (elTypeSearch, SearchInTree,
				       TtaGetParent (currentEl));
      if (elFound == currentEl || TtaIsAncestor (elFound, currentEl))
	{
	  containUseEl = TRUE;
	  stop = 1;
	}
      if (currentEl == lastSelEl)
	currentEl = NULL;
      else
	TtaGiveNextSelectedElement (selDoc, &currentEl, &i, &i);
    }
#endif /* _SVG */
  return containUseEl;
}

/*----------------------------------------------------------------------
  AddNewModelIntoLibraryForm
  Checks if the selection is a SVG element        
  and initializes add svg model form (dialog)
  ----------------------------------------------------------------------*/
void AddNewModelIntoLibraryForm (Document doc, View view)
{
#ifdef _SVG
  int            nbr;
#ifndef _WINGUI
  char           buffer[MAX_LENGTH];
  int            i;
#endif /* _WINGUI */

  /* Initialize Structure if it's not yet done */
  SaveLibraryTitleSelection[0] = EOS;

  /* Check the current selection */
  if (IsCurrentSelectionSVG ())
    {
#ifndef _WINGUI
      i = 0;
      strcpy (&buffer[i], TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
      i += strlen (&buffer[i]) + 1;
      strcpy (&buffer[i], TtaGetMessage (AMAYA, AM_SVGLIB_CREATE_NEW_CATALOGUE));

      TtaNewSheet (BaseLibrary + AddSVGModel, TtaGetViewFrame (doc, view), 
		   TtaGetMessage (AMAYA, AM_SVGLIB_ADD_SVG_MODEL_DIALOG), 2,
		   buffer, TRUE, 2, 'L', D_CANCEL);

      /* Catalogue Or URI text zone */
      nbr = SVGLibraryListItemNumber (SVGlib_list);
      TtaNewSizedSelector (BaseLibrary + SVGLibCatalogueTitle, BaseLibrary + AddSVGModel,
			   TtaGetMessage (AMAYA, AM_SVGLIB_CATALOGUE_TITLE),
			   nbr, SVGlib_list, 50, 3,
			   FALSE, FALSE, TRUE);
      /* activates the Add Library Model Dialogue  */
      TtaSetDialoguePosition ();
      TtaShowDialogue (BaseLibrary + AddSVGModel, TRUE);
      TtaWaitShowDialogue ();
#else /* _WINGUI */
      nbr = SVGLibraryListItemNumber (SVGlib_list);
      CreateAddNewModelIntoLibraryDlgWindow (TtaGetViewFrame (doc, view), nbr, SVGlib_list);
#endif /* _WINGUI */
    }
  else
    {
      /* show an error selection dialogue */
      InitInfo (TtaGetMessage (AMAYA, AM_ERROR),
		TtaGetMessage (AMAYA, AM_SVGLIB_NO_SVG_SELECTION));
    }
#endif /* _SVG */
}


/*----------------------------------------------------------------------
  CloseLibrary
  ----------------------------------------------------------------------*/
void CloseLibrary (Document doc)
{
#ifdef _SVG
  LibraryDocDocument = 0;
  LibraryDocElement = NULL;
#endif /* _SVG */
}



/*----------------------------------------------------------------------
  SVGLibraryListItemNumber
  This function return the number of elements of a List
  A double EOS closes the list.
  ----------------------------------------------------------------------*/
int SVGLibraryListItemNumber (char *buffer)
{
  int          cpt = 0;
#ifdef _SVG
  char        *ptr, *ptr1;

  ptr = buffer;
  if (buffer)
    {
      while (*ptr != EOS)
	{
	  ptr1 = ptr;
	  while (*ptr1 != EOS)
	      ptr1++;
	  cpt++;
	  ptr = ptr1 + 1;
	}
    }
#endif /* _SVG */
  return cpt;
}


/*----------------------------------------------------------------------
  IsSVGCatalogueExist
  Search into HeaderListUriTitle
  parameters: data which is title or URI
  ----------------------------------------------------------------------*/
char *IsSVGCatalogueExist (char *data)
{
  char              *catalogueUri = NULL;
#ifdef _SVG
  ListUriTitle      *curList = HeaderListUriTitle;

  while (curList &&
	 strcmp (curList->URI, (char *) data) != 0 &&
	 strcmp (curList->Title, (char *) data) != 0)
    curList = curList->next;
  if (curList)
    catalogueUri = curList->URI;
  else
    catalogueUri = NULL;
#endif /* _SVG */
  return catalogueUri;
}

/*----------------------------------------------------------------------
  AddGraphicalObjectIntoCatalogue                    
  
  input: svg selected event                        
  output: the graphical object into the catalogue  
  ----------------------------------------------------------------------*/
void AddGraphicalObjectIntoCatalogue (Document doc, View view)
{
#ifdef _SVG
  AddNewModelIntoLibraryForm (doc, view);
#endif /* _SVG */
}

#ifdef _SVG
/*-------------------------------------------------------------------
  InitSVGBufferForCombobox
  Initializes a string by reading data in a Library Manager Structure List
  This function allocates memory and returns it.
  So you have to free them after calling this function.
  -------------------------------------------------------------------*/
static void InitSVGBufferForComboBox ()
{
  ListUriTitle     *curList;
  int               lg = 0;

  if (HeaderListUriTitle)
    {
      /* length of the string */
      curList = HeaderListUriTitle;
      while (curList)
	{
	  lg += strlen (curList->Title) + 1;
	  curList = curList->next;
	}
      /* remove the previous list */
      TtaFreeMemory (SVGlib_list);
      SVGlib_list = (char *)TtaGetMemory (lg + 10);
      curList = HeaderListUriTitle;
      SVGlib_list[0] = EOS;
      lg = 0;
      while (curList)
	{
	  strcpy (&SVGlib_list[lg], curList->Title);
	  lg += strlen (curList->Title) + 1;
	  curList = curList->next;
	}
       SVGlib_list[lg] = EOS;
    }
}
#endif /* _SVG */

/*----------------------------------------------------------------------
  CreateNewLibraryFile
  Creates a library file document, initializes the title of this document
  and opens it into a new window.
  Then this function returns the document
  parameters:
  url: where the new library is save (you have to test if this file exists
  before using this function)
  title: the title of the new library
  Inspired from code function InitializeNewDoc (EDITORaction.c)
  ----------------------------------------------------------------------*/
Document CreateNewLibraryFile (char *libUrl, char *libtitle)
{
  Document             newLibraryDoc = 0;
#ifdef _SVG
  Element              docEl, root, title, text, el, head, child, meta, body;
  Element              doctype, style;
  ElementType          elType;
  Attribute            attr;
  AttributeType        attrType;
  Language             language;
  char                *s, *documentname, *pathname, *textStr;
  char                 tempfile[MAX_LENGTH];
  char                 charsetName[MAX_LENGTH];

  /* create the document */
  textStr = (char * ) TtaGetMemory (MAX_LENGTH);
  pathname = (char * ) TtaGetMemory (MAX_LENGTH);
  documentname = (char * ) TtaGetMemory (MAX_LENGTH);
  NormalizeURL (libUrl, 0, pathname, documentname, NULL);

  AddLibraryDataIntoStructure (TRUE, libUrl, libtitle);
  WriteInterSessionLibraryFileManager ();
  InitSVGBufferForComboBox ();
  DontReplaceOldDoc = TRUE;
  newLibraryDoc = InitDocAndView (newLibraryDoc,
                                  FALSE /* replaceOldDoc */,
                                  TRUE /* inNewWindow */,
                                  documentname, docLibrary, 0, FALSE,
				  L_Xhtml11, 0, CE_RELATIVE);
  DontReplaceOldDoc = FALSE;

  TtaFreeMemory (documentname);
  TtaFreeMemory (pathname);

  /* save the document name into the document table */
  s = TtaStrdup (libUrl);
  TtaSetTextZone (newLibraryDoc, 1, libUrl);
  DocumentURLs[newLibraryDoc] = s;
  DocumentMeta[newLibraryDoc] = DocumentMetaDataAlloc ();
  DocumentMeta[newLibraryDoc]->form_data = NULL;
  DocumentMeta[newLibraryDoc]->initial_url = NULL;
  DocumentMeta[newLibraryDoc]->method = CE_ABSOLUTE;
  DocumentMeta[newLibraryDoc]->xmlformat = FALSE;
  DocumentMeta[newLibraryDoc]->compound = FALSE;
  DocumentSource[newLibraryDoc] = 0;

  /*
   *
   * Initialiser les metas: DocumentMeta[newLibraryDoc]
   * OK mais voir si DocumentSource et DocumentURLs ne pose pas de probl�me
   */
  /* store the document profile */
  TtaSetDocumentProfile (newLibraryDoc, L_Xhtml11, L_NoExtraProfile);
  ResetStop (newLibraryDoc);
  language = TtaGetDefaultLanguage ();
  docEl = TtaGetMainRoot (newLibraryDoc);
  /* Set the document charset */
  TtaSetDocumentCharset (newLibraryDoc, ISO_8859_1, FALSE);
  strcpy (charsetName , "iso-8859-1");
  DocumentMeta[newLibraryDoc]->charset = TtaStrdup (charsetName);

  /*-------------  New XHTML document ------------*/
  /* create the DOCTYPE element corresponding to the document's profile */
  /* force the XML parsing */
  DocumentMeta[newLibraryDoc]->xmlformat = TRUE;
  DocumentMeta[newLibraryDoc]->compound = FALSE;

  elType = TtaGetElementType (docEl);
  attrType.AttrSSchema = elType.ElSSchema;

  elType.ElTypeNum = HTML_EL_DOCTYPE;
  doctype = TtaSearchTypedElement (elType, SearchInTree, docEl);
  CreateDoctype (newLibraryDoc, doctype, L_Xhtml11, FALSE, FALSE);
  
  /* Load user's style sheet */
  LoadUserStyleSheet (newLibraryDoc);
  
  /* attach an attribute PrintURL to the root element */
  elType.ElTypeNum = HTML_EL_HTML;
  root = TtaSearchTypedElement (elType, SearchInTree, docEl);
  attrType.AttrTypeNum = HTML_ATTR_PrintURL;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (root, attr, newLibraryDoc);
  
  /* create a default title if there is no content in the TITLE element */
  elType.ElTypeNum = HTML_EL_TITLE;
  title = TtaSearchTypedElement (elType, SearchInTree, root);
  text = TtaGetFirstChild (title);
  if (TtaGetTextLength (text) == 0)
    TtaSetTextContent (text, (unsigned char *)libtitle, language, newLibraryDoc);
  UpdateTitle (title, newLibraryDoc);

  elType.ElTypeNum = HTML_EL_HEAD;
  head = TtaSearchTypedElement (elType, SearchInTree, root);

  /* create a default style element */
  elType.ElTypeNum = HTML_EL_STYLE_;
  style = TtaNewElement (newLibraryDoc, elType);
  TtaInsertSibling (style, title, FALSE, newLibraryDoc);
  /* attach a style attribute */
  attrType.AttrTypeNum = HTML_ATTR_Notation;
  attr = TtaNewAttribute (attrType);
  if (attr)
    TtaAttachAttribute (style, attr, newLibraryDoc);
  strcpy (textStr, "text/css");
  TtaSetAttributeText (attr, textStr, style, newLibraryDoc);
  /* initialize style text content */
  text = TtaGetFirstChild (style);
  if (text == NULL)
    {
      elType.ElTypeNum = HTML_EL_TEXT_UNIT;
      text = TtaNewElement (newLibraryDoc, elType);
      TtaInsertFirstChild (&text, style, newLibraryDoc);
    }
  strcpy (textStr, " .image {width: 50px; height: 50px} ");
  sprintf (textStr, "%s%c", textStr, EOL);
  strcat (textStr, " .g_title {color: #0000B2; font-family: helvetica; font-weight: bold; vertical-align: middle}");
  sprintf (textStr, "%s%c", textStr, EOL);
  strcat (textStr, " .g_comment {font-size: 12pt; font-weight: normal; color: #B2005A; vertical-align: middle}");
  TtaSetTextContent (text, (unsigned char *)textStr, language, newLibraryDoc);

  /* create a Document_URL element as the first child of HEAD */
  elType.ElTypeNum = HTML_EL_Document_URL;
  el = TtaSearchTypedElement (elType, SearchInTree, head);
  if (el == NULL)
    {
      /* there is no Document_URL element, create one */
      el = TtaNewElement (newLibraryDoc, elType);
      TtaInsertFirstChild (&el, head, newLibraryDoc);
    }
  /* prevent the user from editing this element */
  TtaSetAccessRight (el, ReadOnly, newLibraryDoc);
  /* element Document_URL already exists */
  text = TtaGetFirstChild (el);
  if (text == NULL)
    {
      elType.ElTypeNum = HTML_EL_TEXT_UNIT;
      text = TtaNewElement (newLibraryDoc, elType);
      TtaInsertFirstChild (&text, el, newLibraryDoc);
    }
  if (libUrl != NULL && text != NULL)
    TtaSetTextContent (text, (unsigned char *)libUrl, language, newLibraryDoc);
  
  /* create a META element in the HEAD with name="generator" */
  /* and content="Amaya" */
  child = TtaGetLastChild (head);
  elType.ElTypeNum = HTML_EL_META;
  meta = TtaNewElement (newLibraryDoc, elType);
  attrType.AttrTypeNum = HTML_ATTR_meta_name;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (meta, attr, newLibraryDoc);
  TtaSetAttributeText (attr, "generator", meta, newLibraryDoc);
  attrType.AttrTypeNum = HTML_ATTR_meta_content;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (meta, attr, newLibraryDoc);
  strcpy (tempfile, TtaGetAppName());
  strcat (tempfile, " ");
  strcat (tempfile, TtaGetAppVersion());
  strcat (tempfile, ", see http://www.w3.org/Amaya/");
  TtaSetAttributeText (attr, tempfile, meta, newLibraryDoc);
  TtaInsertSibling (meta, child, FALSE, newLibraryDoc);
  
  /* create a BODY element if there is not */
  elType.ElTypeNum = HTML_EL_BODY;
  body = TtaSearchTypedElement (elType, SearchInTree, root);
  if (!body)
    {
      body = TtaNewTree (newLibraryDoc, elType, "");
      TtaInsertSibling (body, head, FALSE, newLibraryDoc);
    }
  
  /* Search the first element in the BODY to set initial selection */
  elType.ElTypeNum = HTML_EL_Element;
  el = TtaSearchTypedElement (elType, SearchInTree, body);
  /* set the initial selection */
  TtaSelectElement (newLibraryDoc, el);
  /* apply style */
  EnableStyleElement (newLibraryDoc, style);

  if (SelectionDoc != 0)
    UpdateContextSensitiveMenus (SelectionDoc, 1);
  SelectionDoc = newLibraryDoc;
  UpdateContextSensitiveMenus (newLibraryDoc, 1);
  /* Activate show areas */
  if (MapAreas[newLibraryDoc])
    ChangeAttrOnRoot (newLibraryDoc, HTML_ATTR_ShowAreas);
#endif /* _SVG */
  return newLibraryDoc;
}

/*----------------------------------------------------------------------
  GetLibraryFileTitle
  Allocates and returns the title associated to a Library File document
  input parameters:
  URL of the library file encoded with the default charset.
  ----------------------------------------------------------------------*/
char *GetLibraryFileTitle (char *url)
{
  char               *reTitle = NULL;
#ifdef _SVG
  Document             res, libraryDoc;
  ElementType         elTypeSearch;
  Element             el, child;
  Language            lang;
  unsigned char       *title;
  int                 length;
  
  if (url && TtaFileExist (url))
    {
      /* Open library file in a document (Cf. MakeBook) */
      libraryDoc = TtaNewDocument("HTML", "tmp");
      DontReplaceOldDoc = FALSE;
      res = GetAmayaDoc (url, NULL, libraryDoc, 0, CE_MAKEBOOK,
			 FALSE, NULL, NULL);
      
      /* search the Title element */
      el = TtaGetMainRoot (res);
      elTypeSearch.ElSSchema = TtaGetDocumentSSchema (res);
      if (!strcmp (TtaGetSSchemaName (elTypeSearch.ElSSchema), "HTML"))
	/* it's a HTML document */
	{
	  elTypeSearch.ElTypeNum = HTML_EL_TITLE;
	  el = TtaSearchTypedElement (elTypeSearch, SearchForward, el);
	  child = TtaGetFirstChild (el);
	  if (child != NULL)
	    {
	      /* title exist */
	      length = MAX_LENGTH;
	      reTitle = (char *) TtaGetMemory (MAX_LENGTH);
	      TtaGiveTextContent (child, (unsigned char *)reTitle, &length, &lang);
	      title = TtaConvertMbsToByte ((unsigned char *)reTitle,
					   TtaGetDefaultCharset ());
	      strcpy ((char *)reTitle, (char *)title);
	      TtaFreeMemory (title);
	    }
	  else
	    /* Title not found */
	    reTitle = NULL;
	  /* Free resource */
	  FreeDocumentResource (libraryDoc);
	  TtaCloseDocument (libraryDoc);
	}
      else
	reTitle = NULL;
    }
#endif /* _SVG */
  return reTitle;
}

/*----------------------------------------------------------------------
  CreateUniqueLibraryTitle
  This function allocates and returns a unique library title
  by adding a int at the end of the library title
  ----------------------------------------------------------------------*/
char *CreateUniqueLibraryTitle (char *title)
{
  char *Title = NULL;
#ifdef _SVG
  int   i = 1;
  Title = (char *) TtaGetMemory (MAX_LENGTH); /*strlen (title) + 3*/
  strcpy (Title, title);
  while (IsLibraryTitleExist (Title))
    {
      /* A EFFECTUER : ajouter des parenth�ses autour du nombre */
      sprintf (Title, "%s%d", title, i);
      i++;
    }
#endif /* _SVG */
  return Title;
}

/*----------------------------------------------------------------------
  IsLibraryTitleExist
  check if libray title already exist in the structure list to avoid
  a couple of same title in the structure (in combobox too)
  returns TRUE if title already exist, FALSE elseif
  ----------------------------------------------------------------------*/
ThotBool IsLibraryTitleExist (char *title)
{
  ThotBool          tExist = FALSE;
#ifdef _SVG
  ListUriTitle     *listCur = HeaderListUriTitle;
  while (listCur)
    {
      if (!strcmp (listCur->Title, title))
	tExist = TRUE;
      listCur = listCur->next;
    }
#endif /* _SVG */
  return tExist;
}


/*----------------------------------------------------------------------
  SearchURIinCurrentSVGLibraryList
  return TRUE if the URI is yet in the list
  ----------------------------------------------------------------------*/
ThotBool SearchURIinCurrentSVGLibraryList (char *URI)
{
  ThotBool found = FALSE;
#ifdef _SVG
  ListUriTitle     *curList;

  if (HeaderListUriTitle)
    {
      curList = HeaderListUriTitle;
      while (curList &&
	     strcmp (curList->URI, URI))
	{
	  curList = curList->next;
	}
      if (curList)
	found = TRUE;
    }
#endif /* _SVG */
  return found;
}


/*-------------------------------------------------------------------
  SVGLIB_FreeDocumentResource
  Frees all the library resources that are associated with
  Library document when closing Amaya.
  -------------------------------------------------------------------*/
void SVGLIB_FreeDocumentResource ()
{
#ifdef _SVG
  ListUriTitle      *curList, *prevList;
  int                i = 1;
  /* is it the last loaded document ? */

  while (i < DocumentTableLength && 
	 ((DocumentURLs[i] == NULL) || (DocumentTypes[i] == docLibrary)))
    i++;
  
  if (i == DocumentTableLength)
    {
      i = 1;
      while (i < DocumentTableLength && DocumentTypes[i] != docLibrary)
	i++;
      if (i < DocumentTableLength && !TtaIsDocumentModified (i))
	{
	  FreeDocumentResource (i);
	  TtaCloseDocument (i);
	}
      /* Free structure memory */
      curList = HeaderListUriTitle;
      while (curList)
	{
	  TtaFreeMemory (curList->URI);
	  TtaFreeMemory (curList->Title);
	  prevList = curList;
	  curList = curList->next;
	  TtaFreeMemory (prevList);
	}
      HeaderListUriTitle = NULL;
    }
#endif /* _SVG */
}


/*-------------------------------------------------------------------
  GetLibraryPathFromTitle
  Search title in the Library Manager Structure list
  and return the corresponding path
  if it exists or NULL else if
  -------------------------------------------------------------------*/
char *GetLibraryPathFromTitle (char *title)
{
  char             *path = NULL;
#ifdef _SVG
  ListUriTitle     *listCur;

  if (HeaderListUriTitle)
    {
      /* list exist */
      listCur = HeaderListUriTitle;
      while (listCur && (strcmp (listCur->Title, title) != 0))
	listCur = listCur->next;
      if (listCur)
	path = listCur->URI;
    }
#endif /* _SVG */
  return path;
}

/*-------------------------------------------------------------------
  SelectLibraryFromPath
  Search path in the Library Manager Structure List 
  Update the list of library for the combobox.
  -------------------------------------------------------------------*/
void SelectLibraryFromPath (char *path)
{
#ifdef _SVG
  char             *title=NULL;
  ListUriTitle     *curList;
  int               lg = 0;

  if (HeaderListUriTitle)
    {
      /* length of the string */
      curList = HeaderListUriTitle;
      while (curList)
	{
	  lg += strlen (curList->Title) + 1;
	  if (strcmp (curList->URI, path) == 0)
	    title = curList->Title;
	  curList = curList->next;
	}
      if (title)
	{
	  /* remove the previous list */
	  TtaFreeMemory (SVGlib_list);
	  SVGlib_list = (char *)TtaGetMemory (lg + 10);
	  curList = HeaderListUriTitle;
	  /* put the selected library in first position */
	  strcpy (SVGlib_list, title);
	  lg = strlen (title) + 1;
	  while (curList)
	    {
	      if (strcmp (curList->URI, path) != 0)
		{
		  strcpy (&SVGlib_list[lg], curList->Title);
		  lg += strlen (curList->Title) + 1;
		}
	      curList = curList->next;
	    }
	  SVGlib_list[lg] = EOS;
	}
    }
#endif /* _SVG */
}

/*------------------------------------------------------------------
  InitSVGLibraryManagerStructure
  Initialize the svg library manager structure:
  the corresponding table between Catalogue URI and Title
  This function starts by reading lib_files.dat in APP_HOME directory
  Then it reads the same file in THOT_DIR/config directory
  ------------------------------------------------------------------*/
void InitSVGLibraryManagerStructure ()
{
#ifdef _SVG
  char          *url_home, *url_thot, *app_home, *thot_dir;
  char          *urlstring, *title;
  unsigned char  c;
  FILE          *file;
  int            i, nb, len;
  ThotBool       cont;

  SVGlib_list = NULL;
  if (!HeaderListUriTitle)
    {
      url_home = (char *) TtaGetMemory (MAX_LENGTH);
      urlstring = (char *) TtaGetMemory (MAX_LENGTH);
      url_thot = (char *) TtaGetMemory (MAX_LENGTH);
      /* Read lib_files.dat into APP_HOME directory */
      app_home = TtaGetEnvString ("APP_HOME");
      sprintf (url_home, "%s%clib_files.dat", app_home, DIR_SEP); 
      /* ./.amaya/lib_files.dat */
      file = TtaReadOpen (url_home);
      if (file)
	{
	  /* initialize the list by reading the file */
	  i = 0;
	  nb = 0;
	  cont = TRUE;
	  while (cont)
	    {
	      len = 0;
	      while (len < MAX_LENGTH && TtaReadByte (file, &c) && c != EOL)
		{
		  if (c == 13)
		    urlstring[len] = EOS;
		  else
		    urlstring[len++] = (char)c;
		}
	      urlstring[len] = EOS;
	      cont = (c == EOL);
	      if (urlstring[0] != EOS)
		{
		  /* Get the document title by opening the document
		     and then update the Uri-Title structure */
		  title = GetLibraryFileTitle (urlstring);
		  if (title)
		    {
		      /* ajout dans la structure et dans le buffer de memorisation */
		      AddLibraryDataIntoStructure (TRUE, urlstring, title);
		      TtaFreeMemory (title);
		    }
		}
	    }
	  TtaReadClose (file);
	}
      /* Read lib_files.dat into THOTDIR directory */
      thot_dir = TtaGetEnvString ("THOTDIR");
      sprintf (url_thot, "%s%cconfig%clib_files.dat", thot_dir, DIR_SEP, DIR_SEP);
      file = TtaReadOpen (url_thot);
      strcpy (url_thot, thot_dir);
      if (file)
	{
	  /* initialize the list by reading the file */
	  i = 0;
	  nb = 0;
	  cont = TRUE;
	  while (cont)
	    {
	      len = 0;
	      while (len < MAX_LENGTH && TtaReadByte (file, &c) && c != EOL)
		{
		  if (c == 13)
		    urlstring[len] = EOS;
		  else
		    urlstring[len++] = (char)c;
		}
	      urlstring[len] = EOS;
	      cont = (c == EOL);
	      if (urlstring[0] != EOS)
		{
		  sprintf (url_thot, "%s%cconfig%clibconfig%c%s", thot_dir,
			   DIR_SEP, DIR_SEP, DIR_SEP, urlstring);
		  strcpy (urlstring, url_thot);
		  /* Get the document title by opening the document
		     and then update the Uri-Title structure */
		  title = GetLibraryFileTitle (urlstring);
		  if (title)
		    {
		      /* Add into Library Manager Structure List */
		      AddLibraryDataIntoStructure (FALSE, urlstring, title);
		      TtaFreeMemory (title);
		    }
		}
	    }
	  TtaReadClose (file);
	}
      
      TtaFreeMemory (url_thot);
      TtaFreeMemory (url_home);
      TtaFreeMemory (urlstring);
      InitSVGBufferForComboBox ();
    }
#endif /* _SVG */
}


/*----------------------------------------------------------------------
  IsLibraryName                                                         
  returns TRUE if path points to an SVG resource.
  ----------------------------------------------------------------------*/
ThotBool IsLibraryName (const char *path)
{
#ifdef _SVG
   char        temppath[MAX_LENGTH];
   char        suffix[MAX_LENGTH];

   if (!path)
      return (FALSE);

   strcpy (temppath, path);
   TtaExtractSuffix (temppath, suffix);
   /* lhtml = library html */
   if (!strcasecmp (suffix, "lhtml"))
     return (TRUE);
   else if (!strcmp (suffix, "gz"))
     {
       /* take into account compressed files */
       TtaExtractSuffix (temppath, suffix);       
       if (!strcasecmp (suffix, "lhtml"))
	 return (TRUE);
       else
	 return (FALSE);
     }
   else
     return (FALSE);
#endif /* _SVG */
   return FALSE;
}

/*----------------------------------------------------------------------
  Load the first catalogue of the Library Manager Structure List
  ----------------------------------------------------------------------*/
void OpenCatalogue (Document doc, View view)
{
#ifdef _SVG
  char     *lib_path, *app_home, *buffer;

  if (HeaderListUriTitle)
    {
      /* Open the first catalogue of this list*/
      buffer = (char *) TtaGetMemory (strlen (HeaderListUriTitle->URI) + 1);
      strcpy (buffer, HeaderListUriTitle->URI);
      if (buffer)
	{
	  if (IsLibraryName (buffer))
	    {
	      /* load the HOME document */
	      DontReplaceOldDoc = TRUE;
	      CurrentDocument = doc;
	      GetAmayaDoc (buffer, NULL, 0, 0, CE_RELATIVE,
			   FALSE, NULL, NULL);
	    }
	  TtaFreeMemory (buffer);
	}
    }
  else
    /* Open the default catalogue in THOTDIR/config/libconfig/default_cat.lhtml */
    {
      /* Open the Amaya default library file */
      lib_path =  (char *) TtaGetMemory (MAX_LENGTH);
      app_home = TtaGetEnvString ("THOTDIR");
      sprintf (lib_path, "%s%cconfig%clibconfig%cdefault_cat.lhtml",
	       app_home, DIR_SEP, DIR_SEP, DIR_SEP);

      /* load the Catalogue document */
      DontReplaceOldDoc = TRUE;
      CurrentDocument = doc;
      GetAmayaDoc (lib_path, NULL, 0, 0, CE_ABSOLUTE, FALSE, NULL, NULL);
      TtaFreeMemory (lib_path);
    }
#endif /* _SVG */
}

/*----------------------------------------------------------------------
  ShowLibrary
  open a catalogue file and customize the dialogue window widget
  ----------------------------------------------------------------------*/
void ShowLibrary (Document doc, View view)
{
#ifdef _SVG
  /* open catalogue */
  OpenCatalogue (doc, view);
  /* Cr�ation des widgets de la fen�tre pour obtenir l'ergonomie souhait�e */
  /* A effectuer si necessaire en modifiant les Widgets de la FrameTable */
  TtaHandlePendingEvents ();  
#endif /* _SVG */
}

/*----------------------------------------------------------------------
  SaveSVGURL saves SVG URL into a buffer
  input parameters:
  doc: the number of the selected document
  El: the selected element
  output: the SVG URL in local variable LibURL (if URL exist)
  ----------------------------------------------------------------------*/
void SaveSVGURL (Document doc, Element El)
{
#ifdef _SVG
  ElementType       elTypeSearch;
  Element           currentEl, prevEl;
  Attribute         attrSearch;
  AttributeType     attrTypeSearch;
  char             *s;
  int               length;
  
  currentEl = El;
  /*
   *solution avec tant que non TtaGetTextLength alors TtaGetFirstChild 
   *(contournement du probl�me li� � la recherche sur le ptr de l'�l�ment 
   *  ElTerminal = 1 ou 0)
   */
  prevEl = TtaGetParent (currentEl);
  elTypeSearch = TtaGetElementType (prevEl);
  while (elTypeSearch.ElTypeNum != HTML_EL_Anchor && prevEl)
    {
      prevEl = TtaGetParent (prevEl);
      elTypeSearch = TtaGetElementType (prevEl);
    }
  currentEl = prevEl;
  /* search the HREF attribute of the element */
  attrTypeSearch.AttrSSchema = elTypeSearch.ElSSchema;
  attrTypeSearch.AttrTypeNum = HTML_ATTR_HREF_;
  attrSearch = TtaGetAttribute (currentEl, attrTypeSearch);
  if (attrSearch)
    {
      length = MAX_LENGTH;
      TtaGiveTextAttributeValue (attrSearch, LibURL, &length);
      /* convert the libURL with the default encoding */
      s = (char *)TtaConvertMbsToByte ((unsigned char *)LibURL,
				       TtaGetDefaultCharset ());
      strcpy (LibURL, s);
      TtaFreeMemory (s);
    }
#endif /* _SVG */
}


/*----------------------------------------------------------------------
  ChangeSVGLibraryLinePresentation
  function that changes table line presentation of a library Document
  input parameters:
  doc: the selected document
  El: the selected element
  ----------------------------------------------------------------------*/
void ChangeSVGLibraryLinePresentation (Document doc, Element El)
{
#ifdef _SVG
  ElementType       elTypeSearch, elTypeCur;
  Element           prev;
  PRule             PRuleSearch;
  char             *color;

  if (LibraryDocElement)
    {
      /* there is a Background Color Presentation attach to this element */
      /*      PRBackground 43 */
      PRuleSearch = TtaGetPRule (LibraryDocElement, PRBackground);
      TtaRemovePRule (LibraryDocElement, PRuleSearch, doc);
      LibraryDocElement = NULL;
    }
  /* search element where we have to create (modify) the presentation */
  elTypeSearch.ElSSchema = TtaGetDocumentSSchema (doc);
  elTypeSearch.ElTypeNum = /*HTML_EL_Data_cell*/ HTML_EL_Table_row ;
  /*  current = TtaSearchTypedElement (elTypeSearch, SearchBackward, El);*/
  /* =>alternative lourde */
  prev = TtaGetParent (El);
  elTypeCur = TtaGetElementType (prev);
  while (elTypeCur.ElTypeNum != elTypeSearch.ElTypeNum)
    {
      prev = TtaGetParent (prev);
      elTypeCur = TtaGetElementType (prev);
    }
  /* store document information */
  LibraryDocElement = prev;
  LibraryDocDocument = doc;
  /* there is no Background Color attribute attach to this element */
  color = (char *) TtaGetMemory (strlen ("#FFD8C1") + 1);
  strcpy (color, "#FFD8C1");
  HTMLSetBackgroundColor (doc, prev, 200, color);
  TtaFreeMemory (color);
#endif /* _SVG */
}

/*----------------------------------------------------------------------
  InitLibrary
  Initialize Library
  ----------------------------------------------------------------------*/
void InitLibrary (void)
{
#ifdef _SVG
#endif /* _SVG */
}

/*----------------------------------------------------------------------
  GetURIId this function allocates and returns the Identifier of a URI
  if it exists or NULL.
  parameter: the "xlink:href" URI
  ----------------------------------------------------------------------*/
char *GetURIId (char *href)
{
  char    *result = NULL;
#ifdef _SVG
  char    *ptr1;
  int      length;

  if (href)
    {
      ptr1 = href;
      while (*ptr1 != '#' && *ptr1 != EOS)
	{
	  ptr1++;
	}
      if (*ptr1 == '#')
	{
	  ptr1++;
	  length = strlen (ptr1) + 1;
	  result = (char *) TtaGetMemory (strlen (ptr1) + 1);
	  strcpy (result, ptr1);
	}
      else
	result = NULL;
    }
#endif /* _SVG */
  return result;
}

/*----------------------------------------------------------------------
  SearchSVGElement
  Return the svg element which has an Id Attribute
  with identificateur value
  ----------------------------------------------------------------------*/
Element SearchSVGElement (Document doc, char *identificateur)
{
  Element        elFound = NULL;
#ifdef _SVG
  Element        root;
  ElementType    elTypeSearch;
  Attribute      attrFound;
  AttributeType  attrTypeSearch;
  int            length, stop = 0;
  char           *buffer;

  /* initialize ElementType */
  root = TtaGetMainRoot (doc);
  elTypeSearch = TtaGetElementType(root);

  /* initialize AttributeTypeSearch */
  attrTypeSearch.AttrSSchema = elTypeSearch.ElSSchema;
  attrTypeSearch.AttrTypeNum = SVG_ATTR_id;

  while (stop != 1)
    {
      TtaSearchAttribute (attrTypeSearch, SearchInTree, root, &elFound, &attrFound);
      if (elFound && attrFound)
	{
	  /* An element has been found => check value */
	  length = TtaGetTextAttributeLength (attrFound);
	  buffer = (char *) TtaGetMemory (length + 1);
	  TtaGiveTextAttributeValue (attrFound, buffer, &length);

	  if (strcmp (buffer, identificateur) == 0)
	    {
	      stop = 1;
	    }
	}
      else
	{
	  stop = 1;
	}
    }

/*  elFound = TtaGetFirstChild (elFound);
  elTypeSearch = TtaGetElementType (elFound);*/
#endif /* _SVG */
  return elFound;
}

/*----------------------------------------------------------------------
   PasteLibraryGraphicElement
   Paste a Graphics element (inspired of CreateGraphicElement and TtacopyTree)
   at the current selection
   return the root element of the pasted tree
  ----------------------------------------------------------------------*/
Element PasteLibraryGraphicElement (Element sourceEl, Document sourceDoc, int Method)
{
  Element        copiedElement = NULL;
#ifdef _SVG
  Document       destDoc;
  SSchema        docSchema, SvgSchema;
  Element        selEl, child, SvgRootEl, parent, sibling;
  Element        LastInserted, firstSelEl, useEl, elFound, curEl;
  ElementType    elType, selType;
  Attribute      attrSvgRoot, attrSvgUse, attrFound;
  AttributeType  attrTypeSvgRoot, attrTypeSvgUse;
  int            firstChar, lastChar, len;
  char          *pathname, *buffer, *basename, *filename, *relativeURL = NULL;
  DisplayMode    dispMode;
  ThotBool	 found;

  /* check destination document */
  destDoc = TtaGetSelectedDocument();
  if (destDoc == 0 ||
      (DocumentTypes[destDoc] == docLibrary ||
       DocumentTypes[destDoc] == docMath ||
       DocumentTypes[destDoc] == docText ||
       DocumentTypes[destDoc] == docCSS ||
       DocumentTypes[destDoc] == docSource ||
       DocumentTypes[destDoc] == docAnnot))
    /* there is no selection. Nothing to do */
    return NULL;

  TtaGiveFirstSelectedElement (destDoc, &firstSelEl, &firstChar, &lastChar);
  if (firstSelEl)
    {
      parent = TtaGetParent (firstSelEl);
      if (TtaIsReadOnly (parent))
	/* do not create new elements within a read-only element */
	return NULL;
    }
  TtaOpenUndoSequence (destDoc, NULL, NULL, 0, 0);
  selEl = firstSelEl;
  child = NULL;
  /* Are we in a drawing? */
  docSchema = TtaGetDocumentSSchema (destDoc);
  SvgSchema = GetSVGSSchema (destDoc);
  elType = TtaGetElementType (selEl);
  if (elType.ElTypeNum == SVG_EL_SVG &&
      elType.ElSSchema == SvgSchema)
    SvgRootEl = selEl;
  else
    {
      elType.ElTypeNum = SVG_EL_SVG;
      elType.ElSSchema = SvgSchema;
      SvgRootEl = TtaGetTypedAncestor (firstSelEl, elType);
      if (SvgRootEl == NULL)
	/* the current selection is not in a SVG element, create one */
	{
	  selType = TtaGetElementType (firstSelEl);
	  if (strcmp (TtaGetSSchemaName (selType.ElSSchema), "HTML"))
	    {
	      /* selection is not in an HTML element. */
	      TtaCancelLastRegisteredSequence (destDoc);
	      return NULL;
	    }
	  SvgSchema = TtaNewNature (destDoc, docSchema, NULL, "SVG", "SVGP");
	  if (TtaIsSelectionEmpty ())
	    {
	      /* try to create the SVG here */
	      TtaCreateElement (elType, destDoc);
	      TtaGiveFirstSelectedElement (destDoc, &SvgRootEl, &firstChar, &lastChar);
	      LastInserted = TtaGetParent (SvgRootEl);
	      /* initialize SvgRoot Attribute */
	      attrTypeSvgRoot.AttrSSchema = elType.ElSSchema;
	      attrTypeSvgRoot.AttrTypeNum = SVG_ATTR_width_;
	      attrSvgRoot = TtaNewAttribute (attrTypeSvgRoot);
	      /* Search width and height attribute of the svg root element */
	      TtaSearchAttribute (attrTypeSvgRoot, SearchInTree,
				  TtaGetParent (TtaGetRootElement (sourceDoc)),
				  &elFound, &attrFound);
	      if (attrFound)
		{
		  len = TtaGetTextAttributeLength (attrFound);
		  buffer = (char *) TtaGetMemory (len + 1);
		  TtaGiveTextAttributeValue (attrFound, buffer, &len);
		  TtaSetAttributeText (attrSvgRoot, buffer, NULL, destDoc);
		  TtaFreeMemory (buffer);
		}
	      else
		{
		  /* give a default value of the svg size box */
		  TtaSetAttributeText (attrSvgRoot, "250", NULL, destDoc);
		}
	      TtaAttachAttribute (LastInserted, attrSvgRoot, destDoc);
	      ParseWidthHeightAttribute (attrSvgRoot, LastInserted, destDoc,
					 FALSE);
	      attrTypeSvgRoot.AttrTypeNum = SVG_ATTR_height_;
	      attrSvgRoot = TtaNewAttribute (attrTypeSvgRoot);

/*
 * a remplacer par:
 * elFound = TtaGetRootElement(sourceDoc);
 * TtaGetAttribute (elFound, attrTypeSvgRoot);
 *
 */
	      TtaSearchAttribute (attrTypeSvgRoot, SearchInTree, 
				  TtaGetParent (TtaGetRootElement (sourceDoc)),
				  &elFound, &attrFound);
	      if (attrFound)
		{
		  len = TtaGetTextAttributeLength (attrFound);
		  buffer = (char *) TtaGetMemory (len + 1);
		  TtaGiveTextAttributeValue (attrFound, buffer, &len);
		  TtaSetAttributeText (attrSvgRoot, buffer, NULL, destDoc);
		  TtaFreeMemory (buffer);
		}
	      else
		{
		  /* Give a default height value */
		  TtaSetAttributeText (attrSvgRoot, "250", NULL, destDoc);
		}
	      TtaAttachAttribute (LastInserted, attrSvgRoot, destDoc);
	      ParseWidthHeightAttribute (attrSvgRoot, LastInserted, destDoc,
					 FALSE);
	    }
	  else
	    {
	      /* look for a position around */
	      parent = firstSelEl;
	      do
		{
		  firstSelEl = parent;
		  parent = TtaGetParent (firstSelEl);
		  selType = TtaGetElementType (parent);
		}
	      while (selType.ElTypeNum != HTML_EL_BODY &&
		     selType.ElTypeNum != HTML_EL_Division);
	      
	      /* create and insert a SVG element here */
	      SvgRootEl = TtaNewElement (destDoc, elType);
	      TtaInsertSibling (SvgRootEl, firstSelEl, FALSE, destDoc);
	      firstSelEl = SvgRootEl;
	    }
	}
    }
  /* look for the element (sibling) in front of which the new element will be
     created */
  sibling = NULL;
  if (firstSelEl == SvgRootEl)
    parent = NULL;
  else
    {
      sibling = firstSelEl;
      found = FALSE;
      do
	{
	  parent = TtaGetParent (sibling);
	  if (parent)
	    {
	      elType = TtaGetElementType (parent);
	      if (elType.ElSSchema == SvgSchema &&
		  (elType.ElTypeNum == SVG_EL_g ||
		   elType.ElTypeNum == SVG_EL_SVG))
		found = TRUE;
	      else
		sibling = parent;
	    }
	}
      while (parent && !found);
    }

  if (!parent)
    {
      parent = SvgRootEl;
      sibling = TtaGetLastChild (SvgRootEl);
    }
  dispMode = TtaGetDisplayMode (destDoc);
  /* ask Thot to stop displaying changes made in the document */
  if (dispMode != DisplayImmediately)
    TtaSetDisplayMode (destDoc, DisplayImmediately/*DeferredDisplay*/);

  /* insert the svg library element */
  if (Method == CopySVGLibSelection)
    {
      TtaAskFirstCreation();
      curEl = TtaGetFirstChild (sourceEl);
      while (curEl)
	{
	  if (!sibling)
	    {
	      copiedElement = TtaCopyTree (curEl, sourceDoc, destDoc, parent);
	      if (copiedElement)
		{
		  TtaInsertFirstChild (&copiedElement, parent, destDoc);
		  /* check that id attribute is unique */
		  MakeUniqueName (copiedElement, destDoc, TRUE, FALSE);
		  sibling = copiedElement;
		}
	    }
	  else
	    {
	      copiedElement = TtaCopyTree (curEl, sourceDoc, destDoc, sibling);
	      if (copiedElement)
		{
		  TtaInsertSibling (copiedElement, sibling, FALSE, destDoc);
		  /* check that id attribute is unique */
		  MakeUniqueName (copiedElement, destDoc, TRUE, FALSE);
		  sibling = copiedElement;
		}
	    }
	  TtaNextSibling (&curEl);
	}
    }
  else if (Method == ReferToSVGLibSelection)
    {
      TtaAskFirstCreation();
      /* Create use element */
      elType.ElSSchema = SvgSchema;
      elType.ElTypeNum = SVG_EL_use_;
      useEl = TtaNewElement (destDoc, elType);
      attrTypeSvgUse.AttrSSchema = elType.ElSSchema;
      attrTypeSvgUse.AttrTypeNum = SVG_ATTR_xlink_href;
      attrSvgUse = TtaNewAttribute (attrTypeSvgUse);
      /* modify url of the Library Document to adapt it to the edited document */
      pathname = (char *) TtaGetMemory (MAX_LENGTH);
      basename = (char *) TtaGetMemory (MAX_LENGTH);
/*      relativeURL = (char *) TtaGetMemory (MAX_LENGTH);*/
      filename = (char *) TtaGetMemory (MAX_LENGTH);
      NormalizeURL (LibURL, sourceDoc, pathname, filename, NULL);
      NormalizeURL (filename, sourceDoc, pathname, basename, NULL);
      basename = GetBaseURL (destDoc);
      relativeURL = MakeRelativeURL (pathname, basename);
      TtaSetAttributeText (attrSvgUse, relativeURL, NULL, destDoc);
      TtaAttachAttribute (useEl, attrSvgUse, destDoc);
      ParseCSSequivAttribute (attrTypeSvgUse.AttrTypeNum, attrSvgUse, useEl,
			      destDoc, FALSE);
      /* insert use element */
      if (!sibling)
	TtaInsertFirstChild (&useEl, parent, destDoc);
      else
	TtaInsertSibling (useEl, sibling, FALSE, destDoc);

      FetchImage (destDoc, useEl, relativeURL, AMAYA_LOAD_IMAGE, NULL, NULL);
      copiedElement = useEl;
      TtaFreeMemory (pathname);
      TtaFreeMemory (basename);
      if (relativeURL)
	TtaFreeMemory (relativeURL);
    }
  TtaSetDisplayMode (destDoc, dispMode);
 
 /* adapt the size of the SVG root element if necessary */
  CheckSVGRoot (destDoc, copiedElement);
  SetGraphicDepths (destDoc, SvgRootEl);
  TtaCloseUndoSequence (destDoc);
  TtaSetDocumentModified (destDoc);
#endif /* _SVG */
  return copiedElement;
}

/*----------------------------------------------------------------------
  CopyOrReference
  This function starts the drop processus
  ----------------------------------------------------------------------*/
void CopyOrReference (Document doc, View view)
{
#ifdef _SVG
  int            i;

  i = TtaGetSelectedDocument();
  if (i == 0 ||
      (DocumentTypes[i] == docLibrary ||
       DocumentTypes[i] == docMath ||
       DocumentTypes[i] == docText ||
       DocumentTypes[i] == docCSS ||
       DocumentTypes[i] == docSource ||
       DocumentTypes[i] == docAnnot))
    /* there is no selection. Nothing to do */
    return;
#endif /* _SVG */
}


/*----------------------------------------------------------------------
  CreatePNGofSVGSelected
  This function creates a PNG by using screenshot on svg selected elements
  ----------------------------------------------------------------------*/
void CreatePNGofSVGFile (Document svgDoc, char *pngurl)
{
#ifdef _SVG
  unsigned char *screenshot = NULL;

  if (svgDoc != 0)
    {
      /* Get SVG geometry size to initialize x, y, width and height */
      /* Look at GetScreenshot in thotlib/image/picture.c */
      screenshot = GetScreenshot (GetWindowNumber (svgDoc, 1), pngurl);
      if (screenshot)
	TtaFreeMemory (screenshot);
    }
#endif /* _SVG */
}


/*----------------------------------------------------------------------
  GiveSVGXYWidthAndHeight
  Gets x min, y min, width max and height max
  (Cf.TtaGiveXYWH)
  ----------------------------------------------------------------------*/
void GiveSVGXYWidthAndHeight (Element el, Document svgDoc, View view,
			      int *x, int *y, int *width, int *height)
{
#ifdef _SVG
  Element      parent;
  ElementType  elType;
  /* Compare temporary value with static int */
  int          x_tmp, y_tmp, width_tmp, height_tmp;

  elType = TtaGetElementType (el);
  elType.ElTypeNum = SVG_EL_SVG;
  parent = TtaGetTypedAncestor (el, elType);

  TtaGiveBoxPosition (parent, svgDoc, 1, UnPixel, &x_tmp, &y_tmp);
  if (*x > x_tmp)
    *x = x_tmp;
  if (*y > y_tmp)
    *y = y_tmp;

  TtaGiveBoxSize (parent, svgDoc, 1, UnPixel, &width_tmp, &height_tmp);
  if (*width < width_tmp)
    *width = width_tmp;
  if (*height < height_tmp)
    *height = height_tmp;
#endif /*_SVG*/
}

/*----------------------------------------------------------------------
  CreateNewSVGFileofSVGSelected
  This function creates a SVG document by copying svg selected elements.
  Then opens it in a new window.
  Returns document number if operation succeed, 0 else if.
  inspired from CreateDoctype in EDITORactions.c module
  and InitDocAndView in init.c module
  ----------------------------------------------------------------------*/
Document CreateNewSVGFileofSVGSelected (char *url)
{
  Document             newSVGDoc = 0;
#ifdef _SVG
  Document             selDoc;
  Element              firstSelEl, lastSelEl, currentEl, siblingEl, copiedEl;
  Element              elFound, root, newEl, comment, leaf, doctype;
  ElementType          elType;
  Attribute            newAttr;
  AttributeType        attrType;
  Language             lang;
  View                 SVGView;
  char                 buffer[MAX_LENGTH], charsetName[MAX_LENGTH];
  int                  firstChar, lastChar, i;
  ThotBool             oldStructureChecking;
  
  newSVGDoc = TtaInitDocument ("SVG", "tmp", 0);
  if (newSVGDoc != 0)
    {
      TtaSetPSchema (newSVGDoc, "SVGP");
      TtaSetDocumentCharset (newSVGDoc, ISO_8859_1, FALSE);
      oldStructureChecking = TtaGetStructureChecking (newSVGDoc);
      TtaSetStructureChecking (FALSE, newSVGDoc);

      DocumentMeta[newSVGDoc] = DocumentMetaDataAlloc ();
      DocumentMeta[newSVGDoc]->form_data = NULL;
      DocumentMeta[newSVGDoc]->initial_url = NULL;
      DocumentMeta[newSVGDoc]->method = CE_ABSOLUTE;
      /* force the XML parsing */
      DocumentMeta[newSVGDoc]->xmlformat = TRUE;
      DocumentMeta[newSVGDoc]->xmlformat = FALSE;

      /* Set the document charset */
      TtaSetDocumentCharset (newSVGDoc, ISO_8859_1, FALSE);
      strcpy (charsetName , "iso-8859-1");
      DocumentMeta[newSVGDoc]->charset = TtaStrdup (charsetName);

      root = TtaGetMainRoot (newSVGDoc);
      elType = TtaGetElementType (root);

      /* create the SVG DOCTYPE element */
      elType.ElTypeNum = SVG_EL_DOCTYPE;
      doctype = TtaSearchTypedElement (elType, SearchInTree, root);
      CreateDoctype (newSVGDoc, doctype, L_SVG, FALSE, FALSE);

      root = TtaGetRootElement (newSVGDoc);
      elType.ElTypeNum = SVG_EL_XMLcomment;
      comment = TtaNewTree (newSVGDoc, elType, "");
      TtaSetStructureChecking (FALSE, newSVGDoc);
      TtaInsertSibling (comment, root, TRUE, newSVGDoc);
      TtaSetStructureChecking (TRUE, newSVGDoc);
      strcpy (buffer, " Created by ");
      strcat (buffer, TtaGetAppName());
      strcat (buffer, " ");
      strcat (buffer, TtaGetAppVersion());
      strcat (buffer, ", see http://www.w3.org/Amaya/ ");
      leaf = TtaGetFirstLeaf (comment);
      lang = TtaGetLanguageIdFromScript('L');
      TtaSetTextContent (leaf, (unsigned char *)buffer, lang, newSVGDoc);

      TtaSetNotificationMode (newSVGDoc, 1);
      TtaSetDocumentProfile (newSVGDoc, 0, 0);

      /* search the svg root element */
      elType.ElTypeNum = SVG_EL_SVG;
      /*      elFound = TtaSearchTypedElement(elType, SearchForward, TtaGetMainRoot (newSVGDoc));*/
      elFound = TtaGetRootElement (newSVGDoc);
      if (elFound)
        {
          /* Copy all the svg selected elements in the current document */
          selDoc = TtaGetSelectedDocument();
          TtaGiveFirstSelectedElement (selDoc, &firstSelEl, &firstChar, &i);
          TtaGiveLastSelectedElement (selDoc, &lastSelEl, &i, &lastChar);
	  
          /* Insert a group element with unique Id */
          elType.ElTypeNum = SVG_EL_g;
          newEl = TtaNewElement (newSVGDoc, elType);
          attrType.AttrSSchema = elType.ElSSchema;
          attrType.AttrTypeNum = SVG_ATTR_id;
          newAttr = TtaNewAttribute (attrType);
          TtaAttachAttribute (newEl, newAttr, newSVGDoc);
          TtaSetStructureChecking (FALSE, newSVGDoc);
          TtaInsertFirstChild(&newEl, elFound, newSVGDoc);
	  
          /* insert the copy of an element in the same order than selection */
          copiedEl = TtaCopyTree (firstSelEl, selDoc, newSVGDoc, newEl);
          TtaInsertFirstChild (&copiedEl, newEl, newSVGDoc);
          CheckSVGRoot (newSVGDoc, copiedEl);
          /* check "use" element in copied element */
          MakeStaticCopy (copiedEl, selDoc, newSVGDoc, url);
          /* save SVG geometry to adjust screenshot */
          GiveSVGXYWidthAndHeight (firstSelEl, selDoc, 1, &x_box, &y_box, &width_box, &height_box);

          currentEl = firstSelEl;
          siblingEl = copiedEl;
          TtaGiveNextSelectedElement (selDoc, &currentEl, &i, &i);
          while (currentEl)
            {
              copiedEl = TtaCopyTree (currentEl, selDoc, newSVGDoc, newEl);
              TtaInsertSibling (copiedEl, siblingEl, FALSE, newSVGDoc);
              /* check SVG root attribute width and height */
              CheckSVGRoot (newSVGDoc, copiedEl);
              MakeStaticCopy (copiedEl, selDoc, newSVGDoc, url);
              GiveSVGXYWidthAndHeight (currentEl, selDoc, 1, &x_box, &y_box, &width_box, &height_box);

              if (currentEl == lastSelEl)
                currentEl = NULL;
              else
                {
                  siblingEl = copiedEl;
                  TtaGiveNextSelectedElement (newSVGDoc, &currentEl, &i, &i);
                }
            }
          /* initialize id attribute text content of the groupe element*/
          TtaSetAttributeText (newAttr, "object", newEl, newSVGDoc);
          MakeUniqueName (newEl, newSVGDoc, TRUE, FALSE);
        }

      TtaSetStructureChecking (oldStructureChecking, newSVGDoc);
      /* get the width and height before opening the svg file */

      SVGView = TtaOpenMainView (newSVGDoc, "svg", 0, 0, width_box - x_box,
                                 height_box - y_box, FALSE, TRUE,
                                 1, /* window_id */
                                 0, /* page_id */
                                 2 /* page_position */ );
    }
#endif /* _SVG */
  return newSVGDoc;
}

/*----------------------------------------------------------------------
  MakeStaticCopy
  parameters:
  copiedEl: element to check (xlink href attribute)
  selDoc: document where becomes the copied element
  destDoc: document where is pasted the copied element
  ----------------------------------------------------------------------*/
void MakeStaticCopy (Element copiedEl, Document selDoc, Document destDoc,
			                                    char *newurl)
{
#ifdef _SVG
  Element               elFound, child, nextSibling, deletedEl;
  ElementType           elType;
  Attribute             attrFound;
  AttributeType         attrType;
  char                 *uriid, *basename, *relativeURI;
  char                 *pathname, *filename, *utf8path, *path;
  int                   length;
  ThotBool              IsUseElement = FALSE;
  ThotBool              stop = FALSE;

  elType = TtaGetElementType (copiedEl);
  if (elType.ElTypeNum == SVG_EL_symbol_)
    {/* it's simple selection  on a use element */
      copiedEl = TtaGetParent (copiedEl);
    }

  /* replace use element by it's content definition*/
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = SVG_ATTR_xlink_href;
  TtaSearchAttribute (attrType, SearchInTree, TtaGetParent (copiedEl),
		      &elFound, &attrFound);
  if (attrFound && (elFound == copiedEl || TtaIsAncestor (elFound, copiedEl)))
    {
      basename = (char *) TtaGetMemory (MAX_LENGTH);
      filename = (char *) TtaGetMemory (MAX_LENGTH);
      pathname = (char *) TtaGetMemory (MAX_LENGTH);
      while (!stop && elFound)
	{
	  /* get the old href attribute */
	  length = TtaGetTextAttributeLength (attrFound);
	  utf8path = (char *) TtaGetMemory (length + 2);
	  TtaGiveTextAttributeValue (attrFound, utf8path, &length);
	  path = (char *)TtaConvertMbsToByte ((unsigned char *)utf8path,
					      TtaGetDefaultCharset ());
	  TtaFreeMemory (utf8path);
	  if (!IsW3Path (path))
	    {
	      uriid = GetURIId (path);
	      NormalizeURL (DocumentURLs[selDoc], selDoc, pathname, filename, NULL);
	      /* todo: make another test to know if elFound is a use element */
	      if (uriid)
		{
		  /* use element: replace use element by it's content definition */
		  /* Copy what the use element refer from abstract tree          */
		  /* Then remove element use                                     */
		  child = TtaGetFirstChild (elFound);
		  /* todo: test symbols and defs */
		  elType = TtaGetElementType (child);
		  if (elType.ElTypeNum == SVG_EL_symbol_)
		    {
		      /* we only have to keep the under tree */
		      child = TtaGetFirstChild (child);
		    }
		  while (child)
		    {
		      nextSibling = child;
		      TtaNextSibling (&nextSibling);
		      /* move tree */
		      TtaRemoveTree (child, destDoc);
		      TtaInsertSibling (child, elFound, TRUE, destDoc);
		      /* copy use attribute (heritage) */
		      CopyUseElementAttribute (elFound, child, destDoc);
		      child = nextSibling;
		    }
		  IsUseElement = TRUE;
		  TtaFreeMemory (uriid);
		}
	      else
		{
		  /* img element or any other: */
		  basename = GetBaseURL (selDoc);
		  sprintf (pathname, "%s%s", basename, path);
		  SimplifyUrl (&pathname);
		  relativeURI = MakeRelativeURL (pathname, newurl);
		  TtaSetAttributeText (attrFound, relativeURI, elFound, destDoc);
		  TtaFreeMemory (relativeURI);
		}
	    }
	  TtaFreeMemory (path);
	  /* search next attribute */
	  deletedEl = elFound;
	  TtaSearchAttribute (attrType, SearchForward,
			      elFound, &elFound, &attrFound);
	  if (IsUseElement)
	    {
	      IsUseElement = False;
	      TtaDeleteTree (deletedEl, destDoc);
	    }
	  if (!TtaIsAncestor (elFound, copiedEl))
	    stop = TRUE;
	}
      TtaFreeMemory (basename);
      TtaFreeMemory (filename); 
      TtaFreeMemory (pathname);
    }
#endif /* _SVG */
}

/*----------------------------------------------------------------------
  CopyUseElementAttribute
  A REFLECHIR: le mode de copie pour l'ajout d'un mod�le dans la librairie
  lorsqu'on a la pr�sence d'un ou plusieurs �l�ments "use" r�f�rencant le
  m�me objet graphique.
  ----------------------------------------------------------------------*/
void CopyUseElementAttribute (Element useEl, Element destElement, Document doc)
{
#ifdef _SVG
  Attribute             newAttr, attrExist, attrFound = NULL;
  AttributeType         attrType;
  int                   length, val, oldVal, attrKind = 0;
  char                 *text = NULL, *ptr;
  char                  buffer[MAX_LENGTH];
  PresentationValue     pval, oldpval;

  TtaNextAttribute (useEl, &attrFound);
  /* Copy all attribute except xlink:href, x and y position */
  while (attrFound)
    {
      TtaGiveAttributeType (attrFound, &attrType, &attrKind);
      if (attrType.AttrTypeNum != SVG_ATTR_xlink_href)
	{
	  attrExist = TtaGetAttribute (destElement, attrType);
	  if (attrExist)
	    {
	      if (attrType.AttrTypeNum == SVG_ATTR_x ||
		  attrType.AttrTypeNum == SVG_ATTR_y)
		{
		  /* Update X, Y value */
		  switch (attrKind)
		    {
		    case 0:	/* enumerate */
		      val = TtaGetAttributeValue (attrFound);
		      oldVal = TtaGetAttributeValue (attrExist);
		      val += oldVal;
		      TtaSetAttributeValue (attrExist, val, destElement, doc);
		      break;
		    case 1:	/* integer */
		      val = TtaGetAttributeValue (attrFound);
		      oldVal = TtaGetAttributeValue (attrExist);
		      val += oldVal;
		      TtaSetAttributeValue (attrExist, val, destElement, doc);
		      break;
		    case 2:	/* text */
		      length = TtaGetTextAttributeLength (attrFound) + 2;
		      text = (char * ) TtaGetMemory (length);
		      TtaGiveTextAttributeValue (attrFound, text, &length);
		      ptr = text;
		      ParseNumber (ptr, &pval);
		      TtaFreeMemory (text);

		      length = TtaGetTextAttributeLength (attrFound) + 2;
		      text = (char * ) TtaGetMemory (length);
		      TtaGiveTextAttributeValue (attrExist, text, &length);
		      ptr = text;
		      ParseNumber (ptr, &oldpval);
		      TtaFreeMemory (text);
		      /* You can change the unit by adding it after %d */
		      sprintf (buffer, "%d",
			       pval.typed_data.value + oldpval.typed_data.value);
		      TtaSetAttributeText (attrExist, buffer, destElement, doc);
		      break;
		    case 3:	/* reference */
		      break;
		    }
		  SVGAttributeComplete (attrExist, destElement, doc);
		}
	      else
		{
		  /* change the value of the concerned attribute */
		  /*
		    TODO une fonction g�n�rique qui remplace la valeur
		    courante d'un attribut
		    void ChangeAttributeValue (Element curEl,
		    Attribute currentAttr, int TypeOfValue, char *value)
		  */
		}
	    }
	  else
	    {
	      newAttr = TtaNewAttribute (attrType);
	      TtaAttachAttribute (destElement, newAttr, doc);
	      switch (attrKind)
		{
		case 0:	/* enumerate */
		  val = TtaGetAttributeValue (attrFound);
		  TtaSetAttributeValue (newAttr, val, destElement, doc);
		  break;
		case 1:	/* integer */
		  val = TtaGetAttributeValue (attrFound);
		  TtaSetAttributeValue (newAttr, val, destElement, doc);
		  break;
		case 2:	/* text */
		  length = TtaGetTextAttributeLength (attrFound) + 2;
		  text = (char * ) TtaGetMemory (length);
		  TtaGiveTextAttributeValue (attrFound, text, &length);
		  TtaSetAttributeText (newAttr, text, destElement, doc);
		  if (attrType.AttrTypeNum == SVG_ATTR_style_)
		    ApplyCSSRules (destElement, text, doc, FALSE);
		  TtaFreeMemory (text);
		  break;
		case 3:	/* reference */
		  break;
		}
	      if (attrType.AttrTypeNum != SVG_ATTR_style_)
		SVGAttributeComplete (newAttr, destElement, doc);
	    }
	}
      TtaNextAttribute (useEl, &attrFound);
    }
#endif /* _SVG */
}


/*----------------------------------------------------------------------
  AddingModelIntoLibraryFile
  this function updates library file by adding HTML element of a new 
  SVG model.
  ----------------------------------------------------------------------*/
void AddingModelIntoLibraryFile (Document libDoc, char *newURL)
{
#ifdef _SVG
  Element         imgEl, rootEl, rowEl, cellEl, anchorEl, table, newcellEl;
  Element         insertedEl=0, textEl, testEl;
  ElementType     elType;
  Attribute       attr;
  AttributeType   attrType;
  char           *basename, *class_, *relativeURL = NULL;
  ThotBool        oldStructureChecking;

  basename = (char *) TtaGetMemory (MAX_LENGTH);
  /*  relativeURL = (char *) TtaGetMemory (MAX_LENGTH);*/
  class_ = (char *) TtaGetMemory (MAX_LENGTH);

  oldStructureChecking = TtaGetStructureChecking (libDoc);
  TtaSetStructureChecking (FALSE, libDoc);

  /*Insert or paste new element at the beginning of the table */
  rootEl = TtaGetRootElement (libDoc);
  elType = TtaGetElementType (rootEl);

  elType.ElTypeNum = HTML_EL_Table_;
  table = TtaSearchTypedElement (elType, SearchInTree, rootEl);
  if (!table)
    { /* it's a new library, creates it with 3 column cell */
      /* Now just put a table starter */
      elType.ElTypeNum = HTML_EL_Table_;
      table = TtaNewTree (libDoc, elType, "");
      elType.ElTypeNum = HTML_EL_BODY;
      TtaInsertFirstChild (&table,
			   TtaSearchTypedElement (elType, SearchInTree, rootEl), 
			   libDoc);
      attrType.AttrSSchema = elType.ElSSchema;
      attrType.AttrTypeNum = HTML_ATTR_Border;
      attr = TtaGetAttribute (table, attrType);
      if (attr == NULL)
	{
	  attr = TtaNewAttribute (attrType);
	  if (attr != NULL)
	    TtaAttachAttribute (table, attr, libDoc);
	}
      TtaSetAttributeValue (attr, 1, table, libDoc);
      elType.ElTypeNum = HTML_EL_Table_row;
      rowEl = TtaSearchTypedElement (elType, SearchInTree, rootEl);
      if (rowEl)
	{ /* Add 2 columns cell */
	
	  cellEl = TtaGetFirstChild (rowEl);
	  elType = TtaGetElementType (cellEl);
	  while (elType.ElTypeNum == HTML_EL_Data_cell)
	    {
	      cellEl = TtaGetFirstChild (rowEl);
	      elType = TtaGetElementType (cellEl);
	    }
	  newcellEl = TtaNewTree (libDoc, elType, "");
	  TtaInsertSibling (newcellEl, cellEl, FALSE, libDoc);
	  newcellEl = TtaNewTree (libDoc, elType, "");
	  TtaInsertSibling (newcellEl, cellEl, FALSE, libDoc);	    
	}   
    }
  else
    {
      elType.ElTypeNum = HTML_EL_Table_row;
      /* search the last table row element */
      /* 2 methods
	 first one: search table body element and then take the lastchildelement
	 second one: search the table row and then take the next sibling element*/
      testEl = TtaSearchTypedElement (elType, SearchInTree, rootEl);
      while (testEl != NULL)
	{
	  insertedEl = testEl;
	  TtaNextSibling (&testEl);
	}
      /* create a new table row element */
      elType.ElTypeNum = HTML_EL_Table_row;
      rowEl = TtaNewTree (libDoc, elType, "");
    }
  TtaInsertSibling (rowEl, insertedEl, FALSE, libDoc);
  TtaOpenUndoSequence (libDoc, rowEl, rowEl, 0, 0);
  
  elType.ElTypeNum = HTML_EL_Table_;
  table = TtaSearchTypedElement (elType, SearchInTree, rootEl);
  if (table)
    CheckAllRows (table, libDoc, FALSE, FALSE);
  
  /*
   * Edit the first column of the new row
   */
  while (elType.ElTypeNum != HTML_EL_Data_cell)
    {
      cellEl = TtaGetFirstChild (rowEl);
      elType = TtaGetElementType (cellEl);
    }
  
  /* create an anchor element */
  elType.ElTypeNum = HTML_EL_Anchor;
  anchorEl = TtaNewTree (libDoc, elType, "");
  TtaInsertFirstChild (&anchorEl, cellEl, libDoc);
  /* initialize anchor href attribute (relative URL) */
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = HTML_ATTR_HREF_;
  attr = TtaNewAttribute (attrType);
  if (attr != NULL)
    TtaAttachAttribute (anchorEl, attr, libDoc);
  /* newURL must be a svg URL */
  ExtractLibraryPrefixFilename (newURL);
  /* �crire fonction de r�cup�ration de l'id cr�� lors de CreateSVGFileofSVGSelected 
     A EFFECTUER*/
  strcat (newURL, ".svg#object");
  basename = GetBaseURL (libDoc);
  relativeURL = MakeRelativeURL (newURL, basename);
  TtaSetAttributeText (attr, relativeURL, anchorEl, libDoc);
  if (relativeURL)
    TtaFreeMemory (relativeURL);

  /* create an image element */
  elType.ElTypeNum = HTML_EL_PICTURE_UNIT;
  imgEl = TtaNewTree (libDoc, elType, "");
  TtaInsertFirstChild (&imgEl, anchorEl, libDoc);
  /* initialize image source "src" attribute (relative URL) */
  attrType.AttrTypeNum = HTML_ATTR_SRC;
  attr = TtaNewAttribute (attrType);
  if (attr != NULL)
    TtaAttachAttribute (imgEl, attr, libDoc);
  ExtractLibraryPrefixFilename (newURL);
  strcat (newURL, ".png");
  relativeURL = MakeRelativeURL (newURL, basename);
  TtaSetAttributeText (attr, relativeURL, imgEl, libDoc);
  if (relativeURL)
    TtaFreeMemory (relativeURL);
  /* initialize image "class" attribute */
  attrType.AttrTypeNum = HTML_ATTR_Class;
  attr = TtaNewAttribute (attrType);
  if (attr != NULL)
    TtaAttachAttribute (imgEl, attr, libDoc);
  strcpy (class_, "image");
  TtaSetAttributeText (attr, class_, imgEl, libDoc);

  /*
   * Edit the second column of the new row
   */
  TtaNextSibling (&cellEl);
  if (cellEl)
    {
      /* initialize cellEl "class" attribute */
      attrType.AttrTypeNum = HTML_ATTR_Class;
      attr = TtaNewAttribute (attrType);
      if (attr != NULL)
	TtaAttachAttribute (cellEl, attr, libDoc);
      strcpy (class_, "g_title");
      TtaSetAttributeText (attr, class_, cellEl, libDoc);
      elType.ElTypeNum = HTML_EL_TEXT_UNIT;
      textEl = TtaNewTree (libDoc, elType, "");
      TtaInsertFirstChild (&textEl, cellEl, libDoc);
      TtaSetTextContent (textEl, (unsigned char *)"Title", TtaGetDefaultLanguage (), libDoc);
    }
  /* let selection on it */
  TtaSelectElement (libDoc, textEl);

  /*
   * Edit the third column of the new row
   */
  TtaNextSibling (&cellEl);
  if (cellEl)
    {
      /* initialize cellEl "class" attribute */
      attrType.AttrTypeNum = HTML_ATTR_Class;
      attr = TtaNewAttribute (attrType);
      if (attr != NULL)
	TtaAttachAttribute (cellEl, attr, libDoc);
      strcpy (class_, "g_comment");
      TtaSetAttributeText (attr, class_, cellEl, libDoc);
      elType.ElTypeNum = HTML_EL_TEXT_UNIT;
      textEl = TtaNewTree (libDoc, elType, "");
      TtaInsertFirstChild (&textEl, cellEl, libDoc);
      TtaSetTextContent (textEl, (unsigned char *)"Comment", TtaGetDefaultLanguage (), libDoc);
    }

  /* set stop button */
  ActiveTransfer (libDoc);
  FetchImage (libDoc, imgEl, NULL, 0, NULL, NULL);
  ResetStop (libDoc);

  /* Free memory */  
  TtaFreeMemory (basename);
  TtaFreeMemory (class_);  

  TtaSetStructureChecking (oldStructureChecking, libDoc);
  TtaCloseUndoSequence (libDoc);
  TtaSetDocumentModified (libDoc);
#endif /* _SVG */
}


/*----------------------------------------------------------------------
  AddSVGModelIntoLibrary
  This function creates png files and svg files associated to the 
  current selection. Then it adds HTML element into libraryDocument.
  Parameters:
  libraryDoc : document to update if it's not a newLib file
  newLib : TRUE if it's a new library
  title : Title of the new Library
  ----------------------------------------------------------------------*/
void AddSVGModelIntoLibrary (Document libraryDoc, ThotBool newLib, char *title)
{
#ifdef _SVG
  char      *newURL, *suffix, *libraryURL, *tmp, *filename, *dirname;
  ThotBool   repExist, ok = FALSE;
  Document   svgDoc, tmpDoc;
  long int   i = 0;

  libraryURL = NULL;
  suffix = (char *) TtaGetMemory (10);

  if (!newLib)
    {
      if (libraryDoc != 0)
	{
	  libraryURL = (char *) TtaGetMemory (MAX_LENGTH);
	  strcpy (libraryURL, DocumentURLs[libraryDoc]);
	}
      else
	{
	  /* initialize string buffer */
	  tmp = GetLibraryPathFromTitle (title);
	  if (tmp)
	    {
	  libraryURL = (char *) TtaGetMemory (MAX_LENGTH);
	  strcpy (libraryURL, tmp);
	    }
	  /* Open it */
	  tmpDoc = TtaNewDocument ("HTML", "temp_library");
	  DocumentTypes[tmpDoc] = docLibrary;
	  DontReplaceOldDoc = TRUE;
	  libraryDoc = GetAmayaDoc (libraryURL, NULL, tmpDoc, tmpDoc,
				    CE_ABSOLUTE, FALSE, FALSE, FALSE);
	}
    }
  else
    {
      libraryURL = (char *) TtaGetMemory (strlen (LastURLCatalogue) + 1);
      strcpy (libraryURL, LastURLCatalogue);
    }
  /* Create library directory if it doesn't exist yet */
  /* this directory will contain all svg and png file corresponding to the library */
  ExtractLibraryPrefixFilename (libraryURL);
  repExist = CreateLibraryDirectory (libraryURL);

  if (repExist)
    {
      /* We save SVG file and PNG file into */
      filename = (char *) TtaGetMemory (MAX_LENGTH);
      dirname = (char *) TtaGetMemory (MAX_LENGTH);
      TtaExtractName (libraryURL, dirname, filename);
      sprintf (libraryURL, "%s%c%s", libraryURL, DIR_SEP, filename);
      TtaFreeMemory (filename);
      TtaFreeMemory (dirname);
    }
  /* else we save SVG file and PNG file in the same directory than library file */

  /* Create SVG file corresponding to the current selection*/
  strcpy (suffix, ".svg");
  newURL = MakeUniqueSuffixFilename (libraryURL, suffix);

  svgDoc = CreateNewSVGFileofSVGSelected(newURL);
  if (svgDoc)
    {
      SaveAsText = FALSE;
      ok = TtaExportDocumentWithNewLineNumbers (svgDoc, newURL, "SVGT", FALSE);
    }

  while (i < 1000000)
    {
      i++; /* wait !!!!!! */
    }

  /*  Create PNG file corresponding to the svg file */
  ExtractLibraryPrefixFilename (newURL);
  strcat (newURL, ".png");
  CreatePNGofSVGFile (svgDoc, newURL);
  
  /* edit (update) library file */
  if (newLib)
    libraryDoc = CreateNewLibraryFile (LastURLCatalogue, NewLibraryTitle);
  AddingModelIntoLibraryFile (libraryDoc, newURL);

  if (newURL)
    TtaFreeMemory (newURL);
  TtaFreeMemory (suffix);
  if (libraryURL)
    TtaFreeMemory (libraryURL);
#endif /* _SVG */
}


/*----------------------------------------------------------------------
  CreateLibraryDirectory
  this function creates a Directory linked to the library path.
  ----------------------------------------------------------------------*/
ThotBool CreateLibraryDirectory (char *libDirectory)
{
  ThotBool    RepCreated = FALSE;
#ifdef _SVG
  ExtractLibraryPrefixFilename (libDirectory);
  RepCreated = TtaCheckMakeDirectory (libDirectory, TRUE);
#endif /* _SVG */
  return RepCreated;
}

/*----------------------------------------------------------------------
  MakeUniqueSuffixFilename
  Creates and allocates a unique suffix file NAME.
  If the NAME is already used, add a number at the end of the prefix value.
  parameters: the library URL
  ----------------------------------------------------------------------*/
char *MakeUniqueSuffixFilename (char * libraryURL, char *suffix)
{
  char     *pathname = NULL;
#ifdef _SVG
  char     *dirname, *filename;
  int       i = 1;

  dirname = (char *) TtaGetMemory (MAX_LENGTH);
  filename = (char *) TtaGetMemory (MAX_LENGTH);
  pathname = (char *) TtaGetMemory (MAX_LENGTH);

  TtaExtractName (libraryURL, dirname, filename);
  strcpy (pathname, dirname);
  ExtractLibraryPrefixFilename (filename);
  sprintf (pathname, "%s%c%s%s", pathname, DIR_SEP, filename, suffix);
  while (TtaFileExist (pathname))
    {
      strcpy (pathname, dirname);
      sprintf (pathname, "%s%c%s%d%s", pathname, DIR_SEP, filename, i, suffix);
      i++;
    }
  TtaFreeMemory (dirname);
  TtaFreeMemory (filename);
#endif /* _SVG */
  return pathname;
}


/*----------------------------------------------------------------------
  ExtractLibraryPrefixFilename
  This function extracts the prefix file name by removing the suffix.
  Parameter : URL with filename
  ----------------------------------------------------------------------*/
void ExtractLibraryPrefixFilename (char *filename)
{
#ifdef _SVG
  char   *ptr, *oldptr;

  if (filename)
    {
      ptr = oldptr = filename;
      ptr = strrchr (oldptr, '.');
      if (ptr)
	*ptr = EOS;
    }
#endif /* _SVG */
}


/*----------------------------------------------------------------------
  WriteInterSessionLibraryFileManager
  This function writes in "lib_files.dat" all the data concerning
  current library file.
  returns True if the operation succeed, False else if.
  ----------------------------------------------------------------------*/
ThotBool WriteInterSessionLibraryFileManager ()
{
  ThotBool       succeed = FALSE;
#ifdef _SVG
  ListUriTitle  *curList = HeaderListUriTitle;
  FILE          *file;
  char          *url_home, *urlstring, *app_home;

  url_home = (char *) TtaGetMemory (MAX_LENGTH);
  urlstring = (char *) TtaGetMemory (MAX_LENGTH);

  /* Read lib_files.dat into APP_HOME directory */
  app_home = TtaGetEnvString ("APP_HOME");
  sprintf (url_home, "%s%clib_files.dat", app_home, DIR_SEP); 
  /* ./.amaya/lib_files.dat */
  file = TtaWriteOpen (url_home);
  if (file)
    {
      succeed = TRUE;
      /* necessit� de tester le nombre d'uri a placer dans lib_files.dat
	 pour �viter d'avoir un EOL � la fin du fichier (Pas sous GTK)*/
      while (curList)
	{
	  if (curList->customLibrary)
	    /* it's a custom Library, add it to APP_HOME/lib_files.dat */
	    fprintf (file, "%s\n", curList->URI);
	  else
	    fprintf (file, "%s", curList->URI);
	  curList = curList->next;
	}
    }
  TtaWriteClose (file);
  TtaFreeMemory (url_home);
  TtaFreeMemory (urlstring);
#endif /* _SVG */
  return succeed;
}


/*----------------------------------------------------------------------
  SearchGraphicalObjectByTitle
  A EFFECTUER
  Function that search e graphical object by title.
  If it exists, return titles of the libraries where it appears.
  (It may be several libraries)
  ----------------------------------------------------------------------*/
void SearchGraphicalObjectByTitle (char *GraphicalObjectTitle)
{
#ifdef _SVG
  ListUriTitle      *curList = HeaderListUriTitle;
  Document           libDoc, res;
  Element            goTitle, root, rowEl;
  ElementType        elType;
  char              *title;
  ThotBool           goFound = FALSE;
  int                length;
  Language           lang;

  lang = TtaGetDefaultLanguage ();

  while (curList && !goFound)
    {
      /* open curList library (Cf Makebook) and look after graphical object title */
      libDoc = TtaNewDocument("HTML", "tmp");
      res = GetAmayaDoc (curList->URI, NULL, libDoc, 0, CE_MAKEBOOK,
			 FALSE, NULL, NULL);
      if (res)
	{
	  root = TtaGetRootElement (res);
	  elType = TtaGetElementType (root);
	  elType.ElTypeNum = HTML_EL_Table_row;
	  rowEl = TtaSearchTypedElement (elType, SearchForward, root);
	  /* search on class attribute */
/*	  attrType.AttrSSchema = elType.ElSSchema;
	  attrType.AttrTypeNum = HTML_ATTR_Class;*/
	  while (rowEl && !goFound)
	    {
	      goTitle = TtaGetFirstChild (rowEl);
	      TtaNextSibling (&goTitle);
	      length = TtaGetTextLength (goTitle);
	      title = (char *) TtaGetMemory (length + 1);
	      TtaGiveTextContent (goTitle, (unsigned char *)title, &length, &lang);
	      if (!strcmp (GraphicalObjectTitle, title))
		{
		  goFound = TRUE;
		}
	      rowEl = TtaSearchTypedElement (elType, SearchForward, rowEl);
	    }
	  /* free memory only if it's not a loaded document */
/*	  FreeDocumentResource (res);
	  TtaCloseDocument (res);*/
	}
      curList = curList->next;
    }
  if (goFound)
    {
      /* open it */

    }
#endif /* _SVG */
}

/*----------------------------------------------------------------------
  RemoveLibraryModel
  A EFFECTUER
  this function removes a library model
  Parameters: the deleted document and the deleted element
  ----------------------------------------------------------------------*/
void RemoveLibraryModel (Document deletedDoc, Element deletedEl)
{
#ifdef _SVG
  ElementType    elType;

  elType = TtaGetElementType (deletedEl);
  TtaOpenUndoSequence (deletedDoc, NULL, NULL, 0, 0);
  if (elType.ElTypeNum == HTML_EL_Table_row)
    {
      /* we only have to remove deletedEl */
      TtaRegisterElementDelete (deletedEl, deletedDoc);
      TtaRemoveTree (deletedEl, deletedDoc);
    }
  else
    {
      /* find it */
    }
  TtaCloseUndoSequence(deletedDoc);
#endif /* _SVG */
}



/*----------------------------------------------------------------------
   OpenLibraryCallback
   The Address text field in a document window has been modified by the user
   Load the corresponding document in that window.
  ----------------------------------------------------------------------*/
void OpenLibraryCallback (Document doc, View view, char *text)
{
#ifdef _SVG
  ListUriTitle      *curList = HeaderListUriTitle;
  char              *url = NULL;
  ThotBool           change;

  change = FALSE;
  if (text)
    {
      /* remove any trailing '\n' chars that may have gotten there
	 after a cut and paste */
      change = RemoveNewLines (text);

      if (HeaderListUriTitle)
	{
	  while (curList)
	    {
	      if (strcmp (text, curList->Title) == 0)
		{
		  url = (char *)TtaGetMemory (strlen (curList->URI) + 1);
		  strcpy (url, curList->URI);
		  break;
		}
	      else
		curList = curList->next;
	    }
	}
      if (url == NULL)
	return;
      else
	{
	  DontReplaceOldDoc = FALSE;
	  /* TESTER si le document a �t� modifi� avant d'ouvrir le nouveau document */
	  if (TtaIsDocumentModified (doc))
	    CanReplaceCurrentDocument (doc, 1);
	  else
	    {
	      CurrentDocument = doc;
	      GetAmayaDoc (url, NULL, CurrentDocument,
			   CurrentDocument, CE_ABSOLUTE, TRUE,
			   NULL, NULL);
	    }
	  TtaFreeMemory (url);
	}
    }
#endif /* _SVG */
}
