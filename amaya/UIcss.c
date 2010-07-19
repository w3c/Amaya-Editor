/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *
 * Author: I. Vatton
 *
 */

#ifdef _WX
#include "wx/wx.h"
#include "wx/colordlg.h"
#endif /* _WX */

/* nItagetMecluded headerfiles */
#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"
#ifdef _SVG
#include "SVG.h"
#include "svgedit.h"
#endif /* _SVG */
#ifdef _WX
#include "wxdialogapi_f.h"
#include "HTMLtable_f.h"
#include "appdialogue_wx.h"
#endif /* _WX */

static int         CSScase;
static char        CSSpath[500];
static Document    CSSdocument;
static Element    *CSSlink = NULL;
/* Use the same order of CSSCategory defined in css.h */
static const char       *DisplayCategory[]={
  "[x] " /*CSS_Unknown*/,
  "[U] " /*CSS_USER_STYLE*/,
  "[S] " /*CSS_DOCUMENT_STYLE*/,
  "[E] " /*CSS_EXTERNAL_STYLE*/,
  "[I] " /*CSS_IMPORT*/,
  "[ ] " /*CSS_EMBED*/
};
#define DisplayCategory_length 6
#define String_length  4

#include "MENUconf.h"
#include "AHTURLTools_f.h"
#include "EDITstyle_f.h"
#include "EDITORactions_f.h"
#include "HTMLactions_f.h"
#include "HTMLhistory_f.h"
#include "HTMLpresentation_f.h"
#include "HTMLedit_f.h"
#include "UIcss_f.h"
#include "css_f.h"
#include "dialog.h"
#include "fetchXMLname_f.h"
#include "html2thot_f.h"
#include "init_f.h"
#include "query_f.h"
#include "styleparser_f.h"
#include "Xmlbuilder_f.h"
#include "paneltypes_wx.h"
#include "SVGbuilder_f.h"
#include "SVGedit_f.h"
#ifdef TEMPLATES
#include "templateUtils_f.h"
#endif /* TEMPLATES */


/*----------------------------------------------------------------------
  NoStyleEdit
  Return TRUE if the style attribute change is not allowed
  ----------------------------------------------------------------------*/
static ThotBool NoStyleEdit (Element el, Document doc)
{
#ifdef TEMPLATES
  ElementType         elType;
  NotifyAttribute     event;
  AttributeType       attrType;
  char               *name;

  elType = TtaGetElementType (el);
  attrType.AttrSSchema = elType.ElSSchema;
  name = TtaGetSSchemaName (elType.ElSSchema);
  if (!strcmp (name, "Template"))
    return FALSE; // doesn't apply
  if (!strcmp(name, "MathML"))
    attrType.AttrTypeNum = MathML_ATTR_style_;
#ifdef _SVG
  else if (!strcmp(name, "SVG"))
    attrType.AttrTypeNum = SVG_ATTR_style_;
#endif /* _SVG */
  else
    attrType.AttrTypeNum = HTML_ATTR_Style_;

  event.document = doc;
  event.element = el;
  event.attributeType = attrType;
  return CheckTemplateAttrInMenu (&event);
#else /* TEMPLATES */
  return TtaIsReadOnly (el);
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  LoadRemoteStyleSheet loads a remote style sheet into a file.
  Return FALSE if it's a local file and TRUE otherwise.
  urlRef gives the url used to resolve relative paths.
  When returning, the parameter completeURL contains the normalized url
  and the parameter localfile the path of the local copy of the file.
  ----------------------------------------------------------------------*/
ThotBool LoadRemoteStyleSheet (char *url, Document doc, Element el,
                               char *urlRef, char *completeURL,
                               char *localfile)
{
  CSSInfoPtr          oldcss;
  PInfoPtr            pInfo;
  char                tempname[MAX_LENGTH];
  char               *tempdocument = NULL;
  int                 toparse;
  ThotBool            remote = FALSE;

  /* this document is displayed -> load the CSS */
  localfile[0] = EOS;
  if (urlRef)
    NormalizeURL (url, 0, completeURL, tempname, urlRef);
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
      toparse = GetObjectWWW (doc, 0, completeURL, NULL, localfile,
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
          TtaFileRename (localfile, tempdocument);
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
  InitUserStyleSheet: Generates a user Style Sheet if it doesn't exist.           
  ----------------------------------------------------------------------*/
void InitUserStyleSheet (char *url)
{
  FILE             *f;

  if (!TtaFileExist (url))
    {
      f = TtaWriteOpen (url);
      if (f)
        {
          fprintf (f, "/* This is the default Amaya CSS file */\n");
          TtaWriteClose (f);
        }
    }
}

/*----------------------------------------------------------------------
  LoadUserStyleSheet: Load the user Style Sheet found in his/her   
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
  ThotBool            loadcss, doit;

  /* check if we have to load CSS */
  TtaGetEnvBoolean ("LOAD_CSS", &loadcss);
  if (loadcss && UserCSS && TtaFileExist (UserCSS))
    {
      // there is a User stylesheet
      buffer = NULL;
      ptr = UserCSS;
      css = SearchCSS (doc, ptr, NULL, &pInfo);
      if (css == NULL)
        {
          /* store a copy of the local CSS in .amaya/0 */
          ptr = GetLocalPath (0, UserCSS);
          TtaFileCopy (UserCSS, ptr);
          /* allocate a new Presentation structure */ 
          css = AddCSS (0, doc, CSS_USER_STYLE, CSS_ALL, UserCSS, ptr, NULL);
          TtaFreeMemory (ptr);
          doit = TRUE;
        }
      else if (pInfo == NULL)
        /* not already applied */
        {
          AddInfoCSS (doc, css, CSS_USER_STYLE, CSS_ALL, NULL);
          doit = TRUE;
        }
      else if (pInfo->PiSchemas == NULL)
        doit = TRUE;
      else
        doit = FALSE;

      if (doit)
        {
          ptr = css->localName;
          if (ptr[0] != EOS  && TtaFileExist (ptr))
            {
              /* read User preferences */
              res = TtaReadOpen (ptr);
              if (res != NULL)
                {
#ifdef _WINGUI
                  if (fstat (_fileno (res), &buf))
#else  /* !_WINGUI */
                    if (fstat (fileno (res), &buf))
#endif /* !_WINGUI */
                      TtaReadClose (res);
                    else
                      {
                        buffer = (char *)TtaGetMemory (buf.st_size + 1000);
                        if (buffer == NULL)
                          TtaReadClose (res);
                        else
                          {
                            len = fread (buffer, buf.st_size, 1, res);
                            if (len != 1)
                              {
                                TtaFreeMemory (buffer);
                                buffer = NULL;
                                TtaReadClose (res);
                              }
                            else
                              {
                                buffer[buf.st_size] = 0;
                                TtaReadClose (res);
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
    }
#ifdef _WX
  /* Update the list of classes */
  TtaExecuteMenuAction ("ApplyClass", doc, 1, FALSE);
#endif /* _WX */
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
          while (prevInfo && prevInfo->PiNext)
            prevInfo = prevInfo->PiNext;
          while (pInfo)
            {
              if (prevInfo)
                prevInfo->PiNext = pInfo;
              else
                css->infos[docdest] = pInfo;
              pInfo->PiEnabled = TRUE;
              pInfo->PiCategory = CSS_EMBED;
              css->infos[docsrc] = pInfo->PiNext;
              pInfo->PiNext = NULL;
              prevInfo = pInfo;
              pInfo = css->infos[docsrc];
            }
        }
      css = css->NextCSS;
    }
}

/*----------------------------------------------------------------------
  AddStyle adds a style.
  It could be an external CSS file, the User Stylesheet or a
  document Style element.
  category specifies the CSS category.
  ----------------------------------------------------------------------*/
void AddStyle (char *url, Document doc, Element link, CSSCategory category)
{
  CSSInfoPtr      css, match;
  PInfoPtr        pInfo;
  DisplayMode     dispMode;
  int             media;

  if (category == CSS_USER_STYLE)
    {
      /* Change the Display Mode to take into account the new presentation */
      dispMode = TtaGetDisplayMode (doc);
      if (dispMode != NoComputedDisplay)
        TtaSetDisplayMode (doc, NoComputedDisplay);
      LoadUserStyleSheet (doc);
      /* Restore the display mode */
      if (dispMode != NoComputedDisplay)
        TtaSetDisplayMode (doc, dispMode);
    }
  else
    {
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
            {
              link = pInfo->PiLink;
              media = pInfo->PiMedia;
              pInfo->PiEnabled = TRUE;
            }
          else
            media = CSS_ALL;
          LoadStyleSheet (url, doc, link, NULL, NULL, (CSSmedia)media, FALSE);
          /* Restore the display mode */
          if (dispMode != NoComputedDisplay)
            TtaSetDisplayMode (doc, dispMode);
#ifdef _WX
          /* Update the list of classes */
          TtaExecuteMenuAction ("ApplyClass", doc, 1, FALSE);
#endif /* _WX */
        }
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
  Element             el;
  int		              position;
  int		              distance;
  ThotBool            found;

  css = CSSList;
  found = FALSE;
  while (css)
    {
      if (url && ((css->url && !strcmp (url, css->url)) ||
                  (css->localName && !strcmp (url, css->localName))))
        {
          /* an external CSS: update the copy in .amaya/0 */
          if (css->localName && tempdoc)
            TtaFileCopy (tempdoc, css->localName);
          doc = 1;
          /* current parsed CSS file */
          ParsedCSS = css->doc;
          if (ParsedCSS)
            {
              RemoveParsingErrors (ParsedCSS);
              CloseLogs (ParsedCSS);
            }
          while (doc < DocumentTableLength)
            {
              /* don't manage a document used by make book */
              if (DocumentMeta[doc] == NULL ||
                  DocumentMeta[doc]->method != CE_MAKEBOOK)
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

                          /* get the current position in the document */
                          position = RelativePosition (doc, &distance);
                          /* Change the Display Mode to take into account the
                             new presentation */
                          dispMode = TtaGetDisplayMode (doc);
                          if (dispMode == DisplayImmediately)
                            TtaSetDisplayMode (doc, NoComputedDisplay);
                          /* invalidate current logs */
                          RemoveParsingErrors (doc);
                          CloseLogs (doc);
                          found = TRUE; // the css file is almost parsed
                          if (refcss && refcss->infos[doc])
                            {
                              refInfo = refcss->infos[doc];
                              /* re-apply that CSS to each related document */
                              UnlinkCSS (refcss, doc, refInfo->PiLink, TRUE, FALSE, FALSE);
                              refInfo->PiEnabled = TRUE;
                              if (refInfo->PiCategory == CSS_DOCUMENT_STYLE)
                                EnableStyleElement (doc, refInfo->PiLink);
                              else if (UserCSS && !strcmp (refcss->url, UserCSS))
                                LoadUserStyleSheet (doc);
                              else
                                {
                                  LoadStyleSheet (refcss->url, doc, refInfo->PiLink, NULL,
                                                  NULL, (CSSmedia)refInfo->PiMedia,
                                                  refInfo->PiCategory == CSS_USER_STYLE);
#ifdef _WX
                                  /* Update the list of classes */
                                  TtaExecuteMenuAction ("ApplyClass", doc, 1, FALSE);
#endif /* _WX */
                                }
                              if (CSSErrorsFound)
                                {
                                  /* the CSS parser detected an error */
                                  TtaWriteClose (ErrFile);
                                  ErrFile = NULL;
                                  UpdateLogFile (doc, TRUE);
                                  if (ParsedCSS)
                                    {
                                      TtaWriteClose (CSSErrFile);
                                      CSSErrFile = NULL;
                                      UpdateLogFile (ParsedCSS, TRUE);
                                    }
                                  CSSErrorsFound = FALSE;
                                  InitInfo ("", TtaGetMessage (AMAYA, AM_CSS_ERROR));
                                }
                              else
                                {
                                  UpdateLogFile (doc, FALSE);
                                  UpdateLogFile (ParsedCSS, FALSE);
                                }
                              /* Restore the display mode */
                              if (dispMode == DisplayImmediately)
                                TtaSetDisplayMode (doc, dispMode);
                            }
                          ParsedCSS = 0;
                          /* restore the current position in the document */
                          el = ElementAtPosition (doc, position);
                          TtaShowElement (doc, 1, el, distance);
                        }
                      /* manage the next entry */
                      pInfo = nextInfo;
                    }
                }
              doc++;
            }
        }
      if (!found)
        {
          CloseLogs (ParsedCSS);
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
  int                 length, i, index;

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
                  if (pInfo->PiEnabled &&
                      (pInfo->PiMedia == CSS_ALL || pInfo->PiMedia == CSS_PRINT))
                    {
                      if (pInfo->PiCategory == CSS_DOCUMENT_STYLE)
                        /* there is an internal style in the document */
                        length += strlen (printdir) + 10;
                      else
                        /* that external or user style sheet concerns the
                           document */
                        if (css->localName)
                          length += strlen (css->localName) + 3;
                    }
                  pInfo = pInfo->PiNext;
                }
            }
          css = css->NextCSS;
        }

      /* now generate the complete list of style sheets */
      if (length)
        {
          ptr = (char *)TtaGetMemory (length + 1);
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
                      if (pInfo->PiEnabled &&
                          (pInfo->PiMedia == CSS_ALL || pInfo->PiMedia == CSS_PRINT) &&
                          pInfo->PiCategory == CSS_USER_STYLE &&
                          css->localName)
                        {
                          /* add that file name to the list */
                          strcpy (&ptr[length], "u\"");
                          length += 2;
                          strcpy (&ptr[length], css->localName);
                          length += strlen (css->localName);
                          ptr[length++] = '"';
                        }
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
                          (pInfo->PiMedia == CSS_ALL || pInfo->PiMedia == CSS_PRINT) &&
                          css->localName &&
                          (pInfo->PiCategory == CSS_EXTERNAL_STYLE ||
                           pInfo->PiCategory == CSS_IMPORT))
                        {
                          /* add that file name to the list */
                          strcpy (&ptr[length], "a\"");
                          length += 2;
                          strcpy (&ptr[length], css->localName);
                          length += strlen (css->localName);
                          ptr[length++] = '"';
                        }
                      pInfo = pInfo->PiNext;
                    }
                }
              css = css->NextCSS;
            }

          /* Then look for style elements within the document */
          el = TtaGetMainRoot (doc);
          elType = TtaGetElementType (el);
          name = TtaGetSSchemaName (elType.ElSSchema);
          index = 0;
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
                      i += 3;
                      sprintf (&tempfile[i], "%d", index);
                      index++;
                      file = TtaWriteOpen (tempfile);
                      if (file)
                        {
                          /* add that file name to the list */
                          strcpy (&ptr[length], "a\"");
                          length += 2;
                          strcpy (&ptr[length], tempfile);
                          length += strlen (tempfile);
                          ptr[length++] = '"';
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
    TtaReadClose (file);
  return ptr;
}

/*----------------------------------------------------------------------
  GenerateStyle
  Apply the current set of CSS properties to the current selection
  add is TRUE when data is added to the existing style
  check is TRUE if the function must check that edit is allowed
  -----------------------------------------------------------------------*/
static void GenerateStyle (const char * data , ThotBool add, ThotBool check)
{
  Element             el, firstC, lastC, colhead, col;
  ElementType         elType;
  Attribute           attr = NULL;
  AttributeType       attrType;
  char                 *value, *name;
  int                 doc, i, j, len;
  ThotBool            open, colcolgroup;
  PresentationContext ctxt;

  doc = TtaGetSelectedDocument();
  if (doc == 0)
    /* no selection */
    return;

  if (!TtaGetDocumentAccessMode (doc))
    {
      /* the document is in ReadOnly mode */
      TtaDisplaySimpleMessage (CONFIRM, AMAYA, AM_READONLY);
      return;
    }

  TtaGiveFirstSelectedElement (doc, &el, &i, &j);
  if (el == NULL)
    /* no selection */
    return;

  if (check && NoStyleEdit (el, doc))
    return;

  elType = TtaGetElementType (el);
  attrType.AttrSSchema = elType.ElSSchema;
  name = TtaGetSSchemaName (elType.ElSSchema);
  if (data && data[0] != EOS)
    // there are style properties to be associated with the current selection
    {
      // should the style be applied to a COL or COLGROUP element ?
      colcolgroup = FALSE;
      if (!strcmp (name, "HTML"))
        {
          if (elType.ElTypeNum == HTML_EL_COL ||
              elType.ElTypeNum == HTML_EL_COLGROUP)
            {
              // a COL or COLGROUP element is selected
              colcolgroup = TRUE;
              col = el;
            }
          else if (!colcolgroup &&
              (elType.ElTypeNum == HTML_EL_Data_cell ||
               elType.ElTypeNum == HTML_EL_Heading_cell))
            {
              // get the relevant COL or COLGROUP element
              attrType.AttrTypeNum = HTML_ATTR_Ref_column;
              attr = TtaGetAttribute (el, attrType);
              if (attr)
                {
                  TtaGiveReferenceAttributeValue (attr, &colhead);
                  if (colhead)
                    {
                      attrType.AttrTypeNum = HTML_ATTR_Ref_ColColgroup;
                      attr = TtaGetAttribute (colhead, attrType);
                      if (attr)
                        // this Column-head is linked to a COL or COLGROUP
                        {
                          TtaGiveReferenceAttributeValue (attr, &col);
                          if (col)
                            {
                              //attrType.AttrTypeNum = HTML_ATTR_span_;
                              //attr = TtaGetAttribute (col, attrType);
                              //if (!attr ||
                              //    TtaGetAttributeValue (attr) <= 1)
                              // no spanning
                              colcolgroup = TRUE;
                            }
                        }
                    }
                }
            }
        }
      if (colcolgroup)
        // a whole column with a COL or COLGROUP element is selected in a HTML
        // table. Call the table editor
        {
          // create the context of the Specific presentation driver
          ctxt = TtaGetSpecificStyleContext (doc);
          if (ctxt == NULL)
            return;
          ctxt->type = elType.ElTypeNum;
          ctxt->cssSpecificity = 1;
          ctxt->cssLine = TtaGetElementLineNumber (el);
          ctxt->destroy = FALSE;
          if (TtaIsColumnSelected (doc))
            {
              open = !TtaHasUndoSequence (doc);
              if (open)
                TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
              attrType.AttrTypeNum = HTML_ATTR_Style_;
              attr = TtaGetAttribute (col, attrType);
              if (attr == NULL)
                {
                  attr = TtaNewAttribute (attrType);
                  TtaAttachAttribute (col, attr, doc);
                }
              TtaSetAttributeText (attr, (char*)data, col, doc);
              TtaRegisterAttributeCreate (attr, col, doc);
              ApplyCSSRuleOneCol (col, ctxt, (char*)data, NULL);
              TtaSetDocumentModified (doc);
              if (open)
                TtaCloseUndoSequence (doc);
            }
          else
            GenerateInlineElement (HTML_EL_Span, NULL, HTML_ATTR_Style_, data, !add);
        }
      else if (strcmp (name, "Template"))
        GenerateInlineElement (HTML_EL_Span, NULL, HTML_ATTR_Style_, data, !add);
    }
  else if (strcmp (name, "Template"))
    // remove existing style attached to the selection
    {
      TtaGiveLastSelectedElement (doc, &lastC, &i, &j);
      if (el == lastC)
        // only one selected element
        attr = GetCurrentStyleAttribute ();
      if (attr)
        {
          // the attribute is now empty
          open = !TtaHasUndoSequence (doc);
          if (open)
            TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
          // remove style rules
          len = TtaGetTextAttributeLength (attr) + 1;
          value = (char *)TtaGetMemory (len);
          TtaGiveTextAttributeValue (attr, value, &len);
          ParseHTMLSpecificStyle (el, value, doc, 1000, TRUE);
          // remove the attribute
          TtaRegisterAttributeDelete (attr, el, doc);
          TtaRemoveAttribute (el, attr, doc);
          DeleteSpanIfNoAttr (el, doc, &firstC, &lastC);
          TtaSelectElement (doc, firstC);
          if (lastC != firstC)
            TtaExtendSelection (doc, lastC, TtaGetElementVolume (lastC) + 1);
          if (open)
            TtaCloseUndoSequence (doc);
        }
    }
}

/*----------------------------------------------------------------------
  NoCSSEditing returns TRUE if any CSS editing is available
  ----------------------------------------------------------------------*/
ThotBool NoCSSEditing (Document doc)
{
  if (!TtaGetDocumentAccessMode (doc))
    return TRUE;
  if (DocumentTypes[doc] == docText || DocumentTypes[doc] == docSource ||
      DocumentTypes[doc] == docXml || DocumentTypes[doc] == docTemplate ||
      DocumentTypes[doc] == docImage || DocumentTypes[doc] == docLibrary)
    return TRUE;
  return FALSE;
}

/*----------------------------------------------------------------------
  GetEnclosingBlock
  ----------------------------------------------------------------------*/
static ThotBool GetEnclosingBlock (Document doc)
{
  Element             first, last;
  ElementType	        elType;
  int                 i, j;

  if (DocumentTypes[doc] == docText || DocumentTypes[doc] == docSource ||
      DocumentTypes[doc] == docXml || DocumentTypes[doc] == docTemplate)
    return FALSE;
  TtaGiveFirstSelectedElement (doc, &first, &i, &j);
  if (first == NULL)
    return FALSE;
  if (NoStyleEdit (first, doc))
    return FALSE;
  while (IsCharacterLevelElement (first))
    // look for a block element
    first = TtaGetParent (first);
  elType = TtaGetElementType (first);
  while (first &&
         TtaHasHiddenException (elType) &&
         elType.ElSSchema &&
         strcmp (TtaGetSSchemaName (elType.ElSSchema), "Template"))
    {
      // skip hidden and template elements
      first = TtaGetParent (first);
      elType = TtaGetElementType (first);
    }

  TtaGiveLastSelectedElement (doc, &last, &i, &j);
  while (IsCharacterLevelElement (last))
    // look for a block element
    last = TtaGetParent (last);
  elType = TtaGetElementType (last);
  while (last &&
         TtaHasHiddenException (elType) &&
         elType.ElSSchema &&
         strcmp (TtaGetSSchemaName (elType.ElSSchema), "Template"))
    {
      // skip hidden and template elements
      last = TtaGetParent (last);
      elType = TtaGetElementType (last);
    }

  if (first)
    {
      TtaSelectElement (doc, first);
      if (last != first)
        TtaExtendSelection (doc, last, TtaGetElementVolume (last) + 1);
      return TRUE;
    }
  else
    // no block found
    return FALSE;
}

/*----------------------------------------------------------------------
  NewSpanElement
  Apply color style
  ----------------------------------------------------------------------*/
static Element NewSpanElement (Document doc, ThotBool *open)
{
  Element             first, parent, el = NULL;
  ElementType         elType;
  int                 firstChar, lastChar, i;
  ThotBool            before = FALSE;

  *open = FALSE;
  if ( TtaIsSelectionEmpty ())
    {
      TtaGiveFirstSelectedElement (doc, &first, &firstChar, &lastChar);
      parent = TtaGetParent (first);
      elType = TtaGetElementType (parent);
      i =  TtaGetElementVolume (first);
      if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML") &&
          elType.ElTypeNum == HTML_EL_Span)
        {
          elType.ElTypeNum = HTML_EL_TEXT_UNIT;
          if (firstChar == 1 && lastChar == 0 && first == TtaGetFirstChild (parent))
            {
              *open = TRUE;
              before = TRUE;
            }
          else if  (lastChar >= i && first == TtaGetLastChild (parent))
            {
              *open = TRUE;
              before = FALSE;
            }

          if (*open)
            {
              *open = !TtaHasUndoSequence (doc);
              if (*open)
                TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
              el = TtaNewElement (doc, elType);
              TtaInsertSibling (el, parent, before, doc);
              TtaSelectElement (doc, el);
              TtaRegisterElementCreate (el, doc);
            }
        }
    }
  return el;
}

/*----------------------------------------------------------------------
  DoStyleColor
  Apply color style
  ----------------------------------------------------------------------*/
void DoStyleColor (char *color, ThotBool isBg)
{
  Document            doc;
  DisplayMode         dispMode;
  ElementType         elType;
  Element             el = NULL;
  int                 col, bg_col, new_col, firstChar, lastChar;
  unsigned short      red, green, blue;
  char               *ptr, *name;
  char                buffer[50];
  ThotBool            open = FALSE;

  doc = TtaGetSelectedDocument();
  if (color == NULL)
    /* document is ReadOnly */
    return;
  if (NoCSSEditing (doc))
    return;
  // new thot color
  ptr = strstr (color, "#");
  if (ptr == NULL)
    return;

  TtaGiveRGB (ptr, &red, &green, &blue);
  new_col = TtaGetThotColor (red, green, blue);

  // check the current color
  TtaGiveFirstSelectedElement (doc, &el, &firstChar, &lastChar);
  if (el == NULL)
    return;

  elType = TtaGetElementType (el);
  name = TtaGetSSchemaName (elType.ElSSchema);
  if (!strcmp (name, "Template"))
    return;

  TtaGiveBoxColors (el, doc, 1, &col, &bg_col);
  if ((isBg && new_col == bg_col) || (!isBg && new_col == col))
    // do nothing
    return;

  /* Need to force a redisplay */
  dispMode = TtaGetDisplayMode (doc);
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, DeferredDisplay);
  if (!strcmp (name, "SVG"))
    {
      /* It's an SVG element */
      if (elType.ElTypeNum == SVG_EL_g ||
	  elType.ElTypeNum == SVG_EL_rect ||
	  elType.ElTypeNum == SVG_EL_circle_ ||
	  elType.ElTypeNum == SVG_EL_ellipse ||
	  elType.ElTypeNum == SVG_EL_polyline ||
	  elType.ElTypeNum == SVG_EL_polygon ||
	  elType.ElTypeNum == SVG_EL_path ||
	  (!isBg && elType.ElTypeNum == SVG_EL_line_) ||
	  elType.ElTypeNum == SVG_EL_text_ ||
	  elType.ElTypeNum == SVG_EL_tspan ||
	  elType.ElTypeNum == SVG_EL_foreignObject)
        {
          if (isBg)
            sprintf( buffer, "fill:%s", color);
          else 
            sprintf( buffer, "stroke:%s", color);

          TtaSetDisplayMode (doc, DisplayImmediately);
          if ((isBg && new_col == bg_col) || new_col != col)
            GenerateStyle (buffer, TRUE, TRUE);
          TtaSetDisplayMode (doc, dispMode);
        }
    }
  else
    {
      if (isBg)
        sprintf( buffer, "background-color:%s", color);
      else 
        sprintf( buffer, "color:%s", color);
      el = NewSpanElement (doc, &open);
      if (el)
        TtaGiveBoxColors (el, doc, 1, &col, &bg_col);
      if ((isBg && new_col == bg_col) || new_col != col)
          GenerateStyle (buffer, TRUE, TRUE);
    }

  if (open)
    TtaCloseUndoSequence (doc);
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, dispMode);

  // Update menus
  UpdateContextSensitiveMenus (doc, 1);
}


/*----------------------------------------------------------------------
  UpdateStylePanel
  ----------------------------------------------------------------------*/
void UpdateStylePanel (Document doc, View view)
{
#ifdef _WX
  AmayaParams p;

  p.param1 = doc;
  TtaSendDataToPanel (WXAMAYA_PANEL_STYLE, p );
#endif /* _WX */
}

/*----------------------------------------------------------------------
  DoSelectFontSize
  Change the font size of the selection
  ----------------------------------------------------------------------*/
void DoSelectFontSize (Document doc, View view)
{
  Element             el = NULL;
  ElementType         elType;
  DisplayMode         dispMode;
  TypeUnit            unit;
  char                font_string[100], *name;
  int                 firstChar, lastChar;
  int                 size, family;
  ThotBool            open = FALSE;

  doc = TtaGetSelectedDocument();
  if (NoCSSEditing (doc))
    /* document is ReadOnly */
    return;

  TtaGiveFirstSelectedElement (doc, &el, &firstChar, &lastChar);
  if (el == NULL)
    return;
  if (NoStyleEdit (el, doc))
    return;
  elType = TtaGetElementType (el);
  name = TtaGetSSchemaName (elType.ElSSchema);
  if (!strcmp (name, "Template"))
    return;

  TtaGiveBoxFontInfo (el, doc, 1, &size, &unit, &family);
  if (el && size != -1 && (size != Current_FontSize || unit != UnPoint))
    {
      /* Need to force a redisplay */
      dispMode = TtaGetDisplayMode (doc);
      if (dispMode == DisplayImmediately)
        TtaSetDisplayMode (doc, DeferredDisplay);
      NewSpanElement (doc, &open);
      sprintf (font_string, "font-size: %dpt", Current_FontSize);
      GenerateStyle (font_string, TRUE, FALSE);
      if (open)
        TtaCloseUndoSequence (doc);
      if (dispMode == DisplayImmediately)
        TtaSetDisplayMode (doc, dispMode);
    }
}

/*----------------------------------------------------------------------
  DoSelectFontFamily
  Change the font family of the selection
  ----------------------------------------------------------------------*/
void DoSelectFontFamily (Document doc, View view)
{
  Element             el = NULL;
  ElementType         elType;
  DisplayMode         dispMode;
  char               *name;
  int                 firstChar, lastChar;
  int                 size, family;
  TypeUnit            unit;
  ThotBool            open = FALSE;

  doc = TtaGetSelectedDocument();
  if (NoCSSEditing (doc))
    /* document is ReadOnly */
    return;

  TtaGiveFirstSelectedElement (doc, &el, &firstChar, &lastChar);
  if (el == NULL)
    return;
  if (NoStyleEdit (el, doc))
    return;
  elType = TtaGetElementType (el);
  name = TtaGetSSchemaName (elType.ElSSchema);
  if (!strcmp (name, "Template"))
    return;

  TtaGiveBoxFontInfo (el, doc, 1, &size, &unit, &family);
  if (el && size != -1 && family != Current_FontFamily)
    {
      /* Need to force a redisplay */
      dispMode = TtaGetDisplayMode (doc);
      if (dispMode == DisplayImmediately)
        TtaSetDisplayMode (doc, DeferredDisplay);
      NewSpanElement (doc, &open);
      switch (Current_FontFamily)
        {
        case 3:
          GenerateStyle ("font-family: Courier New,Courier,monospace", TRUE, FALSE);
          break;
        case 2:
          GenerateStyle ("font-family: Arial,Helvetica,sans-serif", TRUE, FALSE);
          break;
        default:
          GenerateStyle ("font-family: Times New Roman,Times,serif", TRUE, FALSE);
        }
      if (open)
        TtaCloseUndoSequence (doc);
      if (dispMode == DisplayImmediately)
        TtaSetDisplayMode (doc, dispMode);
    }
}

/*----------------------------------------------------------------------
  DoSelectFontFamily
  Change the font family and the font size of the selection
  ----------------------------------------------------------------------*/
void DoSelectFontFamilyAndSize (Document doc, View view)
{
  Element             el = NULL;
  ElementType         elType;
  DisplayMode         dispMode;
  char                font_string[200], *name;
  int                 firstChar, lastChar;
  int                 size, family, l;
  TypeUnit            unit;
  ThotBool            open = FALSE;

  doc = TtaGetSelectedDocument();
  if (NoCSSEditing (doc))
    /* document is ReadOnly */
    return;

  TtaGiveFirstSelectedElement (doc, &el, &firstChar, &lastChar);
  if (el == NULL)
    return;
  if (NoStyleEdit (el, doc))
    return;
  elType = TtaGetElementType (el);
  name = TtaGetSSchemaName (elType.ElSSchema);
  if (!strcmp (name, "Template"))
    return;

  TtaGiveBoxFontInfo (el, doc, 1, &size, &unit, &family);
  if (el && size != -1 && family != Current_FontFamily)
    {
      /* Need to force a redisplay */
      dispMode = TtaGetDisplayMode (doc);
      if (dispMode == DisplayImmediately)
        TtaSetDisplayMode (doc, DeferredDisplay);
      NewSpanElement (doc, &open);
      font_string[0] = EOS;
      switch (Current_FontFamily)
        {
        case 3:
          strcpy (font_string, "font_string, font-family: Courier New,Courier,monospace; ");
          break;
        case 2:
          strcpy (font_string, "font-family: Arial,Helvetica,sans-serif; ");
          break;
        default:
          strcpy (font_string, "font-family: Times New Roman,Times,serif; ");
        }
      l = strlen (font_string);
      if (size != Current_FontSize || unit != UnPoint)
        sprintf (&font_string[l], "font-size: %dpt", Current_FontSize);
      GenerateStyle (font_string, TRUE, FALSE);
      if (open)
        TtaCloseUndoSequence (doc);
      if (dispMode == DisplayImmediately)
        TtaSetDisplayMode (doc, dispMode);
    }
}

/*----------------------------------------------------------------------
  DoSelectFont
  Dialog to change the font family and/or the font size of the selection
  ----------------------------------------------------------------------*/
void DoSelectFont (Document doc, View view)
{
  Element             el = NULL;
  ElementType         elType;
  DisplayMode         dispMode;
  char               *name;
  int                 firstChar, lastChar;
  int                 family, size;
  ThotBool            open = FALSE;

  if (NoCSSEditing (doc))
    /* document is ReadOnly */
    return;

  TtaGiveFirstSelectedElement (doc, &el, &firstChar, &lastChar);
  if (el == NULL)
    {
      TtaDisplaySimpleMessage (CONFIRM, AMAYA, AM_NO_SELECTION);
      return;
    }
  if (NoStyleEdit (el, doc))
    {
      TtaDisplaySimpleMessage (CONFIRM, AMAYA, AM_NOT_ALLOWED);
      return;
    }
  elType = TtaGetElementType (el);
  name = TtaGetSSchemaName (elType.ElSSchema);
  if (!strcmp (name, "Template"))
    {
      TtaDisplaySimpleMessage (CONFIRM, AMAYA, AM_INVALID_SELECTION);
      return;
    }

  if (el)
    {
      family = Current_FontFamily;
      size = Current_FontSize;
      if (CreateFontDlgWX (TtaGetViewFrame (doc, view),
                           TtaGetMessage(AMAYA,AM_CHOOSE_FONT),
                           &family, &size))
        {
          open = !TtaHasUndoSequence (doc);
          if (open)
            TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
          /* Need to force a redisplay */
          dispMode = TtaGetDisplayMode (doc);
          if (dispMode == DisplayImmediately)
            TtaSetDisplayMode (doc, DeferredDisplay);
          if (Current_FontFamily != family && Current_FontSize != size)
            {
              // change family and size
              Current_FontFamily = family;
              Current_FontSize = size;
              DoSelectFontFamilyAndSize (doc, view);
            }
          else if (Current_FontFamily != family)
            {
              Current_FontFamily = family;
              DoSelectFontFamily (doc, view);
            }
          else if (Current_FontSize != size)
            {
              Current_FontSize = size;
              DoSelectFontSize (doc, view);
            }
          if (dispMode == DisplayImmediately)
            TtaSetDisplayMode (doc, dispMode);
          if (open)
            TtaCloseUndoSequence (doc);
        }
    }
}

/*----------------------------------------------------------------------
  DoSelectColor
  Apply color style
  ----------------------------------------------------------------------*/
void DoSelectColor (Document doc, View view)
{
  wxColour            c;
  wxColourData        colour_data;
  char                color_string[100];
  unsigned short      red;
  unsigned short      green;
  unsigned short      blue;

  if (NoCSSEditing (doc))
    /* document is ReadOnly */
    return;

  if (Current_Color != -1)
    TtaGiveThotRGB (Current_Color, &red, &green, &blue);
  else
    TtaGiveThotRGB (0, &red, &green, &blue);

  colour_data.SetColour( wxColour( red, green, blue ) );
  wxColourDialog dialog (NULL, &colour_data);
  if (dialog.ShowModal() == wxID_OK)
    {
      colour_data = dialog.GetColourData();
      c = colour_data.GetColour();
      Current_Color = TtaGetThotColor (c.Red(), c.Green(), c.Blue());
      sprintf( color_string, "#%02x%02x%02x", c.Red(), c.Green(), c.Blue());
      DoStyleColor (color_string, FALSE);
    }
  UpdateStylePanel (doc, view);
}

/*----------------------------------------------------------------------
  DoSelectBgColor
  Apply color style
  ----------------------------------------------------------------------*/
void DoSelectBgColor (Document doc, View view)
{
  wxColour            c;
  wxColourData        colour_data;
  char                color_string[100];
  unsigned short      red;
  unsigned short      green;
  unsigned short      blue;

  if (NoCSSEditing (doc))
    /* document is ReadOnly */
    return;

  if (Current_BackgroundColor != -1)
    TtaGiveThotRGB (Current_BackgroundColor, &red, &green, &blue);
  else
    TtaGiveThotRGB (0, &red, &green, &blue);
    
  colour_data.SetColour( wxColour( red, green, blue ) );
  wxColourDialog dialog (NULL, &colour_data);
  if (dialog.ShowModal() == wxID_OK)
    {
      colour_data = dialog.GetColourData();
      c = colour_data.GetColour();
      Current_BackgroundColor = TtaGetThotColor (c.Red(), c.Green(), c.Blue());

      sprintf( color_string, "#%02x%02x%02x",
               c.Red(), c.Green(), c.Blue());
      DoStyleColor (color_string, TRUE);
    }
  UpdateStylePanel (doc, view);
}

/*----------------------------------------------------------------------
  DoSelectStrokeColor
  Apply color style
  ----------------------------------------------------------------------*/
void DoSelectStrokeColor (Document doc, View view)
{
  wxColour            c;
  wxColourData        colour_data;
  char                color_string[100];
  unsigned short      red;
  unsigned short      green;
  unsigned short      blue;

  if (NoCSSEditing (doc))
    /* document is ReadOnly */
    return;

  if (Current_StrokeColor != -1)
    TtaGiveThotRGB (Current_StrokeColor, &red, &green, &blue);
  else
    TtaGiveThotRGB (0, &red, &green, &blue);

  colour_data.SetColour( wxColour( red, green, blue ) );
  wxColourDialog dialog (NULL, &colour_data);
  if (dialog.ShowModal() == wxID_OK)
    {
      colour_data = dialog.GetColourData();
      c = colour_data.GetColour();
      Current_StrokeColor = TtaGetThotColor (c.Red(), c.Green(), c.Blue());
      sprintf( color_string, "#%02x%02x%02x", c.Red(), c.Green(), c.Blue());
      DoStyleColor (color_string, FALSE);
    }
  UpdateStylePanel (doc, view);
}

/*----------------------------------------------------------------------
  DoSelectFillColor
  Apply color style
  ----------------------------------------------------------------------*/
void DoSelectFillColor (Document doc, View view)
{
  wxColour            c;
  wxColourData        colour_data;
  char                color_string[100];
  unsigned short      red;
  unsigned short      green;
  unsigned short      blue;

  if (NoCSSEditing (doc))
    /* document is ReadOnly */
    return;

  if (Current_FillColor != -1)
    TtaGiveThotRGB (Current_FillColor, &red, &green, &blue);
  else
    TtaGiveThotRGB (0, &red, &green, &blue);
    
  colour_data.SetColour( wxColour( red, green, blue ) );
  wxColourDialog dialog (NULL, &colour_data);
  if (dialog.ShowModal() == wxID_OK)
    {
      colour_data = dialog.GetColourData();
      c = colour_data.GetColour();
      Current_FillColor = TtaGetThotColor (c.Red(), c.Green(), c.Blue());

      sprintf( color_string, "#%02x%02x%02x",
               c.Red(), c.Green(), c.Blue());
      DoStyleColor (color_string, TRUE);
    }
  UpdateStylePanel (doc, view);
  TtaUpdateAttrMenu(doc);
}

/*----------------------------------------------------------------------
  DoSelectGlobalOpacity
  Change the opacity of the selection
  ----------------------------------------------------------------------*/
void DoSelectOpacity(Document doc, View view)
{
  DoStyleSVG (doc, view, Current_Opacity, PROpacity);
}

/*----------------------------------------------------------------------
  DoSelectFillOpacity
  Change the opacity of the selection
  ----------------------------------------------------------------------*/
void DoSelectFillOpacity(Document doc, View view)
{
  DoStyleSVG (doc, view, Current_FillOpacity, PRFillOpacity);
}

/*----------------------------------------------------------------------
  DoSelectStrokeOpacity
  Change the opacity of the selection
  ----------------------------------------------------------------------*/
void DoSelectStrokeOpacity(Document doc, View view)
{
  DoStyleSVG (doc, view, Current_StrokeOpacity, PRStrokeOpacity);
}

/*----------------------------------------------------------------------
  DoSelectStrokeWidth
  ----------------------------------------------------------------------*/
void DoSelectStrokeWidth(Document doc, View view)
{
  DoStyleSVG (doc, view, Current_StrokeWidth, PRLineWeight);
}

/*----------------------------------------------------------------------
  DoStyleSVG
  ----------------------------------------------------------------------*/
void DoStyleSVG (Document doc, View view, int current_value, int type)
{
  Element             el = NULL;
  ElementType         elType;
  PRule               rule;
  DisplayMode         dispMode;
  int                 firstChar, lastChar;
  int                 value = 0;
  char                buffer[100], *name;
  ThotBool            open = FALSE;

  doc = TtaGetSelectedDocument();
  if (NoCSSEditing (doc))
    /* document is ReadOnly */
    return;

  TtaGiveFirstSelectedElement (doc, &el, &firstChar, &lastChar);
  if (el == NULL)
    return;
  elType = TtaGetElementType (el);
  name = TtaGetSSchemaName (elType.ElSSchema);
  if (strcmp (name, "SVG"))
    return;
  if (el)
    {
      rule = TtaGetPRule(el, type);
      if (rule)
        value = TtaGetPRuleValue (rule);
      if (value != current_value)
        {
          /* Need to force a redisplay */
          dispMode = TtaGetDisplayMode (doc);
          if (dispMode == DisplayImmediately)
            TtaSetDisplayMode (doc, DeferredDisplay);
          
          //	  NewSpanElement (doc, &open);
          switch(type)
            {
            case PROpacity:
              sprintf(buffer, "opacity: %g", ((float)current_value) / 100);
              TtaSetPRuleValue (el, rule, 10*current_value, doc);
              break;
              
            case PRStrokeOpacity:
              sprintf(buffer, "stroke-opacity: %g", ((float)current_value) / 100);
              TtaSetPRuleValue (el, rule, 10*current_value, doc);
              break;
              
            case PRFillOpacity:
              sprintf(buffer, "fill-opacity: %g", ((float)current_value) / 100);
              TtaSetPRuleValue (el, rule, 10*current_value, doc);
              break;
              
            case PRLineWeight:
              sprintf(buffer, "stroke-width: %d", current_value);
              // TtaSetPRuleValue (el, rule, current_value, doc);
              break;
            }

          GenerateStyle (buffer, TRUE, TRUE);

          /* SVG stroke width has changed: so does the size of markers */
          if (type == PRLineWeight)
            TtaUpdateMarkers(el, doc, TRUE, TRUE);
          TtaSetDisplayMode (doc, dispMode);
          
          if (open)
            TtaCloseUndoSequence (doc);
          UpdateStylePanel (doc, view);
          TtaUpdateAttrMenu(doc);
        }
    }
}

/*----------------------------------------------------------------------
  UpdateStylePanelSVG
  Update the style panel to display the properties of el
  ----------------------------------------------------------------------*/
void UpdateStylePanelSVG(Document doc, View view, Element el)
{
  PRule               rule;
  int stroke, fill;

  if (NoCSSEditing (doc))
    /* document is ReadOnly */
    return;

  if (el)
    {
      /* Opacity */
      rule = TtaGetPRule(el, PROpacity);
      if(rule)
        Current_Opacity = TtaGetPRuleValue (rule)/10;
      else
        Current_Opacity = 100;

      /* Stroke-Opacity */
      rule = TtaGetPRule(el, PRStrokeOpacity);
      if(rule)
        Current_StrokeOpacity = TtaGetPRuleValue (rule)/10;
      else
        Current_StrokeOpacity = 100;

      /* Fill-Opacity */
      rule = TtaGetPRule(el, PRFillOpacity);
      if(rule)
        Current_FillOpacity = TtaGetPRuleValue (rule)/10;
      else
        Current_FillOpacity = 100;

      /* Stroke-Width */
      rule = TtaGetPRule(el, PRLineWeight);
      if(rule)
        Current_StrokeWidth = TtaGetPRuleValue (rule);
      else
        Current_StrokeWidth = 1;

      /* Stroke and Fill */
      TtaGiveBoxColors (el, doc, 1, &stroke, &fill);
      /* TODO: find how to know when stroke and fill values 
         are None, Inherit or currentColor */

      if (stroke >= 0)
        {
          Current_StrokeColor = stroke;
          StrokeEnabled = TRUE;
        }
      else
        StrokeEnabled = FALSE;

      Current_FillColor = fill;
      FillEnabled = (fill >= 0);
      UpdateStylePanel (doc, view);
    }
}

/*----------------------------------------------------------------------
  DoUpdateStrokeStatus
  ----------------------------------------------------------------------*/
void DoUpdateStrokeStatus(Document doc, View view)
{
  char                color_string[100];
  unsigned short      red;
  unsigned short      green;
  unsigned short      blue;
  DisplayMode         dispMode;

  dispMode = TtaGetDisplayMode (doc);
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, DeferredDisplay);

  if(StrokeEnabled)
    {
      /* Remove the stroke style */
      RemoveSpecificStyle (doc, "stroke:");
      RemoveSpecificStyle (doc, "stroke-width:");
      RemoveSpecificStyle (doc, "stroke-linecap:");
      RemoveSpecificStyle (doc, "stroke-linejoin:");
      RemoveSpecificStyle (doc, "stroke-miterlimit:");
      RemoveSpecificStyle (doc, "stroke-dasharray:");
      RemoveSpecificStyle (doc, "stroke-dashoffset:");
      RemoveSpecificStyle (doc, "stroke-opacity:");
      GenerateStyle ("stroke: none", TRUE, TRUE);
      StrokeEnabled = FALSE;
    }
  else
    {
      /* Add stroke style */
      if (Current_StrokeColor != -1)
	TtaGiveThotRGB (Current_StrokeColor, &red, &green, &blue);
      else
	TtaGiveThotRGB (0, &red, &green, &blue);
      sprintf( color_string, "#%02x%02x%02x", red, green, blue);
      DoStyleColor (color_string, FALSE);

      DoSelectStrokeOpacity(doc, view);
      DoSelectStrokeWidth(doc, view);
      StrokeEnabled = TRUE;
    }

  UpdateStylePanel (doc, view);
  TtaUpdateAttrMenu(doc);
  TtaSetDisplayMode (doc, dispMode);
}

/*----------------------------------------------------------------------
  DoUpdateFillStatus
  ----------------------------------------------------------------------*/
void DoUpdateFillStatus(Document doc, View view)
{
  Element             el;
  int                 i, j;
  char                color_string[100];
  unsigned short      red;
  unsigned short      green;
  unsigned short      blue;
  DisplayMode         dispMode;

  TtaGiveFirstSelectedElement (doc, &el, &i, &j);
  if (el == NULL)
    /* no selection */
    return;
  if (NoStyleEdit (el, doc))
    return;
  
  dispMode = TtaGetDisplayMode (doc);
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, DeferredDisplay);

  if(FillEnabled)
    {
      /* Remove the fill style */
      RemoveSpecificStyle (doc, "fill:");
      RemoveSpecificStyle (doc, "fill-rule:");
      RemoveSpecificStyle (doc, "fill-opacity:");
      GenerateStyle ("fill: none", TRUE, FALSE);
      FillEnabled = FALSE;
    }
  else
    {
      /* Add fill style */
      if (Current_FillColor != -1)
        TtaGiveThotRGB (Current_FillColor, &red, &green, &blue);
      else
        TtaGiveThotRGB (0, &red, &green, &blue);
      sprintf( color_string, "#%02x%02x%02x", red, green, blue);
      DoStyleColor (color_string, TRUE);

      DoSelectFillOpacity(doc, view);
      FillEnabled = TRUE;
    }

  UpdateStylePanel (doc, view);
  TtaUpdateAttrMenu(doc);
  TtaSetDisplayMode (doc, dispMode);
}

/*----------------------------------------------------------------------
  CleanUpAttribute removes the CSS rule (data) from the attribute value
  Return TRUE if the selection will change
  -----------------------------------------------------------------------*/
static ThotBool CleanUpAttribute (Attribute attr, const char *data, Element el, Document doc)
{
  char     *buffer, *property, *start, *stop, *ptr, *old;
  int       lg;
  ThotBool  selChange = FALSE;

  property = TtaStrdup (data);
  if (property == NULL)
    return selChange;
  ptr = strstr (property, ":");
  lg = TtaGetTextAttributeLength (attr);
  if (lg && ptr)
    {
      // look for the property in the initial string
      buffer = (char *)TtaGetMemory (lg + 2);
      TtaGiveTextAttributeValue (attr, buffer, &lg);
      *ptr = EOS;
      start = strstr (buffer, property);
      lg = strlen(property); // property length
      if (start && start != buffer && start[-1] != SPACE && start[-1] != ';' &&
          start[lg] != ':')
        start = NULL;
      if (start)
        {
          stop = start;
          while (*stop != EOS && *stop != ';')
            stop++;
          while (*stop != EOS)
            {
              stop++;
              *start = *stop;
              start++;
            }
          *start = EOS;
          if (buffer[0] == EOS)
            {
              Element firstC, lastC;
              // the attribute is now empty
              TtaRegisterAttributeDelete (attr, el, doc);
              TtaRemoveAttribute (el, attr, doc);
              DeleteSpanIfNoAttr (el, doc, &firstC, &lastC);
              selChange = TRUE;
            }
          else
            {
              TtaRegisterAttributeReplace (attr, el, doc);
              TtaSetAttributeText (attr, buffer, el, doc);
            }
          // unapply the CSS property
          old = TtaStrdup(data);
          ParseHTMLSpecificStyle (el, old, doc, 2000, TRUE);
          TtaFreeMemory(old);
          TtaSetDocumentModified (doc);
        }
      TtaFreeMemory (buffer);
    }
  TtaFreeMemory (property);
  return selChange;
}

/*----------------------------------------------------------------------
  RemoveSpecificStyle
  Remove a css property
  ----------------------------------------------------------------------*/
ThotBool RemoveSpecificStyle (Document doc, const char *cssproperty)
{
  Element         el, parent1, parent2;
  ElementType	    elType;
  Attribute       attr;
  AttributeType   attrType;
  int             firstChar, lastChar;
  char           *name;
  DisplayMode     dispMode;
  ThotBool        open = FALSE, selChange = FALSE;

  if (NoCSSEditing (doc))
    /* document is ReadOnly */
    return selChange;

  TtaGiveFirstSelectedElement (doc, &el, &firstChar, &lastChar);
  if (el == NULL)
    return selChange;
  parent1 = TtaGetParent (el);
  attrType.AttrTypeNum = 0;
  if (NoStyleEdit (el, doc))
    {
      /* the selected element is read-only */
      TtaDisplaySimpleMessage (CONFIRM, AMAYA, AM_READONLY);
      return selChange;
    }

  /* Need to force a redisplay */
  dispMode = TtaGetDisplayMode (doc);
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, DeferredDisplay);
  open = !TtaHasUndoSequence (doc);
  if (open)
    TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
  while (el)
    {
      // get the style attribute
      elType = TtaGetElementType (el);
      parent2 = TtaGetParent (el);
      attrType.AttrSSchema = elType.ElSSchema;
      name = TtaGetSSchemaName (elType.ElSSchema);
      if (!strcmp (name, "HTML"))
        attrType.AttrTypeNum =  HTML_ATTR_Style_;
#ifdef _SVG
      else if (!strcmp (name, "SVG"))
        attrType.AttrTypeNum = SVG_ATTR_style_;
#endif /* _SVG */
      else if (!strcmp (name, "MathML"))
        attrType.AttrTypeNum = MathML_ATTR_style_;
      attr = TtaGetAttribute (el, attrType);
      if (attr)
        selChange = CleanUpAttribute (attr, cssproperty, el, doc);
      // next element within the selection
      TtaGiveNextSelectedElement (doc, &el, &firstChar, &lastChar);
    }
  if (open)
    TtaCloseUndoSequence (doc);
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, dispMode);
  return selChange;
}

/*----------------------------------------------------------------------
  DoRemoveColor
  Remove color style
  ----------------------------------------------------------------------*/
void DoRemoveColor (Document doc, View view)
{
  RemoveSpecificStyle (doc, "color: black");
}

/*----------------------------------------------------------------------
  DoRemoveBgColor
  Remove color style
  ----------------------------------------------------------------------*/
void DoRemoveBgColor (Document doc, View view)
{
  RemoveSpecificStyle (doc, "background-color: white");
}

/*----------------------------------------------------------------------
  DoRemoveFont
  Remove the font family and/or the font size of the selection
  ----------------------------------------------------------------------*/
void DoRemoveFont (Document doc, View view)
{
  Element             el = NULL;
  int                 firstChar, lastChar;
  int                 size = -1, family;
  TypeUnit            unit;

  doc = TtaGetSelectedDocument();
  if (NoCSSEditing (doc))
    /* document is ReadOnly */
    return;

  RemoveSpecificStyle (doc, "font-family: Times");
  RemoveSpecificStyle (doc, "font-size: 12pt");
  // update the style panel
  TtaGiveFirstSelectedElement (doc, &el, &firstChar, &lastChar);
  if (el == NULL)
    return;
  if (NoStyleEdit (el, doc))
    return;
  TtaGiveBoxFontInfo (el, doc, 1, &size, &unit, &family);
  if (size != -1 &&
      (size != Current_FontSize || unit != UnPoint  || family != Current_FontFamily))
    {
      Current_FontFamily = family;
      Current_FontSize = size;
      UpdateStylePanel (doc, view);
    }
}

/*----------------------------------------------------------------------
  DoLeftAlign
  Apply left-align style
  ----------------------------------------------------------------------*/
void DoLeftAlign (Document doc, View view)
{
  if (GetEnclosingBlock(doc))
    GenerateStyle ("text-align:left;margin-left:0;margin-right:auto;", FALSE, TRUE);
}

/*----------------------------------------------------------------------
  DoRightAlign
  Apply right-align style
  ----------------------------------------------------------------------*/
void DoRightAlign (Document doc, View view)
{
  if (GetEnclosingBlock(doc))
    {
    GenerateStyle ("text-align:right;margin-left:auto;margin-right:0;", FALSE, TRUE);
    }
}

/*----------------------------------------------------------------------
  DoCenter
  Apply center style
  ----------------------------------------------------------------------*/
void DoCenter (Document doc, View view)
{
  if (GetEnclosingBlock(doc))
    GenerateStyle ("text-align:center;margin-left:auto;margin-right:auto;", FALSE, TRUE);
}

/*----------------------------------------------------------------------
  DoJustify
  Apply justify style
  ----------------------------------------------------------------------*/
void DoJustify (Document doc, View view)
{
  if (GetEnclosingBlock(doc))
    GenerateStyle ("text-align:justify;", TRUE, TRUE);
}


/*----------------------------------------------------------------------
  DoRemoveAlign
  Remove alignment
  ----------------------------------------------------------------------*/
void DoRemoveAlign (Document doc, View view)
{
  RemoveSpecificStyle (doc, "text-align:left");
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void LineSpacingSingle (Document doc, View view)
{
  if (GetEnclosingBlock(doc))
    GenerateStyle ("line-height:1em;", TRUE, TRUE);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void LineSpacingHalf (Document doc, View view)
{
  if (GetEnclosingBlock(doc))
    GenerateStyle ("line-height:1.5em;", TRUE, TRUE);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void LineSpacingDouble (Document doc, View view)
{
  if (GetEnclosingBlock(doc))
    GenerateStyle ("line-height:2em;", TRUE, TRUE);
}

/*----------------------------------------------------------------------
  DoRemoveLineSpacing
  Remove line spacing
  ----------------------------------------------------------------------*/
void DoRemoveLineSpacing (Document doc, View view)
{
  RemoveSpecificStyle (doc, "line-height:1em");
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void MarginLeftIncrease (Document doc, View view)
{
  if (GetEnclosingBlock(doc))
    GenerateStyle ("margin-left:2em;", TRUE, TRUE);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void MarginLeftDecrease (Document doc, View view)
{
  if (GetEnclosingBlock(doc))
    GenerateStyle ("margin-left:0;", TRUE, TRUE);
}

/*----------------------------------------------------------------------
  DoRemove Margin
  Remove margin
  ----------------------------------------------------------------------*/
void DoRemoveMargin (Document doc, View view)
{
  RemoveSpecificStyle (doc, "margin-left:0");
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static CSSInfoPtr GetCSSFromInfo(Document doc, PInfoPtr pInfo)
{
  CSSInfoPtr          css = CSSList;
  PInfoPtr            pI;
  while (css)
    {
      pI = css->infos[doc];
      while (pI)
        {
          if(pI==pInfo)
            return css;
          pI = pI->PiNext;
        }
      css = css->NextCSS;
    }
  return NULL;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void MakeDisableCSS(Document doc, PInfoPtr pInfo)
{
  CSSInfoPtr css;  
    
  /* disable the CSS file, but not remove */
  if (pInfo->PiCategory == CSS_DOCUMENT_STYLE)
    RemoveStyle (NULL, doc, TRUE, FALSE, pInfo->PiLink, pInfo->PiCategory);
  else
    {
      css = GetCSSFromInfo(doc, pInfo);
      if(css)
        {
          if(!css->url)
            RemoveStyle (css->localName, doc, TRUE, FALSE, NULL, pInfo->PiCategory);
          else
            RemoveStyle (css->url, doc, TRUE, FALSE, NULL, pInfo->PiCategory);
        }
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void MakeEnableCSS(Document doc, PInfoPtr pInfo)
{
  CSSInfoPtr css;  
    
  /* enable the CSS file */
  if (pInfo->PiCategory == CSS_DOCUMENT_STYLE)
    EnableStyleElement (doc, pInfo->PiLink);
  else
    {
      css = GetCSSFromInfo(doc, pInfo);
      if(css)
        {
          if(!css->url)
            AddStyle (css->localName, doc, NULL, pInfo->PiCategory);
          else
            AddStyle (css->url, doc, NULL, pInfo->PiCategory);
        }
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void MakeOpenCSS(Document doc, PInfoPtr pInfo)
{
  CSSInfoPtr css;
  css = GetCSSFromInfo(doc, pInfo);
  if(css)
    {
      if (pInfo->PiCategory == CSS_DOCUMENT_STYLE)
        {
          ShowSource (doc, 1);
          return;
        }
      DontReplaceOldDoc = TRUE;
      if(!css->url)
        GetAmayaDoc (css->localName, NULL, doc, doc, CE_CSS, TRUE, NULL, NULL);
      else
        GetAmayaDoc (css->url, NULL, doc, doc, CE_CSS, TRUE, NULL, NULL);
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void MakeRemoveCSS(Document doc, PInfoPtr pInfo)
{
  CSSInfoPtr css;
  Element    el;
  ThotBool   open;
  
  /* remove the link to this file */
  if (pInfo->PiCategory == CSS_DOCUMENT_STYLE)
    DeleteStyleElement (doc, pInfo->PiLink);
  else if (pInfo->PiCategory == CSS_EXTERNAL_STYLE)
    {
      css = GetCSSFromInfo(doc, pInfo);
      
      /* look for the element LINK */
      el = pInfo->PiLink;
      RemoveLink (el, doc);
      /* register this element in the editing history */
      open = !TtaHasUndoSequence (doc);
      if (open)
        TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
      TtaRegisterElementDelete (el, doc);
      TtaDeleteTree (el, doc);
      if (open)
        TtaCloseUndoSequence (doc);
      TtaSetDocumentModified (doc);
    }
}


/*----------------------------------------------------------------------
  Callback procedure for dialogue events.                            
  ----------------------------------------------------------------------*/
static void CallbackCSS (int ref, int typedata, char *data)
{
  CSSInfoPtr      css, cssNext;
  PInfoPtr        pInfo, pInfoNext;
  Element         el, firstSel, lastSel;
  char           *ptr = NULL, *localname = NULL;
  int             j, firstChar, lastChar;
  long int        val;
  int             sty;
  CSSCategory     category;
  ThotBool        found;

  val = (long int) data;
  category = CSS_Unknown;
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
                      sscanf (&ptr[j], "%d", &sty);
                      ptr[j] = EOS;
                    }
                  found = TRUE;
                }
              else
                category = (CSSCategory)((int)category + 1);
            }
          if (category == DisplayCategory_length)
            category = CSS_Unknown;

          switch (CSScase)
            {
            case 1:
              /* display the CSS file */
#ifdef _WX
              DontReplaceOldDoc = TRUE;
              GetAmayaDoc (ptr, NULL, CSSdocument,
                           CSSdocument, CE_CSS, TRUE, NULL, NULL);
#else /* _WX */
              GetAmayaDoc (ptr, NULL, 0, 0, CE_CSS, TRUE, NULL, NULL);
#endif /* _WX */
              break;
            case 2:
              /* disable the CSS file, but not remove */
              if (category == CSS_DOCUMENT_STYLE)
                RemoveStyle (NULL, CSSdocument, TRUE, FALSE, CSSlink[sty], category);
              else
                RemoveStyle (ptr, CSSdocument, TRUE, FALSE, NULL, category);
      	      break;
            case 3:
              /* enable the CSS file */
              if (category == CSS_DOCUMENT_STYLE)
                EnableStyleElement (CSSdocument, CSSlink[sty]);
              else
                AddStyle (ptr, CSSdocument, NULL, category);
      	      break;
            case 4:
              /* remove the link to this file */
              if (category == CSS_DOCUMENT_STYLE)
                DeleteStyleElement (CSSdocument, CSSlink[sty]);
              else if (category == CSS_EXTERNAL_STYLE)
                {
                  css = CSSList;
                  while (css)
                    {
                      cssNext = css->NextCSS;
                      if (css->infos[CSSdocument] &&
                          ((css->url && !strcmp (ptr, css->url)) ||
                           (css->localName && !strcmp (ptr, css->localName))))
                        {
                          /* we found out the CSS */
                          pInfo = css->infos[CSSdocument];
                          while (pInfo)
                            {
                              pInfoNext = pInfo->PiNext;
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
                              pInfo = pInfoNext;
                            }
                        }
                      css = cssNext;
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
    case CSSValue:
      if (data)
        GenerateStyle (data, FALSE, TRUE);
      else
        TtaDestroyDialogue (ref);
      break;
    default:
      break;
    }
}

/*----------------------------------------------------------------------
  GenerateStyleListFileForElem
  Create a file (named fileName) containing all the CSS style rules
  that have been applied to element el.
  -----------------------------------------------------------------------*/
static void GenerateStyleListFileForElem (Element el, Document doc,
                                          char *fileName)
{
  ElementType         elType;
  FILE               *list;
  int                 n;

  /* There is no style on terminal elements. If el is a terminal element,
     take its parent instead.  */
  elType = TtaGetElementType (el);
  if (elType.ElTypeNum == HTML_EL_TEXT_UNIT ||
      elType.ElTypeNum == HTML_EL_GRAPHICS_UNIT ||
      elType.ElTypeNum == HTML_EL_PICTURE_UNIT ||
      elType.ElTypeNum == HTML_EL_SYMBOL_UNIT)
    {
      el = TtaGetParent (el);
      if (el)
        elType = TtaGetElementType (el);	  
    }
  /* If the element is not supposed to be seen by the user, take its
     parent instead */
  while (el && TtaHasHiddenException (elType))
    {
      el = TtaGetParent (el);
      if (el)
        elType = TtaGetElementType (el);
    }
  /* open the file */
  list = TtaWriteOpen (fileName);
  /* generate its contents */
  fprintf (list, "\n\n");
  if (TtaIsXmlSSchema(elType.ElSSchema))
    /* it's an element belonging to an generic XML vocabulary */
    fprintf (list, TtaGetMessage (AMAYA, AM_STYLE_APPLIED),
             TtaGetElementTypeName (elType));
  else
    /* it's an element belonging to a known XML vocabulary */
    fprintf (list, TtaGetMessage (AMAYA, AM_STYLE_APPLIED),
             GetXMLElementName (elType, doc));
  fprintf (list, TtaGetMessage (AMAYA, AM_LINK_LINE));
  if (el)
    n = TtaListStyleOfCurrentElement (doc, list);
  else
    n = 0;
  if (n == 0)
    {
      fprintf (list, "\n     ");
      fprintf (list, TtaGetMessage (AMAYA, AM_NO_STYLE_FOR_ELEM));
    }
  TtaWriteClose (list);
}

/*----------------------------------------------------------------------
  ShowAppliedStyle shows style applied to the current selected
  element.
  -----------------------------------------------------------------------*/
void ShowAppliedStyle (Document doc, View view)
{
  Element             el;
  Document            newdoc;
  char                fileName[100];
  int                 f, i, l;

  for (i = 1; i < DocumentTableLength; i++)
    if (DocumentURLs[i] && DocumentSource[i] == doc &&
        DocumentTypes[i] == docLog &&
        strstr (DocumentURLs[i], "STYLE.LST"))
      {
        /* the style window this document is already shown. Just raise it */
        TtaRaiseView (i, 1);
        return;
      }

  TtaGiveFirstSelectedElement (doc, &el, &f, &l);
  if (el == NULL)
    InitInfo (TtaGetMessage (AMAYA, AM_ERROR),
              TtaGetMessage (AMAYA, AM_NO_SELECTION));
  //else if (NoStyleEdit (el, doc))
  //  return;
  else
    {
      /* list CSS rules applied to the current selection */
      sprintf (fileName, "%s%c%d%cSTYLE.LST",
               TempFileDirectory, DIR_SEP, doc, DIR_SEP);
      if (TtaFileExist (fileName))
        TtaFileUnlink (fileName);
      GenerateStyleListFileForElem (el, doc, fileName);
      newdoc = GetAmayaDoc (fileName, "STYLE.LST", 0, doc, CE_LOG, FALSE,
                            NULL, NULL);
      /* store the relation with the original document */
      if (newdoc)
        {
          DocumentSource[newdoc] = doc;
          TtaSetStatus (newdoc, 1, "   ", NULL);
          /* set the STYLE.LST document in read-only mode */
          TtaSetDocumentAccessMode (newdoc, 0);
        }
    }
}

/*----------------------------------------------------------------------
  SynchronizeAppliedStyle updates the displayed style information.
  -----------------------------------------------------------------------*/
void SynchronizeAppliedStyle (NotifyElement *event)
{
  Element             el;
  Document            doc;
  char                fileName[100], dirName[100];
  int                 i;

  /* look for the log document that displays the STYLE.LST file associated
     with the document */
  doc = event->document;
  el = event->element;
  for (i = 1; i < DocumentTableLength; i++)
    if (DocumentURLs[i] && DocumentSource[i] == doc &&
        DocumentTypes[i] == docLog &&
        strstr (DocumentURLs[i], "STYLE.LST"))
      {
        /* list CSS rules applied to the current selection */
        sprintf (dirName, "%s%c%d",
                 TempFileDirectory, DIR_SEP, doc);
        sprintf (fileName, "%s%c%d%cSTYLE.LST",
                 TempFileDirectory, DIR_SEP, doc, DIR_SEP);
        if (TtaFileExist (fileName))
          TtaFileUnlink (fileName);
        GenerateStyleListFileForElem (el, doc, fileName);
        StartParser (i, fileName, "STYLE.LST", dirName, "STYLE.LST", TRUE, FALSE);
        /* set the STYLE.LST document in read-only mode */
        TtaSetDocumentAccessMode (i, 0);
        return;
      }
}

/*----------------------------------------------------------------------
  InitCSS                                                         
  ----------------------------------------------------------------------*/
void InitCSS (void)
{
  /* initialize the dialogs */
  BaseCSS = TtaSetCallback ((Proc)CallbackCSS, MAX_CSS_REF);
  CSSpath[0] = EOS;
}

/*----------------------------------------------------------------------
  InitCSSDialog list downloaded CSS files
  ----------------------------------------------------------------------*/
static void InitCSSDialog (Document doc, char *s)
{
  CSSInfoPtr          css;
  PInfoPtr            pInfo;
  ElementType	      elType;
  char               *name;
  char               *buf = NULL;
  char               *ptr, *localname;
  int                 i, select;
  int                 len, nb, sty;
  int                 index, size;  

  CSSdocument = doc;
  localname = TtaGetMessage (AMAYA, AM_LOCAL_CSS);
  /* clean up the list of links */
  TtaFreeMemory (CSSlink);
  CSSlink = NULL;
  index = 0;
  nb = 0; /* number of entries */
  sty = 0; /* number of style elements */
  size = 1;
#ifdef _GTK
  /* create the form */
  TtaNewSheet (BaseCSS + CSSForm, TtaGetViewFrame (doc, 1), s, 1,
               TtaGetMessage(LIB, TMSG_LIB_CONFIRM), TRUE, 1, 'L', D_DONE);
#endif /* _GTK */
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
              if (pInfo->PiCategory == CSS_DOCUMENT_STYLE)
                {
                  size += strlen (localname) + 11;
                  /* count the number of style element */ 
                  sty++;
                }
              else if (css->url == NULL && css->localName)
                size += strlen (css->localName) + 12;
              else
                size += strlen (css->url) + 12;
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
      buf = (char *)TtaGetMemory (size);
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
                  pInfo->PiCategory != CSS_USER_STYLE &&
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
                      /* skip HTML style attributes */
                      elType = TtaGetElementType (pInfo->PiLink);
                      name = TtaGetSSchemaName (elType.ElSSchema);
                      if (!strcmp (name, "HTML") && elType.ElTypeNum == HTML_EL_STYLE_)
                        {
                          ptr = (char *)TtaGetMemory (strlen (localname) + 11);
                          sprintf (ptr, "%s%d", localname, sty);
                          CSSlink[sty++] = pInfo->PiLink;
                        }
                      else
                        {
                          /* skip this entry */
                          ptr = NULL;
                          nb--;
                          if (nb == 0)
                            {
                              TtaFreeMemory (CSSlink);
                              CSSlink = NULL;
                            }
                        }
                    }
                  else if (css->url == NULL && css->localName)
                    ptr = css->localName;
                  else
                    ptr = css->url;
                  if (ptr)
                    {
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
                      i++;
                    }
                }
              pInfo = pInfo->PiNext;
            }
          css = css->NextCSS;
        }
      /* add the CSS_USER_SYLE entry */
      css = CSSList;
      while (css)
        {
          pInfo = css->infos[doc];
          while (pInfo)
            {
              if (pInfo &&
                  pInfo->PiCategory == CSS_USER_STYLE &&
                  /* it's impossible to disable an imported style sheet */
                  (CSScase == 1 ||
                   (CSScase == 2 && pInfo->PiEnabled) ||
                   /* it's impossible to enable an imported style sheet */
                   (CSScase == 3 && !pInfo->PiEnabled)))
                {
                  /* filter enabled and disabled entries */
                  /* build the CSS list:
                     use the dialogue encoding for buf and UTF-8 for CSS path  */
                  if (css->url == NULL && css->localName)
                    ptr = css->localName;
                  else
                    ptr = css->url;
                  if (ptr)
                    {
                      len = strlen (ptr) + 1; /* + EOS */
                      if (size < len + String_length)
                        break;
                      /* display the category */
                      strcpy (&buf[index], DisplayCategory[pInfo->PiCategory]);
                      index += String_length;
                      strcpy (&buf[index], ptr);
                      index += len;
                      size -= len;
                      if (select == -1 && CSScase < 4)
                        {
                          strcpy (CSSpath, DisplayCategory[pInfo->PiCategory]);
                          if (css->url)
                            strcat (CSSpath, css->url);
                          else
                            strcat (CSSpath, css->localName);
                          select = i;
                        }
                      i++;
                    }
                }
              pInfo = pInfo->PiNext;
            }
          css = css->NextCSS;
        }
    }

  /* display the form */
#ifdef _WX
  ThotBool created = CreateCSSDlgWX (BaseCSS + CSSForm, BaseCSS + CSSForm + 1,
                                     TtaGetViewFrame (doc, 1),
                                     s, nb, buf);
  TtaFreeMemory (buf);
  if (created)
    TtaShowDialogue (BaseCSS + CSSForm, TRUE, TRUE);
#endif /* _WX */

#ifdef _WINGUI
  CreateCSSDlgWindow (TtaGetViewFrame (doc, 1), nb, buf, s,
                      TtaGetMessage (AMAYA, AM_NO_CSS));
  TtaFreeMemory (buf);
#endif /* _WINGUI */

#ifdef _GTK
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
                 TtaGetMessage (AMAYA, AM_NO_CSS));
  TtaFreeMemory (buf);
  TtaShowDialogue (BaseCSS + CSSForm, TRUE, TRUE);
  if (nb > 0)
    TtaSetSelector (BaseCSS + CSSSelect, select, NULL);
#endif /* _GTK */
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
void OpenCSS (Document doc, View view)
{
  CSScase = 1;
  InitCSSDialog (doc, TtaGetMessage (AMAYA, AM_OPEN_CSS));
}

/*----------------------------------------------------------------------
  DisableCSS list downloaded CSS files
  ----------------------------------------------------------------------*/
void DisableCSS (Document doc, View view)
{
  CSScase = 2;
  InitCSSDialog (doc, TtaGetMessage (AMAYA, AM_DISABLE_CSS));
}

/*----------------------------------------------------------------------
  EnableCSS list downloaded CSS files
  ----------------------------------------------------------------------*/
void EnableCSS (Document doc, View view)
{
  CSScase = 3;
  InitCSSDialog (doc, TtaGetMessage (AMAYA, AM_ENABLE_CSS));
}

/*----------------------------------------------------------------------
  RemoveCSS lists downloaded CSS files
  ----------------------------------------------------------------------*/
void RemoveCSS (Document doc, View view)
{
  CSScase = 4;
  InitCSSDialog (doc, TtaGetMessage (AMAYA, AM_REMOVE_CSS));
}

/*----------------------------------------------------------------------
  SetCSSStyle   
  ----------------------------------------------------------------------*/
void SetCSSStyle (Document doc, View view)
{
#ifdef _WX
  ThotBool created;

  created = CreateStyleDlgWX (BaseCSS + CSSValue, TtaGetViewFrame (doc, view));
  if (created)
    TtaShowDialogue (BaseCSS + CSSValue, FALSE, TRUE);
#endif /* _WX */
}
