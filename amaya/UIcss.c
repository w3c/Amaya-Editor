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

/* Included headerfiles */
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
static char       *CSSpath;
static Document    CSSdocument;

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
ThotBool  LoadRemoteStyleSheet (char *url, Document doc, Element el,
				CSSInfoPtr css, char *completeURL,
				char *localfile)
{
  CSSInfoPtr          oldcss;
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
  oldcss = SearchCSS (0, completeURL, NULL);
  remote = IsW3Path (completeURL);
  if (oldcss)
    strcpy (localfile, oldcss->localName);
  else if (remote)
    {
      /* the CSS is not loaded yet */
      /* changed this to doc */
      toparse = GetObjectWWW (doc, completeURL, NULL, localfile,
			      AMAYA_SYNC | AMAYA_LOAD_CSS, NULL, NULL,
			      NULL, NULL, NO, NULL);
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
  struct stat         buf;
  FILE               *res;
  char               *buffer, *ptr;
  int                 len;

  /* look for the User preferences */
  if (!UserCSS || !TtaFileExist (UserCSS))
    return;

  buffer = NULL;
  ptr = UserCSS;
  css = CSSList;
  while (css != NULL)
    {
      if (css->category == CSS_USER_STYLE)
	break;
      else
	css = css->NextCSS;
    }

  if (css == NULL)
    {
      /* store a copy of the local CSS in .amaya/0 */
      ptr = GetLocalPath (0, UserCSS);
      TtaFileCopy (UserCSS, ptr);
      /* allocate a new Presentation structure */ 
      css = AddCSS (0, doc, CSS_USER_STYLE, UserCSS, ptr, NULL);
      TtaFreeMemory (ptr);
      ptr = css->localName;
    }
  else
    {
      /* we have to apply user preferences to this document */
      ptr = css->localName;
      css->documents[doc] = TRUE;
      css->enabled[doc] = TRUE;
    }

  if (css->enabled[doc] && ptr[0] != EOS  && TtaFileExist (ptr))
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
   UpdateStyleSheet removes then reload a style sheet url to all related
   documents.
   The parameter tempdoc points to the updated CSS file.
  ----------------------------------------------------------------------*/
void  UpdateStyleSheet (char *url, char *tempdoc)
{
  CSSInfoPtr          css;
  Document            doc;
  DisplayMode         dispMode;
  ThotBool            found;

  css = CSSList;
  found = FALSE;
  while (css && !found)
    {
      if (url && ((css->url && !strcmp (url, css->url)) ||
		  (css->localName && !strcmp (url, css->localName))))
	/* an external CSS */
	found = TRUE;
      else
	css = css->NextCSS;
    }

  if (css)
    {
      /* update the copy in .amaya/0 */
      if (css->localName && tempdoc)
	  TtaFileCopy (tempdoc, css->localName);
      /* if it's an imported CSS updates the main css */
      while (css && css->category == CSS_IMPORT)
	css = css->NextCSS;
      if (css)
	{
	  for (doc = 1; doc < DocumentTableLength; doc++)
	    if (css->documents[doc] && css->enabled[doc] &&
		/* don't manage a document used by make book */
		(DocumentMeta[doc] == NULL ||
		 DocumentMeta[doc]->method != CE_MAKEBOOK))
	      {
		/* Change the Display Mode to take into account the new presentation */
		dispMode = TtaGetDisplayMode (doc);
		if (dispMode == DisplayImmediately)
		  TtaSetDisplayMode (doc, NoComputedDisplay);
		/* re-apply that CSS to each related document */
		UnlinkCSS (css, doc, TRUE, FALSE);
		css->enabled[doc] = TRUE;
		if (UserCSS && !strcmp (css->url, UserCSS))
		  LoadUserStyleSheet (doc);
		else
		  LoadStyleSheet (css->url, doc, NULL, NULL,
				  css->media[doc],
				  css->category == CSS_USER_STYLE);
		/* Restore the display mode */
		if (dispMode == DisplayImmediately)
		  TtaSetDisplayMode (doc, dispMode);
	      }
	}
    }
}

/*----------------------------------------------------------------------
  AttrMediaChanged: the user has created removed or modified a Media
  attribute
  ----------------------------------------------------------------------*/
void AttrMediaChanged (NotifyAttribute *event)
{
   ElementType         elType;
   Element             el;
   Document            doc;
   Attribute           attr;
   AttributeType       attrType;
   CSSInfoPtr          css;
   CSSmedia            media;
   DisplayMode         dispMode;
   char                completeURL[MAX_LENGTH];
   char                tempname[MAX_LENGTH];
   char               *name2;
   int                 length;

   el = event->element;
   doc = event->document;
   attr = event->attribute;
   /* avoid too many redisplay */
   dispMode = TtaGetDisplayMode (doc);
   if (dispMode == DisplayImmediately)
     TtaSetDisplayMode (doc, DeferredDisplay);

   elType = TtaGetElementType (el);
   /* get the new media value */
   length = TtaGetTextAttributeLength (attr);
   name2 = TtaGetMemory (length + 1);
   TtaGiveTextAttributeValue (attr, name2, &length);
   if (!strcasecmp (name2, "screen"))
     media = CSS_SCREEN;
   else if (!strcasecmp (name2, "print"))
     media = CSS_PRINT;
   else if (!strcasecmp (name2, "all"))
     media = CSS_ALL;
   else
     media = CSS_OTHER;
   TtaFreeMemory (name2);
   /* get the CSS URI */
   attrType.AttrSSchema = elType.ElSSchema;
   attrType.AttrTypeNum = HTML_ATTR_HREF_;
   attr = TtaGetAttribute (el, attrType);
   if (attr &&
       /* don't manage a document used by make book */
       (DocumentMeta[doc] == NULL ||
	DocumentMeta[doc]->method != CE_MAKEBOOK))
     {
       length = TtaGetTextAttributeLength (attr);
       name2 = TtaGetMemory (length + 1);
       TtaGiveTextAttributeValue (attr, name2, &length);
       /* load the stylesheet file found here ! */
       NormalizeURL (name2, doc, completeURL, tempname, NULL);
       TtaFreeMemory (name2);
       css = SearchCSS (0, completeURL, NULL);
       if (css && css->media[doc] != media)
	 {
	   /* something changed and we are not printing */
	   if ((media == CSS_ALL || media == CSS_SCREEN) &&
	       (css->media[doc] == CSS_PRINT || css->media[doc] == CSS_OTHER))
	     LoadStyleSheet (completeURL, doc, el, NULL, media,
			     css->category == CSS_USER_STYLE);
	   else
	     {
	       if ((media == CSS_PRINT || media == CSS_OTHER) &&
	       (css->media[doc] == CSS_ALL || css->media[doc] == CSS_SCREEN))
		 {
		   if (elType.ElTypeNum != HTML_EL_STYLE_)
		     el = NULL;
		   RemoveStyleSheet (completeURL, doc, FALSE, FALSE, el);
		 }
	       /* only update the CSS media info */
	       css->media[doc] = media;
	     }
	 }
     }

   /* restore the display mode */
   if (dispMode == DisplayImmediately)
     TtaSetDisplayMode (doc, dispMode);
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
  char               *ptr, *text, *name;
  char                tempfile[MAX_LENGTH];
  int                 length, i;

  css = CSSList;
  file = NULL;
  ptr = NULL;
  length = 0;
  if (doc && printdir)
    {
      while (css != NULL)
	{
	  if (css->enabled[doc])
	    {
	    if ( css->category == CSS_DOCUMENT_STYLE)
	      /* there is an internal style in the document */
	      length += strlen (printdir) + 5;
	    else
	      /* that external or user style sheet concerns the document */
	      length += strlen (css->localName) + 1;
	    /* add room for "a " or "u " in front of each filename */
	    length += 2;
	    }
	  css = css->NextCSS;
	}
      if (length)
	{
	  ptr = TtaGetMemory (length + 1);
	  length = 0;

	  /* Add first the User style sheet */
	  css = CSSList;
	  while (css != NULL)
	    {
	      if (css->enabled[doc] &&
		  css->category == CSS_USER_STYLE)
		{
		  /* add that file name to the list */
		  strcpy (&ptr[length], "u ");
		  length += 2;
		  strcpy (&ptr[length], css->localName);
		  length += strlen (css->localName);
		  ptr[length++] = SPACE;
		}
	      css = css->NextCSS;
	    }

	  /* Add after the list of external CSS files */
	  css = CSSList;
	  while (css != NULL)
	    {
	      if (css->enabled[doc] &&
		  (css->category == CSS_EXTERNAL_STYLE ||
		   css->category == CSS_IMPORT))
		{
		  /* add that file name to the list */
		  strcpy (&ptr[length], "a ");
		  length += 2;
		  strcpy (&ptr[length], css->localName);
		  length += strlen (css->localName);
		  ptr[length++] = SPACE;
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
  int             j, firstChar, lastChar;
  int             val, length;
  ThotBool        found;

  val = (int) data;
  switch (ref - BaseCSS)
    {
    case CSSForm:
      TtaDestroyDialogue (ref);
      if (val == 1 && CSSpath &&
	  /* don't manage a document used by make book */
	  (DocumentMeta[CSSdocument] == NULL ||
	   DocumentMeta[CSSdocument]->method != CE_MAKEBOOK))
	{
	  switch (CSScase)
	    {
	    case 1:
	      /* display the CSS file */
	      GetAmayaDoc (CSSpath, NULL, 0, 0, CE_CSS, FALSE, NULL,
			   NULL, UTF_8);
	      break;
	    case 2:
	      /* disable the CSS file, but not remove */
	      if (!strcmp (CSSpath, TtaGetMessage (AMAYA, AM_LOCAL_CSS)))
		RemoveStyleSheet (NULL, CSSdocument, TRUE, FALSE, NULL);
	      else
		RemoveStyleSheet (CSSpath, CSSdocument, TRUE, FALSE, NULL);
      	      break;
	    case 3:
	      /* enable the CSS file */
	      if (!strcmp (CSSpath, TtaGetMessage (AMAYA, AM_LOCAL_CSS)))
		{
		  css = SearchCSS (CSSdocument, NULL, NULL);
		  css ->enabled[CSSdocument] = TRUE;
		  EnableStyleElement (CSSdocument);
		}
	      else
		{
		  css = SearchCSS (0, CSSpath, NULL);
		  css->enabled[CSSdocument] = TRUE;
		  /* apply CSS rules */
		  if (UserCSS && !strcmp (CSSpath, UserCSS))
		    LoadUserStyleSheet (CSSdocument);
		  else
		    LoadStyleSheet (CSSpath, CSSdocument, NULL, NULL,
				    css->media[CSSdocument],
				    css->category == CSS_USER_STYLE);
		}
      	      break;
	    case 4:
	      /* remove the link to this file */
	      css = CSSList;
	      found = FALSE;

	      while (css != NULL && !found)
		{
		  if (css->category == CSS_DOCUMENT_STYLE)
		    DeleteStyleElement (CSSdocument);
		  else if (css->documents[CSSdocument] &&
			   ((css->url && !strcmp (CSSpath, css->url)) ||
			    (css->localName && !strcmp (CSSpath, css->localName))))
		    {
		      /* we found out the CSS */
		      found = TRUE;
		      if (css->category == CSS_EXTERNAL_STYLE)
			{
			  /* look for the element LINK */
			  pInfo = css->infos;
			  while (pInfo != NULL && pInfo->PiDoc != CSSdocument)
			    /* next info context */
			    pInfo = pInfo->PiNext;
			  if (pInfo != NULL && pInfo->PiLink != NULL)
			    {
			      el = pInfo->PiLink;
			      RemoveLink (el, CSSdocument);
			      /* give current position */
			      TtaGiveFirstSelectedElement (CSSdocument, &firstSel, &firstChar, &j);
			      TtaGiveLastSelectedElement (CSSdocument, &lastSel, &j, &lastChar);
			      /* register this element in the editing history */
			      TtaOpenUndoSequence (CSSdocument, firstSel, lastSel, firstChar, lastChar);
			      TtaRegisterElementDelete (el, CSSdocument);
			      TtaDeleteTree (el, CSSdocument);
			      TtaCloseUndoSequence (CSSdocument);
			      TtaSetDocumentModified (CSSdocument);
			    }
			}
		    }
		  css = css->NextCSS;
		}
	      break;
	    default:
	      break;
	    }
	}
      /* clean CSSpath */
      TtaFreeMemory (CSSpath);
      CSSpath = NULL;
      break;
    case CSSSelect:
      TtaFreeMemory (CSSpath);
      length = strlen (data);
      CSSpath = TtaGetMemory (length + 1);
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
   CSSpath = NULL;
}

/*----------------------------------------------------------------------
   InitCSSDialog list downloaded CSS files
  ----------------------------------------------------------------------*/
static void InitCSSDialog (Document doc, char *s)
{
  CSSInfoPtr          css;
  char                buf[400];
  char               *ptr;
  int                 i, select;
  int                 len, nb;
  int                 index, size;  

  CSSdocument = doc;
  css = CSSList;
  buf[0] = 0;
  index = 0;
  nb = 0;
  size = 400;
#ifndef _WINDOWS
  /* create the form */
  TtaNewSheet (BaseCSS + CSSForm, TtaGetViewFrame (doc, 1), s, 1,
	       TtaGetMessage(LIB, TMSG_LIB_CONFIRM), TRUE, 1, 'L', D_DONE);
#endif /* !_WINDOWS */
  select = -1;
  i = 0;
  while (css)
    {
      if (css->documents[doc] &&
	  /* the document style cannot be open */
	  ((CSScase == 1 && css->category != CSS_DOCUMENT_STYLE) ||
	  /* it's impossible to disable an imported style sheet */
	   (CSScase == 2 && css->enabled[doc] && css->category != CSS_IMPORT) ||
	   /* it's impossible to enable an imported style sheet */
	   (CSScase == 3 && !css->enabled[doc] && css->category != CSS_IMPORT) ||
	   /* only en external sheet can be removed */
	   (CSScase == 4 && css->category == CSS_EXTERNAL_STYLE)))
	{
	  /* filter enabled and disabled entries */
	  /* build the CSS list:
	     use the dialogue encoding for buf and UTF-8 for CSS path  */
	  if (css->category == CSS_DOCUMENT_STYLE)
	    ptr = TtaStrdup (TtaGetMessage (AMAYA, AM_LOCAL_CSS));
	  else
	    {
	      if (css->url == NULL)
		ptr = TtaConvertMbsToByte (css->localName,
					   TtaGetDefaultCharset ());
	      else
		ptr = TtaConvertMbsToByte (css->url,
					   TtaGetDefaultCharset ());
	    }
	  len = strlen (ptr);
	  len++;
	  if (size < len)
	    break;
	  strcpy (&buf[index], ptr);
	  TtaFreeMemory (ptr);
	  index += len;
	  nb++;
	  size -= len;
	  if (select == -1 &&
	      (CSScase < 4 || css->category == CSS_EXTERNAL_STYLE))
	    {
	      if (CSSpath)
		TtaFreeMemory (CSSpath);
	      if (css->category == CSS_DOCUMENT_STYLE)
		CSSpath = TtaStrdup (TtaGetMessage (AMAYA, AM_LOCAL_CSS));
	      else if (css->url)
		CSSpath = TtaStrdup (css->url);
	      else
		CSSpath = TtaStrdup (css->localName);
	      select = i;
	    }
	  i++;
	}
      css = css->NextCSS;
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
  InitCSSDialog (doc, TtaGetMessage (1, BOpenCSS));
}

/*----------------------------------------------------------------------
   DisableCSS list downloaded CSS files
  ----------------------------------------------------------------------*/
void                DisableCSS (Document doc, View view)
{
  CSScase = 2;
  InitCSSDialog (doc, TtaGetMessage (1, BDisableCSS));
}

/*----------------------------------------------------------------------
  EnableCSS list downloaded CSS files
  ----------------------------------------------------------------------*/
void                EnableCSS (Document doc, View view)
{
  CSScase = 3;
  InitCSSDialog (doc, TtaGetMessage (1, BEnableCSS));
}

/*----------------------------------------------------------------------
   RemoveCSS lists downloaded CSS files
  ----------------------------------------------------------------------*/
void                RemoveCSS (Document doc, View view)
{
  CSScase = 4;
  InitCSSDialog (doc, TtaGetMessage (1, BRemoveCSS));
}
