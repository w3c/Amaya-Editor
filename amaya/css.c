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
#include "query_f.h"
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
  CSSInfoPtr          oldcss;
  PInfoPtr            pInfo;
  PISchemaPtr         pIS;
  PSchema             pSchema, nSchema, prevS;
  Element             prevLink, nextLink, parent;
  ElementType	      elType;
  AttributeType       attrType;
  Attribute           attr;
  CHAR_T              buffer[MAX_LENGTH];
  int                 length;
  ThotBool            found, before;

  if (sSchema == NULL)
    sSchema = TtaGetDocumentSSchema (doc);
  pInfo = css->infos;
  found = FALSE;
  pIS = NULL;
  while (pInfo != NULL && !found)
    {
      if (pInfo->PiDoc == doc)
	{
	  /* look for the list of document schemas */
	  pIS = pInfo->PiSchemas;
	  while (pIS != NULL && !found)
	    {
	      if (sSchema == pIS->PiSSchema && pIS->PiPSchema != NULL)
		/* the pschema is already known */
		return (pIS->PiPSchema);
	      else
		pIS = pIS->PiSNext;
	    }
	  found = TRUE;
	}
      else
	/* next info context */
	pInfo = pInfo->PiNext;
    }

  if (pInfo == NULL)
    {
      /* add the presentation info block for the current document */
      pInfo = (PInfoPtr) TtaGetMemory (sizeof (PInfo));
      pInfo->PiNext = css->infos;
      css->infos = pInfo;
      pInfo->PiDoc = doc;
      pInfo->PiLink = NULL;
      pInfo->PiSchemas = NULL;
      pIS = NULL;
    }

  if (pIS == NULL)
    {
      /* add the schema info */
      pIS = (PISchemaPtr) TtaGetMemory (sizeof (PISchema));
      pIS->PiSNext = pInfo->PiSchemas;
      pInfo->PiSchemas = pIS;
      pIS->PiSSchema = sSchema;
      pIS->PiPSchema = NULL;
    }

  /* create the presentation schema for this structure */
  nSchema = TtaNewPSchema ();
  pSchema = TtaGetFirstPSchema (doc, sSchema);
  pIS->PiPSchema = nSchema;
  /* chain the presentation schema at the right position */
  prevS = NULL;
  before = FALSE;
  if (css->category == CSS_USER_STYLE || pSchema == NULL)
    {
      /* add in first position and last priority */
      /* link the new presentation schema */
      TtaAddPSchema (nSchema, pSchema, TRUE, doc, sSchema);
    }
  else if (css->category == CSS_DOCUMENT_STYLE)
    {
      /* add in last position and first priority */
      while (pSchema != NULL)
	{
	  prevS = pSchema;
	  TtaNextPSchema (&pSchema, doc, NULL);
	}
      /* link the new presentation schema */
      TtaAddPSchema (nSchema, prevS, FALSE, doc, sSchema);
    }
  else
    {
      /* check the order among its external style sheets */
      if (pInfo->PiLink != NULL)
	{
	  /* look for the previous link with rel="STYLESHEET" */
	  prevLink = pInfo->PiLink;
	  parent = TtaGetParent (prevLink);
	  elType = TtaGetElementType (prevLink);
	  attrType.AttrSSchema = elType.ElSSchema;
	  attrType.AttrTypeNum = HTML_ATTR_REL;
	  found = FALSE;
	  while (!found && prevLink != NULL)
	    {
	      prevLink = TtaSearchTypedElementInTree (elType, SearchBackward, parent, prevLink);
	      if (prevLink)
		{
		  attr = TtaGetAttribute (prevLink, attrType);
		  if (attr != 0)
		    {
		      /* get a buffer for the attribute value */
		      length = MAX_LENGTH;
		      TtaGiveTextAttributeValue (attr, buffer, &length);
		      found = (!ustrcasecmp (buffer, TEXT("STYLESHEET")) || !ustrcasecmp (buffer, TEXT("STYLE")));
		    }
		  if (found)
		    {
		      /* there is another linked CSS style sheet before */
		      oldcss = CSSList;
		      /* search if that previous CSS context */
		      while (oldcss != NULL)
			{
			  if (oldcss != css && oldcss->documents[doc] &&
			      oldcss->category == CSS_EXTERNAL_STYLE)
			    {
			      /* check if it includes a presentation schema
				 for that structure */
			      pInfo = oldcss->infos;
			      while (pInfo != NULL && pInfo->PiDoc != doc)
				pInfo = pInfo->PiNext;
			      if (pInfo != NULL && pInfo->PiLink == prevLink)
				{
				  pIS = pInfo->PiSchemas;
				  while (pIS && pIS->PiSSchema != sSchema)
				    pIS = pIS->PiSNext;
				  if (pIS && pIS->PiPSchema)
				    {
				      /* link after that presentation schema */
				      before = FALSE;
				      prevS = pIS->PiPSchema;
				    }
				  else
				    found = FALSE;
				  oldcss = NULL;
				}
			      else
				/* it's not the the previous style sheet */
				oldcss = oldcss->NextCSS;
			    }
			  else
			    oldcss = oldcss->NextCSS;
			}
		    }
		}
	    }

	  /* look for the next link with rel="STYLESHEET" */
	  nextLink = pInfo->PiLink;
	  while (!found && nextLink != NULL)
	    {
	      nextLink = TtaSearchTypedElementInTree (elType, SearchForward, parent, nextLink);
	      if (nextLink)
		{
		  attr = TtaGetAttribute (nextLink, attrType);
		  if (attr != 0)
		    {
		      /* get a buffer for the attribute value */
		      length = MAX_LENGTH;
		      TtaGiveTextAttributeValue (attr, buffer, &length);
		      found = (!ustrcasecmp (buffer, TEXT("STYLESHEET")) || !ustrcasecmp (buffer, TEXT("STYLE")));
		    }
		  /* search if the previous CSS has a presentation schema */
		  if (found)
		    {
		      /* there is another linked CSS style sheet after */
		      oldcss = CSSList;
		      while (oldcss != NULL)
			{
			  if (oldcss != css && oldcss->documents[doc] &&
			      oldcss->category == CSS_EXTERNAL_STYLE)
			    {
			      /* check if it includes a presentation schema
				 for that structure */
			      pInfo = oldcss->infos;
			      while (pInfo != NULL && pInfo->PiDoc != doc)
				pInfo = pInfo->PiNext;
			      if (pInfo != NULL && pInfo->PiLink == nextLink)
				{
				  pIS = pInfo->PiSchemas;
				  while (pIS && pIS->PiSSchema != sSchema)
				    pIS = pIS->PiSNext;
				  if (pIS && pIS->PiPSchema)
				    {
				      /* link before that presentation schema */
				      before = TRUE;
				      prevS = pIS->PiPSchema;
				    }
				  else
				    found = FALSE;
				}
			      oldcss = NULL;
			    }
			  else
			    oldcss = oldcss->NextCSS;
			}
		    }
		}
	    }

	  if (!found)
	    {
	      /* look for CSS_USER_STYLE or CSS_DOCUMENT_STYLE */
	      /* there is another linked CSS style sheet after */
	      oldcss = CSSList;
	      while (!found && oldcss != NULL)
		{
		  if (oldcss != css && oldcss->documents[doc])
		    if (oldcss->category == CSS_USER_STYLE)
		      {
			/* check if it includes a presentation schema
			   for that structure */
			pInfo = oldcss->infos;
			while (pInfo != NULL && pInfo->PiDoc != doc)
			  pInfo = pInfo->PiNext;
			if (pInfo != NULL && pInfo->PiLink == nextLink)
			  {
			    pIS = pInfo->PiSchemas;
			    while (pIS && pIS->PiSSchema != sSchema)
			      pIS = pIS->PiSNext;
			    if (pIS && pIS->PiPSchema)
			      {
				found = TRUE;
				/* add after that schema with a higher priority */
				prevS = pIS->PiPSchema;
				before = FALSE;
			      }
			    else
			      oldcss = oldcss->NextCSS;
			  }
			else
			  oldcss = oldcss->NextCSS;
		      }
		    else if (oldcss->category == CSS_DOCUMENT_STYLE)
		      {
			/* check if it includes a presentation schema
			   for that structure */
			pInfo = oldcss->infos;
			while (pInfo != NULL && pInfo->PiDoc != doc)
			  pInfo = pInfo->PiNext;
			if (pInfo != NULL && pInfo->PiLink == nextLink)
			  {
			    pIS = pInfo->PiSchemas;
			    while (pIS && pIS->PiSSchema != sSchema)
			      pIS = pIS->PiSNext;
			    if (pIS && pIS->PiPSchema)
			      {
				found = TRUE;
				/* add before that schema with a lower priority */
				prevS = pIS->PiPSchema;
				before = TRUE;
			      }
			  }
		      }
		    else
		      oldcss = oldcss->NextCSS;
		  else
		    oldcss = oldcss->NextCSS;
		}
	    }
	  if (found)
	    /* link the new presentation schema */
	    TtaAddPSchema (nSchema, prevS, before, doc, sSchema);
	  else
	    TtaAddPSchema (nSchema, pSchema, TRUE, doc, sSchema);
	}
      else
	{
	  /* link the new presentation schema */
	  TtaAddPSchema (nSchema, pSchema, TRUE, doc, sSchema);
	}
    }
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
      css->localName = TtaWCSdup (localName);
      css->url = TtaWCSdup (url);
      css->category = category;

      /* that CSS is only used by the document docRef */
      for (i = 0; i < DocumentTableLength; i++)
	{
	  css->documents[i] = FALSE;
	  css->enabled[i] = FALSE;
	  css->media[i] = CSS_ALL;
	}
      /* store information about this docRef */
      css->documents[docRef] = TRUE;
      css->enabled[docRef] = TRUE;
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
   UnlinkCSS the CSS is no longer applied to this document and if the
   parameter removed is TRUE, the link is cut.
   If this CSS is no longer used the context and attached information
   are freed.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void     UnlinkCSS (CSSInfoPtr css, Document doc, ThotBool disabled, ThotBool removed)
#else
static void     UnlinkCSS (css, doc, disabled, removed)
CSSInfoPtr      css;
Document        doc;
ThotBool        disabled;
ThotBool        removed;
#endif
{
  CSSInfoPtr          prev;
  PInfoPtr            pInfo, prevInfo;
  PISchemaPtr         pIS;
  int                 i;
  ThotBool            used;

  if (css == NULL)
    return;
  else
    {
      /* look for the specific P descriptors in the css */
      pInfo = css->infos;
      prevInfo = NULL;
      if (removed)
	css->documents[doc] = FALSE;
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
	    }
	  if (css->enabled[doc])
	    /* disapply the CSS */
	    while (pInfo->PiSchemas != NULL)
	      {
		pIS = pInfo->PiSchemas;
		if (pIS->PiPSchema)
		  {
		    TtaUnlinkPSchema (pIS->PiPSchema, pInfo->PiDoc, pIS->PiSSchema);
		    TtaCleanStylePresentation (NULL, pIS->PiPSchema, pInfo->PiDoc);
		    /* remove presentation schemas */
		    TtaRemovePSchema (pIS->PiPSchema, pInfo->PiDoc, pIS->PiSSchema);
		    pInfo->PiSchemas = pIS->PiSNext;
		    if (removed)
		      TtaFreeMemory (pIS);
		  }
	      }
	  /* free the document context */
	  if (removed)
	    TtaFreeMemory (pInfo);
	}

      /* the CSS is no longer applied */
      if (disabled)
	css->enabled[doc] = FALSE;
      /* look at if this css is alway used */
      used = (css->doc != 0);
      i = 0;
      while (!used && i < DocumentTableLength)
	{
	  used = css->documents[i];
	  i++;
	}
      if (!used)
	{
	  /* remove the local copy */
	  if (!TtaIsPrinting ())
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
	  UnlinkCSS (css, doc, TRUE, TRUE);
	}
      else if (css->documents[doc])
	UnlinkCSS (css, doc, TRUE, TRUE);
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
void            RemoveStyleSheet (STRING url, Document doc, ThotBool disabled, ThotBool removed)
#else
void            RemoveStyleSheet (url, doc, disabled, removed)
STRING          url;
Document        doc;
ThotBool        disabled;
ThotBool        removed;
#endif
{
  CSSInfoPtr          css;
  ThotBool            found;

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
    UnlinkCSS (css, doc, disabled, removed);
}


/*----------------------------------------------------------------------
  GetStyleContents returns a buffer that contains the whole text of the
  style element el. It returns NULL if the element is empty.
  The buffer should be freed by the caller.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
STRING              GetStyleContents (Element el)
#else
STRING              GetStyleContents (el)
Element             el;
#endif
{
  ElementType         elType;
  Element             text;
  Language            lang;
  STRING              buffer;
  int                 length, i, j;

  buffer = NULL;
  length = TtaGetElementVolume (el) + 1;
  if (length > 1)
    {
      /* get the length of the included text */
      buffer = TtaAllocString (length);

      /* fill the buffer */
      elType = TtaGetElementType (el);
      elType.ElTypeNum = HTML_EL_TEXT_UNIT;
      text = TtaSearchTypedElementInTree (elType, SearchForward, el, el);
      i = 0;
      while (text != NULL)
	{
	  j = length - i;
	  TtaGiveTextContent (text, &buffer[i], &j, &lang);
	  i += TtaGetTextLength (text);
	  text = TtaSearchTypedElementInTree (elType, SearchForward, el, text);
	}
      buffer[i] = EOS;
    }
  return (buffer);
}

/*----------------------------------------------------------------------
  LoadStyleSheet loads the external Style Sheet found at the given url.
  The parameter el gives the element which links the CSS or NULL.
  The parameter css gives the CSS context which imports this CSS file.
  The parameter media gives the application limits of the CSS.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void        LoadStyleSheet (STRING url, Document doc, Element el, CSSInfoPtr css, CSSmedia media)
#else
void        LoadStyleSheet (url, doc, el, css, media)
STRING      url;
Document    doc;
Element     el;
CSSInfoPtr  css;
CSSmedia    media;
#endif
{
  CSSInfoPtr          oldcss;
  PInfoPtr            pInfo;
  struct stat         buf;
  FILE               *res;
  CHAR_T              tempfile[MAX_LENGTH];
  CHAR_T              tempURL[MAX_LENGTH];
  CHAR_T*             buffer = NULL;
  char*               tmpBuff;
  int                 len;
  ThotBool            import, printing;

  import = (css != NULL);
  printing = TtaIsPrinting ();
  if (TtaGetViewFrame (doc, 1) != 0 || TtaIsPrinting ())
    {
      LoadRemoteStyleSheet (url, doc, el, css, tempURL, tempfile);
      oldcss = SearchCSS (0, tempURL);
      if (oldcss == NULL || oldcss->category != CSS_EXTERNAL_STYLE)
	{
	  /* It could be a @import CSS */
	  if (css == NULL)
	    /* It's a new CSS file: allocate a new Presentation structure */
	    css = AddCSS (0, doc, CSS_EXTERNAL_STYLE, tempURL, tempfile);
	  oldcss = css;
	  oldcss->media[doc] = media;
	}
      else if (!oldcss->documents[doc])
	{
	  /* we have to apply the style sheet to this document */
	  oldcss->documents[doc] = TRUE;
	  oldcss->enabled[doc] = TRUE;
	  /* update the current media value */
	  if (media == CSS_ALL)
	    oldcss->media[doc] = media;
	  else if (oldcss->media[doc] != media ||
		   oldcss->media[doc] != CSS_ALL)
	    if ((printing && media == CSS_PRINT) ||
		(!printing && media == CSS_SCREEN))
		oldcss->media[doc] = media;
	}

      if (tempfile[0] == EOS)
	/* cannot do more */
	return;
      else if (media == CSS_OTHER ||
	       (!printing && media == CSS_PRINT) ||
	       (printing && media == CSS_SCREEN) ||
	       !oldcss->enabled[doc])
	/* nothing more to do */
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
	  pInfo->PiLink = el;
	  pInfo->PiSchemas = NULL;
	  oldcss->infos = pInfo;
	}


      /* apply CSS rules in current Presentation structure (import) */
      if ( pInfo->PiSchemas == NULL || import)
	{
	  /* load the resulting file in memory */
	  res = ufopen (tempfile, TEXT("r"));
	  if (res == NULL)
	    {
	      TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_CANNOT_LOAD), tempURL);
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
		return;
	      }

      tmpBuff = TtaGetMemory (buf.st_size + 1000);
	  if (tmpBuff == NULL)
	    {
	      TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_CANNOT_LOAD), tempURL);
	      fclose (res);
	      return;
	    }
	  len = fread (tmpBuff, buf.st_size, 1, res);
	  if (len != 1)
	    {
	      TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_CANNOT_LOAD), tempURL);
	      fclose (res);
	      TtaFreeMemory (tmpBuff);
	      return;
	    }
	  tmpBuff[buf.st_size] = 0;
	  fclose (res);

#     ifdef _I18N_
	  buffer = TtaAllocString (buf.st_size + 1000);
      mbstowcs (buffer, tmpBuff, buf.st_size + 1000);
#     else  /* !_I18N_ */
      buffer = tmpBuff;
#     endif /* !_I18N_ */
	  ReadCSSRules (doc, oldcss, buffer, FALSE);
	  TtaFreeMemory (buffer);
	}
    }
}
