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
#include "p2css_f.h"
#if !defined(AMAYA_JAVA) && !defined(AMAYA_ILU)
#include "query_f.h"
#endif
#include "AHTURLTools_f.h"
#include "HTMLstyle_f.h"
#include "UIcss_f.h"


/*----------------------------------------------------------------------
   AddCSS                                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
CSSInfoPtr      AddCSS (Document doc, Document docRef, CSSCategory category, char *url, char *localName)
#else
CSSInfoPtr      AddCSS (doc, docRef, category, url, localName)
Document        doc;
Document        docRef;
CSSCategory     category;
char           *url;
char           *localName;
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
      css->pschemas = NULL;
      for (i = 0; i < DocumentTableLength; i++)
	css->documents[i] = FALSE;
      css->documents[docRef] = TRUE;
      css->css_rule = NULL;
      css->NextCSS = NULL;
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
CSSInfoPtr          SearchCSS (Document doc, char *url)
#else
CSSInfoPtr          SearchCSS (doc, url)
Document            doc;
char               *url;
#endif
{
  CSSInfoPtr          css = CSSList;
 
  while (css != NULL)
    {
      if (doc != 0 && css->doc == doc)
	return css;
      else if (doc == 0 && url != NULL &&
	       css->url != NULL && !strcmp(css->url, url))
	return css;
      else
	css = css->NextCSS;
    }
  return css;
}


/*----------------------------------------------------------------------
   RemoveCSS                                 
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
      /* free presentation schemas */
      pInfo = css->pschemas;
      while (pInfo != NULL)
	{
	  nextInfo = pInfo->PiNext;
	  TtaRemovePSchema (pInfo->PiPSchema, pInfo->PiDoc, pInfo->PiSSchema);
	  TtaFreeMemory (pInfo);
	  pInfo = nextInfo;
	}
      css->pschemas = NULL;

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
   RemoveDocCSSs                                
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
	      /* remove this css document */
	      next = css->NextCSS;
	      RemoveCSS (css, removeFile);
	      css = next;
	    }
	}	
      else
	css = css->NextCSS;
    }
}


/*----------------------------------------------------------------------
  GetPExtension returns the Presentation Extension Schema associated with
  the document doc and the structure sSchema
  At the same time, this funciton updates the css context.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PSchema         GetPExtension (Document doc, SSchema sSchema, CSSInfoPtr css)
#else
PSchema         GetPExtension (doc, sSchema)
Document        doc;
SSchema         sSchema;
CSSInfoPtr      css;
#endif
{
  PInfoPtr            pInfo, prevInfo;
  PSchema             pSchema, nSchema, prevS;

  if (sSchema == NULL)
    sSchema = TtaGetDocumentSSchema (doc);
  pInfo = css->pschemas;
  prevInfo = NULL;
  while (pInfo != NULL)
    {
      if (sSchema == pInfo->PiSSchema)
	return (pInfo->PiPSchema);
      else
	{
	  /* next pschema info */
	  prevInfo = pInfo;
	  pInfo = pInfo->PiNext;
	}
    }

  if (pInfo == NULL)
    {
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
      /* add the presentation info block */
      pInfo = (PInfoPtr) TtaGetMemory (sizeof (PInfo));
      if (prevInfo == NULL)
	css->pschemas = pInfo;
      else
	prevInfo->PiNext = pInfo;
      pInfo->PiSSchema = sSchema;
      pInfo->PiDoc = doc;
      pInfo->PiPSchema = nSchema;
      pInfo->PiNext = NULL;
      return (nSchema);
    }
  return (NULL);
}

/*----------------------------------------------------------------------
   LoadHTMLStyleSheet : Load an external Style Sheet found at the
   URL given in argument.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                LoadHTMLStyleSheet (char *URL, Document doc, CSSInfoPtr css)
#else
void                LoadHTMLStyleSheet (URL, doc, css)
char               *URL;
Document            doc;
CSSInfoPtr          css;
#endif
{
  CSSInfoPtr          oldcss;
  struct stat         buf;
  FILE               *res;
  char                tempfile[MAX_LENGTH];
  char                tempname[MAX_LENGTH];
  char                tempURL[MAX_LENGTH];
  char               *tempdocument;
  char               *buffer = NULL;
  int                 len;
  int                 local = FALSE;
  int                 toparse;

  if (TtaGetViewFrame (doc, 1) != 0)
    {
      /* this document is displayed -> load the CSS */
      tempfile[0] = EOS;
      NormalizeURL (URL, doc, tempURL, tempname, NULL);
      
      if (IsW3Path (tempURL))
	{
	  /* check against double inclusion */
	  oldcss = SearchCSS (0, tempURL);
	  if (oldcss == NULL)
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
		  strcpy (tempfile, tempdocument);
		  TtaFreeMemory (tempdocument);
		}
	    }
	}
      else
	{
	  oldcss = SearchCSS (0, tempURL);
	  local = TRUE;
	  strcpy (tempfile, tempURL);
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
      if (fstat (fileno (res), &buf))
	{
	  TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_CANNOT_LOAD), tempURL);
	  fclose (res);
	  if (!local)
	    TtaFileUnlink (tempfile);
	  return;
	}
      buffer = (char *) TtaGetMemory (buf.st_size + 1000);
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
	/* allocate a new Presentation structure */
	css = AddCSS (0, doc, CSS_EXTERNAL_STYLE, tempURL, tempfile);

      if (css != NULL)
	/* apply CSS rules in current Presentation structure (import) */
	ReadCSSRules (0, doc, css, buffer);
      else if (!oldcss->documents[doc])
	{
	  /* apply CSS rules */
	  oldcss->documents[doc] = TRUE;
	  ReadCSSRules (oldcss->doc, doc, oldcss, buffer);
	}
	
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
  char                tempfile[MAX_LENGTH];
  char               *buffer, *ptr;
  char               *home;
  char               *thotdir;
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
      home = NULL;
#     else  /* !_WINDOWS */
      home = TtaGetEnvString ("HOME");
#     endif /* _WINDOWS */

      tempfile[0] = EOS;
      /* try to load the user preferences */
      if (home)
	sprintf (tempfile, "%s%s.%s.css", home, DIR_STR, HTAppName);

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

  if (ptr[0] != EOS  && TtaFileExist (tempfile))
    {
      /* read User preferences */
      res = fopen (ptr, "r");
      if (res != NULL)
	{
	  if (fstat (fileno (res), &buf))
	    fclose (res);
	  else
	    {
	      buffer = (char*) TtaGetMemory (buf.st_size + 1000);
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
	  ReadCSSRules (0, doc, css, buffer);
	  TtaFreeMemory (buffer);
	}
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
}
