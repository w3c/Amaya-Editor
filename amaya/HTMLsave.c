/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996-2001
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Amaya saving functions.
 *
 * Authors: I. Vatton, D. Veillard, J. Kahan
 *
 */

/* DEBUG_AMAYA_SAVE Print out debug information when saving */

/* Included headerfiles */
#define THOT_EXPORT extern
#include "amaya.h"
#include "parser.h"
#include "css.h"
#include "XLink.h"
#include "MathML.h"
#ifdef GRAPHML
#include "GraphML.h"
#endif

#include "HTMLhistory_f.h"
#ifdef ANNOTATIONS
#include "annotlib.h"
#include "ANNOTevent_f.h"
#endif /* ANNOTATIONS */

typedef struct _AttSearch
{
    int   att;
    int   type;
} AttSearch;

#ifdef _WINDOWS
#include "resource.h"
static char       currentDocToSave[MAX_LENGTH];
static char       currentPathName[MAX_LENGTH];
extern HINSTANCE    hInstance;
#endif /* _WINDOWS */


#define StdDefaultName "Overview.html"
static char        *DefaultName;
static char         tempSavedObject[MAX_LENGTH];
static ThotBool     TextFormat;
/* list attributes checked for updating URLs */
static AttSearch    URL_attr_tab[] = {
   {HTML_ATTR_HREF_, XHTML_TYPE},
   {HTML_ATTR_codebase, XHTML_TYPE},
   {HTML_ATTR_Script_URL, XHTML_TYPE},
   {HTML_ATTR_SRC, XHTML_TYPE},
   {HTML_ATTR_data, XHTML_TYPE},
   {HTML_ATTR_background_, XHTML_TYPE},
   {HTML_ATTR_Style_, XHTML_TYPE},
   {HTML_ATTR_cite, XHTML_TYPE},
   {XLink_ATTR_href_, XLINK_TYPE},
   {MathML_ATTR_style_, MATH_TYPE},
#ifdef GRAPHML
   {GraphML_ATTR_style_, GRAPH_TYPE},
   {GraphML_ATTR_xlink_href, GRAPH_TYPE}
#endif
};
/* list of attributes checked for updating images */
static AttSearch    SRC_attr_tab[] = {
   {HTML_ATTR_SRC, XHTML_TYPE},
   {HTML_ATTR_data, XHTML_TYPE},
   {HTML_ATTR_background_, XHTML_TYPE},
   {HTML_ATTR_Style_, XHTML_TYPE},
   {MathML_ATTR_style_, MATH_TYPE},
#ifdef GRAPHML
   {GraphML_ATTR_style_, GRAPH_TYPE},
   {GraphML_ATTR_xlink_href, GRAPH_TYPE}
#endif
};
static char        *QuotedText;


static char *HTMLDocTypes_1[] =
{
  "\"-//W3C//DTD XHTML Basic 1.0//EN\"",
  "\"-//W3C//DTD XHTML 1.0 Strict//EN\"",
  "\"-//W3C//DTD XHTML 1.1//EN\"",
  "\"-//W3C//DTD XHTML 1.0 Transitional//EN\"",
  "\"-//W3C//DTD XHTML 1.0 Frameset//EN\"",
  "\"-//W3C//DTD HTML 4.01//EN\"",
  "\"-//W3C//DTD HTML 4.01 Transitional//EN\"",
  "\"-//W3C//DTD HTML 4.01 Frameset//EN\""
};

static char *HTMLDocTypes_2[] =
{
  "\"http://www.w3.org/TR/xhtml-basic/xhtml-basic10.dtd\">\n",
  "\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n",
  "\"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">\n",
  "\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n",
  "\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-frameset.dtd\">\n",
  "\"http://www.w3.org/TR/html4/strict.dtd\">\n",
  "\"http://www.w3.org/TR/html4/loose.dtd\">\n",
  "\"http://www.w3.org/TR/html4/frameset.dtd\">\n",
};

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
#include "Xml2thot_f.h"

#ifdef _WINDOWS
#include "wininclude.h"

/*-----------------------------------------------------------------------
 SaveAsDlgProc
 ------------------------------------------------------------------------*/
LRESULT CALLBACK GetSaveDlgProc (HWND hwnDlg, UINT msg, WPARAM wParam,
				 LPARAM lParam)
{
  static char txt [500];
  
  switch (msg)
    {
    case WM_INITDIALOG:
      SetWindowText (hwnDlg, TtaGetMessage (AMAYA, AM_SAVE_AS));
      SetWindowText (GetDlgItem (hwnDlg, ID_CONFIRM),
		     TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
      SetWindowText (GetDlgItem (hwnDlg, IDC_BROWSE), "Browse");
      SetWindowText (GetDlgItem (hwnDlg, IDCANCEL),
		     TtaGetMessage (LIB, TMSG_CANCEL));
      SetDlgItemText (hwnDlg, IDC_EDITDOCSAVE, currentPathName);
      break;
      
    case WM_COMMAND:
      if (HIWORD (wParam) == EN_UPDATE)
	{
	  if (LOWORD (wParam) == IDC_EDITDOCSAVE)
	    GetDlgItemText (hwnDlg, IDC_EDITDOCSAVE, currentDocToSave,
			    sizeof (currentDocToSave) - 1);
	}
      switch (LOWORD (wParam))
	{
	case IDC_BROWSE:
	  WIN_ListSaveDirectory (BaseDialog + SaveForm,
				 TtaGetMessage (AMAYA, AM_SAVE_AS),
				 currentDocToSave);
	  SetDlgItemText (hwnDlg, IDC_EDITDOCSAVE, currentDocToSave);
	  break;

	case IDCANCEL:
	  EndDialog (hwnDlg, IDCANCEL);
	  ThotCallback (BaseDialog + SaveForm, INTEGER_DATA, (char *) 0);
	  break;

	case ID_CONFIRM:
	  /* TODO: Extract directory and file name from urlToOpen */
	  EndDialog (hwnDlg, ID_CONFIRM);
	  strcpy (currentDocToSave, LastURLName);
	  /*TtaExtractName (currentDocToSave, SavePath, ObjectName);*/
	  ThotCallback (BaseDialog + SaveForm, INTEGER_DATA, (char *) 1);
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
void CreateGetSaveDlgWindow (HWND parent, char *path_name)
{  
  sprintf (currentPathName, path_name);
  DialogBox (hInstance, MAKEINTRESOURCE (GETSAVEDIALOG), parent,
	     (DLGPROC) GetSaveDlgProc);
}
#endif /* _WINDOWS */

/*----------------------------------------------------------------------
  CheckGenerator                                                 
  ----------------------------------------------------------------------*/
ThotBool CheckGenerator (NotifyElement *event)
{
  AttributeType      attrType;
  Attribute          attr;
  char               buff[MAX_LENGTH];
  char              *ptr;
  int                length;

  attrType.AttrSSchema = TtaGetDocumentSSchema (event->document);
  attrType.AttrTypeNum = HTML_ATTR_meta_name;
  attr = TtaGetAttribute (event->element, attrType);
  if (attr != 0)
    {
      length = MAX_LENGTH - 1;
      TtaGiveTextAttributeValue (attr, buff, &length);
      if (!strcasecmp (buff, "GENERATOR"))
	{
	  /* is it Amaya generator ? */
	  attrType.AttrTypeNum = HTML_ATTR_meta_content;
	  attr = TtaGetAttribute (event->element, attrType);
	  if (attr != 0)
	    {
	      length = MAX_LENGTH - 1;
	      TtaGiveTextAttributeValue (attr, buff, &length);
	      ptr = strstr (buff, "amaya");
	      if (ptr == NULL)
		ptr = strstr (buff, "Amaya");
	      if (ptr == NULL)
		/* it's not a pure Amaya document -> remove the meta element */
		return TRUE;
	      else
		{
		  /* update the version */
		  strcpy (buff, HTAppName); 
		  strcat (buff, " ");
		  strcat (buff, HTAppVersion);
		  TtaSetAttributeText (attr, buff, event->element,
				       event->document);
		}
	    }
	}
    }
  return FALSE;  /* let Thot perform normal operation */
}


/*----------------------------------------------------------------------
  GenerateQuoteBefore                                                  
  ----------------------------------------------------------------------*/
ThotBool GenerateQuoteBefore (NotifyAttribute *event)
{
  char             *ptr;
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
  TtaSetAttributeText (event->attribute, QuotedText, event->element,
		       event->document);
  /* do NOT free the QuotedText string because it's used by
     GenerateQuoteAfter */
  return FALSE;  /* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
  GenerateQuoteAfter                                                 
  ----------------------------------------------------------------------*/
void GenerateQuoteAfter (NotifyAttribute *event)
{
  /* remove quotes before and after the text */
  QuotedText[strlen (QuotedText) - 1] = EOS;
  TtaSetAttributeText (event->attribute, &QuotedText[1], event->element,
		       event->document);
  TtaFreeMemory (QuotedText);
}

/*----------------------------------------------------------------------
  CheckValidID
  A NAME attribute is about to be saved. If the output format is XML
  and the current element does not have an ID attribute, check if
  the value of the NAME attribute is a valid XML ID and if not,
  generate an ID attribute with a valid value.
  ----------------------------------------------------------------------*/
ThotBool CheckValidID (NotifyAttribute *event)
{
  AttributeType     attrType;
  Attribute         attr;
  char             *value;
  int               length, i;

  if (!SaveAsXML)
     /* we are not saving the document in XML */
     return FALSE;  /* let Thot perform normal operation */

  attrType = event->attributeType;
  attrType.AttrTypeNum = HTML_ATTR_ID;
  attr = TtaGetAttribute (event->element, attrType);
  if (!attr)
     /* this element does not have an ID attribute */
     {
     /* get the value of the NAME attribute */
     length = TtaGetTextAttributeLength (event->attribute);
     length+= 10;
     value = TtaGetMemory (length);
     length--;
     TtaGiveTextAttributeValue (event->attribute, &value[1], &length);
     if (value[1] >= '0' && value[1] <= '9')
        /* the value of the NAME attribute starts with a digit */
        {
	/* insert an underscore at the beginning and create an ID attribute
	   with that value */
	value[0] = '_';
	length++;
	/* check that this value is not already used by another ID attribute
           in the document and add a number at the end if it's the case */
        i = 0;
	while (SearchNAMEattribute (event->document, value, NULL))
	   /* this value is already used in the document */
	   {
	   i++;
	   sprintf (&value[length], "%d", i);
	   }
	/* Create the ID attr. */
	attr = TtaNewAttribute (attrType);
	TtaAttachAttribute (event->element, attr, event->document);
	TtaSetAttributeText (attr, value, event->element, event->document);
	}
     TtaFreeMemory (value);
     }
  return FALSE;  /* let Thot perform normal operation */
}


/*----------------------------------------------------------------------
   SetRelativeURLs: try to make relative URLs within an HTML document.
  ----------------------------------------------------------------------*/
void SetRelativeURLs (Document doc, char *newpath)
{
  SSchema             XHTMLSSchema, MathSSchema, GraphSSchema, XLinkSSchema;
  Element             el, root, content;
  ElementType         elType;
  Attribute           attr;
  AttributeType       attrType;
  Language            lang;
  char                old_url[MAX_LENGTH];
  char                oldpath[MAX_LENGTH];
  char                tempname[MAX_LENGTH];
  char               *new_url;
  int                 index, max;
  int                 len;

#ifdef AMAYA_DEBUG
  fprintf(stderr, "SetRelativeURLs\n");
#endif
  XHTMLSSchema = TtaGetSSchema ("HTML", doc);
  MathSSchema = TtaGetSSchema ("MathML", doc);
  GraphSSchema = TtaGetSSchema ("GraphML", doc);
  XLinkSSchema = TtaGetSSchema ("XLink", doc);
  root = TtaGetMainRoot (doc);

  /* handle style elements */
  elType = TtaGetElementType (root);
  if (elType.ElSSchema == XHTMLSSchema || elType.ElSSchema == GraphSSchema)
    {
      if (elType.ElSSchema == XHTMLSSchema)
	elType.ElTypeNum = HTML_EL_STYLE_;
      else if (elType.ElSSchema == GraphSSchema)
	elType.ElTypeNum = GraphML_EL_style__;
      el = TtaSearchTypedElement (elType, SearchInTree, root);
    }
  else
    el = NULL;
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
      new_url = UpdateCSSBackgroundImage (DocumentURLs[doc], newpath,
					  NULL, CSSbuffer);
      if (new_url != NULL)
	{
	  /* register the modification to be able to undo it */
	  TtaRegisterElementReplace (content, doc);
	  TtaSetTextContent (content, new_url, lang, doc);
	  TtaFreeMemory (new_url);
	}
      }
    TtaNextSibling (&el);
    if (el != NULL)
       elType = TtaGetElementType (el);
    }

  /* manage URLs and SRCs attributes */
  max = sizeof (URL_attr_tab) / sizeof (AttSearch);
  for (index = 0; index < max; index++)
    {
      /* search all elements having this attribute */
      attrType.AttrTypeNum = URL_attr_tab[index].att;
      switch (URL_attr_tab[index].type)
	{
	case XHTML_TYPE:
	  attrType.AttrSSchema = XHTMLSSchema;
	  break;
	case MATH_TYPE:
	  attrType.AttrSSchema = MathSSchema;
	  break;
	case GRAPH_TYPE:
	  attrType.AttrSSchema = GraphSSchema;
	  break;
	case XLINK_TYPE:
	  attrType.AttrSSchema = XLinkSSchema;
	  break;
	default:
	  attrType.AttrSSchema = NULL;
	}
      if (attrType.AttrSSchema)
	TtaSearchAttribute (attrType, SearchForward, root, &el, &attr);
      else
	el = NULL;
      while (el && attr)
	{
	  elType = TtaGetElementType (el);
	  if (elType.ElTypeNum != HTML_EL_BASE || elType.ElSSchema != XHTMLSSchema)
	    {
	      /* get the URL contained in the attribute. */
	      len = MAX_LENGTH - 1;
	      TtaGiveTextAttributeValue (attr, old_url, &len);
	      old_url[MAX_LENGTH - 1] = EOS;
	      if ((attrType.AttrTypeNum == HTML_ATTR_Style_ &&
		   attrType.AttrSSchema == XHTMLSSchema) ||
		  (attrType.AttrTypeNum == MathML_ATTR_style_ &&
		   attrType.AttrSSchema == MathSSchema) ||
		  (attrType.AttrTypeNum == GraphML_ATTR_style_  &&
		   attrType.AttrSSchema == GraphSSchema))
		{
		  /* manage background-image rule within style attribute */
		  new_url = UpdateCSSBackgroundImage (DocumentURLs[doc],
						      newpath, NULL, old_url);
		  if (new_url != NULL)
		    {
		      /* register the modification to be able to undo it */
		      TtaRegisterAttributeReplace (attr, el, doc);
		      TtaSetAttributeText (attr, new_url, el, doc);
		      TtaFreeMemory (new_url);
		    }
		}
	      /* save the new attribute value */
	      else if (old_url[0] != '#')
		{
		  NormalizeURL (old_url, doc, oldpath, tempname, NULL);
		  new_url = MakeRelativeURL (oldpath, newpath);
#ifdef AMAYA_DEBUG
		  fprintf(stderr, "Changed URL from %s to %s\n", old_url,
			  new_url);
#endif
		  /* register the modification to be able to undo it */
		  TtaRegisterAttributeReplace (attr, el, doc);
                  /* save the new attribute value */
		  TtaSetAttributeText (attr, new_url, el, doc);
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
static void InitSaveForm (Document document, View view, char *pathname)
{
#ifndef _WINDOWS
   char             buffer[3000];
   char             s[MAX_LENGTH];
   int              i;
#endif /* WINDOWS */

  if (TextFormat)
     {
       SaveAsHTML = FALSE;
       SaveAsXML = FALSE;
       SaveAsText = TRUE;
     }
  else if (IsXMLName (pathname) || DocumentMeta[document]->xmlformat ||
	   (!DocumentMeta[document]->xmlformat && 
	    (ParsingLevel[document] == L_Basic ||
	     ParsingLevel[document] == L_Xhtml11 ||
	     ParsingLevel[document] == L_Strict)))
    {
      SaveAsHTML = FALSE;
      SaveAsXML = TRUE;
      SaveAsText = FALSE;
    }
  else
    {
      SaveAsHTML = TRUE;
      SaveAsXML = FALSE;
      SaveAsText = FALSE;
    }
  
#ifndef _WINDOWS
   /* destroy any previous instance of the Save as form */
   TtaDestroyDialogue (BaseDialog + SaveForm);
   
   /* dialogue form for saving a document */
   i = 0;
   strcpy (&s[i], TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
   i += strlen (&s[i]) + 1;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_BROWSE));
   i += strlen (&s[i]) + 1;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_CLEAR));
   i += strlen (&s[i]) + 1;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_CHANGE_CHARSET));
   i += strlen (&s[i]) + 1;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_CHANGE_MIME_TYPE));
   TtaNewSheet (BaseDialog + SaveForm, TtaGetViewFrame (document, view), 
		TtaGetMessage (AMAYA, AM_SAVE_AS), 5, s, TRUE, 2, 'L',
		D_CANCEL);

   /* first line */
   if (!TextFormat && DocumentTypes[document] != docMath &&
       DocumentTypes[document] != docSVG)
     {
       /* choice between html, xhtml and text */
       sprintf (buffer, "%s%c%s%c%s", "BHTML", EOS, "BXML", EOS,
		"BText");
       TtaNewSubmenu (BaseDialog + RadioSave, BaseDialog + SaveForm, 0,
			 TtaGetMessage (LIB, TMSG_DOCUMENT_FORMAT), 3, buffer,
			 NULL, TRUE);
       if (SaveAsHTML)
	 TtaSetMenuForm (BaseDialog + RadioSave, 0);
       else if (SaveAsXML)
	 TtaSetMenuForm (BaseDialog + RadioSave, 1);
       else
	 TtaSetMenuForm (BaseDialog + RadioSave, 2);
     }
   else
     TtaNewLabel (BaseDialog + RadioSave, BaseDialog + SaveForm, "");
   TtaNewTextForm (BaseDialog + NameSave, BaseDialog + SaveForm,
		   TtaGetMessage (AMAYA, AM_DOC_LOCATION), 50, 1, FALSE);
   TtaSetTextForm (BaseDialog + NameSave, pathname);
   /* second line */
   if (!TextFormat)
     {
       /* choice between html, xhtml and text */
       sprintf (buffer, "B%s%cB%s",
		TtaGetMessage (AMAYA, AM_BCOPY_IMAGES), EOS,
		TtaGetMessage (AMAYA, AM_BTRANSFORM_URL));
       TtaNewToggleMenu (BaseDialog + ToggleSave, BaseDialog + SaveForm,
			 TtaGetMessage (LIB, TMSG_OPTIONS), 2, buffer,
			 NULL, TRUE);
       if (DocumentTypes[document] == docMath)
	 TtaRedrawMenuEntry (BaseDialog + ToggleSave, 0, NULL, -1, FALSE);
       else
	 TtaSetToggleMenu (BaseDialog + ToggleSave, 0, CopyImages);
       TtaSetToggleMenu (BaseDialog + ToggleSave, 1, UpdateURLs);
     }
   else
     TtaNewLabel (BaseDialog + ToggleSave, BaseDialog + SaveForm, "");

   if (!TextFormat && DocumentTypes[document] != docMath)
     {
       TtaNewTextForm (BaseDialog + ImgDirSave, BaseDialog + SaveForm,
		   TtaGetMessage (AMAYA, AM_IMAGES_LOCATION), 50, 1, FALSE);
       TtaSetTextForm (BaseDialog + ImgDirSave, SaveImgsURL);
     }
   else
      TtaNewLabel (BaseDialog + ImgDirSave, BaseDialog + SaveForm, "");
    
   /* third line */
   TtaNewLabel (BaseDialog + CharsetSaveL, BaseDialog + SaveForm,
		"Charset:  ");
   TtaNewLabel (BaseDialog + CharsetSave,  BaseDialog + SaveForm, 
		UserCharset[0] != EOS ? UserCharset : "UNKNOWN");
   /* fourth line */
   TtaNewLabel (BaseDialog + MimeTypeSaveL, BaseDialog + SaveForm, 
		"MIME type:");
   TtaNewLabel (BaseDialog + MimeTypeSave,  BaseDialog + SaveForm, 
		UserMimeType[0] != EOS ? UserMimeType : "UNKNOWN");
   /* fifth line */
   TtaNewLabel (BaseDialog + SaveFormStatus, BaseDialog + SaveForm, 
		" ");

   TtaShowDialogue (BaseDialog + SaveForm, TRUE);
#else /* _WINDOWS */
   CreateSaveAsDlgWindow (TtaGetViewFrame (document, view), pathname);
#endif /* _WINDOWS */
}


/*----------------------------------------------------------------------
  InitSaveObjectForm
  ----------------------------------------------------------------------*/
void InitSaveObjectForm (Document document, View view, char *object,
			 char *pathname)
{
#ifndef _WINDOWS
   char                tempdir[MAX_LENGTH];
#endif /* _WINDOWS */

   if (SavingDocument != 0 || SavingObject != 0)
     return;
   SavingObject = document;
   strncpy (tempSavedObject, object, sizeof (tempSavedObject));

#ifndef _WINDOWS
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
#else  /* _WINDOWS */
   CreateGetSaveDlgWindow (TtaGetViewFrame (document, view), pathname);
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
  DeleteTempObjectFile
  ----------------------------------------------------------------------*/
void DeleteTempObjectFile (void)
{
   TtaFileUnlink (tempSavedObject);
}


/*----------------------------------------------------------------------
  DoSaveObjectAs
  ----------------------------------------------------------------------*/
void DoSaveObjectAs (void)
{
   char           tempfile[MAX_LENGTH];
   char           msg[MAX_LENGTH];
   ThotBool       dst_is_local;
   int            res;
   
   if (SavingObject == 0)
     return;

   /* @@ JK Testing to see if this part of the function is used elsewhere */
#if 0
   dst_is_local = !IsW3Path (SavePath);
#else
   dst_is_local = FALSE;
#endif

   strcpy (tempfile, SavePath);
   strcat (tempfile, DIR_STR);
   strcat (tempfile, ObjectName);

   if (!dst_is_local)
     {
	/* @@ We need to check the use of AMAYA_PREWRITE_VERIFY in this function*/
       /* @@ JK: add mime type  */
       res = PutObjectWWW (SavingObject, tempSavedObject, tempfile, NULL,
			   AMAYA_SYNC | AMAYA_NOCACHE |  AMAYA_FLUSH_REQUEST 
			   | AMAYA_USE_PRECONDITIONS, NULL, NULL);
       if (res)
	 {
#ifndef _WINDOWS
	   TtaSetDialoguePosition ();
	   TtaShowDialogue (BaseDialog + SaveForm, FALSE);
#endif /* !_WINDOWS */
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
#ifndef _WINDOWS
	    TtaSetDialoguePosition ();
	    TtaShowDialogue (BaseDialog + SaveForm, FALSE);
#endif /* !_WINDOWS */
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
void SaveDocumentAs (Document doc, View view)
{
   char           tempname[MAX_LENGTH];
   int            i;

   /* Protection against multiple invocations of this function */
   if ((SavingDocument != 0 && SavingDocument != doc) ||
       SavingObject != 0)
      return;

   TextFormat = (DocumentTypes[doc] == docText ||
		 DocumentTypes[doc] == docCSS ||
		 DocumentTypes[doc] == docSource);

   /*
   ** initialize the user MIME type and charset global variables 
   */

   /* if there's no MIME type for this document, infer one */
   if (DocumentMeta[doc] && DocumentMeta[doc]->content_type)
     strcpy (UserMimeType, DocumentMeta[doc]->content_type);
   else if (DocumentTypes[doc] == docImage)
     strcpy (UserMimeType, DocImageMimeType (doc));
   else
     {
       if (DocumentTypes[doc] == docHTML)
	 strcpy (UserMimeType, "text/html");
       else if (DocumentTypes[doc] == docText)
	 strcpy (UserMimeType, "text/plain");
       else if (DocumentTypes[doc] == docSVG)
	 strcpy (UserMimeType, "text/xml+svg");
       else if (DocumentTypes[doc] == docMath)
	 strcpy (UserMimeType, "text/xml+mathml");
       else if (DocumentTypes[doc] == docXml)
	 strcpy (UserMimeType, "text/xml");
       else
	 UserMimeType[0] = EOS;
     }

   /* charset */
   if (DocumentMeta[doc] && DocumentMeta[doc]->charset)
     strcpy (UserCharset, DocumentMeta[doc]->charset);
   else
     UserCharset[0] = EOS;

   /* memorize the current document */
   if (SavingDocument == 0)
     {
       SavingDocument = doc;
       strcpy (tempname, DocumentURLs[doc]);
       /* suppress compress suffixes from tempname */
       i = strlen (tempname) - 1;
       if (i > 2 && !strcmp (&tempname[i-2], ".gz"))
	 {
	   tempname[i-2] = EOS;
	   TtaFreeMemory (DocumentURLs[doc]);
	   DocumentURLs[doc] = TtaStrdup (tempname);
	 }
       else if (i > 1 && !strcmp (&tempname[i-1], ".Z"))
	 {
	   tempname[i-1] = EOS;
	   TtaFreeMemory (DocumentURLs[doc]);
	   DocumentURLs[doc] = TtaStrdup (tempname);
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
		 strcpy (SaveName, DefaultName);
		 strcat (tempname, SaveName);
	       }

	   /* add the suffix .html for HTML documents */
	   if (!TextFormat &&
	       DocumentTypes[SavingDocument] != docMath &&
	       DocumentTypes[SavingDocument] != docSVG &&
	       !IsHTMLName (SaveName) && !IsXMLName (SaveName))
	     {
	       strcat (SaveName, ".html");
	       strcpy (tempname, SavePath);
	       strcat (tempname, URL_STR);
	       strcat (tempname, SaveName);
 	     }
	 }
       else
	 {
	   TtaExtractName (tempname, SavePath, SaveName);
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
   InitSaveForm (doc, 1, tempname);
}

/*----------------------------------------------------------------------
   SetNamespacesAndDTD
   Whatever the document type, set the content of the Charset attribute
   (on the root element) according to the encoding used in the document.
   For (X)HTML documents, set the content of the Namespaces attribute
   (on the root element) according to the SSchemas used in the document;
   set the HtmlDTD attribute to Frameset if the document uses Frames;
   create a META element to specify Content-Type and Charset.
  ----------------------------------------------------------------------*/
void SetNamespacesAndDTD (Document doc)
{
   Element		root, el, head, meta, docEl;
   ElementType		elType;
   AttributeType	attrType;
   Attribute		attr, charsetAttr;
   SSchema              nature;
   CHARSET              charset;
   char                *ptr;
#define MAX_CHARSET_LEN 50
   char                 Charset[MAX_CHARSET_LEN];
   char		        buffer[200];
   ThotBool		useMathML, useGraphML, useFrames;
   int                  oldStructureChecking;

   root = TtaGetRootElement (doc);
   if (DocumentTypes[doc] == docHTML)
     {
     useMathML = FALSE;
     useGraphML = FALSE;
     /* look for all natures used in the document */
     nature = NULL;
     do
       {
	 TtaNextNature (doc, &nature);
	 if (nature)
	   {
	     ptr = TtaGetSSchemaName (nature);
             if (!strcmp (ptr, "MathML"))
	       useMathML = TRUE;
	     if (!strcmp (ptr, "GraphML"))
	       useGraphML = TRUE;
	   }
       }
     while (nature);

     attrType.AttrSSchema = TtaGetSSchema ("HTML", doc);
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

     attrType.AttrTypeNum = HTML_ATTR_Namespaces;
     attr = TtaGetAttribute (root, attrType);
     if (!useMathML && !useGraphML)
       {
       /* delete the Namespaces attribute */
       if (attr)
	 TtaRemoveAttribute (root, attr, doc);

       /* generating the DOCTYPE */
       buffer[0] = EOS;
       attrType.AttrTypeNum = HTML_ATTR_HtmlDTD;
       attr = TtaGetAttribute (root, attrType);
       if (!attr)
	 {
	   attr = TtaNewAttribute (attrType);
	   TtaAttachAttribute (root, attr, doc);
	 }

	strcat (buffer, "<!DOCTYPE html PUBLIC ");
	if (DocumentMeta[doc]->xmlformat)
	  {
	    /* xml format */
	    if (useFrames)
	      {
		strcat (buffer, HTMLDocTypes_1[4]);
		strcat (buffer, "\n    ");
		strcat (buffer, HTMLDocTypes_2[4]);
	      }
	    else if (ParsingLevel[doc] == L_Basic)
	      {
		strcat (buffer, HTMLDocTypes_1[0]);
		strcat (buffer, "\n    ");
		strcat (buffer, HTMLDocTypes_2[0]);
	      }
	    else if (ParsingLevel[doc] == L_Strict)
	      {
		strcat (buffer, HTMLDocTypes_1[1]);
		strcat (buffer, "\n    ");
		strcat (buffer, HTMLDocTypes_2[1]);
	      }
	    else if (ParsingLevel[doc] == L_Xhtml11)
	      {
		strcat (buffer, HTMLDocTypes_1[2]);
		strcat (buffer, "\n    ");
		strcat (buffer, HTMLDocTypes_2[2]);
	      }
	    else
	      {
		strcat (buffer, HTMLDocTypes_1[3]);
		strcat (buffer, "\n    ");
		strcat (buffer, HTMLDocTypes_2[3]);
	      }
	  }
	else
	  {
	    if (useFrames)
	      {
		strcat (buffer, HTMLDocTypes_1[7]);
		strcat (buffer, "\n    ");
		strcat (buffer, HTMLDocTypes_2[7]);
	      }
	    else if (ParsingLevel[doc] == L_Strict)
	      {
		strcat (buffer, HTMLDocTypes_1[5]);
		strcat (buffer, "\n    ");
		strcat (buffer, HTMLDocTypes_2[5]);
	      }
	    else
	      {
		strcat (buffer, HTMLDocTypes_1[6]);
		strcat (buffer, "\n    ");
		strcat (buffer, HTMLDocTypes_2[6]);
	      }
	  }
	TtaSetAttributeText (attr, buffer, root, doc);
       }
     else if (useMathML || useGraphML)
       {
       /* prepare the value of attribute Namespaces */
       buffer[0] = EOS;
       if (useMathML)
	 strcat (buffer, "\n      xmlns:m=\"http://www.w3.org/1998/Math/MathML/\"");
       if (useGraphML)
	 strcat (buffer, "\n      xmlns:g=\"http://www.w3.org/Graphics/SVG/Amaya2D\"");
       /* set the value of attribute Namespaces */
       if (attr == NULL)
	 {
	   attr = TtaNewAttribute (attrType);
	   TtaAttachAttribute (root, attr, doc);
	 }
       TtaSetAttributeText (attr, buffer, root, doc);
       
       /* delete the DOCTYPE attribute */
       attrType.AttrTypeNum = HTML_ATTR_HtmlDTD;
       attr = TtaGetAttribute (root, attrType);
       if (attr)
	 TtaRemoveAttribute (root, attr, doc);
       }
     }
   
   /* get the document charset */
   Charset[0] = EOS;
   charset = TtaGetDocumentCharset (doc);
   if (charset != UNDEFINED_CHARSET ||
       DocumentTypes[doc] == docMath ||
       DocumentTypes[doc] == docSVG)
     {
       if (charset == UNDEFINED_CHARSET)
	 strcat (Charset, "unknown");
       else
         {
           ptr = TtaGetCharsetName (charset);
           strcat (Charset, ptr);
         }
       /* set the Charset attribute of the root element*/
       elType = TtaGetElementType (root);
       attrType.AttrSSchema = elType.ElSSchema;
       if (DocumentTypes[doc] == docHTML)
	 attrType.AttrTypeNum = HTML_ATTR_Charset;
       else if (DocumentTypes[doc] == docMath)
	 attrType.AttrTypeNum = MathML_ATTR_Charset;
       else if (DocumentTypes[doc] == docSVG)
	 attrType.AttrTypeNum = GraphML_ATTR_Charset;
       docEl = TtaGetParent (root);
       charsetAttr = TtaGetAttribute (docEl, attrType); 

       if (!charsetAttr)
	 {
	   oldStructureChecking = TtaGetStructureChecking (doc);
	   TtaSetStructureChecking (0, doc);
	   charsetAttr = TtaNewAttribute (attrType);
	   TtaAttachAttribute (docEl, charsetAttr, doc);
	   TtaSetAttributeText (charsetAttr, Charset, docEl, doc);	
	   TtaSetStructureChecking ((ThotBool)oldStructureChecking, doc);
	 }
     }

   if (DocumentTypes[doc] == docHTML)
     {
     /* Create (or update) a META element to specify Content-type and Charset*/
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
       /* indicate the MIME type and the charset in a meta element with
	  an http-equiv attr */
       /* look for a meta/http-equiv element */
       el = TtaGetFirstChild (head);
       meta = NULL;
       attrType.AttrTypeNum = HTML_ATTR_http_equiv;
       attr = NULL;
       while (el && !meta)
	 {
	   elType = TtaGetElementType (el);
	   if (elType.ElSSchema == attrType.AttrSSchema &&
	       elType.ElTypeNum == HTML_EL_META)
	     {
	       attr = TtaGetAttribute (el, attrType);
	       if (attr)
		 meta = el;
	     }
	   if (!meta)
	     TtaNextSibling (&el);
	 }
       if (!meta)
	 /* there is no meta element with a http-equiv attribute */
	 /* create one at the begginning of the head */
	 {
	   elType.ElSSchema = attrType.AttrSSchema;
	   elType.ElTypeNum = HTML_EL_META;
	   meta = TtaNewElement (doc, elType);
	   TtaInsertFirstChild (&meta, head, doc);
	 }
       if (!attr)
	 {
	   attr = TtaNewAttribute (attrType);
	   TtaAttachAttribute (meta, attr, doc);
	 }
       TtaSetAttributeText (attr, "Content-Type", meta, doc);
       attrType.AttrTypeNum = HTML_ATTR_meta_content;
       attr = TtaGetAttribute (meta, attrType);
       if (!attr)
	 {
	   attr = TtaNewAttribute (attrType);
	   TtaAttachAttribute (meta, attr, doc);
	 }
       if (Charset[0] == EOS)
	 TtaSetAttributeText (attr, "text/html", meta, doc);
       else
	 {
	   strcpy (buffer, "text/html; charset=");
	   strcat (buffer, Charset);
	   TtaSetAttributeText (attr, buffer, meta, doc);
	 }
       } 
     }
}

/*----------------------------------------------------------------------
   RestartParser
  ----------------------------------------------------------------------*/
static void RestartParser (Document doc, char *localFile,
			   char *tempdir, char *documentname)
{
  CHARSET       charset;
  char          charsetname[MAX_LENGTH];
  int           parsingLevel;
  int           i;
  ThotBool      xmlDec, withDoctype, isXML;
  DocumentType  thotType;

  /* clean up previous Parsing errors file */
  CleanUpParsingErrors ();

  /* remove the Parsing errors file */
  RemoveParsingErrors (doc);

  for (i = 1; i < DocumentTableLength; i++)
    if (DocumentURLs[i] != NULL)
      if (DocumentSource[i] == doc && DocumentTypes[i] == docLog)
	{
	  /* remove the log file attached to the current document */
	  TtaCloseDocument (i);
	  /* remove the log file */
	  TtaFileUnlink (DocumentURLs[i]);
	  TtaFreeMemory (DocumentURLs[i]);
	  DocumentURLs[i] = NULL;
	  /* switch off the button Show Parsing errors file */
	  TtaSetItemOff (doc, 1, Views, BShowLogFile);
	}

  /* check if there is an XML declaration with a charset declaration */
  CheckDocHeader (localFile, &xmlDec, &withDoctype, &isXML,
		  &parsingLevel, &charset, charsetname, &thotType);
  DocumentMeta[doc]->xmlformat = isXML;

  /* When the mode was set to browser by a parsing error,
     restore the original mode */
  if (!ReadOnlyDocument[doc] && 
      !TtaGetDocumentAccessMode (doc))
    ChangeToEditorMode (doc);
  
  /* Calls the corresponding parser */
  if (DocumentMeta[doc]->xmlformat)       
    StartXmlParser (doc, localFile, documentname, tempdir,
		    localFile, xmlDec, withDoctype);
  else
    StartParser (doc, localFile, documentname, tempdir, localFile, FALSE);

  /* fetch and display all images referred by the document */
  DocNetworkStatus[doc] = AMAYA_NET_ACTIVE;
  FetchAndDisplayImages (doc, AMAYA_LOAD_IMAGE, NULL);
  DocNetworkStatus[doc] = AMAYA_NET_INACTIVE;

  /* check parsing errors */
  CheckParsingErrors (doc);
}

/*----------------------------------------------------------------------
   RedisplaySourceFile
   If doc is a HTML document and the source view is open, redisplay the
   source.
  ----------------------------------------------------------------------*/
static void RedisplaySourceFile (Document doc)
{
  char             *localFile;
  char	      documentname[MAX_LENGTH];
  char	      tempdir[MAX_LENGTH];
  NotifyElement       event;

  if (DocumentTypes[doc] == docHTML ||
      DocumentTypes[doc] == docSVG ||
      DocumentTypes[doc] == docMath)
    /* it's a structured document */
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

/*----------------------------------------------------------------------
   SaveDocumentLocally save the document in a local file.
   Return TRUE if the document has been saved
  ----------------------------------------------------------------------*/
static ThotBool SaveDocumentLocally (Document doc, char *directoryName,
				     char *documentName)
{
  char             *ptr;
  char              tempname[MAX_LENGTH];
  char              docname[100];
  ThotBool          ok;
  LoadedImageDesc  *pImage;

#ifdef AMAYA_DEBUG
  fprintf(stderr, "SaveDocumentLocally :  %s / %s\n", directoryName, documentName);
#endif

  strcpy (tempname, directoryName);
  strcat (tempname, DIR_STR);
  strcat (tempname, documentName);
  ok = FALSE;

  if (SaveAsText) 
    {
      SetInternalLinks (doc);
      if (DocumentTypes[doc] == docHTML)
         ok = TtaExportDocument (doc, tempname, "HTMLTT");
      else if (DocumentTypes[doc] == docSVG)
         ok = TtaExportDocument (doc, tempname, "GraphMLT");
      if (DocumentTypes[doc] == docMath)
         ok = TtaExportDocument (doc, tempname, "MathMLT");
    }
  else
    {
      if (DocumentTypes[doc] == docHTML)
	DocumentMeta[doc]->xmlformat = SaveAsXML;
      SetNamespacesAndDTD (doc);
      if (DocumentTypes[doc] == docHTML)
	{
	  if (SaveAsXML)
	    ok = TtaExportDocumentWithNewLineNumbers (doc, tempname,
						      "HTMLTX");
	  else
	    ok = TtaExportDocumentWithNewLineNumbers (doc, tempname,
						      "HTMLT");
	}
      else if (DocumentTypes[doc] == docSVG)
	ok = TtaExportDocumentWithNewLineNumbers (doc, tempname,
						  "GraphMLT");
      else if (DocumentTypes[doc] == docMath)
	ok = TtaExportDocumentWithNewLineNumbers (doc, tempname,
						  "MathMLT");
      else if (DocumentTypes[doc] == docImage)
	{
	  /* copy the image file to the new destination */
	  if (!IsW3Path (DocumentURLs[doc]))
	    {
	      /* local to local */
	      TtaFileCopy (DocumentURLs[doc], tempname);
	      ok = TRUE;
	    }
	  else
	    /* remote to local */
	    {
	      /* copy the copy from the cache to its new destination and
		 updated the pImage description (or maybe just erase it? */
	      pImage = SearchLoadedDocImage (doc, DocumentURLs[doc]);
	      if (pImage)
		{
		  /* copy the file */
		  TtaFileCopy (pImage->localName, tempname);
		  /* remove the old file (note that it's the local name that
		   we have to free, because we're using an HTML container
		   to show the image */
		  TtaFileUnlink (pImage->localName);
		  TtaFreeMemory (pImage->localName);
		  TtaFreeMemory (pImage->originalName);
		  /* save the new location */
		  pImage->originalName = TtaGetMemory (sizeof ("internal:")
						       + strlen (tempname)
						       + 1);
		  sprintf (pImage->originalName, "internal:%s", tempname);
		  pImage->localName = GetLocalPath (doc, tempname);
		  ok = TRUE;
		}
	    }
	}
      if (ok)
	{
	  TtaSetDocumentDirectory (doc, directoryName);
	  strcpy (docname, documentName);
	  /* Change the document name in all views */
	  TtaSetDocumentName (doc, docname);
	  SetWindowTitle (doc, doc, 0);
	  if (DocumentSource[doc])
	     SetWindowTitle (doc, DocumentSource[doc], 0);
	  /* save a local copy of the current document */
	  ptr = GetLocalPath (doc, tempname);
	  if (DocumentTypes[doc] == docImage)
	    /* export the new container (but to the temporary file name */
	    ok = TtaExportDocumentWithNewLineNumbers (doc, ptr,
						      "HTMLTX");
	  else
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
static ThotBool AddNoName (Document document, View view, char *url,
			   ThotBool *ok)
{
   char            msg[MAX_LENGTH];
   char            documentname[MAX_LENGTH];
   int             len;

  len = strlen (url);
  TtaExtractName (url, msg, documentname);
  *ok = (documentname[0] != EOS);
  if (*ok)
    return (FALSE);
  else
    {
      /* the name is not correct for the put operation */
      strcpy (msg, TtaGetMessage(AMAYA, AM_NO_NAME));
      strcat (msg, url);
      if (IsW3Path (url))
	{
	  if (url[len -1] != URL_SEP)
	    strcat (msg, URL_STR);
	}
      else if (url[len -1] != DIR_SEP)
	strcat (msg, DIR_STR);
      /* get default name */
      DefaultName = TtaGetEnvString ("DEFAULTNAME");
      if (DefaultName == NULL || *DefaultName == EOS)
	DefaultName = StdDefaultName;

      strcat (msg, DefaultName);
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
static int SafeSaveFileThroughNet (Document doc, char *localfile,
				   char *remotefile, char *content_type,
				   ThotBool use_preconditions)
{
  char              msg[MAX_LENGTH];
  char              tempfile[MAX_LENGTH]; /* File name used to refetch */
  char              tempURL[MAX_LENGTH];  /* May be redirected */
  char             *verify_publish;
  int               res;
  int               mode = 0;

  verify_publish = TtaGetEnvString("VERIFY_PUBLISH");
  /* verify the PUT by default */
  if (verify_publish == NULL)
    verify_publish = "yes";
  
  
#ifdef AMAYA_DEBUG
  fprintf(stderr, "SafeSaveFileThroughNet :  %s to %s type %s\n", localfile, remotefile, content_type);
#endif

  /* Save */
  /* JK: SYNC requests assume that the remotefile name is a static array */
  strcpy (tempfile, remotefile);
  mode = AMAYA_SYNC | AMAYA_NOCACHE | AMAYA_FLUSH_REQUEST;
  mode = mode | ((use_preconditions) ? AMAYA_USE_PRECONDITIONS : 0);

  res = PutObjectWWW (doc, localfile, tempfile, content_type, mode, NULL, NULL);
  if (res != 0)
    /* The HTTP PUT method failed ! */
    return (res);

  /* does the user want to verify the PUT? */
  if (!verify_publish || !*verify_publish || strcmp (verify_publish, "yes"))
    return (0);

  /* Refetch */
#ifdef AMAYA_DEBUG
  fprintf(stderr, "SafeSaveFileThroughNet :  refetch %s \n", remotefile);
#endif

  TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_VERIFYING), NULL);
  strcpy (tempURL, remotefile);
  res = GetObjectWWW (doc, tempURL, NULL, tempfile, AMAYA_SYNC | AMAYA_NOCACHE
		      | AMAYA_FLUSH_REQUEST, NULL, NULL, NULL, NULL, NO, NULL);
  if (res != 0)
    {
      /* The HTTP GET method failed ! */
      sprintf (msg, TtaGetMessage (AMAYA, AM_SAVE_RELOAD_FAILED), remotefile);
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
	  sprintf (msg, TtaGetMessage (AMAYA, AM_SAVE_COMPARE_FAILED), remotefile);
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
static ThotBool SaveObjectThroughNet (Document document, View view,
				      char *url, ThotBool confirm,
				      ThotBool use_preconditions)
{
  char            *tempname;
  char            *msg;
  char            *content_type;
  int              remainder = 500;
  int              res;

  msg = TtaGetMemory (remainder);
  if (msg == NULL)
    return (FALSE);

  /* save into the temporary document file */
  tempname = GetLocalPath (document, url);

  /* build the output */
  if (DocumentTypes[document] == docSource)
      /* it's a source file, renumber lines */
      TtaExportDocumentWithNewLineNumbers (document, tempname,
					   "TextFileT");
  else
      TtaExportDocument (document, tempname, "TextFileT");

  ActiveTransfer (document);
  TtaHandlePendingEvents ();
  
  if (DocumentMeta[document])
    content_type = DocumentMeta[document]->content_type;
  else
    content_type = NULL;

  res = SafeSaveFileThroughNet (document, tempname, url, content_type,
				use_preconditions);
  if (res != 0)
    {
      DocNetworkStatus[document] |= AMAYA_NET_ERROR;
      ResetStop (document);
      sprintf (msg, "%s %s",
		TtaGetMessage (AMAYA, AM_URL_SAVE_FAILED),
		url);
      if (confirm)
	{
	  InitConfirm3L (document, view, msg, AmayaLastHTTPErrorMsg, 
			 TtaGetMessage (AMAYA, AM_SAVE_DISK), TRUE);
	  if (UserAnswer)
	    res = -1;
	  else
	    res = 0;
	}
      else
	{
	  InitConfirm3L (document, view, msg, AmayaLastHTTPErrorMsg, 
			 AmayaLastHTTPErrorMsgR, FALSE);
	  res = -1;
	}
      /* erase the last status message */
      TtaSetStatus (document, view, "", NULL);	       
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
static ThotBool SaveDocumentThroughNet (Document doc, View view, char *url,
					ThotBool confirm, ThotBool with_images,
					ThotBool use_preconditions)
{
  LoadedImageDesc *pImage;
  char            *tempname;
  char            *msg;
  char            *content_type;
  int              remainder = 10000;
  int              index = 0, len, nb = 0;
  int              res;

  msg = TtaGetMemory (remainder);
  if (msg == NULL)
    return (FALSE);

  /* save into the temporary document file */
  tempname = GetLocalPath (doc, url);

  /* First step : generate the output file and ask for confirmation */
  SetNamespacesAndDTD (doc);
  if (DocumentTypes[doc] == docHTML)
    if (SaveAsXML)
      {
      TtaExportDocumentWithNewLineNumbers (doc, tempname, "HTMLTX");
      DocumentMeta[doc]->xmlformat = TRUE;
      }
    else
      {
      TtaExportDocumentWithNewLineNumbers (doc, tempname, "HTMLT");
      DocumentMeta[doc]->xmlformat = FALSE;
      }
  else if (DocumentTypes[doc] == docSVG)
      TtaExportDocumentWithNewLineNumbers (doc, tempname, "GraphMLT");
  else if (DocumentTypes[doc] == docMath)
      TtaExportDocumentWithNewLineNumbers (doc, tempname, "MathMLT");
  else if (DocumentTypes[doc] == docImage)
    {
      /* export the new container using the image file name */
      TtaExportDocumentWithNewLineNumbers (doc, tempname, "HTMLTX");
      TtaFreeMemory (tempname);
      pImage = SearchLoadedDocImage (doc, url);
      tempname = TtaStrdup (pImage->localName);
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
       
      strcpy (&msg[index], url);
      len = strlen (url);
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
		      "", nb, msg, 6, NULL, FALSE, TRUE);
       
      TtaSetDialoguePosition ();
      TtaShowDialogue (BaseDialog + ConfirmSave, FALSE);
      /* wait for an answer */
      TtaWaitShowDialogue ();
#else  /* _WINDOWS */
      CreateSaveListDlgWindow (TtaGetViewFrame (doc, view), nb, msg);
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

      if (DocumentMeta[doc])
	content_type = DocumentMeta[doc]->content_type;
      else
	content_type = NULL;
      res = SafeSaveFileThroughNet (doc, tempname, url, content_type, use_preconditions);
      if (res != 0)
	{
	  DocNetworkStatus[doc] |= AMAYA_NET_ERROR;
	  ResetStop (doc);
	  sprintf (msg, "%s %s",
		    TtaGetMessage (AMAYA, AM_URL_SAVE_FAILED),
		    url);
	  if (confirm)
	    {
	      InitConfirm3L (doc, view, msg, AmayaLastHTTPErrorMsg,
			     AmayaLastHTTPErrorMsgR, FALSE);
	      if (UserAnswer)
		res = -1;
	      else
		res = -1;
	    }
	  else
	    {
	      InitConfirm3L (doc, view, msg, AmayaLastHTTPErrorMsg, 
			     AmayaLastHTTPErrorMsgR, FALSE);
	      res = -1;
	    }
	  /* JK: to erase the last status message */
	  TtaSetStatus (doc, view, "", NULL);	       
	}
      else if (with_images)
	pImage = ImageURLs;

      while (pImage != NULL)
	{
	  if (pImage->document == doc && pImage->status == IMAGE_MODIFIED)
	    {
	      /* we get the MIME type of the image. We reuse whatever the
		 server sent if we have it, otherwise, we try to infer it from
		 the image type as discovered by the handler */
	      if (pImage->content_type)
		content_type = pImage->content_type;
	      else
		content_type = PicTypeToMIME (pImage->imageType);
	      res = SafeSaveFileThroughNet(doc, pImage->localName,
					   pImage->originalName, content_type,
					   use_preconditions);
	      if (res)
		{
		  DocNetworkStatus[doc] |= AMAYA_NET_ERROR;
		  ResetStop (doc);
		  sprintf (msg, "%s %s",
			    TtaGetMessage (AMAYA, AM_URL_SAVE_FAILED),
			    pImage->originalName);
		  InitConfirm3L (doc, view, msg, AmayaLastHTTPErrorMsg, 
				 AmayaLastHTTPErrorMsgR, FALSE);
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
   GetDocFromSource
   If sourceDoc is a source document, return the corresponding
   structured document.
  ----------------------------------------------------------------------*/
Document       GetDocFromSource (Document sourceDoc)
{
  Document	xmlDoc;
  int		i;

  xmlDoc = 0;
  if (DocumentTypes[sourceDoc] == docSource)
     /* It's a source document */
     for (i = 1; i < DocumentTableLength && xmlDoc == 0; i++)
        if (DocumentTypes[i] == docHTML ||
	    DocumentTypes[i] == docAnnot ||
	    DocumentTypes[i] == docSVG ||
	    DocumentTypes[i] == docMath)
           if (DocumentSource[i] == sourceDoc)
	      xmlDoc = i;
  return xmlDoc;
}

/*----------------------------------------------------------------------
   Synchronize
   save the current view (source/structure) in a temporary file and update
   the other view (structure/source).      
  ----------------------------------------------------------------------*/
void Synchronize (Document document, View view)
{
   NotifyElement       event;
   char*             tempdocument = NULL;
   char              documentname[MAX_LENGTH];
   char              tempdir[MAX_LENGTH];
   DisplayMode         dispMode;
   Document            xmlDoc, otherDoc;

   if (!DocumentURLs[document])
     /* the document is not loaded yet */
     return;
   otherDoc = 0;
   if (!TtaIsDocumentUpdated (document))
     /* nothing new to be saved in this view of the document. Let see if
        the other view has been modified */
     {
       if (DocumentTypes[document] == docHTML ||
	   DocumentTypes[document] == docSVG ||
	   DocumentTypes[document] == docMath)
           /* it's a structured document */
	  otherDoc = DocumentSource[document];
       else if (DocumentTypes[document] == docSource)
          otherDoc = GetDocFromSource (document);
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
       DocumentTypes[document] == docSVG ||
       DocumentTypes[document] == docMath)
     /* it's the structured form of the document */
     {
       /* save the current state of the document into the temporary file */
       tempdocument = GetLocalPath (document, DocumentURLs[document]);
       SetNamespacesAndDTD (document);
       if (DocumentTypes[document] == docHTML)
	 if (DocumentMeta[document]->xmlformat)
	   TtaExportDocumentWithNewLineNumbers (document, tempdocument,
						"HTMLTX");
	 else
	   TtaExportDocumentWithNewLineNumbers (document, tempdocument,
						"HTMLT");
       else if (DocumentTypes[document] == docSVG)
	 TtaExportDocumentWithNewLineNumbers (document, tempdocument,
					      "GraphMLT");
       else if (DocumentTypes[document] == docMath)
	 TtaExportDocumentWithNewLineNumbers (document, tempdocument,
					      "MathMLT");
       RedisplaySourceFile (document);
       otherDoc = DocumentSource[document];
       /* the other document is now different from the original file. It can
	  be saved */
       TtaSetItemOn (otherDoc, 1, File, BSave);
     }
   else if (DocumentTypes[document] == docSource)
     /* it's a source document */
     {
       xmlDoc = GetDocFromSource (document);
       otherDoc = xmlDoc;
       /* save the current state of the document into the temporary file */
       tempdocument = GetLocalPath (xmlDoc, DocumentURLs[xmlDoc]);
       TtaExportDocumentWithNewLineNumbers (document, tempdocument,
					    "TextFileT");
       TtaExtractName (tempdocument, tempdir, documentname);
       RestartParser (xmlDoc, tempdocument, tempdir, documentname);
       /* the other document is now different from the original file. It can
	  be saved */
       TtaSetDocumentModified (otherDoc);
       /* the source can be closed without save */
       TtaSetDocumentUnmodified (document);
       /* but it could be saved too */
       TtaSetItemOn (document, 1, File, BSave);
     }
   /* restore original display mode */
   TtaSetDisplayMode (document, dispMode);

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
void SaveDocument (Document doc, View view)
{
  NotifyElement       event;
  char                tempname[MAX_LENGTH];
  char                localFile[MAX_LENGTH];
  char                documentname[MAX_LENGTH];
  char                tempdir[MAX_LENGTH];
  char               *ptr;
  int                 i, res;
  Document	      xmlDoc;
  DisplayMode         dispMode;
  ThotBool            ok, newLineNumbers;

#ifdef ANNOTATIONS
  if (DocumentTypes[doc] == docAnnot) 
    {
      ANNOT_SaveDocument (doc, view);
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
      TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_NOTHING_TO_SAVE), "");
      return;
    }

  TextFormat = (DocumentTypes[doc] == docText ||
		DocumentTypes[doc] == docCSS ||
		DocumentTypes[doc] == docSource);

  /* if it's a source document, get the corresponding HTML document */
  if (DocumentTypes[doc] == docSource)
     xmlDoc = GetDocFromSource (doc);
  else
     xmlDoc = 0;

  SavingDocument = doc;

  ok = FALSE;
  newLineNumbers = FALSE;

  /* attempt to save through network if possible */
  strcpy (tempname, DocumentURLs[doc]);

  /* suppress compress suffixes from tempname */
  i = strlen (tempname) - 1;
  if (i > 2 && !strcmp (&tempname[i-2], ".gz"))
    {
      tempname[i-2] = EOS;
      TtaFreeMemory (DocumentURLs[doc]);
      DocumentURLs[doc] = TtaStrdup (tempname);
    }
  else if (i > 1 && !strcmp (&tempname[i-1], ".Z"))
    {
      tempname[i-1] = EOS;
      TtaFreeMemory (DocumentURLs[doc]);
      DocumentURLs[doc] = TtaStrdup (tempname);
    }

#ifdef AMAYA_DEBUG
  fprintf(stderr, "SaveDocument : %d to %s\n", doc, tempname);
#endif

  /* change display mode to avoid flicker due to callbacks executed when
     saving some elements, for instance META */
  dispMode = TtaGetDisplayMode (doc);
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, DeferredDisplay);

  /* the suffix determines the output format */
  SaveAsXML = IsXMLName (tempname) || DocumentMeta[doc]->xmlformat;

  /* We automatically save a HTML document as a XML one 
     when we have a xhtml profile */
    if (!SaveAsXML &&
	(ParsingLevel[doc] == L_Basic ||
	 ParsingLevel[doc] == L_Strict ||
	 ParsingLevel[doc] == L_Xhtml11))
      SaveAsXML = TRUE;

  if (IsW3Path (tempname))
    /* it's a remote document */
    {
      if (AddNoName (doc, view, tempname, &ok))
	{
	  ok = TRUE;
	  /* need to update the document url */
	  res = strlen(tempname) - 1;
	  if (tempname[res] != URL_SEP)
	    strcat (tempname, URL_STR);
	  strcat (tempname, DefaultName);
	  TtaFreeMemory (DocumentURLs[doc]);
	  DocumentURLs[doc] = TtaStrdup (tempname);
	  DocumentMeta[doc]->put_default_name = TRUE; 
	  if (DocumentTypes[doc] == docHTML)
	      /* it's an HTML document. It could have a source doc */
	      /* change the URL of the source document if it exists */
	     {
	     if (DocumentSource[doc])
		/* it has a source document */
		{
		TtaFreeMemory (DocumentURLs[DocumentSource[doc]]);
		DocumentURLs[DocumentSource[doc]] = TtaStrdup (tempname);
		}
	      }
	  else if (DocumentTypes[doc] == docSource)
	      {
	      /* it's a source document. Change the URL of the corresponding
		 HTML document */
	      if (xmlDoc)
		 {
		 TtaFreeMemory (DocumentURLs[xmlDoc]);
		 DocumentURLs[xmlDoc] = TtaStrdup (tempname);
		 }
	      }
	}

      ptr = GetLocalPath (doc, DocumentURLs[doc]);
      /*  no need to protect against a null ptr, as GetLocalPath
          will always return something at this point */
      strcpy (localFile, ptr);
      TtaFreeMemory (ptr);
      
      /* it's a complete name: save it */
      if (ok)
	{
	  if (TextFormat)
	    {
	      ok = SaveObjectThroughNet (doc, view, DocumentURLs[doc],
					 FALSE, TRUE);
	      if (DocumentTypes[doc] == docSource)
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
    }
  else
    /* it's a local document */
    {
      strcpy (localFile, tempname);
      if (TextFormat)
	if (DocumentTypes[doc] == docSource)
	   /* it's a source file. renumber lines */
	   {
	   ok = TtaExportDocumentWithNewLineNumbers (doc, tempname,
						     "TextFileT");
	   newLineNumbers = TRUE;
	   }
	else
	  ok = TtaExportDocument (doc, tempname, "TextFileT");
      else
	{
	SetNamespacesAndDTD (doc);
	if (DocumentTypes[doc] == docHTML)
	  if (SaveAsXML)
	    {
	    ok = TtaExportDocumentWithNewLineNumbers (doc, tempname,
						      "HTMLTX");
	    DocumentMeta[doc]->xmlformat = TRUE;
	    }
	  else
	    {
	    ok = TtaExportDocumentWithNewLineNumbers (doc, tempname,
						      "HTMLT");
	    DocumentMeta[doc]->xmlformat = FALSE;
	    }
	else if (DocumentTypes[doc] == docSVG)
	  ok = TtaExportDocumentWithNewLineNumbers (doc, tempname,
						    "GraphMLT");
	else if (DocumentTypes[doc] == docMath)
	  ok = TtaExportDocumentWithNewLineNumbers (doc, tempname,
						    "MathMLT");
	newLineNumbers = TRUE;
	}
      /* save a local copy of the current document */
      if (xmlDoc)
	ptr = GetLocalPath (xmlDoc, tempname);
      else
	ptr = GetLocalPath (doc, tempname);
      TtaFileCopy (tempname, ptr);
      TtaFreeMemory (ptr);
    }

  /* restore original display mode */
  TtaSetDisplayMode (doc, dispMode);

  SavingDocument = 0;
  if (newLineNumbers)
    {
     /* line numbers have been changed in the saved document */
     if (DocumentTypes[doc] == docHTML)
        /* It's a HTML document. If the source view is open, redisplay the
	   source. */
       RedisplaySourceFile (doc);
     else if (DocumentTypes[doc] == docSource)
       {
	/* It's a source document. Reparse the corresponding HTML document */
	if (xmlDoc)
	   {
	   TtaExtractName (localFile, tempdir, documentname);
	   RestartParser (xmlDoc, localFile, tempdir, documentname);
	   TtaSetDocumentUnmodified (xmlDoc);
	   /* Synchronize selections */
	   event.document = doc;
	   SynchronizeSourceView (&event);
	   }
       }
    }
  if (ok)
    {
      if (DocumentMeta[doc]->method == CE_TEMPLATE)
	{
	  DocumentMeta[doc]->method = CE_ABSOLUTE;
	  DocumentMetaClear (DocumentMeta[doc]);
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
void BackUpDocs ()
{
  Document        doc;
  FILE           *f;
  char            pathname[MAX_LENGTH];
  char            docname[MAX_LENGTH];
  char           *ptr;
  int             l;

  /* check all modified documents */
  f = NULL;
  for (doc = 1; doc < DocumentTableLength; doc++)
    if (DocumentURLs[doc] && TtaIsDocumentModified (doc) && doc != W3Loading)
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
	l = strlen (ptr) - 1;
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
	fprintf (f, "\"%s\" \"%s\" %d\n", pathname, ptr, DocumentTypes[doc]);
	TtaFreeMemory (ptr);
      }
  /* now close the crash file */
  if (f != NULL)
    fclose (f);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
ThotBool DocumentToSave (NotifyDialog *event)
{
   SaveDocument (event->document, 1);
   return TRUE;	/* prevent Thot from performing normal save operation */
}

/*----------------------------------------------------------------------
  DeleteDocImageContext
  Deletes all the context related to docImage document doc and URL url.
  ----------------------------------------------------------------------*/
static void DeleteDocImageContext (Document doc, char *url)
{
  LoadedImageDesc    *pImage;
  
  /* we should remove this line if when we change the code
     to use an ImageDesc for local images too */
  if (!IsHTTPPath (url))
    return;

  pImage = SearchLoadedDocImage (doc, url);
  if (!pImage)
    /* nothing to be deleted */
    return;

  /* remove it from the list */
  if (ImageURLs == pImage)
    ImageURLs = pImage->nextImage;
  else
    pImage->prevImage->nextImage = pImage->nextImage;

  /* delete the local copy of the image */
  TtaFileUnlink (pImage->localName);

  /* free all associated memory */
  TtaFreeMemory (pImage->originalName);
  TtaFreeMemory (pImage->localName);
  TtaFreeMemory (pImage);
}

/*----------------------------------------------------------------------
  UpdateDocImage
  Changes the HTML container to point to the new URL of the image.
  If pictures are saved locally, make the copy.
  The parameter imgbase gives the relative path of the new image directory.
  The parameter newURL gives the new document URL (or local file).
  ----------------------------------------------------------------------*/
static ThotBool UpdateDocImage (Document doc, ThotBool src_is_local,
				ThotBool dst_is_local, char *newURL)
{
  Element       el;
  ElementType   elType;
  Attribute     attr;
  AttributeType attrType;
  char          *ptr;
  char          *localName;
  char          *internalURL;
  LoadedImageDesc    *pImage;
  Language      lang = 0;

  /* get the URL of the image and the element */
  if (! ImageElement (doc, NULL, &el))
    return FALSE;

  /* change the value of the src attribute */
  attrType.AttrSSchema = TtaGetSSchema ("HTML", doc);
  attrType.AttrTypeNum = HTML_ATTR_SRC;
  attr = TtaGetAttribute (el, attrType);
  TtaRegisterAttributeReplace (attr, el, doc);
  TtaSetAttributeText (attr, newURL, el, doc);

  /* change the title of the document */
  elType.ElSSchema = attrType.AttrSSchema;
  elType.ElTypeNum = HTML_EL_TITLE;
  el = TtaGetRootElement (doc);
  el = TtaSearchTypedElement (elType, SearchInTree, el);
  if (el)
    {
      el = TtaGetFirstChild (el);
      localName = GetLocalPath (doc, newURL);
      ptr = strrchr (localName, DIR_SEP);
      if (ptr)
	{
	  ptr++;
	  TtaRegisterElementReplace (el, doc);
	  TtaSetTextContent (el, ptr, lang, doc);
	}
      TtaFreeMemory (localName);
    }

  /* copy the file to the amaya cache if it's a remote save */
  if (!dst_is_local)
    {
      /* make the special internal URL (used to display the image from
	 the container */
      internalURL = TtaGetMemory (sizeof ("internal:")
				  + strlen (newURL)
				  + 1);
      sprintf (internalURL, "internal:%s", newURL);
      
      /* make the local name (we switch the extension to .html) */
      localName = TtaGetMemory (strlen (newURL)
			       + sizeof (".html"));
      strcpy (localName, newURL);
      ptr = strrchr (localName, '.');
      if (ptr)
	strcpy (ptr, ".html");
      else
	strcat (localName, ".html");
      ptr = GetLocalPath (doc, localName);
      TtaFreeMemory (localName);
      localName = ptr;

      /* create the pImage descriptor if it doesn't exist
       and copy the file */

      pImage = SearchLoadedDocImage (doc, newURL);
      if (pImage)
	{
	  /* copy the old file to the new location */
	  TtaFileCopy (pImage->localName, localName);
	  if (pImage->originalName != NULL)
	    TtaFreeMemory (pImage->originalName);
	  pImage->originalName = internalURL;
	  if (pImage->localName)
	    TtaFreeMemory (pImage->localName);
	  pImage->localName = localName;
	  pImage->status = IMAGE_LOADED;
	}
      else
	{
	  if (IsHTTPPath (DocumentURLs[doc]))
	    {
	      /* remote to remote copy */
	      pImage = SearchLoadedDocImage (doc, DocumentURLs[doc]);
	      TtaFileCopy (pImage->localName, localName);
	    }
	  else
	    /* local to remote copy */
	    TtaFileCopy (DocumentURLs[doc], localName);

	  pImage = (LoadedImageDesc *) TtaGetMemory (sizeof (LoadedImageDesc));
	  memset ((void *) pImage, 0, sizeof (LoadedImageDesc));
	  pImage->originalName = internalURL;
	  pImage->localName = localName;
	  pImage->prevImage = NULL;
	  if (ImageURLs)
	    {
	      ImageURLs->prevImage = pImage;
	      pImage->nextImage = ImageURLs;
	    }
	  else
	    pImage->nextImage = NULL;
	  ImageURLs = pImage;
	  pImage->document = doc;
	  pImage->elImage = NULL;
	  /* @@ JK: initialize the image type here */
	  pImage->imageType = TtaGetPictureType (el);
	}
	  pImage->status = IMAGE_LOADED;
    }
  return TRUE;
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
static void UpdateImages (Document doc, ThotBool src_is_local,
			  ThotBool dst_is_local, char *imgbase, char *newURL)
{
  SSchema             XHTMLSSchema, MathSSchema, GraphSSchema, XLinkSSchema;
  AttributeType       attrType;
  ElementType         elType;
  Attribute           attr;
  Element             el, root, content;
  LoadedImageDesc    *pImage;
  Language            lang;
  char                tempfile[MAX_LENGTH];
  char                localpath[MAX_LENGTH];
  char                oldpath[MAX_LENGTH];
  char                oldname[MAX_LENGTH];
  char                tempname[MAX_LENGTH];
  char                imgname[MAX_LENGTH];
  char                url[MAX_LENGTH];
  char               *buf, *ptr;
  char               *sStyle, *stringStyle;
  char               *oldStyle;
  int                 buflen, max, index;

  if (imgbase[0] != EOS)
    {
      /* add the separator if needed */
      buflen = strlen (imgbase) - 1;
      if (dst_is_local && !IsW3Path (imgbase))
	{
	  if (imgbase[buflen] != DIR_SEP)
	    strcat (imgbase, DIR_STR);
	}
      else
	{
	  if (imgbase[buflen] != URL_SEP)
	    strcat (imgbase, URL_STR);
	}
    }

  /* save the old document path to locate existing images */
  strcpy (oldpath, DocumentURLs[doc]);
  buflen = strlen (oldpath) - 1;
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

      XHTMLSSchema = TtaGetSSchema ("HTML", doc);
      MathSSchema = TtaGetSSchema ("MathML", doc);
      GraphSSchema = TtaGetSSchema ("GraphML", doc);
      XLinkSSchema = TtaGetSSchema ("XLink", doc);
      root = TtaGetMainRoot (doc);
      /* handle style elements */
      elType = TtaGetElementType (root);
      if (elType.ElSSchema == XHTMLSSchema || elType.ElSSchema == GraphSSchema)
	{
	  if (elType.ElSSchema == XHTMLSSchema)
	    elType.ElTypeNum = HTML_EL_STYLE_;
	  else if (elType.ElSSchema == GraphSSchema)
	    elType.ElTypeNum = GraphML_EL_style__;
	  el = TtaSearchTypedElement (elType, SearchInTree, root);
	}
      else
	el = NULL;
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
		   stringStyle = strstr (stringStyle, "url") + 3;
		   strcpy (url, ptr);
		   TtaFreeMemory (ptr);
		   NormalizeURL (url, 0, tempname, imgname, newURL);

		   /* extract the URL from the old style string */
		   ptr = GetCSSBackgroundURL (oldStyle);
		   if (ptr != NULL)
		     {
		       /* for next research */
		       oldStyle = strstr (oldStyle, "url") + 3;
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
				   strcpy (oldname, localpath);
				   strcat (oldname, imgname);
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
				   strcpy (tempfile, localpath);
				   strcat (tempfile, imgname);
				   pImage = SearchLoadedImage (tempfile, doc);
				   /* update the descriptor */
				   if (pImage)
				     {
				       /* image was already loaded */
				       if (pImage->originalName != NULL)
					 TtaFreeMemory (pImage->originalName);
				       pImage->originalName = TtaStrdup (tempname);
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

       max = sizeof (SRC_attr_tab) / sizeof (AttSearch);
       for (index = 0; index < max; index++)
	 {
	   /* fetch a new attrValue */
	   attrType.AttrTypeNum = SRC_attr_tab[index].att;
	   switch (SRC_attr_tab[index].type)
	     {
	     case XHTML_TYPE:
	       attrType.AttrSSchema = XHTMLSSchema;
	       break;
	     case MATH_TYPE:
	       attrType.AttrSSchema = MathSSchema;
	       break;
	     case GRAPH_TYPE:
	       attrType.AttrSSchema = GraphSSchema;
	       break;
	     case XLINK_TYPE:
	       attrType.AttrSSchema = XLinkSSchema;
	       break;
	     default:
	       attrType.AttrSSchema = NULL;
	     }
	   if (attrType.AttrSSchema)
	     TtaSearchAttribute (attrType, SearchForward, root, &el, &attr);
	   else
	     el = NULL;
	   while (el && attr)
	     {
	       elType = TtaGetElementType (el);
	       buflen = MAX_LENGTH;
	       buf = TtaGetMemory (buflen);
	       if (buf != NULL)
		 {
		   TtaGiveTextAttributeValue (attr, buf, &buflen);
		   if ((attrType.AttrTypeNum == HTML_ATTR_Style_ &&
			attrType.AttrSSchema == XHTMLSSchema) ||
		       (attrType.AttrTypeNum == MathML_ATTR_style_ &&
			attrType.AttrSSchema == MathSSchema) ||
		       (attrType.AttrTypeNum == GraphML_ATTR_style_  &&
			attrType.AttrSSchema == GraphSSchema))
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
			   strcpy (url, ptr);
			   TtaFreeMemory (ptr);
			   /* extract the URL from the new style string */
			   ptr = GetCSSBackgroundURL (url);
			   if (ptr != NULL)
			     {
			       strcpy (url, ptr);
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
		       strcpy (url, buf);
		       NormalizeURL (url, 0, buf, imgname, oldpath);
		       /* save the new SRC attr value */
		       if (imgbase[0] != EOS)
			 {
			   /* compose the relative or absolute name */
			   strcpy (url, imgbase);
			   strcat (url, imgname);
			 }
		       else
			 /* in same directory -> local name */
			 strcpy (url, imgname);
		       
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
			       strcpy (buf, localpath);
			       strcat (buf, imgname);
			     }
			   
			   if (imgbase[0] != EOS)
			     strcpy (tempfile, tempname);
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
			       pImage = SearchLoadedImage (tempfile, doc);
			       /* update the descriptor */
			       if (pImage)
				 {
				   /* image was already loaded */
				   if (pImage->originalName != NULL)
				     TtaFreeMemory (pImage->originalName);
				   pImage->originalName = TtaStrdup (tempname);
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
void DoSaveAs (char *user_charset, char *user_mimetype)
{
  NotifyElement       event;
  Document            doc;
  AttributeType       attrType;
  ElementType         elType;
  Element             el, root;
  char               *documentFile;
  char               *tempname, *oldLocal, *newLocal;
  char               *imagePath, *base;
  char                imgbase[MAX_LENGTH];
  char                documentname[MAX_LENGTH];
  char                tempdir[MAX_LENGTH];
  char                url_sep;
  int                 res;
  int                 len, xmlDoc;
  DisplayMode         dispMode;
  ThotBool            src_is_local;
  ThotBool            dst_is_local, ok;
  ThotBool	      docModified, toUndo;
  ThotBool            new_put_def_name;
  ThotBool            old_put_def_name;
  char               *old_charset = NULL;
  char               *old_mimetype = NULL;
  char               *ptr;
  CHARSET             charset;

  if (SavingDocument == 0)
    return;

  src_is_local = !IsW3Path (DocumentURLs[SavingDocument]);
  dst_is_local = !IsW3Path (SavePath);
  ok = TRUE;
  toUndo = FALSE;
  base = NULL;

  /* save the context */
  doc = SavingDocument;

#ifdef AMAYA_DEBUG
  fprintf(stderr, "DoSaveAs : from %s to %s/%s , with images %d\n",
	  DocumentURLs[SavingDocument], SavePath, SaveName, (int) CopyImages);
#endif

  /* New document path */
  documentFile = TtaGetMemory (MAX_LENGTH);
  strcpy (documentFile, SavePath);
  len = strlen (documentFile);
  if (documentFile [len -1] != DIR_SEP && documentFile [len - 1] != '/')
    {
     if (dst_is_local)
       {
	 strcat (documentFile, DIR_STR);
	 url_sep = DIR_SEP;
       }
     else
       {
	 strcat (documentFile, URL_STR);
	 url_sep = URL_SEP;
       }
    }
  else if (dst_is_local)
    url_sep = DIR_SEP;
  else
    url_sep = URL_SEP;

  new_put_def_name = FALSE;
  if (SaveName[0] == EOS)
    {
      /* there is no document name */
      if (AddNoName (SavingDocument, 1, documentFile, &ok))
	{
	  ok = TRUE;
	  res = strlen(SavePath) - 1;
	  if (SavePath[res] == url_sep)
	    SavePath[res] = EOS;
	  /* need to update the document url */
	  strcpy (SaveName, DefaultName);
	  strcat (documentFile, SaveName);
	  /* set up a temp flag to say we're using the default name */
	  new_put_def_name = TRUE;
	}
      else if (!ok)
	{
	  /* save into the temporary document file */
	  doc = SavingDocument;
	  TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_CANNOT_SAVE), DocumentURLs[doc]);
	  SavingDocument = 0;
	  /* display the dialog box */
	  InitSaveForm (doc, 1, documentFile);
	}
    }
  else
      strcat (documentFile, SaveName);

  doc = SavingDocument;
  if (ok && dst_is_local)
    {
      /* verify that the directory exists */
      if (!TtaCheckDirectory (SavePath))
	{
	  TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_CANNOT_SAVE), SavePath);
	  /* the user has to change the name of the images directory */
	  /* display the dialog box */
	  InitSaveForm (doc, 1, documentFile);
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
	      /* display the dialog box */
	      InitSaveForm (doc, 1, documentFile);
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
      if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML"))
	{
	  attrType.AttrSSchema = elType.ElSSchema;
	  /* search the BASE element */
	  elType.ElTypeNum = HTML_EL_BASE;
	  el = TtaSearchTypedElement (elType, SearchInTree, root);
	  if (el)
	    /* URLs are still relative to the document base */
	    base = GetBaseURL (doc);
	}
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
	      if (imgbase[0] != DIR_SEP)
		  {
		    strcpy (tempname, SavePath);
		    strcat (tempname, DIR_STR);
		    strcat (tempname, imgbase);
		  }
	      else
		strcpy(tempname, imgbase);
	      ok = TtaCheckDirectory (tempname);
	      if (!ok)
		{
		  TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_CANNOT_SAVE), tempname);
		  TtaFreeMemory (tempname);
		  /* free base before returning*/
		  if (base)
		    TtaFreeMemory (base);
		  /* the user has to change the name of the images directory */
		  /* display the dialog box */
		  InitSaveForm (doc, 1, documentFile);
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

      /* adjust the charset and MIME type */
      if (user_charset && *user_charset)
	{
	  if (DocumentMeta[doc]->charset)
	    old_charset = DocumentMeta[doc]->charset;
	  DocumentMeta[doc]->charset = TtaStrdup (user_charset);
	  /* change the charset of the doc itself */
	  charset = TtaGetCharset (user_charset);
	  TtaSetDocumentCharset (doc, charset);
	}
      if (user_mimetype && *user_mimetype)
	{
	  if (DocumentMeta[doc]->content_type)
	    old_mimetype = DocumentMeta[doc]->content_type;
	  DocumentMeta[doc]->content_type = TtaStrdup (user_mimetype);
	}

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
	      ok = TtaExportDocument (doc, documentFile, "TextFileT");
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
	  if (DocumentTypes[doc] == docImage)
	    UpdateDocImage (doc, src_is_local, dst_is_local, documentFile);
	  else if (DocumentTypes[doc] != docMath)
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
	  /* remove the previous docImage context */
	  if (DocumentTypes[doc] == docImage)
	    DeleteDocImageContext (doc, DocumentURLs[doc]);
	  /* add to the history the data of the previous document */
	  if (DocumentTypes[doc] == docSource)
	    {
	      /* the original document must be updated */
	      xmlDoc = GetDocFromSource (doc);
	      /* update the source document */
	      TtaFreeMemory (DocumentURLs[doc]);
	      DocumentURLs[doc] = TtaStrdup (documentFile);
	      TtaSetDocumentUnmodified (doc);
	      /* switch Amaya buttons and menus */
	      DocStatusUpdate (doc, FALSE);
	    }
	  else
	    xmlDoc = doc;
	  AddDocHistory (xmlDoc, DocumentURLs[xmlDoc], 
			 DocumentMeta[xmlDoc]->initial_url,
			 DocumentMeta[xmlDoc]->form_data,
			 DocumentMeta[xmlDoc]->method);
	  /* change the document url */
	  if (TextFormat || !SaveAsText)
	    {
	      TtaFreeMemory (DocumentURLs[xmlDoc]);
	      DocumentURLs[xmlDoc] = TtaStrdup (documentFile);
	      TtaSetTextZone (xmlDoc, 1, 1, DocumentURLs[xmlDoc]);
	      if (DocumentSource[doc])
		{
	          TtaFreeMemory (DocumentURLs[DocumentSource[doc]]);
	          DocumentURLs[DocumentSource[doc]] = TtaStrdup (documentFile);
		}
	      if (DocumentMeta[xmlDoc]->method == CE_TEMPLATE)
		{
		  DocumentMeta[xmlDoc]->method = CE_ABSOLUTE;
		  DocumentMetaClear (DocumentMeta[xmlDoc]);
		}
	      TtaSetDocumentUnmodified (xmlDoc);
	      /* switch Amaya buttons and menus */
	      DocStatusUpdate (xmlDoc, FALSE);
	    }
	  if (doc != xmlDoc)
	   {
	     /* It's a source document. Reparse the corresponding HTML document */
	     TtaExtractName (documentFile, tempdir, documentname);
	     RestartParser (xmlDoc, documentFile, tempdir, documentname);
	     TtaSetDocumentUnmodified (xmlDoc);
	     /* Synchronize selections */
	     event.document = doc;
	     SynchronizeSourceView (&event);
	   }
	  else
	    /* if it's a HTML document and the source view is open, redisplay
	       the source. */
	    RedisplaySourceFile (doc);
	  /* Sucess of the operation */
	  TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_SAVED), documentFile);
	  /* remove the previous temporary file */
	  if (oldLocal && !SaveAsText && strcmp (oldLocal, newLocal))
	    /* free the previous temporary file */
	    TtaFileUnlink (oldLocal);
	  /* free the previous mime type and charset */
	  if (old_charset)
	    TtaFreeMemory (old_charset);
	  if (old_mimetype)
	    TtaFreeMemory (old_mimetype);
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
	  if (DocumentTypes[doc] == docImage)
	    {
	      DeleteDocImageContext (doc, documentFile);
	      /* free the previous temporary file */
	      ptr = GetLocalPath (doc, documentFile);
	      TtaFileUnlink (ptr);
	      TtaFreeMemory (ptr);
	    }
	  if (!docModified)
	    {
	      TtaSetDocumentUnmodified (doc);
	      /* switch Amaya buttons and menus */
	      DocStatusUpdate (doc, docModified);
	    }
	  DocumentMeta[doc]->put_default_name = old_put_def_name;
	  /* restore the previous charset and mime type */
	  if (user_charset && DocumentMeta[doc]->charset)
	    TtaFreeMemory (DocumentMeta[doc]->charset);
	  DocumentMeta[doc]->charset = old_charset;
	  charset = TtaGetCharset (old_charset);
	  TtaSetDocumentCharset (doc, charset);
	  if (user_mimetype && DocumentMeta[doc]->content_type)
	    TtaFreeMemory (DocumentMeta[doc]->content_type);
	  DocumentMeta[doc]->content_type = old_mimetype;
	  /* propose to save a second time */
	  SaveDocumentAs(doc, 1);
	}
      TtaFreeMemory (oldLocal);
      TtaFreeMemory (newLocal);
    }
  TtaFreeMemory (documentFile);
}
