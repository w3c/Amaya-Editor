/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2004
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *
 * Author: I. Vatton
 *
 */

#ifdef _WX
  #include "wx/wx.h"
#endif /* _WX */

/* nItagetMecluded headerfiles */
#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"
#ifdef _SVG
#include "SVG.h"
#endif /* _SVG */
#ifdef _WINGUI
#include "wininclude.h"
#endif /* _WINGUI */
#ifdef _WX
  #include "wxdialogapi_f.h"
#endif /* _WX */

static int         CSScase;
static char        CSSpath[500];
static Document    CSSdocument;
static Element    *CSSlink = NULL;
/* Use the same order of CSSCategory defined in css.h */
static char       *DisplayCategory[]={
  "[x] " /*CSS_Unknown*/,
  "[U] " /*CSS_USER_STYLE*/,
  "[S] " /*CSS_DOCUMENT_STYLE*/,
  "[E] " /*CSS_EXTERNAL_STYLE*/,
  "[I] " /*CSS_IMPORT*/,
  "[ ] " /*CSS_EMBED*/
};
#define DisplayCategory_length 6
#define String_length  4

#include "AHTURLTools_f.h"
#include "EDITstyle_f.h"
#include "EDITORactions_f.h"
#include "HTMLactions_f.h"
#include "HTMLhistory_f.h"
#include "HTMLedit_f.h"
#include "UIcss_f.h"
#include "css_f.h"
#include "dialog.h"
#include "fetchXMLname_f.h"
#include "html2thot_f.h"
#include "init_f.h"
#include "query_f.h"
#include "styleparser_f.h"
#include "Xmlbuilder_f.h"

/*----------------------------------------------------------------------
   LoadRemoteStyleSheet loads a remote style sheet into a file.
   Return FALSE if it's a local file and TRUE otherwise.
   When returning, the parameter completeURL contains the normalized url
   and the parameter localfile the path of the local copy of the file.
  ----------------------------------------------------------------------*/
ThotBool LoadRemoteStyleSheet (char *url, Document doc, Element el,
			       CSSInfoPtr css, char *completeURL,
			       char *localfile)
{
  CSSInfoPtr          oldcss;
  PInfoPtr            pInfo;
  char                tempname[MAX_LENGTH];
  char               *tempdocument = NULL;
  int                 toparse;
  ThotBool            remote = FALSE;
  ThotBool            import = (css != NULL);

  /* this document is displayed -> load the CSS */
  localfile[0] = EOS;
  if (import && css->url)
    NormalizeURL (url, 0, completeURL, tempname, css->url);
  else if (import && css->localName)
    NormalizeURL (url, 0, completeURL, tempname, css->localName);
  else
    NormalizeURL (url, doc, completeURL, tempname, NULL);
  
  /* check if the style sheet is already loaded */
  oldcss = SearchCSS (0, completeURL, el, &pInfo);
  remote = IsW3Path (completeURL);
  if (oldcss)
    strcpy (localfile, oldcss->localName);
  else if (remote)
    {
      /* the CSS is not loaded yet */
      /* changed this to doc */
      UpdateTransfer (doc);
      toparse = GetObjectWWW (doc, 0, completeURL, NULL, localfile,
			      AMAYA_SYNC | AMAYA_LOAD_CSS, NULL, NULL,
			      NULL, NULL, NO, NULL);
      ResetStop (doc);
      if (toparse || localfile[0] == EOS || !TtaFileExist (localfile))
	TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_CANNOT_LOAD),
		      completeURL);
      else
	{
	  /* store a copy of the remote CSS in .amaya/0 */
	  /* allocate and initialize tempdocument */
	  tempdocument = GetLocalPath (0, completeURL);
	  TtaFileUnlink (tempdocument);
	  /* now we can rename the local name of a remote document */
	  TtaFileRename (localfile, tempdocument);
	  strcpy (localfile, tempdocument);
	  TtaFreeMemory (tempdocument);
	}
    }
  else
    {
      /* store a copy of the local CSS in .amaya/0 */
      tempdocument = GetLocalPath (0, completeURL);
      strcpy (localfile, tempdocument);
      TtaFileCopy (completeURL, localfile);
      TtaFreeMemory (tempdocument);
    }
  return (remote);
}


/*----------------------------------------------------------------------
   InitUserStyleSheet: Generates a user Style Sheet if it doesn't exist.           
  ----------------------------------------------------------------------*/
void InitUserStyleSheet (char *url)
{
  FILE             *f;

  if (!TtaFileExist (url))
    {
      f = TtaWriteOpen (url);
      if (f)
	{
	  fprintf (f, "/* This is the default Amaya CSS file */\n");
	  TtaWriteClose (f);
	}
    }
}

/*----------------------------------------------------------------------
   LoadUserStyleSheet: Load the user Style Sheet found in his/her   
   home directory or the default one in THOTDIR.           
  ----------------------------------------------------------------------*/
void LoadUserStyleSheet (Document doc)
{
  CSSInfoPtr          css;
  PInfoPtr            pInfo;
  struct stat         buf;
  FILE               *res;
  char               *buffer, *ptr;
  int                 len;
  ThotBool            loadcss;

  /* check if we have to load CSS */
  TtaGetEnvBoolean ("LOAD_CSS", &loadcss);
  if (!loadcss)
    return;
  /* look for the User preferences */
  if (!UserCSS || !TtaFileExist (UserCSS))
    return;

  buffer = NULL;
  ptr = UserCSS;
  css = SearchCSS (doc, ptr, NULL, &pInfo);
  if (css == NULL)
    {
      /* store a copy of the local CSS in .amaya/0 */
      ptr = GetLocalPath (0, UserCSS);
      TtaFileCopy (UserCSS, ptr);
      /* allocate a new Presentation structure */ 
      css = AddCSS (0, doc, CSS_USER_STYLE, CSS_ALL, UserCSS, ptr, NULL);
      TtaFreeMemory (ptr);
    }
  else if (pInfo == NULL)
    AddInfoCSS (doc, css, CSS_USER_STYLE, CSS_ALL, NULL);
  else if (pInfo->PiSchemas)
    /* already applied */
    return;

  ptr = css->localName;
  if (ptr[0] != EOS  && TtaFileExist (ptr))
    {
      /* read User preferences */
      res = TtaReadOpen (ptr);
      if (res != NULL)
	{
#ifdef _WINGUI
	  if (fstat (_fileno (res), &buf))
#else  /* !_WINGUI */
	  if (fstat (fileno (res), &buf))
#endif /* !_WINGUI */
	    TtaReadClose (res);
	  else
	    {
	      buffer = (char *)TtaGetMemory (buf.st_size + 1000);
	      if (buffer == NULL)
		TtaReadClose (res);
	      else
		{
		  len = fread (buffer, buf.st_size, 1, res);
		  if (len != 1)
		    {
		      TtaFreeMemory (buffer);
		      buffer = NULL;
		      TtaReadClose (res);
		    }
		  else
		    {
		      buffer[buf.st_size] = 0;
		      TtaReadClose (res);
		    }
		}
	    }
	}

      /* parse the whole thing and free the buffer */
      if (buffer != NULL)
	{
	  ReadCSSRules (doc, css, buffer, css->url, 0, FALSE, NULL);
	  TtaFreeMemory (buffer);
	}
    }
}

/*----------------------------------------------------------------------
  EmbedStyleSheets style sheets of document docsrc become embeded of
  document docdest.
  ----------------------------------------------------------------------*/
void EmbedStyleSheets (Document docsrc, Document docdest)
{
  CSSInfoPtr      css;
  PInfoPtr        pInfo, prevInfo;

  css = CSSList;
  while (css)
    {
      if (css->infos[docsrc])
	{
	  /* move all applied CSS style sheets */
	  pInfo = css->infos[docsrc];
	  /* at the end of the encosing document list */
	  prevInfo = css->infos[docdest];
	  while (prevInfo && prevInfo->PiNext)
	    prevInfo = prevInfo->PiNext;
	  while (pInfo)
	    {
	      if (prevInfo)
		prevInfo->PiNext = pInfo;
	      else
		css->infos[docdest] = pInfo;
	      pInfo->PiEnabled = TRUE;
	      pInfo->PiCategory = CSS_EMBED;
	      css->infos[docsrc] = pInfo->PiNext;
	      pInfo->PiNext = NULL;
	      prevInfo = pInfo;
	      pInfo = css->infos[docsrc];
	    }
	}
      css = css->NextCSS;
    }
}

/*----------------------------------------------------------------------
   AddStyle adds a style.
   It could be an external CSS file, the User Stylesheet or a
   document Style element.
   category specifies the CSS category.
  ----------------------------------------------------------------------*/
void AddStyle (char *url, Document doc, Element link, CSSCategory category)
{
  CSSInfoPtr      css, match;
  PInfoPtr        pInfo;
  DisplayMode     dispMode;
  int             media;

  if (category == CSS_USER_STYLE)
    {
      /* Change the Display Mode to take into account the new presentation */
      dispMode = TtaGetDisplayMode (doc);
      if (dispMode != NoComputedDisplay)
	TtaSetDisplayMode (doc, NoComputedDisplay);
      LoadUserStyleSheet (doc);
      /* Restore the display mode */
      if (dispMode != NoComputedDisplay)
	TtaSetDisplayMode (doc, dispMode);
    }
  else
    {
      pInfo = NULL;
      css = CSSList;
      match = NULL;
      while (css)
	{
	  if ((url &&
	       ((css->url && !strcmp (url, css->url)) ||
		(css->localName && !strcmp (url, css->localName)))) ||
	      (url == NULL && doc && css->doc == doc))
	    {
	      if (doc == 0)
		{
		  /* no specific document is requested */
		  match = css;
		  css = NULL;
		}
	      else
		{
		  /* look for an entry with the right link */
		  pInfo = css->infos[doc];
		  while (!match && pInfo)
		    {
		      if (pInfo->PiLink == link &&
			  pInfo->PiCategory == category)
			{
			  match = css;
			  css = NULL;
			}
		      else if (pInfo->PiCategory == category &&
			       category != CSS_DOCUMENT_STYLE &&
			       category != CSS_USER_STYLE)
			{
			  match = css;
			  css = NULL;
			}
		      else
			pInfo = pInfo->PiNext;
		    }
		}
	    }
	  if (css)
	    css = css->NextCSS;
	}

      css = match;
      if (css)
	{
	  /* Change the Display Mode to take into account the new presentation */
	  dispMode = TtaGetDisplayMode (doc);
	  if (dispMode != NoComputedDisplay)
	    TtaSetDisplayMode (doc, NoComputedDisplay);
	  if (pInfo)
	    {
	      link = pInfo->PiLink;
	      media = pInfo->PiMedia;
	      pInfo->PiEnabled = TRUE;
	    }
	  else
	    media = CSS_ALL;
	  LoadStyleSheet (url, doc, link, NULL, (CSSmedia)media, FALSE);
	  /* Restore the display mode */
	  if (dispMode != NoComputedDisplay)
	    TtaSetDisplayMode (doc, dispMode);
	}
    }
}


/*----------------------------------------------------------------------
  UpdateStyleSheet removes then reloads a style sheet url to all related
  documents.
  The parameter tempdoc points to the updated CSS file.
  ----------------------------------------------------------------------*/
void UpdateStyleSheet (char *url, char *tempdoc)
{
  CSSInfoPtr          css, refcss;
  Document            doc;
  PInfoPtr            pInfo, refInfo, nextInfo;
  DisplayMode         dispMode;
  Element             el;
  int		      position;
  int		      distance;
  ThotBool            found;

  css = CSSList;
  found = FALSE;
  while (css)
    {
      if (url && ((css->url && !strcmp (url, css->url)) ||
		  (css->localName && !strcmp (url, css->localName))))
	{
	/* an external CSS */
	  found = FALSE;
	  /* update the copy in .amaya/0 */
	  if (css->localName && tempdoc)
	    TtaFileCopy (tempdoc, css->localName);
	  doc = 1;
	  while (doc < DocumentTableLength)
	    {
	      /* don't manage a document used by make book */
	      if (DocumentURLs &&
		  (DocumentMeta[doc] == NULL ||
		   DocumentMeta[doc]->method != CE_MAKEBOOK))
		{
		  pInfo = css->infos[doc];
		  while (pInfo)
		    {
		      nextInfo = pInfo->PiNext;
		      if (pInfo->PiEnabled)
			{
			  refcss = css;
			  /* if it's an imported CSS updates the main css */
			  if (pInfo->PiCategory == CSS_IMPORT)
			    refcss = (CSSInfoPtr) pInfo->PiLink;

			  /* get the current position in the document */
			  position = RelativePosition (doc, &distance);
			  /* Change the Display Mode to take into account the
			     new presentation */
			  dispMode = TtaGetDisplayMode (doc);
			  if (dispMode == DisplayImmediately)
			    TtaSetDisplayMode (doc, NoComputedDisplay);
			  /* invalidate current logs */
			  CloseLogs (doc);
			  if (refcss && refcss->infos[doc])
			    {
			      refInfo = refcss->infos[doc];
			      /* re-apply that CSS to each related document */
			      UnlinkCSS (refcss, doc, refInfo->PiLink, TRUE, FALSE);
			      refInfo->PiEnabled = TRUE;
			      if (refInfo->PiCategory == CSS_DOCUMENT_STYLE)
				EnableStyleElement (doc, refInfo->PiLink);
			      else if (UserCSS && !strcmp (refcss->url, UserCSS))
				LoadUserStyleSheet (doc);
			      else
				LoadStyleSheet (refcss->url, doc, refInfo->PiLink, NULL,
						(CSSmedia)refInfo->PiMedia,
						refInfo->PiCategory == CSS_USER_STYLE);
			      if (CSSErrorsFound)
				{
				  /* the CSS parser detected an error */
				  TtaWriteClose (ErrFile);
				  ErrFile = NULL;
				  TtaSetItemOn (doc, 1, File, BShowLogFile);
				  CSSErrorsFound = FALSE;
				  InitInfo ("", TtaGetMessage (AMAYA, AM_CSS_ERROR));
				}
			      else
				TtaSetItemOff (doc, 1, File, BShowLogFile);
			      /* Restore the display mode */
			      if (dispMode == DisplayImmediately)
				TtaSetDisplayMode (doc, dispMode);
			    }
			  /* restore the current position in the document */
			  el = ElementAtPosition (doc, position);
			  TtaShowElement (doc, 1, el, distance);
			}
		      /* manage the next entry */
		      pInfo = nextInfo;
		    }
		}
	      doc++;
	    }
	}
      css = css->NextCSS;
    }
}


/*----------------------------------------------------------------------
  CssToPrint stores CSS files to be sent to print into the printing
  directory.
  Return the list of temporary file names. That list includes:
  - first the User style sheet
  - the external style sheets linked to the document
  - the style sheet embedded in a style element within the document
  Each filename is preceded by "u" and a space if it's a user style sheet,
  by "a" and a space if it's an author style sheet.
  The returned string should be freed by the caller.
  ----------------------------------------------------------------------*/
char *CssToPrint (Document doc, char *printdir)
{
  Element             el, head;
  ElementType         elType;
  FILE               *file;
  CSSInfoPtr          css;
  PInfoPtr            pInfo;
  char               *ptr, *text, *name;
  char                tempfile[MAX_LENGTH];
  int                 length, i, index;

  css = CSSList;
  file = NULL;
  ptr = NULL;
  length = 0;
  if (doc && printdir)
    {
      /* compute the length of the list */
      while (css)
	{
	  if (css->infos[doc])
	    {
	      pInfo = css->infos[doc];
	      while (pInfo)
		{
		  if (pInfo->PiEnabled)
		    {
		      if (pInfo->PiCategory == CSS_DOCUMENT_STYLE)
			/* there is an internal style in the document */
			length += strlen (printdir) + 10;
		      else
			/* that external or user style sheet concerns the
			   document */
			if (css->localName)
			  length += strlen (css->localName) + 3;
		    }
		  pInfo = pInfo->PiNext;
		}
	    }
	  css = css->NextCSS;
	}

      /* now generate the complete list of style sheets */
      if (length)
	{
	  ptr = (char *)TtaGetMemory (length + 1);
	  length = 0;

	  /* Add first the User style sheet */
	  css = CSSList;
	  while (css)
	    {
	      if (css->infos[doc])
		{
		  pInfo = css->infos[doc];
		  while (pInfo)
		    {
		      if (pInfo->PiEnabled &&
			  pInfo->PiCategory == CSS_USER_STYLE &&
			  css->localName)
			{
			  /* add that file name to the list */
			  strcpy (&ptr[length], "u\"");
			  length += 2;
			  strcpy (&ptr[length], css->localName);
			  length += strlen (css->localName);
			  ptr[length++] = '"';
			}
		      pInfo = pInfo->PiNext;
		    }
		}
	      css = css->NextCSS;
	    }

	  /* Add after the list of external CSS files */
	  css = CSSList;
	  while (css)
	    {
	      if (css->infos[doc])
		{
		  pInfo = css->infos[doc];
		  while (pInfo)
		    {
		      if (pInfo->PiEnabled &&
			  css->localName &&
			  (pInfo->PiCategory == CSS_EXTERNAL_STYLE ||
			   pInfo->PiCategory == CSS_IMPORT))
			{
			  /* add that file name to the list */
			  strcpy (&ptr[length], "a\"");
			  length += 2;
			  strcpy (&ptr[length], css->localName);
			  length += strlen (css->localName);
			  ptr[length++] = '"';
			}
		      pInfo = pInfo->PiNext;
		    }
		}
	      css = css->NextCSS;
	    }

	  /* Then look for style elements within the document */
	  el = TtaGetMainRoot (doc);
	  elType = TtaGetElementType (el);
	  name = TtaGetSSchemaName (elType.ElSSchema);
	  index = 0;
	  if (!strcmp (name, "HTML"))
	    {
	      elType.ElTypeNum = HTML_EL_HEAD;
	      head = TtaSearchTypedElement (elType, SearchForward, el);
	      elType.ElTypeNum = HTML_EL_STYLE_;
	      el = head;
	    }
#ifdef _SVG
	  else if (!strcmp (name, "SVG"))
	    {
	      elType.ElTypeNum = SVG_EL_style__;
	      head = el;
	    }
#endif /* _SVG */
	  else
	    {
	      el = NULL;
	      head = NULL;
	    }
	  while (el != NULL)
	    {
	      el = TtaSearchTypedElementInTree (elType, SearchForward, head, el);
	      if (el)
		{
		  if (!file)
		    {
		      /* build the temporary file name */
		      i = strlen (printdir);
		      strcpy (tempfile, printdir);
		      tempfile[i++] = DIR_SEP;
		      strcpy (&tempfile[i], "css");
		      i += 3;
		      sprintf (&tempfile[i], "%d", index);
		      index++;
		      file = TtaWriteOpen (tempfile);
		      if (file)
			{
			  /* add that file name to the list */
			  strcpy (&ptr[length], "a\"");
			  length += 2;
			  strcpy (&ptr[length], tempfile);
			  length += strlen (tempfile);
			  ptr[length++] = '"';
			}
		    }
		  if (file)
		    {
		      /* insert the style text into the temporary file */
		      text = GetStyleContents (el);
		      if (text)
			{
			  fprintf (file, "%s", text);
			  TtaFreeMemory (text);
			}
		    }
		}
	    }
	  /* now close the ptr string */
	  ptr[length++] = EOS;
	}
    }
  if (file)
    /* close the new css file */
    TtaReadClose (file);
  return ptr;
}


/*----------------------------------------------------------------------
   Callback procedure for dialogue events.                            
  ----------------------------------------------------------------------*/
static void CallbackCSS (int ref, int typedata, char *data)
{
  CSSInfoPtr      css, cssNext;
  PInfoPtr        pInfo, pInfoNext;
  Element         el;
  Element         firstSel, lastSel;
  char           *ptr = NULL, *localname = NULL;
  int             j, firstChar, lastChar;
  int             val, sty;
  CSSCategory     category;
  ThotBool        found;

  val = (int) data;
  category = CSS_Unknown;
  sty = 0; /* document style order */
  switch (ref - BaseCSS)
    {
    case CSSForm:
      TtaDestroyDialogue (ref);
      if (val == 1 && CSSpath[0] != EOS &&
	  /* don't manage a document used by make book */
	  (DocumentMeta[CSSdocument] == NULL ||
	   DocumentMeta[CSSdocument]->method != CE_MAKEBOOK))
	{
	  /* point the URI */
	  ptr = &CSSpath[String_length];
	  localname = TtaGetMessage (AMAYA, AM_LOCAL_CSS);
	  found = FALSE;
	  /* get the category */
	  while (!found && category < DisplayCategory_length)
	    {
	      if (!strncmp (DisplayCategory[category], CSSpath, 2))
		{
		  if (category == CSS_DOCUMENT_STYLE)
		    {
		      j = strlen (localname);
		      /* the document style order */
		      sscanf (&ptr[j], "%d", &sty);
		      ptr[j] = EOS;
		    }
		  found = TRUE;
		}
	      else
		category = (CSSCategory)((int)category + 1);
	    }
	  if (category == DisplayCategory_length)
	    category = CSS_Unknown;

	  switch (CSScase)
	    {
	    case 1:
	      /* display the CSS file */
#ifdef _WX
	      GetAmayaDoc (ptr, NULL, CSSdocument,
			   CSSdocument, CE_CSS, FALSE, NULL, NULL);
#else /* _WX */
	      GetAmayaDoc (ptr, NULL, 0, 0, CE_CSS, FALSE, NULL, NULL);
#endif /* _WX */
	      break;
	    case 2:
	      /* disable the CSS file, but not remove */
	      if (category == CSS_DOCUMENT_STYLE)
		RemoveStyle (NULL, CSSdocument, TRUE, FALSE, CSSlink[sty], category);
	      else
		RemoveStyle (ptr, CSSdocument, TRUE, FALSE, NULL, category);
      	      break;
	    case 3:
	      /* enable the CSS file */
	      if (category == CSS_DOCUMENT_STYLE)
		EnableStyleElement (CSSdocument, CSSlink[sty]);
	      else
		AddStyle (ptr, CSSdocument, NULL, category);
      	      break;
	    case 4:
	      /* remove the link to this file */
	      if (category == CSS_DOCUMENT_STYLE)
		DeleteStyleElement (CSSdocument, CSSlink[sty]);
	      else if (category == CSS_EXTERNAL_STYLE)
		{
		  css = CSSList;
		  while (css)
		    {
		      cssNext = css->NextCSS;
		      if (css->infos[CSSdocument] &&
			  ((css->url && !strcmp (ptr, css->url)) ||
			   (css->localName && !strcmp (ptr, css->localName))))
			{
			  /* we found out the CSS */
			  pInfo = css->infos[CSSdocument];
			  while (pInfo)
			    {
			      pInfoNext = pInfo->PiNext;
			      if (pInfo->PiCategory == category)
				{
				  /* look for the element LINK */
				  el = pInfo->PiLink;
				  RemoveLink (el, CSSdocument);
				  /* give current position */
				  TtaGiveFirstSelectedElement (CSSdocument,
							       &firstSel,
							       &firstChar, &j);
				  TtaGiveLastSelectedElement (CSSdocument,
							      &lastSel, &j, &lastChar);
				  /* register this element in the editing history */
				  TtaOpenUndoSequence (CSSdocument, firstSel,
						       lastSel, firstChar, lastChar);
				  TtaRegisterElementDelete (el, CSSdocument);
				  TtaDeleteTree (el, CSSdocument);
				  TtaCloseUndoSequence (CSSdocument);
				  TtaSetDocumentModified (CSSdocument);
				}
			      /* next info context */
			      pInfo = pInfoNext;
			    }
			}
		      css = cssNext;
		    }
		}
	      break;
	    default:
	      break;
	    }
	}
      /* clean CSSpath */
      CSSpath[0] = EOS;
      TtaFreeMemory (CSSlink);
      CSSlink = NULL;
      break;
    case CSSSelect:
      strcpy (CSSpath, data);      
      break;
    default:
      break;
    }
}

/*----------------------------------------------------------------------
  GenerateStyleListFileForElem
  Create a file (named fileName) containing all the CSS style rules
  that have been applied to element el.
 -----------------------------------------------------------------------*/
static void GenerateStyleListFileForElem (Element el, Document doc,
					  char *fileName)
{
  ElementType         elType;
  FILE               *list;
  int                 n;

  /* There is no style on terminal elements. If el is a terminal element,
     take its parent instead.  */
  elType = TtaGetElementType (el);
  if (elType.ElTypeNum == HTML_EL_TEXT_UNIT ||
      elType.ElTypeNum == HTML_EL_GRAPHICS_UNIT ||
      elType.ElTypeNum == HTML_EL_SYMBOL_UNIT)
    {
      el = TtaGetParent (el);
      if (el)
	elType = TtaGetElementType (el);	  
    }
  /* If the element is not supposed to be seen by the user, take its
     parent instead */
  while (el && TtaHasHiddenException (elType))
    {
      el = TtaGetParent (el);
      if (el)
	elType = TtaGetElementType (el);
    }
  /* open the file */
  list = TtaWriteOpen (fileName);
  /* generate its contents */
  fprintf (list, "\n\n");
  if (TtaIsXmlSSchema(elType.ElSSchema))
    /* it's an element belonging to an generic XML vocabulary */
    fprintf (list, TtaGetMessage (AMAYA, AM_STYLE_APPLIED),
	     TtaGetElementTypeName (elType));
  else
    /* it's an element belonging to a known XML vocabulary */
    fprintf (list, TtaGetMessage (AMAYA, AM_STYLE_APPLIED),
	     GetXMLElementName (elType, doc));
  fprintf (list, TtaGetMessage (AMAYA, AM_LINK_LINE));
  if (el)
    n = TtaListStyleOfCurrentElement (doc, list);
  else
    n = 0;
  if (n == 0)
    {
      fprintf (list, "\n     ");
      fprintf (list, TtaGetMessage (AMAYA, AM_NO_STYLE_FOR_ELEM));
    }
  TtaWriteClose (list);
}

/*----------------------------------------------------------------------
  ShowAppliedStyle shows style applied to the current selected
  element.
 -----------------------------------------------------------------------*/
void ShowAppliedStyle (Document doc, View view)
{
  Element             el;
  Document            newdoc;
  char                fileName[100];
  int                 f, i, l;

  for (i = 1; i < DocumentTableLength; i++)
    if (DocumentURLs[i] && DocumentSource[i] == doc &&
	DocumentTypes[i] == docLog &&
	strstr (DocumentURLs[i], "STYLE.LST"))
      {
	/* the style window this document is already shown. Just raise it */
	TtaRaiseView (i, 1);
	return;
      }

  TtaGiveFirstSelectedElement (doc, &el, &f, &l);
  if (el == NULL)
      InitInfo (TtaGetMessage (AMAYA, AM_ERROR),
		TtaGetMessage (AMAYA, AM_NO_SELECTION));
  else
    {
      /* list CSS rules applied to the current selection */
      sprintf (fileName, "%s%c%d%cSTYLE.LST",
	       TempFileDirectory, DIR_SEP, doc, DIR_SEP);
      if (TtaFileExist (fileName))
	TtaFileUnlink (fileName);
      GenerateStyleListFileForElem (el, doc, fileName);
      newdoc = GetAmayaDoc (fileName, "STYLE.LST", 0, doc, CE_LOG, FALSE,
			    NULL, NULL);
      /* store the relation with the original document */
      if (newdoc)
	{
	  DocumentSource[newdoc] = doc;
	  TtaSetStatus (newdoc, 1, "   ", NULL);
	  /* set the STYLE.LST document in read-only mode */
	  TtaSetDocumentAccessMode (newdoc, 0);
	}
    }
}

/*----------------------------------------------------------------------
  SynchronizeAppliedStyle updates the displayed style information.
 -----------------------------------------------------------------------*/
void SynchronizeAppliedStyle (NotifyElement *event)
{
  Element             el;
  Document            doc;
  char                fileName[100], dirName[100];
  int                 i;

  /* look for the log document that displays the STYLE.LST file associated
     with the document */
  doc = event->document;
  el = event->element;
  for (i = 1; i < DocumentTableLength; i++)
    if (DocumentURLs[i] && DocumentSource[i] == doc &&
	DocumentTypes[i] == docLog &&
	strstr (DocumentURLs[i], "STYLE.LST"))
      {
	/* list CSS rules applied to the current selection */
	sprintf (dirName, "%s%c%d",
		 TempFileDirectory, DIR_SEP, doc);
	sprintf (fileName, "%s%c%d%cSTYLE.LST",
		 TempFileDirectory, DIR_SEP, doc, DIR_SEP);
	if (TtaFileExist (fileName))
	  TtaFileUnlink (fileName);
	GenerateStyleListFileForElem (el, doc, fileName);
	StartParser (i, fileName, "STYLE.LST", dirName, "STYLE.LST", TRUE, FALSE);
        /* set the STYLE.LST document in read-only mode */
        TtaSetDocumentAccessMode (i, 0);
	return;
    }
}

/*----------------------------------------------------------------------
   InitCSS                                                         
  ----------------------------------------------------------------------*/
void InitCSS (void)
{
   /* initialize the dialogs */
   BaseCSS = TtaSetCallback ((Proc)CallbackCSS, MAX_CSS_REF);
   CSSpath[0] = EOS;
}

/*----------------------------------------------------------------------
   InitCSSDialog list downloaded CSS files
  ----------------------------------------------------------------------*/
static void InitCSSDialog (Document doc, char *s)
{
  CSSInfoPtr          css;
  PInfoPtr            pInfo;
  ElementType	      elType;
  char               *name;
  char               *buf = NULL;
  char               *ptr, *localname;
  int                 i, select;
  int                 len, nb, sty;
  int                 index, size;  

  CSSdocument = doc;
  localname = TtaGetMessage (AMAYA, AM_LOCAL_CSS);
  /* clean up the list of links */
  TtaFreeMemory (CSSlink);
  CSSlink = NULL;
  index = 0;
  nb = 0; /* number of entries */
  sty = 0; /* number of style elements */
  size = 1;
#ifdef _GTK
  /* create the form */
  TtaNewSheet (BaseCSS + CSSForm, TtaGetViewFrame (doc, 1), s, 1,
	       TtaGetMessage(LIB, TMSG_LIB_CONFIRM), TRUE, 1, 'L', D_DONE);
#endif /* _GTK */
  select = -1;
  i = 0;
  css = CSSList;
  /* count the number of menu entries */
  while (css)
    {
      pInfo = css->infos[doc];
      while (pInfo)
	{
	  if (pInfo &&
	      pInfo->PiCategory != CSS_EMBED &&
	      /* the document style cannot be open */
	      ((CSScase == 1 && pInfo->PiCategory != CSS_DOCUMENT_STYLE) ||
	       /* it's impossible to disable an imported style sheet */
	       (CSScase == 2 && pInfo->PiEnabled &&
		pInfo->PiCategory != CSS_IMPORT) ||
	       /* it's impossible to enable an imported style sheet */
	       (CSScase == 3 && !pInfo->PiEnabled &&
		pInfo->PiCategory != CSS_IMPORT) ||
	       /* only en external sheet can be removed */
	       (CSScase == 4 && pInfo->PiCategory == CSS_EXTERNAL_STYLE)))
	    {
	      nb++;
	      if (pInfo->PiCategory == CSS_DOCUMENT_STYLE)
		{
		  size += strlen (localname) + 11;
		  /* count the number of style element */ 
		  sty++;
		}
	      else if (css->url == NULL && css->localName)
		 size += strlen (css->localName) + 12;
	      else
		 size += strlen (css->url) + 12;
	    }
	  pInfo = pInfo->PiNext;
	}
      css = css->NextCSS;
    }

  if (nb > 0)
    {
      i = 0;
      /* create the link list */
      CSSlink = (Element *) TtaGetMemory (sty * sizeof (Element));
      buf = (char *)TtaGetMemory (size);
      sty = 0;
      /* initialize menu entries */
      css = CSSList;
      while (css)
	{
	  pInfo = css->infos[doc];
	  while (pInfo)
	    {
	      if (pInfo &&
		  pInfo->PiCategory != CSS_EMBED &&
		  pInfo->PiCategory != CSS_USER_STYLE &&
		  /* the document style cannot be open */
		  ((CSScase == 1 && pInfo->PiCategory != CSS_DOCUMENT_STYLE) ||
		   /* it's impossible to disable an imported style sheet */
		   (CSScase == 2 && pInfo->PiEnabled &&
		    pInfo->PiCategory != CSS_IMPORT) ||
		   /* it's impossible to enable an imported style sheet */
		   (CSScase == 3 && !pInfo->PiEnabled &&
		    pInfo->PiCategory != CSS_IMPORT) ||
		   /* only en external sheet can be removed */
		   (CSScase == 4 && pInfo->PiCategory == CSS_EXTERNAL_STYLE)))
		{
		  /* filter enabled and disabled entries */
		  /* build the CSS list:
		     use the dialogue encoding for buf and UTF-8 for CSS path  */
		  if (pInfo->PiCategory == CSS_DOCUMENT_STYLE)
		    {
		      /* skip HTML style attributes */
		      elType = TtaGetElementType (pInfo->PiLink);
		      name = TtaGetSSchemaName (elType.ElSSchema);
		      if (!strcmp (name, "HTML") && elType.ElTypeNum == HTML_EL_STYLE_)
			{
			  ptr = (char *)TtaGetMemory (strlen (localname) + 11);
			  sprintf (ptr, "%s%d", localname, sty);
			  CSSlink[sty++] = pInfo->PiLink;
			}
		      else
			{
			  /* skip this entry */
			  ptr = NULL;
			  nb--;
			  if (nb == 0)
			    {
			      TtaFreeMemory (CSSlink);
			      CSSlink = NULL;
			    }
			}
		    }
		  else if (css->url == NULL && css->localName)
		    ptr = css->localName;
		  else
		    ptr = css->url;
		  if (ptr)
		    {
		      len = strlen (ptr) + 1; /* + EOS */
		      if (size < len + String_length)
			break;
		      /* display the category */
		      strcpy (&buf[index], DisplayCategory[pInfo->PiCategory]);
		      index += String_length;
		      strcpy (&buf[index], ptr);
		      index += len;
		      size -= len;
		      if (select == -1 &&
			  (CSScase < 4 || pInfo->PiCategory == CSS_EXTERNAL_STYLE))
			{
			  if (pInfo->PiCategory == CSS_DOCUMENT_STYLE)
			    {
			      strcpy (CSSpath, DisplayCategory[CSS_DOCUMENT_STYLE]);
			      strcat (CSSpath, ptr);
			    }
			  else
			    {
			      strcpy (CSSpath, DisplayCategory[pInfo->PiCategory]);
			      if (css->url)
				strcat (CSSpath, css->url);
			      else
				strcat (CSSpath, css->localName);
			    }
			  select = i;
			}
		      i++;
		    }
		}
	      pInfo = pInfo->PiNext;
	    }
	  css = css->NextCSS;
	}
      /* add the CSS_USER_SYLE entry */
      css = CSSList;
      while (css)
	{
	  pInfo = css->infos[doc];
	  while (pInfo)
	    {
	      if (pInfo &&
		  pInfo->PiCategory == CSS_USER_STYLE &&
		   /* it's impossible to disable an imported style sheet */
		  (CSScase == 1 ||
		   (CSScase == 2 && pInfo->PiEnabled) ||
		   /* it's impossible to enable an imported style sheet */
		   (CSScase == 3 && !pInfo->PiEnabled)))
		{
		  /* filter enabled and disabled entries */
		  /* build the CSS list:
		     use the dialogue encoding for buf and UTF-8 for CSS path  */
		  if (css->url == NULL && css->localName)
		    ptr = css->localName;
		  else
		    ptr = css->url;
		  if (ptr)
		    {
		      len = strlen (ptr) + 1; /* + EOS */
		      if (size < len + String_length)
			break;
		      /* display the category */
		      strcpy (&buf[index], DisplayCategory[pInfo->PiCategory]);
		      index += String_length;
		      strcpy (&buf[index], ptr);
		      index += len;
		      size -= len;
		      if (select == -1 && CSScase < 4)
			{
			  strcpy (CSSpath, DisplayCategory[pInfo->PiCategory]);
			  if (css->url)
			    strcat (CSSpath, css->url);
			  else
			    strcat (CSSpath, css->localName);
			  select = i;
			}
		      i++;
		    }
		}
	      pInfo = pInfo->PiNext;
	    }
	  css = css->NextCSS;
	}
    }

  /* display the form */
#ifdef _WX
  ThotBool created = CreateCSSDlgWX (BaseCSS + CSSForm, BaseCSS + CSSForm + 1,
				     TtaGetViewFrame (doc, 1),
				     s, nb, buf);
  TtaFreeMemory (buf);
  if (created)
    TtaShowDialogue (BaseCSS + CSSForm, TRUE);
#endif /* _WX */

#ifdef _WINGUI
  CreateCSSDlgWindow (TtaGetViewFrame (doc, 1), nb, buf, s,
		      TtaGetMessage (AMAYA, AM_NO_CSS));
  TtaFreeMemory (buf);
#endif /* _WINGUI */

#ifdef _GTK
  if (nb > 0)
    {
      if (nb >= 10)
	i = 10;
      else
	i = nb + 1;
      TtaNewSizedSelector (BaseCSS + CSSSelect, BaseCSS + CSSForm,
			   TtaGetMessage (AMAYA, AM_CSS_FILE),
			   nb, buf, 350, i, NULL, FALSE, TRUE);
    }
  else
    TtaNewLabel (BaseCSS + CSSSelect, BaseCSS + CSSForm,
		 TtaGetMessage (AMAYA, AM_NO_CSS));
  TtaFreeMemory (buf);
  TtaShowDialogue (BaseCSS + CSSForm, TRUE);
  if (nb > 0)
    TtaSetSelector (BaseCSS + CSSSelect, select, NULL);
#endif /* _GTK */
}

/*----------------------------------------------------------------------
   LinkCSS
   Add a new link to a CSS file
  ----------------------------------------------------------------------*/
void LinkCSS (Document doc, View view)
{
  SSchema           docSchema;

  docSchema = TtaGetDocumentSSchema (doc);
  /* LinkAsCSS and LinkAsXmlCSS will be cleared by
     SetREFattribute or by CallbackDialogue */
  if (strcmp(TtaGetSSchemaName (docSchema), "HTML") != 0)
    {
      /* Create a style within a XML document */
      LinkAsXmlCSS = TRUE;
      InsertCssInXml (doc, view);
    }
  else
    {
      LinkAsCSS = TRUE;
      CreateLinkInHead (doc, 1);
    }
}

/*----------------------------------------------------------------------
   OpenCSS lists downloaded CSS files
  ----------------------------------------------------------------------*/
void                OpenCSS (Document doc, View view)
{
  CSScase = 1;
  InitCSSDialog (doc, TtaGetMessage (AMAYA, AM_OPEN_CSS));
}

/*----------------------------------------------------------------------
   DisableCSS list downloaded CSS files
  ----------------------------------------------------------------------*/
void                DisableCSS (Document doc, View view)
{
  CSScase = 2;
  InitCSSDialog (doc, TtaGetMessage (AMAYA, AM_DISABLE_CSS));
}

/*----------------------------------------------------------------------
  EnableCSS list downloaded CSS files
  ----------------------------------------------------------------------*/
void                EnableCSS (Document doc, View view)
{
  CSScase = 3;
  InitCSSDialog (doc, TtaGetMessage (AMAYA, AM_ENABLE_CSS));
}

/*----------------------------------------------------------------------
   RemoveCSS lists downloaded CSS files
  ----------------------------------------------------------------------*/
void                RemoveCSS (Document doc, View view)
{
  CSScase = 4;
  InitCSSDialog (doc, TtaGetMessage (AMAYA, AM_REMOVE_CSS));
}
