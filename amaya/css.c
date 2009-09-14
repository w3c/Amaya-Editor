/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2009
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

#undef THOT_EXPORT
#define THOT_EXPORT 
#define THOT_INITGLOBALVAR
#include "amaya.h"

#undef THOT_EXPORT
#define THOT_EXPORT 
#include "css.h"


#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "css_f.h"
#include "init_f.h"
#include "AHTURLTools_f.h"
#include "UIcss_f.h"
#include "styleparser_f.h"


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
                  /* the current screen type matches */
                  if (media == CSS_PRINT)
                    media = CSS_ALL;
                  else if (media == CSS_OTHER)
                    media = CSS_SCREEN;
                }
              else if (!strncasecmp (ptr, "print", 5))
                {
                  /* the current screen type matches */
                  if (media == CSS_SCREEN)
                    media = CSS_ALL;
                  else if (media == CSS_OTHER)
                    media = CSS_PRINT;
                }
            }
          else if (!strncasecmp (ptr, "screen", 6))
            {
              /* no screen type and media type equals screen */
              if (media == CSS_PRINT)
                media = CSS_ALL;
              else if (media == CSS_OTHER)
                media = CSS_SCREEN;
            }
          else if (!strncasecmp (ptr, "print", 5))
            {
              /* no screen type and media type equals screen */
              if (media == CSS_SCREEN)
                media = CSS_ALL;
              else if (media == CSS_OTHER)
                media = CSS_PRINT;
            }
          /* look for a separator */
          while (*ptr != EOS && *ptr != ',')
            {
              if (*ptr == ';' || *ptr == '{')
                return media;
              ptr++;
            }
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
  name2 = (char *)TtaGetMemory (length + 1);
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
      name2 = (char *)TtaGetMemory (length + 1);
      TtaGiveTextAttributeValue (attr, name2, &length);
      /* load the stylesheet file found here ! */
      NormalizeURL (name2, doc, completeURL, tempname, NULL);
      TtaFreeMemory (name2);
      /* get the right CSS context */ 
      css = SearchCSS (doc, completeURL, el, &pInfo);
    }
  else
    /* get the right CSS context */ 
    css = SearchCSS (doc, NULL, el, &pInfo);
  if (css && pInfo)
    {
      /* avoid too many redisplay */
      dispMode = TtaGetDisplayMode (doc);
      /* something changed and we are not printing */
      if (media == CSS_ALL || media == CSS_SCREEN)
        {
          if (dispMode != NoComputedDisplay)
            TtaSetDisplayMode (doc, NoComputedDisplay);
          LoadStyleSheet (completeURL, doc, el, NULL, NULL, media,
                          pInfo->PiCategory == CSS_USER_STYLE);
          /* restore the display mode */
          if (dispMode != NoComputedDisplay)
            TtaSetDisplayMode (doc, dispMode);
        }
      else
        {
          if (media == CSS_PRINT || media == CSS_OTHER)
            {
              if (dispMode != NoComputedDisplay)
                TtaSetDisplayMode (doc, NoComputedDisplay);
              UnlinkCSS (css, doc, el, TRUE, FALSE, TRUE);
              /* restore the display mode */
              if (dispMode != NoComputedDisplay)
                TtaSetDisplayMode (doc, dispMode);
            }
          /* only update the CSS media info */
          pInfo->PiMedia = media;
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
                  /* search that previous CSS context */
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
                  /* search that previous CSS context */
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
#ifdef CSS_DEBUG
      if (pInfo->PiCategory == CSS_USER_STYLE)
        printf ("AddInfoCSS CSS_USER_STYLE\n");
      else if (pInfo->PiCategory == CSS_DOCUMENT_STYLE)
        printf ("AddInfoCSS CSS_DOCUMENT_STYLE\n");
      else if (pInfo->PiCategory == CSS_IMPORT)
        printf ("AddInfoCSS CSS_IMPORT %s\n", css->url);
      else
        printf ("AddInfoCSS %s\n", css->url);
#endif /* CSS_DEBUG */
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

  css = (CSSInfoPtr)TtaGetMemory (sizeof (CSSInfo));
  if (css)
    {
      css->doc = doc;
      css->url = TtaStrdup (url);
      css->localName = TtaStrdup (localName);
      css->NextCSS = NULL;
      css ->import = (category == CSS_IMPORT);
      css->class_list = NULL;
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
              else if (url)
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
  are freed except if clearCSS is FALSE.
  The parameter link specifies the link.
  Return FALSE when the css context is freed.
  ----------------------------------------------------------------------*/
ThotBool UnlinkCSS (CSSInfoPtr css, Document doc, Element link,
                    ThotBool disabled, ThotBool removed, ThotBool clearCSS)
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
          pIS = pInfo->PiSchemas;
          if (pInfo->PiCategory == CSS_EMBED)
            {
#ifdef CSS_DEBUG
              printf ("Skip CSS_EMBED\n");
#endif /* CSS_DEBUG */
              pInfo->PiSchemas = NULL;
              TtaFreeMemory (pIS);
            }
          else if (pInfo->PiEnabled || removed)
            {
              /* disapply the CSS */
              if (pInfo->PiCategory == CSS_DOCUMENT_STYLE)
                {
                  if (clearCSS)
                    removed = TRUE;
                  if (removed)
                    css->doc = 0;
                }
#ifdef CSS_DEBUG
              if (pInfo->PiCategory == CSS_USER_STYLE)
                printf ("UnlinkCSS CSS_USER_STYLE\n");
              else if (pInfo->PiCategory == CSS_DOCUMENT_STYLE)
                printf ("UnlinkCSS CSS_DOCUMENT_STYLE\n");
              else if (pInfo->PiCategory == CSS_IMPORT)
                {
                  if (pIS)
                    printf ("UnlinkCSS CSS_IMPORT");
                  else
                    printf ("Skip CSS_IMPORT");
                  printf (" %s\n", css->url);
                }
              else
                printf ("UnlinkCSS %s\n", css->url);
#endif /* CSS_DEBUG */
              while (pIS)
                {
                  if (pIS->PiPSchema)
                    TtaUnlinkPSchema (pIS->PiPSchema, doc,
                                      pIS->PiSSchema);
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
      /* If css->import, unlink it if not used */
      if ((!used) && (clearCSS || css->import))
        {
          /* remove the local copy of the CSS file */
          if (!TtaIsPrinting ())
            TtaFileUnlink (css->localName);
          TtaFreeMemory (css->localName);
          TtaFreeMemory (css->url);
          TtaFreeMemory (css->class_list);
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
  DocHasCSS
  ----------------------------------------------------------------------*/
ThotBool DocHasCSS (Document doc)
{
  CSSInfoPtr      css, next;
  PInfoPtr        pInfo;
 
  css = CSSList;
  while (css)
    {
      next = css->NextCSS;
      pInfo = css->infos[doc];
      if (pInfo && pInfo->PiCategory != CSS_USER_STYLE)
        return TRUE;
      /* look at the next CSS context */
      css = next;
    }
  return FALSE;
}

/*----------------------------------------------------------------------
  RemoveDocCSSs removes all CSS information linked with the document.
  The parameter removed is false when the document will be reloaded 
  ----------------------------------------------------------------------*/
void RemoveDocCSSs (Document doc, ThotBool removed)
{
  CSSInfoPtr      css, next;
  PInfoPtr        pInfo, pNext;
 
  css = CSSList;
  while (css)
    {
      next = css->NextCSS;
      pInfo = css->infos[doc];
      while (css && pInfo)
        {
          pNext = pInfo->PiNext;
          if (!UnlinkCSS (css, doc, css->infos[doc]->PiLink, TRUE, removed, TRUE))
            css = NULL;
          pInfo = pNext;
        }
      if (css && css->doc == doc)
        /* the document displays the CSS file itself */
        UnlinkCSS (css, doc, NULL, TRUE, removed, TRUE);
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
      UnlinkCSS (css, doc, link, disabled, removed, TRUE);
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
  Attribute           attr;
  AttributeType       attrType;
  CSSmedia            media = CSS_ALL;
  Language            lang;
  char               *buffer, *name;
  int                 length, i, j;
  ThotBool            loadcss;

  buffer = NULL;

  /* check if we have to load CSS */
  TtaGetEnvBoolean ("LOAD_CSS", &loadcss);
  if (loadcss && el)
    {
      /* check the media type of the element */
      elType = TtaGetElementType (el);
      attrType.AttrSSchema = elType.ElSSchema;
      name = TtaGetSSchemaName (attrType.AttrSSchema);
      if (!strcmp (name, "HTML"))
        attrType.AttrTypeNum = HTML_ATTR_media;
#ifdef _SVG
      else if (!strcmp (name, "HTML"))
        attrType.AttrTypeNum = SVG_ATTR_media;
#endif /* _SVG */
      else
        attrType.AttrTypeNum = 0;
      if (attrType.AttrTypeNum)
        {
          attr = TtaGetAttribute (el, attrType);
          if (attr)
            {
              length = TtaGetTextAttributeLength (attr);
              name = (char *)TtaGetMemory (length + 1);
              TtaGiveTextAttributeValue (attr, name, &length);
              media = CheckMediaCSS (name);
              TtaFreeMemory (name);
            }
        }
      /* get enough space to store UTF-8 characters */
      length = TtaGetElementVolume (el) * 6 + 1;
      if ((media == CSS_ALL || media == CSS_SCREEN) && length > 1)
        {
          /* get the length of the included text */
          buffer = (char *)TtaGetMemory (length);
          /* fill the buffer */
          elType.ElTypeNum = 1 /* 1 = TEXT_UNIT element */;
          text = TtaSearchTypedElementInTree (elType, SearchForward, el, el);
          i = 0;
          while (text != NULL)
            {
              j = length - i;
              TtaGiveTextContent (text, (unsigned char *)&buffer[i], &j, &lang);
              i += TtaGetTextLength (text);
              text = TtaSearchTypedElementInTree (elType, SearchForward, el, text);
            }
          buffer[i] = EOS;
        }
    }
  return (buffer);
}

/*----------------------------------------------------------------------
  LoadACSSFile 
  Allocate and return a buffer that gives the local CSS file contents
  ----------------------------------------------------------------------*/
char *LoadACSSFile (char *cssfile)
{
  gzFile              res;
  int                 lenBuff = 0;
  int                 len, i, j;
  char               *tmpBuff = NULL;
#define	              COPY_BUFFER_SIZE	1024
  char                bufferRead[COPY_BUFFER_SIZE + 1];
  ThotBool            endOfFile = FALSE;

  res = TtaGZOpen (cssfile);
  if (res == NULL)
    return tmpBuff;
  while (!endOfFile)
    {
      len = gzread (res, bufferRead, COPY_BUFFER_SIZE);
      if (len < COPY_BUFFER_SIZE)
        endOfFile = TRUE;
      lenBuff += len;
    }
  len = 0;
  TtaGZClose (res);
  tmpBuff = (char *)TtaGetMemory (lenBuff + 1);
  if (tmpBuff == NULL)
    return tmpBuff;
  res = TtaGZOpen (cssfile);
  if (res == NULL)
    {
      TtaFreeMemory (tmpBuff);
      return NULL;
    }
  len = gzread (res, tmpBuff, lenBuff);
  tmpBuff[lenBuff] = 0;
  // Remove CR characters
  j = i = 0;
  while (j <= len)
    {
      if (tmpBuff[j] == __CR__)
        j++;
      if (i != j)
        tmpBuff[i] = tmpBuff[j];
      i++;
      j++;
    }
  TtaGZClose (res);
  return tmpBuff;     
}

/*----------------------------------------------------------------------
  LoadStyleSheet loads the external Style Sheet found at the given url
  (in dialog charset).
  The parameter link gives the element which links the CSS or NULL.
  The parameter css gives the CSS context which imports this CSS file.
  The parameter urlRef gives the url used to resolve relative paths.
  The parameter media gives the application limits of the CSS.
  The parameter user is true when it's a User style sheet. It's false
  when it's an author style sheet
  ----------------------------------------------------------------------*/
void LoadStyleSheet (char *url, Document doc, Element link, CSSInfoPtr css,
                     char *urlRef, CSSmedia media, ThotBool user)
{
  CSSInfoPtr          refcss = NULL;
  PInfoPtr            pInfo;
  char                tempfile[MAX_LENGTH];
  char                tempURL[MAX_LENGTH];
  char               *tmpBuff;
  CSSCategory         category;
  ThotBool            import, printing;
  ThotBool            loadcss;

  /* check if we have to load CSS */
  TtaGetEnvBoolean ("LOAD_CSS", &loadcss);
  printing = TtaIsPrinting ();

  if (!loadcss && !printing)
    return;
  import = (css != NULL);
  if (import)
    category = CSS_IMPORT;
  else if (user)
    category = CSS_USER_STYLE;
  else
    category = CSS_EXTERNAL_STYLE;
  refcss = css;

  /* get the absolute URI */
  if (import && urlRef == NULL)
    {
      if (css->url)
        urlRef = css->url;
      else
        urlRef = css->localName;
    }
  LoadRemoteStyleSheet (url, doc, link, urlRef, tempURL, tempfile);
  if (!TtaFileExist (tempfile))
    {
    // cannot load the style sheet
      TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_CANNOT_LOAD), tempURL);
      return;
    }
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
  if (import)
    {
      while (refcss && refcss->import)
        refcss = refcss->NextCSS;
      if (refcss->infos[doc])
        link = refcss->infos[doc]->PiLink;
    }
  else
    refcss = css;
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
      tmpBuff = LoadACSSFile (tempfile);
      if (tmpBuff == NULL)
        {
          TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_CANNOT_LOAD), tempURL);
          return;
        }
      if (css)
        urlRef = css->url;
      ReadCSSRules (doc, refcss, tmpBuff, urlRef, 0, FALSE, link);
      TtaFreeMemory (tmpBuff);
    }
}
