/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Amaya saving functions.
 *
 * Authors: D. Veillard, I. Vatton
 *          R. Guetari (W3C/INRIA): Windows NT/95
 *
 */

/* DEBUG_AMAYA_SAVE Print out debug informations when saving */

#ifdef AMAYA_DEBUG
#define DBG(a) a
#else
#define DBG(a)
#endif

/* Included headerfiles */
#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"

#ifdef _WINDOWS
#include "resource.h"
static char         currentDocToSave[MAX_LENGTH];
static char         currentPathName[MAX_LENGTH];
extern HINSTANCE    hInstance;
#ifdef __STDC__
LRESULT CALLBACK GetSaveDlgProc (HWND, UINT, WPARAM, LPARAM);
#else  /* !__STDC__ */
LRESULT CALLBACK GetSaveDlgProc (HWND, UINT, WPARAM, LPARAM);
#endif /* __STDC__ */
#endif /* _WINDOWS */
static char         tempSavedObject[MAX_LENGTH];
static int          URL_attr_tab[] = {
   HTML_ATTR_HREF_,
   HTML_ATTR_codebase,
   HTML_ATTR_Script_URL,
   HTML_ATTR_SRC,
   HTML_ATTR_background_,
   HTML_ATTR_Style_
};

static int          SRC_attr_tab[] = {
   HTML_ATTR_SRC,
   HTML_ATTR_background_,
   HTML_ATTR_Style_
};


#include "AHTURLTools_f.h"
#include "EDITimage_f.h"
#include "HTMLbook_f.h"
#include "HTMLimage_f.h"
#include "HTMLsave_f.h"
#include "HTMLstyle_f.h"
#include "init_f.h"
#if !defined(AMAYA_JAVA) && !defined(AMAYA_ILU)
#include "query_f.h"
#endif


#ifdef _WINDOWS
/*-----------------------------------------------------------------------
 CreateGetSaveDlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void CreateGetSaveDlgWindow (HWND parent, char* path_name)
#else  /* !__STDC__ */
void CreateGetSaveDlgWindow (parent, path_name)
HWND  parent;
char* path_name;
#endif /* __STDC__ */
{  
  sprintf (currentPathName, path_name);
  DialogBox (hInstance, MAKEINTRESOURCE (GETSAVEDIALOG), parent, (DLGPROC) GetSaveDlgProc);
}

/*-----------------------------------------------------------------------
 SaveAsDlgProc
 ------------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK GetSaveDlgProc (HWND hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK GetSaveDlgProc (hwnDlg, msg, wParam, lParam)
HWND   hwndParent; 
UINT   msg; 
WPARAM wParam; 
LPARAM lParam;
#endif /* __STDC__ */
{
  static char txt [500];
  
  switch (msg)
    {
    case WM_INITDIALOG:
      SetDlgItemText (hwnDlg, IDC_EDITDOCSAVE, currentPathName);
      break;
      
    case WM_COMMAND:
      if (HIWORD (wParam) == EN_UPDATE)
	{
	  if (LOWORD (wParam) == IDC_EDITDOCSAVE)
	    GetDlgItemText (hwnDlg, IDC_EDITDOCSAVE, currentDocToSave, sizeof (currentDocToSave) - 1);
	}
      switch (LOWORD (wParam))
	{
	case IDC_BROWSE:
	  WIN_ListSaveDirectory (BaseDialog + SaveForm, currentDocToSave);
	  SetDlgItemText (hwnDlg, IDC_EDITDOCSAVE, currentDocToSave);
	  break;

	case IDCANCEL:
	  ThotCallback (BaseDialog + SaveForm, INTEGER_DATA, (char*) 0);
	  EndDialog (hwnDlg, IDCANCEL);
	  break;

	case ID_CONFIRM:
	  /* TODO: Extract directory and file name from urlToOpen */
	  TtaExtractName (currentDocToSave, SavePath, ObjectName);
	  ThotCallback (BaseDialog + SaveForm, INTEGER_DATA, (char*) 1);
	  EndDialog (hwnDlg, ID_CONFIRM);
	  break;
	}
      break;
      
    default: return FALSE;
    }
  return TRUE ;
}
#endif /* _WINDOWS */


/*----------------------------------------------------------------------
   SetRelativeURLs: try to make relative URLs within an HTML document.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetRelativeURLs (Document document, char *newpath)
#else
void                SetRelativeURLs (document, newpath)
Document            document;
char               *newpath;
#endif
{
  Element             el, root;
  ElementType         elType;
  Attribute           attr;
  AttributeType       attrType;
  Language            lang;
  char                old_url[MAX_LENGTH];
  char               oldpath[MAX_LENGTH];
  char                tempname[MAX_LENGTH];
  char               *new_url;
  int                 index, max;
  int                 len;

DBG(fprintf(stderr, "SetRelativeURLs\n");)

  root = TtaGetMainRoot (document);
  /* manage style elements */
  elType = TtaGetElementType (root);
  attrType.AttrSSchema = elType.ElSSchema;
  elType.ElTypeNum = HTML_EL_StyleRule;
  el = TtaSearchTypedElement (elType, SearchInTree, root);
  while (el != NULL)
    {
      len = MAX_LENGTH - 1;
      TtaGiveTextContent (el, old_url, &len, &lang);
      old_url[MAX_LENGTH - 1] = EOS;
      new_url = UpdateCSSBackgroundImage (DocumentURLs[document], newpath, NULL, old_url);
      if (new_url != NULL)
	{
	  TtaSetTextContent (el, new_url, lang, document);
	  TtaFreeMemory (new_url);
	}
      /* get next StyleRule */
      TtaNextSibling (&el);
    }

  /* manage URLs and SRCs attributes */
  max = sizeof (URL_attr_tab) / sizeof (int);
  for (index = 0; index < max; index++)
    {
      /* search all elements having this attribute */
      attrType.AttrTypeNum = URL_attr_tab[index];
      TtaSearchAttribute (attrType, SearchForward, root, &el, &attr);
      while (el != NULL)
	{
	  elType = TtaGetElementType (el);
	  if (elType.ElTypeNum != HTML_EL_BASE)
	    {
	      /* get the URL contained in the attribute. */
	      len = MAX_LENGTH - 1;
	      TtaGiveTextAttributeValue (attr, old_url, &len);
	      old_url[MAX_LENGTH - 1] = EOS;
	      if (attrType.AttrTypeNum == HTML_ATTR_Style_)
		{
		  /* manage background-image rule within style attribute */
		  new_url = UpdateCSSBackgroundImage (DocumentURLs[document], newpath, NULL, old_url);
		  if (new_url != NULL)
		    {
		      TtaSetAttributeText (attr, new_url, el, document);
		      TtaFreeMemory (new_url);
		    }
		}
	      /* save the new attribute value */
	      else if (old_url[0] != '#')
		{
		  NormalizeURL (old_url, document, oldpath, tempname, NULL);
		  new_url = MakeRelativeURL (oldpath, newpath);
DBG(fprintf(stderr, "Changed URL from %s to %s\n", old_url, new_url);)
                  /* save the new attribute value */
		  TtaSetAttributeText (attr, new_url, el, document);
		  TtaFreeMemory (new_url);
		}
	    }
	  TtaSearchAttribute (attrType, SearchForward, el, &el, &attr);
	}
    }
}

/*----------------------------------------------------------------------
  InitSaveForm
  Build and display the Save As dialog box and prepare for input.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         InitSaveForm (Document document, View view, char *pathname)
#else
static void         InitSaveForm (document, view, pathname)
Document            document;
View                view;
char               *pathname;

#endif
{
#  ifndef _WINDOWS
   char             buffer[3000];
   char             s[MAX_LENGTH];
   int              i;

   /* Dialogue form for saving in local */
   i = 0;
   strcpy (&s[i], TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
   i += strlen (&s[i]) + 1;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_CLEAR));
   i += strlen (&s[i]) + 1;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_PARSE));
   TtaNewSheet (BaseDialog + SaveForm, TtaGetViewFrame (document, view), 
	       TtaGetMessage (AMAYA, AM_SAVE_AS), 3, s, TRUE, 3, 'L', D_CANCEL);
   sprintf (buffer, "%s%c%s%c%s%cB%s%cB%s", "BHTML", EOS, "BText", EOS, "S", EOS,
	    TtaGetMessage (AMAYA, AM_BCOPY_IMAGES), EOS,
	    TtaGetMessage (AMAYA, AM_BTRANSFORM_URL));
   TtaNewToggleMenu (BaseDialog + ToggleSave, BaseDialog + SaveForm,
		     "Output format", 5, buffer, NULL, TRUE);
   SaveAsHTML = TRUE;
   SaveAsText = FALSE;
   TtaSetToggleMenu (BaseDialog + ToggleSave, 0, SaveAsHTML);
   TtaSetToggleMenu (BaseDialog + ToggleSave, 1, SaveAsText);
   TtaSetToggleMenu (BaseDialog + ToggleSave, 3, CopyImages);
   TtaSetToggleMenu (BaseDialog + ToggleSave, 4, UpdateURLs);
   TtaListDirectory (SavePath, BaseDialog + SaveForm,
		     TtaGetMessage (LIB, TMSG_DOC_DIR),
		     BaseDialog + DirSave, ScanFilter,
		     TtaGetMessage (AMAYA, AM_FILES), BaseDialog + DocSave);
   TtaNewTextForm (BaseDialog + NameSave, BaseDialog + SaveForm,
		   TtaGetMessage (AMAYA, AM_DOC_LOCATION), 50, 1, TRUE);
   TtaSetTextForm (BaseDialog + NameSave, pathname);
   TtaNewLabel (BaseDialog + Label1, BaseDialog + SaveForm, "");
   TtaNewLabel (BaseDialog + Label2, BaseDialog + SaveForm, "");
   TtaNewTextForm (BaseDialog + ImgDirSave, BaseDialog + SaveForm,
		   TtaGetMessage (AMAYA, AM_IMAGES_LOCATION), 50, 1, TRUE);
   TtaSetTextForm (BaseDialog + ImgDirSave, SaveImgsURL);
   TtaNewLabel (BaseDialog + Label3, BaseDialog + SaveForm, "");
   TtaNewLabel (BaseDialog + Label4, BaseDialog + SaveForm, "");
   TtaNewTextForm (BaseDialog + FilterText, BaseDialog + SaveForm,
		   TtaGetMessage (AMAYA, AM_PARSE), 10, 1, TRUE);
   TtaSetTextForm (BaseDialog + FilterText, ScanFilter);
   TtaShowDialogue (BaseDialog + SaveForm, FALSE);
#  else /* _WINDOWS */
   CreateSaveAsDlgWindow (TtaGetViewFrame (document, view), pathname, BaseDialog, SaveForm, DirSave, NameSave, ImgDirSave, ToggleSave);
#  endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
   SaveHTMLDocumentAs                                              
   Entry point called when the user selects the SaveAs function
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SaveDocumentAs (Document document, View view)
#else
void                SaveDocumentAs (document, view)
Document            document;
View                view;

#endif
{
   char             tempname[MAX_LENGTH];
   int              i;

   if (SavingDocument != 0 && SavingDocument != document)
      return;
   if (SavingObject != 0)
      return;

   /* memorize the current document */
   if (SavingDocument == 0)
     {
       SavingDocument = document;
       strcpy (tempname, DocumentURLs[document]);
       /* suppress compress suffixes from tempname */
       i = strlen (tempname) - 1;
       if (i > 2 && !strcmp (&tempname[i-2], ".gz"))
	 {
	   tempname[i-2] = EOS;
	   TtaFreeMemory (DocumentURLs[SavingDocument]);
	   DocumentURLs[SavingDocument] = (char *) TtaStrdup (tempname);
	 }
       else if (i > 1 && !strcmp (&tempname[i-1], ".Z"))
	 {
	   tempname[i-1] = EOS;
	   TtaFreeMemory (DocumentURLs[SavingDocument]);
	   DocumentURLs[SavingDocument] = (char *) TtaStrdup (tempname);
	 }
       
       /* if it is a Web document use the current SavePath */
       if (IsW3Path (tempname))
	 TtaExtractName (tempname, SavePath, SaveName);
       else
	 {
	   TtaGetDocumentDirectory (SavingDocument, tempname, MAX_LENGTH);
	   strcpy (SavePath, tempname);
	   strcpy (SaveName, TtaGetDocumentName (SavingDocument));
	   strcat (tempname, DIR_STR);
	   if (!IsHTMLName (SaveName))
	     strcat (SaveName, ".html");
	   strcat (tempname, SaveName);
	 }
       TtaSetDialoguePosition ();
     }
   else
     {
       strcpy (tempname, SavePath);
       strcat (tempname, DIR_STR);
       strcat (tempname, SaveName);
     }

   /* display the dialog box */
   InitSaveForm (document, 1, tempname);
}


/*----------------------------------------------------------------------
   SaveDocumentLocally save the document in a local file.            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         SaveDocumentLocally (char *directoryName, char *documentName)
#else
static void         SaveDocumentLocally (directoryName, documentName)
char               *directoryName;
char               *documentName;

#endif
{
   char                tempname[MAX_LENGTH];
   char                docname[100];

DBG(fprintf(stderr, "SaveDocumentLocally :  %s / %s\n", directoryName, documentName);)

   strcpy (tempname, directoryName);
   strcat (tempname, DIR_STR);
   strcat (tempname, documentName);
   if (SaveAsText) 
     {
      SetInternalLinks (SavingDocument);
      TtaExportDocument (SavingDocument, tempname, "HTMLTT");
     }
   else
     {
      TtaExportDocument (SavingDocument, tempname, "HTMLT");
      TtaSetDocumentDirectory (SavingDocument, directoryName);
      strcpy (docname, documentName);
      ExtractSuffix (docname, tempname);
      /* Change the document name in all views */
      TtaSetDocumentName (SavingDocument, docname);
      TtaSetTextZone (SavingDocument, 1, 1, DocumentURLs[SavingDocument]);
      TtaSetDocumentUnmodified (SavingDocument);
     }
}

/*----------------------------------------------------------------------
  AddNoName
  This function is called whenether one tries to save a document
  without name (just the directory path e.g. http://www.w3.org/pub/WWW/ )
  It ask the user whether an extra name suffix should be added or
  abort.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean      AddNoName (Document document, View view, char *url, boolean *ok)
#else
static boolean      AddNoName (document, view, url, ok)
Document            document;
View                view;
char               *url;
boolean            *ok;
#endif
{
   char                msg[MAX_LENGTH];
   char                documentname[MAX_LENGTH];

DBG(fprintf(stderr, "AddNoName :  %s \n", url);)

   TtaExtractName (url, msg, documentname);
   *ok = (documentname[0] != EOS);
   if (*ok)
     return (FALSE);
   else
     {
       /* the name is not correct for the put operation */
       strcpy (msg, TtaGetMessage(AMAYA, AM_NO_NAME));
       strcat (msg, url);
       strcat (msg, DIR_STR);
       strcat (msg, "noname.html");
       InitConfirm (document, view, msg);
       if (UserAnswer == 0)
	 return (FALSE);
       else
	 return (TRUE);
     }
}

/*----------------------------------------------------------------------
  SafeSaveFileThroughNet
  Send a file through the Network (using the PUT HTTP method) and double
  check for errors using a following GET.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          SafeSaveFileThroughNet (Document doc, char *localfile,
                          char *remotefile, PicType filetype)
#else
static int          SafeSaveFileThroughNet (doc, localfile, remotefile, filetype)
Document            doc;
char               *localfile;
char               *remotefile;
PicType             filetype;
#endif
{
  char msg[MAX_LENGTH];
  char tempfile[MAX_LENGTH]; /* Name of the file used to refetch */
  char tempURL[MAX_LENGTH];  /* May be redirected */
  char *no_reread_check;
  char *no_write_check;
  int res;

  no_reread_check = TtaGetEnvString("NO_REREAD_CHECK");
  no_write_check = TtaGetEnvString("NO_WRITE_CHECK");
  
DBG(fprintf(stderr, "SafeSaveFileThroughNet :  %s to %s type %d\n", localfile, remotefile, filetype);)

  /* Save */
#ifdef AMAYA_JAVA
  res = PutObjectWWW (doc, localfile, remotefile, AMAYA_SYNC | AMAYA_NOCACHE, filetype, NULL, NULL);
#else /* AMAYA_JAVA */
  res = PutObjectWWW (doc, localfile, remotefile, AMAYA_SYNC, filetype, NULL, NULL);
#endif /* AMAYA_JAVA */
  if (res)
    /* The HTTP PUT method failed ! */
    return (res);
  if (no_reread_check != NULL)
    return (0);

  /* Refetch */
DBG(fprintf(stderr, "SafeSaveFileThroughNet :  refetch %s \n", remotefile);)

  TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_VERIFYING), "");
  strcpy (tempURL, remotefile);
#ifdef AMAYA_JAVA
  res = GetObjectWWW (doc, tempURL, NULL, tempfile, AMAYA_SYNC | AMAYA_NOCACHE,
		      NULL, NULL, NULL, NULL, NO, NULL);
#else /* AMAYA_JAVA */
  res = GetObjectWWW (doc, tempURL, NULL, tempfile, AMAYA_SYNC, NULL, NULL, NULL, NULL, YES, NULL);
#endif /* AMAYA_JAVA */
  if (res)
    {
      /* The HTTP GET method failed ! */
      sprintf (msg, TtaGetMessage (AMAYA, AM_SAVE_RELOAD_FAILED), remotefile);
      InitConfirm (doc, 1, msg);
      if (UserAnswer)
	/* Ignore the read failure */
	res = 0;
    }
  else if (strcmp (remotefile, tempURL))
    {
      /* Warning : redirect... */
      sprintf (msg, TtaGetMessage (AMAYA, AM_SAVE_REDIRECTED), remotefile, tempURL);
      InitConfirm (doc, 1, msg);
      if (!UserAnswer)
	/* Trigger the error */
	res = -1;
    }

  if (no_write_check == NULL && res == 0)
    {
      /* Compare content. */
DBG(fprintf(stderr, "SafeSaveFileThroughNet :  compare %s and %s \n", remotefile, localfile);)
#ifdef AMAYA_JAVA
      if (! TtaCompareFiles(tempfile, localfile))
	{
	  sprintf (msg, TtaGetMessage (AMAYA, AM_SAVE_COMPARE_FAILED), remotefile);
	  InitConfirm (doc, 1, msg);
	  if (!UserAnswer)
	    res = -1;
	}
#endif /* AMAYA_JAVA */
    }

  if (TtaFileExist (tempfile))
    TtaFileUnlink (tempfile);
  return(res);
}

/*----------------------------------------------------------------------
  SaveDocumentThroughNet
  Save a document and the included images to a remote network location.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          SaveDocumentThroughNet (Document document, View view,
					 boolean confirm, boolean with_images)
#else
static int          SaveDocumentThroughNet (document, view, confirm,
                                         with_images)
Document            document;
View                view;
boolean             confirm;
boolean             with_images;
#endif
{
  LoadedImageDesc *pImage;
  char            *tempname;
  char            *msg;
  int              remainder = 10000;
  int              index = 0, len, nb = 0;
  int              res;
  int              imageType;

  if (!IsW3Path (DocumentURLs[document]))
    return (-1);

  /*
   * Don't use memory allocated on the stack ! May overflow the 
   * memory allocated for this Java thread.
   */
  /* save into the temporary document file */
  tempname = GetLocalPath (document, DocumentURLs[document]);
  msg = TtaGetMemory(remainder);
  if (msg == NULL)
    return (-1);

  /* First step : build the output and ask for confirmation */
  TtaExportDocument (document, tempname, "HTMLT");
  res = 0;
  if (confirm && with_images)
    {
#ifndef _WINDOWS
      TtaNewForm (BaseDialog + ConfirmSave, TtaGetViewFrame (document, view), 
		  TtaGetMessage (LIB, TMSG_LIB_CONFIRM), TRUE, 1, 'L', D_CANCEL);
      TtaNewLabel (BaseDialog + Label1, BaseDialog + ConfirmSave, TtaGetMessage (AMAYA, AM_WARNING_SAVE_OVERWRITE));
#endif /* _WINDOWS */
       
      strcpy (&msg[index], DocumentURLs[document]);
      len = strlen (DocumentURLs[document]);
      len++;
      remainder -= len;
      index += len;
      nb++;

      pImage = ImageURLs;
      while (pImage != NULL)
	{
	  if (pImage->document == document && pImage->status == IMAGE_MODIFIED)
	    {
	      if (nb > 30)
		{
		  strcpy (&msg[index], "...");
		  len = strlen ("...");
		  len++;
		  remainder -= len;
		  index += len;
		  nb++;
		  break;
		}
	      strcpy (&msg[index], pImage->originalName);
	      len = strlen (pImage->originalName);
	      len++;
	      remainder -= len;
	      index += len;
	      nb++;
	    }
	  pImage = pImage->nextImage;
	}

#ifndef _WINDOWS 
      TtaNewSelector (BaseDialog + ConfirmSaveList, BaseDialog + ConfirmSave,
		      NULL, nb, msg, 6, NULL, FALSE, TRUE);
       
      TtaSetDialoguePosition ();
      TtaShowDialogue (BaseDialog + ConfirmSave, FALSE);
      /* wait for an answer */
      TtaWaitShowDialogue ();
#else  /* _WINDOWS */
      CreateSaveListDlgWindow (TtaGetViewFrame (document, view), nb, msg, BaseDialog, ConfirmSave);
#endif /* _WINDOWS */
      if (!UserAnswer)
	res = -1;
    }

  /*
   * Second step : saving the HTML content and the images modified locally.
   *               if saving failed, suggest to save to disk.
   */
  if (res == 0)
    {
      ActiveTransfer (document);
      TtaHandlePendingEvents ();
      pImage = NULL;

      res = SafeSaveFileThroughNet (document, tempname,
				    DocumentURLs[document], unknown_type);
      if (res)
	{
#if !defined(AMAYA_JAVA) && !defined(AMAYA_ILU)
	  DocNetworkStatus[document] |= AMAYA_NET_ERROR;
#endif /* AMAYA_JAVA || AMAYA_ILU */
	  ResetStop (document);
#if defined(AMAYA_JAVA) || defined(AMAYA_ILU)
	  sprintf (msg, "%s %s \n%s",
		   TtaGetMessage (AMAYA, AM_URL_SAVE_FAILED),
		   DocumentURLs[document],
		   TtaGetMessage (AMAYA, AM_SAVE_DISK));
#else /* AMAYA_JAVA || AMAYA_ILU */
	  sprintf (msg, "%s %s \n%s\n%s",
		   TtaGetMessage (AMAYA, AM_URL_SAVE_FAILED),
		   DocumentURLs[document],
		   AmayaLastHTTPErrorMsg,
		   TtaGetMessage (AMAYA, AM_SAVE_DISK));
#endif /* AMAYA_JAVA || AMAYA_ILU */
	  InitConfirm (document, view, msg);
	  /* JK: to erase the last status message */
	  TtaSetStatus (document, view, "", NULL);	       
	  if (UserAnswer)
	    res = -1;
	  else
	    res = 0;
	}
      else
	{
	  TtaSetDocumentUnmodified (document);
	  if (with_images)
	    pImage = ImageURLs;
	}

      while (pImage != NULL)
	{
	  if (pImage->document == document && pImage->status == IMAGE_MODIFIED)
	    {
	      imageType = (int) TtaGetPictureType ((Element) pImage->elImage);
	      res = SafeSaveFileThroughNet(document, pImage->localName,
					   pImage->originalName, imageType);
	      if (res)
		{
#if !defined(AMAYA_JAVA) && !defined(AMAYA_ILU)
		  DocNetworkStatus[document] |= AMAYA_NET_ERROR;
#endif /* AMAYA_JAVA  || AMAYA_ILU */
		  ResetStop (document);
#if defined(AMAYA_JAVA) || defined(AMAYA_ILU)
		  sprintf (msg, "%s %s \n%s",
			   TtaGetMessage (AMAYA, AM_URL_SAVE_FAILED),
			   pImage->originalName, 
			   TtaGetMessage (AMAYA, AM_SAVE_DISK));
#else /* AMAYA_JAVA || AMAYA_ILU */
		  sprintf (msg, "%s %s \n%s\n%s",
			   TtaGetMessage (AMAYA, AM_URL_SAVE_FAILED),
			   pImage->originalName, 
			   AmayaLastHTTPErrorMsg,
			   TtaGetMessage (AMAYA, AM_SAVE_DISK));
#endif /* AMAYA_JAVA || AMAYA_ILU */
		  InitConfirm (document, view, msg);
		  /* erase the last status message */
		  TtaSetStatus (document, view, "", NULL);
		  if (UserAnswer)
		    res = -1;
		  else
		    res = 0;
		  /* do not continue */
		  pImage = NULL;
		}
	      else
		pImage->status = IMAGE_LOADED;
	    }

	  if (pImage != NULL)
	    pImage = pImage->nextImage;
	}
      ResetStop (document);
    }

DBG(fprintf(stderr, "Saving completed\n");)
  TtaFreeMemory (msg);
  if (tempname)
    TtaFreeMemory (tempname);
  return (res);
}

/*----------------------------------------------------------------------
  SaveDocument
  Entry point called whenether the user select the Save menu entry or
  press the Save button.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SaveDocument (Document document, View view)
#else
void                SaveDocument (document, view)
Document            document;
View                view;

#endif
{
   char                tempname[MAX_LENGTH];
   int                 i;
   boolean             ok;

   if (SavingDocument != 0)
      return;
   else if (!TtaIsDocumentModified (document))
     {
       TtaSetStatus (document, 1, TtaGetMessage (AMAYA, AM_NOTHING_TO_SAVE), "");

       return;
     }
   SavingDocument = document;
   ok = FALSE;

   /* attempt to save through network if possible */
   strcpy (tempname, DocumentURLs[document]);
   /* suppress compress suffixes from tempname */
   i = strlen (tempname) - 1;
   if (i > 2 && !strcmp (&tempname[i-2], ".gz"))
     {
       tempname[i-2] = EOS;
       TtaFreeMemory (DocumentURLs[SavingDocument]);
       DocumentURLs[SavingDocument] = (char *) TtaStrdup (tempname);
     }
   else if (i > 1 && !strcmp (&tempname[i-1], ".Z"))
     {
       tempname[i-1] = EOS;
       TtaFreeMemory (DocumentURLs[SavingDocument]);
       DocumentURLs[SavingDocument] = (char *) TtaStrdup (tempname);
     }

DBG(fprintf(stderr, "SaveDocument : %d to %s\n", document, tempname);)

   if (IsW3Path (tempname))
     {
       if (AddNoName (document, view, tempname, &ok))
	 {
	   ok = TRUE;
	   /* need to update the document url */
	   strcat (tempname, DIR_STR);
	   strcat (tempname, "noname.html");
	   TtaFreeMemory (DocumentURLs[SavingDocument]);
	   DocumentURLs[SavingDocument] = (char *) TtaStrdup (tempname);
	 }

DBG(fprintf(stderr, "SaveDocument : remote saving\n");)

       if (ok && SaveDocumentThroughNet (document, view, FALSE, TRUE) == 0)
	 {
	   TtaSetStatus (document, 1, TtaGetMessage (AMAYA, AM_SAVED), DocumentURLs[document]);
	   SavingDocument = 0;
	   ok = TRUE;
	 }
       else
	 ok = FALSE;

       if (!ok)
	 {
	   /* cannot save */
	   TtaSetStatus (document, 1, TtaGetMessage (AMAYA, AM_CANNOT_SAVE), DocumentURLs[document]);
	   SavingDocument = 0;
	   SaveDocumentAs (document, 1);
	 }
       }
   else
     {

DBG(fprintf(stderr, "SaveDocument : local saving\n");)

       TtaExportDocument (document, tempname, "HTMLT");
       TtaSetDocumentUnmodified (document);
       TtaSetStatus (document, 1, TtaGetMessage (AMAYA, AM_SAVED), DocumentURLs[document]);
       SavingDocument = 0;
     }
}


/*----------------------------------------------------------------------
  BackupAll save all opened documents when the application crashes
  ----------------------------------------------------------------------*/
void                   BackUpDocs ()
{
  Document             doc;
  char                 pathname[MAX_LENGTH];
  char                 docname[MAX_LENGTH];
  char                *ptr;
  int                  l;

  /* check all modified documents */
  for (doc = 1; doc < DocumentTableLength; doc++)
    if (DocumentURLs[doc] != NULL && TtaIsDocumentModified (doc))
      {
	SavingDocument = 0;
	ptr = DocumentURLs[doc];
	l = strlen (ptr) - 1;
	if (ptr[l] == DIR_SEP || ptr[l] == URL_SEP)
	  {
	    /* it's a directory name */
	    ptr[l] = EOS;
	    l = 0;
	  }
	TtaExtractName (DocumentURLs[doc], pathname, docname);
	if (l == 0)
	  sprintf (pathname, "%s%c%%%s.html", TempFileDirectory, DIR_SEP, docname);
	else
	  sprintf (pathname, "%s%c%%%s", TempFileDirectory, DIR_SEP, docname);
 	DocumentURLs[doc] = pathname;
	SaveDocument (doc, 1);
      }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             DocumentToSave (NotifyDialog * event)
#else  /* __STDC__ */
boolean             DocumentToSave (event)
NotifyDialog       *event;

#endif /* __STDC__ */
{
   SaveDocument (event->document, 1);
   /* This function has to be written */
   return TRUE;
}


/*----------------------------------------------------------------------
  UpdateDocAndImages
  change the document URL and if CopyImage is TRUE change all
  picture SRC attribute. If pictures are saved locally, make the copy
  else add them to the list of remote images to be copied.
  The parameter imgbase gives the relative path of the new image directory.
  The parameter newURL gives the new document URL (or local file).
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void            UpdateDocAndImages (boolean src_is_local, boolean dst_is_local, char *imgbase, char *newURL)
#else
static void            UpdateDocAndImages (src_is_local, dst_is_local, imgbase, newURL)
boolean                src_is_local;
boolean                dst_is_local;
char                  *imgbase;
char                  *newURL;
#endif
{
   AttributeType       attrType;
   ElementType         elType;
   Attribute           attr;
   Element             el, root;
   LoadedImageDesc    *pImage;
   Language            lang;
   char                tempfile[MAX_LENGTH];
   char                localpath[MAX_LENGTH];
   char                oldpath[MAX_LENGTH];
   char                tempname[MAX_LENGTH];
   char                imgname[MAX_LENGTH];
   char                url[MAX_LENGTH];
   char               *buf, *ptr;
   char                url_str [2];
   int                 buflen, max, index;

   if (imgbase && strchr (newURL, '/'))
      sprintf (url_str, "/");
   else 
      sprintf (url_str, "%s", DIR_STR);

   /* save the old document path to locate existing images */
   strcpy (oldpath, DocumentURLs[SavingDocument]);
   buflen = strlen (oldpath) - 1;
   if (oldpath[buflen] ==  '/')
     oldpath[buflen] = EOS;
   /* path to search image descriptors */
   sprintf (localpath, "%s%s%d%s", TempFileDirectory, DIR_STR, SavingDocument, DIR_STR);
   /* change the document url if it is not saved as Text */
   if (!SaveAsText)
      {
	TtaFreeMemory (DocumentURLs[SavingDocument]);
	DocumentURLs[SavingDocument] = (char *) TtaStrdup (newURL);
      }

   if (CopyImages)
     {
       /* Change all Picture SRC and prepare the saving process */
       /* 
	*                       \   newpath=local |  newpath=remote
	* oldpath                \                |
	* ------------------------|---------------|------------------
	*        | old img=remote | .amaya->file  | update descriptor
	*  local |----------------|---------------|------------------
	*        | old img=local  | file->file    | add descriptor
	* ------------------------|---------------|------------------
	*        | old img=remote | .amaya->file  | update descriptor
	* remote |----------------|---------------|------------------
	*        | old img=local  |   xxxxxxxxxxxxxxxxxxxxxxxxxxxx
	* ------------------------|---------------|------------------
	*/

       root = TtaGetMainRoot (SavingDocument);
       /* manage style elements */
       elType = TtaGetElementType (root);
       attrType.AttrSSchema = elType.ElSSchema;
       elType.ElTypeNum = HTML_EL_StyleRule;
       el = TtaSearchTypedElement (elType, SearchInTree, root);
       while (el != NULL)
	 {
	   el = TtaGetFirstChild (el);
	   buflen = MAX_LENGTH;
	   buf = (char *) TtaGetMemory (buflen);
	   if (buf == NULL)
	     break;
	   TtaGiveTextContent (el, buf, &buflen, &lang);
	   url[0] = EOS;
	   tempname[0] = EOS;
	   ptr = UpdateCSSBackgroundImage (oldpath, newURL, imgbase, buf);
	   if (ptr != NULL)
	     {
	       /* save this new style element string */
	       TtaSetTextContent (el, ptr, lang, SavingDocument);
	       strcpy (url, ptr);
	       TtaFreeMemory (ptr);

	       /* extract the URL from the new style string */
	       ptr = GetCSSBackgroundURL (url);
	       if (ptr != NULL)
		 {
		   strcpy (url, ptr);
		   TtaFreeMemory (ptr);
		   NormalizeURL (url, SavingDocument, tempname, imgname, NULL);
		 }

	       /* extract the URL from the old style string */
	       ptr = GetCSSBackgroundURL (buf);
	       if (ptr != NULL)
		 {
		   NormalizeURL (ptr, 0, buf, imgname, oldpath);
		   TtaFreeMemory (ptr);
		 }
	       else
		 buf[0] = EOS;
	     }

	   /*
	     At this point:
	     - url gives the relative new image name
	     - tempname gives the new image full name
	     - buf gives the old image full name
	     - imgname contains the image file name
	     */
	   if (url[0] != EOS && buf[0] != EOS)
	     {
	       if ((src_is_local) && (!dst_is_local))
		 /* add the existing localfile to images list to be saved */
		 AddLocalImage (buf, imgname, tempname, SavingDocument, &pImage);
	       
	       /* mark the image descriptor or copy the file */
	       if (dst_is_local)
		 {
		   /* copy the file to the new location */
		   if (IsW3Path (buf) || IsHTTPPath (oldpath))
		     {
		       /*
			 it was a remote image:
			 we use the local temporary name to do the copy
			 */
		       strcpy (buf, localpath);
		       strcat (buf, imgname);
		     }

		   if (imgbase[0] != EOS)
		     {
		       strcpy (tempfile, imgbase);
		       strcat (tempfile, DIR_STR);
		       strcat (tempfile, imgname);
		     }
		   else
		     {
		       strcpy (tempfile, SavePath);
		       strcat (tempfile, DIR_STR);
		       strcat (tempfile, imgname);
		     }
		   
		   TtaFileCopy (buf, tempfile);
		 }
	       else
		 {
		   /* save on a remote server */
		   if (IsW3Path (buf) || IsHTTPPath (oldpath))
		     {
		       /*
			 it was a remote image:
			 get the image descriptor to prepare
			 the saving process
			 */
		       strcpy (tempfile, localpath);
		       strcat (tempfile, imgname);
		       pImage = SearchLoadedImage (tempfile, SavingDocument);
		       /* update the descriptor */
		       if (pImage)
			 {
			   /* image was already loaded */
			   if (pImage->originalName != NULL)
			     TtaFreeMemory (pImage->originalName);
			   pImage->originalName = (char *) TtaStrdup (tempname);
			   if (TtaFileExist(pImage->localName))
			     pImage->status = IMAGE_MODIFIED;
			   else
			     pImage->status = IMAGE_NOT_LOADED;
			   pImage->elImage = (struct _ElemImage *) el;
			 }
		     }
		   else
		     AddLocalImage (buf, imgname, tempname, SavingDocument, &pImage);
		 }
	     }
	   TtaFreeMemory (buf);
	   /* get next StyleRule */
	   TtaNextSibling (&el);
	 }

       max = sizeof (SRC_attr_tab) / sizeof (int);
       for (index = 0; index < max; index++)
	 {
	   /* fetch a new attrValue */
	   attrType.AttrTypeNum = SRC_attr_tab[index];
	   TtaSearchAttribute (attrType, SearchForward, root, &el, &attr);
	   while (el != NULL)
	     {
	       elType = TtaGetElementType (el);
	       if (elType.ElTypeNum == HTML_EL_PICTURE_UNIT ||
			attrType.AttrTypeNum == HTML_ATTR_Style_)
		 {
#ifdef COUGAR
		   elType = TtaGetElementType (TtaGetParent(el));
		   if (elType.ElTypeNum != HTML_EL_Object)
		     {
#endif
		       buflen = MAX_LENGTH;
		       buf = (char *) TtaGetMemory (buflen);
		       if (buf == NULL)
			 break;
		       TtaGiveTextAttributeValue (attr, buf, &buflen);
		       if (attrType.AttrTypeNum == HTML_ATTR_Style_)
			 {
			   /* It's an attribute Style: look for url()*/
			   url[0] = EOS;
			   tempname[0] = EOS;
			   ptr = UpdateCSSBackgroundImage (oldpath, newURL, imgbase, buf);
			   if (ptr != NULL)
			     {
			       /* save this new style attribute string */
			       TtaSetAttributeText (attr, ptr, el, SavingDocument);
			       strcpy (url, ptr);
			       TtaFreeMemory (ptr);
			       /* extract the URL from the new style string */
			       ptr = GetCSSBackgroundURL (url);
			       if (ptr != NULL)
				 {
				   strcpy (url, ptr);
				   TtaFreeMemory (ptr);
				   NormalizeURL (url, SavingDocument, tempname, imgname, NULL);
				 }
			       /* extract the URL from the old style string */
			       ptr = GetCSSBackgroundURL (buf);
			       if (ptr != NULL)
				 {
				   NormalizeURL (ptr, 0, buf, imgname, oldpath);
				   TtaFreeMemory (ptr);
				 }
			       else
				 buf[0] = EOS;
			     }
			 }
		       else
			 {
			   /* extract the old image name and location */
			   strcpy (url, buf);
			   NormalizeURL (url, 0, buf, imgname, oldpath);
			   /* save the new SRC attr value */
			   if (imgbase[0] != EOS)
			     {
			       /* compose the relative or absolute name */
			       strcpy (url, imgbase);
			       strcat (url, url_str);
			       strcat (url, imgname);
			     }
			   else
			     /* in same directory -> local name */
			     strcpy (url, imgname);

			   NormalizeURL (url, SavingDocument, tempname, imgname, NULL);
			   TtaSetAttributeText (attr, url, el, SavingDocument);
			 }

		       /*
			 At this point:
			 - url gives the relative new image name
			 - tempname gives the new image full name
			 - buf gives the old image full name
			 - imgname contains the image file name
			 */
DBG(fprintf(stderr, "     SRC from %s to %s\n", buf, url);)
		       if (url[0] != EOS && buf[0] != EOS)
			 {
			   if ((src_is_local) && (!dst_is_local))
			     /* add the localfile to the images list */
			     AddLocalImage (buf, imgname, tempname, SavingDocument, &pImage);
			     
			   /* mark the image descriptor or copy the file */
			   if (dst_is_local)
			     {
			       /* do a file copy */
			       if (IsW3Path (buf) || IsHTTPPath (oldpath))
				 {
				   /*
				     it was a remote image:
				     we use the local temporary name to do the copy
				     */
				   strcpy (buf, localpath);
				   strcat (buf, imgname);
				 }

			       if (imgbase[0] != EOS)
				 {
				   strcpy (tempfile, imgbase);
				   strcat (tempfile, DIR_STR);
				   strcat (tempfile, imgname);
				 }
			       else
				 {
				   strcpy (tempfile, SavePath);
				   strcat (tempfile, DIR_STR);
				   strcat (tempfile, imgname);
				 }
			       TtaFileCopy (buf, tempfile);
			     }
			   else
			     {
			       /* save on a remote server */
			       if (IsW3Path (buf) || IsHTTPPath (oldpath))
				 {
				   /*
				     it was a remote image:
				     get the image descriptor to prepare
				     the saving process
				    */
				   strcpy (tempfile, localpath);
				   strcat (tempfile, imgname);
				   pImage = SearchLoadedImage (tempfile, SavingDocument);
				   /* update the descriptor */
				   if (pImage)
				     {
				       /* image was already loaded */
				       if (pImage->originalName != NULL)
					 TtaFreeMemory (pImage->originalName);
				       pImage->originalName = (char *) TtaStrdup (tempname);
				       if (TtaFileExist(pImage->localName))
					 pImage->status = IMAGE_MODIFIED;
				       else
					 pImage->status = IMAGE_NOT_LOADED;
				       pImage->elImage = (struct _ElemImage *) el;
				     }
				 }
			       else
				 /* add the localfile to the images list */
				 AddLocalImage (tempfile, imgname, tempname, SavingDocument, &pImage);
			     }
			 }
		       TtaFreeMemory (buf);
#ifdef COUGAR
		     }
#endif
		 }
	       TtaSearchAttribute (attrType, SearchForward, el, &el, &attr);
	     }
	 }
     }
}


/*----------------------------------------------------------------------
  DoSaveAs
  This function is called when the user press the OK button on the
  Save As dialog. This is tricky, one must take care of a lot of
  parameters, whether initial and final location are local or remote
  and recomputes URLs accordingly.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DoSaveAs (void)
#else
void                DoSaveAs ()
#endif
{
  Document            doc;
  AttributeType       attrType;
  ElementType         elType;
  Element             el, root;
  char               *documentFile;
  char               *tempname, *localPath;
  char               *imagePath, *base;
  char                imgbase[MAX_LENGTH];
  char                url_sep;
  int                 res;
  boolean             src_is_local;
  boolean             dst_is_local, ok;

  src_is_local = !IsW3Path (DocumentURLs[SavingDocument]);
  dst_is_local = !IsW3Path (SavePath);
  ok = TRUE;

DBG(fprintf(stderr, "DoSaveAs : from %s to %s/%s , with images %d\n", DocumentURLs[SavingDocument], SavePath, SaveName, (int) CopyImages);)

  /* New document path */
  documentFile = TtaGetMemory (MAX_LENGTH);
  strcpy (documentFile, SavePath);
  if (dst_is_local)
    {
      strcat (documentFile, DIR_STR);
      url_sep = DIR_SEP;
    }
  else
    {
      strcat (documentFile, "/");
      url_sep = '/';
    }
  strcat (documentFile, SaveName);
  if (SaveName[0] == EOS)
    {
      /* there is no document name */
      if (AddNoName (SavingDocument, 1, documentFile, &ok))
	{
	  res = strlen(SavePath) - 1;
	  if (SavePath[res] == url_sep)
	    SavePath[res] = EOS;
	  /* need to update the document url */
	  strcpy (SaveName, "noname.html");
DBG(fprintf(stderr, " set SaveName to noname.html\n");)
	}
      else if (!ok)
	{
	  /* save into the temporary document file */
	  doc = SavingDocument;
	  TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_CANNOT_SAVE), DocumentURLs[doc]);
	  SavingDocument = 0;
	  SaveDocumentAs (doc, 1);
	}
    }

  if (ok && dst_is_local)
    {
      /* verify that the directory exists */
      if (!TtaCheckDirectory (SavePath))
	{
	  doc = SavingDocument;
	  TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_CANNOT_SAVE), SavePath);
	  /* the user has to change the name of the images directory */
	  SaveDocumentAs(SavingDocument, 1);
	  ok = FALSE;
	}
      /* verify that we don't overwite anything and ask for confirmation */
      else if (TtaFileExist (documentFile))
	{
	  /* ask confirmation */
	  tempname = TtaGetMemory (MAX_LENGTH);
	  sprintf (tempname, TtaGetMessage (LIB, TMSG_FILE_EXIST), documentFile);
	  InitConfirm (SavingDocument, 1, tempname);
	  TtaFreeMemory (tempname);
	  if (!UserAnswer)
	    {
	      /* the user has to change the name of the saving file */
	      SaveDocumentAs(SavingDocument, 1);
	      ok = FALSE;
	    }
	}
    }

  if (ok)
    {
      /* search if there is a BASE element within the document */
      root = TtaGetMainRoot (SavingDocument);
      elType.ElSSchema = TtaGetDocumentSSchema (SavingDocument);
      attrType.AttrSSchema = elType.ElSSchema;
      /* search the BASE element */
      elType.ElTypeNum = HTML_EL_BASE;
      el = TtaSearchTypedElement (elType, SearchInTree, root);
      if (el)
	/* URLs are still relative to the document base */
	base = GetBaseURL (SavingDocument);
      else
	base = NULL;
      
      /* Create the base directory/url for the images output */
      if (CopyImages && SaveImgsURL[0] != EOS)
	{
	  if (base)
	    imagePath = MakeRelativeURL (SaveImgsURL, base);
	  else
	    imagePath = MakeRelativeURL (SaveImgsURL, documentFile);
	  if (imagePath != NULL)
	    {
	      strcpy (imgbase, imagePath);
	      TtaFreeMemory (imagePath);
	    }
	  else
	    imgbase[0] = EOS;
	  
	  /* verify that the directory exists */
	  if (dst_is_local)
	    {
	      tempname = TtaGetMemory (MAX_LENGTH);
	      strcpy (tempname, SavePath);
	      strcat (tempname, DIR_STR);
	      strcat (tempname, imgbase);
	      ok = TtaCheckDirectory (tempname);
	      if (!ok)
		{
		  doc = SavingDocument;
		  TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_CANNOT_SAVE), tempname);
		  TtaFreeMemory (tempname);
		  /* free base before returning*/
		  if (base)
		    TtaFreeMemory (base);
		  /* the user has to change the name of the images directory */
		  SaveDocumentAs(SavingDocument, 1);
		}
	      else
		TtaFreeMemory (tempname);
	    }
	}
      else if (CopyImages)
	{
	  if (base)
	    {
	      imagePath = MakeRelativeURL (SavePath, base);
	      strcpy (imgbase, imagePath);
	      TtaFreeMemory (imagePath);
	    }
	  else
	    imgbase[0] = EOS;
	}
      else
	imgbase[0] = EOS;
    }

  if (ok)
    {
      /* Transform all URLs to absolute ones */
        if (UpdateURLs)
	  {
	    if (base)
	      /* URLs are still relative to the document base */
	      SetRelativeURLs (SavingDocument, base);
	    else
	      /* URLs are relative to the new document directory */
	      SetRelativeURLs (SavingDocument, documentFile);
	  }
	/* now free base */
	if (base)
	  TtaFreeMemory (base);
	
	if (!src_is_local)
	  /* store the name of the local temporary file */
	  localPath = GetLocalPath (SavingDocument, DocumentURLs[SavingDocument]);
	else
	  localPath = NULL;
	/* Change the document URL and if CopyImage is TRUE change all
	 * picture SRC attribute. If pictures are saved locally, make the copy
	 * else add them to the list of remote images to be copied.
	 */
	UpdateDocAndImages (src_is_local, dst_is_local, imgbase, documentFile);
	  
	/* update informations on the document. */
	TtaSetTextZone (SavingDocument, 1, 1, DocumentURLs[SavingDocument]);
	  
	if (dst_is_local)
	  {
	    /* Local to Local or Remote to Local */
DBG(fprintf(stderr, "   Saving document locally : to %s\n", documentFile);)

	    /* save the local document */
	    SaveDocumentLocally (SavePath, SaveName);
	    TtaSetStatus (SavingDocument, 1, TtaGetMessage (AMAYA, AM_SAVED), documentFile);
	    SavingDocument = 0;
	  }
	else
	  {
	    /* Local to Remote or Remote to Remote */
DBG(fprintf(stderr, "   Uploading document to net %s\n", documentFile);)

	    /* now save the file as through the normal process of saving */
	    /* to a remote URL. */
            doc = SavingDocument;
	    res = SaveDocumentThroughNet (SavingDocument, 1, TRUE, CopyImages);
	    
	    if (res)
	      /* restore all urls */
	      SaveDocumentAs(doc, 1);
	    else
	      {
		TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_SAVED), documentFile);
		SavingDocument = 0;
	      }
	  }

	/* remove the previous temporary file */
	if (localPath)
	  {
	    TtaFileUnlink (localPath);
	    TtaFreeMemory (localPath);
	  }
    }
  TtaFreeMemory (documentFile);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                InitSaveObjectForm (Document document, View view, char *object,
					char *pathname)
#else
void                InitSaveObjectForm (document, view, object, pathname)
Document            document;
View                view;
char               *object;
char               *pathname;

#endif
{
   char                tempdir[MAX_LENGTH];

   if (SavingDocument != 0)
      return;
   if (SavingObject != 0)
      return;
   SavingObject = document;
   strncpy (tempSavedObject, object, sizeof (tempSavedObject));

#  ifndef _WINDOWS
   /* Dialogue form for saving as */
   TtaNewForm (BaseDialog + SaveForm, TtaGetViewFrame (document, view), 
	       TtaGetMessage (AMAYA, AM_SAVE_AS), TRUE, 2, 'L', D_CANCEL);
   TtaListDirectory (SavePath, BaseDialog + SaveForm,
		     TtaGetMessage (LIB, TMSG_DOC_DIR),
		     BaseDialog + DirSave, "",
		     TtaGetMessage (AMAYA, AM_FILES), BaseDialog + DocSave);
   TtaNewTextForm (BaseDialog + NameSave, BaseDialog + SaveForm,
		   TtaGetMessage (AMAYA, AM_OBJECT_LOCATION), 50, 1, TRUE);
   TtaSetTextForm (BaseDialog + NameSave, pathname);
   TtaExtractName (pathname, tempdir, ObjectName);
   TtaSetDialoguePosition ();
   TtaShowDialogue (BaseDialog + SaveForm, FALSE);
#  else  /* _WINDOWS */
   CreateGetSaveDlgWindow (TtaGetViewFrame (document, view), pathname);
#  endif /* _WINDOWS */
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DeleteTempObjectFile (void)
#else
void                DeleteTempObjectFile ()
#endif
{
   TtaFileUnlink (tempSavedObject);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DoSaveObjectAs (void)
#else
void                DoSaveObjectAs ()
#endif
{
   char                tempfile[MAX_LENGTH];
   char                msg[MAX_LENGTH];
   boolean             dst_is_local;
   int                 res;

   dst_is_local = !IsW3Path (SavePath);

   strcpy (tempfile, SavePath);
   strcat (tempfile, DIR_STR);
   strcat (tempfile, ObjectName);


   if (!dst_is_local)
     {
	res = PutObjectWWW (SavingObject, tempSavedObject, tempfile,
	                    unknown_type, AMAYA_SYNC, NULL, NULL);

	if (res)
	  {
	     TtaSetDialoguePosition ();
	     TtaShowDialogue (BaseDialog + SaveForm, FALSE);
	     return;
	  }
	SavingObject = 0;
	SavingDocument = 0;
	return;
     }
   if (TtaFileExist (tempfile))
     {
	/* ask confirmation */
	sprintf (msg, TtaGetMessage (LIB, TMSG_FILE_EXIST), tempfile);
	InitConfirm (SavingObject, 1, msg);
	if (!UserAnswer)
	  {
	     /* the user has to change the name of the saving file */
	     TtaSetDialoguePosition ();
	     TtaShowDialogue (BaseDialog + SaveForm, FALSE);
	     return;
	  }
     }
   TtaFileCopy (tempSavedObject, tempfile);
   /* delete the temporary file */
   DeleteTempObjectFile ();
   SavingObject = 0;
   SavingDocument = 0;
}

