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
static STRING      CSSpath;
static Document    CSSdocument;

#include "AHTURLTools_f.h"
#include "EDITORactions_f.h"
#include "HTMLedit_f.h"
#include "UIcss_f.h"
#include "css_f.h"
#include "init_f.h"

/*----------------------------------------------------------------------
   LoadRemoteStyleSheet loads a remote style sheet into a file.
   Return FALSE if it's a local file and TRUE otherwise.
   When the returned value is TRUE, the parameter completeURL contains
   the normalized url and the parameter localfile the path of the local
   copy of the file.
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
  STRING              tempdocument;
  int                 toparse;
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
#ifndef AMAYA_JAVA
	      toparse = GetObjectWWW (doc, completeURL, NULL, localfile, AMAYA_SYNC | AMAYA_LOAD_CSS, NULL, NULL, NULL, NULL, NO, NULL);
#else
	toparse = GetObjectWWW (doc, completeURL, NULL, localfile, AMAYA_SYNC, NULL, NULL, NULL, NULL, NO, NULL);
#endif /* ! AMAYA_JAVA */
	      if (toparse || localfile[0] == EOS || !TtaFileExist (localfile))
		{
		  TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_CANNOT_LOAD), completeURL);
		  return;
		}
	      else
		{
		  /* we have to rename the temporary file */
		  /* allocate and initialize tempdocument */
		  tempdocument = GetLocalPath (0, completeURL);
		  TtaFileUnlink (tempdocument);
		  /* now we can rename the local name of a remote document */
		  urename (localfile, tempdocument);
		  ustrcpy (localfile, tempdocument);
		  TtaFreeMemory (tempdocument);
		}
	    }
	  return TRUE;
	}
      else
	return FALSE;
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
	      RemoveStyleSheet (CSSpath, CSSdocument, FALSE);
      	      break;
	    case 3:
	      /* disable the CSS file in case it was applied yet */
	      RemoveStyleSheet (CSSpath, CSSdocument, FALSE);
	      /* enable the CSS file */
	      LoadStyleSheet (CSSpath, CSSdocument, NULL, NULL);
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
	  (CSScase < 4 || css->category == CSS_EXTERNAL_STYLE))
	{
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
  CreateLinkInHead (doc, 1);
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
