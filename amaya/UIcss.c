/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
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

#ifdef _WINDOWS
#include "wininclude.h"
#endif /* _WINDOWS */

#define CSSForm         0
#define CSSSelect       1
#define MAX_CSS_REF     2

static int         BaseCSS;
static int         CSScase;
static CHAR_T*     CSSpath;
static Document    CSSdocument;

#include "AHTURLTools_f.h"
#include "EDITORactions_f.h"
#include "HTMLedit_f.h"
#include "UIcss_f.h"
#include "css_f.h"
#include "init_f.h"
#include "query_f.h"
#include "styleparser_f.h"

/*----------------------------------------------------------------------
   LoadRemoteStyleSheet loads a remote style sheet into a file.
   Return FALSE if it's a local file and TRUE otherwise.
   When returning, the parameter completeURL contains the normalized url
   and the parameter localfile the path of the local copy of the file.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool        LoadRemoteStyleSheet (STRING url, Document doc, Element el, CSSInfoPtr css, STRING completeURL, STRING localfile)
#else
ThotBool        LoadRemoteStyleSheet (url, doc, el, css, completeURL, localfile)
STRING          url;
Document        doc;
Element         el;
CSSInfoPtr      css;
#endif
{
  CSSInfoPtr          oldcss;
  CHAR_T              tempname[MAX_LENGTH];
  STRING              tempdocument = NULL;
  int                 toparse;
  ThotBool            local = FALSE;
  ThotBool            import = (css != NULL);

  /* this document is displayed -> load the CSS */
  localfile[0] = EOS;
  if (import && css->url)
    NormalizeURL (url, 0, completeURL, tempname, css->url);
  else if (import && css->localName)
    NormalizeURL (url, 0, completeURL, tempname, css->localName);
  else
    NormalizeURL (url, doc, completeURL, tempname, NULL);
      
  if (IsW3Path (completeURL))
    {
      /* check against double inclusion */
      oldcss = SearchCSS (0, completeURL);
      if (oldcss != NULL)
	ustrcpy (localfile, oldcss->localName);
      else
	{
	  /* the document is not loaded yet */
	  /* changed this to doc */
	  toparse = GetObjectWWW (doc, completeURL, NULL, localfile, AMAYA_SYNC | AMAYA_LOAD_CSS, NULL, NULL, NULL, NULL, NO, NULL);
	  if (toparse || localfile[0] == EOS || !TtaFileExist (localfile))
	    TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_CANNOT_LOAD), completeURL);
	  else
	    {
	      /* store a copy of the remote CSS in .amaya/0 */
	      /* allocate and initialize tempdocument */
	      tempdocument = GetLocalPath (0, completeURL);
	      TtaFileUnlink (tempdocument);
	      /* now we can rename the local name of a remote document */
	      urename (localfile, tempdocument);
	      ustrcpy (localfile, tempdocument);
	      TtaFreeMemory (tempdocument);
	    }
	}
      local = TRUE;
    }
  else
    {
      /* store a copy of the local CSS in .amaya/0 */
      tempdocument = GetLocalPath (0, completeURL);
      local = FALSE;
      ustrcpy (localfile, tempdocument);
      TtaFileCopy (completeURL, localfile);
      TtaFreeMemory (tempdocument);
    }
  return (local);
}


/*----------------------------------------------------------------------
   LoadUserStyleSheet : Load the user Style Sheet found in it's    
   home directory or the default one in THOTDIR.           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                LoadUserStyleSheet (Document doc)
#else
void                LoadUserStyleSheet (doc)
Document            doc;
#endif
{
  CSSInfoPtr          css;
  struct stat         buf;
  FILE               *res;
  CHAR_T              *buffer, *ptr;
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
      css = AddCSS (0, doc, CSS_USER_STYLE, UserCSS, ptr);
      TtaFreeMemory (ptr);
      ptr = css->localName;
    }
  else if (!css->documents[doc])
    {
      /* we have to apply user preferences to this document */
      ptr = css->localName;
      css->documents[doc] = TRUE;
      css->enabled[doc] = TRUE;
    }

  if (css->enabled[doc] && ptr[0] != EOS  && TtaFileExist (ptr))
    {
      /* read User preferences */
      res = ufopen (ptr, TEXT("r"));
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
	      buffer = TtaAllocString (buf.st_size + 1000);
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
	  ReadCSSRules (doc, css, buffer, FALSE);
	  TtaFreeMemory (buffer);
	}
    }
}

/*----------------------------------------------------------------------
  AttrMediaChanged: the user has created removed or modified a Media
  attribute
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                AttrMediaChanged (NotifyAttribute * event)
#else
void                AttrMediaChanged (event)
NotifyAttribute    *event;
#endif
{
   ElementType         elType;
   Element             el;
   Document            doc;
   Attribute           attr;
   AttributeType       attrType;
   CSSInfoPtr          css;
   CSSmedia            media;
   DisplayMode         dispMode;
   CHAR_T              completeURL[MAX_LENGTH];
   CHAR_T              tempname[MAX_LENGTH];
   STRING              name2;
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
   name2 = TtaAllocString (length + 1);
   TtaGiveTextAttributeValue (attr, name2, &length);
   if (!ustrcasecmp (name2, TEXT ("screen")))
     media = CSS_SCREEN;
   else if (!ustrcasecmp (name2, TEXT ("print")))
     media = CSS_PRINT;
   else if (!ustrcasecmp (name2, TEXT ("all")))
     media = CSS_ALL;
   else
     media = CSS_OTHER;
   TtaFreeMemory (name2);
   /* get the CSS URI */
   attrType.AttrSSchema = elType.ElSSchema;
   attrType.AttrTypeNum = HTML_ATTR_HREF_;
   attr = TtaGetAttribute (el, attrType);
   if (attr != NULL)
     {
       length = TtaGetTextAttributeLength (attr);
       name2 = TtaAllocString (length + 1);
       TtaGiveTextAttributeValue (attr, name2, &length);
       /* load the stylesheet file found here ! */
       NormalizeURL (name2, doc, completeURL, tempname, NULL);
       TtaFreeMemory (name2);
       css = SearchCSS (0, completeURL);
       if (css->media[doc] != media)
	 {
	   /* something changed and we are not printing */
	   if ((media == CSS_ALL || media == CSS_SCREEN) &&
	       (css->media[doc] == CSS_PRINT || css->media[doc] == CSS_OTHER))
	     LoadStyleSheet (completeURL, doc, el, NULL, media);
	   else
	     {
	       if ((media == CSS_PRINT || media == CSS_OTHER) &&
	       (css->media[doc] == CSS_ALL || css->media[doc] == CSS_SCREEN))
		 RemoveStyleSheet (completeURL, doc, FALSE, FALSE);
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
  - after expternal style sheets
  - the the Document style sheet
  The returned string should be freed by the caller.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
STRING              CssToPrint (Document doc, STRING printdir)
#else
STRING              CssToPrint (doc, temdir)
Document            doc;
STRING              printdir;
#endif
{
  Element             el, head;
  ElementType         elType;
  FILE*               file;
  CSSInfoPtr          css;
  STRING              ptr, text;
  CHAR_T              tempfile[MAX_LENGTH];
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
	    if ( css->category == CSS_DOCUMENT_STYLE)
	      /* there is an internal style in the document */
	      length += ustrlen (printdir) + 5;
	    else
	      /* that external or user style sheet concerns the document */
	      length += ustrlen (css->localName) + 1;
	  css = css->NextCSS;
	}
      if (length)
	{
	  ptr = TtaAllocString (length + 1);
	  length = 0;

	  /* Add first the User style sheet */
	  css = CSSList;
	  while (css != NULL)
	    {
	      if (css->enabled[doc] &&
		  css->category == CSS_USER_STYLE)
		{
		  /* add that file name to the list */
		  ustrcpy (&ptr[length], css->localName);
		  length += ustrlen (css->localName);
		  ptr[length++] = SPACE;
		  css = NULL;
		}
	      else
		css = css->NextCSS;
	    }

	  /* Add after the list of external CSS files */
	  css = CSSList;
	  while (css != NULL)
	    {
	      if (css->enabled[doc] &&
		  css->category == CSS_EXTERNAL_STYLE)
		{
		  /* add that file name to the list */
		  ustrcpy (&ptr[length], css->localName);
		  length += ustrlen (css->localName);
		  ptr[length++] = SPACE;
		}
	      css = css->NextCSS;
	    }

	  /* Then look for style elements within the document */
	  el = TtaGetMainRoot (doc);
	  elType = TtaGetElementType (el);
	  elType.ElTypeNum = HTML_EL_HEAD;
	  head = TtaSearchTypedElement (elType, SearchForward, el);
	  elType.ElTypeNum = HTML_EL_STYLE_;
	  el = head;
	  while (el != NULL)
	    {
	      el = TtaSearchTypedElementInTree (elType, SearchForward, head, el);
	      if (el)
		{
		  if (!file)
		    {
		      /* build the temporary file name */
		      i = ustrlen (printdir);
		      ustrcpy (tempfile, printdir);
		      tempfile[i++] = DIR_SEP;
		      ustrcpy (&tempfile[i], TEXT("css"));
		      file = ufopen (tempfile, TEXT("w"));
		      if (file)
			{
			  /* add that file name to the list */
			  ustrcpy (&ptr[length], tempfile);
			  length += ustrlen (tempfile);
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
#ifdef __STDC__
static void         CallbackCSS (int ref, int typedata, STRING data)
#else
static void         CallbackCSS (ref, typedata, data)
int                 ref;
int                 typedata;
STRING              data;
#endif
{
  CSSInfoPtr          css;
  PInfoPtr            pInfo;
  Element             el;
  int                 val, length;
  ThotBool            found;

  val = (int) data;
  switch (ref - BaseCSS)
    {
    case CSSForm:
      TtaDestroyDialogue (ref);
      if (val == 1 && CSSpath != NULL)
	{
	  switch (CSScase)
	    {
	    case 1:
	      /* display the CSS file */
	      GetHTMLDocument (CSSpath, NULL, 0, 0, CE_ABSOLUTE, FALSE, NULL, NULL);
	      break;
	    case 2:
	      /* disable the CSS file, but not remove */
	      RemoveStyleSheet (CSSpath, CSSdocument, TRUE, FALSE);
      	      break;
	    case 3:
	      /* enable the CSS file */
	      css = SearchCSS (0, CSSpath);
	      css ->enabled[CSSdocument] = TRUE;
	      /* apply CSS rules */
	      if (!ustrcmp (CSSpath, UserCSS))
		LoadUserStyleSheet (CSSdocument);
	      else
		LoadStyleSheet (CSSpath, CSSdocument, NULL, NULL, css->media[CSSdocument]);
      	      break;
	    case 4:
	      /* remove the link to this file */
	      css = CSSList;
	      found = FALSE;
	      while (css != NULL && !found)
		{
		  if (css->category != CSS_DOCUMENT_STYLE &&
		      css->documents[CSSdocument] &&
		      ((css->url && !ustrcmp (CSSpath, css->url)) ||
		       (css->localName && !ustrcmp (CSSpath, css->localName))))
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
			      TtaDeleteTree (el, CSSdocument);
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
      if (CSSpath != NULL)
	TtaFreeMemory (CSSpath);
      length = ustrlen (data);
      CSSpath = TtaAllocString (length + 1);
      ustrcpy (CSSpath, data);      
      break;
    default:
      break;
    }
}

/*----------------------------------------------------------------------
   InitCSS                                                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                InitCSS (void)
#else
void                InitCSS ()
#endif
{
   /* initialize the dialogs */
   BaseCSS = TtaSetCallback (CallbackCSS, MAX_CSS_REF);
   CSSpath = NULL;
}

/*----------------------------------------------------------------------
   InitCSSDialog list downloaded CSS files
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         InitCSSDialog (Document doc, STRING s)
#else
static void         InitCSSDialog (doc, s)
Document            doc;
STRING              s;
#endif
{
  CSSInfoPtr          css;
  CHAR_T              buf[400];
  STRING              ptr;
  int                 i, select;
  int                 len, nb;
  int                 index, size;

  CSSdocument = doc;
  css = CSSList;
  buf[0] = 0;
  index = 0;
  nb = 0;
  size = 400;
#  ifndef _WINDOWS
  /* create the form */
  TtaNewSheet (BaseCSS + CSSForm, TtaGetViewFrame (doc, 1),
	       s, 1,
	       TtaGetMessage(LIB, TMSG_LIB_CONFIRM), TRUE, 1, 'L', D_DONE);
#  endif /* !_WINDOWS */
  select = -1;
  i = 0;
  while (css != NULL)
    {
      if (css->category != CSS_DOCUMENT_STYLE &&
	  css->documents[doc] &&
	  /* it's impossible to remove the User style sheet */
	  (CSScase < 4 || css->category == CSS_EXTERNAL_STYLE))
	{
	  if ((CSScase == 3 && !css->enabled[doc]) ||
	      (CSScase == 2 && css->enabled[doc]) ||
	      CSScase == 1 || CSScase == 4)
	    {
	      /* filter enabled and disabled entries */
	      /* build the CSS list */
	      if (css->url == NULL)
		ptr = css->localName;
	      else
		ptr = css->url;

	      len = ustrlen (ptr);
	      len++;
	      if (size < len)
		break;
	      ustrcpy (&buf[index], ptr);
	      index += len;
	      nb++;
	      size -= len;
	      if (select == -1 &&
		  (CSScase < 4 || css->category == CSS_EXTERNAL_STYLE))
		{
		  if (CSSpath != NULL)
		    TtaFreeMemory (CSSpath);
		  if (css->url)
		    {
		      len = ustrlen (css->url);
		      CSSpath = TtaAllocString (len + 1);
		      ustrcpy (CSSpath, css->url);
		    }
		  else
		    {
		      len = ustrlen (css->localName);
		      CSSpath = TtaAllocString (len + 1);
		      ustrcpy (CSSpath, css->localName);
		    }
		  select = i;
		}
	      i++;
	    }
	}
      css = css->NextCSS;
    }

  /* display the form */
#  ifdef _WINDOWS
  CreateCSSDlgWindow (TtaGetViewFrame (doc, 1), BaseCSS, CSSSelect, CSSForm, TtaGetMessage (AMAYA, AM_CSS_FILE), nb, buf, s);
#  else  /* !_WINDOWS */
  if (nb > 0)
    TtaNewSelector (BaseCSS + CSSSelect, BaseCSS + CSSForm,
		    TtaGetMessage (AMAYA, AM_CSS_FILE),
		    nb, buf, 5, NULL, FALSE, TRUE);
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
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                LinkCSS (Document doc, View view)
#else
void                LinkCSS (doc, view)
Document            doc;
View                view;
#endif
{
  /* add a new link to a CSS file */
  LinkAsCSS = TRUE;
  CreateLinkInHead (doc, 1);
  /* LinkAsCSS will be cleared by SetREFattribute or by CallbackDialogue */
}

/*----------------------------------------------------------------------
   OpenCSS lists downloaded CSS files
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                OpenCSS (Document doc, View view)
#else
void                OpenCSS (doc, view)
Document            doc;
View                view;
#endif
{
  CSScase = 1;
  InitCSSDialog (doc, TtaGetMessage (1, BOpenCSS));
}

/*----------------------------------------------------------------------
   DisableCSS list downloaded CSS files
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DisableCSS (Document doc, View view)
#else
void                DisableCSS (doc, view)
Document            doc;
View                view;
#endif
{
  CSScase = 2;
  InitCSSDialog (doc, TtaGetMessage (1, BDisableCSS));
}

/*----------------------------------------------------------------------
  EnableCSS list downloaded CSS files
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                EnableCSS (Document doc, View view)
#else
void                EnableCSS (doc, view)
Document            doc;
View                view;
#endif
{
  CSScase = 3;
  InitCSSDialog (doc, TtaGetMessage (1, BEnableCSS));
}

/*----------------------------------------------------------------------
   RemoveCSS lists downloaded CSS files
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                RemoveCSS (Document doc, View view)
#else
void                RemoveCSS (doc, view)
Document            doc;
View                view;
#endif
{
  CSScase = 4;
  InitCSSDialog (doc, TtaGetMessage (1, BRemoveCSS));
}
