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

#ifdef _WINDOWS
extern STRING WIN_Home;
#endif /* _WINDOWS */


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
      if (sSchema == pInfo->PiSSchema)
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
   RemoveCSS removes a CSS context and frees all attached information.
   The parameter removeFile is TRUE when the local copy of the CSS file
   should be removed.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void     RemoveCSS (CSSInfoPtr css, boolean removeFile)
#else
static void     RemoveCSS (css, removeFile)
CSSInfoPtr      css;
boolean         removeFile;
#endif
{
  CSSInfoPtr          prev;
  PInfoPtr            pInfo, nextInfo;

  if (css == NULL)
    return;
  else
    {
      if (removeFile && css->category == CSS_EXTERNAL_STYLE && IsW3Path (css->url))
	/* remove the file */
	TtaFileUnlink (css->localName);
      TtaFreeMemory (css->localName);
      TtaFreeMemory (css->url);
      /* remove presentation schemas and P descriptors in the css */
      pInfo = css->infos;
      while (pInfo != NULL)
	{
	  /* remove presentation schemas */
	  nextInfo = pInfo->PiNext;
	  TtaRemovePSchema (pInfo->PiPSchema, pInfo->PiDoc, pInfo->PiSSchema);
	  /* remove P descriptors in the css structure */
	  TtaFreeMemory (pInfo);
	  pInfo = nextInfo;
	}
      css->infos = NULL;

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

/*----------------------------------------------------------------------
   RemoveDocCSSs removes all CSS information linked with the document.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                RemoveDocCSSs (Document doc, boolean removeFile)
#else
void                RemoveDocCSSs (doc, removeFile)
Document            doc;
boolean             removeFile;
#endif
{
  CSSInfoPtr          css, next;
  PInfoPtr            pInfo, prevInfo;
  int                 i;
  boolean             used;

  css = CSSList;
  while (css != NULL)
    {
      if (css->doc == doc)
	{
	  next = css->NextCSS;
	  RemoveCSS (css, removeFile);
	  css = next;
	}	
      else if (css->documents[doc])
	{
	  css->documents[doc] = FALSE;
	  /* look at if this css is alway used */
	  used = FALSE;
	  i = 0;
	  while (!used && i < DocumentTableLength)
	    {
	      used = css->documents[i];
	      i++;
	    }
	  if (!used)
	    {
	      /* remove this css file */
	      next = css->NextCSS;
	      RemoveCSS (css, removeFile);
	      css = next;
	    }
	  else
	    {
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
		  /* update the the list of  P descriptors in the css */
		  if (prevInfo == NULL)
		    css->infos = pInfo->PiNext;
		  else
		    prevInfo->PiNext = pInfo->PiNext;
		  /* remove presentation schemas */
		  TtaRemovePSchema (pInfo->PiPSchema, pInfo->PiDoc, pInfo->PiSSchema);
		  /* remove P descriptors in the css structure */
		  TtaFreeMemory (pInfo);
		}
	    }
	}	
      else
	css = css->NextCSS;
    }
}

/*----------------------------------------------------------------------
   RemoveStyleSheet removes a style sheet.
   It could be an external CSS file linked with the document (url not NULL)
   or the document Style element.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                RemoveStyleSheet (STRING url, Document doc)
#else
void                RemoveStyleSheet (url, doc)
STRING              url;
Document            doc;
#endif
{
  CSSInfoPtr          css, prev;
  PInfoPtr            pInfo, prevInfo;
  int                 i;
  boolean             used;

  css = CSSList;
  prev = NULL;
  used = FALSE;
  while (css != NULL && !used)
    {
      if (url && ((css->url && !ustrcmp (url, css->url)) ||
		  (css->localName && !ustrcmp (url, css->localName))))
	/* an external CSS */
	used = TRUE;
      else if (!url && css->doc == doc)
	/* a document CSS */
	used = TRUE;
      else
	{
	  prev = css;
	  css = css->NextCSS;
	}
    }

  if (css != NULL)
    {
      css->documents[doc] = FALSE;
      /* look at if this css is alway used */
      used = FALSE;
      if (css->category != CSS_DOCUMENT_STYLE)
	{
	  i = 0;
	  while (!used && i < DocumentTableLength)
	    {
	      used = css->documents[i];
	      i++;
	    }
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
	  /* update the the list of  P descriptors in the css */
	  if (prevInfo == NULL)
	    css->infos = pInfo->PiNext;
	  else
	    prevInfo->PiNext = pInfo->PiNext;
	  TtaCleanStylePresentation (NULL, pInfo->PiPSchema, pInfo->PiDoc);
	  /* remove presentation schemas */
	  TtaRemovePSchema (pInfo->PiPSchema, pInfo->PiDoc, pInfo->PiSSchema);
	  /* remove P descriptors in the css structure */
	  TtaFreeMemory (pInfo);
	}

      if (!used)
	{
	  /* remove this css file */
	  if (prev)
	    prev->NextCSS = css->NextCSS;
	  else
	    CSSList = css->NextCSS;
	  RemoveCSS (css, TRUE);
	}
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
  CHAR                tempfile[MAX_LENGTH];
  CHAR                tempname[MAX_LENGTH];
  CHAR                tempURL[MAX_LENGTH];
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
	    {
	      if (!oldcss->documents[doc])
		{
		  oldcss->documents[doc] = TRUE;
		  ustrcpy (tempfile, oldcss->localName);
		}
	    }
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
      if (tempfile[0] == EOS)
	return;

      /* load the resulting file in memory */
      res = fopen (tempfile, "r");
      if (res == NULL)
	{
	  TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_CANNOT_LOAD), tempURL);
	  if (!local)
	    TtaFileUnlink (tempfile);
	  return;
	}
#     ifdef _WINDOWS
      if (fstat (_fileno (res), &buf))
#     else  /* !_WINDOWS */
      if (fstat (fileno (res), &buf))
#     endif /* _WINDOWS */
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

      if (oldcss == NULL)
	{
	  /* It's a new CSS file: allocate a new Presentation structure */
	  css = AddCSS (0, doc, CSS_EXTERNAL_STYLE, tempURL, tempfile);
	  oldcss = css;
	}

      if (css != NULL)
	/* apply CSS rules in current Presentation structure (import) */
	ReadCSSRules (0, doc, css, buffer, FALSE);
      else if (!oldcss->documents[doc])
	{
	  /* apply CSS rules */
	  oldcss->documents[doc] = TRUE;
	  ReadCSSRules (oldcss->doc, doc, oldcss, buffer, FALSE);
	}
      /* store the element which links the CSS */
      pInfo = css->infos;
      while (pInfo != NULL && pInfo->PiDoc != doc)
	/* next info context */
	pInfo = pInfo->PiNext;
      if (pInfo == NULL)
	{
	  /* add the presentation info block */
	  pInfo = (PInfoPtr) TtaGetMemory (sizeof (PInfo));
	  pInfo->PiNext = css->infos;
	  css->infos = pInfo;
	  pInfo->PiDoc = doc;
	  pInfo->PiSSchema = NULL;
	  pInfo->PiPSchema = NULL;
	  css->infos = pInfo;
	}
      pInfo->PiLink = el;

      TtaFreeMemory (buffer);
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
  CHAR                tempfile[MAX_LENGTH];
  STRING               buffer, ptr;
  STRING               home;
  STRING               thotdir;
  int                 len;

  /* look for the User preferences */
  buffer = NULL;
  ptr = tempfile;
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
      /* Load User preferences */
#     ifdef _WINDOWS
      home = (STRING) TtaGetMemory (ustrlen (WIN_Home) + 1);
      ustrcpy (home, WIN_Home);
#     else  /* !_WINDOWS */
      home = TtaGetEnvString ("HOME");
#     endif /* _WINDOWS */

      tempfile[0] = EOS;
      /* try to load the user preferences */
      if (home)
#   ifdef _WINDOWS
	sprintf (tempfile, "%s%s%s.css", home, DIR_STR, HTAppName);
#   else  /* !_WINDOWS */
	sprintf (tempfile, "%s%s.%s.css", home, DIR_STR, HTAppName);
#   endif /* _WINDOWS */

      if (tempfile[0] == EOS || !TtaFileExist (tempfile))
	{
	  thotdir = TtaGetEnvString ("THOTDIR");
	  if (thotdir)
	    {
	      /* file not found */
	      sprintf (tempfile, "%s%samaya%s%s.css", thotdir, DIR_STR, DIR_STR, HTAppName);
	    }
	}

      /* allocate a new Presentation structure */ 
      if (tempfile[0] != EOS  && TtaFileExist (tempfile))
	css = AddCSS (0, doc, CSS_USER_STYLE, NULL, tempfile);
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
