/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2003
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
  ReallocUTF8String
  If such convertion is needed, the string url is reallocated with
  the converted string.
  ----------------------------------------------------------------------*/
char *ReallocUTF8String (char *url, Document doc)
{
#ifndef _I18N_
  unsigned char *tmp;

  if (!url || *url == EOS)
    return NULL;
  /* does the URL contain chars > 127 ? */
  tmp = url;
  while (*tmp)
    {
      if (*tmp > 127)
        break;
      tmp++;
    }
  if (*tmp == EOS)
    return url;  /* no such chars found */
  tmp = TtaConvertByteToMbs (url, TtaGetDocumentCharset (doc));
  if (tmp)
    TtaFreeMemory (url);
  return tmp;
#else /* _I18N_ */
  return url;
#endif /* _I18N_ */
}

/*----------------------------------------------------------------------
  CheckMediaCSS
  Return the media given by the string
  ----------------------------------------------------------------------*/
CSSmedia CheckMediaCSS (char *buff)
{
  CSSmedia            media;
  char               *ptr;
  char               *screentype;

  if (buff)
    {
      ptr = buff;
      media = CSS_OTHER;
      screentype = TtaGetEnvString ("SCREEN_TYPE");
      while (*ptr != EOS)
	{
	  while (*ptr != EOS && *ptr == ' ')
	    ptr++;
	  if (!strncasecmp (ptr, "all", 3))
	    media = CSS_ALL;
	  else if (screentype)
	    {
	      /* a specific screen type is defined */
	      if ((!strncasecmp (screentype, "handheld", 8) &&
		   !strncasecmp (ptr, "handheld", 8)) ||
		  (!strncasecmp (screentype, "print", 5) &&
		   !strncasecmp (ptr, "print", 5)) ||
		  (!strncasecmp (screentype, "projection", 10) &&
		   !strncasecmp (ptr, "projection", 10)) ||
		  (!strncasecmp (screentype, "screen", 6) &&
		   !strncasecmp (ptr, "screen", 6)) ||
		  (!strncasecmp (screentype, "tty", 3) &&
		   !strncasecmp (ptr, "tty", 3)) ||
		  (!strncasecmp (screentype, "tv", 2) &&
		   !strncasecmp (ptr, "tv", 2)))
		{
		  if (media == CSS_PRINT)
		    media = CSS_ALL;
		  else if (media == CSS_OTHER)
		    media = CSS_SCREEN;
		}
	      else if (!strncasecmp (screentype, "print", 5) &&
		       !strncasecmp (ptr, "print", 5))
		{
		  if (media == CSS_SCREEN)
		    media = CSS_ALL;
		  else if (media == CSS_OTHER)
		    media = CSS_PRINT;
		}
	    }
	  else if (!strncasecmp (ptr, "screen", 6))
	    {
	      if (media == CSS_PRINT)
		media = CSS_ALL;
	      else if (media == CSS_OTHER)
		media = CSS_SCREEN;
	    }
	  else if (!strncasecmp (ptr, "print", 5))
	    {
	      if (media == CSS_SCREEN)
		media = CSS_ALL;
	      else if (media == CSS_OTHER)
		media = CSS_PRINT;
	    }
	  /* look for a separator */
	  while (*ptr != EOS && *ptr != ',')
	    ptr++;
	  if (*ptr == ',')
	    ptr++;
	}
      return media;
    }
  else
    return CSS_ALL;
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
  PInfoPtr            pInfo;
  DisplayMode         dispMode;
  char                completeURL[MAX_LENGTH];
  char                tempname[MAX_LENGTH];
  char               *name2;
  int                 length;

  el = event->element;
  doc = event->document;
  attr = event->attribute;
  elType = TtaGetElementType (el);
  /* get the new media value */
  length = TtaGetTextAttributeLength (attr);
  name2 = TtaGetMemory (length + 1);
  TtaGiveTextAttributeValue (attr, name2, &length);
  media = CheckMediaCSS (name2);
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
      /* get the right CSS context */ 
      css = SearchCSS (doc, completeURL, el, &pInfo);
      if (css && pInfo)
	{
	  /* avoid too many redisplay */
	  dispMode = TtaGetDisplayMode (doc);
	  if (dispMode == DisplayImmediately)
	    TtaSetDisplayMode (doc, DeferredDisplay);
	  /* something changed and we are not printing */
	  if ((media == CSS_ALL || media == CSS_SCREEN) &&
	      (pInfo->PiMedia == CSS_PRINT || pInfo->PiMedia == CSS_OTHER))
	    LoadStyleSheet (completeURL, doc, el, NULL, media,
			    pInfo->PiCategory == CSS_USER_STYLE);
	  else
	    {
	      if ((media == CSS_PRINT || media == CSS_OTHER) &&
		  (pInfo->PiMedia == CSS_ALL || pInfo->PiMedia == CSS_SCREEN))
		{
		  if (elType.ElTypeNum != HTML_EL_STYLE_)
		    el = NULL;
		  RemoveStyle (completeURL, doc, FALSE, FALSE, el, pInfo->PiCategory);
		}
	      /* only update the CSS media info */
	      pInfo->PiMedia = media;
	    }
	  /* restore the display mode */
	  if (dispMode == DisplayImmediately)
	    TtaSetDisplayMode (doc, dispMode);
	}
    }
}


/*----------------------------------------------------------------------
  GetPExtension returns the Presentation Extension Schema associated with
  the document doc and the structure sSchema
  At the same time, this funciton updates the css context.
  ----------------------------------------------------------------------*/
PSchema GetPExtension (Document doc, SSchema sSchema, CSSInfoPtr css,
		       Element link)
{
  CSSInfoPtr          oldcss;
  PInfoPtr            pInfo, oldInfo;
  PISchemaPtr         pIS, oldIS;
  PSchema             pSchema, nSchema, prevS;
  Element             prevEl, nextEl;
  ElementType	      elType, styleType, linkType, piType;
  AttributeType       attrType;
  Attribute           attr;
  char                buffer[MAX_LENGTH];
  char               *name, pname[30];
  int                 length;
  ThotBool            found, before;

  if (css == NULL)
    return NULL;

  if (sSchema == NULL)
    sSchema = TtaGetDocumentSSchema (doc);
  pInfo = css->infos[doc];
  /* generate the presentation schema name */
  if (css->url)
    {
      length = strlen (css->url);
      if (length > 29)
	{
	  strcpy (pname, "...");
	  strcat (pname, &css->url[length-26]);
	}
      else
	strcpy (pname, css->url);
    }
  else
    pname[0] = EOS;
  nextEl = NULL;
  found = FALSE;
  pIS = NULL;
  while (pInfo && !found)
    {
      if (pInfo->PiLink == link)
	{
	  /* look for the list of document schemas */
	  pIS = pInfo->PiSchemas;
	  while (pIS && !found)
	    {
	      if (sSchema == pIS->PiSSchema)
		{
		  if (pIS->PiPSchema)
		    /* the pschema is already known */
		    return (pIS->PiPSchema);
		}
	      else
		pIS = pIS->PiSNext;
	    }
	  found = TRUE;
	}
      if (!found)
	/* next info context */
	pInfo = pInfo->PiNext;
    }

  if (pInfo == NULL)
    {
      /* add the presentation info block for the current document style */
      pInfo = AddInfoCSS (doc, css, CSS_DOCUMENT_STYLE, CSS_ALL, link);
      pIS = NULL;
      if (pInfo == NULL)
	return NULL;
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
  nSchema = TtaNewPSchema (sSchema, pInfo->PiCategory == CSS_USER_STYLE);
  pSchema = TtaGetFirstPSchema (doc, sSchema);
  pIS->PiPSchema = nSchema;
  /* chain the presentation schema at the right position */
  prevS = NULL;
  before = FALSE;
  if (pInfo->PiCategory == CSS_USER_STYLE || pSchema == NULL)
    {
      /* add in first position and last priority */
      /* link the new presentation schema */
      TtaAddPSchema (nSchema, pSchema, TRUE, doc, sSchema, pname);
    }
  else if (pInfo->PiCategory == CSS_DOCUMENT_STYLE ||
	   pInfo->PiCategory == CSS_EXTERNAL_STYLE)
    {
      /* check the order among its external style sheets */
      if (pInfo->PiLink)
	{
	  /* look for the previous link with rel="STYLESHEET" */
	  prevEl = pInfo->PiLink;
	  nextEl = pInfo->PiLink;
	  elType = TtaGetElementType (prevEl);
	  name = TtaGetSSchemaName (elType.ElSSchema);
	  styleType.ElSSchema = elType.ElSSchema;
	  linkType.ElSSchema = elType.ElSSchema;
	  piType.ElSSchema = elType.ElSSchema;
	  attrType.AttrSSchema = elType.ElSSchema;
	  if (!strcmp (name, "MathML"))
	    {
	      linkType.ElTypeNum = MathML_EL_XMLPI;
	      styleType.ElTypeNum = MathML_EL_XMLPI;
	      piType.ElTypeNum = MathML_EL_XMLPI;
	      attrType.AttrTypeNum = HTML_ATTR_REL;
	    }
#ifdef _SVG
	  /* if it's a SVG document, remove the style defined in the SVG DTD */
	  else if (!strcmp (name, "SVG"))
	    {
	      linkType.ElTypeNum = SVG_EL_XMLPI;
	      styleType.ElTypeNum = SVG_EL_style__;
	      piType.ElTypeNum = SVG_EL_XMLPI;
	      attrType.AttrTypeNum = 0;
	    }
#endif /* _SVG */
	  else
	    {
	      linkType.ElTypeNum = HTML_EL_LINK;
	      styleType.ElTypeNum = HTML_EL_STYLE_;
	      piType.ElTypeNum = HTML_EL_XMLPI;
	      attrType.AttrTypeNum = HTML_ATTR_REL;
	    }

	  /* look for a previous style item (link, style, PI) */
	  found = FALSE;
	  while (!found && prevEl)
	    {
	      prevEl = TtaSearchElementAmong5Types (linkType, styleType, piType,
						    linkType, styleType,
						    SearchBackward, prevEl);
	      if (prevEl)
		{
		  if (attrType.AttrTypeNum == 0)
		    found = TRUE;
		  else
		    {
		      elType = TtaGetElementType (prevEl);
		      if (elType.ElTypeNum == linkType.ElTypeNum)
			{
			  attr = TtaGetAttribute (prevEl, attrType);
			  if (attr)
			    {
			      /* get a buffer for the attribute value */
			      length = MAX_LENGTH;
			      TtaGiveTextAttributeValue (attr, buffer, &length);
			      found = (!strcasecmp (buffer, "STYLESHEET") ||
				       !strcasecmp (buffer, "STYLE"));
			    }
			}
		      else
			found = TRUE;
		    }
		}
              if (found)
                {
                  /* there is another linked CSS style sheet before */
                  oldcss = CSSList;
                  found = FALSE;
                  /* search if that previous CSS context */
                  while (oldcss && !found)
                    {
                      oldInfo = oldcss->infos[doc];
                      while (oldInfo && !found)
                        {
                          if (oldInfo != pInfo && oldInfo->PiLink == prevEl)
                            {
                              oldIS = oldInfo->PiSchemas;
                              while (oldIS && oldIS->PiSSchema != sSchema)
                                oldIS = oldIS->PiSNext;
                              if (oldIS && oldIS->PiPSchema)
                                {
                                  /* link after that presentation schema */
                                  before = FALSE;
                                  prevS = oldIS->PiPSchema;
                                  found = TRUE;
                                }
                            }
                          if (!found)
                            oldInfo = oldInfo->PiNext;
                        }
                      if (!found)
                        /* it's not the the previous style sheet */
                        oldcss = oldcss->NextCSS;
                    }
		}
	    }

	  /* look for a next style item (link, style, PI) */
	  while (!found && nextEl)
	    {
	      nextEl = TtaSearchElementAmong5Types (linkType, styleType, piType,
						    linkType, styleType,
						    SearchForward, nextEl);
	      if (nextEl)
		{
		  if (attrType.AttrTypeNum == 0)
		    found = TRUE;
		  else
		    {
		      elType = TtaGetElementType (nextEl);
		      if (elType.ElTypeNum == linkType.ElTypeNum)
			{
			  attr = TtaGetAttribute (nextEl, attrType);
			  if (attr)
			    {
			      /* get a buffer for the attribute value */
			      length = MAX_LENGTH;
			      TtaGiveTextAttributeValue (attr, buffer, &length);
			      found = (!strcasecmp (buffer, "STYLESHEET") ||
				       !strcasecmp (buffer, "STYLE"));
			    }
			}
		      else
			found = TRUE;
		    }
		}
              if (found)
                {
                  /* there is another linked CSS style sheet before */
                  oldcss = CSSList;
                  found = FALSE;
                  /* search if that previous CSS context */
                  while (oldcss && !found)
                    {
                      oldInfo = oldcss->infos[doc];
                      while (oldInfo && !found)
                        {
                          if (oldInfo != pInfo && oldInfo->PiLink == nextEl)
                            {
                              oldIS = oldInfo->PiSchemas;
                              while (oldIS && oldIS->PiSSchema != sSchema)
                                oldIS = oldIS->PiSNext;
                              if (oldIS && oldIS->PiPSchema)
                                {
                                  /* link after that presentation schema */
                                  before = TRUE;
                                  prevS = oldIS->PiPSchema;
                                  found = TRUE;
                                }
                            }
                          if (!found)
                            oldInfo = oldInfo->PiNext;
                        }
                      if (!found)
                        /* it's not the the previous style sheet */
                        oldcss = oldcss->NextCSS;
                    }
		}
	    }
	 
	  if (!found)
	    {
	      /* look for CSS_USER_STYLE */
	      oldcss = CSSList;
	      while (oldcss && !found)
		{
		  oldInfo = oldcss->infos[doc];
		  while (oldInfo && !found)
		    {
		      if (oldInfo != pInfo &&
			  oldInfo->PiCategory == CSS_USER_STYLE)
			{
			  oldIS = oldInfo->PiSchemas;
			  while (oldIS && oldIS->PiSSchema != sSchema)
			    oldIS = oldIS->PiSNext;
			  if (oldIS && oldIS->PiPSchema)
			    {
				/* add after that schema with a higher priority */
			      prevS = oldIS->PiPSchema;
			      before = FALSE;
			      found = TRUE;
			    }
			}
		      if (!found)
			oldInfo = oldInfo->PiNext;
		    }
		  if (!found)
		    /* it's not the the previous style sheet */
		    oldcss = oldcss->NextCSS;
		}
	    }
	  if (found)
	    /* link the new presentation schema */
	    TtaAddPSchema (nSchema, prevS, before, doc, sSchema, pname);
	  else
	    TtaAddPSchema (nSchema, pSchema, TRUE, doc, sSchema, pname);
	}
      else
	{
	  /* link the new presentation schema */
	  TtaAddPSchema (nSchema, pSchema, TRUE, doc, sSchema, pname);
	}
    }
  return (nSchema);
}


/*----------------------------------------------------------------------
  AddInfoCSS adds a new info context into the CSS context css.
  The parameter link specifies the link and the CSS position for
  imported style sheets.
  ----------------------------------------------------------------------*/
PInfoPtr AddInfoCSS (Document doc, CSSInfoPtr css, CSSCategory category,
		     CSSmedia media,  Element link)
{
  PInfoPtr            pInfo;

  pInfo = NULL;
  if (css)
    {
      /* add the presentation info block */
      pInfo = (PInfoPtr) TtaGetMemory (sizeof (PInfo));
      /* add a new entry at the beginning to avoid trouble with Synchronize */
      pInfo->PiNext = css->infos[doc];
      css->infos[doc] = pInfo;
      pInfo->PiLink = link;
      pInfo->PiSchemas = NULL;
      pInfo->PiCategory = category;
      pInfo->PiMedia = media;
      /* we have to apply the style sheet to this document */
      pInfo->PiEnabled = TRUE;
    }
  return pInfo;
}


/*----------------------------------------------------------------------
  AddCSS adds a new CSS context in the list.
  The parameter link specifies the link andthe CSS position for
  imported style sheets.
  ----------------------------------------------------------------------*/
CSSInfoPtr AddCSS (Document doc, Document docRef, CSSCategory category,
		   CSSmedia media, char *url, char *localName,
		   Element link)
{
  CSSInfoPtr          css, prev;
  int                 i;

  css = TtaGetMemory (sizeof (CSSInfo));
  if (css)
    {
      css->doc = doc;
      css->url = TtaStrdup (url);
      css->localName = TtaStrdup (localName);
      css->NextCSS = NULL;
      css ->import = (category == CSS_IMPORT);
      /* that CSS is only used by the document docRef */
      for (i = 0; i < DocumentTableLength; i++)
	css->infos[i] = NULL;
      AddInfoCSS (docRef, css, category, media, link);
      /* chain to the CSS list */
      if (CSSList == NULL)
	CSSList = css;
      else if (category == CSS_IMPORT && link)
	{
	  prev = CSSList;
	  if (prev == (CSSInfoPtr) link)
	    {
	      /* that CSS becomes the first entry */
	      css->NextCSS = prev;
	      CSSList = css;
	    }
	  else
	    {
	      while (prev->NextCSS && prev->NextCSS != (CSSInfoPtr) link)
		prev = prev->NextCSS;
	      css->NextCSS = prev->NextCSS;
	      prev->NextCSS = css;
	    }
	}
      else
	{
	  prev = CSSList;
	  while (prev->NextCSS)
	    prev = prev->NextCSS;
	  prev->NextCSS = css;
	}
    }
  return css;
}

/*----------------------------------------------------------------------
  SearchCSS searchs the css corresponding to the specific url (doc == 0)
  or the CSS_DOCUMENT_STYLE css of the document.
  The parameter link specifies the link.
  When the CSS context is found the function points to that context.
  If the reference to the element is found within this context, it's
  returned into the info parameter.
  ----------------------------------------------------------------------*/
CSSInfoPtr SearchCSS (Document doc, char *url, Element link, PInfoPtr *info)
{
  CSSInfoPtr          css, match;
  PInfoPtr            pInfo;
  int                 i;

  *info = NULL;
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
	    /* no specific document is requested */
	    return css;
	  else
	    {
	      /* look for an entry with the right link */
	      pInfo = css->infos[doc];
	      while (pInfo)
		{
		  if (pInfo->PiLink == link)
		    {
		      *info = pInfo;
		      return css;
		    }
		  else
		    pInfo = pInfo->PiNext;
		}
	      /* check if the CSS is already used by another document */
	      if (css->import)
		{
		  for (i = 0; i < DocumentTableLength; i++)
		    if (css->infos[i] && css->infos[i]->PiLink == link)
		      return css;
		}
	      else
		/* it could be the right entry */
		match = css;
	    }
	}
      css = css->NextCSS;
    }
  return match;
}


/*----------------------------------------------------------------------
  UnlinkCSS the CSS is no longer applied to this document and if the
  parameter removed is TRUE, the link is cut.
  If this CSS is no longer used the context and attached information
  are freed.
  The parameter link specifies the link.
  Return FALSE when the css context is freed.
  ----------------------------------------------------------------------*/
ThotBool UnlinkCSS (CSSInfoPtr css, Document doc, Element link,
		    ThotBool disabled, ThotBool removed)
{
  CSSInfoPtr          prev;
  PInfoPtr            pInfo, prevInfo;
  PISchemaPtr         pIS;
  int                 i;
  ThotBool            used;

  if (css == NULL)
    return FALSE;
  else
    {
      /* look for the specific P descriptors in the css */
      pInfo = css->infos[doc];
      prevInfo = NULL;
      if (css->doc == doc && (pInfo == NULL || removed))
	/* the document displays the CSS file itself */
	/* or it includes a style element */
	css->doc = 0;
      else
	/* look for the right entry */
	while (pInfo && pInfo->PiLink != link)
	  {
	    prevInfo = pInfo;
	    pInfo = pInfo->PiNext;
	  }
      if (pInfo)
	{
	  if (pInfo->PiEnabled && pInfo->PiCategory != CSS_EMBED)
	    {
	      /* disapply the CSS */
	      pIS = pInfo->PiSchemas;
	      while (pIS)
		{
		  if (pIS->PiPSchema)
		    {
		      TtaCleanStylePresentation (pIS->PiPSchema, doc,
						 pIS->PiSSchema);
		      TtaUnlinkPSchema (pIS->PiPSchema, doc,
					pIS->PiSSchema);
		    }
		  pInfo->PiSchemas = pIS->PiSNext;
		  TtaFreeMemory (pIS);
		  pIS = pInfo->PiSchemas;
		}
	    }
	  /* the CSS is no longer applied */
	  if (disabled)
	    pInfo->PiEnabled = FALSE;
	  if (removed)
	    {
	      /* unlink the context from the list */
	      if (prevInfo == NULL)
		css->infos[doc] = pInfo->PiNext;
	      else
		prevInfo->PiNext = pInfo->PiNext;
	      /* free the context */
	      TtaFreeMemory (pInfo);
	    }
	}

      /* look at if this css is alway used */
      used = (css->doc != 0);
      i = 1;
      while (!used && i < DocumentTableLength)
	{
	  used = (css->infos[i] != NULL);
	  i++;
	}
      if (!used)
	{
	  /* remove the local copy of the CSS file */
	  if (!TtaIsPrinting ())
	    TtaFileUnlink (css->localName);
	  TtaFreeMemory (css->localName);
	  TtaFreeMemory (css->url);
	  if (CSSList == css)
	    CSSList = css->NextCSS;
	  else
	    {
	      prev = CSSList;
	      while (prev && prev->NextCSS != css)
		prev = prev->NextCSS;
	      if (prev)
		prev->NextCSS = css->NextCSS;
	    }
	  TtaFreeMemory (css);
	  return FALSE;
	}
    }
  /* there is still a CSS context */
  return TRUE;
}

/*----------------------------------------------------------------------
   RemoveDocCSSs removes all CSS information linked with the document.
  ----------------------------------------------------------------------*/
void RemoveDocCSSs (Document doc)
{
  CSSInfoPtr          css, next;
 
  css = CSSList;
  while (css)
    {
      next = css->NextCSS;
      if (css->doc == doc)
	/* the document displays the CSS file itself */
	/* or it includes a style element */
	UnlinkCSS (css, doc, NULL, TRUE, TRUE);
      else
	while (css && css->infos[doc])
	  {
	    if (!UnlinkCSS (css, doc, css->infos[doc]->PiLink, TRUE, TRUE))
	      css = NULL;
	  }
      /* look at the next CSS context */
      css = next;
    }
}

/*----------------------------------------------------------------------
   RemoveStyle removes a style.
   It could be an external CSS file, the User Stylesheet or a
   document Style element.
   disabled is TRUE when the CSS style sheet is disabled.
   removed is TRUE when the CSS style sheet is removed.
   category specifies the CSS category.
  ----------------------------------------------------------------------*/
void RemoveStyle (char *url, Document doc, ThotBool disabled,
		  ThotBool removed, Element link, CSSCategory category)
{
  CSSInfoPtr      css, match;
  PInfoPtr        pInfo;
  DisplayMode     dispMode;

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
	link = pInfo->PiLink;
      UnlinkCSS (css, doc, link, disabled, removed);
      /* Restore the display mode */
      if (dispMode != NoComputedDisplay)
	TtaSetDisplayMode (doc, dispMode);
    }
}

/*----------------------------------------------------------------------
  GetStyleContents returns a buffer that contains the whole text of the
  style element el. It returns NULL if the element is empty.
  The buffer should be freed by the caller.
  ----------------------------------------------------------------------*/
char *GetStyleContents (Element el)
{
  ElementType         elType;
  Element             text;
  Language            lang;
  char               *buffer;
  int                 length, i, j;
  ThotBool            loadcss;

  buffer = NULL;

  /* check if we have to load CSS */
  TtaGetEnvBoolean ("LOAD_CSS", &loadcss);
  if (loadcss)
    {
      /* get enough space to store UTF-8 characters */
      length = TtaGetElementVolume (el) * 6 + 1;
      if (length > 1)
	{
	  /* get the length of the included text */
	  buffer = TtaGetMemory (length);
	  /* fill the buffer */
	  elType = TtaGetElementType (el);
	  elType.ElTypeNum = 1 /* 1 = TEXT_UNIT element */;
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
    }
  return (buffer);
}

/*----------------------------------------------------------------------
  LoadStyleSheet loads the external Style Sheet found at the given url.
  The parameter link gives the element which links the CSS or NULL.
  The parameter css gives the CSS context which imports this CSS file.
  The parameter media gives the application limits of the CSS.
  The parameter user is true when it's a User style sheet. It's false
  when it's an authr style sheet
  ----------------------------------------------------------------------*/
void LoadStyleSheet (char *url, Document doc, Element link, CSSInfoPtr css,
		     CSSmedia media, ThotBool user)
{
  CSSInfoPtr          refcss = NULL;
  PInfoPtr            pInfo;
  struct stat         buf;
  FILE               *res;
  char                tempfile[MAX_LENGTH];
  char                tempURL[MAX_LENGTH];
  char               *tmpBuff;
  CSSCategory         category;
  int                 len;
  ThotBool            import, printing;
  ThotBool            loadcss;

  /* check if we have to load CSS */
  TtaGetEnvBoolean ("LOAD_CSS", &loadcss);
  if (!loadcss)
    return;
  import = (css != NULL);
  if (import)
    category = CSS_IMPORT;
  else if (user)
    category = CSS_USER_STYLE;
  else
    category = CSS_EXTERNAL_STYLE;
  printing = TtaIsPrinting ();
  refcss = css;
  /* get the absolute URI */
  LoadRemoteStyleSheet (url, doc, link, css, tempURL, tempfile);
  css = SearchCSS (doc, tempURL, link, &pInfo);
  if (css == NULL ||
      (import && !css->import))
    {
      /* It's a new CSS file: allocate a new Presentation structure */
      /* or a @import CSS: add the CSS descriptor just before the main css */
      css = AddCSS (0, doc, category, media, tempURL, tempfile, link);
      pInfo = css->infos[doc];
    }
  else
    {
      if (pInfo && pInfo->PiCategory != category)
	/* this entry doesn't match */
	pInfo = NULL;
      if (pInfo == NULL)
	/* add a new entry at the end */
	pInfo = AddInfoCSS (doc, css, category, media, link);
    }

  /* look for the CSS descriptor that points to the extension schema */
  refcss = css;
  if (import)
    {
      while (refcss && refcss->import)
	refcss = refcss->NextCSS;
      if (refcss->infos[doc])
	link = refcss->infos[doc]->PiLink;
    }
  /* now apply style rules */
  pInfo = refcss->infos[doc];
  if (tempfile[0] == EOS)
    /* cannot do more */
    return;
  else if (media == CSS_OTHER || css == NULL ||
	   (!printing && media == CSS_PRINT) ||
	   (printing && media == CSS_SCREEN) ||
	   !pInfo->PiEnabled)
    /* nothing more to do */
    return;

  /* apply CSS rules in current Presentation structure (import) */
  if ( pInfo->PiSchemas == NULL || import)
    {
      /* load the resulting file in memory */
      res = fopen (tempfile, "r");
      if (res == NULL)
	{
	  TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_CANNOT_LOAD), tempURL);
	  return;
	}
#ifdef _WINDOWS
      if (fstat (_fileno (res), &buf))
#else  /* _WINDOWS */
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
	  ReadCSSRules (doc, css, tmpBuff, tempURL, 0, FALSE, NULL);
	  TtaFreeMemory (tmpBuff);
	  return;
	}
      tmpBuff[buf.st_size] = 0;
      fclose (res);

      ReadCSSRules (doc, css, tmpBuff, tempURL, 0, FALSE, link);
      TtaFreeMemory (tmpBuff);
    }
}
