/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * CSS.c : Handle all the dialogs and interface functions needed 
 * to manipulate CSS.
 *
 * Author: I. Vatton
 *
 */


#define THOT_EXPORT extern
#include "amaya.h"
#undef THOT_EXPORT
#define THOT_EXPORT
#include "css.h"

#include "css_f.h"
#include "init_f.h"
#if !defined(AMAYA_JAVA) && !defined(AMAYA_ILU)
#include "query_f.h"
#endif
#include "AHTURLTools_f.h"
#include "UIcss_f.h"
#include "styleparser_f.h"


/*----------------------------------------------------------------------
  GetPExtension returns the Presentation Extension Schema associated with
  the document doc and the structure sSchema
  At the same time, this funciton updates the css context.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PSchema         GetPExtension (Document doc, SSchema sSchema, CSSInfoPtr css)
#else
PSchema         GetPExtension (doc, sSchema, css)
Document        doc;
SSchema         sSchema;
CSSInfoPtr      css;
#endif
{
  PInfoPtr            pInfo;
  PSchema             pSchema, nSchema, prevS;
  boolean             found;

  if (sSchema == NULL)
    sSchema = TtaGetDocumentSSchema (doc);
  pInfo = css->infos;
  found = FALSE;
  while (pInfo != NULL && !found)
    {
      if (sSchema == pInfo->PiSSchema && pInfo->PiPSchema != NULL)
	/* the pschema is already known */
	return (pInfo->PiPSchema);
      else if (pInfo->PiDoc == doc)
	found = TRUE;
      else
	/* next info context */
	pInfo = pInfo->PiNext;
    }

  if (pInfo == NULL)
    {
      /* add the presentation info block */
      pInfo = (PInfoPtr) TtaGetMemory (sizeof (PInfo));
      pInfo->PiNext = css->infos;
      css->infos = pInfo;
      pInfo->PiLink = NULL;
      pInfo->PiDoc = doc;
      pInfo->PiPSchema = NULL;
    }

  /* create the presentation schema for this structure */
  nSchema = TtaNewPSchema ();
  pSchema = TtaGetFirstPSchema (doc, sSchema);
  prevS = NULL;
  while (pSchema != NULL)
    {
      prevS = pSchema;
      TtaNextPSchema (&pSchema, doc, NULL);
    }
  TtaAddPSchema (nSchema, prevS, TRUE, doc, sSchema);

  pInfo->PiSSchema = sSchema;
  pInfo->PiPSchema = nSchema;
  return (nSchema);
}

/*----------------------------------------------------------------------
   AddCSS adds a new CSS context in the list.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
CSSInfoPtr      AddCSS (Document doc, Document docRef, CSSCategory category, STRING url, STRING localName)
#else
CSSInfoPtr      AddCSS (doc, docRef, category, url, localName)
Document        doc;
Document        docRef;
CSSCategory     category;
STRING          url;
STRING          localName;
#endif
{
  CSSInfoPtr          css, prev;
  int                 i;

  css = TtaGetMemory (sizeof (CSSInfo));
  if (css != NULL)
    {
      css->doc = doc;
      css->localName = TtaStrdup (localName);
      css->url = TtaStrdup (url);
      css->category = category;
      for (i = 0; i < DocumentTableLength; i++)
	css->documents[i] = FALSE;

      /* store information about this docRef */
      css->documents[docRef] = TRUE;
      css->infos = NULL;
      css->NextCSS = NULL;

      /* chain to the CSS list */
      if (CSSList == NULL)
	CSSList = css;
      else
	{
	  prev = CSSList;
	  while (prev->NextCSS != NULL)
	    prev = prev->NextCSS;
	  prev->NextCSS = css;
	}
    }
  return css;
}

/*----------------------------------------------------------------------
   SearchCSS searchs the css corresponding to the specific url (doc == 0)
   or the CSS_DOCUMENT_STYLE css of the document.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
CSSInfoPtr          SearchCSS (Document doc, STRING url)
#else
CSSInfoPtr          SearchCSS (doc, url)
Document            doc;
STRING               url;
#endif
{
  CSSInfoPtr          css = CSSList;
 
  while (css != NULL)
    {
      if (url && ((css->url && !ustrcmp (url, css->url)) ||
		  (css->localName && !ustrcmp (url, css->localName))))
	/* an external CSS */
	return css;
      else if (doc != 0 && css->doc == doc)
	/* a document CSS */
	return css;
      else
	css = css->NextCSS;
    }
  return css;
}


/*----------------------------------------------------------------------
   UnlinkCSS the CSS is no longer applied to this document and iff the
   parameter removed is TRUE, the link is cut.
   If this CSS is no longer used the context and attached information
   are freed.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void     UnlinkCSS (CSSInfoPtr css, Document doc, boolean removed)
#else
static void     UnlinkCSS (css, doc, removed)
CSSInfoPtr      css;
Document        doc;
boolean         removed;
#endif
{
  CSSInfoPtr          prev;
  PInfoPtr            pInfo, prevInfo;
  int                 i;
  boolean             used;

  if (css == NULL)
    return;
  else
    {
      /* look at if this css is alway used */
      used = (css->doc != 0);
      i = 0;
      while (!used && i < DocumentTableLength)
	{
	  used = css->documents[i];
	  i++;
	}

      /* look for the specific P descriptors in the css */
      pInfo = css->infos;
      prevInfo = NULL;
      while (pInfo != NULL && pInfo->PiDoc != doc)
	{
	  prevInfo = pInfo;
	  pInfo = pInfo->PiNext;
	}
      if (pInfo != NULL)
	{
	  if (removed)
	    {
	      /* unlink the document context from the list */
	      if (prevInfo == NULL)
		css->infos = pInfo->PiNext;
	      else
		prevInfo->PiNext = pInfo->PiNext;
	      used = FALSE;
	    }
	  /* disapply the CSS */
	  if (pInfo->PiPSchema)
	    {
	      TtaUnlinkPSchema (pInfo->PiPSchema, pInfo->PiDoc, pInfo->PiSSchema);
	      TtaCleanStylePresentation (NULL, pInfo->PiPSchema, pInfo->PiDoc);
	      /* remove presentation schemas */
	      TtaRemovePSchema (pInfo->PiPSchema, pInfo->PiDoc, pInfo->PiSSchema);
	      pInfo->PiPSchema = NULL;
	    }
	  /* free the document context */
	  if (removed)
	    TtaFreeMemory (pInfo);
	}

      if (!used)
	{
	  if (css->category == CSS_EXTERNAL_STYLE && IsW3Path (css->url))
	    /* remove the file */
	    TtaFileUnlink (css->localName);
	  TtaFreeMemory (css->localName);
	  TtaFreeMemory (css->url);
	  if (CSSList == css)
	    CSSList = css->NextCSS;
	  else
	    {
	      prev = CSSList;
	      while (prev != NULL && prev->NextCSS != css)
		prev = prev->NextCSS;
	      if (prev != NULL)
		prev->NextCSS = css->NextCSS;
	    }
	  TtaFreeMemory (css);
	}
    }
}

/*----------------------------------------------------------------------
   RemoveDocCSSs removes all CSS information linked with the document.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                RemoveDocCSSs (Document doc)
#else
void                RemoveDocCSSs (doc)
Document            doc;
#endif
{
  CSSInfoPtr          css, next;

  css = CSSList;
  while (css != NULL)
    {
      next = css->NextCSS;
      if (css->doc == doc)
	{
	  /* the document displays the CSS file itself */
	  css->doc = 0;
	  UnlinkCSS (css, doc, TRUE);
	}
      else if (css->documents[doc])
	{
	  css->documents[doc] = FALSE;
	  UnlinkCSS (css, doc, TRUE);
	}
      /* look at the next CSS context */
      css = next;
    }
}

/*----------------------------------------------------------------------
   RemoveStyleSheet removes a style sheet.
   It could be an external CSS file linked with the document (url not NULL)
   or the document Style element.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void            RemoveStyleSheet (STRING url, Document doc, boolean removed)
#else
void            RemoveStyleSheet (url, doc, removed)
STRING          url;
Document        doc;
boolean         removed;
#endif
{
  CSSInfoPtr          css;
  boolean             found;

  css = CSSList;
  found = FALSE;
  while (css != NULL && !found)
    {
      if (url && ((css->url && !ustrcmp (url, css->url)) ||
		  (css->localName && !ustrcmp (url, css->localName))))
	/* an external CSS */
	found = TRUE;
      else if (!url && css->category == CSS_DOCUMENT_STYLE && css->documents[doc])
	found = TRUE;
      else
	css = css->NextCSS;
    }

  if (css != NULL)
    {
      if (removed)
	css->documents[doc] = FALSE;
      UnlinkCSS (css, doc, removed);
    }
}

/*----------------------------------------------------------------------
   LoadStyleSheet loads the external Style Sheet found at the given
   url.
   The parameter el gives the element which links the CSS or NULL.
   The parameter CSS gives the CSS context which imports this CSS file.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                LoadStyleSheet (STRING url, Document doc, Element el, CSSInfoPtr css)
#else
void                LoadStyleSheet (url, doc, el, css)
STRING              url;
Document            doc;
Element             el;
CSSInfoPtr          css;
#endif
{
  CSSInfoPtr          oldcss;
  PInfoPtr            pInfo;
  struct stat         buf;
  FILE               *res;
  CHAR_T                tempfile[MAX_LENGTH];
  CHAR_T                tempname[MAX_LENGTH];
  CHAR_T                tempURL[MAX_LENGTH];
  STRING               tempdocument;
  STRING               buffer = NULL;
  int                 len;
  int                 local = FALSE;
  int                 toparse;

  if (TtaGetViewFrame (doc, 1) != 0)
    {
      /* this document is displayed -> load the CSS */
      tempfile[0] = EOS;
      NormalizeURL (url, doc, tempURL, tempname, NULL);
      
      if (IsW3Path (tempURL))
	{
	  /* check against double inclusion */
	  oldcss = SearchCSS (0, tempURL);
	  if (oldcss != NULL)
	    ustrcpy (tempfile, oldcss->localName);
	  else
	    {
	      /* the document is not loaded yet */
	      /* changed this to doc */
#ifndef AMAYA_JAVA
	      toparse = GetObjectWWW (doc, tempURL, NULL, tempfile, AMAYA_SYNC | AMAYA_LOAD_CSS, NULL, NULL, NULL, NULL, NO, NULL);
#else
	toparse = GetObjectWWW (doc, tempURL, NULL, tempfile, AMAYA_SYNC, NULL, NULL, NULL, NULL, NO, NULL);
#endif /* ! AMAYA_JAVA */
	      if (toparse || tempfile[0] == EOS || !TtaFileExist (tempfile))
		{
		  TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_CANNOT_LOAD), tempURL);
		  return;
		}
	      else
		{
		  /* we have to rename the temporary file */
		  /* allocate and initialize tempdocument */
		  tempdocument = GetLocalPath (0, tempURL);
		  TtaFileUnlink (tempdocument);
		  /* now we can rename the local name of a remote document */
		  rename (tempfile, tempdocument);
		  ustrcpy (tempfile, tempdocument);
		  TtaFreeMemory (tempdocument);
		}
	    }
	}
      else
	{
	  oldcss = SearchCSS (0, tempURL);
	  local = TRUE;
	  ustrcpy (tempfile, tempURL);
	}

      if (oldcss == NULL || oldcss->category != CSS_EXTERNAL_STYLE)
	{
	  /* It's a new CSS file: allocate a new Presentation structure */
	  css = AddCSS (0, doc, CSS_EXTERNAL_STYLE, tempURL, tempfile);
	  oldcss = css;
	}
      else
	oldcss->documents[doc] = TRUE;

      if (tempfile[0] == EOS)
	return;
      /* store the element which links the CSS */
      pInfo = oldcss->infos;
      while (pInfo != NULL && pInfo->PiDoc != doc)
	/* next info context */
	pInfo = pInfo->PiNext;
      if (pInfo == NULL)
	{
	  /* add the presentation info block */
	  pInfo = (PInfoPtr) TtaGetMemory (sizeof (PInfo));
	  pInfo->PiNext = oldcss->infos;
	  pInfo->PiDoc = doc;
	  pInfo->PiSSchema = NULL;
	  pInfo->PiPSchema = NULL;
	  pInfo->PiLink = el;
	  oldcss->infos = pInfo;
	}


      /* apply CSS rules in current Presentation structure (import) */
      if ( pInfo->PiPSchema == NULL)
	{
	  /* load the resulting file in memory */
	  res = fopen (tempfile, "r");
	  if (res == NULL)
	    {
	      TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_CANNOT_LOAD), tempURL);
	      if (!local)
		TtaFileUnlink (tempfile);
	      return;
	    }

#ifdef _WINDOWS
	  if (fstat (_fileno (res), &buf))
#else  /* !_WINDOWS */
	    if (fstat (fileno (res), &buf))
#endif /* _WINDOWS */
	      {
		TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_CANNOT_LOAD), tempURL);
		fclose (res);
		if (!local)
		  TtaFileUnlink (tempfile);
		return;
	      }

	  buffer = (STRING) TtaGetMemory (buf.st_size + 1000);
	  if (buffer == NULL)
	    {
	      TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_CANNOT_LOAD), tempURL);
	      fclose (res);
	      if (!local)
		TtaFileUnlink (tempfile);
	      return;
	    }
	  len = fread (buffer, buf.st_size, 1, res);
	  if (len != 1)
	    {
	      TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_CANNOT_LOAD), tempURL);
	      fclose (res);
	      if (!local)
		TtaFileUnlink (tempfile);
	      TtaFreeMemory (buffer);
	      return;
	    }
	  buffer[buf.st_size] = 0;
	  fclose (res);

	  ReadCSSRules (0, doc, oldcss, buffer, FALSE);
	  TtaFreeMemory (buffer);
	}
    }
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
  STRING              buffer, ptr;
  int                 len;

  /* look for the User preferences */
  if (!UserCSS)
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
      /* allocate a new Presentation structure */ 
      if (TtaFileExist (UserCSS))
	css = AddCSS (0, doc, CSS_USER_STYLE, NULL, UserCSS);
    }
  else if (!css->documents[doc])
    {
      /* we have to apply user preferences to this document */
      ptr = css->localName;
      css->documents[doc] = TRUE;
    }

  if (ptr[0] != EOS  && TtaFileExist (ptr))
    {
      /* read User preferences */
      res = fopen (ptr, "r");
      if (res != NULL)
	{
#     ifdef _WINDOWS
	  if (fstat (_fileno (res), &buf))
#     else  /* !_WINDOWS */
	  if (fstat (fileno (res), &buf))
#     endif /* !_WINDOWS */
	    fclose (res);
	  else
	    {
	      buffer = (STRING) TtaGetMemory (buf.st_size + 1000);
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
	  ReadCSSRules (0, doc, css, buffer, FALSE);
	  TtaFreeMemory (buffer);
	}
    }
}
