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
 *          R. Guetari: Windows NT/95
 *
 */

/* DEBUG_AMAYA_SAVE Print out debug information when saving */

/* Included headerfiles */
#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"

#ifdef _WINDOWS
#include "resource.h"
static CHAR         currentDocToSave[MAX_LENGTH];
static CHAR         currentPathName[MAX_LENGTH];
extern HINSTANCE    hInstance;

#ifdef __STDC__
LRESULT CALLBACK GetSaveDlgProc (HWND, UINT, WPARAM, LPARAM);
#else  /* !__STDC__ */
LRESULT CALLBACK GetSaveDlgProc (HWND, UINT, WPARAM, LPARAM);
#endif /* __STDC__ */
#endif /* _WINDOWS */


static STRING       DefaultName;
static CHAR         StdDefaultName[] = "Overview.html";
static CHAR         tempSavedObject[MAX_LENGTH];
static int          URL_attr_tab[] = {
   HTML_ATTR_HREF_,
   HTML_ATTR_codebase,
   HTML_ATTR_Script_URL,
   HTML_ATTR_SRC,
   HTML_ATTR_background_,
   HTML_ATTR_Style_,
   HTML_ATTR_cite
};
static boolean      TextFormat;
static int          SRC_attr_tab[] = {
   HTML_ATTR_SRC,
   HTML_ATTR_background_,
   HTML_ATTR_Style_
};
static STRING       QuotedText;

#include "AHTURLTools_f.h"
#include "EDITimage_f.h"
#include "HTMLbook_f.h"
#include "HTMLimage_f.h"
#include "HTMLsave_f.h"
#include "init_f.h"
#if !defined(AMAYA_JAVA) && !defined(AMAYA_ILU)
#include "query_f.h"
#endif
#include "styleparser_f.h"


#ifdef _WINDOWS
#include "wininclude.h"

/*-----------------------------------------------------------------------
 CreateGetSaveDlgWindow
 ------------------------------------------------------------------------*/
#ifdef __STDC__
void CreateGetSaveDlgWindow (HWND parent, STRING path_name)
#else  /* !__STDC__ */
void CreateGetSaveDlgWindow (parent, path_name)
HWND  parent;
STRING path_name;
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
  static CHAR txt [500];
  
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
	  EndDialog (hwnDlg, IDCANCEL);
	  ThotCallback (BaseDialog + SaveForm, INTEGER_DATA, (STRING) 0);
	  break;

	case ID_CONFIRM:
	  /* TODO: Extract directory and file name from urlToOpen */
	  EndDialog (hwnDlg, ID_CONFIRM);
	  TtaExtractName (currentDocToSave, SavePath, ObjectName);
	  ThotCallback (BaseDialog + SaveForm, INTEGER_DATA, (STRING) 1);
	  break;
	}
      break;
      
    default: return FALSE;
    }
  return TRUE ;
}
#endif /* _WINDOWS */

/*----------------------------------------------------------------------
  CheckGenerator                                                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             CheckGenerator (NotifyElement * event)
#else  /* __STDC__ */
boolean             CheckGenerator (event)
NotifyElement      *event;

#endif /* __STDC__ */
{
  AttributeType      attrType;
  Attribute          attr;
  CHAR               buff[MAX_LENGTH];
  STRING             ptr;
  int                length;

  attrType.AttrSSchema = TtaGetDocumentSSchema (event->document);
  attrType.AttrTypeNum = HTML_ATTR_meta_name;
  attr = TtaGetAttribute (event->element, attrType);
  if (attr != 0)
    {
      length = MAX_LENGTH - 1;
      TtaGiveTextAttributeValue (attr, buff, &length);
      if (!ustrcasecmp (buff, "GENERATOR"))
	{
	  /* is it Amaya generator ? */
	  attrType.AttrTypeNum = HTML_ATTR_meta_content;
	  attr = TtaGetAttribute (event->element, attrType);
	  if (attr != 0)
	    {
	      length = MAX_LENGTH - 1;
	      TtaGiveTextAttributeValue (attr, buff, &length);
	      ptr = ustrstr (buff, "amaya");
	      if (ptr == NULL)
		ptr = ustrstr (buff, "Amaya");
	      if (ptr == NULL)
		/* it's not a pure Amaya document -> remove the meta element */
		return TRUE;
	      else
		{
		  /* update the version */
		  ustrcpy (buff, HTAppName);
		  ustrcat (buff, " ");
		  ustrcat (buff, HTAppVersion);
		  TtaSetAttributeText (attr, buff, event->element, event->document);
		}
	    }
	}
    }
  return FALSE;  /* let Thot perform normal operation */
}


/*----------------------------------------------------------------------
  GenerateQuoteBefore                                                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             GenerateQuoteBefore (NotifyAttribute * event)
#else  /* __STDC__ */
boolean             GenerateQuoteBefore (event)
NotifyAttribute    *event;

#endif /* __STDC__ */
{
  STRING            ptr;
  int               length;

  length = TtaGetTextAttributeLength (event->attribute);
  QuotedText = TtaGetMemory (length + 3);
  QuotedText[length+1]= EOS;
  QuotedText[length+2]= EOS;
  TtaGiveTextAttributeValue (event->attribute, &QuotedText[1], &length);
  ptr = &QuotedText[1];
  while (*ptr != '"' && *ptr != '\'' && *ptr != EOS)
    ptr++;
  if (*ptr == '\'' || *ptr == EOS)
    {
      /* add double quotes before and after the text */
      QuotedText[0] = '"';
      QuotedText[length+1] = '"';
    }
  else
    {
      /* add simple quotes before and after the text */
      QuotedText[0] = '\'';
      QuotedText[length+1] = '\'';
    }
  TtaSetAttributeText (event->attribute, QuotedText, event->element, event->document);
  /* do NOT free the QuotedText string because it's used by GenerateQuoteAfter */
  return FALSE;  /* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
  GenerateQuoteAfter                                                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GenerateQuoteAfter (NotifyAttribute * event)
#else  /* __STDC__ */
void                GenerateQuoteAfter (event)
NotifyAttribute    *event;

#endif /* __STDC__ */
{
  /* remove quotes before and after the text */
  QuotedText[ustrlen (QuotedText) - 1] = EOS;
  TtaSetAttributeText (event->attribute, &QuotedText[1], event->element, event->document);
  TtaFreeMemory (QuotedText);
}

/*----------------------------------------------------------------------
   SetRelativeURLs: try to make relative URLs within an HTML document.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetRelativeURLs (Document document, STRING newpath)
#else
void                SetRelativeURLs (document, newpath)
Document            document;
STRING              newpath;
#endif
{
  Element             el, root, content;
  ElementType         elType;
  Attribute           attr;
  AttributeType       attrType;
  Language            lang;
  CHAR                old_url[MAX_LENGTH];
  CHAR                oldpath[MAX_LENGTH];
  CHAR                tempname[MAX_LENGTH];
  STRING              new_url;
  int                 index, max;
  int                 len;

#ifdef AMAYA_DEBUG
  fprintf(stderr, "SetRelativeURLs\n");
#endif

  root = TtaGetMainRoot (document);
  /* handle style elements */
  elType = TtaGetElementType (root);
  attrType.AttrSSchema = elType.ElSSchema;
  elType.ElTypeNum = HTML_EL_STYLE_;
  el = TtaSearchTypedElement (elType, SearchInTree, root);
  while (el)
    {
    if (elType.ElTypeNum == HTML_EL_STYLE_)
       content = TtaGetFirstChild (el);
    else
       content = NULL;
    if (content != NULL)
      {
      len = MAX_CSS_LENGTH;
      TtaGiveTextContent (content, CSSbuffer, &len, &lang);
      CSSbuffer[MAX_CSS_LENGTH] = EOS;
      new_url = UpdateCSSBackgroundImage (DocumentURLs[document], newpath,
					  NULL, CSSbuffer);
      if (new_url != NULL)
	{
	  /* register the modification to be able to undo it */
	  TtaRegisterElementReplace (content, document);
	  TtaSetTextContent (content, new_url, lang, document);
	  TtaFreeMemory (new_url);
	}
      }
    TtaNextSibling (&el);
    if (el != NULL)
       elType = TtaGetElementType (el);
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
		      /* register the modification to be able to undo it */
		      TtaRegisterAttributeReplace (attr, el, document);
		      TtaSetAttributeText (attr, new_url, el, document);
		      TtaFreeMemory (new_url);
		    }
		}
	      /* save the new attribute value */
	      else if (old_url[0] != '#')
		{
		  NormalizeURL (old_url, document, oldpath, tempname, NULL);
		  new_url = MakeRelativeURL (oldpath, newpath);
#ifdef AMAYA_DEBUG
		  fprintf(stderr, "Changed URL from %s to %s\n", old_url, new_url);
#endif
		  /* register the modification to be able to undo it */
		  TtaRegisterAttributeReplace (attr, el, document);
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
static void         InitSaveForm (Document document, View view, STRING pathname)
#else
static void         InitSaveForm (document, view, pathname)
Document            document;
View                view;
STRING              pathname;

#endif
{
#  ifndef _WINDOWS
   CHAR             buffer[3000];
   CHAR             s[MAX_LENGTH];
   int              i;

   /* Dialogue form for saving a document */
   i = 0;
   ustrcpy (&s[i], TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
   i += ustrlen (&s[i]) + 1;
   ustrcpy (&s[i], TtaGetMessage (AMAYA, AM_CLEAR));
   i += ustrlen (&s[i]) + 1;
   ustrcpy (&s[i], TtaGetMessage (AMAYA, AM_PARSE));
   TtaNewSheet (BaseDialog + SaveForm, TtaGetViewFrame (document, view), 
		TtaGetMessage (AMAYA, AM_SAVE_AS), 3, s, TRUE, 3, 'L', D_CANCEL);

   /* choice between html, xhtml and text */
   sprintf (buffer, "%s%c%s%c%s%c%s%cB%s%cB%s", "BHTML", EOS, "BXHTML", EOS, "BText", EOS, "S", EOS,
	    TtaGetMessage (AMAYA, AM_BCOPY_IMAGES), EOS,
	    TtaGetMessage (AMAYA, AM_BTRANSFORM_URL));
   TtaNewToggleMenu (BaseDialog + ToggleSave, BaseDialog + SaveForm,
		     TtaGetMessage (LIB, TMSG_DOCUMENT_FORMAT), 6, buffer, NULL, TRUE);
   if (TextFormat)
     {
       SaveAsHTML = FALSE;
       SaveAsXHTML = FALSE;
       SaveAsText = TRUE;
     }
   else if (IsXMLName (pathname))
     {
       SaveAsHTML = FALSE;
       SaveAsXHTML = TRUE;
       SaveAsText = FALSE;
     }
   else
     {
       SaveAsHTML = TRUE;
       SaveAsXHTML = FALSE;
       SaveAsText = FALSE;
     }
   TtaSetToggleMenu (BaseDialog + ToggleSave, 0, SaveAsHTML);
   TtaSetToggleMenu (BaseDialog + ToggleSave, 1, SaveAsXHTML);
   TtaSetToggleMenu (BaseDialog + ToggleSave, 2, SaveAsText);
   TtaSetToggleMenu (BaseDialog + ToggleSave, 4, CopyImages);
   TtaSetToggleMenu (BaseDialog + ToggleSave, 5, UpdateURLs);
   TtaListDirectory (SavePath, BaseDialog + SaveForm,
		     TtaGetMessage (LIB, TMSG_DOC_DIR),
		     BaseDialog + DirSave, ScanFilter,
		     TtaGetMessage (AMAYA, AM_FILES), BaseDialog + DocSave);
   /* second line */
   TtaNewTextForm (BaseDialog + NameSave, BaseDialog + SaveForm,
		   TtaGetMessage (AMAYA, AM_DOC_LOCATION), 50, 1, TRUE);
   TtaSetTextForm (BaseDialog + NameSave, pathname);
   TtaNewLabel (BaseDialog + Label1, BaseDialog + SaveForm, "");
   TtaNewLabel (BaseDialog + Label2, BaseDialog + SaveForm, "");
   /* third line */
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
   if (TextFormat)
     {
       TtaRedrawMenuEntry (BaseDialog + ToggleSave, 0, NULL, -1, FALSE);
       TtaRedrawMenuEntry (BaseDialog + ToggleSave, 1, NULL, -1, FALSE);
       TtaRedrawMenuEntry (BaseDialog + ToggleSave, 4, NULL, -1, FALSE);
       TtaRedrawMenuEntry (BaseDialog + ToggleSave, 5, NULL, -1, FALSE);
     }
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                InitSaveObjectForm (Document document, View view, STRING object, STRING pathname)
#else
void                InitSaveObjectForm (document, view, object, pathname)
Document            document;
View                view;
STRING              object;
STRING              pathname;

#endif
{
#  ifndef _WINDOWS
   CHAR                tempdir[MAX_LENGTH];
#  endif /* _WINDOWS */

   if (SavingDocument != 0 || SavingObject != 0)
     return;
   SavingObject = document;
   ustrncpy (tempSavedObject, object, sizeof (tempSavedObject));

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
   CHAR                tempfile[MAX_LENGTH];
   CHAR                msg[MAX_LENGTH];
   boolean             dst_is_local;
   int                 res;

   dst_is_local = !IsW3Path (SavePath);

   ustrcpy (tempfile, SavePath);
   ustrcat (tempfile, DIR_STR);
   ustrcat (tempfile, ObjectName);


   if (!dst_is_local)
     {
#ifdef AMAYA_JAVA
	res = PutObjectWWW (SavingObject, tempSavedObject, tempfile,
			    unknown_type,
			    AMAYA_SYNC | AMAYA_NOCACHE | AMAYA_USE_PRECONDITIONS, NULL, NULL);
#else
	/* @@ We need to check the use of AMAYA_PREWRITE_VERIFY in this function*/
	res = PutObjectWWW (SavingObject, tempSavedObject, tempfile,
			    unknown_type,
			    AMAYA_SYNC | AMAYA_NOCACHE |  AMAYA_FLUSH_REQUEST 
			    | AMAYA_USE_PRECONDITIONS, NULL, NULL);
#endif /* AMAYA_JAVA */


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

/*----------------------------------------------------------------------
   SaveDocumentAs                                              
   Entry point called when the user selects the SaveAs function
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SaveDocumentAs (Document doc, View view)
#else
void                SaveDocumentAs (doc, view)
Document            doc;
View                view;

#endif
{
   CHAR             tempname[MAX_LENGTH];
   int              i;

   if ((SavingDocument != 0 && SavingDocument != doc) ||
       SavingObject != 0)
      return;

   TextFormat = (DocumentTypes[doc] == docText ||
		 DocumentTypes[doc] == docTextRO ||
		 DocumentTypes[doc] == docCSS ||
		 DocumentTypes[doc] == docCSSRO);

   /* memorize the current document */
   if (SavingDocument == 0)
     {
       SavingDocument = doc;
       ustrcpy (tempname, DocumentURLs[doc]);
       /* suppress compress suffixes from tempname */
       i = ustrlen (tempname) - 1;
       if (i > 2 && !ustrcmp (&tempname[i-2], ".gz"))
	 {
	   tempname[i-2] = EOS;
	   TtaFreeMemory (DocumentURLs[doc]);
	   DocumentURLs[doc] = (STRING) TtaStrdup (tempname);
	 }
       else if (i > 1 && !ustrcmp (&tempname[i-1], ".Z"))
	 {
	   tempname[i-1] = EOS;
	   TtaFreeMemory (DocumentURLs[doc]);
	   DocumentURLs[doc] = (STRING) TtaStrdup (tempname);
	 }

       /* if it is a Web document use the current SavePath */
       if (IsW3Path (tempname))
	 {
	     TtaExtractName (tempname, SavePath, SaveName);
	     if (SaveName[0] == EOS)
	       {
		 DefaultName = TtaGetEnvString ("DEFAULTNAME");
		 if (DefaultName == NULL || *DefaultName == EOS)
		   DefaultName = StdDefaultName;
		 ustrcpy (SaveName, DefaultName);
		 ustrcat (tempname, SaveName);
	       }

	   /* add the suffix .html for HTML documents */
	   if (!TextFormat &&
	       !IsHTMLName (SaveName) &&
	       !IsXMLName (SaveName))
	     {
	       ustrcat (SaveName, ".html");
	       ustrcpy (tempname, SavePath);
	       ustrcat (tempname, URL_STR);
	       ustrcat (tempname, SaveName);
 	     }
	 }
       else
	 {
	   TtaGetDocumentDirectory (doc, tempname, MAX_LENGTH);
	   ustrcpy (SavePath, tempname);
	   ustrcpy (SaveName, TtaGetDocumentName (doc));
	   ustrcat (tempname, DIR_STR);
	   /* add the suffix .html for HTML documents */
	   if (!TextFormat &&
	       !IsHTMLName (SaveName) &&
	       !IsXMLName (SaveName))
	     ustrcat (SaveName, ".html");
	   ustrcat (tempname, SaveName);
	 }
       TtaSetDialoguePosition ();
     }
   else
     {
       ustrcpy (tempname, SavePath);
       ustrcat (tempname, DIR_STR);
       ustrcat (tempname, SaveName);
     }

   /* display the dialog box */
   InitSaveForm (doc, 1, tempname);
}

/*----------------------------------------------------------------------
   SetNamespacesAndDTD
   Set the content of the Namespaces attribute (on the root element)
   according to the SSchemas used in the document.
   Set the HtmlDTD attribute to Frameset if the document uses Frames.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         SetNamespacesAndDTD (Document doc)
#else
static void         SetNamespacesAndDTD (doc)
Document            doc;

#endif
{
   Element		root, el;
   ElementType		elType;
   AttributeType	attrType;
   Attribute		attr;
   CHAR			buffer[200];
   boolean		useMathML, useGraphML, useFrames;

   useMathML = FALSE;
   useGraphML = FALSE;
   if (TtaGetSSchema ("MathML", doc) != NULL)
      useMathML = TRUE;
   if (TtaGetSSchema ("GraphML", doc) != NULL)
      useGraphML = TRUE;

   root = TtaGetMainRoot (doc);
   attrType.AttrSSchema = TtaGetSSchema ("HTML", doc);
   attrType.AttrTypeNum = HTML_ATTR_Namespaces;
   attr = TtaGetAttribute (root, attrType);
   if (!useMathML && !useGraphML && attr)
      /* delete the Namespaces attribute */
      TtaRemoveAttribute (root, attr, doc);
   else if (useMathML || useGraphML)
      {
      /* prepare the value of attribute Namespaces */
      buffer[0] = '\0';
      if (useMathML)
	 {
	 ustrcat (buffer, "\n      xmlns:m=\"http://www.w3.org/TR/REC-MathML/\"");
	 }
      if (useGraphML)
	 {
	 ustrcat (buffer, "\n      xmlns:g=\"http://www.w3.org/Graphics/SVG/Amaya2D\"");
	 }
      /* set the value of attribute Namespaces */
      if (attr == NULL)
	 {
	 attr = TtaNewAttribute (attrType);
	 TtaAttachAttribute (root, attr, doc);
	 }
      TtaSetAttributeText (attr, buffer, root, doc);
      }
   /* looks for a FRAMESET element and set attribute HtmlDTD */
   useFrames = FALSE;
   el = TtaGetFirstChild (root);
   while (el && !useFrames)
      {
      elType = TtaGetElementType (el);
      if (elType.ElSSchema == attrType.AttrSSchema &&
	  elType.ElTypeNum == HTML_EL_FRAMESET)
	 useFrames = TRUE;
      else
         TtaNextSibling (&el);
      }
   attrType.AttrTypeNum = HTML_ATTR_HtmlDTD;
   attr = TtaGetAttribute (root, attrType);
   if (!attr)
      {
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (root, attr, doc);
      }
   if (useFrames)
      TtaSetAttributeValue (attr, HTML_ATTR_HtmlDTD_VAL_Frameset, root, doc);
   else
      TtaSetAttributeValue (attr, HTML_ATTR_HtmlDTD_VAL_Transitional, root, doc);
}

/*----------------------------------------------------------------------
   SaveDocumentLocally save the document in a local file.
   Return TRUE if the document has been saved
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean    SaveDocumentLocally (Document doc, STRING directoryName, STRING documentName)
#else
static boolean    SaveDocumentLocally (doc, directoryName, documentName)
Document          doc;
STRING            directoryName;
STRING            documentName;

#endif
{
  DisplayMode         dispMode;
  CHAR                tempname[MAX_LENGTH];
  CHAR                docname[100];
  boolean             ok;

#ifdef AMAYA_DEBUG
  fprintf(stderr, "SaveDocumentLocally :  %s / %s\n", directoryName, documentName);
#endif

  ustrcpy (tempname, directoryName);
  ustrcat (tempname, DIR_STR);
  ustrcat (tempname, documentName);
  /* suspend the redisplay due to the temporary update of attributes
     STYLE and META-CONTENT */
  dispMode = TtaGetDisplayMode (doc);
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, DeferredDisplay);
  if (SaveAsText) 
    {
      SetInternalLinks (doc);
      ok = TtaExportDocument (doc, tempname, "HTMLTT");
    }
  else
    {
      SetNamespacesAndDTD (doc);
      if (SaveAsXHTML)
	ok = TtaExportDocument (doc, tempname, "HTMLTX");
      else
	ok = TtaExportDocument (doc, tempname, "HTMLT");
      if (ok)
	{
	  TtaSetDocumentDirectory (doc, directoryName);
	  /**********/
	  ustrcpy (docname, documentName);
	  ExtractSuffix (docname, tempname);
	  /* Change the document name in all views */
	  TtaSetDocumentName (doc, docname);
	}
    }
  /* retore the redisplay */
  TtaSetDisplayMode (doc, dispMode);
  return (ok);
}

/*----------------------------------------------------------------------
  AddNoName
  This function is called whenether one tries to save a document
  without name (just the directory path e.g. http://www.w3.org/pub/WWW/ )
  It ask the user whether an extra name suffix should be added or
  abort.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean     AddNoName (Document document, View view, STRING url, boolean *ok)
#else
static boolean     AddNoName (document, view, url, ok)
Document           document;
View               view;
STRING             url;
boolean           *ok;
#endif
{
   CHAR            msg[MAX_LENGTH];
   CHAR            documentname[MAX_LENGTH];
   int             len;

  len = ustrlen (url);
  TtaExtractName (url, msg, documentname);
  *ok = (documentname[0] != EOS);
  if (*ok)
    return (FALSE);
  else
    {
      /* the name is not correct for the put operation */
      ustrcpy (msg, TtaGetMessage(AMAYA, AM_NO_NAME));
      ustrcat (msg, url);
      if (IsW3Path (url))
	{
	  if (url[len -1] != URL_SEP)
	    ustrcat (msg, URL_STR);
	}
      else if (url[len -1] != DIR_SEP)
	ustrcat (msg, DIR_STR);
      /* get default name */
      DefaultName = TtaGetEnvString ("DEFAULTNAME");
      if (DefaultName == NULL || *DefaultName == EOS)
	DefaultName = StdDefaultName;

      ustrcat (msg, DefaultName);
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
  Return 0 if the file has been saved
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          SafeSaveFileThroughNet (Document doc, STRING localfile,
                          STRING remotefile, PicType filetype,
			  boolean use_preconditions)
#else
static int          SafeSaveFileThroughNet (doc, localfile, remotefile, filetype,
					    use_preconditions)
Document            doc;
STRING              localfile;
STRING              remotefile;
PicType             filetype;
boolean             use_preconditions;
#endif
{
  CHAR              msg[MAX_LENGTH];
  CHAR              tempfile[MAX_LENGTH]; /* File name used to refetch */
  CHAR              tempURL[MAX_LENGTH];  /* May be redirected */
  STRING            verify_publish;
  int               res;
  int               mode = 0;

  verify_publish = TtaGetEnvString("VERIFY_PUBLISH");
  /* verify the PUT by default */
  if (verify_publish == NULL)
    verify_publish = "yes";
  
  
#ifdef AMAYA_DEBUG
  fprintf(stderr, "SafeSaveFileThroughNet :  %s to %s type %d\n", localfile, remotefile, filetype);
#endif

  /* Save */
  /* JK: SYNC requests assume that the remotefile name is a static array */
  ustrcpy (tempfile, remotefile);
#ifdef AMAYA_JAVA
  mode = AMAYA_SYNC | AMAYA_NOCACHE;
#else /* AMAYA_JAVA */
  mode = AMAYA_SYNC | AMAYA_NOCACHE | AMAYA_FLUSH_REQUEST;
  mode = mode | ((use_preconditions) ? AMAYA_USE_PRECONDITIONS : 0);
#endif /* AMAYA_JAVA */

  res = PutObjectWWW (doc, localfile, tempfile, mode, filetype, NULL, NULL);
  if (res != 0)
    /* The HTTP PUT method failed ! */
    return (res);
  /* does the user want to verify the PUT? */
  if (!verify_publish || !*verify_publish
      || ustrcmp (verify_publish, "yes"))
    return (0);

  /* Refetch */
#ifdef AMAYA_DEBUG
  fprintf(stderr, "SafeSaveFileThroughNet :  refetch %s \n", remotefile);
#endif

  TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_VERIFYING), NULL);
  ustrcpy (tempURL, remotefile);
#ifdef AMAYA_JAVA
  res = GetObjectWWW (doc, tempURL, NULL, tempfile, AMAYA_SYNC | AMAYA_NOCACHE,
		      NULL, NULL, NULL, NULL, NO, NULL);
#else /* AMAYA_JAVA */
  res = GetObjectWWW (doc, tempURL, NULL, tempfile, AMAYA_SYNC | AMAYA_NOCACHE
		      | AMAYA_FLUSH_REQUEST, NULL, NULL, NULL, NULL, NO, NULL);
#endif /* AMAYA_JAVA */
  if (res != 0)
    {
      /* The HTTP GET method failed ! */
      sprintf (msg, TtaGetMessage (AMAYA, AM_SAVE_RELOAD_FAILED), remotefile);
      InitConfirm (doc, 1, msg);
      if (UserAnswer)
	/* Ignore the read failure */
	res = 0;
    }

#if 0
  /* Removed this test as libwww already asks the user to confirm the
     redirection */
  else if (ustrcmp (remotefile, tempURL))
    {
      /* Warning : redirect... */
      sprintf (msg, TtaGetMessage (AMAYA, AM_SAVE_REDIRECTED), remotefile, tempURL);
      InitConfirm (doc, 1, msg);
      if (!UserAnswer)
	/* Trigger the error */
	res = -1;
    }
#endif

  if (res == 0)
    {
      /* Compare content. */
#ifdef AMAYA_DEBUG
      fprintf(stderr, "SafeSaveFileThroughNet :  compare %s and %s \n", remotefile, localfile);
#endif
#ifdef AMAYA_JAVA
#endif /* AMAYA_JAVA */
      if (! TtaCompareFiles(tempfile, localfile))
	{
	  sprintf (msg, TtaGetMessage (AMAYA, AM_SAVE_COMPARE_FAILED), remotefile);
	  InitConfirm (doc, 1, msg);
	  if (!UserAnswer)
	    res = -1;
	}
#ifdef AMAYA_JAVA
#endif /* AMAYA_JAVA */
    }

  if (TtaFileExist (tempfile))
    TtaFileUnlink (tempfile);
  return(res);
}

/*----------------------------------------------------------------------
  SaveObjectThroughNet
  Save a simple file to a remote network location.
  confirm = TRUE form SAVE_AS and FALSE from SAVE
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean      SaveObjectThroughNet (Document document, View view,
					  STRING url, boolean confirm,
					  boolean use_preconditions)
#else
static boolean      SaveObjectThroughNet (document, view, url, confirm,
					  use_preconditions)
Document            document;
View                view;
STRING              url;
boolean             confirm;
boolean             use_preconditions;
#endif
{
  LoadedImageDesc *pImage;
  STRING           tempname;
  STRING           msg;
  int              remainder = 500;
  int              index = 0, len, nb = 0;
  int              imageType, res;

  msg = TtaGetMemory(remainder);
  if (msg == NULL)
    return (FALSE);

  /*
   * Don't use memory allocated on the stack ! May overflow the 
   * memory allocated for this Java thread.
   */
  /* save into the temporary document file */
  tempname = GetLocalPath (document, url);

  /* build the output */
  TtaExportDocument (document, tempname, "TextFileT");

  ActiveTransfer (document);
  TtaHandlePendingEvents ();
  res = SafeSaveFileThroughNet (document, tempname,
				url, unknown_type, 
				use_preconditions);
  if (res != 0)
    {
#if !defined(AMAYA_JAVA) && !defined(AMAYA_ILU)
      DocNetworkStatus[document] |= AMAYA_NET_ERROR;
#endif /* AMAYA_JAVA || AMAYA_ILU */
      ResetStop (document);
      if (confirm)
	{
#if defined(AMAYA_JAVA) || defined(AMAYA_ILU)
	  sprintf (msg, "%s %s \n%s",
		   TtaGetMessage (AMAYA, AM_URL_SAVE_FAILED),
		   url,
		   TtaGetMessage (AMAYA, AM_SAVE_DISK));
#else /* AMAYA_JAVA || AMAYA_ILU */
	  sprintf (msg, "%s %s \n%s\n%s",
		   TtaGetMessage (AMAYA, AM_URL_SAVE_FAILED),
		   url,
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
	res = -1;
    }
  else
    {
      ResetStop (document);
#ifdef AMAYA_DEBUG
      fprintf(stderr, "Saving completed\n");
#endif
    }
  TtaFreeMemory (msg);
  return (res == 0);
}

/*----------------------------------------------------------------------
  SaveDocumentThroughNet
  Save a document and the included images to a remote network location.
  confirm = TRUE form SAVE_AS and FALSE from SAVE
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean   SaveDocumentThroughNet (Document doc, View view, STRING url,
					 boolean confirm, boolean with_images,
					 boolean use_preconditions)
#else
static boolean   SaveDocumentThroughNet (doc, view, url, confirm,
                                         with_images, use_preconditions)
Document         doc;
View             view;
STRING           url;
boolean          confirm;
boolean          with_images;
boolean          use_preconditions;
#endif
{
  LoadedImageDesc *pImage;
  STRING           tempname;
  STRING           msg;
  int              remainder = 10000;
  int              index = 0, len, nb = 0;
  int              imageType, res;

  msg = TtaGetMemory(remainder);
  if (msg == NULL)
    return (FALSE);

  /*
   * Don't use memory allocated on the stack ! May overflow the 
   * memory allocated for this Java thread.
   */
  /* save into the temporary document file */
  tempname = GetLocalPath (doc, url);

  /* First step : build the output and ask for confirmation */
  SetNamespacesAndDTD (doc);
  if (SaveAsXHTML)
    TtaExportDocument (doc, tempname, "HTMLTX");
  else
    TtaExportDocument (doc, tempname, "HTMLT");
  res = 0;
  if (confirm && with_images)
    {
#ifndef _WINDOWS
      TtaNewForm (BaseDialog + ConfirmSave, TtaGetViewFrame (doc, view), 
		  TtaGetMessage (LIB, TMSG_LIB_CONFIRM),
		  TRUE, 1, 'L', D_CANCEL);
      TtaNewLabel (BaseDialog + Label1, BaseDialog + ConfirmSave,
		   TtaGetMessage (AMAYA, AM_WARNING_SAVE_OVERWRITE));
#endif /* _WINDOWS */
       
      ustrcpy (&msg[index], url);
      len = ustrlen (url);
      len++;
      remainder -= len;
      index += len;
      nb++;

      pImage = ImageURLs;
      while (pImage != NULL)
	{
	  if (pImage->document == doc && pImage->status == IMAGE_MODIFIED)
	    {
	      if (nb > 30)
		{
		  ustrcpy (&msg[index], "...");
		  len = ustrlen ("...");
		  len++;
		  remainder -= len;
		  index += len;
		  nb++;
		  break;
		}
	      ustrcpy (&msg[index], pImage->originalName);
	      len = ustrlen (pImage->originalName);
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
      CreateSaveListDlgWindow (TtaGetViewFrame (doc, view), nb, msg, BaseDialog, ConfirmSave);
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
      ActiveTransfer (doc);
      TtaHandlePendingEvents ();
      pImage = NULL;

      res = SafeSaveFileThroughNet (doc, tempname,
				    url, unknown_type, 
				    use_preconditions);
      if (res != 0)
	{
#if !defined(AMAYA_JAVA) && !defined(AMAYA_ILU)
	  DocNetworkStatus[doc] |= AMAYA_NET_ERROR;
#endif /* AMAYA_JAVA || AMAYA_ILU */
	  ResetStop (doc);
	  if (confirm)
	    {
#if defined(AMAYA_JAVA) || defined(AMAYA_ILU)
	      sprintf (msg, "%s %s",
		       TtaGetMessage (AMAYA, AM_URL_SAVE_FAILED),
		       url);
#else /* AMAYA_JAVA || AMAYA_ILU */
	      sprintf (msg, "%s %s \n%s",
		       TtaGetMessage (AMAYA, AM_URL_SAVE_FAILED),
		       url,
		       AmayaLastHTTPErrorMsg);
#endif /* AMAYA_JAVA || AMAYA_ILU */
	      InitConfirm (doc, view, msg);
	      /* JK: to erase the last status message */
	      TtaSetStatus (doc, view, "", NULL);	       
	      if (UserAnswer)
		res = -1;
	      else
		res = -1;
	    }
	  else
	    res = -1;
	}
      else if (with_images)
	pImage = ImageURLs;

      while (pImage != NULL)
	{
	  if (pImage->document == doc && pImage->status == IMAGE_MODIFIED)
	    {
	      imageType = pImage->imageType;
	      res = SafeSaveFileThroughNet(doc, pImage->localName,
					   pImage->originalName, imageType,
					   use_preconditions);
	      if (res)
		{
#if !defined(AMAYA_JAVA) && !defined(AMAYA_ILU)
		  DocNetworkStatus[doc] |= AMAYA_NET_ERROR;
#endif /* AMAYA_JAVA  || AMAYA_ILU */
		  ResetStop (doc);
#if defined(AMAYA_JAVA) || defined(AMAYA_ILU)
		      sprintf (msg, "%s %s",
			       TtaGetMessage (AMAYA, AM_URL_SAVE_FAILED),
			       pImage->originalName);
#else /* AMAYA_JAVA || AMAYA_ILU */
		      sprintf (msg, "%s %s \n%s",
			       TtaGetMessage (AMAYA, AM_URL_SAVE_FAILED),
			       pImage->originalName, 
			       AmayaLastHTTPErrorMsg);
#endif /* AMAYA_JAVA || AMAYA_ILU */
		      InitConfirm (doc, view, msg);
		      /* erase the last status message */
		      TtaSetStatus (doc, view, "", NULL);
		      if (UserAnswer)
			res = -1;
		      else
			res = -1;
		  /* do not continue */
		  pImage = NULL;
		}
	      else
		pImage->status = IMAGE_LOADED;
	    }

	  if (pImage != NULL)
	    pImage = pImage->nextImage;
	}
      ResetStop (doc);
    }

#ifdef AMAYA_DEBUG
  fprintf(stderr, "Saving completed\n");
#endif
  TtaFreeMemory (msg);
  if (tempname)
    TtaFreeMemory (tempname);
  return (res == 0);
}

/*----------------------------------------------------------------------
  SaveDocument
  Entry point called when the user selects the Save menu entry or
  presses the Save button.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SaveDocument (Document doc, View view)
#else
void                SaveDocument (doc, view)
Document            doc;
View                view;

#endif
{
  CHAR                tempname[MAX_LENGTH];
  int                 i, res;
  boolean             ok;

  if (SavingDocument != 0 || SavingObject != 0)
    return;
  else if (!TtaGetDocumentAccessMode (doc))
    /* the document is in ReadOnly mode */
    return;
  else if (!TtaIsDocumentModified (doc))
    {
      TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_NOTHING_TO_SAVE), "");
      return;
    }
  TextFormat = (DocumentTypes[doc] == docText ||
		DocumentTypes[doc] == docTextRO ||
		DocumentTypes[doc] == docCSS ||
		DocumentTypes[doc] == docCSSRO);
  SavingDocument = doc;

  ok = FALSE;
  /* attempt to save through network if possible */
  ustrcpy (tempname, DocumentURLs[doc]);
  /* suppress compress suffixes from tempname */
  i = ustrlen (tempname) - 1;
  if (i > 2 && !ustrcmp (&tempname[i-2], ".gz"))
    {
      tempname[i-2] = EOS;
      TtaFreeMemory (DocumentURLs[doc]);
      DocumentURLs[doc] = (STRING) TtaStrdup (tempname);
    }
  else if (i > 1 && !ustrcmp (&tempname[i-1], ".Z"))
    {
      tempname[i-1] = EOS;
      TtaFreeMemory (DocumentURLs[doc]);
      DocumentURLs[doc] = (STRING) TtaStrdup (tempname);
    }

#ifdef AMAYA_DEBUG
  fprintf(stderr, "SaveDocument : %d to %s\n", doc, tempname);
#endif

  /* the suffix fixes the output format of HTML saved document */
  SaveAsXHTML = IsXMLName (tempname);
  if (IsW3Path (tempname))
    {
      if (AddNoName (doc, view, tempname, &ok))
	{
	  ok = TRUE;
	  /* need to update the document url */
	  res = ustrlen(tempname) - 1;
	  if (tempname[res] != URL_SEP)
	    ustrcat (tempname, URL_STR);
	  ustrcat (tempname, DefaultName);
	  TtaFreeMemory (DocumentURLs[doc]);
	  DocumentURLs[doc] = (STRING) TtaStrdup (tempname);
	}

      /* it's a complete name: save it */
      if (ok)
	if (TextFormat)
	  ok = SaveObjectThroughNet (doc, view, DocumentURLs[doc], FALSE, TRUE);
	else
	  ok = SaveDocumentThroughNet (doc, view, DocumentURLs[doc], FALSE, TRUE, TRUE);
    }
  else
    {
      if (TextFormat)
	ok = TtaExportDocument (doc, tempname, "TextFileT");
      else
	{
	  SetNamespacesAndDTD (doc);
	  if (SaveAsXHTML)
	    ok = TtaExportDocument (doc, tempname, "HTMLTX");
	  else
	    ok = TtaExportDocument (doc, tempname, "HTMLT");
	}
    }

  SavingDocument = 0;
  if (ok)
    {
      TtaSetDocumentUnmodified (doc);
      TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_SAVED), DocumentURLs[doc]);
    }
  else
    /* cannot save */
    TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_CANNOT_SAVE), DocumentURLs[doc]);
}


/*----------------------------------------------------------------------
  BackupAll save all opened documents when the application crashes
  ----------------------------------------------------------------------*/
void                   BackUpDocs ()
{
  Document             doc;
  FILE                *f;
  CHAR                 pathname[MAX_LENGTH];
  CHAR                 docname[MAX_LENGTH];
  STRING               ptr;
  int                  l;

  /* check all modified documents */
  f = NULL;
  for (doc = 1; doc < DocumentTableLength; doc++)
    if (DocumentURLs[doc] != NULL && TtaIsDocumentModified (doc) && doc != W3Loading)
      {
	if (f == NULL)
	  {
	    /* open the crash file */
	    sprintf (pathname, "%s%cCrash.amaya", TempFileDirectory, DIR_SEP);
	    f = fopen (pathname, "w");
	    if (f == NULL)
	      return;
	  }

	/* generate the backup file name */
        SavingDocument = 0;
	ptr = DocumentURLs[doc];
	l = ustrlen (ptr) - 1;
	if (IsW3Path (ptr) &&  ptr[l] == URL_SEP)
	  {
	    /* it's a directory name */
	    ptr[l] = EOS;
	    TtaExtractName (DocumentURLs[doc], pathname, docname);
	    ptr[l] = URL_SEP;
	    l = 0;
	  }
	else
	  TtaExtractName (DocumentURLs[doc], pathname, docname);
	if (l == 0)
	  sprintf (pathname, "%s%c%s.html", TempFileDirectory, DIR_SEP, docname);
	else
	  sprintf (pathname, "%s%c%s", TempFileDirectory, DIR_SEP, docname);

	/* write the backup file */
        DocumentURLs[doc] = pathname;
        SaveDocument (doc, 1);
	/* register the backup file name and the original document name */
	fprintf (f, "%s %s %d\n", pathname, ptr, DocumentTypes[doc]);
	TtaFreeMemory (ptr);
      }
  /* now close the crash file */
  if (f != NULL)
    fclose (f);
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
   return TRUE;	/* prevent Thot from performing normal save operation */
}


/*----------------------------------------------------------------------
  UpdateImages
  if CopyImage is TRUE change all picture SRC attribute and CSS background
  images.
  If pictures are saved locally, make the copy
  else add them to the list of remote images to be copied.
  The parameter imgbase gives the relative path of the new image directory.
  The parameter newURL gives the new document URL (or local file).
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void            UpdateImages (Document doc, boolean src_is_local, boolean dst_is_local, STRING imgbase, STRING newURL)
#else
static void            UpdateImages (doc, src_is_local, dst_is_local, imgbase, newURL)
Document               doc;
boolean                src_is_local;
boolean                dst_is_local;
STRING                 imgbase;
STRING                 newURL;
#endif
{
   AttributeType       attrType;
   ElementType         elType;
   Attribute           attr;
   Element             el, root, content;
   LoadedImageDesc    *pImage;
   Language            lang;
   CHAR                tempfile[MAX_LENGTH];
   CHAR                localpath[MAX_LENGTH];
   CHAR                oldpath[MAX_LENGTH];
   CHAR                oldname[MAX_LENGTH];
   CHAR                tempname[MAX_LENGTH];
   CHAR                imgname[MAX_LENGTH];
   CHAR                url[MAX_LENGTH];
   STRING              buf, ptr;
   STRING              sStyle, stringStyle;
   STRING              oldStyle;
   int                 buflen, max, index;

   if (imgbase[0] != EOS)
     {
       /* add the separator if needed */
       buflen = ustrlen (imgbase) - 1;
       if (dst_is_local && !IsW3Path (imgbase))
	 {
	   if (imgbase[buflen] != DIR_SEP)
	     ustrcat (imgbase, DIR_STR);
	 }
       else
	 {
	   if (imgbase[buflen] != URL_SEP)
	     ustrcat (imgbase, URL_STR);
	 }
     }

   /* save the old document path to locate existing images */
   ustrcpy (oldpath, DocumentURLs[doc]);
   buflen = ustrlen (oldpath) - 1;
   if (oldpath[buflen] ==  '/')
     oldpath[buflen] = EOS;
   /* path to search image descriptors */
   sprintf (localpath, "%s%s%d%s", TempFileDirectory, DIR_STR, doc, DIR_STR);

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

       root = TtaGetMainRoot (doc);
       /* handle style elements */
       elType = TtaGetElementType (root);
       attrType.AttrSSchema = elType.ElSSchema;
       elType.ElTypeNum = HTML_EL_STYLE_;
       el = TtaSearchTypedElement (elType, SearchInTree, root);
       while (el)
	 {
	 if (elType.ElTypeNum == HTML_EL_STYLE_)
	    content = TtaGetFirstChild (el);
	 else
	    content = NULL;
         if (content != NULL)
	   {
	   buflen = MAX_CSS_LENGTH;
	   TtaGiveTextContent (content, CSSbuffer, &buflen, &lang);
	   CSSbuffer[MAX_CSS_LENGTH] = EOS;
	   url[0] = EOS;
	   tempname[0] = EOS;
	   sStyle = UpdateCSSBackgroundImage (oldpath, newURL, imgbase, CSSbuffer);
	   if (sStyle != NULL)
	     {
	       /* register the modification to be able to undo it */
	       TtaRegisterElementReplace (content, doc);
	       /* save this new style element string */
	       TtaSetTextContent (content, sStyle, lang, doc);

	       /* current point in sStyle */
	       stringStyle = sStyle;
	       /* extract the first URL from the new style string */
	       ptr = GetCSSBackgroundURL (stringStyle);
	       oldStyle = CSSbuffer;
	       while (ptr != NULL)
		 {
		   /* for next research */
		   stringStyle = ustrstr (stringStyle, "url") + 3;
		   ustrcpy (url, ptr);
		   TtaFreeMemory (ptr);
		   NormalizeURL (url, 0, tempname, imgname, newURL);

		   /* extract the URL from the old style string */
		   ptr = GetCSSBackgroundURL (oldStyle);
		   if (ptr != NULL)
		     {
		       /* for next research */
		       oldStyle = ustrstr (oldStyle, "url") + 3;
		       NormalizeURL (ptr, 0, oldname, imgname, oldpath);
		       TtaFreeMemory (ptr);

		       /*
			 At this point:
			 - url gives the relative new image name
			 - tempname gives the new image full name
			 - oldname gives the old image full name
			 - imgname contains the image file name
			 */
		       if (url[0] != EOS && oldname[0] != EOS)
			 {
			   if ((src_is_local) && (!dst_is_local))
			     /* add the existing localfile to images list to be saved */
			     AddLocalImage (oldname, imgname, tempname, doc, &pImage);
			   
			   /* mark the image descriptor or copy the file */
			   if (dst_is_local)
			     {
			       /* copy the file to the new location */
			       if (IsW3Path (oldname) || IsHTTPPath (oldpath))
				 {
				   /*
				     it was a remote image:
				     we use the local temporary name to do the copy
				     */
				   ustrcpy (oldname, localpath);
				   ustrcat (oldname, imgname);
				 }
			       
			       if (imgbase[0] != EOS)
				 {
				   ustrcpy (tempfile, imgbase);
				   ustrcat (tempfile, DIR_STR);
				   ustrcat (tempfile, imgname);
				 }
			       else
				 {
				   ustrcpy (tempfile, SavePath);
				   ustrcat (tempfile, DIR_STR);
				   ustrcat (tempfile, imgname);
				 }
			       
			       TtaFileCopy (oldname, tempfile);
			     }
			   else
			     {
			       /* save on a remote server */
			       if (IsW3Path (oldname) || IsHTTPPath (oldpath))
				 {
				   /*
				     it was a remote image:
				     get the image descriptor to prepare
				     the saving process
				     */
				   ustrcpy (tempfile, localpath);
				   ustrcat (tempfile, imgname);
				   pImage = SearchLoadedImage (tempfile, doc);
				   /* update the descriptor */
				   if (pImage)
				     {
				       /* image was already loaded */
				       if (pImage->originalName != NULL)
					 TtaFreeMemory (pImage->originalName);
				       pImage->originalName = (STRING) TtaStrdup (tempname);
				       if (TtaFileExist(pImage->localName))
					 pImage->status = IMAGE_MODIFIED;
				       else
					 pImage->status = IMAGE_NOT_LOADED;
				       /*pImage->elImage = (struct _ElemImage *) content;*/
				     }
				 }
			       else
				 AddLocalImage (oldname, imgname, tempname, doc, &pImage);
			     }
			 }
		     }
		   ptr = GetCSSBackgroundURL (stringStyle);
		 }
	       TtaFreeMemory (sStyle);
	     }
	   }
	 TtaNextSibling (&el);
	 if (el)
	    elType = TtaGetElementType (el);
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
		   attrType.AttrTypeNum == HTML_ATTR_Style_ ||
		   attrType.AttrTypeNum == HTML_ATTR_background_)
		 {
		   elType = TtaGetElementType (TtaGetParent(el));
		   if (elType.ElTypeNum != HTML_EL_Object)
		     {
		       buflen = MAX_LENGTH;
		       buf = (STRING) TtaGetMemory (buflen);
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
			       /* register the modification to be able to undo it */
			       TtaRegisterAttributeReplace (attr, el, doc);
			       /* save this new style attribute string */
			       TtaSetAttributeText (attr, ptr, el, doc);
			       ustrcpy (url, ptr);
			       TtaFreeMemory (ptr);
			       /* extract the URL from the new style string */
			       ptr = GetCSSBackgroundURL (url);
			       if (ptr != NULL)
				 {
				   ustrcpy (url, ptr);
				   TtaFreeMemory (ptr);
				   NormalizeURL (url, 0, tempname, imgname, newURL);
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
			   ustrcpy (url, buf);
			   NormalizeURL (url, 0, buf, imgname, oldpath);
			   /* save the new SRC attr value */
			   if (imgbase[0] != EOS)
			     {
			       /* compose the relative or absolute name */
			       ustrcpy (url, imgbase);
			       ustrcat (url, imgname);
			     }
			   else
			     /* in same directory -> local name */
			     ustrcpy (url, imgname);

			   NormalizeURL (url, 0, tempname, imgname, newURL);
			   /* register the modification to be able to undo it */
			   TtaRegisterAttributeReplace (attr, el, doc);
			   TtaSetAttributeText (attr, url, el, doc);
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
#ifdef AMAYA_DEBUG
			   fprintf(stderr, "     SRC from %s to %s\n", buf, url);
#endif
			   if ((src_is_local) && (!dst_is_local))
			     /* add the localfile to the images list */
			     AddLocalImage (buf, imgname, tempname, doc, &pImage);
			     
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
				   ustrcpy (buf, localpath);
				   ustrcat (buf, imgname);
				 }

			       if (imgbase[0] != EOS)
				   ustrcpy (tempfile, tempname);
			       else
				 {
				   ustrcpy (tempfile, SavePath);
				   ustrcat (tempfile, DIR_STR);
				   ustrcat (tempfile, imgname);
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
				   ustrcpy (tempfile, localpath);
				   ustrcat (tempfile, imgname);
				   pImage = SearchLoadedImage (tempfile, doc);
				   /* update the descriptor */
				   if (pImage)
				     {
				       /* image was already loaded */
				       if (pImage->originalName != NULL)
					 TtaFreeMemory (pImage->originalName);
				       pImage->originalName = (STRING) TtaStrdup (tempname);
				       if (TtaFileExist(pImage->localName))
					 pImage->status = IMAGE_MODIFIED;
				       else
					 pImage->status = IMAGE_NOT_LOADED;
				       /*pImage->elImage = (struct _ElemImage *) el;*/
				     }
				 }
			       else
				 /* add the localfile to the images list */
				 AddLocalImage (tempfile, imgname, tempname, doc, &pImage);
			     }
			 }
		       TtaFreeMemory (buf);
		     }
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
  STRING              documentFile;
  STRING              tempname, localPath;
  STRING              imagePath, base;
  CHAR                imgbase[MAX_LENGTH];
  CHAR                url_sep;
  int                 res;
  int                 len;
  boolean             src_is_local;
  boolean             dst_is_local, ok;
  boolean	      docModified, toUndo;

  src_is_local = !IsW3Path (DocumentURLs[SavingDocument]);
  dst_is_local = !IsW3Path (SavePath);
  ok = TRUE;
  toUndo = FALSE;

#ifdef AMAYA_DEBUG
  fprintf(stderr, "DoSaveAs : from %s to %s/%s , with images %d\n", DocumentURLs[SavingDocument], SavePath, SaveName, (int) CopyImages);
#endif

  /* New document path */
  documentFile = TtaGetMemory (MAX_LENGTH);
  ustrcpy (documentFile, SavePath);
  len = ustrlen (documentFile);
  if (documentFile [len -1] != DIR_SEP && documentFile [len - 1] != '/')
    {
     if (dst_is_local)
       {
	 ustrcat (documentFile, DIR_STR);
	 url_sep = DIR_SEP;
       }
     else
       {
	 ustrcat (documentFile, URL_STR);
	 url_sep = URL_SEP;
       }
    }
  else if (dst_is_local)
    url_sep = DIR_SEP;
  else
    url_sep = URL_SEP;

  if (SaveName[0] == EOS)
    {
      /* there is no document name */
      if (AddNoName (SavingDocument, 1, documentFile, &ok))
	{
	  ok = TRUE;
	  res = ustrlen(SavePath) - 1;
	  if (SavePath[res] == url_sep)
	    SavePath[res] = EOS;
	  /* need to update the document url */
	  ustrcpy (SaveName, DefaultName);
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
  else
    ustrcat (documentFile, SaveName);

  doc = SavingDocument;
  if (ok && dst_is_local)
    {
      /* verify that the directory exists */
      if (!TtaCheckDirectory (SavePath))
	{
	  TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_CANNOT_SAVE), SavePath);
	  /* the user has to change the name of the images directory */
	  SaveDocumentAs(doc, 1);
	  ok = FALSE;
	}
      /* verify that we don't overwite anything and ask for confirmation */
      else if (TtaFileExist (documentFile))
	{
	  /* ask confirmation */
	  tempname = TtaGetMemory (MAX_LENGTH);
	  sprintf (tempname, TtaGetMessage (LIB, TMSG_FILE_EXIST), documentFile);
	  InitConfirm (doc, 1, tempname);
	  TtaFreeMemory (tempname);
	  if (!UserAnswer)
	    {
	      /* the user has to change the name of the saving file */
	      SaveDocumentAs(doc, 1);
	      ok = FALSE;
	    }
	}
    }

  /* Check information before starting the operation */
  if (ok && !TextFormat)
    {
      /* search if there is a BASE element within the document */
      root = TtaGetMainRoot (doc);
      elType.ElSSchema = TtaGetDocumentSSchema (doc);
      attrType.AttrSSchema = elType.ElSSchema;
      /* search the BASE element */
      elType.ElTypeNum = HTML_EL_BASE;
      el = TtaSearchTypedElement (elType, SearchInTree, root);
      if (el)
	/* URLs are still relative to the document base */
	base = GetBaseURL (doc);
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
	      ustrcpy (imgbase, imagePath);
	      TtaFreeMemory (imagePath);
	    }
	  else
	    imgbase[0] = EOS;
	  
	  /* verify that the directory exists */
	  if (dst_is_local)
	    {
	      tempname = TtaGetMemory (MAX_LENGTH);
	      if (imgbase[0] != DIR_SEP)
		  {
		    ustrcpy (tempname, SavePath);
		    ustrcat (tempname, DIR_STR);
		    ustrcat (tempname, imgbase);
		  }
	      else
		ustrcpy(tempname, imgbase);
	      ok = TtaCheckDirectory (tempname);
	      if (!ok)
		{
		  TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_CANNOT_SAVE), tempname);
		  TtaFreeMemory (tempname);
		  /* free base before returning*/
		  if (base)
		    TtaFreeMemory (base);
		  /* the user has to change the name of the images directory */
		  SaveDocumentAs(doc, 1);
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
	      ustrcpy (imgbase, imagePath);
	      TtaFreeMemory (imagePath);
	    }
	  else
	    imgbase[0] = EOS;
	}
      else
	imgbase[0] = EOS;
    }

  /*
    The saving could change URLs and SRC attributes in the document.
    The current state of the document is saved in a backup file
    and can be retored if the operation fails
    */
  if (ok)
    {
      docModified = TtaIsDocumentModified (doc);
      if (!src_is_local)
	/* store the name of the local temporary file */
	localPath = GetLocalPath (doc, DocumentURLs[doc]);
      else
	localPath = NULL;

      if (TextFormat)
	{
	  if (dst_is_local)
	    {
	      /* Local to Local or Remote to Local */
	      /* save the local document */
	      ok = TtaExportDocument (doc, documentFile, "TextFileT");
	    }
	  else
	    ok = SaveObjectThroughNet (doc, 1, documentFile, TRUE, TRUE);
	}
      else
	{
	  TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
	  /* Transform all URLs to absolute ones */
	  if (UpdateURLs)
	    {
	      if (base)
		/* URLs are still relative to the document base */
		SetRelativeURLs (doc, base);
	      else
		/* URLs are relative to the new document directory */
		SetRelativeURLs (doc, documentFile);
	    }
	  /* now free base */
	  if (base)
	    TtaFreeMemory (base);
	  
	  /* Change the document URL and if CopyImage is TRUE change all
	   * picture SRC attribute. If pictures are saved locally, make the copy
	   * else add them to the list of remote images to be copied.
	   */
	  UpdateImages (doc, src_is_local, dst_is_local, imgbase, documentFile);
	  toUndo = TtaCloseUndoSequence (doc);
	  
	  if (dst_is_local)
	    {
	      /* Local to Local or Remote to Local */
	      /* save the local document */
	      ok = SaveDocumentLocally (doc, SavePath, SaveName);
	    }
	  else
	    {
	      /* Local to Remote or Remote to Remote */
	      /* now save the file as through the normal process of saving */
	      /* to a remote URL. */
	      ok = SaveDocumentThroughNet (doc, 1, documentFile, TRUE, CopyImages, FALSE);
	    }
	}

      /* the saving operation is finished now */
      SavingDocument = 0;
      if (ok)
	{
	  if (toUndo)
	    TtaCancelLastRegisteredSequence (doc);
	  /* change the document url */
	  if (TextFormat || !SaveAsText)
	    {
	      TtaFreeMemory (DocumentURLs[doc]);
	      DocumentURLs[doc] = (STRING) TtaStrdup (documentFile);
	      TtaSetTextZone (doc, 1, 1, DocumentURLs[doc]);
	      TtaSetDocumentUnmodified (doc);
	    }
	  /* Sucess of the operation */
	  TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_SAVED), documentFile);
	  /* remove the previous temporary file */
	  if (localPath)
	    {
	      TtaFileUnlink (localPath);
	      TtaFreeMemory (localPath);
	    }
	}
      else
	{
	  /*
	    Operation failed:
	    restore the previous contents of the document
	    */
	  if (toUndo)
	    TtaUndoNoRedo (doc);
	  TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_CANNOT_SAVE), documentFile);
	  /* restore the previous status of the document */
	  if (!docModified)
	    TtaSetDocumentUnmodified (doc);
	  if (localPath)
	    TtaFreeMemory (localPath);
	  /* propose to save a second time */
	  SaveDocumentAs(doc, 1);
	}
    }
  TtaFreeMemory (documentFile);
}









