/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996-2002
 *
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *
 * libmanag.c
 *
 * This module contains functions to handle Amaya SVG Library
 */

/* header file */
#define THOT_EXPORT
#include "amaya.h"
#include "css.h"
#include "trans.h"
#include "view.h"
#include "content.h"
#include "init_f.h"
#include "zlib.h"
#include "profiles.h"
#include "SVG.h"

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
#include "html2thot_f.h"
#include "Mathedit_f.h"
#include "SVGedit_f.h"
#include "SVGbuilder_f.h"
#include "MENUconf_f.h"
#include "query_f.h"
#include "styleparser_f.h"
#include "templates_f.h"
#include "trans_f.h"
#include "transparse_f.h"
#include "UIcss_f.h"


/* fichiers d'entêtes utilent */
#include "libmanag.h" 
#include "libmanag_f.h"

#ifdef _SVGLIB
/* common local variables */
static char    buffermem[MAX_LENGTH];

/* Variable to handle Use Library Interface (context of png selection)*/
Document	LibraryDocDocument = 0;
static Element	LibraryDocElement = NULL;

/* Inserted boolean */
static ThotBool docModified = FALSE;

/* Structure List URI-Title */
typedef struct _Library_URITitle
{
  char          *URI;
  char          *Title;
  int            indice;
  ThotBool       customLibrary;
  struct _Library_URITitle *next;
} ListUriTitle;
/* List header */
static ListUriTitle *HeaderListUriTitle = NULL;

/* variable to handle inter session file */
static ThotBool LibraryStructureModified = FALSE;

/* variable to handle SVGLibrary dialogue */
static char svgFilter[MAX_LENGTH];
static char libraryFilter[MAX_LENGTH];
static char baseDirectory[MAX_LENGTH];
static char baseDirectory1[MAX_LENGTH];
static char SaveLibraryTitleSelection[MAX_LENGTH];
static char NewLibraryTitle[MAX_LENGTH];
static char LastURLSVG[MAX_LENGTH];
static char LastURLCatalogue[MAX_LENGTH];

#ifndef _WINDOWS
#include  "Libsvg.xpm"
#include  "LibsvgNo.xpm"
#endif /* _WINDOWS */

static Pixmap   iconLibsvg;
static Pixmap   iconLibsvgNo;
static int      LibSVGButton;

/* pour windows
#ifdef _WINDOWS
#include "wininclude.h"
#define iconLibsvg   28
#define iconLibsvgNo 28
#endif _WINDOWS
*/

#endif /* _SVGLIB */

/*----------------------------------------------------------------------
  IsCurrentSelectionSVG
  Check if the current selection is only svg
  ----------------------------------------------------------------------*/
ThotBool IsCurrentSelectionSVG ()
{
  ThotBool     IsSVG = FALSE;
#ifdef _SVGLIB
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
	  /* register in the do queue the element that will be copied */
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
#endif /* _SVGLIB */
  return IsSVG;
}


/*----------------------------------------------------------------------
  Copy_svg_information_tree                                  
  
  Check if the selection is a SVG element        
  and copy it into a temporary buffer

  Initializes add svg model Form
  ----------------------------------------------------------------------*/
void CopySvgInformationTree (Document doc, View view)
{
#ifdef _SVGLIB
#ifndef _WINDOWS
  char           bufButton[MAX_LENGTH];
  char          *buffer_list;
  int            i, nbr;
#endif /* _WINDOWS */

  /* Initialize Structure if it's not yet done */
  InitSVGLibraryManagerStructure();

#ifndef _WINDOWS
  SaveLibraryTitleSelection[0] = EOS;
  /* Check the current selection */
  if (IsCurrentSelectionSVG ())
    {
      i = 0;
      strcpy (&bufButton[i], TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
      i += strlen (&bufButton[i]) + 1;
      strcpy (&bufButton[i], TtaGetMessage (AMAYA, AM_SVGLIB_CREATE_NEW_CATALOGUE));

      TtaNewSheet (BaseLibrary + AddSVGModel, TtaGetViewFrame (doc, view), 
		   TtaGetMessage (AMAYA, AM_SVGLIB_ADD_SVG_MODEL_DIALOG), 2,
		   bufButton, TRUE, 2, 'L', D_CANCEL);

      /* Catalogue Or URI text zone */
      buffer_list = InitSVGBufferForComboBox ();
      nbr = SVGLibraryListItemNumber (buffer_list);
      TtaNewSizedSelector (BaseLibrary + SVGLibCatalogueTitle, BaseLibrary + AddSVGModel,
			   TtaGetMessage (AMAYA, AM_SVGLIB_CATALOGUE_TITLE),
			   nbr, buffer_list, 50, 3,
			   FALSE, FALSE, TRUE);
      TtaFreeMemory (buffer_list);

      /* activates the Add Library Model Dialogue  */
      TtaSetDialoguePosition ();
      TtaShowDialogue (BaseLibrary + AddSVGModel, TRUE);
      TtaWaitShowDialogue ();
    }
  else
    {
      /* show an error selection dialogue */
      InitInfo (TtaGetMessage (AMAYA, AM_ERROR),
		TtaGetMessage (AMAYA, AM_SVGLIB_NO_SVG_SELECTION));
    }
#else /* _WINDOWS */
  CreatePasteLibraryModelDlgWindow (TtaGetViewFrame (document, view));
#endif /* _WINDOWS */
#endif /* _SVGLIB */
}


/*----------------------------------------------------------------------
  CallbackLibrary
  handle return of Library form
  ----------------------------------------------------------------------*/
void CallbackLibrary (int ref, int typedata, char *data)
{
#ifdef _SVGLIB
  int                 val, TypeFile;
  Document            svgDoc, res, libDoc;
  char               *pathname, *documentname, *id, *buffer;
  char                tempname[MAX_LENGTH];
  char               *tempfile, *libraryTitle;
  char               *dirname, *filename;
  Element             copiedElement, Elt = NULL;
  PRule               PRuleSearch;

  val = (int) data + 1;
  switch (ref - BaseLibrary)
    {
    case FormLibrary:
      TtaDestroyDialogue (BaseLibrary + FormLibrary);
      /* Initialize path parameter */
      pathname = TtaGetMemory (MAX_LENGTH);
      documentname = TtaGetMemory (MAX_LENGTH);
      NormalizeURL (buffermem, LibraryDocDocument, pathname, documentname, NULL);
      /* Open svg file in a document (Cf. MakeBook) */
      svgDoc = TtaNewDocument("SVG","tmp");
      res = GetAmayaDoc (pathname, NULL, svgDoc, 0, CE_MAKEBOOK,
			 FALSE, NULL, NULL, ISO_8859_1/* TtaGetDefaultCharset */);
      TtaFreeMemory (pathname);
      TtaFreeMemory (documentname);
      switch (val)
	{
	case CopySVGLibSelection:
	  id = GetURIId (buffermem);
	  if (id)
	    {
	      Elt = SearchSVGElement (svgDoc, id);
	      TtaFreeMemory (id);
	    }
	  DocumentTypes[svgDoc] = docSVG;
	  if (Elt)
	    {
	      /* Copy element */
	      copiedElement = PasteLibraryGraphicElement (Elt, svgDoc, CopySVGLibSelection);
	    }
	  FreeDocumentResource (svgDoc);
	  TtaCloseDocument (svgDoc);
	  break;

	case ReferToSVGLibSelection:
	  /* Only paste a use element into the clicked view  */
	  copiedElement = PasteLibraryGraphicElement (Elt, svgDoc, ReferToSVGLibSelection);
	  /* Free external resource before displaying svg image */
	  FreeDocumentResource (svgDoc);
	  TtaCloseDocument (svgDoc);
	  break;

	default:
	  FreeDocumentResource (svgDoc);
	  TtaCloseDocument (svgDoc);
	  break;
	}
      break;

    case AddSVGModel:
      switch (val)
	{
	case 2: /* Confirm button */
	  /* get the title selection */
	  if (SaveLibraryTitleSelection[0] == EOS)
	    break;
	  else
	    {
	      /* create all the HTML element, *.svg file and *.png file */
	      AddSVGModelIntoLibrary (0, FALSE, SaveLibraryTitleSelection);
	      TtaDestroyDialogue (ref);
	    }
	  break;
	case 3: /* show dialogue create new catalogue */
	  CreateNewCatalogueDialogue (TtaGetSelectedDocument(), 1);
	  break;

	default:
	  break;
	}
      break;

    case NewSVGLibrary:
      buffer = data;
      switch (val)
	{
	case 2: /* Confirm button, create this library document
		   and add this catalogue into the library list */
	  if (!SearchURIinCurrentSVGLibraryList (LastURLCatalogue))
	    {
	      if (!TtaFileExist (LastURLCatalogue))
		{
		  dirname = (char *) TtaGetMemory (MAX_LENGTH);
		  filename = (char *) TtaGetMemory (MAX_LENGTH);
		  TtaExtractName (LastURLCatalogue, dirname, filename);
		  if (TtaCheckDirectory (dirname) &&
		      IsLibraryName (filename))
		    {
		      AddLibraryDataIntoStructure (TRUE, LastURLCatalogue, NewLibraryTitle);
		      LibraryStructureModified = TRUE;
		      /* create all the HTML element, *.svg file and *.png file */
		      AddSVGModelIntoLibrary (0, TRUE, NewLibraryTitle);
		      TtaDestroyDialogue (ref);
		      TtaDestroyDialogue (BaseLibrary + AddSVGModel);
		    }
		  else
		    TtaNewLabel (BaseLibrary + SVGLibraryLabel2, BaseLibrary + NewSVGLibrary,
				 TtaGetMessage (AMAYA, AM_SVGLIB_MISSING_TITLE));
		  TtaFreeMemory (filename);
		  TtaFreeMemory (dirname);
		}
	      else /* this file already exist, just open it */
		{
		  libDoc = TtaNewDocument("HTML", "tmp");
		  InNewWindow = TRUE;
		  res = GetAmayaDoc (LastURLCatalogue, NULL, libDoc, 0, CE_ABSOLUTE,
				     FALSE, NULL, NULL, ISO_8859_1/* TtaGetDefaultCharset()*/);
		}
	    }
	  else /* this file already be in the structure list */
	    TtaDestroyDialogue (ref);
	  break;
	case 4: /* Browser button, show SVG Library File browser dialogue */
	  TypeFile = 2;
	  ShowLibraryBrowser (TypeFile);

	default:
	  break;
	}
      break;


    case NewSVGFileURL: /* 5 */
      buffer = data;
      break;
    case SVGLibCatalogueTitle: /* 6 */
      strcpy (SaveLibraryTitleSelection, data);
      buffer = data;
      /* Is this Library exist? */
      /* Search this data into HeaderListUriTitle */
      if (data)
	{
	  buffer = IsSVGCatalogueExist (data);
	}
      break;
    case NewSVGLibraryTitle:
      strcpy (NewLibraryTitle , data);
      break;
    case SVGLibraryURL:
      strcpy (LastURLCatalogue, data);
      break;
    case SVGFileDir:
      if (!strcmp (data, ".."))
	{
	  /* suppress last directory */
	  tempfile = (char *) TtaGetMemory (MAX_LENGTH);
	  strcpy (tempname, baseDirectory);
	  TtaExtractName (tempname, baseDirectory, tempfile);
	  strcpy (LastURLSVG, baseDirectory);
	  TtaFreeMemory (tempfile);
	}
      else
	{
	  strcat (baseDirectory, DIR_STR);
	  strcat (baseDirectory, data);
	  strcpy (LastURLSVG, baseDirectory);
	}
#ifndef _WINDOWS
/*      TtaSetTextForm (BaseLibrary + NewSVGFileURL, baseDirectory);*/
      TtaSetTextForm (BaseLibrary + SVGFileBrowserText, baseDirectory);
#endif /* !_WINDOWS */
      TtaListDirectory (baseDirectory, BaseLibrary + AddSVGModel,
			TtaGetMessage (LIB, TMSG_DOC_DIR),
			BaseLibrary + SVGFileDir, svgFilter,
			TtaGetMessage (AMAYA, AM_FILES),
			BaseLibrary + SVGFileSel);
      break;
    case SVGFileSel:
      if (baseDirectory[0] == EOS)
	/* set path on current directory */
	getcwd (baseDirectory, MAX_LENGTH);
      /* construct the library full name */
      strcpy (LastURLSVG, baseDirectory);
      val = strlen (LastURLSVG) - 1;
      if (LastURLSVG[val] != DIR_SEP)
	strcat (LastURLSVG, DIR_STR);
      strcat (LastURLSVG, data);
#ifndef _WINDOWS
      TtaSetTextForm (BaseLibrary + SVGFileBrowserText, LastURLSVG);
#endif*/ /* !_WINDOWS */
      break;
    case SVGFilter: /* Filter value */
      if (strlen(data) <= NAME_LENGTH)
	strcpy (svgFilter, data);
#ifndef _WINDOWS
      else
	TtaSetTextForm (BaseLibrary + SVGFilter, svgFilter);
#endif /* !_WINDOWS */
      break;

    case SVGLibraryDir:
      if (!strcmp (data, ".."))
	{
	  /* suppress last directory */
	  tempfile = (char *) TtaGetMemory (MAX_LENGTH);
	  strcpy (tempname, baseDirectory1);
	  TtaExtractName (tempname, baseDirectory1, tempfile);
	  strcpy (LastURLCatalogue, baseDirectory1);
	  TtaFreeMemory (tempfile);
	}
      else
	{
	  strcat (baseDirectory1, DIR_STR);
	  strcat (baseDirectory1, data);
	  strcpy (LastURLCatalogue, baseDirectory1);
	}
#ifndef _WINDOWS
      TtaSetTextForm (BaseLibrary + SVGLibFileBrowserText/*SVGLibraryURL*/, baseDirectory1);
#endif /* !_WINDOWS */
      TtaListDirectory (baseDirectory1, BaseLibrary + NewSVGLibrary,
			TtaGetMessage (LIB, TMSG_DOC_DIR),
			BaseLibrary + SVGLibraryDir, libraryFilter,
			TtaGetMessage (AMAYA, AM_FILES),
			BaseLibrary + SVGLibrarySel);
      break;
    case SVGLibrarySel:
      if (baseDirectory1[0] == EOS)
	/* set path on current directory */
	getcwd (baseDirectory1, MAX_LENGTH);
      /* construct the library full name */
      strcpy (LastURLCatalogue, baseDirectory1);
      val = strlen (LastURLCatalogue) - 1;
      if (LastURLCatalogue[val] != DIR_SEP)
	strcat (LastURLCatalogue, DIR_STR);
      strcat (LastURLCatalogue, data);
#ifndef _WINDOWS
      TtaSetTextForm (BaseLibrary + SVGLibFileBrowserText/*SVGLibraryURL*/, LastURLCatalogue);
#endif /* !_WINDOWS */
      break;
    case LibraryFilter: /* Filter value */
      if (strlen(data) <= NAME_LENGTH)
	strcpy (libraryFilter, data);
#ifndef _WINDOWS
      else
	TtaSetTextForm (BaseLibrary + LibraryFilter, libraryFilter);
#endif /* !_WINDOWS */
      break;

    case SVGLibFileBrowserText: /* store the NewSVGFileURL */
      if (strlen(data) <= MAX_LENGTH)
	strcpy (LastURLCatalogue, data);
#ifndef _WINDOWS
      else
	TtaSetTextForm (BaseLibrary + SVGLibFileBrowserText, LastURLCatalogue);
#endif /* !_WINDOWS */
      break;
   case SVGFileBrowserText: /* store the NewSVGFileURL */
      if (strlen(data) <= MAX_LENGTH)
	strcpy (LastURLSVG, data);
#ifndef _WINDOWS
      else
	TtaSetTextForm (BaseLibrary + SVGFileBrowserText, LastURLSVG);
#endif /* !_WINDOWS */
      break;

    case SVGFileBrowser:
      switch (val)
	{
	case 2: /* Confirm button set NewSVGFileURL Text Zone */
	  TtaSetTextForm (BaseLibrary + NewSVGFileURL, LastURLSVG);
	  TtaDestroyDialogue (ref);
	  break;
	case 3: /* Filter button,  reinitialize directories and document lists */
	  TtaListDirectory (baseDirectory, BaseLibrary + SVGFileBrowser,
			    TtaGetMessage (LIB, TMSG_DOC_DIR),
			    BaseLibrary + SVGFileDir, svgFilter,
			    TtaGetMessage (AMAYA, AM_FILES),
			    BaseLibrary + SVGFileSel);
	  break;
	default:
	  break;
	}
      break;

    case SVGLibFileBrowser:
      switch (val)
	{
	case 2: /* Confirm button set NewSVGLibFileURL Text Zone */
	  TtaSetTextForm (BaseLibrary + SVGLibraryURL, LastURLCatalogue);
	  /* Is this library file exist?*/

	  if (IsLibraryName (LastURLCatalogue))
	    {
	      if (TtaFileExist (LastURLCatalogue))
		{ /* take the library title */
		  libraryTitle = GetLibraryFileTitle (LastURLCatalogue);
		  strcpy (NewLibraryTitle, libraryTitle);
		  if (libraryTitle)
		    TtaFreeMemory (libraryTitle);
		  TtaSetTextForm (BaseLibrary + NewSVGLibraryTitle, NewLibraryTitle);
		}
	    }
	  TtaDestroyDialogue (ref);
	  break;
	case 3: /* Filter button,  reinitialize directories and document lists */
	  TtaListDirectory (baseDirectory1, BaseLibrary + SVGLibFileBrowser,
			    TtaGetMessage (LIB, TMSG_DOC_DIR),
			    BaseLibrary + SVGLibraryDir, libraryFilter,
			    TtaGetMessage (AMAYA, AM_FILES),
			    BaseLibrary + SVGLibrarySel);
	  break;
	default:
	  break;
	}
      break;

    default:
      break;
      
    }
  if (LibraryDocElement)
    {
      /* there is a Background Color Presentation attach to this element */
      /*      PRBackground 43 */
      PRuleSearch = TtaGetPRule (LibraryDocElement, PRBackground);
      TtaRemovePRule (LibraryDocElement, PRuleSearch, LibraryDocDocument);
      LibraryDocElement = NULL;
    }
#endif /* _SVGLIB */
}

/*----------------------------------------------------------------------
  CreateNewCatalogueDialogue
  Initialize create new catalogue form
  ----------------------------------------------------------------------*/
void CreateNewCatalogueDialogue (Document doc, View view)
{
#ifdef _SVGLIB
#ifndef _WINDOWS
  char           bufButton[MAX_LENGTH];
  char          *filename;
  int            i;
#endif /* _WINDOWS */

  /* fonction de test de la selection courante */
  i = 0;
  strcpy (&bufButton[i], TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
  i += strlen (&bufButton[i]) + 1;
  strcpy (&bufButton[i], TtaGetMessage (AMAYA, AM_CLEAR));
  i += strlen (&bufButton[i]) + 1;
  strcpy (&bufButton[i], TtaGetMessage (AMAYA, AM_SVGLIB_ADD_NEW_CATALOGUE));

  filename =  (char *) TtaGetMemory (MAX_LENGTH);
  TtaExtractName (DocumentURLs[doc], baseDirectory1, filename);
  strcpy (libraryFilter, "*.lhtml*");

  TtaNewSheet (BaseLibrary + NewSVGLibrary, TtaGetViewFrame (doc, view),
	       TtaGetMessage (AMAYA, AM_SVGLIB_CREATE_NEW_CATALOGUE_DIALOG), 3,
	       bufButton, TRUE, 2, 'L', D_CANCEL);

  /* Library URL text zone*/
  TtaNewTextForm (BaseLibrary + SVGLibraryURL, BaseLibrary + NewSVGLibrary,
		  TtaGetMessage (AMAYA, AM_SVGLIB_CATALOGUE_URL),
		  50, 1, FALSE);
  TtaSetTextForm (BaseLibrary + SVGLibraryURL, DocumentURLs[doc]);
  TtaNewLabel (BaseLibrary + SVGLibraryLabel, BaseLibrary + NewSVGLibrary, " ");

 /* Library Title text zone*/
  TtaNewTextForm (BaseLibrary + NewSVGLibraryTitle, BaseLibrary + NewSVGLibrary,
		  TtaGetMessage (AMAYA, AM_SVGLIB_CATALOGUE_TITLE),
		  50, 1, FALSE);
  TtaNewLabel (BaseLibrary + SVGLibraryLabel1, BaseLibrary + NewSVGLibrary, " ");
  TtaNewLabel (BaseLibrary + SVGLibraryLabel2, BaseLibrary + NewSVGLibrary, " ");

  /* activates the Create Or Search Library Dialogue */
  TtaSetDialoguePosition ();
  TtaShowDialogue (BaseLibrary + NewSVGLibrary, TRUE);
  TtaWaitShowDialogue ();
  TtaFreeMemory (filename);
/*
  filename =  (char *) TtaGetMemory (MAX_LENGTH);
  TtaExtractName (DocumentURLs[doc], baseDirectory, filename);
*/

#endif /* _SVGLIB */
}

/*----------------------------------------------------------------------
  ShowLibraryBrowser
  ----------------------------------------------------------------------*/
void ShowLibraryBrowser (int FileSuffix)
{
#ifdef _SVGLIB
  Document       doc;
#ifndef _WINDOWS
  char           bufButton[MAX_LENGTH];
  char          *filename;
  int            i;
#endif /* _WINDOWS */

  i = 0;
  strcpy (&bufButton[i], TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
  i += strlen (&bufButton[i]) + 1;
  strcpy (&bufButton[i], TtaGetMessage (AMAYA, AM_PARSE));
  doc = TtaGetSelectedDocument ();

  if (FileSuffix == 1) /* it's a svg file browser */
    {

      filename =  (char *) TtaGetMemory (MAX_LENGTH);
      TtaExtractName (DocumentURLs[doc], baseDirectory, filename);
      strcpy (svgFilter, "*.svg*");

      TtaNewSheet (BaseLibrary + SVGFileBrowser, TtaGetViewFrame (doc, 1),
		   TtaGetMessage (AMAYA, AM_FILE_BROWSER), 2,
		   bufButton, TRUE, 2, 'L', D_CANCEL);

      TtaNewTextForm (BaseLibrary + SVGFileBrowserText, BaseLibrary + SVGFileBrowser,
		      TtaGetMessage (AMAYA, AM_SVGLIB_CREATE_NEW_SVG_FILE),
		      50, 1, FALSE);
      TtaSetTextForm (BaseLibrary + SVGFileBrowserText, baseDirectory);
      TtaNewLabel (BaseLibrary + SVGLibraryLabel1, BaseLibrary + SVGFileBrowser, " ");

      TtaListDirectory (baseDirectory, BaseLibrary + SVGFileBrowser,
			TtaGetMessage (LIB, TMSG_DOC_DIR),
			BaseLibrary + SVGFileDir, svgFilter,
			TtaGetMessage (AMAYA, AM_FILES),
			BaseLibrary + SVGFileSel);

      /* Svg filter text zone */
      TtaNewTextForm (BaseLibrary + SVGFilter, BaseLibrary + SVGFileBrowser,
		      TtaGetMessage (AMAYA, AM_PARSE), 10, 1, TRUE);
      TtaSetTextForm (BaseLibrary + SVGFilter, svgFilter);

      /* Set Browser dialogue actif */
      TtaSetDialoguePosition ();
      TtaShowDialogue (BaseLibrary + SVGFileBrowser, TRUE);
      TtaWaitShowDialogue ();

      TtaFreeMemory (filename);
    }

  if (FileSuffix == 2) /* it's a library file browser */
    {

      filename =  (char *) TtaGetMemory (MAX_LENGTH);
      TtaExtractName (DocumentURLs[doc], baseDirectory1, filename);
      strcpy (libraryFilter, "*.lhtml*");

      TtaNewSheet (BaseLibrary + SVGLibFileBrowser, TtaGetViewFrame (doc, 1),
		   TtaGetMessage (AMAYA, AM_FILE_BROWSER), 2,
		   bufButton, TRUE, 2, 'L', D_CANCEL);

      TtaNewTextForm (BaseLibrary + SVGLibFileBrowserText, BaseLibrary + SVGLibFileBrowser,
		      TtaGetMessage (AMAYA, AM_SVGLIB_CATALOGUE_URL),
		      50, 1, FALSE);
      TtaSetTextForm (BaseLibrary + SVGLibFileBrowserText, baseDirectory1);
      TtaNewLabel (BaseLibrary + SVGLibraryLabel3, BaseLibrary + SVGLibFileBrowser, " ");

      TtaListDirectory (baseDirectory1, BaseLibrary + SVGLibFileBrowser,
			TtaGetMessage (LIB, TMSG_DOC_DIR),
			BaseLibrary + SVGLibraryDir, libraryFilter,
			TtaGetMessage (AMAYA, AM_FILES),
			BaseLibrary + SVGLibrarySel);

      /* Library filter text zone */
      TtaNewTextForm (BaseLibrary + LibraryFilter, BaseLibrary + SVGLibFileBrowser,
		      TtaGetMessage (AMAYA, AM_PARSE), 10, 1, TRUE);
      TtaSetTextForm (BaseLibrary + LibraryFilter, libraryFilter);

      /* Set Browser dialogue actif */
      TtaSetDialoguePosition ();
      TtaShowDialogue (BaseLibrary + SVGLibFileBrowser, TRUE);
      TtaWaitShowDialogue ();

      TtaFreeMemory (filename);
    }


#endif /* _SVGLIB */
}


/*----------------------------------------------------------------------
  SVGLibraryListItemNumber
  This function return the number of elements of a List
  and modify a string by changing EOL in EOS
  ----------------------------------------------------------------------*/
int SVGLibraryListItemNumber (char *buffer)
{
  int          cpt = 0;
#ifdef _SVGLIB
  char        *ptrStr, *ptrStr1;
  int          fin = 0;

  ptrStr = buffer;
  if (buffer)
    {
      while (fin == 0)
	{
	  ptrStr1 = ptrStr;
	  while ((*ptrStr1 != EOS && *ptrStr1 != EOL))
	    {
	      ptrStr1++;
	    }
	  if (*ptrStr1 == EOL)
	    {
	      *ptrStr1 = EOS;
	      cpt++;
	      ptrStr = ++ptrStr1;
	    }
	  else
	    {
	      cpt++;
	      fin = 1;
	    }
	}
    }
#endif /* _SVGLIB */
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
#ifdef _SVGLIB
  ListUriTitle      *curList = HeaderListUriTitle;

  while (curList &&
	 (strcmp (curList->URI, (char *) data) != 0) &&
	 (strcmp (curList->Title, (char *) data) != 0))
    {
      curList = curList->next;
    }
  if (curList != NULL)
    catalogueUri = curList->URI;
  else
    catalogueUri = NULL;
#endif /* _SVGLIB */
      return catalogueUri;
}

/*----------------------------------------------------------------------
  AddGraphicalObjectIntoCatalogue                    
  
  input: svg selected event                        
  output: the graphical object into the catalogue  
  ----------------------------------------------------------------------*/
void AddGraphicalObjectIntoCatalogue (Document doc, View view)
{
#ifdef _SVGLIB
  CopySvgInformationTree (doc, view);
#endif /* _SVGLIB */
}

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
#ifdef _SVGLIB
  Element              docEl, root, title, text, el, head, child, meta, body;
  Element              doctype, style;
  ElementType          elType;
  Attribute            attr;
  AttributeType        attrType;
  char                *s, *documentname, *pathname, *textStr;
  char                 tempfile[MAX_LENGTH];
  char                 charsetName[MAX_LENGTH];
  Language             language;

  /* create the document */
  textStr = (char * ) TtaGetMemory (MAX_LENGTH);
  pathname = (char * ) TtaGetMemory (MAX_LENGTH);
  documentname = (char * ) TtaGetMemory (MAX_LENGTH);
  NormalizeURL (libUrl, 0, pathname, documentname, NULL);

  newLibraryDoc = InitDocAndView (newLibraryDoc, documentname, 0, 0, FALSE, L_Xhtml11);
  InNewWindow = FALSE;

  TtaFreeMemory (documentname);
  TtaFreeMemory (pathname);

  /* save the document name into the document table */
  s = TtaStrdup (libUrl);
  TtaSetTextZone (newLibraryDoc, 1, 1, libUrl);
  DocumentURLs[newLibraryDoc] = s;
  DocumentMeta[newLibraryDoc] = DocumentMetaDataAlloc ();
  DocumentMeta[newLibraryDoc]->form_data = NULL;
  DocumentMeta[newLibraryDoc]->initial_url = NULL;
  DocumentMeta[newLibraryDoc]->method = CE_ABSOLUTE;
  DocumentMeta[newLibraryDoc]->xmlformat = FALSE;
  DocumentSource[newLibraryDoc] = 0;

/*
 *
 * Initialiser les metas: DocumentMeta[newLibraryDoc]
 *
 */




  /* store the document profile */
  TtaSetDocumentProfile (newLibraryDoc, L_Xhtml11);

  ResetStop (newLibraryDoc);
  language = TtaGetDefaultLanguage ();
  docEl = TtaGetMainRoot (newLibraryDoc);
  /* disable auto save */
  TtaSetDocumentBackUpInterval (newLibraryDoc, 0);
  /* Set the document charset */
  TtaSetDocumentCharset (newLibraryDoc, ISO_8859_1);
  strcpy (charsetName , "iso-8859-1");
  DocumentMeta[newLibraryDoc]->charset = TtaStrdup (charsetName);

  /*-------------  New XHTML document ------------*/
  /* create the DOCTYPE element corresponding to the document's profile */
  /* force the XML parsing */
  DocumentMeta[newLibraryDoc]->xmlformat = TRUE;

  elType = TtaGetElementType (docEl);
  attrType.AttrSSchema = elType.ElSSchema;

  elType.ElTypeNum = HTML_EL_DOCTYPE;
  doctype = TtaSearchTypedElement (elType, SearchInTree, docEl);
  if (doctype != NULL)
    TtaDeleteTree (doctype, newLibraryDoc);
  CreateDoctype (newLibraryDoc, L_Xhtml11, FALSE, FALSE);
  
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
    TtaSetTextContent (text, libtitle, language, newLibraryDoc);
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
  strcpy (textStr, " image { width : 50 ; height : 50 } ");
  sprintf (textStr, "%s%c", textStr, EOL);
  strcat (textStr, " g_title { color: #0000B2; font-family: helvetica; font-weight: bold; vertical-align: middle; }");
  sprintf (textStr, "%s%c", textStr, EOL);
  strcat (textStr, " g_comment {font-size: 12pt; font-weight: normal; color: #B2005A; vertical-align: middle; }");
  TtaSetTextContent (text, textStr, language, newLibraryDoc);

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
    TtaSetTextContent (text, libUrl, language, newLibraryDoc);
  
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
  strcpy (tempfile, HTAppName);
  strcat (tempfile, " ");
  strcat (tempfile, HTAppVersion);
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
  if (SelectionDoc != 0)
    UpdateContextSensitiveMenus (SelectionDoc);
  SelectionDoc = newLibraryDoc;
  UpdateContextSensitiveMenus (newLibraryDoc);
  /* Activate the section numbering */
  if (SNumbering[newLibraryDoc])
    ChangeAttrOnRoot (newLibraryDoc, HTML_ATTR_SectionNumbering);
  /* Activate show areas */
  if (MapAreas[newLibraryDoc])
    ChangeAttrOnRoot (newLibraryDoc, HTML_ATTR_ShowAreas);
  DocumentTypes[newLibraryDoc] = docLibrary;
#endif /* _SVGLIB */
  return newLibraryDoc;
}

/*----------------------------------------------------------------------
  GetLibraryFileTitle
  Allocates and returns the title associated to a Library File document
  input parameters:
  URL of the library file
  ----------------------------------------------------------------------*/
char *GetLibraryFileTitle (char *url)
{
  char               *reTitle = NULL;
#ifdef _SVGLIB
  Document             res, libraryDoc;
  ElementType         elTypeSearch;
  Element             el, child;
  Language            lang;
#ifdef _I18N_
  unsigned char       *title;
#endif /* _I18N_ */
  int                 length;
  
  if (url)
    {
      if (TtaFileExist (url))
	{
	  /* Open library file in a document (Cf. MakeBook) */
	  libraryDoc = TtaNewDocument("HTML", "tmp");
	  InNewWindow = FALSE;
	  res = GetAmayaDoc (url, NULL,
			     libraryDoc, 0, CE_MAKEBOOK,
			     FALSE, NULL, NULL,
			     ISO_8859_1/* TtaGetDefaultCharset() */);
	  
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
		  TtaGiveTextContent (child, reTitle, &length, &lang);
#ifdef _I18N_
		  title = TtaConvertMbsToByte (reTitle, TtaGetDefaultCharset ());
		  strcpy (reTitle, title);
		  TtaFreeMemory (title);
#endif /* _I18N_ */
		}
	      else
		/* Title not found */
		reTitle = NULL;
	    }
	  /* Free resource */
	  FreeDocumentResource (libraryDoc);
	  TtaCloseDocument (libraryDoc);
	}
      else
	reTitle = NULL;
    }
#endif /* _SVGLIB */
  return reTitle;
}

/*----------------------------------------------------------------------
  AddLibraryDataIntoStructure
  Add a new structure url title into a dynamic liste :
  the Library Manager Structure List
  parameters: url and title
  persLib : if persLib is FALSE the Library identified by URL is delivered 
  with Amaya distribution, else if it's a custom Library.
  ----------------------------------------------------------------------*/
void AddLibraryDataIntoStructure (ThotBool persLib, char *url, char *title)
{
/* Utiliser un tableau plutôt qu'une liste chainée */
/* ensuite modifier test sur les lignes du tableau avec un indice (Cf. FrameTable)*/
#ifdef _SVGLIB/*!!!!!!modifier le nom des variables */
  ListUriTitle     *listCur, *listNext, *listNew; /* *curCel, *nextCel, *newCel*/
  char             *path, *Title;
  int               index = 1;

  path = (char *) TtaGetMemory (strlen (url) + 1);
  strcpy (path, url);
  Title = (char *) TtaGetMemory (strlen (title) + 2);
  strcpy (Title, title);
  if (HeaderListUriTitle)
    {
      /* list exist */
      listNext = HeaderListUriTitle;
      while (listNext)
	{
	  listCur = listNext;
	  if (!strcmp (listCur->Title, title))
	    {
	      /* create a unique title */
	      /* a effectuer */
	    }
	  listNext = listNext->next;
	  index++;
	}
      listNew = (ListUriTitle *) TtaGetMemory (sizeof (ListUriTitle));
      listNew->next = NULL;
      listNew->URI = path;
      listNew->Title = Title;
      listNew->indice = index;
      if (persLib)
	listCur->customLibrary = TRUE;
      else
	listCur->customLibrary = FALSE;
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
#endif /* _SVGLIB */
}


/*----------------------------------------------------------------------
  SearchURIinCurrentSVGLibraryList
  return TRUE if the URI is yet in the list
  ----------------------------------------------------------------------*/
ThotBool SearchURIinCurrentSVGLibraryList (char *URI)
{
  ThotBool found = FALSE;
#ifdef _SVGLIB
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
#endif /* _SVGLIB */
  return found;
}


/*-------------------------------------------------------------------
  SVGLIB_FreeDocumentResource
  Frees all the library resources that are associated with
  Library document when closing Amaya.
  -------------------------------------------------------------------*/
void SVGLIB_FreeDocumentResource ()
{
#ifdef _SVGLIB
  ListUriTitle      *curList, *prevList;

  /* Problem: when appli use this function there is no EnvString */
  if (LibraryStructureModified)
    {
      /* save inter session file */
      WriteInterSessionLibraryFileManager ();
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
#endif /* _SVGLIB */
}


/*-------------------------------------------------------------------
  GetLibraryTitleFromPath
  Search path in the Library Manager Structure List 
  allocate and return the corresponding title
  if it exists or NULL else if
  -------------------------------------------------------------------*/
char *GetLibraryTitleFromPath (char *path)
{
  char             *title = NULL;
#ifdef _SVGLIB
  ListUriTitle     *listCur;

  if (HeaderListUriTitle)
    {
      /* list exist */
      listCur = HeaderListUriTitle;
      while (listCur && (strcmp (listCur->URI, path) != 0))
	{
	  listCur = listCur->next;
	}
      if (listCur)
	{
	  title = (char *) TtaGetMemory (strlen (listCur->Title) + 1);
	  strcpy (title, listCur->Title);
	}
    }
#endif /* _SVGLIB */
  return title;
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
#ifdef _SVGLIB
  ListUriTitle     *listCur;

  if (HeaderListUriTitle)
    {
      /* list exist */
      listCur = HeaderListUriTitle;
      while (listCur && (strcmp (listCur->Title, title) != 0))
	{
	  listCur = listCur->next;
	}
      if (listCur)
	{
	  path = listCur->URI;
	}
    }
#endif /* _SVGLIB */
  return path;
}

/*-------------------------------------------------------------------
  InitSVGBufferForCombobox
  Initializes a string by reading data in a Library Manager Structure List
  This function allocates memory and return it.
  So you have to free them after calling this function.
  -------------------------------------------------------------------*/
char *InitSVGBufferForComboBox ()
{
#ifdef _SVGLIB
  ListUriTitle     *curList;
  char             *buffer;
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
      buffer = (char *) TtaGetMemory (lg + 10);
      curList = HeaderListUriTitle;
      buffer[0] = EOS;
      while (curList)
	{
	  strcat (buffer, curList->Title);
	  curList = curList->next;
	  if (curList)
	    sprintf (buffer,"%s%c", buffer, EOL);
	}
      return buffer;
    }
#endif /* _SVGLIB */
  return NULL;
}

/*------------------------------------------------------------------
  InitSVGLibraryListURL
  Initializes a string by reading data in a Library Manager Structure List
  This function allocates memory and return it.
  So you have to free them after calling this function.
  ------------------------------------------------------------------*/
char *InitSVGLibraryListURL()
{
#ifdef _SVGLIB
  ListUriTitle     *curList;
  char             *buffer;
  int               lg = 0;

  if (HeaderListUriTitle)
    {
      /* length of the string */
      curList = HeaderListUriTitle;
      while (curList)
	{
	  lg += strlen (curList->URI) + 1;
	  curList = curList->next;
	}
      buffer = (char *) TtaGetMemory (lg + 10);
      curList = HeaderListUriTitle;
      lg = 0;
      buffer[lg] = EOS;
      while (curList)
	{
	  strcpy (&buffer[lg], curList->URI);
	  lg += strlen (curList->URI) + 1;
	  curList = curList->next;
	}
      return buffer;
    }
#endif /* _SVGLIB */
  return NULL;
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
#ifdef _SVGLIB
  char     *url_home, *url_thot, *app_home, *thot_dir;
  char     *urlstring, *librarytitle;
  FILE     *libfile;
  int       cpt = 0;

  if (!HeaderListUriTitle)
    {
      url_home = (char *) TtaGetMemory (MAX_LENGTH);
      urlstring = (char *) TtaGetMemory (MAX_LENGTH);
      url_thot = (char *) TtaGetMemory (MAX_LENGTH);
/*      app_home = (char *) TtaGetMemory (MAX_LENGTH);*/

      /* Read lib_files.dat into APP_HOME directory */
      app_home = TtaGetEnvString ("APP_HOME");
      sprintf (url_home, "%s%clib_files.dat", app_home, DIR_SEP); 
      /* ./.amaya/lib_files.dat */
      libfile = TtaReadOpen (url_home);
      if (libfile)
	{
	  while (fscanf (libfile, "%s", urlstring) > 0)
	    {
	      if (cpt == 0)
		{
		  if (urlstring)
		    {
		      /* Get the document title by opening the document
			 and then update the Uri-Title structure */
		      librarytitle = GetLibraryFileTitle (urlstring);
		      if (librarytitle)
			{
			  /* ajout dans la structure et dans le buffer de memorisation */
			  AddLibraryDataIntoStructure (TRUE, urlstring, librarytitle);
			  TtaFreeMemory (librarytitle);
			}
		    }
		  cpt++;
		}
	      else
		{
		  if (urlstring)
		    {
		      /* Get the document title by opening the document
			 and then update the Uri-Title structure */ 
		      librarytitle = GetLibraryFileTitle (urlstring);
		      if (librarytitle)
			{
			  /* ajout dans la structure et dans le buffer de memorisation */
			  AddLibraryDataIntoStructure (TRUE, urlstring, librarytitle);
			  TtaFreeMemory (librarytitle);
			}
		    }
		}
	    }
	  TtaReadClose (libfile);
	  cpt = 0;
	}
      
      /* Read lib_files.dat into THOTDIR directory */
      thot_dir = TtaGetEnvString ("THOTDIR");
      sprintf (url_thot, "%s%cconfig%clib_files.dat", thot_dir, DIR_SEP,DIR_SEP);
      libfile = TtaReadOpen (url_thot);
      strcpy (url_thot, thot_dir);
      if (libfile)
	{
	  while (fscanf (libfile, "%s", urlstring) > 0)
	    {
	      if (cpt == 0)
		{
		  if (urlstring)
		    {
		      strcat (url_thot, urlstring);
		      strcpy (urlstring, url_thot);
		      /* Get the document title by opening the document
			 and then update the Uri-Title structure */
		      librarytitle = GetLibraryFileTitle (urlstring);
		      if (librarytitle)
			{
			  /* Add into Library Manager Structure List */
			  AddLibraryDataIntoStructure (FALSE, urlstring, librarytitle);
			  TtaFreeMemory (librarytitle);
			}
		    }
		  cpt++;
		}
	      else
		{
		  if (urlstring)
		    {
		      /* Get the document title by opening the document
			 and then update the Uri-Title structure */
		      strcpy (url_thot, thot_dir);
		      strcat (url_thot, urlstring);
		      strcpy (urlstring, url_thot);
		      librarytitle = GetLibraryFileTitle (urlstring);
		      if (librarytitle)
			{
			  /* Add into Library Manager Structure List */
			  AddLibraryDataIntoStructure (FALSE ,urlstring, librarytitle);
			  TtaFreeMemory (librarytitle);
			}
		    }
		}
	    }
	  TtaReadClose (libfile);
	}
      
      TtaFreeMemory (url_thot);
      TtaFreeMemory (url_home);
      TtaFreeMemory (urlstring);
    }
#endif /* _SVGLIB */
}


/*----------------------------------------------------------------------
  IsLibraryName                                                         
  returns TRUE if path points to an SVG resource.
  ----------------------------------------------------------------------*/
ThotBool IsLibraryName (const char *path)
{
#ifdef _SVGLIB
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
#endif /* _SVGLIB */
   return FALSE;
}

/*----------------------------------------------------------------------
  Load the first catalogue of the Library Manager Structure List
  ----------------------------------------------------------------------*/
void OpenCatalogue (Document doc, View view)
{
#ifdef _SVGLIB
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
	      /* Open the first catalogue of the Library File */
	      sprintf (LastURLName, "%s",
		       buffer);
	      /* load the HOME document */
	      InNewWindow = TRUE;
	      CurrentDocument = doc;
	      CallbackDialogue (BaseDialog + OpenForm, INTEGER_DATA, (char *) 1);
	    }
	  TtaFreeMemory (buffer);
	}
    }
  else /* Open the default catalogue in THOTDIR/config/libconfig/default_cat.lhtml */
    {
      /* Open the Amaya default library file */
      lib_path =  (char *) TtaGetMemory (MAX_LENGTH);
      app_home = TtaGetEnvString ("THOTDIR");
      sprintf (lib_path, "%s%cconfig%clibconfig%cdefault_cat.lhtml",
	       app_home, DIR_SEP, DIR_SEP, DIR_SEP);

      sprintf (LastURLName, "%s", lib_path);
      
      /* load the Catalogue document */
      InNewWindow = TRUE;
      CurrentDocument = doc;
      CallbackDialogue (BaseDialog + OpenForm, INTEGER_DATA, (char *) 1);
      TtaFreeMemory (lib_path);
    }
#endif /* _SVGLIB */
}

/*----------------------------------------------------------------------
  ShowLibrary
  open a catalogue file and customize the dialogue window widget
  ----------------------------------------------------------------------*/
void ShowLibrary (Document doc, View view)
{
#ifdef _SVGLIB
  /* Initialize Structure if it's not yet done */
  InitSVGLibraryManagerStructure();

  /* open catalogue */
  OpenCatalogue (doc, view);
  /* Création des widgets de la fenêtre pour obtenir l'ergonomie souhaitée */
  /* A effectuer si necessaire en modifiant les Widgets de la FrameTable */
  TtaHandlePendingEvents ();  
#endif /* _SVGLIB */
}

/*----------------------------------------------------------------------
  SaveSVGURL
  function that save SVG URL into a buffer
  input parameters:
  doc:the number of the selected document
  El:the selected element
  output: the SVG URL in local variable buffermem (if URL exist)
  ----------------------------------------------------------------------*/
void SaveSVGURL (Document doc, Element El)
{
#ifdef _SVGLIB
  ElementType       elTypeSearch;
  Element           currentEl, prevEl;
/*  Language          lang;*/
  Attribute         attrSearch;
  AttributeType     attrTypeSearch;
  int     length;
  char   *buffer;
  
  currentEl = El;
  /*
   *solution avec tant que non TtaGetTextLength alors TtaGetFirstChild 
   *(contournement du problème lié à la recherche sur le ptr de l'élément 
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
      length = TtaGetTextAttributeLength (attrSearch);
      buffer = (char *) TtaGetMemory (length);
      TtaGiveTextAttributeValue (attrSearch, buffer, &length);
      strcpy (buffermem, buffer);
      TtaFreeMemory (buffer);
    }
#endif /* _SVGLIB */
}


/*----------------------------------------------------------------------
  ChangeSVGLibraryLinePresentation
  function that change table line presentation of a library Document
  input parameters:
  doc:the number of the selected document
  El:the selected element
  ----------------------------------------------------------------------*/
void ChangeSVGLibraryLinePresentation (Document doc, Element El)
{
#ifdef _SVGLIB
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
  HTMLSetBackgroundColor (doc, prev, color);
  TtaFreeMemory (color);
/*  TtaHandlePendingEvents ();*/
#endif /* _SVGLIB */
}

/*----------------------------------------------------------------------
  InitLibrary
  Initialize Library
  ----------------------------------------------------------------------*/
void InitLibrary (void)
{
#ifdef _SVGLIB
#ifndef _WINDOWS
  iconLibsvg = TtaCreatePixmapLogo (libsvg_xpm);
  iconLibsvgNo = TtaCreatePixmapLogo (libsvgNo_xpm);
#endif /* _WINDOWS */

  BaseLibrary = TtaSetCallback (CallbackLibrary, LIBRARY_MAX_REF);
#endif /* _SVGLIB */
}

/*----------------------------------------------------------------------
  GetURIId
  ----------------------------------------------------------------------*/
char *GetURIId (char *href)
{
  char    *result = href;
#ifdef _SVGLIB
  char    *ptrStr1;
  int      length;

  if (result)
    {
      ptrStr1 = result;
      while (*ptrStr1 != '#' && *ptrStr1 != EOS)
	{
	  ptrStr1++;
	}
      if (*ptrStr1 == '#')
	{
	  ptrStr1++;
	  length = strlen (ptrStr1) + 1;
	  result = (char *) TtaGetMemory (strlen (ptrStr1) + 1);
	  strcpy (result, ptrStr1);
	}
      else
	result = ptrStr1;
    }
#endif /* _SVGLIB */
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
#ifdef _SVGLIB
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
#endif /* _SVGLIB */
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
#ifdef _SVGLIB
  Document       destDoc;
  SSchema        docSchema, SvgSchema;
  Element        selEl, child, SvgRootEl, parent, sibling;
  Element        LastInserted, firstSelEl, useEl, elFound;
  ElementType    elType, selType;
  Attribute      attrSvgRoot, attrSvgUse, attrFound;
  AttributeType  attrTypeSvgRoot, attrTypeSvgUse;
  int            firstChar, lastChar, len;
  char          *pathname, *buffer, *basename, *filename, *relativeURL;
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
  docModified = TtaIsDocumentModified (destDoc);
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
	  SvgSchema = TtaNewNature (destDoc, docSchema, "SVG",
				      "SVGP");
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
      if (!sibling)
	{
	  copiedElement = TtaCopyTree (sourceEl, sourceDoc, destDoc, parent);
	  if (copiedElement)
	    {
	      TtaInsertFirstChild (&copiedElement, parent, destDoc);
	    }
	}
      else
	{
	  copiedElement = TtaCopyTree (sourceEl, sourceDoc, destDoc, sibling);
	  if (copiedElement)
	    {
	      TtaInsertSibling (copiedElement, sibling, FALSE, destDoc);
	    }
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
      relativeURL = (char *) TtaGetMemory (MAX_LENGTH);
      filename = (char *) TtaGetMemory (MAX_LENGTH);
      NormalizeURL (buffermem, sourceDoc, pathname, filename, NULL);
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
      TtaFreeMemory (relativeURL);
    }
  TtaSetDisplayMode (destDoc, dispMode);
 
 /* adapt the size of the SVG root element if necessary */
  CheckSVGRoot (destDoc, copiedElement);
  SetGraphicDepths (destDoc, SvgRootEl);
  TtaCloseUndoSequence (destDoc);
  TtaSetDocumentModified (destDoc);
#endif /* _SVGLIB */
  return copiedElement;
}

/*----------------------------------------------------------------------
  CopyOrReference
  This function starts the drop processus
  ----------------------------------------------------------------------*/
void CopyOrReference (Document doc, View view)
{
#ifdef _SVGLIB
#ifndef _WINDOWS
  char           bufButton[MAX_LENGTH];
  int            i;
#endif /* _WINDOWS */

  /* Initialize Structure if it's not yet done */
  InitSVGLibraryManagerStructure();

#ifndef _WINDOWS  
  /* Create the dialogue form */
  i = 0;
  strcpy (&bufButton[i], TtaGetMessage (AMAYA, AM_SVGLIB_COPY_SELECTION));
  i += strlen (&bufButton[i]) + 1;
  strcpy (&bufButton[i], TtaGetMessage (AMAYA, AM_SVGLIB_REF_SELECTION));
  
  TtaNewSheet (BaseLibrary + FormLibrary, TtaGetViewFrame (doc, view), 
	       TtaGetMessage (AMAYA, AM_SVGLIB_DIALOG1), 2/*bouton supplementaire*/,
	       bufButton, FALSE, 3, 'L', D_CANCEL);
  /* activates the Library Dialogue 1 */
  TtaShowDialogue (BaseLibrary+FormLibrary, TRUE);
#else /* _WINDOWS */
  CreatePasteLibraryModelDlgWnd (TtaGetViewFrame (doc, view));
#endif /* _WINDOWS */
#endif /* _SVGLIB */
}


/*----------------------------------------------------------------------
  CreatePNGofSVGSelected
  This function creates a PNG by using screenshot on svg selected elements
  ----------------------------------------------------------------------*/
void CreatePNGofSVGFile (Document doc, char *pngurl)
{
#ifdef _SVGLIB
  unsigned char *screenshot;
  int width, height;

  width = height = 150;
  /* Get SVG geometry size to initialize width and height */

  screenshot = GetScreenshot (GetWindowNumber (doc, 1), 0, 0, width, height);

  SavePng (pngurl, 
	   screenshot,
	   (unsigned int) width,
	   (unsigned int) height);
#endif /* _SVGLIB */
}

/*----------------------------------------------------------------------
  CreateNewSVGFileofSVGSelected
  This function creates a SVG document by copying svg selected elements.
  Then opens it in a new window.
  Returns document number if operation succeed, 0 else if.
  ----------------------------------------------------------------------*/
Document CreateNewSVGFileofSVGSelected ()
{
  Document             newSVGDoc = 0;
#ifdef _SVGLIB
  Document             selDoc;
  Element              firstSelEl, lastSelEl, currentEl, siblingEl, copiedEl;
  Element              elFound, root, newEl, comment, leaf;
  ElementType          elType;
  Attribute            newAttr;
  AttributeType        attrType;
  char                 buffer[MAX_LENGTH];
  int                  firstChar, lastChar, i, oldStructureChecking;
  Language             lang;
  View                 SVGView;

  if (IsCurrentSelectionSVG()) /* we can create a new svg file */
    {
      newSVGDoc = TtaInitDocument ("SVG", "tmp", 0);
      if (newSVGDoc != 0)
	{
	  TtaSetPSchema (newSVGDoc, "SVGP");
	  oldStructureChecking = TtaGetStructureChecking (newSVGDoc);
	  TtaSetStructureChecking (0, newSVGDoc);
	  
	  root = TtaGetRootElement (newSVGDoc);
	  elType = TtaGetElementType (root);
	  elType.ElTypeNum = SVG_EL_XMLcomment;
	  comment = TtaNewTree (newSVGDoc, elType, "");
	  TtaSetStructureChecking (0, newSVGDoc);
	  TtaInsertSibling (comment, root, TRUE, newSVGDoc);
	  TtaSetStructureChecking (1, newSVGDoc);
	  strcpy (buffer, " Created by ");
	  strcat (buffer, HTAppName);
	  strcat (buffer, " ");
	  strcat (buffer, HTAppVersion);
	  strcat (buffer, ", see http://www.w3.org/Amaya/ ");
	  leaf = TtaGetFirstLeaf (comment);
	  lang = TtaGetLanguageIdFromScript('L');
	  TtaSetTextContent (leaf, buffer, lang, newSVGDoc);

	  /* search the svg root element */
	  elType.ElTypeNum = SVG_EL_Document;
/*	  elFound = TtaSearchTypedElement(elType, SearchInTree, TtaGetRootElement(newSVGDoc));*/
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
	      TtaSetStructureChecking (0, newSVGDoc);
	      TtaInsertFirstChild(&newEl, elFound, newSVGDoc);

	      /* insert the copy of an element in the same order than selection */
	      copiedEl = TtaCopyTree (firstSelEl, selDoc, newSVGDoc, newEl);
	      TtaInsertFirstChild (&copiedEl, newEl, newSVGDoc);
	      CheckSVGRoot (newSVGDoc, copiedEl);
	      currentEl = firstSelEl;
	      siblingEl = copiedEl;
	      TtaGiveNextSelectedElement (selDoc, &currentEl, &i, &i);
	      while (currentEl)
		{
		  copiedEl = TtaCopyTree (currentEl, selDoc, newSVGDoc, newEl);
		  TtaInsertSibling (copiedEl, siblingEl, FALSE, newSVGDoc);
		  CheckSVGRoot (newSVGDoc, copiedEl);
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
	      MakeUniqueName (newEl, newSVGDoc);
	    }
	  
	  TtaSetStructureChecking (oldStructureChecking, newSVGDoc);
	  /* get the width and height before opening the svg file */
	  SVGView = TtaOpenMainView (newSVGDoc, 100/*x*/, 100/*y*/, 400/*w*/, 400/*h*/);
	}
    }
#endif /* _SVGLIB */
  return newSVGDoc;
}

/*----------------------------------------------------------------------
  AddSVGModelIntoLibrary
  This function creates png files and svg files associated to the 
  current selection. Then it adds HTML element into libraryDocument.
  Parameters:
  libraryDoc : document to update if it's not a newLib file
  newLib : TRUE if it's a new library
  !!!!!ne pas utiliser le titre pour ajouter les informations dans un
  document mais préférer l'URI
  libraryTitle : the Title
  ----------------------------------------------------------------------*/
void AddSVGModelIntoLibrary (Document libraryDoc, ThotBool newLib, char *libraryTitle)
{
#ifdef _SVGLIB
  char      *newURL, *suffix, *libraryURL, *tmp;
  ThotBool   ok;
  Document   svgDoc, tmpDoc;

  libraryURL = NULL;
  suffix = (char *) TtaGetMemory (10);

  if (!newLib)
    {
      if (libraryDoc != 0)
	{
	  libraryURL = (char *) TtaGetMemory (strlen (DocumentURLs[libraryDoc]) + 1);
	  strcpy (libraryURL, DocumentURLs[libraryDoc]);
	}
      else
	{
	  /* initialize string buffer */
	  tmp = GetLibraryPathFromTitle (libraryTitle);
	  libraryURL = (char *) TtaGetMemory (strlen (tmp) + 1);
	  strcpy (libraryURL, tmp);
	  /* Open it */
	  tmpDoc = TtaNewDocument ("HTML", "temp_library"); 
	  libraryDoc = GetAmayaDoc (libraryURL, NULL, tmpDoc, tmpDoc, CE_ABSOLUTE,
				    FALSE, FALSE, FALSE, TtaGetDefaultCharset ());
	}
    }
  else
    {
      libraryURL = (char *) TtaGetMemory (strlen (LastURLCatalogue) + 1);
      strcpy (libraryURL, LastURLCatalogue);
    }
  /* Create SVG file corresponding to the current selection*/
  strcpy (suffix, ".svg");
  newURL = MakeUniqueSuffixFilename (libraryURL, suffix);
  svgDoc = CreateNewSVGFileofSVGSelected();
  if (svgDoc)
    {
      ok = TtaExportDocument (svgDoc, newURL, "SVGT");
    }
  
  /*  Create PNG file corresponding to the svg file */
  ExtractSVGPrefixFilename (newURL);
  strcat (newURL, ".png");
  CreatePNGofSVGFile (svgDoc, newURL);

  if (newLib)
    {
      libraryDoc = CreateNewLibraryFile (LastURLCatalogue, NewLibraryTitle);

    }
  addingModelIntoLibraryFile (libraryDoc, newURL);

  if (newURL)
    TtaFreeMemory (newURL);
  TtaFreeMemory (suffix);
  if (libraryURL)
    TtaFreeMemory (libraryURL);
#endif /* _SVGLIB */
}


/*----------------------------------------------------------------------
  addingModelIntoLibraryFile
  this function updates library file by adding HTML element of a new 
  SVG model.
  ----------------------------------------------------------------------*/
void addingModelIntoLibraryFile (Document libDoc, char *newURL)
{
#ifdef _SVGLIB
  Element         imgEl, rootEl, rowEl, cellEl, anchorEl, table, newcellEl;
  Element         insertedEl;
  ElementType     elType;
  Attribute       attr;
  AttributeType   attrType;
  char           *relativeURL, *basename, *class;
  int             oldStructureChecking;

  basename = (char *) TtaGetMemory (MAX_LENGTH);
  relativeURL = (char *) TtaGetMemory (MAX_LENGTH);
  class = (char *) TtaGetMemory (MAX_LENGTH);

  oldStructureChecking = TtaGetStructureChecking (libDoc);
  TtaSetStructureChecking (0, libDoc);

  /*Insert or paste new element at the beginning of the table */
  rootEl = TtaGetRootElement (libDoc);
  elType = TtaGetElementType (rootEl);

  elType.ElTypeNum = HTML_EL_Table;
  table = TtaSearchTypedElement (elType, SearchInTree, rootEl);
  if (!table)
    { /* it's a new library, creates it with 3 column cell */
      /* Now just put a table starter */
      elType.ElTypeNum = HTML_EL_Table;
      table = TtaNewTree (libDoc, elType, "");
      elType.ElTypeNum = HTML_EL_BODY;
      TtaInsertFirstChild (&table, TtaSearchTypedElement (elType, SearchInTree, rootEl), 
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
      insertedEl = TtaSearchTypedElement (elType, SearchInTree, rootEl);
      /* create a new table row element */
      elType.ElTypeNum = HTML_EL_Table_row;
      rowEl = TtaNewTree (libDoc, elType, "");
    }
      TtaInsertSibling (rowEl, insertedEl, TRUE, libDoc);
      TtaOpenUndoSequence (libDoc, rowEl, rowEl, 0, 0);

      elType.ElTypeNum = HTML_EL_Table;
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
  ExtractSVGPrefixFilename (newURL);
  strcat (newURL, ".svg#object");
  basename = GetBaseURL (libDoc);
  relativeURL = MakeRelativeURL (newURL, basename);
  TtaSetAttributeText (attr, relativeURL, anchorEl, libDoc);
  
  /* create an image element */
  elType.ElTypeNum = HTML_EL_PICTURE_UNIT;
  imgEl = TtaNewTree (libDoc, elType, "");
  TtaInsertFirstChild (&imgEl, anchorEl, libDoc);
  /* initialize image source "src" attribute (relative URL) */
  attrType.AttrTypeNum = HTML_ATTR_SRC;
  attr = TtaNewAttribute (attrType);
  if (attr != NULL)
    TtaAttachAttribute (imgEl, attr, libDoc);
  ExtractSVGPrefixFilename (newURL);
  strcat (newURL, ".png");
  relativeURL = MakeRelativeURL (newURL, basename);
  TtaSetAttributeText (attr, relativeURL, imgEl, libDoc);
  /* initialize image "class" attribute */
  attrType.AttrTypeNum = HTML_ATTR_Class;
  attr = TtaNewAttribute (attrType);
  if (attr != NULL)
    TtaAttachAttribute (imgEl, attr, libDoc);
  strcpy (class, "image");
  TtaSetAttributeText (attr, class, imgEl, libDoc);

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
      strcpy (class, "g_title");
      TtaSetAttributeText (attr, class, cellEl, libDoc);
    }

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
      strcpy (class, "g_comment");
      TtaSetAttributeText (attr, class, cellEl, libDoc);
    }

  /* set stop button */
  ActiveTransfer (libDoc);
  FetchImage (libDoc, imgEl, NULL, 0, NULL, NULL);
  ResetStop (libDoc);

  /* Free memory */  
  TtaFreeMemory (basename);
  TtaFreeMemory (relativeURL);
  
  TtaSetStructureChecking (oldStructureChecking, libDoc);
  TtaCloseUndoSequence (libDoc);
  TtaSetDocumentModified (libDoc);
#endif /* _SVGLIB */
}


/*----------------------------------------------------------------------
  CreateLibraryDirectory
  this function creates a Directory linked to the library path.
  A effectuer (pour l'instant on place les fichiers SVG et PNG
  dans le meme repertoire que le fichier *.lhtml
  ----------------------------------------------------------------------*/
ThotBool CreateLibraryDirectory (char *libraryurl)
{
  ThotBool    RepCreated = FALSE;
#ifdef _SVGLIB

  ExtractSVGPrefixFilename (libraryurl);
  RepCreated = CheckMakeDirectory (libraryurl, FALSE);
#endif /* _SVGLIB */
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
#ifdef _SVGLIB
  char     *dirname, *filename;
  int       i = 1;

  dirname = (char *) TtaGetMemory (MAX_LENGTH);
  filename = (char *) TtaGetMemory (MAX_LENGTH);
  pathname = (char *) TtaGetMemory (MAX_LENGTH);

  TtaExtractName (libraryURL, dirname, filename);
  strcpy (pathname, dirname);
  ExtractSVGPrefixFilename (filename);
  sprintf (pathname, "%s%c%s%s", pathname, DIR_SEP, filename, suffix);
  while (TtaFileExist (pathname))
    {
      strcpy (pathname, dirname);
      sprintf (pathname, "%s%c%s%d%s", pathname, DIR_SEP, filename, i, suffix);
      i++;
    }
  TtaFreeMemory (dirname);
  TtaFreeMemory (filename);
#endif /* _SVGLIB */
  return pathname;
}


/*----------------------------------------------------------------------
  ExtractSVGPrefixFilename
  This function extracts the prefix file name by removing the suffix.
  A renommer ExtractPrefixFilename
  ----------------------------------------------------------------------*/
void ExtractSVGPrefixFilename (char *filename)
{
#ifdef _SVGLIB
  char   *ptrStr, *oldptrStr;

  if (filename)
    {
      ptrStr = oldptrStr = filename;
      ptrStr = strrchr (oldptrStr, '.');
      if (ptrStr)
	{
	  *ptrStr = EOS;
	}
    }
#endif /* _SVGLIB */
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
#ifdef _SVGLIB
  ListUriTitle  *curList = HeaderListUriTitle;
  FILE          *libfile;
  char          *url_home, *urlstring, *app_home;

  url_home = (char *) TtaGetMemory (MAX_LENGTH);
  urlstring = (char *) TtaGetMemory (MAX_LENGTH);

  /* Read lib_files.dat into APP_HOME directory */
  app_home = TtaGetEnvString ("APP_HOME");
  sprintf (url_home, "%s%clib_files.dat", app_home, DIR_SEP); 
  /* ./.amaya/lib_files.dat */
  libfile = TtaWriteOpen (url_home);
  if (libfile)
    {
      succeed = TRUE;
      /* necessité de tester le nombre d'uri a placer dans lib_files.dat
	 pour éviter d'avoir un EOL à la fin du fichier
	 (Pas sous GTK)*/
      while (curList)
	{
	  if(curList->customLibrary)
	    {
	      /* it's a custom Library, add it to lib_files.dat */
	      strcpy (urlstring, curList->URI);
	      fprintf (libfile, "%s%c", urlstring, EOL);
	    }
	  curList = curList->next;
	}
    }
  TtaWriteClose (libfile);
  TtaFreeMemory (url_home);
  TtaFreeMemory (urlstring);
#endif /* _SVGLIB */
  return succeed;
}


/*----------------------------------------------------------------------
   AddLibraryButton    
  ----------------------------------------------------------------------*/
void AddLibraryButton (Document doc, View view)
{
#ifdef _SVGLIB
  LibSVGButton = TtaAddButton (doc, 1, (ThotIcon)iconLibsvg, ShowLibrary,
			      "ShowLibrary",
			      TtaGetMessage (AMAYA, AM_BUTTON_SVG_LIBRARY),
			      TBSTYLE_BUTTON, TRUE);
#endif /* _SVGLIB */ 
}


/*----------------------------------------------------------------------
  SwitchIconGraph
  ----------------------------------------------------------------------*/
void SwitchIconLibrary (Document doc, View view, ThotBool state)
{
#ifdef _SVGLIB
  if (state)
    TtaChangeButton (doc, view, LibSVGButton, (ThotIcon)iconLibsvg, state);
  else
    TtaChangeButton (doc, view, LibSVGButton, (ThotIcon)iconLibsvgNo, state);
#endif /* _SVGLIB */
}
