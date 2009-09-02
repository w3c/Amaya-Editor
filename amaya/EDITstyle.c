/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *
 * Authors: I. Vatton
 *
 */

#ifdef _WX
#include "wx/wx.h"
#endif /* _WX */
#include "constmenu.h"

#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"
#include "undo.h"
#include "MathML.h"
#ifdef _SVG
#include "SVG.h"
#endif
#include "fetchHTMLname.h"
#include "MENUconf.h"

#include "AHTURLTools_f.h"
#include "EDITimage_f.h"
#include "EDITORactions_f.h"
#include "HTMLactions_f.h"
#include "HTMLedit_f.h"
#include "HTMLhistory_f.h"
#include "HTMLimage_f.h"
#include "HTMLpresentation_f.h"
#include "HTMLsave_f.h"
#include "UIcss_f.h"
#include "css_f.h"
#include "fetchHTMLname_f.h"
#include "fetchXMLname_f.h"
#include "html2thot_f.h"
#include "init_f.h"
#include "styleparser_f.h"
#ifdef _WINGUI
#include "wininclude.h"
#else /* _WINGUI */
#include "appdialogue_wx.h"
#endif /* _WINGUI */

#ifdef _WX
#include "wxdialogapi_f.h"
#include "paneltypes_wx.h"
#endif /* _WX */


static char         ListBuffer[MAX_CSS_LENGTH];
static int          NbClass = 0;
static char         CurrentClass[200];
static Element      ClassReference;
static Document     DocReference;
static Document	    ApplyClassDoc;
static ThotBool     OldBuffer = FALSE;


/*----------------------------------------------------------------------
  UpdateStyleList
  ----------------------------------------------------------------------*/
void UpdateStyleList (Document doc, View view)
{
#ifdef _WX
  AmayaParams p;

  p.param1 = doc;
  TtaSendDataToPanel( WXAMAYA_PANEL_STYLE_LIST, p );
#endif /* _WX */
}

/*----------------------------------------------------------------------
  RemoveElementStyle cleans all the presentation rules of a given element.
  The parameter removeSpan is True when the span has to be removed.
  ----------------------------------------------------------------------*/
static void RemoveElementStyle (Element el, Document doc, ThotBool removeSpan)
{
  ElementType		elType;
  Attribute            attr;
  AttributeType        attrType;
  Element		firstChild, lastChild;
  char                *name;

  if (el == NULL)
    return;
  elType = TtaGetElementType (el);
  /* if it's a MathML element, remove the style attribute defined in the
     MathML DTD */
  name = TtaGetSSchemaName (elType.ElSSchema);
  if (!strcmp (name, "MathML"))
    {
      attrType.AttrSSchema = elType.ElSSchema;
      attrType.AttrTypeNum = MathML_ATTR_style_;
    }
#ifdef _SVG
  else if (!strcmp (name, "SVG"))
    /* if it's a SVG element, remove the style attribute defined in the
       SVG DTD */
      {
        attrType.AttrSSchema = elType.ElSSchema;
        attrType.AttrTypeNum = SVG_ATTR_style_;
      }
#endif
    else
      /* remove the style attribute defined in the HTML DTD */
      {
        attrType.AttrSSchema = TtaGetSSchema ("HTML", doc);
        attrType.AttrTypeNum = HTML_ATTR_Style_;
      }
  attr = TtaGetAttribute (el, attrType);
  if (attr != NULL)
    {
      TtaRegisterAttributeDelete (attr, el, doc);
      TtaRemoveAttribute (el, attr, doc);
      if (removeSpan)
        DeleteSpanIfNoAttr (el, doc, &firstChild, &lastChild);
      TtaSetDocumentModified (doc);
    }

  /* remove all the specific presentation rules applied to the element */
  TtaCleanElementPresentation (el, doc);
  UpdateStyleList (doc, 1);
}


/*----------------------------------------------------------------------
  UpdateCSSImport searches strings @import within the styleString and
  makes it relative to the newpath.
  oldpath = old document path
  newpath = new document path
  Returns NULL or a new allocated string.
  styleString returns the last managed position
  ----------------------------------------------------------------------*/
static char *UpdateCSSImport (const char *oldpath, const char *newpath,
                                char **styleString)
{
  char               *b, *e = NULL, *newString, *oldptr, *sString;
  char                old_url[MAX_LENGTH];
  char                tempname[MAX_LENGTH];
  char                cssname[MAX_LENGTH];
  char               *new_url;
  int                 len, newlen;

  newString = NULL;
  sString = *styleString;
  b = strstr (sString, "@import");
  while (b)
    {
      /* we need to compare this url with the new doc path */
      b += 7;
      b = SkipBlanksAndComments (b);
      /*** Caution: Strings can either be written with double quotes or
           with single quotes. Only double quotes are handled here.
           Escaped quotes are not handled. See function SkipQuotedString */
      if (!strncmp (b, "url", 3))
        {
          b += 3;
          b = SkipBlanksAndComments (b);
          if (*b == '(')
            b++;
        }
      if (*b == '"')
        {
          /* search the url end */
          b++;
          e = b;
          while (*e != EOS && *e != '"')
            e++;
        }
      else if (*b == '\'')
        {
          /* search the url end */
          b++;
          e = b;
          while (*e != EOS && *e != '\'')
            e++;
        }

      if (*e != EOS)
        {
          len = (int)(e - b);
          strncpy (old_url, b, len);
          old_url[len] = EOS;
          /* get the old full css name */
          NormalizeURL (old_url, 0, tempname, cssname, oldpath);
          /* build the new full css name */
          new_url = MakeRelativeURL (tempname, newpath);
	        newlen = strlen (new_url);
          /* generate the new style string */
          if (newlen > len)
            {
              // a memory allocation is necessary
              if (newString)
                oldptr = newString;
              else
                oldptr = *styleString;

              len = - len + strlen (oldptr) + newlen + 1;
              newString = (char *)TtaGetMemory (len);	  
              len = (int)(b - oldptr);
              strncpy (newString, oldptr, len);
              sString = &newString[len];
              /* new name */
              strcpy (sString, new_url);
              /* following text */
              strcat (sString, e);
              TtaFreeMemory (oldptr);
            }
          else
            {
              // there is enough space
              strncpy (b, new_url, newlen);
              sString = &b[newlen];
              if (newlen < len)
                // reduce the size of the full string
                strcpy (sString, e);
            }
          TtaFreeMemory (new_url);
        }
      else
        sString = b;

      /* next background-image */
      b = strstr (sString, "@import"); 
    }
  // return the last parsed position
  *styleString = sString;
  return (newString);
}

/*----------------------------------------------------------------------
  UpdateCSSURLs searches url() or url("") within the styleString
  and makes these urls relative to the newpath.
  oldpath = the old CSS path
  newpath = the new CSS path
  imgpath = new relative path for images
  imgSave is TRUE if images must be saved
  localimport is TRUE if imported files become local to the CSS
  A new image url is obtained by concatenation of imgpath and the image name.
  Returns NULL or a new allocated styleString.
  ----------------------------------------------------------------------*/
char *UpdateCSSURLs (Document doc, char *oldpath, char *newpath,
                     const char *imgpath, char *styleString,
                     ThotBool imgSave, ThotBool localimport)
{
  LoadedImageDesc    *pImage;
  char               *b, *e, *newString, *oldptr, *sString;
  char                old_url[MAX_LENGTH];
  char                new_url[MAX_LENGTH];
  char                imgname[MAX_LENGTH];
  char                oldname[MAX_LENGTH];
  char                newname[MAX_LENGTH];
  int                 len, newlen;
  ThotBool            src_is_local, dst_is_local;

  sString = styleString;
  if (localimport)
    newString = UpdateCSSImport ("", "", &sString);
  else
    newString = UpdateCSSImport (oldpath, newpath, &sString);
  b = strstr (sString, "url");
  while (b)
    {
      /* we need to compare this url with the new doc path */
      b += 3;
      b = SkipBlanksAndComments (b);
      if (*b == '(')
        {
          b++;
          b = SkipBlanksAndComments (b);
          /*** Caution: Strings can either be written with double quotes or
               with single quotes. Only double quotes are handled here.
               Escaped quotes are not handled. See function SkipQuotedString */
          if (*b == '"')
            {
              /* search the url end */
              b++;
              e = b;
              while (*e != EOS && *e != '"')
                e++;
            }
          else if (*b == '\'')
            {
              /* search the url end */
              b++;
              e = b;
              while (*e != EOS && *e != '\'')
                e++;
            }
          else
            {
              /* search the url end */
              e = b;
              while (*e != EOS && *e != ')')
                e++;
            }

          if (*e != EOS)
            {
              len = (int)(e - b);
              strncpy (oldname, b, len);
              oldname[len] = EOS;
              // get the old full URL and the name of the image
              NormalizeURL (oldname, 0, old_url, imgname, oldpath);
              /* build the new full image name */
              if (imgpath)
                strcpy (newname, imgpath);
              else
                newname[0] = EOS;
              strcat (newname, imgname);
              newlen = strlen (newname);
              if (newlen != len || strcmp (oldname, newname))
                {
                  /* generate the new style string */
                  if (newlen > len)
                    {
                      // a memory allocation is necessary
                      if (newString)
                        oldptr = newString;
                      else
                        oldptr = styleString;
 
                      len = - len + strlen (oldptr) + newlen + 1;
                      newString = (char *)TtaGetMemory (len);	  
                      len = (int)(b - oldptr);
                      strncpy (newString, oldptr, len);
                      sString = &newString[len];
                      /* new name */
                      strcpy (sString, newname);
                      /* following text */
                      strcat (sString, e);
                      TtaFreeMemory (oldptr);
                    }
                  else
                    {
                      // there is enough space
                      strncpy (b, newname, newlen);
                      sString = &b[newlen];
                      if (newlen < len)
                        // reduce the size of the full string
                        strcpy (sString, e);
                      // note that a change is done
                      if (newString == NULL)
                        newString = styleString;
                    }
                  if (imgSave && newname[0] != EOS && old_url[0] != EOS)
                    {
                      // get the new full URL of the image
                      NormalizeURL (newname, 0, new_url, imgname, newpath);
                      src_is_local = !IsW3Path (old_url);
                      dst_is_local = !IsW3Path (new_url);
#ifdef AMAYA_DEBUG
  fprintf(stderr, "Move image: from %s to %s\n", old_url, new_url);
#endif
                      /* mark the image descriptor or copy the file */
                      if (dst_is_local)
                        {
                          /* copy the file to the new location */
                          if (src_is_local)
                            TtaFileCopy (old_url, new_url);
                          else
                            {
                              /* it was a remote image:
                                 get the image descriptor to prepare the saving process */
                              pImage = SearchLoadedImageByURL (doc, old_url);
                              if (pImage && pImage->tempfile)
                                TtaFileCopy (pImage->tempfile, new_url);
                            }
                        }
                      else
                        {
                          /* save to a remote server */
                          if (src_is_local)
                            /* add the existing localfile to images list to be saved */
                            AddLocalResource (old_url, imgname, new_url, doc,
                                              &pImage, &ImageURLs, FALSE);
                          else
                            {
                              /* it was a remote image:
                                 get the image descriptor to prepare the saving process */
                              pImage = SearchLoadedImageByURL (doc, old_url);
                              /* update the descriptor */
                              if (pImage)
                                {
                                  /* image was already loaded */
                                  if (pImage->originalName)
                                    TtaFreeMemory (pImage->originalName);
                                  pImage->originalName = TtaStrdup (new_url);
                                  if (TtaFileExist(pImage->localName))
                                    pImage->status = RESOURCE_MODIFIED;
                                  else
                                    pImage->status = RESOURCE_NOT_LOADED;
                                }
                            }
                        }
                    }
                }
              else
                // no change: just skip this url
                sString = &b[newlen];
            }
          else
            sString = b;
        }
      else
        sString = b;
      /* next background-image */
      b = strstr (sString, "url"); 
    }
  return (newString);
}

/*----------------------------------------------------------------------
  UpdateStyleSheetContent opens a style sheet file to update included
  @import and images to make them local to the stylesheet
  cssfile = the css file to be updated
  oldpath = old stylesheet path
  newpath = new stylesheet path
  ----------------------------------------------------------------------*/
void UpdateStyleSheetContent (Document doc, char *cssfile, char *oldpath,
                              char *newpath)
{
  gzFile              file;
  BinFile             wfile;
  char               *sStyle, *styleString, *ptr;
  int                 lenBuff = 0;
  int                 len;
#define	              COPY_BUFFER_SIZE	1024
  char                bufferRead[COPY_BUFFER_SIZE + 1];
  ThotBool            endOfFile = FALSE;

  if (oldpath == NULL || newpath == NULL)
    return;
  /* load the file in memory */
  file = TtaGZOpen (cssfile);
  if (file == NULL)
    return;

  // allocate the memory to load the stylesheet
  while (!endOfFile)
    {
      len = gzread (file, bufferRead, COPY_BUFFER_SIZE);
      if (len < COPY_BUFFER_SIZE)
        endOfFile = TRUE;
      lenBuff += len;
    }
  len = 0;
  TtaGZClose (file);
  if (lenBuff > 0)
    {
      styleString = (char *)TtaGetMemory (lenBuff + 1);
      if (styleString == NULL)
        return;

      file = TtaGZOpen (cssfile);
      len = gzread (file, styleString, lenBuff);
      styleString[lenBuff] = EOS;
      TtaGZClose (file);
      // Now update @import and images
      // imported css and images are now local to the stylesheet
      sStyle = UpdateCSSURLs (doc, oldpath, newpath, "", styleString, TRUE, TRUE);
      if (sStyle)
        {
          // Write the new css content
          wfile = TtaWriteOpen (cssfile);
          ptr = sStyle;
          while (lenBuff > COPY_BUFFER_SIZE)
            {
              fwrite (ptr, COPY_BUFFER_SIZE, 1, wfile);
              lenBuff -= COPY_BUFFER_SIZE;
              ptr += COPY_BUFFER_SIZE;
            }
          if (lenBuff > 0)
            fwrite (ptr, lenBuff, 1, wfile);
          TtaWriteClose (wfile);
          TtaFreeMemory (sStyle);
        }
      else
        TtaFreeMemory (styleString);
      return;
    }
}

/*----------------------------------------------------------------------
  UpdateStyleDelete: a style attribute will be deleted.            
  remove the existing style presentation.                      
  ----------------------------------------------------------------------*/
ThotBool UpdateStyleDelete (NotifyAttribute * event)
{
  ElementType         elType;
  Element             el;
  char               *style = NULL;
  int                 len;

  len = TtaGetTextAttributeLength (event->attribute);
  if ((len < 0) || (len > 10000))
    /* error */
    return FALSE;
  if (len == 0)
    /* empty Style attribute */
    return FALSE;
  else
    {
      /* parse the content of the style attribute and remove the corresponding
         presentation rules from the element */
      style = (char *)TtaGetMemory (len + 2);
      if (!style)
        return FALSE;
      TtaGiveTextAttributeValue (event->attribute, style, &len);
      style[len] = EOS;
      el = event->element;
      ParseHTMLSpecificStyle (el, style, event->document, 1000, TRUE);
      /* if it's the body element and the style attribute contains some
         background properties, the corresponding presentation rules have
         to be removed to the (parent) <html> element */
      elType = TtaGetElementType (el);
      if (elType.ElTypeNum == HTML_EL_BODY &&
          strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
        {
          el = TtaGetParent (el);
          ParseHTMLSpecificStyle (el, style, event->document, 1000, TRUE);
        }
      TtaFreeMemory (style);
    }
  return FALSE;  /* let Thot perform normal operation */
}


/*----------------------------------------------------------------------
  DeleteStyle
  the STYLE element will be deleted in the document HEAD.
  ----------------------------------------------------------------------*/
ThotBool DeleteStyle (NotifyElement *event)
{
  RemoveStyle (NULL, event->document, TRUE, TRUE, event->element, CSS_DOCUMENT_STYLE);
  return FALSE;  /* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
  GetCurrentStyleAttribute
  Return the current style attribute
  -----------------------------------------------------------------------*/
Attribute GetCurrentStyleAttribute ()
{
  Document        doc;
  Element         el;
  ElementType	    elType;
  AttributeType   attrType;
  Attribute       attr = NULL;
  char           *name;
  int             first, last;

  doc = TtaGetSelectedDocument();
  if (doc)
    {
      TtaGiveFirstSelectedElement (doc, &el, &first, &last);
      elType = TtaGetElementType (el);
      attrType.AttrSSchema = elType.ElSSchema;
      attrType.AttrTypeNum = 0;
      name = TtaGetSSchemaName (elType.ElSSchema);
      //if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
      //  el = TtaGetParent (el);
      if (!strcmp(name, "MathML"))
        attrType.AttrTypeNum = MathML_ATTR_style_;
#ifdef _SVG
      else if (!strcmp(name, "SVG"))
        attrType.AttrTypeNum = SVG_ATTR_style_;
#endif /* _SVG */
      else if (!strcmp(name, "HTML"))
        attrType.AttrTypeNum = HTML_ATTR_Style_;

      if (attrType.AttrTypeNum)
        // look for the style attribute
        attr = TtaGetAttribute (el, attrType);
    }
  return attr;
}

/*----------------------------------------------------------------------
  GetCurrentStyle
  Get the current set of CSS properties applied to the current selection.
  The returned buffer must be freed by the calling function.
  -----------------------------------------------------------------------*/
char *GetCurrentStyle ()
{
  Attribute       attr;
  char           *buffer = NULL;
  int             len;

  // look for the style attribute
  attr = GetCurrentStyleAttribute ();
  if (attr)
    {
      len = TtaGetTextAttributeLength (attr) + 1;
      buffer = (char *)TtaGetMemory (len);
      TtaGiveTextAttributeValue (attr, buffer, &len);
    }
  return buffer;
}


/*----------------------------------------------------------------------
  EnableStyleElement
  the STYLE element must be reparsed.
  ----------------------------------------------------------------------*/
void EnableStyleElement (Document doc, Element el)
{
  ElementType	      elType;
  CSSInfoPtr          css = NULL;
  PInfoPtr            pInfo;
  DisplayMode         dispMode;
  char               *name;
  char               *buffer;
  ThotBool            loadcss;

  /* check if we have to load CSS */
  TtaGetEnvBoolean ("LOAD_CSS", &loadcss);
  if (!loadcss)
    return;

  if (el)
    {
      elType = TtaGetElementType (el);
      name = TtaGetSSchemaName (elType.ElSSchema);
      if ((!strcmp (name, "HTML") && elType.ElTypeNum == HTML_EL_STYLE_)
#ifdef _SVG
          /* if it's a SVG document, remove the style defined in the SVG DTD */
          || (!strcmp (name, "SVG") && elType.ElTypeNum == SVG_EL_style__)
#endif
          )
        {
          /* get the style element in the document head */
          buffer = GetStyleContents (el);
          if (buffer)
            {
              css = SearchCSS (doc, NULL, el, &pInfo);
              if (pInfo)
                pInfo->PiEnabled = TRUE;
              dispMode = TtaGetDisplayMode (doc);
              if (dispMode != NoComputedDisplay)
                TtaSetDisplayMode (doc, NoComputedDisplay);
              ReadCSSRules (doc, NULL, buffer, NULL, TtaGetElementLineNumber (el),
                            FALSE, el);
              /* Restore the display mode */
              if (dispMode != NoComputedDisplay)
                TtaSetDisplayMode (doc, dispMode);
              TtaFreeMemory (buffer);
            }
        }
    }
}

/*----------------------------------------------------------------------
  DeleteStyleElement
  the STYLE element will be deleted in the document HEAD.
  ----------------------------------------------------------------------*/
void DeleteStyleElement (Document doc, Element el)
{
  ElementType		elType;
  char                 *name;

  if (el && !TtaIsReadOnly (el))
    {
      /* get the style element in the document head */
      elType = TtaGetElementType (el);
      name = TtaGetSSchemaName (elType.ElSSchema);
      if ((!strcmp (name, "HTML") && elType.ElTypeNum == HTML_EL_STYLE_)
#ifdef _SVG
          /* if it's a SVG document, remove the style defined in the SVG DTD */
          || (!strcmp (name, "SVG") && elType.ElTypeNum == SVG_EL_style__)
#endif
          )
        {
          RemoveStyle (NULL, doc, TRUE, TRUE, el, CSS_DOCUMENT_STYLE);
          TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
          TtaRegisterElementDelete (el, doc);
          TtaDeleteTree (el, doc);
          TtaCloseUndoSequence (doc);
        }
    }
}


/*----------------------------------------------------------------------
  ApplyStyleChange
  A STYLE element has been changed in the HEAD
  OldBuffer says if there is a previous contents
  buffer give the new contents
  ----------------------------------------------------------------------*/
static void ApplyStyleChange (Element el, Document doc)
{
  DisplayMode         dispMode;

  dispMode = TtaGetDisplayMode (doc);
  TtaSetDisplayMode (doc, NoComputedDisplay);
  RemoveParsingErrors (doc);
  // close attached log documents
  CloseLogs (doc);

  if (OldBuffer)
    {
      // remove previous style rules
      RemoveStyle (NULL, doc, TRUE, FALSE, el, CSS_DOCUMENT_STYLE);
      OldBuffer = FALSE;
    }
  /* Apply new style rules */
  EnableStyleElement (doc, el);
  TtaSetDisplayMode (doc, dispMode);
  TtaSetDocumentModified (doc);
  UpdateStyleList (doc, 1);
}

/*----------------------------------------------------------------------
  ChangeStyle
  the STYLE element will be changed in the document HEAD.
  ----------------------------------------------------------------------*/
ThotBool ChangeStyle (NotifyOnTarget * event)
{
  ThotBool            loadcss;

  TtaGetEnvBoolean ("LOAD_CSS", &loadcss);
  if (loadcss && event->element)
    OldBuffer = TRUE;
  return FALSE;  /* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
  StyleChanged
  A STYLE element has been changed in the HEAD
  ----------------------------------------------------------------------*/
void StyleChanged (NotifyOnTarget *event)
{
  ThotBool            loadcss;

  TtaGetEnvBoolean ("LOAD_CSS", &loadcss);
  if (loadcss && event->element)
    ApplyStyleChange (event->element, event->document);
  // check if an error is found in the new string
  CheckParsingErrors (event->document);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void ChangeTheme (const char *theme)
{
  Element             root, el, head, content, next;
  Element             el_select, el_show;
  ElementType	        elType;
  Attribute           attr;
  AttributeType       attrType;
  Document            doc;
  Language            language = TtaGetDefaultLanguage ();
  char               *buffer = NULL, *ptr, *filename;
  int                 view, i, j, len;
  int		              position, distance;
  DisplayMode         dispMode;
  ThotBool            isNew = FALSE, found = FALSE;

  TtaGetActiveView (&doc, &view);
  if (doc == 0 || theme == NULL)
    return;
  if (doc != TtaGetSelectedDocument () && DocumentTypes[doc] == docSource)
    doc = GetDocFromSource (doc);
  if (doc && DocumentTypes[doc] == docHTML)
    {
      root = TtaGetMainRoot (doc);

      /* get the current position in the document */
      TtaGiveFirstSelectedElement (doc, &el_select, &i, &j);
      position = RelativePosition (doc, &distance);
      el_show = ElementAtPosition (doc, position);
      
      elType = TtaGetElementType (root);
      attrType.AttrSSchema = elType.ElSSchema;
      elType.ElTypeNum = HTML_EL_HEAD;
      head = TtaSearchTypedElement (elType, SearchForward, root);
      elType.ElTypeNum = HTML_EL_STYLE_;
      el = TtaSearchTypedElement (elType, SearchForward, head);
      while (!found && el)
        {
          // check if it's the right style element
          attrType.AttrTypeNum = HTML_ATTR_Title;
          attr = TtaGetAttribute (el, attrType);
          if (attr)
            {
              len = TtaGetTextAttributeLength (attr) + 1;
              buffer = (char *)TtaGetMemory (len);
              TtaGiveTextAttributeValue (attr, buffer, &len);
              if (!strcmp (buffer, "Amaya theme"))
                found = TRUE;
              else
                el = TtaSearchTypedElementInTree (elType, SearchForward, head, el);
              TtaFreeMemory (buffer);
              buffer = NULL;
            }
          else
            el = TtaSearchTypedElementInTree (elType, SearchForward, head, el);
        }

      if (el == NULL)
        {
          if (!strcmp (theme, "Standard"))
            // nothing to do
            return;

          TtaSetStructureChecking (FALSE, doc);
          el = InsertWithinHead (doc, 1, HTML_EL_STYLE_);
          TtaSetStructureChecking (TRUE, doc);
          isNew = TRUE;
          if (el)
            {
              TtaExtendUndoSequence (doc);
              attrType.AttrTypeNum = HTML_ATTR_Notation;
              attr = TtaNewAttribute (attrType);
              TtaAttachAttribute (el, attr, doc);
              TtaSetAttributeText (attr, "text/css", el, doc);
              attrType.AttrTypeNum = HTML_ATTR_Title;
              attr = TtaNewAttribute (attrType);
              TtaAttachAttribute (el, attr, doc);
              TtaSetAttributeText (attr, "Amaya theme", el, doc);
            }
        }
      else
        TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);

      if (el)
        {
          content = GetNoTemplateChild (el, TRUE);
          if (content && TtaIsReadOnly (content))
            {
              // cannot change the content type
              TtaCloseUndoSequence (doc);
              TtaDisplaySimpleMessage (CONFIRM, AMAYA, AM_READONLY);
              content = NULL;
            }
          if (content)
            {
              dispMode = TtaGetDisplayMode (doc);
              RemoveParsingErrors (doc);
              // close attached log documents
              CloseLogs (doc);
              TtaSetDisplayMode (doc, NoComputedDisplay);
              // remove the current content
              if (!isNew && content && !TtaIsReadOnly (content))
                RemoveStyle (NULL, doc, TRUE, FALSE, el, CSS_DOCUMENT_STYLE);
              while (content)
                {
                  next = content;
                  TtaNextSibling (&next);
                  TtaRegisterElementDelete(content, doc);
                  TtaDeleteTree (content, doc);
                  content = next;
                }
              if (strcmp (theme, "Standard"))
                {
                  // look for the css file
                  ptr = TtaGetEnvString ("APP_HOME");
                  filename = (char *)TtaGetMemory (strlen (ptr) + strlen(theme) + 20);
                  sprintf (filename, "%s%c%s.css", ptr, DIR_SEP, theme);
                  if (!TtaFileExist (filename))
                    {
                      // the user didn't define CSS files
                      TtaFreeMemory (filename);
                      ptr = TtaGetEnvString ("THOTDIR");
                      filename = (char *)TtaGetMemory (strlen (ptr) + strlen(theme) + 20);
                      sprintf (filename, "%s%cconfig%c%s.css", ptr, DIR_SEP, DIR_SEP, theme);
                    }
                  buffer = LoadACSSFile (filename);
                  if (buffer)
                    {
                      // insert the new content
                      elType.ElTypeNum = HTML_EL_TEXT_UNIT;
                      content = TtaNewElement (doc, elType);
                      TtaSetTextContent (content, (unsigned char*)buffer, language, doc);
                      TtaInsertFirstChild (&content, el, doc);
                      TtaRegisterElementCreate (content, doc);
                    }
                  TtaFreeMemory (filename);
                  TtaFreeMemory (buffer);
                  EnableStyleElement (doc, el);
                }
              else
                {
                  TtaRegisterElementDelete(el, doc);
                  TtaDeleteTree (el, doc);
                }

              // check if an error is found in the new string
              TtaCloseUndoSequence (doc);
              TtaSetDisplayMode (doc, dispMode);

              /* restore the current position in the document */
              TtaShowElement (doc, 1, el_show, distance);
              if (el_select)
                {
                  if (j == 0 && i == 0)
                    TtaSelectElement (doc, el_select);
                  else
                    TtaSelectString (doc, el_select, i, j);
                }
              TtaSetDocumentModified (doc);
            }
        }
    }
#ifdef _WX
  TtaRedirectFocus();
  UpdateStyleList (doc, 1);
#endif /* _WX */
}


/*----------------------------------------------------------------------
  DoClassicTheme
  Apply color style
  ----------------------------------------------------------------------*/
void DoClassicTheme (Document doc, View view)
{
  ChangeTheme ("Classic");
}

/*----------------------------------------------------------------------
  DoModernTheme
  Apply color style
  ----------------------------------------------------------------------*/
void DoModernTheme (Document doc, View view)
{
  ChangeTheme ("Modern");
}

/*----------------------------------------------------------------------
  DoNoTheme
  Remove color style
  ----------------------------------------------------------------------*/
void DoNoTheme (Document doc, View view)
{
  ChangeTheme ("Standard");
}

/*----------------------------------------------------------------------
  UpdateStylePost : attribute Style has been updated or created.  
  reflect the new style presentation.                          
  ----------------------------------------------------------------------*/
void UpdateStylePost (NotifyAttribute * event)
{
  Document            doc;
  Element             el, oldParent, newParent;
  char               *style = NULL;
  int                 len;

  el = event->element;
  doc = event->document;

  len = TtaGetTextAttributeLength (event->attribute);
  if ((len < 0) || (len > 10000))
    return;
  if (len > 0)
    {
      /* parse and apply the new style content */
      style = (char *)TtaGetMemory (len + 2);
      if (style == NULL)
        return;
      TtaGiveTextAttributeValue (event->attribute, style, &len);
      style[len] = EOS;
      /* create a Span element if it's a TEXT leaf */
      oldParent = TtaGetParent (el);
      AttrToSpan (el, event->attribute, doc);
      newParent = TtaGetParent (el);
      if (newParent != oldParent)
        {
          /* a new SPAN element has been created. Generate the PRules
             for the SPAN element */
          el = newParent;
          TtaSetElementLineNumber (el, TtaGetElementLineNumber (oldParent));
        }
      ParseHTMLSpecificStyle (el, style, doc, 1000, FALSE);
      if (CSSErrorsFound)
        {
          /* the CSS parser detected an error */
          TtaWriteClose (ErrFile);
          ErrFile = NULL;
          UpdateLogFile (doc, TRUE);
          CSSErrorsFound = FALSE;
          InitInfo ("", TtaGetMessage (AMAYA, AM_CSS_ERROR));
        }
      else
        UpdateLogFile (doc, FALSE);
      TtaFreeMemory (style);
    }
}

/*----------------------------------------------------------------------
  DoApplyClass
  Put a class attribute on all selected elements
  ----------------------------------------------------------------------*/
static void DoApplyClass (Document doc)
{
  Element             firstSelectedEl, lastSelectedEl, el;
  Element             next, firstChild, lastChild;
  ElementType	        elType, lastType;
  Attribute           attr;
  AttributeType       attrType;
  char               *a_class = CurrentClass, *s;
  int		              firstSelectedChar, lastSelectedChar, i, lg;
  DisplayMode         dispMode;

  if (!a_class)
    return;

  /* remove any leading dot in a class definition. */
  if (*a_class == '.')
    a_class++;
  if (*a_class == EOS)
    return;

  /* work with the current selected document */
  doc = TtaGetSelectedDocument ();
  if (doc == 0)
    {
      TtaDisplaySimpleMessage (CONFIRM, AMAYA, AM_NO_INSERT_POINT);
      return;
    }
  if (DocumentTypes[doc] == docSource || DocumentTypes[doc] == docText ||
      DocumentTypes[doc] == docCSS)
    return;
  TtaGiveFirstSelectedElement (doc, &firstSelectedEl, &firstSelectedChar, &i);
  if (TtaIsReadOnly (firstSelectedEl))
    {
      /* the selected element is read-only */
      TtaDisplaySimpleMessage (CONFIRM, AMAYA, AM_READONLY);
      return;
    }

  if (strcmp (CurrentClass, "(no_class)") &&
      !IsImplicitClassName (CurrentClass, doc))
    GenerateInlineElement (HTML_EL_Span, NULL, HTML_ATTR_Class, a_class, FALSE);
  else
    {
      /* remove class attributes */
      TtaGiveLastSelectedElement (doc, &lastSelectedEl, &i, &lastSelectedChar);
      elType = TtaGetElementType (firstSelectedEl);
      lastType = TtaGetElementType (lastSelectedEl);
      if (elType.ElTypeNum == HTML_EL_TEXT_UNIT && firstSelectedChar > 1)
        // no class attribute
        return;
      lg = TtaGetElementVolume (lastSelectedEl);
      if (lastType.ElTypeNum == HTML_EL_TEXT_UNIT &&
          (lastSelectedChar == 0 || lastSelectedChar > lg))
        return;
      if (TtaIsLeaf (elType))
        firstSelectedEl = TtaGetParent (firstSelectedEl);
       if (TtaIsLeaf (lastType))
        lastSelectedEl = TtaGetParent (lastSelectedEl);

      TtaClearViewSelections ();
      /* stop displaying changes that will be made in the document */
      dispMode = TtaGetDisplayMode (doc);
      if (dispMode == DisplayImmediately)
        TtaSetDisplayMode (doc, DeferredDisplay);
      TtaUnselect (doc);
      TtaOpenUndoSequence (doc, firstSelectedEl, lastSelectedEl, 0, 0);

      /* process all selected elements */
      el = firstSelectedEl;
      while (el)
        {
          /* The current element may be deleted by DeleteSpanIfNoAttr. So, get
             first the next element to be processed */
          if (el == lastSelectedEl)
            next = NULL;
          else
            {
              next = el;
              TtaGiveNextElement (doc, &next, lastSelectedEl);
            }
          
          elType = TtaGetElementType (el);
          s = TtaGetSSchemaName (elType.ElSSchema);
          /* remove the current class attribute */
          attrType.AttrSSchema = elType.ElSSchema;
          if (!strcmp (s, "MathML"))
            attrType.AttrTypeNum = MathML_ATTR_class;
#ifdef _SVG
          else if (!strcmp (s, "SVG"))
            attrType.AttrTypeNum = SVG_ATTR_class;
#endif
          else
            {
              attrType.AttrSSchema = TtaGetSSchema ("HTML", doc);
              attrType.AttrTypeNum = HTML_ATTR_Class;
            }
          /* set the Class attribute of the element */
          attr = TtaGetAttribute (el, attrType);
          if (attr)
            {
              TtaRegisterAttributeDelete (attr, el, doc);
              TtaRemoveAttribute (el, attr, doc);
              TtaSetDocumentModified (doc);
            }
          DeleteSpanIfNoAttr (el, doc, &firstChild, &lastChild);
          if (firstChild)
            {
              if (el == firstSelectedEl)
                firstSelectedEl = firstChild;
              if (el == lastSelectedEl)
                lastSelectedEl = lastChild;
            }
          /* jump to the next element */
          el = next;
        }
      
      TtaCloseUndoSequence (doc);
      /* ask Thot to display changes made in the document */
      TtaSetDisplayMode (doc, dispMode);
      TtaSelectElement (doc, firstSelectedEl);
      if (lastSelectedEl != firstSelectedEl)
        TtaExtendSelection (doc, lastSelectedEl, 0);
    }
}

/*----------------------------------------------------------------------
  SpecificSettingsToCSS :  Callback for ApplyAllSpecificSettings,
  enrich the CSS string.
  ----------------------------------------------------------------------*/
static void SpecificSettingsToCSS (Element el, Document doc,
                                   PresentationSetting settings, void *param)
{
  LoadedImageDesc    *imgInfo;
  char               *css_rules = (char *)param;
  char                string[150];
  char               *ptr;

  string[0] = EOS;
  if (settings->type == PRBackgroundPicture)
    {
      /* transform absolute URL into relative URL */
      imgInfo = SearchLoadedImage ((char *)settings->value.pointer, 0);
      if (imgInfo != NULL)
        ptr = MakeRelativeURL (imgInfo->originalName, DocumentURLs[doc]);
      else
        ptr = MakeRelativeURL ((char *)settings->value.pointer, DocumentURLs[doc]);
      settings->value.pointer = ptr;
      TtaPToCss (settings, string, sizeof(string), el, NULL);
      TtaFreeMemory (ptr);
    }
  else
    TtaPToCss (settings, string, sizeof(string), el, NULL);

  if (string[0] != EOS && *css_rules != EOS)
    strcat (css_rules, "; ");
  if (string[0] != EOS)
    strcat (css_rules, string);
}


/*----------------------------------------------------------------------
  SetStyleString returns a string corresponding to the CSS
  description of the presentation attribute applied to an element.
  ----------------------------------------------------------------------*/
void SetStyleString (Document doc, Element el, PRule presRule)
{
  char               *buf;
  int                 len;
  int                 first, last;
#define STYLELEN 1000

  if (DocumentTypes[doc] == docCSS && el)
    {
      buf = (char *)TtaGetMemory (STYLELEN);
      buf[0] = EOS;
      TtaApplyAllSpecificSettings (el, doc, SpecificSettingsToCSS, &buf[0]);
      TtaRemovePRule (el,  presRule, doc);
      if (buf[0] != EOS)
        strcat (buf, "; ");
      len = strlen (buf);
      if (len)
        {
          // not necessary to open the undo sequence
          TtaGiveFirstSelectedElement (doc, &el, &first, &last);
          TtaRegisterElementReplace (el, doc);
          if (TtaIsSelectionEmpty ())
            {
              /* insert a new text */
              if (TtaGetElementVolume (el) == 0)
                TtaSetTextContent (el, (unsigned char *)buf, Latin_Script, doc);
              else
                {
                  TtaInsertTextContent (el, first, (unsigned char *)buf, doc);
                  len += first;
                }
            }
          else
            /* replace the current text */
            TtaSetTextContent (el, (unsigned char *)buf, Latin_Script, doc);
          TtaSelectString (doc, el, len, len-1);
        }
      TtaFreeMemory (buf);
    }
}

/*----------------------------------------------------------------------
  GetHTMLStyleString : return a string corresponding to the CSS
  description of the presentation attribute applied to an element.
  For stupid reasons, if the target element is HTML or BODY,
  one returns the concatenation of both element style strings.
  ----------------------------------------------------------------------*/
void GetHTMLStyleString (Element el, Document doc, char *buf, int *len)
{
  ElementType        elType;
  char              *name;

  if (buf == NULL || len == NULL || *len <= 0)
    return;

  /*
   * this will transform all the Specific Settings associated to
   * the element to one CSS string.
   */
  buf[0] = EOS;
  TtaApplyAllSpecificSettings (el, doc, SpecificSettingsToCSS, &buf[0]);
  *len = strlen (buf);

  /* BODY / HTML elements specific handling */
  elType = TtaGetElementType (el);
  name = TtaGetSSchemaName (elType.ElSSchema);
  if (strcmp(name, "HTML") == 0)
    {
      if (elType.ElTypeNum == HTML_EL_Document)
        {
          elType.ElTypeNum = HTML_EL_BODY;
          el = TtaSearchTypedElement (elType, SearchForward, el);
          if (!el)
            return;
          if (*len > 0)
            strcat(buf, ";");
          *len = strlen (buf);
          TtaApplyAllSpecificSettings (el, doc, SpecificSettingsToCSS,
                                       &buf[*len]);
          *len = strlen (buf);
        }
      else if (elType.ElTypeNum == HTML_EL_BODY ||
               elType.ElTypeNum == HTML_EL_HTML)
        {
          el = TtaGetMainRoot (doc);
          if (!el)
            return;
          if (*len > 0)
            strcat(buf, ";");
          *len = strlen (buf);
          TtaApplyAllSpecificSettings (el, doc, SpecificSettingsToCSS,
                                       &buf[*len]);
          *len = strlen (buf);
        }
    }
}

/*----------------------------------------------------------------------
  HTMLSetBackgroundImage:
  repeat = repeat value
  image = url of background image
  generate is TRUE when the style attribute must be generated.
  ----------------------------------------------------------------------*/
void HTMLSetBackgroundImage (Document doc, Element el, int repeat,
                             int specificity, char *image, ThotBool generate)
{
  Attribute           attr;
  AttributeType       attrType;
  ElementType         elType;
  char               *schName, *ptr;
  char                txt[400];
  int                 len;

  /* First remove a previous value */
  HTMLResetBackgroundImage (doc, el);
  if (el && generate)
    /* update the style attribute */
    SetStyleAttribute (doc, el);
  sprintf (txt, "background-image: url(%s); background-repeat: ", image);
  if (repeat == REPEAT)
    strcat (txt, "repeat");
  else if (repeat == XREPEAT)
    strcat (txt, "repeat-x");
  else if (repeat == YREPEAT)
    strcat (txt, "repeat-y");
  else
    strcat (txt, "no-repeat");
  ParseHTMLSpecificStyle (el, txt, doc, 2000, FALSE);
  if (el && generate)
    {
      elType = TtaGetElementType (el);
      attrType.AttrSSchema = elType.ElSSchema;
      schName = TtaGetSSchemaName (elType.ElSSchema);
      if (strcmp (schName, "MathML") == 0)
        attrType.AttrTypeNum = MathML_ATTR_style_;
      else if (!strcmp (schName, "HTML"))
        /* it's a HTML document */
        attrType.AttrTypeNum = HTML_ATTR_Style_;
#ifdef _SVG
      else if (!strcmp (schName, "SVG"))
        /* it's a SVG document */
        attrType.AttrTypeNum = SVG_ATTR_style_;
#endif /* _SVG */
      else
        return;
      attr = TtaGetAttribute (el, attrType);
      if (attr)
        {
          /* concatenate the old value and the new text */
          len = TtaGetTextAttributeLength (attr) + 1;
          ptr = (char *)TtaGetMemory (len + strlen (txt));
          TtaGiveTextAttributeValue (attr, ptr, &len);
          strcat (ptr, txt);
          TtaRegisterAttributeReplace (attr, el, doc);
          TtaSetAttributeText (attr, ptr, el, doc);
          TtaFreeMemory (ptr);
        }
      else
        {
          /* set the new text */
          attr = TtaNewAttribute (attrType);
          TtaAttachAttribute (el, attr, doc);
          TtaSetAttributeText (attr, txt, el, doc);
          TtaRegisterAttributeCreate (attr, el, doc);
        }
    }
}

/*----------------------------------------------------------------------
  UpdateClass
  Change or create a class attribute to reflect the Style attribute
  of the selected element.
  ----------------------------------------------------------------------*/
static void UpdateClass (Document doc)
{
  DisplayMode         dispMode;
  Attribute           attr;
  AttributeType       attrType, attrTypeT;
  Element             el, root, child, title, head, line, prev, styleEl;
  ElementType         elType, selType;
  char               *stylestring;
  char               *text, *ptr, *a_pseudo, *a_id;
  char               *a_class, *a_elem, *schName;
  int                 len, base, i;
  Language            lang;
  ThotBool            found, empty, insertNewLine, ok;

  elType = TtaGetElementType (ClassReference);
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = 0;
  attrTypeT.AttrSSchema = elType.ElSSchema;
  attrTypeT.AttrTypeNum = 0;
  // look for the selected element and class
  a_elem = CurrentClass;
  ptr = strstr (a_elem, " ");
  while (ptr)
    {
      a_elem = &ptr[1];
      ptr = strstr (a_elem, " ");
    }

  a_class = strstr (a_elem, ".");
  if (a_class != a_elem && a_class)
      *a_class = EOS;
  a_id = strstr (a_elem, "#");
  if (a_id)
      *a_id = EOS;
  a_pseudo = strstr (a_elem, ":");
  if (a_pseudo)
      *a_pseudo = EOS;
  GIType (a_elem, &selType, doc);
  if (a_class != a_elem && a_class)
      *a_class = '.';
  if (a_id)
      *a_id = '#';
  if (a_pseudo)
      *a_pseudo = ':';
  if (selType.ElTypeNum != elType.ElTypeNum && selType.ElTypeNum != 0)
    {
      ok = FALSE;
      if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML"))
        {
          if (selType.ElTypeNum == HTML_EL_Input)
            /* the user has chosen element imput */
            {
              if (elType.ElTypeNum == HTML_EL_Text_Input ||
                  elType.ElTypeNum == HTML_EL_Password_Input ||
                  elType.ElTypeNum == HTML_EL_File_Input ||
                  elType.ElTypeNum == HTML_EL_Checkbox_Input ||
                  elType.ElTypeNum == HTML_EL_Radio_Input ||
                  elType.ElTypeNum == HTML_EL_Submit_Input ||
                  elType.ElTypeNum == HTML_EL_Reset_Input ||
                  elType.ElTypeNum == HTML_EL_Button_Input)
                /* the selected element is a variant of the imput element. */
                ok = TRUE;
            }
          else if (selType.ElTypeNum == HTML_EL_ruby)
            /* the user has chosen element ruby */
            {
              if (elType.ElTypeNum == HTML_EL_simple_ruby ||
                  elType.ElTypeNum == HTML_EL_complex_ruby)
                /* the selected element is a variant of the ruby element. */
                ok = TRUE;
            }
        }
      if (!ok)
        /* it's an invalid element type */
        {
          TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_INVALID_TYPE), NULL);
          return;
        }
    }

  /* locate the style element in the document head */
  root = TtaGetMainRoot (doc);
  elType = TtaGetElementType (root);
  schName = TtaGetSSchemaName (elType.ElSSchema);
  head = NULL;
  if (!strcmp (schName, "HTML"))
    /* it's a HTML document */
    {
      elType.ElTypeNum = HTML_EL_HEAD;
      head = TtaSearchTypedElement (elType, SearchForward, root);
      elType.ElTypeNum = HTML_EL_STYLE_;
      attrType.AttrSSchema = elType.ElSSchema;
      attrType.AttrTypeNum = HTML_ATTR_Notation;
      // check also if it's a theme
      attrTypeT.AttrSSchema = elType.ElSSchema;
      attrTypeT.AttrTypeNum = HTML_ATTR_Title;
    }
#ifdef _SVG
  else if (!strcmp (schName, "SVG"))
    /* it's a SVG document */
    {
      elType.ElTypeNum = SVG_EL_SVG;
      head = TtaSearchTypedElement (elType, SearchForward, root);
      elType.ElTypeNum = SVG_EL_style__;
      attrType.AttrSSchema = elType.ElSSchema;
      attrType.AttrTypeNum = SVG_ATTR_type;
      // check also if it's a theme
      attrTypeT.AttrSSchema = elType.ElSSchema;
      attrTypeT.AttrTypeNum = SVG_ATTR_title_;
    }
#endif /* _SVG */
  el = head;
  found = FALSE;
  while (!found && el)
    {
      /* is there any style element? */
      el = TtaSearchTypedElementInTree (elType, SearchForward, head, el);
      if (el)
        {
          /* does this style element have an attribute type="text/css" ? */
          attr = TtaGetAttribute (el, attrType);
          if (attr)
            {
              len = TtaGetTextAttributeLength (attr);
              ptr = (char *)TtaGetMemory (len + 1);
              TtaGiveTextAttributeValue (attr, ptr, &len);
              found = (!strcmp (ptr, "text/css"));
              TtaFreeMemory (ptr);
              // check if it's a theme
              attr = TtaGetAttribute (el, attrTypeT);
              if (found && attr)
                {
                  len = TtaGetTextAttributeLength (attr);
                  ptr = (char *)TtaGetMemory (len + 1);
                  TtaGiveTextAttributeValue (attr, ptr, &len);
                  found = (strcmp (ptr, "Amaya theme") == 0);
                  TtaFreeMemory (ptr);
                }
            }
        }
    }
  if (!found && head)
    /* the STYLE element doesn't exist. Create it */
    {
      el = TtaNewTree (doc, elType, "");
      if (strcmp (schName, "HTML"))
        title = NULL;
      else
        {
          /* It's a HTML document. Insert the new style element after the
             title if it exists */
          elType.ElTypeNum = HTML_EL_TITLE;
          title = TtaSearchTypedElementInTree (elType, SearchForward, head,
                                               head);
        }
      
      /* do not check mandatory attributes */
      TtaSetStructureChecking (FALSE, doc);
      if (title)
        TtaInsertSibling (el, title, FALSE, doc);
      else
        TtaInsertFirstChild (&el, head, doc);
      TtaSetStructureChecking (TRUE, doc);
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (el, attr, doc);
      TtaSetAttributeText (attr, "text/css", el, doc);
    }

  if (!el)
    /* there is no STYLE element and no way to create one */
    return;

  styleEl = el;
  /* check whether it's an element type or a class name */
  /* get the current style attribute */
  elType = TtaGetElementType (ClassReference);
  schName = TtaGetSSchemaName (elType.ElSSchema);
  if (strcmp (schName, "MathML") == 0)
    {
      attrType.AttrSSchema = elType.ElSSchema;
      attrType.AttrTypeNum = MathML_ATTR_style_;
    }
#ifdef _SVG
  else if (strcmp (schName, "SVG") == 0)
    {
      attrType.AttrSSchema = elType.ElSSchema;
      attrType.AttrTypeNum = SVG_ATTR_style_;
    }
#endif /* _SVG */
  else
    {
      attrType.AttrSSchema = TtaGetSSchema ("HTML", doc);
      attrType.AttrTypeNum = HTML_ATTR_Style_;
    }
  attr = TtaGetAttribute (ClassReference, attrType);
  base = strlen (CurrentClass) + 2;
  if (attr)
    len = TtaGetTextAttributeLength (attr) + base + 3;
  else
    len = base + 3;
  /* create a string containing the new CSS definition. */
  stylestring = (char *)TtaGetMemory (len);
  stylestring[0] = EOS;
#ifdef IV
  if (selType.ElTypeNum == 0)
    {
      /* it's not an element type */
      if (CurrentClass[0] != '.' && CurrentClass[0] != '#')
        {
          /* it's an invalid class name, insert a dot */
          strcat (stylestring, ".");
          base++;
        }
    }
#endif
  strcat (stylestring, CurrentClass);
  strcat (stylestring, " {");
  if (attr)
    {
      len = len - base;
      TtaGiveTextAttributeValue (attr, &stylestring[base], &len);
    }
  strcat (stylestring, "}");

  TtaOpenUndoSequence (doc, ClassReference, ClassReference, 0, 0);
  /* create the class attribute */
  if (a_class)
    {
      a_class++;
      if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML"))
        {
          attrType.AttrSSchema = elType.ElSSchema;
          attrType.AttrTypeNum = MathML_ATTR_class;
        }
      else
#ifdef _SVG
        if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "SVG"))
          {
            attrType.AttrSSchema = elType.ElSSchema;
            attrType.AttrTypeNum = SVG_ATTR_class;
          }
        else
#endif
          {
            attrType.AttrSSchema = TtaGetSSchema ("HTML", doc);
            attrType.AttrTypeNum = HTML_ATTR_Class;
          }
      attr = TtaGetAttribute (ClassReference, attrType);
      if (!attr)
        {
          attr = TtaNewAttribute (attrType);
          TtaAttachAttribute (ClassReference, attr, doc);
          TtaSetAttributeText (attr, a_class, ClassReference, doc);
          TtaRegisterAttributeCreate (attr, ClassReference, doc);
        }
      else
        {
          TtaRegisterAttributeReplace (attr, ClassReference, doc);
          TtaSetAttributeText (attr, a_class, ClassReference, doc);
        }
      TtaSetDocumentModified (doc);
    }

  /* remove the Style attribute */
  RemoveElementStyle (ClassReference, doc, FALSE);

  insertNewLine = FALSE;
  child = TtaGetLastChild (el);
  if (child == NULL)
    {
      /* there is no TEXT element within the STYLE element. We create it now */
      elType.ElTypeNum = HTML_EL_TEXT_UNIT;
      child = TtaNewTree (doc, elType, "");
      TtaInsertFirstChild (&child, el, doc);
      /* remember the element to be registered in the Undo queue */
      el = child;
      found = FALSE;
    }
  else
    {
      elType = TtaGetElementType (child);
      if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
        /* if the last child of the STYLE element is an empty text leaf,
           skip it */
        {
          len = TtaGetTextLength (child) + 1;
          text = (char *)TtaGetMemory (len);
          TtaGiveTextContent (child, (unsigned char *)text, &len, &lang);
          empty = TRUE;
          insertNewLine = TRUE;
          for (i = len - 1; i >= 0 && empty; i--)
            {
              empty = text[i] <= SPACE;
              if ((int) text[i] == EOL || (int) text[i] == __CR__)
                insertNewLine = FALSE;
            }
          TtaFreeMemory (text);
          if (empty)
            {
              prev = child;
              TtaPreviousSibling (&prev);
              if (prev)
                {
                  child = prev;
                  elType = TtaGetElementType (child);
                }
            }
        }
      if (!strcmp (schName, "HTML") && elType.ElTypeNum != HTML_EL_TEXT_UNIT)
        {
          if (elType.ElTypeNum != HTML_EL_Comment_)
            /* the last child of the STYLE element is neither a text leaf nor
               a comment. Don't do anything */
            child = NULL;
          else
            /* the last child of the STYLE element is a comment */
            /* insert the new style rule within the Comment_line */
            {
              line = TtaGetLastChild (child);
              if (line)
                /* there is already a Comment_line */
                {
                  child = TtaGetLastChild (line);
                  len = TtaGetTextLength (child) + 1;
                  text = (char *)TtaGetMemory (len);
                  TtaGiveTextContent (child, (unsigned char *)text, &len, &lang);
                  empty = TRUE;
                  insertNewLine = TRUE;
                  for (i = len - 1; i >= 0 && empty; i--)
                    {
                      empty = text[i] <= SPACE;
                      if ((int) text[i] == EOL || (int) text[i] == __CR__)
                        insertNewLine = FALSE;
                    }
                  TtaFreeMemory (text);
                }
              else
                /* create a Comment_line within the Comment */
                {
                  elType.ElTypeNum = HTML_EL_Comment_line;
                  line = TtaNewTree (doc, elType, "");
                  TtaInsertFirstChild (&line, child, doc);
                  child = TtaGetLastChild (line);
                  insertNewLine = FALSE;
                  /* remember the element to register in the undo queue */
                  found = FALSE;
                  el = line;
                }
            }
        }
    }

  if (child)
    {
      if (found)
        /* Register the previous value of the STYLE element in the Undo queue */
        TtaRegisterElementReplace (child, doc);
      /* update the STYLE element */
      len = TtaGetTextLength (child);
      if (insertNewLine)
        {
          TtaInsertTextContent (child, len, (unsigned char *)"\n", doc);
          len++;
        }
      TtaInsertTextContent (child, len, (unsigned char *)stylestring, doc);

      /* parse and apply this new CSS to the current document */
      dispMode = TtaGetDisplayMode (doc);
      TtaSetDisplayMode (doc, NoComputedDisplay);
      ptr = (char *)TtaConvertMbsToByte ((unsigned char *)stylestring, TtaGetLocaleCharset ());
      ReadCSSRules (doc, NULL, ptr, NULL,
                    TtaGetElementLineNumber (child), TRUE, styleEl);
      TtaFreeMemory (ptr);
      TtaSetDisplayMode (doc, dispMode);
    }
  /* free the stylestring now */
  TtaFreeMemory (stylestring);
  stylestring = NULL;
  
  if (!found && el)
    /* Register the created STYLE or child element in the Undo queue */
    TtaRegisterElementCreate (el, doc);
  TtaCloseUndoSequence (doc);
  TtaSelectElement (doc, ClassReference);
}

/*----------------------------------------------------------------------
  PutClassName adds the class names into the buffer buff if they are
  not already there.
  Class names are contained in attribute attr or in parameter className.
  ----------------------------------------------------------------------*/
static void PutClassName (Attribute attr, char *className, char *buf,
                          int *index, int *free, int *nb)
{
  int		         len, cur, i, k;
  unsigned char  selector[200];
  char          *ptr, *name, c;
  ThotBool       found, previous;

  if (attr)
    {
      len = 198;
      TtaGiveTextAttributeValue (attr, (char *)selector, &len);
      selector[len+1] = EOS;
      /* get the first name contained in the attribute */
      ptr = (char *)selector;
      ptr = (char*)TtaSkipBlanks (ptr);
    }
  else
    ptr = className;

  while (ptr && *ptr != EOS)
    {
      name = ptr;
      /* look for the end of the current name */
      while (*ptr > ' ' && *ptr != EOS)
        ptr++;
      c = *ptr;
      *ptr = EOS;
      /* compare that name with all class names already known */
      cur = 0;
      found = FALSE;
      previous = FALSE;
      for (i = 0; i < *nb && !found && !previous; i++)
        {
          if (buf[cur] == '.')
            cur++;
          len = strlen (&buf[cur]) + 1;
          found = !strcmp (name, &buf[cur]);
          previous = (cur != 0 && name[0] < buf[cur]);
          if (!found && !previous && cur != 0)
            {
              k = 0;
              while (name[k] == buf[cur+k])
                k++;
              previous = (name[k] < buf[cur+ k]);
            }
          if (!previous)
            cur += len;
        }

      if (!found)
        /* this class name is not known, append it */
        {
          len = strlen (name);
          len++; /* add the \0 */
          if (len >= *free)
            return;
          if (previous && i > 1 && i  <= *nb)
            {
              // move the tail of the current list
              cur--;
              len++;
              for (k = *index; k >= cur; k--)
                buf[k+len] = buf[k];
            /* add this new class name with a dot at the current position */
              buf[cur++] = '.';
              strcpy (&buf[cur], name);
            }
          else
            {
            /* add this new class name with a dot at the end */
              buf[(*index)++] = '.';
              strcpy (&buf[*index], name);
	      *free -= 1;
            }
          *free -= len;
          *index += len;
          (*nb)++;
        }
      /* skip spaces after the name that has just been processed */
      if (c != EOS)
        {
          *ptr = c; // restore the character
          ptr++;
          ptr = (char*)TtaSkipBlanks (ptr);
        }
      /* and process the next name, if any */
    }
}

/*----------------------------------------------------------------------
  BuildClassList
  Build the whole list of class names after the first name.
  ----------------------------------------------------------------------*/
static int BuildClassList (Document doc, char *buf, int size, const char *first)
{
  Element             el;
  ElementType         elType;
  Attribute           attr;
  AttributeType       attrType;
  CSSInfoPtr          css;
  int                 free;
  int                 len, nb;
  int                 index;

  /* add the first element if specified */
  memset (buf, 0, size);
  nb = 0;
  index = 0;
  free = size;
  if (first)
    {
      strcpy (&buf[index], first);
      len = strlen (first);
      len++;
      free -= len;
      index += len;
      nb++;
    }
  if (DocumentTypes[doc] == docHTML)
    {
      /* looks for the class attribute defined in the HTML DTD */
      el = TtaGetMainRoot (doc);
      elType = TtaGetElementType (el);
      attrType.AttrSSchema = elType.ElSSchema;
      elType.ElTypeNum = HTML_EL_BODY;
      el = TtaSearchTypedElement (elType, SearchInTree, el);
      if (attrType.AttrSSchema)
        {
          /* this document contains HTML elements */
          attrType.AttrTypeNum = HTML_ATTR_Class;
          while (el)
            {
              TtaSearchAttribute (attrType, SearchForward, el, &el, &attr);
              if (attr)
                PutClassName (attr, NULL, buf, &index, &free, &nb);
            }
        }
    }

  if (DocumentTypes[doc] == docMath ||
      (DocumentTypes[doc] == docHTML &&
       DocumentMeta[doc] && DocumentMeta[doc]->compound))
    {
      /* looks for the class attribute defined in the MathML DTD */
      attrType.AttrSSchema = TtaGetSSchema ("MathML", doc);
      if (attrType.AttrSSchema)
        /* there are some MathML elements in this document */
        {
          attrType.AttrTypeNum = MathML_ATTR_class;
          el = TtaGetMainRoot (doc);
          while (el)
            {
              TtaSearchAttribute (attrType, SearchForward, el, &el, &attr);
              if (attr)
                PutClassName (attr, NULL, buf, &index, &free, &nb);
            }
        }
    }

#ifdef _SVG
 if (DocumentTypes[doc] == docSVG ||
      (DocumentTypes[doc] == docHTML &&
       DocumentMeta[doc] && DocumentMeta[doc]->compound))
    {
      /* looks for the class attribute defined in the SVG DTD */
      attrType.AttrSSchema = TtaGetSSchema ("SVG", doc);
      if (attrType.AttrSSchema)
        /* there are some SVG elements in this document */
        {
          attrType.AttrTypeNum = SVG_ATTR_class;
          el = TtaGetMainRoot (doc);
          while (el)
            {
              TtaSearchAttribute (attrType, SearchForward, el, &el, &attr);
              if (attr)
                PutClassName (attr, NULL, buf, &index, &free, &nb);
            }
        }
    }
#endif /* _SVG */

  /* look for all class names that are used in the STYLE element and in
     all style sheets currently associated with the document */
  css = CSSList;
  while (css)
    {
      if (css->class_list && css->infos [doc])
        {
          //printf ("%s\n LIST=\n", css->class_list, ListBuffer);
          PutClassName (NULL, css->class_list, buf, &index, &free, &nb);
        }
      css = css->NextCSS;
    }
  return (nb);
}
	    
/*----------------------------------------------------------------------
  CreateClass
  creates a class or element rule from the Style attribute of the selected element
  ----------------------------------------------------------------------*/
void CreateClass (Document doc, View view)
{
  Attribute           attr;
  AttributeType       attrType;
  Element             last_elem;
  ElementType         elType;
  char                a_class[50];
  const char         *elHtmlName;
  char               *schName;
  int                 len, i, j;
  int                 firstSelectedChar, lastSelectedChar;

  if (!TtaGetDocumentAccessMode (doc))
    /* the document is in ReadOnly mode */
    return;

  DocReference = doc;
  CurrentClass[0] = 0;
  ClassReference = NULL;
  TtaGiveFirstSelectedElement (doc, &ClassReference,
                               &firstSelectedChar, &lastSelectedChar);
  TtaGiveLastSelectedElement (doc, &last_elem, &i, &j);
  
  /* one can only define a style from one element at a time. */
  if (last_elem != ClassReference)
    return;
  if (ClassReference == NULL)
    return;
  /* if the selected element is read-only, do nothing */
  if (TtaIsReadOnly (ClassReference))
    return;

  /* if only a part of an element is selected, select the parent instead */
  elType = TtaGetElementType (ClassReference);
  if (elType.ElTypeNum == HTML_EL_TEXT_UNIT ||
      elType.ElTypeNum == HTML_EL_GRAPHICS_UNIT)
    {
      ClassReference = TtaGetParent (ClassReference);
      elType = TtaGetElementType (ClassReference);
    }
  if (elType.ElSSchema)
    {
      schName = TtaGetSSchemaName (elType.ElSSchema);
      if (strcmp (schName, "HTML") &&
          strcmp (schName, "SVG") &&
          strcmp (schName, "MathML"))
        /* no class attribute for that element. Do nothing */
        return;
      /* check if the element has a style attribute */
      attrType.AttrSSchema = elType.ElSSchema;
      if (strcmp (schName, "MathML") == 0)
        attrType.AttrTypeNum = MathML_ATTR_style_;
#ifdef _SVG
      else if (strcmp (schName, "SVG") == 0)
        attrType.AttrTypeNum = SVG_ATTR_style_;
#endif
      else
        attrType.AttrTypeNum = HTML_ATTR_Style_;
      attr = TtaGetAttribute (ClassReference, attrType);
      if (attr == NULL)
        {
          /* no attribute style */
          TtaDisplaySimpleMessage (CONFIRM, AMAYA, AM_NO_STYLE_FOR_ELEM);
          return;
        }

      /* update the class name selector. */
      if (!strcmp (schName, "HTML") && elType.ElTypeNum == HTML_EL_Preformatted)
        // avoid to convert a pre to listing
        elHtmlName = "pre";
      else
      elHtmlName =  GetXMLElementName (elType, doc);
      if (elHtmlName[0] == '?')
        InitConfirm (doc, 1, TtaGetMessage (LIB, TMSG_SEL_CLASS));
      else
        {
          NbClass = BuildClassList (doc, ListBuffer, MAX_CSS_LENGTH, elHtmlName);  
          /* preselect the entry corresponding to the class of the element. */
          if (!strcmp (schName, "MathML"))
            attrType.AttrTypeNum = MathML_ATTR_class;
#ifdef _SVG
          else if (!strcmp (schName, "SVG"))
            attrType.AttrTypeNum = SVG_ATTR_class;
#endif /* _SVG */
          else
            attrType.AttrTypeNum = HTML_ATTR_Class;
          attr = TtaGetAttribute (ClassReference, attrType);
          if (attr)
            {
              len = 50;
              TtaGiveTextAttributeValue (attr, a_class, &len);
              strcpy (CurrentClass, a_class);
            }
          else
            {
              strcpy (CurrentClass, elHtmlName);
            }
  
          CreateListEditDlgWX( BaseDialog+ClassForm,
                               TtaGetViewFrame(doc, 1),
                               TtaGetMessage(AMAYA, AM_DEF_CLASS),
                               TtaGetMessage(LIB, TMSG_SEL_CLASS),
                               NbClass,
                               ListBuffer,
                               CurrentClass );

          /* pop-up the dialogue box. */
          TtaSetDialoguePosition ();
          TtaShowDialogue (BaseDialog + ClassForm, TRUE, TRUE);
        }
    }
}

/*----------------------------------------------------------------------
  ApplyClass
  Initialize and activate the Apply Class dialogue box.
  ----------------------------------------------------------------------*/
void ApplyClass (Document doc, View view)
{
  Attribute           attr = NULL;
  AttributeType       attrType;
  Element             el, ancestor;
  ElementType	        elType;
#ifdef _WX
  AmayaParams         p;
  char                a_class_with_dot[51];
#endif /* _WX */
  char                a_class[50], *name;
  int                 len;
  int                 firstSelectedChar, lastSelectedChar;

  TtaGiveFirstSelectedElement (doc, &el, &firstSelectedChar, &lastSelectedChar);
  if (DocumentURLs[doc] == NULL)
    return;
  if (el)
    {
      /* if the selected element is read-only, do nothing */
      if (TtaIsReadOnly (el))
        return;
    }
  else
    el = TtaGetRootElement (doc);

  elType = TtaGetElementType (el);
  CurrentClass[0] = EOS;
  ApplyClassDoc = doc;
  name =  TtaGetSSchemaName (elType.ElSSchema);
  if ((strcmp (name, "HTML") && strcmp (name, "MathML") && strcmp (name, "SVG")) ||
      !TtaGetDocumentAccessMode (doc))
    {
#ifdef _WX 
      AmayaParams p;
      p.param1 = 0;
      p.param2 = NULL;
      p.param3 = NULL;
      p.param4 = (void*)(BaseDialog+AClassForm); /* the dialog reference used to call the right callback in thotlib */
      TtaSendDataToPanel( WXAMAYA_PANEL_APPLYCLASS, p );
#endif /* _WX */
      return;
    }

  /* updating the class name selector. */
  NbClass = BuildClassList (doc, ListBuffer, MAX_CSS_LENGTH, "(no_class)");
  if (el)
    {
      /* preselect the entry corresponding to the class of the first selected
         element. */
      if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML"))
        {
          attrType.AttrSSchema = elType.ElSSchema;
          attrType.AttrTypeNum = MathML_ATTR_class;
        }
#ifdef _SVG
      else if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "SVG"))
        {
          attrType.AttrSSchema = elType.ElSSchema;
          attrType.AttrTypeNum = SVG_ATTR_class;
        }
#endif /* _SVG */
      else
        {
          attrType.AttrSSchema = TtaGetSSchema ("HTML", doc);
          attrType.AttrTypeNum = HTML_ATTR_Class;
        }
      ancestor = el;
      do
        {
          attr = TtaGetAttribute (ancestor, attrType);
          if (!attr)
            ancestor = TtaGetParent (ancestor);
        }
      while (!attr && ancestor);
    }

  if (attr)
    {
      len = 50;
      TtaGiveTextAttributeValue (attr, a_class, &len);
#ifdef _WX
      a_class_with_dot[0] = EOS;
      strcat(a_class_with_dot, ".");
      strcat(a_class_with_dot, a_class);      
      strcpy (CurrentClass, a_class_with_dot);
#else /* _WX */
      strcpy (CurrentClass, a_class);
#endif /* _WX */
    }
  else
    strcpy (CurrentClass, "(no_class)");

  /* pop-up the dialogue box. */
#ifdef _WX  
  p.param1 = NbClass;
  p.param2 = (void*)ListBuffer;
  p.param3 = (void*)CurrentClass;
  p.param4 = (void*)(BaseDialog+AClassForm); /* the dialog reference used to call the right callback in thotlib */
  TtaSendDataToPanel( WXAMAYA_PANEL_APPLYCLASS, p );
#endif /* _WX */
}

/*----------------------------------------------------------------------
  StyleCallbackDialogue : procedure for style dialogue events        
  ----------------------------------------------------------------------*/
void StyleCallbackDialogue (int ref, int typedata, char  *data)
{
  long int          val;

  val = (long int) data;
#ifdef DEBUG_CLASS_INTERF
  if (typedata == INTEGER_DATA)
    fprintf (stderr, "StyleCallbackDialogue(%d,%d) \n", ref, (int) data);
  else if (typedata == STRING_DATA)
    fprintf (stderr, "StyleCallbackDialogue(%d,\"%s\") \n", ref, (char*) data);
#endif

  switch (ref - BaseDialog)
    {
    case ClassForm:
      if (val == 1)
        UpdateClass (DocReference);
      TtaDestroyDialogue (BaseDialog + ClassForm);
      break;
    case ClassSelect:
      strcpy (CurrentClass, data);
      break;
    case AClassSelect:
      strcpy (CurrentClass, data);
      break;
    case AClassForm:
      if (val == 1)
        DoApplyClass (ApplyClassDoc);
      else
        TtaDestroyDialogue (BaseDialog + AClassForm);
      break;
    default:
      break;
    }
}
