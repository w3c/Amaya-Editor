/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2003
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *
 * Author: I. Vatton
 *
 */

/* nItagetMecluded headerfiles */
#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"
#ifdef _SVG
#include "SVG.h"
#endif /* _SVG */
#ifdef _WINDOWS
#include "wininclude.h"
#endif /* _WINDOWS */

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
#include "HTMLedit_f.h"
#include "UIcss_f.h"
#include "css_f.h"
#include "dialog.h"
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
      toparse = GetObjectWWW (doc, completeURL, NULL, localfile,
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
	  rename (localfile, tempdocument);
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
   LoadUserStyleSheet : Load the user Style Sheet found in his/her   
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
  if (css && pInfo)
    /* already applied */
    return;
  else if (css == NULL)
    {
      /* store a copy of the local CSS in .amaya/0 */
      ptr = GetLocalPath (0, UserCSS);
      TtaFileCopy (UserCSS, ptr);
      /* allocate a new Presentation structure */ 
      css = AddCSS (0, doc, CSS_USER_STYLE, CSS_ALL, UserCSS, ptr, NULL);
      TtaFreeMemory (ptr);
    }
  else
    AddInfoCSS (doc, css, CSS_USER_STYLE, CSS_ALL, NULL);

  ptr = css->localName;
  if (ptr[0] != EOS  && TtaFileExist (ptr))
    {
      /* read User preferences */
      res = fopen (ptr, "r");
      if (res != NULL)
	{
#ifdef _WINDOWS
	  if (fstat (_fileno (res), &buf))
#else  /* !_WINDOWS */
	  if (fstat (fileno (res), &buf))
#endif /* !_WINDOWS */
	    fclose (res);
	  else
	    {
	      buffer = TtaGetMemory (buf.st_size + 1000);
	      if (buffer == NULL)
		fclose (res);
	      else
		{
		  len = fread (buffer, buf.st_size, 1, res);
		  if (len != 1)
		    {
		      TtaFreeMemory (buffer);
		      buffer = NULL;
		      fclose (res);
		    }
		  else
		    {
		      buffer[buf.st_size] = 0;
		      fclose (res);
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
	  while (prevInfo->PiNext)
	    prevInfo = prevInfo->PiNext;
	  while (pInfo)
	    {
	      prevInfo->PiNext = pInfo;
	      pInfo->PiEnabled = TRUE;
	      pInfo->PiCategory = CSS_EMBED;
	      css->infos[docsrc] = pInfo->PiNext;
	      pInfo->PiNext = NULL;
	      pInfo = css->infos[docsrc];
	    }
	}
      css = css->NextCSS;
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
			  /* Change the Display Mode to take into account the
			     new presentation */
			  dispMode = TtaGetDisplayMode (doc);
			  if (dispMode == DisplayImmediately)
			    TtaSetDisplayMode (doc, NoComputedDisplay);
			  if (refcss && refcss->infos[doc])
			    {
			      refInfo = refcss->infos[doc];
			      /* re-apply that CSS to each related document */
			      UnlinkCSS (refcss, doc, refInfo->PiLink, TRUE, FALSE);
			      refInfo->PiEnabled = TRUE;
			      if (UserCSS && !strcmp (refcss->url, UserCSS))
				LoadUserStyleSheet (doc);
			      else
				LoadStyleSheet (refcss->url, doc, refInfo->PiLink, NULL,
						refInfo->PiMedia,
						refInfo->PiCategory == CSS_USER_STYLE);
			      /* Restore the display mode */
			      if (dispMode == DisplayImmediately)
				TtaSetDisplayMode (doc, dispMode);
			    }
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
  int                 length, i;

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
			length += strlen (printdir) + 7;
		      else
			/* that external or user style sheet concerns the document */
			length += strlen (css->localName) + 3;
		    }
		  else
		    pInfo = pInfo->PiNext;
		}
	    }
	  css = css->NextCSS;
	}

      /* now generate the complete list of style sheets */
      if (length)
	{
	  ptr = TtaGetMemory (length + 1);
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
		      if (pInfo->PiEnabled && pInfo->PiCategory == CSS_USER_STYLE)
			{
			  /* add that file name to the list */
			  strcpy (&ptr[length], "u ");
			  length += 2;
			  strcpy (&ptr[length], css->localName);
			  length += strlen (css->localName);
			  ptr[length++] = SPACE;
			}
		      else
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
			  (pInfo->PiCategory == CSS_EXTERNAL_STYLE ||
			   pInfo->PiCategory == CSS_IMPORT))
			{
			  /* add that file name to the list */
			  strcpy (&ptr[length], "a ");
			  length += 2;
			  strcpy (&ptr[length], css->localName);
			  length += strlen (css->localName);
			  ptr[length++] = SPACE;
			}
		      else
			pInfo = pInfo->PiNext;
		    }
		}
	      css = css->NextCSS;
	    }

	  /* Then look for style elements within the document */
	  el = TtaGetMainRoot (doc);
	  elType = TtaGetElementType (el);
	  name = TtaGetSSchemaName (elType.ElSSchema);
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
		      file = fopen (tempfile, "w");
		      if (file)
			{
			  /* add that file name to the list */
			  strcpy (&ptr[length], "a ");
			  length += 2;
			  strcpy (&ptr[length], tempfile);
			  length += strlen (tempfile);
			  ptr[length++] = SPACE;
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
    fclose (file);
  return ptr;
}


/*----------------------------------------------------------------------
   Callback procedure for dialogue events.                            
  ----------------------------------------------------------------------*/
static void CallbackCSS (int ref, int typedata, char *data)
{
  CSSInfoPtr      css;
  PInfoPtr        pInfo;
  Element         el;
  Element         firstSel, lastSel;
  char           *ptr = NULL, *localname = NULL;
  int             j, firstChar, lastChar;
  int             val, category, sty;
  ThotBool        found;

  val = (int) data;
  category = 0;
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
		      scanf (&ptr[j], "%d", &sty);
		      ptr[j] = EOS;
		    }
		  found = TRUE;
		}
	      else
		category++;
	    }
	  if (category == DisplayCategory_length)
	    category = 0;

	  switch (CSScase)
	    {
	    case 1:
	      /* display the CSS file */
	      GetAmayaDoc (ptr, NULL, 0, 0, CE_CSS, FALSE, NULL, NULL, UTF_8);
	      break;
	    case 2:
	      /* disable the CSS file, but not remove */
	      if (category == CSS_DOCUMENT_STYLE)
		{
		  css = SearchCSS (CSSdocument, NULL, CSSlink[sty], &pInfo);
		  if (pInfo)
		     UnlinkCSS (css, CSSdocument, CSSlink[sty], TRUE, FALSE);
		}
	      else
		{
		  css = SearchCSS (CSSdocument, ptr, NULL, &pInfo);
		  RemoveStyleSheet (ptr, CSSdocument, TRUE, FALSE, NULL);
		}
      	      break;
	    case 3:
	      /* enable the CSS file */
	      if (category == CSS_DOCUMENT_STYLE)
		{
		  /* style element */
		  css = SearchCSS (CSSdocument, NULL, CSSlink[sty], &pInfo);
		  if (pInfo)
		    {
		      pInfo->PiEnabled = TRUE;
		      EnableStyleElement (CSSdocument);
		    }
		}
	      else
		{
		  /* external style sheet */
		  css = SearchCSS (CSSdocument, ptr, NULL, &pInfo);
		  if (pInfo)
		    {
		      pInfo->PiEnabled = TRUE;
		      /* apply CSS rules */
		      if (UserCSS && !strcmp (ptr, UserCSS))
			LoadUserStyleSheet (CSSdocument);
		      else
			LoadStyleSheet (ptr, CSSdocument,
					pInfo->PiLink, NULL,
					pInfo->PiMedia,
					pInfo->PiCategory == CSS_USER_STYLE);
		    }
		}
      	      break;
	    case 4:
	      /* remove the link to this file */
	      if (category == CSS_DOCUMENT_STYLE)
		DeleteStyleElement (CSSdocument);
	      else if (category == CSS_EXTERNAL_STYLE)
		{
		  css = CSSList;
		  while (css)
		    {
		      if (css->infos[CSSdocument] &&
			  ((css->url && !strcmp (ptr, css->url)) ||
			   (css->localName && !strcmp (ptr, css->localName))))
			{
			  /* we found out the CSS */
			  pInfo = css->infos[CSSdocument];
			  while (pInfo)
			    {
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
			      pInfo = pInfo->PiNext;
			    }
			}
		      css = css->NextCSS;
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
   InitCSS                                                         
  ----------------------------------------------------------------------*/
void InitCSS (void)
{
   /* initialize the dialogs */
   BaseCSS = TtaSetCallback (CallbackCSS, MAX_CSS_REF);
   CSSpath[0] = EOS;
}

/*----------------------------------------------------------------------
   InitCSSDialog list downloaded CSS files
  ----------------------------------------------------------------------*/
static void InitCSSDialog (Document doc, char *s)
{
  CSSInfoPtr          css;
  PInfoPtr            pInfo;
  char                buf[400];
  char               *ptr, *localname;
  int                 i, select;
  int                 len, nb, sty;
  int                 index, size;  

  CSSdocument = doc;
  localname = TtaGetMessage (AMAYA, AM_LOCAL_CSS);
  /* clean up the list of links */
  TtaFreeMemory (CSSlink);
  CSSlink = NULL;
  buf[0] = 0;
  index = 0;
  nb = 0; /* number of entries */
  sty = 0; /* number of style elements */
  size = 400;
#ifndef _WINDOWS
  /* create the form */
  TtaNewSheet (BaseCSS + CSSForm, TtaGetViewFrame (doc, 1), s, 1,
	       TtaGetMessage(LIB, TMSG_LIB_CONFIRM), TRUE, 1, 'L', D_DONE);
#endif /* !_WINDOWS */
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
	      if (pInfo->PiCategory != CSS_DOCUMENT_STYLE)
		/* count the number of style element */ 
		sty++;
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
		      ptr = TtaGetMemory (strlen (localname) + 11);
		      sprintf (ptr, "%s%d", localname, sty);
		      CSSlink[sty++] = pInfo->PiLink;
		    }
		  else
		    {
		      if (css->url == NULL)
			ptr = TtaConvertMbsToByte (css->localName,
						   TtaGetDefaultCharset ());
		      else
			ptr = TtaConvertMbsToByte (css->url,
						   TtaGetDefaultCharset ());
		    }
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
		  TtaFreeMemory (ptr);
		  i++;
		}
	      pInfo = pInfo->PiNext;
	    }
	  css = css->NextCSS;
	}
    }

  /* display the form */
#  ifdef _WINDOWS
  CreateCSSDlgWindow (TtaGetViewFrame (doc, 1), nb, buf, s,
		      TtaGetMessage (AMAYA, AM_NO_CCS_FILE));
#  else  /* !_WINDOWS */
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
		 TtaGetMessage (AMAYA, AM_NO_CCS_FILE));
  TtaShowDialogue (BaseCSS + CSSForm, TRUE);
  if (nb > 0)
    TtaSetSelector (BaseCSS + CSSSelect, select, NULL);
#  endif /* !_WINDOWS */
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
