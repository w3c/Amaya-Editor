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
 *          R. Guetari: Unicode and Windows version
 *
 */

/* DEBUG_AMAYA_SAVE Print out debug information when saving */

/* Included headerfiles */
#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"

#ifdef ANNOTATIONS
#include "annotlib.h"
#endif /* ANNOTATIONS */

#ifdef _WINDOWS
#include "resource.h"
static CHAR_T       currentDocToSave[MAX_LENGTH];
static CHAR_T       currentPathName[MAX_LENGTH];
extern HINSTANCE    hInstance;
#endif /* _WINDOWS */


#define StdDefaultName TEXT("Overview.html")
static STRING       DefaultName;
static CHAR_T       tempSavedObject[MAX_LENGTH];
static int          URL_attr_tab[] = {
   HTML_ATTR_HREF_,
   HTML_ATTR_codebase,
   HTML_ATTR_Script_URL,
   HTML_ATTR_SRC,
   HTML_ATTR_background_,
   HTML_ATTR_Style_,
   HTML_ATTR_cite
};
static ThotBool     TextFormat;
static int          SRC_attr_tab[] = {
   HTML_ATTR_SRC,
   HTML_ATTR_background_,
   HTML_ATTR_Style_
};
static STRING       QuotedText;

#include "AHTURLTools_f.h"
#include "EDITimage_f.h"
#include "EDITstyle_f.h"
#include "HTMLactions_f.h"
#include "HTMLbook_f.h"
#include "HTMLimage_f.h"
#include "HTMLsave_f.h"
#include "html2thot_f.h"
#include "init_f.h"
#include "query_f.h"
#include "styleparser_f.h"

#ifdef _WINDOWS
#include "wininclude.h"

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
  static CHAR_T txt [500];
  
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
	  WIN_ListSaveDirectory (BaseDialog + SaveForm, TtaGetMessage (AMAYA, AM_SAVE_AS), currentDocToSave);
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
  usprintf (currentPathName, path_name);
  DialogBox (hInstance, MAKEINTRESOURCE (GETSAVEDIALOG), parent, (DLGPROC) GetSaveDlgProc);
}
#endif /* _WINDOWS */

/*----------------------------------------------------------------------
  CheckGenerator                                                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            CheckGenerator (NotifyElement * event)
#else  /* __STDC__ */
ThotBool            CheckGenerator (event)
NotifyElement      *event;

#endif /* __STDC__ */
{
  AttributeType      attrType;
  Attribute          attr;
  CHAR_T             buff[MAX_LENGTH];
  STRING             ptr;
  int                length;

  attrType.AttrSSchema = TtaGetDocumentSSchema (event->document);
  attrType.AttrTypeNum = HTML_ATTR_meta_name;
  attr = TtaGetAttribute (event->element, attrType);
  if (attr != 0)
    {
      length = MAX_LENGTH - 1;
      TtaGiveTextAttributeValue (attr, buff, &length);
      if (!ustrcasecmp (buff, TEXT("GENERATOR")))
	{
	  /* is it Amaya generator ? */
	  attrType.AttrTypeNum = HTML_ATTR_meta_content;
	  attr = TtaGetAttribute (event->element, attrType);
	  if (attr != 0)
	    {
	      length = MAX_LENGTH - 1;
	      TtaGiveTextAttributeValue (attr, buff, &length);
	      ptr = ustrstr (buff, TEXT("amaya"));
	      if (ptr == NULL)
		ptr = ustrstr (buff, TEXT("Amaya"));
	      if (ptr == NULL)
		/* it's not a pure Amaya document -> remove the meta element */
		return TRUE;
	      else
		{
		  /* update the version */
		  ustrcpy (buff, HTAppName); 
		  ustrcat (buff, TEXT(" "));
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
ThotBool            GenerateQuoteBefore (NotifyAttribute * event)
#else  /* __STDC__ */
ThotBool            GenerateQuoteBefore (event)
NotifyAttribute    *event;

#endif /* __STDC__ */
{
  STRING            ptr;
  int               length;

  length = TtaGetTextAttributeLength (event->attribute);
  QuotedText = TtaAllocString (length + 3);
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
  CHAR_T                old_url[MAX_LENGTH];
  CHAR_T                oldpath[MAX_LENGTH];
  CHAR_T                tempname[MAX_LENGTH];
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
   CHAR_T             buffer[3000];
   CHAR_T             s[MAX_LENGTH];
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
   else if (IsXMLName (pathname) || DocumentMeta[document]->xmlformat)
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
		   TtaGetMessage (AMAYA, AM_DOC_LOCATION), 50, 1, FALSE);
   TtaSetTextForm (BaseDialog + NameSave, pathname);
   TtaNewLabel (BaseDialog + Label1, BaseDialog + SaveForm, "");
   TtaNewLabel (BaseDialog + Label2, BaseDialog + SaveForm, "");
   /* third line */
   TtaNewTextForm (BaseDialog + ImgDirSave, BaseDialog + SaveForm,
		   TtaGetMessage (AMAYA, AM_IMAGES_LOCATION), 50, 1, FALSE);
   TtaSetTextForm (BaseDialog + ImgDirSave, SaveImgsURL);
   TtaNewLabel (BaseDialog + Label3, BaseDialog + SaveForm, "");
   TtaNewLabel (BaseDialog + Label4, BaseDialog + SaveForm, "");
   TtaNewTextForm (BaseDialog + FilterText, BaseDialog + SaveForm,
		   TtaGetMessage (AMAYA, AM_PARSE), 10, 1, TRUE);
   TtaSetTextForm (BaseDialog + FilterText, ScanFilter);

   TtaShowDialogue (BaseDialog + SaveForm, FALSE);
   if (TextFormat)
     {
       TtaRedrawMenuEntry (BaseDialog + ToggleSave, 0, NULL, -1, FALSE);
       TtaRedrawMenuEntry (BaseDialog + ToggleSave, 1, NULL, -1, FALSE);
       TtaRedrawMenuEntry (BaseDialog + ToggleSave, 4, NULL, -1, FALSE);
       TtaRedrawMenuEntry (BaseDialog + ToggleSave, 5, NULL, -1, FALSE);
     }
#  else /* _WINDOWS */
   CreateSaveAsDlgWindow (TtaGetViewFrame (document, view), pathname, BaseDialog, SaveForm, DirSave, NameSave, ImgDirSave, ToggleSave);
#  endif /* _WINDOWS */
}


/*----------------------------------------------------------------------
  InitSaveObjectForm
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
   CHAR_T                tempdir[MAX_LENGTH];
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
		   TtaGetMessage (AMAYA, AM_OBJECT_LOCATION), 50, 1, FALSE);
   TtaSetTextForm (BaseDialog + NameSave, pathname);
   TtaExtractName (pathname, tempdir, ObjectName);
   TtaSetDialoguePosition ();
   TtaShowDialogue (BaseDialog + SaveForm, FALSE);
#  else  /* _WINDOWS */
   CreateGetSaveDlgWindow (TtaGetViewFrame (document, view), pathname);
#  endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
  DeleteTempObjectFile
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
  DoSaveObjectAs
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DoSaveObjectAs (void)
#else
void                DoSaveObjectAs ()
#endif
{
   CHAR_T           tempfile[MAX_LENGTH];
   CHAR_T           msg[MAX_LENGTH];
   ThotBool         dst_is_local;
   int              res;

   dst_is_local = !IsW3Path (SavePath);

   ustrcpy (tempfile, SavePath);
   ustrcat (tempfile, WC_DIR_STR);
   ustrcat (tempfile, ObjectName);


   if (!dst_is_local)
     {
	/* @@ We need to check the use of AMAYA_PREWRITE_VERIFY in this function*/
	res = PutObjectWWW (SavingObject, tempSavedObject, tempfile,
			    unknown_type,
			    AMAYA_SYNC | AMAYA_NOCACHE |  AMAYA_FLUSH_REQUEST 
			    | AMAYA_USE_PRECONDITIONS, NULL, NULL);
	if (res)
	  {
#        ifndef _WINDOWS
	     TtaSetDialoguePosition ();
	     TtaShowDialogue (BaseDialog + SaveForm, FALSE);
#        endif /* !_WINDOWS */
	     return;
	  }
	SavingObject = 0;
	SavingDocument = 0;
	return;
     }
   if (TtaFileExist (tempfile))
     {
	/* ask confirmation */
	usprintf (msg, TtaGetMessage (LIB, TMSG_FILE_EXIST), tempfile);
	InitConfirm (SavingObject, 1, msg);
	if (!UserAnswer)
	  {
	     /* the user has to change the name of the saving file */
#        ifndef _WINDOWS
	     TtaSetDialoguePosition ();
	     TtaShowDialogue (BaseDialog + SaveForm, FALSE);
#        endif /* !_WINDOWS */
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
   CHAR_T           tempname[MAX_LENGTH];
   int              i;

   if ((SavingDocument != 0 && SavingDocument != doc) ||
       SavingObject != 0)
      return;
   else if (DocumentTypes[doc] == docSource ||
	    DocumentTypes[doc] == docSourceRO)
      /* it's a source "view". Don't save it */
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
       if (i > 2 && !ustrcmp (&tempname[i-2], TEXT(".gz")))
	 {
	   tempname[i-2] = EOS;
	   TtaFreeMemory (DocumentURLs[doc]);
	   DocumentURLs[doc] = TtaWCSdup (tempname);
	 }
       else if (i > 1 && !ustrcmp (&tempname[i-1], TEXT(".Z")))
	 {
	   tempname[i-1] = EOS;
	   TtaFreeMemory (DocumentURLs[doc]);
	   DocumentURLs[doc] = TtaWCSdup (tempname);
	 }

       /* if it is a Web document use the current SavePath */
       if (IsW3Path (tempname))
	 {
	     TtaExtractName (tempname, SavePath, SaveName);
	     if (SaveName[0] == WC_EOS)
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
	       ustrcat (SaveName, TEXT(".html"));
	       ustrcpy (tempname, SavePath);
	       ustrcat (tempname, WC_URL_STR);
	       ustrcat (tempname, SaveName);
 	     }
	 }
       else
	 {
	   TtaExtractName (tempname, SavePath, SaveName);
	 }
#      ifndef _WINDOWS
       TtaSetDialoguePosition ();
#      endif /* !_WINDOWS */
     }
   else
     {
       ustrcpy (tempname, SavePath);
       ustrcat (tempname, WC_DIR_STR);
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
   Create a META element to specify Content-Type and Charset.
   Set the content of the Charset attribute (on the root element)
   according to the encoding used in the document.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void         SetNamespacesAndDTD (Document doc)
#else
void         SetNamespacesAndDTD (doc)
Document     doc;

#endif
{
   Element		root, el, head, meta, lastmeta, lastel;
   ElementType		elType;
   AttributeType	attrType;
   Attribute		attr, charsetAttr;
   Language             lang;
   int                  length;
   STRING               text;
   CHAR_T               ISOlatin;
#define MAX_CHARSET_LEN 50
   CHAR_T               Charset[MAX_CHARSET_LEN];
   CHAR_T		buffer[200];
   ThotBool		useMathML, useGraphML, useFrames;

   useMathML = FALSE;
   useGraphML = FALSE;
   if (TtaGetSSchema (TEXT("MathML"), doc) != NULL)
      useMathML = TRUE;
   if (TtaGetSSchema (TEXT("GraphML"), doc) != NULL)
      useGraphML = TRUE;

   root = TtaGetMainRoot (doc);
   attrType.AttrSSchema = TtaGetSSchema (TEXT("HTML"), doc);
   attrType.AttrTypeNum = HTML_ATTR_Namespaces;
   attr = TtaGetAttribute (root, attrType);
   if (!useMathML && !useGraphML && attr)
      /* delete the Namespaces attribute */
      TtaRemoveAttribute (root, attr, doc);
   else if (useMathML || useGraphML)
      {
      /* prepare the value of attribute Namespaces */
      buffer[0] = EOS;
      if (useMathML)
	 {
	 ustrcat (buffer, TEXT("\n      xmlns:m=\"http://www.w3.org/1998/Math/MathML/\""));
	 }
      if (useGraphML)
	 {
	 ustrcat (buffer, TEXT("\n      xmlns:g=\"http://www.w3.org/Graphics/SVG/Amaya2D\""));
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
      TtaSetAttributeValue (attr, HTML_ATTR_HtmlDTD_VAL_Transitional, root,
			    doc);

   /* get the charset defined by the Charset attribute of the root element*/
   attrType.AttrTypeNum = HTML_ATTR_Charset;
   charsetAttr = TtaGetAttribute (root, attrType);
   Charset[0] = EOS;
   if (charsetAttr)
      {
      length = MAX_CHARSET_LEN - 2;
      TtaGiveTextAttributeValue (charsetAttr, Charset, &length);
      }

   /* Create (or update) a META element to specify Content-type and Charset */
   /* Get the HEAD element first */
   el = TtaGetFirstChild (root);
   head = NULL;
   while (el && !head)
      {
      elType = TtaGetElementType (el);
      if (elType.ElSSchema == attrType.AttrSSchema &&
	  elType.ElTypeNum == HTML_EL_HEAD)
	 head = el;
      else
         TtaNextSibling (&el);
      }
   if (head)
      {
      if (Charset[0] == EOS)
	/* no Charset attribute on the root element */
	{
        /* is there a Language attribute on the root element? */
        attrType.AttrTypeNum = HTML_ATTR_Langue;
        attr = TtaGetAttribute (root, attrType);
        if (attr)
	   /* there is a language attribute on the root */
	   /* is it the defaut attribute set by Thot or the real one */
	   {
           attrType.AttrTypeNum = HTML_ATTR_RealLang;
           if (!TtaGetAttribute (root, attrType))
	     /* not the real one. look further */
	      attr = NULL;
	   }
        if (!attr)
	   /* no Language specified on the root. Look for the body element */
	   {
           attrType.AttrTypeNum = HTML_ATTR_Langue;
	   el = head;
           TtaNextSibling (&el);
	   while (el && !attr)
	     {
             elType = TtaGetElementType (el);
	     if (elType.ElSSchema == attrType.AttrSSchema &&
	         elType.ElTypeNum == HTML_EL_BODY)
	       /* it's the BODY element. Is there a language attribute on it */
	       {
	       attr = TtaGetAttribute (el, attrType);
	       el = NULL;
	       }
	     else
	       TtaNextSibling (&el);
	     }
	   }
        if (attr)
	   /* there is a Language attribute on the root or body element */
	   {
	   length = TtaGetTextAttributeLength (attr);
	   text = TtaAllocString (length + 1);
	   TtaGiveTextAttributeValue (attr, text, &length);
	   lang = TtaGetLanguageIdFromName (text);
	   TtaFreeMemory (text);
	   ISOlatin = TtaGetAlphabet (lang);
	   if (ISOlatin == TEXT('L'))
	     ustrcpy (Charset, TEXT("iso-8859-1"));
	   else if (ISOlatin == TEXT('2'))
	     ustrcpy (Charset, TEXT("iso-8859-2"));
	   else if (ISOlatin == TEXT('9'))
	     ustrcpy (Charset, TEXT("iso-8859-9"));
	   else
	     Charset[0] = EOS;
	   }
	}
      el = TtaGetFirstChild (head);
      meta = NULL;
      lastmeta = NULL;
      lastel = NULL;
      attrType.AttrTypeNum = HTML_ATTR_http_equiv;
      attr = NULL;
      while (el && !meta)
	 {
	 elType = TtaGetElementType (el);
         if (elType.ElSSchema == attrType.AttrSSchema &&
             elType.ElTypeNum == HTML_EL_META)
	    {
	    lastmeta = meta;
	    attr = TtaGetAttribute (el, attrType);
	    if (attr)
	       meta = el;
	    }
	 if (!meta)
	    {
	    lastel = el;
	    TtaNextSibling (&el);
	    }
	 }
      if (!meta)
	 {
	 elType.ElSSchema = attrType.AttrSSchema;
	 elType.ElTypeNum = HTML_EL_META;
	 meta = TtaNewElement (doc, elType);
	 if (!lastmeta)
	    lastmeta = lastel;
	 if (lastmeta)
	    TtaInsertSibling (meta, lastmeta, FALSE, doc);
	 else
	    TtaInsertFirstChild (&meta, head, doc);
	 }
      if (!attr)
	 {
	 attr = TtaNewAttribute (attrType);
	 TtaAttachAttribute (meta, attr, doc);
	 }
      TtaSetAttributeText (attr, TEXT("Content-Type"), meta, doc);
      attrType.AttrTypeNum = HTML_ATTR_meta_content;
      attr = TtaGetAttribute (meta, attrType);
      if (!attr)
	 {
	 attr = TtaNewAttribute (attrType);
	 TtaAttachAttribute (meta, attr, doc);
	 }
      if (Charset[0] == EOS)
	 TtaSetAttributeText (attr, TEXT("text/html"), meta, doc);
      else
	 {
	 ustrcpy (buffer, TEXT("text/html; charset="));
	 ustrcat (buffer, Charset);
	 TtaSetAttributeText (attr, buffer, meta, doc);
	 }
      } 
   if (!charsetAttr)
     if (Charset[0] != EOS)
	/* create a Charset attribute on the root element */
	{
	attrType.AttrTypeNum = HTML_ATTR_Charset;
	charsetAttr = TtaNewAttribute (attrType);
	TtaAttachAttribute (root, charsetAttr, doc);
	TtaSetAttributeText (charsetAttr, Charset, root, doc);	
	}
}

/*----------------------------------------------------------------------
   RedisplaySourceFile
   If doc is a HTML document and the source view is open, redisplay the
   source.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void       RedisplaySourceFile (Document doc)
#else
static void       RedisplaySourceFile (doc)
Document          doc;

#endif
{
  CHAR_T*   localFile;
  CHAR_T	documentname[MAX_LENGTH];
  CHAR_T	tempdir[MAX_LENGTH];
  NotifyElement event;

  if (DocumentTypes[doc] == docHTML || DocumentTypes[doc] == docHTMLRO)
    /* It's a HTML document */
    {
    if (DocumentSource[doc])
       /* The source code of this document is currently displayed */
       {
	 /* Get its local copy */
          localFile = GetLocalPath (doc, DocumentURLs[doc]);
	  TtaExtractName (localFile, tempdir, documentname);
	  /* parse and display the new version */
	  StartParser (DocumentSource[doc], localFile, documentname, tempdir,
		       localFile, TRUE);
	  TtaSetDocumentUnmodified (DocumentSource[doc]);
	  event.document = doc;
	  SynchronizeSourceView (&event);
	  TtaSetDocumentName (DocumentSource[doc], documentname);
	  SetWindowTitle (doc, DocumentSource[doc], 0);
	  TtaFreeMemory (localFile);
       }
    }
}

/*----------------------------------------------------------------------
   SaveDocumentLocally save the document in a local file.
   Return TRUE if the document has been saved
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool   SaveDocumentLocally (Document doc, STRING directoryName, STRING documentName)
#else
static ThotBool   SaveDocumentLocally (doc, directoryName, documentName)
Document          doc;
STRING            directoryName;
STRING            documentName;
#endif
{
  STRING              ptr;
  CHAR_T              tempname[MAX_LENGTH];
  CHAR_T              docname[100];
  ThotBool            ok;

#ifdef AMAYA_DEBUG
  fprintf(stderr, "SaveDocumentLocally :  %s / %s\n", directoryName, documentName);
#endif

  ustrcpy (tempname, directoryName);
  ustrcat (tempname, WC_DIR_STR);
  ustrcat (tempname, documentName);

  if (SaveAsText) 
    {
      SetInternalLinks (doc);
      ok = TtaExportDocument (doc, tempname, TEXT("HTMLTT"));
    }
  else
    {
      SetNamespacesAndDTD (doc);
      if (SaveAsXHTML)
	{
	ok = TtaExportDocumentWithNewLineNumbers (doc, tempname, TEXT("HTMLTX"));
	DocumentMeta[doc]->xmlformat = TRUE;
	}
      else
	{
	ok = TtaExportDocumentWithNewLineNumbers (doc, tempname, TEXT("HTMLT"));
	DocumentMeta[doc]->xmlformat = FALSE;
	}
      if (ok)
	{
	  TtaSetDocumentDirectory (doc, directoryName);
	  ustrcpy (docname, documentName);
	  /* Change the document name in all views */
	  TtaSetDocumentName (doc, docname);
	  SetWindowTitle (doc, doc, 0);
	  if (DocumentSource[doc])
	     SetWindowTitle (doc, DocumentSource[doc], 0);
	  /* save a local copy of the current document */
	  ptr = GetLocalPath (doc, tempname);
	  TtaFileCopy (tempname, ptr);
	  TtaFreeMemory (ptr);
	}
    }
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
static ThotBool    AddNoName (Document document, View view, CHAR_T* url, ThotBool *ok)
#else
static ThotBool    AddNoName (document, view, url, ok)
Document           document;
View               view;
CHAR_T*            url;
ThotBool          *ok;
#endif
{
   CHAR_T            msg[MAX_LENGTH];
   CHAR_T            documentname[MAX_LENGTH];
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
	  if (url[len -1] != WC_URL_SEP)
	    ustrcat (msg, WC_URL_STR);
	}
      else if (url[len -1] != WC_DIR_SEP)
	ustrcat (msg, WC_DIR_STR);
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
static int   SafeSaveFileThroughNet (Document doc, STRING localfile, STRING remotefile, PicType filetype, ThotBool use_preconditions)
#else
static int   SafeSaveFileThroughNet (doc, localfile, remotefile, filetype, use_preconditions)
Document     doc;
STRING       localfile;
STRING       remotefile;
PicType      filetype;
ThotBool     use_preconditions;
#endif
{
  CHAR_T              msg[MAX_LENGTH];
  CHAR_T              tempfile[MAX_LENGTH]; /* File name used to refetch */
  CHAR_T              tempURL[MAX_LENGTH];  /* May be redirected */
  STRING            verify_publish;
  int               res;
  int               mode = 0;

  verify_publish = TtaGetEnvString("VERIFY_PUBLISH");
  /* verify the PUT by default */
  if (verify_publish == NULL)
    verify_publish = TEXT("yes");
  
  
#ifdef AMAYA_DEBUG
  fprintf(stderr, "SafeSaveFileThroughNet :  %s to %s type %d\n", localfile, remotefile, filetype);
#endif

  /* Save */
  /* JK: SYNC requests assume that the remotefile name is a static array */
  ustrcpy (tempfile, remotefile);
  mode = AMAYA_SYNC | AMAYA_NOCACHE | AMAYA_FLUSH_REQUEST;
  mode = mode | ((use_preconditions) ? AMAYA_USE_PRECONDITIONS : 0);

  res = PutObjectWWW (doc, localfile, tempfile, mode, filetype, NULL, NULL);
  if (res != 0)
    /* The HTTP PUT method failed ! */
    return (res);

  /* does the user want to verify the PUT? */
  if (!verify_publish || !*verify_publish || ustrcmp (verify_publish, TEXT("yes")))
    return (0);

  /* Refetch */
#ifdef AMAYA_DEBUG
  fprintf(stderr, "SafeSaveFileThroughNet :  refetch %s \n", remotefile);
#endif

  TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_VERIFYING), NULL);
  ustrcpy (tempURL, remotefile);
  res = GetObjectWWW (doc, tempURL, NULL, tempfile, AMAYA_SYNC | AMAYA_NOCACHE
		      | AMAYA_FLUSH_REQUEST, NULL, NULL, NULL, NULL, NO, NULL);
  if (res != 0)
    {
      /* The HTTP GET method failed ! */
      usprintf (msg, TtaGetMessage (AMAYA, AM_SAVE_RELOAD_FAILED), remotefile);
      InitConfirm (doc, 1, msg);
      if (UserAnswer)
	/* Ignore the read failure */
	res = 0;
    }

  if (res == 0)
    {
      /* Compare content. */
#ifdef AMAYA_DEBUG
      fprintf(stderr, "SafeSaveFileThroughNet :  compare %s and %s \n", remotefile, localfile);
#endif
      if (! TtaCompareFiles(tempfile, localfile))
	{
	  usprintf (msg, TtaGetMessage (AMAYA, AM_SAVE_COMPARE_FAILED), remotefile);
	  InitConfirm (doc, 1, msg);
	  if (!UserAnswer)
	    res = -1;
	}
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
static ThotBool     SaveObjectThroughNet (Document document, View view,
					  STRING url, ThotBool confirm,
					  ThotBool use_preconditions)
#else
static ThotBool     SaveObjectThroughNet (document, view, url, confirm,
					  use_preconditions)
Document            document;
View                view;
STRING              url;
ThotBool            confirm;
ThotBool            use_preconditions;
#endif
{
  STRING           tempname;
  STRING           msg;
  int              remainder = 500;
  int              res;

  msg = TtaAllocString (remainder);
  if (msg == NULL)
    return (FALSE);

  /* save into the temporary document file */
  tempname = GetLocalPath (document, url);

  /* build the output */
  if (DocumentTypes[document] == docSource ||
      DocumentTypes[document] == docSourceRO)
      /* it's a source file, renumber lines */
      TtaExportDocumentWithNewLineNumbers (document, tempname,
					   TEXT("TextFileT"));
  else
      TtaExportDocument (document, tempname, TEXT("TextFileT"));

  ActiveTransfer (document);
  TtaHandlePendingEvents ();
  res = SafeSaveFileThroughNet (document, tempname, url, unknown_type,
				use_preconditions);
  if (res != 0)
    {
      DocNetworkStatus[document] |= AMAYA_NET_ERROR;
      ResetStop (document);
      usprintf (msg, TEXT("%s %s"),
		TtaGetMessage (AMAYA, AM_URL_SAVE_FAILED),
		url);
      if (confirm)
	{
	  InitConfirm3L (document, view, msg, AmayaLastHTTPErrorMsg, 
			 TtaGetMessage (AMAYA, AM_SAVE_DISK));
	  if (UserAnswer)
	    res = -1;
	  else
	    res = 0;
	}
      else
	{
	  InitInfo (TEXT("Save"), msg);
	  res = -1;
	}
      /* erase the last status message */
      TtaSetStatus (document, view, TEXT(""), NULL);	       
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
static ThotBool  SaveDocumentThroughNet (Document doc, View view, STRING url,
					 ThotBool confirm, ThotBool with_images,
					 ThotBool use_preconditions)
#else
static ThotBool  SaveDocumentThroughNet (doc, view, url, confirm,
                                         with_images, use_preconditions)
Document         doc;
View             view;
STRING           url;
ThotBool         confirm;
ThotBool         with_images;
ThotBool         use_preconditions;
#endif
{
  LoadedImageDesc *pImage;
  STRING           tempname;
  STRING           msg;
  int              remainder = 10000;
  int              index = 0, len, nb = 0;
  int              imageType, res;

  msg = TtaAllocString (remainder);
  if (msg == NULL)
    return (FALSE);

  /* save into the temporary document file */
  tempname = GetLocalPath (doc, url);

  /* First step : generate the output file and ask for confirmation */
  SetNamespacesAndDTD (doc);
  if (SaveAsXHTML)
    {
    TtaExportDocumentWithNewLineNumbers (doc, tempname, TEXT("HTMLTX"));
    DocumentMeta[doc]->xmlformat = TRUE;
    }
  else
    {
    TtaExportDocumentWithNewLineNumbers (doc, tempname, TEXT("HTMLT"));
    DocumentMeta[doc]->xmlformat = FALSE;
    }

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
		  ustrcpy (&msg[index], TEXT("..."));
		  len = ustrlen (TEXT("..."));
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

#     ifndef _WINDOWS 
      TtaNewSelector (BaseDialog + ConfirmSaveList, BaseDialog + ConfirmSave,
		      NULL, nb, msg, 6, NULL, FALSE, TRUE);
       
      TtaSetDialoguePosition ();
      TtaShowDialogue (BaseDialog + ConfirmSave, FALSE);
      /* wait for an answer */
      TtaWaitShowDialogue ();
#     else  /* _WINDOWS */
      CreateSaveListDlgWindow (TtaGetViewFrame (doc, view), nb, msg, BaseDialog, ConfirmSave);
#     endif /* _WINDOWS */
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

      res = SafeSaveFileThroughNet (doc, tempname, url, unknown_type, use_preconditions);
      if (res != 0)
	{
	  DocNetworkStatus[doc] |= AMAYA_NET_ERROR;
	  ResetStop (doc);
	  usprintf (msg, TEXT("%s %s"),
		    TtaGetMessage (AMAYA, AM_URL_SAVE_FAILED),
		    url);
	  if (confirm)
	    {
	      InitConfirm3L (doc, view, msg, AmayaLastHTTPErrorMsg, NULL);
	      if (UserAnswer)
		res = -1;
	      else
		res = -1;
	    }
	  else
	    {
	      InitInfo (TEXT("Save"), msg);
	      res = -1;
	    }
	  /* JK: to erase the last status message */
	  TtaSetStatus (doc, view, TEXT(""), NULL);	       
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
		  DocNetworkStatus[doc] |= AMAYA_NET_ERROR;
		  ResetStop (doc);
		  usprintf (msg, TEXT("%s %s"),
			    TtaGetMessage (AMAYA, AM_URL_SAVE_FAILED),
			    pImage->originalName);
		  InitConfirm3L (doc, view, msg, AmayaLastHTTPErrorMsg, NULL);
		  /* erase the last status message */
		  TtaSetStatus (doc, view, TEXT(""), NULL);
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
   GetHTMLdocFromSource
   If sourceDoc is a source file, return the corresponding HTML document.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
Document       GetHTMLdocFromSource (Document sourceDoc)
#else
Document       GetHTMLdocFromSource (sourceDoc)
Document       sourceDoc;

#endif
{
  Document	htmlDoc;
  int		i;

  htmlDoc = 0;
  if (DocumentTypes[sourceDoc] == docSource ||
      DocumentTypes[sourceDoc] == docSourceRO)
     /* It's a source file */
     for (i = 1; i < DocumentTableLength && htmlDoc == 0; i++)
        if (DocumentTypes[i] == docHTML)
           if (DocumentSource[i] == sourceDoc)
	      htmlDoc = i;
  return htmlDoc;
}

/*----------------------------------------------------------------------
   Synchronize
   save the current view (source/html) in a temporary file and update
   the other view (html/source).      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                Synchronize (Document document, View view)
#else  /* __STDC__ */
void                Synchronize (document, view)
Document            document;
View                view;
#endif /* __STDC__ */
{
   NotifyElement       event;
   CHAR_T*             tempdocument = NULL;
   CHAR_T              documentname[MAX_LENGTH];
   CHAR_T              tempdir[MAX_LENGTH];
   DisplayMode         dispMode;
   Document            htmlDoc, otherDoc;

   if (!DocumentURLs[document])
     /* the document is not loaded yet */
     return;
   if (!TtaIsDocumentUpdated (document))
     /* nothing new to be saved in this view of the document. Let see if
        the other view has been modified */
     {
       if (DocumentTypes[document] == docHTML ||
           DocumentTypes[document] == docHTMLRO)
           /* it's an HTML document */
	  otherDoc = DocumentSource[document];
       else if (DocumentTypes[document] == docSource ||
                DocumentTypes[document] == docSourceRO)
          otherDoc = GetHTMLdocFromSource (document);
       else
	  return;
       if (!TtaIsDocumentUpdated (otherDoc))
	  /* the other view has not been modified either */
	  return;
       else
	  /* save the other view */
	  document = otherDoc;
     }

   /* change display mode to avoid flicker due to callbacks executed when
      saving some elements, for instance META */
   dispMode = TtaGetDisplayMode (document);
   if (dispMode == DisplayImmediately)
     TtaSetDisplayMode (document, DeferredDisplay);

   if (DocumentTypes[document] == docHTML ||
       DocumentTypes[document] == docHTMLRO)
     /* it's an HTML document */
     {
       /* save the current state of the document into the temporary file */
       tempdocument = GetLocalPath (document, DocumentURLs[document]);
       SetNamespacesAndDTD (document);
       TtaExportDocumentWithNewLineNumbers (document, tempdocument,
					    TEXT("HTMLT"));
       RedisplaySourceFile (document);
       otherDoc = DocumentSource[document];
     }
   else if (DocumentTypes[document] == docSource ||
       DocumentTypes[document] == docSourceRO)
     /* it's a source document */
     {
       htmlDoc = GetHTMLdocFromSource (document);
       otherDoc = htmlDoc;
       /* save the current state of the document into the temporary file */
       tempdocument = GetLocalPath (htmlDoc, DocumentURLs[htmlDoc]);
       TtaExportDocumentWithNewLineNumbers (document, tempdocument,
					    TEXT("TextFileT"));
       TtaExtractName (tempdocument, tempdir, documentname);
       StartParser (htmlDoc, tempdocument, documentname, tempdir, tempdocument,
		    FALSE);
       /* fetch and display all images referred by the document */
       DocNetworkStatus[htmlDoc] = AMAYA_NET_ACTIVE;
       FetchAndDisplayImages (htmlDoc, AMAYA_LOAD_IMAGE);
       DocNetworkStatus[htmlDoc] = AMAYA_NET_INACTIVE;
     }
   /* restore original display mode */
   TtaSetDisplayMode (document, dispMode);

   /* the other document is now different from the original file. It can
      be saved */
   TtaSetDocumentModified (otherDoc);
   TtaSetItemOn (otherDoc, 1, File, BSave);
   /* disable the Synchronize command for both documents */
   TtaSetItemOff (otherDoc, 1, File, BSynchro);
   TtaSetItemOff (document, 1, File, BSynchro);
   TtaSetDocumentUnupdated (otherDoc);
   TtaSetDocumentUnupdated (document);

   /* Synchronize selections */
   event.document = document;
   SynchronizeSourceView (&event);
   TtaFreeMemory (tempdocument);
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
  NotifyElement       event;
  CHAR_T              tempname[MAX_LENGTH];
  CHAR_T              localFile[MAX_LENGTH];
  CHAR_T              documentname[MAX_LENGTH];
  CHAR_T              tempdir[MAX_LENGTH];
  CHAR_T*             ptr;
  int                 i, res;
  Document	      htmlDoc;
  DisplayMode         dispMode;
  ThotBool            ok, newLineNumbers;

#ifdef ANNOTATIONS
  if (DocumentTypes[doc] == docAnnot) 
    {
      ANNOT_SaveDocument (doc);
      return;
    }
  else
#endif /* ANNOTATIONS */

  if (SavingDocument != 0 || SavingObject != 0)
    return;
  else if (!TtaGetDocumentAccessMode (doc))
    /* the document is in ReadOnly mode */
    return;
  else if (!TtaIsDocumentModified (doc))
    {
      TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_NOTHING_TO_SAVE), TEXT(""));
      return;
    }

  TextFormat = (DocumentTypes[doc] == docText ||
		DocumentTypes[doc] == docTextRO ||
		DocumentTypes[doc] == docCSS ||
		DocumentTypes[doc] == docCSSRO ||
		DocumentTypes[doc] == docSource ||
		DocumentTypes[doc] == docSourceRO);

  /* if it's a source document, get the corresponding HTML document */
  if (DocumentTypes[doc] == docSource ||
      DocumentTypes[doc] == docSourceRO)
     htmlDoc = GetHTMLdocFromSource (doc);
  else
     htmlDoc = 0;

  SavingDocument = doc;

  ok = FALSE;
  newLineNumbers = FALSE;

  /* attempt to save through network if possible */
  ustrcpy (tempname, DocumentURLs[doc]);

  /* suppress compress suffixes from tempname */
  i = ustrlen (tempname) - 1;
  if (i > 2 && !ustrcmp (&tempname[i-2], TEXT(".gz")))
    {
      tempname[i-2] = EOS;
      TtaFreeMemory (DocumentURLs[doc]);
      DocumentURLs[doc] = TtaWCSdup (tempname);
    }
  else if (i > 1 && !ustrcmp (&tempname[i-1], TEXT(".Z")))
    {
      tempname[i-1] = EOS;
      TtaFreeMemory (DocumentURLs[doc]);
      DocumentURLs[doc] = TtaWCSdup (tempname);
    }

#ifdef AMAYA_DEBUG
  fprintf(stderr, "SaveDocument : %d to %s\n", doc, tempname);
#endif

  /* change display mode to avoid flicker due to callbacks executed when
     saving some elements, for instance META */
  dispMode = TtaGetDisplayMode (doc);
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, DeferredDisplay);

  /* the suffix fixes the output format of HTML saved document */
  SaveAsXHTML = IsXMLName (tempname) || DocumentMeta[doc]->xmlformat;
  if (IsW3Path (tempname))
    /* it's a remote document */
    {
      if (AddNoName (doc, view, tempname, &ok))
	{
	  ok = TRUE;
	  /* need to update the document url */
	  res = ustrlen(tempname) - 1;
	  if (tempname[res] != WC_URL_SEP)
	    ustrcat (tempname, WC_URL_STR);
	  ustrcat (tempname, DefaultName);
	  TtaFreeMemory (DocumentURLs[doc]);
	  DocumentURLs[doc] = TtaWCSdup (tempname);
	  DocumentMeta[doc]->put_default_name = TRUE; 
	  if (DocumentTypes[doc] == docHTML ||
	      DocumentTypes[doc] == docHTMLRO)	
	      /* it's an HTML document. It could have a source doc */
	      /* change the URL of the source document if it exists */
	     {
	     if (DocumentSource[doc])
		/* it has a source document */
		{
		TtaFreeMemory (DocumentURLs[DocumentSource[doc]]);
		DocumentURLs[DocumentSource[doc]] = TtaWCSdup (tempname);
		}
	      }
	  else if (DocumentTypes[doc] == docSource ||
		   DocumentTypes[doc] == docSourceRO)
	      {
	      /* it's a source document. Change the URL of the corresponding
		 HTML document */
	      if (htmlDoc)
		 {
		 TtaFreeMemory (DocumentURLs[htmlDoc]);
		 DocumentURLs[htmlDoc] = TtaWCSdup (tempname);
		 }
	      }
	}

      ptr = GetLocalPath (doc, DocumentURLs[doc]);
      /*  no need to protect against a null ptr, as GetLocalPath
          will always return something at this point */
      ustrcpy (localFile, ptr);
      TtaFreeMemory (ptr);
      
      /* it's a complete name: save it */
      if (ok)
	if (TextFormat)
	  {
	  ok = SaveObjectThroughNet (doc, view, DocumentURLs[doc],
				     FALSE, TRUE);
	  if (DocumentTypes[doc] == docSource ||
	      DocumentTypes[doc] == docSourceRO)
	     /* it's a source file. lines have been renumbered */
	     newLineNumbers = TRUE;
	  }
	else
	  {
	  ok = SaveDocumentThroughNet (doc, view, DocumentURLs[doc],
				       FALSE, TRUE, TRUE);
	  newLineNumbers = TRUE;
	  }
    }
  else
    /* it's a local document */
    {
      ustrcpy (localFile, tempname);
      if (TextFormat)
	if (DocumentTypes[doc] == docSource ||
	    DocumentTypes[doc] == docSourceRO)
	   /* it's a source file. renumber lines */
	   {
	   ok = TtaExportDocumentWithNewLineNumbers (doc, tempname,
						     TEXT("TextFileT"));
	   newLineNumbers = TRUE;
	   }
	else
	  {
	    ok = TtaExportDocument (doc, tempname, TEXT("TextFileT"));
	  }
      else
	{
	  SetNamespacesAndDTD (doc);
	  if (SaveAsXHTML)
	    {
	    ok = TtaExportDocumentWithNewLineNumbers (doc, tempname,
						      TEXT("HTMLTX"));
	    DocumentMeta[doc]->xmlformat = TRUE;
	    }
	  else
	    {
	    ok = TtaExportDocumentWithNewLineNumbers (doc, tempname,
						      TEXT("HTMLT"));
	    DocumentMeta[doc]->xmlformat = FALSE;
	    }
	  newLineNumbers = TRUE;
	}
      /* save a local copy of the current document */
      if (htmlDoc)
	ptr = GetLocalPath (htmlDoc, tempname);
      else
	ptr = GetLocalPath (doc, tempname);
      TtaFileCopy (tempname, ptr);
      TtaFreeMemory (ptr);
    }

  /* restore original display mode */
  TtaSetDisplayMode (doc, dispMode);

  SavingDocument = 0;
  if (newLineNumbers)
     /* line numbers have been changed in the saved document */
     if (DocumentTypes[doc] == docHTML || DocumentTypes[doc] == docHTMLRO)
        /* It's a HTML document. If the source view is open, redisplay the
	   source. */
       RedisplaySourceFile (doc);
     else if (DocumentTypes[doc] == docSource ||
	      DocumentTypes[doc] == docSourceRO)
	/* It's a source document. Reparse the corresponding HTML document */
	if (htmlDoc)
	   {
	   TtaExtractName (localFile, tempdir, documentname);
	   StartParser (htmlDoc, localFile, documentname, tempdir, localFile, FALSE);
	   /* fetch and display all images referred by the document */
	   DocNetworkStatus[htmlDoc] = AMAYA_NET_ACTIVE;
	   FetchAndDisplayImages (htmlDoc, AMAYA_LOAD_IMAGE);
	   DocNetworkStatus[htmlDoc] = AMAYA_NET_INACTIVE;
	   TtaSetDocumentUnmodified (htmlDoc);
	   /* Synchronize selections */
	   event.document = doc;
	   SynchronizeSourceView (&event);
	   }
  if (ok)
    {
      if (DocumentMeta[doc]->method == CE_TEMPLATE)
	{
	  DocumentMeta[doc]->method = CE_ABSOLUTE;
	  TtaFreeMemory (DocumentMeta[doc]->form_data);
	  DocumentMeta[doc]->form_data = NULL;
	}
      TtaSetDocumentUnmodified (doc);
      /* switch Amaya buttons and menus */
      DocStatusUpdate (doc, FALSE);
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
  CHAR_T               pathname[MAX_LENGTH];
  CHAR_T               docname[MAX_LENGTH];
  CHAR_T*              ptr;
  int                  l;

  /* check all modified documents */
  f = NULL;
  for (doc = 1; doc < DocumentTableLength; doc++)
    if (DocumentURLs[doc] != NULL && TtaIsDocumentModified (doc) && doc != W3Loading)
      {
	if (f == NULL)
	  {
	    /* open the crash file */
	    usprintf (pathname, TEXT("%s%cCrash.amaya"), TempFileDirectory, DIR_SEP);
	    f = ufopen (pathname, TEXT("w"));
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
	  usprintf (pathname, TEXT("%s%c%s.html"), TempFileDirectory, DIR_SEP, docname);
	else
	  usprintf (pathname, TEXT("%s%c%s"), TempFileDirectory, DIR_SEP, docname);

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
ThotBool            DocumentToSave (NotifyDialog * event)
#else  /* __STDC__ */
ThotBool            DocumentToSave (event)
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
static void            UpdateImages (Document doc, ThotBool src_is_local, ThotBool dst_is_local, CHAR_T* imgbase, CHAR_T* newURL)
#else
static void            UpdateImages (doc, src_is_local, dst_is_local, imgbase, newURL)
Document               doc;
ThotBool               src_is_local;
ThotBool               dst_is_local;
CHAR_T*                imgbase;
CHAR_T*                newURL;
#endif
{
   AttributeType       attrType;
   ElementType         elType;
   Attribute           attr;
   Element             el, root, content;
   LoadedImageDesc    *pImage;
   Language            lang;
   CHAR_T              tempfile[MAX_LENGTH];
   CHAR_T              localpath[MAX_LENGTH];
   CHAR_T              oldpath[MAX_LENGTH];
   CHAR_T              oldname[MAX_LENGTH];
   CHAR_T              tempname[MAX_LENGTH];
   CHAR_T              imgname[MAX_LENGTH];
   CHAR_T              url[MAX_LENGTH];
   CHAR_T              *buf, *ptr;
   CHAR_T              *sStyle, *stringStyle;
   CHAR_T*             oldStyle;
   int                 buflen, max, index;

   if (imgbase[0] != WC_EOS)
     {
       /* add the separator if needed */
       buflen = ustrlen (imgbase) - 1;
       if (dst_is_local && !IsW3Path (imgbase))
	 {
	   if (imgbase[buflen] != WC_DIR_SEP)
	     ustrcat (imgbase, WC_DIR_STR);
	 }
       else
	 {
	   if (imgbase[buflen] != WC_URL_SEP)
	     ustrcat (imgbase, WC_URL_STR);
	 }
     }

   /* save the old document path to locate existing images */
   ustrcpy (oldpath, DocumentURLs[doc]);
   buflen = ustrlen (oldpath) - 1;
   if (oldpath[buflen] ==  TEXT('/'))
     oldpath[buflen] = WC_EOS;
   /* path to search image descriptors */
   usprintf (localpath, TEXT("%s%s%d%s"), TempFileDirectory, WC_DIR_STR, doc, WC_DIR_STR);

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
	   tempname[0] = WC_EOS;
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
		   stringStyle = ustrstr (stringStyle, TEXT("url")) + 3;
		   ustrcpy (url, ptr);
		   TtaFreeMemory (ptr);
		   NormalizeURL (url, 0, tempname, imgname, newURL);

		   /* extract the URL from the old style string */
		   ptr = GetCSSBackgroundURL (oldStyle);
		   if (ptr != NULL)
		     {
		       /* for next research */
		       oldStyle = ustrstr (oldStyle, TEXT("url")) + 3;
		       NormalizeURL (ptr, 0, oldname, imgname, oldpath);
		       TtaFreeMemory (ptr);

		       /*
			 At this point:
			 - url gives the relative new image name
			 - tempname gives the new image full name
			 - oldname gives the old image full name
			 - imgname contains the image file name
			 */
		       if (url[0] != WC_EOS && oldname[0] != WC_EOS)
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
			       
			       if (imgbase[0] != WC_EOS)
				 {
				   ustrcpy (tempfile, imgbase);
				   ustrcat (tempfile, WC_DIR_STR);
				   ustrcat (tempfile, imgname);
				 }
			       else
				 {
				   ustrcpy (tempfile, SavePath);
				   ustrcat (tempfile, WC_DIR_STR);
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
				       pImage->originalName = TtaWCSdup (tempname);
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
		       buf = TtaAllocString (buflen);
		       if (buf == NULL)
			 break;
		       TtaGiveTextAttributeValue (attr, buf, &buflen);
		       if (attrType.AttrTypeNum == HTML_ATTR_Style_)
			 {
			   /* It's an attribute Style: look for url()*/
			   url[0] = WC_EOS;
			   tempname[0] = WC_EOS;
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
				 buf[0] = WC_EOS;
			     }
			 }
		       else
			 {
			   /* extract the old image name and location */
			   ustrcpy (url, buf);
			   NormalizeURL (url, 0, buf, imgname, oldpath);
			   /* save the new SRC attr value */
			   if (imgbase[0] != WC_EOS)
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
		       if (url[0] != WC_EOS && buf[0] != WC_EOS)
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

			       if (imgbase[0] != WC_EOS)
				   ustrcpy (tempfile, tempname);
			       else
				 {
				   ustrcpy (tempfile, SavePath);
				   ustrcat (tempfile, WC_DIR_STR);
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
				       pImage->originalName = TtaWCSdup (tempname);
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
  STRING              tempname, oldLocal, newLocal;
  STRING              imagePath, base;
  CHAR_T              imgbase[MAX_LENGTH];
  CHAR_T              url_sep;
  int                 res;
  int                 len;
  DisplayMode         dispMode;
  ThotBool            src_is_local;
  ThotBool            dst_is_local, ok;
  ThotBool	      docModified, toUndo;
  ThotBool            new_put_def_name;
  ThotBool            old_put_def_name;

  src_is_local = !IsW3Path (DocumentURLs[SavingDocument]);
  dst_is_local = !IsW3Path (SavePath);
  ok = TRUE;
  toUndo = FALSE;
  base = NULL;

#ifdef AMAYA_DEBUG
  fprintf(stderr, "DoSaveAs : from %s to %s/%s , with images %d\n", DocumentURLs[SavingDocument], SavePath, SaveName, (int) CopyImages);
#endif

  /* New document path */
  documentFile = TtaAllocString (MAX_LENGTH);
  ustrcpy (documentFile, SavePath);
  len = ustrlen (documentFile);
  if (documentFile [len -1] != WC_DIR_SEP && documentFile [len - 1] != TEXT('/'))
    {
     if (dst_is_local)
       {
	 ustrcat (documentFile, WC_DIR_STR);
	 url_sep = WC_DIR_SEP;
       }
     else
       {
	 ustrcat (documentFile, WC_URL_STR);
	 url_sep = WC_URL_SEP;
       }
    }
  else if (dst_is_local)
    url_sep = WC_DIR_SEP;
  else
    url_sep = WC_URL_SEP;

  new_put_def_name = FALSE;
  if (SaveName[0] == WC_EOS)
    {
      /* there is no document name */
      if (AddNoName (SavingDocument, 1, documentFile, &ok))
	{
	  ok = TRUE;
	  res = ustrlen(SavePath) - 1;
	  if (SavePath[res] == url_sep)
	    SavePath[res] = WC_EOS;
	  /* need to update the document url */
	  ustrcpy (SaveName, DefaultName);
	  ustrcat (documentFile, SaveName);
	  /* set up a temp flag to say we're using the default name */
	  new_put_def_name = TRUE;
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
	  tempname = TtaAllocString (MAX_LENGTH);
	  usprintf (tempname, TtaGetMessage (LIB, TMSG_FILE_EXIST), documentFile);
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
	      tempname = TtaAllocString (MAX_LENGTH);
	      if (imgbase[0] != DIR_SEP)
		  {
		    ustrcpy (tempname, SavePath);
		    ustrcat (tempname, WC_DIR_STR);
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
      old_put_def_name = DocumentMeta[doc]->put_default_name;
      docModified = TtaIsDocumentModified (doc);
      /* name of local temporary files */
      oldLocal = GetLocalPath (doc, DocumentURLs[doc]);
      newLocal = GetLocalPath (doc, SavePath);

      /* change display mode to avoid flicker due to callbacks executed when
	 saving some elements, for instance META */
      dispMode = TtaGetDisplayMode (doc);
      if (dispMode == DisplayImmediately)
	TtaSetDisplayMode (doc, DeferredDisplay);

      if (TextFormat)
	{
	  if (dst_is_local)
	    {
	      /* Local to Local or Remote to Local */
	      /* save the local document */
	      ok = TtaExportDocument (doc, documentFile, TEXT("TextFileT"));
	    }
	  else
	    {
	      /* update the flag that says if we're using a default name */
	      DocumentMeta[doc]->put_default_name = new_put_def_name;
	      ok = SaveObjectThroughNet (doc, 1, documentFile, TRUE, TRUE);
	    }
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
	   * picture SRC attribute. If pictures are saved locally, make the
	   * copy else add them to the list of remote images to be copied.
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
	      /* update the flag that says if we're using a default name */
	      DocumentMeta[doc]->put_default_name = new_put_def_name;
	      ok = SaveDocumentThroughNet (doc, 1, documentFile, TRUE,
					   CopyImages, FALSE);
	    }
	}

      /* restore original display mode */
      TtaSetDisplayMode (doc, dispMode);

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
	      DocumentURLs[doc] = TtaWCSdup (documentFile);
	      TtaSetTextZone (doc, 1, 1, DocumentURLs[doc]);
	      if (DocumentSource[doc])
		{
	          TtaFreeMemory (DocumentURLs[DocumentSource[doc]]);
	          DocumentURLs[DocumentSource[doc]] = TtaWCSdup (documentFile);
		}
	      if (DocumentMeta[doc]->method == CE_TEMPLATE)
		{
		  DocumentMeta[doc]->method = CE_ABSOLUTE;
		  TtaFreeMemory (DocumentMeta[doc]->form_data);
		  DocumentMeta[doc]->form_data = NULL;
		}
	      TtaSetDocumentUnmodified (doc);
	      /* switch Amaya buttons and menus */
	      DocStatusUpdate (doc, FALSE);
	    }
	  /* if it's a HTML document and the source view is open, redisplay
	     the source. */
	  RedisplaySourceFile (doc);
	  /* Sucess of the operation */
	  TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_SAVED), documentFile);
	  /* remove the previous temporary file */
	  if (oldLocal && !SaveAsText && ustrcmp (oldLocal, newLocal))
	    /* free the previous temporary file */
	    TtaFileUnlink (oldLocal);
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
	    {
	      TtaSetDocumentUnmodified (doc);
	      /* switch Amaya buttons and menus */
	      DocStatusUpdate (doc, docModified);
	    }
	  DocumentMeta[doc]->put_default_name = old_put_def_name;
	  /* propose to save a second time */
	  SaveDocumentAs(doc, 1);
	}
      TtaFreeMemory (oldLocal);
      TtaFreeMemory (newLocal);
    }
  TtaFreeMemory (documentFile);
}
