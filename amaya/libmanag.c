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

/* Structure de memorisation de l'état d'un document librairie */
typedef struct _Ctx_LibraryDoc
{
  int library_doc;
  Element *El;
} Ctx_LibraryDoc;

/* Autre facon de mémoriser le contexte du dernier document librairie*/
Document	LibraryDocDocument = 0;
static Element	LibraryDocElement = NULL;
Attribute	LibraryDocAttribute = NULL;

/* Inserted boolean */
static ThotBool docModified = FALSE;

/* Autre structure de liste chaine contenant uniquement un élément */
typedef struct _Library_URITitle
{
  char *URI;
  char *Title;
  struct _Library_URITitle *next;
} ListUriTitle;
/* List header */

static ListUriTitle *HeaderListUriTitle = NULL;

/* Svg Filter to browse file */
static char svgFilter[MAX_LENGTH];
static char libraryFilter[MAX_LENGTH];
static char baseDirectory[MAX_LENGTH];
static char baseDirectory1[MAX_LENGTH];
static char SaveCatalogueTitleSelection[MAX_LENGTH];
static char LastURLSVG[MAX_LENGTH];
static char LastURLCatalogue[MAX_LENGTH];
#endif /* _SVGLIB */

/*----------------------------------------------------------------------
  IsCurrentSelectionSVG
  Check if the current selection is only svg
  ----------------------------------------------------------------------*/
ThotBool IsCurrentSelectionSVG ()
{
 ThotBool     IsSVG;
#ifdef _SVGLIB
  IsSVG = TRUE;
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
/*  Document          tempDoc;*/
#ifndef _WINDOWS
  char           bufButton[MAX_LENGTH];
  char          *buffer_list;
  int            i, nbr;
#endif /* _WINDOWS */

  /* Initialize Structure if it's not yet done */
  InitSVGLibraryManagerStructure();
  /* fonction de test de la selection courante */
  if (IsCurrentSelectionSVG ())
    {
      i = 0;
      strcpy (&bufButton[i], TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
      i += strlen (&bufButton[i]) + 1;
      strcpy (&bufButton[i], TtaGetMessage (AMAYA, AM_BROWSE));
      i += strlen (&bufButton[i]) + 1;
      strcpy (&bufButton[i], TtaGetMessage (AMAYA, AM_CLEAR));
      i += strlen (&bufButton[i]) + 1;
/*      strcpy (&bufButton[i], TtaGetMessage (AMAYA, AM_PARSE));
      i += strlen (&bufButton[i]) + 1;*/
      strcpy (&bufButton[i], TtaGetMessage (AMAYA, AM_SVGLIB_CREATE_NEW_CATALOGUE));

      TtaNewSheet (BaseLibrary + AddSVGModel, TtaGetViewFrame (doc, view), 
		   TtaGetMessage (AMAYA, AM_SVGLIB_ADD_SVG_MODEL_DIALOG), 4,
		   bufButton, TRUE, 2, 'L', D_CANCEL);

      /* Catalogue Or URI text zone */
      buffer_list = InitSVGBufferForComboBox ();
      nbr = SVGLibraryListItemNumber (buffer_list);
      TtaNewSizedSelector (BaseLibrary + SVGLibCatalogueTitle, BaseLibrary + AddSVGModel,
			   TtaGetMessage (AMAYA, AM_SVGLIB_CATALOGUE_TITLE),
			   nbr, buffer_list, 50, 3,
			   FALSE, FALSE, TRUE);
      TtaFreeMemory (buffer_list);
      TtaNewLabel (BaseLibrary + SVGLibraryLabel, BaseLibrary + AddSVGModel, " ");

      /* SVG file text zone*/
      TtaNewTextForm (BaseLibrary + NewSVGFileURL, BaseLibrary + AddSVGModel,
		      TtaGetMessage (AMAYA, AM_SVGLIB_CREATE_NEW_SVG_FILE),
		      50, 1, FALSE);
      TtaSetTextForm (BaseLibrary + NewSVGFileURL, DocumentURLs[doc]);

      /* activates the Add Library Model Dialogue  */
      TtaSetDialoguePosition ();
      TtaShowDialogue (BaseLibrary + AddSVGModel, TRUE);
      TtaWaitShowDialogue ();
    }
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
  Document            svgDoc, res;
  char               *pathname, *documentname, *id, *buffer;
  char                tempname[MAX_LENGTH];
  char               *tempfile;
  Element             Elt, copiedElement;
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
      res = GetAmayaDoc (pathname, NULL,
			 svgDoc, 0, CE_MAKEBOOK,
			 FALSE, NULL, NULL,
			 ISO_8859_1);
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
	      /* preparation à l'utilisation de copy tree */
	      /*TtaCopySpecificElement (svgDoc, Elt);*/
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

/*============================================================*/
    case AddSVGModel: /* 4 */
      buffer = data;

      switch (val)
	{
	case 3: /* Browser button, show SVG File browser dialogue */
	  TypeFile = 1;
	  ShowLibraryBrowser (TypeFile);
	  break;
	case 5: /* show dialogue create new catalogue */
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
      strcpy (SaveCatalogueTitleSelection, data);
      buffer = data;
      /* Is this Catalogue exist? */
      /* Search this data into HeaderListUriTitle */
      if (data)
	{
	  buffer = IsSVGCatalogueExist (data);
	}
      break;
    case SVGFilter: /* Filter value */
      if (strlen(data) <= NAME_LENGTH)
	strcpy (svgFilter, data);
#ifndef _WINDOWS
      else
	TtaSetTextForm (BaseLibrary + SVGFilter, svgFilter);
#endif /* !_WINDOWS */
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

    case LibraryFilter: /* Filter value */
      if (strlen(data) <= NAME_LENGTH)
	strcpy (libraryFilter, data);
#ifndef _WINDOWS
      else
	TtaSetTextForm (BaseLibrary + LibraryFilter, libraryFilter);
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
#endif*/ /* !_WINDOWS */
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
	case 2: /* Confirm button set NewSVGFileURL Text Zone */
	  TtaSetTextForm (BaseLibrary + SVGLibraryURL, LastURLCatalogue);
	  TtaDestroyDialogue (ref);
	  break;
	case 3: /* Filter button,  reinitialize directories and document lists */
	  TtaListDirectory (baseDirectory, BaseLibrary + SVGLibFileBrowser,
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
  strcpy (&bufButton[i], TtaGetMessage (AMAYA, AM_SVGLIB_ADD_NEW_CATALOGUE));
  i += strlen (&bufButton[i]) + 1;
  strcpy (&bufButton[i], TtaGetMessage (AMAYA, AM_CLEAR));
  i += strlen (&bufButton[i]) + 1;
  strcpy (&bufButton[i], TtaGetMessage (AMAYA, AM_FILE_BROWSER));

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
  TtaNewLabel (BaseLibrary + SVGLibraryLabel2, BaseLibrary + NewSVGLibrary, " ");

 /* Library Title text zone*/
  TtaNewTextForm (BaseLibrary + NewSVGLibraryTitle, BaseLibrary + NewSVGLibrary,
		  TtaGetMessage (AMAYA, AM_SVGLIB_CATALOGUE_TITLE),
		  50, 1, FALSE);/* rendre cette zone texte reactive */
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
			BaseLibrary + SVGLibraryDir, svgFilter,
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
  and modify a string by changing \n in \0
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
	  while ((*ptrStr1 != '\0' && *ptrStr1 != '\n') /*&& stop_boucle == 0*/)
	    {
	      ptrStr1++;
	    }
	  if (*ptrStr1 == '\n')
	    {
	      *ptrStr1 = '\0';
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
  char              *catalogueUri;
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
  CreateLibraryFile
  Create a lib_file.dat into APP_HOME when the user
  want to add a new catalogue
  ----------------------------------------------------------------------*/
void CreateLibraryFile ()
{
#ifdef _SVGLIB
  char     *url_home, *url_thot, *app_home, *thot_dir;
  FILE     *libfile, *newlibfile;
  
  url_home = (char *) TtaGetMemory (MAX_LENGTH);
  app_home = TtaGetEnvString ("APP_HOME");
  strcpy (url_home, app_home);
  strcat (url_home, "/lib_file.dat"); 
  /* ./.amaya/lib_file.dat */
  libfile = TtaReadOpen (url_home);
  if (libfile)
    {
      TtaReadClose (libfile);
    }
  else
    {
      /* Create lib_file.dat into APP_HOME directory */
      thot_dir = TtaGetEnvString ("THOTDIR");
      url_thot = (char *) TtaGetMemory (MAX_LENGTH);
      strcpy (url_thot, thot_dir);
      strcat (url_thot, "/config/libconfig/default_cat.lhtml");
      libfile = TtaReadOpen (url_thot);
      if (libfile)
	{
	  newlibfile = TtaWriteOpen (url_home);
	  fprintf (newlibfile, "%s\n", url_thot);
	  TtaWriteClose (newlibfile);
	}
      TtaFreeMemory (url_thot);
    }
  TtaFreeMemory (url_home);
#endif /* _SVGLIB */
}


/*----------------------------------------------------------------------
   LinkSVGLIBTableAlloc
   Creates a LinkSVGLIBTable element and initializes it to its default
   values.
  ----------------------------------------------------------------------*/
/*Link_Table *LinkSVGLIBTableAlloc (void)
{
  Link_Table    *LT;

  LT = (Link_Table *) TtaGetMemory (sizeof (Link_Table));
  memset ((void *) LT, 0, sizeof (Link_Table));
  return (LT);
}
*/

/*----------------------------------------------------------------------
  GetLibraryFileTitle
  Allocate and return the title associated to a Library File document
  input parameters:
  URL
  ----------------------------------------------------------------------*/
char *GetLibraryFileTitle (char *url)
{
  char               *reTitle = NULL;
#ifdef _SVGLIB
  Document             res, libraryDoc;
  /*  char        *pathname, *documentname;*/
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
			     ISO_8859_1);
	  
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
  Add a new structure url title into a dynamic liste
  the Library Manager Structure List
  parameters: url and title
  ----------------------------------------------------------------------*/
void AddLibraryDataIntoStructure (char *url, char *title)
{
/* Utiliser un tableau plutôt qu'une liste chainée */
/* ensuite modifier test sur les lignes du tableau avec un indice (Cf. FrameTable)*/
#ifdef _SVGLIB/*!!!!!!modifier le nom des variables */
  ListUriTitle     *listCur, *listNext, *listNew; /* *curCel, *nextCel, *newCel*/
  char             *path;

  path = (char *) TtaGetMemory (strlen (url) + 1);
  strcpy (path, url);
  if (HeaderListUriTitle)
    {
      /* list exist */
      listNext = HeaderListUriTitle;
      while (listNext)
	{
	  listCur = listNext;
	  listNext = listNext->next;
	}
      listNew = (ListUriTitle *) TtaGetMemory (sizeof (ListUriTitle));
      listNew->next = NULL;
      listNew->URI = path;
      listNew->Title = title;
      listCur->next = listNew;
    }
  else
    {
      /* list didn't exist, create it */
      HeaderListUriTitle = (ListUriTitle *) TtaGetMemory (sizeof (ListUriTitle));
      HeaderListUriTitle->next = NULL;
      HeaderListUriTitle->URI = path;
      HeaderListUriTitle->Title = title;
    }
#endif /* _SVGLIB */
}

/*-------------------------------------------------------------------
  SVGLIB_FreeDocumentResource
  Frees all the library resources that are associated with
  Document doc
  -------------------------------------------------------------------*/
void SVGLIB_FreeDocumentResource ()
{
#ifdef _SVGLIB
  ListUriTitle      *curList, *prevList;
  
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
  and return the corresponding title
  if it exists or NULL else if
  -------------------------------------------------------------------*/
char *GetLibraryTitleFromPath (char *path)
{
  char             *title;
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
	  title = listCur->Title;
	}
      else
	title = NULL;
    }
  else
    title = NULL;
#endif /* _SVGLIB */
  return title;
}

/*-------------------------------------------------------------------
  GetLibraryPathFromTitle
  Search title in the list and return the corresponding path
  if it exists or NULL else if
  -------------------------------------------------------------------*/
char *GetLibraryPathFromTitle (char *title)
{
  char             *path;
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
	  path = listCur->Title;
	}
      else
	path = NULL;
    }
  else
    path = NULL;
#endif /* _SVGLIB */
  return path;
}

/*-------------------------------------------------------------------
  InitSVGBufferForCombobox
  On initialise une chaîne de caractère afin de la transmettre à
  une fonction d'initialisation d'une liste de choix.
  Pour cela, on parcourt tout d'abord le répertoire contenant un fichier catalogue
  par défaut ensuite on parcourt la liste des catalogues crées par l'utilisateur.
  Deplus lors de la premiére ouverture d'un fichier librairie,
  //on récupère d'abord le fichier lib_file.dat dans le rép APP_HOME
  //puis dans le rép THOTDIR
  on initialise également le tableau de correspondance entre titre et url
  Initialize a string by reading data in a library file (lib_file.dat)
  parameters:
  buffer: the initialize string
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
	    strcat (buffer, "\n");
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
      
      /* Read lib_files.dat into APP_HOME directory */
      app_home = TtaGetEnvString ("APP_HOME");
      strcpy (url_home, app_home);
      strcat (url_home, "/lib_files.dat"); 
      /* ./.amaya/lib_files.dat */
      libfile = TtaReadOpen (url_home);
      if (libfile)
	{
/*	  bool = fscanf (libfile, "%s", urlstring);*/
	  while (/*bool*/fscanf (libfile, "%s", urlstring) > 0)
	    {
	      if (cpt == 0)
		{
		  if (urlstring)
		    {
		      /* on récupère le titre du fichier pour l'initialisation de
			 la structure de correspondance URI-Title */
		      librarytitle = GetLibraryFileTitle (urlstring);
		      if (librarytitle)
			{
			  /* ajout dans la structure et dans le buffer de memorisation */
			  AddLibraryDataIntoStructure (urlstring, librarytitle);
			}
		    }
		  cpt++;
		}
	      else
		{
		  if (urlstring)
		    {
		      /* on récupère le titre du fichier pour l'initialisation de
			 la structure de correspondance URI-Title */
		      librarytitle = GetLibraryFileTitle (urlstring);
		      if (librarytitle)
			{
			  /* ajout dans la structure et dans le buffer de memorisation */
			  AddLibraryDataIntoStructure (urlstring, librarytitle);
			  /*		      TtaFreeMemory (librarytitle);*/
			}
		    }
		}
/*	      if ((bool = fscanf (libfile, "%s", urlstring)) != EOF)*/
	    }
	  TtaReadClose (libfile);
	  cpt = 0;
	}
      
      /* Read lib_files.dat into THOTDIR directory */
      thot_dir = TtaGetEnvString ("THOTDIR");
      strcpy (url_thot, thot_dir);
      strcat (url_thot, "/config/lib_files.dat");
      libfile = TtaReadOpen (url_thot);
      strcpy (url_thot, thot_dir);
      if (libfile)
	{
/*	  bool = fscanf (libfile, "%s", urlstring);*/
	  while (/*bool*/fscanf (libfile, "%s", urlstring) > 0)
	    {
	      if (cpt == 0)
		{
		  if (urlstring)
		    {
		      strcat (url_thot, urlstring);
		      strcpy (urlstring, url_thot);
		      /* on récupère le titre du fichier pour l'initialisation de
			 la structure de correspondance URI-Title */
		      librarytitle = GetLibraryFileTitle (urlstring);
		      if (librarytitle)
			{
			  /* ajout dans la structure et dans le buffer de memorisation */
			  AddLibraryDataIntoStructure (urlstring, librarytitle);
			}
		    }
		  cpt++;
		}
	      else
		{
		  if (urlstring)
		    {
		      /* on récupère le titre du fichier pour l'initialisation de
			 la structure de correspondance URI-Title */
		      strcpy (url_thot, thot_dir);
		      strcat (url_thot, urlstring);
		      strcpy (urlstring, url_thot);
		      librarytitle = GetLibraryFileTitle (urlstring);
		      if (librarytitle)
			{
			  /* ajout dans la structure et dans le buffer de memorisation */
			  AddLibraryDataIntoStructure (urlstring, librarytitle);
			}
		    }
		}
/*	      if ((bool = fscanf (libfile, "%s", urlstring)) != EOF)
		strcat(buffer, "\n");*/
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
      strcpy (lib_path, app_home);
      strcat (lib_path, "/config/libconfig/default_cat.lhtml");

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
      while (*ptrStr1 != '#' && *ptrStr1 != '\0')
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
#endif _SVGLIB
  return result;
}

/*----------------------------------------------------------------------
  SearchSVGElement
  Return the svg element which has an Id Attribute
  with identificateur value
  ----------------------------------------------------------------------*/
Element SearchSVGElement (Document doc, char *identificateur)
{
  Element        elFound;
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
  Element        copiedElement;
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
	      /* créer une fonction de recherche des attributs du document svg copié */
	      TtaSearchAttribute (attrTypeSvgRoot, SearchInTree, TtaGetParent (TtaGetRootElement (sourceDoc)),
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
	      TtaSearchAttribute (attrTypeSvgRoot, SearchInTree, TtaGetParent (TtaGetRootElement (sourceDoc)),
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
		  /* Placer une valeur par défaut pour placer l'objet svg */
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
		     selType.ElTypeNum != HTML_EL_Division );
	      
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
#endif /* _SVGLIB */
}
